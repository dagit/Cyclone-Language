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
x);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*
r2,struct Cyc_List_List*x,struct Cyc_List_List*y);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Iter_Iter{void*env;int(*
next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct
Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(
void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*
s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[
7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern
char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[
7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict Cyc_Dict_rempty(
struct _RegionHandle*,int(*cmp)(void*,void*));struct Cyc_Dict_Dict Cyc_Dict_rshare(
struct _RegionHandle*,struct Cyc_Dict_Dict);struct Cyc_Dict_Dict Cyc_Dict_insert(
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
};struct Cyc_Absyn_RefCntRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{int
tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_struct{
int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};extern struct Cyc_Absyn_HeapRgn_struct
Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;
struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*f1;int f2;struct
Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};enum Cyc_Absyn_Format_Type{
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
int tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};struct
_union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};
struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _tuple4{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};
struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple6 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr
val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
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
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*
k1,struct Cyc_Absyn_Kind*k2);struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t,struct Cyc_Absyn_Kind*def);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(void*t);extern
struct Cyc_Absyn_Kind Cyc_Tcutil_ak;extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;extern
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*,struct Cyc_List_List*,void*);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(
void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple9{struct Cyc_Absyn_Tqual
f1;void*f2;};void*Cyc_Tcutil_snd_tqt(struct _tuple9*);void Cyc_Tcutil_check_no_qual(
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
int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*
ds);struct _tuple0*Cyc_Toc_temp_var();extern struct _dyneither_ptr Cyc_Toc_globals;
extern int Cyc_noexpand_r;int Cyc_Toc_warn_bounds_checks=0;int Cyc_Toc_warn_all_null_deref=
0;unsigned int Cyc_Toc_total_bounds_checks=0;unsigned int Cyc_Toc_bounds_checks_eliminated=
0;static struct Cyc_List_List*Cyc_Toc_result_decls=0;struct Cyc_Toc_TocState{struct
_DynRegionHandle*dyn;struct Cyc_List_List**tuple_types;struct Cyc_Dict_Dict*
aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**
datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*
qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=
0;struct _tuple13{struct _tuple0*f1;struct _dyneither_ptr f2;};int Cyc_Toc_qvar_tag_cmp(
struct _tuple13*x,struct _tuple13*y){struct _tuple0*_tmp1;struct _dyneither_ptr _tmp2;
struct _tuple13 _tmp0=*x;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;{struct _tuple0*_tmp4;struct
_dyneither_ptr _tmp5;struct _tuple13 _tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{int i=
Cyc_Absyn_qvar_cmp(_tmp1,_tmp4);if(i != 0)return i;return Cyc_strcmp((struct
_dyneither_ptr)_tmp2,(struct _dyneither_ptr)_tmp5);};};}struct _tuple14{struct Cyc_Absyn_Aggrdecl*
f1;void*f2;};void*Cyc_Toc_aggrdecl_type(struct _tuple0*q,void*(*type_maker)(
struct _tuple0*)){struct _DynRegionHandle*_tmp7;struct Cyc_Dict_Dict*_tmp8;struct
Cyc_Toc_TocState _tmp6=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp7=_tmp6.dyn;_tmp8=_tmp6.aggrs_so_far;{struct _DynRegionFrame _tmp9;struct
_RegionHandle*d=_open_dynregion(& _tmp9,_tmp7);{struct _tuple14**v=((struct
_tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp8,
q);if(v == 0){void*_tmpA=type_maker(q);_npop_handler(0);return _tmpA;}else{struct
_tuple14 _tmpC;void*_tmpD;struct _tuple14*_tmpB=*v;_tmpC=*_tmpB;_tmpD=_tmpC.f2;{
void*_tmpE=_tmpD;_npop_handler(0);return _tmpE;};}};_pop_dynregion(d);};}static
int Cyc_Toc_tuple_type_counter=0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=
0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_struct{
char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_struct{
char*tag;};static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const char*_tmpA91;void*_tmpA90;(_tmpA90=0,
Cyc_fprintf(Cyc_stderr,((_tmpA91="\n",_tag_dyneither(_tmpA91,sizeof(char),2))),
_tag_dyneither(_tmpA90,sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_struct _tmpA94;struct Cyc_Toc_Toc_Unimplemented_struct*
_tmpA93;(int)_throw((void*)((_tmpA93=_cycalloc_atomic(sizeof(*_tmpA93)),((
_tmpA93[0]=((_tmpA94.tag=Cyc_Toc_Toc_Unimplemented,_tmpA94)),_tmpA93)))));};}
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(
Cyc_stderr,fmt,ap);{const char*_tmpA97;void*_tmpA96;(_tmpA96=0,Cyc_fprintf(Cyc_stderr,((
_tmpA97="\n",_tag_dyneither(_tmpA97,sizeof(char),2))),_tag_dyneither(_tmpA96,
sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Impossible_struct
_tmpA9A;struct Cyc_Toc_Toc_Impossible_struct*_tmpA99;(int)_throw((void*)((_tmpA99=
_cycalloc_atomic(sizeof(*_tmpA99)),((_tmpA99[0]=((_tmpA9A.tag=Cyc_Toc_Toc_Impossible,
_tmpA9A)),_tmpA99)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_struct{
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
_tmpA9B;skip_stmt_opt=((_tmpA9B=_cycalloc(sizeof(*_tmpA9B)),((_tmpA9B[0]=Cyc_Absyn_skip_stmt(
0),_tmpA9B))));}return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(
void*t,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,
0);}static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Cast_e_struct
_tmpA9E;struct Cyc_Absyn_Cast_e_struct*_tmpA9D;return(void*)((_tmpA9D=_cycalloc(
sizeof(*_tmpA9D)),((_tmpA9D[0]=((_tmpA9E.tag=15,((_tmpA9E.f1=(void*)t,((_tmpA9E.f2=
e,((_tmpA9E.f3=0,((_tmpA9E.f4=Cyc_Absyn_No_coercion,_tmpA9E)))))))))),_tmpA9D))));}
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Deref_e_struct
_tmpAA1;struct Cyc_Absyn_Deref_e_struct*_tmpAA0;return(void*)((_tmpAA0=_cycalloc(
sizeof(*_tmpAA0)),((_tmpAA0[0]=((_tmpAA1.tag=21,((_tmpAA1.f1=e,_tmpAA1)))),
_tmpAA0))));}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){struct Cyc_Absyn_Subscript_e_struct _tmpAA4;struct Cyc_Absyn_Subscript_e_struct*
_tmpAA3;return(void*)((_tmpAA3=_cycalloc(sizeof(*_tmpAA3)),((_tmpAA3[0]=((
_tmpAA4.tag=24,((_tmpAA4.f1=e1,((_tmpAA4.f2=e2,_tmpAA4)))))),_tmpAA3))));}static
void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct
_tmpAA7;struct Cyc_Absyn_StmtExp_e_struct*_tmpAA6;return(void*)((_tmpAA6=
_cycalloc(sizeof(*_tmpAA6)),((_tmpAA6[0]=((_tmpAA7.tag=37,((_tmpAA7.f1=s,_tmpAA7)))),
_tmpAA6))));}static void*Cyc_Toc_sizeoftyp_exp_r(void*t){struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpAAA;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpAA9;return(void*)((_tmpAA9=
_cycalloc(sizeof(*_tmpAA9)),((_tmpAA9[0]=((_tmpAAA.tag=18,((_tmpAAA.f1=(void*)t,
_tmpAAA)))),_tmpAA9))));}static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct _tmpAAD;struct Cyc_Absyn_FnCall_e_struct*
_tmpAAC;return(void*)((_tmpAAC=_cycalloc(sizeof(*_tmpAAC)),((_tmpAAC[0]=((
_tmpAAD.tag=11,((_tmpAAD.f1=e,((_tmpAAD.f2=es,((_tmpAAD.f3=0,_tmpAAD)))))))),
_tmpAAC))));}static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_struct
_tmpAB0;struct Cyc_Absyn_Exp_s_struct*_tmpAAF;return(void*)((_tmpAAF=_cycalloc(
sizeof(*_tmpAAF)),((_tmpAAF[0]=((_tmpAB0.tag=1,((_tmpAB0.f1=e,_tmpAB0)))),
_tmpAAF))));}static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2){struct Cyc_Absyn_Seq_s_struct _tmpAB3;struct Cyc_Absyn_Seq_s_struct*_tmpAB2;
return(void*)((_tmpAB2=_cycalloc(sizeof(*_tmpAB2)),((_tmpAB2[0]=((_tmpAB3.tag=2,((
_tmpAB3.f1=s1,((_tmpAB3.f2=s2,_tmpAB3)))))),_tmpAB2))));}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct
_tmpAB6;struct Cyc_Absyn_Conditional_e_struct*_tmpAB5;return(void*)((_tmpAB5=
_cycalloc(sizeof(*_tmpAB5)),((_tmpAB5[0]=((_tmpAB6.tag=6,((_tmpAB6.f1=e1,((
_tmpAB6.f2=e2,((_tmpAB6.f3=e3,_tmpAB6)))))))),_tmpAB5))));}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrMember_e_struct
_tmpAB9;struct Cyc_Absyn_AggrMember_e_struct*_tmpAB8;return(void*)((_tmpAB8=
_cycalloc(sizeof(*_tmpAB8)),((_tmpAB8[0]=((_tmpAB9.tag=22,((_tmpAB9.f1=e,((
_tmpAB9.f2=n,((_tmpAB9.f3=0,((_tmpAB9.f4=0,_tmpAB9)))))))))),_tmpAB8))));}static
void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_struct _tmpABC;struct Cyc_Absyn_AggrArrow_e_struct*
_tmpABB;return(void*)((_tmpABB=_cycalloc(sizeof(*_tmpABB)),((_tmpABB[0]=((
_tmpABC.tag=23,((_tmpABC.f1=e,((_tmpABC.f2=n,((_tmpABC.f3=0,((_tmpABC.f4=0,
_tmpABC)))))))))),_tmpABB))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmpABF;
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpABE;return(void*)((_tmpABE=_cycalloc(
sizeof(*_tmpABE)),((_tmpABE[0]=((_tmpABF.tag=36,((_tmpABF.f1=tdopt,((_tmpABF.f2=
ds,_tmpABF)))))),_tmpABE))));}static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_struct _tmpAC2;
struct Cyc_Absyn_Goto_s_struct*_tmpAC1;return(void*)((_tmpAC1=_cycalloc(sizeof(*
_tmpAC1)),((_tmpAC1[0]=((_tmpAC2.tag=8,((_tmpAC2.f1=v,((_tmpAC2.f2=s,_tmpAC2)))))),
_tmpAC1))));}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,{.Int_c={4,{
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
_tmpAC8;const char*_tmpAC7;struct Cyc_Core_Impossible_struct*_tmpAC6;(int)_throw((
void*)((_tmpAC6=_cycalloc(sizeof(*_tmpAC6)),((_tmpAC6[0]=((_tmpAC8.tag=Cyc_Core_Impossible,((
_tmpAC8.f1=((_tmpAC7="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpAC7,sizeof(char),44))),_tmpAC8)))),_tmpAC6)))));}}goto _LL0;
_LL3: {struct Cyc_Absyn_FloatType_struct*_tmpC9=(struct Cyc_Absyn_FloatType_struct*)
_tmpC6;if(_tmpC9->tag != 7)goto _LL5;}_LL4: function=fS->ffloat;goto _LL0;_LL5: {
struct Cyc_Absyn_DoubleType_struct*_tmpCA=(struct Cyc_Absyn_DoubleType_struct*)
_tmpC6;if(_tmpCA->tag != 8)goto _LL7;else{_tmpCB=_tmpCA->f1;}}_LL6: switch(_tmpCB){
case 1: _LL10: function=fS->flongdouble;break;default: _LL11: function=fS->fdouble;}
goto _LL0;_LL7: {struct Cyc_Absyn_PointerType_struct*_tmpCC=(struct Cyc_Absyn_PointerType_struct*)
_tmpC6;if(_tmpCC->tag != 5)goto _LL9;}_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;
_LLA: {struct Cyc_Core_Impossible_struct _tmpAD5;const char*_tmpAD4;void*_tmpAD3[1];
struct Cyc_String_pa_struct _tmpAD2;struct Cyc_Core_Impossible_struct*_tmpAD1;(int)
_throw((void*)((_tmpAD1=_cycalloc(sizeof(*_tmpAD1)),((_tmpAD1[0]=((_tmpAD5.tag=
Cyc_Core_Impossible,((_tmpAD5.f1=(struct _dyneither_ptr)((_tmpAD2.tag=0,((_tmpAD2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAD3[
0]=& _tmpAD2,Cyc_aprintf(((_tmpAD4="impossible expression type %s (not int, float, double, or pointer)",
_tag_dyneither(_tmpAD4,sizeof(char),67))),_tag_dyneither(_tmpAD3,sizeof(void*),1)))))))),
_tmpAD5)))),_tmpAD1)))));}_LL0:;}return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(
fS,(void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v);}struct Cyc_Absyn_Exp*
Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*
arr){void*_tmpD5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
arr->topt))->v);struct Cyc_Absyn_PtrInfo _tmpD7;void*_tmpD8;_LL14: {struct Cyc_Absyn_PointerType_struct*
_tmpD6=(struct Cyc_Absyn_PointerType_struct*)_tmpD5;if(_tmpD6->tag != 5)goto _LL16;
else{_tmpD7=_tmpD6->f1;_tmpD8=_tmpD7.elt_typ;}}_LL15: return Cyc_Toc_getFunctionType(
fS,_tmpD8);_LL16:;_LL17: {struct Cyc_Core_Impossible_struct _tmpADB;const char*
_tmpADA;struct Cyc_Core_Impossible_struct*_tmpAD9;(int)_throw((void*)((_tmpAD9=
_cycalloc(sizeof(*_tmpAD9)),((_tmpAD9[0]=((_tmpADB.tag=Cyc_Core_Impossible,((
_tmpADB.f1=((_tmpADA="impossible type (not pointer)",_tag_dyneither(_tmpADA,
sizeof(char),30))),_tmpADB)))),_tmpAD9)))));}_LL13:;}struct _tuple15{struct Cyc_List_List*
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
Cyc_Absyn_conref_def)(0,_tmp100);_LL34:;_LL35: {const char*_tmpADE;void*_tmpADD;(
_tmpADD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpADE="is_nullable",_tag_dyneither(_tmpADE,sizeof(char),12))),_tag_dyneither(
_tmpADD,sizeof(void*),0)));}_LL31:;}static char _tmp10F[1]="";static char _tmp110[8]="*bogus*";
static struct _tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct
_dyneither_ptr tag){struct _DynRegionHandle*_tmp104;struct Cyc_Dict_Dict*_tmp105;
struct Cyc_Toc_TocState _tmp103=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp104=_tmp103.dyn;_tmp105=_tmp103.qvar_tags;{static struct _dyneither_ptr
bogus_string={_tmp110,_tmp110,_tmp110 + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
4,0}},& bogus_string};static struct _tuple13 pair={& bogus_qvar,{_tmp10F,_tmp10F,
_tmp10F + 1}};{struct _tuple13 _tmpADF;pair=((_tmpADF.f1=x,((_tmpADF.f2=tag,_tmpADF))));}{
struct _DynRegionFrame _tmp107;struct _RegionHandle*d=_open_dynregion(& _tmp107,
_tmp104);{struct _tuple0**_tmp108=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,
struct _tuple13*k))Cyc_Dict_lookup_opt)(*_tmp105,(struct _tuple13*)& pair);if(
_tmp108 != 0){struct _tuple0*_tmp109=*_tmp108;_npop_handler(0);return _tmp109;}{
struct _tuple13*_tmpAE0;struct _tuple13*_tmp10A=(_tmpAE0=_cycalloc(sizeof(*_tmpAE0)),((
_tmpAE0->f1=x,((_tmpAE0->f2=tag,_tmpAE0)))));struct _dyneither_ptr*_tmpAE3;struct
_tuple0*_tmpAE2;struct _tuple0*res=(_tmpAE2=_cycalloc(sizeof(*_tmpAE2)),((_tmpAE2->f1=(*
x).f1,((_tmpAE2->f2=((_tmpAE3=_cycalloc(sizeof(*_tmpAE3)),((_tmpAE3[0]=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*x).f2,(struct
_dyneither_ptr)tag),_tmpAE3)))),_tmpAE2)))));*_tmp105=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple13*k,struct _tuple0*v))Cyc_Dict_insert)(*_tmp105,(
struct _tuple13*)_tmp10A,res);{struct _tuple0*_tmp10B=res;_npop_handler(0);return
_tmp10B;};};};_pop_dynregion(d);};};}static void*Cyc_Toc_typ_to_c(void*t);struct
_tuple16{void*f1;struct Cyc_List_List*f2;};static void*Cyc_Toc_add_tuple_type(
struct Cyc_List_List*tqs0){struct _DynRegionHandle*_tmp112;struct Cyc_List_List**
_tmp113;struct Cyc_Toc_TocState _tmp111=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp112=_tmp111.dyn;_tmp113=_tmp111.tuple_types;{struct _DynRegionFrame _tmp114;
struct _RegionHandle*d=_open_dynregion(& _tmp114,_tmp112);{struct Cyc_List_List*
_tmp115=*_tmp113;for(0;_tmp115 != 0;_tmp115=_tmp115->tl){struct _tuple16 _tmp117;
void*_tmp118;struct Cyc_List_List*_tmp119;struct _tuple16*_tmp116=(struct _tuple16*)
_tmp115->hd;_tmp117=*_tmp116;_tmp118=_tmp117.f1;_tmp119=_tmp117.f2;{struct Cyc_List_List*
_tmp11A=tqs0;for(0;_tmp11A != 0  && _tmp119 != 0;(_tmp11A=_tmp11A->tl,_tmp119=
_tmp119->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple9*)_tmp11A->hd)).f2,(void*)
_tmp119->hd))break;}if(_tmp11A == 0  && _tmp119 == 0){void*_tmp11B=_tmp118;
_npop_handler(0);return _tmp11B;}};}}{struct Cyc_Int_pa_struct _tmpAEB;void*_tmpAEA[
1];const char*_tmpAE9;struct _dyneither_ptr*_tmpAE8;struct _dyneither_ptr*xname=(
_tmpAE8=_cycalloc(sizeof(*_tmpAE8)),((_tmpAE8[0]=(struct _dyneither_ptr)((_tmpAEB.tag=
1,((_tmpAEB.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAEA[0]=& _tmpAEB,
Cyc_aprintf(((_tmpAE9="_tuple%d",_tag_dyneither(_tmpAE9,sizeof(char),9))),
_tag_dyneither(_tmpAEA,sizeof(void*),1)))))))),_tmpAE8)));void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp11C=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmpAEE;struct Cyc_List_List*
_tmpAED;_tmp11C=((_tmpAED=_cycalloc(sizeof(*_tmpAED)),((_tmpAED->hd=((_tmpAEE=
_cycalloc(sizeof(*_tmpAEE)),((_tmpAEE->name=Cyc_Absyn_fieldname(i),((_tmpAEE->tq=
Cyc_Toc_mt_tq,((_tmpAEE->type=(void*)ts2->hd,((_tmpAEE->width=0,((_tmpAEE->attributes=
0,_tmpAEE)))))))))))),((_tmpAED->tl=_tmp11C,_tmpAED))))));}}_tmp11C=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp11C);{struct Cyc_Absyn_AggrdeclImpl*
_tmpAF3;struct _tuple0*_tmpAF2;struct Cyc_Absyn_Aggrdecl*_tmpAF1;struct Cyc_Absyn_Aggrdecl*
_tmp11F=(_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1->kind=Cyc_Absyn_StructA,((
_tmpAF1->sc=Cyc_Absyn_Public,((_tmpAF1->name=((_tmpAF2=_cycalloc(sizeof(*_tmpAF2)),((
_tmpAF2->f1=Cyc_Absyn_Rel_n(0),((_tmpAF2->f2=xname,_tmpAF2)))))),((_tmpAF1->tvs=
0,((_tmpAF1->impl=((_tmpAF3=_cycalloc(sizeof(*_tmpAF3)),((_tmpAF3->exist_vars=0,((
_tmpAF3->rgn_po=0,((_tmpAF3->fields=_tmp11C,((_tmpAF3->tagged=0,_tmpAF3)))))))))),((
_tmpAF1->attributes=0,_tmpAF1)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*
_tmpAF9;struct Cyc_Absyn_Aggr_d_struct _tmpAF8;struct Cyc_List_List*_tmpAF7;Cyc_Toc_result_decls=((
_tmpAF7=_cycalloc(sizeof(*_tmpAF7)),((_tmpAF7->hd=Cyc_Absyn_new_decl((void*)((
_tmpAF9=_cycalloc(sizeof(*_tmpAF9)),((_tmpAF9[0]=((_tmpAF8.tag=6,((_tmpAF8.f1=
_tmp11F,_tmpAF8)))),_tmpAF9)))),0),((_tmpAF7->tl=Cyc_Toc_result_decls,_tmpAF7))))));}{
struct _tuple16*_tmpAFC;struct Cyc_List_List*_tmpAFB;*_tmp113=((_tmpAFB=
_region_malloc(d,sizeof(*_tmpAFB)),((_tmpAFB->hd=((_tmpAFC=_region_malloc(d,
sizeof(*_tmpAFC)),((_tmpAFC->f1=x,((_tmpAFC->f2=ts,_tmpAFC)))))),((_tmpAFB->tl=*
_tmp113,_tmpAFB))))));}{void*_tmp125=x;_npop_handler(0);return _tmp125;};};};;
_pop_dynregion(d);};}struct _tuple17{struct _tuple0*f1;struct Cyc_List_List*f2;void*
f3;};static void*Cyc_Toc_add_struct_type(struct _tuple0*struct_name,struct Cyc_List_List*
type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){struct
_DynRegionHandle*_tmp12E;struct Cyc_List_List**_tmp12F;struct Cyc_Toc_TocState
_tmp12D=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp12E=
_tmp12D.dyn;_tmp12F=_tmp12D.abs_struct_types;{struct _DynRegionFrame _tmp130;
struct _RegionHandle*d=_open_dynregion(& _tmp130,_tmp12E);{struct Cyc_List_List*
_tmp131=*_tmp12F;for(0;_tmp131 != 0;_tmp131=_tmp131->tl){struct _tuple17 _tmp133;
struct _tuple0*_tmp134;struct Cyc_List_List*_tmp135;void*_tmp136;struct _tuple17*
_tmp132=(struct _tuple17*)_tmp131->hd;_tmp133=*_tmp132;_tmp134=_tmp133.f1;_tmp135=
_tmp133.f2;_tmp136=_tmp133.f3;if(Cyc_Absyn_qvar_cmp(_tmp134,struct_name)== 0  && ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(type_args)== ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp135)){int okay=1;{struct Cyc_List_List*_tmp137=type_args;
for(0;_tmp137 != 0;(_tmp137=_tmp137->tl,_tmp135=_tmp135->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp137->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp135))->hd)){okay=
0;break;}}}if(okay){void*_tmp138=_tmp136;_npop_handler(0);return _tmp138;}}}}{
struct Cyc_Int_pa_struct _tmpB04;void*_tmpB03[1];const char*_tmpB02;struct
_dyneither_ptr*_tmpB01;struct _dyneither_ptr*xname=(_tmpB01=_cycalloc(sizeof(*
_tmpB01)),((_tmpB01[0]=(struct _dyneither_ptr)((_tmpB04.tag=1,((_tmpB04.f1=(
unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB03[0]=& _tmpB04,Cyc_aprintf(((
_tmpB02="_tuple%d",_tag_dyneither(_tmpB02,sizeof(char),9))),_tag_dyneither(
_tmpB03,sizeof(void*),1)))))))),_tmpB01)));void*x=Cyc_Absyn_strct(xname);struct
Cyc_List_List*_tmp139=0;{struct _tuple17*_tmpB07;struct Cyc_List_List*_tmpB06;*
_tmp12F=((_tmpB06=_region_malloc(d,sizeof(*_tmpB06)),((_tmpB06->hd=((_tmpB07=
_region_malloc(d,sizeof(*_tmpB07)),((_tmpB07->f1=struct_name,((_tmpB07->f2=
type_args,((_tmpB07->f3=x,_tmpB07)))))))),((_tmpB06->tl=*_tmp12F,_tmpB06))))));}{
struct _RegionHandle _tmp13C=_new_region("r");struct _RegionHandle*r=& _tmp13C;
_push_region(r);{struct Cyc_List_List*_tmp13D=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,type_vars,type_args);for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp13E=(struct Cyc_Absyn_Aggrfield*)fields->hd;void*t=
_tmp13E->type;t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp13D,t));if(Cyc_Tcutil_unify(
t,(void*)& Cyc_Absyn_VoidType_val))break;t=Cyc_Toc_typ_to_c(t);{struct Cyc_Absyn_Aggrfield*
_tmpB0A;struct Cyc_List_List*_tmpB09;_tmp139=((_tmpB09=_cycalloc(sizeof(*_tmpB09)),((
_tmpB09->hd=((_tmpB0A=_cycalloc(sizeof(*_tmpB0A)),((_tmpB0A->name=_tmp13E->name,((
_tmpB0A->tq=Cyc_Toc_mt_tq,((_tmpB0A->type=t,((_tmpB0A->width=_tmp13E->width,((
_tmpB0A->attributes=_tmp13E->attributes,_tmpB0A)))))))))))),((_tmpB09->tl=
_tmp139,_tmpB09))))));};}_tmp139=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp139);{struct Cyc_Absyn_AggrdeclImpl*_tmpB0F;struct _tuple0*
_tmpB0E;struct Cyc_Absyn_Aggrdecl*_tmpB0D;struct Cyc_Absyn_Aggrdecl*_tmp141=(
_tmpB0D=_cycalloc(sizeof(*_tmpB0D)),((_tmpB0D->kind=Cyc_Absyn_StructA,((_tmpB0D->sc=
Cyc_Absyn_Public,((_tmpB0D->name=((_tmpB0E=_cycalloc(sizeof(*_tmpB0E)),((_tmpB0E->f1=
Cyc_Absyn_Rel_n(0),((_tmpB0E->f2=xname,_tmpB0E)))))),((_tmpB0D->tvs=0,((_tmpB0D->impl=((
_tmpB0F=_cycalloc(sizeof(*_tmpB0F)),((_tmpB0F->exist_vars=0,((_tmpB0F->rgn_po=0,((
_tmpB0F->fields=_tmp139,((_tmpB0F->tagged=0,_tmpB0F)))))))))),((_tmpB0D->attributes=
0,_tmpB0D)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpB15;struct Cyc_Absyn_Aggr_d_struct
_tmpB14;struct Cyc_List_List*_tmpB13;Cyc_Toc_result_decls=((_tmpB13=_cycalloc(
sizeof(*_tmpB13)),((_tmpB13->hd=Cyc_Absyn_new_decl((void*)((_tmpB15=_cycalloc(
sizeof(*_tmpB15)),((_tmpB15[0]=((_tmpB14.tag=6,((_tmpB14.f1=_tmp141,_tmpB14)))),
_tmpB15)))),0),((_tmpB13->tl=Cyc_Toc_result_decls,_tmpB13))))));}{void*_tmp145=x;
_npop_handler(1);return _tmp145;};};};_pop_region(r);};};;_pop_dynregion(d);};}
struct _tuple0*Cyc_Toc_temp_var(){int _tmp14D=Cyc_Toc_temp_var_counter ++;struct
_dyneither_ptr*_tmpB22;const char*_tmpB21;void*_tmpB20[1];struct Cyc_Int_pa_struct
_tmpB1F;struct _tuple0*_tmpB1E;struct _tuple0*res=(_tmpB1E=_cycalloc(sizeof(*
_tmpB1E)),((_tmpB1E->f1=Cyc_Absyn_Loc_n,((_tmpB1E->f2=((_tmpB22=_cycalloc(
sizeof(*_tmpB22)),((_tmpB22[0]=(struct _dyneither_ptr)((_tmpB1F.tag=1,((_tmpB1F.f1=(
unsigned int)_tmp14D,((_tmpB20[0]=& _tmpB1F,Cyc_aprintf(((_tmpB21="_tmp%X",
_tag_dyneither(_tmpB21,sizeof(char),7))),_tag_dyneither(_tmpB20,sizeof(void*),1)))))))),
_tmpB22)))),_tmpB1E)))));return res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp154;struct Cyc_Xarray_Xarray*_tmp155;struct Cyc_Toc_TocState
_tmp153=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp154=
_tmp153.dyn;_tmp155=_tmp153.temp_labels;{struct _DynRegionFrame _tmp156;struct
_RegionHandle*d=_open_dynregion(& _tmp156,_tmp154);{int _tmp157=Cyc_Toc_fresh_label_counter
++;if(_tmp157 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp155)){
struct _dyneither_ptr*_tmp158=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp155,_tmp157);_npop_handler(0);return _tmp158;}{struct Cyc_Int_pa_struct
_tmpB2A;void*_tmpB29[1];const char*_tmpB28;struct _dyneither_ptr*_tmpB27;struct
_dyneither_ptr*res=(_tmpB27=_cycalloc(sizeof(*_tmpB27)),((_tmpB27[0]=(struct
_dyneither_ptr)((_tmpB2A.tag=1,((_tmpB2A.f1=(unsigned int)_tmp157,((_tmpB29[0]=&
_tmpB2A,Cyc_aprintf(((_tmpB28="_LL%X",_tag_dyneither(_tmpB28,sizeof(char),6))),
_tag_dyneither(_tmpB29,sizeof(void*),1)))))))),_tmpB27)));if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp155,res)!= _tmp157){const char*
_tmpB2D;void*_tmpB2C;(_tmpB2C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2D="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmpB2D,sizeof(char),43))),_tag_dyneither(_tmpB2C,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp15B=res;_npop_handler(0);return _tmp15B;};};};
_pop_dynregion(d);};}static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*
td,struct _tuple0*name){int ans=0;struct Cyc_List_List*_tmp160=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((
struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp160))->hd)->name)
!= 0){++ ans;_tmp160=_tmp160->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);static void Cyc_Toc_aggrdecl_to_c(
struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);static void Cyc_Toc_datatypedecl_to_c(
struct Cyc_Absyn_Datatypedecl*tud);static struct _tuple7*Cyc_Toc_arg_to_c(struct
_tuple7*a){struct Cyc_Core_Opt*_tmp162;struct Cyc_Absyn_Tqual _tmp163;void*_tmp164;
struct _tuple7 _tmp161=*a;_tmp162=_tmp161.f1;_tmp163=_tmp161.f2;_tmp164=_tmp161.f3;{
struct _tuple7*_tmpB2E;return(_tmpB2E=_cycalloc(sizeof(*_tmpB2E)),((_tmpB2E->f1=
_tmp162,((_tmpB2E->f2=_tmp163,((_tmpB2E->f3=Cyc_Toc_typ_to_c(_tmp164),_tmpB2E)))))));};}
static struct _tuple9*Cyc_Toc_typ_to_c_f(struct _tuple9*x){struct Cyc_Absyn_Tqual
_tmp167;void*_tmp168;struct _tuple9 _tmp166=*x;_tmp167=_tmp166.f1;_tmp168=_tmp166.f2;{
struct _tuple9*_tmpB2F;return(_tmpB2F=_cycalloc(sizeof(*_tmpB2F)),((_tmpB2F->f1=
_tmp167,((_tmpB2F->f2=Cyc_Toc_typ_to_c(_tmp168),_tmpB2F)))));};}static void*Cyc_Toc_typ_to_c_array(
void*t){void*_tmp16A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp16C;
void*_tmp16D;struct Cyc_Absyn_Tqual _tmp16E;struct Cyc_Absyn_Exp*_tmp16F;union Cyc_Absyn_Constraint*
_tmp170;struct Cyc_Position_Segment*_tmp171;struct Cyc_Core_Opt*_tmp173;struct Cyc_Core_Opt
_tmp174;void*_tmp175;_LL37: {struct Cyc_Absyn_ArrayType_struct*_tmp16B=(struct Cyc_Absyn_ArrayType_struct*)
_tmp16A;if(_tmp16B->tag != 9)goto _LL39;else{_tmp16C=_tmp16B->f1;_tmp16D=_tmp16C.elt_type;
_tmp16E=_tmp16C.tq;_tmp16F=_tmp16C.num_elts;_tmp170=_tmp16C.zero_term;_tmp171=
_tmp16C.zt_loc;}}_LL38: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp16D),
_tmp16E,_tmp16F,Cyc_Absyn_false_conref,_tmp171);_LL39: {struct Cyc_Absyn_Evar_struct*
_tmp172=(struct Cyc_Absyn_Evar_struct*)_tmp16A;if(_tmp172->tag != 1)goto _LL3B;
else{_tmp173=_tmp172->f2;if(_tmp173 == 0)goto _LL3B;_tmp174=*_tmp173;_tmp175=(void*)
_tmp174.v;}}_LL3A: return Cyc_Toc_typ_to_c_array(_tmp175);_LL3B:;_LL3C: return Cyc_Toc_typ_to_c(
t);_LL36:;}static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*
f){struct Cyc_Absyn_Aggrfield*_tmpB30;return(_tmpB30=_cycalloc(sizeof(*_tmpB30)),((
_tmpB30->name=f->name,((_tmpB30->tq=Cyc_Toc_mt_tq,((_tmpB30->type=Cyc_Toc_typ_to_c(
f->type),((_tmpB30->width=f->width,((_tmpB30->attributes=f->attributes,_tmpB30)))))))))));}
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){return;}static void*Cyc_Toc_char_star_typ(){
static void**cs=0;if(cs == 0){void**_tmpB31;cs=((_tmpB31=_cycalloc(sizeof(*_tmpB31)),((
_tmpB31[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpB31))));}return*cs;}static void*Cyc_Toc_rgn_typ(){
static void**r=0;if(r == 0){void**_tmpB32;r=((_tmpB32=_cycalloc(sizeof(*_tmpB32)),((
_tmpB32[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),
_tmpB32))));}return*r;}static void*Cyc_Toc_dyn_rgn_typ(){static void**r=0;if(r == 0){
void**_tmpB33;r=((_tmpB33=_cycalloc(sizeof(*_tmpB33)),((_tmpB33[0]=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpB33))));}return*
r;}static int Cyc_Toc_is_boxed_tvar(void*t){void*_tmp17A=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_Tvar*_tmp17C;_LL3E: {struct Cyc_Absyn_VarType_struct*_tmp17B=(
struct Cyc_Absyn_VarType_struct*)_tmp17A;if(_tmp17B->tag != 2)goto _LL40;else{
_tmp17C=_tmp17B->f1;}}_LL3F: return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),&
Cyc_Tcutil_tbk);_LL40:;_LL41: return 0;_LL3D:;}static int Cyc_Toc_is_abs_tvar(void*t){
void*_tmp17D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp17F;_LL43: {struct
Cyc_Absyn_VarType_struct*_tmp17E=(struct Cyc_Absyn_VarType_struct*)_tmp17D;if(
_tmp17E->tag != 2)goto _LL45;else{_tmp17F=_tmp17E->f1;}}_LL44: return Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t));_LL45:;_LL46: return 0;_LL42:;}static void*
Cyc_Toc_typ_to_c(void*t){void*_tmp180=t;struct Cyc_Core_Opt*_tmp183;struct Cyc_Core_Opt*
_tmp185;struct Cyc_Core_Opt _tmp186;void*_tmp187;struct Cyc_Absyn_Tvar*_tmp189;
struct Cyc_Absyn_DatatypeFieldInfo _tmp18C;union Cyc_Absyn_DatatypeFieldInfoU
_tmp18D;struct _tuple1 _tmp18E;struct Cyc_Absyn_Datatypedecl*_tmp18F;struct Cyc_Absyn_Datatypefield*
_tmp190;struct Cyc_Absyn_PtrInfo _tmp193;void*_tmp194;struct Cyc_Absyn_Tqual _tmp195;
struct Cyc_Absyn_PtrAtts _tmp196;union Cyc_Absyn_Constraint*_tmp197;struct Cyc_Absyn_ArrayInfo
_tmp19C;void*_tmp19D;struct Cyc_Absyn_Tqual _tmp19E;struct Cyc_Absyn_Exp*_tmp19F;
struct Cyc_Position_Segment*_tmp1A0;struct Cyc_Absyn_FnInfo _tmp1A2;struct Cyc_Absyn_Tqual
_tmp1A3;void*_tmp1A4;struct Cyc_List_List*_tmp1A5;int _tmp1A6;struct Cyc_Absyn_VarargInfo*
_tmp1A7;struct Cyc_List_List*_tmp1A8;struct Cyc_List_List*_tmp1AA;enum Cyc_Absyn_AggrKind
_tmp1AC;struct Cyc_List_List*_tmp1AD;struct Cyc_Absyn_AggrInfo _tmp1AF;union Cyc_Absyn_AggrInfoU
_tmp1B0;struct Cyc_List_List*_tmp1B1;struct _tuple0*_tmp1B3;struct Cyc_List_List*
_tmp1B5;struct _tuple0*_tmp1B7;struct Cyc_List_List*_tmp1B8;struct Cyc_Absyn_Typedefdecl*
_tmp1B9;void**_tmp1BA;void*_tmp1BD;struct Cyc_Absyn_TypeDecl*_tmp1C7;struct Cyc_Absyn_TypeDecl
_tmp1C8;void*_tmp1C9;struct Cyc_Absyn_Aggrdecl*_tmp1CB;struct Cyc_Absyn_TypeDecl*
_tmp1CD;struct Cyc_Absyn_TypeDecl _tmp1CE;void*_tmp1CF;struct Cyc_Absyn_Enumdecl*
_tmp1D1;struct Cyc_Absyn_TypeDecl*_tmp1D3;struct Cyc_Absyn_TypeDecl _tmp1D4;void*
_tmp1D5;struct Cyc_Absyn_Datatypedecl*_tmp1D7;void**_tmp1D8;_LL48: {struct Cyc_Absyn_VoidType_struct*
_tmp181=(struct Cyc_Absyn_VoidType_struct*)_tmp180;if(_tmp181->tag != 0)goto _LL4A;}
_LL49: return t;_LL4A: {struct Cyc_Absyn_Evar_struct*_tmp182=(struct Cyc_Absyn_Evar_struct*)
_tmp180;if(_tmp182->tag != 1)goto _LL4C;else{_tmp183=_tmp182->f2;if(_tmp183 != 0)
goto _LL4C;}}_LL4B: return Cyc_Absyn_sint_typ;_LL4C: {struct Cyc_Absyn_Evar_struct*
_tmp184=(struct Cyc_Absyn_Evar_struct*)_tmp180;if(_tmp184->tag != 1)goto _LL4E;
else{_tmp185=_tmp184->f2;if(_tmp185 == 0)goto _LL4E;_tmp186=*_tmp185;_tmp187=(void*)
_tmp186.v;}}_LL4D: return Cyc_Toc_typ_to_c(_tmp187);_LL4E: {struct Cyc_Absyn_VarType_struct*
_tmp188=(struct Cyc_Absyn_VarType_struct*)_tmp180;if(_tmp188->tag != 2)goto _LL50;
else{_tmp189=_tmp188->f1;}}_LL4F: if((Cyc_Tcutil_tvar_kind(_tmp189,& Cyc_Tcutil_bk))->kind
== Cyc_Absyn_AnyKind)return(void*)& Cyc_Absyn_VoidType_val;else{return Cyc_Absyn_void_star_typ();}
_LL50: {struct Cyc_Absyn_DatatypeType_struct*_tmp18A=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp180;if(_tmp18A->tag != 3)goto _LL52;}_LL51: return(void*)& Cyc_Absyn_VoidType_val;
_LL52: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp18B=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp180;if(_tmp18B->tag != 4)goto _LL54;else{_tmp18C=_tmp18B->f1;_tmp18D=_tmp18C.field_info;
if((_tmp18D.KnownDatatypefield).tag != 2)goto _LL54;_tmp18E=(struct _tuple1)(
_tmp18D.KnownDatatypefield).val;_tmp18F=_tmp18E.f1;_tmp190=_tmp18E.f2;}}_LL53: {
const char*_tmpB34;return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp190->name,((
_tmpB34="_struct",_tag_dyneither(_tmpB34,sizeof(char),8)))));}_LL54: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp191=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp180;if(_tmp191->tag != 4)
goto _LL56;}_LL55: {const char*_tmpB37;void*_tmpB36;(_tmpB36=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB37="unresolved DatatypeFieldType",
_tag_dyneither(_tmpB37,sizeof(char),29))),_tag_dyneither(_tmpB36,sizeof(void*),0)));}
_LL56: {struct Cyc_Absyn_PointerType_struct*_tmp192=(struct Cyc_Absyn_PointerType_struct*)
_tmp180;if(_tmp192->tag != 5)goto _LL58;else{_tmp193=_tmp192->f1;_tmp194=_tmp193.elt_typ;
_tmp195=_tmp193.elt_tq;_tmp196=_tmp193.ptr_atts;_tmp197=_tmp196.bounds;}}_LL57:
_tmp194=Cyc_Toc_typ_to_c_array(_tmp194);{void*_tmp1DC=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp197);_LL89: {
struct Cyc_Absyn_DynEither_b_struct*_tmp1DD=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp1DC;if(_tmp1DD->tag != 0)goto _LL8B;}_LL8A: return Cyc_Toc_dyneither_ptr_typ;
_LL8B:;_LL8C: return Cyc_Absyn_star_typ(_tmp194,(void*)& Cyc_Absyn_HeapRgn_val,
_tmp195,Cyc_Absyn_false_conref);_LL88:;};_LL58: {struct Cyc_Absyn_IntType_struct*
_tmp198=(struct Cyc_Absyn_IntType_struct*)_tmp180;if(_tmp198->tag != 6)goto _LL5A;}
_LL59: goto _LL5B;_LL5A: {struct Cyc_Absyn_FloatType_struct*_tmp199=(struct Cyc_Absyn_FloatType_struct*)
_tmp180;if(_tmp199->tag != 7)goto _LL5C;}_LL5B: goto _LL5D;_LL5C: {struct Cyc_Absyn_DoubleType_struct*
_tmp19A=(struct Cyc_Absyn_DoubleType_struct*)_tmp180;if(_tmp19A->tag != 8)goto
_LL5E;}_LL5D: return t;_LL5E: {struct Cyc_Absyn_ArrayType_struct*_tmp19B=(struct Cyc_Absyn_ArrayType_struct*)
_tmp180;if(_tmp19B->tag != 9)goto _LL60;else{_tmp19C=_tmp19B->f1;_tmp19D=_tmp19C.elt_type;
_tmp19E=_tmp19C.tq;_tmp19F=_tmp19C.num_elts;_tmp1A0=_tmp19C.zt_loc;}}_LL5F:
return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp19D),_tmp19E,_tmp19F,Cyc_Absyn_false_conref,
_tmp1A0);_LL60: {struct Cyc_Absyn_FnType_struct*_tmp1A1=(struct Cyc_Absyn_FnType_struct*)
_tmp180;if(_tmp1A1->tag != 10)goto _LL62;else{_tmp1A2=_tmp1A1->f1;_tmp1A3=_tmp1A2.ret_tqual;
_tmp1A4=_tmp1A2.ret_typ;_tmp1A5=_tmp1A2.args;_tmp1A6=_tmp1A2.c_varargs;_tmp1A7=
_tmp1A2.cyc_varargs;_tmp1A8=_tmp1A2.attributes;}}_LL61: {struct Cyc_List_List*
_tmp1DE=0;for(0;_tmp1A8 != 0;_tmp1A8=_tmp1A8->tl){void*_tmp1DF=(void*)_tmp1A8->hd;
_LL8E: {struct Cyc_Absyn_Noreturn_att_struct*_tmp1E0=(struct Cyc_Absyn_Noreturn_att_struct*)
_tmp1DF;if(_tmp1E0->tag != 4)goto _LL90;}_LL8F: goto _LL91;_LL90: {struct Cyc_Absyn_Const_att_struct*
_tmp1E1=(struct Cyc_Absyn_Const_att_struct*)_tmp1DF;if(_tmp1E1->tag != 5)goto _LL92;}
_LL91: goto _LL93;_LL92: {struct Cyc_Absyn_Format_att_struct*_tmp1E2=(struct Cyc_Absyn_Format_att_struct*)
_tmp1DF;if(_tmp1E2->tag != 19)goto _LL94;}_LL93: continue;_LL94: {struct Cyc_Absyn_Noliveunique_att_struct*
_tmp1E3=(struct Cyc_Absyn_Noliveunique_att_struct*)_tmp1DF;if(_tmp1E3->tag != 21)
goto _LL96;}_LL95: continue;_LL96: {struct Cyc_Absyn_Initializes_att_struct*_tmp1E4=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp1DF;if(_tmp1E4->tag != 20)goto _LL98;}
_LL97: continue;_LL98:;_LL99:{struct Cyc_List_List*_tmpB38;_tmp1DE=((_tmpB38=
_cycalloc(sizeof(*_tmpB38)),((_tmpB38->hd=(void*)_tmp1A8->hd,((_tmpB38->tl=
_tmp1DE,_tmpB38))))));}goto _LL8D;_LL8D:;}{struct Cyc_List_List*_tmp1E6=((struct
Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_arg_to_c,_tmp1A5);if(_tmp1A7 != 0){void*_tmp1E7=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp1A7->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple7*_tmpB39;struct _tuple7*_tmp1E8=(_tmpB39=_cycalloc(sizeof(*_tmpB39)),((
_tmpB39->f1=_tmp1A7->name,((_tmpB39->f2=_tmp1A7->tq,((_tmpB39->f3=_tmp1E7,
_tmpB39)))))));struct Cyc_List_List*_tmpB3A;_tmp1E6=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1E6,((
_tmpB3A=_cycalloc(sizeof(*_tmpB3A)),((_tmpB3A->hd=_tmp1E8,((_tmpB3A->tl=0,
_tmpB3A)))))));}{struct Cyc_Absyn_FnType_struct _tmpB40;struct Cyc_Absyn_FnInfo
_tmpB3F;struct Cyc_Absyn_FnType_struct*_tmpB3E;return(void*)((_tmpB3E=_cycalloc(
sizeof(*_tmpB3E)),((_tmpB3E[0]=((_tmpB40.tag=10,((_tmpB40.f1=((_tmpB3F.tvars=0,((
_tmpB3F.effect=0,((_tmpB3F.ret_tqual=_tmp1A3,((_tmpB3F.ret_typ=Cyc_Toc_typ_to_c(
_tmp1A4),((_tmpB3F.args=_tmp1E6,((_tmpB3F.c_varargs=_tmp1A6,((_tmpB3F.cyc_varargs=
0,((_tmpB3F.rgn_po=0,((_tmpB3F.attributes=_tmp1DE,_tmpB3F)))))))))))))))))),
_tmpB40)))),_tmpB3E))));};};}_LL62: {struct Cyc_Absyn_TupleType_struct*_tmp1A9=(
struct Cyc_Absyn_TupleType_struct*)_tmp180;if(_tmp1A9->tag != 11)goto _LL64;else{
_tmp1AA=_tmp1A9->f1;}}_LL63: _tmp1AA=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(
struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1AA);
return Cyc_Toc_add_tuple_type(_tmp1AA);_LL64: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp1AB=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp180;if(_tmp1AB->tag != 13)goto
_LL66;else{_tmp1AC=_tmp1AB->f1;_tmp1AD=_tmp1AB->f2;}}_LL65: {struct Cyc_Absyn_AnonAggrType_struct
_tmpB43;struct Cyc_Absyn_AnonAggrType_struct*_tmpB42;return(void*)((_tmpB42=
_cycalloc(sizeof(*_tmpB42)),((_tmpB42[0]=((_tmpB43.tag=13,((_tmpB43.f1=_tmp1AC,((
_tmpB43.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1AD),_tmpB43)))))),
_tmpB42))));}_LL66: {struct Cyc_Absyn_AggrType_struct*_tmp1AE=(struct Cyc_Absyn_AggrType_struct*)
_tmp180;if(_tmp1AE->tag != 12)goto _LL68;else{_tmp1AF=_tmp1AE->f1;_tmp1B0=_tmp1AF.aggr_info;
_tmp1B1=_tmp1AF.targs;}}_LL67:{union Cyc_Absyn_AggrInfoU _tmp1F0=_tmp1B0;struct
_tuple2 _tmp1F1;_LL9B: if((_tmp1F0.UnknownAggr).tag != 1)goto _LL9D;_tmp1F1=(struct
_tuple2)(_tmp1F0.UnknownAggr).val;_LL9C: return t;_LL9D:;_LL9E: goto _LL9A;_LL9A:;}{
struct Cyc_Absyn_Aggrdecl*_tmp1F2=Cyc_Absyn_get_known_aggrdecl(_tmp1B0);if(
_tmp1F2->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(_tmp1F2->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp1F3=_tmp1F2->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1F2->impl))->fields;if(_tmp1F3 == 0)return Cyc_Toc_aggrdecl_type(
_tmp1F2->name,Cyc_Absyn_strctq);for(0;_tmp1F3->tl != 0;_tmp1F3=_tmp1F3->tl){;}{
void*_tmp1F4=((struct Cyc_Absyn_Aggrfield*)_tmp1F3->hd)->type;if(Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp1F4))){struct _RegionHandle _tmp1F5=
_new_region("r");struct _RegionHandle*r=& _tmp1F5;_push_region(r);{void*_tmp1F6=
Cyc_Tcutil_rsubstitute(r,((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,
_tmp1F2->tvs,_tmp1B1),_tmp1F4);if(Cyc_Toc_is_abs_tvar(_tmp1F6)){void*_tmp1F7=Cyc_Toc_aggrdecl_type(
_tmp1F2->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp1F7;}{void*_tmp1F8=
Cyc_Toc_add_struct_type(_tmp1F2->name,_tmp1F2->tvs,_tmp1B1,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1F2->impl))->fields);_npop_handler(0);return _tmp1F8;};};
_pop_region(r);}return Cyc_Toc_aggrdecl_type(_tmp1F2->name,Cyc_Absyn_strctq);};};};
_LL68: {struct Cyc_Absyn_EnumType_struct*_tmp1B2=(struct Cyc_Absyn_EnumType_struct*)
_tmp180;if(_tmp1B2->tag != 14)goto _LL6A;else{_tmp1B3=_tmp1B2->f1;}}_LL69: return t;
_LL6A: {struct Cyc_Absyn_AnonEnumType_struct*_tmp1B4=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp180;if(_tmp1B4->tag != 15)goto _LL6C;else{_tmp1B5=_tmp1B4->f1;}}_LL6B: Cyc_Toc_enumfields_to_c(
_tmp1B5);return t;_LL6C: {struct Cyc_Absyn_TypedefType_struct*_tmp1B6=(struct Cyc_Absyn_TypedefType_struct*)
_tmp180;if(_tmp1B6->tag != 18)goto _LL6E;else{_tmp1B7=_tmp1B6->f1;_tmp1B8=_tmp1B6->f2;
_tmp1B9=_tmp1B6->f3;_tmp1BA=_tmp1B6->f4;}}_LL6D: if(_tmp1BA == 0  || Cyc_noexpand_r){
if(_tmp1B8 != 0){struct Cyc_Absyn_TypedefType_struct _tmpB46;struct Cyc_Absyn_TypedefType_struct*
_tmpB45;return(void*)((_tmpB45=_cycalloc(sizeof(*_tmpB45)),((_tmpB45[0]=((
_tmpB46.tag=18,((_tmpB46.f1=_tmp1B7,((_tmpB46.f2=0,((_tmpB46.f3=_tmp1B9,((
_tmpB46.f4=0,_tmpB46)))))))))),_tmpB45))));}else{return t;}}else{struct Cyc_Absyn_TypedefType_struct
_tmpB4C;void**_tmpB4B;struct Cyc_Absyn_TypedefType_struct*_tmpB4A;return(void*)((
_tmpB4A=_cycalloc(sizeof(*_tmpB4A)),((_tmpB4A[0]=((_tmpB4C.tag=18,((_tmpB4C.f1=
_tmp1B7,((_tmpB4C.f2=0,((_tmpB4C.f3=_tmp1B9,((_tmpB4C.f4=((_tmpB4B=_cycalloc(
sizeof(*_tmpB4B)),((_tmpB4B[0]=Cyc_Toc_typ_to_c_array(*_tmp1BA),_tmpB4B)))),
_tmpB4C)))))))))),_tmpB4A))));}_LL6E: {struct Cyc_Absyn_TagType_struct*_tmp1BB=(
struct Cyc_Absyn_TagType_struct*)_tmp180;if(_tmp1BB->tag != 20)goto _LL70;}_LL6F:
return Cyc_Absyn_uint_typ;_LL70: {struct Cyc_Absyn_RgnHandleType_struct*_tmp1BC=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp180;if(_tmp1BC->tag != 16)goto _LL72;
else{_tmp1BD=(void*)_tmp1BC->f1;}}_LL71: return Cyc_Toc_rgn_typ();_LL72: {struct
Cyc_Absyn_DynRgnType_struct*_tmp1BE=(struct Cyc_Absyn_DynRgnType_struct*)_tmp180;
if(_tmp1BE->tag != 17)goto _LL74;}_LL73: return Cyc_Toc_dyn_rgn_typ();_LL74: {struct
Cyc_Absyn_HeapRgn_struct*_tmp1BF=(struct Cyc_Absyn_HeapRgn_struct*)_tmp180;if(
_tmp1BF->tag != 21)goto _LL76;}_LL75: {const char*_tmpB4F;void*_tmpB4E;(_tmpB4E=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB4F="Toc::typ_to_c: type translation passed the heap region",_tag_dyneither(
_tmpB4F,sizeof(char),55))),_tag_dyneither(_tmpB4E,sizeof(void*),0)));}_LL76: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp1C0=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp180;if(_tmp1C0->tag != 22)goto _LL78;}_LL77: {const char*_tmpB52;void*_tmpB51;(
_tmpB51=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB52="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpB52,sizeof(char),57))),_tag_dyneither(_tmpB51,sizeof(void*),0)));}
_LL78: {struct Cyc_Absyn_RefCntRgn_struct*_tmp1C1=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp180;if(_tmp1C1->tag != 23)goto _LL7A;}_LL79: {const char*_tmpB55;void*_tmpB54;(
_tmpB54=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB55="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpB55,sizeof(char),57))),_tag_dyneither(_tmpB54,sizeof(void*),0)));}
_LL7A: {struct Cyc_Absyn_AccessEff_struct*_tmp1C2=(struct Cyc_Absyn_AccessEff_struct*)
_tmp180;if(_tmp1C2->tag != 24)goto _LL7C;}_LL7B: goto _LL7D;_LL7C: {struct Cyc_Absyn_JoinEff_struct*
_tmp1C3=(struct Cyc_Absyn_JoinEff_struct*)_tmp180;if(_tmp1C3->tag != 25)goto _LL7E;}
_LL7D: goto _LL7F;_LL7E: {struct Cyc_Absyn_RgnsEff_struct*_tmp1C4=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp180;if(_tmp1C4->tag != 26)goto _LL80;}_LL7F: {const char*_tmpB58;void*_tmpB57;(
_tmpB57=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB58="Toc::typ_to_c: type translation passed an effect",_tag_dyneither(
_tmpB58,sizeof(char),49))),_tag_dyneither(_tmpB57,sizeof(void*),0)));}_LL80: {
struct Cyc_Absyn_ValueofType_struct*_tmp1C5=(struct Cyc_Absyn_ValueofType_struct*)
_tmp180;if(_tmp1C5->tag != 19)goto _LL82;}_LL81: {const char*_tmpB5B;void*_tmpB5A;(
_tmpB5A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB5B="Toc::typ_to_c: type translation passed a valueof_t",_tag_dyneither(
_tmpB5B,sizeof(char),51))),_tag_dyneither(_tmpB5A,sizeof(void*),0)));}_LL82: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp1C6=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp180;if(_tmp1C6->tag != 27)goto _LL84;else{_tmp1C7=_tmp1C6->f1;_tmp1C8=*_tmp1C7;
_tmp1C9=_tmp1C8.r;{struct Cyc_Absyn_Aggr_td_struct*_tmp1CA=(struct Cyc_Absyn_Aggr_td_struct*)
_tmp1C9;if(_tmp1CA->tag != 0)goto _LL84;else{_tmp1CB=_tmp1CA->f1;}};}}_LL83: Cyc_Toc_aggrdecl_to_c(
_tmp1CB,1);if(_tmp1CB->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(
_tmp1CB->name,Cyc_Absyn_unionq_typ);else{return Cyc_Toc_aggrdecl_type(_tmp1CB->name,
Cyc_Absyn_strctq);}_LL84: {struct Cyc_Absyn_TypeDeclType_struct*_tmp1CC=(struct
Cyc_Absyn_TypeDeclType_struct*)_tmp180;if(_tmp1CC->tag != 27)goto _LL86;else{
_tmp1CD=_tmp1CC->f1;_tmp1CE=*_tmp1CD;_tmp1CF=_tmp1CE.r;{struct Cyc_Absyn_Enum_td_struct*
_tmp1D0=(struct Cyc_Absyn_Enum_td_struct*)_tmp1CF;if(_tmp1D0->tag != 1)goto _LL86;
else{_tmp1D1=_tmp1D0->f1;}};}}_LL85: Cyc_Toc_enumdecl_to_c(_tmp1D1);return t;_LL86: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp1D2=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp180;if(_tmp1D2->tag != 27)goto _LL47;else{_tmp1D3=_tmp1D2->f1;_tmp1D4=*_tmp1D3;
_tmp1D5=_tmp1D4.r;{struct Cyc_Absyn_Datatype_td_struct*_tmp1D6=(struct Cyc_Absyn_Datatype_td_struct*)
_tmp1D5;if(_tmp1D6->tag != 2)goto _LL47;else{_tmp1D7=_tmp1D6->f1;}};_tmp1D8=
_tmp1D2->f2;}}_LL87: Cyc_Toc_datatypedecl_to_c(_tmp1D7);return Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp1D8)));_LL47:;}static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(
void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmp208=t;struct
Cyc_Absyn_ArrayInfo _tmp20A;void*_tmp20B;struct Cyc_Absyn_Tqual _tmp20C;_LLA0: {
struct Cyc_Absyn_ArrayType_struct*_tmp209=(struct Cyc_Absyn_ArrayType_struct*)
_tmp208;if(_tmp209->tag != 9)goto _LLA2;else{_tmp20A=_tmp209->f1;_tmp20B=_tmp20A.elt_type;
_tmp20C=_tmp20A.tq;}}_LLA1: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp20B,(
void*)& Cyc_Absyn_HeapRgn_val,_tmp20C,Cyc_Absyn_false_conref),e);_LLA2:;_LLA3:
return Cyc_Toc_cast_it(t,e);_LL9F:;}static int Cyc_Toc_atomic_typ(void*t){void*
_tmp20D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp218;void*_tmp219;
struct Cyc_Absyn_AggrInfo _tmp21B;union Cyc_Absyn_AggrInfoU _tmp21C;struct Cyc_List_List*
_tmp21E;struct Cyc_Absyn_DatatypeFieldInfo _tmp220;union Cyc_Absyn_DatatypeFieldInfoU
_tmp221;struct _tuple1 _tmp222;struct Cyc_Absyn_Datatypedecl*_tmp223;struct Cyc_Absyn_Datatypefield*
_tmp224;struct Cyc_List_List*_tmp226;_LLA5: {struct Cyc_Absyn_VoidType_struct*
_tmp20E=(struct Cyc_Absyn_VoidType_struct*)_tmp20D;if(_tmp20E->tag != 0)goto _LLA7;}
_LLA6: return 1;_LLA7: {struct Cyc_Absyn_VarType_struct*_tmp20F=(struct Cyc_Absyn_VarType_struct*)
_tmp20D;if(_tmp20F->tag != 2)goto _LLA9;}_LLA8: return 0;_LLA9: {struct Cyc_Absyn_IntType_struct*
_tmp210=(struct Cyc_Absyn_IntType_struct*)_tmp20D;if(_tmp210->tag != 6)goto _LLAB;}
_LLAA: goto _LLAC;_LLAB: {struct Cyc_Absyn_EnumType_struct*_tmp211=(struct Cyc_Absyn_EnumType_struct*)
_tmp20D;if(_tmp211->tag != 14)goto _LLAD;}_LLAC: goto _LLAE;_LLAD: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp212=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp20D;if(_tmp212->tag != 15)goto
_LLAF;}_LLAE: goto _LLB0;_LLAF: {struct Cyc_Absyn_FloatType_struct*_tmp213=(struct
Cyc_Absyn_FloatType_struct*)_tmp20D;if(_tmp213->tag != 7)goto _LLB1;}_LLB0: goto
_LLB2;_LLB1: {struct Cyc_Absyn_DoubleType_struct*_tmp214=(struct Cyc_Absyn_DoubleType_struct*)
_tmp20D;if(_tmp214->tag != 8)goto _LLB3;}_LLB2: goto _LLB4;_LLB3: {struct Cyc_Absyn_FnType_struct*
_tmp215=(struct Cyc_Absyn_FnType_struct*)_tmp20D;if(_tmp215->tag != 10)goto _LLB5;}
_LLB4: goto _LLB6;_LLB5: {struct Cyc_Absyn_TagType_struct*_tmp216=(struct Cyc_Absyn_TagType_struct*)
_tmp20D;if(_tmp216->tag != 20)goto _LLB7;}_LLB6: return 1;_LLB7: {struct Cyc_Absyn_ArrayType_struct*
_tmp217=(struct Cyc_Absyn_ArrayType_struct*)_tmp20D;if(_tmp217->tag != 9)goto _LLB9;
else{_tmp218=_tmp217->f1;_tmp219=_tmp218.elt_type;}}_LLB8: return Cyc_Toc_atomic_typ(
_tmp219);_LLB9: {struct Cyc_Absyn_AggrType_struct*_tmp21A=(struct Cyc_Absyn_AggrType_struct*)
_tmp20D;if(_tmp21A->tag != 12)goto _LLBB;else{_tmp21B=_tmp21A->f1;_tmp21C=_tmp21B.aggr_info;}}
_LLBA:{union Cyc_Absyn_AggrInfoU _tmp22B=_tmp21C;struct _tuple2 _tmp22C;_LLCC: if((
_tmp22B.UnknownAggr).tag != 1)goto _LLCE;_tmp22C=(struct _tuple2)(_tmp22B.UnknownAggr).val;
_LLCD: return 0;_LLCE:;_LLCF: goto _LLCB;_LLCB:;}{struct Cyc_Absyn_Aggrdecl*_tmp22D=
Cyc_Absyn_get_known_aggrdecl(_tmp21C);if(_tmp22D->impl == 0)return 0;{struct Cyc_List_List*
_tmp22E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp22D->impl))->fields;for(
0;_tmp22E != 0;_tmp22E=_tmp22E->tl){if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)
_tmp22E->hd)->type))return 0;}}return 1;};_LLBB: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp21D=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp20D;if(_tmp21D->tag != 13)goto
_LLBD;else{_tmp21E=_tmp21D->f2;}}_LLBC: for(0;_tmp21E != 0;_tmp21E=_tmp21E->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp21E->hd)->type))return 0;}
return 1;_LLBD: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp21F=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp20D;if(_tmp21F->tag != 4)goto _LLBF;else{_tmp220=_tmp21F->f1;_tmp221=_tmp220.field_info;
if((_tmp221.KnownDatatypefield).tag != 2)goto _LLBF;_tmp222=(struct _tuple1)(
_tmp221.KnownDatatypefield).val;_tmp223=_tmp222.f1;_tmp224=_tmp222.f2;}}_LLBE:
_tmp226=_tmp224->typs;goto _LLC0;_LLBF: {struct Cyc_Absyn_TupleType_struct*_tmp225=(
struct Cyc_Absyn_TupleType_struct*)_tmp20D;if(_tmp225->tag != 11)goto _LLC1;else{
_tmp226=_tmp225->f1;}}_LLC0: for(0;_tmp226 != 0;_tmp226=_tmp226->tl){if(!Cyc_Toc_atomic_typ((*((
struct _tuple9*)_tmp226->hd)).f2))return 0;}return 1;_LLC1: {struct Cyc_Absyn_DatatypeType_struct*
_tmp227=(struct Cyc_Absyn_DatatypeType_struct*)_tmp20D;if(_tmp227->tag != 3)goto
_LLC3;}_LLC2: goto _LLC4;_LLC3: {struct Cyc_Absyn_PointerType_struct*_tmp228=(
struct Cyc_Absyn_PointerType_struct*)_tmp20D;if(_tmp228->tag != 5)goto _LLC5;}_LLC4:
goto _LLC6;_LLC5: {struct Cyc_Absyn_DynRgnType_struct*_tmp229=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp20D;if(_tmp229->tag != 17)goto _LLC7;}_LLC6: goto _LLC8;_LLC7: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp22A=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp20D;if(_tmp22A->tag != 16)goto
_LLC9;}_LLC8: return 0;_LLC9:;_LLCA: {const char*_tmpB5F;void*_tmpB5E[1];struct Cyc_String_pa_struct
_tmpB5D;(_tmpB5D.tag=0,((_tmpB5D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpB5E[0]=& _tmpB5D,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB5F="atomic_typ:  bad type %s",
_tag_dyneither(_tmpB5F,sizeof(char),25))),_tag_dyneither(_tmpB5E,sizeof(void*),1)))))));}
_LLA4:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp232=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp234;void*_tmp235;_LLD1: {struct Cyc_Absyn_PointerType_struct*
_tmp233=(struct Cyc_Absyn_PointerType_struct*)_tmp232;if(_tmp233->tag != 5)goto
_LLD3;else{_tmp234=_tmp233->f1;_tmp235=_tmp234.elt_typ;}}_LLD2: {void*_tmp236=
Cyc_Tcutil_compress(_tmp235);_LLD6: {struct Cyc_Absyn_VoidType_struct*_tmp237=(
struct Cyc_Absyn_VoidType_struct*)_tmp236;if(_tmp237->tag != 0)goto _LLD8;}_LLD7:
return 1;_LLD8:;_LLD9: return 0;_LLD5:;}_LLD3:;_LLD4: return 0;_LLD0:;}static int Cyc_Toc_is_void_star_or_boxed_tvar(
void*t){return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}static int Cyc_Toc_is_poly_field(
void*t,struct _dyneither_ptr*f){void*_tmp238=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp23A;union Cyc_Absyn_AggrInfoU _tmp23B;struct Cyc_List_List*_tmp23D;_LLDB: {
struct Cyc_Absyn_AggrType_struct*_tmp239=(struct Cyc_Absyn_AggrType_struct*)
_tmp238;if(_tmp239->tag != 12)goto _LLDD;else{_tmp23A=_tmp239->f1;_tmp23B=_tmp23A.aggr_info;}}
_LLDC: {struct Cyc_Absyn_Aggrdecl*_tmp23E=Cyc_Absyn_get_known_aggrdecl(_tmp23B);
if(_tmp23E->impl == 0){const char*_tmpB62;void*_tmpB61;(_tmpB61=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB62="is_poly_field: type missing fields",
_tag_dyneither(_tmpB62,sizeof(char),35))),_tag_dyneither(_tmpB61,sizeof(void*),0)));}
_tmp23D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp23E->impl))->fields;goto
_LLDE;}_LLDD: {struct Cyc_Absyn_AnonAggrType_struct*_tmp23C=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp238;if(_tmp23C->tag != 13)goto _LLDF;else{_tmp23D=_tmp23C->f2;}}_LLDE: {struct
Cyc_Absyn_Aggrfield*_tmp241=Cyc_Absyn_lookup_field(_tmp23D,f);if(_tmp241 == 0){
const char*_tmpB66;void*_tmpB65[1];struct Cyc_String_pa_struct _tmpB64;(_tmpB64.tag=
0,((_tmpB64.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB65[0]=&
_tmpB64,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB66="is_poly_field: bad field %s",_tag_dyneither(_tmpB66,sizeof(char),28))),
_tag_dyneither(_tmpB65,sizeof(void*),1)))))));}return Cyc_Toc_is_void_star_or_boxed_tvar(
_tmp241->type);}_LLDF:;_LLE0: {const char*_tmpB6A;void*_tmpB69[1];struct Cyc_String_pa_struct
_tmpB68;(_tmpB68.tag=0,((_tmpB68.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpB69[0]=& _tmpB68,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6A="is_poly_field: bad type %s",
_tag_dyneither(_tmpB6A,sizeof(char),27))),_tag_dyneither(_tmpB69,sizeof(void*),1)))))));}
_LLDA:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmp248=e->r;
struct Cyc_Absyn_Exp*_tmp24A;struct _dyneither_ptr*_tmp24B;struct Cyc_Absyn_Exp*
_tmp24D;struct _dyneither_ptr*_tmp24E;_LLE2: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp249=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp248;if(_tmp249->tag != 22)goto
_LLE4;else{_tmp24A=_tmp249->f1;_tmp24B=_tmp249->f2;}}_LLE3: return Cyc_Toc_is_poly_field((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp24A->topt))->v,_tmp24B);_LLE4: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp24C=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp248;if(_tmp24C->tag != 23)goto _LLE6;else{_tmp24D=_tmp24C->f1;_tmp24E=_tmp24C->f2;}}
_LLE5: {void*_tmp24F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp24D->topt))->v);struct Cyc_Absyn_PtrInfo _tmp251;void*_tmp252;
_LLE9: {struct Cyc_Absyn_PointerType_struct*_tmp250=(struct Cyc_Absyn_PointerType_struct*)
_tmp24F;if(_tmp250->tag != 5)goto _LLEB;else{_tmp251=_tmp250->f1;_tmp252=_tmp251.elt_typ;}}
_LLEA: return Cyc_Toc_is_poly_field(_tmp252,_tmp24E);_LLEB:;_LLEC: {const char*
_tmpB6E;void*_tmpB6D[1];struct Cyc_String_pa_struct _tmpB6C;(_tmpB6C.tag=0,((
_tmpB6C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp24D->topt))->v)),((_tmpB6D[0]=&
_tmpB6C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB6E="is_poly_project: bad type %s",_tag_dyneither(_tmpB6E,sizeof(char),29))),
_tag_dyneither(_tmpB6D,sizeof(void*),1)))))));}_LLE8:;}_LLE6:;_LLE7: return 0;
_LLE1:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB6F;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((
_tmpB6F=_cycalloc(sizeof(*_tmpB6F)),((_tmpB6F->hd=s,((_tmpB6F->tl=0,_tmpB6F)))))),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB70;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((
_tmpB70=_cycalloc(sizeof(*_tmpB70)),((_tmpB70->hd=s,((_tmpB70->tl=0,_tmpB70)))))),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(
s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmpB71[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((
_tmpB71[1]=s,((_tmpB71[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpB71,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,
struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*
_tmpB72[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpB72[1]=n,((
_tmpB72[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB72,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{struct Cyc_Absyn_Exp*
_tmpB73[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpB73[1]=n,((
_tmpB73[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB73,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n){struct Cyc_Absyn_Exp*_tmpB74[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((
_tmpB74[2]=n,((_tmpB74[1]=s,((_tmpB74[0]=rgn,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB74,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmpB75;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmpB75=_cycalloc(sizeof(*_tmpB75)),((_tmpB75->hd=e,((_tmpB75->tl=0,_tmpB75)))))),
0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){struct Cyc_Absyn_Exp*
_tmpB76[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpB76,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*
_tmp25E=e->r;union Cyc_Absyn_Cnst _tmp260;struct _dyneither_ptr _tmp261;_LLEE: {
struct Cyc_Absyn_Const_e_struct*_tmp25F=(struct Cyc_Absyn_Const_e_struct*)_tmp25E;
if(_tmp25F->tag != 0)goto _LLF0;else{_tmp260=_tmp25F->f1;if((_tmp260.String_c).tag
!= 7)goto _LLF0;_tmp261=(struct _dyneither_ptr)(_tmp260.String_c).val;}}_LLEF:
is_string=1;goto _LLED;_LLF0:;_LLF1: goto _LLED;_LLED:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple0*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_struct*_tmpB7C;struct
Cyc_Absyn_Var_d_struct _tmpB7B;struct Cyc_List_List*_tmpB7A;Cyc_Toc_result_decls=((
_tmpB7A=_cycalloc(sizeof(*_tmpB7A)),((_tmpB7A->hd=Cyc_Absyn_new_decl((void*)((
_tmpB7C=_cycalloc(sizeof(*_tmpB7C)),((_tmpB7C[0]=((_tmpB7B.tag=0,((_tmpB7B.f1=vd,
_tmpB7B)))),_tmpB7C)))),0),((_tmpB7A->tl=Cyc_Toc_result_decls,_tmpB7A))))));}
xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*urm_exp;{struct _tuple15*_tmpB83;
struct _tuple15*_tmpB82;struct _tuple15*_tmpB81;struct _tuple15*_tmpB80[3];urm_exp=
Cyc_Absyn_unresolvedmem_exp(0,((_tmpB80[2]=((_tmpB81=_cycalloc(sizeof(*_tmpB81)),((
_tmpB81->f1=0,((_tmpB81->f2=xplussz,_tmpB81)))))),((_tmpB80[1]=((_tmpB82=
_cycalloc(sizeof(*_tmpB82)),((_tmpB82->f1=0,((_tmpB82->f2=xexp,_tmpB82)))))),((
_tmpB80[0]=((_tmpB83=_cycalloc(sizeof(*_tmpB83)),((_tmpB83->f1=0,((_tmpB83->f2=
xexp,_tmpB83)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB80,sizeof(struct _tuple15*),3)))))))),0);}return urm_exp;};};}
struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*
binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct
_dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct
_RegionHandle*rgn;};static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env
_tmp26A;int _tmp26B;struct Cyc_Toc_Env*_tmp269=nv;_tmp26A=*_tmp269;_tmp26B=_tmp26A.toplevel;
return _tmp26B;}static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env
_tmp26D;int*_tmp26E;struct Cyc_Toc_Env*_tmp26C=nv;_tmp26D=*_tmp26C;_tmp26E=
_tmp26D.in_lhs;return*_tmp26E;}static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(
struct Cyc_Toc_Env*nv,struct _tuple0*x){struct Cyc_Toc_Env _tmp270;struct Cyc_Dict_Dict
_tmp271;struct Cyc_Toc_Env*_tmp26F=nv;_tmp270=*_tmp26F;_tmp271=_tmp270.varmap;
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(
_tmp271,x);}static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){int*
_tmpB86;struct Cyc_Toc_Env*_tmpB85;return(_tmpB85=_region_malloc(r,sizeof(*
_tmpB85)),((_tmpB85->break_lab=(struct _dyneither_ptr**)0,((_tmpB85->continue_lab=(
struct _dyneither_ptr**)0,((_tmpB85->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((
_tmpB85->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((
_tmpB85->toplevel=(int)1,((_tmpB85->in_lhs=(int*)((_tmpB86=_region_malloc(r,
sizeof(*_tmpB86)),((_tmpB86[0]=0,_tmpB86)))),((_tmpB85->rgn=(struct _RegionHandle*)
r,_tmpB85)))))))))))))));}static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp275;struct
_dyneither_ptr**_tmp276;struct _dyneither_ptr**_tmp277;struct Cyc_Toc_FallthruInfo*
_tmp278;struct Cyc_Dict_Dict _tmp279;int _tmp27A;int*_tmp27B;struct Cyc_Toc_Env*
_tmp274=e;_tmp275=*_tmp274;_tmp276=_tmp275.break_lab;_tmp277=_tmp275.continue_lab;
_tmp278=_tmp275.fallthru_info;_tmp279=_tmp275.varmap;_tmp27A=_tmp275.toplevel;
_tmp27B=_tmp275.in_lhs;{struct Cyc_Toc_Env*_tmpB87;return(_tmpB87=_region_malloc(
r,sizeof(*_tmpB87)),((_tmpB87->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp276),((_tmpB87->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp277),((_tmpB87->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp278,((_tmpB87->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp279),((_tmpB87->toplevel=(
int)_tmp27A,((_tmpB87->in_lhs=(int*)_tmp27B,((_tmpB87->rgn=(struct _RegionHandle*)
r,_tmpB87)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp27E;struct
_dyneither_ptr**_tmp27F;struct _dyneither_ptr**_tmp280;struct Cyc_Toc_FallthruInfo*
_tmp281;struct Cyc_Dict_Dict _tmp282;int _tmp283;int*_tmp284;struct Cyc_Toc_Env*
_tmp27D=e;_tmp27E=*_tmp27D;_tmp27F=_tmp27E.break_lab;_tmp280=_tmp27E.continue_lab;
_tmp281=_tmp27E.fallthru_info;_tmp282=_tmp27E.varmap;_tmp283=_tmp27E.toplevel;
_tmp284=_tmp27E.in_lhs;{struct Cyc_Toc_Env*_tmpB88;return(_tmpB88=_region_malloc(
r,sizeof(*_tmpB88)),((_tmpB88->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp27F),((_tmpB88->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp280),((_tmpB88->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp281,((_tmpB88->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp282),((_tmpB88->toplevel=(
int)0,((_tmpB88->in_lhs=(int*)_tmp284,((_tmpB88->rgn=(struct _RegionHandle*)r,
_tmpB88)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct
_RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp287;struct
_dyneither_ptr**_tmp288;struct _dyneither_ptr**_tmp289;struct Cyc_Toc_FallthruInfo*
_tmp28A;struct Cyc_Dict_Dict _tmp28B;int _tmp28C;int*_tmp28D;struct Cyc_Toc_Env*
_tmp286=e;_tmp287=*_tmp286;_tmp288=_tmp287.break_lab;_tmp289=_tmp287.continue_lab;
_tmp28A=_tmp287.fallthru_info;_tmp28B=_tmp287.varmap;_tmp28C=_tmp287.toplevel;
_tmp28D=_tmp287.in_lhs;{struct Cyc_Toc_Env*_tmpB89;return(_tmpB89=_region_malloc(
r,sizeof(*_tmpB89)),((_tmpB89->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp288),((_tmpB89->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp289),((_tmpB89->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp28A,((_tmpB89->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp28B),((_tmpB89->toplevel=(
int)1,((_tmpB89->in_lhs=(int*)_tmp28D,((_tmpB89->rgn=(struct _RegionHandle*)r,
_tmpB89)))))))))))))));};}static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env _tmp290;int*_tmp291;struct Cyc_Toc_Env*_tmp28F=e;_tmp290=*
_tmp28F;_tmp291=_tmp290.in_lhs;*_tmp291=b;}static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace _tmp292=(*x).f1;struct Cyc_List_List*_tmp293;_LLF3: if((
_tmp292.Rel_n).tag != 1)goto _LLF5;_tmp293=(struct Cyc_List_List*)(_tmp292.Rel_n).val;
_LLF4: {const char*_tmpB8D;void*_tmpB8C[1];struct Cyc_String_pa_struct _tmpB8B;(
_tmpB8B.tag=0,((_tmpB8B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
x)),((_tmpB8C[0]=& _tmpB8B,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB8D="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(
_tmpB8D,sizeof(char),30))),_tag_dyneither(_tmpB8C,sizeof(void*),1)))))));}_LLF5:;
_LLF6: goto _LLF2;_LLF2:;}{struct Cyc_Toc_Env _tmp298;struct _dyneither_ptr**_tmp299;
struct _dyneither_ptr**_tmp29A;struct Cyc_Toc_FallthruInfo*_tmp29B;struct Cyc_Dict_Dict
_tmp29C;int _tmp29D;int*_tmp29E;struct Cyc_Toc_Env*_tmp297=e;_tmp298=*_tmp297;
_tmp299=_tmp298.break_lab;_tmp29A=_tmp298.continue_lab;_tmp29B=_tmp298.fallthru_info;
_tmp29C=_tmp298.varmap;_tmp29D=_tmp298.toplevel;_tmp29E=_tmp298.in_lhs;{struct
Cyc_Dict_Dict _tmp29F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*
k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29C),x,y);struct Cyc_Toc_Env*
_tmpB8E;return(_tmpB8E=_region_malloc(r,sizeof(*_tmpB8E)),((_tmpB8E->break_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp299),((_tmpB8E->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp29A),((_tmpB8E->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp29B,((_tmpB8E->varmap=(struct Cyc_Dict_Dict)
_tmp29F,((_tmpB8E->toplevel=(int)_tmp29D,((_tmpB8E->in_lhs=(int*)_tmp29E,((
_tmpB8E->rgn=(struct _RegionHandle*)r,_tmpB8E)))))))))))))));};};}static struct Cyc_Toc_Env*
Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp2A2;struct _dyneither_ptr**_tmp2A3;struct _dyneither_ptr**_tmp2A4;struct Cyc_Toc_FallthruInfo*
_tmp2A5;struct Cyc_Dict_Dict _tmp2A6;int _tmp2A7;int*_tmp2A8;struct Cyc_Toc_Env*
_tmp2A1=e;_tmp2A2=*_tmp2A1;_tmp2A3=_tmp2A2.break_lab;_tmp2A4=_tmp2A2.continue_lab;
_tmp2A5=_tmp2A2.fallthru_info;_tmp2A6=_tmp2A2.varmap;_tmp2A7=_tmp2A2.toplevel;
_tmp2A8=_tmp2A2.in_lhs;{struct Cyc_Toc_Env*_tmpB8F;return(_tmpB8F=_region_malloc(
r,sizeof(*_tmpB8F)),((_tmpB8F->break_lab=(struct _dyneither_ptr**)0,((_tmpB8F->continue_lab=(
struct _dyneither_ptr**)0,((_tmpB8F->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp2A5,((_tmpB8F->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2A6),((_tmpB8F->toplevel=(
int)_tmp2A7,((_tmpB8F->in_lhs=(int*)_tmp2A8,((_tmpB8F->rgn=(struct _RegionHandle*)
r,_tmpB8F)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmpB90;fallthru_vars=((
_tmpB90=_region_malloc(r,sizeof(*_tmpB90)),((_tmpB90->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmpB90->tl=fallthru_vars,_tmpB90))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp2AC;struct _dyneither_ptr**_tmp2AD;struct
_dyneither_ptr**_tmp2AE;struct Cyc_Toc_FallthruInfo*_tmp2AF;struct Cyc_Dict_Dict
_tmp2B0;int _tmp2B1;int*_tmp2B2;struct Cyc_Toc_Env*_tmp2AB=e;_tmp2AC=*_tmp2AB;
_tmp2AD=_tmp2AC.break_lab;_tmp2AE=_tmp2AC.continue_lab;_tmp2AF=_tmp2AC.fallthru_info;
_tmp2B0=_tmp2AC.varmap;_tmp2B1=_tmp2AC.toplevel;_tmp2B2=_tmp2AC.in_lhs;{struct
Cyc_Toc_Env _tmp2B4;struct Cyc_Dict_Dict _tmp2B5;struct Cyc_Toc_Env*_tmp2B3=
next_case_env;_tmp2B4=*_tmp2B3;_tmp2B5=_tmp2B4.varmap;{struct Cyc_Toc_FallthruInfo*
_tmpB91;struct Cyc_Toc_FallthruInfo*fi=(_tmpB91=_region_malloc(r,sizeof(*_tmpB91)),((
_tmpB91->label=fallthru_l,((_tmpB91->binders=fallthru_vars,((_tmpB91->next_case_env=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp2B5),_tmpB91)))))));struct _dyneither_ptr**_tmpB94;struct Cyc_Toc_Env*
_tmpB93;return(_tmpB93=_region_malloc(r,sizeof(*_tmpB93)),((_tmpB93->break_lab=(
struct _dyneither_ptr**)((_tmpB94=_region_malloc(r,sizeof(*_tmpB94)),((_tmpB94[0]=
break_l,_tmpB94)))),((_tmpB93->continue_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp2AE),((_tmpB93->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
fi,((_tmpB93->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2B0),((_tmpB93->toplevel=(
int)_tmp2B1,((_tmpB93->in_lhs=(int*)((int*)_tmp2B2),((_tmpB93->rgn=(struct
_RegionHandle*)r,_tmpB93)))))))))))))));};};};}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){struct
Cyc_Toc_Env _tmp2BA;struct _dyneither_ptr**_tmp2BB;struct _dyneither_ptr**_tmp2BC;
struct Cyc_Toc_FallthruInfo*_tmp2BD;struct Cyc_Dict_Dict _tmp2BE;int _tmp2BF;int*
_tmp2C0;struct Cyc_Toc_Env*_tmp2B9=e;_tmp2BA=*_tmp2B9;_tmp2BB=_tmp2BA.break_lab;
_tmp2BC=_tmp2BA.continue_lab;_tmp2BD=_tmp2BA.fallthru_info;_tmp2BE=_tmp2BA.varmap;
_tmp2BF=_tmp2BA.toplevel;_tmp2C0=_tmp2BA.in_lhs;{struct _dyneither_ptr**_tmpB97;
struct Cyc_Toc_Env*_tmpB96;return(_tmpB96=_region_malloc(r,sizeof(*_tmpB96)),((
_tmpB96->break_lab=(struct _dyneither_ptr**)((_tmpB97=_region_malloc(r,sizeof(*
_tmpB97)),((_tmpB97[0]=break_l,_tmpB97)))),((_tmpB96->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp2BC),((_tmpB96->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpB96->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2BE),((_tmpB96->toplevel=(
int)_tmp2BF,((_tmpB96->in_lhs=(int*)_tmp2C0,((_tmpB96->rgn=(struct _RegionHandle*)
r,_tmpB96)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp2C4;struct _dyneither_ptr**_tmp2C5;struct _dyneither_ptr**_tmp2C6;struct Cyc_Toc_FallthruInfo*
_tmp2C7;struct Cyc_Dict_Dict _tmp2C8;int _tmp2C9;int*_tmp2CA;struct Cyc_Toc_Env*
_tmp2C3=e;_tmp2C4=*_tmp2C3;_tmp2C5=_tmp2C4.break_lab;_tmp2C6=_tmp2C4.continue_lab;
_tmp2C7=_tmp2C4.fallthru_info;_tmp2C8=_tmp2C4.varmap;_tmp2C9=_tmp2C4.toplevel;
_tmp2CA=_tmp2C4.in_lhs;{struct Cyc_Toc_FallthruInfo*_tmpB9A;struct Cyc_Toc_Env*
_tmpB99;return(_tmpB99=_region_malloc(r,sizeof(*_tmpB99)),((_tmpB99->break_lab=(
struct _dyneither_ptr**)0,((_tmpB99->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp2C6),((_tmpB99->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((
_tmpB9A=_region_malloc(r,sizeof(*_tmpB9A)),((_tmpB9A->label=next_l,((_tmpB9A->binders=
0,((_tmpB9A->next_case_env=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*
cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),
_tmpB9A)))))))),((_tmpB99->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2C8),((_tmpB99->toplevel=(
int)_tmp2C9,((_tmpB99->in_lhs=(int*)((int*)_tmp2CA),((_tmpB99->rgn=(struct
_RegionHandle*)r,_tmpB99)))))))))))))));};}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp2CD=e->annot;_LLF8: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2CE=(
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp2CD;if(_tmp2CE->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLFA;}_LLF9: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLFA: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp2CF=(
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp2CD;if(_tmp2CF->tag != Cyc_CfFlowInfo_NotZero)
goto _LLFC;}_LLFB: return 0;_LLFC: {struct Cyc_CfFlowInfo_IsZero_struct*_tmp2D0=(
struct Cyc_CfFlowInfo_IsZero_struct*)_tmp2CD;if(_tmp2D0->tag != Cyc_CfFlowInfo_IsZero)
goto _LLFE;}_LLFD:{const char*_tmpB9D;void*_tmpB9C;(_tmpB9C=0,Cyc_Tcutil_terr(e->loc,((
_tmpB9D="dereference of NULL pointer",_tag_dyneither(_tmpB9D,sizeof(char),28))),
_tag_dyneither(_tmpB9C,sizeof(void*),0)));}return 0;_LLFE: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp2D1=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp2CD;if(_tmp2D1->tag != Cyc_Absyn_EmptyAnnot)
goto _LL100;}_LLFF: return 0;_LL100:;_LL101: {const char*_tmpBA0;void*_tmpB9F;(
_tmpB9F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBA0="need_null_check",_tag_dyneither(_tmpBA0,sizeof(char),16))),
_tag_dyneither(_tmpB9F,sizeof(void*),0)));}_LLF7:;}static struct Cyc_List_List*Cyc_Toc_get_relns(
struct Cyc_Absyn_Exp*e){void*_tmp2D6=e->annot;struct Cyc_List_List*_tmp2D8;struct
Cyc_List_List*_tmp2DA;_LL103: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2D7=(
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp2D6;if(_tmp2D7->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LL105;else{_tmp2D8=_tmp2D7->f1;}}_LL104: return _tmp2D8;_LL105: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmp2D9=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmp2D6;if(_tmp2D9->tag != Cyc_CfFlowInfo_NotZero)
goto _LL107;else{_tmp2DA=_tmp2D9->f1;}}_LL106: return _tmp2DA;_LL107: {struct Cyc_CfFlowInfo_IsZero_struct*
_tmp2DB=(struct Cyc_CfFlowInfo_IsZero_struct*)_tmp2D6;if(_tmp2DB->tag != Cyc_CfFlowInfo_IsZero)
goto _LL109;}_LL108:{const char*_tmpBA3;void*_tmpBA2;(_tmpBA2=0,Cyc_Tcutil_terr(e->loc,((
_tmpBA3="dereference of NULL pointer",_tag_dyneither(_tmpBA3,sizeof(char),28))),
_tag_dyneither(_tmpBA2,sizeof(void*),0)));}return 0;_LL109: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp2DC=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp2D6;if(_tmp2DC->tag != Cyc_Absyn_EmptyAnnot)
goto _LL10B;}_LL10A: return 0;_LL10B:;_LL10C: {const char*_tmpBA6;void*_tmpBA5;(
_tmpBA5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBA6="get_relns",_tag_dyneither(_tmpBA6,sizeof(char),10))),_tag_dyneither(
_tmpBA5,sizeof(void*),0)));}_LL102:;}static int Cyc_Toc_check_const_array(
unsigned int i,void*t){void*_tmp2E1=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp2E3;struct Cyc_Absyn_PtrAtts _tmp2E4;union Cyc_Absyn_Constraint*_tmp2E5;union
Cyc_Absyn_Constraint*_tmp2E6;struct Cyc_Absyn_ArrayInfo _tmp2E8;struct Cyc_Absyn_Exp*
_tmp2E9;_LL10E: {struct Cyc_Absyn_PointerType_struct*_tmp2E2=(struct Cyc_Absyn_PointerType_struct*)
_tmp2E1;if(_tmp2E2->tag != 5)goto _LL110;else{_tmp2E3=_tmp2E2->f1;_tmp2E4=_tmp2E3.ptr_atts;
_tmp2E5=_tmp2E4.bounds;_tmp2E6=_tmp2E4.zero_term;}}_LL10F: {void*_tmp2EA=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp2E5);struct Cyc_Absyn_Exp*_tmp2ED;_LL115: {struct Cyc_Absyn_DynEither_b_struct*
_tmp2EB=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2EA;if(_tmp2EB->tag != 0)goto
_LL117;}_LL116: return 0;_LL117: {struct Cyc_Absyn_Upper_b_struct*_tmp2EC=(struct
Cyc_Absyn_Upper_b_struct*)_tmp2EA;if(_tmp2EC->tag != 1)goto _LL114;else{_tmp2ED=
_tmp2EC->f1;}}_LL118: {unsigned int _tmp2EF;int _tmp2F0;struct _tuple10 _tmp2EE=Cyc_Evexp_eval_const_uint_exp(
_tmp2ED);_tmp2EF=_tmp2EE.f1;_tmp2F0=_tmp2EE.f2;return _tmp2F0  && i <= _tmp2EF;}
_LL114:;}_LL110: {struct Cyc_Absyn_ArrayType_struct*_tmp2E7=(struct Cyc_Absyn_ArrayType_struct*)
_tmp2E1;if(_tmp2E7->tag != 9)goto _LL112;else{_tmp2E8=_tmp2E7->f1;_tmp2E9=_tmp2E8.num_elts;}}
_LL111: if(_tmp2E9 == 0)return 0;{unsigned int _tmp2F2;int _tmp2F3;struct _tuple10
_tmp2F1=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp2E9);_tmp2F2=
_tmp2F1.f1;_tmp2F3=_tmp2F1.f2;return _tmp2F3  && i <= _tmp2F2;};_LL112:;_LL113:
return 0;_LL10D:;}static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2F4=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(
_tmp2F4->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp _tmp2F5=_tmp2F4->rop;struct
Cyc_Absyn_Vardecl*_tmp2F6;struct Cyc_Absyn_Vardecl*_tmp2F7;_LL11A: if((_tmp2F5.LessNumelts).tag
!= 3)goto _LL11C;_tmp2F6=(struct Cyc_Absyn_Vardecl*)(_tmp2F5.LessNumelts).val;
_LL11B: _tmp2F7=_tmp2F6;goto _LL11D;_LL11C: if((_tmp2F5.LessEqNumelts).tag != 5)goto
_LL11E;_tmp2F7=(struct Cyc_Absyn_Vardecl*)(_tmp2F5.LessEqNumelts).val;_LL11D: if(
_tmp2F7 == v)return 1;else{goto _LL119;}_LL11E:;_LL11F: continue;_LL119:;};}return 0;}
static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Exp*e,void*vtype){{void*_tmp2F8=e->r;void*_tmp2FA;struct Cyc_Absyn_Vardecl*
_tmp2FC;void*_tmp2FE;struct Cyc_Absyn_Vardecl*_tmp300;void*_tmp302;struct Cyc_Absyn_Vardecl*
_tmp304;void*_tmp306;struct Cyc_Absyn_Vardecl*_tmp308;union Cyc_Absyn_Cnst _tmp30A;
struct _tuple5 _tmp30B;enum Cyc_Absyn_Sign _tmp30C;int _tmp30D;union Cyc_Absyn_Cnst
_tmp30F;struct _tuple5 _tmp310;enum Cyc_Absyn_Sign _tmp311;int _tmp312;union Cyc_Absyn_Cnst
_tmp314;struct _tuple5 _tmp315;enum Cyc_Absyn_Sign _tmp316;int _tmp317;enum Cyc_Absyn_Primop
_tmp319;struct Cyc_List_List*_tmp31A;struct Cyc_List_List _tmp31B;struct Cyc_Absyn_Exp*
_tmp31C;_LL121: {struct Cyc_Absyn_Var_e_struct*_tmp2F9=(struct Cyc_Absyn_Var_e_struct*)
_tmp2F8;if(_tmp2F9->tag != 1)goto _LL123;else{_tmp2FA=(void*)_tmp2F9->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2FB=(struct Cyc_Absyn_Pat_b_struct*)_tmp2FA;if(_tmp2FB->tag
!= 5)goto _LL123;else{_tmp2FC=_tmp2FB->f1;}};}}_LL122: _tmp300=_tmp2FC;goto _LL124;
_LL123: {struct Cyc_Absyn_Var_e_struct*_tmp2FD=(struct Cyc_Absyn_Var_e_struct*)
_tmp2F8;if(_tmp2FD->tag != 1)goto _LL125;else{_tmp2FE=(void*)_tmp2FD->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2FF=(struct Cyc_Absyn_Local_b_struct*)_tmp2FE;if(
_tmp2FF->tag != 4)goto _LL125;else{_tmp300=_tmp2FF->f1;}};}}_LL124: _tmp304=_tmp300;
goto _LL126;_LL125: {struct Cyc_Absyn_Var_e_struct*_tmp301=(struct Cyc_Absyn_Var_e_struct*)
_tmp2F8;if(_tmp301->tag != 1)goto _LL127;else{_tmp302=(void*)_tmp301->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp303=(struct Cyc_Absyn_Global_b_struct*)_tmp302;if(
_tmp303->tag != 1)goto _LL127;else{_tmp304=_tmp303->f1;}};}}_LL126: _tmp308=_tmp304;
goto _LL128;_LL127: {struct Cyc_Absyn_Var_e_struct*_tmp305=(struct Cyc_Absyn_Var_e_struct*)
_tmp2F8;if(_tmp305->tag != 1)goto _LL129;else{_tmp306=(void*)_tmp305->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp307=(struct Cyc_Absyn_Param_b_struct*)_tmp306;if(
_tmp307->tag != 3)goto _LL129;else{_tmp308=_tmp307->f1;}};}}_LL128: if(_tmp308->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp308))return 1;goto _LL120;_LL129: {
struct Cyc_Absyn_Const_e_struct*_tmp309=(struct Cyc_Absyn_Const_e_struct*)_tmp2F8;
if(_tmp309->tag != 0)goto _LL12B;else{_tmp30A=_tmp309->f1;if((_tmp30A.Int_c).tag != 
4)goto _LL12B;_tmp30B=(struct _tuple5)(_tmp30A.Int_c).val;_tmp30C=_tmp30B.f1;if(
_tmp30C != Cyc_Absyn_None)goto _LL12B;_tmp30D=_tmp30B.f2;}}_LL12A: _tmp312=_tmp30D;
goto _LL12C;_LL12B: {struct Cyc_Absyn_Const_e_struct*_tmp30E=(struct Cyc_Absyn_Const_e_struct*)
_tmp2F8;if(_tmp30E->tag != 0)goto _LL12D;else{_tmp30F=_tmp30E->f1;if((_tmp30F.Int_c).tag
!= 4)goto _LL12D;_tmp310=(struct _tuple5)(_tmp30F.Int_c).val;_tmp311=_tmp310.f1;
if(_tmp311 != Cyc_Absyn_Signed)goto _LL12D;_tmp312=_tmp310.f2;}}_LL12C: return
_tmp312 >= 0  && Cyc_Toc_check_const_array((unsigned int)_tmp312,vtype);_LL12D: {
struct Cyc_Absyn_Const_e_struct*_tmp313=(struct Cyc_Absyn_Const_e_struct*)_tmp2F8;
if(_tmp313->tag != 0)goto _LL12F;else{_tmp314=_tmp313->f1;if((_tmp314.Int_c).tag != 
4)goto _LL12F;_tmp315=(struct _tuple5)(_tmp314.Int_c).val;_tmp316=_tmp315.f1;if(
_tmp316 != Cyc_Absyn_Unsigned)goto _LL12F;_tmp317=_tmp315.f2;}}_LL12E: return Cyc_Toc_check_const_array((
unsigned int)_tmp317,vtype);_LL12F: {struct Cyc_Absyn_Primop_e_struct*_tmp318=(
struct Cyc_Absyn_Primop_e_struct*)_tmp2F8;if(_tmp318->tag != 3)goto _LL131;else{
_tmp319=_tmp318->f1;if(_tmp319 != Cyc_Absyn_Numelts)goto _LL131;_tmp31A=_tmp318->f2;
if(_tmp31A == 0)goto _LL131;_tmp31B=*_tmp31A;_tmp31C=(struct Cyc_Absyn_Exp*)_tmp31B.hd;}}
_LL130:{void*_tmp31D=_tmp31C->r;void*_tmp31F;struct Cyc_Absyn_Vardecl*_tmp321;
void*_tmp323;struct Cyc_Absyn_Vardecl*_tmp325;void*_tmp327;struct Cyc_Absyn_Vardecl*
_tmp329;void*_tmp32B;struct Cyc_Absyn_Vardecl*_tmp32D;_LL134: {struct Cyc_Absyn_Var_e_struct*
_tmp31E=(struct Cyc_Absyn_Var_e_struct*)_tmp31D;if(_tmp31E->tag != 1)goto _LL136;
else{_tmp31F=(void*)_tmp31E->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp320=(struct Cyc_Absyn_Pat_b_struct*)
_tmp31F;if(_tmp320->tag != 5)goto _LL136;else{_tmp321=_tmp320->f1;}};}}_LL135:
_tmp325=_tmp321;goto _LL137;_LL136: {struct Cyc_Absyn_Var_e_struct*_tmp322=(struct
Cyc_Absyn_Var_e_struct*)_tmp31D;if(_tmp322->tag != 1)goto _LL138;else{_tmp323=(
void*)_tmp322->f2;{struct Cyc_Absyn_Local_b_struct*_tmp324=(struct Cyc_Absyn_Local_b_struct*)
_tmp323;if(_tmp324->tag != 4)goto _LL138;else{_tmp325=_tmp324->f1;}};}}_LL137:
_tmp329=_tmp325;goto _LL139;_LL138: {struct Cyc_Absyn_Var_e_struct*_tmp326=(struct
Cyc_Absyn_Var_e_struct*)_tmp31D;if(_tmp326->tag != 1)goto _LL13A;else{_tmp327=(
void*)_tmp326->f2;{struct Cyc_Absyn_Global_b_struct*_tmp328=(struct Cyc_Absyn_Global_b_struct*)
_tmp327;if(_tmp328->tag != 1)goto _LL13A;else{_tmp329=_tmp328->f1;}};}}_LL139:
_tmp32D=_tmp329;goto _LL13B;_LL13A: {struct Cyc_Absyn_Var_e_struct*_tmp32A=(struct
Cyc_Absyn_Var_e_struct*)_tmp31D;if(_tmp32A->tag != 1)goto _LL13C;else{_tmp32B=(
void*)_tmp32A->f2;{struct Cyc_Absyn_Param_b_struct*_tmp32C=(struct Cyc_Absyn_Param_b_struct*)
_tmp32B;if(_tmp32C->tag != 3)goto _LL13C;else{_tmp32D=_tmp32C->f1;}};}}_LL13B:
return _tmp32D == v;_LL13C:;_LL13D: goto _LL133;_LL133:;}goto _LL120;_LL131:;_LL132:
goto _LL120;_LL120:;}return 0;}static char _tmp33F[8]="*bogus*";struct _tuple18{void*
f1;void*f2;};static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,
struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){struct Cyc_Absyn_Vardecl*x;{void*
_tmp32E=a->r;void*_tmp330;struct Cyc_Absyn_Vardecl*_tmp332;void*_tmp334;struct Cyc_Absyn_Vardecl*
_tmp336;void*_tmp338;struct Cyc_Absyn_Vardecl*_tmp33A;void*_tmp33C;struct Cyc_Absyn_Vardecl*
_tmp33E;_LL13F: {struct Cyc_Absyn_Var_e_struct*_tmp32F=(struct Cyc_Absyn_Var_e_struct*)
_tmp32E;if(_tmp32F->tag != 1)goto _LL141;else{_tmp330=(void*)_tmp32F->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp331=(struct Cyc_Absyn_Pat_b_struct*)_tmp330;if(_tmp331->tag
!= 5)goto _LL141;else{_tmp332=_tmp331->f1;}};}}_LL140: _tmp336=_tmp332;goto _LL142;
_LL141: {struct Cyc_Absyn_Var_e_struct*_tmp333=(struct Cyc_Absyn_Var_e_struct*)
_tmp32E;if(_tmp333->tag != 1)goto _LL143;else{_tmp334=(void*)_tmp333->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp335=(struct Cyc_Absyn_Local_b_struct*)_tmp334;if(
_tmp335->tag != 4)goto _LL143;else{_tmp336=_tmp335->f1;}};}}_LL142: _tmp33A=_tmp336;
goto _LL144;_LL143: {struct Cyc_Absyn_Var_e_struct*_tmp337=(struct Cyc_Absyn_Var_e_struct*)
_tmp32E;if(_tmp337->tag != 1)goto _LL145;else{_tmp338=(void*)_tmp337->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp339=(struct Cyc_Absyn_Global_b_struct*)_tmp338;if(
_tmp339->tag != 1)goto _LL145;else{_tmp33A=_tmp339->f1;}};}}_LL144: _tmp33E=_tmp33A;
goto _LL146;_LL145: {struct Cyc_Absyn_Var_e_struct*_tmp33B=(struct Cyc_Absyn_Var_e_struct*)
_tmp32E;if(_tmp33B->tag != 1)goto _LL147;else{_tmp33C=(void*)_tmp33B->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp33D=(struct Cyc_Absyn_Param_b_struct*)_tmp33C;if(
_tmp33D->tag != 3)goto _LL147;else{_tmp33E=_tmp33D->f1;}};}}_LL146: if(_tmp33E->escapes)
goto _LL148;x=_tmp33E;goto _LL13E;_LL147:;_LL148: {static struct _dyneither_ptr
bogus_string={_tmp33F,_tmp33F,_tmp33F + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
4,0}},& bogus_string};static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,&
bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};x=& bogus_vardecl;
x->type=a_typ;}_LL13E:;}{void*_tmp340=a_typ;inner_loop: {void*_tmp341=i->r;void*
_tmp343;struct Cyc_Absyn_Exp*_tmp344;union Cyc_Absyn_Cnst _tmp346;struct _tuple5
_tmp347;enum Cyc_Absyn_Sign _tmp348;int _tmp349;union Cyc_Absyn_Cnst _tmp34B;struct
_tuple5 _tmp34C;enum Cyc_Absyn_Sign _tmp34D;int _tmp34E;union Cyc_Absyn_Cnst _tmp350;
struct _tuple5 _tmp351;enum Cyc_Absyn_Sign _tmp352;int _tmp353;enum Cyc_Absyn_Primop
_tmp355;struct Cyc_List_List*_tmp356;struct Cyc_List_List _tmp357;struct Cyc_Absyn_Exp*
_tmp358;struct Cyc_List_List*_tmp359;struct Cyc_List_List _tmp35A;struct Cyc_Absyn_Exp*
_tmp35B;void*_tmp35D;struct Cyc_Absyn_Vardecl*_tmp35F;void*_tmp361;struct Cyc_Absyn_Vardecl*
_tmp363;void*_tmp365;struct Cyc_Absyn_Vardecl*_tmp367;void*_tmp369;struct Cyc_Absyn_Vardecl*
_tmp36B;_LL14A: {struct Cyc_Absyn_Cast_e_struct*_tmp342=(struct Cyc_Absyn_Cast_e_struct*)
_tmp341;if(_tmp342->tag != 15)goto _LL14C;else{_tmp343=(void*)_tmp342->f1;_tmp344=
_tmp342->f2;}}_LL14B: i=_tmp344;goto inner_loop;_LL14C: {struct Cyc_Absyn_Const_e_struct*
_tmp345=(struct Cyc_Absyn_Const_e_struct*)_tmp341;if(_tmp345->tag != 0)goto _LL14E;
else{_tmp346=_tmp345->f1;if((_tmp346.Int_c).tag != 4)goto _LL14E;_tmp347=(struct
_tuple5)(_tmp346.Int_c).val;_tmp348=_tmp347.f1;if(_tmp348 != Cyc_Absyn_None)goto
_LL14E;_tmp349=_tmp347.f2;}}_LL14D: _tmp34E=_tmp349;goto _LL14F;_LL14E: {struct Cyc_Absyn_Const_e_struct*
_tmp34A=(struct Cyc_Absyn_Const_e_struct*)_tmp341;if(_tmp34A->tag != 0)goto _LL150;
else{_tmp34B=_tmp34A->f1;if((_tmp34B.Int_c).tag != 4)goto _LL150;_tmp34C=(struct
_tuple5)(_tmp34B.Int_c).val;_tmp34D=_tmp34C.f1;if(_tmp34D != Cyc_Absyn_Signed)
goto _LL150;_tmp34E=_tmp34C.f2;}}_LL14F: return _tmp34E >= 0  && Cyc_Toc_check_const_array((
unsigned int)(_tmp34E + 1),_tmp340);_LL150: {struct Cyc_Absyn_Const_e_struct*
_tmp34F=(struct Cyc_Absyn_Const_e_struct*)_tmp341;if(_tmp34F->tag != 0)goto _LL152;
else{_tmp350=_tmp34F->f1;if((_tmp350.Int_c).tag != 4)goto _LL152;_tmp351=(struct
_tuple5)(_tmp350.Int_c).val;_tmp352=_tmp351.f1;if(_tmp352 != Cyc_Absyn_Unsigned)
goto _LL152;_tmp353=_tmp351.f2;}}_LL151: return Cyc_Toc_check_const_array((
unsigned int)(_tmp353 + 1),_tmp340);_LL152: {struct Cyc_Absyn_Primop_e_struct*
_tmp354=(struct Cyc_Absyn_Primop_e_struct*)_tmp341;if(_tmp354->tag != 3)goto _LL154;
else{_tmp355=_tmp354->f1;if(_tmp355 != Cyc_Absyn_Mod)goto _LL154;_tmp356=_tmp354->f2;
if(_tmp356 == 0)goto _LL154;_tmp357=*_tmp356;_tmp358=(struct Cyc_Absyn_Exp*)_tmp357.hd;
_tmp359=_tmp357.tl;if(_tmp359 == 0)goto _LL154;_tmp35A=*_tmp359;_tmp35B=(struct Cyc_Absyn_Exp*)
_tmp35A.hd;}}_LL153: return Cyc_Toc_check_leq_size(relns,x,_tmp35B,_tmp340);_LL154: {
struct Cyc_Absyn_Var_e_struct*_tmp35C=(struct Cyc_Absyn_Var_e_struct*)_tmp341;if(
_tmp35C->tag != 1)goto _LL156;else{_tmp35D=(void*)_tmp35C->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp35E=(struct Cyc_Absyn_Pat_b_struct*)_tmp35D;if(_tmp35E->tag != 5)goto _LL156;
else{_tmp35F=_tmp35E->f1;}};}}_LL155: _tmp363=_tmp35F;goto _LL157;_LL156: {struct
Cyc_Absyn_Var_e_struct*_tmp360=(struct Cyc_Absyn_Var_e_struct*)_tmp341;if(_tmp360->tag
!= 1)goto _LL158;else{_tmp361=(void*)_tmp360->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp362=(struct Cyc_Absyn_Local_b_struct*)_tmp361;if(_tmp362->tag != 4)goto _LL158;
else{_tmp363=_tmp362->f1;}};}}_LL157: _tmp367=_tmp363;goto _LL159;_LL158: {struct
Cyc_Absyn_Var_e_struct*_tmp364=(struct Cyc_Absyn_Var_e_struct*)_tmp341;if(_tmp364->tag
!= 1)goto _LL15A;else{_tmp365=(void*)_tmp364->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp366=(struct Cyc_Absyn_Global_b_struct*)_tmp365;if(_tmp366->tag != 1)goto _LL15A;
else{_tmp367=_tmp366->f1;}};}}_LL159: _tmp36B=_tmp367;goto _LL15B;_LL15A: {struct
Cyc_Absyn_Var_e_struct*_tmp368=(struct Cyc_Absyn_Var_e_struct*)_tmp341;if(_tmp368->tag
!= 1)goto _LL15C;else{_tmp369=(void*)_tmp368->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp36A=(struct Cyc_Absyn_Param_b_struct*)_tmp369;if(_tmp36A->tag != 3)goto _LL15C;
else{_tmp36B=_tmp36A->f1;}};}}_LL15B: if(_tmp36B->escapes)return 0;{struct Cyc_List_List*
_tmp36C=relns;for(0;_tmp36C != 0;_tmp36C=_tmp36C->tl){struct Cyc_CfFlowInfo_Reln*
_tmp36D=(struct Cyc_CfFlowInfo_Reln*)_tmp36C->hd;if(_tmp36D->vd == _tmp36B){union
Cyc_CfFlowInfo_RelnOp _tmp36E=_tmp36D->rop;struct Cyc_Absyn_Vardecl*_tmp36F;struct
_tuple11 _tmp370;struct Cyc_Absyn_Vardecl*_tmp371;void*_tmp372;unsigned int _tmp373;
unsigned int _tmp374;_LL15F: if((_tmp36E.LessNumelts).tag != 3)goto _LL161;_tmp36F=(
struct Cyc_Absyn_Vardecl*)(_tmp36E.LessNumelts).val;_LL160: if(x == _tmp36F)return 1;
else{goto _LL15E;}_LL161: if((_tmp36E.LessVar).tag != 2)goto _LL163;_tmp370=(struct
_tuple11)(_tmp36E.LessVar).val;_tmp371=_tmp370.f1;_tmp372=_tmp370.f2;_LL162:{
struct _tuple18 _tmpBA7;struct _tuple18 _tmp376=(_tmpBA7.f1=Cyc_Tcutil_compress(
_tmp372),((_tmpBA7.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(a->topt))->v),_tmpBA7)));void*_tmp377;void*_tmp379;void*_tmp37A;
struct Cyc_Absyn_PtrInfo _tmp37C;struct Cyc_Absyn_PtrAtts _tmp37D;union Cyc_Absyn_Constraint*
_tmp37E;_LL16A: _tmp377=_tmp376.f1;{struct Cyc_Absyn_TagType_struct*_tmp378=(
struct Cyc_Absyn_TagType_struct*)_tmp377;if(_tmp378->tag != 20)goto _LL16C;else{
_tmp379=(void*)_tmp378->f1;}};_tmp37A=_tmp376.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp37B=(struct Cyc_Absyn_PointerType_struct*)_tmp37A;if(_tmp37B->tag != 5)goto
_LL16C;else{_tmp37C=_tmp37B->f1;_tmp37D=_tmp37C.ptr_atts;_tmp37E=_tmp37D.bounds;}};
_LL16B:{void*_tmp37F=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp37E);struct Cyc_Absyn_Exp*_tmp381;_LL16F: {struct Cyc_Absyn_Upper_b_struct*
_tmp380=(struct Cyc_Absyn_Upper_b_struct*)_tmp37F;if(_tmp380->tag != 1)goto _LL171;
else{_tmp381=_tmp380->f1;}}_LL170: {struct Cyc_Absyn_Exp*_tmp382=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp379,0),0,Cyc_Absyn_No_coercion,0);
if(Cyc_Evexp_lte_const_exp(_tmp382,_tmp381))return 1;goto _LL16E;}_LL171:;_LL172:
goto _LL16E;_LL16E:;}goto _LL169;_LL16C:;_LL16D: goto _LL169;_LL169:;}{struct Cyc_List_List*
_tmp383=relns;for(0;_tmp383 != 0;_tmp383=_tmp383->tl){struct Cyc_CfFlowInfo_Reln*
_tmp384=(struct Cyc_CfFlowInfo_Reln*)_tmp383->hd;if(_tmp384->vd == _tmp371){union
Cyc_CfFlowInfo_RelnOp _tmp385=_tmp384->rop;struct Cyc_Absyn_Vardecl*_tmp386;struct
Cyc_Absyn_Vardecl*_tmp387;unsigned int _tmp388;unsigned int _tmp389;struct _tuple11
_tmp38A;struct Cyc_Absyn_Vardecl*_tmp38B;_LL174: if((_tmp385.LessEqNumelts).tag != 
5)goto _LL176;_tmp386=(struct Cyc_Absyn_Vardecl*)(_tmp385.LessEqNumelts).val;
_LL175: _tmp387=_tmp386;goto _LL177;_LL176: if((_tmp385.LessNumelts).tag != 3)goto
_LL178;_tmp387=(struct Cyc_Absyn_Vardecl*)(_tmp385.LessNumelts).val;_LL177: if(x == 
_tmp387)return 1;goto _LL173;_LL178: if((_tmp385.EqualConst).tag != 1)goto _LL17A;
_tmp388=(unsigned int)(_tmp385.EqualConst).val;_LL179: return Cyc_Toc_check_const_array(
_tmp388,_tmp340);_LL17A: if((_tmp385.LessEqConst).tag != 6)goto _LL17C;_tmp389=(
unsigned int)(_tmp385.LessEqConst).val;if(!(_tmp389 > 0))goto _LL17C;_LL17B: return
Cyc_Toc_check_const_array(_tmp389,_tmp340);_LL17C: if((_tmp385.LessVar).tag != 2)
goto _LL17E;_tmp38A=(struct _tuple11)(_tmp385.LessVar).val;_tmp38B=_tmp38A.f1;
_LL17D: if(Cyc_Toc_check_leq_size_var(relns,x,_tmp38B))return 1;goto _LL173;_LL17E:;
_LL17F: goto _LL173;_LL173:;}}}goto _LL15E;_LL163: if((_tmp36E.LessConst).tag != 4)
goto _LL165;_tmp373=(unsigned int)(_tmp36E.LessConst).val;_LL164: return Cyc_Toc_check_const_array(
_tmp373,_tmp340);_LL165: if((_tmp36E.LessEqConst).tag != 6)goto _LL167;_tmp374=(
unsigned int)(_tmp36E.LessEqConst).val;_LL166: return Cyc_Toc_check_const_array(
_tmp374 + 1,_tmp340);_LL167:;_LL168: goto _LL15E;_LL15E:;}}}goto _LL149;_LL15C:;
_LL15D: goto _LL149;_LL149:;}return 0;};}static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*
e){if(e->topt == 0){const char*_tmpBAA;void*_tmpBA9;(_tmpBA9=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBAA="Missing type in primop ",
_tag_dyneither(_tmpBAA,sizeof(char),24))),_tag_dyneither(_tmpBA9,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*
_tmpBAD;void*_tmpBAC;(_tmpBAC=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBAD="Missing type in primop ",
_tag_dyneither(_tmpBAD,sizeof(char),24))),_tag_dyneither(_tmpBAC,sizeof(void*),0)));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple9*
Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){struct _tuple9*_tmpBAE;return(_tmpBAE=
_cycalloc(sizeof(*_tmpBAE)),((_tmpBAE->f1=Cyc_Toc_mt_tq,((_tmpBAE->f2=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmpBAE)))));}static struct
_tuple15*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple15*_tmpBAF;return(_tmpBAF=_cycalloc(sizeof(*_tmpBAF)),((
_tmpBAF->f1=0,((_tmpBAF->f2=e,_tmpBAF)))));};}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(
struct Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int
pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;
if(pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp392=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0  || Cyc_Absyn_no_regions)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(
_tmp392): Cyc_Toc_malloc_ptr(_tmp392));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp392);}};}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp393=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp393 != 0;_tmp393=_tmp393->tl){struct _tuple15 _tmp395;struct Cyc_List_List*
_tmp396;struct Cyc_Absyn_Exp*_tmp397;struct _tuple15*_tmp394=(struct _tuple15*)
_tmp393->hd;_tmp395=*_tmp394;_tmp396=_tmp395.f1;_tmp397=_tmp395.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp396 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp396->tl != 0){const char*_tmpBB2;void*_tmpBB1;(_tmpBB1=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBB2="multiple designators in array",
_tag_dyneither(_tmpBB2,sizeof(char),30))),_tag_dyneither(_tmpBB1,sizeof(void*),0)));}{
void*_tmp39A=(void*)_tmp396->hd;void*_tmp39B=_tmp39A;struct Cyc_Absyn_Exp*_tmp39D;
_LL181: {struct Cyc_Absyn_ArrayElement_struct*_tmp39C=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp39B;if(_tmp39C->tag != 0)goto _LL183;else{_tmp39D=_tmp39C->f1;}}_LL182: Cyc_Toc_exp_to_c(
nv,_tmp39D);e_index=_tmp39D;goto _LL180;_LL183: {struct Cyc_Absyn_FieldName_struct*
_tmp39E=(struct Cyc_Absyn_FieldName_struct*)_tmp39B;if(_tmp39E->tag != 1)goto
_LL180;}_LL184: {const char*_tmpBB5;void*_tmpBB4;(_tmpBB4=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBB5="field name designators in array",
_tag_dyneither(_tmpBB5,sizeof(char),32))),_tag_dyneither(_tmpBB4,sizeof(void*),0)));}
_LL180:;};}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*
_tmp3A1=_tmp397->r;struct Cyc_List_List*_tmp3A3;struct Cyc_Absyn_Vardecl*_tmp3A5;
struct Cyc_Absyn_Exp*_tmp3A6;struct Cyc_Absyn_Exp*_tmp3A7;int _tmp3A8;void*_tmp3AA;
struct Cyc_List_List*_tmp3AB;_LL186: {struct Cyc_Absyn_Array_e_struct*_tmp3A2=(
struct Cyc_Absyn_Array_e_struct*)_tmp3A1;if(_tmp3A2->tag != 27)goto _LL188;else{
_tmp3A3=_tmp3A2->f1;}}_LL187: s=Cyc_Toc_init_array(nv,lval,_tmp3A3,s);goto _LL185;
_LL188: {struct Cyc_Absyn_Comprehension_e_struct*_tmp3A4=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3A1;if(_tmp3A4->tag != 28)goto _LL18A;else{_tmp3A5=_tmp3A4->f1;_tmp3A6=_tmp3A4->f2;
_tmp3A7=_tmp3A4->f3;_tmp3A8=_tmp3A4->f4;}}_LL189: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp3A5,_tmp3A6,_tmp3A7,_tmp3A8,s,0);goto _LL185;_LL18A: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3A9=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3A1;if(_tmp3A9->tag != 30)goto
_LL18C;else{_tmp3AA=(void*)_tmp3A9->f1;_tmp3AB=_tmp3A9->f2;}}_LL18B: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3AA,_tmp3AB,s);goto _LL185;_LL18C:;_LL18D: Cyc_Toc_exp_to_c(nv,_tmp397);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
_tmp397,0),s,0);goto _LL185;_LL185:;};};}}return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple0*_tmp3AC=vd->name;void*_tmp3AD=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env _tmp3AF;struct _RegionHandle*_tmp3B0;
struct Cyc_Toc_Env*_tmp3AE=nv;_tmp3AF=*_tmp3AE;_tmp3B0=_tmp3AF.rgn;{struct Cyc_Absyn_Local_b_struct
_tmpBB8;struct Cyc_Absyn_Local_b_struct*_tmpBB7;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
_tmp3B0,nv,_tmp3AC,Cyc_Absyn_varb_exp(_tmp3AC,(void*)((_tmpBB7=_cycalloc(sizeof(*
_tmpBB7)),((_tmpBB7[0]=((_tmpBB8.tag=4,((_tmpBB8.f1=vd,_tmpBB8)))),_tmpBB7)))),0));
struct _tuple0*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp3AC,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp3AC,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp3AC,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp3AC,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp3B1=e2->r;struct Cyc_List_List*_tmp3B3;
struct Cyc_Absyn_Vardecl*_tmp3B5;struct Cyc_Absyn_Exp*_tmp3B6;struct Cyc_Absyn_Exp*
_tmp3B7;int _tmp3B8;void*_tmp3BA;struct Cyc_List_List*_tmp3BB;_LL18F: {struct Cyc_Absyn_Array_e_struct*
_tmp3B2=(struct Cyc_Absyn_Array_e_struct*)_tmp3B1;if(_tmp3B2->tag != 27)goto _LL191;
else{_tmp3B3=_tmp3B2->f1;}}_LL190: body=Cyc_Toc_init_array(nv2,lval,_tmp3B3,Cyc_Toc_skip_stmt_dl());
goto _LL18E;_LL191: {struct Cyc_Absyn_Comprehension_e_struct*_tmp3B4=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3B1;if(_tmp3B4->tag != 28)goto _LL193;else{_tmp3B5=_tmp3B4->f1;_tmp3B6=_tmp3B4->f2;
_tmp3B7=_tmp3B4->f3;_tmp3B8=_tmp3B4->f4;}}_LL192: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp3B5,_tmp3B6,_tmp3B7,_tmp3B8,Cyc_Toc_skip_stmt_dl(),0);goto _LL18E;
_LL193: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp3B9=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp3B1;if(_tmp3B9->tag != 30)goto _LL195;else{_tmp3BA=(void*)_tmp3B9->f1;_tmp3BB=
_tmp3B9->f2;}}_LL194: body=Cyc_Toc_init_anon_struct(nv,lval,_tmp3BA,_tmp3BB,Cyc_Toc_skip_stmt_dl());
goto _LL18E;_LL195:;_LL196: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL18E;_LL18E:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp3AD,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp3AC,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp3BE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp3BE != 0;_tmp3BE=_tmp3BE->tl){struct _tuple15 _tmp3C0;
struct Cyc_List_List*_tmp3C1;struct Cyc_Absyn_Exp*_tmp3C2;struct _tuple15*_tmp3BF=(
struct _tuple15*)_tmp3BE->hd;_tmp3C0=*_tmp3BF;_tmp3C1=_tmp3C0.f1;_tmp3C2=_tmp3C0.f2;
if(_tmp3C1 == 0){const char*_tmpBBB;void*_tmpBBA;(_tmpBBA=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBBB="empty designator list",
_tag_dyneither(_tmpBBB,sizeof(char),22))),_tag_dyneither(_tmpBBA,sizeof(void*),0)));}
if(_tmp3C1->tl != 0){const char*_tmpBBE;void*_tmpBBD;(_tmpBBD=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBBE="too many designators in anonymous struct",
_tag_dyneither(_tmpBBE,sizeof(char),41))),_tag_dyneither(_tmpBBD,sizeof(void*),0)));}{
void*_tmp3C7=(void*)_tmp3C1->hd;struct _dyneither_ptr*_tmp3C9;_LL198: {struct Cyc_Absyn_FieldName_struct*
_tmp3C8=(struct Cyc_Absyn_FieldName_struct*)_tmp3C7;if(_tmp3C8->tag != 1)goto
_LL19A;else{_tmp3C9=_tmp3C8->f1;}}_LL199: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(
lhs,_tmp3C9,0);{void*_tmp3CA=_tmp3C2->r;struct Cyc_List_List*_tmp3CC;struct Cyc_Absyn_Vardecl*
_tmp3CE;struct Cyc_Absyn_Exp*_tmp3CF;struct Cyc_Absyn_Exp*_tmp3D0;int _tmp3D1;void*
_tmp3D3;struct Cyc_List_List*_tmp3D4;_LL19D: {struct Cyc_Absyn_Array_e_struct*
_tmp3CB=(struct Cyc_Absyn_Array_e_struct*)_tmp3CA;if(_tmp3CB->tag != 27)goto _LL19F;
else{_tmp3CC=_tmp3CB->f1;}}_LL19E: s=Cyc_Toc_init_array(nv,lval,_tmp3CC,s);goto
_LL19C;_LL19F: {struct Cyc_Absyn_Comprehension_e_struct*_tmp3CD=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3CA;if(_tmp3CD->tag != 28)goto _LL1A1;else{_tmp3CE=_tmp3CD->f1;_tmp3CF=_tmp3CD->f2;
_tmp3D0=_tmp3CD->f3;_tmp3D1=_tmp3CD->f4;}}_LL1A0: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp3CE,_tmp3CF,_tmp3D0,_tmp3D1,s,0);goto _LL19C;_LL1A1: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3D2=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3CA;if(_tmp3D2->tag != 30)goto
_LL1A3;else{_tmp3D3=(void*)_tmp3D2->f1;_tmp3D4=_tmp3D2->f2;}}_LL1A2: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3D3,_tmp3D4,s);goto _LL19C;_LL1A3:;_LL1A4: Cyc_Toc_exp_to_c(nv,_tmp3C2);
if(Cyc_Toc_is_poly_field(struct_type,_tmp3C9))_tmp3C2=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp3C2);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp3C2,0),0),s,0);goto _LL19C;_LL19C:;}goto _LL197;}_LL19A:;_LL19B: {const char*
_tmpBC1;void*_tmpBC0;(_tmpBC0=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC1="array designator in struct",
_tag_dyneither(_tmpBC1,sizeof(char),27))),_tag_dyneither(_tmpBC0,sizeof(void*),0)));}
_LL197:;};}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct Cyc_Toc_Env
_tmp3D8;struct _RegionHandle*_tmp3D9;struct Cyc_Toc_Env*_tmp3D7=nv;_tmp3D8=*
_tmp3D7;_tmp3D9=_tmp3D8.rgn;{struct Cyc_List_List*_tmp3DA=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_rmap)(_tmp3D9,Cyc_Toc_tup_to_c,es);void*_tmp3DB=Cyc_Toc_add_tuple_type(
_tmp3DA);struct _tuple0*_tmp3DC=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp3DD=
Cyc_Absyn_var_exp(_tmp3DC,0);struct Cyc_Absyn_Stmt*_tmp3DE=Cyc_Absyn_exp_stmt(
_tmp3DD,0);struct Cyc_Absyn_Exp*(*_tmp3DF)(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp3E0=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp3D9,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp3E0);for(0;_tmp3E0 != 0;(_tmp3E0=
_tmp3E0->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp3E0->hd;struct
Cyc_Absyn_Exp*lval=_tmp3DF(_tmp3DD,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp3E1=e->r;struct Cyc_List_List*_tmp3E3;struct Cyc_Absyn_Vardecl*_tmp3E5;
struct Cyc_Absyn_Exp*_tmp3E6;struct Cyc_Absyn_Exp*_tmp3E7;int _tmp3E8;_LL1A6: {
struct Cyc_Absyn_Array_e_struct*_tmp3E2=(struct Cyc_Absyn_Array_e_struct*)_tmp3E1;
if(_tmp3E2->tag != 27)goto _LL1A8;else{_tmp3E3=_tmp3E2->f1;}}_LL1A7: _tmp3DE=Cyc_Toc_init_array(
nv,lval,_tmp3E3,_tmp3DE);goto _LL1A5;_LL1A8: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp3E4=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp3E1;if(_tmp3E4->tag != 28)
goto _LL1AA;else{_tmp3E5=_tmp3E4->f1;_tmp3E6=_tmp3E4->f2;_tmp3E7=_tmp3E4->f3;
_tmp3E8=_tmp3E4->f4;}}_LL1A9: _tmp3DE=Cyc_Toc_init_comprehension(nv,lval,_tmp3E5,
_tmp3E6,_tmp3E7,_tmp3E8,_tmp3DE,0);goto _LL1A5;_LL1AA:;_LL1AB: Cyc_Toc_exp_to_c(nv,
e);_tmp3DE=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3DF(
_tmp3DD,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp3DE,0);goto _LL1A5;_LL1A5:;};}}
return Cyc_Toc_make_struct(nv,_tmp3DC,_tmp3DB,_tmp3DE,pointer,rgnopt,is_atomic);};}
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct
_dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp3E9=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp3E9 != 0;_tmp3E9=_tmp3E9->tl){struct Cyc_Absyn_Aggrfield*
_tmp3EA=(struct Cyc_Absyn_Aggrfield*)_tmp3E9->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp3EA->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}{
struct Cyc_String_pa_struct _tmpBC9;void*_tmpBC8[1];const char*_tmpBC7;void*_tmpBC6;(
_tmpBC6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)((_tmpBC9.tag=0,((_tmpBC9.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmpBC8[0]=& _tmpBC9,Cyc_aprintf(((_tmpBC7="get_member_offset %s failed",
_tag_dyneither(_tmpBC7,sizeof(char),28))),_tag_dyneither(_tmpBC8,sizeof(void*),1)))))))),
_tag_dyneither(_tmpBC6,sizeof(void*),0)));};}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){struct _tuple0*_tmp3EF=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3F0=Cyc_Absyn_var_exp(_tmp3EF,0);struct Cyc_Absyn_Stmt*
_tmp3F1=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp3F0),0);struct Cyc_Absyn_Exp*(*
_tmp3F2)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*)=
pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;void*_tmp3F3=Cyc_Toc_typ_to_c(
struct_type);int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp3F4=Cyc_Tcutil_compress(
struct_type);struct Cyc_Absyn_AggrInfo _tmp3F6;union Cyc_Absyn_AggrInfoU _tmp3F7;
_LL1AD: {struct Cyc_Absyn_AggrType_struct*_tmp3F5=(struct Cyc_Absyn_AggrType_struct*)
_tmp3F4;if(_tmp3F5->tag != 12)goto _LL1AF;else{_tmp3F6=_tmp3F5->f1;_tmp3F7=_tmp3F6.aggr_info;}}
_LL1AE: ad=Cyc_Absyn_get_known_aggrdecl(_tmp3F7);goto _LL1AC;_LL1AF:;_LL1B0: {
const char*_tmpBCC;void*_tmpBCB;(_tmpBCB=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBCC="init_struct: bad struct type",
_tag_dyneither(_tmpBCC,sizeof(char),29))),_tag_dyneither(_tmpBCB,sizeof(void*),0)));}
_LL1AC:;}{int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct Cyc_Toc_Env _tmp3FB;struct _RegionHandle*_tmp3FC;struct Cyc_Toc_Env*_tmp3FA=
nv;_tmp3FB=*_tmp3FA;_tmp3FC=_tmp3FB.rgn;{struct Cyc_List_List*_tmp3FD=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp3FC,dles);for(0;
_tmp3FD != 0;_tmp3FD=_tmp3FD->tl){struct _tuple15 _tmp3FF;struct Cyc_List_List*
_tmp400;struct Cyc_Absyn_Exp*_tmp401;struct _tuple15*_tmp3FE=(struct _tuple15*)
_tmp3FD->hd;_tmp3FF=*_tmp3FE;_tmp400=_tmp3FF.f1;_tmp401=_tmp3FF.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp401->topt))->v);
if(_tmp400 == 0){const char*_tmpBCF;void*_tmpBCE;(_tmpBCE=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBCF="empty designator list",
_tag_dyneither(_tmpBCF,sizeof(char),22))),_tag_dyneither(_tmpBCE,sizeof(void*),0)));}
if(_tmp400->tl != 0){struct _tuple0*_tmp404=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp405=Cyc_Absyn_var_exp(_tmp404,0);for(0;_tmp400 != 0;_tmp400=_tmp400->tl){void*
_tmp406=(void*)_tmp400->hd;struct _dyneither_ptr*_tmp408;_LL1B2: {struct Cyc_Absyn_FieldName_struct*
_tmp407=(struct Cyc_Absyn_FieldName_struct*)_tmp406;if(_tmp407->tag != 1)goto
_LL1B4;else{_tmp408=_tmp407->f1;}}_LL1B3: if(Cyc_Toc_is_poly_field(struct_type,
_tmp408))_tmp405=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp405);_tmp3F1=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3F2(_tmp3F0,_tmp408,0),_tmp405,0),0),
_tmp3F1,0);goto _LL1B1;_LL1B4:;_LL1B5: {const char*_tmpBD2;void*_tmpBD1;(_tmpBD1=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBD2="array designator in struct",_tag_dyneither(_tmpBD2,sizeof(char),27))),
_tag_dyneither(_tmpBD1,sizeof(void*),0)));}_LL1B1:;}Cyc_Toc_exp_to_c(nv,_tmp401);
_tmp3F1=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp405,
_tmp401,0),0),_tmp3F1,0);}else{void*_tmp40B=(void*)_tmp400->hd;struct
_dyneither_ptr*_tmp40D;_LL1B7: {struct Cyc_Absyn_FieldName_struct*_tmp40C=(struct
Cyc_Absyn_FieldName_struct*)_tmp40B;if(_tmp40C->tag != 1)goto _LL1B9;else{_tmp40D=
_tmp40C->f1;}}_LL1B8: {struct Cyc_Absyn_Exp*lval=_tmp3F2(_tmp3F0,_tmp40D,0);if(
is_tagged_union){int i=Cyc_Toc_get_member_offset(ad,_tmp40D);struct Cyc_Absyn_Exp*
f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_aggrmember_exp(
lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,
f_tag_exp,0);_tmp3F1=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3F1,0);
lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{void*_tmp40E=_tmp401->r;
struct Cyc_List_List*_tmp410;struct Cyc_Absyn_Vardecl*_tmp412;struct Cyc_Absyn_Exp*
_tmp413;struct Cyc_Absyn_Exp*_tmp414;int _tmp415;void*_tmp417;struct Cyc_List_List*
_tmp418;_LL1BC: {struct Cyc_Absyn_Array_e_struct*_tmp40F=(struct Cyc_Absyn_Array_e_struct*)
_tmp40E;if(_tmp40F->tag != 27)goto _LL1BE;else{_tmp410=_tmp40F->f1;}}_LL1BD:
_tmp3F1=Cyc_Toc_init_array(nv,lval,_tmp410,_tmp3F1);goto _LL1BB;_LL1BE: {struct
Cyc_Absyn_Comprehension_e_struct*_tmp411=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp40E;if(_tmp411->tag != 28)goto _LL1C0;else{_tmp412=_tmp411->f1;_tmp413=_tmp411->f2;
_tmp414=_tmp411->f3;_tmp415=_tmp411->f4;}}_LL1BF: _tmp3F1=Cyc_Toc_init_comprehension(
nv,lval,_tmp412,_tmp413,_tmp414,_tmp415,_tmp3F1,0);goto _LL1BB;_LL1C0: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp416=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp40E;if(_tmp416->tag != 30)goto
_LL1C2;else{_tmp417=(void*)_tmp416->f1;_tmp418=_tmp416->f2;}}_LL1C1: _tmp3F1=Cyc_Toc_init_anon_struct(
nv,lval,_tmp417,_tmp418,_tmp3F1);goto _LL1BB;_LL1C2:;_LL1C3: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp401->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp401);{struct Cyc_Absyn_Aggrfield*_tmp419=Cyc_Absyn_lookup_decl_field(ad,
_tmp40D);if(Cyc_Toc_is_poly_field(struct_type,_tmp40D) && !was_ptr_type)_tmp401=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp401);if(has_exists)_tmp401=Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp419))->type),
_tmp401);_tmp3F1=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp401,0),0),_tmp3F1,0);goto _LL1BB;};}_LL1BB:;}goto _LL1B6;}_LL1B9:;_LL1BA: {
const char*_tmpBD5;void*_tmpBD4;(_tmpBD4=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD5="array designator in struct",
_tag_dyneither(_tmpBD5,sizeof(char),27))),_tag_dyneither(_tmpBD4,sizeof(void*),0)));}
_LL1B6:;}}}return Cyc_Toc_make_struct(nv,_tmp3EF,_tmp3F3,_tmp3F1,pointer,rgnopt,
is_atomic);};}struct _tuple19{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct
_tuple19*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(
e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,
enum Cyc_Absyn_Incrementor incr){struct Cyc_Absyn_Increment_e_struct _tmpBD8;struct
Cyc_Absyn_Increment_e_struct*_tmpBD7;return Cyc_Absyn_new_exp((void*)((_tmpBD7=
_cycalloc(sizeof(*_tmpBD7)),((_tmpBD7[0]=((_tmpBD8.tag=5,((_tmpBD8.f1=e1,((
_tmpBD8.f2=incr,_tmpBD8)))))),_tmpBD7)))),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp41E=e1->r;struct Cyc_Absyn_Stmt*_tmp420;void*_tmp422;struct Cyc_Absyn_Exp*
_tmp423;struct Cyc_Absyn_Exp*_tmp425;struct _dyneither_ptr*_tmp426;int _tmp427;int
_tmp428;_LL1C5: {struct Cyc_Absyn_StmtExp_e_struct*_tmp41F=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp41E;if(_tmp41F->tag != 37)goto _LL1C7;else{_tmp420=_tmp41F->f1;}}_LL1C6: Cyc_Toc_lvalue_assign_stmt(
_tmp420,fs,f,f_env);goto _LL1C4;_LL1C7: {struct Cyc_Absyn_Cast_e_struct*_tmp421=(
struct Cyc_Absyn_Cast_e_struct*)_tmp41E;if(_tmp421->tag != 15)goto _LL1C9;else{
_tmp422=(void*)_tmp421->f1;_tmp423=_tmp421->f2;}}_LL1C8: Cyc_Toc_lvalue_assign(
_tmp423,fs,f,f_env);goto _LL1C4;_LL1C9: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp424=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp41E;if(_tmp424->tag != 22)goto
_LL1CB;else{_tmp425=_tmp424->f1;_tmp426=_tmp424->f2;_tmp427=_tmp424->f3;_tmp428=
_tmp424->f4;}}_LL1CA: e1->r=_tmp425->r;{struct Cyc_List_List*_tmpBD9;Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)((_tmpBD9=_cycalloc(sizeof(*_tmpBD9)),((_tmpBD9->hd=
_tmp426,((_tmpBD9->tl=fs,_tmpBD9)))))),f,f_env);}goto _LL1C4;_LL1CB:;_LL1CC: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;goto _LL1C4;}_LL1C4:;}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env){void*_tmp42A=s->r;struct Cyc_Absyn_Exp*_tmp42C;struct Cyc_Absyn_Decl*
_tmp42E;struct Cyc_Absyn_Stmt*_tmp42F;struct Cyc_Absyn_Stmt*_tmp431;_LL1CE: {
struct Cyc_Absyn_Exp_s_struct*_tmp42B=(struct Cyc_Absyn_Exp_s_struct*)_tmp42A;if(
_tmp42B->tag != 1)goto _LL1D0;else{_tmp42C=_tmp42B->f1;}}_LL1CF: Cyc_Toc_lvalue_assign(
_tmp42C,fs,f,f_env);goto _LL1CD;_LL1D0: {struct Cyc_Absyn_Decl_s_struct*_tmp42D=(
struct Cyc_Absyn_Decl_s_struct*)_tmp42A;if(_tmp42D->tag != 12)goto _LL1D2;else{
_tmp42E=_tmp42D->f1;_tmp42F=_tmp42D->f2;}}_LL1D1: Cyc_Toc_lvalue_assign_stmt(
_tmp42F,fs,f,f_env);goto _LL1CD;_LL1D2: {struct Cyc_Absyn_Seq_s_struct*_tmp430=(
struct Cyc_Absyn_Seq_s_struct*)_tmp42A;if(_tmp430->tag != 2)goto _LL1D4;else{
_tmp431=_tmp430->f2;}}_LL1D3: Cyc_Toc_lvalue_assign_stmt(_tmp431,fs,f,f_env);goto
_LL1CD;_LL1D4:;_LL1D5: {const char*_tmpBDD;void*_tmpBDC[1];struct Cyc_String_pa_struct
_tmpBDB;(_tmpBDB.tag=0,((_tmpBDB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmpBDC[0]=& _tmpBDB,Cyc_Toc_toc_impos(((_tmpBDD="lvalue_assign_stmt: %s",
_tag_dyneither(_tmpBDD,sizeof(char),23))),_tag_dyneither(_tmpBDC,sizeof(void*),1)))))));}
_LL1CD:;}static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);static
struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){void*_tmp435=
e->r;void*_tmp437;void**_tmp438;struct Cyc_Absyn_Exp*_tmp439;struct Cyc_Absyn_Exp**
_tmp43A;struct Cyc_Absyn_Exp*_tmp43C;struct Cyc_Absyn_Stmt*_tmp43E;_LL1D7: {struct
Cyc_Absyn_Cast_e_struct*_tmp436=(struct Cyc_Absyn_Cast_e_struct*)_tmp435;if(
_tmp436->tag != 15)goto _LL1D9;else{_tmp437=(void**)& _tmp436->f1;_tmp438=(void**)((
void**)& _tmp436->f1);_tmp439=_tmp436->f2;_tmp43A=(struct Cyc_Absyn_Exp**)& _tmp436->f2;}}
_LL1D8:*_tmp43A=Cyc_Toc_push_address_exp(*_tmp43A);*_tmp438=Cyc_Absyn_cstar_typ(*
_tmp438,Cyc_Toc_mt_tq);return e;_LL1D9: {struct Cyc_Absyn_Deref_e_struct*_tmp43B=(
struct Cyc_Absyn_Deref_e_struct*)_tmp435;if(_tmp43B->tag != 21)goto _LL1DB;else{
_tmp43C=_tmp43B->f1;}}_LL1DA: return _tmp43C;_LL1DB: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp43D=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp435;if(_tmp43D->tag != 37)goto
_LL1DD;else{_tmp43E=_tmp43D->f1;}}_LL1DC: Cyc_Toc_push_address_stmt(_tmp43E);
return e;_LL1DD:;_LL1DE: if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpBE1;void*_tmpBE0[1];struct Cyc_String_pa_struct _tmpBDF;(_tmpBDF.tag=
0,((_tmpBDF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpBE0[0]=& _tmpBDF,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpBE1="can't take & of exp %s",_tag_dyneither(_tmpBE1,
sizeof(char),23))),_tag_dyneither(_tmpBE0,sizeof(void*),1)))))));};_LL1D6:;}
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){void*_tmp442=s->r;
struct Cyc_Absyn_Stmt*_tmp444;struct Cyc_Absyn_Stmt*_tmp446;struct Cyc_Absyn_Exp*
_tmp448;struct Cyc_Absyn_Exp**_tmp449;_LL1E0: {struct Cyc_Absyn_Seq_s_struct*
_tmp443=(struct Cyc_Absyn_Seq_s_struct*)_tmp442;if(_tmp443->tag != 2)goto _LL1E2;
else{_tmp444=_tmp443->f2;}}_LL1E1: _tmp446=_tmp444;goto _LL1E3;_LL1E2: {struct Cyc_Absyn_Decl_s_struct*
_tmp445=(struct Cyc_Absyn_Decl_s_struct*)_tmp442;if(_tmp445->tag != 12)goto _LL1E4;
else{_tmp446=_tmp445->f2;}}_LL1E3: Cyc_Toc_push_address_stmt(_tmp446);goto _LL1DF;
_LL1E4: {struct Cyc_Absyn_Exp_s_struct*_tmp447=(struct Cyc_Absyn_Exp_s_struct*)
_tmp442;if(_tmp447->tag != 1)goto _LL1E6;else{_tmp448=_tmp447->f1;_tmp449=(struct
Cyc_Absyn_Exp**)& _tmp447->f1;}}_LL1E5:*_tmp449=Cyc_Toc_push_address_exp(*_tmp449);
goto _LL1DF;_LL1E6:;_LL1E7: {const char*_tmpBE5;void*_tmpBE4[1];struct Cyc_String_pa_struct
_tmpBE3;(_tmpBE3.tag=0,((_tmpBE3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmpBE4[0]=& _tmpBE3,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE5="can't take & of stmt %s",
_tag_dyneither(_tmpBE5,sizeof(char),24))),_tag_dyneither(_tmpBE4,sizeof(void*),1)))))));}
_LL1DF:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;
struct Cyc_List_List*prev;if(x == 0)return 0;{struct Cyc_List_List*_tmpBE6;result=((
_tmpBE6=_region_malloc(r2,sizeof(*_tmpBE6)),((_tmpBE6->hd=(void*)f((void*)x->hd,
env),((_tmpBE6->tl=0,_tmpBE6))))));}prev=result;for(x=x->tl;x != 0;x=x->tl){{
struct Cyc_List_List*_tmpBE7;((struct Cyc_List_List*)_check_null(prev))->tl=((
_tmpBE7=_region_malloc(r2,sizeof(*_tmpBE7)),((_tmpBE7->hd=(void*)f((void*)x->hd,
env),((_tmpBE7->tl=0,_tmpBE7))))));}prev=prev->tl;}return result;}static struct Cyc_List_List*
Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region,f,env,x);}static struct _tuple15*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*
e){struct _tuple15*_tmpBE8;return(_tmpBE8=_cycalloc(sizeof(*_tmpBE8)),((_tmpBE8->f1=
0,((_tmpBE8->f2=e,_tmpBE8)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(
void*t){void*_tmp450=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp452;
_LL1E9: {struct Cyc_Absyn_PointerType_struct*_tmp451=(struct Cyc_Absyn_PointerType_struct*)
_tmp450;if(_tmp451->tag != 5)goto _LL1EB;else{_tmp452=_tmp451->f1;}}_LL1EA: return
_tmp452;_LL1EB:;_LL1EC: {const char*_tmpBEB;void*_tmpBEA;(_tmpBEA=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBEB="get_ptr_typ: not a pointer!",
_tag_dyneither(_tmpBEB,sizeof(char),28))),_tag_dyneither(_tmpBEA,sizeof(void*),0)));}
_LL1E8:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp455=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp458;enum Cyc_Absyn_Size_of
_tmp459;enum Cyc_Absyn_Sign _tmp45B;enum Cyc_Absyn_Size_of _tmp45C;enum Cyc_Absyn_Sign
_tmp460;enum Cyc_Absyn_Size_of _tmp461;enum Cyc_Absyn_Sign _tmp463;enum Cyc_Absyn_Size_of
_tmp464;enum Cyc_Absyn_Sign _tmp466;enum Cyc_Absyn_Size_of _tmp467;_LL1EE: {struct
Cyc_Absyn_PointerType_struct*_tmp456=(struct Cyc_Absyn_PointerType_struct*)
_tmp455;if(_tmp456->tag != 5)goto _LL1F0;}_LL1EF: res=Cyc_Absyn_null_exp(0);goto
_LL1ED;_LL1F0: {struct Cyc_Absyn_IntType_struct*_tmp457=(struct Cyc_Absyn_IntType_struct*)
_tmp455;if(_tmp457->tag != 6)goto _LL1F2;else{_tmp458=_tmp457->f1;_tmp459=_tmp457->f2;
if(_tmp459 != Cyc_Absyn_Char_sz)goto _LL1F2;}}_LL1F1: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(
_tmp458,'\000'),0);goto _LL1ED;_LL1F2: {struct Cyc_Absyn_IntType_struct*_tmp45A=(
struct Cyc_Absyn_IntType_struct*)_tmp455;if(_tmp45A->tag != 6)goto _LL1F4;else{
_tmp45B=_tmp45A->f1;_tmp45C=_tmp45A->f2;if(_tmp45C != Cyc_Absyn_Short_sz)goto
_LL1F4;}}_LL1F3: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp45B,0),0);goto
_LL1ED;_LL1F4: {struct Cyc_Absyn_EnumType_struct*_tmp45D=(struct Cyc_Absyn_EnumType_struct*)
_tmp455;if(_tmp45D->tag != 14)goto _LL1F6;}_LL1F5: goto _LL1F7;_LL1F6: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp45E=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp455;if(_tmp45E->tag != 15)goto
_LL1F8;}_LL1F7: _tmp460=Cyc_Absyn_Unsigned;goto _LL1F9;_LL1F8: {struct Cyc_Absyn_IntType_struct*
_tmp45F=(struct Cyc_Absyn_IntType_struct*)_tmp455;if(_tmp45F->tag != 6)goto _LL1FA;
else{_tmp460=_tmp45F->f1;_tmp461=_tmp45F->f2;if(_tmp461 != Cyc_Absyn_Int_sz)goto
_LL1FA;}}_LL1F9: _tmp463=_tmp460;goto _LL1FB;_LL1FA: {struct Cyc_Absyn_IntType_struct*
_tmp462=(struct Cyc_Absyn_IntType_struct*)_tmp455;if(_tmp462->tag != 6)goto _LL1FC;
else{_tmp463=_tmp462->f1;_tmp464=_tmp462->f2;if(_tmp464 != Cyc_Absyn_Long_sz)goto
_LL1FC;}}_LL1FB: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp463,0),0);goto _LL1ED;
_LL1FC: {struct Cyc_Absyn_IntType_struct*_tmp465=(struct Cyc_Absyn_IntType_struct*)
_tmp455;if(_tmp465->tag != 6)goto _LL1FE;else{_tmp466=_tmp465->f1;_tmp467=_tmp465->f2;
if(_tmp467 != Cyc_Absyn_LongLong_sz)goto _LL1FE;}}_LL1FD: res=Cyc_Absyn_const_exp(
Cyc_Absyn_LongLong_c(_tmp466,(long long)0),0);goto _LL1ED;_LL1FE: {struct Cyc_Absyn_FloatType_struct*
_tmp468=(struct Cyc_Absyn_FloatType_struct*)_tmp455;if(_tmp468->tag != 7)goto
_LL200;}_LL1FF: goto _LL201;_LL200: {struct Cyc_Absyn_DoubleType_struct*_tmp469=(
struct Cyc_Absyn_DoubleType_struct*)_tmp455;if(_tmp469->tag != 8)goto _LL202;}
_LL201:{const char*_tmpBEC;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBEC="0.0",
_tag_dyneither(_tmpBEC,sizeof(char),4)))),0);}goto _LL1ED;_LL202:;_LL203: {const
char*_tmpBF0;void*_tmpBEF[1];struct Cyc_String_pa_struct _tmpBEE;(_tmpBEE.tag=0,((
_tmpBEE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmpBEF[0]=& _tmpBEE,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBF0="found non-zero type %s in generate_zero",_tag_dyneither(_tmpBF0,sizeof(
char),40))),_tag_dyneither(_tmpBEF,sizeof(void*),1)))))));}_LL1ED:;}{struct Cyc_Core_Opt*
_tmpBF1;res->topt=((_tmpBF1=_cycalloc(sizeof(*_tmpBF1)),((_tmpBF1->v=t,_tmpBF1))));}
return res;}static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dyneither,void*elt_type){void*fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);void*
_tmp46F=Cyc_Toc_typ_to_c(elt_type);void*_tmp470=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp471=Cyc_Absyn_cstar_typ(_tmp46F,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmpBF2;struct Cyc_Core_Opt*_tmp472=(_tmpBF2=_cycalloc(sizeof(*_tmpBF2)),((
_tmpBF2->v=_tmp471,_tmpBF2)));struct Cyc_Absyn_Exp*xinit;{void*_tmp473=e1->r;
struct Cyc_Absyn_Exp*_tmp475;struct Cyc_Absyn_Exp*_tmp477;struct Cyc_Absyn_Exp*
_tmp478;_LL205: {struct Cyc_Absyn_Deref_e_struct*_tmp474=(struct Cyc_Absyn_Deref_e_struct*)
_tmp473;if(_tmp474->tag != 21)goto _LL207;else{_tmp475=_tmp474->f1;}}_LL206: if(!
is_dyneither){_tmp475=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp475,0,Cyc_Absyn_Other_coercion,
0);{struct Cyc_Core_Opt*_tmpBF3;_tmp475->topt=((_tmpBF3=_cycalloc(sizeof(*_tmpBF3)),((
_tmpBF3->v=fat_ptr_type,_tmpBF3))));};}Cyc_Toc_exp_to_c(nv,_tmp475);xinit=
_tmp475;goto _LL204;_LL207: {struct Cyc_Absyn_Subscript_e_struct*_tmp476=(struct
Cyc_Absyn_Subscript_e_struct*)_tmp473;if(_tmp476->tag != 24)goto _LL209;else{
_tmp477=_tmp476->f1;_tmp478=_tmp476->f2;}}_LL208: if(!is_dyneither){_tmp477=Cyc_Absyn_cast_exp(
fat_ptr_type,_tmp477,0,Cyc_Absyn_Other_coercion,0);{struct Cyc_Core_Opt*_tmpBF4;
_tmp477->topt=((_tmpBF4=_cycalloc(sizeof(*_tmpBF4)),((_tmpBF4->v=fat_ptr_type,
_tmpBF4))));};}Cyc_Toc_exp_to_c(nv,_tmp477);Cyc_Toc_exp_to_c(nv,_tmp478);{struct
Cyc_Absyn_Exp*_tmpBF5[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,((
_tmpBF5[2]=_tmp478,((_tmpBF5[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmpBF5[0]=_tmp477,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpBF5,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL204;_LL209:;_LL20A: {const char*_tmpBF8;void*_tmpBF7;(_tmpBF7=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF8="found bad lhs for zero-terminated pointer assignment",
_tag_dyneither(_tmpBF8,sizeof(char),53))),_tag_dyneither(_tmpBF7,sizeof(void*),0)));}
_LL204:;}{struct _tuple0*_tmp47E=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp480;
struct _RegionHandle*_tmp481;struct Cyc_Toc_Env*_tmp47F=nv;_tmp480=*_tmp47F;
_tmp481=_tmp480.rgn;{struct Cyc_Toc_Env*_tmp482=Cyc_Toc_add_varmap(_tmp481,nv,
_tmp47E,Cyc_Absyn_var_exp(_tmp47E,0));struct Cyc_Absyn_Vardecl*_tmpBF9;struct Cyc_Absyn_Vardecl*
_tmp483=(_tmpBF9=_cycalloc(sizeof(*_tmpBF9)),((_tmpBF9->sc=Cyc_Absyn_Public,((
_tmpBF9->name=_tmp47E,((_tmpBF9->tq=Cyc_Toc_mt_tq,((_tmpBF9->type=_tmp470,((
_tmpBF9->initializer=(struct Cyc_Absyn_Exp*)xinit,((_tmpBF9->rgn=0,((_tmpBF9->attributes=
0,((_tmpBF9->escapes=0,_tmpBF9)))))))))))))))));struct Cyc_Absyn_Local_b_struct
_tmpBFC;struct Cyc_Absyn_Local_b_struct*_tmpBFB;struct Cyc_Absyn_Local_b_struct*
_tmp484=(_tmpBFB=_cycalloc(sizeof(*_tmpBFB)),((_tmpBFB[0]=((_tmpBFC.tag=4,((
_tmpBFC.f1=_tmp483,_tmpBFC)))),_tmpBFB)));struct Cyc_Absyn_Exp*_tmp485=Cyc_Absyn_varb_exp(
_tmp47E,(void*)_tmp484,0);{struct Cyc_Core_Opt*_tmpBFD;_tmp485->topt=((_tmpBFD=
_cycalloc(sizeof(*_tmpBFD)),((_tmpBFD->v=fat_ptr_type,_tmpBFD))));}{struct Cyc_Absyn_Exp*
_tmp487=Cyc_Absyn_deref_exp(_tmp485,0);{struct Cyc_Core_Opt*_tmpBFE;_tmp487->topt=((
_tmpBFE=_cycalloc(sizeof(*_tmpBFE)),((_tmpBFE->v=elt_type,_tmpBFE))));}Cyc_Toc_exp_to_c(
_tmp482,_tmp487);{struct _tuple0*_tmp489=Cyc_Toc_temp_var();_tmp482=Cyc_Toc_add_varmap(
_tmp481,_tmp482,_tmp489,Cyc_Absyn_var_exp(_tmp489,0));{struct Cyc_Absyn_Vardecl*
_tmpBFF;struct Cyc_Absyn_Vardecl*_tmp48A=(_tmpBFF=_cycalloc(sizeof(*_tmpBFF)),((
_tmpBFF->sc=Cyc_Absyn_Public,((_tmpBFF->name=_tmp489,((_tmpBFF->tq=Cyc_Toc_mt_tq,((
_tmpBFF->type=_tmp46F,((_tmpBFF->initializer=(struct Cyc_Absyn_Exp*)_tmp487,((
_tmpBFF->rgn=0,((_tmpBFF->attributes=0,((_tmpBFF->escapes=0,_tmpBFF)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpC02;struct Cyc_Absyn_Local_b_struct*_tmpC01;
struct Cyc_Absyn_Local_b_struct*_tmp48B=(_tmpC01=_cycalloc(sizeof(*_tmpC01)),((
_tmpC01[0]=((_tmpC02.tag=4,((_tmpC02.f1=_tmp48A,_tmpC02)))),_tmpC01)));struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp48C=Cyc_Absyn_varb_exp(_tmp489,(
void*)_tmp48B,0);_tmp48C->topt=_tmp487->topt;z_init=Cyc_Absyn_prim2_exp((enum 
Cyc_Absyn_Primop)popt->v,_tmp48C,e2,0);z_init->topt=_tmp48C->topt;}Cyc_Toc_exp_to_c(
_tmp482,z_init);{struct _tuple0*_tmp48D=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmpC03;struct Cyc_Absyn_Vardecl*_tmp48E=(_tmpC03=_cycalloc(sizeof(*_tmpC03)),((
_tmpC03->sc=Cyc_Absyn_Public,((_tmpC03->name=_tmp48D,((_tmpC03->tq=Cyc_Toc_mt_tq,((
_tmpC03->type=_tmp46F,((_tmpC03->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmpC03->rgn=0,((_tmpC03->attributes=0,((_tmpC03->escapes=0,_tmpC03)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpC06;struct Cyc_Absyn_Local_b_struct*_tmpC05;
struct Cyc_Absyn_Local_b_struct*_tmp48F=(_tmpC05=_cycalloc(sizeof(*_tmpC05)),((
_tmpC05[0]=((_tmpC06.tag=4,((_tmpC06.f1=_tmp48E,_tmpC06)))),_tmpC05)));_tmp482=
Cyc_Toc_add_varmap(_tmp481,_tmp482,_tmp48D,Cyc_Absyn_var_exp(_tmp48D,0));{struct
Cyc_Absyn_Exp*_tmp490=Cyc_Absyn_varb_exp(_tmp489,(void*)_tmp48B,0);_tmp490->topt=
_tmp487->topt;{struct Cyc_Absyn_Exp*_tmp491=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp492=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp490,_tmp491,0);{
struct Cyc_Core_Opt*_tmpC07;_tmp492->topt=((_tmpC07=_cycalloc(sizeof(*_tmpC07)),((
_tmpC07->v=Cyc_Absyn_sint_typ,_tmpC07))));}Cyc_Toc_exp_to_c(_tmp482,_tmp492);{
struct Cyc_Absyn_Exp*_tmp494=Cyc_Absyn_varb_exp(_tmp48D,(void*)_tmp48F,0);_tmp494->topt=
_tmp487->topt;{struct Cyc_Absyn_Exp*_tmp495=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp496=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp494,_tmp495,0);{
struct Cyc_Core_Opt*_tmpC08;_tmp496->topt=((_tmpC08=_cycalloc(sizeof(*_tmpC08)),((
_tmpC08->v=Cyc_Absyn_sint_typ,_tmpC08))));}Cyc_Toc_exp_to_c(_tmp482,_tmp496);{
struct Cyc_Absyn_Exp*_tmpC09[2];struct Cyc_List_List*_tmp498=(_tmpC09[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0),((_tmpC09[0]=Cyc_Absyn_varb_exp(_tmp47E,(void*)
_tmp484,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC09,sizeof(struct Cyc_Absyn_Exp*),2)))));struct Cyc_Absyn_Exp*
_tmp499=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;xsize=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp498,0),
_tmp499,0);{struct Cyc_Absyn_Exp*_tmp49A=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp492,_tmp496,0),0);struct Cyc_Absyn_Stmt*_tmp49B=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp49C=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp47E,(void*)_tmp484,0),Cyc_Toc_curr_sp,0);_tmp49C=Cyc_Toc_cast_it(
_tmp471,_tmp49C);{struct Cyc_Absyn_Exp*_tmp49D=Cyc_Absyn_deref_exp(_tmp49C,0);
struct Cyc_Absyn_Exp*_tmp49E=Cyc_Absyn_assign_exp(_tmp49D,Cyc_Absyn_var_exp(
_tmp48D,0),0);struct Cyc_Absyn_Stmt*_tmp49F=Cyc_Absyn_exp_stmt(_tmp49E,0);_tmp49F=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp49A,_tmp49B,Cyc_Absyn_skip_stmt(
0),0),_tmp49F,0);{struct Cyc_Absyn_Var_d_struct*_tmpC0F;struct Cyc_Absyn_Var_d_struct
_tmpC0E;struct Cyc_Absyn_Decl*_tmpC0D;_tmp49F=Cyc_Absyn_decl_stmt(((_tmpC0D=
_cycalloc(sizeof(*_tmpC0D)),((_tmpC0D->r=(void*)((_tmpC0F=_cycalloc(sizeof(*
_tmpC0F)),((_tmpC0F[0]=((_tmpC0E.tag=0,((_tmpC0E.f1=_tmp48E,_tmpC0E)))),_tmpC0F)))),((
_tmpC0D->loc=0,_tmpC0D)))))),_tmp49F,0);}{struct Cyc_Absyn_Var_d_struct*_tmpC15;
struct Cyc_Absyn_Var_d_struct _tmpC14;struct Cyc_Absyn_Decl*_tmpC13;_tmp49F=Cyc_Absyn_decl_stmt(((
_tmpC13=_cycalloc(sizeof(*_tmpC13)),((_tmpC13->r=(void*)((_tmpC15=_cycalloc(
sizeof(*_tmpC15)),((_tmpC15[0]=((_tmpC14.tag=0,((_tmpC14.f1=_tmp48A,_tmpC14)))),
_tmpC15)))),((_tmpC13->loc=0,_tmpC13)))))),_tmp49F,0);}{struct Cyc_Absyn_Var_d_struct*
_tmpC1B;struct Cyc_Absyn_Var_d_struct _tmpC1A;struct Cyc_Absyn_Decl*_tmpC19;_tmp49F=
Cyc_Absyn_decl_stmt(((_tmpC19=_cycalloc(sizeof(*_tmpC19)),((_tmpC19->r=(void*)((
_tmpC1B=_cycalloc(sizeof(*_tmpC1B)),((_tmpC1B[0]=((_tmpC1A.tag=0,((_tmpC1A.f1=
_tmp483,_tmpC1A)))),_tmpC1B)))),((_tmpC19->loc=0,_tmpC19)))))),_tmp49F,0);}e->r=
Cyc_Toc_stmt_exp_r(_tmp49F);};};};};};};};};};};};};};}static void*Cyc_Toc_check_tagged_union(
struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int
in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,
struct Cyc_Position_Segment*)){struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp4B4=Cyc_Tcutil_compress(
aggrtype);struct Cyc_Absyn_AggrInfo _tmp4B6;union Cyc_Absyn_AggrInfoU _tmp4B7;_LL20C: {
struct Cyc_Absyn_AggrType_struct*_tmp4B5=(struct Cyc_Absyn_AggrType_struct*)
_tmp4B4;if(_tmp4B5->tag != 12)goto _LL20E;else{_tmp4B6=_tmp4B5->f1;_tmp4B7=_tmp4B6.aggr_info;}}
_LL20D: ad=Cyc_Absyn_get_known_aggrdecl(_tmp4B7);goto _LL20B;_LL20E:;_LL20F: {
struct Cyc_String_pa_struct _tmpC23;void*_tmpC22[1];const char*_tmpC21;void*_tmpC20;(
_tmpC20=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((
struct _dyneither_ptr)((_tmpC23.tag=0,((_tmpC23.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpC22[0]=& _tmpC23,Cyc_aprintf(((
_tmpC21="expecting union but found %s in check_tagged_union",_tag_dyneither(
_tmpC21,sizeof(char),51))),_tag_dyneither(_tmpC22,sizeof(void*),1)))))))),
_tag_dyneither(_tmpC20,sizeof(void*),0)));}_LL20B:;}{struct _tuple0*_tmp4BC=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4BD=Cyc_Absyn_var_exp(_tmp4BC,0);struct Cyc_Absyn_Exp*
_tmp4BE=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);if(in_lhs){
struct Cyc_Absyn_Exp*_tmp4BF=Cyc_Absyn_aggrarrow_exp(_tmp4BD,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4C0=Cyc_Absyn_neq_exp(_tmp4BF,_tmp4BE,0);struct Cyc_Absyn_Exp*
_tmp4C1=Cyc_Absyn_aggrarrow_exp(_tmp4BD,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Stmt*
_tmp4C2=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp4C1,0),0);struct Cyc_Absyn_Stmt*
_tmp4C3=Cyc_Absyn_ifthenelse_stmt(_tmp4C0,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);void*_tmp4C4=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp4C5=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);struct Cyc_Absyn_Stmt*_tmp4C6=
Cyc_Absyn_declare_stmt(_tmp4BC,_tmp4C4,(struct Cyc_Absyn_Exp*)_tmp4C5,Cyc_Absyn_seq_stmt(
_tmp4C3,_tmp4C2,0),0);return Cyc_Toc_stmt_exp_r(_tmp4C6);}else{struct Cyc_Absyn_Exp*
_tmp4C7=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp4BD,f,0),Cyc_Toc_tag_sp,0);struct
Cyc_Absyn_Exp*_tmp4C8=Cyc_Absyn_neq_exp(_tmp4C7,_tmp4BE,0);struct Cyc_Absyn_Exp*
_tmp4C9=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp4BD,f,0),Cyc_Toc_val_sp,0);struct
Cyc_Absyn_Stmt*_tmp4CA=Cyc_Absyn_exp_stmt(_tmp4C9,0);struct Cyc_Absyn_Stmt*
_tmp4CB=Cyc_Absyn_ifthenelse_stmt(_tmp4C8,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp4CC=Cyc_Absyn_declare_stmt(_tmp4BC,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp4CB,_tmp4CA,0),0);return Cyc_Toc_stmt_exp_r(
_tmp4CC);}};}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read){void*_tmp4CD=e->r;struct Cyc_Absyn_Exp*
_tmp4CF;struct Cyc_Absyn_Exp*_tmp4D1;struct _dyneither_ptr*_tmp4D2;int _tmp4D3;int*
_tmp4D4;struct Cyc_Absyn_Exp*_tmp4D6;struct _dyneither_ptr*_tmp4D7;int _tmp4D8;int*
_tmp4D9;_LL211: {struct Cyc_Absyn_Cast_e_struct*_tmp4CE=(struct Cyc_Absyn_Cast_e_struct*)
_tmp4CD;if(_tmp4CE->tag != 15)goto _LL213;else{_tmp4CF=_tmp4CE->f2;}}_LL212: {
const char*_tmpC26;void*_tmpC25;(_tmpC25=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC26="cast on lhs!",
_tag_dyneither(_tmpC26,sizeof(char),13))),_tag_dyneither(_tmpC25,sizeof(void*),0)));}
_LL213: {struct Cyc_Absyn_AggrMember_e_struct*_tmp4D0=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp4CD;if(_tmp4D0->tag != 22)goto _LL215;else{_tmp4D1=_tmp4D0->f1;_tmp4D2=_tmp4D0->f2;
_tmp4D3=_tmp4D0->f4;_tmp4D4=(int*)& _tmp4D0->f4;}}_LL214: {void*_tmp4DC=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4D1->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp4DE;union Cyc_Absyn_AggrInfoU _tmp4DF;_LL21A: {struct Cyc_Absyn_AggrType_struct*
_tmp4DD=(struct Cyc_Absyn_AggrType_struct*)_tmp4DC;if(_tmp4DD->tag != 12)goto
_LL21C;else{_tmp4DE=_tmp4DD->f1;_tmp4DF=_tmp4DE.aggr_info;}}_LL21B: {struct Cyc_Absyn_Aggrdecl*
_tmp4E0=Cyc_Absyn_get_known_aggrdecl(_tmp4DF);*f_tag=Cyc_Toc_get_member_offset(
_tmp4E0,_tmp4D2);{const char*_tmpC2B;void*_tmpC2A[2];struct Cyc_String_pa_struct
_tmpC29;struct Cyc_String_pa_struct _tmpC28;struct _dyneither_ptr str=(struct
_dyneither_ptr)((_tmpC28.tag=0,((_tmpC28.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp4D2),((_tmpC29.tag=0,((_tmpC29.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp4E0->name).f2),((_tmpC2A[0]=& _tmpC29,((_tmpC2A[1]=&
_tmpC28,Cyc_aprintf(((_tmpC2B="_union_%s_%s",_tag_dyneither(_tmpC2B,sizeof(char),
13))),_tag_dyneither(_tmpC2A,sizeof(void*),2))))))))))))));{struct _dyneither_ptr*
_tmpC2C;*tagged_member_type=Cyc_Absyn_strct(((_tmpC2C=_cycalloc(sizeof(*_tmpC2C)),((
_tmpC2C[0]=str,_tmpC2C)))));}if(clear_read)*_tmp4D4=0;return((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4E0->impl))->tagged;};}_LL21C:;_LL21D: return 0;_LL219:;}_LL215: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp4D5=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp4CD;if(_tmp4D5->tag != 23)goto _LL217;else{_tmp4D6=_tmp4D5->f1;_tmp4D7=_tmp4D5->f2;
_tmp4D8=_tmp4D5->f4;_tmp4D9=(int*)& _tmp4D5->f4;}}_LL216: {void*_tmp4E6=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4D6->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp4E8;void*_tmp4E9;_LL21F: {struct Cyc_Absyn_PointerType_struct*_tmp4E7=(struct
Cyc_Absyn_PointerType_struct*)_tmp4E6;if(_tmp4E7->tag != 5)goto _LL221;else{
_tmp4E8=_tmp4E7->f1;_tmp4E9=_tmp4E8.elt_typ;}}_LL220: {void*_tmp4EA=Cyc_Tcutil_compress(
_tmp4E9);struct Cyc_Absyn_AggrInfo _tmp4EC;union Cyc_Absyn_AggrInfoU _tmp4ED;_LL224: {
struct Cyc_Absyn_AggrType_struct*_tmp4EB=(struct Cyc_Absyn_AggrType_struct*)
_tmp4EA;if(_tmp4EB->tag != 12)goto _LL226;else{_tmp4EC=_tmp4EB->f1;_tmp4ED=_tmp4EC.aggr_info;}}
_LL225: {struct Cyc_Absyn_Aggrdecl*_tmp4EE=Cyc_Absyn_get_known_aggrdecl(_tmp4ED);*
f_tag=Cyc_Toc_get_member_offset(_tmp4EE,_tmp4D7);{const char*_tmpC31;void*_tmpC30[
2];struct Cyc_String_pa_struct _tmpC2F;struct Cyc_String_pa_struct _tmpC2E;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpC2E.tag=0,((_tmpC2E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp4D7),((_tmpC2F.tag=0,((_tmpC2F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4EE->name).f2),((_tmpC30[0]=&
_tmpC2F,((_tmpC30[1]=& _tmpC2E,Cyc_aprintf(((_tmpC31="_union_%s_%s",
_tag_dyneither(_tmpC31,sizeof(char),13))),_tag_dyneither(_tmpC30,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpC32;*tagged_member_type=Cyc_Absyn_strct(((_tmpC32=
_cycalloc(sizeof(*_tmpC32)),((_tmpC32[0]=str,_tmpC32)))));}if(clear_read)*
_tmp4D9=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4EE->impl))->tagged;};}
_LL226:;_LL227: return 0;_LL223:;}_LL221:;_LL222: return 0;_LL21E:;}_LL217:;_LL218:
return 0;_LL210:;}void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*
e,int tag,void*mem_type){struct _tuple0*_tmp4F4=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
temp_exp=Cyc_Absyn_var_exp(_tmp4F4,0);struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(
tag,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,
0),0);struct Cyc_Absyn_Exp*_tmp4F5=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_ifthenelse_stmt(_tmp4F5,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4F4,Cyc_Absyn_cstar_typ(
mem_type,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(
e)),Cyc_Absyn_seq_stmt(s2,s3,0),0);e->r=Cyc_Toc_stmt_exp_r(s1);}static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type){struct _tuple0*_tmp4F6=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4F6,0);struct Cyc_Absyn_Exp*
temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*
temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 
0)s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*
_tmp4F7=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp4F7,
Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=
Cyc_Absyn_declare_stmt(_tmp4F6,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(e1),Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple20{struct _tuple0*f1;void*f2;struct
Cyc_Absyn_Exp*f3;};struct _tuple21{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*
f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void _tmpC85(unsigned int*_tmpC84,unsigned int*_tmpC83,struct _tuple0***
_tmpC81){for(*_tmpC84=0;*_tmpC84 < *_tmpC83;(*_tmpC84)++){(*_tmpC81)[*_tmpC84]=
Cyc_Toc_temp_var();}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){void*_tmp4F8=e->r;if(e->topt == 0){const char*_tmpC36;void*_tmpC35[1];struct Cyc_String_pa_struct
_tmpC34;(_tmpC34.tag=0,((_tmpC34.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpC35[0]=& _tmpC34,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC36="exp_to_c: no type for %s",
_tag_dyneither(_tmpC36,sizeof(char),25))),_tag_dyneither(_tmpC35,sizeof(void*),1)))))));}{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp4FC=
_tmp4F8;union Cyc_Absyn_Cnst _tmp4FE;int _tmp4FF;struct _tuple0*_tmp502;void*_tmp503;
struct _tuple0*_tmp505;enum Cyc_Absyn_Primop _tmp507;struct Cyc_List_List*_tmp508;
struct Cyc_Absyn_Exp*_tmp50A;enum Cyc_Absyn_Incrementor _tmp50B;struct Cyc_Absyn_Exp*
_tmp50D;struct Cyc_Core_Opt*_tmp50E;struct Cyc_Absyn_Exp*_tmp50F;struct Cyc_Absyn_Exp*
_tmp511;struct Cyc_Absyn_Exp*_tmp512;struct Cyc_Absyn_Exp*_tmp513;struct Cyc_Absyn_Exp*
_tmp515;struct Cyc_Absyn_Exp*_tmp516;struct Cyc_Absyn_Exp*_tmp518;struct Cyc_Absyn_Exp*
_tmp519;struct Cyc_Absyn_Exp*_tmp51B;struct Cyc_Absyn_Exp*_tmp51C;struct Cyc_Absyn_Exp*
_tmp51E;struct Cyc_List_List*_tmp51F;struct Cyc_Absyn_Exp*_tmp521;struct Cyc_List_List*
_tmp522;struct Cyc_Absyn_VarargCallInfo*_tmp523;struct Cyc_Absyn_Exp*_tmp525;
struct Cyc_List_List*_tmp526;struct Cyc_Absyn_VarargCallInfo*_tmp527;struct Cyc_Absyn_VarargCallInfo
_tmp528;int _tmp529;struct Cyc_List_List*_tmp52A;struct Cyc_Absyn_VarargInfo*
_tmp52B;struct Cyc_Absyn_Exp*_tmp52D;struct Cyc_Absyn_Exp*_tmp52F;struct Cyc_Absyn_Exp*
_tmp531;struct Cyc_List_List*_tmp532;void*_tmp534;void**_tmp535;struct Cyc_Absyn_Exp*
_tmp536;int _tmp537;enum Cyc_Absyn_Coercion _tmp538;struct Cyc_Absyn_Exp*_tmp53A;
struct Cyc_Absyn_Exp*_tmp53C;struct Cyc_Absyn_Exp*_tmp53D;struct Cyc_Absyn_Exp*
_tmp53F;void*_tmp541;void*_tmp543;void*_tmp544;struct _dyneither_ptr*_tmp546;void*
_tmp548;void*_tmp549;unsigned int _tmp54B;struct Cyc_Absyn_Exp*_tmp54D;struct Cyc_Absyn_Exp*
_tmp54F;struct _dyneither_ptr*_tmp550;int _tmp551;int _tmp552;struct Cyc_Absyn_Exp*
_tmp554;struct _dyneither_ptr*_tmp555;int _tmp556;int _tmp557;struct Cyc_Absyn_Exp*
_tmp559;struct Cyc_Absyn_Exp*_tmp55A;struct Cyc_List_List*_tmp55C;struct Cyc_List_List*
_tmp55E;struct Cyc_Absyn_Vardecl*_tmp560;struct Cyc_Absyn_Exp*_tmp561;struct Cyc_Absyn_Exp*
_tmp562;int _tmp563;struct _tuple0*_tmp565;struct Cyc_List_List*_tmp566;struct Cyc_List_List*
_tmp567;struct Cyc_Absyn_Aggrdecl*_tmp568;void*_tmp56A;struct Cyc_List_List*
_tmp56B;struct Cyc_List_List*_tmp56D;struct Cyc_Absyn_Datatypedecl*_tmp56E;struct
Cyc_Absyn_Datatypefield*_tmp56F;struct Cyc_Absyn_MallocInfo _tmp573;int _tmp574;
struct Cyc_Absyn_Exp*_tmp575;void**_tmp576;struct Cyc_Absyn_Exp*_tmp577;int _tmp578;
struct Cyc_Absyn_Exp*_tmp57A;struct Cyc_Absyn_Exp*_tmp57B;struct Cyc_Absyn_Exp*
_tmp57D;struct _dyneither_ptr*_tmp57E;struct Cyc_Absyn_Stmt*_tmp580;_LL229: {
struct Cyc_Absyn_Const_e_struct*_tmp4FD=(struct Cyc_Absyn_Const_e_struct*)_tmp4FC;
if(_tmp4FD->tag != 0)goto _LL22B;else{_tmp4FE=_tmp4FD->f1;if((_tmp4FE.Null_c).tag
!= 1)goto _LL22B;_tmp4FF=(int)(_tmp4FE.Null_c).val;}}_LL22A: {struct Cyc_Absyn_Exp*
_tmp585=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp585,
_tmp585))->r;else{struct Cyc_Absyn_Exp*_tmpC37[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((
_tmpC37[2]=_tmp585,((_tmpC37[1]=_tmp585,((_tmpC37[0]=_tmp585,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC37,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL228;}_LL22B: {struct Cyc_Absyn_Const_e_struct*
_tmp500=(struct Cyc_Absyn_Const_e_struct*)_tmp4FC;if(_tmp500->tag != 0)goto _LL22D;}
_LL22C: goto _LL228;_LL22D: {struct Cyc_Absyn_Var_e_struct*_tmp501=(struct Cyc_Absyn_Var_e_struct*)
_tmp4FC;if(_tmp501->tag != 1)goto _LL22F;else{_tmp502=_tmp501->f1;_tmp503=(void*)
_tmp501->f2;}}_LL22E:{struct _handler_cons _tmp587;_push_handler(& _tmp587);{int
_tmp589=0;if(setjmp(_tmp587.handler))_tmp589=1;if(!_tmp589){e->r=(Cyc_Toc_lookup_varmap(
nv,_tmp502))->r;;_pop_handler();}else{void*_tmp588=(void*)_exn_thrown;void*
_tmp58B=_tmp588;_LL282: {struct Cyc_Dict_Absent_struct*_tmp58C=(struct Cyc_Dict_Absent_struct*)
_tmp58B;if(_tmp58C->tag != Cyc_Dict_Absent)goto _LL284;}_LL283: {const char*_tmpC3B;
void*_tmpC3A[1];struct Cyc_String_pa_struct _tmpC39;(_tmpC39.tag=0,((_tmpC39.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp502)),((
_tmpC3A[0]=& _tmpC39,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC3B="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpC3B,sizeof(char),
32))),_tag_dyneither(_tmpC3A,sizeof(void*),1)))))));}_LL284:;_LL285:(void)_throw(
_tmp58B);_LL281:;}};}goto _LL228;_LL22F: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp504=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp4FC;if(_tmp504->tag != 2)goto
_LL231;else{_tmp505=_tmp504->f1;}}_LL230: {const char*_tmpC3E;void*_tmpC3D;(
_tmpC3D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC3E="unknownid",_tag_dyneither(_tmpC3E,sizeof(char),10))),_tag_dyneither(
_tmpC3D,sizeof(void*),0)));}_LL231: {struct Cyc_Absyn_Primop_e_struct*_tmp506=(
struct Cyc_Absyn_Primop_e_struct*)_tmp4FC;if(_tmp506->tag != 3)goto _LL233;else{
_tmp507=_tmp506->f1;_tmp508=_tmp506->f2;}}_LL232: {struct Cyc_List_List*_tmp592=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp508);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp508);switch(_tmp507){case Cyc_Absyn_Numelts: _LL286: {
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp508))->hd;{void*_tmp593=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo _tmp595;struct Cyc_Absyn_Exp*
_tmp596;struct Cyc_Absyn_PtrInfo _tmp598;void*_tmp599;struct Cyc_Absyn_PtrAtts
_tmp59A;union Cyc_Absyn_Constraint*_tmp59B;union Cyc_Absyn_Constraint*_tmp59C;
union Cyc_Absyn_Constraint*_tmp59D;_LL289: {struct Cyc_Absyn_ArrayType_struct*
_tmp594=(struct Cyc_Absyn_ArrayType_struct*)_tmp593;if(_tmp594->tag != 9)goto
_LL28B;else{_tmp595=_tmp594->f1;_tmp596=_tmp595.num_elts;}}_LL28A: if(!Cyc_Evexp_c_can_eval((
struct Cyc_Absyn_Exp*)_check_null(_tmp596))){const char*_tmpC41;void*_tmpC40;(
_tmpC40=0,Cyc_Tcutil_terr(e->loc,((_tmpC41="can't calculate numelts",
_tag_dyneither(_tmpC41,sizeof(char),24))),_tag_dyneither(_tmpC40,sizeof(void*),0)));}
e->r=_tmp596->r;goto _LL288;_LL28B: {struct Cyc_Absyn_PointerType_struct*_tmp597=(
struct Cyc_Absyn_PointerType_struct*)_tmp593;if(_tmp597->tag != 5)goto _LL28D;else{
_tmp598=_tmp597->f1;_tmp599=_tmp598.elt_typ;_tmp59A=_tmp598.ptr_atts;_tmp59B=
_tmp59A.nullable;_tmp59C=_tmp59A.bounds;_tmp59D=_tmp59A.zero_term;}}_LL28C:{void*
_tmp5A0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp59C);struct Cyc_Absyn_Exp*_tmp5A3;_LL290: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5A1=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5A0;if(_tmp5A1->tag != 0)goto
_LL292;}_LL291:{struct Cyc_Absyn_Exp*_tmpC42[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,((
_tmpC42[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp599),0),((_tmpC42[0]=(
struct Cyc_Absyn_Exp*)_tmp508->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC42,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL28F;_LL292: {struct Cyc_Absyn_Upper_b_struct*_tmp5A2=(struct Cyc_Absyn_Upper_b_struct*)
_tmp5A0;if(_tmp5A2->tag != 1)goto _LL28F;else{_tmp5A3=_tmp5A2->f1;}}_LL293: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp59D)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp508->hd);struct Cyc_Absyn_Exp*_tmpC43[2];e->r=Cyc_Toc_fncall_exp_r(
function_e,((_tmpC43[1]=_tmp5A3,((_tmpC43[0]=(struct Cyc_Absyn_Exp*)_tmp508->hd,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC43,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp59B)){if(!Cyc_Evexp_c_can_eval(_tmp5A3)){const
char*_tmpC46;void*_tmpC45;(_tmpC45=0,Cyc_Tcutil_terr(e->loc,((_tmpC46="can't calculate numelts",
_tag_dyneither(_tmpC46,sizeof(char),24))),_tag_dyneither(_tmpC45,sizeof(void*),0)));}
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp5A3,Cyc_Absyn_uint_exp(0,0));}else{e->r=
_tmp5A3->r;goto _LL28F;}}goto _LL28F;_LL28F:;}goto _LL288;_LL28D:;_LL28E: {const
char*_tmpC4B;void*_tmpC4A[2];struct Cyc_String_pa_struct _tmpC49;struct Cyc_String_pa_struct
_tmpC48;(_tmpC48.tag=0,((_tmpC48.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((
_tmpC49.tag=0,((_tmpC49.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmpC4A[0]=& _tmpC49,((
_tmpC4A[1]=& _tmpC48,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC4B="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpC4B,
sizeof(char),41))),_tag_dyneither(_tmpC4A,sizeof(void*),2)))))))))))));}_LL288:;}
break;}case Cyc_Absyn_Plus: _LL287:{void*_tmp5AC=Cyc_Tcutil_compress((void*)((
struct Cyc_List_List*)_check_null(_tmp592))->hd);struct Cyc_Absyn_PtrInfo _tmp5AE;
void*_tmp5AF;struct Cyc_Absyn_PtrAtts _tmp5B0;union Cyc_Absyn_Constraint*_tmp5B1;
union Cyc_Absyn_Constraint*_tmp5B2;_LL296: {struct Cyc_Absyn_PointerType_struct*
_tmp5AD=(struct Cyc_Absyn_PointerType_struct*)_tmp5AC;if(_tmp5AD->tag != 5)goto
_LL298;else{_tmp5AE=_tmp5AD->f1;_tmp5AF=_tmp5AE.elt_typ;_tmp5B0=_tmp5AE.ptr_atts;
_tmp5B1=_tmp5B0.bounds;_tmp5B2=_tmp5B0.zero_term;}}_LL297:{void*_tmp5B3=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp5B1);struct Cyc_Absyn_Exp*_tmp5B6;_LL29B: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5B4=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5B3;if(_tmp5B4->tag != 0)goto
_LL29D;}_LL29C: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp508))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp508->tl))->hd;{struct Cyc_Absyn_Exp*_tmpC4C[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpC4C[2]=e2,((_tmpC4C[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5AF),0),((_tmpC4C[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC4C,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}goto _LL29A;}_LL29D: {struct Cyc_Absyn_Upper_b_struct*_tmp5B5=(struct
Cyc_Absyn_Upper_b_struct*)_tmp5B3;if(_tmp5B5->tag != 1)goto _LL29A;else{_tmp5B6=
_tmp5B5->f1;}}_LL29E: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp5B2)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp508))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp508->tl))->hd;struct Cyc_Absyn_Exp*_tmpC4D[3];e->r=(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
e1),((_tmpC4D[2]=e2,((_tmpC4D[1]=_tmp5B6,((_tmpC4D[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC4D,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r;}goto _LL29A;_LL29A:;}goto _LL295;_LL298:;_LL299: goto _LL295;
_LL295:;}break;case Cyc_Absyn_Minus: _LL294: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp592))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp508))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp508->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp592->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{struct Cyc_Absyn_Exp*_tmpC4E[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpC4E[2]=Cyc_Absyn_prim1_exp(
Cyc_Absyn_Minus,e2,0),((_tmpC4E[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpC4E[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC4E,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}break;}case Cyc_Absyn_Eq:
_LL29F: goto _LL2A0;case Cyc_Absyn_Neq: _LL2A0: goto _LL2A1;case Cyc_Absyn_Gt: _LL2A1:
goto _LL2A2;case Cyc_Absyn_Gte: _LL2A2: goto _LL2A3;case Cyc_Absyn_Lt: _LL2A3: goto
_LL2A4;case Cyc_Absyn_Lte: _LL2A4: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp508))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp508->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp592))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp592->tl))->hd;void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& elt_typ))e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,
0));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp,0));break;}default: _LL2A5: break;}goto _LL228;}_LL233: {
struct Cyc_Absyn_Increment_e_struct*_tmp509=(struct Cyc_Absyn_Increment_e_struct*)
_tmp4FC;if(_tmp509->tag != 5)goto _LL235;else{_tmp50A=_tmp509->f1;_tmp50B=_tmp509->f2;}}
_LL234: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp50A->topt))->v;
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;const char*_tmpC4F;struct _dyneither_ptr incr_str=(_tmpC4F="increment",
_tag_dyneither(_tmpC4F,sizeof(char),10));if(_tmp50B == Cyc_Absyn_PreDec  || 
_tmp50B == Cyc_Absyn_PostDec){const char*_tmpC50;incr_str=((_tmpC50="decrement",
_tag_dyneither(_tmpC50,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp50A,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpC54;void*_tmpC53[1];
struct Cyc_String_pa_struct _tmpC52;(_tmpC52.tag=0,((_tmpC52.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC53[0]=& _tmpC52,Cyc_Tcutil_terr(
e->loc,((_tmpC54="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpC54,sizeof(char),74))),_tag_dyneither(_tmpC53,sizeof(void*),1)))))));}{
const char*_tmpC57;void*_tmpC56;(_tmpC56=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC57="in-place inc/dec on zero-term",
_tag_dyneither(_tmpC57,sizeof(char),30))),_tag_dyneither(_tmpC56,sizeof(void*),0)));};}{
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp50A,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp5C0=Cyc_Absyn_signed_int_exp(
1,0);{struct Cyc_Core_Opt*_tmpC58;_tmp5C0->topt=((_tmpC58=_cycalloc(sizeof(*
_tmpC58)),((_tmpC58->v=Cyc_Absyn_sint_typ,_tmpC58))));}switch(_tmp50B){case Cyc_Absyn_PreInc:
_LL2A7:{struct Cyc_Absyn_AssignOp_e_struct _tmpC5E;struct Cyc_Core_Opt*_tmpC5D;
struct Cyc_Absyn_AssignOp_e_struct*_tmpC5C;e->r=(void*)((_tmpC5C=_cycalloc(
sizeof(*_tmpC5C)),((_tmpC5C[0]=((_tmpC5E.tag=4,((_tmpC5E.f1=_tmp50A,((_tmpC5E.f2=((
_tmpC5D=_cycalloc_atomic(sizeof(*_tmpC5D)),((_tmpC5D->v=(void*)Cyc_Absyn_Plus,
_tmpC5D)))),((_tmpC5E.f3=_tmp5C0,_tmpC5E)))))))),_tmpC5C))));}Cyc_Toc_exp_to_c(
nv,e);return;case Cyc_Absyn_PreDec: _LL2A8:{struct Cyc_Absyn_AssignOp_e_struct
_tmpC64;struct Cyc_Core_Opt*_tmpC63;struct Cyc_Absyn_AssignOp_e_struct*_tmpC62;e->r=(
void*)((_tmpC62=_cycalloc(sizeof(*_tmpC62)),((_tmpC62[0]=((_tmpC64.tag=4,((
_tmpC64.f1=_tmp50A,((_tmpC64.f2=((_tmpC63=_cycalloc_atomic(sizeof(*_tmpC63)),((
_tmpC63->v=(void*)Cyc_Absyn_Minus,_tmpC63)))),((_tmpC64.f3=_tmp5C0,_tmpC64)))))))),
_tmpC62))));}Cyc_Toc_exp_to_c(nv,e);return;default: _LL2A9:{const char*_tmpC68;
void*_tmpC67[1];struct Cyc_String_pa_struct _tmpC66;(_tmpC66.tag=0,((_tmpC66.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC67[0]=& _tmpC66,Cyc_Tcutil_terr(
e->loc,((_tmpC68="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpC68,sizeof(char),59))),_tag_dyneither(_tmpC67,sizeof(void*),1)))))));}{
const char*_tmpC6B;void*_tmpC6A;(_tmpC6A=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC6B="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpC6B,sizeof(char),34))),_tag_dyneither(_tmpC6A,sizeof(void*),0)));};}}
Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp50A);Cyc_Toc_set_lhs(nv,0);{void*
elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp50B == Cyc_Absyn_PostInc
 || _tmp50B == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp50B == Cyc_Absyn_PreDec  || _tmp50B == Cyc_Absyn_PostDec)change=- 1;{struct
Cyc_Absyn_Exp*_tmpC6C[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC6C[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpC6C[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpC6C[0]=Cyc_Toc_push_address_exp(_tmp50A),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC6C,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Toc_functionSet*_tmp5CE=_tmp50B == Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:&
Cyc_Toc__zero_arr_inplace_plus_post_functionSet;struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(
_tmp5CE,_tmp50A);struct Cyc_Absyn_Exp*_tmpC6D[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((
_tmpC6D[1]=Cyc_Absyn_signed_int_exp(1,0),((_tmpC6D[0]=Cyc_Toc_push_address_exp(
_tmp50A),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC6D,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(elt_typ == (
void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp50A)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(
_tmp50A,0,Cyc_Toc_incr_lvalue,_tmp50B);e->r=_tmp50A->r;}}}goto _LL228;};};}_LL235: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp50C=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp4FC;if(_tmp50C->tag != 4)goto _LL237;else{_tmp50D=_tmp50C->f1;_tmp50E=_tmp50C->f2;
_tmp50F=_tmp50C->f3;}}_LL236: {void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp50D->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp50F->topt))->v;int f_tag=0;void*tagged_member_struct_type=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Toc_is_tagged_union_project(_tmp50D,& f_tag,&
tagged_member_struct_type,1)){Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp50D);
Cyc_Toc_set_lhs(nv,0);Cyc_Toc_exp_to_c(nv,_tmp50F);e->r=Cyc_Toc_tagged_union_assignop(
_tmp50D,e1_old_typ,_tmp50E,_tmp50F,e2_old_typ,f_tag,tagged_member_struct_type);
return;}{void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(_tmp50D,& ptr_type,&
is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp50D,_tmp50E,
_tmp50F,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp50D);Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp50D);Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp50F);{int done=0;if(_tmp50E != 0){void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;switch((enum Cyc_Absyn_Primop)_tmp50E->v){case Cyc_Absyn_Plus: _LL2AB:
change=_tmp50F;break;case Cyc_Absyn_Minus: _LL2AC: change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,
_tmp50F,0);break;default: _LL2AD: {const char*_tmpC70;void*_tmpC6F;(_tmpC6F=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC70="bad t ? pointer arithmetic",_tag_dyneither(_tmpC70,sizeof(char),27))),
_tag_dyneither(_tmpC6F,sizeof(void*),0)));}}done=1;{struct Cyc_Absyn_Exp*_tmp5D3=
Cyc_Toc__dyneither_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*_tmpC71[3];e->r=Cyc_Toc_fncall_exp_r(
_tmp5D3,((_tmpC71[2]=change,((_tmpC71[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpC71[0]=Cyc_Toc_push_address_exp(_tmp50D),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC71,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))
switch((enum Cyc_Absyn_Primop)_tmp50E->v){case Cyc_Absyn_Plus: _LL2AF: done=1;{
struct Cyc_Absyn_Exp*_tmpC72[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp50D),((_tmpC72[1]=_tmp50F,((
_tmpC72[0]=_tmp50D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC72,sizeof(struct Cyc_Absyn_Exp*),2)))))));}break;default:
_LL2B0: {const char*_tmpC75;void*_tmpC74;(_tmpC74=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC75="bad zero-terminated pointer arithmetic",
_tag_dyneither(_tmpC75,sizeof(char),39))),_tag_dyneither(_tmpC74,sizeof(void*),0)));}}}}
if(!done){if(e1_poly)_tmp50F->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp50F->r,0));if(!Cyc_Absyn_is_lvalue(_tmp50D)){{struct _tuple19 _tmpC78;struct
_tuple19*_tmpC77;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple19*),struct _tuple19*f_env))
Cyc_Toc_lvalue_assign)(_tmp50D,0,Cyc_Toc_assignop_lvalue,((_tmpC77=_cycalloc(
sizeof(struct _tuple19)* 1),((_tmpC77[0]=((_tmpC78.f1=_tmp50E,((_tmpC78.f2=
_tmp50F,_tmpC78)))),_tmpC77)))));}e->r=_tmp50D->r;}}goto _LL228;};};};}_LL237: {
struct Cyc_Absyn_Conditional_e_struct*_tmp510=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp4FC;if(_tmp510->tag != 6)goto _LL239;else{_tmp511=_tmp510->f1;_tmp512=_tmp510->f2;
_tmp513=_tmp510->f3;}}_LL238: Cyc_Toc_exp_to_c(nv,_tmp511);Cyc_Toc_exp_to_c(nv,
_tmp512);Cyc_Toc_exp_to_c(nv,_tmp513);goto _LL228;_LL239: {struct Cyc_Absyn_And_e_struct*
_tmp514=(struct Cyc_Absyn_And_e_struct*)_tmp4FC;if(_tmp514->tag != 7)goto _LL23B;
else{_tmp515=_tmp514->f1;_tmp516=_tmp514->f2;}}_LL23A: Cyc_Toc_exp_to_c(nv,
_tmp515);Cyc_Toc_exp_to_c(nv,_tmp516);goto _LL228;_LL23B: {struct Cyc_Absyn_Or_e_struct*
_tmp517=(struct Cyc_Absyn_Or_e_struct*)_tmp4FC;if(_tmp517->tag != 8)goto _LL23D;
else{_tmp518=_tmp517->f1;_tmp519=_tmp517->f2;}}_LL23C: Cyc_Toc_exp_to_c(nv,
_tmp518);Cyc_Toc_exp_to_c(nv,_tmp519);goto _LL228;_LL23D: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp51A=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp4FC;if(_tmp51A->tag != 9)goto _LL23F;
else{_tmp51B=_tmp51A->f1;_tmp51C=_tmp51A->f2;}}_LL23E: Cyc_Toc_exp_to_c(nv,
_tmp51B);Cyc_Toc_exp_to_c(nv,_tmp51C);goto _LL228;_LL23F: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp51D=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp4FC;if(_tmp51D->tag != 10)goto
_LL241;else{_tmp51E=_tmp51D->f1;_tmp51F=_tmp51D->f2;}}_LL240: _tmp521=_tmp51E;
_tmp522=_tmp51F;goto _LL242;_LL241: {struct Cyc_Absyn_FnCall_e_struct*_tmp520=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp4FC;if(_tmp520->tag != 11)goto _LL243;else{
_tmp521=_tmp520->f1;_tmp522=_tmp520->f2;_tmp523=_tmp520->f3;if(_tmp523 != 0)goto
_LL243;}}_LL242: Cyc_Toc_exp_to_c(nv,_tmp521);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp522);goto _LL228;_LL243: {struct Cyc_Absyn_FnCall_e_struct*
_tmp524=(struct Cyc_Absyn_FnCall_e_struct*)_tmp4FC;if(_tmp524->tag != 11)goto
_LL245;else{_tmp525=_tmp524->f1;_tmp526=_tmp524->f2;_tmp527=_tmp524->f3;if(
_tmp527 == 0)goto _LL245;_tmp528=*_tmp527;_tmp529=_tmp528.num_varargs;_tmp52A=
_tmp528.injectors;_tmp52B=_tmp528.vai;}}_LL244: {struct Cyc_Toc_Env _tmp5DB;struct
_RegionHandle*_tmp5DC;struct Cyc_Toc_Env*_tmp5DA=nv;_tmp5DB=*_tmp5DA;_tmp5DC=
_tmp5DB.rgn;{struct _tuple0*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=
Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp529,0);void*cva_type=Cyc_Toc_typ_to_c(_tmp52B->type);void*
arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp526);int num_normargs=num_args - _tmp529;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp526=_tmp526->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp526))->hd);{
struct Cyc_List_List*_tmpC79;new_args=((_tmpC79=_cycalloc(sizeof(*_tmpC79)),((
_tmpC79->hd=(struct Cyc_Absyn_Exp*)_tmp526->hd,((_tmpC79->tl=new_args,_tmpC79))))));};}}{
struct Cyc_Absyn_Exp*_tmpC7C[3];struct Cyc_List_List*_tmpC7B;new_args=((_tmpC7B=
_cycalloc(sizeof(*_tmpC7B)),((_tmpC7B->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((
_tmpC7C[2]=num_varargs_exp,((_tmpC7C[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0),((
_tmpC7C[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC7C,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpC7B->tl=
new_args,_tmpC7B))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp525);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp525,new_args,0),0);if(_tmp52B->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp5E0=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp52B->type));struct Cyc_Absyn_DatatypeInfo _tmp5E2;union Cyc_Absyn_DatatypeInfoU
_tmp5E3;struct Cyc_Absyn_Datatypedecl**_tmp5E4;struct Cyc_Absyn_Datatypedecl*
_tmp5E5;_LL2B3: {struct Cyc_Absyn_DatatypeType_struct*_tmp5E1=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp5E0;if(_tmp5E1->tag != 3)goto _LL2B5;else{_tmp5E2=_tmp5E1->f1;_tmp5E3=_tmp5E2.datatype_info;
if((_tmp5E3.KnownDatatype).tag != 2)goto _LL2B5;_tmp5E4=(struct Cyc_Absyn_Datatypedecl**)(
_tmp5E3.KnownDatatype).val;_tmp5E5=*_tmp5E4;}}_LL2B4: tud=_tmp5E5;goto _LL2B2;
_LL2B5:;_LL2B6: {const char*_tmpC7F;void*_tmpC7E;(_tmpC7E=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC7F="toc: unknown datatype in vararg with inject",
_tag_dyneither(_tmpC7F,sizeof(char),44))),_tag_dyneither(_tmpC7E,sizeof(void*),0)));}
_LL2B2:;}{unsigned int _tmpC84;unsigned int _tmpC83;struct _dyneither_ptr _tmpC82;
struct _tuple0**_tmpC81;unsigned int _tmpC80;struct _dyneither_ptr vs=(_tmpC80=(
unsigned int)_tmp529,((_tmpC81=(struct _tuple0**)_region_malloc(_tmp5DC,
_check_times(sizeof(struct _tuple0*),_tmpC80)),((_tmpC82=_tag_dyneither(_tmpC81,
sizeof(struct _tuple0*),_tmpC80),((((_tmpC83=_tmpC80,_tmpC85(& _tmpC84,& _tmpC83,&
_tmpC81))),_tmpC82)))))));if(_tmp529 != 0){struct Cyc_List_List*_tmp5E8=0;{int i=
_tmp529 - 1;for(0;i >= 0;-- i){struct Cyc_List_List*_tmpC86;_tmp5E8=((_tmpC86=
_cycalloc(sizeof(*_tmpC86)),((_tmpC86->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct
_tuple0*),i)),0),0)),((_tmpC86->tl=_tmp5E8,_tmpC86))))));}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp5E8,0),s,0);{
int i=0;for(0;_tmp526 != 0;(((_tmp526=_tmp526->tl,_tmp52A=_tmp52A->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp526->hd;void*arg_type=(void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct _tuple0*var=*((struct
_tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield _tmp5EB;struct
_tuple0*_tmp5EC;struct Cyc_List_List*_tmp5ED;struct Cyc_Absyn_Datatypefield*
_tmp5EA=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp52A))->hd;_tmp5EB=*_tmp5EA;_tmp5EC=_tmp5EB.name;_tmp5ED=_tmp5EB.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)_check_null(
_tmp5ED))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_boxed_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp5EC),0),s,0);{const char*_tmpC87;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp5EC,((_tmpC87="_struct",_tag_dyneither(_tmpC87,
sizeof(char),8))))),0,s,0);};};}};}else{struct _tuple15*_tmpC88[3];struct Cyc_List_List*
_tmp5EF=(_tmpC88[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC88[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpC88[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC88,sizeof(struct _tuple15*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{{int i=0;for(0;_tmp526
!= 0;(_tmp526=_tmp526->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp526->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp526->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL228;}_LL245: {struct Cyc_Absyn_Throw_e_struct*_tmp52C=(struct Cyc_Absyn_Throw_e_struct*)
_tmp4FC;if(_tmp52C->tag != 12)goto _LL247;else{_tmp52D=_tmp52C->f1;}}_LL246: Cyc_Toc_exp_to_c(
nv,_tmp52D);e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(
_tmp52D),0))->r;goto _LL228;_LL247: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp52E=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp4FC;if(_tmp52E->tag != 13)
goto _LL249;else{_tmp52F=_tmp52E->f1;}}_LL248: Cyc_Toc_exp_to_c(nv,_tmp52F);goto
_LL228;_LL249: {struct Cyc_Absyn_Instantiate_e_struct*_tmp530=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp4FC;if(_tmp530->tag != 14)goto _LL24B;else{_tmp531=_tmp530->f1;_tmp532=_tmp530->f2;}}
_LL24A: Cyc_Toc_exp_to_c(nv,_tmp531);for(0;_tmp532 != 0;_tmp532=_tmp532->tl){enum 
Cyc_Absyn_KindQual _tmp5F5=(Cyc_Tcutil_typ_kind((void*)_tmp532->hd))->kind;if(
_tmp5F5 != Cyc_Absyn_EffKind  && _tmp5F5 != Cyc_Absyn_RgnKind){{void*_tmp5F6=Cyc_Tcutil_compress((
void*)_tmp532->hd);_LL2B8: {struct Cyc_Absyn_VarType_struct*_tmp5F7=(struct Cyc_Absyn_VarType_struct*)
_tmp5F6;if(_tmp5F7->tag != 2)goto _LL2BA;}_LL2B9: goto _LL2BB;_LL2BA: {struct Cyc_Absyn_DatatypeType_struct*
_tmp5F8=(struct Cyc_Absyn_DatatypeType_struct*)_tmp5F6;if(_tmp5F8->tag != 3)goto
_LL2BC;}_LL2BB: continue;_LL2BC:;_LL2BD: e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp531,0))->r;goto _LL2B7;
_LL2B7:;}break;}}goto _LL228;_LL24B: {struct Cyc_Absyn_Cast_e_struct*_tmp533=(
struct Cyc_Absyn_Cast_e_struct*)_tmp4FC;if(_tmp533->tag != 15)goto _LL24D;else{
_tmp534=(void**)& _tmp533->f1;_tmp535=(void**)((void**)& _tmp533->f1);_tmp536=
_tmp533->f2;_tmp537=_tmp533->f3;_tmp538=_tmp533->f4;}}_LL24C: {void*old_t2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp536->topt))->v;void*new_typ=*_tmp535;*
_tmp535=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp536);{struct _tuple18
_tmpC89;struct _tuple18 _tmp5FA=(_tmpC89.f1=Cyc_Tcutil_compress(old_t2),((_tmpC89.f2=
Cyc_Tcutil_compress(new_typ),_tmpC89)));void*_tmp5FB;struct Cyc_Absyn_PtrInfo
_tmp5FD;void*_tmp5FE;struct Cyc_Absyn_PtrInfo _tmp600;void*_tmp601;struct Cyc_Absyn_PtrInfo
_tmp603;void*_tmp604;_LL2BF: _tmp5FB=_tmp5FA.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp5FC=(struct Cyc_Absyn_PointerType_struct*)_tmp5FB;if(_tmp5FC->tag != 5)goto
_LL2C1;else{_tmp5FD=_tmp5FC->f1;}};_tmp5FE=_tmp5FA.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp5FF=(struct Cyc_Absyn_PointerType_struct*)_tmp5FE;if(_tmp5FF->tag != 5)goto
_LL2C1;else{_tmp600=_tmp5FF->f1;}};_LL2C0: {int _tmp606=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp5FD.ptr_atts).nullable);int _tmp607=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp600.ptr_atts).nullable);
void*_tmp608=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp5FD.ptr_atts).bounds);void*_tmp609=((void*(*)(void*y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp600.ptr_atts).bounds);
int _tmp60A=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp5FD.ptr_atts).zero_term);int _tmp60B=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp600.ptr_atts).zero_term);{struct _tuple18 _tmpC8A;
struct _tuple18 _tmp60D=(_tmpC8A.f1=_tmp608,((_tmpC8A.f2=_tmp609,_tmpC8A)));void*
_tmp60E;struct Cyc_Absyn_Exp*_tmp610;void*_tmp611;struct Cyc_Absyn_Exp*_tmp613;
void*_tmp614;struct Cyc_Absyn_Exp*_tmp616;void*_tmp617;void*_tmp619;void*_tmp61B;
struct Cyc_Absyn_Exp*_tmp61D;void*_tmp61E;void*_tmp620;_LL2C6: _tmp60E=_tmp60D.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp60F=(struct Cyc_Absyn_Upper_b_struct*)_tmp60E;
if(_tmp60F->tag != 1)goto _LL2C8;else{_tmp610=_tmp60F->f1;}};_tmp611=_tmp60D.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp612=(struct Cyc_Absyn_Upper_b_struct*)_tmp611;
if(_tmp612->tag != 1)goto _LL2C8;else{_tmp613=_tmp612->f1;}};_LL2C7: if((!Cyc_Evexp_c_can_eval(
_tmp610) || !Cyc_Evexp_c_can_eval(_tmp613)) && !Cyc_Evexp_same_const_exp(_tmp610,
_tmp613)){const char*_tmpC8D;void*_tmpC8C;(_tmpC8C=0,Cyc_Tcutil_terr(e->loc,((
_tmpC8D="can't validate cast due to potential size differences",_tag_dyneither(
_tmpC8D,sizeof(char),54))),_tag_dyneither(_tmpC8C,sizeof(void*),0)));}if(_tmp606
 && !_tmp607){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC90;void*_tmpC8F;(
_tmpC8F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC90="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC90,
sizeof(char),44))),_tag_dyneither(_tmpC8F,sizeof(void*),0)));}if(_tmp538 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC94;void*_tmpC93[1];struct Cyc_String_pa_struct _tmpC92;(_tmpC92.tag=
0,((_tmpC92.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpC93[0]=& _tmpC92,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpC94="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpC94,sizeof(char),41))),_tag_dyneither(_tmpC93,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp536);if(do_null_check){if(!_tmp537){
const char*_tmpC97;void*_tmpC96;(_tmpC96=0,Cyc_Tcutil_warn(e->loc,((_tmpC97="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpC97,sizeof(char),58))),_tag_dyneither(_tmpC96,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC98;e->r=Cyc_Toc_cast_it_r(*_tmp535,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC98=_cycalloc(sizeof(*_tmpC98)),((_tmpC98->hd=
_tmp536,((_tmpC98->tl=0,_tmpC98)))))),0));};}};}goto _LL2C5;_LL2C8: _tmp614=
_tmp60D.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp615=(struct Cyc_Absyn_Upper_b_struct*)
_tmp614;if(_tmp615->tag != 1)goto _LL2CA;else{_tmp616=_tmp615->f1;}};_tmp617=
_tmp60D.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp618=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp617;if(_tmp618->tag != 0)goto _LL2CA;};_LL2C9: if(!Cyc_Evexp_c_can_eval(_tmp616)){
const char*_tmpC9B;void*_tmpC9A;(_tmpC9A=0,Cyc_Tcutil_terr(e->loc,((_tmpC9B="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC9B,sizeof(char),71))),_tag_dyneither(_tmpC9A,sizeof(void*),0)));}
if(_tmp538 == Cyc_Absyn_NonNull_to_Null){const char*_tmpC9F;void*_tmpC9E[1];struct
Cyc_String_pa_struct _tmpC9D;(_tmpC9D.tag=0,((_tmpC9D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC9E[0]=& _tmpC9D,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC9F="conversion mis-classified as null-check: %s",
_tag_dyneither(_tmpC9F,sizeof(char),44))),_tag_dyneither(_tmpC9E,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){if((_tmp60A  && !(_tmp600.elt_tq).real_const) && !
_tmp60B)_tmp616=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp616,Cyc_Absyn_uint_exp(1,
0),0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp616,_tmp536))->r;}else{
struct Cyc_Absyn_Exp*_tmp631=Cyc_Toc__tag_dyneither_e;if(_tmp60A){struct _tuple0*
_tmp632=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp633=Cyc_Absyn_var_exp(_tmp632,
0);struct Cyc_Absyn_Exp*arg3;{void*_tmp634=_tmp536->r;union Cyc_Absyn_Cnst _tmp636;
struct _dyneither_ptr _tmp637;_LL2CF: {struct Cyc_Absyn_Const_e_struct*_tmp635=(
struct Cyc_Absyn_Const_e_struct*)_tmp634;if(_tmp635->tag != 0)goto _LL2D1;else{
_tmp636=_tmp635->f1;if((_tmp636.String_c).tag != 7)goto _LL2D1;_tmp637=(struct
_dyneither_ptr)(_tmp636.String_c).val;}}_LL2D0: arg3=_tmp616;goto _LL2CE;_LL2D1:;
_LL2D2:{struct Cyc_Absyn_Exp*_tmpCA0[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp536),((_tmpCA0[1]=_tmp616,((_tmpCA0[0]=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp633),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA0,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);}goto _LL2CE;_LL2CE:;}if(!_tmp60B  && !(_tmp600.elt_tq).real_const)arg3=
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*
_tmp639=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp600.elt_typ),0);struct Cyc_Absyn_Exp*
_tmpCA1[3];struct Cyc_Absyn_Exp*_tmp63A=Cyc_Absyn_fncall_exp(_tmp631,((_tmpCA1[2]=
arg3,((_tmpCA1[1]=_tmp639,((_tmpCA1[0]=_tmp633,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA1,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);struct Cyc_Absyn_Stmt*_tmp63B=Cyc_Absyn_exp_stmt(_tmp63A,0);_tmp63B=
Cyc_Absyn_declare_stmt(_tmp632,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)
_tmp536,_tmp63B,0);e->r=Cyc_Toc_stmt_exp_r(_tmp63B);};}else{struct Cyc_Absyn_Exp*
_tmpCA2[3];e->r=Cyc_Toc_fncall_exp_r(_tmp631,((_tmpCA2[2]=_tmp616,((_tmpCA2[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp600.elt_typ),0),((_tmpCA2[0]=
_tmp536,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCA2,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2C5;
_LL2CA: _tmp619=_tmp60D.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp61A=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp619;if(_tmp61A->tag != 0)goto _LL2CC;};_tmp61B=_tmp60D.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp61C=(struct Cyc_Absyn_Upper_b_struct*)_tmp61B;if(_tmp61C->tag != 1)goto _LL2CC;
else{_tmp61D=_tmp61C->f1;}};_LL2CB: if(!Cyc_Evexp_c_can_eval(_tmp61D)){const char*
_tmpCA5;void*_tmpCA4;(_tmpCA4=0,Cyc_Tcutil_terr(e->loc,((_tmpCA5="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpCA5,sizeof(char),71))),_tag_dyneither(_tmpCA4,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpCA8;void*_tmpCA7;(_tmpCA7=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCA8="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpCA8,sizeof(char),45))),_tag_dyneither(_tmpCA7,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp642=_tmp61D;if(_tmp60A  && !_tmp60B)_tmp642=Cyc_Absyn_add_exp(
_tmp61D,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp643=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpCA9[3];struct Cyc_Absyn_Exp*_tmp644=Cyc_Absyn_fncall_exp(
_tmp643,((_tmpCA9[2]=_tmp642,((_tmpCA9[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp5FD.elt_typ),0),((_tmpCA9[0]=_tmp536,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA9,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp607){struct Cyc_List_List*_tmpCAA;_tmp644->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_null_e,((_tmpCAA=_cycalloc(sizeof(*_tmpCAA)),((_tmpCAA->hd=Cyc_Absyn_copy_exp(
_tmp644),((_tmpCAA->tl=0,_tmpCAA)))))));}e->r=Cyc_Toc_cast_it_r(*_tmp535,_tmp644);
goto _LL2C5;};};_LL2CC: _tmp61E=_tmp60D.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp61F=(struct Cyc_Absyn_DynEither_b_struct*)_tmp61E;if(_tmp61F->tag != 0)goto
_LL2C5;};_tmp620=_tmp60D.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp621=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp620;if(_tmp621->tag != 0)goto _LL2C5;};_LL2CD:
DynCast: if((_tmp60A  && !_tmp60B) && !(_tmp600.elt_tq).real_const){if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpCAD;void*_tmpCAC;(_tmpCAC=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCAD="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpCAD,sizeof(char),70))),_tag_dyneither(_tmpCAC,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp649=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpCAE[3];e->r=Cyc_Toc_fncall_exp_r(_tmp649,((_tmpCAE[2]=Cyc_Absyn_uint_exp(1,0),((
_tmpCAE[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5FD.elt_typ),0),((
_tmpCAE[0]=_tmp536,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCAE,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}goto _LL2C5;
_LL2C5:;}goto _LL2BE;}_LL2C1: _tmp601=_tmp5FA.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp602=(struct Cyc_Absyn_PointerType_struct*)_tmp601;if(_tmp602->tag != 5)goto
_LL2C3;else{_tmp603=_tmp602->f1;}};_tmp604=_tmp5FA.f2;{struct Cyc_Absyn_IntType_struct*
_tmp605=(struct Cyc_Absyn_IntType_struct*)_tmp604;if(_tmp605->tag != 6)goto _LL2C3;};
_LL2C2:{void*_tmp64B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp603.ptr_atts).bounds);_LL2D4: {struct Cyc_Absyn_DynEither_b_struct*
_tmp64C=(struct Cyc_Absyn_DynEither_b_struct*)_tmp64B;if(_tmp64C->tag != 0)goto
_LL2D6;}_LL2D5: _tmp536->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp536->r,
_tmp536->loc),Cyc_Toc_curr_sp);goto _LL2D3;_LL2D6:;_LL2D7: goto _LL2D3;_LL2D3:;}
goto _LL2BE;_LL2C3:;_LL2C4: goto _LL2BE;_LL2BE:;}goto _LL228;}_LL24D: {struct Cyc_Absyn_Address_e_struct*
_tmp539=(struct Cyc_Absyn_Address_e_struct*)_tmp4FC;if(_tmp539->tag != 16)goto
_LL24F;else{_tmp53A=_tmp539->f1;}}_LL24E:{void*_tmp64D=_tmp53A->r;struct _tuple0*
_tmp64F;struct Cyc_List_List*_tmp650;struct Cyc_List_List*_tmp651;struct Cyc_List_List*
_tmp653;_LL2D9: {struct Cyc_Absyn_Aggregate_e_struct*_tmp64E=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp64D;if(_tmp64E->tag != 29)goto _LL2DB;else{_tmp64F=_tmp64E->f1;_tmp650=_tmp64E->f2;
_tmp651=_tmp64E->f3;}}_LL2DA: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpCB2;void*
_tmpCB1[1];struct Cyc_String_pa_struct _tmpCB0;(_tmpCB0.tag=0,((_tmpCB0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp53A->loc)),((
_tmpCB1[0]=& _tmpCB0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpCB2="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpCB2,
sizeof(char),42))),_tag_dyneither(_tmpCB1,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp53A->topt))->v,_tmp650 != 0,1,0,
_tmp651,_tmp64F))->r;goto _LL2D8;_LL2DB: {struct Cyc_Absyn_Tuple_e_struct*_tmp652=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp64D;if(_tmp652->tag != 25)goto _LL2DD;else{
_tmp653=_tmp652->f1;}}_LL2DC: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpCB6;void*
_tmpCB5[1];struct Cyc_String_pa_struct _tmpCB4;(_tmpCB4.tag=0,((_tmpCB4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp53A->loc)),((
_tmpCB5[0]=& _tmpCB4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpCB6="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpCB6,
sizeof(char),42))),_tag_dyneither(_tmpCB5,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_tuple(
nv,1,0,_tmp653))->r;goto _LL2D8;_LL2DD:;_LL2DE: Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(
nv,_tmp53A);Cyc_Toc_set_lhs(nv,0);if(!Cyc_Absyn_is_lvalue(_tmp53A)){((void(*)(
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
int),int f_env))Cyc_Toc_lvalue_assign)(_tmp53A,0,Cyc_Toc_address_lvalue,1);e->r=
Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmp53A);}goto _LL2D8;_LL2D8:;}goto _LL228;_LL24F: {struct Cyc_Absyn_New_e_struct*
_tmp53B=(struct Cyc_Absyn_New_e_struct*)_tmp4FC;if(_tmp53B->tag != 17)goto _LL251;
else{_tmp53C=_tmp53B->f1;_tmp53D=_tmp53B->f2;}}_LL250: if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCBA;void*_tmpCB9[1];struct Cyc_String_pa_struct _tmpCB8;(_tmpCB8.tag=
0,((_tmpCB8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
_tmp53D->loc)),((_tmpCB9[0]=& _tmpCB8,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCBA="%s: new at top-level",_tag_dyneither(
_tmpCBA,sizeof(char),21))),_tag_dyneither(_tmpCB9,sizeof(void*),1)))))));}{void*
_tmp65D=_tmp53D->r;struct Cyc_List_List*_tmp65F;struct Cyc_Absyn_Vardecl*_tmp661;
struct Cyc_Absyn_Exp*_tmp662;struct Cyc_Absyn_Exp*_tmp663;int _tmp664;struct _tuple0*
_tmp666;struct Cyc_List_List*_tmp667;struct Cyc_List_List*_tmp668;struct Cyc_Absyn_Aggrdecl*
_tmp669;struct Cyc_List_List*_tmp66B;_LL2E0: {struct Cyc_Absyn_Array_e_struct*
_tmp65E=(struct Cyc_Absyn_Array_e_struct*)_tmp65D;if(_tmp65E->tag != 27)goto _LL2E2;
else{_tmp65F=_tmp65E->f1;}}_LL2E1: {struct _tuple0*_tmp66C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp66D=Cyc_Absyn_var_exp(_tmp66C,0);struct Cyc_Absyn_Stmt*
_tmp66E=Cyc_Toc_init_array(nv,_tmp66D,_tmp65F,Cyc_Absyn_exp_stmt(_tmp66D,0));
void*old_elt_typ;{void*_tmp66F=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp671;void*_tmp672;struct Cyc_Absyn_Tqual _tmp673;struct Cyc_Absyn_PtrAtts _tmp674;
union Cyc_Absyn_Constraint*_tmp675;_LL2EB: {struct Cyc_Absyn_PointerType_struct*
_tmp670=(struct Cyc_Absyn_PointerType_struct*)_tmp66F;if(_tmp670->tag != 5)goto
_LL2ED;else{_tmp671=_tmp670->f1;_tmp672=_tmp671.elt_typ;_tmp673=_tmp671.elt_tq;
_tmp674=_tmp671.ptr_atts;_tmp675=_tmp674.zero_term;}}_LL2EC: old_elt_typ=_tmp672;
goto _LL2EA;_LL2ED:;_LL2EE: {const char*_tmpCBD;void*_tmpCBC;old_elt_typ=((_tmpCBC=
0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCBD="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(
_tmpCBD,sizeof(char),52))),_tag_dyneither(_tmpCBC,sizeof(void*),0))));}_LL2EA:;}{
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp678=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp679=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp65F),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp53C == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp679);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp53C;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp679);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp66C,_tmp678,(struct Cyc_Absyn_Exp*)e1,_tmp66E,0));goto
_LL2DF;};}_LL2E2: {struct Cyc_Absyn_Comprehension_e_struct*_tmp660=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp65D;if(_tmp660->tag != 28)goto _LL2E4;else{_tmp661=_tmp660->f1;_tmp662=_tmp660->f2;
_tmp663=_tmp660->f3;_tmp664=_tmp660->f4;}}_LL2E3: {int is_dyneither_ptr=0;{void*
_tmp67A=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp67C;void*_tmp67D;
struct Cyc_Absyn_Tqual _tmp67E;struct Cyc_Absyn_PtrAtts _tmp67F;union Cyc_Absyn_Constraint*
_tmp680;union Cyc_Absyn_Constraint*_tmp681;_LL2F0: {struct Cyc_Absyn_PointerType_struct*
_tmp67B=(struct Cyc_Absyn_PointerType_struct*)_tmp67A;if(_tmp67B->tag != 5)goto
_LL2F2;else{_tmp67C=_tmp67B->f1;_tmp67D=_tmp67C.elt_typ;_tmp67E=_tmp67C.elt_tq;
_tmp67F=_tmp67C.ptr_atts;_tmp680=_tmp67F.bounds;_tmp681=_tmp67F.zero_term;}}
_LL2F1: is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp680)== (void*)& Cyc_Absyn_DynEither_b_val;goto _LL2EF;
_LL2F2:;_LL2F3: {const char*_tmpCC0;void*_tmpCBF;(_tmpCBF=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCC0="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpCC0,sizeof(char),42))),_tag_dyneither(_tmpCBF,sizeof(void*),0)));}
_LL2EF:;}{struct _tuple0*max=Cyc_Toc_temp_var();struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp663->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp662);{struct Cyc_Absyn_Exp*_tmp684=
Cyc_Absyn_var_exp(max,0);if(_tmp664)_tmp684=Cyc_Absyn_add_exp(_tmp684,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp661,Cyc_Absyn_var_exp(max,0),_tmp663,_tmp664,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp686;struct _RegionHandle*_tmp687;struct Cyc_Toc_Env*_tmp685=
nv;_tmp686=*_tmp685;_tmp687=_tmp686.rgn;{struct _tuple20*_tmpCC3;struct Cyc_List_List*
_tmpCC2;struct Cyc_List_List*decls=(_tmpCC2=_region_malloc(_tmp687,sizeof(*
_tmpCC2)),((_tmpCC2->hd=((_tmpCC3=_region_malloc(_tmp687,sizeof(*_tmpCC3)),((
_tmpCC3->f1=max,((_tmpCC3->f2=Cyc_Absyn_uint_typ,((_tmpCC3->f3=(struct Cyc_Absyn_Exp*)
_tmp662,_tmpCC3)))))))),((_tmpCC2->tl=0,_tmpCC2)))));struct Cyc_Absyn_Exp*ai;if(
_tmp53C == 0  || Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*_tmpCC4[2];ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((_tmpCC4[1]=_tmp684,((
_tmpCC4[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCC4,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp53C;Cyc_Toc_exp_to_c(
nv,r);{struct Cyc_Absyn_Exp*_tmpCC5[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpCC5[1]=_tmp684,((_tmpCC5[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCC5,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct _tuple20*_tmpCC8;struct Cyc_List_List*
_tmpCC7;decls=((_tmpCC7=_region_malloc(_tmp687,sizeof(*_tmpCC7)),((_tmpCC7->hd=((
_tmpCC8=_region_malloc(_tmp687,sizeof(*_tmpCC8)),((_tmpCC8->f1=a,((_tmpCC8->f2=
ptr_typ,((_tmpCC8->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpCC8)))))))),((_tmpCC7->tl=
decls,_tmpCC7))))));}if(is_dyneither_ptr){struct _tuple0*_tmp68C=Cyc_Toc_temp_var();
void*_tmp68D=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp68E=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpCC9[3];struct Cyc_Absyn_Exp*_tmp68F=Cyc_Absyn_fncall_exp(
_tmp68E,((_tmpCC9[2]=_tmp684,((_tmpCC9[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((
_tmpCC9[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpCC9,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{
struct _tuple20*_tmpCCC;struct Cyc_List_List*_tmpCCB;decls=((_tmpCCB=
_region_malloc(_tmp687,sizeof(*_tmpCCB)),((_tmpCCB->hd=((_tmpCCC=_region_malloc(
_tmp687,sizeof(*_tmpCCC)),((_tmpCCC->f1=_tmp68C,((_tmpCCC->f2=_tmp68D,((_tmpCCC->f3=(
struct Cyc_Absyn_Exp*)_tmp68F,_tmpCCC)))))))),((_tmpCCB->tl=decls,_tmpCCB))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp68C,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp693=decls;for(0;_tmp693 != 0;_tmp693=_tmp693->tl){struct
_tuple0*_tmp695;void*_tmp696;struct Cyc_Absyn_Exp*_tmp697;struct _tuple20 _tmp694=*((
struct _tuple20*)_tmp693->hd);_tmp695=_tmp694.f1;_tmp696=_tmp694.f2;_tmp697=
_tmp694.f3;s=Cyc_Absyn_declare_stmt(_tmp695,_tmp696,_tmp697,s,0);}}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL2DF;};};};}_LL2E4: {struct Cyc_Absyn_Aggregate_e_struct*_tmp665=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp65D;if(_tmp665->tag != 29)goto _LL2E6;
else{_tmp666=_tmp665->f1;_tmp667=_tmp665->f2;_tmp668=_tmp665->f3;_tmp669=_tmp665->f4;}}
_LL2E5: e->r=(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp53D->topt))->v,_tmp667 != 0,1,_tmp53C,_tmp668,_tmp666))->r;goto _LL2DF;_LL2E6: {
struct Cyc_Absyn_Tuple_e_struct*_tmp66A=(struct Cyc_Absyn_Tuple_e_struct*)_tmp65D;
if(_tmp66A->tag != 25)goto _LL2E8;else{_tmp66B=_tmp66A->f1;}}_LL2E7: e->r=(Cyc_Toc_init_tuple(
nv,1,_tmp53C,_tmp66B))->r;goto _LL2DF;_LL2E8:;_LL2E9: {void*old_elt_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp53D->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);struct _tuple0*_tmp69A=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp69B=Cyc_Absyn_var_exp(_tmp69A,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(_tmp69B,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp53C
== 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp53C;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp69C=_tmp53D->r;void*_tmp69E;
struct Cyc_Absyn_Exp*_tmp69F;_LL2F5: {struct Cyc_Absyn_Cast_e_struct*_tmp69D=(
struct Cyc_Absyn_Cast_e_struct*)_tmp69C;if(_tmp69D->tag != 15)goto _LL2F7;else{
_tmp69E=(void*)_tmp69D->f1;_tmp69F=_tmp69D->f2;}}_LL2F6:{struct _tuple18 _tmpCCD;
struct _tuple18 _tmp6A1=(_tmpCCD.f1=Cyc_Tcutil_compress(_tmp69E),((_tmpCCD.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp69F->topt))->v),_tmpCCD)));void*
_tmp6A2;struct Cyc_Absyn_PtrInfo _tmp6A4;void*_tmp6A5;struct Cyc_Absyn_PtrAtts
_tmp6A6;union Cyc_Absyn_Constraint*_tmp6A7;void*_tmp6A8;struct Cyc_Absyn_PtrInfo
_tmp6AA;struct Cyc_Absyn_PtrAtts _tmp6AB;union Cyc_Absyn_Constraint*_tmp6AC;_LL2FA:
_tmp6A2=_tmp6A1.f1;{struct Cyc_Absyn_PointerType_struct*_tmp6A3=(struct Cyc_Absyn_PointerType_struct*)
_tmp6A2;if(_tmp6A3->tag != 5)goto _LL2FC;else{_tmp6A4=_tmp6A3->f1;_tmp6A5=_tmp6A4.elt_typ;
_tmp6A6=_tmp6A4.ptr_atts;_tmp6A7=_tmp6A6.bounds;}};_tmp6A8=_tmp6A1.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp6A9=(struct Cyc_Absyn_PointerType_struct*)_tmp6A8;if(_tmp6A9->tag != 5)goto
_LL2FC;else{_tmp6AA=_tmp6A9->f1;_tmp6AB=_tmp6AA.ptr_atts;_tmp6AC=_tmp6AB.bounds;}};
_LL2FB:{struct _tuple18 _tmpCCE;struct _tuple18 _tmp6AE=(_tmpCCE.f1=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6A7),((_tmpCCE.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp6AC),_tmpCCE)));void*_tmp6AF;void*_tmp6B1;struct Cyc_Absyn_Exp*
_tmp6B3;_LL2FF: _tmp6AF=_tmp6AE.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp6B0=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp6AF;if(_tmp6B0->tag != 0)goto _LL301;};
_tmp6B1=_tmp6AE.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp6B2=(struct Cyc_Absyn_Upper_b_struct*)
_tmp6B1;if(_tmp6B2->tag != 1)goto _LL301;else{_tmp6B3=_tmp6B2->f1;}};_LL300: Cyc_Toc_exp_to_c(
nv,_tmp69F);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp6B4=Cyc_Toc__init_dyneither_ptr_e;{struct Cyc_Absyn_Exp*_tmpCCF[4];e->r=Cyc_Toc_fncall_exp_r(
_tmp6B4,((_tmpCCF[3]=_tmp6B3,((_tmpCCF[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp6A5),0),((_tmpCCF[1]=_tmp69F,((_tmpCCF[0]=mexp,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCCF,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))));}goto _LL2FE;};_LL301:;_LL302: goto _LL2FE;_LL2FE:;}goto _LL2F9;_LL2FC:;
_LL2FD: goto _LL2F9;_LL2F9:;}goto _LL2F4;_LL2F7:;_LL2F8: goto _LL2F4;_LL2F4:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp6B6=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp69B),
0);struct Cyc_Absyn_Exp*_tmp6B7=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,
_tmp53D);_tmp6B6=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
_tmp69B,_tmp6B7,0),_tmp53D,0),_tmp6B6,0);{void*_tmp6B8=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp69A,
_tmp6B8,(struct Cyc_Absyn_Exp*)mexp,_tmp6B6,0));};}goto _LL2DF;};}_LL2DF:;}goto
_LL228;_LL251: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp53E=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp4FC;if(_tmp53E->tag != 19)goto _LL253;else{_tmp53F=_tmp53E->f1;}}_LL252: Cyc_Toc_exp_to_c(
nv,_tmp53F);goto _LL228;_LL253: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp540=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp4FC;if(_tmp540->tag != 18)goto _LL255;
else{_tmp541=(void*)_tmp540->f1;}}_LL254:{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpCD2;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpCD1;e->r=(void*)((_tmpCD1=
_cycalloc(sizeof(*_tmpCD1)),((_tmpCD1[0]=((_tmpCD2.tag=18,((_tmpCD2.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp541),_tmpCD2)))),_tmpCD1))));}goto _LL228;_LL255: {
struct Cyc_Absyn_Offsetof_e_struct*_tmp542=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp4FC;if(_tmp542->tag != 20)goto _LL257;else{_tmp543=(void*)_tmp542->f1;_tmp544=(
void*)_tmp542->f2;{struct Cyc_Absyn_StructField_struct*_tmp545=(struct Cyc_Absyn_StructField_struct*)
_tmp544;if(_tmp545->tag != 0)goto _LL257;else{_tmp546=_tmp545->f1;}};}}_LL256:{
struct Cyc_Absyn_Offsetof_e_struct _tmpCDC;struct Cyc_Absyn_StructField_struct
_tmpCDB;struct Cyc_Absyn_StructField_struct*_tmpCDA;struct Cyc_Absyn_Offsetof_e_struct*
_tmpCD9;e->r=(void*)((_tmpCD9=_cycalloc(sizeof(*_tmpCD9)),((_tmpCD9[0]=((_tmpCDC.tag=
20,((_tmpCDC.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp543),((_tmpCDC.f2=(void*)((
void*)((_tmpCDA=_cycalloc(sizeof(*_tmpCDA)),((_tmpCDA[0]=((_tmpCDB.tag=0,((
_tmpCDB.f1=_tmp546,_tmpCDB)))),_tmpCDA))))),_tmpCDC)))))),_tmpCD9))));}goto
_LL228;_LL257: {struct Cyc_Absyn_Offsetof_e_struct*_tmp547=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp4FC;if(_tmp547->tag != 20)goto _LL259;else{_tmp548=(void*)_tmp547->f1;_tmp549=(
void*)_tmp547->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp54A=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp549;if(_tmp54A->tag != 1)goto _LL259;else{_tmp54B=_tmp54A->f1;}};}}_LL258:{
void*_tmp6BF=Cyc_Tcutil_compress(_tmp548);struct Cyc_Absyn_AggrInfo _tmp6C1;union
Cyc_Absyn_AggrInfoU _tmp6C2;struct Cyc_List_List*_tmp6C4;_LL304: {struct Cyc_Absyn_AggrType_struct*
_tmp6C0=(struct Cyc_Absyn_AggrType_struct*)_tmp6BF;if(_tmp6C0->tag != 12)goto
_LL306;else{_tmp6C1=_tmp6C0->f1;_tmp6C2=_tmp6C1.aggr_info;}}_LL305: {struct Cyc_Absyn_Aggrdecl*
_tmp6C7=Cyc_Absyn_get_known_aggrdecl(_tmp6C2);if(_tmp6C7->impl == 0){const char*
_tmpCDF;void*_tmpCDE;(_tmpCDE=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCDF="struct fields must be known",
_tag_dyneither(_tmpCDF,sizeof(char),28))),_tag_dyneither(_tmpCDE,sizeof(void*),0)));}
_tmp6C4=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6C7->impl))->fields;goto
_LL307;}_LL306: {struct Cyc_Absyn_AnonAggrType_struct*_tmp6C3=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp6BF;if(_tmp6C3->tag != 13)goto _LL308;else{_tmp6C4=_tmp6C3->f2;}}_LL307: {
struct Cyc_Absyn_Aggrfield*_tmp6CA=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp6C4,(int)_tmp54B);{struct Cyc_Absyn_Offsetof_e_struct
_tmpCE9;struct Cyc_Absyn_StructField_struct _tmpCE8;struct Cyc_Absyn_StructField_struct*
_tmpCE7;struct Cyc_Absyn_Offsetof_e_struct*_tmpCE6;e->r=(void*)((_tmpCE6=
_cycalloc(sizeof(*_tmpCE6)),((_tmpCE6[0]=((_tmpCE9.tag=20,((_tmpCE9.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp548),((_tmpCE9.f2=(void*)((void*)((_tmpCE7=_cycalloc(
sizeof(*_tmpCE7)),((_tmpCE7[0]=((_tmpCE8.tag=0,((_tmpCE8.f1=_tmp6CA->name,
_tmpCE8)))),_tmpCE7))))),_tmpCE9)))))),_tmpCE6))));}goto _LL303;}_LL308: {struct
Cyc_Absyn_TupleType_struct*_tmp6C5=(struct Cyc_Absyn_TupleType_struct*)_tmp6BF;
if(_tmp6C5->tag != 11)goto _LL30A;}_LL309:{struct Cyc_Absyn_Offsetof_e_struct
_tmpCF3;struct Cyc_Absyn_StructField_struct _tmpCF2;struct Cyc_Absyn_StructField_struct*
_tmpCF1;struct Cyc_Absyn_Offsetof_e_struct*_tmpCF0;e->r=(void*)((_tmpCF0=
_cycalloc(sizeof(*_tmpCF0)),((_tmpCF0[0]=((_tmpCF3.tag=20,((_tmpCF3.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp548),((_tmpCF3.f2=(void*)((void*)((_tmpCF1=_cycalloc(
sizeof(*_tmpCF1)),((_tmpCF1[0]=((_tmpCF2.tag=0,((_tmpCF2.f1=Cyc_Absyn_fieldname((
int)(_tmp54B + 1)),_tmpCF2)))),_tmpCF1))))),_tmpCF3)))))),_tmpCF0))));}goto _LL303;
_LL30A: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp6C6=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp6BF;if(_tmp6C6->tag != 4)goto _LL30C;}_LL30B: if(_tmp54B == 0){struct Cyc_Absyn_Offsetof_e_struct
_tmpCFD;struct Cyc_Absyn_StructField_struct _tmpCFC;struct Cyc_Absyn_StructField_struct*
_tmpCFB;struct Cyc_Absyn_Offsetof_e_struct*_tmpCFA;e->r=(void*)((_tmpCFA=
_cycalloc(sizeof(*_tmpCFA)),((_tmpCFA[0]=((_tmpCFD.tag=20,((_tmpCFD.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp548),((_tmpCFD.f2=(void*)((void*)((_tmpCFB=_cycalloc(
sizeof(*_tmpCFB)),((_tmpCFB[0]=((_tmpCFC.tag=0,((_tmpCFC.f1=Cyc_Toc_tag_sp,
_tmpCFC)))),_tmpCFB))))),_tmpCFD)))))),_tmpCFA))));}else{struct Cyc_Absyn_Offsetof_e_struct
_tmpD07;struct Cyc_Absyn_StructField_struct _tmpD06;struct Cyc_Absyn_StructField_struct*
_tmpD05;struct Cyc_Absyn_Offsetof_e_struct*_tmpD04;e->r=(void*)((_tmpD04=
_cycalloc(sizeof(*_tmpD04)),((_tmpD04[0]=((_tmpD07.tag=20,((_tmpD07.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp548),((_tmpD07.f2=(void*)((void*)((_tmpD05=_cycalloc(
sizeof(*_tmpD05)),((_tmpD05[0]=((_tmpD06.tag=0,((_tmpD06.f1=Cyc_Absyn_fieldname((
int)_tmp54B),_tmpD06)))),_tmpD05))))),_tmpD07)))))),_tmpD04))));}goto _LL303;
_LL30C:;_LL30D: {const char*_tmpD0A;void*_tmpD09;(_tmpD09=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD0A="impossible type for offsetof tuple index",
_tag_dyneither(_tmpD0A,sizeof(char),41))),_tag_dyneither(_tmpD09,sizeof(void*),0)));}
_LL303:;}goto _LL228;_LL259: {struct Cyc_Absyn_Deref_e_struct*_tmp54C=(struct Cyc_Absyn_Deref_e_struct*)
_tmp4FC;if(_tmp54C->tag != 21)goto _LL25B;else{_tmp54D=_tmp54C->f1;}}_LL25A: {int
_tmp6DD=Cyc_Toc_in_lhs(nv);Cyc_Toc_set_lhs(nv,0);{void*_tmp6DE=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp54D->topt))->v);{void*_tmp6DF=
_tmp6DE;struct Cyc_Absyn_PtrInfo _tmp6E1;void*_tmp6E2;struct Cyc_Absyn_Tqual _tmp6E3;
struct Cyc_Absyn_PtrAtts _tmp6E4;void*_tmp6E5;union Cyc_Absyn_Constraint*_tmp6E6;
union Cyc_Absyn_Constraint*_tmp6E7;union Cyc_Absyn_Constraint*_tmp6E8;_LL30F: {
struct Cyc_Absyn_PointerType_struct*_tmp6E0=(struct Cyc_Absyn_PointerType_struct*)
_tmp6DF;if(_tmp6E0->tag != 5)goto _LL311;else{_tmp6E1=_tmp6E0->f1;_tmp6E2=_tmp6E1.elt_typ;
_tmp6E3=_tmp6E1.elt_tq;_tmp6E4=_tmp6E1.ptr_atts;_tmp6E5=_tmp6E4.rgn;_tmp6E6=
_tmp6E4.nullable;_tmp6E7=_tmp6E4.bounds;_tmp6E8=_tmp6E4.zero_term;}}_LL310:{void*
_tmp6E9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6E7);struct Cyc_Absyn_Exp*_tmp6EB;_LL314: {struct Cyc_Absyn_Upper_b_struct*
_tmp6EA=(struct Cyc_Absyn_Upper_b_struct*)_tmp6E9;if(_tmp6EA->tag != 1)goto _LL316;
else{_tmp6EB=_tmp6EA->f1;}}_LL315: {int do_null_check=Cyc_Toc_need_null_check(
_tmp54D);Cyc_Toc_exp_to_c(nv,_tmp54D);if(do_null_check){if(Cyc_Toc_warn_all_null_deref){
const char*_tmpD0D;void*_tmpD0C;(_tmpD0C=0,Cyc_Tcutil_warn(e->loc,((_tmpD0D="inserted null check due to dereference",
_tag_dyneither(_tmpD0D,sizeof(char),39))),_tag_dyneither(_tmpD0C,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpD0E;_tmp54D->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp6DE),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpD0E=_cycalloc(sizeof(*
_tmpD0E)),((_tmpD0E->hd=Cyc_Absyn_copy_exp(_tmp54D),((_tmpD0E->tl=0,_tmpD0E)))))),
0));};}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6E8)){unsigned int _tmp6F1;int _tmp6F2;struct _tuple10 _tmp6F0=Cyc_Evexp_eval_const_uint_exp(
_tmp6EB);_tmp6F1=_tmp6F0.f1;_tmp6F2=_tmp6F0.f2;if(!_tmp6F2  || _tmp6F1 <= 0){const
char*_tmpD11;void*_tmpD10;(_tmpD10=0,Cyc_Tcutil_terr(e->loc,((_tmpD11="cannot determine dereference is in bounds",
_tag_dyneither(_tmpD11,sizeof(char),42))),_tag_dyneither(_tmpD10,sizeof(void*),0)));}}
goto _LL313;}_LL316: {struct Cyc_Absyn_DynEither_b_struct*_tmp6EC=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp6E9;if(_tmp6EC->tag != 0)goto _LL313;}_LL317: {struct Cyc_Absyn_Exp*_tmp6F5=Cyc_Absyn_uint_exp(
0,0);{struct Cyc_Core_Opt*_tmpD12;_tmp6F5->topt=((_tmpD12=_cycalloc(sizeof(*
_tmpD12)),((_tmpD12->v=Cyc_Absyn_uint_typ,_tmpD12))));}e->r=Cyc_Toc_subscript_exp_r(
_tmp54D,_tmp6F5);Cyc_Toc_exp_to_c(nv,e);goto _LL313;}_LL313:;}goto _LL30E;_LL311:;
_LL312: {const char*_tmpD15;void*_tmpD14;(_tmpD14=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD15="exp_to_c: Deref: non-pointer",
_tag_dyneither(_tmpD15,sizeof(char),29))),_tag_dyneither(_tmpD14,sizeof(void*),0)));}
_LL30E:;}Cyc_Toc_set_lhs(nv,_tmp6DD);goto _LL228;};}_LL25B: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp54E=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp4FC;if(_tmp54E->tag != 22)goto
_LL25D;else{_tmp54F=_tmp54E->f1;_tmp550=_tmp54E->f2;_tmp551=_tmp54E->f3;_tmp552=
_tmp54E->f4;}}_LL25C: {int is_poly=Cyc_Toc_is_poly_project(e);void*e1_cyc_type=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp54F->topt))->v;Cyc_Toc_exp_to_c(nv,
_tmp54F);if(_tmp551  && _tmp552)e->r=Cyc_Toc_check_tagged_union(_tmp54F,Cyc_Toc_typ_to_c(
e1_cyc_type),e1_cyc_type,_tmp550,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrmember_exp);
if(is_poly)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp(e->r,0),0))->r;goto _LL228;}_LL25D: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp553=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp4FC;if(_tmp553->tag != 23)goto _LL25F;else{_tmp554=_tmp553->f1;_tmp555=_tmp553->f2;
_tmp556=_tmp553->f3;_tmp557=_tmp553->f4;}}_LL25E: {int _tmp6F9=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp554->topt))->v);int do_null_check=Cyc_Toc_need_null_check(_tmp554);
Cyc_Toc_exp_to_c(nv,_tmp554);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp6FB;
struct Cyc_Absyn_Tqual _tmp6FC;struct Cyc_Absyn_PtrAtts _tmp6FD;void*_tmp6FE;union
Cyc_Absyn_Constraint*_tmp6FF;union Cyc_Absyn_Constraint*_tmp700;union Cyc_Absyn_Constraint*
_tmp701;struct Cyc_Absyn_PtrInfo _tmp6FA=Cyc_Toc_get_ptr_type(e1typ);_tmp6FB=
_tmp6FA.elt_typ;_tmp6FC=_tmp6FA.elt_tq;_tmp6FD=_tmp6FA.ptr_atts;_tmp6FE=_tmp6FD.rgn;
_tmp6FF=_tmp6FD.nullable;_tmp700=_tmp6FD.bounds;_tmp701=_tmp6FD.zero_term;{void*
_tmp702=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp700);struct Cyc_Absyn_Exp*_tmp704;_LL319: {struct Cyc_Absyn_Upper_b_struct*
_tmp703=(struct Cyc_Absyn_Upper_b_struct*)_tmp702;if(_tmp703->tag != 1)goto _LL31B;
else{_tmp704=_tmp703->f1;}}_LL31A: {unsigned int _tmp707;int _tmp708;struct
_tuple10 _tmp706=Cyc_Evexp_eval_const_uint_exp(_tmp704);_tmp707=_tmp706.f1;
_tmp708=_tmp706.f2;if(_tmp708){if(_tmp707 < 1){const char*_tmpD18;void*_tmpD17;(
_tmpD17=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD18="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpD18,
sizeof(char),44))),_tag_dyneither(_tmpD17,sizeof(void*),0)));}if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){const char*_tmpD1B;void*_tmpD1A;(_tmpD1A=0,Cyc_Tcutil_warn(
e->loc,((_tmpD1B="inserted null check due to dereference",_tag_dyneither(_tmpD1B,
sizeof(char),39))),_tag_dyneither(_tmpD1A,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpD1C[1];_tmp554->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpD1C[0]=Cyc_Absyn_new_exp(_tmp554->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD1C,sizeof(struct Cyc_Absyn_Exp*),
1)))),0));};}}else{if(!Cyc_Evexp_c_can_eval(_tmp704)){const char*_tmpD1F;void*
_tmpD1E;(_tmpD1E=0,Cyc_Tcutil_terr(e->loc,((_tmpD1F="cannot determine pointer dereference in bounds",
_tag_dyneither(_tmpD1F,sizeof(char),47))),_tag_dyneither(_tmpD1E,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpD20[4];_tmp554->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((_tmpD20[3]=
Cyc_Absyn_uint_exp(0,0),((_tmpD20[2]=Cyc_Absyn_sizeoftyp_exp(_tmp6FB,0),((
_tmpD20[1]=_tmp704,((_tmpD20[0]=Cyc_Absyn_new_exp(_tmp554->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD20,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0));};}goto _LL318;}_LL31B: {struct Cyc_Absyn_DynEither_b_struct*
_tmp705=(struct Cyc_Absyn_DynEither_b_struct*)_tmp702;if(_tmp705->tag != 0)goto
_LL318;}_LL31C: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp6FB);{struct Cyc_Absyn_Exp*
_tmpD21[3];_tmp554->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp6FC),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_dyneither_subscript_e,((_tmpD21[2]=Cyc_Absyn_uint_exp(0,0),((
_tmpD21[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD21[0]=Cyc_Absyn_new_exp(_tmp554->r,
0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD21,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}goto _LL318;}_LL318:;}if(
_tmp556  && _tmp557)e->r=Cyc_Toc_check_tagged_union(_tmp554,Cyc_Toc_typ_to_c(
e1typ),_tmp6FB,_tmp555,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);if(is_poly
 && _tmp557)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp(e->r,0),0))->r;Cyc_Toc_set_lhs(nv,
_tmp6F9);goto _LL228;};};}_LL25F: {struct Cyc_Absyn_Subscript_e_struct*_tmp558=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp4FC;if(_tmp558->tag != 24)goto _LL261;
else{_tmp559=_tmp558->f1;_tmp55A=_tmp558->f2;}}_LL260: {int _tmp712=Cyc_Toc_in_lhs(
nv);Cyc_Toc_set_lhs(nv,0);{void*_tmp713=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp559->topt))->v);{void*_tmp714=_tmp713;struct Cyc_List_List*
_tmp716;struct Cyc_Absyn_PtrInfo _tmp718;void*_tmp719;struct Cyc_Absyn_Tqual _tmp71A;
struct Cyc_Absyn_PtrAtts _tmp71B;void*_tmp71C;union Cyc_Absyn_Constraint*_tmp71D;
union Cyc_Absyn_Constraint*_tmp71E;union Cyc_Absyn_Constraint*_tmp71F;_LL31E: {
struct Cyc_Absyn_TupleType_struct*_tmp715=(struct Cyc_Absyn_TupleType_struct*)
_tmp714;if(_tmp715->tag != 11)goto _LL320;else{_tmp716=_tmp715->f1;}}_LL31F: Cyc_Toc_exp_to_c(
nv,_tmp559);Cyc_Toc_exp_to_c(nv,_tmp55A);{unsigned int _tmp721;int _tmp722;struct
_tuple10 _tmp720=Cyc_Evexp_eval_const_uint_exp(_tmp55A);_tmp721=_tmp720.f1;
_tmp722=_tmp720.f2;if(!_tmp722){const char*_tmpD24;void*_tmpD23;(_tmpD23=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD24="unknown tuple subscript in translation to C",
_tag_dyneither(_tmpD24,sizeof(char),44))),_tag_dyneither(_tmpD23,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp559,Cyc_Absyn_fieldname((int)(_tmp721 + 1)));
goto _LL31D;};_LL320: {struct Cyc_Absyn_PointerType_struct*_tmp717=(struct Cyc_Absyn_PointerType_struct*)
_tmp714;if(_tmp717->tag != 5)goto _LL322;else{_tmp718=_tmp717->f1;_tmp719=_tmp718.elt_typ;
_tmp71A=_tmp718.elt_tq;_tmp71B=_tmp718.ptr_atts;_tmp71C=_tmp71B.rgn;_tmp71D=
_tmp71B.nullable;_tmp71E=_tmp71B.bounds;_tmp71F=_tmp71B.zero_term;}}_LL321: {
struct Cyc_List_List*_tmp725=Cyc_Toc_get_relns(_tmp559);int in_bnds=0;{void*
_tmp726=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp71E);_LL325:;_LL326: in_bnds=Cyc_Toc_check_bounds(_tmp713,_tmp725,_tmp559,
_tmp55A);if(Cyc_Toc_warn_bounds_checks  && !in_bnds){const char*_tmpD28;void*
_tmpD27[1];struct Cyc_String_pa_struct _tmpD26;(_tmpD26.tag=0,((_tmpD26.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpD27[0]=&
_tmpD26,Cyc_Tcutil_warn(e->loc,((_tmpD28="bounds check necessary for %s",
_tag_dyneither(_tmpD28,sizeof(char),30))),_tag_dyneither(_tmpD27,sizeof(void*),1)))))));}
_LL324:;}Cyc_Toc_exp_to_c(nv,_tmp559);Cyc_Toc_exp_to_c(nv,_tmp55A);++ Cyc_Toc_total_bounds_checks;{
void*_tmp72A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp71E);struct Cyc_Absyn_Exp*_tmp72C;_LL328: {struct Cyc_Absyn_Upper_b_struct*
_tmp72B=(struct Cyc_Absyn_Upper_b_struct*)_tmp72A;if(_tmp72B->tag != 1)goto _LL32A;
else{_tmp72C=_tmp72B->f1;}}_LL329: {int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp71D);void*ta1=Cyc_Toc_typ_to_c(_tmp719);void*ta2=
Cyc_Absyn_cstar_typ(ta1,_tmp71A);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp71F)){if(!Cyc_Evexp_c_can_eval(_tmp72C)){const char*_tmpD2B;void*_tmpD2A;(
_tmpD2A=0,Cyc_Tcutil_terr(e->loc,((_tmpD2B="cannot determine subscript is in bounds",
_tag_dyneither(_tmpD2B,sizeof(char),40))),_tag_dyneither(_tmpD2A,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp559);struct Cyc_Absyn_Exp*_tmpD2C[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpD2C[2]=_tmp55A,((_tmpD2C[1]=_tmp72C,((
_tmpD2C[0]=_tmp559,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD2C,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp72C)){const char*_tmpD2F;void*_tmpD2E;(
_tmpD2E=0,Cyc_Tcutil_terr(e->loc,((_tmpD2F="cannot determine subscript is in bounds",
_tag_dyneither(_tmpD2F,sizeof(char),40))),_tag_dyneither(_tmpD2E,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){const char*_tmpD32;void*_tmpD31;(_tmpD31=0,Cyc_Tcutil_warn(
e->loc,((_tmpD32="inserted null check due to dereference",_tag_dyneither(_tmpD32,
sizeof(char),39))),_tag_dyneither(_tmpD31,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpD33[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((
_tmpD33[3]=_tmp55A,((_tmpD33[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD33[1]=
_tmp72C,((_tmpD33[0]=_tmp559,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD33,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{if(
!Cyc_Evexp_c_can_eval(_tmp72C)){const char*_tmpD36;void*_tmpD35;(_tmpD35=0,Cyc_Tcutil_terr(
e->loc,((_tmpD36="cannot determine subscript is in bounds",_tag_dyneither(
_tmpD36,sizeof(char),40))),_tag_dyneither(_tmpD35,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpD37[2];_tmp55A->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((
_tmpD37[1]=Cyc_Absyn_copy_exp(_tmp55A),((_tmpD37[0]=_tmp72C,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD37,sizeof(struct Cyc_Absyn_Exp*),
2)))))));};}}}goto _LL327;}_LL32A: {struct Cyc_Absyn_DynEither_b_struct*_tmp72D=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp72A;if(_tmp72D->tag != 0)goto _LL327;}
_LL32B: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp719);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp71A),
Cyc_Absyn_aggrmember_exp(_tmp559,Cyc_Toc_curr_sp,0)),_tmp55A);}else{struct Cyc_Absyn_Exp*
_tmp739=Cyc_Toc__check_dyneither_subscript_e;struct Cyc_Absyn_Exp*_tmpD38[3];e->r=
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp71A),Cyc_Absyn_fncall_exp(
_tmp739,((_tmpD38[2]=_tmp55A,((_tmpD38[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((
_tmpD38[0]=_tmp559,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD38,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL327;}
_LL327:;}goto _LL31D;}_LL322:;_LL323: {const char*_tmpD3B;void*_tmpD3A;(_tmpD3A=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD3B="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(
_tmpD3B,sizeof(char),49))),_tag_dyneither(_tmpD3A,sizeof(void*),0)));}_LL31D:;}
Cyc_Toc_set_lhs(nv,_tmp712);goto _LL228;};}_LL261: {struct Cyc_Absyn_Tuple_e_struct*
_tmp55B=(struct Cyc_Absyn_Tuple_e_struct*)_tmp4FC;if(_tmp55B->tag != 25)goto _LL263;
else{_tmp55C=_tmp55B->f1;}}_LL262: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(
nv,0,0,_tmp55C))->r;else{struct Cyc_List_List*_tmp73D=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_tup_to_c,_tmp55C);void*_tmp73E=Cyc_Toc_add_tuple_type(_tmp73D);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp55C != 0;(_tmp55C=_tmp55C->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp55C->hd);{struct _tuple15*_tmpD3E;struct Cyc_List_List*
_tmpD3D;dles=((_tmpD3D=_cycalloc(sizeof(*_tmpD3D)),((_tmpD3D->hd=((_tmpD3E=
_cycalloc(sizeof(*_tmpD3E)),((_tmpD3E->f1=0,((_tmpD3E->f2=(struct Cyc_Absyn_Exp*)
_tmp55C->hd,_tmpD3E)))))),((_tmpD3D->tl=dles,_tmpD3D))))));};}}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);e->r=Cyc_Toc_unresolvedmem_exp_r(
0,dles);}goto _LL228;_LL263: {struct Cyc_Absyn_Array_e_struct*_tmp55D=(struct Cyc_Absyn_Array_e_struct*)
_tmp4FC;if(_tmp55D->tag != 27)goto _LL265;else{_tmp55E=_tmp55D->f1;}}_LL264: e->r=
Cyc_Toc_unresolvedmem_exp_r(0,_tmp55E);{struct Cyc_List_List*_tmp741=_tmp55E;for(
0;_tmp741 != 0;_tmp741=_tmp741->tl){struct _tuple15 _tmp743;struct Cyc_Absyn_Exp*
_tmp744;struct _tuple15*_tmp742=(struct _tuple15*)_tmp741->hd;_tmp743=*_tmp742;
_tmp744=_tmp743.f2;Cyc_Toc_exp_to_c(nv,_tmp744);}}goto _LL228;_LL265: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp55F=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp4FC;if(_tmp55F->tag != 28)
goto _LL267;else{_tmp560=_tmp55F->f1;_tmp561=_tmp55F->f2;_tmp562=_tmp55F->f3;
_tmp563=_tmp55F->f4;}}_LL266: {unsigned int _tmp746;int _tmp747;struct _tuple10
_tmp745=Cyc_Evexp_eval_const_uint_exp(_tmp561);_tmp746=_tmp745.f1;_tmp747=
_tmp745.f2;{void*_tmp748=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp562->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp562);{struct Cyc_List_List*
es=0;if(!Cyc_Toc_is_zero(_tmp562)){if(!_tmp747){const char*_tmpD41;void*_tmpD40;(
_tmpD40=0,Cyc_Tcutil_terr(_tmp561->loc,((_tmpD41="cannot determine value of constant",
_tag_dyneither(_tmpD41,sizeof(char),35))),_tag_dyneither(_tmpD40,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp746;++ i){struct _tuple15*_tmpD44;struct Cyc_List_List*
_tmpD43;es=((_tmpD43=_cycalloc(sizeof(*_tmpD43)),((_tmpD43->hd=((_tmpD44=
_cycalloc(sizeof(*_tmpD44)),((_tmpD44->f1=0,((_tmpD44->f2=_tmp562,_tmpD44)))))),((
_tmpD43->tl=es,_tmpD43))))));}}if(_tmp563){struct Cyc_Absyn_Exp*_tmp74D=Cyc_Toc_cast_it(
_tmp748,Cyc_Absyn_uint_exp(0,0));struct _tuple15*_tmpD47;struct Cyc_List_List*
_tmpD46;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,((_tmpD46=_cycalloc(sizeof(*_tmpD46)),((_tmpD46->hd=((
_tmpD47=_cycalloc(sizeof(*_tmpD47)),((_tmpD47->f1=0,((_tmpD47->f2=_tmp74D,
_tmpD47)))))),((_tmpD46->tl=0,_tmpD46)))))));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL228;};};}_LL267: {struct Cyc_Absyn_Aggregate_e_struct*_tmp564=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp4FC;if(_tmp564->tag != 29)goto _LL269;
else{_tmp565=_tmp564->f1;_tmp566=_tmp564->f2;_tmp567=_tmp564->f3;_tmp568=_tmp564->f4;}}
_LL268: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp566
!= 0,0,0,_tmp567,_tmp565))->r;else{if(_tmp568 == 0){const char*_tmpD4A;void*
_tmpD49;(_tmpD49=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD4A="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpD4A,sizeof(
char),38))),_tag_dyneither(_tmpD49,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp568;struct Cyc_Toc_Env _tmp753;struct
_RegionHandle*_tmp754;struct Cyc_Toc_Env*_tmp752=nv;_tmp753=*_tmp752;_tmp754=
_tmp753.rgn;{struct Cyc_List_List*_tmp755=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum 
Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp754,e->loc,_tmp567,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp568->impl))->tagged){struct
_tuple21 _tmp757;struct Cyc_Absyn_Aggrfield*_tmp758;struct Cyc_Absyn_Exp*_tmp759;
struct _tuple21*_tmp756=(struct _tuple21*)((struct Cyc_List_List*)_check_null(
_tmp755))->hd;_tmp757=*_tmp756;_tmp758=_tmp757.f1;_tmp759=_tmp757.f2;{void*
_tmp75A=_tmp758->type;Cyc_Toc_exp_to_c(nv,_tmp759);if(Cyc_Toc_is_void_star_or_boxed_tvar(
_tmp75A))_tmp759->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp759->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp568,_tmp758->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple15*_tmpD4F;struct _tuple15*_tmpD4E;struct _tuple15*_tmpD4D[2];
struct Cyc_List_List*_tmp75B=(_tmpD4D[1]=((_tmpD4E=_cycalloc(sizeof(*_tmpD4E)),((
_tmpD4E->f1=0,((_tmpD4E->f2=_tmp759,_tmpD4E)))))),((_tmpD4D[0]=((_tmpD4F=
_cycalloc(sizeof(*_tmpD4F)),((_tmpD4F->f1=0,((_tmpD4F->f2=field_tag_exp,_tmpD4F)))))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD4D,sizeof(struct _tuple15*),2)))));struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(
0,_tmp75B,0);struct Cyc_Absyn_FieldName_struct*_tmpD55;struct Cyc_Absyn_FieldName_struct
_tmpD54;void*_tmpD53[1];struct Cyc_List_List*ds=(_tmpD53[0]=(void*)((_tmpD55=
_cycalloc(sizeof(*_tmpD55)),((_tmpD55[0]=((_tmpD54.tag=1,((_tmpD54.f1=_tmp758->name,
_tmpD54)))),_tmpD55)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD53,sizeof(void*),1)));struct _tuple15*_tmpD58;struct _tuple15*
_tmpD57[1];struct Cyc_List_List*dles=(_tmpD57[0]=((_tmpD58=_cycalloc(sizeof(*
_tmpD58)),((_tmpD58->f1=ds,((_tmpD58->f2=umem,_tmpD58)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD57,sizeof(struct
_tuple15*),1)));e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{struct Cyc_List_List*
_tmp764=0;struct Cyc_List_List*_tmp765=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp765 != 0;_tmp765=_tmp765->tl){struct Cyc_List_List*
_tmp766=_tmp755;for(0;_tmp766 != 0;_tmp766=_tmp766->tl){if((*((struct _tuple21*)
_tmp766->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp765->hd){struct _tuple21
_tmp768;struct Cyc_Absyn_Aggrfield*_tmp769;struct Cyc_Absyn_Exp*_tmp76A;struct
_tuple21*_tmp767=(struct _tuple21*)_tmp766->hd;_tmp768=*_tmp767;_tmp769=_tmp768.f1;
_tmp76A=_tmp768.f2;{void*_tmp76B=_tmp769->type;Cyc_Toc_exp_to_c(nv,_tmp76A);if(
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp76B))_tmp76A->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp76A->r,0));{struct _tuple15*_tmpD5B;struct Cyc_List_List*
_tmpD5A;_tmp764=((_tmpD5A=_cycalloc(sizeof(*_tmpD5A)),((_tmpD5A->hd=((_tmpD5B=
_cycalloc(sizeof(*_tmpD5B)),((_tmpD5B->f1=0,((_tmpD5B->f2=_tmp76A,_tmpD5B)))))),((
_tmpD5A->tl=_tmp764,_tmpD5A))))));}break;};}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp764));}};};}
goto _LL228;_LL269: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp569=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp4FC;if(_tmp569->tag != 30)goto _LL26B;else{_tmp56A=(void*)_tmp569->f1;_tmp56B=
_tmp569->f2;}}_LL26A: {struct Cyc_List_List*fs;{void*_tmp76E=Cyc_Tcutil_compress(
_tmp56A);struct Cyc_List_List*_tmp770;_LL32D: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp76F=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp76E;if(_tmp76F->tag != 13)goto
_LL32F;else{_tmp770=_tmp76F->f2;}}_LL32E: fs=_tmp770;goto _LL32C;_LL32F:;_LL330: {
const char*_tmpD5F;void*_tmpD5E[1];struct Cyc_String_pa_struct _tmpD5D;(_tmpD5D.tag=
0,((_tmpD5D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp56A)),((_tmpD5E[0]=& _tmpD5D,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD5F="anon struct has type %s",
_tag_dyneither(_tmpD5F,sizeof(char),24))),_tag_dyneither(_tmpD5E,sizeof(void*),1)))))));}
_LL32C:;}{struct Cyc_Toc_Env _tmp775;struct _RegionHandle*_tmp776;struct Cyc_Toc_Env*
_tmp774=nv;_tmp775=*_tmp774;_tmp776=_tmp775.rgn;{struct Cyc_List_List*_tmp777=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp776,e->loc,_tmp56B,Cyc_Absyn_StructA,fs);for(0;_tmp777 != 0;_tmp777=_tmp777->tl){
struct _tuple21 _tmp779;struct Cyc_Absyn_Aggrfield*_tmp77A;struct Cyc_Absyn_Exp*
_tmp77B;struct _tuple21*_tmp778=(struct _tuple21*)_tmp777->hd;_tmp779=*_tmp778;
_tmp77A=_tmp779.f1;_tmp77B=_tmp779.f2;{void*_tmp77C=_tmp77A->type;Cyc_Toc_exp_to_c(
nv,_tmp77B);if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp77C))_tmp77B->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp77B->r,0));};}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp56B);}goto _LL228;};}_LL26B: {struct Cyc_Absyn_Datatype_e_struct*_tmp56C=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp4FC;if(_tmp56C->tag != 31)goto _LL26D;else{
_tmp56D=_tmp56C->f1;_tmp56E=_tmp56C->f2;_tmp56F=_tmp56C->f3;}}_LL26C: {void*
datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple0*_tmp77D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp77E=Cyc_Absyn_var_exp(_tmp77D,0);struct Cyc_Absyn_Exp*
member_exp;{const char*_tmpD60;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp56F->name,((_tmpD60="_struct",_tag_dyneither(_tmpD60,sizeof(char),8)))));}
tag_exp=_tmp56E->is_extensible?Cyc_Absyn_var_exp(_tmp56F->name,0): Cyc_Toc_datatype_tag(
_tmp56E,_tmp56F->name);member_exp=_tmp77E;{struct Cyc_List_List*_tmp780=_tmp56F->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp56D != 0;(
_tmp56D=_tmp56D->tl,_tmp780=_tmp780->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp56D->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp780))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_boxed_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct _tuple15*_tmpD63;struct
Cyc_List_List*_tmpD62;dles=((_tmpD62=_cycalloc(sizeof(*_tmpD62)),((_tmpD62->hd=((
_tmpD63=_cycalloc(sizeof(*_tmpD63)),((_tmpD63->f1=0,((_tmpD63->f2=cur_e,_tmpD63)))))),((
_tmpD62->tl=dles,_tmpD62))))));};}{struct _tuple15*_tmpD66;struct Cyc_List_List*
_tmpD65;dles=((_tmpD65=_cycalloc(sizeof(*_tmpD65)),((_tmpD65->hd=((_tmpD66=
_cycalloc(sizeof(*_tmpD66)),((_tmpD66->f1=0,((_tmpD66->f2=tag_exp,_tmpD66)))))),((
_tmpD65->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles),_tmpD65))))));}e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{struct Cyc_List_List*
_tmpD67;struct Cyc_List_List*_tmp785=(_tmpD67=_cycalloc(sizeof(*_tmpD67)),((
_tmpD67->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,
0),tag_exp,0),((_tmpD67->tl=0,_tmpD67)))));{int i=1;for(0;_tmp56D != 0;(((_tmp56D=
_tmp56D->tl,i ++)),_tmp780=_tmp780->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp56D->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp780))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_boxed_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp786=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpD68;_tmp785=((_tmpD68=_cycalloc(sizeof(*
_tmpD68)),((_tmpD68->hd=_tmp786,((_tmpD68->tl=_tmp785,_tmpD68))))));};}}{struct
Cyc_Absyn_Stmt*_tmp788=Cyc_Absyn_exp_stmt(_tmp77E,0);struct Cyc_List_List*_tmpD69;
struct Cyc_Absyn_Stmt*_tmp789=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpD69=_cycalloc(sizeof(*_tmpD69)),((
_tmpD69->hd=_tmp788,((_tmpD69->tl=_tmp785,_tmpD69))))))),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp77D,datatype_ctype,0,_tmp789,0));};}goto _LL228;};}
_LL26D: {struct Cyc_Absyn_Enum_e_struct*_tmp570=(struct Cyc_Absyn_Enum_e_struct*)
_tmp4FC;if(_tmp570->tag != 32)goto _LL26F;}_LL26E: goto _LL270;_LL26F: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp571=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp4FC;if(_tmp571->tag != 33)goto
_LL271;}_LL270: goto _LL228;_LL271: {struct Cyc_Absyn_Malloc_e_struct*_tmp572=(
struct Cyc_Absyn_Malloc_e_struct*)_tmp4FC;if(_tmp572->tag != 34)goto _LL273;else{
_tmp573=_tmp572->f1;_tmp574=_tmp573.is_calloc;_tmp575=_tmp573.rgn;_tmp576=
_tmp573.elt_type;_tmp577=_tmp573.num_elts;_tmp578=_tmp573.fat_result;}}_LL272: {
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp576)));Cyc_Toc_exp_to_c(nv,
_tmp577);if(_tmp578){struct _tuple0*_tmp78C=Cyc_Toc_temp_var();struct _tuple0*
_tmp78D=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp574){xexp=_tmp577;if(_tmp575 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp575;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp78C,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp576,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp78C,0));}{struct Cyc_Absyn_Exp*_tmpD6A[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpD6A[2]=Cyc_Absyn_var_exp(_tmp78C,0),((_tmpD6A[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpD6A[0]=Cyc_Absyn_var_exp(_tmp78D,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD6A,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{if(_tmp575 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp575;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp78C,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp576,Cyc_Absyn_var_exp(_tmp78C,0));}{struct Cyc_Absyn_Exp*_tmpD6B[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpD6B[2]=Cyc_Absyn_var_exp(_tmp78C,0),((_tmpD6B[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpD6B[0]=Cyc_Absyn_var_exp(_tmp78D,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD6B,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);};}{struct Cyc_Absyn_Stmt*_tmp790=Cyc_Absyn_declare_stmt(_tmp78C,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)_tmp577,Cyc_Absyn_declare_stmt(_tmp78D,Cyc_Absyn_cstar_typ(
t_c,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp790);};}else{if(_tmp574){if(_tmp575 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp575;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp577))->r;}else{e->r=(
Cyc_Toc_calloc_exp(*_tmp576,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp577))->r;}}else{
if(_tmp575 != 0  && !Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp575;Cyc_Toc_exp_to_c(nv,rgn);e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp577))->r;}
else{e->r=(Cyc_Toc_malloc_exp(*_tmp576,_tmp577))->r;}}}goto _LL228;}_LL273: {
struct Cyc_Absyn_Swap_e_struct*_tmp579=(struct Cyc_Absyn_Swap_e_struct*)_tmp4FC;
if(_tmp579->tag != 35)goto _LL275;else{_tmp57A=_tmp579->f1;_tmp57B=_tmp579->f2;}}
_LL274: {int is_dyneither_ptr=0;void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp57A->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp57B->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,&
is_dyneither_ptr)){const char*_tmpD6E;void*_tmpD6D;(_tmpD6D=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD6E="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpD6E,sizeof(char),57))),_tag_dyneither(_tmpD6D,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}{int f1_tag=0;void*tagged_mem_type1=(void*)&
Cyc_Absyn_VoidType_val;int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp57A,&
f1_tag,& tagged_mem_type1,1);int f2_tag=0;void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp57B,& f2_tag,& tagged_mem_type2,1);
Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp57A);Cyc_Toc_exp_to_c(nv,_tmp57B);
Cyc_Toc_set_lhs(nv,0);if(e1_tmem)Cyc_Toc_add_tagged_union_check_for_swap(_tmp57A,
f1_tag,tagged_mem_type1);else{_tmp57A=Cyc_Toc_push_address_exp(_tmp57A);}if(
e2_tmem)Cyc_Toc_add_tagged_union_check_for_swap(_tmp57B,f2_tag,tagged_mem_type2);
else{_tmp57B=Cyc_Toc_push_address_exp(_tmp57B);}{struct Cyc_Absyn_Exp*_tmpD6F[2];
e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpD6F[1]=_tmp57B,((_tmpD6F[0]=_tmp57A,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD6F,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL228;};};}_LL275: {struct
Cyc_Absyn_Tagcheck_e_struct*_tmp57C=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp4FC;
if(_tmp57C->tag != 38)goto _LL277;else{_tmp57D=_tmp57C->f1;_tmp57E=_tmp57C->f2;}}
_LL276: {void*_tmp794=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp57D->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp57D);{void*_tmp795=
_tmp794;struct Cyc_Absyn_AggrInfo _tmp797;union Cyc_Absyn_AggrInfoU _tmp798;struct
Cyc_Absyn_Aggrdecl**_tmp799;struct Cyc_Absyn_Aggrdecl*_tmp79A;_LL332: {struct Cyc_Absyn_AggrType_struct*
_tmp796=(struct Cyc_Absyn_AggrType_struct*)_tmp795;if(_tmp796->tag != 12)goto
_LL334;else{_tmp797=_tmp796->f1;_tmp798=_tmp797.aggr_info;if((_tmp798.KnownAggr).tag
!= 2)goto _LL334;_tmp799=(struct Cyc_Absyn_Aggrdecl**)(_tmp798.KnownAggr).val;
_tmp79A=*_tmp799;}}_LL333: {struct Cyc_Absyn_Exp*_tmp79B=Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp79A,_tmp57E),0);struct Cyc_Absyn_Exp*_tmp79C=Cyc_Absyn_aggrmember_exp(
_tmp57D,_tmp57E,0);struct Cyc_Absyn_Exp*_tmp79D=Cyc_Absyn_aggrmember_exp(_tmp79C,
Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(_tmp79D,_tmp79B,0))->r;goto _LL331;}
_LL334:;_LL335: {const char*_tmpD72;void*_tmpD71;(_tmpD71=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD72="non-aggregate type in tagcheck",
_tag_dyneither(_tmpD72,sizeof(char),31))),_tag_dyneither(_tmpD71,sizeof(void*),0)));}
_LL331:;}goto _LL228;}_LL277: {struct Cyc_Absyn_StmtExp_e_struct*_tmp57F=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp4FC;if(_tmp57F->tag != 37)goto _LL279;else{_tmp580=
_tmp57F->f1;}}_LL278: Cyc_Toc_stmt_to_c(nv,_tmp580);goto _LL228;_LL279: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp581=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp4FC;if(_tmp581->tag != 36)
goto _LL27B;}_LL27A: {const char*_tmpD75;void*_tmpD74;(_tmpD74=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD75="UnresolvedMem",
_tag_dyneither(_tmpD75,sizeof(char),14))),_tag_dyneither(_tmpD74,sizeof(void*),0)));}
_LL27B: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp582=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp4FC;if(_tmp582->tag != 26)goto _LL27D;}_LL27C: {const char*_tmpD78;void*_tmpD77;(
_tmpD77=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpD78="compoundlit",_tag_dyneither(_tmpD78,sizeof(char),12))),_tag_dyneither(
_tmpD77,sizeof(void*),0)));}_LL27D: {struct Cyc_Absyn_Valueof_e_struct*_tmp583=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp4FC;if(_tmp583->tag != 39)goto _LL27F;}
_LL27E: {const char*_tmpD7B;void*_tmpD7A;(_tmpD7A=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD7B="valueof(-)",
_tag_dyneither(_tmpD7B,sizeof(char),11))),_tag_dyneither(_tmpD7A,sizeof(void*),0)));}
_LL27F: {struct Cyc_Absyn_Asm_e_struct*_tmp584=(struct Cyc_Absyn_Asm_e_struct*)
_tmp4FC;if(_tmp584->tag != 40)goto _LL228;}_LL280: {const char*_tmpD7E;void*_tmpD7D;(
_tmpD7D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD7E="__asm__",_tag_dyneither(_tmpD7E,sizeof(char),8))),_tag_dyneither(
_tmpD7D,sizeof(void*),0)));}_LL228:;};}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0);}struct _tuple22{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*
f3;};struct _tuple23{struct _tuple0*f1;void*f2;};struct _tuple24{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple22 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*
nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
struct Cyc_Absyn_Stmt*s;{void*_tmp7A8=p->r;struct Cyc_Absyn_Vardecl*_tmp7AA;struct
Cyc_Absyn_Vardecl _tmp7AB;struct _tuple0*_tmp7AC;struct Cyc_Absyn_Pat*_tmp7AD;
struct Cyc_Absyn_Vardecl*_tmp7AF;struct Cyc_Absyn_Vardecl _tmp7B0;struct _tuple0*
_tmp7B1;struct Cyc_Absyn_Vardecl*_tmp7B4;struct Cyc_Absyn_Pat*_tmp7B5;enum Cyc_Absyn_Sign
_tmp7B8;int _tmp7B9;char _tmp7BB;struct _dyneither_ptr _tmp7BD;struct Cyc_Absyn_Enumdecl*
_tmp7BF;struct Cyc_Absyn_Enumfield*_tmp7C0;void*_tmp7C2;struct Cyc_Absyn_Enumfield*
_tmp7C3;struct Cyc_Absyn_Pat*_tmp7C5;struct Cyc_Absyn_Pat _tmp7C6;void*_tmp7C7;
struct Cyc_Absyn_Datatypedecl*_tmp7C9;struct Cyc_Absyn_Datatypefield*_tmp7CA;
struct Cyc_List_List*_tmp7CB;struct Cyc_List_List*_tmp7CD;struct Cyc_List_List*
_tmp7CF;struct Cyc_Absyn_AggrInfo*_tmp7D1;struct Cyc_Absyn_AggrInfo*_tmp7D3;struct
Cyc_Absyn_AggrInfo _tmp7D4;union Cyc_Absyn_AggrInfoU _tmp7D5;struct Cyc_List_List*
_tmp7D6;struct Cyc_Absyn_Pat*_tmp7D8;_LL337: {struct Cyc_Absyn_Var_p_struct*
_tmp7A9=(struct Cyc_Absyn_Var_p_struct*)_tmp7A8;if(_tmp7A9->tag != 1)goto _LL339;
else{_tmp7AA=_tmp7A9->f1;_tmp7AB=*_tmp7AA;_tmp7AC=_tmp7AB.name;_tmp7AD=_tmp7A9->f2;}}
_LL338: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp7AC,r),rgn,t,r,path,
_tmp7AD,fail_stmt,decls);_LL339: {struct Cyc_Absyn_TagInt_p_struct*_tmp7AE=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp7A8;if(_tmp7AE->tag != 3)goto _LL33B;else{
_tmp7AF=_tmp7AE->f2;_tmp7B0=*_tmp7AF;_tmp7B1=_tmp7B0.name;}}_LL33A: nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp7B1,r);goto _LL33C;_LL33B: {struct Cyc_Absyn_Wild_p_struct*_tmp7B2=(
struct Cyc_Absyn_Wild_p_struct*)_tmp7A8;if(_tmp7B2->tag != 0)goto _LL33D;}_LL33C: s=
Cyc_Toc_skip_stmt_dl();goto _LL336;_LL33D: {struct Cyc_Absyn_Reference_p_struct*
_tmp7B3=(struct Cyc_Absyn_Reference_p_struct*)_tmp7A8;if(_tmp7B3->tag != 2)goto
_LL33F;else{_tmp7B4=_tmp7B3->f1;_tmp7B5=_tmp7B3->f2;}}_LL33E: {struct _tuple0*
_tmp7DC=Cyc_Toc_temp_var();{struct _tuple23*_tmpD81;struct Cyc_List_List*_tmpD80;
decls=((_tmpD80=_region_malloc(rgn,sizeof(*_tmpD80)),((_tmpD80->hd=((_tmpD81=
_region_malloc(rgn,sizeof(*_tmpD81)),((_tmpD81->f1=_tmp7DC,((_tmpD81->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpD81)))))),((_tmpD80->tl=decls,_tmpD80))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp7B4->name,Cyc_Absyn_var_exp(_tmp7DC,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp7DC,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Toc_push_address_exp(path)),0);{struct _tuple22 _tmp7DF=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp7B5,fail_stmt,decls);_tmp7DF.f3=Cyc_Absyn_seq_stmt(s,_tmp7DF.f3,
0);return _tmp7DF;};}_LL33F: {struct Cyc_Absyn_Null_p_struct*_tmp7B6=(struct Cyc_Absyn_Null_p_struct*)
_tmp7A8;if(_tmp7B6->tag != 8)goto _LL341;}_LL340: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),fail_stmt);goto _LL336;_LL341: {struct Cyc_Absyn_Int_p_struct*_tmp7B7=(struct
Cyc_Absyn_Int_p_struct*)_tmp7A8;if(_tmp7B7->tag != 9)goto _LL343;else{_tmp7B8=
_tmp7B7->f1;_tmp7B9=_tmp7B7->f2;}}_LL342: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp7B8,_tmp7B9,0),fail_stmt);goto _LL336;_LL343: {struct Cyc_Absyn_Char_p_struct*
_tmp7BA=(struct Cyc_Absyn_Char_p_struct*)_tmp7A8;if(_tmp7BA->tag != 10)goto _LL345;
else{_tmp7BB=_tmp7BA->f1;}}_LL344: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(
_tmp7BB,0),fail_stmt);goto _LL336;_LL345: {struct Cyc_Absyn_Float_p_struct*_tmp7BC=(
struct Cyc_Absyn_Float_p_struct*)_tmp7A8;if(_tmp7BC->tag != 11)goto _LL347;else{
_tmp7BD=_tmp7BC->f1;}}_LL346: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp7BD,
0),fail_stmt);goto _LL336;_LL347: {struct Cyc_Absyn_Enum_p_struct*_tmp7BE=(struct
Cyc_Absyn_Enum_p_struct*)_tmp7A8;if(_tmp7BE->tag != 12)goto _LL349;else{_tmp7BF=
_tmp7BE->f1;_tmp7C0=_tmp7BE->f2;}}_LL348:{struct Cyc_Absyn_Enum_e_struct _tmpD84;
struct Cyc_Absyn_Enum_e_struct*_tmpD83;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD83=_cycalloc(sizeof(*_tmpD83)),((_tmpD83[0]=((_tmpD84.tag=32,((
_tmpD84.f1=_tmp7C0->name,((_tmpD84.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7BF,((
_tmpD84.f3=(struct Cyc_Absyn_Enumfield*)_tmp7C0,_tmpD84)))))))),_tmpD83)))),0),
fail_stmt);}goto _LL336;_LL349: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp7C1=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp7A8;if(_tmp7C1->tag != 13)goto _LL34B;else{
_tmp7C2=(void*)_tmp7C1->f1;_tmp7C3=_tmp7C1->f2;}}_LL34A:{struct Cyc_Absyn_AnonEnum_e_struct
_tmpD87;struct Cyc_Absyn_AnonEnum_e_struct*_tmpD86;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD86=_cycalloc(sizeof(*_tmpD86)),((_tmpD86[0]=((_tmpD87.tag=33,((
_tmpD87.f1=_tmp7C3->name,((_tmpD87.f2=(void*)_tmp7C2,((_tmpD87.f3=(struct Cyc_Absyn_Enumfield*)
_tmp7C3,_tmpD87)))))))),_tmpD86)))),0),fail_stmt);}goto _LL336;_LL34B: {struct Cyc_Absyn_Pointer_p_struct*
_tmp7C4=(struct Cyc_Absyn_Pointer_p_struct*)_tmp7A8;if(_tmp7C4->tag != 5)goto
_LL34D;else{_tmp7C5=_tmp7C4->f1;_tmp7C6=*_tmp7C5;_tmp7C7=_tmp7C6.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp7C8=(struct Cyc_Absyn_Datatype_p_struct*)_tmp7C7;if(_tmp7C8->tag != 7)goto
_LL34D;else{_tmp7C9=_tmp7C8->f1;_tmp7CA=_tmp7C8->f2;_tmp7CB=_tmp7C8->f3;}};}}
_LL34C: s=Cyc_Toc_skip_stmt_dl();{struct _tuple0*_tmp7E4=Cyc_Toc_temp_var();const
char*_tmpD88;struct _tuple0*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp7CA->name,((
_tmpD88="_struct",_tag_dyneither(_tmpD88,sizeof(char),8))));void*_tmp7E5=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);int cnt=1;struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
_tmp7E5,r);struct Cyc_List_List*_tmp7E6=_tmp7CA->typs;for(0;_tmp7CB != 0;(((
_tmp7CB=_tmp7CB->tl,_tmp7E6=((struct Cyc_List_List*)_check_null(_tmp7E6))->tl)),
++ cnt)){struct Cyc_Absyn_Pat*_tmp7E7=(struct Cyc_Absyn_Pat*)_tmp7CB->hd;if(_tmp7E7->r
== (void*)& Cyc_Absyn_Wild_p_val)continue;{void*_tmp7E8=(*((struct _tuple9*)((
struct Cyc_List_List*)_check_null(_tmp7E6))->hd)).f2;struct _tuple0*_tmp7E9=Cyc_Toc_temp_var();
void*_tmp7EA=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp7E7->topt))->v;void*
_tmp7EB=Cyc_Toc_typ_to_c(_tmp7EA);struct Cyc_Absyn_Exp*_tmp7EC=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp7E4,0),Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_boxed_tvar(
Cyc_Toc_typ_to_c(_tmp7E8)))_tmp7EC=Cyc_Toc_cast_it(_tmp7EB,_tmp7EC);{struct
_tuple23*_tmpD8B;struct Cyc_List_List*_tmpD8A;decls=((_tmpD8A=_region_malloc(rgn,
sizeof(*_tmpD8A)),((_tmpD8A->hd=((_tmpD8B=_region_malloc(rgn,sizeof(*_tmpD8B)),((
_tmpD8B->f1=_tmp7E9,((_tmpD8B->f2=_tmp7EB,_tmpD8B)))))),((_tmpD8A->tl=decls,
_tmpD8A))))));}{struct _tuple22 _tmp7EF=Cyc_Toc_xlate_pat(nv,rgn,_tmp7EA,Cyc_Absyn_var_exp(
_tmp7E9,0),_tmp7EC,_tmp7E7,fail_stmt,decls);nv=_tmp7EF.f1;decls=_tmp7EF.f2;{
struct Cyc_Absyn_Stmt*_tmp7F0=_tmp7EF.f3;struct Cyc_Absyn_Stmt*_tmp7F1=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp7E9,0),_tmp7EC,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp7F1,_tmp7F0,0),0);};};};}{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp7E4,0),Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*tag_exp=
_tmp7C9->is_extensible?Cyc_Absyn_var_exp(_tmp7CA->name,0): Cyc_Toc_datatype_tag(
_tmp7C9,_tmp7CA->name);struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,
tag_exp,0);s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);if(Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v))s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp7E4,0),Cyc_Absyn_uint_exp(0,0),0),
fail_stmt,s,0);s=Cyc_Absyn_declare_stmt(_tmp7E4,_tmp7E5,(struct Cyc_Absyn_Exp*)
rcast,s,0);goto _LL336;};};_LL34D: {struct Cyc_Absyn_Datatype_p_struct*_tmp7CC=(
struct Cyc_Absyn_Datatype_p_struct*)_tmp7A8;if(_tmp7CC->tag != 7)goto _LL34F;else{
_tmp7CD=_tmp7CC->f3;}}_LL34E: _tmp7CF=_tmp7CD;goto _LL350;_LL34F: {struct Cyc_Absyn_Tuple_p_struct*
_tmp7CE=(struct Cyc_Absyn_Tuple_p_struct*)_tmp7A8;if(_tmp7CE->tag != 4)goto _LL351;
else{_tmp7CF=_tmp7CE->f1;}}_LL350: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;
_tmp7CF != 0;(_tmp7CF=_tmp7CF->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp7F3=(struct Cyc_Absyn_Pat*)
_tmp7CF->hd;if(_tmp7F3->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{struct _tuple0*
_tmp7F4=Cyc_Toc_temp_var();void*_tmp7F5=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7F3->topt))->v;{struct _tuple23*_tmpD8E;struct Cyc_List_List*_tmpD8D;decls=((
_tmpD8D=_region_malloc(rgn,sizeof(*_tmpD8D)),((_tmpD8D->hd=((_tmpD8E=
_region_malloc(rgn,sizeof(*_tmpD8E)),((_tmpD8E->f1=_tmp7F4,((_tmpD8E->f2=Cyc_Toc_typ_to_c(
_tmp7F5),_tmpD8E)))))),((_tmpD8D->tl=decls,_tmpD8D))))));}{struct _tuple22 _tmp7F8=
Cyc_Toc_xlate_pat(nv,rgn,_tmp7F5,Cyc_Absyn_var_exp(_tmp7F4,0),Cyc_Absyn_aggrmember_exp(
path,Cyc_Absyn_fieldname(cnt),0),_tmp7F3,fail_stmt,decls);nv=_tmp7F8.f1;decls=
_tmp7F8.f2;{struct Cyc_Absyn_Stmt*_tmp7F9=_tmp7F8.f3;struct Cyc_Absyn_Stmt*_tmp7FA=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7F4,0),Cyc_Absyn_aggrmember_exp(r,
Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7FA,
_tmp7F9,0),0);};};};}goto _LL336;};_LL351: {struct Cyc_Absyn_Aggr_p_struct*_tmp7D0=(
struct Cyc_Absyn_Aggr_p_struct*)_tmp7A8;if(_tmp7D0->tag != 6)goto _LL353;else{
_tmp7D1=_tmp7D0->f1;if(_tmp7D1 != 0)goto _LL353;}}_LL352: {const char*_tmpD91;void*
_tmpD90;(_tmpD90=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD91="unresolved aggregate pattern!",_tag_dyneither(_tmpD91,sizeof(char),30))),
_tag_dyneither(_tmpD90,sizeof(void*),0)));}_LL353: {struct Cyc_Absyn_Aggr_p_struct*
_tmp7D2=(struct Cyc_Absyn_Aggr_p_struct*)_tmp7A8;if(_tmp7D2->tag != 6)goto _LL355;
else{_tmp7D3=_tmp7D2->f1;if(_tmp7D3 == 0)goto _LL355;_tmp7D4=*_tmp7D3;_tmp7D5=
_tmp7D4.aggr_info;_tmp7D6=_tmp7D2->f3;}}_LL354: {struct Cyc_Absyn_Aggrdecl*
_tmp7FD=Cyc_Absyn_get_known_aggrdecl(_tmp7D5);if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp7FD->impl))->tagged){struct _tuple24 _tmp7FF;struct Cyc_List_List*
_tmp800;struct Cyc_Absyn_Pat*_tmp801;struct _tuple24*_tmp7FE=(struct _tuple24*)((
struct Cyc_List_List*)_check_null(_tmp7D6))->hd;_tmp7FF=*_tmp7FE;_tmp800=_tmp7FF.f1;
_tmp801=_tmp7FF.f2;{struct _dyneither_ptr*f;{void*_tmp802=(void*)((struct Cyc_List_List*)
_check_null(_tmp800))->hd;struct _dyneither_ptr*_tmp804;_LL35E: {struct Cyc_Absyn_FieldName_struct*
_tmp803=(struct Cyc_Absyn_FieldName_struct*)_tmp802;if(_tmp803->tag != 1)goto
_LL360;else{_tmp804=_tmp803->f1;}}_LL35F: f=_tmp804;goto _LL35D;_LL360:;_LL361: {
const char*_tmpD94;void*_tmpD93;(_tmpD93=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD94="no field name in tagged union pattern",
_tag_dyneither(_tmpD94,sizeof(char),38))),_tag_dyneither(_tmpD93,sizeof(void*),0)));}
_LL35D:;}{struct _tuple0*_tmp807=Cyc_Toc_temp_var();void*_tmp808=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp801->topt))->v;void*_tmp809=Cyc_Toc_typ_to_c(
_tmp808);{struct _tuple23*_tmpD97;struct Cyc_List_List*_tmpD96;decls=((_tmpD96=
_region_malloc(rgn,sizeof(*_tmpD96)),((_tmpD96->hd=((_tmpD97=_region_malloc(rgn,
sizeof(*_tmpD97)),((_tmpD97->f1=_tmp807,((_tmpD97->f2=_tmp809,_tmpD97)))))),((
_tmpD96->tl=decls,_tmpD96))))));}{struct Cyc_Absyn_Exp*_tmp80C=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp80D=
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp80D=Cyc_Toc_cast_it(_tmp809,_tmp80D);_tmp80C=Cyc_Toc_cast_it(_tmp809,_tmp80C);{
struct _tuple22 _tmp80E=Cyc_Toc_xlate_pat(nv,rgn,_tmp808,Cyc_Absyn_var_exp(_tmp807,
0),_tmp80C,_tmp801,fail_stmt,decls);nv=_tmp80E.f1;decls=_tmp80E.f2;{struct Cyc_Absyn_Stmt*
_tmp80F=_tmp80E.f3;struct Cyc_Absyn_Stmt*_tmp810=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp7FD,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp811=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp807,0),_tmp80D,0);s=Cyc_Absyn_seq_stmt(_tmp810,Cyc_Absyn_seq_stmt(_tmp811,
_tmp80F,0),0);};};};};};}else{s=Cyc_Toc_skip_stmt_dl();for(0;_tmp7D6 != 0;_tmp7D6=
_tmp7D6->tl){struct _tuple24*_tmp812=(struct _tuple24*)_tmp7D6->hd;struct Cyc_Absyn_Pat*
_tmp813=(*_tmp812).f2;if(_tmp813->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _dyneither_ptr*f;{void*_tmp814=(void*)((struct Cyc_List_List*)_check_null((*
_tmp812).f1))->hd;struct _dyneither_ptr*_tmp816;_LL363: {struct Cyc_Absyn_FieldName_struct*
_tmp815=(struct Cyc_Absyn_FieldName_struct*)_tmp814;if(_tmp815->tag != 1)goto
_LL365;else{_tmp816=_tmp815->f1;}}_LL364: f=_tmp816;goto _LL362;_LL365:;_LL366: {
struct Cyc_Toc_Match_error_struct _tmpD9A;struct Cyc_Toc_Match_error_struct*_tmpD99;(
int)_throw((void*)((_tmpD99=_cycalloc_atomic(sizeof(*_tmpD99)),((_tmpD99[0]=((
_tmpD9A.tag=Cyc_Toc_Match_error,_tmpD9A)),_tmpD99)))));}_LL362:;}{struct _tuple0*
_tmp819=Cyc_Toc_temp_var();void*_tmp81A=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp813->topt))->v;void*_tmp81B=Cyc_Toc_typ_to_c(_tmp81A);{struct _tuple23*
_tmpD9D;struct Cyc_List_List*_tmpD9C;decls=((_tmpD9C=_region_malloc(rgn,sizeof(*
_tmpD9C)),((_tmpD9C->hd=((_tmpD9D=_region_malloc(rgn,sizeof(*_tmpD9D)),((_tmpD9D->f1=
_tmp819,((_tmpD9D->f2=_tmp81B,_tmpD9D)))))),((_tmpD9C->tl=decls,_tmpD9C))))));}{
struct _tuple22 _tmp81E=Cyc_Toc_xlate_pat(nv,rgn,_tmp81A,Cyc_Absyn_var_exp(_tmp819,
0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp813,fail_stmt,decls);nv=_tmp81E.f1;
decls=_tmp81E.f2;{struct Cyc_Absyn_Exp*_tmp81F=Cyc_Absyn_aggrmember_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(
Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7FD->impl))->fields,
f)))->type))_tmp81F=Cyc_Toc_cast_it(_tmp81B,_tmp81F);{struct Cyc_Absyn_Stmt*
_tmp820=_tmp81E.f3;struct Cyc_Absyn_Stmt*_tmp821=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp819,0),_tmp81F,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp821,_tmp820,
0),0);};};};};};}}goto _LL336;}_LL355: {struct Cyc_Absyn_Pointer_p_struct*_tmp7D7=(
struct Cyc_Absyn_Pointer_p_struct*)_tmp7A8;if(_tmp7D7->tag != 5)goto _LL357;else{
_tmp7D8=_tmp7D7->f1;}}_LL356: {struct _tuple0*_tmp822=Cyc_Toc_temp_var();void*
_tmp823=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp7D8->topt))->v;{struct
_tuple23*_tmpDA0;struct Cyc_List_List*_tmpD9F;decls=((_tmpD9F=_region_malloc(rgn,
sizeof(*_tmpD9F)),((_tmpD9F->hd=((_tmpDA0=_region_malloc(rgn,sizeof(*_tmpDA0)),((
_tmpDA0->f1=_tmp822,((_tmpDA0->f2=Cyc_Toc_typ_to_c(_tmp823),_tmpDA0)))))),((
_tmpD9F->tl=decls,_tmpD9F))))));}{struct _tuple22 _tmp826=Cyc_Toc_xlate_pat(nv,rgn,
_tmp823,Cyc_Absyn_var_exp(_tmp822,0),Cyc_Absyn_deref_exp(path,0),_tmp7D8,
fail_stmt,decls);nv=_tmp826.f1;decls=_tmp826.f2;{struct Cyc_Absyn_Stmt*_tmp827=
_tmp826.f3;struct Cyc_Absyn_Stmt*_tmp828=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp822,0),Cyc_Absyn_deref_exp(r,0),0),_tmp827,0);if(Cyc_Toc_is_nullable(
t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(
0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),_tmp828,0);else{s=_tmp828;}goto
_LL336;};};}_LL357: {struct Cyc_Absyn_UnknownId_p_struct*_tmp7D9=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp7A8;if(_tmp7D9->tag != 14)goto _LL359;}_LL358: {const char*_tmpDA3;void*_tmpDA2;(
_tmpDA2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDA3="unknownid pat",_tag_dyneither(_tmpDA3,sizeof(char),14))),_tag_dyneither(
_tmpDA2,sizeof(void*),0)));}_LL359: {struct Cyc_Absyn_UnknownCall_p_struct*
_tmp7DA=(struct Cyc_Absyn_UnknownCall_p_struct*)_tmp7A8;if(_tmp7DA->tag != 15)goto
_LL35B;}_LL35A: {const char*_tmpDA6;void*_tmpDA5;(_tmpDA5=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDA6="unknowncall pat",
_tag_dyneither(_tmpDA6,sizeof(char),16))),_tag_dyneither(_tmpDA5,sizeof(void*),0)));}
_LL35B: {struct Cyc_Absyn_Exp_p_struct*_tmp7DB=(struct Cyc_Absyn_Exp_p_struct*)
_tmp7A8;if(_tmp7DB->tag != 16)goto _LL336;}_LL35C: {const char*_tmpDA9;void*_tmpDA8;(
_tmpDA8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDA9="exp pat",_tag_dyneither(_tmpDA9,sizeof(char),8))),_tag_dyneither(
_tmpDA8,sizeof(void*),0)));}_LL336:;}{struct _tuple22 _tmpDAA;return(_tmpDAA.f1=nv,((
_tmpDAA.f2=decls,((_tmpDAA.f3=s,_tmpDAA)))));};}struct _tuple25{struct
_dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
static struct _tuple25*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){struct _tuple25*_tmpDAB;return(_tmpDAB=_region_malloc(r,sizeof(*_tmpDAB)),((
_tmpDAB->f1=Cyc_Toc_fresh_label(),((_tmpDAB->f2=Cyc_Toc_fresh_label(),((_tmpDAB->f3=
sc,_tmpDAB)))))));}static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{
void*_tmp831=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int
leave_as_switch;{void*_tmp832=Cyc_Tcutil_compress(_tmp831);_LL368: {struct Cyc_Absyn_IntType_struct*
_tmp833=(struct Cyc_Absyn_IntType_struct*)_tmp832;if(_tmp833->tag != 6)goto _LL36A;}
_LL369: goto _LL36B;_LL36A: {struct Cyc_Absyn_EnumType_struct*_tmp834=(struct Cyc_Absyn_EnumType_struct*)
_tmp832;if(_tmp834->tag != 14)goto _LL36C;}_LL36B: leave_as_switch=1;goto _LL367;
_LL36C:;_LL36D: leave_as_switch=0;goto _LL367;_LL367:;}{struct Cyc_List_List*
_tmp835=scs;for(0;_tmp835 != 0;_tmp835=_tmp835->tl){if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp835->hd)->pat_vars))->v
!= 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp835->hd)->where_clause != 0){
leave_as_switch=0;break;}}}if(leave_as_switch){struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp836=scs;for(0;_tmp836 != 0;_tmp836=_tmp836->tl){struct Cyc_Absyn_Stmt*
_tmp837=((struct Cyc_Absyn_Switch_clause*)_tmp836->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp836->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp837,0);next_l=Cyc_Toc_fresh_label();{
struct Cyc_Toc_Env _tmp839;struct _RegionHandle*_tmp83A;struct Cyc_Toc_Env*_tmp838=
nv;_tmp839=*_tmp838;_tmp83A=_tmp839.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
_tmp83A,nv,next_l),_tmp837);};}}return;}{struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp83C;
struct _RegionHandle*_tmp83D;struct Cyc_Toc_Env*_tmp83B=nv;_tmp83C=*_tmp83B;
_tmp83D=_tmp83C.rgn;{struct Cyc_Toc_Env*_tmp83E=Cyc_Toc_share_env(_tmp83D,nv);
struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple25*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp83D,Cyc_Toc_gen_label,
_tmp83D,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct
Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp83F=lscs;for(0;_tmp83F != 0;
_tmp83F=_tmp83F->tl){struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple25*)
_tmp83F->hd)).f3;struct _dyneither_ptr*fail_lab=_tmp83F->tl == 0?end_l:(*((struct
_tuple25*)((struct Cyc_List_List*)_check_null(_tmp83F->tl))->hd)).f1;struct Cyc_Toc_Env*
_tmp841;struct Cyc_List_List*_tmp842;struct Cyc_Absyn_Stmt*_tmp843;struct _tuple22
_tmp840=Cyc_Toc_xlate_pat(_tmp83E,_tmp83D,_tmp831,r,path,sc->pattern,Cyc_Absyn_goto_stmt(
fail_lab,0),decls);_tmp841=_tmp840.f1;_tmp842=_tmp840.f2;_tmp843=_tmp840.f3;if(
sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp844=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp841,_tmp844);_tmp843=Cyc_Absyn_seq_stmt(
_tmp843,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp844,0),
Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp842;{
struct Cyc_List_List*_tmpDAC;nvs=((_tmpDAC=_region_malloc(_tmp83D,sizeof(*_tmpDAC)),((
_tmpDAC->hd=_tmp841,((_tmpDAC->tl=nvs,_tmpDAC))))));}{struct Cyc_List_List*
_tmpDAD;test_stmts=((_tmpDAD=_region_malloc(_tmp83D,sizeof(*_tmpDAD)),((_tmpDAD->hd=
_tmp843,((_tmpDAD->tl=test_stmts,_tmpDAD))))));};}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple25 _tmp848;struct _dyneither_ptr*_tmp849;struct _dyneither_ptr*_tmp84A;
struct Cyc_Absyn_Switch_clause*_tmp84B;struct _tuple25*_tmp847=(struct _tuple25*)
lscs->hd;_tmp848=*_tmp847;_tmp849=_tmp848.f1;_tmp84A=_tmp848.f2;_tmp84B=_tmp848.f3;{
struct Cyc_Toc_Env*_tmp84C=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp84B->body;struct Cyc_Toc_Env _tmp84E;struct
_RegionHandle*_tmp84F;struct Cyc_Toc_Env*_tmp84D=_tmp83E;_tmp84E=*_tmp84D;_tmp84F=
_tmp84E.rgn;if(lscs->tl != 0){struct _tuple25 _tmp851;struct _dyneither_ptr*_tmp852;
struct Cyc_Absyn_Switch_clause*_tmp853;struct _tuple25*_tmp850=(struct _tuple25*)((
struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp851=*_tmp850;_tmp852=_tmp851.f2;
_tmp853=_tmp851.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp84F,
_tmp84C,end_l,_tmp852,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp853->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp84F,
_tmp84C,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp849,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp84A,s,0),0);{struct Cyc_List_List*_tmpDAE;stmts=((_tmpDAE=_region_malloc(
_tmp83D,sizeof(*_tmpDAE)),((_tmpDAE->hd=s,((_tmpDAE->tl=stmts,_tmpDAE))))));};};}{
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,
Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple23
_tmp856;struct _tuple0*_tmp857;void*_tmp858;struct _tuple23*_tmp855=(struct
_tuple23*)decls->hd;_tmp856=*_tmp855;_tmp857=_tmp856.f1;_tmp858=_tmp856.f2;res=
Cyc_Absyn_declare_stmt(_tmp857,_tmp858,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(
v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(
struct Cyc_Absyn_Exp*)e,res,0))->r;};};};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){struct Cyc_List_List*
_tmpDAF;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,((
_tmpDAF=_cycalloc(sizeof(*_tmpDAF)),((_tmpDAF->hd=Cyc_Absyn_uint_exp((
unsigned int)(n - 1),0),((_tmpDAF->tl=0,_tmpDAF)))))),0),0);}void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s){while(1){void*_tmp85A=s->r;struct Cyc_Absyn_Exp*_tmp85D;
struct Cyc_Absyn_Stmt*_tmp85F;struct Cyc_Absyn_Stmt*_tmp860;struct Cyc_Absyn_Exp*
_tmp862;struct Cyc_Absyn_Exp*_tmp864;struct Cyc_Absyn_Stmt*_tmp865;struct Cyc_Absyn_Stmt*
_tmp866;struct _tuple8 _tmp868;struct Cyc_Absyn_Exp*_tmp869;struct Cyc_Absyn_Stmt*
_tmp86A;struct Cyc_Absyn_Stmt*_tmp86C;struct Cyc_Absyn_Stmt*_tmp86E;struct Cyc_Absyn_Stmt*
_tmp870;struct Cyc_Absyn_Exp*_tmp872;struct _tuple8 _tmp873;struct Cyc_Absyn_Exp*
_tmp874;struct _tuple8 _tmp875;struct Cyc_Absyn_Exp*_tmp876;struct Cyc_Absyn_Stmt*
_tmp877;struct Cyc_Absyn_Exp*_tmp879;struct Cyc_List_List*_tmp87A;struct Cyc_List_List*
_tmp87C;struct Cyc_Absyn_Switch_clause**_tmp87D;struct Cyc_Absyn_Decl*_tmp87F;
struct Cyc_Absyn_Stmt*_tmp880;struct _dyneither_ptr*_tmp882;struct Cyc_Absyn_Stmt*
_tmp883;struct Cyc_Absyn_Stmt*_tmp885;struct _tuple8 _tmp886;struct Cyc_Absyn_Exp*
_tmp887;struct Cyc_Absyn_Stmt*_tmp889;struct Cyc_List_List*_tmp88A;struct Cyc_Absyn_Exp*
_tmp88C;_LL36F: {struct Cyc_Absyn_Skip_s_struct*_tmp85B=(struct Cyc_Absyn_Skip_s_struct*)
_tmp85A;if(_tmp85B->tag != 0)goto _LL371;}_LL370: return;_LL371: {struct Cyc_Absyn_Exp_s_struct*
_tmp85C=(struct Cyc_Absyn_Exp_s_struct*)_tmp85A;if(_tmp85C->tag != 1)goto _LL373;
else{_tmp85D=_tmp85C->f1;}}_LL372: Cyc_Toc_exp_to_c(nv,_tmp85D);return;_LL373: {
struct Cyc_Absyn_Seq_s_struct*_tmp85E=(struct Cyc_Absyn_Seq_s_struct*)_tmp85A;if(
_tmp85E->tag != 2)goto _LL375;else{_tmp85F=_tmp85E->f1;_tmp860=_tmp85E->f2;}}
_LL374: Cyc_Toc_stmt_to_c(nv,_tmp85F);s=_tmp860;continue;_LL375: {struct Cyc_Absyn_Return_s_struct*
_tmp861=(struct Cyc_Absyn_Return_s_struct*)_tmp85A;if(_tmp861->tag != 3)goto _LL377;
else{_tmp862=_tmp861->f1;}}_LL376: {struct Cyc_Core_Opt*topt=0;if(_tmp862 != 0){{
struct Cyc_Core_Opt*_tmpDB0;topt=((_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((_tmpDB0->v=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp862->topt))->v),
_tmpDB0))));}Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp862);}if(s->try_depth
> 0){if(topt != 0){struct _tuple0*_tmp88E=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*
_tmp88F=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp88E,0),
0);s->r=(Cyc_Absyn_declare_stmt(_tmp88E,(void*)topt->v,_tmp862,Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler(s->try_depth),_tmp88F,0),0))->r;}else{Cyc_Toc_do_npop_before(
s->try_depth,s);}}return;}_LL377: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp863=(
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp85A;if(_tmp863->tag != 4)goto _LL379;
else{_tmp864=_tmp863->f1;_tmp865=_tmp863->f2;_tmp866=_tmp863->f3;}}_LL378: Cyc_Toc_exp_to_c(
nv,_tmp864);Cyc_Toc_stmt_to_c(nv,_tmp865);s=_tmp866;continue;_LL379: {struct Cyc_Absyn_While_s_struct*
_tmp867=(struct Cyc_Absyn_While_s_struct*)_tmp85A;if(_tmp867->tag != 5)goto _LL37B;
else{_tmp868=_tmp867->f1;_tmp869=_tmp868.f1;_tmp86A=_tmp867->f2;}}_LL37A: Cyc_Toc_exp_to_c(
nv,_tmp869);{struct Cyc_Toc_Env _tmp891;struct _RegionHandle*_tmp892;struct Cyc_Toc_Env*
_tmp890=nv;_tmp891=*_tmp890;_tmp892=_tmp891.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp892,nv),_tmp86A);return;};_LL37B: {struct Cyc_Absyn_Break_s_struct*_tmp86B=(
struct Cyc_Absyn_Break_s_struct*)_tmp85A;if(_tmp86B->tag != 6)goto _LL37D;else{
_tmp86C=_tmp86B->f1;}}_LL37C: {struct Cyc_Toc_Env _tmp894;struct _dyneither_ptr**
_tmp895;struct Cyc_Toc_Env*_tmp893=nv;_tmp894=*_tmp893;_tmp895=_tmp894.break_lab;
if(_tmp895 != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp895,0);{int dest_depth=_tmp86C == 0?0:
_tmp86C->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;};}
_LL37D: {struct Cyc_Absyn_Continue_s_struct*_tmp86D=(struct Cyc_Absyn_Continue_s_struct*)
_tmp85A;if(_tmp86D->tag != 7)goto _LL37F;else{_tmp86E=_tmp86D->f1;}}_LL37E: {
struct Cyc_Toc_Env _tmp897;struct _dyneither_ptr**_tmp898;struct Cyc_Toc_Env*_tmp896=
nv;_tmp897=*_tmp896;_tmp898=_tmp897.continue_lab;if(_tmp898 != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp898,0);_tmp870=_tmp86E;goto _LL380;}_LL37F: {struct Cyc_Absyn_Goto_s_struct*
_tmp86F=(struct Cyc_Absyn_Goto_s_struct*)_tmp85A;if(_tmp86F->tag != 8)goto _LL381;
else{_tmp870=_tmp86F->f2;}}_LL380: Cyc_Toc_do_npop_before(s->try_depth - ((struct
Cyc_Absyn_Stmt*)_check_null(_tmp870))->try_depth,s);return;_LL381: {struct Cyc_Absyn_For_s_struct*
_tmp871=(struct Cyc_Absyn_For_s_struct*)_tmp85A;if(_tmp871->tag != 9)goto _LL383;
else{_tmp872=_tmp871->f1;_tmp873=_tmp871->f2;_tmp874=_tmp873.f1;_tmp875=_tmp871->f3;
_tmp876=_tmp875.f1;_tmp877=_tmp871->f4;}}_LL382: Cyc_Toc_exp_to_c(nv,_tmp872);Cyc_Toc_exp_to_c(
nv,_tmp874);Cyc_Toc_exp_to_c(nv,_tmp876);{struct Cyc_Toc_Env _tmp89A;struct
_RegionHandle*_tmp89B;struct Cyc_Toc_Env*_tmp899=nv;_tmp89A=*_tmp899;_tmp89B=
_tmp89A.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp89B,nv),_tmp877);return;};
_LL383: {struct Cyc_Absyn_Switch_s_struct*_tmp878=(struct Cyc_Absyn_Switch_s_struct*)
_tmp85A;if(_tmp878->tag != 10)goto _LL385;else{_tmp879=_tmp878->f1;_tmp87A=_tmp878->f2;}}
_LL384: Cyc_Toc_xlate_switch(nv,s,_tmp879,_tmp87A);return;_LL385: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp87B=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp85A;if(_tmp87B->tag != 11)goto
_LL387;else{_tmp87C=_tmp87B->f1;_tmp87D=_tmp87B->f2;}}_LL386: {struct Cyc_Toc_Env
_tmp89D;struct Cyc_Toc_FallthruInfo*_tmp89E;struct Cyc_Toc_Env*_tmp89C=nv;_tmp89D=*
_tmp89C;_tmp89E=_tmp89D.fallthru_info;if(_tmp89E == 0){const char*_tmpDB3;void*
_tmpDB2;(_tmpDB2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDB3="fallthru in unexpected place",_tag_dyneither(_tmpDB3,sizeof(char),29))),
_tag_dyneither(_tmpDB2,sizeof(void*),0)));}{struct _dyneither_ptr*_tmp8A2;struct
Cyc_List_List*_tmp8A3;struct Cyc_Dict_Dict _tmp8A4;struct Cyc_Toc_FallthruInfo
_tmp8A1=*_tmp89E;_tmp8A2=_tmp8A1.label;_tmp8A3=_tmp8A1.binders;_tmp8A4=_tmp8A1.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp8A2,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp87D)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp8A5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp8A3);struct Cyc_List_List*_tmp8A6=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp87C);for(0;_tmp8A5 != 0;(_tmp8A5=_tmp8A5->tl,
_tmp8A6=_tmp8A6->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp8A6))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp8A4,(
struct _tuple0*)_tmp8A5->hd),(struct Cyc_Absyn_Exp*)_tmp8A6->hd,0),s2,0);}s->r=s2->r;
return;};};};}_LL387: {struct Cyc_Absyn_Decl_s_struct*_tmp87E=(struct Cyc_Absyn_Decl_s_struct*)
_tmp85A;if(_tmp87E->tag != 12)goto _LL389;else{_tmp87F=_tmp87E->f1;_tmp880=_tmp87E->f2;}}
_LL388:{void*_tmp8A7=_tmp87F->r;struct Cyc_Absyn_Vardecl*_tmp8A9;struct Cyc_Absyn_Pat*
_tmp8AB;struct Cyc_Absyn_Exp*_tmp8AC;struct Cyc_List_List*_tmp8AE;struct Cyc_Absyn_Fndecl*
_tmp8B0;struct Cyc_Absyn_Tvar*_tmp8B2;struct Cyc_Absyn_Vardecl*_tmp8B3;int _tmp8B4;
struct Cyc_Absyn_Exp*_tmp8B5;struct Cyc_Absyn_Exp*_tmp8B7;struct Cyc_Absyn_Tvar*
_tmp8B8;struct Cyc_Absyn_Vardecl*_tmp8B9;_LL392: {struct Cyc_Absyn_Var_d_struct*
_tmp8A8=(struct Cyc_Absyn_Var_d_struct*)_tmp8A7;if(_tmp8A8->tag != 0)goto _LL394;
else{_tmp8A9=_tmp8A8->f1;}}_LL393: {struct Cyc_Toc_Env _tmp8BB;struct _RegionHandle*
_tmp8BC;struct Cyc_Toc_Env*_tmp8BA=nv;_tmp8BB=*_tmp8BA;_tmp8BC=_tmp8BB.rgn;{
struct Cyc_Absyn_Local_b_struct _tmpDB6;struct Cyc_Absyn_Local_b_struct*_tmpDB5;
struct Cyc_Toc_Env*_tmp8BD=Cyc_Toc_add_varmap(_tmp8BC,nv,_tmp8A9->name,Cyc_Absyn_varb_exp(
_tmp8A9->name,(void*)((_tmpDB5=_cycalloc(sizeof(*_tmpDB5)),((_tmpDB5[0]=((
_tmpDB6.tag=4,((_tmpDB6.f1=_tmp8A9,_tmpDB6)))),_tmpDB5)))),0));Cyc_Toc_local_decl_to_c(
_tmp8BD,_tmp8BD,_tmp8A9,_tmp880);}goto _LL391;}_LL394: {struct Cyc_Absyn_Let_d_struct*
_tmp8AA=(struct Cyc_Absyn_Let_d_struct*)_tmp8A7;if(_tmp8AA->tag != 2)goto _LL396;
else{_tmp8AB=_tmp8AA->f1;_tmp8AC=_tmp8AA->f3;}}_LL395:{void*_tmp8C0=_tmp8AB->r;
struct Cyc_Absyn_Vardecl*_tmp8C2;struct Cyc_Absyn_Pat*_tmp8C3;struct Cyc_Absyn_Pat
_tmp8C4;void*_tmp8C5;_LL3A1: {struct Cyc_Absyn_Var_p_struct*_tmp8C1=(struct Cyc_Absyn_Var_p_struct*)
_tmp8C0;if(_tmp8C1->tag != 1)goto _LL3A3;else{_tmp8C2=_tmp8C1->f1;_tmp8C3=_tmp8C1->f2;
_tmp8C4=*_tmp8C3;_tmp8C5=_tmp8C4.r;{struct Cyc_Absyn_Wild_p_struct*_tmp8C6=(
struct Cyc_Absyn_Wild_p_struct*)_tmp8C5;if(_tmp8C6->tag != 0)goto _LL3A3;};}}_LL3A2: {
struct _tuple0*old_name=_tmp8C2->name;struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp8C2->name=new_name;_tmp8C2->initializer=(struct Cyc_Absyn_Exp*)_tmp8AC;{
struct Cyc_Absyn_Var_d_struct _tmpDB9;struct Cyc_Absyn_Var_d_struct*_tmpDB8;_tmp87F->r=(
void*)((_tmpDB8=_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8[0]=((_tmpDB9.tag=0,((
_tmpDB9.f1=_tmp8C2,_tmpDB9)))),_tmpDB8))));}{struct Cyc_Toc_Env _tmp8CA;struct
_RegionHandle*_tmp8CB;struct Cyc_Toc_Env*_tmp8C9=nv;_tmp8CA=*_tmp8C9;_tmp8CB=
_tmp8CA.rgn;{struct Cyc_Absyn_Local_b_struct _tmpDBC;struct Cyc_Absyn_Local_b_struct*
_tmpDBB;struct Cyc_Toc_Env*_tmp8CC=Cyc_Toc_add_varmap(_tmp8CB,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpDBB=_cycalloc(sizeof(*_tmpDBB)),((_tmpDBB[0]=((_tmpDBC.tag=
4,((_tmpDBC.f1=_tmp8C2,_tmpDBC)))),_tmpDBB)))),0));Cyc_Toc_local_decl_to_c(
_tmp8CC,nv,_tmp8C2,_tmp880);}goto _LL3A0;};}_LL3A3:;_LL3A4: s->r=(Cyc_Toc_letdecl_to_c(
nv,_tmp8AB,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp8AC->topt))->v,_tmp8AC,
_tmp880))->r;goto _LL3A0;_LL3A0:;}goto _LL391;_LL396: {struct Cyc_Absyn_Letv_d_struct*
_tmp8AD=(struct Cyc_Absyn_Letv_d_struct*)_tmp8A7;if(_tmp8AD->tag != 3)goto _LL398;
else{_tmp8AE=_tmp8AD->f1;}}_LL397: {struct Cyc_List_List*_tmp8CF=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp8AE);if(_tmp8CF == 0){const char*_tmpDBF;
void*_tmpDBE;(_tmpDBE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpDBF="empty Letv_d",_tag_dyneither(_tmpDBF,sizeof(char),
13))),_tag_dyneither(_tmpDBE,sizeof(void*),0)));}{struct Cyc_Absyn_Var_d_struct
_tmpDC2;struct Cyc_Absyn_Var_d_struct*_tmpDC1;_tmp87F->r=(void*)((_tmpDC1=
_cycalloc(sizeof(*_tmpDC1)),((_tmpDC1[0]=((_tmpDC2.tag=0,((_tmpDC2.f1=(struct Cyc_Absyn_Vardecl*)
_tmp8CF->hd,_tmpDC2)))),_tmpDC1))));}_tmp8CF=_tmp8CF->tl;for(0;_tmp8CF != 0;
_tmp8CF=_tmp8CF->tl){struct Cyc_Absyn_Var_d_struct _tmpDC5;struct Cyc_Absyn_Var_d_struct*
_tmpDC4;struct Cyc_Absyn_Decl*_tmp8D4=Cyc_Absyn_new_decl((void*)((_tmpDC4=
_cycalloc(sizeof(*_tmpDC4)),((_tmpDC4[0]=((_tmpDC5.tag=0,((_tmpDC5.f1=(struct Cyc_Absyn_Vardecl*)
_tmp8CF->hd,_tmpDC5)))),_tmpDC4)))),0);s->r=(Cyc_Absyn_decl_stmt(_tmp8D4,Cyc_Absyn_new_stmt(
s->r,0),0))->r;}Cyc_Toc_stmt_to_c(nv,s);goto _LL391;}_LL398: {struct Cyc_Absyn_Fn_d_struct*
_tmp8AF=(struct Cyc_Absyn_Fn_d_struct*)_tmp8A7;if(_tmp8AF->tag != 1)goto _LL39A;
else{_tmp8B0=_tmp8AF->f1;}}_LL399: {struct _tuple0*_tmp8D7=_tmp8B0->name;struct
Cyc_Toc_Env _tmp8D9;struct _RegionHandle*_tmp8DA;struct Cyc_Toc_Env*_tmp8D8=nv;
_tmp8D9=*_tmp8D8;_tmp8DA=_tmp8D9.rgn;{struct Cyc_Toc_Env*_tmp8DB=Cyc_Toc_add_varmap(
_tmp8DA,nv,_tmp8B0->name,Cyc_Absyn_var_exp(_tmp8D7,0));Cyc_Toc_fndecl_to_c(
_tmp8DB,_tmp8B0,0);Cyc_Toc_stmt_to_c(_tmp8DB,_tmp880);}goto _LL391;}_LL39A: {
struct Cyc_Absyn_Region_d_struct*_tmp8B1=(struct Cyc_Absyn_Region_d_struct*)
_tmp8A7;if(_tmp8B1->tag != 4)goto _LL39C;else{_tmp8B2=_tmp8B1->f1;_tmp8B3=_tmp8B1->f2;
_tmp8B4=_tmp8B1->f3;_tmp8B5=_tmp8B1->f4;}}_LL39B: {struct Cyc_Absyn_Stmt*_tmp8DC=
_tmp880;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple0*
rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp8B3->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp8DE;struct _RegionHandle*_tmp8DF;struct Cyc_Toc_Env*
_tmp8DD=nv;_tmp8DE=*_tmp8DD;_tmp8DF=_tmp8DE.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp8DF,nv,x_var,x_exp),_tmp8DC);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp8DC,0))->r;
else{if(_tmp8B5 == 0){struct Cyc_Absyn_Exp*_tmpDC8[1];struct Cyc_Absyn_Exp*_tmpDC7[
1];struct Cyc_List_List*_tmpDC6;s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpDC6=
_cycalloc(sizeof(*_tmpDC6)),((_tmpDC6->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0),((_tmpDC6->tl=0,_tmpDC6)))))),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpDC7[0]=
x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDC7,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp8DC,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpDC8[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDC8,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp8B5);{struct Cyc_Absyn_Exp*
_tmpDCA[1];struct Cyc_Absyn_Exp*_tmpDC9[2];s->r=(Cyc_Absyn_declare_stmt(rh_var,
Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpDC9[1]=(struct Cyc_Absyn_Exp*)_tmp8B5,((_tmpDC9[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDC9,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp8DC,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpDCA[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDCA,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
return;}_LL39C: {struct Cyc_Absyn_Alias_d_struct*_tmp8B6=(struct Cyc_Absyn_Alias_d_struct*)
_tmp8A7;if(_tmp8B6->tag != 5)goto _LL39E;else{_tmp8B7=_tmp8B6->f1;_tmp8B8=_tmp8B6->f2;
_tmp8B9=_tmp8B6->f3;}}_LL39D: {struct _tuple0*old_name=_tmp8B9->name;struct
_tuple0*new_name=Cyc_Toc_temp_var();_tmp8B9->name=new_name;_tmp8B9->initializer=(
struct Cyc_Absyn_Exp*)_tmp8B7;{struct Cyc_Absyn_Decl_s_struct _tmpDD9;struct Cyc_Absyn_Var_d_struct*
_tmpDD8;struct Cyc_Absyn_Var_d_struct _tmpDD7;struct Cyc_Absyn_Decl*_tmpDD6;struct
Cyc_Absyn_Decl_s_struct*_tmpDD5;s->r=(void*)((_tmpDD5=_cycalloc(sizeof(*_tmpDD5)),((
_tmpDD5[0]=((_tmpDD9.tag=12,((_tmpDD9.f1=((_tmpDD6=_cycalloc(sizeof(*_tmpDD6)),((
_tmpDD6->r=(void*)((_tmpDD8=_cycalloc(sizeof(*_tmpDD8)),((_tmpDD8[0]=((_tmpDD7.tag=
0,((_tmpDD7.f1=_tmp8B9,_tmpDD7)))),_tmpDD8)))),((_tmpDD6->loc=0,_tmpDD6)))))),((
_tmpDD9.f2=_tmp880,_tmpDD9)))))),_tmpDD5))));}{struct Cyc_Toc_Env _tmp8EB;struct
_RegionHandle*_tmp8EC;struct Cyc_Toc_Env*_tmp8EA=nv;_tmp8EB=*_tmp8EA;_tmp8EC=
_tmp8EB.rgn;{struct Cyc_Absyn_Local_b_struct _tmpDDC;struct Cyc_Absyn_Local_b_struct*
_tmpDDB;struct Cyc_Toc_Env*_tmp8ED=Cyc_Toc_add_varmap(_tmp8EC,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpDDB=_cycalloc(sizeof(*_tmpDDB)),((_tmpDDB[0]=((_tmpDDC.tag=
4,((_tmpDDC.f1=_tmp8B9,_tmpDDC)))),_tmpDDB)))),0));Cyc_Toc_local_decl_to_c(
_tmp8ED,nv,_tmp8B9,_tmp880);}return;};}_LL39E:;_LL39F: {const char*_tmpDDF;void*
_tmpDDE;(_tmpDDE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpDDF="bad nested declaration within function",_tag_dyneither(_tmpDDF,sizeof(
char),39))),_tag_dyneither(_tmpDDE,sizeof(void*),0)));}_LL391:;}return;_LL389: {
struct Cyc_Absyn_Label_s_struct*_tmp881=(struct Cyc_Absyn_Label_s_struct*)_tmp85A;
if(_tmp881->tag != 13)goto _LL38B;else{_tmp882=_tmp881->f1;_tmp883=_tmp881->f2;}}
_LL38A: s=_tmp883;continue;_LL38B: {struct Cyc_Absyn_Do_s_struct*_tmp884=(struct
Cyc_Absyn_Do_s_struct*)_tmp85A;if(_tmp884->tag != 14)goto _LL38D;else{_tmp885=
_tmp884->f1;_tmp886=_tmp884->f2;_tmp887=_tmp886.f1;}}_LL38C: {struct Cyc_Toc_Env
_tmp8F3;struct _RegionHandle*_tmp8F4;struct Cyc_Toc_Env*_tmp8F2=nv;_tmp8F3=*
_tmp8F2;_tmp8F4=_tmp8F3.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp8F4,nv),
_tmp885);Cyc_Toc_exp_to_c(nv,_tmp887);return;}_LL38D: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp888=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp85A;if(_tmp888->tag != 15)goto
_LL38F;else{_tmp889=_tmp888->f1;_tmp88A=_tmp888->f2;}}_LL38E: {struct _tuple0*
h_var=Cyc_Toc_temp_var();struct _tuple0*e_var=Cyc_Toc_temp_var();struct _tuple0*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);{struct Cyc_Core_Opt*_tmpDE0;
e_exp->topt=((_tmpDE0=_cycalloc(sizeof(*_tmpDE0)),((_tmpDE0->v=e_typ,_tmpDE0))));}{
struct Cyc_Toc_Env _tmp8F7;struct _RegionHandle*_tmp8F8;struct Cyc_Toc_Env*_tmp8F6=
nv;_tmp8F7=*_tmp8F6;_tmp8F8=_tmp8F7.rgn;{struct Cyc_Toc_Env*_tmp8F9=Cyc_Toc_add_varmap(
_tmp8F8,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp8F9,_tmp889);{struct Cyc_Absyn_Stmt*
_tmp8FA=Cyc_Absyn_seq_stmt(_tmp889,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple0*_tmp8FB=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp8FC=
Cyc_Absyn_var_exp(_tmp8FB,0);struct Cyc_Absyn_Vardecl*_tmp8FD=Cyc_Absyn_new_vardecl(
_tmp8FB,Cyc_Absyn_exn_typ(),0);{struct Cyc_Core_Opt*_tmpDE1;_tmp8FC->topt=((
_tmpDE1=_cycalloc(sizeof(*_tmpDE1)),((_tmpDE1->v=Cyc_Absyn_exn_typ(),_tmpDE1))));}{
struct Cyc_Core_Opt*_tmpDF2;struct Cyc_Absyn_Var_p_struct*_tmpDF1;struct Cyc_Absyn_Pat*
_tmpDF0;struct Cyc_Core_Opt*_tmpDEF;struct Cyc_Absyn_Var_p_struct _tmpDEE;struct Cyc_Absyn_Pat*
_tmpDED;struct Cyc_Absyn_Pat*_tmp8FF=(_tmpDED=_cycalloc(sizeof(*_tmpDED)),((
_tmpDED->r=(void*)((_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((_tmpDF1[0]=((_tmpDEE.tag=
1,((_tmpDEE.f1=_tmp8FD,((_tmpDEE.f2=((_tmpDF0=_cycalloc(sizeof(*_tmpDF0)),((
_tmpDF0->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpDF0->topt=((_tmpDEF=_cycalloc(
sizeof(*_tmpDEF)),((_tmpDEF->v=Cyc_Absyn_exn_typ(),_tmpDEF)))),((_tmpDF0->loc=0,
_tmpDF0)))))))),_tmpDEE)))))),_tmpDF1)))),((_tmpDED->topt=((_tmpDF2=_cycalloc(
sizeof(*_tmpDF2)),((_tmpDF2->v=Cyc_Absyn_exn_typ(),_tmpDF2)))),((_tmpDED->loc=0,
_tmpDED)))))));struct Cyc_Absyn_Exp*_tmp900=Cyc_Absyn_throw_exp(_tmp8FC,0);{
struct Cyc_Core_Opt*_tmpDF3;_tmp900->topt=((_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((
_tmpDF3->v=(void*)& Cyc_Absyn_VoidType_val,_tmpDF3))));}{struct Cyc_Absyn_Stmt*
_tmp902=Cyc_Absyn_exp_stmt(_tmp900,0);struct Cyc_Core_Opt*_tmpDF9;struct Cyc_List_List*
_tmpDF8;struct Cyc_Absyn_Switch_clause*_tmpDF7;struct Cyc_Absyn_Switch_clause*
_tmp903=(_tmpDF7=_cycalloc(sizeof(*_tmpDF7)),((_tmpDF7->pattern=_tmp8FF,((
_tmpDF7->pat_vars=((_tmpDF9=_cycalloc(sizeof(*_tmpDF9)),((_tmpDF9->v=((_tmpDF8=
_cycalloc(sizeof(*_tmpDF8)),((_tmpDF8->hd=_tmp8FD,((_tmpDF8->tl=0,_tmpDF8)))))),
_tmpDF9)))),((_tmpDF7->where_clause=0,((_tmpDF7->body=_tmp902,((_tmpDF7->loc=0,
_tmpDF7)))))))))));struct Cyc_List_List*_tmpDFA;struct Cyc_Absyn_Stmt*_tmp904=Cyc_Absyn_switch_stmt(
e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp88A,((_tmpDFA=_cycalloc(sizeof(*_tmpDFA)),((_tmpDFA->hd=_tmp903,((_tmpDFA->tl=
0,_tmpDFA))))))),0);Cyc_Toc_stmt_to_c(_tmp8F9,_tmp904);{struct Cyc_List_List*
_tmpDFB;struct Cyc_Absyn_Exp*_tmp905=Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,((
_tmpDFB=_cycalloc(sizeof(*_tmpDFB)),((_tmpDFB->hd=Cyc_Absyn_aggrmember_exp(h_exp,
Cyc_Toc_handler_sp,0),((_tmpDFB->tl=0,_tmpDFB)))))),0);struct Cyc_List_List*
_tmpDFC;struct Cyc_Absyn_Stmt*_tmp906=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpDFC->tl=0,_tmpDFC)))))),0),0);struct Cyc_Absyn_Exp*_tmp907=Cyc_Absyn_int_exp(
Cyc_Absyn_Signed,0,0);struct Cyc_Absyn_Exp*_tmp908=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
1,0);s->r=(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp906,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp907,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp905,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp908,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp8FA,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp904,0),0),0),0),0),0))->r;};};};};}
return;};}_LL38F: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp88B=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp85A;if(_tmp88B->tag != 16)goto _LL36E;else{_tmp88C=_tmp88B->f1;}}_LL390: if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp88C);{struct Cyc_List_List*
_tmpDFD;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((
_tmpDFD=_cycalloc(sizeof(*_tmpDFD)),((_tmpDFD->hd=_tmp88C,((_tmpDFD->tl=0,
_tmpDFD)))))),0));};}return;_LL36E:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp916=_new_region("frgn");struct _RegionHandle*frgn=& _tmp916;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp917=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp918=f->args;for(0;_tmp918 != 0;_tmp918=_tmp918->tl){struct
_tuple0*_tmpDFE;struct _tuple0*_tmp919=(_tmpDFE=_cycalloc(sizeof(*_tmpDFE)),((
_tmpDFE->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpDFE->f2=(*((struct
_tuple26*)_tmp918->hd)).f1,_tmpDFE)))));(*((struct _tuple26*)_tmp918->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple26*)_tmp918->hd)).f3);_tmp917=Cyc_Toc_add_varmap(frgn,_tmp917,
_tmp919,Cyc_Absyn_var_exp(_tmp919,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp91C;struct Cyc_Absyn_Tqual
_tmp91D;void*_tmp91E;int _tmp91F;struct Cyc_Absyn_VarargInfo _tmp91B=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp91C=_tmp91B.name;_tmp91D=_tmp91B.tq;_tmp91E=
_tmp91B.type;_tmp91F=_tmp91B.inject;{void*_tmp920=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp91E,(void*)& Cyc_Absyn_HeapRgn_val,_tmp91D,Cyc_Absyn_false_conref));struct
_tuple0*_tmpDFF;struct _tuple0*_tmp921=(_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((
_tmpDFF->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpDFF->f2=(struct
_dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null(_tmp91C))->v,_tmpDFF)))));{
struct _tuple26*_tmpE02;struct Cyc_List_List*_tmpE01;f->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpE01=
_cycalloc(sizeof(*_tmpE01)),((_tmpE01->hd=((_tmpE02=_cycalloc(sizeof(*_tmpE02)),((
_tmpE02->f1=(struct _dyneither_ptr*)_tmp91C->v,((_tmpE02->f2=_tmp91D,((_tmpE02->f3=
_tmp920,_tmpE02)))))))),((_tmpE01->tl=0,_tmpE01)))))));}_tmp917=Cyc_Toc_add_varmap(
frgn,_tmp917,_tmp921,Cyc_Absyn_var_exp(_tmp921,0));f->cyc_varargs=0;};}{struct
Cyc_List_List*_tmp925=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp925 != 0;_tmp925=_tmp925->tl){((struct Cyc_Absyn_Vardecl*)_tmp925->hd)->type=
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp925->hd)->type);}}Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel(frgn,_tmp917),f->body);};_pop_region(frgn);};}static enum 
Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){switch(s){case Cyc_Absyn_Abstract:
_LL3A5: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3A6: return Cyc_Absyn_Extern;
default: _LL3A7: return s;}}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad,int add_to_decls){struct _tuple0*_tmp926=ad->name;struct _DynRegionHandle*
_tmp928;struct Cyc_Dict_Dict*_tmp929;struct Cyc_Toc_TocState _tmp927=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp928=_tmp927.dyn;_tmp929=_tmp927.aggrs_so_far;{
struct _DynRegionFrame _tmp92A;struct _RegionHandle*d=_open_dynregion(& _tmp92A,
_tmp928);{int seen_defn_before;struct _tuple14**_tmp92B=((struct _tuple14**(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp929,_tmp926);if(
_tmp92B == 0){seen_defn_before=0;{struct _tuple14*v;if(ad->kind == Cyc_Absyn_StructA){
struct _tuple14*_tmpE03;v=((_tmpE03=_region_malloc(d,sizeof(*_tmpE03)),((_tmpE03->f1=
ad,((_tmpE03->f2=Cyc_Absyn_strctq(_tmp926),_tmpE03))))));}else{struct _tuple14*
_tmpE04;v=((_tmpE04=_region_malloc(d,sizeof(*_tmpE04)),((_tmpE04->f1=ad,((
_tmpE04->f2=Cyc_Absyn_unionq_typ(_tmp926),_tmpE04))))));}*_tmp929=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp929,
_tmp926,v);};}else{struct _tuple14 _tmp92F;struct Cyc_Absyn_Aggrdecl*_tmp930;void*
_tmp931;struct _tuple14*_tmp92E=*_tmp92B;_tmp92F=*_tmp92E;_tmp930=_tmp92F.f1;
_tmp931=_tmp92F.f2;if(_tmp930->impl == 0){{struct _tuple14*_tmpE05;*_tmp929=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))
Cyc_Dict_insert)(*_tmp929,_tmp926,((_tmpE05=_region_malloc(d,sizeof(*_tmpE05)),((
_tmpE05->f1=ad,((_tmpE05->f2=_tmp931,_tmpE05)))))));}seen_defn_before=0;}else{
seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*_tmpE06;struct Cyc_Absyn_Aggrdecl*
new_ad=(_tmpE06=_cycalloc(sizeof(*_tmpE06)),((_tmpE06->kind=ad->kind,((_tmpE06->sc=
Cyc_Absyn_Public,((_tmpE06->name=ad->name,((_tmpE06->tvs=0,((_tmpE06->impl=0,((
_tmpE06->attributes=ad->attributes,_tmpE06)))))))))))));if(ad->impl != 0  && !
seen_defn_before){{struct Cyc_Absyn_AggrdeclImpl*_tmpE07;new_ad->impl=((_tmpE07=
_cycalloc(sizeof(*_tmpE07)),((_tmpE07->exist_vars=0,((_tmpE07->rgn_po=0,((
_tmpE07->fields=0,((_tmpE07->tagged=0,_tmpE07))))))))));}{struct Cyc_List_List*
new_fields=0;{struct Cyc_List_List*_tmp934=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp934 != 0;_tmp934=_tmp934->tl){struct Cyc_Absyn_Aggrfield*
_tmp935=(struct Cyc_Absyn_Aggrfield*)_tmp934->hd;if(_tmp934->tl == 0  && Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp935->type)))continue;{struct Cyc_Absyn_Aggrfield*
_tmpE08;struct Cyc_Absyn_Aggrfield*_tmp936=(_tmpE08=_cycalloc(sizeof(*_tmpE08)),((
_tmpE08->name=_tmp935->name,((_tmpE08->tq=Cyc_Toc_mt_tq,((_tmpE08->type=Cyc_Toc_typ_to_c_array(
_tmp935->type),((_tmpE08->width=_tmp935->width,((_tmpE08->attributes=_tmp935->attributes,
_tmpE08)))))))))));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp937=_tmp936->type;struct _dyneither_ptr*_tmp938=_tmp936->name;const char*
_tmpE0D;void*_tmpE0C[2];struct Cyc_String_pa_struct _tmpE0B;struct Cyc_String_pa_struct
_tmpE0A;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpE0A.tag=0,((_tmpE0A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp938),((_tmpE0B.tag=0,((_tmpE0B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpE0C[0]=&
_tmpE0B,((_tmpE0C[1]=& _tmpE0A,Cyc_aprintf(((_tmpE0D="_union_%s_%s",
_tag_dyneither(_tmpE0D,sizeof(char),13))),_tag_dyneither(_tmpE0C,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpE0E;struct _dyneither_ptr*str=(_tmpE0E=_cycalloc(sizeof(*
_tmpE0E)),((_tmpE0E[0]=s,_tmpE0E)));struct Cyc_Absyn_Aggrfield*_tmpE0F;struct Cyc_Absyn_Aggrfield*
_tmp939=(_tmpE0F=_cycalloc(sizeof(*_tmpE0F)),((_tmpE0F->name=Cyc_Toc_val_sp,((
_tmpE0F->tq=Cyc_Toc_mt_tq,((_tmpE0F->type=_tmp937,((_tmpE0F->width=0,((_tmpE0F->attributes=
0,_tmpE0F)))))))))));struct Cyc_Absyn_Aggrfield*_tmpE10;struct Cyc_Absyn_Aggrfield*
_tmp93A=(_tmpE10=_cycalloc(sizeof(*_tmpE10)),((_tmpE10->name=Cyc_Toc_tag_sp,((
_tmpE10->tq=Cyc_Toc_mt_tq,((_tmpE10->type=Cyc_Absyn_sint_typ,((_tmpE10->width=0,((
_tmpE10->attributes=0,_tmpE10)))))))))));struct Cyc_Absyn_Aggrfield*_tmpE11[2];
struct Cyc_List_List*_tmp93B=(_tmpE11[1]=_tmp939,((_tmpE11[0]=_tmp93A,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpE11,sizeof(struct Cyc_Absyn_Aggrfield*),
2)))));struct Cyc_Absyn_AggrdeclImpl*_tmpE16;struct _tuple0*_tmpE15;struct Cyc_Absyn_Aggrdecl*
_tmpE14;struct Cyc_Absyn_Aggrdecl*_tmp93C=(_tmpE14=_cycalloc(sizeof(*_tmpE14)),((
_tmpE14->kind=Cyc_Absyn_StructA,((_tmpE14->sc=Cyc_Absyn_Public,((_tmpE14->name=((
_tmpE15=_cycalloc(sizeof(*_tmpE15)),((_tmpE15->f1=Cyc_Absyn_Loc_n,((_tmpE15->f2=
str,_tmpE15)))))),((_tmpE14->tvs=0,((_tmpE14->impl=((_tmpE16=_cycalloc(sizeof(*
_tmpE16)),((_tmpE16->exist_vars=0,((_tmpE16->rgn_po=0,((_tmpE16->fields=_tmp93B,((
_tmpE16->tagged=0,_tmpE16)))))))))),((_tmpE14->attributes=0,_tmpE14)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmpE1C;struct Cyc_Absyn_Aggr_d_struct _tmpE1B;
struct Cyc_List_List*_tmpE1A;Cyc_Toc_result_decls=((_tmpE1A=_cycalloc(sizeof(*
_tmpE1A)),((_tmpE1A->hd=Cyc_Absyn_new_decl((void*)((_tmpE1C=_cycalloc(sizeof(*
_tmpE1C)),((_tmpE1C[0]=((_tmpE1B.tag=6,((_tmpE1B.f1=_tmp93C,_tmpE1B)))),_tmpE1C)))),
0),((_tmpE1A->tl=Cyc_Toc_result_decls,_tmpE1A))))));}_tmp936->type=Cyc_Absyn_strct(
str);}{struct Cyc_List_List*_tmpE1D;new_fields=((_tmpE1D=_cycalloc(sizeof(*
_tmpE1D)),((_tmpE1D->hd=_tmp936,((_tmpE1D->tl=new_fields,_tmpE1D))))));};};}}(
new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_fields);};}if(add_to_decls){struct Cyc_Absyn_Decl*_tmpE27;struct Cyc_Absyn_Aggr_d_struct
_tmpE26;struct Cyc_Absyn_Aggr_d_struct*_tmpE25;struct Cyc_List_List*_tmpE24;Cyc_Toc_result_decls=((
_tmpE24=_cycalloc(sizeof(*_tmpE24)),((_tmpE24->hd=((_tmpE27=_cycalloc(sizeof(*
_tmpE27)),((_tmpE27->r=(void*)((_tmpE25=_cycalloc(sizeof(*_tmpE25)),((_tmpE25[0]=((
_tmpE26.tag=6,((_tmpE26.f1=new_ad,_tmpE26)))),_tmpE25)))),((_tmpE27->loc=0,
_tmpE27)))))),((_tmpE24->tl=Cyc_Toc_result_decls,_tmpE24))))));}};};
_pop_dynregion(d);};}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud){struct _DynRegionHandle*_tmp953;struct Cyc_Set_Set**_tmp954;struct Cyc_Toc_TocState
_tmp952=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp953=
_tmp952.dyn;_tmp954=_tmp952.datatypes_so_far;{struct _DynRegionFrame _tmp955;
struct _RegionHandle*d=_open_dynregion(& _tmp955,_tmp953);{struct _tuple0*_tmp956=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))
Cyc_Set_member)(*_tmp954,_tmp956)){_npop_handler(0);return;}*_tmp954=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*
_tmp954,_tmp956);}{struct Cyc_List_List*_tmp957=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp957 != 0;_tmp957=_tmp957->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp957->hd;struct Cyc_List_List*_tmp958=0;int i=
1;{struct Cyc_List_List*_tmp959=f->typs;for(0;_tmp959 != 0;(_tmp959=_tmp959->tl,i
++)){struct _dyneither_ptr*_tmp95A=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpE28;struct Cyc_Absyn_Aggrfield*_tmp95B=(_tmpE28=_cycalloc(sizeof(*_tmpE28)),((
_tmpE28->name=_tmp95A,((_tmpE28->tq=(*((struct _tuple9*)_tmp959->hd)).f1,((
_tmpE28->type=Cyc_Toc_typ_to_c_array((*((struct _tuple9*)_tmp959->hd)).f2),((
_tmpE28->width=0,((_tmpE28->attributes=0,_tmpE28)))))))))));struct Cyc_List_List*
_tmpE29;_tmp958=((_tmpE29=_cycalloc(sizeof(*_tmpE29)),((_tmpE29->hd=_tmp95B,((
_tmpE29->tl=_tmp958,_tmpE29))))));}}{struct Cyc_Absyn_Aggrfield*_tmpE2C;struct Cyc_List_List*
_tmpE2B;_tmp958=((_tmpE2B=_cycalloc(sizeof(*_tmpE2B)),((_tmpE2B->hd=((_tmpE2C=
_cycalloc(sizeof(*_tmpE2C)),((_tmpE2C->name=Cyc_Toc_tag_sp,((_tmpE2C->tq=Cyc_Toc_mt_tq,((
_tmpE2C->type=Cyc_Absyn_sint_typ,((_tmpE2C->width=0,((_tmpE2C->attributes=0,
_tmpE2C)))))))))))),((_tmpE2B->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp958),_tmpE2B))))));}{struct Cyc_Absyn_AggrdeclImpl*
_tmpE31;const char*_tmpE30;struct Cyc_Absyn_Aggrdecl*_tmpE2F;struct Cyc_Absyn_Aggrdecl*
_tmp960=(_tmpE2F=_cycalloc(sizeof(*_tmpE2F)),((_tmpE2F->kind=Cyc_Absyn_StructA,((
_tmpE2F->sc=Cyc_Absyn_Public,((_tmpE2F->name=Cyc_Toc_collapse_qvar_tag(f->name,((
_tmpE30="_struct",_tag_dyneither(_tmpE30,sizeof(char),8)))),((_tmpE2F->tvs=0,((
_tmpE2F->impl=((_tmpE31=_cycalloc(sizeof(*_tmpE31)),((_tmpE31->exist_vars=0,((
_tmpE31->rgn_po=0,((_tmpE31->fields=_tmp958,((_tmpE31->tagged=0,_tmpE31)))))))))),((
_tmpE2F->attributes=0,_tmpE2F)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpE37;
struct Cyc_Absyn_Aggr_d_struct _tmpE36;struct Cyc_List_List*_tmpE35;Cyc_Toc_result_decls=((
_tmpE35=_cycalloc(sizeof(*_tmpE35)),((_tmpE35->hd=Cyc_Absyn_new_decl((void*)((
_tmpE37=_cycalloc(sizeof(*_tmpE37)),((_tmpE37[0]=((_tmpE36.tag=6,((_tmpE36.f1=
_tmp960,_tmpE36)))),_tmpE37)))),0),((_tmpE35->tl=Cyc_Toc_result_decls,_tmpE35))))));};}};;
_pop_dynregion(d);};}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd){if(xd->fields == 0)return;{struct _DynRegionHandle*_tmp968;struct Cyc_Dict_Dict*
_tmp969;struct Cyc_Toc_TocState _tmp967=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp968=_tmp967.dyn;_tmp969=_tmp967.xdatatypes_so_far;{struct _DynRegionFrame
_tmp96A;struct _RegionHandle*d=_open_dynregion(& _tmp96A,_tmp968);{struct _tuple0*
_tmp96B=xd->name;struct Cyc_List_List*_tmp96C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp96C != 0;_tmp96C=_tmp96C->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp96C->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp96D=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp96E=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp96D,Cyc_Absyn_false_conref,0);int*_tmp96F=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp969,f->name);int
_tmp970;_LL3AA: if(_tmp96F != 0)goto _LL3AC;_LL3AB: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp971=Cyc_Absyn_new_vardecl(f->name,_tmp96E,
initopt);_tmp971->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*_tmpE3D;struct Cyc_Absyn_Var_d_struct
_tmpE3C;struct Cyc_List_List*_tmpE3B;Cyc_Toc_result_decls=((_tmpE3B=_cycalloc(
sizeof(*_tmpE3B)),((_tmpE3B->hd=Cyc_Absyn_new_decl((void*)((_tmpE3D=_cycalloc(
sizeof(*_tmpE3D)),((_tmpE3D[0]=((_tmpE3C.tag=0,((_tmpE3C.f1=_tmp971,_tmpE3C)))),
_tmpE3D)))),0),((_tmpE3B->tl=Cyc_Toc_result_decls,_tmpE3B))))));}*_tmp969=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp969,f->name,f->sc != Cyc_Absyn_Extern);{struct Cyc_List_List*fields=0;int i=1;{
struct Cyc_List_List*_tmp975=f->typs;for(0;_tmp975 != 0;(_tmp975=_tmp975->tl,i ++)){
struct Cyc_Int_pa_struct _tmpE45;void*_tmpE44[1];const char*_tmpE43;struct
_dyneither_ptr*_tmpE42;struct _dyneither_ptr*_tmp976=(_tmpE42=_cycalloc(sizeof(*
_tmpE42)),((_tmpE42[0]=(struct _dyneither_ptr)((_tmpE45.tag=1,((_tmpE45.f1=(
unsigned long)i,((_tmpE44[0]=& _tmpE45,Cyc_aprintf(((_tmpE43="f%d",_tag_dyneither(
_tmpE43,sizeof(char),4))),_tag_dyneither(_tmpE44,sizeof(void*),1)))))))),_tmpE42)));
struct Cyc_Absyn_Aggrfield*_tmpE46;struct Cyc_Absyn_Aggrfield*_tmp977=(_tmpE46=
_cycalloc(sizeof(*_tmpE46)),((_tmpE46->name=_tmp976,((_tmpE46->tq=(*((struct
_tuple9*)_tmp975->hd)).f1,((_tmpE46->type=Cyc_Toc_typ_to_c_array((*((struct
_tuple9*)_tmp975->hd)).f2),((_tmpE46->width=0,((_tmpE46->attributes=0,_tmpE46)))))))))));
struct Cyc_List_List*_tmpE47;fields=((_tmpE47=_cycalloc(sizeof(*_tmpE47)),((
_tmpE47->hd=_tmp977,((_tmpE47->tl=fields,_tmpE47))))));}}{struct Cyc_Absyn_Aggrfield*
_tmpE4A;struct Cyc_List_List*_tmpE49;fields=((_tmpE49=_cycalloc(sizeof(*_tmpE49)),((
_tmpE49->hd=((_tmpE4A=_cycalloc(sizeof(*_tmpE4A)),((_tmpE4A->name=Cyc_Toc_tag_sp,((
_tmpE4A->tq=Cyc_Toc_mt_tq,((_tmpE4A->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq),((_tmpE4A->width=0,((_tmpE4A->attributes=0,_tmpE4A)))))))))))),((
_tmpE49->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmpE49))))));}{struct Cyc_Absyn_AggrdeclImpl*_tmpE4F;const char*_tmpE4E;
struct Cyc_Absyn_Aggrdecl*_tmpE4D;struct Cyc_Absyn_Aggrdecl*_tmp980=(_tmpE4D=
_cycalloc(sizeof(*_tmpE4D)),((_tmpE4D->kind=Cyc_Absyn_StructA,((_tmpE4D->sc=Cyc_Absyn_Public,((
_tmpE4D->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpE4E="_struct",
_tag_dyneither(_tmpE4E,sizeof(char),8)))),((_tmpE4D->tvs=0,((_tmpE4D->impl=((
_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((_tmpE4F->exist_vars=0,((_tmpE4F->rgn_po=0,((
_tmpE4F->fields=fields,((_tmpE4F->tagged=0,_tmpE4F)))))))))),((_tmpE4D->attributes=
0,_tmpE4D)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpE55;struct Cyc_Absyn_Aggr_d_struct
_tmpE54;struct Cyc_List_List*_tmpE53;Cyc_Toc_result_decls=((_tmpE53=_cycalloc(
sizeof(*_tmpE53)),((_tmpE53->hd=Cyc_Absyn_new_decl((void*)((_tmpE55=_cycalloc(
sizeof(*_tmpE55)),((_tmpE55[0]=((_tmpE54.tag=6,((_tmpE54.f1=_tmp980,_tmpE54)))),
_tmpE55)))),0),((_tmpE53->tl=Cyc_Toc_result_decls,_tmpE53))))));}goto _LL3A9;};};};}
_LL3AC: if(_tmp96F == 0)goto _LL3AE;_tmp970=*_tmp96F;if(_tmp970 != 0)goto _LL3AE;
_LL3AD: if(f->sc != Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*_tmp987=Cyc_Absyn_string_exp(*
fn,0);struct Cyc_Absyn_Vardecl*_tmp988=Cyc_Absyn_new_vardecl(f->name,_tmp96E,(
struct Cyc_Absyn_Exp*)_tmp987);_tmp988->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*
_tmpE5B;struct Cyc_Absyn_Var_d_struct _tmpE5A;struct Cyc_List_List*_tmpE59;Cyc_Toc_result_decls=((
_tmpE59=_cycalloc(sizeof(*_tmpE59)),((_tmpE59->hd=Cyc_Absyn_new_decl((void*)((
_tmpE5B=_cycalloc(sizeof(*_tmpE5B)),((_tmpE5B[0]=((_tmpE5A.tag=0,((_tmpE5A.f1=
_tmp988,_tmpE5A)))),_tmpE5B)))),0),((_tmpE59->tl=Cyc_Toc_result_decls,_tmpE59))))));}*
_tmp969=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp969,f->name,1);}goto _LL3A9;_LL3AE:;_LL3AF: goto _LL3A9;_LL3A9:;}};
_pop_dynregion(d);};};}static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*
ed){ed->sc=Cyc_Absyn_Public;if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(
struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,
struct Cyc_Absyn_Stmt*s){void*old_typ=vd->type;vd->type=Cyc_Toc_typ_to_c_array(
old_typ);if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ))vd->sc=Cyc_Absyn_Public;Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp98C=init->r;struct Cyc_Absyn_Vardecl*_tmp98E;struct Cyc_Absyn_Exp*_tmp98F;
struct Cyc_Absyn_Exp*_tmp990;int _tmp991;_LL3B1: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp98D=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp98C;if(_tmp98D->tag != 28)
goto _LL3B3;else{_tmp98E=_tmp98D->f1;_tmp98F=_tmp98D->f2;_tmp990=_tmp98D->f3;
_tmp991=_tmp98D->f4;}}_LL3B2: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp98E,_tmp98F,_tmp990,_tmp991,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL3B0;_LL3B3:;_LL3B4: if(vd->sc == Cyc_Absyn_Static){struct Cyc_Toc_Env
_tmp993;struct _RegionHandle*_tmp994;struct Cyc_Toc_Env*_tmp992=init_nv;_tmp993=*
_tmp992;_tmp994=_tmp993.rgn;{struct Cyc_Toc_Env*_tmp995=Cyc_Toc_set_toplevel(
_tmp994,init_nv);Cyc_Toc_exp_to_c(_tmp995,init);};}else{Cyc_Toc_exp_to_c(init_nv,
init);}goto _LL3B0;_LL3B0:;}else{void*_tmp996=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_ArrayInfo _tmp998;void*_tmp999;struct Cyc_Absyn_Exp*_tmp99A;union Cyc_Absyn_Constraint*
_tmp99B;_LL3B6:{struct Cyc_Absyn_ArrayType_struct*_tmp997=(struct Cyc_Absyn_ArrayType_struct*)
_tmp996;if(_tmp997->tag != 9)goto _LL3B8;else{_tmp998=_tmp997->f1;_tmp999=_tmp998.elt_type;
_tmp99A=_tmp998.num_elts;_tmp99B=_tmp998.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp99B))goto _LL3B8;_LL3B7: if(_tmp99A == 0){const char*
_tmpE5E;void*_tmpE5D;(_tmpE5D=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE5E="can't initialize zero-terminated array -- size unknown",
_tag_dyneither(_tmpE5E,sizeof(char),55))),_tag_dyneither(_tmpE5D,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp99A;struct Cyc_Absyn_Exp*
_tmp99E=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp99F=Cyc_Absyn_signed_int_exp(
0,0);s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp99E,
_tmp99F,0),0),Cyc_Absyn_new_stmt(s->r,0));goto _LL3B5;};_LL3B8:;_LL3B9: goto _LL3B5;
_LL3B5:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple0*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp9A1;struct
_RegionHandle*_tmp9A2;struct Cyc_Toc_Env*_tmp9A0=nv;_tmp9A1=*_tmp9A0;_tmp9A2=
_tmp9A1.rgn;{struct Cyc_Absyn_Stmt*_tmp9A3=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp9A4=Cyc_Toc_share_env(_tmp9A2,nv);struct Cyc_Toc_Env*_tmp9A6;struct Cyc_List_List*
_tmp9A7;struct Cyc_Absyn_Stmt*_tmp9A8;struct _tuple22 _tmp9A5=Cyc_Toc_xlate_pat(
_tmp9A4,_tmp9A2,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp9A6=_tmp9A5.f1;_tmp9A7=_tmp9A5.f2;_tmp9A8=_tmp9A5.f3;Cyc_Toc_stmt_to_c(
_tmp9A6,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp9A8,s,0),0);for(0;_tmp9A7 != 0;_tmp9A7=_tmp9A7->tl){
struct _tuple23 _tmp9AA;struct _tuple0*_tmp9AB;void*_tmp9AC;struct _tuple23*_tmp9A9=(
struct _tuple23*)_tmp9A7->hd;_tmp9AA=*_tmp9A9;_tmp9AB=_tmp9AA.f1;_tmp9AC=_tmp9AA.f2;
s=Cyc_Absyn_declare_stmt(_tmp9AB,_tmp9AC,0,s,0);}}return s;};}static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e){void*_tmp9AD=e->r;struct Cyc_Absyn_Exp*_tmp9AF;struct Cyc_Absyn_Exp*
_tmp9B1;struct Cyc_Absyn_Exp*_tmp9B3;struct Cyc_Absyn_Exp*_tmp9B5;struct Cyc_Absyn_Exp*
_tmp9B7;struct Cyc_Absyn_Exp*_tmp9B9;struct Cyc_Absyn_Exp*_tmp9BB;struct Cyc_Absyn_Exp*
_tmp9BD;struct Cyc_List_List*_tmp9BF;struct Cyc_Absyn_Exp*_tmp9C1;struct Cyc_Absyn_Exp*
_tmp9C2;struct Cyc_Absyn_Exp*_tmp9C4;struct Cyc_Absyn_Exp*_tmp9C5;struct Cyc_Absyn_Exp*
_tmp9C7;struct Cyc_Absyn_Exp*_tmp9C8;struct Cyc_Absyn_Exp*_tmp9CA;struct Cyc_Absyn_Exp*
_tmp9CB;struct Cyc_Absyn_Exp*_tmp9CD;struct Cyc_Absyn_Exp*_tmp9CE;struct Cyc_Absyn_Exp*
_tmp9D0;struct Cyc_Absyn_Exp*_tmp9D1;struct Cyc_Absyn_Exp*_tmp9D3;struct Cyc_Absyn_Exp*
_tmp9D4;struct Cyc_Absyn_Exp*_tmp9D5;struct Cyc_Absyn_Exp*_tmp9D7;struct Cyc_List_List*
_tmp9D8;struct Cyc_Absyn_Exp*_tmp9DA;struct Cyc_List_List*_tmp9DB;void*_tmp9DD;
void**_tmp9DE;struct Cyc_Absyn_Exp*_tmp9DF;struct _tuple7*_tmp9E1;struct _tuple7
_tmp9E2;void*_tmp9E3;void**_tmp9E4;struct Cyc_List_List*_tmp9E5;struct Cyc_List_List*
_tmp9E7;struct Cyc_List_List*_tmp9E9;void*_tmp9EB;void**_tmp9EC;void*_tmp9EE;void**
_tmp9EF;struct Cyc_Absyn_Stmt*_tmp9F1;struct Cyc_Absyn_MallocInfo _tmp9F3;struct Cyc_Absyn_MallocInfo*
_tmp9F4;_LL3BB: {struct Cyc_Absyn_Deref_e_struct*_tmp9AE=(struct Cyc_Absyn_Deref_e_struct*)
_tmp9AD;if(_tmp9AE->tag != 21)goto _LL3BD;else{_tmp9AF=_tmp9AE->f1;}}_LL3BC:
_tmp9B1=_tmp9AF;goto _LL3BE;_LL3BD: {struct Cyc_Absyn_AggrMember_e_struct*_tmp9B0=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp9AD;if(_tmp9B0->tag != 22)goto _LL3BF;
else{_tmp9B1=_tmp9B0->f1;}}_LL3BE: _tmp9B3=_tmp9B1;goto _LL3C0;_LL3BF: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp9B2=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp9AD;if(_tmp9B2->tag != 23)goto
_LL3C1;else{_tmp9B3=_tmp9B2->f1;}}_LL3C0: _tmp9B5=_tmp9B3;goto _LL3C2;_LL3C1: {
struct Cyc_Absyn_Address_e_struct*_tmp9B4=(struct Cyc_Absyn_Address_e_struct*)
_tmp9AD;if(_tmp9B4->tag != 16)goto _LL3C3;else{_tmp9B5=_tmp9B4->f1;}}_LL3C2:
_tmp9B7=_tmp9B5;goto _LL3C4;_LL3C3: {struct Cyc_Absyn_Throw_e_struct*_tmp9B6=(
struct Cyc_Absyn_Throw_e_struct*)_tmp9AD;if(_tmp9B6->tag != 12)goto _LL3C5;else{
_tmp9B7=_tmp9B6->f1;}}_LL3C4: _tmp9B9=_tmp9B7;goto _LL3C6;_LL3C5: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp9B8=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp9AD;if(_tmp9B8->tag != 13)
goto _LL3C7;else{_tmp9B9=_tmp9B8->f1;}}_LL3C6: _tmp9BB=_tmp9B9;goto _LL3C8;_LL3C7: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp9BA=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp9AD;if(_tmp9BA->tag != 19)goto _LL3C9;else{_tmp9BB=_tmp9BA->f1;}}_LL3C8:
_tmp9BD=_tmp9BB;goto _LL3CA;_LL3C9: {struct Cyc_Absyn_Increment_e_struct*_tmp9BC=(
struct Cyc_Absyn_Increment_e_struct*)_tmp9AD;if(_tmp9BC->tag != 5)goto _LL3CB;else{
_tmp9BD=_tmp9BC->f1;}}_LL3CA: Cyc_Toc_exptypes_to_c(_tmp9BD);goto _LL3BA;_LL3CB: {
struct Cyc_Absyn_Primop_e_struct*_tmp9BE=(struct Cyc_Absyn_Primop_e_struct*)
_tmp9AD;if(_tmp9BE->tag != 3)goto _LL3CD;else{_tmp9BF=_tmp9BE->f2;}}_LL3CC:((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,
_tmp9BF);goto _LL3BA;_LL3CD: {struct Cyc_Absyn_And_e_struct*_tmp9C0=(struct Cyc_Absyn_And_e_struct*)
_tmp9AD;if(_tmp9C0->tag != 7)goto _LL3CF;else{_tmp9C1=_tmp9C0->f1;_tmp9C2=_tmp9C0->f2;}}
_LL3CE: _tmp9C4=_tmp9C1;_tmp9C5=_tmp9C2;goto _LL3D0;_LL3CF: {struct Cyc_Absyn_Or_e_struct*
_tmp9C3=(struct Cyc_Absyn_Or_e_struct*)_tmp9AD;if(_tmp9C3->tag != 8)goto _LL3D1;
else{_tmp9C4=_tmp9C3->f1;_tmp9C5=_tmp9C3->f2;}}_LL3D0: _tmp9C7=_tmp9C4;_tmp9C8=
_tmp9C5;goto _LL3D2;_LL3D1: {struct Cyc_Absyn_SeqExp_e_struct*_tmp9C6=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp9AD;if(_tmp9C6->tag != 9)goto _LL3D3;else{_tmp9C7=_tmp9C6->f1;_tmp9C8=_tmp9C6->f2;}}
_LL3D2: _tmp9CA=_tmp9C7;_tmp9CB=_tmp9C8;goto _LL3D4;_LL3D3: {struct Cyc_Absyn_Subscript_e_struct*
_tmp9C9=(struct Cyc_Absyn_Subscript_e_struct*)_tmp9AD;if(_tmp9C9->tag != 24)goto
_LL3D5;else{_tmp9CA=_tmp9C9->f1;_tmp9CB=_tmp9C9->f2;}}_LL3D4: _tmp9CD=_tmp9CA;
_tmp9CE=_tmp9CB;goto _LL3D6;_LL3D5: {struct Cyc_Absyn_Swap_e_struct*_tmp9CC=(
struct Cyc_Absyn_Swap_e_struct*)_tmp9AD;if(_tmp9CC->tag != 35)goto _LL3D7;else{
_tmp9CD=_tmp9CC->f1;_tmp9CE=_tmp9CC->f2;}}_LL3D6: _tmp9D0=_tmp9CD;_tmp9D1=_tmp9CE;
goto _LL3D8;_LL3D7: {struct Cyc_Absyn_AssignOp_e_struct*_tmp9CF=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp9AD;if(_tmp9CF->tag != 4)goto _LL3D9;else{_tmp9D0=_tmp9CF->f1;_tmp9D1=_tmp9CF->f3;}}
_LL3D8: Cyc_Toc_exptypes_to_c(_tmp9D0);Cyc_Toc_exptypes_to_c(_tmp9D1);goto _LL3BA;
_LL3D9: {struct Cyc_Absyn_Conditional_e_struct*_tmp9D2=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp9AD;if(_tmp9D2->tag != 6)goto _LL3DB;else{_tmp9D3=_tmp9D2->f1;_tmp9D4=_tmp9D2->f2;
_tmp9D5=_tmp9D2->f3;}}_LL3DA: Cyc_Toc_exptypes_to_c(_tmp9D3);Cyc_Toc_exptypes_to_c(
_tmp9D4);Cyc_Toc_exptypes_to_c(_tmp9D5);goto _LL3BA;_LL3DB: {struct Cyc_Absyn_FnCall_e_struct*
_tmp9D6=(struct Cyc_Absyn_FnCall_e_struct*)_tmp9AD;if(_tmp9D6->tag != 11)goto
_LL3DD;else{_tmp9D7=_tmp9D6->f1;_tmp9D8=_tmp9D6->f2;}}_LL3DC: _tmp9DA=_tmp9D7;
_tmp9DB=_tmp9D8;goto _LL3DE;_LL3DD: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp9D9=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp9AD;if(_tmp9D9->tag != 10)goto _LL3DF;
else{_tmp9DA=_tmp9D9->f1;_tmp9DB=_tmp9D9->f2;}}_LL3DE: Cyc_Toc_exptypes_to_c(
_tmp9DA);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp9DB);goto _LL3BA;_LL3DF: {struct Cyc_Absyn_Cast_e_struct*
_tmp9DC=(struct Cyc_Absyn_Cast_e_struct*)_tmp9AD;if(_tmp9DC->tag != 15)goto _LL3E1;
else{_tmp9DD=(void**)& _tmp9DC->f1;_tmp9DE=(void**)((void**)& _tmp9DC->f1);_tmp9DF=
_tmp9DC->f2;}}_LL3E0:*_tmp9DE=Cyc_Toc_typ_to_c(*_tmp9DE);Cyc_Toc_exptypes_to_c(
_tmp9DF);goto _LL3BA;_LL3E1: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp9E0=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp9AD;if(_tmp9E0->tag != 26)goto _LL3E3;
else{_tmp9E1=_tmp9E0->f1;_tmp9E2=*_tmp9E1;_tmp9E3=_tmp9E2.f3;_tmp9E4=(void**)&(*
_tmp9E0->f1).f3;_tmp9E5=_tmp9E0->f2;}}_LL3E2:*_tmp9E4=Cyc_Toc_typ_to_c(*_tmp9E4);
_tmp9E7=_tmp9E5;goto _LL3E4;_LL3E3: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp9E6=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp9AD;if(_tmp9E6->tag != 36)
goto _LL3E5;else{_tmp9E7=_tmp9E6->f2;}}_LL3E4: _tmp9E9=_tmp9E7;goto _LL3E6;_LL3E5: {
struct Cyc_Absyn_Array_e_struct*_tmp9E8=(struct Cyc_Absyn_Array_e_struct*)_tmp9AD;
if(_tmp9E8->tag != 27)goto _LL3E7;else{_tmp9E9=_tmp9E8->f1;}}_LL3E6: for(0;_tmp9E9
!= 0;_tmp9E9=_tmp9E9->tl){struct Cyc_Absyn_Exp*_tmpA05;struct _tuple15 _tmpA04=*((
struct _tuple15*)_tmp9E9->hd);_tmpA05=_tmpA04.f2;Cyc_Toc_exptypes_to_c(_tmpA05);}
goto _LL3BA;_LL3E7: {struct Cyc_Absyn_Offsetof_e_struct*_tmp9EA=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp9AD;if(_tmp9EA->tag != 20)goto _LL3E9;else{_tmp9EB=(void**)& _tmp9EA->f1;
_tmp9EC=(void**)((void**)& _tmp9EA->f1);}}_LL3E8: _tmp9EF=_tmp9EC;goto _LL3EA;
_LL3E9: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp9ED=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp9AD;if(_tmp9ED->tag != 18)goto _LL3EB;else{_tmp9EE=(void**)& _tmp9ED->f1;
_tmp9EF=(void**)((void**)& _tmp9ED->f1);}}_LL3EA:*_tmp9EF=Cyc_Toc_typ_to_c(*
_tmp9EF);goto _LL3BA;_LL3EB: {struct Cyc_Absyn_StmtExp_e_struct*_tmp9F0=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp9AD;if(_tmp9F0->tag != 37)goto _LL3ED;else{_tmp9F1=_tmp9F0->f1;}}_LL3EC: Cyc_Toc_stmttypes_to_c(
_tmp9F1);goto _LL3BA;_LL3ED: {struct Cyc_Absyn_Malloc_e_struct*_tmp9F2=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp9AD;if(_tmp9F2->tag != 34)goto _LL3EF;else{_tmp9F3=_tmp9F2->f1;_tmp9F4=(struct
Cyc_Absyn_MallocInfo*)& _tmp9F2->f1;}}_LL3EE: if(_tmp9F4->elt_type != 0){void**
_tmpE5F;_tmp9F4->elt_type=((_tmpE5F=_cycalloc(sizeof(*_tmpE5F)),((_tmpE5F[0]=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp9F4->elt_type))),_tmpE5F))));}Cyc_Toc_exptypes_to_c(
_tmp9F4->num_elts);goto _LL3BA;_LL3EF: {struct Cyc_Absyn_Const_e_struct*_tmp9F5=(
struct Cyc_Absyn_Const_e_struct*)_tmp9AD;if(_tmp9F5->tag != 0)goto _LL3F1;}_LL3F0:
goto _LL3F2;_LL3F1: {struct Cyc_Absyn_Var_e_struct*_tmp9F6=(struct Cyc_Absyn_Var_e_struct*)
_tmp9AD;if(_tmp9F6->tag != 1)goto _LL3F3;}_LL3F2: goto _LL3F4;_LL3F3: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp9F7=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp9AD;if(_tmp9F7->tag != 2)goto
_LL3F5;}_LL3F4: goto _LL3F6;_LL3F5: {struct Cyc_Absyn_Enum_e_struct*_tmp9F8=(struct
Cyc_Absyn_Enum_e_struct*)_tmp9AD;if(_tmp9F8->tag != 32)goto _LL3F7;}_LL3F6: goto
_LL3F8;_LL3F7: {struct Cyc_Absyn_Asm_e_struct*_tmp9F9=(struct Cyc_Absyn_Asm_e_struct*)
_tmp9AD;if(_tmp9F9->tag != 40)goto _LL3F9;}_LL3F8: goto _LL3FA;_LL3F9: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp9FA=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp9AD;if(_tmp9FA->tag != 33)goto
_LL3FB;}_LL3FA: goto _LL3BA;_LL3FB: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp9FB=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp9AD;if(_tmp9FB->tag != 30)goto _LL3FD;}
_LL3FC: goto _LL3FE;_LL3FD: {struct Cyc_Absyn_Datatype_e_struct*_tmp9FC=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp9AD;if(_tmp9FC->tag != 31)goto _LL3FF;}_LL3FE: goto _LL400;_LL3FF: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp9FD=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp9AD;if(_tmp9FD->tag != 29)goto
_LL401;}_LL400: goto _LL402;_LL401: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp9FE=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp9AD;if(_tmp9FE->tag != 28)
goto _LL403;}_LL402: goto _LL404;_LL403: {struct Cyc_Absyn_Tuple_e_struct*_tmp9FF=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp9AD;if(_tmp9FF->tag != 25)goto _LL405;}_LL404:
goto _LL406;_LL405: {struct Cyc_Absyn_Instantiate_e_struct*_tmpA00=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp9AD;if(_tmpA00->tag != 14)goto _LL407;}_LL406: goto _LL408;_LL407: {struct Cyc_Absyn_New_e_struct*
_tmpA01=(struct Cyc_Absyn_New_e_struct*)_tmp9AD;if(_tmpA01->tag != 17)goto _LL409;}
_LL408: goto _LL40A;_LL409: {struct Cyc_Absyn_Valueof_e_struct*_tmpA02=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp9AD;if(_tmpA02->tag != 39)goto _LL40B;}_LL40A: goto _LL40C;_LL40B: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmpA03=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp9AD;if(_tmpA03->tag != 38)goto
_LL3BA;}_LL40C:{const char*_tmpE62;void*_tmpE61;(_tmpE61=0,Cyc_Tcutil_terr(e->loc,((
_tmpE62="Cyclone expression within C code",_tag_dyneither(_tmpE62,sizeof(char),
33))),_tag_dyneither(_tmpE61,sizeof(void*),0)));}goto _LL3BA;_LL3BA:;}static void
Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){void*_tmpA09=d->r;struct Cyc_Absyn_Vardecl*
_tmpA0B;struct Cyc_Absyn_Fndecl*_tmpA0D;struct Cyc_Absyn_Aggrdecl*_tmpA0F;struct
Cyc_Absyn_Enumdecl*_tmpA11;struct Cyc_Absyn_Typedefdecl*_tmpA13;_LL40E: {struct
Cyc_Absyn_Var_d_struct*_tmpA0A=(struct Cyc_Absyn_Var_d_struct*)_tmpA09;if(_tmpA0A->tag
!= 0)goto _LL410;else{_tmpA0B=_tmpA0A->f1;}}_LL40F: _tmpA0B->type=Cyc_Toc_typ_to_c(
_tmpA0B->type);if(_tmpA0B->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmpA0B->initializer));goto _LL40D;_LL410: {struct Cyc_Absyn_Fn_d_struct*
_tmpA0C=(struct Cyc_Absyn_Fn_d_struct*)_tmpA09;if(_tmpA0C->tag != 1)goto _LL412;
else{_tmpA0D=_tmpA0C->f1;}}_LL411: _tmpA0D->ret_type=Cyc_Toc_typ_to_c(_tmpA0D->ret_type);{
struct Cyc_List_List*_tmpA1F=_tmpA0D->args;for(0;_tmpA1F != 0;_tmpA1F=_tmpA1F->tl){(*((
struct _tuple26*)_tmpA1F->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple26*)_tmpA1F->hd)).f3);}}
goto _LL40D;_LL412: {struct Cyc_Absyn_Aggr_d_struct*_tmpA0E=(struct Cyc_Absyn_Aggr_d_struct*)
_tmpA09;if(_tmpA0E->tag != 6)goto _LL414;else{_tmpA0F=_tmpA0E->f1;}}_LL413: Cyc_Toc_aggrdecl_to_c(
_tmpA0F,1);goto _LL40D;_LL414: {struct Cyc_Absyn_Enum_d_struct*_tmpA10=(struct Cyc_Absyn_Enum_d_struct*)
_tmpA09;if(_tmpA10->tag != 8)goto _LL416;else{_tmpA11=_tmpA10->f1;}}_LL415: if(
_tmpA11->fields != 0){struct Cyc_List_List*_tmpA20=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmpA11->fields))->v;for(0;_tmpA20 != 0;_tmpA20=_tmpA20->tl){
struct Cyc_Absyn_Enumfield*_tmpA21=(struct Cyc_Absyn_Enumfield*)_tmpA20->hd;if(
_tmpA21->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmpA21->tag));}}
goto _LL40D;_LL416: {struct Cyc_Absyn_Typedef_d_struct*_tmpA12=(struct Cyc_Absyn_Typedef_d_struct*)
_tmpA09;if(_tmpA12->tag != 9)goto _LL418;else{_tmpA13=_tmpA12->f1;}}_LL417:{struct
Cyc_Core_Opt*_tmpE63;_tmpA13->defn=((_tmpE63=_cycalloc(sizeof(*_tmpE63)),((
_tmpE63->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA13->defn))->v),_tmpE63))));}goto _LL40D;_LL418: {struct Cyc_Absyn_Let_d_struct*
_tmpA14=(struct Cyc_Absyn_Let_d_struct*)_tmpA09;if(_tmpA14->tag != 2)goto _LL41A;}
_LL419: goto _LL41B;_LL41A: {struct Cyc_Absyn_Letv_d_struct*_tmpA15=(struct Cyc_Absyn_Letv_d_struct*)
_tmpA09;if(_tmpA15->tag != 3)goto _LL41C;}_LL41B: goto _LL41D;_LL41C: {struct Cyc_Absyn_Datatype_d_struct*
_tmpA16=(struct Cyc_Absyn_Datatype_d_struct*)_tmpA09;if(_tmpA16->tag != 7)goto
_LL41E;}_LL41D: goto _LL41F;_LL41E: {struct Cyc_Absyn_Namespace_d_struct*_tmpA17=(
struct Cyc_Absyn_Namespace_d_struct*)_tmpA09;if(_tmpA17->tag != 10)goto _LL420;}
_LL41F: goto _LL421;_LL420: {struct Cyc_Absyn_Using_d_struct*_tmpA18=(struct Cyc_Absyn_Using_d_struct*)
_tmpA09;if(_tmpA18->tag != 11)goto _LL422;}_LL421: goto _LL423;_LL422: {struct Cyc_Absyn_ExternC_d_struct*
_tmpA19=(struct Cyc_Absyn_ExternC_d_struct*)_tmpA09;if(_tmpA19->tag != 12)goto
_LL424;}_LL423: goto _LL425;_LL424: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmpA1A=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmpA09;if(_tmpA1A->tag != 13)
goto _LL426;}_LL425: goto _LL427;_LL426: {struct Cyc_Absyn_Region_d_struct*_tmpA1B=(
struct Cyc_Absyn_Region_d_struct*)_tmpA09;if(_tmpA1B->tag != 4)goto _LL428;}_LL427:
goto _LL429;_LL428: {struct Cyc_Absyn_Alias_d_struct*_tmpA1C=(struct Cyc_Absyn_Alias_d_struct*)
_tmpA09;if(_tmpA1C->tag != 5)goto _LL42A;}_LL429:{const char*_tmpE66;void*_tmpE65;(
_tmpE65=0,Cyc_Tcutil_terr(d->loc,((_tmpE66="Cyclone declaration within C code",
_tag_dyneither(_tmpE66,sizeof(char),34))),_tag_dyneither(_tmpE65,sizeof(void*),0)));}
goto _LL40D;_LL42A: {struct Cyc_Absyn_Porton_d_struct*_tmpA1D=(struct Cyc_Absyn_Porton_d_struct*)
_tmpA09;if(_tmpA1D->tag != 14)goto _LL42C;}_LL42B: goto _LL42D;_LL42C: {struct Cyc_Absyn_Portoff_d_struct*
_tmpA1E=(struct Cyc_Absyn_Portoff_d_struct*)_tmpA09;if(_tmpA1E->tag != 15)goto
_LL40D;}_LL42D: goto _LL40D;_LL40D:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s){void*_tmpA25=s->r;struct Cyc_Absyn_Exp*_tmpA27;struct Cyc_Absyn_Stmt*_tmpA29;
struct Cyc_Absyn_Stmt*_tmpA2A;struct Cyc_Absyn_Exp*_tmpA2C;struct Cyc_Absyn_Exp*
_tmpA2E;struct Cyc_Absyn_Stmt*_tmpA2F;struct Cyc_Absyn_Stmt*_tmpA30;struct _tuple8
_tmpA32;struct Cyc_Absyn_Exp*_tmpA33;struct Cyc_Absyn_Stmt*_tmpA34;struct Cyc_Absyn_Exp*
_tmpA36;struct _tuple8 _tmpA37;struct Cyc_Absyn_Exp*_tmpA38;struct _tuple8 _tmpA39;
struct Cyc_Absyn_Exp*_tmpA3A;struct Cyc_Absyn_Stmt*_tmpA3B;struct Cyc_Absyn_Exp*
_tmpA3D;struct Cyc_List_List*_tmpA3E;struct Cyc_Absyn_Decl*_tmpA40;struct Cyc_Absyn_Stmt*
_tmpA41;struct Cyc_Absyn_Stmt*_tmpA43;struct _tuple8 _tmpA44;struct Cyc_Absyn_Exp*
_tmpA45;_LL42F: {struct Cyc_Absyn_Exp_s_struct*_tmpA26=(struct Cyc_Absyn_Exp_s_struct*)
_tmpA25;if(_tmpA26->tag != 1)goto _LL431;else{_tmpA27=_tmpA26->f1;}}_LL430: Cyc_Toc_exptypes_to_c(
_tmpA27);goto _LL42E;_LL431: {struct Cyc_Absyn_Seq_s_struct*_tmpA28=(struct Cyc_Absyn_Seq_s_struct*)
_tmpA25;if(_tmpA28->tag != 2)goto _LL433;else{_tmpA29=_tmpA28->f1;_tmpA2A=_tmpA28->f2;}}
_LL432: Cyc_Toc_stmttypes_to_c(_tmpA29);Cyc_Toc_stmttypes_to_c(_tmpA2A);goto
_LL42E;_LL433: {struct Cyc_Absyn_Return_s_struct*_tmpA2B=(struct Cyc_Absyn_Return_s_struct*)
_tmpA25;if(_tmpA2B->tag != 3)goto _LL435;else{_tmpA2C=_tmpA2B->f1;}}_LL434: if(
_tmpA2C != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmpA2C);goto _LL42E;
_LL435: {struct Cyc_Absyn_IfThenElse_s_struct*_tmpA2D=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmpA25;if(_tmpA2D->tag != 4)goto _LL437;else{_tmpA2E=_tmpA2D->f1;_tmpA2F=_tmpA2D->f2;
_tmpA30=_tmpA2D->f3;}}_LL436: Cyc_Toc_exptypes_to_c(_tmpA2E);Cyc_Toc_stmttypes_to_c(
_tmpA2F);Cyc_Toc_stmttypes_to_c(_tmpA30);goto _LL42E;_LL437: {struct Cyc_Absyn_While_s_struct*
_tmpA31=(struct Cyc_Absyn_While_s_struct*)_tmpA25;if(_tmpA31->tag != 5)goto _LL439;
else{_tmpA32=_tmpA31->f1;_tmpA33=_tmpA32.f1;_tmpA34=_tmpA31->f2;}}_LL438: Cyc_Toc_exptypes_to_c(
_tmpA33);Cyc_Toc_stmttypes_to_c(_tmpA34);goto _LL42E;_LL439: {struct Cyc_Absyn_For_s_struct*
_tmpA35=(struct Cyc_Absyn_For_s_struct*)_tmpA25;if(_tmpA35->tag != 9)goto _LL43B;
else{_tmpA36=_tmpA35->f1;_tmpA37=_tmpA35->f2;_tmpA38=_tmpA37.f1;_tmpA39=_tmpA35->f3;
_tmpA3A=_tmpA39.f1;_tmpA3B=_tmpA35->f4;}}_LL43A: Cyc_Toc_exptypes_to_c(_tmpA36);
Cyc_Toc_exptypes_to_c(_tmpA38);Cyc_Toc_exptypes_to_c(_tmpA3A);Cyc_Toc_stmttypes_to_c(
_tmpA3B);goto _LL42E;_LL43B: {struct Cyc_Absyn_Switch_s_struct*_tmpA3C=(struct Cyc_Absyn_Switch_s_struct*)
_tmpA25;if(_tmpA3C->tag != 10)goto _LL43D;else{_tmpA3D=_tmpA3C->f1;_tmpA3E=_tmpA3C->f2;}}
_LL43C: Cyc_Toc_exptypes_to_c(_tmpA3D);for(0;_tmpA3E != 0;_tmpA3E=_tmpA3E->tl){Cyc_Toc_stmttypes_to_c(((
struct Cyc_Absyn_Switch_clause*)_tmpA3E->hd)->body);}goto _LL42E;_LL43D: {struct
Cyc_Absyn_Decl_s_struct*_tmpA3F=(struct Cyc_Absyn_Decl_s_struct*)_tmpA25;if(
_tmpA3F->tag != 12)goto _LL43F;else{_tmpA40=_tmpA3F->f1;_tmpA41=_tmpA3F->f2;}}
_LL43E: Cyc_Toc_decltypes_to_c(_tmpA40);Cyc_Toc_stmttypes_to_c(_tmpA41);goto
_LL42E;_LL43F: {struct Cyc_Absyn_Do_s_struct*_tmpA42=(struct Cyc_Absyn_Do_s_struct*)
_tmpA25;if(_tmpA42->tag != 14)goto _LL441;else{_tmpA43=_tmpA42->f1;_tmpA44=_tmpA42->f2;
_tmpA45=_tmpA44.f1;}}_LL440: Cyc_Toc_stmttypes_to_c(_tmpA43);Cyc_Toc_exptypes_to_c(
_tmpA45);goto _LL42E;_LL441: {struct Cyc_Absyn_Skip_s_struct*_tmpA46=(struct Cyc_Absyn_Skip_s_struct*)
_tmpA25;if(_tmpA46->tag != 0)goto _LL443;}_LL442: goto _LL444;_LL443: {struct Cyc_Absyn_Break_s_struct*
_tmpA47=(struct Cyc_Absyn_Break_s_struct*)_tmpA25;if(_tmpA47->tag != 6)goto _LL445;}
_LL444: goto _LL446;_LL445: {struct Cyc_Absyn_Continue_s_struct*_tmpA48=(struct Cyc_Absyn_Continue_s_struct*)
_tmpA25;if(_tmpA48->tag != 7)goto _LL447;}_LL446: goto _LL448;_LL447: {struct Cyc_Absyn_Goto_s_struct*
_tmpA49=(struct Cyc_Absyn_Goto_s_struct*)_tmpA25;if(_tmpA49->tag != 8)goto _LL449;}
_LL448: goto _LL42E;_LL449: {struct Cyc_Absyn_Fallthru_s_struct*_tmpA4A=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmpA25;if(_tmpA4A->tag != 11)goto _LL44B;}_LL44A: goto _LL44C;_LL44B: {struct Cyc_Absyn_Label_s_struct*
_tmpA4B=(struct Cyc_Absyn_Label_s_struct*)_tmpA25;if(_tmpA4B->tag != 13)goto _LL44D;}
_LL44C: goto _LL44E;_LL44D: {struct Cyc_Absyn_TryCatch_s_struct*_tmpA4C=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmpA25;if(_tmpA4C->tag != 15)goto _LL44F;}_LL44E: goto _LL450;_LL44F: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmpA4D=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmpA25;if(_tmpA4D->tag != 16)goto
_LL42E;}_LL450:{const char*_tmpE69;void*_tmpE68;(_tmpE68=0,Cyc_Tcutil_terr(s->loc,((
_tmpE69="Cyclone statement in C code",_tag_dyneither(_tmpE69,sizeof(char),28))),
_tag_dyneither(_tmpE68,sizeof(void*),0)));}goto _LL42E;_LL42E:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude){for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpE6C;void*_tmpE6B;(_tmpE6B=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE6C="decls_to_c: not at toplevel!",
_tag_dyneither(_tmpE6C,sizeof(char),29))),_tag_dyneither(_tmpE6B,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmpA52=d->r;struct Cyc_Absyn_Vardecl*
_tmpA54;struct Cyc_Absyn_Fndecl*_tmpA56;struct Cyc_Absyn_Aggrdecl*_tmpA5C;struct
Cyc_Absyn_Datatypedecl*_tmpA5E;struct Cyc_Absyn_Enumdecl*_tmpA60;struct Cyc_Absyn_Typedefdecl*
_tmpA62;struct Cyc_List_List*_tmpA66;struct Cyc_List_List*_tmpA68;struct Cyc_List_List*
_tmpA6A;struct Cyc_List_List*_tmpA6C;_LL452: {struct Cyc_Absyn_Var_d_struct*
_tmpA53=(struct Cyc_Absyn_Var_d_struct*)_tmpA52;if(_tmpA53->tag != 0)goto _LL454;
else{_tmpA54=_tmpA53->f1;}}_LL453: {struct _tuple0*_tmpA6D=_tmpA54->name;if(
_tmpA54->sc == Cyc_Absyn_ExternC){struct _tuple0*_tmpE6D;_tmpA6D=((_tmpE6D=
_cycalloc(sizeof(*_tmpE6D)),((_tmpE6D->f1=Cyc_Absyn_Rel_n(0),((_tmpE6D->f2=(*
_tmpA6D).f2,_tmpE6D))))));}if(_tmpA54->initializer != 0){if(cinclude)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmpA54->initializer));else{Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_check_null(_tmpA54->initializer));}}{struct Cyc_Absyn_Global_b_struct
_tmpE70;struct Cyc_Absyn_Global_b_struct*_tmpE6F;nv=Cyc_Toc_add_varmap(r,nv,
_tmpA54->name,Cyc_Absyn_varb_exp(_tmpA6D,(void*)((_tmpE6F=_cycalloc(sizeof(*
_tmpE6F)),((_tmpE6F[0]=((_tmpE70.tag=1,((_tmpE70.f1=_tmpA54,_tmpE70)))),_tmpE6F)))),
0));}_tmpA54->name=_tmpA6D;_tmpA54->sc=Cyc_Toc_scope_to_c(_tmpA54->sc);_tmpA54->type=
Cyc_Toc_typ_to_c_array(_tmpA54->type);{struct Cyc_List_List*_tmpE71;Cyc_Toc_result_decls=((
_tmpE71=_cycalloc(sizeof(*_tmpE71)),((_tmpE71->hd=d,((_tmpE71->tl=Cyc_Toc_result_decls,
_tmpE71))))));}goto _LL451;}_LL454: {struct Cyc_Absyn_Fn_d_struct*_tmpA55=(struct
Cyc_Absyn_Fn_d_struct*)_tmpA52;if(_tmpA55->tag != 1)goto _LL456;else{_tmpA56=
_tmpA55->f1;}}_LL455: {struct _tuple0*_tmpA72=_tmpA56->name;if(_tmpA56->sc == Cyc_Absyn_ExternC){{
struct _tuple0*_tmpE72;_tmpA72=((_tmpE72=_cycalloc(sizeof(*_tmpE72)),((_tmpE72->f1=
Cyc_Absyn_Rel_n(0),((_tmpE72->f2=(*_tmpA72).f2,_tmpE72))))));}_tmpA56->sc=Cyc_Absyn_Public;}
nv=Cyc_Toc_add_varmap(r,nv,_tmpA56->name,Cyc_Absyn_var_exp(_tmpA72,0));_tmpA56->name=
_tmpA72;Cyc_Toc_fndecl_to_c(nv,_tmpA56,cinclude);{struct Cyc_List_List*_tmpE73;
Cyc_Toc_result_decls=((_tmpE73=_cycalloc(sizeof(*_tmpE73)),((_tmpE73->hd=d,((
_tmpE73->tl=Cyc_Toc_result_decls,_tmpE73))))));}goto _LL451;}_LL456: {struct Cyc_Absyn_Let_d_struct*
_tmpA57=(struct Cyc_Absyn_Let_d_struct*)_tmpA52;if(_tmpA57->tag != 2)goto _LL458;}
_LL457: goto _LL459;_LL458: {struct Cyc_Absyn_Letv_d_struct*_tmpA58=(struct Cyc_Absyn_Letv_d_struct*)
_tmpA52;if(_tmpA58->tag != 3)goto _LL45A;}_LL459: {const char*_tmpE76;void*_tmpE75;(
_tmpE75=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE76="letdecl at toplevel",_tag_dyneither(_tmpE76,sizeof(char),20))),
_tag_dyneither(_tmpE75,sizeof(void*),0)));}_LL45A: {struct Cyc_Absyn_Region_d_struct*
_tmpA59=(struct Cyc_Absyn_Region_d_struct*)_tmpA52;if(_tmpA59->tag != 4)goto _LL45C;}
_LL45B: {const char*_tmpE79;void*_tmpE78;(_tmpE78=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE79="region decl at toplevel",
_tag_dyneither(_tmpE79,sizeof(char),24))),_tag_dyneither(_tmpE78,sizeof(void*),0)));}
_LL45C: {struct Cyc_Absyn_Alias_d_struct*_tmpA5A=(struct Cyc_Absyn_Alias_d_struct*)
_tmpA52;if(_tmpA5A->tag != 5)goto _LL45E;}_LL45D: {const char*_tmpE7C;void*_tmpE7B;(
_tmpE7B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE7C="alias decl at toplevel",_tag_dyneither(_tmpE7C,sizeof(char),23))),
_tag_dyneither(_tmpE7B,sizeof(void*),0)));}_LL45E: {struct Cyc_Absyn_Aggr_d_struct*
_tmpA5B=(struct Cyc_Absyn_Aggr_d_struct*)_tmpA52;if(_tmpA5B->tag != 6)goto _LL460;
else{_tmpA5C=_tmpA5B->f1;}}_LL45F: Cyc_Toc_aggrdecl_to_c(_tmpA5C,1);goto _LL451;
_LL460: {struct Cyc_Absyn_Datatype_d_struct*_tmpA5D=(struct Cyc_Absyn_Datatype_d_struct*)
_tmpA52;if(_tmpA5D->tag != 7)goto _LL462;else{_tmpA5E=_tmpA5D->f1;}}_LL461: if(
_tmpA5E->is_extensible)Cyc_Toc_xdatatypedecl_to_c(_tmpA5E);else{Cyc_Toc_datatypedecl_to_c(
_tmpA5E);}goto _LL451;_LL462: {struct Cyc_Absyn_Enum_d_struct*_tmpA5F=(struct Cyc_Absyn_Enum_d_struct*)
_tmpA52;if(_tmpA5F->tag != 8)goto _LL464;else{_tmpA60=_tmpA5F->f1;}}_LL463: Cyc_Toc_enumdecl_to_c(
_tmpA60);{struct Cyc_List_List*_tmpE7D;Cyc_Toc_result_decls=((_tmpE7D=_cycalloc(
sizeof(*_tmpE7D)),((_tmpE7D->hd=d,((_tmpE7D->tl=Cyc_Toc_result_decls,_tmpE7D))))));}
goto _LL451;_LL464: {struct Cyc_Absyn_Typedef_d_struct*_tmpA61=(struct Cyc_Absyn_Typedef_d_struct*)
_tmpA52;if(_tmpA61->tag != 9)goto _LL466;else{_tmpA62=_tmpA61->f1;}}_LL465: _tmpA62->name=
_tmpA62->name;_tmpA62->tvs=0;if(_tmpA62->defn != 0){struct Cyc_Core_Opt*_tmpE7E;
_tmpA62->defn=((_tmpE7E=_cycalloc(sizeof(*_tmpE7E)),((_tmpE7E->v=Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA62->defn))->v),_tmpE7E))));}else{
switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmpA62->kind))->v)->kind){
case Cyc_Absyn_BoxKind: _LL472:{struct Cyc_Core_Opt*_tmpE7F;_tmpA62->defn=((_tmpE7F=
_cycalloc(sizeof(*_tmpE7F)),((_tmpE7F->v=Cyc_Absyn_void_star_typ(),_tmpE7F))));}
break;default: _LL473:{struct Cyc_Core_Opt*_tmpE80;_tmpA62->defn=((_tmpE80=
_cycalloc(sizeof(*_tmpE80)),((_tmpE80->v=(void*)& Cyc_Absyn_VoidType_val,_tmpE80))));}
break;}}{struct Cyc_List_List*_tmpE81;Cyc_Toc_result_decls=((_tmpE81=_cycalloc(
sizeof(*_tmpE81)),((_tmpE81->hd=d,((_tmpE81->tl=Cyc_Toc_result_decls,_tmpE81))))));}
goto _LL451;_LL466: {struct Cyc_Absyn_Porton_d_struct*_tmpA63=(struct Cyc_Absyn_Porton_d_struct*)
_tmpA52;if(_tmpA63->tag != 14)goto _LL468;}_LL467: goto _LL469;_LL468: {struct Cyc_Absyn_Portoff_d_struct*
_tmpA64=(struct Cyc_Absyn_Portoff_d_struct*)_tmpA52;if(_tmpA64->tag != 15)goto
_LL46A;}_LL469: goto _LL451;_LL46A: {struct Cyc_Absyn_Namespace_d_struct*_tmpA65=(
struct Cyc_Absyn_Namespace_d_struct*)_tmpA52;if(_tmpA65->tag != 10)goto _LL46C;
else{_tmpA66=_tmpA65->f2;}}_LL46B: _tmpA68=_tmpA66;goto _LL46D;_LL46C: {struct Cyc_Absyn_Using_d_struct*
_tmpA67=(struct Cyc_Absyn_Using_d_struct*)_tmpA52;if(_tmpA67->tag != 11)goto _LL46E;
else{_tmpA68=_tmpA67->f2;}}_LL46D: _tmpA6A=_tmpA68;goto _LL46F;_LL46E: {struct Cyc_Absyn_ExternC_d_struct*
_tmpA69=(struct Cyc_Absyn_ExternC_d_struct*)_tmpA52;if(_tmpA69->tag != 12)goto
_LL470;else{_tmpA6A=_tmpA69->f1;}}_LL46F: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA6A,top,
cinclude);goto _LL451;_LL470: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA6B=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmpA52;if(_tmpA6B->tag != 13)goto _LL451;
else{_tmpA6C=_tmpA6B->f1;}}_LL471: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA6C,top,1);goto
_LL451;_LL451:;};}return nv;}static void Cyc_Toc_init(){struct _DynRegionHandle*
_tmpA81;struct Cyc_Core_NewRegion _tmpA80=Cyc_Core__new_dynregion((const char*)"internal-error",
0);_tmpA81=_tmpA80.dynregion;{struct _DynRegionFrame _tmpA82;struct _RegionHandle*d=
_open_dynregion(& _tmpA82,_tmpA81);{struct Cyc_Dict_Dict*_tmpE8E;struct Cyc_Dict_Dict*
_tmpE8D;struct Cyc_Set_Set**_tmpE8C;struct Cyc_List_List**_tmpE8B;struct Cyc_Dict_Dict*
_tmpE8A;struct Cyc_List_List**_tmpE89;struct Cyc_Toc_TocState*_tmpE88;Cyc_Toc_toc_state=((
_tmpE88=_cycalloc(sizeof(*_tmpE88)),((_tmpE88->dyn=(struct _DynRegionHandle*)
_tmpA81,((_tmpE88->tuple_types=(struct Cyc_List_List**)((_tmpE89=_region_malloc(d,
sizeof(*_tmpE89)),((_tmpE89[0]=0,_tmpE89)))),((_tmpE88->aggrs_so_far=(struct Cyc_Dict_Dict*)((
_tmpE8A=_region_malloc(d,sizeof(*_tmpE8A)),((_tmpE8A[0]=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
d,Cyc_Absyn_qvar_cmp),_tmpE8A)))),((_tmpE88->abs_struct_types=(struct Cyc_List_List**)((
_tmpE8B=_region_malloc(d,sizeof(*_tmpE8B)),((_tmpE8B[0]=0,_tmpE8B)))),((_tmpE88->datatypes_so_far=(
struct Cyc_Set_Set**)((_tmpE8C=_region_malloc(d,sizeof(*_tmpE8C)),((_tmpE8C[0]=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE8C)))),((_tmpE88->xdatatypes_so_far=(
struct Cyc_Dict_Dict*)((_tmpE8D=_region_malloc(d,sizeof(*_tmpE8D)),((_tmpE8D[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE8D)))),((_tmpE88->qvar_tags=(
struct Cyc_Dict_Dict*)((_tmpE8E=_region_malloc(d,sizeof(*_tmpE8E)),((_tmpE8E[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple13*,struct
_tuple13*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpE8E)))),((_tmpE88->temp_labels=(
struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(
d),_tmpE88))))))))))))))))));}Cyc_Toc_result_decls=0;Cyc_Toc_tuple_type_counter=
0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=0;Cyc_Toc_total_bounds_checks=
0;Cyc_Toc_bounds_checks_eliminated=0;{struct _dyneither_ptr**_tmpE8F;Cyc_Toc_globals=
_tag_dyneither(((_tmpE8F=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpE8F[
0]=& Cyc_Toc__throw_str,((_tmpE8F[1]=& Cyc_Toc_setjmp_str,((_tmpE8F[2]=& Cyc_Toc__push_handler_str,((
_tmpE8F[3]=& Cyc_Toc__pop_handler_str,((_tmpE8F[4]=& Cyc_Toc__exn_thrown_str,((
_tmpE8F[5]=& Cyc_Toc__npop_handler_str,((_tmpE8F[6]=& Cyc_Toc__check_null_str,((
_tmpE8F[7]=& Cyc_Toc__check_known_subscript_null_str,((_tmpE8F[8]=& Cyc_Toc__check_known_subscript_notnull_str,((
_tmpE8F[9]=& Cyc_Toc__check_dyneither_subscript_str,((_tmpE8F[10]=& Cyc_Toc__dyneither_ptr_str,((
_tmpE8F[11]=& Cyc_Toc__tag_dyneither_str,((_tmpE8F[12]=& Cyc_Toc__init_dyneither_ptr_str,((
_tmpE8F[13]=& Cyc_Toc__untag_dyneither_ptr_str,((_tmpE8F[14]=& Cyc_Toc__get_dyneither_size_str,((
_tmpE8F[15]=& Cyc_Toc__get_zero_arr_size_str,((_tmpE8F[16]=& Cyc_Toc__dyneither_ptr_plus_str,((
_tmpE8F[17]=& Cyc_Toc__zero_arr_plus_str,((_tmpE8F[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((
_tmpE8F[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpE8F[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((
_tmpE8F[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpE8F[22]=& Cyc_Toc__cycalloc_str,((
_tmpE8F[23]=& Cyc_Toc__cyccalloc_str,((_tmpE8F[24]=& Cyc_Toc__cycalloc_atomic_str,((
_tmpE8F[25]=& Cyc_Toc__cyccalloc_atomic_str,((_tmpE8F[26]=& Cyc_Toc__region_malloc_str,((
_tmpE8F[27]=& Cyc_Toc__region_calloc_str,((_tmpE8F[28]=& Cyc_Toc__check_times_str,((
_tmpE8F[29]=& Cyc_Toc__new_region_str,((_tmpE8F[30]=& Cyc_Toc__push_region_str,((
_tmpE8F[31]=& Cyc_Toc__pop_region_str,((_tmpE8F[32]=& Cyc_Toc__open_dynregion_str,((
_tmpE8F[33]=& Cyc_Toc__push_dynregion_str,((_tmpE8F[34]=& Cyc_Toc__pop_dynregion_str,((
_tmpE8F[35]=& Cyc_Toc__reset_region_str,((_tmpE8F[36]=& Cyc_Toc__throw_arraybounds_str,((
_tmpE8F[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpE8F[38]=& Cyc_Toc__throw_match_str,((
_tmpE8F[39]=& Cyc_Toc__swap_word_str,((_tmpE8F[40]=& Cyc_Toc__swap_dyneither_str,
_tmpE8F)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),41);};;_pop_dynregion(d);};}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();{struct _RegionHandle _tmpA8B=
_new_region("start");struct _RegionHandle*start=& _tmpA8B;_push_region(start);Cyc_Toc_decls_to_c(
start,Cyc_Toc_empty_env(start),ds,1,0);{struct _DynRegionHandle*_tmpA8D;struct Cyc_Toc_TocState
_tmpA8C=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA8D=
_tmpA8C.dyn;Cyc_Core_free_dynregion(_tmpA8D);};{struct Cyc_List_List*_tmpA8E=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);
_npop_handler(0);return _tmpA8E;};;_pop_region(start);};}
