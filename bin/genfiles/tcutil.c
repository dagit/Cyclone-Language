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
Cyc___cycFILE*)Cyc_stderr);{const char*_tmp106C;void*_tmp106B[2];const char*
_tmp106A;const char*_tmp1069;struct Cyc_String_pa_struct _tmp1068;struct Cyc_String_pa_struct
_tmp1067;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmp1067.tag=0,((
_tmp1067.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure)),((_tmp1068.tag=0,((_tmp1068.f1=(struct _dyneither_ptr)(
Cyc_Tcutil_tq1_const?(_tmp1069="const ",_tag_dyneither(_tmp1069,sizeof(char),7)):((
_tmp106A="",_tag_dyneither(_tmp106A,sizeof(char),1)))),((_tmp106B[0]=& _tmp1068,((
_tmp106B[1]=& _tmp1067,Cyc_aprintf(((_tmp106C="%s%s",_tag_dyneither(_tmp106C,
sizeof(char),5))),_tag_dyneither(_tmp106B,sizeof(void*),2))))))))))))));const
char*_tmp1075;void*_tmp1074[2];const char*_tmp1073;const char*_tmp1072;struct Cyc_String_pa_struct
_tmp1071;struct Cyc_String_pa_struct _tmp1070;struct _dyneither_ptr s2=(struct
_dyneither_ptr)((_tmp1070.tag=0,((_tmp1070.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((_tmp1071.tag=0,((
_tmp1071.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?(_tmp1072="const ",
_tag_dyneither(_tmp1072,sizeof(char),7)):((_tmp1073="",_tag_dyneither(_tmp1073,
sizeof(char),1)))),((_tmp1074[0]=& _tmp1071,((_tmp1074[1]=& _tmp1070,Cyc_aprintf(((
_tmp1075="%s%s",_tag_dyneither(_tmp1075,sizeof(char),5))),_tag_dyneither(
_tmp1074,sizeof(void*),2))))))))))))));int pos=2;{const char*_tmp1079;void*
_tmp1078[1];struct Cyc_String_pa_struct _tmp1077;(_tmp1077.tag=0,((_tmp1077.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp1078[0]=& _tmp1077,Cyc_fprintf(
Cyc_stderr,((_tmp1079="  %s",_tag_dyneither(_tmp1079,sizeof(char),5))),
_tag_dyneither(_tmp1078,sizeof(void*),1)))))));}pos +=_get_dyneither_size(s1,
sizeof(char));if(pos + 5 >= 80){{const char*_tmp107C;void*_tmp107B;(_tmp107B=0,Cyc_fprintf(
Cyc_stderr,((_tmp107C="\n\t",_tag_dyneither(_tmp107C,sizeof(char),3))),
_tag_dyneither(_tmp107B,sizeof(void*),0)));}pos=8;}else{{const char*_tmp107F;void*
_tmp107E;(_tmp107E=0,Cyc_fprintf(Cyc_stderr,((_tmp107F=" ",_tag_dyneither(
_tmp107F,sizeof(char),2))),_tag_dyneither(_tmp107E,sizeof(void*),0)));}++ pos;}{
const char*_tmp1082;void*_tmp1081;(_tmp1081=0,Cyc_fprintf(Cyc_stderr,((_tmp1082="and ",
_tag_dyneither(_tmp1082,sizeof(char),5))),_tag_dyneither(_tmp1081,sizeof(void*),
0)));}pos +=4;if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){{const char*
_tmp1085;void*_tmp1084;(_tmp1084=0,Cyc_fprintf(Cyc_stderr,((_tmp1085="\n\t",
_tag_dyneither(_tmp1085,sizeof(char),3))),_tag_dyneither(_tmp1084,sizeof(void*),
0)));}pos=8;}{const char*_tmp1089;void*_tmp1088[1];struct Cyc_String_pa_struct
_tmp1087;(_tmp1087.tag=0,((_tmp1087.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s2),((_tmp1088[0]=& _tmp1087,Cyc_fprintf(Cyc_stderr,((_tmp1089="%s ",
_tag_dyneither(_tmp1089,sizeof(char),4))),_tag_dyneither(_tmp1088,sizeof(void*),
1)))))));}pos +=_get_dyneither_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){{const
char*_tmp108C;void*_tmp108B;(_tmp108B=0,Cyc_fprintf(Cyc_stderr,((_tmp108C="\n\t",
_tag_dyneither(_tmp108C,sizeof(char),3))),_tag_dyneither(_tmp108B,sizeof(void*),
0)));}pos=8;}{const char*_tmp108F;void*_tmp108E;(_tmp108E=0,Cyc_fprintf(Cyc_stderr,((
_tmp108F="are not compatible. ",_tag_dyneither(_tmp108F,sizeof(char),21))),
_tag_dyneither(_tmp108E,sizeof(void*),0)));}pos +=17;if((char*)Cyc_Tcutil_failure_reason.curr
!= (char*)(_tag_dyneither(0,0,0)).curr){if(pos + Cyc_strlen((struct _dyneither_ptr)
Cyc_Tcutil_failure_reason)>= 80){const char*_tmp1092;void*_tmp1091;(_tmp1091=0,
Cyc_fprintf(Cyc_stderr,((_tmp1092="\n\t",_tag_dyneither(_tmp1092,sizeof(char),3))),
_tag_dyneither(_tmp1091,sizeof(void*),0)));}{const char*_tmp1096;void*_tmp1095[1];
struct Cyc_String_pa_struct _tmp1094;(_tmp1094.tag=0,((_tmp1094.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmp1095[0]=&
_tmp1094,Cyc_fprintf(Cyc_stderr,((_tmp1096="%s",_tag_dyneither(_tmp1096,sizeof(
char),3))),_tag_dyneither(_tmp1095,sizeof(void*),1)))))));};}{const char*_tmp1099;
void*_tmp1098;(_tmp1098=0,Cyc_fprintf(Cyc_stderr,((_tmp1099="\n",_tag_dyneither(
_tmp1099,sizeof(char),2))),_tag_dyneither(_tmp1098,sizeof(void*),0)));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);};}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Position_post_error(Cyc_Position_mk_err_elab(
loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct _dyneither_ptr msg=(struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{const char*_tmp109D;
void*_tmp109C[1];struct Cyc_String_pa_struct _tmp109B;(_tmp109B.tag=0,((_tmp109B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp109C[0]=& _tmp109B,Cyc_fprintf(
Cyc_stderr,((_tmp109D="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(
_tmp109D,sizeof(char),36))),_tag_dyneither(_tmp109C,sizeof(void*),1)))))));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Impossible_struct _tmp10A0;
struct Cyc_Core_Impossible_struct*_tmp109F;(int)_throw((void*)((_tmp109F=
_cycalloc(sizeof(*_tmp109F)),((_tmp109F[0]=((_tmp10A0.tag=Cyc_Core_Impossible,((
_tmp10A0.f1=msg,_tmp10A0)))),_tmp109F)))));};}static struct _dyneither_ptr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs){for(0;tvs != 0;tvs=tvs->tl){const char*_tmp10A5;void*_tmp10A4[2];struct Cyc_String_pa_struct
_tmp10A3;struct Cyc_String_pa_struct _tmp10A2;(_tmp10A2.tag=0,((_tmp10A2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(((
struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmp10A3.tag=0,((_tmp10A3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
tvs->hd)),((_tmp10A4[0]=& _tmp10A3,((_tmp10A4[1]=& _tmp10A2,Cyc_fprintf(Cyc_stderr,((
_tmp10A5="%s::%s ",_tag_dyneither(_tmp10A5,sizeof(char),8))),_tag_dyneither(
_tmp10A4,sizeof(void*),2)))))))))))));}{const char*_tmp10A8;void*_tmp10A7;(
_tmp10A7=0,Cyc_fprintf(Cyc_stderr,((_tmp10A8="\n",_tag_dyneither(_tmp10A8,
sizeof(char),2))),_tag_dyneither(_tmp10A7,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct
_dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{
struct Cyc_List_List*_tmp10A9;Cyc_Tcutil_warning_segs=((_tmp10A9=_cycalloc(
sizeof(*_tmp10A9)),((_tmp10A9->hd=sg,((_tmp10A9->tl=Cyc_Tcutil_warning_segs,
_tmp10A9))))));}{struct _dyneither_ptr*_tmp10AC;struct Cyc_List_List*_tmp10AB;Cyc_Tcutil_warning_msgs=((
_tmp10AB=_cycalloc(sizeof(*_tmp10AB)),((_tmp10AB->hd=((_tmp10AC=_cycalloc(
sizeof(*_tmp10AC)),((_tmp10AC[0]=msg,_tmp10AC)))),((_tmp10AB->tl=Cyc_Tcutil_warning_msgs,
_tmp10AB))))));};}void Cyc_Tcutil_flush_warnings(){if(Cyc_Tcutil_warning_segs == 0)
return;{const char*_tmp10AF;void*_tmp10AE;(_tmp10AE=0,Cyc_fprintf(Cyc_stderr,((
_tmp10AF="***Warnings***\n",_tag_dyneither(_tmp10AF,sizeof(char),16))),
_tag_dyneither(_tmp10AE,sizeof(void*),0)));}{struct Cyc_List_List*_tmp36=Cyc_Position_strings_of_segments(
Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){{const char*_tmp10B4;void*_tmp10B3[2];struct
Cyc_String_pa_struct _tmp10B2;struct Cyc_String_pa_struct _tmp10B1;(_tmp10B1.tag=0,((
_tmp10B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((
struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmp10B2.tag=
0,((_tmp10B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((_tmp10B3[0]=&
_tmp10B2,((_tmp10B3[1]=& _tmp10B1,Cyc_fprintf(Cyc_stderr,((_tmp10B4="%s: %s\n",
_tag_dyneither(_tmp10B4,sizeof(char),8))),_tag_dyneither(_tmp10B3,sizeof(void*),
2)))))))))))));}_tmp36=_tmp36->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)
_check_null(Cyc_Tcutil_warning_msgs))->tl;}{const char*_tmp10B7;void*_tmp10B6;(
_tmp10B6=0,Cyc_fprintf(Cyc_stderr,((_tmp10B7="**************\n",_tag_dyneither(
_tmp10B7,sizeof(char),16))),_tag_dyneither(_tmp10B6,sizeof(void*),0)));}Cyc_fflush((
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
if(t2 != *((void**)_check_null(*_tmp44))){void**_tmp10B8;*_tmp44=((_tmp10B8=
_cycalloc(sizeof(*_tmp10B8)),((_tmp10B8[0]=t2,_tmp10B8))));}return t2;}_LL7: {
struct Cyc_Absyn_Evar_struct*_tmp45=(struct Cyc_Absyn_Evar_struct*)_tmp3D;if(
_tmp45->tag != 1)goto _LL9;else{_tmp46=_tmp45->f2;_tmp47=(struct Cyc_Core_Opt**)&
_tmp45->f2;}}_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp47))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp47))->v){struct Cyc_Core_Opt*_tmp10B9;*_tmp47=((_tmp10B9=_cycalloc(sizeof(*
_tmp10B9)),((_tmp10B9->v=t2,_tmp10B9))));}return t2;}_LL9: {struct Cyc_Absyn_ValueofType_struct*
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
struct Cyc_Absyn_Unknown_kb_struct _tmp10BC;struct Cyc_Absyn_Unknown_kb_struct*
_tmp10BB;return(void*)((_tmp10BB=_cycalloc(sizeof(*_tmp10BB)),((_tmp10BB[0]=((
_tmp10BC.tag=1,((_tmp10BC.f1=0,_tmp10BC)))),_tmp10BB))));}_LL1E: {struct Cyc_Absyn_Less_kb_struct*
_tmp5B=(struct Cyc_Absyn_Less_kb_struct*)_tmp59;if(_tmp5B->tag != 2)goto _LL20;
else{_tmp5C=_tmp5B->f2;}}_LL1F: {struct Cyc_Absyn_Less_kb_struct _tmp10BF;struct
Cyc_Absyn_Less_kb_struct*_tmp10BE;return(void*)((_tmp10BE=_cycalloc(sizeof(*
_tmp10BE)),((_tmp10BE[0]=((_tmp10BF.tag=2,((_tmp10BF.f1=0,((_tmp10BF.f2=_tmp5C,
_tmp10BF)))))),_tmp10BE))));}_LL20:;_LL21: return kb;_LL1B:;}static struct Cyc_Absyn_Tvar*
Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_tmp10C0;
return(_tmp10C0=_cycalloc(sizeof(*_tmp10C0)),((_tmp10C0->name=tv->name,((
_tmp10C0->identity=- 1,((_tmp10C0->kind=Cyc_Tcutil_copy_kindbound(tv->kind),
_tmp10C0)))))));}static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
struct _tuple9 _tmp63;struct Cyc_Core_Opt*_tmp64;struct Cyc_Absyn_Tqual _tmp65;void*
_tmp66;struct _tuple9*_tmp62=arg;_tmp63=*_tmp62;_tmp64=_tmp63.f1;_tmp65=_tmp63.f2;
_tmp66=_tmp63.f3;{struct _tuple9*_tmp10C1;return(_tmp10C1=_cycalloc(sizeof(*
_tmp10C1)),((_tmp10C1->f1=_tmp64,((_tmp10C1->f2=_tmp65,((_tmp10C1->f3=Cyc_Tcutil_copy_type(
_tmp66),_tmp10C1)))))));};}static struct _tuple11*Cyc_Tcutil_copy_tqt(struct
_tuple11*arg){struct _tuple11 _tmp69;struct Cyc_Absyn_Tqual _tmp6A;void*_tmp6B;
struct _tuple11*_tmp68=arg;_tmp69=*_tmp68;_tmp6A=_tmp69.f1;_tmp6B=_tmp69.f2;{
struct _tuple11*_tmp10C2;return(_tmp10C2=_cycalloc(sizeof(*_tmp10C2)),((_tmp10C2->f1=
_tmp6A,((_tmp10C2->f2=Cyc_Tcutil_copy_type(_tmp6B),_tmp10C2)))));};}static struct
Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmp10C3;return(_tmp10C3=_cycalloc(sizeof(*_tmp10C3)),((_tmp10C3->name=f->name,((
_tmp10C3->tq=f->tq,((_tmp10C3->type=Cyc_Tcutil_copy_type(f->type),((_tmp10C3->width=
f->width,((_tmp10C3->attributes=f->attributes,_tmp10C3)))))))))));}static struct
_tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){struct _tuple0 _tmp6F;void*_tmp70;
void*_tmp71;struct _tuple0*_tmp6E=x;_tmp6F=*_tmp6E;_tmp70=_tmp6F.f1;_tmp71=_tmp6F.f2;{
struct _tuple0*_tmp10C4;return(_tmp10C4=_cycalloc(sizeof(*_tmp10C4)),((_tmp10C4->f1=
Cyc_Tcutil_copy_type(_tmp70),((_tmp10C4->f2=Cyc_Tcutil_copy_type(_tmp71),
_tmp10C4)))));};}static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f){struct Cyc_Absyn_Enumfield*_tmp10C5;return(_tmp10C5=
_cycalloc(sizeof(*_tmp10C5)),((_tmp10C5->name=f->name,((_tmp10C5->tag=f->tag,((
_tmp10C5->loc=f->loc,_tmp10C5)))))));}static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*
t){struct Cyc_Absyn_VarType_struct _tmp10C8;struct Cyc_Absyn_VarType_struct*
_tmp10C7;return(void*)((_tmp10C7=_cycalloc(sizeof(*_tmp10C7)),((_tmp10C7[0]=((
_tmp10C8.tag=2,((_tmp10C8.f1=Cyc_Tcutil_copy_tvar(t),_tmp10C8)))),_tmp10C7))));}
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
_tmpC3;struct Cyc_List_List*_tmpC4;struct Cyc_Absyn_Typedefdecl*_tmpC5;void*_tmpC9;
struct Cyc_List_List*_tmpCB;void*_tmpCD;struct Cyc_Absyn_TypeDecl*_tmpCF;struct Cyc_Absyn_TypeDecl
_tmpD0;void*_tmpD1;struct Cyc_Absyn_Aggrdecl*_tmpD3;struct Cyc_Absyn_TypeDecl*
_tmpD5;struct Cyc_Absyn_TypeDecl _tmpD6;void*_tmpD7;struct Cyc_Absyn_Enumdecl*
_tmpD9;struct Cyc_Absyn_TypeDecl*_tmpDB;struct Cyc_Absyn_TypeDecl _tmpDC;void*
_tmpDD;struct Cyc_Absyn_Datatypedecl*_tmpDF;_LL23: {struct Cyc_Absyn_VoidType_struct*
_tmp77=(struct Cyc_Absyn_VoidType_struct*)_tmp76;if(_tmp77->tag != 0)goto _LL25;}
_LL24: goto _LL26;_LL25: {struct Cyc_Absyn_Evar_struct*_tmp78=(struct Cyc_Absyn_Evar_struct*)
_tmp76;if(_tmp78->tag != 1)goto _LL27;}_LL26: return t;_LL27: {struct Cyc_Absyn_VarType_struct*
_tmp79=(struct Cyc_Absyn_VarType_struct*)_tmp76;if(_tmp79->tag != 2)goto _LL29;
else{_tmp7A=_tmp79->f1;}}_LL28: {struct Cyc_Absyn_VarType_struct _tmp10CB;struct
Cyc_Absyn_VarType_struct*_tmp10CA;return(void*)((_tmp10CA=_cycalloc(sizeof(*
_tmp10CA)),((_tmp10CA[0]=((_tmp10CB.tag=2,((_tmp10CB.f1=Cyc_Tcutil_copy_tvar(
_tmp7A),_tmp10CB)))),_tmp10CA))));}_LL29: {struct Cyc_Absyn_DatatypeType_struct*
_tmp7B=(struct Cyc_Absyn_DatatypeType_struct*)_tmp76;if(_tmp7B->tag != 3)goto _LL2B;
else{_tmp7C=_tmp7B->f1;_tmp7D=_tmp7C.datatype_info;_tmp7E=_tmp7C.targs;}}_LL2A: {
struct Cyc_Absyn_DatatypeType_struct _tmp10D1;struct Cyc_Absyn_DatatypeInfo _tmp10D0;
struct Cyc_Absyn_DatatypeType_struct*_tmp10CF;return(void*)((_tmp10CF=_cycalloc(
sizeof(*_tmp10CF)),((_tmp10CF[0]=((_tmp10D1.tag=3,((_tmp10D1.f1=((_tmp10D0.datatype_info=
_tmp7D,((_tmp10D0.targs=Cyc_Tcutil_copy_types(_tmp7E),_tmp10D0)))),_tmp10D1)))),
_tmp10CF))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp7F=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp76;if(_tmp7F->tag != 4)goto _LL2D;else{_tmp80=_tmp7F->f1;_tmp81=_tmp80.field_info;
_tmp82=_tmp80.targs;}}_LL2C: {struct Cyc_Absyn_DatatypeFieldType_struct _tmp10D7;
struct Cyc_Absyn_DatatypeFieldInfo _tmp10D6;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp10D5;return(void*)((_tmp10D5=_cycalloc(sizeof(*_tmp10D5)),((_tmp10D5[0]=((
_tmp10D7.tag=4,((_tmp10D7.f1=((_tmp10D6.field_info=_tmp81,((_tmp10D6.targs=Cyc_Tcutil_copy_types(
_tmp82),_tmp10D6)))),_tmp10D7)))),_tmp10D5))));}_LL2D: {struct Cyc_Absyn_PointerType_struct*
_tmp83=(struct Cyc_Absyn_PointerType_struct*)_tmp76;if(_tmp83->tag != 5)goto _LL2F;
else{_tmp84=_tmp83->f1;_tmp85=_tmp84.elt_typ;_tmp86=_tmp84.elt_tq;_tmp87=_tmp84.ptr_atts;
_tmp88=_tmp87.rgn;_tmp89=_tmp87.nullable;_tmp8A=_tmp87.bounds;_tmp8B=_tmp87.zero_term;
_tmp8C=_tmp87.ptrloc;}}_LL2E: {void*_tmpE8=Cyc_Tcutil_copy_type(_tmp85);void*
_tmpE9=Cyc_Tcutil_copy_type(_tmp88);union Cyc_Absyn_Constraint*_tmpEA=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp89);struct Cyc_Absyn_Tqual
_tmpEB=_tmp86;union Cyc_Absyn_Constraint*_tmpEC=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp8A);union Cyc_Absyn_Constraint*
_tmpED=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(
_tmp8B);struct Cyc_Absyn_PointerType_struct _tmp10E1;struct Cyc_Absyn_PtrAtts
_tmp10E0;struct Cyc_Absyn_PtrInfo _tmp10DF;struct Cyc_Absyn_PointerType_struct*
_tmp10DE;return(void*)((_tmp10DE=_cycalloc(sizeof(*_tmp10DE)),((_tmp10DE[0]=((
_tmp10E1.tag=5,((_tmp10E1.f1=((_tmp10DF.elt_typ=_tmpE8,((_tmp10DF.elt_tq=_tmpEB,((
_tmp10DF.ptr_atts=((_tmp10E0.rgn=_tmpE9,((_tmp10E0.nullable=_tmpEA,((_tmp10E0.bounds=
_tmpEC,((_tmp10E0.zero_term=_tmpED,((_tmp10E0.ptrloc=_tmp8C,_tmp10E0)))))))))),
_tmp10DF)))))),_tmp10E1)))),_tmp10DE))));}_LL2F: {struct Cyc_Absyn_IntType_struct*
_tmp8D=(struct Cyc_Absyn_IntType_struct*)_tmp76;if(_tmp8D->tag != 6)goto _LL31;}
_LL30: goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_struct*_tmp8E=(struct Cyc_Absyn_FloatType_struct*)
_tmp76;if(_tmp8E->tag != 7)goto _LL33;}_LL32: goto _LL34;_LL33: {struct Cyc_Absyn_DoubleType_struct*
_tmp8F=(struct Cyc_Absyn_DoubleType_struct*)_tmp76;if(_tmp8F->tag != 8)goto _LL35;}
_LL34: return t;_LL35: {struct Cyc_Absyn_ArrayType_struct*_tmp90=(struct Cyc_Absyn_ArrayType_struct*)
_tmp76;if(_tmp90->tag != 9)goto _LL37;else{_tmp91=_tmp90->f1;_tmp92=_tmp91.elt_type;
_tmp93=_tmp91.tq;_tmp94=_tmp91.num_elts;_tmp95=_tmp91.zero_term;_tmp96=_tmp91.zt_loc;}}
_LL36: {struct Cyc_Absyn_ArrayType_struct _tmp10E7;struct Cyc_Absyn_ArrayInfo
_tmp10E6;struct Cyc_Absyn_ArrayType_struct*_tmp10E5;return(void*)((_tmp10E5=
_cycalloc(sizeof(*_tmp10E5)),((_tmp10E5[0]=((_tmp10E7.tag=9,((_tmp10E7.f1=((
_tmp10E6.elt_type=Cyc_Tcutil_copy_type(_tmp92),((_tmp10E6.tq=_tmp93,((_tmp10E6.num_elts=
_tmp94,((_tmp10E6.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
cptr))Cyc_Tcutil_copy_conref)(_tmp95),((_tmp10E6.zt_loc=_tmp96,_tmp10E6)))))))))),
_tmp10E7)))),_tmp10E5))));}_LL37: {struct Cyc_Absyn_FnType_struct*_tmp97=(struct
Cyc_Absyn_FnType_struct*)_tmp76;if(_tmp97->tag != 10)goto _LL39;else{_tmp98=_tmp97->f1;
_tmp99=_tmp98.tvars;_tmp9A=_tmp98.effect;_tmp9B=_tmp98.ret_tqual;_tmp9C=_tmp98.ret_typ;
_tmp9D=_tmp98.args;_tmp9E=_tmp98.c_varargs;_tmp9F=_tmp98.cyc_varargs;_tmpA0=
_tmp98.rgn_po;_tmpA1=_tmp98.attributes;}}_LL38: {struct Cyc_List_List*_tmpF5=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp99);struct Cyc_Core_Opt*
_tmp10E8;struct Cyc_Core_Opt*_tmpF6=_tmp9A == 0?0:((_tmp10E8=_cycalloc(sizeof(*
_tmp10E8)),((_tmp10E8->v=Cyc_Tcutil_copy_type((void*)_tmp9A->v),_tmp10E8))));
void*_tmpF7=Cyc_Tcutil_copy_type(_tmp9C);struct Cyc_List_List*_tmpF8=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,
_tmp9D);int _tmpF9=_tmp9E;struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;if(_tmp9F != 0){
struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)_tmp9F;struct Cyc_Absyn_VarargInfo*
_tmp10E9;cyc_varargs2=((_tmp10E9=_cycalloc(sizeof(*_tmp10E9)),((_tmp10E9->name=
cv->name,((_tmp10E9->tq=cv->tq,((_tmp10E9->type=Cyc_Tcutil_copy_type(cv->type),((
_tmp10E9->inject=cv->inject,_tmp10E9))))))))));}{struct Cyc_List_List*_tmpFB=((
struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmpA0);struct Cyc_List_List*_tmpFC=_tmpA1;
struct Cyc_Absyn_FnType_struct _tmp10EF;struct Cyc_Absyn_FnInfo _tmp10EE;struct Cyc_Absyn_FnType_struct*
_tmp10ED;return(void*)((_tmp10ED=_cycalloc(sizeof(*_tmp10ED)),((_tmp10ED[0]=((
_tmp10EF.tag=10,((_tmp10EF.f1=((_tmp10EE.tvars=_tmpF5,((_tmp10EE.effect=_tmpF6,((
_tmp10EE.ret_tqual=_tmp9B,((_tmp10EE.ret_typ=_tmpF7,((_tmp10EE.args=_tmpF8,((
_tmp10EE.c_varargs=_tmpF9,((_tmp10EE.cyc_varargs=cyc_varargs2,((_tmp10EE.rgn_po=
_tmpFB,((_tmp10EE.attributes=_tmpFC,_tmp10EE)))))))))))))))))),_tmp10EF)))),
_tmp10ED))));};}_LL39: {struct Cyc_Absyn_TupleType_struct*_tmpA2=(struct Cyc_Absyn_TupleType_struct*)
_tmp76;if(_tmpA2->tag != 11)goto _LL3B;else{_tmpA3=_tmpA2->f1;}}_LL3A: {struct Cyc_Absyn_TupleType_struct
_tmp10F2;struct Cyc_Absyn_TupleType_struct*_tmp10F1;return(void*)((_tmp10F1=
_cycalloc(sizeof(*_tmp10F1)),((_tmp10F1[0]=((_tmp10F2.tag=11,((_tmp10F2.f1=((
struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpA3),_tmp10F2)))),_tmp10F1))));}_LL3B: {
struct Cyc_Absyn_AggrType_struct*_tmpA4=(struct Cyc_Absyn_AggrType_struct*)_tmp76;
if(_tmpA4->tag != 12)goto _LL3D;else{_tmpA5=_tmpA4->f1;_tmpA6=_tmpA5.aggr_info;if((
_tmpA6.UnknownAggr).tag != 1)goto _LL3D;_tmpA7=(struct _tuple4)(_tmpA6.UnknownAggr).val;
_tmpA8=_tmpA7.f1;_tmpA9=_tmpA7.f2;_tmpAA=_tmpA7.f3;_tmpAB=_tmpA5.targs;}}_LL3C: {
struct Cyc_Absyn_AggrType_struct _tmp10F8;struct Cyc_Absyn_AggrInfo _tmp10F7;struct
Cyc_Absyn_AggrType_struct*_tmp10F6;return(void*)((_tmp10F6=_cycalloc(sizeof(*
_tmp10F6)),((_tmp10F6[0]=((_tmp10F8.tag=12,((_tmp10F8.f1=((_tmp10F7.aggr_info=
Cyc_Absyn_UnknownAggr(_tmpA8,_tmpA9,_tmpAA),((_tmp10F7.targs=Cyc_Tcutil_copy_types(
_tmpAB),_tmp10F7)))),_tmp10F8)))),_tmp10F6))));}_LL3D: {struct Cyc_Absyn_AggrType_struct*
_tmpAC=(struct Cyc_Absyn_AggrType_struct*)_tmp76;if(_tmpAC->tag != 12)goto _LL3F;
else{_tmpAD=_tmpAC->f1;_tmpAE=_tmpAD.aggr_info;if((_tmpAE.KnownAggr).tag != 2)
goto _LL3F;_tmpAF=(struct Cyc_Absyn_Aggrdecl**)(_tmpAE.KnownAggr).val;_tmpB0=
_tmpAD.targs;}}_LL3E: {struct Cyc_Absyn_AggrType_struct _tmp10FE;struct Cyc_Absyn_AggrInfo
_tmp10FD;struct Cyc_Absyn_AggrType_struct*_tmp10FC;return(void*)((_tmp10FC=
_cycalloc(sizeof(*_tmp10FC)),((_tmp10FC[0]=((_tmp10FE.tag=12,((_tmp10FE.f1=((
_tmp10FD.aggr_info=Cyc_Absyn_KnownAggr(_tmpAF),((_tmp10FD.targs=Cyc_Tcutil_copy_types(
_tmpB0),_tmp10FD)))),_tmp10FE)))),_tmp10FC))));}_LL3F: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpB1=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp76;if(_tmpB1->tag != 13)goto
_LL41;else{_tmpB2=_tmpB1->f1;_tmpB3=_tmpB1->f2;}}_LL40: {struct Cyc_Absyn_AnonAggrType_struct
_tmp1101;struct Cyc_Absyn_AnonAggrType_struct*_tmp1100;return(void*)((_tmp1100=
_cycalloc(sizeof(*_tmp1100)),((_tmp1100[0]=((_tmp1101.tag=13,((_tmp1101.f1=
_tmpB2,((_tmp1101.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmpB3),_tmp1101)))))),_tmp1100))));}_LL41: {struct Cyc_Absyn_EnumType_struct*
_tmpB4=(struct Cyc_Absyn_EnumType_struct*)_tmp76;if(_tmpB4->tag != 14)goto _LL43;
else{_tmpB5=_tmpB4->f1;_tmpB6=_tmpB4->f2;}}_LL42: {struct Cyc_Absyn_EnumType_struct
_tmp1104;struct Cyc_Absyn_EnumType_struct*_tmp1103;return(void*)((_tmp1103=
_cycalloc(sizeof(*_tmp1103)),((_tmp1103[0]=((_tmp1104.tag=14,((_tmp1104.f1=
_tmpB5,((_tmp1104.f2=_tmpB6,_tmp1104)))))),_tmp1103))));}_LL43: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpB7=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp76;if(_tmpB7->tag != 15)goto
_LL45;else{_tmpB8=_tmpB7->f1;}}_LL44: {struct Cyc_Absyn_AnonEnumType_struct
_tmp1107;struct Cyc_Absyn_AnonEnumType_struct*_tmp1106;return(void*)((_tmp1106=
_cycalloc(sizeof(*_tmp1106)),((_tmp1106[0]=((_tmp1107.tag=15,((_tmp1107.f1=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpB8),_tmp1107)))),
_tmp1106))));}_LL45: {struct Cyc_Absyn_TagType_struct*_tmpB9=(struct Cyc_Absyn_TagType_struct*)
_tmp76;if(_tmpB9->tag != 20)goto _LL47;else{_tmpBA=(void*)_tmpB9->f1;}}_LL46: {
struct Cyc_Absyn_TagType_struct _tmp110A;struct Cyc_Absyn_TagType_struct*_tmp1109;
return(void*)((_tmp1109=_cycalloc(sizeof(*_tmp1109)),((_tmp1109[0]=((_tmp110A.tag=
20,((_tmp110A.f1=(void*)Cyc_Tcutil_copy_type(_tmpBA),_tmp110A)))),_tmp1109))));}
_LL47: {struct Cyc_Absyn_ValueofType_struct*_tmpBB=(struct Cyc_Absyn_ValueofType_struct*)
_tmp76;if(_tmpBB->tag != 19)goto _LL49;else{_tmpBC=_tmpBB->f1;}}_LL48: {struct Cyc_Absyn_ValueofType_struct
_tmp110D;struct Cyc_Absyn_ValueofType_struct*_tmp110C;return(void*)((_tmp110C=
_cycalloc(sizeof(*_tmp110C)),((_tmp110C[0]=((_tmp110D.tag=19,((_tmp110D.f1=
_tmpBC,_tmp110D)))),_tmp110C))));}_LL49: {struct Cyc_Absyn_RgnHandleType_struct*
_tmpBD=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp76;if(_tmpBD->tag != 16)goto
_LL4B;else{_tmpBE=(void*)_tmpBD->f1;}}_LL4A: {struct Cyc_Absyn_RgnHandleType_struct
_tmp1110;struct Cyc_Absyn_RgnHandleType_struct*_tmp110F;return(void*)((_tmp110F=
_cycalloc(sizeof(*_tmp110F)),((_tmp110F[0]=((_tmp1110.tag=16,((_tmp1110.f1=(void*)
Cyc_Tcutil_copy_type(_tmpBE),_tmp1110)))),_tmp110F))));}_LL4B: {struct Cyc_Absyn_DynRgnType_struct*
_tmpBF=(struct Cyc_Absyn_DynRgnType_struct*)_tmp76;if(_tmpBF->tag != 17)goto _LL4D;
else{_tmpC0=(void*)_tmpBF->f1;_tmpC1=(void*)_tmpBF->f2;}}_LL4C: {struct Cyc_Absyn_DynRgnType_struct
_tmp1113;struct Cyc_Absyn_DynRgnType_struct*_tmp1112;return(void*)((_tmp1112=
_cycalloc(sizeof(*_tmp1112)),((_tmp1112[0]=((_tmp1113.tag=17,((_tmp1113.f1=(void*)
Cyc_Tcutil_copy_type(_tmpC0),((_tmp1113.f2=(void*)Cyc_Tcutil_copy_type(_tmpC1),
_tmp1113)))))),_tmp1112))));}_LL4D: {struct Cyc_Absyn_TypedefType_struct*_tmpC2=(
struct Cyc_Absyn_TypedefType_struct*)_tmp76;if(_tmpC2->tag != 18)goto _LL4F;else{
_tmpC3=_tmpC2->f1;_tmpC4=_tmpC2->f2;_tmpC5=_tmpC2->f3;}}_LL4E: {struct Cyc_Absyn_TypedefType_struct
_tmp1116;struct Cyc_Absyn_TypedefType_struct*_tmp1115;return(void*)((_tmp1115=
_cycalloc(sizeof(*_tmp1115)),((_tmp1115[0]=((_tmp1116.tag=18,((_tmp1116.f1=
_tmpC3,((_tmp1116.f2=Cyc_Tcutil_copy_types(_tmpC4),((_tmp1116.f3=_tmpC5,((
_tmp1116.f4=0,_tmp1116)))))))))),_tmp1115))));}_LL4F: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpC6=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp76;if(_tmpC6->tag != 22)goto _LL51;}
_LL50: goto _LL52;_LL51: {struct Cyc_Absyn_HeapRgn_struct*_tmpC7=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp76;if(_tmpC7->tag != 21)goto _LL53;}_LL52: return t;_LL53: {struct Cyc_Absyn_AccessEff_struct*
_tmpC8=(struct Cyc_Absyn_AccessEff_struct*)_tmp76;if(_tmpC8->tag != 23)goto _LL55;
else{_tmpC9=(void*)_tmpC8->f1;}}_LL54: {struct Cyc_Absyn_AccessEff_struct _tmp1119;
struct Cyc_Absyn_AccessEff_struct*_tmp1118;return(void*)((_tmp1118=_cycalloc(
sizeof(*_tmp1118)),((_tmp1118[0]=((_tmp1119.tag=23,((_tmp1119.f1=(void*)Cyc_Tcutil_copy_type(
_tmpC9),_tmp1119)))),_tmp1118))));}_LL55: {struct Cyc_Absyn_JoinEff_struct*_tmpCA=(
struct Cyc_Absyn_JoinEff_struct*)_tmp76;if(_tmpCA->tag != 24)goto _LL57;else{_tmpCB=
_tmpCA->f1;}}_LL56: {struct Cyc_Absyn_JoinEff_struct _tmp111C;struct Cyc_Absyn_JoinEff_struct*
_tmp111B;return(void*)((_tmp111B=_cycalloc(sizeof(*_tmp111B)),((_tmp111B[0]=((
_tmp111C.tag=24,((_tmp111C.f1=Cyc_Tcutil_copy_types(_tmpCB),_tmp111C)))),
_tmp111B))));}_LL57: {struct Cyc_Absyn_RgnsEff_struct*_tmpCC=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp76;if(_tmpCC->tag != 25)goto _LL59;else{_tmpCD=(void*)_tmpCC->f1;}}_LL58: {
struct Cyc_Absyn_RgnsEff_struct _tmp111F;struct Cyc_Absyn_RgnsEff_struct*_tmp111E;
return(void*)((_tmp111E=_cycalloc(sizeof(*_tmp111E)),((_tmp111E[0]=((_tmp111F.tag=
25,((_tmp111F.f1=(void*)Cyc_Tcutil_copy_type(_tmpCD),_tmp111F)))),_tmp111E))));}
_LL59: {struct Cyc_Absyn_TypeDeclType_struct*_tmpCE=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpCE->tag != 26)goto _LL5B;else{_tmpCF=_tmpCE->f1;_tmpD0=*_tmpCF;
_tmpD1=_tmpD0.r;{struct Cyc_Absyn_Aggr_td_struct*_tmpD2=(struct Cyc_Absyn_Aggr_td_struct*)
_tmpD1;if(_tmpD2->tag != 0)goto _LL5B;else{_tmpD3=_tmpD2->f1;}};}}_LL5A: {struct
Cyc_List_List*_tmp11F=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD3->tvs);struct Cyc_Absyn_AggrType_struct
_tmp1125;struct Cyc_Absyn_AggrInfo _tmp1124;struct Cyc_Absyn_AggrType_struct*
_tmp1123;return(void*)((_tmp1123=_cycalloc(sizeof(*_tmp1123)),((_tmp1123[0]=((
_tmp1125.tag=12,((_tmp1125.f1=((_tmp1124.aggr_info=Cyc_Absyn_UnknownAggr(_tmpD3->kind,
_tmpD3->name,0),((_tmp1124.targs=_tmp11F,_tmp1124)))),_tmp1125)))),_tmp1123))));}
_LL5B: {struct Cyc_Absyn_TypeDeclType_struct*_tmpD4=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpD4->tag != 26)goto _LL5D;else{_tmpD5=_tmpD4->f1;_tmpD6=*_tmpD5;
_tmpD7=_tmpD6.r;{struct Cyc_Absyn_Enum_td_struct*_tmpD8=(struct Cyc_Absyn_Enum_td_struct*)
_tmpD7;if(_tmpD8->tag != 1)goto _LL5D;else{_tmpD9=_tmpD8->f1;}};}}_LL5C: {struct
Cyc_Absyn_EnumType_struct _tmp1128;struct Cyc_Absyn_EnumType_struct*_tmp1127;
return(void*)((_tmp1127=_cycalloc(sizeof(*_tmp1127)),((_tmp1127[0]=((_tmp1128.tag=
14,((_tmp1128.f1=_tmpD9->name,((_tmp1128.f2=0,_tmp1128)))))),_tmp1127))));}_LL5D: {
struct Cyc_Absyn_TypeDeclType_struct*_tmpDA=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpDA->tag != 26)goto _LL22;else{_tmpDB=_tmpDA->f1;_tmpDC=*_tmpDB;
_tmpDD=_tmpDC.r;{struct Cyc_Absyn_Datatype_td_struct*_tmpDE=(struct Cyc_Absyn_Datatype_td_struct*)
_tmpDD;if(_tmpDE->tag != 2)goto _LL22;else{_tmpDF=_tmpDE->f1;}};}}_LL5E: {struct
Cyc_List_List*_tmp125=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpDF->tvs);struct Cyc_Absyn_DatatypeType_struct
_tmp1132;struct Cyc_Absyn_UnknownDatatypeInfo _tmp1131;struct Cyc_Absyn_DatatypeInfo
_tmp1130;struct Cyc_Absyn_DatatypeType_struct*_tmp112F;return(void*)((_tmp112F=
_cycalloc(sizeof(*_tmp112F)),((_tmp112F[0]=((_tmp1132.tag=3,((_tmp1132.f1=((
_tmp1130.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmp1131.name=_tmpDF->name,((
_tmp1131.is_extensible=0,_tmp1131))))),((_tmp1130.targs=_tmp125,_tmp1130)))),
_tmp1132)))),_tmp112F))));}_LL22:;}static void*Cyc_Tcutil_copy_designator(void*d){
void*_tmp12A=d;struct Cyc_Absyn_Exp*_tmp12C;struct _dyneither_ptr*_tmp12E;_LL60: {
struct Cyc_Absyn_ArrayElement_struct*_tmp12B=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp12A;if(_tmp12B->tag != 0)goto _LL62;else{_tmp12C=_tmp12B->f1;}}_LL61: {struct
Cyc_Absyn_ArrayElement_struct _tmp1135;struct Cyc_Absyn_ArrayElement_struct*
_tmp1134;return(void*)((_tmp1134=_cycalloc(sizeof(*_tmp1134)),((_tmp1134[0]=((
_tmp1135.tag=0,((_tmp1135.f1=Cyc_Tcutil_deep_copy_exp(_tmp12C),_tmp1135)))),
_tmp1134))));}_LL62: {struct Cyc_Absyn_FieldName_struct*_tmp12D=(struct Cyc_Absyn_FieldName_struct*)
_tmp12A;if(_tmp12D->tag != 1)goto _LL5F;else{_tmp12E=_tmp12D->f1;}}_LL63: return d;
_LL5F:;}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static
struct _tuple18*Cyc_Tcutil_copy_eds(struct _tuple18*e){struct _tuple18*_tmp1136;
return(_tmp1136=_cycalloc(sizeof(*_tmp1136)),((_tmp1136->f1=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_designator,(
e[0]).f1),((_tmp1136->f2=Cyc_Tcutil_deep_copy_exp((e[0]).f2),_tmp1136)))));}
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
_tmp17D;struct Cyc_Absyn_Exp*_tmp17E;struct Cyc_List_List*_tmp180;struct _tuple9*
_tmp182;struct _tuple9 _tmp183;struct Cyc_Core_Opt*_tmp184;struct Cyc_Absyn_Tqual
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
_tmp140=_tmp13D->f3;}}_LL6E: {struct Cyc_Core_Opt*_tmp1137;return Cyc_Absyn_assignop_exp(
Cyc_Tcutil_deep_copy_exp(_tmp13E),(unsigned int)_tmp13F?(_tmp1137=
_cycalloc_atomic(sizeof(*_tmp1137)),((_tmp1137->v=(void*)((enum Cyc_Absyn_Primop)
_tmp13F->v),_tmp1137))): 0,Cyc_Tcutil_deep_copy_exp(_tmp140),e->loc);}_LL6F: {
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
struct Cyc_Absyn_FnCall_e_struct _tmp1141;struct Cyc_Absyn_VarargInfo*_tmp1140;
struct Cyc_Absyn_VarargCallInfo*_tmp113F;struct Cyc_Absyn_FnCall_e_struct*_tmp113E;
return Cyc_Absyn_new_exp((void*)((_tmp113E=_cycalloc(sizeof(*_tmp113E)),((
_tmp113E[0]=((_tmp1141.tag=11,((_tmp1141.f1=Cyc_Tcutil_deep_copy_exp(_tmp155),((
_tmp1141.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp156),((
_tmp1141.f3=((_tmp113F=_cycalloc(sizeof(*_tmp113F)),((_tmp113F->num_varargs=
_tmp1BC,((_tmp113F->injectors=_tmp1BD,((_tmp113F->vai=((_tmp1140=_cycalloc(
sizeof(*_tmp1140)),((_tmp1140->name=_tmp1C1,((_tmp1140->tq=_tmp1C2,((_tmp1140->type=
Cyc_Tcutil_copy_type(_tmp1C3),((_tmp1140->inject=_tmp1C4,_tmp1140)))))))))),
_tmp113F)))))))),_tmp1141)))))))),_tmp113E)))),e->loc);};}_LLBA:;_LLBB: {struct
Cyc_Absyn_FnCall_e_struct _tmp1144;struct Cyc_Absyn_FnCall_e_struct*_tmp1143;
return Cyc_Absyn_new_exp((void*)((_tmp1143=_cycalloc(sizeof(*_tmp1143)),((
_tmp1143[0]=((_tmp1144.tag=11,((_tmp1144.f1=Cyc_Tcutil_deep_copy_exp(_tmp155),((
_tmp1144.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp156),((
_tmp1144.f3=_tmp157,_tmp1144)))))))),_tmp1143)))),e->loc);}_LLB7:;}_LL7D: {
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
_tmp172->f4;}}_LL92: {struct Cyc_Absyn_AggrMember_e_struct _tmp1147;struct Cyc_Absyn_AggrMember_e_struct*
_tmp1146;return Cyc_Absyn_new_exp((void*)((_tmp1146=_cycalloc(sizeof(*_tmp1146)),((
_tmp1146[0]=((_tmp1147.tag=22,((_tmp1147.f1=Cyc_Tcutil_deep_copy_exp(_tmp173),((
_tmp1147.f2=_tmp174,((_tmp1147.f3=_tmp175,((_tmp1147.f4=_tmp176,_tmp1147)))))))))),
_tmp1146)))),e->loc);}_LL93: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp177=(struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp132;if(_tmp177->tag != 23)goto _LL95;else{
_tmp178=_tmp177->f1;_tmp179=_tmp177->f2;_tmp17A=_tmp177->f3;_tmp17B=_tmp177->f4;}}
_LL94: {struct Cyc_Absyn_AggrArrow_e_struct _tmp114A;struct Cyc_Absyn_AggrArrow_e_struct*
_tmp1149;return Cyc_Absyn_new_exp((void*)((_tmp1149=_cycalloc(sizeof(*_tmp1149)),((
_tmp1149[0]=((_tmp114A.tag=23,((_tmp114A.f1=Cyc_Tcutil_deep_copy_exp(_tmp178),((
_tmp114A.f2=_tmp179,((_tmp114A.f3=_tmp17A,((_tmp114A.f4=_tmp17B,_tmp114A)))))))))),
_tmp1149)))),e->loc);}_LL95: {struct Cyc_Absyn_Subscript_e_struct*_tmp17C=(struct
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
_tmp114B;vopt1=((_tmp114B=_cycalloc(sizeof(*_tmp114B)),((_tmp114B->v=(struct
_dyneither_ptr*)_tmp184->v,_tmp114B))));}{struct Cyc_Absyn_CompoundLit_e_struct
_tmp1151;struct _tuple9*_tmp1150;struct Cyc_Absyn_CompoundLit_e_struct*_tmp114F;
return Cyc_Absyn_new_exp((void*)((_tmp114F=_cycalloc(sizeof(*_tmp114F)),((
_tmp114F[0]=((_tmp1151.tag=26,((_tmp1151.f1=((_tmp1150=_cycalloc(sizeof(*
_tmp1150)),((_tmp1150->f1=vopt1,((_tmp1150->f2=_tmp185,((_tmp1150->f3=Cyc_Tcutil_copy_type(
_tmp186),_tmp1150)))))))),((_tmp1151.f2=((struct Cyc_List_List*(*)(struct _tuple18*(*
f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp187),_tmp1151)))))),_tmp114F)))),e->loc);};}_LL9B: {struct Cyc_Absyn_Array_e_struct*
_tmp188=(struct Cyc_Absyn_Array_e_struct*)_tmp132;if(_tmp188->tag != 27)goto _LL9D;
else{_tmp189=_tmp188->f1;}}_LL9C: {struct Cyc_Absyn_Array_e_struct _tmp1154;struct
Cyc_Absyn_Array_e_struct*_tmp1153;return Cyc_Absyn_new_exp((void*)((_tmp1153=
_cycalloc(sizeof(*_tmp1153)),((_tmp1153[0]=((_tmp1154.tag=27,((_tmp1154.f1=((
struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp189),_tmp1154)))),_tmp1153)))),e->loc);}
_LL9D: {struct Cyc_Absyn_Comprehension_e_struct*_tmp18A=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp132;if(_tmp18A->tag != 28)goto _LL9F;else{_tmp18B=_tmp18A->f1;_tmp18C=_tmp18A->f2;
_tmp18D=_tmp18A->f3;_tmp18E=_tmp18A->f4;}}_LL9E: {struct Cyc_Absyn_Comprehension_e_struct
_tmp1157;struct Cyc_Absyn_Comprehension_e_struct*_tmp1156;return Cyc_Absyn_new_exp((
void*)((_tmp1156=_cycalloc(sizeof(*_tmp1156)),((_tmp1156[0]=((_tmp1157.tag=28,((
_tmp1157.f1=_tmp18B,((_tmp1157.f2=Cyc_Tcutil_deep_copy_exp(_tmp18C),((_tmp1157.f3=
Cyc_Tcutil_deep_copy_exp(_tmp18D),((_tmp1157.f4=_tmp18E,_tmp1157)))))))))),
_tmp1156)))),e->loc);}_LL9F: {struct Cyc_Absyn_Aggregate_e_struct*_tmp18F=(struct
Cyc_Absyn_Aggregate_e_struct*)_tmp132;if(_tmp18F->tag != 29)goto _LLA1;else{
_tmp190=_tmp18F->f1;_tmp191=_tmp18F->f2;_tmp192=_tmp18F->f3;_tmp193=_tmp18F->f4;}}
_LLA0: {struct Cyc_Absyn_Aggregate_e_struct _tmp115A;struct Cyc_Absyn_Aggregate_e_struct*
_tmp1159;return Cyc_Absyn_new_exp((void*)((_tmp1159=_cycalloc(sizeof(*_tmp1159)),((
_tmp1159[0]=((_tmp115A.tag=29,((_tmp115A.f1=_tmp190,((_tmp115A.f2=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,
_tmp191),((_tmp115A.f3=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct
_tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp192),((
_tmp115A.f4=_tmp193,_tmp115A)))))))))),_tmp1159)))),e->loc);}_LLA1: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp194=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp132;if(_tmp194->tag != 30)goto
_LLA3;else{_tmp195=(void*)_tmp194->f1;_tmp196=_tmp194->f2;}}_LLA2: {struct Cyc_Absyn_AnonStruct_e_struct
_tmp115D;struct Cyc_Absyn_AnonStruct_e_struct*_tmp115C;return Cyc_Absyn_new_exp((
void*)((_tmp115C=_cycalloc(sizeof(*_tmp115C)),((_tmp115C[0]=((_tmp115D.tag=30,((
_tmp115D.f1=(void*)Cyc_Tcutil_copy_type(_tmp195),((_tmp115D.f2=((struct Cyc_List_List*(*)(
struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp196),_tmp115D)))))),_tmp115C)))),e->loc);}_LLA3: {struct Cyc_Absyn_Datatype_e_struct*
_tmp197=(struct Cyc_Absyn_Datatype_e_struct*)_tmp132;if(_tmp197->tag != 31)goto
_LLA5;else{_tmp198=_tmp197->f1;_tmp199=_tmp197->f2;_tmp19A=_tmp197->f3;}}_LLA4: {
struct Cyc_Absyn_Datatype_e_struct _tmp1160;struct Cyc_Absyn_Datatype_e_struct*
_tmp115F;return Cyc_Absyn_new_exp((void*)((_tmp115F=_cycalloc(sizeof(*_tmp115F)),((
_tmp115F[0]=((_tmp1160.tag=31,((_tmp1160.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,
_tmp198),((_tmp1160.f2=_tmp199,((_tmp1160.f3=_tmp19A,_tmp1160)))))))),_tmp115F)))),
e->loc);}_LLA5: {struct Cyc_Absyn_Enum_e_struct*_tmp19B=(struct Cyc_Absyn_Enum_e_struct*)
_tmp132;if(_tmp19B->tag != 32)goto _LLA7;else{_tmp19C=_tmp19B->f1;_tmp19D=_tmp19B->f2;
_tmp19E=_tmp19B->f3;}}_LLA6: return e;_LLA7: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp19F=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp132;if(_tmp19F->tag != 33)goto
_LLA9;else{_tmp1A0=_tmp19F->f1;_tmp1A1=(void*)_tmp19F->f2;_tmp1A2=_tmp19F->f3;}}
_LLA8: {struct Cyc_Absyn_AnonEnum_e_struct _tmp1163;struct Cyc_Absyn_AnonEnum_e_struct*
_tmp1162;return Cyc_Absyn_new_exp((void*)((_tmp1162=_cycalloc(sizeof(*_tmp1162)),((
_tmp1162[0]=((_tmp1163.tag=33,((_tmp1163.f1=_tmp1A0,((_tmp1163.f2=(void*)Cyc_Tcutil_copy_type(
_tmp1A1),((_tmp1163.f3=_tmp1A2,_tmp1163)))))))),_tmp1162)))),e->loc);}_LLA9: {
struct Cyc_Absyn_Malloc_e_struct*_tmp1A3=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp132;if(_tmp1A3->tag != 34)goto _LLAB;else{_tmp1A4=_tmp1A3->f1;_tmp1A5=_tmp1A4.is_calloc;
_tmp1A6=_tmp1A4.rgn;_tmp1A7=_tmp1A4.elt_type;_tmp1A8=_tmp1A4.num_elts;_tmp1A9=
_tmp1A4.fat_result;}}_LLAA: {struct Cyc_Absyn_Exp*_tmp1DF=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp1A6;if(_tmp1A6 != 0)r1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((
struct Cyc_Absyn_Exp*)_tmp1A6);{void**t1=_tmp1A7;if(_tmp1A7 != 0){void**_tmp1164;
t1=((_tmp1164=_cycalloc(sizeof(*_tmp1164)),((_tmp1164[0]=Cyc_Tcutil_copy_type(*
_tmp1A7),_tmp1164))));}{struct Cyc_Absyn_Malloc_e_struct _tmp116A;struct Cyc_Absyn_MallocInfo
_tmp1169;struct Cyc_Absyn_Malloc_e_struct*_tmp1168;_tmp1DF->r=(void*)((_tmp1168=
_cycalloc(sizeof(*_tmp1168)),((_tmp1168[0]=((_tmp116A.tag=34,((_tmp116A.f1=((
_tmp1169.is_calloc=_tmp1A5,((_tmp1169.rgn=r1,((_tmp1169.elt_type=t1,((_tmp1169.num_elts=
_tmp1A8,((_tmp1169.fat_result=_tmp1A9,_tmp1169)))))))))),_tmp116A)))),_tmp1168))));}
return _tmp1DF;};}_LLAB: {struct Cyc_Absyn_Swap_e_struct*_tmp1AA=(struct Cyc_Absyn_Swap_e_struct*)
_tmp132;if(_tmp1AA->tag != 35)goto _LLAD;else{_tmp1AB=_tmp1AA->f1;_tmp1AC=_tmp1AA->f2;}}
_LLAC: return Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp1AB),Cyc_Tcutil_deep_copy_exp(
_tmp1AC),e->loc);_LLAD: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp1AD=(struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmp132;if(_tmp1AD->tag != 36)goto _LLAF;else{
_tmp1AE=_tmp1AD->f1;_tmp1AF=_tmp1AD->f2;}}_LLAE: {struct Cyc_Core_Opt*nopt1=
_tmp1AE;if(_tmp1AE != 0){struct Cyc_Core_Opt*_tmp116B;nopt1=((_tmp116B=_cycalloc(
sizeof(*_tmp116B)),((_tmp116B->v=(struct _tuple2*)_tmp1AE->v,_tmp116B))));}{
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp116E;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp116D;return Cyc_Absyn_new_exp((void*)((_tmp116D=_cycalloc(sizeof(*_tmp116D)),((
_tmp116D[0]=((_tmp116E.tag=36,((_tmp116E.f1=nopt1,((_tmp116E.f2=((struct Cyc_List_List*(*)(
struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp1AF),_tmp116E)))))),_tmp116D)))),e->loc);};}_LLAF: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp1B0=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp132;if(_tmp1B0->tag != 37)goto
_LLB1;}_LLB0: {struct Cyc_Core_Failure_struct _tmp1174;const char*_tmp1173;struct
Cyc_Core_Failure_struct*_tmp1172;(int)_throw((void*)((_tmp1172=_cycalloc(sizeof(*
_tmp1172)),((_tmp1172[0]=((_tmp1174.tag=Cyc_Core_Failure,((_tmp1174.f1=((
_tmp1173="deep_copy: statement expressions unsupported",_tag_dyneither(_tmp1173,
sizeof(char),45))),_tmp1174)))),_tmp1172)))));}_LLB1: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp1B1=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp132;if(_tmp1B1->tag != 38)goto
_LLB3;else{_tmp1B2=_tmp1B1->f1;_tmp1B3=_tmp1B1->f2;}}_LLB2: {struct Cyc_Absyn_Tagcheck_e_struct
_tmp1177;struct Cyc_Absyn_Tagcheck_e_struct*_tmp1176;return Cyc_Absyn_new_exp((
void*)((_tmp1176=_cycalloc(sizeof(*_tmp1176)),((_tmp1176[0]=((_tmp1177.tag=38,((
_tmp1177.f1=Cyc_Tcutil_deep_copy_exp(_tmp1B2),((_tmp1177.f2=_tmp1B3,_tmp1177)))))),
_tmp1176)))),e->loc);}_LLB3: {struct Cyc_Absyn_Valueof_e_struct*_tmp1B4=(struct
Cyc_Absyn_Valueof_e_struct*)_tmp132;if(_tmp1B4->tag != 39)goto _LLB5;else{_tmp1B5=(
void*)_tmp1B4->f1;}}_LLB4: return Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(
_tmp1B5),e->loc);_LLB5: {struct Cyc_Absyn_Asm_e_struct*_tmp1B6=(struct Cyc_Absyn_Asm_e_struct*)
_tmp132;if(_tmp1B6->tag != 40)goto _LL64;else{_tmp1B7=_tmp1B6->f1;_tmp1B8=_tmp1B6->f2;}}
_LLB6: return Cyc_Absyn_asm_exp(_tmp1B7,_tmp1B8,e->loc);_LL64:;}struct _tuple19{
enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple20{enum Cyc_Absyn_AliasQual
f1;enum Cyc_Absyn_AliasQual f2;};int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,
struct Cyc_Absyn_Kind*ka2){struct Cyc_Absyn_Kind _tmp1ED;enum Cyc_Absyn_KindQual
_tmp1EE;enum Cyc_Absyn_AliasQual _tmp1EF;struct Cyc_Absyn_Kind*_tmp1EC=ka1;_tmp1ED=*
_tmp1EC;_tmp1EE=_tmp1ED.kind;_tmp1EF=_tmp1ED.aliasqual;{struct Cyc_Absyn_Kind
_tmp1F1;enum Cyc_Absyn_KindQual _tmp1F2;enum Cyc_Absyn_AliasQual _tmp1F3;struct Cyc_Absyn_Kind*
_tmp1F0=ka2;_tmp1F1=*_tmp1F0;_tmp1F2=_tmp1F1.kind;_tmp1F3=_tmp1F1.aliasqual;if(
_tmp1EE != _tmp1F2){struct _tuple19 _tmp1178;struct _tuple19 _tmp1F5=(_tmp1178.f1=
_tmp1EE,((_tmp1178.f2=_tmp1F2,_tmp1178)));enum Cyc_Absyn_KindQual _tmp1F6;enum 
Cyc_Absyn_KindQual _tmp1F7;enum Cyc_Absyn_KindQual _tmp1F8;enum Cyc_Absyn_KindQual
_tmp1F9;enum Cyc_Absyn_KindQual _tmp1FA;enum Cyc_Absyn_KindQual _tmp1FB;_LLBD:
_tmp1F6=_tmp1F5.f1;if(_tmp1F6 != Cyc_Absyn_BoxKind)goto _LLBF;_tmp1F7=_tmp1F5.f2;
if(_tmp1F7 != Cyc_Absyn_MemKind)goto _LLBF;_LLBE: goto _LLC0;_LLBF: _tmp1F8=_tmp1F5.f1;
if(_tmp1F8 != Cyc_Absyn_BoxKind)goto _LLC1;_tmp1F9=_tmp1F5.f2;if(_tmp1F9 != Cyc_Absyn_AnyKind)
goto _LLC1;_LLC0: goto _LLC2;_LLC1: _tmp1FA=_tmp1F5.f1;if(_tmp1FA != Cyc_Absyn_MemKind)
goto _LLC3;_tmp1FB=_tmp1F5.f2;if(_tmp1FB != Cyc_Absyn_AnyKind)goto _LLC3;_LLC2: goto
_LLBC;_LLC3:;_LLC4: return 0;_LLBC:;}if(_tmp1EF != _tmp1F3){struct _tuple20 _tmp1179;
struct _tuple20 _tmp1FD=(_tmp1179.f1=_tmp1EF,((_tmp1179.f2=_tmp1F3,_tmp1179)));
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
_LLD1:;_LLD2:{struct Cyc_Absyn_Less_kb_struct _tmp117C;struct Cyc_Absyn_Less_kb_struct*
_tmp117B;tv->kind=(void*)((_tmp117B=_cycalloc(sizeof(*_tmp117B)),((_tmp117B[0]=((
_tmp117C.tag=2,((_tmp117C.f1=0,((_tmp117C.f2=def,_tmp117C)))))),_tmp117B))));}
return def;_LLCC:;}int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){struct _tuple0
_tmp117D;struct _tuple0 _tmp20A=(_tmp117D.f1=Cyc_Absyn_compress_kb(kb1),((_tmp117D.f2=
Cyc_Absyn_compress_kb(kb2),_tmp117D)));void*_tmp20B;struct Cyc_Absyn_Kind*_tmp20D;
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
struct Cyc_Core_Opt*_tmp117E;*_tmp214=((_tmp117E=_cycalloc(sizeof(*_tmp117E)),((
_tmp117E->v=kb2,_tmp117E))));}return 1;}else{return 0;}_LLD8: _tmp219=_tmp20A.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp21A=(struct Cyc_Absyn_Eq_kb_struct*)_tmp219;if(
_tmp21A->tag != 0)goto _LLDA;else{_tmp21B=_tmp21A->f1;}};_tmp21C=_tmp20A.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp21D=(struct Cyc_Absyn_Less_kb_struct*)_tmp21C;
if(_tmp21D->tag != 2)goto _LLDA;else{_tmp21E=_tmp21D->f1;_tmp21F=(struct Cyc_Core_Opt**)&
_tmp21D->f1;_tmp220=_tmp21D->f2;}};_LLD9: if(Cyc_Tcutil_kind_leq(_tmp21B,_tmp220)){{
struct Cyc_Core_Opt*_tmp117F;*_tmp21F=((_tmp117F=_cycalloc(sizeof(*_tmp117F)),((
_tmp117F->v=kb1,_tmp117F))));}return 1;}else{return 0;}_LLDA: _tmp221=_tmp20A.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp222=(struct Cyc_Absyn_Less_kb_struct*)_tmp221;
if(_tmp222->tag != 2)goto _LLDC;else{_tmp223=_tmp222->f1;_tmp224=(struct Cyc_Core_Opt**)&
_tmp222->f1;_tmp225=_tmp222->f2;}};_tmp226=_tmp20A.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp227=(struct Cyc_Absyn_Less_kb_struct*)_tmp226;if(_tmp227->tag != 2)goto _LLDC;
else{_tmp228=_tmp227->f1;_tmp229=(struct Cyc_Core_Opt**)& _tmp227->f1;_tmp22A=
_tmp227->f2;}};_LLDB: if(Cyc_Tcutil_kind_leq(_tmp225,_tmp22A)){{struct Cyc_Core_Opt*
_tmp1180;*_tmp229=((_tmp1180=_cycalloc(sizeof(*_tmp1180)),((_tmp1180->v=kb1,
_tmp1180))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp22A,_tmp225)){{struct Cyc_Core_Opt*
_tmp1181;*_tmp224=((_tmp1181=_cycalloc(sizeof(*_tmp1181)),((_tmp1181->v=kb2,
_tmp1181))));}return 1;}else{return 0;}}_LLDC: _tmp22B=_tmp20A.f1;{struct Cyc_Absyn_Unknown_kb_struct*
_tmp22C=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp22B;if(_tmp22C->tag != 1)goto
_LLDE;else{_tmp22D=_tmp22C->f1;_tmp22E=(struct Cyc_Core_Opt**)& _tmp22C->f1;}};
_tmp22F=_tmp20A.f2;_LLDD: _tmp230=_tmp22F;_tmp234=_tmp22E;goto _LLDF;_LLDE: _tmp230=
_tmp20A.f1;_tmp231=_tmp20A.f2;{struct Cyc_Absyn_Unknown_kb_struct*_tmp232=(struct
Cyc_Absyn_Unknown_kb_struct*)_tmp231;if(_tmp232->tag != 1)goto _LLD3;else{_tmp233=
_tmp232->f1;_tmp234=(struct Cyc_Core_Opt**)& _tmp232->f1;}};_LLDF:{struct Cyc_Core_Opt*
_tmp1182;*_tmp234=((_tmp1182=_cycalloc(sizeof(*_tmp1182)),((_tmp1182->v=_tmp230,
_tmp1182))));}return 1;_LLD3:;}struct _tuple15 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp23A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp23C;_LLE1: {struct
Cyc_Absyn_VarType_struct*_tmp23B=(struct Cyc_Absyn_VarType_struct*)_tmp23A;if(
_tmp23B->tag != 2)goto _LLE3;else{_tmp23C=_tmp23B->f1;}}_LLE2: {void*_tmp23D=
_tmp23C->kind;_tmp23C->kind=kb;{struct _tuple15 _tmp1183;return(_tmp1183.f1=
_tmp23C,((_tmp1183.f2=_tmp23D,_tmp1183)));};}_LLE3:;_LLE4: {const char*_tmp1187;
void*_tmp1186[1];struct Cyc_String_pa_struct _tmp1185;(_tmp1185.tag=0,((_tmp1185.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp1186[0]=& _tmp1185,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1187="swap_kind: cannot update the kind of %s",
_tag_dyneither(_tmp1187,sizeof(char),40))),_tag_dyneither(_tmp1186,sizeof(void*),
1)))))));}_LLE0:;}struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){void*_tmp242=
Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp244;struct Cyc_Absyn_Tvar*_tmp246;
enum Cyc_Absyn_Size_of _tmp249;struct Cyc_Absyn_DatatypeFieldInfo _tmp255;union Cyc_Absyn_DatatypeFieldInfoU
_tmp256;struct _tuple3 _tmp257;struct Cyc_Absyn_Datatypedecl*_tmp258;struct Cyc_Absyn_Datatypefield*
_tmp259;struct Cyc_Absyn_DatatypeFieldInfo _tmp25B;union Cyc_Absyn_DatatypeFieldInfoU
_tmp25C;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp25D;struct Cyc_Absyn_AggrInfo
_tmp25F;union Cyc_Absyn_AggrInfoU _tmp260;struct _tuple4 _tmp261;struct Cyc_Absyn_AggrInfo
_tmp263;union Cyc_Absyn_AggrInfoU _tmp264;struct Cyc_Absyn_Aggrdecl**_tmp265;struct
Cyc_Absyn_Aggrdecl*_tmp266;struct Cyc_Absyn_Aggrdecl _tmp267;struct Cyc_Absyn_AggrdeclImpl*
_tmp268;struct Cyc_Absyn_PtrInfo _tmp26B;struct Cyc_Absyn_Typedefdecl*_tmp271;
struct Cyc_Absyn_TypeDecl*_tmp273;struct Cyc_Absyn_TypeDecl _tmp274;void*_tmp275;
struct Cyc_Absyn_TypeDecl*_tmp278;struct Cyc_Absyn_TypeDecl _tmp279;void*_tmp27A;
struct Cyc_Absyn_TypeDecl*_tmp27D;struct Cyc_Absyn_TypeDecl _tmp27E;void*_tmp27F;
_LLE6: {struct Cyc_Absyn_Evar_struct*_tmp243=(struct Cyc_Absyn_Evar_struct*)
_tmp242;if(_tmp243->tag != 1)goto _LLE8;else{_tmp244=_tmp243->f1;}}_LLE7: return(
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp244))->v;_LLE8: {
struct Cyc_Absyn_VarType_struct*_tmp245=(struct Cyc_Absyn_VarType_struct*)_tmp242;
if(_tmp245->tag != 2)goto _LLEA;else{_tmp246=_tmp245->f1;}}_LLE9: return Cyc_Tcutil_tvar_kind(
_tmp246,& Cyc_Tcutil_bk);_LLEA: {struct Cyc_Absyn_VoidType_struct*_tmp247=(struct
Cyc_Absyn_VoidType_struct*)_tmp242;if(_tmp247->tag != 0)goto _LLEC;}_LLEB: return&
Cyc_Tcutil_mk;_LLEC: {struct Cyc_Absyn_IntType_struct*_tmp248=(struct Cyc_Absyn_IntType_struct*)
_tmp242;if(_tmp248->tag != 6)goto _LLEE;else{_tmp249=_tmp248->f2;}}_LLED: return(
_tmp249 == (enum Cyc_Absyn_Size_of)Cyc_Absyn_Int_sz  || _tmp249 == (enum Cyc_Absyn_Size_of)
Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLEE: {struct Cyc_Absyn_FloatType_struct*
_tmp24A=(struct Cyc_Absyn_FloatType_struct*)_tmp242;if(_tmp24A->tag != 7)goto _LLF0;}
_LLEF: goto _LLF1;_LLF0: {struct Cyc_Absyn_DoubleType_struct*_tmp24B=(struct Cyc_Absyn_DoubleType_struct*)
_tmp242;if(_tmp24B->tag != 8)goto _LLF2;}_LLF1: goto _LLF3;_LLF2: {struct Cyc_Absyn_FnType_struct*
_tmp24C=(struct Cyc_Absyn_FnType_struct*)_tmp242;if(_tmp24C->tag != 10)goto _LLF4;}
_LLF3: return& Cyc_Tcutil_mk;_LLF4: {struct Cyc_Absyn_DynRgnType_struct*_tmp24D=(
struct Cyc_Absyn_DynRgnType_struct*)_tmp242;if(_tmp24D->tag != 17)goto _LLF6;}_LLF5:
goto _LLF7;_LLF6: {struct Cyc_Absyn_EnumType_struct*_tmp24E=(struct Cyc_Absyn_EnumType_struct*)
_tmp242;if(_tmp24E->tag != 14)goto _LLF8;}_LLF7: goto _LLF9;_LLF8: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp24F=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp242;if(_tmp24F->tag != 15)goto
_LLFA;}_LLF9: goto _LLFB;_LLFA: {struct Cyc_Absyn_RgnHandleType_struct*_tmp250=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp242;if(_tmp250->tag != 16)goto _LLFC;}
_LLFB: return& Cyc_Tcutil_bk;_LLFC: {struct Cyc_Absyn_UniqueRgn_struct*_tmp251=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp242;if(_tmp251->tag != 22)goto _LLFE;}_LLFD:
return& Cyc_Tcutil_urk;_LLFE: {struct Cyc_Absyn_HeapRgn_struct*_tmp252=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp242;if(_tmp252->tag != 21)goto _LL100;}_LLFF: return& Cyc_Tcutil_rk;_LL100: {
struct Cyc_Absyn_DatatypeType_struct*_tmp253=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp242;if(_tmp253->tag != 3)goto _LL102;}_LL101: return& Cyc_Tcutil_ak;_LL102: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp254=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp242;if(_tmp254->tag != 4)goto _LL104;else{_tmp255=_tmp254->f1;_tmp256=_tmp255.field_info;
if((_tmp256.KnownDatatypefield).tag != 2)goto _LL104;_tmp257=(struct _tuple3)(
_tmp256.KnownDatatypefield).val;_tmp258=_tmp257.f1;_tmp259=_tmp257.f2;}}_LL103:
return& Cyc_Tcutil_mk;_LL104: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp25A=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp242;if(_tmp25A->tag != 4)goto _LL106;
else{_tmp25B=_tmp25A->f1;_tmp25C=_tmp25B.field_info;if((_tmp25C.UnknownDatatypefield).tag
!= 1)goto _LL106;_tmp25D=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp25C.UnknownDatatypefield).val;}}
_LL105: {const char*_tmp118A;void*_tmp1189;(_tmp1189=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp118A="typ_kind: Unresolved DatatypeFieldType",
_tag_dyneither(_tmp118A,sizeof(char),39))),_tag_dyneither(_tmp1189,sizeof(void*),
0)));}_LL106: {struct Cyc_Absyn_AggrType_struct*_tmp25E=(struct Cyc_Absyn_AggrType_struct*)
_tmp242;if(_tmp25E->tag != 12)goto _LL108;else{_tmp25F=_tmp25E->f1;_tmp260=_tmp25F.aggr_info;
if((_tmp260.UnknownAggr).tag != 1)goto _LL108;_tmp261=(struct _tuple4)(_tmp260.UnknownAggr).val;}}
_LL107: return& Cyc_Tcutil_ak;_LL108: {struct Cyc_Absyn_AggrType_struct*_tmp262=(
struct Cyc_Absyn_AggrType_struct*)_tmp242;if(_tmp262->tag != 12)goto _LL10A;else{
_tmp263=_tmp262->f1;_tmp264=_tmp263.aggr_info;if((_tmp264.KnownAggr).tag != 2)
goto _LL10A;_tmp265=(struct Cyc_Absyn_Aggrdecl**)(_tmp264.KnownAggr).val;_tmp266=*
_tmp265;_tmp267=*_tmp266;_tmp268=_tmp267.impl;}}_LL109: return _tmp268 == 0?& Cyc_Tcutil_ak:&
Cyc_Tcutil_mk;_LL10A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp269=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp242;if(_tmp269->tag != 13)goto _LL10C;}_LL10B: return& Cyc_Tcutil_mk;_LL10C: {
struct Cyc_Absyn_PointerType_struct*_tmp26A=(struct Cyc_Absyn_PointerType_struct*)
_tmp242;if(_tmp26A->tag != 5)goto _LL10E;else{_tmp26B=_tmp26A->f1;}}_LL10D: {void*
_tmp286=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,(_tmp26B.ptr_atts).bounds);_LL125: {struct Cyc_Absyn_DynEither_b_struct*
_tmp287=(struct Cyc_Absyn_DynEither_b_struct*)_tmp286;if(_tmp287->tag != 0)goto
_LL127;}_LL126: {struct Cyc_Absyn_Kind*_tmp118B;return(_tmp118B=_cycalloc_atomic(
sizeof(*_tmp118B)),((_tmp118B->kind=Cyc_Absyn_MemKind,((_tmp118B->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp26B.ptr_atts).rgn))->aliasqual,_tmp118B)))));}_LL127: {struct Cyc_Absyn_Upper_b_struct*
_tmp288=(struct Cyc_Absyn_Upper_b_struct*)_tmp286;if(_tmp288->tag != 1)goto _LL124;}
_LL128: {struct Cyc_Absyn_Kind*_tmp118C;return(_tmp118C=_cycalloc_atomic(sizeof(*
_tmp118C)),((_tmp118C->kind=Cyc_Absyn_BoxKind,((_tmp118C->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp26B.ptr_atts).rgn))->aliasqual,_tmp118C)))));}_LL124:;}_LL10E: {struct Cyc_Absyn_ValueofType_struct*
_tmp26C=(struct Cyc_Absyn_ValueofType_struct*)_tmp242;if(_tmp26C->tag != 19)goto
_LL110;}_LL10F: return& Cyc_Tcutil_ik;_LL110: {struct Cyc_Absyn_TagType_struct*
_tmp26D=(struct Cyc_Absyn_TagType_struct*)_tmp242;if(_tmp26D->tag != 20)goto _LL112;}
_LL111: return& Cyc_Tcutil_bk;_LL112: {struct Cyc_Absyn_ArrayType_struct*_tmp26E=(
struct Cyc_Absyn_ArrayType_struct*)_tmp242;if(_tmp26E->tag != 9)goto _LL114;}_LL113:
goto _LL115;_LL114: {struct Cyc_Absyn_TupleType_struct*_tmp26F=(struct Cyc_Absyn_TupleType_struct*)
_tmp242;if(_tmp26F->tag != 11)goto _LL116;}_LL115: return& Cyc_Tcutil_mk;_LL116: {
struct Cyc_Absyn_TypedefType_struct*_tmp270=(struct Cyc_Absyn_TypedefType_struct*)
_tmp242;if(_tmp270->tag != 18)goto _LL118;else{_tmp271=_tmp270->f3;}}_LL117: if(
_tmp271 == 0  || _tmp271->kind == 0){const char*_tmp1190;void*_tmp118F[1];struct Cyc_String_pa_struct
_tmp118E;(_tmp118E.tag=0,((_tmp118E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp118F[0]=& _tmp118E,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1190="typ_kind: typedef found: %s",
_tag_dyneither(_tmp1190,sizeof(char),28))),_tag_dyneither(_tmp118F,sizeof(void*),
1)))))));}return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp271->kind))->v;
_LL118: {struct Cyc_Absyn_TypeDeclType_struct*_tmp272=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp242;if(_tmp272->tag != 26)goto _LL11A;else{_tmp273=_tmp272->f1;_tmp274=*
_tmp273;_tmp275=_tmp274.r;{struct Cyc_Absyn_Aggr_td_struct*_tmp276=(struct Cyc_Absyn_Aggr_td_struct*)
_tmp275;if(_tmp276->tag != 0)goto _LL11A;};}}_LL119: return& Cyc_Tcutil_ak;_LL11A: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp277=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp242;if(_tmp277->tag != 26)goto _LL11C;else{_tmp278=_tmp277->f1;_tmp279=*
_tmp278;_tmp27A=_tmp279.r;{struct Cyc_Absyn_Enum_td_struct*_tmp27B=(struct Cyc_Absyn_Enum_td_struct*)
_tmp27A;if(_tmp27B->tag != 1)goto _LL11C;};}}_LL11B: return& Cyc_Tcutil_bk;_LL11C: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp27C=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp242;if(_tmp27C->tag != 26)goto _LL11E;else{_tmp27D=_tmp27C->f1;_tmp27E=*
_tmp27D;_tmp27F=_tmp27E.r;{struct Cyc_Absyn_Datatype_td_struct*_tmp280=(struct Cyc_Absyn_Datatype_td_struct*)
_tmp27F;if(_tmp280->tag != 2)goto _LL11E;};}}_LL11D: return& Cyc_Tcutil_ak;_LL11E: {
struct Cyc_Absyn_AccessEff_struct*_tmp281=(struct Cyc_Absyn_AccessEff_struct*)
_tmp242;if(_tmp281->tag != 23)goto _LL120;}_LL11F: goto _LL121;_LL120: {struct Cyc_Absyn_JoinEff_struct*
_tmp282=(struct Cyc_Absyn_JoinEff_struct*)_tmp242;if(_tmp282->tag != 24)goto _LL122;}
_LL121: goto _LL123;_LL122: {struct Cyc_Absyn_RgnsEff_struct*_tmp283=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp242;if(_tmp283->tag != 25)goto _LLE5;}_LL123: return& Cyc_Tcutil_ek;_LLE5:;}int
Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){return k1 == 
k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmp28E;_push_handler(& _tmp28E);{int _tmp290=
0;if(setjmp(_tmp28E.handler))_tmp290=1;if(!_tmp290){Cyc_Tcutil_unify_it(t1,t2);{
int _tmp291=1;_npop_handler(0);return _tmp291;};;_pop_handler();}else{void*_tmp28F=(
void*)_exn_thrown;void*_tmp293=_tmp28F;_LL12A: {struct Cyc_Tcutil_Unify_struct*
_tmp294=(struct Cyc_Tcutil_Unify_struct*)_tmp293;if(_tmp294->tag != Cyc_Tcutil_Unify)
goto _LL12C;}_LL12B: return 0;_LL12C:;_LL12D:(void)_throw(_tmp293);_LL129:;}};}
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(void*evar,struct
_RegionHandle*r,struct Cyc_List_List*env,void*t){t=Cyc_Tcutil_compress(t);{void*
_tmp295=t;struct Cyc_Absyn_Tvar*_tmp297;struct Cyc_Core_Opt*_tmp299;struct Cyc_Core_Opt*
_tmp29A;struct Cyc_Core_Opt**_tmp29B;struct Cyc_Absyn_PtrInfo _tmp29D;struct Cyc_Absyn_ArrayInfo
_tmp29F;void*_tmp2A0;struct Cyc_Absyn_FnInfo _tmp2A2;struct Cyc_List_List*_tmp2A3;
struct Cyc_Core_Opt*_tmp2A4;struct Cyc_Absyn_Tqual _tmp2A5;void*_tmp2A6;struct Cyc_List_List*
_tmp2A7;int _tmp2A8;struct Cyc_Absyn_VarargInfo*_tmp2A9;struct Cyc_List_List*
_tmp2AA;struct Cyc_List_List*_tmp2AB;struct Cyc_List_List*_tmp2AD;struct Cyc_Absyn_DatatypeInfo
_tmp2AF;struct Cyc_List_List*_tmp2B0;struct Cyc_List_List*_tmp2B2;struct Cyc_Absyn_DatatypeFieldInfo
_tmp2B4;struct Cyc_List_List*_tmp2B5;struct Cyc_Absyn_AggrInfo _tmp2B7;struct Cyc_List_List*
_tmp2B8;struct Cyc_List_List*_tmp2BA;void*_tmp2BC;void*_tmp2BE;void*_tmp2C0;void*
_tmp2C2;struct Cyc_List_List*_tmp2C4;_LL12F: {struct Cyc_Absyn_VarType_struct*
_tmp296=(struct Cyc_Absyn_VarType_struct*)_tmp295;if(_tmp296->tag != 2)goto _LL131;
else{_tmp297=_tmp296->f1;}}_LL130: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,env,_tmp297)){{const char*_tmp1191;Cyc_Tcutil_failure_reason=((
_tmp1191="(type variable would escape scope)",_tag_dyneither(_tmp1191,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}goto _LL12E;_LL131: {
struct Cyc_Absyn_Evar_struct*_tmp298=(struct Cyc_Absyn_Evar_struct*)_tmp295;if(
_tmp298->tag != 1)goto _LL133;else{_tmp299=_tmp298->f2;_tmp29A=_tmp298->f4;_tmp29B=(
struct Cyc_Core_Opt**)& _tmp298->f4;}}_LL132: if(t == evar){{const char*_tmp1192;Cyc_Tcutil_failure_reason=((
_tmp1192="(occurs check)",_tag_dyneither(_tmp1192,sizeof(char),15)));}(int)
_throw((void*)& Cyc_Tcutil_Unify_val);}else{if(_tmp299 != 0)Cyc_Tcutil_occurs(evar,
r,env,(void*)_tmp299->v);else{int problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp29B))->v;for(0;s != 0;s=s->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp2C7=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp29B))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd)){struct Cyc_List_List*_tmp1193;_tmp2C7=((
_tmp1193=_cycalloc(sizeof(*_tmp1193)),((_tmp1193->hd=(struct Cyc_Absyn_Tvar*)s->hd,((
_tmp1193->tl=_tmp2C7,_tmp1193))))));}}}{struct Cyc_Core_Opt*_tmp1194;*_tmp29B=((
_tmp1194=_cycalloc(sizeof(*_tmp1194)),((_tmp1194->v=_tmp2C7,_tmp1194))));};}}}
goto _LL12E;_LL133: {struct Cyc_Absyn_PointerType_struct*_tmp29C=(struct Cyc_Absyn_PointerType_struct*)
_tmp295;if(_tmp29C->tag != 5)goto _LL135;else{_tmp29D=_tmp29C->f1;}}_LL134: Cyc_Tcutil_occurs(
evar,r,env,_tmp29D.elt_typ);Cyc_Tcutil_occurs(evar,r,env,(_tmp29D.ptr_atts).rgn);
goto _LL12E;_LL135: {struct Cyc_Absyn_ArrayType_struct*_tmp29E=(struct Cyc_Absyn_ArrayType_struct*)
_tmp295;if(_tmp29E->tag != 9)goto _LL137;else{_tmp29F=_tmp29E->f1;_tmp2A0=_tmp29F.elt_type;}}
_LL136: Cyc_Tcutil_occurs(evar,r,env,_tmp2A0);goto _LL12E;_LL137: {struct Cyc_Absyn_FnType_struct*
_tmp2A1=(struct Cyc_Absyn_FnType_struct*)_tmp295;if(_tmp2A1->tag != 10)goto _LL139;
else{_tmp2A2=_tmp2A1->f1;_tmp2A3=_tmp2A2.tvars;_tmp2A4=_tmp2A2.effect;_tmp2A5=
_tmp2A2.ret_tqual;_tmp2A6=_tmp2A2.ret_typ;_tmp2A7=_tmp2A2.args;_tmp2A8=_tmp2A2.c_varargs;
_tmp2A9=_tmp2A2.cyc_varargs;_tmp2AA=_tmp2A2.rgn_po;_tmp2AB=_tmp2A2.attributes;}}
_LL138: env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp2A3,env);if(_tmp2A4 != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp2A4->v);Cyc_Tcutil_occurs(evar,r,env,_tmp2A6);for(0;
_tmp2A7 != 0;_tmp2A7=_tmp2A7->tl){Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple9*)
_tmp2A7->hd)).f3);}if(_tmp2A9 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp2A9->type);
for(0;_tmp2AA != 0;_tmp2AA=_tmp2AA->tl){struct _tuple0 _tmp2CB;void*_tmp2CC;void*
_tmp2CD;struct _tuple0*_tmp2CA=(struct _tuple0*)_tmp2AA->hd;_tmp2CB=*_tmp2CA;
_tmp2CC=_tmp2CB.f1;_tmp2CD=_tmp2CB.f2;Cyc_Tcutil_occurs(evar,r,env,_tmp2CC);Cyc_Tcutil_occurs(
evar,r,env,_tmp2CD);}goto _LL12E;_LL139: {struct Cyc_Absyn_TupleType_struct*
_tmp2AC=(struct Cyc_Absyn_TupleType_struct*)_tmp295;if(_tmp2AC->tag != 11)goto
_LL13B;else{_tmp2AD=_tmp2AC->f1;}}_LL13A: for(0;_tmp2AD != 0;_tmp2AD=_tmp2AD->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple11*)_tmp2AD->hd)).f2);}goto _LL12E;
_LL13B: {struct Cyc_Absyn_DatatypeType_struct*_tmp2AE=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp295;if(_tmp2AE->tag != 3)goto _LL13D;else{_tmp2AF=_tmp2AE->f1;_tmp2B0=_tmp2AF.targs;}}
_LL13C: Cyc_Tcutil_occurslist(evar,r,env,_tmp2B0);goto _LL12E;_LL13D: {struct Cyc_Absyn_TypedefType_struct*
_tmp2B1=(struct Cyc_Absyn_TypedefType_struct*)_tmp295;if(_tmp2B1->tag != 18)goto
_LL13F;else{_tmp2B2=_tmp2B1->f2;}}_LL13E: _tmp2B5=_tmp2B2;goto _LL140;_LL13F: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp2B3=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp295;if(_tmp2B3->tag != 4)goto _LL141;else{_tmp2B4=_tmp2B3->f1;_tmp2B5=_tmp2B4.targs;}}
_LL140: _tmp2B8=_tmp2B5;goto _LL142;_LL141: {struct Cyc_Absyn_AggrType_struct*
_tmp2B6=(struct Cyc_Absyn_AggrType_struct*)_tmp295;if(_tmp2B6->tag != 12)goto
_LL143;else{_tmp2B7=_tmp2B6->f1;_tmp2B8=_tmp2B7.targs;}}_LL142: Cyc_Tcutil_occurslist(
evar,r,env,_tmp2B8);goto _LL12E;_LL143: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp2B9=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp295;if(_tmp2B9->tag != 13)goto
_LL145;else{_tmp2BA=_tmp2B9->f2;}}_LL144: for(0;_tmp2BA != 0;_tmp2BA=_tmp2BA->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp2BA->hd)->type);}
goto _LL12E;_LL145: {struct Cyc_Absyn_TagType_struct*_tmp2BB=(struct Cyc_Absyn_TagType_struct*)
_tmp295;if(_tmp2BB->tag != 20)goto _LL147;else{_tmp2BC=(void*)_tmp2BB->f1;}}_LL146:
_tmp2BE=_tmp2BC;goto _LL148;_LL147: {struct Cyc_Absyn_AccessEff_struct*_tmp2BD=(
struct Cyc_Absyn_AccessEff_struct*)_tmp295;if(_tmp2BD->tag != 23)goto _LL149;else{
_tmp2BE=(void*)_tmp2BD->f1;}}_LL148: _tmp2C0=_tmp2BE;goto _LL14A;_LL149: {struct
Cyc_Absyn_RgnHandleType_struct*_tmp2BF=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp295;if(_tmp2BF->tag != 16)goto _LL14B;else{_tmp2C0=(void*)_tmp2BF->f1;}}_LL14A:
_tmp2C2=_tmp2C0;goto _LL14C;_LL14B: {struct Cyc_Absyn_RgnsEff_struct*_tmp2C1=(
struct Cyc_Absyn_RgnsEff_struct*)_tmp295;if(_tmp2C1->tag != 25)goto _LL14D;else{
_tmp2C2=(void*)_tmp2C1->f1;}}_LL14C: Cyc_Tcutil_occurs(evar,r,env,_tmp2C2);goto
_LL12E;_LL14D: {struct Cyc_Absyn_JoinEff_struct*_tmp2C3=(struct Cyc_Absyn_JoinEff_struct*)
_tmp295;if(_tmp2C3->tag != 24)goto _LL14F;else{_tmp2C4=_tmp2C3->f1;}}_LL14E: Cyc_Tcutil_occurslist(
evar,r,env,_tmp2C4);goto _LL12E;_LL14F:;_LL150: goto _LL12E;_LL12E:;};}static void
Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,
struct Cyc_List_List*ts){for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(
void*)ts->hd);}}static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*
t2){for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(
void*)t2->hd);}if(t1 != 0  || t2 != 0)(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual
tq2,void*t2){if(tq1.print_const  && !tq1.real_const){const char*_tmp1197;void*
_tmp1196;(_tmp1196=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1197="tq1 real_const not set.",_tag_dyneither(_tmp1197,
sizeof(char),24))),_tag_dyneither(_tmp1196,sizeof(void*),0)));}if(tq2.print_const
 && !tq2.real_const){const char*_tmp119A;void*_tmp1199;(_tmp1199=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp119A="tq2 real_const not set.",
_tag_dyneither(_tmp119A,sizeof(char),24))),_tag_dyneither(_tmp1199,sizeof(void*),
0)));}if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile)
 || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=tq2.real_const;{
const char*_tmp119B;Cyc_Tcutil_failure_reason=((_tmp119B="(qualifiers don't match)",
_tag_dyneither(_tmp119B,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct
Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const
 && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static
void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return;{union Cyc_Absyn_Constraint*
_tmp2D3=x;union Cyc_Absyn_Constraint _tmp2D4;int _tmp2D5;union Cyc_Absyn_Constraint
_tmp2D6;void*_tmp2D7;union Cyc_Absyn_Constraint _tmp2D8;union Cyc_Absyn_Constraint*
_tmp2D9;_LL152: _tmp2D4=*_tmp2D3;if((_tmp2D4.No_constr).tag != 3)goto _LL154;
_tmp2D5=(int)(_tmp2D4.No_constr).val;_LL153:{union Cyc_Absyn_Constraint _tmp119C;*
x=(((_tmp119C.Forward_constr).val=y,(((_tmp119C.Forward_constr).tag=2,_tmp119C))));}
return;_LL154: _tmp2D6=*_tmp2D3;if((_tmp2D6.Eq_constr).tag != 1)goto _LL156;_tmp2D7=(
void*)(_tmp2D6.Eq_constr).val;_LL155: {union Cyc_Absyn_Constraint*_tmp2DB=y;union
Cyc_Absyn_Constraint _tmp2DC;int _tmp2DD;union Cyc_Absyn_Constraint _tmp2DE;void*
_tmp2DF;union Cyc_Absyn_Constraint _tmp2E0;union Cyc_Absyn_Constraint*_tmp2E1;
_LL159: _tmp2DC=*_tmp2DB;if((_tmp2DC.No_constr).tag != 3)goto _LL15B;_tmp2DD=(int)(
_tmp2DC.No_constr).val;_LL15A:*y=*x;return;_LL15B: _tmp2DE=*_tmp2DB;if((_tmp2DE.Eq_constr).tag
!= 1)goto _LL15D;_tmp2DF=(void*)(_tmp2DE.Eq_constr).val;_LL15C: if(cmp(_tmp2D7,
_tmp2DF)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
return;_LL15D: _tmp2E0=*_tmp2DB;if((_tmp2E0.Forward_constr).tag != 2)goto _LL158;
_tmp2E1=(union Cyc_Absyn_Constraint*)(_tmp2E0.Forward_constr).val;_LL15E: {const
char*_tmp119F;void*_tmp119E;(_tmp119E=0,Cyc_Tcutil_impos(((_tmp119F="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp119F,sizeof(char),40))),_tag_dyneither(_tmp119E,sizeof(void*),
0)));}_LL158:;}_LL156: _tmp2D8=*_tmp2D3;if((_tmp2D8.Forward_constr).tag != 2)goto
_LL151;_tmp2D9=(union Cyc_Absyn_Constraint*)(_tmp2D8.Forward_constr).val;_LL157: {
const char*_tmp11A2;void*_tmp11A1;(_tmp11A1=0,Cyc_Tcutil_impos(((_tmp11A2="unify_conref: forward after compress",
_tag_dyneither(_tmp11A2,sizeof(char),37))),_tag_dyneither(_tmp11A1,sizeof(void*),
0)));}_LL151:;};}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){struct _handler_cons _tmp2E6;
_push_handler(& _tmp2E6);{int _tmp2E8=0;if(setjmp(_tmp2E6.handler))_tmp2E8=1;if(!
_tmp2E8){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _dyneither_ptr)
_tag_dyneither(0,0,0));{int _tmp2E9=1;_npop_handler(0);return _tmp2E9;};;
_pop_handler();}else{void*_tmp2E7=(void*)_exn_thrown;void*_tmp2EB=_tmp2E7;_LL160: {
struct Cyc_Tcutil_Unify_struct*_tmp2EC=(struct Cyc_Tcutil_Unify_struct*)_tmp2EB;
if(_tmp2EC->tag != Cyc_Tcutil_Unify)goto _LL162;}_LL161: return 0;_LL162:;_LL163:(
void)_throw(_tmp2EB);_LL15F:;}};}static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp11A3;struct _tuple0 _tmp2EE=(_tmp11A3.f1=b1,((_tmp11A3.f2=b2,
_tmp11A3)));void*_tmp2EF;void*_tmp2F1;void*_tmp2F3;void*_tmp2F5;void*_tmp2F7;
struct Cyc_Absyn_Exp*_tmp2F9;void*_tmp2FA;struct Cyc_Absyn_Exp*_tmp2FC;_LL165:
_tmp2EF=_tmp2EE.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp2F0=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp2EF;if(_tmp2F0->tag != 0)goto _LL167;};_tmp2F1=_tmp2EE.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2F2=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2F1;if(_tmp2F2->tag != 0)goto
_LL167;};_LL166: return 0;_LL167: _tmp2F3=_tmp2EE.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2F4=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2F3;if(_tmp2F4->tag != 0)goto
_LL169;};_LL168: return - 1;_LL169: _tmp2F5=_tmp2EE.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2F6=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2F5;if(_tmp2F6->tag != 0)goto
_LL16B;};_LL16A: return 1;_LL16B: _tmp2F7=_tmp2EE.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp2F8=(struct Cyc_Absyn_Upper_b_struct*)_tmp2F7;if(_tmp2F8->tag != 1)goto _LL164;
else{_tmp2F9=_tmp2F8->f1;}};_tmp2FA=_tmp2EE.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp2FB=(struct Cyc_Absyn_Upper_b_struct*)_tmp2FA;if(_tmp2FB->tag != 1)goto _LL164;
else{_tmp2FC=_tmp2FB->f1;}};_LL16C: return Cyc_Evexp_const_exp_cmp(_tmp2F9,_tmp2FC);
_LL164:;}static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){struct _tuple0
_tmp11A4;struct _tuple0 _tmp2FE=(_tmp11A4.f1=b1,((_tmp11A4.f2=b2,_tmp11A4)));void*
_tmp2FF;void*_tmp301;void*_tmp303;void*_tmp305;void*_tmp307;struct Cyc_Absyn_Exp*
_tmp309;void*_tmp30A;struct Cyc_Absyn_Exp*_tmp30C;_LL16E: _tmp2FF=_tmp2FE.f1;{
struct Cyc_Absyn_DynEither_b_struct*_tmp300=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp2FF;if(_tmp300->tag != 0)goto _LL170;};_tmp301=_tmp2FE.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp302=(struct Cyc_Absyn_DynEither_b_struct*)_tmp301;if(_tmp302->tag != 0)goto
_LL170;};_LL16F: return 0;_LL170: _tmp303=_tmp2FE.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp304=(struct Cyc_Absyn_DynEither_b_struct*)_tmp303;if(_tmp304->tag != 0)goto
_LL172;};_LL171: return - 1;_LL172: _tmp305=_tmp2FE.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp306=(struct Cyc_Absyn_DynEither_b_struct*)_tmp305;if(_tmp306->tag != 0)goto
_LL174;};_LL173: return 1;_LL174: _tmp307=_tmp2FE.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp308=(struct Cyc_Absyn_Upper_b_struct*)_tmp307;if(_tmp308->tag != 1)goto _LL16D;
else{_tmp309=_tmp308->f1;}};_tmp30A=_tmp2FE.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp30B=(struct Cyc_Absyn_Upper_b_struct*)_tmp30A;if(_tmp30B->tag != 1)goto _LL16D;
else{_tmp30C=_tmp30B->f1;}};_LL175: return Cyc_Evexp_const_exp_cmp(_tmp309,_tmp30C);
_LL16D:;}static int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp30D=att;
_LL177: {struct Cyc_Absyn_Regparm_att_struct*_tmp30E=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp30D;if(_tmp30E->tag != 0)goto _LL179;}_LL178: return 0;_LL179: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp30F=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp30D;if(_tmp30F->tag != 1)goto
_LL17B;}_LL17A: return 1;_LL17B: {struct Cyc_Absyn_Cdecl_att_struct*_tmp310=(struct
Cyc_Absyn_Cdecl_att_struct*)_tmp30D;if(_tmp310->tag != 2)goto _LL17D;}_LL17C:
return 2;_LL17D: {struct Cyc_Absyn_Fastcall_att_struct*_tmp311=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp30D;if(_tmp311->tag != 3)goto _LL17F;}_LL17E: return 3;_LL17F: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp312=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp30D;if(_tmp312->tag != 4)goto
_LL181;}_LL180: return 4;_LL181: {struct Cyc_Absyn_Const_att_struct*_tmp313=(struct
Cyc_Absyn_Const_att_struct*)_tmp30D;if(_tmp313->tag != 5)goto _LL183;}_LL182:
return 5;_LL183: {struct Cyc_Absyn_Aligned_att_struct*_tmp314=(struct Cyc_Absyn_Aligned_att_struct*)
_tmp30D;if(_tmp314->tag != 6)goto _LL185;}_LL184: return 6;_LL185: {struct Cyc_Absyn_Packed_att_struct*
_tmp315=(struct Cyc_Absyn_Packed_att_struct*)_tmp30D;if(_tmp315->tag != 7)goto
_LL187;}_LL186: return 7;_LL187: {struct Cyc_Absyn_Section_att_struct*_tmp316=(
struct Cyc_Absyn_Section_att_struct*)_tmp30D;if(_tmp316->tag != 8)goto _LL189;}
_LL188: return 8;_LL189: {struct Cyc_Absyn_Nocommon_att_struct*_tmp317=(struct Cyc_Absyn_Nocommon_att_struct*)
_tmp30D;if(_tmp317->tag != 9)goto _LL18B;}_LL18A: return 9;_LL18B: {struct Cyc_Absyn_Shared_att_struct*
_tmp318=(struct Cyc_Absyn_Shared_att_struct*)_tmp30D;if(_tmp318->tag != 10)goto
_LL18D;}_LL18C: return 10;_LL18D: {struct Cyc_Absyn_Unused_att_struct*_tmp319=(
struct Cyc_Absyn_Unused_att_struct*)_tmp30D;if(_tmp319->tag != 11)goto _LL18F;}
_LL18E: return 11;_LL18F: {struct Cyc_Absyn_Weak_att_struct*_tmp31A=(struct Cyc_Absyn_Weak_att_struct*)
_tmp30D;if(_tmp31A->tag != 12)goto _LL191;}_LL190: return 12;_LL191: {struct Cyc_Absyn_Dllimport_att_struct*
_tmp31B=(struct Cyc_Absyn_Dllimport_att_struct*)_tmp30D;if(_tmp31B->tag != 13)goto
_LL193;}_LL192: return 13;_LL193: {struct Cyc_Absyn_Dllexport_att_struct*_tmp31C=(
struct Cyc_Absyn_Dllexport_att_struct*)_tmp30D;if(_tmp31C->tag != 14)goto _LL195;}
_LL194: return 14;_LL195: {struct Cyc_Absyn_No_instrument_function_att_struct*
_tmp31D=(struct Cyc_Absyn_No_instrument_function_att_struct*)_tmp30D;if(_tmp31D->tag
!= 15)goto _LL197;}_LL196: return 15;_LL197: {struct Cyc_Absyn_Constructor_att_struct*
_tmp31E=(struct Cyc_Absyn_Constructor_att_struct*)_tmp30D;if(_tmp31E->tag != 16)
goto _LL199;}_LL198: return 16;_LL199: {struct Cyc_Absyn_Destructor_att_struct*
_tmp31F=(struct Cyc_Absyn_Destructor_att_struct*)_tmp30D;if(_tmp31F->tag != 17)
goto _LL19B;}_LL19A: return 17;_LL19B: {struct Cyc_Absyn_No_check_memory_usage_att_struct*
_tmp320=(struct Cyc_Absyn_No_check_memory_usage_att_struct*)_tmp30D;if(_tmp320->tag
!= 18)goto _LL19D;}_LL19C: return 18;_LL19D: {struct Cyc_Absyn_Format_att_struct*
_tmp321=(struct Cyc_Absyn_Format_att_struct*)_tmp30D;if(_tmp321->tag != 19)goto
_LL19F;}_LL19E: return 19;_LL19F: {struct Cyc_Absyn_Initializes_att_struct*_tmp322=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp30D;if(_tmp322->tag != 20)goto _LL1A1;}
_LL1A0: return 20;_LL1A1:;_LL1A2: return 21;_LL176:;}static int Cyc_Tcutil_attribute_cmp(
void*att1,void*att2){struct _tuple0 _tmp11A5;struct _tuple0 _tmp324=(_tmp11A5.f1=
att1,((_tmp11A5.f2=att2,_tmp11A5)));void*_tmp325;int _tmp327;void*_tmp328;int
_tmp32A;void*_tmp32B;int _tmp32D;void*_tmp32E;int _tmp330;void*_tmp331;int _tmp333;
void*_tmp334;int _tmp336;void*_tmp337;struct _dyneither_ptr _tmp339;void*_tmp33A;
struct _dyneither_ptr _tmp33C;void*_tmp33D;enum Cyc_Absyn_Format_Type _tmp33F;int
_tmp340;int _tmp341;void*_tmp342;enum Cyc_Absyn_Format_Type _tmp344;int _tmp345;int
_tmp346;_LL1A4: _tmp325=_tmp324.f1;{struct Cyc_Absyn_Regparm_att_struct*_tmp326=(
struct Cyc_Absyn_Regparm_att_struct*)_tmp325;if(_tmp326->tag != 0)goto _LL1A6;else{
_tmp327=_tmp326->f1;}};_tmp328=_tmp324.f2;{struct Cyc_Absyn_Regparm_att_struct*
_tmp329=(struct Cyc_Absyn_Regparm_att_struct*)_tmp328;if(_tmp329->tag != 0)goto
_LL1A6;else{_tmp32A=_tmp329->f1;}};_LL1A5: _tmp32D=_tmp327;_tmp330=_tmp32A;goto
_LL1A7;_LL1A6: _tmp32B=_tmp324.f1;{struct Cyc_Absyn_Initializes_att_struct*_tmp32C=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp32B;if(_tmp32C->tag != 20)goto _LL1A8;
else{_tmp32D=_tmp32C->f1;}};_tmp32E=_tmp324.f2;{struct Cyc_Absyn_Initializes_att_struct*
_tmp32F=(struct Cyc_Absyn_Initializes_att_struct*)_tmp32E;if(_tmp32F->tag != 20)
goto _LL1A8;else{_tmp330=_tmp32F->f1;}};_LL1A7: _tmp333=_tmp32D;_tmp336=_tmp330;
goto _LL1A9;_LL1A8: _tmp331=_tmp324.f1;{struct Cyc_Absyn_Aligned_att_struct*_tmp332=(
struct Cyc_Absyn_Aligned_att_struct*)_tmp331;if(_tmp332->tag != 6)goto _LL1AA;else{
_tmp333=_tmp332->f1;}};_tmp334=_tmp324.f2;{struct Cyc_Absyn_Aligned_att_struct*
_tmp335=(struct Cyc_Absyn_Aligned_att_struct*)_tmp334;if(_tmp335->tag != 6)goto
_LL1AA;else{_tmp336=_tmp335->f1;}};_LL1A9: return Cyc_Core_intcmp(_tmp333,_tmp336);
_LL1AA: _tmp337=_tmp324.f1;{struct Cyc_Absyn_Section_att_struct*_tmp338=(struct Cyc_Absyn_Section_att_struct*)
_tmp337;if(_tmp338->tag != 8)goto _LL1AC;else{_tmp339=_tmp338->f1;}};_tmp33A=
_tmp324.f2;{struct Cyc_Absyn_Section_att_struct*_tmp33B=(struct Cyc_Absyn_Section_att_struct*)
_tmp33A;if(_tmp33B->tag != 8)goto _LL1AC;else{_tmp33C=_tmp33B->f1;}};_LL1AB: return
Cyc_strcmp((struct _dyneither_ptr)_tmp339,(struct _dyneither_ptr)_tmp33C);_LL1AC:
_tmp33D=_tmp324.f1;{struct Cyc_Absyn_Format_att_struct*_tmp33E=(struct Cyc_Absyn_Format_att_struct*)
_tmp33D;if(_tmp33E->tag != 19)goto _LL1AE;else{_tmp33F=_tmp33E->f1;_tmp340=_tmp33E->f2;
_tmp341=_tmp33E->f3;}};_tmp342=_tmp324.f2;{struct Cyc_Absyn_Format_att_struct*
_tmp343=(struct Cyc_Absyn_Format_att_struct*)_tmp342;if(_tmp343->tag != 19)goto
_LL1AE;else{_tmp344=_tmp343->f1;_tmp345=_tmp343->f2;_tmp346=_tmp343->f3;}};
_LL1AD: {int _tmp347=Cyc_Core_intcmp((int)((unsigned int)_tmp33F),(int)((
unsigned int)_tmp344));if(_tmp347 != 0)return _tmp347;{int _tmp348=Cyc_Core_intcmp(
_tmp340,_tmp345);if(_tmp348 != 0)return _tmp348;return Cyc_Core_intcmp(_tmp341,
_tmp346);};}_LL1AE:;_LL1AF: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(
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
_tmp349=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind _tmp34A;
enum Cyc_Absyn_KindQual _tmp34B;enum Cyc_Absyn_AliasQual _tmp34C;struct Cyc_Absyn_Kind
_tmp34D;enum Cyc_Absyn_KindQual _tmp34E;enum Cyc_Absyn_AliasQual _tmp34F;struct Cyc_Absyn_Kind
_tmp350;enum Cyc_Absyn_KindQual _tmp351;struct Cyc_Absyn_Kind _tmp352;enum Cyc_Absyn_KindQual
_tmp353;_LL1B1: _tmp34A=*_tmp349;_tmp34B=_tmp34A.kind;if(_tmp34B != Cyc_Absyn_RgnKind)
goto _LL1B3;_tmp34C=_tmp34A.aliasqual;if(_tmp34C != Cyc_Absyn_Unique)goto _LL1B3;
_LL1B2: t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1B0;_LL1B3: _tmp34D=*_tmp349;
_tmp34E=_tmp34D.kind;if(_tmp34E != Cyc_Absyn_RgnKind)goto _LL1B5;_tmp34F=_tmp34D.aliasqual;
if(_tmp34F != Cyc_Absyn_Aliasable)goto _LL1B5;_LL1B4: t=(void*)& Cyc_Absyn_HeapRgn_val;
goto _LL1B0;_LL1B5: _tmp350=*_tmp349;_tmp351=_tmp350.kind;if(_tmp351 != Cyc_Absyn_EffKind)
goto _LL1B7;_LL1B6: t=Cyc_Absyn_empty_effect;goto _LL1B0;_LL1B7: _tmp352=*_tmp349;
_tmp353=_tmp352.kind;if(_tmp353 != Cyc_Absyn_IntKind)goto _LL1B9;_LL1B8:{struct Cyc_Absyn_ValueofType_struct
_tmp11A8;struct Cyc_Absyn_ValueofType_struct*_tmp11A7;t=(void*)((_tmp11A7=
_cycalloc(sizeof(*_tmp11A7)),((_tmp11A7[0]=((_tmp11A8.tag=19,((_tmp11A8.f1=Cyc_Absyn_uint_exp(
0,0),_tmp11A8)))),_tmp11A7))));}goto _LL1B0;_LL1B9:;_LL1BA: t=Cyc_Absyn_sint_typ;
goto _LL1B0;_LL1B0:;}{struct _tuple15*_tmp11A9;return(_tmp11A9=_cycalloc(sizeof(*
_tmp11A9)),((_tmp11A9->f1=tv,((_tmp11A9->f2=t,_tmp11A9)))));};}static void*Cyc_Tcutil_rgns_of(
void*t){void*_tmp357=Cyc_Tcutil_compress(t);void*_tmp362;void*_tmp364;void*
_tmp365;struct Cyc_Absyn_DatatypeInfo _tmp367;struct Cyc_List_List*_tmp368;struct
Cyc_Absyn_PtrInfo _tmp36A;void*_tmp36B;struct Cyc_Absyn_PtrAtts _tmp36C;void*
_tmp36D;struct Cyc_Absyn_ArrayInfo _tmp36F;void*_tmp370;struct Cyc_List_List*
_tmp372;struct Cyc_Absyn_DatatypeFieldInfo _tmp374;struct Cyc_List_List*_tmp375;
struct Cyc_Absyn_AggrInfo _tmp377;struct Cyc_List_List*_tmp378;struct Cyc_List_List*
_tmp37A;struct Cyc_Absyn_FnInfo _tmp37D;struct Cyc_List_List*_tmp37E;struct Cyc_Core_Opt*
_tmp37F;struct Cyc_Absyn_Tqual _tmp380;void*_tmp381;struct Cyc_List_List*_tmp382;
struct Cyc_Absyn_VarargInfo*_tmp383;struct Cyc_List_List*_tmp384;void*_tmp38A;
struct Cyc_List_List*_tmp38C;_LL1BC: {struct Cyc_Absyn_VoidType_struct*_tmp358=(
struct Cyc_Absyn_VoidType_struct*)_tmp357;if(_tmp358->tag != 0)goto _LL1BE;}_LL1BD:
goto _LL1BF;_LL1BE: {struct Cyc_Absyn_FloatType_struct*_tmp359=(struct Cyc_Absyn_FloatType_struct*)
_tmp357;if(_tmp359->tag != 7)goto _LL1C0;}_LL1BF: goto _LL1C1;_LL1C0: {struct Cyc_Absyn_DoubleType_struct*
_tmp35A=(struct Cyc_Absyn_DoubleType_struct*)_tmp357;if(_tmp35A->tag != 8)goto
_LL1C2;}_LL1C1: goto _LL1C3;_LL1C2: {struct Cyc_Absyn_EnumType_struct*_tmp35B=(
struct Cyc_Absyn_EnumType_struct*)_tmp357;if(_tmp35B->tag != 14)goto _LL1C4;}_LL1C3:
goto _LL1C5;_LL1C4: {struct Cyc_Absyn_AnonEnumType_struct*_tmp35C=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp357;if(_tmp35C->tag != 15)goto _LL1C6;}_LL1C5: goto _LL1C7;_LL1C6: {struct Cyc_Absyn_ValueofType_struct*
_tmp35D=(struct Cyc_Absyn_ValueofType_struct*)_tmp357;if(_tmp35D->tag != 19)goto
_LL1C8;}_LL1C7: goto _LL1C9;_LL1C8: {struct Cyc_Absyn_IntType_struct*_tmp35E=(
struct Cyc_Absyn_IntType_struct*)_tmp357;if(_tmp35E->tag != 6)goto _LL1CA;}_LL1C9:
return Cyc_Absyn_empty_effect;_LL1CA: {struct Cyc_Absyn_Evar_struct*_tmp35F=(
struct Cyc_Absyn_Evar_struct*)_tmp357;if(_tmp35F->tag != 1)goto _LL1CC;}_LL1CB: goto
_LL1CD;_LL1CC: {struct Cyc_Absyn_VarType_struct*_tmp360=(struct Cyc_Absyn_VarType_struct*)
_tmp357;if(_tmp360->tag != 2)goto _LL1CE;}_LL1CD: {struct Cyc_Absyn_Kind*_tmp38E=
Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind _tmp38F;enum Cyc_Absyn_KindQual
_tmp390;struct Cyc_Absyn_Kind _tmp391;enum Cyc_Absyn_KindQual _tmp392;struct Cyc_Absyn_Kind
_tmp393;enum Cyc_Absyn_KindQual _tmp394;_LL1F3: _tmp38F=*_tmp38E;_tmp390=_tmp38F.kind;
if(_tmp390 != Cyc_Absyn_RgnKind)goto _LL1F5;_LL1F4: {struct Cyc_Absyn_AccessEff_struct
_tmp11AC;struct Cyc_Absyn_AccessEff_struct*_tmp11AB;return(void*)((_tmp11AB=
_cycalloc(sizeof(*_tmp11AB)),((_tmp11AB[0]=((_tmp11AC.tag=23,((_tmp11AC.f1=(void*)
t,_tmp11AC)))),_tmp11AB))));}_LL1F5: _tmp391=*_tmp38E;_tmp392=_tmp391.kind;if(
_tmp392 != Cyc_Absyn_EffKind)goto _LL1F7;_LL1F6: return t;_LL1F7: _tmp393=*_tmp38E;
_tmp394=_tmp393.kind;if(_tmp394 != Cyc_Absyn_IntKind)goto _LL1F9;_LL1F8: return Cyc_Absyn_empty_effect;
_LL1F9:;_LL1FA: {struct Cyc_Absyn_RgnsEff_struct _tmp11AF;struct Cyc_Absyn_RgnsEff_struct*
_tmp11AE;return(void*)((_tmp11AE=_cycalloc(sizeof(*_tmp11AE)),((_tmp11AE[0]=((
_tmp11AF.tag=25,((_tmp11AF.f1=(void*)t,_tmp11AF)))),_tmp11AE))));}_LL1F2:;}
_LL1CE: {struct Cyc_Absyn_RgnHandleType_struct*_tmp361=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp357;if(_tmp361->tag != 16)goto _LL1D0;else{_tmp362=(void*)_tmp361->f1;}}_LL1CF: {
struct Cyc_Absyn_AccessEff_struct _tmp11B2;struct Cyc_Absyn_AccessEff_struct*
_tmp11B1;return(void*)((_tmp11B1=_cycalloc(sizeof(*_tmp11B1)),((_tmp11B1[0]=((
_tmp11B2.tag=23,((_tmp11B2.f1=(void*)_tmp362,_tmp11B2)))),_tmp11B1))));}_LL1D0: {
struct Cyc_Absyn_DynRgnType_struct*_tmp363=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp357;if(_tmp363->tag != 17)goto _LL1D2;else{_tmp364=(void*)_tmp363->f1;_tmp365=(
void*)_tmp363->f2;}}_LL1D1: {struct Cyc_Absyn_AccessEff_struct _tmp11B5;struct Cyc_Absyn_AccessEff_struct*
_tmp11B4;return(void*)((_tmp11B4=_cycalloc(sizeof(*_tmp11B4)),((_tmp11B4[0]=((
_tmp11B5.tag=23,((_tmp11B5.f1=(void*)_tmp365,_tmp11B5)))),_tmp11B4))));}_LL1D2: {
struct Cyc_Absyn_DatatypeType_struct*_tmp366=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp357;if(_tmp366->tag != 3)goto _LL1D4;else{_tmp367=_tmp366->f1;_tmp368=_tmp367.targs;}}
_LL1D3: {struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp368);struct Cyc_Absyn_JoinEff_struct
_tmp11B8;struct Cyc_Absyn_JoinEff_struct*_tmp11B7;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11B7=_cycalloc(sizeof(*_tmp11B7)),((_tmp11B7[0]=((_tmp11B8.tag=24,((
_tmp11B8.f1=ts,_tmp11B8)))),_tmp11B7)))));}_LL1D4: {struct Cyc_Absyn_PointerType_struct*
_tmp369=(struct Cyc_Absyn_PointerType_struct*)_tmp357;if(_tmp369->tag != 5)goto
_LL1D6;else{_tmp36A=_tmp369->f1;_tmp36B=_tmp36A.elt_typ;_tmp36C=_tmp36A.ptr_atts;
_tmp36D=_tmp36C.rgn;}}_LL1D5: {struct Cyc_Absyn_JoinEff_struct _tmp11C7;struct Cyc_Absyn_AccessEff_struct*
_tmp11C6;struct Cyc_Absyn_AccessEff_struct _tmp11C5;void*_tmp11C4[2];struct Cyc_Absyn_JoinEff_struct*
_tmp11C3;return Cyc_Tcutil_normalize_effect((void*)((_tmp11C3=_cycalloc(sizeof(*
_tmp11C3)),((_tmp11C3[0]=((_tmp11C7.tag=24,((_tmp11C7.f1=((_tmp11C4[1]=Cyc_Tcutil_rgns_of(
_tmp36B),((_tmp11C4[0]=(void*)((_tmp11C6=_cycalloc(sizeof(*_tmp11C6)),((_tmp11C6[
0]=((_tmp11C5.tag=23,((_tmp11C5.f1=(void*)_tmp36D,_tmp11C5)))),_tmp11C6)))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp11C4,sizeof(void*),2)))))),_tmp11C7)))),_tmp11C3)))));}_LL1D6: {struct Cyc_Absyn_ArrayType_struct*
_tmp36E=(struct Cyc_Absyn_ArrayType_struct*)_tmp357;if(_tmp36E->tag != 9)goto
_LL1D8;else{_tmp36F=_tmp36E->f1;_tmp370=_tmp36F.elt_type;}}_LL1D7: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp370));_LL1D8: {struct Cyc_Absyn_TupleType_struct*_tmp371=(
struct Cyc_Absyn_TupleType_struct*)_tmp357;if(_tmp371->tag != 11)goto _LL1DA;else{
_tmp372=_tmp371->f1;}}_LL1D9: {struct Cyc_List_List*_tmp3A4=0;for(0;_tmp372 != 0;
_tmp372=_tmp372->tl){struct Cyc_List_List*_tmp11C8;_tmp3A4=((_tmp11C8=_cycalloc(
sizeof(*_tmp11C8)),((_tmp11C8->hd=(*((struct _tuple11*)_tmp372->hd)).f2,((
_tmp11C8->tl=_tmp3A4,_tmp11C8))))));}_tmp375=_tmp3A4;goto _LL1DB;}_LL1DA: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp373=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp357;if(_tmp373->tag != 4)goto _LL1DC;else{_tmp374=_tmp373->f1;_tmp375=_tmp374.targs;}}
_LL1DB: _tmp378=_tmp375;goto _LL1DD;_LL1DC: {struct Cyc_Absyn_AggrType_struct*
_tmp376=(struct Cyc_Absyn_AggrType_struct*)_tmp357;if(_tmp376->tag != 12)goto
_LL1DE;else{_tmp377=_tmp376->f1;_tmp378=_tmp377.targs;}}_LL1DD: {struct Cyc_Absyn_JoinEff_struct
_tmp11CB;struct Cyc_Absyn_JoinEff_struct*_tmp11CA;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11CA=_cycalloc(sizeof(*_tmp11CA)),((_tmp11CA[0]=((_tmp11CB.tag=24,((
_tmp11CB.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp378),_tmp11CB)))),_tmp11CA)))));}_LL1DE: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp379=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp357;if(_tmp379->tag != 13)goto _LL1E0;else{_tmp37A=_tmp379->f2;}}_LL1DF: {
struct Cyc_Absyn_JoinEff_struct _tmp11CE;struct Cyc_Absyn_JoinEff_struct*_tmp11CD;
return Cyc_Tcutil_normalize_effect((void*)((_tmp11CD=_cycalloc(sizeof(*_tmp11CD)),((
_tmp11CD[0]=((_tmp11CE.tag=24,((_tmp11CE.f1=((struct Cyc_List_List*(*)(void*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,
_tmp37A),_tmp11CE)))),_tmp11CD)))));}_LL1E0: {struct Cyc_Absyn_TagType_struct*
_tmp37B=(struct Cyc_Absyn_TagType_struct*)_tmp357;if(_tmp37B->tag != 20)goto _LL1E2;}
_LL1E1: return Cyc_Absyn_empty_effect;_LL1E2: {struct Cyc_Absyn_FnType_struct*
_tmp37C=(struct Cyc_Absyn_FnType_struct*)_tmp357;if(_tmp37C->tag != 10)goto _LL1E4;
else{_tmp37D=_tmp37C->f1;_tmp37E=_tmp37D.tvars;_tmp37F=_tmp37D.effect;_tmp380=
_tmp37D.ret_tqual;_tmp381=_tmp37D.ret_typ;_tmp382=_tmp37D.args;_tmp383=_tmp37D.cyc_varargs;
_tmp384=_tmp37D.rgn_po;}}_LL1E3: {void*_tmp3AA=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_region_free_subst,_tmp37E),(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp37F))->v);return Cyc_Tcutil_normalize_effect(_tmp3AA);}_LL1E4: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp385=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp357;if(_tmp385->tag != 22)goto
_LL1E6;}_LL1E5: goto _LL1E7;_LL1E6: {struct Cyc_Absyn_HeapRgn_struct*_tmp386=(
struct Cyc_Absyn_HeapRgn_struct*)_tmp357;if(_tmp386->tag != 21)goto _LL1E8;}_LL1E7:
return Cyc_Absyn_empty_effect;_LL1E8: {struct Cyc_Absyn_AccessEff_struct*_tmp387=(
struct Cyc_Absyn_AccessEff_struct*)_tmp357;if(_tmp387->tag != 23)goto _LL1EA;}
_LL1E9: goto _LL1EB;_LL1EA: {struct Cyc_Absyn_JoinEff_struct*_tmp388=(struct Cyc_Absyn_JoinEff_struct*)
_tmp357;if(_tmp388->tag != 24)goto _LL1EC;}_LL1EB: return t;_LL1EC: {struct Cyc_Absyn_RgnsEff_struct*
_tmp389=(struct Cyc_Absyn_RgnsEff_struct*)_tmp357;if(_tmp389->tag != 25)goto _LL1EE;
else{_tmp38A=(void*)_tmp389->f1;}}_LL1ED: return Cyc_Tcutil_rgns_of(_tmp38A);
_LL1EE: {struct Cyc_Absyn_TypedefType_struct*_tmp38B=(struct Cyc_Absyn_TypedefType_struct*)
_tmp357;if(_tmp38B->tag != 18)goto _LL1F0;else{_tmp38C=_tmp38B->f2;}}_LL1EF: {
struct Cyc_Absyn_JoinEff_struct _tmp11D1;struct Cyc_Absyn_JoinEff_struct*_tmp11D0;
return Cyc_Tcutil_normalize_effect((void*)((_tmp11D0=_cycalloc(sizeof(*_tmp11D0)),((
_tmp11D0[0]=((_tmp11D1.tag=24,((_tmp11D1.f1=((struct Cyc_List_List*(*)(void*(*f)(
void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp38C),_tmp11D1)))),
_tmp11D0)))));}_LL1F0: {struct Cyc_Absyn_TypeDeclType_struct*_tmp38D=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp357;if(_tmp38D->tag != 26)goto _LL1BB;}_LL1F1: {const char*_tmp11D4;void*
_tmp11D3;(_tmp11D3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp11D4="typedecl in rgns_of",_tag_dyneither(_tmp11D4,
sizeof(char),20))),_tag_dyneither(_tmp11D3,sizeof(void*),0)));}_LL1BB:;}void*Cyc_Tcutil_normalize_effect(
void*e){e=Cyc_Tcutil_compress(e);{void*_tmp3AF=e;struct Cyc_List_List*_tmp3B1;
struct Cyc_List_List**_tmp3B2;void*_tmp3B4;_LL1FC: {struct Cyc_Absyn_JoinEff_struct*
_tmp3B0=(struct Cyc_Absyn_JoinEff_struct*)_tmp3AF;if(_tmp3B0->tag != 24)goto _LL1FE;
else{_tmp3B1=_tmp3B0->f1;_tmp3B2=(struct Cyc_List_List**)& _tmp3B0->f1;}}_LL1FD: {
int redo_join=0;{struct Cyc_List_List*effs=*_tmp3B2;for(0;effs != 0;effs=effs->tl){
void*_tmp3B5=(void*)effs->hd;effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(
_tmp3B5));{void*_tmp3B6=(void*)effs->hd;void*_tmp3B9;_LL203: {struct Cyc_Absyn_JoinEff_struct*
_tmp3B7=(struct Cyc_Absyn_JoinEff_struct*)_tmp3B6;if(_tmp3B7->tag != 24)goto _LL205;}
_LL204: goto _LL206;_LL205: {struct Cyc_Absyn_AccessEff_struct*_tmp3B8=(struct Cyc_Absyn_AccessEff_struct*)
_tmp3B6;if(_tmp3B8->tag != 23)goto _LL207;else{_tmp3B9=(void*)_tmp3B8->f1;{struct
Cyc_Absyn_HeapRgn_struct*_tmp3BA=(struct Cyc_Absyn_HeapRgn_struct*)_tmp3B9;if(
_tmp3BA->tag != 21)goto _LL207;};}}_LL206: redo_join=1;goto _LL202;_LL207:;_LL208:
goto _LL202;_LL202:;};}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{
struct Cyc_List_List*effs=*_tmp3B2;for(0;effs != 0;effs=effs->tl){void*_tmp3BB=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp3BD;void*_tmp3BF;_LL20A: {struct Cyc_Absyn_JoinEff_struct*
_tmp3BC=(struct Cyc_Absyn_JoinEff_struct*)_tmp3BB;if(_tmp3BC->tag != 24)goto _LL20C;
else{_tmp3BD=_tmp3BC->f1;}}_LL20B: effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp3BD,effects);goto _LL209;_LL20C: {
struct Cyc_Absyn_AccessEff_struct*_tmp3BE=(struct Cyc_Absyn_AccessEff_struct*)
_tmp3BB;if(_tmp3BE->tag != 23)goto _LL20E;else{_tmp3BF=(void*)_tmp3BE->f1;{struct
Cyc_Absyn_HeapRgn_struct*_tmp3C0=(struct Cyc_Absyn_HeapRgn_struct*)_tmp3BF;if(
_tmp3C0->tag != 21)goto _LL20E;};}}_LL20D: goto _LL209;_LL20E:;_LL20F:{struct Cyc_List_List*
_tmp11D5;effects=((_tmp11D5=_cycalloc(sizeof(*_tmp11D5)),((_tmp11D5->hd=_tmp3BB,((
_tmp11D5->tl=effects,_tmp11D5))))));}goto _LL209;_LL209:;}}*_tmp3B2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);return e;};}_LL1FE: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3B3=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3AF;if(_tmp3B3->tag != 25)goto _LL200;
else{_tmp3B4=(void*)_tmp3B3->f1;}}_LL1FF: {void*_tmp3C2=Cyc_Tcutil_compress(
_tmp3B4);_LL211: {struct Cyc_Absyn_Evar_struct*_tmp3C3=(struct Cyc_Absyn_Evar_struct*)
_tmp3C2;if(_tmp3C3->tag != 1)goto _LL213;}_LL212: goto _LL214;_LL213: {struct Cyc_Absyn_VarType_struct*
_tmp3C4=(struct Cyc_Absyn_VarType_struct*)_tmp3C2;if(_tmp3C4->tag != 2)goto _LL215;}
_LL214: return e;_LL215:;_LL216: return Cyc_Tcutil_rgns_of(_tmp3B4);_LL210:;}_LL200:;
_LL201: return e;_LL1FB:;};}static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct
_tmp11DF;struct Cyc_Core_Opt*_tmp11DE;struct Cyc_Absyn_FnInfo _tmp11DD;struct Cyc_Absyn_FnType_struct*
_tmp11DC;struct Cyc_Absyn_FnType_struct*_tmp3C5=(_tmp11DC=_cycalloc(sizeof(*
_tmp11DC)),((_tmp11DC[0]=((_tmp11DF.tag=10,((_tmp11DF.f1=((_tmp11DD.tvars=0,((
_tmp11DD.effect=((_tmp11DE=_cycalloc(sizeof(*_tmp11DE)),((_tmp11DE->v=eff,
_tmp11DE)))),((_tmp11DD.ret_tqual=Cyc_Absyn_empty_tqual(0),((_tmp11DD.ret_typ=(
void*)& Cyc_Absyn_VoidType_val,((_tmp11DD.args=0,((_tmp11DD.c_varargs=0,((
_tmp11DD.cyc_varargs=0,((_tmp11DD.rgn_po=0,((_tmp11DD.attributes=0,_tmp11DD)))))))))))))))))),
_tmp11DF)))),_tmp11DC)));return Cyc_Absyn_atb_typ((void*)_tmp3C5,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e){r=Cyc_Tcutil_compress(r);if(r == (void*)& Cyc_Absyn_HeapRgn_val
 || r == (void*)& Cyc_Absyn_UniqueRgn_val)return 1;{void*_tmp3CA=Cyc_Tcutil_compress(
e);void*_tmp3CC;struct Cyc_List_List*_tmp3CE;void*_tmp3D0;struct Cyc_Core_Opt*
_tmp3D2;struct Cyc_Core_Opt*_tmp3D3;struct Cyc_Core_Opt**_tmp3D4;struct Cyc_Core_Opt*
_tmp3D5;_LL218: {struct Cyc_Absyn_AccessEff_struct*_tmp3CB=(struct Cyc_Absyn_AccessEff_struct*)
_tmp3CA;if(_tmp3CB->tag != 23)goto _LL21A;else{_tmp3CC=(void*)_tmp3CB->f1;}}_LL219:
if(constrain)return Cyc_Tcutil_unify(r,_tmp3CC);_tmp3CC=Cyc_Tcutil_compress(
_tmp3CC);if(r == _tmp3CC)return 1;{struct _tuple0 _tmp11E0;struct _tuple0 _tmp3D7=(
_tmp11E0.f1=r,((_tmp11E0.f2=_tmp3CC,_tmp11E0)));void*_tmp3D8;struct Cyc_Absyn_Tvar*
_tmp3DA;void*_tmp3DB;struct Cyc_Absyn_Tvar*_tmp3DD;_LL223: _tmp3D8=_tmp3D7.f1;{
struct Cyc_Absyn_VarType_struct*_tmp3D9=(struct Cyc_Absyn_VarType_struct*)_tmp3D8;
if(_tmp3D9->tag != 2)goto _LL225;else{_tmp3DA=_tmp3D9->f1;}};_tmp3DB=_tmp3D7.f2;{
struct Cyc_Absyn_VarType_struct*_tmp3DC=(struct Cyc_Absyn_VarType_struct*)_tmp3DB;
if(_tmp3DC->tag != 2)goto _LL225;else{_tmp3DD=_tmp3DC->f1;}};_LL224: return Cyc_Absyn_tvar_cmp(
_tmp3DA,_tmp3DD)== 0;_LL225:;_LL226: return 0;_LL222:;};_LL21A: {struct Cyc_Absyn_JoinEff_struct*
_tmp3CD=(struct Cyc_Absyn_JoinEff_struct*)_tmp3CA;if(_tmp3CD->tag != 24)goto _LL21C;
else{_tmp3CE=_tmp3CD->f1;}}_LL21B: for(0;_tmp3CE != 0;_tmp3CE=_tmp3CE->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp3CE->hd))return 1;}return 0;_LL21C: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3CF=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3CA;if(_tmp3CF->tag != 25)goto _LL21E;
else{_tmp3D0=(void*)_tmp3CF->f1;}}_LL21D: {void*_tmp3DE=Cyc_Tcutil_rgns_of(
_tmp3D0);void*_tmp3E0;_LL228: {struct Cyc_Absyn_RgnsEff_struct*_tmp3DF=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp3DE;if(_tmp3DF->tag != 25)goto _LL22A;else{_tmp3E0=(void*)_tmp3DF->f1;}}_LL229:
if(!constrain)return 0;{void*_tmp3E1=Cyc_Tcutil_compress(_tmp3E0);struct Cyc_Core_Opt*
_tmp3E3;struct Cyc_Core_Opt*_tmp3E4;struct Cyc_Core_Opt**_tmp3E5;struct Cyc_Core_Opt*
_tmp3E6;_LL22D: {struct Cyc_Absyn_Evar_struct*_tmp3E2=(struct Cyc_Absyn_Evar_struct*)
_tmp3E1;if(_tmp3E2->tag != 1)goto _LL22F;else{_tmp3E3=_tmp3E2->f1;_tmp3E4=_tmp3E2->f2;
_tmp3E5=(struct Cyc_Core_Opt**)& _tmp3E2->f2;_tmp3E6=_tmp3E2->f4;}}_LL22E: {void*
_tmp3E7=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp3E6);Cyc_Tcutil_occurs(
_tmp3E7,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp3E6))->v,r);{struct Cyc_Absyn_JoinEff_struct _tmp11EF;struct Cyc_Absyn_AccessEff_struct*
_tmp11EE;struct Cyc_Absyn_AccessEff_struct _tmp11ED;void*_tmp11EC[2];struct Cyc_Absyn_JoinEff_struct*
_tmp11EB;void*_tmp3E8=Cyc_Tcutil_dummy_fntype((void*)((_tmp11EB=_cycalloc(
sizeof(*_tmp11EB)),((_tmp11EB[0]=((_tmp11EF.tag=24,((_tmp11EF.f1=((_tmp11EC[1]=(
void*)((_tmp11EE=_cycalloc(sizeof(*_tmp11EE)),((_tmp11EE[0]=((_tmp11ED.tag=23,((
_tmp11ED.f1=(void*)r,_tmp11ED)))),_tmp11EE)))),((_tmp11EC[0]=_tmp3E7,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp11EC,sizeof(void*),2)))))),
_tmp11EF)))),_tmp11EB)))));{struct Cyc_Core_Opt*_tmp11F0;*_tmp3E5=((_tmp11F0=
_cycalloc(sizeof(*_tmp11F0)),((_tmp11F0->v=_tmp3E8,_tmp11F0))));}return 1;};}
_LL22F:;_LL230: return 0;_LL22C:;};_LL22A:;_LL22B: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp3DE);_LL227:;}_LL21E: {struct Cyc_Absyn_Evar_struct*_tmp3D1=(
struct Cyc_Absyn_Evar_struct*)_tmp3CA;if(_tmp3D1->tag != 1)goto _LL220;else{_tmp3D2=
_tmp3D1->f1;_tmp3D3=_tmp3D1->f2;_tmp3D4=(struct Cyc_Core_Opt**)& _tmp3D1->f2;
_tmp3D5=_tmp3D1->f4;}}_LL21F: if(_tmp3D2 == 0  || ((struct Cyc_Absyn_Kind*)_tmp3D2->v)->kind
!= Cyc_Absyn_EffKind){const char*_tmp11F3;void*_tmp11F2;(_tmp11F2=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11F3="effect evar has wrong kind",
_tag_dyneither(_tmp11F3,sizeof(char),27))),_tag_dyneither(_tmp11F2,sizeof(void*),
0)));}if(!constrain)return 0;{void*_tmp3F1=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp3D5);Cyc_Tcutil_occurs(_tmp3F1,Cyc_Core_heap_region,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3D5))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmp1208;struct Cyc_List_List*_tmp1207;struct Cyc_Absyn_AccessEff_struct _tmp1206;
struct Cyc_Absyn_AccessEff_struct*_tmp1205;struct Cyc_List_List*_tmp1204;struct Cyc_Absyn_JoinEff_struct*
_tmp1203;struct Cyc_Absyn_JoinEff_struct*_tmp3F2=(_tmp1203=_cycalloc(sizeof(*
_tmp1203)),((_tmp1203[0]=((_tmp1208.tag=24,((_tmp1208.f1=((_tmp1204=_cycalloc(
sizeof(*_tmp1204)),((_tmp1204->hd=_tmp3F1,((_tmp1204->tl=((_tmp1207=_cycalloc(
sizeof(*_tmp1207)),((_tmp1207->hd=(void*)((_tmp1205=_cycalloc(sizeof(*_tmp1205)),((
_tmp1205[0]=((_tmp1206.tag=23,((_tmp1206.f1=(void*)r,_tmp1206)))),_tmp1205)))),((
_tmp1207->tl=0,_tmp1207)))))),_tmp1204)))))),_tmp1208)))),_tmp1203)));{struct Cyc_Core_Opt*
_tmp1209;*_tmp3D4=((_tmp1209=_cycalloc(sizeof(*_tmp1209)),((_tmp1209->v=(void*)
_tmp3F2,_tmp1209))));}return 1;};};_LL220:;_LL221: return 0;_LL217:;};}static int Cyc_Tcutil_type_in_effect(
int may_constrain_evars,void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp3FA=Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp3FD;void*_tmp3FF;struct Cyc_Core_Opt*
_tmp401;struct Cyc_Core_Opt*_tmp402;struct Cyc_Core_Opt**_tmp403;struct Cyc_Core_Opt*
_tmp404;_LL232: {struct Cyc_Absyn_AccessEff_struct*_tmp3FB=(struct Cyc_Absyn_AccessEff_struct*)
_tmp3FA;if(_tmp3FB->tag != 23)goto _LL234;}_LL233: return 0;_LL234: {struct Cyc_Absyn_JoinEff_struct*
_tmp3FC=(struct Cyc_Absyn_JoinEff_struct*)_tmp3FA;if(_tmp3FC->tag != 24)goto _LL236;
else{_tmp3FD=_tmp3FC->f1;}}_LL235: for(0;_tmp3FD != 0;_tmp3FD=_tmp3FD->tl){if(Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,(void*)_tmp3FD->hd))return 1;}return 0;_LL236: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3FE=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3FA;if(_tmp3FE->tag != 25)goto _LL238;
else{_tmp3FF=(void*)_tmp3FE->f1;}}_LL237: _tmp3FF=Cyc_Tcutil_compress(_tmp3FF);
if(t == _tmp3FF)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp3FF);{
void*_tmp405=Cyc_Tcutil_rgns_of(t);void*_tmp407;_LL23D: {struct Cyc_Absyn_RgnsEff_struct*
_tmp406=(struct Cyc_Absyn_RgnsEff_struct*)_tmp405;if(_tmp406->tag != 25)goto _LL23F;
else{_tmp407=(void*)_tmp406->f1;}}_LL23E: {struct _tuple0 _tmp120A;struct _tuple0
_tmp409=(_tmp120A.f1=Cyc_Tcutil_compress(_tmp407),((_tmp120A.f2=_tmp3FF,_tmp120A)));
void*_tmp40A;struct Cyc_Absyn_Tvar*_tmp40C;void*_tmp40D;struct Cyc_Absyn_Tvar*
_tmp40F;_LL242: _tmp40A=_tmp409.f1;{struct Cyc_Absyn_VarType_struct*_tmp40B=(
struct Cyc_Absyn_VarType_struct*)_tmp40A;if(_tmp40B->tag != 2)goto _LL244;else{
_tmp40C=_tmp40B->f1;}};_tmp40D=_tmp409.f2;{struct Cyc_Absyn_VarType_struct*
_tmp40E=(struct Cyc_Absyn_VarType_struct*)_tmp40D;if(_tmp40E->tag != 2)goto _LL244;
else{_tmp40F=_tmp40E->f1;}};_LL243: return Cyc_Tcutil_unify(t,_tmp3FF);_LL244:;
_LL245: return _tmp407 == _tmp3FF;_LL241:;}_LL23F:;_LL240: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,_tmp405);_LL23C:;};_LL238: {struct Cyc_Absyn_Evar_struct*
_tmp400=(struct Cyc_Absyn_Evar_struct*)_tmp3FA;if(_tmp400->tag != 1)goto _LL23A;
else{_tmp401=_tmp400->f1;_tmp402=_tmp400->f2;_tmp403=(struct Cyc_Core_Opt**)&
_tmp400->f2;_tmp404=_tmp400->f4;}}_LL239: if(_tmp401 == 0  || ((struct Cyc_Absyn_Kind*)
_tmp401->v)->kind != Cyc_Absyn_EffKind){const char*_tmp120D;void*_tmp120C;(
_tmp120C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp120D="effect evar has wrong kind",_tag_dyneither(_tmp120D,sizeof(char),27))),
_tag_dyneither(_tmp120C,sizeof(void*),0)));}if(!may_constrain_evars)return 0;{
void*_tmp412=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp404);
Cyc_Tcutil_occurs(_tmp412,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp404))->v,t);{struct Cyc_Absyn_JoinEff_struct _tmp1222;struct Cyc_List_List*
_tmp1221;struct Cyc_Absyn_RgnsEff_struct _tmp1220;struct Cyc_Absyn_RgnsEff_struct*
_tmp121F;struct Cyc_List_List*_tmp121E;struct Cyc_Absyn_JoinEff_struct*_tmp121D;
struct Cyc_Absyn_JoinEff_struct*_tmp413=(_tmp121D=_cycalloc(sizeof(*_tmp121D)),((
_tmp121D[0]=((_tmp1222.tag=24,((_tmp1222.f1=((_tmp121E=_cycalloc(sizeof(*
_tmp121E)),((_tmp121E->hd=_tmp412,((_tmp121E->tl=((_tmp1221=_cycalloc(sizeof(*
_tmp1221)),((_tmp1221->hd=(void*)((_tmp121F=_cycalloc(sizeof(*_tmp121F)),((
_tmp121F[0]=((_tmp1220.tag=25,((_tmp1220.f1=(void*)t,_tmp1220)))),_tmp121F)))),((
_tmp1221->tl=0,_tmp1221)))))),_tmp121E)))))),_tmp1222)))),_tmp121D)));{struct Cyc_Core_Opt*
_tmp1223;*_tmp403=((_tmp1223=_cycalloc(sizeof(*_tmp1223)),((_tmp1223->v=(void*)
_tmp413,_tmp1223))));}return 1;};};_LL23A:;_LL23B: return 0;_LL231:;};}static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{
void*_tmp41B=e;struct Cyc_Absyn_Tvar*_tmp41D;struct Cyc_List_List*_tmp41F;void*
_tmp421;struct Cyc_Core_Opt*_tmp423;struct Cyc_Core_Opt*_tmp424;struct Cyc_Core_Opt**
_tmp425;struct Cyc_Core_Opt*_tmp426;_LL247: {struct Cyc_Absyn_VarType_struct*
_tmp41C=(struct Cyc_Absyn_VarType_struct*)_tmp41B;if(_tmp41C->tag != 2)goto _LL249;
else{_tmp41D=_tmp41C->f1;}}_LL248: return Cyc_Absyn_tvar_cmp(v,_tmp41D)== 0;_LL249: {
struct Cyc_Absyn_JoinEff_struct*_tmp41E=(struct Cyc_Absyn_JoinEff_struct*)_tmp41B;
if(_tmp41E->tag != 24)goto _LL24B;else{_tmp41F=_tmp41E->f1;}}_LL24A: for(0;_tmp41F
!= 0;_tmp41F=_tmp41F->tl){if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(
void*)_tmp41F->hd))return 1;}return 0;_LL24B: {struct Cyc_Absyn_RgnsEff_struct*
_tmp420=(struct Cyc_Absyn_RgnsEff_struct*)_tmp41B;if(_tmp420->tag != 25)goto _LL24D;
else{_tmp421=(void*)_tmp420->f1;}}_LL24C: {void*_tmp427=Cyc_Tcutil_rgns_of(
_tmp421);void*_tmp429;_LL252: {struct Cyc_Absyn_RgnsEff_struct*_tmp428=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp427;if(_tmp428->tag != 25)goto _LL254;else{_tmp429=(void*)_tmp428->f1;}}_LL253:
if(!may_constrain_evars)return 0;{void*_tmp42A=Cyc_Tcutil_compress(_tmp429);
struct Cyc_Core_Opt*_tmp42C;struct Cyc_Core_Opt*_tmp42D;struct Cyc_Core_Opt**
_tmp42E;struct Cyc_Core_Opt*_tmp42F;_LL257: {struct Cyc_Absyn_Evar_struct*_tmp42B=(
struct Cyc_Absyn_Evar_struct*)_tmp42A;if(_tmp42B->tag != 1)goto _LL259;else{_tmp42C=
_tmp42B->f1;_tmp42D=_tmp42B->f2;_tmp42E=(struct Cyc_Core_Opt**)& _tmp42B->f2;
_tmp42F=_tmp42B->f4;}}_LL258: {void*_tmp430=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp42F);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp42F))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_struct _tmp1232;struct Cyc_Absyn_VarType_struct*_tmp1231;
struct Cyc_Absyn_VarType_struct _tmp1230;void*_tmp122F[2];struct Cyc_Absyn_JoinEff_struct*
_tmp122E;void*_tmp431=Cyc_Tcutil_dummy_fntype((void*)((_tmp122E=_cycalloc(
sizeof(*_tmp122E)),((_tmp122E[0]=((_tmp1232.tag=24,((_tmp1232.f1=((_tmp122F[1]=(
void*)((_tmp1231=_cycalloc(sizeof(*_tmp1231)),((_tmp1231[0]=((_tmp1230.tag=2,((
_tmp1230.f1=v,_tmp1230)))),_tmp1231)))),((_tmp122F[0]=_tmp430,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp122F,sizeof(void*),2)))))),
_tmp1232)))),_tmp122E)))));{struct Cyc_Core_Opt*_tmp1233;*_tmp42E=((_tmp1233=
_cycalloc(sizeof(*_tmp1233)),((_tmp1233->v=_tmp431,_tmp1233))));}return 1;};}
_LL259:;_LL25A: return 0;_LL256:;};_LL254:;_LL255: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp427);_LL251:;}_LL24D: {struct Cyc_Absyn_Evar_struct*
_tmp422=(struct Cyc_Absyn_Evar_struct*)_tmp41B;if(_tmp422->tag != 1)goto _LL24F;
else{_tmp423=_tmp422->f1;_tmp424=_tmp422->f2;_tmp425=(struct Cyc_Core_Opt**)&
_tmp422->f2;_tmp426=_tmp422->f4;}}_LL24E: if(_tmp423 == 0  || ((struct Cyc_Absyn_Kind*)
_tmp423->v)->kind != Cyc_Absyn_EffKind){const char*_tmp1236;void*_tmp1235;(
_tmp1235=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1236="effect evar has wrong kind",_tag_dyneither(_tmp1236,sizeof(char),27))),
_tag_dyneither(_tmp1235,sizeof(void*),0)));}{void*_tmp43A=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp426);if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp426))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct _tmp124B;
struct Cyc_List_List*_tmp124A;struct Cyc_Absyn_VarType_struct _tmp1249;struct Cyc_Absyn_VarType_struct*
_tmp1248;struct Cyc_List_List*_tmp1247;struct Cyc_Absyn_JoinEff_struct*_tmp1246;
struct Cyc_Absyn_JoinEff_struct*_tmp43B=(_tmp1246=_cycalloc(sizeof(*_tmp1246)),((
_tmp1246[0]=((_tmp124B.tag=24,((_tmp124B.f1=((_tmp1247=_cycalloc(sizeof(*
_tmp1247)),((_tmp1247->hd=_tmp43A,((_tmp1247->tl=((_tmp124A=_cycalloc(sizeof(*
_tmp124A)),((_tmp124A->hd=(void*)((_tmp1248=_cycalloc(sizeof(*_tmp1248)),((
_tmp1248[0]=((_tmp1249.tag=2,((_tmp1249.f1=v,_tmp1249)))),_tmp1248)))),((
_tmp124A->tl=0,_tmp124A)))))),_tmp1247)))))),_tmp124B)))),_tmp1246)));{struct Cyc_Core_Opt*
_tmp124C;*_tmp425=((_tmp124C=_cycalloc(sizeof(*_tmp124C)),((_tmp124C->v=(void*)
_tmp43B,_tmp124C))));}return 1;};};_LL24F:;_LL250: return 0;_LL246:;};}static int Cyc_Tcutil_evar_in_effect(
void*evar,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp443=e;struct Cyc_List_List*
_tmp445;void*_tmp447;_LL25C: {struct Cyc_Absyn_JoinEff_struct*_tmp444=(struct Cyc_Absyn_JoinEff_struct*)
_tmp443;if(_tmp444->tag != 24)goto _LL25E;else{_tmp445=_tmp444->f1;}}_LL25D: for(0;
_tmp445 != 0;_tmp445=_tmp445->tl){if(Cyc_Tcutil_evar_in_effect(evar,(void*)
_tmp445->hd))return 1;}return 0;_LL25E: {struct Cyc_Absyn_RgnsEff_struct*_tmp446=(
struct Cyc_Absyn_RgnsEff_struct*)_tmp443;if(_tmp446->tag != 25)goto _LL260;else{
_tmp447=(void*)_tmp446->f1;}}_LL25F: {void*_tmp449=Cyc_Tcutil_rgns_of(_tmp447);
void*_tmp44B;_LL265: {struct Cyc_Absyn_RgnsEff_struct*_tmp44A=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp449;if(_tmp44A->tag != 25)goto _LL267;else{_tmp44B=(void*)_tmp44A->f1;}}_LL266:
return 0;_LL267:;_LL268: return Cyc_Tcutil_evar_in_effect(evar,_tmp449);_LL264:;}
_LL260: {struct Cyc_Absyn_Evar_struct*_tmp448=(struct Cyc_Absyn_Evar_struct*)
_tmp443;if(_tmp448->tag != 1)goto _LL262;}_LL261: return evar == e;_LL262:;_LL263:
return 0;_LL25B:;};}int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,
void*e2){void*_tmp44C=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp44E;void*
_tmp450;struct Cyc_Absyn_Tvar*_tmp452;void*_tmp454;struct Cyc_Core_Opt*_tmp456;
struct Cyc_Core_Opt**_tmp457;struct Cyc_Core_Opt*_tmp458;_LL26A: {struct Cyc_Absyn_JoinEff_struct*
_tmp44D=(struct Cyc_Absyn_JoinEff_struct*)_tmp44C;if(_tmp44D->tag != 24)goto _LL26C;
else{_tmp44E=_tmp44D->f1;}}_LL26B: for(0;_tmp44E != 0;_tmp44E=_tmp44E->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp44E->hd,e2))return 0;}return 1;_LL26C: {struct Cyc_Absyn_AccessEff_struct*
_tmp44F=(struct Cyc_Absyn_AccessEff_struct*)_tmp44C;if(_tmp44F->tag != 23)goto
_LL26E;else{_tmp450=(void*)_tmp44F->f1;}}_LL26D: return Cyc_Tcutil_region_in_effect(
0,_tmp450,e2) || may_constrain_evars  && Cyc_Tcutil_unify(_tmp450,(void*)& Cyc_Absyn_HeapRgn_val);
_LL26E: {struct Cyc_Absyn_VarType_struct*_tmp451=(struct Cyc_Absyn_VarType_struct*)
_tmp44C;if(_tmp451->tag != 2)goto _LL270;else{_tmp452=_tmp451->f1;}}_LL26F: return
Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp452,e2);_LL270: {struct Cyc_Absyn_RgnsEff_struct*
_tmp453=(struct Cyc_Absyn_RgnsEff_struct*)_tmp44C;if(_tmp453->tag != 25)goto _LL272;
else{_tmp454=(void*)_tmp453->f1;}}_LL271: {void*_tmp459=Cyc_Tcutil_rgns_of(
_tmp454);void*_tmp45B;_LL277: {struct Cyc_Absyn_RgnsEff_struct*_tmp45A=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp459;if(_tmp45A->tag != 25)goto _LL279;else{_tmp45B=(void*)_tmp45A->f1;}}_LL278:
return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp45B,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp45B,Cyc_Absyn_sint_typ);_LL279:;
_LL27A: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp459,e2);_LL276:;}
_LL272: {struct Cyc_Absyn_Evar_struct*_tmp455=(struct Cyc_Absyn_Evar_struct*)
_tmp44C;if(_tmp455->tag != 1)goto _LL274;else{_tmp456=_tmp455->f2;_tmp457=(struct
Cyc_Core_Opt**)& _tmp455->f2;_tmp458=_tmp455->f4;}}_LL273: if(!Cyc_Tcutil_evar_in_effect(
e1,e2)){struct Cyc_Core_Opt*_tmp124D;*_tmp457=((_tmp124D=_cycalloc(sizeof(*
_tmp124D)),((_tmp124D->v=Cyc_Absyn_empty_effect,_tmp124D))));}return 1;_LL274:;
_LL275: {const char*_tmp1251;void*_tmp1250[1];struct Cyc_String_pa_struct _tmp124F;(
_tmp124F.tag=0,((_tmp124F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e1)),((_tmp1250[0]=& _tmp124F,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1251="subset_effect: bad effect: %s",
_tag_dyneither(_tmp1251,sizeof(char),30))),_tag_dyneither(_tmp1250,sizeof(void*),
1)))))));}_LL269:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && 
Cyc_Tcutil_subset_effect(0,e2,e1))return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2)
 && Cyc_Tcutil_subset_effect(1,e2,e1))return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;
for(0;r1 != 0;r1=r1->tl){struct _tuple0 _tmp461;void*_tmp462;void*_tmp463;struct
_tuple0*_tmp460=(struct _tuple0*)r1->hd;_tmp461=*_tmp460;_tmp462=_tmp461.f1;
_tmp463=_tmp461.f2;{int found=_tmp462 == (void*)& Cyc_Absyn_HeapRgn_val;{struct Cyc_List_List*
r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){struct _tuple0 _tmp465;void*_tmp466;void*
_tmp467;struct _tuple0*_tmp464=(struct _tuple0*)r2->hd;_tmp465=*_tmp464;_tmp466=
_tmp465.f1;_tmp467=_tmp465.f2;if(Cyc_Tcutil_unify(_tmp462,_tmp466) && Cyc_Tcutil_unify(
_tmp463,_tmp467)){found=1;break;}}}if(!found)return 0;};}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple21{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple22{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)
_tag_dyneither(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(
t1 == t2)return;{void*_tmp468=t1;struct Cyc_Core_Opt*_tmp46A;struct Cyc_Core_Opt*
_tmp46B;struct Cyc_Core_Opt**_tmp46C;struct Cyc_Core_Opt*_tmp46D;_LL27C: {struct
Cyc_Absyn_Evar_struct*_tmp469=(struct Cyc_Absyn_Evar_struct*)_tmp468;if(_tmp469->tag
!= 1)goto _LL27E;else{_tmp46A=_tmp469->f1;_tmp46B=_tmp469->f2;_tmp46C=(struct Cyc_Core_Opt**)&
_tmp469->f2;_tmp46D=_tmp469->f4;}}_LL27D: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp46D))->v,t2);{struct
Cyc_Absyn_Kind*_tmp46E=Cyc_Tcutil_typ_kind(t2);if(Cyc_Tcutil_kind_leq(_tmp46E,(
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp46A))->v)){{struct
Cyc_Core_Opt*_tmp1252;*_tmp46C=((_tmp1252=_cycalloc(sizeof(*_tmp1252)),((
_tmp1252->v=t2,_tmp1252))));}return;}else{{void*_tmp470=t2;struct Cyc_Core_Opt*
_tmp472;struct Cyc_Core_Opt**_tmp473;struct Cyc_Core_Opt*_tmp474;struct Cyc_Absyn_PtrInfo
_tmp476;_LL281: {struct Cyc_Absyn_Evar_struct*_tmp471=(struct Cyc_Absyn_Evar_struct*)
_tmp470;if(_tmp471->tag != 1)goto _LL283;else{_tmp472=_tmp471->f2;_tmp473=(struct
Cyc_Core_Opt**)& _tmp471->f2;_tmp474=_tmp471->f4;}}_LL282: {struct Cyc_List_List*
_tmp477=(struct Cyc_List_List*)_tmp46D->v;{struct Cyc_List_List*s2=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp474))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp477,(struct
Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmp1253;Cyc_Tcutil_failure_reason=((
_tmp1253="(type variable would escape scope)",_tag_dyneither(_tmp1253,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}if(Cyc_Tcutil_kind_leq((
struct Cyc_Absyn_Kind*)_tmp46A->v,_tmp46E)){{struct Cyc_Core_Opt*_tmp1254;*_tmp473=((
_tmp1254=_cycalloc(sizeof(*_tmp1254)),((_tmp1254->v=t1,_tmp1254))));}return;}{
const char*_tmp1255;Cyc_Tcutil_failure_reason=((_tmp1255="(kinds are incompatible)",
_tag_dyneither(_tmp1255,sizeof(char),25)));}goto _LL280;}_LL283:{struct Cyc_Absyn_PointerType_struct*
_tmp475=(struct Cyc_Absyn_PointerType_struct*)_tmp470;if(_tmp475->tag != 5)goto
_LL285;else{_tmp476=_tmp475->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp46A->v)->kind
== Cyc_Absyn_BoxKind))goto _LL285;_LL284: {union Cyc_Absyn_Constraint*_tmp47B=((
union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((
_tmp476.ptr_atts).bounds);{union Cyc_Absyn_Constraint*_tmp47C=_tmp47B;union Cyc_Absyn_Constraint
_tmp47D;int _tmp47E;_LL288: _tmp47D=*_tmp47C;if((_tmp47D.No_constr).tag != 3)goto
_LL28A;_tmp47E=(int)(_tmp47D.No_constr).val;_LL289:{struct
_union_Constraint_Eq_constr*_tmp1256;(_tmp1256=& _tmp47B->Eq_constr,((_tmp1256->tag=
1,_tmp1256->val=Cyc_Absyn_bounds_one)));}{struct Cyc_Core_Opt*_tmp1257;*_tmp46C=((
_tmp1257=_cycalloc(sizeof(*_tmp1257)),((_tmp1257->v=t2,_tmp1257))));}return;
_LL28A:;_LL28B: goto _LL287;_LL287:;}goto _LL280;}_LL285:;_LL286: goto _LL280;_LL280:;}{
const char*_tmp1258;Cyc_Tcutil_failure_reason=((_tmp1258="(kinds are incompatible)",
_tag_dyneither(_tmp1258,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};
_LL27E:;_LL27F: goto _LL27B;_LL27B:;}{struct _tuple0 _tmp1259;struct _tuple0 _tmp483=(
_tmp1259.f1=t2,((_tmp1259.f2=t1,_tmp1259)));void*_tmp484;void*_tmp486;void*
_tmp488;void*_tmp48A;struct Cyc_Absyn_Tvar*_tmp48C;void*_tmp48D;struct Cyc_Absyn_Tvar*
_tmp48F;void*_tmp490;struct Cyc_Absyn_AggrInfo _tmp492;union Cyc_Absyn_AggrInfoU
_tmp493;struct Cyc_List_List*_tmp494;void*_tmp495;struct Cyc_Absyn_AggrInfo _tmp497;
union Cyc_Absyn_AggrInfoU _tmp498;struct Cyc_List_List*_tmp499;void*_tmp49A;struct
_tuple2*_tmp49C;void*_tmp49D;struct _tuple2*_tmp49F;void*_tmp4A0;struct Cyc_List_List*
_tmp4A2;void*_tmp4A3;struct Cyc_List_List*_tmp4A5;void*_tmp4A6;struct Cyc_Absyn_DatatypeInfo
_tmp4A8;union Cyc_Absyn_DatatypeInfoU _tmp4A9;struct Cyc_Absyn_Datatypedecl**
_tmp4AA;struct Cyc_Absyn_Datatypedecl*_tmp4AB;struct Cyc_List_List*_tmp4AC;void*
_tmp4AD;struct Cyc_Absyn_DatatypeInfo _tmp4AF;union Cyc_Absyn_DatatypeInfoU _tmp4B0;
struct Cyc_Absyn_Datatypedecl**_tmp4B1;struct Cyc_Absyn_Datatypedecl*_tmp4B2;
struct Cyc_List_List*_tmp4B3;void*_tmp4B4;struct Cyc_Absyn_DatatypeFieldInfo
_tmp4B6;union Cyc_Absyn_DatatypeFieldInfoU _tmp4B7;struct _tuple3 _tmp4B8;struct Cyc_Absyn_Datatypedecl*
_tmp4B9;struct Cyc_Absyn_Datatypefield*_tmp4BA;struct Cyc_List_List*_tmp4BB;void*
_tmp4BC;struct Cyc_Absyn_DatatypeFieldInfo _tmp4BE;union Cyc_Absyn_DatatypeFieldInfoU
_tmp4BF;struct _tuple3 _tmp4C0;struct Cyc_Absyn_Datatypedecl*_tmp4C1;struct Cyc_Absyn_Datatypefield*
_tmp4C2;struct Cyc_List_List*_tmp4C3;void*_tmp4C4;struct Cyc_Absyn_PtrInfo _tmp4C6;
void*_tmp4C7;struct Cyc_Absyn_Tqual _tmp4C8;struct Cyc_Absyn_PtrAtts _tmp4C9;void*
_tmp4CA;union Cyc_Absyn_Constraint*_tmp4CB;union Cyc_Absyn_Constraint*_tmp4CC;
union Cyc_Absyn_Constraint*_tmp4CD;void*_tmp4CE;struct Cyc_Absyn_PtrInfo _tmp4D0;
void*_tmp4D1;struct Cyc_Absyn_Tqual _tmp4D2;struct Cyc_Absyn_PtrAtts _tmp4D3;void*
_tmp4D4;union Cyc_Absyn_Constraint*_tmp4D5;union Cyc_Absyn_Constraint*_tmp4D6;
union Cyc_Absyn_Constraint*_tmp4D7;void*_tmp4D8;enum Cyc_Absyn_Sign _tmp4DA;enum 
Cyc_Absyn_Size_of _tmp4DB;void*_tmp4DC;enum Cyc_Absyn_Sign _tmp4DE;enum Cyc_Absyn_Size_of
_tmp4DF;void*_tmp4E0;void*_tmp4E2;void*_tmp4E4;int _tmp4E6;void*_tmp4E7;int
_tmp4E9;void*_tmp4EA;void*_tmp4EC;void*_tmp4ED;void*_tmp4EF;void*_tmp4F0;struct
Cyc_Absyn_Exp*_tmp4F2;void*_tmp4F3;struct Cyc_Absyn_Exp*_tmp4F5;void*_tmp4F6;
struct Cyc_Absyn_ArrayInfo _tmp4F8;void*_tmp4F9;struct Cyc_Absyn_Tqual _tmp4FA;
struct Cyc_Absyn_Exp*_tmp4FB;union Cyc_Absyn_Constraint*_tmp4FC;void*_tmp4FD;
struct Cyc_Absyn_ArrayInfo _tmp4FF;void*_tmp500;struct Cyc_Absyn_Tqual _tmp501;
struct Cyc_Absyn_Exp*_tmp502;union Cyc_Absyn_Constraint*_tmp503;void*_tmp504;
struct Cyc_Absyn_FnInfo _tmp506;struct Cyc_List_List*_tmp507;struct Cyc_Core_Opt*
_tmp508;struct Cyc_Absyn_Tqual _tmp509;void*_tmp50A;struct Cyc_List_List*_tmp50B;
int _tmp50C;struct Cyc_Absyn_VarargInfo*_tmp50D;struct Cyc_List_List*_tmp50E;struct
Cyc_List_List*_tmp50F;void*_tmp510;struct Cyc_Absyn_FnInfo _tmp512;struct Cyc_List_List*
_tmp513;struct Cyc_Core_Opt*_tmp514;struct Cyc_Absyn_Tqual _tmp515;void*_tmp516;
struct Cyc_List_List*_tmp517;int _tmp518;struct Cyc_Absyn_VarargInfo*_tmp519;struct
Cyc_List_List*_tmp51A;struct Cyc_List_List*_tmp51B;void*_tmp51C;struct Cyc_List_List*
_tmp51E;void*_tmp51F;struct Cyc_List_List*_tmp521;void*_tmp522;enum Cyc_Absyn_AggrKind
_tmp524;struct Cyc_List_List*_tmp525;void*_tmp526;enum Cyc_Absyn_AggrKind _tmp528;
struct Cyc_List_List*_tmp529;void*_tmp52A;void*_tmp52C;void*_tmp52E;void*_tmp530;
void*_tmp532;void*_tmp534;void*_tmp535;void*_tmp537;void*_tmp538;void*_tmp53A;
void*_tmp53B;void*_tmp53C;void*_tmp53E;void*_tmp53F;void*_tmp540;void*_tmp542;
void*_tmp544;void*_tmp546;void*_tmp548;void*_tmp54A;_LL28D: _tmp484=_tmp483.f1;{
struct Cyc_Absyn_Evar_struct*_tmp485=(struct Cyc_Absyn_Evar_struct*)_tmp484;if(
_tmp485->tag != 1)goto _LL28F;};_LL28E: Cyc_Tcutil_unify_it(t2,t1);return;_LL28F:
_tmp486=_tmp483.f1;{struct Cyc_Absyn_VoidType_struct*_tmp487=(struct Cyc_Absyn_VoidType_struct*)
_tmp486;if(_tmp487->tag != 0)goto _LL291;};_tmp488=_tmp483.f2;{struct Cyc_Absyn_VoidType_struct*
_tmp489=(struct Cyc_Absyn_VoidType_struct*)_tmp488;if(_tmp489->tag != 0)goto _LL291;};
_LL290: return;_LL291: _tmp48A=_tmp483.f1;{struct Cyc_Absyn_VarType_struct*_tmp48B=(
struct Cyc_Absyn_VarType_struct*)_tmp48A;if(_tmp48B->tag != 2)goto _LL293;else{
_tmp48C=_tmp48B->f1;}};_tmp48D=_tmp483.f2;{struct Cyc_Absyn_VarType_struct*
_tmp48E=(struct Cyc_Absyn_VarType_struct*)_tmp48D;if(_tmp48E->tag != 2)goto _LL293;
else{_tmp48F=_tmp48E->f1;}};_LL292: {struct _dyneither_ptr*_tmp54C=_tmp48C->name;
struct _dyneither_ptr*_tmp54D=_tmp48F->name;int _tmp54E=_tmp48C->identity;int
_tmp54F=_tmp48F->identity;if(_tmp54F == _tmp54E)return;{const char*_tmp125A;Cyc_Tcutil_failure_reason=((
_tmp125A="(variable types are not the same)",_tag_dyneither(_tmp125A,sizeof(char),
34)));}goto _LL28C;}_LL293: _tmp490=_tmp483.f1;{struct Cyc_Absyn_AggrType_struct*
_tmp491=(struct Cyc_Absyn_AggrType_struct*)_tmp490;if(_tmp491->tag != 12)goto
_LL295;else{_tmp492=_tmp491->f1;_tmp493=_tmp492.aggr_info;_tmp494=_tmp492.targs;}};
_tmp495=_tmp483.f2;{struct Cyc_Absyn_AggrType_struct*_tmp496=(struct Cyc_Absyn_AggrType_struct*)
_tmp495;if(_tmp496->tag != 12)goto _LL295;else{_tmp497=_tmp496->f1;_tmp498=_tmp497.aggr_info;
_tmp499=_tmp497.targs;}};_LL294: {enum Cyc_Absyn_AggrKind _tmp552;struct _tuple2*
_tmp553;struct _tuple12 _tmp551=Cyc_Absyn_aggr_kinded_name(_tmp498);_tmp552=
_tmp551.f1;_tmp553=_tmp551.f2;{enum Cyc_Absyn_AggrKind _tmp555;struct _tuple2*
_tmp556;struct _tuple12 _tmp554=Cyc_Absyn_aggr_kinded_name(_tmp493);_tmp555=
_tmp554.f1;_tmp556=_tmp554.f2;if(_tmp552 != _tmp555){{const char*_tmp125B;Cyc_Tcutil_failure_reason=((
_tmp125B="(struct and union type)",_tag_dyneither(_tmp125B,sizeof(char),24)));}
goto _LL28C;}if(Cyc_Absyn_qvar_cmp(_tmp553,_tmp556)!= 0){{const char*_tmp125C;Cyc_Tcutil_failure_reason=((
_tmp125C="(different type name)",_tag_dyneither(_tmp125C,sizeof(char),22)));}
goto _LL28C;}Cyc_Tcutil_unify_list(_tmp499,_tmp494);return;};}_LL295: _tmp49A=
_tmp483.f1;{struct Cyc_Absyn_EnumType_struct*_tmp49B=(struct Cyc_Absyn_EnumType_struct*)
_tmp49A;if(_tmp49B->tag != 14)goto _LL297;else{_tmp49C=_tmp49B->f1;}};_tmp49D=
_tmp483.f2;{struct Cyc_Absyn_EnumType_struct*_tmp49E=(struct Cyc_Absyn_EnumType_struct*)
_tmp49D;if(_tmp49E->tag != 14)goto _LL297;else{_tmp49F=_tmp49E->f1;}};_LL296: if(
Cyc_Absyn_qvar_cmp(_tmp49C,_tmp49F)== 0)return;{const char*_tmp125D;Cyc_Tcutil_failure_reason=((
_tmp125D="(different enum types)",_tag_dyneither(_tmp125D,sizeof(char),23)));}
goto _LL28C;_LL297: _tmp4A0=_tmp483.f1;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp4A1=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp4A0;if(_tmp4A1->tag != 15)goto
_LL299;else{_tmp4A2=_tmp4A1->f1;}};_tmp4A3=_tmp483.f2;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp4A4=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp4A3;if(_tmp4A4->tag != 15)goto
_LL299;else{_tmp4A5=_tmp4A4->f1;}};_LL298: {int bad=0;for(0;_tmp4A2 != 0  && 
_tmp4A5 != 0;(_tmp4A2=_tmp4A2->tl,_tmp4A5=_tmp4A5->tl)){struct Cyc_Absyn_Enumfield*
_tmp55A=(struct Cyc_Absyn_Enumfield*)_tmp4A2->hd;struct Cyc_Absyn_Enumfield*
_tmp55B=(struct Cyc_Absyn_Enumfield*)_tmp4A5->hd;if(Cyc_Absyn_qvar_cmp(_tmp55A->name,
_tmp55B->name)!= 0){{const char*_tmp125E;Cyc_Tcutil_failure_reason=((_tmp125E="(different names for enum fields)",
_tag_dyneither(_tmp125E,sizeof(char),34)));}bad=1;break;}if(_tmp55A->tag == 
_tmp55B->tag)continue;if(_tmp55A->tag == 0  || _tmp55B->tag == 0){{const char*
_tmp125F;Cyc_Tcutil_failure_reason=((_tmp125F="(different tag values for enum fields)",
_tag_dyneither(_tmp125F,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp55A->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp55B->tag))){{const char*_tmp1260;Cyc_Tcutil_failure_reason=((_tmp1260="(different tag values for enum fields)",
_tag_dyneither(_tmp1260,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL28C;if(
_tmp4A2 == 0  && _tmp4A5 == 0)return;{const char*_tmp1261;Cyc_Tcutil_failure_reason=((
_tmp1261="(different number of fields for enums)",_tag_dyneither(_tmp1261,
sizeof(char),39)));}goto _LL28C;}_LL299: _tmp4A6=_tmp483.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp4A7=(struct Cyc_Absyn_DatatypeType_struct*)_tmp4A6;if(_tmp4A7->tag != 3)goto
_LL29B;else{_tmp4A8=_tmp4A7->f1;_tmp4A9=_tmp4A8.datatype_info;if((_tmp4A9.KnownDatatype).tag
!= 2)goto _LL29B;_tmp4AA=(struct Cyc_Absyn_Datatypedecl**)(_tmp4A9.KnownDatatype).val;
_tmp4AB=*_tmp4AA;_tmp4AC=_tmp4A8.targs;}};_tmp4AD=_tmp483.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp4AE=(struct Cyc_Absyn_DatatypeType_struct*)_tmp4AD;if(_tmp4AE->tag != 3)goto
_LL29B;else{_tmp4AF=_tmp4AE->f1;_tmp4B0=_tmp4AF.datatype_info;if((_tmp4B0.KnownDatatype).tag
!= 2)goto _LL29B;_tmp4B1=(struct Cyc_Absyn_Datatypedecl**)(_tmp4B0.KnownDatatype).val;
_tmp4B2=*_tmp4B1;_tmp4B3=_tmp4AF.targs;}};_LL29A: if(_tmp4AB == _tmp4B2  || Cyc_Absyn_qvar_cmp(
_tmp4AB->name,_tmp4B2->name)== 0){Cyc_Tcutil_unify_list(_tmp4B3,_tmp4AC);return;}
goto _LL28C;_LL29B: _tmp4B4=_tmp483.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp4B5=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp4B4;if(_tmp4B5->tag != 4)
goto _LL29D;else{_tmp4B6=_tmp4B5->f1;_tmp4B7=_tmp4B6.field_info;if((_tmp4B7.KnownDatatypefield).tag
!= 2)goto _LL29D;_tmp4B8=(struct _tuple3)(_tmp4B7.KnownDatatypefield).val;_tmp4B9=
_tmp4B8.f1;_tmp4BA=_tmp4B8.f2;_tmp4BB=_tmp4B6.targs;}};_tmp4BC=_tmp483.f2;{
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp4BD=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp4BC;if(_tmp4BD->tag != 4)goto _LL29D;else{_tmp4BE=_tmp4BD->f1;_tmp4BF=_tmp4BE.field_info;
if((_tmp4BF.KnownDatatypefield).tag != 2)goto _LL29D;_tmp4C0=(struct _tuple3)(
_tmp4BF.KnownDatatypefield).val;_tmp4C1=_tmp4C0.f1;_tmp4C2=_tmp4C0.f2;_tmp4C3=
_tmp4BE.targs;}};_LL29C: if((_tmp4B9 == _tmp4C1  || Cyc_Absyn_qvar_cmp(_tmp4B9->name,
_tmp4C1->name)== 0) && (_tmp4BA == _tmp4C2  || Cyc_Absyn_qvar_cmp(_tmp4BA->name,
_tmp4C2->name)== 0)){Cyc_Tcutil_unify_list(_tmp4C3,_tmp4BB);return;}{const char*
_tmp1262;Cyc_Tcutil_failure_reason=((_tmp1262="(different datatype field types)",
_tag_dyneither(_tmp1262,sizeof(char),33)));}goto _LL28C;_LL29D: _tmp4C4=_tmp483.f1;{
struct Cyc_Absyn_PointerType_struct*_tmp4C5=(struct Cyc_Absyn_PointerType_struct*)
_tmp4C4;if(_tmp4C5->tag != 5)goto _LL29F;else{_tmp4C6=_tmp4C5->f1;_tmp4C7=_tmp4C6.elt_typ;
_tmp4C8=_tmp4C6.elt_tq;_tmp4C9=_tmp4C6.ptr_atts;_tmp4CA=_tmp4C9.rgn;_tmp4CB=
_tmp4C9.nullable;_tmp4CC=_tmp4C9.bounds;_tmp4CD=_tmp4C9.zero_term;}};_tmp4CE=
_tmp483.f2;{struct Cyc_Absyn_PointerType_struct*_tmp4CF=(struct Cyc_Absyn_PointerType_struct*)
_tmp4CE;if(_tmp4CF->tag != 5)goto _LL29F;else{_tmp4D0=_tmp4CF->f1;_tmp4D1=_tmp4D0.elt_typ;
_tmp4D2=_tmp4D0.elt_tq;_tmp4D3=_tmp4D0.ptr_atts;_tmp4D4=_tmp4D3.rgn;_tmp4D5=
_tmp4D3.nullable;_tmp4D6=_tmp4D3.bounds;_tmp4D7=_tmp4D3.zero_term;}};_LL29E: Cyc_Tcutil_unify_it(
_tmp4D1,_tmp4C7);Cyc_Tcutil_unify_it(_tmp4CA,_tmp4D4);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{const char*_tmp1263;((void(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp4D7,_tmp4CD,((_tmp1263="(not both zero terminated)",
_tag_dyneither(_tmp1263,sizeof(char),27))));}Cyc_Tcutil_unify_tqual(_tmp4D2,
_tmp4D1,_tmp4C8,_tmp4C7);{const char*_tmp1264;((void(*)(int(*cmp)(void*,void*),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr
reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp4D6,_tmp4CC,((
_tmp1264="(different pointer bounds)",_tag_dyneither(_tmp1264,sizeof(char),27))));}{
void*_tmp563=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp4D6);_LL2C8: {struct Cyc_Absyn_DynEither_b_struct*
_tmp564=(struct Cyc_Absyn_DynEither_b_struct*)_tmp563;if(_tmp564->tag != 0)goto
_LL2CA;}_LL2C9: return;_LL2CA:;_LL2CB: goto _LL2C7;_LL2C7:;}{const char*_tmp1265;((
void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp4D5,_tmp4CB,((_tmp1265="(incompatible pointer types)",_tag_dyneither(
_tmp1265,sizeof(char),29))));}return;_LL29F: _tmp4D8=_tmp483.f1;{struct Cyc_Absyn_IntType_struct*
_tmp4D9=(struct Cyc_Absyn_IntType_struct*)_tmp4D8;if(_tmp4D9->tag != 6)goto _LL2A1;
else{_tmp4DA=_tmp4D9->f1;_tmp4DB=_tmp4D9->f2;}};_tmp4DC=_tmp483.f2;{struct Cyc_Absyn_IntType_struct*
_tmp4DD=(struct Cyc_Absyn_IntType_struct*)_tmp4DC;if(_tmp4DD->tag != 6)goto _LL2A1;
else{_tmp4DE=_tmp4DD->f1;_tmp4DF=_tmp4DD->f2;}};_LL2A0: if(_tmp4DE == _tmp4DA  && ((
_tmp4DF == _tmp4DB  || _tmp4DF == Cyc_Absyn_Int_sz  && _tmp4DB == Cyc_Absyn_Long_sz)
 || _tmp4DF == Cyc_Absyn_Long_sz  && _tmp4DB == Cyc_Absyn_Int_sz))return;{const char*
_tmp1266;Cyc_Tcutil_failure_reason=((_tmp1266="(different integral types)",
_tag_dyneither(_tmp1266,sizeof(char),27)));}goto _LL28C;_LL2A1: _tmp4E0=_tmp483.f1;{
struct Cyc_Absyn_FloatType_struct*_tmp4E1=(struct Cyc_Absyn_FloatType_struct*)
_tmp4E0;if(_tmp4E1->tag != 7)goto _LL2A3;};_tmp4E2=_tmp483.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp4E3=(struct Cyc_Absyn_FloatType_struct*)_tmp4E2;if(_tmp4E3->tag != 7)goto
_LL2A3;};_LL2A2: return;_LL2A3: _tmp4E4=_tmp483.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp4E5=(struct Cyc_Absyn_DoubleType_struct*)_tmp4E4;if(_tmp4E5->tag != 8)goto
_LL2A5;else{_tmp4E6=_tmp4E5->f1;}};_tmp4E7=_tmp483.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp4E8=(struct Cyc_Absyn_DoubleType_struct*)_tmp4E7;if(_tmp4E8->tag != 8)goto
_LL2A5;else{_tmp4E9=_tmp4E8->f1;}};_LL2A4: if(_tmp4E6 == _tmp4E9)return;goto _LL28C;
_LL2A5: _tmp4EA=_tmp483.f1;{struct Cyc_Absyn_TagType_struct*_tmp4EB=(struct Cyc_Absyn_TagType_struct*)
_tmp4EA;if(_tmp4EB->tag != 20)goto _LL2A7;else{_tmp4EC=(void*)_tmp4EB->f1;}};
_tmp4ED=_tmp483.f2;{struct Cyc_Absyn_TagType_struct*_tmp4EE=(struct Cyc_Absyn_TagType_struct*)
_tmp4ED;if(_tmp4EE->tag != 20)goto _LL2A7;else{_tmp4EF=(void*)_tmp4EE->f1;}};
_LL2A6: Cyc_Tcutil_unify_it(_tmp4EC,_tmp4EF);return;_LL2A7: _tmp4F0=_tmp483.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp4F1=(struct Cyc_Absyn_ValueofType_struct*)
_tmp4F0;if(_tmp4F1->tag != 19)goto _LL2A9;else{_tmp4F2=_tmp4F1->f1;}};_tmp4F3=
_tmp483.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp4F4=(struct Cyc_Absyn_ValueofType_struct*)
_tmp4F3;if(_tmp4F4->tag != 19)goto _LL2A9;else{_tmp4F5=_tmp4F4->f1;}};_LL2A8: if(!
Cyc_Evexp_same_const_exp(_tmp4F2,_tmp4F5)){{const char*_tmp1267;Cyc_Tcutil_failure_reason=((
_tmp1267="(cannot prove expressions are the same)",_tag_dyneither(_tmp1267,
sizeof(char),40)));}goto _LL28C;}return;_LL2A9: _tmp4F6=_tmp483.f1;{struct Cyc_Absyn_ArrayType_struct*
_tmp4F7=(struct Cyc_Absyn_ArrayType_struct*)_tmp4F6;if(_tmp4F7->tag != 9)goto
_LL2AB;else{_tmp4F8=_tmp4F7->f1;_tmp4F9=_tmp4F8.elt_type;_tmp4FA=_tmp4F8.tq;
_tmp4FB=_tmp4F8.num_elts;_tmp4FC=_tmp4F8.zero_term;}};_tmp4FD=_tmp483.f2;{struct
Cyc_Absyn_ArrayType_struct*_tmp4FE=(struct Cyc_Absyn_ArrayType_struct*)_tmp4FD;
if(_tmp4FE->tag != 9)goto _LL2AB;else{_tmp4FF=_tmp4FE->f1;_tmp500=_tmp4FF.elt_type;
_tmp501=_tmp4FF.tq;_tmp502=_tmp4FF.num_elts;_tmp503=_tmp4FF.zero_term;}};_LL2AA:
Cyc_Tcutil_unify_it(_tmp500,_tmp4F9);Cyc_Tcutil_unify_tqual(_tmp501,_tmp500,
_tmp4FA,_tmp4F9);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*
_tmp1268;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp4FC,_tmp503,((_tmp1268="(not both zero terminated)",_tag_dyneither(_tmp1268,
sizeof(char),27))));}if(_tmp4FB == _tmp502)return;if(_tmp4FB == 0  || _tmp502 == 0)
goto _LL28C;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp4FB,(struct Cyc_Absyn_Exp*)
_tmp502))return;{const char*_tmp1269;Cyc_Tcutil_failure_reason=((_tmp1269="(different array sizes)",
_tag_dyneither(_tmp1269,sizeof(char),24)));}goto _LL28C;_LL2AB: _tmp504=_tmp483.f1;{
struct Cyc_Absyn_FnType_struct*_tmp505=(struct Cyc_Absyn_FnType_struct*)_tmp504;
if(_tmp505->tag != 10)goto _LL2AD;else{_tmp506=_tmp505->f1;_tmp507=_tmp506.tvars;
_tmp508=_tmp506.effect;_tmp509=_tmp506.ret_tqual;_tmp50A=_tmp506.ret_typ;_tmp50B=
_tmp506.args;_tmp50C=_tmp506.c_varargs;_tmp50D=_tmp506.cyc_varargs;_tmp50E=
_tmp506.rgn_po;_tmp50F=_tmp506.attributes;}};_tmp510=_tmp483.f2;{struct Cyc_Absyn_FnType_struct*
_tmp511=(struct Cyc_Absyn_FnType_struct*)_tmp510;if(_tmp511->tag != 10)goto _LL2AD;
else{_tmp512=_tmp511->f1;_tmp513=_tmp512.tvars;_tmp514=_tmp512.effect;_tmp515=
_tmp512.ret_tqual;_tmp516=_tmp512.ret_typ;_tmp517=_tmp512.args;_tmp518=_tmp512.c_varargs;
_tmp519=_tmp512.cyc_varargs;_tmp51A=_tmp512.rgn_po;_tmp51B=_tmp512.attributes;}};
_LL2AC: {int done=0;struct _RegionHandle _tmp56A=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp56A;_push_region(rgn);{struct Cyc_List_List*inst=0;while(
_tmp513 != 0){if(_tmp507 == 0){{const char*_tmp126A;Cyc_Tcutil_failure_reason=((
_tmp126A="(second function type has too few type variables)",_tag_dyneither(
_tmp126A,sizeof(char),50)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{void*
_tmp56C=((struct Cyc_Absyn_Tvar*)_tmp513->hd)->kind;void*_tmp56D=((struct Cyc_Absyn_Tvar*)
_tmp507->hd)->kind;if(!Cyc_Tcutil_unify_kindbound(_tmp56C,_tmp56D)){{const char*
_tmp1270;void*_tmp126F[3];struct Cyc_String_pa_struct _tmp126E;struct Cyc_String_pa_struct
_tmp126D;struct Cyc_String_pa_struct _tmp126C;Cyc_Tcutil_failure_reason=(struct
_dyneither_ptr)((_tmp126C.tag=0,((_tmp126C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp507->hd,& Cyc_Tcutil_bk))),((_tmp126D.tag=0,((_tmp126D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp513->hd,& Cyc_Tcutil_bk))),((_tmp126E.tag=0,((_tmp126E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
_tmp513->hd)),((_tmp126F[0]=& _tmp126E,((_tmp126F[1]=& _tmp126D,((_tmp126F[2]=&
_tmp126C,Cyc_aprintf(((_tmp1270="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmp1270,sizeof(char),44))),_tag_dyneither(_tmp126F,sizeof(void*),
3))))))))))))))))))));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{struct
_tuple15*_tmp127A;struct Cyc_Absyn_VarType_struct _tmp1279;struct Cyc_Absyn_VarType_struct*
_tmp1278;struct Cyc_List_List*_tmp1277;inst=((_tmp1277=_region_malloc(rgn,sizeof(*
_tmp1277)),((_tmp1277->hd=((_tmp127A=_region_malloc(rgn,sizeof(*_tmp127A)),((
_tmp127A->f1=(struct Cyc_Absyn_Tvar*)_tmp507->hd,((_tmp127A->f2=(void*)((_tmp1278=
_cycalloc(sizeof(*_tmp1278)),((_tmp1278[0]=((_tmp1279.tag=2,((_tmp1279.f1=(
struct Cyc_Absyn_Tvar*)_tmp513->hd,_tmp1279)))),_tmp1278)))),_tmp127A)))))),((
_tmp1277->tl=inst,_tmp1277))))));}_tmp513=_tmp513->tl;_tmp507=_tmp507->tl;};}if(
_tmp507 != 0){{const char*_tmp127B;Cyc_Tcutil_failure_reason=((_tmp127B="(second function type has too many type variables)",
_tag_dyneither(_tmp127B,sizeof(char),51)));}_npop_handler(0);goto _LL28C;}if(inst
!= 0){{struct Cyc_Absyn_FnType_struct _tmp1287;struct Cyc_Absyn_FnInfo _tmp1286;
struct Cyc_Absyn_FnType_struct*_tmp1285;struct Cyc_Absyn_FnType_struct _tmp1281;
struct Cyc_Absyn_FnInfo _tmp1280;struct Cyc_Absyn_FnType_struct*_tmp127F;Cyc_Tcutil_unify_it((
void*)((_tmp127F=_cycalloc(sizeof(*_tmp127F)),((_tmp127F[0]=((_tmp1281.tag=10,((
_tmp1281.f1=((_tmp1280.tvars=0,((_tmp1280.effect=_tmp514,((_tmp1280.ret_tqual=
_tmp515,((_tmp1280.ret_typ=_tmp516,((_tmp1280.args=_tmp517,((_tmp1280.c_varargs=
_tmp518,((_tmp1280.cyc_varargs=_tmp519,((_tmp1280.rgn_po=_tmp51A,((_tmp1280.attributes=
_tmp51B,_tmp1280)))))))))))))))))),_tmp1281)))),_tmp127F)))),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)((_tmp1285=_cycalloc(sizeof(*_tmp1285)),((_tmp1285[0]=((_tmp1287.tag=
10,((_tmp1287.f1=((_tmp1286.tvars=0,((_tmp1286.effect=_tmp508,((_tmp1286.ret_tqual=
_tmp509,((_tmp1286.ret_typ=_tmp50A,((_tmp1286.args=_tmp50B,((_tmp1286.c_varargs=
_tmp50C,((_tmp1286.cyc_varargs=_tmp50D,((_tmp1286.rgn_po=_tmp50E,((_tmp1286.attributes=
_tmp50F,_tmp1286)))))))))))))))))),_tmp1287)))),_tmp1285))))));}done=1;}}if(done){
_npop_handler(0);return;}Cyc_Tcutil_unify_it(_tmp516,_tmp50A);Cyc_Tcutil_unify_tqual(
_tmp515,_tmp516,_tmp509,_tmp50A);for(0;_tmp517 != 0  && _tmp50B != 0;(_tmp517=
_tmp517->tl,_tmp50B=_tmp50B->tl)){struct Cyc_Absyn_Tqual _tmp57F;void*_tmp580;
struct _tuple9 _tmp57E=*((struct _tuple9*)_tmp517->hd);_tmp57F=_tmp57E.f2;_tmp580=
_tmp57E.f3;{struct Cyc_Absyn_Tqual _tmp582;void*_tmp583;struct _tuple9 _tmp581=*((
struct _tuple9*)_tmp50B->hd);_tmp582=_tmp581.f2;_tmp583=_tmp581.f3;Cyc_Tcutil_unify_it(
_tmp580,_tmp583);Cyc_Tcutil_unify_tqual(_tmp57F,_tmp580,_tmp582,_tmp583);};}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp517 != 0  || _tmp50B != 0){{const char*_tmp1288;
Cyc_Tcutil_failure_reason=((_tmp1288="(function types have different number of arguments)",
_tag_dyneither(_tmp1288,sizeof(char),52)));}_npop_handler(0);goto _LL28C;}if(
_tmp518 != _tmp50C){{const char*_tmp1289;Cyc_Tcutil_failure_reason=((_tmp1289="(only one function type takes C varargs)",
_tag_dyneither(_tmp1289,sizeof(char),41)));}_npop_handler(0);goto _LL28C;}{int
bad_cyc_vararg=0;{struct _tuple21 _tmp128A;struct _tuple21 _tmp587=(_tmp128A.f1=
_tmp519,((_tmp128A.f2=_tmp50D,_tmp128A)));struct Cyc_Absyn_VarargInfo*_tmp588;
struct Cyc_Absyn_VarargInfo*_tmp589;struct Cyc_Absyn_VarargInfo*_tmp58A;struct Cyc_Absyn_VarargInfo*
_tmp58B;struct Cyc_Absyn_VarargInfo*_tmp58C;struct Cyc_Absyn_VarargInfo _tmp58D;
struct Cyc_Core_Opt*_tmp58E;struct Cyc_Absyn_Tqual _tmp58F;void*_tmp590;int _tmp591;
struct Cyc_Absyn_VarargInfo*_tmp592;struct Cyc_Absyn_VarargInfo _tmp593;struct Cyc_Core_Opt*
_tmp594;struct Cyc_Absyn_Tqual _tmp595;void*_tmp596;int _tmp597;_LL2CD: _tmp588=
_tmp587.f1;if(_tmp588 != 0)goto _LL2CF;_tmp589=_tmp587.f2;if(_tmp589 != 0)goto
_LL2CF;_LL2CE: goto _LL2CC;_LL2CF: _tmp58A=_tmp587.f1;if(_tmp58A != 0)goto _LL2D1;
_LL2D0: goto _LL2D2;_LL2D1: _tmp58B=_tmp587.f2;if(_tmp58B != 0)goto _LL2D3;_LL2D2:
bad_cyc_vararg=1;{const char*_tmp128B;Cyc_Tcutil_failure_reason=((_tmp128B="(only one function type takes varargs)",
_tag_dyneither(_tmp128B,sizeof(char),39)));}goto _LL2CC;_LL2D3: _tmp58C=_tmp587.f1;
if(_tmp58C == 0)goto _LL2CC;_tmp58D=*_tmp58C;_tmp58E=_tmp58D.name;_tmp58F=_tmp58D.tq;
_tmp590=_tmp58D.type;_tmp591=_tmp58D.inject;_tmp592=_tmp587.f2;if(_tmp592 == 0)
goto _LL2CC;_tmp593=*_tmp592;_tmp594=_tmp593.name;_tmp595=_tmp593.tq;_tmp596=
_tmp593.type;_tmp597=_tmp593.inject;_LL2D4: Cyc_Tcutil_unify_it(_tmp590,_tmp596);
Cyc_Tcutil_unify_tqual(_tmp58F,_tmp590,_tmp595,_tmp596);if(_tmp591 != _tmp597){
bad_cyc_vararg=1;{const char*_tmp128C;Cyc_Tcutil_failure_reason=((_tmp128C="(only one function type injects varargs)",
_tag_dyneither(_tmp128C,sizeof(char),41)));};}goto _LL2CC;_LL2CC:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL28C;}{int bad_effect=0;{struct _tuple22
_tmp128D;struct _tuple22 _tmp59B=(_tmp128D.f1=_tmp514,((_tmp128D.f2=_tmp508,
_tmp128D)));struct Cyc_Core_Opt*_tmp59C;struct Cyc_Core_Opt*_tmp59D;struct Cyc_Core_Opt*
_tmp59E;struct Cyc_Core_Opt*_tmp59F;_LL2D6: _tmp59C=_tmp59B.f1;if(_tmp59C != 0)goto
_LL2D8;_tmp59D=_tmp59B.f2;if(_tmp59D != 0)goto _LL2D8;_LL2D7: goto _LL2D5;_LL2D8:
_tmp59E=_tmp59B.f1;if(_tmp59E != 0)goto _LL2DA;_LL2D9: goto _LL2DB;_LL2DA: _tmp59F=
_tmp59B.f2;if(_tmp59F != 0)goto _LL2DC;_LL2DB: bad_effect=1;goto _LL2D5;_LL2DC:;
_LL2DD: bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp514))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp508))->v);
goto _LL2D5;_LL2D5:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(
bad_effect){{const char*_tmp128E;Cyc_Tcutil_failure_reason=((_tmp128E="(function type effects do not unify)",
_tag_dyneither(_tmp128E,sizeof(char),37)));}_npop_handler(0);goto _LL28C;}if(!Cyc_Tcutil_same_atts(
_tmp50F,_tmp51B)){{const char*_tmp128F;Cyc_Tcutil_failure_reason=((_tmp128F="(function types have different attributes)",
_tag_dyneither(_tmp128F,sizeof(char),43)));}_npop_handler(0);goto _LL28C;}if(!Cyc_Tcutil_same_rgn_po(
_tmp50E,_tmp51A)){{const char*_tmp1290;Cyc_Tcutil_failure_reason=((_tmp1290="(function types have different region lifetime orderings)",
_tag_dyneither(_tmp1290,sizeof(char),58)));}_npop_handler(0);goto _LL28C;}
_npop_handler(0);return;};};;_pop_region(rgn);}_LL2AD: _tmp51C=_tmp483.f1;{struct
Cyc_Absyn_TupleType_struct*_tmp51D=(struct Cyc_Absyn_TupleType_struct*)_tmp51C;
if(_tmp51D->tag != 11)goto _LL2AF;else{_tmp51E=_tmp51D->f1;}};_tmp51F=_tmp483.f2;{
struct Cyc_Absyn_TupleType_struct*_tmp520=(struct Cyc_Absyn_TupleType_struct*)
_tmp51F;if(_tmp520->tag != 11)goto _LL2AF;else{_tmp521=_tmp520->f1;}};_LL2AE: for(0;
_tmp521 != 0  && _tmp51E != 0;(_tmp521=_tmp521->tl,_tmp51E=_tmp51E->tl)){struct Cyc_Absyn_Tqual
_tmp5A4;void*_tmp5A5;struct _tuple11 _tmp5A3=*((struct _tuple11*)_tmp521->hd);
_tmp5A4=_tmp5A3.f1;_tmp5A5=_tmp5A3.f2;{struct Cyc_Absyn_Tqual _tmp5A7;void*_tmp5A8;
struct _tuple11 _tmp5A6=*((struct _tuple11*)_tmp51E->hd);_tmp5A7=_tmp5A6.f1;_tmp5A8=
_tmp5A6.f2;Cyc_Tcutil_unify_it(_tmp5A5,_tmp5A8);Cyc_Tcutil_unify_tqual(_tmp5A4,
_tmp5A5,_tmp5A7,_tmp5A8);};}if(_tmp521 == 0  && _tmp51E == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmp1291;Cyc_Tcutil_failure_reason=((
_tmp1291="(tuple types have different numbers of components)",_tag_dyneither(
_tmp1291,sizeof(char),51)));}goto _LL28C;_LL2AF: _tmp522=_tmp483.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp523=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp522;if(_tmp523->tag != 13)goto
_LL2B1;else{_tmp524=_tmp523->f1;_tmp525=_tmp523->f2;}};_tmp526=_tmp483.f2;{
struct Cyc_Absyn_AnonAggrType_struct*_tmp527=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp526;if(_tmp527->tag != 13)goto _LL2B1;else{_tmp528=_tmp527->f1;_tmp529=_tmp527->f2;}};
_LL2B0: if(_tmp528 != _tmp524){{const char*_tmp1292;Cyc_Tcutil_failure_reason=((
_tmp1292="(struct and union type)",_tag_dyneither(_tmp1292,sizeof(char),24)));}
goto _LL28C;}for(0;_tmp529 != 0  && _tmp525 != 0;(_tmp529=_tmp529->tl,_tmp525=
_tmp525->tl)){struct Cyc_Absyn_Aggrfield*_tmp5AB=(struct Cyc_Absyn_Aggrfield*)
_tmp529->hd;struct Cyc_Absyn_Aggrfield*_tmp5AC=(struct Cyc_Absyn_Aggrfield*)
_tmp525->hd;if(Cyc_strptrcmp(_tmp5AB->name,_tmp5AC->name)!= 0){{const char*
_tmp1293;Cyc_Tcutil_failure_reason=((_tmp1293="(different member names)",
_tag_dyneither(_tmp1293,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_unify_it(_tmp5AB->type,_tmp5AC->type);Cyc_Tcutil_unify_tqual(_tmp5AB->tq,
_tmp5AB->type,_tmp5AC->tq,_tmp5AC->type);if(!Cyc_Tcutil_same_atts(_tmp5AB->attributes,
_tmp5AC->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const
char*_tmp1294;Cyc_Tcutil_failure_reason=((_tmp1294="(different attributes on member)",
_tag_dyneither(_tmp1294,sizeof(char),33)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
if((_tmp5AB->width != 0  && _tmp5AC->width == 0  || _tmp5AC->width != 0  && _tmp5AB->width
== 0) || (_tmp5AB->width != 0  && _tmp5AC->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp5AB->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp5AC->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmp1295;Cyc_Tcutil_failure_reason=((_tmp1295="(different bitfield widths on member)",
_tag_dyneither(_tmp1295,sizeof(char),38)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
if(_tmp529 == 0  && _tmp525 == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmp1296;Cyc_Tcutil_failure_reason=((_tmp1296="(different number of members)",
_tag_dyneither(_tmp1296,sizeof(char),30)));}goto _LL28C;_LL2B1: _tmp52A=_tmp483.f1;{
struct Cyc_Absyn_HeapRgn_struct*_tmp52B=(struct Cyc_Absyn_HeapRgn_struct*)_tmp52A;
if(_tmp52B->tag != 21)goto _LL2B3;};_tmp52C=_tmp483.f2;{struct Cyc_Absyn_HeapRgn_struct*
_tmp52D=(struct Cyc_Absyn_HeapRgn_struct*)_tmp52C;if(_tmp52D->tag != 21)goto _LL2B3;};
_LL2B2: return;_LL2B3: _tmp52E=_tmp483.f1;{struct Cyc_Absyn_UniqueRgn_struct*
_tmp52F=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp52E;if(_tmp52F->tag != 22)goto
_LL2B5;};_tmp530=_tmp483.f2;{struct Cyc_Absyn_UniqueRgn_struct*_tmp531=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp530;if(_tmp531->tag != 22)goto _LL2B5;};_LL2B4: return;_LL2B5: _tmp532=_tmp483.f1;{
struct Cyc_Absyn_RgnHandleType_struct*_tmp533=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp532;if(_tmp533->tag != 16)goto _LL2B7;else{_tmp534=(void*)_tmp533->f1;}};
_tmp535=_tmp483.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp536=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp535;if(_tmp536->tag != 16)goto _LL2B7;else{_tmp537=(void*)_tmp536->f1;}};
_LL2B6: Cyc_Tcutil_unify_it(_tmp534,_tmp537);return;_LL2B7: _tmp538=_tmp483.f1;{
struct Cyc_Absyn_DynRgnType_struct*_tmp539=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp538;if(_tmp539->tag != 17)goto _LL2B9;else{_tmp53A=(void*)_tmp539->f1;_tmp53B=(
void*)_tmp539->f2;}};_tmp53C=_tmp483.f2;{struct Cyc_Absyn_DynRgnType_struct*
_tmp53D=(struct Cyc_Absyn_DynRgnType_struct*)_tmp53C;if(_tmp53D->tag != 17)goto
_LL2B9;else{_tmp53E=(void*)_tmp53D->f1;_tmp53F=(void*)_tmp53D->f2;}};_LL2B8: Cyc_Tcutil_unify_it(
_tmp53A,_tmp53E);Cyc_Tcutil_unify_it(_tmp53B,_tmp53F);return;_LL2B9: _tmp540=
_tmp483.f1;{struct Cyc_Absyn_JoinEff_struct*_tmp541=(struct Cyc_Absyn_JoinEff_struct*)
_tmp540;if(_tmp541->tag != 24)goto _LL2BB;};_LL2BA: goto _LL2BC;_LL2BB: _tmp542=
_tmp483.f2;{struct Cyc_Absyn_JoinEff_struct*_tmp543=(struct Cyc_Absyn_JoinEff_struct*)
_tmp542;if(_tmp543->tag != 24)goto _LL2BD;};_LL2BC: goto _LL2BE;_LL2BD: _tmp544=
_tmp483.f1;{struct Cyc_Absyn_AccessEff_struct*_tmp545=(struct Cyc_Absyn_AccessEff_struct*)
_tmp544;if(_tmp545->tag != 23)goto _LL2BF;};_LL2BE: goto _LL2C0;_LL2BF: _tmp546=
_tmp483.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp547=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp546;if(_tmp547->tag != 25)goto _LL2C1;};_LL2C0: goto _LL2C2;_LL2C1: _tmp548=
_tmp483.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp549=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp548;if(_tmp549->tag != 25)goto _LL2C3;};_LL2C2: goto _LL2C4;_LL2C3: _tmp54A=
_tmp483.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp54B=(struct Cyc_Absyn_AccessEff_struct*)
_tmp54A;if(_tmp54B->tag != 23)goto _LL2C5;};_LL2C4: if(Cyc_Tcutil_unify_effect(t1,
t2))return;{const char*_tmp1297;Cyc_Tcutil_failure_reason=((_tmp1297="(effects don't unify)",
_tag_dyneither(_tmp1297,sizeof(char),22)));}goto _LL28C;_LL2C5:;_LL2C6: goto _LL28C;
_LL28C:;}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}int Cyc_Tcutil_star_cmp(int(*
cmp)(void*,void*),void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return
- 1;if(a1 != 0  && a2 == 0)return 1;return cmp((void*)_check_null(a1),(void*)
_check_null(a2));}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct
Cyc_Absyn_Tqual tq2){int _tmp5B2=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict
<< 2);int _tmp5B3=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp5B2,_tmp5B3);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(
void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{union Cyc_Absyn_Constraint*
_tmp5B4=x;union Cyc_Absyn_Constraint _tmp5B5;int _tmp5B6;union Cyc_Absyn_Constraint
_tmp5B7;void*_tmp5B8;union Cyc_Absyn_Constraint _tmp5B9;union Cyc_Absyn_Constraint*
_tmp5BA;_LL2DF: _tmp5B5=*_tmp5B4;if((_tmp5B5.No_constr).tag != 3)goto _LL2E1;
_tmp5B6=(int)(_tmp5B5.No_constr).val;_LL2E0: return - 1;_LL2E1: _tmp5B7=*_tmp5B4;if((
_tmp5B7.Eq_constr).tag != 1)goto _LL2E3;_tmp5B8=(void*)(_tmp5B7.Eq_constr).val;
_LL2E2: {union Cyc_Absyn_Constraint*_tmp5BB=y;union Cyc_Absyn_Constraint _tmp5BC;
int _tmp5BD;union Cyc_Absyn_Constraint _tmp5BE;void*_tmp5BF;union Cyc_Absyn_Constraint
_tmp5C0;union Cyc_Absyn_Constraint*_tmp5C1;_LL2E6: _tmp5BC=*_tmp5BB;if((_tmp5BC.No_constr).tag
!= 3)goto _LL2E8;_tmp5BD=(int)(_tmp5BC.No_constr).val;_LL2E7: return 1;_LL2E8:
_tmp5BE=*_tmp5BB;if((_tmp5BE.Eq_constr).tag != 1)goto _LL2EA;_tmp5BF=(void*)(
_tmp5BE.Eq_constr).val;_LL2E9: return cmp(_tmp5B8,_tmp5BF);_LL2EA: _tmp5C0=*_tmp5BB;
if((_tmp5C0.Forward_constr).tag != 2)goto _LL2E5;_tmp5C1=(union Cyc_Absyn_Constraint*)(
_tmp5C0.Forward_constr).val;_LL2EB: {const char*_tmp129A;void*_tmp1299;(_tmp1299=
0,Cyc_Tcutil_impos(((_tmp129A="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp129A,sizeof(char),40))),_tag_dyneither(_tmp1299,sizeof(void*),
0)));}_LL2E5:;}_LL2E3: _tmp5B9=*_tmp5B4;if((_tmp5B9.Forward_constr).tag != 2)goto
_LL2DE;_tmp5BA=(union Cyc_Absyn_Constraint*)(_tmp5B9.Forward_constr).val;_LL2E4: {
const char*_tmp129D;void*_tmp129C;(_tmp129C=0,Cyc_Tcutil_impos(((_tmp129D="unify_conref: forward after compress",
_tag_dyneither(_tmp129D,sizeof(char),37))),_tag_dyneither(_tmp129C,sizeof(void*),
0)));}_LL2DE:;};}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct
_tuple11*tqt2){struct _tuple11 _tmp5C7;struct Cyc_Absyn_Tqual _tmp5C8;void*_tmp5C9;
struct _tuple11*_tmp5C6=tqt1;_tmp5C7=*_tmp5C6;_tmp5C8=_tmp5C7.f1;_tmp5C9=_tmp5C7.f2;{
struct _tuple11 _tmp5CB;struct Cyc_Absyn_Tqual _tmp5CC;void*_tmp5CD;struct _tuple11*
_tmp5CA=tqt2;_tmp5CB=*_tmp5CA;_tmp5CC=_tmp5CB.f1;_tmp5CD=_tmp5CB.f2;{int _tmp5CE=
Cyc_Tcutil_tqual_cmp(_tmp5C8,_tmp5CC);if(_tmp5CE != 0)return _tmp5CE;return Cyc_Tcutil_typecmp(
_tmp5C9,_tmp5CD);};};}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp5CF=Cyc_strptrcmp(f1->name,f2->name);if(
_tmp5CF != 0)return _tmp5CF;{int _tmp5D0=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp5D0 != 0)return _tmp5D0;{int _tmp5D1=Cyc_Tcutil_typecmp(f1->type,f2->type);if(
_tmp5D1 != 0)return _tmp5D1;{int _tmp5D2=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,
f2->attributes);if(_tmp5D2 != 0)return _tmp5D2;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,f1->width,f2->width);};};};}static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){int _tmp5D3=Cyc_Absyn_qvar_cmp(
e1->name,e2->name);if(_tmp5D3 != 0)return _tmp5D3;return((int(*)(int(*cmp)(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp5D4=t;_LL2ED: {struct Cyc_Absyn_VoidType_struct*_tmp5D5=(struct
Cyc_Absyn_VoidType_struct*)_tmp5D4;if(_tmp5D5->tag != 0)goto _LL2EF;}_LL2EE: return
0;_LL2EF: {struct Cyc_Absyn_Evar_struct*_tmp5D6=(struct Cyc_Absyn_Evar_struct*)
_tmp5D4;if(_tmp5D6->tag != 1)goto _LL2F1;}_LL2F0: return 1;_LL2F1: {struct Cyc_Absyn_VarType_struct*
_tmp5D7=(struct Cyc_Absyn_VarType_struct*)_tmp5D4;if(_tmp5D7->tag != 2)goto _LL2F3;}
_LL2F2: return 2;_LL2F3: {struct Cyc_Absyn_DatatypeType_struct*_tmp5D8=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp5D4;if(_tmp5D8->tag != 3)goto _LL2F5;}_LL2F4: return 3;_LL2F5: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp5D9=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp5D4;if(_tmp5D9->tag != 4)
goto _LL2F7;}_LL2F6: return 4;_LL2F7: {struct Cyc_Absyn_PointerType_struct*_tmp5DA=(
struct Cyc_Absyn_PointerType_struct*)_tmp5D4;if(_tmp5DA->tag != 5)goto _LL2F9;}
_LL2F8: return 5;_LL2F9: {struct Cyc_Absyn_IntType_struct*_tmp5DB=(struct Cyc_Absyn_IntType_struct*)
_tmp5D4;if(_tmp5DB->tag != 6)goto _LL2FB;}_LL2FA: return 6;_LL2FB: {struct Cyc_Absyn_FloatType_struct*
_tmp5DC=(struct Cyc_Absyn_FloatType_struct*)_tmp5D4;if(_tmp5DC->tag != 7)goto
_LL2FD;}_LL2FC: return 7;_LL2FD: {struct Cyc_Absyn_DoubleType_struct*_tmp5DD=(
struct Cyc_Absyn_DoubleType_struct*)_tmp5D4;if(_tmp5DD->tag != 8)goto _LL2FF;}
_LL2FE: return 8;_LL2FF: {struct Cyc_Absyn_ArrayType_struct*_tmp5DE=(struct Cyc_Absyn_ArrayType_struct*)
_tmp5D4;if(_tmp5DE->tag != 9)goto _LL301;}_LL300: return 9;_LL301: {struct Cyc_Absyn_FnType_struct*
_tmp5DF=(struct Cyc_Absyn_FnType_struct*)_tmp5D4;if(_tmp5DF->tag != 10)goto _LL303;}
_LL302: return 10;_LL303: {struct Cyc_Absyn_TupleType_struct*_tmp5E0=(struct Cyc_Absyn_TupleType_struct*)
_tmp5D4;if(_tmp5E0->tag != 11)goto _LL305;}_LL304: return 11;_LL305: {struct Cyc_Absyn_AggrType_struct*
_tmp5E1=(struct Cyc_Absyn_AggrType_struct*)_tmp5D4;if(_tmp5E1->tag != 12)goto
_LL307;}_LL306: return 12;_LL307: {struct Cyc_Absyn_AnonAggrType_struct*_tmp5E2=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp5D4;if(_tmp5E2->tag != 13)goto _LL309;}
_LL308: return 14;_LL309: {struct Cyc_Absyn_EnumType_struct*_tmp5E3=(struct Cyc_Absyn_EnumType_struct*)
_tmp5D4;if(_tmp5E3->tag != 14)goto _LL30B;}_LL30A: return 16;_LL30B: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp5E4=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp5D4;if(_tmp5E4->tag != 15)goto
_LL30D;}_LL30C: return 17;_LL30D: {struct Cyc_Absyn_RgnHandleType_struct*_tmp5E5=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp5D4;if(_tmp5E5->tag != 16)goto _LL30F;}
_LL30E: return 18;_LL30F: {struct Cyc_Absyn_TypedefType_struct*_tmp5E6=(struct Cyc_Absyn_TypedefType_struct*)
_tmp5D4;if(_tmp5E6->tag != 18)goto _LL311;}_LL310: return 19;_LL311: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp5E7=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp5D4;if(_tmp5E7->tag != 22)goto
_LL313;}_LL312: return 20;_LL313: {struct Cyc_Absyn_HeapRgn_struct*_tmp5E8=(struct
Cyc_Absyn_HeapRgn_struct*)_tmp5D4;if(_tmp5E8->tag != 21)goto _LL315;}_LL314: return
21;_LL315: {struct Cyc_Absyn_AccessEff_struct*_tmp5E9=(struct Cyc_Absyn_AccessEff_struct*)
_tmp5D4;if(_tmp5E9->tag != 23)goto _LL317;}_LL316: return 22;_LL317: {struct Cyc_Absyn_JoinEff_struct*
_tmp5EA=(struct Cyc_Absyn_JoinEff_struct*)_tmp5D4;if(_tmp5EA->tag != 24)goto _LL319;}
_LL318: return 23;_LL319: {struct Cyc_Absyn_RgnsEff_struct*_tmp5EB=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp5D4;if(_tmp5EB->tag != 25)goto _LL31B;}_LL31A: return 24;_LL31B: {struct Cyc_Absyn_TagType_struct*
_tmp5EC=(struct Cyc_Absyn_TagType_struct*)_tmp5D4;if(_tmp5EC->tag != 20)goto _LL31D;}
_LL31C: return 26;_LL31D: {struct Cyc_Absyn_DynRgnType_struct*_tmp5ED=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp5D4;if(_tmp5ED->tag != 17)goto _LL31F;}_LL31E: return 27;_LL31F: {struct Cyc_Absyn_ValueofType_struct*
_tmp5EE=(struct Cyc_Absyn_ValueofType_struct*)_tmp5D4;if(_tmp5EE->tag != 19)goto
_LL321;}_LL320: return 28;_LL321: {struct Cyc_Absyn_TypeDeclType_struct*_tmp5EF=(
struct Cyc_Absyn_TypeDeclType_struct*)_tmp5D4;if(_tmp5EF->tag != 26)goto _LL2EC;}
_LL322: return 29;_LL2EC:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp5F0=Cyc_Core_intcmp(
Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));if(_tmp5F0 != 0)
return _tmp5F0;{struct _tuple0 _tmp129E;struct _tuple0 _tmp5F2=(_tmp129E.f1=t2,((
_tmp129E.f2=t1,_tmp129E)));void*_tmp5F3;void*_tmp5F5;void*_tmp5F7;struct Cyc_Absyn_Tvar*
_tmp5F9;void*_tmp5FA;struct Cyc_Absyn_Tvar*_tmp5FC;void*_tmp5FD;struct Cyc_Absyn_AggrInfo
_tmp5FF;union Cyc_Absyn_AggrInfoU _tmp600;struct Cyc_List_List*_tmp601;void*_tmp602;
struct Cyc_Absyn_AggrInfo _tmp604;union Cyc_Absyn_AggrInfoU _tmp605;struct Cyc_List_List*
_tmp606;void*_tmp607;struct _tuple2*_tmp609;void*_tmp60A;struct _tuple2*_tmp60C;
void*_tmp60D;struct Cyc_List_List*_tmp60F;void*_tmp610;struct Cyc_List_List*
_tmp612;void*_tmp613;struct Cyc_Absyn_DatatypeInfo _tmp615;union Cyc_Absyn_DatatypeInfoU
_tmp616;struct Cyc_Absyn_Datatypedecl**_tmp617;struct Cyc_Absyn_Datatypedecl*
_tmp618;struct Cyc_List_List*_tmp619;void*_tmp61A;struct Cyc_Absyn_DatatypeInfo
_tmp61C;union Cyc_Absyn_DatatypeInfoU _tmp61D;struct Cyc_Absyn_Datatypedecl**
_tmp61E;struct Cyc_Absyn_Datatypedecl*_tmp61F;struct Cyc_List_List*_tmp620;void*
_tmp621;struct Cyc_Absyn_DatatypeFieldInfo _tmp623;union Cyc_Absyn_DatatypeFieldInfoU
_tmp624;struct _tuple3 _tmp625;struct Cyc_Absyn_Datatypedecl*_tmp626;struct Cyc_Absyn_Datatypefield*
_tmp627;struct Cyc_List_List*_tmp628;void*_tmp629;struct Cyc_Absyn_DatatypeFieldInfo
_tmp62B;union Cyc_Absyn_DatatypeFieldInfoU _tmp62C;struct _tuple3 _tmp62D;struct Cyc_Absyn_Datatypedecl*
_tmp62E;struct Cyc_Absyn_Datatypefield*_tmp62F;struct Cyc_List_List*_tmp630;void*
_tmp631;struct Cyc_Absyn_PtrInfo _tmp633;void*_tmp634;struct Cyc_Absyn_Tqual _tmp635;
struct Cyc_Absyn_PtrAtts _tmp636;void*_tmp637;union Cyc_Absyn_Constraint*_tmp638;
union Cyc_Absyn_Constraint*_tmp639;union Cyc_Absyn_Constraint*_tmp63A;void*_tmp63B;
struct Cyc_Absyn_PtrInfo _tmp63D;void*_tmp63E;struct Cyc_Absyn_Tqual _tmp63F;struct
Cyc_Absyn_PtrAtts _tmp640;void*_tmp641;union Cyc_Absyn_Constraint*_tmp642;union Cyc_Absyn_Constraint*
_tmp643;union Cyc_Absyn_Constraint*_tmp644;void*_tmp645;enum Cyc_Absyn_Sign
_tmp647;enum Cyc_Absyn_Size_of _tmp648;void*_tmp649;enum Cyc_Absyn_Sign _tmp64B;
enum Cyc_Absyn_Size_of _tmp64C;void*_tmp64D;void*_tmp64F;void*_tmp651;int _tmp653;
void*_tmp654;int _tmp656;void*_tmp657;struct Cyc_Absyn_ArrayInfo _tmp659;void*
_tmp65A;struct Cyc_Absyn_Tqual _tmp65B;struct Cyc_Absyn_Exp*_tmp65C;union Cyc_Absyn_Constraint*
_tmp65D;void*_tmp65E;struct Cyc_Absyn_ArrayInfo _tmp660;void*_tmp661;struct Cyc_Absyn_Tqual
_tmp662;struct Cyc_Absyn_Exp*_tmp663;union Cyc_Absyn_Constraint*_tmp664;void*
_tmp665;struct Cyc_Absyn_FnInfo _tmp667;struct Cyc_List_List*_tmp668;struct Cyc_Core_Opt*
_tmp669;struct Cyc_Absyn_Tqual _tmp66A;void*_tmp66B;struct Cyc_List_List*_tmp66C;
int _tmp66D;struct Cyc_Absyn_VarargInfo*_tmp66E;struct Cyc_List_List*_tmp66F;struct
Cyc_List_List*_tmp670;void*_tmp671;struct Cyc_Absyn_FnInfo _tmp673;struct Cyc_List_List*
_tmp674;struct Cyc_Core_Opt*_tmp675;struct Cyc_Absyn_Tqual _tmp676;void*_tmp677;
struct Cyc_List_List*_tmp678;int _tmp679;struct Cyc_Absyn_VarargInfo*_tmp67A;struct
Cyc_List_List*_tmp67B;struct Cyc_List_List*_tmp67C;void*_tmp67D;struct Cyc_List_List*
_tmp67F;void*_tmp680;struct Cyc_List_List*_tmp682;void*_tmp683;enum Cyc_Absyn_AggrKind
_tmp685;struct Cyc_List_List*_tmp686;void*_tmp687;enum Cyc_Absyn_AggrKind _tmp689;
struct Cyc_List_List*_tmp68A;void*_tmp68B;void*_tmp68D;void*_tmp68E;void*_tmp690;
void*_tmp691;void*_tmp693;void*_tmp694;void*_tmp695;void*_tmp697;void*_tmp698;
void*_tmp699;void*_tmp69B;void*_tmp69C;void*_tmp69E;void*_tmp69F;struct Cyc_Absyn_Exp*
_tmp6A1;void*_tmp6A2;struct Cyc_Absyn_Exp*_tmp6A4;void*_tmp6A5;void*_tmp6A7;void*
_tmp6A9;void*_tmp6AB;void*_tmp6AD;void*_tmp6AF;_LL324: _tmp5F3=_tmp5F2.f1;{struct
Cyc_Absyn_Evar_struct*_tmp5F4=(struct Cyc_Absyn_Evar_struct*)_tmp5F3;if(_tmp5F4->tag
!= 1)goto _LL326;};_tmp5F5=_tmp5F2.f2;{struct Cyc_Absyn_Evar_struct*_tmp5F6=(
struct Cyc_Absyn_Evar_struct*)_tmp5F5;if(_tmp5F6->tag != 1)goto _LL326;};_LL325: {
const char*_tmp12A1;void*_tmp12A0;(_tmp12A0=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12A1="typecmp: can only compare closed types",
_tag_dyneither(_tmp12A1,sizeof(char),39))),_tag_dyneither(_tmp12A0,sizeof(void*),
0)));}_LL326: _tmp5F7=_tmp5F2.f1;{struct Cyc_Absyn_VarType_struct*_tmp5F8=(struct
Cyc_Absyn_VarType_struct*)_tmp5F7;if(_tmp5F8->tag != 2)goto _LL328;else{_tmp5F9=
_tmp5F8->f1;}};_tmp5FA=_tmp5F2.f2;{struct Cyc_Absyn_VarType_struct*_tmp5FB=(
struct Cyc_Absyn_VarType_struct*)_tmp5FA;if(_tmp5FB->tag != 2)goto _LL328;else{
_tmp5FC=_tmp5FB->f1;}};_LL327: return Cyc_Core_intcmp(_tmp5FC->identity,_tmp5F9->identity);
_LL328: _tmp5FD=_tmp5F2.f1;{struct Cyc_Absyn_AggrType_struct*_tmp5FE=(struct Cyc_Absyn_AggrType_struct*)
_tmp5FD;if(_tmp5FE->tag != 12)goto _LL32A;else{_tmp5FF=_tmp5FE->f1;_tmp600=_tmp5FF.aggr_info;
_tmp601=_tmp5FF.targs;}};_tmp602=_tmp5F2.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp603=(struct Cyc_Absyn_AggrType_struct*)_tmp602;if(_tmp603->tag != 12)goto
_LL32A;else{_tmp604=_tmp603->f1;_tmp605=_tmp604.aggr_info;_tmp606=_tmp604.targs;}};
_LL329: {struct _tuple2*_tmp6B4;struct _tuple12 _tmp6B3=Cyc_Absyn_aggr_kinded_name(
_tmp600);_tmp6B4=_tmp6B3.f2;{struct _tuple2*_tmp6B6;struct _tuple12 _tmp6B5=Cyc_Absyn_aggr_kinded_name(
_tmp605);_tmp6B6=_tmp6B5.f2;{int _tmp6B7=Cyc_Absyn_qvar_cmp(_tmp6B4,_tmp6B6);if(
_tmp6B7 != 0)return _tmp6B7;else{return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp601,_tmp606);}};};}
_LL32A: _tmp607=_tmp5F2.f1;{struct Cyc_Absyn_EnumType_struct*_tmp608=(struct Cyc_Absyn_EnumType_struct*)
_tmp607;if(_tmp608->tag != 14)goto _LL32C;else{_tmp609=_tmp608->f1;}};_tmp60A=
_tmp5F2.f2;{struct Cyc_Absyn_EnumType_struct*_tmp60B=(struct Cyc_Absyn_EnumType_struct*)
_tmp60A;if(_tmp60B->tag != 14)goto _LL32C;else{_tmp60C=_tmp60B->f1;}};_LL32B:
return Cyc_Absyn_qvar_cmp(_tmp609,_tmp60C);_LL32C: _tmp60D=_tmp5F2.f1;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp60E=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp60D;if(_tmp60E->tag != 15)goto
_LL32E;else{_tmp60F=_tmp60E->f1;}};_tmp610=_tmp5F2.f2;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp611=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp610;if(_tmp611->tag != 15)goto
_LL32E;else{_tmp612=_tmp611->f1;}};_LL32D: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp60F,_tmp612);_LL32E: _tmp613=_tmp5F2.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp614=(struct Cyc_Absyn_DatatypeType_struct*)_tmp613;if(_tmp614->tag != 3)goto
_LL330;else{_tmp615=_tmp614->f1;_tmp616=_tmp615.datatype_info;if((_tmp616.KnownDatatype).tag
!= 2)goto _LL330;_tmp617=(struct Cyc_Absyn_Datatypedecl**)(_tmp616.KnownDatatype).val;
_tmp618=*_tmp617;_tmp619=_tmp615.targs;}};_tmp61A=_tmp5F2.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp61B=(struct Cyc_Absyn_DatatypeType_struct*)_tmp61A;if(_tmp61B->tag != 3)goto
_LL330;else{_tmp61C=_tmp61B->f1;_tmp61D=_tmp61C.datatype_info;if((_tmp61D.KnownDatatype).tag
!= 2)goto _LL330;_tmp61E=(struct Cyc_Absyn_Datatypedecl**)(_tmp61D.KnownDatatype).val;
_tmp61F=*_tmp61E;_tmp620=_tmp61C.targs;}};_LL32F: if(_tmp61F == _tmp618)return 0;{
int _tmp6B8=Cyc_Absyn_qvar_cmp(_tmp61F->name,_tmp618->name);if(_tmp6B8 != 0)return
_tmp6B8;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp620,_tmp619);};_LL330: _tmp621=
_tmp5F2.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*_tmp622=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp621;if(_tmp622->tag != 4)goto _LL332;else{_tmp623=_tmp622->f1;_tmp624=_tmp623.field_info;
if((_tmp624.KnownDatatypefield).tag != 2)goto _LL332;_tmp625=(struct _tuple3)(
_tmp624.KnownDatatypefield).val;_tmp626=_tmp625.f1;_tmp627=_tmp625.f2;_tmp628=
_tmp623.targs;}};_tmp629=_tmp5F2.f2;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp62A=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp629;if(_tmp62A->tag != 4)
goto _LL332;else{_tmp62B=_tmp62A->f1;_tmp62C=_tmp62B.field_info;if((_tmp62C.KnownDatatypefield).tag
!= 2)goto _LL332;_tmp62D=(struct _tuple3)(_tmp62C.KnownDatatypefield).val;_tmp62E=
_tmp62D.f1;_tmp62F=_tmp62D.f2;_tmp630=_tmp62B.targs;}};_LL331: if(_tmp62E == 
_tmp626)return 0;{int _tmp6B9=Cyc_Absyn_qvar_cmp(_tmp626->name,_tmp62E->name);if(
_tmp6B9 != 0)return _tmp6B9;{int _tmp6BA=Cyc_Absyn_qvar_cmp(_tmp627->name,_tmp62F->name);
if(_tmp6BA != 0)return _tmp6BA;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp630,_tmp628);};};
_LL332: _tmp631=_tmp5F2.f1;{struct Cyc_Absyn_PointerType_struct*_tmp632=(struct Cyc_Absyn_PointerType_struct*)
_tmp631;if(_tmp632->tag != 5)goto _LL334;else{_tmp633=_tmp632->f1;_tmp634=_tmp633.elt_typ;
_tmp635=_tmp633.elt_tq;_tmp636=_tmp633.ptr_atts;_tmp637=_tmp636.rgn;_tmp638=
_tmp636.nullable;_tmp639=_tmp636.bounds;_tmp63A=_tmp636.zero_term;}};_tmp63B=
_tmp5F2.f2;{struct Cyc_Absyn_PointerType_struct*_tmp63C=(struct Cyc_Absyn_PointerType_struct*)
_tmp63B;if(_tmp63C->tag != 5)goto _LL334;else{_tmp63D=_tmp63C->f1;_tmp63E=_tmp63D.elt_typ;
_tmp63F=_tmp63D.elt_tq;_tmp640=_tmp63D.ptr_atts;_tmp641=_tmp640.rgn;_tmp642=
_tmp640.nullable;_tmp643=_tmp640.bounds;_tmp644=_tmp640.zero_term;}};_LL333: {
int _tmp6BB=Cyc_Tcutil_typecmp(_tmp63E,_tmp634);if(_tmp6BB != 0)return _tmp6BB;{int
_tmp6BC=Cyc_Tcutil_typecmp(_tmp641,_tmp637);if(_tmp6BC != 0)return _tmp6BC;{int
_tmp6BD=Cyc_Tcutil_tqual_cmp(_tmp63F,_tmp635);if(_tmp6BD != 0)return _tmp6BD;{int
_tmp6BE=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp643,_tmp639);if(_tmp6BE != 0)
return _tmp6BE;{int _tmp6BF=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp644,
_tmp63A);if(_tmp6BF != 0)return _tmp6BF;{void*_tmp6C0=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp643);_LL359: {struct Cyc_Absyn_DynEither_b_struct*
_tmp6C1=(struct Cyc_Absyn_DynEither_b_struct*)_tmp6C0;if(_tmp6C1->tag != 0)goto
_LL35B;}_LL35A: return 0;_LL35B:;_LL35C: goto _LL358;_LL358:;}return((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp642,_tmp638);};};};};}_LL334: _tmp645=_tmp5F2.f1;{struct Cyc_Absyn_IntType_struct*
_tmp646=(struct Cyc_Absyn_IntType_struct*)_tmp645;if(_tmp646->tag != 6)goto _LL336;
else{_tmp647=_tmp646->f1;_tmp648=_tmp646->f2;}};_tmp649=_tmp5F2.f2;{struct Cyc_Absyn_IntType_struct*
_tmp64A=(struct Cyc_Absyn_IntType_struct*)_tmp649;if(_tmp64A->tag != 6)goto _LL336;
else{_tmp64B=_tmp64A->f1;_tmp64C=_tmp64A->f2;}};_LL335: if(_tmp64B != _tmp647)
return Cyc_Core_intcmp((int)((unsigned int)_tmp64B),(int)((unsigned int)_tmp647));
if(_tmp64C != _tmp648)return Cyc_Core_intcmp((int)((unsigned int)_tmp64C),(int)((
unsigned int)_tmp648));return 0;_LL336: _tmp64D=_tmp5F2.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp64E=(struct Cyc_Absyn_FloatType_struct*)_tmp64D;if(_tmp64E->tag != 7)goto
_LL338;};_tmp64F=_tmp5F2.f2;{struct Cyc_Absyn_FloatType_struct*_tmp650=(struct Cyc_Absyn_FloatType_struct*)
_tmp64F;if(_tmp650->tag != 7)goto _LL338;};_LL337: return 0;_LL338: _tmp651=_tmp5F2.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp652=(struct Cyc_Absyn_DoubleType_struct*)
_tmp651;if(_tmp652->tag != 8)goto _LL33A;else{_tmp653=_tmp652->f1;}};_tmp654=
_tmp5F2.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp655=(struct Cyc_Absyn_DoubleType_struct*)
_tmp654;if(_tmp655->tag != 8)goto _LL33A;else{_tmp656=_tmp655->f1;}};_LL339: if(
_tmp653 == _tmp656)return 0;else{if(_tmp653)return - 1;else{return 1;}}_LL33A: _tmp657=
_tmp5F2.f1;{struct Cyc_Absyn_ArrayType_struct*_tmp658=(struct Cyc_Absyn_ArrayType_struct*)
_tmp657;if(_tmp658->tag != 9)goto _LL33C;else{_tmp659=_tmp658->f1;_tmp65A=_tmp659.elt_type;
_tmp65B=_tmp659.tq;_tmp65C=_tmp659.num_elts;_tmp65D=_tmp659.zero_term;}};_tmp65E=
_tmp5F2.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp65F=(struct Cyc_Absyn_ArrayType_struct*)
_tmp65E;if(_tmp65F->tag != 9)goto _LL33C;else{_tmp660=_tmp65F->f1;_tmp661=_tmp660.elt_type;
_tmp662=_tmp660.tq;_tmp663=_tmp660.num_elts;_tmp664=_tmp660.zero_term;}};_LL33B: {
int _tmp6C2=Cyc_Tcutil_tqual_cmp(_tmp662,_tmp65B);if(_tmp6C2 != 0)return _tmp6C2;{
int _tmp6C3=Cyc_Tcutil_typecmp(_tmp661,_tmp65A);if(_tmp6C3 != 0)return _tmp6C3;{int
_tmp6C4=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp65D,_tmp664);if(_tmp6C4 != 0)
return _tmp6C4;if(_tmp65C == _tmp663)return 0;if(_tmp65C == 0  || _tmp663 == 0){const
char*_tmp12A4;void*_tmp12A3;(_tmp12A3=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12A4="missing expression in array index",
_tag_dyneither(_tmp12A4,sizeof(char),34))),_tag_dyneither(_tmp12A3,sizeof(void*),
0)));}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct
Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp65C,_tmp663);};};}_LL33C: _tmp665=_tmp5F2.f1;{struct Cyc_Absyn_FnType_struct*
_tmp666=(struct Cyc_Absyn_FnType_struct*)_tmp665;if(_tmp666->tag != 10)goto _LL33E;
else{_tmp667=_tmp666->f1;_tmp668=_tmp667.tvars;_tmp669=_tmp667.effect;_tmp66A=
_tmp667.ret_tqual;_tmp66B=_tmp667.ret_typ;_tmp66C=_tmp667.args;_tmp66D=_tmp667.c_varargs;
_tmp66E=_tmp667.cyc_varargs;_tmp66F=_tmp667.rgn_po;_tmp670=_tmp667.attributes;}};
_tmp671=_tmp5F2.f2;{struct Cyc_Absyn_FnType_struct*_tmp672=(struct Cyc_Absyn_FnType_struct*)
_tmp671;if(_tmp672->tag != 10)goto _LL33E;else{_tmp673=_tmp672->f1;_tmp674=_tmp673.tvars;
_tmp675=_tmp673.effect;_tmp676=_tmp673.ret_tqual;_tmp677=_tmp673.ret_typ;_tmp678=
_tmp673.args;_tmp679=_tmp673.c_varargs;_tmp67A=_tmp673.cyc_varargs;_tmp67B=
_tmp673.rgn_po;_tmp67C=_tmp673.attributes;}};_LL33D: {const char*_tmp12A7;void*
_tmp12A6;(_tmp12A6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12A7="typecmp: function types not handled",
_tag_dyneither(_tmp12A7,sizeof(char),36))),_tag_dyneither(_tmp12A6,sizeof(void*),
0)));}_LL33E: _tmp67D=_tmp5F2.f1;{struct Cyc_Absyn_TupleType_struct*_tmp67E=(
struct Cyc_Absyn_TupleType_struct*)_tmp67D;if(_tmp67E->tag != 11)goto _LL340;else{
_tmp67F=_tmp67E->f1;}};_tmp680=_tmp5F2.f2;{struct Cyc_Absyn_TupleType_struct*
_tmp681=(struct Cyc_Absyn_TupleType_struct*)_tmp680;if(_tmp681->tag != 11)goto
_LL340;else{_tmp682=_tmp681->f1;}};_LL33F: return((int(*)(int(*cmp)(struct
_tuple11*,struct _tuple11*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_tqual_type_cmp,_tmp682,_tmp67F);_LL340: _tmp683=_tmp5F2.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp684=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp683;if(_tmp684->tag != 13)goto
_LL342;else{_tmp685=_tmp684->f1;_tmp686=_tmp684->f2;}};_tmp687=_tmp5F2.f2;{
struct Cyc_Absyn_AnonAggrType_struct*_tmp688=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp687;if(_tmp688->tag != 13)goto _LL342;else{_tmp689=_tmp688->f1;_tmp68A=_tmp688->f2;}};
_LL341: if(_tmp689 != _tmp685){if(_tmp689 == Cyc_Absyn_StructA)return - 1;else{return
1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp68A,_tmp686);_LL342: _tmp68B=_tmp5F2.f1;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp68C=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp68B;if(_tmp68C->tag != 16)goto
_LL344;else{_tmp68D=(void*)_tmp68C->f1;}};_tmp68E=_tmp5F2.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp68F=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp68E;if(_tmp68F->tag != 16)goto
_LL344;else{_tmp690=(void*)_tmp68F->f1;}};_LL343: return Cyc_Tcutil_typecmp(
_tmp68D,_tmp690);_LL344: _tmp691=_tmp5F2.f1;{struct Cyc_Absyn_DynRgnType_struct*
_tmp692=(struct Cyc_Absyn_DynRgnType_struct*)_tmp691;if(_tmp692->tag != 17)goto
_LL346;else{_tmp693=(void*)_tmp692->f1;_tmp694=(void*)_tmp692->f2;}};_tmp695=
_tmp5F2.f2;{struct Cyc_Absyn_DynRgnType_struct*_tmp696=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp695;if(_tmp696->tag != 17)goto _LL346;else{_tmp697=(void*)_tmp696->f1;_tmp698=(
void*)_tmp696->f2;}};_LL345: {int _tmp6C9=Cyc_Tcutil_typecmp(_tmp693,_tmp697);if(
_tmp6C9 != 0)return _tmp6C9;else{return Cyc_Tcutil_typecmp(_tmp694,_tmp698);}}
_LL346: _tmp699=_tmp5F2.f1;{struct Cyc_Absyn_TagType_struct*_tmp69A=(struct Cyc_Absyn_TagType_struct*)
_tmp699;if(_tmp69A->tag != 20)goto _LL348;else{_tmp69B=(void*)_tmp69A->f1;}};
_tmp69C=_tmp5F2.f2;{struct Cyc_Absyn_TagType_struct*_tmp69D=(struct Cyc_Absyn_TagType_struct*)
_tmp69C;if(_tmp69D->tag != 20)goto _LL348;else{_tmp69E=(void*)_tmp69D->f1;}};
_LL347: return Cyc_Tcutil_typecmp(_tmp69B,_tmp69E);_LL348: _tmp69F=_tmp5F2.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp6A0=(struct Cyc_Absyn_ValueofType_struct*)
_tmp69F;if(_tmp6A0->tag != 19)goto _LL34A;else{_tmp6A1=_tmp6A0->f1;}};_tmp6A2=
_tmp5F2.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp6A3=(struct Cyc_Absyn_ValueofType_struct*)
_tmp6A2;if(_tmp6A3->tag != 19)goto _LL34A;else{_tmp6A4=_tmp6A3->f1;}};_LL349:
return Cyc_Evexp_const_exp_cmp(_tmp6A1,_tmp6A4);_LL34A: _tmp6A5=_tmp5F2.f1;{struct
Cyc_Absyn_JoinEff_struct*_tmp6A6=(struct Cyc_Absyn_JoinEff_struct*)_tmp6A5;if(
_tmp6A6->tag != 24)goto _LL34C;};_LL34B: goto _LL34D;_LL34C: _tmp6A7=_tmp5F2.f2;{
struct Cyc_Absyn_JoinEff_struct*_tmp6A8=(struct Cyc_Absyn_JoinEff_struct*)_tmp6A7;
if(_tmp6A8->tag != 24)goto _LL34E;};_LL34D: goto _LL34F;_LL34E: _tmp6A9=_tmp5F2.f1;{
struct Cyc_Absyn_AccessEff_struct*_tmp6AA=(struct Cyc_Absyn_AccessEff_struct*)
_tmp6A9;if(_tmp6AA->tag != 23)goto _LL350;};_LL34F: goto _LL351;_LL350: _tmp6AB=
_tmp5F2.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp6AC=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp6AB;if(_tmp6AC->tag != 25)goto _LL352;};_LL351: goto _LL353;_LL352: _tmp6AD=
_tmp5F2.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp6AE=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp6AD;if(_tmp6AE->tag != 25)goto _LL354;};_LL353: goto _LL355;_LL354: _tmp6AF=
_tmp5F2.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp6B0=(struct Cyc_Absyn_AccessEff_struct*)
_tmp6AF;if(_tmp6B0->tag != 23)goto _LL356;};_LL355: {const char*_tmp12AA;void*
_tmp12A9;(_tmp12A9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12AA="typecmp: effects not handled",_tag_dyneither(
_tmp12AA,sizeof(char),29))),_tag_dyneither(_tmp12A9,sizeof(void*),0)));}_LL356:;
_LL357: {const char*_tmp12AD;void*_tmp12AC;(_tmp12AC=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12AD="Unmatched case in typecmp",
_tag_dyneither(_tmp12AD,sizeof(char),26))),_tag_dyneither(_tmp12AC,sizeof(void*),
0)));}_LL323:;};};}int Cyc_Tcutil_is_arithmetic_type(void*t){void*_tmp6CE=Cyc_Tcutil_compress(
t);_LL35E: {struct Cyc_Absyn_IntType_struct*_tmp6CF=(struct Cyc_Absyn_IntType_struct*)
_tmp6CE;if(_tmp6CF->tag != 6)goto _LL360;}_LL35F: goto _LL361;_LL360: {struct Cyc_Absyn_FloatType_struct*
_tmp6D0=(struct Cyc_Absyn_FloatType_struct*)_tmp6CE;if(_tmp6D0->tag != 7)goto
_LL362;}_LL361: goto _LL363;_LL362: {struct Cyc_Absyn_DoubleType_struct*_tmp6D1=(
struct Cyc_Absyn_DoubleType_struct*)_tmp6CE;if(_tmp6D1->tag != 8)goto _LL364;}
_LL363: goto _LL365;_LL364: {struct Cyc_Absyn_EnumType_struct*_tmp6D2=(struct Cyc_Absyn_EnumType_struct*)
_tmp6CE;if(_tmp6D2->tag != 14)goto _LL366;}_LL365: goto _LL367;_LL366: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp6D3=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp6CE;if(_tmp6D3->tag != 15)goto
_LL368;}_LL367: return 1;_LL368:;_LL369: return 0;_LL35D:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple0 _tmp12AE;struct _tuple0 _tmp6D5=(_tmp12AE.f1=t1,((_tmp12AE.f2=t2,_tmp12AE)));
void*_tmp6D6;int _tmp6D8;void*_tmp6D9;int _tmp6DB;void*_tmp6DC;void*_tmp6DE;void*
_tmp6E0;void*_tmp6E2;void*_tmp6E4;void*_tmp6E6;void*_tmp6E8;void*_tmp6EA;void*
_tmp6EC;void*_tmp6EE;void*_tmp6F0;enum Cyc_Absyn_Size_of _tmp6F2;void*_tmp6F3;
enum Cyc_Absyn_Size_of _tmp6F5;void*_tmp6F6;enum Cyc_Absyn_Size_of _tmp6F8;void*
_tmp6F9;enum Cyc_Absyn_Size_of _tmp6FB;void*_tmp6FC;enum Cyc_Absyn_Size_of _tmp6FE;
void*_tmp6FF;enum Cyc_Absyn_Size_of _tmp701;void*_tmp702;enum Cyc_Absyn_Size_of
_tmp704;void*_tmp705;enum Cyc_Absyn_Size_of _tmp707;void*_tmp708;void*_tmp70A;
enum Cyc_Absyn_Size_of _tmp70C;void*_tmp70D;void*_tmp70F;enum Cyc_Absyn_Size_of
_tmp711;void*_tmp712;enum Cyc_Absyn_Size_of _tmp714;void*_tmp715;enum Cyc_Absyn_Size_of
_tmp717;void*_tmp718;enum Cyc_Absyn_Size_of _tmp71A;void*_tmp71B;void*_tmp71D;
enum Cyc_Absyn_Size_of _tmp71F;void*_tmp720;enum Cyc_Absyn_Size_of _tmp722;void*
_tmp723;enum Cyc_Absyn_Size_of _tmp725;void*_tmp726;enum Cyc_Absyn_Size_of _tmp728;
void*_tmp729;enum Cyc_Absyn_Size_of _tmp72B;void*_tmp72C;enum Cyc_Absyn_Size_of
_tmp72E;void*_tmp72F;enum Cyc_Absyn_Size_of _tmp731;void*_tmp732;void*_tmp734;
enum Cyc_Absyn_Size_of _tmp736;_LL36B: _tmp6D6=_tmp6D5.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp6D7=(struct Cyc_Absyn_DoubleType_struct*)_tmp6D6;if(_tmp6D7->tag != 8)goto
_LL36D;else{_tmp6D8=_tmp6D7->f1;}};_tmp6D9=_tmp6D5.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp6DA=(struct Cyc_Absyn_DoubleType_struct*)_tmp6D9;if(_tmp6DA->tag != 8)goto
_LL36D;else{_tmp6DB=_tmp6DA->f1;}};_LL36C: return !_tmp6DB  && _tmp6D8;_LL36D:
_tmp6DC=_tmp6D5.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp6DD=(struct Cyc_Absyn_DoubleType_struct*)
_tmp6DC;if(_tmp6DD->tag != 8)goto _LL36F;};_tmp6DE=_tmp6D5.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp6DF=(struct Cyc_Absyn_FloatType_struct*)_tmp6DE;if(_tmp6DF->tag != 7)goto
_LL36F;};_LL36E: goto _LL370;_LL36F: _tmp6E0=_tmp6D5.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp6E1=(struct Cyc_Absyn_DoubleType_struct*)_tmp6E0;if(_tmp6E1->tag != 8)goto
_LL371;};_tmp6E2=_tmp6D5.f2;{struct Cyc_Absyn_IntType_struct*_tmp6E3=(struct Cyc_Absyn_IntType_struct*)
_tmp6E2;if(_tmp6E3->tag != 6)goto _LL371;};_LL370: goto _LL372;_LL371: _tmp6E4=
_tmp6D5.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp6E5=(struct Cyc_Absyn_DoubleType_struct*)
_tmp6E4;if(_tmp6E5->tag != 8)goto _LL373;};_tmp6E6=_tmp6D5.f2;{struct Cyc_Absyn_TagType_struct*
_tmp6E7=(struct Cyc_Absyn_TagType_struct*)_tmp6E6;if(_tmp6E7->tag != 20)goto _LL373;};
_LL372: goto _LL374;_LL373: _tmp6E8=_tmp6D5.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp6E9=(struct Cyc_Absyn_FloatType_struct*)_tmp6E8;if(_tmp6E9->tag != 7)goto
_LL375;};_tmp6EA=_tmp6D5.f2;{struct Cyc_Absyn_TagType_struct*_tmp6EB=(struct Cyc_Absyn_TagType_struct*)
_tmp6EA;if(_tmp6EB->tag != 20)goto _LL375;};_LL374: goto _LL376;_LL375: _tmp6EC=
_tmp6D5.f1;{struct Cyc_Absyn_FloatType_struct*_tmp6ED=(struct Cyc_Absyn_FloatType_struct*)
_tmp6EC;if(_tmp6ED->tag != 7)goto _LL377;};_tmp6EE=_tmp6D5.f2;{struct Cyc_Absyn_IntType_struct*
_tmp6EF=(struct Cyc_Absyn_IntType_struct*)_tmp6EE;if(_tmp6EF->tag != 6)goto _LL377;};
_LL376: return 1;_LL377: _tmp6F0=_tmp6D5.f1;{struct Cyc_Absyn_IntType_struct*_tmp6F1=(
struct Cyc_Absyn_IntType_struct*)_tmp6F0;if(_tmp6F1->tag != 6)goto _LL379;else{
_tmp6F2=_tmp6F1->f2;if(_tmp6F2 != Cyc_Absyn_LongLong_sz)goto _LL379;}};_tmp6F3=
_tmp6D5.f2;{struct Cyc_Absyn_IntType_struct*_tmp6F4=(struct Cyc_Absyn_IntType_struct*)
_tmp6F3;if(_tmp6F4->tag != 6)goto _LL379;else{_tmp6F5=_tmp6F4->f2;if(_tmp6F5 != Cyc_Absyn_LongLong_sz)
goto _LL379;}};_LL378: return 0;_LL379: _tmp6F6=_tmp6D5.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6F7=(struct Cyc_Absyn_IntType_struct*)_tmp6F6;if(_tmp6F7->tag != 6)goto _LL37B;
else{_tmp6F8=_tmp6F7->f2;if(_tmp6F8 != Cyc_Absyn_LongLong_sz)goto _LL37B;}};_LL37A:
return 1;_LL37B: _tmp6F9=_tmp6D5.f1;{struct Cyc_Absyn_IntType_struct*_tmp6FA=(
struct Cyc_Absyn_IntType_struct*)_tmp6F9;if(_tmp6FA->tag != 6)goto _LL37D;else{
_tmp6FB=_tmp6FA->f2;if(_tmp6FB != Cyc_Absyn_Long_sz)goto _LL37D;}};_tmp6FC=_tmp6D5.f2;{
struct Cyc_Absyn_IntType_struct*_tmp6FD=(struct Cyc_Absyn_IntType_struct*)_tmp6FC;
if(_tmp6FD->tag != 6)goto _LL37D;else{_tmp6FE=_tmp6FD->f2;if(_tmp6FE != Cyc_Absyn_Int_sz)
goto _LL37D;}};_LL37C: goto _LL37E;_LL37D: _tmp6FF=_tmp6D5.f1;{struct Cyc_Absyn_IntType_struct*
_tmp700=(struct Cyc_Absyn_IntType_struct*)_tmp6FF;if(_tmp700->tag != 6)goto _LL37F;
else{_tmp701=_tmp700->f2;if(_tmp701 != Cyc_Absyn_Int_sz)goto _LL37F;}};_tmp702=
_tmp6D5.f2;{struct Cyc_Absyn_IntType_struct*_tmp703=(struct Cyc_Absyn_IntType_struct*)
_tmp702;if(_tmp703->tag != 6)goto _LL37F;else{_tmp704=_tmp703->f2;if(_tmp704 != Cyc_Absyn_Long_sz)
goto _LL37F;}};_LL37E: return 0;_LL37F: _tmp705=_tmp6D5.f1;{struct Cyc_Absyn_IntType_struct*
_tmp706=(struct Cyc_Absyn_IntType_struct*)_tmp705;if(_tmp706->tag != 6)goto _LL381;
else{_tmp707=_tmp706->f2;if(_tmp707 != Cyc_Absyn_Long_sz)goto _LL381;}};_tmp708=
_tmp6D5.f2;{struct Cyc_Absyn_FloatType_struct*_tmp709=(struct Cyc_Absyn_FloatType_struct*)
_tmp708;if(_tmp709->tag != 7)goto _LL381;};_LL380: goto _LL382;_LL381: _tmp70A=
_tmp6D5.f1;{struct Cyc_Absyn_IntType_struct*_tmp70B=(struct Cyc_Absyn_IntType_struct*)
_tmp70A;if(_tmp70B->tag != 6)goto _LL383;else{_tmp70C=_tmp70B->f2;if(_tmp70C != Cyc_Absyn_Int_sz)
goto _LL383;}};_tmp70D=_tmp6D5.f2;{struct Cyc_Absyn_FloatType_struct*_tmp70E=(
struct Cyc_Absyn_FloatType_struct*)_tmp70D;if(_tmp70E->tag != 7)goto _LL383;};
_LL382: goto _LL384;_LL383: _tmp70F=_tmp6D5.f1;{struct Cyc_Absyn_IntType_struct*
_tmp710=(struct Cyc_Absyn_IntType_struct*)_tmp70F;if(_tmp710->tag != 6)goto _LL385;
else{_tmp711=_tmp710->f2;if(_tmp711 != Cyc_Absyn_Long_sz)goto _LL385;}};_tmp712=
_tmp6D5.f2;{struct Cyc_Absyn_IntType_struct*_tmp713=(struct Cyc_Absyn_IntType_struct*)
_tmp712;if(_tmp713->tag != 6)goto _LL385;else{_tmp714=_tmp713->f2;if(_tmp714 != Cyc_Absyn_Short_sz)
goto _LL385;}};_LL384: goto _LL386;_LL385: _tmp715=_tmp6D5.f1;{struct Cyc_Absyn_IntType_struct*
_tmp716=(struct Cyc_Absyn_IntType_struct*)_tmp715;if(_tmp716->tag != 6)goto _LL387;
else{_tmp717=_tmp716->f2;if(_tmp717 != Cyc_Absyn_Int_sz)goto _LL387;}};_tmp718=
_tmp6D5.f2;{struct Cyc_Absyn_IntType_struct*_tmp719=(struct Cyc_Absyn_IntType_struct*)
_tmp718;if(_tmp719->tag != 6)goto _LL387;else{_tmp71A=_tmp719->f2;if(_tmp71A != Cyc_Absyn_Short_sz)
goto _LL387;}};_LL386: goto _LL388;_LL387: _tmp71B=_tmp6D5.f1;{struct Cyc_Absyn_TagType_struct*
_tmp71C=(struct Cyc_Absyn_TagType_struct*)_tmp71B;if(_tmp71C->tag != 20)goto _LL389;};
_tmp71D=_tmp6D5.f2;{struct Cyc_Absyn_IntType_struct*_tmp71E=(struct Cyc_Absyn_IntType_struct*)
_tmp71D;if(_tmp71E->tag != 6)goto _LL389;else{_tmp71F=_tmp71E->f2;if(_tmp71F != Cyc_Absyn_Short_sz)
goto _LL389;}};_LL388: goto _LL38A;_LL389: _tmp720=_tmp6D5.f1;{struct Cyc_Absyn_IntType_struct*
_tmp721=(struct Cyc_Absyn_IntType_struct*)_tmp720;if(_tmp721->tag != 6)goto _LL38B;
else{_tmp722=_tmp721->f2;if(_tmp722 != Cyc_Absyn_Long_sz)goto _LL38B;}};_tmp723=
_tmp6D5.f2;{struct Cyc_Absyn_IntType_struct*_tmp724=(struct Cyc_Absyn_IntType_struct*)
_tmp723;if(_tmp724->tag != 6)goto _LL38B;else{_tmp725=_tmp724->f2;if(_tmp725 != Cyc_Absyn_Char_sz)
goto _LL38B;}};_LL38A: goto _LL38C;_LL38B: _tmp726=_tmp6D5.f1;{struct Cyc_Absyn_IntType_struct*
_tmp727=(struct Cyc_Absyn_IntType_struct*)_tmp726;if(_tmp727->tag != 6)goto _LL38D;
else{_tmp728=_tmp727->f2;if(_tmp728 != Cyc_Absyn_Int_sz)goto _LL38D;}};_tmp729=
_tmp6D5.f2;{struct Cyc_Absyn_IntType_struct*_tmp72A=(struct Cyc_Absyn_IntType_struct*)
_tmp729;if(_tmp72A->tag != 6)goto _LL38D;else{_tmp72B=_tmp72A->f2;if(_tmp72B != Cyc_Absyn_Char_sz)
goto _LL38D;}};_LL38C: goto _LL38E;_LL38D: _tmp72C=_tmp6D5.f1;{struct Cyc_Absyn_IntType_struct*
_tmp72D=(struct Cyc_Absyn_IntType_struct*)_tmp72C;if(_tmp72D->tag != 6)goto _LL38F;
else{_tmp72E=_tmp72D->f2;if(_tmp72E != Cyc_Absyn_Short_sz)goto _LL38F;}};_tmp72F=
_tmp6D5.f2;{struct Cyc_Absyn_IntType_struct*_tmp730=(struct Cyc_Absyn_IntType_struct*)
_tmp72F;if(_tmp730->tag != 6)goto _LL38F;else{_tmp731=_tmp730->f2;if(_tmp731 != Cyc_Absyn_Char_sz)
goto _LL38F;}};_LL38E: goto _LL390;_LL38F: _tmp732=_tmp6D5.f1;{struct Cyc_Absyn_TagType_struct*
_tmp733=(struct Cyc_Absyn_TagType_struct*)_tmp732;if(_tmp733->tag != 20)goto _LL391;};
_tmp734=_tmp6D5.f2;{struct Cyc_Absyn_IntType_struct*_tmp735=(struct Cyc_Absyn_IntType_struct*)
_tmp734;if(_tmp735->tag != 6)goto _LL391;else{_tmp736=_tmp735->f2;if(_tmp736 != Cyc_Absyn_Char_sz)
goto _LL391;}};_LL390: return 1;_LL391:;_LL392: return 0;_LL36A:;};}int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct _RegionHandle*
_tmp737=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(
t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(t1,(void*)
max_arith_type->v)){struct Cyc_Core_Opt*_tmp12AF;max_arith_type=((_tmp12AF=
_region_malloc(_tmp737,sizeof(*_tmp12AF)),((_tmp12AF->v=t1,_tmp12AF))));}}}}if(
max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}}{
struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(
te,(struct Cyc_Absyn_Exp*)el->hd,t)){{const char*_tmp12B4;void*_tmp12B3[2];struct
Cyc_String_pa_struct _tmp12B2;struct Cyc_String_pa_struct _tmp12B1;(_tmp12B1.tag=0,((
_tmp12B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v)),((
_tmp12B2.tag=0,((_tmp12B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp12B3[0]=& _tmp12B2,((_tmp12B3[1]=& _tmp12B1,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,((_tmp12B4="type mismatch: expecting %s but found %s",
_tag_dyneither(_tmp12B4,sizeof(char),41))),_tag_dyneither(_tmp12B3,sizeof(void*),
2)))))))))))));}return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,e)){void*_tmp73D=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL394: {struct Cyc_Absyn_PointerType_struct*
_tmp73E=(struct Cyc_Absyn_PointerType_struct*)_tmp73D;if(_tmp73E->tag != 5)goto
_LL396;}_LL395: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);
goto _LL393;_LL396:;_LL397: return 0;_LL393:;}return 1;}int Cyc_Tcutil_is_integral_type(
void*t){void*_tmp73F=Cyc_Tcutil_compress(t);_LL399: {struct Cyc_Absyn_IntType_struct*
_tmp740=(struct Cyc_Absyn_IntType_struct*)_tmp73F;if(_tmp740->tag != 6)goto _LL39B;}
_LL39A: goto _LL39C;_LL39B: {struct Cyc_Absyn_TagType_struct*_tmp741=(struct Cyc_Absyn_TagType_struct*)
_tmp73F;if(_tmp741->tag != 20)goto _LL39D;}_LL39C: goto _LL39E;_LL39D: {struct Cyc_Absyn_EnumType_struct*
_tmp742=(struct Cyc_Absyn_EnumType_struct*)_tmp73F;if(_tmp742->tag != 14)goto
_LL39F;}_LL39E: goto _LL3A0;_LL39F: {struct Cyc_Absyn_AnonEnumType_struct*_tmp743=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp73F;if(_tmp743->tag != 15)goto _LL3A1;}
_LL3A0: return 1;_LL3A1:;_LL3A2: return 0;_LL398:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ)){const
char*_tmp12B7;void*_tmp12B6;(_tmp12B6=0,Cyc_Tcutil_warn(e->loc,((_tmp12B7="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp12B7,sizeof(char),44))),_tag_dyneither(_tmp12B6,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ)){const char*_tmp12BA;void*_tmp12B9;(
_tmp12B9=0,Cyc_Tcutil_warn(e->loc,((_tmp12BA="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp12BA,sizeof(char),44))),_tag_dyneither(_tmp12B9,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct
Cyc_List_List*assume,void*t1,void*t2);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp12BB;struct _tuple0 _tmp749=(_tmp12BB.f1=
t1,((_tmp12BB.f2=t2,_tmp12BB)));void*_tmp74A;struct Cyc_Absyn_PtrInfo _tmp74C;void*
_tmp74D;struct Cyc_Absyn_PtrInfo _tmp74F;void*_tmp750;struct Cyc_Absyn_ArrayInfo
_tmp752;void*_tmp753;struct Cyc_Absyn_Tqual _tmp754;struct Cyc_Absyn_Exp*_tmp755;
union Cyc_Absyn_Constraint*_tmp756;void*_tmp757;struct Cyc_Absyn_ArrayInfo _tmp759;
void*_tmp75A;struct Cyc_Absyn_Tqual _tmp75B;struct Cyc_Absyn_Exp*_tmp75C;union Cyc_Absyn_Constraint*
_tmp75D;void*_tmp75E;void*_tmp760;_LL3A4: _tmp74A=_tmp749.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp74B=(struct Cyc_Absyn_PointerType_struct*)_tmp74A;if(_tmp74B->tag != 5)goto
_LL3A6;else{_tmp74C=_tmp74B->f1;}};_tmp74D=_tmp749.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp74E=(struct Cyc_Absyn_PointerType_struct*)_tmp74D;if(_tmp74E->tag != 5)goto
_LL3A6;else{_tmp74F=_tmp74E->f1;}};_LL3A5: {int okay=1;if(!((int(*)(int(*cmp)(int,
int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp74C.ptr_atts).nullable,(_tmp74F.ptr_atts).nullable))okay=!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp74C.ptr_atts).nullable);
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp74C.ptr_atts).bounds,(
_tmp74F.ptr_atts).bounds)){struct _tuple0 _tmp12BC;struct _tuple0 _tmp763=(_tmp12BC.f1=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(
_tmp74C.ptr_atts).bounds),((_tmp12BC.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp74F.ptr_atts).bounds),
_tmp12BC)));void*_tmp764;void*_tmp766;void*_tmp768;void*_tmp76A;void*_tmp76C;
struct Cyc_Absyn_Exp*_tmp76E;void*_tmp76F;struct Cyc_Absyn_Exp*_tmp771;void*
_tmp772;void*_tmp774;struct Cyc_Absyn_Exp*_tmp776;_LL3AD: _tmp764=_tmp763.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp765=(struct Cyc_Absyn_Upper_b_struct*)_tmp764;
if(_tmp765->tag != 1)goto _LL3AF;};_tmp766=_tmp763.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp767=(struct Cyc_Absyn_DynEither_b_struct*)_tmp766;if(_tmp767->tag != 0)goto
_LL3AF;};_LL3AE: goto _LL3B0;_LL3AF: _tmp768=_tmp763.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp769=(struct Cyc_Absyn_DynEither_b_struct*)_tmp768;if(_tmp769->tag != 0)goto
_LL3B1;};_tmp76A=_tmp763.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp76B=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp76A;if(_tmp76B->tag != 0)goto _LL3B1;};_LL3B0:
okay=1;goto _LL3AC;_LL3B1: _tmp76C=_tmp763.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp76D=(struct Cyc_Absyn_Upper_b_struct*)_tmp76C;if(_tmp76D->tag != 1)goto _LL3B3;
else{_tmp76E=_tmp76D->f1;}};_tmp76F=_tmp763.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp770=(struct Cyc_Absyn_Upper_b_struct*)_tmp76F;if(_tmp770->tag != 1)goto _LL3B3;
else{_tmp771=_tmp770->f1;}};_LL3B2: okay=okay  && Cyc_Evexp_lte_const_exp(_tmp771,
_tmp76E);if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,(_tmp74F.ptr_atts).zero_term)){const char*_tmp12BF;void*_tmp12BE;(_tmp12BE=0,
Cyc_Tcutil_warn(loc,((_tmp12BF="implicit cast to shorter array",_tag_dyneither(
_tmp12BF,sizeof(char),31))),_tag_dyneither(_tmp12BE,sizeof(void*),0)));}goto
_LL3AC;_LL3B3: _tmp772=_tmp763.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp773=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp772;if(_tmp773->tag != 0)goto _LL3AC;};
_tmp774=_tmp763.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp775=(struct Cyc_Absyn_Upper_b_struct*)
_tmp774;if(_tmp775->tag != 1)goto _LL3AC;else{_tmp776=_tmp775->f1;}};_LL3B4: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp74C.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp74F.ptr_atts).bounds))goto _LL3AC;okay=0;goto
_LL3AC;_LL3AC:;}okay=okay  && (!(_tmp74C.elt_tq).real_const  || (_tmp74F.elt_tq).real_const);
okay=okay  && (Cyc_Tcutil_unify((_tmp74C.ptr_atts).rgn,(_tmp74F.ptr_atts).rgn)
 || Cyc_Tcenv_region_outlives(te,(_tmp74C.ptr_atts).rgn,(_tmp74F.ptr_atts).rgn));
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp74C.ptr_atts).zero_term,(
_tmp74F.ptr_atts).zero_term) || ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
1,(_tmp74C.ptr_atts).zero_term) && (_tmp74F.elt_tq).real_const);{int _tmp779=((
int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp74F.ptr_atts).bounds,
Cyc_Absyn_bounds_one_conref) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,(_tmp74F.ptr_atts).zero_term);okay=okay  && (Cyc_Tcutil_unify(_tmp74C.elt_typ,
_tmp74F.elt_typ) || (_tmp779  && ((_tmp74F.elt_tq).real_const  || Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp74F.elt_typ)))) && Cyc_Tcutil_ptrsubtype(
te,0,_tmp74C.elt_typ,_tmp74F.elt_typ));return okay;};}_LL3A6: _tmp750=_tmp749.f1;{
struct Cyc_Absyn_ArrayType_struct*_tmp751=(struct Cyc_Absyn_ArrayType_struct*)
_tmp750;if(_tmp751->tag != 9)goto _LL3A8;else{_tmp752=_tmp751->f1;_tmp753=_tmp752.elt_type;
_tmp754=_tmp752.tq;_tmp755=_tmp752.num_elts;_tmp756=_tmp752.zero_term;}};_tmp757=
_tmp749.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp758=(struct Cyc_Absyn_ArrayType_struct*)
_tmp757;if(_tmp758->tag != 9)goto _LL3A8;else{_tmp759=_tmp758->f1;_tmp75A=_tmp759.elt_type;
_tmp75B=_tmp759.tq;_tmp75C=_tmp759.num_elts;_tmp75D=_tmp759.zero_term;}};_LL3A7: {
int okay;okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp756,_tmp75D) && ((_tmp755 != 0
 && _tmp75C != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp755,(
struct Cyc_Absyn_Exp*)_tmp75C));return(okay  && Cyc_Tcutil_unify(_tmp753,_tmp75A))
 && (!_tmp754.real_const  || _tmp75B.real_const);}_LL3A8: _tmp75E=_tmp749.f1;{
struct Cyc_Absyn_TagType_struct*_tmp75F=(struct Cyc_Absyn_TagType_struct*)_tmp75E;
if(_tmp75F->tag != 20)goto _LL3AA;};_tmp760=_tmp749.f2;{struct Cyc_Absyn_IntType_struct*
_tmp761=(struct Cyc_Absyn_IntType_struct*)_tmp760;if(_tmp761->tag != 6)goto _LL3AA;};
_LL3A9: return 0;_LL3AA:;_LL3AB: return Cyc_Tcutil_unify(t1,t2);_LL3A3:;};}int Cyc_Tcutil_is_pointer_type(
void*t){void*_tmp77A=Cyc_Tcutil_compress(t);_LL3B6: {struct Cyc_Absyn_PointerType_struct*
_tmp77B=(struct Cyc_Absyn_PointerType_struct*)_tmp77A;if(_tmp77B->tag != 5)goto
_LL3B8;}_LL3B7: return 1;_LL3B8:;_LL3B9: return 0;_LL3B5:;}void*Cyc_Tcutil_pointer_elt_type(
void*t){void*_tmp77C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp77E;void*
_tmp77F;_LL3BB: {struct Cyc_Absyn_PointerType_struct*_tmp77D=(struct Cyc_Absyn_PointerType_struct*)
_tmp77C;if(_tmp77D->tag != 5)goto _LL3BD;else{_tmp77E=_tmp77D->f1;_tmp77F=_tmp77E.elt_typ;}}
_LL3BC: return _tmp77F;_LL3BD:;_LL3BE: {const char*_tmp12C2;void*_tmp12C1;(_tmp12C1=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp12C2="pointer_elt_type",_tag_dyneither(_tmp12C2,sizeof(char),17))),
_tag_dyneither(_tmp12C1,sizeof(void*),0)));}_LL3BA:;}void*Cyc_Tcutil_pointer_region(
void*t){void*_tmp782=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp784;
struct Cyc_Absyn_PtrAtts _tmp785;struct Cyc_Absyn_PtrAtts*_tmp786;_LL3C0: {struct
Cyc_Absyn_PointerType_struct*_tmp783=(struct Cyc_Absyn_PointerType_struct*)
_tmp782;if(_tmp783->tag != 5)goto _LL3C2;else{_tmp784=_tmp783->f1;_tmp785=_tmp784.ptr_atts;
_tmp786=(struct Cyc_Absyn_PtrAtts*)&(_tmp783->f1).ptr_atts;}}_LL3C1: return _tmp786->rgn;
_LL3C2:;_LL3C3: {const char*_tmp12C5;void*_tmp12C4;(_tmp12C4=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C5="pointer_elt_type",
_tag_dyneither(_tmp12C5,sizeof(char),17))),_tag_dyneither(_tmp12C4,sizeof(void*),
0)));}_LL3BF:;}int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){void*_tmp789=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp78B;struct Cyc_Absyn_PtrAtts _tmp78C;void*_tmp78D;
_LL3C5: {struct Cyc_Absyn_PointerType_struct*_tmp78A=(struct Cyc_Absyn_PointerType_struct*)
_tmp789;if(_tmp78A->tag != 5)goto _LL3C7;else{_tmp78B=_tmp78A->f1;_tmp78C=_tmp78B.ptr_atts;
_tmp78D=_tmp78C.rgn;}}_LL3C6:*rgn=_tmp78D;return 1;_LL3C7:;_LL3C8: return 0;_LL3C4:;}
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr){void*_tmp78E=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp790;struct Cyc_Absyn_PtrAtts _tmp791;union Cyc_Absyn_Constraint*
_tmp792;_LL3CA: {struct Cyc_Absyn_PointerType_struct*_tmp78F=(struct Cyc_Absyn_PointerType_struct*)
_tmp78E;if(_tmp78F->tag != 5)goto _LL3CC;else{_tmp790=_tmp78F->f1;_tmp791=_tmp790.ptr_atts;
_tmp792=_tmp791.bounds;}}_LL3CB:*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp792)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL3CC:;_LL3CD: return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;
_LL3C9:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){void*_tmp793=e->r;union Cyc_Absyn_Cnst
_tmp795;struct _tuple7 _tmp796;int _tmp797;union Cyc_Absyn_Cnst _tmp799;struct _tuple5
_tmp79A;char _tmp79B;union Cyc_Absyn_Cnst _tmp79D;struct _tuple6 _tmp79E;short _tmp79F;
union Cyc_Absyn_Cnst _tmp7A1;struct _tuple8 _tmp7A2;long long _tmp7A3;void*_tmp7A5;
struct Cyc_Absyn_Exp*_tmp7A6;_LL3CF: {struct Cyc_Absyn_Const_e_struct*_tmp794=(
struct Cyc_Absyn_Const_e_struct*)_tmp793;if(_tmp794->tag != 0)goto _LL3D1;else{
_tmp795=_tmp794->f1;if((_tmp795.Int_c).tag != 4)goto _LL3D1;_tmp796=(struct _tuple7)(
_tmp795.Int_c).val;_tmp797=_tmp796.f2;if(_tmp797 != 0)goto _LL3D1;}}_LL3D0: goto
_LL3D2;_LL3D1: {struct Cyc_Absyn_Const_e_struct*_tmp798=(struct Cyc_Absyn_Const_e_struct*)
_tmp793;if(_tmp798->tag != 0)goto _LL3D3;else{_tmp799=_tmp798->f1;if((_tmp799.Char_c).tag
!= 2)goto _LL3D3;_tmp79A=(struct _tuple5)(_tmp799.Char_c).val;_tmp79B=_tmp79A.f2;
if(_tmp79B != 0)goto _LL3D3;}}_LL3D2: goto _LL3D4;_LL3D3: {struct Cyc_Absyn_Const_e_struct*
_tmp79C=(struct Cyc_Absyn_Const_e_struct*)_tmp793;if(_tmp79C->tag != 0)goto _LL3D5;
else{_tmp79D=_tmp79C->f1;if((_tmp79D.Short_c).tag != 3)goto _LL3D5;_tmp79E=(struct
_tuple6)(_tmp79D.Short_c).val;_tmp79F=_tmp79E.f2;if(_tmp79F != 0)goto _LL3D5;}}
_LL3D4: goto _LL3D6;_LL3D5: {struct Cyc_Absyn_Const_e_struct*_tmp7A0=(struct Cyc_Absyn_Const_e_struct*)
_tmp793;if(_tmp7A0->tag != 0)goto _LL3D7;else{_tmp7A1=_tmp7A0->f1;if((_tmp7A1.LongLong_c).tag
!= 5)goto _LL3D7;_tmp7A2=(struct _tuple8)(_tmp7A1.LongLong_c).val;_tmp7A3=_tmp7A2.f2;
if(_tmp7A3 != 0)goto _LL3D7;}}_LL3D6: return 1;_LL3D7: {struct Cyc_Absyn_Cast_e_struct*
_tmp7A4=(struct Cyc_Absyn_Cast_e_struct*)_tmp793;if(_tmp7A4->tag != 15)goto _LL3D9;
else{_tmp7A5=(void*)_tmp7A4->f1;_tmp7A6=_tmp7A4->f2;}}_LL3D8: return Cyc_Tcutil_is_zero(
_tmp7A6) && Cyc_Tcutil_admits_zero(_tmp7A5);_LL3D9:;_LL3DA: return 0;_LL3CE:;}
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
_tmp7A8;enum Cyc_Absyn_KindQual _tmp7A9;enum Cyc_Absyn_AliasQual _tmp7AA;struct Cyc_Absyn_Kind*
_tmp7A7=ka;_tmp7A8=*_tmp7A7;_tmp7A9=_tmp7A8.kind;_tmp7AA=_tmp7A8.aliasqual;
switch(_tmp7AA){case Cyc_Absyn_Aliasable: _LL3DB: switch(_tmp7A9){case Cyc_Absyn_AnyKind:
_LL3DD: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL3DE:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL3DF: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3E0: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3E1: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3E2: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_iko;}
case Cyc_Absyn_Unique: _LL3DC: switch(_tmp7A9){case Cyc_Absyn_AnyKind: _LL3E5: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL3E6: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3E7: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3E8: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_urko;
default: _LL3E9: break;}break;case Cyc_Absyn_Top: _LL3E4: switch(_tmp7A9){case Cyc_Absyn_AnyKind:
_LL3EC: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3ED:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL3EE: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL3EF: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_trko;default: _LL3F0: break;}break;}{const char*_tmp12C9;
void*_tmp12C8[1];struct Cyc_String_pa_struct _tmp12C7;(_tmp12C7.tag=0,((_tmp12C7.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((
_tmp12C8[0]=& _tmp12C7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12C9="kind_to_opt: bad kind %s\n",_tag_dyneither(
_tmp12C9,sizeof(char),26))),_tag_dyneither(_tmp12C8,sizeof(void*),1)))))));};}
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
_tmp7AF;enum Cyc_Absyn_KindQual _tmp7B0;enum Cyc_Absyn_AliasQual _tmp7B1;struct Cyc_Absyn_Kind*
_tmp7AE=ka;_tmp7AF=*_tmp7AE;_tmp7B0=_tmp7AF.kind;_tmp7B1=_tmp7AF.aliasqual;
switch(_tmp7B1){case Cyc_Absyn_Aliasable: _LL3F2: switch(_tmp7B0){case Cyc_Absyn_AnyKind:
_LL3F4: return& ab;case Cyc_Absyn_MemKind: _LL3F5: return& mb;case Cyc_Absyn_BoxKind:
_LL3F6: return& bb;case Cyc_Absyn_RgnKind: _LL3F7: return& rb;case Cyc_Absyn_EffKind:
_LL3F8: return& eb;case Cyc_Absyn_IntKind: _LL3F9: return& ib;}case Cyc_Absyn_Unique:
_LL3F3: switch(_tmp7B0){case Cyc_Absyn_AnyKind: _LL3FC: return& uab;case Cyc_Absyn_MemKind:
_LL3FD: return& umb;case Cyc_Absyn_BoxKind: _LL3FE: return& ubb;case Cyc_Absyn_RgnKind:
_LL3FF: return& urb;default: _LL400: break;}break;case Cyc_Absyn_Top: _LL3FB: switch(
_tmp7B0){case Cyc_Absyn_AnyKind: _LL403: return& tab;case Cyc_Absyn_MemKind: _LL404:
return& tmb;case Cyc_Absyn_BoxKind: _LL405: return& tbb;case Cyc_Absyn_RgnKind: _LL406:
return& trb;default: _LL407: break;}break;}{const char*_tmp12CD;void*_tmp12CC[1];
struct Cyc_String_pa_struct _tmp12CB;(_tmp12CB.tag=0,((_tmp12CB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp12CC[0]=&
_tmp12CB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp12CD="kind_to_b: bad kind %s\n",_tag_dyneither(_tmp12CD,sizeof(char),24))),
_tag_dyneither(_tmp12CC,sizeof(void*),1)))))));};}void*Cyc_Tcutil_kind_to_bound(
struct Cyc_Absyn_Kind*k){return*Cyc_Tcutil_kind_to_b(k);}struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(
struct Cyc_Absyn_Kind*k){struct Cyc_Core_Opt*_tmp12CE;return(_tmp12CE=_cycalloc(
sizeof(*_tmp12CE)),((_tmp12CE->v=Cyc_Tcutil_kind_to_bound(k),_tmp12CE)));}int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_struct _tmp12D1;struct Cyc_Absyn_Const_e_struct*
_tmp12D0;e1->r=(void*)((_tmp12D0=_cycalloc(sizeof(*_tmp12D0)),((_tmp12D0[0]=((
_tmp12D1.tag=0,((_tmp12D1.f1=Cyc_Absyn_Null_c,_tmp12D1)))),_tmp12D0))));}{struct
Cyc_Core_Opt*_tmp7C6=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct
_tmp12DB;struct Cyc_Absyn_PtrAtts _tmp12DA;struct Cyc_Absyn_PtrInfo _tmp12D9;struct
Cyc_Absyn_PointerType_struct*_tmp12D8;struct Cyc_Absyn_PointerType_struct*_tmp7C7=(
_tmp12D8=_cycalloc(sizeof(*_tmp12D8)),((_tmp12D8[0]=((_tmp12DB.tag=5,((_tmp12DB.f1=((
_tmp12D9.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmp7C6),((
_tmp12D9.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp12D9.ptr_atts=((_tmp12DA.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,_tmp7C6),((_tmp12DA.nullable=Cyc_Absyn_true_conref,((
_tmp12DA.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp12DA.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp12DA.ptrloc=0,_tmp12DA)))))))))),_tmp12D9)))))),_tmp12DB)))),_tmp12D8)));((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp7C7);{int bogus=
0;int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);if(bogus != 0){const char*
_tmp12E0;void*_tmp12DF[2];struct Cyc_String_pa_struct _tmp12DE;struct Cyc_String_pa_struct
_tmp12DD;(_tmp12DD.tag=0,((_tmp12DD.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Position_string_of_segment(e1->loc)),((_tmp12DE.tag=0,((
_tmp12DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp12DF[0]=& _tmp12DE,((_tmp12DF[1]=& _tmp12DD,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12E0="zero_to_null resulted in an alias coercion on %s at %s\n",
_tag_dyneither(_tmp12E0,sizeof(char),56))),_tag_dyneither(_tmp12DF,sizeof(void*),
2)))))))))))));}return retv;};};}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(
enum Cyc_Absyn_Coercion c){switch(c){case Cyc_Absyn_Unknown_coercion: _LL409: {
const char*_tmp12E1;return(_tmp12E1="unknown",_tag_dyneither(_tmp12E1,sizeof(char),
8));}case Cyc_Absyn_No_coercion: _LL40A: {const char*_tmp12E2;return(_tmp12E2="no coercion",
_tag_dyneither(_tmp12E2,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL40B: {
const char*_tmp12E3;return(_tmp12E3="null check",_tag_dyneither(_tmp12E3,sizeof(
char),11));}case Cyc_Absyn_Other_coercion: _LL40C: {const char*_tmp12E4;return(
_tmp12E4="other coercion",_tag_dyneither(_tmp12E4,sizeof(char),15));}}}int Cyc_Tcutil_warn_alias_coerce=
0;struct _tuple14 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){static
struct Cyc_Absyn_Eq_kb_struct rgn_kb={0,& Cyc_Tcutil_rk};static int counter=0;struct
_dyneither_ptr*_tmp12F1;const char*_tmp12F0;void*_tmp12EF[1];struct Cyc_Int_pa_struct
_tmp12EE;struct _tuple2*_tmp12ED;struct _tuple2*v=(_tmp12ED=_cycalloc(sizeof(*
_tmp12ED)),((_tmp12ED->f1=Cyc_Absyn_Loc_n,((_tmp12ED->f2=((_tmp12F1=_cycalloc(
sizeof(*_tmp12F1)),((_tmp12F1[0]=(struct _dyneither_ptr)((_tmp12EE.tag=1,((
_tmp12EE.f1=(unsigned long)counter ++,((_tmp12EF[0]=& _tmp12EE,Cyc_aprintf(((
_tmp12F0="__aliasvar%d",_tag_dyneither(_tmp12F0,sizeof(char),13))),
_tag_dyneither(_tmp12EF,sizeof(void*),1)))))))),_tmp12F1)))),_tmp12ED)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,(void*)& Cyc_Absyn_VoidType_val,
0);struct Cyc_Absyn_Local_b_struct _tmp12F4;struct Cyc_Absyn_Local_b_struct*
_tmp12F3;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmp12F3=_cycalloc(
sizeof(*_tmp12F3)),((_tmp12F3[0]=((_tmp12F4.tag=4,((_tmp12F4.f1=vd,_tmp12F4)))),
_tmp12F3)))),e->loc);struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);{
void*_tmp7D4=Cyc_Tcutil_compress(e_typ);struct Cyc_Absyn_PtrInfo _tmp7D6;struct Cyc_Absyn_PtrAtts
_tmp7D7;void*_tmp7D8;void**_tmp7D9;_LL40F: {struct Cyc_Absyn_PointerType_struct*
_tmp7D5=(struct Cyc_Absyn_PointerType_struct*)_tmp7D4;if(_tmp7D5->tag != 5)goto
_LL411;else{_tmp7D6=_tmp7D5->f1;_tmp7D7=_tmp7D6.ptr_atts;_tmp7D8=_tmp7D7.rgn;
_tmp7D9=(void**)&((_tmp7D5->f1).ptr_atts).rgn;}}_LL410:{struct Cyc_Absyn_VarType_struct
_tmp12F7;struct Cyc_Absyn_VarType_struct*_tmp12F6;*_tmp7D9=(void*)((_tmp12F6=
_cycalloc(sizeof(*_tmp12F6)),((_tmp12F6[0]=((_tmp12F7.tag=2,((_tmp12F7.f1=tv,
_tmp12F7)))),_tmp12F6))));}goto _LL40E;_LL411:;_LL412: goto _LL40E;_LL40E:;}e->topt=
0;e=Cyc_Absyn_cast_exp(e_typ,e,0,Cyc_Absyn_Unknown_coercion,e->loc);{struct Cyc_Absyn_Decl*
d=Cyc_Absyn_alias_decl(e,tv,vd,e->loc);struct _tuple14 _tmp12F8;return(_tmp12F8.f1=
d,((_tmp12F8.f2=ve,_tmp12F8)));};}static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*
e,void*e_typ,void*wants_typ,struct Cyc_Position_Segment*loc){struct _RegionHandle
_tmp7E5=_new_region("r");struct _RegionHandle*r=& _tmp7E5;_push_region(r);if((Cyc_Tcutil_is_noalias_path(
r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ)) && Cyc_Tcutil_is_pointer_type(
e_typ)){void*_tmp7E6=Cyc_Tcutil_compress(wants_typ);struct Cyc_Absyn_PtrInfo
_tmp7E8;struct Cyc_Absyn_PtrAtts _tmp7E9;void*_tmp7EA;_LL414: {struct Cyc_Absyn_PointerType_struct*
_tmp7E7=(struct Cyc_Absyn_PointerType_struct*)_tmp7E6;if(_tmp7E7->tag != 5)goto
_LL416;else{_tmp7E8=_tmp7E7->f1;_tmp7E9=_tmp7E8.ptr_atts;_tmp7EA=_tmp7E9.rgn;}}
_LL415: {void*_tmp7EC=Cyc_Tcutil_compress(_tmp7EA);_LL41B: {struct Cyc_Absyn_HeapRgn_struct*
_tmp7ED=(struct Cyc_Absyn_HeapRgn_struct*)_tmp7EC;if(_tmp7ED->tag != 21)goto _LL41D;}
_LL41C: {int _tmp7EE=0;_npop_handler(0);return _tmp7EE;}_LL41D:;_LL41E: {struct Cyc_Absyn_Kind*
_tmp7EF=Cyc_Tcutil_typ_kind(_tmp7EA);int _tmp7F0=_tmp7EF->kind == Cyc_Absyn_RgnKind
 && _tmp7EF->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp7F0;}
_LL41A:;}_LL416: {struct Cyc_Absyn_TypedefType_struct*_tmp7EB=(struct Cyc_Absyn_TypedefType_struct*)
_tmp7E6;if(_tmp7EB->tag != 18)goto _LL418;}_LL417: {const char*_tmp12FD;void*
_tmp12FC[2];struct Cyc_String_pa_struct _tmp12FB;struct Cyc_String_pa_struct
_tmp12FA;(_tmp12FA.tag=0,((_tmp12FA.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Position_string_of_segment(loc)),((_tmp12FB.tag=0,((_tmp12FB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(
wants_typ))),((_tmp12FC[0]=& _tmp12FB,((_tmp12FC[1]=& _tmp12FA,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12FD="got typedef %s in can_insert_alias at %s\n",
_tag_dyneither(_tmp12FD,sizeof(char),42))),_tag_dyneither(_tmp12FC,sizeof(void*),
2)))))))))))));}_LL418:;_LL419: {int _tmp7F5=0;_npop_handler(0);return _tmp7F5;}
_LL413:;}{int _tmp7F6=0;_npop_handler(0);return _tmp7F6;};;_pop_region(r);}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){void*
t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
enum Cyc_Absyn_Coercion c;int do_alias_coercion=0;if(Cyc_Tcutil_unify(t1,t2))
return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
if(Cyc_Tcutil_will_lose_precision(t1,t2)){const char*_tmp1302;void*_tmp1301[2];
struct Cyc_String_pa_struct _tmp1300;struct Cyc_String_pa_struct _tmp12FF;(_tmp12FF.tag=
0,((_tmp12FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp1300.tag=0,((_tmp1300.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp1301[0]=& _tmp1300,((_tmp1301[1]=& _tmp12FF,Cyc_Tcutil_warn(
e->loc,((_tmp1302="integral size mismatch; %s -> %s conversion supplied",
_tag_dyneither(_tmp1302,sizeof(char),53))),_tag_dyneither(_tmp1301,sizeof(void*),
2)))))))))))));}Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);return 1;}
else{if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmp1308;void*_tmp1307[3];struct Cyc_String_pa_struct _tmp1306;struct Cyc_String_pa_struct
_tmp1305;struct Cyc_String_pa_struct _tmp1304;(_tmp1304.tag=0,((_tmp1304.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((
_tmp1305.tag=0,((_tmp1305.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp1306.tag=0,((_tmp1306.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmp1307[0]=& _tmp1306,((_tmp1307[1]=& _tmp1305,((
_tmp1307[2]=& _tmp1304,Cyc_Tcutil_warn(e->loc,((_tmp1308="implicit alias coercion for %s:%s to %s",
_tag_dyneither(_tmp1308,sizeof(char),40))),_tag_dyneither(_tmp1307,sizeof(void*),
3)))))))))))))))))));}*alias_coercion=1;}if(Cyc_Tcutil_silent_castable(te,e->loc,
t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);return 1;}
else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if((c=Cyc_Tcutil_castable(
te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){Cyc_Tcutil_unchecked_cast(te,e,
t2,c);if(c != Cyc_Absyn_NonNull_to_Null){const char*_tmp130D;void*_tmp130C[2];
struct Cyc_String_pa_struct _tmp130B;struct Cyc_String_pa_struct _tmp130A;(_tmp130A.tag=
0,((_tmp130A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp130B.tag=0,((_tmp130B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp130C[0]=& _tmp130B,((_tmp130C[1]=& _tmp130A,Cyc_Tcutil_warn(
e->loc,((_tmp130D="implicit cast from %s to %s",_tag_dyneither(_tmp130D,sizeof(
char),28))),_tag_dyneither(_tmp130C,sizeof(void*),2)))))))))))));}return 1;}else{
return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t){int bogus=0;return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}int Cyc_Tcutil_coerceable(
void*t){void*_tmp804=Cyc_Tcutil_compress(t);_LL420: {struct Cyc_Absyn_IntType_struct*
_tmp805=(struct Cyc_Absyn_IntType_struct*)_tmp804;if(_tmp805->tag != 6)goto _LL422;}
_LL421: goto _LL423;_LL422: {struct Cyc_Absyn_FloatType_struct*_tmp806=(struct Cyc_Absyn_FloatType_struct*)
_tmp804;if(_tmp806->tag != 7)goto _LL424;}_LL423: goto _LL425;_LL424: {struct Cyc_Absyn_DoubleType_struct*
_tmp807=(struct Cyc_Absyn_DoubleType_struct*)_tmp804;if(_tmp807->tag != 8)goto
_LL426;}_LL425: return 1;_LL426:;_LL427: return 0;_LL41F:;}static struct Cyc_List_List*
Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,
void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*
f3;int f4;};static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,
struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp809;struct _RegionHandle*
_tmp80A;struct Cyc_Tcenv_Tenv*_tmp80B;int _tmp80C;struct _tuple23 _tmp808=*env;
_tmp809=_tmp808.f1;_tmp80A=_tmp808.f2;_tmp80B=_tmp808.f3;_tmp80C=_tmp808.f4;{
void*_tmp80D=Cyc_Tcutil_rsubstitute(_tmp80A,_tmp809,x->type);struct Cyc_List_List*
_tmp80E=Cyc_Tcutil_flatten_typ(_tmp80A,_tmp80C,_tmp80B,_tmp80D);if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp80E)== 1){struct _tuple11*_tmp1310;
struct Cyc_List_List*_tmp130F;return(_tmp130F=_region_malloc(_tmp80A,sizeof(*
_tmp130F)),((_tmp130F->hd=((_tmp1310=_region_malloc(_tmp80A,sizeof(*_tmp1310)),((
_tmp1310->f1=x->tq,((_tmp1310->f2=_tmp80D,_tmp1310)))))),((_tmp130F->tl=0,
_tmp130F)))));}else{return _tmp80E;}};}struct _tuple24{struct _RegionHandle*f1;
struct Cyc_Tcenv_Tenv*f2;int f3;};static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(
struct _tuple24*env,struct _tuple11*x){struct _RegionHandle*_tmp812;struct Cyc_Tcenv_Tenv*
_tmp813;int _tmp814;struct _tuple24 _tmp811=*env;_tmp812=_tmp811.f1;_tmp813=_tmp811.f2;
_tmp814=_tmp811.f3;{struct Cyc_Absyn_Tqual _tmp816;void*_tmp817;struct _tuple11
_tmp815=*x;_tmp816=_tmp815.f1;_tmp817=_tmp815.f2;{struct Cyc_List_List*_tmp818=
Cyc_Tcutil_flatten_typ(_tmp812,_tmp814,_tmp813,_tmp817);if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp818)== 1){struct _tuple11*_tmp1313;struct Cyc_List_List*
_tmp1312;return(_tmp1312=_region_malloc(_tmp812,sizeof(*_tmp1312)),((_tmp1312->hd=((
_tmp1313=_region_malloc(_tmp812,sizeof(*_tmp1313)),((_tmp1313->f1=_tmp816,((
_tmp1313->f2=_tmp817,_tmp1313)))))),((_tmp1312->tl=0,_tmp1312)))));}else{return
_tmp818;}};};}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct
_RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){if(flatten){t1=Cyc_Tcutil_compress(
t1);{void*_tmp81B=t1;struct Cyc_List_List*_tmp81E;struct Cyc_Absyn_AggrInfo _tmp820;
union Cyc_Absyn_AggrInfoU _tmp821;struct Cyc_Absyn_Aggrdecl**_tmp822;struct Cyc_Absyn_Aggrdecl*
_tmp823;struct Cyc_List_List*_tmp824;enum Cyc_Absyn_AggrKind _tmp826;struct Cyc_List_List*
_tmp827;_LL429: {struct Cyc_Absyn_VoidType_struct*_tmp81C=(struct Cyc_Absyn_VoidType_struct*)
_tmp81B;if(_tmp81C->tag != 0)goto _LL42B;}_LL42A: return 0;_LL42B: {struct Cyc_Absyn_TupleType_struct*
_tmp81D=(struct Cyc_Absyn_TupleType_struct*)_tmp81B;if(_tmp81D->tag != 11)goto
_LL42D;else{_tmp81E=_tmp81D->f1;}}_LL42C: {struct _tuple24 _tmp1314;struct _tuple24
_tmp828=(_tmp1314.f1=r,((_tmp1314.f2=te,((_tmp1314.f3=flatten,_tmp1314)))));
struct Cyc_List_List*_tmp829=_tmp81E;struct Cyc_List_List _tmp82A;struct _tuple11*
_tmp82B;struct Cyc_List_List*_tmp82C;_LL434: if(_tmp829 != 0)goto _LL436;_LL435:
return 0;_LL436: if(_tmp829 == 0)goto _LL433;_tmp82A=*_tmp829;_tmp82B=(struct
_tuple11*)_tmp82A.hd;_tmp82C=_tmp82A.tl;_LL437: {struct Cyc_List_List*_tmp82D=Cyc_Tcutil_rcopy_tqt(&
_tmp828,_tmp82B);_tmp828.f3=0;{struct Cyc_List_List*_tmp82E=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple11*),
struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,&
_tmp828,_tmp81E);struct Cyc_List_List*_tmp1315;struct Cyc_List_List*_tmp82F=(
_tmp1315=_region_malloc(r,sizeof(*_tmp1315)),((_tmp1315->hd=_tmp82D,((_tmp1315->tl=
_tmp82E,_tmp1315)))));struct Cyc_List_List*_tmp1316;return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp1316=
_region_malloc(r,sizeof(*_tmp1316)),((_tmp1316->hd=_tmp82D,((_tmp1316->tl=
_tmp82E,_tmp1316)))))));};}_LL433:;}_LL42D: {struct Cyc_Absyn_AggrType_struct*
_tmp81F=(struct Cyc_Absyn_AggrType_struct*)_tmp81B;if(_tmp81F->tag != 12)goto
_LL42F;else{_tmp820=_tmp81F->f1;_tmp821=_tmp820.aggr_info;if((_tmp821.KnownAggr).tag
!= 2)goto _LL42F;_tmp822=(struct Cyc_Absyn_Aggrdecl**)(_tmp821.KnownAggr).val;
_tmp823=*_tmp822;_tmp824=_tmp820.targs;}}_LL42E: if(((_tmp823->kind == Cyc_Absyn_UnionA
 || _tmp823->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp823->impl))->exist_vars
!= 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp823->impl))->rgn_po != 0){
struct _tuple11*_tmp1319;struct Cyc_List_List*_tmp1318;return(_tmp1318=
_region_malloc(r,sizeof(*_tmp1318)),((_tmp1318->hd=((_tmp1319=_region_malloc(r,
sizeof(*_tmp1319)),((_tmp1319->f1=Cyc_Absyn_empty_tqual(0),((_tmp1319->f2=t1,
_tmp1319)))))),((_tmp1318->tl=0,_tmp1318)))));}{struct Cyc_List_List*_tmp835=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp823->tvs,_tmp824);struct _tuple23
_tmp131A;struct _tuple23 env=(_tmp131A.f1=_tmp835,((_tmp131A.f2=r,((_tmp131A.f3=te,((
_tmp131A.f4=flatten,_tmp131A)))))));struct Cyc_List_List*_tmp836=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp823->impl))->fields;struct Cyc_List_List _tmp837;struct Cyc_Absyn_Aggrfield*
_tmp838;struct Cyc_List_List*_tmp839;_LL439: if(_tmp836 != 0)goto _LL43B;_LL43A:
return 0;_LL43B: if(_tmp836 == 0)goto _LL438;_tmp837=*_tmp836;_tmp838=(struct Cyc_Absyn_Aggrfield*)
_tmp837.hd;_tmp839=_tmp837.tl;_LL43C: {struct Cyc_List_List*_tmp83A=Cyc_Tcutil_flatten_typ_f(&
env,_tmp838);env.f4=0;{struct Cyc_List_List*_tmp83B=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),
struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp839);struct Cyc_List_List*_tmp131B;struct Cyc_List_List*_tmp83C=(_tmp131B=
_region_malloc(r,sizeof(*_tmp131B)),((_tmp131B->hd=_tmp83A,((_tmp131B->tl=
_tmp83B,_tmp131B)))));return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp83C);};}_LL438:;};_LL42F: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp825=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp81B;if(_tmp825->tag != 13)goto _LL431;else{_tmp826=_tmp825->f1;if(_tmp826 != 
Cyc_Absyn_StructA)goto _LL431;_tmp827=_tmp825->f2;}}_LL430: {struct _tuple23
_tmp131C;struct _tuple23 env=(_tmp131C.f1=0,((_tmp131C.f2=r,((_tmp131C.f3=te,((
_tmp131C.f4=flatten,_tmp131C)))))));struct Cyc_List_List*_tmp83F=_tmp827;struct
Cyc_List_List _tmp840;struct Cyc_Absyn_Aggrfield*_tmp841;struct Cyc_List_List*
_tmp842;_LL43E: if(_tmp83F != 0)goto _LL440;_LL43F: return 0;_LL440: if(_tmp83F == 0)
goto _LL43D;_tmp840=*_tmp83F;_tmp841=(struct Cyc_Absyn_Aggrfield*)_tmp840.hd;
_tmp842=_tmp840.tl;_LL441: {struct Cyc_List_List*_tmp843=Cyc_Tcutil_flatten_typ_f(&
env,_tmp841);env.f4=0;{struct Cyc_List_List*_tmp844=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),
struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp842);struct Cyc_List_List*_tmp131D;struct Cyc_List_List*_tmp845=(_tmp131D=
_region_malloc(r,sizeof(*_tmp131D)),((_tmp131D->hd=_tmp843,((_tmp131D->tl=
_tmp844,_tmp131D)))));return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp845);};}_LL43D:;}_LL431:;_LL432:
goto _LL428;_LL428:;};}{struct _tuple11*_tmp1320;struct Cyc_List_List*_tmp131F;
return(_tmp131F=_region_malloc(r,sizeof(*_tmp131F)),((_tmp131F->hd=((_tmp1320=
_region_malloc(r,sizeof(*_tmp1320)),((_tmp1320->f1=Cyc_Absyn_empty_tqual(0),((
_tmp1320->f2=t1,_tmp1320)))))),((_tmp131F->tl=0,_tmp131F)))));};}static int Cyc_Tcutil_sub_attributes(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t
!= 0;t=t->tl){void*_tmp84A=(void*)t->hd;_LL443: {struct Cyc_Absyn_Pure_att_struct*
_tmp84B=(struct Cyc_Absyn_Pure_att_struct*)_tmp84A;if(_tmp84B->tag != 22)goto
_LL445;}_LL444: goto _LL446;_LL445: {struct Cyc_Absyn_Noreturn_att_struct*_tmp84C=(
struct Cyc_Absyn_Noreturn_att_struct*)_tmp84A;if(_tmp84C->tag != 4)goto _LL447;}
_LL446: goto _LL448;_LL447: {struct Cyc_Absyn_Initializes_att_struct*_tmp84D=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp84A;if(_tmp84D->tag != 20)goto _LL449;}
_LL448: continue;_LL449:;_LL44A: if(!((int(*)(int(*pred)(void*,void*),void*env,
struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))
return 0;_LL442:;}}for(0;a2 != 0;a2=a2->tl){if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)
a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp1321;struct _tuple0 _tmp84F=(_tmp1321.f1=Cyc_Tcutil_compress(t1),((
_tmp1321.f2=Cyc_Tcutil_compress(t2),_tmp1321)));void*_tmp850;enum Cyc_Absyn_Size_of
_tmp852;void*_tmp853;enum Cyc_Absyn_Size_of _tmp855;_LL44C: _tmp850=_tmp84F.f1;{
struct Cyc_Absyn_IntType_struct*_tmp851=(struct Cyc_Absyn_IntType_struct*)_tmp850;
if(_tmp851->tag != 6)goto _LL44E;else{_tmp852=_tmp851->f2;}};_tmp853=_tmp84F.f2;{
struct Cyc_Absyn_IntType_struct*_tmp854=(struct Cyc_Absyn_IntType_struct*)_tmp853;
if(_tmp854->tag != 6)goto _LL44E;else{_tmp855=_tmp854->f2;}};_LL44D: return(_tmp852
== _tmp855  || _tmp852 == Cyc_Absyn_Int_sz  && _tmp855 == Cyc_Absyn_Long_sz) || 
_tmp852 == Cyc_Absyn_Long_sz  && _tmp855 == Cyc_Absyn_Int_sz;_LL44E:;_LL44F: return 0;
_LL44B:;}static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return 1;{struct Cyc_List_List*
a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1)
 && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))return 1;}}t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp1322;struct _tuple0 _tmp857=(
_tmp1322.f1=t1,((_tmp1322.f2=t2,_tmp1322)));void*_tmp858;struct Cyc_Absyn_PtrInfo
_tmp85A;void*_tmp85B;struct Cyc_Absyn_Tqual _tmp85C;struct Cyc_Absyn_PtrAtts _tmp85D;
void*_tmp85E;union Cyc_Absyn_Constraint*_tmp85F;union Cyc_Absyn_Constraint*_tmp860;
union Cyc_Absyn_Constraint*_tmp861;void*_tmp862;struct Cyc_Absyn_PtrInfo _tmp864;
void*_tmp865;struct Cyc_Absyn_Tqual _tmp866;struct Cyc_Absyn_PtrAtts _tmp867;void*
_tmp868;union Cyc_Absyn_Constraint*_tmp869;union Cyc_Absyn_Constraint*_tmp86A;
union Cyc_Absyn_Constraint*_tmp86B;void*_tmp86C;struct Cyc_Absyn_DatatypeFieldInfo
_tmp86E;union Cyc_Absyn_DatatypeFieldInfoU _tmp86F;struct _tuple3 _tmp870;struct Cyc_Absyn_Datatypedecl*
_tmp871;struct Cyc_Absyn_Datatypefield*_tmp872;struct Cyc_List_List*_tmp873;void*
_tmp874;struct Cyc_Absyn_DatatypeInfo _tmp876;union Cyc_Absyn_DatatypeInfoU _tmp877;
struct Cyc_Absyn_Datatypedecl**_tmp878;struct Cyc_Absyn_Datatypedecl*_tmp879;
struct Cyc_List_List*_tmp87A;void*_tmp87B;struct Cyc_Absyn_FnInfo _tmp87D;void*
_tmp87E;struct Cyc_Absyn_FnInfo _tmp880;_LL451: _tmp858=_tmp857.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp859=(struct Cyc_Absyn_PointerType_struct*)_tmp858;if(_tmp859->tag != 5)goto
_LL453;else{_tmp85A=_tmp859->f1;_tmp85B=_tmp85A.elt_typ;_tmp85C=_tmp85A.elt_tq;
_tmp85D=_tmp85A.ptr_atts;_tmp85E=_tmp85D.rgn;_tmp85F=_tmp85D.nullable;_tmp860=
_tmp85D.bounds;_tmp861=_tmp85D.zero_term;}};_tmp862=_tmp857.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp863=(struct Cyc_Absyn_PointerType_struct*)_tmp862;if(_tmp863->tag != 5)goto
_LL453;else{_tmp864=_tmp863->f1;_tmp865=_tmp864.elt_typ;_tmp866=_tmp864.elt_tq;
_tmp867=_tmp864.ptr_atts;_tmp868=_tmp867.rgn;_tmp869=_tmp867.nullable;_tmp86A=
_tmp867.bounds;_tmp86B=_tmp867.zero_term;}};_LL452: if(_tmp85C.real_const  && !
_tmp866.real_const)return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp85F,
_tmp869) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp85F)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp869))return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,
union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp861,
_tmp86B) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp861)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp86B))return 0;if(!Cyc_Tcutil_unify(_tmp85E,_tmp868) && !Cyc_Tcenv_region_outlives(
te,_tmp85E,_tmp868))return 0;if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp860,_tmp86A)){struct _tuple0 _tmp1323;struct _tuple0 _tmp882=(_tmp1323.f1=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp860),((_tmp1323.f2=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp86A),_tmp1323)));void*
_tmp883;void*_tmp885;void*_tmp887;struct Cyc_Absyn_Exp*_tmp889;void*_tmp88A;
struct Cyc_Absyn_Exp*_tmp88C;_LL45A: _tmp883=_tmp882.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp884=(struct Cyc_Absyn_Upper_b_struct*)_tmp883;if(_tmp884->tag != 1)goto _LL45C;};
_tmp885=_tmp882.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp886=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp885;if(_tmp886->tag != 0)goto _LL45C;};_LL45B: goto _LL459;_LL45C: _tmp887=
_tmp882.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp888=(struct Cyc_Absyn_Upper_b_struct*)
_tmp887;if(_tmp888->tag != 1)goto _LL45E;else{_tmp889=_tmp888->f1;}};_tmp88A=
_tmp882.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp88B=(struct Cyc_Absyn_Upper_b_struct*)
_tmp88A;if(_tmp88B->tag != 1)goto _LL45E;else{_tmp88C=_tmp88B->f1;}};_LL45D: if(!
Cyc_Evexp_lte_const_exp(_tmp88C,_tmp889))return 0;goto _LL459;_LL45E:;_LL45F:
return 0;_LL459:;}if(!_tmp866.real_const  && _tmp85C.real_const){if(!Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp865)))return 0;}{int _tmp88D=((int(*)(int(*
cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_unify_it_bounds,_tmp86A,Cyc_Absyn_bounds_one_conref) && !((int(*)(int
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp86B);struct _tuple0*
_tmp1326;struct Cyc_List_List*_tmp1325;return(_tmp88D  && Cyc_Tcutil_ptrsubtype(te,((
_tmp1325=_cycalloc(sizeof(*_tmp1325)),((_tmp1325->hd=((_tmp1326=_cycalloc(
sizeof(*_tmp1326)),((_tmp1326->f1=t1,((_tmp1326->f2=t2,_tmp1326)))))),((_tmp1325->tl=
assume,_tmp1325)))))),_tmp85B,_tmp865) || Cyc_Tcutil_unify(t1,t2)) || Cyc_Tcutil_isomorphic(
t1,t2);};_LL453: _tmp86C=_tmp857.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp86D=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp86C;if(_tmp86D->tag != 4)
goto _LL455;else{_tmp86E=_tmp86D->f1;_tmp86F=_tmp86E.field_info;if((_tmp86F.KnownDatatypefield).tag
!= 2)goto _LL455;_tmp870=(struct _tuple3)(_tmp86F.KnownDatatypefield).val;_tmp871=
_tmp870.f1;_tmp872=_tmp870.f2;_tmp873=_tmp86E.targs;}};_tmp874=_tmp857.f2;{
struct Cyc_Absyn_DatatypeType_struct*_tmp875=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp874;if(_tmp875->tag != 3)goto _LL455;else{_tmp876=_tmp875->f1;_tmp877=_tmp876.datatype_info;
if((_tmp877.KnownDatatype).tag != 2)goto _LL455;_tmp878=(struct Cyc_Absyn_Datatypedecl**)(
_tmp877.KnownDatatype).val;_tmp879=*_tmp878;_tmp87A=_tmp876.targs;}};_LL454: if(
_tmp871 != _tmp879  && Cyc_Absyn_qvar_cmp(_tmp871->name,_tmp879->name)!= 0)return 0;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp873)!= ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp87A))return 0;for(0;_tmp873 != 0;(_tmp873=
_tmp873->tl,_tmp87A=_tmp87A->tl)){if(!Cyc_Tcutil_unify((void*)_tmp873->hd,(void*)((
struct Cyc_List_List*)_check_null(_tmp87A))->hd))return 0;}return 1;_LL455: _tmp87B=
_tmp857.f1;{struct Cyc_Absyn_FnType_struct*_tmp87C=(struct Cyc_Absyn_FnType_struct*)
_tmp87B;if(_tmp87C->tag != 10)goto _LL457;else{_tmp87D=_tmp87C->f1;}};_tmp87E=
_tmp857.f2;{struct Cyc_Absyn_FnType_struct*_tmp87F=(struct Cyc_Absyn_FnType_struct*)
_tmp87E;if(_tmp87F->tag != 10)goto _LL457;else{_tmp880=_tmp87F->f1;}};_LL456: if(
_tmp87D.tvars != 0  || _tmp880.tvars != 0){struct Cyc_List_List*_tmp890=_tmp87D.tvars;
struct Cyc_List_List*_tmp891=_tmp880.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp890)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp891))return 0;{
struct _RegionHandle*_tmp892=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*inst=0;
while(_tmp890 != 0){if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)
_tmp890->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
_tmp891))->hd)->kind))return 0;{struct _tuple15*_tmp1330;struct Cyc_Absyn_VarType_struct
_tmp132F;struct Cyc_Absyn_VarType_struct*_tmp132E;struct Cyc_List_List*_tmp132D;
inst=((_tmp132D=_region_malloc(_tmp892,sizeof(*_tmp132D)),((_tmp132D->hd=((
_tmp1330=_region_malloc(_tmp892,sizeof(*_tmp1330)),((_tmp1330->f1=(struct Cyc_Absyn_Tvar*)
_tmp891->hd,((_tmp1330->f2=(void*)((_tmp132E=_cycalloc(sizeof(*_tmp132E)),((
_tmp132E[0]=((_tmp132F.tag=2,((_tmp132F.f1=(struct Cyc_Absyn_Tvar*)_tmp890->hd,
_tmp132F)))),_tmp132E)))),_tmp1330)))))),((_tmp132D->tl=inst,_tmp132D))))));}
_tmp890=_tmp890->tl;_tmp891=_tmp891->tl;}if(inst != 0){_tmp87D.tvars=0;_tmp880.tvars=
0;{struct Cyc_Absyn_FnType_struct _tmp1336;struct Cyc_Absyn_FnType_struct*_tmp1335;
struct Cyc_Absyn_FnType_struct _tmp1333;struct Cyc_Absyn_FnType_struct*_tmp1332;
return Cyc_Tcutil_subtype(te,assume,(void*)((_tmp1332=_cycalloc(sizeof(*_tmp1332)),((
_tmp1332[0]=((_tmp1333.tag=10,((_tmp1333.f1=_tmp87D,_tmp1333)))),_tmp1332)))),(
void*)((_tmp1335=_cycalloc(sizeof(*_tmp1335)),((_tmp1335[0]=((_tmp1336.tag=10,((
_tmp1336.f1=_tmp880,_tmp1336)))),_tmp1335)))));};}};}if(!Cyc_Tcutil_subtype(te,
assume,_tmp87D.ret_typ,_tmp880.ret_typ))return 0;{struct Cyc_List_List*_tmp89B=
_tmp87D.args;struct Cyc_List_List*_tmp89C=_tmp880.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp89B)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp89C))return 0;for(0;_tmp89B != 0;(_tmp89B=_tmp89B->tl,_tmp89C=_tmp89C->tl)){
struct Cyc_Absyn_Tqual _tmp89E;void*_tmp89F;struct _tuple9 _tmp89D=*((struct _tuple9*)
_tmp89B->hd);_tmp89E=_tmp89D.f2;_tmp89F=_tmp89D.f3;{struct Cyc_Absyn_Tqual _tmp8A1;
void*_tmp8A2;struct _tuple9 _tmp8A0=*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp89C))->hd);_tmp8A1=_tmp8A0.f2;_tmp8A2=_tmp8A0.f3;if(_tmp8A1.real_const
 && !_tmp89E.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp8A2,_tmp89F))return
0;};}if(_tmp87D.c_varargs != _tmp880.c_varargs)return 0;if(_tmp87D.cyc_varargs != 0
 && _tmp880.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp8A3=*_tmp87D.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp8A4=*_tmp880.cyc_varargs;if((_tmp8A4.tq).real_const
 && !(_tmp8A3.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp8A4.type,
_tmp8A3.type))return 0;}else{if(_tmp87D.cyc_varargs != 0  || _tmp880.cyc_varargs != 
0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp87D.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp880.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp87D.rgn_po,_tmp880.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp87D.attributes,_tmp880.attributes))return 0;return 1;};_LL457:;_LL458: return 0;
_LL450:;};}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2){struct _RegionHandle*_tmp8A5=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(_tmp8A5,1,te,t1);struct Cyc_List_List*
tqs2=Cyc_Tcutil_flatten_typ(_tmp8A5,1,te,t2);for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=
tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple11 _tmp8A7;struct Cyc_Absyn_Tqual
_tmp8A8;void*_tmp8A9;struct _tuple11*_tmp8A6=(struct _tuple11*)tqs1->hd;_tmp8A7=*
_tmp8A6;_tmp8A8=_tmp8A7.f1;_tmp8A9=_tmp8A7.f2;{struct _tuple11 _tmp8AB;struct Cyc_Absyn_Tqual
_tmp8AC;void*_tmp8AD;struct _tuple11*_tmp8AA=(struct _tuple11*)tqs2->hd;_tmp8AB=*
_tmp8AA;_tmp8AC=_tmp8AB.f1;_tmp8AD=_tmp8AB.f2;if(_tmp8A8.real_const  && !_tmp8AC.real_const)
return 0;if((_tmp8AC.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(
_tmp8AD))) && Cyc_Tcutil_subtype(te,assume,_tmp8A9,_tmp8AD))continue;if(Cyc_Tcutil_unify(
_tmp8A9,_tmp8AD))continue;if(Cyc_Tcutil_isomorphic(_tmp8A9,_tmp8AD))continue;
return 0;};};}return 1;}static int Cyc_Tcutil_is_char_type(void*t){void*_tmp8AE=Cyc_Tcutil_compress(
t);enum Cyc_Absyn_Size_of _tmp8B0;_LL461: {struct Cyc_Absyn_IntType_struct*_tmp8AF=(
struct Cyc_Absyn_IntType_struct*)_tmp8AE;if(_tmp8AF->tag != 6)goto _LL463;else{
_tmp8B0=_tmp8AF->f2;if(_tmp8B0 != Cyc_Absyn_Char_sz)goto _LL463;}}_LL462: return 1;
_LL463:;_LL464: return 0;_LL460:;}enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return Cyc_Absyn_No_coercion;t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t2 == (void*)& Cyc_Absyn_VoidType_val)return Cyc_Absyn_No_coercion;{void*
_tmp8B1=t2;enum Cyc_Absyn_Size_of _tmp8B3;enum Cyc_Absyn_Size_of _tmp8B5;_LL466: {
struct Cyc_Absyn_IntType_struct*_tmp8B2=(struct Cyc_Absyn_IntType_struct*)_tmp8B1;
if(_tmp8B2->tag != 6)goto _LL468;else{_tmp8B3=_tmp8B2->f2;if(_tmp8B3 != Cyc_Absyn_Int_sz)
goto _LL468;}}_LL467: goto _LL469;_LL468: {struct Cyc_Absyn_IntType_struct*_tmp8B4=(
struct Cyc_Absyn_IntType_struct*)_tmp8B1;if(_tmp8B4->tag != 6)goto _LL46A;else{
_tmp8B5=_tmp8B4->f2;if(_tmp8B5 != Cyc_Absyn_Long_sz)goto _LL46A;}}_LL469: if((Cyc_Tcutil_typ_kind(
t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_No_coercion;goto _LL465;_LL46A:;
_LL46B: goto _LL465;_LL465:;}{void*_tmp8B6=t1;struct Cyc_Absyn_PtrInfo _tmp8B8;void*
_tmp8B9;struct Cyc_Absyn_Tqual _tmp8BA;struct Cyc_Absyn_PtrAtts _tmp8BB;void*_tmp8BC;
union Cyc_Absyn_Constraint*_tmp8BD;union Cyc_Absyn_Constraint*_tmp8BE;union Cyc_Absyn_Constraint*
_tmp8BF;struct Cyc_Absyn_ArrayInfo _tmp8C1;void*_tmp8C2;struct Cyc_Absyn_Tqual
_tmp8C3;struct Cyc_Absyn_Exp*_tmp8C4;union Cyc_Absyn_Constraint*_tmp8C5;struct Cyc_Absyn_Enumdecl*
_tmp8C7;void*_tmp8CC;_LL46D: {struct Cyc_Absyn_PointerType_struct*_tmp8B7=(struct
Cyc_Absyn_PointerType_struct*)_tmp8B6;if(_tmp8B7->tag != 5)goto _LL46F;else{
_tmp8B8=_tmp8B7->f1;_tmp8B9=_tmp8B8.elt_typ;_tmp8BA=_tmp8B8.elt_tq;_tmp8BB=
_tmp8B8.ptr_atts;_tmp8BC=_tmp8BB.rgn;_tmp8BD=_tmp8BB.nullable;_tmp8BE=_tmp8BB.bounds;
_tmp8BF=_tmp8BB.zero_term;}}_LL46E:{void*_tmp8CD=t2;struct Cyc_Absyn_PtrInfo
_tmp8CF;void*_tmp8D0;struct Cyc_Absyn_Tqual _tmp8D1;struct Cyc_Absyn_PtrAtts _tmp8D2;
void*_tmp8D3;union Cyc_Absyn_Constraint*_tmp8D4;union Cyc_Absyn_Constraint*_tmp8D5;
union Cyc_Absyn_Constraint*_tmp8D6;_LL47E: {struct Cyc_Absyn_PointerType_struct*
_tmp8CE=(struct Cyc_Absyn_PointerType_struct*)_tmp8CD;if(_tmp8CE->tag != 5)goto
_LL480;else{_tmp8CF=_tmp8CE->f1;_tmp8D0=_tmp8CF.elt_typ;_tmp8D1=_tmp8CF.elt_tq;
_tmp8D2=_tmp8CF.ptr_atts;_tmp8D3=_tmp8D2.rgn;_tmp8D4=_tmp8D2.nullable;_tmp8D5=
_tmp8D2.bounds;_tmp8D6=_tmp8D2.zero_term;}}_LL47F: {enum Cyc_Absyn_Coercion
coercion=Cyc_Absyn_Other_coercion;struct _tuple0*_tmp1339;struct Cyc_List_List*
_tmp1338;struct Cyc_List_List*_tmp8D7=(_tmp1338=_cycalloc(sizeof(*_tmp1338)),((
_tmp1338->hd=((_tmp1339=_cycalloc(sizeof(*_tmp1339)),((_tmp1339->f1=t1,((
_tmp1339->f2=t2,_tmp1339)))))),((_tmp1338->tl=0,_tmp1338)))));int _tmp8D8=_tmp8D1.real_const
 || !_tmp8BA.real_const;int _tmp8D9=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp8D5,Cyc_Absyn_bounds_one_conref) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(0,_tmp8D6);int _tmp8DA=_tmp8D8  && (((_tmp8D9  && Cyc_Tcutil_ptrsubtype(
te,_tmp8D7,_tmp8B9,_tmp8D0) || Cyc_Tcutil_unify(_tmp8B9,_tmp8D0)) || Cyc_Tcutil_isomorphic(
_tmp8B9,_tmp8D0)) || Cyc_Tcutil_unify(_tmp8D0,(void*)& Cyc_Absyn_VoidType_val));
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp8BF,_tmp8D6) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp8D6);int _tmp8DB=_tmp8DA?0:((Cyc_Tcutil_bits_only(_tmp8B9) && Cyc_Tcutil_is_char_type(
_tmp8D0)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp8D6)) && (_tmp8D1.real_const  || !_tmp8BA.real_const);int bounds_ok=((int(*)(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))
Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp8BE,_tmp8D5);if(!
bounds_ok  && !_tmp8DB){struct _tuple0 _tmp133A;struct _tuple0 _tmp8DD=(_tmp133A.f1=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8BE),((_tmp133A.f2=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8D5),_tmp133A)));
void*_tmp8DE;struct Cyc_Absyn_Exp*_tmp8E0;void*_tmp8E1;struct Cyc_Absyn_Exp*
_tmp8E3;_LL483: _tmp8DE=_tmp8DD.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp8DF=(
struct Cyc_Absyn_Upper_b_struct*)_tmp8DE;if(_tmp8DF->tag != 1)goto _LL485;else{
_tmp8E0=_tmp8DF->f1;}};_tmp8E1=_tmp8DD.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp8E2=(struct Cyc_Absyn_Upper_b_struct*)_tmp8E1;if(_tmp8E2->tag != 1)goto _LL485;
else{_tmp8E3=_tmp8E2->f1;}};_LL484: if(Cyc_Evexp_lte_const_exp(_tmp8E3,_tmp8E0))
bounds_ok=1;goto _LL482;_LL485:;_LL486: bounds_ok=1;goto _LL482;_LL482:;}if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8BD) && !((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8D4))coercion=Cyc_Absyn_NonNull_to_Null;
if(((bounds_ok  && zeroterm_ok) && (_tmp8DA  || _tmp8DB)) && (Cyc_Tcutil_unify(
_tmp8BC,_tmp8D3) || Cyc_Tcenv_region_outlives(te,_tmp8BC,_tmp8D3)))return
coercion;else{return Cyc_Absyn_Unknown_coercion;}};}_LL480:;_LL481: goto _LL47D;
_LL47D:;}return Cyc_Absyn_Unknown_coercion;_LL46F: {struct Cyc_Absyn_ArrayType_struct*
_tmp8C0=(struct Cyc_Absyn_ArrayType_struct*)_tmp8B6;if(_tmp8C0->tag != 9)goto
_LL471;else{_tmp8C1=_tmp8C0->f1;_tmp8C2=_tmp8C1.elt_type;_tmp8C3=_tmp8C1.tq;
_tmp8C4=_tmp8C1.num_elts;_tmp8C5=_tmp8C1.zero_term;}}_LL470:{void*_tmp8E6=t2;
struct Cyc_Absyn_ArrayInfo _tmp8E8;void*_tmp8E9;struct Cyc_Absyn_Tqual _tmp8EA;
struct Cyc_Absyn_Exp*_tmp8EB;union Cyc_Absyn_Constraint*_tmp8EC;_LL488: {struct Cyc_Absyn_ArrayType_struct*
_tmp8E7=(struct Cyc_Absyn_ArrayType_struct*)_tmp8E6;if(_tmp8E7->tag != 9)goto
_LL48A;else{_tmp8E8=_tmp8E7->f1;_tmp8E9=_tmp8E8.elt_type;_tmp8EA=_tmp8E8.tq;
_tmp8EB=_tmp8E8.num_elts;_tmp8EC=_tmp8E8.zero_term;}}_LL489: {int okay;okay=((
_tmp8C4 != 0  && _tmp8EB != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8C5,
_tmp8EC)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp8EB,(struct Cyc_Absyn_Exp*)
_tmp8C4);return(okay  && Cyc_Tcutil_unify(_tmp8C2,_tmp8E9)) && (!_tmp8C3.real_const
 || _tmp8EA.real_const)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;}
_LL48A:;_LL48B: return Cyc_Absyn_Unknown_coercion;_LL487:;}return Cyc_Absyn_Unknown_coercion;
_LL471: {struct Cyc_Absyn_EnumType_struct*_tmp8C6=(struct Cyc_Absyn_EnumType_struct*)
_tmp8B6;if(_tmp8C6->tag != 14)goto _LL473;else{_tmp8C7=_tmp8C6->f2;}}_LL472:{void*
_tmp8ED=t2;struct Cyc_Absyn_Enumdecl*_tmp8EF;_LL48D: {struct Cyc_Absyn_EnumType_struct*
_tmp8EE=(struct Cyc_Absyn_EnumType_struct*)_tmp8ED;if(_tmp8EE->tag != 14)goto
_LL48F;else{_tmp8EF=_tmp8EE->f2;}}_LL48E: if((_tmp8C7->fields != 0  && _tmp8EF->fields
!= 0) && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp8C7->fields))->v)>= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp8EF->fields))->v))return Cyc_Absyn_No_coercion;goto _LL48C;_LL48F:;_LL490: goto
_LL48C;_LL48C:;}goto _LL474;_LL473: {struct Cyc_Absyn_IntType_struct*_tmp8C8=(
struct Cyc_Absyn_IntType_struct*)_tmp8B6;if(_tmp8C8->tag != 6)goto _LL475;}_LL474:
goto _LL476;_LL475: {struct Cyc_Absyn_FloatType_struct*_tmp8C9=(struct Cyc_Absyn_FloatType_struct*)
_tmp8B6;if(_tmp8C9->tag != 7)goto _LL477;}_LL476: goto _LL478;_LL477: {struct Cyc_Absyn_DoubleType_struct*
_tmp8CA=(struct Cyc_Absyn_DoubleType_struct*)_tmp8B6;if(_tmp8CA->tag != 8)goto
_LL479;}_LL478: return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;
_LL479: {struct Cyc_Absyn_RgnHandleType_struct*_tmp8CB=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp8B6;if(_tmp8CB->tag != 16)goto _LL47B;else{_tmp8CC=(void*)_tmp8CB->f1;}}_LL47A:{
void*_tmp8F0=t2;void*_tmp8F2;_LL492: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp8F1=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp8F0;if(_tmp8F1->tag != 16)goto
_LL494;else{_tmp8F2=(void*)_tmp8F1->f1;}}_LL493: if(Cyc_Tcenv_region_outlives(te,
_tmp8CC,_tmp8F2))return Cyc_Absyn_No_coercion;goto _LL491;_LL494:;_LL495: goto
_LL491;_LL491:;}return Cyc_Absyn_Unknown_coercion;_LL47B:;_LL47C: return Cyc_Absyn_Unknown_coercion;
_LL46C:;};}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t,enum Cyc_Absyn_Coercion c){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp8F3=Cyc_Absyn_copy_exp(e);{
struct Cyc_Absyn_Cast_e_struct _tmp133D;struct Cyc_Absyn_Cast_e_struct*_tmp133C;e->r=(
void*)((_tmp133C=_cycalloc(sizeof(*_tmp133C)),((_tmp133C[0]=((_tmp133D.tag=15,((
_tmp133D.f1=(void*)t,((_tmp133D.f2=_tmp8F3,((_tmp133D.f3=0,((_tmp133D.f4=c,
_tmp133D)))))))))),_tmp133C))));}{struct Cyc_Core_Opt*_tmp133E;e->topt=((_tmp133E=
_cycalloc(sizeof(*_tmp133E)),((_tmp133E->v=t,_tmp133E))));};}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e){void*_tmp8F7=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL497: {struct Cyc_Absyn_IntType_struct*_tmp8F8=(
struct Cyc_Absyn_IntType_struct*)_tmp8F7;if(_tmp8F8->tag != 6)goto _LL499;}_LL498:
goto _LL49A;_LL499: {struct Cyc_Absyn_EnumType_struct*_tmp8F9=(struct Cyc_Absyn_EnumType_struct*)
_tmp8F7;if(_tmp8F9->tag != 14)goto _LL49B;}_LL49A: goto _LL49C;_LL49B: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp8FA=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp8F7;if(_tmp8FA->tag != 15)goto
_LL49D;}_LL49C: goto _LL49E;_LL49D: {struct Cyc_Absyn_TagType_struct*_tmp8FB=(
struct Cyc_Absyn_TagType_struct*)_tmp8F7;if(_tmp8FB->tag != 20)goto _LL49F;}_LL49E:
return 1;_LL49F: {struct Cyc_Absyn_Evar_struct*_tmp8FC=(struct Cyc_Absyn_Evar_struct*)
_tmp8F7;if(_tmp8FC->tag != 1)goto _LL4A1;}_LL4A0: return Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL4A1:;_LL4A2:
return 0;_LL496:;}int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp8FD=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL4A4: {struct Cyc_Absyn_FloatType_struct*_tmp8FE=(
struct Cyc_Absyn_FloatType_struct*)_tmp8FD;if(_tmp8FE->tag != 7)goto _LL4A6;}_LL4A5:
goto _LL4A7;_LL4A6: {struct Cyc_Absyn_DoubleType_struct*_tmp8FF=(struct Cyc_Absyn_DoubleType_struct*)
_tmp8FD;if(_tmp8FF->tag != 8)goto _LL4A8;}_LL4A7: return 1;_LL4A8:;_LL4A9: return 0;
_LL4A3:;};}int Cyc_Tcutil_is_function_type(void*t){void*_tmp900=Cyc_Tcutil_compress(
t);_LL4AB: {struct Cyc_Absyn_FnType_struct*_tmp901=(struct Cyc_Absyn_FnType_struct*)
_tmp900;if(_tmp901->tag != 10)goto _LL4AD;}_LL4AC: return 1;_LL4AD:;_LL4AE: return 0;
_LL4AA:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct _tuple0
_tmp133F;struct _tuple0 _tmp903=(_tmp133F.f1=t1,((_tmp133F.f2=t2,_tmp133F)));void*
_tmp904;int _tmp906;void*_tmp907;int _tmp909;void*_tmp90A;void*_tmp90C;void*
_tmp90E;void*_tmp910;void*_tmp912;enum Cyc_Absyn_Sign _tmp914;enum Cyc_Absyn_Size_of
_tmp915;void*_tmp916;enum Cyc_Absyn_Sign _tmp918;enum Cyc_Absyn_Size_of _tmp919;
void*_tmp91A;enum Cyc_Absyn_Size_of _tmp91C;void*_tmp91D;enum Cyc_Absyn_Size_of
_tmp91F;void*_tmp920;enum Cyc_Absyn_Sign _tmp922;enum Cyc_Absyn_Size_of _tmp923;
void*_tmp924;enum Cyc_Absyn_Sign _tmp926;enum Cyc_Absyn_Size_of _tmp927;void*
_tmp928;enum Cyc_Absyn_Sign _tmp92A;enum Cyc_Absyn_Size_of _tmp92B;void*_tmp92C;
enum Cyc_Absyn_Sign _tmp92E;enum Cyc_Absyn_Size_of _tmp92F;void*_tmp930;enum Cyc_Absyn_Size_of
_tmp932;void*_tmp933;enum Cyc_Absyn_Size_of _tmp935;_LL4B0: _tmp904=_tmp903.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp905=(struct Cyc_Absyn_DoubleType_struct*)
_tmp904;if(_tmp905->tag != 8)goto _LL4B2;else{_tmp906=_tmp905->f1;}};_tmp907=
_tmp903.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp908=(struct Cyc_Absyn_DoubleType_struct*)
_tmp907;if(_tmp908->tag != 8)goto _LL4B2;else{_tmp909=_tmp908->f1;}};_LL4B1: if(
_tmp906)return t1;else{return t2;}_LL4B2: _tmp90A=_tmp903.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp90B=(struct Cyc_Absyn_DoubleType_struct*)_tmp90A;if(_tmp90B->tag != 8)goto
_LL4B4;};_LL4B3: return t1;_LL4B4: _tmp90C=_tmp903.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp90D=(struct Cyc_Absyn_DoubleType_struct*)_tmp90C;if(_tmp90D->tag != 8)goto
_LL4B6;};_LL4B5: return t2;_LL4B6: _tmp90E=_tmp903.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp90F=(struct Cyc_Absyn_FloatType_struct*)_tmp90E;if(_tmp90F->tag != 7)goto
_LL4B8;};_LL4B7: goto _LL4B9;_LL4B8: _tmp910=_tmp903.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp911=(struct Cyc_Absyn_FloatType_struct*)_tmp910;if(_tmp911->tag != 7)goto
_LL4BA;};_LL4B9: return(void*)& Cyc_Absyn_FloatType_val;_LL4BA: _tmp912=_tmp903.f1;{
struct Cyc_Absyn_IntType_struct*_tmp913=(struct Cyc_Absyn_IntType_struct*)_tmp912;
if(_tmp913->tag != 6)goto _LL4BC;else{_tmp914=_tmp913->f1;if(_tmp914 != Cyc_Absyn_Unsigned)
goto _LL4BC;_tmp915=_tmp913->f2;if(_tmp915 != Cyc_Absyn_LongLong_sz)goto _LL4BC;}};
_LL4BB: goto _LL4BD;_LL4BC: _tmp916=_tmp903.f2;{struct Cyc_Absyn_IntType_struct*
_tmp917=(struct Cyc_Absyn_IntType_struct*)_tmp916;if(_tmp917->tag != 6)goto _LL4BE;
else{_tmp918=_tmp917->f1;if(_tmp918 != Cyc_Absyn_Unsigned)goto _LL4BE;_tmp919=
_tmp917->f2;if(_tmp919 != Cyc_Absyn_LongLong_sz)goto _LL4BE;}};_LL4BD: return Cyc_Absyn_ulonglong_typ;
_LL4BE: _tmp91A=_tmp903.f1;{struct Cyc_Absyn_IntType_struct*_tmp91B=(struct Cyc_Absyn_IntType_struct*)
_tmp91A;if(_tmp91B->tag != 6)goto _LL4C0;else{_tmp91C=_tmp91B->f2;if(_tmp91C != Cyc_Absyn_LongLong_sz)
goto _LL4C0;}};_LL4BF: goto _LL4C1;_LL4C0: _tmp91D=_tmp903.f2;{struct Cyc_Absyn_IntType_struct*
_tmp91E=(struct Cyc_Absyn_IntType_struct*)_tmp91D;if(_tmp91E->tag != 6)goto _LL4C2;
else{_tmp91F=_tmp91E->f2;if(_tmp91F != Cyc_Absyn_LongLong_sz)goto _LL4C2;}};_LL4C1:
return Cyc_Absyn_slonglong_typ;_LL4C2: _tmp920=_tmp903.f1;{struct Cyc_Absyn_IntType_struct*
_tmp921=(struct Cyc_Absyn_IntType_struct*)_tmp920;if(_tmp921->tag != 6)goto _LL4C4;
else{_tmp922=_tmp921->f1;if(_tmp922 != Cyc_Absyn_Unsigned)goto _LL4C4;_tmp923=
_tmp921->f2;if(_tmp923 != Cyc_Absyn_Long_sz)goto _LL4C4;}};_LL4C3: goto _LL4C5;
_LL4C4: _tmp924=_tmp903.f2;{struct Cyc_Absyn_IntType_struct*_tmp925=(struct Cyc_Absyn_IntType_struct*)
_tmp924;if(_tmp925->tag != 6)goto _LL4C6;else{_tmp926=_tmp925->f1;if(_tmp926 != Cyc_Absyn_Unsigned)
goto _LL4C6;_tmp927=_tmp925->f2;if(_tmp927 != Cyc_Absyn_Long_sz)goto _LL4C6;}};
_LL4C5: return Cyc_Absyn_ulong_typ;_LL4C6: _tmp928=_tmp903.f1;{struct Cyc_Absyn_IntType_struct*
_tmp929=(struct Cyc_Absyn_IntType_struct*)_tmp928;if(_tmp929->tag != 6)goto _LL4C8;
else{_tmp92A=_tmp929->f1;if(_tmp92A != Cyc_Absyn_Unsigned)goto _LL4C8;_tmp92B=
_tmp929->f2;if(_tmp92B != Cyc_Absyn_Int_sz)goto _LL4C8;}};_LL4C7: goto _LL4C9;_LL4C8:
_tmp92C=_tmp903.f2;{struct Cyc_Absyn_IntType_struct*_tmp92D=(struct Cyc_Absyn_IntType_struct*)
_tmp92C;if(_tmp92D->tag != 6)goto _LL4CA;else{_tmp92E=_tmp92D->f1;if(_tmp92E != Cyc_Absyn_Unsigned)
goto _LL4CA;_tmp92F=_tmp92D->f2;if(_tmp92F != Cyc_Absyn_Int_sz)goto _LL4CA;}};
_LL4C9: return Cyc_Absyn_uint_typ;_LL4CA: _tmp930=_tmp903.f1;{struct Cyc_Absyn_IntType_struct*
_tmp931=(struct Cyc_Absyn_IntType_struct*)_tmp930;if(_tmp931->tag != 6)goto _LL4CC;
else{_tmp932=_tmp931->f2;if(_tmp932 != Cyc_Absyn_Long_sz)goto _LL4CC;}};_LL4CB:
goto _LL4CD;_LL4CC: _tmp933=_tmp903.f2;{struct Cyc_Absyn_IntType_struct*_tmp934=(
struct Cyc_Absyn_IntType_struct*)_tmp933;if(_tmp934->tag != 6)goto _LL4CE;else{
_tmp935=_tmp934->f2;if(_tmp935 != Cyc_Absyn_Long_sz)goto _LL4CE;}};_LL4CD: return
Cyc_Absyn_slong_typ;_LL4CE:;_LL4CF: return Cyc_Absyn_sint_typ;_LL4AF:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp936=e->r;struct Cyc_Core_Opt*_tmp938;_LL4D1: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp937=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp936;if(_tmp937->tag != 4)goto _LL4D3;else{_tmp938=_tmp937->f2;if(_tmp938 != 0)
goto _LL4D3;}}_LL4D2:{const char*_tmp1342;void*_tmp1341;(_tmp1341=0,Cyc_Tcutil_warn(
e->loc,((_tmp1342="assignment in test",_tag_dyneither(_tmp1342,sizeof(char),19))),
_tag_dyneither(_tmp1341,sizeof(void*),0)));}goto _LL4D0;_LL4D3:;_LL4D4: goto _LL4D0;
_LL4D0:;}static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){c1=Cyc_Absyn_compress_kb(
c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple0 _tmp1343;struct _tuple0 _tmp93C=(
_tmp1343.f1=c1,((_tmp1343.f2=c2,_tmp1343)));void*_tmp93D;struct Cyc_Absyn_Kind*
_tmp93F;void*_tmp940;struct Cyc_Absyn_Kind*_tmp942;void*_tmp943;struct Cyc_Core_Opt*
_tmp945;struct Cyc_Core_Opt**_tmp946;void*_tmp947;struct Cyc_Core_Opt*_tmp949;
struct Cyc_Core_Opt**_tmp94A;void*_tmp94B;struct Cyc_Core_Opt*_tmp94D;struct Cyc_Core_Opt**
_tmp94E;struct Cyc_Absyn_Kind*_tmp94F;void*_tmp950;struct Cyc_Absyn_Kind*_tmp952;
void*_tmp953;struct Cyc_Absyn_Kind*_tmp955;void*_tmp956;struct Cyc_Core_Opt*
_tmp958;struct Cyc_Core_Opt**_tmp959;struct Cyc_Absyn_Kind*_tmp95A;void*_tmp95B;
struct Cyc_Core_Opt*_tmp95D;struct Cyc_Core_Opt**_tmp95E;struct Cyc_Absyn_Kind*
_tmp95F;void*_tmp960;struct Cyc_Core_Opt*_tmp962;struct Cyc_Core_Opt**_tmp963;
struct Cyc_Absyn_Kind*_tmp964;_LL4D6: _tmp93D=_tmp93C.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp93E=(struct Cyc_Absyn_Eq_kb_struct*)_tmp93D;if(_tmp93E->tag != 0)goto _LL4D8;
else{_tmp93F=_tmp93E->f1;}};_tmp940=_tmp93C.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp941=(struct Cyc_Absyn_Eq_kb_struct*)_tmp940;if(_tmp941->tag != 0)goto _LL4D8;
else{_tmp942=_tmp941->f1;}};_LL4D7: return _tmp93F == _tmp942;_LL4D8: _tmp943=
_tmp93C.f2;{struct Cyc_Absyn_Unknown_kb_struct*_tmp944=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp943;if(_tmp944->tag != 1)goto _LL4DA;else{_tmp945=_tmp944->f1;_tmp946=(struct
Cyc_Core_Opt**)& _tmp944->f1;}};_LL4D9:{struct Cyc_Core_Opt*_tmp1344;*_tmp946=((
_tmp1344=_cycalloc(sizeof(*_tmp1344)),((_tmp1344->v=c1,_tmp1344))));}return 1;
_LL4DA: _tmp947=_tmp93C.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmp948=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp947;if(_tmp948->tag != 1)goto _LL4DC;else{_tmp949=_tmp948->f1;_tmp94A=(struct
Cyc_Core_Opt**)& _tmp948->f1;}};_LL4DB:{struct Cyc_Core_Opt*_tmp1345;*_tmp94A=((
_tmp1345=_cycalloc(sizeof(*_tmp1345)),((_tmp1345->v=c2,_tmp1345))));}return 1;
_LL4DC: _tmp94B=_tmp93C.f1;{struct Cyc_Absyn_Less_kb_struct*_tmp94C=(struct Cyc_Absyn_Less_kb_struct*)
_tmp94B;if(_tmp94C->tag != 2)goto _LL4DE;else{_tmp94D=_tmp94C->f1;_tmp94E=(struct
Cyc_Core_Opt**)& _tmp94C->f1;_tmp94F=_tmp94C->f2;}};_tmp950=_tmp93C.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp951=(struct Cyc_Absyn_Eq_kb_struct*)_tmp950;if(_tmp951->tag != 0)goto _LL4DE;
else{_tmp952=_tmp951->f1;}};_LL4DD: if(Cyc_Tcutil_kind_leq(_tmp952,_tmp94F)){{
struct Cyc_Core_Opt*_tmp1346;*_tmp94E=((_tmp1346=_cycalloc(sizeof(*_tmp1346)),((
_tmp1346->v=c2,_tmp1346))));}return 1;}else{return 0;}_LL4DE: _tmp953=_tmp93C.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp954=(struct Cyc_Absyn_Eq_kb_struct*)_tmp953;if(
_tmp954->tag != 0)goto _LL4E0;else{_tmp955=_tmp954->f1;}};_tmp956=_tmp93C.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp957=(struct Cyc_Absyn_Less_kb_struct*)_tmp956;
if(_tmp957->tag != 2)goto _LL4E0;else{_tmp958=_tmp957->f1;_tmp959=(struct Cyc_Core_Opt**)&
_tmp957->f1;_tmp95A=_tmp957->f2;}};_LL4DF: if(Cyc_Tcutil_kind_leq(_tmp955,_tmp95A)){{
struct Cyc_Core_Opt*_tmp1347;*_tmp959=((_tmp1347=_cycalloc(sizeof(*_tmp1347)),((
_tmp1347->v=c1,_tmp1347))));}return 1;}else{return 0;}_LL4E0: _tmp95B=_tmp93C.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp95C=(struct Cyc_Absyn_Less_kb_struct*)_tmp95B;
if(_tmp95C->tag != 2)goto _LL4D5;else{_tmp95D=_tmp95C->f1;_tmp95E=(struct Cyc_Core_Opt**)&
_tmp95C->f1;_tmp95F=_tmp95C->f2;}};_tmp960=_tmp93C.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp961=(struct Cyc_Absyn_Less_kb_struct*)_tmp960;if(_tmp961->tag != 2)goto _LL4D5;
else{_tmp962=_tmp961->f1;_tmp963=(struct Cyc_Core_Opt**)& _tmp961->f1;_tmp964=
_tmp961->f2;}};_LL4E1: if(Cyc_Tcutil_kind_leq(_tmp95F,_tmp964)){{struct Cyc_Core_Opt*
_tmp1348;*_tmp963=((_tmp1348=_cycalloc(sizeof(*_tmp1348)),((_tmp1348->v=c1,
_tmp1348))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp964,_tmp95F)){{struct Cyc_Core_Opt*
_tmp1349;*_tmp95E=((_tmp1349=_cycalloc(sizeof(*_tmp1349)),((_tmp1349->v=c2,
_tmp1349))));}return 1;}else{return 0;}}_LL4D5:;};}static int Cyc_Tcutil_tvar_id_counter=
0;int Cyc_Tcutil_new_tvar_id(){return Cyc_Tcutil_tvar_id_counter ++;}static int Cyc_Tcutil_tvar_counter=
0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter
++;const char*_tmp134D;void*_tmp134C[1];struct Cyc_Int_pa_struct _tmp134B;struct
_dyneither_ptr s=(struct _dyneither_ptr)((_tmp134B.tag=1,((_tmp134B.f1=(
unsigned long)i,((_tmp134C[0]=& _tmp134B,Cyc_aprintf(((_tmp134D="#%d",
_tag_dyneither(_tmp134D,sizeof(char),4))),_tag_dyneither(_tmp134C,sizeof(void*),
1))))))));struct _dyneither_ptr*_tmp1350;struct Cyc_Absyn_Tvar*_tmp134F;return(
_tmp134F=_cycalloc(sizeof(*_tmp134F)),((_tmp134F->name=((_tmp1350=_cycalloc(
sizeof(struct _dyneither_ptr)* 1),((_tmp1350[0]=s,_tmp1350)))),((_tmp134F->identity=
- 1,((_tmp134F->kind=k,_tmp134F)))))));}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t){struct _dyneither_ptr _tmp970=*t->name;return*((const char*)
_check_dyneither_subscript(_tmp970,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){{const char*_tmp1354;void*_tmp1353[1];struct Cyc_String_pa_struct
_tmp1352;(_tmp1352.tag=0,((_tmp1352.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*t->name),((_tmp1353[0]=& _tmp1352,Cyc_printf(((_tmp1354="%s",
_tag_dyneither(_tmp1354,sizeof(char),3))),_tag_dyneither(_tmp1353,sizeof(void*),
1)))))));}if(!Cyc_Tcutil_is_temp_tvar(t))return;{const char*_tmp1355;struct
_dyneither_ptr _tmp974=Cyc_strconcat(((_tmp1355="`",_tag_dyneither(_tmp1355,
sizeof(char),2))),(struct _dyneither_ptr)*t->name);{char _tmp1358;char _tmp1357;
struct _dyneither_ptr _tmp1356;(_tmp1356=_dyneither_ptr_plus(_tmp974,sizeof(char),
1),((_tmp1357=*((char*)_check_dyneither_subscript(_tmp1356,sizeof(char),0)),((
_tmp1358='t',((_get_dyneither_size(_tmp1356,sizeof(char))== 1  && (_tmp1357 == '\000'
 && _tmp1358 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp1356.curr)=_tmp1358)))))));}{
struct _dyneither_ptr*_tmp1359;t->name=((_tmp1359=_cycalloc(sizeof(struct
_dyneither_ptr)* 1),((_tmp1359[0]=(struct _dyneither_ptr)_tmp974,_tmp1359))));};};}
struct _tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static
struct _tuple9*Cyc_Tcutil_fndecl2typ_f(struct _tuple25*x){struct Cyc_Core_Opt*
_tmp135C;struct _tuple9*_tmp135B;return(_tmp135B=_cycalloc(sizeof(*_tmp135B)),((
_tmp135B->f1=(struct Cyc_Core_Opt*)((_tmp135C=_cycalloc(sizeof(*_tmp135C)),((
_tmp135C->v=(*x).f1,_tmp135C)))),((_tmp135B->f2=(*x).f2,((_tmp135B->f3=(*x).f3,
_tmp135B)))))));}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp97C=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){struct
Cyc_List_List*_tmp135D;_tmp97C=((_tmp135D=_cycalloc(sizeof(*_tmp135D)),((
_tmp135D->hd=(void*)atts->hd,((_tmp135D->tl=_tmp97C,_tmp135D))))));}}}{struct Cyc_Absyn_FnType_struct
_tmp1363;struct Cyc_Absyn_FnInfo _tmp1362;struct Cyc_Absyn_FnType_struct*_tmp1361;
return(void*)((_tmp1361=_cycalloc(sizeof(*_tmp1361)),((_tmp1361[0]=((_tmp1363.tag=
10,((_tmp1363.f1=((_tmp1362.tvars=fd->tvs,((_tmp1362.effect=fd->effect,((
_tmp1362.ret_tqual=fd->ret_tqual,((_tmp1362.ret_typ=fd->ret_type,((_tmp1362.args=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple25*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmp1362.c_varargs=fd->c_varargs,((
_tmp1362.cyc_varargs=fd->cyc_varargs,((_tmp1362.rgn_po=fd->rgn_po,((_tmp1362.attributes=
_tmp97C,_tmp1362)))))))))))))))))),_tmp1363)))),_tmp1361))));};}return(void*)((
struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}struct _tuple26{void*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple26*
t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple11*t){return(*t).f2;}
static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*t){struct _tuple11*
_tmp1364;return(_tmp1364=_cycalloc(sizeof(*_tmp1364)),((_tmp1364->f1=(*pr).f1,((
_tmp1364->f2=t,_tmp1364)))));}struct _tuple27{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;};struct _tuple28{struct _tuple27*f1;void*f2;};static struct _tuple28*Cyc_Tcutil_substitute_f1(
struct _RegionHandle*rgn,struct _tuple9*y){struct _tuple27*_tmp1367;struct _tuple28*
_tmp1366;return(_tmp1366=_region_malloc(rgn,sizeof(*_tmp1366)),((_tmp1366->f1=((
_tmp1367=_region_malloc(rgn,sizeof(*_tmp1367)),((_tmp1367->f1=(*y).f1,((_tmp1367->f2=(*
y).f2,_tmp1367)))))),((_tmp1366->f2=(*y).f3,_tmp1366)))));}static struct _tuple9*
Cyc_Tcutil_substitute_f2(struct _tuple28*w){struct _tuple27*_tmp985;void*_tmp986;
struct _tuple28 _tmp984=*w;_tmp985=_tmp984.f1;_tmp986=_tmp984.f2;{struct Cyc_Core_Opt*
_tmp988;struct Cyc_Absyn_Tqual _tmp989;struct _tuple27 _tmp987=*_tmp985;_tmp988=
_tmp987.f1;_tmp989=_tmp987.f2;{struct _tuple9*_tmp1368;return(_tmp1368=_cycalloc(
sizeof(*_tmp1368)),((_tmp1368->f1=_tmp988,((_tmp1368->f2=_tmp989,((_tmp1368->f3=
_tmp986,_tmp1368)))))));};};}static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*
f){return f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){struct Cyc_Absyn_Aggrfield*_tmp1369;return(
_tmp1369=_cycalloc(sizeof(*_tmp1369)),((_tmp1369->name=f->name,((_tmp1369->tq=f->tq,((
_tmp1369->type=t,((_tmp1369->width=f->width,((_tmp1369->attributes=f->attributes,
_tmp1369)))))))))));}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);static struct
Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){struct Cyc_Absyn_Exp*
_tmp136A;return(_tmp136A=_cycalloc(sizeof(*_tmp136A)),((_tmp136A->topt=old->topt,((
_tmp136A->r=r,((_tmp136A->loc=old->loc,((_tmp136A->annot=old->annot,_tmp136A)))))))));}
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*
inst,struct Cyc_Absyn_Exp*e){void*_tmp98D=e->r;enum Cyc_Absyn_Primop _tmp993;
struct Cyc_List_List*_tmp994;struct Cyc_Absyn_Exp*_tmp996;struct Cyc_Absyn_Exp*
_tmp997;struct Cyc_Absyn_Exp*_tmp998;struct Cyc_Absyn_Exp*_tmp99A;struct Cyc_Absyn_Exp*
_tmp99B;struct Cyc_Absyn_Exp*_tmp99D;struct Cyc_Absyn_Exp*_tmp99E;struct Cyc_Absyn_Exp*
_tmp9A0;struct Cyc_Absyn_Exp*_tmp9A1;void*_tmp9A3;struct Cyc_Absyn_Exp*_tmp9A4;int
_tmp9A5;enum Cyc_Absyn_Coercion _tmp9A6;void*_tmp9A8;struct Cyc_Absyn_Exp*_tmp9AA;
void*_tmp9AC;void*_tmp9AD;void*_tmp9AF;_LL4E3: {struct Cyc_Absyn_Const_e_struct*
_tmp98E=(struct Cyc_Absyn_Const_e_struct*)_tmp98D;if(_tmp98E->tag != 0)goto _LL4E5;}
_LL4E4: goto _LL4E6;_LL4E5: {struct Cyc_Absyn_Enum_e_struct*_tmp98F=(struct Cyc_Absyn_Enum_e_struct*)
_tmp98D;if(_tmp98F->tag != 32)goto _LL4E7;}_LL4E6: goto _LL4E8;_LL4E7: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp990=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp98D;if(_tmp990->tag != 33)goto
_LL4E9;}_LL4E8: goto _LL4EA;_LL4E9: {struct Cyc_Absyn_Var_e_struct*_tmp991=(struct
Cyc_Absyn_Var_e_struct*)_tmp98D;if(_tmp991->tag != 1)goto _LL4EB;}_LL4EA: return e;
_LL4EB: {struct Cyc_Absyn_Primop_e_struct*_tmp992=(struct Cyc_Absyn_Primop_e_struct*)
_tmp98D;if(_tmp992->tag != 3)goto _LL4ED;else{_tmp993=_tmp992->f1;_tmp994=_tmp992->f2;}}
_LL4EC: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp994)== 1){struct
Cyc_Absyn_Exp*_tmp9B0=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp994))->hd;struct Cyc_Absyn_Exp*_tmp9B1=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B0);
if(_tmp9B1 == _tmp9B0)return e;{struct Cyc_Absyn_Primop_e_struct _tmp1370;struct Cyc_Absyn_Exp*
_tmp136F[1];struct Cyc_Absyn_Primop_e_struct*_tmp136E;return Cyc_Tcutil_copye(e,(
void*)((_tmp136E=_cycalloc(sizeof(*_tmp136E)),((_tmp136E[0]=((_tmp1370.tag=3,((
_tmp1370.f1=_tmp993,((_tmp1370.f2=((_tmp136F[0]=_tmp9B1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp136F,sizeof(struct Cyc_Absyn_Exp*),
1)))),_tmp1370)))))),_tmp136E)))));};}else{if(((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp994)== 2){struct Cyc_Absyn_Exp*_tmp9B5=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp994))->hd;struct Cyc_Absyn_Exp*_tmp9B6=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp994->tl))->hd;struct
Cyc_Absyn_Exp*_tmp9B7=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B5);struct Cyc_Absyn_Exp*
_tmp9B8=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B6);if(_tmp9B7 == _tmp9B5  && _tmp9B8 == 
_tmp9B6)return e;{struct Cyc_Absyn_Primop_e_struct _tmp1376;struct Cyc_Absyn_Exp*
_tmp1375[2];struct Cyc_Absyn_Primop_e_struct*_tmp1374;return Cyc_Tcutil_copye(e,(
void*)((_tmp1374=_cycalloc(sizeof(*_tmp1374)),((_tmp1374[0]=((_tmp1376.tag=3,((
_tmp1376.f1=_tmp993,((_tmp1376.f2=((_tmp1375[1]=_tmp9B8,((_tmp1375[0]=_tmp9B7,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp1375,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp1376)))))),_tmp1374)))));};}
else{const char*_tmp1379;void*_tmp1378;return(_tmp1378=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1379="primop does not have 1 or 2 args!",
_tag_dyneither(_tmp1379,sizeof(char),34))),_tag_dyneither(_tmp1378,sizeof(void*),
0)));}}_LL4ED: {struct Cyc_Absyn_Conditional_e_struct*_tmp995=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp98D;if(_tmp995->tag != 6)goto _LL4EF;else{_tmp996=_tmp995->f1;_tmp997=_tmp995->f2;
_tmp998=_tmp995->f3;}}_LL4EE: {struct Cyc_Absyn_Exp*_tmp9BE=Cyc_Tcutil_rsubsexp(r,
inst,_tmp996);struct Cyc_Absyn_Exp*_tmp9BF=Cyc_Tcutil_rsubsexp(r,inst,_tmp997);
struct Cyc_Absyn_Exp*_tmp9C0=Cyc_Tcutil_rsubsexp(r,inst,_tmp998);if((_tmp9BE == 
_tmp996  && _tmp9BF == _tmp997) && _tmp9C0 == _tmp998)return e;{struct Cyc_Absyn_Conditional_e_struct
_tmp137C;struct Cyc_Absyn_Conditional_e_struct*_tmp137B;return Cyc_Tcutil_copye(e,(
void*)((_tmp137B=_cycalloc(sizeof(*_tmp137B)),((_tmp137B[0]=((_tmp137C.tag=6,((
_tmp137C.f1=_tmp9BE,((_tmp137C.f2=_tmp9BF,((_tmp137C.f3=_tmp9C0,_tmp137C)))))))),
_tmp137B)))));};}_LL4EF: {struct Cyc_Absyn_And_e_struct*_tmp999=(struct Cyc_Absyn_And_e_struct*)
_tmp98D;if(_tmp999->tag != 7)goto _LL4F1;else{_tmp99A=_tmp999->f1;_tmp99B=_tmp999->f2;}}
_LL4F0: {struct Cyc_Absyn_Exp*_tmp9C3=Cyc_Tcutil_rsubsexp(r,inst,_tmp99A);struct
Cyc_Absyn_Exp*_tmp9C4=Cyc_Tcutil_rsubsexp(r,inst,_tmp99B);if(_tmp9C3 == _tmp99A
 && _tmp9C4 == _tmp99B)return e;{struct Cyc_Absyn_And_e_struct _tmp137F;struct Cyc_Absyn_And_e_struct*
_tmp137E;return Cyc_Tcutil_copye(e,(void*)((_tmp137E=_cycalloc(sizeof(*_tmp137E)),((
_tmp137E[0]=((_tmp137F.tag=7,((_tmp137F.f1=_tmp9C3,((_tmp137F.f2=_tmp9C4,
_tmp137F)))))),_tmp137E)))));};}_LL4F1: {struct Cyc_Absyn_Or_e_struct*_tmp99C=(
struct Cyc_Absyn_Or_e_struct*)_tmp98D;if(_tmp99C->tag != 8)goto _LL4F3;else{_tmp99D=
_tmp99C->f1;_tmp99E=_tmp99C->f2;}}_LL4F2: {struct Cyc_Absyn_Exp*_tmp9C7=Cyc_Tcutil_rsubsexp(
r,inst,_tmp99D);struct Cyc_Absyn_Exp*_tmp9C8=Cyc_Tcutil_rsubsexp(r,inst,_tmp99E);
if(_tmp9C7 == _tmp99D  && _tmp9C8 == _tmp99E)return e;{struct Cyc_Absyn_Or_e_struct
_tmp1382;struct Cyc_Absyn_Or_e_struct*_tmp1381;return Cyc_Tcutil_copye(e,(void*)((
_tmp1381=_cycalloc(sizeof(*_tmp1381)),((_tmp1381[0]=((_tmp1382.tag=8,((_tmp1382.f1=
_tmp9C7,((_tmp1382.f2=_tmp9C8,_tmp1382)))))),_tmp1381)))));};}_LL4F3: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp99F=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp98D;if(_tmp99F->tag != 9)goto _LL4F5;
else{_tmp9A0=_tmp99F->f1;_tmp9A1=_tmp99F->f2;}}_LL4F4: {struct Cyc_Absyn_Exp*
_tmp9CB=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A0);struct Cyc_Absyn_Exp*_tmp9CC=Cyc_Tcutil_rsubsexp(
r,inst,_tmp9A1);if(_tmp9CB == _tmp9A0  && _tmp9CC == _tmp9A1)return e;{struct Cyc_Absyn_SeqExp_e_struct
_tmp1385;struct Cyc_Absyn_SeqExp_e_struct*_tmp1384;return Cyc_Tcutil_copye(e,(void*)((
_tmp1384=_cycalloc(sizeof(*_tmp1384)),((_tmp1384[0]=((_tmp1385.tag=9,((_tmp1385.f1=
_tmp9CB,((_tmp1385.f2=_tmp9CC,_tmp1385)))))),_tmp1384)))));};}_LL4F5: {struct Cyc_Absyn_Cast_e_struct*
_tmp9A2=(struct Cyc_Absyn_Cast_e_struct*)_tmp98D;if(_tmp9A2->tag != 15)goto _LL4F7;
else{_tmp9A3=(void*)_tmp9A2->f1;_tmp9A4=_tmp9A2->f2;_tmp9A5=_tmp9A2->f3;_tmp9A6=
_tmp9A2->f4;}}_LL4F6: {struct Cyc_Absyn_Exp*_tmp9CF=Cyc_Tcutil_rsubsexp(r,inst,
_tmp9A4);void*_tmp9D0=Cyc_Tcutil_rsubstitute(r,inst,_tmp9A3);if(_tmp9CF == 
_tmp9A4  && _tmp9D0 == _tmp9A3)return e;{struct Cyc_Absyn_Cast_e_struct _tmp1388;
struct Cyc_Absyn_Cast_e_struct*_tmp1387;return Cyc_Tcutil_copye(e,(void*)((
_tmp1387=_cycalloc(sizeof(*_tmp1387)),((_tmp1387[0]=((_tmp1388.tag=15,((_tmp1388.f1=(
void*)_tmp9D0,((_tmp1388.f2=_tmp9CF,((_tmp1388.f3=_tmp9A5,((_tmp1388.f4=_tmp9A6,
_tmp1388)))))))))),_tmp1387)))));};}_LL4F7: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp9A7=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp98D;if(_tmp9A7->tag != 18)goto
_LL4F9;else{_tmp9A8=(void*)_tmp9A7->f1;}}_LL4F8: {void*_tmp9D3=Cyc_Tcutil_rsubstitute(
r,inst,_tmp9A8);if(_tmp9D3 == _tmp9A8)return e;{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp138B;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp138A;return Cyc_Tcutil_copye(e,(
void*)((_tmp138A=_cycalloc(sizeof(*_tmp138A)),((_tmp138A[0]=((_tmp138B.tag=18,((
_tmp138B.f1=(void*)_tmp9D3,_tmp138B)))),_tmp138A)))));};}_LL4F9: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp9A9=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp98D;if(_tmp9A9->tag != 19)goto
_LL4FB;else{_tmp9AA=_tmp9A9->f1;}}_LL4FA: {struct Cyc_Absyn_Exp*_tmp9D6=Cyc_Tcutil_rsubsexp(
r,inst,_tmp9AA);if(_tmp9D6 == _tmp9AA)return e;{struct Cyc_Absyn_Sizeofexp_e_struct
_tmp138E;struct Cyc_Absyn_Sizeofexp_e_struct*_tmp138D;return Cyc_Tcutil_copye(e,(
void*)((_tmp138D=_cycalloc(sizeof(*_tmp138D)),((_tmp138D[0]=((_tmp138E.tag=19,((
_tmp138E.f1=_tmp9D6,_tmp138E)))),_tmp138D)))));};}_LL4FB: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp9AB=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp98D;if(_tmp9AB->tag != 20)goto
_LL4FD;else{_tmp9AC=(void*)_tmp9AB->f1;_tmp9AD=(void*)_tmp9AB->f2;}}_LL4FC: {
void*_tmp9D9=Cyc_Tcutil_rsubstitute(r,inst,_tmp9AC);if(_tmp9D9 == _tmp9AC)return e;{
struct Cyc_Absyn_Offsetof_e_struct _tmp1391;struct Cyc_Absyn_Offsetof_e_struct*
_tmp1390;return Cyc_Tcutil_copye(e,(void*)((_tmp1390=_cycalloc(sizeof(*_tmp1390)),((
_tmp1390[0]=((_tmp1391.tag=20,((_tmp1391.f1=(void*)_tmp9D9,((_tmp1391.f2=(void*)
_tmp9AD,_tmp1391)))))),_tmp1390)))));};}_LL4FD: {struct Cyc_Absyn_Valueof_e_struct*
_tmp9AE=(struct Cyc_Absyn_Valueof_e_struct*)_tmp98D;if(_tmp9AE->tag != 39)goto
_LL4FF;else{_tmp9AF=(void*)_tmp9AE->f1;}}_LL4FE: {void*_tmp9DC=Cyc_Tcutil_rsubstitute(
r,inst,_tmp9AF);if(_tmp9DC == _tmp9AF)return e;{struct Cyc_Absyn_Valueof_e_struct
_tmp1394;struct Cyc_Absyn_Valueof_e_struct*_tmp1393;return Cyc_Tcutil_copye(e,(
void*)((_tmp1393=_cycalloc(sizeof(*_tmp1393)),((_tmp1393[0]=((_tmp1394.tag=39,((
_tmp1394.f1=(void*)_tmp9DC,_tmp1394)))),_tmp1393)))));};}_LL4FF:;_LL500: {const
char*_tmp1397;void*_tmp1396;return(_tmp1396=0,((struct Cyc_Absyn_Exp*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1397="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmp1397,sizeof(char),46))),_tag_dyneither(_tmp1396,sizeof(void*),
0)));}_LL4E2:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp9E1=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp9E3;
struct Cyc_Absyn_AggrInfo _tmp9E5;union Cyc_Absyn_AggrInfoU _tmp9E6;struct Cyc_List_List*
_tmp9E7;struct Cyc_Absyn_DatatypeInfo _tmp9E9;union Cyc_Absyn_DatatypeInfoU _tmp9EA;
struct Cyc_List_List*_tmp9EB;struct Cyc_Absyn_DatatypeFieldInfo _tmp9ED;union Cyc_Absyn_DatatypeFieldInfoU
_tmp9EE;struct Cyc_List_List*_tmp9EF;struct _tuple2*_tmp9F1;struct Cyc_List_List*
_tmp9F2;struct Cyc_Absyn_Typedefdecl*_tmp9F3;void**_tmp9F4;struct Cyc_Absyn_ArrayInfo
_tmp9F6;void*_tmp9F7;struct Cyc_Absyn_Tqual _tmp9F8;struct Cyc_Absyn_Exp*_tmp9F9;
union Cyc_Absyn_Constraint*_tmp9FA;struct Cyc_Position_Segment*_tmp9FB;struct Cyc_Absyn_PtrInfo
_tmp9FD;void*_tmp9FE;struct Cyc_Absyn_Tqual _tmp9FF;struct Cyc_Absyn_PtrAtts _tmpA00;
void*_tmpA01;union Cyc_Absyn_Constraint*_tmpA02;union Cyc_Absyn_Constraint*_tmpA03;
union Cyc_Absyn_Constraint*_tmpA04;struct Cyc_Absyn_FnInfo _tmpA06;struct Cyc_List_List*
_tmpA07;struct Cyc_Core_Opt*_tmpA08;struct Cyc_Absyn_Tqual _tmpA09;void*_tmpA0A;
struct Cyc_List_List*_tmpA0B;int _tmpA0C;struct Cyc_Absyn_VarargInfo*_tmpA0D;struct
Cyc_List_List*_tmpA0E;struct Cyc_List_List*_tmpA0F;struct Cyc_List_List*_tmpA11;
enum Cyc_Absyn_AggrKind _tmpA13;struct Cyc_List_List*_tmpA14;struct Cyc_Core_Opt*
_tmpA16;void*_tmpA18;void*_tmpA1A;void*_tmpA1B;void*_tmpA1D;struct Cyc_Absyn_Exp*
_tmpA1F;void*_tmpA29;void*_tmpA2B;struct Cyc_List_List*_tmpA2D;_LL502: {struct Cyc_Absyn_VarType_struct*
_tmp9E2=(struct Cyc_Absyn_VarType_struct*)_tmp9E1;if(_tmp9E2->tag != 2)goto _LL504;
else{_tmp9E3=_tmp9E2->f1;}}_LL503: {struct _handler_cons _tmpA2F;_push_handler(&
_tmpA2F);{int _tmpA31=0;if(setjmp(_tmpA2F.handler))_tmpA31=1;if(!_tmpA31){{void*
_tmpA32=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,
inst,_tmp9E3);_npop_handler(0);return _tmpA32;};_pop_handler();}else{void*_tmpA30=(
void*)_exn_thrown;void*_tmpA34=_tmpA30;_LL539: {struct Cyc_Core_Not_found_struct*
_tmpA35=(struct Cyc_Core_Not_found_struct*)_tmpA34;if(_tmpA35->tag != Cyc_Core_Not_found)
goto _LL53B;}_LL53A: return t;_LL53B:;_LL53C:(void)_throw(_tmpA34);_LL538:;}};}
_LL504: {struct Cyc_Absyn_AggrType_struct*_tmp9E4=(struct Cyc_Absyn_AggrType_struct*)
_tmp9E1;if(_tmp9E4->tag != 12)goto _LL506;else{_tmp9E5=_tmp9E4->f1;_tmp9E6=_tmp9E5.aggr_info;
_tmp9E7=_tmp9E5.targs;}}_LL505: {struct Cyc_List_List*_tmpA36=Cyc_Tcutil_substs(
rgn,inst,_tmp9E7);struct Cyc_Absyn_AggrType_struct _tmp139D;struct Cyc_Absyn_AggrInfo
_tmp139C;struct Cyc_Absyn_AggrType_struct*_tmp139B;return _tmpA36 == _tmp9E7?t:(
void*)((_tmp139B=_cycalloc(sizeof(*_tmp139B)),((_tmp139B[0]=((_tmp139D.tag=12,((
_tmp139D.f1=((_tmp139C.aggr_info=_tmp9E6,((_tmp139C.targs=_tmpA36,_tmp139C)))),
_tmp139D)))),_tmp139B))));}_LL506: {struct Cyc_Absyn_DatatypeType_struct*_tmp9E8=(
struct Cyc_Absyn_DatatypeType_struct*)_tmp9E1;if(_tmp9E8->tag != 3)goto _LL508;
else{_tmp9E9=_tmp9E8->f1;_tmp9EA=_tmp9E9.datatype_info;_tmp9EB=_tmp9E9.targs;}}
_LL507: {struct Cyc_List_List*_tmpA3A=Cyc_Tcutil_substs(rgn,inst,_tmp9EB);struct
Cyc_Absyn_DatatypeType_struct _tmp13A3;struct Cyc_Absyn_DatatypeInfo _tmp13A2;
struct Cyc_Absyn_DatatypeType_struct*_tmp13A1;return _tmpA3A == _tmp9EB?t:(void*)((
_tmp13A1=_cycalloc(sizeof(*_tmp13A1)),((_tmp13A1[0]=((_tmp13A3.tag=3,((_tmp13A3.f1=((
_tmp13A2.datatype_info=_tmp9EA,((_tmp13A2.targs=_tmpA3A,_tmp13A2)))),_tmp13A3)))),
_tmp13A1))));}_LL508: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp9EC=(struct
Cyc_Absyn_DatatypeFieldType_struct*)_tmp9E1;if(_tmp9EC->tag != 4)goto _LL50A;else{
_tmp9ED=_tmp9EC->f1;_tmp9EE=_tmp9ED.field_info;_tmp9EF=_tmp9ED.targs;}}_LL509: {
struct Cyc_List_List*_tmpA3E=Cyc_Tcutil_substs(rgn,inst,_tmp9EF);struct Cyc_Absyn_DatatypeFieldType_struct
_tmp13A9;struct Cyc_Absyn_DatatypeFieldInfo _tmp13A8;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp13A7;return _tmpA3E == _tmp9EF?t:(void*)((_tmp13A7=_cycalloc(sizeof(*_tmp13A7)),((
_tmp13A7[0]=((_tmp13A9.tag=4,((_tmp13A9.f1=((_tmp13A8.field_info=_tmp9EE,((
_tmp13A8.targs=_tmpA3E,_tmp13A8)))),_tmp13A9)))),_tmp13A7))));}_LL50A: {struct
Cyc_Absyn_TypedefType_struct*_tmp9F0=(struct Cyc_Absyn_TypedefType_struct*)
_tmp9E1;if(_tmp9F0->tag != 18)goto _LL50C;else{_tmp9F1=_tmp9F0->f1;_tmp9F2=_tmp9F0->f2;
_tmp9F3=_tmp9F0->f3;_tmp9F4=_tmp9F0->f4;}}_LL50B: {struct Cyc_List_List*_tmpA42=
Cyc_Tcutil_substs(rgn,inst,_tmp9F2);struct Cyc_Absyn_TypedefType_struct _tmp13AC;
struct Cyc_Absyn_TypedefType_struct*_tmp13AB;return _tmpA42 == _tmp9F2?t:(void*)((
_tmp13AB=_cycalloc(sizeof(*_tmp13AB)),((_tmp13AB[0]=((_tmp13AC.tag=18,((_tmp13AC.f1=
_tmp9F1,((_tmp13AC.f2=_tmpA42,((_tmp13AC.f3=_tmp9F3,((_tmp13AC.f4=_tmp9F4,
_tmp13AC)))))))))),_tmp13AB))));}_LL50C: {struct Cyc_Absyn_ArrayType_struct*
_tmp9F5=(struct Cyc_Absyn_ArrayType_struct*)_tmp9E1;if(_tmp9F5->tag != 9)goto
_LL50E;else{_tmp9F6=_tmp9F5->f1;_tmp9F7=_tmp9F6.elt_type;_tmp9F8=_tmp9F6.tq;
_tmp9F9=_tmp9F6.num_elts;_tmp9FA=_tmp9F6.zero_term;_tmp9FB=_tmp9F6.zt_loc;}}
_LL50D: {void*_tmpA45=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp9F7);struct Cyc_Absyn_ArrayType_struct
_tmp13B2;struct Cyc_Absyn_ArrayInfo _tmp13B1;struct Cyc_Absyn_ArrayType_struct*
_tmp13B0;return _tmpA45 == _tmp9F7?t:(void*)((_tmp13B0=_cycalloc(sizeof(*_tmp13B0)),((
_tmp13B0[0]=((_tmp13B2.tag=9,((_tmp13B2.f1=((_tmp13B1.elt_type=_tmpA45,((
_tmp13B1.tq=_tmp9F8,((_tmp13B1.num_elts=_tmp9F9,((_tmp13B1.zero_term=_tmp9FA,((
_tmp13B1.zt_loc=_tmp9FB,_tmp13B1)))))))))),_tmp13B2)))),_tmp13B0))));}_LL50E: {
struct Cyc_Absyn_PointerType_struct*_tmp9FC=(struct Cyc_Absyn_PointerType_struct*)
_tmp9E1;if(_tmp9FC->tag != 5)goto _LL510;else{_tmp9FD=_tmp9FC->f1;_tmp9FE=_tmp9FD.elt_typ;
_tmp9FF=_tmp9FD.elt_tq;_tmpA00=_tmp9FD.ptr_atts;_tmpA01=_tmpA00.rgn;_tmpA02=
_tmpA00.nullable;_tmpA03=_tmpA00.bounds;_tmpA04=_tmpA00.zero_term;}}_LL50F: {
void*_tmpA49=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp9FE);void*_tmpA4A=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA01);union Cyc_Absyn_Constraint*_tmpA4B=_tmpA03;{void*_tmpA4C=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpA03);struct Cyc_Absyn_Exp*_tmpA4E;_LL53E: {struct Cyc_Absyn_Upper_b_struct*
_tmpA4D=(struct Cyc_Absyn_Upper_b_struct*)_tmpA4C;if(_tmpA4D->tag != 1)goto _LL540;
else{_tmpA4E=_tmpA4D->f1;}}_LL53F: {struct Cyc_Absyn_Exp*_tmpA4F=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmpA4E);if(_tmpA4F != _tmpA4E){struct Cyc_Absyn_Upper_b_struct _tmp13B5;
struct Cyc_Absyn_Upper_b_struct*_tmp13B4;_tmpA4B=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp13B4=_cycalloc(sizeof(*_tmp13B4)),((
_tmp13B4[0]=((_tmp13B5.tag=1,((_tmp13B5.f1=_tmpA4F,_tmp13B5)))),_tmp13B4)))));}
goto _LL53D;}_LL540:;_LL541: goto _LL53D;_LL53D:;}if((_tmpA49 == _tmp9FE  && _tmpA4A
== _tmpA01) && _tmpA4B == _tmpA03)return t;{struct Cyc_Absyn_PointerType_struct
_tmp13BF;struct Cyc_Absyn_PtrAtts _tmp13BE;struct Cyc_Absyn_PtrInfo _tmp13BD;struct
Cyc_Absyn_PointerType_struct*_tmp13BC;return(void*)((_tmp13BC=_cycalloc(sizeof(*
_tmp13BC)),((_tmp13BC[0]=((_tmp13BF.tag=5,((_tmp13BF.f1=((_tmp13BD.elt_typ=
_tmpA49,((_tmp13BD.elt_tq=_tmp9FF,((_tmp13BD.ptr_atts=((_tmp13BE.rgn=_tmpA4A,((
_tmp13BE.nullable=_tmpA02,((_tmp13BE.bounds=_tmpA4B,((_tmp13BE.zero_term=_tmpA04,((
_tmp13BE.ptrloc=0,_tmp13BE)))))))))),_tmp13BD)))))),_tmp13BF)))),_tmp13BC))));};}
_LL510: {struct Cyc_Absyn_FnType_struct*_tmpA05=(struct Cyc_Absyn_FnType_struct*)
_tmp9E1;if(_tmpA05->tag != 10)goto _LL512;else{_tmpA06=_tmpA05->f1;_tmpA07=_tmpA06.tvars;
_tmpA08=_tmpA06.effect;_tmpA09=_tmpA06.ret_tqual;_tmpA0A=_tmpA06.ret_typ;_tmpA0B=
_tmpA06.args;_tmpA0C=_tmpA06.c_varargs;_tmpA0D=_tmpA06.cyc_varargs;_tmpA0E=
_tmpA06.rgn_po;_tmpA0F=_tmpA06.attributes;}}_LL511:{struct Cyc_List_List*_tmpA56=
_tmpA07;for(0;_tmpA56 != 0;_tmpA56=_tmpA56->tl){struct _tuple15*_tmp13C9;struct Cyc_Absyn_VarType_struct
_tmp13C8;struct Cyc_Absyn_VarType_struct*_tmp13C7;struct Cyc_List_List*_tmp13C6;
inst=((_tmp13C6=_region_malloc(rgn,sizeof(*_tmp13C6)),((_tmp13C6->hd=((_tmp13C9=
_region_malloc(rgn,sizeof(*_tmp13C9)),((_tmp13C9->f1=(struct Cyc_Absyn_Tvar*)
_tmpA56->hd,((_tmp13C9->f2=(void*)((_tmp13C7=_cycalloc(sizeof(*_tmp13C7)),((
_tmp13C7[0]=((_tmp13C8.tag=2,((_tmp13C8.f1=(struct Cyc_Absyn_Tvar*)_tmpA56->hd,
_tmp13C8)))),_tmp13C7)))),_tmp13C9)))))),((_tmp13C6->tl=inst,_tmp13C6))))));}}{
struct Cyc_List_List*_tmpA5C;struct Cyc_List_List*_tmpA5D;struct _tuple1 _tmpA5B=((
struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple28*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmpA0B));
_tmpA5C=_tmpA5B.f1;_tmpA5D=_tmpA5B.f2;{struct Cyc_List_List*_tmpA5E=Cyc_Tcutil_substs(
rgn,inst,_tmpA5D);struct Cyc_List_List*_tmpA5F=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA5C,_tmpA5E));struct Cyc_Core_Opt*
eff2;if(_tmpA08 == 0)eff2=0;else{void*_tmpA60=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmpA08->v);if(_tmpA60 == (void*)_tmpA08->v)eff2=_tmpA08;else{struct Cyc_Core_Opt*
_tmp13CA;eff2=((_tmp13CA=_cycalloc(sizeof(*_tmp13CA)),((_tmp13CA->v=_tmpA60,
_tmp13CA))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmpA0D == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmpA63;struct Cyc_Absyn_Tqual _tmpA64;void*
_tmpA65;int _tmpA66;struct Cyc_Absyn_VarargInfo _tmpA62=*_tmpA0D;_tmpA63=_tmpA62.name;
_tmpA64=_tmpA62.tq;_tmpA65=_tmpA62.type;_tmpA66=_tmpA62.inject;{void*_tmpA67=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA65);if(_tmpA67 == _tmpA65)cyc_varargs2=_tmpA0D;else{struct Cyc_Absyn_VarargInfo*
_tmp13CB;cyc_varargs2=((_tmp13CB=_cycalloc(sizeof(*_tmp13CB)),((_tmp13CB->name=
_tmpA63,((_tmp13CB->tq=_tmpA64,((_tmp13CB->type=_tmpA67,((_tmp13CB->inject=
_tmpA66,_tmp13CB))))))))));}};}{struct Cyc_List_List*rgn_po2;struct Cyc_List_List*
_tmpA6A;struct Cyc_List_List*_tmpA6B;struct _tuple1 _tmpA69=((struct _tuple1(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,_tmpA0E);_tmpA6A=_tmpA69.f1;_tmpA6B=_tmpA69.f2;{struct Cyc_List_List*
_tmpA6C=Cyc_Tcutil_substs(rgn,inst,_tmpA6A);struct Cyc_List_List*_tmpA6D=Cyc_Tcutil_substs(
rgn,inst,_tmpA6B);if(_tmpA6C == _tmpA6A  && _tmpA6D == _tmpA6B)rgn_po2=_tmpA0E;
else{rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_zip)(_tmpA6C,_tmpA6D);}{struct Cyc_Absyn_FnType_struct _tmp13D1;struct
Cyc_Absyn_FnInfo _tmp13D0;struct Cyc_Absyn_FnType_struct*_tmp13CF;return(void*)((
_tmp13CF=_cycalloc(sizeof(*_tmp13CF)),((_tmp13CF[0]=((_tmp13D1.tag=10,((_tmp13D1.f1=((
_tmp13D0.tvars=_tmpA07,((_tmp13D0.effect=eff2,((_tmp13D0.ret_tqual=_tmpA09,((
_tmp13D0.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA0A),((_tmp13D0.args=
_tmpA5F,((_tmp13D0.c_varargs=_tmpA0C,((_tmp13D0.cyc_varargs=cyc_varargs2,((
_tmp13D0.rgn_po=rgn_po2,((_tmp13D0.attributes=_tmpA0F,_tmp13D0)))))))))))))))))),
_tmp13D1)))),_tmp13CF))));};};};};};};_LL512: {struct Cyc_Absyn_TupleType_struct*
_tmpA10=(struct Cyc_Absyn_TupleType_struct*)_tmp9E1;if(_tmpA10->tag != 11)goto
_LL514;else{_tmpA11=_tmpA10->f1;}}_LL513: {struct Cyc_List_List*_tmpA71=((struct
Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*
x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,_tmpA11);struct Cyc_List_List*_tmpA72=
Cyc_Tcutil_substs(rgn,inst,_tmpA71);if(_tmpA72 == _tmpA71)return t;{struct Cyc_List_List*
_tmpA73=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*,void*),
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmpA11,_tmpA72);struct Cyc_Absyn_TupleType_struct _tmp13D4;struct Cyc_Absyn_TupleType_struct*
_tmp13D3;return(void*)((_tmp13D3=_cycalloc(sizeof(*_tmp13D3)),((_tmp13D3[0]=((
_tmp13D4.tag=11,((_tmp13D4.f1=_tmpA73,_tmp13D4)))),_tmp13D3))));};}_LL514: {
struct Cyc_Absyn_AnonAggrType_struct*_tmpA12=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp9E1;if(_tmpA12->tag != 13)goto _LL516;else{_tmpA13=_tmpA12->f1;_tmpA14=_tmpA12->f2;}}
_LL515: {struct Cyc_List_List*_tmpA76=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(
rgn,Cyc_Tcutil_field_type,_tmpA14);struct Cyc_List_List*_tmpA77=Cyc_Tcutil_substs(
rgn,inst,_tmpA76);if(_tmpA77 == _tmpA76)return t;{struct Cyc_List_List*_tmpA78=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,
_tmpA14,_tmpA77);struct Cyc_Absyn_AnonAggrType_struct _tmp13D7;struct Cyc_Absyn_AnonAggrType_struct*
_tmp13D6;return(void*)((_tmp13D6=_cycalloc(sizeof(*_tmp13D6)),((_tmp13D6[0]=((
_tmp13D7.tag=13,((_tmp13D7.f1=_tmpA13,((_tmp13D7.f2=_tmpA78,_tmp13D7)))))),
_tmp13D6))));};}_LL516: {struct Cyc_Absyn_Evar_struct*_tmpA15=(struct Cyc_Absyn_Evar_struct*)
_tmp9E1;if(_tmpA15->tag != 1)goto _LL518;else{_tmpA16=_tmpA15->f2;}}_LL517: if(
_tmpA16 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmpA16->v);else{return
t;}_LL518: {struct Cyc_Absyn_RgnHandleType_struct*_tmpA17=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp9E1;if(_tmpA17->tag != 16)goto _LL51A;else{_tmpA18=(void*)_tmpA17->f1;}}_LL519: {
void*_tmpA7B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA18);struct Cyc_Absyn_RgnHandleType_struct
_tmp13DA;struct Cyc_Absyn_RgnHandleType_struct*_tmp13D9;return _tmpA7B == _tmpA18?t:(
void*)((_tmp13D9=_cycalloc(sizeof(*_tmp13D9)),((_tmp13D9[0]=((_tmp13DA.tag=16,((
_tmp13DA.f1=(void*)_tmpA7B,_tmp13DA)))),_tmp13D9))));}_LL51A: {struct Cyc_Absyn_DynRgnType_struct*
_tmpA19=(struct Cyc_Absyn_DynRgnType_struct*)_tmp9E1;if(_tmpA19->tag != 17)goto
_LL51C;else{_tmpA1A=(void*)_tmpA19->f1;_tmpA1B=(void*)_tmpA19->f2;}}_LL51B: {
void*_tmpA7E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA1A);void*_tmpA7F=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA1B);struct Cyc_Absyn_DynRgnType_struct _tmp13DD;struct Cyc_Absyn_DynRgnType_struct*
_tmp13DC;return _tmpA7E == _tmpA1A  && _tmpA7F == _tmpA1B?t:(void*)((_tmp13DC=
_cycalloc(sizeof(*_tmp13DC)),((_tmp13DC[0]=((_tmp13DD.tag=17,((_tmp13DD.f1=(void*)
_tmpA7E,((_tmp13DD.f2=(void*)_tmpA7F,_tmp13DD)))))),_tmp13DC))));}_LL51C: {
struct Cyc_Absyn_TagType_struct*_tmpA1C=(struct Cyc_Absyn_TagType_struct*)_tmp9E1;
if(_tmpA1C->tag != 20)goto _LL51E;else{_tmpA1D=(void*)_tmpA1C->f1;}}_LL51D: {void*
_tmpA82=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA1D);struct Cyc_Absyn_TagType_struct
_tmp13E0;struct Cyc_Absyn_TagType_struct*_tmp13DF;return _tmpA82 == _tmpA1D?t:(void*)((
_tmp13DF=_cycalloc(sizeof(*_tmp13DF)),((_tmp13DF[0]=((_tmp13E0.tag=20,((_tmp13E0.f1=(
void*)_tmpA82,_tmp13E0)))),_tmp13DF))));}_LL51E: {struct Cyc_Absyn_ValueofType_struct*
_tmpA1E=(struct Cyc_Absyn_ValueofType_struct*)_tmp9E1;if(_tmpA1E->tag != 19)goto
_LL520;else{_tmpA1F=_tmpA1E->f1;}}_LL51F: {struct Cyc_Absyn_Exp*_tmpA85=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmpA1F);struct Cyc_Absyn_ValueofType_struct _tmp13E3;struct Cyc_Absyn_ValueofType_struct*
_tmp13E2;return _tmpA85 == _tmpA1F?t:(void*)((_tmp13E2=_cycalloc(sizeof(*_tmp13E2)),((
_tmp13E2[0]=((_tmp13E3.tag=19,((_tmp13E3.f1=_tmpA85,_tmp13E3)))),_tmp13E2))));}
_LL520: {struct Cyc_Absyn_EnumType_struct*_tmpA20=(struct Cyc_Absyn_EnumType_struct*)
_tmp9E1;if(_tmpA20->tag != 14)goto _LL522;}_LL521: goto _LL523;_LL522: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpA21=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp9E1;if(_tmpA21->tag != 15)goto
_LL524;}_LL523: goto _LL525;_LL524: {struct Cyc_Absyn_VoidType_struct*_tmpA22=(
struct Cyc_Absyn_VoidType_struct*)_tmp9E1;if(_tmpA22->tag != 0)goto _LL526;}_LL525:
goto _LL527;_LL526: {struct Cyc_Absyn_IntType_struct*_tmpA23=(struct Cyc_Absyn_IntType_struct*)
_tmp9E1;if(_tmpA23->tag != 6)goto _LL528;}_LL527: goto _LL529;_LL528: {struct Cyc_Absyn_FloatType_struct*
_tmpA24=(struct Cyc_Absyn_FloatType_struct*)_tmp9E1;if(_tmpA24->tag != 7)goto
_LL52A;}_LL529: goto _LL52B;_LL52A: {struct Cyc_Absyn_DoubleType_struct*_tmpA25=(
struct Cyc_Absyn_DoubleType_struct*)_tmp9E1;if(_tmpA25->tag != 8)goto _LL52C;}
_LL52B: goto _LL52D;_LL52C: {struct Cyc_Absyn_UniqueRgn_struct*_tmpA26=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp9E1;if(_tmpA26->tag != 22)goto _LL52E;}_LL52D: goto _LL52F;_LL52E: {struct Cyc_Absyn_HeapRgn_struct*
_tmpA27=(struct Cyc_Absyn_HeapRgn_struct*)_tmp9E1;if(_tmpA27->tag != 21)goto _LL530;}
_LL52F: return t;_LL530: {struct Cyc_Absyn_RgnsEff_struct*_tmpA28=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp9E1;if(_tmpA28->tag != 25)goto _LL532;else{_tmpA29=(void*)_tmpA28->f1;}}_LL531: {
void*_tmpA88=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA29);struct Cyc_Absyn_RgnsEff_struct
_tmp13E6;struct Cyc_Absyn_RgnsEff_struct*_tmp13E5;return _tmpA88 == _tmpA29?t:(void*)((
_tmp13E5=_cycalloc(sizeof(*_tmp13E5)),((_tmp13E5[0]=((_tmp13E6.tag=25,((_tmp13E6.f1=(
void*)_tmpA88,_tmp13E6)))),_tmp13E5))));}_LL532: {struct Cyc_Absyn_AccessEff_struct*
_tmpA2A=(struct Cyc_Absyn_AccessEff_struct*)_tmp9E1;if(_tmpA2A->tag != 23)goto
_LL534;else{_tmpA2B=(void*)_tmpA2A->f1;}}_LL533: {void*_tmpA8B=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA2B);struct Cyc_Absyn_AccessEff_struct _tmp13E9;struct Cyc_Absyn_AccessEff_struct*
_tmp13E8;return _tmpA8B == _tmpA2B?t:(void*)((_tmp13E8=_cycalloc(sizeof(*_tmp13E8)),((
_tmp13E8[0]=((_tmp13E9.tag=23,((_tmp13E9.f1=(void*)_tmpA8B,_tmp13E9)))),_tmp13E8))));}
_LL534: {struct Cyc_Absyn_JoinEff_struct*_tmpA2C=(struct Cyc_Absyn_JoinEff_struct*)
_tmp9E1;if(_tmpA2C->tag != 24)goto _LL536;else{_tmpA2D=_tmpA2C->f1;}}_LL535: {
struct Cyc_List_List*_tmpA8E=Cyc_Tcutil_substs(rgn,inst,_tmpA2D);struct Cyc_Absyn_JoinEff_struct
_tmp13EC;struct Cyc_Absyn_JoinEff_struct*_tmp13EB;return _tmpA8E == _tmpA2D?t:(void*)((
_tmp13EB=_cycalloc(sizeof(*_tmp13EB)),((_tmp13EB[0]=((_tmp13EC.tag=24,((_tmp13EC.f1=
_tmpA8E,_tmp13EC)))),_tmp13EB))));}_LL536: {struct Cyc_Absyn_TypeDeclType_struct*
_tmpA2E=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp9E1;if(_tmpA2E->tag != 26)goto
_LL501;}_LL537: {const char*_tmp13EF;void*_tmp13EE;(_tmp13EE=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13EF="found typedecltype in rsubs",
_tag_dyneither(_tmp13EF,sizeof(char),28))),_tag_dyneither(_tmp13EE,sizeof(void*),
0)));}_LL501:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*
rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*
_tmpA93=(void*)ts->hd;struct Cyc_List_List*_tmpA94=ts->tl;void*_tmpA95=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA93);struct Cyc_List_List*_tmpA96=Cyc_Tcutil_substs(rgn,inst,_tmpA94);
if(_tmpA93 == _tmpA95  && _tmpA94 == _tmpA96)return ts;{struct Cyc_List_List*_tmp13F0;
return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmp13F0=_cycalloc(sizeof(*
_tmp13F0)),((_tmp13F0->hd=_tmpA95,((_tmp13F0->tl=_tmpA96,_tmp13F0)))))));};};}
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(
Cyc_Core_heap_region,inst,t);}struct _tuple15*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*
s,struct Cyc_Absyn_Tvar*tv){struct Cyc_Core_Opt*_tmpA98=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(
tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp13F3;struct _tuple15*_tmp13F2;return(
_tmp13F2=_cycalloc(sizeof(*_tmp13F2)),((_tmp13F2->f1=tv,((_tmp13F2->f2=Cyc_Absyn_new_evar(
_tmpA98,((_tmp13F3=_cycalloc(sizeof(*_tmp13F3)),((_tmp13F3->v=s,_tmp13F3))))),
_tmp13F2)))));}struct _tuple15*Cyc_Tcutil_r_make_inst_var(struct _tuple16*env,
struct Cyc_Absyn_Tvar*tv){struct _tuple16 _tmpA9C;struct Cyc_List_List*_tmpA9D;
struct _RegionHandle*_tmpA9E;struct _tuple16*_tmpA9B=env;_tmpA9C=*_tmpA9B;_tmpA9D=
_tmpA9C.f1;_tmpA9E=_tmpA9C.f2;{struct Cyc_Core_Opt*_tmpA9F=Cyc_Tcutil_kind_to_opt(
Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp13F6;struct
_tuple15*_tmp13F5;return(_tmp13F5=_region_malloc(_tmpA9E,sizeof(*_tmp13F5)),((
_tmp13F5->f1=tv,((_tmp13F5->f2=Cyc_Absyn_new_evar(_tmpA9F,((_tmp13F6=_cycalloc(
sizeof(*_tmp13F6)),((_tmp13F6->v=_tmpA9D,_tmp13F6))))),_tmp13F5)))));};}static
struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;
for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,
tv->name)== 0){void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){const char*_tmp13FC;void*_tmp13FB[3];
struct Cyc_String_pa_struct _tmp13FA;struct Cyc_String_pa_struct _tmp13F9;struct Cyc_String_pa_struct
_tmp13F8;(_tmp13F8.tag=0,((_tmp13F8.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kindbound2string(k2)),((_tmp13F9.tag=0,((_tmp13F9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((
_tmp13FA.tag=0,((_tmp13FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((
_tmp13FB[0]=& _tmp13FA,((_tmp13FB[1]=& _tmp13F9,((_tmp13FB[2]=& _tmp13F8,Cyc_Tcutil_terr(
loc,((_tmp13FC="type variable %s is used with inconsistent kinds %s and %s",
_tag_dyneither(_tmp13FC,sizeof(char),59))),_tag_dyneither(_tmp13FB,sizeof(void*),
3)))))))))))))))))));}if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp13FF;void*_tmp13FE;(_tmp13FE=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13FF="same type variable has different identity!",
_tag_dyneither(_tmp13FF,sizeof(char),43))),_tag_dyneither(_tmp13FE,sizeof(void*),
0)));}}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*
_tmp1400;return(_tmp1400=_cycalloc(sizeof(*_tmp1400)),((_tmp1400->hd=tv,((
_tmp1400->tl=tvs,_tmp1400)))));};}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp1403;void*_tmp1402;(_tmp1402=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1403="fast_add_free_tvar: bad identity in tv",
_tag_dyneither(_tmp1403,sizeof(char),39))),_tag_dyneither(_tmp1402,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmpAAC=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmpAAC->identity == - 1){const char*
_tmp1406;void*_tmp1405;(_tmp1405=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1406="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmp1406,sizeof(char),41))),_tag_dyneither(_tmp1405,sizeof(void*),
0)));}if(_tmpAAC->identity == tv->identity)return tvs;}}{struct Cyc_List_List*
_tmp1407;return(_tmp1407=_cycalloc(sizeof(*_tmp1407)),((_tmp1407->hd=tv,((
_tmp1407->tl=tvs,_tmp1407)))));};}struct _tuple29{struct Cyc_Absyn_Tvar*f1;int f2;};
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(tv->identity == - 1){
const char*_tmp140A;void*_tmp1409;(_tmp1409=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp140A="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmp140A,sizeof(char),44))),_tag_dyneither(_tmp1409,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple29
_tmpAB3;struct Cyc_Absyn_Tvar*_tmpAB4;int _tmpAB5;int*_tmpAB6;struct _tuple29*
_tmpAB2=(struct _tuple29*)tvs2->hd;_tmpAB3=*_tmpAB2;_tmpAB4=_tmpAB3.f1;_tmpAB5=
_tmpAB3.f2;_tmpAB6=(int*)&(*_tmpAB2).f2;if(_tmpAB4->identity == - 1){const char*
_tmp140D;void*_tmp140C;(_tmp140C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp140D="fast_add_free_tvar_bool: bad identity in tvs2",
_tag_dyneither(_tmp140D,sizeof(char),46))),_tag_dyneither(_tmp140C,sizeof(void*),
0)));}if(_tmpAB4->identity == tv->identity){*_tmpAB6=*_tmpAB6  || b;return tvs;}}}{
struct _tuple29*_tmp1410;struct Cyc_List_List*_tmp140F;return(_tmp140F=
_region_malloc(r,sizeof(*_tmp140F)),((_tmp140F->hd=((_tmp1410=_region_malloc(r,
sizeof(*_tmp1410)),((_tmp1410->f1=tv,((_tmp1410->f2=b,_tmp1410)))))),((_tmp140F->tl=
tvs,_tmp140F)))));};}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp1414;void*_tmp1413[1];struct Cyc_String_pa_struct _tmp1412;(_tmp1412.tag=0,((
_tmp1412.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(
tv)),((_tmp1413[0]=& _tmp1412,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1414="bound tvar id for %s is NULL",
_tag_dyneither(_tmp1414,sizeof(char),29))),_tag_dyneither(_tmp1413,sizeof(void*),
1)))))));}{struct Cyc_List_List*_tmp1415;return(_tmp1415=_cycalloc(sizeof(*
_tmp1415)),((_tmp1415->hd=tv,((_tmp1415->tl=tvs,_tmp1415)))));};}struct _tuple30{
void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct
_RegionHandle*r,struct Cyc_List_List*es,void*e,int b){void*_tmpABF=Cyc_Tcutil_compress(
e);int _tmpAC1;_LL543: {struct Cyc_Absyn_Evar_struct*_tmpAC0=(struct Cyc_Absyn_Evar_struct*)
_tmpABF;if(_tmpAC0->tag != 1)goto _LL545;else{_tmpAC1=_tmpAC0->f3;}}_LL544:{struct
Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){struct _tuple30 _tmpAC3;void*
_tmpAC4;int _tmpAC5;int*_tmpAC6;struct _tuple30*_tmpAC2=(struct _tuple30*)es2->hd;
_tmpAC3=*_tmpAC2;_tmpAC4=_tmpAC3.f1;_tmpAC5=_tmpAC3.f2;_tmpAC6=(int*)&(*_tmpAC2).f2;{
void*_tmpAC7=Cyc_Tcutil_compress(_tmpAC4);int _tmpAC9;_LL548: {struct Cyc_Absyn_Evar_struct*
_tmpAC8=(struct Cyc_Absyn_Evar_struct*)_tmpAC7;if(_tmpAC8->tag != 1)goto _LL54A;
else{_tmpAC9=_tmpAC8->f3;}}_LL549: if(_tmpAC1 == _tmpAC9){if(b != *_tmpAC6)*_tmpAC6=
1;return es;}goto _LL547;_LL54A:;_LL54B: goto _LL547;_LL547:;};}}{struct _tuple30*
_tmp1418;struct Cyc_List_List*_tmp1417;return(_tmp1417=_region_malloc(r,sizeof(*
_tmp1417)),((_tmp1417->hd=((_tmp1418=_region_malloc(r,sizeof(*_tmp1418)),((
_tmp1418->f1=e,((_tmp1418->f2=b,_tmp1418)))))),((_tmp1417->tl=es,_tmp1417)))));};
_LL545:;_LL546: return es;_LL542:;}static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=
btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((
struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){struct
Cyc_List_List*_tmp1419;r=((_tmp1419=_region_malloc(rgn,sizeof(*_tmp1419)),((
_tmp1419->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1419->tl=r,_tmp1419))))));}}r=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpACE;int _tmpACF;struct
_tuple29 _tmpACD=*((struct _tuple29*)tvs->hd);_tmpACE=_tmpACD.f1;_tmpACF=_tmpACD.f2;{
int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(_tmpACE->identity
== ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){
struct Cyc_List_List*_tmp141A;res=((_tmp141A=_region_malloc(r,sizeof(*_tmp141A)),((
_tmp141A->hd=(struct _tuple29*)tvs->hd,((_tmp141A->tl=res,_tmp141A))))));}};}res=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);return res;}
void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){if(width != 
0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)width)){
const char*_tmp141E;void*_tmp141D[1];struct Cyc_String_pa_struct _tmp141C;(_tmp141C.tag=
0,((_tmp141C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp141D[0]=&
_tmp141C,Cyc_Tcutil_terr(loc,((_tmp141E="bitfield %s does not have constant width",
_tag_dyneither(_tmp141E,sizeof(char),41))),_tag_dyneither(_tmp141D,sizeof(void*),
1)))))));}else{unsigned int _tmpAD5;int _tmpAD6;struct _tuple13 _tmpAD4=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmpAD5=_tmpAD4.f1;_tmpAD6=_tmpAD4.f2;if(!_tmpAD6){
const char*_tmp1421;void*_tmp1420;(_tmp1420=0,Cyc_Tcutil_terr(loc,((_tmp1421="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmp1421,sizeof(char),45))),_tag_dyneither(_tmp1420,sizeof(void*),
0)));}w=_tmpAD5;}{void*_tmpAD9=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of
_tmpADB;_LL54D: {struct Cyc_Absyn_IntType_struct*_tmpADA=(struct Cyc_Absyn_IntType_struct*)
_tmpAD9;if(_tmpADA->tag != 6)goto _LL54F;else{_tmpADB=_tmpADA->f2;}}_LL54E: switch(
_tmpADB){case Cyc_Absyn_Char_sz: _LL551: if(w > 8){const char*_tmp1424;void*_tmp1423;(
_tmp1423=0,Cyc_Tcutil_terr(loc,((_tmp1424="bitfield larger than type",
_tag_dyneither(_tmp1424,sizeof(char),26))),_tag_dyneither(_tmp1423,sizeof(void*),
0)));}break;case Cyc_Absyn_Short_sz: _LL552: if(w > 16){const char*_tmp1427;void*
_tmp1426;(_tmp1426=0,Cyc_Tcutil_terr(loc,((_tmp1427="bitfield larger than type",
_tag_dyneither(_tmp1427,sizeof(char),26))),_tag_dyneither(_tmp1426,sizeof(void*),
0)));}break;case Cyc_Absyn_Long_sz: _LL553: goto _LL554;case Cyc_Absyn_Int_sz: _LL554:
if(w > 32){const char*_tmp142A;void*_tmp1429;(_tmp1429=0,Cyc_Tcutil_terr(loc,((
_tmp142A="bitfield larger than type",_tag_dyneither(_tmp142A,sizeof(char),26))),
_tag_dyneither(_tmp1429,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz:
_LL555: if(w > 64){const char*_tmp142D;void*_tmp142C;(_tmp142C=0,Cyc_Tcutil_terr(
loc,((_tmp142D="bitfield larger than type",_tag_dyneither(_tmp142D,sizeof(char),
26))),_tag_dyneither(_tmp142C,sizeof(void*),0)));}break;}goto _LL54C;_LL54F:;
_LL550:{const char*_tmp1432;void*_tmp1431[2];struct Cyc_String_pa_struct _tmp1430;
struct Cyc_String_pa_struct _tmp142F;(_tmp142F.tag=0,((_tmp142F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_typ)),((
_tmp1430.tag=0,((_tmp1430.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((
_tmp1431[0]=& _tmp1430,((_tmp1431[1]=& _tmp142F,Cyc_Tcutil_terr(loc,((_tmp1432="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmp1432,sizeof(char),52))),_tag_dyneither(_tmp1431,sizeof(void*),
2)))))))))))));}goto _LL54C;_LL54C:;};}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmpAE8=(void*)atts->hd;_LL558: {struct Cyc_Absyn_Packed_att_struct*
_tmpAE9=(struct Cyc_Absyn_Packed_att_struct*)_tmpAE8;if(_tmpAE9->tag != 7)goto
_LL55A;}_LL559: continue;_LL55A: {struct Cyc_Absyn_Aligned_att_struct*_tmpAEA=(
struct Cyc_Absyn_Aligned_att_struct*)_tmpAE8;if(_tmpAEA->tag != 6)goto _LL55C;}
_LL55B: continue;_LL55C:;_LL55D: {const char*_tmp1437;void*_tmp1436[2];struct Cyc_String_pa_struct
_tmp1435;struct Cyc_String_pa_struct _tmp1434;(_tmp1434.tag=0,((_tmp1434.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1435.tag=0,((_tmp1435.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)
atts->hd)),((_tmp1436[0]=& _tmp1435,((_tmp1436[1]=& _tmp1434,Cyc_Tcutil_terr(loc,((
_tmp1437="bad attribute %s on member %s",_tag_dyneither(_tmp1437,sizeof(char),30))),
_tag_dyneither(_tmp1436,sizeof(void*),2)))))))))))));}_LL557:;}}struct Cyc_Tcutil_CVTEnv{
struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;
struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmpAEF=t;struct
Cyc_Absyn_Typedefdecl*_tmpAF1;void**_tmpAF2;_LL55F: {struct Cyc_Absyn_TypedefType_struct*
_tmpAF0=(struct Cyc_Absyn_TypedefType_struct*)_tmpAEF;if(_tmpAF0->tag != 18)goto
_LL561;else{_tmpAF1=_tmpAF0->f3;_tmpAF2=_tmpAF0->f4;}}_LL560: if((((struct Cyc_Absyn_Typedefdecl*)
_check_null(_tmpAF1))->tq).real_const  || (_tmpAF1->tq).print_const){if(
declared_const){const char*_tmp143A;void*_tmp1439;(_tmp1439=0,Cyc_Tcutil_warn(loc,((
_tmp143A="extra const",_tag_dyneither(_tmp143A,sizeof(char),12))),_tag_dyneither(
_tmp1439,sizeof(void*),0)));}return 1;}if((unsigned int)_tmpAF2)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmpAF2);else{return declared_const;}_LL561:;_LL562: return
declared_const;_LL55E:;}static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Typedefdecl*td){if(td != 0){if(td->defn != 0){void*_tmpAF5=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);
struct Cyc_Absyn_PtrInfo _tmpAF7;struct Cyc_Absyn_PtrAtts _tmpAF8;void*_tmpAF9;
_LL564: {struct Cyc_Absyn_PointerType_struct*_tmpAF6=(struct Cyc_Absyn_PointerType_struct*)
_tmpAF5;if(_tmpAF6->tag != 5)goto _LL566;else{_tmpAF7=_tmpAF6->f1;_tmpAF8=_tmpAF7.ptr_atts;
_tmpAF9=_tmpAF8.rgn;}}_LL565:{void*_tmpAFA=Cyc_Tcutil_compress(_tmpAF9);struct
Cyc_Absyn_Tvar*_tmpAFC;_LL569: {struct Cyc_Absyn_VarType_struct*_tmpAFB=(struct
Cyc_Absyn_VarType_struct*)_tmpAFA;if(_tmpAFB->tag != 2)goto _LL56B;else{_tmpAFC=
_tmpAFB->f1;}}_LL56A: return Cyc_Absyn_tvar_cmp(tvar,_tmpAFC)== 0;_LL56B:;_LL56C:
goto _LL568;_LL568:;}goto _LL563;_LL566:;_LL567: goto _LL563;_LL563:;}}else{return 1;}
return 0;}static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*
td){void*_tmpAFD=Cyc_Absyn_compress_kb(tvar->kind);struct Cyc_Absyn_Kind*_tmpAFF;
struct Cyc_Absyn_Kind _tmpB00;enum Cyc_Absyn_KindQual _tmpB01;enum Cyc_Absyn_AliasQual
_tmpB02;struct Cyc_Absyn_Kind*_tmpB04;struct Cyc_Absyn_Kind _tmpB05;enum Cyc_Absyn_KindQual
_tmpB06;enum Cyc_Absyn_AliasQual _tmpB07;_LL56E: {struct Cyc_Absyn_Less_kb_struct*
_tmpAFE=(struct Cyc_Absyn_Less_kb_struct*)_tmpAFD;if(_tmpAFE->tag != 2)goto _LL570;
else{_tmpAFF=_tmpAFE->f2;_tmpB00=*_tmpAFF;_tmpB01=_tmpB00.kind;if(_tmpB01 != Cyc_Absyn_RgnKind)
goto _LL570;_tmpB02=_tmpB00.aliasqual;if(_tmpB02 != Cyc_Absyn_Top)goto _LL570;}}
_LL56F: goto _LL571;_LL570: {struct Cyc_Absyn_Eq_kb_struct*_tmpB03=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpAFD;if(_tmpB03->tag != 0)goto _LL572;else{_tmpB04=_tmpB03->f1;_tmpB05=*_tmpB04;
_tmpB06=_tmpB05.kind;if(_tmpB06 != Cyc_Absyn_RgnKind)goto _LL572;_tmpB07=_tmpB05.aliasqual;
if(_tmpB07 != Cyc_Absyn_Top)goto _LL572;}}_LL571: if(((expected_kind->kind == Cyc_Absyn_BoxKind
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
_tmpB08=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpB0B;struct Cyc_Core_Opt**
_tmpB0C;struct Cyc_Core_Opt*_tmpB0D;struct Cyc_Core_Opt**_tmpB0E;struct Cyc_Absyn_Tvar*
_tmpB10;struct Cyc_Absyn_TypeDecl*_tmpB12;struct Cyc_Absyn_TypeDecl _tmpB13;void*
_tmpB14;void**_tmpB15;void***_tmpB16;struct Cyc_List_List*_tmpB18;struct _tuple2*
_tmpB1A;struct Cyc_Absyn_Enumdecl*_tmpB1B;struct Cyc_Absyn_Enumdecl**_tmpB1C;
struct Cyc_Absyn_DatatypeInfo _tmpB1E;union Cyc_Absyn_DatatypeInfoU _tmpB1F;union Cyc_Absyn_DatatypeInfoU*
_tmpB20;struct Cyc_List_List*_tmpB21;struct Cyc_List_List**_tmpB22;struct Cyc_Absyn_DatatypeFieldInfo
_tmpB24;union Cyc_Absyn_DatatypeFieldInfoU _tmpB25;union Cyc_Absyn_DatatypeFieldInfoU*
_tmpB26;struct Cyc_List_List*_tmpB27;struct Cyc_Absyn_PtrInfo _tmpB29;void*_tmpB2A;
struct Cyc_Absyn_Tqual _tmpB2B;struct Cyc_Absyn_Tqual*_tmpB2C;struct Cyc_Absyn_PtrAtts
_tmpB2D;void*_tmpB2E;union Cyc_Absyn_Constraint*_tmpB2F;union Cyc_Absyn_Constraint*
_tmpB30;union Cyc_Absyn_Constraint*_tmpB31;void*_tmpB33;struct Cyc_Absyn_Exp*
_tmpB35;struct Cyc_Absyn_ArrayInfo _tmpB3A;void*_tmpB3B;struct Cyc_Absyn_Tqual
_tmpB3C;struct Cyc_Absyn_Tqual*_tmpB3D;struct Cyc_Absyn_Exp*_tmpB3E;struct Cyc_Absyn_Exp**
_tmpB3F;union Cyc_Absyn_Constraint*_tmpB40;struct Cyc_Position_Segment*_tmpB41;
struct Cyc_Absyn_FnInfo _tmpB43;struct Cyc_List_List*_tmpB44;struct Cyc_List_List**
_tmpB45;struct Cyc_Core_Opt*_tmpB46;struct Cyc_Core_Opt**_tmpB47;struct Cyc_Absyn_Tqual
_tmpB48;struct Cyc_Absyn_Tqual*_tmpB49;void*_tmpB4A;struct Cyc_List_List*_tmpB4B;
int _tmpB4C;struct Cyc_Absyn_VarargInfo*_tmpB4D;struct Cyc_List_List*_tmpB4E;struct
Cyc_List_List*_tmpB4F;struct Cyc_List_List*_tmpB51;enum Cyc_Absyn_AggrKind _tmpB53;
struct Cyc_List_List*_tmpB54;struct Cyc_Absyn_AggrInfo _tmpB56;union Cyc_Absyn_AggrInfoU
_tmpB57;union Cyc_Absyn_AggrInfoU*_tmpB58;struct Cyc_List_List*_tmpB59;struct Cyc_List_List**
_tmpB5A;struct _tuple2*_tmpB5C;struct Cyc_List_List*_tmpB5D;struct Cyc_List_List**
_tmpB5E;struct Cyc_Absyn_Typedefdecl*_tmpB5F;struct Cyc_Absyn_Typedefdecl**_tmpB60;
void**_tmpB61;void***_tmpB62;void*_tmpB66;void*_tmpB68;void*_tmpB69;void*_tmpB6B;
void*_tmpB6D;struct Cyc_List_List*_tmpB6F;_LL575: {struct Cyc_Absyn_VoidType_struct*
_tmpB09=(struct Cyc_Absyn_VoidType_struct*)_tmpB08;if(_tmpB09->tag != 0)goto _LL577;}
_LL576: goto _LL574;_LL577: {struct Cyc_Absyn_Evar_struct*_tmpB0A=(struct Cyc_Absyn_Evar_struct*)
_tmpB08;if(_tmpB0A->tag != 1)goto _LL579;else{_tmpB0B=_tmpB0A->f1;_tmpB0C=(struct
Cyc_Core_Opt**)& _tmpB0A->f1;_tmpB0D=_tmpB0A->f2;_tmpB0E=(struct Cyc_Core_Opt**)&
_tmpB0A->f2;}}_LL578: if(*_tmpB0C == 0  || Cyc_Tcutil_kind_leq(expected_kind,(
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmpB0C))->v) && !Cyc_Tcutil_kind_leq((
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmpB0C))->v,
expected_kind))*_tmpB0C=Cyc_Tcutil_kind_to_opt(expected_kind);if((cvtenv.fn_result
 && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){if(
expected_kind->aliasqual == Cyc_Absyn_Unique)*_tmpB0E=(struct Cyc_Core_Opt*)& urgn;
else{*_tmpB0E=(struct Cyc_Core_Opt*)& hrgn;}}else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_struct _tmp143D;struct Cyc_Absyn_Less_kb_struct*_tmp143C;
struct Cyc_Absyn_Tvar*_tmpB70=Cyc_Tcutil_new_tvar((void*)((_tmp143C=_cycalloc(
sizeof(*_tmp143C)),((_tmp143C[0]=((_tmp143D.tag=2,((_tmp143D.f1=0,((_tmp143D.f2=
expected_kind,_tmp143D)))))),_tmp143C)))));{struct Cyc_Absyn_VarType_struct*
_tmp1443;struct Cyc_Absyn_VarType_struct _tmp1442;struct Cyc_Core_Opt*_tmp1441;*
_tmpB0E=((_tmp1441=_cycalloc(sizeof(*_tmp1441)),((_tmp1441->v=(void*)((_tmp1443=
_cycalloc(sizeof(*_tmp1443)),((_tmp1443[0]=((_tmp1442.tag=2,((_tmp1442.f1=
_tmpB70,_tmp1442)))),_tmp1443)))),_tmp1441))));}_tmpB10=_tmpB70;goto _LL57A;}
else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,
put_in_effect);}}goto _LL574;_LL579: {struct Cyc_Absyn_VarType_struct*_tmpB0F=(
struct Cyc_Absyn_VarType_struct*)_tmpB08;if(_tmpB0F->tag != 2)goto _LL57B;else{
_tmpB10=_tmpB0F->f1;}}_LL57A:{void*_tmpB76=Cyc_Absyn_compress_kb(_tmpB10->kind);
struct Cyc_Core_Opt*_tmpB78;struct Cyc_Core_Opt**_tmpB79;_LL5AC: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpB77=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpB76;if(_tmpB77->tag != 1)goto
_LL5AE;else{_tmpB78=_tmpB77->f1;_tmpB79=(struct Cyc_Core_Opt**)& _tmpB77->f1;}}
_LL5AD:{struct Cyc_Absyn_Less_kb_struct*_tmp1449;struct Cyc_Absyn_Less_kb_struct
_tmp1448;struct Cyc_Core_Opt*_tmp1447;*_tmpB79=((_tmp1447=_cycalloc(sizeof(*
_tmp1447)),((_tmp1447->v=(void*)((_tmp1449=_cycalloc(sizeof(*_tmp1449)),((
_tmp1449[0]=((_tmp1448.tag=2,((_tmp1448.f1=0,((_tmp1448.f2=expected_kind,
_tmp1448)))))),_tmp1449)))),_tmp1447))));}goto _LL5AB;_LL5AE:;_LL5AF: goto _LL5AB;
_LL5AB:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpB10);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpB10,put_in_effect);{void*_tmpB7D=Cyc_Absyn_compress_kb(_tmpB10->kind);struct
Cyc_Core_Opt*_tmpB7F;struct Cyc_Core_Opt**_tmpB80;struct Cyc_Absyn_Kind*_tmpB81;
_LL5B1: {struct Cyc_Absyn_Less_kb_struct*_tmpB7E=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB7D;if(_tmpB7E->tag != 2)goto _LL5B3;else{_tmpB7F=_tmpB7E->f1;_tmpB80=(struct
Cyc_Core_Opt**)& _tmpB7E->f1;_tmpB81=_tmpB7E->f2;}}_LL5B2: if(Cyc_Tcutil_kind_leq(
expected_kind,_tmpB81)){struct Cyc_Absyn_Less_kb_struct*_tmp144F;struct Cyc_Absyn_Less_kb_struct
_tmp144E;struct Cyc_Core_Opt*_tmp144D;*_tmpB80=((_tmp144D=_cycalloc(sizeof(*
_tmp144D)),((_tmp144D->v=(void*)((_tmp144F=_cycalloc(sizeof(*_tmp144F)),((
_tmp144F[0]=((_tmp144E.tag=2,((_tmp144E.f1=0,((_tmp144E.f2=expected_kind,
_tmp144E)))))),_tmp144F)))),_tmp144D))));}goto _LL5B0;_LL5B3:;_LL5B4: goto _LL5B0;
_LL5B0:;}goto _LL574;_LL57B: {struct Cyc_Absyn_TypeDeclType_struct*_tmpB11=(struct
Cyc_Absyn_TypeDeclType_struct*)_tmpB08;if(_tmpB11->tag != 26)goto _LL57D;else{
_tmpB12=_tmpB11->f1;_tmpB13=*_tmpB12;_tmpB14=_tmpB13.r;_tmpB15=_tmpB11->f2;
_tmpB16=(void***)& _tmpB11->f2;}}_LL57C: {void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(
t));{void*_tmpB85=_tmpB14;struct Cyc_Absyn_Aggrdecl*_tmpB87;struct Cyc_Absyn_Enumdecl*
_tmpB89;struct Cyc_Absyn_Datatypedecl*_tmpB8B;_LL5B6: {struct Cyc_Absyn_Aggr_td_struct*
_tmpB86=(struct Cyc_Absyn_Aggr_td_struct*)_tmpB85;if(_tmpB86->tag != 0)goto _LL5B8;
else{_tmpB87=_tmpB86->f1;}}_LL5B7: Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,
_tmpB87);goto _LL5B5;_LL5B8: {struct Cyc_Absyn_Enum_td_struct*_tmpB88=(struct Cyc_Absyn_Enum_td_struct*)
_tmpB85;if(_tmpB88->tag != 1)goto _LL5BA;else{_tmpB89=_tmpB88->f1;}}_LL5B9: Cyc_Tc_tcEnumdecl(
te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns),loc,_tmpB89);goto _LL5B5;_LL5BA: {struct Cyc_Absyn_Datatype_td_struct*
_tmpB8A=(struct Cyc_Absyn_Datatype_td_struct*)_tmpB85;if(_tmpB8A->tag != 2)goto
_LL5B5;else{_tmpB8B=_tmpB8A->f1;}}_LL5BB: Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,
_tmpB8B);goto _LL5B5;_LL5B5:;}{void**_tmp1450;*_tmpB16=((_tmp1450=_cycalloc(
sizeof(*_tmp1450)),((_tmp1450[0]=new_t,_tmp1450))));}return Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL57D: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpB17=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpB08;if(_tmpB17->tag != 15)goto
_LL57F;else{_tmpB18=_tmpB17->f1;}}_LL57E: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle*_tmpB8D=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;for(0;_tmpB18 != 0;_tmpB18=_tmpB18->tl){
struct Cyc_Absyn_Enumfield*_tmpB8E=(struct Cyc_Absyn_Enumfield*)_tmpB18->hd;if(((
int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmpB8E->name).f2)){
const char*_tmp1454;void*_tmp1453[1];struct Cyc_String_pa_struct _tmp1452;(_tmp1452.tag=
0,((_tmp1452.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpB8E->name).f2),((
_tmp1453[0]=& _tmp1452,Cyc_Tcutil_terr(_tmpB8E->loc,((_tmp1454="duplicate enum field name %s",
_tag_dyneither(_tmp1454,sizeof(char),29))),_tag_dyneither(_tmp1453,sizeof(void*),
1)))))));}else{struct Cyc_List_List*_tmp1455;prev_fields=((_tmp1455=
_region_malloc(_tmpB8D,sizeof(*_tmp1455)),((_tmp1455->hd=(*_tmpB8E->name).f2,((
_tmp1455->tl=prev_fields,_tmp1455))))));}if(_tmpB8E->tag == 0)_tmpB8E->tag=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmpB8E->loc);else{if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(_tmpB8E->tag))){const char*_tmp1459;void*
_tmp1458[1];struct Cyc_String_pa_struct _tmp1457;(_tmp1457.tag=0,((_tmp1457.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpB8E->name).f2),((_tmp1458[0]=&
_tmp1457,Cyc_Tcutil_terr(loc,((_tmp1459="enum field %s: expression is not constant",
_tag_dyneither(_tmp1459,sizeof(char),42))),_tag_dyneither(_tmp1458,sizeof(void*),
1)))))));}}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmpB8E->tag))).f1;tag_count=t1 + 1;(*_tmpB8E->name).f1=Cyc_Absyn_Abs_n(
te->ns,0);{struct Cyc_Tcenv_AnonEnumRes_struct*_tmp145F;struct Cyc_Tcenv_AnonEnumRes_struct
_tmp145E;struct _tuple30*_tmp145D;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple30*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmpB8E->name).f2,(struct _tuple30*)((_tmp145D=_cycalloc(sizeof(*_tmp145D)),((
_tmp145D->f1=(void*)((_tmp145F=_cycalloc(sizeof(*_tmp145F)),((_tmp145F[0]=((
_tmp145E.tag=4,((_tmp145E.f1=(void*)t,((_tmp145E.f2=_tmpB8E,_tmp145E)))))),
_tmp145F)))),((_tmp145D->f2=1,_tmp145D)))))));};};}}goto _LL574;}_LL57F: {struct
Cyc_Absyn_EnumType_struct*_tmpB19=(struct Cyc_Absyn_EnumType_struct*)_tmpB08;if(
_tmpB19->tag != 14)goto _LL581;else{_tmpB1A=_tmpB19->f1;_tmpB1B=_tmpB19->f2;
_tmpB1C=(struct Cyc_Absyn_Enumdecl**)& _tmpB19->f2;}}_LL580: if(*_tmpB1C == 0  || ((
struct Cyc_Absyn_Enumdecl*)_check_null(*_tmpB1C))->fields == 0){struct
_handler_cons _tmpB99;_push_handler(& _tmpB99);{int _tmpB9B=0;if(setjmp(_tmpB99.handler))
_tmpB9B=1;if(!_tmpB9B){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmpB1A);*_tmpB1C=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{
void*_tmpB9A=(void*)_exn_thrown;void*_tmpB9D=_tmpB9A;_LL5BD: {struct Cyc_Dict_Absent_struct*
_tmpB9E=(struct Cyc_Dict_Absent_struct*)_tmpB9D;if(_tmpB9E->tag != Cyc_Dict_Absent)
goto _LL5BF;}_LL5BE: {struct Cyc_Tcenv_Genv*_tmpB9F=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp1460;struct Cyc_Absyn_Enumdecl*_tmpBA0=(_tmp1460=
_cycalloc(sizeof(*_tmp1460)),((_tmp1460->sc=Cyc_Absyn_Extern,((_tmp1460->name=
_tmpB1A,((_tmp1460->fields=0,_tmp1460)))))));Cyc_Tc_tcEnumdecl(te,_tmpB9F,loc,
_tmpBA0);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmpB1A);*
_tmpB1C=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL5BC;};}_LL5BF:;_LL5C0:(void)_throw(
_tmpB9D);_LL5BC:;}};}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmpB1C);*_tmpB1A=(ed->name)[0];goto _LL574;};_LL581: {struct Cyc_Absyn_DatatypeType_struct*
_tmpB1D=(struct Cyc_Absyn_DatatypeType_struct*)_tmpB08;if(_tmpB1D->tag != 3)goto
_LL583;else{_tmpB1E=_tmpB1D->f1;_tmpB1F=_tmpB1E.datatype_info;_tmpB20=(union Cyc_Absyn_DatatypeInfoU*)&(
_tmpB1D->f1).datatype_info;_tmpB21=_tmpB1E.targs;_tmpB22=(struct Cyc_List_List**)&(
_tmpB1D->f1).targs;}}_LL582: {struct Cyc_List_List*_tmpBA2=*_tmpB22;{union Cyc_Absyn_DatatypeInfoU
_tmpBA3=*_tmpB20;struct Cyc_Absyn_UnknownDatatypeInfo _tmpBA4;struct _tuple2*
_tmpBA5;int _tmpBA6;struct Cyc_Absyn_Datatypedecl**_tmpBA7;struct Cyc_Absyn_Datatypedecl*
_tmpBA8;_LL5C2: if((_tmpBA3.UnknownDatatype).tag != 1)goto _LL5C4;_tmpBA4=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpBA3.UnknownDatatype).val;_tmpBA5=_tmpBA4.name;
_tmpBA6=_tmpBA4.is_extensible;_LL5C3: {struct Cyc_Absyn_Datatypedecl**tudp;{
struct _handler_cons _tmpBA9;_push_handler(& _tmpBA9);{int _tmpBAB=0;if(setjmp(
_tmpBA9.handler))_tmpBAB=1;if(!_tmpBAB){tudp=Cyc_Tcenv_lookup_datatypedecl(te,
loc,_tmpBA5);;_pop_handler();}else{void*_tmpBAA=(void*)_exn_thrown;void*_tmpBAD=
_tmpBAA;_LL5C7: {struct Cyc_Dict_Absent_struct*_tmpBAE=(struct Cyc_Dict_Absent_struct*)
_tmpBAD;if(_tmpBAE->tag != Cyc_Dict_Absent)goto _LL5C9;}_LL5C8: {struct Cyc_Tcenv_Genv*
_tmpBAF=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Datatypedecl*_tmp1461;struct Cyc_Absyn_Datatypedecl*
_tmpBB0=(_tmp1461=_cycalloc(sizeof(*_tmp1461)),((_tmp1461->sc=Cyc_Absyn_Extern,((
_tmp1461->name=_tmpBA5,((_tmp1461->tvs=0,((_tmp1461->fields=0,((_tmp1461->is_extensible=
_tmpBA6,_tmp1461)))))))))));Cyc_Tc_tcDatatypedecl(te,_tmpBAF,loc,_tmpBB0);tudp=
Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpBA5);if(_tmpBA2 != 0){{const char*
_tmp1465;void*_tmp1464[1];struct Cyc_String_pa_struct _tmp1463;(_tmp1463.tag=0,((
_tmp1463.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBA5)),((_tmp1464[0]=& _tmp1463,Cyc_Tcutil_terr(loc,((_tmp1465="declare parameterized datatype %s before using",
_tag_dyneither(_tmp1465,sizeof(char),47))),_tag_dyneither(_tmp1464,sizeof(void*),
1)))))));}return cvtenv;}goto _LL5C6;}_LL5C9:;_LL5CA:(void)_throw(_tmpBAD);_LL5C6:;}};}
if(_tmpBA6  && !(*tudp)->is_extensible){const char*_tmp1469;void*_tmp1468[1];
struct Cyc_String_pa_struct _tmp1467;(_tmp1467.tag=0,((_tmp1467.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBA5)),((
_tmp1468[0]=& _tmp1467,Cyc_Tcutil_terr(loc,((_tmp1469="datatype %s was not declared @extensible",
_tag_dyneither(_tmp1469,sizeof(char),41))),_tag_dyneither(_tmp1468,sizeof(void*),
1)))))));}*_tmpB20=Cyc_Absyn_KnownDatatype(tudp);_tmpBA8=*tudp;goto _LL5C5;}
_LL5C4: if((_tmpBA3.KnownDatatype).tag != 2)goto _LL5C1;_tmpBA7=(struct Cyc_Absyn_Datatypedecl**)(
_tmpBA3.KnownDatatype).val;_tmpBA8=*_tmpBA7;_LL5C5: {struct Cyc_List_List*tvs=
_tmpBA8->tvs;for(0;_tmpBA2 != 0  && tvs != 0;(_tmpBA2=_tmpBA2->tl,tvs=tvs->tl)){
void*t=(void*)_tmpBA2->hd;struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;{
struct _tuple0 _tmp146A;struct _tuple0 _tmpBB9=(_tmp146A.f1=Cyc_Absyn_compress_kb(tv->kind),((
_tmp146A.f2=t,_tmp146A)));void*_tmpBBA;void*_tmpBBC;struct Cyc_Absyn_Tvar*_tmpBBE;
_LL5CC: _tmpBBA=_tmpBB9.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpBBB=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpBBA;if(_tmpBBB->tag != 1)goto _LL5CE;};_tmpBBC=_tmpBB9.f2;{struct Cyc_Absyn_VarType_struct*
_tmpBBD=(struct Cyc_Absyn_VarType_struct*)_tmpBBC;if(_tmpBBD->tag != 2)goto _LL5CE;
else{_tmpBBE=_tmpBBD->f1;}};_LL5CD: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpBBE);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpBBE,1);continue;_LL5CE:;_LL5CF: goto _LL5CB;_LL5CB:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);Cyc_Tcutil_check_no_qual(loc,t);};}if(_tmpBA2 != 0){const
char*_tmp146E;void*_tmp146D[1];struct Cyc_String_pa_struct _tmp146C;(_tmp146C.tag=
0,((_tmp146C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBA8->name)),((_tmp146D[0]=& _tmp146C,Cyc_Tcutil_terr(loc,((_tmp146E="too many type arguments for datatype %s",
_tag_dyneither(_tmp146E,sizeof(char),40))),_tag_dyneither(_tmp146D,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_bk,expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*
_tmp146F;hidden_ts=((_tmp146F=_cycalloc(sizeof(*_tmp146F)),((_tmp146F->hd=e,((
_tmp146F->tl=hidden_ts,_tmp146F))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,k1,e,1);}*_tmpB22=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpB22,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}goto _LL5C1;}_LL5C1:;}goto
_LL574;}_LL583: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmpB23=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpB08;if(_tmpB23->tag != 4)goto _LL585;else{_tmpB24=_tmpB23->f1;_tmpB25=_tmpB24.field_info;
_tmpB26=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmpB23->f1).field_info;_tmpB27=
_tmpB24.targs;}}_LL584:{union Cyc_Absyn_DatatypeFieldInfoU _tmpBC3=*_tmpB26;struct
Cyc_Absyn_UnknownDatatypeFieldInfo _tmpBC4;struct _tuple2*_tmpBC5;struct _tuple2*
_tmpBC6;int _tmpBC7;struct _tuple3 _tmpBC8;struct Cyc_Absyn_Datatypedecl*_tmpBC9;
struct Cyc_Absyn_Datatypefield*_tmpBCA;_LL5D1: if((_tmpBC3.UnknownDatatypefield).tag
!= 1)goto _LL5D3;_tmpBC4=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpBC3.UnknownDatatypefield).val;
_tmpBC5=_tmpBC4.datatype_name;_tmpBC6=_tmpBC4.field_name;_tmpBC7=_tmpBC4.is_extensible;
_LL5D2: {struct Cyc_Absyn_Datatypedecl*tud;struct Cyc_Absyn_Datatypefield*tuf;{
struct _handler_cons _tmpBCB;_push_handler(& _tmpBCB);{int _tmpBCD=0;if(setjmp(
_tmpBCB.handler))_tmpBCD=1;if(!_tmpBCD){*Cyc_Tcenv_lookup_datatypedecl(te,loc,
_tmpBC5);;_pop_handler();}else{void*_tmpBCC=(void*)_exn_thrown;void*_tmpBCF=
_tmpBCC;_LL5D6: {struct Cyc_Dict_Absent_struct*_tmpBD0=(struct Cyc_Dict_Absent_struct*)
_tmpBCF;if(_tmpBD0->tag != Cyc_Dict_Absent)goto _LL5D8;}_LL5D7:{const char*_tmp1473;
void*_tmp1472[1];struct Cyc_String_pa_struct _tmp1471;(_tmp1471.tag=0,((_tmp1471.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBC5)),((
_tmp1472[0]=& _tmp1471,Cyc_Tcutil_terr(loc,((_tmp1473="unbound datatype %s",
_tag_dyneither(_tmp1473,sizeof(char),20))),_tag_dyneither(_tmp1472,sizeof(void*),
1)))))));}return cvtenv;_LL5D8:;_LL5D9:(void)_throw(_tmpBCF);_LL5D5:;}};}{struct
_handler_cons _tmpBD4;_push_handler(& _tmpBD4);{int _tmpBD6=0;if(setjmp(_tmpBD4.handler))
_tmpBD6=1;if(!_tmpBD6){{struct _RegionHandle*_tmpBD7=Cyc_Tcenv_get_fnrgn(te);void*
_tmpBD8=Cyc_Tcenv_lookup_ordinary(_tmpBD7,te,loc,_tmpBC6);struct Cyc_Absyn_Datatypedecl*
_tmpBDA;struct Cyc_Absyn_Datatypefield*_tmpBDB;_LL5DB: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpBD9=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmpBD8;if(_tmpBD9->tag != 2)goto
_LL5DD;else{_tmpBDA=_tmpBD9->f1;_tmpBDB=_tmpBD9->f2;}}_LL5DC: tuf=_tmpBDB;tud=
_tmpBDA;if(_tmpBC7  && !tud->is_extensible){const char*_tmp1477;void*_tmp1476[1];
struct Cyc_String_pa_struct _tmp1475;(_tmp1475.tag=0,((_tmp1475.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBC5)),((
_tmp1476[0]=& _tmp1475,Cyc_Tcutil_terr(loc,((_tmp1477="datatype %s was not declared @extensible",
_tag_dyneither(_tmp1477,sizeof(char),41))),_tag_dyneither(_tmp1476,sizeof(void*),
1)))))));}goto _LL5DA;_LL5DD:;_LL5DE:{const char*_tmp147C;void*_tmp147B[2];struct
Cyc_String_pa_struct _tmp147A;struct Cyc_String_pa_struct _tmp1479;(_tmp1479.tag=0,((
_tmp1479.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBC5)),((_tmp147A.tag=0,((_tmp147A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBC6)),((_tmp147B[0]=& _tmp147A,((
_tmp147B[1]=& _tmp1479,Cyc_Tcutil_terr(loc,((_tmp147C="unbound field %s in type datatype %s",
_tag_dyneither(_tmp147C,sizeof(char),37))),_tag_dyneither(_tmp147B,sizeof(void*),
2)))))))))))));}{struct Cyc_Tcutil_CVTEnv _tmpBE3=cvtenv;_npop_handler(0);return
_tmpBE3;};_LL5DA:;};_pop_handler();}else{void*_tmpBD5=(void*)_exn_thrown;void*
_tmpBE5=_tmpBD5;_LL5E0: {struct Cyc_Dict_Absent_struct*_tmpBE6=(struct Cyc_Dict_Absent_struct*)
_tmpBE5;if(_tmpBE6->tag != Cyc_Dict_Absent)goto _LL5E2;}_LL5E1:{const char*_tmp1481;
void*_tmp1480[2];struct Cyc_String_pa_struct _tmp147F;struct Cyc_String_pa_struct
_tmp147E;(_tmp147E.tag=0,((_tmp147E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBC5)),((_tmp147F.tag=0,((_tmp147F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBC6)),((
_tmp1480[0]=& _tmp147F,((_tmp1480[1]=& _tmp147E,Cyc_Tcutil_terr(loc,((_tmp1481="unbound field %s in type datatype %s",
_tag_dyneither(_tmp1481,sizeof(char),37))),_tag_dyneither(_tmp1480,sizeof(void*),
2)))))))))))));}return cvtenv;_LL5E2:;_LL5E3:(void)_throw(_tmpBE5);_LL5DF:;}};}*
_tmpB26=Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmpBC9=tud;_tmpBCA=tuf;goto _LL5D4;}
_LL5D3: if((_tmpBC3.KnownDatatypefield).tag != 2)goto _LL5D0;_tmpBC8=(struct _tuple3)(
_tmpBC3.KnownDatatypefield).val;_tmpBC9=_tmpBC8.f1;_tmpBCA=_tmpBC8.f2;_LL5D4: {
struct Cyc_List_List*tvs=_tmpBC9->tvs;for(0;_tmpB27 != 0  && tvs != 0;(_tmpB27=
_tmpB27->tl,tvs=tvs->tl)){void*t=(void*)_tmpB27->hd;struct Cyc_Absyn_Tvar*tv=(
struct Cyc_Absyn_Tvar*)tvs->hd;{struct _tuple0 _tmp1482;struct _tuple0 _tmpBEC=(
_tmp1482.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1482.f2=t,_tmp1482)));void*
_tmpBED;void*_tmpBEF;struct Cyc_Absyn_Tvar*_tmpBF1;_LL5E5: _tmpBED=_tmpBEC.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmpBEE=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpBED;if(_tmpBEE->tag != 1)goto _LL5E7;};_tmpBEF=_tmpBEC.f2;{struct Cyc_Absyn_VarType_struct*
_tmpBF0=(struct Cyc_Absyn_VarType_struct*)_tmpBEF;if(_tmpBF0->tag != 2)goto _LL5E7;
else{_tmpBF1=_tmpBF0->f1;}};_LL5E6: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpBF1);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpBF1,1);continue;_LL5E7:;_LL5E8: goto _LL5E4;_LL5E4:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);Cyc_Tcutil_check_no_qual(loc,t);};}if(_tmpB27 != 0){const
char*_tmp1487;void*_tmp1486[2];struct Cyc_String_pa_struct _tmp1485;struct Cyc_String_pa_struct
_tmp1484;(_tmp1484.tag=0,((_tmp1484.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBCA->name)),((_tmp1485.tag=0,((
_tmp1485.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBC9->name)),((_tmp1486[0]=& _tmp1485,((_tmp1486[1]=& _tmp1484,Cyc_Tcutil_terr(
loc,((_tmp1487="too many type arguments for datatype %s.%s",_tag_dyneither(
_tmp1487,sizeof(char),43))),_tag_dyneither(_tmp1486,sizeof(void*),2)))))))))))));}
if(tvs != 0){const char*_tmp148C;void*_tmp148B[2];struct Cyc_String_pa_struct
_tmp148A;struct Cyc_String_pa_struct _tmp1489;(_tmp1489.tag=0,((_tmp1489.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBCA->name)),((
_tmp148A.tag=0,((_tmp148A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBC9->name)),((_tmp148B[0]=& _tmp148A,((_tmp148B[1]=& _tmp1489,Cyc_Tcutil_terr(
loc,((_tmp148C="too few type arguments for datatype %s.%s",_tag_dyneither(
_tmp148C,sizeof(char),42))),_tag_dyneither(_tmp148B,sizeof(void*),2)))))))))))));}
goto _LL5D0;}_LL5D0:;}goto _LL574;_LL585: {struct Cyc_Absyn_PointerType_struct*
_tmpB28=(struct Cyc_Absyn_PointerType_struct*)_tmpB08;if(_tmpB28->tag != 5)goto
_LL587;else{_tmpB29=_tmpB28->f1;_tmpB2A=_tmpB29.elt_typ;_tmpB2B=_tmpB29.elt_tq;
_tmpB2C=(struct Cyc_Absyn_Tqual*)&(_tmpB28->f1).elt_tq;_tmpB2D=_tmpB29.ptr_atts;
_tmpB2E=_tmpB2D.rgn;_tmpB2F=_tmpB2D.nullable;_tmpB30=_tmpB2D.bounds;_tmpB31=
_tmpB2D.zero_term;}}_LL586: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB2A,1);_tmpB2C->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpB2C->print_const,_tmpB2A);{struct Cyc_Absyn_Kind*k;switch(expected_kind->aliasqual){
case Cyc_Absyn_Aliasable: _LL5E9: k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique:
_LL5EA: k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL5EB: k=& Cyc_Tcutil_trk;break;}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmpB2E,1);{union Cyc_Absyn_Constraint*
_tmpBFA=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
_tmpB31);union Cyc_Absyn_Constraint _tmpBFB;int _tmpBFC;union Cyc_Absyn_Constraint
_tmpBFD;int _tmpBFE;_LL5EE: _tmpBFB=*_tmpBFA;if((_tmpBFB.No_constr).tag != 3)goto
_LL5F0;_tmpBFC=(int)(_tmpBFB.No_constr).val;_LL5EF:{void*_tmpBFF=Cyc_Tcutil_compress(
_tmpB2A);enum Cyc_Absyn_Size_of _tmpC01;_LL5F5: {struct Cyc_Absyn_IntType_struct*
_tmpC00=(struct Cyc_Absyn_IntType_struct*)_tmpBFF;if(_tmpC00->tag != 6)goto _LL5F7;
else{_tmpC01=_tmpC00->f2;if(_tmpC01 != Cyc_Absyn_Char_sz)goto _LL5F7;}}_LL5F6:((
int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB31,Cyc_Absyn_true_conref);
is_zero_terminated=1;goto _LL5F4;_LL5F7:;_LL5F8:((int(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmpB31,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL5F4;
_LL5F4:;}goto _LL5ED;_LL5F0: _tmpBFD=*_tmpBFA;if((_tmpBFD.Eq_constr).tag != 1)goto
_LL5F2;_tmpBFE=(int)(_tmpBFD.Eq_constr).val;if(_tmpBFE != 1)goto _LL5F2;_LL5F1: if(
!Cyc_Tcutil_admits_zero(_tmpB2A)){const char*_tmp1490;void*_tmp148F[1];struct Cyc_String_pa_struct
_tmp148E;(_tmp148E.tag=0,((_tmp148E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB2A)),((_tmp148F[0]=& _tmp148E,Cyc_Tcutil_terr(
loc,((_tmp1490="cannot have a pointer to zero-terminated %s elements",
_tag_dyneither(_tmp1490,sizeof(char),53))),_tag_dyneither(_tmp148F,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL5ED;_LL5F2:;_LL5F3: is_zero_terminated=0;
goto _LL5ED;_LL5ED:;}{void*_tmpC05=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpB30);struct Cyc_Absyn_Exp*
_tmpC08;_LL5FA: {struct Cyc_Absyn_DynEither_b_struct*_tmpC06=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC05;if(_tmpC06->tag != 0)goto _LL5FC;}_LL5FB: goto _LL5F9;_LL5FC: {struct Cyc_Absyn_Upper_b_struct*
_tmpC07=(struct Cyc_Absyn_Upper_b_struct*)_tmpC05;if(_tmpC07->tag != 1)goto _LL5F9;
else{_tmpC08=_tmpC07->f1;}}_LL5FD: {struct _RegionHandle*_tmpC09=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpC0A=Cyc_Tcenv_allow_valueof(_tmpC09,te);Cyc_Tcexp_tcExp(
_tmpC0A,0,_tmpC08);}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC08,te,
cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpC08)){const char*_tmp1493;void*
_tmp1492;(_tmp1492=0,Cyc_Tcutil_terr(loc,((_tmp1493="pointer bounds expression is not an unsigned int",
_tag_dyneither(_tmp1493,sizeof(char),49))),_tag_dyneither(_tmp1492,sizeof(void*),
0)));}{unsigned int _tmpC0E;int _tmpC0F;struct _tuple13 _tmpC0D=Cyc_Evexp_eval_const_uint_exp(
_tmpC08);_tmpC0E=_tmpC0D.f1;_tmpC0F=_tmpC0D.f2;if(is_zero_terminated  && (!
_tmpC0F  || _tmpC0E < 1)){const char*_tmp1496;void*_tmp1495;(_tmp1495=0,Cyc_Tcutil_terr(
loc,((_tmp1496="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmp1496,sizeof(char),55))),_tag_dyneither(_tmp1495,sizeof(void*),
0)));}goto _LL5F9;};}_LL5F9:;}goto _LL574;};}_LL587: {struct Cyc_Absyn_TagType_struct*
_tmpB32=(struct Cyc_Absyn_TagType_struct*)_tmpB08;if(_tmpB32->tag != 20)goto _LL589;
else{_tmpB33=(void*)_tmpB32->f1;}}_LL588: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpB33,1);goto _LL574;_LL589: {struct Cyc_Absyn_ValueofType_struct*
_tmpB34=(struct Cyc_Absyn_ValueofType_struct*)_tmpB08;if(_tmpB34->tag != 19)goto
_LL58B;else{_tmpB35=_tmpB34->f1;}}_LL58A: {struct _RegionHandle*_tmpC12=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpC13=Cyc_Tcenv_allow_valueof(_tmpC12,te);Cyc_Tcexp_tcExp(
_tmpC13,0,_tmpB35);}if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpB35)){const char*
_tmp1499;void*_tmp1498;(_tmp1498=0,Cyc_Tcutil_terr(loc,((_tmp1499="valueof_t requires an int expression",
_tag_dyneither(_tmp1499,sizeof(char),37))),_tag_dyneither(_tmp1498,sizeof(void*),
0)));}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB35,te,cvtenv);goto
_LL574;}_LL58B: {struct Cyc_Absyn_IntType_struct*_tmpB36=(struct Cyc_Absyn_IntType_struct*)
_tmpB08;if(_tmpB36->tag != 6)goto _LL58D;}_LL58C: goto _LL58E;_LL58D: {struct Cyc_Absyn_FloatType_struct*
_tmpB37=(struct Cyc_Absyn_FloatType_struct*)_tmpB08;if(_tmpB37->tag != 7)goto
_LL58F;}_LL58E: goto _LL590;_LL58F: {struct Cyc_Absyn_DoubleType_struct*_tmpB38=(
struct Cyc_Absyn_DoubleType_struct*)_tmpB08;if(_tmpB38->tag != 8)goto _LL591;}
_LL590: goto _LL574;_LL591: {struct Cyc_Absyn_ArrayType_struct*_tmpB39=(struct Cyc_Absyn_ArrayType_struct*)
_tmpB08;if(_tmpB39->tag != 9)goto _LL593;else{_tmpB3A=_tmpB39->f1;_tmpB3B=_tmpB3A.elt_type;
_tmpB3C=_tmpB3A.tq;_tmpB3D=(struct Cyc_Absyn_Tqual*)&(_tmpB39->f1).tq;_tmpB3E=
_tmpB3A.num_elts;_tmpB3F=(struct Cyc_Absyn_Exp**)&(_tmpB39->f1).num_elts;_tmpB40=
_tmpB3A.zero_term;_tmpB41=_tmpB3A.zt_loc;}}_LL592: {struct Cyc_Absyn_Exp*_tmpC16=*
_tmpB3F;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,
_tmpB3B,1);_tmpB3D->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB3D->print_const,
_tmpB3B);{int is_zero_terminated;{union Cyc_Absyn_Constraint*_tmpC17=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmpB40);union Cyc_Absyn_Constraint
_tmpC18;int _tmpC19;union Cyc_Absyn_Constraint _tmpC1A;int _tmpC1B;_LL5FF: _tmpC18=*
_tmpC17;if((_tmpC18.No_constr).tag != 3)goto _LL601;_tmpC19=(int)(_tmpC18.No_constr).val;
_LL600:((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB40,Cyc_Absyn_false_conref);
is_zero_terminated=0;goto _LL5FE;_LL601: _tmpC1A=*_tmpC17;if((_tmpC1A.Eq_constr).tag
!= 1)goto _LL603;_tmpC1B=(int)(_tmpC1A.Eq_constr).val;if(_tmpC1B != 1)goto _LL603;
_LL602: if(!Cyc_Tcutil_admits_zero(_tmpB3B)){const char*_tmp149D;void*_tmp149C[1];
struct Cyc_String_pa_struct _tmp149B;(_tmp149B.tag=0,((_tmp149B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB3B)),((
_tmp149C[0]=& _tmp149B,Cyc_Tcutil_terr(loc,((_tmp149D="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmp149D,sizeof(char),51))),_tag_dyneither(_tmp149C,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL5FE;_LL603:;_LL604: is_zero_terminated=0;
goto _LL5FE;_LL5FE:;}if(_tmpC16 == 0){if(is_zero_terminated)*_tmpB3F=(_tmpC16=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{{const char*_tmp14A0;void*
_tmp149F;(_tmp149F=0,Cyc_Tcutil_warn(loc,((_tmp14A0="array bound defaults to 1 here",
_tag_dyneither(_tmp14A0,sizeof(char),31))),_tag_dyneither(_tmp149F,sizeof(void*),
0)));}*_tmpB3F=(_tmpC16=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmpC16);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_tmpC16)){const char*_tmp14A3;void*_tmp14A2;(_tmp14A2=0,Cyc_Tcutil_terr(loc,((
_tmp14A3="array bounds expression is not constant",_tag_dyneither(_tmp14A3,
sizeof(char),40))),_tag_dyneither(_tmp14A2,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_uint_typ(
te,(struct Cyc_Absyn_Exp*)_tmpC16)){const char*_tmp14A6;void*_tmp14A5;(_tmp14A5=0,
Cyc_Tcutil_terr(loc,((_tmp14A6="array bounds expression is not an unsigned int",
_tag_dyneither(_tmp14A6,sizeof(char),47))),_tag_dyneither(_tmp14A5,sizeof(void*),
0)));}{unsigned int _tmpC26;int _tmpC27;struct _tuple13 _tmpC25=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmpC16);_tmpC26=_tmpC25.f1;_tmpC27=_tmpC25.f2;if((
is_zero_terminated  && _tmpC27) && _tmpC26 < 1){const char*_tmp14A9;void*_tmp14A8;(
_tmp14A8=0,Cyc_Tcutil_warn(loc,((_tmp14A9="zero terminated array cannot have zero elements",
_tag_dyneither(_tmp14A9,sizeof(char),48))),_tag_dyneither(_tmp14A8,sizeof(void*),
0)));}if((_tmpC27  && _tmpC26 < 1) && Cyc_Cyclone_tovc_r){{const char*_tmp14AC;void*
_tmp14AB;(_tmp14AB=0,Cyc_Tcutil_warn(loc,((_tmp14AC="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmp14AC,sizeof(char),75))),_tag_dyneither(_tmp14AB,sizeof(void*),
0)));}*_tmpB3F=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL574;};};}
_LL593: {struct Cyc_Absyn_FnType_struct*_tmpB42=(struct Cyc_Absyn_FnType_struct*)
_tmpB08;if(_tmpB42->tag != 10)goto _LL595;else{_tmpB43=_tmpB42->f1;_tmpB44=_tmpB43.tvars;
_tmpB45=(struct Cyc_List_List**)&(_tmpB42->f1).tvars;_tmpB46=_tmpB43.effect;
_tmpB47=(struct Cyc_Core_Opt**)&(_tmpB42->f1).effect;_tmpB48=_tmpB43.ret_tqual;
_tmpB49=(struct Cyc_Absyn_Tqual*)&(_tmpB42->f1).ret_tqual;_tmpB4A=_tmpB43.ret_typ;
_tmpB4B=_tmpB43.args;_tmpB4C=_tmpB43.c_varargs;_tmpB4D=_tmpB43.cyc_varargs;
_tmpB4E=_tmpB43.rgn_po;_tmpB4F=_tmpB43.attributes;}}_LL594: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;enum Cyc_Absyn_Format_Type
ft=Cyc_Absyn_Printf_ft;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmpB4F != 0;
_tmpB4F=_tmpB4F->tl){if(!Cyc_Absyn_fntype_att((void*)_tmpB4F->hd)){const char*
_tmp14B0;void*_tmp14AF[1];struct Cyc_String_pa_struct _tmp14AE;(_tmp14AE.tag=0,((
_tmp14AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmpB4F->hd)),((_tmp14AF[0]=& _tmp14AE,Cyc_Tcutil_terr(loc,((_tmp14B0="bad function type attribute %s",
_tag_dyneither(_tmp14B0,sizeof(char),31))),_tag_dyneither(_tmp14AF,sizeof(void*),
1)))))));}{void*_tmpC2F=(void*)_tmpB4F->hd;enum Cyc_Absyn_Format_Type _tmpC34;int
_tmpC35;int _tmpC36;_LL606: {struct Cyc_Absyn_Stdcall_att_struct*_tmpC30=(struct
Cyc_Absyn_Stdcall_att_struct*)_tmpC2F;if(_tmpC30->tag != 1)goto _LL608;}_LL607: if(
!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL605;_LL608: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpC31=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpC2F;if(_tmpC31->tag != 2)goto
_LL60A;}_LL609: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL605;_LL60A: {
struct Cyc_Absyn_Fastcall_att_struct*_tmpC32=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpC2F;if(_tmpC32->tag != 3)goto _LL60C;}_LL60B: if(!seen_fastcall){seen_fastcall=
1;++ num_convs;}goto _LL605;_LL60C: {struct Cyc_Absyn_Format_att_struct*_tmpC33=(
struct Cyc_Absyn_Format_att_struct*)_tmpC2F;if(_tmpC33->tag != 19)goto _LL60E;else{
_tmpC34=_tmpC33->f1;_tmpC35=_tmpC33->f2;_tmpC36=_tmpC33->f3;}}_LL60D: if(!
seen_format){seen_format=1;ft=_tmpC34;fmt_desc_arg=_tmpC35;fmt_arg_start=_tmpC36;}
else{const char*_tmp14B3;void*_tmp14B2;(_tmp14B2=0,Cyc_Tcutil_terr(loc,((_tmp14B3="function can't have multiple format attributes",
_tag_dyneither(_tmp14B3,sizeof(char),47))),_tag_dyneither(_tmp14B2,sizeof(void*),
0)));}goto _LL605;_LL60E:;_LL60F: goto _LL605;_LL605:;};}if(num_convs > 1){const char*
_tmp14B6;void*_tmp14B5;(_tmp14B5=0,Cyc_Tcutil_terr(loc,((_tmp14B6="function can't have multiple calling conventions",
_tag_dyneither(_tmp14B6,sizeof(char),49))),_tag_dyneither(_tmp14B5,sizeof(void*),
0)));}Cyc_Tcutil_check_unique_tvars(loc,*_tmpB45);{struct Cyc_List_List*b=*
_tmpB45;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmpC3B=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);
struct Cyc_Absyn_Kind*_tmpC3D;struct Cyc_Absyn_Kind _tmpC3E;enum Cyc_Absyn_KindQual
_tmpC3F;_LL611: {struct Cyc_Absyn_Eq_kb_struct*_tmpC3C=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpC3B;if(_tmpC3C->tag != 0)goto _LL613;else{_tmpC3D=_tmpC3C->f1;_tmpC3E=*_tmpC3D;
_tmpC3F=_tmpC3E.kind;if(_tmpC3F != Cyc_Absyn_MemKind)goto _LL613;}}_LL612:{const
char*_tmp14BA;void*_tmp14B9[1];struct Cyc_String_pa_struct _tmp14B8;(_tmp14B8.tag=
0,((_tmp14B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name),((_tmp14B9[0]=& _tmp14B8,Cyc_Tcutil_terr(loc,((_tmp14BA="function attempts to abstract Mem type variable %s",
_tag_dyneither(_tmp14BA,sizeof(char),51))),_tag_dyneither(_tmp14B9,sizeof(void*),
1)))))));}goto _LL610;_LL613:;_LL614: goto _LL610;_LL610:;};}}{struct _RegionHandle*
_tmpC43=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcutil_CVTEnv _tmp14BB;struct Cyc_Tcutil_CVTEnv
_tmpC44=(_tmp14BB.r=_tmpC43,((_tmp14BB.kind_env=cvtenv.kind_env,((_tmp14BB.free_vars=
0,((_tmp14BB.free_evars=0,((_tmp14BB.generalize_evars=cvtenv.generalize_evars,((
_tmp14BB.fn_result=1,_tmp14BB)))))))))));_tmpC44=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC44,& Cyc_Tcutil_tmk,_tmpB4A,1);_tmpB49->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpB49->print_const,_tmpB4A);_tmpC44.fn_result=0;{struct Cyc_List_List*a=
_tmpB4B;for(0;a != 0;a=a->tl){struct _tuple9*_tmpC45=(struct _tuple9*)a->hd;void*
_tmpC46=(*_tmpC45).f3;_tmpC44=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC44,& Cyc_Tcutil_tmk,
_tmpC46,1);{int _tmpC47=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpC45).f2).print_const,
_tmpC46);((*_tmpC45).f2).real_const=_tmpC47;{void*_tmpC48=Cyc_Tcutil_compress(
_tmpC46);struct Cyc_Absyn_ArrayInfo _tmpC4A;void*_tmpC4B;struct Cyc_Absyn_Tqual
_tmpC4C;struct Cyc_Absyn_Exp*_tmpC4D;union Cyc_Absyn_Constraint*_tmpC4E;struct Cyc_Position_Segment*
_tmpC4F;_LL616: {struct Cyc_Absyn_ArrayType_struct*_tmpC49=(struct Cyc_Absyn_ArrayType_struct*)
_tmpC48;if(_tmpC49->tag != 9)goto _LL618;else{_tmpC4A=_tmpC49->f1;_tmpC4B=_tmpC4A.elt_type;
_tmpC4C=_tmpC4A.tq;_tmpC4D=_tmpC4A.num_elts;_tmpC4E=_tmpC4A.zero_term;_tmpC4F=
_tmpC4A.zt_loc;}}_LL617: {void*_tmpC50=Cyc_Absyn_new_evar(0,0);_tmpC44=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC44,& Cyc_Tcutil_rk,_tmpC50,1);{struct Cyc_Absyn_Upper_b_struct _tmp14BE;
struct Cyc_Absyn_Upper_b_struct*_tmp14BD;void*_tmpC51=Cyc_Absyn_atb_typ(_tmpC4B,
_tmpC50,_tmpC4C,(void*)((_tmp14BD=_cycalloc(sizeof(*_tmp14BD)),((_tmp14BD[0]=((
_tmp14BE.tag=1,((_tmp14BE.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpC4D),_tmp14BE)))),
_tmp14BD)))),_tmpC4E);(*_tmpC45).f3=_tmpC51;goto _LL615;};}_LL618:;_LL619: goto
_LL615;_LL615:;};};}}if(_tmpB4D != 0){if(_tmpB4C){const char*_tmp14C1;void*
_tmp14C0;(_tmp14C0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp14C1="both c_vararg and cyc_vararg",_tag_dyneither(
_tmp14C1,sizeof(char),29))),_tag_dyneither(_tmp14C0,sizeof(void*),0)));}{void*
_tmpC57;int _tmpC58;struct Cyc_Absyn_VarargInfo _tmpC56=*_tmpB4D;_tmpC57=_tmpC56.type;
_tmpC58=_tmpC56.inject;_tmpC44=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC44,& Cyc_Tcutil_tmk,
_tmpC57,1);(_tmpB4D->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(
_tmpB4D->tq).print_const,_tmpC57);if(_tmpC58){void*_tmpC59=Cyc_Tcutil_compress(
_tmpC57);struct Cyc_Absyn_PtrInfo _tmpC5B;void*_tmpC5C;struct Cyc_Absyn_PtrAtts
_tmpC5D;union Cyc_Absyn_Constraint*_tmpC5E;union Cyc_Absyn_Constraint*_tmpC5F;
_LL61B: {struct Cyc_Absyn_PointerType_struct*_tmpC5A=(struct Cyc_Absyn_PointerType_struct*)
_tmpC59;if(_tmpC5A->tag != 5)goto _LL61D;else{_tmpC5B=_tmpC5A->f1;_tmpC5C=_tmpC5B.elt_typ;
_tmpC5D=_tmpC5B.ptr_atts;_tmpC5E=_tmpC5D.bounds;_tmpC5F=_tmpC5D.zero_term;}}
_LL61C:{void*_tmpC60=Cyc_Tcutil_compress(_tmpC5C);_LL620: {struct Cyc_Absyn_DatatypeType_struct*
_tmpC61=(struct Cyc_Absyn_DatatypeType_struct*)_tmpC60;if(_tmpC61->tag != 3)goto
_LL622;}_LL621: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,_tmpC5F)){const char*_tmp14C4;void*_tmp14C3;(_tmp14C3=0,Cyc_Tcutil_terr(loc,((
_tmp14C4="can't inject into a zeroterm pointer",_tag_dyneither(_tmp14C4,sizeof(
char),37))),_tag_dyneither(_tmp14C3,sizeof(void*),0)));}{void*_tmpC64=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,
_tmpC5E);_LL625: {struct Cyc_Absyn_DynEither_b_struct*_tmpC65=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC64;if(_tmpC65->tag != 0)goto _LL627;}_LL626:{const char*_tmp14C7;void*_tmp14C6;(
_tmp14C6=0,Cyc_Tcutil_terr(loc,((_tmp14C7="can't inject into a fat pointer to datatype",
_tag_dyneither(_tmp14C7,sizeof(char),44))),_tag_dyneither(_tmp14C6,sizeof(void*),
0)));}goto _LL624;_LL627:;_LL628: goto _LL624;_LL624:;}goto _LL61F;_LL622:;_LL623:{
const char*_tmp14CA;void*_tmp14C9;(_tmp14C9=0,Cyc_Tcutil_terr(loc,((_tmp14CA="can't inject a non-datatype type",
_tag_dyneither(_tmp14CA,sizeof(char),33))),_tag_dyneither(_tmp14C9,sizeof(void*),
0)));}goto _LL61F;_LL61F:;}goto _LL61A;_LL61D:;_LL61E:{const char*_tmp14CD;void*
_tmp14CC;(_tmp14CC=0,Cyc_Tcutil_terr(loc,((_tmp14CD="expecting a datatype pointer type",
_tag_dyneither(_tmp14CD,sizeof(char),34))),_tag_dyneither(_tmp14CC,sizeof(void*),
0)));}goto _LL61A;_LL61A:;}};}if(seen_format){int _tmpC6C=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmpB4B);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpC6C) || 
fmt_arg_start < 0) || _tmpB4D == 0  && fmt_arg_start != 0) || _tmpB4D != 0  && 
fmt_arg_start != _tmpC6C + 1){const char*_tmp14D0;void*_tmp14CF;(_tmp14CF=0,Cyc_Tcutil_terr(
loc,((_tmp14D0="bad format descriptor",_tag_dyneither(_tmp14D0,sizeof(char),22))),
_tag_dyneither(_tmp14CF,sizeof(void*),0)));}else{void*_tmpC70;struct _tuple9
_tmpC6F=*((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpB4B,
fmt_desc_arg - 1);_tmpC70=_tmpC6F.f3;{void*_tmpC71=Cyc_Tcutil_compress(_tmpC70);
struct Cyc_Absyn_PtrInfo _tmpC73;void*_tmpC74;struct Cyc_Absyn_PtrAtts _tmpC75;union
Cyc_Absyn_Constraint*_tmpC76;union Cyc_Absyn_Constraint*_tmpC77;_LL62A: {struct
Cyc_Absyn_PointerType_struct*_tmpC72=(struct Cyc_Absyn_PointerType_struct*)
_tmpC71;if(_tmpC72->tag != 5)goto _LL62C;else{_tmpC73=_tmpC72->f1;_tmpC74=_tmpC73.elt_typ;
_tmpC75=_tmpC73.ptr_atts;_tmpC76=_tmpC75.bounds;_tmpC77=_tmpC75.zero_term;}}
_LL62B:{struct _tuple0 _tmp14D1;struct _tuple0 _tmpC79=(_tmp14D1.f1=Cyc_Tcutil_compress(
_tmpC74),((_tmp14D1.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmpC76),_tmp14D1)));void*_tmpC7A;enum Cyc_Absyn_Sign
_tmpC7C;enum Cyc_Absyn_Size_of _tmpC7D;void*_tmpC7E;_LL62F: _tmpC7A=_tmpC79.f1;{
struct Cyc_Absyn_IntType_struct*_tmpC7B=(struct Cyc_Absyn_IntType_struct*)_tmpC7A;
if(_tmpC7B->tag != 6)goto _LL631;else{_tmpC7C=_tmpC7B->f1;if(_tmpC7C != Cyc_Absyn_None)
goto _LL631;_tmpC7D=_tmpC7B->f2;if(_tmpC7D != Cyc_Absyn_Char_sz)goto _LL631;}};
_tmpC7E=_tmpC79.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmpC7F=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC7E;if(_tmpC7F->tag != 0)goto _LL631;};_LL630: goto _LL62E;_LL631:;_LL632:{const
char*_tmp14D4;void*_tmp14D3;(_tmp14D3=0,Cyc_Tcutil_terr(loc,((_tmp14D4="format descriptor is not a char ? type",
_tag_dyneither(_tmp14D4,sizeof(char),39))),_tag_dyneither(_tmp14D3,sizeof(void*),
0)));}goto _LL62E;_LL62E:;}goto _LL629;_LL62C:;_LL62D:{const char*_tmp14D7;void*
_tmp14D6;(_tmp14D6=0,Cyc_Tcutil_terr(loc,((_tmp14D7="format descriptor is not a char ? type",
_tag_dyneither(_tmp14D7,sizeof(char),39))),_tag_dyneither(_tmp14D6,sizeof(void*),
0)));}goto _LL629;_LL629:;}if(fmt_arg_start != 0){int problem;{struct _tuple31
_tmp14D8;struct _tuple31 _tmpC85=(_tmp14D8.f1=ft,((_tmp14D8.f2=Cyc_Tcutil_compress(
Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmpB4D))->type)),
_tmp14D8)));enum Cyc_Absyn_Format_Type _tmpC86;void*_tmpC87;struct Cyc_Absyn_DatatypeInfo
_tmpC89;union Cyc_Absyn_DatatypeInfoU _tmpC8A;struct Cyc_Absyn_Datatypedecl**
_tmpC8B;struct Cyc_Absyn_Datatypedecl*_tmpC8C;enum Cyc_Absyn_Format_Type _tmpC8D;
void*_tmpC8E;struct Cyc_Absyn_DatatypeInfo _tmpC90;union Cyc_Absyn_DatatypeInfoU
_tmpC91;struct Cyc_Absyn_Datatypedecl**_tmpC92;struct Cyc_Absyn_Datatypedecl*
_tmpC93;_LL634: _tmpC86=_tmpC85.f1;if(_tmpC86 != Cyc_Absyn_Printf_ft)goto _LL636;
_tmpC87=_tmpC85.f2;{struct Cyc_Absyn_DatatypeType_struct*_tmpC88=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpC87;if(_tmpC88->tag != 3)goto _LL636;else{_tmpC89=_tmpC88->f1;_tmpC8A=_tmpC89.datatype_info;
if((_tmpC8A.KnownDatatype).tag != 2)goto _LL636;_tmpC8B=(struct Cyc_Absyn_Datatypedecl**)(
_tmpC8A.KnownDatatype).val;_tmpC8C=*_tmpC8B;}};_LL635: problem=Cyc_Absyn_qvar_cmp(
_tmpC8C->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL633;_LL636: _tmpC8D=
_tmpC85.f1;if(_tmpC8D != Cyc_Absyn_Scanf_ft)goto _LL638;_tmpC8E=_tmpC85.f2;{struct
Cyc_Absyn_DatatypeType_struct*_tmpC8F=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpC8E;if(_tmpC8F->tag != 3)goto _LL638;else{_tmpC90=_tmpC8F->f1;_tmpC91=_tmpC90.datatype_info;
if((_tmpC91.KnownDatatype).tag != 2)goto _LL638;_tmpC92=(struct Cyc_Absyn_Datatypedecl**)(
_tmpC91.KnownDatatype).val;_tmpC93=*_tmpC92;}};_LL637: problem=Cyc_Absyn_qvar_cmp(
_tmpC93->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL633;_LL638:;_LL639:
problem=1;goto _LL633;_LL633:;}if(problem){const char*_tmp14DB;void*_tmp14DA;(
_tmp14DA=0,Cyc_Tcutil_terr(loc,((_tmp14DB="format attribute and vararg types don't match",
_tag_dyneither(_tmp14DB,sizeof(char),46))),_tag_dyneither(_tmp14DA,sizeof(void*),
0)));}}}}{struct Cyc_List_List*rpo=_tmpB4E;for(0;rpo != 0;rpo=rpo->tl){struct
_tuple0 _tmpC97;void*_tmpC98;void*_tmpC99;struct _tuple0*_tmpC96=(struct _tuple0*)
rpo->hd;_tmpC97=*_tmpC96;_tmpC98=_tmpC97.f1;_tmpC99=_tmpC97.f2;_tmpC44=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC44,& Cyc_Tcutil_ek,_tmpC98,1);_tmpC44=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC44,& Cyc_Tcutil_trk,_tmpC99,1);}}if(*_tmpB47 != 0)_tmpC44=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC44,& Cyc_Tcutil_ek,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmpB47))->v,
1);else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmpC44.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpC9B;int _tmpC9C;struct
_tuple29 _tmpC9A=*((struct _tuple29*)tvs->hd);_tmpC9B=_tmpC9A.f1;_tmpC9C=_tmpC9A.f2;
if(!_tmpC9C)continue;{void*_tmpC9D=Cyc_Absyn_compress_kb(_tmpC9B->kind);struct
Cyc_Core_Opt*_tmpC9F;struct Cyc_Core_Opt**_tmpCA0;struct Cyc_Absyn_Kind*_tmpCA1;
struct Cyc_Absyn_Kind*_tmpCA3;struct Cyc_Absyn_Kind*_tmpCA5;struct Cyc_Absyn_Kind
_tmpCA6;enum Cyc_Absyn_KindQual _tmpCA7;struct Cyc_Absyn_Kind*_tmpCA9;struct Cyc_Absyn_Kind
_tmpCAA;enum Cyc_Absyn_KindQual _tmpCAB;struct Cyc_Core_Opt*_tmpCAD;struct Cyc_Core_Opt**
_tmpCAE;struct Cyc_Absyn_Kind*_tmpCAF;struct Cyc_Absyn_Kind _tmpCB0;enum Cyc_Absyn_KindQual
_tmpCB1;struct Cyc_Absyn_Kind*_tmpCB3;struct Cyc_Absyn_Kind _tmpCB4;enum Cyc_Absyn_KindQual
_tmpCB5;struct Cyc_Core_Opt*_tmpCB7;struct Cyc_Core_Opt**_tmpCB8;_LL63B:{struct Cyc_Absyn_Less_kb_struct*
_tmpC9E=(struct Cyc_Absyn_Less_kb_struct*)_tmpC9D;if(_tmpC9E->tag != 2)goto _LL63D;
else{_tmpC9F=_tmpC9E->f1;_tmpCA0=(struct Cyc_Core_Opt**)& _tmpC9E->f1;_tmpCA1=
_tmpC9E->f2;}}if(!(_tmpCA1->kind == Cyc_Absyn_RgnKind))goto _LL63D;_LL63C: if(
_tmpCA1->aliasqual == Cyc_Absyn_Top){*_tmpCA0=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
_tmpCA3=_tmpCA1;goto _LL63E;}*_tmpCA0=Cyc_Tcutil_kind_to_bound_opt(_tmpCA1);
_tmpCA3=_tmpCA1;goto _LL63E;_LL63D:{struct Cyc_Absyn_Eq_kb_struct*_tmpCA2=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpC9D;if(_tmpCA2->tag != 0)goto _LL63F;else{_tmpCA3=
_tmpCA2->f1;}}if(!(_tmpCA3->kind == Cyc_Absyn_RgnKind))goto _LL63F;_LL63E:{struct
Cyc_Absyn_AccessEff_struct*_tmp14EA;struct Cyc_Absyn_VarType_struct*_tmp14E9;
struct Cyc_Absyn_VarType_struct _tmp14E8;struct Cyc_Absyn_AccessEff_struct _tmp14E7;
struct Cyc_List_List*_tmp14E6;effect=((_tmp14E6=_cycalloc(sizeof(*_tmp14E6)),((
_tmp14E6->hd=(void*)((_tmp14EA=_cycalloc(sizeof(*_tmp14EA)),((_tmp14EA[0]=((
_tmp14E7.tag=23,((_tmp14E7.f1=(void*)((void*)((_tmp14E9=_cycalloc(sizeof(*
_tmp14E9)),((_tmp14E9[0]=((_tmp14E8.tag=2,((_tmp14E8.f1=_tmpC9B,_tmp14E8)))),
_tmp14E9))))),_tmp14E7)))),_tmp14EA)))),((_tmp14E6->tl=effect,_tmp14E6))))));}
goto _LL63A;_LL63F: {struct Cyc_Absyn_Less_kb_struct*_tmpCA4=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC9D;if(_tmpCA4->tag != 2)goto _LL641;else{_tmpCA5=_tmpCA4->f2;_tmpCA6=*_tmpCA5;
_tmpCA7=_tmpCA6.kind;if(_tmpCA7 != Cyc_Absyn_IntKind)goto _LL641;}}_LL640: goto
_LL642;_LL641: {struct Cyc_Absyn_Eq_kb_struct*_tmpCA8=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpC9D;if(_tmpCA8->tag != 0)goto _LL643;else{_tmpCA9=_tmpCA8->f1;_tmpCAA=*_tmpCA9;
_tmpCAB=_tmpCAA.kind;if(_tmpCAB != Cyc_Absyn_IntKind)goto _LL643;}}_LL642: goto
_LL63A;_LL643: {struct Cyc_Absyn_Less_kb_struct*_tmpCAC=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC9D;if(_tmpCAC->tag != 2)goto _LL645;else{_tmpCAD=_tmpCAC->f1;_tmpCAE=(struct
Cyc_Core_Opt**)& _tmpCAC->f1;_tmpCAF=_tmpCAC->f2;_tmpCB0=*_tmpCAF;_tmpCB1=_tmpCB0.kind;
if(_tmpCB1 != Cyc_Absyn_EffKind)goto _LL645;}}_LL644:*_tmpCAE=Cyc_Tcutil_kind_to_bound_opt(&
Cyc_Tcutil_ek);goto _LL646;_LL645: {struct Cyc_Absyn_Eq_kb_struct*_tmpCB2=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpC9D;if(_tmpCB2->tag != 0)goto _LL647;else{_tmpCB3=
_tmpCB2->f1;_tmpCB4=*_tmpCB3;_tmpCB5=_tmpCB4.kind;if(_tmpCB5 != Cyc_Absyn_EffKind)
goto _LL647;}}_LL646:{struct Cyc_Absyn_VarType_struct*_tmp14F0;struct Cyc_Absyn_VarType_struct
_tmp14EF;struct Cyc_List_List*_tmp14EE;effect=((_tmp14EE=_cycalloc(sizeof(*
_tmp14EE)),((_tmp14EE->hd=(void*)((_tmp14F0=_cycalloc(sizeof(*_tmp14F0)),((
_tmp14F0[0]=((_tmp14EF.tag=2,((_tmp14EF.f1=_tmpC9B,_tmp14EF)))),_tmp14F0)))),((
_tmp14EE->tl=effect,_tmp14EE))))));}goto _LL63A;_LL647: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpCB6=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpC9D;if(_tmpCB6->tag != 1)goto
_LL649;else{_tmpCB7=_tmpCB6->f1;_tmpCB8=(struct Cyc_Core_Opt**)& _tmpCB6->f1;}}
_LL648:{struct Cyc_Absyn_Less_kb_struct*_tmp14F6;struct Cyc_Absyn_Less_kb_struct
_tmp14F5;struct Cyc_Core_Opt*_tmp14F4;*_tmpCB8=((_tmp14F4=_cycalloc(sizeof(*
_tmp14F4)),((_tmp14F4->v=(void*)((_tmp14F6=_cycalloc(sizeof(*_tmp14F6)),((
_tmp14F6[0]=((_tmp14F5.tag=2,((_tmp14F5.f1=0,((_tmp14F5.f2=& Cyc_Tcutil_ak,
_tmp14F5)))))),_tmp14F6)))),_tmp14F4))));}goto _LL64A;_LL649:;_LL64A:{struct Cyc_Absyn_RgnsEff_struct*
_tmp1505;struct Cyc_Absyn_VarType_struct*_tmp1504;struct Cyc_Absyn_VarType_struct
_tmp1503;struct Cyc_Absyn_RgnsEff_struct _tmp1502;struct Cyc_List_List*_tmp1501;
effect=((_tmp1501=_cycalloc(sizeof(*_tmp1501)),((_tmp1501->hd=(void*)((_tmp1505=
_cycalloc(sizeof(*_tmp1505)),((_tmp1505[0]=((_tmp1502.tag=25,((_tmp1502.f1=(void*)((
void*)((_tmp1504=_cycalloc(sizeof(*_tmp1504)),((_tmp1504[0]=((_tmp1503.tag=2,((
_tmp1503.f1=_tmpC9B,_tmp1503)))),_tmp1504))))),_tmp1502)))),_tmp1505)))),((
_tmp1501->tl=effect,_tmp1501))))));}goto _LL63A;_LL63A:;};}}{struct Cyc_List_List*
ts=_tmpC44.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmpCCA;int _tmpCCB;struct
_tuple30 _tmpCC9=*((struct _tuple30*)ts->hd);_tmpCCA=_tmpCC9.f1;_tmpCCB=_tmpCC9.f2;
if(!_tmpCCB)continue;{struct Cyc_Absyn_Kind*_tmpCCC=Cyc_Tcutil_typ_kind(_tmpCCA);
struct Cyc_Absyn_Kind _tmpCCD;enum Cyc_Absyn_KindQual _tmpCCE;struct Cyc_Absyn_Kind
_tmpCCF;enum Cyc_Absyn_KindQual _tmpCD0;struct Cyc_Absyn_Kind _tmpCD1;enum Cyc_Absyn_KindQual
_tmpCD2;_LL64C: _tmpCCD=*_tmpCCC;_tmpCCE=_tmpCCD.kind;if(_tmpCCE != Cyc_Absyn_RgnKind)
goto _LL64E;_LL64D:{struct Cyc_Absyn_AccessEff_struct*_tmp150B;struct Cyc_Absyn_AccessEff_struct
_tmp150A;struct Cyc_List_List*_tmp1509;effect=((_tmp1509=_cycalloc(sizeof(*
_tmp1509)),((_tmp1509->hd=(void*)((_tmp150B=_cycalloc(sizeof(*_tmp150B)),((
_tmp150B[0]=((_tmp150A.tag=23,((_tmp150A.f1=(void*)_tmpCCA,_tmp150A)))),_tmp150B)))),((
_tmp1509->tl=effect,_tmp1509))))));}goto _LL64B;_LL64E: _tmpCCF=*_tmpCCC;_tmpCD0=
_tmpCCF.kind;if(_tmpCD0 != Cyc_Absyn_EffKind)goto _LL650;_LL64F:{struct Cyc_List_List*
_tmp150C;effect=((_tmp150C=_cycalloc(sizeof(*_tmp150C)),((_tmp150C->hd=_tmpCCA,((
_tmp150C->tl=effect,_tmp150C))))));}goto _LL64B;_LL650: _tmpCD1=*_tmpCCC;_tmpCD2=
_tmpCD1.kind;if(_tmpCD2 != Cyc_Absyn_IntKind)goto _LL652;_LL651: goto _LL64B;_LL652:;
_LL653:{struct Cyc_Absyn_RgnsEff_struct*_tmp1512;struct Cyc_Absyn_RgnsEff_struct
_tmp1511;struct Cyc_List_List*_tmp1510;effect=((_tmp1510=_cycalloc(sizeof(*
_tmp1510)),((_tmp1510->hd=(void*)((_tmp1512=_cycalloc(sizeof(*_tmp1512)),((
_tmp1512[0]=((_tmp1511.tag=25,((_tmp1511.f1=(void*)_tmpCCA,_tmp1511)))),_tmp1512)))),((
_tmp1510->tl=effect,_tmp1510))))));}goto _LL64B;_LL64B:;};}}{struct Cyc_Absyn_JoinEff_struct*
_tmp1518;struct Cyc_Absyn_JoinEff_struct _tmp1517;struct Cyc_Core_Opt*_tmp1516;*
_tmpB47=((_tmp1516=_cycalloc(sizeof(*_tmp1516)),((_tmp1516->v=(void*)((_tmp1518=
_cycalloc(sizeof(*_tmp1518)),((_tmp1518[0]=((_tmp1517.tag=24,((_tmp1517.f1=
effect,_tmp1517)))),_tmp1518)))),_tmp1516))));};}if(*_tmpB45 != 0){struct Cyc_List_List*
bs=*_tmpB45;for(0;bs != 0;bs=bs->tl){void*_tmpCDD=Cyc_Absyn_compress_kb(((struct
Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmpCDF;struct Cyc_Core_Opt**
_tmpCE0;struct Cyc_Core_Opt*_tmpCE2;struct Cyc_Core_Opt**_tmpCE3;struct Cyc_Absyn_Kind*
_tmpCE4;struct Cyc_Absyn_Kind _tmpCE5;enum Cyc_Absyn_KindQual _tmpCE6;enum Cyc_Absyn_AliasQual
_tmpCE7;struct Cyc_Core_Opt*_tmpCE9;struct Cyc_Core_Opt**_tmpCEA;struct Cyc_Absyn_Kind*
_tmpCEB;struct Cyc_Absyn_Kind _tmpCEC;enum Cyc_Absyn_KindQual _tmpCED;enum Cyc_Absyn_AliasQual
_tmpCEE;struct Cyc_Core_Opt*_tmpCF0;struct Cyc_Core_Opt**_tmpCF1;struct Cyc_Absyn_Kind*
_tmpCF2;struct Cyc_Absyn_Kind _tmpCF3;enum Cyc_Absyn_KindQual _tmpCF4;enum Cyc_Absyn_AliasQual
_tmpCF5;struct Cyc_Core_Opt*_tmpCF7;struct Cyc_Core_Opt**_tmpCF8;struct Cyc_Absyn_Kind*
_tmpCF9;struct Cyc_Absyn_Kind _tmpCFA;enum Cyc_Absyn_KindQual _tmpCFB;enum Cyc_Absyn_AliasQual
_tmpCFC;struct Cyc_Core_Opt*_tmpCFE;struct Cyc_Core_Opt**_tmpCFF;struct Cyc_Absyn_Kind*
_tmpD00;struct Cyc_Absyn_Kind _tmpD01;enum Cyc_Absyn_KindQual _tmpD02;enum Cyc_Absyn_AliasQual
_tmpD03;struct Cyc_Core_Opt*_tmpD05;struct Cyc_Core_Opt**_tmpD06;struct Cyc_Absyn_Kind*
_tmpD07;struct Cyc_Absyn_Kind _tmpD08;enum Cyc_Absyn_KindQual _tmpD09;enum Cyc_Absyn_AliasQual
_tmpD0A;struct Cyc_Core_Opt*_tmpD0C;struct Cyc_Core_Opt**_tmpD0D;struct Cyc_Absyn_Kind*
_tmpD0E;struct Cyc_Absyn_Kind _tmpD0F;enum Cyc_Absyn_KindQual _tmpD10;enum Cyc_Absyn_AliasQual
_tmpD11;struct Cyc_Core_Opt*_tmpD13;struct Cyc_Core_Opt**_tmpD14;struct Cyc_Absyn_Kind*
_tmpD15;struct Cyc_Absyn_Kind _tmpD16;enum Cyc_Absyn_KindQual _tmpD17;enum Cyc_Absyn_AliasQual
_tmpD18;struct Cyc_Core_Opt*_tmpD1A;struct Cyc_Core_Opt**_tmpD1B;struct Cyc_Absyn_Kind*
_tmpD1C;struct Cyc_Absyn_Kind*_tmpD1E;struct Cyc_Absyn_Kind _tmpD1F;enum Cyc_Absyn_KindQual
_tmpD20;_LL655: {struct Cyc_Absyn_Unknown_kb_struct*_tmpCDE=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpCDD;if(_tmpCDE->tag != 1)goto _LL657;else{_tmpCDF=_tmpCDE->f1;_tmpCE0=(struct
Cyc_Core_Opt**)& _tmpCDE->f1;}}_LL656:{const char*_tmp151C;void*_tmp151B[1];struct
Cyc_String_pa_struct _tmp151A;(_tmp151A.tag=0,((_tmp151A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp151B[0]=&
_tmp151A,Cyc_Tcutil_warn(loc,((_tmp151C="Type variable %s unconstrained, assuming boxed",
_tag_dyneither(_tmp151C,sizeof(char),47))),_tag_dyneither(_tmp151B,sizeof(void*),
1)))))));}_tmpCE3=_tmpCE0;goto _LL658;_LL657: {struct Cyc_Absyn_Less_kb_struct*
_tmpCE1=(struct Cyc_Absyn_Less_kb_struct*)_tmpCDD;if(_tmpCE1->tag != 2)goto _LL659;
else{_tmpCE2=_tmpCE1->f1;_tmpCE3=(struct Cyc_Core_Opt**)& _tmpCE1->f1;_tmpCE4=
_tmpCE1->f2;_tmpCE5=*_tmpCE4;_tmpCE6=_tmpCE5.kind;if(_tmpCE6 != Cyc_Absyn_BoxKind)
goto _LL659;_tmpCE7=_tmpCE5.aliasqual;if(_tmpCE7 != Cyc_Absyn_Top)goto _LL659;}}
_LL658: _tmpCEA=_tmpCE3;goto _LL65A;_LL659: {struct Cyc_Absyn_Less_kb_struct*
_tmpCE8=(struct Cyc_Absyn_Less_kb_struct*)_tmpCDD;if(_tmpCE8->tag != 2)goto _LL65B;
else{_tmpCE9=_tmpCE8->f1;_tmpCEA=(struct Cyc_Core_Opt**)& _tmpCE8->f1;_tmpCEB=
_tmpCE8->f2;_tmpCEC=*_tmpCEB;_tmpCED=_tmpCEC.kind;if(_tmpCED != Cyc_Absyn_MemKind)
goto _LL65B;_tmpCEE=_tmpCEC.aliasqual;if(_tmpCEE != Cyc_Absyn_Top)goto _LL65B;}}
_LL65A: _tmpCF1=_tmpCEA;goto _LL65C;_LL65B: {struct Cyc_Absyn_Less_kb_struct*
_tmpCEF=(struct Cyc_Absyn_Less_kb_struct*)_tmpCDD;if(_tmpCEF->tag != 2)goto _LL65D;
else{_tmpCF0=_tmpCEF->f1;_tmpCF1=(struct Cyc_Core_Opt**)& _tmpCEF->f1;_tmpCF2=
_tmpCEF->f2;_tmpCF3=*_tmpCF2;_tmpCF4=_tmpCF3.kind;if(_tmpCF4 != Cyc_Absyn_MemKind)
goto _LL65D;_tmpCF5=_tmpCF3.aliasqual;if(_tmpCF5 != Cyc_Absyn_Aliasable)goto _LL65D;}}
_LL65C: _tmpCF8=_tmpCF1;goto _LL65E;_LL65D: {struct Cyc_Absyn_Less_kb_struct*
_tmpCF6=(struct Cyc_Absyn_Less_kb_struct*)_tmpCDD;if(_tmpCF6->tag != 2)goto _LL65F;
else{_tmpCF7=_tmpCF6->f1;_tmpCF8=(struct Cyc_Core_Opt**)& _tmpCF6->f1;_tmpCF9=
_tmpCF6->f2;_tmpCFA=*_tmpCF9;_tmpCFB=_tmpCFA.kind;if(_tmpCFB != Cyc_Absyn_AnyKind)
goto _LL65F;_tmpCFC=_tmpCFA.aliasqual;if(_tmpCFC != Cyc_Absyn_Top)goto _LL65F;}}
_LL65E: _tmpCFF=_tmpCF8;goto _LL660;_LL65F: {struct Cyc_Absyn_Less_kb_struct*
_tmpCFD=(struct Cyc_Absyn_Less_kb_struct*)_tmpCDD;if(_tmpCFD->tag != 2)goto _LL661;
else{_tmpCFE=_tmpCFD->f1;_tmpCFF=(struct Cyc_Core_Opt**)& _tmpCFD->f1;_tmpD00=
_tmpCFD->f2;_tmpD01=*_tmpD00;_tmpD02=_tmpD01.kind;if(_tmpD02 != Cyc_Absyn_AnyKind)
goto _LL661;_tmpD03=_tmpD01.aliasqual;if(_tmpD03 != Cyc_Absyn_Aliasable)goto _LL661;}}
_LL660:*_tmpCFF=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL654;_LL661: {
struct Cyc_Absyn_Less_kb_struct*_tmpD04=(struct Cyc_Absyn_Less_kb_struct*)_tmpCDD;
if(_tmpD04->tag != 2)goto _LL663;else{_tmpD05=_tmpD04->f1;_tmpD06=(struct Cyc_Core_Opt**)&
_tmpD04->f1;_tmpD07=_tmpD04->f2;_tmpD08=*_tmpD07;_tmpD09=_tmpD08.kind;if(_tmpD09
!= Cyc_Absyn_MemKind)goto _LL663;_tmpD0A=_tmpD08.aliasqual;if(_tmpD0A != Cyc_Absyn_Unique)
goto _LL663;}}_LL662: _tmpD0D=_tmpD06;goto _LL664;_LL663: {struct Cyc_Absyn_Less_kb_struct*
_tmpD0B=(struct Cyc_Absyn_Less_kb_struct*)_tmpCDD;if(_tmpD0B->tag != 2)goto _LL665;
else{_tmpD0C=_tmpD0B->f1;_tmpD0D=(struct Cyc_Core_Opt**)& _tmpD0B->f1;_tmpD0E=
_tmpD0B->f2;_tmpD0F=*_tmpD0E;_tmpD10=_tmpD0F.kind;if(_tmpD10 != Cyc_Absyn_AnyKind)
goto _LL665;_tmpD11=_tmpD0F.aliasqual;if(_tmpD11 != Cyc_Absyn_Unique)goto _LL665;}}
_LL664:*_tmpD0D=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL654;_LL665: {
struct Cyc_Absyn_Less_kb_struct*_tmpD12=(struct Cyc_Absyn_Less_kb_struct*)_tmpCDD;
if(_tmpD12->tag != 2)goto _LL667;else{_tmpD13=_tmpD12->f1;_tmpD14=(struct Cyc_Core_Opt**)&
_tmpD12->f1;_tmpD15=_tmpD12->f2;_tmpD16=*_tmpD15;_tmpD17=_tmpD16.kind;if(_tmpD17
!= Cyc_Absyn_RgnKind)goto _LL667;_tmpD18=_tmpD16.aliasqual;if(_tmpD18 != Cyc_Absyn_Top)
goto _LL667;}}_LL666:*_tmpD14=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto
_LL654;_LL667: {struct Cyc_Absyn_Less_kb_struct*_tmpD19=(struct Cyc_Absyn_Less_kb_struct*)
_tmpCDD;if(_tmpD19->tag != 2)goto _LL669;else{_tmpD1A=_tmpD19->f1;_tmpD1B=(struct
Cyc_Core_Opt**)& _tmpD19->f1;_tmpD1C=_tmpD19->f2;}}_LL668:*_tmpD1B=Cyc_Tcutil_kind_to_bound_opt(
_tmpD1C);goto _LL654;_LL669: {struct Cyc_Absyn_Eq_kb_struct*_tmpD1D=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpCDD;if(_tmpD1D->tag != 0)goto _LL66B;else{_tmpD1E=_tmpD1D->f1;_tmpD1F=*_tmpD1E;
_tmpD20=_tmpD1F.kind;if(_tmpD20 != Cyc_Absyn_MemKind)goto _LL66B;}}_LL66A:{const
char*_tmp151F;void*_tmp151E;(_tmp151E=0,Cyc_Tcutil_terr(loc,((_tmp151F="functions can't abstract M types",
_tag_dyneither(_tmp151F,sizeof(char),33))),_tag_dyneither(_tmp151E,sizeof(void*),
0)));}goto _LL654;_LL66B:;_LL66C: goto _LL654;_LL654:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
Cyc_Core_heap_region,_tmpC44.kind_env,*_tmpB45);_tmpC44.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmpC44.r,_tmpC44.free_vars,*_tmpB45);{struct Cyc_List_List*tvs=_tmpC44.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpD27;int _tmpD28;struct
_tuple29 _tmpD26=*((struct _tuple29*)tvs->hd);_tmpD27=_tmpD26.f1;_tmpD28=_tmpD26.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpD27,_tmpD28);}}{struct Cyc_List_List*evs=_tmpC44.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmpD2A;int _tmpD2B;struct _tuple30 _tmpD29=*((struct _tuple30*)evs->hd);
_tmpD2A=_tmpD29.f1;_tmpD2B=_tmpD29.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmpD2A,_tmpD2B);}};}goto _LL574;};}_LL595: {struct Cyc_Absyn_TupleType_struct*
_tmpB50=(struct Cyc_Absyn_TupleType_struct*)_tmpB08;if(_tmpB50->tag != 11)goto
_LL597;else{_tmpB51=_tmpB50->f1;}}_LL596: for(0;_tmpB51 != 0;_tmpB51=_tmpB51->tl){
struct _tuple11*_tmpD2D=(struct _tuple11*)_tmpB51->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpD2D).f2,1);((*_tmpD2D).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmpD2D).f1).print_const,(*_tmpD2D).f2);}goto _LL574;_LL597: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpB52=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpB08;if(_tmpB52->tag != 13)goto
_LL599;else{_tmpB53=_tmpB52->f1;_tmpB54=_tmpB52->f2;}}_LL598: {struct
_RegionHandle*_tmpD2E=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*prev_fields=0;
for(0;_tmpB54 != 0;_tmpB54=_tmpB54->tl){struct Cyc_Absyn_Aggrfield _tmpD30;struct
_dyneither_ptr*_tmpD31;struct Cyc_Absyn_Tqual _tmpD32;struct Cyc_Absyn_Tqual*
_tmpD33;void*_tmpD34;struct Cyc_Absyn_Exp*_tmpD35;struct Cyc_List_List*_tmpD36;
struct Cyc_Absyn_Aggrfield*_tmpD2F=(struct Cyc_Absyn_Aggrfield*)_tmpB54->hd;
_tmpD30=*_tmpD2F;_tmpD31=_tmpD30.name;_tmpD32=_tmpD30.tq;_tmpD33=(struct Cyc_Absyn_Tqual*)&(*
_tmpD2F).tq;_tmpD34=_tmpD30.type;_tmpD35=_tmpD30.width;_tmpD36=_tmpD30.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpD31)){
const char*_tmp1523;void*_tmp1522[1];struct Cyc_String_pa_struct _tmp1521;(_tmp1521.tag=
0,((_tmp1521.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpD31),((
_tmp1522[0]=& _tmp1521,Cyc_Tcutil_terr(loc,((_tmp1523="duplicate field %s",
_tag_dyneither(_tmp1523,sizeof(char),19))),_tag_dyneither(_tmp1522,sizeof(void*),
1)))))));}{const char*_tmp1524;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpD31,((
_tmp1524="",_tag_dyneither(_tmp1524,sizeof(char),1))))!= 0){struct Cyc_List_List*
_tmp1525;prev_fields=((_tmp1525=_region_malloc(_tmpD2E,sizeof(*_tmp1525)),((
_tmp1525->hd=_tmpD31,((_tmp1525->tl=prev_fields,_tmp1525))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpD34,1);_tmpD33->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpD33->print_const,_tmpD34);if(_tmpB53 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(
_tmpD34)){const char*_tmp1529;void*_tmp1528[1];struct Cyc_String_pa_struct _tmp1527;(
_tmp1527.tag=0,((_tmp1527.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpD31),((_tmp1528[0]=& _tmp1527,Cyc_Tcutil_warn(loc,((_tmp1529="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp1529,sizeof(char),74))),_tag_dyneither(_tmp1528,sizeof(void*),
1)))))));}Cyc_Tcutil_check_bitfield(loc,te,_tmpD34,_tmpD35,_tmpD31);Cyc_Tcutil_check_field_atts(
loc,_tmpD31,_tmpD36);}}goto _LL574;}_LL599: {struct Cyc_Absyn_AggrType_struct*
_tmpB55=(struct Cyc_Absyn_AggrType_struct*)_tmpB08;if(_tmpB55->tag != 12)goto
_LL59B;else{_tmpB56=_tmpB55->f1;_tmpB57=_tmpB56.aggr_info;_tmpB58=(union Cyc_Absyn_AggrInfoU*)&(
_tmpB55->f1).aggr_info;_tmpB59=_tmpB56.targs;_tmpB5A=(struct Cyc_List_List**)&(
_tmpB55->f1).targs;}}_LL59A:{union Cyc_Absyn_AggrInfoU _tmpD3F=*_tmpB58;struct
_tuple4 _tmpD40;enum Cyc_Absyn_AggrKind _tmpD41;struct _tuple2*_tmpD42;struct Cyc_Core_Opt*
_tmpD43;struct Cyc_Absyn_Aggrdecl**_tmpD44;struct Cyc_Absyn_Aggrdecl*_tmpD45;
_LL66E: if((_tmpD3F.UnknownAggr).tag != 1)goto _LL670;_tmpD40=(struct _tuple4)(
_tmpD3F.UnknownAggr).val;_tmpD41=_tmpD40.f1;_tmpD42=_tmpD40.f2;_tmpD43=_tmpD40.f3;
_LL66F: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmpD46;_push_handler(&
_tmpD46);{int _tmpD48=0;if(setjmp(_tmpD46.handler))_tmpD48=1;if(!_tmpD48){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpD42);{struct Cyc_Absyn_Aggrdecl*_tmpD49=*adp;if(_tmpD49->kind != 
_tmpD41){if(_tmpD49->kind == Cyc_Absyn_StructA){const char*_tmp152E;void*_tmp152D[
2];struct Cyc_String_pa_struct _tmp152C;struct Cyc_String_pa_struct _tmp152B;(
_tmp152B.tag=0,((_tmp152B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD42)),((_tmp152C.tag=0,((_tmp152C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD42)),((_tmp152D[0]=& _tmp152C,((
_tmp152D[1]=& _tmp152B,Cyc_Tcutil_terr(loc,((_tmp152E="expecting struct %s instead of union %s",
_tag_dyneither(_tmp152E,sizeof(char),40))),_tag_dyneither(_tmp152D,sizeof(void*),
2)))))))))))));}else{const char*_tmp1533;void*_tmp1532[2];struct Cyc_String_pa_struct
_tmp1531;struct Cyc_String_pa_struct _tmp1530;(_tmp1530.tag=0,((_tmp1530.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD42)),((
_tmp1531.tag=0,((_tmp1531.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD42)),((_tmp1532[0]=& _tmp1531,((_tmp1532[1]=& _tmp1530,Cyc_Tcutil_terr(loc,((
_tmp1533="expecting union %s instead of struct %s",_tag_dyneither(_tmp1533,
sizeof(char),40))),_tag_dyneither(_tmp1532,sizeof(void*),2)))))))))))));}}if((
unsigned int)_tmpD43  && (int)_tmpD43->v){if(!((unsigned int)_tmpD49->impl) || !((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD49->impl))->tagged){const char*
_tmp1537;void*_tmp1536[1];struct Cyc_String_pa_struct _tmp1535;(_tmp1535.tag=0,((
_tmp1535.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD42)),((_tmp1536[0]=& _tmp1535,Cyc_Tcutil_terr(loc,((_tmp1537="@tagged qualfiers don't agree on union %s",
_tag_dyneither(_tmp1537,sizeof(char),42))),_tag_dyneither(_tmp1536,sizeof(void*),
1)))))));}}*_tmpB58=Cyc_Absyn_KnownAggr(adp);};;_pop_handler();}else{void*
_tmpD47=(void*)_exn_thrown;void*_tmpD56=_tmpD47;_LL673: {struct Cyc_Dict_Absent_struct*
_tmpD57=(struct Cyc_Dict_Absent_struct*)_tmpD56;if(_tmpD57->tag != Cyc_Dict_Absent)
goto _LL675;}_LL674: {struct Cyc_Tcenv_Genv*_tmpD58=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp1538;struct Cyc_Absyn_Aggrdecl*_tmpD59=(_tmp1538=
_cycalloc(sizeof(*_tmp1538)),((_tmp1538->kind=_tmpD41,((_tmp1538->sc=Cyc_Absyn_Extern,((
_tmp1538->name=_tmpD42,((_tmp1538->tvs=0,((_tmp1538->impl=0,((_tmp1538->attributes=
0,_tmp1538)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmpD58,loc,_tmpD59);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpD42);*_tmpB58=Cyc_Absyn_KnownAggr(adp);if(*_tmpB5A != 0){{const char*
_tmp153C;void*_tmp153B[1];struct Cyc_String_pa_struct _tmp153A;(_tmp153A.tag=0,((
_tmp153A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD42)),((_tmp153B[0]=& _tmp153A,Cyc_Tcutil_terr(loc,((_tmp153C="declare parameterized type %s before using",
_tag_dyneither(_tmp153C,sizeof(char),43))),_tag_dyneither(_tmp153B,sizeof(void*),
1)))))));}return cvtenv;}goto _LL672;}_LL675:;_LL676:(void)_throw(_tmpD56);_LL672:;}};}
_tmpD45=*adp;goto _LL671;}_LL670: if((_tmpD3F.KnownAggr).tag != 2)goto _LL66D;
_tmpD44=(struct Cyc_Absyn_Aggrdecl**)(_tmpD3F.KnownAggr).val;_tmpD45=*_tmpD44;
_LL671: {struct Cyc_List_List*tvs=_tmpD45->tvs;struct Cyc_List_List*ts=*_tmpB5A;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Tvar*_tmpD5E=(
struct Cyc_Absyn_Tvar*)tvs->hd;void*_tmpD5F=(void*)ts->hd;{struct _tuple0 _tmp153D;
struct _tuple0 _tmpD61=(_tmp153D.f1=Cyc_Absyn_compress_kb(_tmpD5E->kind),((
_tmp153D.f2=_tmpD5F,_tmp153D)));void*_tmpD62;void*_tmpD64;struct Cyc_Absyn_Tvar*
_tmpD66;_LL678: _tmpD62=_tmpD61.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpD63=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmpD62;if(_tmpD63->tag != 1)goto _LL67A;};
_tmpD64=_tmpD61.f2;{struct Cyc_Absyn_VarType_struct*_tmpD65=(struct Cyc_Absyn_VarType_struct*)
_tmpD64;if(_tmpD65->tag != 2)goto _LL67A;else{_tmpD66=_tmpD65->f1;}};_LL679: cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpD66);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpD66,1);continue;_LL67A:;_LL67B: goto _LL677;_LL677:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);Cyc_Tcutil_check_no_qual(
loc,(void*)ts->hd);};}if(ts != 0){const char*_tmp1541;void*_tmp1540[1];struct Cyc_String_pa_struct
_tmp153F;(_tmp153F.tag=0,((_tmp153F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD45->name)),((_tmp1540[0]=& _tmp153F,
Cyc_Tcutil_terr(loc,((_tmp1541="too many parameters for type %s",_tag_dyneither(
_tmp1541,sizeof(char),32))),_tag_dyneither(_tmp1540,sizeof(void*),1)))))));}if(
tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,
expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_tmp1542;
hidden_ts=((_tmp1542=_cycalloc(sizeof(*_tmp1542)),((_tmp1542->hd=e,((_tmp1542->tl=
hidden_ts,_tmp1542))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,
1);}*_tmpB5A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(*_tmpB5A,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(hidden_ts));}}_LL66D:;}goto _LL574;_LL59B: {struct Cyc_Absyn_TypedefType_struct*
_tmpB5B=(struct Cyc_Absyn_TypedefType_struct*)_tmpB08;if(_tmpB5B->tag != 18)goto
_LL59D;else{_tmpB5C=_tmpB5B->f1;_tmpB5D=_tmpB5B->f2;_tmpB5E=(struct Cyc_List_List**)&
_tmpB5B->f2;_tmpB5F=_tmpB5B->f3;_tmpB60=(struct Cyc_Absyn_Typedefdecl**)& _tmpB5B->f3;
_tmpB61=_tmpB5B->f4;_tmpB62=(void***)& _tmpB5B->f4;}}_LL59C: {struct Cyc_List_List*
targs=*_tmpB5E;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmpD6B;
_push_handler(& _tmpD6B);{int _tmpD6D=0;if(setjmp(_tmpD6B.handler))_tmpD6D=1;if(!
_tmpD6D){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmpB5C);;_pop_handler();}else{
void*_tmpD6C=(void*)_exn_thrown;void*_tmpD6F=_tmpD6C;_LL67D: {struct Cyc_Dict_Absent_struct*
_tmpD70=(struct Cyc_Dict_Absent_struct*)_tmpD6F;if(_tmpD70->tag != Cyc_Dict_Absent)
goto _LL67F;}_LL67E:{const char*_tmp1546;void*_tmp1545[1];struct Cyc_String_pa_struct
_tmp1544;(_tmp1544.tag=0,((_tmp1544.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB5C)),((_tmp1545[0]=& _tmp1544,Cyc_Tcutil_terr(
loc,((_tmp1546="unbound typedef name %s",_tag_dyneither(_tmp1546,sizeof(char),24))),
_tag_dyneither(_tmp1545,sizeof(void*),1)))))));}return cvtenv;_LL67F:;_LL680:(
void)_throw(_tmpD6F);_LL67C:;}};}*_tmpB60=(struct Cyc_Absyn_Typedefdecl*)td;
_tmpB5C[0]=(td->name)[0];{struct Cyc_List_List*tvs=td->tvs;struct Cyc_List_List*ts=
targs;struct _RegionHandle*_tmpD74=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Kind*k=
Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,
expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd,1);Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple15*_tmp1549;struct Cyc_List_List*_tmp1548;inst=((_tmp1548=
_region_malloc(_tmpD74,sizeof(*_tmp1548)),((_tmp1548->hd=((_tmp1549=
_region_malloc(_tmpD74,sizeof(*_tmp1549)),((_tmp1549->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmp1549->f2=(void*)ts->hd,_tmp1549)))))),((_tmp1548->tl=inst,_tmp1548))))));};}
if(ts != 0){const char*_tmp154D;void*_tmp154C[1];struct Cyc_String_pa_struct
_tmp154B;(_tmp154B.tag=0,((_tmp154B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB5C)),((_tmp154C[0]=& _tmp154B,Cyc_Tcutil_terr(
loc,((_tmp154D="too many parameters for typedef %s",_tag_dyneither(_tmp154D,
sizeof(char),35))),_tag_dyneither(_tmp154C,sizeof(void*),1)))))));}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,
expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);void*e=Cyc_Absyn_new_evar(0,0);{
struct Cyc_List_List*_tmp154E;hidden_ts=((_tmp154E=_cycalloc(sizeof(*_tmp154E)),((
_tmp154E->hd=e,((_tmp154E->tl=hidden_ts,_tmp154E))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);{struct _tuple15*_tmp1551;struct Cyc_List_List*_tmp1550;inst=(
struct Cyc_List_List*)((_tmp1550=_cycalloc(sizeof(*_tmp1550)),((_tmp1550->hd=(
struct _tuple15*)((_tmp1551=_cycalloc(sizeof(*_tmp1551)),((_tmp1551->f1=(struct
Cyc_Absyn_Tvar*)tvs->hd,((_tmp1551->f2=e,_tmp1551)))))),((_tmp1550->tl=inst,
_tmp1550))))));};}*_tmpB5E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}if(td->defn != 0){void*
new_typ=Cyc_Tcutil_rsubstitute(_tmpD74,inst,(void*)((struct Cyc_Core_Opt*)
_check_null(td->defn))->v);void**_tmp1552;*_tmpB62=((_tmp1552=_cycalloc(sizeof(*
_tmp1552)),((_tmp1552[0]=new_typ,_tmp1552))));}}goto _LL574;};}_LL59D: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpB63=(struct Cyc_Absyn_UniqueRgn_struct*)_tmpB08;if(_tmpB63->tag != 22)goto
_LL59F;}_LL59E: goto _LL5A0;_LL59F: {struct Cyc_Absyn_HeapRgn_struct*_tmpB64=(
struct Cyc_Absyn_HeapRgn_struct*)_tmpB08;if(_tmpB64->tag != 21)goto _LL5A1;}_LL5A0:
goto _LL574;_LL5A1: {struct Cyc_Absyn_RgnHandleType_struct*_tmpB65=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmpB08;if(_tmpB65->tag != 16)goto _LL5A3;else{_tmpB66=(void*)_tmpB65->f1;}}_LL5A2:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpB66,1);
goto _LL574;_LL5A3: {struct Cyc_Absyn_DynRgnType_struct*_tmpB67=(struct Cyc_Absyn_DynRgnType_struct*)
_tmpB08;if(_tmpB67->tag != 17)goto _LL5A5;else{_tmpB68=(void*)_tmpB67->f1;_tmpB69=(
void*)_tmpB67->f2;}}_LL5A4: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,&
Cyc_Tcutil_rk,_tmpB68,0);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,
_tmpB69,1);goto _LL574;_LL5A5: {struct Cyc_Absyn_AccessEff_struct*_tmpB6A=(struct
Cyc_Absyn_AccessEff_struct*)_tmpB08;if(_tmpB6A->tag != 23)goto _LL5A7;else{_tmpB6B=(
void*)_tmpB6A->f1;}}_LL5A6: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,&
Cyc_Tcutil_trk,_tmpB6B,1);goto _LL574;_LL5A7: {struct Cyc_Absyn_RgnsEff_struct*
_tmpB6C=(struct Cyc_Absyn_RgnsEff_struct*)_tmpB08;if(_tmpB6C->tag != 25)goto _LL5A9;
else{_tmpB6D=(void*)_tmpB6C->f1;}}_LL5A8: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB6D,1);goto _LL574;_LL5A9: {struct Cyc_Absyn_JoinEff_struct*
_tmpB6E=(struct Cyc_Absyn_JoinEff_struct*)_tmpB08;if(_tmpB6E->tag != 24)goto _LL574;
else{_tmpB6F=_tmpB6E->f1;}}_LL5AA: for(0;_tmpB6F != 0;_tmpB6F=_tmpB6F->tl){cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmpB6F->hd,1);}
goto _LL574;_LL574:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){{
void*_tmpD7E=t;struct Cyc_Core_Opt*_tmpD80;struct Cyc_Core_Opt*_tmpD81;_LL682: {
struct Cyc_Absyn_Evar_struct*_tmpD7F=(struct Cyc_Absyn_Evar_struct*)_tmpD7E;if(
_tmpD7F->tag != 1)goto _LL684;else{_tmpD80=_tmpD7F->f1;_tmpD81=_tmpD7F->f2;}}
_LL683: {struct _dyneither_ptr s;{struct Cyc_Core_Opt*_tmpD82=_tmpD80;struct Cyc_Core_Opt
_tmpD83;struct Cyc_Absyn_Kind*_tmpD84;_LL687: if(_tmpD82 != 0)goto _LL689;_LL688:{
const char*_tmp1553;s=((_tmp1553="kind=NULL ",_tag_dyneither(_tmp1553,sizeof(char),
11)));}goto _LL686;_LL689: if(_tmpD82 == 0)goto _LL686;_tmpD83=*_tmpD82;_tmpD84=(
struct Cyc_Absyn_Kind*)_tmpD83.v;_LL68A:{const char*_tmp1557;void*_tmp1556[1];
struct Cyc_String_pa_struct _tmp1555;s=(struct _dyneither_ptr)((_tmp1555.tag=0,((
_tmp1555.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmpD84)),((_tmp1556[0]=& _tmp1555,Cyc_aprintf(((_tmp1557="kind=%s ",
_tag_dyneither(_tmp1557,sizeof(char),9))),_tag_dyneither(_tmp1556,sizeof(void*),
1))))))));}goto _LL686;_LL686:;}{struct Cyc_Core_Opt*_tmpD89=_tmpD81;struct Cyc_Core_Opt
_tmpD8A;void*_tmpD8B;_LL68C: if(_tmpD89 != 0)goto _LL68E;_LL68D:{const char*_tmp155B;
void*_tmp155A[1];struct Cyc_String_pa_struct _tmp1559;s=(struct _dyneither_ptr)((
_tmp1559.tag=0,((_tmp1559.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp155A[0]=& _tmp1559,Cyc_aprintf(((_tmp155B="%s ref=NULL",_tag_dyneither(
_tmp155B,sizeof(char),12))),_tag_dyneither(_tmp155A,sizeof(void*),1))))))));}
goto _LL68B;_LL68E: if(_tmpD89 == 0)goto _LL68B;_tmpD8A=*_tmpD89;_tmpD8B=(void*)
_tmpD8A.v;_LL68F:{const char*_tmp1560;void*_tmp155F[2];struct Cyc_String_pa_struct
_tmp155E;struct Cyc_String_pa_struct _tmp155D;s=(struct _dyneither_ptr)((_tmp155D.tag=
0,((_tmp155D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmpD8B)),((_tmp155E.tag=0,((_tmp155E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmp155F[0]=& _tmp155E,((_tmp155F[1]=& _tmp155D,Cyc_aprintf(((
_tmp1560="%s ref=%s",_tag_dyneither(_tmp1560,sizeof(char),10))),_tag_dyneither(
_tmp155F,sizeof(void*),2))))))))))))));}goto _LL68B;_LL68B:;}goto _LL681;}_LL684:;
_LL685: goto _LL681;_LL681:;}{const char*_tmp1566;void*_tmp1565[3];struct Cyc_String_pa_struct
_tmp1564;struct Cyc_String_pa_struct _tmp1563;struct Cyc_String_pa_struct _tmp1562;(
_tmp1562.tag=0,((_tmp1562.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
expected_kind)),((_tmp1563.tag=0,((_tmp1563.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp1564.tag=0,((
_tmp1564.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp1565[0]=& _tmp1564,((_tmp1565[1]=& _tmp1563,((_tmp1565[2]=& _tmp1562,Cyc_Tcutil_terr(
loc,((_tmp1566="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmp1566,sizeof(char),51))),_tag_dyneither(_tmp1565,sizeof(void*),
3)))))))))))))))))));};}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{
void*_tmpD98=e->r;struct Cyc_List_List*_tmpD9E;struct Cyc_Absyn_Exp*_tmpDA0;struct
Cyc_Absyn_Exp*_tmpDA1;struct Cyc_Absyn_Exp*_tmpDA2;struct Cyc_Absyn_Exp*_tmpDA4;
struct Cyc_Absyn_Exp*_tmpDA5;struct Cyc_Absyn_Exp*_tmpDA7;struct Cyc_Absyn_Exp*
_tmpDA8;struct Cyc_Absyn_Exp*_tmpDAA;struct Cyc_Absyn_Exp*_tmpDAB;void*_tmpDAD;
struct Cyc_Absyn_Exp*_tmpDAE;void*_tmpDB0;void*_tmpDB2;void*_tmpDB4;struct Cyc_Absyn_Exp*
_tmpDB6;_LL691: {struct Cyc_Absyn_Const_e_struct*_tmpD99=(struct Cyc_Absyn_Const_e_struct*)
_tmpD98;if(_tmpD99->tag != 0)goto _LL693;}_LL692: goto _LL694;_LL693: {struct Cyc_Absyn_Enum_e_struct*
_tmpD9A=(struct Cyc_Absyn_Enum_e_struct*)_tmpD98;if(_tmpD9A->tag != 32)goto _LL695;}
_LL694: goto _LL696;_LL695: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpD9B=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpD98;if(_tmpD9B->tag != 33)goto _LL697;}_LL696: goto _LL698;_LL697: {struct Cyc_Absyn_Var_e_struct*
_tmpD9C=(struct Cyc_Absyn_Var_e_struct*)_tmpD98;if(_tmpD9C->tag != 1)goto _LL699;}
_LL698: goto _LL690;_LL699: {struct Cyc_Absyn_Primop_e_struct*_tmpD9D=(struct Cyc_Absyn_Primop_e_struct*)
_tmpD98;if(_tmpD9D->tag != 3)goto _LL69B;else{_tmpD9E=_tmpD9D->f2;}}_LL69A: for(0;
_tmpD9E != 0;_tmpD9E=_tmpD9E->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmpD9E->hd,te,cvtenv);}goto _LL690;_LL69B: {struct Cyc_Absyn_Conditional_e_struct*
_tmpD9F=(struct Cyc_Absyn_Conditional_e_struct*)_tmpD98;if(_tmpD9F->tag != 6)goto
_LL69D;else{_tmpDA0=_tmpD9F->f1;_tmpDA1=_tmpD9F->f2;_tmpDA2=_tmpD9F->f3;}}_LL69C:
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDA0,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpDA1,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDA2,te,
cvtenv);goto _LL690;_LL69D: {struct Cyc_Absyn_And_e_struct*_tmpDA3=(struct Cyc_Absyn_And_e_struct*)
_tmpD98;if(_tmpDA3->tag != 7)goto _LL69F;else{_tmpDA4=_tmpDA3->f1;_tmpDA5=_tmpDA3->f2;}}
_LL69E: _tmpDA7=_tmpDA4;_tmpDA8=_tmpDA5;goto _LL6A0;_LL69F: {struct Cyc_Absyn_Or_e_struct*
_tmpDA6=(struct Cyc_Absyn_Or_e_struct*)_tmpD98;if(_tmpDA6->tag != 8)goto _LL6A1;
else{_tmpDA7=_tmpDA6->f1;_tmpDA8=_tmpDA6->f2;}}_LL6A0: _tmpDAA=_tmpDA7;_tmpDAB=
_tmpDA8;goto _LL6A2;_LL6A1: {struct Cyc_Absyn_SeqExp_e_struct*_tmpDA9=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpD98;if(_tmpDA9->tag != 9)goto _LL6A3;else{_tmpDAA=_tmpDA9->f1;_tmpDAB=_tmpDA9->f2;}}
_LL6A2: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDAA,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDAB,te,cvtenv);goto _LL690;_LL6A3: {
struct Cyc_Absyn_Cast_e_struct*_tmpDAC=(struct Cyc_Absyn_Cast_e_struct*)_tmpD98;
if(_tmpDAC->tag != 15)goto _LL6A5;else{_tmpDAD=(void*)_tmpDAC->f1;_tmpDAE=_tmpDAC->f2;}}
_LL6A4: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDAE,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpDAD,1);goto
_LL690;_LL6A5: {struct Cyc_Absyn_Offsetof_e_struct*_tmpDAF=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpD98;if(_tmpDAF->tag != 20)goto _LL6A7;else{_tmpDB0=(void*)_tmpDAF->f1;}}_LL6A6:
_tmpDB2=_tmpDB0;goto _LL6A8;_LL6A7: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpDB1=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpD98;if(_tmpDB1->tag != 18)goto _LL6A9;
else{_tmpDB2=(void*)_tmpDB1->f1;}}_LL6A8: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,
te,cvtenv,& Cyc_Tcutil_tak,_tmpDB2,1);goto _LL690;_LL6A9: {struct Cyc_Absyn_Valueof_e_struct*
_tmpDB3=(struct Cyc_Absyn_Valueof_e_struct*)_tmpD98;if(_tmpDB3->tag != 39)goto
_LL6AB;else{_tmpDB4=(void*)_tmpDB3->f1;}}_LL6AA: cvtenv=Cyc_Tcutil_i_check_valid_type(
e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpDB4,1);goto _LL690;_LL6AB: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpDB5=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpD98;if(_tmpDB5->tag != 19)goto
_LL6AD;else{_tmpDB6=_tmpDB5->f1;}}_LL6AC: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpDB6,te,cvtenv);goto _LL690;_LL6AD:;_LL6AE: {const char*_tmp1569;void*_tmp1568;(
_tmp1568=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1569="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmp1569,sizeof(char),66))),_tag_dyneither(_tmp1568,sizeof(void*),
0)));}_LL690:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){struct Cyc_List_List*_tmpDB9=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*
vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){struct Cyc_Absyn_Tvar*_tmpDBB;struct
_tuple29 _tmpDBA=*((struct _tuple29*)vs->hd);_tmpDBB=_tmpDBA.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(
_tmpDB9,_tmpDBB);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpDBD;struct _tuple30 _tmpDBC=*((struct _tuple30*)evs->hd);_tmpDBD=_tmpDBC.f1;{
void*_tmpDBE=Cyc_Tcutil_compress(_tmpDBD);struct Cyc_Core_Opt*_tmpDC0;struct Cyc_Core_Opt**
_tmpDC1;_LL6B0: {struct Cyc_Absyn_Evar_struct*_tmpDBF=(struct Cyc_Absyn_Evar_struct*)
_tmpDBE;if(_tmpDBF->tag != 1)goto _LL6B2;else{_tmpDC0=_tmpDBF->f4;_tmpDC1=(struct
Cyc_Core_Opt**)& _tmpDBF->f4;}}_LL6B1: if(*_tmpDC1 == 0){struct Cyc_Core_Opt*
_tmp156A;*_tmpDC1=((_tmp156A=_cycalloc(sizeof(*_tmp156A)),((_tmp156A->v=_tmpDB9,
_tmp156A))));}else{struct Cyc_List_List*_tmpDC3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmpDC1))->v;struct Cyc_List_List*_tmpDC4=0;for(0;_tmpDC3 != 0;
_tmpDC3=_tmpDC3->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmpDB9,(struct Cyc_Absyn_Tvar*)_tmpDC3->hd)){struct Cyc_List_List*_tmp156B;
_tmpDC4=((_tmp156B=_cycalloc(sizeof(*_tmp156B)),((_tmp156B->hd=(struct Cyc_Absyn_Tvar*)
_tmpDC3->hd,((_tmp156B->tl=_tmpDC4,_tmp156B))))));}}{struct Cyc_Core_Opt*_tmp156C;*
_tmpDC1=((_tmp156C=_cycalloc(sizeof(*_tmp156C)),((_tmp156C->v=_tmpDC4,_tmp156C))));};}
goto _LL6AF;_LL6B2:;_LL6B3: goto _LL6AF;_LL6AF:;};}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmpDC7=Cyc_Tcenv_lookup_type_vars(
te);struct _RegionHandle*_tmpDC8=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv
_tmp156D;struct Cyc_Tcutil_CVTEnv _tmpDC9=Cyc_Tcutil_check_valid_type(loc,te,((
_tmp156D.r=_tmpDC8,((_tmp156D.kind_env=_tmpDC7,((_tmp156D.free_vars=0,((_tmp156D.free_evars=
0,((_tmp156D.generalize_evars=generalize_evars,((_tmp156D.fn_result=0,_tmp156D)))))))))))),&
Cyc_Tcutil_tmk,t);struct Cyc_List_List*_tmpDCA=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple29*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpDC8,(struct Cyc_Absyn_Tvar*(*)(struct _tuple29*))Cyc_Core_fst,
_tmpDC9.free_vars);struct Cyc_List_List*_tmpDCB=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple30*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmpDC8,(void*(*)(struct _tuple30*))Cyc_Core_fst,_tmpDC9.free_evars);if(_tmpDC7 != 
0){struct Cyc_List_List*_tmpDCC=0;{struct Cyc_List_List*_tmpDCD=_tmpDCA;for(0;(
unsigned int)_tmpDCD;_tmpDCD=_tmpDCD->tl){struct Cyc_Absyn_Tvar*_tmpDCE=(struct
Cyc_Absyn_Tvar*)_tmpDCD->hd;int found=0;{struct Cyc_List_List*_tmpDCF=_tmpDC7;for(
0;(unsigned int)_tmpDCF;_tmpDCF=_tmpDCF->tl){if(Cyc_Absyn_tvar_cmp(_tmpDCE,(
struct Cyc_Absyn_Tvar*)_tmpDCF->hd)== 0){found=1;break;}}}if(!found){struct Cyc_List_List*
_tmp156E;_tmpDCC=((_tmp156E=_region_malloc(_tmpDC8,sizeof(*_tmp156E)),((_tmp156E->hd=(
struct Cyc_Absyn_Tvar*)_tmpDCD->hd,((_tmp156E->tl=_tmpDCC,_tmp156E))))));}}}
_tmpDCA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpDCC);}{struct Cyc_List_List*x=_tmpDCA;for(0;x != 0;x=x->tl){void*_tmpDD1=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmpDD3;struct Cyc_Core_Opt**
_tmpDD4;struct Cyc_Core_Opt*_tmpDD6;struct Cyc_Core_Opt**_tmpDD7;struct Cyc_Absyn_Kind*
_tmpDD8;struct Cyc_Absyn_Kind _tmpDD9;enum Cyc_Absyn_KindQual _tmpDDA;enum Cyc_Absyn_AliasQual
_tmpDDB;struct Cyc_Core_Opt*_tmpDDD;struct Cyc_Core_Opt**_tmpDDE;struct Cyc_Absyn_Kind*
_tmpDDF;struct Cyc_Absyn_Kind _tmpDE0;enum Cyc_Absyn_KindQual _tmpDE1;enum Cyc_Absyn_AliasQual
_tmpDE2;struct Cyc_Core_Opt*_tmpDE4;struct Cyc_Core_Opt**_tmpDE5;struct Cyc_Absyn_Kind*
_tmpDE6;struct Cyc_Absyn_Kind _tmpDE7;enum Cyc_Absyn_KindQual _tmpDE8;enum Cyc_Absyn_AliasQual
_tmpDE9;struct Cyc_Core_Opt*_tmpDEB;struct Cyc_Core_Opt**_tmpDEC;struct Cyc_Absyn_Kind*
_tmpDED;struct Cyc_Absyn_Kind _tmpDEE;enum Cyc_Absyn_KindQual _tmpDEF;enum Cyc_Absyn_AliasQual
_tmpDF0;struct Cyc_Core_Opt*_tmpDF2;struct Cyc_Core_Opt**_tmpDF3;struct Cyc_Absyn_Kind*
_tmpDF4;struct Cyc_Absyn_Kind _tmpDF5;enum Cyc_Absyn_KindQual _tmpDF6;enum Cyc_Absyn_AliasQual
_tmpDF7;struct Cyc_Core_Opt*_tmpDF9;struct Cyc_Core_Opt**_tmpDFA;struct Cyc_Absyn_Kind*
_tmpDFB;struct Cyc_Absyn_Kind*_tmpDFD;struct Cyc_Absyn_Kind _tmpDFE;enum Cyc_Absyn_KindQual
_tmpDFF;enum Cyc_Absyn_AliasQual _tmpE00;_LL6B5: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpDD2=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpDD1;if(_tmpDD2->tag != 1)goto
_LL6B7;else{_tmpDD3=_tmpDD2->f1;_tmpDD4=(struct Cyc_Core_Opt**)& _tmpDD2->f1;}}
_LL6B6: _tmpDD7=_tmpDD4;goto _LL6B8;_LL6B7: {struct Cyc_Absyn_Less_kb_struct*
_tmpDD5=(struct Cyc_Absyn_Less_kb_struct*)_tmpDD1;if(_tmpDD5->tag != 2)goto _LL6B9;
else{_tmpDD6=_tmpDD5->f1;_tmpDD7=(struct Cyc_Core_Opt**)& _tmpDD5->f1;_tmpDD8=
_tmpDD5->f2;_tmpDD9=*_tmpDD8;_tmpDDA=_tmpDD9.kind;if(_tmpDDA != Cyc_Absyn_BoxKind)
goto _LL6B9;_tmpDDB=_tmpDD9.aliasqual;if(_tmpDDB != Cyc_Absyn_Top)goto _LL6B9;}}
_LL6B8: _tmpDDE=_tmpDD7;goto _LL6BA;_LL6B9: {struct Cyc_Absyn_Less_kb_struct*
_tmpDDC=(struct Cyc_Absyn_Less_kb_struct*)_tmpDD1;if(_tmpDDC->tag != 2)goto _LL6BB;
else{_tmpDDD=_tmpDDC->f1;_tmpDDE=(struct Cyc_Core_Opt**)& _tmpDDC->f1;_tmpDDF=
_tmpDDC->f2;_tmpDE0=*_tmpDDF;_tmpDE1=_tmpDE0.kind;if(_tmpDE1 != Cyc_Absyn_MemKind)
goto _LL6BB;_tmpDE2=_tmpDE0.aliasqual;if(_tmpDE2 != Cyc_Absyn_Top)goto _LL6BB;}}
_LL6BA: _tmpDE5=_tmpDDE;goto _LL6BC;_LL6BB: {struct Cyc_Absyn_Less_kb_struct*
_tmpDE3=(struct Cyc_Absyn_Less_kb_struct*)_tmpDD1;if(_tmpDE3->tag != 2)goto _LL6BD;
else{_tmpDE4=_tmpDE3->f1;_tmpDE5=(struct Cyc_Core_Opt**)& _tmpDE3->f1;_tmpDE6=
_tmpDE3->f2;_tmpDE7=*_tmpDE6;_tmpDE8=_tmpDE7.kind;if(_tmpDE8 != Cyc_Absyn_MemKind)
goto _LL6BD;_tmpDE9=_tmpDE7.aliasqual;if(_tmpDE9 != Cyc_Absyn_Aliasable)goto _LL6BD;}}
_LL6BC:*_tmpDE5=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6B4;_LL6BD: {
struct Cyc_Absyn_Less_kb_struct*_tmpDEA=(struct Cyc_Absyn_Less_kb_struct*)_tmpDD1;
if(_tmpDEA->tag != 2)goto _LL6BF;else{_tmpDEB=_tmpDEA->f1;_tmpDEC=(struct Cyc_Core_Opt**)&
_tmpDEA->f1;_tmpDED=_tmpDEA->f2;_tmpDEE=*_tmpDED;_tmpDEF=_tmpDEE.kind;if(_tmpDEF
!= Cyc_Absyn_MemKind)goto _LL6BF;_tmpDF0=_tmpDEE.aliasqual;if(_tmpDF0 != Cyc_Absyn_Unique)
goto _LL6BF;}}_LL6BE:*_tmpDEC=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto
_LL6B4;_LL6BF: {struct Cyc_Absyn_Less_kb_struct*_tmpDF1=(struct Cyc_Absyn_Less_kb_struct*)
_tmpDD1;if(_tmpDF1->tag != 2)goto _LL6C1;else{_tmpDF2=_tmpDF1->f1;_tmpDF3=(struct
Cyc_Core_Opt**)& _tmpDF1->f1;_tmpDF4=_tmpDF1->f2;_tmpDF5=*_tmpDF4;_tmpDF6=_tmpDF5.kind;
if(_tmpDF6 != Cyc_Absyn_RgnKind)goto _LL6C1;_tmpDF7=_tmpDF5.aliasqual;if(_tmpDF7 != 
Cyc_Absyn_Top)goto _LL6C1;}}_LL6C0:*_tmpDF3=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
goto _LL6B4;_LL6C1: {struct Cyc_Absyn_Less_kb_struct*_tmpDF8=(struct Cyc_Absyn_Less_kb_struct*)
_tmpDD1;if(_tmpDF8->tag != 2)goto _LL6C3;else{_tmpDF9=_tmpDF8->f1;_tmpDFA=(struct
Cyc_Core_Opt**)& _tmpDF8->f1;_tmpDFB=_tmpDF8->f2;}}_LL6C2:*_tmpDFA=Cyc_Tcutil_kind_to_bound_opt(
_tmpDFB);goto _LL6B4;_LL6C3: {struct Cyc_Absyn_Eq_kb_struct*_tmpDFC=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpDD1;if(_tmpDFC->tag != 0)goto _LL6C5;else{_tmpDFD=_tmpDFC->f1;_tmpDFE=*_tmpDFD;
_tmpDFF=_tmpDFE.kind;if(_tmpDFF != Cyc_Absyn_MemKind)goto _LL6C5;_tmpE00=_tmpDFE.aliasqual;}}
_LL6C4:{const char*_tmp1575;void*_tmp1574[2];struct Cyc_String_pa_struct _tmp1573;
struct Cyc_Absyn_Kind*_tmp1572;struct Cyc_String_pa_struct _tmp1571;(_tmp1571.tag=0,((
_tmp1571.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(((
_tmp1572=_cycalloc_atomic(sizeof(*_tmp1572)),((_tmp1572->kind=Cyc_Absyn_MemKind,((
_tmp1572->aliasqual=_tmpE00,_tmp1572)))))))),((_tmp1573.tag=0,((_tmp1573.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd)),((_tmp1574[0]=& _tmp1573,((_tmp1574[1]=& _tmp1571,Cyc_Tcutil_terr(loc,((
_tmp1575="type variable %s cannot have kind %s",_tag_dyneither(_tmp1575,sizeof(
char),37))),_tag_dyneither(_tmp1574,sizeof(void*),2)))))))))))));}goto _LL6B4;
_LL6C5:;_LL6C6: goto _LL6B4;_LL6B4:;}}if(_tmpDCA != 0  || _tmpDCB != 0){{void*_tmpE06=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpE08;struct Cyc_List_List*_tmpE09;
struct Cyc_List_List**_tmpE0A;struct Cyc_Core_Opt*_tmpE0B;struct Cyc_Absyn_Tqual
_tmpE0C;void*_tmpE0D;struct Cyc_List_List*_tmpE0E;int _tmpE0F;struct Cyc_Absyn_VarargInfo*
_tmpE10;struct Cyc_List_List*_tmpE11;struct Cyc_List_List*_tmpE12;_LL6C8: {struct
Cyc_Absyn_FnType_struct*_tmpE07=(struct Cyc_Absyn_FnType_struct*)_tmpE06;if(
_tmpE07->tag != 10)goto _LL6CA;else{_tmpE08=_tmpE07->f1;_tmpE09=_tmpE08.tvars;
_tmpE0A=(struct Cyc_List_List**)&(_tmpE07->f1).tvars;_tmpE0B=_tmpE08.effect;
_tmpE0C=_tmpE08.ret_tqual;_tmpE0D=_tmpE08.ret_typ;_tmpE0E=_tmpE08.args;_tmpE0F=
_tmpE08.c_varargs;_tmpE10=_tmpE08.cyc_varargs;_tmpE11=_tmpE08.rgn_po;_tmpE12=
_tmpE08.attributes;}}_LL6C9: if(*_tmpE0A == 0){*_tmpE0A=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_copy)(_tmpDCA);_tmpDCA=0;}goto _LL6C7;_LL6CA:;
_LL6CB: goto _LL6C7;_LL6C7:;}if(_tmpDCA != 0){const char*_tmp1579;void*_tmp1578[1];
struct Cyc_String_pa_struct _tmp1577;(_tmp1577.tag=0,((_tmp1577.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpDCA->hd)->name),((
_tmp1578[0]=& _tmp1577,Cyc_Tcutil_terr(loc,((_tmp1579="unbound type variable %s",
_tag_dyneither(_tmp1579,sizeof(char),25))),_tag_dyneither(_tmp1578,sizeof(void*),
1)))))));}if(_tmpDCB != 0)for(0;_tmpDCB != 0;_tmpDCB=_tmpDCB->tl){void*e=(void*)
_tmpDCB->hd;struct Cyc_Absyn_Kind*_tmpE16=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind
_tmpE17;enum Cyc_Absyn_KindQual _tmpE18;enum Cyc_Absyn_AliasQual _tmpE19;struct Cyc_Absyn_Kind
_tmpE1A;enum Cyc_Absyn_KindQual _tmpE1B;enum Cyc_Absyn_AliasQual _tmpE1C;struct Cyc_Absyn_Kind
_tmpE1D;enum Cyc_Absyn_KindQual _tmpE1E;enum Cyc_Absyn_AliasQual _tmpE1F;struct Cyc_Absyn_Kind
_tmpE20;enum Cyc_Absyn_KindQual _tmpE21;_LL6CD: _tmpE17=*_tmpE16;_tmpE18=_tmpE17.kind;
if(_tmpE18 != Cyc_Absyn_RgnKind)goto _LL6CF;_tmpE19=_tmpE17.aliasqual;if(_tmpE19 != 
Cyc_Absyn_Unique)goto _LL6CF;_LL6CE: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp157C;void*_tmp157B;(_tmp157B=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp157C="can't unify evar with unique region!",
_tag_dyneither(_tmp157C,sizeof(char),37))),_tag_dyneither(_tmp157B,sizeof(void*),
0)));}goto _LL6CC;_LL6CF: _tmpE1A=*_tmpE16;_tmpE1B=_tmpE1A.kind;if(_tmpE1B != Cyc_Absyn_RgnKind)
goto _LL6D1;_tmpE1C=_tmpE1A.aliasqual;if(_tmpE1C != Cyc_Absyn_Aliasable)goto _LL6D1;
_LL6D0: goto _LL6D2;_LL6D1: _tmpE1D=*_tmpE16;_tmpE1E=_tmpE1D.kind;if(_tmpE1E != Cyc_Absyn_RgnKind)
goto _LL6D3;_tmpE1F=_tmpE1D.aliasqual;if(_tmpE1F != Cyc_Absyn_Top)goto _LL6D3;
_LL6D2: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp157F;
void*_tmp157E;(_tmp157E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp157F="can't unify evar with heap!",_tag_dyneither(
_tmp157F,sizeof(char),28))),_tag_dyneither(_tmp157E,sizeof(void*),0)));}goto
_LL6CC;_LL6D3: _tmpE20=*_tmpE16;_tmpE21=_tmpE20.kind;if(_tmpE21 != Cyc_Absyn_EffKind)
goto _LL6D5;_LL6D4: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp1582;void*_tmp1581;(_tmp1581=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1582="can't unify evar with {}!",
_tag_dyneither(_tmp1582,sizeof(char),26))),_tag_dyneither(_tmp1581,sizeof(void*),
0)));}goto _LL6CC;_LL6D5:;_LL6D6:{const char*_tmp1587;void*_tmp1586[2];struct Cyc_String_pa_struct
_tmp1585;struct Cyc_String_pa_struct _tmp1584;(_tmp1584.tag=0,((_tmp1584.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp1585.tag=0,((_tmp1585.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp1586[0]=& _tmp1585,((_tmp1586[1]=& _tmp1584,Cyc_Tcutil_terr(loc,((
_tmp1587="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp1587,sizeof(char),52))),_tag_dyneither(_tmp1586,sizeof(void*),2)))))))))))));}
goto _LL6CC;_LL6CC:;}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{void*_tmpE2D=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmpE2F;struct Cyc_List_List*_tmpE30;struct Cyc_Core_Opt*
_tmpE31;struct Cyc_Absyn_Tqual _tmpE32;void*_tmpE33;struct Cyc_List_List*_tmpE34;
_LL6D8: {struct Cyc_Absyn_FnType_struct*_tmpE2E=(struct Cyc_Absyn_FnType_struct*)
_tmpE2D;if(_tmpE2E->tag != 10)goto _LL6DA;else{_tmpE2F=_tmpE2E->f1;_tmpE30=_tmpE2F.tvars;
_tmpE31=_tmpE2F.effect;_tmpE32=_tmpE2F.ret_tqual;_tmpE33=_tmpE2F.ret_typ;_tmpE34=
_tmpE2F.args;}}_LL6D9: fd->tvs=_tmpE30;fd->effect=_tmpE31;{struct Cyc_List_List*
_tmpE35=fd->args;for(0;_tmpE35 != 0;(_tmpE35=_tmpE35->tl,_tmpE34=_tmpE34->tl)){(*((
struct _tuple25*)_tmpE35->hd)).f2=(*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmpE34))->hd)).f2;(*((struct _tuple25*)_tmpE35->hd)).f3=(*((struct
_tuple9*)_tmpE34->hd)).f3;}}fd->ret_tqual=_tmpE32;fd->ret_type=_tmpE33;(fd->ret_tqual).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpE33);
goto _LL6D7;_LL6DA:;_LL6DB: {const char*_tmp158A;void*_tmp1589;(_tmp1589=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp158A="check_fndecl_valid_type: not a FnType",
_tag_dyneither(_tmp158A,sizeof(char),38))),_tag_dyneither(_tmp1589,sizeof(void*),
0)));}_LL6D7:;}{struct _RegionHandle*_tmpE38=Cyc_Tcenv_get_fnrgn(te);{const char*
_tmp158B;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple25*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpE38,(struct _dyneither_ptr*(*)(struct _tuple25*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,((_tmp158B="function declaration has repeated parameter",
_tag_dyneither(_tmp158B,sizeof(char),44))));}{struct Cyc_Core_Opt*_tmp158C;fd->cached_typ=((
_tmp158C=_cycalloc(sizeof(*_tmp158C)),((_tmp158C->v=t,_tmp158C))));};};}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){struct
_RegionHandle*_tmpE3B=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmp158D;
struct Cyc_Tcutil_CVTEnv _tmpE3C=Cyc_Tcutil_check_valid_type(loc,te,((_tmp158D.r=
_tmpE3B,((_tmp158D.kind_env=bound_tvars,((_tmp158D.free_vars=0,((_tmp158D.free_evars=
0,((_tmp158D.generalize_evars=0,((_tmp158D.fn_result=0,_tmp158D)))))))))))),
expected_kind,t);struct Cyc_List_List*_tmpE3D=Cyc_Tcutil_remove_bound_tvars(
_tmpE3B,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmpE3B,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple29*))Cyc_Core_fst,_tmpE3C.free_vars),bound_tvars);struct Cyc_List_List*
_tmpE3E=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple30*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmpE3B,(void*(*)(struct _tuple30*))Cyc_Core_fst,
_tmpE3C.free_evars);{struct Cyc_List_List*fs=_tmpE3D;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpE3F=((struct Cyc_Absyn_Tvar*)fs->hd)->name;const char*
_tmp1592;void*_tmp1591[2];struct Cyc_String_pa_struct _tmp1590;struct Cyc_String_pa_struct
_tmp158F;(_tmp158F.tag=0,((_tmp158F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1590.tag=0,((_tmp1590.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmpE3F),((_tmp1591[0]=& _tmp1590,((
_tmp1591[1]=& _tmp158F,Cyc_Tcutil_terr(loc,((_tmp1592="unbound type variable %s in type %s",
_tag_dyneither(_tmp1592,sizeof(char),36))),_tag_dyneither(_tmp1591,sizeof(void*),
2)))))))))))));}}if(!allow_evars  && _tmpE3E != 0)for(0;_tmpE3E != 0;_tmpE3E=
_tmpE3E->tl){void*e=(void*)_tmpE3E->hd;struct Cyc_Absyn_Kind*_tmpE44=Cyc_Tcutil_typ_kind(
e);struct Cyc_Absyn_Kind _tmpE45;enum Cyc_Absyn_KindQual _tmpE46;enum Cyc_Absyn_AliasQual
_tmpE47;struct Cyc_Absyn_Kind _tmpE48;enum Cyc_Absyn_KindQual _tmpE49;enum Cyc_Absyn_AliasQual
_tmpE4A;struct Cyc_Absyn_Kind _tmpE4B;enum Cyc_Absyn_KindQual _tmpE4C;enum Cyc_Absyn_AliasQual
_tmpE4D;struct Cyc_Absyn_Kind _tmpE4E;enum Cyc_Absyn_KindQual _tmpE4F;_LL6DD:
_tmpE45=*_tmpE44;_tmpE46=_tmpE45.kind;if(_tmpE46 != Cyc_Absyn_RgnKind)goto _LL6DF;
_tmpE47=_tmpE45.aliasqual;if(_tmpE47 != Cyc_Absyn_Unique)goto _LL6DF;_LL6DE: if(!
Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){const char*_tmp1595;void*
_tmp1594;(_tmp1594=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1595="can't unify evar with unique region!",
_tag_dyneither(_tmp1595,sizeof(char),37))),_tag_dyneither(_tmp1594,sizeof(void*),
0)));}goto _LL6DC;_LL6DF: _tmpE48=*_tmpE44;_tmpE49=_tmpE48.kind;if(_tmpE49 != Cyc_Absyn_RgnKind)
goto _LL6E1;_tmpE4A=_tmpE48.aliasqual;if(_tmpE4A != Cyc_Absyn_Aliasable)goto _LL6E1;
_LL6E0: goto _LL6E2;_LL6E1: _tmpE4B=*_tmpE44;_tmpE4C=_tmpE4B.kind;if(_tmpE4C != Cyc_Absyn_RgnKind)
goto _LL6E3;_tmpE4D=_tmpE4B.aliasqual;if(_tmpE4D != Cyc_Absyn_Top)goto _LL6E3;
_LL6E2: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1598;
void*_tmp1597;(_tmp1597=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp1598="can't unify evar with heap!",_tag_dyneither(
_tmp1598,sizeof(char),28))),_tag_dyneither(_tmp1597,sizeof(void*),0)));}goto
_LL6DC;_LL6E3: _tmpE4E=*_tmpE44;_tmpE4F=_tmpE4E.kind;if(_tmpE4F != Cyc_Absyn_EffKind)
goto _LL6E5;_LL6E4: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp159B;void*_tmp159A;(_tmp159A=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp159B="can't unify evar with {}!",
_tag_dyneither(_tmp159B,sizeof(char),26))),_tag_dyneither(_tmp159A,sizeof(void*),
0)));}goto _LL6DC;_LL6E5:;_LL6E6:{const char*_tmp15A0;void*_tmp159F[2];struct Cyc_String_pa_struct
_tmp159E;struct Cyc_String_pa_struct _tmp159D;(_tmp159D.tag=0,((_tmp159D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp159E.tag=0,((_tmp159E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp159F[0]=& _tmp159E,((_tmp159F[1]=& _tmp159D,Cyc_Tcutil_terr(loc,((
_tmp15A0="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp15A0,sizeof(char),52))),_tag_dyneither(_tmp159F,sizeof(void*),2)))))))))))));}
goto _LL6DC;_LL6DC:;}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)tv->identity=Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){for(0;vs != 0;vs=
vs->tl){struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)
vs->hd,(void*)vs2->hd)== 0){const char*_tmp15A5;void*_tmp15A4[2];struct Cyc_String_pa_struct
_tmp15A3;struct Cyc_String_pa_struct _tmp15A2;(_tmp15A2.tag=0,((_tmp15A2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp15A3.tag=
0,((_tmp15A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp15A4[0]=&
_tmp15A3,((_tmp15A4[1]=& _tmp15A2,Cyc_Tcutil_terr(loc,((_tmp15A5="%s: %s",
_tag_dyneither(_tmp15A5,sizeof(char),7))),_tag_dyneither(_tmp15A4,sizeof(void*),
2)))))))))))));}}}}static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct
_dyneither_ptr*s){return*s;}void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr msg){((void(*)(int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr
msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,
msg);}void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*loc,struct Cyc_List_List*
tvs){const char*_tmp15A6;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*
a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(
Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp15A6="duplicate type variable",
_tag_dyneither(_tmp15A6,sizeof(char),24))));}struct _tuple32{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple33{struct Cyc_List_List*f1;void*f2;};struct _tuple34{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){struct Cyc_List_List*
fields=0;{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=
sd_fields->tl){const char*_tmp15A7;if(Cyc_strcmp((struct _dyneither_ptr)*((struct
Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp15A7="",_tag_dyneither(_tmp15A7,
sizeof(char),1))))!= 0){struct _tuple32*_tmp15AA;struct Cyc_List_List*_tmp15A9;
fields=((_tmp15A9=_cycalloc(sizeof(*_tmp15A9)),((_tmp15A9->hd=((_tmp15AA=
_cycalloc(sizeof(*_tmp15AA)),((_tmp15AA->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd,((_tmp15AA->f2=0,_tmp15AA)))))),((_tmp15A9->tl=fields,_tmp15A9))))));}}}
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
const char*_tmp15AC;const char*_tmp15AB;struct _dyneither_ptr aggr_str=aggr_kind == 
Cyc_Absyn_StructA?(_tmp15AC="struct",_tag_dyneither(_tmp15AC,sizeof(char),7)):((
_tmp15AB="union",_tag_dyneither(_tmp15AB,sizeof(char),6)));struct Cyc_List_List*
ans=0;for(0;des != 0;des=des->tl){struct _tuple33 _tmpE64;struct Cyc_List_List*
_tmpE65;void*_tmpE66;struct _tuple33*_tmpE63=(struct _tuple33*)des->hd;_tmpE64=*
_tmpE63;_tmpE65=_tmpE64.f1;_tmpE66=_tmpE64.f2;if(_tmpE65 == 0){struct Cyc_List_List*
_tmpE67=fields;for(0;_tmpE67 != 0;_tmpE67=_tmpE67->tl){if(!(*((struct _tuple32*)
_tmpE67->hd)).f2){(*((struct _tuple32*)_tmpE67->hd)).f2=1;{struct Cyc_Absyn_FieldName_struct*
_tmp15B2;struct Cyc_Absyn_FieldName_struct _tmp15B1;struct Cyc_List_List*_tmp15B0;(*((
struct _tuple33*)des->hd)).f1=(struct Cyc_List_List*)((_tmp15B0=_cycalloc(sizeof(*
_tmp15B0)),((_tmp15B0->hd=(void*)((_tmp15B2=_cycalloc(sizeof(*_tmp15B2)),((
_tmp15B2[0]=((_tmp15B1.tag=1,((_tmp15B1.f1=((*((struct _tuple32*)_tmpE67->hd)).f1)->name,
_tmp15B1)))),_tmp15B2)))),((_tmp15B0->tl=0,_tmp15B0))))));}{struct _tuple34*
_tmp15B5;struct Cyc_List_List*_tmp15B4;ans=((_tmp15B4=_region_malloc(rgn,sizeof(*
_tmp15B4)),((_tmp15B4->hd=((_tmp15B5=_region_malloc(rgn,sizeof(*_tmp15B5)),((
_tmp15B5->f1=(*((struct _tuple32*)_tmpE67->hd)).f1,((_tmp15B5->f2=_tmpE66,
_tmp15B5)))))),((_tmp15B4->tl=ans,_tmp15B4))))));}break;}}if(_tmpE67 == 0){const
char*_tmp15B9;void*_tmp15B8[1];struct Cyc_String_pa_struct _tmp15B7;(_tmp15B7.tag=
0,((_tmp15B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((
_tmp15B8[0]=& _tmp15B7,Cyc_Tcutil_terr(loc,((_tmp15B9="too many arguments to %s",
_tag_dyneither(_tmp15B9,sizeof(char),25))),_tag_dyneither(_tmp15B8,sizeof(void*),
1)))))));}}else{if(_tmpE65->tl != 0){const char*_tmp15BC;void*_tmp15BB;(_tmp15BB=0,
Cyc_Tcutil_terr(loc,((_tmp15BC="multiple designators are not yet supported",
_tag_dyneither(_tmp15BC,sizeof(char),43))),_tag_dyneither(_tmp15BB,sizeof(void*),
0)));}else{void*_tmpE72=(void*)_tmpE65->hd;struct _dyneither_ptr*_tmpE75;_LL6E8: {
struct Cyc_Absyn_ArrayElement_struct*_tmpE73=(struct Cyc_Absyn_ArrayElement_struct*)
_tmpE72;if(_tmpE73->tag != 0)goto _LL6EA;}_LL6E9:{const char*_tmp15C0;void*_tmp15BF[
1];struct Cyc_String_pa_struct _tmp15BE;(_tmp15BE.tag=0,((_tmp15BE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp15BF[0]=& _tmp15BE,Cyc_Tcutil_terr(
loc,((_tmp15C0="array designator used in argument to %s",_tag_dyneither(_tmp15C0,
sizeof(char),40))),_tag_dyneither(_tmp15BF,sizeof(void*),1)))))));}goto _LL6E7;
_LL6EA: {struct Cyc_Absyn_FieldName_struct*_tmpE74=(struct Cyc_Absyn_FieldName_struct*)
_tmpE72;if(_tmpE74->tag != 1)goto _LL6E7;else{_tmpE75=_tmpE74->f1;}}_LL6EB: {
struct Cyc_List_List*_tmpE79=fields;for(0;_tmpE79 != 0;_tmpE79=_tmpE79->tl){if(Cyc_strptrcmp(
_tmpE75,((*((struct _tuple32*)_tmpE79->hd)).f1)->name)== 0){if((*((struct _tuple32*)
_tmpE79->hd)).f2){const char*_tmp15C4;void*_tmp15C3[1];struct Cyc_String_pa_struct
_tmp15C2;(_tmp15C2.tag=0,((_tmp15C2.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmpE75),((_tmp15C3[0]=& _tmp15C2,Cyc_Tcutil_terr(loc,((_tmp15C4="member %s has already been used as an argument",
_tag_dyneither(_tmp15C4,sizeof(char),47))),_tag_dyneither(_tmp15C3,sizeof(void*),
1)))))));}(*((struct _tuple32*)_tmpE79->hd)).f2=1;{struct _tuple34*_tmp15C7;struct
Cyc_List_List*_tmp15C6;ans=((_tmp15C6=_region_malloc(rgn,sizeof(*_tmp15C6)),((
_tmp15C6->hd=((_tmp15C7=_region_malloc(rgn,sizeof(*_tmp15C7)),((_tmp15C7->f1=(*((
struct _tuple32*)_tmpE79->hd)).f1,((_tmp15C7->f2=_tmpE66,_tmp15C7)))))),((
_tmp15C6->tl=ans,_tmp15C6))))));}break;}}if(_tmpE79 == 0){const char*_tmp15CB;void*
_tmp15CA[1];struct Cyc_String_pa_struct _tmp15C9;(_tmp15C9.tag=0,((_tmp15C9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE75),((_tmp15CA[0]=& _tmp15C9,
Cyc_Tcutil_terr(loc,((_tmp15CB="bad field designator %s",_tag_dyneither(_tmp15CB,
sizeof(char),24))),_tag_dyneither(_tmp15CA,sizeof(void*),1)))))));}goto _LL6E7;}
_LL6E7:;}}}if(aggr_kind == Cyc_Absyn_StructA)for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple32*)fields->hd)).f2){{const char*_tmp15CE;void*_tmp15CD;(
_tmp15CD=0,Cyc_Tcutil_terr(loc,((_tmp15CE="too few arguments to struct",
_tag_dyneither(_tmp15CE,sizeof(char),28))),_tag_dyneither(_tmp15CD,sizeof(void*),
0)));}break;}}else{int found=0;for(0;fields != 0;fields=fields->tl){if((*((struct
_tuple32*)fields->hd)).f2){if(found){const char*_tmp15D1;void*_tmp15D0;(_tmp15D0=
0,Cyc_Tcutil_terr(loc,((_tmp15D1="only one member of a union is allowed",
_tag_dyneither(_tmp15D1,sizeof(char),38))),_tag_dyneither(_tmp15D0,sizeof(void*),
0)));}found=1;}}if(!found){const char*_tmp15D4;void*_tmp15D3;(_tmp15D3=0,Cyc_Tcutil_terr(
loc,((_tmp15D4="missing member for union",_tag_dyneither(_tmp15D4,sizeof(char),
25))),_tag_dyneither(_tmp15D3,sizeof(void*),0)));}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);};}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpE8A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpE8C;void*_tmpE8D;struct Cyc_Absyn_PtrAtts _tmpE8E;union Cyc_Absyn_Constraint*
_tmpE8F;_LL6ED: {struct Cyc_Absyn_PointerType_struct*_tmpE8B=(struct Cyc_Absyn_PointerType_struct*)
_tmpE8A;if(_tmpE8B->tag != 5)goto _LL6EF;else{_tmpE8C=_tmpE8B->f1;_tmpE8D=_tmpE8C.elt_typ;
_tmpE8E=_tmpE8C.ptr_atts;_tmpE8F=_tmpE8E.bounds;}}_LL6EE: {void*_tmpE90=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpE8F);_LL6F2: {struct Cyc_Absyn_DynEither_b_struct*_tmpE91=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpE90;if(_tmpE91->tag != 0)goto _LL6F4;}_LL6F3:*elt_typ_dest=_tmpE8D;return 1;
_LL6F4:;_LL6F5: return 0;_LL6F1:;}_LL6EF:;_LL6F0: return 0;_LL6EC:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpE92=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpE94;void*_tmpE95;struct Cyc_Absyn_PtrAtts _tmpE96;union Cyc_Absyn_Constraint*
_tmpE97;_LL6F7: {struct Cyc_Absyn_PointerType_struct*_tmpE93=(struct Cyc_Absyn_PointerType_struct*)
_tmpE92;if(_tmpE93->tag != 5)goto _LL6F9;else{_tmpE94=_tmpE93->f1;_tmpE95=_tmpE94.elt_typ;
_tmpE96=_tmpE94.ptr_atts;_tmpE97=_tmpE96.zero_term;}}_LL6F8:*elt_typ_dest=
_tmpE95;return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpE97);_LL6F9:;_LL6FA: return 0;_LL6F6:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,
void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpE98=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpE9A;void*_tmpE9B;struct Cyc_Absyn_PtrAtts _tmpE9C;
union Cyc_Absyn_Constraint*_tmpE9D;union Cyc_Absyn_Constraint*_tmpE9E;struct Cyc_Absyn_ArrayInfo
_tmpEA0;void*_tmpEA1;struct Cyc_Absyn_Tqual _tmpEA2;struct Cyc_Absyn_Exp*_tmpEA3;
union Cyc_Absyn_Constraint*_tmpEA4;_LL6FC: {struct Cyc_Absyn_PointerType_struct*
_tmpE99=(struct Cyc_Absyn_PointerType_struct*)_tmpE98;if(_tmpE99->tag != 5)goto
_LL6FE;else{_tmpE9A=_tmpE99->f1;_tmpE9B=_tmpE9A.elt_typ;_tmpE9C=_tmpE9A.ptr_atts;
_tmpE9D=_tmpE9C.bounds;_tmpE9E=_tmpE9C.zero_term;}}_LL6FD: if(((int(*)(int y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE9E)){*ptr_type=t;*elt_type=
_tmpE9B;{void*_tmpEA5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmpE9D);_LL703: {struct Cyc_Absyn_DynEither_b_struct*
_tmpEA6=(struct Cyc_Absyn_DynEither_b_struct*)_tmpEA5;if(_tmpEA6->tag != 0)goto
_LL705;}_LL704:*is_dyneither=1;goto _LL702;_LL705:;_LL706:*is_dyneither=0;goto
_LL702;_LL702:;}return 1;}else{return 0;}_LL6FE: {struct Cyc_Absyn_ArrayType_struct*
_tmpE9F=(struct Cyc_Absyn_ArrayType_struct*)_tmpE98;if(_tmpE9F->tag != 9)goto
_LL700;else{_tmpEA0=_tmpE9F->f1;_tmpEA1=_tmpEA0.elt_type;_tmpEA2=_tmpEA0.tq;
_tmpEA3=_tmpEA0.num_elts;_tmpEA4=_tmpEA0.zero_term;}}_LL6FF: if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEA4)){*elt_type=_tmpEA1;*
is_dyneither=0;{struct Cyc_Absyn_PointerType_struct _tmp15E9;struct Cyc_Absyn_PtrAtts
_tmp15E8;struct Cyc_Absyn_Upper_b_struct _tmp15E7;struct Cyc_Absyn_Upper_b_struct*
_tmp15E6;struct Cyc_Absyn_PtrInfo _tmp15E5;struct Cyc_Absyn_PointerType_struct*
_tmp15E4;*ptr_type=(void*)((_tmp15E4=_cycalloc(sizeof(*_tmp15E4)),((_tmp15E4[0]=((
_tmp15E9.tag=5,((_tmp15E9.f1=((_tmp15E5.elt_typ=_tmpEA1,((_tmp15E5.elt_tq=
_tmpEA2,((_tmp15E5.ptr_atts=((_tmp15E8.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((
_tmp15E8.nullable=Cyc_Absyn_false_conref,((_tmp15E8.bounds=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp15E6=_cycalloc(sizeof(*_tmp15E6)),((
_tmp15E6[0]=((_tmp15E7.tag=1,((_tmp15E7.f1=(struct Cyc_Absyn_Exp*)_check_null(
_tmpEA3),_tmp15E7)))),_tmp15E6))))),((_tmp15E8.zero_term=_tmpEA4,((_tmp15E8.ptrloc=
0,_tmp15E8)))))))))),_tmp15E5)))))),_tmp15E9)))),_tmp15E4))));}return 1;}else{
return 0;}_LL700:;_LL701: return 0;_LL6FB:;}int Cyc_Tcutil_is_zero_ptr_deref(struct
Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpEAD=e1->r;
struct Cyc_Absyn_Exp*_tmpEB0;struct Cyc_Absyn_Exp*_tmpEB2;struct Cyc_Absyn_Exp*
_tmpEB4;struct Cyc_Absyn_Exp*_tmpEB6;struct Cyc_Absyn_Exp*_tmpEB8;struct Cyc_Absyn_Exp*
_tmpEBA;_LL708: {struct Cyc_Absyn_Cast_e_struct*_tmpEAE=(struct Cyc_Absyn_Cast_e_struct*)
_tmpEAD;if(_tmpEAE->tag != 15)goto _LL70A;}_LL709: {const char*_tmp15ED;void*
_tmp15EC[1];struct Cyc_String_pa_struct _tmp15EB;(_tmp15EB.tag=0,((_tmp15EB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp15EC[0]=& _tmp15EB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp15ED="we have a cast in a lhs:  %s",_tag_dyneither(
_tmp15ED,sizeof(char),29))),_tag_dyneither(_tmp15EC,sizeof(void*),1)))))));}
_LL70A: {struct Cyc_Absyn_Deref_e_struct*_tmpEAF=(struct Cyc_Absyn_Deref_e_struct*)
_tmpEAD;if(_tmpEAF->tag != 21)goto _LL70C;else{_tmpEB0=_tmpEAF->f1;}}_LL70B:
_tmpEB2=_tmpEB0;goto _LL70D;_LL70C: {struct Cyc_Absyn_Subscript_e_struct*_tmpEB1=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpEAD;if(_tmpEB1->tag != 24)goto _LL70E;
else{_tmpEB2=_tmpEB1->f1;}}_LL70D: return Cyc_Tcutil_is_zero_ptr_type((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpEB2->topt))->v,ptr_type,is_dyneither,
elt_type);_LL70E: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpEB3=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpEAD;if(_tmpEB3->tag != 23)goto _LL710;else{_tmpEB4=_tmpEB3->f1;}}_LL70F:
_tmpEB6=_tmpEB4;goto _LL711;_LL710: {struct Cyc_Absyn_AggrMember_e_struct*_tmpEB5=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmpEAD;if(_tmpEB5->tag != 22)goto _LL712;
else{_tmpEB6=_tmpEB5->f1;}}_LL711: if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct
Cyc_Core_Opt*)_check_null(_tmpEB6->topt))->v,ptr_type,is_dyneither,elt_type)){
const char*_tmp15F1;void*_tmp15F0[1];struct Cyc_String_pa_struct _tmp15EF;(_tmp15EF.tag=
0,((_tmp15EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp15F0[0]=& _tmp15EF,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15F1="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp15F1,sizeof(char),51))),_tag_dyneither(_tmp15F0,sizeof(void*),
1)))))));}return 0;_LL712: {struct Cyc_Absyn_Instantiate_e_struct*_tmpEB7=(struct
Cyc_Absyn_Instantiate_e_struct*)_tmpEAD;if(_tmpEB7->tag != 14)goto _LL714;else{
_tmpEB8=_tmpEB7->f1;}}_LL713: _tmpEBA=_tmpEB8;goto _LL715;_LL714: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpEB9=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpEAD;if(_tmpEB9->tag != 13)
goto _LL716;else{_tmpEBA=_tmpEB9->f1;}}_LL715: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpEBA->topt))->v,ptr_type,is_dyneither,
elt_type)){const char*_tmp15F5;void*_tmp15F4[1];struct Cyc_String_pa_struct
_tmp15F3;(_tmp15F3.tag=0,((_tmp15F3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp15F4[0]=& _tmp15F3,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15F5="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp15F5,sizeof(char),49))),_tag_dyneither(_tmp15F4,sizeof(void*),
1)))))));}return 0;_LL716: {struct Cyc_Absyn_Var_e_struct*_tmpEBB=(struct Cyc_Absyn_Var_e_struct*)
_tmpEAD;if(_tmpEBB->tag != 1)goto _LL718;}_LL717: return 0;_LL718:;_LL719: {const
char*_tmp15F9;void*_tmp15F8[1];struct Cyc_String_pa_struct _tmp15F7;(_tmp15F7.tag=
0,((_tmp15F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp15F8[0]=& _tmp15F7,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15F9="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp15F9,sizeof(char),39))),_tag_dyneither(_tmp15F8,sizeof(void*),
1)))))));}_LL707:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t){void*ignore=(void*)&
Cyc_Absyn_VoidType_val;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){void*_tmpEC8=Cyc_Tcutil_compress(
r);struct Cyc_Absyn_Tvar*_tmpECB;_LL71B: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpEC9=(struct Cyc_Absyn_UniqueRgn_struct*)_tmpEC8;if(_tmpEC9->tag != 22)goto
_LL71D;}_LL71C: return 1;_LL71D: {struct Cyc_Absyn_VarType_struct*_tmpECA=(struct
Cyc_Absyn_VarType_struct*)_tmpEC8;if(_tmpECA->tag != 2)goto _LL71F;else{_tmpECB=
_tmpECA->f1;}}_LL71E: {struct Cyc_Absyn_Kind _tmpECD;enum Cyc_Absyn_KindQual
_tmpECE;enum Cyc_Absyn_AliasQual _tmpECF;struct Cyc_Absyn_Kind*_tmpECC=Cyc_Tcutil_tvar_kind(
_tmpECB,& Cyc_Tcutil_rk);_tmpECD=*_tmpECC;_tmpECE=_tmpECD.kind;_tmpECF=_tmpECD.aliasqual;
if(_tmpECE == Cyc_Absyn_RgnKind  && (_tmpECF == Cyc_Absyn_Unique  || _tmpECF == Cyc_Absyn_Top
 && !must_be_unique)){void*_tmpED0=Cyc_Absyn_compress_kb(_tmpECB->kind);struct
Cyc_Core_Opt*_tmpED2;struct Cyc_Core_Opt**_tmpED3;struct Cyc_Absyn_Kind*_tmpED4;
struct Cyc_Absyn_Kind _tmpED5;enum Cyc_Absyn_KindQual _tmpED6;enum Cyc_Absyn_AliasQual
_tmpED7;_LL722: {struct Cyc_Absyn_Less_kb_struct*_tmpED1=(struct Cyc_Absyn_Less_kb_struct*)
_tmpED0;if(_tmpED1->tag != 2)goto _LL724;else{_tmpED2=_tmpED1->f1;_tmpED3=(struct
Cyc_Core_Opt**)& _tmpED1->f1;_tmpED4=_tmpED1->f2;_tmpED5=*_tmpED4;_tmpED6=_tmpED5.kind;
if(_tmpED6 != Cyc_Absyn_RgnKind)goto _LL724;_tmpED7=_tmpED5.aliasqual;if(_tmpED7 != 
Cyc_Absyn_Top)goto _LL724;}}_LL723:{struct Cyc_Absyn_Less_kb_struct*_tmp15FF;
struct Cyc_Absyn_Less_kb_struct _tmp15FE;struct Cyc_Core_Opt*_tmp15FD;*_tmpED3=((
_tmp15FD=_cycalloc(sizeof(*_tmp15FD)),((_tmp15FD->v=(void*)((_tmp15FF=_cycalloc(
sizeof(*_tmp15FF)),((_tmp15FF[0]=((_tmp15FE.tag=2,((_tmp15FE.f1=0,((_tmp15FE.f2=&
Cyc_Tcutil_rk,_tmp15FE)))))),_tmp15FF)))),_tmp15FD))));}return 0;_LL724:;_LL725:
return 1;_LL721:;}return 0;}_LL71F:;_LL720: return 0;_LL71A:;}static int Cyc_Tcutil_is_noalias_pointer_aux(
void*t,int must_be_unique){void*_tmpEDB=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpEDD;struct Cyc_Absyn_PtrAtts _tmpEDE;void*_tmpEDF;struct Cyc_Absyn_Tvar*_tmpEE1;
_LL727: {struct Cyc_Absyn_PointerType_struct*_tmpEDC=(struct Cyc_Absyn_PointerType_struct*)
_tmpEDB;if(_tmpEDC->tag != 5)goto _LL729;else{_tmpEDD=_tmpEDC->f1;_tmpEDE=_tmpEDD.ptr_atts;
_tmpEDF=_tmpEDE.rgn;}}_LL728: return Cyc_Tcutil_is_noalias_region(_tmpEDF,
must_be_unique);_LL729: {struct Cyc_Absyn_VarType_struct*_tmpEE0=(struct Cyc_Absyn_VarType_struct*)
_tmpEDB;if(_tmpEE0->tag != 2)goto _LL72B;else{_tmpEE1=_tmpEE0->f1;}}_LL72A: {
struct Cyc_Absyn_Kind _tmpEE3;enum Cyc_Absyn_KindQual _tmpEE4;enum Cyc_Absyn_AliasQual
_tmpEE5;struct Cyc_Absyn_Kind*_tmpEE2=Cyc_Tcutil_tvar_kind(_tmpEE1,& Cyc_Tcutil_bk);
_tmpEE3=*_tmpEE2;_tmpEE4=_tmpEE3.kind;_tmpEE5=_tmpEE3.aliasqual;switch(_tmpEE4){
case Cyc_Absyn_BoxKind: _LL72D: goto _LL72E;case Cyc_Absyn_AnyKind: _LL72E: goto _LL72F;
case Cyc_Absyn_MemKind: _LL72F: if(_tmpEE5 == Cyc_Absyn_Unique  || _tmpEE5 == Cyc_Absyn_Top){
void*_tmpEE6=Cyc_Absyn_compress_kb(_tmpEE1->kind);struct Cyc_Core_Opt*_tmpEE8;
struct Cyc_Core_Opt**_tmpEE9;struct Cyc_Absyn_Kind*_tmpEEA;struct Cyc_Absyn_Kind
_tmpEEB;enum Cyc_Absyn_KindQual _tmpEEC;enum Cyc_Absyn_AliasQual _tmpEED;_LL732: {
struct Cyc_Absyn_Less_kb_struct*_tmpEE7=(struct Cyc_Absyn_Less_kb_struct*)_tmpEE6;
if(_tmpEE7->tag != 2)goto _LL734;else{_tmpEE8=_tmpEE7->f1;_tmpEE9=(struct Cyc_Core_Opt**)&
_tmpEE7->f1;_tmpEEA=_tmpEE7->f2;_tmpEEB=*_tmpEEA;_tmpEEC=_tmpEEB.kind;_tmpEED=
_tmpEEB.aliasqual;if(_tmpEED != Cyc_Absyn_Top)goto _LL734;}}_LL733:{struct Cyc_Absyn_Less_kb_struct*
_tmp1609;struct Cyc_Absyn_Kind*_tmp1608;struct Cyc_Absyn_Less_kb_struct _tmp1607;
struct Cyc_Core_Opt*_tmp1606;*_tmpEE9=((_tmp1606=_cycalloc(sizeof(*_tmp1606)),((
_tmp1606->v=(void*)((_tmp1609=_cycalloc(sizeof(*_tmp1609)),((_tmp1609[0]=((
_tmp1607.tag=2,((_tmp1607.f1=0,((_tmp1607.f2=((_tmp1608=_cycalloc_atomic(sizeof(*
_tmp1608)),((_tmp1608->kind=_tmpEEC,((_tmp1608->aliasqual=Cyc_Absyn_Aliasable,
_tmp1608)))))),_tmp1607)))))),_tmp1609)))),_tmp1606))));}return 0;_LL734:;_LL735:
return 1;_LL731:;}return 0;default: _LL730: return 0;}}_LL72B:;_LL72C: return 0;_LL726:;}
int Cyc_Tcutil_is_noalias_pointer(void*t){return Cyc_Tcutil_is_noalias_pointer_aux(
t,0);}int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpEF2=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(_tmpEF2))
return 1;{void*_tmpEF3=_tmpEF2;struct Cyc_List_List*_tmpEF5;struct Cyc_Absyn_AggrInfo
_tmpEF7;union Cyc_Absyn_AggrInfoU _tmpEF8;struct Cyc_Absyn_Aggrdecl**_tmpEF9;struct
Cyc_List_List*_tmpEFA;struct Cyc_List_List*_tmpEFC;struct Cyc_Absyn_AggrInfo
_tmpEFE;union Cyc_Absyn_AggrInfoU _tmpEFF;struct _tuple4 _tmpF00;struct Cyc_Absyn_DatatypeInfo
_tmpF02;union Cyc_Absyn_DatatypeInfoU _tmpF03;struct Cyc_List_List*_tmpF04;struct
Cyc_Absyn_DatatypeFieldInfo _tmpF06;union Cyc_Absyn_DatatypeFieldInfoU _tmpF07;
struct Cyc_List_List*_tmpF08;_LL738: {struct Cyc_Absyn_TupleType_struct*_tmpEF4=(
struct Cyc_Absyn_TupleType_struct*)_tmpEF3;if(_tmpEF4->tag != 11)goto _LL73A;else{
_tmpEF5=_tmpEF4->f1;}}_LL739: while(_tmpEF5 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,(*((struct _tuple11*)_tmpEF5->hd)).f2))return 1;_tmpEF5=_tmpEF5->tl;}return 0;
_LL73A: {struct Cyc_Absyn_AggrType_struct*_tmpEF6=(struct Cyc_Absyn_AggrType_struct*)
_tmpEF3;if(_tmpEF6->tag != 12)goto _LL73C;else{_tmpEF7=_tmpEF6->f1;_tmpEF8=_tmpEF7.aggr_info;
if((_tmpEF8.KnownAggr).tag != 2)goto _LL73C;_tmpEF9=(struct Cyc_Absyn_Aggrdecl**)(
_tmpEF8.KnownAggr).val;_tmpEFA=_tmpEF7.targs;}}_LL73B: if((*_tmpEF9)->impl == 0)
return 0;else{struct Cyc_List_List*_tmpF09=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmpEF9)->tvs,_tmpEFA);struct Cyc_List_List*_tmpF0A=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpEF9)->impl))->fields;void*t;
while(_tmpF0A != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpF09,((struct Cyc_Absyn_Aggrfield*)
_tmpF0A->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpF0A=_tmpF0A->tl;}return 0;}_LL73C: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpEFB=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpEF3;if(_tmpEFB->tag != 13)goto
_LL73E;else{_tmpEFC=_tmpEFB->f2;}}_LL73D: while(_tmpEFC != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,((struct Cyc_Absyn_Aggrfield*)_tmpEFC->hd)->type))return 1;_tmpEFC=_tmpEFC->tl;}
return 0;_LL73E: {struct Cyc_Absyn_AggrType_struct*_tmpEFD=(struct Cyc_Absyn_AggrType_struct*)
_tmpEF3;if(_tmpEFD->tag != 12)goto _LL740;else{_tmpEFE=_tmpEFD->f1;_tmpEFF=_tmpEFE.aggr_info;
if((_tmpEFF.UnknownAggr).tag != 1)goto _LL740;_tmpF00=(struct _tuple4)(_tmpEFF.UnknownAggr).val;}}
_LL73F: {const char*_tmp160C;void*_tmp160B;(_tmp160B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp160C="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp160C,sizeof(char),36))),_tag_dyneither(_tmp160B,sizeof(void*),
0)));}_LL740: {struct Cyc_Absyn_DatatypeType_struct*_tmpF01=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpEF3;if(_tmpF01->tag != 3)goto _LL742;else{_tmpF02=_tmpF01->f1;_tmpF03=_tmpF02.datatype_info;
_tmpF04=_tmpF02.targs;}}_LL741: {union Cyc_Absyn_DatatypeInfoU _tmpF0D=_tmpF03;
struct Cyc_Absyn_UnknownDatatypeInfo _tmpF0E;struct _tuple2*_tmpF0F;int _tmpF10;
struct Cyc_Absyn_Datatypedecl**_tmpF11;struct Cyc_Absyn_Datatypedecl*_tmpF12;
struct Cyc_Absyn_Datatypedecl _tmpF13;struct Cyc_List_List*_tmpF14;struct Cyc_Core_Opt*
_tmpF15;_LL747: if((_tmpF0D.UnknownDatatype).tag != 1)goto _LL749;_tmpF0E=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpF0D.UnknownDatatype).val;_tmpF0F=_tmpF0E.name;
_tmpF10=_tmpF0E.is_extensible;_LL748: {const char*_tmp160F;void*_tmp160E;(
_tmp160E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp160F="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp160F,
sizeof(char),40))),_tag_dyneither(_tmp160E,sizeof(void*),0)));}_LL749: if((
_tmpF0D.KnownDatatype).tag != 2)goto _LL746;_tmpF11=(struct Cyc_Absyn_Datatypedecl**)(
_tmpF0D.KnownDatatype).val;_tmpF12=*_tmpF11;_tmpF13=*_tmpF12;_tmpF14=_tmpF13.tvs;
_tmpF15=_tmpF13.fields;_LL74A: return 0;_LL746:;}_LL742: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpF05=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpEF3;if(_tmpF05->tag != 4)
goto _LL744;else{_tmpF06=_tmpF05->f1;_tmpF07=_tmpF06.field_info;_tmpF08=_tmpF06.targs;}}
_LL743: {union Cyc_Absyn_DatatypeFieldInfoU _tmpF18=_tmpF07;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpF19;struct _tuple3 _tmpF1A;struct Cyc_Absyn_Datatypedecl*_tmpF1B;struct Cyc_Absyn_Datatypefield*
_tmpF1C;_LL74C: if((_tmpF18.UnknownDatatypefield).tag != 1)goto _LL74E;_tmpF19=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpF18.UnknownDatatypefield).val;
_LL74D: {const char*_tmp1612;void*_tmp1611;(_tmp1611=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1612="got unknown datatype field in is_noalias_aggr",
_tag_dyneither(_tmp1612,sizeof(char),46))),_tag_dyneither(_tmp1611,sizeof(void*),
0)));}_LL74E: if((_tmpF18.KnownDatatypefield).tag != 2)goto _LL74B;_tmpF1A=(struct
_tuple3)(_tmpF18.KnownDatatypefield).val;_tmpF1B=_tmpF1A.f1;_tmpF1C=_tmpF1A.f2;
_LL74F: {struct Cyc_List_List*_tmpF1F=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpF1B->tvs,_tmpF08);struct Cyc_List_List*_tmpF20=
_tmpF1C->typs;while(_tmpF20 != 0){_tmpEF2=Cyc_Tcutil_rsubstitute(rgn,_tmpF1F,(*((
struct _tuple11*)_tmpF20->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,
_tmpEF2))return 1;_tmpF20=_tmpF20->tl;}return 0;}_LL74B:;}_LL744:;_LL745: return 0;
_LL737:;};}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,
struct _dyneither_ptr*f){void*_tmpF21=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpF23;union Cyc_Absyn_AggrInfoU _tmpF24;struct Cyc_Absyn_Aggrdecl**_tmpF25;struct
Cyc_Absyn_Aggrdecl*_tmpF26;struct Cyc_List_List*_tmpF27;struct Cyc_List_List*
_tmpF29;_LL751: {struct Cyc_Absyn_AggrType_struct*_tmpF22=(struct Cyc_Absyn_AggrType_struct*)
_tmpF21;if(_tmpF22->tag != 12)goto _LL753;else{_tmpF23=_tmpF22->f1;_tmpF24=_tmpF23.aggr_info;
if((_tmpF24.KnownAggr).tag != 2)goto _LL753;_tmpF25=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF24.KnownAggr).val;_tmpF26=*_tmpF25;_tmpF27=_tmpF23.targs;}}_LL752: _tmpF29=
_tmpF26->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF26->impl))->fields;
goto _LL754;_LL753: {struct Cyc_Absyn_AnonAggrType_struct*_tmpF28=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpF21;if(_tmpF28->tag != 13)goto _LL755;else{_tmpF29=_tmpF28->f2;}}_LL754: {
struct Cyc_Absyn_Aggrfield*_tmpF2A=Cyc_Absyn_lookup_field(_tmpF29,f);{struct Cyc_Absyn_Aggrfield*
_tmpF2B=_tmpF2A;struct Cyc_Absyn_Aggrfield _tmpF2C;void*_tmpF2D;_LL758: if(_tmpF2B
!= 0)goto _LL75A;_LL759: {const char*_tmp1615;void*_tmp1614;(_tmp1614=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1615="is_noalias_field: missing field",
_tag_dyneither(_tmp1615,sizeof(char),32))),_tag_dyneither(_tmp1614,sizeof(void*),
0)));}_LL75A: if(_tmpF2B == 0)goto _LL757;_tmpF2C=*_tmpF2B;_tmpF2D=_tmpF2C.type;
_LL75B: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpF2D);_LL757:;}return 0;}
_LL755:;_LL756: {const char*_tmp1619;void*_tmp1618[1];struct Cyc_String_pa_struct
_tmp1617;(_tmp1617.tag=0,((_tmp1617.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1618[0]=& _tmp1617,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1619="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp1619,sizeof(char),36))),_tag_dyneither(_tmp1618,sizeof(void*),
1)))))));}_LL750:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e){void*_tmpF33=e->r;void*_tmpF35;struct Cyc_Absyn_Exp*
_tmpF38;struct Cyc_Absyn_Exp*_tmpF3A;struct Cyc_Absyn_Exp*_tmpF3C;struct
_dyneither_ptr*_tmpF3D;struct Cyc_Absyn_Exp*_tmpF3F;struct Cyc_Absyn_Exp*_tmpF40;
struct Cyc_Absyn_Exp*_tmpF42;struct Cyc_Absyn_Exp*_tmpF43;struct Cyc_Absyn_Exp*
_tmpF45;struct Cyc_Absyn_Exp*_tmpF47;struct Cyc_Absyn_Stmt*_tmpF49;_LL75D: {struct
Cyc_Absyn_Var_e_struct*_tmpF34=(struct Cyc_Absyn_Var_e_struct*)_tmpF33;if(_tmpF34->tag
!= 1)goto _LL75F;else{_tmpF35=(void*)_tmpF34->f2;{struct Cyc_Absyn_Global_b_struct*
_tmpF36=(struct Cyc_Absyn_Global_b_struct*)_tmpF35;if(_tmpF36->tag != 1)goto _LL75F;};}}
_LL75E: return 0;_LL75F: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpF37=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpF33;if(_tmpF37->tag != 23)goto _LL761;else{_tmpF38=_tmpF37->f1;}}_LL760:
_tmpF3A=_tmpF38;goto _LL762;_LL761: {struct Cyc_Absyn_Deref_e_struct*_tmpF39=(
struct Cyc_Absyn_Deref_e_struct*)_tmpF33;if(_tmpF39->tag != 21)goto _LL763;else{
_tmpF3A=_tmpF39->f1;}}_LL762: return Cyc_Tcutil_is_noalias_pointer_aux((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpF3A->topt))->v,1) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpF3A);_LL763: {struct Cyc_Absyn_AggrMember_e_struct*_tmpF3B=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmpF33;if(_tmpF3B->tag != 22)goto _LL765;else{_tmpF3C=_tmpF3B->f1;_tmpF3D=_tmpF3B->f2;}}
_LL764: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF3C);_LL765: {struct Cyc_Absyn_Subscript_e_struct*
_tmpF3E=(struct Cyc_Absyn_Subscript_e_struct*)_tmpF33;if(_tmpF3E->tag != 24)goto
_LL767;else{_tmpF3F=_tmpF3E->f1;_tmpF40=_tmpF3E->f2;}}_LL766: {void*_tmpF4A=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF3F->topt))->v);_LL772: {struct Cyc_Absyn_TupleType_struct*
_tmpF4B=(struct Cyc_Absyn_TupleType_struct*)_tmpF4A;if(_tmpF4B->tag != 11)goto
_LL774;}_LL773: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF3F);_LL774:;_LL775:
return 0;_LL771:;}_LL767: {struct Cyc_Absyn_Conditional_e_struct*_tmpF41=(struct
Cyc_Absyn_Conditional_e_struct*)_tmpF33;if(_tmpF41->tag != 6)goto _LL769;else{
_tmpF42=_tmpF41->f2;_tmpF43=_tmpF41->f3;}}_LL768: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpF42) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpF43);_LL769: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpF44=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpF33;if(_tmpF44->tag != 9)goto _LL76B;
else{_tmpF45=_tmpF44->f2;}}_LL76A: _tmpF47=_tmpF45;goto _LL76C;_LL76B: {struct Cyc_Absyn_Cast_e_struct*
_tmpF46=(struct Cyc_Absyn_Cast_e_struct*)_tmpF33;if(_tmpF46->tag != 15)goto _LL76D;
else{_tmpF47=_tmpF46->f2;}}_LL76C: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF47);
_LL76D: {struct Cyc_Absyn_StmtExp_e_struct*_tmpF48=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpF33;if(_tmpF48->tag != 37)goto _LL76F;else{_tmpF49=_tmpF48->f1;}}_LL76E: while(
1){void*_tmpF4C=_tmpF49->r;struct Cyc_Absyn_Stmt*_tmpF4E;struct Cyc_Absyn_Stmt*
_tmpF4F;struct Cyc_Absyn_Decl*_tmpF51;struct Cyc_Absyn_Stmt*_tmpF52;struct Cyc_Absyn_Exp*
_tmpF54;_LL777: {struct Cyc_Absyn_Seq_s_struct*_tmpF4D=(struct Cyc_Absyn_Seq_s_struct*)
_tmpF4C;if(_tmpF4D->tag != 2)goto _LL779;else{_tmpF4E=_tmpF4D->f1;_tmpF4F=_tmpF4D->f2;}}
_LL778: _tmpF49=_tmpF4F;goto _LL776;_LL779: {struct Cyc_Absyn_Decl_s_struct*_tmpF50=(
struct Cyc_Absyn_Decl_s_struct*)_tmpF4C;if(_tmpF50->tag != 12)goto _LL77B;else{
_tmpF51=_tmpF50->f1;_tmpF52=_tmpF50->f2;}}_LL77A: _tmpF49=_tmpF52;goto _LL776;
_LL77B: {struct Cyc_Absyn_Exp_s_struct*_tmpF53=(struct Cyc_Absyn_Exp_s_struct*)
_tmpF4C;if(_tmpF53->tag != 1)goto _LL77D;else{_tmpF54=_tmpF53->f1;}}_LL77C: return
Cyc_Tcutil_is_noalias_path_aux(r,_tmpF54);_LL77D:;_LL77E: {const char*_tmp161C;
void*_tmp161B;(_tmp161B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp161C="is_noalias_stmt_exp: ill-formed StmtExp",
_tag_dyneither(_tmp161C,sizeof(char),40))),_tag_dyneither(_tmp161B,sizeof(void*),
0)));}_LL776:;}_LL76F:;_LL770: return 1;_LL75C:;}int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(
r,e);}struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){struct _tuple17 _tmp161D;struct _tuple17 bogus_ans=(_tmp161D.f1=0,((_tmp161D.f2=(
void*)& Cyc_Absyn_HeapRgn_val,_tmp161D)));void*_tmpF57=e->r;struct _tuple2*_tmpF59;
void*_tmpF5A;struct Cyc_Absyn_Exp*_tmpF5C;struct _dyneither_ptr*_tmpF5D;int _tmpF5E;
struct Cyc_Absyn_Exp*_tmpF60;struct _dyneither_ptr*_tmpF61;int _tmpF62;struct Cyc_Absyn_Exp*
_tmpF64;struct Cyc_Absyn_Exp*_tmpF66;struct Cyc_Absyn_Exp*_tmpF67;_LL780: {struct
Cyc_Absyn_Var_e_struct*_tmpF58=(struct Cyc_Absyn_Var_e_struct*)_tmpF57;if(_tmpF58->tag
!= 1)goto _LL782;else{_tmpF59=_tmpF58->f1;_tmpF5A=(void*)_tmpF58->f2;}}_LL781: {
void*_tmpF68=_tmpF5A;struct Cyc_Absyn_Vardecl*_tmpF6C;struct Cyc_Absyn_Vardecl*
_tmpF6E;struct Cyc_Absyn_Vardecl*_tmpF70;struct Cyc_Absyn_Vardecl*_tmpF72;_LL78D: {
struct Cyc_Absyn_Unresolved_b_struct*_tmpF69=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmpF68;if(_tmpF69->tag != 0)goto _LL78F;}_LL78E: goto _LL790;_LL78F: {struct Cyc_Absyn_Funname_b_struct*
_tmpF6A=(struct Cyc_Absyn_Funname_b_struct*)_tmpF68;if(_tmpF6A->tag != 2)goto
_LL791;}_LL790: return bogus_ans;_LL791: {struct Cyc_Absyn_Global_b_struct*_tmpF6B=(
struct Cyc_Absyn_Global_b_struct*)_tmpF68;if(_tmpF6B->tag != 1)goto _LL793;else{
_tmpF6C=_tmpF6B->f1;}}_LL792: {void*_tmpF73=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);_LL79A: {struct Cyc_Absyn_ArrayType_struct*
_tmpF74=(struct Cyc_Absyn_ArrayType_struct*)_tmpF73;if(_tmpF74->tag != 9)goto
_LL79C;}_LL79B: {struct _tuple17 _tmp161E;return(_tmp161E.f1=1,((_tmp161E.f2=(void*)&
Cyc_Absyn_HeapRgn_val,_tmp161E)));}_LL79C:;_LL79D: {struct _tuple17 _tmp161F;
return(_tmp161F.f1=(_tmpF6C->tq).real_const,((_tmp161F.f2=(void*)& Cyc_Absyn_HeapRgn_val,
_tmp161F)));}_LL799:;}_LL793: {struct Cyc_Absyn_Local_b_struct*_tmpF6D=(struct Cyc_Absyn_Local_b_struct*)
_tmpF68;if(_tmpF6D->tag != 4)goto _LL795;else{_tmpF6E=_tmpF6D->f1;}}_LL794: {void*
_tmpF77=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL79F: {struct Cyc_Absyn_ArrayType_struct*_tmpF78=(struct Cyc_Absyn_ArrayType_struct*)
_tmpF77;if(_tmpF78->tag != 9)goto _LL7A1;}_LL7A0: {struct _tuple17 _tmp1620;return(
_tmp1620.f1=1,((_tmp1620.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpF6E->rgn))->v,
_tmp1620)));}_LL7A1:;_LL7A2: _tmpF6E->escapes=1;{struct _tuple17 _tmp1621;return(
_tmp1621.f1=(_tmpF6E->tq).real_const,((_tmp1621.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF6E->rgn))->v,_tmp1621)));};_LL79E:;}_LL795: {struct Cyc_Absyn_Pat_b_struct*
_tmpF6F=(struct Cyc_Absyn_Pat_b_struct*)_tmpF68;if(_tmpF6F->tag != 5)goto _LL797;
else{_tmpF70=_tmpF6F->f1;}}_LL796: _tmpF72=_tmpF70;goto _LL798;_LL797: {struct Cyc_Absyn_Param_b_struct*
_tmpF71=(struct Cyc_Absyn_Param_b_struct*)_tmpF68;if(_tmpF71->tag != 3)goto _LL78C;
else{_tmpF72=_tmpF71->f1;}}_LL798: _tmpF72->escapes=1;{struct _tuple17 _tmp1622;
return(_tmp1622.f1=(_tmpF72->tq).real_const,((_tmp1622.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF72->rgn))->v,_tmp1622)));};_LL78C:;}_LL782: {struct Cyc_Absyn_AggrMember_e_struct*
_tmpF5B=(struct Cyc_Absyn_AggrMember_e_struct*)_tmpF57;if(_tmpF5B->tag != 22)goto
_LL784;else{_tmpF5C=_tmpF5B->f1;_tmpF5D=_tmpF5B->f2;_tmpF5E=_tmpF5B->f3;}}_LL783:
if(_tmpF5E)return bogus_ans;{void*_tmpF7C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF5C->topt))->v);struct Cyc_List_List*_tmpF7E;struct Cyc_Absyn_AggrInfo
_tmpF80;union Cyc_Absyn_AggrInfoU _tmpF81;struct Cyc_Absyn_Aggrdecl**_tmpF82;struct
Cyc_Absyn_Aggrdecl*_tmpF83;_LL7A4: {struct Cyc_Absyn_AnonAggrType_struct*_tmpF7D=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmpF7C;if(_tmpF7D->tag != 13)goto _LL7A6;
else{_tmpF7E=_tmpF7D->f2;}}_LL7A5: {struct Cyc_Absyn_Aggrfield*_tmpF84=Cyc_Absyn_lookup_field(
_tmpF7E,_tmpF5D);if(_tmpF84 != 0  && _tmpF84->width == 0){struct _tuple17 _tmp1623;
return(_tmp1623.f1=(_tmpF84->tq).real_const,((_tmp1623.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF5C)).f2,_tmp1623)));}return bogus_ans;}_LL7A6: {struct Cyc_Absyn_AggrType_struct*
_tmpF7F=(struct Cyc_Absyn_AggrType_struct*)_tmpF7C;if(_tmpF7F->tag != 12)goto
_LL7A8;else{_tmpF80=_tmpF7F->f1;_tmpF81=_tmpF80.aggr_info;if((_tmpF81.KnownAggr).tag
!= 2)goto _LL7A8;_tmpF82=(struct Cyc_Absyn_Aggrdecl**)(_tmpF81.KnownAggr).val;
_tmpF83=*_tmpF82;}}_LL7A7: {struct Cyc_Absyn_Aggrfield*_tmpF86=Cyc_Absyn_lookup_decl_field(
_tmpF83,_tmpF5D);if(_tmpF86 != 0  && _tmpF86->width == 0){struct _tuple17 _tmp1624;
return(_tmp1624.f1=(_tmpF86->tq).real_const,((_tmp1624.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF5C)).f2,_tmp1624)));}return bogus_ans;}_LL7A8:;_LL7A9: return bogus_ans;
_LL7A3:;};_LL784: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpF5F=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpF57;if(_tmpF5F->tag != 23)goto _LL786;else{_tmpF60=_tmpF5F->f1;_tmpF61=_tmpF5F->f2;
_tmpF62=_tmpF5F->f3;}}_LL785: if(_tmpF62)return bogus_ans;{void*_tmpF88=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF60->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpF8A;void*_tmpF8B;struct Cyc_Absyn_PtrAtts _tmpF8C;void*_tmpF8D;_LL7AB: {struct
Cyc_Absyn_PointerType_struct*_tmpF89=(struct Cyc_Absyn_PointerType_struct*)
_tmpF88;if(_tmpF89->tag != 5)goto _LL7AD;else{_tmpF8A=_tmpF89->f1;_tmpF8B=_tmpF8A.elt_typ;
_tmpF8C=_tmpF8A.ptr_atts;_tmpF8D=_tmpF8C.rgn;}}_LL7AC: {struct Cyc_Absyn_Aggrfield*
finfo;{void*_tmpF8E=Cyc_Tcutil_compress(_tmpF8B);struct Cyc_List_List*_tmpF90;
struct Cyc_Absyn_AggrInfo _tmpF92;union Cyc_Absyn_AggrInfoU _tmpF93;struct Cyc_Absyn_Aggrdecl**
_tmpF94;struct Cyc_Absyn_Aggrdecl*_tmpF95;_LL7B0: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpF8F=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpF8E;if(_tmpF8F->tag != 13)goto
_LL7B2;else{_tmpF90=_tmpF8F->f2;}}_LL7B1: finfo=Cyc_Absyn_lookup_field(_tmpF90,
_tmpF61);goto _LL7AF;_LL7B2: {struct Cyc_Absyn_AggrType_struct*_tmpF91=(struct Cyc_Absyn_AggrType_struct*)
_tmpF8E;if(_tmpF91->tag != 12)goto _LL7B4;else{_tmpF92=_tmpF91->f1;_tmpF93=_tmpF92.aggr_info;
if((_tmpF93.KnownAggr).tag != 2)goto _LL7B4;_tmpF94=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF93.KnownAggr).val;_tmpF95=*_tmpF94;}}_LL7B3: finfo=Cyc_Absyn_lookup_decl_field(
_tmpF95,_tmpF61);goto _LL7AF;_LL7B4:;_LL7B5: return bogus_ans;_LL7AF:;}if(finfo != 0
 && finfo->width == 0){struct _tuple17 _tmp1625;return(_tmp1625.f1=(finfo->tq).real_const,((
_tmp1625.f2=_tmpF8D,_tmp1625)));}return bogus_ans;}_LL7AD:;_LL7AE: return bogus_ans;
_LL7AA:;};_LL786: {struct Cyc_Absyn_Deref_e_struct*_tmpF63=(struct Cyc_Absyn_Deref_e_struct*)
_tmpF57;if(_tmpF63->tag != 21)goto _LL788;else{_tmpF64=_tmpF63->f1;}}_LL787: {void*
_tmpF97=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpF64->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpF99;struct Cyc_Absyn_Tqual _tmpF9A;struct Cyc_Absyn_PtrAtts
_tmpF9B;void*_tmpF9C;_LL7B7: {struct Cyc_Absyn_PointerType_struct*_tmpF98=(struct
Cyc_Absyn_PointerType_struct*)_tmpF97;if(_tmpF98->tag != 5)goto _LL7B9;else{
_tmpF99=_tmpF98->f1;_tmpF9A=_tmpF99.elt_tq;_tmpF9B=_tmpF99.ptr_atts;_tmpF9C=
_tmpF9B.rgn;}}_LL7B8: {struct _tuple17 _tmp1626;return(_tmp1626.f1=_tmpF9A.real_const,((
_tmp1626.f2=_tmpF9C,_tmp1626)));}_LL7B9:;_LL7BA: return bogus_ans;_LL7B6:;}_LL788: {
struct Cyc_Absyn_Subscript_e_struct*_tmpF65=(struct Cyc_Absyn_Subscript_e_struct*)
_tmpF57;if(_tmpF65->tag != 24)goto _LL78A;else{_tmpF66=_tmpF65->f1;_tmpF67=_tmpF65->f2;}}
_LL789: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpF66->topt))->v);void*_tmpF9E=t;struct Cyc_List_List*_tmpFA0;struct Cyc_Absyn_PtrInfo
_tmpFA2;struct Cyc_Absyn_Tqual _tmpFA3;struct Cyc_Absyn_PtrAtts _tmpFA4;void*_tmpFA5;
struct Cyc_Absyn_ArrayInfo _tmpFA7;struct Cyc_Absyn_Tqual _tmpFA8;_LL7BC: {struct Cyc_Absyn_TupleType_struct*
_tmpF9F=(struct Cyc_Absyn_TupleType_struct*)_tmpF9E;if(_tmpF9F->tag != 11)goto
_LL7BE;else{_tmpFA0=_tmpF9F->f1;}}_LL7BD: {unsigned int _tmpFAA;int _tmpFAB;struct
_tuple13 _tmpFA9=Cyc_Evexp_eval_const_uint_exp(_tmpF67);_tmpFAA=_tmpFA9.f1;
_tmpFAB=_tmpFA9.f2;if(!_tmpFAB)return bogus_ans;{struct _tuple11*_tmpFAC=Cyc_Absyn_lookup_tuple_field(
_tmpFA0,(int)_tmpFAA);if(_tmpFAC != 0){struct _tuple17 _tmp1627;return(_tmp1627.f1=((*
_tmpFAC).f1).real_const,((_tmp1627.f2=(Cyc_Tcutil_addressof_props(te,_tmpF66)).f2,
_tmp1627)));}return bogus_ans;};}_LL7BE: {struct Cyc_Absyn_PointerType_struct*
_tmpFA1=(struct Cyc_Absyn_PointerType_struct*)_tmpF9E;if(_tmpFA1->tag != 5)goto
_LL7C0;else{_tmpFA2=_tmpFA1->f1;_tmpFA3=_tmpFA2.elt_tq;_tmpFA4=_tmpFA2.ptr_atts;
_tmpFA5=_tmpFA4.rgn;}}_LL7BF: {struct _tuple17 _tmp1628;return(_tmp1628.f1=_tmpFA3.real_const,((
_tmp1628.f2=_tmpFA5,_tmp1628)));}_LL7C0: {struct Cyc_Absyn_ArrayType_struct*
_tmpFA6=(struct Cyc_Absyn_ArrayType_struct*)_tmpF9E;if(_tmpFA6->tag != 9)goto
_LL7C2;else{_tmpFA7=_tmpFA6->f1;_tmpFA8=_tmpFA7.tq;}}_LL7C1: {struct _tuple17
_tmp1629;return(_tmp1629.f1=_tmpFA8.real_const,((_tmp1629.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF66)).f2,_tmp1629)));}_LL7C2:;_LL7C3: return bogus_ans;_LL7BB:;}_LL78A:;
_LL78B:{const char*_tmp162C;void*_tmp162B;(_tmp162B=0,Cyc_Tcutil_terr(e->loc,((
_tmp162C="unary & applied to non-lvalue",_tag_dyneither(_tmp162C,sizeof(char),30))),
_tag_dyneither(_tmp162B,sizeof(void*),0)));}return bogus_ans;_LL77F:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpFB3=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpFB5;void*_tmpFB6;struct Cyc_Absyn_Tqual
_tmpFB7;union Cyc_Absyn_Constraint*_tmpFB8;_LL7C5: {struct Cyc_Absyn_ArrayType_struct*
_tmpFB4=(struct Cyc_Absyn_ArrayType_struct*)_tmpFB3;if(_tmpFB4->tag != 9)goto
_LL7C7;else{_tmpFB5=_tmpFB4->f1;_tmpFB6=_tmpFB5.elt_type;_tmpFB7=_tmpFB5.tq;
_tmpFB8=_tmpFB5.zero_term;}}_LL7C6: {void*_tmpFBA;struct _tuple17 _tmpFB9=Cyc_Tcutil_addressof_props(
te,e);_tmpFBA=_tmpFB9.f2;{struct Cyc_Absyn_Upper_b_struct _tmp162F;struct Cyc_Absyn_Upper_b_struct*
_tmp162E;return Cyc_Absyn_atb_typ(_tmpFB6,_tmpFBA,_tmpFB7,(void*)((_tmp162E=
_cycalloc(sizeof(*_tmp162E)),((_tmp162E[0]=((_tmp162F.tag=1,((_tmp162F.f1=e,
_tmp162F)))),_tmp162E)))),_tmpFB8);};}_LL7C7:;_LL7C8: return e_typ;_LL7C4:;}void
Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,union Cyc_Absyn_Constraint*
b){b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
b);{void*_tmpFBD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpFC0;_LL7CA: {struct Cyc_Absyn_DynEither_b_struct*
_tmpFBE=(struct Cyc_Absyn_DynEither_b_struct*)_tmpFBD;if(_tmpFBE->tag != 0)goto
_LL7CC;}_LL7CB: return;_LL7CC: {struct Cyc_Absyn_Upper_b_struct*_tmpFBF=(struct Cyc_Absyn_Upper_b_struct*)
_tmpFBD;if(_tmpFBF->tag != 1)goto _LL7C9;else{_tmpFC0=_tmpFBF->f1;}}_LL7CD: {
unsigned int _tmpFC2;int _tmpFC3;struct _tuple13 _tmpFC1=Cyc_Evexp_eval_const_uint_exp(
_tmpFC0);_tmpFC2=_tmpFC1.f1;_tmpFC3=_tmpFC1.f2;if(_tmpFC3  && _tmpFC2 <= i){const
char*_tmp1634;void*_tmp1633[2];struct Cyc_Int_pa_struct _tmp1632;struct Cyc_Int_pa_struct
_tmp1631;(_tmp1631.tag=1,((_tmp1631.f1=(unsigned long)((int)i),((_tmp1632.tag=1,((
_tmp1632.f1=(unsigned long)((int)_tmpFC2),((_tmp1633[0]=& _tmp1632,((_tmp1633[1]=&
_tmp1631,Cyc_Tcutil_terr(loc,((_tmp1634="dereference is out of bounds: %d <= %d",
_tag_dyneither(_tmp1634,sizeof(char),39))),_tag_dyneither(_tmp1633,sizeof(void*),
2)))))))))))));}return;}_LL7C9:;};}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,union Cyc_Absyn_Constraint*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b){void*_tmpFC8=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*
_tmpFCA;_LL7CF: {struct Cyc_Absyn_Upper_b_struct*_tmpFC9=(struct Cyc_Absyn_Upper_b_struct*)
_tmpFC8;if(_tmpFC9->tag != 1)goto _LL7D1;else{_tmpFCA=_tmpFC9->f1;}}_LL7D0: {
unsigned int _tmpFCC;int _tmpFCD;struct _tuple13 _tmpFCB=Cyc_Evexp_eval_const_uint_exp(
_tmpFCA);_tmpFCC=_tmpFCB.f1;_tmpFCD=_tmpFCB.f2;return _tmpFCD  && _tmpFCC == 1;}
_LL7D1:;_LL7D2: return 0;_LL7CE:;}int Cyc_Tcutil_bits_only(void*t){void*_tmpFCE=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmpFD6;void*_tmpFD7;union Cyc_Absyn_Constraint*
_tmpFD8;struct Cyc_List_List*_tmpFDA;struct Cyc_Absyn_AggrInfo _tmpFDC;union Cyc_Absyn_AggrInfoU
_tmpFDD;struct _tuple4 _tmpFDE;struct Cyc_Absyn_AggrInfo _tmpFE0;union Cyc_Absyn_AggrInfoU
_tmpFE1;struct Cyc_Absyn_Aggrdecl**_tmpFE2;struct Cyc_Absyn_Aggrdecl*_tmpFE3;
struct Cyc_List_List*_tmpFE4;struct Cyc_List_List*_tmpFE6;_LL7D4: {struct Cyc_Absyn_VoidType_struct*
_tmpFCF=(struct Cyc_Absyn_VoidType_struct*)_tmpFCE;if(_tmpFCF->tag != 0)goto _LL7D6;}
_LL7D5: goto _LL7D7;_LL7D6: {struct Cyc_Absyn_IntType_struct*_tmpFD0=(struct Cyc_Absyn_IntType_struct*)
_tmpFCE;if(_tmpFD0->tag != 6)goto _LL7D8;}_LL7D7: goto _LL7D9;_LL7D8: {struct Cyc_Absyn_FloatType_struct*
_tmpFD1=(struct Cyc_Absyn_FloatType_struct*)_tmpFCE;if(_tmpFD1->tag != 7)goto
_LL7DA;}_LL7D9: goto _LL7DB;_LL7DA: {struct Cyc_Absyn_DoubleType_struct*_tmpFD2=(
struct Cyc_Absyn_DoubleType_struct*)_tmpFCE;if(_tmpFD2->tag != 8)goto _LL7DC;}
_LL7DB: return 1;_LL7DC: {struct Cyc_Absyn_EnumType_struct*_tmpFD3=(struct Cyc_Absyn_EnumType_struct*)
_tmpFCE;if(_tmpFD3->tag != 14)goto _LL7DE;}_LL7DD: goto _LL7DF;_LL7DE: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpFD4=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpFCE;if(_tmpFD4->tag != 15)goto
_LL7E0;}_LL7DF: return 0;_LL7E0: {struct Cyc_Absyn_ArrayType_struct*_tmpFD5=(struct
Cyc_Absyn_ArrayType_struct*)_tmpFCE;if(_tmpFD5->tag != 9)goto _LL7E2;else{_tmpFD6=
_tmpFD5->f1;_tmpFD7=_tmpFD6.elt_type;_tmpFD8=_tmpFD6.zero_term;}}_LL7E1: return !((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFD8) && Cyc_Tcutil_bits_only(
_tmpFD7);_LL7E2: {struct Cyc_Absyn_TupleType_struct*_tmpFD9=(struct Cyc_Absyn_TupleType_struct*)
_tmpFCE;if(_tmpFD9->tag != 11)goto _LL7E4;else{_tmpFDA=_tmpFD9->f1;}}_LL7E3: for(0;
_tmpFDA != 0;_tmpFDA=_tmpFDA->tl){if(!Cyc_Tcutil_bits_only((*((struct _tuple11*)
_tmpFDA->hd)).f2))return 0;}return 1;_LL7E4: {struct Cyc_Absyn_AggrType_struct*
_tmpFDB=(struct Cyc_Absyn_AggrType_struct*)_tmpFCE;if(_tmpFDB->tag != 12)goto
_LL7E6;else{_tmpFDC=_tmpFDB->f1;_tmpFDD=_tmpFDC.aggr_info;if((_tmpFDD.UnknownAggr).tag
!= 1)goto _LL7E6;_tmpFDE=(struct _tuple4)(_tmpFDD.UnknownAggr).val;}}_LL7E5: return
0;_LL7E6: {struct Cyc_Absyn_AggrType_struct*_tmpFDF=(struct Cyc_Absyn_AggrType_struct*)
_tmpFCE;if(_tmpFDF->tag != 12)goto _LL7E8;else{_tmpFE0=_tmpFDF->f1;_tmpFE1=_tmpFE0.aggr_info;
if((_tmpFE1.KnownAggr).tag != 2)goto _LL7E8;_tmpFE2=(struct Cyc_Absyn_Aggrdecl**)(
_tmpFE1.KnownAggr).val;_tmpFE3=*_tmpFE2;_tmpFE4=_tmpFE0.targs;}}_LL7E7: if(
_tmpFE3->impl == 0)return 0;{struct _RegionHandle _tmpFE7=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpFE7;_push_region(rgn);{struct Cyc_List_List*_tmpFE8=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpFE3->tvs,_tmpFE4);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFE3->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpFE8,((
struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpFE9=0;_npop_handler(0);return
_tmpFE9;}}}{int _tmpFEA=1;_npop_handler(0);return _tmpFEA;};};_pop_region(rgn);};
_LL7E8: {struct Cyc_Absyn_AnonAggrType_struct*_tmpFE5=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpFCE;if(_tmpFE5->tag != 13)goto _LL7EA;else{_tmpFE6=_tmpFE5->f2;}}_LL7E9: for(0;
_tmpFE6 != 0;_tmpFE6=_tmpFE6->tl){if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)
_tmpFE6->hd)->type))return 0;}return 1;_LL7EA:;_LL7EB: return 0;_LL7D3:;}static int
Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpFEB=e->r;struct _tuple2*_tmpFF3;void*_tmpFF4;struct Cyc_Absyn_Exp*_tmpFF6;
struct Cyc_Absyn_Exp*_tmpFF7;struct Cyc_Absyn_Exp*_tmpFF8;struct Cyc_Absyn_Exp*
_tmpFFA;struct Cyc_Absyn_Exp*_tmpFFB;struct Cyc_Absyn_Exp*_tmpFFD;struct Cyc_Absyn_Exp*
_tmpFFF;void*_tmp1001;struct Cyc_Absyn_Exp*_tmp1002;enum Cyc_Absyn_Coercion
_tmp1003;void*_tmp1005;struct Cyc_Absyn_Exp*_tmp1006;struct Cyc_Absyn_Exp*_tmp1008;
struct Cyc_Absyn_Exp*_tmp100A;struct Cyc_Absyn_Exp*_tmp100B;struct Cyc_List_List*
_tmp100D;struct Cyc_List_List*_tmp100F;struct Cyc_List_List*_tmp1011;enum Cyc_Absyn_Primop
_tmp1013;struct Cyc_List_List*_tmp1014;struct Cyc_List_List*_tmp1016;struct Cyc_List_List*
_tmp1018;_LL7ED: {struct Cyc_Absyn_Const_e_struct*_tmpFEC=(struct Cyc_Absyn_Const_e_struct*)
_tmpFEB;if(_tmpFEC->tag != 0)goto _LL7EF;}_LL7EE: goto _LL7F0;_LL7EF: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpFED=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpFEB;if(_tmpFED->tag != 18)goto
_LL7F1;}_LL7F0: goto _LL7F2;_LL7F1: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpFEE=(
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpFEB;if(_tmpFEE->tag != 19)goto _LL7F3;}
_LL7F2: goto _LL7F4;_LL7F3: {struct Cyc_Absyn_Offsetof_e_struct*_tmpFEF=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpFEB;if(_tmpFEF->tag != 20)goto _LL7F5;}_LL7F4: goto _LL7F6;_LL7F5: {struct Cyc_Absyn_Enum_e_struct*
_tmpFF0=(struct Cyc_Absyn_Enum_e_struct*)_tmpFEB;if(_tmpFF0->tag != 32)goto _LL7F7;}
_LL7F6: goto _LL7F8;_LL7F7: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpFF1=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpFEB;if(_tmpFF1->tag != 33)goto _LL7F9;}_LL7F8: return 1;_LL7F9: {struct Cyc_Absyn_Var_e_struct*
_tmpFF2=(struct Cyc_Absyn_Var_e_struct*)_tmpFEB;if(_tmpFF2->tag != 1)goto _LL7FB;
else{_tmpFF3=_tmpFF2->f1;_tmpFF4=(void*)_tmpFF2->f2;}}_LL7FA: {void*_tmp1019=
_tmpFF4;struct Cyc_Absyn_Vardecl*_tmp101C;_LL81A: {struct Cyc_Absyn_Funname_b_struct*
_tmp101A=(struct Cyc_Absyn_Funname_b_struct*)_tmp1019;if(_tmp101A->tag != 2)goto
_LL81C;}_LL81B: return 1;_LL81C: {struct Cyc_Absyn_Global_b_struct*_tmp101B=(struct
Cyc_Absyn_Global_b_struct*)_tmp1019;if(_tmp101B->tag != 1)goto _LL81E;else{
_tmp101C=_tmp101B->f1;}}_LL81D: {void*_tmp101E=Cyc_Tcutil_compress(_tmp101C->type);
_LL823: {struct Cyc_Absyn_ArrayType_struct*_tmp101F=(struct Cyc_Absyn_ArrayType_struct*)
_tmp101E;if(_tmp101F->tag != 9)goto _LL825;}_LL824: goto _LL826;_LL825: {struct Cyc_Absyn_FnType_struct*
_tmp1020=(struct Cyc_Absyn_FnType_struct*)_tmp101E;if(_tmp1020->tag != 10)goto
_LL827;}_LL826: return 1;_LL827:;_LL828: return var_okay;_LL822:;}_LL81E: {struct Cyc_Absyn_Unresolved_b_struct*
_tmp101D=(struct Cyc_Absyn_Unresolved_b_struct*)_tmp1019;if(_tmp101D->tag != 0)
goto _LL820;}_LL81F: return 0;_LL820:;_LL821: return var_okay;_LL819:;}_LL7FB: {
struct Cyc_Absyn_Conditional_e_struct*_tmpFF5=(struct Cyc_Absyn_Conditional_e_struct*)
_tmpFEB;if(_tmpFF5->tag != 6)goto _LL7FD;else{_tmpFF6=_tmpFF5->f1;_tmpFF7=_tmpFF5->f2;
_tmpFF8=_tmpFF5->f3;}}_LL7FC: return(Cyc_Tcutil_cnst_exp(te,0,_tmpFF6) && Cyc_Tcutil_cnst_exp(
te,0,_tmpFF7)) && Cyc_Tcutil_cnst_exp(te,0,_tmpFF8);_LL7FD: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpFF9=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpFEB;if(_tmpFF9->tag != 9)goto _LL7FF;
else{_tmpFFA=_tmpFF9->f1;_tmpFFB=_tmpFF9->f2;}}_LL7FE: return Cyc_Tcutil_cnst_exp(
te,0,_tmpFFA) && Cyc_Tcutil_cnst_exp(te,0,_tmpFFB);_LL7FF: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpFFC=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpFEB;if(_tmpFFC->tag != 13)
goto _LL801;else{_tmpFFD=_tmpFFC->f1;}}_LL800: _tmpFFF=_tmpFFD;goto _LL802;_LL801: {
struct Cyc_Absyn_Instantiate_e_struct*_tmpFFE=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmpFEB;if(_tmpFFE->tag != 14)goto _LL803;else{_tmpFFF=_tmpFFE->f1;}}_LL802: return
Cyc_Tcutil_cnst_exp(te,var_okay,_tmpFFF);_LL803: {struct Cyc_Absyn_Cast_e_struct*
_tmp1000=(struct Cyc_Absyn_Cast_e_struct*)_tmpFEB;if(_tmp1000->tag != 15)goto
_LL805;else{_tmp1001=(void*)_tmp1000->f1;_tmp1002=_tmp1000->f2;_tmp1003=_tmp1000->f4;
if(_tmp1003 != Cyc_Absyn_No_coercion)goto _LL805;}}_LL804: return Cyc_Tcutil_cnst_exp(
te,var_okay,_tmp1002);_LL805: {struct Cyc_Absyn_Cast_e_struct*_tmp1004=(struct Cyc_Absyn_Cast_e_struct*)
_tmpFEB;if(_tmp1004->tag != 15)goto _LL807;else{_tmp1005=(void*)_tmp1004->f1;
_tmp1006=_tmp1004->f2;}}_LL806: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp1006);
_LL807: {struct Cyc_Absyn_Address_e_struct*_tmp1007=(struct Cyc_Absyn_Address_e_struct*)
_tmpFEB;if(_tmp1007->tag != 16)goto _LL809;else{_tmp1008=_tmp1007->f1;}}_LL808:
return Cyc_Tcutil_cnst_exp(te,1,_tmp1008);_LL809: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp1009=(struct Cyc_Absyn_Comprehension_e_struct*)_tmpFEB;if(_tmp1009->tag != 28)
goto _LL80B;else{_tmp100A=_tmp1009->f2;_tmp100B=_tmp1009->f3;}}_LL80A: return Cyc_Tcutil_cnst_exp(
te,0,_tmp100A) && Cyc_Tcutil_cnst_exp(te,0,_tmp100B);_LL80B: {struct Cyc_Absyn_Array_e_struct*
_tmp100C=(struct Cyc_Absyn_Array_e_struct*)_tmpFEB;if(_tmp100C->tag != 27)goto
_LL80D;else{_tmp100D=_tmp100C->f1;}}_LL80C: _tmp100F=_tmp100D;goto _LL80E;_LL80D: {
struct Cyc_Absyn_AnonStruct_e_struct*_tmp100E=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmpFEB;if(_tmp100E->tag != 30)goto _LL80F;else{_tmp100F=_tmp100E->f2;}}_LL80E:
_tmp1011=_tmp100F;goto _LL810;_LL80F: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp1010=(struct Cyc_Absyn_Aggregate_e_struct*)_tmpFEB;if(_tmp1010->tag != 29)goto
_LL811;else{_tmp1011=_tmp1010->f3;}}_LL810: for(0;_tmp1011 != 0;_tmp1011=_tmp1011->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple18*)_tmp1011->hd)).f2))return 0;}
return 1;_LL811: {struct Cyc_Absyn_Primop_e_struct*_tmp1012=(struct Cyc_Absyn_Primop_e_struct*)
_tmpFEB;if(_tmp1012->tag != 3)goto _LL813;else{_tmp1013=_tmp1012->f1;_tmp1014=
_tmp1012->f2;}}_LL812: _tmp1016=_tmp1014;goto _LL814;_LL813: {struct Cyc_Absyn_Tuple_e_struct*
_tmp1015=(struct Cyc_Absyn_Tuple_e_struct*)_tmpFEB;if(_tmp1015->tag != 25)goto
_LL815;else{_tmp1016=_tmp1015->f1;}}_LL814: _tmp1018=_tmp1016;goto _LL816;_LL815: {
struct Cyc_Absyn_Datatype_e_struct*_tmp1017=(struct Cyc_Absyn_Datatype_e_struct*)
_tmpFEB;if(_tmp1017->tag != 31)goto _LL817;else{_tmp1018=_tmp1017->f1;}}_LL816:
for(0;_tmp1018 != 0;_tmp1018=_tmp1018->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(struct
Cyc_Absyn_Exp*)_tmp1018->hd))return 0;}return 1;_LL817:;_LL818: return 0;_LL7EC:;}
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){return
Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(struct
Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp1021=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1027;
struct Cyc_Absyn_PtrAtts _tmp1028;union Cyc_Absyn_Constraint*_tmp1029;union Cyc_Absyn_Constraint*
_tmp102A;struct Cyc_Absyn_ArrayInfo _tmp102C;void*_tmp102D;struct Cyc_List_List*
_tmp102F;struct Cyc_Absyn_AggrInfo _tmp1031;union Cyc_Absyn_AggrInfoU _tmp1032;
struct Cyc_List_List*_tmp1033;struct Cyc_List_List*_tmp1035;_LL82A: {struct Cyc_Absyn_VoidType_struct*
_tmp1022=(struct Cyc_Absyn_VoidType_struct*)_tmp1021;if(_tmp1022->tag != 0)goto
_LL82C;}_LL82B: goto _LL82D;_LL82C: {struct Cyc_Absyn_IntType_struct*_tmp1023=(
struct Cyc_Absyn_IntType_struct*)_tmp1021;if(_tmp1023->tag != 6)goto _LL82E;}_LL82D:
goto _LL82F;_LL82E: {struct Cyc_Absyn_FloatType_struct*_tmp1024=(struct Cyc_Absyn_FloatType_struct*)
_tmp1021;if(_tmp1024->tag != 7)goto _LL830;}_LL82F: goto _LL831;_LL830: {struct Cyc_Absyn_DoubleType_struct*
_tmp1025=(struct Cyc_Absyn_DoubleType_struct*)_tmp1021;if(_tmp1025->tag != 8)goto
_LL832;}_LL831: return 1;_LL832: {struct Cyc_Absyn_PointerType_struct*_tmp1026=(
struct Cyc_Absyn_PointerType_struct*)_tmp1021;if(_tmp1026->tag != 5)goto _LL834;
else{_tmp1027=_tmp1026->f1;_tmp1028=_tmp1027.ptr_atts;_tmp1029=_tmp1028.nullable;
_tmp102A=_tmp1028.bounds;}}_LL833: {void*_tmp1038=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp102A);_LL843: {
struct Cyc_Absyn_DynEither_b_struct*_tmp1039=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp1038;if(_tmp1039->tag != 0)goto _LL845;}_LL844: return 1;_LL845:;_LL846: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp1029);
_LL842:;}_LL834: {struct Cyc_Absyn_ArrayType_struct*_tmp102B=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1021;if(_tmp102B->tag != 9)goto _LL836;else{_tmp102C=_tmp102B->f1;_tmp102D=
_tmp102C.elt_type;}}_LL835: return Cyc_Tcutil_supports_default(_tmp102D);_LL836: {
struct Cyc_Absyn_TupleType_struct*_tmp102E=(struct Cyc_Absyn_TupleType_struct*)
_tmp1021;if(_tmp102E->tag != 11)goto _LL838;else{_tmp102F=_tmp102E->f1;}}_LL837:
for(0;_tmp102F != 0;_tmp102F=_tmp102F->tl){if(!Cyc_Tcutil_supports_default((*((
struct _tuple11*)_tmp102F->hd)).f2))return 0;}return 1;_LL838: {struct Cyc_Absyn_AggrType_struct*
_tmp1030=(struct Cyc_Absyn_AggrType_struct*)_tmp1021;if(_tmp1030->tag != 12)goto
_LL83A;else{_tmp1031=_tmp1030->f1;_tmp1032=_tmp1031.aggr_info;_tmp1033=_tmp1031.targs;}}
_LL839: {struct Cyc_Absyn_Aggrdecl*_tmp103A=Cyc_Absyn_get_known_aggrdecl(_tmp1032);
if(_tmp103A->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp103A->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmp103A->tvs,_tmp1033,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp103A->impl))->fields);}
_LL83A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1034=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1021;if(_tmp1034->tag != 13)goto _LL83C;else{_tmp1035=_tmp1034->f2;}}_LL83B:
return Cyc_Tcutil_fields_support_default(0,0,_tmp1035);_LL83C: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp1036=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp1021;if(_tmp1036->tag != 15)
goto _LL83E;}_LL83D: goto _LL83F;_LL83E: {struct Cyc_Absyn_EnumType_struct*_tmp1037=(
struct Cyc_Absyn_EnumType_struct*)_tmp1021;if(_tmp1037->tag != 14)goto _LL840;}
_LL83F: return 1;_LL840:;_LL841: return 0;_LL829:;}void Cyc_Tcutil_check_no_qual(
struct Cyc_Position_Segment*loc,void*t){void*_tmp103B=t;struct Cyc_Absyn_Typedefdecl*
_tmp103D;_LL848: {struct Cyc_Absyn_TypedefType_struct*_tmp103C=(struct Cyc_Absyn_TypedefType_struct*)
_tmp103B;if(_tmp103C->tag != 18)goto _LL84A;else{_tmp103D=_tmp103C->f3;}}_LL849:
if(_tmp103D != 0){struct Cyc_Absyn_Tqual _tmp103E=_tmp103D->tq;if(((_tmp103E.print_const
 || _tmp103E.q_volatile) || _tmp103E.q_restrict) || _tmp103E.real_const){const
char*_tmp1638;void*_tmp1637[1];struct Cyc_String_pa_struct _tmp1636;(_tmp1636.tag=
0,((_tmp1636.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp1637[0]=& _tmp1636,Cyc_Tcutil_warn(loc,((_tmp1638="qualifier within typedef type %s is ignored",
_tag_dyneither(_tmp1638,sizeof(char),44))),_tag_dyneither(_tmp1637,sizeof(void*),
1)))))));}}goto _LL847;_LL84A:;_LL84B: goto _LL847;_LL847:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){struct
_RegionHandle _tmp1042=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1042;
_push_region(rgn);{struct Cyc_List_List*_tmp1043=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmp1043,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);if(!Cyc_Tcutil_supports_default(
t)){int _tmp1044=0;_npop_handler(0);return _tmp1044;}}}{int _tmp1045=1;
_npop_handler(0);return _tmp1045;};;_pop_region(rgn);}int Cyc_Tcutil_admits_zero(
void*t){void*_tmp1046=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp104B;
struct Cyc_Absyn_PtrAtts _tmp104C;union Cyc_Absyn_Constraint*_tmp104D;union Cyc_Absyn_Constraint*
_tmp104E;_LL84D: {struct Cyc_Absyn_IntType_struct*_tmp1047=(struct Cyc_Absyn_IntType_struct*)
_tmp1046;if(_tmp1047->tag != 6)goto _LL84F;}_LL84E: goto _LL850;_LL84F: {struct Cyc_Absyn_FloatType_struct*
_tmp1048=(struct Cyc_Absyn_FloatType_struct*)_tmp1046;if(_tmp1048->tag != 7)goto
_LL851;}_LL850: goto _LL852;_LL851: {struct Cyc_Absyn_DoubleType_struct*_tmp1049=(
struct Cyc_Absyn_DoubleType_struct*)_tmp1046;if(_tmp1049->tag != 8)goto _LL853;}
_LL852: return 1;_LL853: {struct Cyc_Absyn_PointerType_struct*_tmp104A=(struct Cyc_Absyn_PointerType_struct*)
_tmp1046;if(_tmp104A->tag != 5)goto _LL855;else{_tmp104B=_tmp104A->f1;_tmp104C=
_tmp104B.ptr_atts;_tmp104D=_tmp104C.nullable;_tmp104E=_tmp104C.bounds;}}_LL854: {
void*_tmp104F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmp104E);_LL858: {struct Cyc_Absyn_DynEither_b_struct*
_tmp1050=(struct Cyc_Absyn_DynEither_b_struct*)_tmp104F;if(_tmp1050->tag != 0)goto
_LL85A;}_LL859: return 0;_LL85A:;_LL85B: return((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp104D);_LL857:;}_LL855:;_LL856: return 0;_LL84C:;}
int Cyc_Tcutil_is_noreturn(void*t){{void*_tmp1051=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_PtrInfo _tmp1053;void*_tmp1054;struct Cyc_Absyn_FnInfo _tmp1056;struct Cyc_List_List*
_tmp1057;_LL85D: {struct Cyc_Absyn_PointerType_struct*_tmp1052=(struct Cyc_Absyn_PointerType_struct*)
_tmp1051;if(_tmp1052->tag != 5)goto _LL85F;else{_tmp1053=_tmp1052->f1;_tmp1054=
_tmp1053.elt_typ;}}_LL85E: return Cyc_Tcutil_is_noreturn(_tmp1054);_LL85F: {struct
Cyc_Absyn_FnType_struct*_tmp1055=(struct Cyc_Absyn_FnType_struct*)_tmp1051;if(
_tmp1055->tag != 10)goto _LL861;else{_tmp1056=_tmp1055->f1;_tmp1057=_tmp1056.attributes;}}
_LL860: for(0;_tmp1057 != 0;_tmp1057=_tmp1057->tl){void*_tmp1058=(void*)_tmp1057->hd;
_LL864: {struct Cyc_Absyn_Noreturn_att_struct*_tmp1059=(struct Cyc_Absyn_Noreturn_att_struct*)
_tmp1058;if(_tmp1059->tag != 4)goto _LL866;}_LL865: return 1;_LL866:;_LL867:
continue;_LL863:;}goto _LL85C;_LL861:;_LL862: goto _LL85C;_LL85C:;}return 0;}struct
Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp105A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp105C;struct Cyc_List_List*
_tmp105D;struct Cyc_List_List**_tmp105E;_LL869: {struct Cyc_Absyn_FnType_struct*
_tmp105B=(struct Cyc_Absyn_FnType_struct*)_tmp105A;if(_tmp105B->tag != 10)goto
_LL86B;else{_tmp105C=_tmp105B->f1;_tmp105D=_tmp105C.attributes;_tmp105E=(struct
Cyc_List_List**)&(_tmp105B->f1).attributes;}}_LL86A: {struct Cyc_List_List*
_tmp105F=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1639;*_tmp105E=((_tmp1639=_cycalloc(sizeof(*_tmp1639)),((
_tmp1639->hd=(void*)atts->hd,((_tmp1639->tl=*_tmp105E,_tmp1639))))));}else{
struct Cyc_List_List*_tmp163A;_tmp105F=((_tmp163A=_cycalloc(sizeof(*_tmp163A)),((
_tmp163A->hd=(void*)atts->hd,((_tmp163A->tl=_tmp105F,_tmp163A))))));}}return
_tmp105F;}_LL86B:;_LL86C: {const char*_tmp163D;void*_tmp163C;(_tmp163C=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp163D="transfer_fn_type_atts",
_tag_dyneither(_tmp163D,sizeof(char),22))),_tag_dyneither(_tmp163C,sizeof(void*),
0)));}_LL868:;}
