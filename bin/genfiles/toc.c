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
typedef void*Cyc_sarg_t;int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{
void*v;};typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[
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
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};typedef struct Cyc_List_List*
Cyc_List_list_t;typedef struct Cyc_List_List*Cyc_List_List_t;struct Cyc_List_List*
Cyc_List_list(struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void Cyc_List_iter_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};void*Cyc_List_nth(struct Cyc_List_List*x,int n);int Cyc_List_forall(int(*
pred)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rzip(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Iter_Iter{void*env;int(*
next)(void*env,void*dest);};typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;typedef struct Cyc_Set_Set*Cyc_Set_set_t;
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*
elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];
struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;typedef const struct Cyc_Dict_T*
Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;
const struct Cyc_Dict_T*t;};typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char
Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[
7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict Cyc_Dict_rempty(
struct _RegionHandle*,int(*cmp)(void*,void*));struct Cyc_Dict_Dict Cyc_Dict_rshare(
struct _RegionHandle*,struct Cyc_Dict_Dict);struct Cyc_Dict_Dict Cyc_Dict_insert(
struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,
void*k);void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];
struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;typedef
struct Cyc_Position_Segment*Cyc_Position_seg_t;struct _dyneither_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};typedef void*Cyc_Position_error_kind_t;
struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};typedef struct Cyc_Position_Error*Cyc_Position_error_t;
extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{char*
tag;};struct Cyc_Xarray_Xarray{struct _RegionHandle*r;struct _dyneither_ptr elmts;
int num_elmts;};typedef struct Cyc_Xarray_Xarray*Cyc_Xarray_xarray_t;int Cyc_Xarray_length(
struct Cyc_Xarray_Xarray*);void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*);int Cyc_Xarray_add_ind(
struct Cyc_Xarray_Xarray*,void*);typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};
union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n
Loc_n;};typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct
_tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};typedef struct _tuple0*
Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;typedef struct _tuple0*
Cyc_Absyn_typedef_name_t;typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t;
typedef enum Cyc_Absyn_Size_of Cyc_Absyn_size_of_t;typedef struct Cyc_Absyn_Kind*Cyc_Absyn_kind_t;
typedef void*Cyc_Absyn_kindbound_t;typedef struct Cyc_Absyn_Tvar*Cyc_Absyn_tvar_t;
typedef enum Cyc_Absyn_Sign Cyc_Absyn_sign_t;typedef enum Cyc_Absyn_AggrKind Cyc_Absyn_aggr_kind_t;
typedef void*Cyc_Absyn_bounds_t;typedef struct Cyc_Absyn_PtrAtts Cyc_Absyn_ptr_atts_t;
typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t;typedef struct Cyc_Absyn_VarargInfo
Cyc_Absyn_vararg_info_t;typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;typedef
struct Cyc_Absyn_DatatypeInfo Cyc_Absyn_datatype_info_t;typedef struct Cyc_Absyn_DatatypeFieldInfo
Cyc_Absyn_datatype_field_info_t;typedef struct Cyc_Absyn_AggrInfo Cyc_Absyn_aggr_info_t;
typedef struct Cyc_Absyn_ArrayInfo Cyc_Absyn_array_info_t;typedef void*Cyc_Absyn_type_t;
typedef void*Cyc_Absyn_rgntype_t;typedef union Cyc_Absyn_Cnst Cyc_Absyn_cnst_t;
typedef enum Cyc_Absyn_Primop Cyc_Absyn_primop_t;typedef enum Cyc_Absyn_Incrementor
Cyc_Absyn_incrementor_t;typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
typedef void*Cyc_Absyn_raw_exp_t;typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_t;
typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_opt_t;typedef void*Cyc_Absyn_raw_stmt_t;
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
_tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int
tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
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
int tag;struct Cyc_Absyn_Datatypedecl*f1;};typedef void*Cyc_Absyn_raw_type_decl_t;
struct Cyc_Absyn_TypeDecl{void*r;struct Cyc_Position_Segment*loc;};typedef struct
Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_struct{int tag;
};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;
int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};
struct Cyc_Absyn_DatatypeFieldType_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo
f1;};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};
struct Cyc_Absyn_IntType_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of
f2;};struct Cyc_Absyn_FloatType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
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
};struct Cyc_Absyn_RefCntRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{int
tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_struct{
int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};extern struct Cyc_Absyn_HeapRgn_struct
Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;
struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*f1;int f2;struct
Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};typedef
void*Cyc_Absyn_funcparams_t;enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,
Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct
Cyc_Absyn_Stdcall_att_struct{int tag;};struct Cyc_Absyn_Cdecl_att_struct{int tag;};
struct Cyc_Absyn_Fastcall_att_struct{int tag;};struct Cyc_Absyn_Noreturn_att_struct{
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
struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;
char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct
_union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};
struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct
_union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;
struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct
_dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct
_union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};union Cyc_Absyn_Cnst
Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(
enum Cyc_Absyn_Sign,short);union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,
int);union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);enum Cyc_Absyn_Primop{
Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod
 = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte
 = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand
 = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift
 = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{
Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec
 = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*
injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_struct{
int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{int tag;
unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion
 = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{
int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;
int fat_result;};struct Cyc_Absyn_Const_e_struct{int tag;union Cyc_Absyn_Cnst f1;};
struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{
int tag;struct _tuple0*f1;};struct Cyc_Absyn_Primop_e_struct{int tag;enum Cyc_Absyn_Primop
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
int tag;struct Cyc_List_List*f1;};struct _tuple8{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple8*f1;struct
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
_tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_struct{
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
};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_struct{
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
struct _dyneither_ptr f,int i,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
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
struct _dyneither_ptr*);struct _dyneither_ptr*Cyc_Absyn_fieldname(int);struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);extern int Cyc_Absyn_no_regions;
struct Cyc_PP_Ppstate;typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int
add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;
int print_all_kinds;int print_all_effects;int print_using_stmts;int
print_externC_stmts;int print_full_evars;int print_zeroterm;int
generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple0*);struct Cyc_RgnOrder_RgnPO;typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
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
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void
Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*
k2);struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*
def);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(
void*t);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(void*t);extern
struct Cyc_Absyn_Kind Cyc_Tcutil_ak;extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;extern
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*,struct Cyc_List_List*,void*);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(
void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple10{struct Cyc_Absyn_Tqual
f1;void*f2;};void*Cyc_Tcutil_snd_tqt(struct _tuple10*);void Cyc_Tcutil_check_no_qual(
struct Cyc_Position_Segment*loc,void*t);struct _tuple11{unsigned int f1;int f2;};
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_c_can_eval(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};
struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};typedef void*Cyc_CfFlowInfo_root_t;
struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};typedef struct
Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL
 = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};typedef enum Cyc_CfFlowInfo_InitLevel
Cyc_CfFlowInfo_initlevel_t;struct _union_RelnOp_EqualConst{int tag;unsigned int val;
};struct _tuple12{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct
_union_RelnOp_LessVar{int tag;struct _tuple12 val;};struct _union_RelnOp_LessNumelts{
int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;
unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*
val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct
_union_RelnOp_LessEqConst LessEqConst;};typedef union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_reln_op_t;
struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp
rop;};typedef struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_reln_t;typedef struct Cyc_List_List*
Cyc_CfFlowInfo_relns_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*
val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{
struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;typedef void*Cyc_CfFlowInfo_absRval_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;typedef struct _dyneither_ptr
Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int
fieldnum;};typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;
struct Cyc_CfFlowInfo_Zero_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{
int tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;
enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;
struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_struct{
int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct
_union_FlowInfo_BottomFL{int tag;int val;};struct _tuple13{struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple13
val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct
_union_FlowInfo_ReachableFL ReachableFL;};typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;
void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*
esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict
mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};typedef struct Cyc_CfFlowInfo_FlowEnv*
Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_struct{int tag;};
struct Cyc_CfFlowInfo_RefCntRgn_k_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};typedef void*Cyc_CfFlowInfo_killrgn_t;struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds);struct _tuple0*Cyc_Toc_temp_var();extern
struct _dyneither_ptr Cyc_Toc_globals;extern int Cyc_noexpand_r;int Cyc_Toc_warn_bounds_checks=
0;int Cyc_Toc_warn_all_null_deref=0;unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;static struct Cyc_List_List*Cyc_Toc_result_decls=
0;struct Cyc_Toc_TocState{struct _DynRegionHandle*dyn;struct Cyc_List_List**
tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**
abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*
xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*
temp_labels;};static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=0;struct _tuple14{
struct _tuple0*f1;struct _dyneither_ptr f2;};int Cyc_Toc_qvar_tag_cmp(struct _tuple14*
x,struct _tuple14*y){struct _tuple0*_tmp1;struct _dyneither_ptr _tmp2;struct _tuple14
_tmp0=*x;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;{struct _tuple0*_tmp4;struct _dyneither_ptr
_tmp5;struct _tuple14 _tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{int i=Cyc_Absyn_qvar_cmp(
_tmp1,_tmp4);if(i != 0)return i;return Cyc_strcmp((struct _dyneither_ptr)_tmp2,(
struct _dyneither_ptr)_tmp5);};};}struct _tuple15{struct Cyc_Absyn_Aggrdecl*f1;void*
f2;};void*Cyc_Toc_aggrdecl_type(struct _tuple0*q,void*(*type_maker)(struct _tuple0*)){
struct _DynRegionHandle*_tmp7;struct Cyc_Dict_Dict*_tmp8;struct Cyc_Toc_TocState
_tmp6=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp7=_tmp6.dyn;
_tmp8=_tmp6.aggrs_so_far;{struct _DynRegionFrame _tmp9;struct _RegionHandle*d=
_open_dynregion(& _tmp9,_tmp7);{struct _tuple15**v=((struct _tuple15**(*)(struct Cyc_Dict_Dict
d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp8,q);if(v == 0){void*_tmpA=
type_maker(q);_npop_handler(0);return _tmpA;}else{struct _tuple15 _tmpC;void*_tmpD;
struct _tuple15*_tmpB=*v;_tmpC=*_tmpB;_tmpD=_tmpC.f2;{void*_tmpE=_tmpD;
_npop_handler(0);return _tmpE;};}};_pop_dynregion(d);};}static int Cyc_Toc_tuple_type_counter=
0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=0;
char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_struct{
char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_struct{
char*tag;};static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const char*_tmpAB1;void*_tmpAB0;(_tmpAB0=0,
Cyc_fprintf(Cyc_stderr,((_tmpAB1="\n",_tag_dyneither(_tmpAB1,sizeof(char),2))),
_tag_dyneither(_tmpAB0,sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_struct _tmpAB4;struct Cyc_Toc_Toc_Unimplemented_struct*
_tmpAB3;(int)_throw((void*)((_tmpAB3=_cycalloc_atomic(sizeof(*_tmpAB3)),((
_tmpAB3[0]=((_tmpAB4.tag=Cyc_Toc_Toc_Unimplemented,_tmpAB4)),_tmpAB3)))));};}
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(
Cyc_stderr,fmt,ap);{const char*_tmpAB7;void*_tmpAB6;(_tmpAB6=0,Cyc_fprintf(Cyc_stderr,((
_tmpAB7="\n",_tag_dyneither(_tmpAB7,sizeof(char),2))),_tag_dyneither(_tmpAB6,
sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Impossible_struct
_tmpABA;struct Cyc_Toc_Toc_Impossible_struct*_tmpAB9;(int)_throw((void*)((_tmpAB9=
_cycalloc_atomic(sizeof(*_tmpAB9)),((_tmpAB9[0]=((_tmpABA.tag=Cyc_Toc_Toc_Impossible,
_tmpABA)),_tmpAB9)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_struct{
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
11,{{.UnknownAggr={1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;static
struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;if(skip_stmt_opt == 0){struct Cyc_Absyn_Stmt**
_tmpABB;skip_stmt_opt=((_tmpABB=_cycalloc(sizeof(*_tmpABB)),((_tmpABB[0]=Cyc_Absyn_skip_stmt(
0),_tmpABB))));}return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(
void*t,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,
0);}static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Cast_e_struct
_tmpABE;struct Cyc_Absyn_Cast_e_struct*_tmpABD;return(void*)((_tmpABD=_cycalloc(
sizeof(*_tmpABD)),((_tmpABD[0]=((_tmpABE.tag=15,((_tmpABE.f1=(void*)t,((_tmpABE.f2=
e,((_tmpABE.f3=0,((_tmpABE.f4=Cyc_Absyn_No_coercion,_tmpABE)))))))))),_tmpABD))));}
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Deref_e_struct
_tmpAC1;struct Cyc_Absyn_Deref_e_struct*_tmpAC0;return(void*)((_tmpAC0=_cycalloc(
sizeof(*_tmpAC0)),((_tmpAC0[0]=((_tmpAC1.tag=21,((_tmpAC1.f1=e,_tmpAC1)))),
_tmpAC0))));}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){struct Cyc_Absyn_Subscript_e_struct _tmpAC4;struct Cyc_Absyn_Subscript_e_struct*
_tmpAC3;return(void*)((_tmpAC3=_cycalloc(sizeof(*_tmpAC3)),((_tmpAC3[0]=((
_tmpAC4.tag=24,((_tmpAC4.f1=e1,((_tmpAC4.f2=e2,_tmpAC4)))))),_tmpAC3))));}static
void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct
_tmpAC7;struct Cyc_Absyn_StmtExp_e_struct*_tmpAC6;return(void*)((_tmpAC6=
_cycalloc(sizeof(*_tmpAC6)),((_tmpAC6[0]=((_tmpAC7.tag=37,((_tmpAC7.f1=s,_tmpAC7)))),
_tmpAC6))));}static void*Cyc_Toc_sizeoftyp_exp_r(void*t){struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpACA;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpAC9;return(void*)((_tmpAC9=
_cycalloc(sizeof(*_tmpAC9)),((_tmpAC9[0]=((_tmpACA.tag=18,((_tmpACA.f1=(void*)t,
_tmpACA)))),_tmpAC9))));}static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct _tmpACD;struct Cyc_Absyn_FnCall_e_struct*
_tmpACC;return(void*)((_tmpACC=_cycalloc(sizeof(*_tmpACC)),((_tmpACC[0]=((
_tmpACD.tag=11,((_tmpACD.f1=e,((_tmpACD.f2=es,((_tmpACD.f3=0,_tmpACD)))))))),
_tmpACC))));}static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_struct
_tmpAD0;struct Cyc_Absyn_Exp_s_struct*_tmpACF;return(void*)((_tmpACF=_cycalloc(
sizeof(*_tmpACF)),((_tmpACF[0]=((_tmpAD0.tag=1,((_tmpAD0.f1=e,_tmpAD0)))),
_tmpACF))));}static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2){struct Cyc_Absyn_Seq_s_struct _tmpAD3;struct Cyc_Absyn_Seq_s_struct*_tmpAD2;
return(void*)((_tmpAD2=_cycalloc(sizeof(*_tmpAD2)),((_tmpAD2[0]=((_tmpAD3.tag=2,((
_tmpAD3.f1=s1,((_tmpAD3.f2=s2,_tmpAD3)))))),_tmpAD2))));}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct
_tmpAD6;struct Cyc_Absyn_Conditional_e_struct*_tmpAD5;return(void*)((_tmpAD5=
_cycalloc(sizeof(*_tmpAD5)),((_tmpAD5[0]=((_tmpAD6.tag=6,((_tmpAD6.f1=e1,((
_tmpAD6.f2=e2,((_tmpAD6.f3=e3,_tmpAD6)))))))),_tmpAD5))));}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrMember_e_struct
_tmpAD9;struct Cyc_Absyn_AggrMember_e_struct*_tmpAD8;return(void*)((_tmpAD8=
_cycalloc(sizeof(*_tmpAD8)),((_tmpAD8[0]=((_tmpAD9.tag=22,((_tmpAD9.f1=e,((
_tmpAD9.f2=n,((_tmpAD9.f3=0,((_tmpAD9.f4=0,_tmpAD9)))))))))),_tmpAD8))));}static
void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_struct _tmpADC;struct Cyc_Absyn_AggrArrow_e_struct*
_tmpADB;return(void*)((_tmpADB=_cycalloc(sizeof(*_tmpADB)),((_tmpADB[0]=((
_tmpADC.tag=23,((_tmpADC.f1=e,((_tmpADC.f2=n,((_tmpADC.f3=0,((_tmpADC.f4=0,
_tmpADC)))))))))),_tmpADB))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmpADF;
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpADE;return(void*)((_tmpADE=_cycalloc(
sizeof(*_tmpADE)),((_tmpADE[0]=((_tmpADF.tag=36,((_tmpADF.f1=tdopt,((_tmpADF.f2=
ds,_tmpADF)))))),_tmpADE))));}static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_struct _tmpAE2;
struct Cyc_Absyn_Goto_s_struct*_tmpAE1;return(void*)((_tmpAE1=_cycalloc(sizeof(*
_tmpAE1)),((_tmpAE1[0]=((_tmpAE2.tag=8,((_tmpAE2.f1=v,((_tmpAE2.f2=s,_tmpAE2)))))),
_tmpAE1))));}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{
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
Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmpC8;int _tmpCA;int _tmpCC;_LL1: {
struct Cyc_Absyn_IntType_struct*_tmpC7=(struct Cyc_Absyn_IntType_struct*)_tmpC6;
if(_tmpC7->tag != 6)goto _LL3;else{_tmpC8=_tmpC7->f2;}}_LL2: switch(_tmpC8){case Cyc_Absyn_Char_sz:
_LLD: function=fS->fchar;break;case Cyc_Absyn_Short_sz: _LLE: function=fS->fshort;
break;case Cyc_Absyn_Int_sz: _LLF: function=fS->fint;break;default: _LL10: {struct
Cyc_Core_Impossible_struct _tmpAE8;const char*_tmpAE7;struct Cyc_Core_Impossible_struct*
_tmpAE6;(int)_throw((void*)((_tmpAE6=_cycalloc(sizeof(*_tmpAE6)),((_tmpAE6[0]=((
_tmpAE8.tag=Cyc_Core_Impossible,((_tmpAE8.f1=((_tmpAE7="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpAE7,sizeof(char),44))),_tmpAE8)))),_tmpAE6)))));}}goto _LL0;
_LL3: {struct Cyc_Absyn_FloatType_struct*_tmpC9=(struct Cyc_Absyn_FloatType_struct*)
_tmpC6;if(_tmpC9->tag != 7)goto _LL5;else{_tmpCA=_tmpC9->f1;if(_tmpCA != 0)goto _LL5;}}
_LL4: function=fS->ffloat;goto _LL0;_LL5: {struct Cyc_Absyn_FloatType_struct*_tmpCB=(
struct Cyc_Absyn_FloatType_struct*)_tmpC6;if(_tmpCB->tag != 7)goto _LL7;else{_tmpCC=
_tmpCB->f1;if(_tmpCC != 1)goto _LL7;}}_LL6: function=fS->fdouble;goto _LL0;_LL7: {
struct Cyc_Absyn_FloatType_struct*_tmpCD=(struct Cyc_Absyn_FloatType_struct*)
_tmpC6;if(_tmpCD->tag != 7)goto _LL9;}_LL8: function=fS->flongdouble;goto _LL0;_LL9: {
struct Cyc_Absyn_PointerType_struct*_tmpCE=(struct Cyc_Absyn_PointerType_struct*)
_tmpC6;if(_tmpCE->tag != 5)goto _LLB;}_LLA: function=fS->fvoidstar;goto _LL0;_LLB:;
_LLC: {struct Cyc_Core_Impossible_struct _tmpAF5;const char*_tmpAF4;void*_tmpAF3[1];
struct Cyc_String_pa_struct _tmpAF2;struct Cyc_Core_Impossible_struct*_tmpAF1;(int)
_throw((void*)((_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1[0]=((_tmpAF5.tag=
Cyc_Core_Impossible,((_tmpAF5.f1=(struct _dyneither_ptr)((_tmpAF2.tag=0,((_tmpAF2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAF3[
0]=& _tmpAF2,Cyc_aprintf(((_tmpAF4="impossible expression type %s (not int, float, double, or pointer)",
_tag_dyneither(_tmpAF4,sizeof(char),67))),_tag_dyneither(_tmpAF3,sizeof(void*),1)))))))),
_tmpAF5)))),_tmpAF1)))));}_LL0:;}return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(
fS,(void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v);}struct Cyc_Absyn_Exp*
Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*
arr){void*_tmpD7=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
arr->topt))->v);struct Cyc_Absyn_PtrInfo _tmpD9;void*_tmpDA;_LL13: {struct Cyc_Absyn_PointerType_struct*
_tmpD8=(struct Cyc_Absyn_PointerType_struct*)_tmpD7;if(_tmpD8->tag != 5)goto _LL15;
else{_tmpD9=_tmpD8->f1;_tmpDA=_tmpD9.elt_typ;}}_LL14: return Cyc_Toc_getFunctionType(
fS,_tmpDA);_LL15:;_LL16: {struct Cyc_Core_Impossible_struct _tmpAFB;const char*
_tmpAFA;struct Cyc_Core_Impossible_struct*_tmpAF9;(int)_throw((void*)((_tmpAF9=
_cycalloc(sizeof(*_tmpAF9)),((_tmpAF9[0]=((_tmpAFB.tag=Cyc_Core_Impossible,((
_tmpAFB.f1=((_tmpAFA="impossible type (not pointer)",_tag_dyneither(_tmpAFA,
sizeof(char),30))),_tmpAFB)))),_tmpAF9)))));}_LL12:;}struct _tuple16{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){void*
_tmpDE=e->r;union Cyc_Absyn_Cnst _tmpE0;struct _tuple3 _tmpE1;char _tmpE2;union Cyc_Absyn_Cnst
_tmpE4;struct _dyneither_ptr _tmpE5;union Cyc_Absyn_Cnst _tmpE7;struct _tuple4 _tmpE8;
short _tmpE9;union Cyc_Absyn_Cnst _tmpEB;struct _tuple5 _tmpEC;int _tmpED;union Cyc_Absyn_Cnst
_tmpEF;struct _tuple6 _tmpF0;long long _tmpF1;union Cyc_Absyn_Cnst _tmpF3;int _tmpF4;
struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_List_List*_tmpF8;struct Cyc_List_List*_tmpFA;
struct Cyc_List_List*_tmpFC;struct Cyc_List_List*_tmpFE;struct Cyc_List_List*
_tmp100;_LL18: {struct Cyc_Absyn_Const_e_struct*_tmpDF=(struct Cyc_Absyn_Const_e_struct*)
_tmpDE;if(_tmpDF->tag != 0)goto _LL1A;else{_tmpE0=_tmpDF->f1;if((_tmpE0.Char_c).tag
!= 2)goto _LL1A;_tmpE1=(struct _tuple3)(_tmpE0.Char_c).val;_tmpE2=_tmpE1.f2;}}
_LL19: return _tmpE2 == '\000';_LL1A: {struct Cyc_Absyn_Const_e_struct*_tmpE3=(
struct Cyc_Absyn_Const_e_struct*)_tmpDE;if(_tmpE3->tag != 0)goto _LL1C;else{_tmpE4=
_tmpE3->f1;if((_tmpE4.Wchar_c).tag != 3)goto _LL1C;_tmpE5=(struct _dyneither_ptr)(
_tmpE4.Wchar_c).val;}}_LL1B: {unsigned long _tmp101=Cyc_strlen((struct
_dyneither_ptr)_tmpE5);int i=0;if(_tmp101 >= 2  && *((const char*)
_check_dyneither_subscript(_tmpE5,sizeof(char),0))== '\\'){if(*((const char*)
_check_dyneither_subscript(_tmpE5,sizeof(char),1))== '0')i=2;else{if((*((const
char*)_check_dyneither_subscript(_tmpE5,sizeof(char),1))== 'x'  && _tmp101 >= 3)
 && *((const char*)_check_dyneither_subscript(_tmpE5,sizeof(char),2))== '0')i=3;
else{return 0;}}for(0;i < _tmp101;++ i){if(*((const char*)_check_dyneither_subscript(
_tmpE5,sizeof(char),i))!= '0')return 0;}return 1;}else{return 0;}}_LL1C: {struct Cyc_Absyn_Const_e_struct*
_tmpE6=(struct Cyc_Absyn_Const_e_struct*)_tmpDE;if(_tmpE6->tag != 0)goto _LL1E;
else{_tmpE7=_tmpE6->f1;if((_tmpE7.Short_c).tag != 4)goto _LL1E;_tmpE8=(struct
_tuple4)(_tmpE7.Short_c).val;_tmpE9=_tmpE8.f2;}}_LL1D: return _tmpE9 == 0;_LL1E: {
struct Cyc_Absyn_Const_e_struct*_tmpEA=(struct Cyc_Absyn_Const_e_struct*)_tmpDE;
if(_tmpEA->tag != 0)goto _LL20;else{_tmpEB=_tmpEA->f1;if((_tmpEB.Int_c).tag != 5)
goto _LL20;_tmpEC=(struct _tuple5)(_tmpEB.Int_c).val;_tmpED=_tmpEC.f2;}}_LL1F:
return _tmpED == 0;_LL20: {struct Cyc_Absyn_Const_e_struct*_tmpEE=(struct Cyc_Absyn_Const_e_struct*)
_tmpDE;if(_tmpEE->tag != 0)goto _LL22;else{_tmpEF=_tmpEE->f1;if((_tmpEF.LongLong_c).tag
!= 6)goto _LL22;_tmpF0=(struct _tuple6)(_tmpEF.LongLong_c).val;_tmpF1=_tmpF0.f2;}}
_LL21: return _tmpF1 == 0;_LL22: {struct Cyc_Absyn_Const_e_struct*_tmpF2=(struct Cyc_Absyn_Const_e_struct*)
_tmpDE;if(_tmpF2->tag != 0)goto _LL24;else{_tmpF3=_tmpF2->f1;if((_tmpF3.Null_c).tag
!= 1)goto _LL24;_tmpF4=(int)(_tmpF3.Null_c).val;}}_LL23: return 1;_LL24: {struct Cyc_Absyn_Cast_e_struct*
_tmpF5=(struct Cyc_Absyn_Cast_e_struct*)_tmpDE;if(_tmpF5->tag != 15)goto _LL26;
else{_tmpF6=_tmpF5->f2;}}_LL25: return Cyc_Toc_is_zero(_tmpF6);_LL26: {struct Cyc_Absyn_Tuple_e_struct*
_tmpF7=(struct Cyc_Absyn_Tuple_e_struct*)_tmpDE;if(_tmpF7->tag != 25)goto _LL28;
else{_tmpF8=_tmpF7->f1;}}_LL27: return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpF8);_LL28: {struct
Cyc_Absyn_Array_e_struct*_tmpF9=(struct Cyc_Absyn_Array_e_struct*)_tmpDE;if(
_tmpF9->tag != 27)goto _LL2A;else{_tmpFA=_tmpF9->f1;}}_LL29: _tmpFC=_tmpFA;goto
_LL2B;_LL2A: {struct Cyc_Absyn_Aggregate_e_struct*_tmpFB=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpDE;if(_tmpFB->tag != 29)goto _LL2C;else{_tmpFC=_tmpFB->f3;}}_LL2B: _tmpFE=
_tmpFC;goto _LL2D;_LL2C: {struct Cyc_Absyn_CompoundLit_e_struct*_tmpFD=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpDE;if(_tmpFD->tag != 26)goto _LL2E;else{_tmpFE=_tmpFD->f2;}}_LL2D: _tmp100=
_tmpFE;goto _LL2F;_LL2E: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpFF=(struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmpDE;if(_tmpFF->tag != 36)goto _LL30;else{
_tmp100=_tmpFF->f2;}}_LL2F: for(0;_tmp100 != 0;_tmp100=_tmp100->tl){if(!Cyc_Toc_is_zero((*((
struct _tuple16*)_tmp100->hd)).f2))return 0;}return 1;_LL30:;_LL31: return 0;_LL17:;}
static int Cyc_Toc_is_nullable(void*t){void*_tmp102=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_PtrInfo _tmp104;struct Cyc_Absyn_PtrAtts _tmp105;union Cyc_Absyn_Constraint*
_tmp106;_LL33: {struct Cyc_Absyn_PointerType_struct*_tmp103=(struct Cyc_Absyn_PointerType_struct*)
_tmp102;if(_tmp103->tag != 5)goto _LL35;else{_tmp104=_tmp103->f1;_tmp105=_tmp104.ptr_atts;
_tmp106=_tmp105.nullable;}}_LL34: return((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp106);_LL35:;_LL36: {const char*_tmpAFE;void*_tmpAFD;(
_tmpAFD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAFE="is_nullable",_tag_dyneither(_tmpAFE,sizeof(char),12))),_tag_dyneither(
_tmpAFD,sizeof(void*),0)));}_LL32:;}static char _tmp115[1]="";static char _tmp116[8]="*bogus*";
static struct _tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct
_dyneither_ptr tag){struct _DynRegionHandle*_tmp10A;struct Cyc_Dict_Dict*_tmp10B;
struct Cyc_Toc_TocState _tmp109=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp10A=_tmp109.dyn;_tmp10B=_tmp109.qvar_tags;{static struct _dyneither_ptr
bogus_string={_tmp116,_tmp116,_tmp116 + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
4,0}},& bogus_string};static struct _tuple14 pair={& bogus_qvar,{_tmp115,_tmp115,
_tmp115 + 1}};{struct _tuple14 _tmpAFF;pair=((_tmpAFF.f1=x,((_tmpAFF.f2=tag,_tmpAFF))));}{
struct _DynRegionFrame _tmp10D;struct _RegionHandle*d=_open_dynregion(& _tmp10D,
_tmp10A);{struct _tuple0**_tmp10E=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,
struct _tuple14*k))Cyc_Dict_lookup_opt)(*_tmp10B,(struct _tuple14*)& pair);if(
_tmp10E != 0){struct _tuple0*_tmp10F=*_tmp10E;_npop_handler(0);return _tmp10F;}{
struct _tuple14*_tmpB00;struct _tuple14*_tmp110=(_tmpB00=_cycalloc(sizeof(*_tmpB00)),((
_tmpB00->f1=x,((_tmpB00->f2=tag,_tmpB00)))));struct _dyneither_ptr*_tmpB03;struct
_tuple0*_tmpB02;struct _tuple0*res=(_tmpB02=_cycalloc(sizeof(*_tmpB02)),((_tmpB02->f1=(*
x).f1,((_tmpB02->f2=((_tmpB03=_cycalloc(sizeof(*_tmpB03)),((_tmpB03[0]=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*x).f2,(struct
_dyneither_ptr)tag),_tmpB03)))),_tmpB02)))));*_tmp10B=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple0*v))Cyc_Dict_insert)(*_tmp10B,(
struct _tuple14*)_tmp110,res);{struct _tuple0*_tmp111=res;_npop_handler(0);return
_tmp111;};};};_pop_dynregion(d);};};}static void*Cyc_Toc_typ_to_c(void*t);struct
_tuple17{void*f1;struct Cyc_List_List*f2;};static void*Cyc_Toc_add_tuple_type(
struct Cyc_List_List*tqs0){struct _DynRegionHandle*_tmp118;struct Cyc_List_List**
_tmp119;struct Cyc_Toc_TocState _tmp117=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp118=_tmp117.dyn;_tmp119=_tmp117.tuple_types;{struct _DynRegionFrame _tmp11A;
struct _RegionHandle*d=_open_dynregion(& _tmp11A,_tmp118);{struct Cyc_List_List*
_tmp11B=*_tmp119;for(0;_tmp11B != 0;_tmp11B=_tmp11B->tl){struct _tuple17 _tmp11D;
void*_tmp11E;struct Cyc_List_List*_tmp11F;struct _tuple17*_tmp11C=(struct _tuple17*)
_tmp11B->hd;_tmp11D=*_tmp11C;_tmp11E=_tmp11D.f1;_tmp11F=_tmp11D.f2;{struct Cyc_List_List*
_tmp120=tqs0;for(0;_tmp120 != 0  && _tmp11F != 0;(_tmp120=_tmp120->tl,_tmp11F=
_tmp11F->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple10*)_tmp120->hd)).f2,(void*)
_tmp11F->hd))break;}if(_tmp120 == 0  && _tmp11F == 0){void*_tmp121=_tmp11E;
_npop_handler(0);return _tmp121;}};}}{struct Cyc_Int_pa_struct _tmpB0B;void*_tmpB0A[
1];const char*_tmpB09;struct _dyneither_ptr*_tmpB08;struct _dyneither_ptr*xname=(
_tmpB08=_cycalloc(sizeof(*_tmpB08)),((_tmpB08[0]=(struct _dyneither_ptr)((_tmpB0B.tag=
1,((_tmpB0B.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB0A[0]=& _tmpB0B,
Cyc_aprintf(((_tmpB09="_tuple%d",_tag_dyneither(_tmpB09,sizeof(char),9))),
_tag_dyneither(_tmpB0A,sizeof(void*),1)))))))),_tmpB08)));void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp122=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmpB0E;struct Cyc_List_List*
_tmpB0D;_tmp122=((_tmpB0D=_cycalloc(sizeof(*_tmpB0D)),((_tmpB0D->hd=((_tmpB0E=
_cycalloc(sizeof(*_tmpB0E)),((_tmpB0E->name=Cyc_Absyn_fieldname(i),((_tmpB0E->tq=
Cyc_Toc_mt_tq,((_tmpB0E->type=(void*)ts2->hd,((_tmpB0E->width=0,((_tmpB0E->attributes=
0,_tmpB0E)))))))))))),((_tmpB0D->tl=_tmp122,_tmpB0D))))));}}_tmp122=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp122);{struct Cyc_Absyn_AggrdeclImpl*
_tmpB13;struct _tuple0*_tmpB12;struct Cyc_Absyn_Aggrdecl*_tmpB11;struct Cyc_Absyn_Aggrdecl*
_tmp125=(_tmpB11=_cycalloc(sizeof(*_tmpB11)),((_tmpB11->kind=Cyc_Absyn_StructA,((
_tmpB11->sc=Cyc_Absyn_Public,((_tmpB11->name=((_tmpB12=_cycalloc(sizeof(*_tmpB12)),((
_tmpB12->f1=Cyc_Absyn_Rel_n(0),((_tmpB12->f2=xname,_tmpB12)))))),((_tmpB11->tvs=
0,((_tmpB11->impl=((_tmpB13=_cycalloc(sizeof(*_tmpB13)),((_tmpB13->exist_vars=0,((
_tmpB13->rgn_po=0,((_tmpB13->fields=_tmp122,((_tmpB13->tagged=0,_tmpB13)))))))))),((
_tmpB11->attributes=0,_tmpB11)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*
_tmpB19;struct Cyc_Absyn_Aggr_d_struct _tmpB18;struct Cyc_List_List*_tmpB17;Cyc_Toc_result_decls=((
_tmpB17=_cycalloc(sizeof(*_tmpB17)),((_tmpB17->hd=Cyc_Absyn_new_decl((void*)((
_tmpB19=_cycalloc(sizeof(*_tmpB19)),((_tmpB19[0]=((_tmpB18.tag=6,((_tmpB18.f1=
_tmp125,_tmpB18)))),_tmpB19)))),0),((_tmpB17->tl=Cyc_Toc_result_decls,_tmpB17))))));}{
struct _tuple17*_tmpB1C;struct Cyc_List_List*_tmpB1B;*_tmp119=((_tmpB1B=
_region_malloc(d,sizeof(*_tmpB1B)),((_tmpB1B->hd=((_tmpB1C=_region_malloc(d,
sizeof(*_tmpB1C)),((_tmpB1C->f1=x,((_tmpB1C->f2=ts,_tmpB1C)))))),((_tmpB1B->tl=*
_tmp119,_tmpB1B))))));}{void*_tmp12B=x;_npop_handler(0);return _tmp12B;};};};;
_pop_dynregion(d);};}struct _tuple18{struct _tuple0*f1;struct Cyc_List_List*f2;void*
f3;};static void*Cyc_Toc_add_struct_type(struct _tuple0*struct_name,struct Cyc_List_List*
type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){struct
_DynRegionHandle*_tmp134;struct Cyc_List_List**_tmp135;struct Cyc_Toc_TocState
_tmp133=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp134=
_tmp133.dyn;_tmp135=_tmp133.abs_struct_types;{struct _DynRegionFrame _tmp136;
struct _RegionHandle*d=_open_dynregion(& _tmp136,_tmp134);{struct Cyc_List_List*
_tmp137=*_tmp135;for(0;_tmp137 != 0;_tmp137=_tmp137->tl){struct _tuple18 _tmp139;
struct _tuple0*_tmp13A;struct Cyc_List_List*_tmp13B;void*_tmp13C;struct _tuple18*
_tmp138=(struct _tuple18*)_tmp137->hd;_tmp139=*_tmp138;_tmp13A=_tmp139.f1;_tmp13B=
_tmp139.f2;_tmp13C=_tmp139.f3;if(Cyc_Absyn_qvar_cmp(_tmp13A,struct_name)== 0  && ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(type_args)== ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp13B)){int okay=1;{struct Cyc_List_List*_tmp13D=type_args;
for(0;_tmp13D != 0;(_tmp13D=_tmp13D->tl,_tmp13B=_tmp13B->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp13D->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp13B))->hd)){okay=
0;break;}}}if(okay){void*_tmp13E=_tmp13C;_npop_handler(0);return _tmp13E;}}}}{
struct Cyc_Int_pa_struct _tmpB24;void*_tmpB23[1];const char*_tmpB22;struct
_dyneither_ptr*_tmpB21;struct _dyneither_ptr*xname=(_tmpB21=_cycalloc(sizeof(*
_tmpB21)),((_tmpB21[0]=(struct _dyneither_ptr)((_tmpB24.tag=1,((_tmpB24.f1=(
unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB23[0]=& _tmpB24,Cyc_aprintf(((
_tmpB22="_tuple%d",_tag_dyneither(_tmpB22,sizeof(char),9))),_tag_dyneither(
_tmpB23,sizeof(void*),1)))))))),_tmpB21)));void*x=Cyc_Absyn_strct(xname);struct
Cyc_List_List*_tmp13F=0;{struct _tuple18*_tmpB27;struct Cyc_List_List*_tmpB26;*
_tmp135=((_tmpB26=_region_malloc(d,sizeof(*_tmpB26)),((_tmpB26->hd=((_tmpB27=
_region_malloc(d,sizeof(*_tmpB27)),((_tmpB27->f1=struct_name,((_tmpB27->f2=
type_args,((_tmpB27->f3=x,_tmpB27)))))))),((_tmpB26->tl=*_tmp135,_tmpB26))))));}{
struct _RegionHandle _tmp142=_new_region("r");struct _RegionHandle*r=& _tmp142;
_push_region(r);{struct Cyc_List_List*_tmp143=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,type_vars,type_args);for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp144=(struct Cyc_Absyn_Aggrfield*)fields->hd;void*t=
_tmp144->type;t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp143,t));if(Cyc_Tcutil_unify(
t,(void*)& Cyc_Absyn_VoidType_val))break;t=Cyc_Toc_typ_to_c(t);{struct Cyc_Absyn_Aggrfield*
_tmpB2A;struct Cyc_List_List*_tmpB29;_tmp13F=((_tmpB29=_cycalloc(sizeof(*_tmpB29)),((
_tmpB29->hd=((_tmpB2A=_cycalloc(sizeof(*_tmpB2A)),((_tmpB2A->name=_tmp144->name,((
_tmpB2A->tq=Cyc_Toc_mt_tq,((_tmpB2A->type=t,((_tmpB2A->width=_tmp144->width,((
_tmpB2A->attributes=_tmp144->attributes,_tmpB2A)))))))))))),((_tmpB29->tl=
_tmp13F,_tmpB29))))));};}_tmp13F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp13F);{struct Cyc_Absyn_AggrdeclImpl*_tmpB2F;struct _tuple0*
_tmpB2E;struct Cyc_Absyn_Aggrdecl*_tmpB2D;struct Cyc_Absyn_Aggrdecl*_tmp147=(
_tmpB2D=_cycalloc(sizeof(*_tmpB2D)),((_tmpB2D->kind=Cyc_Absyn_StructA,((_tmpB2D->sc=
Cyc_Absyn_Public,((_tmpB2D->name=((_tmpB2E=_cycalloc(sizeof(*_tmpB2E)),((_tmpB2E->f1=
Cyc_Absyn_Rel_n(0),((_tmpB2E->f2=xname,_tmpB2E)))))),((_tmpB2D->tvs=0,((_tmpB2D->impl=((
_tmpB2F=_cycalloc(sizeof(*_tmpB2F)),((_tmpB2F->exist_vars=0,((_tmpB2F->rgn_po=0,((
_tmpB2F->fields=_tmp13F,((_tmpB2F->tagged=0,_tmpB2F)))))))))),((_tmpB2D->attributes=
0,_tmpB2D)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpB35;struct Cyc_Absyn_Aggr_d_struct
_tmpB34;struct Cyc_List_List*_tmpB33;Cyc_Toc_result_decls=((_tmpB33=_cycalloc(
sizeof(*_tmpB33)),((_tmpB33->hd=Cyc_Absyn_new_decl((void*)((_tmpB35=_cycalloc(
sizeof(*_tmpB35)),((_tmpB35[0]=((_tmpB34.tag=6,((_tmpB34.f1=_tmp147,_tmpB34)))),
_tmpB35)))),0),((_tmpB33->tl=Cyc_Toc_result_decls,_tmpB33))))));}{void*_tmp14B=x;
_npop_handler(1);return _tmp14B;};};};_pop_region(r);};};;_pop_dynregion(d);};}
struct _tuple0*Cyc_Toc_temp_var(){int _tmp153=Cyc_Toc_temp_var_counter ++;struct
_dyneither_ptr*_tmpB42;const char*_tmpB41;void*_tmpB40[1];struct Cyc_Int_pa_struct
_tmpB3F;struct _tuple0*_tmpB3E;struct _tuple0*res=(_tmpB3E=_cycalloc(sizeof(*
_tmpB3E)),((_tmpB3E->f1=Cyc_Absyn_Loc_n,((_tmpB3E->f2=((_tmpB42=_cycalloc(
sizeof(*_tmpB42)),((_tmpB42[0]=(struct _dyneither_ptr)((_tmpB3F.tag=1,((_tmpB3F.f1=(
unsigned int)_tmp153,((_tmpB40[0]=& _tmpB3F,Cyc_aprintf(((_tmpB41="_tmp%X",
_tag_dyneither(_tmpB41,sizeof(char),7))),_tag_dyneither(_tmpB40,sizeof(void*),1)))))))),
_tmpB42)))),_tmpB3E)))));return res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp15A;struct Cyc_Xarray_Xarray*_tmp15B;struct Cyc_Toc_TocState
_tmp159=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp15A=
_tmp159.dyn;_tmp15B=_tmp159.temp_labels;{struct _DynRegionFrame _tmp15C;struct
_RegionHandle*d=_open_dynregion(& _tmp15C,_tmp15A);{int _tmp15D=Cyc_Toc_fresh_label_counter
++;if(_tmp15D < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp15B)){
struct _dyneither_ptr*_tmp15E=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp15B,_tmp15D);_npop_handler(0);return _tmp15E;}{struct Cyc_Int_pa_struct
_tmpB4A;void*_tmpB49[1];const char*_tmpB48;struct _dyneither_ptr*_tmpB47;struct
_dyneither_ptr*res=(_tmpB47=_cycalloc(sizeof(*_tmpB47)),((_tmpB47[0]=(struct
_dyneither_ptr)((_tmpB4A.tag=1,((_tmpB4A.f1=(unsigned int)_tmp15D,((_tmpB49[0]=&
_tmpB4A,Cyc_aprintf(((_tmpB48="_LL%X",_tag_dyneither(_tmpB48,sizeof(char),6))),
_tag_dyneither(_tmpB49,sizeof(void*),1)))))))),_tmpB47)));if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp15B,res)!= _tmp15D){const char*
_tmpB4D;void*_tmpB4C;(_tmpB4C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4D="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmpB4D,sizeof(char),43))),_tag_dyneither(_tmpB4C,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp161=res;_npop_handler(0);return _tmp161;};};};
_pop_dynregion(d);};}static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*
td,struct _tuple0*name){int ans=0;struct Cyc_List_List*_tmp166=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((
struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp166))->hd)->name)
!= 0){++ ans;_tmp166=_tmp166->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);static void Cyc_Toc_aggrdecl_to_c(
struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);static void Cyc_Toc_datatypedecl_to_c(
struct Cyc_Absyn_Datatypedecl*tud);static struct _tuple8*Cyc_Toc_arg_to_c(struct
_tuple8*a){struct Cyc_Core_Opt*_tmp168;struct Cyc_Absyn_Tqual _tmp169;void*_tmp16A;
struct _tuple8 _tmp167=*a;_tmp168=_tmp167.f1;_tmp169=_tmp167.f2;_tmp16A=_tmp167.f3;{
struct _tuple8*_tmpB4E;return(_tmpB4E=_cycalloc(sizeof(*_tmpB4E)),((_tmpB4E->f1=
_tmp168,((_tmpB4E->f2=_tmp169,((_tmpB4E->f3=Cyc_Toc_typ_to_c(_tmp16A),_tmpB4E)))))));};}
static struct _tuple10*Cyc_Toc_typ_to_c_f(struct _tuple10*x){struct Cyc_Absyn_Tqual
_tmp16D;void*_tmp16E;struct _tuple10 _tmp16C=*x;_tmp16D=_tmp16C.f1;_tmp16E=_tmp16C.f2;{
struct _tuple10*_tmpB4F;return(_tmpB4F=_cycalloc(sizeof(*_tmpB4F)),((_tmpB4F->f1=
_tmp16D,((_tmpB4F->f2=Cyc_Toc_typ_to_c(_tmp16E),_tmpB4F)))));};}static void*Cyc_Toc_typ_to_c_array(
void*t){void*_tmp170=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp172;
void*_tmp173;struct Cyc_Absyn_Tqual _tmp174;struct Cyc_Absyn_Exp*_tmp175;union Cyc_Absyn_Constraint*
_tmp176;struct Cyc_Position_Segment*_tmp177;struct Cyc_Core_Opt*_tmp179;struct Cyc_Core_Opt
_tmp17A;void*_tmp17B;_LL38: {struct Cyc_Absyn_ArrayType_struct*_tmp171=(struct Cyc_Absyn_ArrayType_struct*)
_tmp170;if(_tmp171->tag != 8)goto _LL3A;else{_tmp172=_tmp171->f1;_tmp173=_tmp172.elt_type;
_tmp174=_tmp172.tq;_tmp175=_tmp172.num_elts;_tmp176=_tmp172.zero_term;_tmp177=
_tmp172.zt_loc;}}_LL39: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp173),
_tmp174,_tmp175,Cyc_Absyn_false_conref,_tmp177);_LL3A: {struct Cyc_Absyn_Evar_struct*
_tmp178=(struct Cyc_Absyn_Evar_struct*)_tmp170;if(_tmp178->tag != 1)goto _LL3C;
else{_tmp179=_tmp178->f2;if(_tmp179 == 0)goto _LL3C;_tmp17A=*_tmp179;_tmp17B=(void*)
_tmp17A.v;}}_LL3B: return Cyc_Toc_typ_to_c_array(_tmp17B);_LL3C:;_LL3D: return Cyc_Toc_typ_to_c(
t);_LL37:;}static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*
f){struct Cyc_Absyn_Aggrfield*_tmpB50;return(_tmpB50=_cycalloc(sizeof(*_tmpB50)),((
_tmpB50->name=f->name,((_tmpB50->tq=Cyc_Toc_mt_tq,((_tmpB50->type=Cyc_Toc_typ_to_c(
f->type),((_tmpB50->width=f->width,((_tmpB50->attributes=f->attributes,_tmpB50)))))))))));}
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){return;}static void*Cyc_Toc_char_star_typ(){
static void**cs=0;if(cs == 0){void**_tmpB51;cs=((_tmpB51=_cycalloc(sizeof(*_tmpB51)),((
_tmpB51[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpB51))));}return*cs;}static void*Cyc_Toc_rgn_typ(){
static void**r=0;if(r == 0){void**_tmpB52;r=((_tmpB52=_cycalloc(sizeof(*_tmpB52)),((
_tmpB52[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),
_tmpB52))));}return*r;}static void*Cyc_Toc_dyn_rgn_typ(){static void**r=0;if(r == 0){
void**_tmpB53;r=((_tmpB53=_cycalloc(sizeof(*_tmpB53)),((_tmpB53[0]=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpB53))));}return*
r;}static int Cyc_Toc_is_boxed_tvar(void*t){void*_tmp180=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_Tvar*_tmp182;_LL3F: {struct Cyc_Absyn_VarType_struct*_tmp181=(
struct Cyc_Absyn_VarType_struct*)_tmp180;if(_tmp181->tag != 2)goto _LL41;else{
_tmp182=_tmp181->f1;}}_LL40: return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),&
Cyc_Tcutil_tbk);_LL41:;_LL42: return 0;_LL3E:;}static int Cyc_Toc_is_abstract_type(
void*t){struct Cyc_Absyn_Kind*_tmp183=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind
_tmp184;enum Cyc_Absyn_KindQual _tmp185;_LL44: _tmp184=*_tmp183;_tmp185=_tmp184.kind;
if(_tmp185 != Cyc_Absyn_AnyKind)goto _LL46;_LL45: return 1;_LL46:;_LL47: return 0;
_LL43:;}static void*Cyc_Toc_typ_to_c(void*t){void*_tmp186=t;struct Cyc_Core_Opt*
_tmp189;struct Cyc_Core_Opt*_tmp18B;struct Cyc_Core_Opt _tmp18C;void*_tmp18D;struct
Cyc_Absyn_Tvar*_tmp18F;struct Cyc_Absyn_DatatypeFieldInfo _tmp192;union Cyc_Absyn_DatatypeFieldInfoU
_tmp193;struct _tuple1 _tmp194;struct Cyc_Absyn_Datatypedecl*_tmp195;struct Cyc_Absyn_Datatypefield*
_tmp196;struct Cyc_Absyn_PtrInfo _tmp199;void*_tmp19A;struct Cyc_Absyn_Tqual _tmp19B;
struct Cyc_Absyn_PtrAtts _tmp19C;union Cyc_Absyn_Constraint*_tmp19D;struct Cyc_Absyn_ArrayInfo
_tmp1A1;void*_tmp1A2;struct Cyc_Absyn_Tqual _tmp1A3;struct Cyc_Absyn_Exp*_tmp1A4;
struct Cyc_Position_Segment*_tmp1A5;struct Cyc_Absyn_FnInfo _tmp1A7;struct Cyc_Absyn_Tqual
_tmp1A8;void*_tmp1A9;struct Cyc_List_List*_tmp1AA;int _tmp1AB;struct Cyc_Absyn_VarargInfo*
_tmp1AC;struct Cyc_List_List*_tmp1AD;struct Cyc_List_List*_tmp1AF;enum Cyc_Absyn_AggrKind
_tmp1B1;struct Cyc_List_List*_tmp1B2;struct Cyc_Absyn_AggrInfo _tmp1B4;union Cyc_Absyn_AggrInfoU
_tmp1B5;struct Cyc_List_List*_tmp1B6;struct _tuple0*_tmp1B8;struct Cyc_List_List*
_tmp1BA;struct _tuple0*_tmp1BC;struct Cyc_List_List*_tmp1BD;struct Cyc_Absyn_Typedefdecl*
_tmp1BE;void**_tmp1BF;void*_tmp1C2;struct Cyc_Absyn_TypeDecl*_tmp1CC;struct Cyc_Absyn_TypeDecl
_tmp1CD;void*_tmp1CE;struct Cyc_Absyn_Aggrdecl*_tmp1D0;struct Cyc_Absyn_TypeDecl*
_tmp1D2;struct Cyc_Absyn_TypeDecl _tmp1D3;void*_tmp1D4;struct Cyc_Absyn_Enumdecl*
_tmp1D6;struct Cyc_Absyn_TypeDecl*_tmp1D8;struct Cyc_Absyn_TypeDecl _tmp1D9;void*
_tmp1DA;struct Cyc_Absyn_Datatypedecl*_tmp1DC;void**_tmp1DD;_LL49: {struct Cyc_Absyn_VoidType_struct*
_tmp187=(struct Cyc_Absyn_VoidType_struct*)_tmp186;if(_tmp187->tag != 0)goto _LL4B;}
_LL4A: return t;_LL4B: {struct Cyc_Absyn_Evar_struct*_tmp188=(struct Cyc_Absyn_Evar_struct*)
_tmp186;if(_tmp188->tag != 1)goto _LL4D;else{_tmp189=_tmp188->f2;if(_tmp189 != 0)
goto _LL4D;}}_LL4C: return Cyc_Absyn_sint_typ;_LL4D: {struct Cyc_Absyn_Evar_struct*
_tmp18A=(struct Cyc_Absyn_Evar_struct*)_tmp186;if(_tmp18A->tag != 1)goto _LL4F;
else{_tmp18B=_tmp18A->f2;if(_tmp18B == 0)goto _LL4F;_tmp18C=*_tmp18B;_tmp18D=(void*)
_tmp18C.v;}}_LL4E: return Cyc_Toc_typ_to_c(_tmp18D);_LL4F: {struct Cyc_Absyn_VarType_struct*
_tmp18E=(struct Cyc_Absyn_VarType_struct*)_tmp186;if(_tmp18E->tag != 2)goto _LL51;
else{_tmp18F=_tmp18E->f1;}}_LL50: if((Cyc_Tcutil_tvar_kind(_tmp18F,& Cyc_Tcutil_bk))->kind
== Cyc_Absyn_AnyKind)return(void*)& Cyc_Absyn_VoidType_val;else{return Cyc_Absyn_void_star_typ();}
_LL51: {struct Cyc_Absyn_DatatypeType_struct*_tmp190=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp186;if(_tmp190->tag != 3)goto _LL53;}_LL52: return(void*)& Cyc_Absyn_VoidType_val;
_LL53: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp191=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp186;if(_tmp191->tag != 4)goto _LL55;else{_tmp192=_tmp191->f1;_tmp193=_tmp192.field_info;
if((_tmp193.KnownDatatypefield).tag != 2)goto _LL55;_tmp194=(struct _tuple1)(
_tmp193.KnownDatatypefield).val;_tmp195=_tmp194.f1;_tmp196=_tmp194.f2;}}_LL54: {
const char*_tmpB54;return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp196->name,((
_tmpB54="_struct",_tag_dyneither(_tmpB54,sizeof(char),8)))));}_LL55: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp197=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp186;if(_tmp197->tag != 4)
goto _LL57;}_LL56: {const char*_tmpB57;void*_tmpB56;(_tmpB56=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB57="unresolved DatatypeFieldType",
_tag_dyneither(_tmpB57,sizeof(char),29))),_tag_dyneither(_tmpB56,sizeof(void*),0)));}
_LL57: {struct Cyc_Absyn_PointerType_struct*_tmp198=(struct Cyc_Absyn_PointerType_struct*)
_tmp186;if(_tmp198->tag != 5)goto _LL59;else{_tmp199=_tmp198->f1;_tmp19A=_tmp199.elt_typ;
_tmp19B=_tmp199.elt_tq;_tmp19C=_tmp199.ptr_atts;_tmp19D=_tmp19C.bounds;}}_LL58:
_tmp19A=Cyc_Toc_typ_to_c_array(_tmp19A);{void*_tmp1E1=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp19D);_LL88: {
struct Cyc_Absyn_DynEither_b_struct*_tmp1E2=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp1E1;if(_tmp1E2->tag != 0)goto _LL8A;}_LL89: return Cyc_Toc_dyneither_ptr_typ;
_LL8A:;_LL8B: return Cyc_Absyn_star_typ(_tmp19A,(void*)& Cyc_Absyn_HeapRgn_val,
_tmp19B,Cyc_Absyn_false_conref);_LL87:;};_LL59: {struct Cyc_Absyn_IntType_struct*
_tmp19E=(struct Cyc_Absyn_IntType_struct*)_tmp186;if(_tmp19E->tag != 6)goto _LL5B;}
_LL5A: goto _LL5C;_LL5B: {struct Cyc_Absyn_FloatType_struct*_tmp19F=(struct Cyc_Absyn_FloatType_struct*)
_tmp186;if(_tmp19F->tag != 7)goto _LL5D;}_LL5C: return t;_LL5D: {struct Cyc_Absyn_ArrayType_struct*
_tmp1A0=(struct Cyc_Absyn_ArrayType_struct*)_tmp186;if(_tmp1A0->tag != 8)goto _LL5F;
else{_tmp1A1=_tmp1A0->f1;_tmp1A2=_tmp1A1.elt_type;_tmp1A3=_tmp1A1.tq;_tmp1A4=
_tmp1A1.num_elts;_tmp1A5=_tmp1A1.zt_loc;}}_LL5E: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmp1A2),_tmp1A3,_tmp1A4,Cyc_Absyn_false_conref,_tmp1A5);_LL5F: {struct Cyc_Absyn_FnType_struct*
_tmp1A6=(struct Cyc_Absyn_FnType_struct*)_tmp186;if(_tmp1A6->tag != 9)goto _LL61;
else{_tmp1A7=_tmp1A6->f1;_tmp1A8=_tmp1A7.ret_tqual;_tmp1A9=_tmp1A7.ret_typ;
_tmp1AA=_tmp1A7.args;_tmp1AB=_tmp1A7.c_varargs;_tmp1AC=_tmp1A7.cyc_varargs;
_tmp1AD=_tmp1A7.attributes;}}_LL60: {struct Cyc_List_List*_tmp1E3=0;for(0;_tmp1AD
!= 0;_tmp1AD=_tmp1AD->tl){void*_tmp1E4=(void*)_tmp1AD->hd;_LL8D: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp1E5=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp1E4;if(_tmp1E5->tag != 4)goto
_LL8F;}_LL8E: goto _LL90;_LL8F: {struct Cyc_Absyn_Const_att_struct*_tmp1E6=(struct
Cyc_Absyn_Const_att_struct*)_tmp1E4;if(_tmp1E6->tag != 5)goto _LL91;}_LL90: goto
_LL92;_LL91: {struct Cyc_Absyn_Format_att_struct*_tmp1E7=(struct Cyc_Absyn_Format_att_struct*)
_tmp1E4;if(_tmp1E7->tag != 19)goto _LL93;}_LL92: continue;_LL93: {struct Cyc_Absyn_Noliveunique_att_struct*
_tmp1E8=(struct Cyc_Absyn_Noliveunique_att_struct*)_tmp1E4;if(_tmp1E8->tag != 21)
goto _LL95;}_LL94: continue;_LL95: {struct Cyc_Absyn_Initializes_att_struct*_tmp1E9=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp1E4;if(_tmp1E9->tag != 20)goto _LL97;}
_LL96: continue;_LL97:;_LL98:{struct Cyc_List_List*_tmpB58;_tmp1E3=((_tmpB58=
_cycalloc(sizeof(*_tmpB58)),((_tmpB58->hd=(void*)_tmp1AD->hd,((_tmpB58->tl=
_tmp1E3,_tmpB58))))));}goto _LL8C;_LL8C:;}{struct Cyc_List_List*_tmp1EB=((struct
Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_arg_to_c,_tmp1AA);if(_tmp1AC != 0){void*_tmp1EC=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp1AC->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple8*_tmpB59;struct _tuple8*_tmp1ED=(_tmpB59=_cycalloc(sizeof(*_tmpB59)),((
_tmpB59->f1=_tmp1AC->name,((_tmpB59->f2=_tmp1AC->tq,((_tmpB59->f3=_tmp1EC,
_tmpB59)))))));struct Cyc_List_List*_tmpB5A;_tmp1EB=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1EB,((
_tmpB5A=_cycalloc(sizeof(*_tmpB5A)),((_tmpB5A->hd=_tmp1ED,((_tmpB5A->tl=0,
_tmpB5A)))))));}{struct Cyc_Absyn_FnType_struct _tmpB60;struct Cyc_Absyn_FnInfo
_tmpB5F;struct Cyc_Absyn_FnType_struct*_tmpB5E;return(void*)((_tmpB5E=_cycalloc(
sizeof(*_tmpB5E)),((_tmpB5E[0]=((_tmpB60.tag=9,((_tmpB60.f1=((_tmpB5F.tvars=0,((
_tmpB5F.effect=0,((_tmpB5F.ret_tqual=_tmp1A8,((_tmpB5F.ret_typ=Cyc_Toc_typ_to_c(
_tmp1A9),((_tmpB5F.args=_tmp1EB,((_tmpB5F.c_varargs=_tmp1AB,((_tmpB5F.cyc_varargs=
0,((_tmpB5F.rgn_po=0,((_tmpB5F.attributes=_tmp1E3,_tmpB5F)))))))))))))))))),
_tmpB60)))),_tmpB5E))));};};}_LL61: {struct Cyc_Absyn_TupleType_struct*_tmp1AE=(
struct Cyc_Absyn_TupleType_struct*)_tmp186;if(_tmp1AE->tag != 10)goto _LL63;else{
_tmp1AF=_tmp1AE->f1;}}_LL62: _tmp1AF=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(
struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1AF);
return Cyc_Toc_add_tuple_type(_tmp1AF);_LL63: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp1B0=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp186;if(_tmp1B0->tag != 12)goto
_LL65;else{_tmp1B1=_tmp1B0->f1;_tmp1B2=_tmp1B0->f2;}}_LL64: {struct Cyc_Absyn_AnonAggrType_struct
_tmpB63;struct Cyc_Absyn_AnonAggrType_struct*_tmpB62;return(void*)((_tmpB62=
_cycalloc(sizeof(*_tmpB62)),((_tmpB62[0]=((_tmpB63.tag=12,((_tmpB63.f1=_tmp1B1,((
_tmpB63.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1B2),_tmpB63)))))),
_tmpB62))));}_LL65: {struct Cyc_Absyn_AggrType_struct*_tmp1B3=(struct Cyc_Absyn_AggrType_struct*)
_tmp186;if(_tmp1B3->tag != 11)goto _LL67;else{_tmp1B4=_tmp1B3->f1;_tmp1B5=_tmp1B4.aggr_info;
_tmp1B6=_tmp1B4.targs;}}_LL66:{union Cyc_Absyn_AggrInfoU _tmp1F5=_tmp1B5;struct
_tuple2 _tmp1F6;_LL9A: if((_tmp1F5.UnknownAggr).tag != 1)goto _LL9C;_tmp1F6=(struct
_tuple2)(_tmp1F5.UnknownAggr).val;_LL9B: return t;_LL9C:;_LL9D: goto _LL99;_LL99:;}{
struct Cyc_Absyn_Aggrdecl*_tmp1F7=Cyc_Absyn_get_known_aggrdecl(_tmp1B5);if(
_tmp1F7->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(_tmp1F7->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp1F8=_tmp1F7->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1F7->impl))->fields;if(_tmp1F8 == 0)return Cyc_Toc_aggrdecl_type(
_tmp1F7->name,Cyc_Absyn_strctq);for(0;_tmp1F8->tl != 0;_tmp1F8=_tmp1F8->tl){;}{
void*_tmp1F9=((struct Cyc_Absyn_Aggrfield*)_tmp1F8->hd)->type;if(Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp1F9))){struct _RegionHandle _tmp1FA=
_new_region("r");struct _RegionHandle*r=& _tmp1FA;_push_region(r);{void*_tmp1FB=
Cyc_Tcutil_rsubstitute(r,((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,
_tmp1F7->tvs,_tmp1B6),_tmp1F9);if(Cyc_Toc_is_abstract_type(_tmp1FB)){void*
_tmp1FC=Cyc_Toc_aggrdecl_type(_tmp1F7->name,Cyc_Absyn_strctq);_npop_handler(0);
return _tmp1FC;}{void*_tmp1FD=Cyc_Toc_add_struct_type(_tmp1F7->name,_tmp1F7->tvs,
_tmp1B6,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1F7->impl))->fields);
_npop_handler(0);return _tmp1FD;};};_pop_region(r);}return Cyc_Toc_aggrdecl_type(
_tmp1F7->name,Cyc_Absyn_strctq);};};};_LL67: {struct Cyc_Absyn_EnumType_struct*
_tmp1B7=(struct Cyc_Absyn_EnumType_struct*)_tmp186;if(_tmp1B7->tag != 13)goto _LL69;
else{_tmp1B8=_tmp1B7->f1;}}_LL68: return t;_LL69: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp1B9=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp186;if(_tmp1B9->tag != 14)goto
_LL6B;else{_tmp1BA=_tmp1B9->f1;}}_LL6A: Cyc_Toc_enumfields_to_c(_tmp1BA);return t;
_LL6B: {struct Cyc_Absyn_TypedefType_struct*_tmp1BB=(struct Cyc_Absyn_TypedefType_struct*)
_tmp186;if(_tmp1BB->tag != 17)goto _LL6D;else{_tmp1BC=_tmp1BB->f1;_tmp1BD=_tmp1BB->f2;
_tmp1BE=_tmp1BB->f3;_tmp1BF=_tmp1BB->f4;}}_LL6C: if(_tmp1BF == 0  || Cyc_noexpand_r){
if(_tmp1BD != 0){struct Cyc_Absyn_TypedefType_struct _tmpB66;struct Cyc_Absyn_TypedefType_struct*
_tmpB65;return(void*)((_tmpB65=_cycalloc(sizeof(*_tmpB65)),((_tmpB65[0]=((
_tmpB66.tag=17,((_tmpB66.f1=_tmp1BC,((_tmpB66.f2=0,((_tmpB66.f3=_tmp1BE,((
_tmpB66.f4=0,_tmpB66)))))))))),_tmpB65))));}else{return t;}}else{struct Cyc_Absyn_TypedefType_struct
_tmpB6C;void**_tmpB6B;struct Cyc_Absyn_TypedefType_struct*_tmpB6A;return(void*)((
_tmpB6A=_cycalloc(sizeof(*_tmpB6A)),((_tmpB6A[0]=((_tmpB6C.tag=17,((_tmpB6C.f1=
_tmp1BC,((_tmpB6C.f2=0,((_tmpB6C.f3=_tmp1BE,((_tmpB6C.f4=((_tmpB6B=_cycalloc(
sizeof(*_tmpB6B)),((_tmpB6B[0]=Cyc_Toc_typ_to_c_array(*_tmp1BF),_tmpB6B)))),
_tmpB6C)))))))))),_tmpB6A))));}_LL6D: {struct Cyc_Absyn_TagType_struct*_tmp1C0=(
struct Cyc_Absyn_TagType_struct*)_tmp186;if(_tmp1C0->tag != 19)goto _LL6F;}_LL6E:
return Cyc_Absyn_uint_typ;_LL6F: {struct Cyc_Absyn_RgnHandleType_struct*_tmp1C1=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp186;if(_tmp1C1->tag != 15)goto _LL71;
else{_tmp1C2=(void*)_tmp1C1->f1;}}_LL70: return Cyc_Toc_rgn_typ();_LL71: {struct
Cyc_Absyn_DynRgnType_struct*_tmp1C3=(struct Cyc_Absyn_DynRgnType_struct*)_tmp186;
if(_tmp1C3->tag != 16)goto _LL73;}_LL72: return Cyc_Toc_dyn_rgn_typ();_LL73: {struct
Cyc_Absyn_HeapRgn_struct*_tmp1C4=(struct Cyc_Absyn_HeapRgn_struct*)_tmp186;if(
_tmp1C4->tag != 20)goto _LL75;}_LL74: {const char*_tmpB6F;void*_tmpB6E;(_tmpB6E=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB6F="Toc::typ_to_c: type translation passed the heap region",_tag_dyneither(
_tmpB6F,sizeof(char),55))),_tag_dyneither(_tmpB6E,sizeof(void*),0)));}_LL75: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp1C5=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp186;if(_tmp1C5->tag != 21)goto _LL77;}_LL76: {const char*_tmpB72;void*_tmpB71;(
_tmpB71=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB72="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpB72,sizeof(char),57))),_tag_dyneither(_tmpB71,sizeof(void*),0)));}
_LL77: {struct Cyc_Absyn_RefCntRgn_struct*_tmp1C6=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp186;if(_tmp1C6->tag != 22)goto _LL79;}_LL78: {const char*_tmpB75;void*_tmpB74;(
_tmpB74=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB75="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpB75,sizeof(char),57))),_tag_dyneither(_tmpB74,sizeof(void*),0)));}
_LL79: {struct Cyc_Absyn_AccessEff_struct*_tmp1C7=(struct Cyc_Absyn_AccessEff_struct*)
_tmp186;if(_tmp1C7->tag != 23)goto _LL7B;}_LL7A: goto _LL7C;_LL7B: {struct Cyc_Absyn_JoinEff_struct*
_tmp1C8=(struct Cyc_Absyn_JoinEff_struct*)_tmp186;if(_tmp1C8->tag != 24)goto _LL7D;}
_LL7C: goto _LL7E;_LL7D: {struct Cyc_Absyn_RgnsEff_struct*_tmp1C9=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp186;if(_tmp1C9->tag != 25)goto _LL7F;}_LL7E: {const char*_tmpB78;void*_tmpB77;(
_tmpB77=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB78="Toc::typ_to_c: type translation passed an effect",_tag_dyneither(
_tmpB78,sizeof(char),49))),_tag_dyneither(_tmpB77,sizeof(void*),0)));}_LL7F: {
struct Cyc_Absyn_ValueofType_struct*_tmp1CA=(struct Cyc_Absyn_ValueofType_struct*)
_tmp186;if(_tmp1CA->tag != 18)goto _LL81;}_LL80: {const char*_tmpB7B;void*_tmpB7A;(
_tmpB7A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB7B="Toc::typ_to_c: type translation passed a valueof_t",_tag_dyneither(
_tmpB7B,sizeof(char),51))),_tag_dyneither(_tmpB7A,sizeof(void*),0)));}_LL81: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp1CB=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp186;if(_tmp1CB->tag != 26)goto _LL83;else{_tmp1CC=_tmp1CB->f1;_tmp1CD=*_tmp1CC;
_tmp1CE=_tmp1CD.r;{struct Cyc_Absyn_Aggr_td_struct*_tmp1CF=(struct Cyc_Absyn_Aggr_td_struct*)
_tmp1CE;if(_tmp1CF->tag != 0)goto _LL83;else{_tmp1D0=_tmp1CF->f1;}};}}_LL82: Cyc_Toc_aggrdecl_to_c(
_tmp1D0,1);if(_tmp1D0->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(
_tmp1D0->name,Cyc_Absyn_unionq_typ);else{return Cyc_Toc_aggrdecl_type(_tmp1D0->name,
Cyc_Absyn_strctq);}_LL83: {struct Cyc_Absyn_TypeDeclType_struct*_tmp1D1=(struct
Cyc_Absyn_TypeDeclType_struct*)_tmp186;if(_tmp1D1->tag != 26)goto _LL85;else{
_tmp1D2=_tmp1D1->f1;_tmp1D3=*_tmp1D2;_tmp1D4=_tmp1D3.r;{struct Cyc_Absyn_Enum_td_struct*
_tmp1D5=(struct Cyc_Absyn_Enum_td_struct*)_tmp1D4;if(_tmp1D5->tag != 1)goto _LL85;
else{_tmp1D6=_tmp1D5->f1;}};}}_LL84: Cyc_Toc_enumdecl_to_c(_tmp1D6);return t;_LL85: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp1D7=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp186;if(_tmp1D7->tag != 26)goto _LL48;else{_tmp1D8=_tmp1D7->f1;_tmp1D9=*_tmp1D8;
_tmp1DA=_tmp1D9.r;{struct Cyc_Absyn_Datatype_td_struct*_tmp1DB=(struct Cyc_Absyn_Datatype_td_struct*)
_tmp1DA;if(_tmp1DB->tag != 2)goto _LL48;else{_tmp1DC=_tmp1DB->f1;}};_tmp1DD=
_tmp1D7->f2;}}_LL86: Cyc_Toc_datatypedecl_to_c(_tmp1DC);return Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp1DD)));_LL48:;}static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(
void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmp20D=t;struct
Cyc_Absyn_ArrayInfo _tmp20F;void*_tmp210;struct Cyc_Absyn_Tqual _tmp211;_LL9F: {
struct Cyc_Absyn_ArrayType_struct*_tmp20E=(struct Cyc_Absyn_ArrayType_struct*)
_tmp20D;if(_tmp20E->tag != 8)goto _LLA1;else{_tmp20F=_tmp20E->f1;_tmp210=_tmp20F.elt_type;
_tmp211=_tmp20F.tq;}}_LLA0: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp210,(
void*)& Cyc_Absyn_HeapRgn_val,_tmp211,Cyc_Absyn_false_conref),e);_LLA1:;_LLA2:
return Cyc_Toc_cast_it(t,e);_LL9E:;}static int Cyc_Toc_atomic_typ(void*t){void*
_tmp212=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp21C;void*_tmp21D;
struct Cyc_Absyn_AggrInfo _tmp21F;union Cyc_Absyn_AggrInfoU _tmp220;struct Cyc_List_List*
_tmp222;struct Cyc_Absyn_DatatypeFieldInfo _tmp224;union Cyc_Absyn_DatatypeFieldInfoU
_tmp225;struct _tuple1 _tmp226;struct Cyc_Absyn_Datatypedecl*_tmp227;struct Cyc_Absyn_Datatypefield*
_tmp228;struct Cyc_List_List*_tmp22A;_LLA4: {struct Cyc_Absyn_VoidType_struct*
_tmp213=(struct Cyc_Absyn_VoidType_struct*)_tmp212;if(_tmp213->tag != 0)goto _LLA6;}
_LLA5: return 1;_LLA6: {struct Cyc_Absyn_VarType_struct*_tmp214=(struct Cyc_Absyn_VarType_struct*)
_tmp212;if(_tmp214->tag != 2)goto _LLA8;}_LLA7: return 0;_LLA8: {struct Cyc_Absyn_IntType_struct*
_tmp215=(struct Cyc_Absyn_IntType_struct*)_tmp212;if(_tmp215->tag != 6)goto _LLAA;}
_LLA9: goto _LLAB;_LLAA: {struct Cyc_Absyn_EnumType_struct*_tmp216=(struct Cyc_Absyn_EnumType_struct*)
_tmp212;if(_tmp216->tag != 13)goto _LLAC;}_LLAB: goto _LLAD;_LLAC: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp217=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp212;if(_tmp217->tag != 14)goto
_LLAE;}_LLAD: goto _LLAF;_LLAE: {struct Cyc_Absyn_FloatType_struct*_tmp218=(struct
Cyc_Absyn_FloatType_struct*)_tmp212;if(_tmp218->tag != 7)goto _LLB0;}_LLAF: goto
_LLB1;_LLB0: {struct Cyc_Absyn_FnType_struct*_tmp219=(struct Cyc_Absyn_FnType_struct*)
_tmp212;if(_tmp219->tag != 9)goto _LLB2;}_LLB1: goto _LLB3;_LLB2: {struct Cyc_Absyn_TagType_struct*
_tmp21A=(struct Cyc_Absyn_TagType_struct*)_tmp212;if(_tmp21A->tag != 19)goto _LLB4;}
_LLB3: return 1;_LLB4: {struct Cyc_Absyn_ArrayType_struct*_tmp21B=(struct Cyc_Absyn_ArrayType_struct*)
_tmp212;if(_tmp21B->tag != 8)goto _LLB6;else{_tmp21C=_tmp21B->f1;_tmp21D=_tmp21C.elt_type;}}
_LLB5: return Cyc_Toc_atomic_typ(_tmp21D);_LLB6: {struct Cyc_Absyn_AggrType_struct*
_tmp21E=(struct Cyc_Absyn_AggrType_struct*)_tmp212;if(_tmp21E->tag != 11)goto _LLB8;
else{_tmp21F=_tmp21E->f1;_tmp220=_tmp21F.aggr_info;}}_LLB7:{union Cyc_Absyn_AggrInfoU
_tmp22F=_tmp220;struct _tuple2 _tmp230;_LLC9: if((_tmp22F.UnknownAggr).tag != 1)goto
_LLCB;_tmp230=(struct _tuple2)(_tmp22F.UnknownAggr).val;_LLCA: return 0;_LLCB:;
_LLCC: goto _LLC8;_LLC8:;}{struct Cyc_Absyn_Aggrdecl*_tmp231=Cyc_Absyn_get_known_aggrdecl(
_tmp220);if(_tmp231->impl == 0)return 0;{struct Cyc_List_List*_tmp232=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp231->impl))->fields;for(0;_tmp232 != 0;_tmp232=_tmp232->tl){if(!
Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp232->hd)->type))return 0;}}
return 1;};_LLB8: {struct Cyc_Absyn_AnonAggrType_struct*_tmp221=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp212;if(_tmp221->tag != 12)goto _LLBA;else{_tmp222=_tmp221->f2;}}_LLB9: for(0;
_tmp222 != 0;_tmp222=_tmp222->tl){if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)
_tmp222->hd)->type))return 0;}return 1;_LLBA: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp223=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp212;if(_tmp223->tag != 4)
goto _LLBC;else{_tmp224=_tmp223->f1;_tmp225=_tmp224.field_info;if((_tmp225.KnownDatatypefield).tag
!= 2)goto _LLBC;_tmp226=(struct _tuple1)(_tmp225.KnownDatatypefield).val;_tmp227=
_tmp226.f1;_tmp228=_tmp226.f2;}}_LLBB: _tmp22A=_tmp228->typs;goto _LLBD;_LLBC: {
struct Cyc_Absyn_TupleType_struct*_tmp229=(struct Cyc_Absyn_TupleType_struct*)
_tmp212;if(_tmp229->tag != 10)goto _LLBE;else{_tmp22A=_tmp229->f1;}}_LLBD: for(0;
_tmp22A != 0;_tmp22A=_tmp22A->tl){if(!Cyc_Toc_atomic_typ((*((struct _tuple10*)
_tmp22A->hd)).f2))return 0;}return 1;_LLBE: {struct Cyc_Absyn_DatatypeType_struct*
_tmp22B=(struct Cyc_Absyn_DatatypeType_struct*)_tmp212;if(_tmp22B->tag != 3)goto
_LLC0;}_LLBF: goto _LLC1;_LLC0: {struct Cyc_Absyn_PointerType_struct*_tmp22C=(
struct Cyc_Absyn_PointerType_struct*)_tmp212;if(_tmp22C->tag != 5)goto _LLC2;}_LLC1:
goto _LLC3;_LLC2: {struct Cyc_Absyn_DynRgnType_struct*_tmp22D=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp212;if(_tmp22D->tag != 16)goto _LLC4;}_LLC3: goto _LLC5;_LLC4: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp22E=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp212;if(_tmp22E->tag != 15)goto
_LLC6;}_LLC5: return 0;_LLC6:;_LLC7: {const char*_tmpB7F;void*_tmpB7E[1];struct Cyc_String_pa_struct
_tmpB7D;(_tmpB7D.tag=0,((_tmpB7D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpB7E[0]=& _tmpB7D,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB7F="atomic_typ:  bad type %s",
_tag_dyneither(_tmpB7F,sizeof(char),25))),_tag_dyneither(_tmpB7E,sizeof(void*),1)))))));}
_LLA3:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp236=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp238;void*_tmp239;_LLCE: {struct Cyc_Absyn_PointerType_struct*
_tmp237=(struct Cyc_Absyn_PointerType_struct*)_tmp236;if(_tmp237->tag != 5)goto
_LLD0;else{_tmp238=_tmp237->f1;_tmp239=_tmp238.elt_typ;}}_LLCF: {void*_tmp23A=
Cyc_Tcutil_compress(_tmp239);_LLD3: {struct Cyc_Absyn_VoidType_struct*_tmp23B=(
struct Cyc_Absyn_VoidType_struct*)_tmp23A;if(_tmp23B->tag != 0)goto _LLD5;}_LLD4:
return 1;_LLD5:;_LLD6: return 0;_LLD2:;}_LLD0:;_LLD1: return 0;_LLCD:;}static int Cyc_Toc_is_void_star_or_boxed_tvar(
void*t){return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}static int Cyc_Toc_is_poly_field(
void*t,struct _dyneither_ptr*f){void*_tmp23C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp23E;union Cyc_Absyn_AggrInfoU _tmp23F;struct Cyc_List_List*_tmp241;_LLD8: {
struct Cyc_Absyn_AggrType_struct*_tmp23D=(struct Cyc_Absyn_AggrType_struct*)
_tmp23C;if(_tmp23D->tag != 11)goto _LLDA;else{_tmp23E=_tmp23D->f1;_tmp23F=_tmp23E.aggr_info;}}
_LLD9: {struct Cyc_Absyn_Aggrdecl*_tmp242=Cyc_Absyn_get_known_aggrdecl(_tmp23F);
if(_tmp242->impl == 0){const char*_tmpB82;void*_tmpB81;(_tmpB81=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB82="is_poly_field: type missing fields",
_tag_dyneither(_tmpB82,sizeof(char),35))),_tag_dyneither(_tmpB81,sizeof(void*),0)));}
_tmp241=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp242->impl))->fields;goto
_LLDB;}_LLDA: {struct Cyc_Absyn_AnonAggrType_struct*_tmp240=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp23C;if(_tmp240->tag != 12)goto _LLDC;else{_tmp241=_tmp240->f2;}}_LLDB: {struct
Cyc_Absyn_Aggrfield*_tmp245=Cyc_Absyn_lookup_field(_tmp241,f);if(_tmp245 == 0){
const char*_tmpB86;void*_tmpB85[1];struct Cyc_String_pa_struct _tmpB84;(_tmpB84.tag=
0,((_tmpB84.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB85[0]=&
_tmpB84,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB86="is_poly_field: bad field %s",_tag_dyneither(_tmpB86,sizeof(char),28))),
_tag_dyneither(_tmpB85,sizeof(void*),1)))))));}return Cyc_Toc_is_void_star_or_boxed_tvar(
_tmp245->type);}_LLDC:;_LLDD: {const char*_tmpB8A;void*_tmpB89[1];struct Cyc_String_pa_struct
_tmpB88;(_tmpB88.tag=0,((_tmpB88.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpB89[0]=& _tmpB88,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8A="is_poly_field: bad type %s",
_tag_dyneither(_tmpB8A,sizeof(char),27))),_tag_dyneither(_tmpB89,sizeof(void*),1)))))));}
_LLD7:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmp24C=e->r;
struct Cyc_Absyn_Exp*_tmp24E;struct _dyneither_ptr*_tmp24F;struct Cyc_Absyn_Exp*
_tmp251;struct _dyneither_ptr*_tmp252;_LLDF: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp24D=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp24C;if(_tmp24D->tag != 22)goto
_LLE1;else{_tmp24E=_tmp24D->f1;_tmp24F=_tmp24D->f2;}}_LLE0: return Cyc_Toc_is_poly_field((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp24E->topt))->v,_tmp24F);_LLE1: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp250=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp24C;if(_tmp250->tag != 23)goto _LLE3;else{_tmp251=_tmp250->f1;_tmp252=_tmp250->f2;}}
_LLE2: {void*_tmp253=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp251->topt))->v);struct Cyc_Absyn_PtrInfo _tmp255;void*_tmp256;
_LLE6: {struct Cyc_Absyn_PointerType_struct*_tmp254=(struct Cyc_Absyn_PointerType_struct*)
_tmp253;if(_tmp254->tag != 5)goto _LLE8;else{_tmp255=_tmp254->f1;_tmp256=_tmp255.elt_typ;}}
_LLE7: return Cyc_Toc_is_poly_field(_tmp256,_tmp252);_LLE8:;_LLE9: {const char*
_tmpB8E;void*_tmpB8D[1];struct Cyc_String_pa_struct _tmpB8C;(_tmpB8C.tag=0,((
_tmpB8C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp251->topt))->v)),((_tmpB8D[0]=&
_tmpB8C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB8E="is_poly_project: bad type %s",_tag_dyneither(_tmpB8E,sizeof(char),29))),
_tag_dyneither(_tmpB8D,sizeof(void*),1)))))));}_LLE5:;}_LLE3:;_LLE4: return 0;
_LLDE:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB8F;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((
_tmpB8F=_cycalloc(sizeof(*_tmpB8F)),((_tmpB8F->hd=s,((_tmpB8F->tl=0,_tmpB8F)))))),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB90;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((
_tmpB90=_cycalloc(sizeof(*_tmpB90)),((_tmpB90->hd=s,((_tmpB90->tl=0,_tmpB90)))))),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(
s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmpB91[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((
_tmpB91[1]=s,((_tmpB91[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpB91,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,
struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*
_tmpB92[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpB92[1]=n,((
_tmpB92[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB92,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{struct Cyc_Absyn_Exp*
_tmpB93[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpB93[1]=n,((
_tmpB93[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB93,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n){struct Cyc_Absyn_Exp*_tmpB94[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((
_tmpB94[2]=n,((_tmpB94[1]=s,((_tmpB94[0]=rgn,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB94,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmpB95;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmpB95=_cycalloc(sizeof(*_tmpB95)),((_tmpB95->hd=e,((_tmpB95->tl=0,_tmpB95)))))),
0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){struct Cyc_Absyn_Exp*
_tmpB96[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpB96,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*
_tmp262=e->r;union Cyc_Absyn_Cnst _tmp264;struct _dyneither_ptr _tmp265;union Cyc_Absyn_Cnst
_tmp267;struct _dyneither_ptr _tmp268;_LLEB: {struct Cyc_Absyn_Const_e_struct*
_tmp263=(struct Cyc_Absyn_Const_e_struct*)_tmp262;if(_tmp263->tag != 0)goto _LLED;
else{_tmp264=_tmp263->f1;if((_tmp264.String_c).tag != 8)goto _LLED;_tmp265=(struct
_dyneither_ptr)(_tmp264.String_c).val;}}_LLEC: is_string=1;goto _LLEA;_LLED: {
struct Cyc_Absyn_Const_e_struct*_tmp266=(struct Cyc_Absyn_Const_e_struct*)_tmp262;
if(_tmp266->tag != 0)goto _LLEF;else{_tmp267=_tmp266->f1;if((_tmp267.Wstring_c).tag
!= 9)goto _LLEF;_tmp268=(struct _dyneither_ptr)(_tmp267.Wstring_c).val;}}_LLEE:
is_string=1;goto _LLEA;_LLEF:;_LLF0: goto _LLEA;_LLEA:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple0*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_struct*_tmpB9C;struct
Cyc_Absyn_Var_d_struct _tmpB9B;struct Cyc_List_List*_tmpB9A;Cyc_Toc_result_decls=((
_tmpB9A=_cycalloc(sizeof(*_tmpB9A)),((_tmpB9A->hd=Cyc_Absyn_new_decl((void*)((
_tmpB9C=_cycalloc(sizeof(*_tmpB9C)),((_tmpB9C[0]=((_tmpB9B.tag=0,((_tmpB9B.f1=vd,
_tmpB9B)))),_tmpB9C)))),0),((_tmpB9A->tl=Cyc_Toc_result_decls,_tmpB9A))))));}
xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*urm_exp;{struct _tuple16*_tmpBA3;
struct _tuple16*_tmpBA2;struct _tuple16*_tmpBA1;struct _tuple16*_tmpBA0[3];urm_exp=
Cyc_Absyn_unresolvedmem_exp(0,((_tmpBA0[2]=((_tmpBA1=_cycalloc(sizeof(*_tmpBA1)),((
_tmpBA1->f1=0,((_tmpBA1->f2=xplussz,_tmpBA1)))))),((_tmpBA0[1]=((_tmpBA2=
_cycalloc(sizeof(*_tmpBA2)),((_tmpBA2->f1=0,((_tmpBA2->f2=xexp,_tmpBA2)))))),((
_tmpBA0[0]=((_tmpBA3=_cycalloc(sizeof(*_tmpBA3)),((_tmpBA3->f1=0,((_tmpBA3->f2=
xexp,_tmpBA3)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBA0,sizeof(struct _tuple16*),3)))))))),0);}return urm_exp;};};}
struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*
binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct
_dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct
_RegionHandle*rgn;};typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;static int Cyc_Toc_is_toplevel(
struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp271;int _tmp272;struct Cyc_Toc_Env*
_tmp270=nv;_tmp271=*_tmp270;_tmp272=_tmp271.toplevel;return _tmp272;}static int Cyc_Toc_in_lhs(
struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp274;int*_tmp275;struct Cyc_Toc_Env*
_tmp273=nv;_tmp274=*_tmp273;_tmp275=_tmp274.in_lhs;return*_tmp275;}static struct
Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple0*x){struct
Cyc_Toc_Env _tmp277;struct Cyc_Dict_Dict _tmp278;struct Cyc_Toc_Env*_tmp276=nv;
_tmp277=*_tmp276;_tmp278=_tmp277.varmap;return((struct Cyc_Absyn_Exp*(*)(struct
Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp278,x);}static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(struct _RegionHandle*r){int*_tmpBA6;struct Cyc_Toc_Env*_tmpBA5;
return(_tmpBA5=_region_malloc(r,sizeof(*_tmpBA5)),((_tmpBA5->break_lab=(struct
_dyneither_ptr**)0,((_tmpBA5->continue_lab=(struct _dyneither_ptr**)0,((_tmpBA5->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpBA5->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp),((_tmpBA5->toplevel=(int)1,((_tmpBA5->in_lhs=(int*)((
_tmpBA6=_region_malloc(r,sizeof(*_tmpBA6)),((_tmpBA6[0]=0,_tmpBA6)))),((_tmpBA5->rgn=(
struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpBA5)))))))))))))));}static struct
Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct
Cyc_Toc_Env _tmp27C;struct _dyneither_ptr**_tmp27D;struct _dyneither_ptr**_tmp27E;
struct Cyc_Toc_FallthruInfo*_tmp27F;struct Cyc_Dict_Dict _tmp280;int _tmp281;int*
_tmp282;struct Cyc_Toc_Env*_tmp27B=e;_tmp27C=*_tmp27B;_tmp27D=_tmp27C.break_lab;
_tmp27E=_tmp27C.continue_lab;_tmp27F=_tmp27C.fallthru_info;_tmp280=_tmp27C.varmap;
_tmp281=_tmp27C.toplevel;_tmp282=_tmp27C.in_lhs;{struct Cyc_Toc_Env*_tmpBA7;
return(_tmpBA7=_region_malloc(r,sizeof(*_tmpBA7)),((_tmpBA7->break_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp27D),((_tmpBA7->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp27E),((_tmpBA7->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp27F,((_tmpBA7->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp280),((_tmpBA7->toplevel=(int)_tmp281,((_tmpBA7->in_lhs=(int*)_tmp282,((
_tmpBA7->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpBA7)))))))))))))));};}
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp285;struct _dyneither_ptr**_tmp286;struct _dyneither_ptr**
_tmp287;struct Cyc_Toc_FallthruInfo*_tmp288;struct Cyc_Dict_Dict _tmp289;int _tmp28A;
int*_tmp28B;struct Cyc_Toc_Env*_tmp284=e;_tmp285=*_tmp284;_tmp286=_tmp285.break_lab;
_tmp287=_tmp285.continue_lab;_tmp288=_tmp285.fallthru_info;_tmp289=_tmp285.varmap;
_tmp28A=_tmp285.toplevel;_tmp28B=_tmp285.in_lhs;{struct Cyc_Toc_Env*_tmpBA8;
return(_tmpBA8=_region_malloc(r,sizeof(*_tmpBA8)),((_tmpBA8->break_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp286),((_tmpBA8->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp287),((_tmpBA8->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp288,((_tmpBA8->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp289),((_tmpBA8->toplevel=(int)0,((_tmpBA8->in_lhs=(int*)_tmp28B,((_tmpBA8->rgn=(
struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpBA8)))))))))))))));};}static
struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env _tmp28E;struct _dyneither_ptr**_tmp28F;struct _dyneither_ptr**
_tmp290;struct Cyc_Toc_FallthruInfo*_tmp291;struct Cyc_Dict_Dict _tmp292;int _tmp293;
int*_tmp294;struct Cyc_Toc_Env*_tmp28D=e;_tmp28E=*_tmp28D;_tmp28F=_tmp28E.break_lab;
_tmp290=_tmp28E.continue_lab;_tmp291=_tmp28E.fallthru_info;_tmp292=_tmp28E.varmap;
_tmp293=_tmp28E.toplevel;_tmp294=_tmp28E.in_lhs;{struct Cyc_Toc_Env*_tmpBA9;
return(_tmpBA9=_region_malloc(r,sizeof(*_tmpBA9)),((_tmpBA9->break_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp28F),((_tmpBA9->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp290),((_tmpBA9->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp291,((_tmpBA9->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp292),((_tmpBA9->toplevel=(int)1,((_tmpBA9->in_lhs=(int*)_tmp294,((_tmpBA9->rgn=(
struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpBA9)))))))))))))));};}static void
Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){struct Cyc_Toc_Env _tmp297;int*_tmp298;
struct Cyc_Toc_Env*_tmp296=e;_tmp297=*_tmp296;_tmp298=_tmp297.in_lhs;*_tmp298=b;}
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){{union Cyc_Absyn_Nmspace _tmp299=(*x).f1;
struct Cyc_List_List*_tmp29A;_LLF2: if((_tmp299.Rel_n).tag != 1)goto _LLF4;_tmp29A=(
struct Cyc_List_List*)(_tmp299.Rel_n).val;_LLF3: {const char*_tmpBAD;void*_tmpBAC[
1];struct Cyc_String_pa_struct _tmpBAB;(_tmpBAB.tag=0,((_tmpBAB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpBAC[0]=&
_tmpBAB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBAD="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpBAD,sizeof(char),30))),
_tag_dyneither(_tmpBAC,sizeof(void*),1)))))));}_LLF4:;_LLF5: goto _LLF1;_LLF1:;}{
struct Cyc_Toc_Env _tmp29F;struct _dyneither_ptr**_tmp2A0;struct _dyneither_ptr**
_tmp2A1;struct Cyc_Toc_FallthruInfo*_tmp2A2;struct Cyc_Dict_Dict _tmp2A3;int _tmp2A4;
int*_tmp2A5;struct Cyc_Toc_Env*_tmp29E=e;_tmp29F=*_tmp29E;_tmp2A0=_tmp29F.break_lab;
_tmp2A1=_tmp29F.continue_lab;_tmp2A2=_tmp29F.fallthru_info;_tmp2A3=_tmp29F.varmap;
_tmp2A4=_tmp29F.toplevel;_tmp2A5=_tmp29F.in_lhs;{struct Cyc_Dict_Dict _tmp2A6=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*
v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp2A3),x,y);struct Cyc_Toc_Env*_tmpBAE;return(_tmpBAE=
_region_malloc(r,sizeof(*_tmpBAE)),((_tmpBAE->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp2A0),((_tmpBAE->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp2A1),((_tmpBAE->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp2A2,((_tmpBAE->varmap=(struct Cyc_Dict_Dict)_tmp2A6,((_tmpBAE->toplevel=(int)
_tmp2A4,((_tmpBAE->in_lhs=(int*)_tmp2A5,((_tmpBAE->rgn=(struct _RegionHandle*)r,(
struct Cyc_Toc_Env*)_tmpBAE)))))))))))))));};};}static struct Cyc_Toc_Env*Cyc_Toc_loop_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp2A9;struct
_dyneither_ptr**_tmp2AA;struct _dyneither_ptr**_tmp2AB;struct Cyc_Toc_FallthruInfo*
_tmp2AC;struct Cyc_Dict_Dict _tmp2AD;int _tmp2AE;int*_tmp2AF;struct Cyc_Toc_Env*
_tmp2A8=e;_tmp2A9=*_tmp2A8;_tmp2AA=_tmp2A9.break_lab;_tmp2AB=_tmp2A9.continue_lab;
_tmp2AC=_tmp2A9.fallthru_info;_tmp2AD=_tmp2A9.varmap;_tmp2AE=_tmp2A9.toplevel;
_tmp2AF=_tmp2A9.in_lhs;{struct Cyc_Toc_Env*_tmpBAF;return(_tmpBAF=_region_malloc(
r,sizeof(*_tmpBAF)),((_tmpBAF->break_lab=(struct _dyneither_ptr**)0,((_tmpBAF->continue_lab=(
struct _dyneither_ptr**)0,((_tmpBAF->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp2AC,((_tmpBAF->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2AD),((_tmpBAF->toplevel=(
int)_tmp2AE,((_tmpBAF->in_lhs=(int*)_tmp2AF,((_tmpBAF->rgn=(struct _RegionHandle*)
r,(struct Cyc_Toc_Env*)_tmpBAF)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmpBB0;fallthru_vars=((
_tmpBB0=_region_malloc(r,sizeof(*_tmpBB0)),((_tmpBB0->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmpBB0->tl=fallthru_vars,_tmpBB0))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp2B3;struct _dyneither_ptr**_tmp2B4;struct
_dyneither_ptr**_tmp2B5;struct Cyc_Toc_FallthruInfo*_tmp2B6;struct Cyc_Dict_Dict
_tmp2B7;int _tmp2B8;int*_tmp2B9;struct Cyc_Toc_Env*_tmp2B2=e;_tmp2B3=*_tmp2B2;
_tmp2B4=_tmp2B3.break_lab;_tmp2B5=_tmp2B3.continue_lab;_tmp2B6=_tmp2B3.fallthru_info;
_tmp2B7=_tmp2B3.varmap;_tmp2B8=_tmp2B3.toplevel;_tmp2B9=_tmp2B3.in_lhs;{struct
Cyc_Toc_Env _tmp2BB;struct Cyc_Dict_Dict _tmp2BC;struct Cyc_Toc_Env*_tmp2BA=
next_case_env;_tmp2BB=*_tmp2BA;_tmp2BC=_tmp2BB.varmap;{struct Cyc_Toc_FallthruInfo*
_tmpBB1;struct Cyc_Toc_FallthruInfo*fi=(_tmpBB1=_region_malloc(r,sizeof(*_tmpBB1)),((
_tmpBB1->label=fallthru_l,((_tmpBB1->binders=fallthru_vars,((_tmpBB1->next_case_env=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp2BC),_tmpBB1)))))));struct _dyneither_ptr**_tmpBB4;struct Cyc_Toc_Env*
_tmpBB3;return(_tmpBB3=_region_malloc(r,sizeof(*_tmpBB3)),((_tmpBB3->break_lab=(
struct _dyneither_ptr**)((_tmpBB4=_region_malloc(r,sizeof(*_tmpBB4)),((_tmpBB4[0]=
break_l,_tmpBB4)))),((_tmpBB3->continue_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp2B5),((_tmpBB3->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
fi,((_tmpBB3->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2B7),((_tmpBB3->toplevel=(
int)_tmp2B8,((_tmpBB3->in_lhs=(int*)((int*)_tmp2B9),((_tmpBB3->rgn=(struct
_RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpBB3)))))))))))))));};};};}static struct
Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _dyneither_ptr*break_l){struct Cyc_Toc_Env _tmp2C1;struct _dyneither_ptr**
_tmp2C2;struct _dyneither_ptr**_tmp2C3;struct Cyc_Toc_FallthruInfo*_tmp2C4;struct
Cyc_Dict_Dict _tmp2C5;int _tmp2C6;int*_tmp2C7;struct Cyc_Toc_Env*_tmp2C0=e;_tmp2C1=*
_tmp2C0;_tmp2C2=_tmp2C1.break_lab;_tmp2C3=_tmp2C1.continue_lab;_tmp2C4=_tmp2C1.fallthru_info;
_tmp2C5=_tmp2C1.varmap;_tmp2C6=_tmp2C1.toplevel;_tmp2C7=_tmp2C1.in_lhs;{struct
_dyneither_ptr**_tmpBB7;struct Cyc_Toc_Env*_tmpBB6;return(_tmpBB6=_region_malloc(
r,sizeof(*_tmpBB6)),((_tmpBB6->break_lab=(struct _dyneither_ptr**)((_tmpBB7=
_region_malloc(r,sizeof(*_tmpBB7)),((_tmpBB7[0]=break_l,_tmpBB7)))),((_tmpBB6->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp2C3),((_tmpBB6->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpBB6->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2C5),((_tmpBB6->toplevel=(
int)_tmp2C6,((_tmpBB6->in_lhs=(int*)_tmp2C7,((_tmpBB6->rgn=(struct _RegionHandle*)
r,(struct Cyc_Toc_Env*)_tmpBB6)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp2CB;struct _dyneither_ptr**_tmp2CC;struct _dyneither_ptr**_tmp2CD;struct Cyc_Toc_FallthruInfo*
_tmp2CE;struct Cyc_Dict_Dict _tmp2CF;int _tmp2D0;int*_tmp2D1;struct Cyc_Toc_Env*
_tmp2CA=e;_tmp2CB=*_tmp2CA;_tmp2CC=_tmp2CB.break_lab;_tmp2CD=_tmp2CB.continue_lab;
_tmp2CE=_tmp2CB.fallthru_info;_tmp2CF=_tmp2CB.varmap;_tmp2D0=_tmp2CB.toplevel;
_tmp2D1=_tmp2CB.in_lhs;{struct Cyc_Toc_FallthruInfo*_tmpBBA;struct Cyc_Toc_Env*
_tmpBB9;return(_tmpBB9=_region_malloc(r,sizeof(*_tmpBB9)),((_tmpBB9->break_lab=(
struct _dyneither_ptr**)0,((_tmpBB9->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp2CD),((_tmpBB9->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((
_tmpBBA=_region_malloc(r,sizeof(*_tmpBBA)),((_tmpBBA->label=next_l,((_tmpBBA->binders=
0,((_tmpBBA->next_case_env=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*
cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),
_tmpBBA)))))))),((_tmpBB9->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2CF),((_tmpBB9->toplevel=(
int)_tmp2D0,((_tmpBB9->in_lhs=(int*)((int*)_tmp2D1),((_tmpBB9->rgn=(struct
_RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpBB9)))))))))))))));};}static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct
Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);static int Cyc_Toc_need_null_check(struct
Cyc_Absyn_Exp*e){void*_tmp2D4=e->annot;_LLF7: {struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp2D5=(struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp2D4;if(_tmp2D5->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLF9;}_LLF8: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLF9: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp2D6=(
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp2D4;if(_tmp2D6->tag != Cyc_CfFlowInfo_NotZero)
goto _LLFB;}_LLFA: return 0;_LLFB: {struct Cyc_CfFlowInfo_IsZero_struct*_tmp2D7=(
struct Cyc_CfFlowInfo_IsZero_struct*)_tmp2D4;if(_tmp2D7->tag != Cyc_CfFlowInfo_IsZero)
goto _LLFD;}_LLFC:{const char*_tmpBBD;void*_tmpBBC;(_tmpBBC=0,Cyc_Tcutil_terr(e->loc,((
_tmpBBD="dereference of NULL pointer",_tag_dyneither(_tmpBBD,sizeof(char),28))),
_tag_dyneither(_tmpBBC,sizeof(void*),0)));}return 0;_LLFD: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp2D8=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp2D4;if(_tmp2D8->tag != Cyc_Absyn_EmptyAnnot)
goto _LLFF;}_LLFE: return 0;_LLFF:;_LL100: {const char*_tmpBC0;void*_tmpBBF;(_tmpBBF=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBC0="need_null_check",_tag_dyneither(_tmpBC0,sizeof(char),16))),
_tag_dyneither(_tmpBBF,sizeof(void*),0)));}_LLF6:;}static struct Cyc_List_List*Cyc_Toc_get_relns(
struct Cyc_Absyn_Exp*e){void*_tmp2DD=e->annot;struct Cyc_List_List*_tmp2DF;struct
Cyc_List_List*_tmp2E1;_LL102: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2DE=(
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp2DD;if(_tmp2DE->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LL104;else{_tmp2DF=_tmp2DE->f1;}}_LL103: return _tmp2DF;_LL104: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmp2E0=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmp2DD;if(_tmp2E0->tag != Cyc_CfFlowInfo_NotZero)
goto _LL106;else{_tmp2E1=_tmp2E0->f1;}}_LL105: return _tmp2E1;_LL106: {struct Cyc_CfFlowInfo_IsZero_struct*
_tmp2E2=(struct Cyc_CfFlowInfo_IsZero_struct*)_tmp2DD;if(_tmp2E2->tag != Cyc_CfFlowInfo_IsZero)
goto _LL108;}_LL107:{const char*_tmpBC3;void*_tmpBC2;(_tmpBC2=0,Cyc_Tcutil_terr(e->loc,((
_tmpBC3="dereference of NULL pointer",_tag_dyneither(_tmpBC3,sizeof(char),28))),
_tag_dyneither(_tmpBC2,sizeof(void*),0)));}return 0;_LL108: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp2E3=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp2DD;if(_tmp2E3->tag != Cyc_Absyn_EmptyAnnot)
goto _LL10A;}_LL109: return 0;_LL10A:;_LL10B: {const char*_tmpBC6;void*_tmpBC5;(
_tmpBC5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBC6="get_relns",_tag_dyneither(_tmpBC6,sizeof(char),10))),_tag_dyneither(
_tmpBC5,sizeof(void*),0)));}_LL101:;}static int Cyc_Toc_check_const_array(
unsigned int i,void*t){void*_tmp2E8=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp2EA;struct Cyc_Absyn_PtrAtts _tmp2EB;union Cyc_Absyn_Constraint*_tmp2EC;union
Cyc_Absyn_Constraint*_tmp2ED;struct Cyc_Absyn_ArrayInfo _tmp2EF;struct Cyc_Absyn_Exp*
_tmp2F0;_LL10D: {struct Cyc_Absyn_PointerType_struct*_tmp2E9=(struct Cyc_Absyn_PointerType_struct*)
_tmp2E8;if(_tmp2E9->tag != 5)goto _LL10F;else{_tmp2EA=_tmp2E9->f1;_tmp2EB=_tmp2EA.ptr_atts;
_tmp2EC=_tmp2EB.bounds;_tmp2ED=_tmp2EB.zero_term;}}_LL10E: {void*_tmp2F1=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp2EC);struct Cyc_Absyn_Exp*_tmp2F4;_LL114: {struct Cyc_Absyn_DynEither_b_struct*
_tmp2F2=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2F1;if(_tmp2F2->tag != 0)goto
_LL116;}_LL115: return 0;_LL116: {struct Cyc_Absyn_Upper_b_struct*_tmp2F3=(struct
Cyc_Absyn_Upper_b_struct*)_tmp2F1;if(_tmp2F3->tag != 1)goto _LL113;else{_tmp2F4=
_tmp2F3->f1;}}_LL117: {unsigned int _tmp2F6;int _tmp2F7;struct _tuple11 _tmp2F5=Cyc_Evexp_eval_const_uint_exp(
_tmp2F4);_tmp2F6=_tmp2F5.f1;_tmp2F7=_tmp2F5.f2;return _tmp2F7  && i <= _tmp2F6;}
_LL113:;}_LL10F: {struct Cyc_Absyn_ArrayType_struct*_tmp2EE=(struct Cyc_Absyn_ArrayType_struct*)
_tmp2E8;if(_tmp2EE->tag != 8)goto _LL111;else{_tmp2EF=_tmp2EE->f1;_tmp2F0=_tmp2EF.num_elts;}}
_LL110: if(_tmp2F0 == 0)return 0;{unsigned int _tmp2F9;int _tmp2FA;struct _tuple11
_tmp2F8=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp2F0);_tmp2F9=
_tmp2F8.f1;_tmp2FA=_tmp2F8.f2;return _tmp2FA  && i <= _tmp2F9;};_LL111:;_LL112:
return 0;_LL10C:;}static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2FB=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(
_tmp2FB->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp _tmp2FC=_tmp2FB->rop;struct
Cyc_Absyn_Vardecl*_tmp2FD;struct Cyc_Absyn_Vardecl*_tmp2FE;_LL119: if((_tmp2FC.LessNumelts).tag
!= 3)goto _LL11B;_tmp2FD=(struct Cyc_Absyn_Vardecl*)(_tmp2FC.LessNumelts).val;
_LL11A: _tmp2FE=_tmp2FD;goto _LL11C;_LL11B: if((_tmp2FC.LessEqNumelts).tag != 5)goto
_LL11D;_tmp2FE=(struct Cyc_Absyn_Vardecl*)(_tmp2FC.LessEqNumelts).val;_LL11C: if(
_tmp2FE == v)return 1;else{goto _LL118;}_LL11D:;_LL11E: continue;_LL118:;};}return 0;}
static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Exp*e,void*vtype){{void*_tmp2FF=e->r;void*_tmp301;struct Cyc_Absyn_Vardecl*
_tmp303;void*_tmp305;struct Cyc_Absyn_Vardecl*_tmp307;void*_tmp309;struct Cyc_Absyn_Vardecl*
_tmp30B;void*_tmp30D;struct Cyc_Absyn_Vardecl*_tmp30F;union Cyc_Absyn_Cnst _tmp311;
struct _tuple5 _tmp312;enum Cyc_Absyn_Sign _tmp313;int _tmp314;union Cyc_Absyn_Cnst
_tmp316;struct _tuple5 _tmp317;enum Cyc_Absyn_Sign _tmp318;int _tmp319;union Cyc_Absyn_Cnst
_tmp31B;struct _tuple5 _tmp31C;enum Cyc_Absyn_Sign _tmp31D;int _tmp31E;enum Cyc_Absyn_Primop
_tmp320;struct Cyc_List_List*_tmp321;struct Cyc_List_List _tmp322;struct Cyc_Absyn_Exp*
_tmp323;_LL120: {struct Cyc_Absyn_Var_e_struct*_tmp300=(struct Cyc_Absyn_Var_e_struct*)
_tmp2FF;if(_tmp300->tag != 1)goto _LL122;else{_tmp301=(void*)_tmp300->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp302=(struct Cyc_Absyn_Pat_b_struct*)_tmp301;if(_tmp302->tag
!= 5)goto _LL122;else{_tmp303=_tmp302->f1;}};}}_LL121: _tmp307=_tmp303;goto _LL123;
_LL122: {struct Cyc_Absyn_Var_e_struct*_tmp304=(struct Cyc_Absyn_Var_e_struct*)
_tmp2FF;if(_tmp304->tag != 1)goto _LL124;else{_tmp305=(void*)_tmp304->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp306=(struct Cyc_Absyn_Local_b_struct*)_tmp305;if(
_tmp306->tag != 4)goto _LL124;else{_tmp307=_tmp306->f1;}};}}_LL123: _tmp30B=_tmp307;
goto _LL125;_LL124: {struct Cyc_Absyn_Var_e_struct*_tmp308=(struct Cyc_Absyn_Var_e_struct*)
_tmp2FF;if(_tmp308->tag != 1)goto _LL126;else{_tmp309=(void*)_tmp308->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp30A=(struct Cyc_Absyn_Global_b_struct*)_tmp309;if(
_tmp30A->tag != 1)goto _LL126;else{_tmp30B=_tmp30A->f1;}};}}_LL125: _tmp30F=_tmp30B;
goto _LL127;_LL126: {struct Cyc_Absyn_Var_e_struct*_tmp30C=(struct Cyc_Absyn_Var_e_struct*)
_tmp2FF;if(_tmp30C->tag != 1)goto _LL128;else{_tmp30D=(void*)_tmp30C->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp30E=(struct Cyc_Absyn_Param_b_struct*)_tmp30D;if(
_tmp30E->tag != 3)goto _LL128;else{_tmp30F=_tmp30E->f1;}};}}_LL127: if(_tmp30F->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp30F))return 1;goto _LL11F;_LL128: {
struct Cyc_Absyn_Const_e_struct*_tmp310=(struct Cyc_Absyn_Const_e_struct*)_tmp2FF;
if(_tmp310->tag != 0)goto _LL12A;else{_tmp311=_tmp310->f1;if((_tmp311.Int_c).tag != 
5)goto _LL12A;_tmp312=(struct _tuple5)(_tmp311.Int_c).val;_tmp313=_tmp312.f1;if(
_tmp313 != Cyc_Absyn_None)goto _LL12A;_tmp314=_tmp312.f2;}}_LL129: _tmp319=_tmp314;
goto _LL12B;_LL12A: {struct Cyc_Absyn_Const_e_struct*_tmp315=(struct Cyc_Absyn_Const_e_struct*)
_tmp2FF;if(_tmp315->tag != 0)goto _LL12C;else{_tmp316=_tmp315->f1;if((_tmp316.Int_c).tag
!= 5)goto _LL12C;_tmp317=(struct _tuple5)(_tmp316.Int_c).val;_tmp318=_tmp317.f1;
if(_tmp318 != Cyc_Absyn_Signed)goto _LL12C;_tmp319=_tmp317.f2;}}_LL12B: return
_tmp319 >= 0  && Cyc_Toc_check_const_array((unsigned int)_tmp319,vtype);_LL12C: {
struct Cyc_Absyn_Const_e_struct*_tmp31A=(struct Cyc_Absyn_Const_e_struct*)_tmp2FF;
if(_tmp31A->tag != 0)goto _LL12E;else{_tmp31B=_tmp31A->f1;if((_tmp31B.Int_c).tag != 
5)goto _LL12E;_tmp31C=(struct _tuple5)(_tmp31B.Int_c).val;_tmp31D=_tmp31C.f1;if(
_tmp31D != Cyc_Absyn_Unsigned)goto _LL12E;_tmp31E=_tmp31C.f2;}}_LL12D: return Cyc_Toc_check_const_array((
unsigned int)_tmp31E,vtype);_LL12E: {struct Cyc_Absyn_Primop_e_struct*_tmp31F=(
struct Cyc_Absyn_Primop_e_struct*)_tmp2FF;if(_tmp31F->tag != 3)goto _LL130;else{
_tmp320=_tmp31F->f1;if(_tmp320 != Cyc_Absyn_Numelts)goto _LL130;_tmp321=_tmp31F->f2;
if(_tmp321 == 0)goto _LL130;_tmp322=*_tmp321;_tmp323=(struct Cyc_Absyn_Exp*)_tmp322.hd;}}
_LL12F:{void*_tmp324=_tmp323->r;void*_tmp326;struct Cyc_Absyn_Vardecl*_tmp328;
void*_tmp32A;struct Cyc_Absyn_Vardecl*_tmp32C;void*_tmp32E;struct Cyc_Absyn_Vardecl*
_tmp330;void*_tmp332;struct Cyc_Absyn_Vardecl*_tmp334;_LL133: {struct Cyc_Absyn_Var_e_struct*
_tmp325=(struct Cyc_Absyn_Var_e_struct*)_tmp324;if(_tmp325->tag != 1)goto _LL135;
else{_tmp326=(void*)_tmp325->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp327=(struct Cyc_Absyn_Pat_b_struct*)
_tmp326;if(_tmp327->tag != 5)goto _LL135;else{_tmp328=_tmp327->f1;}};}}_LL134:
_tmp32C=_tmp328;goto _LL136;_LL135: {struct Cyc_Absyn_Var_e_struct*_tmp329=(struct
Cyc_Absyn_Var_e_struct*)_tmp324;if(_tmp329->tag != 1)goto _LL137;else{_tmp32A=(
void*)_tmp329->f2;{struct Cyc_Absyn_Local_b_struct*_tmp32B=(struct Cyc_Absyn_Local_b_struct*)
_tmp32A;if(_tmp32B->tag != 4)goto _LL137;else{_tmp32C=_tmp32B->f1;}};}}_LL136:
_tmp330=_tmp32C;goto _LL138;_LL137: {struct Cyc_Absyn_Var_e_struct*_tmp32D=(struct
Cyc_Absyn_Var_e_struct*)_tmp324;if(_tmp32D->tag != 1)goto _LL139;else{_tmp32E=(
void*)_tmp32D->f2;{struct Cyc_Absyn_Global_b_struct*_tmp32F=(struct Cyc_Absyn_Global_b_struct*)
_tmp32E;if(_tmp32F->tag != 1)goto _LL139;else{_tmp330=_tmp32F->f1;}};}}_LL138:
_tmp334=_tmp330;goto _LL13A;_LL139: {struct Cyc_Absyn_Var_e_struct*_tmp331=(struct
Cyc_Absyn_Var_e_struct*)_tmp324;if(_tmp331->tag != 1)goto _LL13B;else{_tmp332=(
void*)_tmp331->f2;{struct Cyc_Absyn_Param_b_struct*_tmp333=(struct Cyc_Absyn_Param_b_struct*)
_tmp332;if(_tmp333->tag != 3)goto _LL13B;else{_tmp334=_tmp333->f1;}};}}_LL13A:
return _tmp334 == v;_LL13B:;_LL13C: goto _LL132;_LL132:;}goto _LL11F;_LL130:;_LL131:
goto _LL11F;_LL11F:;}return 0;}static char _tmp346[8]="*bogus*";struct _tuple19{void*
f1;void*f2;};static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,
struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){struct Cyc_Absyn_Vardecl*x;{void*
_tmp335=a->r;void*_tmp337;struct Cyc_Absyn_Vardecl*_tmp339;void*_tmp33B;struct Cyc_Absyn_Vardecl*
_tmp33D;void*_tmp33F;struct Cyc_Absyn_Vardecl*_tmp341;void*_tmp343;struct Cyc_Absyn_Vardecl*
_tmp345;_LL13E: {struct Cyc_Absyn_Var_e_struct*_tmp336=(struct Cyc_Absyn_Var_e_struct*)
_tmp335;if(_tmp336->tag != 1)goto _LL140;else{_tmp337=(void*)_tmp336->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp338=(struct Cyc_Absyn_Pat_b_struct*)_tmp337;if(_tmp338->tag
!= 5)goto _LL140;else{_tmp339=_tmp338->f1;}};}}_LL13F: _tmp33D=_tmp339;goto _LL141;
_LL140: {struct Cyc_Absyn_Var_e_struct*_tmp33A=(struct Cyc_Absyn_Var_e_struct*)
_tmp335;if(_tmp33A->tag != 1)goto _LL142;else{_tmp33B=(void*)_tmp33A->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp33C=(struct Cyc_Absyn_Local_b_struct*)_tmp33B;if(
_tmp33C->tag != 4)goto _LL142;else{_tmp33D=_tmp33C->f1;}};}}_LL141: _tmp341=_tmp33D;
goto _LL143;_LL142: {struct Cyc_Absyn_Var_e_struct*_tmp33E=(struct Cyc_Absyn_Var_e_struct*)
_tmp335;if(_tmp33E->tag != 1)goto _LL144;else{_tmp33F=(void*)_tmp33E->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp340=(struct Cyc_Absyn_Global_b_struct*)_tmp33F;if(
_tmp340->tag != 1)goto _LL144;else{_tmp341=_tmp340->f1;}};}}_LL143: _tmp345=_tmp341;
goto _LL145;_LL144: {struct Cyc_Absyn_Var_e_struct*_tmp342=(struct Cyc_Absyn_Var_e_struct*)
_tmp335;if(_tmp342->tag != 1)goto _LL146;else{_tmp343=(void*)_tmp342->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp344=(struct Cyc_Absyn_Param_b_struct*)_tmp343;if(
_tmp344->tag != 3)goto _LL146;else{_tmp345=_tmp344->f1;}};}}_LL145: if(_tmp345->escapes)
goto _LL147;x=_tmp345;goto _LL13D;_LL146:;_LL147: {static struct _dyneither_ptr
bogus_string={_tmp346,_tmp346,_tmp346 + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
4,0}},& bogus_string};static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,&
bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};x=& bogus_vardecl;
x->type=a_typ;}_LL13D:;}{void*_tmp347=a_typ;inner_loop: {void*_tmp348=i->r;void*
_tmp34A;struct Cyc_Absyn_Exp*_tmp34B;union Cyc_Absyn_Cnst _tmp34D;struct _tuple5
_tmp34E;enum Cyc_Absyn_Sign _tmp34F;int _tmp350;union Cyc_Absyn_Cnst _tmp352;struct
_tuple5 _tmp353;enum Cyc_Absyn_Sign _tmp354;int _tmp355;union Cyc_Absyn_Cnst _tmp357;
struct _tuple5 _tmp358;enum Cyc_Absyn_Sign _tmp359;int _tmp35A;enum Cyc_Absyn_Primop
_tmp35C;struct Cyc_List_List*_tmp35D;struct Cyc_List_List _tmp35E;struct Cyc_Absyn_Exp*
_tmp35F;struct Cyc_List_List*_tmp360;struct Cyc_List_List _tmp361;struct Cyc_Absyn_Exp*
_tmp362;void*_tmp364;struct Cyc_Absyn_Vardecl*_tmp366;void*_tmp368;struct Cyc_Absyn_Vardecl*
_tmp36A;void*_tmp36C;struct Cyc_Absyn_Vardecl*_tmp36E;void*_tmp370;struct Cyc_Absyn_Vardecl*
_tmp372;_LL149: {struct Cyc_Absyn_Cast_e_struct*_tmp349=(struct Cyc_Absyn_Cast_e_struct*)
_tmp348;if(_tmp349->tag != 15)goto _LL14B;else{_tmp34A=(void*)_tmp349->f1;_tmp34B=
_tmp349->f2;}}_LL14A: i=_tmp34B;goto inner_loop;_LL14B: {struct Cyc_Absyn_Const_e_struct*
_tmp34C=(struct Cyc_Absyn_Const_e_struct*)_tmp348;if(_tmp34C->tag != 0)goto _LL14D;
else{_tmp34D=_tmp34C->f1;if((_tmp34D.Int_c).tag != 5)goto _LL14D;_tmp34E=(struct
_tuple5)(_tmp34D.Int_c).val;_tmp34F=_tmp34E.f1;if(_tmp34F != Cyc_Absyn_None)goto
_LL14D;_tmp350=_tmp34E.f2;}}_LL14C: _tmp355=_tmp350;goto _LL14E;_LL14D: {struct Cyc_Absyn_Const_e_struct*
_tmp351=(struct Cyc_Absyn_Const_e_struct*)_tmp348;if(_tmp351->tag != 0)goto _LL14F;
else{_tmp352=_tmp351->f1;if((_tmp352.Int_c).tag != 5)goto _LL14F;_tmp353=(struct
_tuple5)(_tmp352.Int_c).val;_tmp354=_tmp353.f1;if(_tmp354 != Cyc_Absyn_Signed)
goto _LL14F;_tmp355=_tmp353.f2;}}_LL14E: return _tmp355 >= 0  && Cyc_Toc_check_const_array((
unsigned int)(_tmp355 + 1),_tmp347);_LL14F: {struct Cyc_Absyn_Const_e_struct*
_tmp356=(struct Cyc_Absyn_Const_e_struct*)_tmp348;if(_tmp356->tag != 0)goto _LL151;
else{_tmp357=_tmp356->f1;if((_tmp357.Int_c).tag != 5)goto _LL151;_tmp358=(struct
_tuple5)(_tmp357.Int_c).val;_tmp359=_tmp358.f1;if(_tmp359 != Cyc_Absyn_Unsigned)
goto _LL151;_tmp35A=_tmp358.f2;}}_LL150: return Cyc_Toc_check_const_array((
unsigned int)(_tmp35A + 1),_tmp347);_LL151: {struct Cyc_Absyn_Primop_e_struct*
_tmp35B=(struct Cyc_Absyn_Primop_e_struct*)_tmp348;if(_tmp35B->tag != 3)goto _LL153;
else{_tmp35C=_tmp35B->f1;if(_tmp35C != Cyc_Absyn_Mod)goto _LL153;_tmp35D=_tmp35B->f2;
if(_tmp35D == 0)goto _LL153;_tmp35E=*_tmp35D;_tmp35F=(struct Cyc_Absyn_Exp*)_tmp35E.hd;
_tmp360=_tmp35E.tl;if(_tmp360 == 0)goto _LL153;_tmp361=*_tmp360;_tmp362=(struct Cyc_Absyn_Exp*)
_tmp361.hd;}}_LL152: return Cyc_Toc_check_leq_size(relns,x,_tmp362,_tmp347);_LL153: {
struct Cyc_Absyn_Var_e_struct*_tmp363=(struct Cyc_Absyn_Var_e_struct*)_tmp348;if(
_tmp363->tag != 1)goto _LL155;else{_tmp364=(void*)_tmp363->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp365=(struct Cyc_Absyn_Pat_b_struct*)_tmp364;if(_tmp365->tag != 5)goto _LL155;
else{_tmp366=_tmp365->f1;}};}}_LL154: _tmp36A=_tmp366;goto _LL156;_LL155: {struct
Cyc_Absyn_Var_e_struct*_tmp367=(struct Cyc_Absyn_Var_e_struct*)_tmp348;if(_tmp367->tag
!= 1)goto _LL157;else{_tmp368=(void*)_tmp367->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp369=(struct Cyc_Absyn_Local_b_struct*)_tmp368;if(_tmp369->tag != 4)goto _LL157;
else{_tmp36A=_tmp369->f1;}};}}_LL156: _tmp36E=_tmp36A;goto _LL158;_LL157: {struct
Cyc_Absyn_Var_e_struct*_tmp36B=(struct Cyc_Absyn_Var_e_struct*)_tmp348;if(_tmp36B->tag
!= 1)goto _LL159;else{_tmp36C=(void*)_tmp36B->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp36D=(struct Cyc_Absyn_Global_b_struct*)_tmp36C;if(_tmp36D->tag != 1)goto _LL159;
else{_tmp36E=_tmp36D->f1;}};}}_LL158: _tmp372=_tmp36E;goto _LL15A;_LL159: {struct
Cyc_Absyn_Var_e_struct*_tmp36F=(struct Cyc_Absyn_Var_e_struct*)_tmp348;if(_tmp36F->tag
!= 1)goto _LL15B;else{_tmp370=(void*)_tmp36F->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp371=(struct Cyc_Absyn_Param_b_struct*)_tmp370;if(_tmp371->tag != 3)goto _LL15B;
else{_tmp372=_tmp371->f1;}};}}_LL15A: if(_tmp372->escapes)return 0;{struct Cyc_List_List*
_tmp373=relns;for(0;_tmp373 != 0;_tmp373=_tmp373->tl){struct Cyc_CfFlowInfo_Reln*
_tmp374=(struct Cyc_CfFlowInfo_Reln*)_tmp373->hd;if(_tmp374->vd == _tmp372){union
Cyc_CfFlowInfo_RelnOp _tmp375=_tmp374->rop;struct Cyc_Absyn_Vardecl*_tmp376;struct
_tuple12 _tmp377;struct Cyc_Absyn_Vardecl*_tmp378;void*_tmp379;unsigned int _tmp37A;
unsigned int _tmp37B;_LL15E: if((_tmp375.LessNumelts).tag != 3)goto _LL160;_tmp376=(
struct Cyc_Absyn_Vardecl*)(_tmp375.LessNumelts).val;_LL15F: if(x == _tmp376)return 1;
else{goto _LL15D;}_LL160: if((_tmp375.LessVar).tag != 2)goto _LL162;_tmp377=(struct
_tuple12)(_tmp375.LessVar).val;_tmp378=_tmp377.f1;_tmp379=_tmp377.f2;_LL161:{
struct _tuple19 _tmpBC7;struct _tuple19 _tmp37D=(_tmpBC7.f1=Cyc_Tcutil_compress(
_tmp379),((_tmpBC7.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(a->topt))->v),_tmpBC7)));void*_tmp37E;void*_tmp380;void*_tmp381;
struct Cyc_Absyn_PtrInfo _tmp383;struct Cyc_Absyn_PtrAtts _tmp384;union Cyc_Absyn_Constraint*
_tmp385;_LL169: _tmp37E=_tmp37D.f1;{struct Cyc_Absyn_TagType_struct*_tmp37F=(
struct Cyc_Absyn_TagType_struct*)_tmp37E;if(_tmp37F->tag != 19)goto _LL16B;else{
_tmp380=(void*)_tmp37F->f1;}};_tmp381=_tmp37D.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp382=(struct Cyc_Absyn_PointerType_struct*)_tmp381;if(_tmp382->tag != 5)goto
_LL16B;else{_tmp383=_tmp382->f1;_tmp384=_tmp383.ptr_atts;_tmp385=_tmp384.bounds;}};
_LL16A:{void*_tmp386=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp385);struct Cyc_Absyn_Exp*_tmp388;_LL16E: {struct Cyc_Absyn_Upper_b_struct*
_tmp387=(struct Cyc_Absyn_Upper_b_struct*)_tmp386;if(_tmp387->tag != 1)goto _LL170;
else{_tmp388=_tmp387->f1;}}_LL16F: {struct Cyc_Absyn_Exp*_tmp389=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp380,0),0,Cyc_Absyn_No_coercion,0);
if(Cyc_Evexp_lte_const_exp(_tmp389,_tmp388))return 1;goto _LL16D;}_LL170:;_LL171:
goto _LL16D;_LL16D:;}goto _LL168;_LL16B:;_LL16C: goto _LL168;_LL168:;}{struct Cyc_List_List*
_tmp38A=relns;for(0;_tmp38A != 0;_tmp38A=_tmp38A->tl){struct Cyc_CfFlowInfo_Reln*
_tmp38B=(struct Cyc_CfFlowInfo_Reln*)_tmp38A->hd;if(_tmp38B->vd == _tmp378){union
Cyc_CfFlowInfo_RelnOp _tmp38C=_tmp38B->rop;struct Cyc_Absyn_Vardecl*_tmp38D;struct
Cyc_Absyn_Vardecl*_tmp38E;unsigned int _tmp38F;unsigned int _tmp390;struct _tuple12
_tmp391;struct Cyc_Absyn_Vardecl*_tmp392;_LL173: if((_tmp38C.LessEqNumelts).tag != 
5)goto _LL175;_tmp38D=(struct Cyc_Absyn_Vardecl*)(_tmp38C.LessEqNumelts).val;
_LL174: _tmp38E=_tmp38D;goto _LL176;_LL175: if((_tmp38C.LessNumelts).tag != 3)goto
_LL177;_tmp38E=(struct Cyc_Absyn_Vardecl*)(_tmp38C.LessNumelts).val;_LL176: if(x == 
_tmp38E)return 1;goto _LL172;_LL177: if((_tmp38C.EqualConst).tag != 1)goto _LL179;
_tmp38F=(unsigned int)(_tmp38C.EqualConst).val;_LL178: return Cyc_Toc_check_const_array(
_tmp38F,_tmp347);_LL179: if((_tmp38C.LessEqConst).tag != 6)goto _LL17B;_tmp390=(
unsigned int)(_tmp38C.LessEqConst).val;if(!(_tmp390 > 0))goto _LL17B;_LL17A: return
Cyc_Toc_check_const_array(_tmp390,_tmp347);_LL17B: if((_tmp38C.LessVar).tag != 2)
goto _LL17D;_tmp391=(struct _tuple12)(_tmp38C.LessVar).val;_tmp392=_tmp391.f1;
_LL17C: if(Cyc_Toc_check_leq_size_var(relns,x,_tmp392))return 1;goto _LL172;_LL17D:;
_LL17E: goto _LL172;_LL172:;}}}goto _LL15D;_LL162: if((_tmp375.LessConst).tag != 4)
goto _LL164;_tmp37A=(unsigned int)(_tmp375.LessConst).val;_LL163: return Cyc_Toc_check_const_array(
_tmp37A,_tmp347);_LL164: if((_tmp375.LessEqConst).tag != 6)goto _LL166;_tmp37B=(
unsigned int)(_tmp375.LessEqConst).val;_LL165: return Cyc_Toc_check_const_array(
_tmp37B + 1,_tmp347);_LL166:;_LL167: goto _LL15D;_LL15D:;}}}goto _LL148;_LL15B:;
_LL15C: goto _LL148;_LL148:;}return 0;};}static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*
e){if(e->topt == 0){const char*_tmpBCA;void*_tmpBC9;(_tmpBC9=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBCA="Missing type in primop ",
_tag_dyneither(_tmpBCA,sizeof(char),24))),_tag_dyneither(_tmpBC9,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*
_tmpBCD;void*_tmpBCC;(_tmpBCC=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBCD="Missing type in primop ",
_tag_dyneither(_tmpBCD,sizeof(char),24))),_tag_dyneither(_tmpBCC,sizeof(void*),0)));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct
_tuple10*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){struct _tuple10*_tmpBCE;return(
_tmpBCE=_cycalloc(sizeof(*_tmpBCE)),((_tmpBCE->f1=Cyc_Toc_mt_tq,((_tmpBCE->f2=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmpBCE)))));}
static struct _tuple16*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){Cyc_Toc_exp_to_c(nv,e);{struct _tuple16*_tmpBCF;return(_tmpBCF=_cycalloc(
sizeof(*_tmpBCF)),((_tmpBCF->f1=0,((_tmpBCF->f2=e,_tmpBCF)))));};}static struct
Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct _tuple0*x,void*
struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int
is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(pointer){t=Cyc_Absyn_cstar_typ(
struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*_tmp399=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp399): Cyc_Toc_malloc_ptr(
_tmp399));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)rgnopt;Cyc_Toc_exp_to_c(
nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,_tmp399);}};}else{t=
struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,eo,s,0),0);}
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){int count=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct Cyc_List_List*_tmp39A=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;
_tmp39A != 0;_tmp39A=_tmp39A->tl){struct _tuple16 _tmp39C;struct Cyc_List_List*
_tmp39D;struct Cyc_Absyn_Exp*_tmp39E;struct _tuple16*_tmp39B=(struct _tuple16*)
_tmp39A->hd;_tmp39C=*_tmp39B;_tmp39D=_tmp39C.f1;_tmp39E=_tmp39C.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp39D == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp39D->tl != 0){const char*_tmpBD2;void*_tmpBD1;(_tmpBD1=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBD2="multiple designators in array",
_tag_dyneither(_tmpBD2,sizeof(char),30))),_tag_dyneither(_tmpBD1,sizeof(void*),0)));}{
void*_tmp3A1=(void*)_tmp39D->hd;void*_tmp3A2=_tmp3A1;struct Cyc_Absyn_Exp*_tmp3A4;
_LL180: {struct Cyc_Absyn_ArrayElement_struct*_tmp3A3=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp3A2;if(_tmp3A3->tag != 0)goto _LL182;else{_tmp3A4=_tmp3A3->f1;}}_LL181: Cyc_Toc_exp_to_c(
nv,_tmp3A4);e_index=_tmp3A4;goto _LL17F;_LL182: {struct Cyc_Absyn_FieldName_struct*
_tmp3A5=(struct Cyc_Absyn_FieldName_struct*)_tmp3A2;if(_tmp3A5->tag != 1)goto
_LL17F;}_LL183: {const char*_tmpBD5;void*_tmpBD4;(_tmpBD4=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBD5="field name designators in array",
_tag_dyneither(_tmpBD5,sizeof(char),32))),_tag_dyneither(_tmpBD4,sizeof(void*),0)));}
_LL17F:;};}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*
_tmp3A8=_tmp39E->r;struct Cyc_List_List*_tmp3AA;struct Cyc_Absyn_Vardecl*_tmp3AC;
struct Cyc_Absyn_Exp*_tmp3AD;struct Cyc_Absyn_Exp*_tmp3AE;int _tmp3AF;void*_tmp3B1;
struct Cyc_List_List*_tmp3B2;_LL185: {struct Cyc_Absyn_Array_e_struct*_tmp3A9=(
struct Cyc_Absyn_Array_e_struct*)_tmp3A8;if(_tmp3A9->tag != 27)goto _LL187;else{
_tmp3AA=_tmp3A9->f1;}}_LL186: s=Cyc_Toc_init_array(nv,lval,_tmp3AA,s);goto _LL184;
_LL187: {struct Cyc_Absyn_Comprehension_e_struct*_tmp3AB=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3A8;if(_tmp3AB->tag != 28)goto _LL189;else{_tmp3AC=_tmp3AB->f1;_tmp3AD=_tmp3AB->f2;
_tmp3AE=_tmp3AB->f3;_tmp3AF=_tmp3AB->f4;}}_LL188: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp3AC,_tmp3AD,_tmp3AE,_tmp3AF,s,0);goto _LL184;_LL189: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3B0=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3A8;if(_tmp3B0->tag != 30)goto
_LL18B;else{_tmp3B1=(void*)_tmp3B0->f1;_tmp3B2=_tmp3B0->f2;}}_LL18A: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3B1,_tmp3B2,s);goto _LL184;_LL18B:;_LL18C: Cyc_Toc_exp_to_c(nv,_tmp39E);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
_tmp39E,0),s,0);goto _LL184;_LL184:;};};}}return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple0*_tmp3B3=vd->name;void*_tmp3B4=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env _tmp3B6;struct _RegionHandle*_tmp3B7;
struct Cyc_Toc_Env*_tmp3B5=nv;_tmp3B6=*_tmp3B5;_tmp3B7=_tmp3B6.rgn;{struct Cyc_Absyn_Local_b_struct
_tmpBD8;struct Cyc_Absyn_Local_b_struct*_tmpBD7;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
_tmp3B7,nv,_tmp3B3,Cyc_Absyn_varb_exp(_tmp3B3,(void*)((_tmpBD7=_cycalloc(sizeof(*
_tmpBD7)),((_tmpBD7[0]=((_tmpBD8.tag=4,((_tmpBD8.f1=vd,_tmpBD8)))),_tmpBD7)))),0));
struct _tuple0*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp3B3,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp3B3,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp3B3,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp3B3,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp3B8=e2->r;struct Cyc_List_List*_tmp3BA;
struct Cyc_Absyn_Vardecl*_tmp3BC;struct Cyc_Absyn_Exp*_tmp3BD;struct Cyc_Absyn_Exp*
_tmp3BE;int _tmp3BF;void*_tmp3C1;struct Cyc_List_List*_tmp3C2;_LL18E: {struct Cyc_Absyn_Array_e_struct*
_tmp3B9=(struct Cyc_Absyn_Array_e_struct*)_tmp3B8;if(_tmp3B9->tag != 27)goto _LL190;
else{_tmp3BA=_tmp3B9->f1;}}_LL18F: body=Cyc_Toc_init_array(nv2,lval,_tmp3BA,Cyc_Toc_skip_stmt_dl());
goto _LL18D;_LL190: {struct Cyc_Absyn_Comprehension_e_struct*_tmp3BB=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3B8;if(_tmp3BB->tag != 28)goto _LL192;else{_tmp3BC=_tmp3BB->f1;_tmp3BD=_tmp3BB->f2;
_tmp3BE=_tmp3BB->f3;_tmp3BF=_tmp3BB->f4;}}_LL191: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp3BC,_tmp3BD,_tmp3BE,_tmp3BF,Cyc_Toc_skip_stmt_dl(),0);goto _LL18D;
_LL192: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp3C0=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp3B8;if(_tmp3C0->tag != 30)goto _LL194;else{_tmp3C1=(void*)_tmp3C0->f1;_tmp3C2=
_tmp3C0->f2;}}_LL193: body=Cyc_Toc_init_anon_struct(nv,lval,_tmp3C1,_tmp3C2,Cyc_Toc_skip_stmt_dl());
goto _LL18D;_LL194:;_LL195: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL18D;_LL18D:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp3B4,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp3B3,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp3C5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp3C5 != 0;_tmp3C5=_tmp3C5->tl){struct _tuple16 _tmp3C7;
struct Cyc_List_List*_tmp3C8;struct Cyc_Absyn_Exp*_tmp3C9;struct _tuple16*_tmp3C6=(
struct _tuple16*)_tmp3C5->hd;_tmp3C7=*_tmp3C6;_tmp3C8=_tmp3C7.f1;_tmp3C9=_tmp3C7.f2;
if(_tmp3C8 == 0){const char*_tmpBDB;void*_tmpBDA;(_tmpBDA=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBDB="empty designator list",
_tag_dyneither(_tmpBDB,sizeof(char),22))),_tag_dyneither(_tmpBDA,sizeof(void*),0)));}
if(_tmp3C8->tl != 0){const char*_tmpBDE;void*_tmpBDD;(_tmpBDD=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBDE="too many designators in anonymous struct",
_tag_dyneither(_tmpBDE,sizeof(char),41))),_tag_dyneither(_tmpBDD,sizeof(void*),0)));}{
void*_tmp3CE=(void*)_tmp3C8->hd;struct _dyneither_ptr*_tmp3D0;_LL197: {struct Cyc_Absyn_FieldName_struct*
_tmp3CF=(struct Cyc_Absyn_FieldName_struct*)_tmp3CE;if(_tmp3CF->tag != 1)goto
_LL199;else{_tmp3D0=_tmp3CF->f1;}}_LL198: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(
lhs,_tmp3D0,0);{void*_tmp3D1=_tmp3C9->r;struct Cyc_List_List*_tmp3D3;struct Cyc_Absyn_Vardecl*
_tmp3D5;struct Cyc_Absyn_Exp*_tmp3D6;struct Cyc_Absyn_Exp*_tmp3D7;int _tmp3D8;void*
_tmp3DA;struct Cyc_List_List*_tmp3DB;_LL19C: {struct Cyc_Absyn_Array_e_struct*
_tmp3D2=(struct Cyc_Absyn_Array_e_struct*)_tmp3D1;if(_tmp3D2->tag != 27)goto _LL19E;
else{_tmp3D3=_tmp3D2->f1;}}_LL19D: s=Cyc_Toc_init_array(nv,lval,_tmp3D3,s);goto
_LL19B;_LL19E: {struct Cyc_Absyn_Comprehension_e_struct*_tmp3D4=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3D1;if(_tmp3D4->tag != 28)goto _LL1A0;else{_tmp3D5=_tmp3D4->f1;_tmp3D6=_tmp3D4->f2;
_tmp3D7=_tmp3D4->f3;_tmp3D8=_tmp3D4->f4;}}_LL19F: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp3D5,_tmp3D6,_tmp3D7,_tmp3D8,s,0);goto _LL19B;_LL1A0: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3D9=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3D1;if(_tmp3D9->tag != 30)goto
_LL1A2;else{_tmp3DA=(void*)_tmp3D9->f1;_tmp3DB=_tmp3D9->f2;}}_LL1A1: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3DA,_tmp3DB,s);goto _LL19B;_LL1A2:;_LL1A3: Cyc_Toc_exp_to_c(nv,_tmp3C9);
if(Cyc_Toc_is_poly_field(struct_type,_tmp3D0))_tmp3C9=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp3C9);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp3C9,0),0),s,0);goto _LL19B;_LL19B:;}goto _LL196;}_LL199:;_LL19A: {const char*
_tmpBE1;void*_tmpBE0;(_tmpBE0=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE1="array designator in struct",
_tag_dyneither(_tmpBE1,sizeof(char),27))),_tag_dyneither(_tmpBE0,sizeof(void*),0)));}
_LL196:;};}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct Cyc_Toc_Env
_tmp3DF;struct _RegionHandle*_tmp3E0;struct Cyc_Toc_Env*_tmp3DE=nv;_tmp3DF=*
_tmp3DE;_tmp3E0=_tmp3DF.rgn;{struct Cyc_List_List*_tmp3E1=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_rmap)(_tmp3E0,Cyc_Toc_tup_to_c,es);void*_tmp3E2=Cyc_Toc_add_tuple_type(
_tmp3E1);struct _tuple0*_tmp3E3=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp3E4=
Cyc_Absyn_var_exp(_tmp3E3,0);struct Cyc_Absyn_Stmt*_tmp3E5=Cyc_Absyn_exp_stmt(
_tmp3E4,0);struct Cyc_Absyn_Exp*(*_tmp3E6)(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp3E7=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp3E0,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp3E7);for(0;_tmp3E7 != 0;(_tmp3E7=
_tmp3E7->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp3E7->hd;struct
Cyc_Absyn_Exp*lval=_tmp3E6(_tmp3E4,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp3E8=e->r;struct Cyc_List_List*_tmp3EA;struct Cyc_Absyn_Vardecl*_tmp3EC;
struct Cyc_Absyn_Exp*_tmp3ED;struct Cyc_Absyn_Exp*_tmp3EE;int _tmp3EF;_LL1A5: {
struct Cyc_Absyn_Array_e_struct*_tmp3E9=(struct Cyc_Absyn_Array_e_struct*)_tmp3E8;
if(_tmp3E9->tag != 27)goto _LL1A7;else{_tmp3EA=_tmp3E9->f1;}}_LL1A6: _tmp3E5=Cyc_Toc_init_array(
nv,lval,_tmp3EA,_tmp3E5);goto _LL1A4;_LL1A7: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp3EB=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp3E8;if(_tmp3EB->tag != 28)
goto _LL1A9;else{_tmp3EC=_tmp3EB->f1;_tmp3ED=_tmp3EB->f2;_tmp3EE=_tmp3EB->f3;
_tmp3EF=_tmp3EB->f4;}}_LL1A8: _tmp3E5=Cyc_Toc_init_comprehension(nv,lval,_tmp3EC,
_tmp3ED,_tmp3EE,_tmp3EF,_tmp3E5,0);goto _LL1A4;_LL1A9:;_LL1AA: Cyc_Toc_exp_to_c(nv,
e);_tmp3E5=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3E6(
_tmp3E4,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp3E5,0);goto _LL1A4;_LL1A4:;};}}
return Cyc_Toc_make_struct(nv,_tmp3E3,_tmp3E2,_tmp3E5,pointer,rgnopt,is_atomic);};}
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct
_dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp3F0=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp3F0 != 0;_tmp3F0=_tmp3F0->tl){struct Cyc_Absyn_Aggrfield*
_tmp3F1=(struct Cyc_Absyn_Aggrfield*)_tmp3F0->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp3F1->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}{
struct Cyc_String_pa_struct _tmpBE9;void*_tmpBE8[1];const char*_tmpBE7;void*_tmpBE6;(
_tmpBE6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)((_tmpBE9.tag=0,((_tmpBE9.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmpBE8[0]=& _tmpBE9,Cyc_aprintf(((_tmpBE7="get_member_offset %s failed",
_tag_dyneither(_tmpBE7,sizeof(char),28))),_tag_dyneither(_tmpBE8,sizeof(void*),1)))))))),
_tag_dyneither(_tmpBE6,sizeof(void*),0)));};}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,
struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){struct
_tuple0*_tmp3F6=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp3F7=Cyc_Absyn_var_exp(
_tmp3F6,0);struct Cyc_Absyn_Stmt*_tmp3F8=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(
_tmp3F7),0);struct Cyc_Absyn_Exp*(*_tmp3F9)(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
void*_tmp3FA=Cyc_Toc_typ_to_c(struct_type);int is_atomic=1;struct Cyc_List_List*
forall_types;struct Cyc_Absyn_Aggrdecl*ad;struct Cyc_List_List*aggrfields;{void*
_tmp3FB=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp3FD;union
Cyc_Absyn_AggrInfoU _tmp3FE;struct Cyc_List_List*_tmp3FF;_LL1AC: {struct Cyc_Absyn_AggrType_struct*
_tmp3FC=(struct Cyc_Absyn_AggrType_struct*)_tmp3FB;if(_tmp3FC->tag != 11)goto
_LL1AE;else{_tmp3FD=_tmp3FC->f1;_tmp3FE=_tmp3FD.aggr_info;_tmp3FF=_tmp3FD.targs;}}
_LL1AD: ad=Cyc_Absyn_get_known_aggrdecl(_tmp3FE);aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;forall_types=_tmp3FF;goto _LL1AB;_LL1AE:;_LL1AF: {
const char*_tmpBEC;void*_tmpBEB;(_tmpBEB=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBEC="init_struct: bad struct type",
_tag_dyneither(_tmpBEC,sizeof(char),29))),_tag_dyneither(_tmpBEB,sizeof(void*),0)));}
_LL1AB:;}{void*orig_typ=Cyc_Toc_typ_to_c(struct_type);void*aggr_typ=orig_typ;if(
exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields
!= 0){struct Cyc_List_List*_tmp402=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp402->tl != 0;_tmp402=_tmp402->tl){;}{struct Cyc_Absyn_Aggrfield*_tmp403=(
struct Cyc_Absyn_Aggrfield*)_tmp402->hd;struct Cyc_Toc_Env _tmp405;struct
_RegionHandle*_tmp406;struct Cyc_Toc_Env*_tmp404=nv;_tmp405=*_tmp404;_tmp406=
_tmp405.rgn;{void*cast_type;if(pointer)cast_type=Cyc_Absyn_cstar_typ(orig_typ,
Cyc_Toc_mt_tq);else{cast_type=orig_typ;}_tmp3F8=Cyc_Absyn_exp_stmt(Cyc_Toc_cast_it(
cast_type,Cyc_Absyn_copy_exp(_tmp3F7)),0);{struct Cyc_List_List*_tmp407=((struct
Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp406,_tmp406,ad->tvs,forall_types);
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(_tmp406,_tmp407,_tmp403->type))){
struct Cyc_List_List*_tmp408=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
_tmp406,_tmp406,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,
exist_types);struct Cyc_List_List*_tmp409=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp406,_tmp407,_tmp408);struct Cyc_List_List*new_fields=0;for(_tmp402=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp402 != 0;_tmp402=
_tmp402->tl){struct Cyc_Absyn_Aggrfield*_tmp40A=(struct Cyc_Absyn_Aggrfield*)
_tmp402->hd;struct Cyc_Absyn_Aggrfield*_tmpBED;struct Cyc_Absyn_Aggrfield*_tmp40B=(
_tmpBED=_cycalloc(sizeof(*_tmpBED)),((_tmpBED->name=_tmp40A->name,((_tmpBED->tq=
Cyc_Toc_mt_tq,((_tmpBED->type=Cyc_Tcutil_rsubstitute(_tmp406,_tmp409,_tmp40A->type),((
_tmpBED->width=_tmp40A->width,((_tmpBED->attributes=_tmp40A->attributes,_tmpBED)))))))))));
struct Cyc_List_List*_tmpBEE;new_fields=((_tmpBEE=_cycalloc(sizeof(*_tmpBEE)),((
_tmpBEE->hd=_tmp40B,((_tmpBEE->tl=new_fields,_tmpBEE))))));}exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
new_fields);{struct Cyc_Absyn_AnonAggrType_struct _tmpBF1;struct Cyc_Absyn_AnonAggrType_struct*
_tmpBF0;struct_type=(void*)((_tmpBF0=_cycalloc(sizeof(*_tmpBF0)),((_tmpBF0[0]=((
_tmpBF1.tag=12,((_tmpBF1.f1=ad->kind,((_tmpBF1.f2=aggrfields,_tmpBF1)))))),
_tmpBF0))));}aggr_typ=Cyc_Toc_typ_to_c(struct_type);}};};};}{int is_tagged_union=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;struct Cyc_Toc_Env
_tmp411;struct _RegionHandle*_tmp412;struct Cyc_Toc_Env*_tmp410=nv;_tmp411=*
_tmp410;_tmp412=_tmp411.rgn;{struct Cyc_List_List*_tmp413=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp412,dles);for(0;
_tmp413 != 0;_tmp413=_tmp413->tl){struct _tuple16 _tmp415;struct Cyc_List_List*
_tmp416;struct Cyc_Absyn_Exp*_tmp417;struct _tuple16*_tmp414=(struct _tuple16*)
_tmp413->hd;_tmp415=*_tmp414;_tmp416=_tmp415.f1;_tmp417=_tmp415.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp417->topt))->v);
if(_tmp416 == 0){const char*_tmpBF4;void*_tmpBF3;(_tmpBF3=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF4="empty designator list",
_tag_dyneither(_tmpBF4,sizeof(char),22))),_tag_dyneither(_tmpBF3,sizeof(void*),0)));}
if(_tmp416->tl != 0){struct _tuple0*_tmp41A=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp41B=Cyc_Absyn_var_exp(_tmp41A,0);for(0;_tmp416 != 0;_tmp416=_tmp416->tl){void*
_tmp41C=(void*)_tmp416->hd;struct _dyneither_ptr*_tmp41E;_LL1B1: {struct Cyc_Absyn_FieldName_struct*
_tmp41D=(struct Cyc_Absyn_FieldName_struct*)_tmp41C;if(_tmp41D->tag != 1)goto
_LL1B3;else{_tmp41E=_tmp41D->f1;}}_LL1B2: if(Cyc_Toc_is_poly_field(struct_type,
_tmp41E))_tmp41B=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp41B);_tmp3F8=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3F9(_tmp3F7,_tmp41E,0),_tmp41B,0),0),
_tmp3F8,0);goto _LL1B0;_LL1B3:;_LL1B4: {const char*_tmpBF7;void*_tmpBF6;(_tmpBF6=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBF7="array designator in struct",_tag_dyneither(_tmpBF7,sizeof(char),27))),
_tag_dyneither(_tmpBF6,sizeof(void*),0)));}_LL1B0:;}Cyc_Toc_exp_to_c(nv,_tmp417);
_tmp3F8=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp41B,
_tmp417,0),0),_tmp3F8,0);}else{void*_tmp421=(void*)_tmp416->hd;struct
_dyneither_ptr*_tmp423;_LL1B6: {struct Cyc_Absyn_FieldName_struct*_tmp422=(struct
Cyc_Absyn_FieldName_struct*)_tmp421;if(_tmp422->tag != 1)goto _LL1B8;else{_tmp423=
_tmp422->f1;}}_LL1B7: {struct Cyc_Absyn_Exp*lval=_tmp3F9(_tmp3F7,_tmp423,0);if(
is_tagged_union){int i=Cyc_Toc_get_member_offset(ad,_tmp423);struct Cyc_Absyn_Exp*
f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_aggrmember_exp(
lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,
f_tag_exp,0);_tmp3F8=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3F8,0);
lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{void*_tmp424=_tmp417->r;
struct Cyc_List_List*_tmp426;struct Cyc_Absyn_Vardecl*_tmp428;struct Cyc_Absyn_Exp*
_tmp429;struct Cyc_Absyn_Exp*_tmp42A;int _tmp42B;void*_tmp42D;struct Cyc_List_List*
_tmp42E;_LL1BB: {struct Cyc_Absyn_Array_e_struct*_tmp425=(struct Cyc_Absyn_Array_e_struct*)
_tmp424;if(_tmp425->tag != 27)goto _LL1BD;else{_tmp426=_tmp425->f1;}}_LL1BC:
_tmp3F8=Cyc_Toc_init_array(nv,lval,_tmp426,_tmp3F8);goto _LL1BA;_LL1BD: {struct
Cyc_Absyn_Comprehension_e_struct*_tmp427=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp424;if(_tmp427->tag != 28)goto _LL1BF;else{_tmp428=_tmp427->f1;_tmp429=_tmp427->f2;
_tmp42A=_tmp427->f3;_tmp42B=_tmp427->f4;}}_LL1BE: _tmp3F8=Cyc_Toc_init_comprehension(
nv,lval,_tmp428,_tmp429,_tmp42A,_tmp42B,_tmp3F8,0);goto _LL1BA;_LL1BF: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp42C=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp424;if(_tmp42C->tag != 30)goto
_LL1C1;else{_tmp42D=(void*)_tmp42C->f1;_tmp42E=_tmp42C->f2;}}_LL1C0: _tmp3F8=Cyc_Toc_init_anon_struct(
nv,lval,_tmp42D,_tmp42E,_tmp3F8);goto _LL1BA;_LL1C1:;_LL1C2: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp417->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp417);{struct Cyc_Absyn_Aggrfield*_tmp42F=Cyc_Absyn_lookup_field(aggrfields,
_tmp423);if(Cyc_Toc_is_poly_field(struct_type,_tmp423) && !was_ptr_type)_tmp417=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp417);if(exist_types != 0)_tmp417=
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(
_tmp42F))->type),_tmp417);_tmp3F8=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
lval,_tmp417,0),0),_tmp3F8,0);goto _LL1BA;};}_LL1BA:;}goto _LL1B5;}_LL1B8:;_LL1B9: {
const char*_tmpBFA;void*_tmpBF9;(_tmpBF9=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBFA="array designator in struct",
_tag_dyneither(_tmpBFA,sizeof(char),27))),_tag_dyneither(_tmpBF9,sizeof(void*),0)));}
_LL1B5:;}}}return Cyc_Toc_make_struct(nv,_tmp3F6,aggr_typ,_tmp3F8,pointer,rgnopt,
is_atomic);};};}struct _tuple20{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct
_tuple20*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(
e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,
enum Cyc_Absyn_Incrementor incr){struct Cyc_Absyn_Increment_e_struct _tmpBFD;struct
Cyc_Absyn_Increment_e_struct*_tmpBFC;return Cyc_Absyn_new_exp((void*)((_tmpBFC=
_cycalloc(sizeof(*_tmpBFC)),((_tmpBFC[0]=((_tmpBFD.tag=5,((_tmpBFD.f1=e1,((
_tmpBFD.f2=incr,_tmpBFD)))))),_tmpBFC)))),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp434=e1->r;struct Cyc_Absyn_Stmt*_tmp436;void*_tmp438;struct Cyc_Absyn_Exp*
_tmp439;struct Cyc_Absyn_Exp*_tmp43B;struct _dyneither_ptr*_tmp43C;int _tmp43D;int
_tmp43E;_LL1C4: {struct Cyc_Absyn_StmtExp_e_struct*_tmp435=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp434;if(_tmp435->tag != 37)goto _LL1C6;else{_tmp436=_tmp435->f1;}}_LL1C5: Cyc_Toc_lvalue_assign_stmt(
_tmp436,fs,f,f_env);goto _LL1C3;_LL1C6: {struct Cyc_Absyn_Cast_e_struct*_tmp437=(
struct Cyc_Absyn_Cast_e_struct*)_tmp434;if(_tmp437->tag != 15)goto _LL1C8;else{
_tmp438=(void*)_tmp437->f1;_tmp439=_tmp437->f2;}}_LL1C7: Cyc_Toc_lvalue_assign(
_tmp439,fs,f,f_env);goto _LL1C3;_LL1C8: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp43A=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp434;if(_tmp43A->tag != 22)goto
_LL1CA;else{_tmp43B=_tmp43A->f1;_tmp43C=_tmp43A->f2;_tmp43D=_tmp43A->f3;_tmp43E=
_tmp43A->f4;}}_LL1C9: e1->r=_tmp43B->r;{struct Cyc_List_List*_tmpBFE;Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)((_tmpBFE=_cycalloc(sizeof(*_tmpBFE)),((_tmpBFE->hd=
_tmp43C,((_tmpBFE->tl=fs,_tmpBFE)))))),f,f_env);}goto _LL1C3;_LL1CA:;_LL1CB: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;goto _LL1C3;}_LL1C3:;}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env){void*_tmp440=s->r;struct Cyc_Absyn_Exp*_tmp442;struct Cyc_Absyn_Decl*
_tmp444;struct Cyc_Absyn_Stmt*_tmp445;struct Cyc_Absyn_Stmt*_tmp447;_LL1CD: {
struct Cyc_Absyn_Exp_s_struct*_tmp441=(struct Cyc_Absyn_Exp_s_struct*)_tmp440;if(
_tmp441->tag != 1)goto _LL1CF;else{_tmp442=_tmp441->f1;}}_LL1CE: Cyc_Toc_lvalue_assign(
_tmp442,fs,f,f_env);goto _LL1CC;_LL1CF: {struct Cyc_Absyn_Decl_s_struct*_tmp443=(
struct Cyc_Absyn_Decl_s_struct*)_tmp440;if(_tmp443->tag != 12)goto _LL1D1;else{
_tmp444=_tmp443->f1;_tmp445=_tmp443->f2;}}_LL1D0: Cyc_Toc_lvalue_assign_stmt(
_tmp445,fs,f,f_env);goto _LL1CC;_LL1D1: {struct Cyc_Absyn_Seq_s_struct*_tmp446=(
struct Cyc_Absyn_Seq_s_struct*)_tmp440;if(_tmp446->tag != 2)goto _LL1D3;else{
_tmp447=_tmp446->f2;}}_LL1D2: Cyc_Toc_lvalue_assign_stmt(_tmp447,fs,f,f_env);goto
_LL1CC;_LL1D3:;_LL1D4: {const char*_tmpC02;void*_tmpC01[1];struct Cyc_String_pa_struct
_tmpC00;(_tmpC00.tag=0,((_tmpC00.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmpC01[0]=& _tmpC00,Cyc_Toc_toc_impos(((_tmpC02="lvalue_assign_stmt: %s",
_tag_dyneither(_tmpC02,sizeof(char),23))),_tag_dyneither(_tmpC01,sizeof(void*),1)))))));}
_LL1CC:;}static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);static
struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){void*_tmp44B=
e->r;void*_tmp44D;void**_tmp44E;struct Cyc_Absyn_Exp*_tmp44F;struct Cyc_Absyn_Exp**
_tmp450;struct Cyc_Absyn_Exp*_tmp452;struct Cyc_Absyn_Stmt*_tmp454;_LL1D6: {struct
Cyc_Absyn_Cast_e_struct*_tmp44C=(struct Cyc_Absyn_Cast_e_struct*)_tmp44B;if(
_tmp44C->tag != 15)goto _LL1D8;else{_tmp44D=(void**)& _tmp44C->f1;_tmp44E=(void**)((
void**)& _tmp44C->f1);_tmp44F=_tmp44C->f2;_tmp450=(struct Cyc_Absyn_Exp**)& _tmp44C->f2;}}
_LL1D7:*_tmp450=Cyc_Toc_push_address_exp(*_tmp450);*_tmp44E=Cyc_Absyn_cstar_typ(*
_tmp44E,Cyc_Toc_mt_tq);return e;_LL1D8: {struct Cyc_Absyn_Deref_e_struct*_tmp451=(
struct Cyc_Absyn_Deref_e_struct*)_tmp44B;if(_tmp451->tag != 21)goto _LL1DA;else{
_tmp452=_tmp451->f1;}}_LL1D9: return _tmp452;_LL1DA: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp453=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp44B;if(_tmp453->tag != 37)goto
_LL1DC;else{_tmp454=_tmp453->f1;}}_LL1DB: Cyc_Toc_push_address_stmt(_tmp454);
return e;_LL1DC:;_LL1DD: if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpC06;void*_tmpC05[1];struct Cyc_String_pa_struct _tmpC04;(_tmpC04.tag=
0,((_tmpC04.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpC05[0]=& _tmpC04,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpC06="can't take & of exp %s",_tag_dyneither(_tmpC06,
sizeof(char),23))),_tag_dyneither(_tmpC05,sizeof(void*),1)))))));};_LL1D5:;}
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){void*_tmp458=s->r;
struct Cyc_Absyn_Stmt*_tmp45A;struct Cyc_Absyn_Stmt*_tmp45C;struct Cyc_Absyn_Exp*
_tmp45E;struct Cyc_Absyn_Exp**_tmp45F;_LL1DF: {struct Cyc_Absyn_Seq_s_struct*
_tmp459=(struct Cyc_Absyn_Seq_s_struct*)_tmp458;if(_tmp459->tag != 2)goto _LL1E1;
else{_tmp45A=_tmp459->f2;}}_LL1E0: _tmp45C=_tmp45A;goto _LL1E2;_LL1E1: {struct Cyc_Absyn_Decl_s_struct*
_tmp45B=(struct Cyc_Absyn_Decl_s_struct*)_tmp458;if(_tmp45B->tag != 12)goto _LL1E3;
else{_tmp45C=_tmp45B->f2;}}_LL1E2: Cyc_Toc_push_address_stmt(_tmp45C);goto _LL1DE;
_LL1E3: {struct Cyc_Absyn_Exp_s_struct*_tmp45D=(struct Cyc_Absyn_Exp_s_struct*)
_tmp458;if(_tmp45D->tag != 1)goto _LL1E5;else{_tmp45E=_tmp45D->f1;_tmp45F=(struct
Cyc_Absyn_Exp**)& _tmp45D->f1;}}_LL1E4:*_tmp45F=Cyc_Toc_push_address_exp(*_tmp45F);
goto _LL1DE;_LL1E5:;_LL1E6: {const char*_tmpC0A;void*_tmpC09[1];struct Cyc_String_pa_struct
_tmpC08;(_tmpC08.tag=0,((_tmpC08.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmpC09[0]=& _tmpC08,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC0A="can't take & of stmt %s",
_tag_dyneither(_tmpC0A,sizeof(char),24))),_tag_dyneither(_tmpC09,sizeof(void*),1)))))));}
_LL1DE:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;
struct Cyc_List_List*prev;if(x == 0)return 0;{struct Cyc_List_List*_tmpC0B;result=((
_tmpC0B=_region_malloc(r2,sizeof(*_tmpC0B)),((_tmpC0B->hd=(void*)f((void*)x->hd,
env),((_tmpC0B->tl=0,_tmpC0B))))));}prev=result;for(x=x->tl;x != 0;x=x->tl){{
struct Cyc_List_List*_tmpC0C;((struct Cyc_List_List*)_check_null(prev))->tl=((
_tmpC0C=_region_malloc(r2,sizeof(*_tmpC0C)),((_tmpC0C->hd=(void*)f((void*)x->hd,
env),((_tmpC0C->tl=0,_tmpC0C))))));}prev=prev->tl;}return result;}static struct Cyc_List_List*
Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region,f,env,x);}static struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*
e){struct _tuple16*_tmpC0D;return(_tmpC0D=_cycalloc(sizeof(*_tmpC0D)),((_tmpC0D->f1=
0,((_tmpC0D->f2=e,_tmpC0D)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(
void*t){void*_tmp466=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp468;
_LL1E8: {struct Cyc_Absyn_PointerType_struct*_tmp467=(struct Cyc_Absyn_PointerType_struct*)
_tmp466;if(_tmp467->tag != 5)goto _LL1EA;else{_tmp468=_tmp467->f1;}}_LL1E9: return
_tmp468;_LL1EA:;_LL1EB: {const char*_tmpC10;void*_tmpC0F;(_tmpC0F=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC10="get_ptr_typ: not a pointer!",
_tag_dyneither(_tmpC10,sizeof(char),28))),_tag_dyneither(_tmpC0F,sizeof(void*),0)));}
_LL1E7:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp46B=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp46E;enum Cyc_Absyn_Size_of
_tmp46F;enum Cyc_Absyn_Sign _tmp471;enum Cyc_Absyn_Size_of _tmp472;enum Cyc_Absyn_Sign
_tmp476;enum Cyc_Absyn_Size_of _tmp477;enum Cyc_Absyn_Sign _tmp479;enum Cyc_Absyn_Size_of
_tmp47A;enum Cyc_Absyn_Sign _tmp47C;enum Cyc_Absyn_Size_of _tmp47D;int _tmp47F;int
_tmp481;int _tmp483;_LL1ED: {struct Cyc_Absyn_PointerType_struct*_tmp46C=(struct
Cyc_Absyn_PointerType_struct*)_tmp46B;if(_tmp46C->tag != 5)goto _LL1EF;}_LL1EE: res=
Cyc_Absyn_null_exp(0);goto _LL1EC;_LL1EF: {struct Cyc_Absyn_IntType_struct*_tmp46D=(
struct Cyc_Absyn_IntType_struct*)_tmp46B;if(_tmp46D->tag != 6)goto _LL1F1;else{
_tmp46E=_tmp46D->f1;_tmp46F=_tmp46D->f2;if(_tmp46F != Cyc_Absyn_Char_sz)goto
_LL1F1;}}_LL1F0: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp46E,'\000'),0);goto
_LL1EC;_LL1F1: {struct Cyc_Absyn_IntType_struct*_tmp470=(struct Cyc_Absyn_IntType_struct*)
_tmp46B;if(_tmp470->tag != 6)goto _LL1F3;else{_tmp471=_tmp470->f1;_tmp472=_tmp470->f2;
if(_tmp472 != Cyc_Absyn_Short_sz)goto _LL1F3;}}_LL1F2: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(
_tmp471,0),0);goto _LL1EC;_LL1F3: {struct Cyc_Absyn_EnumType_struct*_tmp473=(
struct Cyc_Absyn_EnumType_struct*)_tmp46B;if(_tmp473->tag != 13)goto _LL1F5;}_LL1F4:
goto _LL1F6;_LL1F5: {struct Cyc_Absyn_AnonEnumType_struct*_tmp474=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp46B;if(_tmp474->tag != 14)goto _LL1F7;}_LL1F6: _tmp476=Cyc_Absyn_Unsigned;goto
_LL1F8;_LL1F7: {struct Cyc_Absyn_IntType_struct*_tmp475=(struct Cyc_Absyn_IntType_struct*)
_tmp46B;if(_tmp475->tag != 6)goto _LL1F9;else{_tmp476=_tmp475->f1;_tmp477=_tmp475->f2;
if(_tmp477 != Cyc_Absyn_Int_sz)goto _LL1F9;}}_LL1F8: _tmp479=_tmp476;goto _LL1FA;
_LL1F9: {struct Cyc_Absyn_IntType_struct*_tmp478=(struct Cyc_Absyn_IntType_struct*)
_tmp46B;if(_tmp478->tag != 6)goto _LL1FB;else{_tmp479=_tmp478->f1;_tmp47A=_tmp478->f2;
if(_tmp47A != Cyc_Absyn_Long_sz)goto _LL1FB;}}_LL1FA: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(
_tmp479,0),0);goto _LL1EC;_LL1FB: {struct Cyc_Absyn_IntType_struct*_tmp47B=(struct
Cyc_Absyn_IntType_struct*)_tmp46B;if(_tmp47B->tag != 6)goto _LL1FD;else{_tmp47C=
_tmp47B->f1;_tmp47D=_tmp47B->f2;if(_tmp47D != Cyc_Absyn_LongLong_sz)goto _LL1FD;}}
_LL1FC: res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp47C,(long long)0),0);
goto _LL1EC;_LL1FD: {struct Cyc_Absyn_FloatType_struct*_tmp47E=(struct Cyc_Absyn_FloatType_struct*)
_tmp46B;if(_tmp47E->tag != 7)goto _LL1FF;else{_tmp47F=_tmp47E->f1;if(_tmp47F != 0)
goto _LL1FF;}}_LL1FE:{const char*_tmpC11;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((
_tmpC11="0.0F",_tag_dyneither(_tmpC11,sizeof(char),5))),0),0);}goto _LL1EC;_LL1FF: {
struct Cyc_Absyn_FloatType_struct*_tmp480=(struct Cyc_Absyn_FloatType_struct*)
_tmp46B;if(_tmp480->tag != 7)goto _LL201;else{_tmp481=_tmp480->f1;if(_tmp481 != 1)
goto _LL201;}}_LL200:{const char*_tmpC12;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((
_tmpC12="0.0",_tag_dyneither(_tmpC12,sizeof(char),4))),1),0);}goto _LL1EC;_LL201: {
struct Cyc_Absyn_FloatType_struct*_tmp482=(struct Cyc_Absyn_FloatType_struct*)
_tmp46B;if(_tmp482->tag != 7)goto _LL203;else{_tmp483=_tmp482->f1;}}_LL202:{const
char*_tmpC13;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpC13="0.0L",
_tag_dyneither(_tmpC13,sizeof(char),5))),_tmp483),0);}goto _LL1EC;_LL203:;_LL204: {
const char*_tmpC17;void*_tmpC16[1];struct Cyc_String_pa_struct _tmpC15;(_tmpC15.tag=
0,((_tmpC15.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmpC16[0]=& _tmpC15,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpC17="found non-zero type %s in generate_zero",
_tag_dyneither(_tmpC17,sizeof(char),40))),_tag_dyneither(_tmpC16,sizeof(void*),1)))))));}
_LL1EC:;}{struct Cyc_Core_Opt*_tmpC18;res->topt=((_tmpC18=_cycalloc(sizeof(*
_tmpC18)),((_tmpC18->v=t,_tmpC18))));}return res;}static void Cyc_Toc_zero_ptr_assign_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){void*
fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,
Cyc_Absyn_true_conref);void*_tmp48B=Cyc_Toc_typ_to_c(elt_type);void*_tmp48C=Cyc_Toc_typ_to_c(
fat_ptr_type);void*_tmp48D=Cyc_Absyn_cstar_typ(_tmp48B,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmpC19;struct Cyc_Core_Opt*_tmp48E=(_tmpC19=_cycalloc(sizeof(*_tmpC19)),((
_tmpC19->v=_tmp48D,_tmpC19)));struct Cyc_Absyn_Exp*xinit;{void*_tmp48F=e1->r;
struct Cyc_Absyn_Exp*_tmp491;struct Cyc_Absyn_Exp*_tmp493;struct Cyc_Absyn_Exp*
_tmp494;_LL206: {struct Cyc_Absyn_Deref_e_struct*_tmp490=(struct Cyc_Absyn_Deref_e_struct*)
_tmp48F;if(_tmp490->tag != 21)goto _LL208;else{_tmp491=_tmp490->f1;}}_LL207: if(!
is_dyneither){_tmp491=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp491,0,Cyc_Absyn_Other_coercion,
0);{struct Cyc_Core_Opt*_tmpC1A;_tmp491->topt=((_tmpC1A=_cycalloc(sizeof(*_tmpC1A)),((
_tmpC1A->v=fat_ptr_type,_tmpC1A))));};}Cyc_Toc_exp_to_c(nv,_tmp491);xinit=
_tmp491;goto _LL205;_LL208: {struct Cyc_Absyn_Subscript_e_struct*_tmp492=(struct
Cyc_Absyn_Subscript_e_struct*)_tmp48F;if(_tmp492->tag != 24)goto _LL20A;else{
_tmp493=_tmp492->f1;_tmp494=_tmp492->f2;}}_LL209: if(!is_dyneither){_tmp493=Cyc_Absyn_cast_exp(
fat_ptr_type,_tmp493,0,Cyc_Absyn_Other_coercion,0);{struct Cyc_Core_Opt*_tmpC1B;
_tmp493->topt=((_tmpC1B=_cycalloc(sizeof(*_tmpC1B)),((_tmpC1B->v=fat_ptr_type,
_tmpC1B))));};}Cyc_Toc_exp_to_c(nv,_tmp493);Cyc_Toc_exp_to_c(nv,_tmp494);{struct
Cyc_Absyn_Exp*_tmpC1C[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,((
_tmpC1C[2]=_tmp494,((_tmpC1C[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmpC1C[0]=_tmp493,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC1C,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL205;_LL20A:;_LL20B: {const char*_tmpC1F;void*_tmpC1E;(_tmpC1E=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC1F="found bad lhs for zero-terminated pointer assignment",
_tag_dyneither(_tmpC1F,sizeof(char),53))),_tag_dyneither(_tmpC1E,sizeof(void*),0)));}
_LL205:;}{struct _tuple0*_tmp49A=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp49C;
struct _RegionHandle*_tmp49D;struct Cyc_Toc_Env*_tmp49B=nv;_tmp49C=*_tmp49B;
_tmp49D=_tmp49C.rgn;{struct Cyc_Toc_Env*_tmp49E=Cyc_Toc_add_varmap(_tmp49D,nv,
_tmp49A,Cyc_Absyn_var_exp(_tmp49A,0));struct Cyc_Absyn_Vardecl*_tmpC20;struct Cyc_Absyn_Vardecl*
_tmp49F=(_tmpC20=_cycalloc(sizeof(*_tmpC20)),((_tmpC20->sc=Cyc_Absyn_Public,((
_tmpC20->name=_tmp49A,((_tmpC20->tq=Cyc_Toc_mt_tq,((_tmpC20->type=_tmp48C,((
_tmpC20->initializer=(struct Cyc_Absyn_Exp*)xinit,((_tmpC20->rgn=0,((_tmpC20->attributes=
0,((_tmpC20->escapes=0,_tmpC20)))))))))))))))));struct Cyc_Absyn_Local_b_struct
_tmpC23;struct Cyc_Absyn_Local_b_struct*_tmpC22;struct Cyc_Absyn_Local_b_struct*
_tmp4A0=(_tmpC22=_cycalloc(sizeof(*_tmpC22)),((_tmpC22[0]=((_tmpC23.tag=4,((
_tmpC23.f1=_tmp49F,_tmpC23)))),_tmpC22)));struct Cyc_Absyn_Exp*_tmp4A1=Cyc_Absyn_varb_exp(
_tmp49A,(void*)_tmp4A0,0);{struct Cyc_Core_Opt*_tmpC24;_tmp4A1->topt=((_tmpC24=
_cycalloc(sizeof(*_tmpC24)),((_tmpC24->v=fat_ptr_type,_tmpC24))));}{struct Cyc_Absyn_Exp*
_tmp4A3=Cyc_Absyn_deref_exp(_tmp4A1,0);{struct Cyc_Core_Opt*_tmpC25;_tmp4A3->topt=((
_tmpC25=_cycalloc(sizeof(*_tmpC25)),((_tmpC25->v=elt_type,_tmpC25))));}Cyc_Toc_exp_to_c(
_tmp49E,_tmp4A3);{struct _tuple0*_tmp4A5=Cyc_Toc_temp_var();_tmp49E=Cyc_Toc_add_varmap(
_tmp49D,_tmp49E,_tmp4A5,Cyc_Absyn_var_exp(_tmp4A5,0));{struct Cyc_Absyn_Vardecl*
_tmpC26;struct Cyc_Absyn_Vardecl*_tmp4A6=(_tmpC26=_cycalloc(sizeof(*_tmpC26)),((
_tmpC26->sc=Cyc_Absyn_Public,((_tmpC26->name=_tmp4A5,((_tmpC26->tq=Cyc_Toc_mt_tq,((
_tmpC26->type=_tmp48B,((_tmpC26->initializer=(struct Cyc_Absyn_Exp*)_tmp4A3,((
_tmpC26->rgn=0,((_tmpC26->attributes=0,((_tmpC26->escapes=0,_tmpC26)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpC29;struct Cyc_Absyn_Local_b_struct*_tmpC28;
struct Cyc_Absyn_Local_b_struct*_tmp4A7=(_tmpC28=_cycalloc(sizeof(*_tmpC28)),((
_tmpC28[0]=((_tmpC29.tag=4,((_tmpC29.f1=_tmp4A6,_tmpC29)))),_tmpC28)));struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp4A8=Cyc_Absyn_varb_exp(_tmp4A5,(
void*)_tmp4A7,0);_tmp4A8->topt=_tmp4A3->topt;z_init=Cyc_Absyn_prim2_exp((enum 
Cyc_Absyn_Primop)popt->v,_tmp4A8,e2,0);z_init->topt=_tmp4A8->topt;}Cyc_Toc_exp_to_c(
_tmp49E,z_init);{struct _tuple0*_tmp4A9=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmpC2A;struct Cyc_Absyn_Vardecl*_tmp4AA=(_tmpC2A=_cycalloc(sizeof(*_tmpC2A)),((
_tmpC2A->sc=Cyc_Absyn_Public,((_tmpC2A->name=_tmp4A9,((_tmpC2A->tq=Cyc_Toc_mt_tq,((
_tmpC2A->type=_tmp48B,((_tmpC2A->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmpC2A->rgn=0,((_tmpC2A->attributes=0,((_tmpC2A->escapes=0,_tmpC2A)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpC2D;struct Cyc_Absyn_Local_b_struct*_tmpC2C;
struct Cyc_Absyn_Local_b_struct*_tmp4AB=(_tmpC2C=_cycalloc(sizeof(*_tmpC2C)),((
_tmpC2C[0]=((_tmpC2D.tag=4,((_tmpC2D.f1=_tmp4AA,_tmpC2D)))),_tmpC2C)));_tmp49E=
Cyc_Toc_add_varmap(_tmp49D,_tmp49E,_tmp4A9,Cyc_Absyn_var_exp(_tmp4A9,0));{struct
Cyc_Absyn_Exp*_tmp4AC=Cyc_Absyn_varb_exp(_tmp4A5,(void*)_tmp4A7,0);_tmp4AC->topt=
_tmp4A3->topt;{struct Cyc_Absyn_Exp*_tmp4AD=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp4AE=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp4AC,_tmp4AD,0);{
struct Cyc_Core_Opt*_tmpC2E;_tmp4AE->topt=((_tmpC2E=_cycalloc(sizeof(*_tmpC2E)),((
_tmpC2E->v=Cyc_Absyn_sint_typ,_tmpC2E))));}Cyc_Toc_exp_to_c(_tmp49E,_tmp4AE);{
struct Cyc_Absyn_Exp*_tmp4B0=Cyc_Absyn_varb_exp(_tmp4A9,(void*)_tmp4AB,0);_tmp4B0->topt=
_tmp4A3->topt;{struct Cyc_Absyn_Exp*_tmp4B1=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp4B2=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp4B0,_tmp4B1,0);{
struct Cyc_Core_Opt*_tmpC2F;_tmp4B2->topt=((_tmpC2F=_cycalloc(sizeof(*_tmpC2F)),((
_tmpC2F->v=Cyc_Absyn_sint_typ,_tmpC2F))));}Cyc_Toc_exp_to_c(_tmp49E,_tmp4B2);{
struct Cyc_Absyn_Exp*_tmpC30[2];struct Cyc_List_List*_tmp4B4=(_tmpC30[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0),((_tmpC30[0]=Cyc_Absyn_varb_exp(_tmp49A,(void*)
_tmp4A0,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC30,sizeof(struct Cyc_Absyn_Exp*),2)))));struct Cyc_Absyn_Exp*
_tmp4B5=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;xsize=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp4B4,0),
_tmp4B5,0);{struct Cyc_Absyn_Exp*_tmp4B6=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp4AE,_tmp4B2,0),0);struct Cyc_Absyn_Stmt*_tmp4B7=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp4B8=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp49A,(void*)_tmp4A0,0),Cyc_Toc_curr_sp,0);_tmp4B8=Cyc_Toc_cast_it(
_tmp48D,_tmp4B8);{struct Cyc_Absyn_Exp*_tmp4B9=Cyc_Absyn_deref_exp(_tmp4B8,0);
struct Cyc_Absyn_Exp*_tmp4BA=Cyc_Absyn_assign_exp(_tmp4B9,Cyc_Absyn_var_exp(
_tmp4A9,0),0);struct Cyc_Absyn_Stmt*_tmp4BB=Cyc_Absyn_exp_stmt(_tmp4BA,0);_tmp4BB=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp4B6,_tmp4B7,Cyc_Absyn_skip_stmt(
0),0),_tmp4BB,0);{struct Cyc_Absyn_Var_d_struct*_tmpC36;struct Cyc_Absyn_Var_d_struct
_tmpC35;struct Cyc_Absyn_Decl*_tmpC34;_tmp4BB=Cyc_Absyn_decl_stmt(((_tmpC34=
_cycalloc(sizeof(*_tmpC34)),((_tmpC34->r=(void*)((_tmpC36=_cycalloc(sizeof(*
_tmpC36)),((_tmpC36[0]=((_tmpC35.tag=0,((_tmpC35.f1=_tmp4AA,_tmpC35)))),_tmpC36)))),((
_tmpC34->loc=0,_tmpC34)))))),_tmp4BB,0);}{struct Cyc_Absyn_Var_d_struct*_tmpC3C;
struct Cyc_Absyn_Var_d_struct _tmpC3B;struct Cyc_Absyn_Decl*_tmpC3A;_tmp4BB=Cyc_Absyn_decl_stmt(((
_tmpC3A=_cycalloc(sizeof(*_tmpC3A)),((_tmpC3A->r=(void*)((_tmpC3C=_cycalloc(
sizeof(*_tmpC3C)),((_tmpC3C[0]=((_tmpC3B.tag=0,((_tmpC3B.f1=_tmp4A6,_tmpC3B)))),
_tmpC3C)))),((_tmpC3A->loc=0,_tmpC3A)))))),_tmp4BB,0);}{struct Cyc_Absyn_Var_d_struct*
_tmpC42;struct Cyc_Absyn_Var_d_struct _tmpC41;struct Cyc_Absyn_Decl*_tmpC40;_tmp4BB=
Cyc_Absyn_decl_stmt(((_tmpC40=_cycalloc(sizeof(*_tmpC40)),((_tmpC40->r=(void*)((
_tmpC42=_cycalloc(sizeof(*_tmpC42)),((_tmpC42[0]=((_tmpC41.tag=0,((_tmpC41.f1=
_tmp49F,_tmpC41)))),_tmpC42)))),((_tmpC40->loc=0,_tmpC40)))))),_tmp4BB,0);}e->r=
Cyc_Toc_stmt_exp_r(_tmp4BB);};};};};};};};};};};};};};}static void*Cyc_Toc_check_tagged_union(
struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int
in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,
struct Cyc_Position_Segment*)){struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp4D0=Cyc_Tcutil_compress(
aggrtype);struct Cyc_Absyn_AggrInfo _tmp4D2;union Cyc_Absyn_AggrInfoU _tmp4D3;_LL20D: {
struct Cyc_Absyn_AggrType_struct*_tmp4D1=(struct Cyc_Absyn_AggrType_struct*)
_tmp4D0;if(_tmp4D1->tag != 11)goto _LL20F;else{_tmp4D2=_tmp4D1->f1;_tmp4D3=_tmp4D2.aggr_info;}}
_LL20E: ad=Cyc_Absyn_get_known_aggrdecl(_tmp4D3);goto _LL20C;_LL20F:;_LL210: {
struct Cyc_String_pa_struct _tmpC4A;void*_tmpC49[1];const char*_tmpC48;void*_tmpC47;(
_tmpC47=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((
struct _dyneither_ptr)((_tmpC4A.tag=0,((_tmpC4A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpC49[0]=& _tmpC4A,Cyc_aprintf(((
_tmpC48="expecting union but found %s in check_tagged_union",_tag_dyneither(
_tmpC48,sizeof(char),51))),_tag_dyneither(_tmpC49,sizeof(void*),1)))))))),
_tag_dyneither(_tmpC47,sizeof(void*),0)));}_LL20C:;}{struct _tuple0*_tmp4D8=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4D9=Cyc_Absyn_var_exp(_tmp4D8,0);struct Cyc_Absyn_Exp*
_tmp4DA=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);if(in_lhs){
struct Cyc_Absyn_Exp*_tmp4DB=Cyc_Absyn_aggrarrow_exp(_tmp4D9,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4DC=Cyc_Absyn_neq_exp(_tmp4DB,_tmp4DA,0);struct Cyc_Absyn_Exp*
_tmp4DD=Cyc_Absyn_aggrarrow_exp(_tmp4D9,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Stmt*
_tmp4DE=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp4DD,0),0);struct Cyc_Absyn_Stmt*
_tmp4DF=Cyc_Absyn_ifthenelse_stmt(_tmp4DC,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);void*_tmp4E0=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp4E1=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);struct Cyc_Absyn_Stmt*_tmp4E2=
Cyc_Absyn_declare_stmt(_tmp4D8,_tmp4E0,(struct Cyc_Absyn_Exp*)_tmp4E1,Cyc_Absyn_seq_stmt(
_tmp4DF,_tmp4DE,0),0);return Cyc_Toc_stmt_exp_r(_tmp4E2);}else{struct Cyc_Absyn_Exp*
_tmp4E3=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp4D9,f,0),Cyc_Toc_tag_sp,0);struct
Cyc_Absyn_Exp*_tmp4E4=Cyc_Absyn_neq_exp(_tmp4E3,_tmp4DA,0);struct Cyc_Absyn_Exp*
_tmp4E5=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp4D9,f,0),Cyc_Toc_val_sp,0);struct
Cyc_Absyn_Stmt*_tmp4E6=Cyc_Absyn_exp_stmt(_tmp4E5,0);struct Cyc_Absyn_Stmt*
_tmp4E7=Cyc_Absyn_ifthenelse_stmt(_tmp4E4,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp4E8=Cyc_Absyn_declare_stmt(_tmp4D8,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp4E7,_tmp4E6,0),0);return Cyc_Toc_stmt_exp_r(
_tmp4E8);}};}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read){void*_tmp4E9=e->r;struct Cyc_Absyn_Exp*
_tmp4EB;struct Cyc_Absyn_Exp*_tmp4ED;struct _dyneither_ptr*_tmp4EE;int _tmp4EF;int*
_tmp4F0;struct Cyc_Absyn_Exp*_tmp4F2;struct _dyneither_ptr*_tmp4F3;int _tmp4F4;int*
_tmp4F5;_LL212: {struct Cyc_Absyn_Cast_e_struct*_tmp4EA=(struct Cyc_Absyn_Cast_e_struct*)
_tmp4E9;if(_tmp4EA->tag != 15)goto _LL214;else{_tmp4EB=_tmp4EA->f2;}}_LL213: {
const char*_tmpC4D;void*_tmpC4C;(_tmpC4C=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC4D="cast on lhs!",
_tag_dyneither(_tmpC4D,sizeof(char),13))),_tag_dyneither(_tmpC4C,sizeof(void*),0)));}
_LL214: {struct Cyc_Absyn_AggrMember_e_struct*_tmp4EC=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp4E9;if(_tmp4EC->tag != 22)goto _LL216;else{_tmp4ED=_tmp4EC->f1;_tmp4EE=_tmp4EC->f2;
_tmp4EF=_tmp4EC->f4;_tmp4F0=(int*)& _tmp4EC->f4;}}_LL215: {void*_tmp4F8=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4ED->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp4FA;union Cyc_Absyn_AggrInfoU _tmp4FB;_LL21B: {struct Cyc_Absyn_AggrType_struct*
_tmp4F9=(struct Cyc_Absyn_AggrType_struct*)_tmp4F8;if(_tmp4F9->tag != 11)goto
_LL21D;else{_tmp4FA=_tmp4F9->f1;_tmp4FB=_tmp4FA.aggr_info;}}_LL21C: {struct Cyc_Absyn_Aggrdecl*
_tmp4FC=Cyc_Absyn_get_known_aggrdecl(_tmp4FB);*f_tag=Cyc_Toc_get_member_offset(
_tmp4FC,_tmp4EE);{const char*_tmpC52;void*_tmpC51[2];struct Cyc_String_pa_struct
_tmpC50;struct Cyc_String_pa_struct _tmpC4F;struct _dyneither_ptr str=(struct
_dyneither_ptr)((_tmpC4F.tag=0,((_tmpC4F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp4EE),((_tmpC50.tag=0,((_tmpC50.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp4FC->name).f2),((_tmpC51[0]=& _tmpC50,((_tmpC51[1]=&
_tmpC4F,Cyc_aprintf(((_tmpC52="_union_%s_%s",_tag_dyneither(_tmpC52,sizeof(char),
13))),_tag_dyneither(_tmpC51,sizeof(void*),2))))))))))))));{struct _dyneither_ptr*
_tmpC53;*tagged_member_type=Cyc_Absyn_strct(((_tmpC53=_cycalloc(sizeof(*_tmpC53)),((
_tmpC53[0]=str,_tmpC53)))));}if(clear_read)*_tmp4F0=0;return((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4FC->impl))->tagged;};}_LL21D:;_LL21E: return 0;_LL21A:;}_LL216: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp4F1=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp4E9;if(_tmp4F1->tag != 23)goto _LL218;else{_tmp4F2=_tmp4F1->f1;_tmp4F3=_tmp4F1->f2;
_tmp4F4=_tmp4F1->f4;_tmp4F5=(int*)& _tmp4F1->f4;}}_LL217: {void*_tmp502=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4F2->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp504;void*_tmp505;_LL220: {struct Cyc_Absyn_PointerType_struct*_tmp503=(struct
Cyc_Absyn_PointerType_struct*)_tmp502;if(_tmp503->tag != 5)goto _LL222;else{
_tmp504=_tmp503->f1;_tmp505=_tmp504.elt_typ;}}_LL221: {void*_tmp506=Cyc_Tcutil_compress(
_tmp505);struct Cyc_Absyn_AggrInfo _tmp508;union Cyc_Absyn_AggrInfoU _tmp509;_LL225: {
struct Cyc_Absyn_AggrType_struct*_tmp507=(struct Cyc_Absyn_AggrType_struct*)
_tmp506;if(_tmp507->tag != 11)goto _LL227;else{_tmp508=_tmp507->f1;_tmp509=_tmp508.aggr_info;}}
_LL226: {struct Cyc_Absyn_Aggrdecl*_tmp50A=Cyc_Absyn_get_known_aggrdecl(_tmp509);*
f_tag=Cyc_Toc_get_member_offset(_tmp50A,_tmp4F3);{const char*_tmpC58;void*_tmpC57[
2];struct Cyc_String_pa_struct _tmpC56;struct Cyc_String_pa_struct _tmpC55;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpC55.tag=0,((_tmpC55.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp4F3),((_tmpC56.tag=0,((_tmpC56.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp50A->name).f2),((_tmpC57[0]=&
_tmpC56,((_tmpC57[1]=& _tmpC55,Cyc_aprintf(((_tmpC58="_union_%s_%s",
_tag_dyneither(_tmpC58,sizeof(char),13))),_tag_dyneither(_tmpC57,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpC59;*tagged_member_type=Cyc_Absyn_strct(((_tmpC59=
_cycalloc(sizeof(*_tmpC59)),((_tmpC59[0]=str,_tmpC59)))));}if(clear_read)*
_tmp4F5=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp50A->impl))->tagged;};}
_LL227:;_LL228: return 0;_LL224:;}_LL222:;_LL223: return 0;_LL21F:;}_LL218:;_LL219:
return 0;_LL211:;}void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*
e,int tag,void*mem_type){struct _tuple0*_tmp510=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
temp_exp=Cyc_Absyn_var_exp(_tmp510,0);struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(
tag,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,
0),0);struct Cyc_Absyn_Exp*_tmp511=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_ifthenelse_stmt(_tmp511,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp510,Cyc_Absyn_cstar_typ(
mem_type,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(
e)),Cyc_Absyn_seq_stmt(s2,s3,0),0);e->r=Cyc_Toc_stmt_exp_r(s1);}static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type){struct _tuple0*_tmp512=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp512,0);struct Cyc_Absyn_Exp*
temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*
temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 
0)s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*
_tmp513=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp513,
Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=
Cyc_Absyn_declare_stmt(_tmp512,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(e1),Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple21{struct _tuple0*f1;void*f2;struct
Cyc_Absyn_Exp*f3;};struct _tuple22{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*
f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void _tmpCAC(unsigned int*_tmpCAB,unsigned int*_tmpCAA,struct _tuple0***
_tmpCA8){for(*_tmpCAB=0;*_tmpCAB < *_tmpCAA;(*_tmpCAB)++){(*_tmpCA8)[*_tmpCAB]=
Cyc_Toc_temp_var();}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){void*_tmp514=e->r;if(e->topt == 0){const char*_tmpC5D;void*_tmpC5C[1];struct Cyc_String_pa_struct
_tmpC5B;(_tmpC5B.tag=0,((_tmpC5B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpC5C[0]=& _tmpC5B,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC5D="exp_to_c: no type for %s",
_tag_dyneither(_tmpC5D,sizeof(char),25))),_tag_dyneither(_tmpC5C,sizeof(void*),1)))))));}{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp518=
_tmp514;union Cyc_Absyn_Cnst _tmp51A;int _tmp51B;struct _tuple0*_tmp51E;void*_tmp51F;
struct _tuple0*_tmp521;enum Cyc_Absyn_Primop _tmp523;struct Cyc_List_List*_tmp524;
struct Cyc_Absyn_Exp*_tmp526;enum Cyc_Absyn_Incrementor _tmp527;struct Cyc_Absyn_Exp*
_tmp529;struct Cyc_Core_Opt*_tmp52A;struct Cyc_Absyn_Exp*_tmp52B;struct Cyc_Absyn_Exp*
_tmp52D;struct Cyc_Absyn_Exp*_tmp52E;struct Cyc_Absyn_Exp*_tmp52F;struct Cyc_Absyn_Exp*
_tmp531;struct Cyc_Absyn_Exp*_tmp532;struct Cyc_Absyn_Exp*_tmp534;struct Cyc_Absyn_Exp*
_tmp535;struct Cyc_Absyn_Exp*_tmp537;struct Cyc_Absyn_Exp*_tmp538;struct Cyc_Absyn_Exp*
_tmp53A;struct Cyc_List_List*_tmp53B;struct Cyc_Absyn_Exp*_tmp53D;struct Cyc_List_List*
_tmp53E;struct Cyc_Absyn_VarargCallInfo*_tmp53F;struct Cyc_Absyn_Exp*_tmp541;
struct Cyc_List_List*_tmp542;struct Cyc_Absyn_VarargCallInfo*_tmp543;struct Cyc_Absyn_VarargCallInfo
_tmp544;int _tmp545;struct Cyc_List_List*_tmp546;struct Cyc_Absyn_VarargInfo*
_tmp547;struct Cyc_Absyn_Exp*_tmp549;struct Cyc_Absyn_Exp*_tmp54B;struct Cyc_Absyn_Exp*
_tmp54D;struct Cyc_List_List*_tmp54E;void*_tmp550;void**_tmp551;struct Cyc_Absyn_Exp*
_tmp552;int _tmp553;enum Cyc_Absyn_Coercion _tmp554;struct Cyc_Absyn_Exp*_tmp556;
struct Cyc_Absyn_Exp*_tmp558;struct Cyc_Absyn_Exp*_tmp559;struct Cyc_Absyn_Exp*
_tmp55B;void*_tmp55D;void*_tmp55F;void*_tmp560;struct _dyneither_ptr*_tmp562;void*
_tmp564;void*_tmp565;unsigned int _tmp567;struct Cyc_Absyn_Exp*_tmp569;struct Cyc_Absyn_Exp*
_tmp56B;struct _dyneither_ptr*_tmp56C;int _tmp56D;int _tmp56E;struct Cyc_Absyn_Exp*
_tmp570;struct _dyneither_ptr*_tmp571;int _tmp572;int _tmp573;struct Cyc_Absyn_Exp*
_tmp575;struct Cyc_Absyn_Exp*_tmp576;struct Cyc_List_List*_tmp578;struct Cyc_List_List*
_tmp57A;struct Cyc_Absyn_Vardecl*_tmp57C;struct Cyc_Absyn_Exp*_tmp57D;struct Cyc_Absyn_Exp*
_tmp57E;int _tmp57F;struct _tuple0*_tmp581;struct Cyc_List_List*_tmp582;struct Cyc_List_List*
_tmp583;struct Cyc_Absyn_Aggrdecl*_tmp584;void*_tmp586;struct Cyc_List_List*
_tmp587;struct Cyc_List_List*_tmp589;struct Cyc_Absyn_Datatypedecl*_tmp58A;struct
Cyc_Absyn_Datatypefield*_tmp58B;struct Cyc_Absyn_MallocInfo _tmp58F;int _tmp590;
struct Cyc_Absyn_Exp*_tmp591;void**_tmp592;struct Cyc_Absyn_Exp*_tmp593;int _tmp594;
struct Cyc_Absyn_Exp*_tmp596;struct Cyc_Absyn_Exp*_tmp597;struct Cyc_Absyn_Exp*
_tmp599;struct _dyneither_ptr*_tmp59A;struct Cyc_Absyn_Stmt*_tmp59C;_LL22A: {
struct Cyc_Absyn_Const_e_struct*_tmp519=(struct Cyc_Absyn_Const_e_struct*)_tmp518;
if(_tmp519->tag != 0)goto _LL22C;else{_tmp51A=_tmp519->f1;if((_tmp51A.Null_c).tag
!= 1)goto _LL22C;_tmp51B=(int)(_tmp51A.Null_c).val;}}_LL22B: {struct Cyc_Absyn_Exp*
_tmp5A1=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp5A1,
_tmp5A1))->r;else{struct Cyc_Absyn_Exp*_tmpC5E[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((
_tmpC5E[2]=_tmp5A1,((_tmpC5E[1]=_tmp5A1,((_tmpC5E[0]=_tmp5A1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5E,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL229;}_LL22C: {struct Cyc_Absyn_Const_e_struct*
_tmp51C=(struct Cyc_Absyn_Const_e_struct*)_tmp518;if(_tmp51C->tag != 0)goto _LL22E;}
_LL22D: goto _LL229;_LL22E: {struct Cyc_Absyn_Var_e_struct*_tmp51D=(struct Cyc_Absyn_Var_e_struct*)
_tmp518;if(_tmp51D->tag != 1)goto _LL230;else{_tmp51E=_tmp51D->f1;_tmp51F=(void*)
_tmp51D->f2;}}_LL22F:{struct _handler_cons _tmp5A3;_push_handler(& _tmp5A3);{int
_tmp5A5=0;if(setjmp(_tmp5A3.handler))_tmp5A5=1;if(!_tmp5A5){e->r=(Cyc_Toc_lookup_varmap(
nv,_tmp51E))->r;;_pop_handler();}else{void*_tmp5A4=(void*)_exn_thrown;void*
_tmp5A7=_tmp5A4;_LL283: {struct Cyc_Dict_Absent_struct*_tmp5A8=(struct Cyc_Dict_Absent_struct*)
_tmp5A7;if(_tmp5A8->tag != Cyc_Dict_Absent)goto _LL285;}_LL284: {const char*_tmpC62;
void*_tmpC61[1];struct Cyc_String_pa_struct _tmpC60;(_tmpC60.tag=0,((_tmpC60.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp51E)),((
_tmpC61[0]=& _tmpC60,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC62="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpC62,sizeof(char),
32))),_tag_dyneither(_tmpC61,sizeof(void*),1)))))));}_LL285:;_LL286:(void)_throw(
_tmp5A7);_LL282:;}};}goto _LL229;_LL230: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp520=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp518;if(_tmp520->tag != 2)goto
_LL232;else{_tmp521=_tmp520->f1;}}_LL231: {const char*_tmpC65;void*_tmpC64;(
_tmpC64=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC65="unknownid",_tag_dyneither(_tmpC65,sizeof(char),10))),_tag_dyneither(
_tmpC64,sizeof(void*),0)));}_LL232: {struct Cyc_Absyn_Primop_e_struct*_tmp522=(
struct Cyc_Absyn_Primop_e_struct*)_tmp518;if(_tmp522->tag != 3)goto _LL234;else{
_tmp523=_tmp522->f1;_tmp524=_tmp522->f2;}}_LL233: {struct Cyc_List_List*_tmp5AE=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp524);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp524);switch(_tmp523){case Cyc_Absyn_Numelts: _LL287: {
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp524))->hd;{void*_tmp5AF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo _tmp5B1;struct Cyc_Absyn_Exp*
_tmp5B2;struct Cyc_Absyn_PtrInfo _tmp5B4;void*_tmp5B5;struct Cyc_Absyn_PtrAtts
_tmp5B6;union Cyc_Absyn_Constraint*_tmp5B7;union Cyc_Absyn_Constraint*_tmp5B8;
union Cyc_Absyn_Constraint*_tmp5B9;_LL28A: {struct Cyc_Absyn_ArrayType_struct*
_tmp5B0=(struct Cyc_Absyn_ArrayType_struct*)_tmp5AF;if(_tmp5B0->tag != 8)goto
_LL28C;else{_tmp5B1=_tmp5B0->f1;_tmp5B2=_tmp5B1.num_elts;}}_LL28B: if(!Cyc_Evexp_c_can_eval((
struct Cyc_Absyn_Exp*)_check_null(_tmp5B2))){const char*_tmpC68;void*_tmpC67;(
_tmpC67=0,Cyc_Tcutil_terr(e->loc,((_tmpC68="can't calculate numelts",
_tag_dyneither(_tmpC68,sizeof(char),24))),_tag_dyneither(_tmpC67,sizeof(void*),0)));}
e->r=_tmp5B2->r;goto _LL289;_LL28C: {struct Cyc_Absyn_PointerType_struct*_tmp5B3=(
struct Cyc_Absyn_PointerType_struct*)_tmp5AF;if(_tmp5B3->tag != 5)goto _LL28E;else{
_tmp5B4=_tmp5B3->f1;_tmp5B5=_tmp5B4.elt_typ;_tmp5B6=_tmp5B4.ptr_atts;_tmp5B7=
_tmp5B6.nullable;_tmp5B8=_tmp5B6.bounds;_tmp5B9=_tmp5B6.zero_term;}}_LL28D:{void*
_tmp5BC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp5B8);struct Cyc_Absyn_Exp*_tmp5BF;_LL291: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5BD=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5BC;if(_tmp5BD->tag != 0)goto
_LL293;}_LL292:{struct Cyc_Absyn_Exp*_tmpC69[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,((
_tmpC69[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5B5),0),((_tmpC69[0]=(
struct Cyc_Absyn_Exp*)_tmp524->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC69,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL290;_LL293: {struct Cyc_Absyn_Upper_b_struct*_tmp5BE=(struct Cyc_Absyn_Upper_b_struct*)
_tmp5BC;if(_tmp5BE->tag != 1)goto _LL290;else{_tmp5BF=_tmp5BE->f1;}}_LL294: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5B9)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp524->hd);struct Cyc_Absyn_Exp*_tmpC6A[2];e->r=Cyc_Toc_fncall_exp_r(
function_e,((_tmpC6A[1]=_tmp5BF,((_tmpC6A[0]=(struct Cyc_Absyn_Exp*)_tmp524->hd,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC6A,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp5B7)){if(!Cyc_Evexp_c_can_eval(_tmp5BF)){const
char*_tmpC6D;void*_tmpC6C;(_tmpC6C=0,Cyc_Tcutil_terr(e->loc,((_tmpC6D="can't calculate numelts",
_tag_dyneither(_tmpC6D,sizeof(char),24))),_tag_dyneither(_tmpC6C,sizeof(void*),0)));}
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp5BF,Cyc_Absyn_uint_exp(0,0));}else{e->r=
_tmp5BF->r;goto _LL290;}}goto _LL290;_LL290:;}goto _LL289;_LL28E:;_LL28F: {const
char*_tmpC72;void*_tmpC71[2];struct Cyc_String_pa_struct _tmpC70;struct Cyc_String_pa_struct
_tmpC6F;(_tmpC6F.tag=0,((_tmpC6F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((
_tmpC70.tag=0,((_tmpC70.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmpC71[0]=& _tmpC70,((
_tmpC71[1]=& _tmpC6F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC72="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpC72,
sizeof(char),41))),_tag_dyneither(_tmpC71,sizeof(void*),2)))))))))))));}_LL289:;}
break;}case Cyc_Absyn_Plus: _LL288:{void*_tmp5C8=Cyc_Tcutil_compress((void*)((
struct Cyc_List_List*)_check_null(_tmp5AE))->hd);struct Cyc_Absyn_PtrInfo _tmp5CA;
void*_tmp5CB;struct Cyc_Absyn_PtrAtts _tmp5CC;union Cyc_Absyn_Constraint*_tmp5CD;
union Cyc_Absyn_Constraint*_tmp5CE;_LL297: {struct Cyc_Absyn_PointerType_struct*
_tmp5C9=(struct Cyc_Absyn_PointerType_struct*)_tmp5C8;if(_tmp5C9->tag != 5)goto
_LL299;else{_tmp5CA=_tmp5C9->f1;_tmp5CB=_tmp5CA.elt_typ;_tmp5CC=_tmp5CA.ptr_atts;
_tmp5CD=_tmp5CC.bounds;_tmp5CE=_tmp5CC.zero_term;}}_LL298:{void*_tmp5CF=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp5CD);struct Cyc_Absyn_Exp*_tmp5D2;_LL29C: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5D0=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5CF;if(_tmp5D0->tag != 0)goto
_LL29E;}_LL29D: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp524))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp524->tl))->hd;{struct Cyc_Absyn_Exp*_tmpC73[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpC73[2]=e2,((_tmpC73[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5CB),0),((_tmpC73[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC73,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}goto _LL29B;}_LL29E: {struct Cyc_Absyn_Upper_b_struct*_tmp5D1=(struct
Cyc_Absyn_Upper_b_struct*)_tmp5CF;if(_tmp5D1->tag != 1)goto _LL29B;else{_tmp5D2=
_tmp5D1->f1;}}_LL29F: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp5CE)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp524))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp524->tl))->hd;struct Cyc_Absyn_Exp*_tmpC74[3];e->r=(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
e1),((_tmpC74[2]=e2,((_tmpC74[1]=_tmp5D2,((_tmpC74[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC74,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r;}goto _LL29B;_LL29B:;}goto _LL296;_LL299:;_LL29A: goto _LL296;
_LL296:;}break;case Cyc_Absyn_Minus: _LL295: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp5AE))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp524))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp524->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp5AE->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{struct Cyc_Absyn_Exp*_tmpC75[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpC75[2]=Cyc_Absyn_prim1_exp(
Cyc_Absyn_Minus,e2,0),((_tmpC75[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpC75[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC75,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}break;}case Cyc_Absyn_Eq:
_LL2A0: goto _LL2A1;case Cyc_Absyn_Neq: _LL2A1: goto _LL2A2;case Cyc_Absyn_Gt: _LL2A2:
goto _LL2A3;case Cyc_Absyn_Gte: _LL2A3: goto _LL2A4;case Cyc_Absyn_Lt: _LL2A4: goto
_LL2A5;case Cyc_Absyn_Lte: _LL2A5: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp524))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp524->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp5AE))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp5AE->tl))->hd;void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& elt_typ))e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,
0));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp,0));break;}default: _LL2A6: break;}goto _LL229;}_LL234: {
struct Cyc_Absyn_Increment_e_struct*_tmp525=(struct Cyc_Absyn_Increment_e_struct*)
_tmp518;if(_tmp525->tag != 5)goto _LL236;else{_tmp526=_tmp525->f1;_tmp527=_tmp525->f2;}}
_LL235: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp526->topt))->v;
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;const char*_tmpC76;struct _dyneither_ptr incr_str=(_tmpC76="increment",
_tag_dyneither(_tmpC76,sizeof(char),10));if(_tmp527 == Cyc_Absyn_PreDec  || 
_tmp527 == Cyc_Absyn_PostDec){const char*_tmpC77;incr_str=((_tmpC77="decrement",
_tag_dyneither(_tmpC77,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp526,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpC7B;void*_tmpC7A[1];
struct Cyc_String_pa_struct _tmpC79;(_tmpC79.tag=0,((_tmpC79.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC7A[0]=& _tmpC79,Cyc_Tcutil_terr(
e->loc,((_tmpC7B="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpC7B,sizeof(char),74))),_tag_dyneither(_tmpC7A,sizeof(void*),1)))))));}{
const char*_tmpC7E;void*_tmpC7D;(_tmpC7D=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC7E="in-place inc/dec on zero-term",
_tag_dyneither(_tmpC7E,sizeof(char),30))),_tag_dyneither(_tmpC7D,sizeof(void*),0)));};}{
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp526,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp5DC=Cyc_Absyn_signed_int_exp(
1,0);{struct Cyc_Core_Opt*_tmpC7F;_tmp5DC->topt=((_tmpC7F=_cycalloc(sizeof(*
_tmpC7F)),((_tmpC7F->v=Cyc_Absyn_sint_typ,_tmpC7F))));}switch(_tmp527){case Cyc_Absyn_PreInc:
_LL2A8:{struct Cyc_Absyn_AssignOp_e_struct _tmpC85;struct Cyc_Core_Opt*_tmpC84;
struct Cyc_Absyn_AssignOp_e_struct*_tmpC83;e->r=(void*)((_tmpC83=_cycalloc(
sizeof(*_tmpC83)),((_tmpC83[0]=((_tmpC85.tag=4,((_tmpC85.f1=_tmp526,((_tmpC85.f2=((
_tmpC84=_cycalloc_atomic(sizeof(*_tmpC84)),((_tmpC84->v=(void*)Cyc_Absyn_Plus,
_tmpC84)))),((_tmpC85.f3=_tmp5DC,_tmpC85)))))))),_tmpC83))));}Cyc_Toc_exp_to_c(
nv,e);return;case Cyc_Absyn_PreDec: _LL2A9:{struct Cyc_Absyn_AssignOp_e_struct
_tmpC8B;struct Cyc_Core_Opt*_tmpC8A;struct Cyc_Absyn_AssignOp_e_struct*_tmpC89;e->r=(
void*)((_tmpC89=_cycalloc(sizeof(*_tmpC89)),((_tmpC89[0]=((_tmpC8B.tag=4,((
_tmpC8B.f1=_tmp526,((_tmpC8B.f2=((_tmpC8A=_cycalloc_atomic(sizeof(*_tmpC8A)),((
_tmpC8A->v=(void*)Cyc_Absyn_Minus,_tmpC8A)))),((_tmpC8B.f3=_tmp5DC,_tmpC8B)))))))),
_tmpC89))));}Cyc_Toc_exp_to_c(nv,e);return;default: _LL2AA:{const char*_tmpC8F;
void*_tmpC8E[1];struct Cyc_String_pa_struct _tmpC8D;(_tmpC8D.tag=0,((_tmpC8D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC8E[0]=& _tmpC8D,Cyc_Tcutil_terr(
e->loc,((_tmpC8F="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpC8F,sizeof(char),59))),_tag_dyneither(_tmpC8E,sizeof(void*),1)))))));}{
const char*_tmpC92;void*_tmpC91;(_tmpC91=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC92="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpC92,sizeof(char),34))),_tag_dyneither(_tmpC91,sizeof(void*),0)));};}}
Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp526);Cyc_Toc_set_lhs(nv,0);{void*
elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp527 == Cyc_Absyn_PostInc
 || _tmp527 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp527 == Cyc_Absyn_PreDec  || _tmp527 == Cyc_Absyn_PostDec)change=- 1;{struct
Cyc_Absyn_Exp*_tmpC93[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC93[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpC93[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpC93[0]=Cyc_Toc_push_address_exp(_tmp526),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC93,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Toc_functionSet*_tmp5EA=_tmp527 == Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:&
Cyc_Toc__zero_arr_inplace_plus_post_functionSet;struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(
_tmp5EA,_tmp526);struct Cyc_Absyn_Exp*_tmpC94[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((
_tmpC94[1]=Cyc_Absyn_signed_int_exp(1,0),((_tmpC94[0]=Cyc_Toc_push_address_exp(
_tmp526),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC94,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(elt_typ == (
void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp526)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(
_tmp526,0,Cyc_Toc_incr_lvalue,_tmp527);e->r=_tmp526->r;}}}goto _LL229;};};}_LL236: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp528=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp518;if(_tmp528->tag != 4)goto _LL238;else{_tmp529=_tmp528->f1;_tmp52A=_tmp528->f2;
_tmp52B=_tmp528->f3;}}_LL237: {void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp529->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp52B->topt))->v;int f_tag=0;void*tagged_member_struct_type=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Toc_is_tagged_union_project(_tmp529,& f_tag,&
tagged_member_struct_type,1)){Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp529);
Cyc_Toc_set_lhs(nv,0);Cyc_Toc_exp_to_c(nv,_tmp52B);e->r=Cyc_Toc_tagged_union_assignop(
_tmp529,e1_old_typ,_tmp52A,_tmp52B,e2_old_typ,f_tag,tagged_member_struct_type);
return;}{void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(_tmp529,& ptr_type,&
is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp529,_tmp52A,
_tmp52B,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp529);Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp529);Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp52B);{int done=0;if(_tmp52A != 0){void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;switch((enum Cyc_Absyn_Primop)_tmp52A->v){case Cyc_Absyn_Plus: _LL2AC:
change=_tmp52B;break;case Cyc_Absyn_Minus: _LL2AD: change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,
_tmp52B,0);break;default: _LL2AE: {const char*_tmpC97;void*_tmpC96;(_tmpC96=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC97="bad t ? pointer arithmetic",_tag_dyneither(_tmpC97,sizeof(char),27))),
_tag_dyneither(_tmpC96,sizeof(void*),0)));}}done=1;{struct Cyc_Absyn_Exp*_tmp5EF=
Cyc_Toc__dyneither_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*_tmpC98[3];e->r=Cyc_Toc_fncall_exp_r(
_tmp5EF,((_tmpC98[2]=change,((_tmpC98[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpC98[0]=Cyc_Toc_push_address_exp(_tmp529),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC98,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))
switch((enum Cyc_Absyn_Primop)_tmp52A->v){case Cyc_Absyn_Plus: _LL2B0: done=1;{
struct Cyc_Absyn_Exp*_tmpC99[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp529),((_tmpC99[1]=_tmp52B,((
_tmpC99[0]=_tmp529,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC99,sizeof(struct Cyc_Absyn_Exp*),2)))))));}break;default:
_LL2B1: {const char*_tmpC9C;void*_tmpC9B;(_tmpC9B=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC9C="bad zero-terminated pointer arithmetic",
_tag_dyneither(_tmpC9C,sizeof(char),39))),_tag_dyneither(_tmpC9B,sizeof(void*),0)));}}}}
if(!done){if(e1_poly)_tmp52B->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp52B->r,0));if(!Cyc_Absyn_is_lvalue(_tmp529)){{struct _tuple20 _tmpC9F;struct
_tuple20*_tmpC9E;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple20*),struct _tuple20*f_env))
Cyc_Toc_lvalue_assign)(_tmp529,0,Cyc_Toc_assignop_lvalue,((_tmpC9E=_cycalloc(
sizeof(struct _tuple20)* 1),((_tmpC9E[0]=((_tmpC9F.f1=_tmp52A,((_tmpC9F.f2=
_tmp52B,_tmpC9F)))),_tmpC9E)))));}e->r=_tmp529->r;}}goto _LL229;};};};}_LL238: {
struct Cyc_Absyn_Conditional_e_struct*_tmp52C=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp518;if(_tmp52C->tag != 6)goto _LL23A;else{_tmp52D=_tmp52C->f1;_tmp52E=_tmp52C->f2;
_tmp52F=_tmp52C->f3;}}_LL239: Cyc_Toc_exp_to_c(nv,_tmp52D);Cyc_Toc_exp_to_c(nv,
_tmp52E);Cyc_Toc_exp_to_c(nv,_tmp52F);goto _LL229;_LL23A: {struct Cyc_Absyn_And_e_struct*
_tmp530=(struct Cyc_Absyn_And_e_struct*)_tmp518;if(_tmp530->tag != 7)goto _LL23C;
else{_tmp531=_tmp530->f1;_tmp532=_tmp530->f2;}}_LL23B: Cyc_Toc_exp_to_c(nv,
_tmp531);Cyc_Toc_exp_to_c(nv,_tmp532);goto _LL229;_LL23C: {struct Cyc_Absyn_Or_e_struct*
_tmp533=(struct Cyc_Absyn_Or_e_struct*)_tmp518;if(_tmp533->tag != 8)goto _LL23E;
else{_tmp534=_tmp533->f1;_tmp535=_tmp533->f2;}}_LL23D: Cyc_Toc_exp_to_c(nv,
_tmp534);Cyc_Toc_exp_to_c(nv,_tmp535);goto _LL229;_LL23E: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp536=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp518;if(_tmp536->tag != 9)goto _LL240;
else{_tmp537=_tmp536->f1;_tmp538=_tmp536->f2;}}_LL23F: Cyc_Toc_exp_to_c(nv,
_tmp537);Cyc_Toc_exp_to_c(nv,_tmp538);goto _LL229;_LL240: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp539=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp518;if(_tmp539->tag != 10)goto
_LL242;else{_tmp53A=_tmp539->f1;_tmp53B=_tmp539->f2;}}_LL241: _tmp53D=_tmp53A;
_tmp53E=_tmp53B;goto _LL243;_LL242: {struct Cyc_Absyn_FnCall_e_struct*_tmp53C=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp518;if(_tmp53C->tag != 11)goto _LL244;else{
_tmp53D=_tmp53C->f1;_tmp53E=_tmp53C->f2;_tmp53F=_tmp53C->f3;if(_tmp53F != 0)goto
_LL244;}}_LL243: Cyc_Toc_exp_to_c(nv,_tmp53D);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp53E);goto _LL229;_LL244: {struct Cyc_Absyn_FnCall_e_struct*
_tmp540=(struct Cyc_Absyn_FnCall_e_struct*)_tmp518;if(_tmp540->tag != 11)goto
_LL246;else{_tmp541=_tmp540->f1;_tmp542=_tmp540->f2;_tmp543=_tmp540->f3;if(
_tmp543 == 0)goto _LL246;_tmp544=*_tmp543;_tmp545=_tmp544.num_varargs;_tmp546=
_tmp544.injectors;_tmp547=_tmp544.vai;}}_LL245: {struct Cyc_Toc_Env _tmp5F7;struct
_RegionHandle*_tmp5F8;struct Cyc_Toc_Env*_tmp5F6=nv;_tmp5F7=*_tmp5F6;_tmp5F8=
_tmp5F7.rgn;{struct _tuple0*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=
Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp545,0);void*cva_type=Cyc_Toc_typ_to_c(_tmp547->type);void*
arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp542);int num_normargs=num_args - _tmp545;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp542=_tmp542->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp542))->hd);{
struct Cyc_List_List*_tmpCA0;new_args=((_tmpCA0=_cycalloc(sizeof(*_tmpCA0)),((
_tmpCA0->hd=(struct Cyc_Absyn_Exp*)_tmp542->hd,((_tmpCA0->tl=new_args,_tmpCA0))))));};}}{
struct Cyc_Absyn_Exp*_tmpCA3[3];struct Cyc_List_List*_tmpCA2;new_args=((_tmpCA2=
_cycalloc(sizeof(*_tmpCA2)),((_tmpCA2->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((
_tmpCA3[2]=num_varargs_exp,((_tmpCA3[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0),((
_tmpCA3[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCA3,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpCA2->tl=
new_args,_tmpCA2))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp541);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp541,new_args,0),0);if(_tmp547->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp5FC=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp547->type));struct Cyc_Absyn_DatatypeInfo _tmp5FE;union Cyc_Absyn_DatatypeInfoU
_tmp5FF;struct Cyc_Absyn_Datatypedecl**_tmp600;struct Cyc_Absyn_Datatypedecl*
_tmp601;_LL2B4: {struct Cyc_Absyn_DatatypeType_struct*_tmp5FD=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp5FC;if(_tmp5FD->tag != 3)goto _LL2B6;else{_tmp5FE=_tmp5FD->f1;_tmp5FF=_tmp5FE.datatype_info;
if((_tmp5FF.KnownDatatype).tag != 2)goto _LL2B6;_tmp600=(struct Cyc_Absyn_Datatypedecl**)(
_tmp5FF.KnownDatatype).val;_tmp601=*_tmp600;}}_LL2B5: tud=_tmp601;goto _LL2B3;
_LL2B6:;_LL2B7: {const char*_tmpCA6;void*_tmpCA5;(_tmpCA5=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA6="toc: unknown datatype in vararg with inject",
_tag_dyneither(_tmpCA6,sizeof(char),44))),_tag_dyneither(_tmpCA5,sizeof(void*),0)));}
_LL2B3:;}{unsigned int _tmpCAB;unsigned int _tmpCAA;struct _dyneither_ptr _tmpCA9;
struct _tuple0**_tmpCA8;unsigned int _tmpCA7;struct _dyneither_ptr vs=(_tmpCA7=(
unsigned int)_tmp545,((_tmpCA8=(struct _tuple0**)_region_malloc(_tmp5F8,
_check_times(sizeof(struct _tuple0*),_tmpCA7)),((_tmpCA9=_tag_dyneither(_tmpCA8,
sizeof(struct _tuple0*),_tmpCA7),((((_tmpCAA=_tmpCA7,_tmpCAC(& _tmpCAB,& _tmpCAA,&
_tmpCA8))),_tmpCA9)))))));if(_tmp545 != 0){struct Cyc_List_List*_tmp604=0;{int i=
_tmp545 - 1;for(0;i >= 0;-- i){struct Cyc_List_List*_tmpCAD;_tmp604=((_tmpCAD=
_cycalloc(sizeof(*_tmpCAD)),((_tmpCAD->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct
_tuple0*),i)),0),0)),((_tmpCAD->tl=_tmp604,_tmpCAD))))));}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp604,0),s,0);{
int i=0;for(0;_tmp542 != 0;(((_tmp542=_tmp542->tl,_tmp546=_tmp546->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp542->hd;void*arg_type=(void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct _tuple0*var=*((struct
_tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield _tmp607;struct
_tuple0*_tmp608;struct Cyc_List_List*_tmp609;struct Cyc_Absyn_Datatypefield*
_tmp606=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp546))->hd;_tmp607=*_tmp606;_tmp608=_tmp607.name;_tmp609=_tmp607.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp609))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_boxed_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp608),0),s,0);{const char*_tmpCAE;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp608,((_tmpCAE="_struct",_tag_dyneither(_tmpCAE,
sizeof(char),8))))),0,s,0);};};}};}else{struct _tuple16*_tmpCAF[3];struct Cyc_List_List*
_tmp60B=(_tmpCAF[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpCAF[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpCAF[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCAF,sizeof(struct _tuple16*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{{int i=0;for(0;_tmp542
!= 0;(_tmp542=_tmp542->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp542->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp542->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL229;}_LL246: {struct Cyc_Absyn_Throw_e_struct*_tmp548=(struct Cyc_Absyn_Throw_e_struct*)
_tmp518;if(_tmp548->tag != 12)goto _LL248;else{_tmp549=_tmp548->f1;}}_LL247: Cyc_Toc_exp_to_c(
nv,_tmp549);e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(
_tmp549),0))->r;goto _LL229;_LL248: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp54A=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp518;if(_tmp54A->tag != 13)
goto _LL24A;else{_tmp54B=_tmp54A->f1;}}_LL249: Cyc_Toc_exp_to_c(nv,_tmp54B);goto
_LL229;_LL24A: {struct Cyc_Absyn_Instantiate_e_struct*_tmp54C=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp518;if(_tmp54C->tag != 14)goto _LL24C;else{_tmp54D=_tmp54C->f1;_tmp54E=_tmp54C->f2;}}
_LL24B: Cyc_Toc_exp_to_c(nv,_tmp54D);for(0;_tmp54E != 0;_tmp54E=_tmp54E->tl){enum 
Cyc_Absyn_KindQual _tmp611=(Cyc_Tcutil_typ_kind((void*)_tmp54E->hd))->kind;if(
_tmp611 != Cyc_Absyn_EffKind  && _tmp611 != Cyc_Absyn_RgnKind){{void*_tmp612=Cyc_Tcutil_compress((
void*)_tmp54E->hd);_LL2B9: {struct Cyc_Absyn_VarType_struct*_tmp613=(struct Cyc_Absyn_VarType_struct*)
_tmp612;if(_tmp613->tag != 2)goto _LL2BB;}_LL2BA: goto _LL2BC;_LL2BB: {struct Cyc_Absyn_DatatypeType_struct*
_tmp614=(struct Cyc_Absyn_DatatypeType_struct*)_tmp612;if(_tmp614->tag != 3)goto
_LL2BD;}_LL2BC: continue;_LL2BD:;_LL2BE: e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp54D,0))->r;goto _LL2B8;
_LL2B8:;}break;}}goto _LL229;_LL24C: {struct Cyc_Absyn_Cast_e_struct*_tmp54F=(
struct Cyc_Absyn_Cast_e_struct*)_tmp518;if(_tmp54F->tag != 15)goto _LL24E;else{
_tmp550=(void**)& _tmp54F->f1;_tmp551=(void**)((void**)& _tmp54F->f1);_tmp552=
_tmp54F->f2;_tmp553=_tmp54F->f3;_tmp554=_tmp54F->f4;}}_LL24D: {void*old_t2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp552->topt))->v;void*new_typ=*_tmp551;*
_tmp551=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp552);{struct _tuple19
_tmpCB0;struct _tuple19 _tmp616=(_tmpCB0.f1=Cyc_Tcutil_compress(old_t2),((_tmpCB0.f2=
Cyc_Tcutil_compress(new_typ),_tmpCB0)));void*_tmp617;struct Cyc_Absyn_PtrInfo
_tmp619;void*_tmp61A;struct Cyc_Absyn_PtrInfo _tmp61C;void*_tmp61D;struct Cyc_Absyn_PtrInfo
_tmp61F;void*_tmp620;_LL2C0: _tmp617=_tmp616.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp618=(struct Cyc_Absyn_PointerType_struct*)_tmp617;if(_tmp618->tag != 5)goto
_LL2C2;else{_tmp619=_tmp618->f1;}};_tmp61A=_tmp616.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp61B=(struct Cyc_Absyn_PointerType_struct*)_tmp61A;if(_tmp61B->tag != 5)goto
_LL2C2;else{_tmp61C=_tmp61B->f1;}};_LL2C1: {int _tmp622=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp619.ptr_atts).nullable);int _tmp623=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp61C.ptr_atts).nullable);
void*_tmp624=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp619.ptr_atts).bounds);void*_tmp625=((void*(*)(void*y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp61C.ptr_atts).bounds);
int _tmp626=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp619.ptr_atts).zero_term);int _tmp627=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp61C.ptr_atts).zero_term);{struct _tuple19 _tmpCB1;
struct _tuple19 _tmp629=(_tmpCB1.f1=_tmp624,((_tmpCB1.f2=_tmp625,_tmpCB1)));void*
_tmp62A;struct Cyc_Absyn_Exp*_tmp62C;void*_tmp62D;struct Cyc_Absyn_Exp*_tmp62F;
void*_tmp630;struct Cyc_Absyn_Exp*_tmp632;void*_tmp633;void*_tmp635;void*_tmp637;
struct Cyc_Absyn_Exp*_tmp639;void*_tmp63A;void*_tmp63C;_LL2C7: _tmp62A=_tmp629.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp62B=(struct Cyc_Absyn_Upper_b_struct*)_tmp62A;
if(_tmp62B->tag != 1)goto _LL2C9;else{_tmp62C=_tmp62B->f1;}};_tmp62D=_tmp629.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp62E=(struct Cyc_Absyn_Upper_b_struct*)_tmp62D;
if(_tmp62E->tag != 1)goto _LL2C9;else{_tmp62F=_tmp62E->f1;}};_LL2C8: if((!Cyc_Evexp_c_can_eval(
_tmp62C) || !Cyc_Evexp_c_can_eval(_tmp62F)) && !Cyc_Evexp_same_const_exp(_tmp62C,
_tmp62F)){const char*_tmpCB4;void*_tmpCB3;(_tmpCB3=0,Cyc_Tcutil_terr(e->loc,((
_tmpCB4="can't validate cast due to potential size differences",_tag_dyneither(
_tmpCB4,sizeof(char),54))),_tag_dyneither(_tmpCB3,sizeof(void*),0)));}if(_tmp622
 && !_tmp623){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpCB7;void*_tmpCB6;(
_tmpCB6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpCB7="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpCB7,
sizeof(char),44))),_tag_dyneither(_tmpCB6,sizeof(void*),0)));}if(_tmp554 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpCBB;void*_tmpCBA[1];struct Cyc_String_pa_struct _tmpCB9;(_tmpCB9.tag=
0,((_tmpCB9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpCBA[0]=& _tmpCB9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpCBB="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpCBB,sizeof(char),41))),_tag_dyneither(_tmpCBA,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp552);if(do_null_check){if(!_tmp553){
const char*_tmpCBE;void*_tmpCBD;(_tmpCBD=0,Cyc_Tcutil_warn(e->loc,((_tmpCBE="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpCBE,sizeof(char),58))),_tag_dyneither(_tmpCBD,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpCBF;e->r=Cyc_Toc_cast_it_r(*_tmp551,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpCBF=_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF->hd=
_tmp552,((_tmpCBF->tl=0,_tmpCBF)))))),0));};}};}goto _LL2C6;_LL2C9: _tmp630=
_tmp629.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp631=(struct Cyc_Absyn_Upper_b_struct*)
_tmp630;if(_tmp631->tag != 1)goto _LL2CB;else{_tmp632=_tmp631->f1;}};_tmp633=
_tmp629.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp634=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp633;if(_tmp634->tag != 0)goto _LL2CB;};_LL2CA: if(!Cyc_Evexp_c_can_eval(_tmp632)){
const char*_tmpCC2;void*_tmpCC1;(_tmpCC1=0,Cyc_Tcutil_terr(e->loc,((_tmpCC2="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpCC2,sizeof(char),71))),_tag_dyneither(_tmpCC1,sizeof(void*),0)));}
if(_tmp554 == Cyc_Absyn_NonNull_to_Null){const char*_tmpCC6;void*_tmpCC5[1];struct
Cyc_String_pa_struct _tmpCC4;(_tmpCC4.tag=0,((_tmpCC4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCC5[0]=& _tmpCC4,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCC6="conversion mis-classified as null-check: %s",
_tag_dyneither(_tmpCC6,sizeof(char),44))),_tag_dyneither(_tmpCC5,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){if((_tmp626  && !(_tmp61C.elt_tq).real_const) && !
_tmp627)_tmp632=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp632,Cyc_Absyn_uint_exp(1,
0),0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp632,_tmp552))->r;}else{
struct Cyc_Absyn_Exp*_tmp64D=Cyc_Toc__tag_dyneither_e;if(_tmp626){struct _tuple0*
_tmp64E=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp64F=Cyc_Absyn_var_exp(_tmp64E,
0);struct Cyc_Absyn_Exp*arg3;{void*_tmp650=_tmp552->r;union Cyc_Absyn_Cnst _tmp652;
struct _dyneither_ptr _tmp653;union Cyc_Absyn_Cnst _tmp655;struct _dyneither_ptr
_tmp656;_LL2D0: {struct Cyc_Absyn_Const_e_struct*_tmp651=(struct Cyc_Absyn_Const_e_struct*)
_tmp650;if(_tmp651->tag != 0)goto _LL2D2;else{_tmp652=_tmp651->f1;if((_tmp652.String_c).tag
!= 8)goto _LL2D2;_tmp653=(struct _dyneither_ptr)(_tmp652.String_c).val;}}_LL2D1:
arg3=_tmp632;goto _LL2CF;_LL2D2: {struct Cyc_Absyn_Const_e_struct*_tmp654=(struct
Cyc_Absyn_Const_e_struct*)_tmp650;if(_tmp654->tag != 0)goto _LL2D4;else{_tmp655=
_tmp654->f1;if((_tmp655.Wstring_c).tag != 9)goto _LL2D4;_tmp656=(struct
_dyneither_ptr)(_tmp655.Wstring_c).val;}}_LL2D3: arg3=_tmp632;goto _LL2CF;_LL2D4:;
_LL2D5:{struct Cyc_Absyn_Exp*_tmpCC7[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp552),((_tmpCC7[1]=_tmp632,((_tmpCC7[0]=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp64F),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCC7,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);}goto _LL2CF;_LL2CF:;}if(!_tmp627  && !(_tmp61C.elt_tq).real_const)arg3=
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*
_tmp658=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp61C.elt_typ),0);struct Cyc_Absyn_Exp*
_tmpCC8[3];struct Cyc_Absyn_Exp*_tmp659=Cyc_Absyn_fncall_exp(_tmp64D,((_tmpCC8[2]=
arg3,((_tmpCC8[1]=_tmp658,((_tmpCC8[0]=_tmp64F,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCC8,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);struct Cyc_Absyn_Stmt*_tmp65A=Cyc_Absyn_exp_stmt(_tmp659,0);_tmp65A=
Cyc_Absyn_declare_stmt(_tmp64E,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)
_tmp552,_tmp65A,0);e->r=Cyc_Toc_stmt_exp_r(_tmp65A);};}else{struct Cyc_Absyn_Exp*
_tmpCC9[3];e->r=Cyc_Toc_fncall_exp_r(_tmp64D,((_tmpCC9[2]=_tmp632,((_tmpCC9[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp61C.elt_typ),0),((_tmpCC9[0]=
_tmp552,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCC9,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2C6;
_LL2CB: _tmp635=_tmp629.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp636=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp635;if(_tmp636->tag != 0)goto _LL2CD;};_tmp637=_tmp629.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp638=(struct Cyc_Absyn_Upper_b_struct*)_tmp637;if(_tmp638->tag != 1)goto _LL2CD;
else{_tmp639=_tmp638->f1;}};_LL2CC: if(!Cyc_Evexp_c_can_eval(_tmp639)){const char*
_tmpCCC;void*_tmpCCB;(_tmpCCB=0,Cyc_Tcutil_terr(e->loc,((_tmpCCC="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpCCC,sizeof(char),71))),_tag_dyneither(_tmpCCB,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpCCF;void*_tmpCCE;(_tmpCCE=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCCF="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpCCF,sizeof(char),45))),_tag_dyneither(_tmpCCE,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp661=_tmp639;if(_tmp626  && !_tmp627)_tmp661=Cyc_Absyn_add_exp(
_tmp639,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp662=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpCD0[3];struct Cyc_Absyn_Exp*_tmp663=Cyc_Absyn_fncall_exp(
_tmp662,((_tmpCD0[2]=_tmp661,((_tmpCD0[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp619.elt_typ),0),((_tmpCD0[0]=_tmp552,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD0,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp623){struct Cyc_List_List*_tmpCD1;_tmp663->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_null_e,((_tmpCD1=_cycalloc(sizeof(*_tmpCD1)),((_tmpCD1->hd=Cyc_Absyn_copy_exp(
_tmp663),((_tmpCD1->tl=0,_tmpCD1)))))));}e->r=Cyc_Toc_cast_it_r(*_tmp551,_tmp663);
goto _LL2C6;};};_LL2CD: _tmp63A=_tmp629.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp63B=(struct Cyc_Absyn_DynEither_b_struct*)_tmp63A;if(_tmp63B->tag != 0)goto
_LL2C6;};_tmp63C=_tmp629.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp63D=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp63C;if(_tmp63D->tag != 0)goto _LL2C6;};_LL2CE:
DynCast: if((_tmp626  && !_tmp627) && !(_tmp61C.elt_tq).real_const){if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpCD4;void*_tmpCD3;(_tmpCD3=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCD4="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpCD4,sizeof(char),70))),_tag_dyneither(_tmpCD3,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp668=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpCD5[3];e->r=Cyc_Toc_fncall_exp_r(_tmp668,((_tmpCD5[2]=Cyc_Absyn_uint_exp(1,0),((
_tmpCD5[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp619.elt_typ),0),((
_tmpCD5[0]=_tmp552,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCD5,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}goto _LL2C6;
_LL2C6:;}goto _LL2BF;}_LL2C2: _tmp61D=_tmp616.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp61E=(struct Cyc_Absyn_PointerType_struct*)_tmp61D;if(_tmp61E->tag != 5)goto
_LL2C4;else{_tmp61F=_tmp61E->f1;}};_tmp620=_tmp616.f2;{struct Cyc_Absyn_IntType_struct*
_tmp621=(struct Cyc_Absyn_IntType_struct*)_tmp620;if(_tmp621->tag != 6)goto _LL2C4;};
_LL2C3:{void*_tmp66A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp61F.ptr_atts).bounds);_LL2D7: {struct Cyc_Absyn_DynEither_b_struct*
_tmp66B=(struct Cyc_Absyn_DynEither_b_struct*)_tmp66A;if(_tmp66B->tag != 0)goto
_LL2D9;}_LL2D8: _tmp552->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp552->r,
_tmp552->loc),Cyc_Toc_curr_sp);goto _LL2D6;_LL2D9:;_LL2DA: goto _LL2D6;_LL2D6:;}
goto _LL2BF;_LL2C4:;_LL2C5: goto _LL2BF;_LL2BF:;}goto _LL229;}_LL24E: {struct Cyc_Absyn_Address_e_struct*
_tmp555=(struct Cyc_Absyn_Address_e_struct*)_tmp518;if(_tmp555->tag != 16)goto
_LL250;else{_tmp556=_tmp555->f1;}}_LL24F:{void*_tmp66C=_tmp556->r;struct _tuple0*
_tmp66E;struct Cyc_List_List*_tmp66F;struct Cyc_List_List*_tmp670;struct Cyc_List_List*
_tmp672;_LL2DC: {struct Cyc_Absyn_Aggregate_e_struct*_tmp66D=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp66C;if(_tmp66D->tag != 29)goto _LL2DE;else{_tmp66E=_tmp66D->f1;_tmp66F=_tmp66D->f2;
_tmp670=_tmp66D->f3;}}_LL2DD: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpCD9;void*
_tmpCD8[1];struct Cyc_String_pa_struct _tmpCD7;(_tmpCD7.tag=0,((_tmpCD7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp556->loc)),((
_tmpCD8[0]=& _tmpCD7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpCD9="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpCD9,
sizeof(char),42))),_tag_dyneither(_tmpCD8,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp556->topt))->v,_tmp66F,1,0,
_tmp670,_tmp66E))->r;goto _LL2DB;_LL2DE: {struct Cyc_Absyn_Tuple_e_struct*_tmp671=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp66C;if(_tmp671->tag != 25)goto _LL2E0;else{
_tmp672=_tmp671->f1;}}_LL2DF: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpCDD;void*
_tmpCDC[1];struct Cyc_String_pa_struct _tmpCDB;(_tmpCDB.tag=0,((_tmpCDB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp556->loc)),((
_tmpCDC[0]=& _tmpCDB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpCDD="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpCDD,
sizeof(char),42))),_tag_dyneither(_tmpCDC,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_tuple(
nv,1,0,_tmp672))->r;goto _LL2DB;_LL2E0:;_LL2E1: Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(
nv,_tmp556);Cyc_Toc_set_lhs(nv,0);if(!Cyc_Absyn_is_lvalue(_tmp556)){((void(*)(
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
int),int f_env))Cyc_Toc_lvalue_assign)(_tmp556,0,Cyc_Toc_address_lvalue,1);e->r=
Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmp556);}goto _LL2DB;_LL2DB:;}goto _LL229;_LL250: {struct Cyc_Absyn_New_e_struct*
_tmp557=(struct Cyc_Absyn_New_e_struct*)_tmp518;if(_tmp557->tag != 17)goto _LL252;
else{_tmp558=_tmp557->f1;_tmp559=_tmp557->f2;}}_LL251: if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCE1;void*_tmpCE0[1];struct Cyc_String_pa_struct _tmpCDF;(_tmpCDF.tag=
0,((_tmpCDF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
_tmp559->loc)),((_tmpCE0[0]=& _tmpCDF,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCE1="%s: new at top-level",_tag_dyneither(
_tmpCE1,sizeof(char),21))),_tag_dyneither(_tmpCE0,sizeof(void*),1)))))));}{void*
_tmp67C=_tmp559->r;struct Cyc_List_List*_tmp67E;struct Cyc_Absyn_Vardecl*_tmp680;
struct Cyc_Absyn_Exp*_tmp681;struct Cyc_Absyn_Exp*_tmp682;int _tmp683;struct _tuple0*
_tmp685;struct Cyc_List_List*_tmp686;struct Cyc_List_List*_tmp687;struct Cyc_Absyn_Aggrdecl*
_tmp688;struct Cyc_List_List*_tmp68A;_LL2E3: {struct Cyc_Absyn_Array_e_struct*
_tmp67D=(struct Cyc_Absyn_Array_e_struct*)_tmp67C;if(_tmp67D->tag != 27)goto _LL2E5;
else{_tmp67E=_tmp67D->f1;}}_LL2E4: {struct _tuple0*_tmp68B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp68C=Cyc_Absyn_var_exp(_tmp68B,0);struct Cyc_Absyn_Stmt*
_tmp68D=Cyc_Toc_init_array(nv,_tmp68C,_tmp67E,Cyc_Absyn_exp_stmt(_tmp68C,0));
void*old_elt_typ;{void*_tmp68E=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp690;void*_tmp691;struct Cyc_Absyn_Tqual _tmp692;struct Cyc_Absyn_PtrAtts _tmp693;
union Cyc_Absyn_Constraint*_tmp694;_LL2EE: {struct Cyc_Absyn_PointerType_struct*
_tmp68F=(struct Cyc_Absyn_PointerType_struct*)_tmp68E;if(_tmp68F->tag != 5)goto
_LL2F0;else{_tmp690=_tmp68F->f1;_tmp691=_tmp690.elt_typ;_tmp692=_tmp690.elt_tq;
_tmp693=_tmp690.ptr_atts;_tmp694=_tmp693.zero_term;}}_LL2EF: old_elt_typ=_tmp691;
goto _LL2ED;_LL2F0:;_LL2F1: {const char*_tmpCE4;void*_tmpCE3;old_elt_typ=((_tmpCE3=
0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCE4="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(
_tmpCE4,sizeof(char),52))),_tag_dyneither(_tmpCE3,sizeof(void*),0))));}_LL2ED:;}{
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp697=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp698=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp67E),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp558 == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp698);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp558;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp698);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp68B,_tmp697,(struct Cyc_Absyn_Exp*)e1,_tmp68D,0));goto
_LL2E2;};}_LL2E5: {struct Cyc_Absyn_Comprehension_e_struct*_tmp67F=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp67C;if(_tmp67F->tag != 28)goto _LL2E7;else{_tmp680=_tmp67F->f1;_tmp681=_tmp67F->f2;
_tmp682=_tmp67F->f3;_tmp683=_tmp67F->f4;}}_LL2E6: {int is_dyneither_ptr=0;{void*
_tmp699=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp69B;void*_tmp69C;
struct Cyc_Absyn_Tqual _tmp69D;struct Cyc_Absyn_PtrAtts _tmp69E;union Cyc_Absyn_Constraint*
_tmp69F;union Cyc_Absyn_Constraint*_tmp6A0;_LL2F3: {struct Cyc_Absyn_PointerType_struct*
_tmp69A=(struct Cyc_Absyn_PointerType_struct*)_tmp699;if(_tmp69A->tag != 5)goto
_LL2F5;else{_tmp69B=_tmp69A->f1;_tmp69C=_tmp69B.elt_typ;_tmp69D=_tmp69B.elt_tq;
_tmp69E=_tmp69B.ptr_atts;_tmp69F=_tmp69E.bounds;_tmp6A0=_tmp69E.zero_term;}}
_LL2F4: is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp69F)== (void*)& Cyc_Absyn_DynEither_b_val;goto _LL2F2;
_LL2F5:;_LL2F6: {const char*_tmpCE7;void*_tmpCE6;(_tmpCE6=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCE7="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpCE7,sizeof(char),42))),_tag_dyneither(_tmpCE6,sizeof(void*),0)));}
_LL2F2:;}{struct _tuple0*max=Cyc_Toc_temp_var();struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp682->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp681);{struct Cyc_Absyn_Exp*_tmp6A3=
Cyc_Absyn_var_exp(max,0);if(_tmp683)_tmp6A3=Cyc_Absyn_add_exp(_tmp6A3,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp680,Cyc_Absyn_var_exp(max,0),_tmp682,_tmp683,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp6A5;struct _RegionHandle*_tmp6A6;struct Cyc_Toc_Env*_tmp6A4=
nv;_tmp6A5=*_tmp6A4;_tmp6A6=_tmp6A5.rgn;{struct _tuple21*_tmpCEA;struct Cyc_List_List*
_tmpCE9;struct Cyc_List_List*decls=(_tmpCE9=_region_malloc(_tmp6A6,sizeof(*
_tmpCE9)),((_tmpCE9->hd=((_tmpCEA=_region_malloc(_tmp6A6,sizeof(*_tmpCEA)),((
_tmpCEA->f1=max,((_tmpCEA->f2=Cyc_Absyn_uint_typ,((_tmpCEA->f3=(struct Cyc_Absyn_Exp*)
_tmp681,_tmpCEA)))))))),((_tmpCE9->tl=0,_tmpCE9)))));struct Cyc_Absyn_Exp*ai;if(
_tmp558 == 0  || Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*_tmpCEB[2];ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((_tmpCEB[1]=_tmp6A3,((
_tmpCEB[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCEB,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp558;Cyc_Toc_exp_to_c(
nv,r);{struct Cyc_Absyn_Exp*_tmpCEC[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpCEC[1]=_tmp6A3,((_tmpCEC[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCEC,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct _tuple21*_tmpCEF;struct Cyc_List_List*
_tmpCEE;decls=((_tmpCEE=_region_malloc(_tmp6A6,sizeof(*_tmpCEE)),((_tmpCEE->hd=((
_tmpCEF=_region_malloc(_tmp6A6,sizeof(*_tmpCEF)),((_tmpCEF->f1=a,((_tmpCEF->f2=
ptr_typ,((_tmpCEF->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpCEF)))))))),((_tmpCEE->tl=
decls,_tmpCEE))))));}if(is_dyneither_ptr){struct _tuple0*_tmp6AB=Cyc_Toc_temp_var();
void*_tmp6AC=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp6AD=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpCF0[3];struct Cyc_Absyn_Exp*_tmp6AE=Cyc_Absyn_fncall_exp(
_tmp6AD,((_tmpCF0[2]=_tmp6A3,((_tmpCF0[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((
_tmpCF0[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpCF0,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{
struct _tuple21*_tmpCF3;struct Cyc_List_List*_tmpCF2;decls=((_tmpCF2=
_region_malloc(_tmp6A6,sizeof(*_tmpCF2)),((_tmpCF2->hd=((_tmpCF3=_region_malloc(
_tmp6A6,sizeof(*_tmpCF3)),((_tmpCF3->f1=_tmp6AB,((_tmpCF3->f2=_tmp6AC,((_tmpCF3->f3=(
struct Cyc_Absyn_Exp*)_tmp6AE,_tmpCF3)))))))),((_tmpCF2->tl=decls,_tmpCF2))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp6AB,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp6B2=decls;for(0;_tmp6B2 != 0;_tmp6B2=_tmp6B2->tl){struct
_tuple0*_tmp6B4;void*_tmp6B5;struct Cyc_Absyn_Exp*_tmp6B6;struct _tuple21 _tmp6B3=*((
struct _tuple21*)_tmp6B2->hd);_tmp6B4=_tmp6B3.f1;_tmp6B5=_tmp6B3.f2;_tmp6B6=
_tmp6B3.f3;s=Cyc_Absyn_declare_stmt(_tmp6B4,_tmp6B5,_tmp6B6,s,0);}}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL2E2;};};};}_LL2E7: {struct Cyc_Absyn_Aggregate_e_struct*_tmp684=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp67C;if(_tmp684->tag != 29)goto _LL2E9;
else{_tmp685=_tmp684->f1;_tmp686=_tmp684->f2;_tmp687=_tmp684->f3;_tmp688=_tmp684->f4;}}
_LL2E8: e->r=(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp559->topt))->v,_tmp686,1,_tmp558,_tmp687,_tmp685))->r;goto _LL2E2;_LL2E9: {
struct Cyc_Absyn_Tuple_e_struct*_tmp689=(struct Cyc_Absyn_Tuple_e_struct*)_tmp67C;
if(_tmp689->tag != 25)goto _LL2EB;else{_tmp68A=_tmp689->f1;}}_LL2EA: e->r=(Cyc_Toc_init_tuple(
nv,1,_tmp558,_tmp68A))->r;goto _LL2E2;_LL2EB:;_LL2EC: {void*old_elt_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp559->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);struct _tuple0*_tmp6B9=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp6BA=Cyc_Absyn_var_exp(_tmp6B9,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(_tmp6BA,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp558
== 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp558;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp6BB=_tmp559->r;void*_tmp6BD;
struct Cyc_Absyn_Exp*_tmp6BE;_LL2F8: {struct Cyc_Absyn_Cast_e_struct*_tmp6BC=(
struct Cyc_Absyn_Cast_e_struct*)_tmp6BB;if(_tmp6BC->tag != 15)goto _LL2FA;else{
_tmp6BD=(void*)_tmp6BC->f1;_tmp6BE=_tmp6BC->f2;}}_LL2F9:{struct _tuple19 _tmpCF4;
struct _tuple19 _tmp6C0=(_tmpCF4.f1=Cyc_Tcutil_compress(_tmp6BD),((_tmpCF4.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp6BE->topt))->v),_tmpCF4)));void*
_tmp6C1;struct Cyc_Absyn_PtrInfo _tmp6C3;void*_tmp6C4;struct Cyc_Absyn_PtrAtts
_tmp6C5;union Cyc_Absyn_Constraint*_tmp6C6;void*_tmp6C7;struct Cyc_Absyn_PtrInfo
_tmp6C9;struct Cyc_Absyn_PtrAtts _tmp6CA;union Cyc_Absyn_Constraint*_tmp6CB;_LL2FD:
_tmp6C1=_tmp6C0.f1;{struct Cyc_Absyn_PointerType_struct*_tmp6C2=(struct Cyc_Absyn_PointerType_struct*)
_tmp6C1;if(_tmp6C2->tag != 5)goto _LL2FF;else{_tmp6C3=_tmp6C2->f1;_tmp6C4=_tmp6C3.elt_typ;
_tmp6C5=_tmp6C3.ptr_atts;_tmp6C6=_tmp6C5.bounds;}};_tmp6C7=_tmp6C0.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp6C8=(struct Cyc_Absyn_PointerType_struct*)_tmp6C7;if(_tmp6C8->tag != 5)goto
_LL2FF;else{_tmp6C9=_tmp6C8->f1;_tmp6CA=_tmp6C9.ptr_atts;_tmp6CB=_tmp6CA.bounds;}};
_LL2FE:{struct _tuple19 _tmpCF5;struct _tuple19 _tmp6CD=(_tmpCF5.f1=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6C6),((_tmpCF5.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp6CB),_tmpCF5)));void*_tmp6CE;void*_tmp6D0;struct Cyc_Absyn_Exp*
_tmp6D2;_LL302: _tmp6CE=_tmp6CD.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp6CF=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp6CE;if(_tmp6CF->tag != 0)goto _LL304;};
_tmp6D0=_tmp6CD.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp6D1=(struct Cyc_Absyn_Upper_b_struct*)
_tmp6D0;if(_tmp6D1->tag != 1)goto _LL304;else{_tmp6D2=_tmp6D1->f1;}};_LL303: Cyc_Toc_exp_to_c(
nv,_tmp6BE);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp6D3=Cyc_Toc__init_dyneither_ptr_e;{struct Cyc_Absyn_Exp*_tmpCF6[4];e->r=Cyc_Toc_fncall_exp_r(
_tmp6D3,((_tmpCF6[3]=_tmp6D2,((_tmpCF6[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp6C4),0),((_tmpCF6[1]=_tmp6BE,((_tmpCF6[0]=mexp,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCF6,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))));}goto _LL301;};_LL304:;_LL305: goto _LL301;_LL301:;}goto _LL2FC;_LL2FF:;
_LL300: goto _LL2FC;_LL2FC:;}goto _LL2F7;_LL2FA:;_LL2FB: goto _LL2F7;_LL2F7:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp6D5=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp6BA),
0);struct Cyc_Absyn_Exp*_tmp6D6=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,
_tmp559);_tmp6D5=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
_tmp6BA,_tmp6D6,0),_tmp559,0),_tmp6D5,0);{void*_tmp6D7=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp6B9,
_tmp6D7,(struct Cyc_Absyn_Exp*)mexp,_tmp6D5,0));};}goto _LL2E2;};}_LL2E2:;}goto
_LL229;_LL252: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp55A=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp518;if(_tmp55A->tag != 19)goto _LL254;else{_tmp55B=_tmp55A->f1;}}_LL253: Cyc_Toc_exp_to_c(
nv,_tmp55B);goto _LL229;_LL254: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp55C=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp518;if(_tmp55C->tag != 18)goto _LL256;
else{_tmp55D=(void*)_tmp55C->f1;}}_LL255:{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpCF9;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpCF8;e->r=(void*)((_tmpCF8=
_cycalloc(sizeof(*_tmpCF8)),((_tmpCF8[0]=((_tmpCF9.tag=18,((_tmpCF9.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp55D),_tmpCF9)))),_tmpCF8))));}goto _LL229;_LL256: {
struct Cyc_Absyn_Offsetof_e_struct*_tmp55E=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp518;if(_tmp55E->tag != 20)goto _LL258;else{_tmp55F=(void*)_tmp55E->f1;_tmp560=(
void*)_tmp55E->f2;{struct Cyc_Absyn_StructField_struct*_tmp561=(struct Cyc_Absyn_StructField_struct*)
_tmp560;if(_tmp561->tag != 0)goto _LL258;else{_tmp562=_tmp561->f1;}};}}_LL257:{
struct Cyc_Absyn_Offsetof_e_struct _tmpD03;struct Cyc_Absyn_StructField_struct
_tmpD02;struct Cyc_Absyn_StructField_struct*_tmpD01;struct Cyc_Absyn_Offsetof_e_struct*
_tmpD00;e->r=(void*)((_tmpD00=_cycalloc(sizeof(*_tmpD00)),((_tmpD00[0]=((_tmpD03.tag=
20,((_tmpD03.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp55F),((_tmpD03.f2=(void*)((
void*)((_tmpD01=_cycalloc(sizeof(*_tmpD01)),((_tmpD01[0]=((_tmpD02.tag=0,((
_tmpD02.f1=_tmp562,_tmpD02)))),_tmpD01))))),_tmpD03)))))),_tmpD00))));}goto
_LL229;_LL258: {struct Cyc_Absyn_Offsetof_e_struct*_tmp563=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp518;if(_tmp563->tag != 20)goto _LL25A;else{_tmp564=(void*)_tmp563->f1;_tmp565=(
void*)_tmp563->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp566=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp565;if(_tmp566->tag != 1)goto _LL25A;else{_tmp567=_tmp566->f1;}};}}_LL259:{
void*_tmp6DE=Cyc_Tcutil_compress(_tmp564);struct Cyc_Absyn_AggrInfo _tmp6E0;union
Cyc_Absyn_AggrInfoU _tmp6E1;struct Cyc_List_List*_tmp6E3;_LL307: {struct Cyc_Absyn_AggrType_struct*
_tmp6DF=(struct Cyc_Absyn_AggrType_struct*)_tmp6DE;if(_tmp6DF->tag != 11)goto
_LL309;else{_tmp6E0=_tmp6DF->f1;_tmp6E1=_tmp6E0.aggr_info;}}_LL308: {struct Cyc_Absyn_Aggrdecl*
_tmp6E6=Cyc_Absyn_get_known_aggrdecl(_tmp6E1);if(_tmp6E6->impl == 0){const char*
_tmpD06;void*_tmpD05;(_tmpD05=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD06="struct fields must be known",
_tag_dyneither(_tmpD06,sizeof(char),28))),_tag_dyneither(_tmpD05,sizeof(void*),0)));}
_tmp6E3=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6E6->impl))->fields;goto
_LL30A;}_LL309: {struct Cyc_Absyn_AnonAggrType_struct*_tmp6E2=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp6DE;if(_tmp6E2->tag != 12)goto _LL30B;else{_tmp6E3=_tmp6E2->f2;}}_LL30A: {
struct Cyc_Absyn_Aggrfield*_tmp6E9=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp6E3,(int)_tmp567);{struct Cyc_Absyn_Offsetof_e_struct
_tmpD10;struct Cyc_Absyn_StructField_struct _tmpD0F;struct Cyc_Absyn_StructField_struct*
_tmpD0E;struct Cyc_Absyn_Offsetof_e_struct*_tmpD0D;e->r=(void*)((_tmpD0D=
_cycalloc(sizeof(*_tmpD0D)),((_tmpD0D[0]=((_tmpD10.tag=20,((_tmpD10.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp564),((_tmpD10.f2=(void*)((void*)((_tmpD0E=_cycalloc(
sizeof(*_tmpD0E)),((_tmpD0E[0]=((_tmpD0F.tag=0,((_tmpD0F.f1=_tmp6E9->name,
_tmpD0F)))),_tmpD0E))))),_tmpD10)))))),_tmpD0D))));}goto _LL306;}_LL30B: {struct
Cyc_Absyn_TupleType_struct*_tmp6E4=(struct Cyc_Absyn_TupleType_struct*)_tmp6DE;
if(_tmp6E4->tag != 10)goto _LL30D;}_LL30C:{struct Cyc_Absyn_Offsetof_e_struct
_tmpD1A;struct Cyc_Absyn_StructField_struct _tmpD19;struct Cyc_Absyn_StructField_struct*
_tmpD18;struct Cyc_Absyn_Offsetof_e_struct*_tmpD17;e->r=(void*)((_tmpD17=
_cycalloc(sizeof(*_tmpD17)),((_tmpD17[0]=((_tmpD1A.tag=20,((_tmpD1A.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp564),((_tmpD1A.f2=(void*)((void*)((_tmpD18=_cycalloc(
sizeof(*_tmpD18)),((_tmpD18[0]=((_tmpD19.tag=0,((_tmpD19.f1=Cyc_Absyn_fieldname((
int)(_tmp567 + 1)),_tmpD19)))),_tmpD18))))),_tmpD1A)))))),_tmpD17))));}goto _LL306;
_LL30D: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp6E5=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp6DE;if(_tmp6E5->tag != 4)goto _LL30F;}_LL30E: if(_tmp567 == 0){struct Cyc_Absyn_Offsetof_e_struct
_tmpD24;struct Cyc_Absyn_StructField_struct _tmpD23;struct Cyc_Absyn_StructField_struct*
_tmpD22;struct Cyc_Absyn_Offsetof_e_struct*_tmpD21;e->r=(void*)((_tmpD21=
_cycalloc(sizeof(*_tmpD21)),((_tmpD21[0]=((_tmpD24.tag=20,((_tmpD24.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp564),((_tmpD24.f2=(void*)((void*)((_tmpD22=_cycalloc(
sizeof(*_tmpD22)),((_tmpD22[0]=((_tmpD23.tag=0,((_tmpD23.f1=Cyc_Toc_tag_sp,
_tmpD23)))),_tmpD22))))),_tmpD24)))))),_tmpD21))));}else{struct Cyc_Absyn_Offsetof_e_struct
_tmpD2E;struct Cyc_Absyn_StructField_struct _tmpD2D;struct Cyc_Absyn_StructField_struct*
_tmpD2C;struct Cyc_Absyn_Offsetof_e_struct*_tmpD2B;e->r=(void*)((_tmpD2B=
_cycalloc(sizeof(*_tmpD2B)),((_tmpD2B[0]=((_tmpD2E.tag=20,((_tmpD2E.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp564),((_tmpD2E.f2=(void*)((void*)((_tmpD2C=_cycalloc(
sizeof(*_tmpD2C)),((_tmpD2C[0]=((_tmpD2D.tag=0,((_tmpD2D.f1=Cyc_Absyn_fieldname((
int)_tmp567),_tmpD2D)))),_tmpD2C))))),_tmpD2E)))))),_tmpD2B))));}goto _LL306;
_LL30F:;_LL310: {const char*_tmpD31;void*_tmpD30;(_tmpD30=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD31="impossible type for offsetof tuple index",
_tag_dyneither(_tmpD31,sizeof(char),41))),_tag_dyneither(_tmpD30,sizeof(void*),0)));}
_LL306:;}goto _LL229;_LL25A: {struct Cyc_Absyn_Deref_e_struct*_tmp568=(struct Cyc_Absyn_Deref_e_struct*)
_tmp518;if(_tmp568->tag != 21)goto _LL25C;else{_tmp569=_tmp568->f1;}}_LL25B: {int
_tmp6FC=Cyc_Toc_in_lhs(nv);Cyc_Toc_set_lhs(nv,0);{void*_tmp6FD=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp569->topt))->v);{void*_tmp6FE=
_tmp6FD;struct Cyc_Absyn_PtrInfo _tmp700;void*_tmp701;struct Cyc_Absyn_Tqual _tmp702;
struct Cyc_Absyn_PtrAtts _tmp703;void*_tmp704;union Cyc_Absyn_Constraint*_tmp705;
union Cyc_Absyn_Constraint*_tmp706;union Cyc_Absyn_Constraint*_tmp707;_LL312: {
struct Cyc_Absyn_PointerType_struct*_tmp6FF=(struct Cyc_Absyn_PointerType_struct*)
_tmp6FE;if(_tmp6FF->tag != 5)goto _LL314;else{_tmp700=_tmp6FF->f1;_tmp701=_tmp700.elt_typ;
_tmp702=_tmp700.elt_tq;_tmp703=_tmp700.ptr_atts;_tmp704=_tmp703.rgn;_tmp705=
_tmp703.nullable;_tmp706=_tmp703.bounds;_tmp707=_tmp703.zero_term;}}_LL313:{void*
_tmp708=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp706);struct Cyc_Absyn_Exp*_tmp70A;_LL317: {struct Cyc_Absyn_Upper_b_struct*
_tmp709=(struct Cyc_Absyn_Upper_b_struct*)_tmp708;if(_tmp709->tag != 1)goto _LL319;
else{_tmp70A=_tmp709->f1;}}_LL318: {int do_null_check=Cyc_Toc_need_null_check(
_tmp569);Cyc_Toc_exp_to_c(nv,_tmp569);if(do_null_check){if(Cyc_Toc_warn_all_null_deref){
const char*_tmpD34;void*_tmpD33;(_tmpD33=0,Cyc_Tcutil_warn(e->loc,((_tmpD34="inserted null check due to dereference",
_tag_dyneither(_tmpD34,sizeof(char),39))),_tag_dyneither(_tmpD33,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpD35;_tmp569->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp6FD),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpD35=_cycalloc(sizeof(*
_tmpD35)),((_tmpD35->hd=Cyc_Absyn_copy_exp(_tmp569),((_tmpD35->tl=0,_tmpD35)))))),
0));};}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp707)){unsigned int _tmp710;int _tmp711;struct _tuple11 _tmp70F=Cyc_Evexp_eval_const_uint_exp(
_tmp70A);_tmp710=_tmp70F.f1;_tmp711=_tmp70F.f2;if(!_tmp711  || _tmp710 <= 0){const
char*_tmpD38;void*_tmpD37;(_tmpD37=0,Cyc_Tcutil_terr(e->loc,((_tmpD38="cannot determine dereference is in bounds",
_tag_dyneither(_tmpD38,sizeof(char),42))),_tag_dyneither(_tmpD37,sizeof(void*),0)));}}
goto _LL316;}_LL319: {struct Cyc_Absyn_DynEither_b_struct*_tmp70B=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp708;if(_tmp70B->tag != 0)goto _LL316;}_LL31A: {struct Cyc_Absyn_Exp*_tmp714=Cyc_Absyn_uint_exp(
0,0);{struct Cyc_Core_Opt*_tmpD39;_tmp714->topt=((_tmpD39=_cycalloc(sizeof(*
_tmpD39)),((_tmpD39->v=Cyc_Absyn_uint_typ,_tmpD39))));}e->r=Cyc_Toc_subscript_exp_r(
_tmp569,_tmp714);Cyc_Toc_exp_to_c(nv,e);goto _LL316;}_LL316:;}goto _LL311;_LL314:;
_LL315: {const char*_tmpD3C;void*_tmpD3B;(_tmpD3B=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD3C="exp_to_c: Deref: non-pointer",
_tag_dyneither(_tmpD3C,sizeof(char),29))),_tag_dyneither(_tmpD3B,sizeof(void*),0)));}
_LL311:;}Cyc_Toc_set_lhs(nv,_tmp6FC);goto _LL229;};}_LL25C: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp56A=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp518;if(_tmp56A->tag != 22)goto
_LL25E;else{_tmp56B=_tmp56A->f1;_tmp56C=_tmp56A->f2;_tmp56D=_tmp56A->f3;_tmp56E=
_tmp56A->f4;}}_LL25D: {int is_poly=Cyc_Toc_is_poly_project(e);void*e1_cyc_type=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp56B->topt))->v;Cyc_Toc_exp_to_c(nv,
_tmp56B);if(_tmp56D  && _tmp56E)e->r=Cyc_Toc_check_tagged_union(_tmp56B,Cyc_Toc_typ_to_c(
e1_cyc_type),e1_cyc_type,_tmp56C,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrmember_exp);
if(is_poly)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp(e->r,0),0))->r;goto _LL229;}_LL25E: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp56F=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp518;if(_tmp56F->tag != 23)goto _LL260;else{_tmp570=_tmp56F->f1;_tmp571=_tmp56F->f2;
_tmp572=_tmp56F->f3;_tmp573=_tmp56F->f4;}}_LL25F: {int _tmp718=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp570->topt))->v);int do_null_check=Cyc_Toc_need_null_check(_tmp570);
Cyc_Toc_exp_to_c(nv,_tmp570);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp71A;
struct Cyc_Absyn_Tqual _tmp71B;struct Cyc_Absyn_PtrAtts _tmp71C;void*_tmp71D;union
Cyc_Absyn_Constraint*_tmp71E;union Cyc_Absyn_Constraint*_tmp71F;union Cyc_Absyn_Constraint*
_tmp720;struct Cyc_Absyn_PtrInfo _tmp719=Cyc_Toc_get_ptr_type(e1typ);_tmp71A=
_tmp719.elt_typ;_tmp71B=_tmp719.elt_tq;_tmp71C=_tmp719.ptr_atts;_tmp71D=_tmp71C.rgn;
_tmp71E=_tmp71C.nullable;_tmp71F=_tmp71C.bounds;_tmp720=_tmp71C.zero_term;{void*
_tmp721=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp71F);struct Cyc_Absyn_Exp*_tmp723;_LL31C: {struct Cyc_Absyn_Upper_b_struct*
_tmp722=(struct Cyc_Absyn_Upper_b_struct*)_tmp721;if(_tmp722->tag != 1)goto _LL31E;
else{_tmp723=_tmp722->f1;}}_LL31D: {unsigned int _tmp726;int _tmp727;struct
_tuple11 _tmp725=Cyc_Evexp_eval_const_uint_exp(_tmp723);_tmp726=_tmp725.f1;
_tmp727=_tmp725.f2;if(_tmp727){if(_tmp726 < 1){const char*_tmpD3F;void*_tmpD3E;(
_tmpD3E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD3F="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpD3F,
sizeof(char),44))),_tag_dyneither(_tmpD3E,sizeof(void*),0)));}if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){const char*_tmpD42;void*_tmpD41;(_tmpD41=0,Cyc_Tcutil_warn(
e->loc,((_tmpD42="inserted null check due to dereference",_tag_dyneither(_tmpD42,
sizeof(char),39))),_tag_dyneither(_tmpD41,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpD43[1];_tmp570->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpD43[0]=Cyc_Absyn_new_exp(_tmp570->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD43,sizeof(struct Cyc_Absyn_Exp*),
1)))),0));};}}else{if(!Cyc_Evexp_c_can_eval(_tmp723)){const char*_tmpD46;void*
_tmpD45;(_tmpD45=0,Cyc_Tcutil_terr(e->loc,((_tmpD46="cannot determine pointer dereference in bounds",
_tag_dyneither(_tmpD46,sizeof(char),47))),_tag_dyneither(_tmpD45,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpD47[4];_tmp570->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((_tmpD47[3]=
Cyc_Absyn_uint_exp(0,0),((_tmpD47[2]=Cyc_Absyn_sizeoftyp_exp(_tmp71A,0),((
_tmpD47[1]=_tmp723,((_tmpD47[0]=Cyc_Absyn_new_exp(_tmp570->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD47,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0));};}goto _LL31B;}_LL31E: {struct Cyc_Absyn_DynEither_b_struct*
_tmp724=(struct Cyc_Absyn_DynEither_b_struct*)_tmp721;if(_tmp724->tag != 0)goto
_LL31B;}_LL31F: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp71A);{struct Cyc_Absyn_Exp*
_tmpD48[3];_tmp570->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp71B),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_dyneither_subscript_e,((_tmpD48[2]=Cyc_Absyn_uint_exp(0,0),((
_tmpD48[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD48[0]=Cyc_Absyn_new_exp(_tmp570->r,
0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD48,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}goto _LL31B;}_LL31B:;}if(
_tmp572  && _tmp573)e->r=Cyc_Toc_check_tagged_union(_tmp570,Cyc_Toc_typ_to_c(
e1typ),_tmp71A,_tmp571,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);if(is_poly
 && _tmp573)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp(e->r,0),0))->r;Cyc_Toc_set_lhs(nv,
_tmp718);goto _LL229;};};}_LL260: {struct Cyc_Absyn_Subscript_e_struct*_tmp574=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp518;if(_tmp574->tag != 24)goto _LL262;
else{_tmp575=_tmp574->f1;_tmp576=_tmp574->f2;}}_LL261: {int _tmp731=Cyc_Toc_in_lhs(
nv);Cyc_Toc_set_lhs(nv,0);{void*_tmp732=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp575->topt))->v);{void*_tmp733=_tmp732;struct Cyc_List_List*
_tmp735;struct Cyc_Absyn_PtrInfo _tmp737;void*_tmp738;struct Cyc_Absyn_Tqual _tmp739;
struct Cyc_Absyn_PtrAtts _tmp73A;void*_tmp73B;union Cyc_Absyn_Constraint*_tmp73C;
union Cyc_Absyn_Constraint*_tmp73D;union Cyc_Absyn_Constraint*_tmp73E;_LL321: {
struct Cyc_Absyn_TupleType_struct*_tmp734=(struct Cyc_Absyn_TupleType_struct*)
_tmp733;if(_tmp734->tag != 10)goto _LL323;else{_tmp735=_tmp734->f1;}}_LL322: Cyc_Toc_exp_to_c(
nv,_tmp575);Cyc_Toc_exp_to_c(nv,_tmp576);{unsigned int _tmp740;int _tmp741;struct
_tuple11 _tmp73F=Cyc_Evexp_eval_const_uint_exp(_tmp576);_tmp740=_tmp73F.f1;
_tmp741=_tmp73F.f2;if(!_tmp741){const char*_tmpD4B;void*_tmpD4A;(_tmpD4A=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD4B="unknown tuple subscript in translation to C",
_tag_dyneither(_tmpD4B,sizeof(char),44))),_tag_dyneither(_tmpD4A,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp575,Cyc_Absyn_fieldname((int)(_tmp740 + 1)));
goto _LL320;};_LL323: {struct Cyc_Absyn_PointerType_struct*_tmp736=(struct Cyc_Absyn_PointerType_struct*)
_tmp733;if(_tmp736->tag != 5)goto _LL325;else{_tmp737=_tmp736->f1;_tmp738=_tmp737.elt_typ;
_tmp739=_tmp737.elt_tq;_tmp73A=_tmp737.ptr_atts;_tmp73B=_tmp73A.rgn;_tmp73C=
_tmp73A.nullable;_tmp73D=_tmp73A.bounds;_tmp73E=_tmp73A.zero_term;}}_LL324: {
struct Cyc_List_List*_tmp744=Cyc_Toc_get_relns(_tmp575);int in_bnds=0;{void*
_tmp745=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp73D);_LL328:;_LL329: in_bnds=Cyc_Toc_check_bounds(_tmp732,_tmp744,_tmp575,
_tmp576);if(Cyc_Toc_warn_bounds_checks  && !in_bnds){const char*_tmpD4F;void*
_tmpD4E[1];struct Cyc_String_pa_struct _tmpD4D;(_tmpD4D.tag=0,((_tmpD4D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpD4E[0]=&
_tmpD4D,Cyc_Tcutil_warn(e->loc,((_tmpD4F="bounds check necessary for %s",
_tag_dyneither(_tmpD4F,sizeof(char),30))),_tag_dyneither(_tmpD4E,sizeof(void*),1)))))));}
_LL327:;}Cyc_Toc_exp_to_c(nv,_tmp575);Cyc_Toc_exp_to_c(nv,_tmp576);++ Cyc_Toc_total_bounds_checks;{
void*_tmp749=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp73D);struct Cyc_Absyn_Exp*_tmp74B;_LL32B: {struct Cyc_Absyn_Upper_b_struct*
_tmp74A=(struct Cyc_Absyn_Upper_b_struct*)_tmp749;if(_tmp74A->tag != 1)goto _LL32D;
else{_tmp74B=_tmp74A->f1;}}_LL32C: {int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp73C);void*ta1=Cyc_Toc_typ_to_c(_tmp738);void*ta2=
Cyc_Absyn_cstar_typ(ta1,_tmp739);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp73E)){if(!Cyc_Evexp_c_can_eval(_tmp74B)){const char*_tmpD52;void*_tmpD51;(
_tmpD51=0,Cyc_Tcutil_terr(e->loc,((_tmpD52="cannot determine subscript is in bounds",
_tag_dyneither(_tmpD52,sizeof(char),40))),_tag_dyneither(_tmpD51,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp575);struct Cyc_Absyn_Exp*_tmpD53[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpD53[2]=_tmp576,((_tmpD53[1]=_tmp74B,((
_tmpD53[0]=_tmp575,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD53,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp74B)){const char*_tmpD56;void*_tmpD55;(
_tmpD55=0,Cyc_Tcutil_terr(e->loc,((_tmpD56="cannot determine subscript is in bounds",
_tag_dyneither(_tmpD56,sizeof(char),40))),_tag_dyneither(_tmpD55,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){const char*_tmpD59;void*_tmpD58;(_tmpD58=0,Cyc_Tcutil_warn(
e->loc,((_tmpD59="inserted null check due to dereference",_tag_dyneither(_tmpD59,
sizeof(char),39))),_tag_dyneither(_tmpD58,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpD5A[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((
_tmpD5A[3]=_tmp576,((_tmpD5A[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD5A[1]=
_tmp74B,((_tmpD5A[0]=_tmp575,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD5A,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{if(
!Cyc_Evexp_c_can_eval(_tmp74B)){const char*_tmpD5D;void*_tmpD5C;(_tmpD5C=0,Cyc_Tcutil_terr(
e->loc,((_tmpD5D="cannot determine subscript is in bounds",_tag_dyneither(
_tmpD5D,sizeof(char),40))),_tag_dyneither(_tmpD5C,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpD5E[2];_tmp576->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((
_tmpD5E[1]=Cyc_Absyn_copy_exp(_tmp576),((_tmpD5E[0]=_tmp74B,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD5E,sizeof(struct Cyc_Absyn_Exp*),
2)))))));};}}}goto _LL32A;}_LL32D: {struct Cyc_Absyn_DynEither_b_struct*_tmp74C=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp749;if(_tmp74C->tag != 0)goto _LL32A;}
_LL32E: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp738);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp739),
Cyc_Absyn_aggrmember_exp(_tmp575,Cyc_Toc_curr_sp,0)),_tmp576);}else{struct Cyc_Absyn_Exp*
_tmp758=Cyc_Toc__check_dyneither_subscript_e;struct Cyc_Absyn_Exp*_tmpD5F[3];e->r=
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp739),Cyc_Absyn_fncall_exp(
_tmp758,((_tmpD5F[2]=_tmp576,((_tmpD5F[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((
_tmpD5F[0]=_tmp575,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD5F,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL32A;}
_LL32A:;}goto _LL320;}_LL325:;_LL326: {const char*_tmpD62;void*_tmpD61;(_tmpD61=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD62="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(
_tmpD62,sizeof(char),49))),_tag_dyneither(_tmpD61,sizeof(void*),0)));}_LL320:;}
Cyc_Toc_set_lhs(nv,_tmp731);goto _LL229;};}_LL262: {struct Cyc_Absyn_Tuple_e_struct*
_tmp577=(struct Cyc_Absyn_Tuple_e_struct*)_tmp518;if(_tmp577->tag != 25)goto _LL264;
else{_tmp578=_tmp577->f1;}}_LL263: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(
nv,0,0,_tmp578))->r;else{struct Cyc_List_List*_tmp75C=((struct Cyc_List_List*(*)(
struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_tup_to_c,_tmp578);void*_tmp75D=Cyc_Toc_add_tuple_type(_tmp75C);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp578 != 0;(_tmp578=_tmp578->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp578->hd);{struct _tuple16*_tmpD65;struct Cyc_List_List*
_tmpD64;dles=((_tmpD64=_cycalloc(sizeof(*_tmpD64)),((_tmpD64->hd=((_tmpD65=
_cycalloc(sizeof(*_tmpD65)),((_tmpD65->f1=0,((_tmpD65->f2=(struct Cyc_Absyn_Exp*)
_tmp578->hd,_tmpD65)))))),((_tmpD64->tl=dles,_tmpD64))))));};}}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);e->r=Cyc_Toc_unresolvedmem_exp_r(
0,dles);}goto _LL229;_LL264: {struct Cyc_Absyn_Array_e_struct*_tmp579=(struct Cyc_Absyn_Array_e_struct*)
_tmp518;if(_tmp579->tag != 27)goto _LL266;else{_tmp57A=_tmp579->f1;}}_LL265: e->r=
Cyc_Toc_unresolvedmem_exp_r(0,_tmp57A);{struct Cyc_List_List*_tmp760=_tmp57A;for(
0;_tmp760 != 0;_tmp760=_tmp760->tl){struct _tuple16 _tmp762;struct Cyc_Absyn_Exp*
_tmp763;struct _tuple16*_tmp761=(struct _tuple16*)_tmp760->hd;_tmp762=*_tmp761;
_tmp763=_tmp762.f2;Cyc_Toc_exp_to_c(nv,_tmp763);}}goto _LL229;_LL266: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp57B=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp518;if(_tmp57B->tag != 28)
goto _LL268;else{_tmp57C=_tmp57B->f1;_tmp57D=_tmp57B->f2;_tmp57E=_tmp57B->f3;
_tmp57F=_tmp57B->f4;}}_LL267: {unsigned int _tmp765;int _tmp766;struct _tuple11
_tmp764=Cyc_Evexp_eval_const_uint_exp(_tmp57D);_tmp765=_tmp764.f1;_tmp766=
_tmp764.f2;{void*_tmp767=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp57E->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp57E);{struct Cyc_List_List*
es=0;if(!Cyc_Toc_is_zero(_tmp57E)){if(!_tmp766){const char*_tmpD68;void*_tmpD67;(
_tmpD67=0,Cyc_Tcutil_terr(_tmp57D->loc,((_tmpD68="cannot determine value of constant",
_tag_dyneither(_tmpD68,sizeof(char),35))),_tag_dyneither(_tmpD67,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp765;++ i){struct _tuple16*_tmpD6B;struct Cyc_List_List*
_tmpD6A;es=((_tmpD6A=_cycalloc(sizeof(*_tmpD6A)),((_tmpD6A->hd=((_tmpD6B=
_cycalloc(sizeof(*_tmpD6B)),((_tmpD6B->f1=0,((_tmpD6B->f2=_tmp57E,_tmpD6B)))))),((
_tmpD6A->tl=es,_tmpD6A))))));}}if(_tmp57F){struct Cyc_Absyn_Exp*_tmp76C=Cyc_Toc_cast_it(
_tmp767,Cyc_Absyn_uint_exp(0,0));struct _tuple16*_tmpD6E;struct Cyc_List_List*
_tmpD6D;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,((_tmpD6D=_cycalloc(sizeof(*_tmpD6D)),((_tmpD6D->hd=((
_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((_tmpD6E->f1=0,((_tmpD6E->f2=_tmp76C,
_tmpD6E)))))),((_tmpD6D->tl=0,_tmpD6D)))))));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL229;};};}_LL268: {struct Cyc_Absyn_Aggregate_e_struct*_tmp580=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp518;if(_tmp580->tag != 29)goto _LL26A;
else{_tmp581=_tmp580->f1;_tmp582=_tmp580->f2;_tmp583=_tmp580->f3;_tmp584=_tmp580->f4;}}
_LL269: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp582,
0,0,_tmp583,_tmp581))->r;else{if(_tmp584 == 0){const char*_tmpD71;void*_tmpD70;(
_tmpD70=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD71="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpD71,sizeof(
char),38))),_tag_dyneither(_tmpD70,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp584;struct Cyc_Toc_Env _tmp772;struct
_RegionHandle*_tmp773;struct Cyc_Toc_Env*_tmp771=nv;_tmp772=*_tmp771;_tmp773=
_tmp772.rgn;{struct Cyc_List_List*_tmp774=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum 
Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp773,e->loc,_tmp583,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp584->impl))->tagged){struct
_tuple22 _tmp776;struct Cyc_Absyn_Aggrfield*_tmp777;struct Cyc_Absyn_Exp*_tmp778;
struct _tuple22*_tmp775=(struct _tuple22*)((struct Cyc_List_List*)_check_null(
_tmp774))->hd;_tmp776=*_tmp775;_tmp777=_tmp776.f1;_tmp778=_tmp776.f2;{void*
_tmp779=_tmp777->type;Cyc_Toc_exp_to_c(nv,_tmp778);if(Cyc_Toc_is_void_star_or_boxed_tvar(
_tmp779))_tmp778->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp778->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp584,_tmp777->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple16*_tmpD76;struct _tuple16*_tmpD75;struct _tuple16*_tmpD74[2];
struct Cyc_List_List*_tmp77A=(_tmpD74[1]=((_tmpD75=_cycalloc(sizeof(*_tmpD75)),((
_tmpD75->f1=0,((_tmpD75->f2=_tmp778,_tmpD75)))))),((_tmpD74[0]=((_tmpD76=
_cycalloc(sizeof(*_tmpD76)),((_tmpD76->f1=0,((_tmpD76->f2=field_tag_exp,_tmpD76)))))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD74,sizeof(struct _tuple16*),2)))));struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(
0,_tmp77A,0);struct Cyc_Absyn_FieldName_struct*_tmpD7C;struct Cyc_Absyn_FieldName_struct
_tmpD7B;void*_tmpD7A[1];struct Cyc_List_List*ds=(_tmpD7A[0]=(void*)((_tmpD7C=
_cycalloc(sizeof(*_tmpD7C)),((_tmpD7C[0]=((_tmpD7B.tag=1,((_tmpD7B.f1=_tmp777->name,
_tmpD7B)))),_tmpD7C)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD7A,sizeof(void*),1)));struct _tuple16*_tmpD7F;struct _tuple16*
_tmpD7E[1];struct Cyc_List_List*dles=(_tmpD7E[0]=((_tmpD7F=_cycalloc(sizeof(*
_tmpD7F)),((_tmpD7F->f1=ds,((_tmpD7F->f2=umem,_tmpD7F)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD7E,sizeof(struct
_tuple16*),1)));e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{struct Cyc_List_List*
_tmp783=0;struct Cyc_List_List*_tmp784=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp784 != 0;_tmp784=_tmp784->tl){struct Cyc_List_List*
_tmp785=_tmp774;for(0;_tmp785 != 0;_tmp785=_tmp785->tl){if((*((struct _tuple22*)
_tmp785->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp784->hd){struct _tuple22
_tmp787;struct Cyc_Absyn_Aggrfield*_tmp788;struct Cyc_Absyn_Exp*_tmp789;struct
_tuple22*_tmp786=(struct _tuple22*)_tmp785->hd;_tmp787=*_tmp786;_tmp788=_tmp787.f1;
_tmp789=_tmp787.f2;{void*_tmp78A=_tmp788->type;Cyc_Toc_exp_to_c(nv,_tmp789);if(
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp78A))_tmp789->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp789->r,0));{struct _tuple16*_tmpD82;struct Cyc_List_List*
_tmpD81;_tmp783=((_tmpD81=_cycalloc(sizeof(*_tmpD81)),((_tmpD81->hd=((_tmpD82=
_cycalloc(sizeof(*_tmpD82)),((_tmpD82->f1=0,((_tmpD82->f2=_tmp789,_tmpD82)))))),((
_tmpD81->tl=_tmp783,_tmpD81))))));}break;};}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp783));}};};}
goto _LL229;_LL26A: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp585=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp518;if(_tmp585->tag != 30)goto _LL26C;else{_tmp586=(void*)_tmp585->f1;_tmp587=
_tmp585->f2;}}_LL26B: {struct Cyc_List_List*fs;{void*_tmp78D=Cyc_Tcutil_compress(
_tmp586);struct Cyc_List_List*_tmp78F;_LL330: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp78E=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp78D;if(_tmp78E->tag != 12)goto
_LL332;else{_tmp78F=_tmp78E->f2;}}_LL331: fs=_tmp78F;goto _LL32F;_LL332:;_LL333: {
const char*_tmpD86;void*_tmpD85[1];struct Cyc_String_pa_struct _tmpD84;(_tmpD84.tag=
0,((_tmpD84.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp586)),((_tmpD85[0]=& _tmpD84,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD86="anon struct has type %s",
_tag_dyneither(_tmpD86,sizeof(char),24))),_tag_dyneither(_tmpD85,sizeof(void*),1)))))));}
_LL32F:;}{struct Cyc_Toc_Env _tmp794;struct _RegionHandle*_tmp795;struct Cyc_Toc_Env*
_tmp793=nv;_tmp794=*_tmp793;_tmp795=_tmp794.rgn;{struct Cyc_List_List*_tmp796=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp795,e->loc,_tmp587,Cyc_Absyn_StructA,fs);for(0;_tmp796 != 0;_tmp796=_tmp796->tl){
struct _tuple22 _tmp798;struct Cyc_Absyn_Aggrfield*_tmp799;struct Cyc_Absyn_Exp*
_tmp79A;struct _tuple22*_tmp797=(struct _tuple22*)_tmp796->hd;_tmp798=*_tmp797;
_tmp799=_tmp798.f1;_tmp79A=_tmp798.f2;{void*_tmp79B=_tmp799->type;Cyc_Toc_exp_to_c(
nv,_tmp79A);if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp79B))_tmp79A->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp79A->r,0));};}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp587);}goto _LL229;};}_LL26C: {struct Cyc_Absyn_Datatype_e_struct*_tmp588=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp518;if(_tmp588->tag != 31)goto _LL26E;else{
_tmp589=_tmp588->f1;_tmp58A=_tmp588->f2;_tmp58B=_tmp588->f3;}}_LL26D: {void*
datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple0*_tmp79C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp79D=Cyc_Absyn_var_exp(_tmp79C,0);struct Cyc_Absyn_Exp*
member_exp;{const char*_tmpD87;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp58B->name,((_tmpD87="_struct",_tag_dyneither(_tmpD87,sizeof(char),8)))));}
tag_exp=_tmp58A->is_extensible?Cyc_Absyn_var_exp(_tmp58B->name,0): Cyc_Toc_datatype_tag(
_tmp58A,_tmp58B->name);member_exp=_tmp79D;{struct Cyc_List_List*_tmp79F=_tmp58B->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp589 != 0;(
_tmp589=_tmp589->tl,_tmp79F=_tmp79F->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp589->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp79F))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_boxed_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct _tuple16*_tmpD8A;struct
Cyc_List_List*_tmpD89;dles=((_tmpD89=_cycalloc(sizeof(*_tmpD89)),((_tmpD89->hd=((
_tmpD8A=_cycalloc(sizeof(*_tmpD8A)),((_tmpD8A->f1=0,((_tmpD8A->f2=cur_e,_tmpD8A)))))),((
_tmpD89->tl=dles,_tmpD89))))));};}{struct _tuple16*_tmpD8D;struct Cyc_List_List*
_tmpD8C;dles=((_tmpD8C=_cycalloc(sizeof(*_tmpD8C)),((_tmpD8C->hd=((_tmpD8D=
_cycalloc(sizeof(*_tmpD8D)),((_tmpD8D->f1=0,((_tmpD8D->f2=tag_exp,_tmpD8D)))))),((
_tmpD8C->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles),_tmpD8C))))));}e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{struct Cyc_List_List*
_tmpD8E;struct Cyc_List_List*_tmp7A4=(_tmpD8E=_cycalloc(sizeof(*_tmpD8E)),((
_tmpD8E->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,
0),tag_exp,0),((_tmpD8E->tl=0,_tmpD8E)))));{int i=1;for(0;_tmp589 != 0;(((_tmp589=
_tmp589->tl,i ++)),_tmp79F=_tmp79F->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp589->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp79F))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_boxed_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp7A5=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpD8F;_tmp7A4=((_tmpD8F=_cycalloc(sizeof(*
_tmpD8F)),((_tmpD8F->hd=_tmp7A5,((_tmpD8F->tl=_tmp7A4,_tmpD8F))))));};}}{struct
Cyc_Absyn_Stmt*_tmp7A7=Cyc_Absyn_exp_stmt(_tmp79D,0);struct Cyc_List_List*_tmpD90;
struct Cyc_Absyn_Stmt*_tmp7A8=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpD90=_cycalloc(sizeof(*_tmpD90)),((
_tmpD90->hd=_tmp7A7,((_tmpD90->tl=_tmp7A4,_tmpD90))))))),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp79C,datatype_ctype,0,_tmp7A8,0));};}goto _LL229;};}
_LL26E: {struct Cyc_Absyn_Enum_e_struct*_tmp58C=(struct Cyc_Absyn_Enum_e_struct*)
_tmp518;if(_tmp58C->tag != 32)goto _LL270;}_LL26F: goto _LL271;_LL270: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp58D=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp518;if(_tmp58D->tag != 33)goto
_LL272;}_LL271: goto _LL229;_LL272: {struct Cyc_Absyn_Malloc_e_struct*_tmp58E=(
struct Cyc_Absyn_Malloc_e_struct*)_tmp518;if(_tmp58E->tag != 34)goto _LL274;else{
_tmp58F=_tmp58E->f1;_tmp590=_tmp58F.is_calloc;_tmp591=_tmp58F.rgn;_tmp592=
_tmp58F.elt_type;_tmp593=_tmp58F.num_elts;_tmp594=_tmp58F.fat_result;}}_LL273: {
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp592)));Cyc_Toc_exp_to_c(nv,
_tmp593);if(_tmp594){struct _tuple0*_tmp7AB=Cyc_Toc_temp_var();struct _tuple0*
_tmp7AC=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp590){xexp=_tmp593;if(_tmp591 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp591;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp7AB,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp592,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp7AB,0));}{struct Cyc_Absyn_Exp*_tmpD91[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpD91[2]=Cyc_Absyn_var_exp(_tmp7AB,0),((_tmpD91[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpD91[0]=Cyc_Absyn_var_exp(_tmp7AC,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD91,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{if(_tmp591 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp591;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp7AB,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp592,Cyc_Absyn_var_exp(_tmp7AB,0));}{struct Cyc_Absyn_Exp*_tmpD92[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpD92[2]=Cyc_Absyn_var_exp(_tmp7AB,0),((_tmpD92[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpD92[0]=Cyc_Absyn_var_exp(_tmp7AC,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD92,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);};}{struct Cyc_Absyn_Stmt*_tmp7AF=Cyc_Absyn_declare_stmt(_tmp7AB,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)_tmp593,Cyc_Absyn_declare_stmt(_tmp7AC,Cyc_Absyn_cstar_typ(
t_c,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp7AF);};}else{if(_tmp590){if(_tmp591 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp591;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp593))->r;}else{e->r=(
Cyc_Toc_calloc_exp(*_tmp592,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp593))->r;}}else{
if(_tmp591 != 0  && !Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp591;Cyc_Toc_exp_to_c(nv,rgn);e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp593))->r;}
else{e->r=(Cyc_Toc_malloc_exp(*_tmp592,_tmp593))->r;}}}goto _LL229;}_LL274: {
struct Cyc_Absyn_Swap_e_struct*_tmp595=(struct Cyc_Absyn_Swap_e_struct*)_tmp518;
if(_tmp595->tag != 35)goto _LL276;else{_tmp596=_tmp595->f1;_tmp597=_tmp595->f2;}}
_LL275: {int is_dyneither_ptr=0;void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp596->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp597->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,&
is_dyneither_ptr)){const char*_tmpD95;void*_tmpD94;(_tmpD94=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD95="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpD95,sizeof(char),57))),_tag_dyneither(_tmpD94,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}{int f1_tag=0;void*tagged_mem_type1=(void*)&
Cyc_Absyn_VoidType_val;int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp596,&
f1_tag,& tagged_mem_type1,1);int f2_tag=0;void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp597,& f2_tag,& tagged_mem_type2,1);
Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp596);Cyc_Toc_exp_to_c(nv,_tmp597);
Cyc_Toc_set_lhs(nv,0);if(e1_tmem)Cyc_Toc_add_tagged_union_check_for_swap(_tmp596,
f1_tag,tagged_mem_type1);else{_tmp596=Cyc_Toc_push_address_exp(_tmp596);}if(
e2_tmem)Cyc_Toc_add_tagged_union_check_for_swap(_tmp597,f2_tag,tagged_mem_type2);
else{_tmp597=Cyc_Toc_push_address_exp(_tmp597);}{struct Cyc_Absyn_Exp*_tmpD96[2];
e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpD96[1]=_tmp597,((_tmpD96[0]=_tmp596,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD96,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL229;};};}_LL276: {struct
Cyc_Absyn_Tagcheck_e_struct*_tmp598=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp518;
if(_tmp598->tag != 38)goto _LL278;else{_tmp599=_tmp598->f1;_tmp59A=_tmp598->f2;}}
_LL277: {void*_tmp7B3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp599->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp599);{void*_tmp7B4=
_tmp7B3;struct Cyc_Absyn_AggrInfo _tmp7B6;union Cyc_Absyn_AggrInfoU _tmp7B7;struct
Cyc_Absyn_Aggrdecl**_tmp7B8;struct Cyc_Absyn_Aggrdecl*_tmp7B9;_LL335: {struct Cyc_Absyn_AggrType_struct*
_tmp7B5=(struct Cyc_Absyn_AggrType_struct*)_tmp7B4;if(_tmp7B5->tag != 11)goto
_LL337;else{_tmp7B6=_tmp7B5->f1;_tmp7B7=_tmp7B6.aggr_info;if((_tmp7B7.KnownAggr).tag
!= 2)goto _LL337;_tmp7B8=(struct Cyc_Absyn_Aggrdecl**)(_tmp7B7.KnownAggr).val;
_tmp7B9=*_tmp7B8;}}_LL336: {struct Cyc_Absyn_Exp*_tmp7BA=Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp7B9,_tmp59A),0);struct Cyc_Absyn_Exp*_tmp7BB=Cyc_Absyn_aggrmember_exp(
_tmp599,_tmp59A,0);struct Cyc_Absyn_Exp*_tmp7BC=Cyc_Absyn_aggrmember_exp(_tmp7BB,
Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(_tmp7BC,_tmp7BA,0))->r;goto _LL334;}
_LL337:;_LL338: {const char*_tmpD99;void*_tmpD98;(_tmpD98=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD99="non-aggregate type in tagcheck",
_tag_dyneither(_tmpD99,sizeof(char),31))),_tag_dyneither(_tmpD98,sizeof(void*),0)));}
_LL334:;}goto _LL229;}_LL278: {struct Cyc_Absyn_StmtExp_e_struct*_tmp59B=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp518;if(_tmp59B->tag != 37)goto _LL27A;else{_tmp59C=
_tmp59B->f1;}}_LL279: Cyc_Toc_stmt_to_c(nv,_tmp59C);goto _LL229;_LL27A: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp59D=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp518;if(_tmp59D->tag != 36)
goto _LL27C;}_LL27B: {const char*_tmpD9C;void*_tmpD9B;(_tmpD9B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD9C="UnresolvedMem",
_tag_dyneither(_tmpD9C,sizeof(char),14))),_tag_dyneither(_tmpD9B,sizeof(void*),0)));}
_LL27C: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp59E=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp518;if(_tmp59E->tag != 26)goto _LL27E;}_LL27D: {const char*_tmpD9F;void*_tmpD9E;(
_tmpD9E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpD9F="compoundlit",_tag_dyneither(_tmpD9F,sizeof(char),12))),_tag_dyneither(
_tmpD9E,sizeof(void*),0)));}_LL27E: {struct Cyc_Absyn_Valueof_e_struct*_tmp59F=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp518;if(_tmp59F->tag != 39)goto _LL280;}
_LL27F: {const char*_tmpDA2;void*_tmpDA1;(_tmpDA1=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDA2="valueof(-)",
_tag_dyneither(_tmpDA2,sizeof(char),11))),_tag_dyneither(_tmpDA1,sizeof(void*),0)));}
_LL280: {struct Cyc_Absyn_Asm_e_struct*_tmp5A0=(struct Cyc_Absyn_Asm_e_struct*)
_tmp518;if(_tmp5A0->tag != 40)goto _LL229;}_LL281: {const char*_tmpDA5;void*_tmpDA4;(
_tmpDA4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDA5="__asm__",_tag_dyneither(_tmpDA5,sizeof(char),8))),_tag_dyneither(
_tmpDA4,sizeof(void*),0)));}_LL229:;};}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0);}struct _tuple23{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*
f3;};struct _tuple24{struct _tuple0*f1;void*f2;};struct _tuple25{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple23 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*
nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
struct Cyc_Absyn_Stmt*s;{void*_tmp7C7=p->r;struct Cyc_Absyn_Vardecl*_tmp7C9;struct
Cyc_Absyn_Vardecl _tmp7CA;struct _tuple0*_tmp7CB;struct Cyc_Absyn_Pat*_tmp7CC;
struct Cyc_Absyn_Vardecl*_tmp7CE;struct Cyc_Absyn_Vardecl _tmp7CF;struct _tuple0*
_tmp7D0;struct Cyc_Absyn_Vardecl*_tmp7D3;struct Cyc_Absyn_Pat*_tmp7D4;enum Cyc_Absyn_Sign
_tmp7D7;int _tmp7D8;char _tmp7DA;struct _dyneither_ptr _tmp7DC;int _tmp7DD;struct Cyc_Absyn_Enumdecl*
_tmp7DF;struct Cyc_Absyn_Enumfield*_tmp7E0;void*_tmp7E2;struct Cyc_Absyn_Enumfield*
_tmp7E3;struct Cyc_Absyn_Pat*_tmp7E5;struct Cyc_Absyn_Pat _tmp7E6;void*_tmp7E7;
struct Cyc_Absyn_Datatypedecl*_tmp7E9;struct Cyc_Absyn_Datatypefield*_tmp7EA;
struct Cyc_List_List*_tmp7EB;struct Cyc_List_List*_tmp7ED;struct Cyc_List_List*
_tmp7EF;struct Cyc_Absyn_AggrInfo*_tmp7F1;struct Cyc_Absyn_AggrInfo*_tmp7F3;struct
Cyc_Absyn_AggrInfo _tmp7F4;union Cyc_Absyn_AggrInfoU _tmp7F5;struct Cyc_List_List*
_tmp7F6;struct Cyc_Absyn_Pat*_tmp7F8;_LL33A: {struct Cyc_Absyn_Var_p_struct*
_tmp7C8=(struct Cyc_Absyn_Var_p_struct*)_tmp7C7;if(_tmp7C8->tag != 1)goto _LL33C;
else{_tmp7C9=_tmp7C8->f1;_tmp7CA=*_tmp7C9;_tmp7CB=_tmp7CA.name;_tmp7CC=_tmp7C8->f2;}}
_LL33B: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp7CB,r),rgn,t,r,path,
_tmp7CC,fail_stmt,decls);_LL33C: {struct Cyc_Absyn_TagInt_p_struct*_tmp7CD=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp7C7;if(_tmp7CD->tag != 3)goto _LL33E;else{
_tmp7CE=_tmp7CD->f2;_tmp7CF=*_tmp7CE;_tmp7D0=_tmp7CF.name;}}_LL33D: nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp7D0,r);goto _LL33F;_LL33E: {struct Cyc_Absyn_Wild_p_struct*_tmp7D1=(
struct Cyc_Absyn_Wild_p_struct*)_tmp7C7;if(_tmp7D1->tag != 0)goto _LL340;}_LL33F: s=
Cyc_Toc_skip_stmt_dl();goto _LL339;_LL340: {struct Cyc_Absyn_Reference_p_struct*
_tmp7D2=(struct Cyc_Absyn_Reference_p_struct*)_tmp7C7;if(_tmp7D2->tag != 2)goto
_LL342;else{_tmp7D3=_tmp7D2->f1;_tmp7D4=_tmp7D2->f2;}}_LL341: {struct _tuple0*
_tmp7FC=Cyc_Toc_temp_var();{struct _tuple24*_tmpDA8;struct Cyc_List_List*_tmpDA7;
decls=((_tmpDA7=_region_malloc(rgn,sizeof(*_tmpDA7)),((_tmpDA7->hd=((_tmpDA8=
_region_malloc(rgn,sizeof(*_tmpDA8)),((_tmpDA8->f1=_tmp7FC,((_tmpDA8->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpDA8)))))),((_tmpDA7->tl=decls,_tmpDA7))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp7D3->name,Cyc_Absyn_var_exp(_tmp7FC,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp7FC,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Toc_push_address_exp(path)),0);{struct _tuple23 _tmp7FF=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp7D4,fail_stmt,decls);_tmp7FF.f3=Cyc_Absyn_seq_stmt(s,_tmp7FF.f3,
0);return _tmp7FF;};}_LL342: {struct Cyc_Absyn_Null_p_struct*_tmp7D5=(struct Cyc_Absyn_Null_p_struct*)
_tmp7C7;if(_tmp7D5->tag != 8)goto _LL344;}_LL343: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),fail_stmt);goto _LL339;_LL344: {struct Cyc_Absyn_Int_p_struct*_tmp7D6=(struct
Cyc_Absyn_Int_p_struct*)_tmp7C7;if(_tmp7D6->tag != 9)goto _LL346;else{_tmp7D7=
_tmp7D6->f1;_tmp7D8=_tmp7D6->f2;}}_LL345: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp7D7,_tmp7D8,0),fail_stmt);goto _LL339;_LL346: {struct Cyc_Absyn_Char_p_struct*
_tmp7D9=(struct Cyc_Absyn_Char_p_struct*)_tmp7C7;if(_tmp7D9->tag != 10)goto _LL348;
else{_tmp7DA=_tmp7D9->f1;}}_LL347: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(
_tmp7DA,0),fail_stmt);goto _LL339;_LL348: {struct Cyc_Absyn_Float_p_struct*_tmp7DB=(
struct Cyc_Absyn_Float_p_struct*)_tmp7C7;if(_tmp7DB->tag != 11)goto _LL34A;else{
_tmp7DC=_tmp7DB->f1;_tmp7DD=_tmp7DB->f2;}}_LL349: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(
_tmp7DC,_tmp7DD,0),fail_stmt);goto _LL339;_LL34A: {struct Cyc_Absyn_Enum_p_struct*
_tmp7DE=(struct Cyc_Absyn_Enum_p_struct*)_tmp7C7;if(_tmp7DE->tag != 12)goto _LL34C;
else{_tmp7DF=_tmp7DE->f1;_tmp7E0=_tmp7DE->f2;}}_LL34B:{struct Cyc_Absyn_Enum_e_struct
_tmpDAB;struct Cyc_Absyn_Enum_e_struct*_tmpDAA;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpDAA=_cycalloc(sizeof(*_tmpDAA)),((_tmpDAA[0]=((_tmpDAB.tag=32,((
_tmpDAB.f1=_tmp7E0->name,((_tmpDAB.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7DF,((
_tmpDAB.f3=(struct Cyc_Absyn_Enumfield*)_tmp7E0,_tmpDAB)))))))),_tmpDAA)))),0),
fail_stmt);}goto _LL339;_LL34C: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp7E1=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp7C7;if(_tmp7E1->tag != 13)goto _LL34E;else{
_tmp7E2=(void*)_tmp7E1->f1;_tmp7E3=_tmp7E1->f2;}}_LL34D:{struct Cyc_Absyn_AnonEnum_e_struct
_tmpDAE;struct Cyc_Absyn_AnonEnum_e_struct*_tmpDAD;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpDAD=_cycalloc(sizeof(*_tmpDAD)),((_tmpDAD[0]=((_tmpDAE.tag=33,((
_tmpDAE.f1=_tmp7E3->name,((_tmpDAE.f2=(void*)_tmp7E2,((_tmpDAE.f3=(struct Cyc_Absyn_Enumfield*)
_tmp7E3,_tmpDAE)))))))),_tmpDAD)))),0),fail_stmt);}goto _LL339;_LL34E: {struct Cyc_Absyn_Pointer_p_struct*
_tmp7E4=(struct Cyc_Absyn_Pointer_p_struct*)_tmp7C7;if(_tmp7E4->tag != 5)goto
_LL350;else{_tmp7E5=_tmp7E4->f1;_tmp7E6=*_tmp7E5;_tmp7E7=_tmp7E6.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp7E8=(struct Cyc_Absyn_Datatype_p_struct*)_tmp7E7;if(_tmp7E8->tag != 7)goto
_LL350;else{_tmp7E9=_tmp7E8->f1;_tmp7EA=_tmp7E8->f2;_tmp7EB=_tmp7E8->f3;}};}}
_LL34F: s=Cyc_Toc_skip_stmt_dl();{struct _tuple0*_tmp804=Cyc_Toc_temp_var();const
char*_tmpDAF;struct _tuple0*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp7EA->name,((
_tmpDAF="_struct",_tag_dyneither(_tmpDAF,sizeof(char),8))));void*_tmp805=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);int cnt=1;struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
_tmp805,r);struct Cyc_List_List*_tmp806=_tmp7EA->typs;for(0;_tmp7EB != 0;(((
_tmp7EB=_tmp7EB->tl,_tmp806=((struct Cyc_List_List*)_check_null(_tmp806))->tl)),
++ cnt)){struct Cyc_Absyn_Pat*_tmp807=(struct Cyc_Absyn_Pat*)_tmp7EB->hd;if(_tmp807->r
== (void*)& Cyc_Absyn_Wild_p_val)continue;{void*_tmp808=(*((struct _tuple10*)((
struct Cyc_List_List*)_check_null(_tmp806))->hd)).f2;struct _tuple0*_tmp809=Cyc_Toc_temp_var();
void*_tmp80A=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp807->topt))->v;void*
_tmp80B=Cyc_Toc_typ_to_c(_tmp80A);struct Cyc_Absyn_Exp*_tmp80C=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp804,0),Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_boxed_tvar(
Cyc_Toc_typ_to_c(_tmp808)))_tmp80C=Cyc_Toc_cast_it(_tmp80B,_tmp80C);{struct
_tuple24*_tmpDB2;struct Cyc_List_List*_tmpDB1;decls=((_tmpDB1=_region_malloc(rgn,
sizeof(*_tmpDB1)),((_tmpDB1->hd=((_tmpDB2=_region_malloc(rgn,sizeof(*_tmpDB2)),((
_tmpDB2->f1=_tmp809,((_tmpDB2->f2=_tmp80B,_tmpDB2)))))),((_tmpDB1->tl=decls,
_tmpDB1))))));}{struct _tuple23 _tmp80F=Cyc_Toc_xlate_pat(nv,rgn,_tmp80A,Cyc_Absyn_var_exp(
_tmp809,0),_tmp80C,_tmp807,fail_stmt,decls);nv=_tmp80F.f1;decls=_tmp80F.f2;{
struct Cyc_Absyn_Stmt*_tmp810=_tmp80F.f3;struct Cyc_Absyn_Stmt*_tmp811=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp809,0),_tmp80C,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp811,_tmp810,0),0);};};};}{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp804,0),Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*tag_exp=
_tmp7E9->is_extensible?Cyc_Absyn_var_exp(_tmp7EA->name,0): Cyc_Toc_datatype_tag(
_tmp7E9,_tmp7EA->name);struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,
tag_exp,0);s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);if(Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v))s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp804,0),Cyc_Absyn_uint_exp(0,0),0),
fail_stmt,s,0);s=Cyc_Absyn_declare_stmt(_tmp804,_tmp805,(struct Cyc_Absyn_Exp*)
rcast,s,0);goto _LL339;};};_LL350: {struct Cyc_Absyn_Datatype_p_struct*_tmp7EC=(
struct Cyc_Absyn_Datatype_p_struct*)_tmp7C7;if(_tmp7EC->tag != 7)goto _LL352;else{
_tmp7ED=_tmp7EC->f3;}}_LL351: _tmp7EF=_tmp7ED;goto _LL353;_LL352: {struct Cyc_Absyn_Tuple_p_struct*
_tmp7EE=(struct Cyc_Absyn_Tuple_p_struct*)_tmp7C7;if(_tmp7EE->tag != 4)goto _LL354;
else{_tmp7EF=_tmp7EE->f1;}}_LL353: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;
_tmp7EF != 0;(_tmp7EF=_tmp7EF->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp813=(struct Cyc_Absyn_Pat*)
_tmp7EF->hd;if(_tmp813->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{struct _tuple0*
_tmp814=Cyc_Toc_temp_var();void*_tmp815=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp813->topt))->v;{struct _tuple24*_tmpDB5;struct Cyc_List_List*_tmpDB4;decls=((
_tmpDB4=_region_malloc(rgn,sizeof(*_tmpDB4)),((_tmpDB4->hd=((_tmpDB5=
_region_malloc(rgn,sizeof(*_tmpDB5)),((_tmpDB5->f1=_tmp814,((_tmpDB5->f2=Cyc_Toc_typ_to_c(
_tmp815),_tmpDB5)))))),((_tmpDB4->tl=decls,_tmpDB4))))));}{struct _tuple23 _tmp818=
Cyc_Toc_xlate_pat(nv,rgn,_tmp815,Cyc_Absyn_var_exp(_tmp814,0),Cyc_Absyn_aggrmember_exp(
path,Cyc_Absyn_fieldname(cnt),0),_tmp813,fail_stmt,decls);nv=_tmp818.f1;decls=
_tmp818.f2;{struct Cyc_Absyn_Stmt*_tmp819=_tmp818.f3;struct Cyc_Absyn_Stmt*_tmp81A=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp814,0),Cyc_Absyn_aggrmember_exp(r,
Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp81A,
_tmp819,0),0);};};};}goto _LL339;};_LL354: {struct Cyc_Absyn_Aggr_p_struct*_tmp7F0=(
struct Cyc_Absyn_Aggr_p_struct*)_tmp7C7;if(_tmp7F0->tag != 6)goto _LL356;else{
_tmp7F1=_tmp7F0->f1;if(_tmp7F1 != 0)goto _LL356;}}_LL355: {const char*_tmpDB8;void*
_tmpDB7;(_tmpDB7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDB8="unresolved aggregate pattern!",_tag_dyneither(_tmpDB8,sizeof(char),30))),
_tag_dyneither(_tmpDB7,sizeof(void*),0)));}_LL356: {struct Cyc_Absyn_Aggr_p_struct*
_tmp7F2=(struct Cyc_Absyn_Aggr_p_struct*)_tmp7C7;if(_tmp7F2->tag != 6)goto _LL358;
else{_tmp7F3=_tmp7F2->f1;if(_tmp7F3 == 0)goto _LL358;_tmp7F4=*_tmp7F3;_tmp7F5=
_tmp7F4.aggr_info;_tmp7F6=_tmp7F2->f3;}}_LL357: {struct Cyc_Absyn_Aggrdecl*
_tmp81D=Cyc_Absyn_get_known_aggrdecl(_tmp7F5);if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp81D->impl))->tagged){struct _tuple25 _tmp81F;struct Cyc_List_List*
_tmp820;struct Cyc_Absyn_Pat*_tmp821;struct _tuple25*_tmp81E=(struct _tuple25*)((
struct Cyc_List_List*)_check_null(_tmp7F6))->hd;_tmp81F=*_tmp81E;_tmp820=_tmp81F.f1;
_tmp821=_tmp81F.f2;{struct _dyneither_ptr*f;{void*_tmp822=(void*)((struct Cyc_List_List*)
_check_null(_tmp820))->hd;struct _dyneither_ptr*_tmp824;_LL361: {struct Cyc_Absyn_FieldName_struct*
_tmp823=(struct Cyc_Absyn_FieldName_struct*)_tmp822;if(_tmp823->tag != 1)goto
_LL363;else{_tmp824=_tmp823->f1;}}_LL362: f=_tmp824;goto _LL360;_LL363:;_LL364: {
const char*_tmpDBB;void*_tmpDBA;(_tmpDBA=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDBB="no field name in tagged union pattern",
_tag_dyneither(_tmpDBB,sizeof(char),38))),_tag_dyneither(_tmpDBA,sizeof(void*),0)));}
_LL360:;}{struct _tuple0*_tmp827=Cyc_Toc_temp_var();void*_tmp828=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp821->topt))->v;void*_tmp829=Cyc_Toc_typ_to_c(
_tmp828);{struct _tuple24*_tmpDBE;struct Cyc_List_List*_tmpDBD;decls=((_tmpDBD=
_region_malloc(rgn,sizeof(*_tmpDBD)),((_tmpDBD->hd=((_tmpDBE=_region_malloc(rgn,
sizeof(*_tmpDBE)),((_tmpDBE->f1=_tmp827,((_tmpDBE->f2=_tmp829,_tmpDBE)))))),((
_tmpDBD->tl=decls,_tmpDBD))))));}{struct Cyc_Absyn_Exp*_tmp82C=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp82D=
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp82D=Cyc_Toc_cast_it(_tmp829,_tmp82D);_tmp82C=Cyc_Toc_cast_it(_tmp829,_tmp82C);{
struct _tuple23 _tmp82E=Cyc_Toc_xlate_pat(nv,rgn,_tmp828,Cyc_Absyn_var_exp(_tmp827,
0),_tmp82C,_tmp821,fail_stmt,decls);nv=_tmp82E.f1;decls=_tmp82E.f2;{struct Cyc_Absyn_Stmt*
_tmp82F=_tmp82E.f3;struct Cyc_Absyn_Stmt*_tmp830=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp81D,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp831=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp827,0),_tmp82D,0);s=Cyc_Absyn_seq_stmt(_tmp830,Cyc_Absyn_seq_stmt(_tmp831,
_tmp82F,0),0);};};};};};}else{s=Cyc_Toc_skip_stmt_dl();for(0;_tmp7F6 != 0;_tmp7F6=
_tmp7F6->tl){struct _tuple25*_tmp832=(struct _tuple25*)_tmp7F6->hd;struct Cyc_Absyn_Pat*
_tmp833=(*_tmp832).f2;if(_tmp833->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _dyneither_ptr*f;{void*_tmp834=(void*)((struct Cyc_List_List*)_check_null((*
_tmp832).f1))->hd;struct _dyneither_ptr*_tmp836;_LL366: {struct Cyc_Absyn_FieldName_struct*
_tmp835=(struct Cyc_Absyn_FieldName_struct*)_tmp834;if(_tmp835->tag != 1)goto
_LL368;else{_tmp836=_tmp835->f1;}}_LL367: f=_tmp836;goto _LL365;_LL368:;_LL369: {
struct Cyc_Toc_Match_error_struct _tmpDC1;struct Cyc_Toc_Match_error_struct*_tmpDC0;(
int)_throw((void*)((_tmpDC0=_cycalloc_atomic(sizeof(*_tmpDC0)),((_tmpDC0[0]=((
_tmpDC1.tag=Cyc_Toc_Match_error,_tmpDC1)),_tmpDC0)))));}_LL365:;}{struct _tuple0*
_tmp839=Cyc_Toc_temp_var();void*_tmp83A=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp833->topt))->v;void*_tmp83B=Cyc_Toc_typ_to_c(_tmp83A);{struct _tuple24*
_tmpDC4;struct Cyc_List_List*_tmpDC3;decls=((_tmpDC3=_region_malloc(rgn,sizeof(*
_tmpDC3)),((_tmpDC3->hd=((_tmpDC4=_region_malloc(rgn,sizeof(*_tmpDC4)),((_tmpDC4->f1=
_tmp839,((_tmpDC4->f2=_tmp83B,_tmpDC4)))))),((_tmpDC3->tl=decls,_tmpDC3))))));}{
struct _tuple23 _tmp83E=Cyc_Toc_xlate_pat(nv,rgn,_tmp83A,Cyc_Absyn_var_exp(_tmp839,
0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp833,fail_stmt,decls);nv=_tmp83E.f1;
decls=_tmp83E.f2;{struct Cyc_Absyn_Exp*_tmp83F=Cyc_Absyn_aggrmember_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(
Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp81D->impl))->fields,
f)))->type))_tmp83F=Cyc_Toc_cast_it(_tmp83B,_tmp83F);{struct Cyc_Absyn_Stmt*
_tmp840=_tmp83E.f3;struct Cyc_Absyn_Stmt*_tmp841=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp839,0),_tmp83F,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp841,_tmp840,
0),0);};};};};};}}goto _LL339;}_LL358: {struct Cyc_Absyn_Pointer_p_struct*_tmp7F7=(
struct Cyc_Absyn_Pointer_p_struct*)_tmp7C7;if(_tmp7F7->tag != 5)goto _LL35A;else{
_tmp7F8=_tmp7F7->f1;}}_LL359: {struct _tuple0*_tmp842=Cyc_Toc_temp_var();void*
_tmp843=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp7F8->topt))->v;{struct
_tuple24*_tmpDC7;struct Cyc_List_List*_tmpDC6;decls=((_tmpDC6=_region_malloc(rgn,
sizeof(*_tmpDC6)),((_tmpDC6->hd=((_tmpDC7=_region_malloc(rgn,sizeof(*_tmpDC7)),((
_tmpDC7->f1=_tmp842,((_tmpDC7->f2=Cyc_Toc_typ_to_c(_tmp843),_tmpDC7)))))),((
_tmpDC6->tl=decls,_tmpDC6))))));}{struct _tuple23 _tmp846=Cyc_Toc_xlate_pat(nv,rgn,
_tmp843,Cyc_Absyn_var_exp(_tmp842,0),Cyc_Absyn_deref_exp(path,0),_tmp7F8,
fail_stmt,decls);nv=_tmp846.f1;decls=_tmp846.f2;{struct Cyc_Absyn_Stmt*_tmp847=
_tmp846.f3;struct Cyc_Absyn_Stmt*_tmp848=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp842,0),Cyc_Absyn_deref_exp(r,0),0),_tmp847,0);if(Cyc_Toc_is_nullable(
t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(
0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),_tmp848,0);else{s=_tmp848;}goto
_LL339;};};}_LL35A: {struct Cyc_Absyn_UnknownId_p_struct*_tmp7F9=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp7C7;if(_tmp7F9->tag != 14)goto _LL35C;}_LL35B: {const char*_tmpDCA;void*_tmpDC9;(
_tmpDC9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDCA="unknownid pat",_tag_dyneither(_tmpDCA,sizeof(char),14))),_tag_dyneither(
_tmpDC9,sizeof(void*),0)));}_LL35C: {struct Cyc_Absyn_UnknownCall_p_struct*
_tmp7FA=(struct Cyc_Absyn_UnknownCall_p_struct*)_tmp7C7;if(_tmp7FA->tag != 15)goto
_LL35E;}_LL35D: {const char*_tmpDCD;void*_tmpDCC;(_tmpDCC=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDCD="unknowncall pat",
_tag_dyneither(_tmpDCD,sizeof(char),16))),_tag_dyneither(_tmpDCC,sizeof(void*),0)));}
_LL35E: {struct Cyc_Absyn_Exp_p_struct*_tmp7FB=(struct Cyc_Absyn_Exp_p_struct*)
_tmp7C7;if(_tmp7FB->tag != 16)goto _LL339;}_LL35F: {const char*_tmpDD0;void*_tmpDCF;(
_tmpDCF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDD0="exp pat",_tag_dyneither(_tmpDD0,sizeof(char),8))),_tag_dyneither(
_tmpDCF,sizeof(void*),0)));}_LL339:;}{struct _tuple23 _tmpDD1;return(_tmpDD1.f1=nv,((
_tmpDD1.f2=decls,((_tmpDD1.f3=s,_tmpDD1)))));};}struct _tuple26{struct
_dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
static struct _tuple26*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){struct _tuple26*_tmpDD2;return(_tmpDD2=_region_malloc(r,sizeof(*_tmpDD2)),((
_tmpDD2->f1=Cyc_Toc_fresh_label(),((_tmpDD2->f2=Cyc_Toc_fresh_label(),((_tmpDD2->f3=
sc,_tmpDD2)))))));}static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{
void*_tmp851=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int
leave_as_switch;{void*_tmp852=Cyc_Tcutil_compress(_tmp851);_LL36B: {struct Cyc_Absyn_IntType_struct*
_tmp853=(struct Cyc_Absyn_IntType_struct*)_tmp852;if(_tmp853->tag != 6)goto _LL36D;}
_LL36C: goto _LL36E;_LL36D: {struct Cyc_Absyn_EnumType_struct*_tmp854=(struct Cyc_Absyn_EnumType_struct*)
_tmp852;if(_tmp854->tag != 13)goto _LL36F;}_LL36E: leave_as_switch=1;goto _LL36A;
_LL36F:;_LL370: leave_as_switch=0;goto _LL36A;_LL36A:;}{struct Cyc_List_List*
_tmp855=scs;for(0;_tmp855 != 0;_tmp855=_tmp855->tl){if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp855->hd)->pat_vars))->v
!= 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp855->hd)->where_clause != 0){
leave_as_switch=0;break;}}}if(leave_as_switch){struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp856=scs;for(0;_tmp856 != 0;_tmp856=_tmp856->tl){struct Cyc_Absyn_Stmt*
_tmp857=((struct Cyc_Absyn_Switch_clause*)_tmp856->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp856->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp857,0);next_l=Cyc_Toc_fresh_label();{
struct Cyc_Toc_Env _tmp859;struct _RegionHandle*_tmp85A;struct Cyc_Toc_Env*_tmp858=
nv;_tmp859=*_tmp858;_tmp85A=_tmp859.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
_tmp85A,nv,next_l),_tmp857);};}}return;}{struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp85C;
struct _RegionHandle*_tmp85D;struct Cyc_Toc_Env*_tmp85B=nv;_tmp85C=*_tmp85B;
_tmp85D=_tmp85C.rgn;{struct Cyc_Toc_Env*_tmp85E=Cyc_Toc_share_env(_tmp85D,nv);
struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple26*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp85D,Cyc_Toc_gen_label,
_tmp85D,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct
Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp85F=lscs;for(0;_tmp85F != 0;
_tmp85F=_tmp85F->tl){struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple26*)
_tmp85F->hd)).f3;struct _dyneither_ptr*fail_lab=_tmp85F->tl == 0?end_l:(*((struct
_tuple26*)((struct Cyc_List_List*)_check_null(_tmp85F->tl))->hd)).f1;struct Cyc_Toc_Env*
_tmp861;struct Cyc_List_List*_tmp862;struct Cyc_Absyn_Stmt*_tmp863;struct _tuple23
_tmp860=Cyc_Toc_xlate_pat(_tmp85E,_tmp85D,_tmp851,r,path,sc->pattern,Cyc_Absyn_goto_stmt(
fail_lab,0),decls);_tmp861=_tmp860.f1;_tmp862=_tmp860.f2;_tmp863=_tmp860.f3;if(
sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp864=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp861,_tmp864);_tmp863=Cyc_Absyn_seq_stmt(
_tmp863,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp864,0),
Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp862;{
struct Cyc_List_List*_tmpDD3;nvs=((_tmpDD3=_region_malloc(_tmp85D,sizeof(*_tmpDD3)),((
_tmpDD3->hd=_tmp861,((_tmpDD3->tl=nvs,_tmpDD3))))));}{struct Cyc_List_List*
_tmpDD4;test_stmts=((_tmpDD4=_region_malloc(_tmp85D,sizeof(*_tmpDD4)),((_tmpDD4->hd=
_tmp863,((_tmpDD4->tl=test_stmts,_tmpDD4))))));};}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple26 _tmp868;struct _dyneither_ptr*_tmp869;struct _dyneither_ptr*_tmp86A;
struct Cyc_Absyn_Switch_clause*_tmp86B;struct _tuple26*_tmp867=(struct _tuple26*)
lscs->hd;_tmp868=*_tmp867;_tmp869=_tmp868.f1;_tmp86A=_tmp868.f2;_tmp86B=_tmp868.f3;{
struct Cyc_Toc_Env*_tmp86C=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp86B->body;struct Cyc_Toc_Env _tmp86E;struct
_RegionHandle*_tmp86F;struct Cyc_Toc_Env*_tmp86D=_tmp85E;_tmp86E=*_tmp86D;_tmp86F=
_tmp86E.rgn;if(lscs->tl != 0){struct _tuple26 _tmp871;struct _dyneither_ptr*_tmp872;
struct Cyc_Absyn_Switch_clause*_tmp873;struct _tuple26*_tmp870=(struct _tuple26*)((
struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp871=*_tmp870;_tmp872=_tmp871.f2;
_tmp873=_tmp871.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp86F,
_tmp86C,end_l,_tmp872,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp873->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp86F,
_tmp86C,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp869,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp86A,s,0),0);{struct Cyc_List_List*_tmpDD5;stmts=((_tmpDD5=_region_malloc(
_tmp85D,sizeof(*_tmpDD5)),((_tmpDD5->hd=s,((_tmpDD5->tl=stmts,_tmpDD5))))));};};}{
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,
Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple24
_tmp876;struct _tuple0*_tmp877;void*_tmp878;struct _tuple24*_tmp875=(struct
_tuple24*)decls->hd;_tmp876=*_tmp875;_tmp877=_tmp876.f1;_tmp878=_tmp876.f2;res=
Cyc_Absyn_declare_stmt(_tmp877,_tmp878,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(
v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(
struct Cyc_Absyn_Exp*)e,res,0))->r;};};};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){struct Cyc_List_List*
_tmpDD6;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,((
_tmpDD6=_cycalloc(sizeof(*_tmpDD6)),((_tmpDD6->hd=Cyc_Absyn_uint_exp((
unsigned int)(n - 1),0),((_tmpDD6->tl=0,_tmpDD6)))))),0),0);}void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s){while(1){void*_tmp87A=s->r;struct Cyc_Absyn_Exp*_tmp87D;
struct Cyc_Absyn_Stmt*_tmp87F;struct Cyc_Absyn_Stmt*_tmp880;struct Cyc_Absyn_Exp*
_tmp882;struct Cyc_Absyn_Exp*_tmp884;struct Cyc_Absyn_Stmt*_tmp885;struct Cyc_Absyn_Stmt*
_tmp886;struct _tuple9 _tmp888;struct Cyc_Absyn_Exp*_tmp889;struct Cyc_Absyn_Stmt*
_tmp88A;struct Cyc_Absyn_Stmt*_tmp88C;struct Cyc_Absyn_Stmt*_tmp88E;struct Cyc_Absyn_Stmt*
_tmp890;struct Cyc_Absyn_Exp*_tmp892;struct _tuple9 _tmp893;struct Cyc_Absyn_Exp*
_tmp894;struct _tuple9 _tmp895;struct Cyc_Absyn_Exp*_tmp896;struct Cyc_Absyn_Stmt*
_tmp897;struct Cyc_Absyn_Exp*_tmp899;struct Cyc_List_List*_tmp89A;struct Cyc_List_List*
_tmp89C;struct Cyc_Absyn_Switch_clause**_tmp89D;struct Cyc_Absyn_Decl*_tmp89F;
struct Cyc_Absyn_Stmt*_tmp8A0;struct _dyneither_ptr*_tmp8A2;struct Cyc_Absyn_Stmt*
_tmp8A3;struct Cyc_Absyn_Stmt*_tmp8A5;struct _tuple9 _tmp8A6;struct Cyc_Absyn_Exp*
_tmp8A7;struct Cyc_Absyn_Stmt*_tmp8A9;struct Cyc_List_List*_tmp8AA;struct Cyc_Absyn_Exp*
_tmp8AC;_LL372: {struct Cyc_Absyn_Skip_s_struct*_tmp87B=(struct Cyc_Absyn_Skip_s_struct*)
_tmp87A;if(_tmp87B->tag != 0)goto _LL374;}_LL373: return;_LL374: {struct Cyc_Absyn_Exp_s_struct*
_tmp87C=(struct Cyc_Absyn_Exp_s_struct*)_tmp87A;if(_tmp87C->tag != 1)goto _LL376;
else{_tmp87D=_tmp87C->f1;}}_LL375: Cyc_Toc_exp_to_c(nv,_tmp87D);return;_LL376: {
struct Cyc_Absyn_Seq_s_struct*_tmp87E=(struct Cyc_Absyn_Seq_s_struct*)_tmp87A;if(
_tmp87E->tag != 2)goto _LL378;else{_tmp87F=_tmp87E->f1;_tmp880=_tmp87E->f2;}}
_LL377: Cyc_Toc_stmt_to_c(nv,_tmp87F);s=_tmp880;continue;_LL378: {struct Cyc_Absyn_Return_s_struct*
_tmp881=(struct Cyc_Absyn_Return_s_struct*)_tmp87A;if(_tmp881->tag != 3)goto _LL37A;
else{_tmp882=_tmp881->f1;}}_LL379: {struct Cyc_Core_Opt*topt=0;if(_tmp882 != 0){{
struct Cyc_Core_Opt*_tmpDD7;topt=((_tmpDD7=_cycalloc(sizeof(*_tmpDD7)),((_tmpDD7->v=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp882->topt))->v),
_tmpDD7))));}Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp882);}if(s->try_depth
> 0){if(topt != 0){struct _tuple0*_tmp8AE=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*
_tmp8AF=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp8AE,0),
0);s->r=(Cyc_Absyn_declare_stmt(_tmp8AE,(void*)topt->v,_tmp882,Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler(s->try_depth),_tmp8AF,0),0))->r;}else{Cyc_Toc_do_npop_before(
s->try_depth,s);}}return;}_LL37A: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp883=(
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp87A;if(_tmp883->tag != 4)goto _LL37C;
else{_tmp884=_tmp883->f1;_tmp885=_tmp883->f2;_tmp886=_tmp883->f3;}}_LL37B: Cyc_Toc_exp_to_c(
nv,_tmp884);Cyc_Toc_stmt_to_c(nv,_tmp885);s=_tmp886;continue;_LL37C: {struct Cyc_Absyn_While_s_struct*
_tmp887=(struct Cyc_Absyn_While_s_struct*)_tmp87A;if(_tmp887->tag != 5)goto _LL37E;
else{_tmp888=_tmp887->f1;_tmp889=_tmp888.f1;_tmp88A=_tmp887->f2;}}_LL37D: Cyc_Toc_exp_to_c(
nv,_tmp889);{struct Cyc_Toc_Env _tmp8B1;struct _RegionHandle*_tmp8B2;struct Cyc_Toc_Env*
_tmp8B0=nv;_tmp8B1=*_tmp8B0;_tmp8B2=_tmp8B1.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp8B2,nv),_tmp88A);return;};_LL37E: {struct Cyc_Absyn_Break_s_struct*_tmp88B=(
struct Cyc_Absyn_Break_s_struct*)_tmp87A;if(_tmp88B->tag != 6)goto _LL380;else{
_tmp88C=_tmp88B->f1;}}_LL37F: {struct Cyc_Toc_Env _tmp8B4;struct _dyneither_ptr**
_tmp8B5;struct Cyc_Toc_Env*_tmp8B3=nv;_tmp8B4=*_tmp8B3;_tmp8B5=_tmp8B4.break_lab;
if(_tmp8B5 != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp8B5,0);{int dest_depth=_tmp88C == 0?0:
_tmp88C->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;};}
_LL380: {struct Cyc_Absyn_Continue_s_struct*_tmp88D=(struct Cyc_Absyn_Continue_s_struct*)
_tmp87A;if(_tmp88D->tag != 7)goto _LL382;else{_tmp88E=_tmp88D->f1;}}_LL381: {
struct Cyc_Toc_Env _tmp8B7;struct _dyneither_ptr**_tmp8B8;struct Cyc_Toc_Env*_tmp8B6=
nv;_tmp8B7=*_tmp8B6;_tmp8B8=_tmp8B7.continue_lab;if(_tmp8B8 != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp8B8,0);_tmp890=_tmp88E;goto _LL383;}_LL382: {struct Cyc_Absyn_Goto_s_struct*
_tmp88F=(struct Cyc_Absyn_Goto_s_struct*)_tmp87A;if(_tmp88F->tag != 8)goto _LL384;
else{_tmp890=_tmp88F->f2;}}_LL383: Cyc_Toc_do_npop_before(s->try_depth - ((struct
Cyc_Absyn_Stmt*)_check_null(_tmp890))->try_depth,s);return;_LL384: {struct Cyc_Absyn_For_s_struct*
_tmp891=(struct Cyc_Absyn_For_s_struct*)_tmp87A;if(_tmp891->tag != 9)goto _LL386;
else{_tmp892=_tmp891->f1;_tmp893=_tmp891->f2;_tmp894=_tmp893.f1;_tmp895=_tmp891->f3;
_tmp896=_tmp895.f1;_tmp897=_tmp891->f4;}}_LL385: Cyc_Toc_exp_to_c(nv,_tmp892);Cyc_Toc_exp_to_c(
nv,_tmp894);Cyc_Toc_exp_to_c(nv,_tmp896);{struct Cyc_Toc_Env _tmp8BA;struct
_RegionHandle*_tmp8BB;struct Cyc_Toc_Env*_tmp8B9=nv;_tmp8BA=*_tmp8B9;_tmp8BB=
_tmp8BA.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp8BB,nv),_tmp897);return;};
_LL386: {struct Cyc_Absyn_Switch_s_struct*_tmp898=(struct Cyc_Absyn_Switch_s_struct*)
_tmp87A;if(_tmp898->tag != 10)goto _LL388;else{_tmp899=_tmp898->f1;_tmp89A=_tmp898->f2;}}
_LL387: Cyc_Toc_xlate_switch(nv,s,_tmp899,_tmp89A);return;_LL388: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp89B=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp87A;if(_tmp89B->tag != 11)goto
_LL38A;else{_tmp89C=_tmp89B->f1;_tmp89D=_tmp89B->f2;}}_LL389: {struct Cyc_Toc_Env
_tmp8BD;struct Cyc_Toc_FallthruInfo*_tmp8BE;struct Cyc_Toc_Env*_tmp8BC=nv;_tmp8BD=*
_tmp8BC;_tmp8BE=_tmp8BD.fallthru_info;if(_tmp8BE == 0){const char*_tmpDDA;void*
_tmpDD9;(_tmpDD9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDDA="fallthru in unexpected place",_tag_dyneither(_tmpDDA,sizeof(char),29))),
_tag_dyneither(_tmpDD9,sizeof(void*),0)));}{struct _dyneither_ptr*_tmp8C2;struct
Cyc_List_List*_tmp8C3;struct Cyc_Dict_Dict _tmp8C4;struct Cyc_Toc_FallthruInfo
_tmp8C1=*_tmp8BE;_tmp8C2=_tmp8C1.label;_tmp8C3=_tmp8C1.binders;_tmp8C4=_tmp8C1.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp8C2,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp89D)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp8C5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp8C3);struct Cyc_List_List*_tmp8C6=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp89C);for(0;_tmp8C5 != 0;(_tmp8C5=_tmp8C5->tl,
_tmp8C6=_tmp8C6->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp8C6))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp8C4,(
struct _tuple0*)_tmp8C5->hd),(struct Cyc_Absyn_Exp*)_tmp8C6->hd,0),s2,0);}s->r=s2->r;
return;};};};}_LL38A: {struct Cyc_Absyn_Decl_s_struct*_tmp89E=(struct Cyc_Absyn_Decl_s_struct*)
_tmp87A;if(_tmp89E->tag != 12)goto _LL38C;else{_tmp89F=_tmp89E->f1;_tmp8A0=_tmp89E->f2;}}
_LL38B:{void*_tmp8C7=_tmp89F->r;struct Cyc_Absyn_Vardecl*_tmp8C9;struct Cyc_Absyn_Pat*
_tmp8CB;struct Cyc_Absyn_Exp*_tmp8CC;struct Cyc_List_List*_tmp8CE;struct Cyc_Absyn_Fndecl*
_tmp8D0;struct Cyc_Absyn_Tvar*_tmp8D2;struct Cyc_Absyn_Vardecl*_tmp8D3;int _tmp8D4;
struct Cyc_Absyn_Exp*_tmp8D5;struct Cyc_Absyn_Exp*_tmp8D7;struct Cyc_Absyn_Tvar*
_tmp8D8;struct Cyc_Absyn_Vardecl*_tmp8D9;_LL395: {struct Cyc_Absyn_Var_d_struct*
_tmp8C8=(struct Cyc_Absyn_Var_d_struct*)_tmp8C7;if(_tmp8C8->tag != 0)goto _LL397;
else{_tmp8C9=_tmp8C8->f1;}}_LL396: {struct Cyc_Toc_Env _tmp8DB;struct _RegionHandle*
_tmp8DC;struct Cyc_Toc_Env*_tmp8DA=nv;_tmp8DB=*_tmp8DA;_tmp8DC=_tmp8DB.rgn;{
struct Cyc_Absyn_Local_b_struct _tmpDDD;struct Cyc_Absyn_Local_b_struct*_tmpDDC;
struct Cyc_Toc_Env*_tmp8DD=Cyc_Toc_add_varmap(_tmp8DC,nv,_tmp8C9->name,Cyc_Absyn_varb_exp(
_tmp8C9->name,(void*)((_tmpDDC=_cycalloc(sizeof(*_tmpDDC)),((_tmpDDC[0]=((
_tmpDDD.tag=4,((_tmpDDD.f1=_tmp8C9,_tmpDDD)))),_tmpDDC)))),0));Cyc_Toc_local_decl_to_c(
_tmp8DD,_tmp8DD,_tmp8C9,_tmp8A0);}goto _LL394;}_LL397: {struct Cyc_Absyn_Let_d_struct*
_tmp8CA=(struct Cyc_Absyn_Let_d_struct*)_tmp8C7;if(_tmp8CA->tag != 2)goto _LL399;
else{_tmp8CB=_tmp8CA->f1;_tmp8CC=_tmp8CA->f3;}}_LL398:{void*_tmp8E0=_tmp8CB->r;
struct Cyc_Absyn_Vardecl*_tmp8E2;struct Cyc_Absyn_Pat*_tmp8E3;struct Cyc_Absyn_Pat
_tmp8E4;void*_tmp8E5;_LL3A4: {struct Cyc_Absyn_Var_p_struct*_tmp8E1=(struct Cyc_Absyn_Var_p_struct*)
_tmp8E0;if(_tmp8E1->tag != 1)goto _LL3A6;else{_tmp8E2=_tmp8E1->f1;_tmp8E3=_tmp8E1->f2;
_tmp8E4=*_tmp8E3;_tmp8E5=_tmp8E4.r;{struct Cyc_Absyn_Wild_p_struct*_tmp8E6=(
struct Cyc_Absyn_Wild_p_struct*)_tmp8E5;if(_tmp8E6->tag != 0)goto _LL3A6;};}}_LL3A5: {
struct _tuple0*old_name=_tmp8E2->name;struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp8E2->name=new_name;_tmp8E2->initializer=(struct Cyc_Absyn_Exp*)_tmp8CC;{
struct Cyc_Absyn_Var_d_struct _tmpDE0;struct Cyc_Absyn_Var_d_struct*_tmpDDF;_tmp89F->r=(
void*)((_tmpDDF=_cycalloc(sizeof(*_tmpDDF)),((_tmpDDF[0]=((_tmpDE0.tag=0,((
_tmpDE0.f1=_tmp8E2,_tmpDE0)))),_tmpDDF))));}{struct Cyc_Toc_Env _tmp8EA;struct
_RegionHandle*_tmp8EB;struct Cyc_Toc_Env*_tmp8E9=nv;_tmp8EA=*_tmp8E9;_tmp8EB=
_tmp8EA.rgn;{struct Cyc_Absyn_Local_b_struct _tmpDE3;struct Cyc_Absyn_Local_b_struct*
_tmpDE2;struct Cyc_Toc_Env*_tmp8EC=Cyc_Toc_add_varmap(_tmp8EB,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2[0]=((_tmpDE3.tag=
4,((_tmpDE3.f1=_tmp8E2,_tmpDE3)))),_tmpDE2)))),0));Cyc_Toc_local_decl_to_c(
_tmp8EC,nv,_tmp8E2,_tmp8A0);}goto _LL3A3;};}_LL3A6:;_LL3A7: s->r=(Cyc_Toc_letdecl_to_c(
nv,_tmp8CB,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp8CC->topt))->v,_tmp8CC,
_tmp8A0))->r;goto _LL3A3;_LL3A3:;}goto _LL394;_LL399: {struct Cyc_Absyn_Letv_d_struct*
_tmp8CD=(struct Cyc_Absyn_Letv_d_struct*)_tmp8C7;if(_tmp8CD->tag != 3)goto _LL39B;
else{_tmp8CE=_tmp8CD->f1;}}_LL39A: {struct Cyc_List_List*_tmp8EF=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp8CE);if(_tmp8EF == 0){const char*_tmpDE6;
void*_tmpDE5;(_tmpDE5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpDE6="empty Letv_d",_tag_dyneither(_tmpDE6,sizeof(char),
13))),_tag_dyneither(_tmpDE5,sizeof(void*),0)));}{struct Cyc_Absyn_Var_d_struct
_tmpDE9;struct Cyc_Absyn_Var_d_struct*_tmpDE8;_tmp89F->r=(void*)((_tmpDE8=
_cycalloc(sizeof(*_tmpDE8)),((_tmpDE8[0]=((_tmpDE9.tag=0,((_tmpDE9.f1=(struct Cyc_Absyn_Vardecl*)
_tmp8EF->hd,_tmpDE9)))),_tmpDE8))));}_tmp8EF=_tmp8EF->tl;for(0;_tmp8EF != 0;
_tmp8EF=_tmp8EF->tl){struct Cyc_Absyn_Var_d_struct _tmpDEC;struct Cyc_Absyn_Var_d_struct*
_tmpDEB;struct Cyc_Absyn_Decl*_tmp8F4=Cyc_Absyn_new_decl((void*)((_tmpDEB=
_cycalloc(sizeof(*_tmpDEB)),((_tmpDEB[0]=((_tmpDEC.tag=0,((_tmpDEC.f1=(struct Cyc_Absyn_Vardecl*)
_tmp8EF->hd,_tmpDEC)))),_tmpDEB)))),0);s->r=(Cyc_Absyn_decl_stmt(_tmp8F4,Cyc_Absyn_new_stmt(
s->r,0),0))->r;}Cyc_Toc_stmt_to_c(nv,s);goto _LL394;}_LL39B: {struct Cyc_Absyn_Fn_d_struct*
_tmp8CF=(struct Cyc_Absyn_Fn_d_struct*)_tmp8C7;if(_tmp8CF->tag != 1)goto _LL39D;
else{_tmp8D0=_tmp8CF->f1;}}_LL39C: {struct _tuple0*_tmp8F7=_tmp8D0->name;struct
Cyc_Toc_Env _tmp8F9;struct _RegionHandle*_tmp8FA;struct Cyc_Toc_Env*_tmp8F8=nv;
_tmp8F9=*_tmp8F8;_tmp8FA=_tmp8F9.rgn;{struct Cyc_Toc_Env*_tmp8FB=Cyc_Toc_add_varmap(
_tmp8FA,nv,_tmp8D0->name,Cyc_Absyn_var_exp(_tmp8F7,0));Cyc_Toc_fndecl_to_c(
_tmp8FB,_tmp8D0,0);Cyc_Toc_stmt_to_c(_tmp8FB,_tmp8A0);}goto _LL394;}_LL39D: {
struct Cyc_Absyn_Region_d_struct*_tmp8D1=(struct Cyc_Absyn_Region_d_struct*)
_tmp8C7;if(_tmp8D1->tag != 4)goto _LL39F;else{_tmp8D2=_tmp8D1->f1;_tmp8D3=_tmp8D1->f2;
_tmp8D4=_tmp8D1->f3;_tmp8D5=_tmp8D1->f4;}}_LL39E: {struct Cyc_Absyn_Stmt*_tmp8FC=
_tmp8A0;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple0*
rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp8D3->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp8FE;struct _RegionHandle*_tmp8FF;struct Cyc_Toc_Env*
_tmp8FD=nv;_tmp8FE=*_tmp8FD;_tmp8FF=_tmp8FE.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp8FF,nv,x_var,x_exp),_tmp8FC);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp8FC,0))->r;
else{if(_tmp8D5 == 0){struct Cyc_Absyn_Exp*_tmpDEF[1];struct Cyc_Absyn_Exp*_tmpDEE[
1];struct Cyc_List_List*_tmpDED;s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpDED=
_cycalloc(sizeof(*_tmpDED)),((_tmpDED->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0),((_tmpDED->tl=0,_tmpDED)))))),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpDEE[0]=
x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDEE,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp8FC,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpDEF[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDEF,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp8D5);{struct Cyc_Absyn_Exp*
_tmpDF1[1];struct Cyc_Absyn_Exp*_tmpDF0[2];s->r=(Cyc_Absyn_declare_stmt(rh_var,
Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpDF0[1]=(struct Cyc_Absyn_Exp*)_tmp8D5,((_tmpDF0[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDF0,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp8FC,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpDF1[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDF1,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
return;}_LL39F: {struct Cyc_Absyn_Alias_d_struct*_tmp8D6=(struct Cyc_Absyn_Alias_d_struct*)
_tmp8C7;if(_tmp8D6->tag != 5)goto _LL3A1;else{_tmp8D7=_tmp8D6->f1;_tmp8D8=_tmp8D6->f2;
_tmp8D9=_tmp8D6->f3;}}_LL3A0: {struct _tuple0*old_name=_tmp8D9->name;struct
_tuple0*new_name=Cyc_Toc_temp_var();_tmp8D9->name=new_name;_tmp8D9->initializer=(
struct Cyc_Absyn_Exp*)_tmp8D7;{struct Cyc_Absyn_Decl_s_struct _tmpE00;struct Cyc_Absyn_Var_d_struct*
_tmpDFF;struct Cyc_Absyn_Var_d_struct _tmpDFE;struct Cyc_Absyn_Decl*_tmpDFD;struct
Cyc_Absyn_Decl_s_struct*_tmpDFC;s->r=(void*)((_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((
_tmpDFC[0]=((_tmpE00.tag=12,((_tmpE00.f1=((_tmpDFD=_cycalloc(sizeof(*_tmpDFD)),((
_tmpDFD->r=(void*)((_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((_tmpDFF[0]=((_tmpDFE.tag=
0,((_tmpDFE.f1=_tmp8D9,_tmpDFE)))),_tmpDFF)))),((_tmpDFD->loc=0,_tmpDFD)))))),((
_tmpE00.f2=_tmp8A0,_tmpE00)))))),_tmpDFC))));}{struct Cyc_Toc_Env _tmp90B;struct
_RegionHandle*_tmp90C;struct Cyc_Toc_Env*_tmp90A=nv;_tmp90B=*_tmp90A;_tmp90C=
_tmp90B.rgn;{struct Cyc_Absyn_Local_b_struct _tmpE03;struct Cyc_Absyn_Local_b_struct*
_tmpE02;struct Cyc_Toc_Env*_tmp90D=Cyc_Toc_add_varmap(_tmp90C,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpE02=_cycalloc(sizeof(*_tmpE02)),((_tmpE02[0]=((_tmpE03.tag=
4,((_tmpE03.f1=_tmp8D9,_tmpE03)))),_tmpE02)))),0));Cyc_Toc_local_decl_to_c(
_tmp90D,nv,_tmp8D9,_tmp8A0);}return;};}_LL3A1:;_LL3A2: {const char*_tmpE06;void*
_tmpE05;(_tmpE05=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpE06="bad nested declaration within function",_tag_dyneither(_tmpE06,sizeof(
char),39))),_tag_dyneither(_tmpE05,sizeof(void*),0)));}_LL394:;}return;_LL38C: {
struct Cyc_Absyn_Label_s_struct*_tmp8A1=(struct Cyc_Absyn_Label_s_struct*)_tmp87A;
if(_tmp8A1->tag != 13)goto _LL38E;else{_tmp8A2=_tmp8A1->f1;_tmp8A3=_tmp8A1->f2;}}
_LL38D: s=_tmp8A3;continue;_LL38E: {struct Cyc_Absyn_Do_s_struct*_tmp8A4=(struct
Cyc_Absyn_Do_s_struct*)_tmp87A;if(_tmp8A4->tag != 14)goto _LL390;else{_tmp8A5=
_tmp8A4->f1;_tmp8A6=_tmp8A4->f2;_tmp8A7=_tmp8A6.f1;}}_LL38F: {struct Cyc_Toc_Env
_tmp913;struct _RegionHandle*_tmp914;struct Cyc_Toc_Env*_tmp912=nv;_tmp913=*
_tmp912;_tmp914=_tmp913.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp914,nv),
_tmp8A5);Cyc_Toc_exp_to_c(nv,_tmp8A7);return;}_LL390: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp8A8=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp87A;if(_tmp8A8->tag != 15)goto
_LL392;else{_tmp8A9=_tmp8A8->f1;_tmp8AA=_tmp8A8->f2;}}_LL391: {struct _tuple0*
h_var=Cyc_Toc_temp_var();struct _tuple0*e_var=Cyc_Toc_temp_var();struct _tuple0*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);{struct Cyc_Core_Opt*_tmpE07;
e_exp->topt=((_tmpE07=_cycalloc(sizeof(*_tmpE07)),((_tmpE07->v=e_typ,_tmpE07))));}{
struct Cyc_Toc_Env _tmp917;struct _RegionHandle*_tmp918;struct Cyc_Toc_Env*_tmp916=
nv;_tmp917=*_tmp916;_tmp918=_tmp917.rgn;{struct Cyc_Toc_Env*_tmp919=Cyc_Toc_add_varmap(
_tmp918,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp919,_tmp8A9);{struct Cyc_Absyn_Stmt*
_tmp91A=Cyc_Absyn_seq_stmt(_tmp8A9,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple0*_tmp91B=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp91C=
Cyc_Absyn_var_exp(_tmp91B,0);struct Cyc_Absyn_Vardecl*_tmp91D=Cyc_Absyn_new_vardecl(
_tmp91B,Cyc_Absyn_exn_typ(),0);{struct Cyc_Core_Opt*_tmpE08;_tmp91C->topt=((
_tmpE08=_cycalloc(sizeof(*_tmpE08)),((_tmpE08->v=Cyc_Absyn_exn_typ(),_tmpE08))));}{
struct Cyc_Core_Opt*_tmpE19;struct Cyc_Absyn_Var_p_struct*_tmpE18;struct Cyc_Absyn_Pat*
_tmpE17;struct Cyc_Core_Opt*_tmpE16;struct Cyc_Absyn_Var_p_struct _tmpE15;struct Cyc_Absyn_Pat*
_tmpE14;struct Cyc_Absyn_Pat*_tmp91F=(_tmpE14=_cycalloc(sizeof(*_tmpE14)),((
_tmpE14->r=(void*)((_tmpE18=_cycalloc(sizeof(*_tmpE18)),((_tmpE18[0]=((_tmpE15.tag=
1,((_tmpE15.f1=_tmp91D,((_tmpE15.f2=((_tmpE17=_cycalloc(sizeof(*_tmpE17)),((
_tmpE17->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpE17->topt=((_tmpE16=_cycalloc(
sizeof(*_tmpE16)),((_tmpE16->v=Cyc_Absyn_exn_typ(),_tmpE16)))),((_tmpE17->loc=0,
_tmpE17)))))))),_tmpE15)))))),_tmpE18)))),((_tmpE14->topt=((_tmpE19=_cycalloc(
sizeof(*_tmpE19)),((_tmpE19->v=Cyc_Absyn_exn_typ(),_tmpE19)))),((_tmpE14->loc=0,
_tmpE14)))))));struct Cyc_Absyn_Exp*_tmp920=Cyc_Absyn_throw_exp(_tmp91C,0);{
struct Cyc_Core_Opt*_tmpE1A;_tmp920->topt=((_tmpE1A=_cycalloc(sizeof(*_tmpE1A)),((
_tmpE1A->v=(void*)& Cyc_Absyn_VoidType_val,_tmpE1A))));}{struct Cyc_Absyn_Stmt*
_tmp922=Cyc_Absyn_exp_stmt(_tmp920,0);struct Cyc_Core_Opt*_tmpE20;struct Cyc_List_List*
_tmpE1F;struct Cyc_Absyn_Switch_clause*_tmpE1E;struct Cyc_Absyn_Switch_clause*
_tmp923=(_tmpE1E=_cycalloc(sizeof(*_tmpE1E)),((_tmpE1E->pattern=_tmp91F,((
_tmpE1E->pat_vars=((_tmpE20=_cycalloc(sizeof(*_tmpE20)),((_tmpE20->v=((_tmpE1F=
_cycalloc(sizeof(*_tmpE1F)),((_tmpE1F->hd=_tmp91D,((_tmpE1F->tl=0,_tmpE1F)))))),
_tmpE20)))),((_tmpE1E->where_clause=0,((_tmpE1E->body=_tmp922,((_tmpE1E->loc=0,
_tmpE1E)))))))))));struct Cyc_List_List*_tmpE21;struct Cyc_Absyn_Stmt*_tmp924=Cyc_Absyn_switch_stmt(
e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp8AA,((_tmpE21=_cycalloc(sizeof(*_tmpE21)),((_tmpE21->hd=_tmp923,((_tmpE21->tl=
0,_tmpE21))))))),0);Cyc_Toc_stmt_to_c(_tmp919,_tmp924);{struct Cyc_List_List*
_tmpE22;struct Cyc_Absyn_Exp*_tmp925=Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,((
_tmpE22=_cycalloc(sizeof(*_tmpE22)),((_tmpE22->hd=Cyc_Absyn_aggrmember_exp(h_exp,
Cyc_Toc_handler_sp,0),((_tmpE22->tl=0,_tmpE22)))))),0);struct Cyc_List_List*
_tmpE23;struct Cyc_Absyn_Stmt*_tmp926=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpE23=_cycalloc(sizeof(*_tmpE23)),((_tmpE23->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpE23->tl=0,_tmpE23)))))),0),0);struct Cyc_Absyn_Exp*_tmp927=Cyc_Absyn_int_exp(
Cyc_Absyn_Signed,0,0);struct Cyc_Absyn_Exp*_tmp928=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
1,0);s->r=(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp926,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp927,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp925,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp928,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp91A,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp924,0),0),0),0),0),0))->r;};};};};}
return;};}_LL392: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp8AB=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp87A;if(_tmp8AB->tag != 16)goto _LL371;else{_tmp8AC=_tmp8AB->f1;}}_LL393: if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp8AC);{struct Cyc_List_List*
_tmpE24;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((
_tmpE24=_cycalloc(sizeof(*_tmpE24)),((_tmpE24->hd=_tmp8AC,((_tmpE24->tl=0,
_tmpE24)))))),0));};}return;_LL371:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);struct _tuple27{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp936=_new_region("frgn");struct _RegionHandle*frgn=& _tmp936;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp937=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp938=f->args;for(0;_tmp938 != 0;_tmp938=_tmp938->tl){struct
_tuple0*_tmpE25;struct _tuple0*_tmp939=(_tmpE25=_cycalloc(sizeof(*_tmpE25)),((
_tmpE25->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpE25->f2=(*((struct
_tuple27*)_tmp938->hd)).f1,_tmpE25)))));(*((struct _tuple27*)_tmp938->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple27*)_tmp938->hd)).f3);_tmp937=Cyc_Toc_add_varmap(frgn,_tmp937,
_tmp939,Cyc_Absyn_var_exp(_tmp939,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp93C;struct Cyc_Absyn_Tqual
_tmp93D;void*_tmp93E;int _tmp93F;struct Cyc_Absyn_VarargInfo _tmp93B=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp93C=_tmp93B.name;_tmp93D=_tmp93B.tq;_tmp93E=
_tmp93B.type;_tmp93F=_tmp93B.inject;{void*_tmp940=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp93E,(void*)& Cyc_Absyn_HeapRgn_val,_tmp93D,Cyc_Absyn_false_conref));struct
_tuple0*_tmpE26;struct _tuple0*_tmp941=(_tmpE26=_cycalloc(sizeof(*_tmpE26)),((
_tmpE26->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpE26->f2=(struct
_dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null(_tmp93C))->v,_tmpE26)))));{
struct _tuple27*_tmpE29;struct Cyc_List_List*_tmpE28;f->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpE28=
_cycalloc(sizeof(*_tmpE28)),((_tmpE28->hd=((_tmpE29=_cycalloc(sizeof(*_tmpE29)),((
_tmpE29->f1=(struct _dyneither_ptr*)_tmp93C->v,((_tmpE29->f2=_tmp93D,((_tmpE29->f3=
_tmp940,_tmpE29)))))))),((_tmpE28->tl=0,_tmpE28)))))));}_tmp937=Cyc_Toc_add_varmap(
frgn,_tmp937,_tmp941,Cyc_Absyn_var_exp(_tmp941,0));f->cyc_varargs=0;};}{struct
Cyc_List_List*_tmp945=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp945 != 0;_tmp945=_tmp945->tl){((struct Cyc_Absyn_Vardecl*)_tmp945->hd)->type=
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp945->hd)->type);}}Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel(frgn,_tmp937),f->body);};_pop_region(frgn);};}static enum 
Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){switch(s){case Cyc_Absyn_Abstract:
_LL3A8: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3A9: return Cyc_Absyn_Extern;
default: _LL3AA: return s;}}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad,int add_to_decls){struct _tuple0*_tmp946=ad->name;struct _DynRegionHandle*
_tmp948;struct Cyc_Dict_Dict*_tmp949;struct Cyc_Toc_TocState _tmp947=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp948=_tmp947.dyn;_tmp949=_tmp947.aggrs_so_far;{
struct _DynRegionFrame _tmp94A;struct _RegionHandle*d=_open_dynregion(& _tmp94A,
_tmp948);{int seen_defn_before;struct _tuple15**_tmp94B=((struct _tuple15**(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp949,_tmp946);if(
_tmp94B == 0){seen_defn_before=0;{struct _tuple15*v;if(ad->kind == Cyc_Absyn_StructA){
struct _tuple15*_tmpE2A;v=((_tmpE2A=_region_malloc(d,sizeof(*_tmpE2A)),((_tmpE2A->f1=
ad,((_tmpE2A->f2=Cyc_Absyn_strctq(_tmp946),_tmpE2A))))));}else{struct _tuple15*
_tmpE2B;v=((_tmpE2B=_region_malloc(d,sizeof(*_tmpE2B)),((_tmpE2B->f1=ad,((
_tmpE2B->f2=Cyc_Absyn_unionq_typ(_tmp946),_tmpE2B))))));}*_tmp949=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp949,
_tmp946,v);};}else{struct _tuple15 _tmp94F;struct Cyc_Absyn_Aggrdecl*_tmp950;void*
_tmp951;struct _tuple15*_tmp94E=*_tmp94B;_tmp94F=*_tmp94E;_tmp950=_tmp94F.f1;
_tmp951=_tmp94F.f2;if(_tmp950->impl == 0){{struct _tuple15*_tmpE2C;*_tmp949=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))
Cyc_Dict_insert)(*_tmp949,_tmp946,((_tmpE2C=_region_malloc(d,sizeof(*_tmpE2C)),((
_tmpE2C->f1=ad,((_tmpE2C->f2=_tmp951,_tmpE2C)))))));}seen_defn_before=0;}else{
seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*_tmpE2D;struct Cyc_Absyn_Aggrdecl*
new_ad=(_tmpE2D=_cycalloc(sizeof(*_tmpE2D)),((_tmpE2D->kind=ad->kind,((_tmpE2D->sc=
Cyc_Absyn_Public,((_tmpE2D->name=ad->name,((_tmpE2D->tvs=0,((_tmpE2D->impl=0,((
_tmpE2D->attributes=ad->attributes,_tmpE2D)))))))))))));if(ad->impl != 0  && !
seen_defn_before){{struct Cyc_Absyn_AggrdeclImpl*_tmpE2E;new_ad->impl=((_tmpE2E=
_cycalloc(sizeof(*_tmpE2E)),((_tmpE2E->exist_vars=0,((_tmpE2E->rgn_po=0,((
_tmpE2E->fields=0,((_tmpE2E->tagged=0,_tmpE2E))))))))));}{struct Cyc_List_List*
new_fields=0;{struct Cyc_List_List*_tmp954=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp954 != 0;_tmp954=_tmp954->tl){struct Cyc_Absyn_Aggrfield*
_tmp955=(struct Cyc_Absyn_Aggrfield*)_tmp954->hd;if(_tmp954->tl == 0  && Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp955->type)))continue;{struct Cyc_Absyn_Aggrfield*
_tmpE2F;struct Cyc_Absyn_Aggrfield*_tmp956=(_tmpE2F=_cycalloc(sizeof(*_tmpE2F)),((
_tmpE2F->name=_tmp955->name,((_tmpE2F->tq=Cyc_Toc_mt_tq,((_tmpE2F->type=Cyc_Toc_typ_to_c_array(
_tmp955->type),((_tmpE2F->width=_tmp955->width,((_tmpE2F->attributes=_tmp955->attributes,
_tmpE2F)))))))))));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp957=_tmp956->type;struct _dyneither_ptr*_tmp958=_tmp956->name;const char*
_tmpE34;void*_tmpE33[2];struct Cyc_String_pa_struct _tmpE32;struct Cyc_String_pa_struct
_tmpE31;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpE31.tag=0,((_tmpE31.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp958),((_tmpE32.tag=0,((_tmpE32.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpE33[0]=&
_tmpE32,((_tmpE33[1]=& _tmpE31,Cyc_aprintf(((_tmpE34="_union_%s_%s",
_tag_dyneither(_tmpE34,sizeof(char),13))),_tag_dyneither(_tmpE33,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpE35;struct _dyneither_ptr*str=(_tmpE35=_cycalloc(sizeof(*
_tmpE35)),((_tmpE35[0]=s,_tmpE35)));struct Cyc_Absyn_Aggrfield*_tmpE36;struct Cyc_Absyn_Aggrfield*
_tmp959=(_tmpE36=_cycalloc(sizeof(*_tmpE36)),((_tmpE36->name=Cyc_Toc_val_sp,((
_tmpE36->tq=Cyc_Toc_mt_tq,((_tmpE36->type=_tmp957,((_tmpE36->width=0,((_tmpE36->attributes=
0,_tmpE36)))))))))));struct Cyc_Absyn_Aggrfield*_tmpE37;struct Cyc_Absyn_Aggrfield*
_tmp95A=(_tmpE37=_cycalloc(sizeof(*_tmpE37)),((_tmpE37->name=Cyc_Toc_tag_sp,((
_tmpE37->tq=Cyc_Toc_mt_tq,((_tmpE37->type=Cyc_Absyn_sint_typ,((_tmpE37->width=0,((
_tmpE37->attributes=0,_tmpE37)))))))))));struct Cyc_Absyn_Aggrfield*_tmpE38[2];
struct Cyc_List_List*_tmp95B=(_tmpE38[1]=_tmp959,((_tmpE38[0]=_tmp95A,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpE38,sizeof(struct Cyc_Absyn_Aggrfield*),
2)))));struct Cyc_Absyn_AggrdeclImpl*_tmpE3D;struct _tuple0*_tmpE3C;struct Cyc_Absyn_Aggrdecl*
_tmpE3B;struct Cyc_Absyn_Aggrdecl*_tmp95C=(_tmpE3B=_cycalloc(sizeof(*_tmpE3B)),((
_tmpE3B->kind=Cyc_Absyn_StructA,((_tmpE3B->sc=Cyc_Absyn_Public,((_tmpE3B->name=((
_tmpE3C=_cycalloc(sizeof(*_tmpE3C)),((_tmpE3C->f1=Cyc_Absyn_Loc_n,((_tmpE3C->f2=
str,_tmpE3C)))))),((_tmpE3B->tvs=0,((_tmpE3B->impl=((_tmpE3D=_cycalloc(sizeof(*
_tmpE3D)),((_tmpE3D->exist_vars=0,((_tmpE3D->rgn_po=0,((_tmpE3D->fields=_tmp95B,((
_tmpE3D->tagged=0,_tmpE3D)))))))))),((_tmpE3B->attributes=0,_tmpE3B)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmpE43;struct Cyc_Absyn_Aggr_d_struct _tmpE42;
struct Cyc_List_List*_tmpE41;Cyc_Toc_result_decls=((_tmpE41=_cycalloc(sizeof(*
_tmpE41)),((_tmpE41->hd=Cyc_Absyn_new_decl((void*)((_tmpE43=_cycalloc(sizeof(*
_tmpE43)),((_tmpE43[0]=((_tmpE42.tag=6,((_tmpE42.f1=_tmp95C,_tmpE42)))),_tmpE43)))),
0),((_tmpE41->tl=Cyc_Toc_result_decls,_tmpE41))))));}_tmp956->type=Cyc_Absyn_strct(
str);}{struct Cyc_List_List*_tmpE44;new_fields=((_tmpE44=_cycalloc(sizeof(*
_tmpE44)),((_tmpE44->hd=_tmp956,((_tmpE44->tl=new_fields,_tmpE44))))));};};}}(
new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_fields);};}if(add_to_decls){struct Cyc_Absyn_Decl*_tmpE4E;struct Cyc_Absyn_Aggr_d_struct
_tmpE4D;struct Cyc_Absyn_Aggr_d_struct*_tmpE4C;struct Cyc_List_List*_tmpE4B;Cyc_Toc_result_decls=((
_tmpE4B=_cycalloc(sizeof(*_tmpE4B)),((_tmpE4B->hd=((_tmpE4E=_cycalloc(sizeof(*
_tmpE4E)),((_tmpE4E->r=(void*)((_tmpE4C=_cycalloc(sizeof(*_tmpE4C)),((_tmpE4C[0]=((
_tmpE4D.tag=6,((_tmpE4D.f1=new_ad,_tmpE4D)))),_tmpE4C)))),((_tmpE4E->loc=0,
_tmpE4E)))))),((_tmpE4B->tl=Cyc_Toc_result_decls,_tmpE4B))))));}};};
_pop_dynregion(d);};}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud){struct _DynRegionHandle*_tmp973;struct Cyc_Set_Set**_tmp974;struct Cyc_Toc_TocState
_tmp972=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp973=
_tmp972.dyn;_tmp974=_tmp972.datatypes_so_far;{struct _DynRegionFrame _tmp975;
struct _RegionHandle*d=_open_dynregion(& _tmp975,_tmp973);{struct _tuple0*_tmp976=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))
Cyc_Set_member)(*_tmp974,_tmp976)){_npop_handler(0);return;}*_tmp974=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*
_tmp974,_tmp976);}{struct Cyc_List_List*_tmp977=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp977 != 0;_tmp977=_tmp977->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp977->hd;struct Cyc_List_List*_tmp978=0;int i=
1;{struct Cyc_List_List*_tmp979=f->typs;for(0;_tmp979 != 0;(_tmp979=_tmp979->tl,i
++)){struct _dyneither_ptr*_tmp97A=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpE4F;struct Cyc_Absyn_Aggrfield*_tmp97B=(_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((
_tmpE4F->name=_tmp97A,((_tmpE4F->tq=(*((struct _tuple10*)_tmp979->hd)).f1,((
_tmpE4F->type=Cyc_Toc_typ_to_c_array((*((struct _tuple10*)_tmp979->hd)).f2),((
_tmpE4F->width=0,((_tmpE4F->attributes=0,_tmpE4F)))))))))));struct Cyc_List_List*
_tmpE50;_tmp978=((_tmpE50=_cycalloc(sizeof(*_tmpE50)),((_tmpE50->hd=_tmp97B,((
_tmpE50->tl=_tmp978,_tmpE50))))));}}{struct Cyc_Absyn_Aggrfield*_tmpE53;struct Cyc_List_List*
_tmpE52;_tmp978=((_tmpE52=_cycalloc(sizeof(*_tmpE52)),((_tmpE52->hd=((_tmpE53=
_cycalloc(sizeof(*_tmpE53)),((_tmpE53->name=Cyc_Toc_tag_sp,((_tmpE53->tq=Cyc_Toc_mt_tq,((
_tmpE53->type=Cyc_Absyn_sint_typ,((_tmpE53->width=0,((_tmpE53->attributes=0,
_tmpE53)))))))))))),((_tmpE52->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp978),_tmpE52))))));}{struct Cyc_Absyn_AggrdeclImpl*
_tmpE58;const char*_tmpE57;struct Cyc_Absyn_Aggrdecl*_tmpE56;struct Cyc_Absyn_Aggrdecl*
_tmp980=(_tmpE56=_cycalloc(sizeof(*_tmpE56)),((_tmpE56->kind=Cyc_Absyn_StructA,((
_tmpE56->sc=Cyc_Absyn_Public,((_tmpE56->name=Cyc_Toc_collapse_qvar_tag(f->name,((
_tmpE57="_struct",_tag_dyneither(_tmpE57,sizeof(char),8)))),((_tmpE56->tvs=0,((
_tmpE56->impl=((_tmpE58=_cycalloc(sizeof(*_tmpE58)),((_tmpE58->exist_vars=0,((
_tmpE58->rgn_po=0,((_tmpE58->fields=_tmp978,((_tmpE58->tagged=0,_tmpE58)))))))))),((
_tmpE56->attributes=0,_tmpE56)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpE5E;
struct Cyc_Absyn_Aggr_d_struct _tmpE5D;struct Cyc_List_List*_tmpE5C;Cyc_Toc_result_decls=((
_tmpE5C=_cycalloc(sizeof(*_tmpE5C)),((_tmpE5C->hd=Cyc_Absyn_new_decl((void*)((
_tmpE5E=_cycalloc(sizeof(*_tmpE5E)),((_tmpE5E[0]=((_tmpE5D.tag=6,((_tmpE5D.f1=
_tmp980,_tmpE5D)))),_tmpE5E)))),0),((_tmpE5C->tl=Cyc_Toc_result_decls,_tmpE5C))))));};}};;
_pop_dynregion(d);};}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd){if(xd->fields == 0)return;{struct _DynRegionHandle*_tmp988;struct Cyc_Dict_Dict*
_tmp989;struct Cyc_Toc_TocState _tmp987=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp988=_tmp987.dyn;_tmp989=_tmp987.xdatatypes_so_far;{struct _DynRegionFrame
_tmp98A;struct _RegionHandle*d=_open_dynregion(& _tmp98A,_tmp988);{struct _tuple0*
_tmp98B=xd->name;struct Cyc_List_List*_tmp98C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp98C != 0;_tmp98C=_tmp98C->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp98C->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp98D=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp98E=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp98D,Cyc_Absyn_false_conref,0);int*_tmp98F=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp989,f->name);int
_tmp990;_LL3AD: if(_tmp98F != 0)goto _LL3AF;_LL3AE: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp991=Cyc_Absyn_new_vardecl(f->name,_tmp98E,
initopt);_tmp991->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*_tmpE64;struct Cyc_Absyn_Var_d_struct
_tmpE63;struct Cyc_List_List*_tmpE62;Cyc_Toc_result_decls=((_tmpE62=_cycalloc(
sizeof(*_tmpE62)),((_tmpE62->hd=Cyc_Absyn_new_decl((void*)((_tmpE64=_cycalloc(
sizeof(*_tmpE64)),((_tmpE64[0]=((_tmpE63.tag=0,((_tmpE63.f1=_tmp991,_tmpE63)))),
_tmpE64)))),0),((_tmpE62->tl=Cyc_Toc_result_decls,_tmpE62))))));}*_tmp989=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp989,f->name,f->sc != Cyc_Absyn_Extern);{struct Cyc_List_List*fields=0;int i=1;{
struct Cyc_List_List*_tmp995=f->typs;for(0;_tmp995 != 0;(_tmp995=_tmp995->tl,i ++)){
struct Cyc_Int_pa_struct _tmpE6C;void*_tmpE6B[1];const char*_tmpE6A;struct
_dyneither_ptr*_tmpE69;struct _dyneither_ptr*_tmp996=(_tmpE69=_cycalloc(sizeof(*
_tmpE69)),((_tmpE69[0]=(struct _dyneither_ptr)((_tmpE6C.tag=1,((_tmpE6C.f1=(
unsigned long)i,((_tmpE6B[0]=& _tmpE6C,Cyc_aprintf(((_tmpE6A="f%d",_tag_dyneither(
_tmpE6A,sizeof(char),4))),_tag_dyneither(_tmpE6B,sizeof(void*),1)))))))),_tmpE69)));
struct Cyc_Absyn_Aggrfield*_tmpE6D;struct Cyc_Absyn_Aggrfield*_tmp997=(_tmpE6D=
_cycalloc(sizeof(*_tmpE6D)),((_tmpE6D->name=_tmp996,((_tmpE6D->tq=(*((struct
_tuple10*)_tmp995->hd)).f1,((_tmpE6D->type=Cyc_Toc_typ_to_c_array((*((struct
_tuple10*)_tmp995->hd)).f2),((_tmpE6D->width=0,((_tmpE6D->attributes=0,_tmpE6D)))))))))));
struct Cyc_List_List*_tmpE6E;fields=((_tmpE6E=_cycalloc(sizeof(*_tmpE6E)),((
_tmpE6E->hd=_tmp997,((_tmpE6E->tl=fields,_tmpE6E))))));}}{struct Cyc_Absyn_Aggrfield*
_tmpE71;struct Cyc_List_List*_tmpE70;fields=((_tmpE70=_cycalloc(sizeof(*_tmpE70)),((
_tmpE70->hd=((_tmpE71=_cycalloc(sizeof(*_tmpE71)),((_tmpE71->name=Cyc_Toc_tag_sp,((
_tmpE71->tq=Cyc_Toc_mt_tq,((_tmpE71->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq),((_tmpE71->width=0,((_tmpE71->attributes=0,_tmpE71)))))))))))),((
_tmpE70->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmpE70))))));}{struct Cyc_Absyn_AggrdeclImpl*_tmpE76;const char*_tmpE75;
struct Cyc_Absyn_Aggrdecl*_tmpE74;struct Cyc_Absyn_Aggrdecl*_tmp9A0=(_tmpE74=
_cycalloc(sizeof(*_tmpE74)),((_tmpE74->kind=Cyc_Absyn_StructA,((_tmpE74->sc=Cyc_Absyn_Public,((
_tmpE74->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpE75="_struct",
_tag_dyneither(_tmpE75,sizeof(char),8)))),((_tmpE74->tvs=0,((_tmpE74->impl=((
_tmpE76=_cycalloc(sizeof(*_tmpE76)),((_tmpE76->exist_vars=0,((_tmpE76->rgn_po=0,((
_tmpE76->fields=fields,((_tmpE76->tagged=0,_tmpE76)))))))))),((_tmpE74->attributes=
0,_tmpE74)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpE7C;struct Cyc_Absyn_Aggr_d_struct
_tmpE7B;struct Cyc_List_List*_tmpE7A;Cyc_Toc_result_decls=((_tmpE7A=_cycalloc(
sizeof(*_tmpE7A)),((_tmpE7A->hd=Cyc_Absyn_new_decl((void*)((_tmpE7C=_cycalloc(
sizeof(*_tmpE7C)),((_tmpE7C[0]=((_tmpE7B.tag=6,((_tmpE7B.f1=_tmp9A0,_tmpE7B)))),
_tmpE7C)))),0),((_tmpE7A->tl=Cyc_Toc_result_decls,_tmpE7A))))));}goto _LL3AC;};};};}
_LL3AF: if(_tmp98F == 0)goto _LL3B1;_tmp990=*_tmp98F;if(_tmp990 != 0)goto _LL3B1;
_LL3B0: if(f->sc != Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*_tmp9A7=Cyc_Absyn_string_exp(*
fn,0);struct Cyc_Absyn_Vardecl*_tmp9A8=Cyc_Absyn_new_vardecl(f->name,_tmp98E,(
struct Cyc_Absyn_Exp*)_tmp9A7);_tmp9A8->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*
_tmpE82;struct Cyc_Absyn_Var_d_struct _tmpE81;struct Cyc_List_List*_tmpE80;Cyc_Toc_result_decls=((
_tmpE80=_cycalloc(sizeof(*_tmpE80)),((_tmpE80->hd=Cyc_Absyn_new_decl((void*)((
_tmpE82=_cycalloc(sizeof(*_tmpE82)),((_tmpE82[0]=((_tmpE81.tag=0,((_tmpE81.f1=
_tmp9A8,_tmpE81)))),_tmpE82)))),0),((_tmpE80->tl=Cyc_Toc_result_decls,_tmpE80))))));}*
_tmp989=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp989,f->name,1);}goto _LL3AC;_LL3B1:;_LL3B2: goto _LL3AC;_LL3AC:;}};
_pop_dynregion(d);};};}static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*
ed){ed->sc=Cyc_Absyn_Public;if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(
struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,
struct Cyc_Absyn_Stmt*s){void*old_typ=vd->type;vd->type=Cyc_Toc_typ_to_c_array(
old_typ);if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ))vd->sc=Cyc_Absyn_Public;Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp9AC=init->r;struct Cyc_Absyn_Vardecl*_tmp9AE;struct Cyc_Absyn_Exp*_tmp9AF;
struct Cyc_Absyn_Exp*_tmp9B0;int _tmp9B1;_LL3B4: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp9AD=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp9AC;if(_tmp9AD->tag != 28)
goto _LL3B6;else{_tmp9AE=_tmp9AD->f1;_tmp9AF=_tmp9AD->f2;_tmp9B0=_tmp9AD->f3;
_tmp9B1=_tmp9AD->f4;}}_LL3B5: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp9AE,_tmp9AF,_tmp9B0,_tmp9B1,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL3B3;_LL3B6:;_LL3B7: if(vd->sc == Cyc_Absyn_Static){struct Cyc_Toc_Env
_tmp9B3;struct _RegionHandle*_tmp9B4;struct Cyc_Toc_Env*_tmp9B2=init_nv;_tmp9B3=*
_tmp9B2;_tmp9B4=_tmp9B3.rgn;{struct Cyc_Toc_Env*_tmp9B5=Cyc_Toc_set_toplevel(
_tmp9B4,init_nv);Cyc_Toc_exp_to_c(_tmp9B5,init);};}else{Cyc_Toc_exp_to_c(init_nv,
init);}goto _LL3B3;_LL3B3:;}else{void*_tmp9B6=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_ArrayInfo _tmp9B8;void*_tmp9B9;struct Cyc_Absyn_Exp*_tmp9BA;union Cyc_Absyn_Constraint*
_tmp9BB;_LL3B9:{struct Cyc_Absyn_ArrayType_struct*_tmp9B7=(struct Cyc_Absyn_ArrayType_struct*)
_tmp9B6;if(_tmp9B7->tag != 8)goto _LL3BB;else{_tmp9B8=_tmp9B7->f1;_tmp9B9=_tmp9B8.elt_type;
_tmp9BA=_tmp9B8.num_elts;_tmp9BB=_tmp9B8.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp9BB))goto _LL3BB;_LL3BA: if(_tmp9BA == 0){const char*
_tmpE85;void*_tmpE84;(_tmpE84=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE85="can't initialize zero-terminated array -- size unknown",
_tag_dyneither(_tmpE85,sizeof(char),55))),_tag_dyneither(_tmpE84,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp9BA;struct Cyc_Absyn_Exp*
_tmp9BE=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp9BF=Cyc_Absyn_signed_int_exp(
0,0);s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp9BE,
_tmp9BF,0),0),Cyc_Absyn_new_stmt(s->r,0));goto _LL3B8;};_LL3BB:;_LL3BC: goto _LL3B8;
_LL3B8:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple0*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp9C1;struct
_RegionHandle*_tmp9C2;struct Cyc_Toc_Env*_tmp9C0=nv;_tmp9C1=*_tmp9C0;_tmp9C2=
_tmp9C1.rgn;{struct Cyc_Absyn_Stmt*_tmp9C3=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp9C4=Cyc_Toc_share_env(_tmp9C2,nv);struct Cyc_Toc_Env*_tmp9C6;struct Cyc_List_List*
_tmp9C7;struct Cyc_Absyn_Stmt*_tmp9C8;struct _tuple23 _tmp9C5=Cyc_Toc_xlate_pat(
_tmp9C4,_tmp9C2,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp9C6=_tmp9C5.f1;_tmp9C7=_tmp9C5.f2;_tmp9C8=_tmp9C5.f3;Cyc_Toc_stmt_to_c(
_tmp9C6,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp9C8,s,0),0);for(0;_tmp9C7 != 0;_tmp9C7=_tmp9C7->tl){
struct _tuple24 _tmp9CA;struct _tuple0*_tmp9CB;void*_tmp9CC;struct _tuple24*_tmp9C9=(
struct _tuple24*)_tmp9C7->hd;_tmp9CA=*_tmp9C9;_tmp9CB=_tmp9CA.f1;_tmp9CC=_tmp9CA.f2;
s=Cyc_Absyn_declare_stmt(_tmp9CB,_tmp9CC,0,s,0);}}return s;};}static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e){void*_tmp9CD=e->r;struct Cyc_Absyn_Exp*_tmp9CF;struct Cyc_Absyn_Exp*
_tmp9D1;struct Cyc_Absyn_Exp*_tmp9D3;struct Cyc_Absyn_Exp*_tmp9D5;struct Cyc_Absyn_Exp*
_tmp9D7;struct Cyc_Absyn_Exp*_tmp9D9;struct Cyc_Absyn_Exp*_tmp9DB;struct Cyc_Absyn_Exp*
_tmp9DD;struct Cyc_List_List*_tmp9DF;struct Cyc_Absyn_Exp*_tmp9E1;struct Cyc_Absyn_Exp*
_tmp9E2;struct Cyc_Absyn_Exp*_tmp9E4;struct Cyc_Absyn_Exp*_tmp9E5;struct Cyc_Absyn_Exp*
_tmp9E7;struct Cyc_Absyn_Exp*_tmp9E8;struct Cyc_Absyn_Exp*_tmp9EA;struct Cyc_Absyn_Exp*
_tmp9EB;struct Cyc_Absyn_Exp*_tmp9ED;struct Cyc_Absyn_Exp*_tmp9EE;struct Cyc_Absyn_Exp*
_tmp9F0;struct Cyc_Absyn_Exp*_tmp9F1;struct Cyc_Absyn_Exp*_tmp9F3;struct Cyc_Absyn_Exp*
_tmp9F4;struct Cyc_Absyn_Exp*_tmp9F5;struct Cyc_Absyn_Exp*_tmp9F7;struct Cyc_List_List*
_tmp9F8;struct Cyc_Absyn_Exp*_tmp9FA;struct Cyc_List_List*_tmp9FB;void*_tmp9FD;
void**_tmp9FE;struct Cyc_Absyn_Exp*_tmp9FF;struct _tuple8*_tmpA01;struct _tuple8
_tmpA02;void*_tmpA03;void**_tmpA04;struct Cyc_List_List*_tmpA05;struct Cyc_List_List*
_tmpA07;struct Cyc_List_List*_tmpA09;void*_tmpA0B;void**_tmpA0C;void*_tmpA0E;void**
_tmpA0F;struct Cyc_Absyn_Stmt*_tmpA11;struct Cyc_Absyn_MallocInfo _tmpA13;struct Cyc_Absyn_MallocInfo*
_tmpA14;_LL3BE: {struct Cyc_Absyn_Deref_e_struct*_tmp9CE=(struct Cyc_Absyn_Deref_e_struct*)
_tmp9CD;if(_tmp9CE->tag != 21)goto _LL3C0;else{_tmp9CF=_tmp9CE->f1;}}_LL3BF:
_tmp9D1=_tmp9CF;goto _LL3C1;_LL3C0: {struct Cyc_Absyn_AggrMember_e_struct*_tmp9D0=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp9CD;if(_tmp9D0->tag != 22)goto _LL3C2;
else{_tmp9D1=_tmp9D0->f1;}}_LL3C1: _tmp9D3=_tmp9D1;goto _LL3C3;_LL3C2: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp9D2=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp9CD;if(_tmp9D2->tag != 23)goto
_LL3C4;else{_tmp9D3=_tmp9D2->f1;}}_LL3C3: _tmp9D5=_tmp9D3;goto _LL3C5;_LL3C4: {
struct Cyc_Absyn_Address_e_struct*_tmp9D4=(struct Cyc_Absyn_Address_e_struct*)
_tmp9CD;if(_tmp9D4->tag != 16)goto _LL3C6;else{_tmp9D5=_tmp9D4->f1;}}_LL3C5:
_tmp9D7=_tmp9D5;goto _LL3C7;_LL3C6: {struct Cyc_Absyn_Throw_e_struct*_tmp9D6=(
struct Cyc_Absyn_Throw_e_struct*)_tmp9CD;if(_tmp9D6->tag != 12)goto _LL3C8;else{
_tmp9D7=_tmp9D6->f1;}}_LL3C7: _tmp9D9=_tmp9D7;goto _LL3C9;_LL3C8: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp9D8=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp9CD;if(_tmp9D8->tag != 13)
goto _LL3CA;else{_tmp9D9=_tmp9D8->f1;}}_LL3C9: _tmp9DB=_tmp9D9;goto _LL3CB;_LL3CA: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp9DA=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp9CD;if(_tmp9DA->tag != 19)goto _LL3CC;else{_tmp9DB=_tmp9DA->f1;}}_LL3CB:
_tmp9DD=_tmp9DB;goto _LL3CD;_LL3CC: {struct Cyc_Absyn_Increment_e_struct*_tmp9DC=(
struct Cyc_Absyn_Increment_e_struct*)_tmp9CD;if(_tmp9DC->tag != 5)goto _LL3CE;else{
_tmp9DD=_tmp9DC->f1;}}_LL3CD: Cyc_Toc_exptypes_to_c(_tmp9DD);goto _LL3BD;_LL3CE: {
struct Cyc_Absyn_Primop_e_struct*_tmp9DE=(struct Cyc_Absyn_Primop_e_struct*)
_tmp9CD;if(_tmp9DE->tag != 3)goto _LL3D0;else{_tmp9DF=_tmp9DE->f2;}}_LL3CF:((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,
_tmp9DF);goto _LL3BD;_LL3D0: {struct Cyc_Absyn_And_e_struct*_tmp9E0=(struct Cyc_Absyn_And_e_struct*)
_tmp9CD;if(_tmp9E0->tag != 7)goto _LL3D2;else{_tmp9E1=_tmp9E0->f1;_tmp9E2=_tmp9E0->f2;}}
_LL3D1: _tmp9E4=_tmp9E1;_tmp9E5=_tmp9E2;goto _LL3D3;_LL3D2: {struct Cyc_Absyn_Or_e_struct*
_tmp9E3=(struct Cyc_Absyn_Or_e_struct*)_tmp9CD;if(_tmp9E3->tag != 8)goto _LL3D4;
else{_tmp9E4=_tmp9E3->f1;_tmp9E5=_tmp9E3->f2;}}_LL3D3: _tmp9E7=_tmp9E4;_tmp9E8=
_tmp9E5;goto _LL3D5;_LL3D4: {struct Cyc_Absyn_SeqExp_e_struct*_tmp9E6=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp9CD;if(_tmp9E6->tag != 9)goto _LL3D6;else{_tmp9E7=_tmp9E6->f1;_tmp9E8=_tmp9E6->f2;}}
_LL3D5: _tmp9EA=_tmp9E7;_tmp9EB=_tmp9E8;goto _LL3D7;_LL3D6: {struct Cyc_Absyn_Subscript_e_struct*
_tmp9E9=(struct Cyc_Absyn_Subscript_e_struct*)_tmp9CD;if(_tmp9E9->tag != 24)goto
_LL3D8;else{_tmp9EA=_tmp9E9->f1;_tmp9EB=_tmp9E9->f2;}}_LL3D7: _tmp9ED=_tmp9EA;
_tmp9EE=_tmp9EB;goto _LL3D9;_LL3D8: {struct Cyc_Absyn_Swap_e_struct*_tmp9EC=(
struct Cyc_Absyn_Swap_e_struct*)_tmp9CD;if(_tmp9EC->tag != 35)goto _LL3DA;else{
_tmp9ED=_tmp9EC->f1;_tmp9EE=_tmp9EC->f2;}}_LL3D9: _tmp9F0=_tmp9ED;_tmp9F1=_tmp9EE;
goto _LL3DB;_LL3DA: {struct Cyc_Absyn_AssignOp_e_struct*_tmp9EF=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp9CD;if(_tmp9EF->tag != 4)goto _LL3DC;else{_tmp9F0=_tmp9EF->f1;_tmp9F1=_tmp9EF->f3;}}
_LL3DB: Cyc_Toc_exptypes_to_c(_tmp9F0);Cyc_Toc_exptypes_to_c(_tmp9F1);goto _LL3BD;
_LL3DC: {struct Cyc_Absyn_Conditional_e_struct*_tmp9F2=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp9CD;if(_tmp9F2->tag != 6)goto _LL3DE;else{_tmp9F3=_tmp9F2->f1;_tmp9F4=_tmp9F2->f2;
_tmp9F5=_tmp9F2->f3;}}_LL3DD: Cyc_Toc_exptypes_to_c(_tmp9F3);Cyc_Toc_exptypes_to_c(
_tmp9F4);Cyc_Toc_exptypes_to_c(_tmp9F5);goto _LL3BD;_LL3DE: {struct Cyc_Absyn_FnCall_e_struct*
_tmp9F6=(struct Cyc_Absyn_FnCall_e_struct*)_tmp9CD;if(_tmp9F6->tag != 11)goto
_LL3E0;else{_tmp9F7=_tmp9F6->f1;_tmp9F8=_tmp9F6->f2;}}_LL3DF: _tmp9FA=_tmp9F7;
_tmp9FB=_tmp9F8;goto _LL3E1;_LL3E0: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp9F9=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp9CD;if(_tmp9F9->tag != 10)goto _LL3E2;
else{_tmp9FA=_tmp9F9->f1;_tmp9FB=_tmp9F9->f2;}}_LL3E1: Cyc_Toc_exptypes_to_c(
_tmp9FA);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp9FB);goto _LL3BD;_LL3E2: {struct Cyc_Absyn_Cast_e_struct*
_tmp9FC=(struct Cyc_Absyn_Cast_e_struct*)_tmp9CD;if(_tmp9FC->tag != 15)goto _LL3E4;
else{_tmp9FD=(void**)& _tmp9FC->f1;_tmp9FE=(void**)((void**)& _tmp9FC->f1);_tmp9FF=
_tmp9FC->f2;}}_LL3E3:*_tmp9FE=Cyc_Toc_typ_to_c(*_tmp9FE);Cyc_Toc_exptypes_to_c(
_tmp9FF);goto _LL3BD;_LL3E4: {struct Cyc_Absyn_CompoundLit_e_struct*_tmpA00=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp9CD;if(_tmpA00->tag != 26)goto _LL3E6;
else{_tmpA01=_tmpA00->f1;_tmpA02=*_tmpA01;_tmpA03=_tmpA02.f3;_tmpA04=(void**)&(*
_tmpA00->f1).f3;_tmpA05=_tmpA00->f2;}}_LL3E5:*_tmpA04=Cyc_Toc_typ_to_c(*_tmpA04);
_tmpA07=_tmpA05;goto _LL3E7;_LL3E6: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpA06=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp9CD;if(_tmpA06->tag != 36)
goto _LL3E8;else{_tmpA07=_tmpA06->f2;}}_LL3E7: _tmpA09=_tmpA07;goto _LL3E9;_LL3E8: {
struct Cyc_Absyn_Array_e_struct*_tmpA08=(struct Cyc_Absyn_Array_e_struct*)_tmp9CD;
if(_tmpA08->tag != 27)goto _LL3EA;else{_tmpA09=_tmpA08->f1;}}_LL3E9: for(0;_tmpA09
!= 0;_tmpA09=_tmpA09->tl){struct Cyc_Absyn_Exp*_tmpA25;struct _tuple16 _tmpA24=*((
struct _tuple16*)_tmpA09->hd);_tmpA25=_tmpA24.f2;Cyc_Toc_exptypes_to_c(_tmpA25);}
goto _LL3BD;_LL3EA: {struct Cyc_Absyn_Offsetof_e_struct*_tmpA0A=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp9CD;if(_tmpA0A->tag != 20)goto _LL3EC;else{_tmpA0B=(void**)& _tmpA0A->f1;
_tmpA0C=(void**)((void**)& _tmpA0A->f1);}}_LL3EB: _tmpA0F=_tmpA0C;goto _LL3ED;
_LL3EC: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpA0D=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp9CD;if(_tmpA0D->tag != 18)goto _LL3EE;else{_tmpA0E=(void**)& _tmpA0D->f1;
_tmpA0F=(void**)((void**)& _tmpA0D->f1);}}_LL3ED:*_tmpA0F=Cyc_Toc_typ_to_c(*
_tmpA0F);goto _LL3BD;_LL3EE: {struct Cyc_Absyn_StmtExp_e_struct*_tmpA10=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp9CD;if(_tmpA10->tag != 37)goto _LL3F0;else{_tmpA11=_tmpA10->f1;}}_LL3EF: Cyc_Toc_stmttypes_to_c(
_tmpA11);goto _LL3BD;_LL3F0: {struct Cyc_Absyn_Malloc_e_struct*_tmpA12=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp9CD;if(_tmpA12->tag != 34)goto _LL3F2;else{_tmpA13=_tmpA12->f1;_tmpA14=(struct
Cyc_Absyn_MallocInfo*)& _tmpA12->f1;}}_LL3F1: if(_tmpA14->elt_type != 0){void**
_tmpE86;_tmpA14->elt_type=((_tmpE86=_cycalloc(sizeof(*_tmpE86)),((_tmpE86[0]=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmpA14->elt_type))),_tmpE86))));}Cyc_Toc_exptypes_to_c(
_tmpA14->num_elts);goto _LL3BD;_LL3F2: {struct Cyc_Absyn_Const_e_struct*_tmpA15=(
struct Cyc_Absyn_Const_e_struct*)_tmp9CD;if(_tmpA15->tag != 0)goto _LL3F4;}_LL3F3:
goto _LL3F5;_LL3F4: {struct Cyc_Absyn_Var_e_struct*_tmpA16=(struct Cyc_Absyn_Var_e_struct*)
_tmp9CD;if(_tmpA16->tag != 1)goto _LL3F6;}_LL3F5: goto _LL3F7;_LL3F6: {struct Cyc_Absyn_UnknownId_e_struct*
_tmpA17=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp9CD;if(_tmpA17->tag != 2)goto
_LL3F8;}_LL3F7: goto _LL3F9;_LL3F8: {struct Cyc_Absyn_Enum_e_struct*_tmpA18=(struct
Cyc_Absyn_Enum_e_struct*)_tmp9CD;if(_tmpA18->tag != 32)goto _LL3FA;}_LL3F9: goto
_LL3FB;_LL3FA: {struct Cyc_Absyn_Asm_e_struct*_tmpA19=(struct Cyc_Absyn_Asm_e_struct*)
_tmp9CD;if(_tmpA19->tag != 40)goto _LL3FC;}_LL3FB: goto _LL3FD;_LL3FC: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmpA1A=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp9CD;if(_tmpA1A->tag != 33)goto
_LL3FE;}_LL3FD: goto _LL3BD;_LL3FE: {struct Cyc_Absyn_AnonStruct_e_struct*_tmpA1B=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp9CD;if(_tmpA1B->tag != 30)goto _LL400;}
_LL3FF: goto _LL401;_LL400: {struct Cyc_Absyn_Datatype_e_struct*_tmpA1C=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp9CD;if(_tmpA1C->tag != 31)goto _LL402;}_LL401: goto _LL403;_LL402: {struct Cyc_Absyn_Aggregate_e_struct*
_tmpA1D=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp9CD;if(_tmpA1D->tag != 29)goto
_LL404;}_LL403: goto _LL405;_LL404: {struct Cyc_Absyn_Comprehension_e_struct*
_tmpA1E=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp9CD;if(_tmpA1E->tag != 28)
goto _LL406;}_LL405: goto _LL407;_LL406: {struct Cyc_Absyn_Tuple_e_struct*_tmpA1F=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp9CD;if(_tmpA1F->tag != 25)goto _LL408;}_LL407:
goto _LL409;_LL408: {struct Cyc_Absyn_Instantiate_e_struct*_tmpA20=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp9CD;if(_tmpA20->tag != 14)goto _LL40A;}_LL409: goto _LL40B;_LL40A: {struct Cyc_Absyn_New_e_struct*
_tmpA21=(struct Cyc_Absyn_New_e_struct*)_tmp9CD;if(_tmpA21->tag != 17)goto _LL40C;}
_LL40B: goto _LL40D;_LL40C: {struct Cyc_Absyn_Valueof_e_struct*_tmpA22=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp9CD;if(_tmpA22->tag != 39)goto _LL40E;}_LL40D: goto _LL40F;_LL40E: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmpA23=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp9CD;if(_tmpA23->tag != 38)goto
_LL3BD;}_LL40F:{const char*_tmpE89;void*_tmpE88;(_tmpE88=0,Cyc_Tcutil_terr(e->loc,((
_tmpE89="Cyclone expression within C code",_tag_dyneither(_tmpE89,sizeof(char),
33))),_tag_dyneither(_tmpE88,sizeof(void*),0)));}goto _LL3BD;_LL3BD:;}static void
Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){void*_tmpA29=d->r;struct Cyc_Absyn_Vardecl*
_tmpA2B;struct Cyc_Absyn_Fndecl*_tmpA2D;struct Cyc_Absyn_Aggrdecl*_tmpA2F;struct
Cyc_Absyn_Enumdecl*_tmpA31;struct Cyc_Absyn_Typedefdecl*_tmpA33;_LL411: {struct
Cyc_Absyn_Var_d_struct*_tmpA2A=(struct Cyc_Absyn_Var_d_struct*)_tmpA29;if(_tmpA2A->tag
!= 0)goto _LL413;else{_tmpA2B=_tmpA2A->f1;}}_LL412: _tmpA2B->type=Cyc_Toc_typ_to_c(
_tmpA2B->type);if(_tmpA2B->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmpA2B->initializer));goto _LL410;_LL413: {struct Cyc_Absyn_Fn_d_struct*
_tmpA2C=(struct Cyc_Absyn_Fn_d_struct*)_tmpA29;if(_tmpA2C->tag != 1)goto _LL415;
else{_tmpA2D=_tmpA2C->f1;}}_LL414: _tmpA2D->ret_type=Cyc_Toc_typ_to_c(_tmpA2D->ret_type);{
struct Cyc_List_List*_tmpA3F=_tmpA2D->args;for(0;_tmpA3F != 0;_tmpA3F=_tmpA3F->tl){(*((
struct _tuple27*)_tmpA3F->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple27*)_tmpA3F->hd)).f3);}}
goto _LL410;_LL415: {struct Cyc_Absyn_Aggr_d_struct*_tmpA2E=(struct Cyc_Absyn_Aggr_d_struct*)
_tmpA29;if(_tmpA2E->tag != 6)goto _LL417;else{_tmpA2F=_tmpA2E->f1;}}_LL416: Cyc_Toc_aggrdecl_to_c(
_tmpA2F,1);goto _LL410;_LL417: {struct Cyc_Absyn_Enum_d_struct*_tmpA30=(struct Cyc_Absyn_Enum_d_struct*)
_tmpA29;if(_tmpA30->tag != 8)goto _LL419;else{_tmpA31=_tmpA30->f1;}}_LL418: if(
_tmpA31->fields != 0){struct Cyc_List_List*_tmpA40=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmpA31->fields))->v;for(0;_tmpA40 != 0;_tmpA40=_tmpA40->tl){
struct Cyc_Absyn_Enumfield*_tmpA41=(struct Cyc_Absyn_Enumfield*)_tmpA40->hd;if(
_tmpA41->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmpA41->tag));}}
goto _LL410;_LL419: {struct Cyc_Absyn_Typedef_d_struct*_tmpA32=(struct Cyc_Absyn_Typedef_d_struct*)
_tmpA29;if(_tmpA32->tag != 9)goto _LL41B;else{_tmpA33=_tmpA32->f1;}}_LL41A:{struct
Cyc_Core_Opt*_tmpE8A;_tmpA33->defn=((_tmpE8A=_cycalloc(sizeof(*_tmpE8A)),((
_tmpE8A->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA33->defn))->v),_tmpE8A))));}goto _LL410;_LL41B: {struct Cyc_Absyn_Let_d_struct*
_tmpA34=(struct Cyc_Absyn_Let_d_struct*)_tmpA29;if(_tmpA34->tag != 2)goto _LL41D;}
_LL41C: goto _LL41E;_LL41D: {struct Cyc_Absyn_Letv_d_struct*_tmpA35=(struct Cyc_Absyn_Letv_d_struct*)
_tmpA29;if(_tmpA35->tag != 3)goto _LL41F;}_LL41E: goto _LL420;_LL41F: {struct Cyc_Absyn_Datatype_d_struct*
_tmpA36=(struct Cyc_Absyn_Datatype_d_struct*)_tmpA29;if(_tmpA36->tag != 7)goto
_LL421;}_LL420: goto _LL422;_LL421: {struct Cyc_Absyn_Namespace_d_struct*_tmpA37=(
struct Cyc_Absyn_Namespace_d_struct*)_tmpA29;if(_tmpA37->tag != 10)goto _LL423;}
_LL422: goto _LL424;_LL423: {struct Cyc_Absyn_Using_d_struct*_tmpA38=(struct Cyc_Absyn_Using_d_struct*)
_tmpA29;if(_tmpA38->tag != 11)goto _LL425;}_LL424: goto _LL426;_LL425: {struct Cyc_Absyn_ExternC_d_struct*
_tmpA39=(struct Cyc_Absyn_ExternC_d_struct*)_tmpA29;if(_tmpA39->tag != 12)goto
_LL427;}_LL426: goto _LL428;_LL427: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmpA3A=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmpA29;if(_tmpA3A->tag != 13)
goto _LL429;}_LL428: goto _LL42A;_LL429: {struct Cyc_Absyn_Region_d_struct*_tmpA3B=(
struct Cyc_Absyn_Region_d_struct*)_tmpA29;if(_tmpA3B->tag != 4)goto _LL42B;}_LL42A:
goto _LL42C;_LL42B: {struct Cyc_Absyn_Alias_d_struct*_tmpA3C=(struct Cyc_Absyn_Alias_d_struct*)
_tmpA29;if(_tmpA3C->tag != 5)goto _LL42D;}_LL42C:{const char*_tmpE8D;void*_tmpE8C;(
_tmpE8C=0,Cyc_Tcutil_terr(d->loc,((_tmpE8D="Cyclone declaration within C code",
_tag_dyneither(_tmpE8D,sizeof(char),34))),_tag_dyneither(_tmpE8C,sizeof(void*),0)));}
goto _LL410;_LL42D: {struct Cyc_Absyn_Porton_d_struct*_tmpA3D=(struct Cyc_Absyn_Porton_d_struct*)
_tmpA29;if(_tmpA3D->tag != 14)goto _LL42F;}_LL42E: goto _LL430;_LL42F: {struct Cyc_Absyn_Portoff_d_struct*
_tmpA3E=(struct Cyc_Absyn_Portoff_d_struct*)_tmpA29;if(_tmpA3E->tag != 15)goto
_LL410;}_LL430: goto _LL410;_LL410:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s){void*_tmpA45=s->r;struct Cyc_Absyn_Exp*_tmpA47;struct Cyc_Absyn_Stmt*_tmpA49;
struct Cyc_Absyn_Stmt*_tmpA4A;struct Cyc_Absyn_Exp*_tmpA4C;struct Cyc_Absyn_Exp*
_tmpA4E;struct Cyc_Absyn_Stmt*_tmpA4F;struct Cyc_Absyn_Stmt*_tmpA50;struct _tuple9
_tmpA52;struct Cyc_Absyn_Exp*_tmpA53;struct Cyc_Absyn_Stmt*_tmpA54;struct Cyc_Absyn_Exp*
_tmpA56;struct _tuple9 _tmpA57;struct Cyc_Absyn_Exp*_tmpA58;struct _tuple9 _tmpA59;
struct Cyc_Absyn_Exp*_tmpA5A;struct Cyc_Absyn_Stmt*_tmpA5B;struct Cyc_Absyn_Exp*
_tmpA5D;struct Cyc_List_List*_tmpA5E;struct Cyc_Absyn_Decl*_tmpA60;struct Cyc_Absyn_Stmt*
_tmpA61;struct Cyc_Absyn_Stmt*_tmpA63;struct _tuple9 _tmpA64;struct Cyc_Absyn_Exp*
_tmpA65;_LL432: {struct Cyc_Absyn_Exp_s_struct*_tmpA46=(struct Cyc_Absyn_Exp_s_struct*)
_tmpA45;if(_tmpA46->tag != 1)goto _LL434;else{_tmpA47=_tmpA46->f1;}}_LL433: Cyc_Toc_exptypes_to_c(
_tmpA47);goto _LL431;_LL434: {struct Cyc_Absyn_Seq_s_struct*_tmpA48=(struct Cyc_Absyn_Seq_s_struct*)
_tmpA45;if(_tmpA48->tag != 2)goto _LL436;else{_tmpA49=_tmpA48->f1;_tmpA4A=_tmpA48->f2;}}
_LL435: Cyc_Toc_stmttypes_to_c(_tmpA49);Cyc_Toc_stmttypes_to_c(_tmpA4A);goto
_LL431;_LL436: {struct Cyc_Absyn_Return_s_struct*_tmpA4B=(struct Cyc_Absyn_Return_s_struct*)
_tmpA45;if(_tmpA4B->tag != 3)goto _LL438;else{_tmpA4C=_tmpA4B->f1;}}_LL437: if(
_tmpA4C != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmpA4C);goto _LL431;
_LL438: {struct Cyc_Absyn_IfThenElse_s_struct*_tmpA4D=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmpA45;if(_tmpA4D->tag != 4)goto _LL43A;else{_tmpA4E=_tmpA4D->f1;_tmpA4F=_tmpA4D->f2;
_tmpA50=_tmpA4D->f3;}}_LL439: Cyc_Toc_exptypes_to_c(_tmpA4E);Cyc_Toc_stmttypes_to_c(
_tmpA4F);Cyc_Toc_stmttypes_to_c(_tmpA50);goto _LL431;_LL43A: {struct Cyc_Absyn_While_s_struct*
_tmpA51=(struct Cyc_Absyn_While_s_struct*)_tmpA45;if(_tmpA51->tag != 5)goto _LL43C;
else{_tmpA52=_tmpA51->f1;_tmpA53=_tmpA52.f1;_tmpA54=_tmpA51->f2;}}_LL43B: Cyc_Toc_exptypes_to_c(
_tmpA53);Cyc_Toc_stmttypes_to_c(_tmpA54);goto _LL431;_LL43C: {struct Cyc_Absyn_For_s_struct*
_tmpA55=(struct Cyc_Absyn_For_s_struct*)_tmpA45;if(_tmpA55->tag != 9)goto _LL43E;
else{_tmpA56=_tmpA55->f1;_tmpA57=_tmpA55->f2;_tmpA58=_tmpA57.f1;_tmpA59=_tmpA55->f3;
_tmpA5A=_tmpA59.f1;_tmpA5B=_tmpA55->f4;}}_LL43D: Cyc_Toc_exptypes_to_c(_tmpA56);
Cyc_Toc_exptypes_to_c(_tmpA58);Cyc_Toc_exptypes_to_c(_tmpA5A);Cyc_Toc_stmttypes_to_c(
_tmpA5B);goto _LL431;_LL43E: {struct Cyc_Absyn_Switch_s_struct*_tmpA5C=(struct Cyc_Absyn_Switch_s_struct*)
_tmpA45;if(_tmpA5C->tag != 10)goto _LL440;else{_tmpA5D=_tmpA5C->f1;_tmpA5E=_tmpA5C->f2;}}
_LL43F: Cyc_Toc_exptypes_to_c(_tmpA5D);for(0;_tmpA5E != 0;_tmpA5E=_tmpA5E->tl){Cyc_Toc_stmttypes_to_c(((
struct Cyc_Absyn_Switch_clause*)_tmpA5E->hd)->body);}goto _LL431;_LL440: {struct
Cyc_Absyn_Decl_s_struct*_tmpA5F=(struct Cyc_Absyn_Decl_s_struct*)_tmpA45;if(
_tmpA5F->tag != 12)goto _LL442;else{_tmpA60=_tmpA5F->f1;_tmpA61=_tmpA5F->f2;}}
_LL441: Cyc_Toc_decltypes_to_c(_tmpA60);Cyc_Toc_stmttypes_to_c(_tmpA61);goto
_LL431;_LL442: {struct Cyc_Absyn_Do_s_struct*_tmpA62=(struct Cyc_Absyn_Do_s_struct*)
_tmpA45;if(_tmpA62->tag != 14)goto _LL444;else{_tmpA63=_tmpA62->f1;_tmpA64=_tmpA62->f2;
_tmpA65=_tmpA64.f1;}}_LL443: Cyc_Toc_stmttypes_to_c(_tmpA63);Cyc_Toc_exptypes_to_c(
_tmpA65);goto _LL431;_LL444: {struct Cyc_Absyn_Skip_s_struct*_tmpA66=(struct Cyc_Absyn_Skip_s_struct*)
_tmpA45;if(_tmpA66->tag != 0)goto _LL446;}_LL445: goto _LL447;_LL446: {struct Cyc_Absyn_Break_s_struct*
_tmpA67=(struct Cyc_Absyn_Break_s_struct*)_tmpA45;if(_tmpA67->tag != 6)goto _LL448;}
_LL447: goto _LL449;_LL448: {struct Cyc_Absyn_Continue_s_struct*_tmpA68=(struct Cyc_Absyn_Continue_s_struct*)
_tmpA45;if(_tmpA68->tag != 7)goto _LL44A;}_LL449: goto _LL44B;_LL44A: {struct Cyc_Absyn_Goto_s_struct*
_tmpA69=(struct Cyc_Absyn_Goto_s_struct*)_tmpA45;if(_tmpA69->tag != 8)goto _LL44C;}
_LL44B: goto _LL431;_LL44C: {struct Cyc_Absyn_Fallthru_s_struct*_tmpA6A=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmpA45;if(_tmpA6A->tag != 11)goto _LL44E;}_LL44D: goto _LL44F;_LL44E: {struct Cyc_Absyn_Label_s_struct*
_tmpA6B=(struct Cyc_Absyn_Label_s_struct*)_tmpA45;if(_tmpA6B->tag != 13)goto _LL450;}
_LL44F: goto _LL451;_LL450: {struct Cyc_Absyn_TryCatch_s_struct*_tmpA6C=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmpA45;if(_tmpA6C->tag != 15)goto _LL452;}_LL451: goto _LL453;_LL452: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmpA6D=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmpA45;if(_tmpA6D->tag != 16)goto
_LL431;}_LL453:{const char*_tmpE90;void*_tmpE8F;(_tmpE8F=0,Cyc_Tcutil_terr(s->loc,((
_tmpE90="Cyclone statement in C code",_tag_dyneither(_tmpE90,sizeof(char),28))),
_tag_dyneither(_tmpE8F,sizeof(void*),0)));}goto _LL431;_LL431:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude){for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpE93;void*_tmpE92;(_tmpE92=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE93="decls_to_c: not at toplevel!",
_tag_dyneither(_tmpE93,sizeof(char),29))),_tag_dyneither(_tmpE92,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmpA72=d->r;struct Cyc_Absyn_Vardecl*
_tmpA74;struct Cyc_Absyn_Fndecl*_tmpA76;struct Cyc_Absyn_Aggrdecl*_tmpA7C;struct
Cyc_Absyn_Datatypedecl*_tmpA7E;struct Cyc_Absyn_Enumdecl*_tmpA80;struct Cyc_Absyn_Typedefdecl*
_tmpA82;struct Cyc_List_List*_tmpA86;struct Cyc_List_List*_tmpA88;struct Cyc_List_List*
_tmpA8A;struct Cyc_List_List*_tmpA8C;_LL455: {struct Cyc_Absyn_Var_d_struct*
_tmpA73=(struct Cyc_Absyn_Var_d_struct*)_tmpA72;if(_tmpA73->tag != 0)goto _LL457;
else{_tmpA74=_tmpA73->f1;}}_LL456: {struct _tuple0*_tmpA8D=_tmpA74->name;if(
_tmpA74->sc == Cyc_Absyn_ExternC){struct _tuple0*_tmpE94;_tmpA8D=((_tmpE94=
_cycalloc(sizeof(*_tmpE94)),((_tmpE94->f1=Cyc_Absyn_Rel_n(0),((_tmpE94->f2=(*
_tmpA8D).f2,_tmpE94))))));}if(_tmpA74->initializer != 0){if(cinclude)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmpA74->initializer));else{Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_check_null(_tmpA74->initializer));}}{struct Cyc_Absyn_Global_b_struct
_tmpE97;struct Cyc_Absyn_Global_b_struct*_tmpE96;nv=Cyc_Toc_add_varmap(r,nv,
_tmpA74->name,Cyc_Absyn_varb_exp(_tmpA8D,(void*)((_tmpE96=_cycalloc(sizeof(*
_tmpE96)),((_tmpE96[0]=((_tmpE97.tag=1,((_tmpE97.f1=_tmpA74,_tmpE97)))),_tmpE96)))),
0));}_tmpA74->name=_tmpA8D;_tmpA74->sc=Cyc_Toc_scope_to_c(_tmpA74->sc);_tmpA74->type=
Cyc_Toc_typ_to_c_array(_tmpA74->type);{struct Cyc_List_List*_tmpE98;Cyc_Toc_result_decls=((
_tmpE98=_cycalloc(sizeof(*_tmpE98)),((_tmpE98->hd=d,((_tmpE98->tl=Cyc_Toc_result_decls,
_tmpE98))))));}goto _LL454;}_LL457: {struct Cyc_Absyn_Fn_d_struct*_tmpA75=(struct
Cyc_Absyn_Fn_d_struct*)_tmpA72;if(_tmpA75->tag != 1)goto _LL459;else{_tmpA76=
_tmpA75->f1;}}_LL458: {struct _tuple0*_tmpA92=_tmpA76->name;if(_tmpA76->sc == Cyc_Absyn_ExternC){{
struct _tuple0*_tmpE99;_tmpA92=((_tmpE99=_cycalloc(sizeof(*_tmpE99)),((_tmpE99->f1=
Cyc_Absyn_Rel_n(0),((_tmpE99->f2=(*_tmpA92).f2,_tmpE99))))));}_tmpA76->sc=Cyc_Absyn_Public;}
nv=Cyc_Toc_add_varmap(r,nv,_tmpA76->name,Cyc_Absyn_var_exp(_tmpA92,0));_tmpA76->name=
_tmpA92;Cyc_Toc_fndecl_to_c(nv,_tmpA76,cinclude);{struct Cyc_List_List*_tmpE9A;
Cyc_Toc_result_decls=((_tmpE9A=_cycalloc(sizeof(*_tmpE9A)),((_tmpE9A->hd=d,((
_tmpE9A->tl=Cyc_Toc_result_decls,_tmpE9A))))));}goto _LL454;}_LL459: {struct Cyc_Absyn_Let_d_struct*
_tmpA77=(struct Cyc_Absyn_Let_d_struct*)_tmpA72;if(_tmpA77->tag != 2)goto _LL45B;}
_LL45A: goto _LL45C;_LL45B: {struct Cyc_Absyn_Letv_d_struct*_tmpA78=(struct Cyc_Absyn_Letv_d_struct*)
_tmpA72;if(_tmpA78->tag != 3)goto _LL45D;}_LL45C: {const char*_tmpE9D;void*_tmpE9C;(
_tmpE9C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE9D="letdecl at toplevel",_tag_dyneither(_tmpE9D,sizeof(char),20))),
_tag_dyneither(_tmpE9C,sizeof(void*),0)));}_LL45D: {struct Cyc_Absyn_Region_d_struct*
_tmpA79=(struct Cyc_Absyn_Region_d_struct*)_tmpA72;if(_tmpA79->tag != 4)goto _LL45F;}
_LL45E: {const char*_tmpEA0;void*_tmpE9F;(_tmpE9F=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpEA0="region decl at toplevel",
_tag_dyneither(_tmpEA0,sizeof(char),24))),_tag_dyneither(_tmpE9F,sizeof(void*),0)));}
_LL45F: {struct Cyc_Absyn_Alias_d_struct*_tmpA7A=(struct Cyc_Absyn_Alias_d_struct*)
_tmpA72;if(_tmpA7A->tag != 5)goto _LL461;}_LL460: {const char*_tmpEA3;void*_tmpEA2;(
_tmpEA2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpEA3="alias decl at toplevel",_tag_dyneither(_tmpEA3,sizeof(char),23))),
_tag_dyneither(_tmpEA2,sizeof(void*),0)));}_LL461: {struct Cyc_Absyn_Aggr_d_struct*
_tmpA7B=(struct Cyc_Absyn_Aggr_d_struct*)_tmpA72;if(_tmpA7B->tag != 6)goto _LL463;
else{_tmpA7C=_tmpA7B->f1;}}_LL462: Cyc_Toc_aggrdecl_to_c(_tmpA7C,1);goto _LL454;
_LL463: {struct Cyc_Absyn_Datatype_d_struct*_tmpA7D=(struct Cyc_Absyn_Datatype_d_struct*)
_tmpA72;if(_tmpA7D->tag != 7)goto _LL465;else{_tmpA7E=_tmpA7D->f1;}}_LL464: if(
_tmpA7E->is_extensible)Cyc_Toc_xdatatypedecl_to_c(_tmpA7E);else{Cyc_Toc_datatypedecl_to_c(
_tmpA7E);}goto _LL454;_LL465: {struct Cyc_Absyn_Enum_d_struct*_tmpA7F=(struct Cyc_Absyn_Enum_d_struct*)
_tmpA72;if(_tmpA7F->tag != 8)goto _LL467;else{_tmpA80=_tmpA7F->f1;}}_LL466: Cyc_Toc_enumdecl_to_c(
_tmpA80);{struct Cyc_List_List*_tmpEA4;Cyc_Toc_result_decls=((_tmpEA4=_cycalloc(
sizeof(*_tmpEA4)),((_tmpEA4->hd=d,((_tmpEA4->tl=Cyc_Toc_result_decls,_tmpEA4))))));}
goto _LL454;_LL467: {struct Cyc_Absyn_Typedef_d_struct*_tmpA81=(struct Cyc_Absyn_Typedef_d_struct*)
_tmpA72;if(_tmpA81->tag != 9)goto _LL469;else{_tmpA82=_tmpA81->f1;}}_LL468: _tmpA82->name=
_tmpA82->name;_tmpA82->tvs=0;if(_tmpA82->defn != 0){struct Cyc_Core_Opt*_tmpEA5;
_tmpA82->defn=((_tmpEA5=_cycalloc(sizeof(*_tmpEA5)),((_tmpEA5->v=Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA82->defn))->v),_tmpEA5))));}else{
switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmpA82->kind))->v)->kind){
case Cyc_Absyn_BoxKind: _LL475:{struct Cyc_Core_Opt*_tmpEA6;_tmpA82->defn=((_tmpEA6=
_cycalloc(sizeof(*_tmpEA6)),((_tmpEA6->v=Cyc_Absyn_void_star_typ(),_tmpEA6))));}
break;default: _LL476:{struct Cyc_Core_Opt*_tmpEA7;_tmpA82->defn=((_tmpEA7=
_cycalloc(sizeof(*_tmpEA7)),((_tmpEA7->v=(void*)& Cyc_Absyn_VoidType_val,_tmpEA7))));}
break;}}{struct Cyc_List_List*_tmpEA8;Cyc_Toc_result_decls=((_tmpEA8=_cycalloc(
sizeof(*_tmpEA8)),((_tmpEA8->hd=d,((_tmpEA8->tl=Cyc_Toc_result_decls,_tmpEA8))))));}
goto _LL454;_LL469: {struct Cyc_Absyn_Porton_d_struct*_tmpA83=(struct Cyc_Absyn_Porton_d_struct*)
_tmpA72;if(_tmpA83->tag != 14)goto _LL46B;}_LL46A: goto _LL46C;_LL46B: {struct Cyc_Absyn_Portoff_d_struct*
_tmpA84=(struct Cyc_Absyn_Portoff_d_struct*)_tmpA72;if(_tmpA84->tag != 15)goto
_LL46D;}_LL46C: goto _LL454;_LL46D: {struct Cyc_Absyn_Namespace_d_struct*_tmpA85=(
struct Cyc_Absyn_Namespace_d_struct*)_tmpA72;if(_tmpA85->tag != 10)goto _LL46F;
else{_tmpA86=_tmpA85->f2;}}_LL46E: _tmpA88=_tmpA86;goto _LL470;_LL46F: {struct Cyc_Absyn_Using_d_struct*
_tmpA87=(struct Cyc_Absyn_Using_d_struct*)_tmpA72;if(_tmpA87->tag != 11)goto _LL471;
else{_tmpA88=_tmpA87->f2;}}_LL470: _tmpA8A=_tmpA88;goto _LL472;_LL471: {struct Cyc_Absyn_ExternC_d_struct*
_tmpA89=(struct Cyc_Absyn_ExternC_d_struct*)_tmpA72;if(_tmpA89->tag != 12)goto
_LL473;else{_tmpA8A=_tmpA89->f1;}}_LL472: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA8A,top,
cinclude);goto _LL454;_LL473: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA8B=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmpA72;if(_tmpA8B->tag != 13)goto _LL454;
else{_tmpA8C=_tmpA8B->f1;}}_LL474: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA8C,top,1);goto
_LL454;_LL454:;};}return nv;}static void Cyc_Toc_init(){struct _DynRegionHandle*
_tmpAA1;struct Cyc_Core_NewRegion _tmpAA0=Cyc_Core__new_dynregion((const char*)"internal-error",
0);_tmpAA1=_tmpAA0.dynregion;{struct _DynRegionFrame _tmpAA2;struct _RegionHandle*d=
_open_dynregion(& _tmpAA2,_tmpAA1);{struct Cyc_Dict_Dict*_tmpEB5;struct Cyc_Dict_Dict*
_tmpEB4;struct Cyc_Set_Set**_tmpEB3;struct Cyc_List_List**_tmpEB2;struct Cyc_Dict_Dict*
_tmpEB1;struct Cyc_List_List**_tmpEB0;struct Cyc_Toc_TocState*_tmpEAF;Cyc_Toc_toc_state=((
_tmpEAF=_cycalloc(sizeof(*_tmpEAF)),((_tmpEAF->dyn=(struct _DynRegionHandle*)
_tmpAA1,((_tmpEAF->tuple_types=(struct Cyc_List_List**)((_tmpEB0=_region_malloc(d,
sizeof(*_tmpEB0)),((_tmpEB0[0]=0,_tmpEB0)))),((_tmpEAF->aggrs_so_far=(struct Cyc_Dict_Dict*)((
_tmpEB1=_region_malloc(d,sizeof(*_tmpEB1)),((_tmpEB1[0]=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
d,Cyc_Absyn_qvar_cmp),_tmpEB1)))),((_tmpEAF->abs_struct_types=(struct Cyc_List_List**)((
_tmpEB2=_region_malloc(d,sizeof(*_tmpEB2)),((_tmpEB2[0]=0,_tmpEB2)))),((_tmpEAF->datatypes_so_far=(
struct Cyc_Set_Set**)((_tmpEB3=_region_malloc(d,sizeof(*_tmpEB3)),((_tmpEB3[0]=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpEB3)))),((_tmpEAF->xdatatypes_so_far=(
struct Cyc_Dict_Dict*)((_tmpEB4=_region_malloc(d,sizeof(*_tmpEB4)),((_tmpEB4[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpEB4)))),((_tmpEAF->qvar_tags=(
struct Cyc_Dict_Dict*)((_tmpEB5=_region_malloc(d,sizeof(*_tmpEB5)),((_tmpEB5[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct
_tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpEB5)))),((_tmpEAF->temp_labels=(
struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(
d),(struct Cyc_Toc_TocState*)_tmpEAF))))))))))))))))));}Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=
0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=0;{struct
_dyneither_ptr**_tmpEB6;Cyc_Toc_globals=_tag_dyneither(((_tmpEB6=_cycalloc(
sizeof(struct _dyneither_ptr*)* 41),((_tmpEB6[0]=& Cyc_Toc__throw_str,((_tmpEB6[1]=&
Cyc_Toc_setjmp_str,((_tmpEB6[2]=& Cyc_Toc__push_handler_str,((_tmpEB6[3]=& Cyc_Toc__pop_handler_str,((
_tmpEB6[4]=& Cyc_Toc__exn_thrown_str,((_tmpEB6[5]=& Cyc_Toc__npop_handler_str,((
_tmpEB6[6]=& Cyc_Toc__check_null_str,((_tmpEB6[7]=& Cyc_Toc__check_known_subscript_null_str,((
_tmpEB6[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpEB6[9]=& Cyc_Toc__check_dyneither_subscript_str,((
_tmpEB6[10]=& Cyc_Toc__dyneither_ptr_str,((_tmpEB6[11]=& Cyc_Toc__tag_dyneither_str,((
_tmpEB6[12]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpEB6[13]=& Cyc_Toc__untag_dyneither_ptr_str,((
_tmpEB6[14]=& Cyc_Toc__get_dyneither_size_str,((_tmpEB6[15]=& Cyc_Toc__get_zero_arr_size_str,((
_tmpEB6[16]=& Cyc_Toc__dyneither_ptr_plus_str,((_tmpEB6[17]=& Cyc_Toc__zero_arr_plus_str,((
_tmpEB6[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpEB6[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((
_tmpEB6[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpEB6[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((
_tmpEB6[22]=& Cyc_Toc__cycalloc_str,((_tmpEB6[23]=& Cyc_Toc__cyccalloc_str,((
_tmpEB6[24]=& Cyc_Toc__cycalloc_atomic_str,((_tmpEB6[25]=& Cyc_Toc__cyccalloc_atomic_str,((
_tmpEB6[26]=& Cyc_Toc__region_malloc_str,((_tmpEB6[27]=& Cyc_Toc__region_calloc_str,((
_tmpEB6[28]=& Cyc_Toc__check_times_str,((_tmpEB6[29]=& Cyc_Toc__new_region_str,((
_tmpEB6[30]=& Cyc_Toc__push_region_str,((_tmpEB6[31]=& Cyc_Toc__pop_region_str,((
_tmpEB6[32]=& Cyc_Toc__open_dynregion_str,((_tmpEB6[33]=& Cyc_Toc__push_dynregion_str,((
_tmpEB6[34]=& Cyc_Toc__pop_dynregion_str,((_tmpEB6[35]=& Cyc_Toc__reset_region_str,((
_tmpEB6[36]=& Cyc_Toc__throw_arraybounds_str,((_tmpEB6[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,((
_tmpEB6[38]=& Cyc_Toc__throw_match_str,((_tmpEB6[39]=& Cyc_Toc__swap_word_str,((
_tmpEB6[40]=& Cyc_Toc__swap_dyneither_str,_tmpEB6)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),41);};;_pop_dynregion(d);};}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();{struct _RegionHandle _tmpAAB=
_new_region("start");struct _RegionHandle*start=& _tmpAAB;_push_region(start);Cyc_Toc_decls_to_c(
start,Cyc_Toc_empty_env(start),ds,1,0);{struct _DynRegionHandle*_tmpAAD;struct Cyc_Toc_TocState
_tmpAAC=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpAAD=
_tmpAAC.dyn;Cyc_Core_free_dynregion(_tmpAAD);};{struct Cyc_List_List*_tmpAAE=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);
_npop_handler(0);return _tmpAAE;};;_pop_region(start);};}
