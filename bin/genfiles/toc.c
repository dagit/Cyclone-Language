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
struct Cyc_Position_Exit_struct{char*tag;};typedef unsigned int Cyc_Position_seg_t;
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Lex_struct{
int tag;};struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{
int tag;};typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct
_dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[
10];struct Cyc_Position_Nocontext_struct{char*tag;};struct Cyc_Xarray_Xarray{
struct _RegionHandle*r;struct _dyneither_ptr elmts;int num_elmts;};typedef struct Cyc_Xarray_Xarray*
Cyc_Xarray_xarray_t;int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);void*Cyc_Xarray_get(
struct Cyc_Xarray_Xarray*,int);struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(
struct _RegionHandle*);int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;typedef struct _dyneither_ptr*
Cyc_Absyn_var_t;typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;typedef struct
_dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
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
typedef void*Cyc_Absyn_rgntype_t;typedef void*Cyc_Absyn_type_opt_t;typedef union Cyc_Absyn_Cnst
Cyc_Absyn_cnst_t;typedef enum Cyc_Absyn_Primop Cyc_Absyn_primop_t;typedef enum Cyc_Absyn_Incrementor
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
struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;
void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*
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
Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};
struct Cyc_Absyn_Aggr_td_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;};typedef void*Cyc_Absyn_raw_type_decl_t;struct
Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;
struct Cyc_Absyn_VoidType_struct{int tag;};struct Cyc_Absyn_Evar_struct{int tag;
struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;
struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_struct{int tag;
struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{int tag;
struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;enum Cyc_Absyn_Sign
f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_struct{int tag;int f1;};
struct Cyc_Absyn_ArrayType_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_struct{
int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple0*f1;
struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct
Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void*f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_struct{
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_RefCntRgn_struct{
int tag;};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_TypeDeclType_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;
};extern struct Cyc_Absyn_HeapRgn_struct Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_VoidType_struct
Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*
f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};typedef
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
Cyc_Absyn_Carray_mod_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};
struct Cyc_Absyn_ConstArray_mod_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*
f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts
f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;
};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;
unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;unsigned int
f1;struct Cyc_List_List*f2;};typedef void*Cyc_Absyn_type_modifier_t;struct
_union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};
struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int
tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};
struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6
val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int
tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;
};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct
_union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct
_union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct
_union_Cnst_Wstring_c Wstring_c;};union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,
char);union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);union Cyc_Absyn_Cnst
Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(
enum Cyc_Absyn_Sign,long long);union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct
_dyneither_ptr,int);enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,
Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq
 = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not
 = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor
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
f1;void*f2;};struct Cyc_Absyn_Primop_e_struct{int tag;enum Cyc_Absyn_Primop f1;
struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_And_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_Or_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_FnCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{int tag;
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
int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
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
int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;
unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_struct{int tag;};struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};
struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;
};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};extern
struct Cyc_Absyn_Skip_s_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;
unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};
struct Cyc_Absyn_Wild_p_struct{int tag;};struct Cyc_Absyn_Var_p_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{
int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;
struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo*
f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct
Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_struct{int tag;};struct Cyc_Absyn_Int_p_struct{
int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_struct{int tag;char f1;
};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{
int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_Wild_p_struct Cyc_Absyn_Wild_p_val;
struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_struct{
int tag;};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};extern struct Cyc_Absyn_Unresolved_b_struct Cyc_Absyn_Unresolved_b_val;struct
Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual
ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;
struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;
struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind
kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct
_tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};
struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{
struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{
enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Region_d_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*
f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};
struct Cyc_Absyn_Namespace_d_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
unsigned int loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct _dyneither_ptr*f1;};extern
char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{char*tag;};extern
struct Cyc_Absyn_EmptyAnnot_struct Cyc_Absyn_EmptyAnnot_val;int Cyc_Absyn_qvar_cmp(
struct _tuple0*,struct _tuple0*);void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*
x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);extern union Cyc_Absyn_Constraint*
Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
void*Cyc_Absyn_exn_typ();extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_star_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple0*name);void*Cyc_Absyn_unionq_typ(struct
_tuple0*name);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,
struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
union Cyc_Absyn_Cnst,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_char_exp(char c,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dyneither_ptr f,int i,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dyneither_ptr s,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct
_tuple0*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,
void*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum 
Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct
Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct
Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,
struct Cyc_List_List*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,
struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);struct
Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp*e,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(
void*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,
struct Cyc_List_List*,unsigned int);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*
s,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,
unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*
e,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(
struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
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
fst_rgn,unsigned int);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
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
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(
unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(
unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);int Cyc_Tcutil_kind_leq(
struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(
void*t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(
void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;int Cyc_Tcutil_unify(void*,void*);void*
Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct
Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,
unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*
fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dyneither,void**elt_type);struct _tuple10{struct Cyc_Absyn_Tqual f1;
void*f2;};void*Cyc_Tcutil_snd_tqt(struct _tuple10*);void Cyc_Tcutil_check_no_qual(
unsigned int loc,void*t);struct _tuple11{unsigned int f1;int f2;};struct _tuple11 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct Cyc_CfFlowInfo_VarRoot_struct{int
tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 
1,Cyc_CfFlowInfo_AllIL  = 2};typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;
struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple12{struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple12 val;};struct
_union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessConst{int tag;unsigned int val;};struct
_union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
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
ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const char*_tmpA57;void*_tmpA56;(_tmpA56=0,
Cyc_fprintf(Cyc_stderr,((_tmpA57="\n",_tag_dyneither(_tmpA57,sizeof(char),2))),
_tag_dyneither(_tmpA56,sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_struct _tmpA5A;struct Cyc_Toc_Toc_Unimplemented_struct*
_tmpA59;(int)_throw((void*)((_tmpA59=_cycalloc_atomic(sizeof(*_tmpA59)),((
_tmpA59[0]=((_tmpA5A.tag=Cyc_Toc_Toc_Unimplemented,_tmpA5A)),_tmpA59)))));};}
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(
Cyc_stderr,fmt,ap);{const char*_tmpA5D;void*_tmpA5C;(_tmpA5C=0,Cyc_fprintf(Cyc_stderr,((
_tmpA5D="\n",_tag_dyneither(_tmpA5D,sizeof(char),2))),_tag_dyneither(_tmpA5C,
sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Impossible_struct
_tmpA60;struct Cyc_Toc_Toc_Impossible_struct*_tmpA5F;(int)_throw((void*)((_tmpA5F=
_cycalloc_atomic(sizeof(*_tmpA5F)),((_tmpA5F[0]=((_tmpA60.tag=Cyc_Toc_Toc_Impossible,
_tmpA60)),_tmpA5F)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_struct{
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
_tmpA61;skip_stmt_opt=((_tmpA61=_cycalloc(sizeof(*_tmpA61)),((_tmpA61[0]=Cyc_Absyn_skip_stmt(
0),_tmpA61))));}return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(
void*t,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,
0);}static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Cast_e_struct
_tmpA64;struct Cyc_Absyn_Cast_e_struct*_tmpA63;return(void*)((_tmpA63=_cycalloc(
sizeof(*_tmpA63)),((_tmpA63[0]=((_tmpA64.tag=13,((_tmpA64.f1=(void*)t,((_tmpA64.f2=
e,((_tmpA64.f3=0,((_tmpA64.f4=Cyc_Absyn_No_coercion,_tmpA64)))))))))),_tmpA63))));}
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Deref_e_struct
_tmpA67;struct Cyc_Absyn_Deref_e_struct*_tmpA66;return(void*)((_tmpA66=_cycalloc(
sizeof(*_tmpA66)),((_tmpA66[0]=((_tmpA67.tag=19,((_tmpA67.f1=e,_tmpA67)))),
_tmpA66))));}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){struct Cyc_Absyn_Subscript_e_struct _tmpA6A;struct Cyc_Absyn_Subscript_e_struct*
_tmpA69;return(void*)((_tmpA69=_cycalloc(sizeof(*_tmpA69)),((_tmpA69[0]=((
_tmpA6A.tag=22,((_tmpA6A.f1=e1,((_tmpA6A.f2=e2,_tmpA6A)))))),_tmpA69))));}static
void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct
_tmpA6D;struct Cyc_Absyn_StmtExp_e_struct*_tmpA6C;return(void*)((_tmpA6C=
_cycalloc(sizeof(*_tmpA6C)),((_tmpA6C[0]=((_tmpA6D.tag=35,((_tmpA6D.f1=s,_tmpA6D)))),
_tmpA6C))));}static void*Cyc_Toc_sizeoftyp_exp_r(void*t){struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpA70;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpA6F;return(void*)((_tmpA6F=
_cycalloc(sizeof(*_tmpA6F)),((_tmpA6F[0]=((_tmpA70.tag=16,((_tmpA70.f1=(void*)t,
_tmpA70)))),_tmpA6F))));}static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct _tmpA73;struct Cyc_Absyn_FnCall_e_struct*
_tmpA72;return(void*)((_tmpA72=_cycalloc(sizeof(*_tmpA72)),((_tmpA72[0]=((
_tmpA73.tag=9,((_tmpA73.f1=e,((_tmpA73.f2=es,((_tmpA73.f3=0,((_tmpA73.f4=1,
_tmpA73)))))))))),_tmpA72))));}static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*
e){struct Cyc_Absyn_Exp_s_struct _tmpA76;struct Cyc_Absyn_Exp_s_struct*_tmpA75;
return(void*)((_tmpA75=_cycalloc(sizeof(*_tmpA75)),((_tmpA75[0]=((_tmpA76.tag=1,((
_tmpA76.f1=e,_tmpA76)))),_tmpA75))));}static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*
s1,struct Cyc_Absyn_Stmt*s2){struct Cyc_Absyn_Seq_s_struct _tmpA79;struct Cyc_Absyn_Seq_s_struct*
_tmpA78;return(void*)((_tmpA78=_cycalloc(sizeof(*_tmpA78)),((_tmpA78[0]=((
_tmpA79.tag=2,((_tmpA79.f1=s1,((_tmpA79.f2=s2,_tmpA79)))))),_tmpA78))));}static
void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,
struct Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct _tmpA7C;struct Cyc_Absyn_Conditional_e_struct*
_tmpA7B;return(void*)((_tmpA7B=_cycalloc(sizeof(*_tmpA7B)),((_tmpA7B[0]=((
_tmpA7C.tag=5,((_tmpA7C.f1=e1,((_tmpA7C.f2=e2,((_tmpA7C.f3=e3,_tmpA7C)))))))),
_tmpA7B))));}static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*n){struct Cyc_Absyn_AggrMember_e_struct _tmpA7F;struct Cyc_Absyn_AggrMember_e_struct*
_tmpA7E;return(void*)((_tmpA7E=_cycalloc(sizeof(*_tmpA7E)),((_tmpA7E[0]=((
_tmpA7F.tag=20,((_tmpA7F.f1=e,((_tmpA7F.f2=n,((_tmpA7F.f3=0,((_tmpA7F.f4=0,
_tmpA7F)))))))))),_tmpA7E))));}static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrArrow_e_struct _tmpA82;struct Cyc_Absyn_AggrArrow_e_struct*
_tmpA81;return(void*)((_tmpA81=_cycalloc(sizeof(*_tmpA81)),((_tmpA81[0]=((
_tmpA82.tag=21,((_tmpA82.f1=e,((_tmpA82.f2=n,((_tmpA82.f3=0,((_tmpA82.f4=0,
_tmpA82)))))))))),_tmpA81))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmpA85;
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpA84;return(void*)((_tmpA84=_cycalloc(
sizeof(*_tmpA84)),((_tmpA84[0]=((_tmpA85.tag=34,((_tmpA85.f1=tdopt,((_tmpA85.f2=
ds,_tmpA85)))))),_tmpA84))));}static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_struct _tmpA88;
struct Cyc_Absyn_Goto_s_struct*_tmpA87;return(void*)((_tmpA87=_cycalloc(sizeof(*
_tmpA87)),((_tmpA87[0]=((_tmpA88.tag=8,((_tmpA88.f1=v,((_tmpA88.f2=s,_tmpA88)))))),
_tmpA87))));}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{
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
Cyc_Core_Impossible_struct _tmpA8E;const char*_tmpA8D;struct Cyc_Core_Impossible_struct*
_tmpA8C;(int)_throw((void*)((_tmpA8C=_cycalloc(sizeof(*_tmpA8C)),((_tmpA8C[0]=((
_tmpA8E.tag=Cyc_Core_Impossible,((_tmpA8E.f1=((_tmpA8D="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpA8D,sizeof(char),44))),_tmpA8E)))),_tmpA8C)))));}}goto _LL0;
_LL3: {struct Cyc_Absyn_FloatType_struct*_tmpC9=(struct Cyc_Absyn_FloatType_struct*)
_tmpC6;if(_tmpC9->tag != 7)goto _LL5;else{_tmpCA=_tmpC9->f1;if(_tmpCA != 0)goto _LL5;}}
_LL4: function=fS->ffloat;goto _LL0;_LL5: {struct Cyc_Absyn_FloatType_struct*_tmpCB=(
struct Cyc_Absyn_FloatType_struct*)_tmpC6;if(_tmpCB->tag != 7)goto _LL7;else{_tmpCC=
_tmpCB->f1;if(_tmpCC != 1)goto _LL7;}}_LL6: function=fS->fdouble;goto _LL0;_LL7: {
struct Cyc_Absyn_FloatType_struct*_tmpCD=(struct Cyc_Absyn_FloatType_struct*)
_tmpC6;if(_tmpCD->tag != 7)goto _LL9;}_LL8: function=fS->flongdouble;goto _LL0;_LL9: {
struct Cyc_Absyn_PointerType_struct*_tmpCE=(struct Cyc_Absyn_PointerType_struct*)
_tmpC6;if(_tmpCE->tag != 5)goto _LLB;}_LLA: function=fS->fvoidstar;goto _LL0;_LLB:;
_LLC: {struct Cyc_Core_Impossible_struct _tmpA9B;const char*_tmpA9A;void*_tmpA99[1];
struct Cyc_String_pa_struct _tmpA98;struct Cyc_Core_Impossible_struct*_tmpA97;(int)
_throw((void*)((_tmpA97=_cycalloc(sizeof(*_tmpA97)),((_tmpA97[0]=((_tmpA9B.tag=
Cyc_Core_Impossible,((_tmpA9B.f1=(struct _dyneither_ptr)((_tmpA98.tag=0,((_tmpA98.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA99[
0]=& _tmpA98,Cyc_aprintf(((_tmpA9A="impossible expression type %s (not int, float, double, or pointer)",
_tag_dyneither(_tmpA9A,sizeof(char),67))),_tag_dyneither(_tmpA99,sizeof(void*),1)))))))),
_tmpA9B)))),_tmpA97)))));}_LL0:;}return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(
fS,(void*)_check_null(arr->topt));}struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(
struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){void*_tmpD7=Cyc_Tcutil_compress((
void*)_check_null(arr->topt));struct Cyc_Absyn_PtrInfo _tmpD9;void*_tmpDA;_LL13: {
struct Cyc_Absyn_PointerType_struct*_tmpD8=(struct Cyc_Absyn_PointerType_struct*)
_tmpD7;if(_tmpD8->tag != 5)goto _LL15;else{_tmpD9=_tmpD8->f1;_tmpDA=_tmpD9.elt_typ;}}
_LL14: return Cyc_Toc_getFunctionType(fS,_tmpDA);_LL15:;_LL16: {struct Cyc_Core_Impossible_struct
_tmpAA1;const char*_tmpAA0;struct Cyc_Core_Impossible_struct*_tmpA9F;(int)_throw((
void*)((_tmpA9F=_cycalloc(sizeof(*_tmpA9F)),((_tmpA9F[0]=((_tmpAA1.tag=Cyc_Core_Impossible,((
_tmpAA1.f1=((_tmpAA0="impossible type (not pointer)",_tag_dyneither(_tmpAA0,
sizeof(char),30))),_tmpAA1)))),_tmpA9F)))));}_LL12:;}struct _tuple16{struct Cyc_List_List*
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
_tmpF5=(struct Cyc_Absyn_Cast_e_struct*)_tmpDE;if(_tmpF5->tag != 13)goto _LL26;
else{_tmpF6=_tmpF5->f2;}}_LL25: return Cyc_Toc_is_zero(_tmpF6);_LL26: {struct Cyc_Absyn_Tuple_e_struct*
_tmpF7=(struct Cyc_Absyn_Tuple_e_struct*)_tmpDE;if(_tmpF7->tag != 23)goto _LL28;
else{_tmpF8=_tmpF7->f1;}}_LL27: return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpF8);_LL28: {struct
Cyc_Absyn_Array_e_struct*_tmpF9=(struct Cyc_Absyn_Array_e_struct*)_tmpDE;if(
_tmpF9->tag != 25)goto _LL2A;else{_tmpFA=_tmpF9->f1;}}_LL29: _tmpFC=_tmpFA;goto
_LL2B;_LL2A: {struct Cyc_Absyn_Aggregate_e_struct*_tmpFB=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpDE;if(_tmpFB->tag != 27)goto _LL2C;else{_tmpFC=_tmpFB->f3;}}_LL2B: _tmpFE=
_tmpFC;goto _LL2D;_LL2C: {struct Cyc_Absyn_CompoundLit_e_struct*_tmpFD=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpDE;if(_tmpFD->tag != 24)goto _LL2E;else{_tmpFE=_tmpFD->f2;}}_LL2D: _tmp100=
_tmpFE;goto _LL2F;_LL2E: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpFF=(struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmpDE;if(_tmpFF->tag != 34)goto _LL30;else{
_tmp100=_tmpFF->f2;}}_LL2F: for(0;_tmp100 != 0;_tmp100=_tmp100->tl){if(!Cyc_Toc_is_zero((*((
struct _tuple16*)_tmp100->hd)).f2))return 0;}return 1;_LL30:;_LL31: return 0;_LL17:;}
static int Cyc_Toc_is_nullable(void*t){void*_tmp102=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_PtrInfo _tmp104;struct Cyc_Absyn_PtrAtts _tmp105;union Cyc_Absyn_Constraint*
_tmp106;_LL33: {struct Cyc_Absyn_PointerType_struct*_tmp103=(struct Cyc_Absyn_PointerType_struct*)
_tmp102;if(_tmp103->tag != 5)goto _LL35;else{_tmp104=_tmp103->f1;_tmp105=_tmp104.ptr_atts;
_tmp106=_tmp105.nullable;}}_LL34: return((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp106);_LL35:;_LL36: {const char*_tmpAA4;void*_tmpAA3;(
_tmpAA3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAA4="is_nullable",_tag_dyneither(_tmpAA4,sizeof(char),12))),_tag_dyneither(
_tmpAA3,sizeof(void*),0)));}_LL32:;}static char _tmp115[1]="";static char _tmp116[8]="*bogus*";
static struct _tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct
_dyneither_ptr tag){struct _DynRegionHandle*_tmp10A;struct Cyc_Dict_Dict*_tmp10B;
struct Cyc_Toc_TocState _tmp109=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp10A=_tmp109.dyn;_tmp10B=_tmp109.qvar_tags;{static struct _dyneither_ptr
bogus_string={_tmp116,_tmp116,_tmp116 + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
4,0}},& bogus_string};static struct _tuple14 pair={& bogus_qvar,{_tmp115,_tmp115,
_tmp115 + 1}};{struct _tuple14 _tmpAA5;pair=((_tmpAA5.f1=x,((_tmpAA5.f2=tag,_tmpAA5))));}{
struct _DynRegionFrame _tmp10D;struct _RegionHandle*d=_open_dynregion(& _tmp10D,
_tmp10A);{struct _tuple0**_tmp10E=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,
struct _tuple14*k))Cyc_Dict_lookup_opt)(*_tmp10B,(struct _tuple14*)& pair);if(
_tmp10E != 0){struct _tuple0*_tmp10F=*_tmp10E;_npop_handler(0);return _tmp10F;}{
struct _tuple14*_tmpAA6;struct _tuple14*_tmp110=(_tmpAA6=_cycalloc(sizeof(*_tmpAA6)),((
_tmpAA6->f1=x,((_tmpAA6->f2=tag,_tmpAA6)))));struct _dyneither_ptr*_tmpAA9;struct
_tuple0*_tmpAA8;struct _tuple0*res=(_tmpAA8=_cycalloc(sizeof(*_tmpAA8)),((_tmpAA8->f1=(*
x).f1,((_tmpAA8->f2=((_tmpAA9=_cycalloc(sizeof(*_tmpAA9)),((_tmpAA9[0]=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*x).f2,(struct
_dyneither_ptr)tag),_tmpAA9)))),_tmpAA8)))));*_tmp10B=((struct Cyc_Dict_Dict(*)(
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
_npop_handler(0);return _tmp121;}};}}{struct Cyc_Int_pa_struct _tmpAB1;void*_tmpAB0[
1];const char*_tmpAAF;struct _dyneither_ptr*_tmpAAE;struct _dyneither_ptr*xname=(
_tmpAAE=_cycalloc(sizeof(*_tmpAAE)),((_tmpAAE[0]=(struct _dyneither_ptr)((_tmpAB1.tag=
1,((_tmpAB1.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAB0[0]=& _tmpAB1,
Cyc_aprintf(((_tmpAAF="_tuple%d",_tag_dyneither(_tmpAAF,sizeof(char),9))),
_tag_dyneither(_tmpAB0,sizeof(void*),1)))))))),_tmpAAE)));void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp122=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmpAB4;struct Cyc_List_List*
_tmpAB3;_tmp122=((_tmpAB3=_cycalloc(sizeof(*_tmpAB3)),((_tmpAB3->hd=((_tmpAB4=
_cycalloc(sizeof(*_tmpAB4)),((_tmpAB4->name=Cyc_Absyn_fieldname(i),((_tmpAB4->tq=
Cyc_Toc_mt_tq,((_tmpAB4->type=(void*)ts2->hd,((_tmpAB4->width=0,((_tmpAB4->attributes=
0,_tmpAB4)))))))))))),((_tmpAB3->tl=_tmp122,_tmpAB3))))));}}_tmp122=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp122);{struct Cyc_Absyn_AggrdeclImpl*
_tmpAB9;struct _tuple0*_tmpAB8;struct Cyc_Absyn_Aggrdecl*_tmpAB7;struct Cyc_Absyn_Aggrdecl*
_tmp125=(_tmpAB7=_cycalloc(sizeof(*_tmpAB7)),((_tmpAB7->kind=Cyc_Absyn_StructA,((
_tmpAB7->sc=Cyc_Absyn_Public,((_tmpAB7->name=((_tmpAB8=_cycalloc(sizeof(*_tmpAB8)),((
_tmpAB8->f1=Cyc_Absyn_Rel_n(0),((_tmpAB8->f2=xname,_tmpAB8)))))),((_tmpAB7->tvs=
0,((_tmpAB7->impl=((_tmpAB9=_cycalloc(sizeof(*_tmpAB9)),((_tmpAB9->exist_vars=0,((
_tmpAB9->rgn_po=0,((_tmpAB9->fields=_tmp122,((_tmpAB9->tagged=0,_tmpAB9)))))))))),((
_tmpAB7->attributes=0,_tmpAB7)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*
_tmpABF;struct Cyc_Absyn_Aggr_d_struct _tmpABE;struct Cyc_List_List*_tmpABD;Cyc_Toc_result_decls=((
_tmpABD=_cycalloc(sizeof(*_tmpABD)),((_tmpABD->hd=Cyc_Absyn_new_decl((void*)((
_tmpABF=_cycalloc(sizeof(*_tmpABF)),((_tmpABF[0]=((_tmpABE.tag=6,((_tmpABE.f1=
_tmp125,_tmpABE)))),_tmpABF)))),0),((_tmpABD->tl=Cyc_Toc_result_decls,_tmpABD))))));}{
struct _tuple17*_tmpAC2;struct Cyc_List_List*_tmpAC1;*_tmp119=((_tmpAC1=
_region_malloc(d,sizeof(*_tmpAC1)),((_tmpAC1->hd=((_tmpAC2=_region_malloc(d,
sizeof(*_tmpAC2)),((_tmpAC2->f1=x,((_tmpAC2->f2=ts,_tmpAC2)))))),((_tmpAC1->tl=*
_tmp119,_tmpAC1))))));}{void*_tmp12B=x;_npop_handler(0);return _tmp12B;};};};;
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
for(0;_tmp13D != 0;(_tmp13D=_tmp13D->tl,_tmp13B=_tmp13B->tl)){void*_tmp13E=(void*)
_tmp13D->hd;void*_tmp13F=(void*)((struct Cyc_List_List*)_check_null(_tmp13B))->hd;
if(Cyc_Tcutil_unify(_tmp13E,_tmp13F))continue;okay=0;break;}}if(okay){void*
_tmp140=_tmp13C;_npop_handler(0);return _tmp140;}}}}{struct Cyc_Int_pa_struct
_tmpACA;void*_tmpAC9[1];const char*_tmpAC8;struct _dyneither_ptr*_tmpAC7;struct
_dyneither_ptr*xname=(_tmpAC7=_cycalloc(sizeof(*_tmpAC7)),((_tmpAC7[0]=(struct
_dyneither_ptr)((_tmpACA.tag=1,((_tmpACA.f1=(unsigned long)Cyc_Toc_tuple_type_counter
++,((_tmpAC9[0]=& _tmpACA,Cyc_aprintf(((_tmpAC8="_tuple%d",_tag_dyneither(_tmpAC8,
sizeof(char),9))),_tag_dyneither(_tmpAC9,sizeof(void*),1)))))))),_tmpAC7)));void*
x=Cyc_Absyn_strct(xname);struct Cyc_List_List*_tmp141=0;{struct _tuple18*_tmpACD;
struct Cyc_List_List*_tmpACC;*_tmp135=((_tmpACC=_region_malloc(d,sizeof(*_tmpACC)),((
_tmpACC->hd=((_tmpACD=_region_malloc(d,sizeof(*_tmpACD)),((_tmpACD->f1=
struct_name,((_tmpACD->f2=type_args,((_tmpACD->f3=x,_tmpACD)))))))),((_tmpACC->tl=*
_tmp135,_tmpACC))))));}{struct _RegionHandle _tmp144=_new_region("r");struct
_RegionHandle*r=& _tmp144;_push_region(r);{struct Cyc_List_List*_tmp145=((struct
Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,type_vars,type_args);for(0;fields != 
0;fields=fields->tl){struct Cyc_Absyn_Aggrfield*_tmp146=(struct Cyc_Absyn_Aggrfield*)
fields->hd;void*t=_tmp146->type;t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,
_tmp145,t));if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))break;{struct
Cyc_Absyn_Aggrfield*_tmpAD0;struct Cyc_List_List*_tmpACF;_tmp141=((_tmpACF=
_cycalloc(sizeof(*_tmpACF)),((_tmpACF->hd=((_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((
_tmpAD0->name=_tmp146->name,((_tmpAD0->tq=Cyc_Toc_mt_tq,((_tmpAD0->type=t,((
_tmpAD0->width=_tmp146->width,((_tmpAD0->attributes=_tmp146->attributes,_tmpAD0)))))))))))),((
_tmpACF->tl=_tmp141,_tmpACF))))));};}_tmp141=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp141);{struct Cyc_Absyn_AggrdeclImpl*_tmpAD5;struct
_tuple0*_tmpAD4;struct Cyc_Absyn_Aggrdecl*_tmpAD3;struct Cyc_Absyn_Aggrdecl*
_tmp149=(_tmpAD3=_cycalloc(sizeof(*_tmpAD3)),((_tmpAD3->kind=Cyc_Absyn_StructA,((
_tmpAD3->sc=Cyc_Absyn_Public,((_tmpAD3->name=((_tmpAD4=_cycalloc(sizeof(*_tmpAD4)),((
_tmpAD4->f1=Cyc_Absyn_Rel_n(0),((_tmpAD4->f2=xname,_tmpAD4)))))),((_tmpAD3->tvs=
0,((_tmpAD3->impl=((_tmpAD5=_cycalloc(sizeof(*_tmpAD5)),((_tmpAD5->exist_vars=0,((
_tmpAD5->rgn_po=0,((_tmpAD5->fields=_tmp141,((_tmpAD5->tagged=0,_tmpAD5)))))))))),((
_tmpAD3->attributes=0,_tmpAD3)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*
_tmpADB;struct Cyc_Absyn_Aggr_d_struct _tmpADA;struct Cyc_List_List*_tmpAD9;Cyc_Toc_result_decls=((
_tmpAD9=_cycalloc(sizeof(*_tmpAD9)),((_tmpAD9->hd=Cyc_Absyn_new_decl((void*)((
_tmpADB=_cycalloc(sizeof(*_tmpADB)),((_tmpADB[0]=((_tmpADA.tag=6,((_tmpADA.f1=
_tmp149,_tmpADA)))),_tmpADB)))),0),((_tmpAD9->tl=Cyc_Toc_result_decls,_tmpAD9))))));}{
void*_tmp14D=x;_npop_handler(1);return _tmp14D;};};};_pop_region(r);};};;
_pop_dynregion(d);};}struct _tuple0*Cyc_Toc_temp_var(){int _tmp155=Cyc_Toc_temp_var_counter
++;struct _dyneither_ptr*_tmpAE8;const char*_tmpAE7;void*_tmpAE6[1];struct Cyc_Int_pa_struct
_tmpAE5;struct _tuple0*_tmpAE4;struct _tuple0*res=(_tmpAE4=_cycalloc(sizeof(*
_tmpAE4)),((_tmpAE4->f1=Cyc_Absyn_Loc_n,((_tmpAE4->f2=((_tmpAE8=_cycalloc(
sizeof(*_tmpAE8)),((_tmpAE8[0]=(struct _dyneither_ptr)((_tmpAE5.tag=1,((_tmpAE5.f1=(
unsigned int)_tmp155,((_tmpAE6[0]=& _tmpAE5,Cyc_aprintf(((_tmpAE7="_tmp%X",
_tag_dyneither(_tmpAE7,sizeof(char),7))),_tag_dyneither(_tmpAE6,sizeof(void*),1)))))))),
_tmpAE8)))),_tmpAE4)))));return res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp15C;struct Cyc_Xarray_Xarray*_tmp15D;struct Cyc_Toc_TocState
_tmp15B=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp15C=
_tmp15B.dyn;_tmp15D=_tmp15B.temp_labels;{struct _DynRegionFrame _tmp15E;struct
_RegionHandle*d=_open_dynregion(& _tmp15E,_tmp15C);{int _tmp15F=Cyc_Toc_fresh_label_counter
++;if(_tmp15F < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp15D)){
struct _dyneither_ptr*_tmp160=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp15D,_tmp15F);_npop_handler(0);return _tmp160;}{struct Cyc_Int_pa_struct
_tmpAF0;void*_tmpAEF[1];const char*_tmpAEE;struct _dyneither_ptr*_tmpAED;struct
_dyneither_ptr*res=(_tmpAED=_cycalloc(sizeof(*_tmpAED)),((_tmpAED[0]=(struct
_dyneither_ptr)((_tmpAF0.tag=1,((_tmpAF0.f1=(unsigned int)_tmp15F,((_tmpAEF[0]=&
_tmpAF0,Cyc_aprintf(((_tmpAEE="_LL%X",_tag_dyneither(_tmpAEE,sizeof(char),6))),
_tag_dyneither(_tmpAEF,sizeof(void*),1)))))))),_tmpAED)));if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp15D,res)!= _tmp15F){const char*
_tmpAF3;void*_tmpAF2;(_tmpAF2=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF3="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmpAF3,sizeof(char),43))),_tag_dyneither(_tmpAF2,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp163=res;_npop_handler(0);return _tmp163;};};};
_pop_dynregion(d);};}static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*
td,struct _tuple0*name){int ans=0;struct Cyc_List_List*_tmp168=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((
struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp168))->hd)->name)
!= 0){++ ans;_tmp168=_tmp168->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);static void Cyc_Toc_aggrdecl_to_c(
struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);static void Cyc_Toc_datatypedecl_to_c(
struct Cyc_Absyn_Datatypedecl*tud);static struct _tuple8*Cyc_Toc_arg_to_c(struct
_tuple8*a){struct _dyneither_ptr*_tmp16A;struct Cyc_Absyn_Tqual _tmp16B;void*
_tmp16C;struct _tuple8 _tmp169=*a;_tmp16A=_tmp169.f1;_tmp16B=_tmp169.f2;_tmp16C=
_tmp169.f3;{struct _tuple8*_tmpAF4;return(_tmpAF4=_cycalloc(sizeof(*_tmpAF4)),((
_tmpAF4->f1=_tmp16A,((_tmpAF4->f2=_tmp16B,((_tmpAF4->f3=Cyc_Toc_typ_to_c(_tmp16C),
_tmpAF4)))))));};}static struct _tuple10*Cyc_Toc_typ_to_c_f(struct _tuple10*x){
struct Cyc_Absyn_Tqual _tmp16F;void*_tmp170;struct _tuple10 _tmp16E=*x;_tmp16F=
_tmp16E.f1;_tmp170=_tmp16E.f2;{struct _tuple10*_tmpAF5;return(_tmpAF5=_cycalloc(
sizeof(*_tmpAF5)),((_tmpAF5->f1=_tmp16F,((_tmpAF5->f2=Cyc_Toc_typ_to_c(_tmp170),
_tmpAF5)))));};}static void*Cyc_Toc_typ_to_c_array(void*t){void*_tmp172=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmp174;void*_tmp175;struct Cyc_Absyn_Tqual _tmp176;
struct Cyc_Absyn_Exp*_tmp177;union Cyc_Absyn_Constraint*_tmp178;unsigned int
_tmp179;void*_tmp17B;_LL38: {struct Cyc_Absyn_ArrayType_struct*_tmp173=(struct Cyc_Absyn_ArrayType_struct*)
_tmp172;if(_tmp173->tag != 8)goto _LL3A;else{_tmp174=_tmp173->f1;_tmp175=_tmp174.elt_type;
_tmp176=_tmp174.tq;_tmp177=_tmp174.num_elts;_tmp178=_tmp174.zero_term;_tmp179=
_tmp174.zt_loc;}}_LL39: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp175),
_tmp176,_tmp177,Cyc_Absyn_false_conref,_tmp179);_LL3A:{struct Cyc_Absyn_Evar_struct*
_tmp17A=(struct Cyc_Absyn_Evar_struct*)_tmp172;if(_tmp17A->tag != 1)goto _LL3C;
else{_tmp17B=(void*)_tmp17A->f2;}}if(!(_tmp17B != 0))goto _LL3C;_LL3B: return Cyc_Toc_typ_to_c_array((
void*)_tmp17B);_LL3C:;_LL3D: return Cyc_Toc_typ_to_c(t);_LL37:;}static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmpAF6;return(_tmpAF6=_cycalloc(sizeof(*_tmpAF6)),((_tmpAF6->name=f->name,((
_tmpAF6->tq=Cyc_Toc_mt_tq,((_tmpAF6->type=Cyc_Toc_typ_to_c(f->type),((_tmpAF6->width=
f->width,((_tmpAF6->attributes=f->attributes,_tmpAF6)))))))))));}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs){return;}static void*Cyc_Toc_char_star_typ(){static void**
cs=0;if(cs == 0){void**_tmpAF7;cs=((_tmpAF7=_cycalloc(sizeof(*_tmpAF7)),((_tmpAF7[
0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,
Cyc_Absyn_false_conref),_tmpAF7))));}return*cs;}static void*Cyc_Toc_rgn_typ(){
static void**r=0;if(r == 0){void**_tmpAF8;r=((_tmpAF8=_cycalloc(sizeof(*_tmpAF8)),((
_tmpAF8[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),
_tmpAF8))));}return*r;}static void*Cyc_Toc_dyn_rgn_typ(){static void**r=0;if(r == 0){
void**_tmpAF9;r=((_tmpAF9=_cycalloc(sizeof(*_tmpAF9)),((_tmpAF9[0]=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpAF9))));}return*
r;}static int Cyc_Toc_is_boxed_tvar(void*t){void*_tmp180=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_Tvar*_tmp182;_LL3F: {struct Cyc_Absyn_VarType_struct*_tmp181=(
struct Cyc_Absyn_VarType_struct*)_tmp180;if(_tmp181->tag != 2)goto _LL41;else{
_tmp182=_tmp181->f1;}}_LL40: return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),&
Cyc_Tcutil_tbk);_LL41:;_LL42: return 0;_LL3E:;}static int Cyc_Toc_is_abstract_type(
void*t){struct Cyc_Absyn_Kind*_tmp183=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind
_tmp184;enum Cyc_Absyn_KindQual _tmp185;_LL44: _tmp184=*_tmp183;_tmp185=_tmp184.kind;
if(_tmp185 != Cyc_Absyn_AnyKind)goto _LL46;_LL45: return 1;_LL46:;_LL47: return 0;
_LL43:;}static void*Cyc_Toc_typ_to_c(void*t){void*_tmp186=t;void*_tmp189;void**
_tmp18A;struct Cyc_Absyn_Tvar*_tmp18C;struct Cyc_Absyn_DatatypeFieldInfo _tmp18F;
union Cyc_Absyn_DatatypeFieldInfoU _tmp190;struct _tuple1 _tmp191;struct Cyc_Absyn_Datatypedecl*
_tmp192;struct Cyc_Absyn_Datatypefield*_tmp193;struct Cyc_Absyn_PtrInfo _tmp196;
void*_tmp197;struct Cyc_Absyn_Tqual _tmp198;struct Cyc_Absyn_PtrAtts _tmp199;union
Cyc_Absyn_Constraint*_tmp19A;struct Cyc_Absyn_ArrayInfo _tmp19E;void*_tmp19F;
struct Cyc_Absyn_Tqual _tmp1A0;struct Cyc_Absyn_Exp*_tmp1A1;unsigned int _tmp1A2;
struct Cyc_Absyn_FnInfo _tmp1A4;struct Cyc_Absyn_Tqual _tmp1A5;void*_tmp1A6;struct
Cyc_List_List*_tmp1A7;int _tmp1A8;struct Cyc_Absyn_VarargInfo*_tmp1A9;struct Cyc_List_List*
_tmp1AA;struct Cyc_List_List*_tmp1AC;enum Cyc_Absyn_AggrKind _tmp1AE;struct Cyc_List_List*
_tmp1AF;struct Cyc_Absyn_AggrInfo _tmp1B1;union Cyc_Absyn_AggrInfoU _tmp1B2;struct
Cyc_List_List*_tmp1B3;struct _tuple0*_tmp1B5;struct Cyc_List_List*_tmp1B7;struct
_tuple0*_tmp1B9;struct Cyc_List_List*_tmp1BA;struct Cyc_Absyn_Typedefdecl*_tmp1BB;
void*_tmp1BC;void*_tmp1BF;struct Cyc_Absyn_TypeDecl*_tmp1C9;struct Cyc_Absyn_TypeDecl
_tmp1CA;void*_tmp1CB;struct Cyc_Absyn_Aggrdecl*_tmp1CD;struct Cyc_Absyn_TypeDecl*
_tmp1CF;struct Cyc_Absyn_TypeDecl _tmp1D0;void*_tmp1D1;struct Cyc_Absyn_Enumdecl*
_tmp1D3;struct Cyc_Absyn_TypeDecl*_tmp1D5;struct Cyc_Absyn_TypeDecl _tmp1D6;void*
_tmp1D7;struct Cyc_Absyn_Datatypedecl*_tmp1D9;void**_tmp1DA;_LL49: {struct Cyc_Absyn_VoidType_struct*
_tmp187=(struct Cyc_Absyn_VoidType_struct*)_tmp186;if(_tmp187->tag != 0)goto _LL4B;}
_LL4A: return t;_LL4B: {struct Cyc_Absyn_Evar_struct*_tmp188=(struct Cyc_Absyn_Evar_struct*)
_tmp186;if(_tmp188->tag != 1)goto _LL4D;else{_tmp189=(void**)& _tmp188->f2;_tmp18A=(
void**)((void**)& _tmp188->f2);}}_LL4C: if(*_tmp18A == 0){*_tmp18A=(void*)Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp18A));
_LL4D: {struct Cyc_Absyn_VarType_struct*_tmp18B=(struct Cyc_Absyn_VarType_struct*)
_tmp186;if(_tmp18B->tag != 2)goto _LL4F;else{_tmp18C=_tmp18B->f1;}}_LL4E: if((Cyc_Tcutil_tvar_kind(
_tmp18C,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)return(void*)& Cyc_Absyn_VoidType_val;
else{return Cyc_Absyn_void_star_typ();}_LL4F: {struct Cyc_Absyn_DatatypeType_struct*
_tmp18D=(struct Cyc_Absyn_DatatypeType_struct*)_tmp186;if(_tmp18D->tag != 3)goto
_LL51;}_LL50: return(void*)& Cyc_Absyn_VoidType_val;_LL51: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp18E=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp186;if(_tmp18E->tag != 4)
goto _LL53;else{_tmp18F=_tmp18E->f1;_tmp190=_tmp18F.field_info;if((_tmp190.KnownDatatypefield).tag
!= 2)goto _LL53;_tmp191=(struct _tuple1)(_tmp190.KnownDatatypefield).val;_tmp192=
_tmp191.f1;_tmp193=_tmp191.f2;}}_LL52: {const char*_tmpAFA;return Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp193->name,((_tmpAFA="_struct",_tag_dyneither(
_tmpAFA,sizeof(char),8)))));}_LL53: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp194=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp186;if(_tmp194->tag != 4)
goto _LL55;}_LL54: {const char*_tmpAFD;void*_tmpAFC;(_tmpAFC=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAFD="unresolved DatatypeFieldType",
_tag_dyneither(_tmpAFD,sizeof(char),29))),_tag_dyneither(_tmpAFC,sizeof(void*),0)));}
_LL55: {struct Cyc_Absyn_PointerType_struct*_tmp195=(struct Cyc_Absyn_PointerType_struct*)
_tmp186;if(_tmp195->tag != 5)goto _LL57;else{_tmp196=_tmp195->f1;_tmp197=_tmp196.elt_typ;
_tmp198=_tmp196.elt_tq;_tmp199=_tmp196.ptr_atts;_tmp19A=_tmp199.bounds;}}_LL56:
_tmp197=Cyc_Toc_typ_to_c_array(_tmp197);{void*_tmp1DE=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp19A);_LL86: {
struct Cyc_Absyn_DynEither_b_struct*_tmp1DF=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp1DE;if(_tmp1DF->tag != 0)goto _LL88;}_LL87: return Cyc_Toc_dyneither_ptr_typ;
_LL88:;_LL89: return Cyc_Absyn_star_typ(_tmp197,(void*)& Cyc_Absyn_HeapRgn_val,
_tmp198,Cyc_Absyn_false_conref);_LL85:;};_LL57: {struct Cyc_Absyn_IntType_struct*
_tmp19B=(struct Cyc_Absyn_IntType_struct*)_tmp186;if(_tmp19B->tag != 6)goto _LL59;}
_LL58: goto _LL5A;_LL59: {struct Cyc_Absyn_FloatType_struct*_tmp19C=(struct Cyc_Absyn_FloatType_struct*)
_tmp186;if(_tmp19C->tag != 7)goto _LL5B;}_LL5A: return t;_LL5B: {struct Cyc_Absyn_ArrayType_struct*
_tmp19D=(struct Cyc_Absyn_ArrayType_struct*)_tmp186;if(_tmp19D->tag != 8)goto _LL5D;
else{_tmp19E=_tmp19D->f1;_tmp19F=_tmp19E.elt_type;_tmp1A0=_tmp19E.tq;_tmp1A1=
_tmp19E.num_elts;_tmp1A2=_tmp19E.zt_loc;}}_LL5C: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmp19F),_tmp1A0,_tmp1A1,Cyc_Absyn_false_conref,_tmp1A2);_LL5D: {struct Cyc_Absyn_FnType_struct*
_tmp1A3=(struct Cyc_Absyn_FnType_struct*)_tmp186;if(_tmp1A3->tag != 9)goto _LL5F;
else{_tmp1A4=_tmp1A3->f1;_tmp1A5=_tmp1A4.ret_tqual;_tmp1A6=_tmp1A4.ret_typ;
_tmp1A7=_tmp1A4.args;_tmp1A8=_tmp1A4.c_varargs;_tmp1A9=_tmp1A4.cyc_varargs;
_tmp1AA=_tmp1A4.attributes;}}_LL5E: {struct Cyc_List_List*_tmp1E0=0;for(0;_tmp1AA
!= 0;_tmp1AA=_tmp1AA->tl){void*_tmp1E1=(void*)_tmp1AA->hd;_LL8B: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp1E2=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp1E1;if(_tmp1E2->tag != 4)goto
_LL8D;}_LL8C: goto _LL8E;_LL8D: {struct Cyc_Absyn_Const_att_struct*_tmp1E3=(struct
Cyc_Absyn_Const_att_struct*)_tmp1E1;if(_tmp1E3->tag != 5)goto _LL8F;}_LL8E: goto
_LL90;_LL8F: {struct Cyc_Absyn_Format_att_struct*_tmp1E4=(struct Cyc_Absyn_Format_att_struct*)
_tmp1E1;if(_tmp1E4->tag != 19)goto _LL91;}_LL90: continue;_LL91: {struct Cyc_Absyn_Noliveunique_att_struct*
_tmp1E5=(struct Cyc_Absyn_Noliveunique_att_struct*)_tmp1E1;if(_tmp1E5->tag != 21)
goto _LL93;}_LL92: continue;_LL93: {struct Cyc_Absyn_Initializes_att_struct*_tmp1E6=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp1E1;if(_tmp1E6->tag != 20)goto _LL95;}
_LL94: continue;_LL95:;_LL96:{struct Cyc_List_List*_tmpAFE;_tmp1E0=((_tmpAFE=
_cycalloc(sizeof(*_tmpAFE)),((_tmpAFE->hd=(void*)_tmp1AA->hd,((_tmpAFE->tl=
_tmp1E0,_tmpAFE))))));}goto _LL8A;_LL8A:;}{struct Cyc_List_List*_tmp1E8=((struct
Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_arg_to_c,_tmp1A7);if(_tmp1A9 != 0){void*_tmp1E9=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp1A9->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple8*_tmpAFF;struct _tuple8*_tmp1EA=(_tmpAFF=_cycalloc(sizeof(*_tmpAFF)),((
_tmpAFF->f1=_tmp1A9->name,((_tmpAFF->f2=_tmp1A9->tq,((_tmpAFF->f3=_tmp1E9,
_tmpAFF)))))));struct Cyc_List_List*_tmpB00;_tmp1E8=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1E8,((
_tmpB00=_cycalloc(sizeof(*_tmpB00)),((_tmpB00->hd=_tmp1EA,((_tmpB00->tl=0,
_tmpB00)))))));}{struct Cyc_Absyn_FnType_struct _tmpB06;struct Cyc_Absyn_FnInfo
_tmpB05;struct Cyc_Absyn_FnType_struct*_tmpB04;return(void*)((_tmpB04=_cycalloc(
sizeof(*_tmpB04)),((_tmpB04[0]=((_tmpB06.tag=9,((_tmpB06.f1=((_tmpB05.tvars=0,((
_tmpB05.effect=0,((_tmpB05.ret_tqual=_tmp1A5,((_tmpB05.ret_typ=Cyc_Toc_typ_to_c(
_tmp1A6),((_tmpB05.args=_tmp1E8,((_tmpB05.c_varargs=_tmp1A8,((_tmpB05.cyc_varargs=
0,((_tmpB05.rgn_po=0,((_tmpB05.attributes=_tmp1E0,_tmpB05)))))))))))))))))),
_tmpB06)))),_tmpB04))));};};}_LL5F: {struct Cyc_Absyn_TupleType_struct*_tmp1AB=(
struct Cyc_Absyn_TupleType_struct*)_tmp186;if(_tmp1AB->tag != 10)goto _LL61;else{
_tmp1AC=_tmp1AB->f1;}}_LL60: _tmp1AC=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(
struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1AC);
return Cyc_Toc_add_tuple_type(_tmp1AC);_LL61: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp1AD=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp186;if(_tmp1AD->tag != 12)goto
_LL63;else{_tmp1AE=_tmp1AD->f1;_tmp1AF=_tmp1AD->f2;}}_LL62: {struct Cyc_Absyn_AnonAggrType_struct
_tmpB09;struct Cyc_Absyn_AnonAggrType_struct*_tmpB08;return(void*)((_tmpB08=
_cycalloc(sizeof(*_tmpB08)),((_tmpB08[0]=((_tmpB09.tag=12,((_tmpB09.f1=_tmp1AE,((
_tmpB09.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1AF),_tmpB09)))))),
_tmpB08))));}_LL63: {struct Cyc_Absyn_AggrType_struct*_tmp1B0=(struct Cyc_Absyn_AggrType_struct*)
_tmp186;if(_tmp1B0->tag != 11)goto _LL65;else{_tmp1B1=_tmp1B0->f1;_tmp1B2=_tmp1B1.aggr_info;
_tmp1B3=_tmp1B1.targs;}}_LL64:{union Cyc_Absyn_AggrInfoU _tmp1F2=_tmp1B2;struct
_tuple2 _tmp1F3;_LL98: if((_tmp1F2.UnknownAggr).tag != 1)goto _LL9A;_tmp1F3=(struct
_tuple2)(_tmp1F2.UnknownAggr).val;_LL99: return t;_LL9A:;_LL9B: goto _LL97;_LL97:;}{
struct Cyc_Absyn_Aggrdecl*_tmp1F4=Cyc_Absyn_get_known_aggrdecl(_tmp1B2);if(
_tmp1F4->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(_tmp1F4->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp1F5=_tmp1F4->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1F4->impl))->fields;if(_tmp1F5 == 0)return Cyc_Toc_aggrdecl_type(
_tmp1F4->name,Cyc_Absyn_strctq);for(0;_tmp1F5->tl != 0;_tmp1F5=_tmp1F5->tl){;}{
void*_tmp1F6=((struct Cyc_Absyn_Aggrfield*)_tmp1F5->hd)->type;if(Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp1F6)))return Cyc_Toc_add_struct_type(
_tmp1F4->name,_tmp1F4->tvs,((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Toc_typ_to_c,_tmp1B3),((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1F4->impl))->fields);return Cyc_Toc_aggrdecl_type(_tmp1F4->name,
Cyc_Absyn_strctq);};};};_LL65: {struct Cyc_Absyn_EnumType_struct*_tmp1B4=(struct
Cyc_Absyn_EnumType_struct*)_tmp186;if(_tmp1B4->tag != 13)goto _LL67;else{_tmp1B5=
_tmp1B4->f1;}}_LL66: return t;_LL67: {struct Cyc_Absyn_AnonEnumType_struct*_tmp1B6=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp186;if(_tmp1B6->tag != 14)goto _LL69;
else{_tmp1B7=_tmp1B6->f1;}}_LL68: Cyc_Toc_enumfields_to_c(_tmp1B7);return t;_LL69: {
struct Cyc_Absyn_TypedefType_struct*_tmp1B8=(struct Cyc_Absyn_TypedefType_struct*)
_tmp186;if(_tmp1B8->tag != 17)goto _LL6B;else{_tmp1B9=_tmp1B8->f1;_tmp1BA=_tmp1B8->f2;
_tmp1BB=_tmp1B8->f3;_tmp1BC=(void*)_tmp1B8->f4;}}_LL6A: if(_tmp1BC == 0  || Cyc_noexpand_r){
if(_tmp1BA != 0){struct Cyc_Absyn_TypedefType_struct _tmpB0C;struct Cyc_Absyn_TypedefType_struct*
_tmpB0B;return(void*)((_tmpB0B=_cycalloc(sizeof(*_tmpB0B)),((_tmpB0B[0]=((
_tmpB0C.tag=17,((_tmpB0C.f1=_tmp1B9,((_tmpB0C.f2=0,((_tmpB0C.f3=_tmp1BB,((
_tmpB0C.f4=(void*)0,_tmpB0C)))))))))),_tmpB0B))));}else{return t;}}else{struct Cyc_Absyn_TypedefType_struct
_tmpB0F;struct Cyc_Absyn_TypedefType_struct*_tmpB0E;return(void*)((_tmpB0E=
_cycalloc(sizeof(*_tmpB0E)),((_tmpB0E[0]=((_tmpB0F.tag=17,((_tmpB0F.f1=_tmp1B9,((
_tmpB0F.f2=0,((_tmpB0F.f3=_tmp1BB,((_tmpB0F.f4=(void*)((void*)Cyc_Toc_typ_to_c_array((
void*)_tmp1BC)),_tmpB0F)))))))))),_tmpB0E))));}_LL6B: {struct Cyc_Absyn_TagType_struct*
_tmp1BD=(struct Cyc_Absyn_TagType_struct*)_tmp186;if(_tmp1BD->tag != 19)goto _LL6D;}
_LL6C: return Cyc_Absyn_uint_typ;_LL6D: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp1BE=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp186;if(_tmp1BE->tag != 15)goto
_LL6F;else{_tmp1BF=(void*)_tmp1BE->f1;}}_LL6E: return Cyc_Toc_rgn_typ();_LL6F: {
struct Cyc_Absyn_DynRgnType_struct*_tmp1C0=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp186;if(_tmp1C0->tag != 16)goto _LL71;}_LL70: return Cyc_Toc_dyn_rgn_typ();_LL71: {
struct Cyc_Absyn_HeapRgn_struct*_tmp1C1=(struct Cyc_Absyn_HeapRgn_struct*)_tmp186;
if(_tmp1C1->tag != 20)goto _LL73;}_LL72: goto _LL74;_LL73: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp1C2=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp186;if(_tmp1C2->tag != 21)goto
_LL75;}_LL74: goto _LL76;_LL75: {struct Cyc_Absyn_RefCntRgn_struct*_tmp1C3=(struct
Cyc_Absyn_RefCntRgn_struct*)_tmp186;if(_tmp1C3->tag != 22)goto _LL77;}_LL76: goto
_LL78;_LL77: {struct Cyc_Absyn_AccessEff_struct*_tmp1C4=(struct Cyc_Absyn_AccessEff_struct*)
_tmp186;if(_tmp1C4->tag != 23)goto _LL79;}_LL78: goto _LL7A;_LL79: {struct Cyc_Absyn_JoinEff_struct*
_tmp1C5=(struct Cyc_Absyn_JoinEff_struct*)_tmp186;if(_tmp1C5->tag != 24)goto _LL7B;}
_LL7A: goto _LL7C;_LL7B: {struct Cyc_Absyn_RgnsEff_struct*_tmp1C6=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp186;if(_tmp1C6->tag != 25)goto _LL7D;}_LL7C: goto _LL7E;_LL7D: {struct Cyc_Absyn_ValueofType_struct*
_tmp1C7=(struct Cyc_Absyn_ValueofType_struct*)_tmp186;if(_tmp1C7->tag != 18)goto
_LL7F;}_LL7E: return Cyc_Absyn_void_star_typ();_LL7F: {struct Cyc_Absyn_TypeDeclType_struct*
_tmp1C8=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp186;if(_tmp1C8->tag != 26)goto
_LL81;else{_tmp1C9=_tmp1C8->f1;_tmp1CA=*_tmp1C9;_tmp1CB=_tmp1CA.r;{struct Cyc_Absyn_Aggr_td_struct*
_tmp1CC=(struct Cyc_Absyn_Aggr_td_struct*)_tmp1CB;if(_tmp1CC->tag != 0)goto _LL81;
else{_tmp1CD=_tmp1CC->f1;}};}}_LL80: Cyc_Toc_aggrdecl_to_c(_tmp1CD,1);if(_tmp1CD->kind
== Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(_tmp1CD->name,Cyc_Absyn_unionq_typ);
else{return Cyc_Toc_aggrdecl_type(_tmp1CD->name,Cyc_Absyn_strctq);}_LL81: {struct
Cyc_Absyn_TypeDeclType_struct*_tmp1CE=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp186;if(_tmp1CE->tag != 26)goto _LL83;else{_tmp1CF=_tmp1CE->f1;_tmp1D0=*_tmp1CF;
_tmp1D1=_tmp1D0.r;{struct Cyc_Absyn_Enum_td_struct*_tmp1D2=(struct Cyc_Absyn_Enum_td_struct*)
_tmp1D1;if(_tmp1D2->tag != 1)goto _LL83;else{_tmp1D3=_tmp1D2->f1;}};}}_LL82: Cyc_Toc_enumdecl_to_c(
_tmp1D3);return t;_LL83: {struct Cyc_Absyn_TypeDeclType_struct*_tmp1D4=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp186;if(_tmp1D4->tag != 26)goto _LL48;else{_tmp1D5=_tmp1D4->f1;_tmp1D6=*_tmp1D5;
_tmp1D7=_tmp1D6.r;{struct Cyc_Absyn_Datatype_td_struct*_tmp1D8=(struct Cyc_Absyn_Datatype_td_struct*)
_tmp1D7;if(_tmp1D8->tag != 2)goto _LL48;else{_tmp1D9=_tmp1D8->f1;}};_tmp1DA=
_tmp1D4->f2;}}_LL84: Cyc_Toc_datatypedecl_to_c(_tmp1D9);return Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp1DA)));_LL48:;}static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(
void*t,struct Cyc_Absyn_Exp*e,unsigned int l){void*_tmp1FB=t;struct Cyc_Absyn_ArrayInfo
_tmp1FD;void*_tmp1FE;struct Cyc_Absyn_Tqual _tmp1FF;_LL9D: {struct Cyc_Absyn_ArrayType_struct*
_tmp1FC=(struct Cyc_Absyn_ArrayType_struct*)_tmp1FB;if(_tmp1FC->tag != 8)goto _LL9F;
else{_tmp1FD=_tmp1FC->f1;_tmp1FE=_tmp1FD.elt_type;_tmp1FF=_tmp1FD.tq;}}_LL9E:
return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp1FE,(void*)& Cyc_Absyn_HeapRgn_val,
_tmp1FF,Cyc_Absyn_false_conref),e);_LL9F:;_LLA0: return Cyc_Toc_cast_it(t,e);_LL9C:;}
static int Cyc_Toc_atomic_typ(void*t){void*_tmp200=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_ArrayInfo _tmp20A;void*_tmp20B;struct Cyc_Absyn_AggrInfo _tmp20D;union Cyc_Absyn_AggrInfoU
_tmp20E;struct Cyc_List_List*_tmp210;struct Cyc_Absyn_DatatypeFieldInfo _tmp212;
union Cyc_Absyn_DatatypeFieldInfoU _tmp213;struct _tuple1 _tmp214;struct Cyc_Absyn_Datatypedecl*
_tmp215;struct Cyc_Absyn_Datatypefield*_tmp216;struct Cyc_List_List*_tmp218;_LLA2: {
struct Cyc_Absyn_VoidType_struct*_tmp201=(struct Cyc_Absyn_VoidType_struct*)
_tmp200;if(_tmp201->tag != 0)goto _LLA4;}_LLA3: return 1;_LLA4: {struct Cyc_Absyn_VarType_struct*
_tmp202=(struct Cyc_Absyn_VarType_struct*)_tmp200;if(_tmp202->tag != 2)goto _LLA6;}
_LLA5: return 0;_LLA6: {struct Cyc_Absyn_IntType_struct*_tmp203=(struct Cyc_Absyn_IntType_struct*)
_tmp200;if(_tmp203->tag != 6)goto _LLA8;}_LLA7: goto _LLA9;_LLA8: {struct Cyc_Absyn_EnumType_struct*
_tmp204=(struct Cyc_Absyn_EnumType_struct*)_tmp200;if(_tmp204->tag != 13)goto _LLAA;}
_LLA9: goto _LLAB;_LLAA: {struct Cyc_Absyn_AnonEnumType_struct*_tmp205=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp200;if(_tmp205->tag != 14)goto _LLAC;}_LLAB: goto _LLAD;_LLAC: {struct Cyc_Absyn_FloatType_struct*
_tmp206=(struct Cyc_Absyn_FloatType_struct*)_tmp200;if(_tmp206->tag != 7)goto _LLAE;}
_LLAD: goto _LLAF;_LLAE: {struct Cyc_Absyn_FnType_struct*_tmp207=(struct Cyc_Absyn_FnType_struct*)
_tmp200;if(_tmp207->tag != 9)goto _LLB0;}_LLAF: goto _LLB1;_LLB0: {struct Cyc_Absyn_TagType_struct*
_tmp208=(struct Cyc_Absyn_TagType_struct*)_tmp200;if(_tmp208->tag != 19)goto _LLB2;}
_LLB1: return 1;_LLB2: {struct Cyc_Absyn_ArrayType_struct*_tmp209=(struct Cyc_Absyn_ArrayType_struct*)
_tmp200;if(_tmp209->tag != 8)goto _LLB4;else{_tmp20A=_tmp209->f1;_tmp20B=_tmp20A.elt_type;}}
_LLB3: return Cyc_Toc_atomic_typ(_tmp20B);_LLB4: {struct Cyc_Absyn_AggrType_struct*
_tmp20C=(struct Cyc_Absyn_AggrType_struct*)_tmp200;if(_tmp20C->tag != 11)goto _LLB6;
else{_tmp20D=_tmp20C->f1;_tmp20E=_tmp20D.aggr_info;}}_LLB5:{union Cyc_Absyn_AggrInfoU
_tmp21D=_tmp20E;struct _tuple2 _tmp21E;_LLC7: if((_tmp21D.UnknownAggr).tag != 1)goto
_LLC9;_tmp21E=(struct _tuple2)(_tmp21D.UnknownAggr).val;_LLC8: return 0;_LLC9:;
_LLCA: goto _LLC6;_LLC6:;}{struct Cyc_Absyn_Aggrdecl*_tmp21F=Cyc_Absyn_get_known_aggrdecl(
_tmp20E);if(_tmp21F->impl == 0)return 0;{struct Cyc_List_List*_tmp220=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp21F->impl))->fields;for(0;_tmp220 != 0;_tmp220=_tmp220->tl){if(!
Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp220->hd)->type))return 0;}}
return 1;};_LLB6: {struct Cyc_Absyn_AnonAggrType_struct*_tmp20F=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp200;if(_tmp20F->tag != 12)goto _LLB8;else{_tmp210=_tmp20F->f2;}}_LLB7: for(0;
_tmp210 != 0;_tmp210=_tmp210->tl){if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)
_tmp210->hd)->type))return 0;}return 1;_LLB8: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp211=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp200;if(_tmp211->tag != 4)
goto _LLBA;else{_tmp212=_tmp211->f1;_tmp213=_tmp212.field_info;if((_tmp213.KnownDatatypefield).tag
!= 2)goto _LLBA;_tmp214=(struct _tuple1)(_tmp213.KnownDatatypefield).val;_tmp215=
_tmp214.f1;_tmp216=_tmp214.f2;}}_LLB9: _tmp218=_tmp216->typs;goto _LLBB;_LLBA: {
struct Cyc_Absyn_TupleType_struct*_tmp217=(struct Cyc_Absyn_TupleType_struct*)
_tmp200;if(_tmp217->tag != 10)goto _LLBC;else{_tmp218=_tmp217->f1;}}_LLBB: for(0;
_tmp218 != 0;_tmp218=_tmp218->tl){if(!Cyc_Toc_atomic_typ((*((struct _tuple10*)
_tmp218->hd)).f2))return 0;}return 1;_LLBC: {struct Cyc_Absyn_DatatypeType_struct*
_tmp219=(struct Cyc_Absyn_DatatypeType_struct*)_tmp200;if(_tmp219->tag != 3)goto
_LLBE;}_LLBD: goto _LLBF;_LLBE: {struct Cyc_Absyn_PointerType_struct*_tmp21A=(
struct Cyc_Absyn_PointerType_struct*)_tmp200;if(_tmp21A->tag != 5)goto _LLC0;}_LLBF:
goto _LLC1;_LLC0: {struct Cyc_Absyn_DynRgnType_struct*_tmp21B=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp200;if(_tmp21B->tag != 16)goto _LLC2;}_LLC1: goto _LLC3;_LLC2: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp21C=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp200;if(_tmp21C->tag != 15)goto
_LLC4;}_LLC3: return 0;_LLC4:;_LLC5: {const char*_tmpB13;void*_tmpB12[1];struct Cyc_String_pa_struct
_tmpB11;(_tmpB11.tag=0,((_tmpB11.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpB12[0]=& _tmpB11,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB13="atomic_typ:  bad type %s",
_tag_dyneither(_tmpB13,sizeof(char),25))),_tag_dyneither(_tmpB12,sizeof(void*),1)))))));}
_LLA1:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp224=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp226;void*_tmp227;_LLCC: {struct Cyc_Absyn_PointerType_struct*
_tmp225=(struct Cyc_Absyn_PointerType_struct*)_tmp224;if(_tmp225->tag != 5)goto
_LLCE;else{_tmp226=_tmp225->f1;_tmp227=_tmp226.elt_typ;}}_LLCD: {void*_tmp228=
Cyc_Tcutil_compress(_tmp227);_LLD1: {struct Cyc_Absyn_VoidType_struct*_tmp229=(
struct Cyc_Absyn_VoidType_struct*)_tmp228;if(_tmp229->tag != 0)goto _LLD3;}_LLD2:
return 1;_LLD3:;_LLD4: return 0;_LLD0:;}_LLCE:;_LLCF: return 0;_LLCB:;}static int Cyc_Toc_is_void_star_or_boxed_tvar(
void*t){return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}static int Cyc_Toc_is_poly_field(
void*t,struct _dyneither_ptr*f){void*_tmp22A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp22C;union Cyc_Absyn_AggrInfoU _tmp22D;struct Cyc_List_List*_tmp22F;_LLD6: {
struct Cyc_Absyn_AggrType_struct*_tmp22B=(struct Cyc_Absyn_AggrType_struct*)
_tmp22A;if(_tmp22B->tag != 11)goto _LLD8;else{_tmp22C=_tmp22B->f1;_tmp22D=_tmp22C.aggr_info;}}
_LLD7: {struct Cyc_Absyn_Aggrdecl*_tmp230=Cyc_Absyn_get_known_aggrdecl(_tmp22D);
if(_tmp230->impl == 0){const char*_tmpB16;void*_tmpB15;(_tmpB15=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB16="is_poly_field: type missing fields",
_tag_dyneither(_tmpB16,sizeof(char),35))),_tag_dyneither(_tmpB15,sizeof(void*),0)));}
_tmp22F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp230->impl))->fields;goto
_LLD9;}_LLD8: {struct Cyc_Absyn_AnonAggrType_struct*_tmp22E=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp22A;if(_tmp22E->tag != 12)goto _LLDA;else{_tmp22F=_tmp22E->f2;}}_LLD9: {struct
Cyc_Absyn_Aggrfield*_tmp233=Cyc_Absyn_lookup_field(_tmp22F,f);if(_tmp233 == 0){
const char*_tmpB1A;void*_tmpB19[1];struct Cyc_String_pa_struct _tmpB18;(_tmpB18.tag=
0,((_tmpB18.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB19[0]=&
_tmpB18,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB1A="is_poly_field: bad field %s",_tag_dyneither(_tmpB1A,sizeof(char),28))),
_tag_dyneither(_tmpB19,sizeof(void*),1)))))));}return Cyc_Toc_is_void_star_or_boxed_tvar(
_tmp233->type);}_LLDA:;_LLDB: {const char*_tmpB1E;void*_tmpB1D[1];struct Cyc_String_pa_struct
_tmpB1C;(_tmpB1C.tag=0,((_tmpB1C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpB1D[0]=& _tmpB1C,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB1E="is_poly_field: bad type %s",
_tag_dyneither(_tmpB1E,sizeof(char),27))),_tag_dyneither(_tmpB1D,sizeof(void*),1)))))));}
_LLD5:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmp23A=e->r;
struct Cyc_Absyn_Exp*_tmp23C;struct _dyneither_ptr*_tmp23D;struct Cyc_Absyn_Exp*
_tmp23F;struct _dyneither_ptr*_tmp240;_LLDD: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp23B=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp23A;if(_tmp23B->tag != 20)goto
_LLDF;else{_tmp23C=_tmp23B->f1;_tmp23D=_tmp23B->f2;}}_LLDE: return Cyc_Toc_is_poly_field((
void*)_check_null(_tmp23C->topt),_tmp23D);_LLDF: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp23E=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp23A;if(_tmp23E->tag != 21)goto
_LLE1;else{_tmp23F=_tmp23E->f1;_tmp240=_tmp23E->f2;}}_LLE0: {void*_tmp241=Cyc_Tcutil_compress((
void*)_check_null(_tmp23F->topt));struct Cyc_Absyn_PtrInfo _tmp243;void*_tmp244;
_LLE4: {struct Cyc_Absyn_PointerType_struct*_tmp242=(struct Cyc_Absyn_PointerType_struct*)
_tmp241;if(_tmp242->tag != 5)goto _LLE6;else{_tmp243=_tmp242->f1;_tmp244=_tmp243.elt_typ;}}
_LLE5: return Cyc_Toc_is_poly_field(_tmp244,_tmp240);_LLE6:;_LLE7: {const char*
_tmpB22;void*_tmpB21[1];struct Cyc_String_pa_struct _tmpB20;(_tmpB20.tag=0,((
_tmpB20.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)_check_null(_tmp23F->topt))),((_tmpB21[0]=& _tmpB20,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB22="is_poly_project: bad type %s",
_tag_dyneither(_tmpB22,sizeof(char),29))),_tag_dyneither(_tmpB21,sizeof(void*),1)))))));}
_LLE3:;}_LLE1:;_LLE2: return 0;_LLDC:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp*s){struct Cyc_List_List*_tmpB23;return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_e,((_tmpB23=_cycalloc(sizeof(*_tmpB23)),((_tmpB23->hd=s,((
_tmpB23->tl=0,_tmpB23)))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp*s){struct Cyc_List_List*_tmpB24;return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_atomic_e,((_tmpB24=_cycalloc(sizeof(*_tmpB24)),((_tmpB24->hd=s,((
_tmpB24->tl=0,_tmpB24)))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(
void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(
s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmpB25[2];
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpB25[1]=s,((_tmpB25[0]=
rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB25,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*_tmpB26[2];return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_atomic_e,((_tmpB26[1]=n,((_tmpB26[0]=s,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB26,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);}else{struct Cyc_Absyn_Exp*_tmpB27[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((
_tmpB27[1]=n,((_tmpB27[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB27,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n){struct Cyc_Absyn_Exp*_tmpB28[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((
_tmpB28[2]=n,((_tmpB28[1]=s,((_tmpB28[0]=rgn,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB28,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmpB29;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmpB29=_cycalloc(sizeof(*_tmpB29)),((_tmpB29->hd=e,((_tmpB29->tl=0,_tmpB29)))))),
0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){struct Cyc_Absyn_Exp*
_tmpB2A[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpB2A,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*
_tmp250=e->r;union Cyc_Absyn_Cnst _tmp252;struct _dyneither_ptr _tmp253;union Cyc_Absyn_Cnst
_tmp255;struct _dyneither_ptr _tmp256;_LLE9: {struct Cyc_Absyn_Const_e_struct*
_tmp251=(struct Cyc_Absyn_Const_e_struct*)_tmp250;if(_tmp251->tag != 0)goto _LLEB;
else{_tmp252=_tmp251->f1;if((_tmp252.String_c).tag != 8)goto _LLEB;_tmp253=(struct
_dyneither_ptr)(_tmp252.String_c).val;}}_LLEA: is_string=1;goto _LLE8;_LLEB: {
struct Cyc_Absyn_Const_e_struct*_tmp254=(struct Cyc_Absyn_Const_e_struct*)_tmp250;
if(_tmp254->tag != 0)goto _LLED;else{_tmp255=_tmp254->f1;if((_tmp255.Wstring_c).tag
!= 9)goto _LLED;_tmp256=(struct _dyneither_ptr)(_tmp255.Wstring_c).val;}}_LLEC:
is_string=1;goto _LLE8;_LLED:;_LLEE: goto _LLE8;_LLE8:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple0*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_struct*_tmpB30;struct
Cyc_Absyn_Var_d_struct _tmpB2F;struct Cyc_List_List*_tmpB2E;Cyc_Toc_result_decls=((
_tmpB2E=_cycalloc(sizeof(*_tmpB2E)),((_tmpB2E->hd=Cyc_Absyn_new_decl((void*)((
_tmpB30=_cycalloc(sizeof(*_tmpB30)),((_tmpB30[0]=((_tmpB2F.tag=0,((_tmpB2F.f1=vd,
_tmpB2F)))),_tmpB30)))),0),((_tmpB2E->tl=Cyc_Toc_result_decls,_tmpB2E))))));}
xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*urm_exp;{struct _tuple16*_tmpB37;
struct _tuple16*_tmpB36;struct _tuple16*_tmpB35;struct _tuple16*_tmpB34[3];urm_exp=
Cyc_Absyn_unresolvedmem_exp(0,((_tmpB34[2]=((_tmpB35=_cycalloc(sizeof(*_tmpB35)),((
_tmpB35->f1=0,((_tmpB35->f2=xplussz,_tmpB35)))))),((_tmpB34[1]=((_tmpB36=
_cycalloc(sizeof(*_tmpB36)),((_tmpB36->f1=0,((_tmpB36->f2=xexp,_tmpB36)))))),((
_tmpB34[0]=((_tmpB37=_cycalloc(sizeof(*_tmpB37)),((_tmpB37->f1=0,((_tmpB37->f2=
xexp,_tmpB37)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB34,sizeof(struct _tuple16*),3)))))))),0);}return urm_exp;};};}
struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*
binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct
_dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct
_RegionHandle*rgn;};typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;static int Cyc_Toc_is_toplevel(
struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp25F;int _tmp260;struct Cyc_Toc_Env*
_tmp25E=nv;_tmp25F=*_tmp25E;_tmp260=_tmp25F.toplevel;return _tmp260;}static int Cyc_Toc_in_lhs(
struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp262;int*_tmp263;struct Cyc_Toc_Env*
_tmp261=nv;_tmp262=*_tmp261;_tmp263=_tmp262.in_lhs;return*_tmp263;}static struct
Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple0*x){struct
Cyc_Toc_Env _tmp265;struct Cyc_Dict_Dict _tmp266;struct Cyc_Toc_Env*_tmp264=nv;
_tmp265=*_tmp264;_tmp266=_tmp265.varmap;return((struct Cyc_Absyn_Exp*(*)(struct
Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp266,x);}static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(struct _RegionHandle*r){int*_tmpB3A;struct Cyc_Toc_Env*_tmpB39;
return(_tmpB39=_region_malloc(r,sizeof(*_tmpB39)),((_tmpB39->break_lab=(struct
_dyneither_ptr**)0,((_tmpB39->continue_lab=(struct _dyneither_ptr**)0,((_tmpB39->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpB39->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp),((_tmpB39->toplevel=(int)1,((_tmpB39->in_lhs=(int*)((
_tmpB3A=_region_malloc(r,sizeof(*_tmpB3A)),((_tmpB3A[0]=0,_tmpB3A)))),((_tmpB39->rgn=(
struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB39)))))))))))))));}static struct
Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct
Cyc_Toc_Env _tmp26A;struct _dyneither_ptr**_tmp26B;struct _dyneither_ptr**_tmp26C;
struct Cyc_Toc_FallthruInfo*_tmp26D;struct Cyc_Dict_Dict _tmp26E;int _tmp26F;int*
_tmp270;struct Cyc_Toc_Env*_tmp269=e;_tmp26A=*_tmp269;_tmp26B=_tmp26A.break_lab;
_tmp26C=_tmp26A.continue_lab;_tmp26D=_tmp26A.fallthru_info;_tmp26E=_tmp26A.varmap;
_tmp26F=_tmp26A.toplevel;_tmp270=_tmp26A.in_lhs;{struct Cyc_Toc_Env*_tmpB3B;
return(_tmpB3B=_region_malloc(r,sizeof(*_tmpB3B)),((_tmpB3B->break_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp26B),((_tmpB3B->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp26C),((_tmpB3B->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp26D,((_tmpB3B->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp26E),((_tmpB3B->toplevel=(int)_tmp26F,((_tmpB3B->in_lhs=(int*)_tmp270,((
_tmpB3B->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB3B)))))))))))))));};}
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp273;struct _dyneither_ptr**_tmp274;struct _dyneither_ptr**
_tmp275;struct Cyc_Toc_FallthruInfo*_tmp276;struct Cyc_Dict_Dict _tmp277;int _tmp278;
int*_tmp279;struct Cyc_Toc_Env*_tmp272=e;_tmp273=*_tmp272;_tmp274=_tmp273.break_lab;
_tmp275=_tmp273.continue_lab;_tmp276=_tmp273.fallthru_info;_tmp277=_tmp273.varmap;
_tmp278=_tmp273.toplevel;_tmp279=_tmp273.in_lhs;{struct Cyc_Toc_Env*_tmpB3C;
return(_tmpB3C=_region_malloc(r,sizeof(*_tmpB3C)),((_tmpB3C->break_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp274),((_tmpB3C->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp275),((_tmpB3C->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp276,((_tmpB3C->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp277),((_tmpB3C->toplevel=(int)0,((_tmpB3C->in_lhs=(int*)_tmp279,((_tmpB3C->rgn=(
struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB3C)))))))))))))));};}static
struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env _tmp27C;struct _dyneither_ptr**_tmp27D;struct _dyneither_ptr**
_tmp27E;struct Cyc_Toc_FallthruInfo*_tmp27F;struct Cyc_Dict_Dict _tmp280;int _tmp281;
int*_tmp282;struct Cyc_Toc_Env*_tmp27B=e;_tmp27C=*_tmp27B;_tmp27D=_tmp27C.break_lab;
_tmp27E=_tmp27C.continue_lab;_tmp27F=_tmp27C.fallthru_info;_tmp280=_tmp27C.varmap;
_tmp281=_tmp27C.toplevel;_tmp282=_tmp27C.in_lhs;{struct Cyc_Toc_Env*_tmpB3D;
return(_tmpB3D=_region_malloc(r,sizeof(*_tmpB3D)),((_tmpB3D->break_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp27D),((_tmpB3D->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp27E),((_tmpB3D->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp27F,((_tmpB3D->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp280),((_tmpB3D->toplevel=(int)1,((_tmpB3D->in_lhs=(int*)_tmp282,((_tmpB3D->rgn=(
struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB3D)))))))))))))));};}static void
Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){struct Cyc_Toc_Env _tmp285;int*_tmp286;
struct Cyc_Toc_Env*_tmp284=e;_tmp285=*_tmp284;_tmp286=_tmp285.in_lhs;*_tmp286=b;}
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){{union Cyc_Absyn_Nmspace _tmp287=(*x).f1;
struct Cyc_List_List*_tmp288;_LLF0: if((_tmp287.Rel_n).tag != 1)goto _LLF2;_tmp288=(
struct Cyc_List_List*)(_tmp287.Rel_n).val;_LLF1: {const char*_tmpB41;void*_tmpB40[
1];struct Cyc_String_pa_struct _tmpB3F;(_tmpB3F.tag=0,((_tmpB3F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpB40[0]=&
_tmpB3F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB41="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpB41,sizeof(char),30))),
_tag_dyneither(_tmpB40,sizeof(void*),1)))))));}_LLF2:;_LLF3: goto _LLEF;_LLEF:;}{
struct Cyc_Toc_Env _tmp28D;struct _dyneither_ptr**_tmp28E;struct _dyneither_ptr**
_tmp28F;struct Cyc_Toc_FallthruInfo*_tmp290;struct Cyc_Dict_Dict _tmp291;int _tmp292;
int*_tmp293;struct Cyc_Toc_Env*_tmp28C=e;_tmp28D=*_tmp28C;_tmp28E=_tmp28D.break_lab;
_tmp28F=_tmp28D.continue_lab;_tmp290=_tmp28D.fallthru_info;_tmp291=_tmp28D.varmap;
_tmp292=_tmp28D.toplevel;_tmp293=_tmp28D.in_lhs;{struct Cyc_Dict_Dict _tmp294=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*
v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp291),x,y);struct Cyc_Toc_Env*_tmpB42;return(_tmpB42=
_region_malloc(r,sizeof(*_tmpB42)),((_tmpB42->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp28E),((_tmpB42->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp28F),((_tmpB42->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp290,((_tmpB42->varmap=(struct Cyc_Dict_Dict)_tmp294,((_tmpB42->toplevel=(int)
_tmp292,((_tmpB42->in_lhs=(int*)_tmp293,((_tmpB42->rgn=(struct _RegionHandle*)r,(
struct Cyc_Toc_Env*)_tmpB42)))))))))))))));};};}static struct Cyc_Toc_Env*Cyc_Toc_loop_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp297;struct
_dyneither_ptr**_tmp298;struct _dyneither_ptr**_tmp299;struct Cyc_Toc_FallthruInfo*
_tmp29A;struct Cyc_Dict_Dict _tmp29B;int _tmp29C;int*_tmp29D;struct Cyc_Toc_Env*
_tmp296=e;_tmp297=*_tmp296;_tmp298=_tmp297.break_lab;_tmp299=_tmp297.continue_lab;
_tmp29A=_tmp297.fallthru_info;_tmp29B=_tmp297.varmap;_tmp29C=_tmp297.toplevel;
_tmp29D=_tmp297.in_lhs;{struct Cyc_Toc_Env*_tmpB43;return(_tmpB43=_region_malloc(
r,sizeof(*_tmpB43)),((_tmpB43->break_lab=(struct _dyneither_ptr**)0,((_tmpB43->continue_lab=(
struct _dyneither_ptr**)0,((_tmpB43->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp29A,((_tmpB43->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29B),((_tmpB43->toplevel=(
int)_tmp29C,((_tmpB43->in_lhs=(int*)_tmp29D,((_tmpB43->rgn=(struct _RegionHandle*)
r,(struct Cyc_Toc_Env*)_tmpB43)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmpB44;fallthru_vars=((
_tmpB44=_region_malloc(r,sizeof(*_tmpB44)),((_tmpB44->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmpB44->tl=fallthru_vars,_tmpB44))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp2A1;struct _dyneither_ptr**_tmp2A2;struct
_dyneither_ptr**_tmp2A3;struct Cyc_Toc_FallthruInfo*_tmp2A4;struct Cyc_Dict_Dict
_tmp2A5;int _tmp2A6;int*_tmp2A7;struct Cyc_Toc_Env*_tmp2A0=e;_tmp2A1=*_tmp2A0;
_tmp2A2=_tmp2A1.break_lab;_tmp2A3=_tmp2A1.continue_lab;_tmp2A4=_tmp2A1.fallthru_info;
_tmp2A5=_tmp2A1.varmap;_tmp2A6=_tmp2A1.toplevel;_tmp2A7=_tmp2A1.in_lhs;{struct
Cyc_Toc_Env _tmp2A9;struct Cyc_Dict_Dict _tmp2AA;struct Cyc_Toc_Env*_tmp2A8=
next_case_env;_tmp2A9=*_tmp2A8;_tmp2AA=_tmp2A9.varmap;{struct Cyc_Toc_FallthruInfo*
_tmpB45;struct Cyc_Toc_FallthruInfo*fi=(_tmpB45=_region_malloc(r,sizeof(*_tmpB45)),((
_tmpB45->label=fallthru_l,((_tmpB45->binders=fallthru_vars,((_tmpB45->next_case_env=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp2AA),_tmpB45)))))));struct _dyneither_ptr**_tmpB48;struct Cyc_Toc_Env*
_tmpB47;return(_tmpB47=_region_malloc(r,sizeof(*_tmpB47)),((_tmpB47->break_lab=(
struct _dyneither_ptr**)((_tmpB48=_region_malloc(r,sizeof(*_tmpB48)),((_tmpB48[0]=
break_l,_tmpB48)))),((_tmpB47->continue_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp2A3),((_tmpB47->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
fi,((_tmpB47->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2A5),((_tmpB47->toplevel=(
int)_tmp2A6,((_tmpB47->in_lhs=(int*)((int*)_tmp2A7),((_tmpB47->rgn=(struct
_RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB47)))))))))))))));};};};}static struct
Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _dyneither_ptr*break_l){struct Cyc_Toc_Env _tmp2AF;struct _dyneither_ptr**
_tmp2B0;struct _dyneither_ptr**_tmp2B1;struct Cyc_Toc_FallthruInfo*_tmp2B2;struct
Cyc_Dict_Dict _tmp2B3;int _tmp2B4;int*_tmp2B5;struct Cyc_Toc_Env*_tmp2AE=e;_tmp2AF=*
_tmp2AE;_tmp2B0=_tmp2AF.break_lab;_tmp2B1=_tmp2AF.continue_lab;_tmp2B2=_tmp2AF.fallthru_info;
_tmp2B3=_tmp2AF.varmap;_tmp2B4=_tmp2AF.toplevel;_tmp2B5=_tmp2AF.in_lhs;{struct
_dyneither_ptr**_tmpB4B;struct Cyc_Toc_Env*_tmpB4A;return(_tmpB4A=_region_malloc(
r,sizeof(*_tmpB4A)),((_tmpB4A->break_lab=(struct _dyneither_ptr**)((_tmpB4B=
_region_malloc(r,sizeof(*_tmpB4B)),((_tmpB4B[0]=break_l,_tmpB4B)))),((_tmpB4A->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp2B1),((_tmpB4A->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpB4A->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2B3),((_tmpB4A->toplevel=(
int)_tmp2B4,((_tmpB4A->in_lhs=(int*)_tmp2B5,((_tmpB4A->rgn=(struct _RegionHandle*)
r,(struct Cyc_Toc_Env*)_tmpB4A)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp2B9;struct _dyneither_ptr**_tmp2BA;struct _dyneither_ptr**_tmp2BB;struct Cyc_Toc_FallthruInfo*
_tmp2BC;struct Cyc_Dict_Dict _tmp2BD;int _tmp2BE;int*_tmp2BF;struct Cyc_Toc_Env*
_tmp2B8=e;_tmp2B9=*_tmp2B8;_tmp2BA=_tmp2B9.break_lab;_tmp2BB=_tmp2B9.continue_lab;
_tmp2BC=_tmp2B9.fallthru_info;_tmp2BD=_tmp2B9.varmap;_tmp2BE=_tmp2B9.toplevel;
_tmp2BF=_tmp2B9.in_lhs;{struct Cyc_Toc_FallthruInfo*_tmpB4E;struct Cyc_Toc_Env*
_tmpB4D;return(_tmpB4D=_region_malloc(r,sizeof(*_tmpB4D)),((_tmpB4D->break_lab=(
struct _dyneither_ptr**)0,((_tmpB4D->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp2BB),((_tmpB4D->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((
_tmpB4E=_region_malloc(r,sizeof(*_tmpB4E)),((_tmpB4E->label=next_l,((_tmpB4E->binders=
0,((_tmpB4E->next_case_env=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*
cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),
_tmpB4E)))))))),((_tmpB4D->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2BD),((_tmpB4D->toplevel=(
int)_tmp2BE,((_tmpB4D->in_lhs=(int*)((int*)_tmp2BF),((_tmpB4D->rgn=(struct
_RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB4D)))))))))))))));};}static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct
Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);static int Cyc_Toc_need_null_check(struct
Cyc_Absyn_Exp*e){void*_tmp2C2=e->annot;_LLF5: {struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp2C3=(struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp2C2;if(_tmp2C3->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLF7;}_LLF6: return Cyc_Toc_is_nullable((void*)_check_null(e->topt));_LLF7: {
struct Cyc_CfFlowInfo_NotZero_struct*_tmp2C4=(struct Cyc_CfFlowInfo_NotZero_struct*)
_tmp2C2;if(_tmp2C4->tag != Cyc_CfFlowInfo_NotZero)goto _LLF9;}_LLF8: return 0;_LLF9: {
struct Cyc_CfFlowInfo_IsZero_struct*_tmp2C5=(struct Cyc_CfFlowInfo_IsZero_struct*)
_tmp2C2;if(_tmp2C5->tag != Cyc_CfFlowInfo_IsZero)goto _LLFB;}_LLFA:{const char*
_tmpB51;void*_tmpB50;(_tmpB50=0,Cyc_Tcutil_terr(e->loc,((_tmpB51="dereference of NULL pointer",
_tag_dyneither(_tmpB51,sizeof(char),28))),_tag_dyneither(_tmpB50,sizeof(void*),0)));}
return 0;_LLFB: {struct Cyc_Absyn_EmptyAnnot_struct*_tmp2C6=(struct Cyc_Absyn_EmptyAnnot_struct*)
_tmp2C2;if(_tmp2C6->tag != Cyc_Absyn_EmptyAnnot)goto _LLFD;}_LLFC: return 0;_LLFD:;
_LLFE: {const char*_tmpB54;void*_tmpB53;(_tmpB53=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB54="need_null_check",
_tag_dyneither(_tmpB54,sizeof(char),16))),_tag_dyneither(_tmpB53,sizeof(void*),0)));}
_LLF4:;}static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*
_tmp2CB=e->annot;struct Cyc_List_List*_tmp2CD;struct Cyc_List_List*_tmp2CF;_LL100: {
struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2CC=(struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmp2CB;if(_tmp2CC->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL102;else{_tmp2CD=
_tmp2CC->f1;}}_LL101: return _tmp2CD;_LL102: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmp2CE=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmp2CB;if(_tmp2CE->tag != Cyc_CfFlowInfo_NotZero)
goto _LL104;else{_tmp2CF=_tmp2CE->f1;}}_LL103: return _tmp2CF;_LL104: {struct Cyc_CfFlowInfo_IsZero_struct*
_tmp2D0=(struct Cyc_CfFlowInfo_IsZero_struct*)_tmp2CB;if(_tmp2D0->tag != Cyc_CfFlowInfo_IsZero)
goto _LL106;}_LL105:{const char*_tmpB57;void*_tmpB56;(_tmpB56=0,Cyc_Tcutil_terr(e->loc,((
_tmpB57="dereference of NULL pointer",_tag_dyneither(_tmpB57,sizeof(char),28))),
_tag_dyneither(_tmpB56,sizeof(void*),0)));}return 0;_LL106: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp2D1=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp2CB;if(_tmp2D1->tag != Cyc_Absyn_EmptyAnnot)
goto _LL108;}_LL107: return 0;_LL108:;_LL109: {const char*_tmpB5A;void*_tmpB59;(
_tmpB59=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB5A="get_relns",_tag_dyneither(_tmpB5A,sizeof(char),10))),_tag_dyneither(
_tmpB59,sizeof(void*),0)));}_LLFF:;}static int Cyc_Toc_check_const_array(
unsigned int i,void*t){void*_tmp2D6=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp2D8;struct Cyc_Absyn_PtrAtts _tmp2D9;union Cyc_Absyn_Constraint*_tmp2DA;union
Cyc_Absyn_Constraint*_tmp2DB;struct Cyc_Absyn_ArrayInfo _tmp2DD;struct Cyc_Absyn_Exp*
_tmp2DE;_LL10B: {struct Cyc_Absyn_PointerType_struct*_tmp2D7=(struct Cyc_Absyn_PointerType_struct*)
_tmp2D6;if(_tmp2D7->tag != 5)goto _LL10D;else{_tmp2D8=_tmp2D7->f1;_tmp2D9=_tmp2D8.ptr_atts;
_tmp2DA=_tmp2D9.bounds;_tmp2DB=_tmp2D9.zero_term;}}_LL10C: {void*_tmp2DF=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp2DA);struct Cyc_Absyn_Exp*_tmp2E2;_LL112: {struct Cyc_Absyn_DynEither_b_struct*
_tmp2E0=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2DF;if(_tmp2E0->tag != 0)goto
_LL114;}_LL113: return 0;_LL114: {struct Cyc_Absyn_Upper_b_struct*_tmp2E1=(struct
Cyc_Absyn_Upper_b_struct*)_tmp2DF;if(_tmp2E1->tag != 1)goto _LL111;else{_tmp2E2=
_tmp2E1->f1;}}_LL115: {unsigned int _tmp2E4;int _tmp2E5;struct _tuple11 _tmp2E3=Cyc_Evexp_eval_const_uint_exp(
_tmp2E2);_tmp2E4=_tmp2E3.f1;_tmp2E5=_tmp2E3.f2;return _tmp2E5  && i <= _tmp2E4;}
_LL111:;}_LL10D: {struct Cyc_Absyn_ArrayType_struct*_tmp2DC=(struct Cyc_Absyn_ArrayType_struct*)
_tmp2D6;if(_tmp2DC->tag != 8)goto _LL10F;else{_tmp2DD=_tmp2DC->f1;_tmp2DE=_tmp2DD.num_elts;}}
_LL10E: if(_tmp2DE == 0)return 0;{unsigned int _tmp2E7;int _tmp2E8;struct _tuple11
_tmp2E6=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp2DE);_tmp2E7=
_tmp2E6.f1;_tmp2E8=_tmp2E6.f2;return _tmp2E8  && i <= _tmp2E7;};_LL10F:;_LL110:
return 0;_LL10A:;}static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2E9=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(
_tmp2E9->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp _tmp2EA=_tmp2E9->rop;struct
Cyc_Absyn_Vardecl*_tmp2EB;struct Cyc_Absyn_Vardecl*_tmp2EC;_LL117: if((_tmp2EA.LessNumelts).tag
!= 3)goto _LL119;_tmp2EB=(struct Cyc_Absyn_Vardecl*)(_tmp2EA.LessNumelts).val;
_LL118: _tmp2EC=_tmp2EB;goto _LL11A;_LL119: if((_tmp2EA.LessEqNumelts).tag != 5)goto
_LL11B;_tmp2EC=(struct Cyc_Absyn_Vardecl*)(_tmp2EA.LessEqNumelts).val;_LL11A: if(
_tmp2EC == v)return 1;else{goto _LL116;}_LL11B:;_LL11C: continue;_LL116:;};}return 0;}
static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Exp*e,void*vtype){{void*_tmp2ED=e->r;void*_tmp2EF;struct Cyc_Absyn_Vardecl*
_tmp2F1;void*_tmp2F3;struct Cyc_Absyn_Vardecl*_tmp2F5;void*_tmp2F7;struct Cyc_Absyn_Vardecl*
_tmp2F9;void*_tmp2FB;struct Cyc_Absyn_Vardecl*_tmp2FD;union Cyc_Absyn_Cnst _tmp2FF;
struct _tuple5 _tmp300;enum Cyc_Absyn_Sign _tmp301;int _tmp302;union Cyc_Absyn_Cnst
_tmp304;struct _tuple5 _tmp305;enum Cyc_Absyn_Sign _tmp306;int _tmp307;union Cyc_Absyn_Cnst
_tmp309;struct _tuple5 _tmp30A;enum Cyc_Absyn_Sign _tmp30B;int _tmp30C;enum Cyc_Absyn_Primop
_tmp30E;struct Cyc_List_List*_tmp30F;struct Cyc_List_List _tmp310;struct Cyc_Absyn_Exp*
_tmp311;_LL11E: {struct Cyc_Absyn_Var_e_struct*_tmp2EE=(struct Cyc_Absyn_Var_e_struct*)
_tmp2ED;if(_tmp2EE->tag != 1)goto _LL120;else{_tmp2EF=(void*)_tmp2EE->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2F0=(struct Cyc_Absyn_Pat_b_struct*)_tmp2EF;if(_tmp2F0->tag
!= 5)goto _LL120;else{_tmp2F1=_tmp2F0->f1;}};}}_LL11F: _tmp2F5=_tmp2F1;goto _LL121;
_LL120: {struct Cyc_Absyn_Var_e_struct*_tmp2F2=(struct Cyc_Absyn_Var_e_struct*)
_tmp2ED;if(_tmp2F2->tag != 1)goto _LL122;else{_tmp2F3=(void*)_tmp2F2->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2F4=(struct Cyc_Absyn_Local_b_struct*)_tmp2F3;if(
_tmp2F4->tag != 4)goto _LL122;else{_tmp2F5=_tmp2F4->f1;}};}}_LL121: _tmp2F9=_tmp2F5;
goto _LL123;_LL122: {struct Cyc_Absyn_Var_e_struct*_tmp2F6=(struct Cyc_Absyn_Var_e_struct*)
_tmp2ED;if(_tmp2F6->tag != 1)goto _LL124;else{_tmp2F7=(void*)_tmp2F6->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2F8=(struct Cyc_Absyn_Global_b_struct*)_tmp2F7;if(
_tmp2F8->tag != 1)goto _LL124;else{_tmp2F9=_tmp2F8->f1;}};}}_LL123: _tmp2FD=_tmp2F9;
goto _LL125;_LL124: {struct Cyc_Absyn_Var_e_struct*_tmp2FA=(struct Cyc_Absyn_Var_e_struct*)
_tmp2ED;if(_tmp2FA->tag != 1)goto _LL126;else{_tmp2FB=(void*)_tmp2FA->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp2FC=(struct Cyc_Absyn_Param_b_struct*)_tmp2FB;if(
_tmp2FC->tag != 3)goto _LL126;else{_tmp2FD=_tmp2FC->f1;}};}}_LL125: if(_tmp2FD->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp2FD))return 1;goto _LL11D;_LL126: {
struct Cyc_Absyn_Const_e_struct*_tmp2FE=(struct Cyc_Absyn_Const_e_struct*)_tmp2ED;
if(_tmp2FE->tag != 0)goto _LL128;else{_tmp2FF=_tmp2FE->f1;if((_tmp2FF.Int_c).tag != 
5)goto _LL128;_tmp300=(struct _tuple5)(_tmp2FF.Int_c).val;_tmp301=_tmp300.f1;if(
_tmp301 != Cyc_Absyn_None)goto _LL128;_tmp302=_tmp300.f2;}}_LL127: _tmp307=_tmp302;
goto _LL129;_LL128: {struct Cyc_Absyn_Const_e_struct*_tmp303=(struct Cyc_Absyn_Const_e_struct*)
_tmp2ED;if(_tmp303->tag != 0)goto _LL12A;else{_tmp304=_tmp303->f1;if((_tmp304.Int_c).tag
!= 5)goto _LL12A;_tmp305=(struct _tuple5)(_tmp304.Int_c).val;_tmp306=_tmp305.f1;
if(_tmp306 != Cyc_Absyn_Signed)goto _LL12A;_tmp307=_tmp305.f2;}}_LL129: return
_tmp307 >= 0  && Cyc_Toc_check_const_array((unsigned int)_tmp307,vtype);_LL12A: {
struct Cyc_Absyn_Const_e_struct*_tmp308=(struct Cyc_Absyn_Const_e_struct*)_tmp2ED;
if(_tmp308->tag != 0)goto _LL12C;else{_tmp309=_tmp308->f1;if((_tmp309.Int_c).tag != 
5)goto _LL12C;_tmp30A=(struct _tuple5)(_tmp309.Int_c).val;_tmp30B=_tmp30A.f1;if(
_tmp30B != Cyc_Absyn_Unsigned)goto _LL12C;_tmp30C=_tmp30A.f2;}}_LL12B: return Cyc_Toc_check_const_array((
unsigned int)_tmp30C,vtype);_LL12C: {struct Cyc_Absyn_Primop_e_struct*_tmp30D=(
struct Cyc_Absyn_Primop_e_struct*)_tmp2ED;if(_tmp30D->tag != 2)goto _LL12E;else{
_tmp30E=_tmp30D->f1;if(_tmp30E != Cyc_Absyn_Numelts)goto _LL12E;_tmp30F=_tmp30D->f2;
if(_tmp30F == 0)goto _LL12E;_tmp310=*_tmp30F;_tmp311=(struct Cyc_Absyn_Exp*)_tmp310.hd;}}
_LL12D:{void*_tmp312=_tmp311->r;void*_tmp314;struct Cyc_Absyn_Vardecl*_tmp316;
void*_tmp318;struct Cyc_Absyn_Vardecl*_tmp31A;void*_tmp31C;struct Cyc_Absyn_Vardecl*
_tmp31E;void*_tmp320;struct Cyc_Absyn_Vardecl*_tmp322;_LL131: {struct Cyc_Absyn_Var_e_struct*
_tmp313=(struct Cyc_Absyn_Var_e_struct*)_tmp312;if(_tmp313->tag != 1)goto _LL133;
else{_tmp314=(void*)_tmp313->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp315=(struct Cyc_Absyn_Pat_b_struct*)
_tmp314;if(_tmp315->tag != 5)goto _LL133;else{_tmp316=_tmp315->f1;}};}}_LL132:
_tmp31A=_tmp316;goto _LL134;_LL133: {struct Cyc_Absyn_Var_e_struct*_tmp317=(struct
Cyc_Absyn_Var_e_struct*)_tmp312;if(_tmp317->tag != 1)goto _LL135;else{_tmp318=(
void*)_tmp317->f2;{struct Cyc_Absyn_Local_b_struct*_tmp319=(struct Cyc_Absyn_Local_b_struct*)
_tmp318;if(_tmp319->tag != 4)goto _LL135;else{_tmp31A=_tmp319->f1;}};}}_LL134:
_tmp31E=_tmp31A;goto _LL136;_LL135: {struct Cyc_Absyn_Var_e_struct*_tmp31B=(struct
Cyc_Absyn_Var_e_struct*)_tmp312;if(_tmp31B->tag != 1)goto _LL137;else{_tmp31C=(
void*)_tmp31B->f2;{struct Cyc_Absyn_Global_b_struct*_tmp31D=(struct Cyc_Absyn_Global_b_struct*)
_tmp31C;if(_tmp31D->tag != 1)goto _LL137;else{_tmp31E=_tmp31D->f1;}};}}_LL136:
_tmp322=_tmp31E;goto _LL138;_LL137: {struct Cyc_Absyn_Var_e_struct*_tmp31F=(struct
Cyc_Absyn_Var_e_struct*)_tmp312;if(_tmp31F->tag != 1)goto _LL139;else{_tmp320=(
void*)_tmp31F->f2;{struct Cyc_Absyn_Param_b_struct*_tmp321=(struct Cyc_Absyn_Param_b_struct*)
_tmp320;if(_tmp321->tag != 3)goto _LL139;else{_tmp322=_tmp321->f1;}};}}_LL138:
return _tmp322 == v;_LL139:;_LL13A: goto _LL130;_LL130:;}goto _LL11D;_LL12E:;_LL12F:
goto _LL11D;_LL11D:;}return 0;}static char _tmp334[8]="*bogus*";struct _tuple19{void*
f1;void*f2;};static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,
struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){struct Cyc_Absyn_Vardecl*x;{void*
_tmp323=a->r;void*_tmp325;struct Cyc_Absyn_Vardecl*_tmp327;void*_tmp329;struct Cyc_Absyn_Vardecl*
_tmp32B;void*_tmp32D;struct Cyc_Absyn_Vardecl*_tmp32F;void*_tmp331;struct Cyc_Absyn_Vardecl*
_tmp333;_LL13C: {struct Cyc_Absyn_Var_e_struct*_tmp324=(struct Cyc_Absyn_Var_e_struct*)
_tmp323;if(_tmp324->tag != 1)goto _LL13E;else{_tmp325=(void*)_tmp324->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp326=(struct Cyc_Absyn_Pat_b_struct*)_tmp325;if(_tmp326->tag
!= 5)goto _LL13E;else{_tmp327=_tmp326->f1;}};}}_LL13D: _tmp32B=_tmp327;goto _LL13F;
_LL13E: {struct Cyc_Absyn_Var_e_struct*_tmp328=(struct Cyc_Absyn_Var_e_struct*)
_tmp323;if(_tmp328->tag != 1)goto _LL140;else{_tmp329=(void*)_tmp328->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp32A=(struct Cyc_Absyn_Local_b_struct*)_tmp329;if(
_tmp32A->tag != 4)goto _LL140;else{_tmp32B=_tmp32A->f1;}};}}_LL13F: _tmp32F=_tmp32B;
goto _LL141;_LL140: {struct Cyc_Absyn_Var_e_struct*_tmp32C=(struct Cyc_Absyn_Var_e_struct*)
_tmp323;if(_tmp32C->tag != 1)goto _LL142;else{_tmp32D=(void*)_tmp32C->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp32E=(struct Cyc_Absyn_Global_b_struct*)_tmp32D;if(
_tmp32E->tag != 1)goto _LL142;else{_tmp32F=_tmp32E->f1;}};}}_LL141: _tmp333=_tmp32F;
goto _LL143;_LL142: {struct Cyc_Absyn_Var_e_struct*_tmp330=(struct Cyc_Absyn_Var_e_struct*)
_tmp323;if(_tmp330->tag != 1)goto _LL144;else{_tmp331=(void*)_tmp330->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp332=(struct Cyc_Absyn_Param_b_struct*)_tmp331;if(
_tmp332->tag != 3)goto _LL144;else{_tmp333=_tmp332->f1;}};}}_LL143: if(_tmp333->escapes)
goto _LL145;x=_tmp333;goto _LL13B;_LL144:;_LL145: {static struct _dyneither_ptr
bogus_string={_tmp334,_tmp334,_tmp334 + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
4,0}},& bogus_string};static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,&
bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};x=& bogus_vardecl;
x->type=a_typ;}_LL13B:;}{void*_tmp335=a_typ;inner_loop: {void*_tmp336=i->r;void*
_tmp338;struct Cyc_Absyn_Exp*_tmp339;union Cyc_Absyn_Cnst _tmp33B;struct _tuple5
_tmp33C;enum Cyc_Absyn_Sign _tmp33D;int _tmp33E;union Cyc_Absyn_Cnst _tmp340;struct
_tuple5 _tmp341;enum Cyc_Absyn_Sign _tmp342;int _tmp343;union Cyc_Absyn_Cnst _tmp345;
struct _tuple5 _tmp346;enum Cyc_Absyn_Sign _tmp347;int _tmp348;enum Cyc_Absyn_Primop
_tmp34A;struct Cyc_List_List*_tmp34B;struct Cyc_List_List _tmp34C;struct Cyc_Absyn_Exp*
_tmp34D;struct Cyc_List_List*_tmp34E;struct Cyc_List_List _tmp34F;struct Cyc_Absyn_Exp*
_tmp350;void*_tmp352;struct Cyc_Absyn_Vardecl*_tmp354;void*_tmp356;struct Cyc_Absyn_Vardecl*
_tmp358;void*_tmp35A;struct Cyc_Absyn_Vardecl*_tmp35C;void*_tmp35E;struct Cyc_Absyn_Vardecl*
_tmp360;_LL147: {struct Cyc_Absyn_Cast_e_struct*_tmp337=(struct Cyc_Absyn_Cast_e_struct*)
_tmp336;if(_tmp337->tag != 13)goto _LL149;else{_tmp338=(void*)_tmp337->f1;_tmp339=
_tmp337->f2;}}_LL148: i=_tmp339;goto inner_loop;_LL149: {struct Cyc_Absyn_Const_e_struct*
_tmp33A=(struct Cyc_Absyn_Const_e_struct*)_tmp336;if(_tmp33A->tag != 0)goto _LL14B;
else{_tmp33B=_tmp33A->f1;if((_tmp33B.Int_c).tag != 5)goto _LL14B;_tmp33C=(struct
_tuple5)(_tmp33B.Int_c).val;_tmp33D=_tmp33C.f1;if(_tmp33D != Cyc_Absyn_None)goto
_LL14B;_tmp33E=_tmp33C.f2;}}_LL14A: _tmp343=_tmp33E;goto _LL14C;_LL14B: {struct Cyc_Absyn_Const_e_struct*
_tmp33F=(struct Cyc_Absyn_Const_e_struct*)_tmp336;if(_tmp33F->tag != 0)goto _LL14D;
else{_tmp340=_tmp33F->f1;if((_tmp340.Int_c).tag != 5)goto _LL14D;_tmp341=(struct
_tuple5)(_tmp340.Int_c).val;_tmp342=_tmp341.f1;if(_tmp342 != Cyc_Absyn_Signed)
goto _LL14D;_tmp343=_tmp341.f2;}}_LL14C: return _tmp343 >= 0  && Cyc_Toc_check_const_array((
unsigned int)(_tmp343 + 1),_tmp335);_LL14D: {struct Cyc_Absyn_Const_e_struct*
_tmp344=(struct Cyc_Absyn_Const_e_struct*)_tmp336;if(_tmp344->tag != 0)goto _LL14F;
else{_tmp345=_tmp344->f1;if((_tmp345.Int_c).tag != 5)goto _LL14F;_tmp346=(struct
_tuple5)(_tmp345.Int_c).val;_tmp347=_tmp346.f1;if(_tmp347 != Cyc_Absyn_Unsigned)
goto _LL14F;_tmp348=_tmp346.f2;}}_LL14E: return Cyc_Toc_check_const_array((
unsigned int)(_tmp348 + 1),_tmp335);_LL14F: {struct Cyc_Absyn_Primop_e_struct*
_tmp349=(struct Cyc_Absyn_Primop_e_struct*)_tmp336;if(_tmp349->tag != 2)goto _LL151;
else{_tmp34A=_tmp349->f1;if(_tmp34A != Cyc_Absyn_Mod)goto _LL151;_tmp34B=_tmp349->f2;
if(_tmp34B == 0)goto _LL151;_tmp34C=*_tmp34B;_tmp34D=(struct Cyc_Absyn_Exp*)_tmp34C.hd;
_tmp34E=_tmp34C.tl;if(_tmp34E == 0)goto _LL151;_tmp34F=*_tmp34E;_tmp350=(struct Cyc_Absyn_Exp*)
_tmp34F.hd;}}_LL150: return Cyc_Toc_check_leq_size(relns,x,_tmp350,_tmp335);_LL151: {
struct Cyc_Absyn_Var_e_struct*_tmp351=(struct Cyc_Absyn_Var_e_struct*)_tmp336;if(
_tmp351->tag != 1)goto _LL153;else{_tmp352=(void*)_tmp351->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp353=(struct Cyc_Absyn_Pat_b_struct*)_tmp352;if(_tmp353->tag != 5)goto _LL153;
else{_tmp354=_tmp353->f1;}};}}_LL152: _tmp358=_tmp354;goto _LL154;_LL153: {struct
Cyc_Absyn_Var_e_struct*_tmp355=(struct Cyc_Absyn_Var_e_struct*)_tmp336;if(_tmp355->tag
!= 1)goto _LL155;else{_tmp356=(void*)_tmp355->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp357=(struct Cyc_Absyn_Local_b_struct*)_tmp356;if(_tmp357->tag != 4)goto _LL155;
else{_tmp358=_tmp357->f1;}};}}_LL154: _tmp35C=_tmp358;goto _LL156;_LL155: {struct
Cyc_Absyn_Var_e_struct*_tmp359=(struct Cyc_Absyn_Var_e_struct*)_tmp336;if(_tmp359->tag
!= 1)goto _LL157;else{_tmp35A=(void*)_tmp359->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp35B=(struct Cyc_Absyn_Global_b_struct*)_tmp35A;if(_tmp35B->tag != 1)goto _LL157;
else{_tmp35C=_tmp35B->f1;}};}}_LL156: _tmp360=_tmp35C;goto _LL158;_LL157: {struct
Cyc_Absyn_Var_e_struct*_tmp35D=(struct Cyc_Absyn_Var_e_struct*)_tmp336;if(_tmp35D->tag
!= 1)goto _LL159;else{_tmp35E=(void*)_tmp35D->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp35F=(struct Cyc_Absyn_Param_b_struct*)_tmp35E;if(_tmp35F->tag != 3)goto _LL159;
else{_tmp360=_tmp35F->f1;}};}}_LL158: if(_tmp360->escapes)return 0;{struct Cyc_List_List*
_tmp361=relns;for(0;_tmp361 != 0;_tmp361=_tmp361->tl){struct Cyc_CfFlowInfo_Reln*
_tmp362=(struct Cyc_CfFlowInfo_Reln*)_tmp361->hd;if(_tmp362->vd == _tmp360){union
Cyc_CfFlowInfo_RelnOp _tmp363=_tmp362->rop;struct Cyc_Absyn_Vardecl*_tmp364;struct
_tuple12 _tmp365;struct Cyc_Absyn_Vardecl*_tmp366;void*_tmp367;unsigned int _tmp368;
unsigned int _tmp369;_LL15C: if((_tmp363.LessNumelts).tag != 3)goto _LL15E;_tmp364=(
struct Cyc_Absyn_Vardecl*)(_tmp363.LessNumelts).val;_LL15D: if(x == _tmp364)return 1;
else{goto _LL15B;}_LL15E: if((_tmp363.LessVar).tag != 2)goto _LL160;_tmp365=(struct
_tuple12)(_tmp363.LessVar).val;_tmp366=_tmp365.f1;_tmp367=_tmp365.f2;_LL15F:{
struct _tuple19 _tmpB5B;struct _tuple19 _tmp36B=(_tmpB5B.f1=Cyc_Tcutil_compress(
_tmp367),((_tmpB5B.f2=Cyc_Tcutil_compress((void*)_check_null(a->topt)),_tmpB5B)));
void*_tmp36C;void*_tmp36E;void*_tmp36F;struct Cyc_Absyn_PtrInfo _tmp371;struct Cyc_Absyn_PtrAtts
_tmp372;union Cyc_Absyn_Constraint*_tmp373;_LL167: _tmp36C=_tmp36B.f1;{struct Cyc_Absyn_TagType_struct*
_tmp36D=(struct Cyc_Absyn_TagType_struct*)_tmp36C;if(_tmp36D->tag != 19)goto _LL169;
else{_tmp36E=(void*)_tmp36D->f1;}};_tmp36F=_tmp36B.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp370=(struct Cyc_Absyn_PointerType_struct*)_tmp36F;if(_tmp370->tag != 5)goto
_LL169;else{_tmp371=_tmp370->f1;_tmp372=_tmp371.ptr_atts;_tmp373=_tmp372.bounds;}};
_LL168:{void*_tmp374=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp373);struct Cyc_Absyn_Exp*_tmp376;_LL16C: {struct Cyc_Absyn_Upper_b_struct*
_tmp375=(struct Cyc_Absyn_Upper_b_struct*)_tmp374;if(_tmp375->tag != 1)goto _LL16E;
else{_tmp376=_tmp375->f1;}}_LL16D: {struct Cyc_Absyn_Exp*_tmp377=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp36E,0),0,Cyc_Absyn_No_coercion,0);
if(Cyc_Evexp_lte_const_exp(_tmp377,_tmp376))return 1;goto _LL16B;}_LL16E:;_LL16F:
goto _LL16B;_LL16B:;}goto _LL166;_LL169:;_LL16A: goto _LL166;_LL166:;}{struct Cyc_List_List*
_tmp378=relns;for(0;_tmp378 != 0;_tmp378=_tmp378->tl){struct Cyc_CfFlowInfo_Reln*
_tmp379=(struct Cyc_CfFlowInfo_Reln*)_tmp378->hd;if(_tmp379->vd == _tmp366){union
Cyc_CfFlowInfo_RelnOp _tmp37A=_tmp379->rop;struct Cyc_Absyn_Vardecl*_tmp37B;struct
Cyc_Absyn_Vardecl*_tmp37C;unsigned int _tmp37D;unsigned int _tmp37E;struct _tuple12
_tmp37F;struct Cyc_Absyn_Vardecl*_tmp380;_LL171: if((_tmp37A.LessEqNumelts).tag != 
5)goto _LL173;_tmp37B=(struct Cyc_Absyn_Vardecl*)(_tmp37A.LessEqNumelts).val;
_LL172: _tmp37C=_tmp37B;goto _LL174;_LL173: if((_tmp37A.LessNumelts).tag != 3)goto
_LL175;_tmp37C=(struct Cyc_Absyn_Vardecl*)(_tmp37A.LessNumelts).val;_LL174: if(x == 
_tmp37C)return 1;goto _LL170;_LL175: if((_tmp37A.EqualConst).tag != 1)goto _LL177;
_tmp37D=(unsigned int)(_tmp37A.EqualConst).val;_LL176: return Cyc_Toc_check_const_array(
_tmp37D,_tmp335);_LL177: if((_tmp37A.LessEqConst).tag != 6)goto _LL179;_tmp37E=(
unsigned int)(_tmp37A.LessEqConst).val;if(!(_tmp37E > 0))goto _LL179;_LL178: return
Cyc_Toc_check_const_array(_tmp37E,_tmp335);_LL179: if((_tmp37A.LessVar).tag != 2)
goto _LL17B;_tmp37F=(struct _tuple12)(_tmp37A.LessVar).val;_tmp380=_tmp37F.f1;
_LL17A: if(Cyc_Toc_check_leq_size_var(relns,x,_tmp380))return 1;goto _LL170;_LL17B:;
_LL17C: goto _LL170;_LL170:;}}}goto _LL15B;_LL160: if((_tmp363.LessConst).tag != 4)
goto _LL162;_tmp368=(unsigned int)(_tmp363.LessConst).val;_LL161: return Cyc_Toc_check_const_array(
_tmp368,_tmp335);_LL162: if((_tmp363.LessEqConst).tag != 6)goto _LL164;_tmp369=(
unsigned int)(_tmp363.LessEqConst).val;_LL163: return Cyc_Toc_check_const_array(
_tmp369 + 1,_tmp335);_LL164:;_LL165: goto _LL15B;_LL15B:;}}}goto _LL146;_LL159:;
_LL15A: goto _LL146;_LL146:;}return 0;};}static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*
e){if(e->topt == 0){const char*_tmpB5E;void*_tmpB5D;(_tmpB5D=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB5E="Missing type in primop ",
_tag_dyneither(_tmpB5E,sizeof(char),24))),_tag_dyneither(_tmpB5D,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*_tmpB61;void*_tmpB60;(_tmpB60=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB61="Missing type in primop ",_tag_dyneither(_tmpB61,sizeof(char),24))),
_tag_dyneither(_tmpB60,sizeof(void*),0)));}return(void*)_check_null(e->topt);}
static struct _tuple10*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){struct _tuple10*
_tmpB62;return(_tmpB62=_cycalloc(sizeof(*_tmpB62)),((_tmpB62->f1=Cyc_Toc_mt_tq,((
_tmpB62->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpB62)))));}static
struct _tuple16*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{struct _tuple16*_tmpB63;return(_tmpB63=_cycalloc(sizeof(*
_tmpB63)),((_tmpB63->f1=0,((_tmpB63->f2=e,_tmpB63)))));};}static struct Cyc_Absyn_Exp*
Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct
Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*
eo;void*t;if(pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp387=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0  || Cyc_Absyn_no_regions)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(
_tmp387): Cyc_Toc_malloc_ptr(_tmp387));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp387);}};}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp388=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp388 != 0;_tmp388=_tmp388->tl){struct _tuple16 _tmp38A;struct Cyc_List_List*
_tmp38B;struct Cyc_Absyn_Exp*_tmp38C;struct _tuple16*_tmp389=(struct _tuple16*)
_tmp388->hd;_tmp38A=*_tmp389;_tmp38B=_tmp38A.f1;_tmp38C=_tmp38A.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp38B == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp38B->tl != 0){const char*_tmpB66;void*_tmpB65;(_tmpB65=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB66="multiple designators in array",
_tag_dyneither(_tmpB66,sizeof(char),30))),_tag_dyneither(_tmpB65,sizeof(void*),0)));}{
void*_tmp38F=(void*)_tmp38B->hd;void*_tmp390=_tmp38F;struct Cyc_Absyn_Exp*_tmp392;
_LL17E: {struct Cyc_Absyn_ArrayElement_struct*_tmp391=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp390;if(_tmp391->tag != 0)goto _LL180;else{_tmp392=_tmp391->f1;}}_LL17F: Cyc_Toc_exp_to_c(
nv,_tmp392);e_index=_tmp392;goto _LL17D;_LL180: {struct Cyc_Absyn_FieldName_struct*
_tmp393=(struct Cyc_Absyn_FieldName_struct*)_tmp390;if(_tmp393->tag != 1)goto
_LL17D;}_LL181: {const char*_tmpB69;void*_tmpB68;(_tmpB68=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB69="field name designators in array",
_tag_dyneither(_tmpB69,sizeof(char),32))),_tag_dyneither(_tmpB68,sizeof(void*),0)));}
_LL17D:;};}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*
_tmp396=_tmp38C->r;struct Cyc_List_List*_tmp398;struct Cyc_Absyn_Vardecl*_tmp39A;
struct Cyc_Absyn_Exp*_tmp39B;struct Cyc_Absyn_Exp*_tmp39C;int _tmp39D;void*_tmp39F;
struct Cyc_List_List*_tmp3A0;_LL183: {struct Cyc_Absyn_Array_e_struct*_tmp397=(
struct Cyc_Absyn_Array_e_struct*)_tmp396;if(_tmp397->tag != 25)goto _LL185;else{
_tmp398=_tmp397->f1;}}_LL184: s=Cyc_Toc_init_array(nv,lval,_tmp398,s);goto _LL182;
_LL185: {struct Cyc_Absyn_Comprehension_e_struct*_tmp399=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp396;if(_tmp399->tag != 26)goto _LL187;else{_tmp39A=_tmp399->f1;_tmp39B=_tmp399->f2;
_tmp39C=_tmp399->f3;_tmp39D=_tmp399->f4;}}_LL186: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp39A,_tmp39B,_tmp39C,_tmp39D,s,0);goto _LL182;_LL187: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp39E=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp396;if(_tmp39E->tag != 28)goto
_LL189;else{_tmp39F=(void*)_tmp39E->f1;_tmp3A0=_tmp39E->f2;}}_LL188: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp39F,_tmp3A0,s);goto _LL182;_LL189:;_LL18A: Cyc_Toc_exp_to_c(nv,_tmp38C);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
_tmp38C,0),s,0);goto _LL182;_LL182:;};};}}return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple0*_tmp3A1=vd->name;void*_tmp3A2=Cyc_Toc_typ_to_c((
void*)_check_null(e2->topt));if(!e1_already_translated)Cyc_Toc_exp_to_c(nv,e1);{
struct _RegionHandle _tmp3A3=_new_region("r2");struct _RegionHandle*r2=& _tmp3A3;
_push_region(r2);{struct Cyc_Absyn_Local_b_struct _tmpB6C;struct Cyc_Absyn_Local_b_struct*
_tmpB6B;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp3A1,Cyc_Absyn_varb_exp(
_tmp3A1,(void*)((_tmpB6B=_cycalloc(sizeof(*_tmpB6B)),((_tmpB6B[0]=((_tmpB6C.tag=
4,((_tmpB6C.f1=vd,_tmpB6C)))),_tmpB6B)))),0));struct _tuple0*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp3A1,0),Cyc_Absyn_signed_int_exp(
0,0),0);struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp3A1,0),Cyc_Absyn_var_exp(
max,0),0);struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(
_tmp3A1,0),0);struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(
_tmp3A1,0),0);struct Cyc_Absyn_Stmt*body;{void*_tmp3A4=e2->r;struct Cyc_List_List*
_tmp3A6;struct Cyc_Absyn_Vardecl*_tmp3A8;struct Cyc_Absyn_Exp*_tmp3A9;struct Cyc_Absyn_Exp*
_tmp3AA;int _tmp3AB;void*_tmp3AD;struct Cyc_List_List*_tmp3AE;_LL18C: {struct Cyc_Absyn_Array_e_struct*
_tmp3A5=(struct Cyc_Absyn_Array_e_struct*)_tmp3A4;if(_tmp3A5->tag != 25)goto _LL18E;
else{_tmp3A6=_tmp3A5->f1;}}_LL18D: body=Cyc_Toc_init_array(nv2,lval,_tmp3A6,Cyc_Toc_skip_stmt_dl());
goto _LL18B;_LL18E: {struct Cyc_Absyn_Comprehension_e_struct*_tmp3A7=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3A4;if(_tmp3A7->tag != 26)goto _LL190;else{_tmp3A8=_tmp3A7->f1;_tmp3A9=_tmp3A7->f2;
_tmp3AA=_tmp3A7->f3;_tmp3AB=_tmp3A7->f4;}}_LL18F: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp3A8,_tmp3A9,_tmp3AA,_tmp3AB,Cyc_Toc_skip_stmt_dl(),0);goto _LL18B;
_LL190: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp3AC=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp3A4;if(_tmp3AC->tag != 28)goto _LL192;else{_tmp3AD=(void*)_tmp3AC->f1;_tmp3AE=
_tmp3AC->f2;}}_LL191: body=Cyc_Toc_init_anon_struct(nv,lval,_tmp3AD,_tmp3AE,Cyc_Toc_skip_stmt_dl());
goto _LL18B;_LL192:;_LL193: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL18B;_LL18B:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp3A2,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
struct Cyc_Absyn_Stmt*_tmp3AF=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp3A1,Cyc_Absyn_uint_typ,0,s2,0),
0),s,0);_npop_handler(0);return _tmp3AF;};};};_pop_region(r2);};}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*
struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){{struct Cyc_List_List*
_tmp3B2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);
for(0;_tmp3B2 != 0;_tmp3B2=_tmp3B2->tl){struct _tuple16 _tmp3B4;struct Cyc_List_List*
_tmp3B5;struct Cyc_Absyn_Exp*_tmp3B6;struct _tuple16*_tmp3B3=(struct _tuple16*)
_tmp3B2->hd;_tmp3B4=*_tmp3B3;_tmp3B5=_tmp3B4.f1;_tmp3B6=_tmp3B4.f2;if(_tmp3B5 == 
0){const char*_tmpB6F;void*_tmpB6E;(_tmpB6E=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6F="empty designator list",
_tag_dyneither(_tmpB6F,sizeof(char),22))),_tag_dyneither(_tmpB6E,sizeof(void*),0)));}
if(_tmp3B5->tl != 0){const char*_tmpB72;void*_tmpB71;(_tmpB71=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB72="too many designators in anonymous struct",
_tag_dyneither(_tmpB72,sizeof(char),41))),_tag_dyneither(_tmpB71,sizeof(void*),0)));}{
void*_tmp3BB=(void*)_tmp3B5->hd;struct _dyneither_ptr*_tmp3BD;_LL195: {struct Cyc_Absyn_FieldName_struct*
_tmp3BC=(struct Cyc_Absyn_FieldName_struct*)_tmp3BB;if(_tmp3BC->tag != 1)goto
_LL197;else{_tmp3BD=_tmp3BC->f1;}}_LL196: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(
lhs,_tmp3BD,0);{void*_tmp3BE=_tmp3B6->r;struct Cyc_List_List*_tmp3C0;struct Cyc_Absyn_Vardecl*
_tmp3C2;struct Cyc_Absyn_Exp*_tmp3C3;struct Cyc_Absyn_Exp*_tmp3C4;int _tmp3C5;void*
_tmp3C7;struct Cyc_List_List*_tmp3C8;_LL19A: {struct Cyc_Absyn_Array_e_struct*
_tmp3BF=(struct Cyc_Absyn_Array_e_struct*)_tmp3BE;if(_tmp3BF->tag != 25)goto _LL19C;
else{_tmp3C0=_tmp3BF->f1;}}_LL19B: s=Cyc_Toc_init_array(nv,lval,_tmp3C0,s);goto
_LL199;_LL19C: {struct Cyc_Absyn_Comprehension_e_struct*_tmp3C1=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3BE;if(_tmp3C1->tag != 26)goto _LL19E;else{_tmp3C2=_tmp3C1->f1;_tmp3C3=_tmp3C1->f2;
_tmp3C4=_tmp3C1->f3;_tmp3C5=_tmp3C1->f4;}}_LL19D: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp3C2,_tmp3C3,_tmp3C4,_tmp3C5,s,0);goto _LL199;_LL19E: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3C6=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3BE;if(_tmp3C6->tag != 28)goto
_LL1A0;else{_tmp3C7=(void*)_tmp3C6->f1;_tmp3C8=_tmp3C6->f2;}}_LL19F: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3C7,_tmp3C8,s);goto _LL199;_LL1A0:;_LL1A1: Cyc_Toc_exp_to_c(nv,_tmp3B6);
if(Cyc_Toc_is_poly_field(struct_type,_tmp3BD))_tmp3B6=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp3B6);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp3B6,0),0),s,0);goto _LL199;_LL199:;}goto _LL194;}_LL197:;_LL198: {const char*
_tmpB75;void*_tmpB74;(_tmpB74=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB75="array designator in struct",
_tag_dyneither(_tmpB75,sizeof(char),27))),_tag_dyneither(_tmpB74,sizeof(void*),0)));}
_LL194:;};}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct
_RegionHandle _tmp3CB=_new_region("r");struct _RegionHandle*r=& _tmp3CB;
_push_region(r);{struct Cyc_List_List*_tmp3CC=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);void*_tmp3CD=Cyc_Toc_add_tuple_type(
_tmp3CC);struct _tuple0*_tmp3CE=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp3CF=
Cyc_Absyn_var_exp(_tmp3CE,0);struct Cyc_Absyn_Stmt*_tmp3D0=Cyc_Absyn_exp_stmt(
_tmp3CF,0);struct Cyc_Absyn_Exp*(*_tmp3D1)(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp3D2=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp3D2);for(0;_tmp3D2 != 0;(_tmp3D2=_tmp3D2->tl,
-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp3D2->hd;struct Cyc_Absyn_Exp*
lval=_tmp3D1(_tmp3CF,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic  && Cyc_Toc_atomic_typ((
void*)_check_null(e->topt));{void*_tmp3D3=e->r;struct Cyc_List_List*_tmp3D5;
struct Cyc_Absyn_Vardecl*_tmp3D7;struct Cyc_Absyn_Exp*_tmp3D8;struct Cyc_Absyn_Exp*
_tmp3D9;int _tmp3DA;_LL1A3: {struct Cyc_Absyn_Array_e_struct*_tmp3D4=(struct Cyc_Absyn_Array_e_struct*)
_tmp3D3;if(_tmp3D4->tag != 25)goto _LL1A5;else{_tmp3D5=_tmp3D4->f1;}}_LL1A4:
_tmp3D0=Cyc_Toc_init_array(nv,lval,_tmp3D5,_tmp3D0);goto _LL1A2;_LL1A5: {struct
Cyc_Absyn_Comprehension_e_struct*_tmp3D6=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3D3;if(_tmp3D6->tag != 26)goto _LL1A7;else{_tmp3D7=_tmp3D6->f1;_tmp3D8=_tmp3D6->f2;
_tmp3D9=_tmp3D6->f3;_tmp3DA=_tmp3D6->f4;}}_LL1A6: _tmp3D0=Cyc_Toc_init_comprehension(
nv,lval,_tmp3D7,_tmp3D8,_tmp3D9,_tmp3DA,_tmp3D0,0);goto _LL1A2;_LL1A7:;_LL1A8: Cyc_Toc_exp_to_c(
nv,e);_tmp3D0=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3D1(
_tmp3CF,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp3D0,0);goto _LL1A2;_LL1A2:;};}}{
struct Cyc_Absyn_Exp*_tmp3DB=Cyc_Toc_make_struct(nv,_tmp3CE,_tmp3CD,_tmp3D0,
pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp3DB;};};_pop_region(r);}
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct
_dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp3DC=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp3DC != 0;_tmp3DC=_tmp3DC->tl){struct Cyc_Absyn_Aggrfield*
_tmp3DD=(struct Cyc_Absyn_Aggrfield*)_tmp3DC->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp3DD->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}{
struct Cyc_String_pa_struct _tmpB7D;void*_tmpB7C[1];const char*_tmpB7B;void*_tmpB7A;(
_tmpB7A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)((_tmpB7D.tag=0,((_tmpB7D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmpB7C[0]=& _tmpB7D,Cyc_aprintf(((_tmpB7B="get_member_offset %s failed",
_tag_dyneither(_tmpB7B,sizeof(char),28))),_tag_dyneither(_tmpB7C,sizeof(void*),1)))))))),
_tag_dyneither(_tmpB7A,sizeof(void*),0)));};}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,
struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){struct
_tuple0*_tmp3E2=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp3E3=Cyc_Absyn_var_exp(
_tmp3E2,0);struct Cyc_Absyn_Stmt*_tmp3E4=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(
_tmp3E3),0);struct Cyc_Absyn_Exp*(*_tmp3E5)(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
void*_tmp3E6=Cyc_Toc_typ_to_c(struct_type);int is_atomic=1;struct Cyc_List_List*
forall_types;struct Cyc_Absyn_Aggrdecl*ad;struct Cyc_List_List*aggrfields;{void*
_tmp3E7=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp3E9;union
Cyc_Absyn_AggrInfoU _tmp3EA;struct Cyc_List_List*_tmp3EB;_LL1AA: {struct Cyc_Absyn_AggrType_struct*
_tmp3E8=(struct Cyc_Absyn_AggrType_struct*)_tmp3E7;if(_tmp3E8->tag != 11)goto
_LL1AC;else{_tmp3E9=_tmp3E8->f1;_tmp3EA=_tmp3E9.aggr_info;_tmp3EB=_tmp3E9.targs;}}
_LL1AB: ad=Cyc_Absyn_get_known_aggrdecl(_tmp3EA);aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;forall_types=_tmp3EB;goto _LL1A9;_LL1AC:;_LL1AD: {
const char*_tmpB80;void*_tmpB7F;(_tmpB7F=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB80="init_struct: bad struct type",
_tag_dyneither(_tmpB80,sizeof(char),29))),_tag_dyneither(_tmpB7F,sizeof(void*),0)));}
_LL1A9:;}{void*orig_typ=Cyc_Toc_typ_to_c(struct_type);void*aggr_typ=orig_typ;if(
exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields
!= 0){struct Cyc_List_List*_tmp3EE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp3EE->tl != 0;_tmp3EE=_tmp3EE->tl){;}{struct Cyc_Absyn_Aggrfield*_tmp3EF=(
struct Cyc_Absyn_Aggrfield*)_tmp3EE->hd;struct _RegionHandle _tmp3F0=_new_region("temp");
struct _RegionHandle*temp=& _tmp3F0;_push_region(temp);{void*cast_type;if(pointer)
cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);else{cast_type=orig_typ;}
_tmp3E4=Cyc_Absyn_exp_stmt(Cyc_Toc_cast_it(cast_type,Cyc_Absyn_copy_exp(_tmp3E3)),
0);{struct Cyc_List_List*_tmp3F1=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
temp,temp,ad->tvs,forall_types);if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(
temp,_tmp3F1,_tmp3EF->type))){struct Cyc_List_List*_tmp3F2=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,
exist_types);struct Cyc_List_List*_tmp3F3=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
temp,_tmp3F1,_tmp3F2);struct Cyc_List_List*new_fields=0;for(_tmp3EE=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;_tmp3EE != 0;_tmp3EE=_tmp3EE->tl){struct Cyc_Absyn_Aggrfield*
_tmp3F4=(struct Cyc_Absyn_Aggrfield*)_tmp3EE->hd;struct Cyc_Absyn_Aggrfield*
_tmpB81;struct Cyc_Absyn_Aggrfield*_tmp3F5=(_tmpB81=_cycalloc(sizeof(*_tmpB81)),((
_tmpB81->name=_tmp3F4->name,((_tmpB81->tq=Cyc_Toc_mt_tq,((_tmpB81->type=Cyc_Tcutil_rsubstitute(
temp,_tmp3F3,_tmp3F4->type),((_tmpB81->width=_tmp3F4->width,((_tmpB81->attributes=
_tmp3F4->attributes,_tmpB81)))))))))));struct Cyc_List_List*_tmpB82;new_fields=((
_tmpB82=_cycalloc(sizeof(*_tmpB82)),((_tmpB82->hd=_tmp3F5,((_tmpB82->tl=
new_fields,_tmpB82))))));}exist_types=0;aggrfields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{struct Cyc_Absyn_AnonAggrType_struct
_tmpB85;struct Cyc_Absyn_AnonAggrType_struct*_tmpB84;struct_type=(void*)((_tmpB84=
_cycalloc(sizeof(*_tmpB84)),((_tmpB84[0]=((_tmpB85.tag=12,((_tmpB85.f1=ad->kind,((
_tmpB85.f2=aggrfields,_tmpB85)))))),_tmpB84))));}aggr_typ=Cyc_Toc_typ_to_c(
struct_type);}};};_pop_region(temp);};}{int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->tagged;struct _RegionHandle _tmp3FA=_new_region("r");
struct _RegionHandle*r=& _tmp3FA;_push_region(r);{struct Cyc_List_List*_tmp3FB=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(
r,dles);for(0;_tmp3FB != 0;_tmp3FB=_tmp3FB->tl){struct _tuple16 _tmp3FD;struct Cyc_List_List*
_tmp3FE;struct Cyc_Absyn_Exp*_tmp3FF;struct _tuple16*_tmp3FC=(struct _tuple16*)
_tmp3FB->hd;_tmp3FD=*_tmp3FC;_tmp3FE=_tmp3FD.f1;_tmp3FF=_tmp3FD.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp3FF->topt));if(_tmp3FE == 
0){const char*_tmpB88;void*_tmpB87;(_tmpB87=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB88="empty designator list",
_tag_dyneither(_tmpB88,sizeof(char),22))),_tag_dyneither(_tmpB87,sizeof(void*),0)));}
if(_tmp3FE->tl != 0){struct _tuple0*_tmp402=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp403=Cyc_Absyn_var_exp(_tmp402,0);for(0;_tmp3FE != 0;_tmp3FE=_tmp3FE->tl){void*
_tmp404=(void*)_tmp3FE->hd;struct _dyneither_ptr*_tmp406;_LL1AF: {struct Cyc_Absyn_FieldName_struct*
_tmp405=(struct Cyc_Absyn_FieldName_struct*)_tmp404;if(_tmp405->tag != 1)goto
_LL1B1;else{_tmp406=_tmp405->f1;}}_LL1B0: if(Cyc_Toc_is_poly_field(struct_type,
_tmp406))_tmp403=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp403);_tmp3E4=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3E5(_tmp3E3,_tmp406,0),_tmp403,0),0),
_tmp3E4,0);goto _LL1AE;_LL1B1:;_LL1B2: {const char*_tmpB8B;void*_tmpB8A;(_tmpB8A=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB8B="array designator in struct",_tag_dyneither(_tmpB8B,sizeof(char),27))),
_tag_dyneither(_tmpB8A,sizeof(void*),0)));}_LL1AE:;}Cyc_Toc_exp_to_c(nv,_tmp3FF);
_tmp3E4=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp403,
_tmp3FF,0),0),_tmp3E4,0);}else{void*_tmp409=(void*)_tmp3FE->hd;struct
_dyneither_ptr*_tmp40B;_LL1B4: {struct Cyc_Absyn_FieldName_struct*_tmp40A=(struct
Cyc_Absyn_FieldName_struct*)_tmp409;if(_tmp40A->tag != 1)goto _LL1B6;else{_tmp40B=
_tmp40A->f1;}}_LL1B5: {struct Cyc_Absyn_Exp*lval=_tmp3E5(_tmp3E3,_tmp40B,0);if(
is_tagged_union){int i=Cyc_Toc_get_member_offset(ad,_tmp40B);struct Cyc_Absyn_Exp*
f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_aggrmember_exp(
lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,
f_tag_exp,0);_tmp3E4=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3E4,0);
lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{void*_tmp40C=_tmp3FF->r;
struct Cyc_List_List*_tmp40E;struct Cyc_Absyn_Vardecl*_tmp410;struct Cyc_Absyn_Exp*
_tmp411;struct Cyc_Absyn_Exp*_tmp412;int _tmp413;void*_tmp415;struct Cyc_List_List*
_tmp416;_LL1B9: {struct Cyc_Absyn_Array_e_struct*_tmp40D=(struct Cyc_Absyn_Array_e_struct*)
_tmp40C;if(_tmp40D->tag != 25)goto _LL1BB;else{_tmp40E=_tmp40D->f1;}}_LL1BA:
_tmp3E4=Cyc_Toc_init_array(nv,lval,_tmp40E,_tmp3E4);goto _LL1B8;_LL1BB: {struct
Cyc_Absyn_Comprehension_e_struct*_tmp40F=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp40C;if(_tmp40F->tag != 26)goto _LL1BD;else{_tmp410=_tmp40F->f1;_tmp411=_tmp40F->f2;
_tmp412=_tmp40F->f3;_tmp413=_tmp40F->f4;}}_LL1BC: _tmp3E4=Cyc_Toc_init_comprehension(
nv,lval,_tmp410,_tmp411,_tmp412,_tmp413,_tmp3E4,0);goto _LL1B8;_LL1BD: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp414=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp40C;if(_tmp414->tag != 28)goto
_LL1BF;else{_tmp415=(void*)_tmp414->f1;_tmp416=_tmp414->f2;}}_LL1BE: _tmp3E4=Cyc_Toc_init_anon_struct(
nv,lval,_tmp415,_tmp416,_tmp3E4);goto _LL1B8;_LL1BF:;_LL1C0: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)_check_null(_tmp3FF->topt));Cyc_Toc_exp_to_c(nv,_tmp3FF);{struct Cyc_Absyn_Aggrfield*
_tmp417=Cyc_Absyn_lookup_field(aggrfields,_tmp40B);if(Cyc_Toc_is_poly_field(
struct_type,_tmp40B) && !was_ptr_type)_tmp3FF=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp3FF);if(exist_types != 0)_tmp3FF=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)
_check_null(_tmp417))->type),_tmp3FF);_tmp3E4=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(lval,_tmp3FF,0),0),_tmp3E4,0);goto _LL1B8;};}_LL1B8:;}goto
_LL1B3;}_LL1B6:;_LL1B7: {const char*_tmpB8E;void*_tmpB8D;(_tmpB8D=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8E="array designator in struct",
_tag_dyneither(_tmpB8E,sizeof(char),27))),_tag_dyneither(_tmpB8D,sizeof(void*),0)));}
_LL1B3:;}}}{struct Cyc_Absyn_Exp*_tmp41A=Cyc_Toc_make_struct(nv,_tmp3E2,aggr_typ,
_tmp3E4,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp41A;};;_pop_region(
r);};};}struct _tuple20{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};static
struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct
_tuple20*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(
e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,
enum Cyc_Absyn_Incrementor incr){struct Cyc_Absyn_Increment_e_struct _tmpB91;struct
Cyc_Absyn_Increment_e_struct*_tmpB90;return Cyc_Absyn_new_exp((void*)((_tmpB90=
_cycalloc(sizeof(*_tmpB90)),((_tmpB90[0]=((_tmpB91.tag=4,((_tmpB91.f1=e1,((
_tmpB91.f2=incr,_tmpB91)))))),_tmpB90)))),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp41D=e1->r;struct Cyc_Absyn_Stmt*_tmp41F;void*_tmp421;struct Cyc_Absyn_Exp*
_tmp422;struct Cyc_Absyn_Exp*_tmp424;struct _dyneither_ptr*_tmp425;int _tmp426;int
_tmp427;_LL1C2: {struct Cyc_Absyn_StmtExp_e_struct*_tmp41E=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp41D;if(_tmp41E->tag != 35)goto _LL1C4;else{_tmp41F=_tmp41E->f1;}}_LL1C3: Cyc_Toc_lvalue_assign_stmt(
_tmp41F,fs,f,f_env);goto _LL1C1;_LL1C4: {struct Cyc_Absyn_Cast_e_struct*_tmp420=(
struct Cyc_Absyn_Cast_e_struct*)_tmp41D;if(_tmp420->tag != 13)goto _LL1C6;else{
_tmp421=(void*)_tmp420->f1;_tmp422=_tmp420->f2;}}_LL1C5: Cyc_Toc_lvalue_assign(
_tmp422,fs,f,f_env);goto _LL1C1;_LL1C6: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp423=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp41D;if(_tmp423->tag != 20)goto
_LL1C8;else{_tmp424=_tmp423->f1;_tmp425=_tmp423->f2;_tmp426=_tmp423->f3;_tmp427=
_tmp423->f4;}}_LL1C7: e1->r=_tmp424->r;{struct Cyc_List_List*_tmpB92;Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)((_tmpB92=_cycalloc(sizeof(*_tmpB92)),((_tmpB92->hd=
_tmp425,((_tmpB92->tl=fs,_tmpB92)))))),f,f_env);}goto _LL1C1;_LL1C8:;_LL1C9: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;goto _LL1C1;}_LL1C1:;}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env){void*_tmp429=s->r;struct Cyc_Absyn_Exp*_tmp42B;struct Cyc_Absyn_Decl*
_tmp42D;struct Cyc_Absyn_Stmt*_tmp42E;struct Cyc_Absyn_Stmt*_tmp430;_LL1CB: {
struct Cyc_Absyn_Exp_s_struct*_tmp42A=(struct Cyc_Absyn_Exp_s_struct*)_tmp429;if(
_tmp42A->tag != 1)goto _LL1CD;else{_tmp42B=_tmp42A->f1;}}_LL1CC: Cyc_Toc_lvalue_assign(
_tmp42B,fs,f,f_env);goto _LL1CA;_LL1CD: {struct Cyc_Absyn_Decl_s_struct*_tmp42C=(
struct Cyc_Absyn_Decl_s_struct*)_tmp429;if(_tmp42C->tag != 12)goto _LL1CF;else{
_tmp42D=_tmp42C->f1;_tmp42E=_tmp42C->f2;}}_LL1CE: Cyc_Toc_lvalue_assign_stmt(
_tmp42E,fs,f,f_env);goto _LL1CA;_LL1CF: {struct Cyc_Absyn_Seq_s_struct*_tmp42F=(
struct Cyc_Absyn_Seq_s_struct*)_tmp429;if(_tmp42F->tag != 2)goto _LL1D1;else{
_tmp430=_tmp42F->f2;}}_LL1D0: Cyc_Toc_lvalue_assign_stmt(_tmp430,fs,f,f_env);goto
_LL1CA;_LL1D1:;_LL1D2: {const char*_tmpB96;void*_tmpB95[1];struct Cyc_String_pa_struct
_tmpB94;(_tmpB94.tag=0,((_tmpB94.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmpB95[0]=& _tmpB94,Cyc_Toc_toc_impos(((_tmpB96="lvalue_assign_stmt: %s",
_tag_dyneither(_tmpB96,sizeof(char),23))),_tag_dyneither(_tmpB95,sizeof(void*),1)))))));}
_LL1CA:;}static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);static
struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){void*_tmp434=
e->r;void*_tmp436;void**_tmp437;struct Cyc_Absyn_Exp*_tmp438;struct Cyc_Absyn_Exp**
_tmp439;struct Cyc_Absyn_Exp*_tmp43B;struct Cyc_Absyn_Stmt*_tmp43D;_LL1D4: {struct
Cyc_Absyn_Cast_e_struct*_tmp435=(struct Cyc_Absyn_Cast_e_struct*)_tmp434;if(
_tmp435->tag != 13)goto _LL1D6;else{_tmp436=(void**)& _tmp435->f1;_tmp437=(void**)((
void**)& _tmp435->f1);_tmp438=_tmp435->f2;_tmp439=(struct Cyc_Absyn_Exp**)& _tmp435->f2;}}
_LL1D5:*_tmp439=Cyc_Toc_push_address_exp(*_tmp439);*_tmp437=Cyc_Absyn_cstar_typ(*
_tmp437,Cyc_Toc_mt_tq);return e;_LL1D6: {struct Cyc_Absyn_Deref_e_struct*_tmp43A=(
struct Cyc_Absyn_Deref_e_struct*)_tmp434;if(_tmp43A->tag != 19)goto _LL1D8;else{
_tmp43B=_tmp43A->f1;}}_LL1D7: return _tmp43B;_LL1D8: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp43C=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp434;if(_tmp43C->tag != 35)goto
_LL1DA;else{_tmp43D=_tmp43C->f1;}}_LL1D9: Cyc_Toc_push_address_stmt(_tmp43D);
return e;_LL1DA:;_LL1DB: if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpB9A;void*_tmpB99[1];struct Cyc_String_pa_struct _tmpB98;(_tmpB98.tag=
0,((_tmpB98.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpB99[0]=& _tmpB98,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB9A="can't take & of exp %s",_tag_dyneither(_tmpB9A,
sizeof(char),23))),_tag_dyneither(_tmpB99,sizeof(void*),1)))))));};_LL1D3:;}
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){void*_tmp441=s->r;
struct Cyc_Absyn_Stmt*_tmp443;struct Cyc_Absyn_Stmt*_tmp445;struct Cyc_Absyn_Exp*
_tmp447;struct Cyc_Absyn_Exp**_tmp448;_LL1DD: {struct Cyc_Absyn_Seq_s_struct*
_tmp442=(struct Cyc_Absyn_Seq_s_struct*)_tmp441;if(_tmp442->tag != 2)goto _LL1DF;
else{_tmp443=_tmp442->f2;}}_LL1DE: _tmp445=_tmp443;goto _LL1E0;_LL1DF: {struct Cyc_Absyn_Decl_s_struct*
_tmp444=(struct Cyc_Absyn_Decl_s_struct*)_tmp441;if(_tmp444->tag != 12)goto _LL1E1;
else{_tmp445=_tmp444->f2;}}_LL1E0: Cyc_Toc_push_address_stmt(_tmp445);goto _LL1DC;
_LL1E1: {struct Cyc_Absyn_Exp_s_struct*_tmp446=(struct Cyc_Absyn_Exp_s_struct*)
_tmp441;if(_tmp446->tag != 1)goto _LL1E3;else{_tmp447=_tmp446->f1;_tmp448=(struct
Cyc_Absyn_Exp**)& _tmp446->f1;}}_LL1E2:*_tmp448=Cyc_Toc_push_address_exp(*_tmp448);
goto _LL1DC;_LL1E3:;_LL1E4: {const char*_tmpB9E;void*_tmpB9D[1];struct Cyc_String_pa_struct
_tmpB9C;(_tmpB9C.tag=0,((_tmpB9C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmpB9D[0]=& _tmpB9C,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB9E="can't take & of stmt %s",
_tag_dyneither(_tmpB9E,sizeof(char),24))),_tag_dyneither(_tmpB9D,sizeof(void*),1)))))));}
_LL1DC:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;
struct Cyc_List_List*prev;if(x == 0)return 0;{struct Cyc_List_List*_tmpB9F;result=((
_tmpB9F=_region_malloc(r2,sizeof(*_tmpB9F)),((_tmpB9F->hd=(void*)f((void*)x->hd,
env),((_tmpB9F->tl=0,_tmpB9F))))));}prev=result;for(x=x->tl;x != 0;x=x->tl){{
struct Cyc_List_List*_tmpBA0;((struct Cyc_List_List*)_check_null(prev))->tl=((
_tmpBA0=_region_malloc(r2,sizeof(*_tmpBA0)),((_tmpBA0->hd=(void*)f((void*)x->hd,
env),((_tmpBA0->tl=0,_tmpBA0))))));}prev=prev->tl;}return result;}static struct Cyc_List_List*
Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region,f,env,x);}static struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*
e){struct _tuple16*_tmpBA1;return(_tmpBA1=_cycalloc(sizeof(*_tmpBA1)),((_tmpBA1->f1=
0,((_tmpBA1->f2=e,_tmpBA1)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(
void*t){void*_tmp44F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp451;
_LL1E6: {struct Cyc_Absyn_PointerType_struct*_tmp450=(struct Cyc_Absyn_PointerType_struct*)
_tmp44F;if(_tmp450->tag != 5)goto _LL1E8;else{_tmp451=_tmp450->f1;}}_LL1E7: return
_tmp451;_LL1E8:;_LL1E9: {const char*_tmpBA4;void*_tmpBA3;(_tmpBA3=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA4="get_ptr_typ: not a pointer!",
_tag_dyneither(_tmpBA4,sizeof(char),28))),_tag_dyneither(_tmpBA3,sizeof(void*),0)));}
_LL1E5:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp454=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp457;enum Cyc_Absyn_Size_of
_tmp458;enum Cyc_Absyn_Sign _tmp45A;enum Cyc_Absyn_Size_of _tmp45B;enum Cyc_Absyn_Sign
_tmp45F;enum Cyc_Absyn_Size_of _tmp460;enum Cyc_Absyn_Sign _tmp462;enum Cyc_Absyn_Size_of
_tmp463;enum Cyc_Absyn_Sign _tmp465;enum Cyc_Absyn_Size_of _tmp466;int _tmp468;int
_tmp46A;int _tmp46C;_LL1EB: {struct Cyc_Absyn_PointerType_struct*_tmp455=(struct
Cyc_Absyn_PointerType_struct*)_tmp454;if(_tmp455->tag != 5)goto _LL1ED;}_LL1EC: res=
Cyc_Absyn_null_exp(0);goto _LL1EA;_LL1ED: {struct Cyc_Absyn_IntType_struct*_tmp456=(
struct Cyc_Absyn_IntType_struct*)_tmp454;if(_tmp456->tag != 6)goto _LL1EF;else{
_tmp457=_tmp456->f1;_tmp458=_tmp456->f2;if(_tmp458 != Cyc_Absyn_Char_sz)goto
_LL1EF;}}_LL1EE: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp457,'\000'),0);goto
_LL1EA;_LL1EF: {struct Cyc_Absyn_IntType_struct*_tmp459=(struct Cyc_Absyn_IntType_struct*)
_tmp454;if(_tmp459->tag != 6)goto _LL1F1;else{_tmp45A=_tmp459->f1;_tmp45B=_tmp459->f2;
if(_tmp45B != Cyc_Absyn_Short_sz)goto _LL1F1;}}_LL1F0: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(
_tmp45A,0),0);goto _LL1EA;_LL1F1: {struct Cyc_Absyn_EnumType_struct*_tmp45C=(
struct Cyc_Absyn_EnumType_struct*)_tmp454;if(_tmp45C->tag != 13)goto _LL1F3;}_LL1F2:
goto _LL1F4;_LL1F3: {struct Cyc_Absyn_AnonEnumType_struct*_tmp45D=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp454;if(_tmp45D->tag != 14)goto _LL1F5;}_LL1F4: _tmp45F=Cyc_Absyn_Unsigned;goto
_LL1F6;_LL1F5: {struct Cyc_Absyn_IntType_struct*_tmp45E=(struct Cyc_Absyn_IntType_struct*)
_tmp454;if(_tmp45E->tag != 6)goto _LL1F7;else{_tmp45F=_tmp45E->f1;_tmp460=_tmp45E->f2;
if(_tmp460 != Cyc_Absyn_Int_sz)goto _LL1F7;}}_LL1F6: _tmp462=_tmp45F;goto _LL1F8;
_LL1F7: {struct Cyc_Absyn_IntType_struct*_tmp461=(struct Cyc_Absyn_IntType_struct*)
_tmp454;if(_tmp461->tag != 6)goto _LL1F9;else{_tmp462=_tmp461->f1;_tmp463=_tmp461->f2;
if(_tmp463 != Cyc_Absyn_Long_sz)goto _LL1F9;}}_LL1F8: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(
_tmp462,0),0);goto _LL1EA;_LL1F9: {struct Cyc_Absyn_IntType_struct*_tmp464=(struct
Cyc_Absyn_IntType_struct*)_tmp454;if(_tmp464->tag != 6)goto _LL1FB;else{_tmp465=
_tmp464->f1;_tmp466=_tmp464->f2;if(_tmp466 != Cyc_Absyn_LongLong_sz)goto _LL1FB;}}
_LL1FA: res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp465,(long long)0),0);
goto _LL1EA;_LL1FB: {struct Cyc_Absyn_FloatType_struct*_tmp467=(struct Cyc_Absyn_FloatType_struct*)
_tmp454;if(_tmp467->tag != 7)goto _LL1FD;else{_tmp468=_tmp467->f1;if(_tmp468 != 0)
goto _LL1FD;}}_LL1FC:{const char*_tmpBA5;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((
_tmpBA5="0.0F",_tag_dyneither(_tmpBA5,sizeof(char),5))),0),0);}goto _LL1EA;_LL1FD: {
struct Cyc_Absyn_FloatType_struct*_tmp469=(struct Cyc_Absyn_FloatType_struct*)
_tmp454;if(_tmp469->tag != 7)goto _LL1FF;else{_tmp46A=_tmp469->f1;if(_tmp46A != 1)
goto _LL1FF;}}_LL1FE:{const char*_tmpBA6;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((
_tmpBA6="0.0",_tag_dyneither(_tmpBA6,sizeof(char),4))),1),0);}goto _LL1EA;_LL1FF: {
struct Cyc_Absyn_FloatType_struct*_tmp46B=(struct Cyc_Absyn_FloatType_struct*)
_tmp454;if(_tmp46B->tag != 7)goto _LL201;else{_tmp46C=_tmp46B->f1;}}_LL200:{const
char*_tmpBA7;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBA7="0.0L",
_tag_dyneither(_tmpBA7,sizeof(char),5))),_tmp46C),0);}goto _LL1EA;_LL201:;_LL202: {
const char*_tmpBAB;void*_tmpBAA[1];struct Cyc_String_pa_struct _tmpBA9;(_tmpBA9.tag=
0,((_tmpBA9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmpBAA[0]=& _tmpBA9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpBAB="found non-zero type %s in generate_zero",
_tag_dyneither(_tmpBAB,sizeof(char),40))),_tag_dyneither(_tmpBAA,sizeof(void*),1)))))));}
_LL1EA:;}res->topt=(void*)t;return res;}static void Cyc_Toc_zero_ptr_assign_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){void*
fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,
Cyc_Absyn_true_conref);void*_tmp473=Cyc_Toc_typ_to_c(elt_type);void*_tmp474=Cyc_Toc_typ_to_c(
fat_ptr_type);void*_tmp475=Cyc_Absyn_cstar_typ(_tmp473,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmpBAC;struct Cyc_Core_Opt*_tmp476=(_tmpBAC=_cycalloc(sizeof(*_tmpBAC)),((
_tmpBAC->v=_tmp475,_tmpBAC)));struct Cyc_Absyn_Exp*xinit;{void*_tmp477=e1->r;
struct Cyc_Absyn_Exp*_tmp479;struct Cyc_Absyn_Exp*_tmp47B;struct Cyc_Absyn_Exp*
_tmp47C;_LL204: {struct Cyc_Absyn_Deref_e_struct*_tmp478=(struct Cyc_Absyn_Deref_e_struct*)
_tmp477;if(_tmp478->tag != 19)goto _LL206;else{_tmp479=_tmp478->f1;}}_LL205: if(!
is_dyneither){_tmp479=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp479,0,Cyc_Absyn_Other_coercion,
0);_tmp479->topt=(void*)fat_ptr_type;}Cyc_Toc_exp_to_c(nv,_tmp479);xinit=_tmp479;
goto _LL203;_LL206: {struct Cyc_Absyn_Subscript_e_struct*_tmp47A=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp477;if(_tmp47A->tag != 22)goto _LL208;else{_tmp47B=_tmp47A->f1;_tmp47C=_tmp47A->f2;}}
_LL207: if(!is_dyneither){_tmp47B=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp47B,0,Cyc_Absyn_Other_coercion,
0);_tmp47B->topt=(void*)fat_ptr_type;}Cyc_Toc_exp_to_c(nv,_tmp47B);Cyc_Toc_exp_to_c(
nv,_tmp47C);{struct Cyc_Absyn_Exp*_tmpBAD[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,((
_tmpBAD[2]=_tmp47C,((_tmpBAD[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmpBAD[0]=_tmp47B,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpBAD,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL203;_LL208:;_LL209: {const char*_tmpBB0;void*_tmpBAF;(_tmpBAF=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB0="found bad lhs for zero-terminated pointer assignment",
_tag_dyneither(_tmpBB0,sizeof(char),53))),_tag_dyneither(_tmpBAF,sizeof(void*),0)));}
_LL203:;}{struct _tuple0*_tmp480=Cyc_Toc_temp_var();struct _RegionHandle _tmp481=
_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp481;_push_region(rgn2);{struct
Cyc_Toc_Env*_tmp482=Cyc_Toc_add_varmap(rgn2,nv,_tmp480,Cyc_Absyn_var_exp(_tmp480,
0));struct Cyc_Absyn_Vardecl*_tmpBB1;struct Cyc_Absyn_Vardecl*_tmp483=(_tmpBB1=
_cycalloc(sizeof(*_tmpBB1)),((_tmpBB1->sc=Cyc_Absyn_Public,((_tmpBB1->name=
_tmp480,((_tmpBB1->tq=Cyc_Toc_mt_tq,((_tmpBB1->type=_tmp474,((_tmpBB1->initializer=(
struct Cyc_Absyn_Exp*)xinit,((_tmpBB1->rgn=0,((_tmpBB1->attributes=0,((_tmpBB1->escapes=
0,_tmpBB1)))))))))))))))));struct Cyc_Absyn_Local_b_struct _tmpBB4;struct Cyc_Absyn_Local_b_struct*
_tmpBB3;struct Cyc_Absyn_Local_b_struct*_tmp484=(_tmpBB3=_cycalloc(sizeof(*
_tmpBB3)),((_tmpBB3[0]=((_tmpBB4.tag=4,((_tmpBB4.f1=_tmp483,_tmpBB4)))),_tmpBB3)));
struct Cyc_Absyn_Exp*_tmp485=Cyc_Absyn_varb_exp(_tmp480,(void*)_tmp484,0);_tmp485->topt=(
void*)fat_ptr_type;{struct Cyc_Absyn_Exp*_tmp486=Cyc_Absyn_deref_exp(_tmp485,0);
_tmp486->topt=(void*)elt_type;Cyc_Toc_exp_to_c(_tmp482,_tmp486);{struct _tuple0*
_tmp487=Cyc_Toc_temp_var();_tmp482=Cyc_Toc_add_varmap(rgn2,_tmp482,_tmp487,Cyc_Absyn_var_exp(
_tmp487,0));{struct Cyc_Absyn_Vardecl*_tmpBB5;struct Cyc_Absyn_Vardecl*_tmp488=(
_tmpBB5=_cycalloc(sizeof(*_tmpBB5)),((_tmpBB5->sc=Cyc_Absyn_Public,((_tmpBB5->name=
_tmp487,((_tmpBB5->tq=Cyc_Toc_mt_tq,((_tmpBB5->type=_tmp473,((_tmpBB5->initializer=(
struct Cyc_Absyn_Exp*)_tmp486,((_tmpBB5->rgn=0,((_tmpBB5->attributes=0,((_tmpBB5->escapes=
0,_tmpBB5)))))))))))))))));struct Cyc_Absyn_Local_b_struct _tmpBB8;struct Cyc_Absyn_Local_b_struct*
_tmpBB7;struct Cyc_Absyn_Local_b_struct*_tmp489=(_tmpBB7=_cycalloc(sizeof(*
_tmpBB7)),((_tmpBB7[0]=((_tmpBB8.tag=4,((_tmpBB8.f1=_tmp488,_tmpBB8)))),_tmpBB7)));
struct Cyc_Absyn_Exp*z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp48A=Cyc_Absyn_varb_exp(
_tmp487,(void*)_tmp489,0);_tmp48A->topt=_tmp486->topt;z_init=Cyc_Absyn_prim2_exp((
enum Cyc_Absyn_Primop)popt->v,_tmp48A,e2,0);z_init->topt=_tmp48A->topt;}Cyc_Toc_exp_to_c(
_tmp482,z_init);{struct _tuple0*_tmp48B=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmpBB9;struct Cyc_Absyn_Vardecl*_tmp48C=(_tmpBB9=_cycalloc(sizeof(*_tmpBB9)),((
_tmpBB9->sc=Cyc_Absyn_Public,((_tmpBB9->name=_tmp48B,((_tmpBB9->tq=Cyc_Toc_mt_tq,((
_tmpBB9->type=_tmp473,((_tmpBB9->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmpBB9->rgn=0,((_tmpBB9->attributes=0,((_tmpBB9->escapes=0,_tmpBB9)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpBBC;struct Cyc_Absyn_Local_b_struct*_tmpBBB;
struct Cyc_Absyn_Local_b_struct*_tmp48D=(_tmpBBB=_cycalloc(sizeof(*_tmpBBB)),((
_tmpBBB[0]=((_tmpBBC.tag=4,((_tmpBBC.f1=_tmp48C,_tmpBBC)))),_tmpBBB)));_tmp482=
Cyc_Toc_add_varmap(rgn2,_tmp482,_tmp48B,Cyc_Absyn_var_exp(_tmp48B,0));{struct Cyc_Absyn_Exp*
_tmp48E=Cyc_Absyn_varb_exp(_tmp487,(void*)_tmp489,0);_tmp48E->topt=_tmp486->topt;{
struct Cyc_Absyn_Exp*_tmp48F=Cyc_Toc_generate_zero(elt_type);struct Cyc_Absyn_Exp*
_tmp490=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp48E,_tmp48F,0);_tmp490->topt=(void*)
Cyc_Absyn_sint_typ;Cyc_Toc_exp_to_c(_tmp482,_tmp490);{struct Cyc_Absyn_Exp*
_tmp491=Cyc_Absyn_varb_exp(_tmp48B,(void*)_tmp48D,0);_tmp491->topt=_tmp486->topt;{
struct Cyc_Absyn_Exp*_tmp492=Cyc_Toc_generate_zero(elt_type);struct Cyc_Absyn_Exp*
_tmp493=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp491,_tmp492,0);_tmp493->topt=(void*)
Cyc_Absyn_sint_typ;Cyc_Toc_exp_to_c(_tmp482,_tmp493);{struct Cyc_Absyn_Exp*
_tmpBBD[2];struct Cyc_List_List*_tmp494=(_tmpBBD[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmpBBD[0]=Cyc_Absyn_varb_exp(_tmp480,(void*)_tmp484,0),((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBBD,
sizeof(struct Cyc_Absyn_Exp*),2)))));struct Cyc_Absyn_Exp*_tmp495=Cyc_Absyn_uint_exp(
1,0);struct Cyc_Absyn_Exp*xsize;xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(
Cyc_Toc__get_dyneither_size_e,_tmp494,0),_tmp495,0);{struct Cyc_Absyn_Exp*_tmp496=
Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp490,_tmp493,0),0);struct Cyc_Absyn_Stmt*
_tmp497=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),
0);struct Cyc_Absyn_Exp*_tmp498=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_varb_exp(
_tmp480,(void*)_tmp484,0),Cyc_Toc_curr_sp,0);_tmp498=Cyc_Toc_cast_it(_tmp475,
_tmp498);{struct Cyc_Absyn_Exp*_tmp499=Cyc_Absyn_deref_exp(_tmp498,0);struct Cyc_Absyn_Exp*
_tmp49A=Cyc_Absyn_assign_exp(_tmp499,Cyc_Absyn_var_exp(_tmp48B,0),0);struct Cyc_Absyn_Stmt*
_tmp49B=Cyc_Absyn_exp_stmt(_tmp49A,0);_tmp49B=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp496,_tmp497,Cyc_Absyn_skip_stmt(0),0),_tmp49B,0);{struct Cyc_Absyn_Var_d_struct*
_tmpBC3;struct Cyc_Absyn_Var_d_struct _tmpBC2;struct Cyc_Absyn_Decl*_tmpBC1;_tmp49B=
Cyc_Absyn_decl_stmt(((_tmpBC1=_cycalloc(sizeof(*_tmpBC1)),((_tmpBC1->r=(void*)((
_tmpBC3=_cycalloc(sizeof(*_tmpBC3)),((_tmpBC3[0]=((_tmpBC2.tag=0,((_tmpBC2.f1=
_tmp48C,_tmpBC2)))),_tmpBC3)))),((_tmpBC1->loc=0,_tmpBC1)))))),_tmp49B,0);}{
struct Cyc_Absyn_Var_d_struct*_tmpBC9;struct Cyc_Absyn_Var_d_struct _tmpBC8;struct
Cyc_Absyn_Decl*_tmpBC7;_tmp49B=Cyc_Absyn_decl_stmt(((_tmpBC7=_cycalloc(sizeof(*
_tmpBC7)),((_tmpBC7->r=(void*)((_tmpBC9=_cycalloc(sizeof(*_tmpBC9)),((_tmpBC9[0]=((
_tmpBC8.tag=0,((_tmpBC8.f1=_tmp488,_tmpBC8)))),_tmpBC9)))),((_tmpBC7->loc=0,
_tmpBC7)))))),_tmp49B,0);}{struct Cyc_Absyn_Var_d_struct*_tmpBCF;struct Cyc_Absyn_Var_d_struct
_tmpBCE;struct Cyc_Absyn_Decl*_tmpBCD;_tmp49B=Cyc_Absyn_decl_stmt(((_tmpBCD=
_cycalloc(sizeof(*_tmpBCD)),((_tmpBCD->r=(void*)((_tmpBCF=_cycalloc(sizeof(*
_tmpBCF)),((_tmpBCF[0]=((_tmpBCE.tag=0,((_tmpBCE.f1=_tmp483,_tmpBCE)))),_tmpBCF)))),((
_tmpBCD->loc=0,_tmpBCD)))))),_tmp49B,0);}e->r=Cyc_Toc_stmt_exp_r(_tmp49B);};};};};};};};};};};};};
_pop_region(rgn2);};}static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*
e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*
aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){struct Cyc_Absyn_Aggrdecl*
ad;{void*_tmp4B0=Cyc_Tcutil_compress(aggrtype);struct Cyc_Absyn_AggrInfo _tmp4B2;
union Cyc_Absyn_AggrInfoU _tmp4B3;_LL20B: {struct Cyc_Absyn_AggrType_struct*_tmp4B1=(
struct Cyc_Absyn_AggrType_struct*)_tmp4B0;if(_tmp4B1->tag != 11)goto _LL20D;else{
_tmp4B2=_tmp4B1->f1;_tmp4B3=_tmp4B2.aggr_info;}}_LL20C: ad=Cyc_Absyn_get_known_aggrdecl(
_tmp4B3);goto _LL20A;_LL20D:;_LL20E: {struct Cyc_String_pa_struct _tmpBD7;void*
_tmpBD6[1];const char*_tmpBD5;void*_tmpBD4;(_tmpBD4=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct
_dyneither_ptr)((_tmpBD7.tag=0,((_tmpBD7.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpBD6[0]=& _tmpBD7,Cyc_aprintf(((
_tmpBD5="expecting union but found %s in check_tagged_union",_tag_dyneither(
_tmpBD5,sizeof(char),51))),_tag_dyneither(_tmpBD6,sizeof(void*),1)))))))),
_tag_dyneither(_tmpBD4,sizeof(void*),0)));}_LL20A:;}{struct _tuple0*_tmp4B8=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4B9=Cyc_Absyn_var_exp(_tmp4B8,0);struct Cyc_Absyn_Exp*
_tmp4BA=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);if(in_lhs){
struct Cyc_Absyn_Exp*_tmp4BB=Cyc_Absyn_aggrarrow_exp(_tmp4B9,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4BC=Cyc_Absyn_neq_exp(_tmp4BB,_tmp4BA,0);struct Cyc_Absyn_Exp*
_tmp4BD=Cyc_Absyn_aggrarrow_exp(_tmp4B9,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Stmt*
_tmp4BE=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp4BD,0),0);struct Cyc_Absyn_Stmt*
_tmp4BF=Cyc_Absyn_ifthenelse_stmt(_tmp4BC,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);void*_tmp4C0=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp4C1=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);struct Cyc_Absyn_Stmt*_tmp4C2=
Cyc_Absyn_declare_stmt(_tmp4B8,_tmp4C0,(struct Cyc_Absyn_Exp*)_tmp4C1,Cyc_Absyn_seq_stmt(
_tmp4BF,_tmp4BE,0),0);return Cyc_Toc_stmt_exp_r(_tmp4C2);}else{struct Cyc_Absyn_Exp*
_tmp4C3=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp4B9,f,0),Cyc_Toc_tag_sp,0);struct
Cyc_Absyn_Exp*_tmp4C4=Cyc_Absyn_neq_exp(_tmp4C3,_tmp4BA,0);struct Cyc_Absyn_Exp*
_tmp4C5=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp4B9,f,0),Cyc_Toc_val_sp,0);struct
Cyc_Absyn_Stmt*_tmp4C6=Cyc_Absyn_exp_stmt(_tmp4C5,0);struct Cyc_Absyn_Stmt*
_tmp4C7=Cyc_Absyn_ifthenelse_stmt(_tmp4C4,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp4C8=Cyc_Absyn_declare_stmt(_tmp4B8,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp4C7,_tmp4C6,0),0);return Cyc_Toc_stmt_exp_r(
_tmp4C8);}};}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read){void*_tmp4C9=e->r;struct Cyc_Absyn_Exp*
_tmp4CB;struct Cyc_Absyn_Exp*_tmp4CD;struct _dyneither_ptr*_tmp4CE;int _tmp4CF;int*
_tmp4D0;struct Cyc_Absyn_Exp*_tmp4D2;struct _dyneither_ptr*_tmp4D3;int _tmp4D4;int*
_tmp4D5;_LL210: {struct Cyc_Absyn_Cast_e_struct*_tmp4CA=(struct Cyc_Absyn_Cast_e_struct*)
_tmp4C9;if(_tmp4CA->tag != 13)goto _LL212;else{_tmp4CB=_tmp4CA->f2;}}_LL211: {
const char*_tmpBDA;void*_tmpBD9;(_tmpBD9=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBDA="cast on lhs!",
_tag_dyneither(_tmpBDA,sizeof(char),13))),_tag_dyneither(_tmpBD9,sizeof(void*),0)));}
_LL212: {struct Cyc_Absyn_AggrMember_e_struct*_tmp4CC=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp4C9;if(_tmp4CC->tag != 20)goto _LL214;else{_tmp4CD=_tmp4CC->f1;_tmp4CE=_tmp4CC->f2;
_tmp4CF=_tmp4CC->f4;_tmp4D0=(int*)& _tmp4CC->f4;}}_LL213: {void*_tmp4D8=Cyc_Tcutil_compress((
void*)_check_null(_tmp4CD->topt));struct Cyc_Absyn_AggrInfo _tmp4DA;union Cyc_Absyn_AggrInfoU
_tmp4DB;_LL219: {struct Cyc_Absyn_AggrType_struct*_tmp4D9=(struct Cyc_Absyn_AggrType_struct*)
_tmp4D8;if(_tmp4D9->tag != 11)goto _LL21B;else{_tmp4DA=_tmp4D9->f1;_tmp4DB=_tmp4DA.aggr_info;}}
_LL21A: {struct Cyc_Absyn_Aggrdecl*_tmp4DC=Cyc_Absyn_get_known_aggrdecl(_tmp4DB);*
f_tag=Cyc_Toc_get_member_offset(_tmp4DC,_tmp4CE);{const char*_tmpBDF;void*_tmpBDE[
2];struct Cyc_String_pa_struct _tmpBDD;struct Cyc_String_pa_struct _tmpBDC;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpBDC.tag=0,((_tmpBDC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp4CE),((_tmpBDD.tag=0,((_tmpBDD.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4DC->name).f2),((_tmpBDE[0]=&
_tmpBDD,((_tmpBDE[1]=& _tmpBDC,Cyc_aprintf(((_tmpBDF="_union_%s_%s",
_tag_dyneither(_tmpBDF,sizeof(char),13))),_tag_dyneither(_tmpBDE,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpBE0;*tagged_member_type=Cyc_Absyn_strct(((_tmpBE0=
_cycalloc(sizeof(*_tmpBE0)),((_tmpBE0[0]=str,_tmpBE0)))));}if(clear_read)*
_tmp4D0=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4DC->impl))->tagged;};}
_LL21B:;_LL21C: return 0;_LL218:;}_LL214: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp4D1=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp4C9;if(_tmp4D1->tag != 21)goto
_LL216;else{_tmp4D2=_tmp4D1->f1;_tmp4D3=_tmp4D1->f2;_tmp4D4=_tmp4D1->f4;_tmp4D5=(
int*)& _tmp4D1->f4;}}_LL215: {void*_tmp4E2=Cyc_Tcutil_compress((void*)_check_null(
_tmp4D2->topt));struct Cyc_Absyn_PtrInfo _tmp4E4;void*_tmp4E5;_LL21E: {struct Cyc_Absyn_PointerType_struct*
_tmp4E3=(struct Cyc_Absyn_PointerType_struct*)_tmp4E2;if(_tmp4E3->tag != 5)goto
_LL220;else{_tmp4E4=_tmp4E3->f1;_tmp4E5=_tmp4E4.elt_typ;}}_LL21F: {void*_tmp4E6=
Cyc_Tcutil_compress(_tmp4E5);struct Cyc_Absyn_AggrInfo _tmp4E8;union Cyc_Absyn_AggrInfoU
_tmp4E9;_LL223: {struct Cyc_Absyn_AggrType_struct*_tmp4E7=(struct Cyc_Absyn_AggrType_struct*)
_tmp4E6;if(_tmp4E7->tag != 11)goto _LL225;else{_tmp4E8=_tmp4E7->f1;_tmp4E9=_tmp4E8.aggr_info;}}
_LL224: {struct Cyc_Absyn_Aggrdecl*_tmp4EA=Cyc_Absyn_get_known_aggrdecl(_tmp4E9);*
f_tag=Cyc_Toc_get_member_offset(_tmp4EA,_tmp4D3);{const char*_tmpBE5;void*_tmpBE4[
2];struct Cyc_String_pa_struct _tmpBE3;struct Cyc_String_pa_struct _tmpBE2;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpBE2.tag=0,((_tmpBE2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp4D3),((_tmpBE3.tag=0,((_tmpBE3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4EA->name).f2),((_tmpBE4[0]=&
_tmpBE3,((_tmpBE4[1]=& _tmpBE2,Cyc_aprintf(((_tmpBE5="_union_%s_%s",
_tag_dyneither(_tmpBE5,sizeof(char),13))),_tag_dyneither(_tmpBE4,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpBE6;*tagged_member_type=Cyc_Absyn_strct(((_tmpBE6=
_cycalloc(sizeof(*_tmpBE6)),((_tmpBE6[0]=str,_tmpBE6)))));}if(clear_read)*
_tmp4D5=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4EA->impl))->tagged;};}
_LL225:;_LL226: return 0;_LL222:;}_LL220:;_LL221: return 0;_LL21D:;}_LL216:;_LL217:
return 0;_LL20F:;}void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*
e,int tag,void*mem_type){struct _tuple0*_tmp4F0=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
temp_exp=Cyc_Absyn_var_exp(_tmp4F0,0);struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(
tag,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,
0),0);struct Cyc_Absyn_Exp*_tmp4F1=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_ifthenelse_stmt(_tmp4F1,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4F0,Cyc_Absyn_cstar_typ(
mem_type,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(
e)),Cyc_Absyn_seq_stmt(s2,s3,0),0);e->r=Cyc_Toc_stmt_exp_r(s1);}static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type){struct _tuple0*_tmp4F2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4F2,0);struct Cyc_Absyn_Exp*
temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*
temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 
0)s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*
_tmp4F3=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp4F3,
Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=
Cyc_Absyn_declare_stmt(_tmp4F2,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(e1),Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple21{struct _tuple0*f1;void*f2;struct
Cyc_Absyn_Exp*f3;};struct _tuple22{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*
f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void _tmpC35(unsigned int*_tmpC34,unsigned int*_tmpC33,struct _tuple0***
_tmpC31){for(*_tmpC34=0;*_tmpC34 < *_tmpC33;(*_tmpC34)++){(*_tmpC31)[*_tmpC34]=
Cyc_Toc_temp_var();}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){void*_tmp4F4=e->r;if(e->topt == 0){const char*_tmpBEA;void*_tmpBE9[1];struct Cyc_String_pa_struct
_tmpBE8;(_tmpBE8.tag=0,((_tmpBE8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpBE9[0]=& _tmpBE8,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBEA="exp_to_c: no type for %s",
_tag_dyneither(_tmpBEA,sizeof(char),25))),_tag_dyneither(_tmpBE9,sizeof(void*),1)))))));}{
void*old_typ=(void*)_check_null(e->topt);void*_tmp4F8=_tmp4F4;union Cyc_Absyn_Cnst
_tmp4FA;int _tmp4FB;struct _tuple0*_tmp4FE;void*_tmp4FF;enum Cyc_Absyn_Primop
_tmp501;struct Cyc_List_List*_tmp502;struct Cyc_Absyn_Exp*_tmp504;enum Cyc_Absyn_Incrementor
_tmp505;struct Cyc_Absyn_Exp*_tmp507;struct Cyc_Core_Opt*_tmp508;struct Cyc_Absyn_Exp*
_tmp509;struct Cyc_Absyn_Exp*_tmp50B;struct Cyc_Absyn_Exp*_tmp50C;struct Cyc_Absyn_Exp*
_tmp50D;struct Cyc_Absyn_Exp*_tmp50F;struct Cyc_Absyn_Exp*_tmp510;struct Cyc_Absyn_Exp*
_tmp512;struct Cyc_Absyn_Exp*_tmp513;struct Cyc_Absyn_Exp*_tmp515;struct Cyc_Absyn_Exp*
_tmp516;struct Cyc_Absyn_Exp*_tmp518;struct Cyc_List_List*_tmp519;struct Cyc_Absyn_VarargCallInfo*
_tmp51A;struct Cyc_Absyn_Exp*_tmp51C;struct Cyc_List_List*_tmp51D;struct Cyc_Absyn_VarargCallInfo*
_tmp51E;struct Cyc_Absyn_VarargCallInfo _tmp51F;int _tmp520;struct Cyc_List_List*
_tmp521;struct Cyc_Absyn_VarargInfo*_tmp522;struct Cyc_Absyn_Exp*_tmp524;struct Cyc_Absyn_Exp*
_tmp526;struct Cyc_Absyn_Exp*_tmp528;struct Cyc_List_List*_tmp529;void*_tmp52B;
void**_tmp52C;struct Cyc_Absyn_Exp*_tmp52D;int _tmp52E;enum Cyc_Absyn_Coercion
_tmp52F;struct Cyc_Absyn_Exp*_tmp531;struct Cyc_Absyn_Exp*_tmp533;struct Cyc_Absyn_Exp*
_tmp534;struct Cyc_Absyn_Exp*_tmp536;void*_tmp538;void*_tmp53A;void*_tmp53B;
struct _dyneither_ptr*_tmp53D;void*_tmp53F;void*_tmp540;unsigned int _tmp542;
struct Cyc_Absyn_Exp*_tmp544;struct Cyc_Absyn_Exp*_tmp546;struct _dyneither_ptr*
_tmp547;int _tmp548;int _tmp549;struct Cyc_Absyn_Exp*_tmp54B;struct _dyneither_ptr*
_tmp54C;int _tmp54D;int _tmp54E;struct Cyc_Absyn_Exp*_tmp550;struct Cyc_Absyn_Exp*
_tmp551;struct Cyc_List_List*_tmp553;struct Cyc_List_List*_tmp555;struct Cyc_Absyn_Vardecl*
_tmp557;struct Cyc_Absyn_Exp*_tmp558;struct Cyc_Absyn_Exp*_tmp559;int _tmp55A;
struct _tuple0*_tmp55C;struct Cyc_List_List*_tmp55D;struct Cyc_List_List*_tmp55E;
struct Cyc_Absyn_Aggrdecl*_tmp55F;void*_tmp561;struct Cyc_List_List*_tmp562;struct
Cyc_List_List*_tmp564;struct Cyc_Absyn_Datatypedecl*_tmp565;struct Cyc_Absyn_Datatypefield*
_tmp566;struct Cyc_Absyn_MallocInfo _tmp56A;int _tmp56B;struct Cyc_Absyn_Exp*_tmp56C;
void**_tmp56D;struct Cyc_Absyn_Exp*_tmp56E;int _tmp56F;struct Cyc_Absyn_Exp*_tmp571;
struct Cyc_Absyn_Exp*_tmp572;struct Cyc_Absyn_Exp*_tmp574;struct _dyneither_ptr*
_tmp575;struct Cyc_Absyn_Stmt*_tmp577;_LL228: {struct Cyc_Absyn_Const_e_struct*
_tmp4F9=(struct Cyc_Absyn_Const_e_struct*)_tmp4F8;if(_tmp4F9->tag != 0)goto _LL22A;
else{_tmp4FA=_tmp4F9->f1;if((_tmp4FA.Null_c).tag != 1)goto _LL22A;_tmp4FB=(int)(
_tmp4FA.Null_c).val;}}_LL229: {struct Cyc_Absyn_Exp*_tmp57C=Cyc_Absyn_uint_exp(0,
0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){if(Cyc_Toc_is_toplevel(nv))e->r=(
Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp57C,_tmp57C))->r;else{struct Cyc_Absyn_Exp*
_tmpBEB[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpBEB[2]=
_tmp57C,((_tmpBEB[1]=_tmp57C,((_tmpBEB[0]=_tmp57C,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBEB,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL227;}_LL22A: {struct Cyc_Absyn_Const_e_struct*
_tmp4FC=(struct Cyc_Absyn_Const_e_struct*)_tmp4F8;if(_tmp4FC->tag != 0)goto _LL22C;}
_LL22B: goto _LL227;_LL22C: {struct Cyc_Absyn_Var_e_struct*_tmp4FD=(struct Cyc_Absyn_Var_e_struct*)
_tmp4F8;if(_tmp4FD->tag != 1)goto _LL22E;else{_tmp4FE=_tmp4FD->f1;_tmp4FF=(void*)
_tmp4FD->f2;}}_LL22D:{struct _handler_cons _tmp57E;_push_handler(& _tmp57E);{int
_tmp580=0;if(setjmp(_tmp57E.handler))_tmp580=1;if(!_tmp580){e->r=(Cyc_Toc_lookup_varmap(
nv,_tmp4FE))->r;;_pop_handler();}else{void*_tmp57F=(void*)_exn_thrown;void*
_tmp582=_tmp57F;_LL27D: {struct Cyc_Dict_Absent_struct*_tmp583=(struct Cyc_Dict_Absent_struct*)
_tmp582;if(_tmp583->tag != Cyc_Dict_Absent)goto _LL27F;}_LL27E: {const char*_tmpBEF;
void*_tmpBEE[1];struct Cyc_String_pa_struct _tmpBED;(_tmpBED.tag=0,((_tmpBED.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4FE)),((
_tmpBEE[0]=& _tmpBED,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBEF="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpBEF,sizeof(char),
32))),_tag_dyneither(_tmpBEE,sizeof(void*),1)))))));}_LL27F:;_LL280:(void)_throw(
_tmp582);_LL27C:;}};}goto _LL227;_LL22E: {struct Cyc_Absyn_Primop_e_struct*_tmp500=(
struct Cyc_Absyn_Primop_e_struct*)_tmp4F8;if(_tmp500->tag != 2)goto _LL230;else{
_tmp501=_tmp500->f1;_tmp502=_tmp500->f2;}}_LL22F: {struct Cyc_List_List*_tmp587=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp502);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp502);switch(_tmp501){case Cyc_Absyn_Numelts: _LL281: {
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp502))->hd;{void*_tmp588=Cyc_Tcutil_compress((void*)_check_null(arg->topt));
struct Cyc_Absyn_ArrayInfo _tmp58A;struct Cyc_Absyn_Exp*_tmp58B;struct Cyc_Absyn_PtrInfo
_tmp58D;void*_tmp58E;struct Cyc_Absyn_PtrAtts _tmp58F;union Cyc_Absyn_Constraint*
_tmp590;union Cyc_Absyn_Constraint*_tmp591;union Cyc_Absyn_Constraint*_tmp592;
_LL284: {struct Cyc_Absyn_ArrayType_struct*_tmp589=(struct Cyc_Absyn_ArrayType_struct*)
_tmp588;if(_tmp589->tag != 8)goto _LL286;else{_tmp58A=_tmp589->f1;_tmp58B=_tmp58A.num_elts;}}
_LL285: if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp58B))){
const char*_tmpBF2;void*_tmpBF1;(_tmpBF1=0,Cyc_Tcutil_terr(e->loc,((_tmpBF2="can't calculate numelts",
_tag_dyneither(_tmpBF2,sizeof(char),24))),_tag_dyneither(_tmpBF1,sizeof(void*),0)));}
e->r=_tmp58B->r;goto _LL283;_LL286: {struct Cyc_Absyn_PointerType_struct*_tmp58C=(
struct Cyc_Absyn_PointerType_struct*)_tmp588;if(_tmp58C->tag != 5)goto _LL288;else{
_tmp58D=_tmp58C->f1;_tmp58E=_tmp58D.elt_typ;_tmp58F=_tmp58D.ptr_atts;_tmp590=
_tmp58F.nullable;_tmp591=_tmp58F.bounds;_tmp592=_tmp58F.zero_term;}}_LL287:{void*
_tmp595=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp591);struct Cyc_Absyn_Exp*_tmp598;_LL28B: {struct Cyc_Absyn_DynEither_b_struct*
_tmp596=(struct Cyc_Absyn_DynEither_b_struct*)_tmp595;if(_tmp596->tag != 0)goto
_LL28D;}_LL28C:{struct Cyc_Absyn_Exp*_tmpBF3[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,((
_tmpBF3[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp58E),0),((_tmpBF3[0]=(
struct Cyc_Absyn_Exp*)_tmp502->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpBF3,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL28A;_LL28D: {struct Cyc_Absyn_Upper_b_struct*_tmp597=(struct Cyc_Absyn_Upper_b_struct*)
_tmp595;if(_tmp597->tag != 1)goto _LL28A;else{_tmp598=_tmp597->f1;}}_LL28E: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp592)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp502->hd);struct Cyc_Absyn_Exp*_tmpBF4[2];e->r=Cyc_Toc_fncall_exp_r(
function_e,((_tmpBF4[1]=_tmp598,((_tmpBF4[0]=(struct Cyc_Absyn_Exp*)_tmp502->hd,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpBF4,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp590)){if(!Cyc_Evexp_c_can_eval(_tmp598)){const
char*_tmpBF7;void*_tmpBF6;(_tmpBF6=0,Cyc_Tcutil_terr(e->loc,((_tmpBF7="can't calculate numelts",
_tag_dyneither(_tmpBF7,sizeof(char),24))),_tag_dyneither(_tmpBF6,sizeof(void*),0)));}
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp598,Cyc_Absyn_uint_exp(0,0));}else{e->r=
_tmp598->r;goto _LL28A;}}goto _LL28A;_LL28A:;}goto _LL283;_LL288:;_LL289: {const
char*_tmpBFC;void*_tmpBFB[2];struct Cyc_String_pa_struct _tmpBFA;struct Cyc_String_pa_struct
_tmpBF9;(_tmpBF9.tag=0,((_tmpBF9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpBFA.tag=0,((_tmpBFA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)
_check_null(arg->topt))),((_tmpBFB[0]=& _tmpBFA,((_tmpBFB[1]=& _tmpBF9,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBFC="size primop applied to non-array %s (%s)",
_tag_dyneither(_tmpBFC,sizeof(char),41))),_tag_dyneither(_tmpBFB,sizeof(void*),2)))))))))))));}
_LL283:;}break;}case Cyc_Absyn_Plus: _LL282:{void*_tmp5A1=Cyc_Tcutil_compress((
void*)((struct Cyc_List_List*)_check_null(_tmp587))->hd);struct Cyc_Absyn_PtrInfo
_tmp5A3;void*_tmp5A4;struct Cyc_Absyn_PtrAtts _tmp5A5;union Cyc_Absyn_Constraint*
_tmp5A6;union Cyc_Absyn_Constraint*_tmp5A7;_LL291: {struct Cyc_Absyn_PointerType_struct*
_tmp5A2=(struct Cyc_Absyn_PointerType_struct*)_tmp5A1;if(_tmp5A2->tag != 5)goto
_LL293;else{_tmp5A3=_tmp5A2->f1;_tmp5A4=_tmp5A3.elt_typ;_tmp5A5=_tmp5A3.ptr_atts;
_tmp5A6=_tmp5A5.bounds;_tmp5A7=_tmp5A5.zero_term;}}_LL292:{void*_tmp5A8=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp5A6);struct Cyc_Absyn_Exp*_tmp5AB;_LL296: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5A9=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5A8;if(_tmp5A9->tag != 0)goto
_LL298;}_LL297: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp502))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp502->tl))->hd;{struct Cyc_Absyn_Exp*_tmpBFD[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBFD[2]=e2,((_tmpBFD[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A4),0),((_tmpBFD[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBFD,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}goto _LL295;}_LL298: {struct Cyc_Absyn_Upper_b_struct*_tmp5AA=(struct
Cyc_Absyn_Upper_b_struct*)_tmp5A8;if(_tmp5AA->tag != 1)goto _LL295;else{_tmp5AB=
_tmp5AA->f1;}}_LL299: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp5A7)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp502))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp502->tl))->hd;struct Cyc_Absyn_Exp*_tmpBFE[3];e->r=(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
e1),((_tmpBFE[2]=e2,((_tmpBFE[1]=_tmp5AB,((_tmpBFE[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBFE,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r;}goto _LL295;_LL295:;}goto _LL290;_LL293:;_LL294: goto _LL290;
_LL290:;}break;case Cyc_Absyn_Minus: _LL28F: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp587))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp502))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp502->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp587->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{struct Cyc_Absyn_Exp*_tmpBFF[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBFF[2]=Cyc_Absyn_prim1_exp(
Cyc_Absyn_Minus,e2,0),((_tmpBFF[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpBFF[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBFF,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}break;}case Cyc_Absyn_Eq:
_LL29A: goto _LL29B;case Cyc_Absyn_Neq: _LL29B: goto _LL29C;case Cyc_Absyn_Gt: _LL29C:
goto _LL29D;case Cyc_Absyn_Gte: _LL29D: goto _LL29E;case Cyc_Absyn_Lt: _LL29E: goto
_LL29F;case Cyc_Absyn_Lte: _LL29F: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp502))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp502->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp587))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp587->tl))->hd;void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& elt_typ))e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,
0));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp,0));break;}default: _LL2A0: break;}goto _LL227;}_LL230: {
struct Cyc_Absyn_Increment_e_struct*_tmp503=(struct Cyc_Absyn_Increment_e_struct*)
_tmp4F8;if(_tmp503->tag != 4)goto _LL232;else{_tmp504=_tmp503->f1;_tmp505=_tmp503->f2;}}
_LL231: {void*e2_cyc_typ=(void*)_check_null(_tmp504->topt);void*ptr_type=(void*)&
Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;int
is_dyneither=0;const char*_tmpC00;struct _dyneither_ptr incr_str=(_tmpC00="increment",
_tag_dyneither(_tmpC00,sizeof(char),10));if(_tmp505 == Cyc_Absyn_PreDec  || 
_tmp505 == Cyc_Absyn_PostDec){const char*_tmpC01;incr_str=((_tmpC01="decrement",
_tag_dyneither(_tmpC01,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp504,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpC05;void*_tmpC04[1];
struct Cyc_String_pa_struct _tmpC03;(_tmpC03.tag=0,((_tmpC03.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC04[0]=& _tmpC03,Cyc_Tcutil_terr(
e->loc,((_tmpC05="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpC05,sizeof(char),74))),_tag_dyneither(_tmpC04,sizeof(void*),1)))))));}{
const char*_tmpC08;void*_tmpC07;(_tmpC07=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC08="in-place inc/dec on zero-term",
_tag_dyneither(_tmpC08,sizeof(char),30))),_tag_dyneither(_tmpC07,sizeof(void*),0)));};}{
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp504,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp5B5=Cyc_Absyn_signed_int_exp(
1,0);_tmp5B5->topt=(void*)Cyc_Absyn_sint_typ;switch(_tmp505){case Cyc_Absyn_PreInc:
_LL2A2:{struct Cyc_Absyn_AssignOp_e_struct _tmpC0E;struct Cyc_Core_Opt*_tmpC0D;
struct Cyc_Absyn_AssignOp_e_struct*_tmpC0C;e->r=(void*)((_tmpC0C=_cycalloc(
sizeof(*_tmpC0C)),((_tmpC0C[0]=((_tmpC0E.tag=3,((_tmpC0E.f1=_tmp504,((_tmpC0E.f2=((
_tmpC0D=_cycalloc_atomic(sizeof(*_tmpC0D)),((_tmpC0D->v=(void*)Cyc_Absyn_Plus,
_tmpC0D)))),((_tmpC0E.f3=_tmp5B5,_tmpC0E)))))))),_tmpC0C))));}Cyc_Toc_exp_to_c(
nv,e);return;case Cyc_Absyn_PreDec: _LL2A3:{struct Cyc_Absyn_AssignOp_e_struct
_tmpC14;struct Cyc_Core_Opt*_tmpC13;struct Cyc_Absyn_AssignOp_e_struct*_tmpC12;e->r=(
void*)((_tmpC12=_cycalloc(sizeof(*_tmpC12)),((_tmpC12[0]=((_tmpC14.tag=3,((
_tmpC14.f1=_tmp504,((_tmpC14.f2=((_tmpC13=_cycalloc_atomic(sizeof(*_tmpC13)),((
_tmpC13->v=(void*)Cyc_Absyn_Minus,_tmpC13)))),((_tmpC14.f3=_tmp5B5,_tmpC14)))))))),
_tmpC12))));}Cyc_Toc_exp_to_c(nv,e);return;default: _LL2A4:{const char*_tmpC18;
void*_tmpC17[1];struct Cyc_String_pa_struct _tmpC16;(_tmpC16.tag=0,((_tmpC16.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC17[0]=& _tmpC16,Cyc_Tcutil_terr(
e->loc,((_tmpC18="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpC18,sizeof(char),59))),_tag_dyneither(_tmpC17,sizeof(void*),1)))))));}{
const char*_tmpC1B;void*_tmpC1A;(_tmpC1A=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC1B="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpC1B,sizeof(char),34))),_tag_dyneither(_tmpC1A,sizeof(void*),0)));};}}
Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp504);Cyc_Toc_set_lhs(nv,0);{void*
elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp505 == Cyc_Absyn_PostInc
 || _tmp505 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp505 == Cyc_Absyn_PreDec  || _tmp505 == Cyc_Absyn_PostDec)change=- 1;{struct
Cyc_Absyn_Exp*_tmpC1C[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC1C[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpC1C[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpC1C[0]=Cyc_Toc_push_address_exp(_tmp504),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1C,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Toc_functionSet*_tmp5C2=_tmp505 == Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:&
Cyc_Toc__zero_arr_inplace_plus_post_functionSet;struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(
_tmp5C2,_tmp504);struct Cyc_Absyn_Exp*_tmpC1D[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((
_tmpC1D[1]=Cyc_Absyn_signed_int_exp(1,0),((_tmpC1D[0]=Cyc_Toc_push_address_exp(
_tmp504),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC1D,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(elt_typ == (
void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp504)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(
_tmp504,0,Cyc_Toc_incr_lvalue,_tmp505);e->r=_tmp504->r;}}}goto _LL227;};};}_LL232: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp506=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp4F8;if(_tmp506->tag != 3)goto _LL234;else{_tmp507=_tmp506->f1;_tmp508=_tmp506->f2;
_tmp509=_tmp506->f3;}}_LL233: {void*e1_old_typ=(void*)_check_null(_tmp507->topt);
void*e2_old_typ=(void*)_check_null(_tmp509->topt);int f_tag=0;void*
tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Toc_is_tagged_union_project(
_tmp507,& f_tag,& tagged_member_struct_type,1)){Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(
nv,_tmp507);Cyc_Toc_set_lhs(nv,0);Cyc_Toc_exp_to_c(nv,_tmp509);e->r=Cyc_Toc_tagged_union_assignop(
_tmp507,e1_old_typ,_tmp508,_tmp509,e2_old_typ,f_tag,tagged_member_struct_type);
return;}{void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(_tmp507,& ptr_type,&
is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp507,_tmp508,
_tmp509,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp507);Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp507);Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp509);{int done=0;if(_tmp508 != 0){void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;switch((enum Cyc_Absyn_Primop)_tmp508->v){case Cyc_Absyn_Plus: _LL2A6:
change=_tmp509;break;case Cyc_Absyn_Minus: _LL2A7: change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,
_tmp509,0);break;default: _LL2A8: {const char*_tmpC20;void*_tmpC1F;(_tmpC1F=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC20="bad t ? pointer arithmetic",_tag_dyneither(_tmpC20,sizeof(char),27))),
_tag_dyneither(_tmpC1F,sizeof(void*),0)));}}done=1;{struct Cyc_Absyn_Exp*_tmp5C7=
Cyc_Toc__dyneither_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*_tmpC21[3];e->r=Cyc_Toc_fncall_exp_r(
_tmp5C7,((_tmpC21[2]=change,((_tmpC21[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpC21[0]=Cyc_Toc_push_address_exp(_tmp507),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC21,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))
switch((enum Cyc_Absyn_Primop)_tmp508->v){case Cyc_Absyn_Plus: _LL2AA: done=1;{
struct Cyc_Absyn_Exp*_tmpC22[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp507),((_tmpC22[1]=_tmp509,((
_tmpC22[0]=_tmp507,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC22,sizeof(struct Cyc_Absyn_Exp*),2)))))));}break;default:
_LL2AB: {const char*_tmpC25;void*_tmpC24;(_tmpC24=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC25="bad zero-terminated pointer arithmetic",
_tag_dyneither(_tmpC25,sizeof(char),39))),_tag_dyneither(_tmpC24,sizeof(void*),0)));}}}}
if(!done){if(e1_poly)_tmp509->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp509->r,0));if(!Cyc_Absyn_is_lvalue(_tmp507)){{struct _tuple20 _tmpC28;struct
_tuple20*_tmpC27;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple20*),struct _tuple20*f_env))
Cyc_Toc_lvalue_assign)(_tmp507,0,Cyc_Toc_assignop_lvalue,((_tmpC27=_cycalloc(
sizeof(struct _tuple20)* 1),((_tmpC27[0]=((_tmpC28.f1=_tmp508,((_tmpC28.f2=
_tmp509,_tmpC28)))),_tmpC27)))));}e->r=_tmp507->r;}}goto _LL227;};};};}_LL234: {
struct Cyc_Absyn_Conditional_e_struct*_tmp50A=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp4F8;if(_tmp50A->tag != 5)goto _LL236;else{_tmp50B=_tmp50A->f1;_tmp50C=_tmp50A->f2;
_tmp50D=_tmp50A->f3;}}_LL235: Cyc_Toc_exp_to_c(nv,_tmp50B);Cyc_Toc_exp_to_c(nv,
_tmp50C);Cyc_Toc_exp_to_c(nv,_tmp50D);goto _LL227;_LL236: {struct Cyc_Absyn_And_e_struct*
_tmp50E=(struct Cyc_Absyn_And_e_struct*)_tmp4F8;if(_tmp50E->tag != 6)goto _LL238;
else{_tmp50F=_tmp50E->f1;_tmp510=_tmp50E->f2;}}_LL237: Cyc_Toc_exp_to_c(nv,
_tmp50F);Cyc_Toc_exp_to_c(nv,_tmp510);goto _LL227;_LL238: {struct Cyc_Absyn_Or_e_struct*
_tmp511=(struct Cyc_Absyn_Or_e_struct*)_tmp4F8;if(_tmp511->tag != 7)goto _LL23A;
else{_tmp512=_tmp511->f1;_tmp513=_tmp511->f2;}}_LL239: Cyc_Toc_exp_to_c(nv,
_tmp512);Cyc_Toc_exp_to_c(nv,_tmp513);goto _LL227;_LL23A: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp514=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp4F8;if(_tmp514->tag != 8)goto _LL23C;
else{_tmp515=_tmp514->f1;_tmp516=_tmp514->f2;}}_LL23B: Cyc_Toc_exp_to_c(nv,
_tmp515);Cyc_Toc_exp_to_c(nv,_tmp516);goto _LL227;_LL23C: {struct Cyc_Absyn_FnCall_e_struct*
_tmp517=(struct Cyc_Absyn_FnCall_e_struct*)_tmp4F8;if(_tmp517->tag != 9)goto _LL23E;
else{_tmp518=_tmp517->f1;_tmp519=_tmp517->f2;_tmp51A=_tmp517->f3;if(_tmp51A != 0)
goto _LL23E;}}_LL23D: Cyc_Toc_exp_to_c(nv,_tmp518);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp519);goto _LL227;_LL23E: {struct Cyc_Absyn_FnCall_e_struct*
_tmp51B=(struct Cyc_Absyn_FnCall_e_struct*)_tmp4F8;if(_tmp51B->tag != 9)goto _LL240;
else{_tmp51C=_tmp51B->f1;_tmp51D=_tmp51B->f2;_tmp51E=_tmp51B->f3;if(_tmp51E == 0)
goto _LL240;_tmp51F=*_tmp51E;_tmp520=_tmp51F.num_varargs;_tmp521=_tmp51F.injectors;
_tmp522=_tmp51F.vai;}}_LL23F: {struct _RegionHandle _tmp5CE=_new_region("r");
struct _RegionHandle*r=& _tmp5CE;_push_region(r);{struct _tuple0*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*
num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp520,0);void*cva_type=Cyc_Toc_typ_to_c(
_tmp522->type);void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct
Cyc_Absyn_Exp*)num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp51D);int num_normargs=num_args - 
_tmp520;struct Cyc_List_List*new_args=0;{int i=0;for(0;i < num_normargs;(++ i,
_tmp51D=_tmp51D->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp51D))->hd);{struct Cyc_List_List*_tmpC29;new_args=((_tmpC29=
_cycalloc(sizeof(*_tmpC29)),((_tmpC29->hd=(struct Cyc_Absyn_Exp*)_tmp51D->hd,((
_tmpC29->tl=new_args,_tmpC29))))));};}}{struct Cyc_Absyn_Exp*_tmpC2C[3];struct Cyc_List_List*
_tmpC2B;new_args=((_tmpC2B=_cycalloc(sizeof(*_tmpC2B)),((_tmpC2B->hd=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpC2C[2]=num_varargs_exp,((_tmpC2C[1]=Cyc_Absyn_sizeoftyp_exp(
cva_type,0),((_tmpC2C[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC2C,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((
_tmpC2B->tl=new_args,_tmpC2B))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp51C);{struct Cyc_Absyn_Stmt*
s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp51C,new_args,0),0);if(_tmp522->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp5D2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp522->type));struct Cyc_Absyn_DatatypeInfo _tmp5D4;union Cyc_Absyn_DatatypeInfoU
_tmp5D5;struct Cyc_Absyn_Datatypedecl**_tmp5D6;struct Cyc_Absyn_Datatypedecl*
_tmp5D7;_LL2AE: {struct Cyc_Absyn_DatatypeType_struct*_tmp5D3=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp5D2;if(_tmp5D3->tag != 3)goto _LL2B0;else{_tmp5D4=_tmp5D3->f1;_tmp5D5=_tmp5D4.datatype_info;
if((_tmp5D5.KnownDatatype).tag != 2)goto _LL2B0;_tmp5D6=(struct Cyc_Absyn_Datatypedecl**)(
_tmp5D5.KnownDatatype).val;_tmp5D7=*_tmp5D6;}}_LL2AF: tud=_tmp5D7;goto _LL2AD;
_LL2B0:;_LL2B1: {const char*_tmpC2F;void*_tmpC2E;(_tmpC2E=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC2F="toc: unknown datatype in vararg with inject",
_tag_dyneither(_tmpC2F,sizeof(char),44))),_tag_dyneither(_tmpC2E,sizeof(void*),0)));}
_LL2AD:;}{unsigned int _tmpC34;unsigned int _tmpC33;struct _dyneither_ptr _tmpC32;
struct _tuple0**_tmpC31;unsigned int _tmpC30;struct _dyneither_ptr vs=(_tmpC30=(
unsigned int)_tmp520,((_tmpC31=(struct _tuple0**)_region_malloc(r,_check_times(
sizeof(struct _tuple0*),_tmpC30)),((_tmpC32=_tag_dyneither(_tmpC31,sizeof(struct
_tuple0*),_tmpC30),((((_tmpC33=_tmpC30,_tmpC35(& _tmpC34,& _tmpC33,& _tmpC31))),
_tmpC32)))))));if(_tmp520 != 0){struct Cyc_List_List*_tmp5DA=0;{int i=_tmp520 - 1;
for(0;i >= 0;-- i){struct Cyc_List_List*_tmpC36;_tmp5DA=((_tmpC36=_cycalloc(sizeof(*
_tmpC36)),((_tmpC36->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((
struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i)),0),0)),((
_tmpC36->tl=_tmp5DA,_tmpC36))))));}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp5DA,0),s,0);{int i=0;for(0;
_tmp51D != 0;(((_tmp51D=_tmp51D->tl,_tmp521=_tmp521->tl)),++ i)){struct Cyc_Absyn_Exp*
arg=(struct Cyc_Absyn_Exp*)_tmp51D->hd;void*arg_type=(void*)_check_null(arg->topt);
struct _tuple0*var=*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct
_tuple0*),i));struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield
_tmp5DD;struct _tuple0*_tmp5DE;struct Cyc_List_List*_tmp5DF;struct Cyc_Absyn_Datatypefield*
_tmp5DC=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp521))->hd;_tmp5DD=*_tmp5DC;_tmp5DE=_tmp5DD.name;_tmp5DF=_tmp5DD.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp5DF))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_boxed_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp5DE),0),s,0);{const char*_tmpC37;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp5DE,((_tmpC37="_struct",_tag_dyneither(_tmpC37,
sizeof(char),8))))),0,s,0);};};}};}else{struct _tuple16*_tmpC38[3];struct Cyc_List_List*
_tmp5E1=(_tmpC38[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC38[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpC38[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC38,sizeof(struct _tuple16*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{{int i=0;for(0;_tmp51D
!= 0;(_tmp51D=_tmp51D->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp51D->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp51D->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);};}_npop_handler(0);goto _LL227;;_pop_region(r);}_LL240: {struct Cyc_Absyn_Throw_e_struct*
_tmp523=(struct Cyc_Absyn_Throw_e_struct*)_tmp4F8;if(_tmp523->tag != 10)goto _LL242;
else{_tmp524=_tmp523->f1;}}_LL241: Cyc_Toc_exp_to_c(nv,_tmp524);e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp524),0))->r;goto _LL227;_LL242: {
struct Cyc_Absyn_NoInstantiate_e_struct*_tmp525=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp4F8;if(_tmp525->tag != 11)goto _LL244;else{_tmp526=_tmp525->f1;}}_LL243: Cyc_Toc_exp_to_c(
nv,_tmp526);goto _LL227;_LL244: {struct Cyc_Absyn_Instantiate_e_struct*_tmp527=(
struct Cyc_Absyn_Instantiate_e_struct*)_tmp4F8;if(_tmp527->tag != 12)goto _LL246;
else{_tmp528=_tmp527->f1;_tmp529=_tmp527->f2;}}_LL245: Cyc_Toc_exp_to_c(nv,
_tmp528);for(0;_tmp529 != 0;_tmp529=_tmp529->tl){enum Cyc_Absyn_KindQual _tmp5E7=(
Cyc_Tcutil_typ_kind((void*)_tmp529->hd))->kind;if(_tmp5E7 != Cyc_Absyn_EffKind
 && _tmp5E7 != Cyc_Absyn_RgnKind){{void*_tmp5E8=Cyc_Tcutil_compress((void*)
_tmp529->hd);_LL2B3: {struct Cyc_Absyn_VarType_struct*_tmp5E9=(struct Cyc_Absyn_VarType_struct*)
_tmp5E8;if(_tmp5E9->tag != 2)goto _LL2B5;}_LL2B4: goto _LL2B6;_LL2B5: {struct Cyc_Absyn_DatatypeType_struct*
_tmp5EA=(struct Cyc_Absyn_DatatypeType_struct*)_tmp5E8;if(_tmp5EA->tag != 3)goto
_LL2B7;}_LL2B6: continue;_LL2B7:;_LL2B8: e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((
void*)_check_null(e->topt)),_tmp528,0))->r;goto _LL2B2;_LL2B2:;}break;}}goto
_LL227;_LL246: {struct Cyc_Absyn_Cast_e_struct*_tmp52A=(struct Cyc_Absyn_Cast_e_struct*)
_tmp4F8;if(_tmp52A->tag != 13)goto _LL248;else{_tmp52B=(void**)& _tmp52A->f1;
_tmp52C=(void**)((void**)& _tmp52A->f1);_tmp52D=_tmp52A->f2;_tmp52E=_tmp52A->f3;
_tmp52F=_tmp52A->f4;}}_LL247: {void*old_t2=(void*)_check_null(_tmp52D->topt);
void*new_typ=*_tmp52C;*_tmp52C=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,
_tmp52D);{struct _tuple19 _tmpC39;struct _tuple19 _tmp5EC=(_tmpC39.f1=Cyc_Tcutil_compress(
old_t2),((_tmpC39.f2=Cyc_Tcutil_compress(new_typ),_tmpC39)));void*_tmp5ED;struct
Cyc_Absyn_PtrInfo _tmp5EF;void*_tmp5F0;struct Cyc_Absyn_PtrInfo _tmp5F2;void*
_tmp5F3;struct Cyc_Absyn_PtrInfo _tmp5F5;void*_tmp5F6;_LL2BA: _tmp5ED=_tmp5EC.f1;{
struct Cyc_Absyn_PointerType_struct*_tmp5EE=(struct Cyc_Absyn_PointerType_struct*)
_tmp5ED;if(_tmp5EE->tag != 5)goto _LL2BC;else{_tmp5EF=_tmp5EE->f1;}};_tmp5F0=
_tmp5EC.f2;{struct Cyc_Absyn_PointerType_struct*_tmp5F1=(struct Cyc_Absyn_PointerType_struct*)
_tmp5F0;if(_tmp5F1->tag != 5)goto _LL2BC;else{_tmp5F2=_tmp5F1->f1;}};_LL2BB: {int
_tmp5F8=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp5EF.ptr_atts).nullable);int _tmp5F9=((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,(_tmp5F2.ptr_atts).nullable);void*_tmp5FA=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(
_tmp5EF.ptr_atts).bounds);void*_tmp5FB=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5F2.ptr_atts).bounds);int
_tmp5FC=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp5EF.ptr_atts).zero_term);int _tmp5FD=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp5F2.ptr_atts).zero_term);{struct _tuple19 _tmpC3A;
struct _tuple19 _tmp5FF=(_tmpC3A.f1=_tmp5FA,((_tmpC3A.f2=_tmp5FB,_tmpC3A)));void*
_tmp600;struct Cyc_Absyn_Exp*_tmp602;void*_tmp603;struct Cyc_Absyn_Exp*_tmp605;
void*_tmp606;struct Cyc_Absyn_Exp*_tmp608;void*_tmp609;void*_tmp60B;void*_tmp60D;
struct Cyc_Absyn_Exp*_tmp60F;void*_tmp610;void*_tmp612;_LL2C1: _tmp600=_tmp5FF.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp601=(struct Cyc_Absyn_Upper_b_struct*)_tmp600;
if(_tmp601->tag != 1)goto _LL2C3;else{_tmp602=_tmp601->f1;}};_tmp603=_tmp5FF.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp604=(struct Cyc_Absyn_Upper_b_struct*)_tmp603;
if(_tmp604->tag != 1)goto _LL2C3;else{_tmp605=_tmp604->f1;}};_LL2C2: if((!Cyc_Evexp_c_can_eval(
_tmp602) || !Cyc_Evexp_c_can_eval(_tmp605)) && !Cyc_Evexp_same_const_exp(_tmp602,
_tmp605)){const char*_tmpC3D;void*_tmpC3C;(_tmpC3C=0,Cyc_Tcutil_terr(e->loc,((
_tmpC3D="can't validate cast due to potential size differences",_tag_dyneither(
_tmpC3D,sizeof(char),54))),_tag_dyneither(_tmpC3C,sizeof(void*),0)));}if(_tmp5F8
 && !_tmp5F9){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC40;void*_tmpC3F;(
_tmpC3F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC40="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC40,
sizeof(char),44))),_tag_dyneither(_tmpC3F,sizeof(void*),0)));}if(_tmp52F != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC44;void*_tmpC43[1];struct Cyc_String_pa_struct _tmpC42;(_tmpC42.tag=
0,((_tmpC42.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpC43[0]=& _tmpC42,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpC44="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpC44,sizeof(char),41))),_tag_dyneither(_tmpC43,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp52D);if(do_null_check){if(!_tmp52E){
const char*_tmpC47;void*_tmpC46;(_tmpC46=0,Cyc_Tcutil_warn(e->loc,((_tmpC47="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpC47,sizeof(char),58))),_tag_dyneither(_tmpC46,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC48;e->r=Cyc_Toc_cast_it_r(*_tmp52C,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC48=_cycalloc(sizeof(*_tmpC48)),((_tmpC48->hd=
_tmp52D,((_tmpC48->tl=0,_tmpC48)))))),0));};}};}goto _LL2C0;_LL2C3: _tmp606=
_tmp5FF.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp607=(struct Cyc_Absyn_Upper_b_struct*)
_tmp606;if(_tmp607->tag != 1)goto _LL2C5;else{_tmp608=_tmp607->f1;}};_tmp609=
_tmp5FF.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp60A=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp609;if(_tmp60A->tag != 0)goto _LL2C5;};_LL2C4: if(!Cyc_Evexp_c_can_eval(_tmp608)){
const char*_tmpC4B;void*_tmpC4A;(_tmpC4A=0,Cyc_Tcutil_terr(e->loc,((_tmpC4B="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC4B,sizeof(char),71))),_tag_dyneither(_tmpC4A,sizeof(void*),0)));}
if(_tmp52F == Cyc_Absyn_NonNull_to_Null){const char*_tmpC4F;void*_tmpC4E[1];struct
Cyc_String_pa_struct _tmpC4D;(_tmpC4D.tag=0,((_tmpC4D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC4E[0]=& _tmpC4D,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC4F="conversion mis-classified as null-check: %s",
_tag_dyneither(_tmpC4F,sizeof(char),44))),_tag_dyneither(_tmpC4E,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){if((_tmp5FC  && !(_tmp5F2.elt_tq).real_const) && !
_tmp5FD)_tmp608=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp608,Cyc_Absyn_uint_exp(1,
0),0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp608,_tmp52D))->r;}else{
struct Cyc_Absyn_Exp*_tmp623=Cyc_Toc__tag_dyneither_e;if(_tmp5FC){struct _tuple0*
_tmp624=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp625=Cyc_Absyn_var_exp(_tmp624,
0);struct Cyc_Absyn_Exp*arg3;{void*_tmp626=_tmp52D->r;union Cyc_Absyn_Cnst _tmp628;
struct _dyneither_ptr _tmp629;union Cyc_Absyn_Cnst _tmp62B;struct _dyneither_ptr
_tmp62C;_LL2CA: {struct Cyc_Absyn_Const_e_struct*_tmp627=(struct Cyc_Absyn_Const_e_struct*)
_tmp626;if(_tmp627->tag != 0)goto _LL2CC;else{_tmp628=_tmp627->f1;if((_tmp628.String_c).tag
!= 8)goto _LL2CC;_tmp629=(struct _dyneither_ptr)(_tmp628.String_c).val;}}_LL2CB:
arg3=_tmp608;goto _LL2C9;_LL2CC: {struct Cyc_Absyn_Const_e_struct*_tmp62A=(struct
Cyc_Absyn_Const_e_struct*)_tmp626;if(_tmp62A->tag != 0)goto _LL2CE;else{_tmp62B=
_tmp62A->f1;if((_tmp62B.Wstring_c).tag != 9)goto _LL2CE;_tmp62C=(struct
_dyneither_ptr)(_tmp62B.Wstring_c).val;}}_LL2CD: arg3=_tmp608;goto _LL2C9;_LL2CE:;
_LL2CF:{struct Cyc_Absyn_Exp*_tmpC50[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp52D),((_tmpC50[1]=_tmp608,((_tmpC50[0]=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp625),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC50,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);}goto _LL2C9;_LL2C9:;}if(!_tmp5FD  && !(_tmp5F2.elt_tq).real_const)arg3=
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*
_tmp62E=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5F2.elt_typ),0);struct Cyc_Absyn_Exp*
_tmpC51[3];struct Cyc_Absyn_Exp*_tmp62F=Cyc_Absyn_fncall_exp(_tmp623,((_tmpC51[2]=
arg3,((_tmpC51[1]=_tmp62E,((_tmpC51[0]=_tmp625,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC51,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);struct Cyc_Absyn_Stmt*_tmp630=Cyc_Absyn_exp_stmt(_tmp62F,0);_tmp630=
Cyc_Absyn_declare_stmt(_tmp624,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)
_tmp52D,_tmp630,0);e->r=Cyc_Toc_stmt_exp_r(_tmp630);};}else{struct Cyc_Absyn_Exp*
_tmpC52[3];e->r=Cyc_Toc_fncall_exp_r(_tmp623,((_tmpC52[2]=_tmp608,((_tmpC52[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5F2.elt_typ),0),((_tmpC52[0]=
_tmp52D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC52,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2C0;
_LL2C5: _tmp60B=_tmp5FF.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp60C=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp60B;if(_tmp60C->tag != 0)goto _LL2C7;};_tmp60D=_tmp5FF.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp60E=(struct Cyc_Absyn_Upper_b_struct*)_tmp60D;if(_tmp60E->tag != 1)goto _LL2C7;
else{_tmp60F=_tmp60E->f1;}};_LL2C6: if(!Cyc_Evexp_c_can_eval(_tmp60F)){const char*
_tmpC55;void*_tmpC54;(_tmpC54=0,Cyc_Tcutil_terr(e->loc,((_tmpC55="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC55,sizeof(char),71))),_tag_dyneither(_tmpC54,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC58;void*_tmpC57;(_tmpC57=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC58="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpC58,sizeof(char),45))),_tag_dyneither(_tmpC57,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp637=_tmp60F;if(_tmp5FC  && !_tmp5FD)_tmp637=Cyc_Absyn_add_exp(
_tmp60F,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp638=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC59[3];struct Cyc_Absyn_Exp*_tmp639=Cyc_Absyn_fncall_exp(
_tmp638,((_tmpC59[2]=_tmp637,((_tmpC59[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp5EF.elt_typ),0),((_tmpC59[0]=_tmp52D,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC59,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp5F9){struct Cyc_List_List*_tmpC5A;_tmp639->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_null_e,((_tmpC5A=_cycalloc(sizeof(*_tmpC5A)),((_tmpC5A->hd=Cyc_Absyn_copy_exp(
_tmp639),((_tmpC5A->tl=0,_tmpC5A)))))));}e->r=Cyc_Toc_cast_it_r(*_tmp52C,_tmp639);
goto _LL2C0;};};_LL2C7: _tmp610=_tmp5FF.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp611=(struct Cyc_Absyn_DynEither_b_struct*)_tmp610;if(_tmp611->tag != 0)goto
_LL2C0;};_tmp612=_tmp5FF.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp613=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp612;if(_tmp613->tag != 0)goto _LL2C0;};_LL2C8:
DynCast: if((_tmp5FC  && !_tmp5FD) && !(_tmp5F2.elt_tq).real_const){if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpC5D;void*_tmpC5C;(_tmpC5C=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC5D="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpC5D,sizeof(char),70))),_tag_dyneither(_tmpC5C,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp63E=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpC5E[3];e->r=Cyc_Toc_fncall_exp_r(_tmp63E,((_tmpC5E[2]=Cyc_Absyn_uint_exp(1,0),((
_tmpC5E[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5EF.elt_typ),0),((
_tmpC5E[0]=_tmp52D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC5E,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}goto _LL2C0;
_LL2C0:;}goto _LL2B9;}_LL2BC: _tmp5F3=_tmp5EC.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp5F4=(struct Cyc_Absyn_PointerType_struct*)_tmp5F3;if(_tmp5F4->tag != 5)goto
_LL2BE;else{_tmp5F5=_tmp5F4->f1;}};_tmp5F6=_tmp5EC.f2;{struct Cyc_Absyn_IntType_struct*
_tmp5F7=(struct Cyc_Absyn_IntType_struct*)_tmp5F6;if(_tmp5F7->tag != 6)goto _LL2BE;};
_LL2BD:{void*_tmp640=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp5F5.ptr_atts).bounds);_LL2D1: {struct Cyc_Absyn_DynEither_b_struct*
_tmp641=(struct Cyc_Absyn_DynEither_b_struct*)_tmp640;if(_tmp641->tag != 0)goto
_LL2D3;}_LL2D2: _tmp52D->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp52D->r,
_tmp52D->loc),Cyc_Toc_curr_sp);goto _LL2D0;_LL2D3:;_LL2D4: goto _LL2D0;_LL2D0:;}
goto _LL2B9;_LL2BE:;_LL2BF: goto _LL2B9;_LL2B9:;}goto _LL227;}_LL248: {struct Cyc_Absyn_Address_e_struct*
_tmp530=(struct Cyc_Absyn_Address_e_struct*)_tmp4F8;if(_tmp530->tag != 14)goto
_LL24A;else{_tmp531=_tmp530->f1;}}_LL249:{void*_tmp642=_tmp531->r;struct _tuple0*
_tmp644;struct Cyc_List_List*_tmp645;struct Cyc_List_List*_tmp646;struct Cyc_List_List*
_tmp648;_LL2D6: {struct Cyc_Absyn_Aggregate_e_struct*_tmp643=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp642;if(_tmp643->tag != 27)goto _LL2D8;else{_tmp644=_tmp643->f1;_tmp645=_tmp643->f2;
_tmp646=_tmp643->f3;}}_LL2D7: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC62;void*
_tmpC61[1];struct Cyc_String_pa_struct _tmpC60;(_tmpC60.tag=0,((_tmpC60.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp531->loc)),((
_tmpC61[0]=& _tmpC60,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC62="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC62,
sizeof(char),42))),_tag_dyneither(_tmpC61,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_struct(
nv,(void*)_check_null(_tmp531->topt),_tmp645,1,0,_tmp646,_tmp644))->r;goto _LL2D5;
_LL2D8: {struct Cyc_Absyn_Tuple_e_struct*_tmp647=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp642;if(_tmp647->tag != 23)goto _LL2DA;else{_tmp648=_tmp647->f1;}}_LL2D9: if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpC66;void*_tmpC65[1];struct Cyc_String_pa_struct _tmpC64;(
_tmpC64.tag=0,((_tmpC64.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
_tmp531->loc)),((_tmpC65[0]=& _tmpC64,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC66="%s: & on non-identifiers at the top-level",
_tag_dyneither(_tmpC66,sizeof(char),42))),_tag_dyneither(_tmpC65,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp648))->r;goto _LL2D5;_LL2DA:;_LL2DB: Cyc_Toc_set_lhs(
nv,1);Cyc_Toc_exp_to_c(nv,_tmp531);Cyc_Toc_set_lhs(nv,0);if(!Cyc_Absyn_is_lvalue(
_tmp531)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp531,0,Cyc_Toc_address_lvalue,
1);e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp531);}
goto _LL2D5;_LL2D5:;}goto _LL227;_LL24A: {struct Cyc_Absyn_New_e_struct*_tmp532=(
struct Cyc_Absyn_New_e_struct*)_tmp4F8;if(_tmp532->tag != 15)goto _LL24C;else{
_tmp533=_tmp532->f1;_tmp534=_tmp532->f2;}}_LL24B: if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC6A;void*_tmpC69[1];struct Cyc_String_pa_struct _tmpC68;(_tmpC68.tag=
0,((_tmpC68.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
_tmp534->loc)),((_tmpC69[0]=& _tmpC68,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC6A="%s: new at top-level",_tag_dyneither(
_tmpC6A,sizeof(char),21))),_tag_dyneither(_tmpC69,sizeof(void*),1)))))));}{void*
_tmp652=_tmp534->r;struct Cyc_List_List*_tmp654;struct Cyc_Absyn_Vardecl*_tmp656;
struct Cyc_Absyn_Exp*_tmp657;struct Cyc_Absyn_Exp*_tmp658;int _tmp659;struct _tuple0*
_tmp65B;struct Cyc_List_List*_tmp65C;struct Cyc_List_List*_tmp65D;struct Cyc_Absyn_Aggrdecl*
_tmp65E;struct Cyc_List_List*_tmp660;_LL2DD: {struct Cyc_Absyn_Array_e_struct*
_tmp653=(struct Cyc_Absyn_Array_e_struct*)_tmp652;if(_tmp653->tag != 25)goto _LL2DF;
else{_tmp654=_tmp653->f1;}}_LL2DE: {struct _tuple0*_tmp661=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp662=Cyc_Absyn_var_exp(_tmp661,0);struct Cyc_Absyn_Stmt*
_tmp663=Cyc_Toc_init_array(nv,_tmp662,_tmp654,Cyc_Absyn_exp_stmt(_tmp662,0));
void*old_elt_typ;{void*_tmp664=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp666;void*_tmp667;struct Cyc_Absyn_Tqual _tmp668;struct Cyc_Absyn_PtrAtts _tmp669;
union Cyc_Absyn_Constraint*_tmp66A;_LL2E8: {struct Cyc_Absyn_PointerType_struct*
_tmp665=(struct Cyc_Absyn_PointerType_struct*)_tmp664;if(_tmp665->tag != 5)goto
_LL2EA;else{_tmp666=_tmp665->f1;_tmp667=_tmp666.elt_typ;_tmp668=_tmp666.elt_tq;
_tmp669=_tmp666.ptr_atts;_tmp66A=_tmp669.zero_term;}}_LL2E9: old_elt_typ=_tmp667;
goto _LL2E7;_LL2EA:;_LL2EB: {const char*_tmpC6D;void*_tmpC6C;old_elt_typ=((_tmpC6C=
0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC6D="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(
_tmpC6D,sizeof(char),52))),_tag_dyneither(_tmpC6C,sizeof(void*),0))));}_LL2E7:;}{
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp66D=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp66E=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp654),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp533 == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp66E);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp533;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp66E);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp661,_tmp66D,(struct Cyc_Absyn_Exp*)e1,_tmp663,0));goto
_LL2DC;};}_LL2DF: {struct Cyc_Absyn_Comprehension_e_struct*_tmp655=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp652;if(_tmp655->tag != 26)goto _LL2E1;else{_tmp656=_tmp655->f1;_tmp657=_tmp655->f2;
_tmp658=_tmp655->f3;_tmp659=_tmp655->f4;}}_LL2E0: {int is_dyneither_ptr=0;{void*
_tmp66F=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp671;void*_tmp672;
struct Cyc_Absyn_Tqual _tmp673;struct Cyc_Absyn_PtrAtts _tmp674;union Cyc_Absyn_Constraint*
_tmp675;union Cyc_Absyn_Constraint*_tmp676;_LL2ED: {struct Cyc_Absyn_PointerType_struct*
_tmp670=(struct Cyc_Absyn_PointerType_struct*)_tmp66F;if(_tmp670->tag != 5)goto
_LL2EF;else{_tmp671=_tmp670->f1;_tmp672=_tmp671.elt_typ;_tmp673=_tmp671.elt_tq;
_tmp674=_tmp671.ptr_atts;_tmp675=_tmp674.bounds;_tmp676=_tmp674.zero_term;}}
_LL2EE: is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp675)== (void*)& Cyc_Absyn_DynEither_b_val;goto _LL2EC;
_LL2EF:;_LL2F0: {const char*_tmpC70;void*_tmpC6F;(_tmpC6F=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC70="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpC70,sizeof(char),42))),_tag_dyneither(_tmpC6F,sizeof(void*),0)));}
_LL2EC:;}{struct _tuple0*max=Cyc_Toc_temp_var();struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)_check_null(_tmp658->topt);void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(
nv,_tmp657);{struct Cyc_Absyn_Exp*_tmp679=Cyc_Absyn_var_exp(max,0);if(_tmp659)
_tmp679=Cyc_Absyn_add_exp(_tmp679,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Stmt*
s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(a,0),_tmp656,Cyc_Absyn_var_exp(
max,0),_tmp658,_tmp659,Cyc_Toc_skip_stmt_dl(),1);struct _RegionHandle _tmp67A=
_new_region("r");struct _RegionHandle*r=& _tmp67A;_push_region(r);{struct _tuple21*
_tmpC73;struct Cyc_List_List*_tmpC72;struct Cyc_List_List*decls=(_tmpC72=
_region_malloc(r,sizeof(*_tmpC72)),((_tmpC72->hd=((_tmpC73=_region_malloc(r,
sizeof(*_tmpC73)),((_tmpC73->f1=max,((_tmpC73->f2=Cyc_Absyn_uint_typ,((_tmpC73->f3=(
struct Cyc_Absyn_Exp*)_tmp657,_tmpC73)))))))),((_tmpC72->tl=0,_tmpC72)))));struct
Cyc_Absyn_Exp*ai;if(_tmp533 == 0  || Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*
_tmpC74[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((
_tmpC74[1]=_tmp679,((_tmpC74[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC74,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp533;Cyc_Toc_exp_to_c(
nv,r);{struct Cyc_Absyn_Exp*_tmpC75[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpC75[1]=_tmp679,((_tmpC75[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC75,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct _tuple21*_tmpC78;struct Cyc_List_List*
_tmpC77;decls=((_tmpC77=_region_malloc(r,sizeof(*_tmpC77)),((_tmpC77->hd=((
_tmpC78=_region_malloc(r,sizeof(*_tmpC78)),((_tmpC78->f1=a,((_tmpC78->f2=ptr_typ,((
_tmpC78->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpC78)))))))),((_tmpC77->tl=decls,
_tmpC77))))));}if(is_dyneither_ptr){struct _tuple0*_tmp67F=Cyc_Toc_temp_var();
void*_tmp680=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp681=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC79[3];struct Cyc_Absyn_Exp*_tmp682=Cyc_Absyn_fncall_exp(
_tmp681,((_tmpC79[2]=_tmp679,((_tmpC79[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((
_tmpC79[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC79,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{
struct _tuple21*_tmpC7C;struct Cyc_List_List*_tmpC7B;decls=((_tmpC7B=
_region_malloc(r,sizeof(*_tmpC7B)),((_tmpC7B->hd=((_tmpC7C=_region_malloc(r,
sizeof(*_tmpC7C)),((_tmpC7C->f1=_tmp67F,((_tmpC7C->f2=_tmp680,((_tmpC7C->f3=(
struct Cyc_Absyn_Exp*)_tmp682,_tmpC7C)))))))),((_tmpC7B->tl=decls,_tmpC7B))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp67F,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp686=decls;for(0;_tmp686 != 0;_tmp686=_tmp686->tl){struct
_tuple0*_tmp688;void*_tmp689;struct Cyc_Absyn_Exp*_tmp68A;struct _tuple21 _tmp687=*((
struct _tuple21*)_tmp686->hd);_tmp688=_tmp687.f1;_tmp689=_tmp687.f2;_tmp68A=
_tmp687.f3;s=Cyc_Absyn_declare_stmt(_tmp688,_tmp689,_tmp68A,s,0);}}e->r=Cyc_Toc_stmt_exp_r(
s);};}_npop_handler(0);goto _LL2DC;;_pop_region(r);};};};}_LL2E1: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp65A=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp652;if(_tmp65A->tag != 27)goto
_LL2E3;else{_tmp65B=_tmp65A->f1;_tmp65C=_tmp65A->f2;_tmp65D=_tmp65A->f3;_tmp65E=
_tmp65A->f4;}}_LL2E2: e->r=(Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp534->topt),
_tmp65C,1,_tmp533,_tmp65D,_tmp65B))->r;goto _LL2DC;_LL2E3: {struct Cyc_Absyn_Tuple_e_struct*
_tmp65F=(struct Cyc_Absyn_Tuple_e_struct*)_tmp652;if(_tmp65F->tag != 23)goto _LL2E5;
else{_tmp660=_tmp65F->f1;}}_LL2E4: e->r=(Cyc_Toc_init_tuple(nv,1,_tmp533,_tmp660))->r;
goto _LL2DC;_LL2E5:;_LL2E6: {void*old_elt_typ=(void*)_check_null(_tmp534->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct _tuple0*_tmp68D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp68E=Cyc_Absyn_var_exp(_tmp68D,0);struct Cyc_Absyn_Exp*
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp68E,0),0);struct Cyc_Absyn_Exp*
inner_mexp=mexp;if(_tmp533 == 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(
old_elt_typ,mexp);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp533;Cyc_Toc_exp_to_c(
nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp68F=_tmp534->r;void*
_tmp691;struct Cyc_Absyn_Exp*_tmp692;_LL2F2: {struct Cyc_Absyn_Cast_e_struct*
_tmp690=(struct Cyc_Absyn_Cast_e_struct*)_tmp68F;if(_tmp690->tag != 13)goto _LL2F4;
else{_tmp691=(void*)_tmp690->f1;_tmp692=_tmp690->f2;}}_LL2F3:{struct _tuple19
_tmpC7D;struct _tuple19 _tmp694=(_tmpC7D.f1=Cyc_Tcutil_compress(_tmp691),((_tmpC7D.f2=
Cyc_Tcutil_compress((void*)_check_null(_tmp692->topt)),_tmpC7D)));void*_tmp695;
struct Cyc_Absyn_PtrInfo _tmp697;void*_tmp698;struct Cyc_Absyn_PtrAtts _tmp699;union
Cyc_Absyn_Constraint*_tmp69A;void*_tmp69B;struct Cyc_Absyn_PtrInfo _tmp69D;struct
Cyc_Absyn_PtrAtts _tmp69E;union Cyc_Absyn_Constraint*_tmp69F;_LL2F7: _tmp695=
_tmp694.f1;{struct Cyc_Absyn_PointerType_struct*_tmp696=(struct Cyc_Absyn_PointerType_struct*)
_tmp695;if(_tmp696->tag != 5)goto _LL2F9;else{_tmp697=_tmp696->f1;_tmp698=_tmp697.elt_typ;
_tmp699=_tmp697.ptr_atts;_tmp69A=_tmp699.bounds;}};_tmp69B=_tmp694.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp69C=(struct Cyc_Absyn_PointerType_struct*)_tmp69B;if(_tmp69C->tag != 5)goto
_LL2F9;else{_tmp69D=_tmp69C->f1;_tmp69E=_tmp69D.ptr_atts;_tmp69F=_tmp69E.bounds;}};
_LL2F8:{struct _tuple19 _tmpC7E;struct _tuple19 _tmp6A1=(_tmpC7E.f1=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp69A),((_tmpC7E.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp69F),_tmpC7E)));void*_tmp6A2;void*_tmp6A4;struct Cyc_Absyn_Exp*
_tmp6A6;_LL2FC: _tmp6A2=_tmp6A1.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp6A3=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp6A2;if(_tmp6A3->tag != 0)goto _LL2FE;};
_tmp6A4=_tmp6A1.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp6A5=(struct Cyc_Absyn_Upper_b_struct*)
_tmp6A4;if(_tmp6A5->tag != 1)goto _LL2FE;else{_tmp6A6=_tmp6A5->f1;}};_LL2FD: Cyc_Toc_exp_to_c(
nv,_tmp692);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp6A7=Cyc_Toc__init_dyneither_ptr_e;{struct Cyc_Absyn_Exp*_tmpC7F[4];e->r=Cyc_Toc_fncall_exp_r(
_tmp6A7,((_tmpC7F[3]=_tmp6A6,((_tmpC7F[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp698),0),((_tmpC7F[1]=_tmp692,((_tmpC7F[0]=mexp,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC7F,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))));}goto _LL2FB;};_LL2FE:;_LL2FF: goto _LL2FB;_LL2FB:;}goto _LL2F6;_LL2F9:;
_LL2FA: goto _LL2F6;_LL2F6:;}goto _LL2F1;_LL2F4:;_LL2F5: goto _LL2F1;_LL2F1:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp6A9=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp68E),
0);struct Cyc_Absyn_Exp*_tmp6AA=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,
_tmp534);_tmp6A9=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
_tmp68E,_tmp6AA,0),_tmp534,0),_tmp6A9,0);{void*_tmp6AB=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp68D,
_tmp6AB,(struct Cyc_Absyn_Exp*)mexp,_tmp6A9,0));};}goto _LL2DC;};}_LL2DC:;}goto
_LL227;_LL24C: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp535=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp4F8;if(_tmp535->tag != 17)goto _LL24E;else{_tmp536=_tmp535->f1;}}_LL24D: Cyc_Toc_exp_to_c(
nv,_tmp536);goto _LL227;_LL24E: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp537=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp4F8;if(_tmp537->tag != 16)goto _LL250;
else{_tmp538=(void*)_tmp537->f1;}}_LL24F:{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpC82;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpC81;e->r=(void*)((_tmpC81=
_cycalloc(sizeof(*_tmpC81)),((_tmpC81[0]=((_tmpC82.tag=16,((_tmpC82.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp538),_tmpC82)))),_tmpC81))));}goto _LL227;_LL250: {
struct Cyc_Absyn_Offsetof_e_struct*_tmp539=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp4F8;if(_tmp539->tag != 18)goto _LL252;else{_tmp53A=(void*)_tmp539->f1;_tmp53B=(
void*)_tmp539->f2;{struct Cyc_Absyn_StructField_struct*_tmp53C=(struct Cyc_Absyn_StructField_struct*)
_tmp53B;if(_tmp53C->tag != 0)goto _LL252;else{_tmp53D=_tmp53C->f1;}};}}_LL251:{
struct Cyc_Absyn_Offsetof_e_struct _tmpC8C;struct Cyc_Absyn_StructField_struct
_tmpC8B;struct Cyc_Absyn_StructField_struct*_tmpC8A;struct Cyc_Absyn_Offsetof_e_struct*
_tmpC89;e->r=(void*)((_tmpC89=_cycalloc(sizeof(*_tmpC89)),((_tmpC89[0]=((_tmpC8C.tag=
18,((_tmpC8C.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp53A),((_tmpC8C.f2=(void*)((
void*)((_tmpC8A=_cycalloc(sizeof(*_tmpC8A)),((_tmpC8A[0]=((_tmpC8B.tag=0,((
_tmpC8B.f1=_tmp53D,_tmpC8B)))),_tmpC8A))))),_tmpC8C)))))),_tmpC89))));}goto
_LL227;_LL252: {struct Cyc_Absyn_Offsetof_e_struct*_tmp53E=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp4F8;if(_tmp53E->tag != 18)goto _LL254;else{_tmp53F=(void*)_tmp53E->f1;_tmp540=(
void*)_tmp53E->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp541=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp540;if(_tmp541->tag != 1)goto _LL254;else{_tmp542=_tmp541->f1;}};}}_LL253:{
void*_tmp6B2=Cyc_Tcutil_compress(_tmp53F);struct Cyc_Absyn_AggrInfo _tmp6B4;union
Cyc_Absyn_AggrInfoU _tmp6B5;struct Cyc_List_List*_tmp6B7;_LL301: {struct Cyc_Absyn_AggrType_struct*
_tmp6B3=(struct Cyc_Absyn_AggrType_struct*)_tmp6B2;if(_tmp6B3->tag != 11)goto
_LL303;else{_tmp6B4=_tmp6B3->f1;_tmp6B5=_tmp6B4.aggr_info;}}_LL302: {struct Cyc_Absyn_Aggrdecl*
_tmp6BA=Cyc_Absyn_get_known_aggrdecl(_tmp6B5);if(_tmp6BA->impl == 0){const char*
_tmpC8F;void*_tmpC8E;(_tmpC8E=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC8F="struct fields must be known",
_tag_dyneither(_tmpC8F,sizeof(char),28))),_tag_dyneither(_tmpC8E,sizeof(void*),0)));}
_tmp6B7=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6BA->impl))->fields;goto
_LL304;}_LL303: {struct Cyc_Absyn_AnonAggrType_struct*_tmp6B6=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp6B2;if(_tmp6B6->tag != 12)goto _LL305;else{_tmp6B7=_tmp6B6->f2;}}_LL304: {
struct Cyc_Absyn_Aggrfield*_tmp6BD=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp6B7,(int)_tmp542);{struct Cyc_Absyn_Offsetof_e_struct
_tmpC99;struct Cyc_Absyn_StructField_struct _tmpC98;struct Cyc_Absyn_StructField_struct*
_tmpC97;struct Cyc_Absyn_Offsetof_e_struct*_tmpC96;e->r=(void*)((_tmpC96=
_cycalloc(sizeof(*_tmpC96)),((_tmpC96[0]=((_tmpC99.tag=18,((_tmpC99.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp53F),((_tmpC99.f2=(void*)((void*)((_tmpC97=_cycalloc(
sizeof(*_tmpC97)),((_tmpC97[0]=((_tmpC98.tag=0,((_tmpC98.f1=_tmp6BD->name,
_tmpC98)))),_tmpC97))))),_tmpC99)))))),_tmpC96))));}goto _LL300;}_LL305: {struct
Cyc_Absyn_TupleType_struct*_tmp6B8=(struct Cyc_Absyn_TupleType_struct*)_tmp6B2;
if(_tmp6B8->tag != 10)goto _LL307;}_LL306:{struct Cyc_Absyn_Offsetof_e_struct
_tmpCA3;struct Cyc_Absyn_StructField_struct _tmpCA2;struct Cyc_Absyn_StructField_struct*
_tmpCA1;struct Cyc_Absyn_Offsetof_e_struct*_tmpCA0;e->r=(void*)((_tmpCA0=
_cycalloc(sizeof(*_tmpCA0)),((_tmpCA0[0]=((_tmpCA3.tag=18,((_tmpCA3.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp53F),((_tmpCA3.f2=(void*)((void*)((_tmpCA1=_cycalloc(
sizeof(*_tmpCA1)),((_tmpCA1[0]=((_tmpCA2.tag=0,((_tmpCA2.f1=Cyc_Absyn_fieldname((
int)(_tmp542 + 1)),_tmpCA2)))),_tmpCA1))))),_tmpCA3)))))),_tmpCA0))));}goto _LL300;
_LL307: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp6B9=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp6B2;if(_tmp6B9->tag != 4)goto _LL309;}_LL308: if(_tmp542 == 0){struct Cyc_Absyn_Offsetof_e_struct
_tmpCAD;struct Cyc_Absyn_StructField_struct _tmpCAC;struct Cyc_Absyn_StructField_struct*
_tmpCAB;struct Cyc_Absyn_Offsetof_e_struct*_tmpCAA;e->r=(void*)((_tmpCAA=
_cycalloc(sizeof(*_tmpCAA)),((_tmpCAA[0]=((_tmpCAD.tag=18,((_tmpCAD.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp53F),((_tmpCAD.f2=(void*)((void*)((_tmpCAB=_cycalloc(
sizeof(*_tmpCAB)),((_tmpCAB[0]=((_tmpCAC.tag=0,((_tmpCAC.f1=Cyc_Toc_tag_sp,
_tmpCAC)))),_tmpCAB))))),_tmpCAD)))))),_tmpCAA))));}else{struct Cyc_Absyn_Offsetof_e_struct
_tmpCB7;struct Cyc_Absyn_StructField_struct _tmpCB6;struct Cyc_Absyn_StructField_struct*
_tmpCB5;struct Cyc_Absyn_Offsetof_e_struct*_tmpCB4;e->r=(void*)((_tmpCB4=
_cycalloc(sizeof(*_tmpCB4)),((_tmpCB4[0]=((_tmpCB7.tag=18,((_tmpCB7.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp53F),((_tmpCB7.f2=(void*)((void*)((_tmpCB5=_cycalloc(
sizeof(*_tmpCB5)),((_tmpCB5[0]=((_tmpCB6.tag=0,((_tmpCB6.f1=Cyc_Absyn_fieldname((
int)_tmp542),_tmpCB6)))),_tmpCB5))))),_tmpCB7)))))),_tmpCB4))));}goto _LL300;
_LL309:;_LL30A: {const char*_tmpCBA;void*_tmpCB9;(_tmpCB9=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCBA="impossible type for offsetof tuple index",
_tag_dyneither(_tmpCBA,sizeof(char),41))),_tag_dyneither(_tmpCB9,sizeof(void*),0)));}
_LL300:;}goto _LL227;_LL254: {struct Cyc_Absyn_Deref_e_struct*_tmp543=(struct Cyc_Absyn_Deref_e_struct*)
_tmp4F8;if(_tmp543->tag != 19)goto _LL256;else{_tmp544=_tmp543->f1;}}_LL255: {int
_tmp6D0=Cyc_Toc_in_lhs(nv);Cyc_Toc_set_lhs(nv,0);{void*_tmp6D1=Cyc_Tcutil_compress((
void*)_check_null(_tmp544->topt));{void*_tmp6D2=_tmp6D1;struct Cyc_Absyn_PtrInfo
_tmp6D4;void*_tmp6D5;struct Cyc_Absyn_Tqual _tmp6D6;struct Cyc_Absyn_PtrAtts _tmp6D7;
void*_tmp6D8;union Cyc_Absyn_Constraint*_tmp6D9;union Cyc_Absyn_Constraint*_tmp6DA;
union Cyc_Absyn_Constraint*_tmp6DB;_LL30C: {struct Cyc_Absyn_PointerType_struct*
_tmp6D3=(struct Cyc_Absyn_PointerType_struct*)_tmp6D2;if(_tmp6D3->tag != 5)goto
_LL30E;else{_tmp6D4=_tmp6D3->f1;_tmp6D5=_tmp6D4.elt_typ;_tmp6D6=_tmp6D4.elt_tq;
_tmp6D7=_tmp6D4.ptr_atts;_tmp6D8=_tmp6D7.rgn;_tmp6D9=_tmp6D7.nullable;_tmp6DA=
_tmp6D7.bounds;_tmp6DB=_tmp6D7.zero_term;}}_LL30D:{void*_tmp6DC=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6DA);struct Cyc_Absyn_Exp*_tmp6DE;_LL311: {struct Cyc_Absyn_Upper_b_struct*
_tmp6DD=(struct Cyc_Absyn_Upper_b_struct*)_tmp6DC;if(_tmp6DD->tag != 1)goto _LL313;
else{_tmp6DE=_tmp6DD->f1;}}_LL312: {int do_null_check=Cyc_Toc_need_null_check(
_tmp544);Cyc_Toc_exp_to_c(nv,_tmp544);if(do_null_check){if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCBD;void*_tmpCBC;(_tmpCBC=0,Cyc_Tcutil_warn(e->loc,((_tmpCBD="inserted null check due to dereference",
_tag_dyneither(_tmpCBD,sizeof(char),39))),_tag_dyneither(_tmpCBC,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpCBE;_tmp544->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp6D1),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpCBE=_cycalloc(sizeof(*
_tmpCBE)),((_tmpCBE->hd=Cyc_Absyn_copy_exp(_tmp544),((_tmpCBE->tl=0,_tmpCBE)))))),
0));};}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6DB)){unsigned int _tmp6E4;int _tmp6E5;struct _tuple11 _tmp6E3=Cyc_Evexp_eval_const_uint_exp(
_tmp6DE);_tmp6E4=_tmp6E3.f1;_tmp6E5=_tmp6E3.f2;if(!_tmp6E5  || _tmp6E4 <= 0){const
char*_tmpCC1;void*_tmpCC0;(_tmpCC0=0,Cyc_Tcutil_terr(e->loc,((_tmpCC1="cannot determine dereference is in bounds",
_tag_dyneither(_tmpCC1,sizeof(char),42))),_tag_dyneither(_tmpCC0,sizeof(void*),0)));}}
goto _LL310;}_LL313: {struct Cyc_Absyn_DynEither_b_struct*_tmp6DF=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp6DC;if(_tmp6DF->tag != 0)goto _LL310;}_LL314: {struct Cyc_Absyn_Exp*_tmp6E8=Cyc_Absyn_uint_exp(
0,0);_tmp6E8->topt=(void*)Cyc_Absyn_uint_typ;e->r=Cyc_Toc_subscript_exp_r(
_tmp544,_tmp6E8);Cyc_Toc_exp_to_c(nv,e);goto _LL310;}_LL310:;}goto _LL30B;_LL30E:;
_LL30F: {const char*_tmpCC4;void*_tmpCC3;(_tmpCC3=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCC4="exp_to_c: Deref: non-pointer",
_tag_dyneither(_tmpCC4,sizeof(char),29))),_tag_dyneither(_tmpCC3,sizeof(void*),0)));}
_LL30B:;}Cyc_Toc_set_lhs(nv,_tmp6D0);goto _LL227;};}_LL256: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp545=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp4F8;if(_tmp545->tag != 20)goto
_LL258;else{_tmp546=_tmp545->f1;_tmp547=_tmp545->f2;_tmp548=_tmp545->f3;_tmp549=
_tmp545->f4;}}_LL257: {int is_poly=Cyc_Toc_is_poly_project(e);void*e1_cyc_type=(
void*)_check_null(_tmp546->topt);Cyc_Toc_exp_to_c(nv,_tmp546);if(_tmp548  && 
_tmp549)e->r=Cyc_Toc_check_tagged_union(_tmp546,Cyc_Toc_typ_to_c(e1_cyc_type),
e1_cyc_type,_tmp547,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrmember_exp);if(is_poly)e->r=(
Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL227;}_LL258: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp54A=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp4F8;if(_tmp54A->tag != 21)goto _LL25A;
else{_tmp54B=_tmp54A->f1;_tmp54C=_tmp54A->f2;_tmp54D=_tmp54A->f3;_tmp54E=_tmp54A->f4;}}
_LL259: {int _tmp6EB=Cyc_Toc_in_lhs(nv);Cyc_Toc_set_lhs(nv,0);{void*e1typ=Cyc_Tcutil_compress((
void*)_check_null(_tmp54B->topt));int do_null_check=Cyc_Toc_need_null_check(
_tmp54B);Cyc_Toc_exp_to_c(nv,_tmp54B);{int is_poly=Cyc_Toc_is_poly_project(e);
void*_tmp6ED;struct Cyc_Absyn_Tqual _tmp6EE;struct Cyc_Absyn_PtrAtts _tmp6EF;void*
_tmp6F0;union Cyc_Absyn_Constraint*_tmp6F1;union Cyc_Absyn_Constraint*_tmp6F2;
union Cyc_Absyn_Constraint*_tmp6F3;struct Cyc_Absyn_PtrInfo _tmp6EC=Cyc_Toc_get_ptr_type(
e1typ);_tmp6ED=_tmp6EC.elt_typ;_tmp6EE=_tmp6EC.elt_tq;_tmp6EF=_tmp6EC.ptr_atts;
_tmp6F0=_tmp6EF.rgn;_tmp6F1=_tmp6EF.nullable;_tmp6F2=_tmp6EF.bounds;_tmp6F3=
_tmp6EF.zero_term;{void*_tmp6F4=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6F2);struct Cyc_Absyn_Exp*_tmp6F6;
_LL316: {struct Cyc_Absyn_Upper_b_struct*_tmp6F5=(struct Cyc_Absyn_Upper_b_struct*)
_tmp6F4;if(_tmp6F5->tag != 1)goto _LL318;else{_tmp6F6=_tmp6F5->f1;}}_LL317: {
unsigned int _tmp6F9;int _tmp6FA;struct _tuple11 _tmp6F8=Cyc_Evexp_eval_const_uint_exp(
_tmp6F6);_tmp6F9=_tmp6F8.f1;_tmp6FA=_tmp6F8.f2;if(_tmp6FA){if(_tmp6F9 < 1){const
char*_tmpCC7;void*_tmpCC6;(_tmpCC6=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCC7="exp_to_c:  AggrArrow_e on pointer of size 0",
_tag_dyneither(_tmpCC7,sizeof(char),44))),_tag_dyneither(_tmpCC6,sizeof(void*),0)));}
if(do_null_check){if(Cyc_Toc_warn_all_null_deref){const char*_tmpCCA;void*_tmpCC9;(
_tmpCC9=0,Cyc_Tcutil_warn(e->loc,((_tmpCCA="inserted null check due to dereference",
_tag_dyneither(_tmpCCA,sizeof(char),39))),_tag_dyneither(_tmpCC9,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpCCB[1];_tmp54B->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpCCB[0]=Cyc_Absyn_new_exp(
_tmp54B->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCCB,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{if(!Cyc_Evexp_c_can_eval(
_tmp6F6)){const char*_tmpCCE;void*_tmpCCD;(_tmpCCD=0,Cyc_Tcutil_terr(e->loc,((
_tmpCCE="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpCCE,
sizeof(char),47))),_tag_dyneither(_tmpCCD,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCCF[4];_tmp54B->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,((_tmpCCF[3]=Cyc_Absyn_uint_exp(0,0),((
_tmpCCF[2]=Cyc_Absyn_sizeoftyp_exp(_tmp6ED,0),((_tmpCCF[1]=_tmp6F6,((_tmpCCF[0]=
Cyc_Absyn_new_exp(_tmp54B->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpCCF,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
goto _LL315;}_LL318: {struct Cyc_Absyn_DynEither_b_struct*_tmp6F7=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp6F4;if(_tmp6F7->tag != 0)goto _LL315;}_LL319: {void*ta1=Cyc_Toc_typ_to_c_array(
_tmp6ED);{struct Cyc_Absyn_Exp*_tmpCD0[3];_tmp54B->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(
ta1,_tmp6EE),Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,((_tmpCD0[
2]=Cyc_Absyn_uint_exp(0,0),((_tmpCD0[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCD0[
0]=Cyc_Absyn_new_exp(_tmp54B->r,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD0,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0));}goto _LL315;}_LL315:;}if(_tmp54D  && _tmp54E)e->r=Cyc_Toc_check_tagged_union(
_tmp54B,Cyc_Toc_typ_to_c(e1typ),_tmp6ED,_tmp54C,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
if(is_poly  && _tmp54E)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)
_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;Cyc_Toc_set_lhs(nv,
_tmp6EB);goto _LL227;};};}_LL25A: {struct Cyc_Absyn_Subscript_e_struct*_tmp54F=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp4F8;if(_tmp54F->tag != 22)goto _LL25C;
else{_tmp550=_tmp54F->f1;_tmp551=_tmp54F->f2;}}_LL25B: {int _tmp704=Cyc_Toc_in_lhs(
nv);Cyc_Toc_set_lhs(nv,0);{void*_tmp705=Cyc_Tcutil_compress((void*)_check_null(
_tmp550->topt));{void*_tmp706=_tmp705;struct Cyc_List_List*_tmp708;struct Cyc_Absyn_PtrInfo
_tmp70A;void*_tmp70B;struct Cyc_Absyn_Tqual _tmp70C;struct Cyc_Absyn_PtrAtts _tmp70D;
void*_tmp70E;union Cyc_Absyn_Constraint*_tmp70F;union Cyc_Absyn_Constraint*_tmp710;
union Cyc_Absyn_Constraint*_tmp711;_LL31B: {struct Cyc_Absyn_TupleType_struct*
_tmp707=(struct Cyc_Absyn_TupleType_struct*)_tmp706;if(_tmp707->tag != 10)goto
_LL31D;else{_tmp708=_tmp707->f1;}}_LL31C: Cyc_Toc_exp_to_c(nv,_tmp550);Cyc_Toc_exp_to_c(
nv,_tmp551);{unsigned int _tmp713;int _tmp714;struct _tuple11 _tmp712=Cyc_Evexp_eval_const_uint_exp(
_tmp551);_tmp713=_tmp712.f1;_tmp714=_tmp712.f2;if(!_tmp714){const char*_tmpCD3;
void*_tmpCD2;(_tmpCD2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpCD3="unknown tuple subscript in translation to C",
_tag_dyneither(_tmpCD3,sizeof(char),44))),_tag_dyneither(_tmpCD2,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp550,Cyc_Absyn_fieldname((int)(_tmp713 + 1)));
goto _LL31A;};_LL31D: {struct Cyc_Absyn_PointerType_struct*_tmp709=(struct Cyc_Absyn_PointerType_struct*)
_tmp706;if(_tmp709->tag != 5)goto _LL31F;else{_tmp70A=_tmp709->f1;_tmp70B=_tmp70A.elt_typ;
_tmp70C=_tmp70A.elt_tq;_tmp70D=_tmp70A.ptr_atts;_tmp70E=_tmp70D.rgn;_tmp70F=
_tmp70D.nullable;_tmp710=_tmp70D.bounds;_tmp711=_tmp70D.zero_term;}}_LL31E: {
struct Cyc_List_List*_tmp717=Cyc_Toc_get_relns(_tmp550);int in_bnds=0;{void*
_tmp718=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp710);_LL322:;_LL323: in_bnds=Cyc_Toc_check_bounds(_tmp705,_tmp717,_tmp550,
_tmp551);if(Cyc_Toc_warn_bounds_checks  && !in_bnds){const char*_tmpCD7;void*
_tmpCD6[1];struct Cyc_String_pa_struct _tmpCD5;(_tmpCD5.tag=0,((_tmpCD5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCD6[0]=&
_tmpCD5,Cyc_Tcutil_warn(e->loc,((_tmpCD7="bounds check necessary for %s",
_tag_dyneither(_tmpCD7,sizeof(char),30))),_tag_dyneither(_tmpCD6,sizeof(void*),1)))))));}
_LL321:;}Cyc_Toc_exp_to_c(nv,_tmp550);Cyc_Toc_exp_to_c(nv,_tmp551);++ Cyc_Toc_total_bounds_checks;{
void*_tmp71C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp710);struct Cyc_Absyn_Exp*_tmp71E;_LL325: {struct Cyc_Absyn_Upper_b_struct*
_tmp71D=(struct Cyc_Absyn_Upper_b_struct*)_tmp71C;if(_tmp71D->tag != 1)goto _LL327;
else{_tmp71E=_tmp71D->f1;}}_LL326: {int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp70F);void*ta1=Cyc_Toc_typ_to_c(_tmp70B);void*ta2=
Cyc_Absyn_cstar_typ(ta1,_tmp70C);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp711)){if(!Cyc_Evexp_c_can_eval(_tmp71E)){const char*_tmpCDA;void*_tmpCD9;(
_tmpCD9=0,Cyc_Tcutil_terr(e->loc,((_tmpCDA="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCDA,sizeof(char),40))),_tag_dyneither(_tmpCD9,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp550);struct Cyc_Absyn_Exp*_tmpCDB[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpCDB[2]=_tmp551,((_tmpCDB[1]=_tmp71E,((
_tmpCDB[0]=_tmp550,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCDB,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp71E)){const char*_tmpCDE;void*_tmpCDD;(
_tmpCDD=0,Cyc_Tcutil_terr(e->loc,((_tmpCDE="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCDE,sizeof(char),40))),_tag_dyneither(_tmpCDD,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCE1;void*_tmpCE0;(_tmpCE0=0,Cyc_Tcutil_warn(
e->loc,((_tmpCE1="inserted null check due to dereference",_tag_dyneither(_tmpCE1,
sizeof(char),39))),_tag_dyneither(_tmpCE0,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCE2[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((
_tmpCE2[3]=_tmp551,((_tmpCE2[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCE2[1]=
_tmp71E,((_tmpCE2[0]=_tmp550,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCE2,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{if(
!Cyc_Evexp_c_can_eval(_tmp71E)){const char*_tmpCE5;void*_tmpCE4;(_tmpCE4=0,Cyc_Tcutil_terr(
e->loc,((_tmpCE5="cannot determine subscript is in bounds",_tag_dyneither(
_tmpCE5,sizeof(char),40))),_tag_dyneither(_tmpCE4,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCE6[2];_tmp551->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((
_tmpCE6[1]=Cyc_Absyn_copy_exp(_tmp551),((_tmpCE6[0]=_tmp71E,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE6,sizeof(struct Cyc_Absyn_Exp*),
2)))))));};}}}goto _LL324;}_LL327: {struct Cyc_Absyn_DynEither_b_struct*_tmp71F=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp71C;if(_tmp71F->tag != 0)goto _LL324;}
_LL328: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp70B);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp70C),
Cyc_Absyn_aggrmember_exp(_tmp550,Cyc_Toc_curr_sp,0)),_tmp551);}else{struct Cyc_Absyn_Exp*
_tmp72B=Cyc_Toc__check_dyneither_subscript_e;struct Cyc_Absyn_Exp*_tmpCE7[3];e->r=
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp70C),Cyc_Absyn_fncall_exp(
_tmp72B,((_tmpCE7[2]=_tmp551,((_tmpCE7[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((
_tmpCE7[0]=_tmp550,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCE7,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL324;}
_LL324:;}goto _LL31A;}_LL31F:;_LL320: {const char*_tmpCEA;void*_tmpCE9;(_tmpCE9=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCEA="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(
_tmpCEA,sizeof(char),49))),_tag_dyneither(_tmpCE9,sizeof(void*),0)));}_LL31A:;}
Cyc_Toc_set_lhs(nv,_tmp704);goto _LL227;};}_LL25C: {struct Cyc_Absyn_Tuple_e_struct*
_tmp552=(struct Cyc_Absyn_Tuple_e_struct*)_tmp4F8;if(_tmp552->tag != 23)goto _LL25E;
else{_tmp553=_tmp552->f1;}}_LL25D: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(
nv,0,0,_tmp553))->r;else{struct Cyc_List_List*_tmp72F=((struct Cyc_List_List*(*)(
struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_tup_to_c,_tmp553);void*_tmp730=Cyc_Toc_add_tuple_type(_tmp72F);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp553 != 0;(_tmp553=_tmp553->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp553->hd);{struct _tuple16*_tmpCED;struct Cyc_List_List*
_tmpCEC;dles=((_tmpCEC=_cycalloc(sizeof(*_tmpCEC)),((_tmpCEC->hd=((_tmpCED=
_cycalloc(sizeof(*_tmpCED)),((_tmpCED->f1=0,((_tmpCED->f2=(struct Cyc_Absyn_Exp*)
_tmp553->hd,_tmpCED)))))),((_tmpCEC->tl=dles,_tmpCEC))))));};}}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);e->r=Cyc_Toc_unresolvedmem_exp_r(
0,dles);}goto _LL227;_LL25E: {struct Cyc_Absyn_Array_e_struct*_tmp554=(struct Cyc_Absyn_Array_e_struct*)
_tmp4F8;if(_tmp554->tag != 25)goto _LL260;else{_tmp555=_tmp554->f1;}}_LL25F: e->r=
Cyc_Toc_unresolvedmem_exp_r(0,_tmp555);{struct Cyc_List_List*_tmp733=_tmp555;for(
0;_tmp733 != 0;_tmp733=_tmp733->tl){struct _tuple16 _tmp735;struct Cyc_Absyn_Exp*
_tmp736;struct _tuple16*_tmp734=(struct _tuple16*)_tmp733->hd;_tmp735=*_tmp734;
_tmp736=_tmp735.f2;Cyc_Toc_exp_to_c(nv,_tmp736);}}goto _LL227;_LL260: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp556=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp4F8;if(_tmp556->tag != 26)
goto _LL262;else{_tmp557=_tmp556->f1;_tmp558=_tmp556->f2;_tmp559=_tmp556->f3;
_tmp55A=_tmp556->f4;}}_LL261: {unsigned int _tmp738;int _tmp739;struct _tuple11
_tmp737=Cyc_Evexp_eval_const_uint_exp(_tmp558);_tmp738=_tmp737.f1;_tmp739=
_tmp737.f2;{void*_tmp73A=Cyc_Toc_typ_to_c((void*)_check_null(_tmp559->topt));Cyc_Toc_exp_to_c(
nv,_tmp559);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp559)){if(!_tmp739){
const char*_tmpCF0;void*_tmpCEF;(_tmpCEF=0,Cyc_Tcutil_terr(_tmp558->loc,((_tmpCF0="cannot determine value of constant",
_tag_dyneither(_tmpCF0,sizeof(char),35))),_tag_dyneither(_tmpCEF,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp738;++ i){struct _tuple16*_tmpCF3;struct Cyc_List_List*
_tmpCF2;es=((_tmpCF2=_cycalloc(sizeof(*_tmpCF2)),((_tmpCF2->hd=((_tmpCF3=
_cycalloc(sizeof(*_tmpCF3)),((_tmpCF3->f1=0,((_tmpCF3->f2=_tmp559,_tmpCF3)))))),((
_tmpCF2->tl=es,_tmpCF2))))));}}if(_tmp55A){struct Cyc_Absyn_Exp*_tmp73F=Cyc_Toc_cast_it(
_tmp73A,Cyc_Absyn_uint_exp(0,0));struct _tuple16*_tmpCF6;struct Cyc_List_List*
_tmpCF5;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,((_tmpCF5=_cycalloc(sizeof(*_tmpCF5)),((_tmpCF5->hd=((
_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6->f1=0,((_tmpCF6->f2=_tmp73F,
_tmpCF6)))))),((_tmpCF5->tl=0,_tmpCF5)))))));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL227;};};}_LL262: {struct Cyc_Absyn_Aggregate_e_struct*_tmp55B=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp4F8;if(_tmp55B->tag != 27)goto _LL264;
else{_tmp55C=_tmp55B->f1;_tmp55D=_tmp55B->f2;_tmp55E=_tmp55B->f3;_tmp55F=_tmp55B->f4;}}
_LL263: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp55D,
0,0,_tmp55E,_tmp55C))->r;else{if(_tmp55F == 0){const char*_tmpCF9;void*_tmpCF8;(
_tmpCF8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCF9="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpCF9,sizeof(
char),38))),_tag_dyneither(_tmpCF8,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp55F;struct _RegionHandle _tmp744=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp744;_push_region(rgn);{struct Cyc_List_List*_tmp745=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*
des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
rgn,e->loc,_tmp55E,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp55F->impl))->tagged){struct
_tuple22 _tmp747;struct Cyc_Absyn_Aggrfield*_tmp748;struct Cyc_Absyn_Exp*_tmp749;
struct _tuple22*_tmp746=(struct _tuple22*)((struct Cyc_List_List*)_check_null(
_tmp745))->hd;_tmp747=*_tmp746;_tmp748=_tmp747.f1;_tmp749=_tmp747.f2;{void*
_tmp74A=_tmp748->type;Cyc_Toc_exp_to_c(nv,_tmp749);if(Cyc_Toc_is_void_star_or_boxed_tvar(
_tmp74A))_tmp749->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp749->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp55F,_tmp748->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple16*_tmpCFE;struct _tuple16*_tmpCFD;struct _tuple16*_tmpCFC[2];
struct Cyc_List_List*_tmp74B=(_tmpCFC[1]=((_tmpCFD=_cycalloc(sizeof(*_tmpCFD)),((
_tmpCFD->f1=0,((_tmpCFD->f2=_tmp749,_tmpCFD)))))),((_tmpCFC[0]=((_tmpCFE=
_cycalloc(sizeof(*_tmpCFE)),((_tmpCFE->f1=0,((_tmpCFE->f2=field_tag_exp,_tmpCFE)))))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCFC,sizeof(struct _tuple16*),2)))));struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(
0,_tmp74B,0);struct Cyc_Absyn_FieldName_struct*_tmpD04;struct Cyc_Absyn_FieldName_struct
_tmpD03;void*_tmpD02[1];struct Cyc_List_List*ds=(_tmpD02[0]=(void*)((_tmpD04=
_cycalloc(sizeof(*_tmpD04)),((_tmpD04[0]=((_tmpD03.tag=1,((_tmpD03.f1=_tmp748->name,
_tmpD03)))),_tmpD04)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD02,sizeof(void*),1)));struct _tuple16*_tmpD07;struct _tuple16*
_tmpD06[1];struct Cyc_List_List*dles=(_tmpD06[0]=((_tmpD07=_cycalloc(sizeof(*
_tmpD07)),((_tmpD07->f1=ds,((_tmpD07->f2=umem,_tmpD07)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD06,sizeof(struct
_tuple16*),1)));e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{struct Cyc_List_List*
_tmp754=0;struct Cyc_List_List*_tmp755=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp755 != 0;_tmp755=_tmp755->tl){struct Cyc_List_List*
_tmp756=_tmp745;for(0;_tmp756 != 0;_tmp756=_tmp756->tl){if((*((struct _tuple22*)
_tmp756->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp755->hd){struct _tuple22
_tmp758;struct Cyc_Absyn_Aggrfield*_tmp759;struct Cyc_Absyn_Exp*_tmp75A;struct
_tuple22*_tmp757=(struct _tuple22*)_tmp756->hd;_tmp758=*_tmp757;_tmp759=_tmp758.f1;
_tmp75A=_tmp758.f2;{void*_tmp75B=_tmp759->type;Cyc_Toc_exp_to_c(nv,_tmp75A);if(
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp75B))_tmp75A->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp75A->r,0));{struct _tuple16*_tmpD0A;struct Cyc_List_List*
_tmpD09;_tmp754=((_tmpD09=_cycalloc(sizeof(*_tmpD09)),((_tmpD09->hd=((_tmpD0A=
_cycalloc(sizeof(*_tmpD0A)),((_tmpD0A->f1=0,((_tmpD0A->f2=_tmp75A,_tmpD0A)))))),((
_tmpD09->tl=_tmp754,_tmpD09))))));}break;};}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp754));}};
_pop_region(rgn);};}goto _LL227;_LL264: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp560=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp4F8;if(_tmp560->tag != 28)goto
_LL266;else{_tmp561=(void*)_tmp560->f1;_tmp562=_tmp560->f2;}}_LL265: {struct Cyc_List_List*
fs;{void*_tmp75E=Cyc_Tcutil_compress(_tmp561);struct Cyc_List_List*_tmp760;_LL32A: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp75F=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp75E;if(_tmp75F->tag != 12)goto _LL32C;else{_tmp760=_tmp75F->f2;}}_LL32B: fs=
_tmp760;goto _LL329;_LL32C:;_LL32D: {const char*_tmpD0E;void*_tmpD0D[1];struct Cyc_String_pa_struct
_tmpD0C;(_tmpD0C.tag=0,((_tmpD0C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp561)),((_tmpD0D[0]=& _tmpD0C,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD0E="anon struct has type %s",
_tag_dyneither(_tmpD0E,sizeof(char),24))),_tag_dyneither(_tmpD0D,sizeof(void*),1)))))));}
_LL329:;}{struct _RegionHandle _tmp764=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp764;_push_region(rgn);{struct Cyc_List_List*_tmp765=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,
_tmp562,Cyc_Absyn_StructA,fs);for(0;_tmp765 != 0;_tmp765=_tmp765->tl){struct
_tuple22 _tmp767;struct Cyc_Absyn_Aggrfield*_tmp768;struct Cyc_Absyn_Exp*_tmp769;
struct _tuple22*_tmp766=(struct _tuple22*)_tmp765->hd;_tmp767=*_tmp766;_tmp768=
_tmp767.f1;_tmp769=_tmp767.f2;{void*_tmp76A=_tmp768->type;Cyc_Toc_exp_to_c(nv,
_tmp769);if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp76A))_tmp769->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp769->r,0));};}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp562);}_npop_handler(0);goto _LL227;;_pop_region(rgn);};}_LL266: {struct Cyc_Absyn_Datatype_e_struct*
_tmp563=(struct Cyc_Absyn_Datatype_e_struct*)_tmp4F8;if(_tmp563->tag != 29)goto
_LL268;else{_tmp564=_tmp563->f1;_tmp565=_tmp563->f2;_tmp566=_tmp563->f3;}}_LL267: {
void*datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple0*_tmp76B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp76C=Cyc_Absyn_var_exp(_tmp76B,0);struct Cyc_Absyn_Exp*
member_exp;{const char*_tmpD0F;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp566->name,((_tmpD0F="_struct",_tag_dyneither(_tmpD0F,sizeof(char),8)))));}
tag_exp=_tmp565->is_extensible?Cyc_Absyn_var_exp(_tmp566->name,0): Cyc_Toc_datatype_tag(
_tmp565,_tmp566->name);member_exp=_tmp76C;{struct Cyc_List_List*_tmp76E=_tmp566->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp564 != 0;(
_tmp564=_tmp564->tl,_tmp76E=_tmp76E->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp564->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp76E))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_boxed_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct _tuple16*_tmpD12;struct
Cyc_List_List*_tmpD11;dles=((_tmpD11=_cycalloc(sizeof(*_tmpD11)),((_tmpD11->hd=((
_tmpD12=_cycalloc(sizeof(*_tmpD12)),((_tmpD12->f1=0,((_tmpD12->f2=cur_e,_tmpD12)))))),((
_tmpD11->tl=dles,_tmpD11))))));};}{struct _tuple16*_tmpD15;struct Cyc_List_List*
_tmpD14;dles=((_tmpD14=_cycalloc(sizeof(*_tmpD14)),((_tmpD14->hd=((_tmpD15=
_cycalloc(sizeof(*_tmpD15)),((_tmpD15->f1=0,((_tmpD15->f2=tag_exp,_tmpD15)))))),((
_tmpD14->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles),_tmpD14))))));}e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{struct Cyc_List_List*
_tmpD16;struct Cyc_List_List*_tmp773=(_tmpD16=_cycalloc(sizeof(*_tmpD16)),((
_tmpD16->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,
0),tag_exp,0),((_tmpD16->tl=0,_tmpD16)))));{int i=1;for(0;_tmp564 != 0;(((_tmp564=
_tmp564->tl,i ++)),_tmp76E=_tmp76E->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp564->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp76E))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_boxed_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp774=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpD17;_tmp773=((_tmpD17=_cycalloc(sizeof(*
_tmpD17)),((_tmpD17->hd=_tmp774,((_tmpD17->tl=_tmp773,_tmpD17))))));};}}{struct
Cyc_Absyn_Stmt*_tmp776=Cyc_Absyn_exp_stmt(_tmp76C,0);struct Cyc_List_List*_tmpD18;
struct Cyc_Absyn_Stmt*_tmp777=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpD18=_cycalloc(sizeof(*_tmpD18)),((
_tmpD18->hd=_tmp776,((_tmpD18->tl=_tmp773,_tmpD18))))))),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp76B,datatype_ctype,0,_tmp777,0));};}goto _LL227;};}
_LL268: {struct Cyc_Absyn_Enum_e_struct*_tmp567=(struct Cyc_Absyn_Enum_e_struct*)
_tmp4F8;if(_tmp567->tag != 30)goto _LL26A;}_LL269: goto _LL26B;_LL26A: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp568=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp4F8;if(_tmp568->tag != 31)goto
_LL26C;}_LL26B: goto _LL227;_LL26C: {struct Cyc_Absyn_Malloc_e_struct*_tmp569=(
struct Cyc_Absyn_Malloc_e_struct*)_tmp4F8;if(_tmp569->tag != 32)goto _LL26E;else{
_tmp56A=_tmp569->f1;_tmp56B=_tmp56A.is_calloc;_tmp56C=_tmp56A.rgn;_tmp56D=
_tmp56A.elt_type;_tmp56E=_tmp56A.num_elts;_tmp56F=_tmp56A.fat_result;}}_LL26D: {
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp56D)));Cyc_Toc_exp_to_c(nv,
_tmp56E);if(_tmp56F){struct _tuple0*_tmp77A=Cyc_Toc_temp_var();struct _tuple0*
_tmp77B=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp56B){xexp=_tmp56E;if(_tmp56C != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp56C;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp77A,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp56D,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp77A,0));}{struct Cyc_Absyn_Exp*_tmpD19[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpD19[2]=Cyc_Absyn_var_exp(_tmp77A,0),((_tmpD19[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpD19[0]=Cyc_Absyn_var_exp(_tmp77B,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD19,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{if(_tmp56C != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp56C;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp77A,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp56D,Cyc_Absyn_var_exp(_tmp77A,0));}{struct Cyc_Absyn_Exp*_tmpD1A[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpD1A[2]=Cyc_Absyn_var_exp(_tmp77A,0),((_tmpD1A[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpD1A[0]=Cyc_Absyn_var_exp(_tmp77B,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD1A,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);};}{struct Cyc_Absyn_Stmt*_tmp77E=Cyc_Absyn_declare_stmt(_tmp77A,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)_tmp56E,Cyc_Absyn_declare_stmt(_tmp77B,Cyc_Absyn_cstar_typ(
t_c,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp77E);};}else{if(_tmp56B){if(_tmp56C != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp56C;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp56E))->r;}else{e->r=(
Cyc_Toc_calloc_exp(*_tmp56D,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp56E))->r;}}else{
if(_tmp56C != 0  && !Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp56C;Cyc_Toc_exp_to_c(nv,rgn);e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp56E))->r;}
else{e->r=(Cyc_Toc_malloc_exp(*_tmp56D,_tmp56E))->r;}}}goto _LL227;}_LL26E: {
struct Cyc_Absyn_Swap_e_struct*_tmp570=(struct Cyc_Absyn_Swap_e_struct*)_tmp4F8;
if(_tmp570->tag != 33)goto _LL270;else{_tmp571=_tmp570->f1;_tmp572=_tmp570->f2;}}
_LL26F: {int is_dyneither_ptr=0;void*e1_old_typ=(void*)_check_null(_tmp571->topt);
void*e2_old_typ=(void*)_check_null(_tmp572->topt);if(!Cyc_Tcutil_is_pointer_or_boxed(
e1_old_typ,& is_dyneither_ptr)){const char*_tmpD1D;void*_tmpD1C;(_tmpD1C=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD1D="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpD1D,sizeof(char),57))),_tag_dyneither(_tmpD1C,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}{int f1_tag=0;void*tagged_mem_type1=(void*)&
Cyc_Absyn_VoidType_val;int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp571,&
f1_tag,& tagged_mem_type1,1);int f2_tag=0;void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp572,& f2_tag,& tagged_mem_type2,1);
Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp571);Cyc_Toc_exp_to_c(nv,_tmp572);
Cyc_Toc_set_lhs(nv,0);if(e1_tmem)Cyc_Toc_add_tagged_union_check_for_swap(_tmp571,
f1_tag,tagged_mem_type1);else{_tmp571=Cyc_Toc_push_address_exp(_tmp571);}if(
e2_tmem)Cyc_Toc_add_tagged_union_check_for_swap(_tmp572,f2_tag,tagged_mem_type2);
else{_tmp572=Cyc_Toc_push_address_exp(_tmp572);}{struct Cyc_Absyn_Exp*_tmpD1E[2];
e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpD1E[1]=_tmp572,((_tmpD1E[0]=_tmp571,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD1E,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL227;};};}_LL270: {struct
Cyc_Absyn_Tagcheck_e_struct*_tmp573=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp4F8;
if(_tmp573->tag != 36)goto _LL272;else{_tmp574=_tmp573->f1;_tmp575=_tmp573->f2;}}
_LL271: {void*_tmp782=Cyc_Tcutil_compress((void*)_check_null(_tmp574->topt));Cyc_Toc_exp_to_c(
nv,_tmp574);{void*_tmp783=_tmp782;struct Cyc_Absyn_AggrInfo _tmp785;union Cyc_Absyn_AggrInfoU
_tmp786;struct Cyc_Absyn_Aggrdecl**_tmp787;struct Cyc_Absyn_Aggrdecl*_tmp788;
_LL32F: {struct Cyc_Absyn_AggrType_struct*_tmp784=(struct Cyc_Absyn_AggrType_struct*)
_tmp783;if(_tmp784->tag != 11)goto _LL331;else{_tmp785=_tmp784->f1;_tmp786=_tmp785.aggr_info;
if((_tmp786.KnownAggr).tag != 2)goto _LL331;_tmp787=(struct Cyc_Absyn_Aggrdecl**)(
_tmp786.KnownAggr).val;_tmp788=*_tmp787;}}_LL330: {struct Cyc_Absyn_Exp*_tmp789=
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp788,_tmp575),0);struct Cyc_Absyn_Exp*
_tmp78A=Cyc_Absyn_aggrmember_exp(_tmp574,_tmp575,0);struct Cyc_Absyn_Exp*_tmp78B=
Cyc_Absyn_aggrmember_exp(_tmp78A,Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(
_tmp78B,_tmp789,0))->r;goto _LL32E;}_LL331:;_LL332: {const char*_tmpD21;void*
_tmpD20;(_tmpD20=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD21="non-aggregate type in tagcheck",_tag_dyneither(_tmpD21,sizeof(char),31))),
_tag_dyneither(_tmpD20,sizeof(void*),0)));}_LL32E:;}goto _LL227;}_LL272: {struct
Cyc_Absyn_StmtExp_e_struct*_tmp576=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp4F8;
if(_tmp576->tag != 35)goto _LL274;else{_tmp577=_tmp576->f1;}}_LL273: Cyc_Toc_stmt_to_c(
nv,_tmp577);goto _LL227;_LL274: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp578=(
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp4F8;if(_tmp578->tag != 34)goto _LL276;}
_LL275: {const char*_tmpD24;void*_tmpD23;(_tmpD23=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD24="UnresolvedMem",
_tag_dyneither(_tmpD24,sizeof(char),14))),_tag_dyneither(_tmpD23,sizeof(void*),0)));}
_LL276: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp579=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp4F8;if(_tmp579->tag != 24)goto _LL278;}_LL277: {const char*_tmpD27;void*_tmpD26;(
_tmpD26=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpD27="compoundlit",_tag_dyneither(_tmpD27,sizeof(char),12))),_tag_dyneither(
_tmpD26,sizeof(void*),0)));}_LL278: {struct Cyc_Absyn_Valueof_e_struct*_tmp57A=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp4F8;if(_tmp57A->tag != 37)goto _LL27A;}
_LL279: {const char*_tmpD2A;void*_tmpD29;(_tmpD29=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD2A="valueof(-)",
_tag_dyneither(_tmpD2A,sizeof(char),11))),_tag_dyneither(_tmpD29,sizeof(void*),0)));}
_LL27A: {struct Cyc_Absyn_Asm_e_struct*_tmp57B=(struct Cyc_Absyn_Asm_e_struct*)
_tmp4F8;if(_tmp57B->tag != 38)goto _LL227;}_LL27B: {const char*_tmpD2D;void*_tmpD2C;(
_tmpD2C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD2D="__asm__",_tag_dyneither(_tmpD2D,sizeof(char),8))),_tag_dyneither(
_tmpD2C,sizeof(void*),0)));}_LL227:;};}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0);}struct _tuple23{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*
f3;};struct _tuple24{struct _tuple0*f1;void*f2;};struct _tuple25{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple23 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*
nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
struct Cyc_Absyn_Stmt*s;{void*_tmp796=p->r;struct Cyc_Absyn_Vardecl*_tmp798;struct
Cyc_Absyn_Vardecl _tmp799;struct _tuple0*_tmp79A;struct Cyc_Absyn_Pat*_tmp79B;
struct Cyc_Absyn_Vardecl*_tmp79D;struct Cyc_Absyn_Vardecl _tmp79E;struct _tuple0*
_tmp79F;struct Cyc_Absyn_Vardecl*_tmp7A2;struct Cyc_Absyn_Pat*_tmp7A3;enum Cyc_Absyn_Sign
_tmp7A6;int _tmp7A7;char _tmp7A9;struct _dyneither_ptr _tmp7AB;int _tmp7AC;struct Cyc_Absyn_Enumdecl*
_tmp7AE;struct Cyc_Absyn_Enumfield*_tmp7AF;void*_tmp7B1;struct Cyc_Absyn_Enumfield*
_tmp7B2;struct Cyc_Absyn_Pat*_tmp7B4;struct Cyc_Absyn_Pat _tmp7B5;void*_tmp7B6;
struct Cyc_Absyn_Datatypedecl*_tmp7B8;struct Cyc_Absyn_Datatypefield*_tmp7B9;
struct Cyc_List_List*_tmp7BA;struct Cyc_List_List*_tmp7BC;struct Cyc_List_List*
_tmp7BE;struct Cyc_Absyn_AggrInfo*_tmp7C0;struct Cyc_Absyn_AggrInfo*_tmp7C2;struct
Cyc_Absyn_AggrInfo _tmp7C3;union Cyc_Absyn_AggrInfoU _tmp7C4;struct Cyc_List_List*
_tmp7C5;struct Cyc_Absyn_Pat*_tmp7C7;_LL334: {struct Cyc_Absyn_Var_p_struct*
_tmp797=(struct Cyc_Absyn_Var_p_struct*)_tmp796;if(_tmp797->tag != 1)goto _LL336;
else{_tmp798=_tmp797->f1;_tmp799=*_tmp798;_tmp79A=_tmp799.name;_tmp79B=_tmp797->f2;}}
_LL335: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp79A,r),rgn,t,r,path,
_tmp79B,fail_stmt,decls);_LL336: {struct Cyc_Absyn_TagInt_p_struct*_tmp79C=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp796;if(_tmp79C->tag != 3)goto _LL338;else{
_tmp79D=_tmp79C->f2;_tmp79E=*_tmp79D;_tmp79F=_tmp79E.name;}}_LL337: nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp79F,r);goto _LL339;_LL338: {struct Cyc_Absyn_Wild_p_struct*_tmp7A0=(
struct Cyc_Absyn_Wild_p_struct*)_tmp796;if(_tmp7A0->tag != 0)goto _LL33A;}_LL339: s=
Cyc_Toc_skip_stmt_dl();goto _LL333;_LL33A: {struct Cyc_Absyn_Reference_p_struct*
_tmp7A1=(struct Cyc_Absyn_Reference_p_struct*)_tmp796;if(_tmp7A1->tag != 2)goto
_LL33C;else{_tmp7A2=_tmp7A1->f1;_tmp7A3=_tmp7A1->f2;}}_LL33B: {struct _tuple0*
_tmp7CB=Cyc_Toc_temp_var();{struct _tuple24*_tmpD30;struct Cyc_List_List*_tmpD2F;
decls=((_tmpD2F=_region_malloc(rgn,sizeof(*_tmpD2F)),((_tmpD2F->hd=((_tmpD30=
_region_malloc(rgn,sizeof(*_tmpD30)),((_tmpD30->f1=_tmp7CB,((_tmpD30->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpD30)))))),((_tmpD2F->tl=decls,_tmpD2F))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp7A2->name,Cyc_Absyn_var_exp(_tmp7CB,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp7CB,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Toc_push_address_exp(path)),0);{struct _tuple23 _tmp7CE=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp7A3,fail_stmt,decls);_tmp7CE.f3=Cyc_Absyn_seq_stmt(s,_tmp7CE.f3,
0);return _tmp7CE;};}_LL33C: {struct Cyc_Absyn_Null_p_struct*_tmp7A4=(struct Cyc_Absyn_Null_p_struct*)
_tmp796;if(_tmp7A4->tag != 8)goto _LL33E;}_LL33D: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),fail_stmt);goto _LL333;_LL33E: {struct Cyc_Absyn_Int_p_struct*_tmp7A5=(struct
Cyc_Absyn_Int_p_struct*)_tmp796;if(_tmp7A5->tag != 9)goto _LL340;else{_tmp7A6=
_tmp7A5->f1;_tmp7A7=_tmp7A5->f2;}}_LL33F: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp7A6,_tmp7A7,0),fail_stmt);goto _LL333;_LL340: {struct Cyc_Absyn_Char_p_struct*
_tmp7A8=(struct Cyc_Absyn_Char_p_struct*)_tmp796;if(_tmp7A8->tag != 10)goto _LL342;
else{_tmp7A9=_tmp7A8->f1;}}_LL341: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(
_tmp7A9,0),fail_stmt);goto _LL333;_LL342: {struct Cyc_Absyn_Float_p_struct*_tmp7AA=(
struct Cyc_Absyn_Float_p_struct*)_tmp796;if(_tmp7AA->tag != 11)goto _LL344;else{
_tmp7AB=_tmp7AA->f1;_tmp7AC=_tmp7AA->f2;}}_LL343: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(
_tmp7AB,_tmp7AC,0),fail_stmt);goto _LL333;_LL344: {struct Cyc_Absyn_Enum_p_struct*
_tmp7AD=(struct Cyc_Absyn_Enum_p_struct*)_tmp796;if(_tmp7AD->tag != 12)goto _LL346;
else{_tmp7AE=_tmp7AD->f1;_tmp7AF=_tmp7AD->f2;}}_LL345:{struct Cyc_Absyn_Enum_e_struct
_tmpD33;struct Cyc_Absyn_Enum_e_struct*_tmpD32;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD32=_cycalloc(sizeof(*_tmpD32)),((_tmpD32[0]=((_tmpD33.tag=30,((
_tmpD33.f1=_tmp7AF->name,((_tmpD33.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7AE,((
_tmpD33.f3=(struct Cyc_Absyn_Enumfield*)_tmp7AF,_tmpD33)))))))),_tmpD32)))),0),
fail_stmt);}goto _LL333;_LL346: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp7B0=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp796;if(_tmp7B0->tag != 13)goto _LL348;else{
_tmp7B1=(void*)_tmp7B0->f1;_tmp7B2=_tmp7B0->f2;}}_LL347:{struct Cyc_Absyn_AnonEnum_e_struct
_tmpD36;struct Cyc_Absyn_AnonEnum_e_struct*_tmpD35;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD35=_cycalloc(sizeof(*_tmpD35)),((_tmpD35[0]=((_tmpD36.tag=31,((
_tmpD36.f1=_tmp7B2->name,((_tmpD36.f2=(void*)_tmp7B1,((_tmpD36.f3=(struct Cyc_Absyn_Enumfield*)
_tmp7B2,_tmpD36)))))))),_tmpD35)))),0),fail_stmt);}goto _LL333;_LL348: {struct Cyc_Absyn_Pointer_p_struct*
_tmp7B3=(struct Cyc_Absyn_Pointer_p_struct*)_tmp796;if(_tmp7B3->tag != 5)goto
_LL34A;else{_tmp7B4=_tmp7B3->f1;_tmp7B5=*_tmp7B4;_tmp7B6=_tmp7B5.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp7B7=(struct Cyc_Absyn_Datatype_p_struct*)_tmp7B6;if(_tmp7B7->tag != 7)goto
_LL34A;else{_tmp7B8=_tmp7B7->f1;_tmp7B9=_tmp7B7->f2;_tmp7BA=_tmp7B7->f3;}};}}
_LL349: s=Cyc_Toc_skip_stmt_dl();{struct _tuple0*_tmp7D3=Cyc_Toc_temp_var();const
char*_tmpD37;struct _tuple0*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp7B9->name,((
_tmpD37="_struct",_tag_dyneither(_tmpD37,sizeof(char),8))));void*_tmp7D4=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);int cnt=1;struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
_tmp7D4,r);struct Cyc_List_List*_tmp7D5=_tmp7B9->typs;for(0;_tmp7BA != 0;(((
_tmp7BA=_tmp7BA->tl,_tmp7D5=((struct Cyc_List_List*)_check_null(_tmp7D5))->tl)),
++ cnt)){struct Cyc_Absyn_Pat*_tmp7D6=(struct Cyc_Absyn_Pat*)_tmp7BA->hd;if(_tmp7D6->r
== (void*)& Cyc_Absyn_Wild_p_val)continue;{void*_tmp7D7=(*((struct _tuple10*)((
struct Cyc_List_List*)_check_null(_tmp7D5))->hd)).f2;struct _tuple0*_tmp7D8=Cyc_Toc_temp_var();
void*_tmp7D9=(void*)_check_null(_tmp7D6->topt);void*_tmp7DA=Cyc_Toc_typ_to_c(
_tmp7D9);struct Cyc_Absyn_Exp*_tmp7DB=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(
_tmp7D3,0),Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(
_tmp7D7)))_tmp7DB=Cyc_Toc_cast_it(_tmp7DA,_tmp7DB);{struct _tuple24*_tmpD3A;
struct Cyc_List_List*_tmpD39;decls=((_tmpD39=_region_malloc(rgn,sizeof(*_tmpD39)),((
_tmpD39->hd=((_tmpD3A=_region_malloc(rgn,sizeof(*_tmpD3A)),((_tmpD3A->f1=_tmp7D8,((
_tmpD3A->f2=_tmp7DA,_tmpD3A)))))),((_tmpD39->tl=decls,_tmpD39))))));}{struct
_tuple23 _tmp7DE=Cyc_Toc_xlate_pat(nv,rgn,_tmp7D9,Cyc_Absyn_var_exp(_tmp7D8,0),
_tmp7DB,_tmp7D6,fail_stmt,decls);nv=_tmp7DE.f1;decls=_tmp7DE.f2;{struct Cyc_Absyn_Stmt*
_tmp7DF=_tmp7DE.f3;struct Cyc_Absyn_Stmt*_tmp7E0=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp7D8,0),_tmp7DB,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7E0,_tmp7DF,
0),0);};};};}{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(
_tmp7D3,0),Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*tag_exp=_tmp7B8->is_extensible?
Cyc_Absyn_var_exp(_tmp7B9->name,0): Cyc_Toc_datatype_tag(_tmp7B8,_tmp7B9->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);s=Cyc_Absyn_ifthenelse_stmt(
test_exp,fail_stmt,s,0);if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))s=
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp7D3,0),Cyc_Absyn_uint_exp(
0,0),0),fail_stmt,s,0);s=Cyc_Absyn_declare_stmt(_tmp7D3,_tmp7D4,(struct Cyc_Absyn_Exp*)
rcast,s,0);goto _LL333;};};_LL34A: {struct Cyc_Absyn_Datatype_p_struct*_tmp7BB=(
struct Cyc_Absyn_Datatype_p_struct*)_tmp796;if(_tmp7BB->tag != 7)goto _LL34C;else{
_tmp7BC=_tmp7BB->f3;}}_LL34B: _tmp7BE=_tmp7BC;goto _LL34D;_LL34C: {struct Cyc_Absyn_Tuple_p_struct*
_tmp7BD=(struct Cyc_Absyn_Tuple_p_struct*)_tmp796;if(_tmp7BD->tag != 4)goto _LL34E;
else{_tmp7BE=_tmp7BD->f1;}}_LL34D: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;
_tmp7BE != 0;(_tmp7BE=_tmp7BE->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp7E2=(struct Cyc_Absyn_Pat*)
_tmp7BE->hd;if(_tmp7E2->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{struct _tuple0*
_tmp7E3=Cyc_Toc_temp_var();void*_tmp7E4=(void*)_check_null(_tmp7E2->topt);{
struct _tuple24*_tmpD3D;struct Cyc_List_List*_tmpD3C;decls=((_tmpD3C=
_region_malloc(rgn,sizeof(*_tmpD3C)),((_tmpD3C->hd=((_tmpD3D=_region_malloc(rgn,
sizeof(*_tmpD3D)),((_tmpD3D->f1=_tmp7E3,((_tmpD3D->f2=Cyc_Toc_typ_to_c(_tmp7E4),
_tmpD3D)))))),((_tmpD3C->tl=decls,_tmpD3C))))));}{struct _tuple23 _tmp7E7=Cyc_Toc_xlate_pat(
nv,rgn,_tmp7E4,Cyc_Absyn_var_exp(_tmp7E3,0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(
cnt),0),_tmp7E2,fail_stmt,decls);nv=_tmp7E7.f1;decls=_tmp7E7.f2;{struct Cyc_Absyn_Stmt*
_tmp7E8=_tmp7E7.f3;struct Cyc_Absyn_Stmt*_tmp7E9=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp7E3,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp7E9,_tmp7E8,0),0);};};};}goto _LL333;};_LL34E: {struct
Cyc_Absyn_Aggr_p_struct*_tmp7BF=(struct Cyc_Absyn_Aggr_p_struct*)_tmp796;if(
_tmp7BF->tag != 6)goto _LL350;else{_tmp7C0=_tmp7BF->f1;if(_tmp7C0 != 0)goto _LL350;}}
_LL34F: {const char*_tmpD40;void*_tmpD3F;(_tmpD3F=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD40="unresolved aggregate pattern!",
_tag_dyneither(_tmpD40,sizeof(char),30))),_tag_dyneither(_tmpD3F,sizeof(void*),0)));}
_LL350: {struct Cyc_Absyn_Aggr_p_struct*_tmp7C1=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp796;if(_tmp7C1->tag != 6)goto _LL352;else{_tmp7C2=_tmp7C1->f1;if(_tmp7C2 == 0)
goto _LL352;_tmp7C3=*_tmp7C2;_tmp7C4=_tmp7C3.aggr_info;_tmp7C5=_tmp7C1->f3;}}
_LL351: {struct Cyc_Absyn_Aggrdecl*_tmp7EC=Cyc_Absyn_get_known_aggrdecl(_tmp7C4);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7EC->impl))->tagged){struct
_tuple25 _tmp7EE;struct Cyc_List_List*_tmp7EF;struct Cyc_Absyn_Pat*_tmp7F0;struct
_tuple25*_tmp7ED=(struct _tuple25*)((struct Cyc_List_List*)_check_null(_tmp7C5))->hd;
_tmp7EE=*_tmp7ED;_tmp7EF=_tmp7EE.f1;_tmp7F0=_tmp7EE.f2;{struct _dyneither_ptr*f;{
void*_tmp7F1=(void*)((struct Cyc_List_List*)_check_null(_tmp7EF))->hd;struct
_dyneither_ptr*_tmp7F3;_LL35B: {struct Cyc_Absyn_FieldName_struct*_tmp7F2=(struct
Cyc_Absyn_FieldName_struct*)_tmp7F1;if(_tmp7F2->tag != 1)goto _LL35D;else{_tmp7F3=
_tmp7F2->f1;}}_LL35C: f=_tmp7F3;goto _LL35A;_LL35D:;_LL35E: {const char*_tmpD43;
void*_tmpD42;(_tmpD42=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(((_tmpD43="no field name in tagged union pattern",
_tag_dyneither(_tmpD43,sizeof(char),38))),_tag_dyneither(_tmpD42,sizeof(void*),0)));}
_LL35A:;}{struct _tuple0*_tmp7F6=Cyc_Toc_temp_var();void*_tmp7F7=(void*)
_check_null(_tmp7F0->topt);void*_tmp7F8=Cyc_Toc_typ_to_c(_tmp7F7);{struct
_tuple24*_tmpD46;struct Cyc_List_List*_tmpD45;decls=((_tmpD45=_region_malloc(rgn,
sizeof(*_tmpD45)),((_tmpD45->hd=((_tmpD46=_region_malloc(rgn,sizeof(*_tmpD46)),((
_tmpD46->f1=_tmp7F6,((_tmpD46->f2=_tmp7F8,_tmpD46)))))),((_tmpD45->tl=decls,
_tmpD45))))));}{struct Cyc_Absyn_Exp*_tmp7FB=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(
path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp7FC=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);_tmp7FC=Cyc_Toc_cast_it(
_tmp7F8,_tmp7FC);_tmp7FB=Cyc_Toc_cast_it(_tmp7F8,_tmp7FB);{struct _tuple23 _tmp7FD=
Cyc_Toc_xlate_pat(nv,rgn,_tmp7F7,Cyc_Absyn_var_exp(_tmp7F6,0),_tmp7FB,_tmp7F0,
fail_stmt,decls);nv=_tmp7FD.f1;decls=_tmp7FD.f2;{struct Cyc_Absyn_Stmt*_tmp7FE=
_tmp7FD.f3;struct Cyc_Absyn_Stmt*_tmp7FF=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp7EC,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp800=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp7F6,0),_tmp7FC,0);s=Cyc_Absyn_seq_stmt(_tmp7FF,Cyc_Absyn_seq_stmt(_tmp800,
_tmp7FE,0),0);};};};};};}else{s=Cyc_Toc_skip_stmt_dl();for(0;_tmp7C5 != 0;_tmp7C5=
_tmp7C5->tl){struct _tuple25*_tmp801=(struct _tuple25*)_tmp7C5->hd;struct Cyc_Absyn_Pat*
_tmp802=(*_tmp801).f2;if(_tmp802->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _dyneither_ptr*f;{void*_tmp803=(void*)((struct Cyc_List_List*)_check_null((*
_tmp801).f1))->hd;struct _dyneither_ptr*_tmp805;_LL360: {struct Cyc_Absyn_FieldName_struct*
_tmp804=(struct Cyc_Absyn_FieldName_struct*)_tmp803;if(_tmp804->tag != 1)goto
_LL362;else{_tmp805=_tmp804->f1;}}_LL361: f=_tmp805;goto _LL35F;_LL362:;_LL363: {
struct Cyc_Toc_Match_error_struct _tmpD49;struct Cyc_Toc_Match_error_struct*_tmpD48;(
int)_throw((void*)((_tmpD48=_cycalloc_atomic(sizeof(*_tmpD48)),((_tmpD48[0]=((
_tmpD49.tag=Cyc_Toc_Match_error,_tmpD49)),_tmpD48)))));}_LL35F:;}{struct _tuple0*
_tmp808=Cyc_Toc_temp_var();void*_tmp809=(void*)_check_null(_tmp802->topt);void*
_tmp80A=Cyc_Toc_typ_to_c(_tmp809);{struct _tuple24*_tmpD4C;struct Cyc_List_List*
_tmpD4B;decls=((_tmpD4B=_region_malloc(rgn,sizeof(*_tmpD4B)),((_tmpD4B->hd=((
_tmpD4C=_region_malloc(rgn,sizeof(*_tmpD4C)),((_tmpD4C->f1=_tmp808,((_tmpD4C->f2=
_tmp80A,_tmpD4C)))))),((_tmpD4B->tl=decls,_tmpD4B))))));}{struct _tuple23 _tmp80D=
Cyc_Toc_xlate_pat(nv,rgn,_tmp809,Cyc_Absyn_var_exp(_tmp808,0),Cyc_Absyn_aggrmember_exp(
path,f,0),_tmp802,fail_stmt,decls);nv=_tmp80D.f1;decls=_tmp80D.f2;{struct Cyc_Absyn_Exp*
_tmp80E=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star_or_boxed_tvar(((
struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp7EC->impl))->fields,f)))->type))_tmp80E=Cyc_Toc_cast_it(_tmp80A,
_tmp80E);{struct Cyc_Absyn_Stmt*_tmp80F=_tmp80D.f3;struct Cyc_Absyn_Stmt*_tmp810=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp808,0),_tmp80E,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp810,_tmp80F,0),0);};};};};};}}goto _LL333;}_LL352: {
struct Cyc_Absyn_Pointer_p_struct*_tmp7C6=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp796;if(_tmp7C6->tag != 5)goto _LL354;else{_tmp7C7=_tmp7C6->f1;}}_LL353: {
struct _tuple0*_tmp811=Cyc_Toc_temp_var();void*_tmp812=(void*)_check_null(_tmp7C7->topt);{
struct _tuple24*_tmpD4F;struct Cyc_List_List*_tmpD4E;decls=((_tmpD4E=
_region_malloc(rgn,sizeof(*_tmpD4E)),((_tmpD4E->hd=((_tmpD4F=_region_malloc(rgn,
sizeof(*_tmpD4F)),((_tmpD4F->f1=_tmp811,((_tmpD4F->f2=Cyc_Toc_typ_to_c(_tmp812),
_tmpD4F)))))),((_tmpD4E->tl=decls,_tmpD4E))))));}{struct _tuple23 _tmp815=Cyc_Toc_xlate_pat(
nv,rgn,_tmp812,Cyc_Absyn_var_exp(_tmp811,0),Cyc_Absyn_deref_exp(path,0),_tmp7C7,
fail_stmt,decls);nv=_tmp815.f1;decls=_tmp815.f2;{struct Cyc_Absyn_Stmt*_tmp816=
_tmp815.f3;struct Cyc_Absyn_Stmt*_tmp817=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp811,0),Cyc_Absyn_deref_exp(r,0),0),_tmp816,0);if(Cyc_Toc_is_nullable(
t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(
0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),_tmp817,0);else{s=_tmp817;}goto
_LL333;};};}_LL354: {struct Cyc_Absyn_UnknownId_p_struct*_tmp7C8=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp796;if(_tmp7C8->tag != 14)goto _LL356;}_LL355: {const char*_tmpD52;void*_tmpD51;(
_tmpD51=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD52="unknownid pat",_tag_dyneither(_tmpD52,sizeof(char),14))),_tag_dyneither(
_tmpD51,sizeof(void*),0)));}_LL356: {struct Cyc_Absyn_UnknownCall_p_struct*
_tmp7C9=(struct Cyc_Absyn_UnknownCall_p_struct*)_tmp796;if(_tmp7C9->tag != 15)goto
_LL358;}_LL357: {const char*_tmpD55;void*_tmpD54;(_tmpD54=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD55="unknowncall pat",
_tag_dyneither(_tmpD55,sizeof(char),16))),_tag_dyneither(_tmpD54,sizeof(void*),0)));}
_LL358: {struct Cyc_Absyn_Exp_p_struct*_tmp7CA=(struct Cyc_Absyn_Exp_p_struct*)
_tmp796;if(_tmp7CA->tag != 16)goto _LL333;}_LL359: {const char*_tmpD58;void*_tmpD57;(
_tmpD57=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD58="exp pat",_tag_dyneither(_tmpD58,sizeof(char),8))),_tag_dyneither(
_tmpD57,sizeof(void*),0)));}_LL333:;}{struct _tuple23 _tmpD59;return(_tmpD59.f1=nv,((
_tmpD59.f2=decls,((_tmpD59.f3=s,_tmpD59)))));};}struct _tuple26{struct
_dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
static struct _tuple26*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){struct _tuple26*_tmpD5A;return(_tmpD5A=_region_malloc(r,sizeof(*_tmpD5A)),((
_tmpD5A->f1=Cyc_Toc_fresh_label(),((_tmpD5A->f2=Cyc_Toc_fresh_label(),((_tmpD5A->f3=
sc,_tmpD5A)))))));}static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{
void*_tmp820=(void*)_check_null(e->topt);int leave_as_switch;{void*_tmp821=Cyc_Tcutil_compress(
_tmp820);_LL365: {struct Cyc_Absyn_IntType_struct*_tmp822=(struct Cyc_Absyn_IntType_struct*)
_tmp821;if(_tmp822->tag != 6)goto _LL367;}_LL366: goto _LL368;_LL367: {struct Cyc_Absyn_EnumType_struct*
_tmp823=(struct Cyc_Absyn_EnumType_struct*)_tmp821;if(_tmp823->tag != 13)goto
_LL369;}_LL368: leave_as_switch=1;goto _LL364;_LL369:;_LL36A: leave_as_switch=0;
goto _LL364;_LL364:;}{struct Cyc_List_List*_tmp824=scs;for(0;_tmp824 != 0;_tmp824=
_tmp824->tl){if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Switch_clause*)_tmp824->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)
_tmp824->hd)->where_clause != 0){leave_as_switch=0;break;}}}if(leave_as_switch){
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{struct Cyc_List_List*_tmp825=
scs;for(0;_tmp825 != 0;_tmp825=_tmp825->tl){struct Cyc_Absyn_Stmt*_tmp826=((struct
Cyc_Absyn_Switch_clause*)_tmp825->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp825->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp826,0);next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp827=_new_region("rgn");struct _RegionHandle*rgn=& _tmp827;
_push_region(rgn);Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),
_tmp826);;_pop_region(rgn);};}}return;}{struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct _RegionHandle _tmp828=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp828;_push_region(rgn);{struct Cyc_Toc_Env*
_tmp829=Cyc_Toc_share_env(rgn,nv);struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple26*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,
rgn,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*
decls=0;{struct Cyc_List_List*_tmp82A=lscs;for(0;_tmp82A != 0;_tmp82A=_tmp82A->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple26*)_tmp82A->hd)).f3;struct
_dyneither_ptr*fail_lab=_tmp82A->tl == 0?end_l:(*((struct _tuple26*)((struct Cyc_List_List*)
_check_null(_tmp82A->tl))->hd)).f1;struct Cyc_Toc_Env*_tmp82C;struct Cyc_List_List*
_tmp82D;struct Cyc_Absyn_Stmt*_tmp82E;struct _tuple23 _tmp82B=Cyc_Toc_xlate_pat(
_tmp829,rgn,_tmp820,r,path,sc->pattern,Cyc_Absyn_goto_stmt(fail_lab,0),decls);
_tmp82C=_tmp82B.f1;_tmp82D=_tmp82B.f2;_tmp82E=_tmp82B.f3;if(sc->where_clause != 0){
struct Cyc_Absyn_Exp*_tmp82F=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp82C,_tmp82F);_tmp82E=Cyc_Absyn_seq_stmt(_tmp82E,Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp82F,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),
0),0);}decls=_tmp82D;{struct Cyc_List_List*_tmpD5B;nvs=((_tmpD5B=_region_malloc(
rgn,sizeof(*_tmpD5B)),((_tmpD5B->hd=_tmp82C,((_tmpD5B->tl=nvs,_tmpD5B))))));}{
struct Cyc_List_List*_tmpD5C;test_stmts=((_tmpD5C=_region_malloc(rgn,sizeof(*
_tmpD5C)),((_tmpD5C->hd=_tmp82E,((_tmpD5C->tl=test_stmts,_tmpD5C))))));};}}nvs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),
test_stmts=test_stmts->tl)){struct _tuple26 _tmp833;struct _dyneither_ptr*_tmp834;
struct _dyneither_ptr*_tmp835;struct Cyc_Absyn_Switch_clause*_tmp836;struct
_tuple26*_tmp832=(struct _tuple26*)lscs->hd;_tmp833=*_tmp832;_tmp834=_tmp833.f1;
_tmp835=_tmp833.f2;_tmp836=_tmp833.f3;{struct Cyc_Toc_Env*_tmp837=(struct Cyc_Toc_Env*)((
struct Cyc_List_List*)_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp836->body;
struct _RegionHandle _tmp838=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp838;
_push_region(rgn2);if(lscs->tl != 0){struct _tuple26 _tmp83A;struct _dyneither_ptr*
_tmp83B;struct Cyc_Absyn_Switch_clause*_tmp83C;struct _tuple26*_tmp839=(struct
_tuple26*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp83A=*_tmp839;
_tmp83B=_tmp83A.f2;_tmp83C=_tmp83A.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(
rgn2,_tmp837,end_l,_tmp83B,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp83C->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)
_check_null(nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(
rgn2,_tmp837,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp834,(
struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp835,s,0),0);{struct Cyc_List_List*_tmpD5D;stmts=((_tmpD5D=_region_malloc(rgn,
sizeof(*_tmpD5D)),((_tmpD5D->hd=s,((_tmpD5D->tl=stmts,_tmpD5D))))));};;
_pop_region(rgn2);};}{struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(
end_l,Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct
_tuple24 _tmp83F;struct _tuple0*_tmp840;void*_tmp841;struct _tuple24*_tmp83E=(
struct _tuple24*)decls->hd;_tmp83F=*_tmp83E;_tmp840=_tmp83F.f1;_tmp841=_tmp83F.f2;
res=Cyc_Absyn_declare_stmt(_tmp840,_tmp841,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(
v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),(struct Cyc_Absyn_Exp*)e,res,0))->r;};};};
_pop_region(rgn);};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*
init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler(int n){struct Cyc_List_List*_tmpD5E;return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,((_tmpD5E=_cycalloc(sizeof(*_tmpD5E)),((
_tmpD5E->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpD5E->tl=0,_tmpD5E)))))),
0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(
Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){while(1){void*_tmp843=s->r;struct
Cyc_Absyn_Exp*_tmp846;struct Cyc_Absyn_Stmt*_tmp848;struct Cyc_Absyn_Stmt*_tmp849;
struct Cyc_Absyn_Exp*_tmp84B;struct Cyc_Absyn_Exp*_tmp84D;struct Cyc_Absyn_Stmt*
_tmp84E;struct Cyc_Absyn_Stmt*_tmp84F;struct _tuple9 _tmp851;struct Cyc_Absyn_Exp*
_tmp852;struct Cyc_Absyn_Stmt*_tmp853;struct Cyc_Absyn_Stmt*_tmp855;struct Cyc_Absyn_Stmt*
_tmp857;struct Cyc_Absyn_Stmt*_tmp859;struct Cyc_Absyn_Exp*_tmp85B;struct _tuple9
_tmp85C;struct Cyc_Absyn_Exp*_tmp85D;struct _tuple9 _tmp85E;struct Cyc_Absyn_Exp*
_tmp85F;struct Cyc_Absyn_Stmt*_tmp860;struct Cyc_Absyn_Exp*_tmp862;struct Cyc_List_List*
_tmp863;struct Cyc_List_List*_tmp865;struct Cyc_Absyn_Switch_clause**_tmp866;
struct Cyc_Absyn_Decl*_tmp868;struct Cyc_Absyn_Stmt*_tmp869;struct _dyneither_ptr*
_tmp86B;struct Cyc_Absyn_Stmt*_tmp86C;struct Cyc_Absyn_Stmt*_tmp86E;struct _tuple9
_tmp86F;struct Cyc_Absyn_Exp*_tmp870;struct Cyc_Absyn_Stmt*_tmp872;struct Cyc_List_List*
_tmp873;struct Cyc_Absyn_Exp*_tmp875;_LL36C: {struct Cyc_Absyn_Skip_s_struct*
_tmp844=(struct Cyc_Absyn_Skip_s_struct*)_tmp843;if(_tmp844->tag != 0)goto _LL36E;}
_LL36D: return;_LL36E: {struct Cyc_Absyn_Exp_s_struct*_tmp845=(struct Cyc_Absyn_Exp_s_struct*)
_tmp843;if(_tmp845->tag != 1)goto _LL370;else{_tmp846=_tmp845->f1;}}_LL36F: Cyc_Toc_exp_to_c(
nv,_tmp846);return;_LL370: {struct Cyc_Absyn_Seq_s_struct*_tmp847=(struct Cyc_Absyn_Seq_s_struct*)
_tmp843;if(_tmp847->tag != 2)goto _LL372;else{_tmp848=_tmp847->f1;_tmp849=_tmp847->f2;}}
_LL371: Cyc_Toc_stmt_to_c(nv,_tmp848);s=_tmp849;continue;_LL372: {struct Cyc_Absyn_Return_s_struct*
_tmp84A=(struct Cyc_Absyn_Return_s_struct*)_tmp843;if(_tmp84A->tag != 3)goto _LL374;
else{_tmp84B=_tmp84A->f1;}}_LL373: {void*topt=0;if(_tmp84B != 0){topt=(void*)Cyc_Toc_typ_to_c((
void*)_check_null(_tmp84B->topt));Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp84B);}if(s->try_depth > 0){if(topt != 0){struct _tuple0*_tmp876=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp877=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(
_tmp876,0),0);s->r=(Cyc_Absyn_declare_stmt(_tmp876,(void*)topt,_tmp84B,Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler(s->try_depth),_tmp877,0),0))->r;}else{Cyc_Toc_do_npop_before(
s->try_depth,s);}}return;}_LL374: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp84C=(
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp843;if(_tmp84C->tag != 4)goto _LL376;
else{_tmp84D=_tmp84C->f1;_tmp84E=_tmp84C->f2;_tmp84F=_tmp84C->f3;}}_LL375: Cyc_Toc_exp_to_c(
nv,_tmp84D);Cyc_Toc_stmt_to_c(nv,_tmp84E);s=_tmp84F;continue;_LL376: {struct Cyc_Absyn_While_s_struct*
_tmp850=(struct Cyc_Absyn_While_s_struct*)_tmp843;if(_tmp850->tag != 5)goto _LL378;
else{_tmp851=_tmp850->f1;_tmp852=_tmp851.f1;_tmp853=_tmp850->f2;}}_LL377: Cyc_Toc_exp_to_c(
nv,_tmp852);{struct _RegionHandle _tmp878=_new_region("temp");struct _RegionHandle*
temp=& _tmp878;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),
_tmp853);_npop_handler(0);return;;_pop_region(temp);};_LL378: {struct Cyc_Absyn_Break_s_struct*
_tmp854=(struct Cyc_Absyn_Break_s_struct*)_tmp843;if(_tmp854->tag != 6)goto _LL37A;
else{_tmp855=_tmp854->f1;}}_LL379: {struct Cyc_Toc_Env _tmp87A;struct
_dyneither_ptr**_tmp87B;struct Cyc_Toc_Env*_tmp879=nv;_tmp87A=*_tmp879;_tmp87B=
_tmp87A.break_lab;if(_tmp87B != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp87B,0);{int
dest_depth=_tmp855 == 0?0: _tmp855->try_depth;Cyc_Toc_do_npop_before(s->try_depth - 
dest_depth,s);return;};}_LL37A: {struct Cyc_Absyn_Continue_s_struct*_tmp856=(
struct Cyc_Absyn_Continue_s_struct*)_tmp843;if(_tmp856->tag != 7)goto _LL37C;else{
_tmp857=_tmp856->f1;}}_LL37B: {struct Cyc_Toc_Env _tmp87D;struct _dyneither_ptr**
_tmp87E;struct Cyc_Toc_Env*_tmp87C=nv;_tmp87D=*_tmp87C;_tmp87E=_tmp87D.continue_lab;
if(_tmp87E != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp87E,0);_tmp859=_tmp857;goto _LL37D;}
_LL37C: {struct Cyc_Absyn_Goto_s_struct*_tmp858=(struct Cyc_Absyn_Goto_s_struct*)
_tmp843;if(_tmp858->tag != 8)goto _LL37E;else{_tmp859=_tmp858->f2;}}_LL37D: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp859))->try_depth,s);
return;_LL37E: {struct Cyc_Absyn_For_s_struct*_tmp85A=(struct Cyc_Absyn_For_s_struct*)
_tmp843;if(_tmp85A->tag != 9)goto _LL380;else{_tmp85B=_tmp85A->f1;_tmp85C=_tmp85A->f2;
_tmp85D=_tmp85C.f1;_tmp85E=_tmp85A->f3;_tmp85F=_tmp85E.f1;_tmp860=_tmp85A->f4;}}
_LL37F: Cyc_Toc_exp_to_c(nv,_tmp85B);Cyc_Toc_exp_to_c(nv,_tmp85D);Cyc_Toc_exp_to_c(
nv,_tmp85F);{struct _RegionHandle _tmp87F=_new_region("temp");struct _RegionHandle*
temp=& _tmp87F;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),
_tmp860);_npop_handler(0);return;;_pop_region(temp);};_LL380: {struct Cyc_Absyn_Switch_s_struct*
_tmp861=(struct Cyc_Absyn_Switch_s_struct*)_tmp843;if(_tmp861->tag != 10)goto
_LL382;else{_tmp862=_tmp861->f1;_tmp863=_tmp861->f2;}}_LL381: Cyc_Toc_xlate_switch(
nv,s,_tmp862,_tmp863);return;_LL382: {struct Cyc_Absyn_Fallthru_s_struct*_tmp864=(
struct Cyc_Absyn_Fallthru_s_struct*)_tmp843;if(_tmp864->tag != 11)goto _LL384;else{
_tmp865=_tmp864->f1;_tmp866=_tmp864->f2;}}_LL383: {struct Cyc_Toc_Env _tmp881;
struct Cyc_Toc_FallthruInfo*_tmp882;struct Cyc_Toc_Env*_tmp880=nv;_tmp881=*_tmp880;
_tmp882=_tmp881.fallthru_info;if(_tmp882 == 0){const char*_tmpD61;void*_tmpD60;(
_tmpD60=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD61="fallthru in unexpected place",_tag_dyneither(_tmpD61,sizeof(char),29))),
_tag_dyneither(_tmpD60,sizeof(void*),0)));}{struct _dyneither_ptr*_tmp886;struct
Cyc_List_List*_tmp887;struct Cyc_Dict_Dict _tmp888;struct Cyc_Toc_FallthruInfo
_tmp885=*_tmp882;_tmp886=_tmp885.label;_tmp887=_tmp885.binders;_tmp888=_tmp885.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp886,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp866)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp889=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp887);struct Cyc_List_List*_tmp88A=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp865);for(0;_tmp889 != 0;(_tmp889=_tmp889->tl,
_tmp88A=_tmp88A->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp88A))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp888,(
struct _tuple0*)_tmp889->hd),(struct Cyc_Absyn_Exp*)_tmp88A->hd,0),s2,0);}s->r=s2->r;
return;};};};}_LL384: {struct Cyc_Absyn_Decl_s_struct*_tmp867=(struct Cyc_Absyn_Decl_s_struct*)
_tmp843;if(_tmp867->tag != 12)goto _LL386;else{_tmp868=_tmp867->f1;_tmp869=_tmp867->f2;}}
_LL385:{void*_tmp88B=_tmp868->r;struct Cyc_Absyn_Vardecl*_tmp88D;struct Cyc_Absyn_Pat*
_tmp88F;struct Cyc_Absyn_Exp*_tmp890;struct Cyc_List_List*_tmp892;struct Cyc_Absyn_Fndecl*
_tmp894;struct Cyc_Absyn_Tvar*_tmp896;struct Cyc_Absyn_Vardecl*_tmp897;int _tmp898;
struct Cyc_Absyn_Exp*_tmp899;struct Cyc_Absyn_Exp*_tmp89B;struct Cyc_Absyn_Tvar*
_tmp89C;struct Cyc_Absyn_Vardecl*_tmp89D;_LL38F: {struct Cyc_Absyn_Var_d_struct*
_tmp88C=(struct Cyc_Absyn_Var_d_struct*)_tmp88B;if(_tmp88C->tag != 0)goto _LL391;
else{_tmp88D=_tmp88C->f1;}}_LL390: {struct _RegionHandle _tmp89E=_new_region("temp");
struct _RegionHandle*temp=& _tmp89E;_push_region(temp);{struct Cyc_Absyn_Local_b_struct
_tmpD64;struct Cyc_Absyn_Local_b_struct*_tmpD63;struct Cyc_Toc_Env*_tmp89F=Cyc_Toc_add_varmap(
temp,nv,_tmp88D->name,Cyc_Absyn_varb_exp(_tmp88D->name,(void*)((_tmpD63=
_cycalloc(sizeof(*_tmpD63)),((_tmpD63[0]=((_tmpD64.tag=4,((_tmpD64.f1=_tmp88D,
_tmpD64)))),_tmpD63)))),0));Cyc_Toc_local_decl_to_c(_tmp89F,_tmp89F,_tmp88D,
_tmp869);}_npop_handler(0);goto _LL38E;;_pop_region(temp);}_LL391: {struct Cyc_Absyn_Let_d_struct*
_tmp88E=(struct Cyc_Absyn_Let_d_struct*)_tmp88B;if(_tmp88E->tag != 2)goto _LL393;
else{_tmp88F=_tmp88E->f1;_tmp890=_tmp88E->f3;}}_LL392:{void*_tmp8A2=_tmp88F->r;
struct Cyc_Absyn_Vardecl*_tmp8A4;struct Cyc_Absyn_Pat*_tmp8A5;struct Cyc_Absyn_Pat
_tmp8A6;void*_tmp8A7;_LL39E: {struct Cyc_Absyn_Var_p_struct*_tmp8A3=(struct Cyc_Absyn_Var_p_struct*)
_tmp8A2;if(_tmp8A3->tag != 1)goto _LL3A0;else{_tmp8A4=_tmp8A3->f1;_tmp8A5=_tmp8A3->f2;
_tmp8A6=*_tmp8A5;_tmp8A7=_tmp8A6.r;{struct Cyc_Absyn_Wild_p_struct*_tmp8A8=(
struct Cyc_Absyn_Wild_p_struct*)_tmp8A7;if(_tmp8A8->tag != 0)goto _LL3A0;};}}_LL39F: {
struct _tuple0*old_name=_tmp8A4->name;struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp8A4->name=new_name;_tmp8A4->initializer=(struct Cyc_Absyn_Exp*)_tmp890;{
struct Cyc_Absyn_Var_d_struct _tmpD67;struct Cyc_Absyn_Var_d_struct*_tmpD66;_tmp868->r=(
void*)((_tmpD66=_cycalloc(sizeof(*_tmpD66)),((_tmpD66[0]=((_tmpD67.tag=0,((
_tmpD67.f1=_tmp8A4,_tmpD67)))),_tmpD66))));}{struct _RegionHandle _tmp8AB=
_new_region("temp");struct _RegionHandle*temp=& _tmp8AB;_push_region(temp);{struct
Cyc_Absyn_Local_b_struct _tmpD6A;struct Cyc_Absyn_Local_b_struct*_tmpD69;struct Cyc_Toc_Env*
_tmp8AC=Cyc_Toc_add_varmap(temp,nv,old_name,Cyc_Absyn_varb_exp(new_name,(void*)((
_tmpD69=_cycalloc(sizeof(*_tmpD69)),((_tmpD69[0]=((_tmpD6A.tag=4,((_tmpD6A.f1=
_tmp8A4,_tmpD6A)))),_tmpD69)))),0));Cyc_Toc_local_decl_to_c(_tmp8AC,nv,_tmp8A4,
_tmp869);}_npop_handler(0);goto _LL39D;;_pop_region(temp);};}_LL3A0:;_LL3A1: s->r=(
Cyc_Toc_letdecl_to_c(nv,_tmp88F,(void*)_check_null(_tmp890->topt),_tmp890,
_tmp869))->r;goto _LL39D;_LL39D:;}goto _LL38E;_LL393: {struct Cyc_Absyn_Letv_d_struct*
_tmp891=(struct Cyc_Absyn_Letv_d_struct*)_tmp88B;if(_tmp891->tag != 3)goto _LL395;
else{_tmp892=_tmp891->f1;}}_LL394: {struct Cyc_List_List*_tmp8AF=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp892);if(_tmp8AF == 0){const char*_tmpD6D;
void*_tmpD6C;(_tmpD6C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpD6D="empty Letv_d",_tag_dyneither(_tmpD6D,sizeof(char),
13))),_tag_dyneither(_tmpD6C,sizeof(void*),0)));}{struct Cyc_Absyn_Var_d_struct
_tmpD70;struct Cyc_Absyn_Var_d_struct*_tmpD6F;_tmp868->r=(void*)((_tmpD6F=
_cycalloc(sizeof(*_tmpD6F)),((_tmpD6F[0]=((_tmpD70.tag=0,((_tmpD70.f1=(struct Cyc_Absyn_Vardecl*)
_tmp8AF->hd,_tmpD70)))),_tmpD6F))));}_tmp8AF=_tmp8AF->tl;for(0;_tmp8AF != 0;
_tmp8AF=_tmp8AF->tl){struct Cyc_Absyn_Var_d_struct _tmpD73;struct Cyc_Absyn_Var_d_struct*
_tmpD72;struct Cyc_Absyn_Decl*_tmp8B4=Cyc_Absyn_new_decl((void*)((_tmpD72=
_cycalloc(sizeof(*_tmpD72)),((_tmpD72[0]=((_tmpD73.tag=0,((_tmpD73.f1=(struct Cyc_Absyn_Vardecl*)
_tmp8AF->hd,_tmpD73)))),_tmpD72)))),0);s->r=(Cyc_Absyn_decl_stmt(_tmp8B4,Cyc_Absyn_new_stmt(
s->r,0),0))->r;}Cyc_Toc_stmt_to_c(nv,s);goto _LL38E;}_LL395: {struct Cyc_Absyn_Fn_d_struct*
_tmp893=(struct Cyc_Absyn_Fn_d_struct*)_tmp88B;if(_tmp893->tag != 1)goto _LL397;
else{_tmp894=_tmp893->f1;}}_LL396: {struct _tuple0*_tmp8B7=_tmp894->name;struct
_RegionHandle _tmp8B8=_new_region("temp");struct _RegionHandle*temp=& _tmp8B8;
_push_region(temp);{struct Cyc_Toc_Env*_tmp8B9=Cyc_Toc_add_varmap(temp,nv,_tmp894->name,
Cyc_Absyn_var_exp(_tmp8B7,0));Cyc_Toc_fndecl_to_c(_tmp8B9,_tmp894,0);Cyc_Toc_stmt_to_c(
_tmp8B9,_tmp869);}_npop_handler(0);goto _LL38E;;_pop_region(temp);}_LL397: {
struct Cyc_Absyn_Region_d_struct*_tmp895=(struct Cyc_Absyn_Region_d_struct*)
_tmp88B;if(_tmp895->tag != 4)goto _LL399;else{_tmp896=_tmp895->f1;_tmp897=_tmp895->f2;
_tmp898=_tmp895->f3;_tmp899=_tmp895->f4;}}_LL398: {struct Cyc_Absyn_Stmt*_tmp8BA=
_tmp869;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple0*
rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp897->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct _RegionHandle _tmp8BB=_new_region("temp");struct _RegionHandle*temp=&
_tmp8BB;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,
x_exp),_tmp8BA);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp8BA,0))->r;
else{if(_tmp899 == 0){struct Cyc_Absyn_Exp*_tmpD76[1];struct Cyc_Absyn_Exp*_tmpD75[
1];struct Cyc_List_List*_tmpD74;s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpD74=
_cycalloc(sizeof(*_tmpD74)),((_tmpD74->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0),((_tmpD74->tl=0,_tmpD74)))))),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD75[0]=
x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD75,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp8BA,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD76[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD76,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp899);{struct Cyc_Absyn_Exp*
_tmpD78[1];struct Cyc_Absyn_Exp*_tmpD77[2];s->r=(Cyc_Absyn_declare_stmt(rh_var,
Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpD77[1]=(struct Cyc_Absyn_Exp*)_tmp899,((_tmpD77[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD77,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp8BA,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpD78[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD78,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
_npop_handler(0);return;;_pop_region(temp);}_LL399: {struct Cyc_Absyn_Alias_d_struct*
_tmp89A=(struct Cyc_Absyn_Alias_d_struct*)_tmp88B;if(_tmp89A->tag != 5)goto _LL39B;
else{_tmp89B=_tmp89A->f1;_tmp89C=_tmp89A->f2;_tmp89D=_tmp89A->f3;}}_LL39A: {
struct _tuple0*old_name=_tmp89D->name;struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp89D->name=new_name;_tmp89D->initializer=(struct Cyc_Absyn_Exp*)_tmp89B;{
struct Cyc_Absyn_Decl_s_struct _tmpD87;struct Cyc_Absyn_Var_d_struct*_tmpD86;struct
Cyc_Absyn_Var_d_struct _tmpD85;struct Cyc_Absyn_Decl*_tmpD84;struct Cyc_Absyn_Decl_s_struct*
_tmpD83;s->r=(void*)((_tmpD83=_cycalloc(sizeof(*_tmpD83)),((_tmpD83[0]=((_tmpD87.tag=
12,((_tmpD87.f1=((_tmpD84=_cycalloc(sizeof(*_tmpD84)),((_tmpD84->r=(void*)((
_tmpD86=_cycalloc(sizeof(*_tmpD86)),((_tmpD86[0]=((_tmpD85.tag=0,((_tmpD85.f1=
_tmp89D,_tmpD85)))),_tmpD86)))),((_tmpD84->loc=0,_tmpD84)))))),((_tmpD87.f2=
_tmp869,_tmpD87)))))),_tmpD83))));}{struct _RegionHandle _tmp8C6=_new_region("temp");
struct _RegionHandle*temp=& _tmp8C6;_push_region(temp);{struct Cyc_Absyn_Local_b_struct
_tmpD8A;struct Cyc_Absyn_Local_b_struct*_tmpD89;struct Cyc_Toc_Env*_tmp8C7=Cyc_Toc_add_varmap(
temp,nv,old_name,Cyc_Absyn_varb_exp(new_name,(void*)((_tmpD89=_cycalloc(sizeof(*
_tmpD89)),((_tmpD89[0]=((_tmpD8A.tag=4,((_tmpD8A.f1=_tmp89D,_tmpD8A)))),_tmpD89)))),
0));Cyc_Toc_local_decl_to_c(_tmp8C7,nv,_tmp89D,_tmp869);}_npop_handler(0);
return;;_pop_region(temp);};}_LL39B:;_LL39C: {const char*_tmpD8D;void*_tmpD8C;(
_tmpD8C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD8D="bad nested declaration within function",_tag_dyneither(_tmpD8D,sizeof(
char),39))),_tag_dyneither(_tmpD8C,sizeof(void*),0)));}_LL38E:;}return;_LL386: {
struct Cyc_Absyn_Label_s_struct*_tmp86A=(struct Cyc_Absyn_Label_s_struct*)_tmp843;
if(_tmp86A->tag != 13)goto _LL388;else{_tmp86B=_tmp86A->f1;_tmp86C=_tmp86A->f2;}}
_LL387: s=_tmp86C;continue;_LL388: {struct Cyc_Absyn_Do_s_struct*_tmp86D=(struct
Cyc_Absyn_Do_s_struct*)_tmp843;if(_tmp86D->tag != 14)goto _LL38A;else{_tmp86E=
_tmp86D->f1;_tmp86F=_tmp86D->f2;_tmp870=_tmp86F.f1;}}_LL389: {struct
_RegionHandle _tmp8CC=_new_region("temp");struct _RegionHandle*temp=& _tmp8CC;
_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp86E);Cyc_Toc_exp_to_c(
nv,_tmp870);_npop_handler(0);return;;_pop_region(temp);}_LL38A: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp871=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp843;if(_tmp871->tag != 15)goto
_LL38C;else{_tmp872=_tmp871->f1;_tmp873=_tmp871->f2;}}_LL38B: {struct _tuple0*
h_var=Cyc_Toc_temp_var();struct _tuple0*e_var=Cyc_Toc_temp_var();struct _tuple0*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);e_exp->topt=(void*)e_typ;{
struct _RegionHandle _tmp8CD=_new_region("temp");struct _RegionHandle*temp=& _tmp8CD;
_push_region(temp);{struct Cyc_Toc_Env*_tmp8CE=Cyc_Toc_add_varmap(temp,nv,e_var,
e_exp);Cyc_Toc_stmt_to_c(_tmp8CE,_tmp872);{struct Cyc_Absyn_Stmt*_tmp8CF=Cyc_Absyn_seq_stmt(
_tmp872,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
struct _tuple0*_tmp8D0=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp8D1=Cyc_Absyn_var_exp(
_tmp8D0,0);struct Cyc_Absyn_Vardecl*_tmp8D2=Cyc_Absyn_new_vardecl(_tmp8D0,Cyc_Absyn_exn_typ(),
0);_tmp8D1->topt=(void*)Cyc_Absyn_exn_typ();{struct Cyc_Absyn_Var_p_struct*
_tmpD97;struct Cyc_Absyn_Pat*_tmpD96;struct Cyc_Absyn_Var_p_struct _tmpD95;struct
Cyc_Absyn_Pat*_tmpD94;struct Cyc_Absyn_Pat*_tmp8D3=(_tmpD94=_cycalloc(sizeof(*
_tmpD94)),((_tmpD94->r=(void*)((_tmpD97=_cycalloc(sizeof(*_tmpD97)),((_tmpD97[0]=((
_tmpD95.tag=1,((_tmpD95.f1=_tmp8D2,((_tmpD95.f2=((_tmpD96=_cycalloc(sizeof(*
_tmpD96)),((_tmpD96->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpD96->topt=(void*)Cyc_Absyn_exn_typ(),((
_tmpD96->loc=0,_tmpD96)))))))),_tmpD95)))))),_tmpD97)))),((_tmpD94->topt=(void*)
Cyc_Absyn_exn_typ(),((_tmpD94->loc=0,_tmpD94)))))));struct Cyc_Absyn_Exp*_tmp8D4=
Cyc_Absyn_throw_exp(_tmp8D1,0);_tmp8D4->topt=(void*)& Cyc_Absyn_VoidType_val;{
struct Cyc_Absyn_Stmt*_tmp8D5=Cyc_Absyn_exp_stmt(_tmp8D4,0);struct Cyc_Core_Opt*
_tmpD9D;struct Cyc_List_List*_tmpD9C;struct Cyc_Absyn_Switch_clause*_tmpD9B;struct
Cyc_Absyn_Switch_clause*_tmp8D6=(_tmpD9B=_cycalloc(sizeof(*_tmpD9B)),((_tmpD9B->pattern=
_tmp8D3,((_tmpD9B->pat_vars=((_tmpD9D=_cycalloc(sizeof(*_tmpD9D)),((_tmpD9D->v=((
_tmpD9C=_cycalloc(sizeof(*_tmpD9C)),((_tmpD9C->hd=_tmp8D2,((_tmpD9C->tl=0,
_tmpD9C)))))),_tmpD9D)))),((_tmpD9B->where_clause=0,((_tmpD9B->body=_tmp8D5,((
_tmpD9B->loc=0,_tmpD9B)))))))))));struct Cyc_List_List*_tmpD9E;struct Cyc_Absyn_Stmt*
_tmp8D7=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp873,((_tmpD9E=_cycalloc(sizeof(*
_tmpD9E)),((_tmpD9E->hd=_tmp8D6,((_tmpD9E->tl=0,_tmpD9E))))))),0);Cyc_Toc_stmt_to_c(
_tmp8CE,_tmp8D7);{struct Cyc_List_List*_tmpD9F;struct Cyc_Absyn_Exp*_tmp8D8=Cyc_Absyn_fncall_exp(
Cyc_Toc_setjmp_e,((_tmpD9F=_cycalloc(sizeof(*_tmpD9F)),((_tmpD9F->hd=Cyc_Absyn_aggrmember_exp(
h_exp,Cyc_Toc_handler_sp,0),((_tmpD9F->tl=0,_tmpD9F)))))),0);struct Cyc_List_List*
_tmpDA0;struct Cyc_Absyn_Stmt*_tmp8D9=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpDA0=_cycalloc(sizeof(*_tmpDA0)),((_tmpDA0->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpDA0->tl=0,_tmpDA0)))))),0),0);struct Cyc_Absyn_Exp*_tmp8DA=Cyc_Absyn_int_exp(
Cyc_Absyn_Signed,0,0);struct Cyc_Absyn_Exp*_tmp8DB=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
1,0);s->r=(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp8D9,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp8DA,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp8D8,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp8DB,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp8CF,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp8D7,0),0),0),0),0),0))->r;};};};};}
_npop_handler(0);return;;_pop_region(temp);};}_LL38C: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp874=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp843;if(_tmp874->tag != 16)goto
_LL36B;else{_tmp875=_tmp874->f1;}}_LL38D: if(Cyc_Absyn_no_regions)s->r=(void*)&
Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp875);{struct Cyc_List_List*
_tmpDA1;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((
_tmpDA1=_cycalloc(sizeof(*_tmpDA1)),((_tmpDA1->hd=_tmp875,((_tmpDA1->tl=0,
_tmpDA1)))))),0));};}return;_LL36B:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,
int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8E7=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8E7;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp8E8=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp8E9=f->args;for(0;_tmp8E9 != 0;_tmp8E9=_tmp8E9->tl){struct
_tuple0*_tmpDA2;struct _tuple0*_tmp8EA=(_tmpDA2=_cycalloc(sizeof(*_tmpDA2)),((
_tmpDA2->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpDA2->f2=(*((struct
_tuple8*)_tmp8E9->hd)).f1,_tmpDA2)))));(*((struct _tuple8*)_tmp8E9->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple8*)_tmp8E9->hd)).f3);_tmp8E8=Cyc_Toc_add_varmap(frgn,_tmp8E8,_tmp8EA,
Cyc_Absyn_var_exp(_tmp8EA,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct _dyneither_ptr*_tmp8ED;struct Cyc_Absyn_Tqual
_tmp8EE;void*_tmp8EF;int _tmp8F0;struct Cyc_Absyn_VarargInfo _tmp8EC=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp8ED=_tmp8EC.name;_tmp8EE=_tmp8EC.tq;_tmp8EF=
_tmp8EC.type;_tmp8F0=_tmp8EC.inject;{void*_tmp8F1=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp8EF,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8EE,Cyc_Absyn_false_conref));struct
_tuple0*_tmpDA3;struct _tuple0*_tmp8F2=(_tmpDA3=_cycalloc(sizeof(*_tmpDA3)),((
_tmpDA3->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpDA3->f2=(struct
_dyneither_ptr*)_check_null(_tmp8ED),_tmpDA3)))));{struct _tuple8*_tmpDA6;struct
Cyc_List_List*_tmpDA5;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpDA5=_cycalloc(sizeof(*
_tmpDA5)),((_tmpDA5->hd=((_tmpDA6=_cycalloc(sizeof(*_tmpDA6)),((_tmpDA6->f1=(
struct _dyneither_ptr*)_tmp8ED,((_tmpDA6->f2=_tmp8EE,((_tmpDA6->f3=_tmp8F1,
_tmpDA6)))))))),((_tmpDA5->tl=0,_tmpDA5)))))));}_tmp8E8=Cyc_Toc_add_varmap(frgn,
_tmp8E8,_tmp8F2,Cyc_Absyn_var_exp(_tmp8F2,0));f->cyc_varargs=0;};}{struct Cyc_List_List*
_tmp8F6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp8F6 != 0;_tmp8F6=_tmp8F6->tl){((struct Cyc_Absyn_Vardecl*)_tmp8F6->hd)->type=
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp8F6->hd)->type);}}Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel(frgn,_tmp8E8),f->body);};_pop_region(frgn);};}static enum 
Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){switch(s){case Cyc_Absyn_Abstract:
_LL3A2: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3A3: return Cyc_Absyn_Extern;
default: _LL3A4: return s;}}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad,int add_to_decls){struct _tuple0*_tmp8F7=ad->name;struct _DynRegionHandle*
_tmp8F9;struct Cyc_Dict_Dict*_tmp8FA;struct Cyc_Toc_TocState _tmp8F8=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp8F9=_tmp8F8.dyn;_tmp8FA=_tmp8F8.aggrs_so_far;{
struct _DynRegionFrame _tmp8FB;struct _RegionHandle*d=_open_dynregion(& _tmp8FB,
_tmp8F9);{int seen_defn_before;struct _tuple15**_tmp8FC=((struct _tuple15**(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp8FA,_tmp8F7);if(
_tmp8FC == 0){seen_defn_before=0;{struct _tuple15*v;if(ad->kind == Cyc_Absyn_StructA){
struct _tuple15*_tmpDA7;v=((_tmpDA7=_region_malloc(d,sizeof(*_tmpDA7)),((_tmpDA7->f1=
ad,((_tmpDA7->f2=Cyc_Absyn_strctq(_tmp8F7),_tmpDA7))))));}else{struct _tuple15*
_tmpDA8;v=((_tmpDA8=_region_malloc(d,sizeof(*_tmpDA8)),((_tmpDA8->f1=ad,((
_tmpDA8->f2=Cyc_Absyn_unionq_typ(_tmp8F7),_tmpDA8))))));}*_tmp8FA=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp8FA,
_tmp8F7,v);};}else{struct _tuple15 _tmp900;struct Cyc_Absyn_Aggrdecl*_tmp901;void*
_tmp902;struct _tuple15*_tmp8FF=*_tmp8FC;_tmp900=*_tmp8FF;_tmp901=_tmp900.f1;
_tmp902=_tmp900.f2;if(_tmp901->impl == 0){{struct _tuple15*_tmpDA9;*_tmp8FA=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))
Cyc_Dict_insert)(*_tmp8FA,_tmp8F7,((_tmpDA9=_region_malloc(d,sizeof(*_tmpDA9)),((
_tmpDA9->f1=ad,((_tmpDA9->f2=_tmp902,_tmpDA9)))))));}seen_defn_before=0;}else{
seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*_tmpDAA;struct Cyc_Absyn_Aggrdecl*
new_ad=(_tmpDAA=_cycalloc(sizeof(*_tmpDAA)),((_tmpDAA->kind=ad->kind,((_tmpDAA->sc=
Cyc_Absyn_Public,((_tmpDAA->name=ad->name,((_tmpDAA->tvs=0,((_tmpDAA->impl=0,((
_tmpDAA->attributes=ad->attributes,_tmpDAA)))))))))))));if(ad->impl != 0  && !
seen_defn_before){{struct Cyc_Absyn_AggrdeclImpl*_tmpDAB;new_ad->impl=((_tmpDAB=
_cycalloc(sizeof(*_tmpDAB)),((_tmpDAB->exist_vars=0,((_tmpDAB->rgn_po=0,((
_tmpDAB->fields=0,((_tmpDAB->tagged=0,_tmpDAB))))))))));}{struct Cyc_List_List*
new_fields=0;{struct Cyc_List_List*_tmp905=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp905 != 0;_tmp905=_tmp905->tl){struct Cyc_Absyn_Aggrfield*
_tmp906=(struct Cyc_Absyn_Aggrfield*)_tmp905->hd;if(_tmp905->tl == 0  && Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp906->type)))continue;{struct Cyc_Absyn_Aggrfield*
_tmpDAC;struct Cyc_Absyn_Aggrfield*_tmp907=(_tmpDAC=_cycalloc(sizeof(*_tmpDAC)),((
_tmpDAC->name=_tmp906->name,((_tmpDAC->tq=Cyc_Toc_mt_tq,((_tmpDAC->type=Cyc_Toc_typ_to_c_array(
_tmp906->type),((_tmpDAC->width=_tmp906->width,((_tmpDAC->attributes=_tmp906->attributes,
_tmpDAC)))))))))));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp908=_tmp907->type;struct _dyneither_ptr*_tmp909=_tmp907->name;const char*
_tmpDB1;void*_tmpDB0[2];struct Cyc_String_pa_struct _tmpDAF;struct Cyc_String_pa_struct
_tmpDAE;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpDAE.tag=0,((_tmpDAE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp909),((_tmpDAF.tag=0,((_tmpDAF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpDB0[0]=&
_tmpDAF,((_tmpDB0[1]=& _tmpDAE,Cyc_aprintf(((_tmpDB1="_union_%s_%s",
_tag_dyneither(_tmpDB1,sizeof(char),13))),_tag_dyneither(_tmpDB0,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpDB2;struct _dyneither_ptr*str=(_tmpDB2=_cycalloc(sizeof(*
_tmpDB2)),((_tmpDB2[0]=s,_tmpDB2)));struct Cyc_Absyn_Aggrfield*_tmpDB3;struct Cyc_Absyn_Aggrfield*
_tmp90A=(_tmpDB3=_cycalloc(sizeof(*_tmpDB3)),((_tmpDB3->name=Cyc_Toc_val_sp,((
_tmpDB3->tq=Cyc_Toc_mt_tq,((_tmpDB3->type=_tmp908,((_tmpDB3->width=0,((_tmpDB3->attributes=
0,_tmpDB3)))))))))));struct Cyc_Absyn_Aggrfield*_tmpDB4;struct Cyc_Absyn_Aggrfield*
_tmp90B=(_tmpDB4=_cycalloc(sizeof(*_tmpDB4)),((_tmpDB4->name=Cyc_Toc_tag_sp,((
_tmpDB4->tq=Cyc_Toc_mt_tq,((_tmpDB4->type=Cyc_Absyn_sint_typ,((_tmpDB4->width=0,((
_tmpDB4->attributes=0,_tmpDB4)))))))))));struct Cyc_Absyn_Aggrfield*_tmpDB5[2];
struct Cyc_List_List*_tmp90C=(_tmpDB5[1]=_tmp90A,((_tmpDB5[0]=_tmp90B,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDB5,sizeof(struct Cyc_Absyn_Aggrfield*),
2)))));struct Cyc_Absyn_AggrdeclImpl*_tmpDBA;struct _tuple0*_tmpDB9;struct Cyc_Absyn_Aggrdecl*
_tmpDB8;struct Cyc_Absyn_Aggrdecl*_tmp90D=(_tmpDB8=_cycalloc(sizeof(*_tmpDB8)),((
_tmpDB8->kind=Cyc_Absyn_StructA,((_tmpDB8->sc=Cyc_Absyn_Public,((_tmpDB8->name=((
_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9->f1=Cyc_Absyn_Loc_n,((_tmpDB9->f2=
str,_tmpDB9)))))),((_tmpDB8->tvs=0,((_tmpDB8->impl=((_tmpDBA=_cycalloc(sizeof(*
_tmpDBA)),((_tmpDBA->exist_vars=0,((_tmpDBA->rgn_po=0,((_tmpDBA->fields=_tmp90C,((
_tmpDBA->tagged=0,_tmpDBA)))))))))),((_tmpDB8->attributes=0,_tmpDB8)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmpDC0;struct Cyc_Absyn_Aggr_d_struct _tmpDBF;
struct Cyc_List_List*_tmpDBE;Cyc_Toc_result_decls=((_tmpDBE=_cycalloc(sizeof(*
_tmpDBE)),((_tmpDBE->hd=Cyc_Absyn_new_decl((void*)((_tmpDC0=_cycalloc(sizeof(*
_tmpDC0)),((_tmpDC0[0]=((_tmpDBF.tag=6,((_tmpDBF.f1=_tmp90D,_tmpDBF)))),_tmpDC0)))),
0),((_tmpDBE->tl=Cyc_Toc_result_decls,_tmpDBE))))));}_tmp907->type=Cyc_Absyn_strct(
str);}{struct Cyc_List_List*_tmpDC1;new_fields=((_tmpDC1=_cycalloc(sizeof(*
_tmpDC1)),((_tmpDC1->hd=_tmp907,((_tmpDC1->tl=new_fields,_tmpDC1))))));};};}}(
new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_fields);};}if(add_to_decls){struct Cyc_Absyn_Decl*_tmpDCB;struct Cyc_Absyn_Aggr_d_struct
_tmpDCA;struct Cyc_Absyn_Aggr_d_struct*_tmpDC9;struct Cyc_List_List*_tmpDC8;Cyc_Toc_result_decls=((
_tmpDC8=_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8->hd=((_tmpDCB=_cycalloc(sizeof(*
_tmpDCB)),((_tmpDCB->r=(void*)((_tmpDC9=_cycalloc(sizeof(*_tmpDC9)),((_tmpDC9[0]=((
_tmpDCA.tag=6,((_tmpDCA.f1=new_ad,_tmpDCA)))),_tmpDC9)))),((_tmpDCB->loc=0,
_tmpDCB)))))),((_tmpDC8->tl=Cyc_Toc_result_decls,_tmpDC8))))));}};};
_pop_dynregion(d);};}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud){struct _DynRegionHandle*_tmp924;struct Cyc_Set_Set**_tmp925;struct Cyc_Toc_TocState
_tmp923=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp924=
_tmp923.dyn;_tmp925=_tmp923.datatypes_so_far;{struct _DynRegionFrame _tmp926;
struct _RegionHandle*d=_open_dynregion(& _tmp926,_tmp924);{struct _tuple0*_tmp927=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))
Cyc_Set_member)(*_tmp925,_tmp927)){_npop_handler(0);return;}*_tmp925=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*
_tmp925,_tmp927);}{struct Cyc_List_List*_tmp928=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp928 != 0;_tmp928=_tmp928->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp928->hd;struct Cyc_List_List*_tmp929=0;int i=
1;{struct Cyc_List_List*_tmp92A=f->typs;for(0;_tmp92A != 0;(_tmp92A=_tmp92A->tl,i
++)){struct _dyneither_ptr*_tmp92B=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpDCC;struct Cyc_Absyn_Aggrfield*_tmp92C=(_tmpDCC=_cycalloc(sizeof(*_tmpDCC)),((
_tmpDCC->name=_tmp92B,((_tmpDCC->tq=(*((struct _tuple10*)_tmp92A->hd)).f1,((
_tmpDCC->type=Cyc_Toc_typ_to_c_array((*((struct _tuple10*)_tmp92A->hd)).f2),((
_tmpDCC->width=0,((_tmpDCC->attributes=0,_tmpDCC)))))))))));struct Cyc_List_List*
_tmpDCD;_tmp929=((_tmpDCD=_cycalloc(sizeof(*_tmpDCD)),((_tmpDCD->hd=_tmp92C,((
_tmpDCD->tl=_tmp929,_tmpDCD))))));}}{struct Cyc_Absyn_Aggrfield*_tmpDD0;struct Cyc_List_List*
_tmpDCF;_tmp929=((_tmpDCF=_cycalloc(sizeof(*_tmpDCF)),((_tmpDCF->hd=((_tmpDD0=
_cycalloc(sizeof(*_tmpDD0)),((_tmpDD0->name=Cyc_Toc_tag_sp,((_tmpDD0->tq=Cyc_Toc_mt_tq,((
_tmpDD0->type=Cyc_Absyn_sint_typ,((_tmpDD0->width=0,((_tmpDD0->attributes=0,
_tmpDD0)))))))))))),((_tmpDCF->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp929),_tmpDCF))))));}{struct Cyc_Absyn_AggrdeclImpl*
_tmpDD5;const char*_tmpDD4;struct Cyc_Absyn_Aggrdecl*_tmpDD3;struct Cyc_Absyn_Aggrdecl*
_tmp931=(_tmpDD3=_cycalloc(sizeof(*_tmpDD3)),((_tmpDD3->kind=Cyc_Absyn_StructA,((
_tmpDD3->sc=Cyc_Absyn_Public,((_tmpDD3->name=Cyc_Toc_collapse_qvar_tag(f->name,((
_tmpDD4="_struct",_tag_dyneither(_tmpDD4,sizeof(char),8)))),((_tmpDD3->tvs=0,((
_tmpDD3->impl=((_tmpDD5=_cycalloc(sizeof(*_tmpDD5)),((_tmpDD5->exist_vars=0,((
_tmpDD5->rgn_po=0,((_tmpDD5->fields=_tmp929,((_tmpDD5->tagged=0,_tmpDD5)))))))))),((
_tmpDD3->attributes=0,_tmpDD3)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpDDB;
struct Cyc_Absyn_Aggr_d_struct _tmpDDA;struct Cyc_List_List*_tmpDD9;Cyc_Toc_result_decls=((
_tmpDD9=_cycalloc(sizeof(*_tmpDD9)),((_tmpDD9->hd=Cyc_Absyn_new_decl((void*)((
_tmpDDB=_cycalloc(sizeof(*_tmpDDB)),((_tmpDDB[0]=((_tmpDDA.tag=6,((_tmpDDA.f1=
_tmp931,_tmpDDA)))),_tmpDDB)))),0),((_tmpDD9->tl=Cyc_Toc_result_decls,_tmpDD9))))));};}};;
_pop_dynregion(d);};}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd){if(xd->fields == 0)return;{struct _DynRegionHandle*_tmp939;struct Cyc_Dict_Dict*
_tmp93A;struct Cyc_Toc_TocState _tmp938=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp939=_tmp938.dyn;_tmp93A=_tmp938.xdatatypes_so_far;{struct _DynRegionFrame
_tmp93B;struct _RegionHandle*d=_open_dynregion(& _tmp93B,_tmp939);{struct _tuple0*
_tmp93C=xd->name;struct Cyc_List_List*_tmp93D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp93D != 0;_tmp93D=_tmp93D->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp93D->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp93E=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp93F=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp93E,Cyc_Absyn_false_conref,0);int*_tmp940=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp93A,f->name);int
_tmp941;_LL3A7: if(_tmp940 != 0)goto _LL3A9;_LL3A8: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp942=Cyc_Absyn_new_vardecl(f->name,_tmp93F,
initopt);_tmp942->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*_tmpDE1;struct Cyc_Absyn_Var_d_struct
_tmpDE0;struct Cyc_List_List*_tmpDDF;Cyc_Toc_result_decls=((_tmpDDF=_cycalloc(
sizeof(*_tmpDDF)),((_tmpDDF->hd=Cyc_Absyn_new_decl((void*)((_tmpDE1=_cycalloc(
sizeof(*_tmpDE1)),((_tmpDE1[0]=((_tmpDE0.tag=0,((_tmpDE0.f1=_tmp942,_tmpDE0)))),
_tmpDE1)))),0),((_tmpDDF->tl=Cyc_Toc_result_decls,_tmpDDF))))));}*_tmp93A=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp93A,f->name,f->sc != Cyc_Absyn_Extern);{struct Cyc_List_List*fields=0;int i=1;{
struct Cyc_List_List*_tmp946=f->typs;for(0;_tmp946 != 0;(_tmp946=_tmp946->tl,i ++)){
struct Cyc_Int_pa_struct _tmpDE9;void*_tmpDE8[1];const char*_tmpDE7;struct
_dyneither_ptr*_tmpDE6;struct _dyneither_ptr*_tmp947=(_tmpDE6=_cycalloc(sizeof(*
_tmpDE6)),((_tmpDE6[0]=(struct _dyneither_ptr)((_tmpDE9.tag=1,((_tmpDE9.f1=(
unsigned long)i,((_tmpDE8[0]=& _tmpDE9,Cyc_aprintf(((_tmpDE7="f%d",_tag_dyneither(
_tmpDE7,sizeof(char),4))),_tag_dyneither(_tmpDE8,sizeof(void*),1)))))))),_tmpDE6)));
struct Cyc_Absyn_Aggrfield*_tmpDEA;struct Cyc_Absyn_Aggrfield*_tmp948=(_tmpDEA=
_cycalloc(sizeof(*_tmpDEA)),((_tmpDEA->name=_tmp947,((_tmpDEA->tq=(*((struct
_tuple10*)_tmp946->hd)).f1,((_tmpDEA->type=Cyc_Toc_typ_to_c_array((*((struct
_tuple10*)_tmp946->hd)).f2),((_tmpDEA->width=0,((_tmpDEA->attributes=0,_tmpDEA)))))))))));
struct Cyc_List_List*_tmpDEB;fields=((_tmpDEB=_cycalloc(sizeof(*_tmpDEB)),((
_tmpDEB->hd=_tmp948,((_tmpDEB->tl=fields,_tmpDEB))))));}}{struct Cyc_Absyn_Aggrfield*
_tmpDEE;struct Cyc_List_List*_tmpDED;fields=((_tmpDED=_cycalloc(sizeof(*_tmpDED)),((
_tmpDED->hd=((_tmpDEE=_cycalloc(sizeof(*_tmpDEE)),((_tmpDEE->name=Cyc_Toc_tag_sp,((
_tmpDEE->tq=Cyc_Toc_mt_tq,((_tmpDEE->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq),((_tmpDEE->width=0,((_tmpDEE->attributes=0,_tmpDEE)))))))))))),((
_tmpDED->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmpDED))))));}{struct Cyc_Absyn_AggrdeclImpl*_tmpDF3;const char*_tmpDF2;
struct Cyc_Absyn_Aggrdecl*_tmpDF1;struct Cyc_Absyn_Aggrdecl*_tmp951=(_tmpDF1=
_cycalloc(sizeof(*_tmpDF1)),((_tmpDF1->kind=Cyc_Absyn_StructA,((_tmpDF1->sc=Cyc_Absyn_Public,((
_tmpDF1->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpDF2="_struct",
_tag_dyneither(_tmpDF2,sizeof(char),8)))),((_tmpDF1->tvs=0,((_tmpDF1->impl=((
_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((_tmpDF3->exist_vars=0,((_tmpDF3->rgn_po=0,((
_tmpDF3->fields=fields,((_tmpDF3->tagged=0,_tmpDF3)))))))))),((_tmpDF1->attributes=
0,_tmpDF1)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpDF9;struct Cyc_Absyn_Aggr_d_struct
_tmpDF8;struct Cyc_List_List*_tmpDF7;Cyc_Toc_result_decls=((_tmpDF7=_cycalloc(
sizeof(*_tmpDF7)),((_tmpDF7->hd=Cyc_Absyn_new_decl((void*)((_tmpDF9=_cycalloc(
sizeof(*_tmpDF9)),((_tmpDF9[0]=((_tmpDF8.tag=6,((_tmpDF8.f1=_tmp951,_tmpDF8)))),
_tmpDF9)))),0),((_tmpDF7->tl=Cyc_Toc_result_decls,_tmpDF7))))));}goto _LL3A6;};};};}
_LL3A9: if(_tmp940 == 0)goto _LL3AB;_tmp941=*_tmp940;if(_tmp941 != 0)goto _LL3AB;
_LL3AA: if(f->sc != Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*_tmp958=Cyc_Absyn_string_exp(*
fn,0);struct Cyc_Absyn_Vardecl*_tmp959=Cyc_Absyn_new_vardecl(f->name,_tmp93F,(
struct Cyc_Absyn_Exp*)_tmp958);_tmp959->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*
_tmpDFF;struct Cyc_Absyn_Var_d_struct _tmpDFE;struct Cyc_List_List*_tmpDFD;Cyc_Toc_result_decls=((
_tmpDFD=_cycalloc(sizeof(*_tmpDFD)),((_tmpDFD->hd=Cyc_Absyn_new_decl((void*)((
_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((_tmpDFF[0]=((_tmpDFE.tag=0,((_tmpDFE.f1=
_tmp959,_tmpDFE)))),_tmpDFF)))),0),((_tmpDFD->tl=Cyc_Toc_result_decls,_tmpDFD))))));}*
_tmp93A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp93A,f->name,1);}goto _LL3A6;_LL3AB:;_LL3AC: goto _LL3A6;_LL3A6:;}};
_pop_dynregion(d);};};}static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*
ed){ed->sc=Cyc_Absyn_Public;if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(
struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,
struct Cyc_Absyn_Stmt*s){void*old_typ=vd->type;vd->type=Cyc_Toc_typ_to_c_array(
old_typ);if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ))vd->sc=Cyc_Absyn_Public;Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp95D=init->r;struct Cyc_Absyn_Vardecl*_tmp95F;struct Cyc_Absyn_Exp*_tmp960;
struct Cyc_Absyn_Exp*_tmp961;int _tmp962;_LL3AE: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp95E=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp95D;if(_tmp95E->tag != 26)
goto _LL3B0;else{_tmp95F=_tmp95E->f1;_tmp960=_tmp95E->f2;_tmp961=_tmp95E->f3;
_tmp962=_tmp95E->f4;}}_LL3AF: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp95F,_tmp960,_tmp961,_tmp962,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL3AD;_LL3B0:;_LL3B1: if(vd->sc == Cyc_Absyn_Static){struct
_RegionHandle _tmp963=_new_region("temp");struct _RegionHandle*temp=& _tmp963;
_push_region(temp);{struct Cyc_Toc_Env*_tmp964=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp964,init);};_pop_region(temp);}else{Cyc_Toc_exp_to_c(
init_nv,init);}goto _LL3AD;_LL3AD:;}else{void*_tmp965=Cyc_Tcutil_compress(old_typ);
struct Cyc_Absyn_ArrayInfo _tmp967;void*_tmp968;struct Cyc_Absyn_Exp*_tmp969;union
Cyc_Absyn_Constraint*_tmp96A;_LL3B3:{struct Cyc_Absyn_ArrayType_struct*_tmp966=(
struct Cyc_Absyn_ArrayType_struct*)_tmp965;if(_tmp966->tag != 8)goto _LL3B5;else{
_tmp967=_tmp966->f1;_tmp968=_tmp967.elt_type;_tmp969=_tmp967.num_elts;_tmp96A=
_tmp967.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp96A))goto _LL3B5;_LL3B4: if(_tmp969 == 0){const char*_tmpE02;void*_tmpE01;(
_tmpE01=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE02="can't initialize zero-terminated array -- size unknown",_tag_dyneither(
_tmpE02,sizeof(char),55))),_tag_dyneither(_tmpE01,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
num_elts=(struct Cyc_Absyn_Exp*)_tmp969;struct Cyc_Absyn_Exp*_tmp96D=Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(
- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp96E=Cyc_Absyn_signed_int_exp(0,0);s->r=Cyc_Toc_seq_stmt_r(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp96D,_tmp96E,0),0),Cyc_Absyn_new_stmt(
s->r,0));goto _LL3B2;};_LL3B5:;_LL3B6: goto _LL3B2;_LL3B2:;}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{struct _tuple0*x=Cyc_Toc_temp_var();
struct _RegionHandle _tmp96F=_new_region("prgn");struct _RegionHandle*prgn=& _tmp96F;
_push_region(prgn);{struct Cyc_Absyn_Stmt*_tmp970=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp971=Cyc_Toc_share_env(prgn,nv);struct Cyc_Toc_Env*_tmp973;
struct Cyc_List_List*_tmp974;struct Cyc_Absyn_Stmt*_tmp975;struct _tuple23 _tmp972=
Cyc_Toc_xlate_pat(_tmp971,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
Cyc_Toc_throw_match_stmt(),0);_tmp973=_tmp972.f1;_tmp974=_tmp972.f2;_tmp975=
_tmp972.f3;Cyc_Toc_stmt_to_c(_tmp973,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(
t),(struct Cyc_Absyn_Exp*)e,Cyc_Absyn_seq_stmt(_tmp975,s,0),0);for(0;_tmp974 != 0;
_tmp974=_tmp974->tl){struct _tuple24 _tmp977;struct _tuple0*_tmp978;void*_tmp979;
struct _tuple24*_tmp976=(struct _tuple24*)_tmp974->hd;_tmp977=*_tmp976;_tmp978=
_tmp977.f1;_tmp979=_tmp977.f2;s=Cyc_Absyn_declare_stmt(_tmp978,_tmp979,0,s,0);}}{
struct Cyc_Absyn_Stmt*_tmp97A=s;_npop_handler(0);return _tmp97A;};;_pop_region(
prgn);};}static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){void*_tmp97B=e->r;
struct Cyc_Absyn_Exp*_tmp97D;struct Cyc_Absyn_Exp*_tmp97F;struct Cyc_Absyn_Exp*
_tmp981;struct Cyc_Absyn_Exp*_tmp983;struct Cyc_Absyn_Exp*_tmp985;struct Cyc_Absyn_Exp*
_tmp987;struct Cyc_Absyn_Exp*_tmp989;struct Cyc_Absyn_Exp*_tmp98B;struct Cyc_List_List*
_tmp98D;struct Cyc_Absyn_Exp*_tmp98F;struct Cyc_Absyn_Exp*_tmp990;struct Cyc_Absyn_Exp*
_tmp992;struct Cyc_Absyn_Exp*_tmp993;struct Cyc_Absyn_Exp*_tmp995;struct Cyc_Absyn_Exp*
_tmp996;struct Cyc_Absyn_Exp*_tmp998;struct Cyc_Absyn_Exp*_tmp999;struct Cyc_Absyn_Exp*
_tmp99B;struct Cyc_Absyn_Exp*_tmp99C;struct Cyc_Absyn_Exp*_tmp99E;struct Cyc_Absyn_Exp*
_tmp99F;struct Cyc_Absyn_Exp*_tmp9A1;struct Cyc_Absyn_Exp*_tmp9A2;struct Cyc_Absyn_Exp*
_tmp9A3;struct Cyc_Absyn_Exp*_tmp9A5;struct Cyc_List_List*_tmp9A6;void*_tmp9A8;
void**_tmp9A9;struct Cyc_Absyn_Exp*_tmp9AA;struct _tuple8*_tmp9AC;struct _tuple8
_tmp9AD;void*_tmp9AE;void**_tmp9AF;struct Cyc_List_List*_tmp9B0;struct Cyc_List_List*
_tmp9B2;struct Cyc_List_List*_tmp9B4;void*_tmp9B6;void**_tmp9B7;void*_tmp9B9;void**
_tmp9BA;struct Cyc_Absyn_Stmt*_tmp9BC;struct Cyc_Absyn_MallocInfo _tmp9BE;struct Cyc_Absyn_MallocInfo*
_tmp9BF;_LL3B8: {struct Cyc_Absyn_Deref_e_struct*_tmp97C=(struct Cyc_Absyn_Deref_e_struct*)
_tmp97B;if(_tmp97C->tag != 19)goto _LL3BA;else{_tmp97D=_tmp97C->f1;}}_LL3B9:
_tmp97F=_tmp97D;goto _LL3BB;_LL3BA: {struct Cyc_Absyn_AggrMember_e_struct*_tmp97E=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp97B;if(_tmp97E->tag != 20)goto _LL3BC;
else{_tmp97F=_tmp97E->f1;}}_LL3BB: _tmp981=_tmp97F;goto _LL3BD;_LL3BC: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp980=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp97B;if(_tmp980->tag != 21)goto
_LL3BE;else{_tmp981=_tmp980->f1;}}_LL3BD: _tmp983=_tmp981;goto _LL3BF;_LL3BE: {
struct Cyc_Absyn_Address_e_struct*_tmp982=(struct Cyc_Absyn_Address_e_struct*)
_tmp97B;if(_tmp982->tag != 14)goto _LL3C0;else{_tmp983=_tmp982->f1;}}_LL3BF:
_tmp985=_tmp983;goto _LL3C1;_LL3C0: {struct Cyc_Absyn_Throw_e_struct*_tmp984=(
struct Cyc_Absyn_Throw_e_struct*)_tmp97B;if(_tmp984->tag != 10)goto _LL3C2;else{
_tmp985=_tmp984->f1;}}_LL3C1: _tmp987=_tmp985;goto _LL3C3;_LL3C2: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp986=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp97B;if(_tmp986->tag != 11)
goto _LL3C4;else{_tmp987=_tmp986->f1;}}_LL3C3: _tmp989=_tmp987;goto _LL3C5;_LL3C4: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp988=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp97B;if(_tmp988->tag != 17)goto _LL3C6;else{_tmp989=_tmp988->f1;}}_LL3C5:
_tmp98B=_tmp989;goto _LL3C7;_LL3C6: {struct Cyc_Absyn_Increment_e_struct*_tmp98A=(
struct Cyc_Absyn_Increment_e_struct*)_tmp97B;if(_tmp98A->tag != 4)goto _LL3C8;else{
_tmp98B=_tmp98A->f1;}}_LL3C7: Cyc_Toc_exptypes_to_c(_tmp98B);goto _LL3B7;_LL3C8: {
struct Cyc_Absyn_Primop_e_struct*_tmp98C=(struct Cyc_Absyn_Primop_e_struct*)
_tmp97B;if(_tmp98C->tag != 2)goto _LL3CA;else{_tmp98D=_tmp98C->f2;}}_LL3C9:((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,
_tmp98D);goto _LL3B7;_LL3CA: {struct Cyc_Absyn_And_e_struct*_tmp98E=(struct Cyc_Absyn_And_e_struct*)
_tmp97B;if(_tmp98E->tag != 6)goto _LL3CC;else{_tmp98F=_tmp98E->f1;_tmp990=_tmp98E->f2;}}
_LL3CB: _tmp992=_tmp98F;_tmp993=_tmp990;goto _LL3CD;_LL3CC: {struct Cyc_Absyn_Or_e_struct*
_tmp991=(struct Cyc_Absyn_Or_e_struct*)_tmp97B;if(_tmp991->tag != 7)goto _LL3CE;
else{_tmp992=_tmp991->f1;_tmp993=_tmp991->f2;}}_LL3CD: _tmp995=_tmp992;_tmp996=
_tmp993;goto _LL3CF;_LL3CE: {struct Cyc_Absyn_SeqExp_e_struct*_tmp994=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp97B;if(_tmp994->tag != 8)goto _LL3D0;else{_tmp995=_tmp994->f1;_tmp996=_tmp994->f2;}}
_LL3CF: _tmp998=_tmp995;_tmp999=_tmp996;goto _LL3D1;_LL3D0: {struct Cyc_Absyn_Subscript_e_struct*
_tmp997=(struct Cyc_Absyn_Subscript_e_struct*)_tmp97B;if(_tmp997->tag != 22)goto
_LL3D2;else{_tmp998=_tmp997->f1;_tmp999=_tmp997->f2;}}_LL3D1: _tmp99B=_tmp998;
_tmp99C=_tmp999;goto _LL3D3;_LL3D2: {struct Cyc_Absyn_Swap_e_struct*_tmp99A=(
struct Cyc_Absyn_Swap_e_struct*)_tmp97B;if(_tmp99A->tag != 33)goto _LL3D4;else{
_tmp99B=_tmp99A->f1;_tmp99C=_tmp99A->f2;}}_LL3D3: _tmp99E=_tmp99B;_tmp99F=_tmp99C;
goto _LL3D5;_LL3D4: {struct Cyc_Absyn_AssignOp_e_struct*_tmp99D=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp97B;if(_tmp99D->tag != 3)goto _LL3D6;else{_tmp99E=_tmp99D->f1;_tmp99F=_tmp99D->f3;}}
_LL3D5: Cyc_Toc_exptypes_to_c(_tmp99E);Cyc_Toc_exptypes_to_c(_tmp99F);goto _LL3B7;
_LL3D6: {struct Cyc_Absyn_Conditional_e_struct*_tmp9A0=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp97B;if(_tmp9A0->tag != 5)goto _LL3D8;else{_tmp9A1=_tmp9A0->f1;_tmp9A2=_tmp9A0->f2;
_tmp9A3=_tmp9A0->f3;}}_LL3D7: Cyc_Toc_exptypes_to_c(_tmp9A1);Cyc_Toc_exptypes_to_c(
_tmp9A2);Cyc_Toc_exptypes_to_c(_tmp9A3);goto _LL3B7;_LL3D8: {struct Cyc_Absyn_FnCall_e_struct*
_tmp9A4=(struct Cyc_Absyn_FnCall_e_struct*)_tmp97B;if(_tmp9A4->tag != 9)goto _LL3DA;
else{_tmp9A5=_tmp9A4->f1;_tmp9A6=_tmp9A4->f2;}}_LL3D9: Cyc_Toc_exptypes_to_c(
_tmp9A5);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp9A6);goto _LL3B7;_LL3DA: {struct Cyc_Absyn_Cast_e_struct*
_tmp9A7=(struct Cyc_Absyn_Cast_e_struct*)_tmp97B;if(_tmp9A7->tag != 13)goto _LL3DC;
else{_tmp9A8=(void**)& _tmp9A7->f1;_tmp9A9=(void**)((void**)& _tmp9A7->f1);_tmp9AA=
_tmp9A7->f2;}}_LL3DB:*_tmp9A9=Cyc_Toc_typ_to_c(*_tmp9A9);Cyc_Toc_exptypes_to_c(
_tmp9AA);goto _LL3B7;_LL3DC: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp9AB=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp97B;if(_tmp9AB->tag != 24)goto _LL3DE;
else{_tmp9AC=_tmp9AB->f1;_tmp9AD=*_tmp9AC;_tmp9AE=_tmp9AD.f3;_tmp9AF=(void**)&(*
_tmp9AB->f1).f3;_tmp9B0=_tmp9AB->f2;}}_LL3DD:*_tmp9AF=Cyc_Toc_typ_to_c(*_tmp9AF);
_tmp9B2=_tmp9B0;goto _LL3DF;_LL3DE: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp9B1=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp97B;if(_tmp9B1->tag != 34)
goto _LL3E0;else{_tmp9B2=_tmp9B1->f2;}}_LL3DF: _tmp9B4=_tmp9B2;goto _LL3E1;_LL3E0: {
struct Cyc_Absyn_Array_e_struct*_tmp9B3=(struct Cyc_Absyn_Array_e_struct*)_tmp97B;
if(_tmp9B3->tag != 25)goto _LL3E2;else{_tmp9B4=_tmp9B3->f1;}}_LL3E1: for(0;_tmp9B4
!= 0;_tmp9B4=_tmp9B4->tl){struct Cyc_Absyn_Exp*_tmp9CF;struct _tuple16 _tmp9CE=*((
struct _tuple16*)_tmp9B4->hd);_tmp9CF=_tmp9CE.f2;Cyc_Toc_exptypes_to_c(_tmp9CF);}
goto _LL3B7;_LL3E2: {struct Cyc_Absyn_Offsetof_e_struct*_tmp9B5=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp97B;if(_tmp9B5->tag != 18)goto _LL3E4;else{_tmp9B6=(void**)& _tmp9B5->f1;
_tmp9B7=(void**)((void**)& _tmp9B5->f1);}}_LL3E3: _tmp9BA=_tmp9B7;goto _LL3E5;
_LL3E4: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp9B8=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp97B;if(_tmp9B8->tag != 16)goto _LL3E6;else{_tmp9B9=(void**)& _tmp9B8->f1;
_tmp9BA=(void**)((void**)& _tmp9B8->f1);}}_LL3E5:*_tmp9BA=Cyc_Toc_typ_to_c(*
_tmp9BA);goto _LL3B7;_LL3E6: {struct Cyc_Absyn_StmtExp_e_struct*_tmp9BB=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp97B;if(_tmp9BB->tag != 35)goto _LL3E8;else{_tmp9BC=_tmp9BB->f1;}}_LL3E7: Cyc_Toc_stmttypes_to_c(
_tmp9BC);goto _LL3B7;_LL3E8: {struct Cyc_Absyn_Malloc_e_struct*_tmp9BD=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp97B;if(_tmp9BD->tag != 32)goto _LL3EA;else{_tmp9BE=_tmp9BD->f1;_tmp9BF=(struct
Cyc_Absyn_MallocInfo*)& _tmp9BD->f1;}}_LL3E9: if(_tmp9BF->elt_type != 0){void**
_tmpE03;_tmp9BF->elt_type=((_tmpE03=_cycalloc(sizeof(*_tmpE03)),((_tmpE03[0]=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp9BF->elt_type))),_tmpE03))));}Cyc_Toc_exptypes_to_c(
_tmp9BF->num_elts);goto _LL3B7;_LL3EA: {struct Cyc_Absyn_Const_e_struct*_tmp9C0=(
struct Cyc_Absyn_Const_e_struct*)_tmp97B;if(_tmp9C0->tag != 0)goto _LL3EC;}_LL3EB:
goto _LL3ED;_LL3EC: {struct Cyc_Absyn_Var_e_struct*_tmp9C1=(struct Cyc_Absyn_Var_e_struct*)
_tmp97B;if(_tmp9C1->tag != 1)goto _LL3EE;}_LL3ED: goto _LL3EF;_LL3EE: {struct Cyc_Absyn_Enum_e_struct*
_tmp9C2=(struct Cyc_Absyn_Enum_e_struct*)_tmp97B;if(_tmp9C2->tag != 30)goto _LL3F0;}
_LL3EF: goto _LL3F1;_LL3F0: {struct Cyc_Absyn_Asm_e_struct*_tmp9C3=(struct Cyc_Absyn_Asm_e_struct*)
_tmp97B;if(_tmp9C3->tag != 38)goto _LL3F2;}_LL3F1: goto _LL3F3;_LL3F2: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp9C4=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp97B;if(_tmp9C4->tag != 31)goto
_LL3F4;}_LL3F3: goto _LL3B7;_LL3F4: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp9C5=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp97B;if(_tmp9C5->tag != 28)goto _LL3F6;}
_LL3F5: goto _LL3F7;_LL3F6: {struct Cyc_Absyn_Datatype_e_struct*_tmp9C6=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp97B;if(_tmp9C6->tag != 29)goto _LL3F8;}_LL3F7: goto _LL3F9;_LL3F8: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp9C7=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp97B;if(_tmp9C7->tag != 27)goto
_LL3FA;}_LL3F9: goto _LL3FB;_LL3FA: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp9C8=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp97B;if(_tmp9C8->tag != 26)
goto _LL3FC;}_LL3FB: goto _LL3FD;_LL3FC: {struct Cyc_Absyn_Tuple_e_struct*_tmp9C9=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp97B;if(_tmp9C9->tag != 23)goto _LL3FE;}_LL3FD:
goto _LL3FF;_LL3FE: {struct Cyc_Absyn_Instantiate_e_struct*_tmp9CA=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp97B;if(_tmp9CA->tag != 12)goto _LL400;}_LL3FF: goto _LL401;_LL400: {struct Cyc_Absyn_New_e_struct*
_tmp9CB=(struct Cyc_Absyn_New_e_struct*)_tmp97B;if(_tmp9CB->tag != 15)goto _LL402;}
_LL401: goto _LL403;_LL402: {struct Cyc_Absyn_Valueof_e_struct*_tmp9CC=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp97B;if(_tmp9CC->tag != 37)goto _LL404;}_LL403: goto _LL405;_LL404: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp9CD=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp97B;if(_tmp9CD->tag != 36)goto
_LL3B7;}_LL405:{const char*_tmpE06;void*_tmpE05;(_tmpE05=0,Cyc_Tcutil_terr(e->loc,((
_tmpE06="Cyclone expression within C code",_tag_dyneither(_tmpE06,sizeof(char),
33))),_tag_dyneither(_tmpE05,sizeof(void*),0)));}goto _LL3B7;_LL3B7:;}static void
Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){void*_tmp9D3=d->r;struct Cyc_Absyn_Vardecl*
_tmp9D5;struct Cyc_Absyn_Fndecl*_tmp9D7;struct Cyc_Absyn_Aggrdecl*_tmp9D9;struct
Cyc_Absyn_Enumdecl*_tmp9DB;struct Cyc_Absyn_Typedefdecl*_tmp9DD;_LL407: {struct
Cyc_Absyn_Var_d_struct*_tmp9D4=(struct Cyc_Absyn_Var_d_struct*)_tmp9D3;if(_tmp9D4->tag
!= 0)goto _LL409;else{_tmp9D5=_tmp9D4->f1;}}_LL408: _tmp9D5->type=Cyc_Toc_typ_to_c(
_tmp9D5->type);if(_tmp9D5->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp9D5->initializer));goto _LL406;_LL409: {struct Cyc_Absyn_Fn_d_struct*
_tmp9D6=(struct Cyc_Absyn_Fn_d_struct*)_tmp9D3;if(_tmp9D6->tag != 1)goto _LL40B;
else{_tmp9D7=_tmp9D6->f1;}}_LL40A: _tmp9D7->ret_type=Cyc_Toc_typ_to_c(_tmp9D7->ret_type);{
struct Cyc_List_List*_tmp9E9=_tmp9D7->args;for(0;_tmp9E9 != 0;_tmp9E9=_tmp9E9->tl){(*((
struct _tuple8*)_tmp9E9->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmp9E9->hd)).f3);}}
goto _LL406;_LL40B: {struct Cyc_Absyn_Aggr_d_struct*_tmp9D8=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp9D3;if(_tmp9D8->tag != 6)goto _LL40D;else{_tmp9D9=_tmp9D8->f1;}}_LL40C: Cyc_Toc_aggrdecl_to_c(
_tmp9D9,1);goto _LL406;_LL40D: {struct Cyc_Absyn_Enum_d_struct*_tmp9DA=(struct Cyc_Absyn_Enum_d_struct*)
_tmp9D3;if(_tmp9DA->tag != 8)goto _LL40F;else{_tmp9DB=_tmp9DA->f1;}}_LL40E: if(
_tmp9DB->fields != 0){struct Cyc_List_List*_tmp9EA=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp9DB->fields))->v;for(0;_tmp9EA != 0;_tmp9EA=_tmp9EA->tl){
struct Cyc_Absyn_Enumfield*_tmp9EB=(struct Cyc_Absyn_Enumfield*)_tmp9EA->hd;if(
_tmp9EB->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9EB->tag));}}
goto _LL406;_LL40F: {struct Cyc_Absyn_Typedef_d_struct*_tmp9DC=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp9D3;if(_tmp9DC->tag != 9)goto _LL411;else{_tmp9DD=_tmp9DC->f1;}}_LL410: _tmp9DD->defn=(
void*)Cyc_Toc_typ_to_c_array((void*)_check_null(_tmp9DD->defn));goto _LL406;
_LL411: {struct Cyc_Absyn_Let_d_struct*_tmp9DE=(struct Cyc_Absyn_Let_d_struct*)
_tmp9D3;if(_tmp9DE->tag != 2)goto _LL413;}_LL412: goto _LL414;_LL413: {struct Cyc_Absyn_Letv_d_struct*
_tmp9DF=(struct Cyc_Absyn_Letv_d_struct*)_tmp9D3;if(_tmp9DF->tag != 3)goto _LL415;}
_LL414: goto _LL416;_LL415: {struct Cyc_Absyn_Datatype_d_struct*_tmp9E0=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp9D3;if(_tmp9E0->tag != 7)goto _LL417;}_LL416: goto _LL418;_LL417: {struct Cyc_Absyn_Namespace_d_struct*
_tmp9E1=(struct Cyc_Absyn_Namespace_d_struct*)_tmp9D3;if(_tmp9E1->tag != 10)goto
_LL419;}_LL418: goto _LL41A;_LL419: {struct Cyc_Absyn_Using_d_struct*_tmp9E2=(
struct Cyc_Absyn_Using_d_struct*)_tmp9D3;if(_tmp9E2->tag != 11)goto _LL41B;}_LL41A:
goto _LL41C;_LL41B: {struct Cyc_Absyn_ExternC_d_struct*_tmp9E3=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp9D3;if(_tmp9E3->tag != 12)goto _LL41D;}_LL41C: goto _LL41E;_LL41D: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp9E4=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9D3;if(_tmp9E4->tag != 13)
goto _LL41F;}_LL41E: goto _LL420;_LL41F: {struct Cyc_Absyn_Region_d_struct*_tmp9E5=(
struct Cyc_Absyn_Region_d_struct*)_tmp9D3;if(_tmp9E5->tag != 4)goto _LL421;}_LL420:
goto _LL422;_LL421: {struct Cyc_Absyn_Alias_d_struct*_tmp9E6=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9D3;if(_tmp9E6->tag != 5)goto _LL423;}_LL422:{const char*_tmpE09;void*_tmpE08;(
_tmpE08=0,Cyc_Tcutil_terr(d->loc,((_tmpE09="Cyclone declaration within C code",
_tag_dyneither(_tmpE09,sizeof(char),34))),_tag_dyneither(_tmpE08,sizeof(void*),0)));}
goto _LL406;_LL423: {struct Cyc_Absyn_Porton_d_struct*_tmp9E7=(struct Cyc_Absyn_Porton_d_struct*)
_tmp9D3;if(_tmp9E7->tag != 14)goto _LL425;}_LL424: goto _LL426;_LL425: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9E8=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9D3;if(_tmp9E8->tag != 15)goto
_LL406;}_LL426: goto _LL406;_LL406:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s){void*_tmp9EE=s->r;struct Cyc_Absyn_Exp*_tmp9F0;struct Cyc_Absyn_Stmt*_tmp9F2;
struct Cyc_Absyn_Stmt*_tmp9F3;struct Cyc_Absyn_Exp*_tmp9F5;struct Cyc_Absyn_Exp*
_tmp9F7;struct Cyc_Absyn_Stmt*_tmp9F8;struct Cyc_Absyn_Stmt*_tmp9F9;struct _tuple9
_tmp9FB;struct Cyc_Absyn_Exp*_tmp9FC;struct Cyc_Absyn_Stmt*_tmp9FD;struct Cyc_Absyn_Exp*
_tmp9FF;struct _tuple9 _tmpA00;struct Cyc_Absyn_Exp*_tmpA01;struct _tuple9 _tmpA02;
struct Cyc_Absyn_Exp*_tmpA03;struct Cyc_Absyn_Stmt*_tmpA04;struct Cyc_Absyn_Exp*
_tmpA06;struct Cyc_List_List*_tmpA07;struct Cyc_Absyn_Decl*_tmpA09;struct Cyc_Absyn_Stmt*
_tmpA0A;struct Cyc_Absyn_Stmt*_tmpA0C;struct _tuple9 _tmpA0D;struct Cyc_Absyn_Exp*
_tmpA0E;_LL428: {struct Cyc_Absyn_Exp_s_struct*_tmp9EF=(struct Cyc_Absyn_Exp_s_struct*)
_tmp9EE;if(_tmp9EF->tag != 1)goto _LL42A;else{_tmp9F0=_tmp9EF->f1;}}_LL429: Cyc_Toc_exptypes_to_c(
_tmp9F0);goto _LL427;_LL42A: {struct Cyc_Absyn_Seq_s_struct*_tmp9F1=(struct Cyc_Absyn_Seq_s_struct*)
_tmp9EE;if(_tmp9F1->tag != 2)goto _LL42C;else{_tmp9F2=_tmp9F1->f1;_tmp9F3=_tmp9F1->f2;}}
_LL42B: Cyc_Toc_stmttypes_to_c(_tmp9F2);Cyc_Toc_stmttypes_to_c(_tmp9F3);goto
_LL427;_LL42C: {struct Cyc_Absyn_Return_s_struct*_tmp9F4=(struct Cyc_Absyn_Return_s_struct*)
_tmp9EE;if(_tmp9F4->tag != 3)goto _LL42E;else{_tmp9F5=_tmp9F4->f1;}}_LL42D: if(
_tmp9F5 != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmp9F5);goto _LL427;
_LL42E: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp9F6=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp9EE;if(_tmp9F6->tag != 4)goto _LL430;else{_tmp9F7=_tmp9F6->f1;_tmp9F8=_tmp9F6->f2;
_tmp9F9=_tmp9F6->f3;}}_LL42F: Cyc_Toc_exptypes_to_c(_tmp9F7);Cyc_Toc_stmttypes_to_c(
_tmp9F8);Cyc_Toc_stmttypes_to_c(_tmp9F9);goto _LL427;_LL430: {struct Cyc_Absyn_While_s_struct*
_tmp9FA=(struct Cyc_Absyn_While_s_struct*)_tmp9EE;if(_tmp9FA->tag != 5)goto _LL432;
else{_tmp9FB=_tmp9FA->f1;_tmp9FC=_tmp9FB.f1;_tmp9FD=_tmp9FA->f2;}}_LL431: Cyc_Toc_exptypes_to_c(
_tmp9FC);Cyc_Toc_stmttypes_to_c(_tmp9FD);goto _LL427;_LL432: {struct Cyc_Absyn_For_s_struct*
_tmp9FE=(struct Cyc_Absyn_For_s_struct*)_tmp9EE;if(_tmp9FE->tag != 9)goto _LL434;
else{_tmp9FF=_tmp9FE->f1;_tmpA00=_tmp9FE->f2;_tmpA01=_tmpA00.f1;_tmpA02=_tmp9FE->f3;
_tmpA03=_tmpA02.f1;_tmpA04=_tmp9FE->f4;}}_LL433: Cyc_Toc_exptypes_to_c(_tmp9FF);
Cyc_Toc_exptypes_to_c(_tmpA01);Cyc_Toc_exptypes_to_c(_tmpA03);Cyc_Toc_stmttypes_to_c(
_tmpA04);goto _LL427;_LL434: {struct Cyc_Absyn_Switch_s_struct*_tmpA05=(struct Cyc_Absyn_Switch_s_struct*)
_tmp9EE;if(_tmpA05->tag != 10)goto _LL436;else{_tmpA06=_tmpA05->f1;_tmpA07=_tmpA05->f2;}}
_LL435: Cyc_Toc_exptypes_to_c(_tmpA06);for(0;_tmpA07 != 0;_tmpA07=_tmpA07->tl){Cyc_Toc_stmttypes_to_c(((
struct Cyc_Absyn_Switch_clause*)_tmpA07->hd)->body);}goto _LL427;_LL436: {struct
Cyc_Absyn_Decl_s_struct*_tmpA08=(struct Cyc_Absyn_Decl_s_struct*)_tmp9EE;if(
_tmpA08->tag != 12)goto _LL438;else{_tmpA09=_tmpA08->f1;_tmpA0A=_tmpA08->f2;}}
_LL437: Cyc_Toc_decltypes_to_c(_tmpA09);Cyc_Toc_stmttypes_to_c(_tmpA0A);goto
_LL427;_LL438: {struct Cyc_Absyn_Do_s_struct*_tmpA0B=(struct Cyc_Absyn_Do_s_struct*)
_tmp9EE;if(_tmpA0B->tag != 14)goto _LL43A;else{_tmpA0C=_tmpA0B->f1;_tmpA0D=_tmpA0B->f2;
_tmpA0E=_tmpA0D.f1;}}_LL439: Cyc_Toc_stmttypes_to_c(_tmpA0C);Cyc_Toc_exptypes_to_c(
_tmpA0E);goto _LL427;_LL43A: {struct Cyc_Absyn_Skip_s_struct*_tmpA0F=(struct Cyc_Absyn_Skip_s_struct*)
_tmp9EE;if(_tmpA0F->tag != 0)goto _LL43C;}_LL43B: goto _LL43D;_LL43C: {struct Cyc_Absyn_Break_s_struct*
_tmpA10=(struct Cyc_Absyn_Break_s_struct*)_tmp9EE;if(_tmpA10->tag != 6)goto _LL43E;}
_LL43D: goto _LL43F;_LL43E: {struct Cyc_Absyn_Continue_s_struct*_tmpA11=(struct Cyc_Absyn_Continue_s_struct*)
_tmp9EE;if(_tmpA11->tag != 7)goto _LL440;}_LL43F: goto _LL441;_LL440: {struct Cyc_Absyn_Goto_s_struct*
_tmpA12=(struct Cyc_Absyn_Goto_s_struct*)_tmp9EE;if(_tmpA12->tag != 8)goto _LL442;}
_LL441: goto _LL427;_LL442: {struct Cyc_Absyn_Fallthru_s_struct*_tmpA13=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp9EE;if(_tmpA13->tag != 11)goto _LL444;}_LL443: goto _LL445;_LL444: {struct Cyc_Absyn_Label_s_struct*
_tmpA14=(struct Cyc_Absyn_Label_s_struct*)_tmp9EE;if(_tmpA14->tag != 13)goto _LL446;}
_LL445: goto _LL447;_LL446: {struct Cyc_Absyn_TryCatch_s_struct*_tmpA15=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp9EE;if(_tmpA15->tag != 15)goto _LL448;}_LL447: goto _LL449;_LL448: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmpA16=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp9EE;if(_tmpA16->tag != 16)goto
_LL427;}_LL449:{const char*_tmpE0C;void*_tmpE0B;(_tmpE0B=0,Cyc_Tcutil_terr(s->loc,((
_tmpE0C="Cyclone statement in C code",_tag_dyneither(_tmpE0C,sizeof(char),28))),
_tag_dyneither(_tmpE0B,sizeof(void*),0)));}goto _LL427;_LL427:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude){for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpE0F;void*_tmpE0E;(_tmpE0E=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE0F="decls_to_c: not at toplevel!",
_tag_dyneither(_tmpE0F,sizeof(char),29))),_tag_dyneither(_tmpE0E,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmpA1B=d->r;struct Cyc_Absyn_Vardecl*
_tmpA1D;struct Cyc_Absyn_Fndecl*_tmpA1F;struct Cyc_Absyn_Aggrdecl*_tmpA25;struct
Cyc_Absyn_Datatypedecl*_tmpA27;struct Cyc_Absyn_Enumdecl*_tmpA29;struct Cyc_Absyn_Typedefdecl*
_tmpA2B;struct Cyc_List_List*_tmpA2F;struct Cyc_List_List*_tmpA31;struct Cyc_List_List*
_tmpA33;struct Cyc_List_List*_tmpA35;_LL44B: {struct Cyc_Absyn_Var_d_struct*
_tmpA1C=(struct Cyc_Absyn_Var_d_struct*)_tmpA1B;if(_tmpA1C->tag != 0)goto _LL44D;
else{_tmpA1D=_tmpA1C->f1;}}_LL44C: {struct _tuple0*_tmpA36=_tmpA1D->name;if(
_tmpA1D->sc == Cyc_Absyn_ExternC){struct _tuple0*_tmpE10;_tmpA36=((_tmpE10=
_cycalloc(sizeof(*_tmpE10)),((_tmpE10->f1=Cyc_Absyn_Rel_n(0),((_tmpE10->f2=(*
_tmpA36).f2,_tmpE10))))));}if(_tmpA1D->initializer != 0){if(cinclude)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmpA1D->initializer));else{Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_check_null(_tmpA1D->initializer));}}{struct Cyc_Absyn_Global_b_struct
_tmpE13;struct Cyc_Absyn_Global_b_struct*_tmpE12;nv=Cyc_Toc_add_varmap(r,nv,
_tmpA1D->name,Cyc_Absyn_varb_exp(_tmpA36,(void*)((_tmpE12=_cycalloc(sizeof(*
_tmpE12)),((_tmpE12[0]=((_tmpE13.tag=1,((_tmpE13.f1=_tmpA1D,_tmpE13)))),_tmpE12)))),
0));}_tmpA1D->name=_tmpA36;_tmpA1D->sc=Cyc_Toc_scope_to_c(_tmpA1D->sc);_tmpA1D->type=
Cyc_Toc_typ_to_c_array(_tmpA1D->type);{struct Cyc_List_List*_tmpE14;Cyc_Toc_result_decls=((
_tmpE14=_cycalloc(sizeof(*_tmpE14)),((_tmpE14->hd=d,((_tmpE14->tl=Cyc_Toc_result_decls,
_tmpE14))))));}goto _LL44A;}_LL44D: {struct Cyc_Absyn_Fn_d_struct*_tmpA1E=(struct
Cyc_Absyn_Fn_d_struct*)_tmpA1B;if(_tmpA1E->tag != 1)goto _LL44F;else{_tmpA1F=
_tmpA1E->f1;}}_LL44E: {struct _tuple0*_tmpA3B=_tmpA1F->name;if(_tmpA1F->sc == Cyc_Absyn_ExternC){{
struct _tuple0*_tmpE15;_tmpA3B=((_tmpE15=_cycalloc(sizeof(*_tmpE15)),((_tmpE15->f1=
Cyc_Absyn_Rel_n(0),((_tmpE15->f2=(*_tmpA3B).f2,_tmpE15))))));}_tmpA1F->sc=Cyc_Absyn_Public;}
nv=Cyc_Toc_add_varmap(r,nv,_tmpA1F->name,Cyc_Absyn_var_exp(_tmpA3B,0));_tmpA1F->name=
_tmpA3B;Cyc_Toc_fndecl_to_c(nv,_tmpA1F,cinclude);{struct Cyc_List_List*_tmpE16;
Cyc_Toc_result_decls=((_tmpE16=_cycalloc(sizeof(*_tmpE16)),((_tmpE16->hd=d,((
_tmpE16->tl=Cyc_Toc_result_decls,_tmpE16))))));}goto _LL44A;}_LL44F: {struct Cyc_Absyn_Let_d_struct*
_tmpA20=(struct Cyc_Absyn_Let_d_struct*)_tmpA1B;if(_tmpA20->tag != 2)goto _LL451;}
_LL450: goto _LL452;_LL451: {struct Cyc_Absyn_Letv_d_struct*_tmpA21=(struct Cyc_Absyn_Letv_d_struct*)
_tmpA1B;if(_tmpA21->tag != 3)goto _LL453;}_LL452: {const char*_tmpE19;void*_tmpE18;(
_tmpE18=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE19="letdecl at toplevel",_tag_dyneither(_tmpE19,sizeof(char),20))),
_tag_dyneither(_tmpE18,sizeof(void*),0)));}_LL453: {struct Cyc_Absyn_Region_d_struct*
_tmpA22=(struct Cyc_Absyn_Region_d_struct*)_tmpA1B;if(_tmpA22->tag != 4)goto _LL455;}
_LL454: {const char*_tmpE1C;void*_tmpE1B;(_tmpE1B=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE1C="region decl at toplevel",
_tag_dyneither(_tmpE1C,sizeof(char),24))),_tag_dyneither(_tmpE1B,sizeof(void*),0)));}
_LL455: {struct Cyc_Absyn_Alias_d_struct*_tmpA23=(struct Cyc_Absyn_Alias_d_struct*)
_tmpA1B;if(_tmpA23->tag != 5)goto _LL457;}_LL456: {const char*_tmpE1F;void*_tmpE1E;(
_tmpE1E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE1F="alias decl at toplevel",_tag_dyneither(_tmpE1F,sizeof(char),23))),
_tag_dyneither(_tmpE1E,sizeof(void*),0)));}_LL457: {struct Cyc_Absyn_Aggr_d_struct*
_tmpA24=(struct Cyc_Absyn_Aggr_d_struct*)_tmpA1B;if(_tmpA24->tag != 6)goto _LL459;
else{_tmpA25=_tmpA24->f1;}}_LL458: Cyc_Toc_aggrdecl_to_c(_tmpA25,1);goto _LL44A;
_LL459: {struct Cyc_Absyn_Datatype_d_struct*_tmpA26=(struct Cyc_Absyn_Datatype_d_struct*)
_tmpA1B;if(_tmpA26->tag != 7)goto _LL45B;else{_tmpA27=_tmpA26->f1;}}_LL45A: if(
_tmpA27->is_extensible)Cyc_Toc_xdatatypedecl_to_c(_tmpA27);else{Cyc_Toc_datatypedecl_to_c(
_tmpA27);}goto _LL44A;_LL45B: {struct Cyc_Absyn_Enum_d_struct*_tmpA28=(struct Cyc_Absyn_Enum_d_struct*)
_tmpA1B;if(_tmpA28->tag != 8)goto _LL45D;else{_tmpA29=_tmpA28->f1;}}_LL45C: Cyc_Toc_enumdecl_to_c(
_tmpA29);{struct Cyc_List_List*_tmpE20;Cyc_Toc_result_decls=((_tmpE20=_cycalloc(
sizeof(*_tmpE20)),((_tmpE20->hd=d,((_tmpE20->tl=Cyc_Toc_result_decls,_tmpE20))))));}
goto _LL44A;_LL45D: {struct Cyc_Absyn_Typedef_d_struct*_tmpA2A=(struct Cyc_Absyn_Typedef_d_struct*)
_tmpA1B;if(_tmpA2A->tag != 9)goto _LL45F;else{_tmpA2B=_tmpA2A->f1;}}_LL45E: _tmpA2B->name=
_tmpA2B->name;_tmpA2B->tvs=0;if(_tmpA2B->defn != 0)_tmpA2B->defn=(void*)Cyc_Toc_typ_to_c_array((
void*)_check_null(_tmpA2B->defn));else{switch(((struct Cyc_Absyn_Kind*)((struct
Cyc_Core_Opt*)_check_null(_tmpA2B->kind))->v)->kind){case Cyc_Absyn_BoxKind:
_LL46B: _tmpA2B->defn=(void*)Cyc_Absyn_void_star_typ();break;default: _LL46C:
_tmpA2B->defn=(void*)& Cyc_Absyn_VoidType_val;break;}}{struct Cyc_List_List*
_tmpE21;Cyc_Toc_result_decls=((_tmpE21=_cycalloc(sizeof(*_tmpE21)),((_tmpE21->hd=
d,((_tmpE21->tl=Cyc_Toc_result_decls,_tmpE21))))));}goto _LL44A;_LL45F: {struct
Cyc_Absyn_Porton_d_struct*_tmpA2C=(struct Cyc_Absyn_Porton_d_struct*)_tmpA1B;if(
_tmpA2C->tag != 14)goto _LL461;}_LL460: goto _LL462;_LL461: {struct Cyc_Absyn_Portoff_d_struct*
_tmpA2D=(struct Cyc_Absyn_Portoff_d_struct*)_tmpA1B;if(_tmpA2D->tag != 15)goto
_LL463;}_LL462: goto _LL44A;_LL463: {struct Cyc_Absyn_Namespace_d_struct*_tmpA2E=(
struct Cyc_Absyn_Namespace_d_struct*)_tmpA1B;if(_tmpA2E->tag != 10)goto _LL465;
else{_tmpA2F=_tmpA2E->f2;}}_LL464: _tmpA31=_tmpA2F;goto _LL466;_LL465: {struct Cyc_Absyn_Using_d_struct*
_tmpA30=(struct Cyc_Absyn_Using_d_struct*)_tmpA1B;if(_tmpA30->tag != 11)goto _LL467;
else{_tmpA31=_tmpA30->f2;}}_LL466: _tmpA33=_tmpA31;goto _LL468;_LL467: {struct Cyc_Absyn_ExternC_d_struct*
_tmpA32=(struct Cyc_Absyn_ExternC_d_struct*)_tmpA1B;if(_tmpA32->tag != 12)goto
_LL469;else{_tmpA33=_tmpA32->f1;}}_LL468: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA33,top,
cinclude);goto _LL44A;_LL469: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA34=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmpA1B;if(_tmpA34->tag != 13)goto _LL44A;
else{_tmpA35=_tmpA34->f1;}}_LL46A: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA35,top,1);goto
_LL44A;_LL44A:;};}return nv;}static void Cyc_Toc_init(){struct _DynRegionHandle*
_tmpA47;struct Cyc_Core_NewRegion _tmpA46=Cyc_Core__new_dynregion((const char*)"internal-error",
0);_tmpA47=_tmpA46.dynregion;{struct _DynRegionFrame _tmpA48;struct _RegionHandle*d=
_open_dynregion(& _tmpA48,_tmpA47);{struct Cyc_Dict_Dict*_tmpE2E;struct Cyc_Dict_Dict*
_tmpE2D;struct Cyc_Set_Set**_tmpE2C;struct Cyc_List_List**_tmpE2B;struct Cyc_Dict_Dict*
_tmpE2A;struct Cyc_List_List**_tmpE29;struct Cyc_Toc_TocState*_tmpE28;Cyc_Toc_toc_state=((
_tmpE28=_cycalloc(sizeof(*_tmpE28)),((_tmpE28->dyn=(struct _DynRegionHandle*)
_tmpA47,((_tmpE28->tuple_types=(struct Cyc_List_List**)((_tmpE29=_region_malloc(d,
sizeof(*_tmpE29)),((_tmpE29[0]=0,_tmpE29)))),((_tmpE28->aggrs_so_far=(struct Cyc_Dict_Dict*)((
_tmpE2A=_region_malloc(d,sizeof(*_tmpE2A)),((_tmpE2A[0]=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
d,Cyc_Absyn_qvar_cmp),_tmpE2A)))),((_tmpE28->abs_struct_types=(struct Cyc_List_List**)((
_tmpE2B=_region_malloc(d,sizeof(*_tmpE2B)),((_tmpE2B[0]=0,_tmpE2B)))),((_tmpE28->datatypes_so_far=(
struct Cyc_Set_Set**)((_tmpE2C=_region_malloc(d,sizeof(*_tmpE2C)),((_tmpE2C[0]=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE2C)))),((_tmpE28->xdatatypes_so_far=(
struct Cyc_Dict_Dict*)((_tmpE2D=_region_malloc(d,sizeof(*_tmpE2D)),((_tmpE2D[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE2D)))),((_tmpE28->qvar_tags=(
struct Cyc_Dict_Dict*)((_tmpE2E=_region_malloc(d,sizeof(*_tmpE2E)),((_tmpE2E[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct
_tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpE2E)))),((_tmpE28->temp_labels=(
struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(
d),(struct Cyc_Toc_TocState*)_tmpE28))))))))))))))))));}Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=
0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=0;{struct
_dyneither_ptr**_tmpE2F;Cyc_Toc_globals=_tag_dyneither(((_tmpE2F=_cycalloc(
sizeof(struct _dyneither_ptr*)* 41),((_tmpE2F[0]=& Cyc_Toc__throw_str,((_tmpE2F[1]=&
Cyc_Toc_setjmp_str,((_tmpE2F[2]=& Cyc_Toc__push_handler_str,((_tmpE2F[3]=& Cyc_Toc__pop_handler_str,((
_tmpE2F[4]=& Cyc_Toc__exn_thrown_str,((_tmpE2F[5]=& Cyc_Toc__npop_handler_str,((
_tmpE2F[6]=& Cyc_Toc__check_null_str,((_tmpE2F[7]=& Cyc_Toc__check_known_subscript_null_str,((
_tmpE2F[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpE2F[9]=& Cyc_Toc__check_dyneither_subscript_str,((
_tmpE2F[10]=& Cyc_Toc__dyneither_ptr_str,((_tmpE2F[11]=& Cyc_Toc__tag_dyneither_str,((
_tmpE2F[12]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpE2F[13]=& Cyc_Toc__untag_dyneither_ptr_str,((
_tmpE2F[14]=& Cyc_Toc__get_dyneither_size_str,((_tmpE2F[15]=& Cyc_Toc__get_zero_arr_size_str,((
_tmpE2F[16]=& Cyc_Toc__dyneither_ptr_plus_str,((_tmpE2F[17]=& Cyc_Toc__zero_arr_plus_str,((
_tmpE2F[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpE2F[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((
_tmpE2F[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpE2F[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((
_tmpE2F[22]=& Cyc_Toc__cycalloc_str,((_tmpE2F[23]=& Cyc_Toc__cyccalloc_str,((
_tmpE2F[24]=& Cyc_Toc__cycalloc_atomic_str,((_tmpE2F[25]=& Cyc_Toc__cyccalloc_atomic_str,((
_tmpE2F[26]=& Cyc_Toc__region_malloc_str,((_tmpE2F[27]=& Cyc_Toc__region_calloc_str,((
_tmpE2F[28]=& Cyc_Toc__check_times_str,((_tmpE2F[29]=& Cyc_Toc__new_region_str,((
_tmpE2F[30]=& Cyc_Toc__push_region_str,((_tmpE2F[31]=& Cyc_Toc__pop_region_str,((
_tmpE2F[32]=& Cyc_Toc__open_dynregion_str,((_tmpE2F[33]=& Cyc_Toc__push_dynregion_str,((
_tmpE2F[34]=& Cyc_Toc__pop_dynregion_str,((_tmpE2F[35]=& Cyc_Toc__reset_region_str,((
_tmpE2F[36]=& Cyc_Toc__throw_arraybounds_str,((_tmpE2F[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,((
_tmpE2F[38]=& Cyc_Toc__throw_match_str,((_tmpE2F[39]=& Cyc_Toc__swap_word_str,((
_tmpE2F[40]=& Cyc_Toc__swap_dyneither_str,_tmpE2F)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),41);};;_pop_dynregion(d);};}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();{struct _RegionHandle _tmpA51=
_new_region("start");struct _RegionHandle*start=& _tmpA51;_push_region(start);Cyc_Toc_decls_to_c(
start,Cyc_Toc_empty_env(start),ds,1,0);{struct _DynRegionHandle*_tmpA53;struct Cyc_Toc_TocState
_tmpA52=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA53=
_tmpA52.dyn;Cyc_Core_free_dynregion(_tmpA53);};{struct Cyc_List_List*_tmpA54=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);
_npop_handler(0);return _tmpA54;};;_pop_region(start);};}
