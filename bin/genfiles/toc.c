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
r2,struct Cyc_List_List*x,struct Cyc_List_List*y);unsigned long Cyc_strlen(struct
_dyneither_ptr s);int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(
struct _RegionHandle*r,int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(
struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*
s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};
struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*
r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
struct Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*,struct Cyc_Dict_Dict);
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(
struct Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*
k);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr
line;int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct _dyneither_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
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
tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;
};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple0*f1;
struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct
Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_struct{
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_RefCntRgn_struct{
int tag;};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
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
_tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct
_tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct
_tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{
int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};
struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct
_dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};
struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
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
e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple10{struct Cyc_Absyn_Tqual
f1;void*f2;};void*Cyc_Tcutil_snd_tqt(struct _tuple10*);void Cyc_Tcutil_check_no_qual(
struct Cyc_Position_Segment*loc,void*t);struct _tuple11{unsigned int f1;int f2;};
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_c_can_eval(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};
struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL
 = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};struct
_union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple12{struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple12 val;};struct
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
f1;int f2;void*f3;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple13{
struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{
int tag;struct _tuple13 val;};union Cyc_CfFlowInfo_FlowInfo{struct
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
0;struct _tuple14{struct _tuple0*f1;struct _dyneither_ptr f2;};int Cyc_Toc_qvar_tag_cmp(
struct _tuple14*x,struct _tuple14*y){struct _tuple0*_tmp1;struct _dyneither_ptr _tmp2;
struct _tuple14 _tmp0=*x;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;{struct _tuple0*_tmp4;struct
_dyneither_ptr _tmp5;struct _tuple14 _tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{int i=
Cyc_Absyn_qvar_cmp(_tmp1,_tmp4);if(i != 0)return i;return Cyc_strcmp((struct
_dyneither_ptr)_tmp2,(struct _dyneither_ptr)_tmp5);};};}struct _tuple15{struct Cyc_Absyn_Aggrdecl*
f1;void*f2;};void*Cyc_Toc_aggrdecl_type(struct _tuple0*q,void*(*type_maker)(
struct _tuple0*)){struct _DynRegionHandle*_tmp7;struct Cyc_Dict_Dict*_tmp8;struct
Cyc_Toc_TocState _tmp6=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp7=_tmp6.dyn;_tmp8=_tmp6.aggrs_so_far;{struct _DynRegionFrame _tmp9;struct
_RegionHandle*d=_open_dynregion(& _tmp9,_tmp7);{struct _tuple15**v=((struct
_tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp8,
q);if(v == 0){void*_tmpA=type_maker(q);_npop_handler(0);return _tmpA;}else{struct
_tuple15 _tmpC;void*_tmpD;struct _tuple15*_tmpB=*v;_tmpC=*_tmpB;_tmpD=_tmpC.f2;{
void*_tmpE=_tmpD;_npop_handler(0);return _tmpE;};}};_pop_dynregion(d);};}static
int Cyc_Toc_tuple_type_counter=0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=
0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_struct{
char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_struct{
char*tag;};static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const char*_tmpAA2;void*_tmpAA1;(_tmpAA1=0,
Cyc_fprintf(Cyc_stderr,((_tmpAA2="\n",_tag_dyneither(_tmpAA2,sizeof(char),2))),
_tag_dyneither(_tmpAA1,sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_struct _tmpAA5;struct Cyc_Toc_Toc_Unimplemented_struct*
_tmpAA4;(int)_throw((void*)((_tmpAA4=_cycalloc_atomic(sizeof(*_tmpAA4)),((
_tmpAA4[0]=((_tmpAA5.tag=Cyc_Toc_Toc_Unimplemented,_tmpAA5)),_tmpAA4)))));};}
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(
Cyc_stderr,fmt,ap);{const char*_tmpAA8;void*_tmpAA7;(_tmpAA7=0,Cyc_fprintf(Cyc_stderr,((
_tmpAA8="\n",_tag_dyneither(_tmpAA8,sizeof(char),2))),_tag_dyneither(_tmpAA7,
sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Impossible_struct
_tmpAAB;struct Cyc_Toc_Toc_Impossible_struct*_tmpAAA;(int)_throw((void*)((_tmpAAA=
_cycalloc_atomic(sizeof(*_tmpAAA)),((_tmpAAA[0]=((_tmpAAB.tag=Cyc_Toc_Toc_Impossible,
_tmpAAB)),_tmpAAA)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_struct{
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
_tmpAAC;skip_stmt_opt=((_tmpAAC=_cycalloc(sizeof(*_tmpAAC)),((_tmpAAC[0]=Cyc_Absyn_skip_stmt(
0),_tmpAAC))));}return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(
void*t,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,
0);}static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Cast_e_struct
_tmpAAF;struct Cyc_Absyn_Cast_e_struct*_tmpAAE;return(void*)((_tmpAAE=_cycalloc(
sizeof(*_tmpAAE)),((_tmpAAE[0]=((_tmpAAF.tag=15,((_tmpAAF.f1=(void*)t,((_tmpAAF.f2=
e,((_tmpAAF.f3=0,((_tmpAAF.f4=Cyc_Absyn_No_coercion,_tmpAAF)))))))))),_tmpAAE))));}
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Deref_e_struct
_tmpAB2;struct Cyc_Absyn_Deref_e_struct*_tmpAB1;return(void*)((_tmpAB1=_cycalloc(
sizeof(*_tmpAB1)),((_tmpAB1[0]=((_tmpAB2.tag=21,((_tmpAB2.f1=e,_tmpAB2)))),
_tmpAB1))));}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){struct Cyc_Absyn_Subscript_e_struct _tmpAB5;struct Cyc_Absyn_Subscript_e_struct*
_tmpAB4;return(void*)((_tmpAB4=_cycalloc(sizeof(*_tmpAB4)),((_tmpAB4[0]=((
_tmpAB5.tag=24,((_tmpAB5.f1=e1,((_tmpAB5.f2=e2,_tmpAB5)))))),_tmpAB4))));}static
void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct
_tmpAB8;struct Cyc_Absyn_StmtExp_e_struct*_tmpAB7;return(void*)((_tmpAB7=
_cycalloc(sizeof(*_tmpAB7)),((_tmpAB7[0]=((_tmpAB8.tag=37,((_tmpAB8.f1=s,_tmpAB8)))),
_tmpAB7))));}static void*Cyc_Toc_sizeoftyp_exp_r(void*t){struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpABB;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpABA;return(void*)((_tmpABA=
_cycalloc(sizeof(*_tmpABA)),((_tmpABA[0]=((_tmpABB.tag=18,((_tmpABB.f1=(void*)t,
_tmpABB)))),_tmpABA))));}static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct _tmpABE;struct Cyc_Absyn_FnCall_e_struct*
_tmpABD;return(void*)((_tmpABD=_cycalloc(sizeof(*_tmpABD)),((_tmpABD[0]=((
_tmpABE.tag=11,((_tmpABE.f1=e,((_tmpABE.f2=es,((_tmpABE.f3=0,_tmpABE)))))))),
_tmpABD))));}static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_struct
_tmpAC1;struct Cyc_Absyn_Exp_s_struct*_tmpAC0;return(void*)((_tmpAC0=_cycalloc(
sizeof(*_tmpAC0)),((_tmpAC0[0]=((_tmpAC1.tag=1,((_tmpAC1.f1=e,_tmpAC1)))),
_tmpAC0))));}static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2){struct Cyc_Absyn_Seq_s_struct _tmpAC4;struct Cyc_Absyn_Seq_s_struct*_tmpAC3;
return(void*)((_tmpAC3=_cycalloc(sizeof(*_tmpAC3)),((_tmpAC3[0]=((_tmpAC4.tag=2,((
_tmpAC4.f1=s1,((_tmpAC4.f2=s2,_tmpAC4)))))),_tmpAC3))));}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct
_tmpAC7;struct Cyc_Absyn_Conditional_e_struct*_tmpAC6;return(void*)((_tmpAC6=
_cycalloc(sizeof(*_tmpAC6)),((_tmpAC6[0]=((_tmpAC7.tag=6,((_tmpAC7.f1=e1,((
_tmpAC7.f2=e2,((_tmpAC7.f3=e3,_tmpAC7)))))))),_tmpAC6))));}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrMember_e_struct
_tmpACA;struct Cyc_Absyn_AggrMember_e_struct*_tmpAC9;return(void*)((_tmpAC9=
_cycalloc(sizeof(*_tmpAC9)),((_tmpAC9[0]=((_tmpACA.tag=22,((_tmpACA.f1=e,((
_tmpACA.f2=n,((_tmpACA.f3=0,((_tmpACA.f4=0,_tmpACA)))))))))),_tmpAC9))));}static
void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_struct _tmpACD;struct Cyc_Absyn_AggrArrow_e_struct*
_tmpACC;return(void*)((_tmpACC=_cycalloc(sizeof(*_tmpACC)),((_tmpACC[0]=((
_tmpACD.tag=23,((_tmpACD.f1=e,((_tmpACD.f2=n,((_tmpACD.f3=0,((_tmpACD.f4=0,
_tmpACD)))))))))),_tmpACC))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmpAD0;
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpACF;return(void*)((_tmpACF=_cycalloc(
sizeof(*_tmpACF)),((_tmpACF[0]=((_tmpAD0.tag=36,((_tmpAD0.f1=tdopt,((_tmpAD0.f2=
ds,_tmpAD0)))))),_tmpACF))));}static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_struct _tmpAD3;
struct Cyc_Absyn_Goto_s_struct*_tmpAD2;return(void*)((_tmpAD2=_cycalloc(sizeof(*
_tmpAD2)),((_tmpAD2[0]=((_tmpAD3.tag=8,((_tmpAD3.f1=v,((_tmpAD3.f2=s,_tmpAD3)))))),
_tmpAD2))));}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{
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
Cyc_Core_Impossible_struct _tmpAD9;const char*_tmpAD8;struct Cyc_Core_Impossible_struct*
_tmpAD7;(int)_throw((void*)((_tmpAD7=_cycalloc(sizeof(*_tmpAD7)),((_tmpAD7[0]=((
_tmpAD9.tag=Cyc_Core_Impossible,((_tmpAD9.f1=((_tmpAD8="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpAD8,sizeof(char),44))),_tmpAD9)))),_tmpAD7)))));}}goto _LL0;
_LL3: {struct Cyc_Absyn_FloatType_struct*_tmpC9=(struct Cyc_Absyn_FloatType_struct*)
_tmpC6;if(_tmpC9->tag != 7)goto _LL5;else{_tmpCA=_tmpC9->f1;if(_tmpCA != 0)goto _LL5;}}
_LL4: function=fS->ffloat;goto _LL0;_LL5: {struct Cyc_Absyn_FloatType_struct*_tmpCB=(
struct Cyc_Absyn_FloatType_struct*)_tmpC6;if(_tmpCB->tag != 7)goto _LL7;else{_tmpCC=
_tmpCB->f1;if(_tmpCC != 1)goto _LL7;}}_LL6: function=fS->fdouble;goto _LL0;_LL7: {
struct Cyc_Absyn_FloatType_struct*_tmpCD=(struct Cyc_Absyn_FloatType_struct*)
_tmpC6;if(_tmpCD->tag != 7)goto _LL9;}_LL8: function=fS->flongdouble;goto _LL0;_LL9: {
struct Cyc_Absyn_PointerType_struct*_tmpCE=(struct Cyc_Absyn_PointerType_struct*)
_tmpC6;if(_tmpCE->tag != 5)goto _LLB;}_LLA: function=fS->fvoidstar;goto _LL0;_LLB:;
_LLC: {struct Cyc_Core_Impossible_struct _tmpAE6;const char*_tmpAE5;void*_tmpAE4[1];
struct Cyc_String_pa_struct _tmpAE3;struct Cyc_Core_Impossible_struct*_tmpAE2;(int)
_throw((void*)((_tmpAE2=_cycalloc(sizeof(*_tmpAE2)),((_tmpAE2[0]=((_tmpAE6.tag=
Cyc_Core_Impossible,((_tmpAE6.f1=(struct _dyneither_ptr)((_tmpAE3.tag=0,((_tmpAE3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAE4[
0]=& _tmpAE3,Cyc_aprintf(((_tmpAE5="impossible expression type %s (not int, float, double, or pointer)",
_tag_dyneither(_tmpAE5,sizeof(char),67))),_tag_dyneither(_tmpAE4,sizeof(void*),1)))))))),
_tmpAE6)))),_tmpAE2)))));}_LL0:;}return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(
fS,(void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v);}struct Cyc_Absyn_Exp*
Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*
arr){void*_tmpD7=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
arr->topt))->v);struct Cyc_Absyn_PtrInfo _tmpD9;void*_tmpDA;_LL13: {struct Cyc_Absyn_PointerType_struct*
_tmpD8=(struct Cyc_Absyn_PointerType_struct*)_tmpD7;if(_tmpD8->tag != 5)goto _LL15;
else{_tmpD9=_tmpD8->f1;_tmpDA=_tmpD9.elt_typ;}}_LL14: return Cyc_Toc_getFunctionType(
fS,_tmpDA);_LL15:;_LL16: {struct Cyc_Core_Impossible_struct _tmpAEC;const char*
_tmpAEB;struct Cyc_Core_Impossible_struct*_tmpAEA;(int)_throw((void*)((_tmpAEA=
_cycalloc(sizeof(*_tmpAEA)),((_tmpAEA[0]=((_tmpAEC.tag=Cyc_Core_Impossible,((
_tmpAEC.f1=((_tmpAEB="impossible type (not pointer)",_tag_dyneither(_tmpAEB,
sizeof(char),30))),_tmpAEC)))),_tmpAEA)))));}_LL12:;}struct _tuple16{struct Cyc_List_List*
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
Cyc_Absyn_conref_def)(0,_tmp106);_LL35:;_LL36: {const char*_tmpAEF;void*_tmpAEE;(
_tmpAEE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAEF="is_nullable",_tag_dyneither(_tmpAEF,sizeof(char),12))),_tag_dyneither(
_tmpAEE,sizeof(void*),0)));}_LL32:;}static char _tmp115[1]="";static char _tmp116[8]="*bogus*";
static struct _tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct
_dyneither_ptr tag){struct _DynRegionHandle*_tmp10A;struct Cyc_Dict_Dict*_tmp10B;
struct Cyc_Toc_TocState _tmp109=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp10A=_tmp109.dyn;_tmp10B=_tmp109.qvar_tags;{static struct _dyneither_ptr
bogus_string={_tmp116,_tmp116,_tmp116 + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
4,0}},& bogus_string};static struct _tuple14 pair={& bogus_qvar,{_tmp115,_tmp115,
_tmp115 + 1}};{struct _tuple14 _tmpAF0;pair=((_tmpAF0.f1=x,((_tmpAF0.f2=tag,_tmpAF0))));}{
struct _DynRegionFrame _tmp10D;struct _RegionHandle*d=_open_dynregion(& _tmp10D,
_tmp10A);{struct _tuple0**_tmp10E=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,
struct _tuple14*k))Cyc_Dict_lookup_opt)(*_tmp10B,(struct _tuple14*)& pair);if(
_tmp10E != 0){struct _tuple0*_tmp10F=*_tmp10E;_npop_handler(0);return _tmp10F;}{
struct _tuple14*_tmpAF1;struct _tuple14*_tmp110=(_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((
_tmpAF1->f1=x,((_tmpAF1->f2=tag,_tmpAF1)))));struct _dyneither_ptr*_tmpAF4;struct
_tuple0*_tmpAF3;struct _tuple0*res=(_tmpAF3=_cycalloc(sizeof(*_tmpAF3)),((_tmpAF3->f1=(*
x).f1,((_tmpAF3->f2=((_tmpAF4=_cycalloc(sizeof(*_tmpAF4)),((_tmpAF4[0]=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*x).f2,(struct
_dyneither_ptr)tag),_tmpAF4)))),_tmpAF3)))));*_tmp10B=((struct Cyc_Dict_Dict(*)(
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
_npop_handler(0);return _tmp121;}};}}{struct Cyc_Int_pa_struct _tmpAFC;void*_tmpAFB[
1];const char*_tmpAFA;struct _dyneither_ptr*_tmpAF9;struct _dyneither_ptr*xname=(
_tmpAF9=_cycalloc(sizeof(*_tmpAF9)),((_tmpAF9[0]=(struct _dyneither_ptr)((_tmpAFC.tag=
1,((_tmpAFC.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAFB[0]=& _tmpAFC,
Cyc_aprintf(((_tmpAFA="_tuple%d",_tag_dyneither(_tmpAFA,sizeof(char),9))),
_tag_dyneither(_tmpAFB,sizeof(void*),1)))))))),_tmpAF9)));void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp122=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmpAFF;struct Cyc_List_List*
_tmpAFE;_tmp122=((_tmpAFE=_cycalloc(sizeof(*_tmpAFE)),((_tmpAFE->hd=((_tmpAFF=
_cycalloc(sizeof(*_tmpAFF)),((_tmpAFF->name=Cyc_Absyn_fieldname(i),((_tmpAFF->tq=
Cyc_Toc_mt_tq,((_tmpAFF->type=(void*)ts2->hd,((_tmpAFF->width=0,((_tmpAFF->attributes=
0,_tmpAFF)))))))))))),((_tmpAFE->tl=_tmp122,_tmpAFE))))));}}_tmp122=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp122);{struct Cyc_Absyn_AggrdeclImpl*
_tmpB04;struct _tuple0*_tmpB03;struct Cyc_Absyn_Aggrdecl*_tmpB02;struct Cyc_Absyn_Aggrdecl*
_tmp125=(_tmpB02=_cycalloc(sizeof(*_tmpB02)),((_tmpB02->kind=Cyc_Absyn_StructA,((
_tmpB02->sc=Cyc_Absyn_Public,((_tmpB02->name=((_tmpB03=_cycalloc(sizeof(*_tmpB03)),((
_tmpB03->f1=Cyc_Absyn_Rel_n(0),((_tmpB03->f2=xname,_tmpB03)))))),((_tmpB02->tvs=
0,((_tmpB02->impl=((_tmpB04=_cycalloc(sizeof(*_tmpB04)),((_tmpB04->exist_vars=0,((
_tmpB04->rgn_po=0,((_tmpB04->fields=_tmp122,((_tmpB04->tagged=0,_tmpB04)))))))))),((
_tmpB02->attributes=0,_tmpB02)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*
_tmpB0A;struct Cyc_Absyn_Aggr_d_struct _tmpB09;struct Cyc_List_List*_tmpB08;Cyc_Toc_result_decls=((
_tmpB08=_cycalloc(sizeof(*_tmpB08)),((_tmpB08->hd=Cyc_Absyn_new_decl((void*)((
_tmpB0A=_cycalloc(sizeof(*_tmpB0A)),((_tmpB0A[0]=((_tmpB09.tag=6,((_tmpB09.f1=
_tmp125,_tmpB09)))),_tmpB0A)))),0),((_tmpB08->tl=Cyc_Toc_result_decls,_tmpB08))))));}{
struct _tuple17*_tmpB0D;struct Cyc_List_List*_tmpB0C;*_tmp119=((_tmpB0C=
_region_malloc(d,sizeof(*_tmpB0C)),((_tmpB0C->hd=((_tmpB0D=_region_malloc(d,
sizeof(*_tmpB0D)),((_tmpB0D->f1=x,((_tmpB0D->f2=ts,_tmpB0D)))))),((_tmpB0C->tl=*
_tmp119,_tmpB0C))))));}{void*_tmp12B=x;_npop_handler(0);return _tmp12B;};};};;
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
struct Cyc_Int_pa_struct _tmpB15;void*_tmpB14[1];const char*_tmpB13;struct
_dyneither_ptr*_tmpB12;struct _dyneither_ptr*xname=(_tmpB12=_cycalloc(sizeof(*
_tmpB12)),((_tmpB12[0]=(struct _dyneither_ptr)((_tmpB15.tag=1,((_tmpB15.f1=(
unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB14[0]=& _tmpB15,Cyc_aprintf(((
_tmpB13="_tuple%d",_tag_dyneither(_tmpB13,sizeof(char),9))),_tag_dyneither(
_tmpB14,sizeof(void*),1)))))))),_tmpB12)));void*x=Cyc_Absyn_strct(xname);struct
Cyc_List_List*_tmp13F=0;{struct _tuple18*_tmpB18;struct Cyc_List_List*_tmpB17;*
_tmp135=((_tmpB17=_region_malloc(d,sizeof(*_tmpB17)),((_tmpB17->hd=((_tmpB18=
_region_malloc(d,sizeof(*_tmpB18)),((_tmpB18->f1=struct_name,((_tmpB18->f2=
type_args,((_tmpB18->f3=x,_tmpB18)))))))),((_tmpB17->tl=*_tmp135,_tmpB17))))));}{
struct _RegionHandle _tmp142=_new_region("r");struct _RegionHandle*r=& _tmp142;
_push_region(r);{struct Cyc_List_List*_tmp143=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,type_vars,type_args);for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp144=(struct Cyc_Absyn_Aggrfield*)fields->hd;void*t=
_tmp144->type;t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp143,t));if(Cyc_Tcutil_unify(
t,(void*)& Cyc_Absyn_VoidType_val))break;t=Cyc_Toc_typ_to_c(t);{struct Cyc_Absyn_Aggrfield*
_tmpB1B;struct Cyc_List_List*_tmpB1A;_tmp13F=((_tmpB1A=_cycalloc(sizeof(*_tmpB1A)),((
_tmpB1A->hd=((_tmpB1B=_cycalloc(sizeof(*_tmpB1B)),((_tmpB1B->name=_tmp144->name,((
_tmpB1B->tq=Cyc_Toc_mt_tq,((_tmpB1B->type=t,((_tmpB1B->width=_tmp144->width,((
_tmpB1B->attributes=_tmp144->attributes,_tmpB1B)))))))))))),((_tmpB1A->tl=
_tmp13F,_tmpB1A))))));};}_tmp13F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp13F);{struct Cyc_Absyn_AggrdeclImpl*_tmpB20;struct _tuple0*
_tmpB1F;struct Cyc_Absyn_Aggrdecl*_tmpB1E;struct Cyc_Absyn_Aggrdecl*_tmp147=(
_tmpB1E=_cycalloc(sizeof(*_tmpB1E)),((_tmpB1E->kind=Cyc_Absyn_StructA,((_tmpB1E->sc=
Cyc_Absyn_Public,((_tmpB1E->name=((_tmpB1F=_cycalloc(sizeof(*_tmpB1F)),((_tmpB1F->f1=
Cyc_Absyn_Rel_n(0),((_tmpB1F->f2=xname,_tmpB1F)))))),((_tmpB1E->tvs=0,((_tmpB1E->impl=((
_tmpB20=_cycalloc(sizeof(*_tmpB20)),((_tmpB20->exist_vars=0,((_tmpB20->rgn_po=0,((
_tmpB20->fields=_tmp13F,((_tmpB20->tagged=0,_tmpB20)))))))))),((_tmpB1E->attributes=
0,_tmpB1E)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpB26;struct Cyc_Absyn_Aggr_d_struct
_tmpB25;struct Cyc_List_List*_tmpB24;Cyc_Toc_result_decls=((_tmpB24=_cycalloc(
sizeof(*_tmpB24)),((_tmpB24->hd=Cyc_Absyn_new_decl((void*)((_tmpB26=_cycalloc(
sizeof(*_tmpB26)),((_tmpB26[0]=((_tmpB25.tag=6,((_tmpB25.f1=_tmp147,_tmpB25)))),
_tmpB26)))),0),((_tmpB24->tl=Cyc_Toc_result_decls,_tmpB24))))));}{void*_tmp14B=x;
_npop_handler(1);return _tmp14B;};};};_pop_region(r);};};;_pop_dynregion(d);};}
struct _tuple0*Cyc_Toc_temp_var(){int _tmp153=Cyc_Toc_temp_var_counter ++;struct
_dyneither_ptr*_tmpB33;const char*_tmpB32;void*_tmpB31[1];struct Cyc_Int_pa_struct
_tmpB30;struct _tuple0*_tmpB2F;struct _tuple0*res=(_tmpB2F=_cycalloc(sizeof(*
_tmpB2F)),((_tmpB2F->f1=Cyc_Absyn_Loc_n,((_tmpB2F->f2=((_tmpB33=_cycalloc(
sizeof(*_tmpB33)),((_tmpB33[0]=(struct _dyneither_ptr)((_tmpB30.tag=1,((_tmpB30.f1=(
unsigned int)_tmp153,((_tmpB31[0]=& _tmpB30,Cyc_aprintf(((_tmpB32="_tmp%X",
_tag_dyneither(_tmpB32,sizeof(char),7))),_tag_dyneither(_tmpB31,sizeof(void*),1)))))))),
_tmpB33)))),_tmpB2F)))));return res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp15A;struct Cyc_Xarray_Xarray*_tmp15B;struct Cyc_Toc_TocState
_tmp159=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp15A=
_tmp159.dyn;_tmp15B=_tmp159.temp_labels;{struct _DynRegionFrame _tmp15C;struct
_RegionHandle*d=_open_dynregion(& _tmp15C,_tmp15A);{int _tmp15D=Cyc_Toc_fresh_label_counter
++;if(_tmp15D < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp15B)){
struct _dyneither_ptr*_tmp15E=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp15B,_tmp15D);_npop_handler(0);return _tmp15E;}{struct Cyc_Int_pa_struct
_tmpB3B;void*_tmpB3A[1];const char*_tmpB39;struct _dyneither_ptr*_tmpB38;struct
_dyneither_ptr*res=(_tmpB38=_cycalloc(sizeof(*_tmpB38)),((_tmpB38[0]=(struct
_dyneither_ptr)((_tmpB3B.tag=1,((_tmpB3B.f1=(unsigned int)_tmp15D,((_tmpB3A[0]=&
_tmpB3B,Cyc_aprintf(((_tmpB39="_LL%X",_tag_dyneither(_tmpB39,sizeof(char),6))),
_tag_dyneither(_tmpB3A,sizeof(void*),1)))))))),_tmpB38)));if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp15B,res)!= _tmp15D){const char*
_tmpB3E;void*_tmpB3D;(_tmpB3D=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3E="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmpB3E,sizeof(char),43))),_tag_dyneither(_tmpB3D,sizeof(void*),0)));}{
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
struct _tuple8*_tmpB3F;return(_tmpB3F=_cycalloc(sizeof(*_tmpB3F)),((_tmpB3F->f1=
_tmp168,((_tmpB3F->f2=_tmp169,((_tmpB3F->f3=Cyc_Toc_typ_to_c(_tmp16A),_tmpB3F)))))));};}
static struct _tuple10*Cyc_Toc_typ_to_c_f(struct _tuple10*x){struct Cyc_Absyn_Tqual
_tmp16D;void*_tmp16E;struct _tuple10 _tmp16C=*x;_tmp16D=_tmp16C.f1;_tmp16E=_tmp16C.f2;{
struct _tuple10*_tmpB40;return(_tmpB40=_cycalloc(sizeof(*_tmpB40)),((_tmpB40->f1=
_tmp16D,((_tmpB40->f2=Cyc_Toc_typ_to_c(_tmp16E),_tmpB40)))));};}static void*Cyc_Toc_typ_to_c_array(
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
f){struct Cyc_Absyn_Aggrfield*_tmpB41;return(_tmpB41=_cycalloc(sizeof(*_tmpB41)),((
_tmpB41->name=f->name,((_tmpB41->tq=Cyc_Toc_mt_tq,((_tmpB41->type=Cyc_Toc_typ_to_c(
f->type),((_tmpB41->width=f->width,((_tmpB41->attributes=f->attributes,_tmpB41)))))))))));}
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){return;}static void*Cyc_Toc_char_star_typ(){
static void**cs=0;if(cs == 0){void**_tmpB42;cs=((_tmpB42=_cycalloc(sizeof(*_tmpB42)),((
_tmpB42[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpB42))));}return*cs;}static void*Cyc_Toc_rgn_typ(){
static void**r=0;if(r == 0){void**_tmpB43;r=((_tmpB43=_cycalloc(sizeof(*_tmpB43)),((
_tmpB43[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),
_tmpB43))));}return*r;}static void*Cyc_Toc_dyn_rgn_typ(){static void**r=0;if(r == 0){
void**_tmpB44;r=((_tmpB44=_cycalloc(sizeof(*_tmpB44)),((_tmpB44[0]=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpB44))));}return*
r;}static int Cyc_Toc_is_boxed_tvar(void*t){void*_tmp180=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_Tvar*_tmp182;_LL3F: {struct Cyc_Absyn_VarType_struct*_tmp181=(
struct Cyc_Absyn_VarType_struct*)_tmp180;if(_tmp181->tag != 2)goto _LL41;else{
_tmp182=_tmp181->f1;}}_LL40: return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),&
Cyc_Tcutil_tbk);_LL41:;_LL42: return 0;_LL3E:;}static int Cyc_Toc_is_abs_tvar(void*t){
void*_tmp183=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp185;_LL44: {struct
Cyc_Absyn_VarType_struct*_tmp184=(struct Cyc_Absyn_VarType_struct*)_tmp183;if(
_tmp184->tag != 2)goto _LL46;else{_tmp185=_tmp184->f1;}}_LL45: return Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t));_LL46:;_LL47: return 0;_LL43:;}static void*
Cyc_Toc_typ_to_c(void*t){void*_tmp186=t;struct Cyc_Core_Opt*_tmp189;struct Cyc_Core_Opt*
_tmp18B;struct Cyc_Core_Opt _tmp18C;void*_tmp18D;struct Cyc_Absyn_Tvar*_tmp18F;
struct Cyc_Absyn_DatatypeFieldInfo _tmp192;union Cyc_Absyn_DatatypeFieldInfoU
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
const char*_tmpB45;return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp196->name,((
_tmpB45="_struct",_tag_dyneither(_tmpB45,sizeof(char),8)))));}_LL55: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp197=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp186;if(_tmp197->tag != 4)
goto _LL57;}_LL56: {const char*_tmpB48;void*_tmpB47;(_tmpB47=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB48="unresolved DatatypeFieldType",
_tag_dyneither(_tmpB48,sizeof(char),29))),_tag_dyneither(_tmpB47,sizeof(void*),0)));}
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
_LL96: continue;_LL97:;_LL98:{struct Cyc_List_List*_tmpB49;_tmp1E3=((_tmpB49=
_cycalloc(sizeof(*_tmpB49)),((_tmpB49->hd=(void*)_tmp1AD->hd,((_tmpB49->tl=
_tmp1E3,_tmpB49))))));}goto _LL8C;_LL8C:;}{struct Cyc_List_List*_tmp1EB=((struct
Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_arg_to_c,_tmp1AA);if(_tmp1AC != 0){void*_tmp1EC=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp1AC->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple8*_tmpB4A;struct _tuple8*_tmp1ED=(_tmpB4A=_cycalloc(sizeof(*_tmpB4A)),((
_tmpB4A->f1=_tmp1AC->name,((_tmpB4A->f2=_tmp1AC->tq,((_tmpB4A->f3=_tmp1EC,
_tmpB4A)))))));struct Cyc_List_List*_tmpB4B;_tmp1EB=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1EB,((
_tmpB4B=_cycalloc(sizeof(*_tmpB4B)),((_tmpB4B->hd=_tmp1ED,((_tmpB4B->tl=0,
_tmpB4B)))))));}{struct Cyc_Absyn_FnType_struct _tmpB51;struct Cyc_Absyn_FnInfo
_tmpB50;struct Cyc_Absyn_FnType_struct*_tmpB4F;return(void*)((_tmpB4F=_cycalloc(
sizeof(*_tmpB4F)),((_tmpB4F[0]=((_tmpB51.tag=9,((_tmpB51.f1=((_tmpB50.tvars=0,((
_tmpB50.effect=0,((_tmpB50.ret_tqual=_tmp1A8,((_tmpB50.ret_typ=Cyc_Toc_typ_to_c(
_tmp1A9),((_tmpB50.args=_tmp1EB,((_tmpB50.c_varargs=_tmp1AB,((_tmpB50.cyc_varargs=
0,((_tmpB50.rgn_po=0,((_tmpB50.attributes=_tmp1E3,_tmpB50)))))))))))))))))),
_tmpB51)))),_tmpB4F))));};};}_LL61: {struct Cyc_Absyn_TupleType_struct*_tmp1AE=(
struct Cyc_Absyn_TupleType_struct*)_tmp186;if(_tmp1AE->tag != 10)goto _LL63;else{
_tmp1AF=_tmp1AE->f1;}}_LL62: _tmp1AF=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(
struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1AF);
return Cyc_Toc_add_tuple_type(_tmp1AF);_LL63: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp1B0=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp186;if(_tmp1B0->tag != 12)goto
_LL65;else{_tmp1B1=_tmp1B0->f1;_tmp1B2=_tmp1B0->f2;}}_LL64: {struct Cyc_Absyn_AnonAggrType_struct
_tmpB54;struct Cyc_Absyn_AnonAggrType_struct*_tmpB53;return(void*)((_tmpB53=
_cycalloc(sizeof(*_tmpB53)),((_tmpB53[0]=((_tmpB54.tag=12,((_tmpB54.f1=_tmp1B1,((
_tmpB54.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1B2),_tmpB54)))))),
_tmpB53))));}_LL65: {struct Cyc_Absyn_AggrType_struct*_tmp1B3=(struct Cyc_Absyn_AggrType_struct*)
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
_tmp1F7->tvs,_tmp1B6),_tmp1F9);if(Cyc_Toc_is_abs_tvar(_tmp1FB)){void*_tmp1FC=Cyc_Toc_aggrdecl_type(
_tmp1F7->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp1FC;}{void*_tmp1FD=
Cyc_Toc_add_struct_type(_tmp1F7->name,_tmp1F7->tvs,_tmp1B6,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1F7->impl))->fields);_npop_handler(0);return _tmp1FD;};};
_pop_region(r);}return Cyc_Toc_aggrdecl_type(_tmp1F7->name,Cyc_Absyn_strctq);};};};
_LL67: {struct Cyc_Absyn_EnumType_struct*_tmp1B7=(struct Cyc_Absyn_EnumType_struct*)
_tmp186;if(_tmp1B7->tag != 13)goto _LL69;else{_tmp1B8=_tmp1B7->f1;}}_LL68: return t;
_LL69: {struct Cyc_Absyn_AnonEnumType_struct*_tmp1B9=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp186;if(_tmp1B9->tag != 14)goto _LL6B;else{_tmp1BA=_tmp1B9->f1;}}_LL6A: Cyc_Toc_enumfields_to_c(
_tmp1BA);return t;_LL6B: {struct Cyc_Absyn_TypedefType_struct*_tmp1BB=(struct Cyc_Absyn_TypedefType_struct*)
_tmp186;if(_tmp1BB->tag != 17)goto _LL6D;else{_tmp1BC=_tmp1BB->f1;_tmp1BD=_tmp1BB->f2;
_tmp1BE=_tmp1BB->f3;_tmp1BF=_tmp1BB->f4;}}_LL6C: if(_tmp1BF == 0  || Cyc_noexpand_r){
if(_tmp1BD != 0){struct Cyc_Absyn_TypedefType_struct _tmpB57;struct Cyc_Absyn_TypedefType_struct*
_tmpB56;return(void*)((_tmpB56=_cycalloc(sizeof(*_tmpB56)),((_tmpB56[0]=((
_tmpB57.tag=17,((_tmpB57.f1=_tmp1BC,((_tmpB57.f2=0,((_tmpB57.f3=_tmp1BE,((
_tmpB57.f4=0,_tmpB57)))))))))),_tmpB56))));}else{return t;}}else{struct Cyc_Absyn_TypedefType_struct
_tmpB5D;void**_tmpB5C;struct Cyc_Absyn_TypedefType_struct*_tmpB5B;return(void*)((
_tmpB5B=_cycalloc(sizeof(*_tmpB5B)),((_tmpB5B[0]=((_tmpB5D.tag=17,((_tmpB5D.f1=
_tmp1BC,((_tmpB5D.f2=0,((_tmpB5D.f3=_tmp1BE,((_tmpB5D.f4=((_tmpB5C=_cycalloc(
sizeof(*_tmpB5C)),((_tmpB5C[0]=Cyc_Toc_typ_to_c_array(*_tmp1BF),_tmpB5C)))),
_tmpB5D)))))))))),_tmpB5B))));}_LL6D: {struct Cyc_Absyn_TagType_struct*_tmp1C0=(
struct Cyc_Absyn_TagType_struct*)_tmp186;if(_tmp1C0->tag != 19)goto _LL6F;}_LL6E:
return Cyc_Absyn_uint_typ;_LL6F: {struct Cyc_Absyn_RgnHandleType_struct*_tmp1C1=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp186;if(_tmp1C1->tag != 15)goto _LL71;
else{_tmp1C2=(void*)_tmp1C1->f1;}}_LL70: return Cyc_Toc_rgn_typ();_LL71: {struct
Cyc_Absyn_DynRgnType_struct*_tmp1C3=(struct Cyc_Absyn_DynRgnType_struct*)_tmp186;
if(_tmp1C3->tag != 16)goto _LL73;}_LL72: return Cyc_Toc_dyn_rgn_typ();_LL73: {struct
Cyc_Absyn_HeapRgn_struct*_tmp1C4=(struct Cyc_Absyn_HeapRgn_struct*)_tmp186;if(
_tmp1C4->tag != 20)goto _LL75;}_LL74: {const char*_tmpB60;void*_tmpB5F;(_tmpB5F=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB60="Toc::typ_to_c: type translation passed the heap region",_tag_dyneither(
_tmpB60,sizeof(char),55))),_tag_dyneither(_tmpB5F,sizeof(void*),0)));}_LL75: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp1C5=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp186;if(_tmp1C5->tag != 21)goto _LL77;}_LL76: {const char*_tmpB63;void*_tmpB62;(
_tmpB62=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB63="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpB63,sizeof(char),57))),_tag_dyneither(_tmpB62,sizeof(void*),0)));}
_LL77: {struct Cyc_Absyn_RefCntRgn_struct*_tmp1C6=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp186;if(_tmp1C6->tag != 22)goto _LL79;}_LL78: {const char*_tmpB66;void*_tmpB65;(
_tmpB65=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB66="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpB66,sizeof(char),57))),_tag_dyneither(_tmpB65,sizeof(void*),0)));}
_LL79: {struct Cyc_Absyn_AccessEff_struct*_tmp1C7=(struct Cyc_Absyn_AccessEff_struct*)
_tmp186;if(_tmp1C7->tag != 23)goto _LL7B;}_LL7A: goto _LL7C;_LL7B: {struct Cyc_Absyn_JoinEff_struct*
_tmp1C8=(struct Cyc_Absyn_JoinEff_struct*)_tmp186;if(_tmp1C8->tag != 24)goto _LL7D;}
_LL7C: goto _LL7E;_LL7D: {struct Cyc_Absyn_RgnsEff_struct*_tmp1C9=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp186;if(_tmp1C9->tag != 25)goto _LL7F;}_LL7E: {const char*_tmpB69;void*_tmpB68;(
_tmpB68=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB69="Toc::typ_to_c: type translation passed an effect",_tag_dyneither(
_tmpB69,sizeof(char),49))),_tag_dyneither(_tmpB68,sizeof(void*),0)));}_LL7F: {
struct Cyc_Absyn_ValueofType_struct*_tmp1CA=(struct Cyc_Absyn_ValueofType_struct*)
_tmp186;if(_tmp1CA->tag != 18)goto _LL81;}_LL80: {const char*_tmpB6C;void*_tmpB6B;(
_tmpB6B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB6C="Toc::typ_to_c: type translation passed a valueof_t",_tag_dyneither(
_tmpB6C,sizeof(char),51))),_tag_dyneither(_tmpB6B,sizeof(void*),0)));}_LL81: {
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
_LLC6;}_LLC5: return 0;_LLC6:;_LLC7: {const char*_tmpB70;void*_tmpB6F[1];struct Cyc_String_pa_struct
_tmpB6E;(_tmpB6E.tag=0,((_tmpB6E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpB6F[0]=& _tmpB6E,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB70="atomic_typ:  bad type %s",
_tag_dyneither(_tmpB70,sizeof(char),25))),_tag_dyneither(_tmpB6F,sizeof(void*),1)))))));}
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
if(_tmp242->impl == 0){const char*_tmpB73;void*_tmpB72;(_tmpB72=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB73="is_poly_field: type missing fields",
_tag_dyneither(_tmpB73,sizeof(char),35))),_tag_dyneither(_tmpB72,sizeof(void*),0)));}
_tmp241=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp242->impl))->fields;goto
_LLDB;}_LLDA: {struct Cyc_Absyn_AnonAggrType_struct*_tmp240=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp23C;if(_tmp240->tag != 12)goto _LLDC;else{_tmp241=_tmp240->f2;}}_LLDB: {struct
Cyc_Absyn_Aggrfield*_tmp245=Cyc_Absyn_lookup_field(_tmp241,f);if(_tmp245 == 0){
const char*_tmpB77;void*_tmpB76[1];struct Cyc_String_pa_struct _tmpB75;(_tmpB75.tag=
0,((_tmpB75.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB76[0]=&
_tmpB75,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB77="is_poly_field: bad field %s",_tag_dyneither(_tmpB77,sizeof(char),28))),
_tag_dyneither(_tmpB76,sizeof(void*),1)))))));}return Cyc_Toc_is_void_star_or_boxed_tvar(
_tmp245->type);}_LLDC:;_LLDD: {const char*_tmpB7B;void*_tmpB7A[1];struct Cyc_String_pa_struct
_tmpB79;(_tmpB79.tag=0,((_tmpB79.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpB7A[0]=& _tmpB79,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB7B="is_poly_field: bad type %s",
_tag_dyneither(_tmpB7B,sizeof(char),27))),_tag_dyneither(_tmpB7A,sizeof(void*),1)))))));}
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
_tmpB7F;void*_tmpB7E[1];struct Cyc_String_pa_struct _tmpB7D;(_tmpB7D.tag=0,((
_tmpB7D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp251->topt))->v)),((_tmpB7E[0]=&
_tmpB7D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB7F="is_poly_project: bad type %s",_tag_dyneither(_tmpB7F,sizeof(char),29))),
_tag_dyneither(_tmpB7E,sizeof(void*),1)))))));}_LLE5:;}_LLE3:;_LLE4: return 0;
_LLDE:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB80;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((
_tmpB80=_cycalloc(sizeof(*_tmpB80)),((_tmpB80->hd=s,((_tmpB80->tl=0,_tmpB80)))))),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB81;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((
_tmpB81=_cycalloc(sizeof(*_tmpB81)),((_tmpB81->hd=s,((_tmpB81->tl=0,_tmpB81)))))),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(
s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmpB82[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((
_tmpB82[1]=s,((_tmpB82[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpB82,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,
struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*
_tmpB83[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpB83[1]=n,((
_tmpB83[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB83,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{struct Cyc_Absyn_Exp*
_tmpB84[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpB84[1]=n,((
_tmpB84[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB84,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n){struct Cyc_Absyn_Exp*_tmpB85[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((
_tmpB85[2]=n,((_tmpB85[1]=s,((_tmpB85[0]=rgn,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB85,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmpB86;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmpB86=_cycalloc(sizeof(*_tmpB86)),((_tmpB86->hd=e,((_tmpB86->tl=0,_tmpB86)))))),
0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){struct Cyc_Absyn_Exp*
_tmpB87[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpB87,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
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
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_struct*_tmpB8D;struct
Cyc_Absyn_Var_d_struct _tmpB8C;struct Cyc_List_List*_tmpB8B;Cyc_Toc_result_decls=((
_tmpB8B=_cycalloc(sizeof(*_tmpB8B)),((_tmpB8B->hd=Cyc_Absyn_new_decl((void*)((
_tmpB8D=_cycalloc(sizeof(*_tmpB8D)),((_tmpB8D[0]=((_tmpB8C.tag=0,((_tmpB8C.f1=vd,
_tmpB8C)))),_tmpB8D)))),0),((_tmpB8B->tl=Cyc_Toc_result_decls,_tmpB8B))))));}
xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*urm_exp;{struct _tuple16*_tmpB94;
struct _tuple16*_tmpB93;struct _tuple16*_tmpB92;struct _tuple16*_tmpB91[3];urm_exp=
Cyc_Absyn_unresolvedmem_exp(0,((_tmpB91[2]=((_tmpB92=_cycalloc(sizeof(*_tmpB92)),((
_tmpB92->f1=0,((_tmpB92->f2=xplussz,_tmpB92)))))),((_tmpB91[1]=((_tmpB93=
_cycalloc(sizeof(*_tmpB93)),((_tmpB93->f1=0,((_tmpB93->f2=xexp,_tmpB93)))))),((
_tmpB91[0]=((_tmpB94=_cycalloc(sizeof(*_tmpB94)),((_tmpB94->f1=0,((_tmpB94->f2=
xexp,_tmpB94)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB91,sizeof(struct _tuple16*),3)))))))),0);}return urm_exp;};};}
struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*
binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct
_dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct
_RegionHandle*rgn;};static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env
_tmp271;int _tmp272;struct Cyc_Toc_Env*_tmp270=nv;_tmp271=*_tmp270;_tmp272=_tmp271.toplevel;
return _tmp272;}static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env
_tmp274;int*_tmp275;struct Cyc_Toc_Env*_tmp273=nv;_tmp274=*_tmp273;_tmp275=
_tmp274.in_lhs;return*_tmp275;}static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(
struct Cyc_Toc_Env*nv,struct _tuple0*x){struct Cyc_Toc_Env _tmp277;struct Cyc_Dict_Dict
_tmp278;struct Cyc_Toc_Env*_tmp276=nv;_tmp277=*_tmp276;_tmp278=_tmp277.varmap;
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(
_tmp278,x);}static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){int*
_tmpB97;struct Cyc_Toc_Env*_tmpB96;return(_tmpB96=_region_malloc(r,sizeof(*
_tmpB96)),((_tmpB96->break_lab=(struct _dyneither_ptr**)0,((_tmpB96->continue_lab=(
struct _dyneither_ptr**)0,((_tmpB96->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((
_tmpB96->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((
_tmpB96->toplevel=(int)1,((_tmpB96->in_lhs=(int*)((_tmpB97=_region_malloc(r,
sizeof(*_tmpB97)),((_tmpB97[0]=0,_tmpB97)))),((_tmpB96->rgn=(struct _RegionHandle*)
r,_tmpB96)))))))))))))));}static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp27C;struct
_dyneither_ptr**_tmp27D;struct _dyneither_ptr**_tmp27E;struct Cyc_Toc_FallthruInfo*
_tmp27F;struct Cyc_Dict_Dict _tmp280;int _tmp281;int*_tmp282;struct Cyc_Toc_Env*
_tmp27B=e;_tmp27C=*_tmp27B;_tmp27D=_tmp27C.break_lab;_tmp27E=_tmp27C.continue_lab;
_tmp27F=_tmp27C.fallthru_info;_tmp280=_tmp27C.varmap;_tmp281=_tmp27C.toplevel;
_tmp282=_tmp27C.in_lhs;{struct Cyc_Toc_Env*_tmpB98;return(_tmpB98=_region_malloc(
r,sizeof(*_tmpB98)),((_tmpB98->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp27D),((_tmpB98->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp27E),((_tmpB98->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp27F,((_tmpB98->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp280),((_tmpB98->toplevel=(
int)_tmp281,((_tmpB98->in_lhs=(int*)_tmp282,((_tmpB98->rgn=(struct _RegionHandle*)
r,_tmpB98)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp285;struct
_dyneither_ptr**_tmp286;struct _dyneither_ptr**_tmp287;struct Cyc_Toc_FallthruInfo*
_tmp288;struct Cyc_Dict_Dict _tmp289;int _tmp28A;int*_tmp28B;struct Cyc_Toc_Env*
_tmp284=e;_tmp285=*_tmp284;_tmp286=_tmp285.break_lab;_tmp287=_tmp285.continue_lab;
_tmp288=_tmp285.fallthru_info;_tmp289=_tmp285.varmap;_tmp28A=_tmp285.toplevel;
_tmp28B=_tmp285.in_lhs;{struct Cyc_Toc_Env*_tmpB99;return(_tmpB99=_region_malloc(
r,sizeof(*_tmpB99)),((_tmpB99->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp286),((_tmpB99->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp287),((_tmpB99->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp288,((_tmpB99->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp289),((_tmpB99->toplevel=(
int)0,((_tmpB99->in_lhs=(int*)_tmp28B,((_tmpB99->rgn=(struct _RegionHandle*)r,
_tmpB99)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct
_RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp28E;struct
_dyneither_ptr**_tmp28F;struct _dyneither_ptr**_tmp290;struct Cyc_Toc_FallthruInfo*
_tmp291;struct Cyc_Dict_Dict _tmp292;int _tmp293;int*_tmp294;struct Cyc_Toc_Env*
_tmp28D=e;_tmp28E=*_tmp28D;_tmp28F=_tmp28E.break_lab;_tmp290=_tmp28E.continue_lab;
_tmp291=_tmp28E.fallthru_info;_tmp292=_tmp28E.varmap;_tmp293=_tmp28E.toplevel;
_tmp294=_tmp28E.in_lhs;{struct Cyc_Toc_Env*_tmpB9A;return(_tmpB9A=_region_malloc(
r,sizeof(*_tmpB9A)),((_tmpB9A->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp28F),((_tmpB9A->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp290),((_tmpB9A->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp291,((_tmpB9A->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp292),((_tmpB9A->toplevel=(
int)1,((_tmpB9A->in_lhs=(int*)_tmp294,((_tmpB9A->rgn=(struct _RegionHandle*)r,
_tmpB9A)))))))))))))));};}static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env _tmp297;int*_tmp298;struct Cyc_Toc_Env*_tmp296=e;_tmp297=*
_tmp296;_tmp298=_tmp297.in_lhs;*_tmp298=b;}static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace _tmp299=(*x).f1;struct Cyc_List_List*_tmp29A;_LLF2: if((
_tmp299.Rel_n).tag != 1)goto _LLF4;_tmp29A=(struct Cyc_List_List*)(_tmp299.Rel_n).val;
_LLF3: {const char*_tmpB9E;void*_tmpB9D[1];struct Cyc_String_pa_struct _tmpB9C;(
_tmpB9C.tag=0,((_tmpB9C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
x)),((_tmpB9D[0]=& _tmpB9C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB9E="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(
_tmpB9E,sizeof(char),30))),_tag_dyneither(_tmpB9D,sizeof(void*),1)))))));}_LLF4:;
_LLF5: goto _LLF1;_LLF1:;}{struct Cyc_Toc_Env _tmp29F;struct _dyneither_ptr**_tmp2A0;
struct _dyneither_ptr**_tmp2A1;struct Cyc_Toc_FallthruInfo*_tmp2A2;struct Cyc_Dict_Dict
_tmp2A3;int _tmp2A4;int*_tmp2A5;struct Cyc_Toc_Env*_tmp29E=e;_tmp29F=*_tmp29E;
_tmp2A0=_tmp29F.break_lab;_tmp2A1=_tmp29F.continue_lab;_tmp2A2=_tmp29F.fallthru_info;
_tmp2A3=_tmp29F.varmap;_tmp2A4=_tmp29F.toplevel;_tmp2A5=_tmp29F.in_lhs;{struct
Cyc_Dict_Dict _tmp2A6=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*
k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2A3),x,y);struct Cyc_Toc_Env*
_tmpB9F;return(_tmpB9F=_region_malloc(r,sizeof(*_tmpB9F)),((_tmpB9F->break_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp2A0),((_tmpB9F->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp2A1),((_tmpB9F->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp2A2,((_tmpB9F->varmap=(struct Cyc_Dict_Dict)
_tmp2A6,((_tmpB9F->toplevel=(int)_tmp2A4,((_tmpB9F->in_lhs=(int*)_tmp2A5,((
_tmpB9F->rgn=(struct _RegionHandle*)r,_tmpB9F)))))))))))))));};};}static struct Cyc_Toc_Env*
Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp2A9;struct _dyneither_ptr**_tmp2AA;struct _dyneither_ptr**_tmp2AB;struct Cyc_Toc_FallthruInfo*
_tmp2AC;struct Cyc_Dict_Dict _tmp2AD;int _tmp2AE;int*_tmp2AF;struct Cyc_Toc_Env*
_tmp2A8=e;_tmp2A9=*_tmp2A8;_tmp2AA=_tmp2A9.break_lab;_tmp2AB=_tmp2A9.continue_lab;
_tmp2AC=_tmp2A9.fallthru_info;_tmp2AD=_tmp2A9.varmap;_tmp2AE=_tmp2A9.toplevel;
_tmp2AF=_tmp2A9.in_lhs;{struct Cyc_Toc_Env*_tmpBA0;return(_tmpBA0=_region_malloc(
r,sizeof(*_tmpBA0)),((_tmpBA0->break_lab=(struct _dyneither_ptr**)0,((_tmpBA0->continue_lab=(
struct _dyneither_ptr**)0,((_tmpBA0->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp2AC,((_tmpBA0->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2AD),((_tmpBA0->toplevel=(
int)_tmp2AE,((_tmpBA0->in_lhs=(int*)_tmp2AF,((_tmpBA0->rgn=(struct _RegionHandle*)
r,_tmpBA0)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmpBA1;fallthru_vars=((
_tmpBA1=_region_malloc(r,sizeof(*_tmpBA1)),((_tmpBA1->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmpBA1->tl=fallthru_vars,_tmpBA1))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp2B3;struct _dyneither_ptr**_tmp2B4;struct
_dyneither_ptr**_tmp2B5;struct Cyc_Toc_FallthruInfo*_tmp2B6;struct Cyc_Dict_Dict
_tmp2B7;int _tmp2B8;int*_tmp2B9;struct Cyc_Toc_Env*_tmp2B2=e;_tmp2B3=*_tmp2B2;
_tmp2B4=_tmp2B3.break_lab;_tmp2B5=_tmp2B3.continue_lab;_tmp2B6=_tmp2B3.fallthru_info;
_tmp2B7=_tmp2B3.varmap;_tmp2B8=_tmp2B3.toplevel;_tmp2B9=_tmp2B3.in_lhs;{struct
Cyc_Toc_Env _tmp2BB;struct Cyc_Dict_Dict _tmp2BC;struct Cyc_Toc_Env*_tmp2BA=
next_case_env;_tmp2BB=*_tmp2BA;_tmp2BC=_tmp2BB.varmap;{struct Cyc_Toc_FallthruInfo*
_tmpBA2;struct Cyc_Toc_FallthruInfo*fi=(_tmpBA2=_region_malloc(r,sizeof(*_tmpBA2)),((
_tmpBA2->label=fallthru_l,((_tmpBA2->binders=fallthru_vars,((_tmpBA2->next_case_env=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp2BC),_tmpBA2)))))));struct _dyneither_ptr**_tmpBA5;struct Cyc_Toc_Env*
_tmpBA4;return(_tmpBA4=_region_malloc(r,sizeof(*_tmpBA4)),((_tmpBA4->break_lab=(
struct _dyneither_ptr**)((_tmpBA5=_region_malloc(r,sizeof(*_tmpBA5)),((_tmpBA5[0]=
break_l,_tmpBA5)))),((_tmpBA4->continue_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp2B5),((_tmpBA4->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
fi,((_tmpBA4->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2B7),((_tmpBA4->toplevel=(
int)_tmp2B8,((_tmpBA4->in_lhs=(int*)((int*)_tmp2B9),((_tmpBA4->rgn=(struct
_RegionHandle*)r,_tmpBA4)))))))))))))));};};};}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){struct
Cyc_Toc_Env _tmp2C1;struct _dyneither_ptr**_tmp2C2;struct _dyneither_ptr**_tmp2C3;
struct Cyc_Toc_FallthruInfo*_tmp2C4;struct Cyc_Dict_Dict _tmp2C5;int _tmp2C6;int*
_tmp2C7;struct Cyc_Toc_Env*_tmp2C0=e;_tmp2C1=*_tmp2C0;_tmp2C2=_tmp2C1.break_lab;
_tmp2C3=_tmp2C1.continue_lab;_tmp2C4=_tmp2C1.fallthru_info;_tmp2C5=_tmp2C1.varmap;
_tmp2C6=_tmp2C1.toplevel;_tmp2C7=_tmp2C1.in_lhs;{struct _dyneither_ptr**_tmpBA8;
struct Cyc_Toc_Env*_tmpBA7;return(_tmpBA7=_region_malloc(r,sizeof(*_tmpBA7)),((
_tmpBA7->break_lab=(struct _dyneither_ptr**)((_tmpBA8=_region_malloc(r,sizeof(*
_tmpBA8)),((_tmpBA8[0]=break_l,_tmpBA8)))),((_tmpBA7->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp2C3),((_tmpBA7->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpBA7->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2C5),((_tmpBA7->toplevel=(
int)_tmp2C6,((_tmpBA7->in_lhs=(int*)_tmp2C7,((_tmpBA7->rgn=(struct _RegionHandle*)
r,_tmpBA7)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp2CB;struct _dyneither_ptr**_tmp2CC;struct _dyneither_ptr**_tmp2CD;struct Cyc_Toc_FallthruInfo*
_tmp2CE;struct Cyc_Dict_Dict _tmp2CF;int _tmp2D0;int*_tmp2D1;struct Cyc_Toc_Env*
_tmp2CA=e;_tmp2CB=*_tmp2CA;_tmp2CC=_tmp2CB.break_lab;_tmp2CD=_tmp2CB.continue_lab;
_tmp2CE=_tmp2CB.fallthru_info;_tmp2CF=_tmp2CB.varmap;_tmp2D0=_tmp2CB.toplevel;
_tmp2D1=_tmp2CB.in_lhs;{struct Cyc_Toc_FallthruInfo*_tmpBAB;struct Cyc_Toc_Env*
_tmpBAA;return(_tmpBAA=_region_malloc(r,sizeof(*_tmpBAA)),((_tmpBAA->break_lab=(
struct _dyneither_ptr**)0,((_tmpBAA->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp2CD),((_tmpBAA->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((
_tmpBAB=_region_malloc(r,sizeof(*_tmpBAB)),((_tmpBAB->label=next_l,((_tmpBAB->binders=
0,((_tmpBAB->next_case_env=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*
cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),
_tmpBAB)))))))),((_tmpBAA->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2CF),((_tmpBAA->toplevel=(
int)_tmp2D0,((_tmpBAA->in_lhs=(int*)((int*)_tmp2D1),((_tmpBAA->rgn=(struct
_RegionHandle*)r,_tmpBAA)))))))))))))));};}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp2D4=e->annot;_LLF7: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2D5=(
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp2D4;if(_tmp2D5->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLF9;}_LLF8: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLF9: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp2D6=(
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp2D4;if(_tmp2D6->tag != Cyc_CfFlowInfo_NotZero)
goto _LLFB;}_LLFA: return 0;_LLFB: {struct Cyc_CfFlowInfo_IsZero_struct*_tmp2D7=(
struct Cyc_CfFlowInfo_IsZero_struct*)_tmp2D4;if(_tmp2D7->tag != Cyc_CfFlowInfo_IsZero)
goto _LLFD;}_LLFC:{const char*_tmpBAE;void*_tmpBAD;(_tmpBAD=0,Cyc_Tcutil_terr(e->loc,((
_tmpBAE="dereference of NULL pointer",_tag_dyneither(_tmpBAE,sizeof(char),28))),
_tag_dyneither(_tmpBAD,sizeof(void*),0)));}return 0;_LLFD: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp2D8=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp2D4;if(_tmp2D8->tag != Cyc_Absyn_EmptyAnnot)
goto _LLFF;}_LLFE: return 0;_LLFF:;_LL100: {const char*_tmpBB1;void*_tmpBB0;(_tmpBB0=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBB1="need_null_check",_tag_dyneither(_tmpBB1,sizeof(char),16))),
_tag_dyneither(_tmpBB0,sizeof(void*),0)));}_LLF6:;}static struct Cyc_List_List*Cyc_Toc_get_relns(
struct Cyc_Absyn_Exp*e){void*_tmp2DD=e->annot;struct Cyc_List_List*_tmp2DF;struct
Cyc_List_List*_tmp2E1;_LL102: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2DE=(
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp2DD;if(_tmp2DE->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LL104;else{_tmp2DF=_tmp2DE->f1;}}_LL103: return _tmp2DF;_LL104: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmp2E0=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmp2DD;if(_tmp2E0->tag != Cyc_CfFlowInfo_NotZero)
goto _LL106;else{_tmp2E1=_tmp2E0->f1;}}_LL105: return _tmp2E1;_LL106: {struct Cyc_CfFlowInfo_IsZero_struct*
_tmp2E2=(struct Cyc_CfFlowInfo_IsZero_struct*)_tmp2DD;if(_tmp2E2->tag != Cyc_CfFlowInfo_IsZero)
goto _LL108;}_LL107:{const char*_tmpBB4;void*_tmpBB3;(_tmpBB3=0,Cyc_Tcutil_terr(e->loc,((
_tmpBB4="dereference of NULL pointer",_tag_dyneither(_tmpBB4,sizeof(char),28))),
_tag_dyneither(_tmpBB3,sizeof(void*),0)));}return 0;_LL108: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp2E3=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp2DD;if(_tmp2E3->tag != Cyc_Absyn_EmptyAnnot)
goto _LL10A;}_LL109: return 0;_LL10A:;_LL10B: {const char*_tmpBB7;void*_tmpBB6;(
_tmpBB6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBB7="get_relns",_tag_dyneither(_tmpBB7,sizeof(char),10))),_tag_dyneither(
_tmpBB6,sizeof(void*),0)));}_LL101:;}static int Cyc_Toc_check_const_array(
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
struct _tuple19 _tmpBB8;struct _tuple19 _tmp37D=(_tmpBB8.f1=Cyc_Tcutil_compress(
_tmp379),((_tmpBB8.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(a->topt))->v),_tmpBB8)));void*_tmp37E;void*_tmp380;void*_tmp381;
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
e){if(e->topt == 0){const char*_tmpBBB;void*_tmpBBA;(_tmpBBA=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBBB="Missing type in primop ",
_tag_dyneither(_tmpBBB,sizeof(char),24))),_tag_dyneither(_tmpBBA,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*
_tmpBBE;void*_tmpBBD;(_tmpBBD=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBBE="Missing type in primop ",
_tag_dyneither(_tmpBBE,sizeof(char),24))),_tag_dyneither(_tmpBBD,sizeof(void*),0)));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct
_tuple10*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){struct _tuple10*_tmpBBF;return(
_tmpBBF=_cycalloc(sizeof(*_tmpBBF)),((_tmpBBF->f1=Cyc_Toc_mt_tq,((_tmpBBF->f2=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmpBBF)))));}
static struct _tuple16*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){Cyc_Toc_exp_to_c(nv,e);{struct _tuple16*_tmpBC0;return(_tmpBC0=_cycalloc(
sizeof(*_tmpBC0)),((_tmpBC0->f1=0,((_tmpBC0->f2=e,_tmpBC0)))));};}static struct
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
_tmp39D->tl != 0){const char*_tmpBC3;void*_tmpBC2;(_tmpBC2=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBC3="multiple designators in array",
_tag_dyneither(_tmpBC3,sizeof(char),30))),_tag_dyneither(_tmpBC2,sizeof(void*),0)));}{
void*_tmp3A1=(void*)_tmp39D->hd;void*_tmp3A2=_tmp3A1;struct Cyc_Absyn_Exp*_tmp3A4;
_LL180: {struct Cyc_Absyn_ArrayElement_struct*_tmp3A3=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp3A2;if(_tmp3A3->tag != 0)goto _LL182;else{_tmp3A4=_tmp3A3->f1;}}_LL181: Cyc_Toc_exp_to_c(
nv,_tmp3A4);e_index=_tmp3A4;goto _LL17F;_LL182: {struct Cyc_Absyn_FieldName_struct*
_tmp3A5=(struct Cyc_Absyn_FieldName_struct*)_tmp3A2;if(_tmp3A5->tag != 1)goto
_LL17F;}_LL183: {const char*_tmpBC6;void*_tmpBC5;(_tmpBC5=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBC6="field name designators in array",
_tag_dyneither(_tmpBC6,sizeof(char),32))),_tag_dyneither(_tmpBC5,sizeof(void*),0)));}
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
_tmpBC9;struct Cyc_Absyn_Local_b_struct*_tmpBC8;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
_tmp3B7,nv,_tmp3B3,Cyc_Absyn_varb_exp(_tmp3B3,(void*)((_tmpBC8=_cycalloc(sizeof(*
_tmpBC8)),((_tmpBC8[0]=((_tmpBC9.tag=4,((_tmpBC9.f1=vd,_tmpBC9)))),_tmpBC8)))),0));
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
if(_tmp3C8 == 0){const char*_tmpBCC;void*_tmpBCB;(_tmpBCB=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBCC="empty designator list",
_tag_dyneither(_tmpBCC,sizeof(char),22))),_tag_dyneither(_tmpBCB,sizeof(void*),0)));}
if(_tmp3C8->tl != 0){const char*_tmpBCF;void*_tmpBCE;(_tmpBCE=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBCF="too many designators in anonymous struct",
_tag_dyneither(_tmpBCF,sizeof(char),41))),_tag_dyneither(_tmpBCE,sizeof(void*),0)));}{
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
_tmpBD2;void*_tmpBD1;(_tmpBD1=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD2="array designator in struct",
_tag_dyneither(_tmpBD2,sizeof(char),27))),_tag_dyneither(_tmpBD1,sizeof(void*),0)));}
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
struct Cyc_String_pa_struct _tmpBDA;void*_tmpBD9[1];const char*_tmpBD8;void*_tmpBD7;(
_tmpBD7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)((_tmpBDA.tag=0,((_tmpBDA.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmpBD9[0]=& _tmpBDA,Cyc_aprintf(((_tmpBD8="get_member_offset %s failed",
_tag_dyneither(_tmpBD8,sizeof(char),28))),_tag_dyneither(_tmpBD9,sizeof(void*),1)))))))),
_tag_dyneither(_tmpBD7,sizeof(void*),0)));};}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){struct _tuple0*_tmp3F6=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3F7=Cyc_Absyn_var_exp(_tmp3F6,0);struct Cyc_Absyn_Stmt*
_tmp3F8=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp3F7),0);struct Cyc_Absyn_Exp*(*
_tmp3F9)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*)=
pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;void*_tmp3FA=Cyc_Toc_typ_to_c(
struct_type);int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp3FB=Cyc_Tcutil_compress(
struct_type);struct Cyc_Absyn_AggrInfo _tmp3FD;union Cyc_Absyn_AggrInfoU _tmp3FE;
_LL1AC: {struct Cyc_Absyn_AggrType_struct*_tmp3FC=(struct Cyc_Absyn_AggrType_struct*)
_tmp3FB;if(_tmp3FC->tag != 11)goto _LL1AE;else{_tmp3FD=_tmp3FC->f1;_tmp3FE=_tmp3FD.aggr_info;}}
_LL1AD: ad=Cyc_Absyn_get_known_aggrdecl(_tmp3FE);goto _LL1AB;_LL1AE:;_LL1AF: {
const char*_tmpBDD;void*_tmpBDC;(_tmpBDC=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBDD="init_struct: bad struct type",
_tag_dyneither(_tmpBDD,sizeof(char),29))),_tag_dyneither(_tmpBDC,sizeof(void*),0)));}
_LL1AB:;}{int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct Cyc_Toc_Env _tmp402;struct _RegionHandle*_tmp403;struct Cyc_Toc_Env*_tmp401=
nv;_tmp402=*_tmp401;_tmp403=_tmp402.rgn;{struct Cyc_List_List*_tmp404=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp403,dles);for(0;
_tmp404 != 0;_tmp404=_tmp404->tl){struct _tuple16 _tmp406;struct Cyc_List_List*
_tmp407;struct Cyc_Absyn_Exp*_tmp408;struct _tuple16*_tmp405=(struct _tuple16*)
_tmp404->hd;_tmp406=*_tmp405;_tmp407=_tmp406.f1;_tmp408=_tmp406.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp408->topt))->v);
if(_tmp407 == 0){const char*_tmpBE0;void*_tmpBDF;(_tmpBDF=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE0="empty designator list",
_tag_dyneither(_tmpBE0,sizeof(char),22))),_tag_dyneither(_tmpBDF,sizeof(void*),0)));}
if(_tmp407->tl != 0){struct _tuple0*_tmp40B=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp40C=Cyc_Absyn_var_exp(_tmp40B,0);for(0;_tmp407 != 0;_tmp407=_tmp407->tl){void*
_tmp40D=(void*)_tmp407->hd;struct _dyneither_ptr*_tmp40F;_LL1B1: {struct Cyc_Absyn_FieldName_struct*
_tmp40E=(struct Cyc_Absyn_FieldName_struct*)_tmp40D;if(_tmp40E->tag != 1)goto
_LL1B3;else{_tmp40F=_tmp40E->f1;}}_LL1B2: if(Cyc_Toc_is_poly_field(struct_type,
_tmp40F))_tmp40C=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp40C);_tmp3F8=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3F9(_tmp3F7,_tmp40F,0),_tmp40C,0),0),
_tmp3F8,0);goto _LL1B0;_LL1B3:;_LL1B4: {const char*_tmpBE3;void*_tmpBE2;(_tmpBE2=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBE3="array designator in struct",_tag_dyneither(_tmpBE3,sizeof(char),27))),
_tag_dyneither(_tmpBE2,sizeof(void*),0)));}_LL1B0:;}Cyc_Toc_exp_to_c(nv,_tmp408);
_tmp3F8=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp40C,
_tmp408,0),0),_tmp3F8,0);}else{void*_tmp412=(void*)_tmp407->hd;struct
_dyneither_ptr*_tmp414;_LL1B6: {struct Cyc_Absyn_FieldName_struct*_tmp413=(struct
Cyc_Absyn_FieldName_struct*)_tmp412;if(_tmp413->tag != 1)goto _LL1B8;else{_tmp414=
_tmp413->f1;}}_LL1B7: {struct Cyc_Absyn_Exp*lval=_tmp3F9(_tmp3F7,_tmp414,0);if(
is_tagged_union){int i=Cyc_Toc_get_member_offset(ad,_tmp414);struct Cyc_Absyn_Exp*
f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_aggrmember_exp(
lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,
f_tag_exp,0);_tmp3F8=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3F8,0);
lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{void*_tmp415=_tmp408->r;
struct Cyc_List_List*_tmp417;struct Cyc_Absyn_Vardecl*_tmp419;struct Cyc_Absyn_Exp*
_tmp41A;struct Cyc_Absyn_Exp*_tmp41B;int _tmp41C;void*_tmp41E;struct Cyc_List_List*
_tmp41F;_LL1BB: {struct Cyc_Absyn_Array_e_struct*_tmp416=(struct Cyc_Absyn_Array_e_struct*)
_tmp415;if(_tmp416->tag != 27)goto _LL1BD;else{_tmp417=_tmp416->f1;}}_LL1BC:
_tmp3F8=Cyc_Toc_init_array(nv,lval,_tmp417,_tmp3F8);goto _LL1BA;_LL1BD: {struct
Cyc_Absyn_Comprehension_e_struct*_tmp418=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp415;if(_tmp418->tag != 28)goto _LL1BF;else{_tmp419=_tmp418->f1;_tmp41A=_tmp418->f2;
_tmp41B=_tmp418->f3;_tmp41C=_tmp418->f4;}}_LL1BE: _tmp3F8=Cyc_Toc_init_comprehension(
nv,lval,_tmp419,_tmp41A,_tmp41B,_tmp41C,_tmp3F8,0);goto _LL1BA;_LL1BF: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp41D=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp415;if(_tmp41D->tag != 30)goto
_LL1C1;else{_tmp41E=(void*)_tmp41D->f1;_tmp41F=_tmp41D->f2;}}_LL1C0: _tmp3F8=Cyc_Toc_init_anon_struct(
nv,lval,_tmp41E,_tmp41F,_tmp3F8);goto _LL1BA;_LL1C1:;_LL1C2: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp408->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp408);{struct Cyc_Absyn_Aggrfield*_tmp420=Cyc_Absyn_lookup_decl_field(ad,
_tmp414);if(Cyc_Toc_is_poly_field(struct_type,_tmp414) && !was_ptr_type)_tmp408=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp408);if(has_exists)_tmp408=Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp420))->type),
_tmp408);_tmp3F8=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp408,0),0),_tmp3F8,0);goto _LL1BA;};}_LL1BA:;}goto _LL1B5;}_LL1B8:;_LL1B9: {
const char*_tmpBE6;void*_tmpBE5;(_tmpBE5=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE6="array designator in struct",
_tag_dyneither(_tmpBE6,sizeof(char),27))),_tag_dyneither(_tmpBE5,sizeof(void*),0)));}
_LL1B5:;}}}return Cyc_Toc_make_struct(nv,_tmp3F6,_tmp3FA,_tmp3F8,pointer,rgnopt,
is_atomic);};}struct _tuple20{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct
_tuple20*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(
e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,
enum Cyc_Absyn_Incrementor incr){struct Cyc_Absyn_Increment_e_struct _tmpBE9;struct
Cyc_Absyn_Increment_e_struct*_tmpBE8;return Cyc_Absyn_new_exp((void*)((_tmpBE8=
_cycalloc(sizeof(*_tmpBE8)),((_tmpBE8[0]=((_tmpBE9.tag=5,((_tmpBE9.f1=e1,((
_tmpBE9.f2=incr,_tmpBE9)))))),_tmpBE8)))),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp425=e1->r;struct Cyc_Absyn_Stmt*_tmp427;void*_tmp429;struct Cyc_Absyn_Exp*
_tmp42A;struct Cyc_Absyn_Exp*_tmp42C;struct _dyneither_ptr*_tmp42D;int _tmp42E;int
_tmp42F;_LL1C4: {struct Cyc_Absyn_StmtExp_e_struct*_tmp426=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp425;if(_tmp426->tag != 37)goto _LL1C6;else{_tmp427=_tmp426->f1;}}_LL1C5: Cyc_Toc_lvalue_assign_stmt(
_tmp427,fs,f,f_env);goto _LL1C3;_LL1C6: {struct Cyc_Absyn_Cast_e_struct*_tmp428=(
struct Cyc_Absyn_Cast_e_struct*)_tmp425;if(_tmp428->tag != 15)goto _LL1C8;else{
_tmp429=(void*)_tmp428->f1;_tmp42A=_tmp428->f2;}}_LL1C7: Cyc_Toc_lvalue_assign(
_tmp42A,fs,f,f_env);goto _LL1C3;_LL1C8: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp42B=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp425;if(_tmp42B->tag != 22)goto
_LL1CA;else{_tmp42C=_tmp42B->f1;_tmp42D=_tmp42B->f2;_tmp42E=_tmp42B->f3;_tmp42F=
_tmp42B->f4;}}_LL1C9: e1->r=_tmp42C->r;{struct Cyc_List_List*_tmpBEA;Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)((_tmpBEA=_cycalloc(sizeof(*_tmpBEA)),((_tmpBEA->hd=
_tmp42D,((_tmpBEA->tl=fs,_tmpBEA)))))),f,f_env);}goto _LL1C3;_LL1CA:;_LL1CB: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;goto _LL1C3;}_LL1C3:;}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env){void*_tmp431=s->r;struct Cyc_Absyn_Exp*_tmp433;struct Cyc_Absyn_Decl*
_tmp435;struct Cyc_Absyn_Stmt*_tmp436;struct Cyc_Absyn_Stmt*_tmp438;_LL1CD: {
struct Cyc_Absyn_Exp_s_struct*_tmp432=(struct Cyc_Absyn_Exp_s_struct*)_tmp431;if(
_tmp432->tag != 1)goto _LL1CF;else{_tmp433=_tmp432->f1;}}_LL1CE: Cyc_Toc_lvalue_assign(
_tmp433,fs,f,f_env);goto _LL1CC;_LL1CF: {struct Cyc_Absyn_Decl_s_struct*_tmp434=(
struct Cyc_Absyn_Decl_s_struct*)_tmp431;if(_tmp434->tag != 12)goto _LL1D1;else{
_tmp435=_tmp434->f1;_tmp436=_tmp434->f2;}}_LL1D0: Cyc_Toc_lvalue_assign_stmt(
_tmp436,fs,f,f_env);goto _LL1CC;_LL1D1: {struct Cyc_Absyn_Seq_s_struct*_tmp437=(
struct Cyc_Absyn_Seq_s_struct*)_tmp431;if(_tmp437->tag != 2)goto _LL1D3;else{
_tmp438=_tmp437->f2;}}_LL1D2: Cyc_Toc_lvalue_assign_stmt(_tmp438,fs,f,f_env);goto
_LL1CC;_LL1D3:;_LL1D4: {const char*_tmpBEE;void*_tmpBED[1];struct Cyc_String_pa_struct
_tmpBEC;(_tmpBEC.tag=0,((_tmpBEC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmpBED[0]=& _tmpBEC,Cyc_Toc_toc_impos(((_tmpBEE="lvalue_assign_stmt: %s",
_tag_dyneither(_tmpBEE,sizeof(char),23))),_tag_dyneither(_tmpBED,sizeof(void*),1)))))));}
_LL1CC:;}static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);static
struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){void*_tmp43C=
e->r;void*_tmp43E;void**_tmp43F;struct Cyc_Absyn_Exp*_tmp440;struct Cyc_Absyn_Exp**
_tmp441;struct Cyc_Absyn_Exp*_tmp443;struct Cyc_Absyn_Stmt*_tmp445;_LL1D6: {struct
Cyc_Absyn_Cast_e_struct*_tmp43D=(struct Cyc_Absyn_Cast_e_struct*)_tmp43C;if(
_tmp43D->tag != 15)goto _LL1D8;else{_tmp43E=(void**)& _tmp43D->f1;_tmp43F=(void**)((
void**)& _tmp43D->f1);_tmp440=_tmp43D->f2;_tmp441=(struct Cyc_Absyn_Exp**)& _tmp43D->f2;}}
_LL1D7:*_tmp441=Cyc_Toc_push_address_exp(*_tmp441);*_tmp43F=Cyc_Absyn_cstar_typ(*
_tmp43F,Cyc_Toc_mt_tq);return e;_LL1D8: {struct Cyc_Absyn_Deref_e_struct*_tmp442=(
struct Cyc_Absyn_Deref_e_struct*)_tmp43C;if(_tmp442->tag != 21)goto _LL1DA;else{
_tmp443=_tmp442->f1;}}_LL1D9: return _tmp443;_LL1DA: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp444=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp43C;if(_tmp444->tag != 37)goto
_LL1DC;else{_tmp445=_tmp444->f1;}}_LL1DB: Cyc_Toc_push_address_stmt(_tmp445);
return e;_LL1DC:;_LL1DD: if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpBF2;void*_tmpBF1[1];struct Cyc_String_pa_struct _tmpBF0;(_tmpBF0.tag=
0,((_tmpBF0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpBF1[0]=& _tmpBF0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpBF2="can't take & of exp %s",_tag_dyneither(_tmpBF2,
sizeof(char),23))),_tag_dyneither(_tmpBF1,sizeof(void*),1)))))));};_LL1D5:;}
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){void*_tmp449=s->r;
struct Cyc_Absyn_Stmt*_tmp44B;struct Cyc_Absyn_Stmt*_tmp44D;struct Cyc_Absyn_Exp*
_tmp44F;struct Cyc_Absyn_Exp**_tmp450;_LL1DF: {struct Cyc_Absyn_Seq_s_struct*
_tmp44A=(struct Cyc_Absyn_Seq_s_struct*)_tmp449;if(_tmp44A->tag != 2)goto _LL1E1;
else{_tmp44B=_tmp44A->f2;}}_LL1E0: _tmp44D=_tmp44B;goto _LL1E2;_LL1E1: {struct Cyc_Absyn_Decl_s_struct*
_tmp44C=(struct Cyc_Absyn_Decl_s_struct*)_tmp449;if(_tmp44C->tag != 12)goto _LL1E3;
else{_tmp44D=_tmp44C->f2;}}_LL1E2: Cyc_Toc_push_address_stmt(_tmp44D);goto _LL1DE;
_LL1E3: {struct Cyc_Absyn_Exp_s_struct*_tmp44E=(struct Cyc_Absyn_Exp_s_struct*)
_tmp449;if(_tmp44E->tag != 1)goto _LL1E5;else{_tmp44F=_tmp44E->f1;_tmp450=(struct
Cyc_Absyn_Exp**)& _tmp44E->f1;}}_LL1E4:*_tmp450=Cyc_Toc_push_address_exp(*_tmp450);
goto _LL1DE;_LL1E5:;_LL1E6: {const char*_tmpBF6;void*_tmpBF5[1];struct Cyc_String_pa_struct
_tmpBF4;(_tmpBF4.tag=0,((_tmpBF4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmpBF5[0]=& _tmpBF4,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF6="can't take & of stmt %s",
_tag_dyneither(_tmpBF6,sizeof(char),24))),_tag_dyneither(_tmpBF5,sizeof(void*),1)))))));}
_LL1DE:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;
struct Cyc_List_List*prev;if(x == 0)return 0;{struct Cyc_List_List*_tmpBF7;result=((
_tmpBF7=_region_malloc(r2,sizeof(*_tmpBF7)),((_tmpBF7->hd=(void*)f((void*)x->hd,
env),((_tmpBF7->tl=0,_tmpBF7))))));}prev=result;for(x=x->tl;x != 0;x=x->tl){{
struct Cyc_List_List*_tmpBF8;((struct Cyc_List_List*)_check_null(prev))->tl=((
_tmpBF8=_region_malloc(r2,sizeof(*_tmpBF8)),((_tmpBF8->hd=(void*)f((void*)x->hd,
env),((_tmpBF8->tl=0,_tmpBF8))))));}prev=prev->tl;}return result;}static struct Cyc_List_List*
Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region,f,env,x);}static struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*
e){struct _tuple16*_tmpBF9;return(_tmpBF9=_cycalloc(sizeof(*_tmpBF9)),((_tmpBF9->f1=
0,((_tmpBF9->f2=e,_tmpBF9)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(
void*t){void*_tmp457=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp459;
_LL1E8: {struct Cyc_Absyn_PointerType_struct*_tmp458=(struct Cyc_Absyn_PointerType_struct*)
_tmp457;if(_tmp458->tag != 5)goto _LL1EA;else{_tmp459=_tmp458->f1;}}_LL1E9: return
_tmp459;_LL1EA:;_LL1EB: {const char*_tmpBFC;void*_tmpBFB;(_tmpBFB=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBFC="get_ptr_typ: not a pointer!",
_tag_dyneither(_tmpBFC,sizeof(char),28))),_tag_dyneither(_tmpBFB,sizeof(void*),0)));}
_LL1E7:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp45C=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp45F;enum Cyc_Absyn_Size_of
_tmp460;enum Cyc_Absyn_Sign _tmp462;enum Cyc_Absyn_Size_of _tmp463;enum Cyc_Absyn_Sign
_tmp467;enum Cyc_Absyn_Size_of _tmp468;enum Cyc_Absyn_Sign _tmp46A;enum Cyc_Absyn_Size_of
_tmp46B;enum Cyc_Absyn_Sign _tmp46D;enum Cyc_Absyn_Size_of _tmp46E;int _tmp470;int
_tmp472;int _tmp474;_LL1ED: {struct Cyc_Absyn_PointerType_struct*_tmp45D=(struct
Cyc_Absyn_PointerType_struct*)_tmp45C;if(_tmp45D->tag != 5)goto _LL1EF;}_LL1EE: res=
Cyc_Absyn_null_exp(0);goto _LL1EC;_LL1EF: {struct Cyc_Absyn_IntType_struct*_tmp45E=(
struct Cyc_Absyn_IntType_struct*)_tmp45C;if(_tmp45E->tag != 6)goto _LL1F1;else{
_tmp45F=_tmp45E->f1;_tmp460=_tmp45E->f2;if(_tmp460 != Cyc_Absyn_Char_sz)goto
_LL1F1;}}_LL1F0: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp45F,'\000'),0);goto
_LL1EC;_LL1F1: {struct Cyc_Absyn_IntType_struct*_tmp461=(struct Cyc_Absyn_IntType_struct*)
_tmp45C;if(_tmp461->tag != 6)goto _LL1F3;else{_tmp462=_tmp461->f1;_tmp463=_tmp461->f2;
if(_tmp463 != Cyc_Absyn_Short_sz)goto _LL1F3;}}_LL1F2: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(
_tmp462,0),0);goto _LL1EC;_LL1F3: {struct Cyc_Absyn_EnumType_struct*_tmp464=(
struct Cyc_Absyn_EnumType_struct*)_tmp45C;if(_tmp464->tag != 13)goto _LL1F5;}_LL1F4:
goto _LL1F6;_LL1F5: {struct Cyc_Absyn_AnonEnumType_struct*_tmp465=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp45C;if(_tmp465->tag != 14)goto _LL1F7;}_LL1F6: _tmp467=Cyc_Absyn_Unsigned;goto
_LL1F8;_LL1F7: {struct Cyc_Absyn_IntType_struct*_tmp466=(struct Cyc_Absyn_IntType_struct*)
_tmp45C;if(_tmp466->tag != 6)goto _LL1F9;else{_tmp467=_tmp466->f1;_tmp468=_tmp466->f2;
if(_tmp468 != Cyc_Absyn_Int_sz)goto _LL1F9;}}_LL1F8: _tmp46A=_tmp467;goto _LL1FA;
_LL1F9: {struct Cyc_Absyn_IntType_struct*_tmp469=(struct Cyc_Absyn_IntType_struct*)
_tmp45C;if(_tmp469->tag != 6)goto _LL1FB;else{_tmp46A=_tmp469->f1;_tmp46B=_tmp469->f2;
if(_tmp46B != Cyc_Absyn_Long_sz)goto _LL1FB;}}_LL1FA: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(
_tmp46A,0),0);goto _LL1EC;_LL1FB: {struct Cyc_Absyn_IntType_struct*_tmp46C=(struct
Cyc_Absyn_IntType_struct*)_tmp45C;if(_tmp46C->tag != 6)goto _LL1FD;else{_tmp46D=
_tmp46C->f1;_tmp46E=_tmp46C->f2;if(_tmp46E != Cyc_Absyn_LongLong_sz)goto _LL1FD;}}
_LL1FC: res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp46D,(long long)0),0);
goto _LL1EC;_LL1FD: {struct Cyc_Absyn_FloatType_struct*_tmp46F=(struct Cyc_Absyn_FloatType_struct*)
_tmp45C;if(_tmp46F->tag != 7)goto _LL1FF;else{_tmp470=_tmp46F->f1;if(_tmp470 != 0)
goto _LL1FF;}}_LL1FE:{const char*_tmpBFD;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((
_tmpBFD="0.0F",_tag_dyneither(_tmpBFD,sizeof(char),5))),0),0);}goto _LL1EC;_LL1FF: {
struct Cyc_Absyn_FloatType_struct*_tmp471=(struct Cyc_Absyn_FloatType_struct*)
_tmp45C;if(_tmp471->tag != 7)goto _LL201;else{_tmp472=_tmp471->f1;if(_tmp472 != 1)
goto _LL201;}}_LL200:{const char*_tmpBFE;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((
_tmpBFE="0.0",_tag_dyneither(_tmpBFE,sizeof(char),4))),1),0);}goto _LL1EC;_LL201: {
struct Cyc_Absyn_FloatType_struct*_tmp473=(struct Cyc_Absyn_FloatType_struct*)
_tmp45C;if(_tmp473->tag != 7)goto _LL203;else{_tmp474=_tmp473->f1;}}_LL202:{const
char*_tmpBFF;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBFF="0.0L",
_tag_dyneither(_tmpBFF,sizeof(char),5))),_tmp474),0);}goto _LL1EC;_LL203:;_LL204: {
const char*_tmpC03;void*_tmpC02[1];struct Cyc_String_pa_struct _tmpC01;(_tmpC01.tag=
0,((_tmpC01.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmpC02[0]=& _tmpC01,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpC03="found non-zero type %s in generate_zero",
_tag_dyneither(_tmpC03,sizeof(char),40))),_tag_dyneither(_tmpC02,sizeof(void*),1)))))));}
_LL1EC:;}{struct Cyc_Core_Opt*_tmpC04;res->topt=((_tmpC04=_cycalloc(sizeof(*
_tmpC04)),((_tmpC04->v=t,_tmpC04))));}return res;}static void Cyc_Toc_zero_ptr_assign_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){void*
fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,
Cyc_Absyn_true_conref);void*_tmp47C=Cyc_Toc_typ_to_c(elt_type);void*_tmp47D=Cyc_Toc_typ_to_c(
fat_ptr_type);void*_tmp47E=Cyc_Absyn_cstar_typ(_tmp47C,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmpC05;struct Cyc_Core_Opt*_tmp47F=(_tmpC05=_cycalloc(sizeof(*_tmpC05)),((
_tmpC05->v=_tmp47E,_tmpC05)));struct Cyc_Absyn_Exp*xinit;{void*_tmp480=e1->r;
struct Cyc_Absyn_Exp*_tmp482;struct Cyc_Absyn_Exp*_tmp484;struct Cyc_Absyn_Exp*
_tmp485;_LL206: {struct Cyc_Absyn_Deref_e_struct*_tmp481=(struct Cyc_Absyn_Deref_e_struct*)
_tmp480;if(_tmp481->tag != 21)goto _LL208;else{_tmp482=_tmp481->f1;}}_LL207: if(!
is_dyneither){_tmp482=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp482,0,Cyc_Absyn_Other_coercion,
0);{struct Cyc_Core_Opt*_tmpC06;_tmp482->topt=((_tmpC06=_cycalloc(sizeof(*_tmpC06)),((
_tmpC06->v=fat_ptr_type,_tmpC06))));};}Cyc_Toc_exp_to_c(nv,_tmp482);xinit=
_tmp482;goto _LL205;_LL208: {struct Cyc_Absyn_Subscript_e_struct*_tmp483=(struct
Cyc_Absyn_Subscript_e_struct*)_tmp480;if(_tmp483->tag != 24)goto _LL20A;else{
_tmp484=_tmp483->f1;_tmp485=_tmp483->f2;}}_LL209: if(!is_dyneither){_tmp484=Cyc_Absyn_cast_exp(
fat_ptr_type,_tmp484,0,Cyc_Absyn_Other_coercion,0);{struct Cyc_Core_Opt*_tmpC07;
_tmp484->topt=((_tmpC07=_cycalloc(sizeof(*_tmpC07)),((_tmpC07->v=fat_ptr_type,
_tmpC07))));};}Cyc_Toc_exp_to_c(nv,_tmp484);Cyc_Toc_exp_to_c(nv,_tmp485);{struct
Cyc_Absyn_Exp*_tmpC08[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,((
_tmpC08[2]=_tmp485,((_tmpC08[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmpC08[0]=_tmp484,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC08,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL205;_LL20A:;_LL20B: {const char*_tmpC0B;void*_tmpC0A;(_tmpC0A=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC0B="found bad lhs for zero-terminated pointer assignment",
_tag_dyneither(_tmpC0B,sizeof(char),53))),_tag_dyneither(_tmpC0A,sizeof(void*),0)));}
_LL205:;}{struct _tuple0*_tmp48B=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp48D;
struct _RegionHandle*_tmp48E;struct Cyc_Toc_Env*_tmp48C=nv;_tmp48D=*_tmp48C;
_tmp48E=_tmp48D.rgn;{struct Cyc_Toc_Env*_tmp48F=Cyc_Toc_add_varmap(_tmp48E,nv,
_tmp48B,Cyc_Absyn_var_exp(_tmp48B,0));struct Cyc_Absyn_Vardecl*_tmpC0C;struct Cyc_Absyn_Vardecl*
_tmp490=(_tmpC0C=_cycalloc(sizeof(*_tmpC0C)),((_tmpC0C->sc=Cyc_Absyn_Public,((
_tmpC0C->name=_tmp48B,((_tmpC0C->tq=Cyc_Toc_mt_tq,((_tmpC0C->type=_tmp47D,((
_tmpC0C->initializer=(struct Cyc_Absyn_Exp*)xinit,((_tmpC0C->rgn=0,((_tmpC0C->attributes=
0,((_tmpC0C->escapes=0,_tmpC0C)))))))))))))))));struct Cyc_Absyn_Local_b_struct
_tmpC0F;struct Cyc_Absyn_Local_b_struct*_tmpC0E;struct Cyc_Absyn_Local_b_struct*
_tmp491=(_tmpC0E=_cycalloc(sizeof(*_tmpC0E)),((_tmpC0E[0]=((_tmpC0F.tag=4,((
_tmpC0F.f1=_tmp490,_tmpC0F)))),_tmpC0E)));struct Cyc_Absyn_Exp*_tmp492=Cyc_Absyn_varb_exp(
_tmp48B,(void*)_tmp491,0);{struct Cyc_Core_Opt*_tmpC10;_tmp492->topt=((_tmpC10=
_cycalloc(sizeof(*_tmpC10)),((_tmpC10->v=fat_ptr_type,_tmpC10))));}{struct Cyc_Absyn_Exp*
_tmp494=Cyc_Absyn_deref_exp(_tmp492,0);{struct Cyc_Core_Opt*_tmpC11;_tmp494->topt=((
_tmpC11=_cycalloc(sizeof(*_tmpC11)),((_tmpC11->v=elt_type,_tmpC11))));}Cyc_Toc_exp_to_c(
_tmp48F,_tmp494);{struct _tuple0*_tmp496=Cyc_Toc_temp_var();_tmp48F=Cyc_Toc_add_varmap(
_tmp48E,_tmp48F,_tmp496,Cyc_Absyn_var_exp(_tmp496,0));{struct Cyc_Absyn_Vardecl*
_tmpC12;struct Cyc_Absyn_Vardecl*_tmp497=(_tmpC12=_cycalloc(sizeof(*_tmpC12)),((
_tmpC12->sc=Cyc_Absyn_Public,((_tmpC12->name=_tmp496,((_tmpC12->tq=Cyc_Toc_mt_tq,((
_tmpC12->type=_tmp47C,((_tmpC12->initializer=(struct Cyc_Absyn_Exp*)_tmp494,((
_tmpC12->rgn=0,((_tmpC12->attributes=0,((_tmpC12->escapes=0,_tmpC12)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpC15;struct Cyc_Absyn_Local_b_struct*_tmpC14;
struct Cyc_Absyn_Local_b_struct*_tmp498=(_tmpC14=_cycalloc(sizeof(*_tmpC14)),((
_tmpC14[0]=((_tmpC15.tag=4,((_tmpC15.f1=_tmp497,_tmpC15)))),_tmpC14)));struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp499=Cyc_Absyn_varb_exp(_tmp496,(
void*)_tmp498,0);_tmp499->topt=_tmp494->topt;z_init=Cyc_Absyn_prim2_exp((enum 
Cyc_Absyn_Primop)popt->v,_tmp499,e2,0);z_init->topt=_tmp499->topt;}Cyc_Toc_exp_to_c(
_tmp48F,z_init);{struct _tuple0*_tmp49A=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmpC16;struct Cyc_Absyn_Vardecl*_tmp49B=(_tmpC16=_cycalloc(sizeof(*_tmpC16)),((
_tmpC16->sc=Cyc_Absyn_Public,((_tmpC16->name=_tmp49A,((_tmpC16->tq=Cyc_Toc_mt_tq,((
_tmpC16->type=_tmp47C,((_tmpC16->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmpC16->rgn=0,((_tmpC16->attributes=0,((_tmpC16->escapes=0,_tmpC16)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpC19;struct Cyc_Absyn_Local_b_struct*_tmpC18;
struct Cyc_Absyn_Local_b_struct*_tmp49C=(_tmpC18=_cycalloc(sizeof(*_tmpC18)),((
_tmpC18[0]=((_tmpC19.tag=4,((_tmpC19.f1=_tmp49B,_tmpC19)))),_tmpC18)));_tmp48F=
Cyc_Toc_add_varmap(_tmp48E,_tmp48F,_tmp49A,Cyc_Absyn_var_exp(_tmp49A,0));{struct
Cyc_Absyn_Exp*_tmp49D=Cyc_Absyn_varb_exp(_tmp496,(void*)_tmp498,0);_tmp49D->topt=
_tmp494->topt;{struct Cyc_Absyn_Exp*_tmp49E=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp49F=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp49D,_tmp49E,0);{
struct Cyc_Core_Opt*_tmpC1A;_tmp49F->topt=((_tmpC1A=_cycalloc(sizeof(*_tmpC1A)),((
_tmpC1A->v=Cyc_Absyn_sint_typ,_tmpC1A))));}Cyc_Toc_exp_to_c(_tmp48F,_tmp49F);{
struct Cyc_Absyn_Exp*_tmp4A1=Cyc_Absyn_varb_exp(_tmp49A,(void*)_tmp49C,0);_tmp4A1->topt=
_tmp494->topt;{struct Cyc_Absyn_Exp*_tmp4A2=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp4A3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp4A1,_tmp4A2,0);{
struct Cyc_Core_Opt*_tmpC1B;_tmp4A3->topt=((_tmpC1B=_cycalloc(sizeof(*_tmpC1B)),((
_tmpC1B->v=Cyc_Absyn_sint_typ,_tmpC1B))));}Cyc_Toc_exp_to_c(_tmp48F,_tmp4A3);{
struct Cyc_Absyn_Exp*_tmpC1C[2];struct Cyc_List_List*_tmp4A5=(_tmpC1C[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0),((_tmpC1C[0]=Cyc_Absyn_varb_exp(_tmp48B,(void*)
_tmp491,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC1C,sizeof(struct Cyc_Absyn_Exp*),2)))));struct Cyc_Absyn_Exp*
_tmp4A6=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;xsize=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp4A5,0),
_tmp4A6,0);{struct Cyc_Absyn_Exp*_tmp4A7=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp49F,_tmp4A3,0),0);struct Cyc_Absyn_Stmt*_tmp4A8=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp4A9=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp48B,(void*)_tmp491,0),Cyc_Toc_curr_sp,0);_tmp4A9=Cyc_Toc_cast_it(
_tmp47E,_tmp4A9);{struct Cyc_Absyn_Exp*_tmp4AA=Cyc_Absyn_deref_exp(_tmp4A9,0);
struct Cyc_Absyn_Exp*_tmp4AB=Cyc_Absyn_assign_exp(_tmp4AA,Cyc_Absyn_var_exp(
_tmp49A,0),0);struct Cyc_Absyn_Stmt*_tmp4AC=Cyc_Absyn_exp_stmt(_tmp4AB,0);_tmp4AC=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp4A7,_tmp4A8,Cyc_Absyn_skip_stmt(
0),0),_tmp4AC,0);{struct Cyc_Absyn_Var_d_struct*_tmpC22;struct Cyc_Absyn_Var_d_struct
_tmpC21;struct Cyc_Absyn_Decl*_tmpC20;_tmp4AC=Cyc_Absyn_decl_stmt(((_tmpC20=
_cycalloc(sizeof(*_tmpC20)),((_tmpC20->r=(void*)((_tmpC22=_cycalloc(sizeof(*
_tmpC22)),((_tmpC22[0]=((_tmpC21.tag=0,((_tmpC21.f1=_tmp49B,_tmpC21)))),_tmpC22)))),((
_tmpC20->loc=0,_tmpC20)))))),_tmp4AC,0);}{struct Cyc_Absyn_Var_d_struct*_tmpC28;
struct Cyc_Absyn_Var_d_struct _tmpC27;struct Cyc_Absyn_Decl*_tmpC26;_tmp4AC=Cyc_Absyn_decl_stmt(((
_tmpC26=_cycalloc(sizeof(*_tmpC26)),((_tmpC26->r=(void*)((_tmpC28=_cycalloc(
sizeof(*_tmpC28)),((_tmpC28[0]=((_tmpC27.tag=0,((_tmpC27.f1=_tmp497,_tmpC27)))),
_tmpC28)))),((_tmpC26->loc=0,_tmpC26)))))),_tmp4AC,0);}{struct Cyc_Absyn_Var_d_struct*
_tmpC2E;struct Cyc_Absyn_Var_d_struct _tmpC2D;struct Cyc_Absyn_Decl*_tmpC2C;_tmp4AC=
Cyc_Absyn_decl_stmt(((_tmpC2C=_cycalloc(sizeof(*_tmpC2C)),((_tmpC2C->r=(void*)((
_tmpC2E=_cycalloc(sizeof(*_tmpC2E)),((_tmpC2E[0]=((_tmpC2D.tag=0,((_tmpC2D.f1=
_tmp490,_tmpC2D)))),_tmpC2E)))),((_tmpC2C->loc=0,_tmpC2C)))))),_tmp4AC,0);}e->r=
Cyc_Toc_stmt_exp_r(_tmp4AC);};};};};};};};};};};};};};}static void*Cyc_Toc_check_tagged_union(
struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int
in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,
struct Cyc_Position_Segment*)){struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp4C1=Cyc_Tcutil_compress(
aggrtype);struct Cyc_Absyn_AggrInfo _tmp4C3;union Cyc_Absyn_AggrInfoU _tmp4C4;_LL20D: {
struct Cyc_Absyn_AggrType_struct*_tmp4C2=(struct Cyc_Absyn_AggrType_struct*)
_tmp4C1;if(_tmp4C2->tag != 11)goto _LL20F;else{_tmp4C3=_tmp4C2->f1;_tmp4C4=_tmp4C3.aggr_info;}}
_LL20E: ad=Cyc_Absyn_get_known_aggrdecl(_tmp4C4);goto _LL20C;_LL20F:;_LL210: {
struct Cyc_String_pa_struct _tmpC36;void*_tmpC35[1];const char*_tmpC34;void*_tmpC33;(
_tmpC33=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((
struct _dyneither_ptr)((_tmpC36.tag=0,((_tmpC36.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpC35[0]=& _tmpC36,Cyc_aprintf(((
_tmpC34="expecting union but found %s in check_tagged_union",_tag_dyneither(
_tmpC34,sizeof(char),51))),_tag_dyneither(_tmpC35,sizeof(void*),1)))))))),
_tag_dyneither(_tmpC33,sizeof(void*),0)));}_LL20C:;}{struct _tuple0*_tmp4C9=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4CA=Cyc_Absyn_var_exp(_tmp4C9,0);struct Cyc_Absyn_Exp*
_tmp4CB=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);if(in_lhs){
struct Cyc_Absyn_Exp*_tmp4CC=Cyc_Absyn_aggrarrow_exp(_tmp4CA,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4CD=Cyc_Absyn_neq_exp(_tmp4CC,_tmp4CB,0);struct Cyc_Absyn_Exp*
_tmp4CE=Cyc_Absyn_aggrarrow_exp(_tmp4CA,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Stmt*
_tmp4CF=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp4CE,0),0);struct Cyc_Absyn_Stmt*
_tmp4D0=Cyc_Absyn_ifthenelse_stmt(_tmp4CD,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);void*_tmp4D1=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp4D2=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);struct Cyc_Absyn_Stmt*_tmp4D3=
Cyc_Absyn_declare_stmt(_tmp4C9,_tmp4D1,(struct Cyc_Absyn_Exp*)_tmp4D2,Cyc_Absyn_seq_stmt(
_tmp4D0,_tmp4CF,0),0);return Cyc_Toc_stmt_exp_r(_tmp4D3);}else{struct Cyc_Absyn_Exp*
_tmp4D4=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp4CA,f,0),Cyc_Toc_tag_sp,0);struct
Cyc_Absyn_Exp*_tmp4D5=Cyc_Absyn_neq_exp(_tmp4D4,_tmp4CB,0);struct Cyc_Absyn_Exp*
_tmp4D6=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp4CA,f,0),Cyc_Toc_val_sp,0);struct
Cyc_Absyn_Stmt*_tmp4D7=Cyc_Absyn_exp_stmt(_tmp4D6,0);struct Cyc_Absyn_Stmt*
_tmp4D8=Cyc_Absyn_ifthenelse_stmt(_tmp4D5,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp4D9=Cyc_Absyn_declare_stmt(_tmp4C9,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp4D8,_tmp4D7,0),0);return Cyc_Toc_stmt_exp_r(
_tmp4D9);}};}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read){void*_tmp4DA=e->r;struct Cyc_Absyn_Exp*
_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DE;struct _dyneither_ptr*_tmp4DF;int _tmp4E0;int*
_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E3;struct _dyneither_ptr*_tmp4E4;int _tmp4E5;int*
_tmp4E6;_LL212: {struct Cyc_Absyn_Cast_e_struct*_tmp4DB=(struct Cyc_Absyn_Cast_e_struct*)
_tmp4DA;if(_tmp4DB->tag != 15)goto _LL214;else{_tmp4DC=_tmp4DB->f2;}}_LL213: {
const char*_tmpC39;void*_tmpC38;(_tmpC38=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC39="cast on lhs!",
_tag_dyneither(_tmpC39,sizeof(char),13))),_tag_dyneither(_tmpC38,sizeof(void*),0)));}
_LL214: {struct Cyc_Absyn_AggrMember_e_struct*_tmp4DD=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp4DA;if(_tmp4DD->tag != 22)goto _LL216;else{_tmp4DE=_tmp4DD->f1;_tmp4DF=_tmp4DD->f2;
_tmp4E0=_tmp4DD->f4;_tmp4E1=(int*)& _tmp4DD->f4;}}_LL215: {void*_tmp4E9=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4DE->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp4EB;union Cyc_Absyn_AggrInfoU _tmp4EC;_LL21B: {struct Cyc_Absyn_AggrType_struct*
_tmp4EA=(struct Cyc_Absyn_AggrType_struct*)_tmp4E9;if(_tmp4EA->tag != 11)goto
_LL21D;else{_tmp4EB=_tmp4EA->f1;_tmp4EC=_tmp4EB.aggr_info;}}_LL21C: {struct Cyc_Absyn_Aggrdecl*
_tmp4ED=Cyc_Absyn_get_known_aggrdecl(_tmp4EC);*f_tag=Cyc_Toc_get_member_offset(
_tmp4ED,_tmp4DF);{const char*_tmpC3E;void*_tmpC3D[2];struct Cyc_String_pa_struct
_tmpC3C;struct Cyc_String_pa_struct _tmpC3B;struct _dyneither_ptr str=(struct
_dyneither_ptr)((_tmpC3B.tag=0,((_tmpC3B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp4DF),((_tmpC3C.tag=0,((_tmpC3C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp4ED->name).f2),((_tmpC3D[0]=& _tmpC3C,((_tmpC3D[1]=&
_tmpC3B,Cyc_aprintf(((_tmpC3E="_union_%s_%s",_tag_dyneither(_tmpC3E,sizeof(char),
13))),_tag_dyneither(_tmpC3D,sizeof(void*),2))))))))))))));{struct _dyneither_ptr*
_tmpC3F;*tagged_member_type=Cyc_Absyn_strct(((_tmpC3F=_cycalloc(sizeof(*_tmpC3F)),((
_tmpC3F[0]=str,_tmpC3F)))));}if(clear_read)*_tmp4E1=0;return((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4ED->impl))->tagged;};}_LL21D:;_LL21E: return 0;_LL21A:;}_LL216: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp4E2=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp4DA;if(_tmp4E2->tag != 23)goto _LL218;else{_tmp4E3=_tmp4E2->f1;_tmp4E4=_tmp4E2->f2;
_tmp4E5=_tmp4E2->f4;_tmp4E6=(int*)& _tmp4E2->f4;}}_LL217: {void*_tmp4F3=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4E3->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp4F5;void*_tmp4F6;_LL220: {struct Cyc_Absyn_PointerType_struct*_tmp4F4=(struct
Cyc_Absyn_PointerType_struct*)_tmp4F3;if(_tmp4F4->tag != 5)goto _LL222;else{
_tmp4F5=_tmp4F4->f1;_tmp4F6=_tmp4F5.elt_typ;}}_LL221: {void*_tmp4F7=Cyc_Tcutil_compress(
_tmp4F6);struct Cyc_Absyn_AggrInfo _tmp4F9;union Cyc_Absyn_AggrInfoU _tmp4FA;_LL225: {
struct Cyc_Absyn_AggrType_struct*_tmp4F8=(struct Cyc_Absyn_AggrType_struct*)
_tmp4F7;if(_tmp4F8->tag != 11)goto _LL227;else{_tmp4F9=_tmp4F8->f1;_tmp4FA=_tmp4F9.aggr_info;}}
_LL226: {struct Cyc_Absyn_Aggrdecl*_tmp4FB=Cyc_Absyn_get_known_aggrdecl(_tmp4FA);*
f_tag=Cyc_Toc_get_member_offset(_tmp4FB,_tmp4E4);{const char*_tmpC44;void*_tmpC43[
2];struct Cyc_String_pa_struct _tmpC42;struct Cyc_String_pa_struct _tmpC41;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpC41.tag=0,((_tmpC41.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp4E4),((_tmpC42.tag=0,((_tmpC42.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4FB->name).f2),((_tmpC43[0]=&
_tmpC42,((_tmpC43[1]=& _tmpC41,Cyc_aprintf(((_tmpC44="_union_%s_%s",
_tag_dyneither(_tmpC44,sizeof(char),13))),_tag_dyneither(_tmpC43,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpC45;*tagged_member_type=Cyc_Absyn_strct(((_tmpC45=
_cycalloc(sizeof(*_tmpC45)),((_tmpC45[0]=str,_tmpC45)))));}if(clear_read)*
_tmp4E6=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4FB->impl))->tagged;};}
_LL227:;_LL228: return 0;_LL224:;}_LL222:;_LL223: return 0;_LL21F:;}_LL218:;_LL219:
return 0;_LL211:;}void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*
e,int tag,void*mem_type){struct _tuple0*_tmp501=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
temp_exp=Cyc_Absyn_var_exp(_tmp501,0);struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(
tag,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,
0),0);struct Cyc_Absyn_Exp*_tmp502=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_ifthenelse_stmt(_tmp502,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp501,Cyc_Absyn_cstar_typ(
mem_type,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(
e)),Cyc_Absyn_seq_stmt(s2,s3,0),0);e->r=Cyc_Toc_stmt_exp_r(s1);}static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type){struct _tuple0*_tmp503=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp503,0);struct Cyc_Absyn_Exp*
temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*
temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 
0)s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*
_tmp504=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp504,
Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=
Cyc_Absyn_declare_stmt(_tmp503,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(e1),Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple21{struct _tuple0*f1;void*f2;struct
Cyc_Absyn_Exp*f3;};struct _tuple22{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*
f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void _tmpC98(unsigned int*_tmpC97,unsigned int*_tmpC96,struct _tuple0***
_tmpC94){for(*_tmpC97=0;*_tmpC97 < *_tmpC96;(*_tmpC97)++){(*_tmpC94)[*_tmpC97]=
Cyc_Toc_temp_var();}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){void*_tmp505=e->r;if(e->topt == 0){const char*_tmpC49;void*_tmpC48[1];struct Cyc_String_pa_struct
_tmpC47;(_tmpC47.tag=0,((_tmpC47.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpC48[0]=& _tmpC47,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC49="exp_to_c: no type for %s",
_tag_dyneither(_tmpC49,sizeof(char),25))),_tag_dyneither(_tmpC48,sizeof(void*),1)))))));}{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp509=
_tmp505;union Cyc_Absyn_Cnst _tmp50B;int _tmp50C;struct _tuple0*_tmp50F;void*_tmp510;
struct _tuple0*_tmp512;enum Cyc_Absyn_Primop _tmp514;struct Cyc_List_List*_tmp515;
struct Cyc_Absyn_Exp*_tmp517;enum Cyc_Absyn_Incrementor _tmp518;struct Cyc_Absyn_Exp*
_tmp51A;struct Cyc_Core_Opt*_tmp51B;struct Cyc_Absyn_Exp*_tmp51C;struct Cyc_Absyn_Exp*
_tmp51E;struct Cyc_Absyn_Exp*_tmp51F;struct Cyc_Absyn_Exp*_tmp520;struct Cyc_Absyn_Exp*
_tmp522;struct Cyc_Absyn_Exp*_tmp523;struct Cyc_Absyn_Exp*_tmp525;struct Cyc_Absyn_Exp*
_tmp526;struct Cyc_Absyn_Exp*_tmp528;struct Cyc_Absyn_Exp*_tmp529;struct Cyc_Absyn_Exp*
_tmp52B;struct Cyc_List_List*_tmp52C;struct Cyc_Absyn_Exp*_tmp52E;struct Cyc_List_List*
_tmp52F;struct Cyc_Absyn_VarargCallInfo*_tmp530;struct Cyc_Absyn_Exp*_tmp532;
struct Cyc_List_List*_tmp533;struct Cyc_Absyn_VarargCallInfo*_tmp534;struct Cyc_Absyn_VarargCallInfo
_tmp535;int _tmp536;struct Cyc_List_List*_tmp537;struct Cyc_Absyn_VarargInfo*
_tmp538;struct Cyc_Absyn_Exp*_tmp53A;struct Cyc_Absyn_Exp*_tmp53C;struct Cyc_Absyn_Exp*
_tmp53E;struct Cyc_List_List*_tmp53F;void*_tmp541;void**_tmp542;struct Cyc_Absyn_Exp*
_tmp543;int _tmp544;enum Cyc_Absyn_Coercion _tmp545;struct Cyc_Absyn_Exp*_tmp547;
struct Cyc_Absyn_Exp*_tmp549;struct Cyc_Absyn_Exp*_tmp54A;struct Cyc_Absyn_Exp*
_tmp54C;void*_tmp54E;void*_tmp550;void*_tmp551;struct _dyneither_ptr*_tmp553;void*
_tmp555;void*_tmp556;unsigned int _tmp558;struct Cyc_Absyn_Exp*_tmp55A;struct Cyc_Absyn_Exp*
_tmp55C;struct _dyneither_ptr*_tmp55D;int _tmp55E;int _tmp55F;struct Cyc_Absyn_Exp*
_tmp561;struct _dyneither_ptr*_tmp562;int _tmp563;int _tmp564;struct Cyc_Absyn_Exp*
_tmp566;struct Cyc_Absyn_Exp*_tmp567;struct Cyc_List_List*_tmp569;struct Cyc_List_List*
_tmp56B;struct Cyc_Absyn_Vardecl*_tmp56D;struct Cyc_Absyn_Exp*_tmp56E;struct Cyc_Absyn_Exp*
_tmp56F;int _tmp570;struct _tuple0*_tmp572;struct Cyc_List_List*_tmp573;struct Cyc_List_List*
_tmp574;struct Cyc_Absyn_Aggrdecl*_tmp575;void*_tmp577;struct Cyc_List_List*
_tmp578;struct Cyc_List_List*_tmp57A;struct Cyc_Absyn_Datatypedecl*_tmp57B;struct
Cyc_Absyn_Datatypefield*_tmp57C;struct Cyc_Absyn_MallocInfo _tmp580;int _tmp581;
struct Cyc_Absyn_Exp*_tmp582;void**_tmp583;struct Cyc_Absyn_Exp*_tmp584;int _tmp585;
struct Cyc_Absyn_Exp*_tmp587;struct Cyc_Absyn_Exp*_tmp588;struct Cyc_Absyn_Exp*
_tmp58A;struct _dyneither_ptr*_tmp58B;struct Cyc_Absyn_Stmt*_tmp58D;_LL22A: {
struct Cyc_Absyn_Const_e_struct*_tmp50A=(struct Cyc_Absyn_Const_e_struct*)_tmp509;
if(_tmp50A->tag != 0)goto _LL22C;else{_tmp50B=_tmp50A->f1;if((_tmp50B.Null_c).tag
!= 1)goto _LL22C;_tmp50C=(int)(_tmp50B.Null_c).val;}}_LL22B: {struct Cyc_Absyn_Exp*
_tmp592=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp592,
_tmp592))->r;else{struct Cyc_Absyn_Exp*_tmpC4A[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((
_tmpC4A[2]=_tmp592,((_tmpC4A[1]=_tmp592,((_tmpC4A[0]=_tmp592,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC4A,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL229;}_LL22C: {struct Cyc_Absyn_Const_e_struct*
_tmp50D=(struct Cyc_Absyn_Const_e_struct*)_tmp509;if(_tmp50D->tag != 0)goto _LL22E;}
_LL22D: goto _LL229;_LL22E: {struct Cyc_Absyn_Var_e_struct*_tmp50E=(struct Cyc_Absyn_Var_e_struct*)
_tmp509;if(_tmp50E->tag != 1)goto _LL230;else{_tmp50F=_tmp50E->f1;_tmp510=(void*)
_tmp50E->f2;}}_LL22F:{struct _handler_cons _tmp594;_push_handler(& _tmp594);{int
_tmp596=0;if(setjmp(_tmp594.handler))_tmp596=1;if(!_tmp596){e->r=(Cyc_Toc_lookup_varmap(
nv,_tmp50F))->r;;_pop_handler();}else{void*_tmp595=(void*)_exn_thrown;void*
_tmp598=_tmp595;_LL283: {struct Cyc_Dict_Absent_struct*_tmp599=(struct Cyc_Dict_Absent_struct*)
_tmp598;if(_tmp599->tag != Cyc_Dict_Absent)goto _LL285;}_LL284: {const char*_tmpC4E;
void*_tmpC4D[1];struct Cyc_String_pa_struct _tmpC4C;(_tmpC4C.tag=0,((_tmpC4C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp50F)),((
_tmpC4D[0]=& _tmpC4C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC4E="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpC4E,sizeof(char),
32))),_tag_dyneither(_tmpC4D,sizeof(void*),1)))))));}_LL285:;_LL286:(void)_throw(
_tmp598);_LL282:;}};}goto _LL229;_LL230: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp511=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp509;if(_tmp511->tag != 2)goto
_LL232;else{_tmp512=_tmp511->f1;}}_LL231: {const char*_tmpC51;void*_tmpC50;(
_tmpC50=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC51="unknownid",_tag_dyneither(_tmpC51,sizeof(char),10))),_tag_dyneither(
_tmpC50,sizeof(void*),0)));}_LL232: {struct Cyc_Absyn_Primop_e_struct*_tmp513=(
struct Cyc_Absyn_Primop_e_struct*)_tmp509;if(_tmp513->tag != 3)goto _LL234;else{
_tmp514=_tmp513->f1;_tmp515=_tmp513->f2;}}_LL233: {struct Cyc_List_List*_tmp59F=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp515);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp515);switch(_tmp514){case Cyc_Absyn_Numelts: _LL287: {
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp515))->hd;{void*_tmp5A0=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo _tmp5A2;struct Cyc_Absyn_Exp*
_tmp5A3;struct Cyc_Absyn_PtrInfo _tmp5A5;void*_tmp5A6;struct Cyc_Absyn_PtrAtts
_tmp5A7;union Cyc_Absyn_Constraint*_tmp5A8;union Cyc_Absyn_Constraint*_tmp5A9;
union Cyc_Absyn_Constraint*_tmp5AA;_LL28A: {struct Cyc_Absyn_ArrayType_struct*
_tmp5A1=(struct Cyc_Absyn_ArrayType_struct*)_tmp5A0;if(_tmp5A1->tag != 8)goto
_LL28C;else{_tmp5A2=_tmp5A1->f1;_tmp5A3=_tmp5A2.num_elts;}}_LL28B: if(!Cyc_Evexp_c_can_eval((
struct Cyc_Absyn_Exp*)_check_null(_tmp5A3))){const char*_tmpC54;void*_tmpC53;(
_tmpC53=0,Cyc_Tcutil_terr(e->loc,((_tmpC54="can't calculate numelts",
_tag_dyneither(_tmpC54,sizeof(char),24))),_tag_dyneither(_tmpC53,sizeof(void*),0)));}
e->r=_tmp5A3->r;goto _LL289;_LL28C: {struct Cyc_Absyn_PointerType_struct*_tmp5A4=(
struct Cyc_Absyn_PointerType_struct*)_tmp5A0;if(_tmp5A4->tag != 5)goto _LL28E;else{
_tmp5A5=_tmp5A4->f1;_tmp5A6=_tmp5A5.elt_typ;_tmp5A7=_tmp5A5.ptr_atts;_tmp5A8=
_tmp5A7.nullable;_tmp5A9=_tmp5A7.bounds;_tmp5AA=_tmp5A7.zero_term;}}_LL28D:{void*
_tmp5AD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp5A9);struct Cyc_Absyn_Exp*_tmp5B0;_LL291: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5AE=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5AD;if(_tmp5AE->tag != 0)goto
_LL293;}_LL292:{struct Cyc_Absyn_Exp*_tmpC55[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,((
_tmpC55[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A6),0),((_tmpC55[0]=(
struct Cyc_Absyn_Exp*)_tmp515->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC55,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL290;_LL293: {struct Cyc_Absyn_Upper_b_struct*_tmp5AF=(struct Cyc_Absyn_Upper_b_struct*)
_tmp5AD;if(_tmp5AF->tag != 1)goto _LL290;else{_tmp5B0=_tmp5AF->f1;}}_LL294: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5AA)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp515->hd);struct Cyc_Absyn_Exp*_tmpC56[2];e->r=Cyc_Toc_fncall_exp_r(
function_e,((_tmpC56[1]=_tmp5B0,((_tmpC56[0]=(struct Cyc_Absyn_Exp*)_tmp515->hd,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC56,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp5A8)){if(!Cyc_Evexp_c_can_eval(_tmp5B0)){const
char*_tmpC59;void*_tmpC58;(_tmpC58=0,Cyc_Tcutil_terr(e->loc,((_tmpC59="can't calculate numelts",
_tag_dyneither(_tmpC59,sizeof(char),24))),_tag_dyneither(_tmpC58,sizeof(void*),0)));}
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp5B0,Cyc_Absyn_uint_exp(0,0));}else{e->r=
_tmp5B0->r;goto _LL290;}}goto _LL290;_LL290:;}goto _LL289;_LL28E:;_LL28F: {const
char*_tmpC5E;void*_tmpC5D[2];struct Cyc_String_pa_struct _tmpC5C;struct Cyc_String_pa_struct
_tmpC5B;(_tmpC5B.tag=0,((_tmpC5B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((
_tmpC5C.tag=0,((_tmpC5C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmpC5D[0]=& _tmpC5C,((
_tmpC5D[1]=& _tmpC5B,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC5E="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpC5E,
sizeof(char),41))),_tag_dyneither(_tmpC5D,sizeof(void*),2)))))))))))));}_LL289:;}
break;}case Cyc_Absyn_Plus: _LL288:{void*_tmp5B9=Cyc_Tcutil_compress((void*)((
struct Cyc_List_List*)_check_null(_tmp59F))->hd);struct Cyc_Absyn_PtrInfo _tmp5BB;
void*_tmp5BC;struct Cyc_Absyn_PtrAtts _tmp5BD;union Cyc_Absyn_Constraint*_tmp5BE;
union Cyc_Absyn_Constraint*_tmp5BF;_LL297: {struct Cyc_Absyn_PointerType_struct*
_tmp5BA=(struct Cyc_Absyn_PointerType_struct*)_tmp5B9;if(_tmp5BA->tag != 5)goto
_LL299;else{_tmp5BB=_tmp5BA->f1;_tmp5BC=_tmp5BB.elt_typ;_tmp5BD=_tmp5BB.ptr_atts;
_tmp5BE=_tmp5BD.bounds;_tmp5BF=_tmp5BD.zero_term;}}_LL298:{void*_tmp5C0=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp5BE);struct Cyc_Absyn_Exp*_tmp5C3;_LL29C: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5C1=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5C0;if(_tmp5C1->tag != 0)goto
_LL29E;}_LL29D: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp515))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp515->tl))->hd;{struct Cyc_Absyn_Exp*_tmpC5F[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpC5F[2]=e2,((_tmpC5F[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5BC),0),((_tmpC5F[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5F,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}goto _LL29B;}_LL29E: {struct Cyc_Absyn_Upper_b_struct*_tmp5C2=(struct
Cyc_Absyn_Upper_b_struct*)_tmp5C0;if(_tmp5C2->tag != 1)goto _LL29B;else{_tmp5C3=
_tmp5C2->f1;}}_LL29F: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp5BF)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp515))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp515->tl))->hd;struct Cyc_Absyn_Exp*_tmpC60[3];e->r=(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
e1),((_tmpC60[2]=e2,((_tmpC60[1]=_tmp5C3,((_tmpC60[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC60,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r;}goto _LL29B;_LL29B:;}goto _LL296;_LL299:;_LL29A: goto _LL296;
_LL296:;}break;case Cyc_Absyn_Minus: _LL295: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp59F))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp515))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp515->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp59F->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{struct Cyc_Absyn_Exp*_tmpC61[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpC61[2]=Cyc_Absyn_prim1_exp(
Cyc_Absyn_Minus,e2,0),((_tmpC61[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpC61[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC61,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}break;}case Cyc_Absyn_Eq:
_LL2A0: goto _LL2A1;case Cyc_Absyn_Neq: _LL2A1: goto _LL2A2;case Cyc_Absyn_Gt: _LL2A2:
goto _LL2A3;case Cyc_Absyn_Gte: _LL2A3: goto _LL2A4;case Cyc_Absyn_Lt: _LL2A4: goto
_LL2A5;case Cyc_Absyn_Lte: _LL2A5: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp515))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp515->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp59F))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp59F->tl))->hd;void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& elt_typ))e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,
0));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp,0));break;}default: _LL2A6: break;}goto _LL229;}_LL234: {
struct Cyc_Absyn_Increment_e_struct*_tmp516=(struct Cyc_Absyn_Increment_e_struct*)
_tmp509;if(_tmp516->tag != 5)goto _LL236;else{_tmp517=_tmp516->f1;_tmp518=_tmp516->f2;}}
_LL235: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp517->topt))->v;
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;const char*_tmpC62;struct _dyneither_ptr incr_str=(_tmpC62="increment",
_tag_dyneither(_tmpC62,sizeof(char),10));if(_tmp518 == Cyc_Absyn_PreDec  || 
_tmp518 == Cyc_Absyn_PostDec){const char*_tmpC63;incr_str=((_tmpC63="decrement",
_tag_dyneither(_tmpC63,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp517,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpC67;void*_tmpC66[1];
struct Cyc_String_pa_struct _tmpC65;(_tmpC65.tag=0,((_tmpC65.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC66[0]=& _tmpC65,Cyc_Tcutil_terr(
e->loc,((_tmpC67="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpC67,sizeof(char),74))),_tag_dyneither(_tmpC66,sizeof(void*),1)))))));}{
const char*_tmpC6A;void*_tmpC69;(_tmpC69=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC6A="in-place inc/dec on zero-term",
_tag_dyneither(_tmpC6A,sizeof(char),30))),_tag_dyneither(_tmpC69,sizeof(void*),0)));};}{
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp517,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp5CD=Cyc_Absyn_signed_int_exp(
1,0);{struct Cyc_Core_Opt*_tmpC6B;_tmp5CD->topt=((_tmpC6B=_cycalloc(sizeof(*
_tmpC6B)),((_tmpC6B->v=Cyc_Absyn_sint_typ,_tmpC6B))));}switch(_tmp518){case Cyc_Absyn_PreInc:
_LL2A8:{struct Cyc_Absyn_AssignOp_e_struct _tmpC71;struct Cyc_Core_Opt*_tmpC70;
struct Cyc_Absyn_AssignOp_e_struct*_tmpC6F;e->r=(void*)((_tmpC6F=_cycalloc(
sizeof(*_tmpC6F)),((_tmpC6F[0]=((_tmpC71.tag=4,((_tmpC71.f1=_tmp517,((_tmpC71.f2=((
_tmpC70=_cycalloc_atomic(sizeof(*_tmpC70)),((_tmpC70->v=(void*)Cyc_Absyn_Plus,
_tmpC70)))),((_tmpC71.f3=_tmp5CD,_tmpC71)))))))),_tmpC6F))));}Cyc_Toc_exp_to_c(
nv,e);return;case Cyc_Absyn_PreDec: _LL2A9:{struct Cyc_Absyn_AssignOp_e_struct
_tmpC77;struct Cyc_Core_Opt*_tmpC76;struct Cyc_Absyn_AssignOp_e_struct*_tmpC75;e->r=(
void*)((_tmpC75=_cycalloc(sizeof(*_tmpC75)),((_tmpC75[0]=((_tmpC77.tag=4,((
_tmpC77.f1=_tmp517,((_tmpC77.f2=((_tmpC76=_cycalloc_atomic(sizeof(*_tmpC76)),((
_tmpC76->v=(void*)Cyc_Absyn_Minus,_tmpC76)))),((_tmpC77.f3=_tmp5CD,_tmpC77)))))))),
_tmpC75))));}Cyc_Toc_exp_to_c(nv,e);return;default: _LL2AA:{const char*_tmpC7B;
void*_tmpC7A[1];struct Cyc_String_pa_struct _tmpC79;(_tmpC79.tag=0,((_tmpC79.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC7A[0]=& _tmpC79,Cyc_Tcutil_terr(
e->loc,((_tmpC7B="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpC7B,sizeof(char),59))),_tag_dyneither(_tmpC7A,sizeof(void*),1)))))));}{
const char*_tmpC7E;void*_tmpC7D;(_tmpC7D=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC7E="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpC7E,sizeof(char),34))),_tag_dyneither(_tmpC7D,sizeof(void*),0)));};}}
Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp517);Cyc_Toc_set_lhs(nv,0);{void*
elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp518 == Cyc_Absyn_PostInc
 || _tmp518 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp518 == Cyc_Absyn_PreDec  || _tmp518 == Cyc_Absyn_PostDec)change=- 1;{struct
Cyc_Absyn_Exp*_tmpC7F[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC7F[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpC7F[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpC7F[0]=Cyc_Toc_push_address_exp(_tmp517),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC7F,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Toc_functionSet*_tmp5DB=_tmp518 == Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:&
Cyc_Toc__zero_arr_inplace_plus_post_functionSet;struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(
_tmp5DB,_tmp517);struct Cyc_Absyn_Exp*_tmpC80[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((
_tmpC80[1]=Cyc_Absyn_signed_int_exp(1,0),((_tmpC80[0]=Cyc_Toc_push_address_exp(
_tmp517),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC80,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(elt_typ == (
void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp517)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(
_tmp517,0,Cyc_Toc_incr_lvalue,_tmp518);e->r=_tmp517->r;}}}goto _LL229;};};}_LL236: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp519=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp509;if(_tmp519->tag != 4)goto _LL238;else{_tmp51A=_tmp519->f1;_tmp51B=_tmp519->f2;
_tmp51C=_tmp519->f3;}}_LL237: {void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp51A->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp51C->topt))->v;int f_tag=0;void*tagged_member_struct_type=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Toc_is_tagged_union_project(_tmp51A,& f_tag,&
tagged_member_struct_type,1)){Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp51A);
Cyc_Toc_set_lhs(nv,0);Cyc_Toc_exp_to_c(nv,_tmp51C);e->r=Cyc_Toc_tagged_union_assignop(
_tmp51A,e1_old_typ,_tmp51B,_tmp51C,e2_old_typ,f_tag,tagged_member_struct_type);
return;}{void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(_tmp51A,& ptr_type,&
is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp51A,_tmp51B,
_tmp51C,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp51A);Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp51A);Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp51C);{int done=0;if(_tmp51B != 0){void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;switch((enum Cyc_Absyn_Primop)_tmp51B->v){case Cyc_Absyn_Plus: _LL2AC:
change=_tmp51C;break;case Cyc_Absyn_Minus: _LL2AD: change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,
_tmp51C,0);break;default: _LL2AE: {const char*_tmpC83;void*_tmpC82;(_tmpC82=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC83="bad t ? pointer arithmetic",_tag_dyneither(_tmpC83,sizeof(char),27))),
_tag_dyneither(_tmpC82,sizeof(void*),0)));}}done=1;{struct Cyc_Absyn_Exp*_tmp5E0=
Cyc_Toc__dyneither_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*_tmpC84[3];e->r=Cyc_Toc_fncall_exp_r(
_tmp5E0,((_tmpC84[2]=change,((_tmpC84[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpC84[0]=Cyc_Toc_push_address_exp(_tmp51A),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC84,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))
switch((enum Cyc_Absyn_Primop)_tmp51B->v){case Cyc_Absyn_Plus: _LL2B0: done=1;{
struct Cyc_Absyn_Exp*_tmpC85[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp51A),((_tmpC85[1]=_tmp51C,((
_tmpC85[0]=_tmp51A,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC85,sizeof(struct Cyc_Absyn_Exp*),2)))))));}break;default:
_LL2B1: {const char*_tmpC88;void*_tmpC87;(_tmpC87=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC88="bad zero-terminated pointer arithmetic",
_tag_dyneither(_tmpC88,sizeof(char),39))),_tag_dyneither(_tmpC87,sizeof(void*),0)));}}}}
if(!done){if(e1_poly)_tmp51C->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp51C->r,0));if(!Cyc_Absyn_is_lvalue(_tmp51A)){{struct _tuple20 _tmpC8B;struct
_tuple20*_tmpC8A;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple20*),struct _tuple20*f_env))
Cyc_Toc_lvalue_assign)(_tmp51A,0,Cyc_Toc_assignop_lvalue,((_tmpC8A=_cycalloc(
sizeof(struct _tuple20)* 1),((_tmpC8A[0]=((_tmpC8B.f1=_tmp51B,((_tmpC8B.f2=
_tmp51C,_tmpC8B)))),_tmpC8A)))));}e->r=_tmp51A->r;}}goto _LL229;};};};}_LL238: {
struct Cyc_Absyn_Conditional_e_struct*_tmp51D=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp509;if(_tmp51D->tag != 6)goto _LL23A;else{_tmp51E=_tmp51D->f1;_tmp51F=_tmp51D->f2;
_tmp520=_tmp51D->f3;}}_LL239: Cyc_Toc_exp_to_c(nv,_tmp51E);Cyc_Toc_exp_to_c(nv,
_tmp51F);Cyc_Toc_exp_to_c(nv,_tmp520);goto _LL229;_LL23A: {struct Cyc_Absyn_And_e_struct*
_tmp521=(struct Cyc_Absyn_And_e_struct*)_tmp509;if(_tmp521->tag != 7)goto _LL23C;
else{_tmp522=_tmp521->f1;_tmp523=_tmp521->f2;}}_LL23B: Cyc_Toc_exp_to_c(nv,
_tmp522);Cyc_Toc_exp_to_c(nv,_tmp523);goto _LL229;_LL23C: {struct Cyc_Absyn_Or_e_struct*
_tmp524=(struct Cyc_Absyn_Or_e_struct*)_tmp509;if(_tmp524->tag != 8)goto _LL23E;
else{_tmp525=_tmp524->f1;_tmp526=_tmp524->f2;}}_LL23D: Cyc_Toc_exp_to_c(nv,
_tmp525);Cyc_Toc_exp_to_c(nv,_tmp526);goto _LL229;_LL23E: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp527=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp509;if(_tmp527->tag != 9)goto _LL240;
else{_tmp528=_tmp527->f1;_tmp529=_tmp527->f2;}}_LL23F: Cyc_Toc_exp_to_c(nv,
_tmp528);Cyc_Toc_exp_to_c(nv,_tmp529);goto _LL229;_LL240: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp52A=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp509;if(_tmp52A->tag != 10)goto
_LL242;else{_tmp52B=_tmp52A->f1;_tmp52C=_tmp52A->f2;}}_LL241: _tmp52E=_tmp52B;
_tmp52F=_tmp52C;goto _LL243;_LL242: {struct Cyc_Absyn_FnCall_e_struct*_tmp52D=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp509;if(_tmp52D->tag != 11)goto _LL244;else{
_tmp52E=_tmp52D->f1;_tmp52F=_tmp52D->f2;_tmp530=_tmp52D->f3;if(_tmp530 != 0)goto
_LL244;}}_LL243: Cyc_Toc_exp_to_c(nv,_tmp52E);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp52F);goto _LL229;_LL244: {struct Cyc_Absyn_FnCall_e_struct*
_tmp531=(struct Cyc_Absyn_FnCall_e_struct*)_tmp509;if(_tmp531->tag != 11)goto
_LL246;else{_tmp532=_tmp531->f1;_tmp533=_tmp531->f2;_tmp534=_tmp531->f3;if(
_tmp534 == 0)goto _LL246;_tmp535=*_tmp534;_tmp536=_tmp535.num_varargs;_tmp537=
_tmp535.injectors;_tmp538=_tmp535.vai;}}_LL245: {struct Cyc_Toc_Env _tmp5E8;struct
_RegionHandle*_tmp5E9;struct Cyc_Toc_Env*_tmp5E7=nv;_tmp5E8=*_tmp5E7;_tmp5E9=
_tmp5E8.rgn;{struct _tuple0*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=
Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp536,0);void*cva_type=Cyc_Toc_typ_to_c(_tmp538->type);void*
arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp533);int num_normargs=num_args - _tmp536;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp533=_tmp533->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp533))->hd);{
struct Cyc_List_List*_tmpC8C;new_args=((_tmpC8C=_cycalloc(sizeof(*_tmpC8C)),((
_tmpC8C->hd=(struct Cyc_Absyn_Exp*)_tmp533->hd,((_tmpC8C->tl=new_args,_tmpC8C))))));};}}{
struct Cyc_Absyn_Exp*_tmpC8F[3];struct Cyc_List_List*_tmpC8E;new_args=((_tmpC8E=
_cycalloc(sizeof(*_tmpC8E)),((_tmpC8E->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((
_tmpC8F[2]=num_varargs_exp,((_tmpC8F[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0),((
_tmpC8F[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC8F,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpC8E->tl=
new_args,_tmpC8E))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp532);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp532,new_args,0),0);if(_tmp538->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp5ED=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp538->type));struct Cyc_Absyn_DatatypeInfo _tmp5EF;union Cyc_Absyn_DatatypeInfoU
_tmp5F0;struct Cyc_Absyn_Datatypedecl**_tmp5F1;struct Cyc_Absyn_Datatypedecl*
_tmp5F2;_LL2B4: {struct Cyc_Absyn_DatatypeType_struct*_tmp5EE=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp5ED;if(_tmp5EE->tag != 3)goto _LL2B6;else{_tmp5EF=_tmp5EE->f1;_tmp5F0=_tmp5EF.datatype_info;
if((_tmp5F0.KnownDatatype).tag != 2)goto _LL2B6;_tmp5F1=(struct Cyc_Absyn_Datatypedecl**)(
_tmp5F0.KnownDatatype).val;_tmp5F2=*_tmp5F1;}}_LL2B5: tud=_tmp5F2;goto _LL2B3;
_LL2B6:;_LL2B7: {const char*_tmpC92;void*_tmpC91;(_tmpC91=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC92="toc: unknown datatype in vararg with inject",
_tag_dyneither(_tmpC92,sizeof(char),44))),_tag_dyneither(_tmpC91,sizeof(void*),0)));}
_LL2B3:;}{unsigned int _tmpC97;unsigned int _tmpC96;struct _dyneither_ptr _tmpC95;
struct _tuple0**_tmpC94;unsigned int _tmpC93;struct _dyneither_ptr vs=(_tmpC93=(
unsigned int)_tmp536,((_tmpC94=(struct _tuple0**)_region_malloc(_tmp5E9,
_check_times(sizeof(struct _tuple0*),_tmpC93)),((_tmpC95=_tag_dyneither(_tmpC94,
sizeof(struct _tuple0*),_tmpC93),((((_tmpC96=_tmpC93,_tmpC98(& _tmpC97,& _tmpC96,&
_tmpC94))),_tmpC95)))))));if(_tmp536 != 0){struct Cyc_List_List*_tmp5F5=0;{int i=
_tmp536 - 1;for(0;i >= 0;-- i){struct Cyc_List_List*_tmpC99;_tmp5F5=((_tmpC99=
_cycalloc(sizeof(*_tmpC99)),((_tmpC99->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct
_tuple0*),i)),0),0)),((_tmpC99->tl=_tmp5F5,_tmpC99))))));}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp5F5,0),s,0);{
int i=0;for(0;_tmp533 != 0;(((_tmp533=_tmp533->tl,_tmp537=_tmp537->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp533->hd;void*arg_type=(void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct _tuple0*var=*((struct
_tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield _tmp5F8;struct
_tuple0*_tmp5F9;struct Cyc_List_List*_tmp5FA;struct Cyc_Absyn_Datatypefield*
_tmp5F7=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp537))->hd;_tmp5F8=*_tmp5F7;_tmp5F9=_tmp5F8.name;_tmp5FA=_tmp5F8.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp5FA))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_boxed_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp5F9),0),s,0);{const char*_tmpC9A;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp5F9,((_tmpC9A="_struct",_tag_dyneither(_tmpC9A,
sizeof(char),8))))),0,s,0);};};}};}else{struct _tuple16*_tmpC9B[3];struct Cyc_List_List*
_tmp5FC=(_tmpC9B[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC9B[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpC9B[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC9B,sizeof(struct _tuple16*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{{int i=0;for(0;_tmp533
!= 0;(_tmp533=_tmp533->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp533->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp533->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL229;}_LL246: {struct Cyc_Absyn_Throw_e_struct*_tmp539=(struct Cyc_Absyn_Throw_e_struct*)
_tmp509;if(_tmp539->tag != 12)goto _LL248;else{_tmp53A=_tmp539->f1;}}_LL247: Cyc_Toc_exp_to_c(
nv,_tmp53A);e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(
_tmp53A),0))->r;goto _LL229;_LL248: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp53B=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp509;if(_tmp53B->tag != 13)
goto _LL24A;else{_tmp53C=_tmp53B->f1;}}_LL249: Cyc_Toc_exp_to_c(nv,_tmp53C);goto
_LL229;_LL24A: {struct Cyc_Absyn_Instantiate_e_struct*_tmp53D=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp509;if(_tmp53D->tag != 14)goto _LL24C;else{_tmp53E=_tmp53D->f1;_tmp53F=_tmp53D->f2;}}
_LL24B: Cyc_Toc_exp_to_c(nv,_tmp53E);for(0;_tmp53F != 0;_tmp53F=_tmp53F->tl){enum 
Cyc_Absyn_KindQual _tmp602=(Cyc_Tcutil_typ_kind((void*)_tmp53F->hd))->kind;if(
_tmp602 != Cyc_Absyn_EffKind  && _tmp602 != Cyc_Absyn_RgnKind){{void*_tmp603=Cyc_Tcutil_compress((
void*)_tmp53F->hd);_LL2B9: {struct Cyc_Absyn_VarType_struct*_tmp604=(struct Cyc_Absyn_VarType_struct*)
_tmp603;if(_tmp604->tag != 2)goto _LL2BB;}_LL2BA: goto _LL2BC;_LL2BB: {struct Cyc_Absyn_DatatypeType_struct*
_tmp605=(struct Cyc_Absyn_DatatypeType_struct*)_tmp603;if(_tmp605->tag != 3)goto
_LL2BD;}_LL2BC: continue;_LL2BD:;_LL2BE: e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp53E,0))->r;goto _LL2B8;
_LL2B8:;}break;}}goto _LL229;_LL24C: {struct Cyc_Absyn_Cast_e_struct*_tmp540=(
struct Cyc_Absyn_Cast_e_struct*)_tmp509;if(_tmp540->tag != 15)goto _LL24E;else{
_tmp541=(void**)& _tmp540->f1;_tmp542=(void**)((void**)& _tmp540->f1);_tmp543=
_tmp540->f2;_tmp544=_tmp540->f3;_tmp545=_tmp540->f4;}}_LL24D: {void*old_t2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp543->topt))->v;void*new_typ=*_tmp542;*
_tmp542=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp543);{struct _tuple19
_tmpC9C;struct _tuple19 _tmp607=(_tmpC9C.f1=Cyc_Tcutil_compress(old_t2),((_tmpC9C.f2=
Cyc_Tcutil_compress(new_typ),_tmpC9C)));void*_tmp608;struct Cyc_Absyn_PtrInfo
_tmp60A;void*_tmp60B;struct Cyc_Absyn_PtrInfo _tmp60D;void*_tmp60E;struct Cyc_Absyn_PtrInfo
_tmp610;void*_tmp611;_LL2C0: _tmp608=_tmp607.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp609=(struct Cyc_Absyn_PointerType_struct*)_tmp608;if(_tmp609->tag != 5)goto
_LL2C2;else{_tmp60A=_tmp609->f1;}};_tmp60B=_tmp607.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp60C=(struct Cyc_Absyn_PointerType_struct*)_tmp60B;if(_tmp60C->tag != 5)goto
_LL2C2;else{_tmp60D=_tmp60C->f1;}};_LL2C1: {int _tmp613=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp60A.ptr_atts).nullable);int _tmp614=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp60D.ptr_atts).nullable);
void*_tmp615=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp60A.ptr_atts).bounds);void*_tmp616=((void*(*)(void*y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp60D.ptr_atts).bounds);
int _tmp617=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp60A.ptr_atts).zero_term);int _tmp618=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp60D.ptr_atts).zero_term);{struct _tuple19 _tmpC9D;
struct _tuple19 _tmp61A=(_tmpC9D.f1=_tmp615,((_tmpC9D.f2=_tmp616,_tmpC9D)));void*
_tmp61B;struct Cyc_Absyn_Exp*_tmp61D;void*_tmp61E;struct Cyc_Absyn_Exp*_tmp620;
void*_tmp621;struct Cyc_Absyn_Exp*_tmp623;void*_tmp624;void*_tmp626;void*_tmp628;
struct Cyc_Absyn_Exp*_tmp62A;void*_tmp62B;void*_tmp62D;_LL2C7: _tmp61B=_tmp61A.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp61C=(struct Cyc_Absyn_Upper_b_struct*)_tmp61B;
if(_tmp61C->tag != 1)goto _LL2C9;else{_tmp61D=_tmp61C->f1;}};_tmp61E=_tmp61A.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp61F=(struct Cyc_Absyn_Upper_b_struct*)_tmp61E;
if(_tmp61F->tag != 1)goto _LL2C9;else{_tmp620=_tmp61F->f1;}};_LL2C8: if((!Cyc_Evexp_c_can_eval(
_tmp61D) || !Cyc_Evexp_c_can_eval(_tmp620)) && !Cyc_Evexp_same_const_exp(_tmp61D,
_tmp620)){const char*_tmpCA0;void*_tmpC9F;(_tmpC9F=0,Cyc_Tcutil_terr(e->loc,((
_tmpCA0="can't validate cast due to potential size differences",_tag_dyneither(
_tmpCA0,sizeof(char),54))),_tag_dyneither(_tmpC9F,sizeof(void*),0)));}if(_tmp613
 && !_tmp614){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpCA3;void*_tmpCA2;(
_tmpCA2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpCA3="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpCA3,
sizeof(char),44))),_tag_dyneither(_tmpCA2,sizeof(void*),0)));}if(_tmp545 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpCA7;void*_tmpCA6[1];struct Cyc_String_pa_struct _tmpCA5;(_tmpCA5.tag=
0,((_tmpCA5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpCA6[0]=& _tmpCA5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpCA7="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpCA7,sizeof(char),41))),_tag_dyneither(_tmpCA6,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp543);if(do_null_check){if(!_tmp544){
const char*_tmpCAA;void*_tmpCA9;(_tmpCA9=0,Cyc_Tcutil_warn(e->loc,((_tmpCAA="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpCAA,sizeof(char),58))),_tag_dyneither(_tmpCA9,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpCAB;e->r=Cyc_Toc_cast_it_r(*_tmp542,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpCAB=_cycalloc(sizeof(*_tmpCAB)),((_tmpCAB->hd=
_tmp543,((_tmpCAB->tl=0,_tmpCAB)))))),0));};}};}goto _LL2C6;_LL2C9: _tmp621=
_tmp61A.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp622=(struct Cyc_Absyn_Upper_b_struct*)
_tmp621;if(_tmp622->tag != 1)goto _LL2CB;else{_tmp623=_tmp622->f1;}};_tmp624=
_tmp61A.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp625=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp624;if(_tmp625->tag != 0)goto _LL2CB;};_LL2CA: if(!Cyc_Evexp_c_can_eval(_tmp623)){
const char*_tmpCAE;void*_tmpCAD;(_tmpCAD=0,Cyc_Tcutil_terr(e->loc,((_tmpCAE="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpCAE,sizeof(char),71))),_tag_dyneither(_tmpCAD,sizeof(void*),0)));}
if(_tmp545 == Cyc_Absyn_NonNull_to_Null){const char*_tmpCB2;void*_tmpCB1[1];struct
Cyc_String_pa_struct _tmpCB0;(_tmpCB0.tag=0,((_tmpCB0.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCB1[0]=& _tmpCB0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCB2="conversion mis-classified as null-check: %s",
_tag_dyneither(_tmpCB2,sizeof(char),44))),_tag_dyneither(_tmpCB1,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){if((_tmp617  && !(_tmp60D.elt_tq).real_const) && !
_tmp618)_tmp623=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp623,Cyc_Absyn_uint_exp(1,
0),0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp623,_tmp543))->r;}else{
struct Cyc_Absyn_Exp*_tmp63E=Cyc_Toc__tag_dyneither_e;if(_tmp617){struct _tuple0*
_tmp63F=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp640=Cyc_Absyn_var_exp(_tmp63F,
0);struct Cyc_Absyn_Exp*arg3;{void*_tmp641=_tmp543->r;union Cyc_Absyn_Cnst _tmp643;
struct _dyneither_ptr _tmp644;union Cyc_Absyn_Cnst _tmp646;struct _dyneither_ptr
_tmp647;_LL2D0: {struct Cyc_Absyn_Const_e_struct*_tmp642=(struct Cyc_Absyn_Const_e_struct*)
_tmp641;if(_tmp642->tag != 0)goto _LL2D2;else{_tmp643=_tmp642->f1;if((_tmp643.String_c).tag
!= 8)goto _LL2D2;_tmp644=(struct _dyneither_ptr)(_tmp643.String_c).val;}}_LL2D1:
arg3=_tmp623;goto _LL2CF;_LL2D2: {struct Cyc_Absyn_Const_e_struct*_tmp645=(struct
Cyc_Absyn_Const_e_struct*)_tmp641;if(_tmp645->tag != 0)goto _LL2D4;else{_tmp646=
_tmp645->f1;if((_tmp646.Wstring_c).tag != 9)goto _LL2D4;_tmp647=(struct
_dyneither_ptr)(_tmp646.Wstring_c).val;}}_LL2D3: arg3=_tmp623;goto _LL2CF;_LL2D4:;
_LL2D5:{struct Cyc_Absyn_Exp*_tmpCB3[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp543),((_tmpCB3[1]=_tmp623,((_tmpCB3[0]=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp640),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB3,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);}goto _LL2CF;_LL2CF:;}if(!_tmp618  && !(_tmp60D.elt_tq).real_const)arg3=
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*
_tmp649=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp60D.elt_typ),0);struct Cyc_Absyn_Exp*
_tmpCB4[3];struct Cyc_Absyn_Exp*_tmp64A=Cyc_Absyn_fncall_exp(_tmp63E,((_tmpCB4[2]=
arg3,((_tmpCB4[1]=_tmp649,((_tmpCB4[0]=_tmp640,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB4,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);struct Cyc_Absyn_Stmt*_tmp64B=Cyc_Absyn_exp_stmt(_tmp64A,0);_tmp64B=
Cyc_Absyn_declare_stmt(_tmp63F,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)
_tmp543,_tmp64B,0);e->r=Cyc_Toc_stmt_exp_r(_tmp64B);};}else{struct Cyc_Absyn_Exp*
_tmpCB5[3];e->r=Cyc_Toc_fncall_exp_r(_tmp63E,((_tmpCB5[2]=_tmp623,((_tmpCB5[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp60D.elt_typ),0),((_tmpCB5[0]=
_tmp543,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCB5,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2C6;
_LL2CB: _tmp626=_tmp61A.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp627=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp626;if(_tmp627->tag != 0)goto _LL2CD;};_tmp628=_tmp61A.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp629=(struct Cyc_Absyn_Upper_b_struct*)_tmp628;if(_tmp629->tag != 1)goto _LL2CD;
else{_tmp62A=_tmp629->f1;}};_LL2CC: if(!Cyc_Evexp_c_can_eval(_tmp62A)){const char*
_tmpCB8;void*_tmpCB7;(_tmpCB7=0,Cyc_Tcutil_terr(e->loc,((_tmpCB8="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpCB8,sizeof(char),71))),_tag_dyneither(_tmpCB7,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpCBB;void*_tmpCBA;(_tmpCBA=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCBB="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpCBB,sizeof(char),45))),_tag_dyneither(_tmpCBA,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp652=_tmp62A;if(_tmp617  && !_tmp618)_tmp652=Cyc_Absyn_add_exp(
_tmp62A,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp653=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpCBC[3];struct Cyc_Absyn_Exp*_tmp654=Cyc_Absyn_fncall_exp(
_tmp653,((_tmpCBC[2]=_tmp652,((_tmpCBC[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp60A.elt_typ),0),((_tmpCBC[0]=_tmp543,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCBC,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp614){struct Cyc_List_List*_tmpCBD;_tmp654->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_null_e,((_tmpCBD=_cycalloc(sizeof(*_tmpCBD)),((_tmpCBD->hd=Cyc_Absyn_copy_exp(
_tmp654),((_tmpCBD->tl=0,_tmpCBD)))))));}e->r=Cyc_Toc_cast_it_r(*_tmp542,_tmp654);
goto _LL2C6;};};_LL2CD: _tmp62B=_tmp61A.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp62C=(struct Cyc_Absyn_DynEither_b_struct*)_tmp62B;if(_tmp62C->tag != 0)goto
_LL2C6;};_tmp62D=_tmp61A.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp62E=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp62D;if(_tmp62E->tag != 0)goto _LL2C6;};_LL2CE:
DynCast: if((_tmp617  && !_tmp618) && !(_tmp60D.elt_tq).real_const){if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpCC0;void*_tmpCBF;(_tmpCBF=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCC0="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpCC0,sizeof(char),70))),_tag_dyneither(_tmpCBF,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp659=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpCC1[3];e->r=Cyc_Toc_fncall_exp_r(_tmp659,((_tmpCC1[2]=Cyc_Absyn_uint_exp(1,0),((
_tmpCC1[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp60A.elt_typ),0),((
_tmpCC1[0]=_tmp543,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCC1,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}goto _LL2C6;
_LL2C6:;}goto _LL2BF;}_LL2C2: _tmp60E=_tmp607.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp60F=(struct Cyc_Absyn_PointerType_struct*)_tmp60E;if(_tmp60F->tag != 5)goto
_LL2C4;else{_tmp610=_tmp60F->f1;}};_tmp611=_tmp607.f2;{struct Cyc_Absyn_IntType_struct*
_tmp612=(struct Cyc_Absyn_IntType_struct*)_tmp611;if(_tmp612->tag != 6)goto _LL2C4;};
_LL2C3:{void*_tmp65B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp610.ptr_atts).bounds);_LL2D7: {struct Cyc_Absyn_DynEither_b_struct*
_tmp65C=(struct Cyc_Absyn_DynEither_b_struct*)_tmp65B;if(_tmp65C->tag != 0)goto
_LL2D9;}_LL2D8: _tmp543->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp543->r,
_tmp543->loc),Cyc_Toc_curr_sp);goto _LL2D6;_LL2D9:;_LL2DA: goto _LL2D6;_LL2D6:;}
goto _LL2BF;_LL2C4:;_LL2C5: goto _LL2BF;_LL2BF:;}goto _LL229;}_LL24E: {struct Cyc_Absyn_Address_e_struct*
_tmp546=(struct Cyc_Absyn_Address_e_struct*)_tmp509;if(_tmp546->tag != 16)goto
_LL250;else{_tmp547=_tmp546->f1;}}_LL24F:{void*_tmp65D=_tmp547->r;struct _tuple0*
_tmp65F;struct Cyc_List_List*_tmp660;struct Cyc_List_List*_tmp661;struct Cyc_List_List*
_tmp663;_LL2DC: {struct Cyc_Absyn_Aggregate_e_struct*_tmp65E=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp65D;if(_tmp65E->tag != 29)goto _LL2DE;else{_tmp65F=_tmp65E->f1;_tmp660=_tmp65E->f2;
_tmp661=_tmp65E->f3;}}_LL2DD: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpCC5;void*
_tmpCC4[1];struct Cyc_String_pa_struct _tmpCC3;(_tmpCC3.tag=0,((_tmpCC3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp547->loc)),((
_tmpCC4[0]=& _tmpCC3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpCC5="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpCC5,
sizeof(char),42))),_tag_dyneither(_tmpCC4,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp547->topt))->v,_tmp660 != 0,1,0,
_tmp661,_tmp65F))->r;goto _LL2DB;_LL2DE: {struct Cyc_Absyn_Tuple_e_struct*_tmp662=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp65D;if(_tmp662->tag != 25)goto _LL2E0;else{
_tmp663=_tmp662->f1;}}_LL2DF: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpCC9;void*
_tmpCC8[1];struct Cyc_String_pa_struct _tmpCC7;(_tmpCC7.tag=0,((_tmpCC7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp547->loc)),((
_tmpCC8[0]=& _tmpCC7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpCC9="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpCC9,
sizeof(char),42))),_tag_dyneither(_tmpCC8,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_tuple(
nv,1,0,_tmp663))->r;goto _LL2DB;_LL2E0:;_LL2E1: Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(
nv,_tmp547);Cyc_Toc_set_lhs(nv,0);if(!Cyc_Absyn_is_lvalue(_tmp547)){((void(*)(
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
int),int f_env))Cyc_Toc_lvalue_assign)(_tmp547,0,Cyc_Toc_address_lvalue,1);e->r=
Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmp547);}goto _LL2DB;_LL2DB:;}goto _LL229;_LL250: {struct Cyc_Absyn_New_e_struct*
_tmp548=(struct Cyc_Absyn_New_e_struct*)_tmp509;if(_tmp548->tag != 17)goto _LL252;
else{_tmp549=_tmp548->f1;_tmp54A=_tmp548->f2;}}_LL251: if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCCD;void*_tmpCCC[1];struct Cyc_String_pa_struct _tmpCCB;(_tmpCCB.tag=
0,((_tmpCCB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
_tmp54A->loc)),((_tmpCCC[0]=& _tmpCCB,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCCD="%s: new at top-level",_tag_dyneither(
_tmpCCD,sizeof(char),21))),_tag_dyneither(_tmpCCC,sizeof(void*),1)))))));}{void*
_tmp66D=_tmp54A->r;struct Cyc_List_List*_tmp66F;struct Cyc_Absyn_Vardecl*_tmp671;
struct Cyc_Absyn_Exp*_tmp672;struct Cyc_Absyn_Exp*_tmp673;int _tmp674;struct _tuple0*
_tmp676;struct Cyc_List_List*_tmp677;struct Cyc_List_List*_tmp678;struct Cyc_Absyn_Aggrdecl*
_tmp679;struct Cyc_List_List*_tmp67B;_LL2E3: {struct Cyc_Absyn_Array_e_struct*
_tmp66E=(struct Cyc_Absyn_Array_e_struct*)_tmp66D;if(_tmp66E->tag != 27)goto _LL2E5;
else{_tmp66F=_tmp66E->f1;}}_LL2E4: {struct _tuple0*_tmp67C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp67D=Cyc_Absyn_var_exp(_tmp67C,0);struct Cyc_Absyn_Stmt*
_tmp67E=Cyc_Toc_init_array(nv,_tmp67D,_tmp66F,Cyc_Absyn_exp_stmt(_tmp67D,0));
void*old_elt_typ;{void*_tmp67F=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp681;void*_tmp682;struct Cyc_Absyn_Tqual _tmp683;struct Cyc_Absyn_PtrAtts _tmp684;
union Cyc_Absyn_Constraint*_tmp685;_LL2EE: {struct Cyc_Absyn_PointerType_struct*
_tmp680=(struct Cyc_Absyn_PointerType_struct*)_tmp67F;if(_tmp680->tag != 5)goto
_LL2F0;else{_tmp681=_tmp680->f1;_tmp682=_tmp681.elt_typ;_tmp683=_tmp681.elt_tq;
_tmp684=_tmp681.ptr_atts;_tmp685=_tmp684.zero_term;}}_LL2EF: old_elt_typ=_tmp682;
goto _LL2ED;_LL2F0:;_LL2F1: {const char*_tmpCD0;void*_tmpCCF;old_elt_typ=((_tmpCCF=
0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCD0="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(
_tmpCD0,sizeof(char),52))),_tag_dyneither(_tmpCCF,sizeof(void*),0))));}_LL2ED:;}{
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp688=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp689=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp66F),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp549 == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp689);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp549;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp689);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp67C,_tmp688,(struct Cyc_Absyn_Exp*)e1,_tmp67E,0));goto
_LL2E2;};}_LL2E5: {struct Cyc_Absyn_Comprehension_e_struct*_tmp670=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp66D;if(_tmp670->tag != 28)goto _LL2E7;else{_tmp671=_tmp670->f1;_tmp672=_tmp670->f2;
_tmp673=_tmp670->f3;_tmp674=_tmp670->f4;}}_LL2E6: {int is_dyneither_ptr=0;{void*
_tmp68A=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp68C;void*_tmp68D;
struct Cyc_Absyn_Tqual _tmp68E;struct Cyc_Absyn_PtrAtts _tmp68F;union Cyc_Absyn_Constraint*
_tmp690;union Cyc_Absyn_Constraint*_tmp691;_LL2F3: {struct Cyc_Absyn_PointerType_struct*
_tmp68B=(struct Cyc_Absyn_PointerType_struct*)_tmp68A;if(_tmp68B->tag != 5)goto
_LL2F5;else{_tmp68C=_tmp68B->f1;_tmp68D=_tmp68C.elt_typ;_tmp68E=_tmp68C.elt_tq;
_tmp68F=_tmp68C.ptr_atts;_tmp690=_tmp68F.bounds;_tmp691=_tmp68F.zero_term;}}
_LL2F4: is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp690)== (void*)& Cyc_Absyn_DynEither_b_val;goto _LL2F2;
_LL2F5:;_LL2F6: {const char*_tmpCD3;void*_tmpCD2;(_tmpCD2=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD3="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpCD3,sizeof(char),42))),_tag_dyneither(_tmpCD2,sizeof(void*),0)));}
_LL2F2:;}{struct _tuple0*max=Cyc_Toc_temp_var();struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp673->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp672);{struct Cyc_Absyn_Exp*_tmp694=
Cyc_Absyn_var_exp(max,0);if(_tmp674)_tmp694=Cyc_Absyn_add_exp(_tmp694,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp671,Cyc_Absyn_var_exp(max,0),_tmp673,_tmp674,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp696;struct _RegionHandle*_tmp697;struct Cyc_Toc_Env*_tmp695=
nv;_tmp696=*_tmp695;_tmp697=_tmp696.rgn;{struct _tuple21*_tmpCD6;struct Cyc_List_List*
_tmpCD5;struct Cyc_List_List*decls=(_tmpCD5=_region_malloc(_tmp697,sizeof(*
_tmpCD5)),((_tmpCD5->hd=((_tmpCD6=_region_malloc(_tmp697,sizeof(*_tmpCD6)),((
_tmpCD6->f1=max,((_tmpCD6->f2=Cyc_Absyn_uint_typ,((_tmpCD6->f3=(struct Cyc_Absyn_Exp*)
_tmp672,_tmpCD6)))))))),((_tmpCD5->tl=0,_tmpCD5)))));struct Cyc_Absyn_Exp*ai;if(
_tmp549 == 0  || Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*_tmpCD7[2];ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((_tmpCD7[1]=_tmp694,((
_tmpCD7[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD7,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp549;Cyc_Toc_exp_to_c(
nv,r);{struct Cyc_Absyn_Exp*_tmpCD8[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpCD8[1]=_tmp694,((_tmpCD8[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCD8,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct _tuple21*_tmpCDB;struct Cyc_List_List*
_tmpCDA;decls=((_tmpCDA=_region_malloc(_tmp697,sizeof(*_tmpCDA)),((_tmpCDA->hd=((
_tmpCDB=_region_malloc(_tmp697,sizeof(*_tmpCDB)),((_tmpCDB->f1=a,((_tmpCDB->f2=
ptr_typ,((_tmpCDB->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpCDB)))))))),((_tmpCDA->tl=
decls,_tmpCDA))))));}if(is_dyneither_ptr){struct _tuple0*_tmp69C=Cyc_Toc_temp_var();
void*_tmp69D=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp69E=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpCDC[3];struct Cyc_Absyn_Exp*_tmp69F=Cyc_Absyn_fncall_exp(
_tmp69E,((_tmpCDC[2]=_tmp694,((_tmpCDC[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((
_tmpCDC[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpCDC,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{
struct _tuple21*_tmpCDF;struct Cyc_List_List*_tmpCDE;decls=((_tmpCDE=
_region_malloc(_tmp697,sizeof(*_tmpCDE)),((_tmpCDE->hd=((_tmpCDF=_region_malloc(
_tmp697,sizeof(*_tmpCDF)),((_tmpCDF->f1=_tmp69C,((_tmpCDF->f2=_tmp69D,((_tmpCDF->f3=(
struct Cyc_Absyn_Exp*)_tmp69F,_tmpCDF)))))))),((_tmpCDE->tl=decls,_tmpCDE))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp69C,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp6A3=decls;for(0;_tmp6A3 != 0;_tmp6A3=_tmp6A3->tl){struct
_tuple0*_tmp6A5;void*_tmp6A6;struct Cyc_Absyn_Exp*_tmp6A7;struct _tuple21 _tmp6A4=*((
struct _tuple21*)_tmp6A3->hd);_tmp6A5=_tmp6A4.f1;_tmp6A6=_tmp6A4.f2;_tmp6A7=
_tmp6A4.f3;s=Cyc_Absyn_declare_stmt(_tmp6A5,_tmp6A6,_tmp6A7,s,0);}}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL2E2;};};};}_LL2E7: {struct Cyc_Absyn_Aggregate_e_struct*_tmp675=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp66D;if(_tmp675->tag != 29)goto _LL2E9;
else{_tmp676=_tmp675->f1;_tmp677=_tmp675->f2;_tmp678=_tmp675->f3;_tmp679=_tmp675->f4;}}
_LL2E8: e->r=(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp54A->topt))->v,_tmp677 != 0,1,_tmp549,_tmp678,_tmp676))->r;goto _LL2E2;_LL2E9: {
struct Cyc_Absyn_Tuple_e_struct*_tmp67A=(struct Cyc_Absyn_Tuple_e_struct*)_tmp66D;
if(_tmp67A->tag != 25)goto _LL2EB;else{_tmp67B=_tmp67A->f1;}}_LL2EA: e->r=(Cyc_Toc_init_tuple(
nv,1,_tmp549,_tmp67B))->r;goto _LL2E2;_LL2EB:;_LL2EC: {void*old_elt_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp54A->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);struct _tuple0*_tmp6AA=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp6AB=Cyc_Absyn_var_exp(_tmp6AA,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(_tmp6AB,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp549
== 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp549;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp6AC=_tmp54A->r;void*_tmp6AE;
struct Cyc_Absyn_Exp*_tmp6AF;_LL2F8: {struct Cyc_Absyn_Cast_e_struct*_tmp6AD=(
struct Cyc_Absyn_Cast_e_struct*)_tmp6AC;if(_tmp6AD->tag != 15)goto _LL2FA;else{
_tmp6AE=(void*)_tmp6AD->f1;_tmp6AF=_tmp6AD->f2;}}_LL2F9:{struct _tuple19 _tmpCE0;
struct _tuple19 _tmp6B1=(_tmpCE0.f1=Cyc_Tcutil_compress(_tmp6AE),((_tmpCE0.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp6AF->topt))->v),_tmpCE0)));void*
_tmp6B2;struct Cyc_Absyn_PtrInfo _tmp6B4;void*_tmp6B5;struct Cyc_Absyn_PtrAtts
_tmp6B6;union Cyc_Absyn_Constraint*_tmp6B7;void*_tmp6B8;struct Cyc_Absyn_PtrInfo
_tmp6BA;struct Cyc_Absyn_PtrAtts _tmp6BB;union Cyc_Absyn_Constraint*_tmp6BC;_LL2FD:
_tmp6B2=_tmp6B1.f1;{struct Cyc_Absyn_PointerType_struct*_tmp6B3=(struct Cyc_Absyn_PointerType_struct*)
_tmp6B2;if(_tmp6B3->tag != 5)goto _LL2FF;else{_tmp6B4=_tmp6B3->f1;_tmp6B5=_tmp6B4.elt_typ;
_tmp6B6=_tmp6B4.ptr_atts;_tmp6B7=_tmp6B6.bounds;}};_tmp6B8=_tmp6B1.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp6B9=(struct Cyc_Absyn_PointerType_struct*)_tmp6B8;if(_tmp6B9->tag != 5)goto
_LL2FF;else{_tmp6BA=_tmp6B9->f1;_tmp6BB=_tmp6BA.ptr_atts;_tmp6BC=_tmp6BB.bounds;}};
_LL2FE:{struct _tuple19 _tmpCE1;struct _tuple19 _tmp6BE=(_tmpCE1.f1=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6B7),((_tmpCE1.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp6BC),_tmpCE1)));void*_tmp6BF;void*_tmp6C1;struct Cyc_Absyn_Exp*
_tmp6C3;_LL302: _tmp6BF=_tmp6BE.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp6C0=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp6BF;if(_tmp6C0->tag != 0)goto _LL304;};
_tmp6C1=_tmp6BE.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp6C2=(struct Cyc_Absyn_Upper_b_struct*)
_tmp6C1;if(_tmp6C2->tag != 1)goto _LL304;else{_tmp6C3=_tmp6C2->f1;}};_LL303: Cyc_Toc_exp_to_c(
nv,_tmp6AF);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp6C4=Cyc_Toc__init_dyneither_ptr_e;{struct Cyc_Absyn_Exp*_tmpCE2[4];e->r=Cyc_Toc_fncall_exp_r(
_tmp6C4,((_tmpCE2[3]=_tmp6C3,((_tmpCE2[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp6B5),0),((_tmpCE2[1]=_tmp6AF,((_tmpCE2[0]=mexp,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE2,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))));}goto _LL301;};_LL304:;_LL305: goto _LL301;_LL301:;}goto _LL2FC;_LL2FF:;
_LL300: goto _LL2FC;_LL2FC:;}goto _LL2F7;_LL2FA:;_LL2FB: goto _LL2F7;_LL2F7:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp6C6=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp6AB),
0);struct Cyc_Absyn_Exp*_tmp6C7=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,
_tmp54A);_tmp6C6=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
_tmp6AB,_tmp6C7,0),_tmp54A,0),_tmp6C6,0);{void*_tmp6C8=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp6AA,
_tmp6C8,(struct Cyc_Absyn_Exp*)mexp,_tmp6C6,0));};}goto _LL2E2;};}_LL2E2:;}goto
_LL229;_LL252: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp54B=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp509;if(_tmp54B->tag != 19)goto _LL254;else{_tmp54C=_tmp54B->f1;}}_LL253: Cyc_Toc_exp_to_c(
nv,_tmp54C);goto _LL229;_LL254: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp54D=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp509;if(_tmp54D->tag != 18)goto _LL256;
else{_tmp54E=(void*)_tmp54D->f1;}}_LL255:{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpCE5;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpCE4;e->r=(void*)((_tmpCE4=
_cycalloc(sizeof(*_tmpCE4)),((_tmpCE4[0]=((_tmpCE5.tag=18,((_tmpCE5.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp54E),_tmpCE5)))),_tmpCE4))));}goto _LL229;_LL256: {
struct Cyc_Absyn_Offsetof_e_struct*_tmp54F=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp509;if(_tmp54F->tag != 20)goto _LL258;else{_tmp550=(void*)_tmp54F->f1;_tmp551=(
void*)_tmp54F->f2;{struct Cyc_Absyn_StructField_struct*_tmp552=(struct Cyc_Absyn_StructField_struct*)
_tmp551;if(_tmp552->tag != 0)goto _LL258;else{_tmp553=_tmp552->f1;}};}}_LL257:{
struct Cyc_Absyn_Offsetof_e_struct _tmpCEF;struct Cyc_Absyn_StructField_struct
_tmpCEE;struct Cyc_Absyn_StructField_struct*_tmpCED;struct Cyc_Absyn_Offsetof_e_struct*
_tmpCEC;e->r=(void*)((_tmpCEC=_cycalloc(sizeof(*_tmpCEC)),((_tmpCEC[0]=((_tmpCEF.tag=
20,((_tmpCEF.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp550),((_tmpCEF.f2=(void*)((
void*)((_tmpCED=_cycalloc(sizeof(*_tmpCED)),((_tmpCED[0]=((_tmpCEE.tag=0,((
_tmpCEE.f1=_tmp553,_tmpCEE)))),_tmpCED))))),_tmpCEF)))))),_tmpCEC))));}goto
_LL229;_LL258: {struct Cyc_Absyn_Offsetof_e_struct*_tmp554=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp509;if(_tmp554->tag != 20)goto _LL25A;else{_tmp555=(void*)_tmp554->f1;_tmp556=(
void*)_tmp554->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp557=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp556;if(_tmp557->tag != 1)goto _LL25A;else{_tmp558=_tmp557->f1;}};}}_LL259:{
void*_tmp6CF=Cyc_Tcutil_compress(_tmp555);struct Cyc_Absyn_AggrInfo _tmp6D1;union
Cyc_Absyn_AggrInfoU _tmp6D2;struct Cyc_List_List*_tmp6D4;_LL307: {struct Cyc_Absyn_AggrType_struct*
_tmp6D0=(struct Cyc_Absyn_AggrType_struct*)_tmp6CF;if(_tmp6D0->tag != 11)goto
_LL309;else{_tmp6D1=_tmp6D0->f1;_tmp6D2=_tmp6D1.aggr_info;}}_LL308: {struct Cyc_Absyn_Aggrdecl*
_tmp6D7=Cyc_Absyn_get_known_aggrdecl(_tmp6D2);if(_tmp6D7->impl == 0){const char*
_tmpCF2;void*_tmpCF1;(_tmpCF1=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF2="struct fields must be known",
_tag_dyneither(_tmpCF2,sizeof(char),28))),_tag_dyneither(_tmpCF1,sizeof(void*),0)));}
_tmp6D4=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6D7->impl))->fields;goto
_LL30A;}_LL309: {struct Cyc_Absyn_AnonAggrType_struct*_tmp6D3=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp6CF;if(_tmp6D3->tag != 12)goto _LL30B;else{_tmp6D4=_tmp6D3->f2;}}_LL30A: {
struct Cyc_Absyn_Aggrfield*_tmp6DA=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp6D4,(int)_tmp558);{struct Cyc_Absyn_Offsetof_e_struct
_tmpCFC;struct Cyc_Absyn_StructField_struct _tmpCFB;struct Cyc_Absyn_StructField_struct*
_tmpCFA;struct Cyc_Absyn_Offsetof_e_struct*_tmpCF9;e->r=(void*)((_tmpCF9=
_cycalloc(sizeof(*_tmpCF9)),((_tmpCF9[0]=((_tmpCFC.tag=20,((_tmpCFC.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp555),((_tmpCFC.f2=(void*)((void*)((_tmpCFA=_cycalloc(
sizeof(*_tmpCFA)),((_tmpCFA[0]=((_tmpCFB.tag=0,((_tmpCFB.f1=_tmp6DA->name,
_tmpCFB)))),_tmpCFA))))),_tmpCFC)))))),_tmpCF9))));}goto _LL306;}_LL30B: {struct
Cyc_Absyn_TupleType_struct*_tmp6D5=(struct Cyc_Absyn_TupleType_struct*)_tmp6CF;
if(_tmp6D5->tag != 10)goto _LL30D;}_LL30C:{struct Cyc_Absyn_Offsetof_e_struct
_tmpD06;struct Cyc_Absyn_StructField_struct _tmpD05;struct Cyc_Absyn_StructField_struct*
_tmpD04;struct Cyc_Absyn_Offsetof_e_struct*_tmpD03;e->r=(void*)((_tmpD03=
_cycalloc(sizeof(*_tmpD03)),((_tmpD03[0]=((_tmpD06.tag=20,((_tmpD06.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp555),((_tmpD06.f2=(void*)((void*)((_tmpD04=_cycalloc(
sizeof(*_tmpD04)),((_tmpD04[0]=((_tmpD05.tag=0,((_tmpD05.f1=Cyc_Absyn_fieldname((
int)(_tmp558 + 1)),_tmpD05)))),_tmpD04))))),_tmpD06)))))),_tmpD03))));}goto _LL306;
_LL30D: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp6D6=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp6CF;if(_tmp6D6->tag != 4)goto _LL30F;}_LL30E: if(_tmp558 == 0){struct Cyc_Absyn_Offsetof_e_struct
_tmpD10;struct Cyc_Absyn_StructField_struct _tmpD0F;struct Cyc_Absyn_StructField_struct*
_tmpD0E;struct Cyc_Absyn_Offsetof_e_struct*_tmpD0D;e->r=(void*)((_tmpD0D=
_cycalloc(sizeof(*_tmpD0D)),((_tmpD0D[0]=((_tmpD10.tag=20,((_tmpD10.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp555),((_tmpD10.f2=(void*)((void*)((_tmpD0E=_cycalloc(
sizeof(*_tmpD0E)),((_tmpD0E[0]=((_tmpD0F.tag=0,((_tmpD0F.f1=Cyc_Toc_tag_sp,
_tmpD0F)))),_tmpD0E))))),_tmpD10)))))),_tmpD0D))));}else{struct Cyc_Absyn_Offsetof_e_struct
_tmpD1A;struct Cyc_Absyn_StructField_struct _tmpD19;struct Cyc_Absyn_StructField_struct*
_tmpD18;struct Cyc_Absyn_Offsetof_e_struct*_tmpD17;e->r=(void*)((_tmpD17=
_cycalloc(sizeof(*_tmpD17)),((_tmpD17[0]=((_tmpD1A.tag=20,((_tmpD1A.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp555),((_tmpD1A.f2=(void*)((void*)((_tmpD18=_cycalloc(
sizeof(*_tmpD18)),((_tmpD18[0]=((_tmpD19.tag=0,((_tmpD19.f1=Cyc_Absyn_fieldname((
int)_tmp558),_tmpD19)))),_tmpD18))))),_tmpD1A)))))),_tmpD17))));}goto _LL306;
_LL30F:;_LL310: {const char*_tmpD1D;void*_tmpD1C;(_tmpD1C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD1D="impossible type for offsetof tuple index",
_tag_dyneither(_tmpD1D,sizeof(char),41))),_tag_dyneither(_tmpD1C,sizeof(void*),0)));}
_LL306:;}goto _LL229;_LL25A: {struct Cyc_Absyn_Deref_e_struct*_tmp559=(struct Cyc_Absyn_Deref_e_struct*)
_tmp509;if(_tmp559->tag != 21)goto _LL25C;else{_tmp55A=_tmp559->f1;}}_LL25B: {int
_tmp6ED=Cyc_Toc_in_lhs(nv);Cyc_Toc_set_lhs(nv,0);{void*_tmp6EE=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp55A->topt))->v);{void*_tmp6EF=
_tmp6EE;struct Cyc_Absyn_PtrInfo _tmp6F1;void*_tmp6F2;struct Cyc_Absyn_Tqual _tmp6F3;
struct Cyc_Absyn_PtrAtts _tmp6F4;void*_tmp6F5;union Cyc_Absyn_Constraint*_tmp6F6;
union Cyc_Absyn_Constraint*_tmp6F7;union Cyc_Absyn_Constraint*_tmp6F8;_LL312: {
struct Cyc_Absyn_PointerType_struct*_tmp6F0=(struct Cyc_Absyn_PointerType_struct*)
_tmp6EF;if(_tmp6F0->tag != 5)goto _LL314;else{_tmp6F1=_tmp6F0->f1;_tmp6F2=_tmp6F1.elt_typ;
_tmp6F3=_tmp6F1.elt_tq;_tmp6F4=_tmp6F1.ptr_atts;_tmp6F5=_tmp6F4.rgn;_tmp6F6=
_tmp6F4.nullable;_tmp6F7=_tmp6F4.bounds;_tmp6F8=_tmp6F4.zero_term;}}_LL313:{void*
_tmp6F9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6F7);struct Cyc_Absyn_Exp*_tmp6FB;_LL317: {struct Cyc_Absyn_Upper_b_struct*
_tmp6FA=(struct Cyc_Absyn_Upper_b_struct*)_tmp6F9;if(_tmp6FA->tag != 1)goto _LL319;
else{_tmp6FB=_tmp6FA->f1;}}_LL318: {int do_null_check=Cyc_Toc_need_null_check(
_tmp55A);Cyc_Toc_exp_to_c(nv,_tmp55A);if(do_null_check){if(Cyc_Toc_warn_all_null_deref){
const char*_tmpD20;void*_tmpD1F;(_tmpD1F=0,Cyc_Tcutil_warn(e->loc,((_tmpD20="inserted null check due to dereference",
_tag_dyneither(_tmpD20,sizeof(char),39))),_tag_dyneither(_tmpD1F,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpD21;_tmp55A->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp6EE),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpD21=_cycalloc(sizeof(*
_tmpD21)),((_tmpD21->hd=Cyc_Absyn_copy_exp(_tmp55A),((_tmpD21->tl=0,_tmpD21)))))),
0));};}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6F8)){unsigned int _tmp701;int _tmp702;struct _tuple11 _tmp700=Cyc_Evexp_eval_const_uint_exp(
_tmp6FB);_tmp701=_tmp700.f1;_tmp702=_tmp700.f2;if(!_tmp702  || _tmp701 <= 0){const
char*_tmpD24;void*_tmpD23;(_tmpD23=0,Cyc_Tcutil_terr(e->loc,((_tmpD24="cannot determine dereference is in bounds",
_tag_dyneither(_tmpD24,sizeof(char),42))),_tag_dyneither(_tmpD23,sizeof(void*),0)));}}
goto _LL316;}_LL319: {struct Cyc_Absyn_DynEither_b_struct*_tmp6FC=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp6F9;if(_tmp6FC->tag != 0)goto _LL316;}_LL31A: {struct Cyc_Absyn_Exp*_tmp705=Cyc_Absyn_uint_exp(
0,0);{struct Cyc_Core_Opt*_tmpD25;_tmp705->topt=((_tmpD25=_cycalloc(sizeof(*
_tmpD25)),((_tmpD25->v=Cyc_Absyn_uint_typ,_tmpD25))));}e->r=Cyc_Toc_subscript_exp_r(
_tmp55A,_tmp705);Cyc_Toc_exp_to_c(nv,e);goto _LL316;}_LL316:;}goto _LL311;_LL314:;
_LL315: {const char*_tmpD28;void*_tmpD27;(_tmpD27=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD28="exp_to_c: Deref: non-pointer",
_tag_dyneither(_tmpD28,sizeof(char),29))),_tag_dyneither(_tmpD27,sizeof(void*),0)));}
_LL311:;}Cyc_Toc_set_lhs(nv,_tmp6ED);goto _LL229;};}_LL25C: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp55B=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp509;if(_tmp55B->tag != 22)goto
_LL25E;else{_tmp55C=_tmp55B->f1;_tmp55D=_tmp55B->f2;_tmp55E=_tmp55B->f3;_tmp55F=
_tmp55B->f4;}}_LL25D: {int is_poly=Cyc_Toc_is_poly_project(e);void*e1_cyc_type=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp55C->topt))->v;Cyc_Toc_exp_to_c(nv,
_tmp55C);if(_tmp55E  && _tmp55F)e->r=Cyc_Toc_check_tagged_union(_tmp55C,Cyc_Toc_typ_to_c(
e1_cyc_type),e1_cyc_type,_tmp55D,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrmember_exp);
if(is_poly)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp(e->r,0),0))->r;goto _LL229;}_LL25E: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp560=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp509;if(_tmp560->tag != 23)goto _LL260;else{_tmp561=_tmp560->f1;_tmp562=_tmp560->f2;
_tmp563=_tmp560->f3;_tmp564=_tmp560->f4;}}_LL25F: {int _tmp709=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp561->topt))->v);int do_null_check=Cyc_Toc_need_null_check(_tmp561);
Cyc_Toc_exp_to_c(nv,_tmp561);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp70B;
struct Cyc_Absyn_Tqual _tmp70C;struct Cyc_Absyn_PtrAtts _tmp70D;void*_tmp70E;union
Cyc_Absyn_Constraint*_tmp70F;union Cyc_Absyn_Constraint*_tmp710;union Cyc_Absyn_Constraint*
_tmp711;struct Cyc_Absyn_PtrInfo _tmp70A=Cyc_Toc_get_ptr_type(e1typ);_tmp70B=
_tmp70A.elt_typ;_tmp70C=_tmp70A.elt_tq;_tmp70D=_tmp70A.ptr_atts;_tmp70E=_tmp70D.rgn;
_tmp70F=_tmp70D.nullable;_tmp710=_tmp70D.bounds;_tmp711=_tmp70D.zero_term;{void*
_tmp712=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp710);struct Cyc_Absyn_Exp*_tmp714;_LL31C: {struct Cyc_Absyn_Upper_b_struct*
_tmp713=(struct Cyc_Absyn_Upper_b_struct*)_tmp712;if(_tmp713->tag != 1)goto _LL31E;
else{_tmp714=_tmp713->f1;}}_LL31D: {unsigned int _tmp717;int _tmp718;struct
_tuple11 _tmp716=Cyc_Evexp_eval_const_uint_exp(_tmp714);_tmp717=_tmp716.f1;
_tmp718=_tmp716.f2;if(_tmp718){if(_tmp717 < 1){const char*_tmpD2B;void*_tmpD2A;(
_tmpD2A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD2B="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpD2B,
sizeof(char),44))),_tag_dyneither(_tmpD2A,sizeof(void*),0)));}if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){const char*_tmpD2E;void*_tmpD2D;(_tmpD2D=0,Cyc_Tcutil_warn(
e->loc,((_tmpD2E="inserted null check due to dereference",_tag_dyneither(_tmpD2E,
sizeof(char),39))),_tag_dyneither(_tmpD2D,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpD2F[1];_tmp561->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpD2F[0]=Cyc_Absyn_new_exp(_tmp561->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD2F,sizeof(struct Cyc_Absyn_Exp*),
1)))),0));};}}else{if(!Cyc_Evexp_c_can_eval(_tmp714)){const char*_tmpD32;void*
_tmpD31;(_tmpD31=0,Cyc_Tcutil_terr(e->loc,((_tmpD32="cannot determine pointer dereference in bounds",
_tag_dyneither(_tmpD32,sizeof(char),47))),_tag_dyneither(_tmpD31,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpD33[4];_tmp561->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((_tmpD33[3]=
Cyc_Absyn_uint_exp(0,0),((_tmpD33[2]=Cyc_Absyn_sizeoftyp_exp(_tmp70B,0),((
_tmpD33[1]=_tmp714,((_tmpD33[0]=Cyc_Absyn_new_exp(_tmp561->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD33,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0));};}goto _LL31B;}_LL31E: {struct Cyc_Absyn_DynEither_b_struct*
_tmp715=(struct Cyc_Absyn_DynEither_b_struct*)_tmp712;if(_tmp715->tag != 0)goto
_LL31B;}_LL31F: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp70B);{struct Cyc_Absyn_Exp*
_tmpD34[3];_tmp561->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp70C),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_dyneither_subscript_e,((_tmpD34[2]=Cyc_Absyn_uint_exp(0,0),((
_tmpD34[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD34[0]=Cyc_Absyn_new_exp(_tmp561->r,
0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD34,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}goto _LL31B;}_LL31B:;}if(
_tmp563  && _tmp564)e->r=Cyc_Toc_check_tagged_union(_tmp561,Cyc_Toc_typ_to_c(
e1typ),_tmp70B,_tmp562,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);if(is_poly
 && _tmp564)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp(e->r,0),0))->r;Cyc_Toc_set_lhs(nv,
_tmp709);goto _LL229;};};}_LL260: {struct Cyc_Absyn_Subscript_e_struct*_tmp565=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp509;if(_tmp565->tag != 24)goto _LL262;
else{_tmp566=_tmp565->f1;_tmp567=_tmp565->f2;}}_LL261: {int _tmp722=Cyc_Toc_in_lhs(
nv);Cyc_Toc_set_lhs(nv,0);{void*_tmp723=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp566->topt))->v);{void*_tmp724=_tmp723;struct Cyc_List_List*
_tmp726;struct Cyc_Absyn_PtrInfo _tmp728;void*_tmp729;struct Cyc_Absyn_Tqual _tmp72A;
struct Cyc_Absyn_PtrAtts _tmp72B;void*_tmp72C;union Cyc_Absyn_Constraint*_tmp72D;
union Cyc_Absyn_Constraint*_tmp72E;union Cyc_Absyn_Constraint*_tmp72F;_LL321: {
struct Cyc_Absyn_TupleType_struct*_tmp725=(struct Cyc_Absyn_TupleType_struct*)
_tmp724;if(_tmp725->tag != 10)goto _LL323;else{_tmp726=_tmp725->f1;}}_LL322: Cyc_Toc_exp_to_c(
nv,_tmp566);Cyc_Toc_exp_to_c(nv,_tmp567);{unsigned int _tmp731;int _tmp732;struct
_tuple11 _tmp730=Cyc_Evexp_eval_const_uint_exp(_tmp567);_tmp731=_tmp730.f1;
_tmp732=_tmp730.f2;if(!_tmp732){const char*_tmpD37;void*_tmpD36;(_tmpD36=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD37="unknown tuple subscript in translation to C",
_tag_dyneither(_tmpD37,sizeof(char),44))),_tag_dyneither(_tmpD36,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp566,Cyc_Absyn_fieldname((int)(_tmp731 + 1)));
goto _LL320;};_LL323: {struct Cyc_Absyn_PointerType_struct*_tmp727=(struct Cyc_Absyn_PointerType_struct*)
_tmp724;if(_tmp727->tag != 5)goto _LL325;else{_tmp728=_tmp727->f1;_tmp729=_tmp728.elt_typ;
_tmp72A=_tmp728.elt_tq;_tmp72B=_tmp728.ptr_atts;_tmp72C=_tmp72B.rgn;_tmp72D=
_tmp72B.nullable;_tmp72E=_tmp72B.bounds;_tmp72F=_tmp72B.zero_term;}}_LL324: {
struct Cyc_List_List*_tmp735=Cyc_Toc_get_relns(_tmp566);int in_bnds=0;{void*
_tmp736=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp72E);_LL328:;_LL329: in_bnds=Cyc_Toc_check_bounds(_tmp723,_tmp735,_tmp566,
_tmp567);if(Cyc_Toc_warn_bounds_checks  && !in_bnds){const char*_tmpD3B;void*
_tmpD3A[1];struct Cyc_String_pa_struct _tmpD39;(_tmpD39.tag=0,((_tmpD39.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpD3A[0]=&
_tmpD39,Cyc_Tcutil_warn(e->loc,((_tmpD3B="bounds check necessary for %s",
_tag_dyneither(_tmpD3B,sizeof(char),30))),_tag_dyneither(_tmpD3A,sizeof(void*),1)))))));}
_LL327:;}Cyc_Toc_exp_to_c(nv,_tmp566);Cyc_Toc_exp_to_c(nv,_tmp567);++ Cyc_Toc_total_bounds_checks;{
void*_tmp73A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp72E);struct Cyc_Absyn_Exp*_tmp73C;_LL32B: {struct Cyc_Absyn_Upper_b_struct*
_tmp73B=(struct Cyc_Absyn_Upper_b_struct*)_tmp73A;if(_tmp73B->tag != 1)goto _LL32D;
else{_tmp73C=_tmp73B->f1;}}_LL32C: {int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp72D);void*ta1=Cyc_Toc_typ_to_c(_tmp729);void*ta2=
Cyc_Absyn_cstar_typ(ta1,_tmp72A);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp72F)){if(!Cyc_Evexp_c_can_eval(_tmp73C)){const char*_tmpD3E;void*_tmpD3D;(
_tmpD3D=0,Cyc_Tcutil_terr(e->loc,((_tmpD3E="cannot determine subscript is in bounds",
_tag_dyneither(_tmpD3E,sizeof(char),40))),_tag_dyneither(_tmpD3D,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp566);struct Cyc_Absyn_Exp*_tmpD3F[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpD3F[2]=_tmp567,((_tmpD3F[1]=_tmp73C,((
_tmpD3F[0]=_tmp566,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD3F,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp73C)){const char*_tmpD42;void*_tmpD41;(
_tmpD41=0,Cyc_Tcutil_terr(e->loc,((_tmpD42="cannot determine subscript is in bounds",
_tag_dyneither(_tmpD42,sizeof(char),40))),_tag_dyneither(_tmpD41,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){const char*_tmpD45;void*_tmpD44;(_tmpD44=0,Cyc_Tcutil_warn(
e->loc,((_tmpD45="inserted null check due to dereference",_tag_dyneither(_tmpD45,
sizeof(char),39))),_tag_dyneither(_tmpD44,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpD46[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((
_tmpD46[3]=_tmp567,((_tmpD46[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD46[1]=
_tmp73C,((_tmpD46[0]=_tmp566,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD46,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{if(
!Cyc_Evexp_c_can_eval(_tmp73C)){const char*_tmpD49;void*_tmpD48;(_tmpD48=0,Cyc_Tcutil_terr(
e->loc,((_tmpD49="cannot determine subscript is in bounds",_tag_dyneither(
_tmpD49,sizeof(char),40))),_tag_dyneither(_tmpD48,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpD4A[2];_tmp567->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((
_tmpD4A[1]=Cyc_Absyn_copy_exp(_tmp567),((_tmpD4A[0]=_tmp73C,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD4A,sizeof(struct Cyc_Absyn_Exp*),
2)))))));};}}}goto _LL32A;}_LL32D: {struct Cyc_Absyn_DynEither_b_struct*_tmp73D=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp73A;if(_tmp73D->tag != 0)goto _LL32A;}
_LL32E: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp729);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp72A),
Cyc_Absyn_aggrmember_exp(_tmp566,Cyc_Toc_curr_sp,0)),_tmp567);}else{struct Cyc_Absyn_Exp*
_tmp749=Cyc_Toc__check_dyneither_subscript_e;struct Cyc_Absyn_Exp*_tmpD4B[3];e->r=
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp72A),Cyc_Absyn_fncall_exp(
_tmp749,((_tmpD4B[2]=_tmp567,((_tmpD4B[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((
_tmpD4B[0]=_tmp566,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD4B,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL32A;}
_LL32A:;}goto _LL320;}_LL325:;_LL326: {const char*_tmpD4E;void*_tmpD4D;(_tmpD4D=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD4E="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(
_tmpD4E,sizeof(char),49))),_tag_dyneither(_tmpD4D,sizeof(void*),0)));}_LL320:;}
Cyc_Toc_set_lhs(nv,_tmp722);goto _LL229;};}_LL262: {struct Cyc_Absyn_Tuple_e_struct*
_tmp568=(struct Cyc_Absyn_Tuple_e_struct*)_tmp509;if(_tmp568->tag != 25)goto _LL264;
else{_tmp569=_tmp568->f1;}}_LL263: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(
nv,0,0,_tmp569))->r;else{struct Cyc_List_List*_tmp74D=((struct Cyc_List_List*(*)(
struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_tup_to_c,_tmp569);void*_tmp74E=Cyc_Toc_add_tuple_type(_tmp74D);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp569 != 0;(_tmp569=_tmp569->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp569->hd);{struct _tuple16*_tmpD51;struct Cyc_List_List*
_tmpD50;dles=((_tmpD50=_cycalloc(sizeof(*_tmpD50)),((_tmpD50->hd=((_tmpD51=
_cycalloc(sizeof(*_tmpD51)),((_tmpD51->f1=0,((_tmpD51->f2=(struct Cyc_Absyn_Exp*)
_tmp569->hd,_tmpD51)))))),((_tmpD50->tl=dles,_tmpD50))))));};}}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);e->r=Cyc_Toc_unresolvedmem_exp_r(
0,dles);}goto _LL229;_LL264: {struct Cyc_Absyn_Array_e_struct*_tmp56A=(struct Cyc_Absyn_Array_e_struct*)
_tmp509;if(_tmp56A->tag != 27)goto _LL266;else{_tmp56B=_tmp56A->f1;}}_LL265: e->r=
Cyc_Toc_unresolvedmem_exp_r(0,_tmp56B);{struct Cyc_List_List*_tmp751=_tmp56B;for(
0;_tmp751 != 0;_tmp751=_tmp751->tl){struct _tuple16 _tmp753;struct Cyc_Absyn_Exp*
_tmp754;struct _tuple16*_tmp752=(struct _tuple16*)_tmp751->hd;_tmp753=*_tmp752;
_tmp754=_tmp753.f2;Cyc_Toc_exp_to_c(nv,_tmp754);}}goto _LL229;_LL266: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp56C=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp509;if(_tmp56C->tag != 28)
goto _LL268;else{_tmp56D=_tmp56C->f1;_tmp56E=_tmp56C->f2;_tmp56F=_tmp56C->f3;
_tmp570=_tmp56C->f4;}}_LL267: {unsigned int _tmp756;int _tmp757;struct _tuple11
_tmp755=Cyc_Evexp_eval_const_uint_exp(_tmp56E);_tmp756=_tmp755.f1;_tmp757=
_tmp755.f2;{void*_tmp758=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp56F->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp56F);{struct Cyc_List_List*
es=0;if(!Cyc_Toc_is_zero(_tmp56F)){if(!_tmp757){const char*_tmpD54;void*_tmpD53;(
_tmpD53=0,Cyc_Tcutil_terr(_tmp56E->loc,((_tmpD54="cannot determine value of constant",
_tag_dyneither(_tmpD54,sizeof(char),35))),_tag_dyneither(_tmpD53,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp756;++ i){struct _tuple16*_tmpD57;struct Cyc_List_List*
_tmpD56;es=((_tmpD56=_cycalloc(sizeof(*_tmpD56)),((_tmpD56->hd=((_tmpD57=
_cycalloc(sizeof(*_tmpD57)),((_tmpD57->f1=0,((_tmpD57->f2=_tmp56F,_tmpD57)))))),((
_tmpD56->tl=es,_tmpD56))))));}}if(_tmp570){struct Cyc_Absyn_Exp*_tmp75D=Cyc_Toc_cast_it(
_tmp758,Cyc_Absyn_uint_exp(0,0));struct _tuple16*_tmpD5A;struct Cyc_List_List*
_tmpD59;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,((_tmpD59=_cycalloc(sizeof(*_tmpD59)),((_tmpD59->hd=((
_tmpD5A=_cycalloc(sizeof(*_tmpD5A)),((_tmpD5A->f1=0,((_tmpD5A->f2=_tmp75D,
_tmpD5A)))))),((_tmpD59->tl=0,_tmpD59)))))));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL229;};};}_LL268: {struct Cyc_Absyn_Aggregate_e_struct*_tmp571=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp509;if(_tmp571->tag != 29)goto _LL26A;
else{_tmp572=_tmp571->f1;_tmp573=_tmp571->f2;_tmp574=_tmp571->f3;_tmp575=_tmp571->f4;}}
_LL269: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp573
!= 0,0,0,_tmp574,_tmp572))->r;else{if(_tmp575 == 0){const char*_tmpD5D;void*
_tmpD5C;(_tmpD5C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD5D="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpD5D,sizeof(
char),38))),_tag_dyneither(_tmpD5C,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp575;struct Cyc_Toc_Env _tmp763;struct
_RegionHandle*_tmp764;struct Cyc_Toc_Env*_tmp762=nv;_tmp763=*_tmp762;_tmp764=
_tmp763.rgn;{struct Cyc_List_List*_tmp765=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum 
Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp764,e->loc,_tmp574,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp575->impl))->tagged){struct
_tuple22 _tmp767;struct Cyc_Absyn_Aggrfield*_tmp768;struct Cyc_Absyn_Exp*_tmp769;
struct _tuple22*_tmp766=(struct _tuple22*)((struct Cyc_List_List*)_check_null(
_tmp765))->hd;_tmp767=*_tmp766;_tmp768=_tmp767.f1;_tmp769=_tmp767.f2;{void*
_tmp76A=_tmp768->type;Cyc_Toc_exp_to_c(nv,_tmp769);if(Cyc_Toc_is_void_star_or_boxed_tvar(
_tmp76A))_tmp769->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp769->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp575,_tmp768->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple16*_tmpD62;struct _tuple16*_tmpD61;struct _tuple16*_tmpD60[2];
struct Cyc_List_List*_tmp76B=(_tmpD60[1]=((_tmpD61=_cycalloc(sizeof(*_tmpD61)),((
_tmpD61->f1=0,((_tmpD61->f2=_tmp769,_tmpD61)))))),((_tmpD60[0]=((_tmpD62=
_cycalloc(sizeof(*_tmpD62)),((_tmpD62->f1=0,((_tmpD62->f2=field_tag_exp,_tmpD62)))))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD60,sizeof(struct _tuple16*),2)))));struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(
0,_tmp76B,0);struct Cyc_Absyn_FieldName_struct*_tmpD68;struct Cyc_Absyn_FieldName_struct
_tmpD67;void*_tmpD66[1];struct Cyc_List_List*ds=(_tmpD66[0]=(void*)((_tmpD68=
_cycalloc(sizeof(*_tmpD68)),((_tmpD68[0]=((_tmpD67.tag=1,((_tmpD67.f1=_tmp768->name,
_tmpD67)))),_tmpD68)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD66,sizeof(void*),1)));struct _tuple16*_tmpD6B;struct _tuple16*
_tmpD6A[1];struct Cyc_List_List*dles=(_tmpD6A[0]=((_tmpD6B=_cycalloc(sizeof(*
_tmpD6B)),((_tmpD6B->f1=ds,((_tmpD6B->f2=umem,_tmpD6B)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD6A,sizeof(struct
_tuple16*),1)));e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{struct Cyc_List_List*
_tmp774=0;struct Cyc_List_List*_tmp775=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp775 != 0;_tmp775=_tmp775->tl){struct Cyc_List_List*
_tmp776=_tmp765;for(0;_tmp776 != 0;_tmp776=_tmp776->tl){if((*((struct _tuple22*)
_tmp776->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp775->hd){struct _tuple22
_tmp778;struct Cyc_Absyn_Aggrfield*_tmp779;struct Cyc_Absyn_Exp*_tmp77A;struct
_tuple22*_tmp777=(struct _tuple22*)_tmp776->hd;_tmp778=*_tmp777;_tmp779=_tmp778.f1;
_tmp77A=_tmp778.f2;{void*_tmp77B=_tmp779->type;Cyc_Toc_exp_to_c(nv,_tmp77A);if(
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp77B))_tmp77A->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp77A->r,0));{struct _tuple16*_tmpD6E;struct Cyc_List_List*
_tmpD6D;_tmp774=((_tmpD6D=_cycalloc(sizeof(*_tmpD6D)),((_tmpD6D->hd=((_tmpD6E=
_cycalloc(sizeof(*_tmpD6E)),((_tmpD6E->f1=0,((_tmpD6E->f2=_tmp77A,_tmpD6E)))))),((
_tmpD6D->tl=_tmp774,_tmpD6D))))));}break;};}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp774));}};};}
goto _LL229;_LL26A: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp576=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp509;if(_tmp576->tag != 30)goto _LL26C;else{_tmp577=(void*)_tmp576->f1;_tmp578=
_tmp576->f2;}}_LL26B: {struct Cyc_List_List*fs;{void*_tmp77E=Cyc_Tcutil_compress(
_tmp577);struct Cyc_List_List*_tmp780;_LL330: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp77F=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp77E;if(_tmp77F->tag != 12)goto
_LL332;else{_tmp780=_tmp77F->f2;}}_LL331: fs=_tmp780;goto _LL32F;_LL332:;_LL333: {
const char*_tmpD72;void*_tmpD71[1];struct Cyc_String_pa_struct _tmpD70;(_tmpD70.tag=
0,((_tmpD70.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp577)),((_tmpD71[0]=& _tmpD70,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD72="anon struct has type %s",
_tag_dyneither(_tmpD72,sizeof(char),24))),_tag_dyneither(_tmpD71,sizeof(void*),1)))))));}
_LL32F:;}{struct Cyc_Toc_Env _tmp785;struct _RegionHandle*_tmp786;struct Cyc_Toc_Env*
_tmp784=nv;_tmp785=*_tmp784;_tmp786=_tmp785.rgn;{struct Cyc_List_List*_tmp787=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp786,e->loc,_tmp578,Cyc_Absyn_StructA,fs);for(0;_tmp787 != 0;_tmp787=_tmp787->tl){
struct _tuple22 _tmp789;struct Cyc_Absyn_Aggrfield*_tmp78A;struct Cyc_Absyn_Exp*
_tmp78B;struct _tuple22*_tmp788=(struct _tuple22*)_tmp787->hd;_tmp789=*_tmp788;
_tmp78A=_tmp789.f1;_tmp78B=_tmp789.f2;{void*_tmp78C=_tmp78A->type;Cyc_Toc_exp_to_c(
nv,_tmp78B);if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp78C))_tmp78B->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp78B->r,0));};}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp578);}goto _LL229;};}_LL26C: {struct Cyc_Absyn_Datatype_e_struct*_tmp579=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp509;if(_tmp579->tag != 31)goto _LL26E;else{
_tmp57A=_tmp579->f1;_tmp57B=_tmp579->f2;_tmp57C=_tmp579->f3;}}_LL26D: {void*
datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple0*_tmp78D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp78E=Cyc_Absyn_var_exp(_tmp78D,0);struct Cyc_Absyn_Exp*
member_exp;{const char*_tmpD73;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp57C->name,((_tmpD73="_struct",_tag_dyneither(_tmpD73,sizeof(char),8)))));}
tag_exp=_tmp57B->is_extensible?Cyc_Absyn_var_exp(_tmp57C->name,0): Cyc_Toc_datatype_tag(
_tmp57B,_tmp57C->name);member_exp=_tmp78E;{struct Cyc_List_List*_tmp790=_tmp57C->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp57A != 0;(
_tmp57A=_tmp57A->tl,_tmp790=_tmp790->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp57A->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp790))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_boxed_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct _tuple16*_tmpD76;struct
Cyc_List_List*_tmpD75;dles=((_tmpD75=_cycalloc(sizeof(*_tmpD75)),((_tmpD75->hd=((
_tmpD76=_cycalloc(sizeof(*_tmpD76)),((_tmpD76->f1=0,((_tmpD76->f2=cur_e,_tmpD76)))))),((
_tmpD75->tl=dles,_tmpD75))))));};}{struct _tuple16*_tmpD79;struct Cyc_List_List*
_tmpD78;dles=((_tmpD78=_cycalloc(sizeof(*_tmpD78)),((_tmpD78->hd=((_tmpD79=
_cycalloc(sizeof(*_tmpD79)),((_tmpD79->f1=0,((_tmpD79->f2=tag_exp,_tmpD79)))))),((
_tmpD78->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles),_tmpD78))))));}e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{struct Cyc_List_List*
_tmpD7A;struct Cyc_List_List*_tmp795=(_tmpD7A=_cycalloc(sizeof(*_tmpD7A)),((
_tmpD7A->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,
0),tag_exp,0),((_tmpD7A->tl=0,_tmpD7A)))));{int i=1;for(0;_tmp57A != 0;(((_tmp57A=
_tmp57A->tl,i ++)),_tmp790=_tmp790->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp57A->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp790))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_boxed_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp796=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpD7B;_tmp795=((_tmpD7B=_cycalloc(sizeof(*
_tmpD7B)),((_tmpD7B->hd=_tmp796,((_tmpD7B->tl=_tmp795,_tmpD7B))))));};}}{struct
Cyc_Absyn_Stmt*_tmp798=Cyc_Absyn_exp_stmt(_tmp78E,0);struct Cyc_List_List*_tmpD7C;
struct Cyc_Absyn_Stmt*_tmp799=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpD7C=_cycalloc(sizeof(*_tmpD7C)),((
_tmpD7C->hd=_tmp798,((_tmpD7C->tl=_tmp795,_tmpD7C))))))),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp78D,datatype_ctype,0,_tmp799,0));};}goto _LL229;};}
_LL26E: {struct Cyc_Absyn_Enum_e_struct*_tmp57D=(struct Cyc_Absyn_Enum_e_struct*)
_tmp509;if(_tmp57D->tag != 32)goto _LL270;}_LL26F: goto _LL271;_LL270: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp57E=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp509;if(_tmp57E->tag != 33)goto
_LL272;}_LL271: goto _LL229;_LL272: {struct Cyc_Absyn_Malloc_e_struct*_tmp57F=(
struct Cyc_Absyn_Malloc_e_struct*)_tmp509;if(_tmp57F->tag != 34)goto _LL274;else{
_tmp580=_tmp57F->f1;_tmp581=_tmp580.is_calloc;_tmp582=_tmp580.rgn;_tmp583=
_tmp580.elt_type;_tmp584=_tmp580.num_elts;_tmp585=_tmp580.fat_result;}}_LL273: {
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp583)));Cyc_Toc_exp_to_c(nv,
_tmp584);if(_tmp585){struct _tuple0*_tmp79C=Cyc_Toc_temp_var();struct _tuple0*
_tmp79D=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp581){xexp=_tmp584;if(_tmp582 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp582;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp79C,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp583,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp79C,0));}{struct Cyc_Absyn_Exp*_tmpD7D[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpD7D[2]=Cyc_Absyn_var_exp(_tmp79C,0),((_tmpD7D[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpD7D[0]=Cyc_Absyn_var_exp(_tmp79D,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD7D,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{if(_tmp582 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp582;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp79C,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp583,Cyc_Absyn_var_exp(_tmp79C,0));}{struct Cyc_Absyn_Exp*_tmpD7E[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpD7E[2]=Cyc_Absyn_var_exp(_tmp79C,0),((_tmpD7E[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpD7E[0]=Cyc_Absyn_var_exp(_tmp79D,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD7E,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);};}{struct Cyc_Absyn_Stmt*_tmp7A0=Cyc_Absyn_declare_stmt(_tmp79C,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)_tmp584,Cyc_Absyn_declare_stmt(_tmp79D,Cyc_Absyn_cstar_typ(
t_c,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp7A0);};}else{if(_tmp581){if(_tmp582 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp582;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp584))->r;}else{e->r=(
Cyc_Toc_calloc_exp(*_tmp583,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp584))->r;}}else{
if(_tmp582 != 0  && !Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp582;Cyc_Toc_exp_to_c(nv,rgn);e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp584))->r;}
else{e->r=(Cyc_Toc_malloc_exp(*_tmp583,_tmp584))->r;}}}goto _LL229;}_LL274: {
struct Cyc_Absyn_Swap_e_struct*_tmp586=(struct Cyc_Absyn_Swap_e_struct*)_tmp509;
if(_tmp586->tag != 35)goto _LL276;else{_tmp587=_tmp586->f1;_tmp588=_tmp586->f2;}}
_LL275: {int is_dyneither_ptr=0;void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp587->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp588->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,&
is_dyneither_ptr)){const char*_tmpD81;void*_tmpD80;(_tmpD80=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD81="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpD81,sizeof(char),57))),_tag_dyneither(_tmpD80,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}{int f1_tag=0;void*tagged_mem_type1=(void*)&
Cyc_Absyn_VoidType_val;int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp587,&
f1_tag,& tagged_mem_type1,1);int f2_tag=0;void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp588,& f2_tag,& tagged_mem_type2,1);
Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp587);Cyc_Toc_exp_to_c(nv,_tmp588);
Cyc_Toc_set_lhs(nv,0);if(e1_tmem)Cyc_Toc_add_tagged_union_check_for_swap(_tmp587,
f1_tag,tagged_mem_type1);else{_tmp587=Cyc_Toc_push_address_exp(_tmp587);}if(
e2_tmem)Cyc_Toc_add_tagged_union_check_for_swap(_tmp588,f2_tag,tagged_mem_type2);
else{_tmp588=Cyc_Toc_push_address_exp(_tmp588);}{struct Cyc_Absyn_Exp*_tmpD82[2];
e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpD82[1]=_tmp588,((_tmpD82[0]=_tmp587,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD82,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL229;};};}_LL276: {struct
Cyc_Absyn_Tagcheck_e_struct*_tmp589=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp509;
if(_tmp589->tag != 38)goto _LL278;else{_tmp58A=_tmp589->f1;_tmp58B=_tmp589->f2;}}
_LL277: {void*_tmp7A4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp58A->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp58A);{void*_tmp7A5=
_tmp7A4;struct Cyc_Absyn_AggrInfo _tmp7A7;union Cyc_Absyn_AggrInfoU _tmp7A8;struct
Cyc_Absyn_Aggrdecl**_tmp7A9;struct Cyc_Absyn_Aggrdecl*_tmp7AA;_LL335: {struct Cyc_Absyn_AggrType_struct*
_tmp7A6=(struct Cyc_Absyn_AggrType_struct*)_tmp7A5;if(_tmp7A6->tag != 11)goto
_LL337;else{_tmp7A7=_tmp7A6->f1;_tmp7A8=_tmp7A7.aggr_info;if((_tmp7A8.KnownAggr).tag
!= 2)goto _LL337;_tmp7A9=(struct Cyc_Absyn_Aggrdecl**)(_tmp7A8.KnownAggr).val;
_tmp7AA=*_tmp7A9;}}_LL336: {struct Cyc_Absyn_Exp*_tmp7AB=Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp7AA,_tmp58B),0);struct Cyc_Absyn_Exp*_tmp7AC=Cyc_Absyn_aggrmember_exp(
_tmp58A,_tmp58B,0);struct Cyc_Absyn_Exp*_tmp7AD=Cyc_Absyn_aggrmember_exp(_tmp7AC,
Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(_tmp7AD,_tmp7AB,0))->r;goto _LL334;}
_LL337:;_LL338: {const char*_tmpD85;void*_tmpD84;(_tmpD84=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD85="non-aggregate type in tagcheck",
_tag_dyneither(_tmpD85,sizeof(char),31))),_tag_dyneither(_tmpD84,sizeof(void*),0)));}
_LL334:;}goto _LL229;}_LL278: {struct Cyc_Absyn_StmtExp_e_struct*_tmp58C=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp509;if(_tmp58C->tag != 37)goto _LL27A;else{_tmp58D=
_tmp58C->f1;}}_LL279: Cyc_Toc_stmt_to_c(nv,_tmp58D);goto _LL229;_LL27A: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp58E=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp509;if(_tmp58E->tag != 36)
goto _LL27C;}_LL27B: {const char*_tmpD88;void*_tmpD87;(_tmpD87=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD88="UnresolvedMem",
_tag_dyneither(_tmpD88,sizeof(char),14))),_tag_dyneither(_tmpD87,sizeof(void*),0)));}
_LL27C: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp58F=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp509;if(_tmp58F->tag != 26)goto _LL27E;}_LL27D: {const char*_tmpD8B;void*_tmpD8A;(
_tmpD8A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpD8B="compoundlit",_tag_dyneither(_tmpD8B,sizeof(char),12))),_tag_dyneither(
_tmpD8A,sizeof(void*),0)));}_LL27E: {struct Cyc_Absyn_Valueof_e_struct*_tmp590=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp509;if(_tmp590->tag != 39)goto _LL280;}
_LL27F: {const char*_tmpD8E;void*_tmpD8D;(_tmpD8D=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD8E="valueof(-)",
_tag_dyneither(_tmpD8E,sizeof(char),11))),_tag_dyneither(_tmpD8D,sizeof(void*),0)));}
_LL280: {struct Cyc_Absyn_Asm_e_struct*_tmp591=(struct Cyc_Absyn_Asm_e_struct*)
_tmp509;if(_tmp591->tag != 40)goto _LL229;}_LL281: {const char*_tmpD91;void*_tmpD90;(
_tmpD90=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD91="__asm__",_tag_dyneither(_tmpD91,sizeof(char),8))),_tag_dyneither(
_tmpD90,sizeof(void*),0)));}_LL229:;};}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0);}struct _tuple23{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*
f3;};struct _tuple24{struct _tuple0*f1;void*f2;};struct _tuple25{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple23 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*
nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
struct Cyc_Absyn_Stmt*s;{void*_tmp7B8=p->r;struct Cyc_Absyn_Vardecl*_tmp7BA;struct
Cyc_Absyn_Vardecl _tmp7BB;struct _tuple0*_tmp7BC;struct Cyc_Absyn_Pat*_tmp7BD;
struct Cyc_Absyn_Vardecl*_tmp7BF;struct Cyc_Absyn_Vardecl _tmp7C0;struct _tuple0*
_tmp7C1;struct Cyc_Absyn_Vardecl*_tmp7C4;struct Cyc_Absyn_Pat*_tmp7C5;enum Cyc_Absyn_Sign
_tmp7C8;int _tmp7C9;char _tmp7CB;struct _dyneither_ptr _tmp7CD;int _tmp7CE;struct Cyc_Absyn_Enumdecl*
_tmp7D0;struct Cyc_Absyn_Enumfield*_tmp7D1;void*_tmp7D3;struct Cyc_Absyn_Enumfield*
_tmp7D4;struct Cyc_Absyn_Pat*_tmp7D6;struct Cyc_Absyn_Pat _tmp7D7;void*_tmp7D8;
struct Cyc_Absyn_Datatypedecl*_tmp7DA;struct Cyc_Absyn_Datatypefield*_tmp7DB;
struct Cyc_List_List*_tmp7DC;struct Cyc_List_List*_tmp7DE;struct Cyc_List_List*
_tmp7E0;struct Cyc_Absyn_AggrInfo*_tmp7E2;struct Cyc_Absyn_AggrInfo*_tmp7E4;struct
Cyc_Absyn_AggrInfo _tmp7E5;union Cyc_Absyn_AggrInfoU _tmp7E6;struct Cyc_List_List*
_tmp7E7;struct Cyc_Absyn_Pat*_tmp7E9;_LL33A: {struct Cyc_Absyn_Var_p_struct*
_tmp7B9=(struct Cyc_Absyn_Var_p_struct*)_tmp7B8;if(_tmp7B9->tag != 1)goto _LL33C;
else{_tmp7BA=_tmp7B9->f1;_tmp7BB=*_tmp7BA;_tmp7BC=_tmp7BB.name;_tmp7BD=_tmp7B9->f2;}}
_LL33B: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp7BC,r),rgn,t,r,path,
_tmp7BD,fail_stmt,decls);_LL33C: {struct Cyc_Absyn_TagInt_p_struct*_tmp7BE=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp7B8;if(_tmp7BE->tag != 3)goto _LL33E;else{
_tmp7BF=_tmp7BE->f2;_tmp7C0=*_tmp7BF;_tmp7C1=_tmp7C0.name;}}_LL33D: nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp7C1,r);goto _LL33F;_LL33E: {struct Cyc_Absyn_Wild_p_struct*_tmp7C2=(
struct Cyc_Absyn_Wild_p_struct*)_tmp7B8;if(_tmp7C2->tag != 0)goto _LL340;}_LL33F: s=
Cyc_Toc_skip_stmt_dl();goto _LL339;_LL340: {struct Cyc_Absyn_Reference_p_struct*
_tmp7C3=(struct Cyc_Absyn_Reference_p_struct*)_tmp7B8;if(_tmp7C3->tag != 2)goto
_LL342;else{_tmp7C4=_tmp7C3->f1;_tmp7C5=_tmp7C3->f2;}}_LL341: {struct _tuple0*
_tmp7ED=Cyc_Toc_temp_var();{struct _tuple24*_tmpD94;struct Cyc_List_List*_tmpD93;
decls=((_tmpD93=_region_malloc(rgn,sizeof(*_tmpD93)),((_tmpD93->hd=((_tmpD94=
_region_malloc(rgn,sizeof(*_tmpD94)),((_tmpD94->f1=_tmp7ED,((_tmpD94->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpD94)))))),((_tmpD93->tl=decls,_tmpD93))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp7C4->name,Cyc_Absyn_var_exp(_tmp7ED,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp7ED,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Toc_push_address_exp(path)),0);{struct _tuple23 _tmp7F0=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp7C5,fail_stmt,decls);_tmp7F0.f3=Cyc_Absyn_seq_stmt(s,_tmp7F0.f3,
0);return _tmp7F0;};}_LL342: {struct Cyc_Absyn_Null_p_struct*_tmp7C6=(struct Cyc_Absyn_Null_p_struct*)
_tmp7B8;if(_tmp7C6->tag != 8)goto _LL344;}_LL343: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),fail_stmt);goto _LL339;_LL344: {struct Cyc_Absyn_Int_p_struct*_tmp7C7=(struct
Cyc_Absyn_Int_p_struct*)_tmp7B8;if(_tmp7C7->tag != 9)goto _LL346;else{_tmp7C8=
_tmp7C7->f1;_tmp7C9=_tmp7C7->f2;}}_LL345: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp7C8,_tmp7C9,0),fail_stmt);goto _LL339;_LL346: {struct Cyc_Absyn_Char_p_struct*
_tmp7CA=(struct Cyc_Absyn_Char_p_struct*)_tmp7B8;if(_tmp7CA->tag != 10)goto _LL348;
else{_tmp7CB=_tmp7CA->f1;}}_LL347: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(
_tmp7CB,0),fail_stmt);goto _LL339;_LL348: {struct Cyc_Absyn_Float_p_struct*_tmp7CC=(
struct Cyc_Absyn_Float_p_struct*)_tmp7B8;if(_tmp7CC->tag != 11)goto _LL34A;else{
_tmp7CD=_tmp7CC->f1;_tmp7CE=_tmp7CC->f2;}}_LL349: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(
_tmp7CD,_tmp7CE,0),fail_stmt);goto _LL339;_LL34A: {struct Cyc_Absyn_Enum_p_struct*
_tmp7CF=(struct Cyc_Absyn_Enum_p_struct*)_tmp7B8;if(_tmp7CF->tag != 12)goto _LL34C;
else{_tmp7D0=_tmp7CF->f1;_tmp7D1=_tmp7CF->f2;}}_LL34B:{struct Cyc_Absyn_Enum_e_struct
_tmpD97;struct Cyc_Absyn_Enum_e_struct*_tmpD96;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD96=_cycalloc(sizeof(*_tmpD96)),((_tmpD96[0]=((_tmpD97.tag=32,((
_tmpD97.f1=_tmp7D1->name,((_tmpD97.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7D0,((
_tmpD97.f3=(struct Cyc_Absyn_Enumfield*)_tmp7D1,_tmpD97)))))))),_tmpD96)))),0),
fail_stmt);}goto _LL339;_LL34C: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp7D2=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp7B8;if(_tmp7D2->tag != 13)goto _LL34E;else{
_tmp7D3=(void*)_tmp7D2->f1;_tmp7D4=_tmp7D2->f2;}}_LL34D:{struct Cyc_Absyn_AnonEnum_e_struct
_tmpD9A;struct Cyc_Absyn_AnonEnum_e_struct*_tmpD99;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD99=_cycalloc(sizeof(*_tmpD99)),((_tmpD99[0]=((_tmpD9A.tag=33,((
_tmpD9A.f1=_tmp7D4->name,((_tmpD9A.f2=(void*)_tmp7D3,((_tmpD9A.f3=(struct Cyc_Absyn_Enumfield*)
_tmp7D4,_tmpD9A)))))))),_tmpD99)))),0),fail_stmt);}goto _LL339;_LL34E: {struct Cyc_Absyn_Pointer_p_struct*
_tmp7D5=(struct Cyc_Absyn_Pointer_p_struct*)_tmp7B8;if(_tmp7D5->tag != 5)goto
_LL350;else{_tmp7D6=_tmp7D5->f1;_tmp7D7=*_tmp7D6;_tmp7D8=_tmp7D7.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp7D9=(struct Cyc_Absyn_Datatype_p_struct*)_tmp7D8;if(_tmp7D9->tag != 7)goto
_LL350;else{_tmp7DA=_tmp7D9->f1;_tmp7DB=_tmp7D9->f2;_tmp7DC=_tmp7D9->f3;}};}}
_LL34F: s=Cyc_Toc_skip_stmt_dl();{struct _tuple0*_tmp7F5=Cyc_Toc_temp_var();const
char*_tmpD9B;struct _tuple0*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp7DB->name,((
_tmpD9B="_struct",_tag_dyneither(_tmpD9B,sizeof(char),8))));void*_tmp7F6=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);int cnt=1;struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
_tmp7F6,r);struct Cyc_List_List*_tmp7F7=_tmp7DB->typs;for(0;_tmp7DC != 0;(((
_tmp7DC=_tmp7DC->tl,_tmp7F7=((struct Cyc_List_List*)_check_null(_tmp7F7))->tl)),
++ cnt)){struct Cyc_Absyn_Pat*_tmp7F8=(struct Cyc_Absyn_Pat*)_tmp7DC->hd;if(_tmp7F8->r
== (void*)& Cyc_Absyn_Wild_p_val)continue;{void*_tmp7F9=(*((struct _tuple10*)((
struct Cyc_List_List*)_check_null(_tmp7F7))->hd)).f2;struct _tuple0*_tmp7FA=Cyc_Toc_temp_var();
void*_tmp7FB=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp7F8->topt))->v;void*
_tmp7FC=Cyc_Toc_typ_to_c(_tmp7FB);struct Cyc_Absyn_Exp*_tmp7FD=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp7F5,0),Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_boxed_tvar(
Cyc_Toc_typ_to_c(_tmp7F9)))_tmp7FD=Cyc_Toc_cast_it(_tmp7FC,_tmp7FD);{struct
_tuple24*_tmpD9E;struct Cyc_List_List*_tmpD9D;decls=((_tmpD9D=_region_malloc(rgn,
sizeof(*_tmpD9D)),((_tmpD9D->hd=((_tmpD9E=_region_malloc(rgn,sizeof(*_tmpD9E)),((
_tmpD9E->f1=_tmp7FA,((_tmpD9E->f2=_tmp7FC,_tmpD9E)))))),((_tmpD9D->tl=decls,
_tmpD9D))))));}{struct _tuple23 _tmp800=Cyc_Toc_xlate_pat(nv,rgn,_tmp7FB,Cyc_Absyn_var_exp(
_tmp7FA,0),_tmp7FD,_tmp7F8,fail_stmt,decls);nv=_tmp800.f1;decls=_tmp800.f2;{
struct Cyc_Absyn_Stmt*_tmp801=_tmp800.f3;struct Cyc_Absyn_Stmt*_tmp802=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp7FA,0),_tmp7FD,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp802,_tmp801,0),0);};};};}{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp7F5,0),Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*tag_exp=
_tmp7DA->is_extensible?Cyc_Absyn_var_exp(_tmp7DB->name,0): Cyc_Toc_datatype_tag(
_tmp7DA,_tmp7DB->name);struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,
tag_exp,0);s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);if(Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v))s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp7F5,0),Cyc_Absyn_uint_exp(0,0),0),
fail_stmt,s,0);s=Cyc_Absyn_declare_stmt(_tmp7F5,_tmp7F6,(struct Cyc_Absyn_Exp*)
rcast,s,0);goto _LL339;};};_LL350: {struct Cyc_Absyn_Datatype_p_struct*_tmp7DD=(
struct Cyc_Absyn_Datatype_p_struct*)_tmp7B8;if(_tmp7DD->tag != 7)goto _LL352;else{
_tmp7DE=_tmp7DD->f3;}}_LL351: _tmp7E0=_tmp7DE;goto _LL353;_LL352: {struct Cyc_Absyn_Tuple_p_struct*
_tmp7DF=(struct Cyc_Absyn_Tuple_p_struct*)_tmp7B8;if(_tmp7DF->tag != 4)goto _LL354;
else{_tmp7E0=_tmp7DF->f1;}}_LL353: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;
_tmp7E0 != 0;(_tmp7E0=_tmp7E0->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp804=(struct Cyc_Absyn_Pat*)
_tmp7E0->hd;if(_tmp804->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{struct _tuple0*
_tmp805=Cyc_Toc_temp_var();void*_tmp806=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp804->topt))->v;{struct _tuple24*_tmpDA1;struct Cyc_List_List*_tmpDA0;decls=((
_tmpDA0=_region_malloc(rgn,sizeof(*_tmpDA0)),((_tmpDA0->hd=((_tmpDA1=
_region_malloc(rgn,sizeof(*_tmpDA1)),((_tmpDA1->f1=_tmp805,((_tmpDA1->f2=Cyc_Toc_typ_to_c(
_tmp806),_tmpDA1)))))),((_tmpDA0->tl=decls,_tmpDA0))))));}{struct _tuple23 _tmp809=
Cyc_Toc_xlate_pat(nv,rgn,_tmp806,Cyc_Absyn_var_exp(_tmp805,0),Cyc_Absyn_aggrmember_exp(
path,Cyc_Absyn_fieldname(cnt),0),_tmp804,fail_stmt,decls);nv=_tmp809.f1;decls=
_tmp809.f2;{struct Cyc_Absyn_Stmt*_tmp80A=_tmp809.f3;struct Cyc_Absyn_Stmt*_tmp80B=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp805,0),Cyc_Absyn_aggrmember_exp(r,
Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp80B,
_tmp80A,0),0);};};};}goto _LL339;};_LL354: {struct Cyc_Absyn_Aggr_p_struct*_tmp7E1=(
struct Cyc_Absyn_Aggr_p_struct*)_tmp7B8;if(_tmp7E1->tag != 6)goto _LL356;else{
_tmp7E2=_tmp7E1->f1;if(_tmp7E2 != 0)goto _LL356;}}_LL355: {const char*_tmpDA4;void*
_tmpDA3;(_tmpDA3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDA4="unresolved aggregate pattern!",_tag_dyneither(_tmpDA4,sizeof(char),30))),
_tag_dyneither(_tmpDA3,sizeof(void*),0)));}_LL356: {struct Cyc_Absyn_Aggr_p_struct*
_tmp7E3=(struct Cyc_Absyn_Aggr_p_struct*)_tmp7B8;if(_tmp7E3->tag != 6)goto _LL358;
else{_tmp7E4=_tmp7E3->f1;if(_tmp7E4 == 0)goto _LL358;_tmp7E5=*_tmp7E4;_tmp7E6=
_tmp7E5.aggr_info;_tmp7E7=_tmp7E3->f3;}}_LL357: {struct Cyc_Absyn_Aggrdecl*
_tmp80E=Cyc_Absyn_get_known_aggrdecl(_tmp7E6);if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp80E->impl))->tagged){struct _tuple25 _tmp810;struct Cyc_List_List*
_tmp811;struct Cyc_Absyn_Pat*_tmp812;struct _tuple25*_tmp80F=(struct _tuple25*)((
struct Cyc_List_List*)_check_null(_tmp7E7))->hd;_tmp810=*_tmp80F;_tmp811=_tmp810.f1;
_tmp812=_tmp810.f2;{struct _dyneither_ptr*f;{void*_tmp813=(void*)((struct Cyc_List_List*)
_check_null(_tmp811))->hd;struct _dyneither_ptr*_tmp815;_LL361: {struct Cyc_Absyn_FieldName_struct*
_tmp814=(struct Cyc_Absyn_FieldName_struct*)_tmp813;if(_tmp814->tag != 1)goto
_LL363;else{_tmp815=_tmp814->f1;}}_LL362: f=_tmp815;goto _LL360;_LL363:;_LL364: {
const char*_tmpDA7;void*_tmpDA6;(_tmpDA6=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDA7="no field name in tagged union pattern",
_tag_dyneither(_tmpDA7,sizeof(char),38))),_tag_dyneither(_tmpDA6,sizeof(void*),0)));}
_LL360:;}{struct _tuple0*_tmp818=Cyc_Toc_temp_var();void*_tmp819=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp812->topt))->v;void*_tmp81A=Cyc_Toc_typ_to_c(
_tmp819);{struct _tuple24*_tmpDAA;struct Cyc_List_List*_tmpDA9;decls=((_tmpDA9=
_region_malloc(rgn,sizeof(*_tmpDA9)),((_tmpDA9->hd=((_tmpDAA=_region_malloc(rgn,
sizeof(*_tmpDAA)),((_tmpDAA->f1=_tmp818,((_tmpDAA->f2=_tmp81A,_tmpDAA)))))),((
_tmpDA9->tl=decls,_tmpDA9))))));}{struct Cyc_Absyn_Exp*_tmp81D=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp81E=
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp81E=Cyc_Toc_cast_it(_tmp81A,_tmp81E);_tmp81D=Cyc_Toc_cast_it(_tmp81A,_tmp81D);{
struct _tuple23 _tmp81F=Cyc_Toc_xlate_pat(nv,rgn,_tmp819,Cyc_Absyn_var_exp(_tmp818,
0),_tmp81D,_tmp812,fail_stmt,decls);nv=_tmp81F.f1;decls=_tmp81F.f2;{struct Cyc_Absyn_Stmt*
_tmp820=_tmp81F.f3;struct Cyc_Absyn_Stmt*_tmp821=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp80E,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp822=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp818,0),_tmp81E,0);s=Cyc_Absyn_seq_stmt(_tmp821,Cyc_Absyn_seq_stmt(_tmp822,
_tmp820,0),0);};};};};};}else{s=Cyc_Toc_skip_stmt_dl();for(0;_tmp7E7 != 0;_tmp7E7=
_tmp7E7->tl){struct _tuple25*_tmp823=(struct _tuple25*)_tmp7E7->hd;struct Cyc_Absyn_Pat*
_tmp824=(*_tmp823).f2;if(_tmp824->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _dyneither_ptr*f;{void*_tmp825=(void*)((struct Cyc_List_List*)_check_null((*
_tmp823).f1))->hd;struct _dyneither_ptr*_tmp827;_LL366: {struct Cyc_Absyn_FieldName_struct*
_tmp826=(struct Cyc_Absyn_FieldName_struct*)_tmp825;if(_tmp826->tag != 1)goto
_LL368;else{_tmp827=_tmp826->f1;}}_LL367: f=_tmp827;goto _LL365;_LL368:;_LL369: {
struct Cyc_Toc_Match_error_struct _tmpDAD;struct Cyc_Toc_Match_error_struct*_tmpDAC;(
int)_throw((void*)((_tmpDAC=_cycalloc_atomic(sizeof(*_tmpDAC)),((_tmpDAC[0]=((
_tmpDAD.tag=Cyc_Toc_Match_error,_tmpDAD)),_tmpDAC)))));}_LL365:;}{struct _tuple0*
_tmp82A=Cyc_Toc_temp_var();void*_tmp82B=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp824->topt))->v;void*_tmp82C=Cyc_Toc_typ_to_c(_tmp82B);{struct _tuple24*
_tmpDB0;struct Cyc_List_List*_tmpDAF;decls=((_tmpDAF=_region_malloc(rgn,sizeof(*
_tmpDAF)),((_tmpDAF->hd=((_tmpDB0=_region_malloc(rgn,sizeof(*_tmpDB0)),((_tmpDB0->f1=
_tmp82A,((_tmpDB0->f2=_tmp82C,_tmpDB0)))))),((_tmpDAF->tl=decls,_tmpDAF))))));}{
struct _tuple23 _tmp82F=Cyc_Toc_xlate_pat(nv,rgn,_tmp82B,Cyc_Absyn_var_exp(_tmp82A,
0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp824,fail_stmt,decls);nv=_tmp82F.f1;
decls=_tmp82F.f2;{struct Cyc_Absyn_Exp*_tmp830=Cyc_Absyn_aggrmember_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(
Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp80E->impl))->fields,
f)))->type))_tmp830=Cyc_Toc_cast_it(_tmp82C,_tmp830);{struct Cyc_Absyn_Stmt*
_tmp831=_tmp82F.f3;struct Cyc_Absyn_Stmt*_tmp832=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp82A,0),_tmp830,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp832,_tmp831,
0),0);};};};};};}}goto _LL339;}_LL358: {struct Cyc_Absyn_Pointer_p_struct*_tmp7E8=(
struct Cyc_Absyn_Pointer_p_struct*)_tmp7B8;if(_tmp7E8->tag != 5)goto _LL35A;else{
_tmp7E9=_tmp7E8->f1;}}_LL359: {struct _tuple0*_tmp833=Cyc_Toc_temp_var();void*
_tmp834=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp7E9->topt))->v;{struct
_tuple24*_tmpDB3;struct Cyc_List_List*_tmpDB2;decls=((_tmpDB2=_region_malloc(rgn,
sizeof(*_tmpDB2)),((_tmpDB2->hd=((_tmpDB3=_region_malloc(rgn,sizeof(*_tmpDB3)),((
_tmpDB3->f1=_tmp833,((_tmpDB3->f2=Cyc_Toc_typ_to_c(_tmp834),_tmpDB3)))))),((
_tmpDB2->tl=decls,_tmpDB2))))));}{struct _tuple23 _tmp837=Cyc_Toc_xlate_pat(nv,rgn,
_tmp834,Cyc_Absyn_var_exp(_tmp833,0),Cyc_Absyn_deref_exp(path,0),_tmp7E9,
fail_stmt,decls);nv=_tmp837.f1;decls=_tmp837.f2;{struct Cyc_Absyn_Stmt*_tmp838=
_tmp837.f3;struct Cyc_Absyn_Stmt*_tmp839=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp833,0),Cyc_Absyn_deref_exp(r,0),0),_tmp838,0);if(Cyc_Toc_is_nullable(
t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(
0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),_tmp839,0);else{s=_tmp839;}goto
_LL339;};};}_LL35A: {struct Cyc_Absyn_UnknownId_p_struct*_tmp7EA=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp7B8;if(_tmp7EA->tag != 14)goto _LL35C;}_LL35B: {const char*_tmpDB6;void*_tmpDB5;(
_tmpDB5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDB6="unknownid pat",_tag_dyneither(_tmpDB6,sizeof(char),14))),_tag_dyneither(
_tmpDB5,sizeof(void*),0)));}_LL35C: {struct Cyc_Absyn_UnknownCall_p_struct*
_tmp7EB=(struct Cyc_Absyn_UnknownCall_p_struct*)_tmp7B8;if(_tmp7EB->tag != 15)goto
_LL35E;}_LL35D: {const char*_tmpDB9;void*_tmpDB8;(_tmpDB8=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDB9="unknowncall pat",
_tag_dyneither(_tmpDB9,sizeof(char),16))),_tag_dyneither(_tmpDB8,sizeof(void*),0)));}
_LL35E: {struct Cyc_Absyn_Exp_p_struct*_tmp7EC=(struct Cyc_Absyn_Exp_p_struct*)
_tmp7B8;if(_tmp7EC->tag != 16)goto _LL339;}_LL35F: {const char*_tmpDBC;void*_tmpDBB;(
_tmpDBB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDBC="exp pat",_tag_dyneither(_tmpDBC,sizeof(char),8))),_tag_dyneither(
_tmpDBB,sizeof(void*),0)));}_LL339:;}{struct _tuple23 _tmpDBD;return(_tmpDBD.f1=nv,((
_tmpDBD.f2=decls,((_tmpDBD.f3=s,_tmpDBD)))));};}struct _tuple26{struct
_dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
static struct _tuple26*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){struct _tuple26*_tmpDBE;return(_tmpDBE=_region_malloc(r,sizeof(*_tmpDBE)),((
_tmpDBE->f1=Cyc_Toc_fresh_label(),((_tmpDBE->f2=Cyc_Toc_fresh_label(),((_tmpDBE->f3=
sc,_tmpDBE)))))));}static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{
void*_tmp842=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int
leave_as_switch;{void*_tmp843=Cyc_Tcutil_compress(_tmp842);_LL36B: {struct Cyc_Absyn_IntType_struct*
_tmp844=(struct Cyc_Absyn_IntType_struct*)_tmp843;if(_tmp844->tag != 6)goto _LL36D;}
_LL36C: goto _LL36E;_LL36D: {struct Cyc_Absyn_EnumType_struct*_tmp845=(struct Cyc_Absyn_EnumType_struct*)
_tmp843;if(_tmp845->tag != 13)goto _LL36F;}_LL36E: leave_as_switch=1;goto _LL36A;
_LL36F:;_LL370: leave_as_switch=0;goto _LL36A;_LL36A:;}{struct Cyc_List_List*
_tmp846=scs;for(0;_tmp846 != 0;_tmp846=_tmp846->tl){if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp846->hd)->pat_vars))->v
!= 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp846->hd)->where_clause != 0){
leave_as_switch=0;break;}}}if(leave_as_switch){struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp847=scs;for(0;_tmp847 != 0;_tmp847=_tmp847->tl){struct Cyc_Absyn_Stmt*
_tmp848=((struct Cyc_Absyn_Switch_clause*)_tmp847->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp847->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp848,0);next_l=Cyc_Toc_fresh_label();{
struct Cyc_Toc_Env _tmp84A;struct _RegionHandle*_tmp84B;struct Cyc_Toc_Env*_tmp849=
nv;_tmp84A=*_tmp849;_tmp84B=_tmp84A.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
_tmp84B,nv,next_l),_tmp848);};}}return;}{struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp84D;
struct _RegionHandle*_tmp84E;struct Cyc_Toc_Env*_tmp84C=nv;_tmp84D=*_tmp84C;
_tmp84E=_tmp84D.rgn;{struct Cyc_Toc_Env*_tmp84F=Cyc_Toc_share_env(_tmp84E,nv);
struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple26*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp84E,Cyc_Toc_gen_label,
_tmp84E,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct
Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp850=lscs;for(0;_tmp850 != 0;
_tmp850=_tmp850->tl){struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple26*)
_tmp850->hd)).f3;struct _dyneither_ptr*fail_lab=_tmp850->tl == 0?end_l:(*((struct
_tuple26*)((struct Cyc_List_List*)_check_null(_tmp850->tl))->hd)).f1;struct Cyc_Toc_Env*
_tmp852;struct Cyc_List_List*_tmp853;struct Cyc_Absyn_Stmt*_tmp854;struct _tuple23
_tmp851=Cyc_Toc_xlate_pat(_tmp84F,_tmp84E,_tmp842,r,path,sc->pattern,Cyc_Absyn_goto_stmt(
fail_lab,0),decls);_tmp852=_tmp851.f1;_tmp853=_tmp851.f2;_tmp854=_tmp851.f3;if(
sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp855=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp852,_tmp855);_tmp854=Cyc_Absyn_seq_stmt(
_tmp854,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp855,0),
Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp853;{
struct Cyc_List_List*_tmpDBF;nvs=((_tmpDBF=_region_malloc(_tmp84E,sizeof(*_tmpDBF)),((
_tmpDBF->hd=_tmp852,((_tmpDBF->tl=nvs,_tmpDBF))))));}{struct Cyc_List_List*
_tmpDC0;test_stmts=((_tmpDC0=_region_malloc(_tmp84E,sizeof(*_tmpDC0)),((_tmpDC0->hd=
_tmp854,((_tmpDC0->tl=test_stmts,_tmpDC0))))));};}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple26 _tmp859;struct _dyneither_ptr*_tmp85A;struct _dyneither_ptr*_tmp85B;
struct Cyc_Absyn_Switch_clause*_tmp85C;struct _tuple26*_tmp858=(struct _tuple26*)
lscs->hd;_tmp859=*_tmp858;_tmp85A=_tmp859.f1;_tmp85B=_tmp859.f2;_tmp85C=_tmp859.f3;{
struct Cyc_Toc_Env*_tmp85D=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp85C->body;struct Cyc_Toc_Env _tmp85F;struct
_RegionHandle*_tmp860;struct Cyc_Toc_Env*_tmp85E=_tmp84F;_tmp85F=*_tmp85E;_tmp860=
_tmp85F.rgn;if(lscs->tl != 0){struct _tuple26 _tmp862;struct _dyneither_ptr*_tmp863;
struct Cyc_Absyn_Switch_clause*_tmp864;struct _tuple26*_tmp861=(struct _tuple26*)((
struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp862=*_tmp861;_tmp863=_tmp862.f2;
_tmp864=_tmp862.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp860,
_tmp85D,end_l,_tmp863,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp864->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp860,
_tmp85D,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp85A,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp85B,s,0),0);{struct Cyc_List_List*_tmpDC1;stmts=((_tmpDC1=_region_malloc(
_tmp84E,sizeof(*_tmpDC1)),((_tmpDC1->hd=s,((_tmpDC1->tl=stmts,_tmpDC1))))));};};}{
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,
Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple24
_tmp867;struct _tuple0*_tmp868;void*_tmp869;struct _tuple24*_tmp866=(struct
_tuple24*)decls->hd;_tmp867=*_tmp866;_tmp868=_tmp867.f1;_tmp869=_tmp867.f2;res=
Cyc_Absyn_declare_stmt(_tmp868,_tmp869,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(
v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(
struct Cyc_Absyn_Exp*)e,res,0))->r;};};};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){struct Cyc_List_List*
_tmpDC2;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,((
_tmpDC2=_cycalloc(sizeof(*_tmpDC2)),((_tmpDC2->hd=Cyc_Absyn_uint_exp((
unsigned int)(n - 1),0),((_tmpDC2->tl=0,_tmpDC2)))))),0),0);}void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s){while(1){void*_tmp86B=s->r;struct Cyc_Absyn_Exp*_tmp86E;
struct Cyc_Absyn_Stmt*_tmp870;struct Cyc_Absyn_Stmt*_tmp871;struct Cyc_Absyn_Exp*
_tmp873;struct Cyc_Absyn_Exp*_tmp875;struct Cyc_Absyn_Stmt*_tmp876;struct Cyc_Absyn_Stmt*
_tmp877;struct _tuple9 _tmp879;struct Cyc_Absyn_Exp*_tmp87A;struct Cyc_Absyn_Stmt*
_tmp87B;struct Cyc_Absyn_Stmt*_tmp87D;struct Cyc_Absyn_Stmt*_tmp87F;struct Cyc_Absyn_Stmt*
_tmp881;struct Cyc_Absyn_Exp*_tmp883;struct _tuple9 _tmp884;struct Cyc_Absyn_Exp*
_tmp885;struct _tuple9 _tmp886;struct Cyc_Absyn_Exp*_tmp887;struct Cyc_Absyn_Stmt*
_tmp888;struct Cyc_Absyn_Exp*_tmp88A;struct Cyc_List_List*_tmp88B;struct Cyc_List_List*
_tmp88D;struct Cyc_Absyn_Switch_clause**_tmp88E;struct Cyc_Absyn_Decl*_tmp890;
struct Cyc_Absyn_Stmt*_tmp891;struct _dyneither_ptr*_tmp893;struct Cyc_Absyn_Stmt*
_tmp894;struct Cyc_Absyn_Stmt*_tmp896;struct _tuple9 _tmp897;struct Cyc_Absyn_Exp*
_tmp898;struct Cyc_Absyn_Stmt*_tmp89A;struct Cyc_List_List*_tmp89B;struct Cyc_Absyn_Exp*
_tmp89D;_LL372: {struct Cyc_Absyn_Skip_s_struct*_tmp86C=(struct Cyc_Absyn_Skip_s_struct*)
_tmp86B;if(_tmp86C->tag != 0)goto _LL374;}_LL373: return;_LL374: {struct Cyc_Absyn_Exp_s_struct*
_tmp86D=(struct Cyc_Absyn_Exp_s_struct*)_tmp86B;if(_tmp86D->tag != 1)goto _LL376;
else{_tmp86E=_tmp86D->f1;}}_LL375: Cyc_Toc_exp_to_c(nv,_tmp86E);return;_LL376: {
struct Cyc_Absyn_Seq_s_struct*_tmp86F=(struct Cyc_Absyn_Seq_s_struct*)_tmp86B;if(
_tmp86F->tag != 2)goto _LL378;else{_tmp870=_tmp86F->f1;_tmp871=_tmp86F->f2;}}
_LL377: Cyc_Toc_stmt_to_c(nv,_tmp870);s=_tmp871;continue;_LL378: {struct Cyc_Absyn_Return_s_struct*
_tmp872=(struct Cyc_Absyn_Return_s_struct*)_tmp86B;if(_tmp872->tag != 3)goto _LL37A;
else{_tmp873=_tmp872->f1;}}_LL379: {struct Cyc_Core_Opt*topt=0;if(_tmp873 != 0){{
struct Cyc_Core_Opt*_tmpDC3;topt=((_tmpDC3=_cycalloc(sizeof(*_tmpDC3)),((_tmpDC3->v=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp873->topt))->v),
_tmpDC3))));}Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp873);}if(s->try_depth
> 0){if(topt != 0){struct _tuple0*_tmp89F=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*
_tmp8A0=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp89F,0),
0);s->r=(Cyc_Absyn_declare_stmt(_tmp89F,(void*)topt->v,_tmp873,Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler(s->try_depth),_tmp8A0,0),0))->r;}else{Cyc_Toc_do_npop_before(
s->try_depth,s);}}return;}_LL37A: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp874=(
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp86B;if(_tmp874->tag != 4)goto _LL37C;
else{_tmp875=_tmp874->f1;_tmp876=_tmp874->f2;_tmp877=_tmp874->f3;}}_LL37B: Cyc_Toc_exp_to_c(
nv,_tmp875);Cyc_Toc_stmt_to_c(nv,_tmp876);s=_tmp877;continue;_LL37C: {struct Cyc_Absyn_While_s_struct*
_tmp878=(struct Cyc_Absyn_While_s_struct*)_tmp86B;if(_tmp878->tag != 5)goto _LL37E;
else{_tmp879=_tmp878->f1;_tmp87A=_tmp879.f1;_tmp87B=_tmp878->f2;}}_LL37D: Cyc_Toc_exp_to_c(
nv,_tmp87A);{struct Cyc_Toc_Env _tmp8A2;struct _RegionHandle*_tmp8A3;struct Cyc_Toc_Env*
_tmp8A1=nv;_tmp8A2=*_tmp8A1;_tmp8A3=_tmp8A2.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp8A3,nv),_tmp87B);return;};_LL37E: {struct Cyc_Absyn_Break_s_struct*_tmp87C=(
struct Cyc_Absyn_Break_s_struct*)_tmp86B;if(_tmp87C->tag != 6)goto _LL380;else{
_tmp87D=_tmp87C->f1;}}_LL37F: {struct Cyc_Toc_Env _tmp8A5;struct _dyneither_ptr**
_tmp8A6;struct Cyc_Toc_Env*_tmp8A4=nv;_tmp8A5=*_tmp8A4;_tmp8A6=_tmp8A5.break_lab;
if(_tmp8A6 != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp8A6,0);{int dest_depth=_tmp87D == 0?0:
_tmp87D->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;};}
_LL380: {struct Cyc_Absyn_Continue_s_struct*_tmp87E=(struct Cyc_Absyn_Continue_s_struct*)
_tmp86B;if(_tmp87E->tag != 7)goto _LL382;else{_tmp87F=_tmp87E->f1;}}_LL381: {
struct Cyc_Toc_Env _tmp8A8;struct _dyneither_ptr**_tmp8A9;struct Cyc_Toc_Env*_tmp8A7=
nv;_tmp8A8=*_tmp8A7;_tmp8A9=_tmp8A8.continue_lab;if(_tmp8A9 != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp8A9,0);_tmp881=_tmp87F;goto _LL383;}_LL382: {struct Cyc_Absyn_Goto_s_struct*
_tmp880=(struct Cyc_Absyn_Goto_s_struct*)_tmp86B;if(_tmp880->tag != 8)goto _LL384;
else{_tmp881=_tmp880->f2;}}_LL383: Cyc_Toc_do_npop_before(s->try_depth - ((struct
Cyc_Absyn_Stmt*)_check_null(_tmp881))->try_depth,s);return;_LL384: {struct Cyc_Absyn_For_s_struct*
_tmp882=(struct Cyc_Absyn_For_s_struct*)_tmp86B;if(_tmp882->tag != 9)goto _LL386;
else{_tmp883=_tmp882->f1;_tmp884=_tmp882->f2;_tmp885=_tmp884.f1;_tmp886=_tmp882->f3;
_tmp887=_tmp886.f1;_tmp888=_tmp882->f4;}}_LL385: Cyc_Toc_exp_to_c(nv,_tmp883);Cyc_Toc_exp_to_c(
nv,_tmp885);Cyc_Toc_exp_to_c(nv,_tmp887);{struct Cyc_Toc_Env _tmp8AB;struct
_RegionHandle*_tmp8AC;struct Cyc_Toc_Env*_tmp8AA=nv;_tmp8AB=*_tmp8AA;_tmp8AC=
_tmp8AB.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp8AC,nv),_tmp888);return;};
_LL386: {struct Cyc_Absyn_Switch_s_struct*_tmp889=(struct Cyc_Absyn_Switch_s_struct*)
_tmp86B;if(_tmp889->tag != 10)goto _LL388;else{_tmp88A=_tmp889->f1;_tmp88B=_tmp889->f2;}}
_LL387: Cyc_Toc_xlate_switch(nv,s,_tmp88A,_tmp88B);return;_LL388: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp88C=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp86B;if(_tmp88C->tag != 11)goto
_LL38A;else{_tmp88D=_tmp88C->f1;_tmp88E=_tmp88C->f2;}}_LL389: {struct Cyc_Toc_Env
_tmp8AE;struct Cyc_Toc_FallthruInfo*_tmp8AF;struct Cyc_Toc_Env*_tmp8AD=nv;_tmp8AE=*
_tmp8AD;_tmp8AF=_tmp8AE.fallthru_info;if(_tmp8AF == 0){const char*_tmpDC6;void*
_tmpDC5;(_tmpDC5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDC6="fallthru in unexpected place",_tag_dyneither(_tmpDC6,sizeof(char),29))),
_tag_dyneither(_tmpDC5,sizeof(void*),0)));}{struct _dyneither_ptr*_tmp8B3;struct
Cyc_List_List*_tmp8B4;struct Cyc_Dict_Dict _tmp8B5;struct Cyc_Toc_FallthruInfo
_tmp8B2=*_tmp8AF;_tmp8B3=_tmp8B2.label;_tmp8B4=_tmp8B2.binders;_tmp8B5=_tmp8B2.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp8B3,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp88E)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp8B6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp8B4);struct Cyc_List_List*_tmp8B7=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp88D);for(0;_tmp8B6 != 0;(_tmp8B6=_tmp8B6->tl,
_tmp8B7=_tmp8B7->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp8B7))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp8B5,(
struct _tuple0*)_tmp8B6->hd),(struct Cyc_Absyn_Exp*)_tmp8B7->hd,0),s2,0);}s->r=s2->r;
return;};};};}_LL38A: {struct Cyc_Absyn_Decl_s_struct*_tmp88F=(struct Cyc_Absyn_Decl_s_struct*)
_tmp86B;if(_tmp88F->tag != 12)goto _LL38C;else{_tmp890=_tmp88F->f1;_tmp891=_tmp88F->f2;}}
_LL38B:{void*_tmp8B8=_tmp890->r;struct Cyc_Absyn_Vardecl*_tmp8BA;struct Cyc_Absyn_Pat*
_tmp8BC;struct Cyc_Absyn_Exp*_tmp8BD;struct Cyc_List_List*_tmp8BF;struct Cyc_Absyn_Fndecl*
_tmp8C1;struct Cyc_Absyn_Tvar*_tmp8C3;struct Cyc_Absyn_Vardecl*_tmp8C4;int _tmp8C5;
struct Cyc_Absyn_Exp*_tmp8C6;struct Cyc_Absyn_Exp*_tmp8C8;struct Cyc_Absyn_Tvar*
_tmp8C9;struct Cyc_Absyn_Vardecl*_tmp8CA;_LL395: {struct Cyc_Absyn_Var_d_struct*
_tmp8B9=(struct Cyc_Absyn_Var_d_struct*)_tmp8B8;if(_tmp8B9->tag != 0)goto _LL397;
else{_tmp8BA=_tmp8B9->f1;}}_LL396: {struct Cyc_Toc_Env _tmp8CC;struct _RegionHandle*
_tmp8CD;struct Cyc_Toc_Env*_tmp8CB=nv;_tmp8CC=*_tmp8CB;_tmp8CD=_tmp8CC.rgn;{
struct Cyc_Absyn_Local_b_struct _tmpDC9;struct Cyc_Absyn_Local_b_struct*_tmpDC8;
struct Cyc_Toc_Env*_tmp8CE=Cyc_Toc_add_varmap(_tmp8CD,nv,_tmp8BA->name,Cyc_Absyn_varb_exp(
_tmp8BA->name,(void*)((_tmpDC8=_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8[0]=((
_tmpDC9.tag=4,((_tmpDC9.f1=_tmp8BA,_tmpDC9)))),_tmpDC8)))),0));Cyc_Toc_local_decl_to_c(
_tmp8CE,_tmp8CE,_tmp8BA,_tmp891);}goto _LL394;}_LL397: {struct Cyc_Absyn_Let_d_struct*
_tmp8BB=(struct Cyc_Absyn_Let_d_struct*)_tmp8B8;if(_tmp8BB->tag != 2)goto _LL399;
else{_tmp8BC=_tmp8BB->f1;_tmp8BD=_tmp8BB->f3;}}_LL398:{void*_tmp8D1=_tmp8BC->r;
struct Cyc_Absyn_Vardecl*_tmp8D3;struct Cyc_Absyn_Pat*_tmp8D4;struct Cyc_Absyn_Pat
_tmp8D5;void*_tmp8D6;_LL3A4: {struct Cyc_Absyn_Var_p_struct*_tmp8D2=(struct Cyc_Absyn_Var_p_struct*)
_tmp8D1;if(_tmp8D2->tag != 1)goto _LL3A6;else{_tmp8D3=_tmp8D2->f1;_tmp8D4=_tmp8D2->f2;
_tmp8D5=*_tmp8D4;_tmp8D6=_tmp8D5.r;{struct Cyc_Absyn_Wild_p_struct*_tmp8D7=(
struct Cyc_Absyn_Wild_p_struct*)_tmp8D6;if(_tmp8D7->tag != 0)goto _LL3A6;};}}_LL3A5: {
struct _tuple0*old_name=_tmp8D3->name;struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp8D3->name=new_name;_tmp8D3->initializer=(struct Cyc_Absyn_Exp*)_tmp8BD;{
struct Cyc_Absyn_Var_d_struct _tmpDCC;struct Cyc_Absyn_Var_d_struct*_tmpDCB;_tmp890->r=(
void*)((_tmpDCB=_cycalloc(sizeof(*_tmpDCB)),((_tmpDCB[0]=((_tmpDCC.tag=0,((
_tmpDCC.f1=_tmp8D3,_tmpDCC)))),_tmpDCB))));}{struct Cyc_Toc_Env _tmp8DB;struct
_RegionHandle*_tmp8DC;struct Cyc_Toc_Env*_tmp8DA=nv;_tmp8DB=*_tmp8DA;_tmp8DC=
_tmp8DB.rgn;{struct Cyc_Absyn_Local_b_struct _tmpDCF;struct Cyc_Absyn_Local_b_struct*
_tmpDCE;struct Cyc_Toc_Env*_tmp8DD=Cyc_Toc_add_varmap(_tmp8DC,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpDCE=_cycalloc(sizeof(*_tmpDCE)),((_tmpDCE[0]=((_tmpDCF.tag=
4,((_tmpDCF.f1=_tmp8D3,_tmpDCF)))),_tmpDCE)))),0));Cyc_Toc_local_decl_to_c(
_tmp8DD,nv,_tmp8D3,_tmp891);}goto _LL3A3;};}_LL3A6:;_LL3A7: s->r=(Cyc_Toc_letdecl_to_c(
nv,_tmp8BC,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp8BD->topt))->v,_tmp8BD,
_tmp891))->r;goto _LL3A3;_LL3A3:;}goto _LL394;_LL399: {struct Cyc_Absyn_Letv_d_struct*
_tmp8BE=(struct Cyc_Absyn_Letv_d_struct*)_tmp8B8;if(_tmp8BE->tag != 3)goto _LL39B;
else{_tmp8BF=_tmp8BE->f1;}}_LL39A: {struct Cyc_List_List*_tmp8E0=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp8BF);if(_tmp8E0 == 0){const char*_tmpDD2;
void*_tmpDD1;(_tmpDD1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpDD2="empty Letv_d",_tag_dyneither(_tmpDD2,sizeof(char),
13))),_tag_dyneither(_tmpDD1,sizeof(void*),0)));}{struct Cyc_Absyn_Var_d_struct
_tmpDD5;struct Cyc_Absyn_Var_d_struct*_tmpDD4;_tmp890->r=(void*)((_tmpDD4=
_cycalloc(sizeof(*_tmpDD4)),((_tmpDD4[0]=((_tmpDD5.tag=0,((_tmpDD5.f1=(struct Cyc_Absyn_Vardecl*)
_tmp8E0->hd,_tmpDD5)))),_tmpDD4))));}_tmp8E0=_tmp8E0->tl;for(0;_tmp8E0 != 0;
_tmp8E0=_tmp8E0->tl){struct Cyc_Absyn_Var_d_struct _tmpDD8;struct Cyc_Absyn_Var_d_struct*
_tmpDD7;struct Cyc_Absyn_Decl*_tmp8E5=Cyc_Absyn_new_decl((void*)((_tmpDD7=
_cycalloc(sizeof(*_tmpDD7)),((_tmpDD7[0]=((_tmpDD8.tag=0,((_tmpDD8.f1=(struct Cyc_Absyn_Vardecl*)
_tmp8E0->hd,_tmpDD8)))),_tmpDD7)))),0);s->r=(Cyc_Absyn_decl_stmt(_tmp8E5,Cyc_Absyn_new_stmt(
s->r,0),0))->r;}Cyc_Toc_stmt_to_c(nv,s);goto _LL394;}_LL39B: {struct Cyc_Absyn_Fn_d_struct*
_tmp8C0=(struct Cyc_Absyn_Fn_d_struct*)_tmp8B8;if(_tmp8C0->tag != 1)goto _LL39D;
else{_tmp8C1=_tmp8C0->f1;}}_LL39C: {struct _tuple0*_tmp8E8=_tmp8C1->name;struct
Cyc_Toc_Env _tmp8EA;struct _RegionHandle*_tmp8EB;struct Cyc_Toc_Env*_tmp8E9=nv;
_tmp8EA=*_tmp8E9;_tmp8EB=_tmp8EA.rgn;{struct Cyc_Toc_Env*_tmp8EC=Cyc_Toc_add_varmap(
_tmp8EB,nv,_tmp8C1->name,Cyc_Absyn_var_exp(_tmp8E8,0));Cyc_Toc_fndecl_to_c(
_tmp8EC,_tmp8C1,0);Cyc_Toc_stmt_to_c(_tmp8EC,_tmp891);}goto _LL394;}_LL39D: {
struct Cyc_Absyn_Region_d_struct*_tmp8C2=(struct Cyc_Absyn_Region_d_struct*)
_tmp8B8;if(_tmp8C2->tag != 4)goto _LL39F;else{_tmp8C3=_tmp8C2->f1;_tmp8C4=_tmp8C2->f2;
_tmp8C5=_tmp8C2->f3;_tmp8C6=_tmp8C2->f4;}}_LL39E: {struct Cyc_Absyn_Stmt*_tmp8ED=
_tmp891;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple0*
rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp8C4->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp8EF;struct _RegionHandle*_tmp8F0;struct Cyc_Toc_Env*
_tmp8EE=nv;_tmp8EF=*_tmp8EE;_tmp8F0=_tmp8EF.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp8F0,nv,x_var,x_exp),_tmp8ED);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp8ED,0))->r;
else{if(_tmp8C6 == 0){struct Cyc_Absyn_Exp*_tmpDDB[1];struct Cyc_Absyn_Exp*_tmpDDA[
1];struct Cyc_List_List*_tmpDD9;s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpDD9=
_cycalloc(sizeof(*_tmpDD9)),((_tmpDD9->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0),((_tmpDD9->tl=0,_tmpDD9)))))),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpDDA[0]=
x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDDA,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp8ED,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpDDB[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDDB,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp8C6);{struct Cyc_Absyn_Exp*
_tmpDDD[1];struct Cyc_Absyn_Exp*_tmpDDC[2];s->r=(Cyc_Absyn_declare_stmt(rh_var,
Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpDDC[1]=(struct Cyc_Absyn_Exp*)_tmp8C6,((_tmpDDC[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDDC,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp8ED,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpDDD[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDDD,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
return;}_LL39F: {struct Cyc_Absyn_Alias_d_struct*_tmp8C7=(struct Cyc_Absyn_Alias_d_struct*)
_tmp8B8;if(_tmp8C7->tag != 5)goto _LL3A1;else{_tmp8C8=_tmp8C7->f1;_tmp8C9=_tmp8C7->f2;
_tmp8CA=_tmp8C7->f3;}}_LL3A0: {struct _tuple0*old_name=_tmp8CA->name;struct
_tuple0*new_name=Cyc_Toc_temp_var();_tmp8CA->name=new_name;_tmp8CA->initializer=(
struct Cyc_Absyn_Exp*)_tmp8C8;{struct Cyc_Absyn_Decl_s_struct _tmpDEC;struct Cyc_Absyn_Var_d_struct*
_tmpDEB;struct Cyc_Absyn_Var_d_struct _tmpDEA;struct Cyc_Absyn_Decl*_tmpDE9;struct
Cyc_Absyn_Decl_s_struct*_tmpDE8;s->r=(void*)((_tmpDE8=_cycalloc(sizeof(*_tmpDE8)),((
_tmpDE8[0]=((_tmpDEC.tag=12,((_tmpDEC.f1=((_tmpDE9=_cycalloc(sizeof(*_tmpDE9)),((
_tmpDE9->r=(void*)((_tmpDEB=_cycalloc(sizeof(*_tmpDEB)),((_tmpDEB[0]=((_tmpDEA.tag=
0,((_tmpDEA.f1=_tmp8CA,_tmpDEA)))),_tmpDEB)))),((_tmpDE9->loc=0,_tmpDE9)))))),((
_tmpDEC.f2=_tmp891,_tmpDEC)))))),_tmpDE8))));}{struct Cyc_Toc_Env _tmp8FC;struct
_RegionHandle*_tmp8FD;struct Cyc_Toc_Env*_tmp8FB=nv;_tmp8FC=*_tmp8FB;_tmp8FD=
_tmp8FC.rgn;{struct Cyc_Absyn_Local_b_struct _tmpDEF;struct Cyc_Absyn_Local_b_struct*
_tmpDEE;struct Cyc_Toc_Env*_tmp8FE=Cyc_Toc_add_varmap(_tmp8FD,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpDEE=_cycalloc(sizeof(*_tmpDEE)),((_tmpDEE[0]=((_tmpDEF.tag=
4,((_tmpDEF.f1=_tmp8CA,_tmpDEF)))),_tmpDEE)))),0));Cyc_Toc_local_decl_to_c(
_tmp8FE,nv,_tmp8CA,_tmp891);}return;};}_LL3A1:;_LL3A2: {const char*_tmpDF2;void*
_tmpDF1;(_tmpDF1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpDF2="bad nested declaration within function",_tag_dyneither(_tmpDF2,sizeof(
char),39))),_tag_dyneither(_tmpDF1,sizeof(void*),0)));}_LL394:;}return;_LL38C: {
struct Cyc_Absyn_Label_s_struct*_tmp892=(struct Cyc_Absyn_Label_s_struct*)_tmp86B;
if(_tmp892->tag != 13)goto _LL38E;else{_tmp893=_tmp892->f1;_tmp894=_tmp892->f2;}}
_LL38D: s=_tmp894;continue;_LL38E: {struct Cyc_Absyn_Do_s_struct*_tmp895=(struct
Cyc_Absyn_Do_s_struct*)_tmp86B;if(_tmp895->tag != 14)goto _LL390;else{_tmp896=
_tmp895->f1;_tmp897=_tmp895->f2;_tmp898=_tmp897.f1;}}_LL38F: {struct Cyc_Toc_Env
_tmp904;struct _RegionHandle*_tmp905;struct Cyc_Toc_Env*_tmp903=nv;_tmp904=*
_tmp903;_tmp905=_tmp904.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp905,nv),
_tmp896);Cyc_Toc_exp_to_c(nv,_tmp898);return;}_LL390: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp899=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp86B;if(_tmp899->tag != 15)goto
_LL392;else{_tmp89A=_tmp899->f1;_tmp89B=_tmp899->f2;}}_LL391: {struct _tuple0*
h_var=Cyc_Toc_temp_var();struct _tuple0*e_var=Cyc_Toc_temp_var();struct _tuple0*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);{struct Cyc_Core_Opt*_tmpDF3;
e_exp->topt=((_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((_tmpDF3->v=e_typ,_tmpDF3))));}{
struct Cyc_Toc_Env _tmp908;struct _RegionHandle*_tmp909;struct Cyc_Toc_Env*_tmp907=
nv;_tmp908=*_tmp907;_tmp909=_tmp908.rgn;{struct Cyc_Toc_Env*_tmp90A=Cyc_Toc_add_varmap(
_tmp909,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp90A,_tmp89A);{struct Cyc_Absyn_Stmt*
_tmp90B=Cyc_Absyn_seq_stmt(_tmp89A,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple0*_tmp90C=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp90D=
Cyc_Absyn_var_exp(_tmp90C,0);struct Cyc_Absyn_Vardecl*_tmp90E=Cyc_Absyn_new_vardecl(
_tmp90C,Cyc_Absyn_exn_typ(),0);{struct Cyc_Core_Opt*_tmpDF4;_tmp90D->topt=((
_tmpDF4=_cycalloc(sizeof(*_tmpDF4)),((_tmpDF4->v=Cyc_Absyn_exn_typ(),_tmpDF4))));}{
struct Cyc_Core_Opt*_tmpE05;struct Cyc_Absyn_Var_p_struct*_tmpE04;struct Cyc_Absyn_Pat*
_tmpE03;struct Cyc_Core_Opt*_tmpE02;struct Cyc_Absyn_Var_p_struct _tmpE01;struct Cyc_Absyn_Pat*
_tmpE00;struct Cyc_Absyn_Pat*_tmp910=(_tmpE00=_cycalloc(sizeof(*_tmpE00)),((
_tmpE00->r=(void*)((_tmpE04=_cycalloc(sizeof(*_tmpE04)),((_tmpE04[0]=((_tmpE01.tag=
1,((_tmpE01.f1=_tmp90E,((_tmpE01.f2=((_tmpE03=_cycalloc(sizeof(*_tmpE03)),((
_tmpE03->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpE03->topt=((_tmpE02=_cycalloc(
sizeof(*_tmpE02)),((_tmpE02->v=Cyc_Absyn_exn_typ(),_tmpE02)))),((_tmpE03->loc=0,
_tmpE03)))))))),_tmpE01)))))),_tmpE04)))),((_tmpE00->topt=((_tmpE05=_cycalloc(
sizeof(*_tmpE05)),((_tmpE05->v=Cyc_Absyn_exn_typ(),_tmpE05)))),((_tmpE00->loc=0,
_tmpE00)))))));struct Cyc_Absyn_Exp*_tmp911=Cyc_Absyn_throw_exp(_tmp90D,0);{
struct Cyc_Core_Opt*_tmpE06;_tmp911->topt=((_tmpE06=_cycalloc(sizeof(*_tmpE06)),((
_tmpE06->v=(void*)& Cyc_Absyn_VoidType_val,_tmpE06))));}{struct Cyc_Absyn_Stmt*
_tmp913=Cyc_Absyn_exp_stmt(_tmp911,0);struct Cyc_Core_Opt*_tmpE0C;struct Cyc_List_List*
_tmpE0B;struct Cyc_Absyn_Switch_clause*_tmpE0A;struct Cyc_Absyn_Switch_clause*
_tmp914=(_tmpE0A=_cycalloc(sizeof(*_tmpE0A)),((_tmpE0A->pattern=_tmp910,((
_tmpE0A->pat_vars=((_tmpE0C=_cycalloc(sizeof(*_tmpE0C)),((_tmpE0C->v=((_tmpE0B=
_cycalloc(sizeof(*_tmpE0B)),((_tmpE0B->hd=_tmp90E,((_tmpE0B->tl=0,_tmpE0B)))))),
_tmpE0C)))),((_tmpE0A->where_clause=0,((_tmpE0A->body=_tmp913,((_tmpE0A->loc=0,
_tmpE0A)))))))))));struct Cyc_List_List*_tmpE0D;struct Cyc_Absyn_Stmt*_tmp915=Cyc_Absyn_switch_stmt(
e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp89B,((_tmpE0D=_cycalloc(sizeof(*_tmpE0D)),((_tmpE0D->hd=_tmp914,((_tmpE0D->tl=
0,_tmpE0D))))))),0);Cyc_Toc_stmt_to_c(_tmp90A,_tmp915);{struct Cyc_List_List*
_tmpE0E;struct Cyc_Absyn_Exp*_tmp916=Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,((
_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E->hd=Cyc_Absyn_aggrmember_exp(h_exp,
Cyc_Toc_handler_sp,0),((_tmpE0E->tl=0,_tmpE0E)))))),0);struct Cyc_List_List*
_tmpE0F;struct Cyc_Absyn_Stmt*_tmp917=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpE0F=_cycalloc(sizeof(*_tmpE0F)),((_tmpE0F->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpE0F->tl=0,_tmpE0F)))))),0),0);struct Cyc_Absyn_Exp*_tmp918=Cyc_Absyn_int_exp(
Cyc_Absyn_Signed,0,0);struct Cyc_Absyn_Exp*_tmp919=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
1,0);s->r=(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp917,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp918,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp916,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp919,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp90B,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp915,0),0),0),0),0),0))->r;};};};};}
return;};}_LL392: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp89C=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp86B;if(_tmp89C->tag != 16)goto _LL371;else{_tmp89D=_tmp89C->f1;}}_LL393: if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp89D);{struct Cyc_List_List*
_tmpE10;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((
_tmpE10=_cycalloc(sizeof(*_tmpE10)),((_tmpE10->hd=_tmp89D,((_tmpE10->tl=0,
_tmpE10)))))),0));};}return;_LL371:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);struct _tuple27{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp927=_new_region("frgn");struct _RegionHandle*frgn=& _tmp927;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp928=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp929=f->args;for(0;_tmp929 != 0;_tmp929=_tmp929->tl){struct
_tuple0*_tmpE11;struct _tuple0*_tmp92A=(_tmpE11=_cycalloc(sizeof(*_tmpE11)),((
_tmpE11->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpE11->f2=(*((struct
_tuple27*)_tmp929->hd)).f1,_tmpE11)))));(*((struct _tuple27*)_tmp929->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple27*)_tmp929->hd)).f3);_tmp928=Cyc_Toc_add_varmap(frgn,_tmp928,
_tmp92A,Cyc_Absyn_var_exp(_tmp92A,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp92D;struct Cyc_Absyn_Tqual
_tmp92E;void*_tmp92F;int _tmp930;struct Cyc_Absyn_VarargInfo _tmp92C=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp92D=_tmp92C.name;_tmp92E=_tmp92C.tq;_tmp92F=
_tmp92C.type;_tmp930=_tmp92C.inject;{void*_tmp931=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp92F,(void*)& Cyc_Absyn_HeapRgn_val,_tmp92E,Cyc_Absyn_false_conref));struct
_tuple0*_tmpE12;struct _tuple0*_tmp932=(_tmpE12=_cycalloc(sizeof(*_tmpE12)),((
_tmpE12->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpE12->f2=(struct
_dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null(_tmp92D))->v,_tmpE12)))));{
struct _tuple27*_tmpE15;struct Cyc_List_List*_tmpE14;f->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpE14=
_cycalloc(sizeof(*_tmpE14)),((_tmpE14->hd=((_tmpE15=_cycalloc(sizeof(*_tmpE15)),((
_tmpE15->f1=(struct _dyneither_ptr*)_tmp92D->v,((_tmpE15->f2=_tmp92E,((_tmpE15->f3=
_tmp931,_tmpE15)))))))),((_tmpE14->tl=0,_tmpE14)))))));}_tmp928=Cyc_Toc_add_varmap(
frgn,_tmp928,_tmp932,Cyc_Absyn_var_exp(_tmp932,0));f->cyc_varargs=0;};}{struct
Cyc_List_List*_tmp936=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp936 != 0;_tmp936=_tmp936->tl){((struct Cyc_Absyn_Vardecl*)_tmp936->hd)->type=
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp936->hd)->type);}}Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel(frgn,_tmp928),f->body);};_pop_region(frgn);};}static enum 
Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){switch(s){case Cyc_Absyn_Abstract:
_LL3A8: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3A9: return Cyc_Absyn_Extern;
default: _LL3AA: return s;}}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad,int add_to_decls){struct _tuple0*_tmp937=ad->name;struct _DynRegionHandle*
_tmp939;struct Cyc_Dict_Dict*_tmp93A;struct Cyc_Toc_TocState _tmp938=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp939=_tmp938.dyn;_tmp93A=_tmp938.aggrs_so_far;{
struct _DynRegionFrame _tmp93B;struct _RegionHandle*d=_open_dynregion(& _tmp93B,
_tmp939);{int seen_defn_before;struct _tuple15**_tmp93C=((struct _tuple15**(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp93A,_tmp937);if(
_tmp93C == 0){seen_defn_before=0;{struct _tuple15*v;if(ad->kind == Cyc_Absyn_StructA){
struct _tuple15*_tmpE16;v=((_tmpE16=_region_malloc(d,sizeof(*_tmpE16)),((_tmpE16->f1=
ad,((_tmpE16->f2=Cyc_Absyn_strctq(_tmp937),_tmpE16))))));}else{struct _tuple15*
_tmpE17;v=((_tmpE17=_region_malloc(d,sizeof(*_tmpE17)),((_tmpE17->f1=ad,((
_tmpE17->f2=Cyc_Absyn_unionq_typ(_tmp937),_tmpE17))))));}*_tmp93A=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp93A,
_tmp937,v);};}else{struct _tuple15 _tmp940;struct Cyc_Absyn_Aggrdecl*_tmp941;void*
_tmp942;struct _tuple15*_tmp93F=*_tmp93C;_tmp940=*_tmp93F;_tmp941=_tmp940.f1;
_tmp942=_tmp940.f2;if(_tmp941->impl == 0){{struct _tuple15*_tmpE18;*_tmp93A=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))
Cyc_Dict_insert)(*_tmp93A,_tmp937,((_tmpE18=_region_malloc(d,sizeof(*_tmpE18)),((
_tmpE18->f1=ad,((_tmpE18->f2=_tmp942,_tmpE18)))))));}seen_defn_before=0;}else{
seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*_tmpE19;struct Cyc_Absyn_Aggrdecl*
new_ad=(_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19->kind=ad->kind,((_tmpE19->sc=
Cyc_Absyn_Public,((_tmpE19->name=ad->name,((_tmpE19->tvs=0,((_tmpE19->impl=0,((
_tmpE19->attributes=ad->attributes,_tmpE19)))))))))))));if(ad->impl != 0  && !
seen_defn_before){{struct Cyc_Absyn_AggrdeclImpl*_tmpE1A;new_ad->impl=((_tmpE1A=
_cycalloc(sizeof(*_tmpE1A)),((_tmpE1A->exist_vars=0,((_tmpE1A->rgn_po=0,((
_tmpE1A->fields=0,((_tmpE1A->tagged=0,_tmpE1A))))))))));}{struct Cyc_List_List*
new_fields=0;{struct Cyc_List_List*_tmp945=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp945 != 0;_tmp945=_tmp945->tl){struct Cyc_Absyn_Aggrfield*
_tmp946=(struct Cyc_Absyn_Aggrfield*)_tmp945->hd;if(_tmp945->tl == 0  && Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp946->type)))continue;{struct Cyc_Absyn_Aggrfield*
_tmpE1B;struct Cyc_Absyn_Aggrfield*_tmp947=(_tmpE1B=_cycalloc(sizeof(*_tmpE1B)),((
_tmpE1B->name=_tmp946->name,((_tmpE1B->tq=Cyc_Toc_mt_tq,((_tmpE1B->type=Cyc_Toc_typ_to_c_array(
_tmp946->type),((_tmpE1B->width=_tmp946->width,((_tmpE1B->attributes=_tmp946->attributes,
_tmpE1B)))))))))));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp948=_tmp947->type;struct _dyneither_ptr*_tmp949=_tmp947->name;const char*
_tmpE20;void*_tmpE1F[2];struct Cyc_String_pa_struct _tmpE1E;struct Cyc_String_pa_struct
_tmpE1D;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpE1D.tag=0,((_tmpE1D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp949),((_tmpE1E.tag=0,((_tmpE1E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpE1F[0]=&
_tmpE1E,((_tmpE1F[1]=& _tmpE1D,Cyc_aprintf(((_tmpE20="_union_%s_%s",
_tag_dyneither(_tmpE20,sizeof(char),13))),_tag_dyneither(_tmpE1F,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpE21;struct _dyneither_ptr*str=(_tmpE21=_cycalloc(sizeof(*
_tmpE21)),((_tmpE21[0]=s,_tmpE21)));struct Cyc_Absyn_Aggrfield*_tmpE22;struct Cyc_Absyn_Aggrfield*
_tmp94A=(_tmpE22=_cycalloc(sizeof(*_tmpE22)),((_tmpE22->name=Cyc_Toc_val_sp,((
_tmpE22->tq=Cyc_Toc_mt_tq,((_tmpE22->type=_tmp948,((_tmpE22->width=0,((_tmpE22->attributes=
0,_tmpE22)))))))))));struct Cyc_Absyn_Aggrfield*_tmpE23;struct Cyc_Absyn_Aggrfield*
_tmp94B=(_tmpE23=_cycalloc(sizeof(*_tmpE23)),((_tmpE23->name=Cyc_Toc_tag_sp,((
_tmpE23->tq=Cyc_Toc_mt_tq,((_tmpE23->type=Cyc_Absyn_sint_typ,((_tmpE23->width=0,((
_tmpE23->attributes=0,_tmpE23)))))))))));struct Cyc_Absyn_Aggrfield*_tmpE24[2];
struct Cyc_List_List*_tmp94C=(_tmpE24[1]=_tmp94A,((_tmpE24[0]=_tmp94B,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpE24,sizeof(struct Cyc_Absyn_Aggrfield*),
2)))));struct Cyc_Absyn_AggrdeclImpl*_tmpE29;struct _tuple0*_tmpE28;struct Cyc_Absyn_Aggrdecl*
_tmpE27;struct Cyc_Absyn_Aggrdecl*_tmp94D=(_tmpE27=_cycalloc(sizeof(*_tmpE27)),((
_tmpE27->kind=Cyc_Absyn_StructA,((_tmpE27->sc=Cyc_Absyn_Public,((_tmpE27->name=((
_tmpE28=_cycalloc(sizeof(*_tmpE28)),((_tmpE28->f1=Cyc_Absyn_Loc_n,((_tmpE28->f2=
str,_tmpE28)))))),((_tmpE27->tvs=0,((_tmpE27->impl=((_tmpE29=_cycalloc(sizeof(*
_tmpE29)),((_tmpE29->exist_vars=0,((_tmpE29->rgn_po=0,((_tmpE29->fields=_tmp94C,((
_tmpE29->tagged=0,_tmpE29)))))))))),((_tmpE27->attributes=0,_tmpE27)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmpE2F;struct Cyc_Absyn_Aggr_d_struct _tmpE2E;
struct Cyc_List_List*_tmpE2D;Cyc_Toc_result_decls=((_tmpE2D=_cycalloc(sizeof(*
_tmpE2D)),((_tmpE2D->hd=Cyc_Absyn_new_decl((void*)((_tmpE2F=_cycalloc(sizeof(*
_tmpE2F)),((_tmpE2F[0]=((_tmpE2E.tag=6,((_tmpE2E.f1=_tmp94D,_tmpE2E)))),_tmpE2F)))),
0),((_tmpE2D->tl=Cyc_Toc_result_decls,_tmpE2D))))));}_tmp947->type=Cyc_Absyn_strct(
str);}{struct Cyc_List_List*_tmpE30;new_fields=((_tmpE30=_cycalloc(sizeof(*
_tmpE30)),((_tmpE30->hd=_tmp947,((_tmpE30->tl=new_fields,_tmpE30))))));};};}}(
new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_fields);};}if(add_to_decls){struct Cyc_Absyn_Decl*_tmpE3A;struct Cyc_Absyn_Aggr_d_struct
_tmpE39;struct Cyc_Absyn_Aggr_d_struct*_tmpE38;struct Cyc_List_List*_tmpE37;Cyc_Toc_result_decls=((
_tmpE37=_cycalloc(sizeof(*_tmpE37)),((_tmpE37->hd=((_tmpE3A=_cycalloc(sizeof(*
_tmpE3A)),((_tmpE3A->r=(void*)((_tmpE38=_cycalloc(sizeof(*_tmpE38)),((_tmpE38[0]=((
_tmpE39.tag=6,((_tmpE39.f1=new_ad,_tmpE39)))),_tmpE38)))),((_tmpE3A->loc=0,
_tmpE3A)))))),((_tmpE37->tl=Cyc_Toc_result_decls,_tmpE37))))));}};};
_pop_dynregion(d);};}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud){struct _DynRegionHandle*_tmp964;struct Cyc_Set_Set**_tmp965;struct Cyc_Toc_TocState
_tmp963=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp964=
_tmp963.dyn;_tmp965=_tmp963.datatypes_so_far;{struct _DynRegionFrame _tmp966;
struct _RegionHandle*d=_open_dynregion(& _tmp966,_tmp964);{struct _tuple0*_tmp967=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))
Cyc_Set_member)(*_tmp965,_tmp967)){_npop_handler(0);return;}*_tmp965=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*
_tmp965,_tmp967);}{struct Cyc_List_List*_tmp968=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp968 != 0;_tmp968=_tmp968->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp968->hd;struct Cyc_List_List*_tmp969=0;int i=
1;{struct Cyc_List_List*_tmp96A=f->typs;for(0;_tmp96A != 0;(_tmp96A=_tmp96A->tl,i
++)){struct _dyneither_ptr*_tmp96B=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpE3B;struct Cyc_Absyn_Aggrfield*_tmp96C=(_tmpE3B=_cycalloc(sizeof(*_tmpE3B)),((
_tmpE3B->name=_tmp96B,((_tmpE3B->tq=(*((struct _tuple10*)_tmp96A->hd)).f1,((
_tmpE3B->type=Cyc_Toc_typ_to_c_array((*((struct _tuple10*)_tmp96A->hd)).f2),((
_tmpE3B->width=0,((_tmpE3B->attributes=0,_tmpE3B)))))))))));struct Cyc_List_List*
_tmpE3C;_tmp969=((_tmpE3C=_cycalloc(sizeof(*_tmpE3C)),((_tmpE3C->hd=_tmp96C,((
_tmpE3C->tl=_tmp969,_tmpE3C))))));}}{struct Cyc_Absyn_Aggrfield*_tmpE3F;struct Cyc_List_List*
_tmpE3E;_tmp969=((_tmpE3E=_cycalloc(sizeof(*_tmpE3E)),((_tmpE3E->hd=((_tmpE3F=
_cycalloc(sizeof(*_tmpE3F)),((_tmpE3F->name=Cyc_Toc_tag_sp,((_tmpE3F->tq=Cyc_Toc_mt_tq,((
_tmpE3F->type=Cyc_Absyn_sint_typ,((_tmpE3F->width=0,((_tmpE3F->attributes=0,
_tmpE3F)))))))))))),((_tmpE3E->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp969),_tmpE3E))))));}{struct Cyc_Absyn_AggrdeclImpl*
_tmpE44;const char*_tmpE43;struct Cyc_Absyn_Aggrdecl*_tmpE42;struct Cyc_Absyn_Aggrdecl*
_tmp971=(_tmpE42=_cycalloc(sizeof(*_tmpE42)),((_tmpE42->kind=Cyc_Absyn_StructA,((
_tmpE42->sc=Cyc_Absyn_Public,((_tmpE42->name=Cyc_Toc_collapse_qvar_tag(f->name,((
_tmpE43="_struct",_tag_dyneither(_tmpE43,sizeof(char),8)))),((_tmpE42->tvs=0,((
_tmpE42->impl=((_tmpE44=_cycalloc(sizeof(*_tmpE44)),((_tmpE44->exist_vars=0,((
_tmpE44->rgn_po=0,((_tmpE44->fields=_tmp969,((_tmpE44->tagged=0,_tmpE44)))))))))),((
_tmpE42->attributes=0,_tmpE42)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpE4A;
struct Cyc_Absyn_Aggr_d_struct _tmpE49;struct Cyc_List_List*_tmpE48;Cyc_Toc_result_decls=((
_tmpE48=_cycalloc(sizeof(*_tmpE48)),((_tmpE48->hd=Cyc_Absyn_new_decl((void*)((
_tmpE4A=_cycalloc(sizeof(*_tmpE4A)),((_tmpE4A[0]=((_tmpE49.tag=6,((_tmpE49.f1=
_tmp971,_tmpE49)))),_tmpE4A)))),0),((_tmpE48->tl=Cyc_Toc_result_decls,_tmpE48))))));};}};;
_pop_dynregion(d);};}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd){if(xd->fields == 0)return;{struct _DynRegionHandle*_tmp979;struct Cyc_Dict_Dict*
_tmp97A;struct Cyc_Toc_TocState _tmp978=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp979=_tmp978.dyn;_tmp97A=_tmp978.xdatatypes_so_far;{struct _DynRegionFrame
_tmp97B;struct _RegionHandle*d=_open_dynregion(& _tmp97B,_tmp979);{struct _tuple0*
_tmp97C=xd->name;struct Cyc_List_List*_tmp97D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp97D != 0;_tmp97D=_tmp97D->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp97D->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp97E=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp97F=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp97E,Cyc_Absyn_false_conref,0);int*_tmp980=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp97A,f->name);int
_tmp981;_LL3AD: if(_tmp980 != 0)goto _LL3AF;_LL3AE: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp982=Cyc_Absyn_new_vardecl(f->name,_tmp97F,
initopt);_tmp982->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*_tmpE50;struct Cyc_Absyn_Var_d_struct
_tmpE4F;struct Cyc_List_List*_tmpE4E;Cyc_Toc_result_decls=((_tmpE4E=_cycalloc(
sizeof(*_tmpE4E)),((_tmpE4E->hd=Cyc_Absyn_new_decl((void*)((_tmpE50=_cycalloc(
sizeof(*_tmpE50)),((_tmpE50[0]=((_tmpE4F.tag=0,((_tmpE4F.f1=_tmp982,_tmpE4F)))),
_tmpE50)))),0),((_tmpE4E->tl=Cyc_Toc_result_decls,_tmpE4E))))));}*_tmp97A=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp97A,f->name,f->sc != Cyc_Absyn_Extern);{struct Cyc_List_List*fields=0;int i=1;{
struct Cyc_List_List*_tmp986=f->typs;for(0;_tmp986 != 0;(_tmp986=_tmp986->tl,i ++)){
struct Cyc_Int_pa_struct _tmpE58;void*_tmpE57[1];const char*_tmpE56;struct
_dyneither_ptr*_tmpE55;struct _dyneither_ptr*_tmp987=(_tmpE55=_cycalloc(sizeof(*
_tmpE55)),((_tmpE55[0]=(struct _dyneither_ptr)((_tmpE58.tag=1,((_tmpE58.f1=(
unsigned long)i,((_tmpE57[0]=& _tmpE58,Cyc_aprintf(((_tmpE56="f%d",_tag_dyneither(
_tmpE56,sizeof(char),4))),_tag_dyneither(_tmpE57,sizeof(void*),1)))))))),_tmpE55)));
struct Cyc_Absyn_Aggrfield*_tmpE59;struct Cyc_Absyn_Aggrfield*_tmp988=(_tmpE59=
_cycalloc(sizeof(*_tmpE59)),((_tmpE59->name=_tmp987,((_tmpE59->tq=(*((struct
_tuple10*)_tmp986->hd)).f1,((_tmpE59->type=Cyc_Toc_typ_to_c_array((*((struct
_tuple10*)_tmp986->hd)).f2),((_tmpE59->width=0,((_tmpE59->attributes=0,_tmpE59)))))))))));
struct Cyc_List_List*_tmpE5A;fields=((_tmpE5A=_cycalloc(sizeof(*_tmpE5A)),((
_tmpE5A->hd=_tmp988,((_tmpE5A->tl=fields,_tmpE5A))))));}}{struct Cyc_Absyn_Aggrfield*
_tmpE5D;struct Cyc_List_List*_tmpE5C;fields=((_tmpE5C=_cycalloc(sizeof(*_tmpE5C)),((
_tmpE5C->hd=((_tmpE5D=_cycalloc(sizeof(*_tmpE5D)),((_tmpE5D->name=Cyc_Toc_tag_sp,((
_tmpE5D->tq=Cyc_Toc_mt_tq,((_tmpE5D->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq),((_tmpE5D->width=0,((_tmpE5D->attributes=0,_tmpE5D)))))))))))),((
_tmpE5C->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmpE5C))))));}{struct Cyc_Absyn_AggrdeclImpl*_tmpE62;const char*_tmpE61;
struct Cyc_Absyn_Aggrdecl*_tmpE60;struct Cyc_Absyn_Aggrdecl*_tmp991=(_tmpE60=
_cycalloc(sizeof(*_tmpE60)),((_tmpE60->kind=Cyc_Absyn_StructA,((_tmpE60->sc=Cyc_Absyn_Public,((
_tmpE60->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpE61="_struct",
_tag_dyneither(_tmpE61,sizeof(char),8)))),((_tmpE60->tvs=0,((_tmpE60->impl=((
_tmpE62=_cycalloc(sizeof(*_tmpE62)),((_tmpE62->exist_vars=0,((_tmpE62->rgn_po=0,((
_tmpE62->fields=fields,((_tmpE62->tagged=0,_tmpE62)))))))))),((_tmpE60->attributes=
0,_tmpE60)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpE68;struct Cyc_Absyn_Aggr_d_struct
_tmpE67;struct Cyc_List_List*_tmpE66;Cyc_Toc_result_decls=((_tmpE66=_cycalloc(
sizeof(*_tmpE66)),((_tmpE66->hd=Cyc_Absyn_new_decl((void*)((_tmpE68=_cycalloc(
sizeof(*_tmpE68)),((_tmpE68[0]=((_tmpE67.tag=6,((_tmpE67.f1=_tmp991,_tmpE67)))),
_tmpE68)))),0),((_tmpE66->tl=Cyc_Toc_result_decls,_tmpE66))))));}goto _LL3AC;};};};}
_LL3AF: if(_tmp980 == 0)goto _LL3B1;_tmp981=*_tmp980;if(_tmp981 != 0)goto _LL3B1;
_LL3B0: if(f->sc != Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*_tmp998=Cyc_Absyn_string_exp(*
fn,0);struct Cyc_Absyn_Vardecl*_tmp999=Cyc_Absyn_new_vardecl(f->name,_tmp97F,(
struct Cyc_Absyn_Exp*)_tmp998);_tmp999->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*
_tmpE6E;struct Cyc_Absyn_Var_d_struct _tmpE6D;struct Cyc_List_List*_tmpE6C;Cyc_Toc_result_decls=((
_tmpE6C=_cycalloc(sizeof(*_tmpE6C)),((_tmpE6C->hd=Cyc_Absyn_new_decl((void*)((
_tmpE6E=_cycalloc(sizeof(*_tmpE6E)),((_tmpE6E[0]=((_tmpE6D.tag=0,((_tmpE6D.f1=
_tmp999,_tmpE6D)))),_tmpE6E)))),0),((_tmpE6C->tl=Cyc_Toc_result_decls,_tmpE6C))))));}*
_tmp97A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp97A,f->name,1);}goto _LL3AC;_LL3B1:;_LL3B2: goto _LL3AC;_LL3AC:;}};
_pop_dynregion(d);};};}static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*
ed){ed->sc=Cyc_Absyn_Public;if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(
struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,
struct Cyc_Absyn_Stmt*s){void*old_typ=vd->type;vd->type=Cyc_Toc_typ_to_c_array(
old_typ);if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ))vd->sc=Cyc_Absyn_Public;Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp99D=init->r;struct Cyc_Absyn_Vardecl*_tmp99F;struct Cyc_Absyn_Exp*_tmp9A0;
struct Cyc_Absyn_Exp*_tmp9A1;int _tmp9A2;_LL3B4: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp99E=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp99D;if(_tmp99E->tag != 28)
goto _LL3B6;else{_tmp99F=_tmp99E->f1;_tmp9A0=_tmp99E->f2;_tmp9A1=_tmp99E->f3;
_tmp9A2=_tmp99E->f4;}}_LL3B5: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp99F,_tmp9A0,_tmp9A1,_tmp9A2,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL3B3;_LL3B6:;_LL3B7: if(vd->sc == Cyc_Absyn_Static){struct Cyc_Toc_Env
_tmp9A4;struct _RegionHandle*_tmp9A5;struct Cyc_Toc_Env*_tmp9A3=init_nv;_tmp9A4=*
_tmp9A3;_tmp9A5=_tmp9A4.rgn;{struct Cyc_Toc_Env*_tmp9A6=Cyc_Toc_set_toplevel(
_tmp9A5,init_nv);Cyc_Toc_exp_to_c(_tmp9A6,init);};}else{Cyc_Toc_exp_to_c(init_nv,
init);}goto _LL3B3;_LL3B3:;}else{void*_tmp9A7=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_ArrayInfo _tmp9A9;void*_tmp9AA;struct Cyc_Absyn_Exp*_tmp9AB;union Cyc_Absyn_Constraint*
_tmp9AC;_LL3B9:{struct Cyc_Absyn_ArrayType_struct*_tmp9A8=(struct Cyc_Absyn_ArrayType_struct*)
_tmp9A7;if(_tmp9A8->tag != 8)goto _LL3BB;else{_tmp9A9=_tmp9A8->f1;_tmp9AA=_tmp9A9.elt_type;
_tmp9AB=_tmp9A9.num_elts;_tmp9AC=_tmp9A9.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp9AC))goto _LL3BB;_LL3BA: if(_tmp9AB == 0){const char*
_tmpE71;void*_tmpE70;(_tmpE70=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE71="can't initialize zero-terminated array -- size unknown",
_tag_dyneither(_tmpE71,sizeof(char),55))),_tag_dyneither(_tmpE70,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp9AB;struct Cyc_Absyn_Exp*
_tmp9AF=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp9B0=Cyc_Absyn_signed_int_exp(
0,0);s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp9AF,
_tmp9B0,0),0),Cyc_Absyn_new_stmt(s->r,0));goto _LL3B8;};_LL3BB:;_LL3BC: goto _LL3B8;
_LL3B8:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple0*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp9B2;struct
_RegionHandle*_tmp9B3;struct Cyc_Toc_Env*_tmp9B1=nv;_tmp9B2=*_tmp9B1;_tmp9B3=
_tmp9B2.rgn;{struct Cyc_Absyn_Stmt*_tmp9B4=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp9B5=Cyc_Toc_share_env(_tmp9B3,nv);struct Cyc_Toc_Env*_tmp9B7;struct Cyc_List_List*
_tmp9B8;struct Cyc_Absyn_Stmt*_tmp9B9;struct _tuple23 _tmp9B6=Cyc_Toc_xlate_pat(
_tmp9B5,_tmp9B3,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp9B7=_tmp9B6.f1;_tmp9B8=_tmp9B6.f2;_tmp9B9=_tmp9B6.f3;Cyc_Toc_stmt_to_c(
_tmp9B7,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp9B9,s,0),0);for(0;_tmp9B8 != 0;_tmp9B8=_tmp9B8->tl){
struct _tuple24 _tmp9BB;struct _tuple0*_tmp9BC;void*_tmp9BD;struct _tuple24*_tmp9BA=(
struct _tuple24*)_tmp9B8->hd;_tmp9BB=*_tmp9BA;_tmp9BC=_tmp9BB.f1;_tmp9BD=_tmp9BB.f2;
s=Cyc_Absyn_declare_stmt(_tmp9BC,_tmp9BD,0,s,0);}}return s;};}static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e){void*_tmp9BE=e->r;struct Cyc_Absyn_Exp*_tmp9C0;struct Cyc_Absyn_Exp*
_tmp9C2;struct Cyc_Absyn_Exp*_tmp9C4;struct Cyc_Absyn_Exp*_tmp9C6;struct Cyc_Absyn_Exp*
_tmp9C8;struct Cyc_Absyn_Exp*_tmp9CA;struct Cyc_Absyn_Exp*_tmp9CC;struct Cyc_Absyn_Exp*
_tmp9CE;struct Cyc_List_List*_tmp9D0;struct Cyc_Absyn_Exp*_tmp9D2;struct Cyc_Absyn_Exp*
_tmp9D3;struct Cyc_Absyn_Exp*_tmp9D5;struct Cyc_Absyn_Exp*_tmp9D6;struct Cyc_Absyn_Exp*
_tmp9D8;struct Cyc_Absyn_Exp*_tmp9D9;struct Cyc_Absyn_Exp*_tmp9DB;struct Cyc_Absyn_Exp*
_tmp9DC;struct Cyc_Absyn_Exp*_tmp9DE;struct Cyc_Absyn_Exp*_tmp9DF;struct Cyc_Absyn_Exp*
_tmp9E1;struct Cyc_Absyn_Exp*_tmp9E2;struct Cyc_Absyn_Exp*_tmp9E4;struct Cyc_Absyn_Exp*
_tmp9E5;struct Cyc_Absyn_Exp*_tmp9E6;struct Cyc_Absyn_Exp*_tmp9E8;struct Cyc_List_List*
_tmp9E9;struct Cyc_Absyn_Exp*_tmp9EB;struct Cyc_List_List*_tmp9EC;void*_tmp9EE;
void**_tmp9EF;struct Cyc_Absyn_Exp*_tmp9F0;struct _tuple8*_tmp9F2;struct _tuple8
_tmp9F3;void*_tmp9F4;void**_tmp9F5;struct Cyc_List_List*_tmp9F6;struct Cyc_List_List*
_tmp9F8;struct Cyc_List_List*_tmp9FA;void*_tmp9FC;void**_tmp9FD;void*_tmp9FF;void**
_tmpA00;struct Cyc_Absyn_Stmt*_tmpA02;struct Cyc_Absyn_MallocInfo _tmpA04;struct Cyc_Absyn_MallocInfo*
_tmpA05;_LL3BE: {struct Cyc_Absyn_Deref_e_struct*_tmp9BF=(struct Cyc_Absyn_Deref_e_struct*)
_tmp9BE;if(_tmp9BF->tag != 21)goto _LL3C0;else{_tmp9C0=_tmp9BF->f1;}}_LL3BF:
_tmp9C2=_tmp9C0;goto _LL3C1;_LL3C0: {struct Cyc_Absyn_AggrMember_e_struct*_tmp9C1=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp9BE;if(_tmp9C1->tag != 22)goto _LL3C2;
else{_tmp9C2=_tmp9C1->f1;}}_LL3C1: _tmp9C4=_tmp9C2;goto _LL3C3;_LL3C2: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp9C3=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp9BE;if(_tmp9C3->tag != 23)goto
_LL3C4;else{_tmp9C4=_tmp9C3->f1;}}_LL3C3: _tmp9C6=_tmp9C4;goto _LL3C5;_LL3C4: {
struct Cyc_Absyn_Address_e_struct*_tmp9C5=(struct Cyc_Absyn_Address_e_struct*)
_tmp9BE;if(_tmp9C5->tag != 16)goto _LL3C6;else{_tmp9C6=_tmp9C5->f1;}}_LL3C5:
_tmp9C8=_tmp9C6;goto _LL3C7;_LL3C6: {struct Cyc_Absyn_Throw_e_struct*_tmp9C7=(
struct Cyc_Absyn_Throw_e_struct*)_tmp9BE;if(_tmp9C7->tag != 12)goto _LL3C8;else{
_tmp9C8=_tmp9C7->f1;}}_LL3C7: _tmp9CA=_tmp9C8;goto _LL3C9;_LL3C8: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp9C9=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp9BE;if(_tmp9C9->tag != 13)
goto _LL3CA;else{_tmp9CA=_tmp9C9->f1;}}_LL3C9: _tmp9CC=_tmp9CA;goto _LL3CB;_LL3CA: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp9CB=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp9BE;if(_tmp9CB->tag != 19)goto _LL3CC;else{_tmp9CC=_tmp9CB->f1;}}_LL3CB:
_tmp9CE=_tmp9CC;goto _LL3CD;_LL3CC: {struct Cyc_Absyn_Increment_e_struct*_tmp9CD=(
struct Cyc_Absyn_Increment_e_struct*)_tmp9BE;if(_tmp9CD->tag != 5)goto _LL3CE;else{
_tmp9CE=_tmp9CD->f1;}}_LL3CD: Cyc_Toc_exptypes_to_c(_tmp9CE);goto _LL3BD;_LL3CE: {
struct Cyc_Absyn_Primop_e_struct*_tmp9CF=(struct Cyc_Absyn_Primop_e_struct*)
_tmp9BE;if(_tmp9CF->tag != 3)goto _LL3D0;else{_tmp9D0=_tmp9CF->f2;}}_LL3CF:((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,
_tmp9D0);goto _LL3BD;_LL3D0: {struct Cyc_Absyn_And_e_struct*_tmp9D1=(struct Cyc_Absyn_And_e_struct*)
_tmp9BE;if(_tmp9D1->tag != 7)goto _LL3D2;else{_tmp9D2=_tmp9D1->f1;_tmp9D3=_tmp9D1->f2;}}
_LL3D1: _tmp9D5=_tmp9D2;_tmp9D6=_tmp9D3;goto _LL3D3;_LL3D2: {struct Cyc_Absyn_Or_e_struct*
_tmp9D4=(struct Cyc_Absyn_Or_e_struct*)_tmp9BE;if(_tmp9D4->tag != 8)goto _LL3D4;
else{_tmp9D5=_tmp9D4->f1;_tmp9D6=_tmp9D4->f2;}}_LL3D3: _tmp9D8=_tmp9D5;_tmp9D9=
_tmp9D6;goto _LL3D5;_LL3D4: {struct Cyc_Absyn_SeqExp_e_struct*_tmp9D7=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp9BE;if(_tmp9D7->tag != 9)goto _LL3D6;else{_tmp9D8=_tmp9D7->f1;_tmp9D9=_tmp9D7->f2;}}
_LL3D5: _tmp9DB=_tmp9D8;_tmp9DC=_tmp9D9;goto _LL3D7;_LL3D6: {struct Cyc_Absyn_Subscript_e_struct*
_tmp9DA=(struct Cyc_Absyn_Subscript_e_struct*)_tmp9BE;if(_tmp9DA->tag != 24)goto
_LL3D8;else{_tmp9DB=_tmp9DA->f1;_tmp9DC=_tmp9DA->f2;}}_LL3D7: _tmp9DE=_tmp9DB;
_tmp9DF=_tmp9DC;goto _LL3D9;_LL3D8: {struct Cyc_Absyn_Swap_e_struct*_tmp9DD=(
struct Cyc_Absyn_Swap_e_struct*)_tmp9BE;if(_tmp9DD->tag != 35)goto _LL3DA;else{
_tmp9DE=_tmp9DD->f1;_tmp9DF=_tmp9DD->f2;}}_LL3D9: _tmp9E1=_tmp9DE;_tmp9E2=_tmp9DF;
goto _LL3DB;_LL3DA: {struct Cyc_Absyn_AssignOp_e_struct*_tmp9E0=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp9BE;if(_tmp9E0->tag != 4)goto _LL3DC;else{_tmp9E1=_tmp9E0->f1;_tmp9E2=_tmp9E0->f3;}}
_LL3DB: Cyc_Toc_exptypes_to_c(_tmp9E1);Cyc_Toc_exptypes_to_c(_tmp9E2);goto _LL3BD;
_LL3DC: {struct Cyc_Absyn_Conditional_e_struct*_tmp9E3=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp9BE;if(_tmp9E3->tag != 6)goto _LL3DE;else{_tmp9E4=_tmp9E3->f1;_tmp9E5=_tmp9E3->f2;
_tmp9E6=_tmp9E3->f3;}}_LL3DD: Cyc_Toc_exptypes_to_c(_tmp9E4);Cyc_Toc_exptypes_to_c(
_tmp9E5);Cyc_Toc_exptypes_to_c(_tmp9E6);goto _LL3BD;_LL3DE: {struct Cyc_Absyn_FnCall_e_struct*
_tmp9E7=(struct Cyc_Absyn_FnCall_e_struct*)_tmp9BE;if(_tmp9E7->tag != 11)goto
_LL3E0;else{_tmp9E8=_tmp9E7->f1;_tmp9E9=_tmp9E7->f2;}}_LL3DF: _tmp9EB=_tmp9E8;
_tmp9EC=_tmp9E9;goto _LL3E1;_LL3E0: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp9EA=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp9BE;if(_tmp9EA->tag != 10)goto _LL3E2;
else{_tmp9EB=_tmp9EA->f1;_tmp9EC=_tmp9EA->f2;}}_LL3E1: Cyc_Toc_exptypes_to_c(
_tmp9EB);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp9EC);goto _LL3BD;_LL3E2: {struct Cyc_Absyn_Cast_e_struct*
_tmp9ED=(struct Cyc_Absyn_Cast_e_struct*)_tmp9BE;if(_tmp9ED->tag != 15)goto _LL3E4;
else{_tmp9EE=(void**)& _tmp9ED->f1;_tmp9EF=(void**)((void**)& _tmp9ED->f1);_tmp9F0=
_tmp9ED->f2;}}_LL3E3:*_tmp9EF=Cyc_Toc_typ_to_c(*_tmp9EF);Cyc_Toc_exptypes_to_c(
_tmp9F0);goto _LL3BD;_LL3E4: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp9F1=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp9BE;if(_tmp9F1->tag != 26)goto _LL3E6;
else{_tmp9F2=_tmp9F1->f1;_tmp9F3=*_tmp9F2;_tmp9F4=_tmp9F3.f3;_tmp9F5=(void**)&(*
_tmp9F1->f1).f3;_tmp9F6=_tmp9F1->f2;}}_LL3E5:*_tmp9F5=Cyc_Toc_typ_to_c(*_tmp9F5);
_tmp9F8=_tmp9F6;goto _LL3E7;_LL3E6: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp9F7=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp9BE;if(_tmp9F7->tag != 36)
goto _LL3E8;else{_tmp9F8=_tmp9F7->f2;}}_LL3E7: _tmp9FA=_tmp9F8;goto _LL3E9;_LL3E8: {
struct Cyc_Absyn_Array_e_struct*_tmp9F9=(struct Cyc_Absyn_Array_e_struct*)_tmp9BE;
if(_tmp9F9->tag != 27)goto _LL3EA;else{_tmp9FA=_tmp9F9->f1;}}_LL3E9: for(0;_tmp9FA
!= 0;_tmp9FA=_tmp9FA->tl){struct Cyc_Absyn_Exp*_tmpA16;struct _tuple16 _tmpA15=*((
struct _tuple16*)_tmp9FA->hd);_tmpA16=_tmpA15.f2;Cyc_Toc_exptypes_to_c(_tmpA16);}
goto _LL3BD;_LL3EA: {struct Cyc_Absyn_Offsetof_e_struct*_tmp9FB=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp9BE;if(_tmp9FB->tag != 20)goto _LL3EC;else{_tmp9FC=(void**)& _tmp9FB->f1;
_tmp9FD=(void**)((void**)& _tmp9FB->f1);}}_LL3EB: _tmpA00=_tmp9FD;goto _LL3ED;
_LL3EC: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp9FE=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp9BE;if(_tmp9FE->tag != 18)goto _LL3EE;else{_tmp9FF=(void**)& _tmp9FE->f1;
_tmpA00=(void**)((void**)& _tmp9FE->f1);}}_LL3ED:*_tmpA00=Cyc_Toc_typ_to_c(*
_tmpA00);goto _LL3BD;_LL3EE: {struct Cyc_Absyn_StmtExp_e_struct*_tmpA01=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp9BE;if(_tmpA01->tag != 37)goto _LL3F0;else{_tmpA02=_tmpA01->f1;}}_LL3EF: Cyc_Toc_stmttypes_to_c(
_tmpA02);goto _LL3BD;_LL3F0: {struct Cyc_Absyn_Malloc_e_struct*_tmpA03=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp9BE;if(_tmpA03->tag != 34)goto _LL3F2;else{_tmpA04=_tmpA03->f1;_tmpA05=(struct
Cyc_Absyn_MallocInfo*)& _tmpA03->f1;}}_LL3F1: if(_tmpA05->elt_type != 0){void**
_tmpE72;_tmpA05->elt_type=((_tmpE72=_cycalloc(sizeof(*_tmpE72)),((_tmpE72[0]=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmpA05->elt_type))),_tmpE72))));}Cyc_Toc_exptypes_to_c(
_tmpA05->num_elts);goto _LL3BD;_LL3F2: {struct Cyc_Absyn_Const_e_struct*_tmpA06=(
struct Cyc_Absyn_Const_e_struct*)_tmp9BE;if(_tmpA06->tag != 0)goto _LL3F4;}_LL3F3:
goto _LL3F5;_LL3F4: {struct Cyc_Absyn_Var_e_struct*_tmpA07=(struct Cyc_Absyn_Var_e_struct*)
_tmp9BE;if(_tmpA07->tag != 1)goto _LL3F6;}_LL3F5: goto _LL3F7;_LL3F6: {struct Cyc_Absyn_UnknownId_e_struct*
_tmpA08=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp9BE;if(_tmpA08->tag != 2)goto
_LL3F8;}_LL3F7: goto _LL3F9;_LL3F8: {struct Cyc_Absyn_Enum_e_struct*_tmpA09=(struct
Cyc_Absyn_Enum_e_struct*)_tmp9BE;if(_tmpA09->tag != 32)goto _LL3FA;}_LL3F9: goto
_LL3FB;_LL3FA: {struct Cyc_Absyn_Asm_e_struct*_tmpA0A=(struct Cyc_Absyn_Asm_e_struct*)
_tmp9BE;if(_tmpA0A->tag != 40)goto _LL3FC;}_LL3FB: goto _LL3FD;_LL3FC: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmpA0B=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp9BE;if(_tmpA0B->tag != 33)goto
_LL3FE;}_LL3FD: goto _LL3BD;_LL3FE: {struct Cyc_Absyn_AnonStruct_e_struct*_tmpA0C=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp9BE;if(_tmpA0C->tag != 30)goto _LL400;}
_LL3FF: goto _LL401;_LL400: {struct Cyc_Absyn_Datatype_e_struct*_tmpA0D=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp9BE;if(_tmpA0D->tag != 31)goto _LL402;}_LL401: goto _LL403;_LL402: {struct Cyc_Absyn_Aggregate_e_struct*
_tmpA0E=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp9BE;if(_tmpA0E->tag != 29)goto
_LL404;}_LL403: goto _LL405;_LL404: {struct Cyc_Absyn_Comprehension_e_struct*
_tmpA0F=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp9BE;if(_tmpA0F->tag != 28)
goto _LL406;}_LL405: goto _LL407;_LL406: {struct Cyc_Absyn_Tuple_e_struct*_tmpA10=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp9BE;if(_tmpA10->tag != 25)goto _LL408;}_LL407:
goto _LL409;_LL408: {struct Cyc_Absyn_Instantiate_e_struct*_tmpA11=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp9BE;if(_tmpA11->tag != 14)goto _LL40A;}_LL409: goto _LL40B;_LL40A: {struct Cyc_Absyn_New_e_struct*
_tmpA12=(struct Cyc_Absyn_New_e_struct*)_tmp9BE;if(_tmpA12->tag != 17)goto _LL40C;}
_LL40B: goto _LL40D;_LL40C: {struct Cyc_Absyn_Valueof_e_struct*_tmpA13=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp9BE;if(_tmpA13->tag != 39)goto _LL40E;}_LL40D: goto _LL40F;_LL40E: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmpA14=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp9BE;if(_tmpA14->tag != 38)goto
_LL3BD;}_LL40F:{const char*_tmpE75;void*_tmpE74;(_tmpE74=0,Cyc_Tcutil_terr(e->loc,((
_tmpE75="Cyclone expression within C code",_tag_dyneither(_tmpE75,sizeof(char),
33))),_tag_dyneither(_tmpE74,sizeof(void*),0)));}goto _LL3BD;_LL3BD:;}static void
Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){void*_tmpA1A=d->r;struct Cyc_Absyn_Vardecl*
_tmpA1C;struct Cyc_Absyn_Fndecl*_tmpA1E;struct Cyc_Absyn_Aggrdecl*_tmpA20;struct
Cyc_Absyn_Enumdecl*_tmpA22;struct Cyc_Absyn_Typedefdecl*_tmpA24;_LL411: {struct
Cyc_Absyn_Var_d_struct*_tmpA1B=(struct Cyc_Absyn_Var_d_struct*)_tmpA1A;if(_tmpA1B->tag
!= 0)goto _LL413;else{_tmpA1C=_tmpA1B->f1;}}_LL412: _tmpA1C->type=Cyc_Toc_typ_to_c(
_tmpA1C->type);if(_tmpA1C->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmpA1C->initializer));goto _LL410;_LL413: {struct Cyc_Absyn_Fn_d_struct*
_tmpA1D=(struct Cyc_Absyn_Fn_d_struct*)_tmpA1A;if(_tmpA1D->tag != 1)goto _LL415;
else{_tmpA1E=_tmpA1D->f1;}}_LL414: _tmpA1E->ret_type=Cyc_Toc_typ_to_c(_tmpA1E->ret_type);{
struct Cyc_List_List*_tmpA30=_tmpA1E->args;for(0;_tmpA30 != 0;_tmpA30=_tmpA30->tl){(*((
struct _tuple27*)_tmpA30->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple27*)_tmpA30->hd)).f3);}}
goto _LL410;_LL415: {struct Cyc_Absyn_Aggr_d_struct*_tmpA1F=(struct Cyc_Absyn_Aggr_d_struct*)
_tmpA1A;if(_tmpA1F->tag != 6)goto _LL417;else{_tmpA20=_tmpA1F->f1;}}_LL416: Cyc_Toc_aggrdecl_to_c(
_tmpA20,1);goto _LL410;_LL417: {struct Cyc_Absyn_Enum_d_struct*_tmpA21=(struct Cyc_Absyn_Enum_d_struct*)
_tmpA1A;if(_tmpA21->tag != 8)goto _LL419;else{_tmpA22=_tmpA21->f1;}}_LL418: if(
_tmpA22->fields != 0){struct Cyc_List_List*_tmpA31=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmpA22->fields))->v;for(0;_tmpA31 != 0;_tmpA31=_tmpA31->tl){
struct Cyc_Absyn_Enumfield*_tmpA32=(struct Cyc_Absyn_Enumfield*)_tmpA31->hd;if(
_tmpA32->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmpA32->tag));}}
goto _LL410;_LL419: {struct Cyc_Absyn_Typedef_d_struct*_tmpA23=(struct Cyc_Absyn_Typedef_d_struct*)
_tmpA1A;if(_tmpA23->tag != 9)goto _LL41B;else{_tmpA24=_tmpA23->f1;}}_LL41A:{struct
Cyc_Core_Opt*_tmpE76;_tmpA24->defn=((_tmpE76=_cycalloc(sizeof(*_tmpE76)),((
_tmpE76->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA24->defn))->v),_tmpE76))));}goto _LL410;_LL41B: {struct Cyc_Absyn_Let_d_struct*
_tmpA25=(struct Cyc_Absyn_Let_d_struct*)_tmpA1A;if(_tmpA25->tag != 2)goto _LL41D;}
_LL41C: goto _LL41E;_LL41D: {struct Cyc_Absyn_Letv_d_struct*_tmpA26=(struct Cyc_Absyn_Letv_d_struct*)
_tmpA1A;if(_tmpA26->tag != 3)goto _LL41F;}_LL41E: goto _LL420;_LL41F: {struct Cyc_Absyn_Datatype_d_struct*
_tmpA27=(struct Cyc_Absyn_Datatype_d_struct*)_tmpA1A;if(_tmpA27->tag != 7)goto
_LL421;}_LL420: goto _LL422;_LL421: {struct Cyc_Absyn_Namespace_d_struct*_tmpA28=(
struct Cyc_Absyn_Namespace_d_struct*)_tmpA1A;if(_tmpA28->tag != 10)goto _LL423;}
_LL422: goto _LL424;_LL423: {struct Cyc_Absyn_Using_d_struct*_tmpA29=(struct Cyc_Absyn_Using_d_struct*)
_tmpA1A;if(_tmpA29->tag != 11)goto _LL425;}_LL424: goto _LL426;_LL425: {struct Cyc_Absyn_ExternC_d_struct*
_tmpA2A=(struct Cyc_Absyn_ExternC_d_struct*)_tmpA1A;if(_tmpA2A->tag != 12)goto
_LL427;}_LL426: goto _LL428;_LL427: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmpA2B=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmpA1A;if(_tmpA2B->tag != 13)
goto _LL429;}_LL428: goto _LL42A;_LL429: {struct Cyc_Absyn_Region_d_struct*_tmpA2C=(
struct Cyc_Absyn_Region_d_struct*)_tmpA1A;if(_tmpA2C->tag != 4)goto _LL42B;}_LL42A:
goto _LL42C;_LL42B: {struct Cyc_Absyn_Alias_d_struct*_tmpA2D=(struct Cyc_Absyn_Alias_d_struct*)
_tmpA1A;if(_tmpA2D->tag != 5)goto _LL42D;}_LL42C:{const char*_tmpE79;void*_tmpE78;(
_tmpE78=0,Cyc_Tcutil_terr(d->loc,((_tmpE79="Cyclone declaration within C code",
_tag_dyneither(_tmpE79,sizeof(char),34))),_tag_dyneither(_tmpE78,sizeof(void*),0)));}
goto _LL410;_LL42D: {struct Cyc_Absyn_Porton_d_struct*_tmpA2E=(struct Cyc_Absyn_Porton_d_struct*)
_tmpA1A;if(_tmpA2E->tag != 14)goto _LL42F;}_LL42E: goto _LL430;_LL42F: {struct Cyc_Absyn_Portoff_d_struct*
_tmpA2F=(struct Cyc_Absyn_Portoff_d_struct*)_tmpA1A;if(_tmpA2F->tag != 15)goto
_LL410;}_LL430: goto _LL410;_LL410:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s){void*_tmpA36=s->r;struct Cyc_Absyn_Exp*_tmpA38;struct Cyc_Absyn_Stmt*_tmpA3A;
struct Cyc_Absyn_Stmt*_tmpA3B;struct Cyc_Absyn_Exp*_tmpA3D;struct Cyc_Absyn_Exp*
_tmpA3F;struct Cyc_Absyn_Stmt*_tmpA40;struct Cyc_Absyn_Stmt*_tmpA41;struct _tuple9
_tmpA43;struct Cyc_Absyn_Exp*_tmpA44;struct Cyc_Absyn_Stmt*_tmpA45;struct Cyc_Absyn_Exp*
_tmpA47;struct _tuple9 _tmpA48;struct Cyc_Absyn_Exp*_tmpA49;struct _tuple9 _tmpA4A;
struct Cyc_Absyn_Exp*_tmpA4B;struct Cyc_Absyn_Stmt*_tmpA4C;struct Cyc_Absyn_Exp*
_tmpA4E;struct Cyc_List_List*_tmpA4F;struct Cyc_Absyn_Decl*_tmpA51;struct Cyc_Absyn_Stmt*
_tmpA52;struct Cyc_Absyn_Stmt*_tmpA54;struct _tuple9 _tmpA55;struct Cyc_Absyn_Exp*
_tmpA56;_LL432: {struct Cyc_Absyn_Exp_s_struct*_tmpA37=(struct Cyc_Absyn_Exp_s_struct*)
_tmpA36;if(_tmpA37->tag != 1)goto _LL434;else{_tmpA38=_tmpA37->f1;}}_LL433: Cyc_Toc_exptypes_to_c(
_tmpA38);goto _LL431;_LL434: {struct Cyc_Absyn_Seq_s_struct*_tmpA39=(struct Cyc_Absyn_Seq_s_struct*)
_tmpA36;if(_tmpA39->tag != 2)goto _LL436;else{_tmpA3A=_tmpA39->f1;_tmpA3B=_tmpA39->f2;}}
_LL435: Cyc_Toc_stmttypes_to_c(_tmpA3A);Cyc_Toc_stmttypes_to_c(_tmpA3B);goto
_LL431;_LL436: {struct Cyc_Absyn_Return_s_struct*_tmpA3C=(struct Cyc_Absyn_Return_s_struct*)
_tmpA36;if(_tmpA3C->tag != 3)goto _LL438;else{_tmpA3D=_tmpA3C->f1;}}_LL437: if(
_tmpA3D != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmpA3D);goto _LL431;
_LL438: {struct Cyc_Absyn_IfThenElse_s_struct*_tmpA3E=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmpA36;if(_tmpA3E->tag != 4)goto _LL43A;else{_tmpA3F=_tmpA3E->f1;_tmpA40=_tmpA3E->f2;
_tmpA41=_tmpA3E->f3;}}_LL439: Cyc_Toc_exptypes_to_c(_tmpA3F);Cyc_Toc_stmttypes_to_c(
_tmpA40);Cyc_Toc_stmttypes_to_c(_tmpA41);goto _LL431;_LL43A: {struct Cyc_Absyn_While_s_struct*
_tmpA42=(struct Cyc_Absyn_While_s_struct*)_tmpA36;if(_tmpA42->tag != 5)goto _LL43C;
else{_tmpA43=_tmpA42->f1;_tmpA44=_tmpA43.f1;_tmpA45=_tmpA42->f2;}}_LL43B: Cyc_Toc_exptypes_to_c(
_tmpA44);Cyc_Toc_stmttypes_to_c(_tmpA45);goto _LL431;_LL43C: {struct Cyc_Absyn_For_s_struct*
_tmpA46=(struct Cyc_Absyn_For_s_struct*)_tmpA36;if(_tmpA46->tag != 9)goto _LL43E;
else{_tmpA47=_tmpA46->f1;_tmpA48=_tmpA46->f2;_tmpA49=_tmpA48.f1;_tmpA4A=_tmpA46->f3;
_tmpA4B=_tmpA4A.f1;_tmpA4C=_tmpA46->f4;}}_LL43D: Cyc_Toc_exptypes_to_c(_tmpA47);
Cyc_Toc_exptypes_to_c(_tmpA49);Cyc_Toc_exptypes_to_c(_tmpA4B);Cyc_Toc_stmttypes_to_c(
_tmpA4C);goto _LL431;_LL43E: {struct Cyc_Absyn_Switch_s_struct*_tmpA4D=(struct Cyc_Absyn_Switch_s_struct*)
_tmpA36;if(_tmpA4D->tag != 10)goto _LL440;else{_tmpA4E=_tmpA4D->f1;_tmpA4F=_tmpA4D->f2;}}
_LL43F: Cyc_Toc_exptypes_to_c(_tmpA4E);for(0;_tmpA4F != 0;_tmpA4F=_tmpA4F->tl){Cyc_Toc_stmttypes_to_c(((
struct Cyc_Absyn_Switch_clause*)_tmpA4F->hd)->body);}goto _LL431;_LL440: {struct
Cyc_Absyn_Decl_s_struct*_tmpA50=(struct Cyc_Absyn_Decl_s_struct*)_tmpA36;if(
_tmpA50->tag != 12)goto _LL442;else{_tmpA51=_tmpA50->f1;_tmpA52=_tmpA50->f2;}}
_LL441: Cyc_Toc_decltypes_to_c(_tmpA51);Cyc_Toc_stmttypes_to_c(_tmpA52);goto
_LL431;_LL442: {struct Cyc_Absyn_Do_s_struct*_tmpA53=(struct Cyc_Absyn_Do_s_struct*)
_tmpA36;if(_tmpA53->tag != 14)goto _LL444;else{_tmpA54=_tmpA53->f1;_tmpA55=_tmpA53->f2;
_tmpA56=_tmpA55.f1;}}_LL443: Cyc_Toc_stmttypes_to_c(_tmpA54);Cyc_Toc_exptypes_to_c(
_tmpA56);goto _LL431;_LL444: {struct Cyc_Absyn_Skip_s_struct*_tmpA57=(struct Cyc_Absyn_Skip_s_struct*)
_tmpA36;if(_tmpA57->tag != 0)goto _LL446;}_LL445: goto _LL447;_LL446: {struct Cyc_Absyn_Break_s_struct*
_tmpA58=(struct Cyc_Absyn_Break_s_struct*)_tmpA36;if(_tmpA58->tag != 6)goto _LL448;}
_LL447: goto _LL449;_LL448: {struct Cyc_Absyn_Continue_s_struct*_tmpA59=(struct Cyc_Absyn_Continue_s_struct*)
_tmpA36;if(_tmpA59->tag != 7)goto _LL44A;}_LL449: goto _LL44B;_LL44A: {struct Cyc_Absyn_Goto_s_struct*
_tmpA5A=(struct Cyc_Absyn_Goto_s_struct*)_tmpA36;if(_tmpA5A->tag != 8)goto _LL44C;}
_LL44B: goto _LL431;_LL44C: {struct Cyc_Absyn_Fallthru_s_struct*_tmpA5B=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmpA36;if(_tmpA5B->tag != 11)goto _LL44E;}_LL44D: goto _LL44F;_LL44E: {struct Cyc_Absyn_Label_s_struct*
_tmpA5C=(struct Cyc_Absyn_Label_s_struct*)_tmpA36;if(_tmpA5C->tag != 13)goto _LL450;}
_LL44F: goto _LL451;_LL450: {struct Cyc_Absyn_TryCatch_s_struct*_tmpA5D=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmpA36;if(_tmpA5D->tag != 15)goto _LL452;}_LL451: goto _LL453;_LL452: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmpA5E=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmpA36;if(_tmpA5E->tag != 16)goto
_LL431;}_LL453:{const char*_tmpE7C;void*_tmpE7B;(_tmpE7B=0,Cyc_Tcutil_terr(s->loc,((
_tmpE7C="Cyclone statement in C code",_tag_dyneither(_tmpE7C,sizeof(char),28))),
_tag_dyneither(_tmpE7B,sizeof(void*),0)));}goto _LL431;_LL431:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude){for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpE7F;void*_tmpE7E;(_tmpE7E=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE7F="decls_to_c: not at toplevel!",
_tag_dyneither(_tmpE7F,sizeof(char),29))),_tag_dyneither(_tmpE7E,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmpA63=d->r;struct Cyc_Absyn_Vardecl*
_tmpA65;struct Cyc_Absyn_Fndecl*_tmpA67;struct Cyc_Absyn_Aggrdecl*_tmpA6D;struct
Cyc_Absyn_Datatypedecl*_tmpA6F;struct Cyc_Absyn_Enumdecl*_tmpA71;struct Cyc_Absyn_Typedefdecl*
_tmpA73;struct Cyc_List_List*_tmpA77;struct Cyc_List_List*_tmpA79;struct Cyc_List_List*
_tmpA7B;struct Cyc_List_List*_tmpA7D;_LL455: {struct Cyc_Absyn_Var_d_struct*
_tmpA64=(struct Cyc_Absyn_Var_d_struct*)_tmpA63;if(_tmpA64->tag != 0)goto _LL457;
else{_tmpA65=_tmpA64->f1;}}_LL456: {struct _tuple0*_tmpA7E=_tmpA65->name;if(
_tmpA65->sc == Cyc_Absyn_ExternC){struct _tuple0*_tmpE80;_tmpA7E=((_tmpE80=
_cycalloc(sizeof(*_tmpE80)),((_tmpE80->f1=Cyc_Absyn_Rel_n(0),((_tmpE80->f2=(*
_tmpA7E).f2,_tmpE80))))));}if(_tmpA65->initializer != 0){if(cinclude)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmpA65->initializer));else{Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_check_null(_tmpA65->initializer));}}{struct Cyc_Absyn_Global_b_struct
_tmpE83;struct Cyc_Absyn_Global_b_struct*_tmpE82;nv=Cyc_Toc_add_varmap(r,nv,
_tmpA65->name,Cyc_Absyn_varb_exp(_tmpA7E,(void*)((_tmpE82=_cycalloc(sizeof(*
_tmpE82)),((_tmpE82[0]=((_tmpE83.tag=1,((_tmpE83.f1=_tmpA65,_tmpE83)))),_tmpE82)))),
0));}_tmpA65->name=_tmpA7E;_tmpA65->sc=Cyc_Toc_scope_to_c(_tmpA65->sc);_tmpA65->type=
Cyc_Toc_typ_to_c_array(_tmpA65->type);{struct Cyc_List_List*_tmpE84;Cyc_Toc_result_decls=((
_tmpE84=_cycalloc(sizeof(*_tmpE84)),((_tmpE84->hd=d,((_tmpE84->tl=Cyc_Toc_result_decls,
_tmpE84))))));}goto _LL454;}_LL457: {struct Cyc_Absyn_Fn_d_struct*_tmpA66=(struct
Cyc_Absyn_Fn_d_struct*)_tmpA63;if(_tmpA66->tag != 1)goto _LL459;else{_tmpA67=
_tmpA66->f1;}}_LL458: {struct _tuple0*_tmpA83=_tmpA67->name;if(_tmpA67->sc == Cyc_Absyn_ExternC){{
struct _tuple0*_tmpE85;_tmpA83=((_tmpE85=_cycalloc(sizeof(*_tmpE85)),((_tmpE85->f1=
Cyc_Absyn_Rel_n(0),((_tmpE85->f2=(*_tmpA83).f2,_tmpE85))))));}_tmpA67->sc=Cyc_Absyn_Public;}
nv=Cyc_Toc_add_varmap(r,nv,_tmpA67->name,Cyc_Absyn_var_exp(_tmpA83,0));_tmpA67->name=
_tmpA83;Cyc_Toc_fndecl_to_c(nv,_tmpA67,cinclude);{struct Cyc_List_List*_tmpE86;
Cyc_Toc_result_decls=((_tmpE86=_cycalloc(sizeof(*_tmpE86)),((_tmpE86->hd=d,((
_tmpE86->tl=Cyc_Toc_result_decls,_tmpE86))))));}goto _LL454;}_LL459: {struct Cyc_Absyn_Let_d_struct*
_tmpA68=(struct Cyc_Absyn_Let_d_struct*)_tmpA63;if(_tmpA68->tag != 2)goto _LL45B;}
_LL45A: goto _LL45C;_LL45B: {struct Cyc_Absyn_Letv_d_struct*_tmpA69=(struct Cyc_Absyn_Letv_d_struct*)
_tmpA63;if(_tmpA69->tag != 3)goto _LL45D;}_LL45C: {const char*_tmpE89;void*_tmpE88;(
_tmpE88=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE89="letdecl at toplevel",_tag_dyneither(_tmpE89,sizeof(char),20))),
_tag_dyneither(_tmpE88,sizeof(void*),0)));}_LL45D: {struct Cyc_Absyn_Region_d_struct*
_tmpA6A=(struct Cyc_Absyn_Region_d_struct*)_tmpA63;if(_tmpA6A->tag != 4)goto _LL45F;}
_LL45E: {const char*_tmpE8C;void*_tmpE8B;(_tmpE8B=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE8C="region decl at toplevel",
_tag_dyneither(_tmpE8C,sizeof(char),24))),_tag_dyneither(_tmpE8B,sizeof(void*),0)));}
_LL45F: {struct Cyc_Absyn_Alias_d_struct*_tmpA6B=(struct Cyc_Absyn_Alias_d_struct*)
_tmpA63;if(_tmpA6B->tag != 5)goto _LL461;}_LL460: {const char*_tmpE8F;void*_tmpE8E;(
_tmpE8E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE8F="alias decl at toplevel",_tag_dyneither(_tmpE8F,sizeof(char),23))),
_tag_dyneither(_tmpE8E,sizeof(void*),0)));}_LL461: {struct Cyc_Absyn_Aggr_d_struct*
_tmpA6C=(struct Cyc_Absyn_Aggr_d_struct*)_tmpA63;if(_tmpA6C->tag != 6)goto _LL463;
else{_tmpA6D=_tmpA6C->f1;}}_LL462: Cyc_Toc_aggrdecl_to_c(_tmpA6D,1);goto _LL454;
_LL463: {struct Cyc_Absyn_Datatype_d_struct*_tmpA6E=(struct Cyc_Absyn_Datatype_d_struct*)
_tmpA63;if(_tmpA6E->tag != 7)goto _LL465;else{_tmpA6F=_tmpA6E->f1;}}_LL464: if(
_tmpA6F->is_extensible)Cyc_Toc_xdatatypedecl_to_c(_tmpA6F);else{Cyc_Toc_datatypedecl_to_c(
_tmpA6F);}goto _LL454;_LL465: {struct Cyc_Absyn_Enum_d_struct*_tmpA70=(struct Cyc_Absyn_Enum_d_struct*)
_tmpA63;if(_tmpA70->tag != 8)goto _LL467;else{_tmpA71=_tmpA70->f1;}}_LL466: Cyc_Toc_enumdecl_to_c(
_tmpA71);{struct Cyc_List_List*_tmpE90;Cyc_Toc_result_decls=((_tmpE90=_cycalloc(
sizeof(*_tmpE90)),((_tmpE90->hd=d,((_tmpE90->tl=Cyc_Toc_result_decls,_tmpE90))))));}
goto _LL454;_LL467: {struct Cyc_Absyn_Typedef_d_struct*_tmpA72=(struct Cyc_Absyn_Typedef_d_struct*)
_tmpA63;if(_tmpA72->tag != 9)goto _LL469;else{_tmpA73=_tmpA72->f1;}}_LL468: _tmpA73->name=
_tmpA73->name;_tmpA73->tvs=0;if(_tmpA73->defn != 0){struct Cyc_Core_Opt*_tmpE91;
_tmpA73->defn=((_tmpE91=_cycalloc(sizeof(*_tmpE91)),((_tmpE91->v=Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA73->defn))->v),_tmpE91))));}else{
switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmpA73->kind))->v)->kind){
case Cyc_Absyn_BoxKind: _LL475:{struct Cyc_Core_Opt*_tmpE92;_tmpA73->defn=((_tmpE92=
_cycalloc(sizeof(*_tmpE92)),((_tmpE92->v=Cyc_Absyn_void_star_typ(),_tmpE92))));}
break;default: _LL476:{struct Cyc_Core_Opt*_tmpE93;_tmpA73->defn=((_tmpE93=
_cycalloc(sizeof(*_tmpE93)),((_tmpE93->v=(void*)& Cyc_Absyn_VoidType_val,_tmpE93))));}
break;}}{struct Cyc_List_List*_tmpE94;Cyc_Toc_result_decls=((_tmpE94=_cycalloc(
sizeof(*_tmpE94)),((_tmpE94->hd=d,((_tmpE94->tl=Cyc_Toc_result_decls,_tmpE94))))));}
goto _LL454;_LL469: {struct Cyc_Absyn_Porton_d_struct*_tmpA74=(struct Cyc_Absyn_Porton_d_struct*)
_tmpA63;if(_tmpA74->tag != 14)goto _LL46B;}_LL46A: goto _LL46C;_LL46B: {struct Cyc_Absyn_Portoff_d_struct*
_tmpA75=(struct Cyc_Absyn_Portoff_d_struct*)_tmpA63;if(_tmpA75->tag != 15)goto
_LL46D;}_LL46C: goto _LL454;_LL46D: {struct Cyc_Absyn_Namespace_d_struct*_tmpA76=(
struct Cyc_Absyn_Namespace_d_struct*)_tmpA63;if(_tmpA76->tag != 10)goto _LL46F;
else{_tmpA77=_tmpA76->f2;}}_LL46E: _tmpA79=_tmpA77;goto _LL470;_LL46F: {struct Cyc_Absyn_Using_d_struct*
_tmpA78=(struct Cyc_Absyn_Using_d_struct*)_tmpA63;if(_tmpA78->tag != 11)goto _LL471;
else{_tmpA79=_tmpA78->f2;}}_LL470: _tmpA7B=_tmpA79;goto _LL472;_LL471: {struct Cyc_Absyn_ExternC_d_struct*
_tmpA7A=(struct Cyc_Absyn_ExternC_d_struct*)_tmpA63;if(_tmpA7A->tag != 12)goto
_LL473;else{_tmpA7B=_tmpA7A->f1;}}_LL472: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA7B,top,
cinclude);goto _LL454;_LL473: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA7C=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmpA63;if(_tmpA7C->tag != 13)goto _LL454;
else{_tmpA7D=_tmpA7C->f1;}}_LL474: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA7D,top,1);goto
_LL454;_LL454:;};}return nv;}static void Cyc_Toc_init(){struct _DynRegionHandle*
_tmpA92;struct Cyc_Core_NewRegion _tmpA91=Cyc_Core__new_dynregion((const char*)"internal-error",
0);_tmpA92=_tmpA91.dynregion;{struct _DynRegionFrame _tmpA93;struct _RegionHandle*d=
_open_dynregion(& _tmpA93,_tmpA92);{struct Cyc_Dict_Dict*_tmpEA1;struct Cyc_Dict_Dict*
_tmpEA0;struct Cyc_Set_Set**_tmpE9F;struct Cyc_List_List**_tmpE9E;struct Cyc_Dict_Dict*
_tmpE9D;struct Cyc_List_List**_tmpE9C;struct Cyc_Toc_TocState*_tmpE9B;Cyc_Toc_toc_state=((
_tmpE9B=_cycalloc(sizeof(*_tmpE9B)),((_tmpE9B->dyn=(struct _DynRegionHandle*)
_tmpA92,((_tmpE9B->tuple_types=(struct Cyc_List_List**)((_tmpE9C=_region_malloc(d,
sizeof(*_tmpE9C)),((_tmpE9C[0]=0,_tmpE9C)))),((_tmpE9B->aggrs_so_far=(struct Cyc_Dict_Dict*)((
_tmpE9D=_region_malloc(d,sizeof(*_tmpE9D)),((_tmpE9D[0]=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
d,Cyc_Absyn_qvar_cmp),_tmpE9D)))),((_tmpE9B->abs_struct_types=(struct Cyc_List_List**)((
_tmpE9E=_region_malloc(d,sizeof(*_tmpE9E)),((_tmpE9E[0]=0,_tmpE9E)))),((_tmpE9B->datatypes_so_far=(
struct Cyc_Set_Set**)((_tmpE9F=_region_malloc(d,sizeof(*_tmpE9F)),((_tmpE9F[0]=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE9F)))),((_tmpE9B->xdatatypes_so_far=(
struct Cyc_Dict_Dict*)((_tmpEA0=_region_malloc(d,sizeof(*_tmpEA0)),((_tmpEA0[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpEA0)))),((_tmpE9B->qvar_tags=(
struct Cyc_Dict_Dict*)((_tmpEA1=_region_malloc(d,sizeof(*_tmpEA1)),((_tmpEA1[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct
_tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpEA1)))),((_tmpE9B->temp_labels=(
struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(
d),_tmpE9B))))))))))))))))));}Cyc_Toc_result_decls=0;Cyc_Toc_tuple_type_counter=
0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=0;Cyc_Toc_total_bounds_checks=
0;Cyc_Toc_bounds_checks_eliminated=0;{struct _dyneither_ptr**_tmpEA2;Cyc_Toc_globals=
_tag_dyneither(((_tmpEA2=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpEA2[
0]=& Cyc_Toc__throw_str,((_tmpEA2[1]=& Cyc_Toc_setjmp_str,((_tmpEA2[2]=& Cyc_Toc__push_handler_str,((
_tmpEA2[3]=& Cyc_Toc__pop_handler_str,((_tmpEA2[4]=& Cyc_Toc__exn_thrown_str,((
_tmpEA2[5]=& Cyc_Toc__npop_handler_str,((_tmpEA2[6]=& Cyc_Toc__check_null_str,((
_tmpEA2[7]=& Cyc_Toc__check_known_subscript_null_str,((_tmpEA2[8]=& Cyc_Toc__check_known_subscript_notnull_str,((
_tmpEA2[9]=& Cyc_Toc__check_dyneither_subscript_str,((_tmpEA2[10]=& Cyc_Toc__dyneither_ptr_str,((
_tmpEA2[11]=& Cyc_Toc__tag_dyneither_str,((_tmpEA2[12]=& Cyc_Toc__init_dyneither_ptr_str,((
_tmpEA2[13]=& Cyc_Toc__untag_dyneither_ptr_str,((_tmpEA2[14]=& Cyc_Toc__get_dyneither_size_str,((
_tmpEA2[15]=& Cyc_Toc__get_zero_arr_size_str,((_tmpEA2[16]=& Cyc_Toc__dyneither_ptr_plus_str,((
_tmpEA2[17]=& Cyc_Toc__zero_arr_plus_str,((_tmpEA2[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((
_tmpEA2[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpEA2[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((
_tmpEA2[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpEA2[22]=& Cyc_Toc__cycalloc_str,((
_tmpEA2[23]=& Cyc_Toc__cyccalloc_str,((_tmpEA2[24]=& Cyc_Toc__cycalloc_atomic_str,((
_tmpEA2[25]=& Cyc_Toc__cyccalloc_atomic_str,((_tmpEA2[26]=& Cyc_Toc__region_malloc_str,((
_tmpEA2[27]=& Cyc_Toc__region_calloc_str,((_tmpEA2[28]=& Cyc_Toc__check_times_str,((
_tmpEA2[29]=& Cyc_Toc__new_region_str,((_tmpEA2[30]=& Cyc_Toc__push_region_str,((
_tmpEA2[31]=& Cyc_Toc__pop_region_str,((_tmpEA2[32]=& Cyc_Toc__open_dynregion_str,((
_tmpEA2[33]=& Cyc_Toc__push_dynregion_str,((_tmpEA2[34]=& Cyc_Toc__pop_dynregion_str,((
_tmpEA2[35]=& Cyc_Toc__reset_region_str,((_tmpEA2[36]=& Cyc_Toc__throw_arraybounds_str,((
_tmpEA2[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpEA2[38]=& Cyc_Toc__throw_match_str,((
_tmpEA2[39]=& Cyc_Toc__swap_word_str,((_tmpEA2[40]=& Cyc_Toc__swap_dyneither_str,
_tmpEA2)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),41);};;_pop_dynregion(d);};}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();{struct _RegionHandle _tmpA9C=
_new_region("start");struct _RegionHandle*start=& _tmpA9C;_push_region(start);Cyc_Toc_decls_to_c(
start,Cyc_Toc_empty_env(start),ds,1,0);{struct _DynRegionHandle*_tmpA9E;struct Cyc_Toc_TocState
_tmpA9D=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA9E=
_tmpA9D.dyn;Cyc_Core_free_dynregion(_tmpA9E);};{struct Cyc_List_List*_tmpA9F=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);
_npop_handler(0);return _tmpA9F;};;_pop_region(start);};}
