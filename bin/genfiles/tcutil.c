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
struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct
_dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};
struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
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
_tmp3D;if(_tmp40->tag != 17)goto _LL5;else{_tmp41=_tmp40->f4;if(_tmp41 != 0)goto
_LL5;}}_LL4: return t;_LL5: {struct Cyc_Absyn_TypedefType_struct*_tmp42=(struct Cyc_Absyn_TypedefType_struct*)
_tmp3D;if(_tmp42->tag != 17)goto _LL7;else{_tmp43=_tmp42->f4;_tmp44=(void***)&
_tmp42->f4;}}_LL6: {void*t2=Cyc_Tcutil_compress(*((void**)_check_null(*_tmp44)));
if(t2 != *((void**)_check_null(*_tmp44))){void**_tmp10B8;*_tmp44=((_tmp10B8=
_cycalloc(sizeof(*_tmp10B8)),((_tmp10B8[0]=t2,_tmp10B8))));}return t2;}_LL7: {
struct Cyc_Absyn_Evar_struct*_tmp45=(struct Cyc_Absyn_Evar_struct*)_tmp3D;if(
_tmp45->tag != 1)goto _LL9;else{_tmp46=_tmp45->f2;_tmp47=(struct Cyc_Core_Opt**)&
_tmp45->f2;}}_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp47))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp47))->v){struct Cyc_Core_Opt*_tmp10B9;*_tmp47=((_tmp10B9=_cycalloc(sizeof(*
_tmp10B9)),((_tmp10B9->v=t2,_tmp10B9))));}return t2;}_LL9: {struct Cyc_Absyn_ValueofType_struct*
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
_tmp10C0)))))));}static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
struct _tuple10 _tmp63;struct Cyc_Core_Opt*_tmp64;struct Cyc_Absyn_Tqual _tmp65;void*
_tmp66;struct _tuple10*_tmp62=arg;_tmp63=*_tmp62;_tmp64=_tmp63.f1;_tmp65=_tmp63.f2;
_tmp66=_tmp63.f3;{struct _tuple10*_tmp10C1;return(_tmp10C1=_cycalloc(sizeof(*
_tmp10C1)),((_tmp10C1->f1=_tmp64,((_tmp10C1->f2=_tmp65,((_tmp10C1->f3=Cyc_Tcutil_copy_type(
_tmp66),_tmp10C1)))))));};}static struct _tuple12*Cyc_Tcutil_copy_tqt(struct
_tuple12*arg){struct _tuple12 _tmp69;struct Cyc_Absyn_Tqual _tmp6A;void*_tmp6B;
struct _tuple12*_tmp68=arg;_tmp69=*_tmp68;_tmp6A=_tmp69.f1;_tmp6B=_tmp69.f2;{
struct _tuple12*_tmp10C2;return(_tmp10C2=_cycalloc(sizeof(*_tmp10C2)),((_tmp10C2->f1=
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
_tmp76;if(_tmp8E->tag != 7)goto _LL33;}_LL32: return t;_LL33: {struct Cyc_Absyn_ArrayType_struct*
_tmp8F=(struct Cyc_Absyn_ArrayType_struct*)_tmp76;if(_tmp8F->tag != 8)goto _LL35;
else{_tmp90=_tmp8F->f1;_tmp91=_tmp90.elt_type;_tmp92=_tmp90.tq;_tmp93=_tmp90.num_elts;
_tmp94=_tmp90.zero_term;_tmp95=_tmp90.zt_loc;}}_LL34: {struct Cyc_Absyn_ArrayType_struct
_tmp10E7;struct Cyc_Absyn_ArrayInfo _tmp10E6;struct Cyc_Absyn_ArrayType_struct*
_tmp10E5;return(void*)((_tmp10E5=_cycalloc(sizeof(*_tmp10E5)),((_tmp10E5[0]=((
_tmp10E7.tag=8,((_tmp10E7.f1=((_tmp10E6.elt_type=Cyc_Tcutil_copy_type(_tmp91),((
_tmp10E6.tq=_tmp92,((_tmp10E6.num_elts=_tmp93,((_tmp10E6.zero_term=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp94),((_tmp10E6.zt_loc=
_tmp95,_tmp10E6)))))))))),_tmp10E7)))),_tmp10E5))));}_LL35: {struct Cyc_Absyn_FnType_struct*
_tmp96=(struct Cyc_Absyn_FnType_struct*)_tmp76;if(_tmp96->tag != 9)goto _LL37;else{
_tmp97=_tmp96->f1;_tmp98=_tmp97.tvars;_tmp99=_tmp97.effect;_tmp9A=_tmp97.ret_tqual;
_tmp9B=_tmp97.ret_typ;_tmp9C=_tmp97.args;_tmp9D=_tmp97.c_varargs;_tmp9E=_tmp97.cyc_varargs;
_tmp9F=_tmp97.rgn_po;_tmpA0=_tmp97.attributes;}}_LL36: {struct Cyc_List_List*
_tmpF5=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp98);struct Cyc_Core_Opt*
_tmp10E8;struct Cyc_Core_Opt*_tmpF6=_tmp99 == 0?0:((_tmp10E8=_cycalloc(sizeof(*
_tmp10E8)),((_tmp10E8->v=Cyc_Tcutil_copy_type((void*)_tmp99->v),_tmp10E8))));
void*_tmpF7=Cyc_Tcutil_copy_type(_tmp9B);struct Cyc_List_List*_tmpF8=((struct Cyc_List_List*(*)(
struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,
_tmp9C);int _tmpF9=_tmp9D;struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;if(_tmp9E != 0){
struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)_tmp9E;struct Cyc_Absyn_VarargInfo*
_tmp10E9;cyc_varargs2=((_tmp10E9=_cycalloc(sizeof(*_tmp10E9)),((_tmp10E9->name=
cv->name,((_tmp10E9->tq=cv->tq,((_tmp10E9->type=Cyc_Tcutil_copy_type(cv->type),((
_tmp10E9->inject=cv->inject,_tmp10E9))))))))));}{struct Cyc_List_List*_tmpFB=((
struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp9F);struct Cyc_List_List*_tmpFC=_tmpA0;
struct Cyc_Absyn_FnType_struct _tmp10EF;struct Cyc_Absyn_FnInfo _tmp10EE;struct Cyc_Absyn_FnType_struct*
_tmp10ED;return(void*)((_tmp10ED=_cycalloc(sizeof(*_tmp10ED)),((_tmp10ED[0]=((
_tmp10EF.tag=9,((_tmp10EF.f1=((_tmp10EE.tvars=_tmpF5,((_tmp10EE.effect=_tmpF6,((
_tmp10EE.ret_tqual=_tmp9A,((_tmp10EE.ret_typ=_tmpF7,((_tmp10EE.args=_tmpF8,((
_tmp10EE.c_varargs=_tmpF9,((_tmp10EE.cyc_varargs=cyc_varargs2,((_tmp10EE.rgn_po=
_tmpFB,((_tmp10EE.attributes=_tmpFC,_tmp10EE)))))))))))))))))),_tmp10EF)))),
_tmp10ED))));};}_LL37: {struct Cyc_Absyn_TupleType_struct*_tmpA1=(struct Cyc_Absyn_TupleType_struct*)
_tmp76;if(_tmpA1->tag != 10)goto _LL39;else{_tmpA2=_tmpA1->f1;}}_LL38: {struct Cyc_Absyn_TupleType_struct
_tmp10F2;struct Cyc_Absyn_TupleType_struct*_tmp10F1;return(void*)((_tmp10F1=
_cycalloc(sizeof(*_tmp10F1)),((_tmp10F1[0]=((_tmp10F2.tag=10,((_tmp10F2.f1=((
struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpA2),_tmp10F2)))),_tmp10F1))));}_LL39: {
struct Cyc_Absyn_AggrType_struct*_tmpA3=(struct Cyc_Absyn_AggrType_struct*)_tmp76;
if(_tmpA3->tag != 11)goto _LL3B;else{_tmpA4=_tmpA3->f1;_tmpA5=_tmpA4.aggr_info;if((
_tmpA5.UnknownAggr).tag != 1)goto _LL3B;_tmpA6=(struct _tuple4)(_tmpA5.UnknownAggr).val;
_tmpA7=_tmpA6.f1;_tmpA8=_tmpA6.f2;_tmpA9=_tmpA6.f3;_tmpAA=_tmpA4.targs;}}_LL3A: {
struct Cyc_Absyn_AggrType_struct _tmp10F8;struct Cyc_Absyn_AggrInfo _tmp10F7;struct
Cyc_Absyn_AggrType_struct*_tmp10F6;return(void*)((_tmp10F6=_cycalloc(sizeof(*
_tmp10F6)),((_tmp10F6[0]=((_tmp10F8.tag=11,((_tmp10F8.f1=((_tmp10F7.aggr_info=
Cyc_Absyn_UnknownAggr(_tmpA7,_tmpA8,_tmpA9),((_tmp10F7.targs=Cyc_Tcutil_copy_types(
_tmpAA),_tmp10F7)))),_tmp10F8)))),_tmp10F6))));}_LL3B: {struct Cyc_Absyn_AggrType_struct*
_tmpAB=(struct Cyc_Absyn_AggrType_struct*)_tmp76;if(_tmpAB->tag != 11)goto _LL3D;
else{_tmpAC=_tmpAB->f1;_tmpAD=_tmpAC.aggr_info;if((_tmpAD.KnownAggr).tag != 2)
goto _LL3D;_tmpAE=(struct Cyc_Absyn_Aggrdecl**)(_tmpAD.KnownAggr).val;_tmpAF=
_tmpAC.targs;}}_LL3C: {struct Cyc_Absyn_AggrType_struct _tmp10FE;struct Cyc_Absyn_AggrInfo
_tmp10FD;struct Cyc_Absyn_AggrType_struct*_tmp10FC;return(void*)((_tmp10FC=
_cycalloc(sizeof(*_tmp10FC)),((_tmp10FC[0]=((_tmp10FE.tag=11,((_tmp10FE.f1=((
_tmp10FD.aggr_info=Cyc_Absyn_KnownAggr(_tmpAE),((_tmp10FD.targs=Cyc_Tcutil_copy_types(
_tmpAF),_tmp10FD)))),_tmp10FE)))),_tmp10FC))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpB0=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp76;if(_tmpB0->tag != 12)goto
_LL3F;else{_tmpB1=_tmpB0->f1;_tmpB2=_tmpB0->f2;}}_LL3E: {struct Cyc_Absyn_AnonAggrType_struct
_tmp1101;struct Cyc_Absyn_AnonAggrType_struct*_tmp1100;return(void*)((_tmp1100=
_cycalloc(sizeof(*_tmp1100)),((_tmp1100[0]=((_tmp1101.tag=12,((_tmp1101.f1=
_tmpB1,((_tmp1101.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmpB2),_tmp1101)))))),_tmp1100))));}_LL3F: {struct Cyc_Absyn_EnumType_struct*
_tmpB3=(struct Cyc_Absyn_EnumType_struct*)_tmp76;if(_tmpB3->tag != 13)goto _LL41;
else{_tmpB4=_tmpB3->f1;_tmpB5=_tmpB3->f2;}}_LL40: {struct Cyc_Absyn_EnumType_struct
_tmp1104;struct Cyc_Absyn_EnumType_struct*_tmp1103;return(void*)((_tmp1103=
_cycalloc(sizeof(*_tmp1103)),((_tmp1103[0]=((_tmp1104.tag=13,((_tmp1104.f1=
_tmpB4,((_tmp1104.f2=_tmpB5,_tmp1104)))))),_tmp1103))));}_LL41: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpB6=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp76;if(_tmpB6->tag != 14)goto
_LL43;else{_tmpB7=_tmpB6->f1;}}_LL42: {struct Cyc_Absyn_AnonEnumType_struct
_tmp1107;struct Cyc_Absyn_AnonEnumType_struct*_tmp1106;return(void*)((_tmp1106=
_cycalloc(sizeof(*_tmp1106)),((_tmp1106[0]=((_tmp1107.tag=14,((_tmp1107.f1=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpB7),_tmp1107)))),
_tmp1106))));}_LL43: {struct Cyc_Absyn_TagType_struct*_tmpB8=(struct Cyc_Absyn_TagType_struct*)
_tmp76;if(_tmpB8->tag != 19)goto _LL45;else{_tmpB9=(void*)_tmpB8->f1;}}_LL44: {
struct Cyc_Absyn_TagType_struct _tmp110A;struct Cyc_Absyn_TagType_struct*_tmp1109;
return(void*)((_tmp1109=_cycalloc(sizeof(*_tmp1109)),((_tmp1109[0]=((_tmp110A.tag=
19,((_tmp110A.f1=(void*)Cyc_Tcutil_copy_type(_tmpB9),_tmp110A)))),_tmp1109))));}
_LL45: {struct Cyc_Absyn_ValueofType_struct*_tmpBA=(struct Cyc_Absyn_ValueofType_struct*)
_tmp76;if(_tmpBA->tag != 18)goto _LL47;else{_tmpBB=_tmpBA->f1;}}_LL46: {struct Cyc_Absyn_ValueofType_struct
_tmp110D;struct Cyc_Absyn_ValueofType_struct*_tmp110C;return(void*)((_tmp110C=
_cycalloc(sizeof(*_tmp110C)),((_tmp110C[0]=((_tmp110D.tag=18,((_tmp110D.f1=
_tmpBB,_tmp110D)))),_tmp110C))));}_LL47: {struct Cyc_Absyn_RgnHandleType_struct*
_tmpBC=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp76;if(_tmpBC->tag != 15)goto
_LL49;else{_tmpBD=(void*)_tmpBC->f1;}}_LL48: {struct Cyc_Absyn_RgnHandleType_struct
_tmp1110;struct Cyc_Absyn_RgnHandleType_struct*_tmp110F;return(void*)((_tmp110F=
_cycalloc(sizeof(*_tmp110F)),((_tmp110F[0]=((_tmp1110.tag=15,((_tmp1110.f1=(void*)
Cyc_Tcutil_copy_type(_tmpBD),_tmp1110)))),_tmp110F))));}_LL49: {struct Cyc_Absyn_DynRgnType_struct*
_tmpBE=(struct Cyc_Absyn_DynRgnType_struct*)_tmp76;if(_tmpBE->tag != 16)goto _LL4B;
else{_tmpBF=(void*)_tmpBE->f1;_tmpC0=(void*)_tmpBE->f2;}}_LL4A: {struct Cyc_Absyn_DynRgnType_struct
_tmp1113;struct Cyc_Absyn_DynRgnType_struct*_tmp1112;return(void*)((_tmp1112=
_cycalloc(sizeof(*_tmp1112)),((_tmp1112[0]=((_tmp1113.tag=16,((_tmp1113.f1=(void*)
Cyc_Tcutil_copy_type(_tmpBF),((_tmp1113.f2=(void*)Cyc_Tcutil_copy_type(_tmpC0),
_tmp1113)))))),_tmp1112))));}_LL4B: {struct Cyc_Absyn_TypedefType_struct*_tmpC1=(
struct Cyc_Absyn_TypedefType_struct*)_tmp76;if(_tmpC1->tag != 17)goto _LL4D;else{
_tmpC2=_tmpC1->f1;_tmpC3=_tmpC1->f2;_tmpC4=_tmpC1->f3;}}_LL4C: {struct Cyc_Absyn_TypedefType_struct
_tmp1116;struct Cyc_Absyn_TypedefType_struct*_tmp1115;return(void*)((_tmp1115=
_cycalloc(sizeof(*_tmp1115)),((_tmp1115[0]=((_tmp1116.tag=17,((_tmp1116.f1=
_tmpC2,((_tmp1116.f2=Cyc_Tcutil_copy_types(_tmpC3),((_tmp1116.f3=_tmpC4,((
_tmp1116.f4=0,_tmp1116)))))))))),_tmp1115))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpC5=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp76;if(_tmpC5->tag != 21)goto _LL4F;}
_LL4E: goto _LL50;_LL4F: {struct Cyc_Absyn_RefCntRgn_struct*_tmpC6=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp76;if(_tmpC6->tag != 22)goto _LL51;}_LL50: goto _LL52;_LL51: {struct Cyc_Absyn_HeapRgn_struct*
_tmpC7=(struct Cyc_Absyn_HeapRgn_struct*)_tmp76;if(_tmpC7->tag != 20)goto _LL53;}
_LL52: return t;_LL53: {struct Cyc_Absyn_AccessEff_struct*_tmpC8=(struct Cyc_Absyn_AccessEff_struct*)
_tmp76;if(_tmpC8->tag != 23)goto _LL55;else{_tmpC9=(void*)_tmpC8->f1;}}_LL54: {
struct Cyc_Absyn_AccessEff_struct _tmp1119;struct Cyc_Absyn_AccessEff_struct*
_tmp1118;return(void*)((_tmp1118=_cycalloc(sizeof(*_tmp1118)),((_tmp1118[0]=((
_tmp1119.tag=23,((_tmp1119.f1=(void*)Cyc_Tcutil_copy_type(_tmpC9),_tmp1119)))),
_tmp1118))));}_LL55: {struct Cyc_Absyn_JoinEff_struct*_tmpCA=(struct Cyc_Absyn_JoinEff_struct*)
_tmp76;if(_tmpCA->tag != 24)goto _LL57;else{_tmpCB=_tmpCA->f1;}}_LL56: {struct Cyc_Absyn_JoinEff_struct
_tmp111C;struct Cyc_Absyn_JoinEff_struct*_tmp111B;return(void*)((_tmp111B=
_cycalloc(sizeof(*_tmp111B)),((_tmp111B[0]=((_tmp111C.tag=24,((_tmp111C.f1=Cyc_Tcutil_copy_types(
_tmpCB),_tmp111C)))),_tmp111B))));}_LL57: {struct Cyc_Absyn_RgnsEff_struct*_tmpCC=(
struct Cyc_Absyn_RgnsEff_struct*)_tmp76;if(_tmpCC->tag != 25)goto _LL59;else{_tmpCD=(
void*)_tmpCC->f1;}}_LL58: {struct Cyc_Absyn_RgnsEff_struct _tmp111F;struct Cyc_Absyn_RgnsEff_struct*
_tmp111E;return(void*)((_tmp111E=_cycalloc(sizeof(*_tmp111E)),((_tmp111E[0]=((
_tmp111F.tag=25,((_tmp111F.f1=(void*)Cyc_Tcutil_copy_type(_tmpCD),_tmp111F)))),
_tmp111E))));}_LL59: {struct Cyc_Absyn_TypeDeclType_struct*_tmpCE=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpCE->tag != 26)goto _LL5B;else{_tmpCF=_tmpCE->f1;_tmpD0=*_tmpCF;
_tmpD1=_tmpD0.r;{struct Cyc_Absyn_Aggr_td_struct*_tmpD2=(struct Cyc_Absyn_Aggr_td_struct*)
_tmpD1;if(_tmpD2->tag != 0)goto _LL5B;else{_tmpD3=_tmpD2->f1;}};}}_LL5A: {struct
Cyc_List_List*_tmp11F=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD3->tvs);struct Cyc_Absyn_AggrType_struct
_tmp1125;struct Cyc_Absyn_AggrInfo _tmp1124;struct Cyc_Absyn_AggrType_struct*
_tmp1123;return(void*)((_tmp1123=_cycalloc(sizeof(*_tmp1123)),((_tmp1123[0]=((
_tmp1125.tag=11,((_tmp1125.f1=((_tmp1124.aggr_info=Cyc_Absyn_UnknownAggr(_tmpD3->kind,
_tmpD3->name,0),((_tmp1124.targs=_tmp11F,_tmp1124)))),_tmp1125)))),_tmp1123))));}
_LL5B: {struct Cyc_Absyn_TypeDeclType_struct*_tmpD4=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpD4->tag != 26)goto _LL5D;else{_tmpD5=_tmpD4->f1;_tmpD6=*_tmpD5;
_tmpD7=_tmpD6.r;{struct Cyc_Absyn_Enum_td_struct*_tmpD8=(struct Cyc_Absyn_Enum_td_struct*)
_tmpD7;if(_tmpD8->tag != 1)goto _LL5D;else{_tmpD9=_tmpD8->f1;}};}}_LL5C: {struct
Cyc_Absyn_EnumType_struct _tmp1128;struct Cyc_Absyn_EnumType_struct*_tmp1127;
return(void*)((_tmp1127=_cycalloc(sizeof(*_tmp1127)),((_tmp1127[0]=((_tmp1128.tag=
13,((_tmp1128.f1=_tmpD9->name,((_tmp1128.f2=0,_tmp1128)))))),_tmp1127))));}_LL5D: {
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
_LL5F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static
struct _tuple19*Cyc_Tcutil_copy_eds(struct _tuple19*e){struct _tuple19*_tmp1136;
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
_tmp1151;struct _tuple10*_tmp1150;struct Cyc_Absyn_CompoundLit_e_struct*_tmp114F;
return Cyc_Absyn_new_exp((void*)((_tmp114F=_cycalloc(sizeof(*_tmp114F)),((
_tmp114F[0]=((_tmp1151.tag=26,((_tmp1151.f1=((_tmp1150=_cycalloc(sizeof(*
_tmp1150)),((_tmp1150->f1=vopt1,((_tmp1150->f2=_tmp185,((_tmp1150->f3=Cyc_Tcutil_copy_type(
_tmp186),_tmp1150)))))))),((_tmp1151.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*
f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp187),_tmp1151)))))),_tmp114F)))),e->loc);};}_LL9B: {struct Cyc_Absyn_Array_e_struct*
_tmp188=(struct Cyc_Absyn_Array_e_struct*)_tmp132;if(_tmp188->tag != 27)goto _LL9D;
else{_tmp189=_tmp188->f1;}}_LL9C: {struct Cyc_Absyn_Array_e_struct _tmp1154;struct
Cyc_Absyn_Array_e_struct*_tmp1153;return Cyc_Absyn_new_exp((void*)((_tmp1153=
_cycalloc(sizeof(*_tmp1153)),((_tmp1153[0]=((_tmp1154.tag=27,((_tmp1154.f1=((
struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*
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
_tmp191),((_tmp115A.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct
_tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp192),((
_tmp115A.f4=_tmp193,_tmp115A)))))))))),_tmp1159)))),e->loc);}_LLA1: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp194=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp132;if(_tmp194->tag != 30)goto
_LLA3;else{_tmp195=(void*)_tmp194->f1;_tmp196=_tmp194->f2;}}_LLA2: {struct Cyc_Absyn_AnonStruct_e_struct
_tmp115D;struct Cyc_Absyn_AnonStruct_e_struct*_tmp115C;return Cyc_Absyn_new_exp((
void*)((_tmp115C=_cycalloc(sizeof(*_tmp115C)),((_tmp115C[0]=((_tmp115D.tag=30,((
_tmp115D.f1=(void*)Cyc_Tcutil_copy_type(_tmp195),((_tmp115D.f2=((struct Cyc_List_List*(*)(
struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
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
struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
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
_LLB6: return Cyc_Absyn_asm_exp(_tmp1B7,_tmp1B8,e->loc);_LL64:;}struct _tuple20{
enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual
f1;enum Cyc_Absyn_AliasQual f2;};int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,
struct Cyc_Absyn_Kind*ka2){struct Cyc_Absyn_Kind _tmp1ED;enum Cyc_Absyn_KindQual
_tmp1EE;enum Cyc_Absyn_AliasQual _tmp1EF;struct Cyc_Absyn_Kind*_tmp1EC=ka1;_tmp1ED=*
_tmp1EC;_tmp1EE=_tmp1ED.kind;_tmp1EF=_tmp1ED.aliasqual;{struct Cyc_Absyn_Kind
_tmp1F1;enum Cyc_Absyn_KindQual _tmp1F2;enum Cyc_Absyn_AliasQual _tmp1F3;struct Cyc_Absyn_Kind*
_tmp1F0=ka2;_tmp1F1=*_tmp1F0;_tmp1F2=_tmp1F1.kind;_tmp1F3=_tmp1F1.aliasqual;if(
_tmp1EE != _tmp1F2){struct _tuple20 _tmp1178;struct _tuple20 _tmp1F5=(_tmp1178.f1=
_tmp1EE,((_tmp1178.f2=_tmp1F2,_tmp1178)));enum Cyc_Absyn_KindQual _tmp1F6;enum 
Cyc_Absyn_KindQual _tmp1F7;enum Cyc_Absyn_KindQual _tmp1F8;enum Cyc_Absyn_KindQual
_tmp1F9;enum Cyc_Absyn_KindQual _tmp1FA;enum Cyc_Absyn_KindQual _tmp1FB;_LLBD:
_tmp1F6=_tmp1F5.f1;if(_tmp1F6 != Cyc_Absyn_BoxKind)goto _LLBF;_tmp1F7=_tmp1F5.f2;
if(_tmp1F7 != Cyc_Absyn_MemKind)goto _LLBF;_LLBE: goto _LLC0;_LLBF: _tmp1F8=_tmp1F5.f1;
if(_tmp1F8 != Cyc_Absyn_BoxKind)goto _LLC1;_tmp1F9=_tmp1F5.f2;if(_tmp1F9 != Cyc_Absyn_AnyKind)
goto _LLC1;_LLC0: goto _LLC2;_LLC1: _tmp1FA=_tmp1F5.f1;if(_tmp1FA != Cyc_Absyn_MemKind)
goto _LLC3;_tmp1FB=_tmp1F5.f2;if(_tmp1FB != Cyc_Absyn_AnyKind)goto _LLC3;_LLC2: goto
_LLBC;_LLC3:;_LLC4: return 0;_LLBC:;}if(_tmp1EF != _tmp1F3){struct _tuple21 _tmp1179;
struct _tuple21 _tmp1FD=(_tmp1179.f1=_tmp1EF,((_tmp1179.f2=_tmp1F3,_tmp1179)));
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
_tmp1182))));}return 1;_LLD3:;}struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp23A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp23C;_LLE1: {struct
Cyc_Absyn_VarType_struct*_tmp23B=(struct Cyc_Absyn_VarType_struct*)_tmp23A;if(
_tmp23B->tag != 2)goto _LLE3;else{_tmp23C=_tmp23B->f1;}}_LLE2: {void*_tmp23D=
_tmp23C->kind;_tmp23C->kind=kb;{struct _tuple16 _tmp1183;return(_tmp1183.f1=
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
_LL105: {const char*_tmp118A;void*_tmp1189;(_tmp1189=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp118A="typ_kind: Unresolved DatatypeFieldType",
_tag_dyneither(_tmp118A,sizeof(char),39))),_tag_dyneither(_tmp1189,sizeof(void*),
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
_LL127;}_LL126: {struct Cyc_Absyn_Kind*_tmp118B;return(_tmp118B=_cycalloc_atomic(
sizeof(*_tmp118B)),((_tmp118B->kind=Cyc_Absyn_MemKind,((_tmp118B->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp26D.ptr_atts).rgn))->aliasqual,_tmp118B)))));}_LL127: {struct Cyc_Absyn_Upper_b_struct*
_tmp28E=(struct Cyc_Absyn_Upper_b_struct*)_tmp28C;if(_tmp28E->tag != 1)goto _LL124;}
_LL128: {struct Cyc_Absyn_Kind*_tmp118C;return(_tmp118C=_cycalloc_atomic(sizeof(*
_tmp118C)),((_tmp118C->kind=Cyc_Absyn_BoxKind,((_tmp118C->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp26D.ptr_atts).rgn))->aliasqual,_tmp118C)))));}_LL124:;}_LL10E: {struct Cyc_Absyn_ValueofType_struct*
_tmp26E=(struct Cyc_Absyn_ValueofType_struct*)_tmp242;if(_tmp26E->tag != 18)goto
_LL110;}_LL10F: return& Cyc_Tcutil_ik;_LL110: {struct Cyc_Absyn_TagType_struct*
_tmp26F=(struct Cyc_Absyn_TagType_struct*)_tmp242;if(_tmp26F->tag != 19)goto _LL112;}
_LL111: return& Cyc_Tcutil_bk;_LL112: {struct Cyc_Absyn_ArrayType_struct*_tmp270=(
struct Cyc_Absyn_ArrayType_struct*)_tmp242;if(_tmp270->tag != 8)goto _LL114;}_LL113:
goto _LL115;_LL114: {struct Cyc_Absyn_TupleType_struct*_tmp271=(struct Cyc_Absyn_TupleType_struct*)
_tmp242;if(_tmp271->tag != 10)goto _LL116;}_LL115: return& Cyc_Tcutil_mk;_LL116: {
struct Cyc_Absyn_TypedefType_struct*_tmp272=(struct Cyc_Absyn_TypedefType_struct*)
_tmp242;if(_tmp272->tag != 17)goto _LL118;else{_tmp273=_tmp272->f3;}}_LL117: if(
_tmp273 == 0  || _tmp273->kind == 0){const char*_tmp1190;void*_tmp118F[1];struct Cyc_String_pa_struct
_tmp118E;(_tmp118E.tag=0,((_tmp118E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp118F[0]=& _tmp118E,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1190="typ_kind: typedef found: %s",
_tag_dyneither(_tmp1190,sizeof(char),28))),_tag_dyneither(_tmp118F,sizeof(void*),
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
Cyc_Tcutil_fast_tvar_cmp,env,_tmp29D)){{const char*_tmp1191;Cyc_Tcutil_failure_reason=((
_tmp1191="(type variable would escape scope)",_tag_dyneither(_tmp1191,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}goto _LL12E;_LL131: {
struct Cyc_Absyn_Evar_struct*_tmp29E=(struct Cyc_Absyn_Evar_struct*)_tmp29B;if(
_tmp29E->tag != 1)goto _LL133;else{_tmp29F=_tmp29E->f2;_tmp2A0=_tmp29E->f4;_tmp2A1=(
struct Cyc_Core_Opt**)& _tmp29E->f4;}}_LL132: if(t == evar){{const char*_tmp1192;Cyc_Tcutil_failure_reason=((
_tmp1192="(occurs check)",_tag_dyneither(_tmp1192,sizeof(char),15)));}(int)
_throw((void*)& Cyc_Tcutil_Unify_val);}else{if(_tmp29F != 0)Cyc_Tcutil_occurs(evar,
r,env,(void*)_tmp29F->v);else{int problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp2A1))->v;for(0;s != 0;s=s->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp2CD=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp2A1))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd)){struct Cyc_List_List*_tmp1193;_tmp2CD=((
_tmp1193=_cycalloc(sizeof(*_tmp1193)),((_tmp1193->hd=(struct Cyc_Absyn_Tvar*)s->hd,((
_tmp1193->tl=_tmp2CD,_tmp1193))))));}}}{struct Cyc_Core_Opt*_tmp1194;*_tmp2A1=((
_tmp1194=_cycalloc(sizeof(*_tmp1194)),((_tmp1194->v=_tmp2CD,_tmp1194))));};}}}
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
_tmp2D9=x;union Cyc_Absyn_Constraint _tmp2DA;int _tmp2DB;union Cyc_Absyn_Constraint
_tmp2DC;void*_tmp2DD;union Cyc_Absyn_Constraint _tmp2DE;union Cyc_Absyn_Constraint*
_tmp2DF;_LL152: _tmp2DA=*_tmp2D9;if((_tmp2DA.No_constr).tag != 3)goto _LL154;
_tmp2DB=(int)(_tmp2DA.No_constr).val;_LL153:{union Cyc_Absyn_Constraint _tmp119C;*
x=(((_tmp119C.Forward_constr).val=y,(((_tmp119C.Forward_constr).tag=2,_tmp119C))));}
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
char*_tmp119F;void*_tmp119E;(_tmp119E=0,Cyc_Tcutil_impos(((_tmp119F="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp119F,sizeof(char),40))),_tag_dyneither(_tmp119E,sizeof(void*),
0)));}_LL158:;}_LL156: _tmp2DE=*_tmp2D9;if((_tmp2DE.Forward_constr).tag != 2)goto
_LL151;_tmp2DF=(union Cyc_Absyn_Constraint*)(_tmp2DE.Forward_constr).val;_LL157: {
const char*_tmp11A2;void*_tmp11A1;(_tmp11A1=0,Cyc_Tcutil_impos(((_tmp11A2="unify_conref: forward after compress",
_tag_dyneither(_tmp11A2,sizeof(char),37))),_tag_dyneither(_tmp11A1,sizeof(void*),
0)));}_LL151:;};}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){struct _handler_cons _tmp2EC;
_push_handler(& _tmp2EC);{int _tmp2EE=0;if(setjmp(_tmp2EC.handler))_tmp2EE=1;if(!
_tmp2EE){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _dyneither_ptr)
_tag_dyneither(0,0,0));{int _tmp2EF=1;_npop_handler(0);return _tmp2EF;};;
_pop_handler();}else{void*_tmp2ED=(void*)_exn_thrown;void*_tmp2F1=_tmp2ED;_LL160: {
struct Cyc_Tcutil_Unify_struct*_tmp2F2=(struct Cyc_Tcutil_Unify_struct*)_tmp2F1;
if(_tmp2F2->tag != Cyc_Tcutil_Unify)goto _LL162;}_LL161: return 0;_LL162:;_LL163:(
void)_throw(_tmp2F1);_LL15F:;}};}static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp11A3;struct _tuple0 _tmp2F4=(_tmp11A3.f1=b1,((_tmp11A3.f2=b2,
_tmp11A3)));void*_tmp2F5;void*_tmp2F7;void*_tmp2F9;void*_tmp2FB;void*_tmp2FD;
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
_tmp11A4;struct _tuple0 _tmp304=(_tmp11A4.f1=b1,((_tmp11A4.f2=b2,_tmp11A4)));void*
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
void*att1,void*att2){struct _tuple0 _tmp11A5;struct _tuple0 _tmp32A=(_tmp11A5.f1=
att1,((_tmp11A5.f2=att2,_tmp11A5)));void*_tmp32B;int _tmp32D;void*_tmp32E;int
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
_tmp11A8;struct Cyc_Absyn_ValueofType_struct*_tmp11A7;t=(void*)((_tmp11A7=
_cycalloc(sizeof(*_tmp11A7)),((_tmp11A7[0]=((_tmp11A8.tag=18,((_tmp11A8.f1=Cyc_Absyn_uint_exp(
0,0),_tmp11A8)))),_tmp11A7))));}goto _LL1B0;_LL1B9:;_LL1BA: t=Cyc_Absyn_sint_typ;
goto _LL1B0;_LL1B0:;}{struct _tuple16*_tmp11A9;return(_tmp11A9=_cycalloc(sizeof(*
_tmp11A9)),((_tmp11A9->f1=tv,((_tmp11A9->f2=t,_tmp11A9)))));};}static void*Cyc_Tcutil_rgns_of(
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
_tmp11AC;struct Cyc_Absyn_AccessEff_struct*_tmp11AB;return(void*)((_tmp11AB=
_cycalloc(sizeof(*_tmp11AB)),((_tmp11AB[0]=((_tmp11AC.tag=23,((_tmp11AC.f1=(void*)
t,_tmp11AC)))),_tmp11AB))));}_LL1F5: _tmp397=*_tmp394;_tmp398=_tmp397.kind;if(
_tmp398 != Cyc_Absyn_EffKind)goto _LL1F7;_LL1F6: return t;_LL1F7: _tmp399=*_tmp394;
_tmp39A=_tmp399.kind;if(_tmp39A != Cyc_Absyn_IntKind)goto _LL1F9;_LL1F8: return Cyc_Absyn_empty_effect;
_LL1F9:;_LL1FA: {struct Cyc_Absyn_RgnsEff_struct _tmp11AF;struct Cyc_Absyn_RgnsEff_struct*
_tmp11AE;return(void*)((_tmp11AE=_cycalloc(sizeof(*_tmp11AE)),((_tmp11AE[0]=((
_tmp11AF.tag=25,((_tmp11AF.f1=(void*)t,_tmp11AF)))),_tmp11AE))));}_LL1F2:;}
_LL1CC: {struct Cyc_Absyn_RgnHandleType_struct*_tmp366=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp35D;if(_tmp366->tag != 15)goto _LL1CE;else{_tmp367=(void*)_tmp366->f1;}}_LL1CD: {
struct Cyc_Absyn_AccessEff_struct _tmp11B2;struct Cyc_Absyn_AccessEff_struct*
_tmp11B1;return(void*)((_tmp11B1=_cycalloc(sizeof(*_tmp11B1)),((_tmp11B1[0]=((
_tmp11B2.tag=23,((_tmp11B2.f1=(void*)_tmp367,_tmp11B2)))),_tmp11B1))));}_LL1CE: {
struct Cyc_Absyn_DynRgnType_struct*_tmp368=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp35D;if(_tmp368->tag != 16)goto _LL1D0;else{_tmp369=(void*)_tmp368->f1;_tmp36A=(
void*)_tmp368->f2;}}_LL1CF: {struct Cyc_Absyn_AccessEff_struct _tmp11B5;struct Cyc_Absyn_AccessEff_struct*
_tmp11B4;return(void*)((_tmp11B4=_cycalloc(sizeof(*_tmp11B4)),((_tmp11B4[0]=((
_tmp11B5.tag=23,((_tmp11B5.f1=(void*)_tmp36A,_tmp11B5)))),_tmp11B4))));}_LL1D0: {
struct Cyc_Absyn_DatatypeType_struct*_tmp36B=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp35D;if(_tmp36B->tag != 3)goto _LL1D2;else{_tmp36C=_tmp36B->f1;_tmp36D=_tmp36C.targs;}}
_LL1D1: {struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp36D);struct Cyc_Absyn_JoinEff_struct
_tmp11B8;struct Cyc_Absyn_JoinEff_struct*_tmp11B7;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11B7=_cycalloc(sizeof(*_tmp11B7)),((_tmp11B7[0]=((_tmp11B8.tag=24,((
_tmp11B8.f1=ts,_tmp11B8)))),_tmp11B7)))));}_LL1D2: {struct Cyc_Absyn_PointerType_struct*
_tmp36E=(struct Cyc_Absyn_PointerType_struct*)_tmp35D;if(_tmp36E->tag != 5)goto
_LL1D4;else{_tmp36F=_tmp36E->f1;_tmp370=_tmp36F.elt_typ;_tmp371=_tmp36F.ptr_atts;
_tmp372=_tmp371.rgn;}}_LL1D3: {struct Cyc_Absyn_JoinEff_struct _tmp11C7;struct Cyc_Absyn_AccessEff_struct*
_tmp11C6;struct Cyc_Absyn_AccessEff_struct _tmp11C5;void*_tmp11C4[2];struct Cyc_Absyn_JoinEff_struct*
_tmp11C3;return Cyc_Tcutil_normalize_effect((void*)((_tmp11C3=_cycalloc(sizeof(*
_tmp11C3)),((_tmp11C3[0]=((_tmp11C7.tag=24,((_tmp11C7.f1=((_tmp11C4[1]=Cyc_Tcutil_rgns_of(
_tmp370),((_tmp11C4[0]=(void*)((_tmp11C6=_cycalloc(sizeof(*_tmp11C6)),((_tmp11C6[
0]=((_tmp11C5.tag=23,((_tmp11C5.f1=(void*)_tmp372,_tmp11C5)))),_tmp11C6)))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp11C4,sizeof(void*),2)))))),_tmp11C7)))),_tmp11C3)))));}_LL1D4: {struct Cyc_Absyn_ArrayType_struct*
_tmp373=(struct Cyc_Absyn_ArrayType_struct*)_tmp35D;if(_tmp373->tag != 8)goto
_LL1D6;else{_tmp374=_tmp373->f1;_tmp375=_tmp374.elt_type;}}_LL1D5: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp375));_LL1D6: {struct Cyc_Absyn_TupleType_struct*_tmp376=(
struct Cyc_Absyn_TupleType_struct*)_tmp35D;if(_tmp376->tag != 10)goto _LL1D8;else{
_tmp377=_tmp376->f1;}}_LL1D7: {struct Cyc_List_List*_tmp3AA=0;for(0;_tmp377 != 0;
_tmp377=_tmp377->tl){struct Cyc_List_List*_tmp11C8;_tmp3AA=((_tmp11C8=_cycalloc(
sizeof(*_tmp11C8)),((_tmp11C8->hd=(*((struct _tuple12*)_tmp377->hd)).f2,((
_tmp11C8->tl=_tmp3AA,_tmp11C8))))));}_tmp37A=_tmp3AA;goto _LL1D9;}_LL1D8: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp378=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp35D;if(_tmp378->tag != 4)goto _LL1DA;else{_tmp379=_tmp378->f1;_tmp37A=_tmp379.targs;}}
_LL1D9: _tmp37D=_tmp37A;goto _LL1DB;_LL1DA: {struct Cyc_Absyn_AggrType_struct*
_tmp37B=(struct Cyc_Absyn_AggrType_struct*)_tmp35D;if(_tmp37B->tag != 11)goto
_LL1DC;else{_tmp37C=_tmp37B->f1;_tmp37D=_tmp37C.targs;}}_LL1DB: {struct Cyc_Absyn_JoinEff_struct
_tmp11CB;struct Cyc_Absyn_JoinEff_struct*_tmp11CA;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11CA=_cycalloc(sizeof(*_tmp11CA)),((_tmp11CA[0]=((_tmp11CB.tag=24,((
_tmp11CB.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp37D),_tmp11CB)))),_tmp11CA)))));}_LL1DC: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp37E=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp35D;if(_tmp37E->tag != 12)goto _LL1DE;else{_tmp37F=_tmp37E->f2;}}_LL1DD: {
struct Cyc_Absyn_JoinEff_struct _tmp11CE;struct Cyc_Absyn_JoinEff_struct*_tmp11CD;
return Cyc_Tcutil_normalize_effect((void*)((_tmp11CD=_cycalloc(sizeof(*_tmp11CD)),((
_tmp11CD[0]=((_tmp11CE.tag=24,((_tmp11CE.f1=((struct Cyc_List_List*(*)(void*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,
_tmp37F),_tmp11CE)))),_tmp11CD)))));}_LL1DE: {struct Cyc_Absyn_TagType_struct*
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
_tmp11D1;struct Cyc_Absyn_JoinEff_struct*_tmp11D0;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11D0=_cycalloc(sizeof(*_tmp11D0)),((_tmp11D0[0]=((_tmp11D1.tag=24,((
_tmp11D1.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp392),_tmp11D1)))),_tmp11D0)))));}_LL1F0: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp393=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp35D;if(_tmp393->tag != 26)goto _LL1BB;}_LL1F1: {const char*_tmp11D4;void*
_tmp11D3;(_tmp11D3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp11D4="typedecl in rgns_of",_tag_dyneither(_tmp11D4,
sizeof(char),20))),_tag_dyneither(_tmp11D3,sizeof(void*),0)));}_LL1BB:;}void*Cyc_Tcutil_normalize_effect(
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
_LL215: goto _LL20D;_LL216:;_LL217:{struct Cyc_List_List*_tmp11D5;effects=((
_tmp11D5=_cycalloc(sizeof(*_tmp11D5)),((_tmp11D5->hd=_tmp3C7,((_tmp11D5->tl=
effects,_tmp11D5))))));}goto _LL20D;_LL20D:;}}*_tmp3B8=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);return e;};}_LL1FE: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3B9=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3B5;if(_tmp3B9->tag != 25)goto _LL200;
else{_tmp3BA=(void*)_tmp3B9->f1;}}_LL1FF: {void*_tmp3D4=Cyc_Tcutil_compress(
_tmp3BA);_LL219: {struct Cyc_Absyn_Evar_struct*_tmp3D5=(struct Cyc_Absyn_Evar_struct*)
_tmp3D4;if(_tmp3D5->tag != 1)goto _LL21B;}_LL21A: goto _LL21C;_LL21B: {struct Cyc_Absyn_VarType_struct*
_tmp3D6=(struct Cyc_Absyn_VarType_struct*)_tmp3D4;if(_tmp3D6->tag != 2)goto _LL21D;}
_LL21C: return e;_LL21D:;_LL21E: return Cyc_Tcutil_rgns_of(_tmp3BA);_LL218:;}_LL200:;
_LL201: return e;_LL1FB:;};}static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct
_tmp11DF;struct Cyc_Core_Opt*_tmp11DE;struct Cyc_Absyn_FnInfo _tmp11DD;struct Cyc_Absyn_FnType_struct*
_tmp11DC;struct Cyc_Absyn_FnType_struct*_tmp3D7=(_tmp11DC=_cycalloc(sizeof(*
_tmp11DC)),((_tmp11DC[0]=((_tmp11DF.tag=9,((_tmp11DF.f1=((_tmp11DD.tvars=0,((
_tmp11DD.effect=((_tmp11DE=_cycalloc(sizeof(*_tmp11DE)),((_tmp11DE->v=eff,
_tmp11DE)))),((_tmp11DD.ret_tqual=Cyc_Absyn_empty_tqual(0),((_tmp11DD.ret_typ=(
void*)& Cyc_Absyn_VoidType_val,((_tmp11DD.args=0,((_tmp11DD.c_varargs=0,((
_tmp11DD.cyc_varargs=0,((_tmp11DD.rgn_po=0,((_tmp11DD.attributes=0,_tmp11DD)))))))))))))))))),
_tmp11DF)))),_tmp11DC)));return Cyc_Absyn_atb_typ((void*)_tmp3D7,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e){r=Cyc_Tcutil_compress(r);if((r == (void*)& Cyc_Absyn_HeapRgn_val
 || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{void*_tmp3DC=Cyc_Tcutil_compress(e);void*_tmp3DE;struct Cyc_List_List*
_tmp3E0;void*_tmp3E2;struct Cyc_Core_Opt*_tmp3E4;struct Cyc_Core_Opt*_tmp3E5;
struct Cyc_Core_Opt**_tmp3E6;struct Cyc_Core_Opt*_tmp3E7;_LL220: {struct Cyc_Absyn_AccessEff_struct*
_tmp3DD=(struct Cyc_Absyn_AccessEff_struct*)_tmp3DC;if(_tmp3DD->tag != 23)goto
_LL222;else{_tmp3DE=(void*)_tmp3DD->f1;}}_LL221: if(constrain)return Cyc_Tcutil_unify(
r,_tmp3DE);_tmp3DE=Cyc_Tcutil_compress(_tmp3DE);if(r == _tmp3DE)return 1;{struct
_tuple0 _tmp11E0;struct _tuple0 _tmp3E9=(_tmp11E0.f1=r,((_tmp11E0.f2=_tmp3DE,
_tmp11E0)));void*_tmp3EA;struct Cyc_Absyn_Tvar*_tmp3EC;void*_tmp3ED;struct Cyc_Absyn_Tvar*
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
_check_null(_tmp3F8))->v,r);{struct Cyc_Absyn_JoinEff_struct _tmp11EF;struct Cyc_Absyn_AccessEff_struct*
_tmp11EE;struct Cyc_Absyn_AccessEff_struct _tmp11ED;void*_tmp11EC[2];struct Cyc_Absyn_JoinEff_struct*
_tmp11EB;void*_tmp3FA=Cyc_Tcutil_dummy_fntype((void*)((_tmp11EB=_cycalloc(
sizeof(*_tmp11EB)),((_tmp11EB[0]=((_tmp11EF.tag=24,((_tmp11EF.f1=((_tmp11EC[1]=(
void*)((_tmp11EE=_cycalloc(sizeof(*_tmp11EE)),((_tmp11EE[0]=((_tmp11ED.tag=23,((
_tmp11ED.f1=(void*)r,_tmp11ED)))),_tmp11EE)))),((_tmp11EC[0]=_tmp3F9,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp11EC,sizeof(void*),2)))))),
_tmp11EF)))),_tmp11EB)))));{struct Cyc_Core_Opt*_tmp11F0;*_tmp3F7=((_tmp11F0=
_cycalloc(sizeof(*_tmp11F0)),((_tmp11F0->v=_tmp3FA,_tmp11F0))));}return 1;};}
_LL237:;_LL238: return 0;_LL234:;};_LL232:;_LL233: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp3F0);_LL22F:;}_LL226: {struct Cyc_Absyn_Evar_struct*_tmp3E3=(
struct Cyc_Absyn_Evar_struct*)_tmp3DC;if(_tmp3E3->tag != 1)goto _LL228;else{_tmp3E4=
_tmp3E3->f1;_tmp3E5=_tmp3E3->f2;_tmp3E6=(struct Cyc_Core_Opt**)& _tmp3E3->f2;
_tmp3E7=_tmp3E3->f4;}}_LL227: if(_tmp3E4 == 0  || ((struct Cyc_Absyn_Kind*)_tmp3E4->v)->kind
!= Cyc_Absyn_EffKind){const char*_tmp11F3;void*_tmp11F2;(_tmp11F2=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11F3="effect evar has wrong kind",
_tag_dyneither(_tmp11F3,sizeof(char),27))),_tag_dyneither(_tmp11F2,sizeof(void*),
0)));}if(!constrain)return 0;{void*_tmp403=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp3E7);Cyc_Tcutil_occurs(_tmp403,Cyc_Core_heap_region,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3E7))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmp1208;struct Cyc_List_List*_tmp1207;struct Cyc_Absyn_AccessEff_struct _tmp1206;
struct Cyc_Absyn_AccessEff_struct*_tmp1205;struct Cyc_List_List*_tmp1204;struct Cyc_Absyn_JoinEff_struct*
_tmp1203;struct Cyc_Absyn_JoinEff_struct*_tmp404=(_tmp1203=_cycalloc(sizeof(*
_tmp1203)),((_tmp1203[0]=((_tmp1208.tag=24,((_tmp1208.f1=((_tmp1204=_cycalloc(
sizeof(*_tmp1204)),((_tmp1204->hd=_tmp403,((_tmp1204->tl=((_tmp1207=_cycalloc(
sizeof(*_tmp1207)),((_tmp1207->hd=(void*)((_tmp1205=_cycalloc(sizeof(*_tmp1205)),((
_tmp1205[0]=((_tmp1206.tag=23,((_tmp1206.f1=(void*)r,_tmp1206)))),_tmp1205)))),((
_tmp1207->tl=0,_tmp1207)))))),_tmp1204)))))),_tmp1208)))),_tmp1203)));{struct Cyc_Core_Opt*
_tmp1209;*_tmp3E6=((_tmp1209=_cycalloc(sizeof(*_tmp1209)),((_tmp1209->v=(void*)
_tmp404,_tmp1209))));}return 1;};};_LL228:;_LL229: return 0;_LL21F:;};}static int Cyc_Tcutil_type_in_effect(
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
else{_tmp419=(void*)_tmp418->f1;}}_LL246: {struct _tuple0 _tmp120A;struct _tuple0
_tmp41B=(_tmp120A.f1=Cyc_Tcutil_compress(_tmp419),((_tmp120A.f2=_tmp411,_tmp120A)));
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
_tmp413->v)->kind != Cyc_Absyn_EffKind){const char*_tmp120D;void*_tmp120C;(
_tmp120C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp120D="effect evar has wrong kind",_tag_dyneither(_tmp120D,sizeof(char),27))),
_tag_dyneither(_tmp120C,sizeof(void*),0)));}if(!may_constrain_evars)return 0;{
void*_tmp424=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp416);
Cyc_Tcutil_occurs(_tmp424,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp416))->v,t);{struct Cyc_Absyn_JoinEff_struct _tmp1222;struct Cyc_List_List*
_tmp1221;struct Cyc_Absyn_RgnsEff_struct _tmp1220;struct Cyc_Absyn_RgnsEff_struct*
_tmp121F;struct Cyc_List_List*_tmp121E;struct Cyc_Absyn_JoinEff_struct*_tmp121D;
struct Cyc_Absyn_JoinEff_struct*_tmp425=(_tmp121D=_cycalloc(sizeof(*_tmp121D)),((
_tmp121D[0]=((_tmp1222.tag=24,((_tmp1222.f1=((_tmp121E=_cycalloc(sizeof(*
_tmp121E)),((_tmp121E->hd=_tmp424,((_tmp121E->tl=((_tmp1221=_cycalloc(sizeof(*
_tmp1221)),((_tmp1221->hd=(void*)((_tmp121F=_cycalloc(sizeof(*_tmp121F)),((
_tmp121F[0]=((_tmp1220.tag=25,((_tmp1220.f1=(void*)t,_tmp1220)))),_tmp121F)))),((
_tmp1221->tl=0,_tmp1221)))))),_tmp121E)))))),_tmp1222)))),_tmp121D)));{struct Cyc_Core_Opt*
_tmp1223;*_tmp415=((_tmp1223=_cycalloc(sizeof(*_tmp1223)),((_tmp1223->v=(void*)
_tmp425,_tmp1223))));}return 1;};};_LL242:;_LL243: return 0;_LL239:;};}static int Cyc_Tcutil_variable_in_effect(
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
struct Cyc_Absyn_JoinEff_struct _tmp1232;struct Cyc_Absyn_VarType_struct*_tmp1231;
struct Cyc_Absyn_VarType_struct _tmp1230;void*_tmp122F[2];struct Cyc_Absyn_JoinEff_struct*
_tmp122E;void*_tmp443=Cyc_Tcutil_dummy_fntype((void*)((_tmp122E=_cycalloc(
sizeof(*_tmp122E)),((_tmp122E[0]=((_tmp1232.tag=24,((_tmp1232.f1=((_tmp122F[1]=(
void*)((_tmp1231=_cycalloc(sizeof(*_tmp1231)),((_tmp1231[0]=((_tmp1230.tag=2,((
_tmp1230.f1=v,_tmp1230)))),_tmp1231)))),((_tmp122F[0]=_tmp442,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp122F,sizeof(void*),2)))))),
_tmp1232)))),_tmp122E)))));{struct Cyc_Core_Opt*_tmp1233;*_tmp440=((_tmp1233=
_cycalloc(sizeof(*_tmp1233)),((_tmp1233->v=_tmp443,_tmp1233))));}return 1;};}
_LL261:;_LL262: return 0;_LL25E:;};_LL25C:;_LL25D: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp439);_LL259:;}_LL255: {struct Cyc_Absyn_Evar_struct*
_tmp434=(struct Cyc_Absyn_Evar_struct*)_tmp42D;if(_tmp434->tag != 1)goto _LL257;
else{_tmp435=_tmp434->f1;_tmp436=_tmp434->f2;_tmp437=(struct Cyc_Core_Opt**)&
_tmp434->f2;_tmp438=_tmp434->f4;}}_LL256: if(_tmp435 == 0  || ((struct Cyc_Absyn_Kind*)
_tmp435->v)->kind != Cyc_Absyn_EffKind){const char*_tmp1236;void*_tmp1235;(
_tmp1235=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1236="effect evar has wrong kind",_tag_dyneither(_tmp1236,sizeof(char),27))),
_tag_dyneither(_tmp1235,sizeof(void*),0)));}{void*_tmp44C=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp438);if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp438))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct _tmp124B;
struct Cyc_List_List*_tmp124A;struct Cyc_Absyn_VarType_struct _tmp1249;struct Cyc_Absyn_VarType_struct*
_tmp1248;struct Cyc_List_List*_tmp1247;struct Cyc_Absyn_JoinEff_struct*_tmp1246;
struct Cyc_Absyn_JoinEff_struct*_tmp44D=(_tmp1246=_cycalloc(sizeof(*_tmp1246)),((
_tmp1246[0]=((_tmp124B.tag=24,((_tmp124B.f1=((_tmp1247=_cycalloc(sizeof(*
_tmp1247)),((_tmp1247->hd=_tmp44C,((_tmp1247->tl=((_tmp124A=_cycalloc(sizeof(*
_tmp124A)),((_tmp124A->hd=(void*)((_tmp1248=_cycalloc(sizeof(*_tmp1248)),((
_tmp1248[0]=((_tmp1249.tag=2,((_tmp1249.f1=v,_tmp1249)))),_tmp1248)))),((
_tmp124A->tl=0,_tmp124A)))))),_tmp1247)))))),_tmp124B)))),_tmp1246)));{struct Cyc_Core_Opt*
_tmp124C;*_tmp437=((_tmp124C=_cycalloc(sizeof(*_tmp124C)),((_tmp124C->v=(void*)
_tmp44D,_tmp124C))));}return 1;};};_LL257:;_LL258: return 0;_LL24E:;};}static int Cyc_Tcutil_evar_in_effect(
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
e1,e2)){struct Cyc_Core_Opt*_tmp124D;*_tmp469=((_tmp124D=_cycalloc(sizeof(*
_tmp124D)),((_tmp124D->v=Cyc_Absyn_empty_effect,_tmp124D))));}return 1;_LL27C:;
_LL27D: {const char*_tmp1251;void*_tmp1250[1];struct Cyc_String_pa_struct _tmp124F;(
_tmp124F.tag=0,((_tmp124F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e1)),((_tmp1250[0]=& _tmp124F,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1251="subset_effect: bad effect: %s",
_tag_dyneither(_tmp1251,sizeof(char),30))),_tag_dyneither(_tmp1250,sizeof(void*),
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
Cyc_Core_Opt*_tmp1252;*_tmp47E=((_tmp1252=_cycalloc(sizeof(*_tmp1252)),((
_tmp1252->v=t2,_tmp1252))));}return;}else{{void*_tmp482=t2;struct Cyc_Core_Opt*
_tmp484;struct Cyc_Core_Opt**_tmp485;struct Cyc_Core_Opt*_tmp486;struct Cyc_Absyn_PtrInfo
_tmp488;_LL289: {struct Cyc_Absyn_Evar_struct*_tmp483=(struct Cyc_Absyn_Evar_struct*)
_tmp482;if(_tmp483->tag != 1)goto _LL28B;else{_tmp484=_tmp483->f2;_tmp485=(struct
Cyc_Core_Opt**)& _tmp483->f2;_tmp486=_tmp483->f4;}}_LL28A: {struct Cyc_List_List*
_tmp489=(struct Cyc_List_List*)_tmp47F->v;{struct Cyc_List_List*s2=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp486))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp489,(struct
Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmp1253;Cyc_Tcutil_failure_reason=((
_tmp1253="(type variable would escape scope)",_tag_dyneither(_tmp1253,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}if(Cyc_Tcutil_kind_leq((
struct Cyc_Absyn_Kind*)_tmp47C->v,_tmp480)){{struct Cyc_Core_Opt*_tmp1254;*_tmp485=((
_tmp1254=_cycalloc(sizeof(*_tmp1254)),((_tmp1254->v=t1,_tmp1254))));}return;}{
const char*_tmp1255;Cyc_Tcutil_failure_reason=((_tmp1255="(kinds are incompatible)",
_tag_dyneither(_tmp1255,sizeof(char),25)));}goto _LL288;}_LL28B:{struct Cyc_Absyn_PointerType_struct*
_tmp487=(struct Cyc_Absyn_PointerType_struct*)_tmp482;if(_tmp487->tag != 5)goto
_LL28D;else{_tmp488=_tmp487->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp47C->v)->kind
== Cyc_Absyn_BoxKind))goto _LL28D;_LL28C: {union Cyc_Absyn_Constraint*_tmp48D=((
union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((
_tmp488.ptr_atts).bounds);{union Cyc_Absyn_Constraint*_tmp48E=_tmp48D;union Cyc_Absyn_Constraint
_tmp48F;int _tmp490;_LL290: _tmp48F=*_tmp48E;if((_tmp48F.No_constr).tag != 3)goto
_LL292;_tmp490=(int)(_tmp48F.No_constr).val;_LL291:{struct
_union_Constraint_Eq_constr*_tmp1256;(_tmp1256=& _tmp48D->Eq_constr,((_tmp1256->tag=
1,_tmp1256->val=Cyc_Absyn_bounds_one)));}{struct Cyc_Core_Opt*_tmp1257;*_tmp47E=((
_tmp1257=_cycalloc(sizeof(*_tmp1257)),((_tmp1257->v=t2,_tmp1257))));}return;
_LL292:;_LL293: goto _LL28F;_LL28F:;}goto _LL288;}_LL28D:;_LL28E: goto _LL288;_LL288:;}{
const char*_tmp1258;Cyc_Tcutil_failure_reason=((_tmp1258="(kinds are incompatible)",
_tag_dyneither(_tmp1258,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};
_LL286:;_LL287: goto _LL283;_LL283:;}{struct _tuple0 _tmp1259;struct _tuple0 _tmp495=(
_tmp1259.f1=t2,((_tmp1259.f2=t1,_tmp1259)));void*_tmp496;void*_tmp498;void*
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
enum Cyc_Absyn_AggrKind _tmp536;struct Cyc_List_List*_tmp537;void*_tmp538;void*
_tmp53A;void*_tmp53C;void*_tmp53E;void*_tmp540;void*_tmp542;void*_tmp544;void*
_tmp546;void*_tmp547;void*_tmp549;void*_tmp54A;void*_tmp54C;void*_tmp54D;void*
_tmp54E;void*_tmp550;void*_tmp551;void*_tmp552;void*_tmp554;void*_tmp556;void*
_tmp558;void*_tmp55A;void*_tmp55C;_LL295: _tmp496=_tmp495.f1;{struct Cyc_Absyn_Evar_struct*
_tmp497=(struct Cyc_Absyn_Evar_struct*)_tmp496;if(_tmp497->tag != 1)goto _LL297;};
_LL296: Cyc_Tcutil_unify_it(t2,t1);return;_LL297: _tmp498=_tmp495.f1;{struct Cyc_Absyn_VoidType_struct*
_tmp499=(struct Cyc_Absyn_VoidType_struct*)_tmp498;if(_tmp499->tag != 0)goto _LL299;};
_tmp49A=_tmp495.f2;{struct Cyc_Absyn_VoidType_struct*_tmp49B=(struct Cyc_Absyn_VoidType_struct*)
_tmp49A;if(_tmp49B->tag != 0)goto _LL299;};_LL298: return;_LL299: _tmp49C=_tmp495.f1;{
struct Cyc_Absyn_VarType_struct*_tmp49D=(struct Cyc_Absyn_VarType_struct*)_tmp49C;
if(_tmp49D->tag != 2)goto _LL29B;else{_tmp49E=_tmp49D->f1;}};_tmp49F=_tmp495.f2;{
struct Cyc_Absyn_VarType_struct*_tmp4A0=(struct Cyc_Absyn_VarType_struct*)_tmp49F;
if(_tmp4A0->tag != 2)goto _LL29B;else{_tmp4A1=_tmp4A0->f1;}};_LL29A: {struct
_dyneither_ptr*_tmp55E=_tmp49E->name;struct _dyneither_ptr*_tmp55F=_tmp4A1->name;
int _tmp560=_tmp49E->identity;int _tmp561=_tmp4A1->identity;if(_tmp561 == _tmp560)
return;{const char*_tmp125A;Cyc_Tcutil_failure_reason=((_tmp125A="(variable types are not the same)",
_tag_dyneither(_tmp125A,sizeof(char),34)));}goto _LL294;}_LL29B: _tmp4A2=_tmp495.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp4A3=(struct Cyc_Absyn_AggrType_struct*)
_tmp4A2;if(_tmp4A3->tag != 11)goto _LL29D;else{_tmp4A4=_tmp4A3->f1;_tmp4A5=_tmp4A4.aggr_info;
_tmp4A6=_tmp4A4.targs;}};_tmp4A7=_tmp495.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp4A8=(struct Cyc_Absyn_AggrType_struct*)_tmp4A7;if(_tmp4A8->tag != 11)goto
_LL29D;else{_tmp4A9=_tmp4A8->f1;_tmp4AA=_tmp4A9.aggr_info;_tmp4AB=_tmp4A9.targs;}};
_LL29C: {enum Cyc_Absyn_AggrKind _tmp564;struct _tuple2*_tmp565;struct _tuple13
_tmp563=Cyc_Absyn_aggr_kinded_name(_tmp4AA);_tmp564=_tmp563.f1;_tmp565=_tmp563.f2;{
enum Cyc_Absyn_AggrKind _tmp567;struct _tuple2*_tmp568;struct _tuple13 _tmp566=Cyc_Absyn_aggr_kinded_name(
_tmp4A5);_tmp567=_tmp566.f1;_tmp568=_tmp566.f2;if(_tmp564 != _tmp567){{const char*
_tmp125B;Cyc_Tcutil_failure_reason=((_tmp125B="(struct and union type)",
_tag_dyneither(_tmp125B,sizeof(char),24)));}goto _LL294;}if(Cyc_Absyn_qvar_cmp(
_tmp565,_tmp568)!= 0){{const char*_tmp125C;Cyc_Tcutil_failure_reason=((_tmp125C="(different type name)",
_tag_dyneither(_tmp125C,sizeof(char),22)));}goto _LL294;}Cyc_Tcutil_unify_list(
_tmp4AB,_tmp4A6);return;};}_LL29D: _tmp4AC=_tmp495.f1;{struct Cyc_Absyn_EnumType_struct*
_tmp4AD=(struct Cyc_Absyn_EnumType_struct*)_tmp4AC;if(_tmp4AD->tag != 13)goto
_LL29F;else{_tmp4AE=_tmp4AD->f1;}};_tmp4AF=_tmp495.f2;{struct Cyc_Absyn_EnumType_struct*
_tmp4B0=(struct Cyc_Absyn_EnumType_struct*)_tmp4AF;if(_tmp4B0->tag != 13)goto
_LL29F;else{_tmp4B1=_tmp4B0->f1;}};_LL29E: if(Cyc_Absyn_qvar_cmp(_tmp4AE,_tmp4B1)
== 0)return;{const char*_tmp125D;Cyc_Tcutil_failure_reason=((_tmp125D="(different enum types)",
_tag_dyneither(_tmp125D,sizeof(char),23)));}goto _LL294;_LL29F: _tmp4B2=_tmp495.f1;{
struct Cyc_Absyn_AnonEnumType_struct*_tmp4B3=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp4B2;if(_tmp4B3->tag != 14)goto _LL2A1;else{_tmp4B4=_tmp4B3->f1;}};_tmp4B5=
_tmp495.f2;{struct Cyc_Absyn_AnonEnumType_struct*_tmp4B6=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp4B5;if(_tmp4B6->tag != 14)goto _LL2A1;else{_tmp4B7=_tmp4B6->f1;}};_LL2A0: {int
bad=0;for(0;_tmp4B4 != 0  && _tmp4B7 != 0;(_tmp4B4=_tmp4B4->tl,_tmp4B7=_tmp4B7->tl)){
struct Cyc_Absyn_Enumfield*_tmp56C=(struct Cyc_Absyn_Enumfield*)_tmp4B4->hd;struct
Cyc_Absyn_Enumfield*_tmp56D=(struct Cyc_Absyn_Enumfield*)_tmp4B7->hd;if(Cyc_Absyn_qvar_cmp(
_tmp56C->name,_tmp56D->name)!= 0){{const char*_tmp125E;Cyc_Tcutil_failure_reason=((
_tmp125E="(different names for enum fields)",_tag_dyneither(_tmp125E,sizeof(char),
34)));}bad=1;break;}if(_tmp56C->tag == _tmp56D->tag)continue;if(_tmp56C->tag == 0
 || _tmp56D->tag == 0){{const char*_tmp125F;Cyc_Tcutil_failure_reason=((_tmp125F="(different tag values for enum fields)",
_tag_dyneither(_tmp125F,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp56C->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp56D->tag))){{const char*_tmp1260;Cyc_Tcutil_failure_reason=((_tmp1260="(different tag values for enum fields)",
_tag_dyneither(_tmp1260,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL294;if(
_tmp4B4 == 0  && _tmp4B7 == 0)return;{const char*_tmp1261;Cyc_Tcutil_failure_reason=((
_tmp1261="(different number of fields for enums)",_tag_dyneither(_tmp1261,
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
_tmp1262;Cyc_Tcutil_failure_reason=((_tmp1262="(different datatype field types)",
_tag_dyneither(_tmp1262,sizeof(char),33)));}goto _LL294;_LL2A5: _tmp4D6=_tmp495.f1;{
struct Cyc_Absyn_PointerType_struct*_tmp4D7=(struct Cyc_Absyn_PointerType_struct*)
_tmp4D6;if(_tmp4D7->tag != 5)goto _LL2A7;else{_tmp4D8=_tmp4D7->f1;_tmp4D9=_tmp4D8.elt_typ;
_tmp4DA=_tmp4D8.elt_tq;_tmp4DB=_tmp4D8.ptr_atts;_tmp4DC=_tmp4DB.rgn;_tmp4DD=
_tmp4DB.nullable;_tmp4DE=_tmp4DB.bounds;_tmp4DF=_tmp4DB.zero_term;}};_tmp4E0=
_tmp495.f2;{struct Cyc_Absyn_PointerType_struct*_tmp4E1=(struct Cyc_Absyn_PointerType_struct*)
_tmp4E0;if(_tmp4E1->tag != 5)goto _LL2A7;else{_tmp4E2=_tmp4E1->f1;_tmp4E3=_tmp4E2.elt_typ;
_tmp4E4=_tmp4E2.elt_tq;_tmp4E5=_tmp4E2.ptr_atts;_tmp4E6=_tmp4E5.rgn;_tmp4E7=
_tmp4E5.nullable;_tmp4E8=_tmp4E5.bounds;_tmp4E9=_tmp4E5.zero_term;}};_LL2A6: Cyc_Tcutil_unify_it(
_tmp4E3,_tmp4D9);Cyc_Tcutil_unify_it(_tmp4DC,_tmp4E6);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{const char*_tmp1263;((void(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp4E9,_tmp4DF,((_tmp1263="(not both zero terminated)",
_tag_dyneither(_tmp1263,sizeof(char),27))));}Cyc_Tcutil_unify_tqual(_tmp4E4,
_tmp4E3,_tmp4DA,_tmp4D9);{const char*_tmp1264;((void(*)(int(*cmp)(void*,void*),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr
reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp4E8,_tmp4DE,((
_tmp1264="(different pointer bounds)",_tag_dyneither(_tmp1264,sizeof(char),27))));}{
void*_tmp575=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp4E8);_LL2D0: {struct Cyc_Absyn_DynEither_b_struct*
_tmp576=(struct Cyc_Absyn_DynEither_b_struct*)_tmp575;if(_tmp576->tag != 0)goto
_LL2D2;}_LL2D1: return;_LL2D2:;_LL2D3: goto _LL2CF;_LL2CF:;}{const char*_tmp1265;((
void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp4E7,_tmp4DD,((_tmp1265="(incompatible pointer types)",_tag_dyneither(
_tmp1265,sizeof(char),29))));}return;_LL2A7: _tmp4EA=_tmp495.f1;{struct Cyc_Absyn_IntType_struct*
_tmp4EB=(struct Cyc_Absyn_IntType_struct*)_tmp4EA;if(_tmp4EB->tag != 6)goto _LL2A9;
else{_tmp4EC=_tmp4EB->f1;_tmp4ED=_tmp4EB->f2;}};_tmp4EE=_tmp495.f2;{struct Cyc_Absyn_IntType_struct*
_tmp4EF=(struct Cyc_Absyn_IntType_struct*)_tmp4EE;if(_tmp4EF->tag != 6)goto _LL2A9;
else{_tmp4F0=_tmp4EF->f1;_tmp4F1=_tmp4EF->f2;}};_LL2A8: if(_tmp4F0 == _tmp4EC  && ((
_tmp4F1 == _tmp4ED  || _tmp4F1 == Cyc_Absyn_Int_sz  && _tmp4ED == Cyc_Absyn_Long_sz)
 || _tmp4F1 == Cyc_Absyn_Long_sz  && _tmp4ED == Cyc_Absyn_Int_sz))return;{const char*
_tmp1266;Cyc_Tcutil_failure_reason=((_tmp1266="(different integral types)",
_tag_dyneither(_tmp1266,sizeof(char),27)));}goto _LL294;_LL2A9: _tmp4F2=_tmp495.f1;{
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
_tmp503)){{const char*_tmp1267;Cyc_Tcutil_failure_reason=((_tmp1267="(cannot prove expressions are the same)",
_tag_dyneither(_tmp1267,sizeof(char),40)));}goto _LL294;}return;_LL2AF: _tmp504=
_tmp495.f1;{struct Cyc_Absyn_ArrayType_struct*_tmp505=(struct Cyc_Absyn_ArrayType_struct*)
_tmp504;if(_tmp505->tag != 8)goto _LL2B1;else{_tmp506=_tmp505->f1;_tmp507=_tmp506.elt_type;
_tmp508=_tmp506.tq;_tmp509=_tmp506.num_elts;_tmp50A=_tmp506.zero_term;}};_tmp50B=
_tmp495.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp50C=(struct Cyc_Absyn_ArrayType_struct*)
_tmp50B;if(_tmp50C->tag != 8)goto _LL2B1;else{_tmp50D=_tmp50C->f1;_tmp50E=_tmp50D.elt_type;
_tmp50F=_tmp50D.tq;_tmp510=_tmp50D.num_elts;_tmp511=_tmp50D.zero_term;}};_LL2B0:
Cyc_Tcutil_unify_it(_tmp50E,_tmp507);Cyc_Tcutil_unify_tqual(_tmp50F,_tmp50E,
_tmp508,_tmp507);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*
_tmp1268;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp50A,_tmp511,((_tmp1268="(not both zero terminated)",_tag_dyneither(_tmp1268,
sizeof(char),27))));}if(_tmp509 == _tmp510)return;if(_tmp509 == 0  || _tmp510 == 0)
goto _LL294;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp509,(struct Cyc_Absyn_Exp*)
_tmp510))return;{const char*_tmp1269;Cyc_Tcutil_failure_reason=((_tmp1269="(different array sizes)",
_tag_dyneither(_tmp1269,sizeof(char),24)));}goto _LL294;_LL2B1: _tmp512=_tmp495.f1;{
struct Cyc_Absyn_FnType_struct*_tmp513=(struct Cyc_Absyn_FnType_struct*)_tmp512;
if(_tmp513->tag != 9)goto _LL2B3;else{_tmp514=_tmp513->f1;_tmp515=_tmp514.tvars;
_tmp516=_tmp514.effect;_tmp517=_tmp514.ret_tqual;_tmp518=_tmp514.ret_typ;_tmp519=
_tmp514.args;_tmp51A=_tmp514.c_varargs;_tmp51B=_tmp514.cyc_varargs;_tmp51C=
_tmp514.rgn_po;_tmp51D=_tmp514.attributes;}};_tmp51E=_tmp495.f2;{struct Cyc_Absyn_FnType_struct*
_tmp51F=(struct Cyc_Absyn_FnType_struct*)_tmp51E;if(_tmp51F->tag != 9)goto _LL2B3;
else{_tmp520=_tmp51F->f1;_tmp521=_tmp520.tvars;_tmp522=_tmp520.effect;_tmp523=
_tmp520.ret_tqual;_tmp524=_tmp520.ret_typ;_tmp525=_tmp520.args;_tmp526=_tmp520.c_varargs;
_tmp527=_tmp520.cyc_varargs;_tmp528=_tmp520.rgn_po;_tmp529=_tmp520.attributes;}};
_LL2B2: {int done=0;struct _RegionHandle _tmp57C=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp57C;_push_region(rgn);{struct Cyc_List_List*inst=0;while(
_tmp521 != 0){if(_tmp515 == 0){{const char*_tmp126A;Cyc_Tcutil_failure_reason=((
_tmp126A="(second function type has too few type variables)",_tag_dyneither(
_tmp126A,sizeof(char),50)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{void*
_tmp57E=((struct Cyc_Absyn_Tvar*)_tmp521->hd)->kind;void*_tmp57F=((struct Cyc_Absyn_Tvar*)
_tmp515->hd)->kind;if(!Cyc_Tcutil_unify_kindbound(_tmp57E,_tmp57F)){{const char*
_tmp1270;void*_tmp126F[3];struct Cyc_String_pa_struct _tmp126E;struct Cyc_String_pa_struct
_tmp126D;struct Cyc_String_pa_struct _tmp126C;Cyc_Tcutil_failure_reason=(struct
_dyneither_ptr)((_tmp126C.tag=0,((_tmp126C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp515->hd,& Cyc_Tcutil_bk))),((_tmp126D.tag=0,((_tmp126D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp521->hd,& Cyc_Tcutil_bk))),((_tmp126E.tag=0,((_tmp126E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
_tmp521->hd)),((_tmp126F[0]=& _tmp126E,((_tmp126F[1]=& _tmp126D,((_tmp126F[2]=&
_tmp126C,Cyc_aprintf(((_tmp1270="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmp1270,sizeof(char),44))),_tag_dyneither(_tmp126F,sizeof(void*),
3))))))))))))))))))));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{struct
_tuple16*_tmp127A;struct Cyc_Absyn_VarType_struct _tmp1279;struct Cyc_Absyn_VarType_struct*
_tmp1278;struct Cyc_List_List*_tmp1277;inst=((_tmp1277=_region_malloc(rgn,sizeof(*
_tmp1277)),((_tmp1277->hd=((_tmp127A=_region_malloc(rgn,sizeof(*_tmp127A)),((
_tmp127A->f1=(struct Cyc_Absyn_Tvar*)_tmp515->hd,((_tmp127A->f2=(void*)((_tmp1278=
_cycalloc(sizeof(*_tmp1278)),((_tmp1278[0]=((_tmp1279.tag=2,((_tmp1279.f1=(
struct Cyc_Absyn_Tvar*)_tmp521->hd,_tmp1279)))),_tmp1278)))),_tmp127A)))))),((
_tmp1277->tl=inst,_tmp1277))))));}_tmp521=_tmp521->tl;_tmp515=_tmp515->tl;};}if(
_tmp515 != 0){{const char*_tmp127B;Cyc_Tcutil_failure_reason=((_tmp127B="(second function type has too many type variables)",
_tag_dyneither(_tmp127B,sizeof(char),51)));}_npop_handler(0);goto _LL294;}if(inst
!= 0){{struct Cyc_Absyn_FnType_struct _tmp1287;struct Cyc_Absyn_FnInfo _tmp1286;
struct Cyc_Absyn_FnType_struct*_tmp1285;struct Cyc_Absyn_FnType_struct _tmp1281;
struct Cyc_Absyn_FnInfo _tmp1280;struct Cyc_Absyn_FnType_struct*_tmp127F;Cyc_Tcutil_unify_it((
void*)((_tmp127F=_cycalloc(sizeof(*_tmp127F)),((_tmp127F[0]=((_tmp1281.tag=9,((
_tmp1281.f1=((_tmp1280.tvars=0,((_tmp1280.effect=_tmp522,((_tmp1280.ret_tqual=
_tmp523,((_tmp1280.ret_typ=_tmp524,((_tmp1280.args=_tmp525,((_tmp1280.c_varargs=
_tmp526,((_tmp1280.cyc_varargs=_tmp527,((_tmp1280.rgn_po=_tmp528,((_tmp1280.attributes=
_tmp529,_tmp1280)))))))))))))))))),_tmp1281)))),_tmp127F)))),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)((_tmp1285=_cycalloc(sizeof(*_tmp1285)),((_tmp1285[0]=((_tmp1287.tag=
9,((_tmp1287.f1=((_tmp1286.tvars=0,((_tmp1286.effect=_tmp516,((_tmp1286.ret_tqual=
_tmp517,((_tmp1286.ret_typ=_tmp518,((_tmp1286.args=_tmp519,((_tmp1286.c_varargs=
_tmp51A,((_tmp1286.cyc_varargs=_tmp51B,((_tmp1286.rgn_po=_tmp51C,((_tmp1286.attributes=
_tmp51D,_tmp1286)))))))))))))))))),_tmp1287)))),_tmp1285))))));}done=1;}}if(done){
_npop_handler(0);return;}Cyc_Tcutil_unify_it(_tmp524,_tmp518);Cyc_Tcutil_unify_tqual(
_tmp523,_tmp524,_tmp517,_tmp518);for(0;_tmp525 != 0  && _tmp519 != 0;(_tmp525=
_tmp525->tl,_tmp519=_tmp519->tl)){struct Cyc_Absyn_Tqual _tmp591;void*_tmp592;
struct _tuple10 _tmp590=*((struct _tuple10*)_tmp525->hd);_tmp591=_tmp590.f2;_tmp592=
_tmp590.f3;{struct Cyc_Absyn_Tqual _tmp594;void*_tmp595;struct _tuple10 _tmp593=*((
struct _tuple10*)_tmp519->hd);_tmp594=_tmp593.f2;_tmp595=_tmp593.f3;Cyc_Tcutil_unify_it(
_tmp592,_tmp595);Cyc_Tcutil_unify_tqual(_tmp591,_tmp592,_tmp594,_tmp595);};}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp525 != 0  || _tmp519 != 0){{const char*_tmp1288;
Cyc_Tcutil_failure_reason=((_tmp1288="(function types have different number of arguments)",
_tag_dyneither(_tmp1288,sizeof(char),52)));}_npop_handler(0);goto _LL294;}if(
_tmp526 != _tmp51A){{const char*_tmp1289;Cyc_Tcutil_failure_reason=((_tmp1289="(only one function type takes C varargs)",
_tag_dyneither(_tmp1289,sizeof(char),41)));}_npop_handler(0);goto _LL294;}{int
bad_cyc_vararg=0;{struct _tuple22 _tmp128A;struct _tuple22 _tmp599=(_tmp128A.f1=
_tmp527,((_tmp128A.f2=_tmp51B,_tmp128A)));struct Cyc_Absyn_VarargInfo*_tmp59A;
struct Cyc_Absyn_VarargInfo*_tmp59B;struct Cyc_Absyn_VarargInfo*_tmp59C;struct Cyc_Absyn_VarargInfo*
_tmp59D;struct Cyc_Absyn_VarargInfo*_tmp59E;struct Cyc_Absyn_VarargInfo _tmp59F;
struct Cyc_Core_Opt*_tmp5A0;struct Cyc_Absyn_Tqual _tmp5A1;void*_tmp5A2;int _tmp5A3;
struct Cyc_Absyn_VarargInfo*_tmp5A4;struct Cyc_Absyn_VarargInfo _tmp5A5;struct Cyc_Core_Opt*
_tmp5A6;struct Cyc_Absyn_Tqual _tmp5A7;void*_tmp5A8;int _tmp5A9;_LL2D5: _tmp59A=
_tmp599.f1;if(_tmp59A != 0)goto _LL2D7;_tmp59B=_tmp599.f2;if(_tmp59B != 0)goto
_LL2D7;_LL2D6: goto _LL2D4;_LL2D7: _tmp59C=_tmp599.f1;if(_tmp59C != 0)goto _LL2D9;
_LL2D8: goto _LL2DA;_LL2D9: _tmp59D=_tmp599.f2;if(_tmp59D != 0)goto _LL2DB;_LL2DA:
bad_cyc_vararg=1;{const char*_tmp128B;Cyc_Tcutil_failure_reason=((_tmp128B="(only one function type takes varargs)",
_tag_dyneither(_tmp128B,sizeof(char),39)));}goto _LL2D4;_LL2DB: _tmp59E=_tmp599.f1;
if(_tmp59E == 0)goto _LL2D4;_tmp59F=*_tmp59E;_tmp5A0=_tmp59F.name;_tmp5A1=_tmp59F.tq;
_tmp5A2=_tmp59F.type;_tmp5A3=_tmp59F.inject;_tmp5A4=_tmp599.f2;if(_tmp5A4 == 0)
goto _LL2D4;_tmp5A5=*_tmp5A4;_tmp5A6=_tmp5A5.name;_tmp5A7=_tmp5A5.tq;_tmp5A8=
_tmp5A5.type;_tmp5A9=_tmp5A5.inject;_LL2DC: Cyc_Tcutil_unify_it(_tmp5A2,_tmp5A8);
Cyc_Tcutil_unify_tqual(_tmp5A1,_tmp5A2,_tmp5A7,_tmp5A8);if(_tmp5A3 != _tmp5A9){
bad_cyc_vararg=1;{const char*_tmp128C;Cyc_Tcutil_failure_reason=((_tmp128C="(only one function type injects varargs)",
_tag_dyneither(_tmp128C,sizeof(char),41)));};}goto _LL2D4;_LL2D4:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL294;}{int bad_effect=0;{struct _tuple23
_tmp128D;struct _tuple23 _tmp5AD=(_tmp128D.f1=_tmp522,((_tmp128D.f2=_tmp516,
_tmp128D)));struct Cyc_Core_Opt*_tmp5AE;struct Cyc_Core_Opt*_tmp5AF;struct Cyc_Core_Opt*
_tmp5B0;struct Cyc_Core_Opt*_tmp5B1;_LL2DE: _tmp5AE=_tmp5AD.f1;if(_tmp5AE != 0)goto
_LL2E0;_tmp5AF=_tmp5AD.f2;if(_tmp5AF != 0)goto _LL2E0;_LL2DF: goto _LL2DD;_LL2E0:
_tmp5B0=_tmp5AD.f1;if(_tmp5B0 != 0)goto _LL2E2;_LL2E1: goto _LL2E3;_LL2E2: _tmp5B1=
_tmp5AD.f2;if(_tmp5B1 != 0)goto _LL2E4;_LL2E3: bad_effect=1;goto _LL2DD;_LL2E4:;
_LL2E5: bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp522))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp516))->v);
goto _LL2DD;_LL2DD:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(
bad_effect){{const char*_tmp128E;Cyc_Tcutil_failure_reason=((_tmp128E="(function type effects do not unify)",
_tag_dyneither(_tmp128E,sizeof(char),37)));}_npop_handler(0);goto _LL294;}if(!Cyc_Tcutil_same_atts(
_tmp51D,_tmp529)){{const char*_tmp128F;Cyc_Tcutil_failure_reason=((_tmp128F="(function types have different attributes)",
_tag_dyneither(_tmp128F,sizeof(char),43)));}_npop_handler(0);goto _LL294;}if(!Cyc_Tcutil_same_rgn_po(
_tmp51C,_tmp528)){{const char*_tmp1290;Cyc_Tcutil_failure_reason=((_tmp1290="(function types have different region lifetime orderings)",
_tag_dyneither(_tmp1290,sizeof(char),58)));}_npop_handler(0);goto _LL294;}
_npop_handler(0);return;};};;_pop_region(rgn);}_LL2B3: _tmp52A=_tmp495.f1;{struct
Cyc_Absyn_TupleType_struct*_tmp52B=(struct Cyc_Absyn_TupleType_struct*)_tmp52A;
if(_tmp52B->tag != 10)goto _LL2B5;else{_tmp52C=_tmp52B->f1;}};_tmp52D=_tmp495.f2;{
struct Cyc_Absyn_TupleType_struct*_tmp52E=(struct Cyc_Absyn_TupleType_struct*)
_tmp52D;if(_tmp52E->tag != 10)goto _LL2B5;else{_tmp52F=_tmp52E->f1;}};_LL2B4: for(0;
_tmp52F != 0  && _tmp52C != 0;(_tmp52F=_tmp52F->tl,_tmp52C=_tmp52C->tl)){struct Cyc_Absyn_Tqual
_tmp5B6;void*_tmp5B7;struct _tuple12 _tmp5B5=*((struct _tuple12*)_tmp52F->hd);
_tmp5B6=_tmp5B5.f1;_tmp5B7=_tmp5B5.f2;{struct Cyc_Absyn_Tqual _tmp5B9;void*_tmp5BA;
struct _tuple12 _tmp5B8=*((struct _tuple12*)_tmp52C->hd);_tmp5B9=_tmp5B8.f1;_tmp5BA=
_tmp5B8.f2;Cyc_Tcutil_unify_it(_tmp5B7,_tmp5BA);Cyc_Tcutil_unify_tqual(_tmp5B6,
_tmp5B7,_tmp5B9,_tmp5BA);};}if(_tmp52F == 0  && _tmp52C == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmp1291;Cyc_Tcutil_failure_reason=((
_tmp1291="(tuple types have different numbers of components)",_tag_dyneither(
_tmp1291,sizeof(char),51)));}goto _LL294;_LL2B5: _tmp530=_tmp495.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp531=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp530;if(_tmp531->tag != 12)goto
_LL2B7;else{_tmp532=_tmp531->f1;_tmp533=_tmp531->f2;}};_tmp534=_tmp495.f2;{
struct Cyc_Absyn_AnonAggrType_struct*_tmp535=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp534;if(_tmp535->tag != 12)goto _LL2B7;else{_tmp536=_tmp535->f1;_tmp537=_tmp535->f2;}};
_LL2B6: if(_tmp536 != _tmp532){{const char*_tmp1292;Cyc_Tcutil_failure_reason=((
_tmp1292="(struct and union type)",_tag_dyneither(_tmp1292,sizeof(char),24)));}
goto _LL294;}for(0;_tmp537 != 0  && _tmp533 != 0;(_tmp537=_tmp537->tl,_tmp533=
_tmp533->tl)){struct Cyc_Absyn_Aggrfield*_tmp5BD=(struct Cyc_Absyn_Aggrfield*)
_tmp537->hd;struct Cyc_Absyn_Aggrfield*_tmp5BE=(struct Cyc_Absyn_Aggrfield*)
_tmp533->hd;if(Cyc_strptrcmp(_tmp5BD->name,_tmp5BE->name)!= 0){{const char*
_tmp1293;Cyc_Tcutil_failure_reason=((_tmp1293="(different member names)",
_tag_dyneither(_tmp1293,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_unify_it(_tmp5BD->type,_tmp5BE->type);Cyc_Tcutil_unify_tqual(_tmp5BD->tq,
_tmp5BD->type,_tmp5BE->tq,_tmp5BE->type);if(!Cyc_Tcutil_same_atts(_tmp5BD->attributes,
_tmp5BE->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const
char*_tmp1294;Cyc_Tcutil_failure_reason=((_tmp1294="(different attributes on member)",
_tag_dyneither(_tmp1294,sizeof(char),33)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
if((_tmp5BD->width != 0  && _tmp5BE->width == 0  || _tmp5BE->width != 0  && _tmp5BD->width
== 0) || (_tmp5BD->width != 0  && _tmp5BE->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp5BD->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp5BE->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmp1295;Cyc_Tcutil_failure_reason=((_tmp1295="(different bitfield widths on member)",
_tag_dyneither(_tmp1295,sizeof(char),38)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
if(_tmp537 == 0  && _tmp533 == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmp1296;Cyc_Tcutil_failure_reason=((_tmp1296="(different number of members)",
_tag_dyneither(_tmp1296,sizeof(char),30)));}goto _LL294;_LL2B7: _tmp538=_tmp495.f1;{
struct Cyc_Absyn_HeapRgn_struct*_tmp539=(struct Cyc_Absyn_HeapRgn_struct*)_tmp538;
if(_tmp539->tag != 20)goto _LL2B9;};_tmp53A=_tmp495.f2;{struct Cyc_Absyn_HeapRgn_struct*
_tmp53B=(struct Cyc_Absyn_HeapRgn_struct*)_tmp53A;if(_tmp53B->tag != 20)goto _LL2B9;};
_LL2B8: return;_LL2B9: _tmp53C=_tmp495.f1;{struct Cyc_Absyn_UniqueRgn_struct*
_tmp53D=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp53C;if(_tmp53D->tag != 21)goto
_LL2BB;};_tmp53E=_tmp495.f2;{struct Cyc_Absyn_UniqueRgn_struct*_tmp53F=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp53E;if(_tmp53F->tag != 21)goto _LL2BB;};_LL2BA: return;_LL2BB: _tmp540=_tmp495.f1;{
struct Cyc_Absyn_RefCntRgn_struct*_tmp541=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp540;if(_tmp541->tag != 22)goto _LL2BD;};_tmp542=_tmp495.f2;{struct Cyc_Absyn_RefCntRgn_struct*
_tmp543=(struct Cyc_Absyn_RefCntRgn_struct*)_tmp542;if(_tmp543->tag != 22)goto
_LL2BD;};_LL2BC: return;_LL2BD: _tmp544=_tmp495.f1;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp545=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp544;if(_tmp545->tag != 15)goto
_LL2BF;else{_tmp546=(void*)_tmp545->f1;}};_tmp547=_tmp495.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp548=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp547;if(_tmp548->tag != 15)goto
_LL2BF;else{_tmp549=(void*)_tmp548->f1;}};_LL2BE: Cyc_Tcutil_unify_it(_tmp546,
_tmp549);return;_LL2BF: _tmp54A=_tmp495.f1;{struct Cyc_Absyn_DynRgnType_struct*
_tmp54B=(struct Cyc_Absyn_DynRgnType_struct*)_tmp54A;if(_tmp54B->tag != 16)goto
_LL2C1;else{_tmp54C=(void*)_tmp54B->f1;_tmp54D=(void*)_tmp54B->f2;}};_tmp54E=
_tmp495.f2;{struct Cyc_Absyn_DynRgnType_struct*_tmp54F=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp54E;if(_tmp54F->tag != 16)goto _LL2C1;else{_tmp550=(void*)_tmp54F->f1;_tmp551=(
void*)_tmp54F->f2;}};_LL2C0: Cyc_Tcutil_unify_it(_tmp54C,_tmp550);Cyc_Tcutil_unify_it(
_tmp54D,_tmp551);return;_LL2C1: _tmp552=_tmp495.f1;{struct Cyc_Absyn_JoinEff_struct*
_tmp553=(struct Cyc_Absyn_JoinEff_struct*)_tmp552;if(_tmp553->tag != 24)goto _LL2C3;};
_LL2C2: goto _LL2C4;_LL2C3: _tmp554=_tmp495.f2;{struct Cyc_Absyn_JoinEff_struct*
_tmp555=(struct Cyc_Absyn_JoinEff_struct*)_tmp554;if(_tmp555->tag != 24)goto _LL2C5;};
_LL2C4: goto _LL2C6;_LL2C5: _tmp556=_tmp495.f1;{struct Cyc_Absyn_AccessEff_struct*
_tmp557=(struct Cyc_Absyn_AccessEff_struct*)_tmp556;if(_tmp557->tag != 23)goto
_LL2C7;};_LL2C6: goto _LL2C8;_LL2C7: _tmp558=_tmp495.f1;{struct Cyc_Absyn_RgnsEff_struct*
_tmp559=(struct Cyc_Absyn_RgnsEff_struct*)_tmp558;if(_tmp559->tag != 25)goto _LL2C9;};
_LL2C8: goto _LL2CA;_LL2C9: _tmp55A=_tmp495.f2;{struct Cyc_Absyn_RgnsEff_struct*
_tmp55B=(struct Cyc_Absyn_RgnsEff_struct*)_tmp55A;if(_tmp55B->tag != 25)goto _LL2CB;};
_LL2CA: goto _LL2CC;_LL2CB: _tmp55C=_tmp495.f2;{struct Cyc_Absyn_AccessEff_struct*
_tmp55D=(struct Cyc_Absyn_AccessEff_struct*)_tmp55C;if(_tmp55D->tag != 23)goto
_LL2CD;};_LL2CC: if(Cyc_Tcutil_unify_effect(t1,t2))return;{const char*_tmp1297;Cyc_Tcutil_failure_reason=((
_tmp1297="(effects don't unify)",_tag_dyneither(_tmp1297,sizeof(char),22)));}
goto _LL294;_LL2CD:;_LL2CE: goto _LL294;_LL294:;}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){if(a1 == a2)return
0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2 == 0)return 1;return cmp((void*)
_check_null(a1),(void*)_check_null(a2));}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual
tq1,struct Cyc_Absyn_Tqual tq2){int _tmp5C4=(tq1.real_const + (tq1.q_volatile << 1))+ (
tq1.q_restrict << 2);int _tmp5C5=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict
<< 2);return Cyc_Core_intcmp(_tmp5C4,_tmp5C5);}static int Cyc_Tcutil_conrefs_cmp(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp5C6=x;union Cyc_Absyn_Constraint _tmp5C7;int _tmp5C8;
union Cyc_Absyn_Constraint _tmp5C9;void*_tmp5CA;union Cyc_Absyn_Constraint _tmp5CB;
union Cyc_Absyn_Constraint*_tmp5CC;_LL2E7: _tmp5C7=*_tmp5C6;if((_tmp5C7.No_constr).tag
!= 3)goto _LL2E9;_tmp5C8=(int)(_tmp5C7.No_constr).val;_LL2E8: return - 1;_LL2E9:
_tmp5C9=*_tmp5C6;if((_tmp5C9.Eq_constr).tag != 1)goto _LL2EB;_tmp5CA=(void*)(
_tmp5C9.Eq_constr).val;_LL2EA: {union Cyc_Absyn_Constraint*_tmp5CD=y;union Cyc_Absyn_Constraint
_tmp5CE;int _tmp5CF;union Cyc_Absyn_Constraint _tmp5D0;void*_tmp5D1;union Cyc_Absyn_Constraint
_tmp5D2;union Cyc_Absyn_Constraint*_tmp5D3;_LL2EE: _tmp5CE=*_tmp5CD;if((_tmp5CE.No_constr).tag
!= 3)goto _LL2F0;_tmp5CF=(int)(_tmp5CE.No_constr).val;_LL2EF: return 1;_LL2F0:
_tmp5D0=*_tmp5CD;if((_tmp5D0.Eq_constr).tag != 1)goto _LL2F2;_tmp5D1=(void*)(
_tmp5D0.Eq_constr).val;_LL2F1: return cmp(_tmp5CA,_tmp5D1);_LL2F2: _tmp5D2=*_tmp5CD;
if((_tmp5D2.Forward_constr).tag != 2)goto _LL2ED;_tmp5D3=(union Cyc_Absyn_Constraint*)(
_tmp5D2.Forward_constr).val;_LL2F3: {const char*_tmp129A;void*_tmp1299;(_tmp1299=
0,Cyc_Tcutil_impos(((_tmp129A="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp129A,sizeof(char),40))),_tag_dyneither(_tmp1299,sizeof(void*),
0)));}_LL2ED:;}_LL2EB: _tmp5CB=*_tmp5C6;if((_tmp5CB.Forward_constr).tag != 2)goto
_LL2E6;_tmp5CC=(union Cyc_Absyn_Constraint*)(_tmp5CB.Forward_constr).val;_LL2EC: {
const char*_tmp129D;void*_tmp129C;(_tmp129C=0,Cyc_Tcutil_impos(((_tmp129D="unify_conref: forward after compress",
_tag_dyneither(_tmp129D,sizeof(char),37))),_tag_dyneither(_tmp129C,sizeof(void*),
0)));}_LL2E6:;};}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct
_tuple12*tqt2){struct _tuple12 _tmp5D9;struct Cyc_Absyn_Tqual _tmp5DA;void*_tmp5DB;
struct _tuple12*_tmp5D8=tqt1;_tmp5D9=*_tmp5D8;_tmp5DA=_tmp5D9.f1;_tmp5DB=_tmp5D9.f2;{
struct _tuple12 _tmp5DD;struct Cyc_Absyn_Tqual _tmp5DE;void*_tmp5DF;struct _tuple12*
_tmp5DC=tqt2;_tmp5DD=*_tmp5DC;_tmp5DE=_tmp5DD.f1;_tmp5DF=_tmp5DD.f2;{int _tmp5E0=
Cyc_Tcutil_tqual_cmp(_tmp5DA,_tmp5DE);if(_tmp5E0 != 0)return _tmp5E0;return Cyc_Tcutil_typecmp(
_tmp5DB,_tmp5DF);};};}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp5E1=Cyc_strptrcmp(f1->name,f2->name);if(
_tmp5E1 != 0)return _tmp5E1;{int _tmp5E2=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp5E2 != 0)return _tmp5E2;{int _tmp5E3=Cyc_Tcutil_typecmp(f1->type,f2->type);if(
_tmp5E3 != 0)return _tmp5E3;{int _tmp5E4=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,
f2->attributes);if(_tmp5E4 != 0)return _tmp5E4;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,f1->width,f2->width);};};};}static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){int _tmp5E5=Cyc_Absyn_qvar_cmp(
e1->name,e2->name);if(_tmp5E5 != 0)return _tmp5E5;return((int(*)(int(*cmp)(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp5E6=t;int _tmp5EF;_LL2F5: {struct Cyc_Absyn_VoidType_struct*
_tmp5E7=(struct Cyc_Absyn_VoidType_struct*)_tmp5E6;if(_tmp5E7->tag != 0)goto _LL2F7;}
_LL2F6: return 0;_LL2F7: {struct Cyc_Absyn_Evar_struct*_tmp5E8=(struct Cyc_Absyn_Evar_struct*)
_tmp5E6;if(_tmp5E8->tag != 1)goto _LL2F9;}_LL2F8: return 1;_LL2F9: {struct Cyc_Absyn_VarType_struct*
_tmp5E9=(struct Cyc_Absyn_VarType_struct*)_tmp5E6;if(_tmp5E9->tag != 2)goto _LL2FB;}
_LL2FA: return 2;_LL2FB: {struct Cyc_Absyn_DatatypeType_struct*_tmp5EA=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp5E6;if(_tmp5EA->tag != 3)goto _LL2FD;}_LL2FC: return 3;_LL2FD: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp5EB=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp5E6;if(_tmp5EB->tag != 4)
goto _LL2FF;}_LL2FE: return 4;_LL2FF: {struct Cyc_Absyn_PointerType_struct*_tmp5EC=(
struct Cyc_Absyn_PointerType_struct*)_tmp5E6;if(_tmp5EC->tag != 5)goto _LL301;}
_LL300: return 5;_LL301: {struct Cyc_Absyn_IntType_struct*_tmp5ED=(struct Cyc_Absyn_IntType_struct*)
_tmp5E6;if(_tmp5ED->tag != 6)goto _LL303;}_LL302: return 6;_LL303: {struct Cyc_Absyn_FloatType_struct*
_tmp5EE=(struct Cyc_Absyn_FloatType_struct*)_tmp5E6;if(_tmp5EE->tag != 7)goto
_LL305;else{_tmp5EF=_tmp5EE->f1;if(_tmp5EF != 0)goto _LL305;}}_LL304: return 7;
_LL305: {struct Cyc_Absyn_FloatType_struct*_tmp5F0=(struct Cyc_Absyn_FloatType_struct*)
_tmp5E6;if(_tmp5F0->tag != 7)goto _LL307;}_LL306: return 8;_LL307: {struct Cyc_Absyn_ArrayType_struct*
_tmp5F1=(struct Cyc_Absyn_ArrayType_struct*)_tmp5E6;if(_tmp5F1->tag != 8)goto
_LL309;}_LL308: return 9;_LL309: {struct Cyc_Absyn_FnType_struct*_tmp5F2=(struct Cyc_Absyn_FnType_struct*)
_tmp5E6;if(_tmp5F2->tag != 9)goto _LL30B;}_LL30A: return 10;_LL30B: {struct Cyc_Absyn_TupleType_struct*
_tmp5F3=(struct Cyc_Absyn_TupleType_struct*)_tmp5E6;if(_tmp5F3->tag != 10)goto
_LL30D;}_LL30C: return 11;_LL30D: {struct Cyc_Absyn_AggrType_struct*_tmp5F4=(struct
Cyc_Absyn_AggrType_struct*)_tmp5E6;if(_tmp5F4->tag != 11)goto _LL30F;}_LL30E:
return 12;_LL30F: {struct Cyc_Absyn_AnonAggrType_struct*_tmp5F5=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp5E6;if(_tmp5F5->tag != 12)goto _LL311;}_LL310: return 14;_LL311: {struct Cyc_Absyn_EnumType_struct*
_tmp5F6=(struct Cyc_Absyn_EnumType_struct*)_tmp5E6;if(_tmp5F6->tag != 13)goto
_LL313;}_LL312: return 16;_LL313: {struct Cyc_Absyn_AnonEnumType_struct*_tmp5F7=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp5E6;if(_tmp5F7->tag != 14)goto _LL315;}
_LL314: return 17;_LL315: {struct Cyc_Absyn_RgnHandleType_struct*_tmp5F8=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp5E6;if(_tmp5F8->tag != 15)goto _LL317;}_LL316: return 18;_LL317: {struct Cyc_Absyn_TypedefType_struct*
_tmp5F9=(struct Cyc_Absyn_TypedefType_struct*)_tmp5E6;if(_tmp5F9->tag != 17)goto
_LL319;}_LL318: return 19;_LL319: {struct Cyc_Absyn_UniqueRgn_struct*_tmp5FA=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp5E6;if(_tmp5FA->tag != 21)goto _LL31B;}
_LL31A: return 20;_LL31B: {struct Cyc_Absyn_HeapRgn_struct*_tmp5FB=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp5E6;if(_tmp5FB->tag != 20)goto _LL31D;}_LL31C: return 21;_LL31D: {struct Cyc_Absyn_AccessEff_struct*
_tmp5FC=(struct Cyc_Absyn_AccessEff_struct*)_tmp5E6;if(_tmp5FC->tag != 23)goto
_LL31F;}_LL31E: return 22;_LL31F: {struct Cyc_Absyn_JoinEff_struct*_tmp5FD=(struct
Cyc_Absyn_JoinEff_struct*)_tmp5E6;if(_tmp5FD->tag != 24)goto _LL321;}_LL320: return
23;_LL321: {struct Cyc_Absyn_RgnsEff_struct*_tmp5FE=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp5E6;if(_tmp5FE->tag != 25)goto _LL323;}_LL322: return 24;_LL323: {struct Cyc_Absyn_RefCntRgn_struct*
_tmp5FF=(struct Cyc_Absyn_RefCntRgn_struct*)_tmp5E6;if(_tmp5FF->tag != 22)goto
_LL325;}_LL324: return 25;_LL325: {struct Cyc_Absyn_TagType_struct*_tmp600=(struct
Cyc_Absyn_TagType_struct*)_tmp5E6;if(_tmp600->tag != 19)goto _LL327;}_LL326: return
26;_LL327: {struct Cyc_Absyn_DynRgnType_struct*_tmp601=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp5E6;if(_tmp601->tag != 16)goto _LL329;}_LL328: return 27;_LL329: {struct Cyc_Absyn_ValueofType_struct*
_tmp602=(struct Cyc_Absyn_ValueofType_struct*)_tmp5E6;if(_tmp602->tag != 18)goto
_LL32B;}_LL32A: return 28;_LL32B: {struct Cyc_Absyn_TypeDeclType_struct*_tmp603=(
struct Cyc_Absyn_TypeDeclType_struct*)_tmp5E6;if(_tmp603->tag != 26)goto _LL2F4;}
_LL32C: return 29;_LL2F4:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp604=Cyc_Core_intcmp(
Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));if(_tmp604 != 0)
return _tmp604;{struct _tuple0 _tmp129E;struct _tuple0 _tmp606=(_tmp129E.f1=t2,((
_tmp129E.f2=t1,_tmp129E)));void*_tmp607;void*_tmp609;void*_tmp60B;struct Cyc_Absyn_Tvar*
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
enum Cyc_Absyn_Size_of _tmp660;void*_tmp661;int _tmp663;void*_tmp664;int _tmp666;
void*_tmp667;struct Cyc_Absyn_ArrayInfo _tmp669;void*_tmp66A;struct Cyc_Absyn_Tqual
_tmp66B;struct Cyc_Absyn_Exp*_tmp66C;union Cyc_Absyn_Constraint*_tmp66D;void*
_tmp66E;struct Cyc_Absyn_ArrayInfo _tmp670;void*_tmp671;struct Cyc_Absyn_Tqual
_tmp672;struct Cyc_Absyn_Exp*_tmp673;union Cyc_Absyn_Constraint*_tmp674;void*
_tmp675;struct Cyc_Absyn_FnInfo _tmp677;struct Cyc_List_List*_tmp678;struct Cyc_Core_Opt*
_tmp679;struct Cyc_Absyn_Tqual _tmp67A;void*_tmp67B;struct Cyc_List_List*_tmp67C;
int _tmp67D;struct Cyc_Absyn_VarargInfo*_tmp67E;struct Cyc_List_List*_tmp67F;struct
Cyc_List_List*_tmp680;void*_tmp681;struct Cyc_Absyn_FnInfo _tmp683;struct Cyc_List_List*
_tmp684;struct Cyc_Core_Opt*_tmp685;struct Cyc_Absyn_Tqual _tmp686;void*_tmp687;
struct Cyc_List_List*_tmp688;int _tmp689;struct Cyc_Absyn_VarargInfo*_tmp68A;struct
Cyc_List_List*_tmp68B;struct Cyc_List_List*_tmp68C;void*_tmp68D;struct Cyc_List_List*
_tmp68F;void*_tmp690;struct Cyc_List_List*_tmp692;void*_tmp693;enum Cyc_Absyn_AggrKind
_tmp695;struct Cyc_List_List*_tmp696;void*_tmp697;enum Cyc_Absyn_AggrKind _tmp699;
struct Cyc_List_List*_tmp69A;void*_tmp69B;void*_tmp69D;void*_tmp69E;void*_tmp6A0;
void*_tmp6A1;void*_tmp6A3;void*_tmp6A4;void*_tmp6A5;void*_tmp6A7;void*_tmp6A8;
void*_tmp6A9;void*_tmp6AB;void*_tmp6AC;void*_tmp6AE;void*_tmp6AF;struct Cyc_Absyn_Exp*
_tmp6B1;void*_tmp6B2;struct Cyc_Absyn_Exp*_tmp6B4;void*_tmp6B5;void*_tmp6B7;void*
_tmp6B9;void*_tmp6BB;void*_tmp6BD;void*_tmp6BF;_LL32E: _tmp607=_tmp606.f1;{struct
Cyc_Absyn_Evar_struct*_tmp608=(struct Cyc_Absyn_Evar_struct*)_tmp607;if(_tmp608->tag
!= 1)goto _LL330;};_tmp609=_tmp606.f2;{struct Cyc_Absyn_Evar_struct*_tmp60A=(
struct Cyc_Absyn_Evar_struct*)_tmp609;if(_tmp60A->tag != 1)goto _LL330;};_LL32F: {
const char*_tmp12A1;void*_tmp12A0;(_tmp12A0=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12A1="typecmp: can only compare closed types",
_tag_dyneither(_tmp12A1,sizeof(char),39))),_tag_dyneither(_tmp12A0,sizeof(void*),
0)));}_LL330: _tmp60B=_tmp606.f1;{struct Cyc_Absyn_VarType_struct*_tmp60C=(struct
Cyc_Absyn_VarType_struct*)_tmp60B;if(_tmp60C->tag != 2)goto _LL332;else{_tmp60D=
_tmp60C->f1;}};_tmp60E=_tmp606.f2;{struct Cyc_Absyn_VarType_struct*_tmp60F=(
struct Cyc_Absyn_VarType_struct*)_tmp60E;if(_tmp60F->tag != 2)goto _LL332;else{
_tmp610=_tmp60F->f1;}};_LL331: return Cyc_Core_intcmp(_tmp610->identity,_tmp60D->identity);
_LL332: _tmp611=_tmp606.f1;{struct Cyc_Absyn_AggrType_struct*_tmp612=(struct Cyc_Absyn_AggrType_struct*)
_tmp611;if(_tmp612->tag != 11)goto _LL334;else{_tmp613=_tmp612->f1;_tmp614=_tmp613.aggr_info;
_tmp615=_tmp613.targs;}};_tmp616=_tmp606.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp617=(struct Cyc_Absyn_AggrType_struct*)_tmp616;if(_tmp617->tag != 11)goto
_LL334;else{_tmp618=_tmp617->f1;_tmp619=_tmp618.aggr_info;_tmp61A=_tmp618.targs;}};
_LL333: {struct _tuple2*_tmp6C4;struct _tuple13 _tmp6C3=Cyc_Absyn_aggr_kinded_name(
_tmp614);_tmp6C4=_tmp6C3.f2;{struct _tuple2*_tmp6C6;struct _tuple13 _tmp6C5=Cyc_Absyn_aggr_kinded_name(
_tmp619);_tmp6C6=_tmp6C5.f2;{int _tmp6C7=Cyc_Absyn_qvar_cmp(_tmp6C4,_tmp6C6);if(
_tmp6C7 != 0)return _tmp6C7;else{return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp615,_tmp61A);}};};}
_LL334: _tmp61B=_tmp606.f1;{struct Cyc_Absyn_EnumType_struct*_tmp61C=(struct Cyc_Absyn_EnumType_struct*)
_tmp61B;if(_tmp61C->tag != 13)goto _LL336;else{_tmp61D=_tmp61C->f1;}};_tmp61E=
_tmp606.f2;{struct Cyc_Absyn_EnumType_struct*_tmp61F=(struct Cyc_Absyn_EnumType_struct*)
_tmp61E;if(_tmp61F->tag != 13)goto _LL336;else{_tmp620=_tmp61F->f1;}};_LL335:
return Cyc_Absyn_qvar_cmp(_tmp61D,_tmp620);_LL336: _tmp621=_tmp606.f1;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp622=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp621;if(_tmp622->tag != 14)goto
_LL338;else{_tmp623=_tmp622->f1;}};_tmp624=_tmp606.f2;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp625=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp624;if(_tmp625->tag != 14)goto
_LL338;else{_tmp626=_tmp625->f1;}};_LL337: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp623,_tmp626);_LL338: _tmp627=_tmp606.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp628=(struct Cyc_Absyn_DatatypeType_struct*)_tmp627;if(_tmp628->tag != 3)goto
_LL33A;else{_tmp629=_tmp628->f1;_tmp62A=_tmp629.datatype_info;if((_tmp62A.KnownDatatype).tag
!= 2)goto _LL33A;_tmp62B=(struct Cyc_Absyn_Datatypedecl**)(_tmp62A.KnownDatatype).val;
_tmp62C=*_tmp62B;_tmp62D=_tmp629.targs;}};_tmp62E=_tmp606.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp62F=(struct Cyc_Absyn_DatatypeType_struct*)_tmp62E;if(_tmp62F->tag != 3)goto
_LL33A;else{_tmp630=_tmp62F->f1;_tmp631=_tmp630.datatype_info;if((_tmp631.KnownDatatype).tag
!= 2)goto _LL33A;_tmp632=(struct Cyc_Absyn_Datatypedecl**)(_tmp631.KnownDatatype).val;
_tmp633=*_tmp632;_tmp634=_tmp630.targs;}};_LL339: if(_tmp633 == _tmp62C)return 0;{
int _tmp6C8=Cyc_Absyn_qvar_cmp(_tmp633->name,_tmp62C->name);if(_tmp6C8 != 0)return
_tmp6C8;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp634,_tmp62D);};_LL33A: _tmp635=
_tmp606.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*_tmp636=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp635;if(_tmp636->tag != 4)goto _LL33C;else{_tmp637=_tmp636->f1;_tmp638=_tmp637.field_info;
if((_tmp638.KnownDatatypefield).tag != 2)goto _LL33C;_tmp639=(struct _tuple3)(
_tmp638.KnownDatatypefield).val;_tmp63A=_tmp639.f1;_tmp63B=_tmp639.f2;_tmp63C=
_tmp637.targs;}};_tmp63D=_tmp606.f2;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp63E=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp63D;if(_tmp63E->tag != 4)
goto _LL33C;else{_tmp63F=_tmp63E->f1;_tmp640=_tmp63F.field_info;if((_tmp640.KnownDatatypefield).tag
!= 2)goto _LL33C;_tmp641=(struct _tuple3)(_tmp640.KnownDatatypefield).val;_tmp642=
_tmp641.f1;_tmp643=_tmp641.f2;_tmp644=_tmp63F.targs;}};_LL33B: if(_tmp642 == 
_tmp63A)return 0;{int _tmp6C9=Cyc_Absyn_qvar_cmp(_tmp63A->name,_tmp642->name);if(
_tmp6C9 != 0)return _tmp6C9;{int _tmp6CA=Cyc_Absyn_qvar_cmp(_tmp63B->name,_tmp643->name);
if(_tmp6CA != 0)return _tmp6CA;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp644,_tmp63C);};};
_LL33C: _tmp645=_tmp606.f1;{struct Cyc_Absyn_PointerType_struct*_tmp646=(struct Cyc_Absyn_PointerType_struct*)
_tmp645;if(_tmp646->tag != 5)goto _LL33E;else{_tmp647=_tmp646->f1;_tmp648=_tmp647.elt_typ;
_tmp649=_tmp647.elt_tq;_tmp64A=_tmp647.ptr_atts;_tmp64B=_tmp64A.rgn;_tmp64C=
_tmp64A.nullable;_tmp64D=_tmp64A.bounds;_tmp64E=_tmp64A.zero_term;}};_tmp64F=
_tmp606.f2;{struct Cyc_Absyn_PointerType_struct*_tmp650=(struct Cyc_Absyn_PointerType_struct*)
_tmp64F;if(_tmp650->tag != 5)goto _LL33E;else{_tmp651=_tmp650->f1;_tmp652=_tmp651.elt_typ;
_tmp653=_tmp651.elt_tq;_tmp654=_tmp651.ptr_atts;_tmp655=_tmp654.rgn;_tmp656=
_tmp654.nullable;_tmp657=_tmp654.bounds;_tmp658=_tmp654.zero_term;}};_LL33D: {
int _tmp6CB=Cyc_Tcutil_typecmp(_tmp652,_tmp648);if(_tmp6CB != 0)return _tmp6CB;{int
_tmp6CC=Cyc_Tcutil_typecmp(_tmp655,_tmp64B);if(_tmp6CC != 0)return _tmp6CC;{int
_tmp6CD=Cyc_Tcutil_tqual_cmp(_tmp653,_tmp649);if(_tmp6CD != 0)return _tmp6CD;{int
_tmp6CE=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp657,_tmp64D);if(_tmp6CE != 0)
return _tmp6CE;{int _tmp6CF=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp658,
_tmp64E);if(_tmp6CF != 0)return _tmp6CF;{void*_tmp6D0=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp657);_LL361: {struct Cyc_Absyn_DynEither_b_struct*
_tmp6D1=(struct Cyc_Absyn_DynEither_b_struct*)_tmp6D0;if(_tmp6D1->tag != 0)goto
_LL363;}_LL362: return 0;_LL363:;_LL364: goto _LL360;_LL360:;}return((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp656,_tmp64C);};};};};}_LL33E: _tmp659=_tmp606.f1;{struct Cyc_Absyn_IntType_struct*
_tmp65A=(struct Cyc_Absyn_IntType_struct*)_tmp659;if(_tmp65A->tag != 6)goto _LL340;
else{_tmp65B=_tmp65A->f1;_tmp65C=_tmp65A->f2;}};_tmp65D=_tmp606.f2;{struct Cyc_Absyn_IntType_struct*
_tmp65E=(struct Cyc_Absyn_IntType_struct*)_tmp65D;if(_tmp65E->tag != 6)goto _LL340;
else{_tmp65F=_tmp65E->f1;_tmp660=_tmp65E->f2;}};_LL33F: if(_tmp65F != _tmp65B)
return Cyc_Core_intcmp((int)((unsigned int)_tmp65F),(int)((unsigned int)_tmp65B));
if(_tmp660 != _tmp65C)return Cyc_Core_intcmp((int)((unsigned int)_tmp660),(int)((
unsigned int)_tmp65C));return 0;_LL340: _tmp661=_tmp606.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp662=(struct Cyc_Absyn_FloatType_struct*)_tmp661;if(_tmp662->tag != 7)goto
_LL342;else{_tmp663=_tmp662->f1;}};_tmp664=_tmp606.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp665=(struct Cyc_Absyn_FloatType_struct*)_tmp664;if(_tmp665->tag != 7)goto
_LL342;else{_tmp666=_tmp665->f1;}};_LL341: if(_tmp663 == _tmp666)return 0;else{if(
_tmp666 == 0)return - 1;else{if(_tmp666 == 1  && _tmp663 == 0)return - 1;else{return 1;}}}
_LL342: _tmp667=_tmp606.f1;{struct Cyc_Absyn_ArrayType_struct*_tmp668=(struct Cyc_Absyn_ArrayType_struct*)
_tmp667;if(_tmp668->tag != 8)goto _LL344;else{_tmp669=_tmp668->f1;_tmp66A=_tmp669.elt_type;
_tmp66B=_tmp669.tq;_tmp66C=_tmp669.num_elts;_tmp66D=_tmp669.zero_term;}};_tmp66E=
_tmp606.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp66F=(struct Cyc_Absyn_ArrayType_struct*)
_tmp66E;if(_tmp66F->tag != 8)goto _LL344;else{_tmp670=_tmp66F->f1;_tmp671=_tmp670.elt_type;
_tmp672=_tmp670.tq;_tmp673=_tmp670.num_elts;_tmp674=_tmp670.zero_term;}};_LL343: {
int _tmp6D2=Cyc_Tcutil_tqual_cmp(_tmp672,_tmp66B);if(_tmp6D2 != 0)return _tmp6D2;{
int _tmp6D3=Cyc_Tcutil_typecmp(_tmp671,_tmp66A);if(_tmp6D3 != 0)return _tmp6D3;{int
_tmp6D4=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp66D,_tmp674);if(_tmp6D4 != 0)
return _tmp6D4;if(_tmp66C == _tmp673)return 0;if(_tmp66C == 0  || _tmp673 == 0){const
char*_tmp12A4;void*_tmp12A3;(_tmp12A3=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12A4="missing expression in array index",
_tag_dyneither(_tmp12A4,sizeof(char),34))),_tag_dyneither(_tmp12A3,sizeof(void*),
0)));}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct
Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp66C,_tmp673);};};}_LL344: _tmp675=_tmp606.f1;{struct Cyc_Absyn_FnType_struct*
_tmp676=(struct Cyc_Absyn_FnType_struct*)_tmp675;if(_tmp676->tag != 9)goto _LL346;
else{_tmp677=_tmp676->f1;_tmp678=_tmp677.tvars;_tmp679=_tmp677.effect;_tmp67A=
_tmp677.ret_tqual;_tmp67B=_tmp677.ret_typ;_tmp67C=_tmp677.args;_tmp67D=_tmp677.c_varargs;
_tmp67E=_tmp677.cyc_varargs;_tmp67F=_tmp677.rgn_po;_tmp680=_tmp677.attributes;}};
_tmp681=_tmp606.f2;{struct Cyc_Absyn_FnType_struct*_tmp682=(struct Cyc_Absyn_FnType_struct*)
_tmp681;if(_tmp682->tag != 9)goto _LL346;else{_tmp683=_tmp682->f1;_tmp684=_tmp683.tvars;
_tmp685=_tmp683.effect;_tmp686=_tmp683.ret_tqual;_tmp687=_tmp683.ret_typ;_tmp688=
_tmp683.args;_tmp689=_tmp683.c_varargs;_tmp68A=_tmp683.cyc_varargs;_tmp68B=
_tmp683.rgn_po;_tmp68C=_tmp683.attributes;}};_LL345: {const char*_tmp12A7;void*
_tmp12A6;(_tmp12A6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12A7="typecmp: function types not handled",
_tag_dyneither(_tmp12A7,sizeof(char),36))),_tag_dyneither(_tmp12A6,sizeof(void*),
0)));}_LL346: _tmp68D=_tmp606.f1;{struct Cyc_Absyn_TupleType_struct*_tmp68E=(
struct Cyc_Absyn_TupleType_struct*)_tmp68D;if(_tmp68E->tag != 10)goto _LL348;else{
_tmp68F=_tmp68E->f1;}};_tmp690=_tmp606.f2;{struct Cyc_Absyn_TupleType_struct*
_tmp691=(struct Cyc_Absyn_TupleType_struct*)_tmp690;if(_tmp691->tag != 10)goto
_LL348;else{_tmp692=_tmp691->f1;}};_LL347: return((int(*)(int(*cmp)(struct
_tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_tqual_type_cmp,_tmp692,_tmp68F);_LL348: _tmp693=_tmp606.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp694=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp693;if(_tmp694->tag != 12)goto
_LL34A;else{_tmp695=_tmp694->f1;_tmp696=_tmp694->f2;}};_tmp697=_tmp606.f2;{
struct Cyc_Absyn_AnonAggrType_struct*_tmp698=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp697;if(_tmp698->tag != 12)goto _LL34A;else{_tmp699=_tmp698->f1;_tmp69A=_tmp698->f2;}};
_LL349: if(_tmp699 != _tmp695){if(_tmp699 == Cyc_Absyn_StructA)return - 1;else{return
1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp69A,_tmp696);_LL34A: _tmp69B=_tmp606.f1;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp69C=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp69B;if(_tmp69C->tag != 15)goto
_LL34C;else{_tmp69D=(void*)_tmp69C->f1;}};_tmp69E=_tmp606.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp69F=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp69E;if(_tmp69F->tag != 15)goto
_LL34C;else{_tmp6A0=(void*)_tmp69F->f1;}};_LL34B: return Cyc_Tcutil_typecmp(
_tmp69D,_tmp6A0);_LL34C: _tmp6A1=_tmp606.f1;{struct Cyc_Absyn_DynRgnType_struct*
_tmp6A2=(struct Cyc_Absyn_DynRgnType_struct*)_tmp6A1;if(_tmp6A2->tag != 16)goto
_LL34E;else{_tmp6A3=(void*)_tmp6A2->f1;_tmp6A4=(void*)_tmp6A2->f2;}};_tmp6A5=
_tmp606.f2;{struct Cyc_Absyn_DynRgnType_struct*_tmp6A6=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp6A5;if(_tmp6A6->tag != 16)goto _LL34E;else{_tmp6A7=(void*)_tmp6A6->f1;_tmp6A8=(
void*)_tmp6A6->f2;}};_LL34D: {int _tmp6D9=Cyc_Tcutil_typecmp(_tmp6A3,_tmp6A7);if(
_tmp6D9 != 0)return _tmp6D9;else{return Cyc_Tcutil_typecmp(_tmp6A4,_tmp6A8);}}
_LL34E: _tmp6A9=_tmp606.f1;{struct Cyc_Absyn_TagType_struct*_tmp6AA=(struct Cyc_Absyn_TagType_struct*)
_tmp6A9;if(_tmp6AA->tag != 19)goto _LL350;else{_tmp6AB=(void*)_tmp6AA->f1;}};
_tmp6AC=_tmp606.f2;{struct Cyc_Absyn_TagType_struct*_tmp6AD=(struct Cyc_Absyn_TagType_struct*)
_tmp6AC;if(_tmp6AD->tag != 19)goto _LL350;else{_tmp6AE=(void*)_tmp6AD->f1;}};
_LL34F: return Cyc_Tcutil_typecmp(_tmp6AB,_tmp6AE);_LL350: _tmp6AF=_tmp606.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp6B0=(struct Cyc_Absyn_ValueofType_struct*)
_tmp6AF;if(_tmp6B0->tag != 18)goto _LL352;else{_tmp6B1=_tmp6B0->f1;}};_tmp6B2=
_tmp606.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp6B3=(struct Cyc_Absyn_ValueofType_struct*)
_tmp6B2;if(_tmp6B3->tag != 18)goto _LL352;else{_tmp6B4=_tmp6B3->f1;}};_LL351:
return Cyc_Evexp_const_exp_cmp(_tmp6B1,_tmp6B4);_LL352: _tmp6B5=_tmp606.f1;{struct
Cyc_Absyn_JoinEff_struct*_tmp6B6=(struct Cyc_Absyn_JoinEff_struct*)_tmp6B5;if(
_tmp6B6->tag != 24)goto _LL354;};_LL353: goto _LL355;_LL354: _tmp6B7=_tmp606.f2;{
struct Cyc_Absyn_JoinEff_struct*_tmp6B8=(struct Cyc_Absyn_JoinEff_struct*)_tmp6B7;
if(_tmp6B8->tag != 24)goto _LL356;};_LL355: goto _LL357;_LL356: _tmp6B9=_tmp606.f1;{
struct Cyc_Absyn_AccessEff_struct*_tmp6BA=(struct Cyc_Absyn_AccessEff_struct*)
_tmp6B9;if(_tmp6BA->tag != 23)goto _LL358;};_LL357: goto _LL359;_LL358: _tmp6BB=
_tmp606.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp6BC=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp6BB;if(_tmp6BC->tag != 25)goto _LL35A;};_LL359: goto _LL35B;_LL35A: _tmp6BD=
_tmp606.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp6BE=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp6BD;if(_tmp6BE->tag != 25)goto _LL35C;};_LL35B: goto _LL35D;_LL35C: _tmp6BF=
_tmp606.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp6C0=(struct Cyc_Absyn_AccessEff_struct*)
_tmp6BF;if(_tmp6C0->tag != 23)goto _LL35E;};_LL35D: {const char*_tmp12AA;void*
_tmp12A9;(_tmp12A9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12AA="typecmp: effects not handled",_tag_dyneither(
_tmp12AA,sizeof(char),29))),_tag_dyneither(_tmp12A9,sizeof(void*),0)));}_LL35E:;
_LL35F: {const char*_tmp12AD;void*_tmp12AC;(_tmp12AC=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12AD="Unmatched case in typecmp",
_tag_dyneither(_tmp12AD,sizeof(char),26))),_tag_dyneither(_tmp12AC,sizeof(void*),
0)));}_LL32D:;};};}int Cyc_Tcutil_is_arithmetic_type(void*t){void*_tmp6DE=Cyc_Tcutil_compress(
t);_LL366: {struct Cyc_Absyn_IntType_struct*_tmp6DF=(struct Cyc_Absyn_IntType_struct*)
_tmp6DE;if(_tmp6DF->tag != 6)goto _LL368;}_LL367: goto _LL369;_LL368: {struct Cyc_Absyn_FloatType_struct*
_tmp6E0=(struct Cyc_Absyn_FloatType_struct*)_tmp6DE;if(_tmp6E0->tag != 7)goto
_LL36A;}_LL369: goto _LL36B;_LL36A: {struct Cyc_Absyn_EnumType_struct*_tmp6E1=(
struct Cyc_Absyn_EnumType_struct*)_tmp6DE;if(_tmp6E1->tag != 13)goto _LL36C;}_LL36B:
goto _LL36D;_LL36C: {struct Cyc_Absyn_AnonEnumType_struct*_tmp6E2=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp6DE;if(_tmp6E2->tag != 14)goto _LL36E;}_LL36D: return 1;_LL36E:;_LL36F: return 0;
_LL365:;}int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp12AE;struct _tuple0 _tmp6E4=(
_tmp12AE.f1=t1,((_tmp12AE.f2=t2,_tmp12AE)));void*_tmp6E5;int _tmp6E7;void*_tmp6E8;
int _tmp6EA;void*_tmp6EB;void*_tmp6ED;void*_tmp6EF;void*_tmp6F1;void*_tmp6F3;
enum Cyc_Absyn_Size_of _tmp6F5;void*_tmp6F6;enum Cyc_Absyn_Size_of _tmp6F8;void*
_tmp6F9;enum Cyc_Absyn_Size_of _tmp6FB;void*_tmp6FC;enum Cyc_Absyn_Size_of _tmp6FE;
void*_tmp6FF;enum Cyc_Absyn_Size_of _tmp701;void*_tmp702;enum Cyc_Absyn_Size_of
_tmp704;void*_tmp705;enum Cyc_Absyn_Size_of _tmp707;void*_tmp708;enum Cyc_Absyn_Size_of
_tmp70A;void*_tmp70B;int _tmp70D;void*_tmp70E;enum Cyc_Absyn_Size_of _tmp710;void*
_tmp711;int _tmp713;void*_tmp714;enum Cyc_Absyn_Size_of _tmp716;void*_tmp717;enum 
Cyc_Absyn_Size_of _tmp719;void*_tmp71A;enum Cyc_Absyn_Size_of _tmp71C;void*_tmp71D;
enum Cyc_Absyn_Size_of _tmp71F;void*_tmp720;void*_tmp722;enum Cyc_Absyn_Size_of
_tmp724;void*_tmp725;enum Cyc_Absyn_Size_of _tmp727;void*_tmp728;enum Cyc_Absyn_Size_of
_tmp72A;void*_tmp72B;enum Cyc_Absyn_Size_of _tmp72D;void*_tmp72E;enum Cyc_Absyn_Size_of
_tmp730;void*_tmp731;enum Cyc_Absyn_Size_of _tmp733;void*_tmp734;enum Cyc_Absyn_Size_of
_tmp736;void*_tmp737;void*_tmp739;enum Cyc_Absyn_Size_of _tmp73B;_LL371: _tmp6E5=
_tmp6E4.f1;{struct Cyc_Absyn_FloatType_struct*_tmp6E6=(struct Cyc_Absyn_FloatType_struct*)
_tmp6E5;if(_tmp6E6->tag != 7)goto _LL373;else{_tmp6E7=_tmp6E6->f1;}};_tmp6E8=
_tmp6E4.f2;{struct Cyc_Absyn_FloatType_struct*_tmp6E9=(struct Cyc_Absyn_FloatType_struct*)
_tmp6E8;if(_tmp6E9->tag != 7)goto _LL373;else{_tmp6EA=_tmp6E9->f1;}};_LL372: return
_tmp6EA == 0  && _tmp6E7 != 0  || (_tmp6EA == 1  && _tmp6E7 != 0) && _tmp6E7 != 1;_LL373:
_tmp6EB=_tmp6E4.f1;{struct Cyc_Absyn_FloatType_struct*_tmp6EC=(struct Cyc_Absyn_FloatType_struct*)
_tmp6EB;if(_tmp6EC->tag != 7)goto _LL375;};_tmp6ED=_tmp6E4.f2;{struct Cyc_Absyn_IntType_struct*
_tmp6EE=(struct Cyc_Absyn_IntType_struct*)_tmp6ED;if(_tmp6EE->tag != 6)goto _LL375;};
_LL374: goto _LL376;_LL375: _tmp6EF=_tmp6E4.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp6F0=(struct Cyc_Absyn_FloatType_struct*)_tmp6EF;if(_tmp6F0->tag != 7)goto
_LL377;};_tmp6F1=_tmp6E4.f2;{struct Cyc_Absyn_TagType_struct*_tmp6F2=(struct Cyc_Absyn_TagType_struct*)
_tmp6F1;if(_tmp6F2->tag != 19)goto _LL377;};_LL376: return 1;_LL377: _tmp6F3=_tmp6E4.f1;{
struct Cyc_Absyn_IntType_struct*_tmp6F4=(struct Cyc_Absyn_IntType_struct*)_tmp6F3;
if(_tmp6F4->tag != 6)goto _LL379;else{_tmp6F5=_tmp6F4->f2;if(_tmp6F5 != Cyc_Absyn_LongLong_sz)
goto _LL379;}};_tmp6F6=_tmp6E4.f2;{struct Cyc_Absyn_IntType_struct*_tmp6F7=(struct
Cyc_Absyn_IntType_struct*)_tmp6F6;if(_tmp6F7->tag != 6)goto _LL379;else{_tmp6F8=
_tmp6F7->f2;if(_tmp6F8 != Cyc_Absyn_LongLong_sz)goto _LL379;}};_LL378: return 0;
_LL379: _tmp6F9=_tmp6E4.f1;{struct Cyc_Absyn_IntType_struct*_tmp6FA=(struct Cyc_Absyn_IntType_struct*)
_tmp6F9;if(_tmp6FA->tag != 6)goto _LL37B;else{_tmp6FB=_tmp6FA->f2;if(_tmp6FB != Cyc_Absyn_LongLong_sz)
goto _LL37B;}};_LL37A: return 1;_LL37B: _tmp6FC=_tmp6E4.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6FD=(struct Cyc_Absyn_IntType_struct*)_tmp6FC;if(_tmp6FD->tag != 6)goto _LL37D;
else{_tmp6FE=_tmp6FD->f2;if(_tmp6FE != Cyc_Absyn_Long_sz)goto _LL37D;}};_tmp6FF=
_tmp6E4.f2;{struct Cyc_Absyn_IntType_struct*_tmp700=(struct Cyc_Absyn_IntType_struct*)
_tmp6FF;if(_tmp700->tag != 6)goto _LL37D;else{_tmp701=_tmp700->f2;if(_tmp701 != Cyc_Absyn_Int_sz)
goto _LL37D;}};_LL37C: goto _LL37E;_LL37D: _tmp702=_tmp6E4.f1;{struct Cyc_Absyn_IntType_struct*
_tmp703=(struct Cyc_Absyn_IntType_struct*)_tmp702;if(_tmp703->tag != 6)goto _LL37F;
else{_tmp704=_tmp703->f2;if(_tmp704 != Cyc_Absyn_Int_sz)goto _LL37F;}};_tmp705=
_tmp6E4.f2;{struct Cyc_Absyn_IntType_struct*_tmp706=(struct Cyc_Absyn_IntType_struct*)
_tmp705;if(_tmp706->tag != 6)goto _LL37F;else{_tmp707=_tmp706->f2;if(_tmp707 != Cyc_Absyn_Long_sz)
goto _LL37F;}};_LL37E: return 0;_LL37F: _tmp708=_tmp6E4.f1;{struct Cyc_Absyn_IntType_struct*
_tmp709=(struct Cyc_Absyn_IntType_struct*)_tmp708;if(_tmp709->tag != 6)goto _LL381;
else{_tmp70A=_tmp709->f2;if(_tmp70A != Cyc_Absyn_Long_sz)goto _LL381;}};_tmp70B=
_tmp6E4.f2;{struct Cyc_Absyn_FloatType_struct*_tmp70C=(struct Cyc_Absyn_FloatType_struct*)
_tmp70B;if(_tmp70C->tag != 7)goto _LL381;else{_tmp70D=_tmp70C->f1;if(_tmp70D != 0)
goto _LL381;}};_LL380: goto _LL382;_LL381: _tmp70E=_tmp6E4.f1;{struct Cyc_Absyn_IntType_struct*
_tmp70F=(struct Cyc_Absyn_IntType_struct*)_tmp70E;if(_tmp70F->tag != 6)goto _LL383;
else{_tmp710=_tmp70F->f2;if(_tmp710 != Cyc_Absyn_Int_sz)goto _LL383;}};_tmp711=
_tmp6E4.f2;{struct Cyc_Absyn_FloatType_struct*_tmp712=(struct Cyc_Absyn_FloatType_struct*)
_tmp711;if(_tmp712->tag != 7)goto _LL383;else{_tmp713=_tmp712->f1;if(_tmp713 != 0)
goto _LL383;}};_LL382: goto _LL384;_LL383: _tmp714=_tmp6E4.f1;{struct Cyc_Absyn_IntType_struct*
_tmp715=(struct Cyc_Absyn_IntType_struct*)_tmp714;if(_tmp715->tag != 6)goto _LL385;
else{_tmp716=_tmp715->f2;if(_tmp716 != Cyc_Absyn_Long_sz)goto _LL385;}};_tmp717=
_tmp6E4.f2;{struct Cyc_Absyn_IntType_struct*_tmp718=(struct Cyc_Absyn_IntType_struct*)
_tmp717;if(_tmp718->tag != 6)goto _LL385;else{_tmp719=_tmp718->f2;if(_tmp719 != Cyc_Absyn_Short_sz)
goto _LL385;}};_LL384: goto _LL386;_LL385: _tmp71A=_tmp6E4.f1;{struct Cyc_Absyn_IntType_struct*
_tmp71B=(struct Cyc_Absyn_IntType_struct*)_tmp71A;if(_tmp71B->tag != 6)goto _LL387;
else{_tmp71C=_tmp71B->f2;if(_tmp71C != Cyc_Absyn_Int_sz)goto _LL387;}};_tmp71D=
_tmp6E4.f2;{struct Cyc_Absyn_IntType_struct*_tmp71E=(struct Cyc_Absyn_IntType_struct*)
_tmp71D;if(_tmp71E->tag != 6)goto _LL387;else{_tmp71F=_tmp71E->f2;if(_tmp71F != Cyc_Absyn_Short_sz)
goto _LL387;}};_LL386: goto _LL388;_LL387: _tmp720=_tmp6E4.f1;{struct Cyc_Absyn_TagType_struct*
_tmp721=(struct Cyc_Absyn_TagType_struct*)_tmp720;if(_tmp721->tag != 19)goto _LL389;};
_tmp722=_tmp6E4.f2;{struct Cyc_Absyn_IntType_struct*_tmp723=(struct Cyc_Absyn_IntType_struct*)
_tmp722;if(_tmp723->tag != 6)goto _LL389;else{_tmp724=_tmp723->f2;if(_tmp724 != Cyc_Absyn_Short_sz)
goto _LL389;}};_LL388: goto _LL38A;_LL389: _tmp725=_tmp6E4.f1;{struct Cyc_Absyn_IntType_struct*
_tmp726=(struct Cyc_Absyn_IntType_struct*)_tmp725;if(_tmp726->tag != 6)goto _LL38B;
else{_tmp727=_tmp726->f2;if(_tmp727 != Cyc_Absyn_Long_sz)goto _LL38B;}};_tmp728=
_tmp6E4.f2;{struct Cyc_Absyn_IntType_struct*_tmp729=(struct Cyc_Absyn_IntType_struct*)
_tmp728;if(_tmp729->tag != 6)goto _LL38B;else{_tmp72A=_tmp729->f2;if(_tmp72A != Cyc_Absyn_Char_sz)
goto _LL38B;}};_LL38A: goto _LL38C;_LL38B: _tmp72B=_tmp6E4.f1;{struct Cyc_Absyn_IntType_struct*
_tmp72C=(struct Cyc_Absyn_IntType_struct*)_tmp72B;if(_tmp72C->tag != 6)goto _LL38D;
else{_tmp72D=_tmp72C->f2;if(_tmp72D != Cyc_Absyn_Int_sz)goto _LL38D;}};_tmp72E=
_tmp6E4.f2;{struct Cyc_Absyn_IntType_struct*_tmp72F=(struct Cyc_Absyn_IntType_struct*)
_tmp72E;if(_tmp72F->tag != 6)goto _LL38D;else{_tmp730=_tmp72F->f2;if(_tmp730 != Cyc_Absyn_Char_sz)
goto _LL38D;}};_LL38C: goto _LL38E;_LL38D: _tmp731=_tmp6E4.f1;{struct Cyc_Absyn_IntType_struct*
_tmp732=(struct Cyc_Absyn_IntType_struct*)_tmp731;if(_tmp732->tag != 6)goto _LL38F;
else{_tmp733=_tmp732->f2;if(_tmp733 != Cyc_Absyn_Short_sz)goto _LL38F;}};_tmp734=
_tmp6E4.f2;{struct Cyc_Absyn_IntType_struct*_tmp735=(struct Cyc_Absyn_IntType_struct*)
_tmp734;if(_tmp735->tag != 6)goto _LL38F;else{_tmp736=_tmp735->f2;if(_tmp736 != Cyc_Absyn_Char_sz)
goto _LL38F;}};_LL38E: goto _LL390;_LL38F: _tmp737=_tmp6E4.f1;{struct Cyc_Absyn_TagType_struct*
_tmp738=(struct Cyc_Absyn_TagType_struct*)_tmp737;if(_tmp738->tag != 19)goto _LL391;};
_tmp739=_tmp6E4.f2;{struct Cyc_Absyn_IntType_struct*_tmp73A=(struct Cyc_Absyn_IntType_struct*)
_tmp739;if(_tmp73A->tag != 6)goto _LL391;else{_tmp73B=_tmp73A->f2;if(_tmp73B != Cyc_Absyn_Char_sz)
goto _LL391;}};_LL390: return 1;_LL391:;_LL392: return 0;_LL370:;};}int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct _RegionHandle*
_tmp73C=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(
t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(t1,(void*)
max_arith_type->v)){struct Cyc_Core_Opt*_tmp12AF;max_arith_type=((_tmp12AF=
_region_malloc(_tmp73C,sizeof(*_tmp12AF)),((_tmp12AF->v=t1,_tmp12AF))));}}}}if(
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
te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,e)){void*_tmp742=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL394: {struct Cyc_Absyn_PointerType_struct*
_tmp743=(struct Cyc_Absyn_PointerType_struct*)_tmp742;if(_tmp743->tag != 5)goto
_LL396;}_LL395: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);
goto _LL393;_LL396:;_LL397: return 0;_LL393:;}return 1;}int Cyc_Tcutil_is_integral_type(
void*t){void*_tmp744=Cyc_Tcutil_compress(t);_LL399: {struct Cyc_Absyn_IntType_struct*
_tmp745=(struct Cyc_Absyn_IntType_struct*)_tmp744;if(_tmp745->tag != 6)goto _LL39B;}
_LL39A: goto _LL39C;_LL39B: {struct Cyc_Absyn_TagType_struct*_tmp746=(struct Cyc_Absyn_TagType_struct*)
_tmp744;if(_tmp746->tag != 19)goto _LL39D;}_LL39C: goto _LL39E;_LL39D: {struct Cyc_Absyn_EnumType_struct*
_tmp747=(struct Cyc_Absyn_EnumType_struct*)_tmp744;if(_tmp747->tag != 13)goto
_LL39F;}_LL39E: goto _LL3A0;_LL39F: {struct Cyc_Absyn_AnonEnumType_struct*_tmp748=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp744;if(_tmp748->tag != 14)goto _LL3A1;}
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
Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp12BB;struct _tuple0 _tmp74E=(_tmp12BB.f1=
t1,((_tmp12BB.f2=t2,_tmp12BB)));void*_tmp74F;struct Cyc_Absyn_PtrInfo _tmp751;void*
_tmp752;struct Cyc_Absyn_PtrInfo _tmp754;void*_tmp755;struct Cyc_Absyn_ArrayInfo
_tmp757;void*_tmp758;struct Cyc_Absyn_Tqual _tmp759;struct Cyc_Absyn_Exp*_tmp75A;
union Cyc_Absyn_Constraint*_tmp75B;void*_tmp75C;struct Cyc_Absyn_ArrayInfo _tmp75E;
void*_tmp75F;struct Cyc_Absyn_Tqual _tmp760;struct Cyc_Absyn_Exp*_tmp761;union Cyc_Absyn_Constraint*
_tmp762;void*_tmp763;void*_tmp765;_LL3A4: _tmp74F=_tmp74E.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp750=(struct Cyc_Absyn_PointerType_struct*)_tmp74F;if(_tmp750->tag != 5)goto
_LL3A6;else{_tmp751=_tmp750->f1;}};_tmp752=_tmp74E.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp753=(struct Cyc_Absyn_PointerType_struct*)_tmp752;if(_tmp753->tag != 5)goto
_LL3A6;else{_tmp754=_tmp753->f1;}};_LL3A5: {int okay=1;if(!((int(*)(int(*cmp)(int,
int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp751.ptr_atts).nullable,(_tmp754.ptr_atts).nullable))okay=!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp751.ptr_atts).nullable);
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp751.ptr_atts).bounds,(
_tmp754.ptr_atts).bounds)){struct _tuple0 _tmp12BC;struct _tuple0 _tmp768=(_tmp12BC.f1=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(
_tmp751.ptr_atts).bounds),((_tmp12BC.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp754.ptr_atts).bounds),
_tmp12BC)));void*_tmp769;void*_tmp76B;void*_tmp76D;void*_tmp76F;void*_tmp771;
struct Cyc_Absyn_Exp*_tmp773;void*_tmp774;struct Cyc_Absyn_Exp*_tmp776;void*
_tmp777;void*_tmp779;struct Cyc_Absyn_Exp*_tmp77B;_LL3AD: _tmp769=_tmp768.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp76A=(struct Cyc_Absyn_Upper_b_struct*)_tmp769;
if(_tmp76A->tag != 1)goto _LL3AF;};_tmp76B=_tmp768.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp76C=(struct Cyc_Absyn_DynEither_b_struct*)_tmp76B;if(_tmp76C->tag != 0)goto
_LL3AF;};_LL3AE: goto _LL3B0;_LL3AF: _tmp76D=_tmp768.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp76E=(struct Cyc_Absyn_DynEither_b_struct*)_tmp76D;if(_tmp76E->tag != 0)goto
_LL3B1;};_tmp76F=_tmp768.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp770=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp76F;if(_tmp770->tag != 0)goto _LL3B1;};_LL3B0:
okay=1;goto _LL3AC;_LL3B1: _tmp771=_tmp768.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp772=(struct Cyc_Absyn_Upper_b_struct*)_tmp771;if(_tmp772->tag != 1)goto _LL3B3;
else{_tmp773=_tmp772->f1;}};_tmp774=_tmp768.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp775=(struct Cyc_Absyn_Upper_b_struct*)_tmp774;if(_tmp775->tag != 1)goto _LL3B3;
else{_tmp776=_tmp775->f1;}};_LL3B2: okay=okay  && Cyc_Evexp_lte_const_exp(_tmp776,
_tmp773);if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,(_tmp754.ptr_atts).zero_term)){const char*_tmp12BF;void*_tmp12BE;(_tmp12BE=0,
Cyc_Tcutil_warn(loc,((_tmp12BF="implicit cast to shorter array",_tag_dyneither(
_tmp12BF,sizeof(char),31))),_tag_dyneither(_tmp12BE,sizeof(void*),0)));}goto
_LL3AC;_LL3B3: _tmp777=_tmp768.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp778=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp777;if(_tmp778->tag != 0)goto _LL3AC;};
_tmp779=_tmp768.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp77A=(struct Cyc_Absyn_Upper_b_struct*)
_tmp779;if(_tmp77A->tag != 1)goto _LL3AC;else{_tmp77B=_tmp77A->f1;}};_LL3B4: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp751.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp754.ptr_atts).bounds))goto _LL3AC;okay=0;goto
_LL3AC;_LL3AC:;}okay=okay  && (!(_tmp751.elt_tq).real_const  || (_tmp754.elt_tq).real_const);
okay=okay  && (Cyc_Tcutil_unify((_tmp751.ptr_atts).rgn,(_tmp754.ptr_atts).rgn)
 || Cyc_Tcenv_region_outlives(te,(_tmp751.ptr_atts).rgn,(_tmp754.ptr_atts).rgn));
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp751.ptr_atts).zero_term,(
_tmp754.ptr_atts).zero_term) || ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
1,(_tmp751.ptr_atts).zero_term) && (_tmp754.elt_tq).real_const);{int _tmp77E=((
int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp754.ptr_atts).bounds,
Cyc_Absyn_bounds_one_conref) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,(_tmp754.ptr_atts).zero_term);okay=okay  && (Cyc_Tcutil_unify(_tmp751.elt_typ,
_tmp754.elt_typ) || (_tmp77E  && ((_tmp754.elt_tq).real_const  || Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp754.elt_typ)))) && Cyc_Tcutil_ptrsubtype(
te,0,_tmp751.elt_typ,_tmp754.elt_typ));return okay;};}_LL3A6: _tmp755=_tmp74E.f1;{
struct Cyc_Absyn_ArrayType_struct*_tmp756=(struct Cyc_Absyn_ArrayType_struct*)
_tmp755;if(_tmp756->tag != 8)goto _LL3A8;else{_tmp757=_tmp756->f1;_tmp758=_tmp757.elt_type;
_tmp759=_tmp757.tq;_tmp75A=_tmp757.num_elts;_tmp75B=_tmp757.zero_term;}};_tmp75C=
_tmp74E.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp75D=(struct Cyc_Absyn_ArrayType_struct*)
_tmp75C;if(_tmp75D->tag != 8)goto _LL3A8;else{_tmp75E=_tmp75D->f1;_tmp75F=_tmp75E.elt_type;
_tmp760=_tmp75E.tq;_tmp761=_tmp75E.num_elts;_tmp762=_tmp75E.zero_term;}};_LL3A7: {
int okay;okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp75B,_tmp762) && ((_tmp75A != 0
 && _tmp761 != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp75A,(
struct Cyc_Absyn_Exp*)_tmp761));return(okay  && Cyc_Tcutil_unify(_tmp758,_tmp75F))
 && (!_tmp759.real_const  || _tmp760.real_const);}_LL3A8: _tmp763=_tmp74E.f1;{
struct Cyc_Absyn_TagType_struct*_tmp764=(struct Cyc_Absyn_TagType_struct*)_tmp763;
if(_tmp764->tag != 19)goto _LL3AA;};_tmp765=_tmp74E.f2;{struct Cyc_Absyn_IntType_struct*
_tmp766=(struct Cyc_Absyn_IntType_struct*)_tmp765;if(_tmp766->tag != 6)goto _LL3AA;};
_LL3A9: return 0;_LL3AA:;_LL3AB: return Cyc_Tcutil_unify(t1,t2);_LL3A3:;};}int Cyc_Tcutil_is_pointer_type(
void*t){void*_tmp77F=Cyc_Tcutil_compress(t);_LL3B6: {struct Cyc_Absyn_PointerType_struct*
_tmp780=(struct Cyc_Absyn_PointerType_struct*)_tmp77F;if(_tmp780->tag != 5)goto
_LL3B8;}_LL3B7: return 1;_LL3B8:;_LL3B9: return 0;_LL3B5:;}void*Cyc_Tcutil_pointer_elt_type(
void*t){void*_tmp781=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp783;void*
_tmp784;_LL3BB: {struct Cyc_Absyn_PointerType_struct*_tmp782=(struct Cyc_Absyn_PointerType_struct*)
_tmp781;if(_tmp782->tag != 5)goto _LL3BD;else{_tmp783=_tmp782->f1;_tmp784=_tmp783.elt_typ;}}
_LL3BC: return _tmp784;_LL3BD:;_LL3BE: {const char*_tmp12C2;void*_tmp12C1;(_tmp12C1=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp12C2="pointer_elt_type",_tag_dyneither(_tmp12C2,sizeof(char),17))),
_tag_dyneither(_tmp12C1,sizeof(void*),0)));}_LL3BA:;}void*Cyc_Tcutil_pointer_region(
void*t){void*_tmp787=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp789;
struct Cyc_Absyn_PtrAtts _tmp78A;struct Cyc_Absyn_PtrAtts*_tmp78B;_LL3C0: {struct
Cyc_Absyn_PointerType_struct*_tmp788=(struct Cyc_Absyn_PointerType_struct*)
_tmp787;if(_tmp788->tag != 5)goto _LL3C2;else{_tmp789=_tmp788->f1;_tmp78A=_tmp789.ptr_atts;
_tmp78B=(struct Cyc_Absyn_PtrAtts*)&(_tmp788->f1).ptr_atts;}}_LL3C1: return _tmp78B->rgn;
_LL3C2:;_LL3C3: {const char*_tmp12C5;void*_tmp12C4;(_tmp12C4=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C5="pointer_elt_type",
_tag_dyneither(_tmp12C5,sizeof(char),17))),_tag_dyneither(_tmp12C4,sizeof(void*),
0)));}_LL3BF:;}int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){void*_tmp78E=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp790;struct Cyc_Absyn_PtrAtts _tmp791;void*_tmp792;
_LL3C5: {struct Cyc_Absyn_PointerType_struct*_tmp78F=(struct Cyc_Absyn_PointerType_struct*)
_tmp78E;if(_tmp78F->tag != 5)goto _LL3C7;else{_tmp790=_tmp78F->f1;_tmp791=_tmp790.ptr_atts;
_tmp792=_tmp791.rgn;}}_LL3C6:*rgn=_tmp792;return 1;_LL3C7:;_LL3C8: return 0;_LL3C4:;}
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr){void*_tmp793=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp795;struct Cyc_Absyn_PtrAtts _tmp796;union Cyc_Absyn_Constraint*
_tmp797;_LL3CA: {struct Cyc_Absyn_PointerType_struct*_tmp794=(struct Cyc_Absyn_PointerType_struct*)
_tmp793;if(_tmp794->tag != 5)goto _LL3CC;else{_tmp795=_tmp794->f1;_tmp796=_tmp795.ptr_atts;
_tmp797=_tmp796.bounds;}}_LL3CB:*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp797)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL3CC:;_LL3CD: return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;
_LL3C9:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){void*_tmp798=e->r;union Cyc_Absyn_Cnst
_tmp79A;struct _tuple7 _tmp79B;int _tmp79C;union Cyc_Absyn_Cnst _tmp79E;struct _tuple5
_tmp79F;char _tmp7A0;union Cyc_Absyn_Cnst _tmp7A2;struct _tuple6 _tmp7A3;short _tmp7A4;
union Cyc_Absyn_Cnst _tmp7A6;struct _tuple8 _tmp7A7;long long _tmp7A8;union Cyc_Absyn_Cnst
_tmp7AA;struct _dyneither_ptr _tmp7AB;void*_tmp7AD;struct Cyc_Absyn_Exp*_tmp7AE;
_LL3CF: {struct Cyc_Absyn_Const_e_struct*_tmp799=(struct Cyc_Absyn_Const_e_struct*)
_tmp798;if(_tmp799->tag != 0)goto _LL3D1;else{_tmp79A=_tmp799->f1;if((_tmp79A.Int_c).tag
!= 5)goto _LL3D1;_tmp79B=(struct _tuple7)(_tmp79A.Int_c).val;_tmp79C=_tmp79B.f2;
if(_tmp79C != 0)goto _LL3D1;}}_LL3D0: goto _LL3D2;_LL3D1: {struct Cyc_Absyn_Const_e_struct*
_tmp79D=(struct Cyc_Absyn_Const_e_struct*)_tmp798;if(_tmp79D->tag != 0)goto _LL3D3;
else{_tmp79E=_tmp79D->f1;if((_tmp79E.Char_c).tag != 2)goto _LL3D3;_tmp79F=(struct
_tuple5)(_tmp79E.Char_c).val;_tmp7A0=_tmp79F.f2;if(_tmp7A0 != 0)goto _LL3D3;}}
_LL3D2: goto _LL3D4;_LL3D3: {struct Cyc_Absyn_Const_e_struct*_tmp7A1=(struct Cyc_Absyn_Const_e_struct*)
_tmp798;if(_tmp7A1->tag != 0)goto _LL3D5;else{_tmp7A2=_tmp7A1->f1;if((_tmp7A2.Short_c).tag
!= 4)goto _LL3D5;_tmp7A3=(struct _tuple6)(_tmp7A2.Short_c).val;_tmp7A4=_tmp7A3.f2;
if(_tmp7A4 != 0)goto _LL3D5;}}_LL3D4: goto _LL3D6;_LL3D5: {struct Cyc_Absyn_Const_e_struct*
_tmp7A5=(struct Cyc_Absyn_Const_e_struct*)_tmp798;if(_tmp7A5->tag != 0)goto _LL3D7;
else{_tmp7A6=_tmp7A5->f1;if((_tmp7A6.LongLong_c).tag != 6)goto _LL3D7;_tmp7A7=(
struct _tuple8)(_tmp7A6.LongLong_c).val;_tmp7A8=_tmp7A7.f2;if(_tmp7A8 != 0)goto
_LL3D7;}}_LL3D6: return 1;_LL3D7: {struct Cyc_Absyn_Const_e_struct*_tmp7A9=(struct
Cyc_Absyn_Const_e_struct*)_tmp798;if(_tmp7A9->tag != 0)goto _LL3D9;else{_tmp7AA=
_tmp7A9->f1;if((_tmp7AA.Wchar_c).tag != 3)goto _LL3D9;_tmp7AB=(struct
_dyneither_ptr)(_tmp7AA.Wchar_c).val;}}_LL3D8: {unsigned long _tmp7AF=Cyc_strlen((
struct _dyneither_ptr)_tmp7AB);int i=0;if(_tmp7AF >= 2  && *((const char*)
_check_dyneither_subscript(_tmp7AB,sizeof(char),0))== '\\'){if(*((const char*)
_check_dyneither_subscript(_tmp7AB,sizeof(char),1))== '0')i=2;else{if((*((const
char*)_check_dyneither_subscript(_tmp7AB,sizeof(char),1))== 'x'  && _tmp7AF >= 3)
 && *((const char*)_check_dyneither_subscript(_tmp7AB,sizeof(char),2))== '0')i=3;
else{return 0;}}for(0;i < _tmp7AF;++ i){if(*((const char*)_check_dyneither_subscript(
_tmp7AB,sizeof(char),i))!= '0')return 0;}return 1;}else{return 0;}}_LL3D9: {struct
Cyc_Absyn_Cast_e_struct*_tmp7AC=(struct Cyc_Absyn_Cast_e_struct*)_tmp798;if(
_tmp7AC->tag != 15)goto _LL3DB;else{_tmp7AD=(void*)_tmp7AC->f1;_tmp7AE=_tmp7AC->f2;}}
_LL3DA: return Cyc_Tcutil_is_zero(_tmp7AE) && Cyc_Tcutil_admits_zero(_tmp7AD);
_LL3DB:;_LL3DC: return 0;_LL3CE:;}struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,
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
ka){struct Cyc_Absyn_Kind _tmp7B1;enum Cyc_Absyn_KindQual _tmp7B2;enum Cyc_Absyn_AliasQual
_tmp7B3;struct Cyc_Absyn_Kind*_tmp7B0=ka;_tmp7B1=*_tmp7B0;_tmp7B2=_tmp7B1.kind;
_tmp7B3=_tmp7B1.aliasqual;switch(_tmp7B3){case Cyc_Absyn_Aliasable: _LL3DD: switch(
_tmp7B2){case Cyc_Absyn_AnyKind: _LL3DF: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ako;
case Cyc_Absyn_MemKind: _LL3E0: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind:
_LL3E1: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3E2:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3E3: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3E4: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_iko;}case Cyc_Absyn_Unique: _LL3DE: switch(_tmp7B2){case
Cyc_Absyn_AnyKind: _LL3E7: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind:
_LL3E8: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3E9:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3EA: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_urko;default: _LL3EB: break;}break;case Cyc_Absyn_Top:
_LL3E6: switch(_tmp7B2){case Cyc_Absyn_AnyKind: _LL3EE: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3EF: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko;
case Cyc_Absyn_BoxKind: _LL3F0: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind:
_LL3F1: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_trko;default: _LL3F2: break;}break;}{
const char*_tmp12C9;void*_tmp12C8[1];struct Cyc_String_pa_struct _tmp12C7;(_tmp12C7.tag=
0,((_tmp12C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
ka)),((_tmp12C8[0]=& _tmp12C7,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C9="kind_to_opt: bad kind %s\n",
_tag_dyneither(_tmp12C9,sizeof(char),26))),_tag_dyneither(_tmp12C8,sizeof(void*),
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
ubb_v;static void*urb=(void*)& urb_v;struct Cyc_Absyn_Kind _tmp7B8;enum Cyc_Absyn_KindQual
_tmp7B9;enum Cyc_Absyn_AliasQual _tmp7BA;struct Cyc_Absyn_Kind*_tmp7B7=ka;_tmp7B8=*
_tmp7B7;_tmp7B9=_tmp7B8.kind;_tmp7BA=_tmp7B8.aliasqual;switch(_tmp7BA){case Cyc_Absyn_Aliasable:
_LL3F4: switch(_tmp7B9){case Cyc_Absyn_AnyKind: _LL3F6: return& ab;case Cyc_Absyn_MemKind:
_LL3F7: return& mb;case Cyc_Absyn_BoxKind: _LL3F8: return& bb;case Cyc_Absyn_RgnKind:
_LL3F9: return& rb;case Cyc_Absyn_EffKind: _LL3FA: return& eb;case Cyc_Absyn_IntKind:
_LL3FB: return& ib;}case Cyc_Absyn_Unique: _LL3F5: switch(_tmp7B9){case Cyc_Absyn_AnyKind:
_LL3FE: return& uab;case Cyc_Absyn_MemKind: _LL3FF: return& umb;case Cyc_Absyn_BoxKind:
_LL400: return& ubb;case Cyc_Absyn_RgnKind: _LL401: return& urb;default: _LL402: break;}
break;case Cyc_Absyn_Top: _LL3FD: switch(_tmp7B9){case Cyc_Absyn_AnyKind: _LL405:
return& tab;case Cyc_Absyn_MemKind: _LL406: return& tmb;case Cyc_Absyn_BoxKind: _LL407:
return& tbb;case Cyc_Absyn_RgnKind: _LL408: return& trb;default: _LL409: break;}break;}{
const char*_tmp12CD;void*_tmp12CC[1];struct Cyc_String_pa_struct _tmp12CB;(_tmp12CB.tag=
0,((_tmp12CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
ka)),((_tmp12CC[0]=& _tmp12CB,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12CD="kind_to_b: bad kind %s\n",
_tag_dyneither(_tmp12CD,sizeof(char),24))),_tag_dyneither(_tmp12CC,sizeof(void*),
1)))))));};}void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){return*Cyc_Tcutil_kind_to_b(
k);}struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
struct Cyc_Core_Opt*_tmp12CE;return(_tmp12CE=_cycalloc(sizeof(*_tmp12CE)),((
_tmp12CE->v=Cyc_Tcutil_kind_to_bound(k),_tmp12CE)));}int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_struct _tmp12D1;struct Cyc_Absyn_Const_e_struct*
_tmp12D0;e1->r=(void*)((_tmp12D0=_cycalloc(sizeof(*_tmp12D0)),((_tmp12D0[0]=((
_tmp12D1.tag=0,((_tmp12D1.f1=Cyc_Absyn_Null_c,_tmp12D1)))),_tmp12D0))));}{struct
Cyc_Core_Opt*_tmp7CF=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct
_tmp12DB;struct Cyc_Absyn_PtrAtts _tmp12DA;struct Cyc_Absyn_PtrInfo _tmp12D9;struct
Cyc_Absyn_PointerType_struct*_tmp12D8;struct Cyc_Absyn_PointerType_struct*_tmp7D0=(
_tmp12D8=_cycalloc(sizeof(*_tmp12D8)),((_tmp12D8[0]=((_tmp12DB.tag=5,((_tmp12DB.f1=((
_tmp12D9.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmp7CF),((
_tmp12D9.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp12D9.ptr_atts=((_tmp12DA.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,_tmp7CF),((_tmp12DA.nullable=Cyc_Absyn_true_conref,((
_tmp12DA.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp12DA.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp12DA.ptrloc=0,_tmp12DA)))))))))),_tmp12D9)))))),_tmp12DB)))),_tmp12D8)));((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp7D0);{int bogus=
0;int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);if(bogus != 0){const char*
_tmp12E0;void*_tmp12DF[2];struct Cyc_String_pa_struct _tmp12DE;struct Cyc_String_pa_struct
_tmp12DD;(_tmp12DD.tag=0,((_tmp12DD.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Position_string_of_segment(e1->loc)),((_tmp12DE.tag=0,((
_tmp12DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp12DF[0]=& _tmp12DE,((_tmp12DF[1]=& _tmp12DD,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12E0="zero_to_null resulted in an alias coercion on %s at %s\n",
_tag_dyneither(_tmp12E0,sizeof(char),56))),_tag_dyneither(_tmp12DF,sizeof(void*),
2)))))))))))));}return retv;};};}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(
enum Cyc_Absyn_Coercion c){switch(c){case Cyc_Absyn_Unknown_coercion: _LL40B: {
const char*_tmp12E1;return(_tmp12E1="unknown",_tag_dyneither(_tmp12E1,sizeof(char),
8));}case Cyc_Absyn_No_coercion: _LL40C: {const char*_tmp12E2;return(_tmp12E2="no coercion",
_tag_dyneither(_tmp12E2,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL40D: {
const char*_tmp12E3;return(_tmp12E3="null check",_tag_dyneither(_tmp12E3,sizeof(
char),11));}case Cyc_Absyn_Other_coercion: _LL40E: {const char*_tmp12E4;return(
_tmp12E4="other coercion",_tag_dyneither(_tmp12E4,sizeof(char),15));}}}int Cyc_Tcutil_warn_alias_coerce=
0;struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){static
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
void*_tmp7DD=Cyc_Tcutil_compress(e_typ);struct Cyc_Absyn_PtrInfo _tmp7DF;struct Cyc_Absyn_PtrAtts
_tmp7E0;void*_tmp7E1;void**_tmp7E2;_LL411: {struct Cyc_Absyn_PointerType_struct*
_tmp7DE=(struct Cyc_Absyn_PointerType_struct*)_tmp7DD;if(_tmp7DE->tag != 5)goto
_LL413;else{_tmp7DF=_tmp7DE->f1;_tmp7E0=_tmp7DF.ptr_atts;_tmp7E1=_tmp7E0.rgn;
_tmp7E2=(void**)&((_tmp7DE->f1).ptr_atts).rgn;}}_LL412:{struct Cyc_Absyn_VarType_struct
_tmp12F7;struct Cyc_Absyn_VarType_struct*_tmp12F6;*_tmp7E2=(void*)((_tmp12F6=
_cycalloc(sizeof(*_tmp12F6)),((_tmp12F6[0]=((_tmp12F7.tag=2,((_tmp12F7.f1=tv,
_tmp12F7)))),_tmp12F6))));}goto _LL410;_LL413:;_LL414: goto _LL410;_LL410:;}e->topt=
0;e=Cyc_Absyn_cast_exp(e_typ,e,0,Cyc_Absyn_Unknown_coercion,e->loc);{struct Cyc_Absyn_Decl*
d=Cyc_Absyn_alias_decl(e,tv,vd,e->loc);struct _tuple15 _tmp12F8;return(_tmp12F8.f1=
d,((_tmp12F8.f2=ve,_tmp12F8)));};}static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*
e,void*e_typ,void*wants_typ,struct Cyc_Position_Segment*loc){struct _RegionHandle
_tmp7EE=_new_region("r");struct _RegionHandle*r=& _tmp7EE;_push_region(r);if((Cyc_Tcutil_is_noalias_path(
r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ)) && Cyc_Tcutil_is_pointer_type(
e_typ)){void*_tmp7EF=Cyc_Tcutil_compress(wants_typ);struct Cyc_Absyn_PtrInfo
_tmp7F1;struct Cyc_Absyn_PtrAtts _tmp7F2;void*_tmp7F3;_LL416: {struct Cyc_Absyn_PointerType_struct*
_tmp7F0=(struct Cyc_Absyn_PointerType_struct*)_tmp7EF;if(_tmp7F0->tag != 5)goto
_LL418;else{_tmp7F1=_tmp7F0->f1;_tmp7F2=_tmp7F1.ptr_atts;_tmp7F3=_tmp7F2.rgn;}}
_LL417: {void*_tmp7F5=Cyc_Tcutil_compress(_tmp7F3);_LL41D: {struct Cyc_Absyn_HeapRgn_struct*
_tmp7F6=(struct Cyc_Absyn_HeapRgn_struct*)_tmp7F5;if(_tmp7F6->tag != 20)goto _LL41F;}
_LL41E: {int _tmp7F7=0;_npop_handler(0);return _tmp7F7;}_LL41F:;_LL420: {struct Cyc_Absyn_Kind*
_tmp7F8=Cyc_Tcutil_typ_kind(_tmp7F3);int _tmp7F9=_tmp7F8->kind == Cyc_Absyn_RgnKind
 && _tmp7F8->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp7F9;}
_LL41C:;}_LL418: {struct Cyc_Absyn_TypedefType_struct*_tmp7F4=(struct Cyc_Absyn_TypedefType_struct*)
_tmp7EF;if(_tmp7F4->tag != 17)goto _LL41A;}_LL419: {const char*_tmp12FD;void*
_tmp12FC[2];struct Cyc_String_pa_struct _tmp12FB;struct Cyc_String_pa_struct
_tmp12FA;(_tmp12FA.tag=0,((_tmp12FA.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Position_string_of_segment(loc)),((_tmp12FB.tag=0,((_tmp12FB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(
wants_typ))),((_tmp12FC[0]=& _tmp12FB,((_tmp12FC[1]=& _tmp12FA,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12FD="got typedef %s in can_insert_alias at %s\n",
_tag_dyneither(_tmp12FD,sizeof(char),42))),_tag_dyneither(_tmp12FC,sizeof(void*),
2)))))))))))));}_LL41A:;_LL41B: {int _tmp7FE=0;_npop_handler(0);return _tmp7FE;}
_LL415:;}{int _tmp7FF=0;_npop_handler(0);return _tmp7FF;};;_pop_region(r);}int Cyc_Tcutil_coerce_arg(
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
void*t){void*_tmp80D=Cyc_Tcutil_compress(t);_LL422: {struct Cyc_Absyn_IntType_struct*
_tmp80E=(struct Cyc_Absyn_IntType_struct*)_tmp80D;if(_tmp80E->tag != 6)goto _LL424;}
_LL423: goto _LL425;_LL424: {struct Cyc_Absyn_FloatType_struct*_tmp80F=(struct Cyc_Absyn_FloatType_struct*)
_tmp80D;if(_tmp80F->tag != 7)goto _LL426;}_LL425: return 1;_LL426:;_LL427: return 0;
_LL421:;}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,
int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple24{struct Cyc_List_List*
f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};static struct Cyc_List_List*
Cyc_Tcutil_flatten_typ_f(struct _tuple24*env,struct Cyc_Absyn_Aggrfield*x){struct
Cyc_List_List*_tmp811;struct _RegionHandle*_tmp812;struct Cyc_Tcenv_Tenv*_tmp813;
int _tmp814;struct _tuple24 _tmp810=*env;_tmp811=_tmp810.f1;_tmp812=_tmp810.f2;
_tmp813=_tmp810.f3;_tmp814=_tmp810.f4;{void*_tmp815=Cyc_Tcutil_rsubstitute(
_tmp812,_tmp811,x->type);struct Cyc_List_List*_tmp816=Cyc_Tcutil_flatten_typ(
_tmp812,_tmp814,_tmp813,_tmp815);if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp816)== 1){struct _tuple12*_tmp1310;struct Cyc_List_List*_tmp130F;return(
_tmp130F=_region_malloc(_tmp812,sizeof(*_tmp130F)),((_tmp130F->hd=((_tmp1310=
_region_malloc(_tmp812,sizeof(*_tmp1310)),((_tmp1310->f1=x->tq,((_tmp1310->f2=
_tmp815,_tmp1310)))))),((_tmp130F->tl=0,_tmp130F)))));}else{return _tmp816;}};}
struct _tuple25{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};static
struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple25*env,struct _tuple12*x){
struct _RegionHandle*_tmp81A;struct Cyc_Tcenv_Tenv*_tmp81B;int _tmp81C;struct
_tuple25 _tmp819=*env;_tmp81A=_tmp819.f1;_tmp81B=_tmp819.f2;_tmp81C=_tmp819.f3;{
struct Cyc_Absyn_Tqual _tmp81E;void*_tmp81F;struct _tuple12 _tmp81D=*x;_tmp81E=
_tmp81D.f1;_tmp81F=_tmp81D.f2;{struct Cyc_List_List*_tmp820=Cyc_Tcutil_flatten_typ(
_tmp81A,_tmp81C,_tmp81B,_tmp81F);if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp820)== 1){struct _tuple12*_tmp1313;struct Cyc_List_List*_tmp1312;return(
_tmp1312=_region_malloc(_tmp81A,sizeof(*_tmp1312)),((_tmp1312->hd=((_tmp1313=
_region_malloc(_tmp81A,sizeof(*_tmp1313)),((_tmp1313->f1=_tmp81E,((_tmp1313->f2=
_tmp81F,_tmp1313)))))),((_tmp1312->tl=0,_tmp1312)))));}else{return _tmp820;}};};}
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int
flatten,struct Cyc_Tcenv_Tenv*te,void*t1){if(flatten){t1=Cyc_Tcutil_compress(t1);{
void*_tmp823=t1;struct Cyc_List_List*_tmp826;struct Cyc_Absyn_AggrInfo _tmp828;
union Cyc_Absyn_AggrInfoU _tmp829;struct Cyc_Absyn_Aggrdecl**_tmp82A;struct Cyc_Absyn_Aggrdecl*
_tmp82B;struct Cyc_List_List*_tmp82C;enum Cyc_Absyn_AggrKind _tmp82E;struct Cyc_List_List*
_tmp82F;_LL429: {struct Cyc_Absyn_VoidType_struct*_tmp824=(struct Cyc_Absyn_VoidType_struct*)
_tmp823;if(_tmp824->tag != 0)goto _LL42B;}_LL42A: return 0;_LL42B: {struct Cyc_Absyn_TupleType_struct*
_tmp825=(struct Cyc_Absyn_TupleType_struct*)_tmp823;if(_tmp825->tag != 10)goto
_LL42D;else{_tmp826=_tmp825->f1;}}_LL42C: {struct _tuple25 _tmp1314;struct _tuple25
_tmp830=(_tmp1314.f1=r,((_tmp1314.f2=te,((_tmp1314.f3=flatten,_tmp1314)))));
struct Cyc_List_List*_tmp831=_tmp826;struct Cyc_List_List _tmp832;struct _tuple12*
_tmp833;struct Cyc_List_List*_tmp834;_LL434: if(_tmp831 != 0)goto _LL436;_LL435:
return 0;_LL436: if(_tmp831 == 0)goto _LL433;_tmp832=*_tmp831;_tmp833=(struct
_tuple12*)_tmp832.hd;_tmp834=_tmp832.tl;_LL437: {struct Cyc_List_List*_tmp835=Cyc_Tcutil_rcopy_tqt(&
_tmp830,_tmp833);_tmp830.f3=0;{struct Cyc_List_List*_tmp836=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct _tuple12*),
struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,&
_tmp830,_tmp826);struct Cyc_List_List*_tmp1315;struct Cyc_List_List*_tmp837=(
_tmp1315=_region_malloc(r,sizeof(*_tmp1315)),((_tmp1315->hd=_tmp835,((_tmp1315->tl=
_tmp836,_tmp1315)))));struct Cyc_List_List*_tmp1316;return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp1316=
_region_malloc(r,sizeof(*_tmp1316)),((_tmp1316->hd=_tmp835,((_tmp1316->tl=
_tmp836,_tmp1316)))))));};}_LL433:;}_LL42D: {struct Cyc_Absyn_AggrType_struct*
_tmp827=(struct Cyc_Absyn_AggrType_struct*)_tmp823;if(_tmp827->tag != 11)goto
_LL42F;else{_tmp828=_tmp827->f1;_tmp829=_tmp828.aggr_info;if((_tmp829.KnownAggr).tag
!= 2)goto _LL42F;_tmp82A=(struct Cyc_Absyn_Aggrdecl**)(_tmp829.KnownAggr).val;
_tmp82B=*_tmp82A;_tmp82C=_tmp828.targs;}}_LL42E: if(((_tmp82B->kind == Cyc_Absyn_UnionA
 || _tmp82B->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp82B->impl))->exist_vars
!= 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp82B->impl))->rgn_po != 0){
struct _tuple12*_tmp1319;struct Cyc_List_List*_tmp1318;return(_tmp1318=
_region_malloc(r,sizeof(*_tmp1318)),((_tmp1318->hd=((_tmp1319=_region_malloc(r,
sizeof(*_tmp1319)),((_tmp1319->f1=Cyc_Absyn_empty_tqual(0),((_tmp1319->f2=t1,
_tmp1319)))))),((_tmp1318->tl=0,_tmp1318)))));}{struct Cyc_List_List*_tmp83D=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp82B->tvs,_tmp82C);struct _tuple24
_tmp131A;struct _tuple24 env=(_tmp131A.f1=_tmp83D,((_tmp131A.f2=r,((_tmp131A.f3=te,((
_tmp131A.f4=flatten,_tmp131A)))))));struct Cyc_List_List*_tmp83E=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp82B->impl))->fields;struct Cyc_List_List _tmp83F;struct Cyc_Absyn_Aggrfield*
_tmp840;struct Cyc_List_List*_tmp841;_LL439: if(_tmp83E != 0)goto _LL43B;_LL43A:
return 0;_LL43B: if(_tmp83E == 0)goto _LL438;_tmp83F=*_tmp83E;_tmp840=(struct Cyc_Absyn_Aggrfield*)
_tmp83F.hd;_tmp841=_tmp83F.tl;_LL43C: {struct Cyc_List_List*_tmp842=Cyc_Tcutil_flatten_typ_f(&
env,_tmp840);env.f4=0;{struct Cyc_List_List*_tmp843=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),
struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp841);struct Cyc_List_List*_tmp131B;struct Cyc_List_List*_tmp844=(_tmp131B=
_region_malloc(r,sizeof(*_tmp131B)),((_tmp131B->hd=_tmp842,((_tmp131B->tl=
_tmp843,_tmp131B)))));return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp844);};}_LL438:;};_LL42F: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp82D=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp823;if(_tmp82D->tag != 12)goto _LL431;else{_tmp82E=_tmp82D->f1;if(_tmp82E != 
Cyc_Absyn_StructA)goto _LL431;_tmp82F=_tmp82D->f2;}}_LL430: {struct _tuple24
_tmp131C;struct _tuple24 env=(_tmp131C.f1=0,((_tmp131C.f2=r,((_tmp131C.f3=te,((
_tmp131C.f4=flatten,_tmp131C)))))));struct Cyc_List_List*_tmp847=_tmp82F;struct
Cyc_List_List _tmp848;struct Cyc_Absyn_Aggrfield*_tmp849;struct Cyc_List_List*
_tmp84A;_LL43E: if(_tmp847 != 0)goto _LL440;_LL43F: return 0;_LL440: if(_tmp847 == 0)
goto _LL43D;_tmp848=*_tmp847;_tmp849=(struct Cyc_Absyn_Aggrfield*)_tmp848.hd;
_tmp84A=_tmp848.tl;_LL441: {struct Cyc_List_List*_tmp84B=Cyc_Tcutil_flatten_typ_f(&
env,_tmp849);env.f4=0;{struct Cyc_List_List*_tmp84C=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),
struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp84A);struct Cyc_List_List*_tmp131D;struct Cyc_List_List*_tmp84D=(_tmp131D=
_region_malloc(r,sizeof(*_tmp131D)),((_tmp131D->hd=_tmp84B,((_tmp131D->tl=
_tmp84C,_tmp131D)))));return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp84D);};}_LL43D:;}_LL431:;_LL432:
goto _LL428;_LL428:;};}{struct _tuple12*_tmp1320;struct Cyc_List_List*_tmp131F;
return(_tmp131F=_region_malloc(r,sizeof(*_tmp131F)),((_tmp131F->hd=((_tmp1320=
_region_malloc(r,sizeof(*_tmp1320)),((_tmp1320->f1=Cyc_Absyn_empty_tqual(0),((
_tmp1320->f2=t1,_tmp1320)))))),((_tmp131F->tl=0,_tmp131F)))));};}static int Cyc_Tcutil_sub_attributes(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t
!= 0;t=t->tl){void*_tmp852=(void*)t->hd;_LL443: {struct Cyc_Absyn_Pure_att_struct*
_tmp853=(struct Cyc_Absyn_Pure_att_struct*)_tmp852;if(_tmp853->tag != 22)goto
_LL445;}_LL444: goto _LL446;_LL445: {struct Cyc_Absyn_Noreturn_att_struct*_tmp854=(
struct Cyc_Absyn_Noreturn_att_struct*)_tmp852;if(_tmp854->tag != 4)goto _LL447;}
_LL446: goto _LL448;_LL447: {struct Cyc_Absyn_Initializes_att_struct*_tmp855=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp852;if(_tmp855->tag != 20)goto _LL449;}
_LL448: continue;_LL449:;_LL44A: if(!((int(*)(int(*pred)(void*,void*),void*env,
struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))
return 0;_LL442:;}}for(0;a2 != 0;a2=a2->tl){if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)
a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp1321;struct _tuple0 _tmp857=(_tmp1321.f1=Cyc_Tcutil_compress(t1),((
_tmp1321.f2=Cyc_Tcutil_compress(t2),_tmp1321)));void*_tmp858;enum Cyc_Absyn_Size_of
_tmp85A;void*_tmp85B;enum Cyc_Absyn_Size_of _tmp85D;_LL44C: _tmp858=_tmp857.f1;{
struct Cyc_Absyn_IntType_struct*_tmp859=(struct Cyc_Absyn_IntType_struct*)_tmp858;
if(_tmp859->tag != 6)goto _LL44E;else{_tmp85A=_tmp859->f2;}};_tmp85B=_tmp857.f2;{
struct Cyc_Absyn_IntType_struct*_tmp85C=(struct Cyc_Absyn_IntType_struct*)_tmp85B;
if(_tmp85C->tag != 6)goto _LL44E;else{_tmp85D=_tmp85C->f2;}};_LL44D: return(_tmp85A
== _tmp85D  || _tmp85A == Cyc_Absyn_Int_sz  && _tmp85D == Cyc_Absyn_Long_sz) || 
_tmp85A == Cyc_Absyn_Long_sz  && _tmp85D == Cyc_Absyn_Int_sz;_LL44E:;_LL44F: return 0;
_LL44B:;}static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return 1;{struct Cyc_List_List*
a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1)
 && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))return 1;}}t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp1322;struct _tuple0 _tmp85F=(
_tmp1322.f1=t1,((_tmp1322.f2=t2,_tmp1322)));void*_tmp860;struct Cyc_Absyn_PtrInfo
_tmp862;void*_tmp863;struct Cyc_Absyn_Tqual _tmp864;struct Cyc_Absyn_PtrAtts _tmp865;
void*_tmp866;union Cyc_Absyn_Constraint*_tmp867;union Cyc_Absyn_Constraint*_tmp868;
union Cyc_Absyn_Constraint*_tmp869;void*_tmp86A;struct Cyc_Absyn_PtrInfo _tmp86C;
void*_tmp86D;struct Cyc_Absyn_Tqual _tmp86E;struct Cyc_Absyn_PtrAtts _tmp86F;void*
_tmp870;union Cyc_Absyn_Constraint*_tmp871;union Cyc_Absyn_Constraint*_tmp872;
union Cyc_Absyn_Constraint*_tmp873;void*_tmp874;struct Cyc_Absyn_DatatypeFieldInfo
_tmp876;union Cyc_Absyn_DatatypeFieldInfoU _tmp877;struct _tuple3 _tmp878;struct Cyc_Absyn_Datatypedecl*
_tmp879;struct Cyc_Absyn_Datatypefield*_tmp87A;struct Cyc_List_List*_tmp87B;void*
_tmp87C;struct Cyc_Absyn_DatatypeInfo _tmp87E;union Cyc_Absyn_DatatypeInfoU _tmp87F;
struct Cyc_Absyn_Datatypedecl**_tmp880;struct Cyc_Absyn_Datatypedecl*_tmp881;
struct Cyc_List_List*_tmp882;void*_tmp883;struct Cyc_Absyn_FnInfo _tmp885;void*
_tmp886;struct Cyc_Absyn_FnInfo _tmp888;_LL451: _tmp860=_tmp85F.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp861=(struct Cyc_Absyn_PointerType_struct*)_tmp860;if(_tmp861->tag != 5)goto
_LL453;else{_tmp862=_tmp861->f1;_tmp863=_tmp862.elt_typ;_tmp864=_tmp862.elt_tq;
_tmp865=_tmp862.ptr_atts;_tmp866=_tmp865.rgn;_tmp867=_tmp865.nullable;_tmp868=
_tmp865.bounds;_tmp869=_tmp865.zero_term;}};_tmp86A=_tmp85F.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp86B=(struct Cyc_Absyn_PointerType_struct*)_tmp86A;if(_tmp86B->tag != 5)goto
_LL453;else{_tmp86C=_tmp86B->f1;_tmp86D=_tmp86C.elt_typ;_tmp86E=_tmp86C.elt_tq;
_tmp86F=_tmp86C.ptr_atts;_tmp870=_tmp86F.rgn;_tmp871=_tmp86F.nullable;_tmp872=
_tmp86F.bounds;_tmp873=_tmp86F.zero_term;}};_LL452: if(_tmp864.real_const  && !
_tmp86E.real_const)return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp867,
_tmp871) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp867)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp871))return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,
union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp869,
_tmp873) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp869)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp873))return 0;if(!Cyc_Tcutil_unify(_tmp866,_tmp870) && !Cyc_Tcenv_region_outlives(
te,_tmp866,_tmp870))return 0;if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp868,_tmp872)){struct _tuple0 _tmp1323;struct _tuple0 _tmp88A=(_tmp1323.f1=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp868),((_tmp1323.f2=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp872),_tmp1323)));void*
_tmp88B;void*_tmp88D;void*_tmp88F;struct Cyc_Absyn_Exp*_tmp891;void*_tmp892;
struct Cyc_Absyn_Exp*_tmp894;_LL45A: _tmp88B=_tmp88A.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp88C=(struct Cyc_Absyn_Upper_b_struct*)_tmp88B;if(_tmp88C->tag != 1)goto _LL45C;};
_tmp88D=_tmp88A.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp88E=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp88D;if(_tmp88E->tag != 0)goto _LL45C;};_LL45B: goto _LL459;_LL45C: _tmp88F=
_tmp88A.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp890=(struct Cyc_Absyn_Upper_b_struct*)
_tmp88F;if(_tmp890->tag != 1)goto _LL45E;else{_tmp891=_tmp890->f1;}};_tmp892=
_tmp88A.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp893=(struct Cyc_Absyn_Upper_b_struct*)
_tmp892;if(_tmp893->tag != 1)goto _LL45E;else{_tmp894=_tmp893->f1;}};_LL45D: if(!
Cyc_Evexp_lte_const_exp(_tmp894,_tmp891))return 0;goto _LL459;_LL45E:;_LL45F:
return 0;_LL459:;}if(!_tmp86E.real_const  && _tmp864.real_const){if(!Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp86D)))return 0;}{int _tmp895=((int(*)(int(*
cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_unify_it_bounds,_tmp872,Cyc_Absyn_bounds_one_conref) && !((int(*)(int
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp873);struct _tuple0*
_tmp1326;struct Cyc_List_List*_tmp1325;return(_tmp895  && Cyc_Tcutil_ptrsubtype(te,((
_tmp1325=_cycalloc(sizeof(*_tmp1325)),((_tmp1325->hd=((_tmp1326=_cycalloc(
sizeof(*_tmp1326)),((_tmp1326->f1=t1,((_tmp1326->f2=t2,_tmp1326)))))),((_tmp1325->tl=
assume,_tmp1325)))))),_tmp863,_tmp86D) || Cyc_Tcutil_unify(t1,t2)) || Cyc_Tcutil_isomorphic(
t1,t2);};_LL453: _tmp874=_tmp85F.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp875=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp874;if(_tmp875->tag != 4)
goto _LL455;else{_tmp876=_tmp875->f1;_tmp877=_tmp876.field_info;if((_tmp877.KnownDatatypefield).tag
!= 2)goto _LL455;_tmp878=(struct _tuple3)(_tmp877.KnownDatatypefield).val;_tmp879=
_tmp878.f1;_tmp87A=_tmp878.f2;_tmp87B=_tmp876.targs;}};_tmp87C=_tmp85F.f2;{
struct Cyc_Absyn_DatatypeType_struct*_tmp87D=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp87C;if(_tmp87D->tag != 3)goto _LL455;else{_tmp87E=_tmp87D->f1;_tmp87F=_tmp87E.datatype_info;
if((_tmp87F.KnownDatatype).tag != 2)goto _LL455;_tmp880=(struct Cyc_Absyn_Datatypedecl**)(
_tmp87F.KnownDatatype).val;_tmp881=*_tmp880;_tmp882=_tmp87E.targs;}};_LL454: if(
_tmp879 != _tmp881  && Cyc_Absyn_qvar_cmp(_tmp879->name,_tmp881->name)!= 0)return 0;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp87B)!= ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp882))return 0;for(0;_tmp87B != 0;(_tmp87B=
_tmp87B->tl,_tmp882=_tmp882->tl)){if(!Cyc_Tcutil_unify((void*)_tmp87B->hd,(void*)((
struct Cyc_List_List*)_check_null(_tmp882))->hd))return 0;}return 1;_LL455: _tmp883=
_tmp85F.f1;{struct Cyc_Absyn_FnType_struct*_tmp884=(struct Cyc_Absyn_FnType_struct*)
_tmp883;if(_tmp884->tag != 9)goto _LL457;else{_tmp885=_tmp884->f1;}};_tmp886=
_tmp85F.f2;{struct Cyc_Absyn_FnType_struct*_tmp887=(struct Cyc_Absyn_FnType_struct*)
_tmp886;if(_tmp887->tag != 9)goto _LL457;else{_tmp888=_tmp887->f1;}};_LL456: if(
_tmp885.tvars != 0  || _tmp888.tvars != 0){struct Cyc_List_List*_tmp898=_tmp885.tvars;
struct Cyc_List_List*_tmp899=_tmp888.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp898)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp899))return 0;{
struct _RegionHandle*_tmp89A=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*inst=0;
while(_tmp898 != 0){if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)
_tmp898->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
_tmp899))->hd)->kind))return 0;{struct _tuple16*_tmp1330;struct Cyc_Absyn_VarType_struct
_tmp132F;struct Cyc_Absyn_VarType_struct*_tmp132E;struct Cyc_List_List*_tmp132D;
inst=((_tmp132D=_region_malloc(_tmp89A,sizeof(*_tmp132D)),((_tmp132D->hd=((
_tmp1330=_region_malloc(_tmp89A,sizeof(*_tmp1330)),((_tmp1330->f1=(struct Cyc_Absyn_Tvar*)
_tmp899->hd,((_tmp1330->f2=(void*)((_tmp132E=_cycalloc(sizeof(*_tmp132E)),((
_tmp132E[0]=((_tmp132F.tag=2,((_tmp132F.f1=(struct Cyc_Absyn_Tvar*)_tmp898->hd,
_tmp132F)))),_tmp132E)))),_tmp1330)))))),((_tmp132D->tl=inst,_tmp132D))))));}
_tmp898=_tmp898->tl;_tmp899=_tmp899->tl;}if(inst != 0){_tmp885.tvars=0;_tmp888.tvars=
0;{struct Cyc_Absyn_FnType_struct _tmp1336;struct Cyc_Absyn_FnType_struct*_tmp1335;
struct Cyc_Absyn_FnType_struct _tmp1333;struct Cyc_Absyn_FnType_struct*_tmp1332;
return Cyc_Tcutil_subtype(te,assume,(void*)((_tmp1332=_cycalloc(sizeof(*_tmp1332)),((
_tmp1332[0]=((_tmp1333.tag=9,((_tmp1333.f1=_tmp885,_tmp1333)))),_tmp1332)))),(
void*)((_tmp1335=_cycalloc(sizeof(*_tmp1335)),((_tmp1335[0]=((_tmp1336.tag=9,((
_tmp1336.f1=_tmp888,_tmp1336)))),_tmp1335)))));};}};}if(!Cyc_Tcutil_subtype(te,
assume,_tmp885.ret_typ,_tmp888.ret_typ))return 0;{struct Cyc_List_List*_tmp8A3=
_tmp885.args;struct Cyc_List_List*_tmp8A4=_tmp888.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp8A3)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp8A4))return 0;for(0;_tmp8A3 != 0;(_tmp8A3=_tmp8A3->tl,_tmp8A4=_tmp8A4->tl)){
struct Cyc_Absyn_Tqual _tmp8A6;void*_tmp8A7;struct _tuple10 _tmp8A5=*((struct
_tuple10*)_tmp8A3->hd);_tmp8A6=_tmp8A5.f2;_tmp8A7=_tmp8A5.f3;{struct Cyc_Absyn_Tqual
_tmp8A9;void*_tmp8AA;struct _tuple10 _tmp8A8=*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp8A4))->hd);_tmp8A9=_tmp8A8.f2;_tmp8AA=_tmp8A8.f3;if(_tmp8A9.real_const
 && !_tmp8A6.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp8AA,_tmp8A7))return
0;};}if(_tmp885.c_varargs != _tmp888.c_varargs)return 0;if(_tmp885.cyc_varargs != 0
 && _tmp888.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp8AB=*_tmp885.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp8AC=*_tmp888.cyc_varargs;if((_tmp8AC.tq).real_const
 && !(_tmp8AB.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp8AC.type,
_tmp8AB.type))return 0;}else{if(_tmp885.cyc_varargs != 0  || _tmp888.cyc_varargs != 
0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp885.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp888.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp885.rgn_po,_tmp888.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp885.attributes,_tmp888.attributes))return 0;return 1;};_LL457:;_LL458: return 0;
_LL450:;};}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2){struct _RegionHandle*_tmp8AD=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(_tmp8AD,1,te,t1);struct Cyc_List_List*
tqs2=Cyc_Tcutil_flatten_typ(_tmp8AD,1,te,t2);for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=
tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple12 _tmp8AF;struct Cyc_Absyn_Tqual
_tmp8B0;void*_tmp8B1;struct _tuple12*_tmp8AE=(struct _tuple12*)tqs1->hd;_tmp8AF=*
_tmp8AE;_tmp8B0=_tmp8AF.f1;_tmp8B1=_tmp8AF.f2;{struct _tuple12 _tmp8B3;struct Cyc_Absyn_Tqual
_tmp8B4;void*_tmp8B5;struct _tuple12*_tmp8B2=(struct _tuple12*)tqs2->hd;_tmp8B3=*
_tmp8B2;_tmp8B4=_tmp8B3.f1;_tmp8B5=_tmp8B3.f2;if(_tmp8B0.real_const  && !_tmp8B4.real_const)
return 0;if((_tmp8B4.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(
_tmp8B5))) && Cyc_Tcutil_subtype(te,assume,_tmp8B1,_tmp8B5))continue;if(Cyc_Tcutil_unify(
_tmp8B1,_tmp8B5))continue;if(Cyc_Tcutil_isomorphic(_tmp8B1,_tmp8B5))continue;
return 0;};};}return 1;}static int Cyc_Tcutil_is_char_type(void*t){void*_tmp8B6=Cyc_Tcutil_compress(
t);enum Cyc_Absyn_Size_of _tmp8B8;_LL461: {struct Cyc_Absyn_IntType_struct*_tmp8B7=(
struct Cyc_Absyn_IntType_struct*)_tmp8B6;if(_tmp8B7->tag != 6)goto _LL463;else{
_tmp8B8=_tmp8B7->f2;if(_tmp8B8 != Cyc_Absyn_Char_sz)goto _LL463;}}_LL462: return 1;
_LL463:;_LL464: return 0;_LL460:;}enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return Cyc_Absyn_No_coercion;t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t2 == (void*)& Cyc_Absyn_VoidType_val)return Cyc_Absyn_No_coercion;{void*
_tmp8B9=t2;enum Cyc_Absyn_Size_of _tmp8BB;enum Cyc_Absyn_Size_of _tmp8BD;_LL466: {
struct Cyc_Absyn_IntType_struct*_tmp8BA=(struct Cyc_Absyn_IntType_struct*)_tmp8B9;
if(_tmp8BA->tag != 6)goto _LL468;else{_tmp8BB=_tmp8BA->f2;if(_tmp8BB != Cyc_Absyn_Int_sz)
goto _LL468;}}_LL467: goto _LL469;_LL468: {struct Cyc_Absyn_IntType_struct*_tmp8BC=(
struct Cyc_Absyn_IntType_struct*)_tmp8B9;if(_tmp8BC->tag != 6)goto _LL46A;else{
_tmp8BD=_tmp8BC->f2;if(_tmp8BD != Cyc_Absyn_Long_sz)goto _LL46A;}}_LL469: if((Cyc_Tcutil_typ_kind(
t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_No_coercion;goto _LL465;_LL46A:;
_LL46B: goto _LL465;_LL465:;}{void*_tmp8BE=t1;struct Cyc_Absyn_PtrInfo _tmp8C0;void*
_tmp8C1;struct Cyc_Absyn_Tqual _tmp8C2;struct Cyc_Absyn_PtrAtts _tmp8C3;void*_tmp8C4;
union Cyc_Absyn_Constraint*_tmp8C5;union Cyc_Absyn_Constraint*_tmp8C6;union Cyc_Absyn_Constraint*
_tmp8C7;struct Cyc_Absyn_ArrayInfo _tmp8C9;void*_tmp8CA;struct Cyc_Absyn_Tqual
_tmp8CB;struct Cyc_Absyn_Exp*_tmp8CC;union Cyc_Absyn_Constraint*_tmp8CD;struct Cyc_Absyn_Enumdecl*
_tmp8CF;void*_tmp8D3;_LL46D: {struct Cyc_Absyn_PointerType_struct*_tmp8BF=(struct
Cyc_Absyn_PointerType_struct*)_tmp8BE;if(_tmp8BF->tag != 5)goto _LL46F;else{
_tmp8C0=_tmp8BF->f1;_tmp8C1=_tmp8C0.elt_typ;_tmp8C2=_tmp8C0.elt_tq;_tmp8C3=
_tmp8C0.ptr_atts;_tmp8C4=_tmp8C3.rgn;_tmp8C5=_tmp8C3.nullable;_tmp8C6=_tmp8C3.bounds;
_tmp8C7=_tmp8C3.zero_term;}}_LL46E:{void*_tmp8D4=t2;struct Cyc_Absyn_PtrInfo
_tmp8D6;void*_tmp8D7;struct Cyc_Absyn_Tqual _tmp8D8;struct Cyc_Absyn_PtrAtts _tmp8D9;
void*_tmp8DA;union Cyc_Absyn_Constraint*_tmp8DB;union Cyc_Absyn_Constraint*_tmp8DC;
union Cyc_Absyn_Constraint*_tmp8DD;_LL47C: {struct Cyc_Absyn_PointerType_struct*
_tmp8D5=(struct Cyc_Absyn_PointerType_struct*)_tmp8D4;if(_tmp8D5->tag != 5)goto
_LL47E;else{_tmp8D6=_tmp8D5->f1;_tmp8D7=_tmp8D6.elt_typ;_tmp8D8=_tmp8D6.elt_tq;
_tmp8D9=_tmp8D6.ptr_atts;_tmp8DA=_tmp8D9.rgn;_tmp8DB=_tmp8D9.nullable;_tmp8DC=
_tmp8D9.bounds;_tmp8DD=_tmp8D9.zero_term;}}_LL47D: {enum Cyc_Absyn_Coercion
coercion=Cyc_Absyn_Other_coercion;struct _tuple0*_tmp1339;struct Cyc_List_List*
_tmp1338;struct Cyc_List_List*_tmp8DE=(_tmp1338=_cycalloc(sizeof(*_tmp1338)),((
_tmp1338->hd=((_tmp1339=_cycalloc(sizeof(*_tmp1339)),((_tmp1339->f1=t1,((
_tmp1339->f2=t2,_tmp1339)))))),((_tmp1338->tl=0,_tmp1338)))));int _tmp8DF=_tmp8D8.real_const
 || !_tmp8C2.real_const;int _tmp8E0=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp8DC,Cyc_Absyn_bounds_one_conref) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(0,_tmp8DD);int _tmp8E1=_tmp8DF  && (((_tmp8E0  && Cyc_Tcutil_ptrsubtype(
te,_tmp8DE,_tmp8C1,_tmp8D7) || Cyc_Tcutil_unify(_tmp8C1,_tmp8D7)) || Cyc_Tcutil_isomorphic(
_tmp8C1,_tmp8D7)) || Cyc_Tcutil_unify(_tmp8D7,(void*)& Cyc_Absyn_VoidType_val));
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp8C7,_tmp8DD) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp8DD);int _tmp8E2=_tmp8E1?0:((Cyc_Tcutil_bits_only(_tmp8C1) && Cyc_Tcutil_is_char_type(
_tmp8D7)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp8DD)) && (_tmp8D8.real_const  || !_tmp8C2.real_const);int bounds_ok=((int(*)(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))
Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp8C6,_tmp8DC);if(!
bounds_ok  && !_tmp8E2){struct _tuple0 _tmp133A;struct _tuple0 _tmp8E4=(_tmp133A.f1=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8C6),((_tmp133A.f2=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8DC),_tmp133A)));
void*_tmp8E5;struct Cyc_Absyn_Exp*_tmp8E7;void*_tmp8E8;struct Cyc_Absyn_Exp*
_tmp8EA;_LL481: _tmp8E5=_tmp8E4.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp8E6=(
struct Cyc_Absyn_Upper_b_struct*)_tmp8E5;if(_tmp8E6->tag != 1)goto _LL483;else{
_tmp8E7=_tmp8E6->f1;}};_tmp8E8=_tmp8E4.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp8E9=(struct Cyc_Absyn_Upper_b_struct*)_tmp8E8;if(_tmp8E9->tag != 1)goto _LL483;
else{_tmp8EA=_tmp8E9->f1;}};_LL482: if(Cyc_Evexp_lte_const_exp(_tmp8EA,_tmp8E7))
bounds_ok=1;goto _LL480;_LL483:;_LL484: bounds_ok=1;goto _LL480;_LL480:;}if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8C5) && !((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8DB))coercion=Cyc_Absyn_NonNull_to_Null;
if(((bounds_ok  && zeroterm_ok) && (_tmp8E1  || _tmp8E2)) && (Cyc_Tcutil_unify(
_tmp8C4,_tmp8DA) || Cyc_Tcenv_region_outlives(te,_tmp8C4,_tmp8DA)))return
coercion;else{return Cyc_Absyn_Unknown_coercion;}};}_LL47E:;_LL47F: goto _LL47B;
_LL47B:;}return Cyc_Absyn_Unknown_coercion;_LL46F: {struct Cyc_Absyn_ArrayType_struct*
_tmp8C8=(struct Cyc_Absyn_ArrayType_struct*)_tmp8BE;if(_tmp8C8->tag != 8)goto
_LL471;else{_tmp8C9=_tmp8C8->f1;_tmp8CA=_tmp8C9.elt_type;_tmp8CB=_tmp8C9.tq;
_tmp8CC=_tmp8C9.num_elts;_tmp8CD=_tmp8C9.zero_term;}}_LL470:{void*_tmp8ED=t2;
struct Cyc_Absyn_ArrayInfo _tmp8EF;void*_tmp8F0;struct Cyc_Absyn_Tqual _tmp8F1;
struct Cyc_Absyn_Exp*_tmp8F2;union Cyc_Absyn_Constraint*_tmp8F3;_LL486: {struct Cyc_Absyn_ArrayType_struct*
_tmp8EE=(struct Cyc_Absyn_ArrayType_struct*)_tmp8ED;if(_tmp8EE->tag != 8)goto
_LL488;else{_tmp8EF=_tmp8EE->f1;_tmp8F0=_tmp8EF.elt_type;_tmp8F1=_tmp8EF.tq;
_tmp8F2=_tmp8EF.num_elts;_tmp8F3=_tmp8EF.zero_term;}}_LL487: {int okay;okay=((
_tmp8CC != 0  && _tmp8F2 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8CD,
_tmp8F3)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp8F2,(struct Cyc_Absyn_Exp*)
_tmp8CC);return(okay  && Cyc_Tcutil_unify(_tmp8CA,_tmp8F0)) && (!_tmp8CB.real_const
 || _tmp8F1.real_const)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;}
_LL488:;_LL489: return Cyc_Absyn_Unknown_coercion;_LL485:;}return Cyc_Absyn_Unknown_coercion;
_LL471: {struct Cyc_Absyn_EnumType_struct*_tmp8CE=(struct Cyc_Absyn_EnumType_struct*)
_tmp8BE;if(_tmp8CE->tag != 13)goto _LL473;else{_tmp8CF=_tmp8CE->f2;}}_LL472:{void*
_tmp8F4=t2;struct Cyc_Absyn_Enumdecl*_tmp8F6;_LL48B: {struct Cyc_Absyn_EnumType_struct*
_tmp8F5=(struct Cyc_Absyn_EnumType_struct*)_tmp8F4;if(_tmp8F5->tag != 13)goto
_LL48D;else{_tmp8F6=_tmp8F5->f2;}}_LL48C: if((_tmp8CF->fields != 0  && _tmp8F6->fields
!= 0) && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp8CF->fields))->v)>= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp8F6->fields))->v))return Cyc_Absyn_No_coercion;goto _LL48A;_LL48D:;_LL48E: goto
_LL48A;_LL48A:;}goto _LL474;_LL473: {struct Cyc_Absyn_IntType_struct*_tmp8D0=(
struct Cyc_Absyn_IntType_struct*)_tmp8BE;if(_tmp8D0->tag != 6)goto _LL475;}_LL474:
goto _LL476;_LL475: {struct Cyc_Absyn_FloatType_struct*_tmp8D1=(struct Cyc_Absyn_FloatType_struct*)
_tmp8BE;if(_tmp8D1->tag != 7)goto _LL477;}_LL476: return Cyc_Tcutil_coerceable(t2)?
Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;_LL477: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp8D2=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp8BE;if(_tmp8D2->tag != 15)goto
_LL479;else{_tmp8D3=(void*)_tmp8D2->f1;}}_LL478:{void*_tmp8F7=t2;void*_tmp8F9;
_LL490: {struct Cyc_Absyn_RgnHandleType_struct*_tmp8F8=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp8F7;if(_tmp8F8->tag != 15)goto _LL492;else{_tmp8F9=(void*)_tmp8F8->f1;}}_LL491:
if(Cyc_Tcenv_region_outlives(te,_tmp8D3,_tmp8F9))return Cyc_Absyn_No_coercion;
goto _LL48F;_LL492:;_LL493: goto _LL48F;_LL48F:;}return Cyc_Absyn_Unknown_coercion;
_LL479:;_LL47A: return Cyc_Absyn_Unknown_coercion;_LL46C:;};}void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,t)){
struct Cyc_Absyn_Exp*_tmp8FA=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Cast_e_struct
_tmp133D;struct Cyc_Absyn_Cast_e_struct*_tmp133C;e->r=(void*)((_tmp133C=_cycalloc(
sizeof(*_tmp133C)),((_tmp133C[0]=((_tmp133D.tag=15,((_tmp133D.f1=(void*)t,((
_tmp133D.f2=_tmp8FA,((_tmp133D.f3=0,((_tmp133D.f4=c,_tmp133D)))))))))),_tmp133C))));}{
struct Cyc_Core_Opt*_tmp133E;e->topt=((_tmp133E=_cycalloc(sizeof(*_tmp133E)),((
_tmp133E->v=t,_tmp133E))));};}}int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp8FE=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL495: {struct Cyc_Absyn_IntType_struct*_tmp8FF=(struct Cyc_Absyn_IntType_struct*)
_tmp8FE;if(_tmp8FF->tag != 6)goto _LL497;}_LL496: goto _LL498;_LL497: {struct Cyc_Absyn_EnumType_struct*
_tmp900=(struct Cyc_Absyn_EnumType_struct*)_tmp8FE;if(_tmp900->tag != 13)goto
_LL499;}_LL498: goto _LL49A;_LL499: {struct Cyc_Absyn_AnonEnumType_struct*_tmp901=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp8FE;if(_tmp901->tag != 14)goto _LL49B;}
_LL49A: goto _LL49C;_LL49B: {struct Cyc_Absyn_TagType_struct*_tmp902=(struct Cyc_Absyn_TagType_struct*)
_tmp8FE;if(_tmp902->tag != 19)goto _LL49D;}_LL49C: return 1;_LL49D: {struct Cyc_Absyn_Evar_struct*
_tmp903=(struct Cyc_Absyn_Evar_struct*)_tmp8FE;if(_tmp903->tag != 1)goto _LL49F;}
_LL49E: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ);_LL49F:;_LL4A0: return 0;_LL494:;}int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(e))return 1;{void*_tmp904=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL4A2: {struct Cyc_Absyn_FloatType_struct*
_tmp905=(struct Cyc_Absyn_FloatType_struct*)_tmp904;if(_tmp905->tag != 7)goto
_LL4A4;}_LL4A3: return 1;_LL4A4:;_LL4A5: return 0;_LL4A1:;};}int Cyc_Tcutil_is_function_type(
void*t){void*_tmp906=Cyc_Tcutil_compress(t);_LL4A7: {struct Cyc_Absyn_FnType_struct*
_tmp907=(struct Cyc_Absyn_FnType_struct*)_tmp906;if(_tmp907->tag != 9)goto _LL4A9;}
_LL4A8: return 1;_LL4A9:;_LL4AA: return 0;_LL4A6:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2){struct _tuple0 _tmp133F;struct _tuple0 _tmp909=(_tmp133F.f1=t1,((
_tmp133F.f2=t2,_tmp133F)));void*_tmp90A;int _tmp90C;void*_tmp90D;int _tmp90F;void*
_tmp910;void*_tmp912;void*_tmp914;enum Cyc_Absyn_Sign _tmp916;enum Cyc_Absyn_Size_of
_tmp917;void*_tmp918;enum Cyc_Absyn_Sign _tmp91A;enum Cyc_Absyn_Size_of _tmp91B;
void*_tmp91C;enum Cyc_Absyn_Size_of _tmp91E;void*_tmp91F;enum Cyc_Absyn_Size_of
_tmp921;void*_tmp922;enum Cyc_Absyn_Sign _tmp924;enum Cyc_Absyn_Size_of _tmp925;
void*_tmp926;enum Cyc_Absyn_Sign _tmp928;enum Cyc_Absyn_Size_of _tmp929;void*
_tmp92A;enum Cyc_Absyn_Sign _tmp92C;enum Cyc_Absyn_Size_of _tmp92D;void*_tmp92E;
enum Cyc_Absyn_Sign _tmp930;enum Cyc_Absyn_Size_of _tmp931;void*_tmp932;enum Cyc_Absyn_Size_of
_tmp934;void*_tmp935;enum Cyc_Absyn_Size_of _tmp937;_LL4AC: _tmp90A=_tmp909.f1;{
struct Cyc_Absyn_FloatType_struct*_tmp90B=(struct Cyc_Absyn_FloatType_struct*)
_tmp90A;if(_tmp90B->tag != 7)goto _LL4AE;else{_tmp90C=_tmp90B->f1;}};_tmp90D=
_tmp909.f2;{struct Cyc_Absyn_FloatType_struct*_tmp90E=(struct Cyc_Absyn_FloatType_struct*)
_tmp90D;if(_tmp90E->tag != 7)goto _LL4AE;else{_tmp90F=_tmp90E->f1;}};_LL4AD: if(
_tmp90C != 0  && _tmp90C != 1)return t1;else{if(_tmp90F != 0  && _tmp90F != 1)return t2;
else{if(_tmp90C >= _tmp90F)return t1;else{return t2;}}}_LL4AE: _tmp910=_tmp909.f1;{
struct Cyc_Absyn_FloatType_struct*_tmp911=(struct Cyc_Absyn_FloatType_struct*)
_tmp910;if(_tmp911->tag != 7)goto _LL4B0;};_LL4AF: return t1;_LL4B0: _tmp912=_tmp909.f2;{
struct Cyc_Absyn_FloatType_struct*_tmp913=(struct Cyc_Absyn_FloatType_struct*)
_tmp912;if(_tmp913->tag != 7)goto _LL4B2;};_LL4B1: return t2;_LL4B2: _tmp914=_tmp909.f1;{
struct Cyc_Absyn_IntType_struct*_tmp915=(struct Cyc_Absyn_IntType_struct*)_tmp914;
if(_tmp915->tag != 6)goto _LL4B4;else{_tmp916=_tmp915->f1;if(_tmp916 != Cyc_Absyn_Unsigned)
goto _LL4B4;_tmp917=_tmp915->f2;if(_tmp917 != Cyc_Absyn_LongLong_sz)goto _LL4B4;}};
_LL4B3: goto _LL4B5;_LL4B4: _tmp918=_tmp909.f2;{struct Cyc_Absyn_IntType_struct*
_tmp919=(struct Cyc_Absyn_IntType_struct*)_tmp918;if(_tmp919->tag != 6)goto _LL4B6;
else{_tmp91A=_tmp919->f1;if(_tmp91A != Cyc_Absyn_Unsigned)goto _LL4B6;_tmp91B=
_tmp919->f2;if(_tmp91B != Cyc_Absyn_LongLong_sz)goto _LL4B6;}};_LL4B5: return Cyc_Absyn_ulonglong_typ;
_LL4B6: _tmp91C=_tmp909.f1;{struct Cyc_Absyn_IntType_struct*_tmp91D=(struct Cyc_Absyn_IntType_struct*)
_tmp91C;if(_tmp91D->tag != 6)goto _LL4B8;else{_tmp91E=_tmp91D->f2;if(_tmp91E != Cyc_Absyn_LongLong_sz)
goto _LL4B8;}};_LL4B7: goto _LL4B9;_LL4B8: _tmp91F=_tmp909.f2;{struct Cyc_Absyn_IntType_struct*
_tmp920=(struct Cyc_Absyn_IntType_struct*)_tmp91F;if(_tmp920->tag != 6)goto _LL4BA;
else{_tmp921=_tmp920->f2;if(_tmp921 != Cyc_Absyn_LongLong_sz)goto _LL4BA;}};_LL4B9:
return Cyc_Absyn_slonglong_typ;_LL4BA: _tmp922=_tmp909.f1;{struct Cyc_Absyn_IntType_struct*
_tmp923=(struct Cyc_Absyn_IntType_struct*)_tmp922;if(_tmp923->tag != 6)goto _LL4BC;
else{_tmp924=_tmp923->f1;if(_tmp924 != Cyc_Absyn_Unsigned)goto _LL4BC;_tmp925=
_tmp923->f2;if(_tmp925 != Cyc_Absyn_Long_sz)goto _LL4BC;}};_LL4BB: goto _LL4BD;
_LL4BC: _tmp926=_tmp909.f2;{struct Cyc_Absyn_IntType_struct*_tmp927=(struct Cyc_Absyn_IntType_struct*)
_tmp926;if(_tmp927->tag != 6)goto _LL4BE;else{_tmp928=_tmp927->f1;if(_tmp928 != Cyc_Absyn_Unsigned)
goto _LL4BE;_tmp929=_tmp927->f2;if(_tmp929 != Cyc_Absyn_Long_sz)goto _LL4BE;}};
_LL4BD: return Cyc_Absyn_ulong_typ;_LL4BE: _tmp92A=_tmp909.f1;{struct Cyc_Absyn_IntType_struct*
_tmp92B=(struct Cyc_Absyn_IntType_struct*)_tmp92A;if(_tmp92B->tag != 6)goto _LL4C0;
else{_tmp92C=_tmp92B->f1;if(_tmp92C != Cyc_Absyn_Unsigned)goto _LL4C0;_tmp92D=
_tmp92B->f2;if(_tmp92D != Cyc_Absyn_Int_sz)goto _LL4C0;}};_LL4BF: goto _LL4C1;_LL4C0:
_tmp92E=_tmp909.f2;{struct Cyc_Absyn_IntType_struct*_tmp92F=(struct Cyc_Absyn_IntType_struct*)
_tmp92E;if(_tmp92F->tag != 6)goto _LL4C2;else{_tmp930=_tmp92F->f1;if(_tmp930 != Cyc_Absyn_Unsigned)
goto _LL4C2;_tmp931=_tmp92F->f2;if(_tmp931 != Cyc_Absyn_Int_sz)goto _LL4C2;}};
_LL4C1: return Cyc_Absyn_uint_typ;_LL4C2: _tmp932=_tmp909.f1;{struct Cyc_Absyn_IntType_struct*
_tmp933=(struct Cyc_Absyn_IntType_struct*)_tmp932;if(_tmp933->tag != 6)goto _LL4C4;
else{_tmp934=_tmp933->f2;if(_tmp934 != Cyc_Absyn_Long_sz)goto _LL4C4;}};_LL4C3:
goto _LL4C5;_LL4C4: _tmp935=_tmp909.f2;{struct Cyc_Absyn_IntType_struct*_tmp936=(
struct Cyc_Absyn_IntType_struct*)_tmp935;if(_tmp936->tag != 6)goto _LL4C6;else{
_tmp937=_tmp936->f2;if(_tmp937 != Cyc_Absyn_Long_sz)goto _LL4C6;}};_LL4C5: return
Cyc_Absyn_slong_typ;_LL4C6:;_LL4C7: return Cyc_Absyn_sint_typ;_LL4AB:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp938=e->r;struct Cyc_Core_Opt*_tmp93A;_LL4C9: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp939=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp938;if(_tmp939->tag != 4)goto _LL4CB;else{_tmp93A=_tmp939->f2;if(_tmp93A != 0)
goto _LL4CB;}}_LL4CA:{const char*_tmp1342;void*_tmp1341;(_tmp1341=0,Cyc_Tcutil_warn(
e->loc,((_tmp1342="assignment in test",_tag_dyneither(_tmp1342,sizeof(char),19))),
_tag_dyneither(_tmp1341,sizeof(void*),0)));}goto _LL4C8;_LL4CB:;_LL4CC: goto _LL4C8;
_LL4C8:;}static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){c1=Cyc_Absyn_compress_kb(
c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple0 _tmp1343;struct _tuple0 _tmp93E=(
_tmp1343.f1=c1,((_tmp1343.f2=c2,_tmp1343)));void*_tmp93F;struct Cyc_Absyn_Kind*
_tmp941;void*_tmp942;struct Cyc_Absyn_Kind*_tmp944;void*_tmp945;struct Cyc_Core_Opt*
_tmp947;struct Cyc_Core_Opt**_tmp948;void*_tmp949;struct Cyc_Core_Opt*_tmp94B;
struct Cyc_Core_Opt**_tmp94C;void*_tmp94D;struct Cyc_Core_Opt*_tmp94F;struct Cyc_Core_Opt**
_tmp950;struct Cyc_Absyn_Kind*_tmp951;void*_tmp952;struct Cyc_Absyn_Kind*_tmp954;
void*_tmp955;struct Cyc_Absyn_Kind*_tmp957;void*_tmp958;struct Cyc_Core_Opt*
_tmp95A;struct Cyc_Core_Opt**_tmp95B;struct Cyc_Absyn_Kind*_tmp95C;void*_tmp95D;
struct Cyc_Core_Opt*_tmp95F;struct Cyc_Core_Opt**_tmp960;struct Cyc_Absyn_Kind*
_tmp961;void*_tmp962;struct Cyc_Core_Opt*_tmp964;struct Cyc_Core_Opt**_tmp965;
struct Cyc_Absyn_Kind*_tmp966;_LL4CE: _tmp93F=_tmp93E.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp940=(struct Cyc_Absyn_Eq_kb_struct*)_tmp93F;if(_tmp940->tag != 0)goto _LL4D0;
else{_tmp941=_tmp940->f1;}};_tmp942=_tmp93E.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp943=(struct Cyc_Absyn_Eq_kb_struct*)_tmp942;if(_tmp943->tag != 0)goto _LL4D0;
else{_tmp944=_tmp943->f1;}};_LL4CF: return _tmp941 == _tmp944;_LL4D0: _tmp945=
_tmp93E.f2;{struct Cyc_Absyn_Unknown_kb_struct*_tmp946=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp945;if(_tmp946->tag != 1)goto _LL4D2;else{_tmp947=_tmp946->f1;_tmp948=(struct
Cyc_Core_Opt**)& _tmp946->f1;}};_LL4D1:{struct Cyc_Core_Opt*_tmp1344;*_tmp948=((
_tmp1344=_cycalloc(sizeof(*_tmp1344)),((_tmp1344->v=c1,_tmp1344))));}return 1;
_LL4D2: _tmp949=_tmp93E.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmp94A=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp949;if(_tmp94A->tag != 1)goto _LL4D4;else{_tmp94B=_tmp94A->f1;_tmp94C=(struct
Cyc_Core_Opt**)& _tmp94A->f1;}};_LL4D3:{struct Cyc_Core_Opt*_tmp1345;*_tmp94C=((
_tmp1345=_cycalloc(sizeof(*_tmp1345)),((_tmp1345->v=c2,_tmp1345))));}return 1;
_LL4D4: _tmp94D=_tmp93E.f1;{struct Cyc_Absyn_Less_kb_struct*_tmp94E=(struct Cyc_Absyn_Less_kb_struct*)
_tmp94D;if(_tmp94E->tag != 2)goto _LL4D6;else{_tmp94F=_tmp94E->f1;_tmp950=(struct
Cyc_Core_Opt**)& _tmp94E->f1;_tmp951=_tmp94E->f2;}};_tmp952=_tmp93E.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp953=(struct Cyc_Absyn_Eq_kb_struct*)_tmp952;if(_tmp953->tag != 0)goto _LL4D6;
else{_tmp954=_tmp953->f1;}};_LL4D5: if(Cyc_Tcutil_kind_leq(_tmp954,_tmp951)){{
struct Cyc_Core_Opt*_tmp1346;*_tmp950=((_tmp1346=_cycalloc(sizeof(*_tmp1346)),((
_tmp1346->v=c2,_tmp1346))));}return 1;}else{return 0;}_LL4D6: _tmp955=_tmp93E.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp956=(struct Cyc_Absyn_Eq_kb_struct*)_tmp955;if(
_tmp956->tag != 0)goto _LL4D8;else{_tmp957=_tmp956->f1;}};_tmp958=_tmp93E.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp959=(struct Cyc_Absyn_Less_kb_struct*)_tmp958;
if(_tmp959->tag != 2)goto _LL4D8;else{_tmp95A=_tmp959->f1;_tmp95B=(struct Cyc_Core_Opt**)&
_tmp959->f1;_tmp95C=_tmp959->f2;}};_LL4D7: if(Cyc_Tcutil_kind_leq(_tmp957,_tmp95C)){{
struct Cyc_Core_Opt*_tmp1347;*_tmp95B=((_tmp1347=_cycalloc(sizeof(*_tmp1347)),((
_tmp1347->v=c1,_tmp1347))));}return 1;}else{return 0;}_LL4D8: _tmp95D=_tmp93E.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp95E=(struct Cyc_Absyn_Less_kb_struct*)_tmp95D;
if(_tmp95E->tag != 2)goto _LL4CD;else{_tmp95F=_tmp95E->f1;_tmp960=(struct Cyc_Core_Opt**)&
_tmp95E->f1;_tmp961=_tmp95E->f2;}};_tmp962=_tmp93E.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp963=(struct Cyc_Absyn_Less_kb_struct*)_tmp962;if(_tmp963->tag != 2)goto _LL4CD;
else{_tmp964=_tmp963->f1;_tmp965=(struct Cyc_Core_Opt**)& _tmp963->f1;_tmp966=
_tmp963->f2;}};_LL4D9: if(Cyc_Tcutil_kind_leq(_tmp961,_tmp966)){{struct Cyc_Core_Opt*
_tmp1348;*_tmp965=((_tmp1348=_cycalloc(sizeof(*_tmp1348)),((_tmp1348->v=c1,
_tmp1348))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp966,_tmp961)){{struct Cyc_Core_Opt*
_tmp1349;*_tmp960=((_tmp1349=_cycalloc(sizeof(*_tmp1349)),((_tmp1349->v=c2,
_tmp1349))));}return 1;}else{return 0;}}_LL4CD:;};}static int Cyc_Tcutil_tvar_id_counter=
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
t){struct _dyneither_ptr _tmp972=*t->name;return*((const char*)
_check_dyneither_subscript(_tmp972,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){{const char*_tmp1354;void*_tmp1353[1];struct Cyc_String_pa_struct
_tmp1352;(_tmp1352.tag=0,((_tmp1352.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*t->name),((_tmp1353[0]=& _tmp1352,Cyc_printf(((_tmp1354="%s",
_tag_dyneither(_tmp1354,sizeof(char),3))),_tag_dyneither(_tmp1353,sizeof(void*),
1)))))));}if(!Cyc_Tcutil_is_temp_tvar(t))return;{const char*_tmp1355;struct
_dyneither_ptr _tmp976=Cyc_strconcat(((_tmp1355="`",_tag_dyneither(_tmp1355,
sizeof(char),2))),(struct _dyneither_ptr)*t->name);{char _tmp1358;char _tmp1357;
struct _dyneither_ptr _tmp1356;(_tmp1356=_dyneither_ptr_plus(_tmp976,sizeof(char),
1),((_tmp1357=*((char*)_check_dyneither_subscript(_tmp1356,sizeof(char),0)),((
_tmp1358='t',((_get_dyneither_size(_tmp1356,sizeof(char))== 1  && (_tmp1357 == '\000'
 && _tmp1358 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp1356.curr)=_tmp1358)))))));}{
struct _dyneither_ptr*_tmp1359;t->name=((_tmp1359=_cycalloc(sizeof(struct
_dyneither_ptr)* 1),((_tmp1359[0]=(struct _dyneither_ptr)_tmp976,_tmp1359))));};};}
struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static
struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple26*x){struct Cyc_Core_Opt*
_tmp135C;struct _tuple10*_tmp135B;return(_tmp135B=_cycalloc(sizeof(*_tmp135B)),((
_tmp135B->f1=(struct Cyc_Core_Opt*)((_tmp135C=_cycalloc(sizeof(*_tmp135C)),((
_tmp135C->v=(*x).f1,_tmp135C)))),((_tmp135B->f2=(*x).f2,((_tmp135B->f3=(*x).f3,
_tmp135B)))))));}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp97E=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){struct
Cyc_List_List*_tmp135D;_tmp97E=((_tmp135D=_cycalloc(sizeof(*_tmp135D)),((
_tmp135D->hd=(void*)atts->hd,((_tmp135D->tl=_tmp97E,_tmp135D))))));}}}{struct Cyc_Absyn_FnType_struct
_tmp1363;struct Cyc_Absyn_FnInfo _tmp1362;struct Cyc_Absyn_FnType_struct*_tmp1361;
return(void*)((_tmp1361=_cycalloc(sizeof(*_tmp1361)),((_tmp1361[0]=((_tmp1363.tag=
9,((_tmp1363.f1=((_tmp1362.tvars=fd->tvs,((_tmp1362.effect=fd->effect,((_tmp1362.ret_tqual=
fd->ret_tqual,((_tmp1362.ret_typ=fd->ret_type,((_tmp1362.args=((struct Cyc_List_List*(*)(
struct _tuple10*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args),((_tmp1362.c_varargs=fd->c_varargs,((_tmp1362.cyc_varargs=fd->cyc_varargs,((
_tmp1362.rgn_po=fd->rgn_po,((_tmp1362.attributes=_tmp97E,_tmp1362)))))))))))))))))),
_tmp1363)))),_tmp1361))));};}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple27{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple27*t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){
return(*t).f2;}static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp1364;return(_tmp1364=_cycalloc(sizeof(*_tmp1364)),((_tmp1364->f1=(*
pr).f1,((_tmp1364->f2=t,_tmp1364)))));}struct _tuple28{struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Tqual f2;};struct _tuple29{struct _tuple28*f1;void*f2;};static
struct _tuple29*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
struct _tuple28*_tmp1367;struct _tuple29*_tmp1366;return(_tmp1366=_region_malloc(
rgn,sizeof(*_tmp1366)),((_tmp1366->f1=((_tmp1367=_region_malloc(rgn,sizeof(*
_tmp1367)),((_tmp1367->f1=(*y).f1,((_tmp1367->f2=(*y).f2,_tmp1367)))))),((
_tmp1366->f2=(*y).f3,_tmp1366)))));}static struct _tuple10*Cyc_Tcutil_substitute_f2(
struct _tuple29*w){struct _tuple28*_tmp987;void*_tmp988;struct _tuple29 _tmp986=*w;
_tmp987=_tmp986.f1;_tmp988=_tmp986.f2;{struct Cyc_Core_Opt*_tmp98A;struct Cyc_Absyn_Tqual
_tmp98B;struct _tuple28 _tmp989=*_tmp987;_tmp98A=_tmp989.f1;_tmp98B=_tmp989.f2;{
struct _tuple10*_tmp1368;return(_tmp1368=_cycalloc(sizeof(*_tmp1368)),((_tmp1368->f1=
_tmp98A,((_tmp1368->f2=_tmp98B,((_tmp1368->f3=_tmp988,_tmp1368)))))));};};}
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){return f->type;}
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(struct Cyc_Absyn_Aggrfield*
f,void*t){struct Cyc_Absyn_Aggrfield*_tmp1369;return(_tmp1369=_cycalloc(sizeof(*
_tmp1369)),((_tmp1369->name=f->name,((_tmp1369->tq=f->tq,((_tmp1369->type=t,((
_tmp1369->width=f->width,((_tmp1369->attributes=f->attributes,_tmp1369)))))))))));}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct
Cyc_Absyn_Exp*old,void*r){struct Cyc_Absyn_Exp*_tmp136A;return(_tmp136A=_cycalloc(
sizeof(*_tmp136A)),((_tmp136A->topt=old->topt,((_tmp136A->r=r,((_tmp136A->loc=
old->loc,((_tmp136A->annot=old->annot,_tmp136A)))))))));}static struct Cyc_Absyn_Exp*
Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*
e){void*_tmp98F=e->r;enum Cyc_Absyn_Primop _tmp995;struct Cyc_List_List*_tmp996;
struct Cyc_Absyn_Exp*_tmp998;struct Cyc_Absyn_Exp*_tmp999;struct Cyc_Absyn_Exp*
_tmp99A;struct Cyc_Absyn_Exp*_tmp99C;struct Cyc_Absyn_Exp*_tmp99D;struct Cyc_Absyn_Exp*
_tmp99F;struct Cyc_Absyn_Exp*_tmp9A0;struct Cyc_Absyn_Exp*_tmp9A2;struct Cyc_Absyn_Exp*
_tmp9A3;void*_tmp9A5;struct Cyc_Absyn_Exp*_tmp9A6;int _tmp9A7;enum Cyc_Absyn_Coercion
_tmp9A8;void*_tmp9AA;struct Cyc_Absyn_Exp*_tmp9AC;void*_tmp9AE;void*_tmp9AF;void*
_tmp9B1;_LL4DB: {struct Cyc_Absyn_Const_e_struct*_tmp990=(struct Cyc_Absyn_Const_e_struct*)
_tmp98F;if(_tmp990->tag != 0)goto _LL4DD;}_LL4DC: goto _LL4DE;_LL4DD: {struct Cyc_Absyn_Enum_e_struct*
_tmp991=(struct Cyc_Absyn_Enum_e_struct*)_tmp98F;if(_tmp991->tag != 32)goto _LL4DF;}
_LL4DE: goto _LL4E0;_LL4DF: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp992=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp98F;if(_tmp992->tag != 33)goto _LL4E1;}_LL4E0: goto _LL4E2;_LL4E1: {struct Cyc_Absyn_Var_e_struct*
_tmp993=(struct Cyc_Absyn_Var_e_struct*)_tmp98F;if(_tmp993->tag != 1)goto _LL4E3;}
_LL4E2: return e;_LL4E3: {struct Cyc_Absyn_Primop_e_struct*_tmp994=(struct Cyc_Absyn_Primop_e_struct*)
_tmp98F;if(_tmp994->tag != 3)goto _LL4E5;else{_tmp995=_tmp994->f1;_tmp996=_tmp994->f2;}}
_LL4E4: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp996)== 1){struct
Cyc_Absyn_Exp*_tmp9B2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp996))->hd;struct Cyc_Absyn_Exp*_tmp9B3=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B2);
if(_tmp9B3 == _tmp9B2)return e;{struct Cyc_Absyn_Primop_e_struct _tmp1370;struct Cyc_Absyn_Exp*
_tmp136F[1];struct Cyc_Absyn_Primop_e_struct*_tmp136E;return Cyc_Tcutil_copye(e,(
void*)((_tmp136E=_cycalloc(sizeof(*_tmp136E)),((_tmp136E[0]=((_tmp1370.tag=3,((
_tmp1370.f1=_tmp995,((_tmp1370.f2=((_tmp136F[0]=_tmp9B3,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp136F,sizeof(struct Cyc_Absyn_Exp*),
1)))),_tmp1370)))))),_tmp136E)))));};}else{if(((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp996)== 2){struct Cyc_Absyn_Exp*_tmp9B7=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp996))->hd;struct Cyc_Absyn_Exp*_tmp9B8=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp996->tl))->hd;struct
Cyc_Absyn_Exp*_tmp9B9=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B7);struct Cyc_Absyn_Exp*
_tmp9BA=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B8);if(_tmp9B9 == _tmp9B7  && _tmp9BA == 
_tmp9B8)return e;{struct Cyc_Absyn_Primop_e_struct _tmp1376;struct Cyc_Absyn_Exp*
_tmp1375[2];struct Cyc_Absyn_Primop_e_struct*_tmp1374;return Cyc_Tcutil_copye(e,(
void*)((_tmp1374=_cycalloc(sizeof(*_tmp1374)),((_tmp1374[0]=((_tmp1376.tag=3,((
_tmp1376.f1=_tmp995,((_tmp1376.f2=((_tmp1375[1]=_tmp9BA,((_tmp1375[0]=_tmp9B9,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp1375,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp1376)))))),_tmp1374)))));};}
else{const char*_tmp1379;void*_tmp1378;return(_tmp1378=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1379="primop does not have 1 or 2 args!",
_tag_dyneither(_tmp1379,sizeof(char),34))),_tag_dyneither(_tmp1378,sizeof(void*),
0)));}}_LL4E5: {struct Cyc_Absyn_Conditional_e_struct*_tmp997=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp98F;if(_tmp997->tag != 6)goto _LL4E7;else{_tmp998=_tmp997->f1;_tmp999=_tmp997->f2;
_tmp99A=_tmp997->f3;}}_LL4E6: {struct Cyc_Absyn_Exp*_tmp9C0=Cyc_Tcutil_rsubsexp(r,
inst,_tmp998);struct Cyc_Absyn_Exp*_tmp9C1=Cyc_Tcutil_rsubsexp(r,inst,_tmp999);
struct Cyc_Absyn_Exp*_tmp9C2=Cyc_Tcutil_rsubsexp(r,inst,_tmp99A);if((_tmp9C0 == 
_tmp998  && _tmp9C1 == _tmp999) && _tmp9C2 == _tmp99A)return e;{struct Cyc_Absyn_Conditional_e_struct
_tmp137C;struct Cyc_Absyn_Conditional_e_struct*_tmp137B;return Cyc_Tcutil_copye(e,(
void*)((_tmp137B=_cycalloc(sizeof(*_tmp137B)),((_tmp137B[0]=((_tmp137C.tag=6,((
_tmp137C.f1=_tmp9C0,((_tmp137C.f2=_tmp9C1,((_tmp137C.f3=_tmp9C2,_tmp137C)))))))),
_tmp137B)))));};}_LL4E7: {struct Cyc_Absyn_And_e_struct*_tmp99B=(struct Cyc_Absyn_And_e_struct*)
_tmp98F;if(_tmp99B->tag != 7)goto _LL4E9;else{_tmp99C=_tmp99B->f1;_tmp99D=_tmp99B->f2;}}
_LL4E8: {struct Cyc_Absyn_Exp*_tmp9C5=Cyc_Tcutil_rsubsexp(r,inst,_tmp99C);struct
Cyc_Absyn_Exp*_tmp9C6=Cyc_Tcutil_rsubsexp(r,inst,_tmp99D);if(_tmp9C5 == _tmp99C
 && _tmp9C6 == _tmp99D)return e;{struct Cyc_Absyn_And_e_struct _tmp137F;struct Cyc_Absyn_And_e_struct*
_tmp137E;return Cyc_Tcutil_copye(e,(void*)((_tmp137E=_cycalloc(sizeof(*_tmp137E)),((
_tmp137E[0]=((_tmp137F.tag=7,((_tmp137F.f1=_tmp9C5,((_tmp137F.f2=_tmp9C6,
_tmp137F)))))),_tmp137E)))));};}_LL4E9: {struct Cyc_Absyn_Or_e_struct*_tmp99E=(
struct Cyc_Absyn_Or_e_struct*)_tmp98F;if(_tmp99E->tag != 8)goto _LL4EB;else{_tmp99F=
_tmp99E->f1;_tmp9A0=_tmp99E->f2;}}_LL4EA: {struct Cyc_Absyn_Exp*_tmp9C9=Cyc_Tcutil_rsubsexp(
r,inst,_tmp99F);struct Cyc_Absyn_Exp*_tmp9CA=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A0);
if(_tmp9C9 == _tmp99F  && _tmp9CA == _tmp9A0)return e;{struct Cyc_Absyn_Or_e_struct
_tmp1382;struct Cyc_Absyn_Or_e_struct*_tmp1381;return Cyc_Tcutil_copye(e,(void*)((
_tmp1381=_cycalloc(sizeof(*_tmp1381)),((_tmp1381[0]=((_tmp1382.tag=8,((_tmp1382.f1=
_tmp9C9,((_tmp1382.f2=_tmp9CA,_tmp1382)))))),_tmp1381)))));};}_LL4EB: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp9A1=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp98F;if(_tmp9A1->tag != 9)goto _LL4ED;
else{_tmp9A2=_tmp9A1->f1;_tmp9A3=_tmp9A1->f2;}}_LL4EC: {struct Cyc_Absyn_Exp*
_tmp9CD=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A2);struct Cyc_Absyn_Exp*_tmp9CE=Cyc_Tcutil_rsubsexp(
r,inst,_tmp9A3);if(_tmp9CD == _tmp9A2  && _tmp9CE == _tmp9A3)return e;{struct Cyc_Absyn_SeqExp_e_struct
_tmp1385;struct Cyc_Absyn_SeqExp_e_struct*_tmp1384;return Cyc_Tcutil_copye(e,(void*)((
_tmp1384=_cycalloc(sizeof(*_tmp1384)),((_tmp1384[0]=((_tmp1385.tag=9,((_tmp1385.f1=
_tmp9CD,((_tmp1385.f2=_tmp9CE,_tmp1385)))))),_tmp1384)))));};}_LL4ED: {struct Cyc_Absyn_Cast_e_struct*
_tmp9A4=(struct Cyc_Absyn_Cast_e_struct*)_tmp98F;if(_tmp9A4->tag != 15)goto _LL4EF;
else{_tmp9A5=(void*)_tmp9A4->f1;_tmp9A6=_tmp9A4->f2;_tmp9A7=_tmp9A4->f3;_tmp9A8=
_tmp9A4->f4;}}_LL4EE: {struct Cyc_Absyn_Exp*_tmp9D1=Cyc_Tcutil_rsubsexp(r,inst,
_tmp9A6);void*_tmp9D2=Cyc_Tcutil_rsubstitute(r,inst,_tmp9A5);if(_tmp9D1 == 
_tmp9A6  && _tmp9D2 == _tmp9A5)return e;{struct Cyc_Absyn_Cast_e_struct _tmp1388;
struct Cyc_Absyn_Cast_e_struct*_tmp1387;return Cyc_Tcutil_copye(e,(void*)((
_tmp1387=_cycalloc(sizeof(*_tmp1387)),((_tmp1387[0]=((_tmp1388.tag=15,((_tmp1388.f1=(
void*)_tmp9D2,((_tmp1388.f2=_tmp9D1,((_tmp1388.f3=_tmp9A7,((_tmp1388.f4=_tmp9A8,
_tmp1388)))))))))),_tmp1387)))));};}_LL4EF: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp9A9=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp98F;if(_tmp9A9->tag != 18)goto
_LL4F1;else{_tmp9AA=(void*)_tmp9A9->f1;}}_LL4F0: {void*_tmp9D5=Cyc_Tcutil_rsubstitute(
r,inst,_tmp9AA);if(_tmp9D5 == _tmp9AA)return e;{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp138B;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp138A;return Cyc_Tcutil_copye(e,(
void*)((_tmp138A=_cycalloc(sizeof(*_tmp138A)),((_tmp138A[0]=((_tmp138B.tag=18,((
_tmp138B.f1=(void*)_tmp9D5,_tmp138B)))),_tmp138A)))));};}_LL4F1: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp9AB=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp98F;if(_tmp9AB->tag != 19)goto
_LL4F3;else{_tmp9AC=_tmp9AB->f1;}}_LL4F2: {struct Cyc_Absyn_Exp*_tmp9D8=Cyc_Tcutil_rsubsexp(
r,inst,_tmp9AC);if(_tmp9D8 == _tmp9AC)return e;{struct Cyc_Absyn_Sizeofexp_e_struct
_tmp138E;struct Cyc_Absyn_Sizeofexp_e_struct*_tmp138D;return Cyc_Tcutil_copye(e,(
void*)((_tmp138D=_cycalloc(sizeof(*_tmp138D)),((_tmp138D[0]=((_tmp138E.tag=19,((
_tmp138E.f1=_tmp9D8,_tmp138E)))),_tmp138D)))));};}_LL4F3: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp9AD=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp98F;if(_tmp9AD->tag != 20)goto
_LL4F5;else{_tmp9AE=(void*)_tmp9AD->f1;_tmp9AF=(void*)_tmp9AD->f2;}}_LL4F4: {
void*_tmp9DB=Cyc_Tcutil_rsubstitute(r,inst,_tmp9AE);if(_tmp9DB == _tmp9AE)return e;{
struct Cyc_Absyn_Offsetof_e_struct _tmp1391;struct Cyc_Absyn_Offsetof_e_struct*
_tmp1390;return Cyc_Tcutil_copye(e,(void*)((_tmp1390=_cycalloc(sizeof(*_tmp1390)),((
_tmp1390[0]=((_tmp1391.tag=20,((_tmp1391.f1=(void*)_tmp9DB,((_tmp1391.f2=(void*)
_tmp9AF,_tmp1391)))))),_tmp1390)))));};}_LL4F5: {struct Cyc_Absyn_Valueof_e_struct*
_tmp9B0=(struct Cyc_Absyn_Valueof_e_struct*)_tmp98F;if(_tmp9B0->tag != 39)goto
_LL4F7;else{_tmp9B1=(void*)_tmp9B0->f1;}}_LL4F6: {void*_tmp9DE=Cyc_Tcutil_rsubstitute(
r,inst,_tmp9B1);if(_tmp9DE == _tmp9B1)return e;{struct Cyc_Absyn_Valueof_e_struct
_tmp1394;struct Cyc_Absyn_Valueof_e_struct*_tmp1393;return Cyc_Tcutil_copye(e,(
void*)((_tmp1393=_cycalloc(sizeof(*_tmp1393)),((_tmp1393[0]=((_tmp1394.tag=39,((
_tmp1394.f1=(void*)_tmp9DE,_tmp1394)))),_tmp1393)))));};}_LL4F7:;_LL4F8: {const
char*_tmp1397;void*_tmp1396;return(_tmp1396=0,((struct Cyc_Absyn_Exp*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1397="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmp1397,sizeof(char),46))),_tag_dyneither(_tmp1396,sizeof(void*),
0)));}_LL4DA:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp9E3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp9E5;
struct Cyc_Absyn_AggrInfo _tmp9E7;union Cyc_Absyn_AggrInfoU _tmp9E8;struct Cyc_List_List*
_tmp9E9;struct Cyc_Absyn_DatatypeInfo _tmp9EB;union Cyc_Absyn_DatatypeInfoU _tmp9EC;
struct Cyc_List_List*_tmp9ED;struct Cyc_Absyn_DatatypeFieldInfo _tmp9EF;union Cyc_Absyn_DatatypeFieldInfoU
_tmp9F0;struct Cyc_List_List*_tmp9F1;struct _tuple2*_tmp9F3;struct Cyc_List_List*
_tmp9F4;struct Cyc_Absyn_Typedefdecl*_tmp9F5;void**_tmp9F6;struct Cyc_Absyn_ArrayInfo
_tmp9F8;void*_tmp9F9;struct Cyc_Absyn_Tqual _tmp9FA;struct Cyc_Absyn_Exp*_tmp9FB;
union Cyc_Absyn_Constraint*_tmp9FC;struct Cyc_Position_Segment*_tmp9FD;struct Cyc_Absyn_PtrInfo
_tmp9FF;void*_tmpA00;struct Cyc_Absyn_Tqual _tmpA01;struct Cyc_Absyn_PtrAtts _tmpA02;
void*_tmpA03;union Cyc_Absyn_Constraint*_tmpA04;union Cyc_Absyn_Constraint*_tmpA05;
union Cyc_Absyn_Constraint*_tmpA06;struct Cyc_Absyn_FnInfo _tmpA08;struct Cyc_List_List*
_tmpA09;struct Cyc_Core_Opt*_tmpA0A;struct Cyc_Absyn_Tqual _tmpA0B;void*_tmpA0C;
struct Cyc_List_List*_tmpA0D;int _tmpA0E;struct Cyc_Absyn_VarargInfo*_tmpA0F;struct
Cyc_List_List*_tmpA10;struct Cyc_List_List*_tmpA11;struct Cyc_List_List*_tmpA13;
enum Cyc_Absyn_AggrKind _tmpA15;struct Cyc_List_List*_tmpA16;struct Cyc_Core_Opt*
_tmpA18;void*_tmpA1A;void*_tmpA1C;void*_tmpA1D;void*_tmpA1F;struct Cyc_Absyn_Exp*
_tmpA21;void*_tmpA2B;void*_tmpA2D;struct Cyc_List_List*_tmpA2F;_LL4FA: {struct Cyc_Absyn_VarType_struct*
_tmp9E4=(struct Cyc_Absyn_VarType_struct*)_tmp9E3;if(_tmp9E4->tag != 2)goto _LL4FC;
else{_tmp9E5=_tmp9E4->f1;}}_LL4FB: {struct _handler_cons _tmpA31;_push_handler(&
_tmpA31);{int _tmpA33=0;if(setjmp(_tmpA31.handler))_tmpA33=1;if(!_tmpA33){{void*
_tmpA34=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,
inst,_tmp9E5);_npop_handler(0);return _tmpA34;};_pop_handler();}else{void*_tmpA32=(
void*)_exn_thrown;void*_tmpA36=_tmpA32;_LL531: {struct Cyc_Core_Not_found_struct*
_tmpA37=(struct Cyc_Core_Not_found_struct*)_tmpA36;if(_tmpA37->tag != Cyc_Core_Not_found)
goto _LL533;}_LL532: return t;_LL533:;_LL534:(void)_throw(_tmpA36);_LL530:;}};}
_LL4FC: {struct Cyc_Absyn_AggrType_struct*_tmp9E6=(struct Cyc_Absyn_AggrType_struct*)
_tmp9E3;if(_tmp9E6->tag != 11)goto _LL4FE;else{_tmp9E7=_tmp9E6->f1;_tmp9E8=_tmp9E7.aggr_info;
_tmp9E9=_tmp9E7.targs;}}_LL4FD: {struct Cyc_List_List*_tmpA38=Cyc_Tcutil_substs(
rgn,inst,_tmp9E9);struct Cyc_Absyn_AggrType_struct _tmp139D;struct Cyc_Absyn_AggrInfo
_tmp139C;struct Cyc_Absyn_AggrType_struct*_tmp139B;return _tmpA38 == _tmp9E9?t:(
void*)((_tmp139B=_cycalloc(sizeof(*_tmp139B)),((_tmp139B[0]=((_tmp139D.tag=11,((
_tmp139D.f1=((_tmp139C.aggr_info=_tmp9E8,((_tmp139C.targs=_tmpA38,_tmp139C)))),
_tmp139D)))),_tmp139B))));}_LL4FE: {struct Cyc_Absyn_DatatypeType_struct*_tmp9EA=(
struct Cyc_Absyn_DatatypeType_struct*)_tmp9E3;if(_tmp9EA->tag != 3)goto _LL500;
else{_tmp9EB=_tmp9EA->f1;_tmp9EC=_tmp9EB.datatype_info;_tmp9ED=_tmp9EB.targs;}}
_LL4FF: {struct Cyc_List_List*_tmpA3C=Cyc_Tcutil_substs(rgn,inst,_tmp9ED);struct
Cyc_Absyn_DatatypeType_struct _tmp13A3;struct Cyc_Absyn_DatatypeInfo _tmp13A2;
struct Cyc_Absyn_DatatypeType_struct*_tmp13A1;return _tmpA3C == _tmp9ED?t:(void*)((
_tmp13A1=_cycalloc(sizeof(*_tmp13A1)),((_tmp13A1[0]=((_tmp13A3.tag=3,((_tmp13A3.f1=((
_tmp13A2.datatype_info=_tmp9EC,((_tmp13A2.targs=_tmpA3C,_tmp13A2)))),_tmp13A3)))),
_tmp13A1))));}_LL500: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp9EE=(struct
Cyc_Absyn_DatatypeFieldType_struct*)_tmp9E3;if(_tmp9EE->tag != 4)goto _LL502;else{
_tmp9EF=_tmp9EE->f1;_tmp9F0=_tmp9EF.field_info;_tmp9F1=_tmp9EF.targs;}}_LL501: {
struct Cyc_List_List*_tmpA40=Cyc_Tcutil_substs(rgn,inst,_tmp9F1);struct Cyc_Absyn_DatatypeFieldType_struct
_tmp13A9;struct Cyc_Absyn_DatatypeFieldInfo _tmp13A8;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp13A7;return _tmpA40 == _tmp9F1?t:(void*)((_tmp13A7=_cycalloc(sizeof(*_tmp13A7)),((
_tmp13A7[0]=((_tmp13A9.tag=4,((_tmp13A9.f1=((_tmp13A8.field_info=_tmp9F0,((
_tmp13A8.targs=_tmpA40,_tmp13A8)))),_tmp13A9)))),_tmp13A7))));}_LL502: {struct
Cyc_Absyn_TypedefType_struct*_tmp9F2=(struct Cyc_Absyn_TypedefType_struct*)
_tmp9E3;if(_tmp9F2->tag != 17)goto _LL504;else{_tmp9F3=_tmp9F2->f1;_tmp9F4=_tmp9F2->f2;
_tmp9F5=_tmp9F2->f3;_tmp9F6=_tmp9F2->f4;}}_LL503: {struct Cyc_List_List*_tmpA44=
Cyc_Tcutil_substs(rgn,inst,_tmp9F4);struct Cyc_Absyn_TypedefType_struct _tmp13AC;
struct Cyc_Absyn_TypedefType_struct*_tmp13AB;return _tmpA44 == _tmp9F4?t:(void*)((
_tmp13AB=_cycalloc(sizeof(*_tmp13AB)),((_tmp13AB[0]=((_tmp13AC.tag=17,((_tmp13AC.f1=
_tmp9F3,((_tmp13AC.f2=_tmpA44,((_tmp13AC.f3=_tmp9F5,((_tmp13AC.f4=_tmp9F6,
_tmp13AC)))))))))),_tmp13AB))));}_LL504: {struct Cyc_Absyn_ArrayType_struct*
_tmp9F7=(struct Cyc_Absyn_ArrayType_struct*)_tmp9E3;if(_tmp9F7->tag != 8)goto
_LL506;else{_tmp9F8=_tmp9F7->f1;_tmp9F9=_tmp9F8.elt_type;_tmp9FA=_tmp9F8.tq;
_tmp9FB=_tmp9F8.num_elts;_tmp9FC=_tmp9F8.zero_term;_tmp9FD=_tmp9F8.zt_loc;}}
_LL505: {void*_tmpA47=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp9F9);struct Cyc_Absyn_ArrayType_struct
_tmp13B2;struct Cyc_Absyn_ArrayInfo _tmp13B1;struct Cyc_Absyn_ArrayType_struct*
_tmp13B0;return _tmpA47 == _tmp9F9?t:(void*)((_tmp13B0=_cycalloc(sizeof(*_tmp13B0)),((
_tmp13B0[0]=((_tmp13B2.tag=8,((_tmp13B2.f1=((_tmp13B1.elt_type=_tmpA47,((
_tmp13B1.tq=_tmp9FA,((_tmp13B1.num_elts=_tmp9FB,((_tmp13B1.zero_term=_tmp9FC,((
_tmp13B1.zt_loc=_tmp9FD,_tmp13B1)))))))))),_tmp13B2)))),_tmp13B0))));}_LL506: {
struct Cyc_Absyn_PointerType_struct*_tmp9FE=(struct Cyc_Absyn_PointerType_struct*)
_tmp9E3;if(_tmp9FE->tag != 5)goto _LL508;else{_tmp9FF=_tmp9FE->f1;_tmpA00=_tmp9FF.elt_typ;
_tmpA01=_tmp9FF.elt_tq;_tmpA02=_tmp9FF.ptr_atts;_tmpA03=_tmpA02.rgn;_tmpA04=
_tmpA02.nullable;_tmpA05=_tmpA02.bounds;_tmpA06=_tmpA02.zero_term;}}_LL507: {
void*_tmpA4B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA00);void*_tmpA4C=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA03);union Cyc_Absyn_Constraint*_tmpA4D=_tmpA05;{void*_tmpA4E=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpA05);struct Cyc_Absyn_Exp*_tmpA50;_LL536: {struct Cyc_Absyn_Upper_b_struct*
_tmpA4F=(struct Cyc_Absyn_Upper_b_struct*)_tmpA4E;if(_tmpA4F->tag != 1)goto _LL538;
else{_tmpA50=_tmpA4F->f1;}}_LL537: {struct Cyc_Absyn_Exp*_tmpA51=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmpA50);if(_tmpA51 != _tmpA50){struct Cyc_Absyn_Upper_b_struct _tmp13B5;
struct Cyc_Absyn_Upper_b_struct*_tmp13B4;_tmpA4D=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp13B4=_cycalloc(sizeof(*_tmp13B4)),((
_tmp13B4[0]=((_tmp13B5.tag=1,((_tmp13B5.f1=_tmpA51,_tmp13B5)))),_tmp13B4)))));}
goto _LL535;}_LL538:;_LL539: goto _LL535;_LL535:;}if((_tmpA4B == _tmpA00  && _tmpA4C
== _tmpA03) && _tmpA4D == _tmpA05)return t;{struct Cyc_Absyn_PointerType_struct
_tmp13BF;struct Cyc_Absyn_PtrAtts _tmp13BE;struct Cyc_Absyn_PtrInfo _tmp13BD;struct
Cyc_Absyn_PointerType_struct*_tmp13BC;return(void*)((_tmp13BC=_cycalloc(sizeof(*
_tmp13BC)),((_tmp13BC[0]=((_tmp13BF.tag=5,((_tmp13BF.f1=((_tmp13BD.elt_typ=
_tmpA4B,((_tmp13BD.elt_tq=_tmpA01,((_tmp13BD.ptr_atts=((_tmp13BE.rgn=_tmpA4C,((
_tmp13BE.nullable=_tmpA04,((_tmp13BE.bounds=_tmpA4D,((_tmp13BE.zero_term=_tmpA06,((
_tmp13BE.ptrloc=0,_tmp13BE)))))))))),_tmp13BD)))))),_tmp13BF)))),_tmp13BC))));};}
_LL508: {struct Cyc_Absyn_FnType_struct*_tmpA07=(struct Cyc_Absyn_FnType_struct*)
_tmp9E3;if(_tmpA07->tag != 9)goto _LL50A;else{_tmpA08=_tmpA07->f1;_tmpA09=_tmpA08.tvars;
_tmpA0A=_tmpA08.effect;_tmpA0B=_tmpA08.ret_tqual;_tmpA0C=_tmpA08.ret_typ;_tmpA0D=
_tmpA08.args;_tmpA0E=_tmpA08.c_varargs;_tmpA0F=_tmpA08.cyc_varargs;_tmpA10=
_tmpA08.rgn_po;_tmpA11=_tmpA08.attributes;}}_LL509:{struct Cyc_List_List*_tmpA58=
_tmpA09;for(0;_tmpA58 != 0;_tmpA58=_tmpA58->tl){struct _tuple16*_tmp13C9;struct Cyc_Absyn_VarType_struct
_tmp13C8;struct Cyc_Absyn_VarType_struct*_tmp13C7;struct Cyc_List_List*_tmp13C6;
inst=((_tmp13C6=_region_malloc(rgn,sizeof(*_tmp13C6)),((_tmp13C6->hd=((_tmp13C9=
_region_malloc(rgn,sizeof(*_tmp13C9)),((_tmp13C9->f1=(struct Cyc_Absyn_Tvar*)
_tmpA58->hd,((_tmp13C9->f2=(void*)((_tmp13C7=_cycalloc(sizeof(*_tmp13C7)),((
_tmp13C7[0]=((_tmp13C8.tag=2,((_tmp13C8.f1=(struct Cyc_Absyn_Tvar*)_tmpA58->hd,
_tmp13C8)))),_tmp13C7)))),_tmp13C9)))))),((_tmp13C6->tl=inst,_tmp13C6))))));}}{
struct Cyc_List_List*_tmpA5E;struct Cyc_List_List*_tmpA5F;struct _tuple1 _tmpA5D=((
struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple29*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*
env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,
_tmpA0D));_tmpA5E=_tmpA5D.f1;_tmpA5F=_tmpA5D.f2;{struct Cyc_List_List*_tmpA60=Cyc_Tcutil_substs(
rgn,inst,_tmpA5F);struct Cyc_List_List*_tmpA61=((struct Cyc_List_List*(*)(struct
_tuple10*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA5E,_tmpA60));struct Cyc_Core_Opt*
eff2;if(_tmpA0A == 0)eff2=0;else{void*_tmpA62=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmpA0A->v);if(_tmpA62 == (void*)_tmpA0A->v)eff2=_tmpA0A;else{struct Cyc_Core_Opt*
_tmp13CA;eff2=((_tmp13CA=_cycalloc(sizeof(*_tmp13CA)),((_tmp13CA->v=_tmpA62,
_tmp13CA))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmpA0F == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmpA65;struct Cyc_Absyn_Tqual _tmpA66;void*
_tmpA67;int _tmpA68;struct Cyc_Absyn_VarargInfo _tmpA64=*_tmpA0F;_tmpA65=_tmpA64.name;
_tmpA66=_tmpA64.tq;_tmpA67=_tmpA64.type;_tmpA68=_tmpA64.inject;{void*_tmpA69=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA67);if(_tmpA69 == _tmpA67)cyc_varargs2=_tmpA0F;else{struct Cyc_Absyn_VarargInfo*
_tmp13CB;cyc_varargs2=((_tmp13CB=_cycalloc(sizeof(*_tmp13CB)),((_tmp13CB->name=
_tmpA65,((_tmp13CB->tq=_tmpA66,((_tmp13CB->type=_tmpA69,((_tmp13CB->inject=
_tmpA68,_tmp13CB))))))))));}};}{struct Cyc_List_List*rgn_po2;struct Cyc_List_List*
_tmpA6C;struct Cyc_List_List*_tmpA6D;struct _tuple1 _tmpA6B=((struct _tuple1(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,_tmpA10);_tmpA6C=_tmpA6B.f1;_tmpA6D=_tmpA6B.f2;{struct Cyc_List_List*
_tmpA6E=Cyc_Tcutil_substs(rgn,inst,_tmpA6C);struct Cyc_List_List*_tmpA6F=Cyc_Tcutil_substs(
rgn,inst,_tmpA6D);if(_tmpA6E == _tmpA6C  && _tmpA6F == _tmpA6D)rgn_po2=_tmpA10;
else{rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_zip)(_tmpA6E,_tmpA6F);}{struct Cyc_Absyn_FnType_struct _tmp13D1;struct
Cyc_Absyn_FnInfo _tmp13D0;struct Cyc_Absyn_FnType_struct*_tmp13CF;return(void*)((
_tmp13CF=_cycalloc(sizeof(*_tmp13CF)),((_tmp13CF[0]=((_tmp13D1.tag=9,((_tmp13D1.f1=((
_tmp13D0.tvars=_tmpA09,((_tmp13D0.effect=eff2,((_tmp13D0.ret_tqual=_tmpA0B,((
_tmp13D0.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA0C),((_tmp13D0.args=
_tmpA61,((_tmp13D0.c_varargs=_tmpA0E,((_tmp13D0.cyc_varargs=cyc_varargs2,((
_tmp13D0.rgn_po=rgn_po2,((_tmp13D0.attributes=_tmpA11,_tmp13D0)))))))))))))))))),
_tmp13D1)))),_tmp13CF))));};};};};};};_LL50A: {struct Cyc_Absyn_TupleType_struct*
_tmpA12=(struct Cyc_Absyn_TupleType_struct*)_tmp9E3;if(_tmpA12->tag != 10)goto
_LL50C;else{_tmpA13=_tmpA12->f1;}}_LL50B: {struct Cyc_List_List*_tmpA73=((struct
Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple12*),struct Cyc_List_List*
x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,_tmpA13);struct Cyc_List_List*_tmpA74=
Cyc_Tcutil_substs(rgn,inst,_tmpA73);if(_tmpA74 == _tmpA73)return t;{struct Cyc_List_List*
_tmpA75=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmpA13,_tmpA74);struct Cyc_Absyn_TupleType_struct _tmp13D4;struct Cyc_Absyn_TupleType_struct*
_tmp13D3;return(void*)((_tmp13D3=_cycalloc(sizeof(*_tmp13D3)),((_tmp13D3[0]=((
_tmp13D4.tag=10,((_tmp13D4.f1=_tmpA75,_tmp13D4)))),_tmp13D3))));};}_LL50C: {
struct Cyc_Absyn_AnonAggrType_struct*_tmpA14=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp9E3;if(_tmpA14->tag != 12)goto _LL50E;else{_tmpA15=_tmpA14->f1;_tmpA16=_tmpA14->f2;}}
_LL50D: {struct Cyc_List_List*_tmpA78=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(
rgn,Cyc_Tcutil_field_type,_tmpA16);struct Cyc_List_List*_tmpA79=Cyc_Tcutil_substs(
rgn,inst,_tmpA78);if(_tmpA79 == _tmpA78)return t;{struct Cyc_List_List*_tmpA7A=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,
_tmpA16,_tmpA79);struct Cyc_Absyn_AnonAggrType_struct _tmp13D7;struct Cyc_Absyn_AnonAggrType_struct*
_tmp13D6;return(void*)((_tmp13D6=_cycalloc(sizeof(*_tmp13D6)),((_tmp13D6[0]=((
_tmp13D7.tag=12,((_tmp13D7.f1=_tmpA15,((_tmp13D7.f2=_tmpA7A,_tmp13D7)))))),
_tmp13D6))));};}_LL50E: {struct Cyc_Absyn_Evar_struct*_tmpA17=(struct Cyc_Absyn_Evar_struct*)
_tmp9E3;if(_tmpA17->tag != 1)goto _LL510;else{_tmpA18=_tmpA17->f2;}}_LL50F: if(
_tmpA18 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmpA18->v);else{return
t;}_LL510: {struct Cyc_Absyn_RgnHandleType_struct*_tmpA19=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp9E3;if(_tmpA19->tag != 15)goto _LL512;else{_tmpA1A=(void*)_tmpA19->f1;}}_LL511: {
void*_tmpA7D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA1A);struct Cyc_Absyn_RgnHandleType_struct
_tmp13DA;struct Cyc_Absyn_RgnHandleType_struct*_tmp13D9;return _tmpA7D == _tmpA1A?t:(
void*)((_tmp13D9=_cycalloc(sizeof(*_tmp13D9)),((_tmp13D9[0]=((_tmp13DA.tag=15,((
_tmp13DA.f1=(void*)_tmpA7D,_tmp13DA)))),_tmp13D9))));}_LL512: {struct Cyc_Absyn_DynRgnType_struct*
_tmpA1B=(struct Cyc_Absyn_DynRgnType_struct*)_tmp9E3;if(_tmpA1B->tag != 16)goto
_LL514;else{_tmpA1C=(void*)_tmpA1B->f1;_tmpA1D=(void*)_tmpA1B->f2;}}_LL513: {
void*_tmpA80=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA1C);void*_tmpA81=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA1D);struct Cyc_Absyn_DynRgnType_struct _tmp13DD;struct Cyc_Absyn_DynRgnType_struct*
_tmp13DC;return _tmpA80 == _tmpA1C  && _tmpA81 == _tmpA1D?t:(void*)((_tmp13DC=
_cycalloc(sizeof(*_tmp13DC)),((_tmp13DC[0]=((_tmp13DD.tag=16,((_tmp13DD.f1=(void*)
_tmpA80,((_tmp13DD.f2=(void*)_tmpA81,_tmp13DD)))))),_tmp13DC))));}_LL514: {
struct Cyc_Absyn_TagType_struct*_tmpA1E=(struct Cyc_Absyn_TagType_struct*)_tmp9E3;
if(_tmpA1E->tag != 19)goto _LL516;else{_tmpA1F=(void*)_tmpA1E->f1;}}_LL515: {void*
_tmpA84=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA1F);struct Cyc_Absyn_TagType_struct
_tmp13E0;struct Cyc_Absyn_TagType_struct*_tmp13DF;return _tmpA84 == _tmpA1F?t:(void*)((
_tmp13DF=_cycalloc(sizeof(*_tmp13DF)),((_tmp13DF[0]=((_tmp13E0.tag=19,((_tmp13E0.f1=(
void*)_tmpA84,_tmp13E0)))),_tmp13DF))));}_LL516: {struct Cyc_Absyn_ValueofType_struct*
_tmpA20=(struct Cyc_Absyn_ValueofType_struct*)_tmp9E3;if(_tmpA20->tag != 18)goto
_LL518;else{_tmpA21=_tmpA20->f1;}}_LL517: {struct Cyc_Absyn_Exp*_tmpA87=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmpA21);struct Cyc_Absyn_ValueofType_struct _tmp13E3;struct Cyc_Absyn_ValueofType_struct*
_tmp13E2;return _tmpA87 == _tmpA21?t:(void*)((_tmp13E2=_cycalloc(sizeof(*_tmp13E2)),((
_tmp13E2[0]=((_tmp13E3.tag=18,((_tmp13E3.f1=_tmpA87,_tmp13E3)))),_tmp13E2))));}
_LL518: {struct Cyc_Absyn_EnumType_struct*_tmpA22=(struct Cyc_Absyn_EnumType_struct*)
_tmp9E3;if(_tmpA22->tag != 13)goto _LL51A;}_LL519: goto _LL51B;_LL51A: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpA23=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp9E3;if(_tmpA23->tag != 14)goto
_LL51C;}_LL51B: goto _LL51D;_LL51C: {struct Cyc_Absyn_VoidType_struct*_tmpA24=(
struct Cyc_Absyn_VoidType_struct*)_tmp9E3;if(_tmpA24->tag != 0)goto _LL51E;}_LL51D:
goto _LL51F;_LL51E: {struct Cyc_Absyn_IntType_struct*_tmpA25=(struct Cyc_Absyn_IntType_struct*)
_tmp9E3;if(_tmpA25->tag != 6)goto _LL520;}_LL51F: goto _LL521;_LL520: {struct Cyc_Absyn_FloatType_struct*
_tmpA26=(struct Cyc_Absyn_FloatType_struct*)_tmp9E3;if(_tmpA26->tag != 7)goto
_LL522;}_LL521: goto _LL523;_LL522: {struct Cyc_Absyn_RefCntRgn_struct*_tmpA27=(
struct Cyc_Absyn_RefCntRgn_struct*)_tmp9E3;if(_tmpA27->tag != 22)goto _LL524;}
_LL523: goto _LL525;_LL524: {struct Cyc_Absyn_UniqueRgn_struct*_tmpA28=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp9E3;if(_tmpA28->tag != 21)goto _LL526;}_LL525: goto _LL527;_LL526: {struct Cyc_Absyn_HeapRgn_struct*
_tmpA29=(struct Cyc_Absyn_HeapRgn_struct*)_tmp9E3;if(_tmpA29->tag != 20)goto _LL528;}
_LL527: return t;_LL528: {struct Cyc_Absyn_RgnsEff_struct*_tmpA2A=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp9E3;if(_tmpA2A->tag != 25)goto _LL52A;else{_tmpA2B=(void*)_tmpA2A->f1;}}_LL529: {
void*_tmpA8A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA2B);struct Cyc_Absyn_RgnsEff_struct
_tmp13E6;struct Cyc_Absyn_RgnsEff_struct*_tmp13E5;return _tmpA8A == _tmpA2B?t:(void*)((
_tmp13E5=_cycalloc(sizeof(*_tmp13E5)),((_tmp13E5[0]=((_tmp13E6.tag=25,((_tmp13E6.f1=(
void*)_tmpA8A,_tmp13E6)))),_tmp13E5))));}_LL52A: {struct Cyc_Absyn_AccessEff_struct*
_tmpA2C=(struct Cyc_Absyn_AccessEff_struct*)_tmp9E3;if(_tmpA2C->tag != 23)goto
_LL52C;else{_tmpA2D=(void*)_tmpA2C->f1;}}_LL52B: {void*_tmpA8D=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA2D);struct Cyc_Absyn_AccessEff_struct _tmp13E9;struct Cyc_Absyn_AccessEff_struct*
_tmp13E8;return _tmpA8D == _tmpA2D?t:(void*)((_tmp13E8=_cycalloc(sizeof(*_tmp13E8)),((
_tmp13E8[0]=((_tmp13E9.tag=23,((_tmp13E9.f1=(void*)_tmpA8D,_tmp13E9)))),_tmp13E8))));}
_LL52C: {struct Cyc_Absyn_JoinEff_struct*_tmpA2E=(struct Cyc_Absyn_JoinEff_struct*)
_tmp9E3;if(_tmpA2E->tag != 24)goto _LL52E;else{_tmpA2F=_tmpA2E->f1;}}_LL52D: {
struct Cyc_List_List*_tmpA90=Cyc_Tcutil_substs(rgn,inst,_tmpA2F);struct Cyc_Absyn_JoinEff_struct
_tmp13EC;struct Cyc_Absyn_JoinEff_struct*_tmp13EB;return _tmpA90 == _tmpA2F?t:(void*)((
_tmp13EB=_cycalloc(sizeof(*_tmp13EB)),((_tmp13EB[0]=((_tmp13EC.tag=24,((_tmp13EC.f1=
_tmpA90,_tmp13EC)))),_tmp13EB))));}_LL52E: {struct Cyc_Absyn_TypeDeclType_struct*
_tmpA30=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp9E3;if(_tmpA30->tag != 26)goto
_LL4F9;}_LL52F: {const char*_tmp13EF;void*_tmp13EE;(_tmp13EE=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13EF="found typedecltype in rsubs",
_tag_dyneither(_tmp13EF,sizeof(char),28))),_tag_dyneither(_tmp13EE,sizeof(void*),
0)));}_LL4F9:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*
rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*
_tmpA95=(void*)ts->hd;struct Cyc_List_List*_tmpA96=ts->tl;void*_tmpA97=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA95);struct Cyc_List_List*_tmpA98=Cyc_Tcutil_substs(rgn,inst,_tmpA96);
if(_tmpA95 == _tmpA97  && _tmpA96 == _tmpA98)return ts;{struct Cyc_List_List*_tmp13F0;
return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmp13F0=_cycalloc(sizeof(*
_tmp13F0)),((_tmp13F0->hd=_tmpA97,((_tmp13F0->tl=_tmpA98,_tmp13F0)))))));};};}
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(
Cyc_Core_heap_region,inst,t);}struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*
s,struct Cyc_Absyn_Tvar*tv){struct Cyc_Core_Opt*_tmpA9A=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(
tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp13F3;struct _tuple16*_tmp13F2;return(
_tmp13F2=_cycalloc(sizeof(*_tmp13F2)),((_tmp13F2->f1=tv,((_tmp13F2->f2=Cyc_Absyn_new_evar(
_tmpA9A,((_tmp13F3=_cycalloc(sizeof(*_tmp13F3)),((_tmp13F3->v=s,_tmp13F3))))),
_tmp13F2)))));}struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,
struct Cyc_Absyn_Tvar*tv){struct _tuple17 _tmpA9E;struct Cyc_List_List*_tmpA9F;
struct _RegionHandle*_tmpAA0;struct _tuple17*_tmpA9D=env;_tmpA9E=*_tmpA9D;_tmpA9F=
_tmpA9E.f1;_tmpAA0=_tmpA9E.f2;{struct Cyc_Core_Opt*_tmpAA1=Cyc_Tcutil_kind_to_opt(
Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp13F6;struct
_tuple16*_tmp13F5;return(_tmp13F5=_region_malloc(_tmpAA0,sizeof(*_tmp13F5)),((
_tmp13F5->f1=tv,((_tmp13F5->f2=Cyc_Absyn_new_evar(_tmpAA1,((_tmp13F6=_cycalloc(
sizeof(*_tmp13F6)),((_tmp13F6->v=_tmpA9F,_tmp13F6))))),_tmp13F5)))));};}static
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
_tmpAAE=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmpAAE->identity == - 1){const char*
_tmp1406;void*_tmp1405;(_tmp1405=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1406="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmp1406,sizeof(char),41))),_tag_dyneither(_tmp1405,sizeof(void*),
0)));}if(_tmpAAE->identity == tv->identity)return tvs;}}{struct Cyc_List_List*
_tmp1407;return(_tmp1407=_cycalloc(sizeof(*_tmp1407)),((_tmp1407->hd=tv,((
_tmp1407->tl=tvs,_tmp1407)))));};}struct _tuple30{struct Cyc_Absyn_Tvar*f1;int f2;};
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(tv->identity == - 1){
const char*_tmp140A;void*_tmp1409;(_tmp1409=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp140A="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmp140A,sizeof(char),44))),_tag_dyneither(_tmp1409,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple30
_tmpAB5;struct Cyc_Absyn_Tvar*_tmpAB6;int _tmpAB7;int*_tmpAB8;struct _tuple30*
_tmpAB4=(struct _tuple30*)tvs2->hd;_tmpAB5=*_tmpAB4;_tmpAB6=_tmpAB5.f1;_tmpAB7=
_tmpAB5.f2;_tmpAB8=(int*)&(*_tmpAB4).f2;if(_tmpAB6->identity == - 1){const char*
_tmp140D;void*_tmp140C;(_tmp140C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp140D="fast_add_free_tvar_bool: bad identity in tvs2",
_tag_dyneither(_tmp140D,sizeof(char),46))),_tag_dyneither(_tmp140C,sizeof(void*),
0)));}if(_tmpAB6->identity == tv->identity){*_tmpAB8=*_tmpAB8  || b;return tvs;}}}{
struct _tuple30*_tmp1410;struct Cyc_List_List*_tmp140F;return(_tmp140F=
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
_tmp1415)),((_tmp1415->hd=tv,((_tmp1415->tl=tvs,_tmp1415)))));};}struct _tuple31{
void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct
_RegionHandle*r,struct Cyc_List_List*es,void*e,int b){void*_tmpAC1=Cyc_Tcutil_compress(
e);int _tmpAC3;_LL53B: {struct Cyc_Absyn_Evar_struct*_tmpAC2=(struct Cyc_Absyn_Evar_struct*)
_tmpAC1;if(_tmpAC2->tag != 1)goto _LL53D;else{_tmpAC3=_tmpAC2->f3;}}_LL53C:{struct
Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){struct _tuple31 _tmpAC5;void*
_tmpAC6;int _tmpAC7;int*_tmpAC8;struct _tuple31*_tmpAC4=(struct _tuple31*)es2->hd;
_tmpAC5=*_tmpAC4;_tmpAC6=_tmpAC5.f1;_tmpAC7=_tmpAC5.f2;_tmpAC8=(int*)&(*_tmpAC4).f2;{
void*_tmpAC9=Cyc_Tcutil_compress(_tmpAC6);int _tmpACB;_LL540: {struct Cyc_Absyn_Evar_struct*
_tmpACA=(struct Cyc_Absyn_Evar_struct*)_tmpAC9;if(_tmpACA->tag != 1)goto _LL542;
else{_tmpACB=_tmpACA->f3;}}_LL541: if(_tmpAC3 == _tmpACB){if(b != *_tmpAC8)*_tmpAC8=
1;return es;}goto _LL53F;_LL542:;_LL543: goto _LL53F;_LL53F:;};}}{struct _tuple31*
_tmp1418;struct Cyc_List_List*_tmp1417;return(_tmp1417=_region_malloc(r,sizeof(*
_tmp1417)),((_tmp1417->hd=((_tmp1418=_region_malloc(r,sizeof(*_tmp1418)),((
_tmp1418->f1=e,((_tmp1418->f2=b,_tmp1418)))))),((_tmp1417->tl=es,_tmp1417)))));};
_LL53D:;_LL53E: return es;_LL53A:;}static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=
btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((
struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){struct
Cyc_List_List*_tmp1419;r=((_tmp1419=_region_malloc(rgn,sizeof(*_tmp1419)),((
_tmp1419->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1419->tl=r,_tmp1419))))));}}r=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpAD0;int _tmpAD1;struct
_tuple30 _tmpACF=*((struct _tuple30*)tvs->hd);_tmpAD0=_tmpACF.f1;_tmpAD1=_tmpACF.f2;{
int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(_tmpAD0->identity
== ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){
struct Cyc_List_List*_tmp141A;res=((_tmp141A=_region_malloc(r,sizeof(*_tmp141A)),((
_tmp141A->hd=(struct _tuple30*)tvs->hd,((_tmp141A->tl=res,_tmp141A))))));}};}res=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);return res;}
void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){if(width != 
0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)width)){
const char*_tmp141E;void*_tmp141D[1];struct Cyc_String_pa_struct _tmp141C;(_tmp141C.tag=
0,((_tmp141C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp141D[0]=&
_tmp141C,Cyc_Tcutil_terr(loc,((_tmp141E="bitfield %s does not have constant width",
_tag_dyneither(_tmp141E,sizeof(char),41))),_tag_dyneither(_tmp141D,sizeof(void*),
1)))))));}else{unsigned int _tmpAD7;int _tmpAD8;struct _tuple14 _tmpAD6=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmpAD7=_tmpAD6.f1;_tmpAD8=_tmpAD6.f2;if(!_tmpAD8){
const char*_tmp1421;void*_tmp1420;(_tmp1420=0,Cyc_Tcutil_terr(loc,((_tmp1421="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmp1421,sizeof(char),45))),_tag_dyneither(_tmp1420,sizeof(void*),
0)));}w=_tmpAD7;}{void*_tmpADB=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of
_tmpADD;_LL545: {struct Cyc_Absyn_IntType_struct*_tmpADC=(struct Cyc_Absyn_IntType_struct*)
_tmpADB;if(_tmpADC->tag != 6)goto _LL547;else{_tmpADD=_tmpADC->f2;}}_LL546: switch(
_tmpADD){case Cyc_Absyn_Char_sz: _LL549: if(w > 8){const char*_tmp1424;void*_tmp1423;(
_tmp1423=0,Cyc_Tcutil_terr(loc,((_tmp1424="bitfield larger than type",
_tag_dyneither(_tmp1424,sizeof(char),26))),_tag_dyneither(_tmp1423,sizeof(void*),
0)));}break;case Cyc_Absyn_Short_sz: _LL54A: if(w > 16){const char*_tmp1427;void*
_tmp1426;(_tmp1426=0,Cyc_Tcutil_terr(loc,((_tmp1427="bitfield larger than type",
_tag_dyneither(_tmp1427,sizeof(char),26))),_tag_dyneither(_tmp1426,sizeof(void*),
0)));}break;case Cyc_Absyn_Long_sz: _LL54B: goto _LL54C;case Cyc_Absyn_Int_sz: _LL54C:
if(w > 32){const char*_tmp142A;void*_tmp1429;(_tmp1429=0,Cyc_Tcutil_terr(loc,((
_tmp142A="bitfield larger than type",_tag_dyneither(_tmp142A,sizeof(char),26))),
_tag_dyneither(_tmp1429,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz:
_LL54D: if(w > 64){const char*_tmp142D;void*_tmp142C;(_tmp142C=0,Cyc_Tcutil_terr(
loc,((_tmp142D="bitfield larger than type",_tag_dyneither(_tmp142D,sizeof(char),
26))),_tag_dyneither(_tmp142C,sizeof(void*),0)));}break;}goto _LL544;_LL547:;
_LL548:{const char*_tmp1432;void*_tmp1431[2];struct Cyc_String_pa_struct _tmp1430;
struct Cyc_String_pa_struct _tmp142F;(_tmp142F.tag=0,((_tmp142F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_typ)),((
_tmp1430.tag=0,((_tmp1430.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((
_tmp1431[0]=& _tmp1430,((_tmp1431[1]=& _tmp142F,Cyc_Tcutil_terr(loc,((_tmp1432="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmp1432,sizeof(char),52))),_tag_dyneither(_tmp1431,sizeof(void*),
2)))))))))))));}goto _LL544;_LL544:;};}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmpAEA=(void*)atts->hd;_LL550: {struct Cyc_Absyn_Packed_att_struct*
_tmpAEB=(struct Cyc_Absyn_Packed_att_struct*)_tmpAEA;if(_tmpAEB->tag != 7)goto
_LL552;}_LL551: continue;_LL552: {struct Cyc_Absyn_Aligned_att_struct*_tmpAEC=(
struct Cyc_Absyn_Aligned_att_struct*)_tmpAEA;if(_tmpAEC->tag != 6)goto _LL554;}
_LL553: continue;_LL554:;_LL555: {const char*_tmp1437;void*_tmp1436[2];struct Cyc_String_pa_struct
_tmp1435;struct Cyc_String_pa_struct _tmp1434;(_tmp1434.tag=0,((_tmp1434.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1435.tag=0,((_tmp1435.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)
atts->hd)),((_tmp1436[0]=& _tmp1435,((_tmp1436[1]=& _tmp1434,Cyc_Tcutil_terr(loc,((
_tmp1437="bad attribute %s on member %s",_tag_dyneither(_tmp1437,sizeof(char),30))),
_tag_dyneither(_tmp1436,sizeof(void*),2)))))))))))));}_LL54F:;}}struct Cyc_Tcutil_CVTEnv{
struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;
struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmpAF1=t;struct
Cyc_Absyn_Typedefdecl*_tmpAF3;void**_tmpAF4;_LL557: {struct Cyc_Absyn_TypedefType_struct*
_tmpAF2=(struct Cyc_Absyn_TypedefType_struct*)_tmpAF1;if(_tmpAF2->tag != 17)goto
_LL559;else{_tmpAF3=_tmpAF2->f3;_tmpAF4=_tmpAF2->f4;}}_LL558: if((((struct Cyc_Absyn_Typedefdecl*)
_check_null(_tmpAF3))->tq).real_const  || (_tmpAF3->tq).print_const){if(
declared_const){const char*_tmp143A;void*_tmp1439;(_tmp1439=0,Cyc_Tcutil_warn(loc,((
_tmp143A="extra const",_tag_dyneither(_tmp143A,sizeof(char),12))),_tag_dyneither(
_tmp1439,sizeof(void*),0)));}return 1;}if((unsigned int)_tmpAF4)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmpAF4);else{return declared_const;}_LL559:;_LL55A: return
declared_const;_LL556:;}static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Typedefdecl*td){if(td != 0){if(td->defn != 0){void*_tmpAF7=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);
struct Cyc_Absyn_PtrInfo _tmpAF9;struct Cyc_Absyn_PtrAtts _tmpAFA;void*_tmpAFB;
_LL55C: {struct Cyc_Absyn_PointerType_struct*_tmpAF8=(struct Cyc_Absyn_PointerType_struct*)
_tmpAF7;if(_tmpAF8->tag != 5)goto _LL55E;else{_tmpAF9=_tmpAF8->f1;_tmpAFA=_tmpAF9.ptr_atts;
_tmpAFB=_tmpAFA.rgn;}}_LL55D:{void*_tmpAFC=Cyc_Tcutil_compress(_tmpAFB);struct
Cyc_Absyn_Tvar*_tmpAFE;_LL561: {struct Cyc_Absyn_VarType_struct*_tmpAFD=(struct
Cyc_Absyn_VarType_struct*)_tmpAFC;if(_tmpAFD->tag != 2)goto _LL563;else{_tmpAFE=
_tmpAFD->f1;}}_LL562: return Cyc_Absyn_tvar_cmp(tvar,_tmpAFE)== 0;_LL563:;_LL564:
goto _LL560;_LL560:;}goto _LL55B;_LL55E:;_LL55F: goto _LL55B;_LL55B:;}}else{return 1;}
return 0;}static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*
td){void*_tmpAFF=Cyc_Absyn_compress_kb(tvar->kind);struct Cyc_Absyn_Kind*_tmpB01;
struct Cyc_Absyn_Kind _tmpB02;enum Cyc_Absyn_KindQual _tmpB03;enum Cyc_Absyn_AliasQual
_tmpB04;struct Cyc_Absyn_Kind*_tmpB06;struct Cyc_Absyn_Kind _tmpB07;enum Cyc_Absyn_KindQual
_tmpB08;enum Cyc_Absyn_AliasQual _tmpB09;_LL566: {struct Cyc_Absyn_Less_kb_struct*
_tmpB00=(struct Cyc_Absyn_Less_kb_struct*)_tmpAFF;if(_tmpB00->tag != 2)goto _LL568;
else{_tmpB01=_tmpB00->f2;_tmpB02=*_tmpB01;_tmpB03=_tmpB02.kind;if(_tmpB03 != Cyc_Absyn_RgnKind)
goto _LL568;_tmpB04=_tmpB02.aliasqual;if(_tmpB04 != Cyc_Absyn_Top)goto _LL568;}}
_LL567: goto _LL569;_LL568: {struct Cyc_Absyn_Eq_kb_struct*_tmpB05=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpAFF;if(_tmpB05->tag != 0)goto _LL56A;else{_tmpB06=_tmpB05->f1;_tmpB07=*_tmpB06;
_tmpB08=_tmpB07.kind;if(_tmpB08 != Cyc_Absyn_RgnKind)goto _LL56A;_tmpB09=_tmpB07.aliasqual;
if(_tmpB09 != Cyc_Absyn_Top)goto _LL56A;}}_LL569: if(((expected_kind->kind == Cyc_Absyn_BoxKind
 || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind)
 && Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){if(expected_kind->aliasqual == 
Cyc_Absyn_Aliasable)return& Cyc_Tcutil_rk;else{if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}return& Cyc_Tcutil_trk;_LL56A:;_LL56B: return Cyc_Tcutil_tvar_kind(
tvar,def_kind);_LL565:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
struct _tuple32{enum Cyc_Absyn_Format_Type f1;void*f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int
put_in_effect){static struct Cyc_Core_Opt urgn={(void*)((void*)& Cyc_Absyn_UniqueRgn_val)};
static struct Cyc_Core_Opt hrgn={(void*)((void*)& Cyc_Absyn_HeapRgn_val)};{void*
_tmpB0A=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpB0D;struct Cyc_Core_Opt**
_tmpB0E;struct Cyc_Core_Opt*_tmpB0F;struct Cyc_Core_Opt**_tmpB10;struct Cyc_Absyn_Tvar*
_tmpB12;struct Cyc_Absyn_TypeDecl*_tmpB14;struct Cyc_Absyn_TypeDecl _tmpB15;void*
_tmpB16;void**_tmpB17;void***_tmpB18;struct Cyc_List_List*_tmpB1A;struct _tuple2*
_tmpB1C;struct Cyc_Absyn_Enumdecl*_tmpB1D;struct Cyc_Absyn_Enumdecl**_tmpB1E;
struct Cyc_Absyn_DatatypeInfo _tmpB20;union Cyc_Absyn_DatatypeInfoU _tmpB21;union Cyc_Absyn_DatatypeInfoU*
_tmpB22;struct Cyc_List_List*_tmpB23;struct Cyc_List_List**_tmpB24;struct Cyc_Absyn_DatatypeFieldInfo
_tmpB26;union Cyc_Absyn_DatatypeFieldInfoU _tmpB27;union Cyc_Absyn_DatatypeFieldInfoU*
_tmpB28;struct Cyc_List_List*_tmpB29;struct Cyc_Absyn_PtrInfo _tmpB2B;void*_tmpB2C;
struct Cyc_Absyn_Tqual _tmpB2D;struct Cyc_Absyn_Tqual*_tmpB2E;struct Cyc_Absyn_PtrAtts
_tmpB2F;void*_tmpB30;union Cyc_Absyn_Constraint*_tmpB31;union Cyc_Absyn_Constraint*
_tmpB32;union Cyc_Absyn_Constraint*_tmpB33;void*_tmpB35;struct Cyc_Absyn_Exp*
_tmpB37;struct Cyc_Absyn_ArrayInfo _tmpB3B;void*_tmpB3C;struct Cyc_Absyn_Tqual
_tmpB3D;struct Cyc_Absyn_Tqual*_tmpB3E;struct Cyc_Absyn_Exp*_tmpB3F;struct Cyc_Absyn_Exp**
_tmpB40;union Cyc_Absyn_Constraint*_tmpB41;struct Cyc_Position_Segment*_tmpB42;
struct Cyc_Absyn_FnInfo _tmpB44;struct Cyc_List_List*_tmpB45;struct Cyc_List_List**
_tmpB46;struct Cyc_Core_Opt*_tmpB47;struct Cyc_Core_Opt**_tmpB48;struct Cyc_Absyn_Tqual
_tmpB49;struct Cyc_Absyn_Tqual*_tmpB4A;void*_tmpB4B;struct Cyc_List_List*_tmpB4C;
int _tmpB4D;struct Cyc_Absyn_VarargInfo*_tmpB4E;struct Cyc_List_List*_tmpB4F;struct
Cyc_List_List*_tmpB50;struct Cyc_List_List*_tmpB52;enum Cyc_Absyn_AggrKind _tmpB54;
struct Cyc_List_List*_tmpB55;struct Cyc_Absyn_AggrInfo _tmpB57;union Cyc_Absyn_AggrInfoU
_tmpB58;union Cyc_Absyn_AggrInfoU*_tmpB59;struct Cyc_List_List*_tmpB5A;struct Cyc_List_List**
_tmpB5B;struct _tuple2*_tmpB5D;struct Cyc_List_List*_tmpB5E;struct Cyc_List_List**
_tmpB5F;struct Cyc_Absyn_Typedefdecl*_tmpB60;struct Cyc_Absyn_Typedefdecl**_tmpB61;
void**_tmpB62;void***_tmpB63;void*_tmpB68;void*_tmpB6A;void*_tmpB6B;void*_tmpB6D;
void*_tmpB6F;struct Cyc_List_List*_tmpB71;_LL56D: {struct Cyc_Absyn_VoidType_struct*
_tmpB0B=(struct Cyc_Absyn_VoidType_struct*)_tmpB0A;if(_tmpB0B->tag != 0)goto _LL56F;}
_LL56E: goto _LL56C;_LL56F: {struct Cyc_Absyn_Evar_struct*_tmpB0C=(struct Cyc_Absyn_Evar_struct*)
_tmpB0A;if(_tmpB0C->tag != 1)goto _LL571;else{_tmpB0D=_tmpB0C->f1;_tmpB0E=(struct
Cyc_Core_Opt**)& _tmpB0C->f1;_tmpB0F=_tmpB0C->f2;_tmpB10=(struct Cyc_Core_Opt**)&
_tmpB0C->f2;}}_LL570: if(*_tmpB0E == 0  || Cyc_Tcutil_kind_leq(expected_kind,(
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmpB0E))->v) && !Cyc_Tcutil_kind_leq((
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmpB0E))->v,
expected_kind))*_tmpB0E=Cyc_Tcutil_kind_to_opt(expected_kind);if((cvtenv.fn_result
 && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){if(
expected_kind->aliasqual == Cyc_Absyn_Unique)*_tmpB10=(struct Cyc_Core_Opt*)& urgn;
else{*_tmpB10=(struct Cyc_Core_Opt*)& hrgn;}}else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_struct _tmp143D;struct Cyc_Absyn_Less_kb_struct*_tmp143C;
struct Cyc_Absyn_Tvar*_tmpB72=Cyc_Tcutil_new_tvar((void*)((_tmp143C=_cycalloc(
sizeof(*_tmp143C)),((_tmp143C[0]=((_tmp143D.tag=2,((_tmp143D.f1=0,((_tmp143D.f2=
expected_kind,_tmp143D)))))),_tmp143C)))));{struct Cyc_Absyn_VarType_struct*
_tmp1443;struct Cyc_Absyn_VarType_struct _tmp1442;struct Cyc_Core_Opt*_tmp1441;*
_tmpB10=((_tmp1441=_cycalloc(sizeof(*_tmp1441)),((_tmp1441->v=(void*)((_tmp1443=
_cycalloc(sizeof(*_tmp1443)),((_tmp1443[0]=((_tmp1442.tag=2,((_tmp1442.f1=
_tmpB72,_tmp1442)))),_tmp1443)))),_tmp1441))));}_tmpB12=_tmpB72;goto _LL572;}
else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,
put_in_effect);}}goto _LL56C;_LL571: {struct Cyc_Absyn_VarType_struct*_tmpB11=(
struct Cyc_Absyn_VarType_struct*)_tmpB0A;if(_tmpB11->tag != 2)goto _LL573;else{
_tmpB12=_tmpB11->f1;}}_LL572:{void*_tmpB78=Cyc_Absyn_compress_kb(_tmpB12->kind);
struct Cyc_Core_Opt*_tmpB7A;struct Cyc_Core_Opt**_tmpB7B;_LL5A4: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpB79=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpB78;if(_tmpB79->tag != 1)goto
_LL5A6;else{_tmpB7A=_tmpB79->f1;_tmpB7B=(struct Cyc_Core_Opt**)& _tmpB79->f1;}}
_LL5A5:{struct Cyc_Absyn_Less_kb_struct*_tmp1449;struct Cyc_Absyn_Less_kb_struct
_tmp1448;struct Cyc_Core_Opt*_tmp1447;*_tmpB7B=((_tmp1447=_cycalloc(sizeof(*
_tmp1447)),((_tmp1447->v=(void*)((_tmp1449=_cycalloc(sizeof(*_tmp1449)),((
_tmp1449[0]=((_tmp1448.tag=2,((_tmp1448.f1=0,((_tmp1448.f2=expected_kind,
_tmp1448)))))),_tmp1449)))),_tmp1447))));}goto _LL5A3;_LL5A6:;_LL5A7: goto _LL5A3;
_LL5A3:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpB12);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpB12,put_in_effect);{void*_tmpB7F=Cyc_Absyn_compress_kb(_tmpB12->kind);struct
Cyc_Core_Opt*_tmpB81;struct Cyc_Core_Opt**_tmpB82;struct Cyc_Absyn_Kind*_tmpB83;
_LL5A9: {struct Cyc_Absyn_Less_kb_struct*_tmpB80=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB7F;if(_tmpB80->tag != 2)goto _LL5AB;else{_tmpB81=_tmpB80->f1;_tmpB82=(struct
Cyc_Core_Opt**)& _tmpB80->f1;_tmpB83=_tmpB80->f2;}}_LL5AA: if(Cyc_Tcutil_kind_leq(
expected_kind,_tmpB83)){struct Cyc_Absyn_Less_kb_struct*_tmp144F;struct Cyc_Absyn_Less_kb_struct
_tmp144E;struct Cyc_Core_Opt*_tmp144D;*_tmpB82=((_tmp144D=_cycalloc(sizeof(*
_tmp144D)),((_tmp144D->v=(void*)((_tmp144F=_cycalloc(sizeof(*_tmp144F)),((
_tmp144F[0]=((_tmp144E.tag=2,((_tmp144E.f1=0,((_tmp144E.f2=expected_kind,
_tmp144E)))))),_tmp144F)))),_tmp144D))));}goto _LL5A8;_LL5AB:;_LL5AC: goto _LL5A8;
_LL5A8:;}goto _LL56C;_LL573: {struct Cyc_Absyn_TypeDeclType_struct*_tmpB13=(struct
Cyc_Absyn_TypeDeclType_struct*)_tmpB0A;if(_tmpB13->tag != 26)goto _LL575;else{
_tmpB14=_tmpB13->f1;_tmpB15=*_tmpB14;_tmpB16=_tmpB15.r;_tmpB17=_tmpB13->f2;
_tmpB18=(void***)& _tmpB13->f2;}}_LL574: {void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(
t));{void*_tmpB87=_tmpB16;struct Cyc_Absyn_Aggrdecl*_tmpB89;struct Cyc_Absyn_Enumdecl*
_tmpB8B;struct Cyc_Absyn_Datatypedecl*_tmpB8D;_LL5AE: {struct Cyc_Absyn_Aggr_td_struct*
_tmpB88=(struct Cyc_Absyn_Aggr_td_struct*)_tmpB87;if(_tmpB88->tag != 0)goto _LL5B0;
else{_tmpB89=_tmpB88->f1;}}_LL5AF: Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,
_tmpB89);goto _LL5AD;_LL5B0: {struct Cyc_Absyn_Enum_td_struct*_tmpB8A=(struct Cyc_Absyn_Enum_td_struct*)
_tmpB87;if(_tmpB8A->tag != 1)goto _LL5B2;else{_tmpB8B=_tmpB8A->f1;}}_LL5B1: Cyc_Tc_tcEnumdecl(
te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns),loc,_tmpB8B);goto _LL5AD;_LL5B2: {struct Cyc_Absyn_Datatype_td_struct*
_tmpB8C=(struct Cyc_Absyn_Datatype_td_struct*)_tmpB87;if(_tmpB8C->tag != 2)goto
_LL5AD;else{_tmpB8D=_tmpB8C->f1;}}_LL5B3: Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,
_tmpB8D);goto _LL5AD;_LL5AD:;}{void**_tmp1450;*_tmpB18=((_tmp1450=_cycalloc(
sizeof(*_tmp1450)),((_tmp1450[0]=new_t,_tmp1450))));}return Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL575: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpB19=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpB0A;if(_tmpB19->tag != 14)goto
_LL577;else{_tmpB1A=_tmpB19->f1;}}_LL576: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle*_tmpB8F=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;for(0;_tmpB1A != 0;_tmpB1A=_tmpB1A->tl){
struct Cyc_Absyn_Enumfield*_tmpB90=(struct Cyc_Absyn_Enumfield*)_tmpB1A->hd;if(((
int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmpB90->name).f2)){
const char*_tmp1454;void*_tmp1453[1];struct Cyc_String_pa_struct _tmp1452;(_tmp1452.tag=
0,((_tmp1452.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpB90->name).f2),((
_tmp1453[0]=& _tmp1452,Cyc_Tcutil_terr(_tmpB90->loc,((_tmp1454="duplicate enum field name %s",
_tag_dyneither(_tmp1454,sizeof(char),29))),_tag_dyneither(_tmp1453,sizeof(void*),
1)))))));}else{struct Cyc_List_List*_tmp1455;prev_fields=((_tmp1455=
_region_malloc(_tmpB8F,sizeof(*_tmp1455)),((_tmp1455->hd=(*_tmpB90->name).f2,((
_tmp1455->tl=prev_fields,_tmp1455))))));}if(_tmpB90->tag == 0)_tmpB90->tag=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmpB90->loc);else{if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(_tmpB90->tag))){const char*_tmp1459;void*
_tmp1458[1];struct Cyc_String_pa_struct _tmp1457;(_tmp1457.tag=0,((_tmp1457.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpB90->name).f2),((_tmp1458[0]=&
_tmp1457,Cyc_Tcutil_terr(loc,((_tmp1459="enum field %s: expression is not constant",
_tag_dyneither(_tmp1459,sizeof(char),42))),_tag_dyneither(_tmp1458,sizeof(void*),
1)))))));}}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmpB90->tag))).f1;tag_count=t1 + 1;(*_tmpB90->name).f1=Cyc_Absyn_Abs_n(
te->ns,0);{struct Cyc_Tcenv_AnonEnumRes_struct*_tmp145F;struct Cyc_Tcenv_AnonEnumRes_struct
_tmp145E;struct _tuple31*_tmp145D;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple31*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmpB90->name).f2,(struct _tuple31*)((_tmp145D=_cycalloc(sizeof(*_tmp145D)),((
_tmp145D->f1=(void*)((_tmp145F=_cycalloc(sizeof(*_tmp145F)),((_tmp145F[0]=((
_tmp145E.tag=4,((_tmp145E.f1=(void*)t,((_tmp145E.f2=_tmpB90,_tmp145E)))))),
_tmp145F)))),((_tmp145D->f2=1,_tmp145D)))))));};};}}goto _LL56C;}_LL577: {struct
Cyc_Absyn_EnumType_struct*_tmpB1B=(struct Cyc_Absyn_EnumType_struct*)_tmpB0A;if(
_tmpB1B->tag != 13)goto _LL579;else{_tmpB1C=_tmpB1B->f1;_tmpB1D=_tmpB1B->f2;
_tmpB1E=(struct Cyc_Absyn_Enumdecl**)& _tmpB1B->f2;}}_LL578: if(*_tmpB1E == 0  || ((
struct Cyc_Absyn_Enumdecl*)_check_null(*_tmpB1E))->fields == 0){struct
_handler_cons _tmpB9B;_push_handler(& _tmpB9B);{int _tmpB9D=0;if(setjmp(_tmpB9B.handler))
_tmpB9D=1;if(!_tmpB9D){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmpB1C);*_tmpB1E=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{
void*_tmpB9C=(void*)_exn_thrown;void*_tmpB9F=_tmpB9C;_LL5B5: {struct Cyc_Dict_Absent_struct*
_tmpBA0=(struct Cyc_Dict_Absent_struct*)_tmpB9F;if(_tmpBA0->tag != Cyc_Dict_Absent)
goto _LL5B7;}_LL5B6: {struct Cyc_Tcenv_Genv*_tmpBA1=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp1460;struct Cyc_Absyn_Enumdecl*_tmpBA2=(_tmp1460=
_cycalloc(sizeof(*_tmp1460)),((_tmp1460->sc=Cyc_Absyn_Extern,((_tmp1460->name=
_tmpB1C,((_tmp1460->fields=0,_tmp1460)))))));Cyc_Tc_tcEnumdecl(te,_tmpBA1,loc,
_tmpBA2);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmpB1C);*
_tmpB1E=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL5B4;};}_LL5B7:;_LL5B8:(void)_throw(
_tmpB9F);_LL5B4:;}};}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmpB1E);*_tmpB1C=(ed->name)[0];goto _LL56C;};_LL579: {struct Cyc_Absyn_DatatypeType_struct*
_tmpB1F=(struct Cyc_Absyn_DatatypeType_struct*)_tmpB0A;if(_tmpB1F->tag != 3)goto
_LL57B;else{_tmpB20=_tmpB1F->f1;_tmpB21=_tmpB20.datatype_info;_tmpB22=(union Cyc_Absyn_DatatypeInfoU*)&(
_tmpB1F->f1).datatype_info;_tmpB23=_tmpB20.targs;_tmpB24=(struct Cyc_List_List**)&(
_tmpB1F->f1).targs;}}_LL57A: {struct Cyc_List_List*_tmpBA4=*_tmpB24;{union Cyc_Absyn_DatatypeInfoU
_tmpBA5=*_tmpB22;struct Cyc_Absyn_UnknownDatatypeInfo _tmpBA6;struct _tuple2*
_tmpBA7;int _tmpBA8;struct Cyc_Absyn_Datatypedecl**_tmpBA9;struct Cyc_Absyn_Datatypedecl*
_tmpBAA;_LL5BA: if((_tmpBA5.UnknownDatatype).tag != 1)goto _LL5BC;_tmpBA6=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpBA5.UnknownDatatype).val;_tmpBA7=_tmpBA6.name;
_tmpBA8=_tmpBA6.is_extensible;_LL5BB: {struct Cyc_Absyn_Datatypedecl**tudp;{
struct _handler_cons _tmpBAB;_push_handler(& _tmpBAB);{int _tmpBAD=0;if(setjmp(
_tmpBAB.handler))_tmpBAD=1;if(!_tmpBAD){tudp=Cyc_Tcenv_lookup_datatypedecl(te,
loc,_tmpBA7);;_pop_handler();}else{void*_tmpBAC=(void*)_exn_thrown;void*_tmpBAF=
_tmpBAC;_LL5BF: {struct Cyc_Dict_Absent_struct*_tmpBB0=(struct Cyc_Dict_Absent_struct*)
_tmpBAF;if(_tmpBB0->tag != Cyc_Dict_Absent)goto _LL5C1;}_LL5C0: {struct Cyc_Tcenv_Genv*
_tmpBB1=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Datatypedecl*_tmp1461;struct Cyc_Absyn_Datatypedecl*
_tmpBB2=(_tmp1461=_cycalloc(sizeof(*_tmp1461)),((_tmp1461->sc=Cyc_Absyn_Extern,((
_tmp1461->name=_tmpBA7,((_tmp1461->tvs=0,((_tmp1461->fields=0,((_tmp1461->is_extensible=
_tmpBA8,_tmp1461)))))))))));Cyc_Tc_tcDatatypedecl(te,_tmpBB1,loc,_tmpBB2);tudp=
Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpBA7);if(_tmpBA4 != 0){{const char*
_tmp1465;void*_tmp1464[1];struct Cyc_String_pa_struct _tmp1463;(_tmp1463.tag=0,((
_tmp1463.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBA7)),((_tmp1464[0]=& _tmp1463,Cyc_Tcutil_terr(loc,((_tmp1465="declare parameterized datatype %s before using",
_tag_dyneither(_tmp1465,sizeof(char),47))),_tag_dyneither(_tmp1464,sizeof(void*),
1)))))));}return cvtenv;}goto _LL5BE;}_LL5C1:;_LL5C2:(void)_throw(_tmpBAF);_LL5BE:;}};}
if(_tmpBA8  && !(*tudp)->is_extensible){const char*_tmp1469;void*_tmp1468[1];
struct Cyc_String_pa_struct _tmp1467;(_tmp1467.tag=0,((_tmp1467.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBA7)),((
_tmp1468[0]=& _tmp1467,Cyc_Tcutil_terr(loc,((_tmp1469="datatype %s was not declared @extensible",
_tag_dyneither(_tmp1469,sizeof(char),41))),_tag_dyneither(_tmp1468,sizeof(void*),
1)))))));}*_tmpB22=Cyc_Absyn_KnownDatatype(tudp);_tmpBAA=*tudp;goto _LL5BD;}
_LL5BC: if((_tmpBA5.KnownDatatype).tag != 2)goto _LL5B9;_tmpBA9=(struct Cyc_Absyn_Datatypedecl**)(
_tmpBA5.KnownDatatype).val;_tmpBAA=*_tmpBA9;_LL5BD: {struct Cyc_List_List*tvs=
_tmpBAA->tvs;for(0;_tmpBA4 != 0  && tvs != 0;(_tmpBA4=_tmpBA4->tl,tvs=tvs->tl)){
void*t=(void*)_tmpBA4->hd;struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;{
struct _tuple0 _tmp146A;struct _tuple0 _tmpBBB=(_tmp146A.f1=Cyc_Absyn_compress_kb(tv->kind),((
_tmp146A.f2=t,_tmp146A)));void*_tmpBBC;void*_tmpBBE;struct Cyc_Absyn_Tvar*_tmpBC0;
_LL5C4: _tmpBBC=_tmpBBB.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpBBD=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpBBC;if(_tmpBBD->tag != 1)goto _LL5C6;};_tmpBBE=_tmpBBB.f2;{struct Cyc_Absyn_VarType_struct*
_tmpBBF=(struct Cyc_Absyn_VarType_struct*)_tmpBBE;if(_tmpBBF->tag != 2)goto _LL5C6;
else{_tmpBC0=_tmpBBF->f1;}};_LL5C5: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpBC0);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpBC0,1);continue;_LL5C6:;_LL5C7: goto _LL5C3;_LL5C3:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);Cyc_Tcutil_check_no_qual(loc,t);};}if(_tmpBA4 != 0){const
char*_tmp146E;void*_tmp146D[1];struct Cyc_String_pa_struct _tmp146C;(_tmp146C.tag=
0,((_tmp146C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBAA->name)),((_tmp146D[0]=& _tmp146C,Cyc_Tcutil_terr(loc,((_tmp146E="too many type arguments for datatype %s",
_tag_dyneither(_tmp146E,sizeof(char),40))),_tag_dyneither(_tmp146D,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_bk,expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*
_tmp146F;hidden_ts=((_tmp146F=_cycalloc(sizeof(*_tmp146F)),((_tmp146F->hd=e,((
_tmp146F->tl=hidden_ts,_tmp146F))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,k1,e,1);}*_tmpB24=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpB24,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}goto _LL5B9;}_LL5B9:;}goto
_LL56C;}_LL57B: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmpB25=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpB0A;if(_tmpB25->tag != 4)goto _LL57D;else{_tmpB26=_tmpB25->f1;_tmpB27=_tmpB26.field_info;
_tmpB28=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmpB25->f1).field_info;_tmpB29=
_tmpB26.targs;}}_LL57C:{union Cyc_Absyn_DatatypeFieldInfoU _tmpBC5=*_tmpB28;struct
Cyc_Absyn_UnknownDatatypeFieldInfo _tmpBC6;struct _tuple2*_tmpBC7;struct _tuple2*
_tmpBC8;int _tmpBC9;struct _tuple3 _tmpBCA;struct Cyc_Absyn_Datatypedecl*_tmpBCB;
struct Cyc_Absyn_Datatypefield*_tmpBCC;_LL5C9: if((_tmpBC5.UnknownDatatypefield).tag
!= 1)goto _LL5CB;_tmpBC6=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpBC5.UnknownDatatypefield).val;
_tmpBC7=_tmpBC6.datatype_name;_tmpBC8=_tmpBC6.field_name;_tmpBC9=_tmpBC6.is_extensible;
_LL5CA: {struct Cyc_Absyn_Datatypedecl*tud;struct Cyc_Absyn_Datatypefield*tuf;{
struct _handler_cons _tmpBCD;_push_handler(& _tmpBCD);{int _tmpBCF=0;if(setjmp(
_tmpBCD.handler))_tmpBCF=1;if(!_tmpBCF){*Cyc_Tcenv_lookup_datatypedecl(te,loc,
_tmpBC7);;_pop_handler();}else{void*_tmpBCE=(void*)_exn_thrown;void*_tmpBD1=
_tmpBCE;_LL5CE: {struct Cyc_Dict_Absent_struct*_tmpBD2=(struct Cyc_Dict_Absent_struct*)
_tmpBD1;if(_tmpBD2->tag != Cyc_Dict_Absent)goto _LL5D0;}_LL5CF:{const char*_tmp1473;
void*_tmp1472[1];struct Cyc_String_pa_struct _tmp1471;(_tmp1471.tag=0,((_tmp1471.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBC7)),((
_tmp1472[0]=& _tmp1471,Cyc_Tcutil_terr(loc,((_tmp1473="unbound datatype %s",
_tag_dyneither(_tmp1473,sizeof(char),20))),_tag_dyneither(_tmp1472,sizeof(void*),
1)))))));}return cvtenv;_LL5D0:;_LL5D1:(void)_throw(_tmpBD1);_LL5CD:;}};}{struct
_handler_cons _tmpBD6;_push_handler(& _tmpBD6);{int _tmpBD8=0;if(setjmp(_tmpBD6.handler))
_tmpBD8=1;if(!_tmpBD8){{struct _RegionHandle*_tmpBD9=Cyc_Tcenv_get_fnrgn(te);void*
_tmpBDA=Cyc_Tcenv_lookup_ordinary(_tmpBD9,te,loc,_tmpBC8);struct Cyc_Absyn_Datatypedecl*
_tmpBDC;struct Cyc_Absyn_Datatypefield*_tmpBDD;_LL5D3: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpBDB=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmpBDA;if(_tmpBDB->tag != 2)goto
_LL5D5;else{_tmpBDC=_tmpBDB->f1;_tmpBDD=_tmpBDB->f2;}}_LL5D4: tuf=_tmpBDD;tud=
_tmpBDC;if(_tmpBC9  && !tud->is_extensible){const char*_tmp1477;void*_tmp1476[1];
struct Cyc_String_pa_struct _tmp1475;(_tmp1475.tag=0,((_tmp1475.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBC7)),((
_tmp1476[0]=& _tmp1475,Cyc_Tcutil_terr(loc,((_tmp1477="datatype %s was not declared @extensible",
_tag_dyneither(_tmp1477,sizeof(char),41))),_tag_dyneither(_tmp1476,sizeof(void*),
1)))))));}goto _LL5D2;_LL5D5:;_LL5D6:{const char*_tmp147C;void*_tmp147B[2];struct
Cyc_String_pa_struct _tmp147A;struct Cyc_String_pa_struct _tmp1479;(_tmp1479.tag=0,((
_tmp1479.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBC7)),((_tmp147A.tag=0,((_tmp147A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBC8)),((_tmp147B[0]=& _tmp147A,((
_tmp147B[1]=& _tmp1479,Cyc_Tcutil_terr(loc,((_tmp147C="unbound field %s in type datatype %s",
_tag_dyneither(_tmp147C,sizeof(char),37))),_tag_dyneither(_tmp147B,sizeof(void*),
2)))))))))))));}{struct Cyc_Tcutil_CVTEnv _tmpBE5=cvtenv;_npop_handler(0);return
_tmpBE5;};_LL5D2:;};_pop_handler();}else{void*_tmpBD7=(void*)_exn_thrown;void*
_tmpBE7=_tmpBD7;_LL5D8: {struct Cyc_Dict_Absent_struct*_tmpBE8=(struct Cyc_Dict_Absent_struct*)
_tmpBE7;if(_tmpBE8->tag != Cyc_Dict_Absent)goto _LL5DA;}_LL5D9:{const char*_tmp1481;
void*_tmp1480[2];struct Cyc_String_pa_struct _tmp147F;struct Cyc_String_pa_struct
_tmp147E;(_tmp147E.tag=0,((_tmp147E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBC7)),((_tmp147F.tag=0,((_tmp147F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBC8)),((
_tmp1480[0]=& _tmp147F,((_tmp1480[1]=& _tmp147E,Cyc_Tcutil_terr(loc,((_tmp1481="unbound field %s in type datatype %s",
_tag_dyneither(_tmp1481,sizeof(char),37))),_tag_dyneither(_tmp1480,sizeof(void*),
2)))))))))))));}return cvtenv;_LL5DA:;_LL5DB:(void)_throw(_tmpBE7);_LL5D7:;}};}*
_tmpB28=Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmpBCB=tud;_tmpBCC=tuf;goto _LL5CC;}
_LL5CB: if((_tmpBC5.KnownDatatypefield).tag != 2)goto _LL5C8;_tmpBCA=(struct _tuple3)(
_tmpBC5.KnownDatatypefield).val;_tmpBCB=_tmpBCA.f1;_tmpBCC=_tmpBCA.f2;_LL5CC: {
struct Cyc_List_List*tvs=_tmpBCB->tvs;for(0;_tmpB29 != 0  && tvs != 0;(_tmpB29=
_tmpB29->tl,tvs=tvs->tl)){void*t=(void*)_tmpB29->hd;struct Cyc_Absyn_Tvar*tv=(
struct Cyc_Absyn_Tvar*)tvs->hd;{struct _tuple0 _tmp1482;struct _tuple0 _tmpBEE=(
_tmp1482.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1482.f2=t,_tmp1482)));void*
_tmpBEF;void*_tmpBF1;struct Cyc_Absyn_Tvar*_tmpBF3;_LL5DD: _tmpBEF=_tmpBEE.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmpBF0=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpBEF;if(_tmpBF0->tag != 1)goto _LL5DF;};_tmpBF1=_tmpBEE.f2;{struct Cyc_Absyn_VarType_struct*
_tmpBF2=(struct Cyc_Absyn_VarType_struct*)_tmpBF1;if(_tmpBF2->tag != 2)goto _LL5DF;
else{_tmpBF3=_tmpBF2->f1;}};_LL5DE: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpBF3);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpBF3,1);continue;_LL5DF:;_LL5E0: goto _LL5DC;_LL5DC:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);Cyc_Tcutil_check_no_qual(loc,t);};}if(_tmpB29 != 0){const
char*_tmp1487;void*_tmp1486[2];struct Cyc_String_pa_struct _tmp1485;struct Cyc_String_pa_struct
_tmp1484;(_tmp1484.tag=0,((_tmp1484.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBCC->name)),((_tmp1485.tag=0,((
_tmp1485.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBCB->name)),((_tmp1486[0]=& _tmp1485,((_tmp1486[1]=& _tmp1484,Cyc_Tcutil_terr(
loc,((_tmp1487="too many type arguments for datatype %s.%s",_tag_dyneither(
_tmp1487,sizeof(char),43))),_tag_dyneither(_tmp1486,sizeof(void*),2)))))))))))));}
if(tvs != 0){const char*_tmp148C;void*_tmp148B[2];struct Cyc_String_pa_struct
_tmp148A;struct Cyc_String_pa_struct _tmp1489;(_tmp1489.tag=0,((_tmp1489.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBCC->name)),((
_tmp148A.tag=0,((_tmp148A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBCB->name)),((_tmp148B[0]=& _tmp148A,((_tmp148B[1]=& _tmp1489,Cyc_Tcutil_terr(
loc,((_tmp148C="too few type arguments for datatype %s.%s",_tag_dyneither(
_tmp148C,sizeof(char),42))),_tag_dyneither(_tmp148B,sizeof(void*),2)))))))))))));}
goto _LL5C8;}_LL5C8:;}goto _LL56C;_LL57D: {struct Cyc_Absyn_PointerType_struct*
_tmpB2A=(struct Cyc_Absyn_PointerType_struct*)_tmpB0A;if(_tmpB2A->tag != 5)goto
_LL57F;else{_tmpB2B=_tmpB2A->f1;_tmpB2C=_tmpB2B.elt_typ;_tmpB2D=_tmpB2B.elt_tq;
_tmpB2E=(struct Cyc_Absyn_Tqual*)&(_tmpB2A->f1).elt_tq;_tmpB2F=_tmpB2B.ptr_atts;
_tmpB30=_tmpB2F.rgn;_tmpB31=_tmpB2F.nullable;_tmpB32=_tmpB2F.bounds;_tmpB33=
_tmpB2F.zero_term;}}_LL57E: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB2C,1);_tmpB2E->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpB2E->print_const,_tmpB2C);{struct Cyc_Absyn_Kind*k;switch(expected_kind->aliasqual){
case Cyc_Absyn_Aliasable: _LL5E1: k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique:
_LL5E2: k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL5E3: k=& Cyc_Tcutil_trk;break;}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmpB30,1);{union Cyc_Absyn_Constraint*
_tmpBFC=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
_tmpB33);union Cyc_Absyn_Constraint _tmpBFD;int _tmpBFE;union Cyc_Absyn_Constraint
_tmpBFF;int _tmpC00;_LL5E6: _tmpBFD=*_tmpBFC;if((_tmpBFD.No_constr).tag != 3)goto
_LL5E8;_tmpBFE=(int)(_tmpBFD.No_constr).val;_LL5E7:{void*_tmpC01=Cyc_Tcutil_compress(
_tmpB2C);enum Cyc_Absyn_Size_of _tmpC03;_LL5ED: {struct Cyc_Absyn_IntType_struct*
_tmpC02=(struct Cyc_Absyn_IntType_struct*)_tmpC01;if(_tmpC02->tag != 6)goto _LL5EF;
else{_tmpC03=_tmpC02->f2;if(_tmpC03 != Cyc_Absyn_Char_sz)goto _LL5EF;}}_LL5EE:((
int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB33,Cyc_Absyn_true_conref);
is_zero_terminated=1;goto _LL5EC;_LL5EF:;_LL5F0:((int(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmpB33,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL5EC;
_LL5EC:;}goto _LL5E5;_LL5E8: _tmpBFF=*_tmpBFC;if((_tmpBFF.Eq_constr).tag != 1)goto
_LL5EA;_tmpC00=(int)(_tmpBFF.Eq_constr).val;if(_tmpC00 != 1)goto _LL5EA;_LL5E9: if(
!Cyc_Tcutil_admits_zero(_tmpB2C)){const char*_tmp1490;void*_tmp148F[1];struct Cyc_String_pa_struct
_tmp148E;(_tmp148E.tag=0,((_tmp148E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB2C)),((_tmp148F[0]=& _tmp148E,Cyc_Tcutil_terr(
loc,((_tmp1490="cannot have a pointer to zero-terminated %s elements",
_tag_dyneither(_tmp1490,sizeof(char),53))),_tag_dyneither(_tmp148F,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL5E5;_LL5EA:;_LL5EB: is_zero_terminated=0;
goto _LL5E5;_LL5E5:;}{void*_tmpC07=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpB32);struct Cyc_Absyn_Exp*
_tmpC0A;_LL5F2: {struct Cyc_Absyn_DynEither_b_struct*_tmpC08=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC07;if(_tmpC08->tag != 0)goto _LL5F4;}_LL5F3: goto _LL5F1;_LL5F4: {struct Cyc_Absyn_Upper_b_struct*
_tmpC09=(struct Cyc_Absyn_Upper_b_struct*)_tmpC07;if(_tmpC09->tag != 1)goto _LL5F1;
else{_tmpC0A=_tmpC09->f1;}}_LL5F5: {struct _RegionHandle*_tmpC0B=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpC0C=Cyc_Tcenv_allow_valueof(_tmpC0B,te);Cyc_Tcexp_tcExp(
_tmpC0C,0,_tmpC0A);}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC0A,te,
cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpC0A)){const char*_tmp1493;void*
_tmp1492;(_tmp1492=0,Cyc_Tcutil_terr(loc,((_tmp1493="pointer bounds expression is not an unsigned int",
_tag_dyneither(_tmp1493,sizeof(char),49))),_tag_dyneither(_tmp1492,sizeof(void*),
0)));}{unsigned int _tmpC10;int _tmpC11;struct _tuple14 _tmpC0F=Cyc_Evexp_eval_const_uint_exp(
_tmpC0A);_tmpC10=_tmpC0F.f1;_tmpC11=_tmpC0F.f2;if(is_zero_terminated  && (!
_tmpC11  || _tmpC10 < 1)){const char*_tmp1496;void*_tmp1495;(_tmp1495=0,Cyc_Tcutil_terr(
loc,((_tmp1496="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmp1496,sizeof(char),55))),_tag_dyneither(_tmp1495,sizeof(void*),
0)));}goto _LL5F1;};}_LL5F1:;}goto _LL56C;};}_LL57F: {struct Cyc_Absyn_TagType_struct*
_tmpB34=(struct Cyc_Absyn_TagType_struct*)_tmpB0A;if(_tmpB34->tag != 19)goto _LL581;
else{_tmpB35=(void*)_tmpB34->f1;}}_LL580: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpB35,1);goto _LL56C;_LL581: {struct Cyc_Absyn_ValueofType_struct*
_tmpB36=(struct Cyc_Absyn_ValueofType_struct*)_tmpB0A;if(_tmpB36->tag != 18)goto
_LL583;else{_tmpB37=_tmpB36->f1;}}_LL582: {struct _RegionHandle*_tmpC14=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpC15=Cyc_Tcenv_allow_valueof(_tmpC14,te);Cyc_Tcexp_tcExp(
_tmpC15,0,_tmpB37);}if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpB37)){const char*
_tmp1499;void*_tmp1498;(_tmp1498=0,Cyc_Tcutil_terr(loc,((_tmp1499="valueof_t requires an int expression",
_tag_dyneither(_tmp1499,sizeof(char),37))),_tag_dyneither(_tmp1498,sizeof(void*),
0)));}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB37,te,cvtenv);goto
_LL56C;}_LL583: {struct Cyc_Absyn_IntType_struct*_tmpB38=(struct Cyc_Absyn_IntType_struct*)
_tmpB0A;if(_tmpB38->tag != 6)goto _LL585;}_LL584: goto _LL586;_LL585: {struct Cyc_Absyn_FloatType_struct*
_tmpB39=(struct Cyc_Absyn_FloatType_struct*)_tmpB0A;if(_tmpB39->tag != 7)goto
_LL587;}_LL586: goto _LL56C;_LL587: {struct Cyc_Absyn_ArrayType_struct*_tmpB3A=(
struct Cyc_Absyn_ArrayType_struct*)_tmpB0A;if(_tmpB3A->tag != 8)goto _LL589;else{
_tmpB3B=_tmpB3A->f1;_tmpB3C=_tmpB3B.elt_type;_tmpB3D=_tmpB3B.tq;_tmpB3E=(struct
Cyc_Absyn_Tqual*)&(_tmpB3A->f1).tq;_tmpB3F=_tmpB3B.num_elts;_tmpB40=(struct Cyc_Absyn_Exp**)&(
_tmpB3A->f1).num_elts;_tmpB41=_tmpB3B.zero_term;_tmpB42=_tmpB3B.zt_loc;}}_LL588: {
struct Cyc_Absyn_Exp*_tmpC18=*_tmpB40;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,& Cyc_Tcutil_tmk,_tmpB3C,1);_tmpB3E->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpB3E->print_const,_tmpB3C);{int is_zero_terminated;{union Cyc_Absyn_Constraint*
_tmpC19=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
_tmpB41);union Cyc_Absyn_Constraint _tmpC1A;int _tmpC1B;union Cyc_Absyn_Constraint
_tmpC1C;int _tmpC1D;_LL5F7: _tmpC1A=*_tmpC19;if((_tmpC1A.No_constr).tag != 3)goto
_LL5F9;_tmpC1B=(int)(_tmpC1A.No_constr).val;_LL5F8:((int(*)(int(*cmp)(int,int),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmpB41,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL5F6;
_LL5F9: _tmpC1C=*_tmpC19;if((_tmpC1C.Eq_constr).tag != 1)goto _LL5FB;_tmpC1D=(int)(
_tmpC1C.Eq_constr).val;if(_tmpC1D != 1)goto _LL5FB;_LL5FA: if(!Cyc_Tcutil_admits_zero(
_tmpB3C)){const char*_tmp149D;void*_tmp149C[1];struct Cyc_String_pa_struct _tmp149B;(
_tmp149B.tag=0,((_tmp149B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmpB3C)),((_tmp149C[0]=& _tmp149B,Cyc_Tcutil_terr(loc,((_tmp149D="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmp149D,sizeof(char),51))),_tag_dyneither(_tmp149C,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL5F6;_LL5FB:;_LL5FC: is_zero_terminated=0;
goto _LL5F6;_LL5F6:;}if(_tmpC18 == 0){if(is_zero_terminated)*_tmpB40=(_tmpC18=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{{const char*_tmp14A0;void*
_tmp149F;(_tmp149F=0,Cyc_Tcutil_warn(loc,((_tmp14A0="array bound defaults to 1 here",
_tag_dyneither(_tmp14A0,sizeof(char),31))),_tag_dyneither(_tmp149F,sizeof(void*),
0)));}*_tmpB40=(_tmpC18=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmpC18);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_tmpC18)){const char*_tmp14A3;void*_tmp14A2;(_tmp14A2=0,Cyc_Tcutil_terr(loc,((
_tmp14A3="array bounds expression is not constant",_tag_dyneither(_tmp14A3,
sizeof(char),40))),_tag_dyneither(_tmp14A2,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_uint_typ(
te,(struct Cyc_Absyn_Exp*)_tmpC18)){const char*_tmp14A6;void*_tmp14A5;(_tmp14A5=0,
Cyc_Tcutil_terr(loc,((_tmp14A6="array bounds expression is not an unsigned int",
_tag_dyneither(_tmp14A6,sizeof(char),47))),_tag_dyneither(_tmp14A5,sizeof(void*),
0)));}{unsigned int _tmpC28;int _tmpC29;struct _tuple14 _tmpC27=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmpC18);_tmpC28=_tmpC27.f1;_tmpC29=_tmpC27.f2;if((
is_zero_terminated  && _tmpC29) && _tmpC28 < 1){const char*_tmp14A9;void*_tmp14A8;(
_tmp14A8=0,Cyc_Tcutil_warn(loc,((_tmp14A9="zero terminated array cannot have zero elements",
_tag_dyneither(_tmp14A9,sizeof(char),48))),_tag_dyneither(_tmp14A8,sizeof(void*),
0)));}if((_tmpC29  && _tmpC28 < 1) && Cyc_Cyclone_tovc_r){{const char*_tmp14AC;void*
_tmp14AB;(_tmp14AB=0,Cyc_Tcutil_warn(loc,((_tmp14AC="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmp14AC,sizeof(char),75))),_tag_dyneither(_tmp14AB,sizeof(void*),
0)));}*_tmpB40=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL56C;};};}
_LL589: {struct Cyc_Absyn_FnType_struct*_tmpB43=(struct Cyc_Absyn_FnType_struct*)
_tmpB0A;if(_tmpB43->tag != 9)goto _LL58B;else{_tmpB44=_tmpB43->f1;_tmpB45=_tmpB44.tvars;
_tmpB46=(struct Cyc_List_List**)&(_tmpB43->f1).tvars;_tmpB47=_tmpB44.effect;
_tmpB48=(struct Cyc_Core_Opt**)&(_tmpB43->f1).effect;_tmpB49=_tmpB44.ret_tqual;
_tmpB4A=(struct Cyc_Absyn_Tqual*)&(_tmpB43->f1).ret_tqual;_tmpB4B=_tmpB44.ret_typ;
_tmpB4C=_tmpB44.args;_tmpB4D=_tmpB44.c_varargs;_tmpB4E=_tmpB44.cyc_varargs;
_tmpB4F=_tmpB44.rgn_po;_tmpB50=_tmpB44.attributes;}}_LL58A: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;enum Cyc_Absyn_Format_Type
ft=Cyc_Absyn_Printf_ft;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmpB50 != 0;
_tmpB50=_tmpB50->tl){if(!Cyc_Absyn_fntype_att((void*)_tmpB50->hd)){const char*
_tmp14B0;void*_tmp14AF[1];struct Cyc_String_pa_struct _tmp14AE;(_tmp14AE.tag=0,((
_tmp14AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmpB50->hd)),((_tmp14AF[0]=& _tmp14AE,Cyc_Tcutil_terr(loc,((_tmp14B0="bad function type attribute %s",
_tag_dyneither(_tmp14B0,sizeof(char),31))),_tag_dyneither(_tmp14AF,sizeof(void*),
1)))))));}{void*_tmpC31=(void*)_tmpB50->hd;enum Cyc_Absyn_Format_Type _tmpC36;int
_tmpC37;int _tmpC38;_LL5FE: {struct Cyc_Absyn_Stdcall_att_struct*_tmpC32=(struct
Cyc_Absyn_Stdcall_att_struct*)_tmpC31;if(_tmpC32->tag != 1)goto _LL600;}_LL5FF: if(
!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL5FD;_LL600: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpC33=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpC31;if(_tmpC33->tag != 2)goto
_LL602;}_LL601: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL5FD;_LL602: {
struct Cyc_Absyn_Fastcall_att_struct*_tmpC34=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpC31;if(_tmpC34->tag != 3)goto _LL604;}_LL603: if(!seen_fastcall){seen_fastcall=
1;++ num_convs;}goto _LL5FD;_LL604: {struct Cyc_Absyn_Format_att_struct*_tmpC35=(
struct Cyc_Absyn_Format_att_struct*)_tmpC31;if(_tmpC35->tag != 19)goto _LL606;else{
_tmpC36=_tmpC35->f1;_tmpC37=_tmpC35->f2;_tmpC38=_tmpC35->f3;}}_LL605: if(!
seen_format){seen_format=1;ft=_tmpC36;fmt_desc_arg=_tmpC37;fmt_arg_start=_tmpC38;}
else{const char*_tmp14B3;void*_tmp14B2;(_tmp14B2=0,Cyc_Tcutil_terr(loc,((_tmp14B3="function can't have multiple format attributes",
_tag_dyneither(_tmp14B3,sizeof(char),47))),_tag_dyneither(_tmp14B2,sizeof(void*),
0)));}goto _LL5FD;_LL606:;_LL607: goto _LL5FD;_LL5FD:;};}if(num_convs > 1){const char*
_tmp14B6;void*_tmp14B5;(_tmp14B5=0,Cyc_Tcutil_terr(loc,((_tmp14B6="function can't have multiple calling conventions",
_tag_dyneither(_tmp14B6,sizeof(char),49))),_tag_dyneither(_tmp14B5,sizeof(void*),
0)));}Cyc_Tcutil_check_unique_tvars(loc,*_tmpB46);{struct Cyc_List_List*b=*
_tmpB46;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmpC3D=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);
struct Cyc_Absyn_Kind*_tmpC3F;struct Cyc_Absyn_Kind _tmpC40;enum Cyc_Absyn_KindQual
_tmpC41;_LL609: {struct Cyc_Absyn_Eq_kb_struct*_tmpC3E=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpC3D;if(_tmpC3E->tag != 0)goto _LL60B;else{_tmpC3F=_tmpC3E->f1;_tmpC40=*_tmpC3F;
_tmpC41=_tmpC40.kind;if(_tmpC41 != Cyc_Absyn_MemKind)goto _LL60B;}}_LL60A:{const
char*_tmp14BA;void*_tmp14B9[1];struct Cyc_String_pa_struct _tmp14B8;(_tmp14B8.tag=
0,((_tmp14B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name),((_tmp14B9[0]=& _tmp14B8,Cyc_Tcutil_terr(loc,((_tmp14BA="function attempts to abstract Mem type variable %s",
_tag_dyneither(_tmp14BA,sizeof(char),51))),_tag_dyneither(_tmp14B9,sizeof(void*),
1)))))));}goto _LL608;_LL60B:;_LL60C: goto _LL608;_LL608:;};}}{struct _RegionHandle*
_tmpC45=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcutil_CVTEnv _tmp14BB;struct Cyc_Tcutil_CVTEnv
_tmpC46=(_tmp14BB.r=_tmpC45,((_tmp14BB.kind_env=cvtenv.kind_env,((_tmp14BB.free_vars=
0,((_tmp14BB.free_evars=0,((_tmp14BB.generalize_evars=cvtenv.generalize_evars,((
_tmp14BB.fn_result=1,_tmp14BB)))))))))));_tmpC46=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC46,& Cyc_Tcutil_tmk,_tmpB4B,1);_tmpB4A->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpB4A->print_const,_tmpB4B);_tmpC46.fn_result=0;{struct Cyc_List_List*a=
_tmpB4C;for(0;a != 0;a=a->tl){struct _tuple10*_tmpC47=(struct _tuple10*)a->hd;void*
_tmpC48=(*_tmpC47).f3;_tmpC46=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC46,& Cyc_Tcutil_tmk,
_tmpC48,1);{int _tmpC49=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpC47).f2).print_const,
_tmpC48);((*_tmpC47).f2).real_const=_tmpC49;{void*_tmpC4A=Cyc_Tcutil_compress(
_tmpC48);struct Cyc_Absyn_ArrayInfo _tmpC4C;void*_tmpC4D;struct Cyc_Absyn_Tqual
_tmpC4E;struct Cyc_Absyn_Exp*_tmpC4F;union Cyc_Absyn_Constraint*_tmpC50;struct Cyc_Position_Segment*
_tmpC51;_LL60E: {struct Cyc_Absyn_ArrayType_struct*_tmpC4B=(struct Cyc_Absyn_ArrayType_struct*)
_tmpC4A;if(_tmpC4B->tag != 8)goto _LL610;else{_tmpC4C=_tmpC4B->f1;_tmpC4D=_tmpC4C.elt_type;
_tmpC4E=_tmpC4C.tq;_tmpC4F=_tmpC4C.num_elts;_tmpC50=_tmpC4C.zero_term;_tmpC51=
_tmpC4C.zt_loc;}}_LL60F: {void*_tmpC52=Cyc_Absyn_new_evar(0,0);_tmpC46=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC46,& Cyc_Tcutil_rk,_tmpC52,1);{struct Cyc_Absyn_Upper_b_struct _tmp14BE;
struct Cyc_Absyn_Upper_b_struct*_tmp14BD;void*_tmpC53=Cyc_Absyn_atb_typ(_tmpC4D,
_tmpC52,_tmpC4E,(void*)((_tmp14BD=_cycalloc(sizeof(*_tmp14BD)),((_tmp14BD[0]=((
_tmp14BE.tag=1,((_tmp14BE.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpC4F),_tmp14BE)))),
_tmp14BD)))),_tmpC50);(*_tmpC47).f3=_tmpC53;goto _LL60D;};}_LL610:;_LL611: goto
_LL60D;_LL60D:;};};}}if(_tmpB4E != 0){if(_tmpB4D){const char*_tmp14C1;void*
_tmp14C0;(_tmp14C0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp14C1="both c_vararg and cyc_vararg",_tag_dyneither(
_tmp14C1,sizeof(char),29))),_tag_dyneither(_tmp14C0,sizeof(void*),0)));}{void*
_tmpC59;int _tmpC5A;struct Cyc_Absyn_VarargInfo _tmpC58=*_tmpB4E;_tmpC59=_tmpC58.type;
_tmpC5A=_tmpC58.inject;_tmpC46=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC46,& Cyc_Tcutil_tmk,
_tmpC59,1);(_tmpB4E->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(
_tmpB4E->tq).print_const,_tmpC59);if(_tmpC5A){void*_tmpC5B=Cyc_Tcutil_compress(
_tmpC59);struct Cyc_Absyn_PtrInfo _tmpC5D;void*_tmpC5E;struct Cyc_Absyn_PtrAtts
_tmpC5F;union Cyc_Absyn_Constraint*_tmpC60;union Cyc_Absyn_Constraint*_tmpC61;
_LL613: {struct Cyc_Absyn_PointerType_struct*_tmpC5C=(struct Cyc_Absyn_PointerType_struct*)
_tmpC5B;if(_tmpC5C->tag != 5)goto _LL615;else{_tmpC5D=_tmpC5C->f1;_tmpC5E=_tmpC5D.elt_typ;
_tmpC5F=_tmpC5D.ptr_atts;_tmpC60=_tmpC5F.bounds;_tmpC61=_tmpC5F.zero_term;}}
_LL614:{void*_tmpC62=Cyc_Tcutil_compress(_tmpC5E);_LL618: {struct Cyc_Absyn_DatatypeType_struct*
_tmpC63=(struct Cyc_Absyn_DatatypeType_struct*)_tmpC62;if(_tmpC63->tag != 3)goto
_LL61A;}_LL619: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,_tmpC61)){const char*_tmp14C4;void*_tmp14C3;(_tmp14C3=0,Cyc_Tcutil_terr(loc,((
_tmp14C4="can't inject into a zeroterm pointer",_tag_dyneither(_tmp14C4,sizeof(
char),37))),_tag_dyneither(_tmp14C3,sizeof(void*),0)));}{void*_tmpC66=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,
_tmpC60);_LL61D: {struct Cyc_Absyn_DynEither_b_struct*_tmpC67=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC66;if(_tmpC67->tag != 0)goto _LL61F;}_LL61E:{const char*_tmp14C7;void*_tmp14C6;(
_tmp14C6=0,Cyc_Tcutil_terr(loc,((_tmp14C7="can't inject into a fat pointer to datatype",
_tag_dyneither(_tmp14C7,sizeof(char),44))),_tag_dyneither(_tmp14C6,sizeof(void*),
0)));}goto _LL61C;_LL61F:;_LL620: goto _LL61C;_LL61C:;}goto _LL617;_LL61A:;_LL61B:{
const char*_tmp14CA;void*_tmp14C9;(_tmp14C9=0,Cyc_Tcutil_terr(loc,((_tmp14CA="can't inject a non-datatype type",
_tag_dyneither(_tmp14CA,sizeof(char),33))),_tag_dyneither(_tmp14C9,sizeof(void*),
0)));}goto _LL617;_LL617:;}goto _LL612;_LL615:;_LL616:{const char*_tmp14CD;void*
_tmp14CC;(_tmp14CC=0,Cyc_Tcutil_terr(loc,((_tmp14CD="expecting a datatype pointer type",
_tag_dyneither(_tmp14CD,sizeof(char),34))),_tag_dyneither(_tmp14CC,sizeof(void*),
0)));}goto _LL612;_LL612:;}};}if(seen_format){int _tmpC6E=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmpB4C);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpC6E) || 
fmt_arg_start < 0) || _tmpB4E == 0  && fmt_arg_start != 0) || _tmpB4E != 0  && 
fmt_arg_start != _tmpC6E + 1){const char*_tmp14D0;void*_tmp14CF;(_tmp14CF=0,Cyc_Tcutil_terr(
loc,((_tmp14D0="bad format descriptor",_tag_dyneither(_tmp14D0,sizeof(char),22))),
_tag_dyneither(_tmp14CF,sizeof(void*),0)));}else{void*_tmpC72;struct _tuple10
_tmpC71=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpB4C,
fmt_desc_arg - 1);_tmpC72=_tmpC71.f3;{void*_tmpC73=Cyc_Tcutil_compress(_tmpC72);
struct Cyc_Absyn_PtrInfo _tmpC75;void*_tmpC76;struct Cyc_Absyn_PtrAtts _tmpC77;union
Cyc_Absyn_Constraint*_tmpC78;union Cyc_Absyn_Constraint*_tmpC79;_LL622: {struct
Cyc_Absyn_PointerType_struct*_tmpC74=(struct Cyc_Absyn_PointerType_struct*)
_tmpC73;if(_tmpC74->tag != 5)goto _LL624;else{_tmpC75=_tmpC74->f1;_tmpC76=_tmpC75.elt_typ;
_tmpC77=_tmpC75.ptr_atts;_tmpC78=_tmpC77.bounds;_tmpC79=_tmpC77.zero_term;}}
_LL623:{struct _tuple0 _tmp14D1;struct _tuple0 _tmpC7B=(_tmp14D1.f1=Cyc_Tcutil_compress(
_tmpC76),((_tmp14D1.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmpC78),_tmp14D1)));void*_tmpC7C;enum Cyc_Absyn_Sign
_tmpC7E;enum Cyc_Absyn_Size_of _tmpC7F;void*_tmpC80;_LL627: _tmpC7C=_tmpC7B.f1;{
struct Cyc_Absyn_IntType_struct*_tmpC7D=(struct Cyc_Absyn_IntType_struct*)_tmpC7C;
if(_tmpC7D->tag != 6)goto _LL629;else{_tmpC7E=_tmpC7D->f1;if(_tmpC7E != Cyc_Absyn_None)
goto _LL629;_tmpC7F=_tmpC7D->f2;if(_tmpC7F != Cyc_Absyn_Char_sz)goto _LL629;}};
_tmpC80=_tmpC7B.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmpC81=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC80;if(_tmpC81->tag != 0)goto _LL629;};_LL628: goto _LL626;_LL629:;_LL62A:{const
char*_tmp14D4;void*_tmp14D3;(_tmp14D3=0,Cyc_Tcutil_terr(loc,((_tmp14D4="format descriptor is not a char ? type",
_tag_dyneither(_tmp14D4,sizeof(char),39))),_tag_dyneither(_tmp14D3,sizeof(void*),
0)));}goto _LL626;_LL626:;}goto _LL621;_LL624:;_LL625:{const char*_tmp14D7;void*
_tmp14D6;(_tmp14D6=0,Cyc_Tcutil_terr(loc,((_tmp14D7="format descriptor is not a char ? type",
_tag_dyneither(_tmp14D7,sizeof(char),39))),_tag_dyneither(_tmp14D6,sizeof(void*),
0)));}goto _LL621;_LL621:;}if(fmt_arg_start != 0){int problem;{struct _tuple32
_tmp14D8;struct _tuple32 _tmpC87=(_tmp14D8.f1=ft,((_tmp14D8.f2=Cyc_Tcutil_compress(
Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmpB4E))->type)),
_tmp14D8)));enum Cyc_Absyn_Format_Type _tmpC88;void*_tmpC89;struct Cyc_Absyn_DatatypeInfo
_tmpC8B;union Cyc_Absyn_DatatypeInfoU _tmpC8C;struct Cyc_Absyn_Datatypedecl**
_tmpC8D;struct Cyc_Absyn_Datatypedecl*_tmpC8E;enum Cyc_Absyn_Format_Type _tmpC8F;
void*_tmpC90;struct Cyc_Absyn_DatatypeInfo _tmpC92;union Cyc_Absyn_DatatypeInfoU
_tmpC93;struct Cyc_Absyn_Datatypedecl**_tmpC94;struct Cyc_Absyn_Datatypedecl*
_tmpC95;_LL62C: _tmpC88=_tmpC87.f1;if(_tmpC88 != Cyc_Absyn_Printf_ft)goto _LL62E;
_tmpC89=_tmpC87.f2;{struct Cyc_Absyn_DatatypeType_struct*_tmpC8A=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpC89;if(_tmpC8A->tag != 3)goto _LL62E;else{_tmpC8B=_tmpC8A->f1;_tmpC8C=_tmpC8B.datatype_info;
if((_tmpC8C.KnownDatatype).tag != 2)goto _LL62E;_tmpC8D=(struct Cyc_Absyn_Datatypedecl**)(
_tmpC8C.KnownDatatype).val;_tmpC8E=*_tmpC8D;}};_LL62D: problem=Cyc_Absyn_qvar_cmp(
_tmpC8E->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL62B;_LL62E: _tmpC8F=
_tmpC87.f1;if(_tmpC8F != Cyc_Absyn_Scanf_ft)goto _LL630;_tmpC90=_tmpC87.f2;{struct
Cyc_Absyn_DatatypeType_struct*_tmpC91=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpC90;if(_tmpC91->tag != 3)goto _LL630;else{_tmpC92=_tmpC91->f1;_tmpC93=_tmpC92.datatype_info;
if((_tmpC93.KnownDatatype).tag != 2)goto _LL630;_tmpC94=(struct Cyc_Absyn_Datatypedecl**)(
_tmpC93.KnownDatatype).val;_tmpC95=*_tmpC94;}};_LL62F: problem=Cyc_Absyn_qvar_cmp(
_tmpC95->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL62B;_LL630:;_LL631:
problem=1;goto _LL62B;_LL62B:;}if(problem){const char*_tmp14DB;void*_tmp14DA;(
_tmp14DA=0,Cyc_Tcutil_terr(loc,((_tmp14DB="format attribute and vararg types don't match",
_tag_dyneither(_tmp14DB,sizeof(char),46))),_tag_dyneither(_tmp14DA,sizeof(void*),
0)));}}}}{struct Cyc_List_List*rpo=_tmpB4F;for(0;rpo != 0;rpo=rpo->tl){struct
_tuple0 _tmpC99;void*_tmpC9A;void*_tmpC9B;struct _tuple0*_tmpC98=(struct _tuple0*)
rpo->hd;_tmpC99=*_tmpC98;_tmpC9A=_tmpC99.f1;_tmpC9B=_tmpC99.f2;_tmpC46=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC46,& Cyc_Tcutil_ek,_tmpC9A,1);_tmpC46=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC46,& Cyc_Tcutil_trk,_tmpC9B,1);}}if(*_tmpB48 != 0)_tmpC46=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC46,& Cyc_Tcutil_ek,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmpB48))->v,
1);else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmpC46.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpC9D;int _tmpC9E;struct
_tuple30 _tmpC9C=*((struct _tuple30*)tvs->hd);_tmpC9D=_tmpC9C.f1;_tmpC9E=_tmpC9C.f2;
if(!_tmpC9E)continue;{void*_tmpC9F=Cyc_Absyn_compress_kb(_tmpC9D->kind);struct
Cyc_Core_Opt*_tmpCA1;struct Cyc_Core_Opt**_tmpCA2;struct Cyc_Absyn_Kind*_tmpCA3;
struct Cyc_Absyn_Kind*_tmpCA5;struct Cyc_Absyn_Kind*_tmpCA7;struct Cyc_Absyn_Kind
_tmpCA8;enum Cyc_Absyn_KindQual _tmpCA9;struct Cyc_Absyn_Kind*_tmpCAB;struct Cyc_Absyn_Kind
_tmpCAC;enum Cyc_Absyn_KindQual _tmpCAD;struct Cyc_Core_Opt*_tmpCAF;struct Cyc_Core_Opt**
_tmpCB0;struct Cyc_Absyn_Kind*_tmpCB1;struct Cyc_Absyn_Kind _tmpCB2;enum Cyc_Absyn_KindQual
_tmpCB3;struct Cyc_Absyn_Kind*_tmpCB5;struct Cyc_Absyn_Kind _tmpCB6;enum Cyc_Absyn_KindQual
_tmpCB7;struct Cyc_Core_Opt*_tmpCB9;struct Cyc_Core_Opt**_tmpCBA;_LL633:{struct Cyc_Absyn_Less_kb_struct*
_tmpCA0=(struct Cyc_Absyn_Less_kb_struct*)_tmpC9F;if(_tmpCA0->tag != 2)goto _LL635;
else{_tmpCA1=_tmpCA0->f1;_tmpCA2=(struct Cyc_Core_Opt**)& _tmpCA0->f1;_tmpCA3=
_tmpCA0->f2;}}if(!(_tmpCA3->kind == Cyc_Absyn_RgnKind))goto _LL635;_LL634: if(
_tmpCA3->aliasqual == Cyc_Absyn_Top){*_tmpCA2=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
_tmpCA5=_tmpCA3;goto _LL636;}*_tmpCA2=Cyc_Tcutil_kind_to_bound_opt(_tmpCA3);
_tmpCA5=_tmpCA3;goto _LL636;_LL635:{struct Cyc_Absyn_Eq_kb_struct*_tmpCA4=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpC9F;if(_tmpCA4->tag != 0)goto _LL637;else{_tmpCA5=
_tmpCA4->f1;}}if(!(_tmpCA5->kind == Cyc_Absyn_RgnKind))goto _LL637;_LL636:{struct
Cyc_Absyn_AccessEff_struct*_tmp14EA;struct Cyc_Absyn_VarType_struct*_tmp14E9;
struct Cyc_Absyn_VarType_struct _tmp14E8;struct Cyc_Absyn_AccessEff_struct _tmp14E7;
struct Cyc_List_List*_tmp14E6;effect=((_tmp14E6=_cycalloc(sizeof(*_tmp14E6)),((
_tmp14E6->hd=(void*)((_tmp14EA=_cycalloc(sizeof(*_tmp14EA)),((_tmp14EA[0]=((
_tmp14E7.tag=23,((_tmp14E7.f1=(void*)((void*)((_tmp14E9=_cycalloc(sizeof(*
_tmp14E9)),((_tmp14E9[0]=((_tmp14E8.tag=2,((_tmp14E8.f1=_tmpC9D,_tmp14E8)))),
_tmp14E9))))),_tmp14E7)))),_tmp14EA)))),((_tmp14E6->tl=effect,_tmp14E6))))));}
goto _LL632;_LL637: {struct Cyc_Absyn_Less_kb_struct*_tmpCA6=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC9F;if(_tmpCA6->tag != 2)goto _LL639;else{_tmpCA7=_tmpCA6->f2;_tmpCA8=*_tmpCA7;
_tmpCA9=_tmpCA8.kind;if(_tmpCA9 != Cyc_Absyn_IntKind)goto _LL639;}}_LL638: goto
_LL63A;_LL639: {struct Cyc_Absyn_Eq_kb_struct*_tmpCAA=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpC9F;if(_tmpCAA->tag != 0)goto _LL63B;else{_tmpCAB=_tmpCAA->f1;_tmpCAC=*_tmpCAB;
_tmpCAD=_tmpCAC.kind;if(_tmpCAD != Cyc_Absyn_IntKind)goto _LL63B;}}_LL63A: goto
_LL632;_LL63B: {struct Cyc_Absyn_Less_kb_struct*_tmpCAE=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC9F;if(_tmpCAE->tag != 2)goto _LL63D;else{_tmpCAF=_tmpCAE->f1;_tmpCB0=(struct
Cyc_Core_Opt**)& _tmpCAE->f1;_tmpCB1=_tmpCAE->f2;_tmpCB2=*_tmpCB1;_tmpCB3=_tmpCB2.kind;
if(_tmpCB3 != Cyc_Absyn_EffKind)goto _LL63D;}}_LL63C:*_tmpCB0=Cyc_Tcutil_kind_to_bound_opt(&
Cyc_Tcutil_ek);goto _LL63E;_LL63D: {struct Cyc_Absyn_Eq_kb_struct*_tmpCB4=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpC9F;if(_tmpCB4->tag != 0)goto _LL63F;else{_tmpCB5=
_tmpCB4->f1;_tmpCB6=*_tmpCB5;_tmpCB7=_tmpCB6.kind;if(_tmpCB7 != Cyc_Absyn_EffKind)
goto _LL63F;}}_LL63E:{struct Cyc_Absyn_VarType_struct*_tmp14F0;struct Cyc_Absyn_VarType_struct
_tmp14EF;struct Cyc_List_List*_tmp14EE;effect=((_tmp14EE=_cycalloc(sizeof(*
_tmp14EE)),((_tmp14EE->hd=(void*)((_tmp14F0=_cycalloc(sizeof(*_tmp14F0)),((
_tmp14F0[0]=((_tmp14EF.tag=2,((_tmp14EF.f1=_tmpC9D,_tmp14EF)))),_tmp14F0)))),((
_tmp14EE->tl=effect,_tmp14EE))))));}goto _LL632;_LL63F: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpCB8=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpC9F;if(_tmpCB8->tag != 1)goto
_LL641;else{_tmpCB9=_tmpCB8->f1;_tmpCBA=(struct Cyc_Core_Opt**)& _tmpCB8->f1;}}
_LL640:{struct Cyc_Absyn_Less_kb_struct*_tmp14F6;struct Cyc_Absyn_Less_kb_struct
_tmp14F5;struct Cyc_Core_Opt*_tmp14F4;*_tmpCBA=((_tmp14F4=_cycalloc(sizeof(*
_tmp14F4)),((_tmp14F4->v=(void*)((_tmp14F6=_cycalloc(sizeof(*_tmp14F6)),((
_tmp14F6[0]=((_tmp14F5.tag=2,((_tmp14F5.f1=0,((_tmp14F5.f2=& Cyc_Tcutil_ak,
_tmp14F5)))))),_tmp14F6)))),_tmp14F4))));}goto _LL642;_LL641:;_LL642:{struct Cyc_Absyn_RgnsEff_struct*
_tmp1505;struct Cyc_Absyn_VarType_struct*_tmp1504;struct Cyc_Absyn_VarType_struct
_tmp1503;struct Cyc_Absyn_RgnsEff_struct _tmp1502;struct Cyc_List_List*_tmp1501;
effect=((_tmp1501=_cycalloc(sizeof(*_tmp1501)),((_tmp1501->hd=(void*)((_tmp1505=
_cycalloc(sizeof(*_tmp1505)),((_tmp1505[0]=((_tmp1502.tag=25,((_tmp1502.f1=(void*)((
void*)((_tmp1504=_cycalloc(sizeof(*_tmp1504)),((_tmp1504[0]=((_tmp1503.tag=2,((
_tmp1503.f1=_tmpC9D,_tmp1503)))),_tmp1504))))),_tmp1502)))),_tmp1505)))),((
_tmp1501->tl=effect,_tmp1501))))));}goto _LL632;_LL632:;};}}{struct Cyc_List_List*
ts=_tmpC46.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmpCCC;int _tmpCCD;struct
_tuple31 _tmpCCB=*((struct _tuple31*)ts->hd);_tmpCCC=_tmpCCB.f1;_tmpCCD=_tmpCCB.f2;
if(!_tmpCCD)continue;{struct Cyc_Absyn_Kind*_tmpCCE=Cyc_Tcutil_typ_kind(_tmpCCC);
struct Cyc_Absyn_Kind _tmpCCF;enum Cyc_Absyn_KindQual _tmpCD0;struct Cyc_Absyn_Kind
_tmpCD1;enum Cyc_Absyn_KindQual _tmpCD2;struct Cyc_Absyn_Kind _tmpCD3;enum Cyc_Absyn_KindQual
_tmpCD4;_LL644: _tmpCCF=*_tmpCCE;_tmpCD0=_tmpCCF.kind;if(_tmpCD0 != Cyc_Absyn_RgnKind)
goto _LL646;_LL645:{struct Cyc_Absyn_AccessEff_struct*_tmp150B;struct Cyc_Absyn_AccessEff_struct
_tmp150A;struct Cyc_List_List*_tmp1509;effect=((_tmp1509=_cycalloc(sizeof(*
_tmp1509)),((_tmp1509->hd=(void*)((_tmp150B=_cycalloc(sizeof(*_tmp150B)),((
_tmp150B[0]=((_tmp150A.tag=23,((_tmp150A.f1=(void*)_tmpCCC,_tmp150A)))),_tmp150B)))),((
_tmp1509->tl=effect,_tmp1509))))));}goto _LL643;_LL646: _tmpCD1=*_tmpCCE;_tmpCD2=
_tmpCD1.kind;if(_tmpCD2 != Cyc_Absyn_EffKind)goto _LL648;_LL647:{struct Cyc_List_List*
_tmp150C;effect=((_tmp150C=_cycalloc(sizeof(*_tmp150C)),((_tmp150C->hd=_tmpCCC,((
_tmp150C->tl=effect,_tmp150C))))));}goto _LL643;_LL648: _tmpCD3=*_tmpCCE;_tmpCD4=
_tmpCD3.kind;if(_tmpCD4 != Cyc_Absyn_IntKind)goto _LL64A;_LL649: goto _LL643;_LL64A:;
_LL64B:{struct Cyc_Absyn_RgnsEff_struct*_tmp1512;struct Cyc_Absyn_RgnsEff_struct
_tmp1511;struct Cyc_List_List*_tmp1510;effect=((_tmp1510=_cycalloc(sizeof(*
_tmp1510)),((_tmp1510->hd=(void*)((_tmp1512=_cycalloc(sizeof(*_tmp1512)),((
_tmp1512[0]=((_tmp1511.tag=25,((_tmp1511.f1=(void*)_tmpCCC,_tmp1511)))),_tmp1512)))),((
_tmp1510->tl=effect,_tmp1510))))));}goto _LL643;_LL643:;};}}{struct Cyc_Absyn_JoinEff_struct*
_tmp1518;struct Cyc_Absyn_JoinEff_struct _tmp1517;struct Cyc_Core_Opt*_tmp1516;*
_tmpB48=((_tmp1516=_cycalloc(sizeof(*_tmp1516)),((_tmp1516->v=(void*)((_tmp1518=
_cycalloc(sizeof(*_tmp1518)),((_tmp1518[0]=((_tmp1517.tag=24,((_tmp1517.f1=
effect,_tmp1517)))),_tmp1518)))),_tmp1516))));};}if(*_tmpB46 != 0){struct Cyc_List_List*
bs=*_tmpB46;for(0;bs != 0;bs=bs->tl){void*_tmpCDF=Cyc_Absyn_compress_kb(((struct
Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmpCE1;struct Cyc_Core_Opt**
_tmpCE2;struct Cyc_Core_Opt*_tmpCE4;struct Cyc_Core_Opt**_tmpCE5;struct Cyc_Absyn_Kind*
_tmpCE6;struct Cyc_Absyn_Kind _tmpCE7;enum Cyc_Absyn_KindQual _tmpCE8;enum Cyc_Absyn_AliasQual
_tmpCE9;struct Cyc_Core_Opt*_tmpCEB;struct Cyc_Core_Opt**_tmpCEC;struct Cyc_Absyn_Kind*
_tmpCED;struct Cyc_Absyn_Kind _tmpCEE;enum Cyc_Absyn_KindQual _tmpCEF;enum Cyc_Absyn_AliasQual
_tmpCF0;struct Cyc_Core_Opt*_tmpCF2;struct Cyc_Core_Opt**_tmpCF3;struct Cyc_Absyn_Kind*
_tmpCF4;struct Cyc_Absyn_Kind _tmpCF5;enum Cyc_Absyn_KindQual _tmpCF6;enum Cyc_Absyn_AliasQual
_tmpCF7;struct Cyc_Core_Opt*_tmpCF9;struct Cyc_Core_Opt**_tmpCFA;struct Cyc_Absyn_Kind*
_tmpCFB;struct Cyc_Absyn_Kind _tmpCFC;enum Cyc_Absyn_KindQual _tmpCFD;enum Cyc_Absyn_AliasQual
_tmpCFE;struct Cyc_Core_Opt*_tmpD00;struct Cyc_Core_Opt**_tmpD01;struct Cyc_Absyn_Kind*
_tmpD02;struct Cyc_Absyn_Kind _tmpD03;enum Cyc_Absyn_KindQual _tmpD04;enum Cyc_Absyn_AliasQual
_tmpD05;struct Cyc_Core_Opt*_tmpD07;struct Cyc_Core_Opt**_tmpD08;struct Cyc_Absyn_Kind*
_tmpD09;struct Cyc_Absyn_Kind _tmpD0A;enum Cyc_Absyn_KindQual _tmpD0B;enum Cyc_Absyn_AliasQual
_tmpD0C;struct Cyc_Core_Opt*_tmpD0E;struct Cyc_Core_Opt**_tmpD0F;struct Cyc_Absyn_Kind*
_tmpD10;struct Cyc_Absyn_Kind _tmpD11;enum Cyc_Absyn_KindQual _tmpD12;enum Cyc_Absyn_AliasQual
_tmpD13;struct Cyc_Core_Opt*_tmpD15;struct Cyc_Core_Opt**_tmpD16;struct Cyc_Absyn_Kind*
_tmpD17;struct Cyc_Absyn_Kind _tmpD18;enum Cyc_Absyn_KindQual _tmpD19;enum Cyc_Absyn_AliasQual
_tmpD1A;struct Cyc_Core_Opt*_tmpD1C;struct Cyc_Core_Opt**_tmpD1D;struct Cyc_Absyn_Kind*
_tmpD1E;struct Cyc_Absyn_Kind*_tmpD20;struct Cyc_Absyn_Kind _tmpD21;enum Cyc_Absyn_KindQual
_tmpD22;_LL64D: {struct Cyc_Absyn_Unknown_kb_struct*_tmpCE0=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpCDF;if(_tmpCE0->tag != 1)goto _LL64F;else{_tmpCE1=_tmpCE0->f1;_tmpCE2=(struct
Cyc_Core_Opt**)& _tmpCE0->f1;}}_LL64E:{const char*_tmp151C;void*_tmp151B[1];struct
Cyc_String_pa_struct _tmp151A;(_tmp151A.tag=0,((_tmp151A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp151B[0]=&
_tmp151A,Cyc_Tcutil_warn(loc,((_tmp151C="Type variable %s unconstrained, assuming boxed",
_tag_dyneither(_tmp151C,sizeof(char),47))),_tag_dyneither(_tmp151B,sizeof(void*),
1)))))));}_tmpCE5=_tmpCE2;goto _LL650;_LL64F: {struct Cyc_Absyn_Less_kb_struct*
_tmpCE3=(struct Cyc_Absyn_Less_kb_struct*)_tmpCDF;if(_tmpCE3->tag != 2)goto _LL651;
else{_tmpCE4=_tmpCE3->f1;_tmpCE5=(struct Cyc_Core_Opt**)& _tmpCE3->f1;_tmpCE6=
_tmpCE3->f2;_tmpCE7=*_tmpCE6;_tmpCE8=_tmpCE7.kind;if(_tmpCE8 != Cyc_Absyn_BoxKind)
goto _LL651;_tmpCE9=_tmpCE7.aliasqual;if(_tmpCE9 != Cyc_Absyn_Top)goto _LL651;}}
_LL650: _tmpCEC=_tmpCE5;goto _LL652;_LL651: {struct Cyc_Absyn_Less_kb_struct*
_tmpCEA=(struct Cyc_Absyn_Less_kb_struct*)_tmpCDF;if(_tmpCEA->tag != 2)goto _LL653;
else{_tmpCEB=_tmpCEA->f1;_tmpCEC=(struct Cyc_Core_Opt**)& _tmpCEA->f1;_tmpCED=
_tmpCEA->f2;_tmpCEE=*_tmpCED;_tmpCEF=_tmpCEE.kind;if(_tmpCEF != Cyc_Absyn_MemKind)
goto _LL653;_tmpCF0=_tmpCEE.aliasqual;if(_tmpCF0 != Cyc_Absyn_Top)goto _LL653;}}
_LL652: _tmpCF3=_tmpCEC;goto _LL654;_LL653: {struct Cyc_Absyn_Less_kb_struct*
_tmpCF1=(struct Cyc_Absyn_Less_kb_struct*)_tmpCDF;if(_tmpCF1->tag != 2)goto _LL655;
else{_tmpCF2=_tmpCF1->f1;_tmpCF3=(struct Cyc_Core_Opt**)& _tmpCF1->f1;_tmpCF4=
_tmpCF1->f2;_tmpCF5=*_tmpCF4;_tmpCF6=_tmpCF5.kind;if(_tmpCF6 != Cyc_Absyn_MemKind)
goto _LL655;_tmpCF7=_tmpCF5.aliasqual;if(_tmpCF7 != Cyc_Absyn_Aliasable)goto _LL655;}}
_LL654: _tmpCFA=_tmpCF3;goto _LL656;_LL655: {struct Cyc_Absyn_Less_kb_struct*
_tmpCF8=(struct Cyc_Absyn_Less_kb_struct*)_tmpCDF;if(_tmpCF8->tag != 2)goto _LL657;
else{_tmpCF9=_tmpCF8->f1;_tmpCFA=(struct Cyc_Core_Opt**)& _tmpCF8->f1;_tmpCFB=
_tmpCF8->f2;_tmpCFC=*_tmpCFB;_tmpCFD=_tmpCFC.kind;if(_tmpCFD != Cyc_Absyn_AnyKind)
goto _LL657;_tmpCFE=_tmpCFC.aliasqual;if(_tmpCFE != Cyc_Absyn_Top)goto _LL657;}}
_LL656: _tmpD01=_tmpCFA;goto _LL658;_LL657: {struct Cyc_Absyn_Less_kb_struct*
_tmpCFF=(struct Cyc_Absyn_Less_kb_struct*)_tmpCDF;if(_tmpCFF->tag != 2)goto _LL659;
else{_tmpD00=_tmpCFF->f1;_tmpD01=(struct Cyc_Core_Opt**)& _tmpCFF->f1;_tmpD02=
_tmpCFF->f2;_tmpD03=*_tmpD02;_tmpD04=_tmpD03.kind;if(_tmpD04 != Cyc_Absyn_AnyKind)
goto _LL659;_tmpD05=_tmpD03.aliasqual;if(_tmpD05 != Cyc_Absyn_Aliasable)goto _LL659;}}
_LL658:*_tmpD01=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL64C;_LL659: {
struct Cyc_Absyn_Less_kb_struct*_tmpD06=(struct Cyc_Absyn_Less_kb_struct*)_tmpCDF;
if(_tmpD06->tag != 2)goto _LL65B;else{_tmpD07=_tmpD06->f1;_tmpD08=(struct Cyc_Core_Opt**)&
_tmpD06->f1;_tmpD09=_tmpD06->f2;_tmpD0A=*_tmpD09;_tmpD0B=_tmpD0A.kind;if(_tmpD0B
!= Cyc_Absyn_MemKind)goto _LL65B;_tmpD0C=_tmpD0A.aliasqual;if(_tmpD0C != Cyc_Absyn_Unique)
goto _LL65B;}}_LL65A: _tmpD0F=_tmpD08;goto _LL65C;_LL65B: {struct Cyc_Absyn_Less_kb_struct*
_tmpD0D=(struct Cyc_Absyn_Less_kb_struct*)_tmpCDF;if(_tmpD0D->tag != 2)goto _LL65D;
else{_tmpD0E=_tmpD0D->f1;_tmpD0F=(struct Cyc_Core_Opt**)& _tmpD0D->f1;_tmpD10=
_tmpD0D->f2;_tmpD11=*_tmpD10;_tmpD12=_tmpD11.kind;if(_tmpD12 != Cyc_Absyn_AnyKind)
goto _LL65D;_tmpD13=_tmpD11.aliasqual;if(_tmpD13 != Cyc_Absyn_Unique)goto _LL65D;}}
_LL65C:*_tmpD0F=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL64C;_LL65D: {
struct Cyc_Absyn_Less_kb_struct*_tmpD14=(struct Cyc_Absyn_Less_kb_struct*)_tmpCDF;
if(_tmpD14->tag != 2)goto _LL65F;else{_tmpD15=_tmpD14->f1;_tmpD16=(struct Cyc_Core_Opt**)&
_tmpD14->f1;_tmpD17=_tmpD14->f2;_tmpD18=*_tmpD17;_tmpD19=_tmpD18.kind;if(_tmpD19
!= Cyc_Absyn_RgnKind)goto _LL65F;_tmpD1A=_tmpD18.aliasqual;if(_tmpD1A != Cyc_Absyn_Top)
goto _LL65F;}}_LL65E:*_tmpD16=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto
_LL64C;_LL65F: {struct Cyc_Absyn_Less_kb_struct*_tmpD1B=(struct Cyc_Absyn_Less_kb_struct*)
_tmpCDF;if(_tmpD1B->tag != 2)goto _LL661;else{_tmpD1C=_tmpD1B->f1;_tmpD1D=(struct
Cyc_Core_Opt**)& _tmpD1B->f1;_tmpD1E=_tmpD1B->f2;}}_LL660:*_tmpD1D=Cyc_Tcutil_kind_to_bound_opt(
_tmpD1E);goto _LL64C;_LL661: {struct Cyc_Absyn_Eq_kb_struct*_tmpD1F=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpCDF;if(_tmpD1F->tag != 0)goto _LL663;else{_tmpD20=_tmpD1F->f1;_tmpD21=*_tmpD20;
_tmpD22=_tmpD21.kind;if(_tmpD22 != Cyc_Absyn_MemKind)goto _LL663;}}_LL662:{const
char*_tmp151F;void*_tmp151E;(_tmp151E=0,Cyc_Tcutil_terr(loc,((_tmp151F="functions can't abstract M types",
_tag_dyneither(_tmp151F,sizeof(char),33))),_tag_dyneither(_tmp151E,sizeof(void*),
0)));}goto _LL64C;_LL663:;_LL664: goto _LL64C;_LL64C:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
Cyc_Core_heap_region,_tmpC46.kind_env,*_tmpB46);_tmpC46.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmpC46.r,_tmpC46.free_vars,*_tmpB46);{struct Cyc_List_List*tvs=_tmpC46.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpD29;int _tmpD2A;struct
_tuple30 _tmpD28=*((struct _tuple30*)tvs->hd);_tmpD29=_tmpD28.f1;_tmpD2A=_tmpD28.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpD29,_tmpD2A);}}{struct Cyc_List_List*evs=_tmpC46.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmpD2C;int _tmpD2D;struct _tuple31 _tmpD2B=*((struct _tuple31*)evs->hd);
_tmpD2C=_tmpD2B.f1;_tmpD2D=_tmpD2B.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmpD2C,_tmpD2D);}};}goto _LL56C;};}_LL58B: {struct Cyc_Absyn_TupleType_struct*
_tmpB51=(struct Cyc_Absyn_TupleType_struct*)_tmpB0A;if(_tmpB51->tag != 10)goto
_LL58D;else{_tmpB52=_tmpB51->f1;}}_LL58C: for(0;_tmpB52 != 0;_tmpB52=_tmpB52->tl){
struct _tuple12*_tmpD2F=(struct _tuple12*)_tmpB52->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpD2F).f2,1);((*_tmpD2F).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmpD2F).f1).print_const,(*_tmpD2F).f2);}goto _LL56C;_LL58D: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpB53=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpB0A;if(_tmpB53->tag != 12)goto
_LL58F;else{_tmpB54=_tmpB53->f1;_tmpB55=_tmpB53->f2;}}_LL58E: {struct
_RegionHandle*_tmpD30=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*prev_fields=0;
for(0;_tmpB55 != 0;_tmpB55=_tmpB55->tl){struct Cyc_Absyn_Aggrfield _tmpD32;struct
_dyneither_ptr*_tmpD33;struct Cyc_Absyn_Tqual _tmpD34;struct Cyc_Absyn_Tqual*
_tmpD35;void*_tmpD36;struct Cyc_Absyn_Exp*_tmpD37;struct Cyc_List_List*_tmpD38;
struct Cyc_Absyn_Aggrfield*_tmpD31=(struct Cyc_Absyn_Aggrfield*)_tmpB55->hd;
_tmpD32=*_tmpD31;_tmpD33=_tmpD32.name;_tmpD34=_tmpD32.tq;_tmpD35=(struct Cyc_Absyn_Tqual*)&(*
_tmpD31).tq;_tmpD36=_tmpD32.type;_tmpD37=_tmpD32.width;_tmpD38=_tmpD32.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpD33)){
const char*_tmp1523;void*_tmp1522[1];struct Cyc_String_pa_struct _tmp1521;(_tmp1521.tag=
0,((_tmp1521.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpD33),((
_tmp1522[0]=& _tmp1521,Cyc_Tcutil_terr(loc,((_tmp1523="duplicate field %s",
_tag_dyneither(_tmp1523,sizeof(char),19))),_tag_dyneither(_tmp1522,sizeof(void*),
1)))))));}{const char*_tmp1524;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpD33,((
_tmp1524="",_tag_dyneither(_tmp1524,sizeof(char),1))))!= 0){struct Cyc_List_List*
_tmp1525;prev_fields=((_tmp1525=_region_malloc(_tmpD30,sizeof(*_tmp1525)),((
_tmp1525->hd=_tmpD33,((_tmp1525->tl=prev_fields,_tmp1525))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpD36,1);_tmpD35->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpD35->print_const,_tmpD36);if(_tmpB54 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(
_tmpD36)){const char*_tmp1529;void*_tmp1528[1];struct Cyc_String_pa_struct _tmp1527;(
_tmp1527.tag=0,((_tmp1527.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpD33),((_tmp1528[0]=& _tmp1527,Cyc_Tcutil_warn(loc,((_tmp1529="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp1529,sizeof(char),74))),_tag_dyneither(_tmp1528,sizeof(void*),
1)))))));}Cyc_Tcutil_check_bitfield(loc,te,_tmpD36,_tmpD37,_tmpD33);Cyc_Tcutil_check_field_atts(
loc,_tmpD33,_tmpD38);}}goto _LL56C;}_LL58F: {struct Cyc_Absyn_AggrType_struct*
_tmpB56=(struct Cyc_Absyn_AggrType_struct*)_tmpB0A;if(_tmpB56->tag != 11)goto
_LL591;else{_tmpB57=_tmpB56->f1;_tmpB58=_tmpB57.aggr_info;_tmpB59=(union Cyc_Absyn_AggrInfoU*)&(
_tmpB56->f1).aggr_info;_tmpB5A=_tmpB57.targs;_tmpB5B=(struct Cyc_List_List**)&(
_tmpB56->f1).targs;}}_LL590:{union Cyc_Absyn_AggrInfoU _tmpD41=*_tmpB59;struct
_tuple4 _tmpD42;enum Cyc_Absyn_AggrKind _tmpD43;struct _tuple2*_tmpD44;struct Cyc_Core_Opt*
_tmpD45;struct Cyc_Absyn_Aggrdecl**_tmpD46;struct Cyc_Absyn_Aggrdecl*_tmpD47;
_LL666: if((_tmpD41.UnknownAggr).tag != 1)goto _LL668;_tmpD42=(struct _tuple4)(
_tmpD41.UnknownAggr).val;_tmpD43=_tmpD42.f1;_tmpD44=_tmpD42.f2;_tmpD45=_tmpD42.f3;
_LL667: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmpD48;_push_handler(&
_tmpD48);{int _tmpD4A=0;if(setjmp(_tmpD48.handler))_tmpD4A=1;if(!_tmpD4A){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpD44);{struct Cyc_Absyn_Aggrdecl*_tmpD4B=*adp;if(_tmpD4B->kind != 
_tmpD43){if(_tmpD4B->kind == Cyc_Absyn_StructA){const char*_tmp152E;void*_tmp152D[
2];struct Cyc_String_pa_struct _tmp152C;struct Cyc_String_pa_struct _tmp152B;(
_tmp152B.tag=0,((_tmp152B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD44)),((_tmp152C.tag=0,((_tmp152C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD44)),((_tmp152D[0]=& _tmp152C,((
_tmp152D[1]=& _tmp152B,Cyc_Tcutil_terr(loc,((_tmp152E="expecting struct %s instead of union %s",
_tag_dyneither(_tmp152E,sizeof(char),40))),_tag_dyneither(_tmp152D,sizeof(void*),
2)))))))))))));}else{const char*_tmp1533;void*_tmp1532[2];struct Cyc_String_pa_struct
_tmp1531;struct Cyc_String_pa_struct _tmp1530;(_tmp1530.tag=0,((_tmp1530.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD44)),((
_tmp1531.tag=0,((_tmp1531.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD44)),((_tmp1532[0]=& _tmp1531,((_tmp1532[1]=& _tmp1530,Cyc_Tcutil_terr(loc,((
_tmp1533="expecting union %s instead of struct %s",_tag_dyneither(_tmp1533,
sizeof(char),40))),_tag_dyneither(_tmp1532,sizeof(void*),2)))))))))))));}}if((
unsigned int)_tmpD45  && (int)_tmpD45->v){if(!((unsigned int)_tmpD4B->impl) || !((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD4B->impl))->tagged){const char*
_tmp1537;void*_tmp1536[1];struct Cyc_String_pa_struct _tmp1535;(_tmp1535.tag=0,((
_tmp1535.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD44)),((_tmp1536[0]=& _tmp1535,Cyc_Tcutil_terr(loc,((_tmp1537="@tagged qualfiers don't agree on union %s",
_tag_dyneither(_tmp1537,sizeof(char),42))),_tag_dyneither(_tmp1536,sizeof(void*),
1)))))));}}*_tmpB59=Cyc_Absyn_KnownAggr(adp);};;_pop_handler();}else{void*
_tmpD49=(void*)_exn_thrown;void*_tmpD58=_tmpD49;_LL66B: {struct Cyc_Dict_Absent_struct*
_tmpD59=(struct Cyc_Dict_Absent_struct*)_tmpD58;if(_tmpD59->tag != Cyc_Dict_Absent)
goto _LL66D;}_LL66C: {struct Cyc_Tcenv_Genv*_tmpD5A=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp1538;struct Cyc_Absyn_Aggrdecl*_tmpD5B=(_tmp1538=
_cycalloc(sizeof(*_tmp1538)),((_tmp1538->kind=_tmpD43,((_tmp1538->sc=Cyc_Absyn_Extern,((
_tmp1538->name=_tmpD44,((_tmp1538->tvs=0,((_tmp1538->impl=0,((_tmp1538->attributes=
0,_tmp1538)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmpD5A,loc,_tmpD5B);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpD44);*_tmpB59=Cyc_Absyn_KnownAggr(adp);if(*_tmpB5B != 0){{const char*
_tmp153C;void*_tmp153B[1];struct Cyc_String_pa_struct _tmp153A;(_tmp153A.tag=0,((
_tmp153A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD44)),((_tmp153B[0]=& _tmp153A,Cyc_Tcutil_terr(loc,((_tmp153C="declare parameterized type %s before using",
_tag_dyneither(_tmp153C,sizeof(char),43))),_tag_dyneither(_tmp153B,sizeof(void*),
1)))))));}return cvtenv;}goto _LL66A;}_LL66D:;_LL66E:(void)_throw(_tmpD58);_LL66A:;}};}
_tmpD47=*adp;goto _LL669;}_LL668: if((_tmpD41.KnownAggr).tag != 2)goto _LL665;
_tmpD46=(struct Cyc_Absyn_Aggrdecl**)(_tmpD41.KnownAggr).val;_tmpD47=*_tmpD46;
_LL669: {struct Cyc_List_List*tvs=_tmpD47->tvs;struct Cyc_List_List*ts=*_tmpB5B;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Tvar*_tmpD60=(
struct Cyc_Absyn_Tvar*)tvs->hd;void*_tmpD61=(void*)ts->hd;{struct _tuple0 _tmp153D;
struct _tuple0 _tmpD63=(_tmp153D.f1=Cyc_Absyn_compress_kb(_tmpD60->kind),((
_tmp153D.f2=_tmpD61,_tmp153D)));void*_tmpD64;void*_tmpD66;struct Cyc_Absyn_Tvar*
_tmpD68;_LL670: _tmpD64=_tmpD63.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpD65=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmpD64;if(_tmpD65->tag != 1)goto _LL672;};
_tmpD66=_tmpD63.f2;{struct Cyc_Absyn_VarType_struct*_tmpD67=(struct Cyc_Absyn_VarType_struct*)
_tmpD66;if(_tmpD67->tag != 2)goto _LL672;else{_tmpD68=_tmpD67->f1;}};_LL671: cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpD68);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpD68,1);continue;_LL672:;_LL673: goto _LL66F;_LL66F:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);Cyc_Tcutil_check_no_qual(
loc,(void*)ts->hd);};}if(ts != 0){const char*_tmp1541;void*_tmp1540[1];struct Cyc_String_pa_struct
_tmp153F;(_tmp153F.tag=0,((_tmp153F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD47->name)),((_tmp1540[0]=& _tmp153F,
Cyc_Tcutil_terr(loc,((_tmp1541="too many parameters for type %s",_tag_dyneither(
_tmp1541,sizeof(char),32))),_tag_dyneither(_tmp1540,sizeof(void*),1)))))));}if(
tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,
expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_tmp1542;
hidden_ts=((_tmp1542=_cycalloc(sizeof(*_tmp1542)),((_tmp1542->hd=e,((_tmp1542->tl=
hidden_ts,_tmp1542))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,
1);}*_tmpB5B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(*_tmpB5B,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(hidden_ts));}}_LL665:;}goto _LL56C;_LL591: {struct Cyc_Absyn_TypedefType_struct*
_tmpB5C=(struct Cyc_Absyn_TypedefType_struct*)_tmpB0A;if(_tmpB5C->tag != 17)goto
_LL593;else{_tmpB5D=_tmpB5C->f1;_tmpB5E=_tmpB5C->f2;_tmpB5F=(struct Cyc_List_List**)&
_tmpB5C->f2;_tmpB60=_tmpB5C->f3;_tmpB61=(struct Cyc_Absyn_Typedefdecl**)& _tmpB5C->f3;
_tmpB62=_tmpB5C->f4;_tmpB63=(void***)& _tmpB5C->f4;}}_LL592: {struct Cyc_List_List*
targs=*_tmpB5F;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmpD6D;
_push_handler(& _tmpD6D);{int _tmpD6F=0;if(setjmp(_tmpD6D.handler))_tmpD6F=1;if(!
_tmpD6F){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmpB5D);;_pop_handler();}else{
void*_tmpD6E=(void*)_exn_thrown;void*_tmpD71=_tmpD6E;_LL675: {struct Cyc_Dict_Absent_struct*
_tmpD72=(struct Cyc_Dict_Absent_struct*)_tmpD71;if(_tmpD72->tag != Cyc_Dict_Absent)
goto _LL677;}_LL676:{const char*_tmp1546;void*_tmp1545[1];struct Cyc_String_pa_struct
_tmp1544;(_tmp1544.tag=0,((_tmp1544.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB5D)),((_tmp1545[0]=& _tmp1544,Cyc_Tcutil_terr(
loc,((_tmp1546="unbound typedef name %s",_tag_dyneither(_tmp1546,sizeof(char),24))),
_tag_dyneither(_tmp1545,sizeof(void*),1)))))));}return cvtenv;_LL677:;_LL678:(
void)_throw(_tmpD71);_LL674:;}};}*_tmpB61=(struct Cyc_Absyn_Typedefdecl*)td;
_tmpB5D[0]=(td->name)[0];{struct Cyc_List_List*tvs=td->tvs;struct Cyc_List_List*ts=
targs;struct _RegionHandle*_tmpD76=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Kind*k=
Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,
expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd,1);Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple16*_tmp1549;struct Cyc_List_List*_tmp1548;inst=((_tmp1548=
_region_malloc(_tmpD76,sizeof(*_tmp1548)),((_tmp1548->hd=((_tmp1549=
_region_malloc(_tmpD76,sizeof(*_tmp1549)),((_tmp1549->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmp1549->f2=(void*)ts->hd,_tmp1549)))))),((_tmp1548->tl=inst,_tmp1548))))));};}
if(ts != 0){const char*_tmp154D;void*_tmp154C[1];struct Cyc_String_pa_struct
_tmp154B;(_tmp154B.tag=0,((_tmp154B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB5D)),((_tmp154C[0]=& _tmp154B,Cyc_Tcutil_terr(
loc,((_tmp154D="too many parameters for typedef %s",_tag_dyneither(_tmp154D,
sizeof(char),35))),_tag_dyneither(_tmp154C,sizeof(void*),1)))))));}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,
expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);void*e=Cyc_Absyn_new_evar(0,0);{
struct Cyc_List_List*_tmp154E;hidden_ts=((_tmp154E=_cycalloc(sizeof(*_tmp154E)),((
_tmp154E->hd=e,((_tmp154E->tl=hidden_ts,_tmp154E))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);{struct _tuple16*_tmp1551;struct Cyc_List_List*_tmp1550;inst=(
struct Cyc_List_List*)((_tmp1550=_cycalloc(sizeof(*_tmp1550)),((_tmp1550->hd=(
struct _tuple16*)((_tmp1551=_cycalloc(sizeof(*_tmp1551)),((_tmp1551->f1=(struct
Cyc_Absyn_Tvar*)tvs->hd,((_tmp1551->f2=e,_tmp1551)))))),((_tmp1550->tl=inst,
_tmp1550))))));};}*_tmpB5F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}if(td->defn != 0){void*
new_typ=Cyc_Tcutil_rsubstitute(_tmpD76,inst,(void*)((struct Cyc_Core_Opt*)
_check_null(td->defn))->v);void**_tmp1552;*_tmpB63=((_tmp1552=_cycalloc(sizeof(*
_tmp1552)),((_tmp1552[0]=new_typ,_tmp1552))));}}goto _LL56C;};}_LL593: {struct Cyc_Absyn_RefCntRgn_struct*
_tmpB64=(struct Cyc_Absyn_RefCntRgn_struct*)_tmpB0A;if(_tmpB64->tag != 22)goto
_LL595;}_LL594: goto _LL596;_LL595: {struct Cyc_Absyn_UniqueRgn_struct*_tmpB65=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmpB0A;if(_tmpB65->tag != 21)goto _LL597;}
_LL596: goto _LL598;_LL597: {struct Cyc_Absyn_HeapRgn_struct*_tmpB66=(struct Cyc_Absyn_HeapRgn_struct*)
_tmpB0A;if(_tmpB66->tag != 20)goto _LL599;}_LL598: goto _LL56C;_LL599: {struct Cyc_Absyn_RgnHandleType_struct*
_tmpB67=(struct Cyc_Absyn_RgnHandleType_struct*)_tmpB0A;if(_tmpB67->tag != 15)goto
_LL59B;else{_tmpB68=(void*)_tmpB67->f1;}}_LL59A: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpB68,1);goto _LL56C;_LL59B: {struct Cyc_Absyn_DynRgnType_struct*
_tmpB69=(struct Cyc_Absyn_DynRgnType_struct*)_tmpB0A;if(_tmpB69->tag != 16)goto
_LL59D;else{_tmpB6A=(void*)_tmpB69->f1;_tmpB6B=(void*)_tmpB69->f2;}}_LL59C:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpB6A,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpB6B,1);
goto _LL56C;_LL59D: {struct Cyc_Absyn_AccessEff_struct*_tmpB6C=(struct Cyc_Absyn_AccessEff_struct*)
_tmpB0A;if(_tmpB6C->tag != 23)goto _LL59F;else{_tmpB6D=(void*)_tmpB6C->f1;}}_LL59E:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpB6D,1);
goto _LL56C;_LL59F: {struct Cyc_Absyn_RgnsEff_struct*_tmpB6E=(struct Cyc_Absyn_RgnsEff_struct*)
_tmpB0A;if(_tmpB6E->tag != 25)goto _LL5A1;else{_tmpB6F=(void*)_tmpB6E->f1;}}_LL5A0:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB6F,1);
goto _LL56C;_LL5A1: {struct Cyc_Absyn_JoinEff_struct*_tmpB70=(struct Cyc_Absyn_JoinEff_struct*)
_tmpB0A;if(_tmpB70->tag != 24)goto _LL56C;else{_tmpB71=_tmpB70->f1;}}_LL5A2: for(0;
_tmpB71 != 0;_tmpB71=_tmpB71->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,& Cyc_Tcutil_ek,(void*)_tmpB71->hd,1);}goto _LL56C;_LL56C:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind)){{void*_tmpD80=t;struct Cyc_Core_Opt*
_tmpD82;struct Cyc_Core_Opt*_tmpD83;_LL67A: {struct Cyc_Absyn_Evar_struct*_tmpD81=(
struct Cyc_Absyn_Evar_struct*)_tmpD80;if(_tmpD81->tag != 1)goto _LL67C;else{_tmpD82=
_tmpD81->f1;_tmpD83=_tmpD81->f2;}}_LL67B: {struct _dyneither_ptr s;{struct Cyc_Core_Opt*
_tmpD84=_tmpD82;struct Cyc_Core_Opt _tmpD85;struct Cyc_Absyn_Kind*_tmpD86;_LL67F:
if(_tmpD84 != 0)goto _LL681;_LL680:{const char*_tmp1553;s=((_tmp1553="kind=NULL ",
_tag_dyneither(_tmp1553,sizeof(char),11)));}goto _LL67E;_LL681: if(_tmpD84 == 0)
goto _LL67E;_tmpD85=*_tmpD84;_tmpD86=(struct Cyc_Absyn_Kind*)_tmpD85.v;_LL682:{
const char*_tmp1557;void*_tmp1556[1];struct Cyc_String_pa_struct _tmp1555;s=(struct
_dyneither_ptr)((_tmp1555.tag=0,((_tmp1555.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(_tmpD86)),((_tmp1556[0]=& _tmp1555,Cyc_aprintf(((
_tmp1557="kind=%s ",_tag_dyneither(_tmp1557,sizeof(char),9))),_tag_dyneither(
_tmp1556,sizeof(void*),1))))))));}goto _LL67E;_LL67E:;}{struct Cyc_Core_Opt*
_tmpD8B=_tmpD83;struct Cyc_Core_Opt _tmpD8C;void*_tmpD8D;_LL684: if(_tmpD8B != 0)
goto _LL686;_LL685:{const char*_tmp155B;void*_tmp155A[1];struct Cyc_String_pa_struct
_tmp1559;s=(struct _dyneither_ptr)((_tmp1559.tag=0,((_tmp1559.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)s),((_tmp155A[0]=& _tmp1559,Cyc_aprintf(((
_tmp155B="%s ref=NULL",_tag_dyneither(_tmp155B,sizeof(char),12))),_tag_dyneither(
_tmp155A,sizeof(void*),1))))))));}goto _LL683;_LL686: if(_tmpD8B == 0)goto _LL683;
_tmpD8C=*_tmpD8B;_tmpD8D=(void*)_tmpD8C.v;_LL687:{const char*_tmp1560;void*
_tmp155F[2];struct Cyc_String_pa_struct _tmp155E;struct Cyc_String_pa_struct
_tmp155D;s=(struct _dyneither_ptr)((_tmp155D.tag=0,((_tmp155D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpD8D)),((
_tmp155E.tag=0,((_tmp155E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp155F[0]=& _tmp155E,((_tmp155F[1]=& _tmp155D,Cyc_aprintf(((_tmp1560="%s ref=%s",
_tag_dyneither(_tmp1560,sizeof(char),10))),_tag_dyneither(_tmp155F,sizeof(void*),
2))))))))))))));}goto _LL683;_LL683:;}goto _LL679;}_LL67C:;_LL67D: goto _LL679;
_LL679:;}{const char*_tmp1566;void*_tmp1565[3];struct Cyc_String_pa_struct _tmp1564;
struct Cyc_String_pa_struct _tmp1563;struct Cyc_String_pa_struct _tmp1562;(_tmp1562.tag=
0,((_tmp1562.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
expected_kind)),((_tmp1563.tag=0,((_tmp1563.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp1564.tag=0,((
_tmp1564.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp1565[0]=& _tmp1564,((_tmp1565[1]=& _tmp1563,((_tmp1565[2]=& _tmp1562,Cyc_Tcutil_terr(
loc,((_tmp1566="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmp1566,sizeof(char),51))),_tag_dyneither(_tmp1565,sizeof(void*),
3)))))))))))))))))));};}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{
void*_tmpD9A=e->r;struct Cyc_List_List*_tmpDA0;struct Cyc_Absyn_Exp*_tmpDA2;struct
Cyc_Absyn_Exp*_tmpDA3;struct Cyc_Absyn_Exp*_tmpDA4;struct Cyc_Absyn_Exp*_tmpDA6;
struct Cyc_Absyn_Exp*_tmpDA7;struct Cyc_Absyn_Exp*_tmpDA9;struct Cyc_Absyn_Exp*
_tmpDAA;struct Cyc_Absyn_Exp*_tmpDAC;struct Cyc_Absyn_Exp*_tmpDAD;void*_tmpDAF;
struct Cyc_Absyn_Exp*_tmpDB0;void*_tmpDB2;void*_tmpDB4;void*_tmpDB6;struct Cyc_Absyn_Exp*
_tmpDB8;_LL689: {struct Cyc_Absyn_Const_e_struct*_tmpD9B=(struct Cyc_Absyn_Const_e_struct*)
_tmpD9A;if(_tmpD9B->tag != 0)goto _LL68B;}_LL68A: goto _LL68C;_LL68B: {struct Cyc_Absyn_Enum_e_struct*
_tmpD9C=(struct Cyc_Absyn_Enum_e_struct*)_tmpD9A;if(_tmpD9C->tag != 32)goto _LL68D;}
_LL68C: goto _LL68E;_LL68D: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpD9D=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpD9A;if(_tmpD9D->tag != 33)goto _LL68F;}_LL68E: goto _LL690;_LL68F: {struct Cyc_Absyn_Var_e_struct*
_tmpD9E=(struct Cyc_Absyn_Var_e_struct*)_tmpD9A;if(_tmpD9E->tag != 1)goto _LL691;}
_LL690: goto _LL688;_LL691: {struct Cyc_Absyn_Primop_e_struct*_tmpD9F=(struct Cyc_Absyn_Primop_e_struct*)
_tmpD9A;if(_tmpD9F->tag != 3)goto _LL693;else{_tmpDA0=_tmpD9F->f2;}}_LL692: for(0;
_tmpDA0 != 0;_tmpDA0=_tmpDA0->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmpDA0->hd,te,cvtenv);}goto _LL688;_LL693: {struct Cyc_Absyn_Conditional_e_struct*
_tmpDA1=(struct Cyc_Absyn_Conditional_e_struct*)_tmpD9A;if(_tmpDA1->tag != 6)goto
_LL695;else{_tmpDA2=_tmpDA1->f1;_tmpDA3=_tmpDA1->f2;_tmpDA4=_tmpDA1->f3;}}_LL694:
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDA2,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpDA3,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDA4,te,
cvtenv);goto _LL688;_LL695: {struct Cyc_Absyn_And_e_struct*_tmpDA5=(struct Cyc_Absyn_And_e_struct*)
_tmpD9A;if(_tmpDA5->tag != 7)goto _LL697;else{_tmpDA6=_tmpDA5->f1;_tmpDA7=_tmpDA5->f2;}}
_LL696: _tmpDA9=_tmpDA6;_tmpDAA=_tmpDA7;goto _LL698;_LL697: {struct Cyc_Absyn_Or_e_struct*
_tmpDA8=(struct Cyc_Absyn_Or_e_struct*)_tmpD9A;if(_tmpDA8->tag != 8)goto _LL699;
else{_tmpDA9=_tmpDA8->f1;_tmpDAA=_tmpDA8->f2;}}_LL698: _tmpDAC=_tmpDA9;_tmpDAD=
_tmpDAA;goto _LL69A;_LL699: {struct Cyc_Absyn_SeqExp_e_struct*_tmpDAB=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpD9A;if(_tmpDAB->tag != 9)goto _LL69B;else{_tmpDAC=_tmpDAB->f1;_tmpDAD=_tmpDAB->f2;}}
_LL69A: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDAC,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDAD,te,cvtenv);goto _LL688;_LL69B: {
struct Cyc_Absyn_Cast_e_struct*_tmpDAE=(struct Cyc_Absyn_Cast_e_struct*)_tmpD9A;
if(_tmpDAE->tag != 15)goto _LL69D;else{_tmpDAF=(void*)_tmpDAE->f1;_tmpDB0=_tmpDAE->f2;}}
_LL69C: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDB0,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpDAF,1);goto
_LL688;_LL69D: {struct Cyc_Absyn_Offsetof_e_struct*_tmpDB1=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpD9A;if(_tmpDB1->tag != 20)goto _LL69F;else{_tmpDB2=(void*)_tmpDB1->f1;}}_LL69E:
_tmpDB4=_tmpDB2;goto _LL6A0;_LL69F: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpDB3=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpD9A;if(_tmpDB3->tag != 18)goto _LL6A1;
else{_tmpDB4=(void*)_tmpDB3->f1;}}_LL6A0: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,
te,cvtenv,& Cyc_Tcutil_tak,_tmpDB4,1);goto _LL688;_LL6A1: {struct Cyc_Absyn_Valueof_e_struct*
_tmpDB5=(struct Cyc_Absyn_Valueof_e_struct*)_tmpD9A;if(_tmpDB5->tag != 39)goto
_LL6A3;else{_tmpDB6=(void*)_tmpDB5->f1;}}_LL6A2: cvtenv=Cyc_Tcutil_i_check_valid_type(
e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpDB6,1);goto _LL688;_LL6A3: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpDB7=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpD9A;if(_tmpDB7->tag != 19)goto
_LL6A5;else{_tmpDB8=_tmpDB7->f1;}}_LL6A4: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpDB8,te,cvtenv);goto _LL688;_LL6A5:;_LL6A6: {const char*_tmp1569;void*_tmp1568;(
_tmp1568=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1569="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmp1569,sizeof(char),66))),_tag_dyneither(_tmp1568,sizeof(void*),
0)));}_LL688:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){struct Cyc_List_List*_tmpDBB=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*
vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){struct Cyc_Absyn_Tvar*_tmpDBD;struct
_tuple30 _tmpDBC=*((struct _tuple30*)vs->hd);_tmpDBD=_tmpDBC.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(
_tmpDBB,_tmpDBD);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpDBF;struct _tuple31 _tmpDBE=*((struct _tuple31*)evs->hd);_tmpDBF=_tmpDBE.f1;{
void*_tmpDC0=Cyc_Tcutil_compress(_tmpDBF);struct Cyc_Core_Opt*_tmpDC2;struct Cyc_Core_Opt**
_tmpDC3;_LL6A8: {struct Cyc_Absyn_Evar_struct*_tmpDC1=(struct Cyc_Absyn_Evar_struct*)
_tmpDC0;if(_tmpDC1->tag != 1)goto _LL6AA;else{_tmpDC2=_tmpDC1->f4;_tmpDC3=(struct
Cyc_Core_Opt**)& _tmpDC1->f4;}}_LL6A9: if(*_tmpDC3 == 0){struct Cyc_Core_Opt*
_tmp156A;*_tmpDC3=((_tmp156A=_cycalloc(sizeof(*_tmp156A)),((_tmp156A->v=_tmpDBB,
_tmp156A))));}else{struct Cyc_List_List*_tmpDC5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmpDC3))->v;struct Cyc_List_List*_tmpDC6=0;for(0;_tmpDC5 != 0;
_tmpDC5=_tmpDC5->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmpDBB,(struct Cyc_Absyn_Tvar*)_tmpDC5->hd)){struct Cyc_List_List*_tmp156B;
_tmpDC6=((_tmp156B=_cycalloc(sizeof(*_tmp156B)),((_tmp156B->hd=(struct Cyc_Absyn_Tvar*)
_tmpDC5->hd,((_tmp156B->tl=_tmpDC6,_tmp156B))))));}}{struct Cyc_Core_Opt*_tmp156C;*
_tmpDC3=((_tmp156C=_cycalloc(sizeof(*_tmp156C)),((_tmp156C->v=_tmpDC6,_tmp156C))));};}
goto _LL6A7;_LL6AA:;_LL6AB: goto _LL6A7;_LL6A7:;};}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmpDC9=Cyc_Tcenv_lookup_type_vars(
te);struct _RegionHandle*_tmpDCA=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv
_tmp156D;struct Cyc_Tcutil_CVTEnv _tmpDCB=Cyc_Tcutil_check_valid_type(loc,te,((
_tmp156D.r=_tmpDCA,((_tmp156D.kind_env=_tmpDC9,((_tmp156D.free_vars=0,((_tmp156D.free_evars=
0,((_tmp156D.generalize_evars=generalize_evars,((_tmp156D.fn_result=0,_tmp156D)))))))))))),&
Cyc_Tcutil_tmk,t);struct Cyc_List_List*_tmpDCC=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple30*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpDCA,(struct Cyc_Absyn_Tvar*(*)(struct _tuple30*))Cyc_Core_fst,
_tmpDCB.free_vars);struct Cyc_List_List*_tmpDCD=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple31*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmpDCA,(void*(*)(struct _tuple31*))Cyc_Core_fst,_tmpDCB.free_evars);if(_tmpDC9 != 
0){struct Cyc_List_List*_tmpDCE=0;{struct Cyc_List_List*_tmpDCF=_tmpDCC;for(0;(
unsigned int)_tmpDCF;_tmpDCF=_tmpDCF->tl){struct Cyc_Absyn_Tvar*_tmpDD0=(struct
Cyc_Absyn_Tvar*)_tmpDCF->hd;int found=0;{struct Cyc_List_List*_tmpDD1=_tmpDC9;for(
0;(unsigned int)_tmpDD1;_tmpDD1=_tmpDD1->tl){if(Cyc_Absyn_tvar_cmp(_tmpDD0,(
struct Cyc_Absyn_Tvar*)_tmpDD1->hd)== 0){found=1;break;}}}if(!found){struct Cyc_List_List*
_tmp156E;_tmpDCE=((_tmp156E=_region_malloc(_tmpDCA,sizeof(*_tmp156E)),((_tmp156E->hd=(
struct Cyc_Absyn_Tvar*)_tmpDCF->hd,((_tmp156E->tl=_tmpDCE,_tmp156E))))));}}}
_tmpDCC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpDCE);}{struct Cyc_List_List*x=_tmpDCC;for(0;x != 0;x=x->tl){void*_tmpDD3=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmpDD5;struct Cyc_Core_Opt**
_tmpDD6;struct Cyc_Core_Opt*_tmpDD8;struct Cyc_Core_Opt**_tmpDD9;struct Cyc_Absyn_Kind*
_tmpDDA;struct Cyc_Absyn_Kind _tmpDDB;enum Cyc_Absyn_KindQual _tmpDDC;enum Cyc_Absyn_AliasQual
_tmpDDD;struct Cyc_Core_Opt*_tmpDDF;struct Cyc_Core_Opt**_tmpDE0;struct Cyc_Absyn_Kind*
_tmpDE1;struct Cyc_Absyn_Kind _tmpDE2;enum Cyc_Absyn_KindQual _tmpDE3;enum Cyc_Absyn_AliasQual
_tmpDE4;struct Cyc_Core_Opt*_tmpDE6;struct Cyc_Core_Opt**_tmpDE7;struct Cyc_Absyn_Kind*
_tmpDE8;struct Cyc_Absyn_Kind _tmpDE9;enum Cyc_Absyn_KindQual _tmpDEA;enum Cyc_Absyn_AliasQual
_tmpDEB;struct Cyc_Core_Opt*_tmpDED;struct Cyc_Core_Opt**_tmpDEE;struct Cyc_Absyn_Kind*
_tmpDEF;struct Cyc_Absyn_Kind _tmpDF0;enum Cyc_Absyn_KindQual _tmpDF1;enum Cyc_Absyn_AliasQual
_tmpDF2;struct Cyc_Core_Opt*_tmpDF4;struct Cyc_Core_Opt**_tmpDF5;struct Cyc_Absyn_Kind*
_tmpDF6;struct Cyc_Absyn_Kind _tmpDF7;enum Cyc_Absyn_KindQual _tmpDF8;enum Cyc_Absyn_AliasQual
_tmpDF9;struct Cyc_Core_Opt*_tmpDFB;struct Cyc_Core_Opt**_tmpDFC;struct Cyc_Absyn_Kind*
_tmpDFD;struct Cyc_Absyn_Kind*_tmpDFF;struct Cyc_Absyn_Kind _tmpE00;enum Cyc_Absyn_KindQual
_tmpE01;enum Cyc_Absyn_AliasQual _tmpE02;_LL6AD: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpDD4=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpDD3;if(_tmpDD4->tag != 1)goto
_LL6AF;else{_tmpDD5=_tmpDD4->f1;_tmpDD6=(struct Cyc_Core_Opt**)& _tmpDD4->f1;}}
_LL6AE: _tmpDD9=_tmpDD6;goto _LL6B0;_LL6AF: {struct Cyc_Absyn_Less_kb_struct*
_tmpDD7=(struct Cyc_Absyn_Less_kb_struct*)_tmpDD3;if(_tmpDD7->tag != 2)goto _LL6B1;
else{_tmpDD8=_tmpDD7->f1;_tmpDD9=(struct Cyc_Core_Opt**)& _tmpDD7->f1;_tmpDDA=
_tmpDD7->f2;_tmpDDB=*_tmpDDA;_tmpDDC=_tmpDDB.kind;if(_tmpDDC != Cyc_Absyn_BoxKind)
goto _LL6B1;_tmpDDD=_tmpDDB.aliasqual;if(_tmpDDD != Cyc_Absyn_Top)goto _LL6B1;}}
_LL6B0: _tmpDE0=_tmpDD9;goto _LL6B2;_LL6B1: {struct Cyc_Absyn_Less_kb_struct*
_tmpDDE=(struct Cyc_Absyn_Less_kb_struct*)_tmpDD3;if(_tmpDDE->tag != 2)goto _LL6B3;
else{_tmpDDF=_tmpDDE->f1;_tmpDE0=(struct Cyc_Core_Opt**)& _tmpDDE->f1;_tmpDE1=
_tmpDDE->f2;_tmpDE2=*_tmpDE1;_tmpDE3=_tmpDE2.kind;if(_tmpDE3 != Cyc_Absyn_MemKind)
goto _LL6B3;_tmpDE4=_tmpDE2.aliasqual;if(_tmpDE4 != Cyc_Absyn_Top)goto _LL6B3;}}
_LL6B2: _tmpDE7=_tmpDE0;goto _LL6B4;_LL6B3: {struct Cyc_Absyn_Less_kb_struct*
_tmpDE5=(struct Cyc_Absyn_Less_kb_struct*)_tmpDD3;if(_tmpDE5->tag != 2)goto _LL6B5;
else{_tmpDE6=_tmpDE5->f1;_tmpDE7=(struct Cyc_Core_Opt**)& _tmpDE5->f1;_tmpDE8=
_tmpDE5->f2;_tmpDE9=*_tmpDE8;_tmpDEA=_tmpDE9.kind;if(_tmpDEA != Cyc_Absyn_MemKind)
goto _LL6B5;_tmpDEB=_tmpDE9.aliasqual;if(_tmpDEB != Cyc_Absyn_Aliasable)goto _LL6B5;}}
_LL6B4:*_tmpDE7=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6AC;_LL6B5: {
struct Cyc_Absyn_Less_kb_struct*_tmpDEC=(struct Cyc_Absyn_Less_kb_struct*)_tmpDD3;
if(_tmpDEC->tag != 2)goto _LL6B7;else{_tmpDED=_tmpDEC->f1;_tmpDEE=(struct Cyc_Core_Opt**)&
_tmpDEC->f1;_tmpDEF=_tmpDEC->f2;_tmpDF0=*_tmpDEF;_tmpDF1=_tmpDF0.kind;if(_tmpDF1
!= Cyc_Absyn_MemKind)goto _LL6B7;_tmpDF2=_tmpDF0.aliasqual;if(_tmpDF2 != Cyc_Absyn_Unique)
goto _LL6B7;}}_LL6B6:*_tmpDEE=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto
_LL6AC;_LL6B7: {struct Cyc_Absyn_Less_kb_struct*_tmpDF3=(struct Cyc_Absyn_Less_kb_struct*)
_tmpDD3;if(_tmpDF3->tag != 2)goto _LL6B9;else{_tmpDF4=_tmpDF3->f1;_tmpDF5=(struct
Cyc_Core_Opt**)& _tmpDF3->f1;_tmpDF6=_tmpDF3->f2;_tmpDF7=*_tmpDF6;_tmpDF8=_tmpDF7.kind;
if(_tmpDF8 != Cyc_Absyn_RgnKind)goto _LL6B9;_tmpDF9=_tmpDF7.aliasqual;if(_tmpDF9 != 
Cyc_Absyn_Top)goto _LL6B9;}}_LL6B8:*_tmpDF5=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
goto _LL6AC;_LL6B9: {struct Cyc_Absyn_Less_kb_struct*_tmpDFA=(struct Cyc_Absyn_Less_kb_struct*)
_tmpDD3;if(_tmpDFA->tag != 2)goto _LL6BB;else{_tmpDFB=_tmpDFA->f1;_tmpDFC=(struct
Cyc_Core_Opt**)& _tmpDFA->f1;_tmpDFD=_tmpDFA->f2;}}_LL6BA:*_tmpDFC=Cyc_Tcutil_kind_to_bound_opt(
_tmpDFD);goto _LL6AC;_LL6BB: {struct Cyc_Absyn_Eq_kb_struct*_tmpDFE=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpDD3;if(_tmpDFE->tag != 0)goto _LL6BD;else{_tmpDFF=_tmpDFE->f1;_tmpE00=*_tmpDFF;
_tmpE01=_tmpE00.kind;if(_tmpE01 != Cyc_Absyn_MemKind)goto _LL6BD;_tmpE02=_tmpE00.aliasqual;}}
_LL6BC:{const char*_tmp1575;void*_tmp1574[2];struct Cyc_String_pa_struct _tmp1573;
struct Cyc_Absyn_Kind*_tmp1572;struct Cyc_String_pa_struct _tmp1571;(_tmp1571.tag=0,((
_tmp1571.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(((
_tmp1572=_cycalloc_atomic(sizeof(*_tmp1572)),((_tmp1572->kind=Cyc_Absyn_MemKind,((
_tmp1572->aliasqual=_tmpE02,_tmp1572)))))))),((_tmp1573.tag=0,((_tmp1573.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd)),((_tmp1574[0]=& _tmp1573,((_tmp1574[1]=& _tmp1571,Cyc_Tcutil_terr(loc,((
_tmp1575="type variable %s cannot have kind %s",_tag_dyneither(_tmp1575,sizeof(
char),37))),_tag_dyneither(_tmp1574,sizeof(void*),2)))))))))))));}goto _LL6AC;
_LL6BD:;_LL6BE: goto _LL6AC;_LL6AC:;}}if(_tmpDCC != 0  || _tmpDCD != 0){{void*_tmpE08=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpE0A;struct Cyc_List_List*_tmpE0B;
struct Cyc_List_List**_tmpE0C;struct Cyc_Core_Opt*_tmpE0D;struct Cyc_Absyn_Tqual
_tmpE0E;void*_tmpE0F;struct Cyc_List_List*_tmpE10;int _tmpE11;struct Cyc_Absyn_VarargInfo*
_tmpE12;struct Cyc_List_List*_tmpE13;struct Cyc_List_List*_tmpE14;_LL6C0: {struct
Cyc_Absyn_FnType_struct*_tmpE09=(struct Cyc_Absyn_FnType_struct*)_tmpE08;if(
_tmpE09->tag != 9)goto _LL6C2;else{_tmpE0A=_tmpE09->f1;_tmpE0B=_tmpE0A.tvars;
_tmpE0C=(struct Cyc_List_List**)&(_tmpE09->f1).tvars;_tmpE0D=_tmpE0A.effect;
_tmpE0E=_tmpE0A.ret_tqual;_tmpE0F=_tmpE0A.ret_typ;_tmpE10=_tmpE0A.args;_tmpE11=
_tmpE0A.c_varargs;_tmpE12=_tmpE0A.cyc_varargs;_tmpE13=_tmpE0A.rgn_po;_tmpE14=
_tmpE0A.attributes;}}_LL6C1: if(*_tmpE0C == 0){*_tmpE0C=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_copy)(_tmpDCC);_tmpDCC=0;}goto _LL6BF;_LL6C2:;
_LL6C3: goto _LL6BF;_LL6BF:;}if(_tmpDCC != 0){const char*_tmp1579;void*_tmp1578[1];
struct Cyc_String_pa_struct _tmp1577;(_tmp1577.tag=0,((_tmp1577.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpDCC->hd)->name),((
_tmp1578[0]=& _tmp1577,Cyc_Tcutil_terr(loc,((_tmp1579="unbound type variable %s",
_tag_dyneither(_tmp1579,sizeof(char),25))),_tag_dyneither(_tmp1578,sizeof(void*),
1)))))));}if(_tmpDCD != 0)for(0;_tmpDCD != 0;_tmpDCD=_tmpDCD->tl){void*e=(void*)
_tmpDCD->hd;struct Cyc_Absyn_Kind*_tmpE18=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind
_tmpE19;enum Cyc_Absyn_KindQual _tmpE1A;enum Cyc_Absyn_AliasQual _tmpE1B;struct Cyc_Absyn_Kind
_tmpE1C;enum Cyc_Absyn_KindQual _tmpE1D;enum Cyc_Absyn_AliasQual _tmpE1E;struct Cyc_Absyn_Kind
_tmpE1F;enum Cyc_Absyn_KindQual _tmpE20;enum Cyc_Absyn_AliasQual _tmpE21;struct Cyc_Absyn_Kind
_tmpE22;enum Cyc_Absyn_KindQual _tmpE23;_LL6C5: _tmpE19=*_tmpE18;_tmpE1A=_tmpE19.kind;
if(_tmpE1A != Cyc_Absyn_RgnKind)goto _LL6C7;_tmpE1B=_tmpE19.aliasqual;if(_tmpE1B != 
Cyc_Absyn_Unique)goto _LL6C7;_LL6C6: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp157C;void*_tmp157B;(_tmp157B=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp157C="can't unify evar with unique region!",
_tag_dyneither(_tmp157C,sizeof(char),37))),_tag_dyneither(_tmp157B,sizeof(void*),
0)));}goto _LL6C4;_LL6C7: _tmpE1C=*_tmpE18;_tmpE1D=_tmpE1C.kind;if(_tmpE1D != Cyc_Absyn_RgnKind)
goto _LL6C9;_tmpE1E=_tmpE1C.aliasqual;if(_tmpE1E != Cyc_Absyn_Aliasable)goto _LL6C9;
_LL6C8: goto _LL6CA;_LL6C9: _tmpE1F=*_tmpE18;_tmpE20=_tmpE1F.kind;if(_tmpE20 != Cyc_Absyn_RgnKind)
goto _LL6CB;_tmpE21=_tmpE1F.aliasqual;if(_tmpE21 != Cyc_Absyn_Top)goto _LL6CB;
_LL6CA: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp157F;
void*_tmp157E;(_tmp157E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp157F="can't unify evar with heap!",_tag_dyneither(
_tmp157F,sizeof(char),28))),_tag_dyneither(_tmp157E,sizeof(void*),0)));}goto
_LL6C4;_LL6CB: _tmpE22=*_tmpE18;_tmpE23=_tmpE22.kind;if(_tmpE23 != Cyc_Absyn_EffKind)
goto _LL6CD;_LL6CC: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp1582;void*_tmp1581;(_tmp1581=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1582="can't unify evar with {}!",
_tag_dyneither(_tmp1582,sizeof(char),26))),_tag_dyneither(_tmp1581,sizeof(void*),
0)));}goto _LL6C4;_LL6CD:;_LL6CE:{const char*_tmp1587;void*_tmp1586[2];struct Cyc_String_pa_struct
_tmp1585;struct Cyc_String_pa_struct _tmp1584;(_tmp1584.tag=0,((_tmp1584.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp1585.tag=0,((_tmp1585.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp1586[0]=& _tmp1585,((_tmp1586[1]=& _tmp1584,Cyc_Tcutil_terr(loc,((
_tmp1587="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp1587,sizeof(char),52))),_tag_dyneither(_tmp1586,sizeof(void*),2)))))))))))));}
goto _LL6C4;_LL6C4:;}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{void*_tmpE2F=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmpE31;struct Cyc_List_List*_tmpE32;struct Cyc_Core_Opt*
_tmpE33;struct Cyc_Absyn_Tqual _tmpE34;void*_tmpE35;struct Cyc_List_List*_tmpE36;
_LL6D0: {struct Cyc_Absyn_FnType_struct*_tmpE30=(struct Cyc_Absyn_FnType_struct*)
_tmpE2F;if(_tmpE30->tag != 9)goto _LL6D2;else{_tmpE31=_tmpE30->f1;_tmpE32=_tmpE31.tvars;
_tmpE33=_tmpE31.effect;_tmpE34=_tmpE31.ret_tqual;_tmpE35=_tmpE31.ret_typ;_tmpE36=
_tmpE31.args;}}_LL6D1: fd->tvs=_tmpE32;fd->effect=_tmpE33;{struct Cyc_List_List*
_tmpE37=fd->args;for(0;_tmpE37 != 0;(_tmpE37=_tmpE37->tl,_tmpE36=_tmpE36->tl)){(*((
struct _tuple26*)_tmpE37->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmpE36))->hd)).f2;(*((struct _tuple26*)_tmpE37->hd)).f3=(*((struct
_tuple10*)_tmpE36->hd)).f3;}}fd->ret_tqual=_tmpE34;fd->ret_type=_tmpE35;(fd->ret_tqual).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpE35);
goto _LL6CF;_LL6D2:;_LL6D3: {const char*_tmp158A;void*_tmp1589;(_tmp1589=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp158A="check_fndecl_valid_type: not a FnType",
_tag_dyneither(_tmp158A,sizeof(char),38))),_tag_dyneither(_tmp1589,sizeof(void*),
0)));}_LL6CF:;}{struct _RegionHandle*_tmpE3A=Cyc_Tcenv_get_fnrgn(te);{const char*
_tmp158B;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple26*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpE3A,(struct _dyneither_ptr*(*)(struct _tuple26*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,((_tmp158B="function declaration has repeated parameter",
_tag_dyneither(_tmp158B,sizeof(char),44))));}{struct Cyc_Core_Opt*_tmp158C;fd->cached_typ=((
_tmp158C=_cycalloc(sizeof(*_tmp158C)),((_tmp158C->v=t,_tmp158C))));};};}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){struct
_RegionHandle*_tmpE3D=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmp158D;
struct Cyc_Tcutil_CVTEnv _tmpE3E=Cyc_Tcutil_check_valid_type(loc,te,((_tmp158D.r=
_tmpE3D,((_tmp158D.kind_env=bound_tvars,((_tmp158D.free_vars=0,((_tmp158D.free_evars=
0,((_tmp158D.generalize_evars=0,((_tmp158D.fn_result=0,_tmp158D)))))))))))),
expected_kind,t);struct Cyc_List_List*_tmpE3F=Cyc_Tcutil_remove_bound_tvars(
_tmpE3D,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple30*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmpE3D,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple30*))Cyc_Core_fst,_tmpE3E.free_vars),bound_tvars);struct Cyc_List_List*
_tmpE40=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple31*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmpE3D,(void*(*)(struct _tuple31*))Cyc_Core_fst,
_tmpE3E.free_evars);{struct Cyc_List_List*fs=_tmpE3F;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpE41=((struct Cyc_Absyn_Tvar*)fs->hd)->name;const char*
_tmp1592;void*_tmp1591[2];struct Cyc_String_pa_struct _tmp1590;struct Cyc_String_pa_struct
_tmp158F;(_tmp158F.tag=0,((_tmp158F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1590.tag=0,((_tmp1590.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmpE41),((_tmp1591[0]=& _tmp1590,((
_tmp1591[1]=& _tmp158F,Cyc_Tcutil_terr(loc,((_tmp1592="unbound type variable %s in type %s",
_tag_dyneither(_tmp1592,sizeof(char),36))),_tag_dyneither(_tmp1591,sizeof(void*),
2)))))))))))));}}if(!allow_evars  && _tmpE40 != 0)for(0;_tmpE40 != 0;_tmpE40=
_tmpE40->tl){void*e=(void*)_tmpE40->hd;struct Cyc_Absyn_Kind*_tmpE46=Cyc_Tcutil_typ_kind(
e);struct Cyc_Absyn_Kind _tmpE47;enum Cyc_Absyn_KindQual _tmpE48;enum Cyc_Absyn_AliasQual
_tmpE49;struct Cyc_Absyn_Kind _tmpE4A;enum Cyc_Absyn_KindQual _tmpE4B;enum Cyc_Absyn_AliasQual
_tmpE4C;struct Cyc_Absyn_Kind _tmpE4D;enum Cyc_Absyn_KindQual _tmpE4E;enum Cyc_Absyn_AliasQual
_tmpE4F;struct Cyc_Absyn_Kind _tmpE50;enum Cyc_Absyn_KindQual _tmpE51;_LL6D5:
_tmpE47=*_tmpE46;_tmpE48=_tmpE47.kind;if(_tmpE48 != Cyc_Absyn_RgnKind)goto _LL6D7;
_tmpE49=_tmpE47.aliasqual;if(_tmpE49 != Cyc_Absyn_Unique)goto _LL6D7;_LL6D6: if(!
Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){const char*_tmp1595;void*
_tmp1594;(_tmp1594=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1595="can't unify evar with unique region!",
_tag_dyneither(_tmp1595,sizeof(char),37))),_tag_dyneither(_tmp1594,sizeof(void*),
0)));}goto _LL6D4;_LL6D7: _tmpE4A=*_tmpE46;_tmpE4B=_tmpE4A.kind;if(_tmpE4B != Cyc_Absyn_RgnKind)
goto _LL6D9;_tmpE4C=_tmpE4A.aliasqual;if(_tmpE4C != Cyc_Absyn_Aliasable)goto _LL6D9;
_LL6D8: goto _LL6DA;_LL6D9: _tmpE4D=*_tmpE46;_tmpE4E=_tmpE4D.kind;if(_tmpE4E != Cyc_Absyn_RgnKind)
goto _LL6DB;_tmpE4F=_tmpE4D.aliasqual;if(_tmpE4F != Cyc_Absyn_Top)goto _LL6DB;
_LL6DA: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1598;
void*_tmp1597;(_tmp1597=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp1598="can't unify evar with heap!",_tag_dyneither(
_tmp1598,sizeof(char),28))),_tag_dyneither(_tmp1597,sizeof(void*),0)));}goto
_LL6D4;_LL6DB: _tmpE50=*_tmpE46;_tmpE51=_tmpE50.kind;if(_tmpE51 != Cyc_Absyn_EffKind)
goto _LL6DD;_LL6DC: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp159B;void*_tmp159A;(_tmp159A=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp159B="can't unify evar with {}!",
_tag_dyneither(_tmp159B,sizeof(char),26))),_tag_dyneither(_tmp159A,sizeof(void*),
0)));}goto _LL6D4;_LL6DD:;_LL6DE:{const char*_tmp15A0;void*_tmp159F[2];struct Cyc_String_pa_struct
_tmp159E;struct Cyc_String_pa_struct _tmp159D;(_tmp159D.tag=0,((_tmp159D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp159E.tag=0,((_tmp159E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp159F[0]=& _tmp159E,((_tmp159F[1]=& _tmp159D,Cyc_Tcutil_terr(loc,((
_tmp15A0="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp15A0,sizeof(char),52))),_tag_dyneither(_tmp159F,sizeof(void*),2)))))))))))));}
goto _LL6D4;_LL6D4:;}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
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
_tag_dyneither(_tmp15A6,sizeof(char),24))));}struct _tuple33{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple34{struct Cyc_List_List*f1;void*f2;};struct _tuple35{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){struct Cyc_List_List*
fields=0;{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=
sd_fields->tl){const char*_tmp15A7;if(Cyc_strcmp((struct _dyneither_ptr)*((struct
Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp15A7="",_tag_dyneither(_tmp15A7,
sizeof(char),1))))!= 0){struct _tuple33*_tmp15AA;struct Cyc_List_List*_tmp15A9;
fields=((_tmp15A9=_cycalloc(sizeof(*_tmp15A9)),((_tmp15A9->hd=((_tmp15AA=
_cycalloc(sizeof(*_tmp15AA)),((_tmp15AA->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd,((_tmp15AA->f2=0,_tmp15AA)))))),((_tmp15A9->tl=fields,_tmp15A9))))));}}}
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
const char*_tmp15AC;const char*_tmp15AB;struct _dyneither_ptr aggr_str=aggr_kind == 
Cyc_Absyn_StructA?(_tmp15AC="struct",_tag_dyneither(_tmp15AC,sizeof(char),7)):((
_tmp15AB="union",_tag_dyneither(_tmp15AB,sizeof(char),6)));struct Cyc_List_List*
ans=0;for(0;des != 0;des=des->tl){struct _tuple34 _tmpE66;struct Cyc_List_List*
_tmpE67;void*_tmpE68;struct _tuple34*_tmpE65=(struct _tuple34*)des->hd;_tmpE66=*
_tmpE65;_tmpE67=_tmpE66.f1;_tmpE68=_tmpE66.f2;if(_tmpE67 == 0){struct Cyc_List_List*
_tmpE69=fields;for(0;_tmpE69 != 0;_tmpE69=_tmpE69->tl){if(!(*((struct _tuple33*)
_tmpE69->hd)).f2){(*((struct _tuple33*)_tmpE69->hd)).f2=1;{struct Cyc_Absyn_FieldName_struct*
_tmp15B2;struct Cyc_Absyn_FieldName_struct _tmp15B1;struct Cyc_List_List*_tmp15B0;(*((
struct _tuple34*)des->hd)).f1=(struct Cyc_List_List*)((_tmp15B0=_cycalloc(sizeof(*
_tmp15B0)),((_tmp15B0->hd=(void*)((_tmp15B2=_cycalloc(sizeof(*_tmp15B2)),((
_tmp15B2[0]=((_tmp15B1.tag=1,((_tmp15B1.f1=((*((struct _tuple33*)_tmpE69->hd)).f1)->name,
_tmp15B1)))),_tmp15B2)))),((_tmp15B0->tl=0,_tmp15B0))))));}{struct _tuple35*
_tmp15B5;struct Cyc_List_List*_tmp15B4;ans=((_tmp15B4=_region_malloc(rgn,sizeof(*
_tmp15B4)),((_tmp15B4->hd=((_tmp15B5=_region_malloc(rgn,sizeof(*_tmp15B5)),((
_tmp15B5->f1=(*((struct _tuple33*)_tmpE69->hd)).f1,((_tmp15B5->f2=_tmpE68,
_tmp15B5)))))),((_tmp15B4->tl=ans,_tmp15B4))))));}break;}}if(_tmpE69 == 0){const
char*_tmp15B9;void*_tmp15B8[1];struct Cyc_String_pa_struct _tmp15B7;(_tmp15B7.tag=
0,((_tmp15B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((
_tmp15B8[0]=& _tmp15B7,Cyc_Tcutil_terr(loc,((_tmp15B9="too many arguments to %s",
_tag_dyneither(_tmp15B9,sizeof(char),25))),_tag_dyneither(_tmp15B8,sizeof(void*),
1)))))));}}else{if(_tmpE67->tl != 0){const char*_tmp15BC;void*_tmp15BB;(_tmp15BB=0,
Cyc_Tcutil_terr(loc,((_tmp15BC="multiple designators are not yet supported",
_tag_dyneither(_tmp15BC,sizeof(char),43))),_tag_dyneither(_tmp15BB,sizeof(void*),
0)));}else{void*_tmpE74=(void*)_tmpE67->hd;struct _dyneither_ptr*_tmpE77;_LL6E0: {
struct Cyc_Absyn_ArrayElement_struct*_tmpE75=(struct Cyc_Absyn_ArrayElement_struct*)
_tmpE74;if(_tmpE75->tag != 0)goto _LL6E2;}_LL6E1:{const char*_tmp15C0;void*_tmp15BF[
1];struct Cyc_String_pa_struct _tmp15BE;(_tmp15BE.tag=0,((_tmp15BE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp15BF[0]=& _tmp15BE,Cyc_Tcutil_terr(
loc,((_tmp15C0="array designator used in argument to %s",_tag_dyneither(_tmp15C0,
sizeof(char),40))),_tag_dyneither(_tmp15BF,sizeof(void*),1)))))));}goto _LL6DF;
_LL6E2: {struct Cyc_Absyn_FieldName_struct*_tmpE76=(struct Cyc_Absyn_FieldName_struct*)
_tmpE74;if(_tmpE76->tag != 1)goto _LL6DF;else{_tmpE77=_tmpE76->f1;}}_LL6E3: {
struct Cyc_List_List*_tmpE7B=fields;for(0;_tmpE7B != 0;_tmpE7B=_tmpE7B->tl){if(Cyc_strptrcmp(
_tmpE77,((*((struct _tuple33*)_tmpE7B->hd)).f1)->name)== 0){if((*((struct _tuple33*)
_tmpE7B->hd)).f2){const char*_tmp15C4;void*_tmp15C3[1];struct Cyc_String_pa_struct
_tmp15C2;(_tmp15C2.tag=0,((_tmp15C2.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmpE77),((_tmp15C3[0]=& _tmp15C2,Cyc_Tcutil_terr(loc,((_tmp15C4="member %s has already been used as an argument",
_tag_dyneither(_tmp15C4,sizeof(char),47))),_tag_dyneither(_tmp15C3,sizeof(void*),
1)))))));}(*((struct _tuple33*)_tmpE7B->hd)).f2=1;{struct _tuple35*_tmp15C7;struct
Cyc_List_List*_tmp15C6;ans=((_tmp15C6=_region_malloc(rgn,sizeof(*_tmp15C6)),((
_tmp15C6->hd=((_tmp15C7=_region_malloc(rgn,sizeof(*_tmp15C7)),((_tmp15C7->f1=(*((
struct _tuple33*)_tmpE7B->hd)).f1,((_tmp15C7->f2=_tmpE68,_tmp15C7)))))),((
_tmp15C6->tl=ans,_tmp15C6))))));}break;}}if(_tmpE7B == 0){const char*_tmp15CB;void*
_tmp15CA[1];struct Cyc_String_pa_struct _tmp15C9;(_tmp15C9.tag=0,((_tmp15C9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE77),((_tmp15CA[0]=& _tmp15C9,
Cyc_Tcutil_terr(loc,((_tmp15CB="bad field designator %s",_tag_dyneither(_tmp15CB,
sizeof(char),24))),_tag_dyneither(_tmp15CA,sizeof(void*),1)))))));}goto _LL6DF;}
_LL6DF:;}}}if(aggr_kind == Cyc_Absyn_StructA)for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple33*)fields->hd)).f2){{const char*_tmp15CE;void*_tmp15CD;(
_tmp15CD=0,Cyc_Tcutil_terr(loc,((_tmp15CE="too few arguments to struct",
_tag_dyneither(_tmp15CE,sizeof(char),28))),_tag_dyneither(_tmp15CD,sizeof(void*),
0)));}break;}}else{int found=0;for(0;fields != 0;fields=fields->tl){if((*((struct
_tuple33*)fields->hd)).f2){if(found){const char*_tmp15D1;void*_tmp15D0;(_tmp15D0=
0,Cyc_Tcutil_terr(loc,((_tmp15D1="only one member of a union is allowed",
_tag_dyneither(_tmp15D1,sizeof(char),38))),_tag_dyneither(_tmp15D0,sizeof(void*),
0)));}found=1;}}if(!found){const char*_tmp15D4;void*_tmp15D3;(_tmp15D3=0,Cyc_Tcutil_terr(
loc,((_tmp15D4="missing member for union",_tag_dyneither(_tmp15D4,sizeof(char),
25))),_tag_dyneither(_tmp15D3,sizeof(void*),0)));}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);};}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpE8C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpE8E;void*_tmpE8F;struct Cyc_Absyn_PtrAtts _tmpE90;union Cyc_Absyn_Constraint*
_tmpE91;_LL6E5: {struct Cyc_Absyn_PointerType_struct*_tmpE8D=(struct Cyc_Absyn_PointerType_struct*)
_tmpE8C;if(_tmpE8D->tag != 5)goto _LL6E7;else{_tmpE8E=_tmpE8D->f1;_tmpE8F=_tmpE8E.elt_typ;
_tmpE90=_tmpE8E.ptr_atts;_tmpE91=_tmpE90.bounds;}}_LL6E6: {void*_tmpE92=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpE91);_LL6EA: {struct Cyc_Absyn_DynEither_b_struct*_tmpE93=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpE92;if(_tmpE93->tag != 0)goto _LL6EC;}_LL6EB:*elt_typ_dest=_tmpE8F;return 1;
_LL6EC:;_LL6ED: return 0;_LL6E9:;}_LL6E7:;_LL6E8: return 0;_LL6E4:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpE94=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpE96;void*_tmpE97;struct Cyc_Absyn_PtrAtts _tmpE98;union Cyc_Absyn_Constraint*
_tmpE99;_LL6EF: {struct Cyc_Absyn_PointerType_struct*_tmpE95=(struct Cyc_Absyn_PointerType_struct*)
_tmpE94;if(_tmpE95->tag != 5)goto _LL6F1;else{_tmpE96=_tmpE95->f1;_tmpE97=_tmpE96.elt_typ;
_tmpE98=_tmpE96.ptr_atts;_tmpE99=_tmpE98.zero_term;}}_LL6F0:*elt_typ_dest=
_tmpE97;return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpE99);_LL6F1:;_LL6F2: return 0;_LL6EE:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,
void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpE9A=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpE9C;void*_tmpE9D;struct Cyc_Absyn_PtrAtts _tmpE9E;
union Cyc_Absyn_Constraint*_tmpE9F;union Cyc_Absyn_Constraint*_tmpEA0;struct Cyc_Absyn_ArrayInfo
_tmpEA2;void*_tmpEA3;struct Cyc_Absyn_Tqual _tmpEA4;struct Cyc_Absyn_Exp*_tmpEA5;
union Cyc_Absyn_Constraint*_tmpEA6;_LL6F4: {struct Cyc_Absyn_PointerType_struct*
_tmpE9B=(struct Cyc_Absyn_PointerType_struct*)_tmpE9A;if(_tmpE9B->tag != 5)goto
_LL6F6;else{_tmpE9C=_tmpE9B->f1;_tmpE9D=_tmpE9C.elt_typ;_tmpE9E=_tmpE9C.ptr_atts;
_tmpE9F=_tmpE9E.bounds;_tmpEA0=_tmpE9E.zero_term;}}_LL6F5: if(((int(*)(int y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEA0)){*ptr_type=t;*elt_type=
_tmpE9D;{void*_tmpEA7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmpE9F);_LL6FB: {struct Cyc_Absyn_DynEither_b_struct*
_tmpEA8=(struct Cyc_Absyn_DynEither_b_struct*)_tmpEA7;if(_tmpEA8->tag != 0)goto
_LL6FD;}_LL6FC:*is_dyneither=1;goto _LL6FA;_LL6FD:;_LL6FE:*is_dyneither=0;goto
_LL6FA;_LL6FA:;}return 1;}else{return 0;}_LL6F6: {struct Cyc_Absyn_ArrayType_struct*
_tmpEA1=(struct Cyc_Absyn_ArrayType_struct*)_tmpE9A;if(_tmpEA1->tag != 8)goto
_LL6F8;else{_tmpEA2=_tmpEA1->f1;_tmpEA3=_tmpEA2.elt_type;_tmpEA4=_tmpEA2.tq;
_tmpEA5=_tmpEA2.num_elts;_tmpEA6=_tmpEA2.zero_term;}}_LL6F7: if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEA6)){*elt_type=_tmpEA3;*
is_dyneither=0;{struct Cyc_Absyn_PointerType_struct _tmp15E9;struct Cyc_Absyn_PtrAtts
_tmp15E8;struct Cyc_Absyn_Upper_b_struct _tmp15E7;struct Cyc_Absyn_Upper_b_struct*
_tmp15E6;struct Cyc_Absyn_PtrInfo _tmp15E5;struct Cyc_Absyn_PointerType_struct*
_tmp15E4;*ptr_type=(void*)((_tmp15E4=_cycalloc(sizeof(*_tmp15E4)),((_tmp15E4[0]=((
_tmp15E9.tag=5,((_tmp15E9.f1=((_tmp15E5.elt_typ=_tmpEA3,((_tmp15E5.elt_tq=
_tmpEA4,((_tmp15E5.ptr_atts=((_tmp15E8.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((
_tmp15E8.nullable=Cyc_Absyn_false_conref,((_tmp15E8.bounds=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp15E6=_cycalloc(sizeof(*_tmp15E6)),((
_tmp15E6[0]=((_tmp15E7.tag=1,((_tmp15E7.f1=(struct Cyc_Absyn_Exp*)_check_null(
_tmpEA5),_tmp15E7)))),_tmp15E6))))),((_tmp15E8.zero_term=_tmpEA6,((_tmp15E8.ptrloc=
0,_tmp15E8)))))))))),_tmp15E5)))))),_tmp15E9)))),_tmp15E4))));}return 1;}else{
return 0;}_LL6F8:;_LL6F9: return 0;_LL6F3:;}int Cyc_Tcutil_is_zero_ptr_deref(struct
Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpEAF=e1->r;
struct Cyc_Absyn_Exp*_tmpEB2;struct Cyc_Absyn_Exp*_tmpEB4;struct Cyc_Absyn_Exp*
_tmpEB6;struct Cyc_Absyn_Exp*_tmpEB8;struct Cyc_Absyn_Exp*_tmpEBA;struct Cyc_Absyn_Exp*
_tmpEBC;_LL700: {struct Cyc_Absyn_Cast_e_struct*_tmpEB0=(struct Cyc_Absyn_Cast_e_struct*)
_tmpEAF;if(_tmpEB0->tag != 15)goto _LL702;}_LL701: {const char*_tmp15ED;void*
_tmp15EC[1];struct Cyc_String_pa_struct _tmp15EB;(_tmp15EB.tag=0,((_tmp15EB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp15EC[0]=& _tmp15EB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp15ED="we have a cast in a lhs:  %s",_tag_dyneither(
_tmp15ED,sizeof(char),29))),_tag_dyneither(_tmp15EC,sizeof(void*),1)))))));}
_LL702: {struct Cyc_Absyn_Deref_e_struct*_tmpEB1=(struct Cyc_Absyn_Deref_e_struct*)
_tmpEAF;if(_tmpEB1->tag != 21)goto _LL704;else{_tmpEB2=_tmpEB1->f1;}}_LL703:
_tmpEB4=_tmpEB2;goto _LL705;_LL704: {struct Cyc_Absyn_Subscript_e_struct*_tmpEB3=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpEAF;if(_tmpEB3->tag != 24)goto _LL706;
else{_tmpEB4=_tmpEB3->f1;}}_LL705: return Cyc_Tcutil_is_zero_ptr_type((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpEB4->topt))->v,ptr_type,is_dyneither,
elt_type);_LL706: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpEB5=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpEAF;if(_tmpEB5->tag != 23)goto _LL708;else{_tmpEB6=_tmpEB5->f1;}}_LL707:
_tmpEB8=_tmpEB6;goto _LL709;_LL708: {struct Cyc_Absyn_AggrMember_e_struct*_tmpEB7=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmpEAF;if(_tmpEB7->tag != 22)goto _LL70A;
else{_tmpEB8=_tmpEB7->f1;}}_LL709: if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct
Cyc_Core_Opt*)_check_null(_tmpEB8->topt))->v,ptr_type,is_dyneither,elt_type)){
const char*_tmp15F1;void*_tmp15F0[1];struct Cyc_String_pa_struct _tmp15EF;(_tmp15EF.tag=
0,((_tmp15EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp15F0[0]=& _tmp15EF,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15F1="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp15F1,sizeof(char),51))),_tag_dyneither(_tmp15F0,sizeof(void*),
1)))))));}return 0;_LL70A: {struct Cyc_Absyn_Instantiate_e_struct*_tmpEB9=(struct
Cyc_Absyn_Instantiate_e_struct*)_tmpEAF;if(_tmpEB9->tag != 14)goto _LL70C;else{
_tmpEBA=_tmpEB9->f1;}}_LL70B: _tmpEBC=_tmpEBA;goto _LL70D;_LL70C: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpEBB=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpEAF;if(_tmpEBB->tag != 13)
goto _LL70E;else{_tmpEBC=_tmpEBB->f1;}}_LL70D: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpEBC->topt))->v,ptr_type,is_dyneither,
elt_type)){const char*_tmp15F5;void*_tmp15F4[1];struct Cyc_String_pa_struct
_tmp15F3;(_tmp15F3.tag=0,((_tmp15F3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp15F4[0]=& _tmp15F3,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15F5="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp15F5,sizeof(char),49))),_tag_dyneither(_tmp15F4,sizeof(void*),
1)))))));}return 0;_LL70E: {struct Cyc_Absyn_Var_e_struct*_tmpEBD=(struct Cyc_Absyn_Var_e_struct*)
_tmpEAF;if(_tmpEBD->tag != 1)goto _LL710;}_LL70F: return 0;_LL710:;_LL711: {const
char*_tmp15F9;void*_tmp15F8[1];struct Cyc_String_pa_struct _tmp15F7;(_tmp15F7.tag=
0,((_tmp15F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp15F8[0]=& _tmp15F7,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15F9="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp15F9,sizeof(char),39))),_tag_dyneither(_tmp15F8,sizeof(void*),
1)))))));}_LL6FF:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t){void*ignore=(void*)&
Cyc_Absyn_VoidType_val;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){void*_tmpECA=Cyc_Tcutil_compress(
r);struct Cyc_Absyn_Tvar*_tmpECE;_LL713: {struct Cyc_Absyn_RefCntRgn_struct*
_tmpECB=(struct Cyc_Absyn_RefCntRgn_struct*)_tmpECA;if(_tmpECB->tag != 22)goto
_LL715;}_LL714: return !must_be_unique;_LL715: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpECC=(struct Cyc_Absyn_UniqueRgn_struct*)_tmpECA;if(_tmpECC->tag != 21)goto
_LL717;}_LL716: return 1;_LL717: {struct Cyc_Absyn_VarType_struct*_tmpECD=(struct
Cyc_Absyn_VarType_struct*)_tmpECA;if(_tmpECD->tag != 2)goto _LL719;else{_tmpECE=
_tmpECD->f1;}}_LL718: {struct Cyc_Absyn_Kind _tmpED0;enum Cyc_Absyn_KindQual
_tmpED1;enum Cyc_Absyn_AliasQual _tmpED2;struct Cyc_Absyn_Kind*_tmpECF=Cyc_Tcutil_tvar_kind(
_tmpECE,& Cyc_Tcutil_rk);_tmpED0=*_tmpECF;_tmpED1=_tmpED0.kind;_tmpED2=_tmpED0.aliasqual;
if(_tmpED1 == Cyc_Absyn_RgnKind  && (_tmpED2 == Cyc_Absyn_Unique  || _tmpED2 == Cyc_Absyn_Top
 && !must_be_unique)){void*_tmpED3=Cyc_Absyn_compress_kb(_tmpECE->kind);struct
Cyc_Core_Opt*_tmpED5;struct Cyc_Core_Opt**_tmpED6;struct Cyc_Absyn_Kind*_tmpED7;
struct Cyc_Absyn_Kind _tmpED8;enum Cyc_Absyn_KindQual _tmpED9;enum Cyc_Absyn_AliasQual
_tmpEDA;_LL71C: {struct Cyc_Absyn_Less_kb_struct*_tmpED4=(struct Cyc_Absyn_Less_kb_struct*)
_tmpED3;if(_tmpED4->tag != 2)goto _LL71E;else{_tmpED5=_tmpED4->f1;_tmpED6=(struct
Cyc_Core_Opt**)& _tmpED4->f1;_tmpED7=_tmpED4->f2;_tmpED8=*_tmpED7;_tmpED9=_tmpED8.kind;
if(_tmpED9 != Cyc_Absyn_RgnKind)goto _LL71E;_tmpEDA=_tmpED8.aliasqual;if(_tmpEDA != 
Cyc_Absyn_Top)goto _LL71E;}}_LL71D:{struct Cyc_Absyn_Less_kb_struct*_tmp15FF;
struct Cyc_Absyn_Less_kb_struct _tmp15FE;struct Cyc_Core_Opt*_tmp15FD;*_tmpED6=((
_tmp15FD=_cycalloc(sizeof(*_tmp15FD)),((_tmp15FD->v=(void*)((_tmp15FF=_cycalloc(
sizeof(*_tmp15FF)),((_tmp15FF[0]=((_tmp15FE.tag=2,((_tmp15FE.f1=0,((_tmp15FE.f2=&
Cyc_Tcutil_rk,_tmp15FE)))))),_tmp15FF)))),_tmp15FD))));}return 0;_LL71E:;_LL71F:
return 1;_LL71B:;}return 0;}_LL719:;_LL71A: return 0;_LL712:;}static int Cyc_Tcutil_is_noalias_pointer_aux(
void*t,int must_be_unique){void*_tmpEDE=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpEE0;struct Cyc_Absyn_PtrAtts _tmpEE1;void*_tmpEE2;struct Cyc_Absyn_Tvar*_tmpEE4;
_LL721: {struct Cyc_Absyn_PointerType_struct*_tmpEDF=(struct Cyc_Absyn_PointerType_struct*)
_tmpEDE;if(_tmpEDF->tag != 5)goto _LL723;else{_tmpEE0=_tmpEDF->f1;_tmpEE1=_tmpEE0.ptr_atts;
_tmpEE2=_tmpEE1.rgn;}}_LL722: return Cyc_Tcutil_is_noalias_region(_tmpEE2,
must_be_unique);_LL723: {struct Cyc_Absyn_VarType_struct*_tmpEE3=(struct Cyc_Absyn_VarType_struct*)
_tmpEDE;if(_tmpEE3->tag != 2)goto _LL725;else{_tmpEE4=_tmpEE3->f1;}}_LL724: {
struct Cyc_Absyn_Kind _tmpEE6;enum Cyc_Absyn_KindQual _tmpEE7;enum Cyc_Absyn_AliasQual
_tmpEE8;struct Cyc_Absyn_Kind*_tmpEE5=Cyc_Tcutil_tvar_kind(_tmpEE4,& Cyc_Tcutil_bk);
_tmpEE6=*_tmpEE5;_tmpEE7=_tmpEE6.kind;_tmpEE8=_tmpEE6.aliasqual;switch(_tmpEE7){
case Cyc_Absyn_BoxKind: _LL727: goto _LL728;case Cyc_Absyn_AnyKind: _LL728: goto _LL729;
case Cyc_Absyn_MemKind: _LL729: if(_tmpEE8 == Cyc_Absyn_Unique  || _tmpEE8 == Cyc_Absyn_Top){
void*_tmpEE9=Cyc_Absyn_compress_kb(_tmpEE4->kind);struct Cyc_Core_Opt*_tmpEEB;
struct Cyc_Core_Opt**_tmpEEC;struct Cyc_Absyn_Kind*_tmpEED;struct Cyc_Absyn_Kind
_tmpEEE;enum Cyc_Absyn_KindQual _tmpEEF;enum Cyc_Absyn_AliasQual _tmpEF0;_LL72C: {
struct Cyc_Absyn_Less_kb_struct*_tmpEEA=(struct Cyc_Absyn_Less_kb_struct*)_tmpEE9;
if(_tmpEEA->tag != 2)goto _LL72E;else{_tmpEEB=_tmpEEA->f1;_tmpEEC=(struct Cyc_Core_Opt**)&
_tmpEEA->f1;_tmpEED=_tmpEEA->f2;_tmpEEE=*_tmpEED;_tmpEEF=_tmpEEE.kind;_tmpEF0=
_tmpEEE.aliasqual;if(_tmpEF0 != Cyc_Absyn_Top)goto _LL72E;}}_LL72D:{struct Cyc_Absyn_Less_kb_struct*
_tmp1609;struct Cyc_Absyn_Kind*_tmp1608;struct Cyc_Absyn_Less_kb_struct _tmp1607;
struct Cyc_Core_Opt*_tmp1606;*_tmpEEC=((_tmp1606=_cycalloc(sizeof(*_tmp1606)),((
_tmp1606->v=(void*)((_tmp1609=_cycalloc(sizeof(*_tmp1609)),((_tmp1609[0]=((
_tmp1607.tag=2,((_tmp1607.f1=0,((_tmp1607.f2=((_tmp1608=_cycalloc_atomic(sizeof(*
_tmp1608)),((_tmp1608->kind=_tmpEEF,((_tmp1608->aliasqual=Cyc_Absyn_Aliasable,
_tmp1608)))))),_tmp1607)))))),_tmp1609)))),_tmp1606))));}return 0;_LL72E:;_LL72F:
return 1;_LL72B:;}return 0;default: _LL72A: return 0;}}_LL725:;_LL726: return 0;_LL720:;}
int Cyc_Tcutil_is_noalias_pointer(void*t){return Cyc_Tcutil_is_noalias_pointer_aux(
t,0);}int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpEF5=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(_tmpEF5))
return 1;{void*_tmpEF6=_tmpEF5;struct Cyc_List_List*_tmpEF8;struct Cyc_Absyn_AggrInfo
_tmpEFA;union Cyc_Absyn_AggrInfoU _tmpEFB;struct Cyc_Absyn_Aggrdecl**_tmpEFC;struct
Cyc_List_List*_tmpEFD;struct Cyc_List_List*_tmpEFF;struct Cyc_Absyn_AggrInfo
_tmpF01;union Cyc_Absyn_AggrInfoU _tmpF02;struct _tuple4 _tmpF03;struct Cyc_Absyn_DatatypeInfo
_tmpF05;union Cyc_Absyn_DatatypeInfoU _tmpF06;struct Cyc_List_List*_tmpF07;struct
Cyc_Absyn_DatatypeFieldInfo _tmpF09;union Cyc_Absyn_DatatypeFieldInfoU _tmpF0A;
struct Cyc_List_List*_tmpF0B;_LL732: {struct Cyc_Absyn_TupleType_struct*_tmpEF7=(
struct Cyc_Absyn_TupleType_struct*)_tmpEF6;if(_tmpEF7->tag != 10)goto _LL734;else{
_tmpEF8=_tmpEF7->f1;}}_LL733: while(_tmpEF8 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,(*((struct _tuple12*)_tmpEF8->hd)).f2))return 1;_tmpEF8=_tmpEF8->tl;}return 0;
_LL734: {struct Cyc_Absyn_AggrType_struct*_tmpEF9=(struct Cyc_Absyn_AggrType_struct*)
_tmpEF6;if(_tmpEF9->tag != 11)goto _LL736;else{_tmpEFA=_tmpEF9->f1;_tmpEFB=_tmpEFA.aggr_info;
if((_tmpEFB.KnownAggr).tag != 2)goto _LL736;_tmpEFC=(struct Cyc_Absyn_Aggrdecl**)(
_tmpEFB.KnownAggr).val;_tmpEFD=_tmpEFA.targs;}}_LL735: if((*_tmpEFC)->impl == 0)
return 0;else{struct Cyc_List_List*_tmpF0C=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmpEFC)->tvs,_tmpEFD);struct Cyc_List_List*_tmpF0D=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpEFC)->impl))->fields;void*t;
while(_tmpF0D != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpF0C,((struct Cyc_Absyn_Aggrfield*)
_tmpF0D->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpF0D=_tmpF0D->tl;}return 0;}_LL736: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpEFE=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpEF6;if(_tmpEFE->tag != 12)goto
_LL738;else{_tmpEFF=_tmpEFE->f2;}}_LL737: while(_tmpEFF != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,((struct Cyc_Absyn_Aggrfield*)_tmpEFF->hd)->type))return 1;_tmpEFF=_tmpEFF->tl;}
return 0;_LL738: {struct Cyc_Absyn_AggrType_struct*_tmpF00=(struct Cyc_Absyn_AggrType_struct*)
_tmpEF6;if(_tmpF00->tag != 11)goto _LL73A;else{_tmpF01=_tmpF00->f1;_tmpF02=_tmpF01.aggr_info;
if((_tmpF02.UnknownAggr).tag != 1)goto _LL73A;_tmpF03=(struct _tuple4)(_tmpF02.UnknownAggr).val;}}
_LL739: {const char*_tmp160C;void*_tmp160B;(_tmp160B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp160C="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp160C,sizeof(char),36))),_tag_dyneither(_tmp160B,sizeof(void*),
0)));}_LL73A: {struct Cyc_Absyn_DatatypeType_struct*_tmpF04=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpEF6;if(_tmpF04->tag != 3)goto _LL73C;else{_tmpF05=_tmpF04->f1;_tmpF06=_tmpF05.datatype_info;
_tmpF07=_tmpF05.targs;}}_LL73B: {union Cyc_Absyn_DatatypeInfoU _tmpF10=_tmpF06;
struct Cyc_Absyn_UnknownDatatypeInfo _tmpF11;struct _tuple2*_tmpF12;int _tmpF13;
struct Cyc_Absyn_Datatypedecl**_tmpF14;struct Cyc_Absyn_Datatypedecl*_tmpF15;
struct Cyc_Absyn_Datatypedecl _tmpF16;struct Cyc_List_List*_tmpF17;struct Cyc_Core_Opt*
_tmpF18;_LL741: if((_tmpF10.UnknownDatatype).tag != 1)goto _LL743;_tmpF11=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpF10.UnknownDatatype).val;_tmpF12=_tmpF11.name;
_tmpF13=_tmpF11.is_extensible;_LL742: {const char*_tmp160F;void*_tmp160E;(
_tmp160E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp160F="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp160F,
sizeof(char),40))),_tag_dyneither(_tmp160E,sizeof(void*),0)));}_LL743: if((
_tmpF10.KnownDatatype).tag != 2)goto _LL740;_tmpF14=(struct Cyc_Absyn_Datatypedecl**)(
_tmpF10.KnownDatatype).val;_tmpF15=*_tmpF14;_tmpF16=*_tmpF15;_tmpF17=_tmpF16.tvs;
_tmpF18=_tmpF16.fields;_LL744: return 0;_LL740:;}_LL73C: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpF08=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpEF6;if(_tmpF08->tag != 4)
goto _LL73E;else{_tmpF09=_tmpF08->f1;_tmpF0A=_tmpF09.field_info;_tmpF0B=_tmpF09.targs;}}
_LL73D: {union Cyc_Absyn_DatatypeFieldInfoU _tmpF1B=_tmpF0A;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpF1C;struct _tuple3 _tmpF1D;struct Cyc_Absyn_Datatypedecl*_tmpF1E;struct Cyc_Absyn_Datatypefield*
_tmpF1F;_LL746: if((_tmpF1B.UnknownDatatypefield).tag != 1)goto _LL748;_tmpF1C=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpF1B.UnknownDatatypefield).val;
_LL747: {const char*_tmp1612;void*_tmp1611;(_tmp1611=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1612="got unknown datatype field in is_noalias_aggr",
_tag_dyneither(_tmp1612,sizeof(char),46))),_tag_dyneither(_tmp1611,sizeof(void*),
0)));}_LL748: if((_tmpF1B.KnownDatatypefield).tag != 2)goto _LL745;_tmpF1D=(struct
_tuple3)(_tmpF1B.KnownDatatypefield).val;_tmpF1E=_tmpF1D.f1;_tmpF1F=_tmpF1D.f2;
_LL749: {struct Cyc_List_List*_tmpF22=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpF1E->tvs,_tmpF0B);struct Cyc_List_List*_tmpF23=
_tmpF1F->typs;while(_tmpF23 != 0){_tmpEF5=Cyc_Tcutil_rsubstitute(rgn,_tmpF22,(*((
struct _tuple12*)_tmpF23->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,
_tmpEF5))return 1;_tmpF23=_tmpF23->tl;}return 0;}_LL745:;}_LL73E:;_LL73F: return 0;
_LL731:;};}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,
struct _dyneither_ptr*f){void*_tmpF24=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpF26;union Cyc_Absyn_AggrInfoU _tmpF27;struct Cyc_Absyn_Aggrdecl**_tmpF28;struct
Cyc_Absyn_Aggrdecl*_tmpF29;struct Cyc_List_List*_tmpF2A;struct Cyc_List_List*
_tmpF2C;_LL74B: {struct Cyc_Absyn_AggrType_struct*_tmpF25=(struct Cyc_Absyn_AggrType_struct*)
_tmpF24;if(_tmpF25->tag != 11)goto _LL74D;else{_tmpF26=_tmpF25->f1;_tmpF27=_tmpF26.aggr_info;
if((_tmpF27.KnownAggr).tag != 2)goto _LL74D;_tmpF28=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF27.KnownAggr).val;_tmpF29=*_tmpF28;_tmpF2A=_tmpF26.targs;}}_LL74C: _tmpF2C=
_tmpF29->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF29->impl))->fields;
goto _LL74E;_LL74D: {struct Cyc_Absyn_AnonAggrType_struct*_tmpF2B=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpF24;if(_tmpF2B->tag != 12)goto _LL74F;else{_tmpF2C=_tmpF2B->f2;}}_LL74E: {
struct Cyc_Absyn_Aggrfield*_tmpF2D=Cyc_Absyn_lookup_field(_tmpF2C,f);{struct Cyc_Absyn_Aggrfield*
_tmpF2E=_tmpF2D;struct Cyc_Absyn_Aggrfield _tmpF2F;void*_tmpF30;_LL752: if(_tmpF2E
!= 0)goto _LL754;_LL753: {const char*_tmp1615;void*_tmp1614;(_tmp1614=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1615="is_noalias_field: missing field",
_tag_dyneither(_tmp1615,sizeof(char),32))),_tag_dyneither(_tmp1614,sizeof(void*),
0)));}_LL754: if(_tmpF2E == 0)goto _LL751;_tmpF2F=*_tmpF2E;_tmpF30=_tmpF2F.type;
_LL755: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpF30);_LL751:;}return 0;}
_LL74F:;_LL750: {const char*_tmp1619;void*_tmp1618[1];struct Cyc_String_pa_struct
_tmp1617;(_tmp1617.tag=0,((_tmp1617.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1618[0]=& _tmp1617,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1619="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp1619,sizeof(char),36))),_tag_dyneither(_tmp1618,sizeof(void*),
1)))))));}_LL74A:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e){void*_tmpF36=e->r;void*_tmpF38;struct Cyc_Absyn_Exp*
_tmpF3B;struct Cyc_Absyn_Exp*_tmpF3D;struct Cyc_Absyn_Exp*_tmpF3F;struct
_dyneither_ptr*_tmpF40;struct Cyc_Absyn_Exp*_tmpF42;struct Cyc_Absyn_Exp*_tmpF43;
struct Cyc_Absyn_Exp*_tmpF45;struct Cyc_Absyn_Exp*_tmpF46;struct Cyc_Absyn_Exp*
_tmpF48;struct Cyc_Absyn_Exp*_tmpF4A;struct Cyc_Absyn_Stmt*_tmpF4C;_LL757: {struct
Cyc_Absyn_Var_e_struct*_tmpF37=(struct Cyc_Absyn_Var_e_struct*)_tmpF36;if(_tmpF37->tag
!= 1)goto _LL759;else{_tmpF38=(void*)_tmpF37->f2;{struct Cyc_Absyn_Global_b_struct*
_tmpF39=(struct Cyc_Absyn_Global_b_struct*)_tmpF38;if(_tmpF39->tag != 1)goto _LL759;};}}
_LL758: return 0;_LL759: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpF3A=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpF36;if(_tmpF3A->tag != 23)goto _LL75B;else{_tmpF3B=_tmpF3A->f1;}}_LL75A:
_tmpF3D=_tmpF3B;goto _LL75C;_LL75B: {struct Cyc_Absyn_Deref_e_struct*_tmpF3C=(
struct Cyc_Absyn_Deref_e_struct*)_tmpF36;if(_tmpF3C->tag != 21)goto _LL75D;else{
_tmpF3D=_tmpF3C->f1;}}_LL75C: return Cyc_Tcutil_is_noalias_pointer_aux((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpF3D->topt))->v,1) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpF3D);_LL75D: {struct Cyc_Absyn_AggrMember_e_struct*_tmpF3E=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmpF36;if(_tmpF3E->tag != 22)goto _LL75F;else{_tmpF3F=_tmpF3E->f1;_tmpF40=_tmpF3E->f2;}}
_LL75E: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF3F);_LL75F: {struct Cyc_Absyn_Subscript_e_struct*
_tmpF41=(struct Cyc_Absyn_Subscript_e_struct*)_tmpF36;if(_tmpF41->tag != 24)goto
_LL761;else{_tmpF42=_tmpF41->f1;_tmpF43=_tmpF41->f2;}}_LL760: {void*_tmpF4D=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF42->topt))->v);_LL76C: {struct Cyc_Absyn_TupleType_struct*
_tmpF4E=(struct Cyc_Absyn_TupleType_struct*)_tmpF4D;if(_tmpF4E->tag != 10)goto
_LL76E;}_LL76D: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF42);_LL76E:;_LL76F:
return 0;_LL76B:;}_LL761: {struct Cyc_Absyn_Conditional_e_struct*_tmpF44=(struct
Cyc_Absyn_Conditional_e_struct*)_tmpF36;if(_tmpF44->tag != 6)goto _LL763;else{
_tmpF45=_tmpF44->f2;_tmpF46=_tmpF44->f3;}}_LL762: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpF45) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpF46);_LL763: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpF47=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpF36;if(_tmpF47->tag != 9)goto _LL765;
else{_tmpF48=_tmpF47->f2;}}_LL764: _tmpF4A=_tmpF48;goto _LL766;_LL765: {struct Cyc_Absyn_Cast_e_struct*
_tmpF49=(struct Cyc_Absyn_Cast_e_struct*)_tmpF36;if(_tmpF49->tag != 15)goto _LL767;
else{_tmpF4A=_tmpF49->f2;}}_LL766: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF4A);
_LL767: {struct Cyc_Absyn_StmtExp_e_struct*_tmpF4B=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpF36;if(_tmpF4B->tag != 37)goto _LL769;else{_tmpF4C=_tmpF4B->f1;}}_LL768: while(
1){void*_tmpF4F=_tmpF4C->r;struct Cyc_Absyn_Stmt*_tmpF51;struct Cyc_Absyn_Stmt*
_tmpF52;struct Cyc_Absyn_Decl*_tmpF54;struct Cyc_Absyn_Stmt*_tmpF55;struct Cyc_Absyn_Exp*
_tmpF57;_LL771: {struct Cyc_Absyn_Seq_s_struct*_tmpF50=(struct Cyc_Absyn_Seq_s_struct*)
_tmpF4F;if(_tmpF50->tag != 2)goto _LL773;else{_tmpF51=_tmpF50->f1;_tmpF52=_tmpF50->f2;}}
_LL772: _tmpF4C=_tmpF52;goto _LL770;_LL773: {struct Cyc_Absyn_Decl_s_struct*_tmpF53=(
struct Cyc_Absyn_Decl_s_struct*)_tmpF4F;if(_tmpF53->tag != 12)goto _LL775;else{
_tmpF54=_tmpF53->f1;_tmpF55=_tmpF53->f2;}}_LL774: _tmpF4C=_tmpF55;goto _LL770;
_LL775: {struct Cyc_Absyn_Exp_s_struct*_tmpF56=(struct Cyc_Absyn_Exp_s_struct*)
_tmpF4F;if(_tmpF56->tag != 1)goto _LL777;else{_tmpF57=_tmpF56->f1;}}_LL776: return
Cyc_Tcutil_is_noalias_path_aux(r,_tmpF57);_LL777:;_LL778: {const char*_tmp161C;
void*_tmp161B;(_tmp161B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp161C="is_noalias_stmt_exp: ill-formed StmtExp",
_tag_dyneither(_tmp161C,sizeof(char),40))),_tag_dyneither(_tmp161B,sizeof(void*),
0)));}_LL770:;}_LL769:;_LL76A: return 1;_LL756:;}int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(
r,e);}struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){struct _tuple18 _tmp161D;struct _tuple18 bogus_ans=(_tmp161D.f1=0,((_tmp161D.f2=(
void*)& Cyc_Absyn_HeapRgn_val,_tmp161D)));void*_tmpF5A=e->r;struct _tuple2*_tmpF5C;
void*_tmpF5D;struct Cyc_Absyn_Exp*_tmpF5F;struct _dyneither_ptr*_tmpF60;int _tmpF61;
struct Cyc_Absyn_Exp*_tmpF63;struct _dyneither_ptr*_tmpF64;int _tmpF65;struct Cyc_Absyn_Exp*
_tmpF67;struct Cyc_Absyn_Exp*_tmpF69;struct Cyc_Absyn_Exp*_tmpF6A;_LL77A: {struct
Cyc_Absyn_Var_e_struct*_tmpF5B=(struct Cyc_Absyn_Var_e_struct*)_tmpF5A;if(_tmpF5B->tag
!= 1)goto _LL77C;else{_tmpF5C=_tmpF5B->f1;_tmpF5D=(void*)_tmpF5B->f2;}}_LL77B: {
void*_tmpF6B=_tmpF5D;struct Cyc_Absyn_Vardecl*_tmpF6F;struct Cyc_Absyn_Vardecl*
_tmpF71;struct Cyc_Absyn_Vardecl*_tmpF73;struct Cyc_Absyn_Vardecl*_tmpF75;_LL787: {
struct Cyc_Absyn_Unresolved_b_struct*_tmpF6C=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmpF6B;if(_tmpF6C->tag != 0)goto _LL789;}_LL788: goto _LL78A;_LL789: {struct Cyc_Absyn_Funname_b_struct*
_tmpF6D=(struct Cyc_Absyn_Funname_b_struct*)_tmpF6B;if(_tmpF6D->tag != 2)goto
_LL78B;}_LL78A: return bogus_ans;_LL78B: {struct Cyc_Absyn_Global_b_struct*_tmpF6E=(
struct Cyc_Absyn_Global_b_struct*)_tmpF6B;if(_tmpF6E->tag != 1)goto _LL78D;else{
_tmpF6F=_tmpF6E->f1;}}_LL78C: {void*_tmpF76=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);_LL794: {struct Cyc_Absyn_ArrayType_struct*
_tmpF77=(struct Cyc_Absyn_ArrayType_struct*)_tmpF76;if(_tmpF77->tag != 8)goto
_LL796;}_LL795: {struct _tuple18 _tmp161E;return(_tmp161E.f1=1,((_tmp161E.f2=(void*)&
Cyc_Absyn_HeapRgn_val,_tmp161E)));}_LL796:;_LL797: {struct _tuple18 _tmp161F;
return(_tmp161F.f1=(_tmpF6F->tq).real_const,((_tmp161F.f2=(void*)& Cyc_Absyn_HeapRgn_val,
_tmp161F)));}_LL793:;}_LL78D: {struct Cyc_Absyn_Local_b_struct*_tmpF70=(struct Cyc_Absyn_Local_b_struct*)
_tmpF6B;if(_tmpF70->tag != 4)goto _LL78F;else{_tmpF71=_tmpF70->f1;}}_LL78E: {void*
_tmpF7A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL799: {struct Cyc_Absyn_ArrayType_struct*_tmpF7B=(struct Cyc_Absyn_ArrayType_struct*)
_tmpF7A;if(_tmpF7B->tag != 8)goto _LL79B;}_LL79A: {struct _tuple18 _tmp1620;return(
_tmp1620.f1=1,((_tmp1620.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpF71->rgn))->v,
_tmp1620)));}_LL79B:;_LL79C: _tmpF71->escapes=1;{struct _tuple18 _tmp1621;return(
_tmp1621.f1=(_tmpF71->tq).real_const,((_tmp1621.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF71->rgn))->v,_tmp1621)));};_LL798:;}_LL78F: {struct Cyc_Absyn_Pat_b_struct*
_tmpF72=(struct Cyc_Absyn_Pat_b_struct*)_tmpF6B;if(_tmpF72->tag != 5)goto _LL791;
else{_tmpF73=_tmpF72->f1;}}_LL790: _tmpF75=_tmpF73;goto _LL792;_LL791: {struct Cyc_Absyn_Param_b_struct*
_tmpF74=(struct Cyc_Absyn_Param_b_struct*)_tmpF6B;if(_tmpF74->tag != 3)goto _LL786;
else{_tmpF75=_tmpF74->f1;}}_LL792: _tmpF75->escapes=1;{struct _tuple18 _tmp1622;
return(_tmp1622.f1=(_tmpF75->tq).real_const,((_tmp1622.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF75->rgn))->v,_tmp1622)));};_LL786:;}_LL77C: {struct Cyc_Absyn_AggrMember_e_struct*
_tmpF5E=(struct Cyc_Absyn_AggrMember_e_struct*)_tmpF5A;if(_tmpF5E->tag != 22)goto
_LL77E;else{_tmpF5F=_tmpF5E->f1;_tmpF60=_tmpF5E->f2;_tmpF61=_tmpF5E->f3;}}_LL77D:
if(_tmpF61)return bogus_ans;{void*_tmpF7F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF5F->topt))->v);struct Cyc_List_List*_tmpF81;struct Cyc_Absyn_AggrInfo
_tmpF83;union Cyc_Absyn_AggrInfoU _tmpF84;struct Cyc_Absyn_Aggrdecl**_tmpF85;struct
Cyc_Absyn_Aggrdecl*_tmpF86;_LL79E: {struct Cyc_Absyn_AnonAggrType_struct*_tmpF80=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmpF7F;if(_tmpF80->tag != 12)goto _LL7A0;
else{_tmpF81=_tmpF80->f2;}}_LL79F: {struct Cyc_Absyn_Aggrfield*_tmpF87=Cyc_Absyn_lookup_field(
_tmpF81,_tmpF60);if(_tmpF87 != 0  && _tmpF87->width == 0){struct _tuple18 _tmp1623;
return(_tmp1623.f1=(_tmpF87->tq).real_const,((_tmp1623.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF5F)).f2,_tmp1623)));}return bogus_ans;}_LL7A0: {struct Cyc_Absyn_AggrType_struct*
_tmpF82=(struct Cyc_Absyn_AggrType_struct*)_tmpF7F;if(_tmpF82->tag != 11)goto
_LL7A2;else{_tmpF83=_tmpF82->f1;_tmpF84=_tmpF83.aggr_info;if((_tmpF84.KnownAggr).tag
!= 2)goto _LL7A2;_tmpF85=(struct Cyc_Absyn_Aggrdecl**)(_tmpF84.KnownAggr).val;
_tmpF86=*_tmpF85;}}_LL7A1: {struct Cyc_Absyn_Aggrfield*_tmpF89=Cyc_Absyn_lookup_decl_field(
_tmpF86,_tmpF60);if(_tmpF89 != 0  && _tmpF89->width == 0){struct _tuple18 _tmp1624;
return(_tmp1624.f1=(_tmpF89->tq).real_const,((_tmp1624.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF5F)).f2,_tmp1624)));}return bogus_ans;}_LL7A2:;_LL7A3: return bogus_ans;
_LL79D:;};_LL77E: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpF62=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpF5A;if(_tmpF62->tag != 23)goto _LL780;else{_tmpF63=_tmpF62->f1;_tmpF64=_tmpF62->f2;
_tmpF65=_tmpF62->f3;}}_LL77F: if(_tmpF65)return bogus_ans;{void*_tmpF8B=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF63->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpF8D;void*_tmpF8E;struct Cyc_Absyn_PtrAtts _tmpF8F;void*_tmpF90;_LL7A5: {struct
Cyc_Absyn_PointerType_struct*_tmpF8C=(struct Cyc_Absyn_PointerType_struct*)
_tmpF8B;if(_tmpF8C->tag != 5)goto _LL7A7;else{_tmpF8D=_tmpF8C->f1;_tmpF8E=_tmpF8D.elt_typ;
_tmpF8F=_tmpF8D.ptr_atts;_tmpF90=_tmpF8F.rgn;}}_LL7A6: {struct Cyc_Absyn_Aggrfield*
finfo;{void*_tmpF91=Cyc_Tcutil_compress(_tmpF8E);struct Cyc_List_List*_tmpF93;
struct Cyc_Absyn_AggrInfo _tmpF95;union Cyc_Absyn_AggrInfoU _tmpF96;struct Cyc_Absyn_Aggrdecl**
_tmpF97;struct Cyc_Absyn_Aggrdecl*_tmpF98;_LL7AA: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpF92=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpF91;if(_tmpF92->tag != 12)goto
_LL7AC;else{_tmpF93=_tmpF92->f2;}}_LL7AB: finfo=Cyc_Absyn_lookup_field(_tmpF93,
_tmpF64);goto _LL7A9;_LL7AC: {struct Cyc_Absyn_AggrType_struct*_tmpF94=(struct Cyc_Absyn_AggrType_struct*)
_tmpF91;if(_tmpF94->tag != 11)goto _LL7AE;else{_tmpF95=_tmpF94->f1;_tmpF96=_tmpF95.aggr_info;
if((_tmpF96.KnownAggr).tag != 2)goto _LL7AE;_tmpF97=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF96.KnownAggr).val;_tmpF98=*_tmpF97;}}_LL7AD: finfo=Cyc_Absyn_lookup_decl_field(
_tmpF98,_tmpF64);goto _LL7A9;_LL7AE:;_LL7AF: return bogus_ans;_LL7A9:;}if(finfo != 0
 && finfo->width == 0){struct _tuple18 _tmp1625;return(_tmp1625.f1=(finfo->tq).real_const,((
_tmp1625.f2=_tmpF90,_tmp1625)));}return bogus_ans;}_LL7A7:;_LL7A8: return bogus_ans;
_LL7A4:;};_LL780: {struct Cyc_Absyn_Deref_e_struct*_tmpF66=(struct Cyc_Absyn_Deref_e_struct*)
_tmpF5A;if(_tmpF66->tag != 21)goto _LL782;else{_tmpF67=_tmpF66->f1;}}_LL781: {void*
_tmpF9A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpF67->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpF9C;struct Cyc_Absyn_Tqual _tmpF9D;struct Cyc_Absyn_PtrAtts
_tmpF9E;void*_tmpF9F;_LL7B1: {struct Cyc_Absyn_PointerType_struct*_tmpF9B=(struct
Cyc_Absyn_PointerType_struct*)_tmpF9A;if(_tmpF9B->tag != 5)goto _LL7B3;else{
_tmpF9C=_tmpF9B->f1;_tmpF9D=_tmpF9C.elt_tq;_tmpF9E=_tmpF9C.ptr_atts;_tmpF9F=
_tmpF9E.rgn;}}_LL7B2: {struct _tuple18 _tmp1626;return(_tmp1626.f1=_tmpF9D.real_const,((
_tmp1626.f2=_tmpF9F,_tmp1626)));}_LL7B3:;_LL7B4: return bogus_ans;_LL7B0:;}_LL782: {
struct Cyc_Absyn_Subscript_e_struct*_tmpF68=(struct Cyc_Absyn_Subscript_e_struct*)
_tmpF5A;if(_tmpF68->tag != 24)goto _LL784;else{_tmpF69=_tmpF68->f1;_tmpF6A=_tmpF68->f2;}}
_LL783: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpF69->topt))->v);void*_tmpFA1=t;struct Cyc_List_List*_tmpFA3;struct Cyc_Absyn_PtrInfo
_tmpFA5;struct Cyc_Absyn_Tqual _tmpFA6;struct Cyc_Absyn_PtrAtts _tmpFA7;void*_tmpFA8;
struct Cyc_Absyn_ArrayInfo _tmpFAA;struct Cyc_Absyn_Tqual _tmpFAB;_LL7B6: {struct Cyc_Absyn_TupleType_struct*
_tmpFA2=(struct Cyc_Absyn_TupleType_struct*)_tmpFA1;if(_tmpFA2->tag != 10)goto
_LL7B8;else{_tmpFA3=_tmpFA2->f1;}}_LL7B7: {unsigned int _tmpFAD;int _tmpFAE;struct
_tuple14 _tmpFAC=Cyc_Evexp_eval_const_uint_exp(_tmpF6A);_tmpFAD=_tmpFAC.f1;
_tmpFAE=_tmpFAC.f2;if(!_tmpFAE)return bogus_ans;{struct _tuple12*_tmpFAF=Cyc_Absyn_lookup_tuple_field(
_tmpFA3,(int)_tmpFAD);if(_tmpFAF != 0){struct _tuple18 _tmp1627;return(_tmp1627.f1=((*
_tmpFAF).f1).real_const,((_tmp1627.f2=(Cyc_Tcutil_addressof_props(te,_tmpF69)).f2,
_tmp1627)));}return bogus_ans;};}_LL7B8: {struct Cyc_Absyn_PointerType_struct*
_tmpFA4=(struct Cyc_Absyn_PointerType_struct*)_tmpFA1;if(_tmpFA4->tag != 5)goto
_LL7BA;else{_tmpFA5=_tmpFA4->f1;_tmpFA6=_tmpFA5.elt_tq;_tmpFA7=_tmpFA5.ptr_atts;
_tmpFA8=_tmpFA7.rgn;}}_LL7B9: {struct _tuple18 _tmp1628;return(_tmp1628.f1=_tmpFA6.real_const,((
_tmp1628.f2=_tmpFA8,_tmp1628)));}_LL7BA: {struct Cyc_Absyn_ArrayType_struct*
_tmpFA9=(struct Cyc_Absyn_ArrayType_struct*)_tmpFA1;if(_tmpFA9->tag != 8)goto
_LL7BC;else{_tmpFAA=_tmpFA9->f1;_tmpFAB=_tmpFAA.tq;}}_LL7BB: {struct _tuple18
_tmp1629;return(_tmp1629.f1=_tmpFAB.real_const,((_tmp1629.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF69)).f2,_tmp1629)));}_LL7BC:;_LL7BD: return bogus_ans;_LL7B5:;}_LL784:;
_LL785:{const char*_tmp162C;void*_tmp162B;(_tmp162B=0,Cyc_Tcutil_terr(e->loc,((
_tmp162C="unary & applied to non-lvalue",_tag_dyneither(_tmp162C,sizeof(char),30))),
_tag_dyneither(_tmp162B,sizeof(void*),0)));}return bogus_ans;_LL779:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpFB6=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpFB8;void*_tmpFB9;struct Cyc_Absyn_Tqual
_tmpFBA;union Cyc_Absyn_Constraint*_tmpFBB;_LL7BF: {struct Cyc_Absyn_ArrayType_struct*
_tmpFB7=(struct Cyc_Absyn_ArrayType_struct*)_tmpFB6;if(_tmpFB7->tag != 8)goto
_LL7C1;else{_tmpFB8=_tmpFB7->f1;_tmpFB9=_tmpFB8.elt_type;_tmpFBA=_tmpFB8.tq;
_tmpFBB=_tmpFB8.zero_term;}}_LL7C0: {void*_tmpFBD;struct _tuple18 _tmpFBC=Cyc_Tcutil_addressof_props(
te,e);_tmpFBD=_tmpFBC.f2;{struct Cyc_Absyn_Upper_b_struct _tmp162F;struct Cyc_Absyn_Upper_b_struct*
_tmp162E;return Cyc_Absyn_atb_typ(_tmpFB9,_tmpFBD,_tmpFBA,(void*)((_tmp162E=
_cycalloc(sizeof(*_tmp162E)),((_tmp162E[0]=((_tmp162F.tag=1,((_tmp162F.f1=e,
_tmp162F)))),_tmp162E)))),_tmpFBB);};}_LL7C1:;_LL7C2: return e_typ;_LL7BE:;}void
Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,union Cyc_Absyn_Constraint*
b){b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
b);{void*_tmpFC0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpFC3;_LL7C4: {struct Cyc_Absyn_DynEither_b_struct*
_tmpFC1=(struct Cyc_Absyn_DynEither_b_struct*)_tmpFC0;if(_tmpFC1->tag != 0)goto
_LL7C6;}_LL7C5: return;_LL7C6: {struct Cyc_Absyn_Upper_b_struct*_tmpFC2=(struct Cyc_Absyn_Upper_b_struct*)
_tmpFC0;if(_tmpFC2->tag != 1)goto _LL7C3;else{_tmpFC3=_tmpFC2->f1;}}_LL7C7: {
unsigned int _tmpFC5;int _tmpFC6;struct _tuple14 _tmpFC4=Cyc_Evexp_eval_const_uint_exp(
_tmpFC3);_tmpFC5=_tmpFC4.f1;_tmpFC6=_tmpFC4.f2;if(_tmpFC6  && _tmpFC5 <= i){const
char*_tmp1634;void*_tmp1633[2];struct Cyc_Int_pa_struct _tmp1632;struct Cyc_Int_pa_struct
_tmp1631;(_tmp1631.tag=1,((_tmp1631.f1=(unsigned long)((int)i),((_tmp1632.tag=1,((
_tmp1632.f1=(unsigned long)((int)_tmpFC5),((_tmp1633[0]=& _tmp1632,((_tmp1633[1]=&
_tmp1631,Cyc_Tcutil_terr(loc,((_tmp1634="dereference is out of bounds: %d <= %d",
_tag_dyneither(_tmp1634,sizeof(char),39))),_tag_dyneither(_tmp1633,sizeof(void*),
2)))))))))))));}return;}_LL7C3:;};}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,union Cyc_Absyn_Constraint*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b){void*_tmpFCB=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*
_tmpFCD;_LL7C9: {struct Cyc_Absyn_Upper_b_struct*_tmpFCC=(struct Cyc_Absyn_Upper_b_struct*)
_tmpFCB;if(_tmpFCC->tag != 1)goto _LL7CB;else{_tmpFCD=_tmpFCC->f1;}}_LL7CA: {
unsigned int _tmpFCF;int _tmpFD0;struct _tuple14 _tmpFCE=Cyc_Evexp_eval_const_uint_exp(
_tmpFCD);_tmpFCF=_tmpFCE.f1;_tmpFD0=_tmpFCE.f2;return _tmpFD0  && _tmpFCF == 1;}
_LL7CB:;_LL7CC: return 0;_LL7C8:;}int Cyc_Tcutil_bits_only(void*t){void*_tmpFD1=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmpFD8;void*_tmpFD9;union Cyc_Absyn_Constraint*
_tmpFDA;struct Cyc_List_List*_tmpFDC;struct Cyc_Absyn_AggrInfo _tmpFDE;union Cyc_Absyn_AggrInfoU
_tmpFDF;struct _tuple4 _tmpFE0;struct Cyc_Absyn_AggrInfo _tmpFE2;union Cyc_Absyn_AggrInfoU
_tmpFE3;struct Cyc_Absyn_Aggrdecl**_tmpFE4;struct Cyc_Absyn_Aggrdecl*_tmpFE5;
struct Cyc_List_List*_tmpFE6;struct Cyc_List_List*_tmpFE8;_LL7CE: {struct Cyc_Absyn_VoidType_struct*
_tmpFD2=(struct Cyc_Absyn_VoidType_struct*)_tmpFD1;if(_tmpFD2->tag != 0)goto _LL7D0;}
_LL7CF: goto _LL7D1;_LL7D0: {struct Cyc_Absyn_IntType_struct*_tmpFD3=(struct Cyc_Absyn_IntType_struct*)
_tmpFD1;if(_tmpFD3->tag != 6)goto _LL7D2;}_LL7D1: goto _LL7D3;_LL7D2: {struct Cyc_Absyn_FloatType_struct*
_tmpFD4=(struct Cyc_Absyn_FloatType_struct*)_tmpFD1;if(_tmpFD4->tag != 7)goto
_LL7D4;}_LL7D3: return 1;_LL7D4: {struct Cyc_Absyn_EnumType_struct*_tmpFD5=(struct
Cyc_Absyn_EnumType_struct*)_tmpFD1;if(_tmpFD5->tag != 13)goto _LL7D6;}_LL7D5: goto
_LL7D7;_LL7D6: {struct Cyc_Absyn_AnonEnumType_struct*_tmpFD6=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmpFD1;if(_tmpFD6->tag != 14)goto _LL7D8;}_LL7D7: return 0;_LL7D8: {struct Cyc_Absyn_ArrayType_struct*
_tmpFD7=(struct Cyc_Absyn_ArrayType_struct*)_tmpFD1;if(_tmpFD7->tag != 8)goto
_LL7DA;else{_tmpFD8=_tmpFD7->f1;_tmpFD9=_tmpFD8.elt_type;_tmpFDA=_tmpFD8.zero_term;}}
_LL7D9: return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpFDA) && Cyc_Tcutil_bits_only(_tmpFD9);_LL7DA: {struct Cyc_Absyn_TupleType_struct*
_tmpFDB=(struct Cyc_Absyn_TupleType_struct*)_tmpFD1;if(_tmpFDB->tag != 10)goto
_LL7DC;else{_tmpFDC=_tmpFDB->f1;}}_LL7DB: for(0;_tmpFDC != 0;_tmpFDC=_tmpFDC->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpFDC->hd)).f2))return 0;}return 1;
_LL7DC: {struct Cyc_Absyn_AggrType_struct*_tmpFDD=(struct Cyc_Absyn_AggrType_struct*)
_tmpFD1;if(_tmpFDD->tag != 11)goto _LL7DE;else{_tmpFDE=_tmpFDD->f1;_tmpFDF=_tmpFDE.aggr_info;
if((_tmpFDF.UnknownAggr).tag != 1)goto _LL7DE;_tmpFE0=(struct _tuple4)(_tmpFDF.UnknownAggr).val;}}
_LL7DD: return 0;_LL7DE: {struct Cyc_Absyn_AggrType_struct*_tmpFE1=(struct Cyc_Absyn_AggrType_struct*)
_tmpFD1;if(_tmpFE1->tag != 11)goto _LL7E0;else{_tmpFE2=_tmpFE1->f1;_tmpFE3=_tmpFE2.aggr_info;
if((_tmpFE3.KnownAggr).tag != 2)goto _LL7E0;_tmpFE4=(struct Cyc_Absyn_Aggrdecl**)(
_tmpFE3.KnownAggr).val;_tmpFE5=*_tmpFE4;_tmpFE6=_tmpFE2.targs;}}_LL7DF: if(
_tmpFE5->impl == 0)return 0;{struct _RegionHandle _tmpFE9=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpFE9;_push_region(rgn);{struct Cyc_List_List*_tmpFEA=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpFE5->tvs,_tmpFE6);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFE5->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpFEA,((
struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpFEB=0;_npop_handler(0);return
_tmpFEB;}}}{int _tmpFEC=1;_npop_handler(0);return _tmpFEC;};};_pop_region(rgn);};
_LL7E0: {struct Cyc_Absyn_AnonAggrType_struct*_tmpFE7=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpFD1;if(_tmpFE7->tag != 12)goto _LL7E2;else{_tmpFE8=_tmpFE7->f2;}}_LL7E1: for(0;
_tmpFE8 != 0;_tmpFE8=_tmpFE8->tl){if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)
_tmpFE8->hd)->type))return 0;}return 1;_LL7E2:;_LL7E3: return 0;_LL7CD:;}static int
Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpFED=e->r;struct _tuple2*_tmpFF5;void*_tmpFF6;struct Cyc_Absyn_Exp*_tmpFF8;
struct Cyc_Absyn_Exp*_tmpFF9;struct Cyc_Absyn_Exp*_tmpFFA;struct Cyc_Absyn_Exp*
_tmpFFC;struct Cyc_Absyn_Exp*_tmpFFD;struct Cyc_Absyn_Exp*_tmpFFF;struct Cyc_Absyn_Exp*
_tmp1001;void*_tmp1003;struct Cyc_Absyn_Exp*_tmp1004;enum Cyc_Absyn_Coercion
_tmp1005;void*_tmp1007;struct Cyc_Absyn_Exp*_tmp1008;struct Cyc_Absyn_Exp*_tmp100A;
struct Cyc_Absyn_Exp*_tmp100C;struct Cyc_Absyn_Exp*_tmp100D;struct Cyc_List_List*
_tmp100F;struct Cyc_List_List*_tmp1011;struct Cyc_List_List*_tmp1013;enum Cyc_Absyn_Primop
_tmp1015;struct Cyc_List_List*_tmp1016;struct Cyc_List_List*_tmp1018;struct Cyc_List_List*
_tmp101A;_LL7E5: {struct Cyc_Absyn_Const_e_struct*_tmpFEE=(struct Cyc_Absyn_Const_e_struct*)
_tmpFED;if(_tmpFEE->tag != 0)goto _LL7E7;}_LL7E6: goto _LL7E8;_LL7E7: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpFEF=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpFED;if(_tmpFEF->tag != 18)goto
_LL7E9;}_LL7E8: goto _LL7EA;_LL7E9: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpFF0=(
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpFED;if(_tmpFF0->tag != 19)goto _LL7EB;}
_LL7EA: goto _LL7EC;_LL7EB: {struct Cyc_Absyn_Offsetof_e_struct*_tmpFF1=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpFED;if(_tmpFF1->tag != 20)goto _LL7ED;}_LL7EC: goto _LL7EE;_LL7ED: {struct Cyc_Absyn_Enum_e_struct*
_tmpFF2=(struct Cyc_Absyn_Enum_e_struct*)_tmpFED;if(_tmpFF2->tag != 32)goto _LL7EF;}
_LL7EE: goto _LL7F0;_LL7EF: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpFF3=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpFED;if(_tmpFF3->tag != 33)goto _LL7F1;}_LL7F0: return 1;_LL7F1: {struct Cyc_Absyn_Var_e_struct*
_tmpFF4=(struct Cyc_Absyn_Var_e_struct*)_tmpFED;if(_tmpFF4->tag != 1)goto _LL7F3;
else{_tmpFF5=_tmpFF4->f1;_tmpFF6=(void*)_tmpFF4->f2;}}_LL7F2: {void*_tmp101B=
_tmpFF6;struct Cyc_Absyn_Vardecl*_tmp101E;_LL812: {struct Cyc_Absyn_Funname_b_struct*
_tmp101C=(struct Cyc_Absyn_Funname_b_struct*)_tmp101B;if(_tmp101C->tag != 2)goto
_LL814;}_LL813: return 1;_LL814: {struct Cyc_Absyn_Global_b_struct*_tmp101D=(struct
Cyc_Absyn_Global_b_struct*)_tmp101B;if(_tmp101D->tag != 1)goto _LL816;else{
_tmp101E=_tmp101D->f1;}}_LL815: {void*_tmp1020=Cyc_Tcutil_compress(_tmp101E->type);
_LL81B: {struct Cyc_Absyn_ArrayType_struct*_tmp1021=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1020;if(_tmp1021->tag != 8)goto _LL81D;}_LL81C: goto _LL81E;_LL81D: {struct Cyc_Absyn_FnType_struct*
_tmp1022=(struct Cyc_Absyn_FnType_struct*)_tmp1020;if(_tmp1022->tag != 9)goto
_LL81F;}_LL81E: return 1;_LL81F:;_LL820: return var_okay;_LL81A:;}_LL816: {struct Cyc_Absyn_Unresolved_b_struct*
_tmp101F=(struct Cyc_Absyn_Unresolved_b_struct*)_tmp101B;if(_tmp101F->tag != 0)
goto _LL818;}_LL817: return 0;_LL818:;_LL819: return var_okay;_LL811:;}_LL7F3: {
struct Cyc_Absyn_Conditional_e_struct*_tmpFF7=(struct Cyc_Absyn_Conditional_e_struct*)
_tmpFED;if(_tmpFF7->tag != 6)goto _LL7F5;else{_tmpFF8=_tmpFF7->f1;_tmpFF9=_tmpFF7->f2;
_tmpFFA=_tmpFF7->f3;}}_LL7F4: return(Cyc_Tcutil_cnst_exp(te,0,_tmpFF8) && Cyc_Tcutil_cnst_exp(
te,0,_tmpFF9)) && Cyc_Tcutil_cnst_exp(te,0,_tmpFFA);_LL7F5: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpFFB=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpFED;if(_tmpFFB->tag != 9)goto _LL7F7;
else{_tmpFFC=_tmpFFB->f1;_tmpFFD=_tmpFFB->f2;}}_LL7F6: return Cyc_Tcutil_cnst_exp(
te,0,_tmpFFC) && Cyc_Tcutil_cnst_exp(te,0,_tmpFFD);_LL7F7: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpFFE=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpFED;if(_tmpFFE->tag != 13)
goto _LL7F9;else{_tmpFFF=_tmpFFE->f1;}}_LL7F8: _tmp1001=_tmpFFF;goto _LL7FA;_LL7F9: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp1000=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmpFED;if(_tmp1000->tag != 14)goto _LL7FB;else{_tmp1001=_tmp1000->f1;}}_LL7FA:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp1001);_LL7FB: {struct Cyc_Absyn_Cast_e_struct*
_tmp1002=(struct Cyc_Absyn_Cast_e_struct*)_tmpFED;if(_tmp1002->tag != 15)goto
_LL7FD;else{_tmp1003=(void*)_tmp1002->f1;_tmp1004=_tmp1002->f2;_tmp1005=_tmp1002->f4;
if(_tmp1005 != Cyc_Absyn_No_coercion)goto _LL7FD;}}_LL7FC: return Cyc_Tcutil_cnst_exp(
te,var_okay,_tmp1004);_LL7FD: {struct Cyc_Absyn_Cast_e_struct*_tmp1006=(struct Cyc_Absyn_Cast_e_struct*)
_tmpFED;if(_tmp1006->tag != 15)goto _LL7FF;else{_tmp1007=(void*)_tmp1006->f1;
_tmp1008=_tmp1006->f2;}}_LL7FE: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp1008);
_LL7FF: {struct Cyc_Absyn_Address_e_struct*_tmp1009=(struct Cyc_Absyn_Address_e_struct*)
_tmpFED;if(_tmp1009->tag != 16)goto _LL801;else{_tmp100A=_tmp1009->f1;}}_LL800:
return Cyc_Tcutil_cnst_exp(te,1,_tmp100A);_LL801: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp100B=(struct Cyc_Absyn_Comprehension_e_struct*)_tmpFED;if(_tmp100B->tag != 28)
goto _LL803;else{_tmp100C=_tmp100B->f2;_tmp100D=_tmp100B->f3;}}_LL802: return Cyc_Tcutil_cnst_exp(
te,0,_tmp100C) && Cyc_Tcutil_cnst_exp(te,0,_tmp100D);_LL803: {struct Cyc_Absyn_Array_e_struct*
_tmp100E=(struct Cyc_Absyn_Array_e_struct*)_tmpFED;if(_tmp100E->tag != 27)goto
_LL805;else{_tmp100F=_tmp100E->f1;}}_LL804: _tmp1011=_tmp100F;goto _LL806;_LL805: {
struct Cyc_Absyn_AnonStruct_e_struct*_tmp1010=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmpFED;if(_tmp1010->tag != 30)goto _LL807;else{_tmp1011=_tmp1010->f2;}}_LL806:
_tmp1013=_tmp1011;goto _LL808;_LL807: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp1012=(struct Cyc_Absyn_Aggregate_e_struct*)_tmpFED;if(_tmp1012->tag != 29)goto
_LL809;else{_tmp1013=_tmp1012->f3;}}_LL808: for(0;_tmp1013 != 0;_tmp1013=_tmp1013->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple19*)_tmp1013->hd)).f2))return 0;}
return 1;_LL809: {struct Cyc_Absyn_Primop_e_struct*_tmp1014=(struct Cyc_Absyn_Primop_e_struct*)
_tmpFED;if(_tmp1014->tag != 3)goto _LL80B;else{_tmp1015=_tmp1014->f1;_tmp1016=
_tmp1014->f2;}}_LL80A: _tmp1018=_tmp1016;goto _LL80C;_LL80B: {struct Cyc_Absyn_Tuple_e_struct*
_tmp1017=(struct Cyc_Absyn_Tuple_e_struct*)_tmpFED;if(_tmp1017->tag != 25)goto
_LL80D;else{_tmp1018=_tmp1017->f1;}}_LL80C: _tmp101A=_tmp1018;goto _LL80E;_LL80D: {
struct Cyc_Absyn_Datatype_e_struct*_tmp1019=(struct Cyc_Absyn_Datatype_e_struct*)
_tmpFED;if(_tmp1019->tag != 31)goto _LL80F;else{_tmp101A=_tmp1019->f1;}}_LL80E:
for(0;_tmp101A != 0;_tmp101A=_tmp101A->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(struct
Cyc_Absyn_Exp*)_tmp101A->hd))return 0;}return 1;_LL80F:;_LL810: return 0;_LL7E4:;}
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){return
Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(struct
Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp1023=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1028;
struct Cyc_Absyn_PtrAtts _tmp1029;union Cyc_Absyn_Constraint*_tmp102A;union Cyc_Absyn_Constraint*
_tmp102B;struct Cyc_Absyn_ArrayInfo _tmp102D;void*_tmp102E;struct Cyc_List_List*
_tmp1030;struct Cyc_Absyn_AggrInfo _tmp1032;union Cyc_Absyn_AggrInfoU _tmp1033;
struct Cyc_List_List*_tmp1034;struct Cyc_List_List*_tmp1036;_LL822: {struct Cyc_Absyn_VoidType_struct*
_tmp1024=(struct Cyc_Absyn_VoidType_struct*)_tmp1023;if(_tmp1024->tag != 0)goto
_LL824;}_LL823: goto _LL825;_LL824: {struct Cyc_Absyn_IntType_struct*_tmp1025=(
struct Cyc_Absyn_IntType_struct*)_tmp1023;if(_tmp1025->tag != 6)goto _LL826;}_LL825:
goto _LL827;_LL826: {struct Cyc_Absyn_FloatType_struct*_tmp1026=(struct Cyc_Absyn_FloatType_struct*)
_tmp1023;if(_tmp1026->tag != 7)goto _LL828;}_LL827: return 1;_LL828: {struct Cyc_Absyn_PointerType_struct*
_tmp1027=(struct Cyc_Absyn_PointerType_struct*)_tmp1023;if(_tmp1027->tag != 5)goto
_LL82A;else{_tmp1028=_tmp1027->f1;_tmp1029=_tmp1028.ptr_atts;_tmp102A=_tmp1029.nullable;
_tmp102B=_tmp1029.bounds;}}_LL829: {void*_tmp1039=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp102B);_LL839: {
struct Cyc_Absyn_DynEither_b_struct*_tmp103A=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp1039;if(_tmp103A->tag != 0)goto _LL83B;}_LL83A: return 1;_LL83B:;_LL83C: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp102A);
_LL838:;}_LL82A: {struct Cyc_Absyn_ArrayType_struct*_tmp102C=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1023;if(_tmp102C->tag != 8)goto _LL82C;else{_tmp102D=_tmp102C->f1;_tmp102E=
_tmp102D.elt_type;}}_LL82B: return Cyc_Tcutil_supports_default(_tmp102E);_LL82C: {
struct Cyc_Absyn_TupleType_struct*_tmp102F=(struct Cyc_Absyn_TupleType_struct*)
_tmp1023;if(_tmp102F->tag != 10)goto _LL82E;else{_tmp1030=_tmp102F->f1;}}_LL82D:
for(0;_tmp1030 != 0;_tmp1030=_tmp1030->tl){if(!Cyc_Tcutil_supports_default((*((
struct _tuple12*)_tmp1030->hd)).f2))return 0;}return 1;_LL82E: {struct Cyc_Absyn_AggrType_struct*
_tmp1031=(struct Cyc_Absyn_AggrType_struct*)_tmp1023;if(_tmp1031->tag != 11)goto
_LL830;else{_tmp1032=_tmp1031->f1;_tmp1033=_tmp1032.aggr_info;_tmp1034=_tmp1032.targs;}}
_LL82F: {struct Cyc_Absyn_Aggrdecl*_tmp103B=Cyc_Absyn_get_known_aggrdecl(_tmp1033);
if(_tmp103B->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp103B->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmp103B->tvs,_tmp1034,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp103B->impl))->fields);}
_LL830: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1035=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1023;if(_tmp1035->tag != 12)goto _LL832;else{_tmp1036=_tmp1035->f2;}}_LL831:
return Cyc_Tcutil_fields_support_default(0,0,_tmp1036);_LL832: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp1037=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp1023;if(_tmp1037->tag != 14)
goto _LL834;}_LL833: goto _LL835;_LL834: {struct Cyc_Absyn_EnumType_struct*_tmp1038=(
struct Cyc_Absyn_EnumType_struct*)_tmp1023;if(_tmp1038->tag != 13)goto _LL836;}
_LL835: return 1;_LL836:;_LL837: return 0;_LL821:;}void Cyc_Tcutil_check_no_qual(
struct Cyc_Position_Segment*loc,void*t){void*_tmp103C=t;struct Cyc_Absyn_Typedefdecl*
_tmp103E;_LL83E: {struct Cyc_Absyn_TypedefType_struct*_tmp103D=(struct Cyc_Absyn_TypedefType_struct*)
_tmp103C;if(_tmp103D->tag != 17)goto _LL840;else{_tmp103E=_tmp103D->f3;}}_LL83F:
if(_tmp103E != 0){struct Cyc_Absyn_Tqual _tmp103F=_tmp103E->tq;if(((_tmp103F.print_const
 || _tmp103F.q_volatile) || _tmp103F.q_restrict) || _tmp103F.real_const){const
char*_tmp1638;void*_tmp1637[1];struct Cyc_String_pa_struct _tmp1636;(_tmp1636.tag=
0,((_tmp1636.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp1637[0]=& _tmp1636,Cyc_Tcutil_warn(loc,((_tmp1638="qualifier within typedef type %s is ignored",
_tag_dyneither(_tmp1638,sizeof(char),44))),_tag_dyneither(_tmp1637,sizeof(void*),
1)))))));}}goto _LL83D;_LL840:;_LL841: goto _LL83D;_LL83D:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){struct
_RegionHandle _tmp1043=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1043;
_push_region(rgn);{struct Cyc_List_List*_tmp1044=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmp1044,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);if(!Cyc_Tcutil_supports_default(
t)){int _tmp1045=0;_npop_handler(0);return _tmp1045;}}}{int _tmp1046=1;
_npop_handler(0);return _tmp1046;};;_pop_region(rgn);}int Cyc_Tcutil_admits_zero(
void*t){void*_tmp1047=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp104B;
struct Cyc_Absyn_PtrAtts _tmp104C;union Cyc_Absyn_Constraint*_tmp104D;union Cyc_Absyn_Constraint*
_tmp104E;_LL843: {struct Cyc_Absyn_IntType_struct*_tmp1048=(struct Cyc_Absyn_IntType_struct*)
_tmp1047;if(_tmp1048->tag != 6)goto _LL845;}_LL844: goto _LL846;_LL845: {struct Cyc_Absyn_FloatType_struct*
_tmp1049=(struct Cyc_Absyn_FloatType_struct*)_tmp1047;if(_tmp1049->tag != 7)goto
_LL847;}_LL846: return 1;_LL847: {struct Cyc_Absyn_PointerType_struct*_tmp104A=(
struct Cyc_Absyn_PointerType_struct*)_tmp1047;if(_tmp104A->tag != 5)goto _LL849;
else{_tmp104B=_tmp104A->f1;_tmp104C=_tmp104B.ptr_atts;_tmp104D=_tmp104C.nullable;
_tmp104E=_tmp104C.bounds;}}_LL848: {void*_tmp104F=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp104E);_LL84C: {
struct Cyc_Absyn_DynEither_b_struct*_tmp1050=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp104F;if(_tmp1050->tag != 0)goto _LL84E;}_LL84D: return 0;_LL84E:;_LL84F: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp104D);
_LL84B:;}_LL849:;_LL84A: return 0;_LL842:;}int Cyc_Tcutil_is_noreturn(void*t){{void*
_tmp1051=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1053;void*_tmp1054;
struct Cyc_Absyn_FnInfo _tmp1056;struct Cyc_List_List*_tmp1057;_LL851: {struct Cyc_Absyn_PointerType_struct*
_tmp1052=(struct Cyc_Absyn_PointerType_struct*)_tmp1051;if(_tmp1052->tag != 5)goto
_LL853;else{_tmp1053=_tmp1052->f1;_tmp1054=_tmp1053.elt_typ;}}_LL852: return Cyc_Tcutil_is_noreturn(
_tmp1054);_LL853: {struct Cyc_Absyn_FnType_struct*_tmp1055=(struct Cyc_Absyn_FnType_struct*)
_tmp1051;if(_tmp1055->tag != 9)goto _LL855;else{_tmp1056=_tmp1055->f1;_tmp1057=
_tmp1056.attributes;}}_LL854: for(0;_tmp1057 != 0;_tmp1057=_tmp1057->tl){void*
_tmp1058=(void*)_tmp1057->hd;_LL858: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp1059=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp1058;if(_tmp1059->tag != 4)
goto _LL85A;}_LL859: return 1;_LL85A:;_LL85B: continue;_LL857:;}goto _LL850;_LL855:;
_LL856: goto _LL850;_LL850:;}return 0;}struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts){void*_tmp105A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmp105C;struct Cyc_List_List*_tmp105D;struct Cyc_List_List**_tmp105E;_LL85D: {
struct Cyc_Absyn_FnType_struct*_tmp105B=(struct Cyc_Absyn_FnType_struct*)_tmp105A;
if(_tmp105B->tag != 9)goto _LL85F;else{_tmp105C=_tmp105B->f1;_tmp105D=_tmp105C.attributes;
_tmp105E=(struct Cyc_List_List**)&(_tmp105B->f1).attributes;}}_LL85E: {struct Cyc_List_List*
_tmp105F=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1639;*_tmp105E=((_tmp1639=_cycalloc(sizeof(*_tmp1639)),((
_tmp1639->hd=(void*)atts->hd,((_tmp1639->tl=*_tmp105E,_tmp1639))))));}else{
struct Cyc_List_List*_tmp163A;_tmp105F=((_tmp163A=_cycalloc(sizeof(*_tmp163A)),((
_tmp163A->hd=(void*)atts->hd,((_tmp163A->tl=_tmp105F,_tmp163A))))));}}return
_tmp105F;}_LL85F:;_LL860: {const char*_tmp163D;void*_tmp163C;(_tmp163C=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp163D="transfer_fn_type_atts",
_tag_dyneither(_tmp163D,sizeof(char),22))),_tag_dyneither(_tmp163C,sizeof(void*),
0)));}_LL85C:;}
