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
Cyc_Absyn_var_t;typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;struct
_union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};typedef union Cyc_Absyn_Nmspace
Cyc_Absyn_nmspace_t;union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct
_dyneither_ptr*f2;};typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*
Cyc_Absyn_qvar_opt_t;typedef struct _tuple0*Cyc_Absyn_typedef_name_t;typedef struct
_tuple0*Cyc_Absyn_typedef_name_opt_t;typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;
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
topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_struct{int tag;};
struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{
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
struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;
int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_struct{int tag;};struct Cyc_Absyn_Var_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_struct{
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
struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_struct{
int tag;};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};extern struct Cyc_Absyn_Unresolved_b_struct Cyc_Absyn_Unresolved_b_val;struct
Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;
};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope
sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*
name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{
enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*
tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct
_tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const char*_tmpA87;void*_tmpA86;(_tmpA86=0,
Cyc_fprintf(Cyc_stderr,((_tmpA87="\n",_tag_dyneither(_tmpA87,sizeof(char),2))),
_tag_dyneither(_tmpA86,sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_struct _tmpA8A;struct Cyc_Toc_Toc_Unimplemented_struct*
_tmpA89;(int)_throw((void*)((_tmpA89=_cycalloc_atomic(sizeof(*_tmpA89)),((
_tmpA89[0]=((_tmpA8A.tag=Cyc_Toc_Toc_Unimplemented,_tmpA8A)),_tmpA89)))));};}
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(
Cyc_stderr,fmt,ap);{const char*_tmpA8D;void*_tmpA8C;(_tmpA8C=0,Cyc_fprintf(Cyc_stderr,((
_tmpA8D="\n",_tag_dyneither(_tmpA8D,sizeof(char),2))),_tag_dyneither(_tmpA8C,
sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Impossible_struct
_tmpA90;struct Cyc_Toc_Toc_Impossible_struct*_tmpA8F;(int)_throw((void*)((_tmpA8F=
_cycalloc_atomic(sizeof(*_tmpA8F)),((_tmpA8F[0]=((_tmpA90.tag=Cyc_Toc_Toc_Impossible,
_tmpA90)),_tmpA8F)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_struct{
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
_tmpA91;skip_stmt_opt=((_tmpA91=_cycalloc(sizeof(*_tmpA91)),((_tmpA91[0]=Cyc_Absyn_skip_stmt(
0),_tmpA91))));}return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(
void*t,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,
0);}static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Cast_e_struct
_tmpA94;struct Cyc_Absyn_Cast_e_struct*_tmpA93;return(void*)((_tmpA93=_cycalloc(
sizeof(*_tmpA93)),((_tmpA93[0]=((_tmpA94.tag=15,((_tmpA94.f1=(void*)t,((_tmpA94.f2=
e,((_tmpA94.f3=0,((_tmpA94.f4=Cyc_Absyn_No_coercion,_tmpA94)))))))))),_tmpA93))));}
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Deref_e_struct
_tmpA97;struct Cyc_Absyn_Deref_e_struct*_tmpA96;return(void*)((_tmpA96=_cycalloc(
sizeof(*_tmpA96)),((_tmpA96[0]=((_tmpA97.tag=21,((_tmpA97.f1=e,_tmpA97)))),
_tmpA96))));}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){struct Cyc_Absyn_Subscript_e_struct _tmpA9A;struct Cyc_Absyn_Subscript_e_struct*
_tmpA99;return(void*)((_tmpA99=_cycalloc(sizeof(*_tmpA99)),((_tmpA99[0]=((
_tmpA9A.tag=24,((_tmpA9A.f1=e1,((_tmpA9A.f2=e2,_tmpA9A)))))),_tmpA99))));}static
void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct
_tmpA9D;struct Cyc_Absyn_StmtExp_e_struct*_tmpA9C;return(void*)((_tmpA9C=
_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C[0]=((_tmpA9D.tag=37,((_tmpA9D.f1=s,_tmpA9D)))),
_tmpA9C))));}static void*Cyc_Toc_sizeoftyp_exp_r(void*t){struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpAA0;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpA9F;return(void*)((_tmpA9F=
_cycalloc(sizeof(*_tmpA9F)),((_tmpA9F[0]=((_tmpAA0.tag=18,((_tmpAA0.f1=(void*)t,
_tmpAA0)))),_tmpA9F))));}static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct _tmpAA3;struct Cyc_Absyn_FnCall_e_struct*
_tmpAA2;return(void*)((_tmpAA2=_cycalloc(sizeof(*_tmpAA2)),((_tmpAA2[0]=((
_tmpAA3.tag=11,((_tmpAA3.f1=e,((_tmpAA3.f2=es,((_tmpAA3.f3=0,_tmpAA3)))))))),
_tmpAA2))));}static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_struct
_tmpAA6;struct Cyc_Absyn_Exp_s_struct*_tmpAA5;return(void*)((_tmpAA5=_cycalloc(
sizeof(*_tmpAA5)),((_tmpAA5[0]=((_tmpAA6.tag=1,((_tmpAA6.f1=e,_tmpAA6)))),
_tmpAA5))));}static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2){struct Cyc_Absyn_Seq_s_struct _tmpAA9;struct Cyc_Absyn_Seq_s_struct*_tmpAA8;
return(void*)((_tmpAA8=_cycalloc(sizeof(*_tmpAA8)),((_tmpAA8[0]=((_tmpAA9.tag=2,((
_tmpAA9.f1=s1,((_tmpAA9.f2=s2,_tmpAA9)))))),_tmpAA8))));}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct
_tmpAAC;struct Cyc_Absyn_Conditional_e_struct*_tmpAAB;return(void*)((_tmpAAB=
_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB[0]=((_tmpAAC.tag=6,((_tmpAAC.f1=e1,((
_tmpAAC.f2=e2,((_tmpAAC.f3=e3,_tmpAAC)))))))),_tmpAAB))));}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrMember_e_struct
_tmpAAF;struct Cyc_Absyn_AggrMember_e_struct*_tmpAAE;return(void*)((_tmpAAE=
_cycalloc(sizeof(*_tmpAAE)),((_tmpAAE[0]=((_tmpAAF.tag=22,((_tmpAAF.f1=e,((
_tmpAAF.f2=n,((_tmpAAF.f3=0,((_tmpAAF.f4=0,_tmpAAF)))))))))),_tmpAAE))));}static
void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_struct _tmpAB2;struct Cyc_Absyn_AggrArrow_e_struct*
_tmpAB1;return(void*)((_tmpAB1=_cycalloc(sizeof(*_tmpAB1)),((_tmpAB1[0]=((
_tmpAB2.tag=23,((_tmpAB2.f1=e,((_tmpAB2.f2=n,((_tmpAB2.f3=0,((_tmpAB2.f4=0,
_tmpAB2)))))))))),_tmpAB1))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmpAB5;
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpAB4;return(void*)((_tmpAB4=_cycalloc(
sizeof(*_tmpAB4)),((_tmpAB4[0]=((_tmpAB5.tag=36,((_tmpAB5.f1=tdopt,((_tmpAB5.f2=
ds,_tmpAB5)))))),_tmpAB4))));}static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_struct _tmpAB8;
struct Cyc_Absyn_Goto_s_struct*_tmpAB7;return(void*)((_tmpAB7=_cycalloc(sizeof(*
_tmpAB7)),((_tmpAB7[0]=((_tmpAB8.tag=8,((_tmpAB8.f1=v,((_tmpAB8.f2=s,_tmpAB8)))))),
_tmpAB7))));}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{
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
Cyc_Core_Impossible_struct _tmpABE;const char*_tmpABD;struct Cyc_Core_Impossible_struct*
_tmpABC;(int)_throw((void*)((_tmpABC=_cycalloc(sizeof(*_tmpABC)),((_tmpABC[0]=((
_tmpABE.tag=Cyc_Core_Impossible,((_tmpABE.f1=((_tmpABD="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpABD,sizeof(char),44))),_tmpABE)))),_tmpABC)))));}}goto _LL0;
_LL3: {struct Cyc_Absyn_FloatType_struct*_tmpC9=(struct Cyc_Absyn_FloatType_struct*)
_tmpC6;if(_tmpC9->tag != 7)goto _LL5;else{_tmpCA=_tmpC9->f1;if(_tmpCA != 0)goto _LL5;}}
_LL4: function=fS->ffloat;goto _LL0;_LL5: {struct Cyc_Absyn_FloatType_struct*_tmpCB=(
struct Cyc_Absyn_FloatType_struct*)_tmpC6;if(_tmpCB->tag != 7)goto _LL7;else{_tmpCC=
_tmpCB->f1;if(_tmpCC != 1)goto _LL7;}}_LL6: function=fS->fdouble;goto _LL0;_LL7: {
struct Cyc_Absyn_FloatType_struct*_tmpCD=(struct Cyc_Absyn_FloatType_struct*)
_tmpC6;if(_tmpCD->tag != 7)goto _LL9;}_LL8: function=fS->flongdouble;goto _LL0;_LL9: {
struct Cyc_Absyn_PointerType_struct*_tmpCE=(struct Cyc_Absyn_PointerType_struct*)
_tmpC6;if(_tmpCE->tag != 5)goto _LLB;}_LLA: function=fS->fvoidstar;goto _LL0;_LLB:;
_LLC: {struct Cyc_Core_Impossible_struct _tmpACB;const char*_tmpACA;void*_tmpAC9[1];
struct Cyc_String_pa_struct _tmpAC8;struct Cyc_Core_Impossible_struct*_tmpAC7;(int)
_throw((void*)((_tmpAC7=_cycalloc(sizeof(*_tmpAC7)),((_tmpAC7[0]=((_tmpACB.tag=
Cyc_Core_Impossible,((_tmpACB.f1=(struct _dyneither_ptr)((_tmpAC8.tag=0,((_tmpAC8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAC9[
0]=& _tmpAC8,Cyc_aprintf(((_tmpACA="impossible expression type %s (not int, float, double, or pointer)",
_tag_dyneither(_tmpACA,sizeof(char),67))),_tag_dyneither(_tmpAC9,sizeof(void*),1)))))))),
_tmpACB)))),_tmpAC7)))));}_LL0:;}return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(
fS,(void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v);}struct Cyc_Absyn_Exp*
Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*
arr){void*_tmpD7=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
arr->topt))->v);struct Cyc_Absyn_PtrInfo _tmpD9;void*_tmpDA;_LL13: {struct Cyc_Absyn_PointerType_struct*
_tmpD8=(struct Cyc_Absyn_PointerType_struct*)_tmpD7;if(_tmpD8->tag != 5)goto _LL15;
else{_tmpD9=_tmpD8->f1;_tmpDA=_tmpD9.elt_typ;}}_LL14: return Cyc_Toc_getFunctionType(
fS,_tmpDA);_LL15:;_LL16: {struct Cyc_Core_Impossible_struct _tmpAD1;const char*
_tmpAD0;struct Cyc_Core_Impossible_struct*_tmpACF;(int)_throw((void*)((_tmpACF=
_cycalloc(sizeof(*_tmpACF)),((_tmpACF[0]=((_tmpAD1.tag=Cyc_Core_Impossible,((
_tmpAD1.f1=((_tmpAD0="impossible type (not pointer)",_tag_dyneither(_tmpAD0,
sizeof(char),30))),_tmpAD1)))),_tmpACF)))));}_LL12:;}struct _tuple16{struct Cyc_List_List*
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
Cyc_Absyn_conref_def)(0,_tmp106);_LL35:;_LL36: {const char*_tmpAD4;void*_tmpAD3;(
_tmpAD3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAD4="is_nullable",_tag_dyneither(_tmpAD4,sizeof(char),12))),_tag_dyneither(
_tmpAD3,sizeof(void*),0)));}_LL32:;}static char _tmp115[1]="";static char _tmp116[8]="*bogus*";
static struct _tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct
_dyneither_ptr tag){struct _DynRegionHandle*_tmp10A;struct Cyc_Dict_Dict*_tmp10B;
struct Cyc_Toc_TocState _tmp109=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp10A=_tmp109.dyn;_tmp10B=_tmp109.qvar_tags;{static struct _dyneither_ptr
bogus_string={_tmp116,_tmp116,_tmp116 + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
4,0}},& bogus_string};static struct _tuple14 pair={& bogus_qvar,{_tmp115,_tmp115,
_tmp115 + 1}};{struct _tuple14 _tmpAD5;pair=((_tmpAD5.f1=x,((_tmpAD5.f2=tag,_tmpAD5))));}{
struct _DynRegionFrame _tmp10D;struct _RegionHandle*d=_open_dynregion(& _tmp10D,
_tmp10A);{struct _tuple0**_tmp10E=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,
struct _tuple14*k))Cyc_Dict_lookup_opt)(*_tmp10B,(struct _tuple14*)& pair);if(
_tmp10E != 0){struct _tuple0*_tmp10F=*_tmp10E;_npop_handler(0);return _tmp10F;}{
struct _tuple14*_tmpAD6;struct _tuple14*_tmp110=(_tmpAD6=_cycalloc(sizeof(*_tmpAD6)),((
_tmpAD6->f1=x,((_tmpAD6->f2=tag,_tmpAD6)))));struct _dyneither_ptr*_tmpAD9;struct
_tuple0*_tmpAD8;struct _tuple0*res=(_tmpAD8=_cycalloc(sizeof(*_tmpAD8)),((_tmpAD8->f1=(*
x).f1,((_tmpAD8->f2=((_tmpAD9=_cycalloc(sizeof(*_tmpAD9)),((_tmpAD9[0]=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*x).f2,(struct
_dyneither_ptr)tag),_tmpAD9)))),_tmpAD8)))));*_tmp10B=((struct Cyc_Dict_Dict(*)(
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
_npop_handler(0);return _tmp121;}};}}{struct Cyc_Int_pa_struct _tmpAE1;void*_tmpAE0[
1];const char*_tmpADF;struct _dyneither_ptr*_tmpADE;struct _dyneither_ptr*xname=(
_tmpADE=_cycalloc(sizeof(*_tmpADE)),((_tmpADE[0]=(struct _dyneither_ptr)((_tmpAE1.tag=
1,((_tmpAE1.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAE0[0]=& _tmpAE1,
Cyc_aprintf(((_tmpADF="_tuple%d",_tag_dyneither(_tmpADF,sizeof(char),9))),
_tag_dyneither(_tmpAE0,sizeof(void*),1)))))))),_tmpADE)));void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp122=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmpAE4;struct Cyc_List_List*
_tmpAE3;_tmp122=((_tmpAE3=_cycalloc(sizeof(*_tmpAE3)),((_tmpAE3->hd=((_tmpAE4=
_cycalloc(sizeof(*_tmpAE4)),((_tmpAE4->name=Cyc_Absyn_fieldname(i),((_tmpAE4->tq=
Cyc_Toc_mt_tq,((_tmpAE4->type=(void*)ts2->hd,((_tmpAE4->width=0,((_tmpAE4->attributes=
0,_tmpAE4)))))))))))),((_tmpAE3->tl=_tmp122,_tmpAE3))))));}}_tmp122=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp122);{struct Cyc_Absyn_AggrdeclImpl*
_tmpAE9;struct _tuple0*_tmpAE8;struct Cyc_Absyn_Aggrdecl*_tmpAE7;struct Cyc_Absyn_Aggrdecl*
_tmp125=(_tmpAE7=_cycalloc(sizeof(*_tmpAE7)),((_tmpAE7->kind=Cyc_Absyn_StructA,((
_tmpAE7->sc=Cyc_Absyn_Public,((_tmpAE7->name=((_tmpAE8=_cycalloc(sizeof(*_tmpAE8)),((
_tmpAE8->f1=Cyc_Absyn_Rel_n(0),((_tmpAE8->f2=xname,_tmpAE8)))))),((_tmpAE7->tvs=
0,((_tmpAE7->impl=((_tmpAE9=_cycalloc(sizeof(*_tmpAE9)),((_tmpAE9->exist_vars=0,((
_tmpAE9->rgn_po=0,((_tmpAE9->fields=_tmp122,((_tmpAE9->tagged=0,_tmpAE9)))))))))),((
_tmpAE7->attributes=0,_tmpAE7)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*
_tmpAEF;struct Cyc_Absyn_Aggr_d_struct _tmpAEE;struct Cyc_List_List*_tmpAED;Cyc_Toc_result_decls=((
_tmpAED=_cycalloc(sizeof(*_tmpAED)),((_tmpAED->hd=Cyc_Absyn_new_decl((void*)((
_tmpAEF=_cycalloc(sizeof(*_tmpAEF)),((_tmpAEF[0]=((_tmpAEE.tag=6,((_tmpAEE.f1=
_tmp125,_tmpAEE)))),_tmpAEF)))),0),((_tmpAED->tl=Cyc_Toc_result_decls,_tmpAED))))));}{
struct _tuple17*_tmpAF2;struct Cyc_List_List*_tmpAF1;*_tmp119=((_tmpAF1=
_region_malloc(d,sizeof(*_tmpAF1)),((_tmpAF1->hd=((_tmpAF2=_region_malloc(d,
sizeof(*_tmpAF2)),((_tmpAF2->f1=x,((_tmpAF2->f2=ts,_tmpAF2)))))),((_tmpAF1->tl=*
_tmp119,_tmpAF1))))));}{void*_tmp12B=x;_npop_handler(0);return _tmp12B;};};};;
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
struct Cyc_Int_pa_struct _tmpAFA;void*_tmpAF9[1];const char*_tmpAF8;struct
_dyneither_ptr*_tmpAF7;struct _dyneither_ptr*xname=(_tmpAF7=_cycalloc(sizeof(*
_tmpAF7)),((_tmpAF7[0]=(struct _dyneither_ptr)((_tmpAFA.tag=1,((_tmpAFA.f1=(
unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAF9[0]=& _tmpAFA,Cyc_aprintf(((
_tmpAF8="_tuple%d",_tag_dyneither(_tmpAF8,sizeof(char),9))),_tag_dyneither(
_tmpAF9,sizeof(void*),1)))))))),_tmpAF7)));void*x=Cyc_Absyn_strct(xname);struct
Cyc_List_List*_tmp13F=0;{struct _tuple18*_tmpAFD;struct Cyc_List_List*_tmpAFC;*
_tmp135=((_tmpAFC=_region_malloc(d,sizeof(*_tmpAFC)),((_tmpAFC->hd=((_tmpAFD=
_region_malloc(d,sizeof(*_tmpAFD)),((_tmpAFD->f1=struct_name,((_tmpAFD->f2=
type_args,((_tmpAFD->f3=x,_tmpAFD)))))))),((_tmpAFC->tl=*_tmp135,_tmpAFC))))));}{
struct _RegionHandle _tmp142=_new_region("r");struct _RegionHandle*r=& _tmp142;
_push_region(r);{struct Cyc_List_List*_tmp143=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,type_vars,type_args);for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp144=(struct Cyc_Absyn_Aggrfield*)fields->hd;void*t=
_tmp144->type;t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp143,t));if(Cyc_Tcutil_unify(
t,(void*)& Cyc_Absyn_VoidType_val))break;t=Cyc_Toc_typ_to_c(t);{struct Cyc_Absyn_Aggrfield*
_tmpB00;struct Cyc_List_List*_tmpAFF;_tmp13F=((_tmpAFF=_cycalloc(sizeof(*_tmpAFF)),((
_tmpAFF->hd=((_tmpB00=_cycalloc(sizeof(*_tmpB00)),((_tmpB00->name=_tmp144->name,((
_tmpB00->tq=Cyc_Toc_mt_tq,((_tmpB00->type=t,((_tmpB00->width=_tmp144->width,((
_tmpB00->attributes=_tmp144->attributes,_tmpB00)))))))))))),((_tmpAFF->tl=
_tmp13F,_tmpAFF))))));};}_tmp13F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp13F);{struct Cyc_Absyn_AggrdeclImpl*_tmpB05;struct _tuple0*
_tmpB04;struct Cyc_Absyn_Aggrdecl*_tmpB03;struct Cyc_Absyn_Aggrdecl*_tmp147=(
_tmpB03=_cycalloc(sizeof(*_tmpB03)),((_tmpB03->kind=Cyc_Absyn_StructA,((_tmpB03->sc=
Cyc_Absyn_Public,((_tmpB03->name=((_tmpB04=_cycalloc(sizeof(*_tmpB04)),((_tmpB04->f1=
Cyc_Absyn_Rel_n(0),((_tmpB04->f2=xname,_tmpB04)))))),((_tmpB03->tvs=0,((_tmpB03->impl=((
_tmpB05=_cycalloc(sizeof(*_tmpB05)),((_tmpB05->exist_vars=0,((_tmpB05->rgn_po=0,((
_tmpB05->fields=_tmp13F,((_tmpB05->tagged=0,_tmpB05)))))))))),((_tmpB03->attributes=
0,_tmpB03)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpB0B;struct Cyc_Absyn_Aggr_d_struct
_tmpB0A;struct Cyc_List_List*_tmpB09;Cyc_Toc_result_decls=((_tmpB09=_cycalloc(
sizeof(*_tmpB09)),((_tmpB09->hd=Cyc_Absyn_new_decl((void*)((_tmpB0B=_cycalloc(
sizeof(*_tmpB0B)),((_tmpB0B[0]=((_tmpB0A.tag=6,((_tmpB0A.f1=_tmp147,_tmpB0A)))),
_tmpB0B)))),0),((_tmpB09->tl=Cyc_Toc_result_decls,_tmpB09))))));}{void*_tmp14B=x;
_npop_handler(1);return _tmp14B;};};};_pop_region(r);};};;_pop_dynregion(d);};}
struct _tuple0*Cyc_Toc_temp_var(){int _tmp153=Cyc_Toc_temp_var_counter ++;struct
_dyneither_ptr*_tmpB18;const char*_tmpB17;void*_tmpB16[1];struct Cyc_Int_pa_struct
_tmpB15;struct _tuple0*_tmpB14;struct _tuple0*res=(_tmpB14=_cycalloc(sizeof(*
_tmpB14)),((_tmpB14->f1=Cyc_Absyn_Loc_n,((_tmpB14->f2=((_tmpB18=_cycalloc(
sizeof(*_tmpB18)),((_tmpB18[0]=(struct _dyneither_ptr)((_tmpB15.tag=1,((_tmpB15.f1=(
unsigned int)_tmp153,((_tmpB16[0]=& _tmpB15,Cyc_aprintf(((_tmpB17="_tmp%X",
_tag_dyneither(_tmpB17,sizeof(char),7))),_tag_dyneither(_tmpB16,sizeof(void*),1)))))))),
_tmpB18)))),_tmpB14)))));return res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp15A;struct Cyc_Xarray_Xarray*_tmp15B;struct Cyc_Toc_TocState
_tmp159=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp15A=
_tmp159.dyn;_tmp15B=_tmp159.temp_labels;{struct _DynRegionFrame _tmp15C;struct
_RegionHandle*d=_open_dynregion(& _tmp15C,_tmp15A);{int _tmp15D=Cyc_Toc_fresh_label_counter
++;if(_tmp15D < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp15B)){
struct _dyneither_ptr*_tmp15E=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp15B,_tmp15D);_npop_handler(0);return _tmp15E;}{struct Cyc_Int_pa_struct
_tmpB20;void*_tmpB1F[1];const char*_tmpB1E;struct _dyneither_ptr*_tmpB1D;struct
_dyneither_ptr*res=(_tmpB1D=_cycalloc(sizeof(*_tmpB1D)),((_tmpB1D[0]=(struct
_dyneither_ptr)((_tmpB20.tag=1,((_tmpB20.f1=(unsigned int)_tmp15D,((_tmpB1F[0]=&
_tmpB20,Cyc_aprintf(((_tmpB1E="_LL%X",_tag_dyneither(_tmpB1E,sizeof(char),6))),
_tag_dyneither(_tmpB1F,sizeof(void*),1)))))))),_tmpB1D)));if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp15B,res)!= _tmp15D){const char*
_tmpB23;void*_tmpB22;(_tmpB22=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB23="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmpB23,sizeof(char),43))),_tag_dyneither(_tmpB22,sizeof(void*),0)));}{
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
struct _tuple8*_tmpB24;return(_tmpB24=_cycalloc(sizeof(*_tmpB24)),((_tmpB24->f1=
_tmp168,((_tmpB24->f2=_tmp169,((_tmpB24->f3=Cyc_Toc_typ_to_c(_tmp16A),_tmpB24)))))));};}
static struct _tuple10*Cyc_Toc_typ_to_c_f(struct _tuple10*x){struct Cyc_Absyn_Tqual
_tmp16D;void*_tmp16E;struct _tuple10 _tmp16C=*x;_tmp16D=_tmp16C.f1;_tmp16E=_tmp16C.f2;{
struct _tuple10*_tmpB25;return(_tmpB25=_cycalloc(sizeof(*_tmpB25)),((_tmpB25->f1=
_tmp16D,((_tmpB25->f2=Cyc_Toc_typ_to_c(_tmp16E),_tmpB25)))));};}static void*Cyc_Toc_typ_to_c_array(
void*t){void*_tmp170=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp172;
void*_tmp173;struct Cyc_Absyn_Tqual _tmp174;struct Cyc_Absyn_Exp*_tmp175;union Cyc_Absyn_Constraint*
_tmp176;unsigned int _tmp177;struct Cyc_Core_Opt*_tmp179;struct Cyc_Core_Opt _tmp17A;
void*_tmp17B;_LL38: {struct Cyc_Absyn_ArrayType_struct*_tmp171=(struct Cyc_Absyn_ArrayType_struct*)
_tmp170;if(_tmp171->tag != 8)goto _LL3A;else{_tmp172=_tmp171->f1;_tmp173=_tmp172.elt_type;
_tmp174=_tmp172.tq;_tmp175=_tmp172.num_elts;_tmp176=_tmp172.zero_term;_tmp177=
_tmp172.zt_loc;}}_LL39: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp173),
_tmp174,_tmp175,Cyc_Absyn_false_conref,_tmp177);_LL3A: {struct Cyc_Absyn_Evar_struct*
_tmp178=(struct Cyc_Absyn_Evar_struct*)_tmp170;if(_tmp178->tag != 1)goto _LL3C;
else{_tmp179=_tmp178->f2;if(_tmp179 == 0)goto _LL3C;_tmp17A=*_tmp179;_tmp17B=(void*)
_tmp17A.v;}}_LL3B: return Cyc_Toc_typ_to_c_array(_tmp17B);_LL3C:;_LL3D: return Cyc_Toc_typ_to_c(
t);_LL37:;}static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*
f){struct Cyc_Absyn_Aggrfield*_tmpB26;return(_tmpB26=_cycalloc(sizeof(*_tmpB26)),((
_tmpB26->name=f->name,((_tmpB26->tq=Cyc_Toc_mt_tq,((_tmpB26->type=Cyc_Toc_typ_to_c(
f->type),((_tmpB26->width=f->width,((_tmpB26->attributes=f->attributes,_tmpB26)))))))))));}
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){return;}static void*Cyc_Toc_char_star_typ(){
static void**cs=0;if(cs == 0){void**_tmpB27;cs=((_tmpB27=_cycalloc(sizeof(*_tmpB27)),((
_tmpB27[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpB27))));}return*cs;}static void*Cyc_Toc_rgn_typ(){
static void**r=0;if(r == 0){void**_tmpB28;r=((_tmpB28=_cycalloc(sizeof(*_tmpB28)),((
_tmpB28[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),
_tmpB28))));}return*r;}static void*Cyc_Toc_dyn_rgn_typ(){static void**r=0;if(r == 0){
void**_tmpB29;r=((_tmpB29=_cycalloc(sizeof(*_tmpB29)),((_tmpB29[0]=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpB29))));}return*
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
unsigned int _tmp1A5;struct Cyc_Absyn_FnInfo _tmp1A7;struct Cyc_Absyn_Tqual _tmp1A8;
void*_tmp1A9;struct Cyc_List_List*_tmp1AA;int _tmp1AB;struct Cyc_Absyn_VarargInfo*
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
const char*_tmpB2A;return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp196->name,((
_tmpB2A="_struct",_tag_dyneither(_tmpB2A,sizeof(char),8)))));}_LL55: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp197=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp186;if(_tmp197->tag != 4)
goto _LL57;}_LL56: {const char*_tmpB2D;void*_tmpB2C;(_tmpB2C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2D="unresolved DatatypeFieldType",
_tag_dyneither(_tmpB2D,sizeof(char),29))),_tag_dyneither(_tmpB2C,sizeof(void*),0)));}
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
_LL96: continue;_LL97:;_LL98:{struct Cyc_List_List*_tmpB2E;_tmp1E3=((_tmpB2E=
_cycalloc(sizeof(*_tmpB2E)),((_tmpB2E->hd=(void*)_tmp1AD->hd,((_tmpB2E->tl=
_tmp1E3,_tmpB2E))))));}goto _LL8C;_LL8C:;}{struct Cyc_List_List*_tmp1EB=((struct
Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_arg_to_c,_tmp1AA);if(_tmp1AC != 0){void*_tmp1EC=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp1AC->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple8*_tmpB2F;struct _tuple8*_tmp1ED=(_tmpB2F=_cycalloc(sizeof(*_tmpB2F)),((
_tmpB2F->f1=_tmp1AC->name,((_tmpB2F->f2=_tmp1AC->tq,((_tmpB2F->f3=_tmp1EC,
_tmpB2F)))))));struct Cyc_List_List*_tmpB30;_tmp1EB=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1EB,((
_tmpB30=_cycalloc(sizeof(*_tmpB30)),((_tmpB30->hd=_tmp1ED,((_tmpB30->tl=0,
_tmpB30)))))));}{struct Cyc_Absyn_FnType_struct _tmpB36;struct Cyc_Absyn_FnInfo
_tmpB35;struct Cyc_Absyn_FnType_struct*_tmpB34;return(void*)((_tmpB34=_cycalloc(
sizeof(*_tmpB34)),((_tmpB34[0]=((_tmpB36.tag=9,((_tmpB36.f1=((_tmpB35.tvars=0,((
_tmpB35.effect=0,((_tmpB35.ret_tqual=_tmp1A8,((_tmpB35.ret_typ=Cyc_Toc_typ_to_c(
_tmp1A9),((_tmpB35.args=_tmp1EB,((_tmpB35.c_varargs=_tmp1AB,((_tmpB35.cyc_varargs=
0,((_tmpB35.rgn_po=0,((_tmpB35.attributes=_tmp1E3,_tmpB35)))))))))))))))))),
_tmpB36)))),_tmpB34))));};};}_LL61: {struct Cyc_Absyn_TupleType_struct*_tmp1AE=(
struct Cyc_Absyn_TupleType_struct*)_tmp186;if(_tmp1AE->tag != 10)goto _LL63;else{
_tmp1AF=_tmp1AE->f1;}}_LL62: _tmp1AF=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(
struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1AF);
return Cyc_Toc_add_tuple_type(_tmp1AF);_LL63: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp1B0=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp186;if(_tmp1B0->tag != 12)goto
_LL65;else{_tmp1B1=_tmp1B0->f1;_tmp1B2=_tmp1B0->f2;}}_LL64: {struct Cyc_Absyn_AnonAggrType_struct
_tmpB39;struct Cyc_Absyn_AnonAggrType_struct*_tmpB38;return(void*)((_tmpB38=
_cycalloc(sizeof(*_tmpB38)),((_tmpB38[0]=((_tmpB39.tag=12,((_tmpB39.f1=_tmp1B1,((
_tmpB39.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1B2),_tmpB39)))))),
_tmpB38))));}_LL65: {struct Cyc_Absyn_AggrType_struct*_tmp1B3=(struct Cyc_Absyn_AggrType_struct*)
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
if(_tmp1BD != 0){struct Cyc_Absyn_TypedefType_struct _tmpB3C;struct Cyc_Absyn_TypedefType_struct*
_tmpB3B;return(void*)((_tmpB3B=_cycalloc(sizeof(*_tmpB3B)),((_tmpB3B[0]=((
_tmpB3C.tag=17,((_tmpB3C.f1=_tmp1BC,((_tmpB3C.f2=0,((_tmpB3C.f3=_tmp1BE,((
_tmpB3C.f4=0,_tmpB3C)))))))))),_tmpB3B))));}else{return t;}}else{struct Cyc_Absyn_TypedefType_struct
_tmpB42;void**_tmpB41;struct Cyc_Absyn_TypedefType_struct*_tmpB40;return(void*)((
_tmpB40=_cycalloc(sizeof(*_tmpB40)),((_tmpB40[0]=((_tmpB42.tag=17,((_tmpB42.f1=
_tmp1BC,((_tmpB42.f2=0,((_tmpB42.f3=_tmp1BE,((_tmpB42.f4=((_tmpB41=_cycalloc(
sizeof(*_tmpB41)),((_tmpB41[0]=Cyc_Toc_typ_to_c_array(*_tmp1BF),_tmpB41)))),
_tmpB42)))))))))),_tmpB40))));}_LL6D: {struct Cyc_Absyn_TagType_struct*_tmp1C0=(
struct Cyc_Absyn_TagType_struct*)_tmp186;if(_tmp1C0->tag != 19)goto _LL6F;}_LL6E:
return Cyc_Absyn_uint_typ;_LL6F: {struct Cyc_Absyn_RgnHandleType_struct*_tmp1C1=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp186;if(_tmp1C1->tag != 15)goto _LL71;
else{_tmp1C2=(void*)_tmp1C1->f1;}}_LL70: return Cyc_Toc_rgn_typ();_LL71: {struct
Cyc_Absyn_DynRgnType_struct*_tmp1C3=(struct Cyc_Absyn_DynRgnType_struct*)_tmp186;
if(_tmp1C3->tag != 16)goto _LL73;}_LL72: return Cyc_Toc_dyn_rgn_typ();_LL73: {struct
Cyc_Absyn_HeapRgn_struct*_tmp1C4=(struct Cyc_Absyn_HeapRgn_struct*)_tmp186;if(
_tmp1C4->tag != 20)goto _LL75;}_LL74: {const char*_tmpB45;void*_tmpB44;(_tmpB44=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB45="Toc::typ_to_c: type translation passed the heap region",_tag_dyneither(
_tmpB45,sizeof(char),55))),_tag_dyneither(_tmpB44,sizeof(void*),0)));}_LL75: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp1C5=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp186;if(_tmp1C5->tag != 21)goto _LL77;}_LL76: {const char*_tmpB48;void*_tmpB47;(
_tmpB47=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB48="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpB48,sizeof(char),57))),_tag_dyneither(_tmpB47,sizeof(void*),0)));}
_LL77: {struct Cyc_Absyn_RefCntRgn_struct*_tmp1C6=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp186;if(_tmp1C6->tag != 22)goto _LL79;}_LL78: {const char*_tmpB4B;void*_tmpB4A;(
_tmpB4A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB4B="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpB4B,sizeof(char),57))),_tag_dyneither(_tmpB4A,sizeof(void*),0)));}
_LL79: {struct Cyc_Absyn_AccessEff_struct*_tmp1C7=(struct Cyc_Absyn_AccessEff_struct*)
_tmp186;if(_tmp1C7->tag != 23)goto _LL7B;}_LL7A: goto _LL7C;_LL7B: {struct Cyc_Absyn_JoinEff_struct*
_tmp1C8=(struct Cyc_Absyn_JoinEff_struct*)_tmp186;if(_tmp1C8->tag != 24)goto _LL7D;}
_LL7C: goto _LL7E;_LL7D: {struct Cyc_Absyn_RgnsEff_struct*_tmp1C9=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp186;if(_tmp1C9->tag != 25)goto _LL7F;}_LL7E: {const char*_tmpB4E;void*_tmpB4D;(
_tmpB4D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB4E="Toc::typ_to_c: type translation passed an effect",_tag_dyneither(
_tmpB4E,sizeof(char),49))),_tag_dyneither(_tmpB4D,sizeof(void*),0)));}_LL7F: {
struct Cyc_Absyn_ValueofType_struct*_tmp1CA=(struct Cyc_Absyn_ValueofType_struct*)
_tmp186;if(_tmp1CA->tag != 18)goto _LL81;}_LL80: {const char*_tmpB51;void*_tmpB50;(
_tmpB50=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB51="Toc::typ_to_c: type translation passed a valueof_t",_tag_dyneither(
_tmpB51,sizeof(char),51))),_tag_dyneither(_tmpB50,sizeof(void*),0)));}_LL81: {
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
void*t,struct Cyc_Absyn_Exp*e,unsigned int l){void*_tmp20D=t;struct Cyc_Absyn_ArrayInfo
_tmp20F;void*_tmp210;struct Cyc_Absyn_Tqual _tmp211;_LL9F: {struct Cyc_Absyn_ArrayType_struct*
_tmp20E=(struct Cyc_Absyn_ArrayType_struct*)_tmp20D;if(_tmp20E->tag != 8)goto _LLA1;
else{_tmp20F=_tmp20E->f1;_tmp210=_tmp20F.elt_type;_tmp211=_tmp20F.tq;}}_LLA0:
return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp210,(void*)& Cyc_Absyn_HeapRgn_val,
_tmp211,Cyc_Absyn_false_conref),e);_LLA1:;_LLA2: return Cyc_Toc_cast_it(t,e);_LL9E:;}
static int Cyc_Toc_atomic_typ(void*t){void*_tmp212=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_ArrayInfo _tmp21C;void*_tmp21D;struct Cyc_Absyn_AggrInfo _tmp21F;union Cyc_Absyn_AggrInfoU
_tmp220;struct Cyc_List_List*_tmp222;struct Cyc_Absyn_DatatypeFieldInfo _tmp224;
union Cyc_Absyn_DatatypeFieldInfoU _tmp225;struct _tuple1 _tmp226;struct Cyc_Absyn_Datatypedecl*
_tmp227;struct Cyc_Absyn_Datatypefield*_tmp228;struct Cyc_List_List*_tmp22A;_LLA4: {
struct Cyc_Absyn_VoidType_struct*_tmp213=(struct Cyc_Absyn_VoidType_struct*)
_tmp212;if(_tmp213->tag != 0)goto _LLA6;}_LLA5: return 1;_LLA6: {struct Cyc_Absyn_VarType_struct*
_tmp214=(struct Cyc_Absyn_VarType_struct*)_tmp212;if(_tmp214->tag != 2)goto _LLA8;}
_LLA7: return 0;_LLA8: {struct Cyc_Absyn_IntType_struct*_tmp215=(struct Cyc_Absyn_IntType_struct*)
_tmp212;if(_tmp215->tag != 6)goto _LLAA;}_LLA9: goto _LLAB;_LLAA: {struct Cyc_Absyn_EnumType_struct*
_tmp216=(struct Cyc_Absyn_EnumType_struct*)_tmp212;if(_tmp216->tag != 13)goto _LLAC;}
_LLAB: goto _LLAD;_LLAC: {struct Cyc_Absyn_AnonEnumType_struct*_tmp217=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp212;if(_tmp217->tag != 14)goto _LLAE;}_LLAD: goto _LLAF;_LLAE: {struct Cyc_Absyn_FloatType_struct*
_tmp218=(struct Cyc_Absyn_FloatType_struct*)_tmp212;if(_tmp218->tag != 7)goto _LLB0;}
_LLAF: goto _LLB1;_LLB0: {struct Cyc_Absyn_FnType_struct*_tmp219=(struct Cyc_Absyn_FnType_struct*)
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
_LLC6;}_LLC5: return 0;_LLC6:;_LLC7: {const char*_tmpB55;void*_tmpB54[1];struct Cyc_String_pa_struct
_tmpB53;(_tmpB53.tag=0,((_tmpB53.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpB54[0]=& _tmpB53,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB55="atomic_typ:  bad type %s",
_tag_dyneither(_tmpB55,sizeof(char),25))),_tag_dyneither(_tmpB54,sizeof(void*),1)))))));}
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
if(_tmp242->impl == 0){const char*_tmpB58;void*_tmpB57;(_tmpB57=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB58="is_poly_field: type missing fields",
_tag_dyneither(_tmpB58,sizeof(char),35))),_tag_dyneither(_tmpB57,sizeof(void*),0)));}
_tmp241=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp242->impl))->fields;goto
_LLDB;}_LLDA: {struct Cyc_Absyn_AnonAggrType_struct*_tmp240=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp23C;if(_tmp240->tag != 12)goto _LLDC;else{_tmp241=_tmp240->f2;}}_LLDB: {struct
Cyc_Absyn_Aggrfield*_tmp245=Cyc_Absyn_lookup_field(_tmp241,f);if(_tmp245 == 0){
const char*_tmpB5C;void*_tmpB5B[1];struct Cyc_String_pa_struct _tmpB5A;(_tmpB5A.tag=
0,((_tmpB5A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB5B[0]=&
_tmpB5A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB5C="is_poly_field: bad field %s",_tag_dyneither(_tmpB5C,sizeof(char),28))),
_tag_dyneither(_tmpB5B,sizeof(void*),1)))))));}return Cyc_Toc_is_void_star_or_boxed_tvar(
_tmp245->type);}_LLDC:;_LLDD: {const char*_tmpB60;void*_tmpB5F[1];struct Cyc_String_pa_struct
_tmpB5E;(_tmpB5E.tag=0,((_tmpB5E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpB5F[0]=& _tmpB5E,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB60="is_poly_field: bad type %s",
_tag_dyneither(_tmpB60,sizeof(char),27))),_tag_dyneither(_tmpB5F,sizeof(void*),1)))))));}
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
_tmpB64;void*_tmpB63[1];struct Cyc_String_pa_struct _tmpB62;(_tmpB62.tag=0,((
_tmpB62.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp251->topt))->v)),((_tmpB63[0]=&
_tmpB62,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB64="is_poly_project: bad type %s",_tag_dyneither(_tmpB64,sizeof(char),29))),
_tag_dyneither(_tmpB63,sizeof(void*),1)))))));}_LLE5:;}_LLE3:;_LLE4: return 0;
_LLDE:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB65;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((
_tmpB65=_cycalloc(sizeof(*_tmpB65)),((_tmpB65->hd=s,((_tmpB65->tl=0,_tmpB65)))))),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB66;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((
_tmpB66=_cycalloc(sizeof(*_tmpB66)),((_tmpB66->hd=s,((_tmpB66->tl=0,_tmpB66)))))),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(
s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmpB67[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((
_tmpB67[1]=s,((_tmpB67[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpB67,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,
struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*
_tmpB68[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpB68[1]=n,((
_tmpB68[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB68,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{struct Cyc_Absyn_Exp*
_tmpB69[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpB69[1]=n,((
_tmpB69[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB69,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n){struct Cyc_Absyn_Exp*_tmpB6A[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((
_tmpB6A[2]=n,((_tmpB6A[1]=s,((_tmpB6A[0]=rgn,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB6A,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmpB6B;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmpB6B=_cycalloc(sizeof(*_tmpB6B)),((_tmpB6B->hd=e,((_tmpB6B->tl=0,_tmpB6B)))))),
0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){struct Cyc_Absyn_Exp*
_tmpB6C[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpB6C,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
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
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_struct*_tmpB72;struct
Cyc_Absyn_Var_d_struct _tmpB71;struct Cyc_List_List*_tmpB70;Cyc_Toc_result_decls=((
_tmpB70=_cycalloc(sizeof(*_tmpB70)),((_tmpB70->hd=Cyc_Absyn_new_decl((void*)((
_tmpB72=_cycalloc(sizeof(*_tmpB72)),((_tmpB72[0]=((_tmpB71.tag=0,((_tmpB71.f1=vd,
_tmpB71)))),_tmpB72)))),0),((_tmpB70->tl=Cyc_Toc_result_decls,_tmpB70))))));}
xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*urm_exp;{struct _tuple16*_tmpB79;
struct _tuple16*_tmpB78;struct _tuple16*_tmpB77;struct _tuple16*_tmpB76[3];urm_exp=
Cyc_Absyn_unresolvedmem_exp(0,((_tmpB76[2]=((_tmpB77=_cycalloc(sizeof(*_tmpB77)),((
_tmpB77->f1=0,((_tmpB77->f2=xplussz,_tmpB77)))))),((_tmpB76[1]=((_tmpB78=
_cycalloc(sizeof(*_tmpB78)),((_tmpB78->f1=0,((_tmpB78->f2=xexp,_tmpB78)))))),((
_tmpB76[0]=((_tmpB79=_cycalloc(sizeof(*_tmpB79)),((_tmpB79->f1=0,((_tmpB79->f2=
xexp,_tmpB79)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB76,sizeof(struct _tuple16*),3)))))))),0);}return urm_exp;};};}
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
Cyc_Toc_empty_env(struct _RegionHandle*r){int*_tmpB7C;struct Cyc_Toc_Env*_tmpB7B;
return(_tmpB7B=_region_malloc(r,sizeof(*_tmpB7B)),((_tmpB7B->break_lab=(struct
_dyneither_ptr**)0,((_tmpB7B->continue_lab=(struct _dyneither_ptr**)0,((_tmpB7B->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpB7B->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp),((_tmpB7B->toplevel=(int)1,((_tmpB7B->in_lhs=(int*)((
_tmpB7C=_region_malloc(r,sizeof(*_tmpB7C)),((_tmpB7C[0]=0,_tmpB7C)))),((_tmpB7B->rgn=(
struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB7B)))))))))))))));}static struct
Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct
Cyc_Toc_Env _tmp27C;struct _dyneither_ptr**_tmp27D;struct _dyneither_ptr**_tmp27E;
struct Cyc_Toc_FallthruInfo*_tmp27F;struct Cyc_Dict_Dict _tmp280;int _tmp281;int*
_tmp282;struct Cyc_Toc_Env*_tmp27B=e;_tmp27C=*_tmp27B;_tmp27D=_tmp27C.break_lab;
_tmp27E=_tmp27C.continue_lab;_tmp27F=_tmp27C.fallthru_info;_tmp280=_tmp27C.varmap;
_tmp281=_tmp27C.toplevel;_tmp282=_tmp27C.in_lhs;{struct Cyc_Toc_Env*_tmpB7D;
return(_tmpB7D=_region_malloc(r,sizeof(*_tmpB7D)),((_tmpB7D->break_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp27D),((_tmpB7D->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp27E),((_tmpB7D->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp27F,((_tmpB7D->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp280),((_tmpB7D->toplevel=(int)_tmp281,((_tmpB7D->in_lhs=(int*)_tmp282,((
_tmpB7D->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB7D)))))))))))))));};}
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp285;struct _dyneither_ptr**_tmp286;struct _dyneither_ptr**
_tmp287;struct Cyc_Toc_FallthruInfo*_tmp288;struct Cyc_Dict_Dict _tmp289;int _tmp28A;
int*_tmp28B;struct Cyc_Toc_Env*_tmp284=e;_tmp285=*_tmp284;_tmp286=_tmp285.break_lab;
_tmp287=_tmp285.continue_lab;_tmp288=_tmp285.fallthru_info;_tmp289=_tmp285.varmap;
_tmp28A=_tmp285.toplevel;_tmp28B=_tmp285.in_lhs;{struct Cyc_Toc_Env*_tmpB7E;
return(_tmpB7E=_region_malloc(r,sizeof(*_tmpB7E)),((_tmpB7E->break_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp286),((_tmpB7E->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp287),((_tmpB7E->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp288,((_tmpB7E->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp289),((_tmpB7E->toplevel=(int)0,((_tmpB7E->in_lhs=(int*)_tmp28B,((_tmpB7E->rgn=(
struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB7E)))))))))))))));};}static
struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env _tmp28E;struct _dyneither_ptr**_tmp28F;struct _dyneither_ptr**
_tmp290;struct Cyc_Toc_FallthruInfo*_tmp291;struct Cyc_Dict_Dict _tmp292;int _tmp293;
int*_tmp294;struct Cyc_Toc_Env*_tmp28D=e;_tmp28E=*_tmp28D;_tmp28F=_tmp28E.break_lab;
_tmp290=_tmp28E.continue_lab;_tmp291=_tmp28E.fallthru_info;_tmp292=_tmp28E.varmap;
_tmp293=_tmp28E.toplevel;_tmp294=_tmp28E.in_lhs;{struct Cyc_Toc_Env*_tmpB7F;
return(_tmpB7F=_region_malloc(r,sizeof(*_tmpB7F)),((_tmpB7F->break_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp28F),((_tmpB7F->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp290),((_tmpB7F->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp291,((_tmpB7F->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp292),((_tmpB7F->toplevel=(int)1,((_tmpB7F->in_lhs=(int*)_tmp294,((_tmpB7F->rgn=(
struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB7F)))))))))))))));};}static void
Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){struct Cyc_Toc_Env _tmp297;int*_tmp298;
struct Cyc_Toc_Env*_tmp296=e;_tmp297=*_tmp296;_tmp298=_tmp297.in_lhs;*_tmp298=b;}
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){{union Cyc_Absyn_Nmspace _tmp299=(*x).f1;
struct Cyc_List_List*_tmp29A;_LLF2: if((_tmp299.Rel_n).tag != 1)goto _LLF4;_tmp29A=(
struct Cyc_List_List*)(_tmp299.Rel_n).val;_LLF3: {const char*_tmpB83;void*_tmpB82[
1];struct Cyc_String_pa_struct _tmpB81;(_tmpB81.tag=0,((_tmpB81.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpB82[0]=&
_tmpB81,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB83="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpB83,sizeof(char),30))),
_tag_dyneither(_tmpB82,sizeof(void*),1)))))));}_LLF4:;_LLF5: goto _LLF1;_LLF1:;}{
struct Cyc_Toc_Env _tmp29F;struct _dyneither_ptr**_tmp2A0;struct _dyneither_ptr**
_tmp2A1;struct Cyc_Toc_FallthruInfo*_tmp2A2;struct Cyc_Dict_Dict _tmp2A3;int _tmp2A4;
int*_tmp2A5;struct Cyc_Toc_Env*_tmp29E=e;_tmp29F=*_tmp29E;_tmp2A0=_tmp29F.break_lab;
_tmp2A1=_tmp29F.continue_lab;_tmp2A2=_tmp29F.fallthru_info;_tmp2A3=_tmp29F.varmap;
_tmp2A4=_tmp29F.toplevel;_tmp2A5=_tmp29F.in_lhs;{struct Cyc_Dict_Dict _tmp2A6=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*
v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp2A3),x,y);struct Cyc_Toc_Env*_tmpB84;return(_tmpB84=
_region_malloc(r,sizeof(*_tmpB84)),((_tmpB84->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp2A0),((_tmpB84->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp2A1),((_tmpB84->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp2A2,((_tmpB84->varmap=(struct Cyc_Dict_Dict)_tmp2A6,((_tmpB84->toplevel=(int)
_tmp2A4,((_tmpB84->in_lhs=(int*)_tmp2A5,((_tmpB84->rgn=(struct _RegionHandle*)r,(
struct Cyc_Toc_Env*)_tmpB84)))))))))))))));};};}static struct Cyc_Toc_Env*Cyc_Toc_loop_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp2A9;struct
_dyneither_ptr**_tmp2AA;struct _dyneither_ptr**_tmp2AB;struct Cyc_Toc_FallthruInfo*
_tmp2AC;struct Cyc_Dict_Dict _tmp2AD;int _tmp2AE;int*_tmp2AF;struct Cyc_Toc_Env*
_tmp2A8=e;_tmp2A9=*_tmp2A8;_tmp2AA=_tmp2A9.break_lab;_tmp2AB=_tmp2A9.continue_lab;
_tmp2AC=_tmp2A9.fallthru_info;_tmp2AD=_tmp2A9.varmap;_tmp2AE=_tmp2A9.toplevel;
_tmp2AF=_tmp2A9.in_lhs;{struct Cyc_Toc_Env*_tmpB85;return(_tmpB85=_region_malloc(
r,sizeof(*_tmpB85)),((_tmpB85->break_lab=(struct _dyneither_ptr**)0,((_tmpB85->continue_lab=(
struct _dyneither_ptr**)0,((_tmpB85->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp2AC,((_tmpB85->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2AD),((_tmpB85->toplevel=(
int)_tmp2AE,((_tmpB85->in_lhs=(int*)_tmp2AF,((_tmpB85->rgn=(struct _RegionHandle*)
r,(struct Cyc_Toc_Env*)_tmpB85)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmpB86;fallthru_vars=((
_tmpB86=_region_malloc(r,sizeof(*_tmpB86)),((_tmpB86->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmpB86->tl=fallthru_vars,_tmpB86))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp2B3;struct _dyneither_ptr**_tmp2B4;struct
_dyneither_ptr**_tmp2B5;struct Cyc_Toc_FallthruInfo*_tmp2B6;struct Cyc_Dict_Dict
_tmp2B7;int _tmp2B8;int*_tmp2B9;struct Cyc_Toc_Env*_tmp2B2=e;_tmp2B3=*_tmp2B2;
_tmp2B4=_tmp2B3.break_lab;_tmp2B5=_tmp2B3.continue_lab;_tmp2B6=_tmp2B3.fallthru_info;
_tmp2B7=_tmp2B3.varmap;_tmp2B8=_tmp2B3.toplevel;_tmp2B9=_tmp2B3.in_lhs;{struct
Cyc_Toc_Env _tmp2BB;struct Cyc_Dict_Dict _tmp2BC;struct Cyc_Toc_Env*_tmp2BA=
next_case_env;_tmp2BB=*_tmp2BA;_tmp2BC=_tmp2BB.varmap;{struct Cyc_Toc_FallthruInfo*
_tmpB87;struct Cyc_Toc_FallthruInfo*fi=(_tmpB87=_region_malloc(r,sizeof(*_tmpB87)),((
_tmpB87->label=fallthru_l,((_tmpB87->binders=fallthru_vars,((_tmpB87->next_case_env=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp2BC),_tmpB87)))))));struct _dyneither_ptr**_tmpB8A;struct Cyc_Toc_Env*
_tmpB89;return(_tmpB89=_region_malloc(r,sizeof(*_tmpB89)),((_tmpB89->break_lab=(
struct _dyneither_ptr**)((_tmpB8A=_region_malloc(r,sizeof(*_tmpB8A)),((_tmpB8A[0]=
break_l,_tmpB8A)))),((_tmpB89->continue_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp2B5),((_tmpB89->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
fi,((_tmpB89->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2B7),((_tmpB89->toplevel=(
int)_tmp2B8,((_tmpB89->in_lhs=(int*)((int*)_tmp2B9),((_tmpB89->rgn=(struct
_RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB89)))))))))))))));};};};}static struct
Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _dyneither_ptr*break_l){struct Cyc_Toc_Env _tmp2C1;struct _dyneither_ptr**
_tmp2C2;struct _dyneither_ptr**_tmp2C3;struct Cyc_Toc_FallthruInfo*_tmp2C4;struct
Cyc_Dict_Dict _tmp2C5;int _tmp2C6;int*_tmp2C7;struct Cyc_Toc_Env*_tmp2C0=e;_tmp2C1=*
_tmp2C0;_tmp2C2=_tmp2C1.break_lab;_tmp2C3=_tmp2C1.continue_lab;_tmp2C4=_tmp2C1.fallthru_info;
_tmp2C5=_tmp2C1.varmap;_tmp2C6=_tmp2C1.toplevel;_tmp2C7=_tmp2C1.in_lhs;{struct
_dyneither_ptr**_tmpB8D;struct Cyc_Toc_Env*_tmpB8C;return(_tmpB8C=_region_malloc(
r,sizeof(*_tmpB8C)),((_tmpB8C->break_lab=(struct _dyneither_ptr**)((_tmpB8D=
_region_malloc(r,sizeof(*_tmpB8D)),((_tmpB8D[0]=break_l,_tmpB8D)))),((_tmpB8C->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp2C3),((_tmpB8C->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpB8C->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2C5),((_tmpB8C->toplevel=(
int)_tmp2C6,((_tmpB8C->in_lhs=(int*)_tmp2C7,((_tmpB8C->rgn=(struct _RegionHandle*)
r,(struct Cyc_Toc_Env*)_tmpB8C)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp2CB;struct _dyneither_ptr**_tmp2CC;struct _dyneither_ptr**_tmp2CD;struct Cyc_Toc_FallthruInfo*
_tmp2CE;struct Cyc_Dict_Dict _tmp2CF;int _tmp2D0;int*_tmp2D1;struct Cyc_Toc_Env*
_tmp2CA=e;_tmp2CB=*_tmp2CA;_tmp2CC=_tmp2CB.break_lab;_tmp2CD=_tmp2CB.continue_lab;
_tmp2CE=_tmp2CB.fallthru_info;_tmp2CF=_tmp2CB.varmap;_tmp2D0=_tmp2CB.toplevel;
_tmp2D1=_tmp2CB.in_lhs;{struct Cyc_Toc_FallthruInfo*_tmpB90;struct Cyc_Toc_Env*
_tmpB8F;return(_tmpB8F=_region_malloc(r,sizeof(*_tmpB8F)),((_tmpB8F->break_lab=(
struct _dyneither_ptr**)0,((_tmpB8F->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp2CD),((_tmpB8F->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((
_tmpB90=_region_malloc(r,sizeof(*_tmpB90)),((_tmpB90->label=next_l,((_tmpB90->binders=
0,((_tmpB90->next_case_env=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*
cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),
_tmpB90)))))))),((_tmpB8F->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2CF),((_tmpB8F->toplevel=(
int)_tmp2D0,((_tmpB8F->in_lhs=(int*)((int*)_tmp2D1),((_tmpB8F->rgn=(struct
_RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB8F)))))))))))))));};}static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct
Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);static int Cyc_Toc_need_null_check(struct
Cyc_Absyn_Exp*e){void*_tmp2D4=e->annot;_LLF7: {struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp2D5=(struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp2D4;if(_tmp2D5->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLF9;}_LLF8: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLF9: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp2D6=(
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp2D4;if(_tmp2D6->tag != Cyc_CfFlowInfo_NotZero)
goto _LLFB;}_LLFA: return 0;_LLFB: {struct Cyc_CfFlowInfo_IsZero_struct*_tmp2D7=(
struct Cyc_CfFlowInfo_IsZero_struct*)_tmp2D4;if(_tmp2D7->tag != Cyc_CfFlowInfo_IsZero)
goto _LLFD;}_LLFC:{const char*_tmpB93;void*_tmpB92;(_tmpB92=0,Cyc_Tcutil_terr(e->loc,((
_tmpB93="dereference of NULL pointer",_tag_dyneither(_tmpB93,sizeof(char),28))),
_tag_dyneither(_tmpB92,sizeof(void*),0)));}return 0;_LLFD: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp2D8=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp2D4;if(_tmp2D8->tag != Cyc_Absyn_EmptyAnnot)
goto _LLFF;}_LLFE: return 0;_LLFF:;_LL100: {const char*_tmpB96;void*_tmpB95;(_tmpB95=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB96="need_null_check",_tag_dyneither(_tmpB96,sizeof(char),16))),
_tag_dyneither(_tmpB95,sizeof(void*),0)));}_LLF6:;}static struct Cyc_List_List*Cyc_Toc_get_relns(
struct Cyc_Absyn_Exp*e){void*_tmp2DD=e->annot;struct Cyc_List_List*_tmp2DF;struct
Cyc_List_List*_tmp2E1;_LL102: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2DE=(
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp2DD;if(_tmp2DE->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LL104;else{_tmp2DF=_tmp2DE->f1;}}_LL103: return _tmp2DF;_LL104: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmp2E0=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmp2DD;if(_tmp2E0->tag != Cyc_CfFlowInfo_NotZero)
goto _LL106;else{_tmp2E1=_tmp2E0->f1;}}_LL105: return _tmp2E1;_LL106: {struct Cyc_CfFlowInfo_IsZero_struct*
_tmp2E2=(struct Cyc_CfFlowInfo_IsZero_struct*)_tmp2DD;if(_tmp2E2->tag != Cyc_CfFlowInfo_IsZero)
goto _LL108;}_LL107:{const char*_tmpB99;void*_tmpB98;(_tmpB98=0,Cyc_Tcutil_terr(e->loc,((
_tmpB99="dereference of NULL pointer",_tag_dyneither(_tmpB99,sizeof(char),28))),
_tag_dyneither(_tmpB98,sizeof(void*),0)));}return 0;_LL108: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp2E3=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp2DD;if(_tmp2E3->tag != Cyc_Absyn_EmptyAnnot)
goto _LL10A;}_LL109: return 0;_LL10A:;_LL10B: {const char*_tmpB9C;void*_tmpB9B;(
_tmpB9B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB9C="get_relns",_tag_dyneither(_tmpB9C,sizeof(char),10))),_tag_dyneither(
_tmpB9B,sizeof(void*),0)));}_LL101:;}static int Cyc_Toc_check_const_array(
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
struct _tuple19 _tmpB9D;struct _tuple19 _tmp37D=(_tmpB9D.f1=Cyc_Tcutil_compress(
_tmp379),((_tmpB9D.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(a->topt))->v),_tmpB9D)));void*_tmp37E;void*_tmp380;void*_tmp381;
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
e){if(e->topt == 0){const char*_tmpBA0;void*_tmpB9F;(_tmpB9F=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA0="Missing type in primop ",
_tag_dyneither(_tmpBA0,sizeof(char),24))),_tag_dyneither(_tmpB9F,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*
_tmpBA3;void*_tmpBA2;(_tmpBA2=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA3="Missing type in primop ",
_tag_dyneither(_tmpBA3,sizeof(char),24))),_tag_dyneither(_tmpBA2,sizeof(void*),0)));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct
_tuple10*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){struct _tuple10*_tmpBA4;return(
_tmpBA4=_cycalloc(sizeof(*_tmpBA4)),((_tmpBA4->f1=Cyc_Toc_mt_tq,((_tmpBA4->f2=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmpBA4)))));}
static struct _tuple16*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){Cyc_Toc_exp_to_c(nv,e);{struct _tuple16*_tmpBA5;return(_tmpBA5=_cycalloc(
sizeof(*_tmpBA5)),((_tmpBA5->f1=0,((_tmpBA5->f2=e,_tmpBA5)))));};}static struct
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
_tmp39D->tl != 0){const char*_tmpBA8;void*_tmpBA7;(_tmpBA7=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBA8="multiple designators in array",
_tag_dyneither(_tmpBA8,sizeof(char),30))),_tag_dyneither(_tmpBA7,sizeof(void*),0)));}{
void*_tmp3A1=(void*)_tmp39D->hd;void*_tmp3A2=_tmp3A1;struct Cyc_Absyn_Exp*_tmp3A4;
_LL180: {struct Cyc_Absyn_ArrayElement_struct*_tmp3A3=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp3A2;if(_tmp3A3->tag != 0)goto _LL182;else{_tmp3A4=_tmp3A3->f1;}}_LL181: Cyc_Toc_exp_to_c(
nv,_tmp3A4);e_index=_tmp3A4;goto _LL17F;_LL182: {struct Cyc_Absyn_FieldName_struct*
_tmp3A5=(struct Cyc_Absyn_FieldName_struct*)_tmp3A2;if(_tmp3A5->tag != 1)goto
_LL17F;}_LL183: {const char*_tmpBAB;void*_tmpBAA;(_tmpBAA=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBAB="field name designators in array",
_tag_dyneither(_tmpBAB,sizeof(char),32))),_tag_dyneither(_tmpBAA,sizeof(void*),0)));}
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
Cyc_Toc_exp_to_c(nv,e1);{struct _RegionHandle _tmp3B5=_new_region("r2");struct
_RegionHandle*r2=& _tmp3B5;_push_region(r2);{struct Cyc_Absyn_Local_b_struct
_tmpBAE;struct Cyc_Absyn_Local_b_struct*_tmpBAD;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
r2,nv,_tmp3B3,Cyc_Absyn_varb_exp(_tmp3B3,(void*)((_tmpBAD=_cycalloc(sizeof(*
_tmpBAD)),((_tmpBAD[0]=((_tmpBAE.tag=4,((_tmpBAE.f1=vd,_tmpBAE)))),_tmpBAD)))),0));
struct _tuple0*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp3B3,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp3B3,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp3B3,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp3B3,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp3B6=e2->r;struct Cyc_List_List*_tmp3B8;
struct Cyc_Absyn_Vardecl*_tmp3BA;struct Cyc_Absyn_Exp*_tmp3BB;struct Cyc_Absyn_Exp*
_tmp3BC;int _tmp3BD;void*_tmp3BF;struct Cyc_List_List*_tmp3C0;_LL18E: {struct Cyc_Absyn_Array_e_struct*
_tmp3B7=(struct Cyc_Absyn_Array_e_struct*)_tmp3B6;if(_tmp3B7->tag != 27)goto _LL190;
else{_tmp3B8=_tmp3B7->f1;}}_LL18F: body=Cyc_Toc_init_array(nv2,lval,_tmp3B8,Cyc_Toc_skip_stmt_dl());
goto _LL18D;_LL190: {struct Cyc_Absyn_Comprehension_e_struct*_tmp3B9=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3B6;if(_tmp3B9->tag != 28)goto _LL192;else{_tmp3BA=_tmp3B9->f1;_tmp3BB=_tmp3B9->f2;
_tmp3BC=_tmp3B9->f3;_tmp3BD=_tmp3B9->f4;}}_LL191: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp3BA,_tmp3BB,_tmp3BC,_tmp3BD,Cyc_Toc_skip_stmt_dl(),0);goto _LL18D;
_LL192: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp3BE=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp3B6;if(_tmp3BE->tag != 30)goto _LL194;else{_tmp3BF=(void*)_tmp3BE->f1;_tmp3C0=
_tmp3BE->f2;}}_LL193: body=Cyc_Toc_init_anon_struct(nv,lval,_tmp3BF,_tmp3C0,Cyc_Toc_skip_stmt_dl());
goto _LL18D;_LL194:;_LL195: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL18D;_LL18D:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp3B4,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
struct Cyc_Absyn_Stmt*_tmp3C1=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp3B3,Cyc_Absyn_uint_typ,0,s2,0),
0),s,0);_npop_handler(0);return _tmp3C1;};};};_pop_region(r2);};}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*
struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){{struct Cyc_List_List*
_tmp3C4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);
for(0;_tmp3C4 != 0;_tmp3C4=_tmp3C4->tl){struct _tuple16 _tmp3C6;struct Cyc_List_List*
_tmp3C7;struct Cyc_Absyn_Exp*_tmp3C8;struct _tuple16*_tmp3C5=(struct _tuple16*)
_tmp3C4->hd;_tmp3C6=*_tmp3C5;_tmp3C7=_tmp3C6.f1;_tmp3C8=_tmp3C6.f2;if(_tmp3C7 == 
0){const char*_tmpBB1;void*_tmpBB0;(_tmpBB0=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB1="empty designator list",
_tag_dyneither(_tmpBB1,sizeof(char),22))),_tag_dyneither(_tmpBB0,sizeof(void*),0)));}
if(_tmp3C7->tl != 0){const char*_tmpBB4;void*_tmpBB3;(_tmpBB3=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB4="too many designators in anonymous struct",
_tag_dyneither(_tmpBB4,sizeof(char),41))),_tag_dyneither(_tmpBB3,sizeof(void*),0)));}{
void*_tmp3CD=(void*)_tmp3C7->hd;struct _dyneither_ptr*_tmp3CF;_LL197: {struct Cyc_Absyn_FieldName_struct*
_tmp3CE=(struct Cyc_Absyn_FieldName_struct*)_tmp3CD;if(_tmp3CE->tag != 1)goto
_LL199;else{_tmp3CF=_tmp3CE->f1;}}_LL198: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(
lhs,_tmp3CF,0);{void*_tmp3D0=_tmp3C8->r;struct Cyc_List_List*_tmp3D2;struct Cyc_Absyn_Vardecl*
_tmp3D4;struct Cyc_Absyn_Exp*_tmp3D5;struct Cyc_Absyn_Exp*_tmp3D6;int _tmp3D7;void*
_tmp3D9;struct Cyc_List_List*_tmp3DA;_LL19C: {struct Cyc_Absyn_Array_e_struct*
_tmp3D1=(struct Cyc_Absyn_Array_e_struct*)_tmp3D0;if(_tmp3D1->tag != 27)goto _LL19E;
else{_tmp3D2=_tmp3D1->f1;}}_LL19D: s=Cyc_Toc_init_array(nv,lval,_tmp3D2,s);goto
_LL19B;_LL19E: {struct Cyc_Absyn_Comprehension_e_struct*_tmp3D3=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3D0;if(_tmp3D3->tag != 28)goto _LL1A0;else{_tmp3D4=_tmp3D3->f1;_tmp3D5=_tmp3D3->f2;
_tmp3D6=_tmp3D3->f3;_tmp3D7=_tmp3D3->f4;}}_LL19F: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp3D4,_tmp3D5,_tmp3D6,_tmp3D7,s,0);goto _LL19B;_LL1A0: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3D8=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3D0;if(_tmp3D8->tag != 30)goto
_LL1A2;else{_tmp3D9=(void*)_tmp3D8->f1;_tmp3DA=_tmp3D8->f2;}}_LL1A1: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3D9,_tmp3DA,s);goto _LL19B;_LL1A2:;_LL1A3: Cyc_Toc_exp_to_c(nv,_tmp3C8);
if(Cyc_Toc_is_poly_field(struct_type,_tmp3CF))_tmp3C8=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp3C8);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp3C8,0),0),s,0);goto _LL19B;_LL19B:;}goto _LL196;}_LL199:;_LL19A: {const char*
_tmpBB7;void*_tmpBB6;(_tmpBB6=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB7="array designator in struct",
_tag_dyneither(_tmpBB7,sizeof(char),27))),_tag_dyneither(_tmpBB6,sizeof(void*),0)));}
_LL196:;};}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct
_RegionHandle _tmp3DD=_new_region("r");struct _RegionHandle*r=& _tmp3DD;
_push_region(r);{struct Cyc_List_List*_tmp3DE=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);void*_tmp3DF=Cyc_Toc_add_tuple_type(
_tmp3DE);struct _tuple0*_tmp3E0=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp3E1=
Cyc_Absyn_var_exp(_tmp3E0,0);struct Cyc_Absyn_Stmt*_tmp3E2=Cyc_Absyn_exp_stmt(
_tmp3E1,0);struct Cyc_Absyn_Exp*(*_tmp3E3)(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp3E4=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp3E4);for(0;_tmp3E4 != 0;(_tmp3E4=_tmp3E4->tl,
-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp3E4->hd;struct Cyc_Absyn_Exp*
lval=_tmp3E3(_tmp3E1,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic  && Cyc_Toc_atomic_typ((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp3E5=e->r;struct
Cyc_List_List*_tmp3E7;struct Cyc_Absyn_Vardecl*_tmp3E9;struct Cyc_Absyn_Exp*
_tmp3EA;struct Cyc_Absyn_Exp*_tmp3EB;int _tmp3EC;_LL1A5: {struct Cyc_Absyn_Array_e_struct*
_tmp3E6=(struct Cyc_Absyn_Array_e_struct*)_tmp3E5;if(_tmp3E6->tag != 27)goto _LL1A7;
else{_tmp3E7=_tmp3E6->f1;}}_LL1A6: _tmp3E2=Cyc_Toc_init_array(nv,lval,_tmp3E7,
_tmp3E2);goto _LL1A4;_LL1A7: {struct Cyc_Absyn_Comprehension_e_struct*_tmp3E8=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmp3E5;if(_tmp3E8->tag != 28)goto _LL1A9;
else{_tmp3E9=_tmp3E8->f1;_tmp3EA=_tmp3E8->f2;_tmp3EB=_tmp3E8->f3;_tmp3EC=_tmp3E8->f4;}}
_LL1A8: _tmp3E2=Cyc_Toc_init_comprehension(nv,lval,_tmp3E9,_tmp3EA,_tmp3EB,
_tmp3EC,_tmp3E2,0);goto _LL1A4;_LL1A9:;_LL1AA: Cyc_Toc_exp_to_c(nv,e);_tmp3E2=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3E3(_tmp3E1,Cyc_Absyn_fieldname(i),0),
e,0),0),_tmp3E2,0);goto _LL1A4;_LL1A4:;};}}{struct Cyc_Absyn_Exp*_tmp3ED=Cyc_Toc_make_struct(
nv,_tmp3E0,_tmp3DF,_tmp3E2,pointer,rgnopt,is_atomic);_npop_handler(0);return
_tmp3ED;};};_pop_region(r);}static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*
ad,struct _dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp3EE=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp3EE != 0;_tmp3EE=_tmp3EE->tl){struct Cyc_Absyn_Aggrfield*
_tmp3EF=(struct Cyc_Absyn_Aggrfield*)_tmp3EE->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp3EF->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}{
struct Cyc_String_pa_struct _tmpBBF;void*_tmpBBE[1];const char*_tmpBBD;void*_tmpBBC;(
_tmpBBC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)((_tmpBBF.tag=0,((_tmpBBF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmpBBE[0]=& _tmpBBF,Cyc_aprintf(((_tmpBBD="get_member_offset %s failed",
_tag_dyneither(_tmpBBD,sizeof(char),28))),_tag_dyneither(_tmpBBE,sizeof(void*),1)))))))),
_tag_dyneither(_tmpBBC,sizeof(void*),0)));};}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,
struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){struct
_tuple0*_tmp3F4=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp3F5=Cyc_Absyn_var_exp(
_tmp3F4,0);struct Cyc_Absyn_Stmt*_tmp3F6=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(
_tmp3F5),0);struct Cyc_Absyn_Exp*(*_tmp3F7)(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
void*_tmp3F8=Cyc_Toc_typ_to_c(struct_type);int is_atomic=1;struct Cyc_List_List*
forall_types;struct Cyc_Absyn_Aggrdecl*ad;struct Cyc_List_List*aggrfields;{void*
_tmp3F9=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp3FB;union
Cyc_Absyn_AggrInfoU _tmp3FC;struct Cyc_List_List*_tmp3FD;_LL1AC: {struct Cyc_Absyn_AggrType_struct*
_tmp3FA=(struct Cyc_Absyn_AggrType_struct*)_tmp3F9;if(_tmp3FA->tag != 11)goto
_LL1AE;else{_tmp3FB=_tmp3FA->f1;_tmp3FC=_tmp3FB.aggr_info;_tmp3FD=_tmp3FB.targs;}}
_LL1AD: ad=Cyc_Absyn_get_known_aggrdecl(_tmp3FC);aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;forall_types=_tmp3FD;goto _LL1AB;_LL1AE:;_LL1AF: {
const char*_tmpBC2;void*_tmpBC1;(_tmpBC1=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC2="init_struct: bad struct type",
_tag_dyneither(_tmpBC2,sizeof(char),29))),_tag_dyneither(_tmpBC1,sizeof(void*),0)));}
_LL1AB:;}{void*orig_typ=Cyc_Toc_typ_to_c(struct_type);void*aggr_typ=orig_typ;if(
exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields
!= 0){struct Cyc_List_List*_tmp400=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp400->tl != 0;_tmp400=_tmp400->tl){;}{struct Cyc_Absyn_Aggrfield*_tmp401=(
struct Cyc_Absyn_Aggrfield*)_tmp400->hd;struct _RegionHandle _tmp402=_new_region("temp");
struct _RegionHandle*temp=& _tmp402;_push_region(temp);{void*cast_type;if(pointer)
cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);else{cast_type=orig_typ;}
_tmp3F6=Cyc_Absyn_exp_stmt(Cyc_Toc_cast_it(cast_type,Cyc_Absyn_copy_exp(_tmp3F5)),
0);{struct Cyc_List_List*_tmp403=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
temp,temp,ad->tvs,forall_types);if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(
temp,_tmp403,_tmp401->type))){struct Cyc_List_List*_tmp404=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,
exist_types);struct Cyc_List_List*_tmp405=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
temp,_tmp403,_tmp404);struct Cyc_List_List*new_fields=0;for(_tmp400=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;_tmp400 != 0;_tmp400=_tmp400->tl){struct Cyc_Absyn_Aggrfield*
_tmp406=(struct Cyc_Absyn_Aggrfield*)_tmp400->hd;struct Cyc_Absyn_Aggrfield*
_tmpBC3;struct Cyc_Absyn_Aggrfield*_tmp407=(_tmpBC3=_cycalloc(sizeof(*_tmpBC3)),((
_tmpBC3->name=_tmp406->name,((_tmpBC3->tq=Cyc_Toc_mt_tq,((_tmpBC3->type=Cyc_Tcutil_rsubstitute(
temp,_tmp405,_tmp406->type),((_tmpBC3->width=_tmp406->width,((_tmpBC3->attributes=
_tmp406->attributes,_tmpBC3)))))))))));struct Cyc_List_List*_tmpBC4;new_fields=((
_tmpBC4=_cycalloc(sizeof(*_tmpBC4)),((_tmpBC4->hd=_tmp407,((_tmpBC4->tl=
new_fields,_tmpBC4))))));}exist_types=0;aggrfields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{struct Cyc_Absyn_AnonAggrType_struct
_tmpBC7;struct Cyc_Absyn_AnonAggrType_struct*_tmpBC6;struct_type=(void*)((_tmpBC6=
_cycalloc(sizeof(*_tmpBC6)),((_tmpBC6[0]=((_tmpBC7.tag=12,((_tmpBC7.f1=ad->kind,((
_tmpBC7.f2=aggrfields,_tmpBC7)))))),_tmpBC6))));}aggr_typ=Cyc_Toc_typ_to_c(
struct_type);}};};_pop_region(temp);};}{int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->tagged;struct _RegionHandle _tmp40C=_new_region("r");
struct _RegionHandle*r=& _tmp40C;_push_region(r);{struct Cyc_List_List*_tmp40D=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(
r,dles);for(0;_tmp40D != 0;_tmp40D=_tmp40D->tl){struct _tuple16 _tmp40F;struct Cyc_List_List*
_tmp410;struct Cyc_Absyn_Exp*_tmp411;struct _tuple16*_tmp40E=(struct _tuple16*)
_tmp40D->hd;_tmp40F=*_tmp40E;_tmp410=_tmp40F.f1;_tmp411=_tmp40F.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp411->topt))->v);
if(_tmp410 == 0){const char*_tmpBCA;void*_tmpBC9;(_tmpBC9=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBCA="empty designator list",
_tag_dyneither(_tmpBCA,sizeof(char),22))),_tag_dyneither(_tmpBC9,sizeof(void*),0)));}
if(_tmp410->tl != 0){struct _tuple0*_tmp414=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp415=Cyc_Absyn_var_exp(_tmp414,0);for(0;_tmp410 != 0;_tmp410=_tmp410->tl){void*
_tmp416=(void*)_tmp410->hd;struct _dyneither_ptr*_tmp418;_LL1B1: {struct Cyc_Absyn_FieldName_struct*
_tmp417=(struct Cyc_Absyn_FieldName_struct*)_tmp416;if(_tmp417->tag != 1)goto
_LL1B3;else{_tmp418=_tmp417->f1;}}_LL1B2: if(Cyc_Toc_is_poly_field(struct_type,
_tmp418))_tmp415=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp415);_tmp3F6=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3F7(_tmp3F5,_tmp418,0),_tmp415,0),0),
_tmp3F6,0);goto _LL1B0;_LL1B3:;_LL1B4: {const char*_tmpBCD;void*_tmpBCC;(_tmpBCC=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBCD="array designator in struct",_tag_dyneither(_tmpBCD,sizeof(char),27))),
_tag_dyneither(_tmpBCC,sizeof(void*),0)));}_LL1B0:;}Cyc_Toc_exp_to_c(nv,_tmp411);
_tmp3F6=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp415,
_tmp411,0),0),_tmp3F6,0);}else{void*_tmp41B=(void*)_tmp410->hd;struct
_dyneither_ptr*_tmp41D;_LL1B6: {struct Cyc_Absyn_FieldName_struct*_tmp41C=(struct
Cyc_Absyn_FieldName_struct*)_tmp41B;if(_tmp41C->tag != 1)goto _LL1B8;else{_tmp41D=
_tmp41C->f1;}}_LL1B7: {struct Cyc_Absyn_Exp*lval=_tmp3F7(_tmp3F5,_tmp41D,0);if(
is_tagged_union){int i=Cyc_Toc_get_member_offset(ad,_tmp41D);struct Cyc_Absyn_Exp*
f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_aggrmember_exp(
lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,
f_tag_exp,0);_tmp3F6=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3F6,0);
lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{void*_tmp41E=_tmp411->r;
struct Cyc_List_List*_tmp420;struct Cyc_Absyn_Vardecl*_tmp422;struct Cyc_Absyn_Exp*
_tmp423;struct Cyc_Absyn_Exp*_tmp424;int _tmp425;void*_tmp427;struct Cyc_List_List*
_tmp428;_LL1BB: {struct Cyc_Absyn_Array_e_struct*_tmp41F=(struct Cyc_Absyn_Array_e_struct*)
_tmp41E;if(_tmp41F->tag != 27)goto _LL1BD;else{_tmp420=_tmp41F->f1;}}_LL1BC:
_tmp3F6=Cyc_Toc_init_array(nv,lval,_tmp420,_tmp3F6);goto _LL1BA;_LL1BD: {struct
Cyc_Absyn_Comprehension_e_struct*_tmp421=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp41E;if(_tmp421->tag != 28)goto _LL1BF;else{_tmp422=_tmp421->f1;_tmp423=_tmp421->f2;
_tmp424=_tmp421->f3;_tmp425=_tmp421->f4;}}_LL1BE: _tmp3F6=Cyc_Toc_init_comprehension(
nv,lval,_tmp422,_tmp423,_tmp424,_tmp425,_tmp3F6,0);goto _LL1BA;_LL1BF: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp426=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp41E;if(_tmp426->tag != 30)goto
_LL1C1;else{_tmp427=(void*)_tmp426->f1;_tmp428=_tmp426->f2;}}_LL1C0: _tmp3F6=Cyc_Toc_init_anon_struct(
nv,lval,_tmp427,_tmp428,_tmp3F6);goto _LL1BA;_LL1C1:;_LL1C2: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp411->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp411);{struct Cyc_Absyn_Aggrfield*_tmp429=Cyc_Absyn_lookup_field(aggrfields,
_tmp41D);if(Cyc_Toc_is_poly_field(struct_type,_tmp41D) && !was_ptr_type)_tmp411=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp411);if(exist_types != 0)_tmp411=
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(
_tmp429))->type),_tmp411);_tmp3F6=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
lval,_tmp411,0),0),_tmp3F6,0);goto _LL1BA;};}_LL1BA:;}goto _LL1B5;}_LL1B8:;_LL1B9: {
const char*_tmpBD0;void*_tmpBCF;(_tmpBCF=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD0="array designator in struct",
_tag_dyneither(_tmpBD0,sizeof(char),27))),_tag_dyneither(_tmpBCF,sizeof(void*),0)));}
_LL1B5:;}}}{struct Cyc_Absyn_Exp*_tmp42C=Cyc_Toc_make_struct(nv,_tmp3F4,aggr_typ,
_tmp3F6,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp42C;};;_pop_region(
r);};};}struct _tuple20{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};static
struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct
_tuple20*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(
e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,
enum Cyc_Absyn_Incrementor incr){struct Cyc_Absyn_Increment_e_struct _tmpBD3;struct
Cyc_Absyn_Increment_e_struct*_tmpBD2;return Cyc_Absyn_new_exp((void*)((_tmpBD2=
_cycalloc(sizeof(*_tmpBD2)),((_tmpBD2[0]=((_tmpBD3.tag=5,((_tmpBD3.f1=e1,((
_tmpBD3.f2=incr,_tmpBD3)))))),_tmpBD2)))),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp42F=e1->r;struct Cyc_Absyn_Stmt*_tmp431;void*_tmp433;struct Cyc_Absyn_Exp*
_tmp434;struct Cyc_Absyn_Exp*_tmp436;struct _dyneither_ptr*_tmp437;int _tmp438;int
_tmp439;_LL1C4: {struct Cyc_Absyn_StmtExp_e_struct*_tmp430=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp42F;if(_tmp430->tag != 37)goto _LL1C6;else{_tmp431=_tmp430->f1;}}_LL1C5: Cyc_Toc_lvalue_assign_stmt(
_tmp431,fs,f,f_env);goto _LL1C3;_LL1C6: {struct Cyc_Absyn_Cast_e_struct*_tmp432=(
struct Cyc_Absyn_Cast_e_struct*)_tmp42F;if(_tmp432->tag != 15)goto _LL1C8;else{
_tmp433=(void*)_tmp432->f1;_tmp434=_tmp432->f2;}}_LL1C7: Cyc_Toc_lvalue_assign(
_tmp434,fs,f,f_env);goto _LL1C3;_LL1C8: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp435=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp42F;if(_tmp435->tag != 22)goto
_LL1CA;else{_tmp436=_tmp435->f1;_tmp437=_tmp435->f2;_tmp438=_tmp435->f3;_tmp439=
_tmp435->f4;}}_LL1C9: e1->r=_tmp436->r;{struct Cyc_List_List*_tmpBD4;Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)((_tmpBD4=_cycalloc(sizeof(*_tmpBD4)),((_tmpBD4->hd=
_tmp437,((_tmpBD4->tl=fs,_tmpBD4)))))),f,f_env);}goto _LL1C3;_LL1CA:;_LL1CB: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;goto _LL1C3;}_LL1C3:;}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env){void*_tmp43B=s->r;struct Cyc_Absyn_Exp*_tmp43D;struct Cyc_Absyn_Decl*
_tmp43F;struct Cyc_Absyn_Stmt*_tmp440;struct Cyc_Absyn_Stmt*_tmp442;_LL1CD: {
struct Cyc_Absyn_Exp_s_struct*_tmp43C=(struct Cyc_Absyn_Exp_s_struct*)_tmp43B;if(
_tmp43C->tag != 1)goto _LL1CF;else{_tmp43D=_tmp43C->f1;}}_LL1CE: Cyc_Toc_lvalue_assign(
_tmp43D,fs,f,f_env);goto _LL1CC;_LL1CF: {struct Cyc_Absyn_Decl_s_struct*_tmp43E=(
struct Cyc_Absyn_Decl_s_struct*)_tmp43B;if(_tmp43E->tag != 12)goto _LL1D1;else{
_tmp43F=_tmp43E->f1;_tmp440=_tmp43E->f2;}}_LL1D0: Cyc_Toc_lvalue_assign_stmt(
_tmp440,fs,f,f_env);goto _LL1CC;_LL1D1: {struct Cyc_Absyn_Seq_s_struct*_tmp441=(
struct Cyc_Absyn_Seq_s_struct*)_tmp43B;if(_tmp441->tag != 2)goto _LL1D3;else{
_tmp442=_tmp441->f2;}}_LL1D2: Cyc_Toc_lvalue_assign_stmt(_tmp442,fs,f,f_env);goto
_LL1CC;_LL1D3:;_LL1D4: {const char*_tmpBD8;void*_tmpBD7[1];struct Cyc_String_pa_struct
_tmpBD6;(_tmpBD6.tag=0,((_tmpBD6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmpBD7[0]=& _tmpBD6,Cyc_Toc_toc_impos(((_tmpBD8="lvalue_assign_stmt: %s",
_tag_dyneither(_tmpBD8,sizeof(char),23))),_tag_dyneither(_tmpBD7,sizeof(void*),1)))))));}
_LL1CC:;}static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);static
struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){void*_tmp446=
e->r;void*_tmp448;void**_tmp449;struct Cyc_Absyn_Exp*_tmp44A;struct Cyc_Absyn_Exp**
_tmp44B;struct Cyc_Absyn_Exp*_tmp44D;struct Cyc_Absyn_Stmt*_tmp44F;_LL1D6: {struct
Cyc_Absyn_Cast_e_struct*_tmp447=(struct Cyc_Absyn_Cast_e_struct*)_tmp446;if(
_tmp447->tag != 15)goto _LL1D8;else{_tmp448=(void**)& _tmp447->f1;_tmp449=(void**)((
void**)& _tmp447->f1);_tmp44A=_tmp447->f2;_tmp44B=(struct Cyc_Absyn_Exp**)& _tmp447->f2;}}
_LL1D7:*_tmp44B=Cyc_Toc_push_address_exp(*_tmp44B);*_tmp449=Cyc_Absyn_cstar_typ(*
_tmp449,Cyc_Toc_mt_tq);return e;_LL1D8: {struct Cyc_Absyn_Deref_e_struct*_tmp44C=(
struct Cyc_Absyn_Deref_e_struct*)_tmp446;if(_tmp44C->tag != 21)goto _LL1DA;else{
_tmp44D=_tmp44C->f1;}}_LL1D9: return _tmp44D;_LL1DA: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp44E=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp446;if(_tmp44E->tag != 37)goto
_LL1DC;else{_tmp44F=_tmp44E->f1;}}_LL1DB: Cyc_Toc_push_address_stmt(_tmp44F);
return e;_LL1DC:;_LL1DD: if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpBDC;void*_tmpBDB[1];struct Cyc_String_pa_struct _tmpBDA;(_tmpBDA.tag=
0,((_tmpBDA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpBDB[0]=& _tmpBDA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpBDC="can't take & of exp %s",_tag_dyneither(_tmpBDC,
sizeof(char),23))),_tag_dyneither(_tmpBDB,sizeof(void*),1)))))));};_LL1D5:;}
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){void*_tmp453=s->r;
struct Cyc_Absyn_Stmt*_tmp455;struct Cyc_Absyn_Stmt*_tmp457;struct Cyc_Absyn_Exp*
_tmp459;struct Cyc_Absyn_Exp**_tmp45A;_LL1DF: {struct Cyc_Absyn_Seq_s_struct*
_tmp454=(struct Cyc_Absyn_Seq_s_struct*)_tmp453;if(_tmp454->tag != 2)goto _LL1E1;
else{_tmp455=_tmp454->f2;}}_LL1E0: _tmp457=_tmp455;goto _LL1E2;_LL1E1: {struct Cyc_Absyn_Decl_s_struct*
_tmp456=(struct Cyc_Absyn_Decl_s_struct*)_tmp453;if(_tmp456->tag != 12)goto _LL1E3;
else{_tmp457=_tmp456->f2;}}_LL1E2: Cyc_Toc_push_address_stmt(_tmp457);goto _LL1DE;
_LL1E3: {struct Cyc_Absyn_Exp_s_struct*_tmp458=(struct Cyc_Absyn_Exp_s_struct*)
_tmp453;if(_tmp458->tag != 1)goto _LL1E5;else{_tmp459=_tmp458->f1;_tmp45A=(struct
Cyc_Absyn_Exp**)& _tmp458->f1;}}_LL1E4:*_tmp45A=Cyc_Toc_push_address_exp(*_tmp45A);
goto _LL1DE;_LL1E5:;_LL1E6: {const char*_tmpBE0;void*_tmpBDF[1];struct Cyc_String_pa_struct
_tmpBDE;(_tmpBDE.tag=0,((_tmpBDE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmpBDF[0]=& _tmpBDE,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE0="can't take & of stmt %s",
_tag_dyneither(_tmpBE0,sizeof(char),24))),_tag_dyneither(_tmpBDF,sizeof(void*),1)))))));}
_LL1DE:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;
struct Cyc_List_List*prev;if(x == 0)return 0;{struct Cyc_List_List*_tmpBE1;result=((
_tmpBE1=_region_malloc(r2,sizeof(*_tmpBE1)),((_tmpBE1->hd=(void*)f((void*)x->hd,
env),((_tmpBE1->tl=0,_tmpBE1))))));}prev=result;for(x=x->tl;x != 0;x=x->tl){{
struct Cyc_List_List*_tmpBE2;((struct Cyc_List_List*)_check_null(prev))->tl=((
_tmpBE2=_region_malloc(r2,sizeof(*_tmpBE2)),((_tmpBE2->hd=(void*)f((void*)x->hd,
env),((_tmpBE2->tl=0,_tmpBE2))))));}prev=prev->tl;}return result;}static struct Cyc_List_List*
Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region,f,env,x);}static struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*
e){struct _tuple16*_tmpBE3;return(_tmpBE3=_cycalloc(sizeof(*_tmpBE3)),((_tmpBE3->f1=
0,((_tmpBE3->f2=e,_tmpBE3)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(
void*t){void*_tmp461=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp463;
_LL1E8: {struct Cyc_Absyn_PointerType_struct*_tmp462=(struct Cyc_Absyn_PointerType_struct*)
_tmp461;if(_tmp462->tag != 5)goto _LL1EA;else{_tmp463=_tmp462->f1;}}_LL1E9: return
_tmp463;_LL1EA:;_LL1EB: {const char*_tmpBE6;void*_tmpBE5;(_tmpBE5=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE6="get_ptr_typ: not a pointer!",
_tag_dyneither(_tmpBE6,sizeof(char),28))),_tag_dyneither(_tmpBE5,sizeof(void*),0)));}
_LL1E7:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp466=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp469;enum Cyc_Absyn_Size_of
_tmp46A;enum Cyc_Absyn_Sign _tmp46C;enum Cyc_Absyn_Size_of _tmp46D;enum Cyc_Absyn_Sign
_tmp471;enum Cyc_Absyn_Size_of _tmp472;enum Cyc_Absyn_Sign _tmp474;enum Cyc_Absyn_Size_of
_tmp475;enum Cyc_Absyn_Sign _tmp477;enum Cyc_Absyn_Size_of _tmp478;int _tmp47A;int
_tmp47C;int _tmp47E;_LL1ED: {struct Cyc_Absyn_PointerType_struct*_tmp467=(struct
Cyc_Absyn_PointerType_struct*)_tmp466;if(_tmp467->tag != 5)goto _LL1EF;}_LL1EE: res=
Cyc_Absyn_null_exp(0);goto _LL1EC;_LL1EF: {struct Cyc_Absyn_IntType_struct*_tmp468=(
struct Cyc_Absyn_IntType_struct*)_tmp466;if(_tmp468->tag != 6)goto _LL1F1;else{
_tmp469=_tmp468->f1;_tmp46A=_tmp468->f2;if(_tmp46A != Cyc_Absyn_Char_sz)goto
_LL1F1;}}_LL1F0: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp469,'\000'),0);goto
_LL1EC;_LL1F1: {struct Cyc_Absyn_IntType_struct*_tmp46B=(struct Cyc_Absyn_IntType_struct*)
_tmp466;if(_tmp46B->tag != 6)goto _LL1F3;else{_tmp46C=_tmp46B->f1;_tmp46D=_tmp46B->f2;
if(_tmp46D != Cyc_Absyn_Short_sz)goto _LL1F3;}}_LL1F2: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(
_tmp46C,0),0);goto _LL1EC;_LL1F3: {struct Cyc_Absyn_EnumType_struct*_tmp46E=(
struct Cyc_Absyn_EnumType_struct*)_tmp466;if(_tmp46E->tag != 13)goto _LL1F5;}_LL1F4:
goto _LL1F6;_LL1F5: {struct Cyc_Absyn_AnonEnumType_struct*_tmp46F=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp466;if(_tmp46F->tag != 14)goto _LL1F7;}_LL1F6: _tmp471=Cyc_Absyn_Unsigned;goto
_LL1F8;_LL1F7: {struct Cyc_Absyn_IntType_struct*_tmp470=(struct Cyc_Absyn_IntType_struct*)
_tmp466;if(_tmp470->tag != 6)goto _LL1F9;else{_tmp471=_tmp470->f1;_tmp472=_tmp470->f2;
if(_tmp472 != Cyc_Absyn_Int_sz)goto _LL1F9;}}_LL1F8: _tmp474=_tmp471;goto _LL1FA;
_LL1F9: {struct Cyc_Absyn_IntType_struct*_tmp473=(struct Cyc_Absyn_IntType_struct*)
_tmp466;if(_tmp473->tag != 6)goto _LL1FB;else{_tmp474=_tmp473->f1;_tmp475=_tmp473->f2;
if(_tmp475 != Cyc_Absyn_Long_sz)goto _LL1FB;}}_LL1FA: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(
_tmp474,0),0);goto _LL1EC;_LL1FB: {struct Cyc_Absyn_IntType_struct*_tmp476=(struct
Cyc_Absyn_IntType_struct*)_tmp466;if(_tmp476->tag != 6)goto _LL1FD;else{_tmp477=
_tmp476->f1;_tmp478=_tmp476->f2;if(_tmp478 != Cyc_Absyn_LongLong_sz)goto _LL1FD;}}
_LL1FC: res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp477,(long long)0),0);
goto _LL1EC;_LL1FD: {struct Cyc_Absyn_FloatType_struct*_tmp479=(struct Cyc_Absyn_FloatType_struct*)
_tmp466;if(_tmp479->tag != 7)goto _LL1FF;else{_tmp47A=_tmp479->f1;if(_tmp47A != 0)
goto _LL1FF;}}_LL1FE:{const char*_tmpBE7;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((
_tmpBE7="0.0F",_tag_dyneither(_tmpBE7,sizeof(char),5))),0),0);}goto _LL1EC;_LL1FF: {
struct Cyc_Absyn_FloatType_struct*_tmp47B=(struct Cyc_Absyn_FloatType_struct*)
_tmp466;if(_tmp47B->tag != 7)goto _LL201;else{_tmp47C=_tmp47B->f1;if(_tmp47C != 1)
goto _LL201;}}_LL200:{const char*_tmpBE8;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((
_tmpBE8="0.0",_tag_dyneither(_tmpBE8,sizeof(char),4))),1),0);}goto _LL1EC;_LL201: {
struct Cyc_Absyn_FloatType_struct*_tmp47D=(struct Cyc_Absyn_FloatType_struct*)
_tmp466;if(_tmp47D->tag != 7)goto _LL203;else{_tmp47E=_tmp47D->f1;}}_LL202:{const
char*_tmpBE9;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBE9="0.0L",
_tag_dyneither(_tmpBE9,sizeof(char),5))),_tmp47E),0);}goto _LL1EC;_LL203:;_LL204: {
const char*_tmpBED;void*_tmpBEC[1];struct Cyc_String_pa_struct _tmpBEB;(_tmpBEB.tag=
0,((_tmpBEB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmpBEC[0]=& _tmpBEB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpBED="found non-zero type %s in generate_zero",
_tag_dyneither(_tmpBED,sizeof(char),40))),_tag_dyneither(_tmpBEC,sizeof(void*),1)))))));}
_LL1EC:;}{struct Cyc_Core_Opt*_tmpBEE;res->topt=((_tmpBEE=_cycalloc(sizeof(*
_tmpBEE)),((_tmpBEE->v=t,_tmpBEE))));}return res;}static void Cyc_Toc_zero_ptr_assign_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){void*
fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,
Cyc_Absyn_true_conref);void*_tmp486=Cyc_Toc_typ_to_c(elt_type);void*_tmp487=Cyc_Toc_typ_to_c(
fat_ptr_type);void*_tmp488=Cyc_Absyn_cstar_typ(_tmp486,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmpBEF;struct Cyc_Core_Opt*_tmp489=(_tmpBEF=_cycalloc(sizeof(*_tmpBEF)),((
_tmpBEF->v=_tmp488,_tmpBEF)));struct Cyc_Absyn_Exp*xinit;{void*_tmp48A=e1->r;
struct Cyc_Absyn_Exp*_tmp48C;struct Cyc_Absyn_Exp*_tmp48E;struct Cyc_Absyn_Exp*
_tmp48F;_LL206: {struct Cyc_Absyn_Deref_e_struct*_tmp48B=(struct Cyc_Absyn_Deref_e_struct*)
_tmp48A;if(_tmp48B->tag != 21)goto _LL208;else{_tmp48C=_tmp48B->f1;}}_LL207: if(!
is_dyneither){_tmp48C=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp48C,0,Cyc_Absyn_Other_coercion,
0);{struct Cyc_Core_Opt*_tmpBF0;_tmp48C->topt=((_tmpBF0=_cycalloc(sizeof(*_tmpBF0)),((
_tmpBF0->v=fat_ptr_type,_tmpBF0))));};}Cyc_Toc_exp_to_c(nv,_tmp48C);xinit=
_tmp48C;goto _LL205;_LL208: {struct Cyc_Absyn_Subscript_e_struct*_tmp48D=(struct
Cyc_Absyn_Subscript_e_struct*)_tmp48A;if(_tmp48D->tag != 24)goto _LL20A;else{
_tmp48E=_tmp48D->f1;_tmp48F=_tmp48D->f2;}}_LL209: if(!is_dyneither){_tmp48E=Cyc_Absyn_cast_exp(
fat_ptr_type,_tmp48E,0,Cyc_Absyn_Other_coercion,0);{struct Cyc_Core_Opt*_tmpBF1;
_tmp48E->topt=((_tmpBF1=_cycalloc(sizeof(*_tmpBF1)),((_tmpBF1->v=fat_ptr_type,
_tmpBF1))));};}Cyc_Toc_exp_to_c(nv,_tmp48E);Cyc_Toc_exp_to_c(nv,_tmp48F);{struct
Cyc_Absyn_Exp*_tmpBF2[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,((
_tmpBF2[2]=_tmp48F,((_tmpBF2[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmpBF2[0]=_tmp48E,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpBF2,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL205;_LL20A:;_LL20B: {const char*_tmpBF5;void*_tmpBF4;(_tmpBF4=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF5="found bad lhs for zero-terminated pointer assignment",
_tag_dyneither(_tmpBF5,sizeof(char),53))),_tag_dyneither(_tmpBF4,sizeof(void*),0)));}
_LL205:;}{struct _tuple0*_tmp495=Cyc_Toc_temp_var();struct _RegionHandle _tmp496=
_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp496;_push_region(rgn2);{struct
Cyc_Toc_Env*_tmp497=Cyc_Toc_add_varmap(rgn2,nv,_tmp495,Cyc_Absyn_var_exp(_tmp495,
0));struct Cyc_Absyn_Vardecl*_tmpBF6;struct Cyc_Absyn_Vardecl*_tmp498=(_tmpBF6=
_cycalloc(sizeof(*_tmpBF6)),((_tmpBF6->sc=Cyc_Absyn_Public,((_tmpBF6->name=
_tmp495,((_tmpBF6->tq=Cyc_Toc_mt_tq,((_tmpBF6->type=_tmp487,((_tmpBF6->initializer=(
struct Cyc_Absyn_Exp*)xinit,((_tmpBF6->rgn=0,((_tmpBF6->attributes=0,((_tmpBF6->escapes=
0,_tmpBF6)))))))))))))))));struct Cyc_Absyn_Local_b_struct _tmpBF9;struct Cyc_Absyn_Local_b_struct*
_tmpBF8;struct Cyc_Absyn_Local_b_struct*_tmp499=(_tmpBF8=_cycalloc(sizeof(*
_tmpBF8)),((_tmpBF8[0]=((_tmpBF9.tag=4,((_tmpBF9.f1=_tmp498,_tmpBF9)))),_tmpBF8)));
struct Cyc_Absyn_Exp*_tmp49A=Cyc_Absyn_varb_exp(_tmp495,(void*)_tmp499,0);{struct
Cyc_Core_Opt*_tmpBFA;_tmp49A->topt=((_tmpBFA=_cycalloc(sizeof(*_tmpBFA)),((
_tmpBFA->v=fat_ptr_type,_tmpBFA))));}{struct Cyc_Absyn_Exp*_tmp49C=Cyc_Absyn_deref_exp(
_tmp49A,0);{struct Cyc_Core_Opt*_tmpBFB;_tmp49C->topt=((_tmpBFB=_cycalloc(sizeof(*
_tmpBFB)),((_tmpBFB->v=elt_type,_tmpBFB))));}Cyc_Toc_exp_to_c(_tmp497,_tmp49C);{
struct _tuple0*_tmp49E=Cyc_Toc_temp_var();_tmp497=Cyc_Toc_add_varmap(rgn2,_tmp497,
_tmp49E,Cyc_Absyn_var_exp(_tmp49E,0));{struct Cyc_Absyn_Vardecl*_tmpBFC;struct Cyc_Absyn_Vardecl*
_tmp49F=(_tmpBFC=_cycalloc(sizeof(*_tmpBFC)),((_tmpBFC->sc=Cyc_Absyn_Public,((
_tmpBFC->name=_tmp49E,((_tmpBFC->tq=Cyc_Toc_mt_tq,((_tmpBFC->type=_tmp486,((
_tmpBFC->initializer=(struct Cyc_Absyn_Exp*)_tmp49C,((_tmpBFC->rgn=0,((_tmpBFC->attributes=
0,((_tmpBFC->escapes=0,_tmpBFC)))))))))))))))));struct Cyc_Absyn_Local_b_struct
_tmpBFF;struct Cyc_Absyn_Local_b_struct*_tmpBFE;struct Cyc_Absyn_Local_b_struct*
_tmp4A0=(_tmpBFE=_cycalloc(sizeof(*_tmpBFE)),((_tmpBFE[0]=((_tmpBFF.tag=4,((
_tmpBFF.f1=_tmp49F,_tmpBFF)))),_tmpBFE)));struct Cyc_Absyn_Exp*z_init=e2;if(popt
!= 0){struct Cyc_Absyn_Exp*_tmp4A1=Cyc_Absyn_varb_exp(_tmp49E,(void*)_tmp4A0,0);
_tmp4A1->topt=_tmp49C->topt;z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)
popt->v,_tmp4A1,e2,0);z_init->topt=_tmp4A1->topt;}Cyc_Toc_exp_to_c(_tmp497,
z_init);{struct _tuple0*_tmp4A2=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmpC00;struct Cyc_Absyn_Vardecl*_tmp4A3=(_tmpC00=_cycalloc(sizeof(*_tmpC00)),((
_tmpC00->sc=Cyc_Absyn_Public,((_tmpC00->name=_tmp4A2,((_tmpC00->tq=Cyc_Toc_mt_tq,((
_tmpC00->type=_tmp486,((_tmpC00->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmpC00->rgn=0,((_tmpC00->attributes=0,((_tmpC00->escapes=0,_tmpC00)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpC03;struct Cyc_Absyn_Local_b_struct*_tmpC02;
struct Cyc_Absyn_Local_b_struct*_tmp4A4=(_tmpC02=_cycalloc(sizeof(*_tmpC02)),((
_tmpC02[0]=((_tmpC03.tag=4,((_tmpC03.f1=_tmp4A3,_tmpC03)))),_tmpC02)));_tmp497=
Cyc_Toc_add_varmap(rgn2,_tmp497,_tmp4A2,Cyc_Absyn_var_exp(_tmp4A2,0));{struct Cyc_Absyn_Exp*
_tmp4A5=Cyc_Absyn_varb_exp(_tmp49E,(void*)_tmp4A0,0);_tmp4A5->topt=_tmp49C->topt;{
struct Cyc_Absyn_Exp*_tmp4A6=Cyc_Toc_generate_zero(elt_type);struct Cyc_Absyn_Exp*
_tmp4A7=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp4A5,_tmp4A6,0);{struct Cyc_Core_Opt*
_tmpC04;_tmp4A7->topt=((_tmpC04=_cycalloc(sizeof(*_tmpC04)),((_tmpC04->v=Cyc_Absyn_sint_typ,
_tmpC04))));}Cyc_Toc_exp_to_c(_tmp497,_tmp4A7);{struct Cyc_Absyn_Exp*_tmp4A9=Cyc_Absyn_varb_exp(
_tmp4A2,(void*)_tmp4A4,0);_tmp4A9->topt=_tmp49C->topt;{struct Cyc_Absyn_Exp*
_tmp4AA=Cyc_Toc_generate_zero(elt_type);struct Cyc_Absyn_Exp*_tmp4AB=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Neq,_tmp4A9,_tmp4AA,0);{struct Cyc_Core_Opt*_tmpC05;_tmp4AB->topt=((
_tmpC05=_cycalloc(sizeof(*_tmpC05)),((_tmpC05->v=Cyc_Absyn_sint_typ,_tmpC05))));}
Cyc_Toc_exp_to_c(_tmp497,_tmp4AB);{struct Cyc_Absyn_Exp*_tmpC06[2];struct Cyc_List_List*
_tmp4AD=(_tmpC06[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((
_tmpC06[0]=Cyc_Absyn_varb_exp(_tmp495,(void*)_tmp499,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC06,sizeof(struct Cyc_Absyn_Exp*),
2)))));struct Cyc_Absyn_Exp*_tmp4AE=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*
xsize;xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,
_tmp4AD,0),_tmp4AE,0);{struct Cyc_Absyn_Exp*_tmp4AF=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp4A7,_tmp4AB,0),0);struct Cyc_Absyn_Stmt*_tmp4B0=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp4B1=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp495,(void*)_tmp499,0),Cyc_Toc_curr_sp,0);_tmp4B1=Cyc_Toc_cast_it(
_tmp488,_tmp4B1);{struct Cyc_Absyn_Exp*_tmp4B2=Cyc_Absyn_deref_exp(_tmp4B1,0);
struct Cyc_Absyn_Exp*_tmp4B3=Cyc_Absyn_assign_exp(_tmp4B2,Cyc_Absyn_var_exp(
_tmp4A2,0),0);struct Cyc_Absyn_Stmt*_tmp4B4=Cyc_Absyn_exp_stmt(_tmp4B3,0);_tmp4B4=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp4AF,_tmp4B0,Cyc_Absyn_skip_stmt(
0),0),_tmp4B4,0);{struct Cyc_Absyn_Var_d_struct*_tmpC0C;struct Cyc_Absyn_Var_d_struct
_tmpC0B;struct Cyc_Absyn_Decl*_tmpC0A;_tmp4B4=Cyc_Absyn_decl_stmt(((_tmpC0A=
_cycalloc(sizeof(*_tmpC0A)),((_tmpC0A->r=(void*)((_tmpC0C=_cycalloc(sizeof(*
_tmpC0C)),((_tmpC0C[0]=((_tmpC0B.tag=0,((_tmpC0B.f1=_tmp4A3,_tmpC0B)))),_tmpC0C)))),((
_tmpC0A->loc=0,_tmpC0A)))))),_tmp4B4,0);}{struct Cyc_Absyn_Var_d_struct*_tmpC12;
struct Cyc_Absyn_Var_d_struct _tmpC11;struct Cyc_Absyn_Decl*_tmpC10;_tmp4B4=Cyc_Absyn_decl_stmt(((
_tmpC10=_cycalloc(sizeof(*_tmpC10)),((_tmpC10->r=(void*)((_tmpC12=_cycalloc(
sizeof(*_tmpC12)),((_tmpC12[0]=((_tmpC11.tag=0,((_tmpC11.f1=_tmp49F,_tmpC11)))),
_tmpC12)))),((_tmpC10->loc=0,_tmpC10)))))),_tmp4B4,0);}{struct Cyc_Absyn_Var_d_struct*
_tmpC18;struct Cyc_Absyn_Var_d_struct _tmpC17;struct Cyc_Absyn_Decl*_tmpC16;_tmp4B4=
Cyc_Absyn_decl_stmt(((_tmpC16=_cycalloc(sizeof(*_tmpC16)),((_tmpC16->r=(void*)((
_tmpC18=_cycalloc(sizeof(*_tmpC18)),((_tmpC18[0]=((_tmpC17.tag=0,((_tmpC17.f1=
_tmp498,_tmpC17)))),_tmpC18)))),((_tmpC16->loc=0,_tmpC16)))))),_tmp4B4,0);}e->r=
Cyc_Toc_stmt_exp_r(_tmp4B4);};};};};};};};};};};};};_pop_region(rgn2);};}static
void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*
aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct
Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){struct Cyc_Absyn_Aggrdecl*ad;{
void*_tmp4C9=Cyc_Tcutil_compress(aggrtype);struct Cyc_Absyn_AggrInfo _tmp4CB;union
Cyc_Absyn_AggrInfoU _tmp4CC;_LL20D: {struct Cyc_Absyn_AggrType_struct*_tmp4CA=(
struct Cyc_Absyn_AggrType_struct*)_tmp4C9;if(_tmp4CA->tag != 11)goto _LL20F;else{
_tmp4CB=_tmp4CA->f1;_tmp4CC=_tmp4CB.aggr_info;}}_LL20E: ad=Cyc_Absyn_get_known_aggrdecl(
_tmp4CC);goto _LL20C;_LL20F:;_LL210: {struct Cyc_String_pa_struct _tmpC20;void*
_tmpC1F[1];const char*_tmpC1E;void*_tmpC1D;(_tmpC1D=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct
_dyneither_ptr)((_tmpC20.tag=0,((_tmpC20.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpC1F[0]=& _tmpC20,Cyc_aprintf(((
_tmpC1E="expecting union but found %s in check_tagged_union",_tag_dyneither(
_tmpC1E,sizeof(char),51))),_tag_dyneither(_tmpC1F,sizeof(void*),1)))))))),
_tag_dyneither(_tmpC1D,sizeof(void*),0)));}_LL20C:;}{struct _tuple0*_tmp4D1=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4D2=Cyc_Absyn_var_exp(_tmp4D1,0);struct Cyc_Absyn_Exp*
_tmp4D3=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);if(in_lhs){
struct Cyc_Absyn_Exp*_tmp4D4=Cyc_Absyn_aggrarrow_exp(_tmp4D2,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4D5=Cyc_Absyn_neq_exp(_tmp4D4,_tmp4D3,0);struct Cyc_Absyn_Exp*
_tmp4D6=Cyc_Absyn_aggrarrow_exp(_tmp4D2,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Stmt*
_tmp4D7=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp4D6,0),0);struct Cyc_Absyn_Stmt*
_tmp4D8=Cyc_Absyn_ifthenelse_stmt(_tmp4D5,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);void*_tmp4D9=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp4DA=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);struct Cyc_Absyn_Stmt*_tmp4DB=
Cyc_Absyn_declare_stmt(_tmp4D1,_tmp4D9,(struct Cyc_Absyn_Exp*)_tmp4DA,Cyc_Absyn_seq_stmt(
_tmp4D8,_tmp4D7,0),0);return Cyc_Toc_stmt_exp_r(_tmp4DB);}else{struct Cyc_Absyn_Exp*
_tmp4DC=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp4D2,f,0),Cyc_Toc_tag_sp,0);struct
Cyc_Absyn_Exp*_tmp4DD=Cyc_Absyn_neq_exp(_tmp4DC,_tmp4D3,0);struct Cyc_Absyn_Exp*
_tmp4DE=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp4D2,f,0),Cyc_Toc_val_sp,0);struct
Cyc_Absyn_Stmt*_tmp4DF=Cyc_Absyn_exp_stmt(_tmp4DE,0);struct Cyc_Absyn_Stmt*
_tmp4E0=Cyc_Absyn_ifthenelse_stmt(_tmp4DD,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp4E1=Cyc_Absyn_declare_stmt(_tmp4D1,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp4E0,_tmp4DF,0),0);return Cyc_Toc_stmt_exp_r(
_tmp4E1);}};}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read){void*_tmp4E2=e->r;struct Cyc_Absyn_Exp*
_tmp4E4;struct Cyc_Absyn_Exp*_tmp4E6;struct _dyneither_ptr*_tmp4E7;int _tmp4E8;int*
_tmp4E9;struct Cyc_Absyn_Exp*_tmp4EB;struct _dyneither_ptr*_tmp4EC;int _tmp4ED;int*
_tmp4EE;_LL212: {struct Cyc_Absyn_Cast_e_struct*_tmp4E3=(struct Cyc_Absyn_Cast_e_struct*)
_tmp4E2;if(_tmp4E3->tag != 15)goto _LL214;else{_tmp4E4=_tmp4E3->f2;}}_LL213: {
const char*_tmpC23;void*_tmpC22;(_tmpC22=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC23="cast on lhs!",
_tag_dyneither(_tmpC23,sizeof(char),13))),_tag_dyneither(_tmpC22,sizeof(void*),0)));}
_LL214: {struct Cyc_Absyn_AggrMember_e_struct*_tmp4E5=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp4E2;if(_tmp4E5->tag != 22)goto _LL216;else{_tmp4E6=_tmp4E5->f1;_tmp4E7=_tmp4E5->f2;
_tmp4E8=_tmp4E5->f4;_tmp4E9=(int*)& _tmp4E5->f4;}}_LL215: {void*_tmp4F1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4E6->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp4F3;union Cyc_Absyn_AggrInfoU _tmp4F4;_LL21B: {struct Cyc_Absyn_AggrType_struct*
_tmp4F2=(struct Cyc_Absyn_AggrType_struct*)_tmp4F1;if(_tmp4F2->tag != 11)goto
_LL21D;else{_tmp4F3=_tmp4F2->f1;_tmp4F4=_tmp4F3.aggr_info;}}_LL21C: {struct Cyc_Absyn_Aggrdecl*
_tmp4F5=Cyc_Absyn_get_known_aggrdecl(_tmp4F4);*f_tag=Cyc_Toc_get_member_offset(
_tmp4F5,_tmp4E7);{const char*_tmpC28;void*_tmpC27[2];struct Cyc_String_pa_struct
_tmpC26;struct Cyc_String_pa_struct _tmpC25;struct _dyneither_ptr str=(struct
_dyneither_ptr)((_tmpC25.tag=0,((_tmpC25.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp4E7),((_tmpC26.tag=0,((_tmpC26.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp4F5->name).f2),((_tmpC27[0]=& _tmpC26,((_tmpC27[1]=&
_tmpC25,Cyc_aprintf(((_tmpC28="_union_%s_%s",_tag_dyneither(_tmpC28,sizeof(char),
13))),_tag_dyneither(_tmpC27,sizeof(void*),2))))))))))))));{struct _dyneither_ptr*
_tmpC29;*tagged_member_type=Cyc_Absyn_strct(((_tmpC29=_cycalloc(sizeof(*_tmpC29)),((
_tmpC29[0]=str,_tmpC29)))));}if(clear_read)*_tmp4E9=0;return((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4F5->impl))->tagged;};}_LL21D:;_LL21E: return 0;_LL21A:;}_LL216: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp4EA=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp4E2;if(_tmp4EA->tag != 23)goto _LL218;else{_tmp4EB=_tmp4EA->f1;_tmp4EC=_tmp4EA->f2;
_tmp4ED=_tmp4EA->f4;_tmp4EE=(int*)& _tmp4EA->f4;}}_LL217: {void*_tmp4FB=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4EB->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp4FD;void*_tmp4FE;_LL220: {struct Cyc_Absyn_PointerType_struct*_tmp4FC=(struct
Cyc_Absyn_PointerType_struct*)_tmp4FB;if(_tmp4FC->tag != 5)goto _LL222;else{
_tmp4FD=_tmp4FC->f1;_tmp4FE=_tmp4FD.elt_typ;}}_LL221: {void*_tmp4FF=Cyc_Tcutil_compress(
_tmp4FE);struct Cyc_Absyn_AggrInfo _tmp501;union Cyc_Absyn_AggrInfoU _tmp502;_LL225: {
struct Cyc_Absyn_AggrType_struct*_tmp500=(struct Cyc_Absyn_AggrType_struct*)
_tmp4FF;if(_tmp500->tag != 11)goto _LL227;else{_tmp501=_tmp500->f1;_tmp502=_tmp501.aggr_info;}}
_LL226: {struct Cyc_Absyn_Aggrdecl*_tmp503=Cyc_Absyn_get_known_aggrdecl(_tmp502);*
f_tag=Cyc_Toc_get_member_offset(_tmp503,_tmp4EC);{const char*_tmpC2E;void*_tmpC2D[
2];struct Cyc_String_pa_struct _tmpC2C;struct Cyc_String_pa_struct _tmpC2B;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpC2B.tag=0,((_tmpC2B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp4EC),((_tmpC2C.tag=0,((_tmpC2C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp503->name).f2),((_tmpC2D[0]=&
_tmpC2C,((_tmpC2D[1]=& _tmpC2B,Cyc_aprintf(((_tmpC2E="_union_%s_%s",
_tag_dyneither(_tmpC2E,sizeof(char),13))),_tag_dyneither(_tmpC2D,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpC2F;*tagged_member_type=Cyc_Absyn_strct(((_tmpC2F=
_cycalloc(sizeof(*_tmpC2F)),((_tmpC2F[0]=str,_tmpC2F)))));}if(clear_read)*
_tmp4EE=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp503->impl))->tagged;};}
_LL227:;_LL228: return 0;_LL224:;}_LL222:;_LL223: return 0;_LL21F:;}_LL218:;_LL219:
return 0;_LL211:;}void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*
e,int tag,void*mem_type){struct _tuple0*_tmp509=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
temp_exp=Cyc_Absyn_var_exp(_tmp509,0);struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(
tag,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,
0),0);struct Cyc_Absyn_Exp*_tmp50A=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_ifthenelse_stmt(_tmp50A,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp509,Cyc_Absyn_cstar_typ(
mem_type,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(
e)),Cyc_Absyn_seq_stmt(s2,s3,0),0);e->r=Cyc_Toc_stmt_exp_r(s1);}static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type){struct _tuple0*_tmp50B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp50B,0);struct Cyc_Absyn_Exp*
temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*
temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 
0)s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*
_tmp50C=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp50C,
Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=
Cyc_Absyn_declare_stmt(_tmp50B,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(e1),Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple21{struct _tuple0*f1;void*f2;struct
Cyc_Absyn_Exp*f3;};struct _tuple22{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*
f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void _tmpC82(unsigned int*_tmpC81,unsigned int*_tmpC80,struct _tuple0***
_tmpC7E){for(*_tmpC81=0;*_tmpC81 < *_tmpC80;(*_tmpC81)++){(*_tmpC7E)[*_tmpC81]=
Cyc_Toc_temp_var();}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){void*_tmp50D=e->r;if(e->topt == 0){const char*_tmpC33;void*_tmpC32[1];struct Cyc_String_pa_struct
_tmpC31;(_tmpC31.tag=0,((_tmpC31.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpC32[0]=& _tmpC31,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC33="exp_to_c: no type for %s",
_tag_dyneither(_tmpC33,sizeof(char),25))),_tag_dyneither(_tmpC32,sizeof(void*),1)))))));}{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp511=
_tmp50D;union Cyc_Absyn_Cnst _tmp513;int _tmp514;struct _tuple0*_tmp517;void*_tmp518;
struct _tuple0*_tmp51A;enum Cyc_Absyn_Primop _tmp51C;struct Cyc_List_List*_tmp51D;
struct Cyc_Absyn_Exp*_tmp51F;enum Cyc_Absyn_Incrementor _tmp520;struct Cyc_Absyn_Exp*
_tmp522;struct Cyc_Core_Opt*_tmp523;struct Cyc_Absyn_Exp*_tmp524;struct Cyc_Absyn_Exp*
_tmp526;struct Cyc_Absyn_Exp*_tmp527;struct Cyc_Absyn_Exp*_tmp528;struct Cyc_Absyn_Exp*
_tmp52A;struct Cyc_Absyn_Exp*_tmp52B;struct Cyc_Absyn_Exp*_tmp52D;struct Cyc_Absyn_Exp*
_tmp52E;struct Cyc_Absyn_Exp*_tmp530;struct Cyc_Absyn_Exp*_tmp531;struct Cyc_Absyn_Exp*
_tmp533;struct Cyc_List_List*_tmp534;struct Cyc_Absyn_Exp*_tmp536;struct Cyc_List_List*
_tmp537;struct Cyc_Absyn_VarargCallInfo*_tmp538;struct Cyc_Absyn_Exp*_tmp53A;
struct Cyc_List_List*_tmp53B;struct Cyc_Absyn_VarargCallInfo*_tmp53C;struct Cyc_Absyn_VarargCallInfo
_tmp53D;int _tmp53E;struct Cyc_List_List*_tmp53F;struct Cyc_Absyn_VarargInfo*
_tmp540;struct Cyc_Absyn_Exp*_tmp542;struct Cyc_Absyn_Exp*_tmp544;struct Cyc_Absyn_Exp*
_tmp546;struct Cyc_List_List*_tmp547;void*_tmp549;void**_tmp54A;struct Cyc_Absyn_Exp*
_tmp54B;int _tmp54C;enum Cyc_Absyn_Coercion _tmp54D;struct Cyc_Absyn_Exp*_tmp54F;
struct Cyc_Absyn_Exp*_tmp551;struct Cyc_Absyn_Exp*_tmp552;struct Cyc_Absyn_Exp*
_tmp554;void*_tmp556;void*_tmp558;void*_tmp559;struct _dyneither_ptr*_tmp55B;void*
_tmp55D;void*_tmp55E;unsigned int _tmp560;struct Cyc_Absyn_Exp*_tmp562;struct Cyc_Absyn_Exp*
_tmp564;struct _dyneither_ptr*_tmp565;int _tmp566;int _tmp567;struct Cyc_Absyn_Exp*
_tmp569;struct _dyneither_ptr*_tmp56A;int _tmp56B;int _tmp56C;struct Cyc_Absyn_Exp*
_tmp56E;struct Cyc_Absyn_Exp*_tmp56F;struct Cyc_List_List*_tmp571;struct Cyc_List_List*
_tmp573;struct Cyc_Absyn_Vardecl*_tmp575;struct Cyc_Absyn_Exp*_tmp576;struct Cyc_Absyn_Exp*
_tmp577;int _tmp578;struct _tuple0*_tmp57A;struct Cyc_List_List*_tmp57B;struct Cyc_List_List*
_tmp57C;struct Cyc_Absyn_Aggrdecl*_tmp57D;void*_tmp57F;struct Cyc_List_List*
_tmp580;struct Cyc_List_List*_tmp582;struct Cyc_Absyn_Datatypedecl*_tmp583;struct
Cyc_Absyn_Datatypefield*_tmp584;struct Cyc_Absyn_MallocInfo _tmp588;int _tmp589;
struct Cyc_Absyn_Exp*_tmp58A;void**_tmp58B;struct Cyc_Absyn_Exp*_tmp58C;int _tmp58D;
struct Cyc_Absyn_Exp*_tmp58F;struct Cyc_Absyn_Exp*_tmp590;struct Cyc_Absyn_Exp*
_tmp592;struct _dyneither_ptr*_tmp593;struct Cyc_Absyn_Stmt*_tmp595;_LL22A: {
struct Cyc_Absyn_Const_e_struct*_tmp512=(struct Cyc_Absyn_Const_e_struct*)_tmp511;
if(_tmp512->tag != 0)goto _LL22C;else{_tmp513=_tmp512->f1;if((_tmp513.Null_c).tag
!= 1)goto _LL22C;_tmp514=(int)(_tmp513.Null_c).val;}}_LL22B: {struct Cyc_Absyn_Exp*
_tmp59A=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp59A,
_tmp59A))->r;else{struct Cyc_Absyn_Exp*_tmpC34[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((
_tmpC34[2]=_tmp59A,((_tmpC34[1]=_tmp59A,((_tmpC34[0]=_tmp59A,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC34,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL229;}_LL22C: {struct Cyc_Absyn_Const_e_struct*
_tmp515=(struct Cyc_Absyn_Const_e_struct*)_tmp511;if(_tmp515->tag != 0)goto _LL22E;}
_LL22D: goto _LL229;_LL22E: {struct Cyc_Absyn_Var_e_struct*_tmp516=(struct Cyc_Absyn_Var_e_struct*)
_tmp511;if(_tmp516->tag != 1)goto _LL230;else{_tmp517=_tmp516->f1;_tmp518=(void*)
_tmp516->f2;}}_LL22F:{struct _handler_cons _tmp59C;_push_handler(& _tmp59C);{int
_tmp59E=0;if(setjmp(_tmp59C.handler))_tmp59E=1;if(!_tmp59E){e->r=(Cyc_Toc_lookup_varmap(
nv,_tmp517))->r;;_pop_handler();}else{void*_tmp59D=(void*)_exn_thrown;void*
_tmp5A0=_tmp59D;_LL283: {struct Cyc_Dict_Absent_struct*_tmp5A1=(struct Cyc_Dict_Absent_struct*)
_tmp5A0;if(_tmp5A1->tag != Cyc_Dict_Absent)goto _LL285;}_LL284: {const char*_tmpC38;
void*_tmpC37[1];struct Cyc_String_pa_struct _tmpC36;(_tmpC36.tag=0,((_tmpC36.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp517)),((
_tmpC37[0]=& _tmpC36,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC38="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpC38,sizeof(char),
32))),_tag_dyneither(_tmpC37,sizeof(void*),1)))))));}_LL285:;_LL286:(void)_throw(
_tmp5A0);_LL282:;}};}goto _LL229;_LL230: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp519=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp511;if(_tmp519->tag != 2)goto
_LL232;else{_tmp51A=_tmp519->f1;}}_LL231: {const char*_tmpC3B;void*_tmpC3A;(
_tmpC3A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC3B="unknownid",_tag_dyneither(_tmpC3B,sizeof(char),10))),_tag_dyneither(
_tmpC3A,sizeof(void*),0)));}_LL232: {struct Cyc_Absyn_Primop_e_struct*_tmp51B=(
struct Cyc_Absyn_Primop_e_struct*)_tmp511;if(_tmp51B->tag != 3)goto _LL234;else{
_tmp51C=_tmp51B->f1;_tmp51D=_tmp51B->f2;}}_LL233: {struct Cyc_List_List*_tmp5A7=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp51D);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp51D);switch(_tmp51C){case Cyc_Absyn_Numelts: _LL287: {
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp51D))->hd;{void*_tmp5A8=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo _tmp5AA;struct Cyc_Absyn_Exp*
_tmp5AB;struct Cyc_Absyn_PtrInfo _tmp5AD;void*_tmp5AE;struct Cyc_Absyn_PtrAtts
_tmp5AF;union Cyc_Absyn_Constraint*_tmp5B0;union Cyc_Absyn_Constraint*_tmp5B1;
union Cyc_Absyn_Constraint*_tmp5B2;_LL28A: {struct Cyc_Absyn_ArrayType_struct*
_tmp5A9=(struct Cyc_Absyn_ArrayType_struct*)_tmp5A8;if(_tmp5A9->tag != 8)goto
_LL28C;else{_tmp5AA=_tmp5A9->f1;_tmp5AB=_tmp5AA.num_elts;}}_LL28B: if(!Cyc_Evexp_c_can_eval((
struct Cyc_Absyn_Exp*)_check_null(_tmp5AB))){const char*_tmpC3E;void*_tmpC3D;(
_tmpC3D=0,Cyc_Tcutil_terr(e->loc,((_tmpC3E="can't calculate numelts",
_tag_dyneither(_tmpC3E,sizeof(char),24))),_tag_dyneither(_tmpC3D,sizeof(void*),0)));}
e->r=_tmp5AB->r;goto _LL289;_LL28C: {struct Cyc_Absyn_PointerType_struct*_tmp5AC=(
struct Cyc_Absyn_PointerType_struct*)_tmp5A8;if(_tmp5AC->tag != 5)goto _LL28E;else{
_tmp5AD=_tmp5AC->f1;_tmp5AE=_tmp5AD.elt_typ;_tmp5AF=_tmp5AD.ptr_atts;_tmp5B0=
_tmp5AF.nullable;_tmp5B1=_tmp5AF.bounds;_tmp5B2=_tmp5AF.zero_term;}}_LL28D:{void*
_tmp5B5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp5B1);struct Cyc_Absyn_Exp*_tmp5B8;_LL291: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5B6=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5B5;if(_tmp5B6->tag != 0)goto
_LL293;}_LL292:{struct Cyc_Absyn_Exp*_tmpC3F[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,((
_tmpC3F[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5AE),0),((_tmpC3F[0]=(
struct Cyc_Absyn_Exp*)_tmp51D->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC3F,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL290;_LL293: {struct Cyc_Absyn_Upper_b_struct*_tmp5B7=(struct Cyc_Absyn_Upper_b_struct*)
_tmp5B5;if(_tmp5B7->tag != 1)goto _LL290;else{_tmp5B8=_tmp5B7->f1;}}_LL294: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5B2)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp51D->hd);struct Cyc_Absyn_Exp*_tmpC40[2];e->r=Cyc_Toc_fncall_exp_r(
function_e,((_tmpC40[1]=_tmp5B8,((_tmpC40[0]=(struct Cyc_Absyn_Exp*)_tmp51D->hd,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC40,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp5B0)){if(!Cyc_Evexp_c_can_eval(_tmp5B8)){const
char*_tmpC43;void*_tmpC42;(_tmpC42=0,Cyc_Tcutil_terr(e->loc,((_tmpC43="can't calculate numelts",
_tag_dyneither(_tmpC43,sizeof(char),24))),_tag_dyneither(_tmpC42,sizeof(void*),0)));}
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp5B8,Cyc_Absyn_uint_exp(0,0));}else{e->r=
_tmp5B8->r;goto _LL290;}}goto _LL290;_LL290:;}goto _LL289;_LL28E:;_LL28F: {const
char*_tmpC48;void*_tmpC47[2];struct Cyc_String_pa_struct _tmpC46;struct Cyc_String_pa_struct
_tmpC45;(_tmpC45.tag=0,((_tmpC45.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((
_tmpC46.tag=0,((_tmpC46.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmpC47[0]=& _tmpC46,((
_tmpC47[1]=& _tmpC45,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC48="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpC48,
sizeof(char),41))),_tag_dyneither(_tmpC47,sizeof(void*),2)))))))))))));}_LL289:;}
break;}case Cyc_Absyn_Plus: _LL288:{void*_tmp5C1=Cyc_Tcutil_compress((void*)((
struct Cyc_List_List*)_check_null(_tmp5A7))->hd);struct Cyc_Absyn_PtrInfo _tmp5C3;
void*_tmp5C4;struct Cyc_Absyn_PtrAtts _tmp5C5;union Cyc_Absyn_Constraint*_tmp5C6;
union Cyc_Absyn_Constraint*_tmp5C7;_LL297: {struct Cyc_Absyn_PointerType_struct*
_tmp5C2=(struct Cyc_Absyn_PointerType_struct*)_tmp5C1;if(_tmp5C2->tag != 5)goto
_LL299;else{_tmp5C3=_tmp5C2->f1;_tmp5C4=_tmp5C3.elt_typ;_tmp5C5=_tmp5C3.ptr_atts;
_tmp5C6=_tmp5C5.bounds;_tmp5C7=_tmp5C5.zero_term;}}_LL298:{void*_tmp5C8=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp5C6);struct Cyc_Absyn_Exp*_tmp5CB;_LL29C: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5C9=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5C8;if(_tmp5C9->tag != 0)goto
_LL29E;}_LL29D: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp51D))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp51D->tl))->hd;{struct Cyc_Absyn_Exp*_tmpC49[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpC49[2]=e2,((_tmpC49[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5C4),0),((_tmpC49[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC49,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}goto _LL29B;}_LL29E: {struct Cyc_Absyn_Upper_b_struct*_tmp5CA=(struct
Cyc_Absyn_Upper_b_struct*)_tmp5C8;if(_tmp5CA->tag != 1)goto _LL29B;else{_tmp5CB=
_tmp5CA->f1;}}_LL29F: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp5C7)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp51D))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp51D->tl))->hd;struct Cyc_Absyn_Exp*_tmpC4A[3];e->r=(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
e1),((_tmpC4A[2]=e2,((_tmpC4A[1]=_tmp5CB,((_tmpC4A[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC4A,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r;}goto _LL29B;_LL29B:;}goto _LL296;_LL299:;_LL29A: goto _LL296;
_LL296:;}break;case Cyc_Absyn_Minus: _LL295: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp5A7))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp51D))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp51D->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp5A7->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{struct Cyc_Absyn_Exp*_tmpC4B[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpC4B[2]=Cyc_Absyn_prim1_exp(
Cyc_Absyn_Minus,e2,0),((_tmpC4B[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpC4B[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC4B,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}break;}case Cyc_Absyn_Eq:
_LL2A0: goto _LL2A1;case Cyc_Absyn_Neq: _LL2A1: goto _LL2A2;case Cyc_Absyn_Gt: _LL2A2:
goto _LL2A3;case Cyc_Absyn_Gte: _LL2A3: goto _LL2A4;case Cyc_Absyn_Lt: _LL2A4: goto
_LL2A5;case Cyc_Absyn_Lte: _LL2A5: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp51D))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp51D->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp5A7))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp5A7->tl))->hd;void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& elt_typ))e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,
0));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp,0));break;}default: _LL2A6: break;}goto _LL229;}_LL234: {
struct Cyc_Absyn_Increment_e_struct*_tmp51E=(struct Cyc_Absyn_Increment_e_struct*)
_tmp511;if(_tmp51E->tag != 5)goto _LL236;else{_tmp51F=_tmp51E->f1;_tmp520=_tmp51E->f2;}}
_LL235: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp51F->topt))->v;
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;const char*_tmpC4C;struct _dyneither_ptr incr_str=(_tmpC4C="increment",
_tag_dyneither(_tmpC4C,sizeof(char),10));if(_tmp520 == Cyc_Absyn_PreDec  || 
_tmp520 == Cyc_Absyn_PostDec){const char*_tmpC4D;incr_str=((_tmpC4D="decrement",
_tag_dyneither(_tmpC4D,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp51F,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpC51;void*_tmpC50[1];
struct Cyc_String_pa_struct _tmpC4F;(_tmpC4F.tag=0,((_tmpC4F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC50[0]=& _tmpC4F,Cyc_Tcutil_terr(
e->loc,((_tmpC51="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpC51,sizeof(char),74))),_tag_dyneither(_tmpC50,sizeof(void*),1)))))));}{
const char*_tmpC54;void*_tmpC53;(_tmpC53=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC54="in-place inc/dec on zero-term",
_tag_dyneither(_tmpC54,sizeof(char),30))),_tag_dyneither(_tmpC53,sizeof(void*),0)));};}{
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp51F,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp5D5=Cyc_Absyn_signed_int_exp(
1,0);{struct Cyc_Core_Opt*_tmpC55;_tmp5D5->topt=((_tmpC55=_cycalloc(sizeof(*
_tmpC55)),((_tmpC55->v=Cyc_Absyn_sint_typ,_tmpC55))));}switch(_tmp520){case Cyc_Absyn_PreInc:
_LL2A8:{struct Cyc_Absyn_AssignOp_e_struct _tmpC5B;struct Cyc_Core_Opt*_tmpC5A;
struct Cyc_Absyn_AssignOp_e_struct*_tmpC59;e->r=(void*)((_tmpC59=_cycalloc(
sizeof(*_tmpC59)),((_tmpC59[0]=((_tmpC5B.tag=4,((_tmpC5B.f1=_tmp51F,((_tmpC5B.f2=((
_tmpC5A=_cycalloc_atomic(sizeof(*_tmpC5A)),((_tmpC5A->v=(void*)Cyc_Absyn_Plus,
_tmpC5A)))),((_tmpC5B.f3=_tmp5D5,_tmpC5B)))))))),_tmpC59))));}Cyc_Toc_exp_to_c(
nv,e);return;case Cyc_Absyn_PreDec: _LL2A9:{struct Cyc_Absyn_AssignOp_e_struct
_tmpC61;struct Cyc_Core_Opt*_tmpC60;struct Cyc_Absyn_AssignOp_e_struct*_tmpC5F;e->r=(
void*)((_tmpC5F=_cycalloc(sizeof(*_tmpC5F)),((_tmpC5F[0]=((_tmpC61.tag=4,((
_tmpC61.f1=_tmp51F,((_tmpC61.f2=((_tmpC60=_cycalloc_atomic(sizeof(*_tmpC60)),((
_tmpC60->v=(void*)Cyc_Absyn_Minus,_tmpC60)))),((_tmpC61.f3=_tmp5D5,_tmpC61)))))))),
_tmpC5F))));}Cyc_Toc_exp_to_c(nv,e);return;default: _LL2AA:{const char*_tmpC65;
void*_tmpC64[1];struct Cyc_String_pa_struct _tmpC63;(_tmpC63.tag=0,((_tmpC63.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC64[0]=& _tmpC63,Cyc_Tcutil_terr(
e->loc,((_tmpC65="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpC65,sizeof(char),59))),_tag_dyneither(_tmpC64,sizeof(void*),1)))))));}{
const char*_tmpC68;void*_tmpC67;(_tmpC67=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC68="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpC68,sizeof(char),34))),_tag_dyneither(_tmpC67,sizeof(void*),0)));};}}
Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp51F);Cyc_Toc_set_lhs(nv,0);{void*
elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp520 == Cyc_Absyn_PostInc
 || _tmp520 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp520 == Cyc_Absyn_PreDec  || _tmp520 == Cyc_Absyn_PostDec)change=- 1;{struct
Cyc_Absyn_Exp*_tmpC69[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC69[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpC69[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpC69[0]=Cyc_Toc_push_address_exp(_tmp51F),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC69,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Toc_functionSet*_tmp5E3=_tmp520 == Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:&
Cyc_Toc__zero_arr_inplace_plus_post_functionSet;struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(
_tmp5E3,_tmp51F);struct Cyc_Absyn_Exp*_tmpC6A[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((
_tmpC6A[1]=Cyc_Absyn_signed_int_exp(1,0),((_tmpC6A[0]=Cyc_Toc_push_address_exp(
_tmp51F),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC6A,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(elt_typ == (
void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp51F)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(
_tmp51F,0,Cyc_Toc_incr_lvalue,_tmp520);e->r=_tmp51F->r;}}}goto _LL229;};};}_LL236: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp521=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp511;if(_tmp521->tag != 4)goto _LL238;else{_tmp522=_tmp521->f1;_tmp523=_tmp521->f2;
_tmp524=_tmp521->f3;}}_LL237: {void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp522->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp524->topt))->v;int f_tag=0;void*tagged_member_struct_type=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Toc_is_tagged_union_project(_tmp522,& f_tag,&
tagged_member_struct_type,1)){Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp522);
Cyc_Toc_set_lhs(nv,0);Cyc_Toc_exp_to_c(nv,_tmp524);e->r=Cyc_Toc_tagged_union_assignop(
_tmp522,e1_old_typ,_tmp523,_tmp524,e2_old_typ,f_tag,tagged_member_struct_type);
return;}{void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(_tmp522,& ptr_type,&
is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp522,_tmp523,
_tmp524,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp522);Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp522);Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp524);{int done=0;if(_tmp523 != 0){void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;switch((enum Cyc_Absyn_Primop)_tmp523->v){case Cyc_Absyn_Plus: _LL2AC:
change=_tmp524;break;case Cyc_Absyn_Minus: _LL2AD: change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,
_tmp524,0);break;default: _LL2AE: {const char*_tmpC6D;void*_tmpC6C;(_tmpC6C=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC6D="bad t ? pointer arithmetic",_tag_dyneither(_tmpC6D,sizeof(char),27))),
_tag_dyneither(_tmpC6C,sizeof(void*),0)));}}done=1;{struct Cyc_Absyn_Exp*_tmp5E8=
Cyc_Toc__dyneither_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*_tmpC6E[3];e->r=Cyc_Toc_fncall_exp_r(
_tmp5E8,((_tmpC6E[2]=change,((_tmpC6E[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpC6E[0]=Cyc_Toc_push_address_exp(_tmp522),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC6E,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))
switch((enum Cyc_Absyn_Primop)_tmp523->v){case Cyc_Absyn_Plus: _LL2B0: done=1;{
struct Cyc_Absyn_Exp*_tmpC6F[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp522),((_tmpC6F[1]=_tmp524,((
_tmpC6F[0]=_tmp522,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC6F,sizeof(struct Cyc_Absyn_Exp*),2)))))));}break;default:
_LL2B1: {const char*_tmpC72;void*_tmpC71;(_tmpC71=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC72="bad zero-terminated pointer arithmetic",
_tag_dyneither(_tmpC72,sizeof(char),39))),_tag_dyneither(_tmpC71,sizeof(void*),0)));}}}}
if(!done){if(e1_poly)_tmp524->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp524->r,0));if(!Cyc_Absyn_is_lvalue(_tmp522)){{struct _tuple20 _tmpC75;struct
_tuple20*_tmpC74;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple20*),struct _tuple20*f_env))
Cyc_Toc_lvalue_assign)(_tmp522,0,Cyc_Toc_assignop_lvalue,((_tmpC74=_cycalloc(
sizeof(struct _tuple20)* 1),((_tmpC74[0]=((_tmpC75.f1=_tmp523,((_tmpC75.f2=
_tmp524,_tmpC75)))),_tmpC74)))));}e->r=_tmp522->r;}}goto _LL229;};};};}_LL238: {
struct Cyc_Absyn_Conditional_e_struct*_tmp525=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp511;if(_tmp525->tag != 6)goto _LL23A;else{_tmp526=_tmp525->f1;_tmp527=_tmp525->f2;
_tmp528=_tmp525->f3;}}_LL239: Cyc_Toc_exp_to_c(nv,_tmp526);Cyc_Toc_exp_to_c(nv,
_tmp527);Cyc_Toc_exp_to_c(nv,_tmp528);goto _LL229;_LL23A: {struct Cyc_Absyn_And_e_struct*
_tmp529=(struct Cyc_Absyn_And_e_struct*)_tmp511;if(_tmp529->tag != 7)goto _LL23C;
else{_tmp52A=_tmp529->f1;_tmp52B=_tmp529->f2;}}_LL23B: Cyc_Toc_exp_to_c(nv,
_tmp52A);Cyc_Toc_exp_to_c(nv,_tmp52B);goto _LL229;_LL23C: {struct Cyc_Absyn_Or_e_struct*
_tmp52C=(struct Cyc_Absyn_Or_e_struct*)_tmp511;if(_tmp52C->tag != 8)goto _LL23E;
else{_tmp52D=_tmp52C->f1;_tmp52E=_tmp52C->f2;}}_LL23D: Cyc_Toc_exp_to_c(nv,
_tmp52D);Cyc_Toc_exp_to_c(nv,_tmp52E);goto _LL229;_LL23E: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp52F=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp511;if(_tmp52F->tag != 9)goto _LL240;
else{_tmp530=_tmp52F->f1;_tmp531=_tmp52F->f2;}}_LL23F: Cyc_Toc_exp_to_c(nv,
_tmp530);Cyc_Toc_exp_to_c(nv,_tmp531);goto _LL229;_LL240: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp532=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp511;if(_tmp532->tag != 10)goto
_LL242;else{_tmp533=_tmp532->f1;_tmp534=_tmp532->f2;}}_LL241: _tmp536=_tmp533;
_tmp537=_tmp534;goto _LL243;_LL242: {struct Cyc_Absyn_FnCall_e_struct*_tmp535=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp511;if(_tmp535->tag != 11)goto _LL244;else{
_tmp536=_tmp535->f1;_tmp537=_tmp535->f2;_tmp538=_tmp535->f3;if(_tmp538 != 0)goto
_LL244;}}_LL243: Cyc_Toc_exp_to_c(nv,_tmp536);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp537);goto _LL229;_LL244: {struct Cyc_Absyn_FnCall_e_struct*
_tmp539=(struct Cyc_Absyn_FnCall_e_struct*)_tmp511;if(_tmp539->tag != 11)goto
_LL246;else{_tmp53A=_tmp539->f1;_tmp53B=_tmp539->f2;_tmp53C=_tmp539->f3;if(
_tmp53C == 0)goto _LL246;_tmp53D=*_tmp53C;_tmp53E=_tmp53D.num_varargs;_tmp53F=
_tmp53D.injectors;_tmp540=_tmp53D.vai;}}_LL245: {struct _RegionHandle _tmp5EF=
_new_region("r");struct _RegionHandle*r=& _tmp5EF;_push_region(r);{struct _tuple0*
argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp53E,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp540->type);void*arr_type=Cyc_Absyn_array_typ(
cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)num_varargs_exp,Cyc_Absyn_false_conref,
0);int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp53B);int
num_normargs=num_args - _tmp53E;struct Cyc_List_List*new_args=0;{int i=0;for(0;i < 
num_normargs;(++ i,_tmp53B=_tmp53B->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp53B))->hd);{struct Cyc_List_List*_tmpC76;
new_args=((_tmpC76=_cycalloc(sizeof(*_tmpC76)),((_tmpC76->hd=(struct Cyc_Absyn_Exp*)
_tmp53B->hd,((_tmpC76->tl=new_args,_tmpC76))))));};}}{struct Cyc_Absyn_Exp*
_tmpC79[3];struct Cyc_List_List*_tmpC78;new_args=((_tmpC78=_cycalloc(sizeof(*
_tmpC78)),((_tmpC78->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpC79[
2]=num_varargs_exp,((_tmpC79[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpC79[0]=
argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC79,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpC78->tl=
new_args,_tmpC78))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp53A);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp53A,new_args,0),0);if(_tmp540->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp5F3=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp540->type));struct Cyc_Absyn_DatatypeInfo _tmp5F5;union Cyc_Absyn_DatatypeInfoU
_tmp5F6;struct Cyc_Absyn_Datatypedecl**_tmp5F7;struct Cyc_Absyn_Datatypedecl*
_tmp5F8;_LL2B4: {struct Cyc_Absyn_DatatypeType_struct*_tmp5F4=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp5F3;if(_tmp5F4->tag != 3)goto _LL2B6;else{_tmp5F5=_tmp5F4->f1;_tmp5F6=_tmp5F5.datatype_info;
if((_tmp5F6.KnownDatatype).tag != 2)goto _LL2B6;_tmp5F7=(struct Cyc_Absyn_Datatypedecl**)(
_tmp5F6.KnownDatatype).val;_tmp5F8=*_tmp5F7;}}_LL2B5: tud=_tmp5F8;goto _LL2B3;
_LL2B6:;_LL2B7: {const char*_tmpC7C;void*_tmpC7B;(_tmpC7B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC7C="toc: unknown datatype in vararg with inject",
_tag_dyneither(_tmpC7C,sizeof(char),44))),_tag_dyneither(_tmpC7B,sizeof(void*),0)));}
_LL2B3:;}{unsigned int _tmpC81;unsigned int _tmpC80;struct _dyneither_ptr _tmpC7F;
struct _tuple0**_tmpC7E;unsigned int _tmpC7D;struct _dyneither_ptr vs=(_tmpC7D=(
unsigned int)_tmp53E,((_tmpC7E=(struct _tuple0**)_region_malloc(r,_check_times(
sizeof(struct _tuple0*),_tmpC7D)),((_tmpC7F=_tag_dyneither(_tmpC7E,sizeof(struct
_tuple0*),_tmpC7D),((((_tmpC80=_tmpC7D,_tmpC82(& _tmpC81,& _tmpC80,& _tmpC7E))),
_tmpC7F)))))));if(_tmp53E != 0){struct Cyc_List_List*_tmp5FB=0;{int i=_tmp53E - 1;
for(0;i >= 0;-- i){struct Cyc_List_List*_tmpC83;_tmp5FB=((_tmpC83=_cycalloc(sizeof(*
_tmpC83)),((_tmpC83->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((
struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i)),0),0)),((
_tmpC83->tl=_tmp5FB,_tmpC83))))));}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp5FB,0),s,0);{int i=0;for(0;
_tmp53B != 0;(((_tmp53B=_tmp53B->tl,_tmp53F=_tmp53F->tl)),++ i)){struct Cyc_Absyn_Exp*
arg=(struct Cyc_Absyn_Exp*)_tmp53B->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple0*var=*((struct _tuple0**)
_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield _tmp5FE;struct
_tuple0*_tmp5FF;struct Cyc_List_List*_tmp600;struct Cyc_Absyn_Datatypefield*
_tmp5FD=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp53F))->hd;_tmp5FE=*_tmp5FD;_tmp5FF=_tmp5FE.name;_tmp600=_tmp5FE.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp600))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_boxed_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp5FF),0),s,0);{const char*_tmpC84;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp5FF,((_tmpC84="_struct",_tag_dyneither(_tmpC84,
sizeof(char),8))))),0,s,0);};};}};}else{struct _tuple16*_tmpC85[3];struct Cyc_List_List*
_tmp602=(_tmpC85[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC85[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpC85[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC85,sizeof(struct _tuple16*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{{int i=0;for(0;_tmp53B
!= 0;(_tmp53B=_tmp53B->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp53B->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp53B->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);};}_npop_handler(0);goto _LL229;;_pop_region(r);}_LL246: {struct Cyc_Absyn_Throw_e_struct*
_tmp541=(struct Cyc_Absyn_Throw_e_struct*)_tmp511;if(_tmp541->tag != 12)goto _LL248;
else{_tmp542=_tmp541->f1;}}_LL247: Cyc_Toc_exp_to_c(nv,_tmp542);e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp542),0))->r;goto _LL229;_LL248: {
struct Cyc_Absyn_NoInstantiate_e_struct*_tmp543=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp511;if(_tmp543->tag != 13)goto _LL24A;else{_tmp544=_tmp543->f1;}}_LL249: Cyc_Toc_exp_to_c(
nv,_tmp544);goto _LL229;_LL24A: {struct Cyc_Absyn_Instantiate_e_struct*_tmp545=(
struct Cyc_Absyn_Instantiate_e_struct*)_tmp511;if(_tmp545->tag != 14)goto _LL24C;
else{_tmp546=_tmp545->f1;_tmp547=_tmp545->f2;}}_LL24B: Cyc_Toc_exp_to_c(nv,
_tmp546);for(0;_tmp547 != 0;_tmp547=_tmp547->tl){enum Cyc_Absyn_KindQual _tmp608=(
Cyc_Tcutil_typ_kind((void*)_tmp547->hd))->kind;if(_tmp608 != Cyc_Absyn_EffKind
 && _tmp608 != Cyc_Absyn_RgnKind){{void*_tmp609=Cyc_Tcutil_compress((void*)
_tmp547->hd);_LL2B9: {struct Cyc_Absyn_VarType_struct*_tmp60A=(struct Cyc_Absyn_VarType_struct*)
_tmp609;if(_tmp60A->tag != 2)goto _LL2BB;}_LL2BA: goto _LL2BC;_LL2BB: {struct Cyc_Absyn_DatatypeType_struct*
_tmp60B=(struct Cyc_Absyn_DatatypeType_struct*)_tmp609;if(_tmp60B->tag != 3)goto
_LL2BD;}_LL2BC: continue;_LL2BD:;_LL2BE: e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp546,0))->r;goto _LL2B8;
_LL2B8:;}break;}}goto _LL229;_LL24C: {struct Cyc_Absyn_Cast_e_struct*_tmp548=(
struct Cyc_Absyn_Cast_e_struct*)_tmp511;if(_tmp548->tag != 15)goto _LL24E;else{
_tmp549=(void**)& _tmp548->f1;_tmp54A=(void**)((void**)& _tmp548->f1);_tmp54B=
_tmp548->f2;_tmp54C=_tmp548->f3;_tmp54D=_tmp548->f4;}}_LL24D: {void*old_t2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp54B->topt))->v;void*new_typ=*_tmp54A;*
_tmp54A=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp54B);{struct _tuple19
_tmpC86;struct _tuple19 _tmp60D=(_tmpC86.f1=Cyc_Tcutil_compress(old_t2),((_tmpC86.f2=
Cyc_Tcutil_compress(new_typ),_tmpC86)));void*_tmp60E;struct Cyc_Absyn_PtrInfo
_tmp610;void*_tmp611;struct Cyc_Absyn_PtrInfo _tmp613;void*_tmp614;struct Cyc_Absyn_PtrInfo
_tmp616;void*_tmp617;_LL2C0: _tmp60E=_tmp60D.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp60F=(struct Cyc_Absyn_PointerType_struct*)_tmp60E;if(_tmp60F->tag != 5)goto
_LL2C2;else{_tmp610=_tmp60F->f1;}};_tmp611=_tmp60D.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp612=(struct Cyc_Absyn_PointerType_struct*)_tmp611;if(_tmp612->tag != 5)goto
_LL2C2;else{_tmp613=_tmp612->f1;}};_LL2C1: {int _tmp619=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp610.ptr_atts).nullable);int _tmp61A=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp613.ptr_atts).nullable);
void*_tmp61B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp610.ptr_atts).bounds);void*_tmp61C=((void*(*)(void*y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp613.ptr_atts).bounds);
int _tmp61D=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp610.ptr_atts).zero_term);int _tmp61E=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp613.ptr_atts).zero_term);{struct _tuple19 _tmpC87;
struct _tuple19 _tmp620=(_tmpC87.f1=_tmp61B,((_tmpC87.f2=_tmp61C,_tmpC87)));void*
_tmp621;struct Cyc_Absyn_Exp*_tmp623;void*_tmp624;struct Cyc_Absyn_Exp*_tmp626;
void*_tmp627;struct Cyc_Absyn_Exp*_tmp629;void*_tmp62A;void*_tmp62C;void*_tmp62E;
struct Cyc_Absyn_Exp*_tmp630;void*_tmp631;void*_tmp633;_LL2C7: _tmp621=_tmp620.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp622=(struct Cyc_Absyn_Upper_b_struct*)_tmp621;
if(_tmp622->tag != 1)goto _LL2C9;else{_tmp623=_tmp622->f1;}};_tmp624=_tmp620.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp625=(struct Cyc_Absyn_Upper_b_struct*)_tmp624;
if(_tmp625->tag != 1)goto _LL2C9;else{_tmp626=_tmp625->f1;}};_LL2C8: if((!Cyc_Evexp_c_can_eval(
_tmp623) || !Cyc_Evexp_c_can_eval(_tmp626)) && !Cyc_Evexp_same_const_exp(_tmp623,
_tmp626)){const char*_tmpC8A;void*_tmpC89;(_tmpC89=0,Cyc_Tcutil_terr(e->loc,((
_tmpC8A="can't validate cast due to potential size differences",_tag_dyneither(
_tmpC8A,sizeof(char),54))),_tag_dyneither(_tmpC89,sizeof(void*),0)));}if(_tmp619
 && !_tmp61A){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC8D;void*_tmpC8C;(
_tmpC8C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC8D="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC8D,
sizeof(char),44))),_tag_dyneither(_tmpC8C,sizeof(void*),0)));}if(_tmp54D != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC91;void*_tmpC90[1];struct Cyc_String_pa_struct _tmpC8F;(_tmpC8F.tag=
0,((_tmpC8F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpC90[0]=& _tmpC8F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpC91="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpC91,sizeof(char),41))),_tag_dyneither(_tmpC90,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp54B);if(do_null_check){if(!_tmp54C){
const char*_tmpC94;void*_tmpC93;(_tmpC93=0,Cyc_Tcutil_warn(e->loc,((_tmpC94="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpC94,sizeof(char),58))),_tag_dyneither(_tmpC93,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC95;e->r=Cyc_Toc_cast_it_r(*_tmp54A,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC95=_cycalloc(sizeof(*_tmpC95)),((_tmpC95->hd=
_tmp54B,((_tmpC95->tl=0,_tmpC95)))))),0));};}};}goto _LL2C6;_LL2C9: _tmp627=
_tmp620.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp628=(struct Cyc_Absyn_Upper_b_struct*)
_tmp627;if(_tmp628->tag != 1)goto _LL2CB;else{_tmp629=_tmp628->f1;}};_tmp62A=
_tmp620.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp62B=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp62A;if(_tmp62B->tag != 0)goto _LL2CB;};_LL2CA: if(!Cyc_Evexp_c_can_eval(_tmp629)){
const char*_tmpC98;void*_tmpC97;(_tmpC97=0,Cyc_Tcutil_terr(e->loc,((_tmpC98="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC98,sizeof(char),71))),_tag_dyneither(_tmpC97,sizeof(void*),0)));}
if(_tmp54D == Cyc_Absyn_NonNull_to_Null){const char*_tmpC9C;void*_tmpC9B[1];struct
Cyc_String_pa_struct _tmpC9A;(_tmpC9A.tag=0,((_tmpC9A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC9B[0]=& _tmpC9A,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC9C="conversion mis-classified as null-check: %s",
_tag_dyneither(_tmpC9C,sizeof(char),44))),_tag_dyneither(_tmpC9B,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){if((_tmp61D  && !(_tmp613.elt_tq).real_const) && !
_tmp61E)_tmp629=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp629,Cyc_Absyn_uint_exp(1,
0),0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp629,_tmp54B))->r;}else{
struct Cyc_Absyn_Exp*_tmp644=Cyc_Toc__tag_dyneither_e;if(_tmp61D){struct _tuple0*
_tmp645=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp646=Cyc_Absyn_var_exp(_tmp645,
0);struct Cyc_Absyn_Exp*arg3;{void*_tmp647=_tmp54B->r;union Cyc_Absyn_Cnst _tmp649;
struct _dyneither_ptr _tmp64A;union Cyc_Absyn_Cnst _tmp64C;struct _dyneither_ptr
_tmp64D;_LL2D0: {struct Cyc_Absyn_Const_e_struct*_tmp648=(struct Cyc_Absyn_Const_e_struct*)
_tmp647;if(_tmp648->tag != 0)goto _LL2D2;else{_tmp649=_tmp648->f1;if((_tmp649.String_c).tag
!= 8)goto _LL2D2;_tmp64A=(struct _dyneither_ptr)(_tmp649.String_c).val;}}_LL2D1:
arg3=_tmp629;goto _LL2CF;_LL2D2: {struct Cyc_Absyn_Const_e_struct*_tmp64B=(struct
Cyc_Absyn_Const_e_struct*)_tmp647;if(_tmp64B->tag != 0)goto _LL2D4;else{_tmp64C=
_tmp64B->f1;if((_tmp64C.Wstring_c).tag != 9)goto _LL2D4;_tmp64D=(struct
_dyneither_ptr)(_tmp64C.Wstring_c).val;}}_LL2D3: arg3=_tmp629;goto _LL2CF;_LL2D4:;
_LL2D5:{struct Cyc_Absyn_Exp*_tmpC9D[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp54B),((_tmpC9D[1]=_tmp629,((_tmpC9D[0]=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp646),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC9D,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);}goto _LL2CF;_LL2CF:;}if(!_tmp61E  && !(_tmp613.elt_tq).real_const)arg3=
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*
_tmp64F=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp613.elt_typ),0);struct Cyc_Absyn_Exp*
_tmpC9E[3];struct Cyc_Absyn_Exp*_tmp650=Cyc_Absyn_fncall_exp(_tmp644,((_tmpC9E[2]=
arg3,((_tmpC9E[1]=_tmp64F,((_tmpC9E[0]=_tmp646,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC9E,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);struct Cyc_Absyn_Stmt*_tmp651=Cyc_Absyn_exp_stmt(_tmp650,0);_tmp651=
Cyc_Absyn_declare_stmt(_tmp645,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)
_tmp54B,_tmp651,0);e->r=Cyc_Toc_stmt_exp_r(_tmp651);};}else{struct Cyc_Absyn_Exp*
_tmpC9F[3];e->r=Cyc_Toc_fncall_exp_r(_tmp644,((_tmpC9F[2]=_tmp629,((_tmpC9F[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp613.elt_typ),0),((_tmpC9F[0]=
_tmp54B,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC9F,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2C6;
_LL2CB: _tmp62C=_tmp620.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp62D=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp62C;if(_tmp62D->tag != 0)goto _LL2CD;};_tmp62E=_tmp620.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp62F=(struct Cyc_Absyn_Upper_b_struct*)_tmp62E;if(_tmp62F->tag != 1)goto _LL2CD;
else{_tmp630=_tmp62F->f1;}};_LL2CC: if(!Cyc_Evexp_c_can_eval(_tmp630)){const char*
_tmpCA2;void*_tmpCA1;(_tmpCA1=0,Cyc_Tcutil_terr(e->loc,((_tmpCA2="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpCA2,sizeof(char),71))),_tag_dyneither(_tmpCA1,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpCA5;void*_tmpCA4;(_tmpCA4=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCA5="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpCA5,sizeof(char),45))),_tag_dyneither(_tmpCA4,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp658=_tmp630;if(_tmp61D  && !_tmp61E)_tmp658=Cyc_Absyn_add_exp(
_tmp630,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp659=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpCA6[3];struct Cyc_Absyn_Exp*_tmp65A=Cyc_Absyn_fncall_exp(
_tmp659,((_tmpCA6[2]=_tmp658,((_tmpCA6[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp610.elt_typ),0),((_tmpCA6[0]=_tmp54B,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA6,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp61A){struct Cyc_List_List*_tmpCA7;_tmp65A->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_null_e,((_tmpCA7=_cycalloc(sizeof(*_tmpCA7)),((_tmpCA7->hd=Cyc_Absyn_copy_exp(
_tmp65A),((_tmpCA7->tl=0,_tmpCA7)))))));}e->r=Cyc_Toc_cast_it_r(*_tmp54A,_tmp65A);
goto _LL2C6;};};_LL2CD: _tmp631=_tmp620.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp632=(struct Cyc_Absyn_DynEither_b_struct*)_tmp631;if(_tmp632->tag != 0)goto
_LL2C6;};_tmp633=_tmp620.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp634=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp633;if(_tmp634->tag != 0)goto _LL2C6;};_LL2CE:
DynCast: if((_tmp61D  && !_tmp61E) && !(_tmp613.elt_tq).real_const){if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpCAA;void*_tmpCA9;(_tmpCA9=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCAA="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpCAA,sizeof(char),70))),_tag_dyneither(_tmpCA9,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp65F=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpCAB[3];e->r=Cyc_Toc_fncall_exp_r(_tmp65F,((_tmpCAB[2]=Cyc_Absyn_uint_exp(1,0),((
_tmpCAB[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp610.elt_typ),0),((
_tmpCAB[0]=_tmp54B,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCAB,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}goto _LL2C6;
_LL2C6:;}goto _LL2BF;}_LL2C2: _tmp614=_tmp60D.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp615=(struct Cyc_Absyn_PointerType_struct*)_tmp614;if(_tmp615->tag != 5)goto
_LL2C4;else{_tmp616=_tmp615->f1;}};_tmp617=_tmp60D.f2;{struct Cyc_Absyn_IntType_struct*
_tmp618=(struct Cyc_Absyn_IntType_struct*)_tmp617;if(_tmp618->tag != 6)goto _LL2C4;};
_LL2C3:{void*_tmp661=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp616.ptr_atts).bounds);_LL2D7: {struct Cyc_Absyn_DynEither_b_struct*
_tmp662=(struct Cyc_Absyn_DynEither_b_struct*)_tmp661;if(_tmp662->tag != 0)goto
_LL2D9;}_LL2D8: _tmp54B->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp54B->r,
_tmp54B->loc),Cyc_Toc_curr_sp);goto _LL2D6;_LL2D9:;_LL2DA: goto _LL2D6;_LL2D6:;}
goto _LL2BF;_LL2C4:;_LL2C5: goto _LL2BF;_LL2BF:;}goto _LL229;}_LL24E: {struct Cyc_Absyn_Address_e_struct*
_tmp54E=(struct Cyc_Absyn_Address_e_struct*)_tmp511;if(_tmp54E->tag != 16)goto
_LL250;else{_tmp54F=_tmp54E->f1;}}_LL24F:{void*_tmp663=_tmp54F->r;struct _tuple0*
_tmp665;struct Cyc_List_List*_tmp666;struct Cyc_List_List*_tmp667;struct Cyc_List_List*
_tmp669;_LL2DC: {struct Cyc_Absyn_Aggregate_e_struct*_tmp664=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp663;if(_tmp664->tag != 29)goto _LL2DE;else{_tmp665=_tmp664->f1;_tmp666=_tmp664->f2;
_tmp667=_tmp664->f3;}}_LL2DD: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpCAF;void*
_tmpCAE[1];struct Cyc_String_pa_struct _tmpCAD;(_tmpCAD.tag=0,((_tmpCAD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp54F->loc)),((
_tmpCAE[0]=& _tmpCAD,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpCAF="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpCAF,
sizeof(char),42))),_tag_dyneither(_tmpCAE,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp54F->topt))->v,_tmp666,1,0,
_tmp667,_tmp665))->r;goto _LL2DB;_LL2DE: {struct Cyc_Absyn_Tuple_e_struct*_tmp668=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp663;if(_tmp668->tag != 25)goto _LL2E0;else{
_tmp669=_tmp668->f1;}}_LL2DF: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpCB3;void*
_tmpCB2[1];struct Cyc_String_pa_struct _tmpCB1;(_tmpCB1.tag=0,((_tmpCB1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp54F->loc)),((
_tmpCB2[0]=& _tmpCB1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpCB3="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpCB3,
sizeof(char),42))),_tag_dyneither(_tmpCB2,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_tuple(
nv,1,0,_tmp669))->r;goto _LL2DB;_LL2E0:;_LL2E1: Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(
nv,_tmp54F);Cyc_Toc_set_lhs(nv,0);if(!Cyc_Absyn_is_lvalue(_tmp54F)){((void(*)(
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
int),int f_env))Cyc_Toc_lvalue_assign)(_tmp54F,0,Cyc_Toc_address_lvalue,1);e->r=
Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmp54F);}goto _LL2DB;_LL2DB:;}goto _LL229;_LL250: {struct Cyc_Absyn_New_e_struct*
_tmp550=(struct Cyc_Absyn_New_e_struct*)_tmp511;if(_tmp550->tag != 17)goto _LL252;
else{_tmp551=_tmp550->f1;_tmp552=_tmp550->f2;}}_LL251: if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCB7;void*_tmpCB6[1];struct Cyc_String_pa_struct _tmpCB5;(_tmpCB5.tag=
0,((_tmpCB5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
_tmp552->loc)),((_tmpCB6[0]=& _tmpCB5,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCB7="%s: new at top-level",_tag_dyneither(
_tmpCB7,sizeof(char),21))),_tag_dyneither(_tmpCB6,sizeof(void*),1)))))));}{void*
_tmp673=_tmp552->r;struct Cyc_List_List*_tmp675;struct Cyc_Absyn_Vardecl*_tmp677;
struct Cyc_Absyn_Exp*_tmp678;struct Cyc_Absyn_Exp*_tmp679;int _tmp67A;struct _tuple0*
_tmp67C;struct Cyc_List_List*_tmp67D;struct Cyc_List_List*_tmp67E;struct Cyc_Absyn_Aggrdecl*
_tmp67F;struct Cyc_List_List*_tmp681;_LL2E3: {struct Cyc_Absyn_Array_e_struct*
_tmp674=(struct Cyc_Absyn_Array_e_struct*)_tmp673;if(_tmp674->tag != 27)goto _LL2E5;
else{_tmp675=_tmp674->f1;}}_LL2E4: {struct _tuple0*_tmp682=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp683=Cyc_Absyn_var_exp(_tmp682,0);struct Cyc_Absyn_Stmt*
_tmp684=Cyc_Toc_init_array(nv,_tmp683,_tmp675,Cyc_Absyn_exp_stmt(_tmp683,0));
void*old_elt_typ;{void*_tmp685=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp687;void*_tmp688;struct Cyc_Absyn_Tqual _tmp689;struct Cyc_Absyn_PtrAtts _tmp68A;
union Cyc_Absyn_Constraint*_tmp68B;_LL2EE: {struct Cyc_Absyn_PointerType_struct*
_tmp686=(struct Cyc_Absyn_PointerType_struct*)_tmp685;if(_tmp686->tag != 5)goto
_LL2F0;else{_tmp687=_tmp686->f1;_tmp688=_tmp687.elt_typ;_tmp689=_tmp687.elt_tq;
_tmp68A=_tmp687.ptr_atts;_tmp68B=_tmp68A.zero_term;}}_LL2EF: old_elt_typ=_tmp688;
goto _LL2ED;_LL2F0:;_LL2F1: {const char*_tmpCBA;void*_tmpCB9;old_elt_typ=((_tmpCB9=
0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCBA="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(
_tmpCBA,sizeof(char),52))),_tag_dyneither(_tmpCB9,sizeof(void*),0))));}_LL2ED:;}{
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp68E=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp68F=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp675),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp551 == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp68F);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp551;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp68F);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp682,_tmp68E,(struct Cyc_Absyn_Exp*)e1,_tmp684,0));goto
_LL2E2;};}_LL2E5: {struct Cyc_Absyn_Comprehension_e_struct*_tmp676=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp673;if(_tmp676->tag != 28)goto _LL2E7;else{_tmp677=_tmp676->f1;_tmp678=_tmp676->f2;
_tmp679=_tmp676->f3;_tmp67A=_tmp676->f4;}}_LL2E6: {int is_dyneither_ptr=0;{void*
_tmp690=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp692;void*_tmp693;
struct Cyc_Absyn_Tqual _tmp694;struct Cyc_Absyn_PtrAtts _tmp695;union Cyc_Absyn_Constraint*
_tmp696;union Cyc_Absyn_Constraint*_tmp697;_LL2F3: {struct Cyc_Absyn_PointerType_struct*
_tmp691=(struct Cyc_Absyn_PointerType_struct*)_tmp690;if(_tmp691->tag != 5)goto
_LL2F5;else{_tmp692=_tmp691->f1;_tmp693=_tmp692.elt_typ;_tmp694=_tmp692.elt_tq;
_tmp695=_tmp692.ptr_atts;_tmp696=_tmp695.bounds;_tmp697=_tmp695.zero_term;}}
_LL2F4: is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp696)== (void*)& Cyc_Absyn_DynEither_b_val;goto _LL2F2;
_LL2F5:;_LL2F6: {const char*_tmpCBD;void*_tmpCBC;(_tmpCBC=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCBD="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpCBD,sizeof(char),42))),_tag_dyneither(_tmpCBC,sizeof(void*),0)));}
_LL2F2:;}{struct _tuple0*max=Cyc_Toc_temp_var();struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp679->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp678);{struct Cyc_Absyn_Exp*_tmp69A=
Cyc_Absyn_var_exp(max,0);if(_tmp67A)_tmp69A=Cyc_Absyn_add_exp(_tmp69A,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp677,Cyc_Absyn_var_exp(max,0),_tmp679,_tmp67A,Cyc_Toc_skip_stmt_dl(),1);
struct _RegionHandle _tmp69B=_new_region("r");struct _RegionHandle*r=& _tmp69B;
_push_region(r);{struct _tuple21*_tmpCC0;struct Cyc_List_List*_tmpCBF;struct Cyc_List_List*
decls=(_tmpCBF=_region_malloc(r,sizeof(*_tmpCBF)),((_tmpCBF->hd=((_tmpCC0=
_region_malloc(r,sizeof(*_tmpCC0)),((_tmpCC0->f1=max,((_tmpCC0->f2=Cyc_Absyn_uint_typ,((
_tmpCC0->f3=(struct Cyc_Absyn_Exp*)_tmp678,_tmpCC0)))))))),((_tmpCBF->tl=0,
_tmpCBF)))));struct Cyc_Absyn_Exp*ai;if(_tmp551 == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpCC1[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpCC1[1]=_tmp69A,((_tmpCC1[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCC1,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{struct Cyc_Absyn_Exp*
r=(struct Cyc_Absyn_Exp*)_tmp551;Cyc_Toc_exp_to_c(nv,r);{struct Cyc_Absyn_Exp*
_tmpCC2[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((
_tmpCC2[1]=_tmp69A,((_tmpCC2[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCC2,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));};}{struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct
_tuple21*_tmpCC5;struct Cyc_List_List*_tmpCC4;decls=((_tmpCC4=_region_malloc(r,
sizeof(*_tmpCC4)),((_tmpCC4->hd=((_tmpCC5=_region_malloc(r,sizeof(*_tmpCC5)),((
_tmpCC5->f1=a,((_tmpCC5->f2=ptr_typ,((_tmpCC5->f3=(struct Cyc_Absyn_Exp*)ainit,
_tmpCC5)))))))),((_tmpCC4->tl=decls,_tmpCC4))))));}if(is_dyneither_ptr){struct
_tuple0*_tmp6A0=Cyc_Toc_temp_var();void*_tmp6A1=Cyc_Toc_typ_to_c(old_typ);struct
Cyc_Absyn_Exp*_tmp6A2=Cyc_Toc__tag_dyneither_e;struct Cyc_Absyn_Exp*_tmpCC6[3];
struct Cyc_Absyn_Exp*_tmp6A3=Cyc_Absyn_fncall_exp(_tmp6A2,((_tmpCC6[2]=_tmp69A,((
_tmpCC6[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpCC6[0]=Cyc_Absyn_var_exp(a,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCC6,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{struct _tuple21*_tmpCC9;struct
Cyc_List_List*_tmpCC8;decls=((_tmpCC8=_region_malloc(r,sizeof(*_tmpCC8)),((
_tmpCC8->hd=((_tmpCC9=_region_malloc(r,sizeof(*_tmpCC9)),((_tmpCC9->f1=_tmp6A0,((
_tmpCC9->f2=_tmp6A1,((_tmpCC9->f3=(struct Cyc_Absyn_Exp*)_tmp6A3,_tmpCC9)))))))),((
_tmpCC8->tl=decls,_tmpCC8))))));}s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(
_tmp6A0,0),0),0);}else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(
a,0),0),0);}{struct Cyc_List_List*_tmp6A7=decls;for(0;_tmp6A7 != 0;_tmp6A7=_tmp6A7->tl){
struct _tuple0*_tmp6A9;void*_tmp6AA;struct Cyc_Absyn_Exp*_tmp6AB;struct _tuple21
_tmp6A8=*((struct _tuple21*)_tmp6A7->hd);_tmp6A9=_tmp6A8.f1;_tmp6AA=_tmp6A8.f2;
_tmp6AB=_tmp6A8.f3;s=Cyc_Absyn_declare_stmt(_tmp6A9,_tmp6AA,_tmp6AB,s,0);}}e->r=
Cyc_Toc_stmt_exp_r(s);};}_npop_handler(0);goto _LL2E2;;_pop_region(r);};};};}
_LL2E7: {struct Cyc_Absyn_Aggregate_e_struct*_tmp67B=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp673;if(_tmp67B->tag != 29)goto _LL2E9;else{_tmp67C=_tmp67B->f1;_tmp67D=_tmp67B->f2;
_tmp67E=_tmp67B->f3;_tmp67F=_tmp67B->f4;}}_LL2E8: e->r=(Cyc_Toc_init_struct(nv,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp552->topt))->v,_tmp67D,1,_tmp551,
_tmp67E,_tmp67C))->r;goto _LL2E2;_LL2E9: {struct Cyc_Absyn_Tuple_e_struct*_tmp680=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp673;if(_tmp680->tag != 25)goto _LL2EB;else{
_tmp681=_tmp680->f1;}}_LL2EA: e->r=(Cyc_Toc_init_tuple(nv,1,_tmp551,_tmp681))->r;
goto _LL2E2;_LL2EB:;_LL2EC: {void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp552->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct
_tuple0*_tmp6AE=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp6AF=Cyc_Absyn_var_exp(
_tmp6AE,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(
_tmp6AF,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp551 == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp551;Cyc_Toc_exp_to_c(nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{
void*_tmp6B0=_tmp552->r;void*_tmp6B2;struct Cyc_Absyn_Exp*_tmp6B3;_LL2F8: {struct
Cyc_Absyn_Cast_e_struct*_tmp6B1=(struct Cyc_Absyn_Cast_e_struct*)_tmp6B0;if(
_tmp6B1->tag != 15)goto _LL2FA;else{_tmp6B2=(void*)_tmp6B1->f1;_tmp6B3=_tmp6B1->f2;}}
_LL2F9:{struct _tuple19 _tmpCCA;struct _tuple19 _tmp6B5=(_tmpCCA.f1=Cyc_Tcutil_compress(
_tmp6B2),((_tmpCCA.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp6B3->topt))->v),_tmpCCA)));void*_tmp6B6;struct Cyc_Absyn_PtrInfo
_tmp6B8;void*_tmp6B9;struct Cyc_Absyn_PtrAtts _tmp6BA;union Cyc_Absyn_Constraint*
_tmp6BB;void*_tmp6BC;struct Cyc_Absyn_PtrInfo _tmp6BE;struct Cyc_Absyn_PtrAtts
_tmp6BF;union Cyc_Absyn_Constraint*_tmp6C0;_LL2FD: _tmp6B6=_tmp6B5.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp6B7=(struct Cyc_Absyn_PointerType_struct*)_tmp6B6;if(_tmp6B7->tag != 5)goto
_LL2FF;else{_tmp6B8=_tmp6B7->f1;_tmp6B9=_tmp6B8.elt_typ;_tmp6BA=_tmp6B8.ptr_atts;
_tmp6BB=_tmp6BA.bounds;}};_tmp6BC=_tmp6B5.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp6BD=(struct Cyc_Absyn_PointerType_struct*)_tmp6BC;if(_tmp6BD->tag != 5)goto
_LL2FF;else{_tmp6BE=_tmp6BD->f1;_tmp6BF=_tmp6BE.ptr_atts;_tmp6C0=_tmp6BF.bounds;}};
_LL2FE:{struct _tuple19 _tmpCCB;struct _tuple19 _tmp6C2=(_tmpCCB.f1=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6BB),((_tmpCCB.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp6C0),_tmpCCB)));void*_tmp6C3;void*_tmp6C5;struct Cyc_Absyn_Exp*
_tmp6C7;_LL302: _tmp6C3=_tmp6C2.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp6C4=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp6C3;if(_tmp6C4->tag != 0)goto _LL304;};
_tmp6C5=_tmp6C2.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp6C6=(struct Cyc_Absyn_Upper_b_struct*)
_tmp6C5;if(_tmp6C6->tag != 1)goto _LL304;else{_tmp6C7=_tmp6C6->f1;}};_LL303: Cyc_Toc_exp_to_c(
nv,_tmp6B3);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp6C8=Cyc_Toc__init_dyneither_ptr_e;{struct Cyc_Absyn_Exp*_tmpCCC[4];e->r=Cyc_Toc_fncall_exp_r(
_tmp6C8,((_tmpCCC[3]=_tmp6C7,((_tmpCCC[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp6B9),0),((_tmpCCC[1]=_tmp6B3,((_tmpCCC[0]=mexp,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCCC,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))));}goto _LL301;};_LL304:;_LL305: goto _LL301;_LL301:;}goto _LL2FC;_LL2FF:;
_LL300: goto _LL2FC;_LL2FC:;}goto _LL2F7;_LL2FA:;_LL2FB: goto _LL2F7;_LL2F7:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp6CA=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp6AF),
0);struct Cyc_Absyn_Exp*_tmp6CB=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,
_tmp552);_tmp6CA=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
_tmp6AF,_tmp6CB,0),_tmp552,0),_tmp6CA,0);{void*_tmp6CC=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp6AE,
_tmp6CC,(struct Cyc_Absyn_Exp*)mexp,_tmp6CA,0));};}goto _LL2E2;};}_LL2E2:;}goto
_LL229;_LL252: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp553=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp511;if(_tmp553->tag != 19)goto _LL254;else{_tmp554=_tmp553->f1;}}_LL253: Cyc_Toc_exp_to_c(
nv,_tmp554);goto _LL229;_LL254: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp555=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp511;if(_tmp555->tag != 18)goto _LL256;
else{_tmp556=(void*)_tmp555->f1;}}_LL255:{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpCCF;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpCCE;e->r=(void*)((_tmpCCE=
_cycalloc(sizeof(*_tmpCCE)),((_tmpCCE[0]=((_tmpCCF.tag=18,((_tmpCCF.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp556),_tmpCCF)))),_tmpCCE))));}goto _LL229;_LL256: {
struct Cyc_Absyn_Offsetof_e_struct*_tmp557=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp511;if(_tmp557->tag != 20)goto _LL258;else{_tmp558=(void*)_tmp557->f1;_tmp559=(
void*)_tmp557->f2;{struct Cyc_Absyn_StructField_struct*_tmp55A=(struct Cyc_Absyn_StructField_struct*)
_tmp559;if(_tmp55A->tag != 0)goto _LL258;else{_tmp55B=_tmp55A->f1;}};}}_LL257:{
struct Cyc_Absyn_Offsetof_e_struct _tmpCD9;struct Cyc_Absyn_StructField_struct
_tmpCD8;struct Cyc_Absyn_StructField_struct*_tmpCD7;struct Cyc_Absyn_Offsetof_e_struct*
_tmpCD6;e->r=(void*)((_tmpCD6=_cycalloc(sizeof(*_tmpCD6)),((_tmpCD6[0]=((_tmpCD9.tag=
20,((_tmpCD9.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp558),((_tmpCD9.f2=(void*)((
void*)((_tmpCD7=_cycalloc(sizeof(*_tmpCD7)),((_tmpCD7[0]=((_tmpCD8.tag=0,((
_tmpCD8.f1=_tmp55B,_tmpCD8)))),_tmpCD7))))),_tmpCD9)))))),_tmpCD6))));}goto
_LL229;_LL258: {struct Cyc_Absyn_Offsetof_e_struct*_tmp55C=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp511;if(_tmp55C->tag != 20)goto _LL25A;else{_tmp55D=(void*)_tmp55C->f1;_tmp55E=(
void*)_tmp55C->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp55F=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp55E;if(_tmp55F->tag != 1)goto _LL25A;else{_tmp560=_tmp55F->f1;}};}}_LL259:{
void*_tmp6D3=Cyc_Tcutil_compress(_tmp55D);struct Cyc_Absyn_AggrInfo _tmp6D5;union
Cyc_Absyn_AggrInfoU _tmp6D6;struct Cyc_List_List*_tmp6D8;_LL307: {struct Cyc_Absyn_AggrType_struct*
_tmp6D4=(struct Cyc_Absyn_AggrType_struct*)_tmp6D3;if(_tmp6D4->tag != 11)goto
_LL309;else{_tmp6D5=_tmp6D4->f1;_tmp6D6=_tmp6D5.aggr_info;}}_LL308: {struct Cyc_Absyn_Aggrdecl*
_tmp6DB=Cyc_Absyn_get_known_aggrdecl(_tmp6D6);if(_tmp6DB->impl == 0){const char*
_tmpCDC;void*_tmpCDB;(_tmpCDB=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCDC="struct fields must be known",
_tag_dyneither(_tmpCDC,sizeof(char),28))),_tag_dyneither(_tmpCDB,sizeof(void*),0)));}
_tmp6D8=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6DB->impl))->fields;goto
_LL30A;}_LL309: {struct Cyc_Absyn_AnonAggrType_struct*_tmp6D7=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp6D3;if(_tmp6D7->tag != 12)goto _LL30B;else{_tmp6D8=_tmp6D7->f2;}}_LL30A: {
struct Cyc_Absyn_Aggrfield*_tmp6DE=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp6D8,(int)_tmp560);{struct Cyc_Absyn_Offsetof_e_struct
_tmpCE6;struct Cyc_Absyn_StructField_struct _tmpCE5;struct Cyc_Absyn_StructField_struct*
_tmpCE4;struct Cyc_Absyn_Offsetof_e_struct*_tmpCE3;e->r=(void*)((_tmpCE3=
_cycalloc(sizeof(*_tmpCE3)),((_tmpCE3[0]=((_tmpCE6.tag=20,((_tmpCE6.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp55D),((_tmpCE6.f2=(void*)((void*)((_tmpCE4=_cycalloc(
sizeof(*_tmpCE4)),((_tmpCE4[0]=((_tmpCE5.tag=0,((_tmpCE5.f1=_tmp6DE->name,
_tmpCE5)))),_tmpCE4))))),_tmpCE6)))))),_tmpCE3))));}goto _LL306;}_LL30B: {struct
Cyc_Absyn_TupleType_struct*_tmp6D9=(struct Cyc_Absyn_TupleType_struct*)_tmp6D3;
if(_tmp6D9->tag != 10)goto _LL30D;}_LL30C:{struct Cyc_Absyn_Offsetof_e_struct
_tmpCF0;struct Cyc_Absyn_StructField_struct _tmpCEF;struct Cyc_Absyn_StructField_struct*
_tmpCEE;struct Cyc_Absyn_Offsetof_e_struct*_tmpCED;e->r=(void*)((_tmpCED=
_cycalloc(sizeof(*_tmpCED)),((_tmpCED[0]=((_tmpCF0.tag=20,((_tmpCF0.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp55D),((_tmpCF0.f2=(void*)((void*)((_tmpCEE=_cycalloc(
sizeof(*_tmpCEE)),((_tmpCEE[0]=((_tmpCEF.tag=0,((_tmpCEF.f1=Cyc_Absyn_fieldname((
int)(_tmp560 + 1)),_tmpCEF)))),_tmpCEE))))),_tmpCF0)))))),_tmpCED))));}goto _LL306;
_LL30D: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp6DA=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp6D3;if(_tmp6DA->tag != 4)goto _LL30F;}_LL30E: if(_tmp560 == 0){struct Cyc_Absyn_Offsetof_e_struct
_tmpCFA;struct Cyc_Absyn_StructField_struct _tmpCF9;struct Cyc_Absyn_StructField_struct*
_tmpCF8;struct Cyc_Absyn_Offsetof_e_struct*_tmpCF7;e->r=(void*)((_tmpCF7=
_cycalloc(sizeof(*_tmpCF7)),((_tmpCF7[0]=((_tmpCFA.tag=20,((_tmpCFA.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp55D),((_tmpCFA.f2=(void*)((void*)((_tmpCF8=_cycalloc(
sizeof(*_tmpCF8)),((_tmpCF8[0]=((_tmpCF9.tag=0,((_tmpCF9.f1=Cyc_Toc_tag_sp,
_tmpCF9)))),_tmpCF8))))),_tmpCFA)))))),_tmpCF7))));}else{struct Cyc_Absyn_Offsetof_e_struct
_tmpD04;struct Cyc_Absyn_StructField_struct _tmpD03;struct Cyc_Absyn_StructField_struct*
_tmpD02;struct Cyc_Absyn_Offsetof_e_struct*_tmpD01;e->r=(void*)((_tmpD01=
_cycalloc(sizeof(*_tmpD01)),((_tmpD01[0]=((_tmpD04.tag=20,((_tmpD04.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp55D),((_tmpD04.f2=(void*)((void*)((_tmpD02=_cycalloc(
sizeof(*_tmpD02)),((_tmpD02[0]=((_tmpD03.tag=0,((_tmpD03.f1=Cyc_Absyn_fieldname((
int)_tmp560),_tmpD03)))),_tmpD02))))),_tmpD04)))))),_tmpD01))));}goto _LL306;
_LL30F:;_LL310: {const char*_tmpD07;void*_tmpD06;(_tmpD06=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD07="impossible type for offsetof tuple index",
_tag_dyneither(_tmpD07,sizeof(char),41))),_tag_dyneither(_tmpD06,sizeof(void*),0)));}
_LL306:;}goto _LL229;_LL25A: {struct Cyc_Absyn_Deref_e_struct*_tmp561=(struct Cyc_Absyn_Deref_e_struct*)
_tmp511;if(_tmp561->tag != 21)goto _LL25C;else{_tmp562=_tmp561->f1;}}_LL25B: {int
_tmp6F1=Cyc_Toc_in_lhs(nv);Cyc_Toc_set_lhs(nv,0);{void*_tmp6F2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp562->topt))->v);{void*_tmp6F3=
_tmp6F2;struct Cyc_Absyn_PtrInfo _tmp6F5;void*_tmp6F6;struct Cyc_Absyn_Tqual _tmp6F7;
struct Cyc_Absyn_PtrAtts _tmp6F8;void*_tmp6F9;union Cyc_Absyn_Constraint*_tmp6FA;
union Cyc_Absyn_Constraint*_tmp6FB;union Cyc_Absyn_Constraint*_tmp6FC;_LL312: {
struct Cyc_Absyn_PointerType_struct*_tmp6F4=(struct Cyc_Absyn_PointerType_struct*)
_tmp6F3;if(_tmp6F4->tag != 5)goto _LL314;else{_tmp6F5=_tmp6F4->f1;_tmp6F6=_tmp6F5.elt_typ;
_tmp6F7=_tmp6F5.elt_tq;_tmp6F8=_tmp6F5.ptr_atts;_tmp6F9=_tmp6F8.rgn;_tmp6FA=
_tmp6F8.nullable;_tmp6FB=_tmp6F8.bounds;_tmp6FC=_tmp6F8.zero_term;}}_LL313:{void*
_tmp6FD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6FB);struct Cyc_Absyn_Exp*_tmp6FF;_LL317: {struct Cyc_Absyn_Upper_b_struct*
_tmp6FE=(struct Cyc_Absyn_Upper_b_struct*)_tmp6FD;if(_tmp6FE->tag != 1)goto _LL319;
else{_tmp6FF=_tmp6FE->f1;}}_LL318: {int do_null_check=Cyc_Toc_need_null_check(
_tmp562);Cyc_Toc_exp_to_c(nv,_tmp562);if(do_null_check){if(Cyc_Toc_warn_all_null_deref){
const char*_tmpD0A;void*_tmpD09;(_tmpD09=0,Cyc_Tcutil_warn(e->loc,((_tmpD0A="inserted null check due to dereference",
_tag_dyneither(_tmpD0A,sizeof(char),39))),_tag_dyneither(_tmpD09,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpD0B;_tmp562->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp6F2),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpD0B=_cycalloc(sizeof(*
_tmpD0B)),((_tmpD0B->hd=Cyc_Absyn_copy_exp(_tmp562),((_tmpD0B->tl=0,_tmpD0B)))))),
0));};}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6FC)){unsigned int _tmp705;int _tmp706;struct _tuple11 _tmp704=Cyc_Evexp_eval_const_uint_exp(
_tmp6FF);_tmp705=_tmp704.f1;_tmp706=_tmp704.f2;if(!_tmp706  || _tmp705 <= 0){const
char*_tmpD0E;void*_tmpD0D;(_tmpD0D=0,Cyc_Tcutil_terr(e->loc,((_tmpD0E="cannot determine dereference is in bounds",
_tag_dyneither(_tmpD0E,sizeof(char),42))),_tag_dyneither(_tmpD0D,sizeof(void*),0)));}}
goto _LL316;}_LL319: {struct Cyc_Absyn_DynEither_b_struct*_tmp700=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp6FD;if(_tmp700->tag != 0)goto _LL316;}_LL31A: {struct Cyc_Absyn_Exp*_tmp709=Cyc_Absyn_uint_exp(
0,0);{struct Cyc_Core_Opt*_tmpD0F;_tmp709->topt=((_tmpD0F=_cycalloc(sizeof(*
_tmpD0F)),((_tmpD0F->v=Cyc_Absyn_uint_typ,_tmpD0F))));}e->r=Cyc_Toc_subscript_exp_r(
_tmp562,_tmp709);Cyc_Toc_exp_to_c(nv,e);goto _LL316;}_LL316:;}goto _LL311;_LL314:;
_LL315: {const char*_tmpD12;void*_tmpD11;(_tmpD11=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD12="exp_to_c: Deref: non-pointer",
_tag_dyneither(_tmpD12,sizeof(char),29))),_tag_dyneither(_tmpD11,sizeof(void*),0)));}
_LL311:;}Cyc_Toc_set_lhs(nv,_tmp6F1);goto _LL229;};}_LL25C: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp563=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp511;if(_tmp563->tag != 22)goto
_LL25E;else{_tmp564=_tmp563->f1;_tmp565=_tmp563->f2;_tmp566=_tmp563->f3;_tmp567=
_tmp563->f4;}}_LL25D: {int is_poly=Cyc_Toc_is_poly_project(e);void*e1_cyc_type=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp564->topt))->v;Cyc_Toc_exp_to_c(nv,
_tmp564);if(_tmp566  && _tmp567)e->r=Cyc_Toc_check_tagged_union(_tmp564,Cyc_Toc_typ_to_c(
e1_cyc_type),e1_cyc_type,_tmp565,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrmember_exp);
if(is_poly)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp(e->r,0),0))->r;goto _LL229;}_LL25E: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp568=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp511;if(_tmp568->tag != 23)goto _LL260;else{_tmp569=_tmp568->f1;_tmp56A=_tmp568->f2;
_tmp56B=_tmp568->f3;_tmp56C=_tmp568->f4;}}_LL25F: {int _tmp70D=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp569->topt))->v);int do_null_check=Cyc_Toc_need_null_check(_tmp569);
Cyc_Toc_exp_to_c(nv,_tmp569);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp70F;
struct Cyc_Absyn_Tqual _tmp710;struct Cyc_Absyn_PtrAtts _tmp711;void*_tmp712;union
Cyc_Absyn_Constraint*_tmp713;union Cyc_Absyn_Constraint*_tmp714;union Cyc_Absyn_Constraint*
_tmp715;struct Cyc_Absyn_PtrInfo _tmp70E=Cyc_Toc_get_ptr_type(e1typ);_tmp70F=
_tmp70E.elt_typ;_tmp710=_tmp70E.elt_tq;_tmp711=_tmp70E.ptr_atts;_tmp712=_tmp711.rgn;
_tmp713=_tmp711.nullable;_tmp714=_tmp711.bounds;_tmp715=_tmp711.zero_term;{void*
_tmp716=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp714);struct Cyc_Absyn_Exp*_tmp718;_LL31C: {struct Cyc_Absyn_Upper_b_struct*
_tmp717=(struct Cyc_Absyn_Upper_b_struct*)_tmp716;if(_tmp717->tag != 1)goto _LL31E;
else{_tmp718=_tmp717->f1;}}_LL31D: {unsigned int _tmp71B;int _tmp71C;struct
_tuple11 _tmp71A=Cyc_Evexp_eval_const_uint_exp(_tmp718);_tmp71B=_tmp71A.f1;
_tmp71C=_tmp71A.f2;if(_tmp71C){if(_tmp71B < 1){const char*_tmpD15;void*_tmpD14;(
_tmpD14=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD15="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpD15,
sizeof(char),44))),_tag_dyneither(_tmpD14,sizeof(void*),0)));}if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){const char*_tmpD18;void*_tmpD17;(_tmpD17=0,Cyc_Tcutil_warn(
e->loc,((_tmpD18="inserted null check due to dereference",_tag_dyneither(_tmpD18,
sizeof(char),39))),_tag_dyneither(_tmpD17,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpD19[1];_tmp569->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpD19[0]=Cyc_Absyn_new_exp(_tmp569->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD19,sizeof(struct Cyc_Absyn_Exp*),
1)))),0));};}}else{if(!Cyc_Evexp_c_can_eval(_tmp718)){const char*_tmpD1C;void*
_tmpD1B;(_tmpD1B=0,Cyc_Tcutil_terr(e->loc,((_tmpD1C="cannot determine pointer dereference in bounds",
_tag_dyneither(_tmpD1C,sizeof(char),47))),_tag_dyneither(_tmpD1B,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpD1D[4];_tmp569->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((_tmpD1D[3]=
Cyc_Absyn_uint_exp(0,0),((_tmpD1D[2]=Cyc_Absyn_sizeoftyp_exp(_tmp70F,0),((
_tmpD1D[1]=_tmp718,((_tmpD1D[0]=Cyc_Absyn_new_exp(_tmp569->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD1D,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0));};}goto _LL31B;}_LL31E: {struct Cyc_Absyn_DynEither_b_struct*
_tmp719=(struct Cyc_Absyn_DynEither_b_struct*)_tmp716;if(_tmp719->tag != 0)goto
_LL31B;}_LL31F: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp70F);{struct Cyc_Absyn_Exp*
_tmpD1E[3];_tmp569->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp710),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_dyneither_subscript_e,((_tmpD1E[2]=Cyc_Absyn_uint_exp(0,0),((
_tmpD1E[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD1E[0]=Cyc_Absyn_new_exp(_tmp569->r,
0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD1E,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}goto _LL31B;}_LL31B:;}if(
_tmp56B  && _tmp56C)e->r=Cyc_Toc_check_tagged_union(_tmp569,Cyc_Toc_typ_to_c(
e1typ),_tmp70F,_tmp56A,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);if(is_poly
 && _tmp56C)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp(e->r,0),0))->r;Cyc_Toc_set_lhs(nv,
_tmp70D);goto _LL229;};};}_LL260: {struct Cyc_Absyn_Subscript_e_struct*_tmp56D=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp511;if(_tmp56D->tag != 24)goto _LL262;
else{_tmp56E=_tmp56D->f1;_tmp56F=_tmp56D->f2;}}_LL261: {int _tmp726=Cyc_Toc_in_lhs(
nv);Cyc_Toc_set_lhs(nv,0);{void*_tmp727=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp56E->topt))->v);{void*_tmp728=_tmp727;struct Cyc_List_List*
_tmp72A;struct Cyc_Absyn_PtrInfo _tmp72C;void*_tmp72D;struct Cyc_Absyn_Tqual _tmp72E;
struct Cyc_Absyn_PtrAtts _tmp72F;void*_tmp730;union Cyc_Absyn_Constraint*_tmp731;
union Cyc_Absyn_Constraint*_tmp732;union Cyc_Absyn_Constraint*_tmp733;_LL321: {
struct Cyc_Absyn_TupleType_struct*_tmp729=(struct Cyc_Absyn_TupleType_struct*)
_tmp728;if(_tmp729->tag != 10)goto _LL323;else{_tmp72A=_tmp729->f1;}}_LL322: Cyc_Toc_exp_to_c(
nv,_tmp56E);Cyc_Toc_exp_to_c(nv,_tmp56F);{unsigned int _tmp735;int _tmp736;struct
_tuple11 _tmp734=Cyc_Evexp_eval_const_uint_exp(_tmp56F);_tmp735=_tmp734.f1;
_tmp736=_tmp734.f2;if(!_tmp736){const char*_tmpD21;void*_tmpD20;(_tmpD20=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD21="unknown tuple subscript in translation to C",
_tag_dyneither(_tmpD21,sizeof(char),44))),_tag_dyneither(_tmpD20,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp56E,Cyc_Absyn_fieldname((int)(_tmp735 + 1)));
goto _LL320;};_LL323: {struct Cyc_Absyn_PointerType_struct*_tmp72B=(struct Cyc_Absyn_PointerType_struct*)
_tmp728;if(_tmp72B->tag != 5)goto _LL325;else{_tmp72C=_tmp72B->f1;_tmp72D=_tmp72C.elt_typ;
_tmp72E=_tmp72C.elt_tq;_tmp72F=_tmp72C.ptr_atts;_tmp730=_tmp72F.rgn;_tmp731=
_tmp72F.nullable;_tmp732=_tmp72F.bounds;_tmp733=_tmp72F.zero_term;}}_LL324: {
struct Cyc_List_List*_tmp739=Cyc_Toc_get_relns(_tmp56E);int in_bnds=0;{void*
_tmp73A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp732);_LL328:;_LL329: in_bnds=Cyc_Toc_check_bounds(_tmp727,_tmp739,_tmp56E,
_tmp56F);if(Cyc_Toc_warn_bounds_checks  && !in_bnds){const char*_tmpD25;void*
_tmpD24[1];struct Cyc_String_pa_struct _tmpD23;(_tmpD23.tag=0,((_tmpD23.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpD24[0]=&
_tmpD23,Cyc_Tcutil_warn(e->loc,((_tmpD25="bounds check necessary for %s",
_tag_dyneither(_tmpD25,sizeof(char),30))),_tag_dyneither(_tmpD24,sizeof(void*),1)))))));}
_LL327:;}Cyc_Toc_exp_to_c(nv,_tmp56E);Cyc_Toc_exp_to_c(nv,_tmp56F);++ Cyc_Toc_total_bounds_checks;{
void*_tmp73E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp732);struct Cyc_Absyn_Exp*_tmp740;_LL32B: {struct Cyc_Absyn_Upper_b_struct*
_tmp73F=(struct Cyc_Absyn_Upper_b_struct*)_tmp73E;if(_tmp73F->tag != 1)goto _LL32D;
else{_tmp740=_tmp73F->f1;}}_LL32C: {int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp731);void*ta1=Cyc_Toc_typ_to_c(_tmp72D);void*ta2=
Cyc_Absyn_cstar_typ(ta1,_tmp72E);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp733)){if(!Cyc_Evexp_c_can_eval(_tmp740)){const char*_tmpD28;void*_tmpD27;(
_tmpD27=0,Cyc_Tcutil_terr(e->loc,((_tmpD28="cannot determine subscript is in bounds",
_tag_dyneither(_tmpD28,sizeof(char),40))),_tag_dyneither(_tmpD27,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp56E);struct Cyc_Absyn_Exp*_tmpD29[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpD29[2]=_tmp56F,((_tmpD29[1]=_tmp740,((
_tmpD29[0]=_tmp56E,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD29,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp740)){const char*_tmpD2C;void*_tmpD2B;(
_tmpD2B=0,Cyc_Tcutil_terr(e->loc,((_tmpD2C="cannot determine subscript is in bounds",
_tag_dyneither(_tmpD2C,sizeof(char),40))),_tag_dyneither(_tmpD2B,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){const char*_tmpD2F;void*_tmpD2E;(_tmpD2E=0,Cyc_Tcutil_warn(
e->loc,((_tmpD2F="inserted null check due to dereference",_tag_dyneither(_tmpD2F,
sizeof(char),39))),_tag_dyneither(_tmpD2E,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpD30[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((
_tmpD30[3]=_tmp56F,((_tmpD30[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD30[1]=
_tmp740,((_tmpD30[0]=_tmp56E,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD30,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{if(
!Cyc_Evexp_c_can_eval(_tmp740)){const char*_tmpD33;void*_tmpD32;(_tmpD32=0,Cyc_Tcutil_terr(
e->loc,((_tmpD33="cannot determine subscript is in bounds",_tag_dyneither(
_tmpD33,sizeof(char),40))),_tag_dyneither(_tmpD32,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpD34[2];_tmp56F->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((
_tmpD34[1]=Cyc_Absyn_copy_exp(_tmp56F),((_tmpD34[0]=_tmp740,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD34,sizeof(struct Cyc_Absyn_Exp*),
2)))))));};}}}goto _LL32A;}_LL32D: {struct Cyc_Absyn_DynEither_b_struct*_tmp741=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp73E;if(_tmp741->tag != 0)goto _LL32A;}
_LL32E: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp72D);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp72E),
Cyc_Absyn_aggrmember_exp(_tmp56E,Cyc_Toc_curr_sp,0)),_tmp56F);}else{struct Cyc_Absyn_Exp*
_tmp74D=Cyc_Toc__check_dyneither_subscript_e;struct Cyc_Absyn_Exp*_tmpD35[3];e->r=
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp72E),Cyc_Absyn_fncall_exp(
_tmp74D,((_tmpD35[2]=_tmp56F,((_tmpD35[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((
_tmpD35[0]=_tmp56E,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD35,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL32A;}
_LL32A:;}goto _LL320;}_LL325:;_LL326: {const char*_tmpD38;void*_tmpD37;(_tmpD37=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD38="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(
_tmpD38,sizeof(char),49))),_tag_dyneither(_tmpD37,sizeof(void*),0)));}_LL320:;}
Cyc_Toc_set_lhs(nv,_tmp726);goto _LL229;};}_LL262: {struct Cyc_Absyn_Tuple_e_struct*
_tmp570=(struct Cyc_Absyn_Tuple_e_struct*)_tmp511;if(_tmp570->tag != 25)goto _LL264;
else{_tmp571=_tmp570->f1;}}_LL263: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(
nv,0,0,_tmp571))->r;else{struct Cyc_List_List*_tmp751=((struct Cyc_List_List*(*)(
struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_tup_to_c,_tmp571);void*_tmp752=Cyc_Toc_add_tuple_type(_tmp751);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp571 != 0;(_tmp571=_tmp571->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp571->hd);{struct _tuple16*_tmpD3B;struct Cyc_List_List*
_tmpD3A;dles=((_tmpD3A=_cycalloc(sizeof(*_tmpD3A)),((_tmpD3A->hd=((_tmpD3B=
_cycalloc(sizeof(*_tmpD3B)),((_tmpD3B->f1=0,((_tmpD3B->f2=(struct Cyc_Absyn_Exp*)
_tmp571->hd,_tmpD3B)))))),((_tmpD3A->tl=dles,_tmpD3A))))));};}}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);e->r=Cyc_Toc_unresolvedmem_exp_r(
0,dles);}goto _LL229;_LL264: {struct Cyc_Absyn_Array_e_struct*_tmp572=(struct Cyc_Absyn_Array_e_struct*)
_tmp511;if(_tmp572->tag != 27)goto _LL266;else{_tmp573=_tmp572->f1;}}_LL265: e->r=
Cyc_Toc_unresolvedmem_exp_r(0,_tmp573);{struct Cyc_List_List*_tmp755=_tmp573;for(
0;_tmp755 != 0;_tmp755=_tmp755->tl){struct _tuple16 _tmp757;struct Cyc_Absyn_Exp*
_tmp758;struct _tuple16*_tmp756=(struct _tuple16*)_tmp755->hd;_tmp757=*_tmp756;
_tmp758=_tmp757.f2;Cyc_Toc_exp_to_c(nv,_tmp758);}}goto _LL229;_LL266: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp574=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp511;if(_tmp574->tag != 28)
goto _LL268;else{_tmp575=_tmp574->f1;_tmp576=_tmp574->f2;_tmp577=_tmp574->f3;
_tmp578=_tmp574->f4;}}_LL267: {unsigned int _tmp75A;int _tmp75B;struct _tuple11
_tmp759=Cyc_Evexp_eval_const_uint_exp(_tmp576);_tmp75A=_tmp759.f1;_tmp75B=
_tmp759.f2;{void*_tmp75C=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp577->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp577);{struct Cyc_List_List*
es=0;if(!Cyc_Toc_is_zero(_tmp577)){if(!_tmp75B){const char*_tmpD3E;void*_tmpD3D;(
_tmpD3D=0,Cyc_Tcutil_terr(_tmp576->loc,((_tmpD3E="cannot determine value of constant",
_tag_dyneither(_tmpD3E,sizeof(char),35))),_tag_dyneither(_tmpD3D,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp75A;++ i){struct _tuple16*_tmpD41;struct Cyc_List_List*
_tmpD40;es=((_tmpD40=_cycalloc(sizeof(*_tmpD40)),((_tmpD40->hd=((_tmpD41=
_cycalloc(sizeof(*_tmpD41)),((_tmpD41->f1=0,((_tmpD41->f2=_tmp577,_tmpD41)))))),((
_tmpD40->tl=es,_tmpD40))))));}}if(_tmp578){struct Cyc_Absyn_Exp*_tmp761=Cyc_Toc_cast_it(
_tmp75C,Cyc_Absyn_uint_exp(0,0));struct _tuple16*_tmpD44;struct Cyc_List_List*
_tmpD43;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,((_tmpD43=_cycalloc(sizeof(*_tmpD43)),((_tmpD43->hd=((
_tmpD44=_cycalloc(sizeof(*_tmpD44)),((_tmpD44->f1=0,((_tmpD44->f2=_tmp761,
_tmpD44)))))),((_tmpD43->tl=0,_tmpD43)))))));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL229;};};}_LL268: {struct Cyc_Absyn_Aggregate_e_struct*_tmp579=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp511;if(_tmp579->tag != 29)goto _LL26A;
else{_tmp57A=_tmp579->f1;_tmp57B=_tmp579->f2;_tmp57C=_tmp579->f3;_tmp57D=_tmp579->f4;}}
_LL269: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp57B,
0,0,_tmp57C,_tmp57A))->r;else{if(_tmp57D == 0){const char*_tmpD47;void*_tmpD46;(
_tmpD46=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD47="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpD47,sizeof(
char),38))),_tag_dyneither(_tmpD46,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp57D;struct _RegionHandle _tmp766=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp766;_push_region(rgn);{struct Cyc_List_List*_tmp767=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*
des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
rgn,e->loc,_tmp57C,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp57D->impl))->tagged){struct
_tuple22 _tmp769;struct Cyc_Absyn_Aggrfield*_tmp76A;struct Cyc_Absyn_Exp*_tmp76B;
struct _tuple22*_tmp768=(struct _tuple22*)((struct Cyc_List_List*)_check_null(
_tmp767))->hd;_tmp769=*_tmp768;_tmp76A=_tmp769.f1;_tmp76B=_tmp769.f2;{void*
_tmp76C=_tmp76A->type;Cyc_Toc_exp_to_c(nv,_tmp76B);if(Cyc_Toc_is_void_star_or_boxed_tvar(
_tmp76C))_tmp76B->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp76B->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp57D,_tmp76A->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple16*_tmpD4C;struct _tuple16*_tmpD4B;struct _tuple16*_tmpD4A[2];
struct Cyc_List_List*_tmp76D=(_tmpD4A[1]=((_tmpD4B=_cycalloc(sizeof(*_tmpD4B)),((
_tmpD4B->f1=0,((_tmpD4B->f2=_tmp76B,_tmpD4B)))))),((_tmpD4A[0]=((_tmpD4C=
_cycalloc(sizeof(*_tmpD4C)),((_tmpD4C->f1=0,((_tmpD4C->f2=field_tag_exp,_tmpD4C)))))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD4A,sizeof(struct _tuple16*),2)))));struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(
0,_tmp76D,0);struct Cyc_Absyn_FieldName_struct*_tmpD52;struct Cyc_Absyn_FieldName_struct
_tmpD51;void*_tmpD50[1];struct Cyc_List_List*ds=(_tmpD50[0]=(void*)((_tmpD52=
_cycalloc(sizeof(*_tmpD52)),((_tmpD52[0]=((_tmpD51.tag=1,((_tmpD51.f1=_tmp76A->name,
_tmpD51)))),_tmpD52)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD50,sizeof(void*),1)));struct _tuple16*_tmpD55;struct _tuple16*
_tmpD54[1];struct Cyc_List_List*dles=(_tmpD54[0]=((_tmpD55=_cycalloc(sizeof(*
_tmpD55)),((_tmpD55->f1=ds,((_tmpD55->f2=umem,_tmpD55)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD54,sizeof(struct
_tuple16*),1)));e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{struct Cyc_List_List*
_tmp776=0;struct Cyc_List_List*_tmp777=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp777 != 0;_tmp777=_tmp777->tl){struct Cyc_List_List*
_tmp778=_tmp767;for(0;_tmp778 != 0;_tmp778=_tmp778->tl){if((*((struct _tuple22*)
_tmp778->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp777->hd){struct _tuple22
_tmp77A;struct Cyc_Absyn_Aggrfield*_tmp77B;struct Cyc_Absyn_Exp*_tmp77C;struct
_tuple22*_tmp779=(struct _tuple22*)_tmp778->hd;_tmp77A=*_tmp779;_tmp77B=_tmp77A.f1;
_tmp77C=_tmp77A.f2;{void*_tmp77D=_tmp77B->type;Cyc_Toc_exp_to_c(nv,_tmp77C);if(
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp77D))_tmp77C->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp77C->r,0));{struct _tuple16*_tmpD58;struct Cyc_List_List*
_tmpD57;_tmp776=((_tmpD57=_cycalloc(sizeof(*_tmpD57)),((_tmpD57->hd=((_tmpD58=
_cycalloc(sizeof(*_tmpD58)),((_tmpD58->f1=0,((_tmpD58->f2=_tmp77C,_tmpD58)))))),((
_tmpD57->tl=_tmp776,_tmpD57))))));}break;};}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp776));}};
_pop_region(rgn);};}goto _LL229;_LL26A: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp57E=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp511;if(_tmp57E->tag != 30)goto
_LL26C;else{_tmp57F=(void*)_tmp57E->f1;_tmp580=_tmp57E->f2;}}_LL26B: {struct Cyc_List_List*
fs;{void*_tmp780=Cyc_Tcutil_compress(_tmp57F);struct Cyc_List_List*_tmp782;_LL330: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp781=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp780;if(_tmp781->tag != 12)goto _LL332;else{_tmp782=_tmp781->f2;}}_LL331: fs=
_tmp782;goto _LL32F;_LL332:;_LL333: {const char*_tmpD5C;void*_tmpD5B[1];struct Cyc_String_pa_struct
_tmpD5A;(_tmpD5A.tag=0,((_tmpD5A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp57F)),((_tmpD5B[0]=& _tmpD5A,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD5C="anon struct has type %s",
_tag_dyneither(_tmpD5C,sizeof(char),24))),_tag_dyneither(_tmpD5B,sizeof(void*),1)))))));}
_LL32F:;}{struct _RegionHandle _tmp786=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp786;_push_region(rgn);{struct Cyc_List_List*_tmp787=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,
_tmp580,Cyc_Absyn_StructA,fs);for(0;_tmp787 != 0;_tmp787=_tmp787->tl){struct
_tuple22 _tmp789;struct Cyc_Absyn_Aggrfield*_tmp78A;struct Cyc_Absyn_Exp*_tmp78B;
struct _tuple22*_tmp788=(struct _tuple22*)_tmp787->hd;_tmp789=*_tmp788;_tmp78A=
_tmp789.f1;_tmp78B=_tmp789.f2;{void*_tmp78C=_tmp78A->type;Cyc_Toc_exp_to_c(nv,
_tmp78B);if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp78C))_tmp78B->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp78B->r,0));};}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp580);}_npop_handler(0);goto _LL229;;_pop_region(rgn);};}_LL26C: {struct Cyc_Absyn_Datatype_e_struct*
_tmp581=(struct Cyc_Absyn_Datatype_e_struct*)_tmp511;if(_tmp581->tag != 31)goto
_LL26E;else{_tmp582=_tmp581->f1;_tmp583=_tmp581->f2;_tmp584=_tmp581->f3;}}_LL26D: {
void*datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple0*_tmp78D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp78E=Cyc_Absyn_var_exp(_tmp78D,0);struct Cyc_Absyn_Exp*
member_exp;{const char*_tmpD5D;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp584->name,((_tmpD5D="_struct",_tag_dyneither(_tmpD5D,sizeof(char),8)))));}
tag_exp=_tmp583->is_extensible?Cyc_Absyn_var_exp(_tmp584->name,0): Cyc_Toc_datatype_tag(
_tmp583,_tmp584->name);member_exp=_tmp78E;{struct Cyc_List_List*_tmp790=_tmp584->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp582 != 0;(
_tmp582=_tmp582->tl,_tmp790=_tmp790->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp582->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp790))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_boxed_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct _tuple16*_tmpD60;struct
Cyc_List_List*_tmpD5F;dles=((_tmpD5F=_cycalloc(sizeof(*_tmpD5F)),((_tmpD5F->hd=((
_tmpD60=_cycalloc(sizeof(*_tmpD60)),((_tmpD60->f1=0,((_tmpD60->f2=cur_e,_tmpD60)))))),((
_tmpD5F->tl=dles,_tmpD5F))))));};}{struct _tuple16*_tmpD63;struct Cyc_List_List*
_tmpD62;dles=((_tmpD62=_cycalloc(sizeof(*_tmpD62)),((_tmpD62->hd=((_tmpD63=
_cycalloc(sizeof(*_tmpD63)),((_tmpD63->f1=0,((_tmpD63->f2=tag_exp,_tmpD63)))))),((
_tmpD62->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles),_tmpD62))))));}e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{struct Cyc_List_List*
_tmpD64;struct Cyc_List_List*_tmp795=(_tmpD64=_cycalloc(sizeof(*_tmpD64)),((
_tmpD64->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,
0),tag_exp,0),((_tmpD64->tl=0,_tmpD64)))));{int i=1;for(0;_tmp582 != 0;(((_tmp582=
_tmp582->tl,i ++)),_tmp790=_tmp790->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp582->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp790))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_boxed_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp796=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpD65;_tmp795=((_tmpD65=_cycalloc(sizeof(*
_tmpD65)),((_tmpD65->hd=_tmp796,((_tmpD65->tl=_tmp795,_tmpD65))))));};}}{struct
Cyc_Absyn_Stmt*_tmp798=Cyc_Absyn_exp_stmt(_tmp78E,0);struct Cyc_List_List*_tmpD66;
struct Cyc_Absyn_Stmt*_tmp799=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpD66=_cycalloc(sizeof(*_tmpD66)),((
_tmpD66->hd=_tmp798,((_tmpD66->tl=_tmp795,_tmpD66))))))),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp78D,datatype_ctype,0,_tmp799,0));};}goto _LL229;};}
_LL26E: {struct Cyc_Absyn_Enum_e_struct*_tmp585=(struct Cyc_Absyn_Enum_e_struct*)
_tmp511;if(_tmp585->tag != 32)goto _LL270;}_LL26F: goto _LL271;_LL270: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp586=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp511;if(_tmp586->tag != 33)goto
_LL272;}_LL271: goto _LL229;_LL272: {struct Cyc_Absyn_Malloc_e_struct*_tmp587=(
struct Cyc_Absyn_Malloc_e_struct*)_tmp511;if(_tmp587->tag != 34)goto _LL274;else{
_tmp588=_tmp587->f1;_tmp589=_tmp588.is_calloc;_tmp58A=_tmp588.rgn;_tmp58B=
_tmp588.elt_type;_tmp58C=_tmp588.num_elts;_tmp58D=_tmp588.fat_result;}}_LL273: {
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp58B)));Cyc_Toc_exp_to_c(nv,
_tmp58C);if(_tmp58D){struct _tuple0*_tmp79C=Cyc_Toc_temp_var();struct _tuple0*
_tmp79D=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp589){xexp=_tmp58C;if(_tmp58A != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp58A;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp79C,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp58B,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp79C,0));}{struct Cyc_Absyn_Exp*_tmpD67[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpD67[2]=Cyc_Absyn_var_exp(_tmp79C,0),((_tmpD67[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpD67[0]=Cyc_Absyn_var_exp(_tmp79D,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD67,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{if(_tmp58A != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp58A;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp79C,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp58B,Cyc_Absyn_var_exp(_tmp79C,0));}{struct Cyc_Absyn_Exp*_tmpD68[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpD68[2]=Cyc_Absyn_var_exp(_tmp79C,0),((_tmpD68[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpD68[0]=Cyc_Absyn_var_exp(_tmp79D,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD68,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);};}{struct Cyc_Absyn_Stmt*_tmp7A0=Cyc_Absyn_declare_stmt(_tmp79C,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)_tmp58C,Cyc_Absyn_declare_stmt(_tmp79D,Cyc_Absyn_cstar_typ(
t_c,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp7A0);};}else{if(_tmp589){if(_tmp58A != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp58A;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp58C))->r;}else{e->r=(
Cyc_Toc_calloc_exp(*_tmp58B,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp58C))->r;}}else{
if(_tmp58A != 0  && !Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp58A;Cyc_Toc_exp_to_c(nv,rgn);e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp58C))->r;}
else{e->r=(Cyc_Toc_malloc_exp(*_tmp58B,_tmp58C))->r;}}}goto _LL229;}_LL274: {
struct Cyc_Absyn_Swap_e_struct*_tmp58E=(struct Cyc_Absyn_Swap_e_struct*)_tmp511;
if(_tmp58E->tag != 35)goto _LL276;else{_tmp58F=_tmp58E->f1;_tmp590=_tmp58E->f2;}}
_LL275: {int is_dyneither_ptr=0;void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp58F->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp590->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,&
is_dyneither_ptr)){const char*_tmpD6B;void*_tmpD6A;(_tmpD6A=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD6B="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpD6B,sizeof(char),57))),_tag_dyneither(_tmpD6A,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}{int f1_tag=0;void*tagged_mem_type1=(void*)&
Cyc_Absyn_VoidType_val;int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp58F,&
f1_tag,& tagged_mem_type1,1);int f2_tag=0;void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp590,& f2_tag,& tagged_mem_type2,1);
Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp58F);Cyc_Toc_exp_to_c(nv,_tmp590);
Cyc_Toc_set_lhs(nv,0);if(e1_tmem)Cyc_Toc_add_tagged_union_check_for_swap(_tmp58F,
f1_tag,tagged_mem_type1);else{_tmp58F=Cyc_Toc_push_address_exp(_tmp58F);}if(
e2_tmem)Cyc_Toc_add_tagged_union_check_for_swap(_tmp590,f2_tag,tagged_mem_type2);
else{_tmp590=Cyc_Toc_push_address_exp(_tmp590);}{struct Cyc_Absyn_Exp*_tmpD6C[2];
e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpD6C[1]=_tmp590,((_tmpD6C[0]=_tmp58F,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD6C,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL229;};};}_LL276: {struct
Cyc_Absyn_Tagcheck_e_struct*_tmp591=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp511;
if(_tmp591->tag != 38)goto _LL278;else{_tmp592=_tmp591->f1;_tmp593=_tmp591->f2;}}
_LL277: {void*_tmp7A4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp592->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp592);{void*_tmp7A5=
_tmp7A4;struct Cyc_Absyn_AggrInfo _tmp7A7;union Cyc_Absyn_AggrInfoU _tmp7A8;struct
Cyc_Absyn_Aggrdecl**_tmp7A9;struct Cyc_Absyn_Aggrdecl*_tmp7AA;_LL335: {struct Cyc_Absyn_AggrType_struct*
_tmp7A6=(struct Cyc_Absyn_AggrType_struct*)_tmp7A5;if(_tmp7A6->tag != 11)goto
_LL337;else{_tmp7A7=_tmp7A6->f1;_tmp7A8=_tmp7A7.aggr_info;if((_tmp7A8.KnownAggr).tag
!= 2)goto _LL337;_tmp7A9=(struct Cyc_Absyn_Aggrdecl**)(_tmp7A8.KnownAggr).val;
_tmp7AA=*_tmp7A9;}}_LL336: {struct Cyc_Absyn_Exp*_tmp7AB=Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp7AA,_tmp593),0);struct Cyc_Absyn_Exp*_tmp7AC=Cyc_Absyn_aggrmember_exp(
_tmp592,_tmp593,0);struct Cyc_Absyn_Exp*_tmp7AD=Cyc_Absyn_aggrmember_exp(_tmp7AC,
Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(_tmp7AD,_tmp7AB,0))->r;goto _LL334;}
_LL337:;_LL338: {const char*_tmpD6F;void*_tmpD6E;(_tmpD6E=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD6F="non-aggregate type in tagcheck",
_tag_dyneither(_tmpD6F,sizeof(char),31))),_tag_dyneither(_tmpD6E,sizeof(void*),0)));}
_LL334:;}goto _LL229;}_LL278: {struct Cyc_Absyn_StmtExp_e_struct*_tmp594=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp511;if(_tmp594->tag != 37)goto _LL27A;else{_tmp595=
_tmp594->f1;}}_LL279: Cyc_Toc_stmt_to_c(nv,_tmp595);goto _LL229;_LL27A: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp596=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp511;if(_tmp596->tag != 36)
goto _LL27C;}_LL27B: {const char*_tmpD72;void*_tmpD71;(_tmpD71=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD72="UnresolvedMem",
_tag_dyneither(_tmpD72,sizeof(char),14))),_tag_dyneither(_tmpD71,sizeof(void*),0)));}
_LL27C: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp597=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp511;if(_tmp597->tag != 26)goto _LL27E;}_LL27D: {const char*_tmpD75;void*_tmpD74;(
_tmpD74=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpD75="compoundlit",_tag_dyneither(_tmpD75,sizeof(char),12))),_tag_dyneither(
_tmpD74,sizeof(void*),0)));}_LL27E: {struct Cyc_Absyn_Valueof_e_struct*_tmp598=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp511;if(_tmp598->tag != 39)goto _LL280;}
_LL27F: {const char*_tmpD78;void*_tmpD77;(_tmpD77=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD78="valueof(-)",
_tag_dyneither(_tmpD78,sizeof(char),11))),_tag_dyneither(_tmpD77,sizeof(void*),0)));}
_LL280: {struct Cyc_Absyn_Asm_e_struct*_tmp599=(struct Cyc_Absyn_Asm_e_struct*)
_tmp511;if(_tmp599->tag != 40)goto _LL229;}_LL281: {const char*_tmpD7B;void*_tmpD7A;(
_tmpD7A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD7B="__asm__",_tag_dyneither(_tmpD7B,sizeof(char),8))),_tag_dyneither(
_tmpD7A,sizeof(void*),0)));}_LL229:;};}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(
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
_tmp7ED=Cyc_Toc_temp_var();{struct _tuple24*_tmpD7E;struct Cyc_List_List*_tmpD7D;
decls=((_tmpD7D=_region_malloc(rgn,sizeof(*_tmpD7D)),((_tmpD7D->hd=((_tmpD7E=
_region_malloc(rgn,sizeof(*_tmpD7E)),((_tmpD7E->f1=_tmp7ED,((_tmpD7E->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpD7E)))))),((_tmpD7D->tl=decls,_tmpD7D))))));}
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
_tmpD81;struct Cyc_Absyn_Enum_e_struct*_tmpD80;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD80=_cycalloc(sizeof(*_tmpD80)),((_tmpD80[0]=((_tmpD81.tag=32,((
_tmpD81.f1=_tmp7D1->name,((_tmpD81.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7D0,((
_tmpD81.f3=(struct Cyc_Absyn_Enumfield*)_tmp7D1,_tmpD81)))))))),_tmpD80)))),0),
fail_stmt);}goto _LL339;_LL34C: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp7D2=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp7B8;if(_tmp7D2->tag != 13)goto _LL34E;else{
_tmp7D3=(void*)_tmp7D2->f1;_tmp7D4=_tmp7D2->f2;}}_LL34D:{struct Cyc_Absyn_AnonEnum_e_struct
_tmpD84;struct Cyc_Absyn_AnonEnum_e_struct*_tmpD83;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD83=_cycalloc(sizeof(*_tmpD83)),((_tmpD83[0]=((_tmpD84.tag=33,((
_tmpD84.f1=_tmp7D4->name,((_tmpD84.f2=(void*)_tmp7D3,((_tmpD84.f3=(struct Cyc_Absyn_Enumfield*)
_tmp7D4,_tmpD84)))))))),_tmpD83)))),0),fail_stmt);}goto _LL339;_LL34E: {struct Cyc_Absyn_Pointer_p_struct*
_tmp7D5=(struct Cyc_Absyn_Pointer_p_struct*)_tmp7B8;if(_tmp7D5->tag != 5)goto
_LL350;else{_tmp7D6=_tmp7D5->f1;_tmp7D7=*_tmp7D6;_tmp7D8=_tmp7D7.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp7D9=(struct Cyc_Absyn_Datatype_p_struct*)_tmp7D8;if(_tmp7D9->tag != 7)goto
_LL350;else{_tmp7DA=_tmp7D9->f1;_tmp7DB=_tmp7D9->f2;_tmp7DC=_tmp7D9->f3;}};}}
_LL34F: s=Cyc_Toc_skip_stmt_dl();{struct _tuple0*_tmp7F5=Cyc_Toc_temp_var();const
char*_tmpD85;struct _tuple0*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp7DB->name,((
_tmpD85="_struct",_tag_dyneither(_tmpD85,sizeof(char),8))));void*_tmp7F6=Cyc_Absyn_cstar_typ(
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
_tuple24*_tmpD88;struct Cyc_List_List*_tmpD87;decls=((_tmpD87=_region_malloc(rgn,
sizeof(*_tmpD87)),((_tmpD87->hd=((_tmpD88=_region_malloc(rgn,sizeof(*_tmpD88)),((
_tmpD88->f1=_tmp7FA,((_tmpD88->f2=_tmp7FC,_tmpD88)))))),((_tmpD87->tl=decls,
_tmpD87))))));}{struct _tuple23 _tmp800=Cyc_Toc_xlate_pat(nv,rgn,_tmp7FB,Cyc_Absyn_var_exp(
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
_tmp804->topt))->v;{struct _tuple24*_tmpD8B;struct Cyc_List_List*_tmpD8A;decls=((
_tmpD8A=_region_malloc(rgn,sizeof(*_tmpD8A)),((_tmpD8A->hd=((_tmpD8B=
_region_malloc(rgn,sizeof(*_tmpD8B)),((_tmpD8B->f1=_tmp805,((_tmpD8B->f2=Cyc_Toc_typ_to_c(
_tmp806),_tmpD8B)))))),((_tmpD8A->tl=decls,_tmpD8A))))));}{struct _tuple23 _tmp809=
Cyc_Toc_xlate_pat(nv,rgn,_tmp806,Cyc_Absyn_var_exp(_tmp805,0),Cyc_Absyn_aggrmember_exp(
path,Cyc_Absyn_fieldname(cnt),0),_tmp804,fail_stmt,decls);nv=_tmp809.f1;decls=
_tmp809.f2;{struct Cyc_Absyn_Stmt*_tmp80A=_tmp809.f3;struct Cyc_Absyn_Stmt*_tmp80B=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp805,0),Cyc_Absyn_aggrmember_exp(r,
Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp80B,
_tmp80A,0),0);};};};}goto _LL339;};_LL354: {struct Cyc_Absyn_Aggr_p_struct*_tmp7E1=(
struct Cyc_Absyn_Aggr_p_struct*)_tmp7B8;if(_tmp7E1->tag != 6)goto _LL356;else{
_tmp7E2=_tmp7E1->f1;if(_tmp7E2 != 0)goto _LL356;}}_LL355: {const char*_tmpD8E;void*
_tmpD8D;(_tmpD8D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD8E="unresolved aggregate pattern!",_tag_dyneither(_tmpD8E,sizeof(char),30))),
_tag_dyneither(_tmpD8D,sizeof(void*),0)));}_LL356: {struct Cyc_Absyn_Aggr_p_struct*
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
const char*_tmpD91;void*_tmpD90;(_tmpD90=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD91="no field name in tagged union pattern",
_tag_dyneither(_tmpD91,sizeof(char),38))),_tag_dyneither(_tmpD90,sizeof(void*),0)));}
_LL360:;}{struct _tuple0*_tmp818=Cyc_Toc_temp_var();void*_tmp819=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp812->topt))->v;void*_tmp81A=Cyc_Toc_typ_to_c(
_tmp819);{struct _tuple24*_tmpD94;struct Cyc_List_List*_tmpD93;decls=((_tmpD93=
_region_malloc(rgn,sizeof(*_tmpD93)),((_tmpD93->hd=((_tmpD94=_region_malloc(rgn,
sizeof(*_tmpD94)),((_tmpD94->f1=_tmp818,((_tmpD94->f2=_tmp81A,_tmpD94)))))),((
_tmpD93->tl=decls,_tmpD93))))));}{struct Cyc_Absyn_Exp*_tmp81D=Cyc_Absyn_aggrmember_exp(
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
struct Cyc_Toc_Match_error_struct _tmpD97;struct Cyc_Toc_Match_error_struct*_tmpD96;(
int)_throw((void*)((_tmpD96=_cycalloc_atomic(sizeof(*_tmpD96)),((_tmpD96[0]=((
_tmpD97.tag=Cyc_Toc_Match_error,_tmpD97)),_tmpD96)))));}_LL365:;}{struct _tuple0*
_tmp82A=Cyc_Toc_temp_var();void*_tmp82B=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp824->topt))->v;void*_tmp82C=Cyc_Toc_typ_to_c(_tmp82B);{struct _tuple24*
_tmpD9A;struct Cyc_List_List*_tmpD99;decls=((_tmpD99=_region_malloc(rgn,sizeof(*
_tmpD99)),((_tmpD99->hd=((_tmpD9A=_region_malloc(rgn,sizeof(*_tmpD9A)),((_tmpD9A->f1=
_tmp82A,((_tmpD9A->f2=_tmp82C,_tmpD9A)))))),((_tmpD99->tl=decls,_tmpD99))))));}{
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
_tuple24*_tmpD9D;struct Cyc_List_List*_tmpD9C;decls=((_tmpD9C=_region_malloc(rgn,
sizeof(*_tmpD9C)),((_tmpD9C->hd=((_tmpD9D=_region_malloc(rgn,sizeof(*_tmpD9D)),((
_tmpD9D->f1=_tmp833,((_tmpD9D->f2=Cyc_Toc_typ_to_c(_tmp834),_tmpD9D)))))),((
_tmpD9C->tl=decls,_tmpD9C))))));}{struct _tuple23 _tmp837=Cyc_Toc_xlate_pat(nv,rgn,
_tmp834,Cyc_Absyn_var_exp(_tmp833,0),Cyc_Absyn_deref_exp(path,0),_tmp7E9,
fail_stmt,decls);nv=_tmp837.f1;decls=_tmp837.f2;{struct Cyc_Absyn_Stmt*_tmp838=
_tmp837.f3;struct Cyc_Absyn_Stmt*_tmp839=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp833,0),Cyc_Absyn_deref_exp(r,0),0),_tmp838,0);if(Cyc_Toc_is_nullable(
t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(
0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),_tmp839,0);else{s=_tmp839;}goto
_LL339;};};}_LL35A: {struct Cyc_Absyn_UnknownId_p_struct*_tmp7EA=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp7B8;if(_tmp7EA->tag != 14)goto _LL35C;}_LL35B: {const char*_tmpDA0;void*_tmpD9F;(
_tmpD9F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDA0="unknownid pat",_tag_dyneither(_tmpDA0,sizeof(char),14))),_tag_dyneither(
_tmpD9F,sizeof(void*),0)));}_LL35C: {struct Cyc_Absyn_UnknownCall_p_struct*
_tmp7EB=(struct Cyc_Absyn_UnknownCall_p_struct*)_tmp7B8;if(_tmp7EB->tag != 15)goto
_LL35E;}_LL35D: {const char*_tmpDA3;void*_tmpDA2;(_tmpDA2=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDA3="unknowncall pat",
_tag_dyneither(_tmpDA3,sizeof(char),16))),_tag_dyneither(_tmpDA2,sizeof(void*),0)));}
_LL35E: {struct Cyc_Absyn_Exp_p_struct*_tmp7EC=(struct Cyc_Absyn_Exp_p_struct*)
_tmp7B8;if(_tmp7EC->tag != 16)goto _LL339;}_LL35F: {const char*_tmpDA6;void*_tmpDA5;(
_tmpDA5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDA6="exp pat",_tag_dyneither(_tmpDA6,sizeof(char),8))),_tag_dyneither(
_tmpDA5,sizeof(void*),0)));}_LL339:;}{struct _tuple23 _tmpDA7;return(_tmpDA7.f1=nv,((
_tmpDA7.f2=decls,((_tmpDA7.f3=s,_tmpDA7)))));};}struct _tuple26{struct
_dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
static struct _tuple26*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){struct _tuple26*_tmpDA8;return(_tmpDA8=_region_malloc(r,sizeof(*_tmpDA8)),((
_tmpDA8->f1=Cyc_Toc_fresh_label(),((_tmpDA8->f2=Cyc_Toc_fresh_label(),((_tmpDA8->f3=
sc,_tmpDA8)))))));}static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
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
struct _RegionHandle _tmp849=_new_region("rgn");struct _RegionHandle*rgn=& _tmp849;
_push_region(rgn);Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),
_tmp848);;_pop_region(rgn);};}}return;}{struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct _RegionHandle _tmp84A=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp84A;_push_region(rgn);{struct Cyc_Toc_Env*
_tmp84B=Cyc_Toc_share_env(rgn,nv);struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple26*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,
rgn,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*
decls=0;{struct Cyc_List_List*_tmp84C=lscs;for(0;_tmp84C != 0;_tmp84C=_tmp84C->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple26*)_tmp84C->hd)).f3;struct
_dyneither_ptr*fail_lab=_tmp84C->tl == 0?end_l:(*((struct _tuple26*)((struct Cyc_List_List*)
_check_null(_tmp84C->tl))->hd)).f1;struct Cyc_Toc_Env*_tmp84E;struct Cyc_List_List*
_tmp84F;struct Cyc_Absyn_Stmt*_tmp850;struct _tuple23 _tmp84D=Cyc_Toc_xlate_pat(
_tmp84B,rgn,_tmp842,r,path,sc->pattern,Cyc_Absyn_goto_stmt(fail_lab,0),decls);
_tmp84E=_tmp84D.f1;_tmp84F=_tmp84D.f2;_tmp850=_tmp84D.f3;if(sc->where_clause != 0){
struct Cyc_Absyn_Exp*_tmp851=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp84E,_tmp851);_tmp850=Cyc_Absyn_seq_stmt(_tmp850,Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp851,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),
0),0);}decls=_tmp84F;{struct Cyc_List_List*_tmpDA9;nvs=((_tmpDA9=_region_malloc(
rgn,sizeof(*_tmpDA9)),((_tmpDA9->hd=_tmp84E,((_tmpDA9->tl=nvs,_tmpDA9))))));}{
struct Cyc_List_List*_tmpDAA;test_stmts=((_tmpDAA=_region_malloc(rgn,sizeof(*
_tmpDAA)),((_tmpDAA->hd=_tmp850,((_tmpDAA->tl=test_stmts,_tmpDAA))))));};}}nvs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),
test_stmts=test_stmts->tl)){struct _tuple26 _tmp855;struct _dyneither_ptr*_tmp856;
struct _dyneither_ptr*_tmp857;struct Cyc_Absyn_Switch_clause*_tmp858;struct
_tuple26*_tmp854=(struct _tuple26*)lscs->hd;_tmp855=*_tmp854;_tmp856=_tmp855.f1;
_tmp857=_tmp855.f2;_tmp858=_tmp855.f3;{struct Cyc_Toc_Env*_tmp859=(struct Cyc_Toc_Env*)((
struct Cyc_List_List*)_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp858->body;
struct _RegionHandle _tmp85A=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp85A;
_push_region(rgn2);if(lscs->tl != 0){struct _tuple26 _tmp85C;struct _dyneither_ptr*
_tmp85D;struct Cyc_Absyn_Switch_clause*_tmp85E;struct _tuple26*_tmp85B=(struct
_tuple26*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp85C=*_tmp85B;
_tmp85D=_tmp85C.f2;_tmp85E=_tmp85C.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(
rgn2,_tmp859,end_l,_tmp85D,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp85E->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)
_check_null(nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(
rgn2,_tmp859,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp856,(
struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp857,s,0),0);{struct Cyc_List_List*_tmpDAB;stmts=((_tmpDAB=_region_malloc(rgn,
sizeof(*_tmpDAB)),((_tmpDAB->hd=s,((_tmpDAB->tl=stmts,_tmpDAB))))));};;
_pop_region(rgn2);};}{struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(
end_l,Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct
_tuple24 _tmp861;struct _tuple0*_tmp862;void*_tmp863;struct _tuple24*_tmp860=(
struct _tuple24*)decls->hd;_tmp861=*_tmp860;_tmp862=_tmp861.f1;_tmp863=_tmp861.f2;
res=Cyc_Absyn_declare_stmt(_tmp862,_tmp863,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(
v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(
struct Cyc_Absyn_Exp*)e,res,0))->r;};};};_pop_region(rgn);};};}static struct Cyc_Absyn_Stmt*
Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,
int cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){struct Cyc_List_List*
_tmpDAC;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,((
_tmpDAC=_cycalloc(sizeof(*_tmpDAC)),((_tmpDAC->hd=Cyc_Absyn_uint_exp((
unsigned int)(n - 1),0),((_tmpDAC->tl=0,_tmpDAC)))))),0),0);}void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s){while(1){void*_tmp865=s->r;struct Cyc_Absyn_Exp*_tmp868;
struct Cyc_Absyn_Stmt*_tmp86A;struct Cyc_Absyn_Stmt*_tmp86B;struct Cyc_Absyn_Exp*
_tmp86D;struct Cyc_Absyn_Exp*_tmp86F;struct Cyc_Absyn_Stmt*_tmp870;struct Cyc_Absyn_Stmt*
_tmp871;struct _tuple9 _tmp873;struct Cyc_Absyn_Exp*_tmp874;struct Cyc_Absyn_Stmt*
_tmp875;struct Cyc_Absyn_Stmt*_tmp877;struct Cyc_Absyn_Stmt*_tmp879;struct Cyc_Absyn_Stmt*
_tmp87B;struct Cyc_Absyn_Exp*_tmp87D;struct _tuple9 _tmp87E;struct Cyc_Absyn_Exp*
_tmp87F;struct _tuple9 _tmp880;struct Cyc_Absyn_Exp*_tmp881;struct Cyc_Absyn_Stmt*
_tmp882;struct Cyc_Absyn_Exp*_tmp884;struct Cyc_List_List*_tmp885;struct Cyc_List_List*
_tmp887;struct Cyc_Absyn_Switch_clause**_tmp888;struct Cyc_Absyn_Decl*_tmp88A;
struct Cyc_Absyn_Stmt*_tmp88B;struct _dyneither_ptr*_tmp88D;struct Cyc_Absyn_Stmt*
_tmp88E;struct Cyc_Absyn_Stmt*_tmp890;struct _tuple9 _tmp891;struct Cyc_Absyn_Exp*
_tmp892;struct Cyc_Absyn_Stmt*_tmp894;struct Cyc_List_List*_tmp895;struct Cyc_Absyn_Exp*
_tmp897;_LL372: {struct Cyc_Absyn_Skip_s_struct*_tmp866=(struct Cyc_Absyn_Skip_s_struct*)
_tmp865;if(_tmp866->tag != 0)goto _LL374;}_LL373: return;_LL374: {struct Cyc_Absyn_Exp_s_struct*
_tmp867=(struct Cyc_Absyn_Exp_s_struct*)_tmp865;if(_tmp867->tag != 1)goto _LL376;
else{_tmp868=_tmp867->f1;}}_LL375: Cyc_Toc_exp_to_c(nv,_tmp868);return;_LL376: {
struct Cyc_Absyn_Seq_s_struct*_tmp869=(struct Cyc_Absyn_Seq_s_struct*)_tmp865;if(
_tmp869->tag != 2)goto _LL378;else{_tmp86A=_tmp869->f1;_tmp86B=_tmp869->f2;}}
_LL377: Cyc_Toc_stmt_to_c(nv,_tmp86A);s=_tmp86B;continue;_LL378: {struct Cyc_Absyn_Return_s_struct*
_tmp86C=(struct Cyc_Absyn_Return_s_struct*)_tmp865;if(_tmp86C->tag != 3)goto _LL37A;
else{_tmp86D=_tmp86C->f1;}}_LL379: {struct Cyc_Core_Opt*topt=0;if(_tmp86D != 0){{
struct Cyc_Core_Opt*_tmpDAD;topt=((_tmpDAD=_cycalloc(sizeof(*_tmpDAD)),((_tmpDAD->v=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp86D->topt))->v),
_tmpDAD))));}Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp86D);}if(s->try_depth
> 0){if(topt != 0){struct _tuple0*_tmp899=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*
_tmp89A=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp899,0),
0);s->r=(Cyc_Absyn_declare_stmt(_tmp899,(void*)topt->v,_tmp86D,Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler(s->try_depth),_tmp89A,0),0))->r;}else{Cyc_Toc_do_npop_before(
s->try_depth,s);}}return;}_LL37A: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp86E=(
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp865;if(_tmp86E->tag != 4)goto _LL37C;
else{_tmp86F=_tmp86E->f1;_tmp870=_tmp86E->f2;_tmp871=_tmp86E->f3;}}_LL37B: Cyc_Toc_exp_to_c(
nv,_tmp86F);Cyc_Toc_stmt_to_c(nv,_tmp870);s=_tmp871;continue;_LL37C: {struct Cyc_Absyn_While_s_struct*
_tmp872=(struct Cyc_Absyn_While_s_struct*)_tmp865;if(_tmp872->tag != 5)goto _LL37E;
else{_tmp873=_tmp872->f1;_tmp874=_tmp873.f1;_tmp875=_tmp872->f2;}}_LL37D: Cyc_Toc_exp_to_c(
nv,_tmp874);{struct _RegionHandle _tmp89B=_new_region("temp");struct _RegionHandle*
temp=& _tmp89B;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),
_tmp875);_npop_handler(0);return;;_pop_region(temp);};_LL37E: {struct Cyc_Absyn_Break_s_struct*
_tmp876=(struct Cyc_Absyn_Break_s_struct*)_tmp865;if(_tmp876->tag != 6)goto _LL380;
else{_tmp877=_tmp876->f1;}}_LL37F: {struct Cyc_Toc_Env _tmp89D;struct
_dyneither_ptr**_tmp89E;struct Cyc_Toc_Env*_tmp89C=nv;_tmp89D=*_tmp89C;_tmp89E=
_tmp89D.break_lab;if(_tmp89E != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp89E,0);{int
dest_depth=_tmp877 == 0?0: _tmp877->try_depth;Cyc_Toc_do_npop_before(s->try_depth - 
dest_depth,s);return;};}_LL380: {struct Cyc_Absyn_Continue_s_struct*_tmp878=(
struct Cyc_Absyn_Continue_s_struct*)_tmp865;if(_tmp878->tag != 7)goto _LL382;else{
_tmp879=_tmp878->f1;}}_LL381: {struct Cyc_Toc_Env _tmp8A0;struct _dyneither_ptr**
_tmp8A1;struct Cyc_Toc_Env*_tmp89F=nv;_tmp8A0=*_tmp89F;_tmp8A1=_tmp8A0.continue_lab;
if(_tmp8A1 != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp8A1,0);_tmp87B=_tmp879;goto _LL383;}
_LL382: {struct Cyc_Absyn_Goto_s_struct*_tmp87A=(struct Cyc_Absyn_Goto_s_struct*)
_tmp865;if(_tmp87A->tag != 8)goto _LL384;else{_tmp87B=_tmp87A->f2;}}_LL383: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp87B))->try_depth,s);
return;_LL384: {struct Cyc_Absyn_For_s_struct*_tmp87C=(struct Cyc_Absyn_For_s_struct*)
_tmp865;if(_tmp87C->tag != 9)goto _LL386;else{_tmp87D=_tmp87C->f1;_tmp87E=_tmp87C->f2;
_tmp87F=_tmp87E.f1;_tmp880=_tmp87C->f3;_tmp881=_tmp880.f1;_tmp882=_tmp87C->f4;}}
_LL385: Cyc_Toc_exp_to_c(nv,_tmp87D);Cyc_Toc_exp_to_c(nv,_tmp87F);Cyc_Toc_exp_to_c(
nv,_tmp881);{struct _RegionHandle _tmp8A2=_new_region("temp");struct _RegionHandle*
temp=& _tmp8A2;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),
_tmp882);_npop_handler(0);return;;_pop_region(temp);};_LL386: {struct Cyc_Absyn_Switch_s_struct*
_tmp883=(struct Cyc_Absyn_Switch_s_struct*)_tmp865;if(_tmp883->tag != 10)goto
_LL388;else{_tmp884=_tmp883->f1;_tmp885=_tmp883->f2;}}_LL387: Cyc_Toc_xlate_switch(
nv,s,_tmp884,_tmp885);return;_LL388: {struct Cyc_Absyn_Fallthru_s_struct*_tmp886=(
struct Cyc_Absyn_Fallthru_s_struct*)_tmp865;if(_tmp886->tag != 11)goto _LL38A;else{
_tmp887=_tmp886->f1;_tmp888=_tmp886->f2;}}_LL389: {struct Cyc_Toc_Env _tmp8A4;
struct Cyc_Toc_FallthruInfo*_tmp8A5;struct Cyc_Toc_Env*_tmp8A3=nv;_tmp8A4=*_tmp8A3;
_tmp8A5=_tmp8A4.fallthru_info;if(_tmp8A5 == 0){const char*_tmpDB0;void*_tmpDAF;(
_tmpDAF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDB0="fallthru in unexpected place",_tag_dyneither(_tmpDB0,sizeof(char),29))),
_tag_dyneither(_tmpDAF,sizeof(void*),0)));}{struct _dyneither_ptr*_tmp8A9;struct
Cyc_List_List*_tmp8AA;struct Cyc_Dict_Dict _tmp8AB;struct Cyc_Toc_FallthruInfo
_tmp8A8=*_tmp8A5;_tmp8A9=_tmp8A8.label;_tmp8AA=_tmp8A8.binders;_tmp8AB=_tmp8A8.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp8A9,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp888)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp8AC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp8AA);struct Cyc_List_List*_tmp8AD=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp887);for(0;_tmp8AC != 0;(_tmp8AC=_tmp8AC->tl,
_tmp8AD=_tmp8AD->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp8AD))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp8AB,(
struct _tuple0*)_tmp8AC->hd),(struct Cyc_Absyn_Exp*)_tmp8AD->hd,0),s2,0);}s->r=s2->r;
return;};};};}_LL38A: {struct Cyc_Absyn_Decl_s_struct*_tmp889=(struct Cyc_Absyn_Decl_s_struct*)
_tmp865;if(_tmp889->tag != 12)goto _LL38C;else{_tmp88A=_tmp889->f1;_tmp88B=_tmp889->f2;}}
_LL38B:{void*_tmp8AE=_tmp88A->r;struct Cyc_Absyn_Vardecl*_tmp8B0;struct Cyc_Absyn_Pat*
_tmp8B2;struct Cyc_Absyn_Exp*_tmp8B3;struct Cyc_List_List*_tmp8B5;struct Cyc_Absyn_Fndecl*
_tmp8B7;struct Cyc_Absyn_Tvar*_tmp8B9;struct Cyc_Absyn_Vardecl*_tmp8BA;int _tmp8BB;
struct Cyc_Absyn_Exp*_tmp8BC;struct Cyc_Absyn_Exp*_tmp8BE;struct Cyc_Absyn_Tvar*
_tmp8BF;struct Cyc_Absyn_Vardecl*_tmp8C0;_LL395: {struct Cyc_Absyn_Var_d_struct*
_tmp8AF=(struct Cyc_Absyn_Var_d_struct*)_tmp8AE;if(_tmp8AF->tag != 0)goto _LL397;
else{_tmp8B0=_tmp8AF->f1;}}_LL396: {struct _RegionHandle _tmp8C1=_new_region("temp");
struct _RegionHandle*temp=& _tmp8C1;_push_region(temp);{struct Cyc_Absyn_Local_b_struct
_tmpDB3;struct Cyc_Absyn_Local_b_struct*_tmpDB2;struct Cyc_Toc_Env*_tmp8C2=Cyc_Toc_add_varmap(
temp,nv,_tmp8B0->name,Cyc_Absyn_varb_exp(_tmp8B0->name,(void*)((_tmpDB2=
_cycalloc(sizeof(*_tmpDB2)),((_tmpDB2[0]=((_tmpDB3.tag=4,((_tmpDB3.f1=_tmp8B0,
_tmpDB3)))),_tmpDB2)))),0));Cyc_Toc_local_decl_to_c(_tmp8C2,_tmp8C2,_tmp8B0,
_tmp88B);}_npop_handler(0);goto _LL394;;_pop_region(temp);}_LL397: {struct Cyc_Absyn_Let_d_struct*
_tmp8B1=(struct Cyc_Absyn_Let_d_struct*)_tmp8AE;if(_tmp8B1->tag != 2)goto _LL399;
else{_tmp8B2=_tmp8B1->f1;_tmp8B3=_tmp8B1->f3;}}_LL398:{void*_tmp8C5=_tmp8B2->r;
struct Cyc_Absyn_Vardecl*_tmp8C7;struct Cyc_Absyn_Pat*_tmp8C8;struct Cyc_Absyn_Pat
_tmp8C9;void*_tmp8CA;_LL3A4: {struct Cyc_Absyn_Var_p_struct*_tmp8C6=(struct Cyc_Absyn_Var_p_struct*)
_tmp8C5;if(_tmp8C6->tag != 1)goto _LL3A6;else{_tmp8C7=_tmp8C6->f1;_tmp8C8=_tmp8C6->f2;
_tmp8C9=*_tmp8C8;_tmp8CA=_tmp8C9.r;{struct Cyc_Absyn_Wild_p_struct*_tmp8CB=(
struct Cyc_Absyn_Wild_p_struct*)_tmp8CA;if(_tmp8CB->tag != 0)goto _LL3A6;};}}_LL3A5: {
struct _tuple0*old_name=_tmp8C7->name;struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp8C7->name=new_name;_tmp8C7->initializer=(struct Cyc_Absyn_Exp*)_tmp8B3;{
struct Cyc_Absyn_Var_d_struct _tmpDB6;struct Cyc_Absyn_Var_d_struct*_tmpDB5;_tmp88A->r=(
void*)((_tmpDB5=_cycalloc(sizeof(*_tmpDB5)),((_tmpDB5[0]=((_tmpDB6.tag=0,((
_tmpDB6.f1=_tmp8C7,_tmpDB6)))),_tmpDB5))));}{struct _RegionHandle _tmp8CE=
_new_region("temp");struct _RegionHandle*temp=& _tmp8CE;_push_region(temp);{struct
Cyc_Absyn_Local_b_struct _tmpDB9;struct Cyc_Absyn_Local_b_struct*_tmpDB8;struct Cyc_Toc_Env*
_tmp8CF=Cyc_Toc_add_varmap(temp,nv,old_name,Cyc_Absyn_varb_exp(new_name,(void*)((
_tmpDB8=_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8[0]=((_tmpDB9.tag=4,((_tmpDB9.f1=
_tmp8C7,_tmpDB9)))),_tmpDB8)))),0));Cyc_Toc_local_decl_to_c(_tmp8CF,nv,_tmp8C7,
_tmp88B);}_npop_handler(0);goto _LL3A3;;_pop_region(temp);};}_LL3A6:;_LL3A7: s->r=(
Cyc_Toc_letdecl_to_c(nv,_tmp8B2,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp8B3->topt))->v,
_tmp8B3,_tmp88B))->r;goto _LL3A3;_LL3A3:;}goto _LL394;_LL399: {struct Cyc_Absyn_Letv_d_struct*
_tmp8B4=(struct Cyc_Absyn_Letv_d_struct*)_tmp8AE;if(_tmp8B4->tag != 3)goto _LL39B;
else{_tmp8B5=_tmp8B4->f1;}}_LL39A: {struct Cyc_List_List*_tmp8D2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp8B5);if(_tmp8D2 == 0){const char*_tmpDBC;
void*_tmpDBB;(_tmpDBB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpDBC="empty Letv_d",_tag_dyneither(_tmpDBC,sizeof(char),
13))),_tag_dyneither(_tmpDBB,sizeof(void*),0)));}{struct Cyc_Absyn_Var_d_struct
_tmpDBF;struct Cyc_Absyn_Var_d_struct*_tmpDBE;_tmp88A->r=(void*)((_tmpDBE=
_cycalloc(sizeof(*_tmpDBE)),((_tmpDBE[0]=((_tmpDBF.tag=0,((_tmpDBF.f1=(struct Cyc_Absyn_Vardecl*)
_tmp8D2->hd,_tmpDBF)))),_tmpDBE))));}_tmp8D2=_tmp8D2->tl;for(0;_tmp8D2 != 0;
_tmp8D2=_tmp8D2->tl){struct Cyc_Absyn_Var_d_struct _tmpDC2;struct Cyc_Absyn_Var_d_struct*
_tmpDC1;struct Cyc_Absyn_Decl*_tmp8D7=Cyc_Absyn_new_decl((void*)((_tmpDC1=
_cycalloc(sizeof(*_tmpDC1)),((_tmpDC1[0]=((_tmpDC2.tag=0,((_tmpDC2.f1=(struct Cyc_Absyn_Vardecl*)
_tmp8D2->hd,_tmpDC2)))),_tmpDC1)))),0);s->r=(Cyc_Absyn_decl_stmt(_tmp8D7,Cyc_Absyn_new_stmt(
s->r,0),0))->r;}Cyc_Toc_stmt_to_c(nv,s);goto _LL394;}_LL39B: {struct Cyc_Absyn_Fn_d_struct*
_tmp8B6=(struct Cyc_Absyn_Fn_d_struct*)_tmp8AE;if(_tmp8B6->tag != 1)goto _LL39D;
else{_tmp8B7=_tmp8B6->f1;}}_LL39C: {struct _tuple0*_tmp8DA=_tmp8B7->name;struct
_RegionHandle _tmp8DB=_new_region("temp");struct _RegionHandle*temp=& _tmp8DB;
_push_region(temp);{struct Cyc_Toc_Env*_tmp8DC=Cyc_Toc_add_varmap(temp,nv,_tmp8B7->name,
Cyc_Absyn_var_exp(_tmp8DA,0));Cyc_Toc_fndecl_to_c(_tmp8DC,_tmp8B7,0);Cyc_Toc_stmt_to_c(
_tmp8DC,_tmp88B);}_npop_handler(0);goto _LL394;;_pop_region(temp);}_LL39D: {
struct Cyc_Absyn_Region_d_struct*_tmp8B8=(struct Cyc_Absyn_Region_d_struct*)
_tmp8AE;if(_tmp8B8->tag != 4)goto _LL39F;else{_tmp8B9=_tmp8B8->f1;_tmp8BA=_tmp8B8->f2;
_tmp8BB=_tmp8B8->f3;_tmp8BC=_tmp8B8->f4;}}_LL39E: {struct Cyc_Absyn_Stmt*_tmp8DD=
_tmp88B;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple0*
rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp8BA->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct _RegionHandle _tmp8DE=_new_region("temp");struct _RegionHandle*temp=&
_tmp8DE;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,
x_exp),_tmp8DD);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp8DD,0))->r;
else{if(_tmp8BC == 0){struct Cyc_Absyn_Exp*_tmpDC5[1];struct Cyc_Absyn_Exp*_tmpDC4[
1];struct Cyc_List_List*_tmpDC3;s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpDC3=
_cycalloc(sizeof(*_tmpDC3)),((_tmpDC3->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0),((_tmpDC3->tl=0,_tmpDC3)))))),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpDC4[0]=
x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDC4,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp8DD,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpDC5[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDC5,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp8BC);{struct Cyc_Absyn_Exp*
_tmpDC7[1];struct Cyc_Absyn_Exp*_tmpDC6[2];s->r=(Cyc_Absyn_declare_stmt(rh_var,
Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpDC6[1]=(struct Cyc_Absyn_Exp*)_tmp8BC,((_tmpDC6[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDC6,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp8DD,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpDC7[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDC7,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
_npop_handler(0);return;;_pop_region(temp);}_LL39F: {struct Cyc_Absyn_Alias_d_struct*
_tmp8BD=(struct Cyc_Absyn_Alias_d_struct*)_tmp8AE;if(_tmp8BD->tag != 5)goto _LL3A1;
else{_tmp8BE=_tmp8BD->f1;_tmp8BF=_tmp8BD->f2;_tmp8C0=_tmp8BD->f3;}}_LL3A0: {
struct _tuple0*old_name=_tmp8C0->name;struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp8C0->name=new_name;_tmp8C0->initializer=(struct Cyc_Absyn_Exp*)_tmp8BE;{
struct Cyc_Absyn_Decl_s_struct _tmpDD6;struct Cyc_Absyn_Var_d_struct*_tmpDD5;struct
Cyc_Absyn_Var_d_struct _tmpDD4;struct Cyc_Absyn_Decl*_tmpDD3;struct Cyc_Absyn_Decl_s_struct*
_tmpDD2;s->r=(void*)((_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2[0]=((_tmpDD6.tag=
12,((_tmpDD6.f1=((_tmpDD3=_cycalloc(sizeof(*_tmpDD3)),((_tmpDD3->r=(void*)((
_tmpDD5=_cycalloc(sizeof(*_tmpDD5)),((_tmpDD5[0]=((_tmpDD4.tag=0,((_tmpDD4.f1=
_tmp8C0,_tmpDD4)))),_tmpDD5)))),((_tmpDD3->loc=0,_tmpDD3)))))),((_tmpDD6.f2=
_tmp88B,_tmpDD6)))))),_tmpDD2))));}{struct _RegionHandle _tmp8E9=_new_region("temp");
struct _RegionHandle*temp=& _tmp8E9;_push_region(temp);{struct Cyc_Absyn_Local_b_struct
_tmpDD9;struct Cyc_Absyn_Local_b_struct*_tmpDD8;struct Cyc_Toc_Env*_tmp8EA=Cyc_Toc_add_varmap(
temp,nv,old_name,Cyc_Absyn_varb_exp(new_name,(void*)((_tmpDD8=_cycalloc(sizeof(*
_tmpDD8)),((_tmpDD8[0]=((_tmpDD9.tag=4,((_tmpDD9.f1=_tmp8C0,_tmpDD9)))),_tmpDD8)))),
0));Cyc_Toc_local_decl_to_c(_tmp8EA,nv,_tmp8C0,_tmp88B);}_npop_handler(0);
return;;_pop_region(temp);};}_LL3A1:;_LL3A2: {const char*_tmpDDC;void*_tmpDDB;(
_tmpDDB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpDDC="bad nested declaration within function",_tag_dyneither(_tmpDDC,sizeof(
char),39))),_tag_dyneither(_tmpDDB,sizeof(void*),0)));}_LL394:;}return;_LL38C: {
struct Cyc_Absyn_Label_s_struct*_tmp88C=(struct Cyc_Absyn_Label_s_struct*)_tmp865;
if(_tmp88C->tag != 13)goto _LL38E;else{_tmp88D=_tmp88C->f1;_tmp88E=_tmp88C->f2;}}
_LL38D: s=_tmp88E;continue;_LL38E: {struct Cyc_Absyn_Do_s_struct*_tmp88F=(struct
Cyc_Absyn_Do_s_struct*)_tmp865;if(_tmp88F->tag != 14)goto _LL390;else{_tmp890=
_tmp88F->f1;_tmp891=_tmp88F->f2;_tmp892=_tmp891.f1;}}_LL38F: {struct
_RegionHandle _tmp8EF=_new_region("temp");struct _RegionHandle*temp=& _tmp8EF;
_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp890);Cyc_Toc_exp_to_c(
nv,_tmp892);_npop_handler(0);return;;_pop_region(temp);}_LL390: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp893=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp865;if(_tmp893->tag != 15)goto
_LL392;else{_tmp894=_tmp893->f1;_tmp895=_tmp893->f2;}}_LL391: {struct _tuple0*
h_var=Cyc_Toc_temp_var();struct _tuple0*e_var=Cyc_Toc_temp_var();struct _tuple0*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);{struct Cyc_Core_Opt*_tmpDDD;
e_exp->topt=((_tmpDDD=_cycalloc(sizeof(*_tmpDDD)),((_tmpDDD->v=e_typ,_tmpDDD))));}{
struct _RegionHandle _tmp8F1=_new_region("temp");struct _RegionHandle*temp=& _tmp8F1;
_push_region(temp);{struct Cyc_Toc_Env*_tmp8F2=Cyc_Toc_add_varmap(temp,nv,e_var,
e_exp);Cyc_Toc_stmt_to_c(_tmp8F2,_tmp894);{struct Cyc_Absyn_Stmt*_tmp8F3=Cyc_Absyn_seq_stmt(
_tmp894,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
struct _tuple0*_tmp8F4=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp8F5=Cyc_Absyn_var_exp(
_tmp8F4,0);struct Cyc_Absyn_Vardecl*_tmp8F6=Cyc_Absyn_new_vardecl(_tmp8F4,Cyc_Absyn_exn_typ(),
0);{struct Cyc_Core_Opt*_tmpDDE;_tmp8F5->topt=((_tmpDDE=_cycalloc(sizeof(*_tmpDDE)),((
_tmpDDE->v=Cyc_Absyn_exn_typ(),_tmpDDE))));}{struct Cyc_Core_Opt*_tmpDEF;struct
Cyc_Absyn_Var_p_struct*_tmpDEE;struct Cyc_Absyn_Pat*_tmpDED;struct Cyc_Core_Opt*
_tmpDEC;struct Cyc_Absyn_Var_p_struct _tmpDEB;struct Cyc_Absyn_Pat*_tmpDEA;struct
Cyc_Absyn_Pat*_tmp8F8=(_tmpDEA=_cycalloc(sizeof(*_tmpDEA)),((_tmpDEA->r=(void*)((
_tmpDEE=_cycalloc(sizeof(*_tmpDEE)),((_tmpDEE[0]=((_tmpDEB.tag=1,((_tmpDEB.f1=
_tmp8F6,((_tmpDEB.f2=((_tmpDED=_cycalloc(sizeof(*_tmpDED)),((_tmpDED->r=(void*)&
Cyc_Absyn_Wild_p_val,((_tmpDED->topt=((_tmpDEC=_cycalloc(sizeof(*_tmpDEC)),((
_tmpDEC->v=Cyc_Absyn_exn_typ(),_tmpDEC)))),((_tmpDED->loc=0,_tmpDED)))))))),
_tmpDEB)))))),_tmpDEE)))),((_tmpDEA->topt=((_tmpDEF=_cycalloc(sizeof(*_tmpDEF)),((
_tmpDEF->v=Cyc_Absyn_exn_typ(),_tmpDEF)))),((_tmpDEA->loc=0,_tmpDEA)))))));
struct Cyc_Absyn_Exp*_tmp8F9=Cyc_Absyn_throw_exp(_tmp8F5,0);{struct Cyc_Core_Opt*
_tmpDF0;_tmp8F9->topt=((_tmpDF0=_cycalloc(sizeof(*_tmpDF0)),((_tmpDF0->v=(void*)&
Cyc_Absyn_VoidType_val,_tmpDF0))));}{struct Cyc_Absyn_Stmt*_tmp8FB=Cyc_Absyn_exp_stmt(
_tmp8F9,0);struct Cyc_Core_Opt*_tmpDF6;struct Cyc_List_List*_tmpDF5;struct Cyc_Absyn_Switch_clause*
_tmpDF4;struct Cyc_Absyn_Switch_clause*_tmp8FC=(_tmpDF4=_cycalloc(sizeof(*_tmpDF4)),((
_tmpDF4->pattern=_tmp8F8,((_tmpDF4->pat_vars=((_tmpDF6=_cycalloc(sizeof(*_tmpDF6)),((
_tmpDF6->v=((_tmpDF5=_cycalloc(sizeof(*_tmpDF5)),((_tmpDF5->hd=_tmp8F6,((_tmpDF5->tl=
0,_tmpDF5)))))),_tmpDF6)))),((_tmpDF4->where_clause=0,((_tmpDF4->body=_tmp8FB,((
_tmpDF4->loc=0,_tmpDF4)))))))))));struct Cyc_List_List*_tmpDF7;struct Cyc_Absyn_Stmt*
_tmp8FD=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp895,((_tmpDF7=_cycalloc(sizeof(*
_tmpDF7)),((_tmpDF7->hd=_tmp8FC,((_tmpDF7->tl=0,_tmpDF7))))))),0);Cyc_Toc_stmt_to_c(
_tmp8F2,_tmp8FD);{struct Cyc_List_List*_tmpDF8;struct Cyc_Absyn_Exp*_tmp8FE=Cyc_Absyn_fncall_exp(
Cyc_Toc_setjmp_e,((_tmpDF8=_cycalloc(sizeof(*_tmpDF8)),((_tmpDF8->hd=Cyc_Absyn_aggrmember_exp(
h_exp,Cyc_Toc_handler_sp,0),((_tmpDF8->tl=0,_tmpDF8)))))),0);struct Cyc_List_List*
_tmpDF9;struct Cyc_Absyn_Stmt*_tmp8FF=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpDF9=_cycalloc(sizeof(*_tmpDF9)),((_tmpDF9->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpDF9->tl=0,_tmpDF9)))))),0),0);struct Cyc_Absyn_Exp*_tmp900=Cyc_Absyn_int_exp(
Cyc_Absyn_Signed,0,0);struct Cyc_Absyn_Exp*_tmp901=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
1,0);s->r=(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp8FF,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp900,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp8FE,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp901,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp8F3,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp8FD,0),0),0),0),0),0))->r;};};};};}
_npop_handler(0);return;;_pop_region(temp);};}_LL392: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp896=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp865;if(_tmp896->tag != 16)goto
_LL371;else{_tmp897=_tmp896->f1;}}_LL393: if(Cyc_Absyn_no_regions)s->r=(void*)&
Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp897);{struct Cyc_List_List*
_tmpDFA;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((
_tmpDFA=_cycalloc(sizeof(*_tmpDFA)),((_tmpDFA->hd=_tmp897,((_tmpDFA->tl=0,
_tmpDFA)))))),0));};}return;_LL371:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);struct _tuple27{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp90F=_new_region("frgn");struct _RegionHandle*frgn=& _tmp90F;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp910=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp911=f->args;for(0;_tmp911 != 0;_tmp911=_tmp911->tl){struct
_tuple0*_tmpDFB;struct _tuple0*_tmp912=(_tmpDFB=_cycalloc(sizeof(*_tmpDFB)),((
_tmpDFB->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpDFB->f2=(*((struct
_tuple27*)_tmp911->hd)).f1,_tmpDFB)))));(*((struct _tuple27*)_tmp911->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple27*)_tmp911->hd)).f3);_tmp910=Cyc_Toc_add_varmap(frgn,_tmp910,
_tmp912,Cyc_Absyn_var_exp(_tmp912,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp915;struct Cyc_Absyn_Tqual
_tmp916;void*_tmp917;int _tmp918;struct Cyc_Absyn_VarargInfo _tmp914=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp915=_tmp914.name;_tmp916=_tmp914.tq;_tmp917=
_tmp914.type;_tmp918=_tmp914.inject;{void*_tmp919=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp917,(void*)& Cyc_Absyn_HeapRgn_val,_tmp916,Cyc_Absyn_false_conref));struct
_tuple0*_tmpDFC;struct _tuple0*_tmp91A=(_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((
_tmpDFC->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpDFC->f2=(struct
_dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null(_tmp915))->v,_tmpDFC)))));{
struct _tuple27*_tmpDFF;struct Cyc_List_List*_tmpDFE;f->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpDFE=
_cycalloc(sizeof(*_tmpDFE)),((_tmpDFE->hd=((_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((
_tmpDFF->f1=(struct _dyneither_ptr*)_tmp915->v,((_tmpDFF->f2=_tmp916,((_tmpDFF->f3=
_tmp919,_tmpDFF)))))))),((_tmpDFE->tl=0,_tmpDFE)))))));}_tmp910=Cyc_Toc_add_varmap(
frgn,_tmp910,_tmp91A,Cyc_Absyn_var_exp(_tmp91A,0));f->cyc_varargs=0;};}{struct
Cyc_List_List*_tmp91E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp91E != 0;_tmp91E=_tmp91E->tl){((struct Cyc_Absyn_Vardecl*)_tmp91E->hd)->type=
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp91E->hd)->type);}}Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel(frgn,_tmp910),f->body);};_pop_region(frgn);};}static enum 
Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){switch(s){case Cyc_Absyn_Abstract:
_LL3A8: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3A9: return Cyc_Absyn_Extern;
default: _LL3AA: return s;}}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad,int add_to_decls){struct _tuple0*_tmp91F=ad->name;struct _DynRegionHandle*
_tmp921;struct Cyc_Dict_Dict*_tmp922;struct Cyc_Toc_TocState _tmp920=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp921=_tmp920.dyn;_tmp922=_tmp920.aggrs_so_far;{
struct _DynRegionFrame _tmp923;struct _RegionHandle*d=_open_dynregion(& _tmp923,
_tmp921);{int seen_defn_before;struct _tuple15**_tmp924=((struct _tuple15**(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp922,_tmp91F);if(
_tmp924 == 0){seen_defn_before=0;{struct _tuple15*v;if(ad->kind == Cyc_Absyn_StructA){
struct _tuple15*_tmpE00;v=((_tmpE00=_region_malloc(d,sizeof(*_tmpE00)),((_tmpE00->f1=
ad,((_tmpE00->f2=Cyc_Absyn_strctq(_tmp91F),_tmpE00))))));}else{struct _tuple15*
_tmpE01;v=((_tmpE01=_region_malloc(d,sizeof(*_tmpE01)),((_tmpE01->f1=ad,((
_tmpE01->f2=Cyc_Absyn_unionq_typ(_tmp91F),_tmpE01))))));}*_tmp922=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp922,
_tmp91F,v);};}else{struct _tuple15 _tmp928;struct Cyc_Absyn_Aggrdecl*_tmp929;void*
_tmp92A;struct _tuple15*_tmp927=*_tmp924;_tmp928=*_tmp927;_tmp929=_tmp928.f1;
_tmp92A=_tmp928.f2;if(_tmp929->impl == 0){{struct _tuple15*_tmpE02;*_tmp922=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))
Cyc_Dict_insert)(*_tmp922,_tmp91F,((_tmpE02=_region_malloc(d,sizeof(*_tmpE02)),((
_tmpE02->f1=ad,((_tmpE02->f2=_tmp92A,_tmpE02)))))));}seen_defn_before=0;}else{
seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*_tmpE03;struct Cyc_Absyn_Aggrdecl*
new_ad=(_tmpE03=_cycalloc(sizeof(*_tmpE03)),((_tmpE03->kind=ad->kind,((_tmpE03->sc=
Cyc_Absyn_Public,((_tmpE03->name=ad->name,((_tmpE03->tvs=0,((_tmpE03->impl=0,((
_tmpE03->attributes=ad->attributes,_tmpE03)))))))))))));if(ad->impl != 0  && !
seen_defn_before){{struct Cyc_Absyn_AggrdeclImpl*_tmpE04;new_ad->impl=((_tmpE04=
_cycalloc(sizeof(*_tmpE04)),((_tmpE04->exist_vars=0,((_tmpE04->rgn_po=0,((
_tmpE04->fields=0,((_tmpE04->tagged=0,_tmpE04))))))))));}{struct Cyc_List_List*
new_fields=0;{struct Cyc_List_List*_tmp92D=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp92D != 0;_tmp92D=_tmp92D->tl){struct Cyc_Absyn_Aggrfield*
_tmp92E=(struct Cyc_Absyn_Aggrfield*)_tmp92D->hd;if(_tmp92D->tl == 0  && Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp92E->type)))continue;{struct Cyc_Absyn_Aggrfield*
_tmpE05;struct Cyc_Absyn_Aggrfield*_tmp92F=(_tmpE05=_cycalloc(sizeof(*_tmpE05)),((
_tmpE05->name=_tmp92E->name,((_tmpE05->tq=Cyc_Toc_mt_tq,((_tmpE05->type=Cyc_Toc_typ_to_c_array(
_tmp92E->type),((_tmpE05->width=_tmp92E->width,((_tmpE05->attributes=_tmp92E->attributes,
_tmpE05)))))))))));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp930=_tmp92F->type;struct _dyneither_ptr*_tmp931=_tmp92F->name;const char*
_tmpE0A;void*_tmpE09[2];struct Cyc_String_pa_struct _tmpE08;struct Cyc_String_pa_struct
_tmpE07;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpE07.tag=0,((_tmpE07.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp931),((_tmpE08.tag=0,((_tmpE08.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpE09[0]=&
_tmpE08,((_tmpE09[1]=& _tmpE07,Cyc_aprintf(((_tmpE0A="_union_%s_%s",
_tag_dyneither(_tmpE0A,sizeof(char),13))),_tag_dyneither(_tmpE09,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpE0B;struct _dyneither_ptr*str=(_tmpE0B=_cycalloc(sizeof(*
_tmpE0B)),((_tmpE0B[0]=s,_tmpE0B)));struct Cyc_Absyn_Aggrfield*_tmpE0C;struct Cyc_Absyn_Aggrfield*
_tmp932=(_tmpE0C=_cycalloc(sizeof(*_tmpE0C)),((_tmpE0C->name=Cyc_Toc_val_sp,((
_tmpE0C->tq=Cyc_Toc_mt_tq,((_tmpE0C->type=_tmp930,((_tmpE0C->width=0,((_tmpE0C->attributes=
0,_tmpE0C)))))))))));struct Cyc_Absyn_Aggrfield*_tmpE0D;struct Cyc_Absyn_Aggrfield*
_tmp933=(_tmpE0D=_cycalloc(sizeof(*_tmpE0D)),((_tmpE0D->name=Cyc_Toc_tag_sp,((
_tmpE0D->tq=Cyc_Toc_mt_tq,((_tmpE0D->type=Cyc_Absyn_sint_typ,((_tmpE0D->width=0,((
_tmpE0D->attributes=0,_tmpE0D)))))))))));struct Cyc_Absyn_Aggrfield*_tmpE0E[2];
struct Cyc_List_List*_tmp934=(_tmpE0E[1]=_tmp932,((_tmpE0E[0]=_tmp933,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpE0E,sizeof(struct Cyc_Absyn_Aggrfield*),
2)))));struct Cyc_Absyn_AggrdeclImpl*_tmpE13;struct _tuple0*_tmpE12;struct Cyc_Absyn_Aggrdecl*
_tmpE11;struct Cyc_Absyn_Aggrdecl*_tmp935=(_tmpE11=_cycalloc(sizeof(*_tmpE11)),((
_tmpE11->kind=Cyc_Absyn_StructA,((_tmpE11->sc=Cyc_Absyn_Public,((_tmpE11->name=((
_tmpE12=_cycalloc(sizeof(*_tmpE12)),((_tmpE12->f1=Cyc_Absyn_Loc_n,((_tmpE12->f2=
str,_tmpE12)))))),((_tmpE11->tvs=0,((_tmpE11->impl=((_tmpE13=_cycalloc(sizeof(*
_tmpE13)),((_tmpE13->exist_vars=0,((_tmpE13->rgn_po=0,((_tmpE13->fields=_tmp934,((
_tmpE13->tagged=0,_tmpE13)))))))))),((_tmpE11->attributes=0,_tmpE11)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmpE19;struct Cyc_Absyn_Aggr_d_struct _tmpE18;
struct Cyc_List_List*_tmpE17;Cyc_Toc_result_decls=((_tmpE17=_cycalloc(sizeof(*
_tmpE17)),((_tmpE17->hd=Cyc_Absyn_new_decl((void*)((_tmpE19=_cycalloc(sizeof(*
_tmpE19)),((_tmpE19[0]=((_tmpE18.tag=6,((_tmpE18.f1=_tmp935,_tmpE18)))),_tmpE19)))),
0),((_tmpE17->tl=Cyc_Toc_result_decls,_tmpE17))))));}_tmp92F->type=Cyc_Absyn_strct(
str);}{struct Cyc_List_List*_tmpE1A;new_fields=((_tmpE1A=_cycalloc(sizeof(*
_tmpE1A)),((_tmpE1A->hd=_tmp92F,((_tmpE1A->tl=new_fields,_tmpE1A))))));};};}}(
new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_fields);};}if(add_to_decls){struct Cyc_Absyn_Decl*_tmpE24;struct Cyc_Absyn_Aggr_d_struct
_tmpE23;struct Cyc_Absyn_Aggr_d_struct*_tmpE22;struct Cyc_List_List*_tmpE21;Cyc_Toc_result_decls=((
_tmpE21=_cycalloc(sizeof(*_tmpE21)),((_tmpE21->hd=((_tmpE24=_cycalloc(sizeof(*
_tmpE24)),((_tmpE24->r=(void*)((_tmpE22=_cycalloc(sizeof(*_tmpE22)),((_tmpE22[0]=((
_tmpE23.tag=6,((_tmpE23.f1=new_ad,_tmpE23)))),_tmpE22)))),((_tmpE24->loc=0,
_tmpE24)))))),((_tmpE21->tl=Cyc_Toc_result_decls,_tmpE21))))));}};};
_pop_dynregion(d);};}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud){struct _DynRegionHandle*_tmp94C;struct Cyc_Set_Set**_tmp94D;struct Cyc_Toc_TocState
_tmp94B=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp94C=
_tmp94B.dyn;_tmp94D=_tmp94B.datatypes_so_far;{struct _DynRegionFrame _tmp94E;
struct _RegionHandle*d=_open_dynregion(& _tmp94E,_tmp94C);{struct _tuple0*_tmp94F=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))
Cyc_Set_member)(*_tmp94D,_tmp94F)){_npop_handler(0);return;}*_tmp94D=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*
_tmp94D,_tmp94F);}{struct Cyc_List_List*_tmp950=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp950 != 0;_tmp950=_tmp950->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp950->hd;struct Cyc_List_List*_tmp951=0;int i=
1;{struct Cyc_List_List*_tmp952=f->typs;for(0;_tmp952 != 0;(_tmp952=_tmp952->tl,i
++)){struct _dyneither_ptr*_tmp953=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpE25;struct Cyc_Absyn_Aggrfield*_tmp954=(_tmpE25=_cycalloc(sizeof(*_tmpE25)),((
_tmpE25->name=_tmp953,((_tmpE25->tq=(*((struct _tuple10*)_tmp952->hd)).f1,((
_tmpE25->type=Cyc_Toc_typ_to_c_array((*((struct _tuple10*)_tmp952->hd)).f2),((
_tmpE25->width=0,((_tmpE25->attributes=0,_tmpE25)))))))))));struct Cyc_List_List*
_tmpE26;_tmp951=((_tmpE26=_cycalloc(sizeof(*_tmpE26)),((_tmpE26->hd=_tmp954,((
_tmpE26->tl=_tmp951,_tmpE26))))));}}{struct Cyc_Absyn_Aggrfield*_tmpE29;struct Cyc_List_List*
_tmpE28;_tmp951=((_tmpE28=_cycalloc(sizeof(*_tmpE28)),((_tmpE28->hd=((_tmpE29=
_cycalloc(sizeof(*_tmpE29)),((_tmpE29->name=Cyc_Toc_tag_sp,((_tmpE29->tq=Cyc_Toc_mt_tq,((
_tmpE29->type=Cyc_Absyn_sint_typ,((_tmpE29->width=0,((_tmpE29->attributes=0,
_tmpE29)))))))))))),((_tmpE28->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp951),_tmpE28))))));}{struct Cyc_Absyn_AggrdeclImpl*
_tmpE2E;const char*_tmpE2D;struct Cyc_Absyn_Aggrdecl*_tmpE2C;struct Cyc_Absyn_Aggrdecl*
_tmp959=(_tmpE2C=_cycalloc(sizeof(*_tmpE2C)),((_tmpE2C->kind=Cyc_Absyn_StructA,((
_tmpE2C->sc=Cyc_Absyn_Public,((_tmpE2C->name=Cyc_Toc_collapse_qvar_tag(f->name,((
_tmpE2D="_struct",_tag_dyneither(_tmpE2D,sizeof(char),8)))),((_tmpE2C->tvs=0,((
_tmpE2C->impl=((_tmpE2E=_cycalloc(sizeof(*_tmpE2E)),((_tmpE2E->exist_vars=0,((
_tmpE2E->rgn_po=0,((_tmpE2E->fields=_tmp951,((_tmpE2E->tagged=0,_tmpE2E)))))))))),((
_tmpE2C->attributes=0,_tmpE2C)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpE34;
struct Cyc_Absyn_Aggr_d_struct _tmpE33;struct Cyc_List_List*_tmpE32;Cyc_Toc_result_decls=((
_tmpE32=_cycalloc(sizeof(*_tmpE32)),((_tmpE32->hd=Cyc_Absyn_new_decl((void*)((
_tmpE34=_cycalloc(sizeof(*_tmpE34)),((_tmpE34[0]=((_tmpE33.tag=6,((_tmpE33.f1=
_tmp959,_tmpE33)))),_tmpE34)))),0),((_tmpE32->tl=Cyc_Toc_result_decls,_tmpE32))))));};}};;
_pop_dynregion(d);};}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd){if(xd->fields == 0)return;{struct _DynRegionHandle*_tmp961;struct Cyc_Dict_Dict*
_tmp962;struct Cyc_Toc_TocState _tmp960=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp961=_tmp960.dyn;_tmp962=_tmp960.xdatatypes_so_far;{struct _DynRegionFrame
_tmp963;struct _RegionHandle*d=_open_dynregion(& _tmp963,_tmp961);{struct _tuple0*
_tmp964=xd->name;struct Cyc_List_List*_tmp965=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp965 != 0;_tmp965=_tmp965->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp965->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp966=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp967=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp966,Cyc_Absyn_false_conref,0);int*_tmp968=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp962,f->name);int
_tmp969;_LL3AD: if(_tmp968 != 0)goto _LL3AF;_LL3AE: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp96A=Cyc_Absyn_new_vardecl(f->name,_tmp967,
initopt);_tmp96A->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*_tmpE3A;struct Cyc_Absyn_Var_d_struct
_tmpE39;struct Cyc_List_List*_tmpE38;Cyc_Toc_result_decls=((_tmpE38=_cycalloc(
sizeof(*_tmpE38)),((_tmpE38->hd=Cyc_Absyn_new_decl((void*)((_tmpE3A=_cycalloc(
sizeof(*_tmpE3A)),((_tmpE3A[0]=((_tmpE39.tag=0,((_tmpE39.f1=_tmp96A,_tmpE39)))),
_tmpE3A)))),0),((_tmpE38->tl=Cyc_Toc_result_decls,_tmpE38))))));}*_tmp962=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp962,f->name,f->sc != Cyc_Absyn_Extern);{struct Cyc_List_List*fields=0;int i=1;{
struct Cyc_List_List*_tmp96E=f->typs;for(0;_tmp96E != 0;(_tmp96E=_tmp96E->tl,i ++)){
struct Cyc_Int_pa_struct _tmpE42;void*_tmpE41[1];const char*_tmpE40;struct
_dyneither_ptr*_tmpE3F;struct _dyneither_ptr*_tmp96F=(_tmpE3F=_cycalloc(sizeof(*
_tmpE3F)),((_tmpE3F[0]=(struct _dyneither_ptr)((_tmpE42.tag=1,((_tmpE42.f1=(
unsigned long)i,((_tmpE41[0]=& _tmpE42,Cyc_aprintf(((_tmpE40="f%d",_tag_dyneither(
_tmpE40,sizeof(char),4))),_tag_dyneither(_tmpE41,sizeof(void*),1)))))))),_tmpE3F)));
struct Cyc_Absyn_Aggrfield*_tmpE43;struct Cyc_Absyn_Aggrfield*_tmp970=(_tmpE43=
_cycalloc(sizeof(*_tmpE43)),((_tmpE43->name=_tmp96F,((_tmpE43->tq=(*((struct
_tuple10*)_tmp96E->hd)).f1,((_tmpE43->type=Cyc_Toc_typ_to_c_array((*((struct
_tuple10*)_tmp96E->hd)).f2),((_tmpE43->width=0,((_tmpE43->attributes=0,_tmpE43)))))))))));
struct Cyc_List_List*_tmpE44;fields=((_tmpE44=_cycalloc(sizeof(*_tmpE44)),((
_tmpE44->hd=_tmp970,((_tmpE44->tl=fields,_tmpE44))))));}}{struct Cyc_Absyn_Aggrfield*
_tmpE47;struct Cyc_List_List*_tmpE46;fields=((_tmpE46=_cycalloc(sizeof(*_tmpE46)),((
_tmpE46->hd=((_tmpE47=_cycalloc(sizeof(*_tmpE47)),((_tmpE47->name=Cyc_Toc_tag_sp,((
_tmpE47->tq=Cyc_Toc_mt_tq,((_tmpE47->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq),((_tmpE47->width=0,((_tmpE47->attributes=0,_tmpE47)))))))))))),((
_tmpE46->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmpE46))))));}{struct Cyc_Absyn_AggrdeclImpl*_tmpE4C;const char*_tmpE4B;
struct Cyc_Absyn_Aggrdecl*_tmpE4A;struct Cyc_Absyn_Aggrdecl*_tmp979=(_tmpE4A=
_cycalloc(sizeof(*_tmpE4A)),((_tmpE4A->kind=Cyc_Absyn_StructA,((_tmpE4A->sc=Cyc_Absyn_Public,((
_tmpE4A->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpE4B="_struct",
_tag_dyneither(_tmpE4B,sizeof(char),8)))),((_tmpE4A->tvs=0,((_tmpE4A->impl=((
_tmpE4C=_cycalloc(sizeof(*_tmpE4C)),((_tmpE4C->exist_vars=0,((_tmpE4C->rgn_po=0,((
_tmpE4C->fields=fields,((_tmpE4C->tagged=0,_tmpE4C)))))))))),((_tmpE4A->attributes=
0,_tmpE4A)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpE52;struct Cyc_Absyn_Aggr_d_struct
_tmpE51;struct Cyc_List_List*_tmpE50;Cyc_Toc_result_decls=((_tmpE50=_cycalloc(
sizeof(*_tmpE50)),((_tmpE50->hd=Cyc_Absyn_new_decl((void*)((_tmpE52=_cycalloc(
sizeof(*_tmpE52)),((_tmpE52[0]=((_tmpE51.tag=6,((_tmpE51.f1=_tmp979,_tmpE51)))),
_tmpE52)))),0),((_tmpE50->tl=Cyc_Toc_result_decls,_tmpE50))))));}goto _LL3AC;};};};}
_LL3AF: if(_tmp968 == 0)goto _LL3B1;_tmp969=*_tmp968;if(_tmp969 != 0)goto _LL3B1;
_LL3B0: if(f->sc != Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*_tmp980=Cyc_Absyn_string_exp(*
fn,0);struct Cyc_Absyn_Vardecl*_tmp981=Cyc_Absyn_new_vardecl(f->name,_tmp967,(
struct Cyc_Absyn_Exp*)_tmp980);_tmp981->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*
_tmpE58;struct Cyc_Absyn_Var_d_struct _tmpE57;struct Cyc_List_List*_tmpE56;Cyc_Toc_result_decls=((
_tmpE56=_cycalloc(sizeof(*_tmpE56)),((_tmpE56->hd=Cyc_Absyn_new_decl((void*)((
_tmpE58=_cycalloc(sizeof(*_tmpE58)),((_tmpE58[0]=((_tmpE57.tag=0,((_tmpE57.f1=
_tmp981,_tmpE57)))),_tmpE58)))),0),((_tmpE56->tl=Cyc_Toc_result_decls,_tmpE56))))));}*
_tmp962=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp962,f->name,1);}goto _LL3AC;_LL3B1:;_LL3B2: goto _LL3AC;_LL3AC:;}};
_pop_dynregion(d);};};}static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*
ed){ed->sc=Cyc_Absyn_Public;if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(
struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,
struct Cyc_Absyn_Stmt*s){void*old_typ=vd->type;vd->type=Cyc_Toc_typ_to_c_array(
old_typ);if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ))vd->sc=Cyc_Absyn_Public;Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp985=init->r;struct Cyc_Absyn_Vardecl*_tmp987;struct Cyc_Absyn_Exp*_tmp988;
struct Cyc_Absyn_Exp*_tmp989;int _tmp98A;_LL3B4: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp986=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp985;if(_tmp986->tag != 28)
goto _LL3B6;else{_tmp987=_tmp986->f1;_tmp988=_tmp986->f2;_tmp989=_tmp986->f3;
_tmp98A=_tmp986->f4;}}_LL3B5: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp987,_tmp988,_tmp989,_tmp98A,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL3B3;_LL3B6:;_LL3B7: if(vd->sc == Cyc_Absyn_Static){struct
_RegionHandle _tmp98B=_new_region("temp");struct _RegionHandle*temp=& _tmp98B;
_push_region(temp);{struct Cyc_Toc_Env*_tmp98C=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp98C,init);};_pop_region(temp);}else{Cyc_Toc_exp_to_c(
init_nv,init);}goto _LL3B3;_LL3B3:;}else{void*_tmp98D=Cyc_Tcutil_compress(old_typ);
struct Cyc_Absyn_ArrayInfo _tmp98F;void*_tmp990;struct Cyc_Absyn_Exp*_tmp991;union
Cyc_Absyn_Constraint*_tmp992;_LL3B9:{struct Cyc_Absyn_ArrayType_struct*_tmp98E=(
struct Cyc_Absyn_ArrayType_struct*)_tmp98D;if(_tmp98E->tag != 8)goto _LL3BB;else{
_tmp98F=_tmp98E->f1;_tmp990=_tmp98F.elt_type;_tmp991=_tmp98F.num_elts;_tmp992=
_tmp98F.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp992))goto _LL3BB;_LL3BA: if(_tmp991 == 0){const char*_tmpE5B;void*_tmpE5A;(
_tmpE5A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE5B="can't initialize zero-terminated array -- size unknown",_tag_dyneither(
_tmpE5B,sizeof(char),55))),_tag_dyneither(_tmpE5A,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
num_elts=(struct Cyc_Absyn_Exp*)_tmp991;struct Cyc_Absyn_Exp*_tmp995=Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(
- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp996=Cyc_Absyn_signed_int_exp(0,0);s->r=Cyc_Toc_seq_stmt_r(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp995,_tmp996,0),0),Cyc_Absyn_new_stmt(
s->r,0));goto _LL3B8;};_LL3BB:;_LL3BC: goto _LL3B8;_LL3B8:;}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{struct _tuple0*x=Cyc_Toc_temp_var();
struct _RegionHandle _tmp997=_new_region("prgn");struct _RegionHandle*prgn=& _tmp997;
_push_region(prgn);{struct Cyc_Absyn_Stmt*_tmp998=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp999=Cyc_Toc_share_env(prgn,nv);struct Cyc_Toc_Env*_tmp99B;
struct Cyc_List_List*_tmp99C;struct Cyc_Absyn_Stmt*_tmp99D;struct _tuple23 _tmp99A=
Cyc_Toc_xlate_pat(_tmp999,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
Cyc_Toc_throw_match_stmt(),0);_tmp99B=_tmp99A.f1;_tmp99C=_tmp99A.f2;_tmp99D=
_tmp99A.f3;Cyc_Toc_stmt_to_c(_tmp99B,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(
t),(struct Cyc_Absyn_Exp*)e,Cyc_Absyn_seq_stmt(_tmp99D,s,0),0);for(0;_tmp99C != 0;
_tmp99C=_tmp99C->tl){struct _tuple24 _tmp99F;struct _tuple0*_tmp9A0;void*_tmp9A1;
struct _tuple24*_tmp99E=(struct _tuple24*)_tmp99C->hd;_tmp99F=*_tmp99E;_tmp9A0=
_tmp99F.f1;_tmp9A1=_tmp99F.f2;s=Cyc_Absyn_declare_stmt(_tmp9A0,_tmp9A1,0,s,0);}}{
struct Cyc_Absyn_Stmt*_tmp9A2=s;_npop_handler(0);return _tmp9A2;};;_pop_region(
prgn);};}static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){void*_tmp9A3=e->r;
struct Cyc_Absyn_Exp*_tmp9A5;struct Cyc_Absyn_Exp*_tmp9A7;struct Cyc_Absyn_Exp*
_tmp9A9;struct Cyc_Absyn_Exp*_tmp9AB;struct Cyc_Absyn_Exp*_tmp9AD;struct Cyc_Absyn_Exp*
_tmp9AF;struct Cyc_Absyn_Exp*_tmp9B1;struct Cyc_Absyn_Exp*_tmp9B3;struct Cyc_List_List*
_tmp9B5;struct Cyc_Absyn_Exp*_tmp9B7;struct Cyc_Absyn_Exp*_tmp9B8;struct Cyc_Absyn_Exp*
_tmp9BA;struct Cyc_Absyn_Exp*_tmp9BB;struct Cyc_Absyn_Exp*_tmp9BD;struct Cyc_Absyn_Exp*
_tmp9BE;struct Cyc_Absyn_Exp*_tmp9C0;struct Cyc_Absyn_Exp*_tmp9C1;struct Cyc_Absyn_Exp*
_tmp9C3;struct Cyc_Absyn_Exp*_tmp9C4;struct Cyc_Absyn_Exp*_tmp9C6;struct Cyc_Absyn_Exp*
_tmp9C7;struct Cyc_Absyn_Exp*_tmp9C9;struct Cyc_Absyn_Exp*_tmp9CA;struct Cyc_Absyn_Exp*
_tmp9CB;struct Cyc_Absyn_Exp*_tmp9CD;struct Cyc_List_List*_tmp9CE;struct Cyc_Absyn_Exp*
_tmp9D0;struct Cyc_List_List*_tmp9D1;void*_tmp9D3;void**_tmp9D4;struct Cyc_Absyn_Exp*
_tmp9D5;struct _tuple8*_tmp9D7;struct _tuple8 _tmp9D8;void*_tmp9D9;void**_tmp9DA;
struct Cyc_List_List*_tmp9DB;struct Cyc_List_List*_tmp9DD;struct Cyc_List_List*
_tmp9DF;void*_tmp9E1;void**_tmp9E2;void*_tmp9E4;void**_tmp9E5;struct Cyc_Absyn_Stmt*
_tmp9E7;struct Cyc_Absyn_MallocInfo _tmp9E9;struct Cyc_Absyn_MallocInfo*_tmp9EA;
_LL3BE: {struct Cyc_Absyn_Deref_e_struct*_tmp9A4=(struct Cyc_Absyn_Deref_e_struct*)
_tmp9A3;if(_tmp9A4->tag != 21)goto _LL3C0;else{_tmp9A5=_tmp9A4->f1;}}_LL3BF:
_tmp9A7=_tmp9A5;goto _LL3C1;_LL3C0: {struct Cyc_Absyn_AggrMember_e_struct*_tmp9A6=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp9A3;if(_tmp9A6->tag != 22)goto _LL3C2;
else{_tmp9A7=_tmp9A6->f1;}}_LL3C1: _tmp9A9=_tmp9A7;goto _LL3C3;_LL3C2: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp9A8=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp9A3;if(_tmp9A8->tag != 23)goto
_LL3C4;else{_tmp9A9=_tmp9A8->f1;}}_LL3C3: _tmp9AB=_tmp9A9;goto _LL3C5;_LL3C4: {
struct Cyc_Absyn_Address_e_struct*_tmp9AA=(struct Cyc_Absyn_Address_e_struct*)
_tmp9A3;if(_tmp9AA->tag != 16)goto _LL3C6;else{_tmp9AB=_tmp9AA->f1;}}_LL3C5:
_tmp9AD=_tmp9AB;goto _LL3C7;_LL3C6: {struct Cyc_Absyn_Throw_e_struct*_tmp9AC=(
struct Cyc_Absyn_Throw_e_struct*)_tmp9A3;if(_tmp9AC->tag != 12)goto _LL3C8;else{
_tmp9AD=_tmp9AC->f1;}}_LL3C7: _tmp9AF=_tmp9AD;goto _LL3C9;_LL3C8: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp9AE=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp9A3;if(_tmp9AE->tag != 13)
goto _LL3CA;else{_tmp9AF=_tmp9AE->f1;}}_LL3C9: _tmp9B1=_tmp9AF;goto _LL3CB;_LL3CA: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp9B0=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp9A3;if(_tmp9B0->tag != 19)goto _LL3CC;else{_tmp9B1=_tmp9B0->f1;}}_LL3CB:
_tmp9B3=_tmp9B1;goto _LL3CD;_LL3CC: {struct Cyc_Absyn_Increment_e_struct*_tmp9B2=(
struct Cyc_Absyn_Increment_e_struct*)_tmp9A3;if(_tmp9B2->tag != 5)goto _LL3CE;else{
_tmp9B3=_tmp9B2->f1;}}_LL3CD: Cyc_Toc_exptypes_to_c(_tmp9B3);goto _LL3BD;_LL3CE: {
struct Cyc_Absyn_Primop_e_struct*_tmp9B4=(struct Cyc_Absyn_Primop_e_struct*)
_tmp9A3;if(_tmp9B4->tag != 3)goto _LL3D0;else{_tmp9B5=_tmp9B4->f2;}}_LL3CF:((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,
_tmp9B5);goto _LL3BD;_LL3D0: {struct Cyc_Absyn_And_e_struct*_tmp9B6=(struct Cyc_Absyn_And_e_struct*)
_tmp9A3;if(_tmp9B6->tag != 7)goto _LL3D2;else{_tmp9B7=_tmp9B6->f1;_tmp9B8=_tmp9B6->f2;}}
_LL3D1: _tmp9BA=_tmp9B7;_tmp9BB=_tmp9B8;goto _LL3D3;_LL3D2: {struct Cyc_Absyn_Or_e_struct*
_tmp9B9=(struct Cyc_Absyn_Or_e_struct*)_tmp9A3;if(_tmp9B9->tag != 8)goto _LL3D4;
else{_tmp9BA=_tmp9B9->f1;_tmp9BB=_tmp9B9->f2;}}_LL3D3: _tmp9BD=_tmp9BA;_tmp9BE=
_tmp9BB;goto _LL3D5;_LL3D4: {struct Cyc_Absyn_SeqExp_e_struct*_tmp9BC=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp9A3;if(_tmp9BC->tag != 9)goto _LL3D6;else{_tmp9BD=_tmp9BC->f1;_tmp9BE=_tmp9BC->f2;}}
_LL3D5: _tmp9C0=_tmp9BD;_tmp9C1=_tmp9BE;goto _LL3D7;_LL3D6: {struct Cyc_Absyn_Subscript_e_struct*
_tmp9BF=(struct Cyc_Absyn_Subscript_e_struct*)_tmp9A3;if(_tmp9BF->tag != 24)goto
_LL3D8;else{_tmp9C0=_tmp9BF->f1;_tmp9C1=_tmp9BF->f2;}}_LL3D7: _tmp9C3=_tmp9C0;
_tmp9C4=_tmp9C1;goto _LL3D9;_LL3D8: {struct Cyc_Absyn_Swap_e_struct*_tmp9C2=(
struct Cyc_Absyn_Swap_e_struct*)_tmp9A3;if(_tmp9C2->tag != 35)goto _LL3DA;else{
_tmp9C3=_tmp9C2->f1;_tmp9C4=_tmp9C2->f2;}}_LL3D9: _tmp9C6=_tmp9C3;_tmp9C7=_tmp9C4;
goto _LL3DB;_LL3DA: {struct Cyc_Absyn_AssignOp_e_struct*_tmp9C5=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp9A3;if(_tmp9C5->tag != 4)goto _LL3DC;else{_tmp9C6=_tmp9C5->f1;_tmp9C7=_tmp9C5->f3;}}
_LL3DB: Cyc_Toc_exptypes_to_c(_tmp9C6);Cyc_Toc_exptypes_to_c(_tmp9C7);goto _LL3BD;
_LL3DC: {struct Cyc_Absyn_Conditional_e_struct*_tmp9C8=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp9A3;if(_tmp9C8->tag != 6)goto _LL3DE;else{_tmp9C9=_tmp9C8->f1;_tmp9CA=_tmp9C8->f2;
_tmp9CB=_tmp9C8->f3;}}_LL3DD: Cyc_Toc_exptypes_to_c(_tmp9C9);Cyc_Toc_exptypes_to_c(
_tmp9CA);Cyc_Toc_exptypes_to_c(_tmp9CB);goto _LL3BD;_LL3DE: {struct Cyc_Absyn_FnCall_e_struct*
_tmp9CC=(struct Cyc_Absyn_FnCall_e_struct*)_tmp9A3;if(_tmp9CC->tag != 11)goto
_LL3E0;else{_tmp9CD=_tmp9CC->f1;_tmp9CE=_tmp9CC->f2;}}_LL3DF: _tmp9D0=_tmp9CD;
_tmp9D1=_tmp9CE;goto _LL3E1;_LL3E0: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp9CF=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp9A3;if(_tmp9CF->tag != 10)goto _LL3E2;
else{_tmp9D0=_tmp9CF->f1;_tmp9D1=_tmp9CF->f2;}}_LL3E1: Cyc_Toc_exptypes_to_c(
_tmp9D0);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp9D1);goto _LL3BD;_LL3E2: {struct Cyc_Absyn_Cast_e_struct*
_tmp9D2=(struct Cyc_Absyn_Cast_e_struct*)_tmp9A3;if(_tmp9D2->tag != 15)goto _LL3E4;
else{_tmp9D3=(void**)& _tmp9D2->f1;_tmp9D4=(void**)((void**)& _tmp9D2->f1);_tmp9D5=
_tmp9D2->f2;}}_LL3E3:*_tmp9D4=Cyc_Toc_typ_to_c(*_tmp9D4);Cyc_Toc_exptypes_to_c(
_tmp9D5);goto _LL3BD;_LL3E4: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp9D6=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp9A3;if(_tmp9D6->tag != 26)goto _LL3E6;
else{_tmp9D7=_tmp9D6->f1;_tmp9D8=*_tmp9D7;_tmp9D9=_tmp9D8.f3;_tmp9DA=(void**)&(*
_tmp9D6->f1).f3;_tmp9DB=_tmp9D6->f2;}}_LL3E5:*_tmp9DA=Cyc_Toc_typ_to_c(*_tmp9DA);
_tmp9DD=_tmp9DB;goto _LL3E7;_LL3E6: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp9DC=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp9A3;if(_tmp9DC->tag != 36)
goto _LL3E8;else{_tmp9DD=_tmp9DC->f2;}}_LL3E7: _tmp9DF=_tmp9DD;goto _LL3E9;_LL3E8: {
struct Cyc_Absyn_Array_e_struct*_tmp9DE=(struct Cyc_Absyn_Array_e_struct*)_tmp9A3;
if(_tmp9DE->tag != 27)goto _LL3EA;else{_tmp9DF=_tmp9DE->f1;}}_LL3E9: for(0;_tmp9DF
!= 0;_tmp9DF=_tmp9DF->tl){struct Cyc_Absyn_Exp*_tmp9FB;struct _tuple16 _tmp9FA=*((
struct _tuple16*)_tmp9DF->hd);_tmp9FB=_tmp9FA.f2;Cyc_Toc_exptypes_to_c(_tmp9FB);}
goto _LL3BD;_LL3EA: {struct Cyc_Absyn_Offsetof_e_struct*_tmp9E0=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp9A3;if(_tmp9E0->tag != 20)goto _LL3EC;else{_tmp9E1=(void**)& _tmp9E0->f1;
_tmp9E2=(void**)((void**)& _tmp9E0->f1);}}_LL3EB: _tmp9E5=_tmp9E2;goto _LL3ED;
_LL3EC: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp9E3=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp9A3;if(_tmp9E3->tag != 18)goto _LL3EE;else{_tmp9E4=(void**)& _tmp9E3->f1;
_tmp9E5=(void**)((void**)& _tmp9E3->f1);}}_LL3ED:*_tmp9E5=Cyc_Toc_typ_to_c(*
_tmp9E5);goto _LL3BD;_LL3EE: {struct Cyc_Absyn_StmtExp_e_struct*_tmp9E6=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp9A3;if(_tmp9E6->tag != 37)goto _LL3F0;else{_tmp9E7=_tmp9E6->f1;}}_LL3EF: Cyc_Toc_stmttypes_to_c(
_tmp9E7);goto _LL3BD;_LL3F0: {struct Cyc_Absyn_Malloc_e_struct*_tmp9E8=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp9A3;if(_tmp9E8->tag != 34)goto _LL3F2;else{_tmp9E9=_tmp9E8->f1;_tmp9EA=(struct
Cyc_Absyn_MallocInfo*)& _tmp9E8->f1;}}_LL3F1: if(_tmp9EA->elt_type != 0){void**
_tmpE5C;_tmp9EA->elt_type=((_tmpE5C=_cycalloc(sizeof(*_tmpE5C)),((_tmpE5C[0]=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp9EA->elt_type))),_tmpE5C))));}Cyc_Toc_exptypes_to_c(
_tmp9EA->num_elts);goto _LL3BD;_LL3F2: {struct Cyc_Absyn_Const_e_struct*_tmp9EB=(
struct Cyc_Absyn_Const_e_struct*)_tmp9A3;if(_tmp9EB->tag != 0)goto _LL3F4;}_LL3F3:
goto _LL3F5;_LL3F4: {struct Cyc_Absyn_Var_e_struct*_tmp9EC=(struct Cyc_Absyn_Var_e_struct*)
_tmp9A3;if(_tmp9EC->tag != 1)goto _LL3F6;}_LL3F5: goto _LL3F7;_LL3F6: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp9ED=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp9A3;if(_tmp9ED->tag != 2)goto
_LL3F8;}_LL3F7: goto _LL3F9;_LL3F8: {struct Cyc_Absyn_Enum_e_struct*_tmp9EE=(struct
Cyc_Absyn_Enum_e_struct*)_tmp9A3;if(_tmp9EE->tag != 32)goto _LL3FA;}_LL3F9: goto
_LL3FB;_LL3FA: {struct Cyc_Absyn_Asm_e_struct*_tmp9EF=(struct Cyc_Absyn_Asm_e_struct*)
_tmp9A3;if(_tmp9EF->tag != 40)goto _LL3FC;}_LL3FB: goto _LL3FD;_LL3FC: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp9F0=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp9A3;if(_tmp9F0->tag != 33)goto
_LL3FE;}_LL3FD: goto _LL3BD;_LL3FE: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp9F1=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp9A3;if(_tmp9F1->tag != 30)goto _LL400;}
_LL3FF: goto _LL401;_LL400: {struct Cyc_Absyn_Datatype_e_struct*_tmp9F2=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp9A3;if(_tmp9F2->tag != 31)goto _LL402;}_LL401: goto _LL403;_LL402: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp9F3=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp9A3;if(_tmp9F3->tag != 29)goto
_LL404;}_LL403: goto _LL405;_LL404: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp9F4=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp9A3;if(_tmp9F4->tag != 28)
goto _LL406;}_LL405: goto _LL407;_LL406: {struct Cyc_Absyn_Tuple_e_struct*_tmp9F5=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp9A3;if(_tmp9F5->tag != 25)goto _LL408;}_LL407:
goto _LL409;_LL408: {struct Cyc_Absyn_Instantiate_e_struct*_tmp9F6=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp9A3;if(_tmp9F6->tag != 14)goto _LL40A;}_LL409: goto _LL40B;_LL40A: {struct Cyc_Absyn_New_e_struct*
_tmp9F7=(struct Cyc_Absyn_New_e_struct*)_tmp9A3;if(_tmp9F7->tag != 17)goto _LL40C;}
_LL40B: goto _LL40D;_LL40C: {struct Cyc_Absyn_Valueof_e_struct*_tmp9F8=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp9A3;if(_tmp9F8->tag != 39)goto _LL40E;}_LL40D: goto _LL40F;_LL40E: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp9F9=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp9A3;if(_tmp9F9->tag != 38)goto
_LL3BD;}_LL40F:{const char*_tmpE5F;void*_tmpE5E;(_tmpE5E=0,Cyc_Tcutil_terr(e->loc,((
_tmpE5F="Cyclone expression within C code",_tag_dyneither(_tmpE5F,sizeof(char),
33))),_tag_dyneither(_tmpE5E,sizeof(void*),0)));}goto _LL3BD;_LL3BD:;}static void
Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){void*_tmp9FF=d->r;struct Cyc_Absyn_Vardecl*
_tmpA01;struct Cyc_Absyn_Fndecl*_tmpA03;struct Cyc_Absyn_Aggrdecl*_tmpA05;struct
Cyc_Absyn_Enumdecl*_tmpA07;struct Cyc_Absyn_Typedefdecl*_tmpA09;_LL411: {struct
Cyc_Absyn_Var_d_struct*_tmpA00=(struct Cyc_Absyn_Var_d_struct*)_tmp9FF;if(_tmpA00->tag
!= 0)goto _LL413;else{_tmpA01=_tmpA00->f1;}}_LL412: _tmpA01->type=Cyc_Toc_typ_to_c(
_tmpA01->type);if(_tmpA01->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmpA01->initializer));goto _LL410;_LL413: {struct Cyc_Absyn_Fn_d_struct*
_tmpA02=(struct Cyc_Absyn_Fn_d_struct*)_tmp9FF;if(_tmpA02->tag != 1)goto _LL415;
else{_tmpA03=_tmpA02->f1;}}_LL414: _tmpA03->ret_type=Cyc_Toc_typ_to_c(_tmpA03->ret_type);{
struct Cyc_List_List*_tmpA15=_tmpA03->args;for(0;_tmpA15 != 0;_tmpA15=_tmpA15->tl){(*((
struct _tuple27*)_tmpA15->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple27*)_tmpA15->hd)).f3);}}
goto _LL410;_LL415: {struct Cyc_Absyn_Aggr_d_struct*_tmpA04=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp9FF;if(_tmpA04->tag != 6)goto _LL417;else{_tmpA05=_tmpA04->f1;}}_LL416: Cyc_Toc_aggrdecl_to_c(
_tmpA05,1);goto _LL410;_LL417: {struct Cyc_Absyn_Enum_d_struct*_tmpA06=(struct Cyc_Absyn_Enum_d_struct*)
_tmp9FF;if(_tmpA06->tag != 8)goto _LL419;else{_tmpA07=_tmpA06->f1;}}_LL418: if(
_tmpA07->fields != 0){struct Cyc_List_List*_tmpA16=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmpA07->fields))->v;for(0;_tmpA16 != 0;_tmpA16=_tmpA16->tl){
struct Cyc_Absyn_Enumfield*_tmpA17=(struct Cyc_Absyn_Enumfield*)_tmpA16->hd;if(
_tmpA17->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmpA17->tag));}}
goto _LL410;_LL419: {struct Cyc_Absyn_Typedef_d_struct*_tmpA08=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp9FF;if(_tmpA08->tag != 9)goto _LL41B;else{_tmpA09=_tmpA08->f1;}}_LL41A:{struct
Cyc_Core_Opt*_tmpE60;_tmpA09->defn=((_tmpE60=_cycalloc(sizeof(*_tmpE60)),((
_tmpE60->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA09->defn))->v),_tmpE60))));}goto _LL410;_LL41B: {struct Cyc_Absyn_Let_d_struct*
_tmpA0A=(struct Cyc_Absyn_Let_d_struct*)_tmp9FF;if(_tmpA0A->tag != 2)goto _LL41D;}
_LL41C: goto _LL41E;_LL41D: {struct Cyc_Absyn_Letv_d_struct*_tmpA0B=(struct Cyc_Absyn_Letv_d_struct*)
_tmp9FF;if(_tmpA0B->tag != 3)goto _LL41F;}_LL41E: goto _LL420;_LL41F: {struct Cyc_Absyn_Datatype_d_struct*
_tmpA0C=(struct Cyc_Absyn_Datatype_d_struct*)_tmp9FF;if(_tmpA0C->tag != 7)goto
_LL421;}_LL420: goto _LL422;_LL421: {struct Cyc_Absyn_Namespace_d_struct*_tmpA0D=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp9FF;if(_tmpA0D->tag != 10)goto _LL423;}
_LL422: goto _LL424;_LL423: {struct Cyc_Absyn_Using_d_struct*_tmpA0E=(struct Cyc_Absyn_Using_d_struct*)
_tmp9FF;if(_tmpA0E->tag != 11)goto _LL425;}_LL424: goto _LL426;_LL425: {struct Cyc_Absyn_ExternC_d_struct*
_tmpA0F=(struct Cyc_Absyn_ExternC_d_struct*)_tmp9FF;if(_tmpA0F->tag != 12)goto
_LL427;}_LL426: goto _LL428;_LL427: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmpA10=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9FF;if(_tmpA10->tag != 13)
goto _LL429;}_LL428: goto _LL42A;_LL429: {struct Cyc_Absyn_Region_d_struct*_tmpA11=(
struct Cyc_Absyn_Region_d_struct*)_tmp9FF;if(_tmpA11->tag != 4)goto _LL42B;}_LL42A:
goto _LL42C;_LL42B: {struct Cyc_Absyn_Alias_d_struct*_tmpA12=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9FF;if(_tmpA12->tag != 5)goto _LL42D;}_LL42C:{const char*_tmpE63;void*_tmpE62;(
_tmpE62=0,Cyc_Tcutil_terr(d->loc,((_tmpE63="Cyclone declaration within C code",
_tag_dyneither(_tmpE63,sizeof(char),34))),_tag_dyneither(_tmpE62,sizeof(void*),0)));}
goto _LL410;_LL42D: {struct Cyc_Absyn_Porton_d_struct*_tmpA13=(struct Cyc_Absyn_Porton_d_struct*)
_tmp9FF;if(_tmpA13->tag != 14)goto _LL42F;}_LL42E: goto _LL430;_LL42F: {struct Cyc_Absyn_Portoff_d_struct*
_tmpA14=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9FF;if(_tmpA14->tag != 15)goto
_LL410;}_LL430: goto _LL410;_LL410:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s){void*_tmpA1B=s->r;struct Cyc_Absyn_Exp*_tmpA1D;struct Cyc_Absyn_Stmt*_tmpA1F;
struct Cyc_Absyn_Stmt*_tmpA20;struct Cyc_Absyn_Exp*_tmpA22;struct Cyc_Absyn_Exp*
_tmpA24;struct Cyc_Absyn_Stmt*_tmpA25;struct Cyc_Absyn_Stmt*_tmpA26;struct _tuple9
_tmpA28;struct Cyc_Absyn_Exp*_tmpA29;struct Cyc_Absyn_Stmt*_tmpA2A;struct Cyc_Absyn_Exp*
_tmpA2C;struct _tuple9 _tmpA2D;struct Cyc_Absyn_Exp*_tmpA2E;struct _tuple9 _tmpA2F;
struct Cyc_Absyn_Exp*_tmpA30;struct Cyc_Absyn_Stmt*_tmpA31;struct Cyc_Absyn_Exp*
_tmpA33;struct Cyc_List_List*_tmpA34;struct Cyc_Absyn_Decl*_tmpA36;struct Cyc_Absyn_Stmt*
_tmpA37;struct Cyc_Absyn_Stmt*_tmpA39;struct _tuple9 _tmpA3A;struct Cyc_Absyn_Exp*
_tmpA3B;_LL432: {struct Cyc_Absyn_Exp_s_struct*_tmpA1C=(struct Cyc_Absyn_Exp_s_struct*)
_tmpA1B;if(_tmpA1C->tag != 1)goto _LL434;else{_tmpA1D=_tmpA1C->f1;}}_LL433: Cyc_Toc_exptypes_to_c(
_tmpA1D);goto _LL431;_LL434: {struct Cyc_Absyn_Seq_s_struct*_tmpA1E=(struct Cyc_Absyn_Seq_s_struct*)
_tmpA1B;if(_tmpA1E->tag != 2)goto _LL436;else{_tmpA1F=_tmpA1E->f1;_tmpA20=_tmpA1E->f2;}}
_LL435: Cyc_Toc_stmttypes_to_c(_tmpA1F);Cyc_Toc_stmttypes_to_c(_tmpA20);goto
_LL431;_LL436: {struct Cyc_Absyn_Return_s_struct*_tmpA21=(struct Cyc_Absyn_Return_s_struct*)
_tmpA1B;if(_tmpA21->tag != 3)goto _LL438;else{_tmpA22=_tmpA21->f1;}}_LL437: if(
_tmpA22 != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmpA22);goto _LL431;
_LL438: {struct Cyc_Absyn_IfThenElse_s_struct*_tmpA23=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmpA1B;if(_tmpA23->tag != 4)goto _LL43A;else{_tmpA24=_tmpA23->f1;_tmpA25=_tmpA23->f2;
_tmpA26=_tmpA23->f3;}}_LL439: Cyc_Toc_exptypes_to_c(_tmpA24);Cyc_Toc_stmttypes_to_c(
_tmpA25);Cyc_Toc_stmttypes_to_c(_tmpA26);goto _LL431;_LL43A: {struct Cyc_Absyn_While_s_struct*
_tmpA27=(struct Cyc_Absyn_While_s_struct*)_tmpA1B;if(_tmpA27->tag != 5)goto _LL43C;
else{_tmpA28=_tmpA27->f1;_tmpA29=_tmpA28.f1;_tmpA2A=_tmpA27->f2;}}_LL43B: Cyc_Toc_exptypes_to_c(
_tmpA29);Cyc_Toc_stmttypes_to_c(_tmpA2A);goto _LL431;_LL43C: {struct Cyc_Absyn_For_s_struct*
_tmpA2B=(struct Cyc_Absyn_For_s_struct*)_tmpA1B;if(_tmpA2B->tag != 9)goto _LL43E;
else{_tmpA2C=_tmpA2B->f1;_tmpA2D=_tmpA2B->f2;_tmpA2E=_tmpA2D.f1;_tmpA2F=_tmpA2B->f3;
_tmpA30=_tmpA2F.f1;_tmpA31=_tmpA2B->f4;}}_LL43D: Cyc_Toc_exptypes_to_c(_tmpA2C);
Cyc_Toc_exptypes_to_c(_tmpA2E);Cyc_Toc_exptypes_to_c(_tmpA30);Cyc_Toc_stmttypes_to_c(
_tmpA31);goto _LL431;_LL43E: {struct Cyc_Absyn_Switch_s_struct*_tmpA32=(struct Cyc_Absyn_Switch_s_struct*)
_tmpA1B;if(_tmpA32->tag != 10)goto _LL440;else{_tmpA33=_tmpA32->f1;_tmpA34=_tmpA32->f2;}}
_LL43F: Cyc_Toc_exptypes_to_c(_tmpA33);for(0;_tmpA34 != 0;_tmpA34=_tmpA34->tl){Cyc_Toc_stmttypes_to_c(((
struct Cyc_Absyn_Switch_clause*)_tmpA34->hd)->body);}goto _LL431;_LL440: {struct
Cyc_Absyn_Decl_s_struct*_tmpA35=(struct Cyc_Absyn_Decl_s_struct*)_tmpA1B;if(
_tmpA35->tag != 12)goto _LL442;else{_tmpA36=_tmpA35->f1;_tmpA37=_tmpA35->f2;}}
_LL441: Cyc_Toc_decltypes_to_c(_tmpA36);Cyc_Toc_stmttypes_to_c(_tmpA37);goto
_LL431;_LL442: {struct Cyc_Absyn_Do_s_struct*_tmpA38=(struct Cyc_Absyn_Do_s_struct*)
_tmpA1B;if(_tmpA38->tag != 14)goto _LL444;else{_tmpA39=_tmpA38->f1;_tmpA3A=_tmpA38->f2;
_tmpA3B=_tmpA3A.f1;}}_LL443: Cyc_Toc_stmttypes_to_c(_tmpA39);Cyc_Toc_exptypes_to_c(
_tmpA3B);goto _LL431;_LL444: {struct Cyc_Absyn_Skip_s_struct*_tmpA3C=(struct Cyc_Absyn_Skip_s_struct*)
_tmpA1B;if(_tmpA3C->tag != 0)goto _LL446;}_LL445: goto _LL447;_LL446: {struct Cyc_Absyn_Break_s_struct*
_tmpA3D=(struct Cyc_Absyn_Break_s_struct*)_tmpA1B;if(_tmpA3D->tag != 6)goto _LL448;}
_LL447: goto _LL449;_LL448: {struct Cyc_Absyn_Continue_s_struct*_tmpA3E=(struct Cyc_Absyn_Continue_s_struct*)
_tmpA1B;if(_tmpA3E->tag != 7)goto _LL44A;}_LL449: goto _LL44B;_LL44A: {struct Cyc_Absyn_Goto_s_struct*
_tmpA3F=(struct Cyc_Absyn_Goto_s_struct*)_tmpA1B;if(_tmpA3F->tag != 8)goto _LL44C;}
_LL44B: goto _LL431;_LL44C: {struct Cyc_Absyn_Fallthru_s_struct*_tmpA40=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmpA1B;if(_tmpA40->tag != 11)goto _LL44E;}_LL44D: goto _LL44F;_LL44E: {struct Cyc_Absyn_Label_s_struct*
_tmpA41=(struct Cyc_Absyn_Label_s_struct*)_tmpA1B;if(_tmpA41->tag != 13)goto _LL450;}
_LL44F: goto _LL451;_LL450: {struct Cyc_Absyn_TryCatch_s_struct*_tmpA42=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmpA1B;if(_tmpA42->tag != 15)goto _LL452;}_LL451: goto _LL453;_LL452: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmpA43=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmpA1B;if(_tmpA43->tag != 16)goto
_LL431;}_LL453:{const char*_tmpE66;void*_tmpE65;(_tmpE65=0,Cyc_Tcutil_terr(s->loc,((
_tmpE66="Cyclone statement in C code",_tag_dyneither(_tmpE66,sizeof(char),28))),
_tag_dyneither(_tmpE65,sizeof(void*),0)));}goto _LL431;_LL431:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude){for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpE69;void*_tmpE68;(_tmpE68=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE69="decls_to_c: not at toplevel!",
_tag_dyneither(_tmpE69,sizeof(char),29))),_tag_dyneither(_tmpE68,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmpA48=d->r;struct Cyc_Absyn_Vardecl*
_tmpA4A;struct Cyc_Absyn_Fndecl*_tmpA4C;struct Cyc_Absyn_Aggrdecl*_tmpA52;struct
Cyc_Absyn_Datatypedecl*_tmpA54;struct Cyc_Absyn_Enumdecl*_tmpA56;struct Cyc_Absyn_Typedefdecl*
_tmpA58;struct Cyc_List_List*_tmpA5C;struct Cyc_List_List*_tmpA5E;struct Cyc_List_List*
_tmpA60;struct Cyc_List_List*_tmpA62;_LL455: {struct Cyc_Absyn_Var_d_struct*
_tmpA49=(struct Cyc_Absyn_Var_d_struct*)_tmpA48;if(_tmpA49->tag != 0)goto _LL457;
else{_tmpA4A=_tmpA49->f1;}}_LL456: {struct _tuple0*_tmpA63=_tmpA4A->name;if(
_tmpA4A->sc == Cyc_Absyn_ExternC){struct _tuple0*_tmpE6A;_tmpA63=((_tmpE6A=
_cycalloc(sizeof(*_tmpE6A)),((_tmpE6A->f1=Cyc_Absyn_Rel_n(0),((_tmpE6A->f2=(*
_tmpA63).f2,_tmpE6A))))));}if(_tmpA4A->initializer != 0){if(cinclude)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmpA4A->initializer));else{Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_check_null(_tmpA4A->initializer));}}{struct Cyc_Absyn_Global_b_struct
_tmpE6D;struct Cyc_Absyn_Global_b_struct*_tmpE6C;nv=Cyc_Toc_add_varmap(r,nv,
_tmpA4A->name,Cyc_Absyn_varb_exp(_tmpA63,(void*)((_tmpE6C=_cycalloc(sizeof(*
_tmpE6C)),((_tmpE6C[0]=((_tmpE6D.tag=1,((_tmpE6D.f1=_tmpA4A,_tmpE6D)))),_tmpE6C)))),
0));}_tmpA4A->name=_tmpA63;_tmpA4A->sc=Cyc_Toc_scope_to_c(_tmpA4A->sc);_tmpA4A->type=
Cyc_Toc_typ_to_c_array(_tmpA4A->type);{struct Cyc_List_List*_tmpE6E;Cyc_Toc_result_decls=((
_tmpE6E=_cycalloc(sizeof(*_tmpE6E)),((_tmpE6E->hd=d,((_tmpE6E->tl=Cyc_Toc_result_decls,
_tmpE6E))))));}goto _LL454;}_LL457: {struct Cyc_Absyn_Fn_d_struct*_tmpA4B=(struct
Cyc_Absyn_Fn_d_struct*)_tmpA48;if(_tmpA4B->tag != 1)goto _LL459;else{_tmpA4C=
_tmpA4B->f1;}}_LL458: {struct _tuple0*_tmpA68=_tmpA4C->name;if(_tmpA4C->sc == Cyc_Absyn_ExternC){{
struct _tuple0*_tmpE6F;_tmpA68=((_tmpE6F=_cycalloc(sizeof(*_tmpE6F)),((_tmpE6F->f1=
Cyc_Absyn_Rel_n(0),((_tmpE6F->f2=(*_tmpA68).f2,_tmpE6F))))));}_tmpA4C->sc=Cyc_Absyn_Public;}
nv=Cyc_Toc_add_varmap(r,nv,_tmpA4C->name,Cyc_Absyn_var_exp(_tmpA68,0));_tmpA4C->name=
_tmpA68;Cyc_Toc_fndecl_to_c(nv,_tmpA4C,cinclude);{struct Cyc_List_List*_tmpE70;
Cyc_Toc_result_decls=((_tmpE70=_cycalloc(sizeof(*_tmpE70)),((_tmpE70->hd=d,((
_tmpE70->tl=Cyc_Toc_result_decls,_tmpE70))))));}goto _LL454;}_LL459: {struct Cyc_Absyn_Let_d_struct*
_tmpA4D=(struct Cyc_Absyn_Let_d_struct*)_tmpA48;if(_tmpA4D->tag != 2)goto _LL45B;}
_LL45A: goto _LL45C;_LL45B: {struct Cyc_Absyn_Letv_d_struct*_tmpA4E=(struct Cyc_Absyn_Letv_d_struct*)
_tmpA48;if(_tmpA4E->tag != 3)goto _LL45D;}_LL45C: {const char*_tmpE73;void*_tmpE72;(
_tmpE72=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE73="letdecl at toplevel",_tag_dyneither(_tmpE73,sizeof(char),20))),
_tag_dyneither(_tmpE72,sizeof(void*),0)));}_LL45D: {struct Cyc_Absyn_Region_d_struct*
_tmpA4F=(struct Cyc_Absyn_Region_d_struct*)_tmpA48;if(_tmpA4F->tag != 4)goto _LL45F;}
_LL45E: {const char*_tmpE76;void*_tmpE75;(_tmpE75=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE76="region decl at toplevel",
_tag_dyneither(_tmpE76,sizeof(char),24))),_tag_dyneither(_tmpE75,sizeof(void*),0)));}
_LL45F: {struct Cyc_Absyn_Alias_d_struct*_tmpA50=(struct Cyc_Absyn_Alias_d_struct*)
_tmpA48;if(_tmpA50->tag != 5)goto _LL461;}_LL460: {const char*_tmpE79;void*_tmpE78;(
_tmpE78=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE79="alias decl at toplevel",_tag_dyneither(_tmpE79,sizeof(char),23))),
_tag_dyneither(_tmpE78,sizeof(void*),0)));}_LL461: {struct Cyc_Absyn_Aggr_d_struct*
_tmpA51=(struct Cyc_Absyn_Aggr_d_struct*)_tmpA48;if(_tmpA51->tag != 6)goto _LL463;
else{_tmpA52=_tmpA51->f1;}}_LL462: Cyc_Toc_aggrdecl_to_c(_tmpA52,1);goto _LL454;
_LL463: {struct Cyc_Absyn_Datatype_d_struct*_tmpA53=(struct Cyc_Absyn_Datatype_d_struct*)
_tmpA48;if(_tmpA53->tag != 7)goto _LL465;else{_tmpA54=_tmpA53->f1;}}_LL464: if(
_tmpA54->is_extensible)Cyc_Toc_xdatatypedecl_to_c(_tmpA54);else{Cyc_Toc_datatypedecl_to_c(
_tmpA54);}goto _LL454;_LL465: {struct Cyc_Absyn_Enum_d_struct*_tmpA55=(struct Cyc_Absyn_Enum_d_struct*)
_tmpA48;if(_tmpA55->tag != 8)goto _LL467;else{_tmpA56=_tmpA55->f1;}}_LL466: Cyc_Toc_enumdecl_to_c(
_tmpA56);{struct Cyc_List_List*_tmpE7A;Cyc_Toc_result_decls=((_tmpE7A=_cycalloc(
sizeof(*_tmpE7A)),((_tmpE7A->hd=d,((_tmpE7A->tl=Cyc_Toc_result_decls,_tmpE7A))))));}
goto _LL454;_LL467: {struct Cyc_Absyn_Typedef_d_struct*_tmpA57=(struct Cyc_Absyn_Typedef_d_struct*)
_tmpA48;if(_tmpA57->tag != 9)goto _LL469;else{_tmpA58=_tmpA57->f1;}}_LL468: _tmpA58->name=
_tmpA58->name;_tmpA58->tvs=0;if(_tmpA58->defn != 0){struct Cyc_Core_Opt*_tmpE7B;
_tmpA58->defn=((_tmpE7B=_cycalloc(sizeof(*_tmpE7B)),((_tmpE7B->v=Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA58->defn))->v),_tmpE7B))));}else{
switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmpA58->kind))->v)->kind){
case Cyc_Absyn_BoxKind: _LL475:{struct Cyc_Core_Opt*_tmpE7C;_tmpA58->defn=((_tmpE7C=
_cycalloc(sizeof(*_tmpE7C)),((_tmpE7C->v=Cyc_Absyn_void_star_typ(),_tmpE7C))));}
break;default: _LL476:{struct Cyc_Core_Opt*_tmpE7D;_tmpA58->defn=((_tmpE7D=
_cycalloc(sizeof(*_tmpE7D)),((_tmpE7D->v=(void*)& Cyc_Absyn_VoidType_val,_tmpE7D))));}
break;}}{struct Cyc_List_List*_tmpE7E;Cyc_Toc_result_decls=((_tmpE7E=_cycalloc(
sizeof(*_tmpE7E)),((_tmpE7E->hd=d,((_tmpE7E->tl=Cyc_Toc_result_decls,_tmpE7E))))));}
goto _LL454;_LL469: {struct Cyc_Absyn_Porton_d_struct*_tmpA59=(struct Cyc_Absyn_Porton_d_struct*)
_tmpA48;if(_tmpA59->tag != 14)goto _LL46B;}_LL46A: goto _LL46C;_LL46B: {struct Cyc_Absyn_Portoff_d_struct*
_tmpA5A=(struct Cyc_Absyn_Portoff_d_struct*)_tmpA48;if(_tmpA5A->tag != 15)goto
_LL46D;}_LL46C: goto _LL454;_LL46D: {struct Cyc_Absyn_Namespace_d_struct*_tmpA5B=(
struct Cyc_Absyn_Namespace_d_struct*)_tmpA48;if(_tmpA5B->tag != 10)goto _LL46F;
else{_tmpA5C=_tmpA5B->f2;}}_LL46E: _tmpA5E=_tmpA5C;goto _LL470;_LL46F: {struct Cyc_Absyn_Using_d_struct*
_tmpA5D=(struct Cyc_Absyn_Using_d_struct*)_tmpA48;if(_tmpA5D->tag != 11)goto _LL471;
else{_tmpA5E=_tmpA5D->f2;}}_LL470: _tmpA60=_tmpA5E;goto _LL472;_LL471: {struct Cyc_Absyn_ExternC_d_struct*
_tmpA5F=(struct Cyc_Absyn_ExternC_d_struct*)_tmpA48;if(_tmpA5F->tag != 12)goto
_LL473;else{_tmpA60=_tmpA5F->f1;}}_LL472: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA60,top,
cinclude);goto _LL454;_LL473: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA61=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmpA48;if(_tmpA61->tag != 13)goto _LL454;
else{_tmpA62=_tmpA61->f1;}}_LL474: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA62,top,1);goto
_LL454;_LL454:;};}return nv;}static void Cyc_Toc_init(){struct _DynRegionHandle*
_tmpA77;struct Cyc_Core_NewRegion _tmpA76=Cyc_Core__new_dynregion((const char*)"internal-error",
0);_tmpA77=_tmpA76.dynregion;{struct _DynRegionFrame _tmpA78;struct _RegionHandle*d=
_open_dynregion(& _tmpA78,_tmpA77);{struct Cyc_Dict_Dict*_tmpE8B;struct Cyc_Dict_Dict*
_tmpE8A;struct Cyc_Set_Set**_tmpE89;struct Cyc_List_List**_tmpE88;struct Cyc_Dict_Dict*
_tmpE87;struct Cyc_List_List**_tmpE86;struct Cyc_Toc_TocState*_tmpE85;Cyc_Toc_toc_state=((
_tmpE85=_cycalloc(sizeof(*_tmpE85)),((_tmpE85->dyn=(struct _DynRegionHandle*)
_tmpA77,((_tmpE85->tuple_types=(struct Cyc_List_List**)((_tmpE86=_region_malloc(d,
sizeof(*_tmpE86)),((_tmpE86[0]=0,_tmpE86)))),((_tmpE85->aggrs_so_far=(struct Cyc_Dict_Dict*)((
_tmpE87=_region_malloc(d,sizeof(*_tmpE87)),((_tmpE87[0]=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
d,Cyc_Absyn_qvar_cmp),_tmpE87)))),((_tmpE85->abs_struct_types=(struct Cyc_List_List**)((
_tmpE88=_region_malloc(d,sizeof(*_tmpE88)),((_tmpE88[0]=0,_tmpE88)))),((_tmpE85->datatypes_so_far=(
struct Cyc_Set_Set**)((_tmpE89=_region_malloc(d,sizeof(*_tmpE89)),((_tmpE89[0]=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE89)))),((_tmpE85->xdatatypes_so_far=(
struct Cyc_Dict_Dict*)((_tmpE8A=_region_malloc(d,sizeof(*_tmpE8A)),((_tmpE8A[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE8A)))),((_tmpE85->qvar_tags=(
struct Cyc_Dict_Dict*)((_tmpE8B=_region_malloc(d,sizeof(*_tmpE8B)),((_tmpE8B[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct
_tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpE8B)))),((_tmpE85->temp_labels=(
struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(
d),(struct Cyc_Toc_TocState*)_tmpE85))))))))))))))))));}Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=
0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=0;{struct
_dyneither_ptr**_tmpE8C;Cyc_Toc_globals=_tag_dyneither(((_tmpE8C=_cycalloc(
sizeof(struct _dyneither_ptr*)* 41),((_tmpE8C[0]=& Cyc_Toc__throw_str,((_tmpE8C[1]=&
Cyc_Toc_setjmp_str,((_tmpE8C[2]=& Cyc_Toc__push_handler_str,((_tmpE8C[3]=& Cyc_Toc__pop_handler_str,((
_tmpE8C[4]=& Cyc_Toc__exn_thrown_str,((_tmpE8C[5]=& Cyc_Toc__npop_handler_str,((
_tmpE8C[6]=& Cyc_Toc__check_null_str,((_tmpE8C[7]=& Cyc_Toc__check_known_subscript_null_str,((
_tmpE8C[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpE8C[9]=& Cyc_Toc__check_dyneither_subscript_str,((
_tmpE8C[10]=& Cyc_Toc__dyneither_ptr_str,((_tmpE8C[11]=& Cyc_Toc__tag_dyneither_str,((
_tmpE8C[12]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpE8C[13]=& Cyc_Toc__untag_dyneither_ptr_str,((
_tmpE8C[14]=& Cyc_Toc__get_dyneither_size_str,((_tmpE8C[15]=& Cyc_Toc__get_zero_arr_size_str,((
_tmpE8C[16]=& Cyc_Toc__dyneither_ptr_plus_str,((_tmpE8C[17]=& Cyc_Toc__zero_arr_plus_str,((
_tmpE8C[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpE8C[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((
_tmpE8C[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpE8C[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((
_tmpE8C[22]=& Cyc_Toc__cycalloc_str,((_tmpE8C[23]=& Cyc_Toc__cyccalloc_str,((
_tmpE8C[24]=& Cyc_Toc__cycalloc_atomic_str,((_tmpE8C[25]=& Cyc_Toc__cyccalloc_atomic_str,((
_tmpE8C[26]=& Cyc_Toc__region_malloc_str,((_tmpE8C[27]=& Cyc_Toc__region_calloc_str,((
_tmpE8C[28]=& Cyc_Toc__check_times_str,((_tmpE8C[29]=& Cyc_Toc__new_region_str,((
_tmpE8C[30]=& Cyc_Toc__push_region_str,((_tmpE8C[31]=& Cyc_Toc__pop_region_str,((
_tmpE8C[32]=& Cyc_Toc__open_dynregion_str,((_tmpE8C[33]=& Cyc_Toc__push_dynregion_str,((
_tmpE8C[34]=& Cyc_Toc__pop_dynregion_str,((_tmpE8C[35]=& Cyc_Toc__reset_region_str,((
_tmpE8C[36]=& Cyc_Toc__throw_arraybounds_str,((_tmpE8C[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,((
_tmpE8C[38]=& Cyc_Toc__throw_match_str,((_tmpE8C[39]=& Cyc_Toc__swap_word_str,((
_tmpE8C[40]=& Cyc_Toc__swap_dyneither_str,_tmpE8C)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),41);};;_pop_dynregion(d);};}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();{struct _RegionHandle _tmpA81=
_new_region("start");struct _RegionHandle*start=& _tmpA81;_push_region(start);Cyc_Toc_decls_to_c(
start,Cyc_Toc_empty_env(start),ds,1,0);{struct _DynRegionHandle*_tmpA83;struct Cyc_Toc_TocState
_tmpA82=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA83=
_tmpA82.dyn;Cyc_Core_free_dynregion(_tmpA83);};{struct Cyc_List_List*_tmpA84=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);
_npop_handler(0);return _tmpA84;};;_pop_region(start);};}
