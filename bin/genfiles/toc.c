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
tag;struct _dyneither_ptr f1;};int Cyc_vfprintf(struct Cyc___cycFILE*,struct
_dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[
17];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_NewRegion Cyc_Core__new_dynregion(
const char*file,int lineno);extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};void Cyc_Core_free_dynregion(struct _DynRegionHandle*);inline static void* 
arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){if(bd >> 20  || sz
>> 12)return 0;{unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < dyn.curr  || dyn.curr
== 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)return 0;return dyn.curr;};}
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(
struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(
struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void Cyc_List_iter_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};void*Cyc_List_nth(
struct Cyc_List_List*x,int n);int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*
x);int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct
_dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,
int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,
struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);
extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict
Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));struct Cyc_Dict_Dict
Cyc_Dict_rshare(struct _RegionHandle*,struct Cyc_Dict_Dict);struct Cyc_Dict_Dict Cyc_Dict_insert(
struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,
void*k);void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct
Cyc_Position_Segment;struct _dyneither_ptr Cyc_Position_string_of_segment(struct
Cyc_Position_Segment*);struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};struct Cyc_Xarray_Xarray{struct _RegionHandle*r;struct _dyneither_ptr
elmts;int num_elmts;};int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);void*Cyc_Xarray_get(
struct Cyc_Xarray_Xarray*,int);struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(
struct _RegionHandle*);int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};
union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n
Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(
struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*
ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public
 = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct
Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;struct
Cyc_Position_Segment*loc;};enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz
 = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};enum 
Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top
 = 2};enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind
 = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct
Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None
 = 2};enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct
_union_Constraint_Eq_constr{int tag;void*val;};struct
_union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct
_union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr
Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_struct{
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
int tag;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual
tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct
Cyc_Core_Opt*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{
struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{
int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;
struct _tuple0*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};
struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct
_union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{
struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr
KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct
Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_Aggr_td_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};
struct Cyc_Absyn_Enum_td_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;struct
Cyc_Position_Segment*loc;};struct Cyc_Absyn_VoidType_struct{int tag;};struct Cyc_Absyn_Evar_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;};
struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_struct{
int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_struct{
int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{
int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;enum 
Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_struct{int
tag;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_struct{int tag;struct
Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct
Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple0*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_ValueofType_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};
struct Cyc_Absyn_HeapRgn_struct{int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;
};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_TypeDeclType_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;
};extern struct Cyc_Absyn_HeapRgn_struct Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_VoidType_struct
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
_tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct
_tuple3 val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct
_union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6
val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct
_union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,
char);union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);union Cyc_Absyn_Cnst
Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(
enum Cyc_Absyn_Sign,long long);union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct
_dyneither_ptr);enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus
 = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt
 = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,
Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor
 = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 
18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc
 = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,
Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion
 = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**
elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple0*f1;};struct
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
int tag;struct Cyc_List_List*f1;};struct _tuple7{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple7*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;};
struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*
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
_tuple8{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple8 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple8 f2;struct _tuple8 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple8 f2;};struct Cyc_Absyn_TryCatch_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_Skip_s_struct Cyc_Absyn_Skip_s_val;
struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*loc;struct Cyc_List_List*
non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_struct{int tag;};
struct Cyc_Absyn_Var_p_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*
f2;};struct Cyc_Absyn_Reference_p_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct
Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{int tag;struct Cyc_Absyn_Tvar*
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
int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_Wild_p_struct Cyc_Absyn_Wild_p_val;
struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};extern struct Cyc_Absyn_Unresolved_b_struct
Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;
struct Cyc_Core_Opt*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;
};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope
sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum Cyc_Absyn_Scope
sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;
struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{
struct _tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};
struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*
fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;
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
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};extern struct Cyc_Absyn_EmptyAnnot_struct Cyc_Absyn_EmptyAnnot_val;int
Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);void*Cyc_Absyn_conref_val(
union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*
x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_false_conref;extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_sint_typ;void*Cyc_Absyn_exn_typ();extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple0*name);void*Cyc_Absyn_unionq_typ(struct
_tuple0*name);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,
struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
enum Cyc_Absyn_Sign,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dyneither_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dyneither_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
struct _tuple0*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(
struct _tuple0*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,
struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(
void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,
struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct
Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*
r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);int Cyc_Absyn_is_lvalue(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _dyneither_ptr*Cyc_Absyn_fieldname(
int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU
info);extern int Cyc_Absyn_no_regions;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int
add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;
int print_all_kinds;int print_all_effects;int print_using_stmts;int
print_externC_stmts;int print_full_evars;int print_zeroterm;int
generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple0*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(
void*t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(
void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;int Cyc_Tcutil_unify(void*,void*);
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*
rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dyneither,void**elt_type);struct _tuple9{struct Cyc_Absyn_Tqual f1;
void*f2;};void*Cyc_Tcutil_snd_tqt(struct _tuple9*);void Cyc_Tcutil_check_no_qual(
struct Cyc_Position_Segment*loc,void*t);struct _tuple10{unsigned int f1;int f2;};
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_c_can_eval(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};
struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL
 = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};struct
_union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple11{struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple11 val;};struct
_union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessConst{int tag;unsigned int val;};struct
_union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct
_union_RelnOp_LessEqConst LessEqConst;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;union Cyc_CfFlowInfo_RelnOp rop;};extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*
val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{
struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct
Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_struct{
int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{
int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{int tag;enum Cyc_CfFlowInfo_InitLevel
f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};
struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};
struct Cyc_CfFlowInfo_Aggregate_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;
struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_struct{int tag;struct Cyc_Absyn_Exp*
f1;int f2;void*f3;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple12{
struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{
int tag;struct _tuple12 val;};union Cyc_CfFlowInfo_FlowInfo{struct
_union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;
void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*
esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict
mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_UniqueRgn_k_struct{
int tag;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);struct _tuple0*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;extern int Cyc_noexpand_r;int Cyc_Toc_warn_bounds_checks=
0;int Cyc_Toc_warn_all_null_deref=0;unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;static struct Cyc_List_List*Cyc_Toc_result_decls=
0;struct Cyc_Toc_TocState{struct _DynRegionHandle*dyn;struct Cyc_List_List**
tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_Set_Set**datatypes_so_far;
struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*
temp_labels;};static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=0;struct _tuple13{
struct _tuple0*f1;struct _dyneither_ptr f2;};int Cyc_Toc_qvar_tag_cmp(struct _tuple13*
x,struct _tuple13*y){struct _tuple0*_tmp1;struct _dyneither_ptr _tmp2;struct _tuple13
_tmp0=*x;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;{struct _tuple0*_tmp4;struct _dyneither_ptr
_tmp5;struct _tuple13 _tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{int i=Cyc_Absyn_qvar_cmp(
_tmp1,_tmp4);if(i != 0)return i;return Cyc_strcmp((struct _dyneither_ptr)_tmp2,(
struct _dyneither_ptr)_tmp5);};};}struct _tuple14{struct Cyc_Absyn_Aggrdecl*f1;void*
f2;};void*Cyc_Toc_aggrdecl_type(struct _tuple0*q,void*(*type_maker)(struct _tuple0*)){
struct _DynRegionHandle*_tmp7;struct Cyc_Dict_Dict*_tmp8;struct Cyc_Toc_TocState
_tmp6=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp7=_tmp6.dyn;
_tmp8=_tmp6.aggrs_so_far;{struct _DynRegionFrame _tmp9;struct _RegionHandle*d=
_open_dynregion(& _tmp9,_tmp7);{struct _tuple14**v=((struct _tuple14**(*)(struct Cyc_Dict_Dict
d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp8,q);if(v == 0){void*_tmpA=
type_maker(q);_npop_handler(0);return _tmpA;}else{struct _tuple14 _tmpC;void*_tmpD;
struct _tuple14*_tmpB=*v;_tmpC=*_tmpB;_tmpD=_tmpC.f2;{void*_tmpE=_tmpD;
_npop_handler(0);return _tmpE;};}};_pop_dynregion(d);};}static int Cyc_Toc_tuple_type_counter=
0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=0;
char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_struct{
char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_struct{
char*tag;};static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const char*_tmpA69;void*_tmpA68;(_tmpA68=0,
Cyc_fprintf(Cyc_stderr,((_tmpA69="\n",_tag_dyneither(_tmpA69,sizeof(char),2))),
_tag_dyneither(_tmpA68,sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_struct _tmpA6C;struct Cyc_Toc_Toc_Unimplemented_struct*
_tmpA6B;(int)_throw((void*)((_tmpA6B=_cycalloc_atomic(sizeof(*_tmpA6B)),((
_tmpA6B[0]=((_tmpA6C.tag=Cyc_Toc_Toc_Unimplemented,_tmpA6C)),_tmpA6B)))));};}
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(
Cyc_stderr,fmt,ap);{const char*_tmpA6F;void*_tmpA6E;(_tmpA6E=0,Cyc_fprintf(Cyc_stderr,((
_tmpA6F="\n",_tag_dyneither(_tmpA6F,sizeof(char),2))),_tag_dyneither(_tmpA6E,
sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Impossible_struct
_tmpA72;struct Cyc_Toc_Toc_Impossible_struct*_tmpA71;(int)_throw((void*)((_tmpA71=
_cycalloc_atomic(sizeof(*_tmpA71)),((_tmpA71[0]=((_tmpA72.tag=Cyc_Toc_Toc_Impossible,
_tmpA72)),_tmpA71)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_struct{
char*tag;};static char _tmp17[5]="curr";static struct _dyneither_ptr Cyc_Toc_curr_string={
_tmp17,_tmp17,_tmp17 + 5};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static char _tmp18[4]="tag";static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp18,
_tmp18,_tmp18 + 4};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;
static char _tmp19[4]="val";static struct _dyneither_ptr Cyc_Toc_val_string={_tmp19,
_tmp19,_tmp19 + 4};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;
static char _tmp1A[14]="_handler_cons";static struct _dyneither_ptr Cyc_Toc__handler_cons_string={
_tmp1A,_tmp1A,_tmp1A + 14};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string;static char _tmp1B[8]="handler";static struct
_dyneither_ptr Cyc_Toc_handler_string={_tmp1B,_tmp1B,_tmp1B + 8};static struct
_dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp1C[14]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp1C,_tmp1C,_tmp1C + 14};
static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static char _tmp1D[17]="_DynRegionHandle";static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={
_tmp1D,_tmp1D,_tmp1D + 17};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=&
Cyc_Toc__DynRegionHandle_string;static char _tmp1E[16]="_DynRegionFrame";static
struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp1E,_tmp1E,_tmp1E + 16};
static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};static char
_tmp1F[7]="_throw";static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp1F,_tmp1F,
_tmp1F + 7};static struct _tuple0 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(void*)((
void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={
0,(void*)& Cyc_Toc__throw_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*
Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp21[7]="setjmp";static struct
_dyneither_ptr Cyc_Toc_setjmp_str={_tmp21,_tmp21,_tmp21 + 7};static struct _tuple0
Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0,(void*)&
Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;
static char _tmp23[14]="_push_handler";static struct _dyneither_ptr Cyc_Toc__push_handler_str={
_tmp23,_tmp23,_tmp23 + 14};static struct _tuple0 Cyc_Toc__push_handler_pr={{.Loc_n={
4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={
1,& Cyc_Toc__push_handler_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=&
Cyc_Toc__push_handler_ev;static char _tmp25[13]="_pop_handler";static struct
_dyneither_ptr Cyc_Toc__pop_handler_str={_tmp25,_tmp25,_tmp25 + 13};static struct
_tuple0 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={1,& Cyc_Toc__pop_handler_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)& Cyc_Toc__pop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char
_tmp27[12]="_exn_thrown";static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={
_tmp27,_tmp27,_tmp27 + 12};static struct _tuple0 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,
0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={
1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=&
Cyc_Toc__exn_thrown_ev;static char _tmp29[14]="_npop_handler";static struct
_dyneither_ptr Cyc_Toc__npop_handler_str={_tmp29,_tmp29,_tmp29 + 14};static struct
_tuple0 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={
0,(void*)& Cyc_Toc__npop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char
_tmp2B[12]="_check_null";static struct _dyneither_ptr Cyc_Toc__check_null_str={
_tmp2B,_tmp2B,_tmp2B + 12};static struct _tuple0 Cyc_Toc__check_null_pr={{.Loc_n={4,
0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={
1,& Cyc_Toc__check_null_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev;static char _tmp2D[28]="_check_known_subscript_null";static
struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp2D,_tmp2D,
_tmp2D + 28};static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={
4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)& Cyc_Toc__check_known_subscript_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp2F[31]="_check_known_subscript_notnull";static struct _dyneither_ptr
Cyc_Toc__check_known_subscript_notnull_str={_tmp2F,_tmp2F,_tmp2F + 31};static
struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={1,&
Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)&
Cyc_Toc__check_known_subscript_notnull_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;
static char _tmp31[27]="_check_dyneither_subscript";static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={
_tmp31,_tmp31,_tmp31 + 27};static struct _tuple0 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={
4,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={
0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;
static char _tmp33[15]="_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={
_tmp33,_tmp33,_tmp33 + 15};static struct _tuple0 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={
4,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_re={
1,& Cyc_Toc__dyneither_ptr_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=&
Cyc_Toc__dyneither_ptr_ev;static char _tmp35[15]="_tag_dyneither";static struct
_dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp35,_tmp35,_tmp35 + 15};static struct
_tuple0 Cyc_Toc__tag_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_dyneither_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_dyneither_re={1,& Cyc_Toc__tag_dyneither_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={
0,(void*)& Cyc_Toc__tag_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static
char _tmp37[20]="_init_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={
_tmp37,_tmp37,_tmp37 + 20};static struct _tuple0 Cyc_Toc__init_dyneither_ptr_pr={{.Loc_n={
4,0}},& Cyc_Toc__init_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_dyneither_ptr_re={
1,& Cyc_Toc__init_dyneither_ptr_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(void*)& Cyc_Toc__init_dyneither_ptr_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=&
Cyc_Toc__init_dyneither_ptr_ev;static char _tmp39[21]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp39,_tmp39,_tmp39
+ 21};static struct _tuple0 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_dyneither_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={
0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;
static char _tmp3B[20]="_get_dyneither_size";static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={
_tmp3B,_tmp3B,_tmp3B + 20};static struct _tuple0 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={
4,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_dyneither_size_re={
1,& Cyc_Toc__get_dyneither_size_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=&
Cyc_Toc__get_dyneither_size_ev;static char _tmp3D[19]="_get_zero_arr_size";static
struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp3D,_tmp3D,_tmp3D + 19};
static struct _tuple0 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;
static char _tmp3F[24]="_get_zero_arr_size_char";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={
_tmp3F,_tmp3F,_tmp3F + 24};static struct _tuple0 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={
4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_char_re={1,& Cyc_Toc__get_zero_arr_size_char_pr,(void*)((
void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;
static char _tmp41[25]="_get_zero_arr_size_short";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={
_tmp41,_tmp41,_tmp41 + 25};static struct _tuple0 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={
4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_short_re={1,& Cyc_Toc__get_zero_arr_size_short_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;
static char _tmp43[23]="_get_zero_arr_size_int";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={
_tmp43,_tmp43,_tmp43 + 23};static struct _tuple0 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={
4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_int_re={
1,& Cyc_Toc__get_zero_arr_size_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=&
Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp45[25]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp45,_tmp45,
_tmp45 + 25};static struct _tuple0 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},&
Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_float_re={
1,& Cyc_Toc__get_zero_arr_size_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=&
Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp47[26]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp47,_tmp47,
_tmp47 + 26};static struct _tuple0 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,
0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_double_re={1,& Cyc_Toc__get_zero_arr_size_double_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;
static char _tmp49[30]="_get_zero_arr_size_longdouble";static struct _dyneither_ptr
Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp49,_tmp49,_tmp49 + 30};static
struct _tuple0 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,&
Cyc_Toc__get_zero_arr_size_longdouble_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=&
Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp4B[28]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp4B,_tmp4B,
_tmp4B + 28};static struct _tuple0 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={
4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_voidstar_re={1,& Cyc_Toc__get_zero_arr_size_voidstar_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;
static char _tmp4D[20]="_dyneither_ptr_plus";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={
_tmp4D,_tmp4D,_tmp4D + 20};static struct _tuple0 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={
4,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_plus_re={
1,& Cyc_Toc__dyneither_ptr_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=&
Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp4F[15]="_zero_arr_plus";static
struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp4F,_tmp4F,_tmp4F + 15};static
struct _tuple0 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static
char _tmp51[20]="_zero_arr_plus_char";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={
_tmp51,_tmp51,_tmp51 + 20};static struct _tuple0 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_char_re={
1,& Cyc_Toc__zero_arr_plus_char_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=&
Cyc_Toc__zero_arr_plus_char_ev;static char _tmp53[21]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp53,_tmp53,_tmp53
+ 21};static struct _tuple0 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_short_re={1,& Cyc_Toc__zero_arr_plus_short_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;
static char _tmp55[19]="_zero_arr_plus_int";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={
_tmp55,_tmp55,_tmp55 + 19};static struct _tuple0 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_int_re={
1,& Cyc_Toc__zero_arr_plus_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=&
Cyc_Toc__zero_arr_plus_int_ev;static char _tmp57[21]="_zero_arr_plus_float";static
struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp57,_tmp57,_tmp57 + 21};
static struct _tuple0 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_float_re={1,& Cyc_Toc__zero_arr_plus_float_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;
static char _tmp59[22]="_zero_arr_plus_double";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={
_tmp59,_tmp59,_tmp59 + 22};static struct _tuple0 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_double_re={
1,& Cyc_Toc__zero_arr_plus_double_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=&
Cyc_Toc__zero_arr_plus_double_ev;static char _tmp5B[26]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp5B,_tmp5B,
_tmp5B + 26};static struct _tuple0 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,
0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_plus_longdouble_re={1,& Cyc_Toc__zero_arr_plus_longdouble_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;
static char _tmp5D[24]="_zero_arr_plus_voidstar";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={
_tmp5D,_tmp5D,_tmp5D + 24};static struct _tuple0 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_plus_voidstar_re={1,& Cyc_Toc__zero_arr_plus_voidstar_pr,(void*)((
void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;
static char _tmp5F[28]="_dyneither_ptr_inplace_plus";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={
_tmp5F,_tmp5F,_tmp5F + 28};static struct _tuple0 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={
4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={
0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;
static char _tmp61[23]="_zero_arr_inplace_plus";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp61,_tmp61,_tmp61 + 23};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_re={
1,& Cyc_Toc__zero_arr_inplace_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=&
Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp63[28]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp63,_tmp63,
_tmp63 + 28};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_char_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;
static char _tmp65[29]="_zero_arr_inplace_plus_short";static struct _dyneither_ptr
Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp65,_tmp65,_tmp65 + 29};static struct
_tuple0 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,&
Cyc_Toc__zero_arr_inplace_plus_short_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=&
Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp67[27]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp67,_tmp67,
_tmp67 + 27};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,
0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_int_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;
static char _tmp69[29]="_zero_arr_inplace_plus_float";static struct _dyneither_ptr
Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp69,_tmp69,_tmp69 + 29};static struct
_tuple0 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,&
Cyc_Toc__zero_arr_inplace_plus_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=&
Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp6B[30]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp6B,
_tmp6B,_tmp6B + 30};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_double_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;
static char _tmp6D[34]="_zero_arr_inplace_plus_longdouble";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp6D,_tmp6D,
_tmp6D + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;
static char _tmp6F[32]="_zero_arr_inplace_plus_voidstar";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp6F,_tmp6F,_tmp6F + 
32};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},&
Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;
static char _tmp71[33]="_dyneither_ptr_inplace_plus_post";static struct
_dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp71,_tmp71,_tmp71
+ 33};static struct _tuple0 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={4,
0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={
0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;
static char _tmp73[28]="_zero_arr_inplace_plus_post";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={
_tmp73,_tmp73,_tmp73 + 28};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmp75[33]="_zero_arr_inplace_plus_post_char";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp75,_tmp75,_tmp75
+ 33};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,
0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;
static char _tmp77[34]="_zero_arr_inplace_plus_post_short";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp77,_tmp77,
_tmp77 + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;
static char _tmp79[32]="_zero_arr_inplace_plus_post_int";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp79,_tmp79,_tmp79 + 
32};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},&
Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;
static char _tmp7B[34]="_zero_arr_inplace_plus_post_float";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp7B,_tmp7B,
_tmp7B + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;
static char _tmp7D[35]="_zero_arr_inplace_plus_post_double";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp7D,_tmp7D,
_tmp7D + 35};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;
static char _tmp7F[39]="_zero_arr_inplace_plus_post_longdouble";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp7F,_tmp7F,
_tmp7F + 39};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;
static char _tmp81[37]="_zero_arr_inplace_plus_post_voidstar";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp81,_tmp81,
_tmp81 + 37};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;
static char _tmp83[10]="_cycalloc";static struct _dyneither_ptr Cyc_Toc__cycalloc_str={
_tmp83,_tmp83,_tmp83 + 10};static struct _tuple0 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},&
Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_re={1,&
Cyc_Toc__cycalloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct
Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char
_tmp85[11]="_cyccalloc";static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmp85,
_tmp85,_tmp85 + 11};static struct _tuple0 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={
0,(void*)& Cyc_Toc__cyccalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct
Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp87[17]="_cycalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp87,_tmp87,_tmp87 + 17};
static struct _tuple0 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={
0,(void*)& Cyc_Toc__cycalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;
static char _tmp89[18]="_cyccalloc_atomic";static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={
_tmp89,_tmp89,_tmp89 + 18};static struct _tuple0 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={
4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={
1,& Cyc_Toc__cyccalloc_atomic_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=&
Cyc_Toc__cyccalloc_atomic_ev;static char _tmp8B[15]="_region_malloc";static struct
_dyneither_ptr Cyc_Toc__region_malloc_str={_tmp8B,_tmp8B,_tmp8B + 15};static struct
_tuple0 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={
0,(void*)& Cyc_Toc__region_malloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static
char _tmp8D[15]="_region_calloc";static struct _dyneither_ptr Cyc_Toc__region_calloc_str={
_tmp8D,_tmp8D,_tmp8D + 15};static struct _tuple0 Cyc_Toc__region_calloc_pr={{.Loc_n={
4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={
1,& Cyc_Toc__region_calloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=&
Cyc_Toc__region_calloc_ev;static char _tmp8F[13]="_check_times";static struct
_dyneither_ptr Cyc_Toc__check_times_str={_tmp8F,_tmp8F,_tmp8F + 13};static struct
_tuple0 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={
0,(void*)& Cyc_Toc__check_times_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char
_tmp91[12]="_new_region";static struct _dyneither_ptr Cyc_Toc__new_region_str={
_tmp91,_tmp91,_tmp91 + 12};static struct _tuple0 Cyc_Toc__new_region_pr={{.Loc_n={4,
0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={
1,& Cyc_Toc__new_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=&
Cyc_Toc__new_region_ev;static char _tmp93[13]="_push_region";static struct
_dyneither_ptr Cyc_Toc__push_region_str={_tmp93,_tmp93,_tmp93 + 13};static struct
_tuple0 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={
0,(void*)& Cyc_Toc__push_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char
_tmp95[12]="_pop_region";static struct _dyneither_ptr Cyc_Toc__pop_region_str={
_tmp95,_tmp95,_tmp95 + 12};static struct _tuple0 Cyc_Toc__pop_region_pr={{.Loc_n={4,
0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={
1,& Cyc_Toc__pop_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=&
Cyc_Toc__pop_region_ev;static char _tmp97[16]="_open_dynregion";static struct
_dyneither_ptr Cyc_Toc__open_dynregion_str={_tmp97,_tmp97,_tmp97 + 16};static
struct _tuple0 Cyc_Toc__open_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__open_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={
0,(void*)& Cyc_Toc__open_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;static
char _tmp99[16]="_push_dynregion";static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={
_tmp99,_tmp99,_tmp99 + 16};static struct _tuple0 Cyc_Toc__push_dynregion_pr={{.Loc_n={
4,0}},& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_dynregion_re={
1,& Cyc_Toc__push_dynregion_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)& Cyc_Toc__push_dynregion_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=&
Cyc_Toc__push_dynregion_ev;static char _tmp9B[15]="_pop_dynregion";static struct
_dyneither_ptr Cyc_Toc__pop_dynregion_str={_tmp9B,_tmp9B,_tmp9B + 15};static struct
_tuple0 Cyc_Toc__pop_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={
0,(void*)& Cyc_Toc__pop_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static
char _tmp9D[14]="_reset_region";static struct _dyneither_ptr Cyc_Toc__reset_region_str={
_tmp9D,_tmp9D,_tmp9D + 14};static struct _tuple0 Cyc_Toc__reset_region_pr={{.Loc_n={
4,0}},& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__reset_region_re={
1,& Cyc_Toc__reset_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)& Cyc_Toc__reset_region_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=&
Cyc_Toc__reset_region_ev;static char _tmp9F[19]="_throw_arraybounds";static struct
_dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmp9F,_tmp9F,_tmp9F + 19};static
struct _tuple0 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={
0,(void*)& Cyc_Toc__throw_arraybounds_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;
static char _tmpA1[29]="_dyneither_ptr_decrease_size";static struct _dyneither_ptr
Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpA1,_tmpA1,_tmpA1 + 29};static struct
_tuple0 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,&
Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=&
Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpA3[11]="_swap_word";static
struct _dyneither_ptr Cyc_Toc__swap_word_str={_tmpA3,_tmpA3,_tmpA3 + 11};static
struct _tuple0 Cyc_Toc__swap_word_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_word_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_word_re={1,& Cyc_Toc__swap_word_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={
0,(void*)& Cyc_Toc__swap_word_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct
Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char _tmpA5[16]="_swap_dyneither";
static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={_tmpA5,_tmpA5,_tmpA5 + 16};
static struct _tuple0 Cyc_Toc__swap_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_dyneither_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_dyneither_re={1,& Cyc_Toc__swap_dyneither_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={
0,(void*)& Cyc_Toc__swap_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;static
char _tmpA7[13]="_throw_match";static struct _dyneither_ptr Cyc_Toc__throw_match_str={
_tmpA7,_tmpA7,_tmpA7 + 13};static struct _tuple0 Cyc_Toc__throw_match_pr={{.Loc_n={4,
0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_match_re={
1,& Cyc_Toc__throw_match_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=&
Cyc_Toc__throw_match_ev;static struct Cyc_Absyn_AggrType_struct Cyc_Toc_dyneither_ptr_typ_v={
12,{{.UnknownAggr={1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;static
struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;if(skip_stmt_opt == 0){struct Cyc_Absyn_Stmt**
_tmpA73;skip_stmt_opt=((_tmpA73=_cycalloc(sizeof(*_tmpA73)),((_tmpA73[0]=Cyc_Absyn_skip_stmt(
0),_tmpA73))));}return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(
void*t,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,
0);}static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Cast_e_struct
_tmpA76;struct Cyc_Absyn_Cast_e_struct*_tmpA75;return(void*)((_tmpA75=_cycalloc(
sizeof(*_tmpA75)),((_tmpA75[0]=((_tmpA76.tag=15,((_tmpA76.f1=(void*)t,((_tmpA76.f2=
e,((_tmpA76.f3=0,((_tmpA76.f4=Cyc_Absyn_No_coercion,_tmpA76)))))))))),_tmpA75))));}
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Deref_e_struct
_tmpA79;struct Cyc_Absyn_Deref_e_struct*_tmpA78;return(void*)((_tmpA78=_cycalloc(
sizeof(*_tmpA78)),((_tmpA78[0]=((_tmpA79.tag=21,((_tmpA79.f1=e,_tmpA79)))),
_tmpA78))));}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){struct Cyc_Absyn_Subscript_e_struct _tmpA7C;struct Cyc_Absyn_Subscript_e_struct*
_tmpA7B;return(void*)((_tmpA7B=_cycalloc(sizeof(*_tmpA7B)),((_tmpA7B[0]=((
_tmpA7C.tag=24,((_tmpA7C.f1=e1,((_tmpA7C.f2=e2,_tmpA7C)))))),_tmpA7B))));}static
void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct
_tmpA7F;struct Cyc_Absyn_StmtExp_e_struct*_tmpA7E;return(void*)((_tmpA7E=
_cycalloc(sizeof(*_tmpA7E)),((_tmpA7E[0]=((_tmpA7F.tag=37,((_tmpA7F.f1=s,_tmpA7F)))),
_tmpA7E))));}static void*Cyc_Toc_sizeoftyp_exp_r(void*t){struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpA82;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpA81;return(void*)((_tmpA81=
_cycalloc(sizeof(*_tmpA81)),((_tmpA81[0]=((_tmpA82.tag=18,((_tmpA82.f1=(void*)t,
_tmpA82)))),_tmpA81))));}static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct _tmpA85;struct Cyc_Absyn_FnCall_e_struct*
_tmpA84;return(void*)((_tmpA84=_cycalloc(sizeof(*_tmpA84)),((_tmpA84[0]=((
_tmpA85.tag=11,((_tmpA85.f1=e,((_tmpA85.f2=es,((_tmpA85.f3=0,_tmpA85)))))))),
_tmpA84))));}static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_struct
_tmpA88;struct Cyc_Absyn_Exp_s_struct*_tmpA87;return(void*)((_tmpA87=_cycalloc(
sizeof(*_tmpA87)),((_tmpA87[0]=((_tmpA88.tag=1,((_tmpA88.f1=e,_tmpA88)))),
_tmpA87))));}static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2){struct Cyc_Absyn_Seq_s_struct _tmpA8B;struct Cyc_Absyn_Seq_s_struct*_tmpA8A;
return(void*)((_tmpA8A=_cycalloc(sizeof(*_tmpA8A)),((_tmpA8A[0]=((_tmpA8B.tag=2,((
_tmpA8B.f1=s1,((_tmpA8B.f2=s2,_tmpA8B)))))),_tmpA8A))));}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct
_tmpA8E;struct Cyc_Absyn_Conditional_e_struct*_tmpA8D;return(void*)((_tmpA8D=
_cycalloc(sizeof(*_tmpA8D)),((_tmpA8D[0]=((_tmpA8E.tag=6,((_tmpA8E.f1=e1,((
_tmpA8E.f2=e2,((_tmpA8E.f3=e3,_tmpA8E)))))))),_tmpA8D))));}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrMember_e_struct
_tmpA91;struct Cyc_Absyn_AggrMember_e_struct*_tmpA90;return(void*)((_tmpA90=
_cycalloc(sizeof(*_tmpA90)),((_tmpA90[0]=((_tmpA91.tag=22,((_tmpA91.f1=e,((
_tmpA91.f2=n,((_tmpA91.f3=0,((_tmpA91.f4=0,_tmpA91)))))))))),_tmpA90))));}static
void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_struct _tmpA94;struct Cyc_Absyn_AggrArrow_e_struct*
_tmpA93;return(void*)((_tmpA93=_cycalloc(sizeof(*_tmpA93)),((_tmpA93[0]=((
_tmpA94.tag=23,((_tmpA94.f1=e,((_tmpA94.f2=n,((_tmpA94.f3=0,((_tmpA94.f4=0,
_tmpA94)))))))))),_tmpA93))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmpA97;
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpA96;return(void*)((_tmpA96=_cycalloc(
sizeof(*_tmpA96)),((_tmpA96[0]=((_tmpA97.tag=36,((_tmpA97.f1=tdopt,((_tmpA97.f2=
ds,_tmpA97)))))),_tmpA96))));}static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_struct _tmpA9A;
struct Cyc_Absyn_Goto_s_struct*_tmpA99;return(void*)((_tmpA99=_cycalloc(sizeof(*
_tmpA99)),((_tmpA99[0]=((_tmpA9A.tag=8,((_tmpA9A.f1=v,((_tmpA9A.f2=s,_tmpA9A)))))),
_tmpA99))));}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,{.Int_c={4,{
Cyc_Absyn_Signed,0}}}};struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;
struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;
struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*
fvoidstar;};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,&
Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,&
Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,&
Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,&
Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,&
Cyc_Toc__get_zero_arr_size_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(
struct Cyc_Toc_functionSet*fS,void*t){struct Cyc_Absyn_Exp*function;{void*_tmpC6=
Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmpC8;int _tmpCB;_LL1: {struct Cyc_Absyn_IntType_struct*
_tmpC7=(struct Cyc_Absyn_IntType_struct*)_tmpC6;if(_tmpC7->tag != 6)goto _LL3;else{
_tmpC8=_tmpC7->f2;}}_LL2: switch(_tmpC8){case Cyc_Absyn_Char_sz: _LLB: function=fS->fchar;
break;case Cyc_Absyn_Short_sz: _LLC: function=fS->fshort;break;case Cyc_Absyn_Int_sz:
_LLD: function=fS->fint;break;default: _LLE: {struct Cyc_Core_Impossible_struct
_tmpAA0;const char*_tmpA9F;struct Cyc_Core_Impossible_struct*_tmpA9E;(int)_throw((
void*)((_tmpA9E=_cycalloc(sizeof(*_tmpA9E)),((_tmpA9E[0]=((_tmpAA0.tag=Cyc_Core_Impossible,((
_tmpAA0.f1=((_tmpA9F="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpA9F,sizeof(char),44))),_tmpAA0)))),_tmpA9E)))));}}goto _LL0;
_LL3: {struct Cyc_Absyn_FloatType_struct*_tmpC9=(struct Cyc_Absyn_FloatType_struct*)
_tmpC6;if(_tmpC9->tag != 7)goto _LL5;}_LL4: function=fS->ffloat;goto _LL0;_LL5: {
struct Cyc_Absyn_DoubleType_struct*_tmpCA=(struct Cyc_Absyn_DoubleType_struct*)
_tmpC6;if(_tmpCA->tag != 8)goto _LL7;else{_tmpCB=_tmpCA->f1;}}_LL6: switch(_tmpCB){
case 1: _LL10: function=fS->flongdouble;break;default: _LL11: function=fS->fdouble;}
goto _LL0;_LL7: {struct Cyc_Absyn_PointerType_struct*_tmpCC=(struct Cyc_Absyn_PointerType_struct*)
_tmpC6;if(_tmpCC->tag != 5)goto _LL9;}_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;
_LLA: {struct Cyc_Core_Impossible_struct _tmpAAD;const char*_tmpAAC;void*_tmpAAB[1];
struct Cyc_String_pa_struct _tmpAAA;struct Cyc_Core_Impossible_struct*_tmpAA9;(int)
_throw((void*)((_tmpAA9=_cycalloc(sizeof(*_tmpAA9)),((_tmpAA9[0]=((_tmpAAD.tag=
Cyc_Core_Impossible,((_tmpAAD.f1=(struct _dyneither_ptr)((_tmpAAA.tag=0,((_tmpAAA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAAB[
0]=& _tmpAAA,Cyc_aprintf(((_tmpAAC="impossible expression type %s (not int, float, double, or pointer)",
_tag_dyneither(_tmpAAC,sizeof(char),67))),_tag_dyneither(_tmpAAB,sizeof(void*),1)))))))),
_tmpAAD)))),_tmpAA9)))));}_LL0:;}return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(
fS,(void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v);}struct Cyc_Absyn_Exp*
Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*
arr){void*_tmpD5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
arr->topt))->v);struct Cyc_Absyn_PtrInfo _tmpD7;void*_tmpD8;_LL14: {struct Cyc_Absyn_PointerType_struct*
_tmpD6=(struct Cyc_Absyn_PointerType_struct*)_tmpD5;if(_tmpD6->tag != 5)goto _LL16;
else{_tmpD7=_tmpD6->f1;_tmpD8=_tmpD7.elt_typ;}}_LL15: return Cyc_Toc_getFunctionType(
fS,_tmpD8);_LL16:;_LL17: {struct Cyc_Core_Impossible_struct _tmpAB3;const char*
_tmpAB2;struct Cyc_Core_Impossible_struct*_tmpAB1;(int)_throw((void*)((_tmpAB1=
_cycalloc(sizeof(*_tmpAB1)),((_tmpAB1[0]=((_tmpAB3.tag=Cyc_Core_Impossible,((
_tmpAB3.f1=((_tmpAB2="impossible type (not pointer)",_tag_dyneither(_tmpAB2,
sizeof(char),30))),_tmpAB3)))),_tmpAB1)))));}_LL13:;}struct _tuple15{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){void*
_tmpDC=e->r;union Cyc_Absyn_Cnst _tmpDE;struct _tuple3 _tmpDF;char _tmpE0;union Cyc_Absyn_Cnst
_tmpE2;struct _tuple4 _tmpE3;short _tmpE4;union Cyc_Absyn_Cnst _tmpE6;struct _tuple5
_tmpE7;int _tmpE8;union Cyc_Absyn_Cnst _tmpEA;struct _tuple6 _tmpEB;long long _tmpEC;
union Cyc_Absyn_Cnst _tmpEE;int _tmpEF;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_List_List*
_tmpF3;struct Cyc_List_List*_tmpF5;struct Cyc_List_List*_tmpF7;struct Cyc_List_List*
_tmpF9;struct Cyc_List_List*_tmpFB;_LL19: {struct Cyc_Absyn_Const_e_struct*_tmpDD=(
struct Cyc_Absyn_Const_e_struct*)_tmpDC;if(_tmpDD->tag != 0)goto _LL1B;else{_tmpDE=
_tmpDD->f1;if((_tmpDE.Char_c).tag != 2)goto _LL1B;_tmpDF=(struct _tuple3)(_tmpDE.Char_c).val;
_tmpE0=_tmpDF.f2;}}_LL1A: return _tmpE0 == '\000';_LL1B: {struct Cyc_Absyn_Const_e_struct*
_tmpE1=(struct Cyc_Absyn_Const_e_struct*)_tmpDC;if(_tmpE1->tag != 0)goto _LL1D;
else{_tmpE2=_tmpE1->f1;if((_tmpE2.Short_c).tag != 3)goto _LL1D;_tmpE3=(struct
_tuple4)(_tmpE2.Short_c).val;_tmpE4=_tmpE3.f2;}}_LL1C: return _tmpE4 == 0;_LL1D: {
struct Cyc_Absyn_Const_e_struct*_tmpE5=(struct Cyc_Absyn_Const_e_struct*)_tmpDC;
if(_tmpE5->tag != 0)goto _LL1F;else{_tmpE6=_tmpE5->f1;if((_tmpE6.Int_c).tag != 4)
goto _LL1F;_tmpE7=(struct _tuple5)(_tmpE6.Int_c).val;_tmpE8=_tmpE7.f2;}}_LL1E:
return _tmpE8 == 0;_LL1F: {struct Cyc_Absyn_Const_e_struct*_tmpE9=(struct Cyc_Absyn_Const_e_struct*)
_tmpDC;if(_tmpE9->tag != 0)goto _LL21;else{_tmpEA=_tmpE9->f1;if((_tmpEA.LongLong_c).tag
!= 5)goto _LL21;_tmpEB=(struct _tuple6)(_tmpEA.LongLong_c).val;_tmpEC=_tmpEB.f2;}}
_LL20: return _tmpEC == 0;_LL21: {struct Cyc_Absyn_Const_e_struct*_tmpED=(struct Cyc_Absyn_Const_e_struct*)
_tmpDC;if(_tmpED->tag != 0)goto _LL23;else{_tmpEE=_tmpED->f1;if((_tmpEE.Null_c).tag
!= 1)goto _LL23;_tmpEF=(int)(_tmpEE.Null_c).val;}}_LL22: return 1;_LL23: {struct Cyc_Absyn_Cast_e_struct*
_tmpF0=(struct Cyc_Absyn_Cast_e_struct*)_tmpDC;if(_tmpF0->tag != 15)goto _LL25;
else{_tmpF1=_tmpF0->f2;}}_LL24: return Cyc_Toc_is_zero(_tmpF1);_LL25: {struct Cyc_Absyn_Tuple_e_struct*
_tmpF2=(struct Cyc_Absyn_Tuple_e_struct*)_tmpDC;if(_tmpF2->tag != 25)goto _LL27;
else{_tmpF3=_tmpF2->f1;}}_LL26: return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpF3);_LL27: {struct
Cyc_Absyn_Array_e_struct*_tmpF4=(struct Cyc_Absyn_Array_e_struct*)_tmpDC;if(
_tmpF4->tag != 27)goto _LL29;else{_tmpF5=_tmpF4->f1;}}_LL28: _tmpF7=_tmpF5;goto
_LL2A;_LL29: {struct Cyc_Absyn_Aggregate_e_struct*_tmpF6=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpDC;if(_tmpF6->tag != 29)goto _LL2B;else{_tmpF7=_tmpF6->f3;}}_LL2A: _tmpF9=
_tmpF7;goto _LL2C;_LL2B: {struct Cyc_Absyn_CompoundLit_e_struct*_tmpF8=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpDC;if(_tmpF8->tag != 26)goto _LL2D;else{_tmpF9=_tmpF8->f2;}}_LL2C: _tmpFB=
_tmpF9;goto _LL2E;_LL2D: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpFA=(struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmpDC;if(_tmpFA->tag != 36)goto _LL2F;else{
_tmpFB=_tmpFA->f2;}}_LL2E: for(0;_tmpFB != 0;_tmpFB=_tmpFB->tl){if(!Cyc_Toc_is_zero((*((
struct _tuple15*)_tmpFB->hd)).f2))return 0;}return 1;_LL2F:;_LL30: return 0;_LL18:;}
static int Cyc_Toc_is_nullable(void*t){void*_tmpFC=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_PtrInfo _tmpFE;struct Cyc_Absyn_PtrAtts _tmpFF;union Cyc_Absyn_Constraint*
_tmp100;_LL32: {struct Cyc_Absyn_PointerType_struct*_tmpFD=(struct Cyc_Absyn_PointerType_struct*)
_tmpFC;if(_tmpFD->tag != 5)goto _LL34;else{_tmpFE=_tmpFD->f1;_tmpFF=_tmpFE.ptr_atts;
_tmp100=_tmpFF.nullable;}}_LL33: return((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp100);_LL34:;_LL35: {const char*_tmpAB6;void*_tmpAB5;(
_tmpAB5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAB6="is_nullable",_tag_dyneither(_tmpAB6,sizeof(char),12))),_tag_dyneither(
_tmpAB5,sizeof(void*),0)));}_LL31:;}static char _tmp10F[1]="";static char _tmp110[8]="*bogus*";
static struct _tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct
_dyneither_ptr tag){struct _DynRegionHandle*_tmp104;struct Cyc_Dict_Dict*_tmp105;
struct Cyc_Toc_TocState _tmp103=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp104=_tmp103.dyn;_tmp105=_tmp103.qvar_tags;{static struct _dyneither_ptr
bogus_string={_tmp110,_tmp110,_tmp110 + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
4,0}},& bogus_string};static struct _tuple13 pair={& bogus_qvar,{_tmp10F,_tmp10F,
_tmp10F + 1}};{struct _tuple13 _tmpAB7;pair=((_tmpAB7.f1=x,((_tmpAB7.f2=tag,_tmpAB7))));}{
struct _DynRegionFrame _tmp107;struct _RegionHandle*d=_open_dynregion(& _tmp107,
_tmp104);{struct _tuple0**_tmp108=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,
struct _tuple13*k))Cyc_Dict_lookup_opt)(*_tmp105,(struct _tuple13*)& pair);if(
_tmp108 != 0){struct _tuple0*_tmp109=*_tmp108;_npop_handler(0);return _tmp109;}{
struct _tuple13*_tmpAB8;struct _tuple13*_tmp10A=(_tmpAB8=_cycalloc(sizeof(*_tmpAB8)),((
_tmpAB8->f1=x,((_tmpAB8->f2=tag,_tmpAB8)))));struct _dyneither_ptr*_tmpABB;struct
_tuple0*_tmpABA;struct _tuple0*res=(_tmpABA=_cycalloc(sizeof(*_tmpABA)),((_tmpABA->f1=(*
x).f1,((_tmpABA->f2=((_tmpABB=_cycalloc(sizeof(*_tmpABB)),((_tmpABB[0]=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*x).f2,(struct
_dyneither_ptr)tag),_tmpABB)))),_tmpABA)))));*_tmp105=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple13*k,struct _tuple0*v))Cyc_Dict_insert)(*_tmp105,(
struct _tuple13*)_tmp10A,res);{struct _tuple0*_tmp10B=res;_npop_handler(0);return
_tmp10B;};};};_pop_dynregion(d);};};}struct _tuple16{void*f1;struct Cyc_List_List*
f2;};static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){struct
_DynRegionHandle*_tmp112;struct Cyc_List_List**_tmp113;struct Cyc_Toc_TocState
_tmp111=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp112=
_tmp111.dyn;_tmp113=_tmp111.tuple_types;{struct _DynRegionFrame _tmp114;struct
_RegionHandle*d=_open_dynregion(& _tmp114,_tmp112);{struct Cyc_List_List*_tmp115=*
_tmp113;for(0;_tmp115 != 0;_tmp115=_tmp115->tl){struct _tuple16 _tmp117;void*
_tmp118;struct Cyc_List_List*_tmp119;struct _tuple16*_tmp116=(struct _tuple16*)
_tmp115->hd;_tmp117=*_tmp116;_tmp118=_tmp117.f1;_tmp119=_tmp117.f2;{struct Cyc_List_List*
_tmp11A=tqs0;for(0;_tmp11A != 0  && _tmp119 != 0;(_tmp11A=_tmp11A->tl,_tmp119=
_tmp119->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple9*)_tmp11A->hd)).f2,(void*)
_tmp119->hd))break;}if(_tmp11A == 0  && _tmp119 == 0){void*_tmp11B=_tmp118;
_npop_handler(0);return _tmp11B;}};}}{struct Cyc_Int_pa_struct _tmpAC3;void*_tmpAC2[
1];const char*_tmpAC1;struct _dyneither_ptr*_tmpAC0;struct _dyneither_ptr*xname=(
_tmpAC0=_cycalloc(sizeof(*_tmpAC0)),((_tmpAC0[0]=(struct _dyneither_ptr)((_tmpAC3.tag=
1,((_tmpAC3.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAC2[0]=& _tmpAC3,
Cyc_aprintf(((_tmpAC1="_tuple%d",_tag_dyneither(_tmpAC1,sizeof(char),9))),
_tag_dyneither(_tmpAC2,sizeof(void*),1)))))))),_tmpAC0)));void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp11C=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmpAC6;struct Cyc_List_List*
_tmpAC5;_tmp11C=((_tmpAC5=_cycalloc(sizeof(*_tmpAC5)),((_tmpAC5->hd=((_tmpAC6=
_cycalloc(sizeof(*_tmpAC6)),((_tmpAC6->name=Cyc_Absyn_fieldname(i),((_tmpAC6->tq=
Cyc_Toc_mt_tq,((_tmpAC6->type=(void*)ts2->hd,((_tmpAC6->width=0,((_tmpAC6->attributes=
0,_tmpAC6)))))))))))),((_tmpAC5->tl=_tmp11C,_tmpAC5))))));}}_tmp11C=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp11C);{struct Cyc_Absyn_AggrdeclImpl*
_tmpACB;struct _tuple0*_tmpACA;struct Cyc_Absyn_Aggrdecl*_tmpAC9;struct Cyc_Absyn_Aggrdecl*
_tmp11F=(_tmpAC9=_cycalloc(sizeof(*_tmpAC9)),((_tmpAC9->kind=Cyc_Absyn_StructA,((
_tmpAC9->sc=Cyc_Absyn_Public,((_tmpAC9->name=((_tmpACA=_cycalloc(sizeof(*_tmpACA)),((
_tmpACA->f1=Cyc_Absyn_Rel_n(0),((_tmpACA->f2=xname,_tmpACA)))))),((_tmpAC9->tvs=
0,((_tmpAC9->impl=((_tmpACB=_cycalloc(sizeof(*_tmpACB)),((_tmpACB->exist_vars=0,((
_tmpACB->rgn_po=0,((_tmpACB->fields=_tmp11C,((_tmpACB->tagged=0,_tmpACB)))))))))),((
_tmpAC9->attributes=0,_tmpAC9)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*
_tmpAD1;struct Cyc_Absyn_Aggr_d_struct _tmpAD0;struct Cyc_List_List*_tmpACF;Cyc_Toc_result_decls=((
_tmpACF=_cycalloc(sizeof(*_tmpACF)),((_tmpACF->hd=Cyc_Absyn_new_decl((void*)((
_tmpAD1=_cycalloc(sizeof(*_tmpAD1)),((_tmpAD1[0]=((_tmpAD0.tag=6,((_tmpAD0.f1=
_tmp11F,_tmpAD0)))),_tmpAD1)))),0),((_tmpACF->tl=Cyc_Toc_result_decls,_tmpACF))))));}{
struct _tuple16*_tmpAD4;struct Cyc_List_List*_tmpAD3;*_tmp113=((_tmpAD3=
_region_malloc(d,sizeof(*_tmpAD3)),((_tmpAD3->hd=((_tmpAD4=_region_malloc(d,
sizeof(*_tmpAD4)),((_tmpAD4->f1=x,((_tmpAD4->f2=ts,_tmpAD4)))))),((_tmpAD3->tl=*
_tmp113,_tmpAD3))))));}{void*_tmp125=x;_npop_handler(0);return _tmp125;};};};;
_pop_dynregion(d);};}struct _tuple0*Cyc_Toc_temp_var(){int _tmp12D=Cyc_Toc_temp_var_counter
++;struct _dyneither_ptr*_tmpAE1;const char*_tmpAE0;void*_tmpADF[1];struct Cyc_Int_pa_struct
_tmpADE;struct _tuple0*_tmpADD;struct _tuple0*res=(_tmpADD=_cycalloc(sizeof(*
_tmpADD)),((_tmpADD->f1=Cyc_Absyn_Loc_n,((_tmpADD->f2=((_tmpAE1=_cycalloc(
sizeof(*_tmpAE1)),((_tmpAE1[0]=(struct _dyneither_ptr)((_tmpADE.tag=1,((_tmpADE.f1=(
unsigned int)_tmp12D,((_tmpADF[0]=& _tmpADE,Cyc_aprintf(((_tmpAE0="_tmp%X",
_tag_dyneither(_tmpAE0,sizeof(char),7))),_tag_dyneither(_tmpADF,sizeof(void*),1)))))))),
_tmpAE1)))),_tmpADD)))));return res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp134;struct Cyc_Xarray_Xarray*_tmp135;struct Cyc_Toc_TocState
_tmp133=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp134=
_tmp133.dyn;_tmp135=_tmp133.temp_labels;{struct _DynRegionFrame _tmp136;struct
_RegionHandle*d=_open_dynregion(& _tmp136,_tmp134);{int _tmp137=Cyc_Toc_fresh_label_counter
++;if(_tmp137 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp135)){
struct _dyneither_ptr*_tmp138=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp135,_tmp137);_npop_handler(0);return _tmp138;}{struct Cyc_Int_pa_struct
_tmpAE9;void*_tmpAE8[1];const char*_tmpAE7;struct _dyneither_ptr*_tmpAE6;struct
_dyneither_ptr*res=(_tmpAE6=_cycalloc(sizeof(*_tmpAE6)),((_tmpAE6[0]=(struct
_dyneither_ptr)((_tmpAE9.tag=1,((_tmpAE9.f1=(unsigned int)_tmp137,((_tmpAE8[0]=&
_tmpAE9,Cyc_aprintf(((_tmpAE7="_LL%X",_tag_dyneither(_tmpAE7,sizeof(char),6))),
_tag_dyneither(_tmpAE8,sizeof(void*),1)))))))),_tmpAE6)));if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp135,res)!= _tmp137){const char*
_tmpAEC;void*_tmpAEB;(_tmpAEB=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAEC="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmpAEC,sizeof(char),43))),_tag_dyneither(_tmpAEB,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp13B=res;_npop_handler(0);return _tmp13B;};};};
_pop_dynregion(d);};}static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*
td,struct _tuple0*name){int ans=0;struct Cyc_List_List*_tmp140=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((
struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp140))->hd)->name)
!= 0){++ ans;_tmp140=_tmp140->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
static void*Cyc_Toc_typ_to_c(void*t);static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*
ed);static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int
add_to_result_decls);static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud);static struct _tuple7*Cyc_Toc_arg_to_c(struct _tuple7*a){struct Cyc_Core_Opt*
_tmp142;struct Cyc_Absyn_Tqual _tmp143;void*_tmp144;struct _tuple7 _tmp141=*a;
_tmp142=_tmp141.f1;_tmp143=_tmp141.f2;_tmp144=_tmp141.f3;{struct _tuple7*_tmpAED;
return(_tmpAED=_cycalloc(sizeof(*_tmpAED)),((_tmpAED->f1=_tmp142,((_tmpAED->f2=
_tmp143,((_tmpAED->f3=Cyc_Toc_typ_to_c(_tmp144),_tmpAED)))))));};}static struct
_tuple9*Cyc_Toc_typ_to_c_f(struct _tuple9*x){struct Cyc_Absyn_Tqual _tmp147;void*
_tmp148;struct _tuple9 _tmp146=*x;_tmp147=_tmp146.f1;_tmp148=_tmp146.f2;{struct
_tuple9*_tmpAEE;return(_tmpAEE=_cycalloc(sizeof(*_tmpAEE)),((_tmpAEE->f1=_tmp147,((
_tmpAEE->f2=Cyc_Toc_typ_to_c(_tmp148),_tmpAEE)))));};}static void*Cyc_Toc_typ_to_c_array(
void*t){void*_tmp14A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp14C;
void*_tmp14D;struct Cyc_Absyn_Tqual _tmp14E;struct Cyc_Absyn_Exp*_tmp14F;union Cyc_Absyn_Constraint*
_tmp150;struct Cyc_Position_Segment*_tmp151;struct Cyc_Core_Opt*_tmp153;struct Cyc_Core_Opt
_tmp154;void*_tmp155;_LL37: {struct Cyc_Absyn_ArrayType_struct*_tmp14B=(struct Cyc_Absyn_ArrayType_struct*)
_tmp14A;if(_tmp14B->tag != 9)goto _LL39;else{_tmp14C=_tmp14B->f1;_tmp14D=_tmp14C.elt_type;
_tmp14E=_tmp14C.tq;_tmp14F=_tmp14C.num_elts;_tmp150=_tmp14C.zero_term;_tmp151=
_tmp14C.zt_loc;}}_LL38: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp14D),
_tmp14E,_tmp14F,Cyc_Absyn_false_conref,_tmp151);_LL39: {struct Cyc_Absyn_Evar_struct*
_tmp152=(struct Cyc_Absyn_Evar_struct*)_tmp14A;if(_tmp152->tag != 1)goto _LL3B;
else{_tmp153=_tmp152->f2;if(_tmp153 == 0)goto _LL3B;_tmp154=*_tmp153;_tmp155=(void*)
_tmp154.v;}}_LL3A: return Cyc_Toc_typ_to_c_array(_tmp155);_LL3B:;_LL3C: return Cyc_Toc_typ_to_c(
t);_LL36:;}static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*
f){struct Cyc_Absyn_Aggrfield*_tmpAEF;return(_tmpAEF=_cycalloc(sizeof(*_tmpAEF)),((
_tmpAEF->name=f->name,((_tmpAEF->tq=Cyc_Toc_mt_tq,((_tmpAEF->type=Cyc_Toc_typ_to_c(
f->type),((_tmpAEF->width=f->width,((_tmpAEF->attributes=f->attributes,_tmpAEF)))))))))));}
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){return;}static void*Cyc_Toc_char_star_typ(){
static void**cs=0;if(cs == 0){void**_tmpAF0;cs=((_tmpAF0=_cycalloc(sizeof(*_tmpAF0)),((
_tmpAF0[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpAF0))));}return*cs;}static void*Cyc_Toc_rgn_typ(){
static void**r=0;if(r == 0){void**_tmpAF1;r=((_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((
_tmpAF1[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),
_tmpAF1))));}return*r;}static void*Cyc_Toc_dyn_rgn_typ(){static void**r=0;if(r == 0){
void**_tmpAF2;r=((_tmpAF2=_cycalloc(sizeof(*_tmpAF2)),((_tmpAF2[0]=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpAF2))));}return*
r;}static void*Cyc_Toc_typ_to_c(void*t){void*_tmp15A=t;struct Cyc_Core_Opt*_tmp15D;
struct Cyc_Core_Opt*_tmp15F;struct Cyc_Core_Opt _tmp160;void*_tmp161;struct Cyc_Absyn_Tvar*
_tmp163;struct Cyc_Absyn_DatatypeFieldInfo _tmp166;union Cyc_Absyn_DatatypeFieldInfoU
_tmp167;struct _tuple1 _tmp168;struct Cyc_Absyn_Datatypedecl*_tmp169;struct Cyc_Absyn_Datatypefield*
_tmp16A;struct Cyc_Absyn_PtrInfo _tmp16D;void*_tmp16E;struct Cyc_Absyn_Tqual _tmp16F;
struct Cyc_Absyn_PtrAtts _tmp170;union Cyc_Absyn_Constraint*_tmp171;struct Cyc_Absyn_ArrayInfo
_tmp176;void*_tmp177;struct Cyc_Absyn_Tqual _tmp178;struct Cyc_Absyn_Exp*_tmp179;
struct Cyc_Position_Segment*_tmp17A;struct Cyc_Absyn_FnInfo _tmp17C;struct Cyc_Absyn_Tqual
_tmp17D;void*_tmp17E;struct Cyc_List_List*_tmp17F;int _tmp180;struct Cyc_Absyn_VarargInfo*
_tmp181;struct Cyc_List_List*_tmp182;struct Cyc_List_List*_tmp184;enum Cyc_Absyn_AggrKind
_tmp186;struct Cyc_List_List*_tmp187;struct Cyc_Absyn_AggrInfo _tmp189;union Cyc_Absyn_AggrInfoU
_tmp18A;struct Cyc_List_List*_tmp18B;struct _tuple0*_tmp18D;struct Cyc_List_List*
_tmp18F;struct _tuple0*_tmp191;struct Cyc_List_List*_tmp192;struct Cyc_Absyn_Typedefdecl*
_tmp193;void**_tmp194;void*_tmp197;struct Cyc_Absyn_TypeDecl*_tmp1A0;struct Cyc_Absyn_TypeDecl
_tmp1A1;void*_tmp1A2;struct Cyc_Absyn_Aggrdecl*_tmp1A4;struct Cyc_Absyn_TypeDecl*
_tmp1A6;struct Cyc_Absyn_TypeDecl _tmp1A7;void*_tmp1A8;struct Cyc_Absyn_Enumdecl*
_tmp1AA;struct Cyc_Absyn_TypeDecl*_tmp1AC;struct Cyc_Absyn_TypeDecl _tmp1AD;void*
_tmp1AE;struct Cyc_Absyn_Datatypedecl*_tmp1B0;void**_tmp1B1;_LL3E: {struct Cyc_Absyn_VoidType_struct*
_tmp15B=(struct Cyc_Absyn_VoidType_struct*)_tmp15A;if(_tmp15B->tag != 0)goto _LL40;}
_LL3F: return t;_LL40: {struct Cyc_Absyn_Evar_struct*_tmp15C=(struct Cyc_Absyn_Evar_struct*)
_tmp15A;if(_tmp15C->tag != 1)goto _LL42;else{_tmp15D=_tmp15C->f2;if(_tmp15D != 0)
goto _LL42;}}_LL41: return Cyc_Absyn_sint_typ;_LL42: {struct Cyc_Absyn_Evar_struct*
_tmp15E=(struct Cyc_Absyn_Evar_struct*)_tmp15A;if(_tmp15E->tag != 1)goto _LL44;
else{_tmp15F=_tmp15E->f2;if(_tmp15F == 0)goto _LL44;_tmp160=*_tmp15F;_tmp161=(void*)
_tmp160.v;}}_LL43: return Cyc_Toc_typ_to_c(_tmp161);_LL44: {struct Cyc_Absyn_VarType_struct*
_tmp162=(struct Cyc_Absyn_VarType_struct*)_tmp15A;if(_tmp162->tag != 2)goto _LL46;
else{_tmp163=_tmp162->f1;}}_LL45: if((Cyc_Tcutil_tvar_kind(_tmp163,& Cyc_Tcutil_bk))->kind
== Cyc_Absyn_AnyKind)return(void*)& Cyc_Absyn_VoidType_val;else{return Cyc_Absyn_void_star_typ();}
_LL46: {struct Cyc_Absyn_DatatypeType_struct*_tmp164=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp15A;if(_tmp164->tag != 3)goto _LL48;}_LL47: return(void*)& Cyc_Absyn_VoidType_val;
_LL48: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp165=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp15A;if(_tmp165->tag != 4)goto _LL4A;else{_tmp166=_tmp165->f1;_tmp167=_tmp166.field_info;
if((_tmp167.KnownDatatypefield).tag != 2)goto _LL4A;_tmp168=(struct _tuple1)(
_tmp167.KnownDatatypefield).val;_tmp169=_tmp168.f1;_tmp16A=_tmp168.f2;}}_LL49: {
const char*_tmpAF3;return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp16A->name,((
_tmpAF3="_struct",_tag_dyneither(_tmpAF3,sizeof(char),8)))));}_LL4A: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp16B=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp15A;if(_tmp16B->tag != 4)
goto _LL4C;}_LL4B: {const char*_tmpAF6;void*_tmpAF5;(_tmpAF5=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF6="unresolved DatatypeFieldType",
_tag_dyneither(_tmpAF6,sizeof(char),29))),_tag_dyneither(_tmpAF5,sizeof(void*),0)));}
_LL4C: {struct Cyc_Absyn_PointerType_struct*_tmp16C=(struct Cyc_Absyn_PointerType_struct*)
_tmp15A;if(_tmp16C->tag != 5)goto _LL4E;else{_tmp16D=_tmp16C->f1;_tmp16E=_tmp16D.elt_typ;
_tmp16F=_tmp16D.elt_tq;_tmp170=_tmp16D.ptr_atts;_tmp171=_tmp170.bounds;}}_LL4D:
_tmp16E=Cyc_Toc_typ_to_c_array(_tmp16E);{void*_tmp1B5=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp171);_LL7D: {
struct Cyc_Absyn_DynEither_b_struct*_tmp1B6=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp1B5;if(_tmp1B6->tag != 0)goto _LL7F;}_LL7E: return Cyc_Toc_dyneither_ptr_typ;
_LL7F:;_LL80: return Cyc_Absyn_star_typ(_tmp16E,(void*)& Cyc_Absyn_HeapRgn_val,
_tmp16F,Cyc_Absyn_false_conref);_LL7C:;};_LL4E: {struct Cyc_Absyn_IntType_struct*
_tmp172=(struct Cyc_Absyn_IntType_struct*)_tmp15A;if(_tmp172->tag != 6)goto _LL50;}
_LL4F: goto _LL51;_LL50: {struct Cyc_Absyn_FloatType_struct*_tmp173=(struct Cyc_Absyn_FloatType_struct*)
_tmp15A;if(_tmp173->tag != 7)goto _LL52;}_LL51: goto _LL53;_LL52: {struct Cyc_Absyn_DoubleType_struct*
_tmp174=(struct Cyc_Absyn_DoubleType_struct*)_tmp15A;if(_tmp174->tag != 8)goto
_LL54;}_LL53: return t;_LL54: {struct Cyc_Absyn_ArrayType_struct*_tmp175=(struct Cyc_Absyn_ArrayType_struct*)
_tmp15A;if(_tmp175->tag != 9)goto _LL56;else{_tmp176=_tmp175->f1;_tmp177=_tmp176.elt_type;
_tmp178=_tmp176.tq;_tmp179=_tmp176.num_elts;_tmp17A=_tmp176.zt_loc;}}_LL55:
return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp177),_tmp178,_tmp179,Cyc_Absyn_false_conref,
_tmp17A);_LL56: {struct Cyc_Absyn_FnType_struct*_tmp17B=(struct Cyc_Absyn_FnType_struct*)
_tmp15A;if(_tmp17B->tag != 10)goto _LL58;else{_tmp17C=_tmp17B->f1;_tmp17D=_tmp17C.ret_tqual;
_tmp17E=_tmp17C.ret_typ;_tmp17F=_tmp17C.args;_tmp180=_tmp17C.c_varargs;_tmp181=
_tmp17C.cyc_varargs;_tmp182=_tmp17C.attributes;}}_LL57: {struct Cyc_List_List*
_tmp1B7=0;for(0;_tmp182 != 0;_tmp182=_tmp182->tl){void*_tmp1B8=(void*)_tmp182->hd;
_LL82: {struct Cyc_Absyn_Noreturn_att_struct*_tmp1B9=(struct Cyc_Absyn_Noreturn_att_struct*)
_tmp1B8;if(_tmp1B9->tag != 4)goto _LL84;}_LL83: goto _LL85;_LL84: {struct Cyc_Absyn_Const_att_struct*
_tmp1BA=(struct Cyc_Absyn_Const_att_struct*)_tmp1B8;if(_tmp1BA->tag != 5)goto _LL86;}
_LL85: goto _LL87;_LL86: {struct Cyc_Absyn_Format_att_struct*_tmp1BB=(struct Cyc_Absyn_Format_att_struct*)
_tmp1B8;if(_tmp1BB->tag != 19)goto _LL88;}_LL87: continue;_LL88: {struct Cyc_Absyn_Noliveunique_att_struct*
_tmp1BC=(struct Cyc_Absyn_Noliveunique_att_struct*)_tmp1B8;if(_tmp1BC->tag != 21)
goto _LL8A;}_LL89: continue;_LL8A: {struct Cyc_Absyn_Initializes_att_struct*_tmp1BD=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp1B8;if(_tmp1BD->tag != 20)goto _LL8C;}
_LL8B: continue;_LL8C:;_LL8D:{struct Cyc_List_List*_tmpAF7;_tmp1B7=((_tmpAF7=
_cycalloc(sizeof(*_tmpAF7)),((_tmpAF7->hd=(void*)_tmp182->hd,((_tmpAF7->tl=
_tmp1B7,_tmpAF7))))));}goto _LL81;_LL81:;}{struct Cyc_List_List*_tmp1BF=((struct
Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_arg_to_c,_tmp17F);if(_tmp181 != 0){void*_tmp1C0=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp181->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple7*_tmpAF8;struct _tuple7*_tmp1C1=(_tmpAF8=_cycalloc(sizeof(*_tmpAF8)),((
_tmpAF8->f1=_tmp181->name,((_tmpAF8->f2=_tmp181->tq,((_tmpAF8->f3=_tmp1C0,
_tmpAF8)))))));struct Cyc_List_List*_tmpAF9;_tmp1BF=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1BF,((
_tmpAF9=_cycalloc(sizeof(*_tmpAF9)),((_tmpAF9->hd=_tmp1C1,((_tmpAF9->tl=0,
_tmpAF9)))))));}{struct Cyc_Absyn_FnType_struct _tmpAFF;struct Cyc_Absyn_FnInfo
_tmpAFE;struct Cyc_Absyn_FnType_struct*_tmpAFD;return(void*)((_tmpAFD=_cycalloc(
sizeof(*_tmpAFD)),((_tmpAFD[0]=((_tmpAFF.tag=10,((_tmpAFF.f1=((_tmpAFE.tvars=0,((
_tmpAFE.effect=0,((_tmpAFE.ret_tqual=_tmp17D,((_tmpAFE.ret_typ=Cyc_Toc_typ_to_c(
_tmp17E),((_tmpAFE.args=_tmp1BF,((_tmpAFE.c_varargs=_tmp180,((_tmpAFE.cyc_varargs=
0,((_tmpAFE.rgn_po=0,((_tmpAFE.attributes=_tmp1B7,_tmpAFE)))))))))))))))))),
_tmpAFF)))),_tmpAFD))));};};}_LL58: {struct Cyc_Absyn_TupleType_struct*_tmp183=(
struct Cyc_Absyn_TupleType_struct*)_tmp15A;if(_tmp183->tag != 11)goto _LL5A;else{
_tmp184=_tmp183->f1;}}_LL59: _tmp184=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(
struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp184);
return Cyc_Toc_add_tuple_type(_tmp184);_LL5A: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp185=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp15A;if(_tmp185->tag != 13)goto
_LL5C;else{_tmp186=_tmp185->f1;_tmp187=_tmp185->f2;}}_LL5B: {struct Cyc_Absyn_AnonAggrType_struct
_tmpB02;struct Cyc_Absyn_AnonAggrType_struct*_tmpB01;return(void*)((_tmpB01=
_cycalloc(sizeof(*_tmpB01)),((_tmpB01[0]=((_tmpB02.tag=13,((_tmpB02.f1=_tmp186,((
_tmpB02.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp187),_tmpB02)))))),
_tmpB01))));}_LL5C: {struct Cyc_Absyn_AggrType_struct*_tmp188=(struct Cyc_Absyn_AggrType_struct*)
_tmp15A;if(_tmp188->tag != 12)goto _LL5E;else{_tmp189=_tmp188->f1;_tmp18A=_tmp189.aggr_info;
_tmp18B=_tmp189.targs;}}_LL5D:{union Cyc_Absyn_AggrInfoU _tmp1C9=_tmp18A;struct
_tuple2 _tmp1CA;_LL8F: if((_tmp1C9.UnknownAggr).tag != 1)goto _LL91;_tmp1CA=(struct
_tuple2)(_tmp1C9.UnknownAggr).val;_LL90: return t;_LL91:;_LL92: goto _LL8E;_LL8E:;}{
struct Cyc_Absyn_Aggrdecl*_tmp1CB=Cyc_Absyn_get_known_aggrdecl(_tmp18A);if(
_tmp1CB->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(_tmp1CB->name,Cyc_Absyn_unionq_typ);
else{return Cyc_Toc_aggrdecl_type(_tmp1CB->name,Cyc_Absyn_strctq);}};_LL5E: {
struct Cyc_Absyn_EnumType_struct*_tmp18C=(struct Cyc_Absyn_EnumType_struct*)
_tmp15A;if(_tmp18C->tag != 14)goto _LL60;else{_tmp18D=_tmp18C->f1;}}_LL5F: return t;
_LL60: {struct Cyc_Absyn_AnonEnumType_struct*_tmp18E=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp15A;if(_tmp18E->tag != 15)goto _LL62;else{_tmp18F=_tmp18E->f1;}}_LL61: Cyc_Toc_enumfields_to_c(
_tmp18F);return t;_LL62: {struct Cyc_Absyn_TypedefType_struct*_tmp190=(struct Cyc_Absyn_TypedefType_struct*)
_tmp15A;if(_tmp190->tag != 18)goto _LL64;else{_tmp191=_tmp190->f1;_tmp192=_tmp190->f2;
_tmp193=_tmp190->f3;_tmp194=_tmp190->f4;}}_LL63: if(_tmp194 == 0  || Cyc_noexpand_r){
if(_tmp192 != 0){struct Cyc_Absyn_TypedefType_struct _tmpB05;struct Cyc_Absyn_TypedefType_struct*
_tmpB04;return(void*)((_tmpB04=_cycalloc(sizeof(*_tmpB04)),((_tmpB04[0]=((
_tmpB05.tag=18,((_tmpB05.f1=_tmp191,((_tmpB05.f2=0,((_tmpB05.f3=_tmp193,((
_tmpB05.f4=0,_tmpB05)))))))))),_tmpB04))));}else{return t;}}else{struct Cyc_Absyn_TypedefType_struct
_tmpB0B;void**_tmpB0A;struct Cyc_Absyn_TypedefType_struct*_tmpB09;return(void*)((
_tmpB09=_cycalloc(sizeof(*_tmpB09)),((_tmpB09[0]=((_tmpB0B.tag=18,((_tmpB0B.f1=
_tmp191,((_tmpB0B.f2=0,((_tmpB0B.f3=_tmp193,((_tmpB0B.f4=((_tmpB0A=_cycalloc(
sizeof(*_tmpB0A)),((_tmpB0A[0]=Cyc_Toc_typ_to_c_array(*_tmp194),_tmpB0A)))),
_tmpB0B)))))))))),_tmpB09))));}_LL64: {struct Cyc_Absyn_TagType_struct*_tmp195=(
struct Cyc_Absyn_TagType_struct*)_tmp15A;if(_tmp195->tag != 20)goto _LL66;}_LL65:
return Cyc_Absyn_uint_typ;_LL66: {struct Cyc_Absyn_RgnHandleType_struct*_tmp196=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp15A;if(_tmp196->tag != 16)goto _LL68;
else{_tmp197=(void*)_tmp196->f1;}}_LL67: return Cyc_Toc_rgn_typ();_LL68: {struct
Cyc_Absyn_DynRgnType_struct*_tmp198=(struct Cyc_Absyn_DynRgnType_struct*)_tmp15A;
if(_tmp198->tag != 17)goto _LL6A;}_LL69: return Cyc_Toc_dyn_rgn_typ();_LL6A: {struct
Cyc_Absyn_HeapRgn_struct*_tmp199=(struct Cyc_Absyn_HeapRgn_struct*)_tmp15A;if(
_tmp199->tag != 21)goto _LL6C;}_LL6B: {const char*_tmpB0E;void*_tmpB0D;(_tmpB0D=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB0E="Toc::typ_to_c: type translation passed the heap region",_tag_dyneither(
_tmpB0E,sizeof(char),55))),_tag_dyneither(_tmpB0D,sizeof(void*),0)));}_LL6C: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp19A=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp15A;if(_tmp19A->tag != 22)goto _LL6E;}_LL6D: {const char*_tmpB11;void*_tmpB10;(
_tmpB10=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB11="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpB11,sizeof(char),57))),_tag_dyneither(_tmpB10,sizeof(void*),0)));}
_LL6E: {struct Cyc_Absyn_AccessEff_struct*_tmp19B=(struct Cyc_Absyn_AccessEff_struct*)
_tmp15A;if(_tmp19B->tag != 23)goto _LL70;}_LL6F: goto _LL71;_LL70: {struct Cyc_Absyn_JoinEff_struct*
_tmp19C=(struct Cyc_Absyn_JoinEff_struct*)_tmp15A;if(_tmp19C->tag != 24)goto _LL72;}
_LL71: goto _LL73;_LL72: {struct Cyc_Absyn_RgnsEff_struct*_tmp19D=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp15A;if(_tmp19D->tag != 25)goto _LL74;}_LL73: {const char*_tmpB14;void*_tmpB13;(
_tmpB13=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB14="Toc::typ_to_c: type translation passed an effect",_tag_dyneither(
_tmpB14,sizeof(char),49))),_tag_dyneither(_tmpB13,sizeof(void*),0)));}_LL74: {
struct Cyc_Absyn_ValueofType_struct*_tmp19E=(struct Cyc_Absyn_ValueofType_struct*)
_tmp15A;if(_tmp19E->tag != 19)goto _LL76;}_LL75: {const char*_tmpB17;void*_tmpB16;(
_tmpB16=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB17="Toc::typ_to_c: type translation passed a valueof_t",_tag_dyneither(
_tmpB17,sizeof(char),51))),_tag_dyneither(_tmpB16,sizeof(void*),0)));}_LL76: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp19F=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp15A;if(_tmp19F->tag != 26)goto _LL78;else{_tmp1A0=_tmp19F->f1;_tmp1A1=*_tmp1A0;
_tmp1A2=_tmp1A1.r;{struct Cyc_Absyn_Aggr_td_struct*_tmp1A3=(struct Cyc_Absyn_Aggr_td_struct*)
_tmp1A2;if(_tmp1A3->tag != 0)goto _LL78;else{_tmp1A4=_tmp1A3->f1;}};}}_LL77: Cyc_Toc_aggrdecl_to_c(
_tmp1A4,1);if(_tmp1A4->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(
_tmp1A4->name,Cyc_Absyn_unionq_typ);else{return Cyc_Toc_aggrdecl_type(_tmp1A4->name,
Cyc_Absyn_strctq);}_LL78: {struct Cyc_Absyn_TypeDeclType_struct*_tmp1A5=(struct
Cyc_Absyn_TypeDeclType_struct*)_tmp15A;if(_tmp1A5->tag != 26)goto _LL7A;else{
_tmp1A6=_tmp1A5->f1;_tmp1A7=*_tmp1A6;_tmp1A8=_tmp1A7.r;{struct Cyc_Absyn_Enum_td_struct*
_tmp1A9=(struct Cyc_Absyn_Enum_td_struct*)_tmp1A8;if(_tmp1A9->tag != 1)goto _LL7A;
else{_tmp1AA=_tmp1A9->f1;}};}}_LL79: Cyc_Toc_enumdecl_to_c(_tmp1AA);return t;_LL7A: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp1AB=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp15A;if(_tmp1AB->tag != 26)goto _LL3D;else{_tmp1AC=_tmp1AB->f1;_tmp1AD=*_tmp1AC;
_tmp1AE=_tmp1AD.r;{struct Cyc_Absyn_Datatype_td_struct*_tmp1AF=(struct Cyc_Absyn_Datatype_td_struct*)
_tmp1AE;if(_tmp1AF->tag != 2)goto _LL3D;else{_tmp1B0=_tmp1AF->f1;}};_tmp1B1=
_tmp1AB->f2;}}_LL7B: Cyc_Toc_datatypedecl_to_c(_tmp1B0);return Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp1B1)));_LL3D:;}static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(
void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmp1D9=t;struct
Cyc_Absyn_ArrayInfo _tmp1DB;void*_tmp1DC;struct Cyc_Absyn_Tqual _tmp1DD;_LL94: {
struct Cyc_Absyn_ArrayType_struct*_tmp1DA=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1D9;if(_tmp1DA->tag != 9)goto _LL96;else{_tmp1DB=_tmp1DA->f1;_tmp1DC=_tmp1DB.elt_type;
_tmp1DD=_tmp1DB.tq;}}_LL95: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp1DC,(
void*)& Cyc_Absyn_HeapRgn_val,_tmp1DD,Cyc_Absyn_false_conref),e);_LL96:;_LL97:
return Cyc_Toc_cast_it(t,e);_LL93:;}static int Cyc_Toc_atomic_typ(void*t){void*
_tmp1DE=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp1E9;void*_tmp1EA;
struct Cyc_Absyn_AggrInfo _tmp1EC;union Cyc_Absyn_AggrInfoU _tmp1ED;struct Cyc_List_List*
_tmp1EF;struct Cyc_Absyn_DatatypeFieldInfo _tmp1F1;union Cyc_Absyn_DatatypeFieldInfoU
_tmp1F2;struct _tuple1 _tmp1F3;struct Cyc_Absyn_Datatypedecl*_tmp1F4;struct Cyc_Absyn_Datatypefield*
_tmp1F5;struct Cyc_List_List*_tmp1F7;_LL99: {struct Cyc_Absyn_VoidType_struct*
_tmp1DF=(struct Cyc_Absyn_VoidType_struct*)_tmp1DE;if(_tmp1DF->tag != 0)goto _LL9B;}
_LL9A: return 1;_LL9B: {struct Cyc_Absyn_VarType_struct*_tmp1E0=(struct Cyc_Absyn_VarType_struct*)
_tmp1DE;if(_tmp1E0->tag != 2)goto _LL9D;}_LL9C: return 0;_LL9D: {struct Cyc_Absyn_IntType_struct*
_tmp1E1=(struct Cyc_Absyn_IntType_struct*)_tmp1DE;if(_tmp1E1->tag != 6)goto _LL9F;}
_LL9E: goto _LLA0;_LL9F: {struct Cyc_Absyn_EnumType_struct*_tmp1E2=(struct Cyc_Absyn_EnumType_struct*)
_tmp1DE;if(_tmp1E2->tag != 14)goto _LLA1;}_LLA0: goto _LLA2;_LLA1: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp1E3=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp1DE;if(_tmp1E3->tag != 15)goto
_LLA3;}_LLA2: goto _LLA4;_LLA3: {struct Cyc_Absyn_FloatType_struct*_tmp1E4=(struct
Cyc_Absyn_FloatType_struct*)_tmp1DE;if(_tmp1E4->tag != 7)goto _LLA5;}_LLA4: goto
_LLA6;_LLA5: {struct Cyc_Absyn_DoubleType_struct*_tmp1E5=(struct Cyc_Absyn_DoubleType_struct*)
_tmp1DE;if(_tmp1E5->tag != 8)goto _LLA7;}_LLA6: goto _LLA8;_LLA7: {struct Cyc_Absyn_FnType_struct*
_tmp1E6=(struct Cyc_Absyn_FnType_struct*)_tmp1DE;if(_tmp1E6->tag != 10)goto _LLA9;}
_LLA8: goto _LLAA;_LLA9: {struct Cyc_Absyn_TagType_struct*_tmp1E7=(struct Cyc_Absyn_TagType_struct*)
_tmp1DE;if(_tmp1E7->tag != 20)goto _LLAB;}_LLAA: return 1;_LLAB: {struct Cyc_Absyn_ArrayType_struct*
_tmp1E8=(struct Cyc_Absyn_ArrayType_struct*)_tmp1DE;if(_tmp1E8->tag != 9)goto _LLAD;
else{_tmp1E9=_tmp1E8->f1;_tmp1EA=_tmp1E9.elt_type;}}_LLAC: return Cyc_Toc_atomic_typ(
_tmp1EA);_LLAD: {struct Cyc_Absyn_AggrType_struct*_tmp1EB=(struct Cyc_Absyn_AggrType_struct*)
_tmp1DE;if(_tmp1EB->tag != 12)goto _LLAF;else{_tmp1EC=_tmp1EB->f1;_tmp1ED=_tmp1EC.aggr_info;}}
_LLAE:{union Cyc_Absyn_AggrInfoU _tmp1FC=_tmp1ED;struct _tuple2 _tmp1FD;_LLC0: if((
_tmp1FC.UnknownAggr).tag != 1)goto _LLC2;_tmp1FD=(struct _tuple2)(_tmp1FC.UnknownAggr).val;
_LLC1: return 0;_LLC2:;_LLC3: goto _LLBF;_LLBF:;}{struct Cyc_Absyn_Aggrdecl*_tmp1FE=
Cyc_Absyn_get_known_aggrdecl(_tmp1ED);if(_tmp1FE->impl == 0)return 0;{struct Cyc_List_List*
_tmp1FF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1FE->impl))->fields;for(
0;_tmp1FF != 0;_tmp1FF=_tmp1FF->tl){if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)
_tmp1FF->hd)->type))return 0;}}return 1;};_LLAF: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp1EE=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp1DE;if(_tmp1EE->tag != 13)goto
_LLB1;else{_tmp1EF=_tmp1EE->f2;}}_LLB0: for(0;_tmp1EF != 0;_tmp1EF=_tmp1EF->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1EF->hd)->type))return 0;}
return 1;_LLB1: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp1F0=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp1DE;if(_tmp1F0->tag != 4)goto _LLB3;else{_tmp1F1=_tmp1F0->f1;_tmp1F2=_tmp1F1.field_info;
if((_tmp1F2.KnownDatatypefield).tag != 2)goto _LLB3;_tmp1F3=(struct _tuple1)(
_tmp1F2.KnownDatatypefield).val;_tmp1F4=_tmp1F3.f1;_tmp1F5=_tmp1F3.f2;}}_LLB2:
_tmp1F7=_tmp1F5->typs;goto _LLB4;_LLB3: {struct Cyc_Absyn_TupleType_struct*_tmp1F6=(
struct Cyc_Absyn_TupleType_struct*)_tmp1DE;if(_tmp1F6->tag != 11)goto _LLB5;else{
_tmp1F7=_tmp1F6->f1;}}_LLB4: for(0;_tmp1F7 != 0;_tmp1F7=_tmp1F7->tl){if(!Cyc_Toc_atomic_typ((*((
struct _tuple9*)_tmp1F7->hd)).f2))return 0;}return 1;_LLB5: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1F8=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1DE;if(_tmp1F8->tag != 3)goto
_LLB7;}_LLB6: goto _LLB8;_LLB7: {struct Cyc_Absyn_PointerType_struct*_tmp1F9=(
struct Cyc_Absyn_PointerType_struct*)_tmp1DE;if(_tmp1F9->tag != 5)goto _LLB9;}_LLB8:
goto _LLBA;_LLB9: {struct Cyc_Absyn_DynRgnType_struct*_tmp1FA=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp1DE;if(_tmp1FA->tag != 17)goto _LLBB;}_LLBA: goto _LLBC;_LLBB: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp1FB=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp1DE;if(_tmp1FB->tag != 16)goto
_LLBD;}_LLBC: return 0;_LLBD:;_LLBE: {const char*_tmpB1B;void*_tmpB1A[1];struct Cyc_String_pa_struct
_tmpB19;(_tmpB19.tag=0,((_tmpB19.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpB1A[0]=& _tmpB19,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB1B="atomic_typ:  bad type %s",
_tag_dyneither(_tmpB1B,sizeof(char),25))),_tag_dyneither(_tmpB1A,sizeof(void*),1)))))));}
_LL98:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp203=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp205;void*_tmp206;_LLC5: {struct Cyc_Absyn_PointerType_struct*
_tmp204=(struct Cyc_Absyn_PointerType_struct*)_tmp203;if(_tmp204->tag != 5)goto
_LLC7;else{_tmp205=_tmp204->f1;_tmp206=_tmp205.elt_typ;}}_LLC6: {void*_tmp207=
Cyc_Tcutil_compress(_tmp206);_LLCA: {struct Cyc_Absyn_VoidType_struct*_tmp208=(
struct Cyc_Absyn_VoidType_struct*)_tmp207;if(_tmp208->tag != 0)goto _LLCC;}_LLCB:
return 1;_LLCC:;_LLCD: return 0;_LLC9:;}_LLC7:;_LLC8: return 0;_LLC4:;}static int Cyc_Toc_is_tvar(
void*t){void*_tmp209=Cyc_Tcutil_compress(t);_LLCF: {struct Cyc_Absyn_VarType_struct*
_tmp20A=(struct Cyc_Absyn_VarType_struct*)_tmp209;if(_tmp20A->tag != 2)goto _LLD1;}
_LLD0: return 1;_LLD1:;_LLD2: return 0;_LLCE:;}static int Cyc_Toc_is_void_star_or_tvar(
void*t){return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_tvar(t);}static int Cyc_Toc_is_poly_field(
void*t,struct _dyneither_ptr*f){void*_tmp20B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp20D;union Cyc_Absyn_AggrInfoU _tmp20E;struct Cyc_List_List*_tmp210;_LLD4: {
struct Cyc_Absyn_AggrType_struct*_tmp20C=(struct Cyc_Absyn_AggrType_struct*)
_tmp20B;if(_tmp20C->tag != 12)goto _LLD6;else{_tmp20D=_tmp20C->f1;_tmp20E=_tmp20D.aggr_info;}}
_LLD5: {struct Cyc_Absyn_Aggrdecl*_tmp211=Cyc_Absyn_get_known_aggrdecl(_tmp20E);
if(_tmp211->impl == 0){const char*_tmpB1E;void*_tmpB1D;(_tmpB1D=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB1E="is_poly_field: type missing fields",
_tag_dyneither(_tmpB1E,sizeof(char),35))),_tag_dyneither(_tmpB1D,sizeof(void*),0)));}
_tmp210=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp211->impl))->fields;goto
_LLD7;}_LLD6: {struct Cyc_Absyn_AnonAggrType_struct*_tmp20F=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp20B;if(_tmp20F->tag != 13)goto _LLD8;else{_tmp210=_tmp20F->f2;}}_LLD7: {struct
Cyc_Absyn_Aggrfield*_tmp214=Cyc_Absyn_lookup_field(_tmp210,f);if(_tmp214 == 0){
const char*_tmpB22;void*_tmpB21[1];struct Cyc_String_pa_struct _tmpB20;(_tmpB20.tag=
0,((_tmpB20.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB21[0]=&
_tmpB20,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB22="is_poly_field: bad field %s",_tag_dyneither(_tmpB22,sizeof(char),28))),
_tag_dyneither(_tmpB21,sizeof(void*),1)))))));}return Cyc_Toc_is_void_star_or_tvar(
_tmp214->type);}_LLD8:;_LLD9: {const char*_tmpB26;void*_tmpB25[1];struct Cyc_String_pa_struct
_tmpB24;(_tmpB24.tag=0,((_tmpB24.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpB25[0]=& _tmpB24,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB26="is_poly_field: bad type %s",
_tag_dyneither(_tmpB26,sizeof(char),27))),_tag_dyneither(_tmpB25,sizeof(void*),1)))))));}
_LLD3:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmp21B=e->r;
struct Cyc_Absyn_Exp*_tmp21D;struct _dyneither_ptr*_tmp21E;struct Cyc_Absyn_Exp*
_tmp220;struct _dyneither_ptr*_tmp221;_LLDB: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp21C=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp21B;if(_tmp21C->tag != 22)goto
_LLDD;else{_tmp21D=_tmp21C->f1;_tmp21E=_tmp21C->f2;}}_LLDC: return Cyc_Toc_is_poly_field((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp21D->topt))->v,_tmp21E);_LLDD: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp21F=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp21B;if(_tmp21F->tag != 23)goto _LLDF;else{_tmp220=_tmp21F->f1;_tmp221=_tmp21F->f2;}}
_LLDE: {void*_tmp222=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp220->topt))->v);struct Cyc_Absyn_PtrInfo _tmp224;void*_tmp225;
_LLE2: {struct Cyc_Absyn_PointerType_struct*_tmp223=(struct Cyc_Absyn_PointerType_struct*)
_tmp222;if(_tmp223->tag != 5)goto _LLE4;else{_tmp224=_tmp223->f1;_tmp225=_tmp224.elt_typ;}}
_LLE3: return Cyc_Toc_is_poly_field(_tmp225,_tmp221);_LLE4:;_LLE5: {const char*
_tmpB2A;void*_tmpB29[1];struct Cyc_String_pa_struct _tmpB28;(_tmpB28.tag=0,((
_tmpB28.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp220->topt))->v)),((_tmpB29[0]=&
_tmpB28,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB2A="is_poly_project: bad type %s",_tag_dyneither(_tmpB2A,sizeof(char),29))),
_tag_dyneither(_tmpB29,sizeof(void*),1)))))));}_LLE1:;}_LLDF:;_LLE0: return 0;
_LLDA:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB2B;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((
_tmpB2B=_cycalloc(sizeof(*_tmpB2B)),((_tmpB2B->hd=s,((_tmpB2B->tl=0,_tmpB2B)))))),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB2C;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((
_tmpB2C=_cycalloc(sizeof(*_tmpB2C)),((_tmpB2C->hd=s,((_tmpB2C->tl=0,_tmpB2C)))))),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(
s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmpB2D[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((
_tmpB2D[1]=s,((_tmpB2D[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpB2D,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,
struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*
_tmpB2E[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpB2E[1]=n,((
_tmpB2E[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB2E,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{struct Cyc_Absyn_Exp*
_tmpB2F[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpB2F[1]=n,((
_tmpB2F[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB2F,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n){struct Cyc_Absyn_Exp*_tmpB30[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((
_tmpB30[2]=n,((_tmpB30[1]=s,((_tmpB30[0]=rgn,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB30,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmpB31;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmpB31=_cycalloc(sizeof(*_tmpB31)),((_tmpB31->hd=e,((_tmpB31->tl=0,_tmpB31)))))),
0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){struct Cyc_Absyn_Exp*
_tmpB32[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpB32,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*
_tmp231=e->r;union Cyc_Absyn_Cnst _tmp233;struct _dyneither_ptr _tmp234;_LLE7: {
struct Cyc_Absyn_Const_e_struct*_tmp232=(struct Cyc_Absyn_Const_e_struct*)_tmp231;
if(_tmp232->tag != 0)goto _LLE9;else{_tmp233=_tmp232->f1;if((_tmp233.String_c).tag
!= 7)goto _LLE9;_tmp234=(struct _dyneither_ptr)(_tmp233.String_c).val;}}_LLE8:
is_string=1;goto _LLE6;_LLE9:;_LLEA: goto _LLE6;_LLE6:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple0*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_struct*_tmpB38;struct
Cyc_Absyn_Var_d_struct _tmpB37;struct Cyc_List_List*_tmpB36;Cyc_Toc_result_decls=((
_tmpB36=_cycalloc(sizeof(*_tmpB36)),((_tmpB36->hd=Cyc_Absyn_new_decl((void*)((
_tmpB38=_cycalloc(sizeof(*_tmpB38)),((_tmpB38[0]=((_tmpB37.tag=0,((_tmpB37.f1=vd,
_tmpB37)))),_tmpB38)))),0),((_tmpB36->tl=Cyc_Toc_result_decls,_tmpB36))))));}
xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*urm_exp;{struct _tuple15*_tmpB3F;
struct _tuple15*_tmpB3E;struct _tuple15*_tmpB3D;struct _tuple15*_tmpB3C[3];urm_exp=
Cyc_Absyn_unresolvedmem_exp(0,((_tmpB3C[2]=((_tmpB3D=_cycalloc(sizeof(*_tmpB3D)),((
_tmpB3D->f1=0,((_tmpB3D->f2=xplussz,_tmpB3D)))))),((_tmpB3C[1]=((_tmpB3E=
_cycalloc(sizeof(*_tmpB3E)),((_tmpB3E->f1=0,((_tmpB3E->f2=xexp,_tmpB3E)))))),((
_tmpB3C[0]=((_tmpB3F=_cycalloc(sizeof(*_tmpB3F)),((_tmpB3F->f1=0,((_tmpB3F->f2=
xexp,_tmpB3F)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB3C,sizeof(struct _tuple15*),3)))))))),0);}return urm_exp;};};}
struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*
binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct
_dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct
_RegionHandle*rgn;};static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env
_tmp23D;int _tmp23E;struct Cyc_Toc_Env*_tmp23C=nv;_tmp23D=*_tmp23C;_tmp23E=_tmp23D.toplevel;
return _tmp23E;}static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env
_tmp240;int*_tmp241;struct Cyc_Toc_Env*_tmp23F=nv;_tmp240=*_tmp23F;_tmp241=
_tmp240.in_lhs;return*_tmp241;}static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(
struct Cyc_Toc_Env*nv,struct _tuple0*x){struct Cyc_Toc_Env _tmp243;struct Cyc_Dict_Dict
_tmp244;struct Cyc_Toc_Env*_tmp242=nv;_tmp243=*_tmp242;_tmp244=_tmp243.varmap;
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(
_tmp244,x);}static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){int*
_tmpB42;struct Cyc_Toc_Env*_tmpB41;return(_tmpB41=_region_malloc(r,sizeof(*
_tmpB41)),((_tmpB41->break_lab=(struct _dyneither_ptr**)0,((_tmpB41->continue_lab=(
struct _dyneither_ptr**)0,((_tmpB41->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((
_tmpB41->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((
_tmpB41->toplevel=(int)1,((_tmpB41->in_lhs=(int*)((_tmpB42=_region_malloc(r,
sizeof(*_tmpB42)),((_tmpB42[0]=0,_tmpB42)))),((_tmpB41->rgn=(struct _RegionHandle*)
r,_tmpB41)))))))))))))));}static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp248;struct
_dyneither_ptr**_tmp249;struct _dyneither_ptr**_tmp24A;struct Cyc_Toc_FallthruInfo*
_tmp24B;struct Cyc_Dict_Dict _tmp24C;int _tmp24D;int*_tmp24E;struct Cyc_Toc_Env*
_tmp247=e;_tmp248=*_tmp247;_tmp249=_tmp248.break_lab;_tmp24A=_tmp248.continue_lab;
_tmp24B=_tmp248.fallthru_info;_tmp24C=_tmp248.varmap;_tmp24D=_tmp248.toplevel;
_tmp24E=_tmp248.in_lhs;{struct Cyc_Toc_Env*_tmpB43;return(_tmpB43=_region_malloc(
r,sizeof(*_tmpB43)),((_tmpB43->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp249),((_tmpB43->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp24A),((_tmpB43->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp24B,((_tmpB43->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp24C),((_tmpB43->toplevel=(
int)_tmp24D,((_tmpB43->in_lhs=(int*)_tmp24E,((_tmpB43->rgn=(struct _RegionHandle*)
r,_tmpB43)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp251;struct
_dyneither_ptr**_tmp252;struct _dyneither_ptr**_tmp253;struct Cyc_Toc_FallthruInfo*
_tmp254;struct Cyc_Dict_Dict _tmp255;int _tmp256;int*_tmp257;struct Cyc_Toc_Env*
_tmp250=e;_tmp251=*_tmp250;_tmp252=_tmp251.break_lab;_tmp253=_tmp251.continue_lab;
_tmp254=_tmp251.fallthru_info;_tmp255=_tmp251.varmap;_tmp256=_tmp251.toplevel;
_tmp257=_tmp251.in_lhs;{struct Cyc_Toc_Env*_tmpB44;return(_tmpB44=_region_malloc(
r,sizeof(*_tmpB44)),((_tmpB44->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp252),((_tmpB44->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp253),((_tmpB44->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp254,((_tmpB44->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp255),((_tmpB44->toplevel=(
int)0,((_tmpB44->in_lhs=(int*)_tmp257,((_tmpB44->rgn=(struct _RegionHandle*)r,
_tmpB44)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct
_RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp25A;struct
_dyneither_ptr**_tmp25B;struct _dyneither_ptr**_tmp25C;struct Cyc_Toc_FallthruInfo*
_tmp25D;struct Cyc_Dict_Dict _tmp25E;int _tmp25F;int*_tmp260;struct Cyc_Toc_Env*
_tmp259=e;_tmp25A=*_tmp259;_tmp25B=_tmp25A.break_lab;_tmp25C=_tmp25A.continue_lab;
_tmp25D=_tmp25A.fallthru_info;_tmp25E=_tmp25A.varmap;_tmp25F=_tmp25A.toplevel;
_tmp260=_tmp25A.in_lhs;{struct Cyc_Toc_Env*_tmpB45;return(_tmpB45=_region_malloc(
r,sizeof(*_tmpB45)),((_tmpB45->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp25B),((_tmpB45->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp25C),((_tmpB45->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp25D,((_tmpB45->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp25E),((_tmpB45->toplevel=(
int)1,((_tmpB45->in_lhs=(int*)_tmp260,((_tmpB45->rgn=(struct _RegionHandle*)r,
_tmpB45)))))))))))))));};}static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env _tmp263;int*_tmp264;struct Cyc_Toc_Env*_tmp262=e;_tmp263=*
_tmp262;_tmp264=_tmp263.in_lhs;*_tmp264=b;}static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace _tmp265=(*x).f1;struct Cyc_List_List*_tmp266;_LLEC: if((
_tmp265.Rel_n).tag != 1)goto _LLEE;_tmp266=(struct Cyc_List_List*)(_tmp265.Rel_n).val;
_LLED: {const char*_tmpB49;void*_tmpB48[1];struct Cyc_String_pa_struct _tmpB47;(
_tmpB47.tag=0,((_tmpB47.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
x)),((_tmpB48[0]=& _tmpB47,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB49="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(
_tmpB49,sizeof(char),30))),_tag_dyneither(_tmpB48,sizeof(void*),1)))))));}_LLEE:;
_LLEF: goto _LLEB;_LLEB:;}{struct Cyc_Toc_Env _tmp26B;struct _dyneither_ptr**_tmp26C;
struct _dyneither_ptr**_tmp26D;struct Cyc_Toc_FallthruInfo*_tmp26E;struct Cyc_Dict_Dict
_tmp26F;int _tmp270;int*_tmp271;struct Cyc_Toc_Env*_tmp26A=e;_tmp26B=*_tmp26A;
_tmp26C=_tmp26B.break_lab;_tmp26D=_tmp26B.continue_lab;_tmp26E=_tmp26B.fallthru_info;
_tmp26F=_tmp26B.varmap;_tmp270=_tmp26B.toplevel;_tmp271=_tmp26B.in_lhs;{struct
Cyc_Dict_Dict _tmp272=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*
k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp26F),x,y);struct Cyc_Toc_Env*
_tmpB4A;return(_tmpB4A=_region_malloc(r,sizeof(*_tmpB4A)),((_tmpB4A->break_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp26C),((_tmpB4A->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp26D),((_tmpB4A->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp26E,((_tmpB4A->varmap=(struct Cyc_Dict_Dict)
_tmp272,((_tmpB4A->toplevel=(int)_tmp270,((_tmpB4A->in_lhs=(int*)_tmp271,((
_tmpB4A->rgn=(struct _RegionHandle*)r,_tmpB4A)))))))))))))));};};}static struct Cyc_Toc_Env*
Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp275;struct _dyneither_ptr**_tmp276;struct _dyneither_ptr**_tmp277;struct Cyc_Toc_FallthruInfo*
_tmp278;struct Cyc_Dict_Dict _tmp279;int _tmp27A;int*_tmp27B;struct Cyc_Toc_Env*
_tmp274=e;_tmp275=*_tmp274;_tmp276=_tmp275.break_lab;_tmp277=_tmp275.continue_lab;
_tmp278=_tmp275.fallthru_info;_tmp279=_tmp275.varmap;_tmp27A=_tmp275.toplevel;
_tmp27B=_tmp275.in_lhs;{struct Cyc_Toc_Env*_tmpB4B;return(_tmpB4B=_region_malloc(
r,sizeof(*_tmpB4B)),((_tmpB4B->break_lab=(struct _dyneither_ptr**)0,((_tmpB4B->continue_lab=(
struct _dyneither_ptr**)0,((_tmpB4B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp278,((_tmpB4B->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp279),((_tmpB4B->toplevel=(
int)_tmp27A,((_tmpB4B->in_lhs=(int*)_tmp27B,((_tmpB4B->rgn=(struct _RegionHandle*)
r,_tmpB4B)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmpB4C;fallthru_vars=((
_tmpB4C=_region_malloc(r,sizeof(*_tmpB4C)),((_tmpB4C->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmpB4C->tl=fallthru_vars,_tmpB4C))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp27F;struct _dyneither_ptr**_tmp280;struct
_dyneither_ptr**_tmp281;struct Cyc_Toc_FallthruInfo*_tmp282;struct Cyc_Dict_Dict
_tmp283;int _tmp284;int*_tmp285;struct Cyc_Toc_Env*_tmp27E=e;_tmp27F=*_tmp27E;
_tmp280=_tmp27F.break_lab;_tmp281=_tmp27F.continue_lab;_tmp282=_tmp27F.fallthru_info;
_tmp283=_tmp27F.varmap;_tmp284=_tmp27F.toplevel;_tmp285=_tmp27F.in_lhs;{struct
Cyc_Toc_Env _tmp287;struct Cyc_Dict_Dict _tmp288;struct Cyc_Toc_Env*_tmp286=
next_case_env;_tmp287=*_tmp286;_tmp288=_tmp287.varmap;{struct Cyc_Toc_FallthruInfo*
_tmpB4D;struct Cyc_Toc_FallthruInfo*fi=(_tmpB4D=_region_malloc(r,sizeof(*_tmpB4D)),((
_tmpB4D->label=fallthru_l,((_tmpB4D->binders=fallthru_vars,((_tmpB4D->next_case_env=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp288),_tmpB4D)))))));struct _dyneither_ptr**_tmpB50;struct Cyc_Toc_Env*
_tmpB4F;return(_tmpB4F=_region_malloc(r,sizeof(*_tmpB4F)),((_tmpB4F->break_lab=(
struct _dyneither_ptr**)((_tmpB50=_region_malloc(r,sizeof(*_tmpB50)),((_tmpB50[0]=
break_l,_tmpB50)))),((_tmpB4F->continue_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp281),((_tmpB4F->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
fi,((_tmpB4F->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp283),((_tmpB4F->toplevel=(
int)_tmp284,((_tmpB4F->in_lhs=(int*)((int*)_tmp285),((_tmpB4F->rgn=(struct
_RegionHandle*)r,_tmpB4F)))))))))))))));};};};}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){struct
Cyc_Toc_Env _tmp28D;struct _dyneither_ptr**_tmp28E;struct _dyneither_ptr**_tmp28F;
struct Cyc_Toc_FallthruInfo*_tmp290;struct Cyc_Dict_Dict _tmp291;int _tmp292;int*
_tmp293;struct Cyc_Toc_Env*_tmp28C=e;_tmp28D=*_tmp28C;_tmp28E=_tmp28D.break_lab;
_tmp28F=_tmp28D.continue_lab;_tmp290=_tmp28D.fallthru_info;_tmp291=_tmp28D.varmap;
_tmp292=_tmp28D.toplevel;_tmp293=_tmp28D.in_lhs;{struct _dyneither_ptr**_tmpB53;
struct Cyc_Toc_Env*_tmpB52;return(_tmpB52=_region_malloc(r,sizeof(*_tmpB52)),((
_tmpB52->break_lab=(struct _dyneither_ptr**)((_tmpB53=_region_malloc(r,sizeof(*
_tmpB53)),((_tmpB53[0]=break_l,_tmpB53)))),((_tmpB52->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp28F),((_tmpB52->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpB52->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp291),((_tmpB52->toplevel=(
int)_tmp292,((_tmpB52->in_lhs=(int*)_tmp293,((_tmpB52->rgn=(struct _RegionHandle*)
r,_tmpB52)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp297;struct _dyneither_ptr**_tmp298;struct _dyneither_ptr**_tmp299;struct Cyc_Toc_FallthruInfo*
_tmp29A;struct Cyc_Dict_Dict _tmp29B;int _tmp29C;int*_tmp29D;struct Cyc_Toc_Env*
_tmp296=e;_tmp297=*_tmp296;_tmp298=_tmp297.break_lab;_tmp299=_tmp297.continue_lab;
_tmp29A=_tmp297.fallthru_info;_tmp29B=_tmp297.varmap;_tmp29C=_tmp297.toplevel;
_tmp29D=_tmp297.in_lhs;{struct Cyc_Toc_FallthruInfo*_tmpB56;struct Cyc_Toc_Env*
_tmpB55;return(_tmpB55=_region_malloc(r,sizeof(*_tmpB55)),((_tmpB55->break_lab=(
struct _dyneither_ptr**)0,((_tmpB55->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp299),((_tmpB55->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((
_tmpB56=_region_malloc(r,sizeof(*_tmpB56)),((_tmpB56->label=next_l,((_tmpB56->binders=
0,((_tmpB56->next_case_env=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*
cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),
_tmpB56)))))))),((_tmpB55->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29B),((_tmpB55->toplevel=(
int)_tmp29C,((_tmpB55->in_lhs=(int*)((int*)_tmp29D),((_tmpB55->rgn=(struct
_RegionHandle*)r,_tmpB55)))))))))))))));};}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp2A0=e->annot;_LLF1: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2A1=(
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp2A0;if(_tmp2A1->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLF3;}_LLF2: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLF3: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp2A2=(
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp2A0;if(_tmp2A2->tag != Cyc_CfFlowInfo_NotZero)
goto _LLF5;}_LLF4: return 0;_LLF5: {struct Cyc_CfFlowInfo_IsZero_struct*_tmp2A3=(
struct Cyc_CfFlowInfo_IsZero_struct*)_tmp2A0;if(_tmp2A3->tag != Cyc_CfFlowInfo_IsZero)
goto _LLF7;}_LLF6:{const char*_tmpB59;void*_tmpB58;(_tmpB58=0,Cyc_Tcutil_terr(e->loc,((
_tmpB59="dereference of NULL pointer",_tag_dyneither(_tmpB59,sizeof(char),28))),
_tag_dyneither(_tmpB58,sizeof(void*),0)));}return 0;_LLF7: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp2A4=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp2A0;if(_tmp2A4->tag != Cyc_Absyn_EmptyAnnot)
goto _LLF9;}_LLF8: return 0;_LLF9:;_LLFA: {const char*_tmpB5C;void*_tmpB5B;(_tmpB5B=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB5C="need_null_check",_tag_dyneither(_tmpB5C,sizeof(char),16))),
_tag_dyneither(_tmpB5B,sizeof(void*),0)));}_LLF0:;}static struct Cyc_List_List*Cyc_Toc_get_relns(
struct Cyc_Absyn_Exp*e){void*_tmp2A9=e->annot;struct Cyc_List_List*_tmp2AB;struct
Cyc_List_List*_tmp2AD;_LLFC: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2AA=(
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp2A9;if(_tmp2AA->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLFE;else{_tmp2AB=_tmp2AA->f1;}}_LLFD: return _tmp2AB;_LLFE: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmp2AC=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmp2A9;if(_tmp2AC->tag != Cyc_CfFlowInfo_NotZero)
goto _LL100;else{_tmp2AD=_tmp2AC->f1;}}_LLFF: return _tmp2AD;_LL100: {struct Cyc_CfFlowInfo_IsZero_struct*
_tmp2AE=(struct Cyc_CfFlowInfo_IsZero_struct*)_tmp2A9;if(_tmp2AE->tag != Cyc_CfFlowInfo_IsZero)
goto _LL102;}_LL101:{const char*_tmpB5F;void*_tmpB5E;(_tmpB5E=0,Cyc_Tcutil_terr(e->loc,((
_tmpB5F="dereference of NULL pointer",_tag_dyneither(_tmpB5F,sizeof(char),28))),
_tag_dyneither(_tmpB5E,sizeof(void*),0)));}return 0;_LL102: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp2AF=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp2A9;if(_tmp2AF->tag != Cyc_Absyn_EmptyAnnot)
goto _LL104;}_LL103: return 0;_LL104:;_LL105: {const char*_tmpB62;void*_tmpB61;(
_tmpB61=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB62="get_relns",_tag_dyneither(_tmpB62,sizeof(char),10))),_tag_dyneither(
_tmpB61,sizeof(void*),0)));}_LLFB:;}static int Cyc_Toc_check_const_array(
unsigned int i,void*t){void*_tmp2B4=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp2B6;struct Cyc_Absyn_PtrAtts _tmp2B7;union Cyc_Absyn_Constraint*_tmp2B8;union
Cyc_Absyn_Constraint*_tmp2B9;struct Cyc_Absyn_ArrayInfo _tmp2BB;struct Cyc_Absyn_Exp*
_tmp2BC;_LL107: {struct Cyc_Absyn_PointerType_struct*_tmp2B5=(struct Cyc_Absyn_PointerType_struct*)
_tmp2B4;if(_tmp2B5->tag != 5)goto _LL109;else{_tmp2B6=_tmp2B5->f1;_tmp2B7=_tmp2B6.ptr_atts;
_tmp2B8=_tmp2B7.bounds;_tmp2B9=_tmp2B7.zero_term;}}_LL108: {void*_tmp2BD=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp2B8);struct Cyc_Absyn_Exp*_tmp2C0;_LL10E: {struct Cyc_Absyn_DynEither_b_struct*
_tmp2BE=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2BD;if(_tmp2BE->tag != 0)goto
_LL110;}_LL10F: return 0;_LL110: {struct Cyc_Absyn_Upper_b_struct*_tmp2BF=(struct
Cyc_Absyn_Upper_b_struct*)_tmp2BD;if(_tmp2BF->tag != 1)goto _LL10D;else{_tmp2C0=
_tmp2BF->f1;}}_LL111: {unsigned int _tmp2C2;int _tmp2C3;struct _tuple10 _tmp2C1=Cyc_Evexp_eval_const_uint_exp(
_tmp2C0);_tmp2C2=_tmp2C1.f1;_tmp2C3=_tmp2C1.f2;return _tmp2C3  && i <= _tmp2C2;}
_LL10D:;}_LL109: {struct Cyc_Absyn_ArrayType_struct*_tmp2BA=(struct Cyc_Absyn_ArrayType_struct*)
_tmp2B4;if(_tmp2BA->tag != 9)goto _LL10B;else{_tmp2BB=_tmp2BA->f1;_tmp2BC=_tmp2BB.num_elts;}}
_LL10A: if(_tmp2BC == 0)return 0;{unsigned int _tmp2C5;int _tmp2C6;struct _tuple10
_tmp2C4=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp2BC);_tmp2C5=
_tmp2C4.f1;_tmp2C6=_tmp2C4.f2;return _tmp2C6  && i <= _tmp2C5;};_LL10B:;_LL10C:
return 0;_LL106:;}static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2C7=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(
_tmp2C7->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp _tmp2C8=_tmp2C7->rop;struct
Cyc_Absyn_Vardecl*_tmp2C9;struct Cyc_Absyn_Vardecl*_tmp2CA;_LL113: if((_tmp2C8.LessNumelts).tag
!= 3)goto _LL115;_tmp2C9=(struct Cyc_Absyn_Vardecl*)(_tmp2C8.LessNumelts).val;
_LL114: _tmp2CA=_tmp2C9;goto _LL116;_LL115: if((_tmp2C8.LessEqNumelts).tag != 5)goto
_LL117;_tmp2CA=(struct Cyc_Absyn_Vardecl*)(_tmp2C8.LessEqNumelts).val;_LL116: if(
_tmp2CA == v)return 1;else{goto _LL112;}_LL117:;_LL118: continue;_LL112:;};}return 0;}
static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Exp*e,void*vtype){{void*_tmp2CB=e->r;void*_tmp2CD;struct Cyc_Absyn_Vardecl*
_tmp2CF;void*_tmp2D1;struct Cyc_Absyn_Vardecl*_tmp2D3;void*_tmp2D5;struct Cyc_Absyn_Vardecl*
_tmp2D7;void*_tmp2D9;struct Cyc_Absyn_Vardecl*_tmp2DB;union Cyc_Absyn_Cnst _tmp2DD;
struct _tuple5 _tmp2DE;enum Cyc_Absyn_Sign _tmp2DF;int _tmp2E0;union Cyc_Absyn_Cnst
_tmp2E2;struct _tuple5 _tmp2E3;enum Cyc_Absyn_Sign _tmp2E4;int _tmp2E5;union Cyc_Absyn_Cnst
_tmp2E7;struct _tuple5 _tmp2E8;enum Cyc_Absyn_Sign _tmp2E9;int _tmp2EA;enum Cyc_Absyn_Primop
_tmp2EC;struct Cyc_List_List*_tmp2ED;struct Cyc_List_List _tmp2EE;struct Cyc_Absyn_Exp*
_tmp2EF;_LL11A: {struct Cyc_Absyn_Var_e_struct*_tmp2CC=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CB;if(_tmp2CC->tag != 1)goto _LL11C;else{_tmp2CD=(void*)_tmp2CC->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2CE=(struct Cyc_Absyn_Pat_b_struct*)_tmp2CD;if(_tmp2CE->tag
!= 5)goto _LL11C;else{_tmp2CF=_tmp2CE->f1;}};}}_LL11B: _tmp2D3=_tmp2CF;goto _LL11D;
_LL11C: {struct Cyc_Absyn_Var_e_struct*_tmp2D0=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CB;if(_tmp2D0->tag != 1)goto _LL11E;else{_tmp2D1=(void*)_tmp2D0->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2D2=(struct Cyc_Absyn_Local_b_struct*)_tmp2D1;if(
_tmp2D2->tag != 4)goto _LL11E;else{_tmp2D3=_tmp2D2->f1;}};}}_LL11D: _tmp2D7=_tmp2D3;
goto _LL11F;_LL11E: {struct Cyc_Absyn_Var_e_struct*_tmp2D4=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CB;if(_tmp2D4->tag != 1)goto _LL120;else{_tmp2D5=(void*)_tmp2D4->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2D6=(struct Cyc_Absyn_Global_b_struct*)_tmp2D5;if(
_tmp2D6->tag != 1)goto _LL120;else{_tmp2D7=_tmp2D6->f1;}};}}_LL11F: _tmp2DB=_tmp2D7;
goto _LL121;_LL120: {struct Cyc_Absyn_Var_e_struct*_tmp2D8=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CB;if(_tmp2D8->tag != 1)goto _LL122;else{_tmp2D9=(void*)_tmp2D8->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp2DA=(struct Cyc_Absyn_Param_b_struct*)_tmp2D9;if(
_tmp2DA->tag != 3)goto _LL122;else{_tmp2DB=_tmp2DA->f1;}};}}_LL121: if(_tmp2DB->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp2DB))return 1;goto _LL119;_LL122: {
struct Cyc_Absyn_Const_e_struct*_tmp2DC=(struct Cyc_Absyn_Const_e_struct*)_tmp2CB;
if(_tmp2DC->tag != 0)goto _LL124;else{_tmp2DD=_tmp2DC->f1;if((_tmp2DD.Int_c).tag != 
4)goto _LL124;_tmp2DE=(struct _tuple5)(_tmp2DD.Int_c).val;_tmp2DF=_tmp2DE.f1;if(
_tmp2DF != Cyc_Absyn_None)goto _LL124;_tmp2E0=_tmp2DE.f2;}}_LL123: _tmp2E5=_tmp2E0;
goto _LL125;_LL124: {struct Cyc_Absyn_Const_e_struct*_tmp2E1=(struct Cyc_Absyn_Const_e_struct*)
_tmp2CB;if(_tmp2E1->tag != 0)goto _LL126;else{_tmp2E2=_tmp2E1->f1;if((_tmp2E2.Int_c).tag
!= 4)goto _LL126;_tmp2E3=(struct _tuple5)(_tmp2E2.Int_c).val;_tmp2E4=_tmp2E3.f1;
if(_tmp2E4 != Cyc_Absyn_Signed)goto _LL126;_tmp2E5=_tmp2E3.f2;}}_LL125: return
_tmp2E5 >= 0  && Cyc_Toc_check_const_array((unsigned int)_tmp2E5,vtype);_LL126: {
struct Cyc_Absyn_Const_e_struct*_tmp2E6=(struct Cyc_Absyn_Const_e_struct*)_tmp2CB;
if(_tmp2E6->tag != 0)goto _LL128;else{_tmp2E7=_tmp2E6->f1;if((_tmp2E7.Int_c).tag != 
4)goto _LL128;_tmp2E8=(struct _tuple5)(_tmp2E7.Int_c).val;_tmp2E9=_tmp2E8.f1;if(
_tmp2E9 != Cyc_Absyn_Unsigned)goto _LL128;_tmp2EA=_tmp2E8.f2;}}_LL127: return Cyc_Toc_check_const_array((
unsigned int)_tmp2EA,vtype);_LL128: {struct Cyc_Absyn_Primop_e_struct*_tmp2EB=(
struct Cyc_Absyn_Primop_e_struct*)_tmp2CB;if(_tmp2EB->tag != 3)goto _LL12A;else{
_tmp2EC=_tmp2EB->f1;if(_tmp2EC != Cyc_Absyn_Numelts)goto _LL12A;_tmp2ED=_tmp2EB->f2;
if(_tmp2ED == 0)goto _LL12A;_tmp2EE=*_tmp2ED;_tmp2EF=(struct Cyc_Absyn_Exp*)_tmp2EE.hd;}}
_LL129:{void*_tmp2F0=_tmp2EF->r;void*_tmp2F2;struct Cyc_Absyn_Vardecl*_tmp2F4;
void*_tmp2F6;struct Cyc_Absyn_Vardecl*_tmp2F8;void*_tmp2FA;struct Cyc_Absyn_Vardecl*
_tmp2FC;void*_tmp2FE;struct Cyc_Absyn_Vardecl*_tmp300;_LL12D: {struct Cyc_Absyn_Var_e_struct*
_tmp2F1=(struct Cyc_Absyn_Var_e_struct*)_tmp2F0;if(_tmp2F1->tag != 1)goto _LL12F;
else{_tmp2F2=(void*)_tmp2F1->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp2F3=(struct Cyc_Absyn_Pat_b_struct*)
_tmp2F2;if(_tmp2F3->tag != 5)goto _LL12F;else{_tmp2F4=_tmp2F3->f1;}};}}_LL12E:
_tmp2F8=_tmp2F4;goto _LL130;_LL12F: {struct Cyc_Absyn_Var_e_struct*_tmp2F5=(struct
Cyc_Absyn_Var_e_struct*)_tmp2F0;if(_tmp2F5->tag != 1)goto _LL131;else{_tmp2F6=(
void*)_tmp2F5->f2;{struct Cyc_Absyn_Local_b_struct*_tmp2F7=(struct Cyc_Absyn_Local_b_struct*)
_tmp2F6;if(_tmp2F7->tag != 4)goto _LL131;else{_tmp2F8=_tmp2F7->f1;}};}}_LL130:
_tmp2FC=_tmp2F8;goto _LL132;_LL131: {struct Cyc_Absyn_Var_e_struct*_tmp2F9=(struct
Cyc_Absyn_Var_e_struct*)_tmp2F0;if(_tmp2F9->tag != 1)goto _LL133;else{_tmp2FA=(
void*)_tmp2F9->f2;{struct Cyc_Absyn_Global_b_struct*_tmp2FB=(struct Cyc_Absyn_Global_b_struct*)
_tmp2FA;if(_tmp2FB->tag != 1)goto _LL133;else{_tmp2FC=_tmp2FB->f1;}};}}_LL132:
_tmp300=_tmp2FC;goto _LL134;_LL133: {struct Cyc_Absyn_Var_e_struct*_tmp2FD=(struct
Cyc_Absyn_Var_e_struct*)_tmp2F0;if(_tmp2FD->tag != 1)goto _LL135;else{_tmp2FE=(
void*)_tmp2FD->f2;{struct Cyc_Absyn_Param_b_struct*_tmp2FF=(struct Cyc_Absyn_Param_b_struct*)
_tmp2FE;if(_tmp2FF->tag != 3)goto _LL135;else{_tmp300=_tmp2FF->f1;}};}}_LL134:
return _tmp300 == v;_LL135:;_LL136: goto _LL12C;_LL12C:;}goto _LL119;_LL12A:;_LL12B:
goto _LL119;_LL119:;}return 0;}static char _tmp312[8]="*bogus*";struct _tuple17{void*
f1;void*f2;};static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,
struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){struct Cyc_Absyn_Vardecl*x;{void*
_tmp301=a->r;void*_tmp303;struct Cyc_Absyn_Vardecl*_tmp305;void*_tmp307;struct Cyc_Absyn_Vardecl*
_tmp309;void*_tmp30B;struct Cyc_Absyn_Vardecl*_tmp30D;void*_tmp30F;struct Cyc_Absyn_Vardecl*
_tmp311;_LL138: {struct Cyc_Absyn_Var_e_struct*_tmp302=(struct Cyc_Absyn_Var_e_struct*)
_tmp301;if(_tmp302->tag != 1)goto _LL13A;else{_tmp303=(void*)_tmp302->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp304=(struct Cyc_Absyn_Pat_b_struct*)_tmp303;if(_tmp304->tag
!= 5)goto _LL13A;else{_tmp305=_tmp304->f1;}};}}_LL139: _tmp309=_tmp305;goto _LL13B;
_LL13A: {struct Cyc_Absyn_Var_e_struct*_tmp306=(struct Cyc_Absyn_Var_e_struct*)
_tmp301;if(_tmp306->tag != 1)goto _LL13C;else{_tmp307=(void*)_tmp306->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp308=(struct Cyc_Absyn_Local_b_struct*)_tmp307;if(
_tmp308->tag != 4)goto _LL13C;else{_tmp309=_tmp308->f1;}};}}_LL13B: _tmp30D=_tmp309;
goto _LL13D;_LL13C: {struct Cyc_Absyn_Var_e_struct*_tmp30A=(struct Cyc_Absyn_Var_e_struct*)
_tmp301;if(_tmp30A->tag != 1)goto _LL13E;else{_tmp30B=(void*)_tmp30A->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp30C=(struct Cyc_Absyn_Global_b_struct*)_tmp30B;if(
_tmp30C->tag != 1)goto _LL13E;else{_tmp30D=_tmp30C->f1;}};}}_LL13D: _tmp311=_tmp30D;
goto _LL13F;_LL13E: {struct Cyc_Absyn_Var_e_struct*_tmp30E=(struct Cyc_Absyn_Var_e_struct*)
_tmp301;if(_tmp30E->tag != 1)goto _LL140;else{_tmp30F=(void*)_tmp30E->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp310=(struct Cyc_Absyn_Param_b_struct*)_tmp30F;if(
_tmp310->tag != 3)goto _LL140;else{_tmp311=_tmp310->f1;}};}}_LL13F: if(_tmp311->escapes)
goto _LL141;x=_tmp311;goto _LL137;_LL140:;_LL141: {static struct _dyneither_ptr
bogus_string={_tmp312,_tmp312,_tmp312 + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
4,0}},& bogus_string};static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,&
bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};x=& bogus_vardecl;
x->type=a_typ;}_LL137:;}{void*_tmp313=a_typ;inner_loop: {void*_tmp314=i->r;void*
_tmp316;struct Cyc_Absyn_Exp*_tmp317;union Cyc_Absyn_Cnst _tmp319;struct _tuple5
_tmp31A;enum Cyc_Absyn_Sign _tmp31B;int _tmp31C;union Cyc_Absyn_Cnst _tmp31E;struct
_tuple5 _tmp31F;enum Cyc_Absyn_Sign _tmp320;int _tmp321;union Cyc_Absyn_Cnst _tmp323;
struct _tuple5 _tmp324;enum Cyc_Absyn_Sign _tmp325;int _tmp326;enum Cyc_Absyn_Primop
_tmp328;struct Cyc_List_List*_tmp329;struct Cyc_List_List _tmp32A;struct Cyc_Absyn_Exp*
_tmp32B;struct Cyc_List_List*_tmp32C;struct Cyc_List_List _tmp32D;struct Cyc_Absyn_Exp*
_tmp32E;void*_tmp330;struct Cyc_Absyn_Vardecl*_tmp332;void*_tmp334;struct Cyc_Absyn_Vardecl*
_tmp336;void*_tmp338;struct Cyc_Absyn_Vardecl*_tmp33A;void*_tmp33C;struct Cyc_Absyn_Vardecl*
_tmp33E;_LL143: {struct Cyc_Absyn_Cast_e_struct*_tmp315=(struct Cyc_Absyn_Cast_e_struct*)
_tmp314;if(_tmp315->tag != 15)goto _LL145;else{_tmp316=(void*)_tmp315->f1;_tmp317=
_tmp315->f2;}}_LL144: i=_tmp317;goto inner_loop;_LL145: {struct Cyc_Absyn_Const_e_struct*
_tmp318=(struct Cyc_Absyn_Const_e_struct*)_tmp314;if(_tmp318->tag != 0)goto _LL147;
else{_tmp319=_tmp318->f1;if((_tmp319.Int_c).tag != 4)goto _LL147;_tmp31A=(struct
_tuple5)(_tmp319.Int_c).val;_tmp31B=_tmp31A.f1;if(_tmp31B != Cyc_Absyn_None)goto
_LL147;_tmp31C=_tmp31A.f2;}}_LL146: _tmp321=_tmp31C;goto _LL148;_LL147: {struct Cyc_Absyn_Const_e_struct*
_tmp31D=(struct Cyc_Absyn_Const_e_struct*)_tmp314;if(_tmp31D->tag != 0)goto _LL149;
else{_tmp31E=_tmp31D->f1;if((_tmp31E.Int_c).tag != 4)goto _LL149;_tmp31F=(struct
_tuple5)(_tmp31E.Int_c).val;_tmp320=_tmp31F.f1;if(_tmp320 != Cyc_Absyn_Signed)
goto _LL149;_tmp321=_tmp31F.f2;}}_LL148: return _tmp321 >= 0  && Cyc_Toc_check_const_array((
unsigned int)(_tmp321 + 1),_tmp313);_LL149: {struct Cyc_Absyn_Const_e_struct*
_tmp322=(struct Cyc_Absyn_Const_e_struct*)_tmp314;if(_tmp322->tag != 0)goto _LL14B;
else{_tmp323=_tmp322->f1;if((_tmp323.Int_c).tag != 4)goto _LL14B;_tmp324=(struct
_tuple5)(_tmp323.Int_c).val;_tmp325=_tmp324.f1;if(_tmp325 != Cyc_Absyn_Unsigned)
goto _LL14B;_tmp326=_tmp324.f2;}}_LL14A: return Cyc_Toc_check_const_array((
unsigned int)(_tmp326 + 1),_tmp313);_LL14B: {struct Cyc_Absyn_Primop_e_struct*
_tmp327=(struct Cyc_Absyn_Primop_e_struct*)_tmp314;if(_tmp327->tag != 3)goto _LL14D;
else{_tmp328=_tmp327->f1;if(_tmp328 != Cyc_Absyn_Mod)goto _LL14D;_tmp329=_tmp327->f2;
if(_tmp329 == 0)goto _LL14D;_tmp32A=*_tmp329;_tmp32B=(struct Cyc_Absyn_Exp*)_tmp32A.hd;
_tmp32C=_tmp32A.tl;if(_tmp32C == 0)goto _LL14D;_tmp32D=*_tmp32C;_tmp32E=(struct Cyc_Absyn_Exp*)
_tmp32D.hd;}}_LL14C: return Cyc_Toc_check_leq_size(relns,x,_tmp32E,_tmp313);_LL14D: {
struct Cyc_Absyn_Var_e_struct*_tmp32F=(struct Cyc_Absyn_Var_e_struct*)_tmp314;if(
_tmp32F->tag != 1)goto _LL14F;else{_tmp330=(void*)_tmp32F->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp331=(struct Cyc_Absyn_Pat_b_struct*)_tmp330;if(_tmp331->tag != 5)goto _LL14F;
else{_tmp332=_tmp331->f1;}};}}_LL14E: _tmp336=_tmp332;goto _LL150;_LL14F: {struct
Cyc_Absyn_Var_e_struct*_tmp333=(struct Cyc_Absyn_Var_e_struct*)_tmp314;if(_tmp333->tag
!= 1)goto _LL151;else{_tmp334=(void*)_tmp333->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp335=(struct Cyc_Absyn_Local_b_struct*)_tmp334;if(_tmp335->tag != 4)goto _LL151;
else{_tmp336=_tmp335->f1;}};}}_LL150: _tmp33A=_tmp336;goto _LL152;_LL151: {struct
Cyc_Absyn_Var_e_struct*_tmp337=(struct Cyc_Absyn_Var_e_struct*)_tmp314;if(_tmp337->tag
!= 1)goto _LL153;else{_tmp338=(void*)_tmp337->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp339=(struct Cyc_Absyn_Global_b_struct*)_tmp338;if(_tmp339->tag != 1)goto _LL153;
else{_tmp33A=_tmp339->f1;}};}}_LL152: _tmp33E=_tmp33A;goto _LL154;_LL153: {struct
Cyc_Absyn_Var_e_struct*_tmp33B=(struct Cyc_Absyn_Var_e_struct*)_tmp314;if(_tmp33B->tag
!= 1)goto _LL155;else{_tmp33C=(void*)_tmp33B->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp33D=(struct Cyc_Absyn_Param_b_struct*)_tmp33C;if(_tmp33D->tag != 3)goto _LL155;
else{_tmp33E=_tmp33D->f1;}};}}_LL154: if(_tmp33E->escapes)return 0;{struct Cyc_List_List*
_tmp33F=relns;for(0;_tmp33F != 0;_tmp33F=_tmp33F->tl){struct Cyc_CfFlowInfo_Reln*
_tmp340=(struct Cyc_CfFlowInfo_Reln*)_tmp33F->hd;if(_tmp340->vd == _tmp33E){union
Cyc_CfFlowInfo_RelnOp _tmp341=_tmp340->rop;struct Cyc_Absyn_Vardecl*_tmp342;struct
_tuple11 _tmp343;struct Cyc_Absyn_Vardecl*_tmp344;void*_tmp345;unsigned int _tmp346;
unsigned int _tmp347;_LL158: if((_tmp341.LessNumelts).tag != 3)goto _LL15A;_tmp342=(
struct Cyc_Absyn_Vardecl*)(_tmp341.LessNumelts).val;_LL159: if(x == _tmp342)return 1;
else{goto _LL157;}_LL15A: if((_tmp341.LessVar).tag != 2)goto _LL15C;_tmp343=(struct
_tuple11)(_tmp341.LessVar).val;_tmp344=_tmp343.f1;_tmp345=_tmp343.f2;_LL15B:{
struct _tuple17 _tmpB63;struct _tuple17 _tmp349=(_tmpB63.f1=Cyc_Tcutil_compress(
_tmp345),((_tmpB63.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(a->topt))->v),_tmpB63)));void*_tmp34A;void*_tmp34C;void*_tmp34D;
struct Cyc_Absyn_PtrInfo _tmp34F;struct Cyc_Absyn_PtrAtts _tmp350;union Cyc_Absyn_Constraint*
_tmp351;_LL163: _tmp34A=_tmp349.f1;{struct Cyc_Absyn_TagType_struct*_tmp34B=(
struct Cyc_Absyn_TagType_struct*)_tmp34A;if(_tmp34B->tag != 20)goto _LL165;else{
_tmp34C=(void*)_tmp34B->f1;}};_tmp34D=_tmp349.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp34E=(struct Cyc_Absyn_PointerType_struct*)_tmp34D;if(_tmp34E->tag != 5)goto
_LL165;else{_tmp34F=_tmp34E->f1;_tmp350=_tmp34F.ptr_atts;_tmp351=_tmp350.bounds;}};
_LL164:{void*_tmp352=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp351);struct Cyc_Absyn_Exp*_tmp354;_LL168: {struct Cyc_Absyn_Upper_b_struct*
_tmp353=(struct Cyc_Absyn_Upper_b_struct*)_tmp352;if(_tmp353->tag != 1)goto _LL16A;
else{_tmp354=_tmp353->f1;}}_LL169: {struct Cyc_Absyn_Exp*_tmp355=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp34C,0),0,Cyc_Absyn_No_coercion,0);
if(Cyc_Evexp_lte_const_exp(_tmp355,_tmp354))return 1;goto _LL167;}_LL16A:;_LL16B:
goto _LL167;_LL167:;}goto _LL162;_LL165:;_LL166: goto _LL162;_LL162:;}{struct Cyc_List_List*
_tmp356=relns;for(0;_tmp356 != 0;_tmp356=_tmp356->tl){struct Cyc_CfFlowInfo_Reln*
_tmp357=(struct Cyc_CfFlowInfo_Reln*)_tmp356->hd;if(_tmp357->vd == _tmp344){union
Cyc_CfFlowInfo_RelnOp _tmp358=_tmp357->rop;struct Cyc_Absyn_Vardecl*_tmp359;struct
Cyc_Absyn_Vardecl*_tmp35A;unsigned int _tmp35B;unsigned int _tmp35C;struct _tuple11
_tmp35D;struct Cyc_Absyn_Vardecl*_tmp35E;_LL16D: if((_tmp358.LessEqNumelts).tag != 
5)goto _LL16F;_tmp359=(struct Cyc_Absyn_Vardecl*)(_tmp358.LessEqNumelts).val;
_LL16E: _tmp35A=_tmp359;goto _LL170;_LL16F: if((_tmp358.LessNumelts).tag != 3)goto
_LL171;_tmp35A=(struct Cyc_Absyn_Vardecl*)(_tmp358.LessNumelts).val;_LL170: if(x == 
_tmp35A)return 1;goto _LL16C;_LL171: if((_tmp358.EqualConst).tag != 1)goto _LL173;
_tmp35B=(unsigned int)(_tmp358.EqualConst).val;_LL172: return Cyc_Toc_check_const_array(
_tmp35B,_tmp313);_LL173: if((_tmp358.LessEqConst).tag != 6)goto _LL175;_tmp35C=(
unsigned int)(_tmp358.LessEqConst).val;if(!(_tmp35C > 0))goto _LL175;_LL174: return
Cyc_Toc_check_const_array(_tmp35C,_tmp313);_LL175: if((_tmp358.LessVar).tag != 2)
goto _LL177;_tmp35D=(struct _tuple11)(_tmp358.LessVar).val;_tmp35E=_tmp35D.f1;
_LL176: if(Cyc_Toc_check_leq_size_var(relns,x,_tmp35E))return 1;goto _LL16C;_LL177:;
_LL178: goto _LL16C;_LL16C:;}}}goto _LL157;_LL15C: if((_tmp341.LessConst).tag != 4)
goto _LL15E;_tmp346=(unsigned int)(_tmp341.LessConst).val;_LL15D: return Cyc_Toc_check_const_array(
_tmp346,_tmp313);_LL15E: if((_tmp341.LessEqConst).tag != 6)goto _LL160;_tmp347=(
unsigned int)(_tmp341.LessEqConst).val;_LL15F: return Cyc_Toc_check_const_array(
_tmp347 + 1,_tmp313);_LL160:;_LL161: goto _LL157;_LL157:;}}}goto _LL142;_LL155:;
_LL156: goto _LL142;_LL142:;}return 0;};}static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*
e){if(e->topt == 0){const char*_tmpB66;void*_tmpB65;(_tmpB65=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB66="Missing type in primop ",
_tag_dyneither(_tmpB66,sizeof(char),24))),_tag_dyneither(_tmpB65,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*
_tmpB69;void*_tmpB68;(_tmpB68=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB69="Missing type in primop ",
_tag_dyneither(_tmpB69,sizeof(char),24))),_tag_dyneither(_tmpB68,sizeof(void*),0)));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple9*
Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){struct _tuple9*_tmpB6A;return(_tmpB6A=
_cycalloc(sizeof(*_tmpB6A)),((_tmpB6A->f1=Cyc_Toc_mt_tq,((_tmpB6A->f2=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmpB6A)))));}static struct
_tuple15*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple15*_tmpB6B;return(_tmpB6B=_cycalloc(sizeof(*_tmpB6B)),((
_tmpB6B->f1=0,((_tmpB6B->f2=e,_tmpB6B)))));};}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(
struct Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int
pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;
if(pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp365=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0  || Cyc_Absyn_no_regions)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(
_tmp365): Cyc_Toc_malloc_ptr(_tmp365));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp365);}};}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp366=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp366 != 0;_tmp366=_tmp366->tl){struct _tuple15 _tmp368;struct Cyc_List_List*
_tmp369;struct Cyc_Absyn_Exp*_tmp36A;struct _tuple15*_tmp367=(struct _tuple15*)
_tmp366->hd;_tmp368=*_tmp367;_tmp369=_tmp368.f1;_tmp36A=_tmp368.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp369 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp369->tl != 0){const char*_tmpB6E;void*_tmpB6D;(_tmpB6D=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB6E="multiple designators in array",
_tag_dyneither(_tmpB6E,sizeof(char),30))),_tag_dyneither(_tmpB6D,sizeof(void*),0)));}{
void*_tmp36D=(void*)_tmp369->hd;void*_tmp36E=_tmp36D;struct Cyc_Absyn_Exp*_tmp370;
_LL17A: {struct Cyc_Absyn_ArrayElement_struct*_tmp36F=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp36E;if(_tmp36F->tag != 0)goto _LL17C;else{_tmp370=_tmp36F->f1;}}_LL17B: Cyc_Toc_exp_to_c(
nv,_tmp370);e_index=_tmp370;goto _LL179;_LL17C: {struct Cyc_Absyn_FieldName_struct*
_tmp371=(struct Cyc_Absyn_FieldName_struct*)_tmp36E;if(_tmp371->tag != 1)goto
_LL179;}_LL17D: {const char*_tmpB71;void*_tmpB70;(_tmpB70=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB71="field name designators in array",
_tag_dyneither(_tmpB71,sizeof(char),32))),_tag_dyneither(_tmpB70,sizeof(void*),0)));}
_LL179:;};}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*
_tmp374=_tmp36A->r;struct Cyc_List_List*_tmp376;struct Cyc_Absyn_Vardecl*_tmp378;
struct Cyc_Absyn_Exp*_tmp379;struct Cyc_Absyn_Exp*_tmp37A;int _tmp37B;void*_tmp37D;
struct Cyc_List_List*_tmp37E;_LL17F: {struct Cyc_Absyn_Array_e_struct*_tmp375=(
struct Cyc_Absyn_Array_e_struct*)_tmp374;if(_tmp375->tag != 27)goto _LL181;else{
_tmp376=_tmp375->f1;}}_LL180: s=Cyc_Toc_init_array(nv,lval,_tmp376,s);goto _LL17E;
_LL181: {struct Cyc_Absyn_Comprehension_e_struct*_tmp377=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp374;if(_tmp377->tag != 28)goto _LL183;else{_tmp378=_tmp377->f1;_tmp379=_tmp377->f2;
_tmp37A=_tmp377->f3;_tmp37B=_tmp377->f4;}}_LL182: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp378,_tmp379,_tmp37A,_tmp37B,s,0);goto _LL17E;_LL183: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp37C=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp374;if(_tmp37C->tag != 30)goto
_LL185;else{_tmp37D=(void*)_tmp37C->f1;_tmp37E=_tmp37C->f2;}}_LL184: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp37D,_tmp37E,s);goto _LL17E;_LL185:;_LL186: Cyc_Toc_exp_to_c(nv,_tmp36A);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
_tmp36A,0),s,0);goto _LL17E;_LL17E:;};};}}return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple0*_tmp37F=vd->name;void*_tmp380=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env _tmp382;struct _RegionHandle*_tmp383;
struct Cyc_Toc_Env*_tmp381=nv;_tmp382=*_tmp381;_tmp383=_tmp382.rgn;{struct Cyc_Absyn_Local_b_struct
_tmpB74;struct Cyc_Absyn_Local_b_struct*_tmpB73;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
_tmp383,nv,_tmp37F,Cyc_Absyn_varb_exp(_tmp37F,(void*)((_tmpB73=_cycalloc(sizeof(*
_tmpB73)),((_tmpB73[0]=((_tmpB74.tag=4,((_tmpB74.f1=vd,_tmpB74)))),_tmpB73)))),0));
struct _tuple0*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp37F,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp37F,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp37F,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp37F,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp384=e2->r;struct Cyc_List_List*_tmp386;
struct Cyc_Absyn_Vardecl*_tmp388;struct Cyc_Absyn_Exp*_tmp389;struct Cyc_Absyn_Exp*
_tmp38A;int _tmp38B;void*_tmp38D;struct Cyc_List_List*_tmp38E;_LL188: {struct Cyc_Absyn_Array_e_struct*
_tmp385=(struct Cyc_Absyn_Array_e_struct*)_tmp384;if(_tmp385->tag != 27)goto _LL18A;
else{_tmp386=_tmp385->f1;}}_LL189: body=Cyc_Toc_init_array(nv2,lval,_tmp386,Cyc_Toc_skip_stmt_dl());
goto _LL187;_LL18A: {struct Cyc_Absyn_Comprehension_e_struct*_tmp387=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp384;if(_tmp387->tag != 28)goto _LL18C;else{_tmp388=_tmp387->f1;_tmp389=_tmp387->f2;
_tmp38A=_tmp387->f3;_tmp38B=_tmp387->f4;}}_LL18B: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp388,_tmp389,_tmp38A,_tmp38B,Cyc_Toc_skip_stmt_dl(),0);goto _LL187;
_LL18C: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp38C=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp384;if(_tmp38C->tag != 30)goto _LL18E;else{_tmp38D=(void*)_tmp38C->f1;_tmp38E=
_tmp38C->f2;}}_LL18D: body=Cyc_Toc_init_anon_struct(nv,lval,_tmp38D,_tmp38E,Cyc_Toc_skip_stmt_dl());
goto _LL187;_LL18E:;_LL18F: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL187;_LL187:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp380,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp37F,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp391=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp391 != 0;_tmp391=_tmp391->tl){struct _tuple15 _tmp393;
struct Cyc_List_List*_tmp394;struct Cyc_Absyn_Exp*_tmp395;struct _tuple15*_tmp392=(
struct _tuple15*)_tmp391->hd;_tmp393=*_tmp392;_tmp394=_tmp393.f1;_tmp395=_tmp393.f2;
if(_tmp394 == 0){const char*_tmpB77;void*_tmpB76;(_tmpB76=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB77="empty designator list",
_tag_dyneither(_tmpB77,sizeof(char),22))),_tag_dyneither(_tmpB76,sizeof(void*),0)));}
if(_tmp394->tl != 0){const char*_tmpB7A;void*_tmpB79;(_tmpB79=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB7A="too many designators in anonymous struct",
_tag_dyneither(_tmpB7A,sizeof(char),41))),_tag_dyneither(_tmpB79,sizeof(void*),0)));}{
void*_tmp39A=(void*)_tmp394->hd;struct _dyneither_ptr*_tmp39C;_LL191: {struct Cyc_Absyn_FieldName_struct*
_tmp39B=(struct Cyc_Absyn_FieldName_struct*)_tmp39A;if(_tmp39B->tag != 1)goto
_LL193;else{_tmp39C=_tmp39B->f1;}}_LL192: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(
lhs,_tmp39C,0);{void*_tmp39D=_tmp395->r;struct Cyc_List_List*_tmp39F;struct Cyc_Absyn_Vardecl*
_tmp3A1;struct Cyc_Absyn_Exp*_tmp3A2;struct Cyc_Absyn_Exp*_tmp3A3;int _tmp3A4;void*
_tmp3A6;struct Cyc_List_List*_tmp3A7;_LL196: {struct Cyc_Absyn_Array_e_struct*
_tmp39E=(struct Cyc_Absyn_Array_e_struct*)_tmp39D;if(_tmp39E->tag != 27)goto _LL198;
else{_tmp39F=_tmp39E->f1;}}_LL197: s=Cyc_Toc_init_array(nv,lval,_tmp39F,s);goto
_LL195;_LL198: {struct Cyc_Absyn_Comprehension_e_struct*_tmp3A0=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp39D;if(_tmp3A0->tag != 28)goto _LL19A;else{_tmp3A1=_tmp3A0->f1;_tmp3A2=_tmp3A0->f2;
_tmp3A3=_tmp3A0->f3;_tmp3A4=_tmp3A0->f4;}}_LL199: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp3A1,_tmp3A2,_tmp3A3,_tmp3A4,s,0);goto _LL195;_LL19A: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3A5=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp39D;if(_tmp3A5->tag != 30)goto
_LL19C;else{_tmp3A6=(void*)_tmp3A5->f1;_tmp3A7=_tmp3A5->f2;}}_LL19B: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3A6,_tmp3A7,s);goto _LL195;_LL19C:;_LL19D: Cyc_Toc_exp_to_c(nv,_tmp395);
if(Cyc_Toc_is_poly_field(struct_type,_tmp39C))_tmp395=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp395);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp395,0),0),s,0);goto _LL195;_LL195:;}goto _LL190;}_LL193:;_LL194: {const char*
_tmpB7D;void*_tmpB7C;(_tmpB7C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB7D="array designator in struct",
_tag_dyneither(_tmpB7D,sizeof(char),27))),_tag_dyneither(_tmpB7C,sizeof(void*),0)));}
_LL190:;};}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct Cyc_Toc_Env
_tmp3AB;struct _RegionHandle*_tmp3AC;struct Cyc_Toc_Env*_tmp3AA=nv;_tmp3AB=*
_tmp3AA;_tmp3AC=_tmp3AB.rgn;{struct Cyc_List_List*_tmp3AD=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_rmap)(_tmp3AC,Cyc_Toc_tup_to_c,es);void*_tmp3AE=Cyc_Toc_add_tuple_type(
_tmp3AD);struct _tuple0*_tmp3AF=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp3B0=
Cyc_Absyn_var_exp(_tmp3AF,0);struct Cyc_Absyn_Stmt*_tmp3B1=Cyc_Absyn_exp_stmt(
_tmp3B0,0);struct Cyc_Absyn_Exp*(*_tmp3B2)(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp3B3=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp3AC,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp3B3);for(0;_tmp3B3 != 0;(_tmp3B3=
_tmp3B3->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp3B3->hd;struct
Cyc_Absyn_Exp*lval=_tmp3B2(_tmp3B0,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp3B4=e->r;struct Cyc_List_List*_tmp3B6;struct Cyc_Absyn_Vardecl*_tmp3B8;
struct Cyc_Absyn_Exp*_tmp3B9;struct Cyc_Absyn_Exp*_tmp3BA;int _tmp3BB;_LL19F: {
struct Cyc_Absyn_Array_e_struct*_tmp3B5=(struct Cyc_Absyn_Array_e_struct*)_tmp3B4;
if(_tmp3B5->tag != 27)goto _LL1A1;else{_tmp3B6=_tmp3B5->f1;}}_LL1A0: _tmp3B1=Cyc_Toc_init_array(
nv,lval,_tmp3B6,_tmp3B1);goto _LL19E;_LL1A1: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp3B7=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp3B4;if(_tmp3B7->tag != 28)
goto _LL1A3;else{_tmp3B8=_tmp3B7->f1;_tmp3B9=_tmp3B7->f2;_tmp3BA=_tmp3B7->f3;
_tmp3BB=_tmp3B7->f4;}}_LL1A2: _tmp3B1=Cyc_Toc_init_comprehension(nv,lval,_tmp3B8,
_tmp3B9,_tmp3BA,_tmp3BB,_tmp3B1,0);goto _LL19E;_LL1A3:;_LL1A4: Cyc_Toc_exp_to_c(nv,
e);_tmp3B1=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3B2(
_tmp3B0,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp3B1,0);goto _LL19E;_LL19E:;};}}
return Cyc_Toc_make_struct(nv,_tmp3AF,_tmp3AE,_tmp3B1,pointer,rgnopt,is_atomic);};}
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct
_dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp3BC=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp3BC != 0;_tmp3BC=_tmp3BC->tl){struct Cyc_Absyn_Aggrfield*
_tmp3BD=(struct Cyc_Absyn_Aggrfield*)_tmp3BC->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp3BD->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}{
struct Cyc_String_pa_struct _tmpB85;void*_tmpB84[1];const char*_tmpB83;void*_tmpB82;(
_tmpB82=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)((_tmpB85.tag=0,((_tmpB85.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmpB84[0]=& _tmpB85,Cyc_aprintf(((_tmpB83="get_member_offset %s failed",
_tag_dyneither(_tmpB83,sizeof(char),28))),_tag_dyneither(_tmpB84,sizeof(void*),1)))))))),
_tag_dyneither(_tmpB82,sizeof(void*),0)));};}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){struct _tuple0*_tmp3C2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3C3=Cyc_Absyn_var_exp(_tmp3C2,0);struct Cyc_Absyn_Stmt*
_tmp3C4=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp3C3),0);struct Cyc_Absyn_Exp*(*
_tmp3C5)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*)=
pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;void*_tmp3C6=Cyc_Toc_aggrdecl_type(
tdn,Cyc_Absyn_strctq);int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp3C7=
Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp3C9;union Cyc_Absyn_AggrInfoU
_tmp3CA;_LL1A6: {struct Cyc_Absyn_AggrType_struct*_tmp3C8=(struct Cyc_Absyn_AggrType_struct*)
_tmp3C7;if(_tmp3C8->tag != 12)goto _LL1A8;else{_tmp3C9=_tmp3C8->f1;_tmp3CA=_tmp3C9.aggr_info;}}
_LL1A7: ad=Cyc_Absyn_get_known_aggrdecl(_tmp3CA);goto _LL1A5;_LL1A8:;_LL1A9: {
const char*_tmpB88;void*_tmpB87;(_tmpB87=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB88="init_struct: bad struct type",
_tag_dyneither(_tmpB88,sizeof(char),29))),_tag_dyneither(_tmpB87,sizeof(void*),0)));}
_LL1A5:;}{int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct Cyc_Toc_Env _tmp3CE;struct _RegionHandle*_tmp3CF;struct Cyc_Toc_Env*_tmp3CD=
nv;_tmp3CE=*_tmp3CD;_tmp3CF=_tmp3CE.rgn;{struct Cyc_List_List*_tmp3D0=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp3CF,dles);for(0;
_tmp3D0 != 0;_tmp3D0=_tmp3D0->tl){struct _tuple15 _tmp3D2;struct Cyc_List_List*
_tmp3D3;struct Cyc_Absyn_Exp*_tmp3D4;struct _tuple15*_tmp3D1=(struct _tuple15*)
_tmp3D0->hd;_tmp3D2=*_tmp3D1;_tmp3D3=_tmp3D2.f1;_tmp3D4=_tmp3D2.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3D4->topt))->v);
if(_tmp3D3 == 0){const char*_tmpB8B;void*_tmpB8A;(_tmpB8A=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8B="empty designator list",
_tag_dyneither(_tmpB8B,sizeof(char),22))),_tag_dyneither(_tmpB8A,sizeof(void*),0)));}
if(_tmp3D3->tl != 0){struct _tuple0*_tmp3D7=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp3D8=Cyc_Absyn_var_exp(_tmp3D7,0);for(0;_tmp3D3 != 0;_tmp3D3=_tmp3D3->tl){void*
_tmp3D9=(void*)_tmp3D3->hd;struct _dyneither_ptr*_tmp3DB;_LL1AB: {struct Cyc_Absyn_FieldName_struct*
_tmp3DA=(struct Cyc_Absyn_FieldName_struct*)_tmp3D9;if(_tmp3DA->tag != 1)goto
_LL1AD;else{_tmp3DB=_tmp3DA->f1;}}_LL1AC: if(Cyc_Toc_is_poly_field(struct_type,
_tmp3DB))_tmp3D8=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3D8);_tmp3C4=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3C5(_tmp3C3,_tmp3DB,0),_tmp3D8,0),0),
_tmp3C4,0);goto _LL1AA;_LL1AD:;_LL1AE: {const char*_tmpB8E;void*_tmpB8D;(_tmpB8D=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB8E="array designator in struct",_tag_dyneither(_tmpB8E,sizeof(char),27))),
_tag_dyneither(_tmpB8D,sizeof(void*),0)));}_LL1AA:;}Cyc_Toc_exp_to_c(nv,_tmp3D4);
_tmp3C4=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3D8,
_tmp3D4,0),0),_tmp3C4,0);}else{void*_tmp3DE=(void*)_tmp3D3->hd;struct
_dyneither_ptr*_tmp3E0;_LL1B0: {struct Cyc_Absyn_FieldName_struct*_tmp3DF=(struct
Cyc_Absyn_FieldName_struct*)_tmp3DE;if(_tmp3DF->tag != 1)goto _LL1B2;else{_tmp3E0=
_tmp3DF->f1;}}_LL1B1: {struct Cyc_Absyn_Exp*lval=_tmp3C5(_tmp3C3,_tmp3E0,0);if(
is_tagged_union){int i=Cyc_Toc_get_member_offset(ad,_tmp3E0);struct Cyc_Absyn_Exp*
f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_aggrmember_exp(
lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,
f_tag_exp,0);_tmp3C4=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3C4,0);
lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{void*_tmp3E1=_tmp3D4->r;
struct Cyc_List_List*_tmp3E3;struct Cyc_Absyn_Vardecl*_tmp3E5;struct Cyc_Absyn_Exp*
_tmp3E6;struct Cyc_Absyn_Exp*_tmp3E7;int _tmp3E8;void*_tmp3EA;struct Cyc_List_List*
_tmp3EB;_LL1B5: {struct Cyc_Absyn_Array_e_struct*_tmp3E2=(struct Cyc_Absyn_Array_e_struct*)
_tmp3E1;if(_tmp3E2->tag != 27)goto _LL1B7;else{_tmp3E3=_tmp3E2->f1;}}_LL1B6:
_tmp3C4=Cyc_Toc_init_array(nv,lval,_tmp3E3,_tmp3C4);goto _LL1B4;_LL1B7: {struct
Cyc_Absyn_Comprehension_e_struct*_tmp3E4=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3E1;if(_tmp3E4->tag != 28)goto _LL1B9;else{_tmp3E5=_tmp3E4->f1;_tmp3E6=_tmp3E4->f2;
_tmp3E7=_tmp3E4->f3;_tmp3E8=_tmp3E4->f4;}}_LL1B8: _tmp3C4=Cyc_Toc_init_comprehension(
nv,lval,_tmp3E5,_tmp3E6,_tmp3E7,_tmp3E8,_tmp3C4,0);goto _LL1B4;_LL1B9: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3E9=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3E1;if(_tmp3E9->tag != 30)goto
_LL1BB;else{_tmp3EA=(void*)_tmp3E9->f1;_tmp3EB=_tmp3E9->f2;}}_LL1BA: _tmp3C4=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3EA,_tmp3EB,_tmp3C4);goto _LL1B4;_LL1BB:;_LL1BC: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3D4->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp3D4);{struct Cyc_Absyn_Aggrfield*_tmp3EC=Cyc_Absyn_lookup_decl_field(ad,
_tmp3E0);if(Cyc_Toc_is_poly_field(struct_type,_tmp3E0) && !was_ptr_type)_tmp3D4=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3D4);if(has_exists)_tmp3D4=Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3EC))->type),
_tmp3D4);_tmp3C4=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp3D4,0),0),_tmp3C4,0);goto _LL1B4;};}_LL1B4:;}goto _LL1AF;}_LL1B2:;_LL1B3: {
const char*_tmpB91;void*_tmpB90;(_tmpB90=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB91="array designator in struct",
_tag_dyneither(_tmpB91,sizeof(char),27))),_tag_dyneither(_tmpB90,sizeof(void*),0)));}
_LL1AF:;}}}return Cyc_Toc_make_struct(nv,_tmp3C2,_tmp3C6,_tmp3C4,pointer,rgnopt,
is_atomic);};}struct _tuple18{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct
_tuple18*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(
e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,
enum Cyc_Absyn_Incrementor incr){struct Cyc_Absyn_Increment_e_struct _tmpB94;struct
Cyc_Absyn_Increment_e_struct*_tmpB93;return Cyc_Absyn_new_exp((void*)((_tmpB93=
_cycalloc(sizeof(*_tmpB93)),((_tmpB93[0]=((_tmpB94.tag=5,((_tmpB94.f1=e1,((
_tmpB94.f2=incr,_tmpB94)))))),_tmpB93)))),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp3F1=e1->r;struct Cyc_Absyn_Stmt*_tmp3F3;void*_tmp3F5;struct Cyc_Absyn_Exp*
_tmp3F6;struct Cyc_Absyn_Exp*_tmp3F8;struct _dyneither_ptr*_tmp3F9;int _tmp3FA;int
_tmp3FB;_LL1BE: {struct Cyc_Absyn_StmtExp_e_struct*_tmp3F2=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp3F1;if(_tmp3F2->tag != 37)goto _LL1C0;else{_tmp3F3=_tmp3F2->f1;}}_LL1BF: Cyc_Toc_lvalue_assign_stmt(
_tmp3F3,fs,f,f_env);goto _LL1BD;_LL1C0: {struct Cyc_Absyn_Cast_e_struct*_tmp3F4=(
struct Cyc_Absyn_Cast_e_struct*)_tmp3F1;if(_tmp3F4->tag != 15)goto _LL1C2;else{
_tmp3F5=(void*)_tmp3F4->f1;_tmp3F6=_tmp3F4->f2;}}_LL1C1: Cyc_Toc_lvalue_assign(
_tmp3F6,fs,f,f_env);goto _LL1BD;_LL1C2: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp3F7=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp3F1;if(_tmp3F7->tag != 22)goto
_LL1C4;else{_tmp3F8=_tmp3F7->f1;_tmp3F9=_tmp3F7->f2;_tmp3FA=_tmp3F7->f3;_tmp3FB=
_tmp3F7->f4;}}_LL1C3: e1->r=_tmp3F8->r;{struct Cyc_List_List*_tmpB95;Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)((_tmpB95=_cycalloc(sizeof(*_tmpB95)),((_tmpB95->hd=
_tmp3F9,((_tmpB95->tl=fs,_tmpB95)))))),f,f_env);}goto _LL1BD;_LL1C4:;_LL1C5: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;goto _LL1BD;}_LL1BD:;}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env){void*_tmp3FD=s->r;struct Cyc_Absyn_Exp*_tmp3FF;struct Cyc_Absyn_Decl*
_tmp401;struct Cyc_Absyn_Stmt*_tmp402;struct Cyc_Absyn_Stmt*_tmp404;_LL1C7: {
struct Cyc_Absyn_Exp_s_struct*_tmp3FE=(struct Cyc_Absyn_Exp_s_struct*)_tmp3FD;if(
_tmp3FE->tag != 1)goto _LL1C9;else{_tmp3FF=_tmp3FE->f1;}}_LL1C8: Cyc_Toc_lvalue_assign(
_tmp3FF,fs,f,f_env);goto _LL1C6;_LL1C9: {struct Cyc_Absyn_Decl_s_struct*_tmp400=(
struct Cyc_Absyn_Decl_s_struct*)_tmp3FD;if(_tmp400->tag != 12)goto _LL1CB;else{
_tmp401=_tmp400->f1;_tmp402=_tmp400->f2;}}_LL1CA: Cyc_Toc_lvalue_assign_stmt(
_tmp402,fs,f,f_env);goto _LL1C6;_LL1CB: {struct Cyc_Absyn_Seq_s_struct*_tmp403=(
struct Cyc_Absyn_Seq_s_struct*)_tmp3FD;if(_tmp403->tag != 2)goto _LL1CD;else{
_tmp404=_tmp403->f2;}}_LL1CC: Cyc_Toc_lvalue_assign_stmt(_tmp404,fs,f,f_env);goto
_LL1C6;_LL1CD:;_LL1CE: {const char*_tmpB99;void*_tmpB98[1];struct Cyc_String_pa_struct
_tmpB97;(_tmpB97.tag=0,((_tmpB97.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmpB98[0]=& _tmpB97,Cyc_Toc_toc_impos(((_tmpB99="lvalue_assign_stmt: %s",
_tag_dyneither(_tmpB99,sizeof(char),23))),_tag_dyneither(_tmpB98,sizeof(void*),1)))))));}
_LL1C6:;}static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);static
struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){void*_tmp408=
e->r;void*_tmp40A;void**_tmp40B;struct Cyc_Absyn_Exp*_tmp40C;struct Cyc_Absyn_Exp**
_tmp40D;struct Cyc_Absyn_Exp*_tmp40F;struct Cyc_Absyn_Stmt*_tmp411;_LL1D0: {struct
Cyc_Absyn_Cast_e_struct*_tmp409=(struct Cyc_Absyn_Cast_e_struct*)_tmp408;if(
_tmp409->tag != 15)goto _LL1D2;else{_tmp40A=(void**)& _tmp409->f1;_tmp40B=(void**)((
void**)& _tmp409->f1);_tmp40C=_tmp409->f2;_tmp40D=(struct Cyc_Absyn_Exp**)& _tmp409->f2;}}
_LL1D1:*_tmp40D=Cyc_Toc_push_address_exp(*_tmp40D);*_tmp40B=Cyc_Absyn_cstar_typ(*
_tmp40B,Cyc_Toc_mt_tq);return e;_LL1D2: {struct Cyc_Absyn_Deref_e_struct*_tmp40E=(
struct Cyc_Absyn_Deref_e_struct*)_tmp408;if(_tmp40E->tag != 21)goto _LL1D4;else{
_tmp40F=_tmp40E->f1;}}_LL1D3: return _tmp40F;_LL1D4: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp410=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp408;if(_tmp410->tag != 37)goto
_LL1D6;else{_tmp411=_tmp410->f1;}}_LL1D5: Cyc_Toc_push_address_stmt(_tmp411);
return e;_LL1D6:;_LL1D7: if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpB9D;void*_tmpB9C[1];struct Cyc_String_pa_struct _tmpB9B;(_tmpB9B.tag=
0,((_tmpB9B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpB9C[0]=& _tmpB9B,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB9D="can't take & of exp %s",_tag_dyneither(_tmpB9D,
sizeof(char),23))),_tag_dyneither(_tmpB9C,sizeof(void*),1)))))));};_LL1CF:;}
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){void*_tmp415=s->r;
struct Cyc_Absyn_Stmt*_tmp417;struct Cyc_Absyn_Stmt*_tmp419;struct Cyc_Absyn_Exp*
_tmp41B;struct Cyc_Absyn_Exp**_tmp41C;_LL1D9: {struct Cyc_Absyn_Seq_s_struct*
_tmp416=(struct Cyc_Absyn_Seq_s_struct*)_tmp415;if(_tmp416->tag != 2)goto _LL1DB;
else{_tmp417=_tmp416->f2;}}_LL1DA: _tmp419=_tmp417;goto _LL1DC;_LL1DB: {struct Cyc_Absyn_Decl_s_struct*
_tmp418=(struct Cyc_Absyn_Decl_s_struct*)_tmp415;if(_tmp418->tag != 12)goto _LL1DD;
else{_tmp419=_tmp418->f2;}}_LL1DC: Cyc_Toc_push_address_stmt(_tmp419);goto _LL1D8;
_LL1DD: {struct Cyc_Absyn_Exp_s_struct*_tmp41A=(struct Cyc_Absyn_Exp_s_struct*)
_tmp415;if(_tmp41A->tag != 1)goto _LL1DF;else{_tmp41B=_tmp41A->f1;_tmp41C=(struct
Cyc_Absyn_Exp**)& _tmp41A->f1;}}_LL1DE:*_tmp41C=Cyc_Toc_push_address_exp(*_tmp41C);
goto _LL1D8;_LL1DF:;_LL1E0: {const char*_tmpBA1;void*_tmpBA0[1];struct Cyc_String_pa_struct
_tmpB9F;(_tmpB9F.tag=0,((_tmpB9F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmpBA0[0]=& _tmpB9F,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA1="can't take & of stmt %s",
_tag_dyneither(_tmpBA1,sizeof(char),24))),_tag_dyneither(_tmpBA0,sizeof(void*),1)))))));}
_LL1D8:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;
struct Cyc_List_List*prev;if(x == 0)return 0;{struct Cyc_List_List*_tmpBA2;result=((
_tmpBA2=_region_malloc(r2,sizeof(*_tmpBA2)),((_tmpBA2->hd=(void*)f((void*)x->hd,
env),((_tmpBA2->tl=0,_tmpBA2))))));}prev=result;for(x=x->tl;x != 0;x=x->tl){{
struct Cyc_List_List*_tmpBA3;((struct Cyc_List_List*)_check_null(prev))->tl=((
_tmpBA3=_region_malloc(r2,sizeof(*_tmpBA3)),((_tmpBA3->hd=(void*)f((void*)x->hd,
env),((_tmpBA3->tl=0,_tmpBA3))))));}prev=prev->tl;}return result;}static struct Cyc_List_List*
Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region,f,env,x);}static struct _tuple15*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*
e){struct _tuple15*_tmpBA4;return(_tmpBA4=_cycalloc(sizeof(*_tmpBA4)),((_tmpBA4->f1=
0,((_tmpBA4->f2=e,_tmpBA4)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(
void*t){void*_tmp423=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp425;
_LL1E2: {struct Cyc_Absyn_PointerType_struct*_tmp424=(struct Cyc_Absyn_PointerType_struct*)
_tmp423;if(_tmp424->tag != 5)goto _LL1E4;else{_tmp425=_tmp424->f1;}}_LL1E3: return
_tmp425;_LL1E4:;_LL1E5: {const char*_tmpBA7;void*_tmpBA6;(_tmpBA6=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA7="get_ptr_typ: not a pointer!",
_tag_dyneither(_tmpBA7,sizeof(char),28))),_tag_dyneither(_tmpBA6,sizeof(void*),0)));}
_LL1E1:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp428=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp42B;enum Cyc_Absyn_Size_of
_tmp42C;enum Cyc_Absyn_Sign _tmp42E;enum Cyc_Absyn_Size_of _tmp42F;enum Cyc_Absyn_Sign
_tmp433;enum Cyc_Absyn_Size_of _tmp434;enum Cyc_Absyn_Sign _tmp436;enum Cyc_Absyn_Size_of
_tmp437;enum Cyc_Absyn_Sign _tmp439;enum Cyc_Absyn_Size_of _tmp43A;_LL1E7: {struct
Cyc_Absyn_PointerType_struct*_tmp429=(struct Cyc_Absyn_PointerType_struct*)
_tmp428;if(_tmp429->tag != 5)goto _LL1E9;}_LL1E8: res=Cyc_Absyn_null_exp(0);goto
_LL1E6;_LL1E9: {struct Cyc_Absyn_IntType_struct*_tmp42A=(struct Cyc_Absyn_IntType_struct*)
_tmp428;if(_tmp42A->tag != 6)goto _LL1EB;else{_tmp42B=_tmp42A->f1;_tmp42C=_tmp42A->f2;
if(_tmp42C != Cyc_Absyn_Char_sz)goto _LL1EB;}}_LL1EA: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(
_tmp42B,'\000'),0);goto _LL1E6;_LL1EB: {struct Cyc_Absyn_IntType_struct*_tmp42D=(
struct Cyc_Absyn_IntType_struct*)_tmp428;if(_tmp42D->tag != 6)goto _LL1ED;else{
_tmp42E=_tmp42D->f1;_tmp42F=_tmp42D->f2;if(_tmp42F != Cyc_Absyn_Short_sz)goto
_LL1ED;}}_LL1EC: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp42E,0),0);goto
_LL1E6;_LL1ED: {struct Cyc_Absyn_EnumType_struct*_tmp430=(struct Cyc_Absyn_EnumType_struct*)
_tmp428;if(_tmp430->tag != 14)goto _LL1EF;}_LL1EE: goto _LL1F0;_LL1EF: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp431=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp428;if(_tmp431->tag != 15)goto
_LL1F1;}_LL1F0: _tmp433=Cyc_Absyn_Unsigned;goto _LL1F2;_LL1F1: {struct Cyc_Absyn_IntType_struct*
_tmp432=(struct Cyc_Absyn_IntType_struct*)_tmp428;if(_tmp432->tag != 6)goto _LL1F3;
else{_tmp433=_tmp432->f1;_tmp434=_tmp432->f2;if(_tmp434 != Cyc_Absyn_Int_sz)goto
_LL1F3;}}_LL1F2: _tmp436=_tmp433;goto _LL1F4;_LL1F3: {struct Cyc_Absyn_IntType_struct*
_tmp435=(struct Cyc_Absyn_IntType_struct*)_tmp428;if(_tmp435->tag != 6)goto _LL1F5;
else{_tmp436=_tmp435->f1;_tmp437=_tmp435->f2;if(_tmp437 != Cyc_Absyn_Long_sz)goto
_LL1F5;}}_LL1F4: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp436,0),0);goto _LL1E6;
_LL1F5: {struct Cyc_Absyn_IntType_struct*_tmp438=(struct Cyc_Absyn_IntType_struct*)
_tmp428;if(_tmp438->tag != 6)goto _LL1F7;else{_tmp439=_tmp438->f1;_tmp43A=_tmp438->f2;
if(_tmp43A != Cyc_Absyn_LongLong_sz)goto _LL1F7;}}_LL1F6: res=Cyc_Absyn_const_exp(
Cyc_Absyn_LongLong_c(_tmp439,(long long)0),0);goto _LL1E6;_LL1F7: {struct Cyc_Absyn_FloatType_struct*
_tmp43B=(struct Cyc_Absyn_FloatType_struct*)_tmp428;if(_tmp43B->tag != 7)goto
_LL1F9;}_LL1F8: goto _LL1FA;_LL1F9: {struct Cyc_Absyn_DoubleType_struct*_tmp43C=(
struct Cyc_Absyn_DoubleType_struct*)_tmp428;if(_tmp43C->tag != 8)goto _LL1FB;}
_LL1FA:{const char*_tmpBA8;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBA8="0.0",
_tag_dyneither(_tmpBA8,sizeof(char),4)))),0);}goto _LL1E6;_LL1FB:;_LL1FC: {const
char*_tmpBAC;void*_tmpBAB[1];struct Cyc_String_pa_struct _tmpBAA;(_tmpBAA.tag=0,((
_tmpBAA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmpBAB[0]=& _tmpBAA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBAC="found non-zero type %s in generate_zero",_tag_dyneither(_tmpBAC,sizeof(
char),40))),_tag_dyneither(_tmpBAB,sizeof(void*),1)))))));}_LL1E6:;}{struct Cyc_Core_Opt*
_tmpBAD;res->topt=((_tmpBAD=_cycalloc(sizeof(*_tmpBAD)),((_tmpBAD->v=t,_tmpBAD))));}
return res;}static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dyneither,void*elt_type){void*fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);void*
_tmp442=Cyc_Toc_typ_to_c(elt_type);void*_tmp443=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp444=Cyc_Absyn_cstar_typ(_tmp442,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmpBAE;struct Cyc_Core_Opt*_tmp445=(_tmpBAE=_cycalloc(sizeof(*_tmpBAE)),((
_tmpBAE->v=_tmp444,_tmpBAE)));struct Cyc_Absyn_Exp*xinit;{void*_tmp446=e1->r;
struct Cyc_Absyn_Exp*_tmp448;struct Cyc_Absyn_Exp*_tmp44A;struct Cyc_Absyn_Exp*
_tmp44B;_LL1FE: {struct Cyc_Absyn_Deref_e_struct*_tmp447=(struct Cyc_Absyn_Deref_e_struct*)
_tmp446;if(_tmp447->tag != 21)goto _LL200;else{_tmp448=_tmp447->f1;}}_LL1FF: if(!
is_dyneither){_tmp448=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp448,0,Cyc_Absyn_Other_coercion,
0);{struct Cyc_Core_Opt*_tmpBAF;_tmp448->topt=((_tmpBAF=_cycalloc(sizeof(*_tmpBAF)),((
_tmpBAF->v=fat_ptr_type,_tmpBAF))));};}Cyc_Toc_exp_to_c(nv,_tmp448);xinit=
_tmp448;goto _LL1FD;_LL200: {struct Cyc_Absyn_Subscript_e_struct*_tmp449=(struct
Cyc_Absyn_Subscript_e_struct*)_tmp446;if(_tmp449->tag != 24)goto _LL202;else{
_tmp44A=_tmp449->f1;_tmp44B=_tmp449->f2;}}_LL201: if(!is_dyneither){_tmp44A=Cyc_Absyn_cast_exp(
fat_ptr_type,_tmp44A,0,Cyc_Absyn_Other_coercion,0);{struct Cyc_Core_Opt*_tmpBB0;
_tmp44A->topt=((_tmpBB0=_cycalloc(sizeof(*_tmpBB0)),((_tmpBB0->v=fat_ptr_type,
_tmpBB0))));};}Cyc_Toc_exp_to_c(nv,_tmp44A);Cyc_Toc_exp_to_c(nv,_tmp44B);{struct
Cyc_Absyn_Exp*_tmpBB1[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,((
_tmpBB1[2]=_tmp44B,((_tmpBB1[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmpBB1[0]=_tmp44A,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpBB1,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1FD;_LL202:;_LL203: {const char*_tmpBB4;void*_tmpBB3;(_tmpBB3=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB4="found bad lhs for zero-terminated pointer assignment",
_tag_dyneither(_tmpBB4,sizeof(char),53))),_tag_dyneither(_tmpBB3,sizeof(void*),0)));}
_LL1FD:;}{struct _tuple0*_tmp451=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp453;
struct _RegionHandle*_tmp454;struct Cyc_Toc_Env*_tmp452=nv;_tmp453=*_tmp452;
_tmp454=_tmp453.rgn;{struct Cyc_Toc_Env*_tmp455=Cyc_Toc_add_varmap(_tmp454,nv,
_tmp451,Cyc_Absyn_var_exp(_tmp451,0));struct Cyc_Absyn_Vardecl*_tmpBB5;struct Cyc_Absyn_Vardecl*
_tmp456=(_tmpBB5=_cycalloc(sizeof(*_tmpBB5)),((_tmpBB5->sc=Cyc_Absyn_Public,((
_tmpBB5->name=_tmp451,((_tmpBB5->tq=Cyc_Toc_mt_tq,((_tmpBB5->type=_tmp443,((
_tmpBB5->initializer=(struct Cyc_Absyn_Exp*)xinit,((_tmpBB5->rgn=0,((_tmpBB5->attributes=
0,((_tmpBB5->escapes=0,_tmpBB5)))))))))))))))));struct Cyc_Absyn_Local_b_struct
_tmpBB8;struct Cyc_Absyn_Local_b_struct*_tmpBB7;struct Cyc_Absyn_Local_b_struct*
_tmp457=(_tmpBB7=_cycalloc(sizeof(*_tmpBB7)),((_tmpBB7[0]=((_tmpBB8.tag=4,((
_tmpBB8.f1=_tmp456,_tmpBB8)))),_tmpBB7)));struct Cyc_Absyn_Exp*_tmp458=Cyc_Absyn_varb_exp(
_tmp451,(void*)_tmp457,0);{struct Cyc_Core_Opt*_tmpBB9;_tmp458->topt=((_tmpBB9=
_cycalloc(sizeof(*_tmpBB9)),((_tmpBB9->v=fat_ptr_type,_tmpBB9))));}{struct Cyc_Absyn_Exp*
_tmp45A=Cyc_Absyn_deref_exp(_tmp458,0);{struct Cyc_Core_Opt*_tmpBBA;_tmp45A->topt=((
_tmpBBA=_cycalloc(sizeof(*_tmpBBA)),((_tmpBBA->v=elt_type,_tmpBBA))));}Cyc_Toc_exp_to_c(
_tmp455,_tmp45A);{struct _tuple0*_tmp45C=Cyc_Toc_temp_var();_tmp455=Cyc_Toc_add_varmap(
_tmp454,_tmp455,_tmp45C,Cyc_Absyn_var_exp(_tmp45C,0));{struct Cyc_Absyn_Vardecl*
_tmpBBB;struct Cyc_Absyn_Vardecl*_tmp45D=(_tmpBBB=_cycalloc(sizeof(*_tmpBBB)),((
_tmpBBB->sc=Cyc_Absyn_Public,((_tmpBBB->name=_tmp45C,((_tmpBBB->tq=Cyc_Toc_mt_tq,((
_tmpBBB->type=_tmp442,((_tmpBBB->initializer=(struct Cyc_Absyn_Exp*)_tmp45A,((
_tmpBBB->rgn=0,((_tmpBBB->attributes=0,((_tmpBBB->escapes=0,_tmpBBB)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpBBE;struct Cyc_Absyn_Local_b_struct*_tmpBBD;
struct Cyc_Absyn_Local_b_struct*_tmp45E=(_tmpBBD=_cycalloc(sizeof(*_tmpBBD)),((
_tmpBBD[0]=((_tmpBBE.tag=4,((_tmpBBE.f1=_tmp45D,_tmpBBE)))),_tmpBBD)));struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp45F=Cyc_Absyn_varb_exp(_tmp45C,(
void*)_tmp45E,0);_tmp45F->topt=_tmp45A->topt;z_init=Cyc_Absyn_prim2_exp((enum 
Cyc_Absyn_Primop)popt->v,_tmp45F,e2,0);z_init->topt=_tmp45F->topt;}Cyc_Toc_exp_to_c(
_tmp455,z_init);{struct _tuple0*_tmp460=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmpBBF;struct Cyc_Absyn_Vardecl*_tmp461=(_tmpBBF=_cycalloc(sizeof(*_tmpBBF)),((
_tmpBBF->sc=Cyc_Absyn_Public,((_tmpBBF->name=_tmp460,((_tmpBBF->tq=Cyc_Toc_mt_tq,((
_tmpBBF->type=_tmp442,((_tmpBBF->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmpBBF->rgn=0,((_tmpBBF->attributes=0,((_tmpBBF->escapes=0,_tmpBBF)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpBC2;struct Cyc_Absyn_Local_b_struct*_tmpBC1;
struct Cyc_Absyn_Local_b_struct*_tmp462=(_tmpBC1=_cycalloc(sizeof(*_tmpBC1)),((
_tmpBC1[0]=((_tmpBC2.tag=4,((_tmpBC2.f1=_tmp461,_tmpBC2)))),_tmpBC1)));_tmp455=
Cyc_Toc_add_varmap(_tmp454,_tmp455,_tmp460,Cyc_Absyn_var_exp(_tmp460,0));{struct
Cyc_Absyn_Exp*_tmp463=Cyc_Absyn_varb_exp(_tmp45C,(void*)_tmp45E,0);_tmp463->topt=
_tmp45A->topt;{struct Cyc_Absyn_Exp*_tmp464=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp465=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp463,_tmp464,0);{
struct Cyc_Core_Opt*_tmpBC3;_tmp465->topt=((_tmpBC3=_cycalloc(sizeof(*_tmpBC3)),((
_tmpBC3->v=Cyc_Absyn_sint_typ,_tmpBC3))));}Cyc_Toc_exp_to_c(_tmp455,_tmp465);{
struct Cyc_Absyn_Exp*_tmp467=Cyc_Absyn_varb_exp(_tmp460,(void*)_tmp462,0);_tmp467->topt=
_tmp45A->topt;{struct Cyc_Absyn_Exp*_tmp468=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp469=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp467,_tmp468,0);{
struct Cyc_Core_Opt*_tmpBC4;_tmp469->topt=((_tmpBC4=_cycalloc(sizeof(*_tmpBC4)),((
_tmpBC4->v=Cyc_Absyn_sint_typ,_tmpBC4))));}Cyc_Toc_exp_to_c(_tmp455,_tmp469);{
struct Cyc_Absyn_Exp*_tmpBC5[2];struct Cyc_List_List*_tmp46B=(_tmpBC5[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0),((_tmpBC5[0]=Cyc_Absyn_varb_exp(_tmp451,(void*)
_tmp457,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBC5,sizeof(struct Cyc_Absyn_Exp*),2)))));struct Cyc_Absyn_Exp*
_tmp46C=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;xsize=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp46B,0),
_tmp46C,0);{struct Cyc_Absyn_Exp*_tmp46D=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp465,_tmp469,0),0);struct Cyc_Absyn_Stmt*_tmp46E=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp46F=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp451,(void*)_tmp457,0),Cyc_Toc_curr_sp,0);_tmp46F=Cyc_Toc_cast_it(
_tmp444,_tmp46F);{struct Cyc_Absyn_Exp*_tmp470=Cyc_Absyn_deref_exp(_tmp46F,0);
struct Cyc_Absyn_Exp*_tmp471=Cyc_Absyn_assign_exp(_tmp470,Cyc_Absyn_var_exp(
_tmp460,0),0);struct Cyc_Absyn_Stmt*_tmp472=Cyc_Absyn_exp_stmt(_tmp471,0);_tmp472=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp46D,_tmp46E,Cyc_Absyn_skip_stmt(
0),0),_tmp472,0);{struct Cyc_Absyn_Var_d_struct*_tmpBCB;struct Cyc_Absyn_Var_d_struct
_tmpBCA;struct Cyc_Absyn_Decl*_tmpBC9;_tmp472=Cyc_Absyn_decl_stmt(((_tmpBC9=
_cycalloc(sizeof(*_tmpBC9)),((_tmpBC9->r=(void*)((_tmpBCB=_cycalloc(sizeof(*
_tmpBCB)),((_tmpBCB[0]=((_tmpBCA.tag=0,((_tmpBCA.f1=_tmp461,_tmpBCA)))),_tmpBCB)))),((
_tmpBC9->loc=0,_tmpBC9)))))),_tmp472,0);}{struct Cyc_Absyn_Var_d_struct*_tmpBD1;
struct Cyc_Absyn_Var_d_struct _tmpBD0;struct Cyc_Absyn_Decl*_tmpBCF;_tmp472=Cyc_Absyn_decl_stmt(((
_tmpBCF=_cycalloc(sizeof(*_tmpBCF)),((_tmpBCF->r=(void*)((_tmpBD1=_cycalloc(
sizeof(*_tmpBD1)),((_tmpBD1[0]=((_tmpBD0.tag=0,((_tmpBD0.f1=_tmp45D,_tmpBD0)))),
_tmpBD1)))),((_tmpBCF->loc=0,_tmpBCF)))))),_tmp472,0);}{struct Cyc_Absyn_Var_d_struct*
_tmpBD7;struct Cyc_Absyn_Var_d_struct _tmpBD6;struct Cyc_Absyn_Decl*_tmpBD5;_tmp472=
Cyc_Absyn_decl_stmt(((_tmpBD5=_cycalloc(sizeof(*_tmpBD5)),((_tmpBD5->r=(void*)((
_tmpBD7=_cycalloc(sizeof(*_tmpBD7)),((_tmpBD7[0]=((_tmpBD6.tag=0,((_tmpBD6.f1=
_tmp456,_tmpBD6)))),_tmpBD7)))),((_tmpBD5->loc=0,_tmpBD5)))))),_tmp472,0);}e->r=
Cyc_Toc_stmt_exp_r(_tmp472);};};};};};};};};};};};};};}static void*Cyc_Toc_check_tagged_union(
struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int
in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,
struct Cyc_Position_Segment*)){struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp487=Cyc_Tcutil_compress(
aggrtype);struct Cyc_Absyn_AggrInfo _tmp489;union Cyc_Absyn_AggrInfoU _tmp48A;_LL205: {
struct Cyc_Absyn_AggrType_struct*_tmp488=(struct Cyc_Absyn_AggrType_struct*)
_tmp487;if(_tmp488->tag != 12)goto _LL207;else{_tmp489=_tmp488->f1;_tmp48A=_tmp489.aggr_info;}}
_LL206: ad=Cyc_Absyn_get_known_aggrdecl(_tmp48A);goto _LL204;_LL207:;_LL208: {
struct Cyc_String_pa_struct _tmpBDF;void*_tmpBDE[1];const char*_tmpBDD;void*_tmpBDC;(
_tmpBDC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((
struct _dyneither_ptr)((_tmpBDF.tag=0,((_tmpBDF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpBDE[0]=& _tmpBDF,Cyc_aprintf(((
_tmpBDD="expecting union but found %s in check_tagged_union",_tag_dyneither(
_tmpBDD,sizeof(char),51))),_tag_dyneither(_tmpBDE,sizeof(void*),1)))))))),
_tag_dyneither(_tmpBDC,sizeof(void*),0)));}_LL204:;}{struct _tuple0*_tmp48F=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp490=Cyc_Absyn_var_exp(_tmp48F,0);struct Cyc_Absyn_Exp*
_tmp491=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);if(in_lhs){
struct Cyc_Absyn_Exp*_tmp492=Cyc_Absyn_aggrarrow_exp(_tmp490,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp493=Cyc_Absyn_neq_exp(_tmp492,_tmp491,0);struct Cyc_Absyn_Exp*
_tmp494=Cyc_Absyn_aggrarrow_exp(_tmp490,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Stmt*
_tmp495=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp494,0),0);struct Cyc_Absyn_Stmt*
_tmp496=Cyc_Absyn_ifthenelse_stmt(_tmp493,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);void*_tmp497=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp498=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);struct Cyc_Absyn_Stmt*_tmp499=
Cyc_Absyn_declare_stmt(_tmp48F,_tmp497,(struct Cyc_Absyn_Exp*)_tmp498,Cyc_Absyn_seq_stmt(
_tmp496,_tmp495,0),0);return Cyc_Toc_stmt_exp_r(_tmp499);}else{struct Cyc_Absyn_Exp*
_tmp49A=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp490,f,0),Cyc_Toc_tag_sp,0);struct
Cyc_Absyn_Exp*_tmp49B=Cyc_Absyn_neq_exp(_tmp49A,_tmp491,0);struct Cyc_Absyn_Exp*
_tmp49C=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp490,f,0),Cyc_Toc_val_sp,0);struct
Cyc_Absyn_Stmt*_tmp49D=Cyc_Absyn_exp_stmt(_tmp49C,0);struct Cyc_Absyn_Stmt*
_tmp49E=Cyc_Absyn_ifthenelse_stmt(_tmp49B,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp49F=Cyc_Absyn_declare_stmt(_tmp48F,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp49E,_tmp49D,0),0);return Cyc_Toc_stmt_exp_r(
_tmp49F);}};}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read){void*_tmp4A0=e->r;struct Cyc_Absyn_Exp*
_tmp4A2;struct Cyc_Absyn_Exp*_tmp4A4;struct _dyneither_ptr*_tmp4A5;int _tmp4A6;int*
_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A9;struct _dyneither_ptr*_tmp4AA;int _tmp4AB;int*
_tmp4AC;_LL20A: {struct Cyc_Absyn_Cast_e_struct*_tmp4A1=(struct Cyc_Absyn_Cast_e_struct*)
_tmp4A0;if(_tmp4A1->tag != 15)goto _LL20C;else{_tmp4A2=_tmp4A1->f2;}}_LL20B: {
const char*_tmpBE2;void*_tmpBE1;(_tmpBE1=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE2="cast on lhs!",
_tag_dyneither(_tmpBE2,sizeof(char),13))),_tag_dyneither(_tmpBE1,sizeof(void*),0)));}
_LL20C: {struct Cyc_Absyn_AggrMember_e_struct*_tmp4A3=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp4A0;if(_tmp4A3->tag != 22)goto _LL20E;else{_tmp4A4=_tmp4A3->f1;_tmp4A5=_tmp4A3->f2;
_tmp4A6=_tmp4A3->f4;_tmp4A7=(int*)& _tmp4A3->f4;}}_LL20D: {void*_tmp4AF=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4A4->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp4B1;union Cyc_Absyn_AggrInfoU _tmp4B2;_LL213: {struct Cyc_Absyn_AggrType_struct*
_tmp4B0=(struct Cyc_Absyn_AggrType_struct*)_tmp4AF;if(_tmp4B0->tag != 12)goto
_LL215;else{_tmp4B1=_tmp4B0->f1;_tmp4B2=_tmp4B1.aggr_info;}}_LL214: {struct Cyc_Absyn_Aggrdecl*
_tmp4B3=Cyc_Absyn_get_known_aggrdecl(_tmp4B2);*f_tag=Cyc_Toc_get_member_offset(
_tmp4B3,_tmp4A5);{const char*_tmpBE7;void*_tmpBE6[2];struct Cyc_String_pa_struct
_tmpBE5;struct Cyc_String_pa_struct _tmpBE4;struct _dyneither_ptr str=(struct
_dyneither_ptr)((_tmpBE4.tag=0,((_tmpBE4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp4A5),((_tmpBE5.tag=0,((_tmpBE5.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp4B3->name).f2),((_tmpBE6[0]=& _tmpBE5,((_tmpBE6[1]=&
_tmpBE4,Cyc_aprintf(((_tmpBE7="_union_%s_%s",_tag_dyneither(_tmpBE7,sizeof(char),
13))),_tag_dyneither(_tmpBE6,sizeof(void*),2))))))))))))));{struct _dyneither_ptr*
_tmpBE8;*tagged_member_type=Cyc_Absyn_strct(((_tmpBE8=_cycalloc(sizeof(*_tmpBE8)),((
_tmpBE8[0]=str,_tmpBE8)))));}if(clear_read)*_tmp4A7=0;return((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4B3->impl))->tagged;};}_LL215:;_LL216: return 0;_LL212:;}_LL20E: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp4A8=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp4A0;if(_tmp4A8->tag != 23)goto _LL210;else{_tmp4A9=_tmp4A8->f1;_tmp4AA=_tmp4A8->f2;
_tmp4AB=_tmp4A8->f4;_tmp4AC=(int*)& _tmp4A8->f4;}}_LL20F: {void*_tmp4B9=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4A9->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp4BB;void*_tmp4BC;_LL218: {struct Cyc_Absyn_PointerType_struct*_tmp4BA=(struct
Cyc_Absyn_PointerType_struct*)_tmp4B9;if(_tmp4BA->tag != 5)goto _LL21A;else{
_tmp4BB=_tmp4BA->f1;_tmp4BC=_tmp4BB.elt_typ;}}_LL219: {void*_tmp4BD=Cyc_Tcutil_compress(
_tmp4BC);struct Cyc_Absyn_AggrInfo _tmp4BF;union Cyc_Absyn_AggrInfoU _tmp4C0;_LL21D: {
struct Cyc_Absyn_AggrType_struct*_tmp4BE=(struct Cyc_Absyn_AggrType_struct*)
_tmp4BD;if(_tmp4BE->tag != 12)goto _LL21F;else{_tmp4BF=_tmp4BE->f1;_tmp4C0=_tmp4BF.aggr_info;}}
_LL21E: {struct Cyc_Absyn_Aggrdecl*_tmp4C1=Cyc_Absyn_get_known_aggrdecl(_tmp4C0);*
f_tag=Cyc_Toc_get_member_offset(_tmp4C1,_tmp4AA);{const char*_tmpBED;void*_tmpBEC[
2];struct Cyc_String_pa_struct _tmpBEB;struct Cyc_String_pa_struct _tmpBEA;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpBEA.tag=0,((_tmpBEA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp4AA),((_tmpBEB.tag=0,((_tmpBEB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4C1->name).f2),((_tmpBEC[0]=&
_tmpBEB,((_tmpBEC[1]=& _tmpBEA,Cyc_aprintf(((_tmpBED="_union_%s_%s",
_tag_dyneither(_tmpBED,sizeof(char),13))),_tag_dyneither(_tmpBEC,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpBEE;*tagged_member_type=Cyc_Absyn_strct(((_tmpBEE=
_cycalloc(sizeof(*_tmpBEE)),((_tmpBEE[0]=str,_tmpBEE)))));}if(clear_read)*
_tmp4AC=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4C1->impl))->tagged;};}
_LL21F:;_LL220: return 0;_LL21C:;}_LL21A:;_LL21B: return 0;_LL217:;}_LL210:;_LL211:
return 0;_LL209:;}void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*
e,int tag,void*mem_type){struct _tuple0*_tmp4C7=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
temp_exp=Cyc_Absyn_var_exp(_tmp4C7,0);struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(
tag,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,
0),0);struct Cyc_Absyn_Exp*_tmp4C8=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_ifthenelse_stmt(_tmp4C8,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4C7,Cyc_Absyn_cstar_typ(
mem_type,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(
e)),Cyc_Absyn_seq_stmt(s2,s3,0),0);e->r=Cyc_Toc_stmt_exp_r(s1);}static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type){struct _tuple0*_tmp4C9=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4C9,0);struct Cyc_Absyn_Exp*
temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*
temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 
0)s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*
_tmp4CA=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp4CA,
Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=
Cyc_Absyn_declare_stmt(_tmp4C9,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(e1),Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple19{struct _tuple0*f1;void*f2;struct
Cyc_Absyn_Exp*f3;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*
f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void _tmpC41(unsigned int*_tmpC40,unsigned int*_tmpC3F,struct _tuple0***
_tmpC3D){for(*_tmpC40=0;*_tmpC40 < *_tmpC3F;(*_tmpC40)++){(*_tmpC3D)[*_tmpC40]=
Cyc_Toc_temp_var();}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){void*_tmp4CB=e->r;if(e->topt == 0){const char*_tmpBF2;void*_tmpBF1[1];struct Cyc_String_pa_struct
_tmpBF0;(_tmpBF0.tag=0,((_tmpBF0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpBF1[0]=& _tmpBF0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF2="exp_to_c: no type for %s",
_tag_dyneither(_tmpBF2,sizeof(char),25))),_tag_dyneither(_tmpBF1,sizeof(void*),1)))))));}{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp4CF=
_tmp4CB;union Cyc_Absyn_Cnst _tmp4D1;int _tmp4D2;struct _tuple0*_tmp4D5;void*_tmp4D6;
struct _tuple0*_tmp4D8;enum Cyc_Absyn_Primop _tmp4DA;struct Cyc_List_List*_tmp4DB;
struct Cyc_Absyn_Exp*_tmp4DD;enum Cyc_Absyn_Incrementor _tmp4DE;struct Cyc_Absyn_Exp*
_tmp4E0;struct Cyc_Core_Opt*_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E2;struct Cyc_Absyn_Exp*
_tmp4E4;struct Cyc_Absyn_Exp*_tmp4E5;struct Cyc_Absyn_Exp*_tmp4E6;struct Cyc_Absyn_Exp*
_tmp4E8;struct Cyc_Absyn_Exp*_tmp4E9;struct Cyc_Absyn_Exp*_tmp4EB;struct Cyc_Absyn_Exp*
_tmp4EC;struct Cyc_Absyn_Exp*_tmp4EE;struct Cyc_Absyn_Exp*_tmp4EF;struct Cyc_Absyn_Exp*
_tmp4F1;struct Cyc_List_List*_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F4;struct Cyc_List_List*
_tmp4F5;struct Cyc_Absyn_VarargCallInfo*_tmp4F6;struct Cyc_Absyn_Exp*_tmp4F8;
struct Cyc_List_List*_tmp4F9;struct Cyc_Absyn_VarargCallInfo*_tmp4FA;struct Cyc_Absyn_VarargCallInfo
_tmp4FB;int _tmp4FC;struct Cyc_List_List*_tmp4FD;struct Cyc_Absyn_VarargInfo*
_tmp4FE;struct Cyc_Absyn_Exp*_tmp500;struct Cyc_Absyn_Exp*_tmp502;struct Cyc_Absyn_Exp*
_tmp504;struct Cyc_List_List*_tmp505;void*_tmp507;void**_tmp508;struct Cyc_Absyn_Exp*
_tmp509;int _tmp50A;enum Cyc_Absyn_Coercion _tmp50B;struct Cyc_Absyn_Exp*_tmp50D;
struct Cyc_Absyn_Exp*_tmp50F;struct Cyc_Absyn_Exp*_tmp510;struct Cyc_Absyn_Exp*
_tmp512;void*_tmp514;void*_tmp516;void*_tmp517;struct _dyneither_ptr*_tmp519;void*
_tmp51B;void*_tmp51C;unsigned int _tmp51E;struct Cyc_Absyn_Exp*_tmp520;struct Cyc_Absyn_Exp*
_tmp522;struct _dyneither_ptr*_tmp523;int _tmp524;int _tmp525;struct Cyc_Absyn_Exp*
_tmp527;struct _dyneither_ptr*_tmp528;int _tmp529;int _tmp52A;struct Cyc_Absyn_Exp*
_tmp52C;struct Cyc_Absyn_Exp*_tmp52D;struct Cyc_List_List*_tmp52F;struct Cyc_List_List*
_tmp531;struct Cyc_Absyn_Vardecl*_tmp533;struct Cyc_Absyn_Exp*_tmp534;struct Cyc_Absyn_Exp*
_tmp535;int _tmp536;struct _tuple0*_tmp538;struct Cyc_List_List*_tmp539;struct Cyc_List_List*
_tmp53A;struct Cyc_Absyn_Aggrdecl*_tmp53B;void*_tmp53D;struct Cyc_List_List*
_tmp53E;struct Cyc_List_List*_tmp540;struct Cyc_Absyn_Datatypedecl*_tmp541;struct
Cyc_Absyn_Datatypefield*_tmp542;struct Cyc_Absyn_MallocInfo _tmp546;int _tmp547;
struct Cyc_Absyn_Exp*_tmp548;void**_tmp549;struct Cyc_Absyn_Exp*_tmp54A;int _tmp54B;
struct Cyc_Absyn_Exp*_tmp54D;struct Cyc_Absyn_Exp*_tmp54E;struct Cyc_Absyn_Exp*
_tmp550;struct _dyneither_ptr*_tmp551;struct Cyc_Absyn_Stmt*_tmp553;_LL222: {
struct Cyc_Absyn_Const_e_struct*_tmp4D0=(struct Cyc_Absyn_Const_e_struct*)_tmp4CF;
if(_tmp4D0->tag != 0)goto _LL224;else{_tmp4D1=_tmp4D0->f1;if((_tmp4D1.Null_c).tag
!= 1)goto _LL224;_tmp4D2=(int)(_tmp4D1.Null_c).val;}}_LL223: {struct Cyc_Absyn_Exp*
_tmp558=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp558,
_tmp558))->r;else{struct Cyc_Absyn_Exp*_tmpBF3[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((
_tmpBF3[2]=_tmp558,((_tmpBF3[1]=_tmp558,((_tmpBF3[0]=_tmp558,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF3,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL221;}_LL224: {struct Cyc_Absyn_Const_e_struct*
_tmp4D3=(struct Cyc_Absyn_Const_e_struct*)_tmp4CF;if(_tmp4D3->tag != 0)goto _LL226;}
_LL225: goto _LL221;_LL226: {struct Cyc_Absyn_Var_e_struct*_tmp4D4=(struct Cyc_Absyn_Var_e_struct*)
_tmp4CF;if(_tmp4D4->tag != 1)goto _LL228;else{_tmp4D5=_tmp4D4->f1;_tmp4D6=(void*)
_tmp4D4->f2;}}_LL227:{struct _handler_cons _tmp55A;_push_handler(& _tmp55A);{int
_tmp55C=0;if(setjmp(_tmp55A.handler))_tmp55C=1;if(!_tmp55C){e->r=(Cyc_Toc_lookup_varmap(
nv,_tmp4D5))->r;;_pop_handler();}else{void*_tmp55B=(void*)_exn_thrown;void*
_tmp55E=_tmp55B;_LL27B: {struct Cyc_Dict_Absent_struct*_tmp55F=(struct Cyc_Dict_Absent_struct*)
_tmp55E;if(_tmp55F->tag != Cyc_Dict_Absent)goto _LL27D;}_LL27C: {const char*_tmpBF7;
void*_tmpBF6[1];struct Cyc_String_pa_struct _tmpBF5;(_tmpBF5.tag=0,((_tmpBF5.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4D5)),((
_tmpBF6[0]=& _tmpBF5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBF7="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpBF7,sizeof(char),
32))),_tag_dyneither(_tmpBF6,sizeof(void*),1)))))));}_LL27D:;_LL27E:(void)_throw(
_tmp55E);_LL27A:;}};}goto _LL221;_LL228: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp4D7=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp4CF;if(_tmp4D7->tag != 2)goto
_LL22A;else{_tmp4D8=_tmp4D7->f1;}}_LL229: {const char*_tmpBFA;void*_tmpBF9;(
_tmpBF9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBFA="unknownid",_tag_dyneither(_tmpBFA,sizeof(char),10))),_tag_dyneither(
_tmpBF9,sizeof(void*),0)));}_LL22A: {struct Cyc_Absyn_Primop_e_struct*_tmp4D9=(
struct Cyc_Absyn_Primop_e_struct*)_tmp4CF;if(_tmp4D9->tag != 3)goto _LL22C;else{
_tmp4DA=_tmp4D9->f1;_tmp4DB=_tmp4D9->f2;}}_LL22B: {struct Cyc_List_List*_tmp565=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4DB);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp4DB);switch(_tmp4DA){case Cyc_Absyn_Numelts: _LL27F: {
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp4DB))->hd;{void*_tmp566=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo _tmp568;struct Cyc_Absyn_Exp*
_tmp569;struct Cyc_Absyn_PtrInfo _tmp56B;void*_tmp56C;struct Cyc_Absyn_PtrAtts
_tmp56D;union Cyc_Absyn_Constraint*_tmp56E;union Cyc_Absyn_Constraint*_tmp56F;
union Cyc_Absyn_Constraint*_tmp570;_LL282: {struct Cyc_Absyn_ArrayType_struct*
_tmp567=(struct Cyc_Absyn_ArrayType_struct*)_tmp566;if(_tmp567->tag != 9)goto
_LL284;else{_tmp568=_tmp567->f1;_tmp569=_tmp568.num_elts;}}_LL283: if(!Cyc_Evexp_c_can_eval((
struct Cyc_Absyn_Exp*)_check_null(_tmp569))){const char*_tmpBFD;void*_tmpBFC;(
_tmpBFC=0,Cyc_Tcutil_terr(e->loc,((_tmpBFD="can't calculate numelts",
_tag_dyneither(_tmpBFD,sizeof(char),24))),_tag_dyneither(_tmpBFC,sizeof(void*),0)));}
e->r=_tmp569->r;goto _LL281;_LL284: {struct Cyc_Absyn_PointerType_struct*_tmp56A=(
struct Cyc_Absyn_PointerType_struct*)_tmp566;if(_tmp56A->tag != 5)goto _LL286;else{
_tmp56B=_tmp56A->f1;_tmp56C=_tmp56B.elt_typ;_tmp56D=_tmp56B.ptr_atts;_tmp56E=
_tmp56D.nullable;_tmp56F=_tmp56D.bounds;_tmp570=_tmp56D.zero_term;}}_LL285:{void*
_tmp573=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp56F);struct Cyc_Absyn_Exp*_tmp576;_LL289: {struct Cyc_Absyn_DynEither_b_struct*
_tmp574=(struct Cyc_Absyn_DynEither_b_struct*)_tmp573;if(_tmp574->tag != 0)goto
_LL28B;}_LL28A:{struct Cyc_Absyn_Exp*_tmpBFE[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,((
_tmpBFE[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp56C),0),((_tmpBFE[0]=(
struct Cyc_Absyn_Exp*)_tmp4DB->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpBFE,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL288;_LL28B: {struct Cyc_Absyn_Upper_b_struct*_tmp575=(struct Cyc_Absyn_Upper_b_struct*)
_tmp573;if(_tmp575->tag != 1)goto _LL288;else{_tmp576=_tmp575->f1;}}_LL28C: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp570)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp4DB->hd);struct Cyc_Absyn_Exp*_tmpBFF[2];e->r=Cyc_Toc_fncall_exp_r(
function_e,((_tmpBFF[1]=_tmp576,((_tmpBFF[0]=(struct Cyc_Absyn_Exp*)_tmp4DB->hd,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpBFF,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp56E)){if(!Cyc_Evexp_c_can_eval(_tmp576)){const
char*_tmpC02;void*_tmpC01;(_tmpC01=0,Cyc_Tcutil_terr(e->loc,((_tmpC02="can't calculate numelts",
_tag_dyneither(_tmpC02,sizeof(char),24))),_tag_dyneither(_tmpC01,sizeof(void*),0)));}
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp576,Cyc_Absyn_uint_exp(0,0));}else{e->r=
_tmp576->r;goto _LL288;}}goto _LL288;_LL288:;}goto _LL281;_LL286:;_LL287: {const
char*_tmpC07;void*_tmpC06[2];struct Cyc_String_pa_struct _tmpC05;struct Cyc_String_pa_struct
_tmpC04;(_tmpC04.tag=0,((_tmpC04.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((
_tmpC05.tag=0,((_tmpC05.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmpC06[0]=& _tmpC05,((
_tmpC06[1]=& _tmpC04,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC07="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpC07,
sizeof(char),41))),_tag_dyneither(_tmpC06,sizeof(void*),2)))))))))))));}_LL281:;}
break;}case Cyc_Absyn_Plus: _LL280:{void*_tmp57F=Cyc_Tcutil_compress((void*)((
struct Cyc_List_List*)_check_null(_tmp565))->hd);struct Cyc_Absyn_PtrInfo _tmp581;
void*_tmp582;struct Cyc_Absyn_PtrAtts _tmp583;union Cyc_Absyn_Constraint*_tmp584;
union Cyc_Absyn_Constraint*_tmp585;_LL28F: {struct Cyc_Absyn_PointerType_struct*
_tmp580=(struct Cyc_Absyn_PointerType_struct*)_tmp57F;if(_tmp580->tag != 5)goto
_LL291;else{_tmp581=_tmp580->f1;_tmp582=_tmp581.elt_typ;_tmp583=_tmp581.ptr_atts;
_tmp584=_tmp583.bounds;_tmp585=_tmp583.zero_term;}}_LL290:{void*_tmp586=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp584);struct Cyc_Absyn_Exp*_tmp589;_LL294: {struct Cyc_Absyn_DynEither_b_struct*
_tmp587=(struct Cyc_Absyn_DynEither_b_struct*)_tmp586;if(_tmp587->tag != 0)goto
_LL296;}_LL295: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp4DB))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp4DB->tl))->hd;{struct Cyc_Absyn_Exp*_tmpC08[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpC08[2]=e2,((_tmpC08[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp582),0),((_tmpC08[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC08,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}goto _LL293;}_LL296: {struct Cyc_Absyn_Upper_b_struct*_tmp588=(struct
Cyc_Absyn_Upper_b_struct*)_tmp586;if(_tmp588->tag != 1)goto _LL293;else{_tmp589=
_tmp588->f1;}}_LL297: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp585)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp4DB))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp4DB->tl))->hd;struct Cyc_Absyn_Exp*_tmpC09[3];e->r=(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
e1),((_tmpC09[2]=e2,((_tmpC09[1]=_tmp589,((_tmpC09[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC09,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r;}goto _LL293;_LL293:;}goto _LL28E;_LL291:;_LL292: goto _LL28E;
_LL28E:;}break;case Cyc_Absyn_Minus: _LL28D: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp565))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4DB))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4DB->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp565->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{struct Cyc_Absyn_Exp*_tmpC0A[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpC0A[2]=Cyc_Absyn_prim1_exp(
Cyc_Absyn_Minus,e2,0),((_tmpC0A[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpC0A[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC0A,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}break;}case Cyc_Absyn_Eq:
_LL298: goto _LL299;case Cyc_Absyn_Neq: _LL299: goto _LL29A;case Cyc_Absyn_Gt: _LL29A:
goto _LL29B;case Cyc_Absyn_Gte: _LL29B: goto _LL29C;case Cyc_Absyn_Lt: _LL29C: goto
_LL29D;case Cyc_Absyn_Lte: _LL29D: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4DB))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4DB->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp565))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp565->tl))->hd;void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& elt_typ))e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,
0));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp,0));break;}default: _LL29E: break;}goto _LL221;}_LL22C: {
struct Cyc_Absyn_Increment_e_struct*_tmp4DC=(struct Cyc_Absyn_Increment_e_struct*)
_tmp4CF;if(_tmp4DC->tag != 5)goto _LL22E;else{_tmp4DD=_tmp4DC->f1;_tmp4DE=_tmp4DC->f2;}}
_LL22D: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4DD->topt))->v;
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;const char*_tmpC0B;struct _dyneither_ptr incr_str=(_tmpC0B="increment",
_tag_dyneither(_tmpC0B,sizeof(char),10));if(_tmp4DE == Cyc_Absyn_PreDec  || 
_tmp4DE == Cyc_Absyn_PostDec){const char*_tmpC0C;incr_str=((_tmpC0C="decrement",
_tag_dyneither(_tmpC0C,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp4DD,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpC10;void*_tmpC0F[1];
struct Cyc_String_pa_struct _tmpC0E;(_tmpC0E.tag=0,((_tmpC0E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC0F[0]=& _tmpC0E,Cyc_Tcutil_terr(
e->loc,((_tmpC10="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpC10,sizeof(char),74))),_tag_dyneither(_tmpC0F,sizeof(void*),1)))))));}{
const char*_tmpC13;void*_tmpC12;(_tmpC12=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC13="in-place inc/dec on zero-term",
_tag_dyneither(_tmpC13,sizeof(char),30))),_tag_dyneither(_tmpC12,sizeof(void*),0)));};}{
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp4DD,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp593=Cyc_Absyn_signed_int_exp(
1,0);{struct Cyc_Core_Opt*_tmpC14;_tmp593->topt=((_tmpC14=_cycalloc(sizeof(*
_tmpC14)),((_tmpC14->v=Cyc_Absyn_sint_typ,_tmpC14))));}switch(_tmp4DE){case Cyc_Absyn_PreInc:
_LL2A0:{struct Cyc_Absyn_AssignOp_e_struct _tmpC1A;struct Cyc_Core_Opt*_tmpC19;
struct Cyc_Absyn_AssignOp_e_struct*_tmpC18;e->r=(void*)((_tmpC18=_cycalloc(
sizeof(*_tmpC18)),((_tmpC18[0]=((_tmpC1A.tag=4,((_tmpC1A.f1=_tmp4DD,((_tmpC1A.f2=((
_tmpC19=_cycalloc_atomic(sizeof(*_tmpC19)),((_tmpC19->v=(void*)Cyc_Absyn_Plus,
_tmpC19)))),((_tmpC1A.f3=_tmp593,_tmpC1A)))))))),_tmpC18))));}Cyc_Toc_exp_to_c(
nv,e);return;case Cyc_Absyn_PreDec: _LL2A1:{struct Cyc_Absyn_AssignOp_e_struct
_tmpC20;struct Cyc_Core_Opt*_tmpC1F;struct Cyc_Absyn_AssignOp_e_struct*_tmpC1E;e->r=(
void*)((_tmpC1E=_cycalloc(sizeof(*_tmpC1E)),((_tmpC1E[0]=((_tmpC20.tag=4,((
_tmpC20.f1=_tmp4DD,((_tmpC20.f2=((_tmpC1F=_cycalloc_atomic(sizeof(*_tmpC1F)),((
_tmpC1F->v=(void*)Cyc_Absyn_Minus,_tmpC1F)))),((_tmpC20.f3=_tmp593,_tmpC20)))))))),
_tmpC1E))));}Cyc_Toc_exp_to_c(nv,e);return;default: _LL2A2:{const char*_tmpC24;
void*_tmpC23[1];struct Cyc_String_pa_struct _tmpC22;(_tmpC22.tag=0,((_tmpC22.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC23[0]=& _tmpC22,Cyc_Tcutil_terr(
e->loc,((_tmpC24="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpC24,sizeof(char),59))),_tag_dyneither(_tmpC23,sizeof(void*),1)))))));}{
const char*_tmpC27;void*_tmpC26;(_tmpC26=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC27="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpC27,sizeof(char),34))),_tag_dyneither(_tmpC26,sizeof(void*),0)));};}}
Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp4DD);Cyc_Toc_set_lhs(nv,0);{void*
elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp4DE == Cyc_Absyn_PostInc
 || _tmp4DE == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp4DE == Cyc_Absyn_PreDec  || _tmp4DE == Cyc_Absyn_PostDec)change=- 1;{struct
Cyc_Absyn_Exp*_tmpC28[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC28[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpC28[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpC28[0]=Cyc_Toc_push_address_exp(_tmp4DD),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC28,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Toc_functionSet*_tmp5A1=_tmp4DE == Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:&
Cyc_Toc__zero_arr_inplace_plus_post_functionSet;struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(
_tmp5A1,_tmp4DD);struct Cyc_Absyn_Exp*_tmpC29[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((
_tmpC29[1]=Cyc_Absyn_signed_int_exp(1,0),((_tmpC29[0]=Cyc_Toc_push_address_exp(
_tmp4DD),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC29,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(elt_typ == (
void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4DD)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(
_tmp4DD,0,Cyc_Toc_incr_lvalue,_tmp4DE);e->r=_tmp4DD->r;}}}goto _LL221;};};}_LL22E: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp4DF=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp4CF;if(_tmp4DF->tag != 4)goto _LL230;else{_tmp4E0=_tmp4DF->f1;_tmp4E1=_tmp4DF->f2;
_tmp4E2=_tmp4DF->f3;}}_LL22F: {void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4E0->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4E2->topt))->v;int f_tag=0;void*tagged_member_struct_type=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Toc_is_tagged_union_project(_tmp4E0,& f_tag,&
tagged_member_struct_type,1)){Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp4E0);
Cyc_Toc_set_lhs(nv,0);Cyc_Toc_exp_to_c(nv,_tmp4E2);e->r=Cyc_Toc_tagged_union_assignop(
_tmp4E0,e1_old_typ,_tmp4E1,_tmp4E2,e2_old_typ,f_tag,tagged_member_struct_type);
return;}{void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4E0,& ptr_type,&
is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4E0,_tmp4E1,
_tmp4E2,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp4E0);Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp4E0);Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4E2);{int done=0;if(_tmp4E1 != 0){void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;switch((enum Cyc_Absyn_Primop)_tmp4E1->v){case Cyc_Absyn_Plus: _LL2A4:
change=_tmp4E2;break;case Cyc_Absyn_Minus: _LL2A5: change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,
_tmp4E2,0);break;default: _LL2A6: {const char*_tmpC2C;void*_tmpC2B;(_tmpC2B=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC2C="bad t ? pointer arithmetic",_tag_dyneither(_tmpC2C,sizeof(char),27))),
_tag_dyneither(_tmpC2B,sizeof(void*),0)));}}done=1;{struct Cyc_Absyn_Exp*_tmp5A6=
Cyc_Toc__dyneither_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*_tmpC2D[3];e->r=Cyc_Toc_fncall_exp_r(
_tmp5A6,((_tmpC2D[2]=change,((_tmpC2D[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpC2D[0]=Cyc_Toc_push_address_exp(_tmp4E0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2D,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))
switch((enum Cyc_Absyn_Primop)_tmp4E1->v){case Cyc_Absyn_Plus: _LL2A8: done=1;{
struct Cyc_Absyn_Exp*_tmpC2E[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4E0),((_tmpC2E[1]=_tmp4E2,((
_tmpC2E[0]=_tmp4E0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC2E,sizeof(struct Cyc_Absyn_Exp*),2)))))));}break;default:
_LL2A9: {const char*_tmpC31;void*_tmpC30;(_tmpC30=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC31="bad zero-terminated pointer arithmetic",
_tag_dyneither(_tmpC31,sizeof(char),39))),_tag_dyneither(_tmpC30,sizeof(void*),0)));}}}}
if(!done){if(e1_poly)_tmp4E2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp4E2->r,0));if(!Cyc_Absyn_is_lvalue(_tmp4E0)){{struct _tuple18 _tmpC34;struct
_tuple18*_tmpC33;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple18*),struct _tuple18*f_env))
Cyc_Toc_lvalue_assign)(_tmp4E0,0,Cyc_Toc_assignop_lvalue,((_tmpC33=_cycalloc(
sizeof(struct _tuple18)* 1),((_tmpC33[0]=((_tmpC34.f1=_tmp4E1,((_tmpC34.f2=
_tmp4E2,_tmpC34)))),_tmpC33)))));}e->r=_tmp4E0->r;}}goto _LL221;};};};}_LL230: {
struct Cyc_Absyn_Conditional_e_struct*_tmp4E3=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp4CF;if(_tmp4E3->tag != 6)goto _LL232;else{_tmp4E4=_tmp4E3->f1;_tmp4E5=_tmp4E3->f2;
_tmp4E6=_tmp4E3->f3;}}_LL231: Cyc_Toc_exp_to_c(nv,_tmp4E4);Cyc_Toc_exp_to_c(nv,
_tmp4E5);Cyc_Toc_exp_to_c(nv,_tmp4E6);goto _LL221;_LL232: {struct Cyc_Absyn_And_e_struct*
_tmp4E7=(struct Cyc_Absyn_And_e_struct*)_tmp4CF;if(_tmp4E7->tag != 7)goto _LL234;
else{_tmp4E8=_tmp4E7->f1;_tmp4E9=_tmp4E7->f2;}}_LL233: Cyc_Toc_exp_to_c(nv,
_tmp4E8);Cyc_Toc_exp_to_c(nv,_tmp4E9);goto _LL221;_LL234: {struct Cyc_Absyn_Or_e_struct*
_tmp4EA=(struct Cyc_Absyn_Or_e_struct*)_tmp4CF;if(_tmp4EA->tag != 8)goto _LL236;
else{_tmp4EB=_tmp4EA->f1;_tmp4EC=_tmp4EA->f2;}}_LL235: Cyc_Toc_exp_to_c(nv,
_tmp4EB);Cyc_Toc_exp_to_c(nv,_tmp4EC);goto _LL221;_LL236: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp4ED=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp4CF;if(_tmp4ED->tag != 9)goto _LL238;
else{_tmp4EE=_tmp4ED->f1;_tmp4EF=_tmp4ED->f2;}}_LL237: Cyc_Toc_exp_to_c(nv,
_tmp4EE);Cyc_Toc_exp_to_c(nv,_tmp4EF);goto _LL221;_LL238: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp4F0=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp4CF;if(_tmp4F0->tag != 10)goto
_LL23A;else{_tmp4F1=_tmp4F0->f1;_tmp4F2=_tmp4F0->f2;}}_LL239: _tmp4F4=_tmp4F1;
_tmp4F5=_tmp4F2;goto _LL23B;_LL23A: {struct Cyc_Absyn_FnCall_e_struct*_tmp4F3=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp4CF;if(_tmp4F3->tag != 11)goto _LL23C;else{
_tmp4F4=_tmp4F3->f1;_tmp4F5=_tmp4F3->f2;_tmp4F6=_tmp4F3->f3;if(_tmp4F6 != 0)goto
_LL23C;}}_LL23B: Cyc_Toc_exp_to_c(nv,_tmp4F4);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp4F5);goto _LL221;_LL23C: {struct Cyc_Absyn_FnCall_e_struct*
_tmp4F7=(struct Cyc_Absyn_FnCall_e_struct*)_tmp4CF;if(_tmp4F7->tag != 11)goto
_LL23E;else{_tmp4F8=_tmp4F7->f1;_tmp4F9=_tmp4F7->f2;_tmp4FA=_tmp4F7->f3;if(
_tmp4FA == 0)goto _LL23E;_tmp4FB=*_tmp4FA;_tmp4FC=_tmp4FB.num_varargs;_tmp4FD=
_tmp4FB.injectors;_tmp4FE=_tmp4FB.vai;}}_LL23D: {struct Cyc_Toc_Env _tmp5AE;struct
_RegionHandle*_tmp5AF;struct Cyc_Toc_Env*_tmp5AD=nv;_tmp5AE=*_tmp5AD;_tmp5AF=
_tmp5AE.rgn;{struct _tuple0*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=
Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp4FC,0);void*cva_type=Cyc_Toc_typ_to_c(_tmp4FE->type);void*
arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp4F9);int num_normargs=num_args - _tmp4FC;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp4F9=_tmp4F9->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4F9))->hd);{
struct Cyc_List_List*_tmpC35;new_args=((_tmpC35=_cycalloc(sizeof(*_tmpC35)),((
_tmpC35->hd=(struct Cyc_Absyn_Exp*)_tmp4F9->hd,((_tmpC35->tl=new_args,_tmpC35))))));};}}{
struct Cyc_Absyn_Exp*_tmpC38[3];struct Cyc_List_List*_tmpC37;new_args=((_tmpC37=
_cycalloc(sizeof(*_tmpC37)),((_tmpC37->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((
_tmpC38[2]=num_varargs_exp,((_tmpC38[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0),((
_tmpC38[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC38,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpC37->tl=
new_args,_tmpC37))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp4F8);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4F8,new_args,0),0);if(_tmp4FE->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp5B3=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp4FE->type));struct Cyc_Absyn_DatatypeInfo _tmp5B5;union Cyc_Absyn_DatatypeInfoU
_tmp5B6;struct Cyc_Absyn_Datatypedecl**_tmp5B7;struct Cyc_Absyn_Datatypedecl*
_tmp5B8;_LL2AC: {struct Cyc_Absyn_DatatypeType_struct*_tmp5B4=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp5B3;if(_tmp5B4->tag != 3)goto _LL2AE;else{_tmp5B5=_tmp5B4->f1;_tmp5B6=_tmp5B5.datatype_info;
if((_tmp5B6.KnownDatatype).tag != 2)goto _LL2AE;_tmp5B7=(struct Cyc_Absyn_Datatypedecl**)(
_tmp5B6.KnownDatatype).val;_tmp5B8=*_tmp5B7;}}_LL2AD: tud=_tmp5B8;goto _LL2AB;
_LL2AE:;_LL2AF: {const char*_tmpC3B;void*_tmpC3A;(_tmpC3A=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC3B="toc: unknown datatype in vararg with inject",
_tag_dyneither(_tmpC3B,sizeof(char),44))),_tag_dyneither(_tmpC3A,sizeof(void*),0)));}
_LL2AB:;}{unsigned int _tmpC40;unsigned int _tmpC3F;struct _dyneither_ptr _tmpC3E;
struct _tuple0**_tmpC3D;unsigned int _tmpC3C;struct _dyneither_ptr vs=(_tmpC3C=(
unsigned int)_tmp4FC,((_tmpC3D=(struct _tuple0**)_region_malloc(_tmp5AF,
_check_times(sizeof(struct _tuple0*),_tmpC3C)),((_tmpC3E=_tag_dyneither(_tmpC3D,
sizeof(struct _tuple0*),_tmpC3C),((((_tmpC3F=_tmpC3C,_tmpC41(& _tmpC40,& _tmpC3F,&
_tmpC3D))),_tmpC3E)))))));if(_tmp4FC != 0){struct Cyc_List_List*_tmp5BB=0;{int i=
_tmp4FC - 1;for(0;i >= 0;-- i){struct Cyc_List_List*_tmpC42;_tmp5BB=((_tmpC42=
_cycalloc(sizeof(*_tmpC42)),((_tmpC42->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct
_tuple0*),i)),0),0)),((_tmpC42->tl=_tmp5BB,_tmpC42))))));}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp5BB,0),s,0);{
int i=0;for(0;_tmp4F9 != 0;(((_tmp4F9=_tmp4F9->tl,_tmp4FD=_tmp4FD->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4F9->hd;void*arg_type=(void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct _tuple0*var=*((struct
_tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield _tmp5BE;struct
_tuple0*_tmp5BF;struct Cyc_List_List*_tmp5C0;struct Cyc_Absyn_Datatypefield*
_tmp5BD=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp4FD))->hd;_tmp5BE=*_tmp5BD;_tmp5BF=_tmp5BE.name;_tmp5C0=_tmp5BE.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)_check_null(
_tmp5C0))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp5BF),0),s,0);{const char*_tmpC43;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp5BF,((_tmpC43="_struct",_tag_dyneither(_tmpC43,
sizeof(char),8))))),0,s,0);};};}};}else{struct _tuple15*_tmpC44[3];struct Cyc_List_List*
_tmp5C2=(_tmpC44[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC44[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpC44[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC44,sizeof(struct _tuple15*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{{int i=0;for(0;_tmp4F9
!= 0;(_tmp4F9=_tmp4F9->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp4F9->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4F9->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL221;}_LL23E: {struct Cyc_Absyn_Throw_e_struct*_tmp4FF=(struct Cyc_Absyn_Throw_e_struct*)
_tmp4CF;if(_tmp4FF->tag != 12)goto _LL240;else{_tmp500=_tmp4FF->f1;}}_LL23F: Cyc_Toc_exp_to_c(
nv,_tmp500);e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(
_tmp500),0))->r;goto _LL221;_LL240: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp501=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp4CF;if(_tmp501->tag != 13)
goto _LL242;else{_tmp502=_tmp501->f1;}}_LL241: Cyc_Toc_exp_to_c(nv,_tmp502);goto
_LL221;_LL242: {struct Cyc_Absyn_Instantiate_e_struct*_tmp503=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp4CF;if(_tmp503->tag != 14)goto _LL244;else{_tmp504=_tmp503->f1;_tmp505=_tmp503->f2;}}
_LL243: Cyc_Toc_exp_to_c(nv,_tmp504);for(0;_tmp505 != 0;_tmp505=_tmp505->tl){enum 
Cyc_Absyn_KindQual _tmp5C8=(Cyc_Tcutil_typ_kind((void*)_tmp505->hd))->kind;if(
_tmp5C8 != Cyc_Absyn_EffKind  && _tmp5C8 != Cyc_Absyn_RgnKind){{void*_tmp5C9=Cyc_Tcutil_compress((
void*)_tmp505->hd);_LL2B1: {struct Cyc_Absyn_VarType_struct*_tmp5CA=(struct Cyc_Absyn_VarType_struct*)
_tmp5C9;if(_tmp5CA->tag != 2)goto _LL2B3;}_LL2B2: goto _LL2B4;_LL2B3: {struct Cyc_Absyn_DatatypeType_struct*
_tmp5CB=(struct Cyc_Absyn_DatatypeType_struct*)_tmp5C9;if(_tmp5CB->tag != 3)goto
_LL2B5;}_LL2B4: continue;_LL2B5:;_LL2B6: e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp504,0))->r;goto _LL2B0;
_LL2B0:;}break;}}goto _LL221;_LL244: {struct Cyc_Absyn_Cast_e_struct*_tmp506=(
struct Cyc_Absyn_Cast_e_struct*)_tmp4CF;if(_tmp506->tag != 15)goto _LL246;else{
_tmp507=(void**)& _tmp506->f1;_tmp508=(void**)((void**)& _tmp506->f1);_tmp509=
_tmp506->f2;_tmp50A=_tmp506->f3;_tmp50B=_tmp506->f4;}}_LL245: {void*old_t2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp509->topt))->v;void*new_typ=*_tmp508;*
_tmp508=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp509);{struct _tuple17
_tmpC45;struct _tuple17 _tmp5CD=(_tmpC45.f1=Cyc_Tcutil_compress(old_t2),((_tmpC45.f2=
Cyc_Tcutil_compress(new_typ),_tmpC45)));void*_tmp5CE;struct Cyc_Absyn_PtrInfo
_tmp5D0;void*_tmp5D1;struct Cyc_Absyn_PtrInfo _tmp5D3;void*_tmp5D4;struct Cyc_Absyn_PtrInfo
_tmp5D6;void*_tmp5D7;_LL2B8: _tmp5CE=_tmp5CD.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp5CF=(struct Cyc_Absyn_PointerType_struct*)_tmp5CE;if(_tmp5CF->tag != 5)goto
_LL2BA;else{_tmp5D0=_tmp5CF->f1;}};_tmp5D1=_tmp5CD.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp5D2=(struct Cyc_Absyn_PointerType_struct*)_tmp5D1;if(_tmp5D2->tag != 5)goto
_LL2BA;else{_tmp5D3=_tmp5D2->f1;}};_LL2B9: {int _tmp5D9=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp5D0.ptr_atts).nullable);int _tmp5DA=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5D3.ptr_atts).nullable);
void*_tmp5DB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp5D0.ptr_atts).bounds);void*_tmp5DC=((void*(*)(void*y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5D3.ptr_atts).bounds);
int _tmp5DD=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp5D0.ptr_atts).zero_term);int _tmp5DE=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp5D3.ptr_atts).zero_term);{struct _tuple17 _tmpC46;
struct _tuple17 _tmp5E0=(_tmpC46.f1=_tmp5DB,((_tmpC46.f2=_tmp5DC,_tmpC46)));void*
_tmp5E1;struct Cyc_Absyn_Exp*_tmp5E3;void*_tmp5E4;struct Cyc_Absyn_Exp*_tmp5E6;
void*_tmp5E7;struct Cyc_Absyn_Exp*_tmp5E9;void*_tmp5EA;void*_tmp5EC;void*_tmp5EE;
struct Cyc_Absyn_Exp*_tmp5F0;void*_tmp5F1;void*_tmp5F3;_LL2BF: _tmp5E1=_tmp5E0.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp5E2=(struct Cyc_Absyn_Upper_b_struct*)_tmp5E1;
if(_tmp5E2->tag != 1)goto _LL2C1;else{_tmp5E3=_tmp5E2->f1;}};_tmp5E4=_tmp5E0.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp5E5=(struct Cyc_Absyn_Upper_b_struct*)_tmp5E4;
if(_tmp5E5->tag != 1)goto _LL2C1;else{_tmp5E6=_tmp5E5->f1;}};_LL2C0: if((!Cyc_Evexp_c_can_eval(
_tmp5E3) || !Cyc_Evexp_c_can_eval(_tmp5E6)) && !Cyc_Evexp_same_const_exp(_tmp5E3,
_tmp5E6)){const char*_tmpC49;void*_tmpC48;(_tmpC48=0,Cyc_Tcutil_terr(e->loc,((
_tmpC49="can't validate cast due to potential size differences",_tag_dyneither(
_tmpC49,sizeof(char),54))),_tag_dyneither(_tmpC48,sizeof(void*),0)));}if(_tmp5D9
 && !_tmp5DA){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC4C;void*_tmpC4B;(
_tmpC4B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC4C="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC4C,
sizeof(char),44))),_tag_dyneither(_tmpC4B,sizeof(void*),0)));}if(_tmp50B != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC50;void*_tmpC4F[1];struct Cyc_String_pa_struct _tmpC4E;(_tmpC4E.tag=
0,((_tmpC4E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpC4F[0]=& _tmpC4E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpC50="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpC50,sizeof(char),41))),_tag_dyneither(_tmpC4F,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp509);if(do_null_check){if(!_tmp50A){
const char*_tmpC53;void*_tmpC52;(_tmpC52=0,Cyc_Tcutil_warn(e->loc,((_tmpC53="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpC53,sizeof(char),58))),_tag_dyneither(_tmpC52,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC54;e->r=Cyc_Toc_cast_it_r(*_tmp508,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC54=_cycalloc(sizeof(*_tmpC54)),((_tmpC54->hd=
_tmp509,((_tmpC54->tl=0,_tmpC54)))))),0));};}};}goto _LL2BE;_LL2C1: _tmp5E7=
_tmp5E0.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp5E8=(struct Cyc_Absyn_Upper_b_struct*)
_tmp5E7;if(_tmp5E8->tag != 1)goto _LL2C3;else{_tmp5E9=_tmp5E8->f1;}};_tmp5EA=
_tmp5E0.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5EB=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5EA;if(_tmp5EB->tag != 0)goto _LL2C3;};_LL2C2: if(!Cyc_Evexp_c_can_eval(_tmp5E9)){
const char*_tmpC57;void*_tmpC56;(_tmpC56=0,Cyc_Tcutil_terr(e->loc,((_tmpC57="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC57,sizeof(char),71))),_tag_dyneither(_tmpC56,sizeof(void*),0)));}
if(_tmp50B == Cyc_Absyn_NonNull_to_Null){const char*_tmpC5B;void*_tmpC5A[1];struct
Cyc_String_pa_struct _tmpC59;(_tmpC59.tag=0,((_tmpC59.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC5A[0]=& _tmpC59,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC5B="conversion mis-classified as null-check: %s",
_tag_dyneither(_tmpC5B,sizeof(char),44))),_tag_dyneither(_tmpC5A,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){if((_tmp5DD  && !(_tmp5D3.elt_tq).real_const) && !
_tmp5DE)_tmp5E9=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp5E9,Cyc_Absyn_uint_exp(1,
0),0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp5E9,_tmp509))->r;}else{
struct Cyc_Absyn_Exp*_tmp604=Cyc_Toc__tag_dyneither_e;if(_tmp5DD){struct _tuple0*
_tmp605=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp606=Cyc_Absyn_var_exp(_tmp605,
0);struct Cyc_Absyn_Exp*arg3;{void*_tmp607=_tmp509->r;union Cyc_Absyn_Cnst _tmp609;
struct _dyneither_ptr _tmp60A;_LL2C8: {struct Cyc_Absyn_Const_e_struct*_tmp608=(
struct Cyc_Absyn_Const_e_struct*)_tmp607;if(_tmp608->tag != 0)goto _LL2CA;else{
_tmp609=_tmp608->f1;if((_tmp609.String_c).tag != 7)goto _LL2CA;_tmp60A=(struct
_dyneither_ptr)(_tmp609.String_c).val;}}_LL2C9: arg3=_tmp5E9;goto _LL2C7;_LL2CA:;
_LL2CB:{struct Cyc_Absyn_Exp*_tmpC5C[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp509),((_tmpC5C[1]=_tmp5E9,((_tmpC5C[0]=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp606),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5C,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);}goto _LL2C7;_LL2C7:;}if(!_tmp5DE  && !(_tmp5D3.elt_tq).real_const)arg3=
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*
_tmp60C=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5D3.elt_typ),0);struct Cyc_Absyn_Exp*
_tmpC5D[3];struct Cyc_Absyn_Exp*_tmp60D=Cyc_Absyn_fncall_exp(_tmp604,((_tmpC5D[2]=
arg3,((_tmpC5D[1]=_tmp60C,((_tmpC5D[0]=_tmp606,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5D,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);struct Cyc_Absyn_Stmt*_tmp60E=Cyc_Absyn_exp_stmt(_tmp60D,0);_tmp60E=
Cyc_Absyn_declare_stmt(_tmp605,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)
_tmp509,_tmp60E,0);e->r=Cyc_Toc_stmt_exp_r(_tmp60E);};}else{struct Cyc_Absyn_Exp*
_tmpC5E[3];e->r=Cyc_Toc_fncall_exp_r(_tmp604,((_tmpC5E[2]=_tmp5E9,((_tmpC5E[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5D3.elt_typ),0),((_tmpC5E[0]=
_tmp509,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC5E,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2BE;
_LL2C3: _tmp5EC=_tmp5E0.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp5ED=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5EC;if(_tmp5ED->tag != 0)goto _LL2C5;};_tmp5EE=_tmp5E0.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp5EF=(struct Cyc_Absyn_Upper_b_struct*)_tmp5EE;if(_tmp5EF->tag != 1)goto _LL2C5;
else{_tmp5F0=_tmp5EF->f1;}};_LL2C4: if(!Cyc_Evexp_c_can_eval(_tmp5F0)){const char*
_tmpC61;void*_tmpC60;(_tmpC60=0,Cyc_Tcutil_terr(e->loc,((_tmpC61="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC61,sizeof(char),71))),_tag_dyneither(_tmpC60,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC64;void*_tmpC63;(_tmpC63=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC64="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpC64,sizeof(char),45))),_tag_dyneither(_tmpC63,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp615=_tmp5F0;if(_tmp5DD  && !_tmp5DE)_tmp615=Cyc_Absyn_add_exp(
_tmp5F0,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp616=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC65[3];struct Cyc_Absyn_Exp*_tmp617=Cyc_Absyn_fncall_exp(
_tmp616,((_tmpC65[2]=_tmp615,((_tmpC65[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp5D0.elt_typ),0),((_tmpC65[0]=_tmp509,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC65,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp5DA){struct Cyc_List_List*_tmpC66;_tmp617->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_null_e,((_tmpC66=_cycalloc(sizeof(*_tmpC66)),((_tmpC66->hd=Cyc_Absyn_copy_exp(
_tmp617),((_tmpC66->tl=0,_tmpC66)))))));}e->r=Cyc_Toc_cast_it_r(*_tmp508,_tmp617);
goto _LL2BE;};};_LL2C5: _tmp5F1=_tmp5E0.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp5F2=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5F1;if(_tmp5F2->tag != 0)goto
_LL2BE;};_tmp5F3=_tmp5E0.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5F4=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp5F3;if(_tmp5F4->tag != 0)goto _LL2BE;};_LL2C6:
DynCast: if((_tmp5DD  && !_tmp5DE) && !(_tmp5D3.elt_tq).real_const){if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpC69;void*_tmpC68;(_tmpC68=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC69="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpC69,sizeof(char),70))),_tag_dyneither(_tmpC68,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp61C=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpC6A[3];e->r=Cyc_Toc_fncall_exp_r(_tmp61C,((_tmpC6A[2]=Cyc_Absyn_uint_exp(1,0),((
_tmpC6A[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5D0.elt_typ),0),((
_tmpC6A[0]=_tmp509,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC6A,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}goto _LL2BE;
_LL2BE:;}goto _LL2B7;}_LL2BA: _tmp5D4=_tmp5CD.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp5D5=(struct Cyc_Absyn_PointerType_struct*)_tmp5D4;if(_tmp5D5->tag != 5)goto
_LL2BC;else{_tmp5D6=_tmp5D5->f1;}};_tmp5D7=_tmp5CD.f2;{struct Cyc_Absyn_IntType_struct*
_tmp5D8=(struct Cyc_Absyn_IntType_struct*)_tmp5D7;if(_tmp5D8->tag != 6)goto _LL2BC;};
_LL2BB:{void*_tmp61E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp5D6.ptr_atts).bounds);_LL2CD: {struct Cyc_Absyn_DynEither_b_struct*
_tmp61F=(struct Cyc_Absyn_DynEither_b_struct*)_tmp61E;if(_tmp61F->tag != 0)goto
_LL2CF;}_LL2CE: _tmp509->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp509->r,
_tmp509->loc),Cyc_Toc_curr_sp);goto _LL2CC;_LL2CF:;_LL2D0: goto _LL2CC;_LL2CC:;}
goto _LL2B7;_LL2BC:;_LL2BD: goto _LL2B7;_LL2B7:;}goto _LL221;}_LL246: {struct Cyc_Absyn_Address_e_struct*
_tmp50C=(struct Cyc_Absyn_Address_e_struct*)_tmp4CF;if(_tmp50C->tag != 16)goto
_LL248;else{_tmp50D=_tmp50C->f1;}}_LL247:{void*_tmp620=_tmp50D->r;struct _tuple0*
_tmp622;struct Cyc_List_List*_tmp623;struct Cyc_List_List*_tmp624;struct Cyc_List_List*
_tmp626;_LL2D2: {struct Cyc_Absyn_Aggregate_e_struct*_tmp621=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp620;if(_tmp621->tag != 29)goto _LL2D4;else{_tmp622=_tmp621->f1;_tmp623=_tmp621->f2;
_tmp624=_tmp621->f3;}}_LL2D3: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC6E;void*
_tmpC6D[1];struct Cyc_String_pa_struct _tmpC6C;(_tmpC6C.tag=0,((_tmpC6C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp50D->loc)),((
_tmpC6D[0]=& _tmpC6C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC6E="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC6E,
sizeof(char),42))),_tag_dyneither(_tmpC6D,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp50D->topt))->v,_tmp623 != 0,1,0,
_tmp624,_tmp622))->r;goto _LL2D1;_LL2D4: {struct Cyc_Absyn_Tuple_e_struct*_tmp625=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp620;if(_tmp625->tag != 25)goto _LL2D6;else{
_tmp626=_tmp625->f1;}}_LL2D5: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC72;void*
_tmpC71[1];struct Cyc_String_pa_struct _tmpC70;(_tmpC70.tag=0,((_tmpC70.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp50D->loc)),((
_tmpC71[0]=& _tmpC70,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC72="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC72,
sizeof(char),42))),_tag_dyneither(_tmpC71,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_tuple(
nv,1,0,_tmp626))->r;goto _LL2D1;_LL2D6:;_LL2D7: Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(
nv,_tmp50D);Cyc_Toc_set_lhs(nv,0);if(!Cyc_Absyn_is_lvalue(_tmp50D)){((void(*)(
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
int),int f_env))Cyc_Toc_lvalue_assign)(_tmp50D,0,Cyc_Toc_address_lvalue,1);e->r=
Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmp50D);}goto _LL2D1;_LL2D1:;}goto _LL221;_LL248: {struct Cyc_Absyn_New_e_struct*
_tmp50E=(struct Cyc_Absyn_New_e_struct*)_tmp4CF;if(_tmp50E->tag != 17)goto _LL24A;
else{_tmp50F=_tmp50E->f1;_tmp510=_tmp50E->f2;}}_LL249: if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC76;void*_tmpC75[1];struct Cyc_String_pa_struct _tmpC74;(_tmpC74.tag=
0,((_tmpC74.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
_tmp510->loc)),((_tmpC75[0]=& _tmpC74,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC76="%s: new at top-level",_tag_dyneither(
_tmpC76,sizeof(char),21))),_tag_dyneither(_tmpC75,sizeof(void*),1)))))));}{void*
_tmp630=_tmp510->r;struct Cyc_List_List*_tmp632;struct Cyc_Absyn_Vardecl*_tmp634;
struct Cyc_Absyn_Exp*_tmp635;struct Cyc_Absyn_Exp*_tmp636;int _tmp637;struct _tuple0*
_tmp639;struct Cyc_List_List*_tmp63A;struct Cyc_List_List*_tmp63B;struct Cyc_Absyn_Aggrdecl*
_tmp63C;struct Cyc_List_List*_tmp63E;_LL2D9: {struct Cyc_Absyn_Array_e_struct*
_tmp631=(struct Cyc_Absyn_Array_e_struct*)_tmp630;if(_tmp631->tag != 27)goto _LL2DB;
else{_tmp632=_tmp631->f1;}}_LL2DA: {struct _tuple0*_tmp63F=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp640=Cyc_Absyn_var_exp(_tmp63F,0);struct Cyc_Absyn_Stmt*
_tmp641=Cyc_Toc_init_array(nv,_tmp640,_tmp632,Cyc_Absyn_exp_stmt(_tmp640,0));
void*old_elt_typ;{void*_tmp642=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp644;void*_tmp645;struct Cyc_Absyn_Tqual _tmp646;struct Cyc_Absyn_PtrAtts _tmp647;
union Cyc_Absyn_Constraint*_tmp648;_LL2E4: {struct Cyc_Absyn_PointerType_struct*
_tmp643=(struct Cyc_Absyn_PointerType_struct*)_tmp642;if(_tmp643->tag != 5)goto
_LL2E6;else{_tmp644=_tmp643->f1;_tmp645=_tmp644.elt_typ;_tmp646=_tmp644.elt_tq;
_tmp647=_tmp644.ptr_atts;_tmp648=_tmp647.zero_term;}}_LL2E5: old_elt_typ=_tmp645;
goto _LL2E3;_LL2E6:;_LL2E7: {const char*_tmpC79;void*_tmpC78;old_elt_typ=((_tmpC78=
0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC79="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(
_tmpC79,sizeof(char),52))),_tag_dyneither(_tmpC78,sizeof(void*),0))));}_LL2E3:;}{
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp64B=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp64C=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp632),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp50F == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp64C);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp50F;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp64C);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp63F,_tmp64B,(struct Cyc_Absyn_Exp*)e1,_tmp641,0));goto
_LL2D8;};}_LL2DB: {struct Cyc_Absyn_Comprehension_e_struct*_tmp633=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp630;if(_tmp633->tag != 28)goto _LL2DD;else{_tmp634=_tmp633->f1;_tmp635=_tmp633->f2;
_tmp636=_tmp633->f3;_tmp637=_tmp633->f4;}}_LL2DC: {int is_dyneither_ptr=0;{void*
_tmp64D=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp64F;void*_tmp650;
struct Cyc_Absyn_Tqual _tmp651;struct Cyc_Absyn_PtrAtts _tmp652;union Cyc_Absyn_Constraint*
_tmp653;union Cyc_Absyn_Constraint*_tmp654;_LL2E9: {struct Cyc_Absyn_PointerType_struct*
_tmp64E=(struct Cyc_Absyn_PointerType_struct*)_tmp64D;if(_tmp64E->tag != 5)goto
_LL2EB;else{_tmp64F=_tmp64E->f1;_tmp650=_tmp64F.elt_typ;_tmp651=_tmp64F.elt_tq;
_tmp652=_tmp64F.ptr_atts;_tmp653=_tmp652.bounds;_tmp654=_tmp652.zero_term;}}
_LL2EA: is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp653)== (void*)& Cyc_Absyn_DynEither_b_val;goto _LL2E8;
_LL2EB:;_LL2EC: {const char*_tmpC7C;void*_tmpC7B;(_tmpC7B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC7C="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpC7C,sizeof(char),42))),_tag_dyneither(_tmpC7B,sizeof(void*),0)));}
_LL2E8:;}{struct _tuple0*max=Cyc_Toc_temp_var();struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp636->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp635);{struct Cyc_Absyn_Exp*_tmp657=
Cyc_Absyn_var_exp(max,0);if(_tmp637)_tmp657=Cyc_Absyn_add_exp(_tmp657,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp634,Cyc_Absyn_var_exp(max,0),_tmp636,_tmp637,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp659;struct _RegionHandle*_tmp65A;struct Cyc_Toc_Env*_tmp658=
nv;_tmp659=*_tmp658;_tmp65A=_tmp659.rgn;{struct _tuple19*_tmpC7F;struct Cyc_List_List*
_tmpC7E;struct Cyc_List_List*decls=(_tmpC7E=_region_malloc(_tmp65A,sizeof(*
_tmpC7E)),((_tmpC7E->hd=((_tmpC7F=_region_malloc(_tmp65A,sizeof(*_tmpC7F)),((
_tmpC7F->f1=max,((_tmpC7F->f2=Cyc_Absyn_uint_typ,((_tmpC7F->f3=(struct Cyc_Absyn_Exp*)
_tmp635,_tmpC7F)))))))),((_tmpC7E->tl=0,_tmpC7E)))));struct Cyc_Absyn_Exp*ai;if(
_tmp50F == 0  || Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*_tmpC80[2];ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((_tmpC80[1]=_tmp657,((
_tmpC80[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC80,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp50F;Cyc_Toc_exp_to_c(
nv,r);{struct Cyc_Absyn_Exp*_tmpC81[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpC81[1]=_tmp657,((_tmpC81[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC81,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct _tuple19*_tmpC84;struct Cyc_List_List*
_tmpC83;decls=((_tmpC83=_region_malloc(_tmp65A,sizeof(*_tmpC83)),((_tmpC83->hd=((
_tmpC84=_region_malloc(_tmp65A,sizeof(*_tmpC84)),((_tmpC84->f1=a,((_tmpC84->f2=
ptr_typ,((_tmpC84->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpC84)))))))),((_tmpC83->tl=
decls,_tmpC83))))));}if(is_dyneither_ptr){struct _tuple0*_tmp65F=Cyc_Toc_temp_var();
void*_tmp660=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp661=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC85[3];struct Cyc_Absyn_Exp*_tmp662=Cyc_Absyn_fncall_exp(
_tmp661,((_tmpC85[2]=_tmp657,((_tmpC85[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((
_tmpC85[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC85,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{
struct _tuple19*_tmpC88;struct Cyc_List_List*_tmpC87;decls=((_tmpC87=
_region_malloc(_tmp65A,sizeof(*_tmpC87)),((_tmpC87->hd=((_tmpC88=_region_malloc(
_tmp65A,sizeof(*_tmpC88)),((_tmpC88->f1=_tmp65F,((_tmpC88->f2=_tmp660,((_tmpC88->f3=(
struct Cyc_Absyn_Exp*)_tmp662,_tmpC88)))))))),((_tmpC87->tl=decls,_tmpC87))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp65F,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp666=decls;for(0;_tmp666 != 0;_tmp666=_tmp666->tl){struct
_tuple0*_tmp668;void*_tmp669;struct Cyc_Absyn_Exp*_tmp66A;struct _tuple19 _tmp667=*((
struct _tuple19*)_tmp666->hd);_tmp668=_tmp667.f1;_tmp669=_tmp667.f2;_tmp66A=
_tmp667.f3;s=Cyc_Absyn_declare_stmt(_tmp668,_tmp669,_tmp66A,s,0);}}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL2D8;};};};}_LL2DD: {struct Cyc_Absyn_Aggregate_e_struct*_tmp638=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp630;if(_tmp638->tag != 29)goto _LL2DF;
else{_tmp639=_tmp638->f1;_tmp63A=_tmp638->f2;_tmp63B=_tmp638->f3;_tmp63C=_tmp638->f4;}}
_LL2DE: e->r=(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp510->topt))->v,_tmp63A != 0,1,_tmp50F,_tmp63B,_tmp639))->r;goto _LL2D8;_LL2DF: {
struct Cyc_Absyn_Tuple_e_struct*_tmp63D=(struct Cyc_Absyn_Tuple_e_struct*)_tmp630;
if(_tmp63D->tag != 25)goto _LL2E1;else{_tmp63E=_tmp63D->f1;}}_LL2E0: e->r=(Cyc_Toc_init_tuple(
nv,1,_tmp50F,_tmp63E))->r;goto _LL2D8;_LL2E1:;_LL2E2: {void*old_elt_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp510->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);struct _tuple0*_tmp66D=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp66E=Cyc_Absyn_var_exp(_tmp66D,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(_tmp66E,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp50F
== 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp50F;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp66F=_tmp510->r;void*_tmp671;
struct Cyc_Absyn_Exp*_tmp672;_LL2EE: {struct Cyc_Absyn_Cast_e_struct*_tmp670=(
struct Cyc_Absyn_Cast_e_struct*)_tmp66F;if(_tmp670->tag != 15)goto _LL2F0;else{
_tmp671=(void*)_tmp670->f1;_tmp672=_tmp670->f2;}}_LL2EF:{struct _tuple17 _tmpC89;
struct _tuple17 _tmp674=(_tmpC89.f1=Cyc_Tcutil_compress(_tmp671),((_tmpC89.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp672->topt))->v),_tmpC89)));void*
_tmp675;struct Cyc_Absyn_PtrInfo _tmp677;void*_tmp678;struct Cyc_Absyn_PtrAtts
_tmp679;union Cyc_Absyn_Constraint*_tmp67A;void*_tmp67B;struct Cyc_Absyn_PtrInfo
_tmp67D;struct Cyc_Absyn_PtrAtts _tmp67E;union Cyc_Absyn_Constraint*_tmp67F;_LL2F3:
_tmp675=_tmp674.f1;{struct Cyc_Absyn_PointerType_struct*_tmp676=(struct Cyc_Absyn_PointerType_struct*)
_tmp675;if(_tmp676->tag != 5)goto _LL2F5;else{_tmp677=_tmp676->f1;_tmp678=_tmp677.elt_typ;
_tmp679=_tmp677.ptr_atts;_tmp67A=_tmp679.bounds;}};_tmp67B=_tmp674.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp67C=(struct Cyc_Absyn_PointerType_struct*)_tmp67B;if(_tmp67C->tag != 5)goto
_LL2F5;else{_tmp67D=_tmp67C->f1;_tmp67E=_tmp67D.ptr_atts;_tmp67F=_tmp67E.bounds;}};
_LL2F4:{struct _tuple17 _tmpC8A;struct _tuple17 _tmp681=(_tmpC8A.f1=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp67A),((_tmpC8A.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp67F),_tmpC8A)));void*_tmp682;void*_tmp684;struct Cyc_Absyn_Exp*
_tmp686;_LL2F8: _tmp682=_tmp681.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp683=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp682;if(_tmp683->tag != 0)goto _LL2FA;};
_tmp684=_tmp681.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp685=(struct Cyc_Absyn_Upper_b_struct*)
_tmp684;if(_tmp685->tag != 1)goto _LL2FA;else{_tmp686=_tmp685->f1;}};_LL2F9: Cyc_Toc_exp_to_c(
nv,_tmp672);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp687=Cyc_Toc__init_dyneither_ptr_e;{struct Cyc_Absyn_Exp*_tmpC8B[4];e->r=Cyc_Toc_fncall_exp_r(
_tmp687,((_tmpC8B[3]=_tmp686,((_tmpC8B[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp678),0),((_tmpC8B[1]=_tmp672,((_tmpC8B[0]=mexp,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC8B,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))));}goto _LL2F7;};_LL2FA:;_LL2FB: goto _LL2F7;_LL2F7:;}goto _LL2F2;_LL2F5:;
_LL2F6: goto _LL2F2;_LL2F2:;}goto _LL2ED;_LL2F0:;_LL2F1: goto _LL2ED;_LL2ED:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp689=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp66E),
0);struct Cyc_Absyn_Exp*_tmp68A=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,
_tmp510);_tmp689=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
_tmp66E,_tmp68A,0),_tmp510,0),_tmp689,0);{void*_tmp68B=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp66D,
_tmp68B,(struct Cyc_Absyn_Exp*)mexp,_tmp689,0));};}goto _LL2D8;};}_LL2D8:;}goto
_LL221;_LL24A: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp511=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp4CF;if(_tmp511->tag != 19)goto _LL24C;else{_tmp512=_tmp511->f1;}}_LL24B: Cyc_Toc_exp_to_c(
nv,_tmp512);goto _LL221;_LL24C: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp513=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp4CF;if(_tmp513->tag != 18)goto _LL24E;
else{_tmp514=(void*)_tmp513->f1;}}_LL24D:{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpC8E;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpC8D;e->r=(void*)((_tmpC8D=
_cycalloc(sizeof(*_tmpC8D)),((_tmpC8D[0]=((_tmpC8E.tag=18,((_tmpC8E.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp514),_tmpC8E)))),_tmpC8D))));}goto _LL221;_LL24E: {
struct Cyc_Absyn_Offsetof_e_struct*_tmp515=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp4CF;if(_tmp515->tag != 20)goto _LL250;else{_tmp516=(void*)_tmp515->f1;_tmp517=(
void*)_tmp515->f2;{struct Cyc_Absyn_StructField_struct*_tmp518=(struct Cyc_Absyn_StructField_struct*)
_tmp517;if(_tmp518->tag != 0)goto _LL250;else{_tmp519=_tmp518->f1;}};}}_LL24F:{
struct Cyc_Absyn_Offsetof_e_struct _tmpC98;struct Cyc_Absyn_StructField_struct
_tmpC97;struct Cyc_Absyn_StructField_struct*_tmpC96;struct Cyc_Absyn_Offsetof_e_struct*
_tmpC95;e->r=(void*)((_tmpC95=_cycalloc(sizeof(*_tmpC95)),((_tmpC95[0]=((_tmpC98.tag=
20,((_tmpC98.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp516),((_tmpC98.f2=(void*)((
void*)((_tmpC96=_cycalloc(sizeof(*_tmpC96)),((_tmpC96[0]=((_tmpC97.tag=0,((
_tmpC97.f1=_tmp519,_tmpC97)))),_tmpC96))))),_tmpC98)))))),_tmpC95))));}goto
_LL221;_LL250: {struct Cyc_Absyn_Offsetof_e_struct*_tmp51A=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp4CF;if(_tmp51A->tag != 20)goto _LL252;else{_tmp51B=(void*)_tmp51A->f1;_tmp51C=(
void*)_tmp51A->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp51D=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp51C;if(_tmp51D->tag != 1)goto _LL252;else{_tmp51E=_tmp51D->f1;}};}}_LL251:{
void*_tmp692=Cyc_Tcutil_compress(_tmp51B);struct Cyc_Absyn_AggrInfo _tmp694;union
Cyc_Absyn_AggrInfoU _tmp695;struct Cyc_List_List*_tmp697;_LL2FD: {struct Cyc_Absyn_AggrType_struct*
_tmp693=(struct Cyc_Absyn_AggrType_struct*)_tmp692;if(_tmp693->tag != 12)goto
_LL2FF;else{_tmp694=_tmp693->f1;_tmp695=_tmp694.aggr_info;}}_LL2FE: {struct Cyc_Absyn_Aggrdecl*
_tmp69A=Cyc_Absyn_get_known_aggrdecl(_tmp695);if(_tmp69A->impl == 0){const char*
_tmpC9B;void*_tmpC9A;(_tmpC9A=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC9B="struct fields must be known",
_tag_dyneither(_tmpC9B,sizeof(char),28))),_tag_dyneither(_tmpC9A,sizeof(void*),0)));}
_tmp697=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp69A->impl))->fields;goto
_LL300;}_LL2FF: {struct Cyc_Absyn_AnonAggrType_struct*_tmp696=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp692;if(_tmp696->tag != 13)goto _LL301;else{_tmp697=_tmp696->f2;}}_LL300: {
struct Cyc_Absyn_Aggrfield*_tmp69D=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp697,(int)_tmp51E);{struct Cyc_Absyn_Offsetof_e_struct
_tmpCA5;struct Cyc_Absyn_StructField_struct _tmpCA4;struct Cyc_Absyn_StructField_struct*
_tmpCA3;struct Cyc_Absyn_Offsetof_e_struct*_tmpCA2;e->r=(void*)((_tmpCA2=
_cycalloc(sizeof(*_tmpCA2)),((_tmpCA2[0]=((_tmpCA5.tag=20,((_tmpCA5.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp51B),((_tmpCA5.f2=(void*)((void*)((_tmpCA3=_cycalloc(
sizeof(*_tmpCA3)),((_tmpCA3[0]=((_tmpCA4.tag=0,((_tmpCA4.f1=_tmp69D->name,
_tmpCA4)))),_tmpCA3))))),_tmpCA5)))))),_tmpCA2))));}goto _LL2FC;}_LL301: {struct
Cyc_Absyn_TupleType_struct*_tmp698=(struct Cyc_Absyn_TupleType_struct*)_tmp692;
if(_tmp698->tag != 11)goto _LL303;}_LL302:{struct Cyc_Absyn_Offsetof_e_struct
_tmpCAF;struct Cyc_Absyn_StructField_struct _tmpCAE;struct Cyc_Absyn_StructField_struct*
_tmpCAD;struct Cyc_Absyn_Offsetof_e_struct*_tmpCAC;e->r=(void*)((_tmpCAC=
_cycalloc(sizeof(*_tmpCAC)),((_tmpCAC[0]=((_tmpCAF.tag=20,((_tmpCAF.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp51B),((_tmpCAF.f2=(void*)((void*)((_tmpCAD=_cycalloc(
sizeof(*_tmpCAD)),((_tmpCAD[0]=((_tmpCAE.tag=0,((_tmpCAE.f1=Cyc_Absyn_fieldname((
int)(_tmp51E + 1)),_tmpCAE)))),_tmpCAD))))),_tmpCAF)))))),_tmpCAC))));}goto _LL2FC;
_LL303: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp699=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp692;if(_tmp699->tag != 4)goto _LL305;}_LL304: if(_tmp51E == 0){struct Cyc_Absyn_Offsetof_e_struct
_tmpCB9;struct Cyc_Absyn_StructField_struct _tmpCB8;struct Cyc_Absyn_StructField_struct*
_tmpCB7;struct Cyc_Absyn_Offsetof_e_struct*_tmpCB6;e->r=(void*)((_tmpCB6=
_cycalloc(sizeof(*_tmpCB6)),((_tmpCB6[0]=((_tmpCB9.tag=20,((_tmpCB9.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp51B),((_tmpCB9.f2=(void*)((void*)((_tmpCB7=_cycalloc(
sizeof(*_tmpCB7)),((_tmpCB7[0]=((_tmpCB8.tag=0,((_tmpCB8.f1=Cyc_Toc_tag_sp,
_tmpCB8)))),_tmpCB7))))),_tmpCB9)))))),_tmpCB6))));}else{struct Cyc_Absyn_Offsetof_e_struct
_tmpCC3;struct Cyc_Absyn_StructField_struct _tmpCC2;struct Cyc_Absyn_StructField_struct*
_tmpCC1;struct Cyc_Absyn_Offsetof_e_struct*_tmpCC0;e->r=(void*)((_tmpCC0=
_cycalloc(sizeof(*_tmpCC0)),((_tmpCC0[0]=((_tmpCC3.tag=20,((_tmpCC3.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp51B),((_tmpCC3.f2=(void*)((void*)((_tmpCC1=_cycalloc(
sizeof(*_tmpCC1)),((_tmpCC1[0]=((_tmpCC2.tag=0,((_tmpCC2.f1=Cyc_Absyn_fieldname((
int)_tmp51E),_tmpCC2)))),_tmpCC1))))),_tmpCC3)))))),_tmpCC0))));}goto _LL2FC;
_LL305:;_LL306: {const char*_tmpCC6;void*_tmpCC5;(_tmpCC5=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCC6="impossible type for offsetof tuple index",
_tag_dyneither(_tmpCC6,sizeof(char),41))),_tag_dyneither(_tmpCC5,sizeof(void*),0)));}
_LL2FC:;}goto _LL221;_LL252: {struct Cyc_Absyn_Deref_e_struct*_tmp51F=(struct Cyc_Absyn_Deref_e_struct*)
_tmp4CF;if(_tmp51F->tag != 21)goto _LL254;else{_tmp520=_tmp51F->f1;}}_LL253: {int
_tmp6B0=Cyc_Toc_in_lhs(nv);Cyc_Toc_set_lhs(nv,0);{void*_tmp6B1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp520->topt))->v);{void*_tmp6B2=
_tmp6B1;struct Cyc_Absyn_PtrInfo _tmp6B4;void*_tmp6B5;struct Cyc_Absyn_Tqual _tmp6B6;
struct Cyc_Absyn_PtrAtts _tmp6B7;void*_tmp6B8;union Cyc_Absyn_Constraint*_tmp6B9;
union Cyc_Absyn_Constraint*_tmp6BA;union Cyc_Absyn_Constraint*_tmp6BB;_LL308: {
struct Cyc_Absyn_PointerType_struct*_tmp6B3=(struct Cyc_Absyn_PointerType_struct*)
_tmp6B2;if(_tmp6B3->tag != 5)goto _LL30A;else{_tmp6B4=_tmp6B3->f1;_tmp6B5=_tmp6B4.elt_typ;
_tmp6B6=_tmp6B4.elt_tq;_tmp6B7=_tmp6B4.ptr_atts;_tmp6B8=_tmp6B7.rgn;_tmp6B9=
_tmp6B7.nullable;_tmp6BA=_tmp6B7.bounds;_tmp6BB=_tmp6B7.zero_term;}}_LL309:{void*
_tmp6BC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6BA);struct Cyc_Absyn_Exp*_tmp6BE;_LL30D: {struct Cyc_Absyn_Upper_b_struct*
_tmp6BD=(struct Cyc_Absyn_Upper_b_struct*)_tmp6BC;if(_tmp6BD->tag != 1)goto _LL30F;
else{_tmp6BE=_tmp6BD->f1;}}_LL30E: {int do_null_check=Cyc_Toc_need_null_check(
_tmp520);Cyc_Toc_exp_to_c(nv,_tmp520);if(do_null_check){if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCC9;void*_tmpCC8;(_tmpCC8=0,Cyc_Tcutil_warn(e->loc,((_tmpCC9="inserted null check due to dereference",
_tag_dyneither(_tmpCC9,sizeof(char),39))),_tag_dyneither(_tmpCC8,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpCCA;_tmp520->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp6B1),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpCCA=_cycalloc(sizeof(*
_tmpCCA)),((_tmpCCA->hd=Cyc_Absyn_copy_exp(_tmp520),((_tmpCCA->tl=0,_tmpCCA)))))),
0));};}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6BB)){unsigned int _tmp6C4;int _tmp6C5;struct _tuple10 _tmp6C3=Cyc_Evexp_eval_const_uint_exp(
_tmp6BE);_tmp6C4=_tmp6C3.f1;_tmp6C5=_tmp6C3.f2;if(!_tmp6C5  || _tmp6C4 <= 0){const
char*_tmpCCD;void*_tmpCCC;(_tmpCCC=0,Cyc_Tcutil_terr(e->loc,((_tmpCCD="cannot determine dereference is in bounds",
_tag_dyneither(_tmpCCD,sizeof(char),42))),_tag_dyneither(_tmpCCC,sizeof(void*),0)));}}
goto _LL30C;}_LL30F: {struct Cyc_Absyn_DynEither_b_struct*_tmp6BF=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp6BC;if(_tmp6BF->tag != 0)goto _LL30C;}_LL310: {struct Cyc_Absyn_Exp*_tmp6C8=Cyc_Absyn_uint_exp(
0,0);{struct Cyc_Core_Opt*_tmpCCE;_tmp6C8->topt=((_tmpCCE=_cycalloc(sizeof(*
_tmpCCE)),((_tmpCCE->v=Cyc_Absyn_uint_typ,_tmpCCE))));}e->r=Cyc_Toc_subscript_exp_r(
_tmp520,_tmp6C8);Cyc_Toc_exp_to_c(nv,e);goto _LL30C;}_LL30C:;}goto _LL307;_LL30A:;
_LL30B: {const char*_tmpCD1;void*_tmpCD0;(_tmpCD0=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD1="exp_to_c: Deref: non-pointer",
_tag_dyneither(_tmpCD1,sizeof(char),29))),_tag_dyneither(_tmpCD0,sizeof(void*),0)));}
_LL307:;}Cyc_Toc_set_lhs(nv,_tmp6B0);goto _LL221;};}_LL254: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp521=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp4CF;if(_tmp521->tag != 22)goto
_LL256;else{_tmp522=_tmp521->f1;_tmp523=_tmp521->f2;_tmp524=_tmp521->f3;_tmp525=
_tmp521->f4;}}_LL255: {int is_poly=Cyc_Toc_is_poly_project(e);void*e1_cyc_type=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp522->topt))->v;Cyc_Toc_exp_to_c(nv,
_tmp522);if(_tmp524  && _tmp525)e->r=Cyc_Toc_check_tagged_union(_tmp522,Cyc_Toc_typ_to_c(
e1_cyc_type),e1_cyc_type,_tmp523,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrmember_exp);
if(is_poly)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp(e->r,0),0))->r;goto _LL221;}_LL256: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp526=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp4CF;if(_tmp526->tag != 23)goto _LL258;else{_tmp527=_tmp526->f1;_tmp528=_tmp526->f2;
_tmp529=_tmp526->f3;_tmp52A=_tmp526->f4;}}_LL257: {int _tmp6CC=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp527->topt))->v);int do_null_check=Cyc_Toc_need_null_check(_tmp527);
Cyc_Toc_exp_to_c(nv,_tmp527);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp6CE;
struct Cyc_Absyn_Tqual _tmp6CF;struct Cyc_Absyn_PtrAtts _tmp6D0;void*_tmp6D1;union
Cyc_Absyn_Constraint*_tmp6D2;union Cyc_Absyn_Constraint*_tmp6D3;union Cyc_Absyn_Constraint*
_tmp6D4;struct Cyc_Absyn_PtrInfo _tmp6CD=Cyc_Toc_get_ptr_type(e1typ);_tmp6CE=
_tmp6CD.elt_typ;_tmp6CF=_tmp6CD.elt_tq;_tmp6D0=_tmp6CD.ptr_atts;_tmp6D1=_tmp6D0.rgn;
_tmp6D2=_tmp6D0.nullable;_tmp6D3=_tmp6D0.bounds;_tmp6D4=_tmp6D0.zero_term;{void*
_tmp6D5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6D3);struct Cyc_Absyn_Exp*_tmp6D7;_LL312: {struct Cyc_Absyn_Upper_b_struct*
_tmp6D6=(struct Cyc_Absyn_Upper_b_struct*)_tmp6D5;if(_tmp6D6->tag != 1)goto _LL314;
else{_tmp6D7=_tmp6D6->f1;}}_LL313: {unsigned int _tmp6DA;int _tmp6DB;struct
_tuple10 _tmp6D9=Cyc_Evexp_eval_const_uint_exp(_tmp6D7);_tmp6DA=_tmp6D9.f1;
_tmp6DB=_tmp6D9.f2;if(_tmp6DB){if(_tmp6DA < 1){const char*_tmpCD4;void*_tmpCD3;(
_tmpCD3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpCD4="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpCD4,
sizeof(char),44))),_tag_dyneither(_tmpCD3,sizeof(void*),0)));}if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCD7;void*_tmpCD6;(_tmpCD6=0,Cyc_Tcutil_warn(
e->loc,((_tmpCD7="inserted null check due to dereference",_tag_dyneither(_tmpCD7,
sizeof(char),39))),_tag_dyneither(_tmpCD6,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCD8[1];_tmp527->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpCD8[0]=Cyc_Absyn_new_exp(_tmp527->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD8,sizeof(struct Cyc_Absyn_Exp*),
1)))),0));};}}else{if(!Cyc_Evexp_c_can_eval(_tmp6D7)){const char*_tmpCDB;void*
_tmpCDA;(_tmpCDA=0,Cyc_Tcutil_terr(e->loc,((_tmpCDB="cannot determine pointer dereference in bounds",
_tag_dyneither(_tmpCDB,sizeof(char),47))),_tag_dyneither(_tmpCDA,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpCDC[4];_tmp527->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((_tmpCDC[3]=
Cyc_Absyn_uint_exp(0,0),((_tmpCDC[2]=Cyc_Absyn_sizeoftyp_exp(_tmp6CE,0),((
_tmpCDC[1]=_tmp6D7,((_tmpCDC[0]=Cyc_Absyn_new_exp(_tmp527->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCDC,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0));};}goto _LL311;}_LL314: {struct Cyc_Absyn_DynEither_b_struct*
_tmp6D8=(struct Cyc_Absyn_DynEither_b_struct*)_tmp6D5;if(_tmp6D8->tag != 0)goto
_LL311;}_LL315: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp6CE);{struct Cyc_Absyn_Exp*
_tmpCDD[3];_tmp527->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp6CF),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_dyneither_subscript_e,((_tmpCDD[2]=Cyc_Absyn_uint_exp(0,0),((
_tmpCDD[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCDD[0]=Cyc_Absyn_new_exp(_tmp527->r,
0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCDD,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}goto _LL311;}_LL311:;}if(
_tmp529  && _tmp52A)e->r=Cyc_Toc_check_tagged_union(_tmp527,Cyc_Toc_typ_to_c(
e1typ),_tmp6CE,_tmp528,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);if(is_poly
 && _tmp52A)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp(e->r,0),0))->r;Cyc_Toc_set_lhs(nv,
_tmp6CC);goto _LL221;};};}_LL258: {struct Cyc_Absyn_Subscript_e_struct*_tmp52B=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp4CF;if(_tmp52B->tag != 24)goto _LL25A;
else{_tmp52C=_tmp52B->f1;_tmp52D=_tmp52B->f2;}}_LL259: {int _tmp6E5=Cyc_Toc_in_lhs(
nv);Cyc_Toc_set_lhs(nv,0);{void*_tmp6E6=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp52C->topt))->v);{void*_tmp6E7=_tmp6E6;struct Cyc_List_List*
_tmp6E9;struct Cyc_Absyn_PtrInfo _tmp6EB;void*_tmp6EC;struct Cyc_Absyn_Tqual _tmp6ED;
struct Cyc_Absyn_PtrAtts _tmp6EE;void*_tmp6EF;union Cyc_Absyn_Constraint*_tmp6F0;
union Cyc_Absyn_Constraint*_tmp6F1;union Cyc_Absyn_Constraint*_tmp6F2;_LL317: {
struct Cyc_Absyn_TupleType_struct*_tmp6E8=(struct Cyc_Absyn_TupleType_struct*)
_tmp6E7;if(_tmp6E8->tag != 11)goto _LL319;else{_tmp6E9=_tmp6E8->f1;}}_LL318: Cyc_Toc_exp_to_c(
nv,_tmp52C);Cyc_Toc_exp_to_c(nv,_tmp52D);{unsigned int _tmp6F4;int _tmp6F5;struct
_tuple10 _tmp6F3=Cyc_Evexp_eval_const_uint_exp(_tmp52D);_tmp6F4=_tmp6F3.f1;
_tmp6F5=_tmp6F3.f2;if(!_tmp6F5){const char*_tmpCE0;void*_tmpCDF;(_tmpCDF=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCE0="unknown tuple subscript in translation to C",
_tag_dyneither(_tmpCE0,sizeof(char),44))),_tag_dyneither(_tmpCDF,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp52C,Cyc_Absyn_fieldname((int)(_tmp6F4 + 1)));
goto _LL316;};_LL319: {struct Cyc_Absyn_PointerType_struct*_tmp6EA=(struct Cyc_Absyn_PointerType_struct*)
_tmp6E7;if(_tmp6EA->tag != 5)goto _LL31B;else{_tmp6EB=_tmp6EA->f1;_tmp6EC=_tmp6EB.elt_typ;
_tmp6ED=_tmp6EB.elt_tq;_tmp6EE=_tmp6EB.ptr_atts;_tmp6EF=_tmp6EE.rgn;_tmp6F0=
_tmp6EE.nullable;_tmp6F1=_tmp6EE.bounds;_tmp6F2=_tmp6EE.zero_term;}}_LL31A: {
struct Cyc_List_List*_tmp6F8=Cyc_Toc_get_relns(_tmp52C);int in_bnds=0;{void*
_tmp6F9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6F1);_LL31E:;_LL31F: in_bnds=Cyc_Toc_check_bounds(_tmp6E6,_tmp6F8,_tmp52C,
_tmp52D);if(Cyc_Toc_warn_bounds_checks  && !in_bnds){const char*_tmpCE4;void*
_tmpCE3[1];struct Cyc_String_pa_struct _tmpCE2;(_tmpCE2.tag=0,((_tmpCE2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCE3[0]=&
_tmpCE2,Cyc_Tcutil_warn(e->loc,((_tmpCE4="bounds check necessary for %s",
_tag_dyneither(_tmpCE4,sizeof(char),30))),_tag_dyneither(_tmpCE3,sizeof(void*),1)))))));}
_LL31D:;}Cyc_Toc_exp_to_c(nv,_tmp52C);Cyc_Toc_exp_to_c(nv,_tmp52D);++ Cyc_Toc_total_bounds_checks;{
void*_tmp6FD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp6F1);struct Cyc_Absyn_Exp*_tmp6FF;_LL321: {struct Cyc_Absyn_Upper_b_struct*
_tmp6FE=(struct Cyc_Absyn_Upper_b_struct*)_tmp6FD;if(_tmp6FE->tag != 1)goto _LL323;
else{_tmp6FF=_tmp6FE->f1;}}_LL322: {int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp6F0);void*ta1=Cyc_Toc_typ_to_c(_tmp6EC);void*ta2=
Cyc_Absyn_cstar_typ(ta1,_tmp6ED);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6F2)){if(!Cyc_Evexp_c_can_eval(_tmp6FF)){const char*_tmpCE7;void*_tmpCE6;(
_tmpCE6=0,Cyc_Tcutil_terr(e->loc,((_tmpCE7="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCE7,sizeof(char),40))),_tag_dyneither(_tmpCE6,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp52C);struct Cyc_Absyn_Exp*_tmpCE8[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpCE8[2]=_tmp52D,((_tmpCE8[1]=_tmp6FF,((
_tmpCE8[0]=_tmp52C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCE8,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp6FF)){const char*_tmpCEB;void*_tmpCEA;(
_tmpCEA=0,Cyc_Tcutil_terr(e->loc,((_tmpCEB="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCEB,sizeof(char),40))),_tag_dyneither(_tmpCEA,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCEE;void*_tmpCED;(_tmpCED=0,Cyc_Tcutil_warn(
e->loc,((_tmpCEE="inserted null check due to dereference",_tag_dyneither(_tmpCEE,
sizeof(char),39))),_tag_dyneither(_tmpCED,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCEF[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((
_tmpCEF[3]=_tmp52D,((_tmpCEF[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCEF[1]=
_tmp6FF,((_tmpCEF[0]=_tmp52C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCEF,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{if(
!Cyc_Evexp_c_can_eval(_tmp6FF)){const char*_tmpCF2;void*_tmpCF1;(_tmpCF1=0,Cyc_Tcutil_terr(
e->loc,((_tmpCF2="cannot determine subscript is in bounds",_tag_dyneither(
_tmpCF2,sizeof(char),40))),_tag_dyneither(_tmpCF1,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCF3[2];_tmp52D->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((
_tmpCF3[1]=Cyc_Absyn_copy_exp(_tmp52D),((_tmpCF3[0]=_tmp6FF,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCF3,sizeof(struct Cyc_Absyn_Exp*),
2)))))));};}}}goto _LL320;}_LL323: {struct Cyc_Absyn_DynEither_b_struct*_tmp700=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp6FD;if(_tmp700->tag != 0)goto _LL320;}
_LL324: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp6EC);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6ED),
Cyc_Absyn_aggrmember_exp(_tmp52C,Cyc_Toc_curr_sp,0)),_tmp52D);}else{struct Cyc_Absyn_Exp*
_tmp70C=Cyc_Toc__check_dyneither_subscript_e;struct Cyc_Absyn_Exp*_tmpCF4[3];e->r=
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6ED),Cyc_Absyn_fncall_exp(
_tmp70C,((_tmpCF4[2]=_tmp52D,((_tmpCF4[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((
_tmpCF4[0]=_tmp52C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCF4,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL320;}
_LL320:;}goto _LL316;}_LL31B:;_LL31C: {const char*_tmpCF7;void*_tmpCF6;(_tmpCF6=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCF7="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(
_tmpCF7,sizeof(char),49))),_tag_dyneither(_tmpCF6,sizeof(void*),0)));}_LL316:;}
Cyc_Toc_set_lhs(nv,_tmp6E5);goto _LL221;};}_LL25A: {struct Cyc_Absyn_Tuple_e_struct*
_tmp52E=(struct Cyc_Absyn_Tuple_e_struct*)_tmp4CF;if(_tmp52E->tag != 25)goto _LL25C;
else{_tmp52F=_tmp52E->f1;}}_LL25B: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(
nv,0,0,_tmp52F))->r;else{struct Cyc_List_List*_tmp710=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_tup_to_c,_tmp52F);void*_tmp711=Cyc_Toc_add_tuple_type(_tmp710);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp52F != 0;(_tmp52F=_tmp52F->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp52F->hd);{struct _tuple15*_tmpCFA;struct Cyc_List_List*
_tmpCF9;dles=((_tmpCF9=_cycalloc(sizeof(*_tmpCF9)),((_tmpCF9->hd=((_tmpCFA=
_cycalloc(sizeof(*_tmpCFA)),((_tmpCFA->f1=0,((_tmpCFA->f2=(struct Cyc_Absyn_Exp*)
_tmp52F->hd,_tmpCFA)))))),((_tmpCF9->tl=dles,_tmpCF9))))));};}}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);e->r=Cyc_Toc_unresolvedmem_exp_r(
0,dles);}goto _LL221;_LL25C: {struct Cyc_Absyn_Array_e_struct*_tmp530=(struct Cyc_Absyn_Array_e_struct*)
_tmp4CF;if(_tmp530->tag != 27)goto _LL25E;else{_tmp531=_tmp530->f1;}}_LL25D: e->r=
Cyc_Toc_unresolvedmem_exp_r(0,_tmp531);{struct Cyc_List_List*_tmp714=_tmp531;for(
0;_tmp714 != 0;_tmp714=_tmp714->tl){struct _tuple15 _tmp716;struct Cyc_Absyn_Exp*
_tmp717;struct _tuple15*_tmp715=(struct _tuple15*)_tmp714->hd;_tmp716=*_tmp715;
_tmp717=_tmp716.f2;Cyc_Toc_exp_to_c(nv,_tmp717);}}goto _LL221;_LL25E: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp532=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp4CF;if(_tmp532->tag != 28)
goto _LL260;else{_tmp533=_tmp532->f1;_tmp534=_tmp532->f2;_tmp535=_tmp532->f3;
_tmp536=_tmp532->f4;}}_LL25F: {unsigned int _tmp719;int _tmp71A;struct _tuple10
_tmp718=Cyc_Evexp_eval_const_uint_exp(_tmp534);_tmp719=_tmp718.f1;_tmp71A=
_tmp718.f2;{void*_tmp71B=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp535->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp535);{struct Cyc_List_List*
es=0;if(!Cyc_Toc_is_zero(_tmp535)){if(!_tmp71A){const char*_tmpCFD;void*_tmpCFC;(
_tmpCFC=0,Cyc_Tcutil_terr(_tmp534->loc,((_tmpCFD="cannot determine value of constant",
_tag_dyneither(_tmpCFD,sizeof(char),35))),_tag_dyneither(_tmpCFC,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp719;++ i){struct _tuple15*_tmpD00;struct Cyc_List_List*
_tmpCFF;es=((_tmpCFF=_cycalloc(sizeof(*_tmpCFF)),((_tmpCFF->hd=((_tmpD00=
_cycalloc(sizeof(*_tmpD00)),((_tmpD00->f1=0,((_tmpD00->f2=_tmp535,_tmpD00)))))),((
_tmpCFF->tl=es,_tmpCFF))))));}}if(_tmp536){struct Cyc_Absyn_Exp*_tmp720=Cyc_Toc_cast_it(
_tmp71B,Cyc_Absyn_uint_exp(0,0));struct _tuple15*_tmpD03;struct Cyc_List_List*
_tmpD02;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,((_tmpD02=_cycalloc(sizeof(*_tmpD02)),((_tmpD02->hd=((
_tmpD03=_cycalloc(sizeof(*_tmpD03)),((_tmpD03->f1=0,((_tmpD03->f2=_tmp720,
_tmpD03)))))),((_tmpD02->tl=0,_tmpD02)))))));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL221;};};}_LL260: {struct Cyc_Absyn_Aggregate_e_struct*_tmp537=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp4CF;if(_tmp537->tag != 29)goto _LL262;
else{_tmp538=_tmp537->f1;_tmp539=_tmp537->f2;_tmp53A=_tmp537->f3;_tmp53B=_tmp537->f4;}}
_LL261: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp539
!= 0,0,0,_tmp53A,_tmp538))->r;else{if(_tmp53B == 0){const char*_tmpD06;void*
_tmpD05;(_tmpD05=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD06="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpD06,sizeof(
char),38))),_tag_dyneither(_tmpD05,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp53B;struct Cyc_Toc_Env _tmp726;struct
_RegionHandle*_tmp727;struct Cyc_Toc_Env*_tmp725=nv;_tmp726=*_tmp725;_tmp727=
_tmp726.rgn;{struct Cyc_List_List*_tmp728=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum 
Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp727,e->loc,_tmp53A,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp53B->impl))->tagged){struct
_tuple20 _tmp72A;struct Cyc_Absyn_Aggrfield*_tmp72B;struct Cyc_Absyn_Exp*_tmp72C;
struct _tuple20*_tmp729=(struct _tuple20*)((struct Cyc_List_List*)_check_null(
_tmp728))->hd;_tmp72A=*_tmp729;_tmp72B=_tmp72A.f1;_tmp72C=_tmp72A.f2;{void*
_tmp72D=_tmp72B->type;Cyc_Toc_exp_to_c(nv,_tmp72C);if(Cyc_Toc_is_void_star_or_tvar(
_tmp72D))_tmp72C->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp72C->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp53B,_tmp72B->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple15*_tmpD0B;struct _tuple15*_tmpD0A;struct _tuple15*_tmpD09[2];
struct Cyc_List_List*_tmp72E=(_tmpD09[1]=((_tmpD0A=_cycalloc(sizeof(*_tmpD0A)),((
_tmpD0A->f1=0,((_tmpD0A->f2=_tmp72C,_tmpD0A)))))),((_tmpD09[0]=((_tmpD0B=
_cycalloc(sizeof(*_tmpD0B)),((_tmpD0B->f1=0,((_tmpD0B->f2=field_tag_exp,_tmpD0B)))))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD09,sizeof(struct _tuple15*),2)))));struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(
0,_tmp72E,0);struct Cyc_Absyn_FieldName_struct*_tmpD11;struct Cyc_Absyn_FieldName_struct
_tmpD10;void*_tmpD0F[1];struct Cyc_List_List*ds=(_tmpD0F[0]=(void*)((_tmpD11=
_cycalloc(sizeof(*_tmpD11)),((_tmpD11[0]=((_tmpD10.tag=1,((_tmpD10.f1=_tmp72B->name,
_tmpD10)))),_tmpD11)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD0F,sizeof(void*),1)));struct _tuple15*_tmpD14;struct _tuple15*
_tmpD13[1];struct Cyc_List_List*dles=(_tmpD13[0]=((_tmpD14=_cycalloc(sizeof(*
_tmpD14)),((_tmpD14->f1=ds,((_tmpD14->f2=umem,_tmpD14)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD13,sizeof(struct
_tuple15*),1)));e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{struct Cyc_List_List*
_tmp737=0;struct Cyc_List_List*_tmp738=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp738 != 0;_tmp738=_tmp738->tl){struct Cyc_List_List*
_tmp739=_tmp728;for(0;_tmp739 != 0;_tmp739=_tmp739->tl){if((*((struct _tuple20*)
_tmp739->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp738->hd){struct _tuple20
_tmp73B;struct Cyc_Absyn_Aggrfield*_tmp73C;struct Cyc_Absyn_Exp*_tmp73D;struct
_tuple20*_tmp73A=(struct _tuple20*)_tmp739->hd;_tmp73B=*_tmp73A;_tmp73C=_tmp73B.f1;
_tmp73D=_tmp73B.f2;{void*_tmp73E=_tmp73C->type;Cyc_Toc_exp_to_c(nv,_tmp73D);if(
Cyc_Toc_is_void_star_or_tvar(_tmp73E))_tmp73D->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp73D->r,0));{struct _tuple15*_tmpD17;struct Cyc_List_List*
_tmpD16;_tmp737=((_tmpD16=_cycalloc(sizeof(*_tmpD16)),((_tmpD16->hd=((_tmpD17=
_cycalloc(sizeof(*_tmpD17)),((_tmpD17->f1=0,((_tmpD17->f2=_tmp73D,_tmpD17)))))),((
_tmpD16->tl=_tmp737,_tmpD16))))));}break;};}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp737));}};};}
goto _LL221;_LL262: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp53C=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp4CF;if(_tmp53C->tag != 30)goto _LL264;else{_tmp53D=(void*)_tmp53C->f1;_tmp53E=
_tmp53C->f2;}}_LL263: {struct Cyc_List_List*fs;{void*_tmp741=Cyc_Tcutil_compress(
_tmp53D);struct Cyc_List_List*_tmp743;_LL326: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp742=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp741;if(_tmp742->tag != 13)goto
_LL328;else{_tmp743=_tmp742->f2;}}_LL327: fs=_tmp743;goto _LL325;_LL328:;_LL329: {
const char*_tmpD1B;void*_tmpD1A[1];struct Cyc_String_pa_struct _tmpD19;(_tmpD19.tag=
0,((_tmpD19.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp53D)),((_tmpD1A[0]=& _tmpD19,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD1B="anon struct has type %s",
_tag_dyneither(_tmpD1B,sizeof(char),24))),_tag_dyneither(_tmpD1A,sizeof(void*),1)))))));}
_LL325:;}{struct Cyc_Toc_Env _tmp748;struct _RegionHandle*_tmp749;struct Cyc_Toc_Env*
_tmp747=nv;_tmp748=*_tmp747;_tmp749=_tmp748.rgn;{struct Cyc_List_List*_tmp74A=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp749,e->loc,_tmp53E,Cyc_Absyn_StructA,fs);for(0;_tmp74A != 0;_tmp74A=_tmp74A->tl){
struct _tuple20 _tmp74C;struct Cyc_Absyn_Aggrfield*_tmp74D;struct Cyc_Absyn_Exp*
_tmp74E;struct _tuple20*_tmp74B=(struct _tuple20*)_tmp74A->hd;_tmp74C=*_tmp74B;
_tmp74D=_tmp74C.f1;_tmp74E=_tmp74C.f2;{void*_tmp74F=_tmp74D->type;Cyc_Toc_exp_to_c(
nv,_tmp74E);if(Cyc_Toc_is_void_star_or_tvar(_tmp74F))_tmp74E->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp74E->r,0));};}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp53E);}goto _LL221;};}_LL264: {struct Cyc_Absyn_Datatype_e_struct*_tmp53F=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp4CF;if(_tmp53F->tag != 31)goto _LL266;else{
_tmp540=_tmp53F->f1;_tmp541=_tmp53F->f2;_tmp542=_tmp53F->f3;}}_LL265: {void*
datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple0*_tmp750=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp751=Cyc_Absyn_var_exp(_tmp750,0);struct Cyc_Absyn_Exp*
member_exp;{const char*_tmpD1C;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp542->name,((_tmpD1C="_struct",_tag_dyneither(_tmpD1C,sizeof(char),8)))));}
tag_exp=_tmp541->is_extensible?Cyc_Absyn_var_exp(_tmp542->name,0): Cyc_Toc_datatype_tag(
_tmp541,_tmp542->name);member_exp=_tmp751;{struct Cyc_List_List*_tmp753=_tmp542->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp540 != 0;(
_tmp540=_tmp540->tl,_tmp753=_tmp753->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp540->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp753))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct _tuple15*_tmpD1F;struct
Cyc_List_List*_tmpD1E;dles=((_tmpD1E=_cycalloc(sizeof(*_tmpD1E)),((_tmpD1E->hd=((
_tmpD1F=_cycalloc(sizeof(*_tmpD1F)),((_tmpD1F->f1=0,((_tmpD1F->f2=cur_e,_tmpD1F)))))),((
_tmpD1E->tl=dles,_tmpD1E))))));};}{struct _tuple15*_tmpD22;struct Cyc_List_List*
_tmpD21;dles=((_tmpD21=_cycalloc(sizeof(*_tmpD21)),((_tmpD21->hd=((_tmpD22=
_cycalloc(sizeof(*_tmpD22)),((_tmpD22->f1=0,((_tmpD22->f2=tag_exp,_tmpD22)))))),((
_tmpD21->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles),_tmpD21))))));}e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{struct Cyc_List_List*
_tmpD23;struct Cyc_List_List*_tmp758=(_tmpD23=_cycalloc(sizeof(*_tmpD23)),((
_tmpD23->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,
0),tag_exp,0),((_tmpD23->tl=0,_tmpD23)))));{int i=1;for(0;_tmp540 != 0;(((_tmp540=
_tmp540->tl,i ++)),_tmp753=_tmp753->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp540->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp753))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp759=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpD24;_tmp758=((_tmpD24=_cycalloc(sizeof(*
_tmpD24)),((_tmpD24->hd=_tmp759,((_tmpD24->tl=_tmp758,_tmpD24))))));};}}{struct
Cyc_Absyn_Stmt*_tmp75B=Cyc_Absyn_exp_stmt(_tmp751,0);struct Cyc_List_List*_tmpD25;
struct Cyc_Absyn_Stmt*_tmp75C=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpD25=_cycalloc(sizeof(*_tmpD25)),((
_tmpD25->hd=_tmp75B,((_tmpD25->tl=_tmp758,_tmpD25))))))),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp750,datatype_ctype,0,_tmp75C,0));};}goto _LL221;};}
_LL266: {struct Cyc_Absyn_Enum_e_struct*_tmp543=(struct Cyc_Absyn_Enum_e_struct*)
_tmp4CF;if(_tmp543->tag != 32)goto _LL268;}_LL267: goto _LL269;_LL268: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp544=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp4CF;if(_tmp544->tag != 33)goto
_LL26A;}_LL269: goto _LL221;_LL26A: {struct Cyc_Absyn_Malloc_e_struct*_tmp545=(
struct Cyc_Absyn_Malloc_e_struct*)_tmp4CF;if(_tmp545->tag != 34)goto _LL26C;else{
_tmp546=_tmp545->f1;_tmp547=_tmp546.is_calloc;_tmp548=_tmp546.rgn;_tmp549=
_tmp546.elt_type;_tmp54A=_tmp546.num_elts;_tmp54B=_tmp546.fat_result;}}_LL26B: {
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp549)));Cyc_Toc_exp_to_c(nv,
_tmp54A);if(_tmp54B){struct _tuple0*_tmp75F=Cyc_Toc_temp_var();struct _tuple0*
_tmp760=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp547){xexp=_tmp54A;if(_tmp548 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp548;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp75F,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp549,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp75F,0));}{struct Cyc_Absyn_Exp*_tmpD26[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpD26[2]=Cyc_Absyn_var_exp(_tmp75F,0),((_tmpD26[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpD26[0]=Cyc_Absyn_var_exp(_tmp760,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD26,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{xexp=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp54A,0);if(_tmp548 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp548;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp75F,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp549,Cyc_Absyn_var_exp(_tmp75F,0));}{struct Cyc_Absyn_Exp*_tmpD27[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpD27[2]=Cyc_Absyn_var_exp(_tmp75F,0),((_tmpD27[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpD27[0]=Cyc_Absyn_var_exp(_tmp760,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD27,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);};}{struct Cyc_Absyn_Stmt*_tmp763=Cyc_Absyn_declare_stmt(_tmp75F,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp760,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp763);};}else{struct Cyc_Absyn_Exp*_tmp764=Cyc_Absyn_sizeoftyp_exp(
t_c,0);{void*_tmp765=_tmp54A->r;union Cyc_Absyn_Cnst _tmp767;struct _tuple5 _tmp768;
int _tmp769;_LL32B: {struct Cyc_Absyn_Const_e_struct*_tmp766=(struct Cyc_Absyn_Const_e_struct*)
_tmp765;if(_tmp766->tag != 0)goto _LL32D;else{_tmp767=_tmp766->f1;if((_tmp767.Int_c).tag
!= 4)goto _LL32D;_tmp768=(struct _tuple5)(_tmp767.Int_c).val;_tmp769=_tmp768.f2;
if(_tmp769 != 1)goto _LL32D;}}_LL32C: goto _LL32A;_LL32D:;_LL32E: _tmp764=Cyc_Absyn_times_exp(
_tmp764,_tmp54A,0);goto _LL32A;_LL32A:;}if(_tmp548 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp548;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rmalloc_exp(rgn,_tmp764))->r;}else{e->r=(Cyc_Toc_malloc_exp(*_tmp549,
_tmp764))->r;}}goto _LL221;}_LL26C: {struct Cyc_Absyn_Swap_e_struct*_tmp54C=(
struct Cyc_Absyn_Swap_e_struct*)_tmp4CF;if(_tmp54C->tag != 35)goto _LL26E;else{
_tmp54D=_tmp54C->f1;_tmp54E=_tmp54C->f2;}}_LL26D: {int is_dyneither_ptr=0;void*
e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp54D->topt))->v;void*
e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp54E->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(
e1_old_typ,& is_dyneither_ptr)){const char*_tmpD2A;void*_tmpD29;(_tmpD29=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD2A="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpD2A,sizeof(char),57))),_tag_dyneither(_tmpD29,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}{int f1_tag=0;void*tagged_mem_type1=(void*)&
Cyc_Absyn_VoidType_val;int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp54D,&
f1_tag,& tagged_mem_type1,1);int f2_tag=0;void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp54E,& f2_tag,& tagged_mem_type2,1);
Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp54D);Cyc_Toc_exp_to_c(nv,_tmp54E);
Cyc_Toc_set_lhs(nv,0);if(e1_tmem)Cyc_Toc_add_tagged_union_check_for_swap(_tmp54D,
f1_tag,tagged_mem_type1);else{_tmp54D=Cyc_Toc_push_address_exp(_tmp54D);}if(
e2_tmem)Cyc_Toc_add_tagged_union_check_for_swap(_tmp54E,f2_tag,tagged_mem_type2);
else{_tmp54E=Cyc_Toc_push_address_exp(_tmp54E);}{struct Cyc_Absyn_Exp*_tmpD2B[2];
e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpD2B[1]=_tmp54E,((_tmpD2B[0]=_tmp54D,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD2B,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL221;};};}_LL26E: {struct
Cyc_Absyn_Tagcheck_e_struct*_tmp54F=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp4CF;
if(_tmp54F->tag != 38)goto _LL270;else{_tmp550=_tmp54F->f1;_tmp551=_tmp54F->f2;}}
_LL26F: {void*_tmp76D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp550->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp550);{void*_tmp76E=
_tmp76D;struct Cyc_Absyn_AggrInfo _tmp770;union Cyc_Absyn_AggrInfoU _tmp771;struct
Cyc_Absyn_Aggrdecl**_tmp772;struct Cyc_Absyn_Aggrdecl*_tmp773;_LL330: {struct Cyc_Absyn_AggrType_struct*
_tmp76F=(struct Cyc_Absyn_AggrType_struct*)_tmp76E;if(_tmp76F->tag != 12)goto
_LL332;else{_tmp770=_tmp76F->f1;_tmp771=_tmp770.aggr_info;if((_tmp771.KnownAggr).tag
!= 2)goto _LL332;_tmp772=(struct Cyc_Absyn_Aggrdecl**)(_tmp771.KnownAggr).val;
_tmp773=*_tmp772;}}_LL331: {struct Cyc_Absyn_Exp*_tmp774=Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp773,_tmp551),0);struct Cyc_Absyn_Exp*_tmp775=Cyc_Absyn_aggrmember_exp(
_tmp550,_tmp551,0);struct Cyc_Absyn_Exp*_tmp776=Cyc_Absyn_aggrmember_exp(_tmp775,
Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(_tmp776,_tmp774,0))->r;goto _LL32F;}
_LL332:;_LL333: {const char*_tmpD2E;void*_tmpD2D;(_tmpD2D=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD2E="non-aggregate type in tagcheck",
_tag_dyneither(_tmpD2E,sizeof(char),31))),_tag_dyneither(_tmpD2D,sizeof(void*),0)));}
_LL32F:;}goto _LL221;}_LL270: {struct Cyc_Absyn_StmtExp_e_struct*_tmp552=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp4CF;if(_tmp552->tag != 37)goto _LL272;else{_tmp553=
_tmp552->f1;}}_LL271: Cyc_Toc_stmt_to_c(nv,_tmp553);goto _LL221;_LL272: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp554=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp4CF;if(_tmp554->tag != 36)
goto _LL274;}_LL273: {const char*_tmpD31;void*_tmpD30;(_tmpD30=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD31="UnresolvedMem",
_tag_dyneither(_tmpD31,sizeof(char),14))),_tag_dyneither(_tmpD30,sizeof(void*),0)));}
_LL274: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp555=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp4CF;if(_tmp555->tag != 26)goto _LL276;}_LL275: {const char*_tmpD34;void*_tmpD33;(
_tmpD33=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpD34="compoundlit",_tag_dyneither(_tmpD34,sizeof(char),12))),_tag_dyneither(
_tmpD33,sizeof(void*),0)));}_LL276: {struct Cyc_Absyn_Valueof_e_struct*_tmp556=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp4CF;if(_tmp556->tag != 39)goto _LL278;}
_LL277: {const char*_tmpD37;void*_tmpD36;(_tmpD36=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD37="valueof(-)",
_tag_dyneither(_tmpD37,sizeof(char),11))),_tag_dyneither(_tmpD36,sizeof(void*),0)));}
_LL278: {struct Cyc_Absyn_Asm_e_struct*_tmp557=(struct Cyc_Absyn_Asm_e_struct*)
_tmp4CF;if(_tmp557->tag != 40)goto _LL221;}_LL279: {const char*_tmpD3A;void*_tmpD39;(
_tmpD39=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD3A="__asm__",_tag_dyneither(_tmpD3A,sizeof(char),8))),_tag_dyneither(
_tmpD39,sizeof(void*),0)));}_LL221:;};}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0);}struct _tuple21{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*
f3;};struct _tuple22{struct _tuple0*f1;void*f2;};struct _tuple23{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple21 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*
nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
struct Cyc_Absyn_Stmt*s;{void*_tmp781=p->r;struct Cyc_Absyn_Vardecl*_tmp783;struct
Cyc_Absyn_Vardecl _tmp784;struct _tuple0*_tmp785;struct Cyc_Absyn_Pat*_tmp786;
struct Cyc_Absyn_Vardecl*_tmp788;struct Cyc_Absyn_Vardecl _tmp789;struct _tuple0*
_tmp78A;struct Cyc_Absyn_Vardecl*_tmp78D;struct Cyc_Absyn_Pat*_tmp78E;enum Cyc_Absyn_Sign
_tmp791;int _tmp792;char _tmp794;struct _dyneither_ptr _tmp796;struct Cyc_Absyn_Enumdecl*
_tmp798;struct Cyc_Absyn_Enumfield*_tmp799;void*_tmp79B;struct Cyc_Absyn_Enumfield*
_tmp79C;struct Cyc_Absyn_Pat*_tmp79E;struct Cyc_Absyn_Pat _tmp79F;void*_tmp7A0;
struct Cyc_Absyn_Datatypedecl*_tmp7A2;struct Cyc_Absyn_Datatypefield*_tmp7A3;
struct Cyc_List_List*_tmp7A4;struct Cyc_List_List*_tmp7A6;struct Cyc_List_List*
_tmp7A8;struct Cyc_Absyn_AggrInfo*_tmp7AA;struct Cyc_Absyn_AggrInfo*_tmp7AC;struct
Cyc_Absyn_AggrInfo _tmp7AD;union Cyc_Absyn_AggrInfoU _tmp7AE;struct Cyc_List_List*
_tmp7AF;struct Cyc_Absyn_Pat*_tmp7B1;_LL335: {struct Cyc_Absyn_Var_p_struct*
_tmp782=(struct Cyc_Absyn_Var_p_struct*)_tmp781;if(_tmp782->tag != 1)goto _LL337;
else{_tmp783=_tmp782->f1;_tmp784=*_tmp783;_tmp785=_tmp784.name;_tmp786=_tmp782->f2;}}
_LL336: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp785,r),rgn,t,r,path,
_tmp786,fail_stmt,decls);_LL337: {struct Cyc_Absyn_TagInt_p_struct*_tmp787=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp781;if(_tmp787->tag != 3)goto _LL339;else{
_tmp788=_tmp787->f2;_tmp789=*_tmp788;_tmp78A=_tmp789.name;}}_LL338: nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp78A,r);goto _LL33A;_LL339: {struct Cyc_Absyn_Wild_p_struct*_tmp78B=(
struct Cyc_Absyn_Wild_p_struct*)_tmp781;if(_tmp78B->tag != 0)goto _LL33B;}_LL33A: s=
Cyc_Toc_skip_stmt_dl();goto _LL334;_LL33B: {struct Cyc_Absyn_Reference_p_struct*
_tmp78C=(struct Cyc_Absyn_Reference_p_struct*)_tmp781;if(_tmp78C->tag != 2)goto
_LL33D;else{_tmp78D=_tmp78C->f1;_tmp78E=_tmp78C->f2;}}_LL33C: {struct _tuple0*
_tmp7B5=Cyc_Toc_temp_var();{struct _tuple22*_tmpD3D;struct Cyc_List_List*_tmpD3C;
decls=((_tmpD3C=_region_malloc(rgn,sizeof(*_tmpD3C)),((_tmpD3C->hd=((_tmpD3D=
_region_malloc(rgn,sizeof(*_tmpD3D)),((_tmpD3D->f1=_tmp7B5,((_tmpD3D->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpD3D)))))),((_tmpD3C->tl=decls,_tmpD3C))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp78D->name,Cyc_Absyn_var_exp(_tmp7B5,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp7B5,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Toc_push_address_exp(path)),0);{struct _tuple21 _tmp7B8=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp78E,fail_stmt,decls);_tmp7B8.f3=Cyc_Absyn_seq_stmt(s,_tmp7B8.f3,
0);return _tmp7B8;};}_LL33D: {struct Cyc_Absyn_Null_p_struct*_tmp78F=(struct Cyc_Absyn_Null_p_struct*)
_tmp781;if(_tmp78F->tag != 8)goto _LL33F;}_LL33E: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),fail_stmt);goto _LL334;_LL33F: {struct Cyc_Absyn_Int_p_struct*_tmp790=(struct
Cyc_Absyn_Int_p_struct*)_tmp781;if(_tmp790->tag != 9)goto _LL341;else{_tmp791=
_tmp790->f1;_tmp792=_tmp790->f2;}}_LL340: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp791,_tmp792,0),fail_stmt);goto _LL334;_LL341: {struct Cyc_Absyn_Char_p_struct*
_tmp793=(struct Cyc_Absyn_Char_p_struct*)_tmp781;if(_tmp793->tag != 10)goto _LL343;
else{_tmp794=_tmp793->f1;}}_LL342: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(
_tmp794,0),fail_stmt);goto _LL334;_LL343: {struct Cyc_Absyn_Float_p_struct*_tmp795=(
struct Cyc_Absyn_Float_p_struct*)_tmp781;if(_tmp795->tag != 11)goto _LL345;else{
_tmp796=_tmp795->f1;}}_LL344: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp796,
0),fail_stmt);goto _LL334;_LL345: {struct Cyc_Absyn_Enum_p_struct*_tmp797=(struct
Cyc_Absyn_Enum_p_struct*)_tmp781;if(_tmp797->tag != 12)goto _LL347;else{_tmp798=
_tmp797->f1;_tmp799=_tmp797->f2;}}_LL346:{struct Cyc_Absyn_Enum_e_struct _tmpD40;
struct Cyc_Absyn_Enum_e_struct*_tmpD3F;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD3F=_cycalloc(sizeof(*_tmpD3F)),((_tmpD3F[0]=((_tmpD40.tag=32,((
_tmpD40.f1=_tmp799->name,((_tmpD40.f2=(struct Cyc_Absyn_Enumdecl*)_tmp798,((
_tmpD40.f3=(struct Cyc_Absyn_Enumfield*)_tmp799,_tmpD40)))))))),_tmpD3F)))),0),
fail_stmt);}goto _LL334;_LL347: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp79A=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp781;if(_tmp79A->tag != 13)goto _LL349;else{
_tmp79B=(void*)_tmp79A->f1;_tmp79C=_tmp79A->f2;}}_LL348:{struct Cyc_Absyn_AnonEnum_e_struct
_tmpD43;struct Cyc_Absyn_AnonEnum_e_struct*_tmpD42;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD42=_cycalloc(sizeof(*_tmpD42)),((_tmpD42[0]=((_tmpD43.tag=33,((
_tmpD43.f1=_tmp79C->name,((_tmpD43.f2=(void*)_tmp79B,((_tmpD43.f3=(struct Cyc_Absyn_Enumfield*)
_tmp79C,_tmpD43)))))))),_tmpD42)))),0),fail_stmt);}goto _LL334;_LL349: {struct Cyc_Absyn_Pointer_p_struct*
_tmp79D=(struct Cyc_Absyn_Pointer_p_struct*)_tmp781;if(_tmp79D->tag != 5)goto
_LL34B;else{_tmp79E=_tmp79D->f1;_tmp79F=*_tmp79E;_tmp7A0=_tmp79F.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp7A1=(struct Cyc_Absyn_Datatype_p_struct*)_tmp7A0;if(_tmp7A1->tag != 7)goto
_LL34B;else{_tmp7A2=_tmp7A1->f1;_tmp7A3=_tmp7A1->f2;_tmp7A4=_tmp7A1->f3;}};}}
_LL34A: s=Cyc_Toc_skip_stmt_dl();{struct _tuple0*_tmp7BD=Cyc_Toc_temp_var();const
char*_tmpD44;struct _tuple0*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp7A3->name,((
_tmpD44="_struct",_tag_dyneither(_tmpD44,sizeof(char),8))));void*_tmp7BE=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);int cnt=1;struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
_tmp7BE,r);struct Cyc_List_List*_tmp7BF=_tmp7A3->typs;for(0;_tmp7A4 != 0;(((
_tmp7A4=_tmp7A4->tl,_tmp7BF=((struct Cyc_List_List*)_check_null(_tmp7BF))->tl)),
++ cnt)){struct Cyc_Absyn_Pat*_tmp7C0=(struct Cyc_Absyn_Pat*)_tmp7A4->hd;if(_tmp7C0->r
== (void*)& Cyc_Absyn_Wild_p_val)continue;{void*_tmp7C1=(*((struct _tuple9*)((
struct Cyc_List_List*)_check_null(_tmp7BF))->hd)).f2;struct _tuple0*_tmp7C2=Cyc_Toc_temp_var();
void*_tmp7C3=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp7C0->topt))->v;void*
_tmp7C4=Cyc_Toc_typ_to_c(_tmp7C3);struct Cyc_Absyn_Exp*_tmp7C5=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp7BD,0),Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_tvar(
Cyc_Toc_typ_to_c(_tmp7C1)))_tmp7C5=Cyc_Toc_cast_it(_tmp7C4,_tmp7C5);{struct
_tuple22*_tmpD47;struct Cyc_List_List*_tmpD46;decls=((_tmpD46=_region_malloc(rgn,
sizeof(*_tmpD46)),((_tmpD46->hd=((_tmpD47=_region_malloc(rgn,sizeof(*_tmpD47)),((
_tmpD47->f1=_tmp7C2,((_tmpD47->f2=_tmp7C4,_tmpD47)))))),((_tmpD46->tl=decls,
_tmpD46))))));}{struct _tuple21 _tmp7C8=Cyc_Toc_xlate_pat(nv,rgn,_tmp7C3,Cyc_Absyn_var_exp(
_tmp7C2,0),_tmp7C5,_tmp7C0,fail_stmt,decls);nv=_tmp7C8.f1;decls=_tmp7C8.f2;{
struct Cyc_Absyn_Stmt*_tmp7C9=_tmp7C8.f3;struct Cyc_Absyn_Stmt*_tmp7CA=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp7C2,0),_tmp7C5,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp7CA,_tmp7C9,0),0);};};};}{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp7BD,0),Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*tag_exp=
_tmp7A2->is_extensible?Cyc_Absyn_var_exp(_tmp7A3->name,0): Cyc_Toc_datatype_tag(
_tmp7A2,_tmp7A3->name);struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,
tag_exp,0);s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);if(Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v))s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp7BD,0),Cyc_Absyn_uint_exp(0,0),0),
fail_stmt,s,0);s=Cyc_Absyn_declare_stmt(_tmp7BD,_tmp7BE,(struct Cyc_Absyn_Exp*)
rcast,s,0);goto _LL334;};};_LL34B: {struct Cyc_Absyn_Datatype_p_struct*_tmp7A5=(
struct Cyc_Absyn_Datatype_p_struct*)_tmp781;if(_tmp7A5->tag != 7)goto _LL34D;else{
_tmp7A6=_tmp7A5->f3;}}_LL34C: _tmp7A8=_tmp7A6;goto _LL34E;_LL34D: {struct Cyc_Absyn_Tuple_p_struct*
_tmp7A7=(struct Cyc_Absyn_Tuple_p_struct*)_tmp781;if(_tmp7A7->tag != 4)goto _LL34F;
else{_tmp7A8=_tmp7A7->f1;}}_LL34E: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;
_tmp7A8 != 0;(_tmp7A8=_tmp7A8->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp7CC=(struct Cyc_Absyn_Pat*)
_tmp7A8->hd;if(_tmp7CC->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{struct _tuple0*
_tmp7CD=Cyc_Toc_temp_var();void*_tmp7CE=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7CC->topt))->v;{struct _tuple22*_tmpD4A;struct Cyc_List_List*_tmpD49;decls=((
_tmpD49=_region_malloc(rgn,sizeof(*_tmpD49)),((_tmpD49->hd=((_tmpD4A=
_region_malloc(rgn,sizeof(*_tmpD4A)),((_tmpD4A->f1=_tmp7CD,((_tmpD4A->f2=Cyc_Toc_typ_to_c(
_tmp7CE),_tmpD4A)))))),((_tmpD49->tl=decls,_tmpD49))))));}{struct _tuple21 _tmp7D1=
Cyc_Toc_xlate_pat(nv,rgn,_tmp7CE,Cyc_Absyn_var_exp(_tmp7CD,0),Cyc_Absyn_aggrmember_exp(
path,Cyc_Absyn_fieldname(cnt),0),_tmp7CC,fail_stmt,decls);nv=_tmp7D1.f1;decls=
_tmp7D1.f2;{struct Cyc_Absyn_Stmt*_tmp7D2=_tmp7D1.f3;struct Cyc_Absyn_Stmt*_tmp7D3=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7CD,0),Cyc_Absyn_aggrmember_exp(r,
Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7D3,
_tmp7D2,0),0);};};};}goto _LL334;};_LL34F: {struct Cyc_Absyn_Aggr_p_struct*_tmp7A9=(
struct Cyc_Absyn_Aggr_p_struct*)_tmp781;if(_tmp7A9->tag != 6)goto _LL351;else{
_tmp7AA=_tmp7A9->f1;if(_tmp7AA != 0)goto _LL351;}}_LL350: {const char*_tmpD4D;void*
_tmpD4C;(_tmpD4C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD4D="unresolved aggregate pattern!",_tag_dyneither(_tmpD4D,sizeof(char),30))),
_tag_dyneither(_tmpD4C,sizeof(void*),0)));}_LL351: {struct Cyc_Absyn_Aggr_p_struct*
_tmp7AB=(struct Cyc_Absyn_Aggr_p_struct*)_tmp781;if(_tmp7AB->tag != 6)goto _LL353;
else{_tmp7AC=_tmp7AB->f1;if(_tmp7AC == 0)goto _LL353;_tmp7AD=*_tmp7AC;_tmp7AE=
_tmp7AD.aggr_info;_tmp7AF=_tmp7AB->f3;}}_LL352: {struct Cyc_Absyn_Aggrdecl*
_tmp7D6=Cyc_Absyn_get_known_aggrdecl(_tmp7AE);if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp7D6->impl))->tagged){struct _tuple23 _tmp7D8;struct Cyc_List_List*
_tmp7D9;struct Cyc_Absyn_Pat*_tmp7DA;struct _tuple23*_tmp7D7=(struct _tuple23*)((
struct Cyc_List_List*)_check_null(_tmp7AF))->hd;_tmp7D8=*_tmp7D7;_tmp7D9=_tmp7D8.f1;
_tmp7DA=_tmp7D8.f2;{struct _dyneither_ptr*f;{void*_tmp7DB=(void*)((struct Cyc_List_List*)
_check_null(_tmp7D9))->hd;struct _dyneither_ptr*_tmp7DD;_LL35C: {struct Cyc_Absyn_FieldName_struct*
_tmp7DC=(struct Cyc_Absyn_FieldName_struct*)_tmp7DB;if(_tmp7DC->tag != 1)goto
_LL35E;else{_tmp7DD=_tmp7DC->f1;}}_LL35D: f=_tmp7DD;goto _LL35B;_LL35E:;_LL35F: {
const char*_tmpD50;void*_tmpD4F;(_tmpD4F=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD50="no field name in tagged union pattern",
_tag_dyneither(_tmpD50,sizeof(char),38))),_tag_dyneither(_tmpD4F,sizeof(void*),0)));}
_LL35B:;}{struct _tuple0*_tmp7E0=Cyc_Toc_temp_var();void*_tmp7E1=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp7DA->topt))->v;void*_tmp7E2=Cyc_Toc_typ_to_c(
_tmp7E1);{struct _tuple22*_tmpD53;struct Cyc_List_List*_tmpD52;decls=((_tmpD52=
_region_malloc(rgn,sizeof(*_tmpD52)),((_tmpD52->hd=((_tmpD53=_region_malloc(rgn,
sizeof(*_tmpD53)),((_tmpD53->f1=_tmp7E0,((_tmpD53->f2=_tmp7E2,_tmpD53)))))),((
_tmpD52->tl=decls,_tmpD52))))));}{struct Cyc_Absyn_Exp*_tmp7E5=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp7E6=
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp7E6=Cyc_Toc_cast_it(_tmp7E2,_tmp7E6);_tmp7E5=Cyc_Toc_cast_it(_tmp7E2,_tmp7E5);{
struct _tuple21 _tmp7E7=Cyc_Toc_xlate_pat(nv,rgn,_tmp7E1,Cyc_Absyn_var_exp(_tmp7E0,
0),_tmp7E5,_tmp7DA,fail_stmt,decls);nv=_tmp7E7.f1;decls=_tmp7E7.f2;{struct Cyc_Absyn_Stmt*
_tmp7E8=_tmp7E7.f3;struct Cyc_Absyn_Stmt*_tmp7E9=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp7D6,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp7EA=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp7E0,0),_tmp7E6,0);s=Cyc_Absyn_seq_stmt(_tmp7E9,Cyc_Absyn_seq_stmt(_tmp7EA,
_tmp7E8,0),0);};};};};};}else{s=Cyc_Toc_skip_stmt_dl();for(0;_tmp7AF != 0;_tmp7AF=
_tmp7AF->tl){struct _tuple23*_tmp7EB=(struct _tuple23*)_tmp7AF->hd;struct Cyc_Absyn_Pat*
_tmp7EC=(*_tmp7EB).f2;if(_tmp7EC->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _dyneither_ptr*f;{void*_tmp7ED=(void*)((struct Cyc_List_List*)_check_null((*
_tmp7EB).f1))->hd;struct _dyneither_ptr*_tmp7EF;_LL361: {struct Cyc_Absyn_FieldName_struct*
_tmp7EE=(struct Cyc_Absyn_FieldName_struct*)_tmp7ED;if(_tmp7EE->tag != 1)goto
_LL363;else{_tmp7EF=_tmp7EE->f1;}}_LL362: f=_tmp7EF;goto _LL360;_LL363:;_LL364: {
struct Cyc_Toc_Match_error_struct _tmpD56;struct Cyc_Toc_Match_error_struct*_tmpD55;(
int)_throw((void*)((_tmpD55=_cycalloc_atomic(sizeof(*_tmpD55)),((_tmpD55[0]=((
_tmpD56.tag=Cyc_Toc_Match_error,_tmpD56)),_tmpD55)))));}_LL360:;}{struct _tuple0*
_tmp7F2=Cyc_Toc_temp_var();void*_tmp7F3=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7EC->topt))->v;void*_tmp7F4=Cyc_Toc_typ_to_c(_tmp7F3);{struct _tuple22*
_tmpD59;struct Cyc_List_List*_tmpD58;decls=((_tmpD58=_region_malloc(rgn,sizeof(*
_tmpD58)),((_tmpD58->hd=((_tmpD59=_region_malloc(rgn,sizeof(*_tmpD59)),((_tmpD59->f1=
_tmp7F2,((_tmpD59->f2=_tmp7F4,_tmpD59)))))),((_tmpD58->tl=decls,_tmpD58))))));}{
struct _tuple21 _tmp7F7=Cyc_Toc_xlate_pat(nv,rgn,_tmp7F3,Cyc_Absyn_var_exp(_tmp7F2,
0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp7EC,fail_stmt,decls);nv=_tmp7F7.f1;
decls=_tmp7F7.f2;{struct Cyc_Absyn_Exp*_tmp7F8=Cyc_Absyn_aggrmember_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7D6->impl))->fields,f)))->type))
_tmp7F8=Cyc_Toc_cast_it(_tmp7F4,_tmp7F8);{struct Cyc_Absyn_Stmt*_tmp7F9=_tmp7F7.f3;
struct Cyc_Absyn_Stmt*_tmp7FA=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7F2,0),
_tmp7F8,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7FA,_tmp7F9,0),0);};};};};};}}
goto _LL334;}_LL353: {struct Cyc_Absyn_Pointer_p_struct*_tmp7B0=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp781;if(_tmp7B0->tag != 5)goto _LL355;else{_tmp7B1=_tmp7B0->f1;}}_LL354: {
struct _tuple0*_tmp7FB=Cyc_Toc_temp_var();void*_tmp7FC=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp7B1->topt))->v;{struct _tuple22*_tmpD5C;struct Cyc_List_List*
_tmpD5B;decls=((_tmpD5B=_region_malloc(rgn,sizeof(*_tmpD5B)),((_tmpD5B->hd=((
_tmpD5C=_region_malloc(rgn,sizeof(*_tmpD5C)),((_tmpD5C->f1=_tmp7FB,((_tmpD5C->f2=
Cyc_Toc_typ_to_c(_tmp7FC),_tmpD5C)))))),((_tmpD5B->tl=decls,_tmpD5B))))));}{
struct _tuple21 _tmp7FF=Cyc_Toc_xlate_pat(nv,rgn,_tmp7FC,Cyc_Absyn_var_exp(_tmp7FB,
0),Cyc_Absyn_deref_exp(path,0),_tmp7B1,fail_stmt,decls);nv=_tmp7FF.f1;decls=
_tmp7FF.f2;{struct Cyc_Absyn_Stmt*_tmp800=_tmp7FF.f3;struct Cyc_Absyn_Stmt*_tmp801=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7FB,0),Cyc_Absyn_deref_exp(
r,0),0),_tmp800,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp801,0);else{s=_tmp801;}goto _LL334;};};}_LL355: {struct Cyc_Absyn_UnknownId_p_struct*
_tmp7B2=(struct Cyc_Absyn_UnknownId_p_struct*)_tmp781;if(_tmp7B2->tag != 14)goto
_LL357;}_LL356: {const char*_tmpD5F;void*_tmpD5E;(_tmpD5E=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD5F="unknownid pat",
_tag_dyneither(_tmpD5F,sizeof(char),14))),_tag_dyneither(_tmpD5E,sizeof(void*),0)));}
_LL357: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp7B3=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp781;if(_tmp7B3->tag != 15)goto _LL359;}_LL358: {const char*_tmpD62;void*_tmpD61;(
_tmpD61=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD62="unknowncall pat",_tag_dyneither(_tmpD62,sizeof(char),16))),
_tag_dyneither(_tmpD61,sizeof(void*),0)));}_LL359: {struct Cyc_Absyn_Exp_p_struct*
_tmp7B4=(struct Cyc_Absyn_Exp_p_struct*)_tmp781;if(_tmp7B4->tag != 16)goto _LL334;}
_LL35A: {const char*_tmpD65;void*_tmpD64;(_tmpD64=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD65="exp pat",
_tag_dyneither(_tmpD65,sizeof(char),8))),_tag_dyneither(_tmpD64,sizeof(void*),0)));}
_LL334:;}{struct _tuple21 _tmpD66;return(_tmpD66.f1=nv,((_tmpD66.f2=decls,((
_tmpD66.f3=s,_tmpD66)))));};}struct _tuple24{struct _dyneither_ptr*f1;struct
_dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){struct _tuple24*_tmpD67;
return(_tmpD67=_region_malloc(r,sizeof(*_tmpD67)),((_tmpD67->f1=Cyc_Toc_fresh_label(),((
_tmpD67->f2=Cyc_Toc_fresh_label(),((_tmpD67->f3=sc,_tmpD67)))))));}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*_tmp80A=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;int leave_as_switch;{void*_tmp80B=Cyc_Tcutil_compress(
_tmp80A);_LL366: {struct Cyc_Absyn_IntType_struct*_tmp80C=(struct Cyc_Absyn_IntType_struct*)
_tmp80B;if(_tmp80C->tag != 6)goto _LL368;}_LL367: goto _LL369;_LL368: {struct Cyc_Absyn_EnumType_struct*
_tmp80D=(struct Cyc_Absyn_EnumType_struct*)_tmp80B;if(_tmp80D->tag != 14)goto
_LL36A;}_LL369: leave_as_switch=1;goto _LL365;_LL36A:;_LL36B: leave_as_switch=0;
goto _LL365;_LL365:;}{struct Cyc_List_List*_tmp80E=scs;for(0;_tmp80E != 0;_tmp80E=
_tmp80E->tl){if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Switch_clause*)_tmp80E->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)
_tmp80E->hd)->where_clause != 0){leave_as_switch=0;break;}}}if(leave_as_switch){
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{struct Cyc_List_List*_tmp80F=
scs;for(0;_tmp80F != 0;_tmp80F=_tmp80F->tl){struct Cyc_Absyn_Stmt*_tmp810=((struct
Cyc_Absyn_Switch_clause*)_tmp80F->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp80F->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp810,0);next_l=Cyc_Toc_fresh_label();{
struct Cyc_Toc_Env _tmp812;struct _RegionHandle*_tmp813;struct Cyc_Toc_Env*_tmp811=
nv;_tmp812=*_tmp811;_tmp813=_tmp812.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
_tmp813,nv,next_l),_tmp810);};}}return;}{struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp815;
struct _RegionHandle*_tmp816;struct Cyc_Toc_Env*_tmp814=nv;_tmp815=*_tmp814;
_tmp816=_tmp815.rgn;{struct Cyc_Toc_Env*_tmp817=Cyc_Toc_share_env(_tmp816,nv);
struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple24*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp816,Cyc_Toc_gen_label,
_tmp816,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct
Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp818=lscs;for(0;_tmp818 != 0;
_tmp818=_tmp818->tl){struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple24*)
_tmp818->hd)).f3;struct _dyneither_ptr*fail_lab=_tmp818->tl == 0?end_l:(*((struct
_tuple24*)((struct Cyc_List_List*)_check_null(_tmp818->tl))->hd)).f1;struct Cyc_Toc_Env*
_tmp81A;struct Cyc_List_List*_tmp81B;struct Cyc_Absyn_Stmt*_tmp81C;struct _tuple21
_tmp819=Cyc_Toc_xlate_pat(_tmp817,_tmp816,_tmp80A,r,path,sc->pattern,Cyc_Absyn_goto_stmt(
fail_lab,0),decls);_tmp81A=_tmp819.f1;_tmp81B=_tmp819.f2;_tmp81C=_tmp819.f3;if(
sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp81D=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp81A,_tmp81D);_tmp81C=Cyc_Absyn_seq_stmt(
_tmp81C,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp81D,0),
Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp81B;{
struct Cyc_List_List*_tmpD68;nvs=((_tmpD68=_region_malloc(_tmp816,sizeof(*_tmpD68)),((
_tmpD68->hd=_tmp81A,((_tmpD68->tl=nvs,_tmpD68))))));}{struct Cyc_List_List*
_tmpD69;test_stmts=((_tmpD69=_region_malloc(_tmp816,sizeof(*_tmpD69)),((_tmpD69->hd=
_tmp81C,((_tmpD69->tl=test_stmts,_tmpD69))))));};}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple24 _tmp821;struct _dyneither_ptr*_tmp822;struct _dyneither_ptr*_tmp823;
struct Cyc_Absyn_Switch_clause*_tmp824;struct _tuple24*_tmp820=(struct _tuple24*)
lscs->hd;_tmp821=*_tmp820;_tmp822=_tmp821.f1;_tmp823=_tmp821.f2;_tmp824=_tmp821.f3;{
struct Cyc_Toc_Env*_tmp825=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp824->body;struct Cyc_Toc_Env _tmp827;struct
_RegionHandle*_tmp828;struct Cyc_Toc_Env*_tmp826=_tmp817;_tmp827=*_tmp826;_tmp828=
_tmp827.rgn;if(lscs->tl != 0){struct _tuple24 _tmp82A;struct _dyneither_ptr*_tmp82B;
struct Cyc_Absyn_Switch_clause*_tmp82C;struct _tuple24*_tmp829=(struct _tuple24*)((
struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp82A=*_tmp829;_tmp82B=_tmp82A.f2;
_tmp82C=_tmp82A.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp828,
_tmp825,end_l,_tmp82B,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp82C->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp828,
_tmp825,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp822,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp823,s,0),0);{struct Cyc_List_List*_tmpD6A;stmts=((_tmpD6A=_region_malloc(
_tmp816,sizeof(*_tmpD6A)),((_tmpD6A->hd=s,((_tmpD6A->tl=stmts,_tmpD6A))))));};};}{
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,
Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple22
_tmp82F;struct _tuple0*_tmp830;void*_tmp831;struct _tuple22*_tmp82E=(struct
_tuple22*)decls->hd;_tmp82F=*_tmp82E;_tmp830=_tmp82F.f1;_tmp831=_tmp82F.f2;res=
Cyc_Absyn_declare_stmt(_tmp830,_tmp831,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(
v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(
struct Cyc_Absyn_Exp*)e,res,0))->r;};};};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){struct Cyc_List_List*
_tmpD6B;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,((
_tmpD6B=_cycalloc(sizeof(*_tmpD6B)),((_tmpD6B->hd=Cyc_Absyn_uint_exp((
unsigned int)(n - 1),0),((_tmpD6B->tl=0,_tmpD6B)))))),0),0);}void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s){while(1){void*_tmp833=s->r;struct Cyc_Absyn_Exp*_tmp836;
struct Cyc_Absyn_Stmt*_tmp838;struct Cyc_Absyn_Stmt*_tmp839;struct Cyc_Absyn_Exp*
_tmp83B;struct Cyc_Absyn_Exp*_tmp83D;struct Cyc_Absyn_Stmt*_tmp83E;struct Cyc_Absyn_Stmt*
_tmp83F;struct _tuple8 _tmp841;struct Cyc_Absyn_Exp*_tmp842;struct Cyc_Absyn_Stmt*
_tmp843;struct Cyc_Absyn_Stmt*_tmp845;struct Cyc_Absyn_Stmt*_tmp847;struct Cyc_Absyn_Stmt*
_tmp849;struct Cyc_Absyn_Exp*_tmp84B;struct _tuple8 _tmp84C;struct Cyc_Absyn_Exp*
_tmp84D;struct _tuple8 _tmp84E;struct Cyc_Absyn_Exp*_tmp84F;struct Cyc_Absyn_Stmt*
_tmp850;struct Cyc_Absyn_Exp*_tmp852;struct Cyc_List_List*_tmp853;struct Cyc_List_List*
_tmp855;struct Cyc_Absyn_Switch_clause**_tmp856;struct Cyc_Absyn_Decl*_tmp858;
struct Cyc_Absyn_Stmt*_tmp859;struct _dyneither_ptr*_tmp85B;struct Cyc_Absyn_Stmt*
_tmp85C;struct Cyc_Absyn_Stmt*_tmp85E;struct _tuple8 _tmp85F;struct Cyc_Absyn_Exp*
_tmp860;struct Cyc_Absyn_Stmt*_tmp862;struct Cyc_List_List*_tmp863;struct Cyc_Absyn_Exp*
_tmp865;_LL36D: {struct Cyc_Absyn_Skip_s_struct*_tmp834=(struct Cyc_Absyn_Skip_s_struct*)
_tmp833;if(_tmp834->tag != 0)goto _LL36F;}_LL36E: return;_LL36F: {struct Cyc_Absyn_Exp_s_struct*
_tmp835=(struct Cyc_Absyn_Exp_s_struct*)_tmp833;if(_tmp835->tag != 1)goto _LL371;
else{_tmp836=_tmp835->f1;}}_LL370: Cyc_Toc_exp_to_c(nv,_tmp836);return;_LL371: {
struct Cyc_Absyn_Seq_s_struct*_tmp837=(struct Cyc_Absyn_Seq_s_struct*)_tmp833;if(
_tmp837->tag != 2)goto _LL373;else{_tmp838=_tmp837->f1;_tmp839=_tmp837->f2;}}
_LL372: Cyc_Toc_stmt_to_c(nv,_tmp838);s=_tmp839;continue;_LL373: {struct Cyc_Absyn_Return_s_struct*
_tmp83A=(struct Cyc_Absyn_Return_s_struct*)_tmp833;if(_tmp83A->tag != 3)goto _LL375;
else{_tmp83B=_tmp83A->f1;}}_LL374: {struct Cyc_Core_Opt*topt=0;if(_tmp83B != 0){{
struct Cyc_Core_Opt*_tmpD6C;topt=((_tmpD6C=_cycalloc(sizeof(*_tmpD6C)),((_tmpD6C->v=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp83B->topt))->v),
_tmpD6C))));}Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp83B);}if(s->try_depth
> 0){if(topt != 0){struct _tuple0*_tmp867=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*
_tmp868=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp867,0),
0);s->r=(Cyc_Absyn_declare_stmt(_tmp867,(void*)topt->v,_tmp83B,Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler(s->try_depth),_tmp868,0),0))->r;}else{Cyc_Toc_do_npop_before(
s->try_depth,s);}}return;}_LL375: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp83C=(
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp833;if(_tmp83C->tag != 4)goto _LL377;
else{_tmp83D=_tmp83C->f1;_tmp83E=_tmp83C->f2;_tmp83F=_tmp83C->f3;}}_LL376: Cyc_Toc_exp_to_c(
nv,_tmp83D);Cyc_Toc_stmt_to_c(nv,_tmp83E);s=_tmp83F;continue;_LL377: {struct Cyc_Absyn_While_s_struct*
_tmp840=(struct Cyc_Absyn_While_s_struct*)_tmp833;if(_tmp840->tag != 5)goto _LL379;
else{_tmp841=_tmp840->f1;_tmp842=_tmp841.f1;_tmp843=_tmp840->f2;}}_LL378: Cyc_Toc_exp_to_c(
nv,_tmp842);{struct Cyc_Toc_Env _tmp86A;struct _RegionHandle*_tmp86B;struct Cyc_Toc_Env*
_tmp869=nv;_tmp86A=*_tmp869;_tmp86B=_tmp86A.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp86B,nv),_tmp843);return;};_LL379: {struct Cyc_Absyn_Break_s_struct*_tmp844=(
struct Cyc_Absyn_Break_s_struct*)_tmp833;if(_tmp844->tag != 6)goto _LL37B;else{
_tmp845=_tmp844->f1;}}_LL37A: {struct Cyc_Toc_Env _tmp86D;struct _dyneither_ptr**
_tmp86E;struct Cyc_Toc_Env*_tmp86C=nv;_tmp86D=*_tmp86C;_tmp86E=_tmp86D.break_lab;
if(_tmp86E != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp86E,0);{int dest_depth=_tmp845 == 0?0:
_tmp845->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;};}
_LL37B: {struct Cyc_Absyn_Continue_s_struct*_tmp846=(struct Cyc_Absyn_Continue_s_struct*)
_tmp833;if(_tmp846->tag != 7)goto _LL37D;else{_tmp847=_tmp846->f1;}}_LL37C: {
struct Cyc_Toc_Env _tmp870;struct _dyneither_ptr**_tmp871;struct Cyc_Toc_Env*_tmp86F=
nv;_tmp870=*_tmp86F;_tmp871=_tmp870.continue_lab;if(_tmp871 != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp871,0);_tmp849=_tmp847;goto _LL37E;}_LL37D: {struct Cyc_Absyn_Goto_s_struct*
_tmp848=(struct Cyc_Absyn_Goto_s_struct*)_tmp833;if(_tmp848->tag != 8)goto _LL37F;
else{_tmp849=_tmp848->f2;}}_LL37E: Cyc_Toc_do_npop_before(s->try_depth - ((struct
Cyc_Absyn_Stmt*)_check_null(_tmp849))->try_depth,s);return;_LL37F: {struct Cyc_Absyn_For_s_struct*
_tmp84A=(struct Cyc_Absyn_For_s_struct*)_tmp833;if(_tmp84A->tag != 9)goto _LL381;
else{_tmp84B=_tmp84A->f1;_tmp84C=_tmp84A->f2;_tmp84D=_tmp84C.f1;_tmp84E=_tmp84A->f3;
_tmp84F=_tmp84E.f1;_tmp850=_tmp84A->f4;}}_LL380: Cyc_Toc_exp_to_c(nv,_tmp84B);Cyc_Toc_exp_to_c(
nv,_tmp84D);Cyc_Toc_exp_to_c(nv,_tmp84F);{struct Cyc_Toc_Env _tmp873;struct
_RegionHandle*_tmp874;struct Cyc_Toc_Env*_tmp872=nv;_tmp873=*_tmp872;_tmp874=
_tmp873.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp874,nv),_tmp850);return;};
_LL381: {struct Cyc_Absyn_Switch_s_struct*_tmp851=(struct Cyc_Absyn_Switch_s_struct*)
_tmp833;if(_tmp851->tag != 10)goto _LL383;else{_tmp852=_tmp851->f1;_tmp853=_tmp851->f2;}}
_LL382: Cyc_Toc_xlate_switch(nv,s,_tmp852,_tmp853);return;_LL383: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp854=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp833;if(_tmp854->tag != 11)goto
_LL385;else{_tmp855=_tmp854->f1;_tmp856=_tmp854->f2;}}_LL384: {struct Cyc_Toc_Env
_tmp876;struct Cyc_Toc_FallthruInfo*_tmp877;struct Cyc_Toc_Env*_tmp875=nv;_tmp876=*
_tmp875;_tmp877=_tmp876.fallthru_info;if(_tmp877 == 0){const char*_tmpD6F;void*
_tmpD6E;(_tmpD6E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD6F="fallthru in unexpected place",_tag_dyneither(_tmpD6F,sizeof(char),29))),
_tag_dyneither(_tmpD6E,sizeof(void*),0)));}{struct _dyneither_ptr*_tmp87B;struct
Cyc_List_List*_tmp87C;struct Cyc_Dict_Dict _tmp87D;struct Cyc_Toc_FallthruInfo
_tmp87A=*_tmp877;_tmp87B=_tmp87A.label;_tmp87C=_tmp87A.binders;_tmp87D=_tmp87A.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp87B,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp856)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp87E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp87C);struct Cyc_List_List*_tmp87F=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp855);for(0;_tmp87E != 0;(_tmp87E=_tmp87E->tl,
_tmp87F=_tmp87F->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp87F))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp87D,(
struct _tuple0*)_tmp87E->hd),(struct Cyc_Absyn_Exp*)_tmp87F->hd,0),s2,0);}s->r=s2->r;
return;};};};}_LL385: {struct Cyc_Absyn_Decl_s_struct*_tmp857=(struct Cyc_Absyn_Decl_s_struct*)
_tmp833;if(_tmp857->tag != 12)goto _LL387;else{_tmp858=_tmp857->f1;_tmp859=_tmp857->f2;}}
_LL386:{void*_tmp880=_tmp858->r;struct Cyc_Absyn_Vardecl*_tmp882;struct Cyc_Absyn_Pat*
_tmp884;struct Cyc_Absyn_Exp*_tmp885;struct Cyc_List_List*_tmp887;struct Cyc_Absyn_Fndecl*
_tmp889;struct Cyc_Absyn_Tvar*_tmp88B;struct Cyc_Absyn_Vardecl*_tmp88C;int _tmp88D;
struct Cyc_Absyn_Exp*_tmp88E;struct Cyc_Absyn_Exp*_tmp890;struct Cyc_Absyn_Tvar*
_tmp891;struct Cyc_Absyn_Vardecl*_tmp892;_LL390: {struct Cyc_Absyn_Var_d_struct*
_tmp881=(struct Cyc_Absyn_Var_d_struct*)_tmp880;if(_tmp881->tag != 0)goto _LL392;
else{_tmp882=_tmp881->f1;}}_LL391: {struct Cyc_Toc_Env _tmp894;struct _RegionHandle*
_tmp895;struct Cyc_Toc_Env*_tmp893=nv;_tmp894=*_tmp893;_tmp895=_tmp894.rgn;{
struct Cyc_Absyn_Local_b_struct _tmpD72;struct Cyc_Absyn_Local_b_struct*_tmpD71;
struct Cyc_Toc_Env*_tmp896=Cyc_Toc_add_varmap(_tmp895,nv,_tmp882->name,Cyc_Absyn_varb_exp(
_tmp882->name,(void*)((_tmpD71=_cycalloc(sizeof(*_tmpD71)),((_tmpD71[0]=((
_tmpD72.tag=4,((_tmpD72.f1=_tmp882,_tmpD72)))),_tmpD71)))),0));Cyc_Toc_local_decl_to_c(
_tmp896,_tmp896,_tmp882,_tmp859);}goto _LL38F;}_LL392: {struct Cyc_Absyn_Let_d_struct*
_tmp883=(struct Cyc_Absyn_Let_d_struct*)_tmp880;if(_tmp883->tag != 2)goto _LL394;
else{_tmp884=_tmp883->f1;_tmp885=_tmp883->f3;}}_LL393:{void*_tmp899=_tmp884->r;
struct Cyc_Absyn_Vardecl*_tmp89B;struct Cyc_Absyn_Pat*_tmp89C;struct Cyc_Absyn_Pat
_tmp89D;void*_tmp89E;_LL39F: {struct Cyc_Absyn_Var_p_struct*_tmp89A=(struct Cyc_Absyn_Var_p_struct*)
_tmp899;if(_tmp89A->tag != 1)goto _LL3A1;else{_tmp89B=_tmp89A->f1;_tmp89C=_tmp89A->f2;
_tmp89D=*_tmp89C;_tmp89E=_tmp89D.r;{struct Cyc_Absyn_Wild_p_struct*_tmp89F=(
struct Cyc_Absyn_Wild_p_struct*)_tmp89E;if(_tmp89F->tag != 0)goto _LL3A1;};}}_LL3A0: {
struct _tuple0*old_name=_tmp89B->name;struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp89B->name=new_name;_tmp89B->initializer=(struct Cyc_Absyn_Exp*)_tmp885;{
struct Cyc_Absyn_Var_d_struct _tmpD75;struct Cyc_Absyn_Var_d_struct*_tmpD74;_tmp858->r=(
void*)((_tmpD74=_cycalloc(sizeof(*_tmpD74)),((_tmpD74[0]=((_tmpD75.tag=0,((
_tmpD75.f1=_tmp89B,_tmpD75)))),_tmpD74))));}{struct Cyc_Toc_Env _tmp8A3;struct
_RegionHandle*_tmp8A4;struct Cyc_Toc_Env*_tmp8A2=nv;_tmp8A3=*_tmp8A2;_tmp8A4=
_tmp8A3.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD78;struct Cyc_Absyn_Local_b_struct*
_tmpD77;struct Cyc_Toc_Env*_tmp8A5=Cyc_Toc_add_varmap(_tmp8A4,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD77=_cycalloc(sizeof(*_tmpD77)),((_tmpD77[0]=((_tmpD78.tag=
4,((_tmpD78.f1=_tmp89B,_tmpD78)))),_tmpD77)))),0));Cyc_Toc_local_decl_to_c(
_tmp8A5,nv,_tmp89B,_tmp859);}goto _LL39E;};}_LL3A1:;_LL3A2: s->r=(Cyc_Toc_letdecl_to_c(
nv,_tmp884,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp885->topt))->v,_tmp885,
_tmp859))->r;goto _LL39E;_LL39E:;}goto _LL38F;_LL394: {struct Cyc_Absyn_Letv_d_struct*
_tmp886=(struct Cyc_Absyn_Letv_d_struct*)_tmp880;if(_tmp886->tag != 3)goto _LL396;
else{_tmp887=_tmp886->f1;}}_LL395: {struct Cyc_List_List*_tmp8A8=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp887);if(_tmp8A8 == 0){const char*_tmpD7B;
void*_tmpD7A;(_tmpD7A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpD7B="empty Letv_d",_tag_dyneither(_tmpD7B,sizeof(char),
13))),_tag_dyneither(_tmpD7A,sizeof(void*),0)));}{struct Cyc_Absyn_Var_d_struct
_tmpD7E;struct Cyc_Absyn_Var_d_struct*_tmpD7D;_tmp858->r=(void*)((_tmpD7D=
_cycalloc(sizeof(*_tmpD7D)),((_tmpD7D[0]=((_tmpD7E.tag=0,((_tmpD7E.f1=(struct Cyc_Absyn_Vardecl*)
_tmp8A8->hd,_tmpD7E)))),_tmpD7D))));}_tmp8A8=_tmp8A8->tl;for(0;_tmp8A8 != 0;
_tmp8A8=_tmp8A8->tl){struct Cyc_Absyn_Var_d_struct _tmpD81;struct Cyc_Absyn_Var_d_struct*
_tmpD80;struct Cyc_Absyn_Decl*_tmp8AD=Cyc_Absyn_new_decl((void*)((_tmpD80=
_cycalloc(sizeof(*_tmpD80)),((_tmpD80[0]=((_tmpD81.tag=0,((_tmpD81.f1=(struct Cyc_Absyn_Vardecl*)
_tmp8A8->hd,_tmpD81)))),_tmpD80)))),0);s->r=(Cyc_Absyn_decl_stmt(_tmp8AD,Cyc_Absyn_new_stmt(
s->r,0),0))->r;}Cyc_Toc_stmt_to_c(nv,s);goto _LL38F;}_LL396: {struct Cyc_Absyn_Fn_d_struct*
_tmp888=(struct Cyc_Absyn_Fn_d_struct*)_tmp880;if(_tmp888->tag != 1)goto _LL398;
else{_tmp889=_tmp888->f1;}}_LL397: {struct _tuple0*_tmp8B0=_tmp889->name;struct
Cyc_Toc_Env _tmp8B2;struct _RegionHandle*_tmp8B3;struct Cyc_Toc_Env*_tmp8B1=nv;
_tmp8B2=*_tmp8B1;_tmp8B3=_tmp8B2.rgn;{struct Cyc_Toc_Env*_tmp8B4=Cyc_Toc_add_varmap(
_tmp8B3,nv,_tmp889->name,Cyc_Absyn_var_exp(_tmp8B0,0));Cyc_Toc_fndecl_to_c(
_tmp8B4,_tmp889,0);Cyc_Toc_stmt_to_c(_tmp8B4,_tmp859);}goto _LL38F;}_LL398: {
struct Cyc_Absyn_Region_d_struct*_tmp88A=(struct Cyc_Absyn_Region_d_struct*)
_tmp880;if(_tmp88A->tag != 4)goto _LL39A;else{_tmp88B=_tmp88A->f1;_tmp88C=_tmp88A->f2;
_tmp88D=_tmp88A->f3;_tmp88E=_tmp88A->f4;}}_LL399: {struct Cyc_Absyn_Stmt*_tmp8B5=
_tmp859;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple0*
rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp88C->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp8B7;struct _RegionHandle*_tmp8B8;struct Cyc_Toc_Env*
_tmp8B6=nv;_tmp8B7=*_tmp8B6;_tmp8B8=_tmp8B7.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp8B8,nv,x_var,x_exp),_tmp8B5);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp8B5,0))->r;
else{if(_tmp88E == 0){struct Cyc_Absyn_Exp*_tmpD84[1];struct Cyc_Absyn_Exp*_tmpD83[
1];struct Cyc_List_List*_tmpD82;s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpD82=
_cycalloc(sizeof(*_tmpD82)),((_tmpD82->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0),((_tmpD82->tl=0,_tmpD82)))))),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD83[0]=
x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD83,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp8B5,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD84[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD84,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp88E);{struct Cyc_Absyn_Exp*
_tmpD86[1];struct Cyc_Absyn_Exp*_tmpD85[2];s->r=(Cyc_Absyn_declare_stmt(rh_var,
Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpD85[1]=(struct Cyc_Absyn_Exp*)_tmp88E,((_tmpD85[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD85,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp8B5,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpD86[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD86,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
return;}_LL39A: {struct Cyc_Absyn_Alias_d_struct*_tmp88F=(struct Cyc_Absyn_Alias_d_struct*)
_tmp880;if(_tmp88F->tag != 5)goto _LL39C;else{_tmp890=_tmp88F->f1;_tmp891=_tmp88F->f2;
_tmp892=_tmp88F->f3;}}_LL39B: {struct _tuple0*old_name=_tmp892->name;struct
_tuple0*new_name=Cyc_Toc_temp_var();_tmp892->name=new_name;_tmp892->initializer=(
struct Cyc_Absyn_Exp*)_tmp890;{struct Cyc_Absyn_Decl_s_struct _tmpD95;struct Cyc_Absyn_Var_d_struct*
_tmpD94;struct Cyc_Absyn_Var_d_struct _tmpD93;struct Cyc_Absyn_Decl*_tmpD92;struct
Cyc_Absyn_Decl_s_struct*_tmpD91;s->r=(void*)((_tmpD91=_cycalloc(sizeof(*_tmpD91)),((
_tmpD91[0]=((_tmpD95.tag=12,((_tmpD95.f1=((_tmpD92=_cycalloc(sizeof(*_tmpD92)),((
_tmpD92->r=(void*)((_tmpD94=_cycalloc(sizeof(*_tmpD94)),((_tmpD94[0]=((_tmpD93.tag=
0,((_tmpD93.f1=_tmp892,_tmpD93)))),_tmpD94)))),((_tmpD92->loc=0,_tmpD92)))))),((
_tmpD95.f2=_tmp859,_tmpD95)))))),_tmpD91))));}{struct Cyc_Toc_Env _tmp8C4;struct
_RegionHandle*_tmp8C5;struct Cyc_Toc_Env*_tmp8C3=nv;_tmp8C4=*_tmp8C3;_tmp8C5=
_tmp8C4.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD98;struct Cyc_Absyn_Local_b_struct*
_tmpD97;struct Cyc_Toc_Env*_tmp8C6=Cyc_Toc_add_varmap(_tmp8C5,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD97=_cycalloc(sizeof(*_tmpD97)),((_tmpD97[0]=((_tmpD98.tag=
4,((_tmpD98.f1=_tmp892,_tmpD98)))),_tmpD97)))),0));Cyc_Toc_local_decl_to_c(
_tmp8C6,nv,_tmp892,_tmp859);}return;};}_LL39C:;_LL39D: {const char*_tmpD9B;void*
_tmpD9A;(_tmpD9A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD9B="bad nested declaration within function",_tag_dyneither(_tmpD9B,sizeof(
char),39))),_tag_dyneither(_tmpD9A,sizeof(void*),0)));}_LL38F:;}return;_LL387: {
struct Cyc_Absyn_Label_s_struct*_tmp85A=(struct Cyc_Absyn_Label_s_struct*)_tmp833;
if(_tmp85A->tag != 13)goto _LL389;else{_tmp85B=_tmp85A->f1;_tmp85C=_tmp85A->f2;}}
_LL388: s=_tmp85C;continue;_LL389: {struct Cyc_Absyn_Do_s_struct*_tmp85D=(struct
Cyc_Absyn_Do_s_struct*)_tmp833;if(_tmp85D->tag != 14)goto _LL38B;else{_tmp85E=
_tmp85D->f1;_tmp85F=_tmp85D->f2;_tmp860=_tmp85F.f1;}}_LL38A: {struct Cyc_Toc_Env
_tmp8CC;struct _RegionHandle*_tmp8CD;struct Cyc_Toc_Env*_tmp8CB=nv;_tmp8CC=*
_tmp8CB;_tmp8CD=_tmp8CC.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp8CD,nv),
_tmp85E);Cyc_Toc_exp_to_c(nv,_tmp860);return;}_LL38B: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp861=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp833;if(_tmp861->tag != 15)goto
_LL38D;else{_tmp862=_tmp861->f1;_tmp863=_tmp861->f2;}}_LL38C: {struct _tuple0*
h_var=Cyc_Toc_temp_var();struct _tuple0*e_var=Cyc_Toc_temp_var();struct _tuple0*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);{struct Cyc_Core_Opt*_tmpD9C;
e_exp->topt=((_tmpD9C=_cycalloc(sizeof(*_tmpD9C)),((_tmpD9C->v=e_typ,_tmpD9C))));}{
struct Cyc_Toc_Env _tmp8D0;struct _RegionHandle*_tmp8D1;struct Cyc_Toc_Env*_tmp8CF=
nv;_tmp8D0=*_tmp8CF;_tmp8D1=_tmp8D0.rgn;{struct Cyc_Toc_Env*_tmp8D2=Cyc_Toc_add_varmap(
_tmp8D1,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp8D2,_tmp862);{struct Cyc_Absyn_Stmt*
_tmp8D3=Cyc_Absyn_seq_stmt(_tmp862,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple0*_tmp8D4=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp8D5=
Cyc_Absyn_var_exp(_tmp8D4,0);struct Cyc_Absyn_Vardecl*_tmp8D6=Cyc_Absyn_new_vardecl(
_tmp8D4,Cyc_Absyn_exn_typ(),0);{struct Cyc_Core_Opt*_tmpD9D;_tmp8D5->topt=((
_tmpD9D=_cycalloc(sizeof(*_tmpD9D)),((_tmpD9D->v=Cyc_Absyn_exn_typ(),_tmpD9D))));}{
struct Cyc_Core_Opt*_tmpDAE;struct Cyc_Absyn_Var_p_struct*_tmpDAD;struct Cyc_Absyn_Pat*
_tmpDAC;struct Cyc_Core_Opt*_tmpDAB;struct Cyc_Absyn_Var_p_struct _tmpDAA;struct Cyc_Absyn_Pat*
_tmpDA9;struct Cyc_Absyn_Pat*_tmp8D8=(_tmpDA9=_cycalloc(sizeof(*_tmpDA9)),((
_tmpDA9->r=(void*)((_tmpDAD=_cycalloc(sizeof(*_tmpDAD)),((_tmpDAD[0]=((_tmpDAA.tag=
1,((_tmpDAA.f1=_tmp8D6,((_tmpDAA.f2=((_tmpDAC=_cycalloc(sizeof(*_tmpDAC)),((
_tmpDAC->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpDAC->topt=((_tmpDAB=_cycalloc(
sizeof(*_tmpDAB)),((_tmpDAB->v=Cyc_Absyn_exn_typ(),_tmpDAB)))),((_tmpDAC->loc=0,
_tmpDAC)))))))),_tmpDAA)))))),_tmpDAD)))),((_tmpDA9->topt=((_tmpDAE=_cycalloc(
sizeof(*_tmpDAE)),((_tmpDAE->v=Cyc_Absyn_exn_typ(),_tmpDAE)))),((_tmpDA9->loc=0,
_tmpDA9)))))));struct Cyc_Absyn_Exp*_tmp8D9=Cyc_Absyn_throw_exp(_tmp8D5,0);{
struct Cyc_Core_Opt*_tmpDAF;_tmp8D9->topt=((_tmpDAF=_cycalloc(sizeof(*_tmpDAF)),((
_tmpDAF->v=(void*)& Cyc_Absyn_VoidType_val,_tmpDAF))));}{struct Cyc_Absyn_Stmt*
_tmp8DB=Cyc_Absyn_exp_stmt(_tmp8D9,0);struct Cyc_Core_Opt*_tmpDB5;struct Cyc_List_List*
_tmpDB4;struct Cyc_Absyn_Switch_clause*_tmpDB3;struct Cyc_Absyn_Switch_clause*
_tmp8DC=(_tmpDB3=_cycalloc(sizeof(*_tmpDB3)),((_tmpDB3->pattern=_tmp8D8,((
_tmpDB3->pat_vars=((_tmpDB5=_cycalloc(sizeof(*_tmpDB5)),((_tmpDB5->v=((_tmpDB4=
_cycalloc(sizeof(*_tmpDB4)),((_tmpDB4->hd=_tmp8D6,((_tmpDB4->tl=0,_tmpDB4)))))),
_tmpDB5)))),((_tmpDB3->where_clause=0,((_tmpDB3->body=_tmp8DB,((_tmpDB3->loc=0,
_tmpDB3)))))))))));struct Cyc_List_List*_tmpDB6;struct Cyc_Absyn_Stmt*_tmp8DD=Cyc_Absyn_switch_stmt(
e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp863,((_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6->hd=_tmp8DC,((_tmpDB6->tl=
0,_tmpDB6))))))),0);Cyc_Toc_stmt_to_c(_tmp8D2,_tmp8DD);{struct Cyc_List_List*
_tmpDB7;struct Cyc_Absyn_Exp*_tmp8DE=Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,((
_tmpDB7=_cycalloc(sizeof(*_tmpDB7)),((_tmpDB7->hd=Cyc_Absyn_aggrmember_exp(h_exp,
Cyc_Toc_handler_sp,0),((_tmpDB7->tl=0,_tmpDB7)))))),0);struct Cyc_List_List*
_tmpDB8;struct Cyc_Absyn_Stmt*_tmp8DF=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpDB8=_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpDB8->tl=0,_tmpDB8)))))),0),0);struct Cyc_Absyn_Exp*_tmp8E0=Cyc_Absyn_int_exp(
Cyc_Absyn_Signed,0,0);struct Cyc_Absyn_Exp*_tmp8E1=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
1,0);s->r=(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp8DF,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp8E0,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp8DE,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp8E1,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp8D3,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp8DD,0),0),0),0),0),0))->r;};};};};}
return;};}_LL38D: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp864=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp833;if(_tmp864->tag != 16)goto _LL36C;else{_tmp865=_tmp864->f1;}}_LL38E: if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp865);{struct Cyc_List_List*
_tmpDB9;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((
_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9->hd=_tmp865,((_tmpDB9->tl=0,
_tmpDB9)))))),0));};}return;_LL36C:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);struct _tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8EF=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8EF;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp8F0=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp8F1=f->args;for(0;_tmp8F1 != 0;_tmp8F1=_tmp8F1->tl){struct
_tuple0*_tmpDBA;struct _tuple0*_tmp8F2=(_tmpDBA=_cycalloc(sizeof(*_tmpDBA)),((
_tmpDBA->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpDBA->f2=(*((struct
_tuple25*)_tmp8F1->hd)).f1,_tmpDBA)))));(*((struct _tuple25*)_tmp8F1->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple25*)_tmp8F1->hd)).f3);_tmp8F0=Cyc_Toc_add_varmap(frgn,_tmp8F0,
_tmp8F2,Cyc_Absyn_var_exp(_tmp8F2,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp8F5;struct Cyc_Absyn_Tqual
_tmp8F6;void*_tmp8F7;int _tmp8F8;struct Cyc_Absyn_VarargInfo _tmp8F4=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp8F5=_tmp8F4.name;_tmp8F6=_tmp8F4.tq;_tmp8F7=
_tmp8F4.type;_tmp8F8=_tmp8F4.inject;{void*_tmp8F9=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp8F7,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8F6,Cyc_Absyn_false_conref));struct
_tuple0*_tmpDBB;struct _tuple0*_tmp8FA=(_tmpDBB=_cycalloc(sizeof(*_tmpDBB)),((
_tmpDBB->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpDBB->f2=(struct
_dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null(_tmp8F5))->v,_tmpDBB)))));{
struct _tuple25*_tmpDBE;struct Cyc_List_List*_tmpDBD;f->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpDBD=
_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD->hd=((_tmpDBE=_cycalloc(sizeof(*_tmpDBE)),((
_tmpDBE->f1=(struct _dyneither_ptr*)_tmp8F5->v,((_tmpDBE->f2=_tmp8F6,((_tmpDBE->f3=
_tmp8F9,_tmpDBE)))))))),((_tmpDBD->tl=0,_tmpDBD)))))));}_tmp8F0=Cyc_Toc_add_varmap(
frgn,_tmp8F0,_tmp8FA,Cyc_Absyn_var_exp(_tmp8FA,0));f->cyc_varargs=0;};}{struct
Cyc_List_List*_tmp8FE=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp8FE != 0;_tmp8FE=_tmp8FE->tl){((struct Cyc_Absyn_Vardecl*)_tmp8FE->hd)->type=
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp8FE->hd)->type);}}Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel(frgn,_tmp8F0),f->body);};_pop_region(frgn);};}static enum 
Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){switch(s){case Cyc_Absyn_Abstract:
_LL3A3: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3A4: return Cyc_Absyn_Extern;
default: _LL3A5: return s;}}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad,int add_to_decls){struct _tuple0*_tmp8FF=ad->name;struct _DynRegionHandle*
_tmp901;struct Cyc_Dict_Dict*_tmp902;struct Cyc_Toc_TocState _tmp900=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp901=_tmp900.dyn;_tmp902=_tmp900.aggrs_so_far;{
struct _DynRegionFrame _tmp903;struct _RegionHandle*d=_open_dynregion(& _tmp903,
_tmp901);{int seen_defn_before;struct _tuple14**_tmp904=((struct _tuple14**(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp902,_tmp8FF);if(
_tmp904 == 0){seen_defn_before=0;{struct _tuple14*v;if(ad->kind == Cyc_Absyn_StructA){
struct _tuple14*_tmpDBF;v=((_tmpDBF=_region_malloc(d,sizeof(*_tmpDBF)),((_tmpDBF->f1=
ad,((_tmpDBF->f2=Cyc_Absyn_strctq(_tmp8FF),_tmpDBF))))));}else{struct _tuple14*
_tmpDC0;v=((_tmpDC0=_region_malloc(d,sizeof(*_tmpDC0)),((_tmpDC0->f1=ad,((
_tmpDC0->f2=Cyc_Absyn_unionq_typ(_tmp8FF),_tmpDC0))))));}*_tmp902=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp902,
_tmp8FF,v);};}else{struct _tuple14 _tmp908;struct Cyc_Absyn_Aggrdecl*_tmp909;void*
_tmp90A;struct _tuple14*_tmp907=*_tmp904;_tmp908=*_tmp907;_tmp909=_tmp908.f1;
_tmp90A=_tmp908.f2;if(_tmp909->impl == 0){{struct _tuple14*_tmpDC1;*_tmp902=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))
Cyc_Dict_insert)(*_tmp902,_tmp8FF,((_tmpDC1=_region_malloc(d,sizeof(*_tmpDC1)),((
_tmpDC1->f1=ad,((_tmpDC1->f2=_tmp90A,_tmpDC1)))))));}seen_defn_before=0;}else{
seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*_tmpDC2;struct Cyc_Absyn_Aggrdecl*
new_ad=(_tmpDC2=_cycalloc(sizeof(*_tmpDC2)),((_tmpDC2->kind=ad->kind,((_tmpDC2->sc=
Cyc_Absyn_Public,((_tmpDC2->name=ad->name,((_tmpDC2->tvs=0,((_tmpDC2->impl=0,((
_tmpDC2->attributes=ad->attributes,_tmpDC2)))))))))))));if(ad->impl != 0  && !
seen_defn_before){{struct Cyc_Absyn_AggrdeclImpl*_tmpDC3;new_ad->impl=((_tmpDC3=
_cycalloc(sizeof(*_tmpDC3)),((_tmpDC3->exist_vars=0,((_tmpDC3->rgn_po=0,((
_tmpDC3->fields=0,((_tmpDC3->tagged=0,_tmpDC3))))))))));}{struct Cyc_List_List*
new_fields=0;{struct Cyc_List_List*_tmp90D=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp90D != 0;_tmp90D=_tmp90D->tl){struct Cyc_Absyn_Aggrfield*
_tmp90E=(struct Cyc_Absyn_Aggrfield*)_tmp90D->hd;struct Cyc_Absyn_Aggrfield*
_tmpDC4;struct Cyc_Absyn_Aggrfield*_tmp90F=(_tmpDC4=_cycalloc(sizeof(*_tmpDC4)),((
_tmpDC4->name=_tmp90E->name,((_tmpDC4->tq=Cyc_Toc_mt_tq,((_tmpDC4->type=Cyc_Toc_typ_to_c_array(
_tmp90E->type),((_tmpDC4->width=_tmp90E->width,((_tmpDC4->attributes=_tmp90E->attributes,
_tmpDC4)))))))))));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp910=_tmp90F->type;struct _dyneither_ptr*_tmp911=_tmp90F->name;const char*
_tmpDC9;void*_tmpDC8[2];struct Cyc_String_pa_struct _tmpDC7;struct Cyc_String_pa_struct
_tmpDC6;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpDC6.tag=0,((_tmpDC6.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp911),((_tmpDC7.tag=0,((_tmpDC7.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpDC8[0]=&
_tmpDC7,((_tmpDC8[1]=& _tmpDC6,Cyc_aprintf(((_tmpDC9="_union_%s_%s",
_tag_dyneither(_tmpDC9,sizeof(char),13))),_tag_dyneither(_tmpDC8,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpDCA;struct _dyneither_ptr*str=(_tmpDCA=_cycalloc(sizeof(*
_tmpDCA)),((_tmpDCA[0]=s,_tmpDCA)));struct Cyc_Absyn_Aggrfield*_tmpDCB;struct Cyc_Absyn_Aggrfield*
_tmp912=(_tmpDCB=_cycalloc(sizeof(*_tmpDCB)),((_tmpDCB->name=Cyc_Toc_val_sp,((
_tmpDCB->tq=Cyc_Toc_mt_tq,((_tmpDCB->type=_tmp910,((_tmpDCB->width=0,((_tmpDCB->attributes=
0,_tmpDCB)))))))))));struct Cyc_Absyn_Aggrfield*_tmpDCC;struct Cyc_Absyn_Aggrfield*
_tmp913=(_tmpDCC=_cycalloc(sizeof(*_tmpDCC)),((_tmpDCC->name=Cyc_Toc_tag_sp,((
_tmpDCC->tq=Cyc_Toc_mt_tq,((_tmpDCC->type=Cyc_Absyn_sint_typ,((_tmpDCC->width=0,((
_tmpDCC->attributes=0,_tmpDCC)))))))))));struct Cyc_Absyn_Aggrfield*_tmpDCD[2];
struct Cyc_List_List*_tmp914=(_tmpDCD[1]=_tmp912,((_tmpDCD[0]=_tmp913,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDCD,sizeof(struct Cyc_Absyn_Aggrfield*),
2)))));struct Cyc_Absyn_AggrdeclImpl*_tmpDD2;struct _tuple0*_tmpDD1;struct Cyc_Absyn_Aggrdecl*
_tmpDD0;struct Cyc_Absyn_Aggrdecl*_tmp915=(_tmpDD0=_cycalloc(sizeof(*_tmpDD0)),((
_tmpDD0->kind=Cyc_Absyn_StructA,((_tmpDD0->sc=Cyc_Absyn_Public,((_tmpDD0->name=((
_tmpDD1=_cycalloc(sizeof(*_tmpDD1)),((_tmpDD1->f1=Cyc_Absyn_Loc_n,((_tmpDD1->f2=
str,_tmpDD1)))))),((_tmpDD0->tvs=0,((_tmpDD0->impl=((_tmpDD2=_cycalloc(sizeof(*
_tmpDD2)),((_tmpDD2->exist_vars=0,((_tmpDD2->rgn_po=0,((_tmpDD2->fields=_tmp914,((
_tmpDD2->tagged=0,_tmpDD2)))))))))),((_tmpDD0->attributes=0,_tmpDD0)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmpDD8;struct Cyc_Absyn_Aggr_d_struct _tmpDD7;
struct Cyc_List_List*_tmpDD6;Cyc_Toc_result_decls=((_tmpDD6=_cycalloc(sizeof(*
_tmpDD6)),((_tmpDD6->hd=Cyc_Absyn_new_decl((void*)((_tmpDD8=_cycalloc(sizeof(*
_tmpDD8)),((_tmpDD8[0]=((_tmpDD7.tag=6,((_tmpDD7.f1=_tmp915,_tmpDD7)))),_tmpDD8)))),
0),((_tmpDD6->tl=Cyc_Toc_result_decls,_tmpDD6))))));}_tmp90F->type=Cyc_Absyn_strct(
str);}{struct Cyc_List_List*_tmpDD9;new_fields=((_tmpDD9=_cycalloc(sizeof(*
_tmpDD9)),((_tmpDD9->hd=_tmp90F,((_tmpDD9->tl=new_fields,_tmpDD9))))));};}}(
new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_fields);};}if(add_to_decls){struct Cyc_Absyn_Decl*_tmpDE3;struct Cyc_Absyn_Aggr_d_struct
_tmpDE2;struct Cyc_Absyn_Aggr_d_struct*_tmpDE1;struct Cyc_List_List*_tmpDE0;Cyc_Toc_result_decls=((
_tmpDE0=_cycalloc(sizeof(*_tmpDE0)),((_tmpDE0->hd=((_tmpDE3=_cycalloc(sizeof(*
_tmpDE3)),((_tmpDE3->r=(void*)((_tmpDE1=_cycalloc(sizeof(*_tmpDE1)),((_tmpDE1[0]=((
_tmpDE2.tag=6,((_tmpDE2.f1=new_ad,_tmpDE2)))),_tmpDE1)))),((_tmpDE3->loc=0,
_tmpDE3)))))),((_tmpDE0->tl=Cyc_Toc_result_decls,_tmpDE0))))));}};};
_pop_dynregion(d);};}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud){struct _DynRegionHandle*_tmp92C;struct Cyc_Set_Set**_tmp92D;struct Cyc_Toc_TocState
_tmp92B=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp92C=
_tmp92B.dyn;_tmp92D=_tmp92B.datatypes_so_far;{struct _DynRegionFrame _tmp92E;
struct _RegionHandle*d=_open_dynregion(& _tmp92E,_tmp92C);{struct _tuple0*_tmp92F=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))
Cyc_Set_member)(*_tmp92D,_tmp92F)){_npop_handler(0);return;}*_tmp92D=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*
_tmp92D,_tmp92F);}{struct Cyc_List_List*_tmp930=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp930 != 0;_tmp930=_tmp930->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp930->hd;struct Cyc_List_List*_tmp931=0;int i=
1;{struct Cyc_List_List*_tmp932=f->typs;for(0;_tmp932 != 0;(_tmp932=_tmp932->tl,i
++)){struct _dyneither_ptr*_tmp933=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpDE4;struct Cyc_Absyn_Aggrfield*_tmp934=(_tmpDE4=_cycalloc(sizeof(*_tmpDE4)),((
_tmpDE4->name=_tmp933,((_tmpDE4->tq=(*((struct _tuple9*)_tmp932->hd)).f1,((
_tmpDE4->type=Cyc_Toc_typ_to_c_array((*((struct _tuple9*)_tmp932->hd)).f2),((
_tmpDE4->width=0,((_tmpDE4->attributes=0,_tmpDE4)))))))))));struct Cyc_List_List*
_tmpDE5;_tmp931=((_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5->hd=_tmp934,((
_tmpDE5->tl=_tmp931,_tmpDE5))))));}}{struct Cyc_Absyn_Aggrfield*_tmpDE8;struct Cyc_List_List*
_tmpDE7;_tmp931=((_tmpDE7=_cycalloc(sizeof(*_tmpDE7)),((_tmpDE7->hd=((_tmpDE8=
_cycalloc(sizeof(*_tmpDE8)),((_tmpDE8->name=Cyc_Toc_tag_sp,((_tmpDE8->tq=Cyc_Toc_mt_tq,((
_tmpDE8->type=Cyc_Absyn_sint_typ,((_tmpDE8->width=0,((_tmpDE8->attributes=0,
_tmpDE8)))))))))))),((_tmpDE7->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp931),_tmpDE7))))));}{struct Cyc_Absyn_AggrdeclImpl*
_tmpDED;const char*_tmpDEC;struct Cyc_Absyn_Aggrdecl*_tmpDEB;struct Cyc_Absyn_Aggrdecl*
_tmp939=(_tmpDEB=_cycalloc(sizeof(*_tmpDEB)),((_tmpDEB->kind=Cyc_Absyn_StructA,((
_tmpDEB->sc=Cyc_Absyn_Public,((_tmpDEB->name=Cyc_Toc_collapse_qvar_tag(f->name,((
_tmpDEC="_struct",_tag_dyneither(_tmpDEC,sizeof(char),8)))),((_tmpDEB->tvs=0,((
_tmpDEB->impl=((_tmpDED=_cycalloc(sizeof(*_tmpDED)),((_tmpDED->exist_vars=0,((
_tmpDED->rgn_po=0,((_tmpDED->fields=_tmp931,((_tmpDED->tagged=0,_tmpDED)))))))))),((
_tmpDEB->attributes=0,_tmpDEB)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpDF3;
struct Cyc_Absyn_Aggr_d_struct _tmpDF2;struct Cyc_List_List*_tmpDF1;Cyc_Toc_result_decls=((
_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((_tmpDF1->hd=Cyc_Absyn_new_decl((void*)((
_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((_tmpDF3[0]=((_tmpDF2.tag=6,((_tmpDF2.f1=
_tmp939,_tmpDF2)))),_tmpDF3)))),0),((_tmpDF1->tl=Cyc_Toc_result_decls,_tmpDF1))))));};}};;
_pop_dynregion(d);};}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd){if(xd->fields == 0)return;{struct _DynRegionHandle*_tmp941;struct Cyc_Dict_Dict*
_tmp942;struct Cyc_Toc_TocState _tmp940=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp941=_tmp940.dyn;_tmp942=_tmp940.xdatatypes_so_far;{struct _DynRegionFrame
_tmp943;struct _RegionHandle*d=_open_dynregion(& _tmp943,_tmp941);{struct _tuple0*
_tmp944=xd->name;struct Cyc_List_List*_tmp945=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp945 != 0;_tmp945=_tmp945->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp945->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp946=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp947=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp946,Cyc_Absyn_false_conref,0);int*_tmp948=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp942,f->name);int
_tmp949;_LL3A8: if(_tmp948 != 0)goto _LL3AA;_LL3A9: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp94A=Cyc_Absyn_new_vardecl(f->name,_tmp947,
initopt);_tmp94A->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*_tmpDF9;struct Cyc_Absyn_Var_d_struct
_tmpDF8;struct Cyc_List_List*_tmpDF7;Cyc_Toc_result_decls=((_tmpDF7=_cycalloc(
sizeof(*_tmpDF7)),((_tmpDF7->hd=Cyc_Absyn_new_decl((void*)((_tmpDF9=_cycalloc(
sizeof(*_tmpDF9)),((_tmpDF9[0]=((_tmpDF8.tag=0,((_tmpDF8.f1=_tmp94A,_tmpDF8)))),
_tmpDF9)))),0),((_tmpDF7->tl=Cyc_Toc_result_decls,_tmpDF7))))));}*_tmp942=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp942,f->name,f->sc != Cyc_Absyn_Extern);{struct Cyc_List_List*fields=0;int i=1;{
struct Cyc_List_List*_tmp94E=f->typs;for(0;_tmp94E != 0;(_tmp94E=_tmp94E->tl,i ++)){
struct Cyc_Int_pa_struct _tmpE01;void*_tmpE00[1];const char*_tmpDFF;struct
_dyneither_ptr*_tmpDFE;struct _dyneither_ptr*_tmp94F=(_tmpDFE=_cycalloc(sizeof(*
_tmpDFE)),((_tmpDFE[0]=(struct _dyneither_ptr)((_tmpE01.tag=1,((_tmpE01.f1=(
unsigned long)i,((_tmpE00[0]=& _tmpE01,Cyc_aprintf(((_tmpDFF="f%d",_tag_dyneither(
_tmpDFF,sizeof(char),4))),_tag_dyneither(_tmpE00,sizeof(void*),1)))))))),_tmpDFE)));
struct Cyc_Absyn_Aggrfield*_tmpE02;struct Cyc_Absyn_Aggrfield*_tmp950=(_tmpE02=
_cycalloc(sizeof(*_tmpE02)),((_tmpE02->name=_tmp94F,((_tmpE02->tq=(*((struct
_tuple9*)_tmp94E->hd)).f1,((_tmpE02->type=Cyc_Toc_typ_to_c_array((*((struct
_tuple9*)_tmp94E->hd)).f2),((_tmpE02->width=0,((_tmpE02->attributes=0,_tmpE02)))))))))));
struct Cyc_List_List*_tmpE03;fields=((_tmpE03=_cycalloc(sizeof(*_tmpE03)),((
_tmpE03->hd=_tmp950,((_tmpE03->tl=fields,_tmpE03))))));}}{struct Cyc_Absyn_Aggrfield*
_tmpE06;struct Cyc_List_List*_tmpE05;fields=((_tmpE05=_cycalloc(sizeof(*_tmpE05)),((
_tmpE05->hd=((_tmpE06=_cycalloc(sizeof(*_tmpE06)),((_tmpE06->name=Cyc_Toc_tag_sp,((
_tmpE06->tq=Cyc_Toc_mt_tq,((_tmpE06->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq),((_tmpE06->width=0,((_tmpE06->attributes=0,_tmpE06)))))))))))),((
_tmpE05->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmpE05))))));}{struct Cyc_Absyn_AggrdeclImpl*_tmpE0B;const char*_tmpE0A;
struct Cyc_Absyn_Aggrdecl*_tmpE09;struct Cyc_Absyn_Aggrdecl*_tmp959=(_tmpE09=
_cycalloc(sizeof(*_tmpE09)),((_tmpE09->kind=Cyc_Absyn_StructA,((_tmpE09->sc=Cyc_Absyn_Public,((
_tmpE09->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpE0A="_struct",
_tag_dyneither(_tmpE0A,sizeof(char),8)))),((_tmpE09->tvs=0,((_tmpE09->impl=((
_tmpE0B=_cycalloc(sizeof(*_tmpE0B)),((_tmpE0B->exist_vars=0,((_tmpE0B->rgn_po=0,((
_tmpE0B->fields=fields,((_tmpE0B->tagged=0,_tmpE0B)))))))))),((_tmpE09->attributes=
0,_tmpE09)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpE11;struct Cyc_Absyn_Aggr_d_struct
_tmpE10;struct Cyc_List_List*_tmpE0F;Cyc_Toc_result_decls=((_tmpE0F=_cycalloc(
sizeof(*_tmpE0F)),((_tmpE0F->hd=Cyc_Absyn_new_decl((void*)((_tmpE11=_cycalloc(
sizeof(*_tmpE11)),((_tmpE11[0]=((_tmpE10.tag=6,((_tmpE10.f1=_tmp959,_tmpE10)))),
_tmpE11)))),0),((_tmpE0F->tl=Cyc_Toc_result_decls,_tmpE0F))))));}goto _LL3A7;};};};}
_LL3AA: if(_tmp948 == 0)goto _LL3AC;_tmp949=*_tmp948;if(_tmp949 != 0)goto _LL3AC;
_LL3AB: if(f->sc != Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*_tmp960=Cyc_Absyn_string_exp(*
fn,0);struct Cyc_Absyn_Vardecl*_tmp961=Cyc_Absyn_new_vardecl(f->name,_tmp947,(
struct Cyc_Absyn_Exp*)_tmp960);_tmp961->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*
_tmpE17;struct Cyc_Absyn_Var_d_struct _tmpE16;struct Cyc_List_List*_tmpE15;Cyc_Toc_result_decls=((
_tmpE15=_cycalloc(sizeof(*_tmpE15)),((_tmpE15->hd=Cyc_Absyn_new_decl((void*)((
_tmpE17=_cycalloc(sizeof(*_tmpE17)),((_tmpE17[0]=((_tmpE16.tag=0,((_tmpE16.f1=
_tmp961,_tmpE16)))),_tmpE17)))),0),((_tmpE15->tl=Cyc_Toc_result_decls,_tmpE15))))));}*
_tmp942=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp942,f->name,1);}goto _LL3A7;_LL3AC:;_LL3AD: goto _LL3A7;_LL3A7:;}};
_pop_dynregion(d);};};}static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*
ed){ed->sc=Cyc_Absyn_Public;if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(
struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,
struct Cyc_Absyn_Stmt*s){void*old_typ=vd->type;vd->type=Cyc_Toc_typ_to_c_array(
old_typ);if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ))vd->sc=Cyc_Absyn_Public;Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp965=init->r;struct Cyc_Absyn_Vardecl*_tmp967;struct Cyc_Absyn_Exp*_tmp968;
struct Cyc_Absyn_Exp*_tmp969;int _tmp96A;_LL3AF: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp966=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp965;if(_tmp966->tag != 28)
goto _LL3B1;else{_tmp967=_tmp966->f1;_tmp968=_tmp966->f2;_tmp969=_tmp966->f3;
_tmp96A=_tmp966->f4;}}_LL3B0: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp967,_tmp968,_tmp969,_tmp96A,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL3AE;_LL3B1:;_LL3B2: if(vd->sc == Cyc_Absyn_Static){struct Cyc_Toc_Env
_tmp96C;struct _RegionHandle*_tmp96D;struct Cyc_Toc_Env*_tmp96B=init_nv;_tmp96C=*
_tmp96B;_tmp96D=_tmp96C.rgn;{struct Cyc_Toc_Env*_tmp96E=Cyc_Toc_set_toplevel(
_tmp96D,init_nv);Cyc_Toc_exp_to_c(_tmp96E,init);};}else{Cyc_Toc_exp_to_c(init_nv,
init);}goto _LL3AE;_LL3AE:;}else{void*_tmp96F=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_ArrayInfo _tmp971;void*_tmp972;struct Cyc_Absyn_Exp*_tmp973;union Cyc_Absyn_Constraint*
_tmp974;_LL3B4:{struct Cyc_Absyn_ArrayType_struct*_tmp970=(struct Cyc_Absyn_ArrayType_struct*)
_tmp96F;if(_tmp970->tag != 9)goto _LL3B6;else{_tmp971=_tmp970->f1;_tmp972=_tmp971.elt_type;
_tmp973=_tmp971.num_elts;_tmp974=_tmp971.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp974))goto _LL3B6;_LL3B5: if(_tmp973 == 0){const char*
_tmpE1A;void*_tmpE19;(_tmpE19=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE1A="can't initialize zero-terminated array -- size unknown",
_tag_dyneither(_tmpE1A,sizeof(char),55))),_tag_dyneither(_tmpE19,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp973;struct Cyc_Absyn_Exp*
_tmp977=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp978=Cyc_Absyn_signed_int_exp(
0,0);s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp977,
_tmp978,0),0),Cyc_Absyn_new_stmt(s->r,0));goto _LL3B3;};_LL3B6:;_LL3B7: goto _LL3B3;
_LL3B3:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple0*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp97A;struct
_RegionHandle*_tmp97B;struct Cyc_Toc_Env*_tmp979=nv;_tmp97A=*_tmp979;_tmp97B=
_tmp97A.rgn;{struct Cyc_Absyn_Stmt*_tmp97C=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp97D=Cyc_Toc_share_env(_tmp97B,nv);struct Cyc_Toc_Env*_tmp97F;struct Cyc_List_List*
_tmp980;struct Cyc_Absyn_Stmt*_tmp981;struct _tuple21 _tmp97E=Cyc_Toc_xlate_pat(
_tmp97D,_tmp97B,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp97F=_tmp97E.f1;_tmp980=_tmp97E.f2;_tmp981=_tmp97E.f3;Cyc_Toc_stmt_to_c(
_tmp97F,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp981,s,0),0);for(0;_tmp980 != 0;_tmp980=_tmp980->tl){
struct _tuple22 _tmp983;struct _tuple0*_tmp984;void*_tmp985;struct _tuple22*_tmp982=(
struct _tuple22*)_tmp980->hd;_tmp983=*_tmp982;_tmp984=_tmp983.f1;_tmp985=_tmp983.f2;
s=Cyc_Absyn_declare_stmt(_tmp984,_tmp985,0,s,0);}}return s;};}static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e){void*_tmp986=e->r;struct Cyc_Absyn_Exp*_tmp988;struct Cyc_Absyn_Exp*
_tmp98A;struct Cyc_Absyn_Exp*_tmp98C;struct Cyc_Absyn_Exp*_tmp98E;struct Cyc_Absyn_Exp*
_tmp990;struct Cyc_Absyn_Exp*_tmp992;struct Cyc_Absyn_Exp*_tmp994;struct Cyc_Absyn_Exp*
_tmp996;struct Cyc_List_List*_tmp998;struct Cyc_Absyn_Exp*_tmp99A;struct Cyc_Absyn_Exp*
_tmp99B;struct Cyc_Absyn_Exp*_tmp99D;struct Cyc_Absyn_Exp*_tmp99E;struct Cyc_Absyn_Exp*
_tmp9A0;struct Cyc_Absyn_Exp*_tmp9A1;struct Cyc_Absyn_Exp*_tmp9A3;struct Cyc_Absyn_Exp*
_tmp9A4;struct Cyc_Absyn_Exp*_tmp9A6;struct Cyc_Absyn_Exp*_tmp9A7;struct Cyc_Absyn_Exp*
_tmp9A9;struct Cyc_Absyn_Exp*_tmp9AA;struct Cyc_Absyn_Exp*_tmp9AC;struct Cyc_Absyn_Exp*
_tmp9AD;struct Cyc_Absyn_Exp*_tmp9AE;struct Cyc_Absyn_Exp*_tmp9B0;struct Cyc_List_List*
_tmp9B1;struct Cyc_Absyn_Exp*_tmp9B3;struct Cyc_List_List*_tmp9B4;void*_tmp9B6;
void**_tmp9B7;struct Cyc_Absyn_Exp*_tmp9B8;struct _tuple7*_tmp9BA;struct _tuple7
_tmp9BB;void*_tmp9BC;void**_tmp9BD;struct Cyc_List_List*_tmp9BE;struct Cyc_List_List*
_tmp9C0;struct Cyc_List_List*_tmp9C2;void*_tmp9C4;void**_tmp9C5;void*_tmp9C7;void**
_tmp9C8;struct Cyc_Absyn_Stmt*_tmp9CA;struct Cyc_Absyn_MallocInfo _tmp9CC;struct Cyc_Absyn_MallocInfo*
_tmp9CD;_LL3B9: {struct Cyc_Absyn_Deref_e_struct*_tmp987=(struct Cyc_Absyn_Deref_e_struct*)
_tmp986;if(_tmp987->tag != 21)goto _LL3BB;else{_tmp988=_tmp987->f1;}}_LL3BA:
_tmp98A=_tmp988;goto _LL3BC;_LL3BB: {struct Cyc_Absyn_AggrMember_e_struct*_tmp989=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp986;if(_tmp989->tag != 22)goto _LL3BD;
else{_tmp98A=_tmp989->f1;}}_LL3BC: _tmp98C=_tmp98A;goto _LL3BE;_LL3BD: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp98B=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp986;if(_tmp98B->tag != 23)goto
_LL3BF;else{_tmp98C=_tmp98B->f1;}}_LL3BE: _tmp98E=_tmp98C;goto _LL3C0;_LL3BF: {
struct Cyc_Absyn_Address_e_struct*_tmp98D=(struct Cyc_Absyn_Address_e_struct*)
_tmp986;if(_tmp98D->tag != 16)goto _LL3C1;else{_tmp98E=_tmp98D->f1;}}_LL3C0:
_tmp990=_tmp98E;goto _LL3C2;_LL3C1: {struct Cyc_Absyn_Throw_e_struct*_tmp98F=(
struct Cyc_Absyn_Throw_e_struct*)_tmp986;if(_tmp98F->tag != 12)goto _LL3C3;else{
_tmp990=_tmp98F->f1;}}_LL3C2: _tmp992=_tmp990;goto _LL3C4;_LL3C3: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp991=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp986;if(_tmp991->tag != 13)
goto _LL3C5;else{_tmp992=_tmp991->f1;}}_LL3C4: _tmp994=_tmp992;goto _LL3C6;_LL3C5: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp993=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp986;if(_tmp993->tag != 19)goto _LL3C7;else{_tmp994=_tmp993->f1;}}_LL3C6:
_tmp996=_tmp994;goto _LL3C8;_LL3C7: {struct Cyc_Absyn_Increment_e_struct*_tmp995=(
struct Cyc_Absyn_Increment_e_struct*)_tmp986;if(_tmp995->tag != 5)goto _LL3C9;else{
_tmp996=_tmp995->f1;}}_LL3C8: Cyc_Toc_exptypes_to_c(_tmp996);goto _LL3B8;_LL3C9: {
struct Cyc_Absyn_Primop_e_struct*_tmp997=(struct Cyc_Absyn_Primop_e_struct*)
_tmp986;if(_tmp997->tag != 3)goto _LL3CB;else{_tmp998=_tmp997->f2;}}_LL3CA:((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,
_tmp998);goto _LL3B8;_LL3CB: {struct Cyc_Absyn_And_e_struct*_tmp999=(struct Cyc_Absyn_And_e_struct*)
_tmp986;if(_tmp999->tag != 7)goto _LL3CD;else{_tmp99A=_tmp999->f1;_tmp99B=_tmp999->f2;}}
_LL3CC: _tmp99D=_tmp99A;_tmp99E=_tmp99B;goto _LL3CE;_LL3CD: {struct Cyc_Absyn_Or_e_struct*
_tmp99C=(struct Cyc_Absyn_Or_e_struct*)_tmp986;if(_tmp99C->tag != 8)goto _LL3CF;
else{_tmp99D=_tmp99C->f1;_tmp99E=_tmp99C->f2;}}_LL3CE: _tmp9A0=_tmp99D;_tmp9A1=
_tmp99E;goto _LL3D0;_LL3CF: {struct Cyc_Absyn_SeqExp_e_struct*_tmp99F=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp986;if(_tmp99F->tag != 9)goto _LL3D1;else{_tmp9A0=_tmp99F->f1;_tmp9A1=_tmp99F->f2;}}
_LL3D0: _tmp9A3=_tmp9A0;_tmp9A4=_tmp9A1;goto _LL3D2;_LL3D1: {struct Cyc_Absyn_Subscript_e_struct*
_tmp9A2=(struct Cyc_Absyn_Subscript_e_struct*)_tmp986;if(_tmp9A2->tag != 24)goto
_LL3D3;else{_tmp9A3=_tmp9A2->f1;_tmp9A4=_tmp9A2->f2;}}_LL3D2: _tmp9A6=_tmp9A3;
_tmp9A7=_tmp9A4;goto _LL3D4;_LL3D3: {struct Cyc_Absyn_Swap_e_struct*_tmp9A5=(
struct Cyc_Absyn_Swap_e_struct*)_tmp986;if(_tmp9A5->tag != 35)goto _LL3D5;else{
_tmp9A6=_tmp9A5->f1;_tmp9A7=_tmp9A5->f2;}}_LL3D4: _tmp9A9=_tmp9A6;_tmp9AA=_tmp9A7;
goto _LL3D6;_LL3D5: {struct Cyc_Absyn_AssignOp_e_struct*_tmp9A8=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp986;if(_tmp9A8->tag != 4)goto _LL3D7;else{_tmp9A9=_tmp9A8->f1;_tmp9AA=_tmp9A8->f3;}}
_LL3D6: Cyc_Toc_exptypes_to_c(_tmp9A9);Cyc_Toc_exptypes_to_c(_tmp9AA);goto _LL3B8;
_LL3D7: {struct Cyc_Absyn_Conditional_e_struct*_tmp9AB=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp986;if(_tmp9AB->tag != 6)goto _LL3D9;else{_tmp9AC=_tmp9AB->f1;_tmp9AD=_tmp9AB->f2;
_tmp9AE=_tmp9AB->f3;}}_LL3D8: Cyc_Toc_exptypes_to_c(_tmp9AC);Cyc_Toc_exptypes_to_c(
_tmp9AD);Cyc_Toc_exptypes_to_c(_tmp9AE);goto _LL3B8;_LL3D9: {struct Cyc_Absyn_FnCall_e_struct*
_tmp9AF=(struct Cyc_Absyn_FnCall_e_struct*)_tmp986;if(_tmp9AF->tag != 11)goto
_LL3DB;else{_tmp9B0=_tmp9AF->f1;_tmp9B1=_tmp9AF->f2;}}_LL3DA: _tmp9B3=_tmp9B0;
_tmp9B4=_tmp9B1;goto _LL3DC;_LL3DB: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp9B2=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp986;if(_tmp9B2->tag != 10)goto _LL3DD;
else{_tmp9B3=_tmp9B2->f1;_tmp9B4=_tmp9B2->f2;}}_LL3DC: Cyc_Toc_exptypes_to_c(
_tmp9B3);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp9B4);goto _LL3B8;_LL3DD: {struct Cyc_Absyn_Cast_e_struct*
_tmp9B5=(struct Cyc_Absyn_Cast_e_struct*)_tmp986;if(_tmp9B5->tag != 15)goto _LL3DF;
else{_tmp9B6=(void**)& _tmp9B5->f1;_tmp9B7=(void**)((void**)& _tmp9B5->f1);_tmp9B8=
_tmp9B5->f2;}}_LL3DE:*_tmp9B7=Cyc_Toc_typ_to_c(*_tmp9B7);Cyc_Toc_exptypes_to_c(
_tmp9B8);goto _LL3B8;_LL3DF: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp9B9=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp986;if(_tmp9B9->tag != 26)goto _LL3E1;
else{_tmp9BA=_tmp9B9->f1;_tmp9BB=*_tmp9BA;_tmp9BC=_tmp9BB.f3;_tmp9BD=(void**)&(*
_tmp9B9->f1).f3;_tmp9BE=_tmp9B9->f2;}}_LL3E0:*_tmp9BD=Cyc_Toc_typ_to_c(*_tmp9BD);
_tmp9C0=_tmp9BE;goto _LL3E2;_LL3E1: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp9BF=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp986;if(_tmp9BF->tag != 36)
goto _LL3E3;else{_tmp9C0=_tmp9BF->f2;}}_LL3E2: _tmp9C2=_tmp9C0;goto _LL3E4;_LL3E3: {
struct Cyc_Absyn_Array_e_struct*_tmp9C1=(struct Cyc_Absyn_Array_e_struct*)_tmp986;
if(_tmp9C1->tag != 27)goto _LL3E5;else{_tmp9C2=_tmp9C1->f1;}}_LL3E4: for(0;_tmp9C2
!= 0;_tmp9C2=_tmp9C2->tl){struct Cyc_Absyn_Exp*_tmp9DE;struct _tuple15 _tmp9DD=*((
struct _tuple15*)_tmp9C2->hd);_tmp9DE=_tmp9DD.f2;Cyc_Toc_exptypes_to_c(_tmp9DE);}
goto _LL3B8;_LL3E5: {struct Cyc_Absyn_Offsetof_e_struct*_tmp9C3=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp986;if(_tmp9C3->tag != 20)goto _LL3E7;else{_tmp9C4=(void**)& _tmp9C3->f1;
_tmp9C5=(void**)((void**)& _tmp9C3->f1);}}_LL3E6: _tmp9C8=_tmp9C5;goto _LL3E8;
_LL3E7: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp9C6=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp986;if(_tmp9C6->tag != 18)goto _LL3E9;else{_tmp9C7=(void**)& _tmp9C6->f1;
_tmp9C8=(void**)((void**)& _tmp9C6->f1);}}_LL3E8:*_tmp9C8=Cyc_Toc_typ_to_c(*
_tmp9C8);goto _LL3B8;_LL3E9: {struct Cyc_Absyn_StmtExp_e_struct*_tmp9C9=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp986;if(_tmp9C9->tag != 37)goto _LL3EB;else{_tmp9CA=_tmp9C9->f1;}}_LL3EA: Cyc_Toc_stmttypes_to_c(
_tmp9CA);goto _LL3B8;_LL3EB: {struct Cyc_Absyn_Malloc_e_struct*_tmp9CB=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp986;if(_tmp9CB->tag != 34)goto _LL3ED;else{_tmp9CC=_tmp9CB->f1;_tmp9CD=(struct
Cyc_Absyn_MallocInfo*)& _tmp9CB->f1;}}_LL3EC: if(_tmp9CD->elt_type != 0){void**
_tmpE1B;_tmp9CD->elt_type=((_tmpE1B=_cycalloc(sizeof(*_tmpE1B)),((_tmpE1B[0]=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp9CD->elt_type))),_tmpE1B))));}Cyc_Toc_exptypes_to_c(
_tmp9CD->num_elts);goto _LL3B8;_LL3ED: {struct Cyc_Absyn_Const_e_struct*_tmp9CE=(
struct Cyc_Absyn_Const_e_struct*)_tmp986;if(_tmp9CE->tag != 0)goto _LL3EF;}_LL3EE:
goto _LL3F0;_LL3EF: {struct Cyc_Absyn_Var_e_struct*_tmp9CF=(struct Cyc_Absyn_Var_e_struct*)
_tmp986;if(_tmp9CF->tag != 1)goto _LL3F1;}_LL3F0: goto _LL3F2;_LL3F1: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp9D0=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp986;if(_tmp9D0->tag != 2)goto
_LL3F3;}_LL3F2: goto _LL3F4;_LL3F3: {struct Cyc_Absyn_Enum_e_struct*_tmp9D1=(struct
Cyc_Absyn_Enum_e_struct*)_tmp986;if(_tmp9D1->tag != 32)goto _LL3F5;}_LL3F4: goto
_LL3F6;_LL3F5: {struct Cyc_Absyn_Asm_e_struct*_tmp9D2=(struct Cyc_Absyn_Asm_e_struct*)
_tmp986;if(_tmp9D2->tag != 40)goto _LL3F7;}_LL3F6: goto _LL3F8;_LL3F7: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp9D3=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp986;if(_tmp9D3->tag != 33)goto
_LL3F9;}_LL3F8: goto _LL3B8;_LL3F9: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp9D4=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp986;if(_tmp9D4->tag != 30)goto _LL3FB;}
_LL3FA: goto _LL3FC;_LL3FB: {struct Cyc_Absyn_Datatype_e_struct*_tmp9D5=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp986;if(_tmp9D5->tag != 31)goto _LL3FD;}_LL3FC: goto _LL3FE;_LL3FD: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp9D6=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp986;if(_tmp9D6->tag != 29)goto
_LL3FF;}_LL3FE: goto _LL400;_LL3FF: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp9D7=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp986;if(_tmp9D7->tag != 28)
goto _LL401;}_LL400: goto _LL402;_LL401: {struct Cyc_Absyn_Tuple_e_struct*_tmp9D8=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp986;if(_tmp9D8->tag != 25)goto _LL403;}_LL402:
goto _LL404;_LL403: {struct Cyc_Absyn_Instantiate_e_struct*_tmp9D9=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp986;if(_tmp9D9->tag != 14)goto _LL405;}_LL404: goto _LL406;_LL405: {struct Cyc_Absyn_New_e_struct*
_tmp9DA=(struct Cyc_Absyn_New_e_struct*)_tmp986;if(_tmp9DA->tag != 17)goto _LL407;}
_LL406: goto _LL408;_LL407: {struct Cyc_Absyn_Valueof_e_struct*_tmp9DB=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp986;if(_tmp9DB->tag != 39)goto _LL409;}_LL408: goto _LL40A;_LL409: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp9DC=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp986;if(_tmp9DC->tag != 38)goto
_LL3B8;}_LL40A:{const char*_tmpE1E;void*_tmpE1D;(_tmpE1D=0,Cyc_Tcutil_terr(e->loc,((
_tmpE1E="Cyclone expression within C code",_tag_dyneither(_tmpE1E,sizeof(char),
33))),_tag_dyneither(_tmpE1D,sizeof(void*),0)));}goto _LL3B8;_LL3B8:;}static void
Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){void*_tmp9E2=d->r;struct Cyc_Absyn_Vardecl*
_tmp9E4;struct Cyc_Absyn_Fndecl*_tmp9E6;struct Cyc_Absyn_Aggrdecl*_tmp9E8;struct
Cyc_Absyn_Enumdecl*_tmp9EA;struct Cyc_Absyn_Typedefdecl*_tmp9EC;_LL40C: {struct
Cyc_Absyn_Var_d_struct*_tmp9E3=(struct Cyc_Absyn_Var_d_struct*)_tmp9E2;if(_tmp9E3->tag
!= 0)goto _LL40E;else{_tmp9E4=_tmp9E3->f1;}}_LL40D: _tmp9E4->type=Cyc_Toc_typ_to_c(
_tmp9E4->type);if(_tmp9E4->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp9E4->initializer));goto _LL40B;_LL40E: {struct Cyc_Absyn_Fn_d_struct*
_tmp9E5=(struct Cyc_Absyn_Fn_d_struct*)_tmp9E2;if(_tmp9E5->tag != 1)goto _LL410;
else{_tmp9E6=_tmp9E5->f1;}}_LL40F: _tmp9E6->ret_type=Cyc_Toc_typ_to_c(_tmp9E6->ret_type);{
struct Cyc_List_List*_tmp9F8=_tmp9E6->args;for(0;_tmp9F8 != 0;_tmp9F8=_tmp9F8->tl){(*((
struct _tuple25*)_tmp9F8->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple25*)_tmp9F8->hd)).f3);}}
goto _LL40B;_LL410: {struct Cyc_Absyn_Aggr_d_struct*_tmp9E7=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp9E2;if(_tmp9E7->tag != 6)goto _LL412;else{_tmp9E8=_tmp9E7->f1;}}_LL411: Cyc_Toc_aggrdecl_to_c(
_tmp9E8,1);goto _LL40B;_LL412: {struct Cyc_Absyn_Enum_d_struct*_tmp9E9=(struct Cyc_Absyn_Enum_d_struct*)
_tmp9E2;if(_tmp9E9->tag != 8)goto _LL414;else{_tmp9EA=_tmp9E9->f1;}}_LL413: if(
_tmp9EA->fields != 0){struct Cyc_List_List*_tmp9F9=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp9EA->fields))->v;for(0;_tmp9F9 != 0;_tmp9F9=_tmp9F9->tl){
struct Cyc_Absyn_Enumfield*_tmp9FA=(struct Cyc_Absyn_Enumfield*)_tmp9F9->hd;if(
_tmp9FA->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9FA->tag));}}
goto _LL40B;_LL414: {struct Cyc_Absyn_Typedef_d_struct*_tmp9EB=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp9E2;if(_tmp9EB->tag != 9)goto _LL416;else{_tmp9EC=_tmp9EB->f1;}}_LL415:{struct
Cyc_Core_Opt*_tmpE1F;_tmp9EC->defn=((_tmpE1F=_cycalloc(sizeof(*_tmpE1F)),((
_tmpE1F->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp9EC->defn))->v),_tmpE1F))));}goto _LL40B;_LL416: {struct Cyc_Absyn_Let_d_struct*
_tmp9ED=(struct Cyc_Absyn_Let_d_struct*)_tmp9E2;if(_tmp9ED->tag != 2)goto _LL418;}
_LL417: goto _LL419;_LL418: {struct Cyc_Absyn_Letv_d_struct*_tmp9EE=(struct Cyc_Absyn_Letv_d_struct*)
_tmp9E2;if(_tmp9EE->tag != 3)goto _LL41A;}_LL419: goto _LL41B;_LL41A: {struct Cyc_Absyn_Datatype_d_struct*
_tmp9EF=(struct Cyc_Absyn_Datatype_d_struct*)_tmp9E2;if(_tmp9EF->tag != 7)goto
_LL41C;}_LL41B: goto _LL41D;_LL41C: {struct Cyc_Absyn_Namespace_d_struct*_tmp9F0=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp9E2;if(_tmp9F0->tag != 10)goto _LL41E;}
_LL41D: goto _LL41F;_LL41E: {struct Cyc_Absyn_Using_d_struct*_tmp9F1=(struct Cyc_Absyn_Using_d_struct*)
_tmp9E2;if(_tmp9F1->tag != 11)goto _LL420;}_LL41F: goto _LL421;_LL420: {struct Cyc_Absyn_ExternC_d_struct*
_tmp9F2=(struct Cyc_Absyn_ExternC_d_struct*)_tmp9E2;if(_tmp9F2->tag != 12)goto
_LL422;}_LL421: goto _LL423;_LL422: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp9F3=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9E2;if(_tmp9F3->tag != 13)
goto _LL424;}_LL423: goto _LL425;_LL424: {struct Cyc_Absyn_Region_d_struct*_tmp9F4=(
struct Cyc_Absyn_Region_d_struct*)_tmp9E2;if(_tmp9F4->tag != 4)goto _LL426;}_LL425:
goto _LL427;_LL426: {struct Cyc_Absyn_Alias_d_struct*_tmp9F5=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9E2;if(_tmp9F5->tag != 5)goto _LL428;}_LL427:{const char*_tmpE22;void*_tmpE21;(
_tmpE21=0,Cyc_Tcutil_terr(d->loc,((_tmpE22="Cyclone declaration within C code",
_tag_dyneither(_tmpE22,sizeof(char),34))),_tag_dyneither(_tmpE21,sizeof(void*),0)));}
goto _LL40B;_LL428: {struct Cyc_Absyn_Porton_d_struct*_tmp9F6=(struct Cyc_Absyn_Porton_d_struct*)
_tmp9E2;if(_tmp9F6->tag != 14)goto _LL42A;}_LL429: goto _LL42B;_LL42A: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9F7=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9E2;if(_tmp9F7->tag != 15)goto
_LL40B;}_LL42B: goto _LL40B;_LL40B:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s){void*_tmp9FE=s->r;struct Cyc_Absyn_Exp*_tmpA00;struct Cyc_Absyn_Stmt*_tmpA02;
struct Cyc_Absyn_Stmt*_tmpA03;struct Cyc_Absyn_Exp*_tmpA05;struct Cyc_Absyn_Exp*
_tmpA07;struct Cyc_Absyn_Stmt*_tmpA08;struct Cyc_Absyn_Stmt*_tmpA09;struct _tuple8
_tmpA0B;struct Cyc_Absyn_Exp*_tmpA0C;struct Cyc_Absyn_Stmt*_tmpA0D;struct Cyc_Absyn_Exp*
_tmpA0F;struct _tuple8 _tmpA10;struct Cyc_Absyn_Exp*_tmpA11;struct _tuple8 _tmpA12;
struct Cyc_Absyn_Exp*_tmpA13;struct Cyc_Absyn_Stmt*_tmpA14;struct Cyc_Absyn_Exp*
_tmpA16;struct Cyc_List_List*_tmpA17;struct Cyc_Absyn_Decl*_tmpA19;struct Cyc_Absyn_Stmt*
_tmpA1A;struct Cyc_Absyn_Stmt*_tmpA1C;struct _tuple8 _tmpA1D;struct Cyc_Absyn_Exp*
_tmpA1E;_LL42D: {struct Cyc_Absyn_Exp_s_struct*_tmp9FF=(struct Cyc_Absyn_Exp_s_struct*)
_tmp9FE;if(_tmp9FF->tag != 1)goto _LL42F;else{_tmpA00=_tmp9FF->f1;}}_LL42E: Cyc_Toc_exptypes_to_c(
_tmpA00);goto _LL42C;_LL42F: {struct Cyc_Absyn_Seq_s_struct*_tmpA01=(struct Cyc_Absyn_Seq_s_struct*)
_tmp9FE;if(_tmpA01->tag != 2)goto _LL431;else{_tmpA02=_tmpA01->f1;_tmpA03=_tmpA01->f2;}}
_LL430: Cyc_Toc_stmttypes_to_c(_tmpA02);Cyc_Toc_stmttypes_to_c(_tmpA03);goto
_LL42C;_LL431: {struct Cyc_Absyn_Return_s_struct*_tmpA04=(struct Cyc_Absyn_Return_s_struct*)
_tmp9FE;if(_tmpA04->tag != 3)goto _LL433;else{_tmpA05=_tmpA04->f1;}}_LL432: if(
_tmpA05 != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmpA05);goto _LL42C;
_LL433: {struct Cyc_Absyn_IfThenElse_s_struct*_tmpA06=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp9FE;if(_tmpA06->tag != 4)goto _LL435;else{_tmpA07=_tmpA06->f1;_tmpA08=_tmpA06->f2;
_tmpA09=_tmpA06->f3;}}_LL434: Cyc_Toc_exptypes_to_c(_tmpA07);Cyc_Toc_stmttypes_to_c(
_tmpA08);Cyc_Toc_stmttypes_to_c(_tmpA09);goto _LL42C;_LL435: {struct Cyc_Absyn_While_s_struct*
_tmpA0A=(struct Cyc_Absyn_While_s_struct*)_tmp9FE;if(_tmpA0A->tag != 5)goto _LL437;
else{_tmpA0B=_tmpA0A->f1;_tmpA0C=_tmpA0B.f1;_tmpA0D=_tmpA0A->f2;}}_LL436: Cyc_Toc_exptypes_to_c(
_tmpA0C);Cyc_Toc_stmttypes_to_c(_tmpA0D);goto _LL42C;_LL437: {struct Cyc_Absyn_For_s_struct*
_tmpA0E=(struct Cyc_Absyn_For_s_struct*)_tmp9FE;if(_tmpA0E->tag != 9)goto _LL439;
else{_tmpA0F=_tmpA0E->f1;_tmpA10=_tmpA0E->f2;_tmpA11=_tmpA10.f1;_tmpA12=_tmpA0E->f3;
_tmpA13=_tmpA12.f1;_tmpA14=_tmpA0E->f4;}}_LL438: Cyc_Toc_exptypes_to_c(_tmpA0F);
Cyc_Toc_exptypes_to_c(_tmpA11);Cyc_Toc_exptypes_to_c(_tmpA13);Cyc_Toc_stmttypes_to_c(
_tmpA14);goto _LL42C;_LL439: {struct Cyc_Absyn_Switch_s_struct*_tmpA15=(struct Cyc_Absyn_Switch_s_struct*)
_tmp9FE;if(_tmpA15->tag != 10)goto _LL43B;else{_tmpA16=_tmpA15->f1;_tmpA17=_tmpA15->f2;}}
_LL43A: Cyc_Toc_exptypes_to_c(_tmpA16);for(0;_tmpA17 != 0;_tmpA17=_tmpA17->tl){Cyc_Toc_stmttypes_to_c(((
struct Cyc_Absyn_Switch_clause*)_tmpA17->hd)->body);}goto _LL42C;_LL43B: {struct
Cyc_Absyn_Decl_s_struct*_tmpA18=(struct Cyc_Absyn_Decl_s_struct*)_tmp9FE;if(
_tmpA18->tag != 12)goto _LL43D;else{_tmpA19=_tmpA18->f1;_tmpA1A=_tmpA18->f2;}}
_LL43C: Cyc_Toc_decltypes_to_c(_tmpA19);Cyc_Toc_stmttypes_to_c(_tmpA1A);goto
_LL42C;_LL43D: {struct Cyc_Absyn_Do_s_struct*_tmpA1B=(struct Cyc_Absyn_Do_s_struct*)
_tmp9FE;if(_tmpA1B->tag != 14)goto _LL43F;else{_tmpA1C=_tmpA1B->f1;_tmpA1D=_tmpA1B->f2;
_tmpA1E=_tmpA1D.f1;}}_LL43E: Cyc_Toc_stmttypes_to_c(_tmpA1C);Cyc_Toc_exptypes_to_c(
_tmpA1E);goto _LL42C;_LL43F: {struct Cyc_Absyn_Skip_s_struct*_tmpA1F=(struct Cyc_Absyn_Skip_s_struct*)
_tmp9FE;if(_tmpA1F->tag != 0)goto _LL441;}_LL440: goto _LL442;_LL441: {struct Cyc_Absyn_Break_s_struct*
_tmpA20=(struct Cyc_Absyn_Break_s_struct*)_tmp9FE;if(_tmpA20->tag != 6)goto _LL443;}
_LL442: goto _LL444;_LL443: {struct Cyc_Absyn_Continue_s_struct*_tmpA21=(struct Cyc_Absyn_Continue_s_struct*)
_tmp9FE;if(_tmpA21->tag != 7)goto _LL445;}_LL444: goto _LL446;_LL445: {struct Cyc_Absyn_Goto_s_struct*
_tmpA22=(struct Cyc_Absyn_Goto_s_struct*)_tmp9FE;if(_tmpA22->tag != 8)goto _LL447;}
_LL446: goto _LL42C;_LL447: {struct Cyc_Absyn_Fallthru_s_struct*_tmpA23=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp9FE;if(_tmpA23->tag != 11)goto _LL449;}_LL448: goto _LL44A;_LL449: {struct Cyc_Absyn_Label_s_struct*
_tmpA24=(struct Cyc_Absyn_Label_s_struct*)_tmp9FE;if(_tmpA24->tag != 13)goto _LL44B;}
_LL44A: goto _LL44C;_LL44B: {struct Cyc_Absyn_TryCatch_s_struct*_tmpA25=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp9FE;if(_tmpA25->tag != 15)goto _LL44D;}_LL44C: goto _LL44E;_LL44D: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmpA26=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp9FE;if(_tmpA26->tag != 16)goto
_LL42C;}_LL44E:{const char*_tmpE25;void*_tmpE24;(_tmpE24=0,Cyc_Tcutil_terr(s->loc,((
_tmpE25="Cyclone statement in C code",_tag_dyneither(_tmpE25,sizeof(char),28))),
_tag_dyneither(_tmpE24,sizeof(void*),0)));}goto _LL42C;_LL42C:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude){for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpE28;void*_tmpE27;(_tmpE27=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE28="decls_to_c: not at toplevel!",
_tag_dyneither(_tmpE28,sizeof(char),29))),_tag_dyneither(_tmpE27,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmpA2B=d->r;struct Cyc_Absyn_Vardecl*
_tmpA2D;struct Cyc_Absyn_Fndecl*_tmpA2F;struct Cyc_Absyn_Aggrdecl*_tmpA35;struct
Cyc_Absyn_Datatypedecl*_tmpA37;struct Cyc_Absyn_Enumdecl*_tmpA39;struct Cyc_Absyn_Typedefdecl*
_tmpA3B;struct Cyc_List_List*_tmpA3F;struct Cyc_List_List*_tmpA41;struct Cyc_List_List*
_tmpA43;struct Cyc_List_List*_tmpA45;_LL450: {struct Cyc_Absyn_Var_d_struct*
_tmpA2C=(struct Cyc_Absyn_Var_d_struct*)_tmpA2B;if(_tmpA2C->tag != 0)goto _LL452;
else{_tmpA2D=_tmpA2C->f1;}}_LL451: {struct _tuple0*_tmpA46=_tmpA2D->name;if(
_tmpA2D->sc == Cyc_Absyn_ExternC){struct _tuple0*_tmpE29;_tmpA46=((_tmpE29=
_cycalloc(sizeof(*_tmpE29)),((_tmpE29->f1=Cyc_Absyn_Rel_n(0),((_tmpE29->f2=(*
_tmpA46).f2,_tmpE29))))));}if(_tmpA2D->initializer != 0){if(cinclude)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmpA2D->initializer));else{Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_check_null(_tmpA2D->initializer));}}{struct Cyc_Absyn_Global_b_struct
_tmpE2C;struct Cyc_Absyn_Global_b_struct*_tmpE2B;nv=Cyc_Toc_add_varmap(r,nv,
_tmpA2D->name,Cyc_Absyn_varb_exp(_tmpA46,(void*)((_tmpE2B=_cycalloc(sizeof(*
_tmpE2B)),((_tmpE2B[0]=((_tmpE2C.tag=1,((_tmpE2C.f1=_tmpA2D,_tmpE2C)))),_tmpE2B)))),
0));}_tmpA2D->name=_tmpA46;_tmpA2D->sc=Cyc_Toc_scope_to_c(_tmpA2D->sc);_tmpA2D->type=
Cyc_Toc_typ_to_c_array(_tmpA2D->type);{struct Cyc_List_List*_tmpE2D;Cyc_Toc_result_decls=((
_tmpE2D=_cycalloc(sizeof(*_tmpE2D)),((_tmpE2D->hd=d,((_tmpE2D->tl=Cyc_Toc_result_decls,
_tmpE2D))))));}goto _LL44F;}_LL452: {struct Cyc_Absyn_Fn_d_struct*_tmpA2E=(struct
Cyc_Absyn_Fn_d_struct*)_tmpA2B;if(_tmpA2E->tag != 1)goto _LL454;else{_tmpA2F=
_tmpA2E->f1;}}_LL453: {struct _tuple0*_tmpA4B=_tmpA2F->name;if(_tmpA2F->sc == Cyc_Absyn_ExternC){{
struct _tuple0*_tmpE2E;_tmpA4B=((_tmpE2E=_cycalloc(sizeof(*_tmpE2E)),((_tmpE2E->f1=
Cyc_Absyn_Rel_n(0),((_tmpE2E->f2=(*_tmpA4B).f2,_tmpE2E))))));}_tmpA2F->sc=Cyc_Absyn_Public;}
nv=Cyc_Toc_add_varmap(r,nv,_tmpA2F->name,Cyc_Absyn_var_exp(_tmpA4B,0));_tmpA2F->name=
_tmpA4B;Cyc_Toc_fndecl_to_c(nv,_tmpA2F,cinclude);{struct Cyc_List_List*_tmpE2F;
Cyc_Toc_result_decls=((_tmpE2F=_cycalloc(sizeof(*_tmpE2F)),((_tmpE2F->hd=d,((
_tmpE2F->tl=Cyc_Toc_result_decls,_tmpE2F))))));}goto _LL44F;}_LL454: {struct Cyc_Absyn_Let_d_struct*
_tmpA30=(struct Cyc_Absyn_Let_d_struct*)_tmpA2B;if(_tmpA30->tag != 2)goto _LL456;}
_LL455: goto _LL457;_LL456: {struct Cyc_Absyn_Letv_d_struct*_tmpA31=(struct Cyc_Absyn_Letv_d_struct*)
_tmpA2B;if(_tmpA31->tag != 3)goto _LL458;}_LL457: {const char*_tmpE32;void*_tmpE31;(
_tmpE31=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE32="letdecl at toplevel",_tag_dyneither(_tmpE32,sizeof(char),20))),
_tag_dyneither(_tmpE31,sizeof(void*),0)));}_LL458: {struct Cyc_Absyn_Region_d_struct*
_tmpA32=(struct Cyc_Absyn_Region_d_struct*)_tmpA2B;if(_tmpA32->tag != 4)goto _LL45A;}
_LL459: {const char*_tmpE35;void*_tmpE34;(_tmpE34=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE35="region decl at toplevel",
_tag_dyneither(_tmpE35,sizeof(char),24))),_tag_dyneither(_tmpE34,sizeof(void*),0)));}
_LL45A: {struct Cyc_Absyn_Alias_d_struct*_tmpA33=(struct Cyc_Absyn_Alias_d_struct*)
_tmpA2B;if(_tmpA33->tag != 5)goto _LL45C;}_LL45B: {const char*_tmpE38;void*_tmpE37;(
_tmpE37=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE38="alias decl at toplevel",_tag_dyneither(_tmpE38,sizeof(char),23))),
_tag_dyneither(_tmpE37,sizeof(void*),0)));}_LL45C: {struct Cyc_Absyn_Aggr_d_struct*
_tmpA34=(struct Cyc_Absyn_Aggr_d_struct*)_tmpA2B;if(_tmpA34->tag != 6)goto _LL45E;
else{_tmpA35=_tmpA34->f1;}}_LL45D: Cyc_Toc_aggrdecl_to_c(_tmpA35,1);goto _LL44F;
_LL45E: {struct Cyc_Absyn_Datatype_d_struct*_tmpA36=(struct Cyc_Absyn_Datatype_d_struct*)
_tmpA2B;if(_tmpA36->tag != 7)goto _LL460;else{_tmpA37=_tmpA36->f1;}}_LL45F: if(
_tmpA37->is_extensible)Cyc_Toc_xdatatypedecl_to_c(_tmpA37);else{Cyc_Toc_datatypedecl_to_c(
_tmpA37);}goto _LL44F;_LL460: {struct Cyc_Absyn_Enum_d_struct*_tmpA38=(struct Cyc_Absyn_Enum_d_struct*)
_tmpA2B;if(_tmpA38->tag != 8)goto _LL462;else{_tmpA39=_tmpA38->f1;}}_LL461: Cyc_Toc_enumdecl_to_c(
_tmpA39);{struct Cyc_List_List*_tmpE39;Cyc_Toc_result_decls=((_tmpE39=_cycalloc(
sizeof(*_tmpE39)),((_tmpE39->hd=d,((_tmpE39->tl=Cyc_Toc_result_decls,_tmpE39))))));}
goto _LL44F;_LL462: {struct Cyc_Absyn_Typedef_d_struct*_tmpA3A=(struct Cyc_Absyn_Typedef_d_struct*)
_tmpA2B;if(_tmpA3A->tag != 9)goto _LL464;else{_tmpA3B=_tmpA3A->f1;}}_LL463: _tmpA3B->name=
_tmpA3B->name;_tmpA3B->tvs=0;if(_tmpA3B->defn != 0){struct Cyc_Core_Opt*_tmpE3A;
_tmpA3B->defn=((_tmpE3A=_cycalloc(sizeof(*_tmpE3A)),((_tmpE3A->v=Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA3B->defn))->v),_tmpE3A))));}else{
switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmpA3B->kind))->v)->kind){
case Cyc_Absyn_BoxKind: _LL470:{struct Cyc_Core_Opt*_tmpE3B;_tmpA3B->defn=((_tmpE3B=
_cycalloc(sizeof(*_tmpE3B)),((_tmpE3B->v=Cyc_Absyn_void_star_typ(),_tmpE3B))));}
break;default: _LL471:{struct Cyc_Core_Opt*_tmpE3C;_tmpA3B->defn=((_tmpE3C=
_cycalloc(sizeof(*_tmpE3C)),((_tmpE3C->v=(void*)& Cyc_Absyn_VoidType_val,_tmpE3C))));}
break;}}{struct Cyc_List_List*_tmpE3D;Cyc_Toc_result_decls=((_tmpE3D=_cycalloc(
sizeof(*_tmpE3D)),((_tmpE3D->hd=d,((_tmpE3D->tl=Cyc_Toc_result_decls,_tmpE3D))))));}
goto _LL44F;_LL464: {struct Cyc_Absyn_Porton_d_struct*_tmpA3C=(struct Cyc_Absyn_Porton_d_struct*)
_tmpA2B;if(_tmpA3C->tag != 14)goto _LL466;}_LL465: goto _LL467;_LL466: {struct Cyc_Absyn_Portoff_d_struct*
_tmpA3D=(struct Cyc_Absyn_Portoff_d_struct*)_tmpA2B;if(_tmpA3D->tag != 15)goto
_LL468;}_LL467: goto _LL44F;_LL468: {struct Cyc_Absyn_Namespace_d_struct*_tmpA3E=(
struct Cyc_Absyn_Namespace_d_struct*)_tmpA2B;if(_tmpA3E->tag != 10)goto _LL46A;
else{_tmpA3F=_tmpA3E->f2;}}_LL469: _tmpA41=_tmpA3F;goto _LL46B;_LL46A: {struct Cyc_Absyn_Using_d_struct*
_tmpA40=(struct Cyc_Absyn_Using_d_struct*)_tmpA2B;if(_tmpA40->tag != 11)goto _LL46C;
else{_tmpA41=_tmpA40->f2;}}_LL46B: _tmpA43=_tmpA41;goto _LL46D;_LL46C: {struct Cyc_Absyn_ExternC_d_struct*
_tmpA42=(struct Cyc_Absyn_ExternC_d_struct*)_tmpA2B;if(_tmpA42->tag != 12)goto
_LL46E;else{_tmpA43=_tmpA42->f1;}}_LL46D: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA43,top,
cinclude);goto _LL44F;_LL46E: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA44=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmpA2B;if(_tmpA44->tag != 13)goto _LL44F;
else{_tmpA45=_tmpA44->f1;}}_LL46F: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA45,top,1);goto
_LL44F;_LL44F:;};}return nv;}static void Cyc_Toc_init(){struct _DynRegionHandle*
_tmpA5A;struct Cyc_Core_NewRegion _tmpA59=Cyc_Core__new_dynregion((const char*)"internal-error",
0);_tmpA5A=_tmpA59.dynregion;{struct _DynRegionFrame _tmpA5B;struct _RegionHandle*d=
_open_dynregion(& _tmpA5B,_tmpA5A);{struct Cyc_Dict_Dict*_tmpE48;struct Cyc_Dict_Dict*
_tmpE47;struct Cyc_Set_Set**_tmpE46;struct Cyc_Dict_Dict*_tmpE45;struct Cyc_List_List**
_tmpE44;struct Cyc_Toc_TocState*_tmpE43;Cyc_Toc_toc_state=((_tmpE43=_cycalloc(
sizeof(*_tmpE43)),((_tmpE43->dyn=(struct _DynRegionHandle*)_tmpA5A,((_tmpE43->tuple_types=(
struct Cyc_List_List**)((_tmpE44=_region_malloc(d,sizeof(*_tmpE44)),((_tmpE44[0]=
0,_tmpE44)))),((_tmpE43->aggrs_so_far=(struct Cyc_Dict_Dict*)((_tmpE45=
_region_malloc(d,sizeof(*_tmpE45)),((_tmpE45[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE45)))),((_tmpE43->datatypes_so_far=(struct Cyc_Set_Set**)((_tmpE46=
_region_malloc(d,sizeof(*_tmpE46)),((_tmpE46[0]=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE46)))),((_tmpE43->xdatatypes_so_far=(struct Cyc_Dict_Dict*)((_tmpE47=
_region_malloc(d,sizeof(*_tmpE47)),((_tmpE47[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE47)))),((_tmpE43->qvar_tags=(struct Cyc_Dict_Dict*)((_tmpE48=_region_malloc(
d,sizeof(*_tmpE48)),((_tmpE48[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,
int(*cmp)(struct _tuple13*,struct _tuple13*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),
_tmpE48)))),((_tmpE43->temp_labels=(struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(
struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpE43))))))))))))))));}Cyc_Toc_result_decls=
0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=
0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=0;{struct
_dyneither_ptr**_tmpE49;Cyc_Toc_globals=_tag_dyneither(((_tmpE49=_cycalloc(
sizeof(struct _dyneither_ptr*)* 41),((_tmpE49[0]=& Cyc_Toc__throw_str,((_tmpE49[1]=&
Cyc_Toc_setjmp_str,((_tmpE49[2]=& Cyc_Toc__push_handler_str,((_tmpE49[3]=& Cyc_Toc__pop_handler_str,((
_tmpE49[4]=& Cyc_Toc__exn_thrown_str,((_tmpE49[5]=& Cyc_Toc__npop_handler_str,((
_tmpE49[6]=& Cyc_Toc__check_null_str,((_tmpE49[7]=& Cyc_Toc__check_known_subscript_null_str,((
_tmpE49[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpE49[9]=& Cyc_Toc__check_dyneither_subscript_str,((
_tmpE49[10]=& Cyc_Toc__dyneither_ptr_str,((_tmpE49[11]=& Cyc_Toc__tag_dyneither_str,((
_tmpE49[12]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpE49[13]=& Cyc_Toc__untag_dyneither_ptr_str,((
_tmpE49[14]=& Cyc_Toc__get_dyneither_size_str,((_tmpE49[15]=& Cyc_Toc__get_zero_arr_size_str,((
_tmpE49[16]=& Cyc_Toc__dyneither_ptr_plus_str,((_tmpE49[17]=& Cyc_Toc__zero_arr_plus_str,((
_tmpE49[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpE49[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((
_tmpE49[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpE49[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((
_tmpE49[22]=& Cyc_Toc__cycalloc_str,((_tmpE49[23]=& Cyc_Toc__cyccalloc_str,((
_tmpE49[24]=& Cyc_Toc__cycalloc_atomic_str,((_tmpE49[25]=& Cyc_Toc__cyccalloc_atomic_str,((
_tmpE49[26]=& Cyc_Toc__region_malloc_str,((_tmpE49[27]=& Cyc_Toc__region_calloc_str,((
_tmpE49[28]=& Cyc_Toc__check_times_str,((_tmpE49[29]=& Cyc_Toc__new_region_str,((
_tmpE49[30]=& Cyc_Toc__push_region_str,((_tmpE49[31]=& Cyc_Toc__pop_region_str,((
_tmpE49[32]=& Cyc_Toc__open_dynregion_str,((_tmpE49[33]=& Cyc_Toc__push_dynregion_str,((
_tmpE49[34]=& Cyc_Toc__pop_dynregion_str,((_tmpE49[35]=& Cyc_Toc__reset_region_str,((
_tmpE49[36]=& Cyc_Toc__throw_arraybounds_str,((_tmpE49[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,((
_tmpE49[38]=& Cyc_Toc__throw_match_str,((_tmpE49[39]=& Cyc_Toc__swap_word_str,((
_tmpE49[40]=& Cyc_Toc__swap_dyneither_str,_tmpE49)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),41);};;_pop_dynregion(d);};}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();{struct _RegionHandle _tmpA63=
_new_region("start");struct _RegionHandle*start=& _tmpA63;_push_region(start);Cyc_Toc_decls_to_c(
start,Cyc_Toc_empty_env(start),ds,1,0);{struct _DynRegionHandle*_tmpA65;struct Cyc_Toc_TocState
_tmpA64=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA65=
_tmpA64.dyn;Cyc_Core_free_dynregion(_tmpA65);};{struct Cyc_List_List*_tmpA66=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);
_npop_handler(0);return _tmpA66;};;_pop_region(start);};}
