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
struct Cyc_Core_NewRegion Cyc_Core_new_dynregion();extern char Cyc_Core_Open_Region[
12];struct Cyc_Core_Open_Region_struct{char*tag;};extern char Cyc_Core_Free_Region[
12];struct Cyc_Core_Free_Region_struct{char*tag;};void Cyc_Core_free_dynregion(
struct _DynRegionHandle*);inline static void* arrcast(struct _dyneither_ptr dyn,
unsigned int bd,unsigned int sz){if(bd >> 20  || sz >> 12)return 0;{unsigned char*
ptrbd=dyn.curr + bd * sz;if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base)
 || ptrbd > dyn.last_plus_one)return 0;return dyn.curr;};}struct Cyc_List_List{void*
hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
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
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
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
struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;
struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*
body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*
fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct
_dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*
exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};
struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;
struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;
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
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
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
void*f2;};void*Cyc_Tcutil_snd_tqt(struct _tuple9*);struct _tuple10{unsigned int f1;
int f2;};struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int
Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
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
ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const char*_tmpA41;void*_tmpA40;(_tmpA40=0,
Cyc_fprintf(Cyc_stderr,((_tmpA41="\n",_tag_dyneither(_tmpA41,sizeof(char),2))),
_tag_dyneither(_tmpA40,sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_struct _tmpA44;struct Cyc_Toc_Toc_Unimplemented_struct*
_tmpA43;(int)_throw((void*)((_tmpA43=_cycalloc_atomic(sizeof(*_tmpA43)),((
_tmpA43[0]=((_tmpA44.tag=Cyc_Toc_Toc_Unimplemented,_tmpA44)),_tmpA43)))));};}
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(
Cyc_stderr,fmt,ap);{const char*_tmpA47;void*_tmpA46;(_tmpA46=0,Cyc_fprintf(Cyc_stderr,((
_tmpA47="\n",_tag_dyneither(_tmpA47,sizeof(char),2))),_tag_dyneither(_tmpA46,
sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Impossible_struct
_tmpA4A;struct Cyc_Toc_Toc_Impossible_struct*_tmpA49;(int)_throw((void*)((_tmpA49=
_cycalloc_atomic(sizeof(*_tmpA49)),((_tmpA49[0]=((_tmpA4A.tag=Cyc_Toc_Toc_Impossible,
_tmpA4A)),_tmpA49)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_struct{
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
_tmpA4B;skip_stmt_opt=((_tmpA4B=_cycalloc(sizeof(*_tmpA4B)),((_tmpA4B[0]=Cyc_Absyn_skip_stmt(
0),_tmpA4B))));}return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(
void*t,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,
0);}static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Cast_e_struct
_tmpA4E;struct Cyc_Absyn_Cast_e_struct*_tmpA4D;return(void*)((_tmpA4D=_cycalloc(
sizeof(*_tmpA4D)),((_tmpA4D[0]=((_tmpA4E.tag=15,((_tmpA4E.f1=(void*)t,((_tmpA4E.f2=
e,((_tmpA4E.f3=0,((_tmpA4E.f4=Cyc_Absyn_No_coercion,_tmpA4E)))))))))),_tmpA4D))));}
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Deref_e_struct
_tmpA51;struct Cyc_Absyn_Deref_e_struct*_tmpA50;return(void*)((_tmpA50=_cycalloc(
sizeof(*_tmpA50)),((_tmpA50[0]=((_tmpA51.tag=21,((_tmpA51.f1=e,_tmpA51)))),
_tmpA50))));}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){struct Cyc_Absyn_Subscript_e_struct _tmpA54;struct Cyc_Absyn_Subscript_e_struct*
_tmpA53;return(void*)((_tmpA53=_cycalloc(sizeof(*_tmpA53)),((_tmpA53[0]=((
_tmpA54.tag=24,((_tmpA54.f1=e1,((_tmpA54.f2=e2,_tmpA54)))))),_tmpA53))));}static
void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct
_tmpA57;struct Cyc_Absyn_StmtExp_e_struct*_tmpA56;return(void*)((_tmpA56=
_cycalloc(sizeof(*_tmpA56)),((_tmpA56[0]=((_tmpA57.tag=37,((_tmpA57.f1=s,_tmpA57)))),
_tmpA56))));}static void*Cyc_Toc_sizeoftyp_exp_r(void*t){struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpA5A;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpA59;return(void*)((_tmpA59=
_cycalloc(sizeof(*_tmpA59)),((_tmpA59[0]=((_tmpA5A.tag=18,((_tmpA5A.f1=(void*)t,
_tmpA5A)))),_tmpA59))));}static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct _tmpA5D;struct Cyc_Absyn_FnCall_e_struct*
_tmpA5C;return(void*)((_tmpA5C=_cycalloc(sizeof(*_tmpA5C)),((_tmpA5C[0]=((
_tmpA5D.tag=11,((_tmpA5D.f1=e,((_tmpA5D.f2=es,((_tmpA5D.f3=0,_tmpA5D)))))))),
_tmpA5C))));}static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_struct
_tmpA60;struct Cyc_Absyn_Exp_s_struct*_tmpA5F;return(void*)((_tmpA5F=_cycalloc(
sizeof(*_tmpA5F)),((_tmpA5F[0]=((_tmpA60.tag=1,((_tmpA60.f1=e,_tmpA60)))),
_tmpA5F))));}static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2){struct Cyc_Absyn_Seq_s_struct _tmpA63;struct Cyc_Absyn_Seq_s_struct*_tmpA62;
return(void*)((_tmpA62=_cycalloc(sizeof(*_tmpA62)),((_tmpA62[0]=((_tmpA63.tag=2,((
_tmpA63.f1=s1,((_tmpA63.f2=s2,_tmpA63)))))),_tmpA62))));}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct
_tmpA66;struct Cyc_Absyn_Conditional_e_struct*_tmpA65;return(void*)((_tmpA65=
_cycalloc(sizeof(*_tmpA65)),((_tmpA65[0]=((_tmpA66.tag=6,((_tmpA66.f1=e1,((
_tmpA66.f2=e2,((_tmpA66.f3=e3,_tmpA66)))))))),_tmpA65))));}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrMember_e_struct
_tmpA69;struct Cyc_Absyn_AggrMember_e_struct*_tmpA68;return(void*)((_tmpA68=
_cycalloc(sizeof(*_tmpA68)),((_tmpA68[0]=((_tmpA69.tag=22,((_tmpA69.f1=e,((
_tmpA69.f2=n,((_tmpA69.f3=0,((_tmpA69.f4=0,_tmpA69)))))))))),_tmpA68))));}static
void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_struct _tmpA6C;struct Cyc_Absyn_AggrArrow_e_struct*
_tmpA6B;return(void*)((_tmpA6B=_cycalloc(sizeof(*_tmpA6B)),((_tmpA6B[0]=((
_tmpA6C.tag=23,((_tmpA6C.f1=e,((_tmpA6C.f2=n,((_tmpA6C.f3=0,((_tmpA6C.f4=0,
_tmpA6C)))))))))),_tmpA6B))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmpA6F;
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpA6E;return(void*)((_tmpA6E=_cycalloc(
sizeof(*_tmpA6E)),((_tmpA6E[0]=((_tmpA6F.tag=36,((_tmpA6F.f1=tdopt,((_tmpA6F.f2=
ds,_tmpA6F)))))),_tmpA6E))));}static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_struct _tmpA72;
struct Cyc_Absyn_Goto_s_struct*_tmpA71;return(void*)((_tmpA71=_cycalloc(sizeof(*
_tmpA71)),((_tmpA71[0]=((_tmpA72.tag=8,((_tmpA72.f1=v,((_tmpA72.f2=s,_tmpA72)))))),
_tmpA71))));}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,{.Int_c={4,{
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
_tmpA78;const char*_tmpA77;struct Cyc_Core_Impossible_struct*_tmpA76;(int)_throw((
void*)((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76[0]=((_tmpA78.tag=Cyc_Core_Impossible,((
_tmpA78.f1=((_tmpA77="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpA77,sizeof(char),44))),_tmpA78)))),_tmpA76)))));}}goto _LL0;
_LL3: {struct Cyc_Absyn_FloatType_struct*_tmpC9=(struct Cyc_Absyn_FloatType_struct*)
_tmpC6;if(_tmpC9->tag != 7)goto _LL5;}_LL4: function=fS->ffloat;goto _LL0;_LL5: {
struct Cyc_Absyn_DoubleType_struct*_tmpCA=(struct Cyc_Absyn_DoubleType_struct*)
_tmpC6;if(_tmpCA->tag != 8)goto _LL7;else{_tmpCB=_tmpCA->f1;}}_LL6: switch(_tmpCB){
case 1: _LL10: function=fS->flongdouble;break;default: _LL11: function=fS->fdouble;}
goto _LL0;_LL7: {struct Cyc_Absyn_PointerType_struct*_tmpCC=(struct Cyc_Absyn_PointerType_struct*)
_tmpC6;if(_tmpCC->tag != 5)goto _LL9;}_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;
_LLA: {struct Cyc_Core_Impossible_struct _tmpA85;const char*_tmpA84;void*_tmpA83[1];
struct Cyc_String_pa_struct _tmpA82;struct Cyc_Core_Impossible_struct*_tmpA81;(int)
_throw((void*)((_tmpA81=_cycalloc(sizeof(*_tmpA81)),((_tmpA81[0]=((_tmpA85.tag=
Cyc_Core_Impossible,((_tmpA85.f1=(struct _dyneither_ptr)((_tmpA82.tag=0,((_tmpA82.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA83[
0]=& _tmpA82,Cyc_aprintf(((_tmpA84="impossible expression type %s (not int, float, double, or pointer)",
_tag_dyneither(_tmpA84,sizeof(char),67))),_tag_dyneither(_tmpA83,sizeof(void*),1)))))))),
_tmpA85)))),_tmpA81)))));}_LL0:;}return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(
fS,(void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v);}struct Cyc_Absyn_Exp*
Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*
arr){void*_tmpD5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
arr->topt))->v);struct Cyc_Absyn_PtrInfo _tmpD7;void*_tmpD8;_LL14: {struct Cyc_Absyn_PointerType_struct*
_tmpD6=(struct Cyc_Absyn_PointerType_struct*)_tmpD5;if(_tmpD6->tag != 5)goto _LL16;
else{_tmpD7=_tmpD6->f1;_tmpD8=_tmpD7.elt_typ;}}_LL15: return Cyc_Toc_getFunctionType(
fS,_tmpD8);_LL16:;_LL17: {struct Cyc_Core_Impossible_struct _tmpA8B;const char*
_tmpA8A;struct Cyc_Core_Impossible_struct*_tmpA89;(int)_throw((void*)((_tmpA89=
_cycalloc(sizeof(*_tmpA89)),((_tmpA89[0]=((_tmpA8B.tag=Cyc_Core_Impossible,((
_tmpA8B.f1=((_tmpA8A="impossible type (not pointer)",_tag_dyneither(_tmpA8A,
sizeof(char),30))),_tmpA8B)))),_tmpA89)))));}_LL13:;}struct _tuple15{struct Cyc_List_List*
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
Cyc_Absyn_conref_def)(0,_tmp100);_LL34:;_LL35: {const char*_tmpA8E;void*_tmpA8D;(
_tmpA8D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpA8E="is_nullable",_tag_dyneither(_tmpA8E,sizeof(char),12))),_tag_dyneither(
_tmpA8D,sizeof(void*),0)));}_LL31:;}static char _tmp10F[1]="";static char _tmp110[8]="*bogus*";
static struct _tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct
_dyneither_ptr tag){struct _DynRegionHandle*_tmp104;struct Cyc_Dict_Dict*_tmp105;
struct Cyc_Toc_TocState _tmp103=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp104=_tmp103.dyn;_tmp105=_tmp103.qvar_tags;{static struct _dyneither_ptr
bogus_string={_tmp110,_tmp110,_tmp110 + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
4,0}},& bogus_string};static struct _tuple13 pair={& bogus_qvar,{_tmp10F,_tmp10F,
_tmp10F + 1}};{struct _tuple13 _tmpA8F;pair=((_tmpA8F.f1=x,((_tmpA8F.f2=tag,_tmpA8F))));}{
struct _DynRegionFrame _tmp107;struct _RegionHandle*d=_open_dynregion(& _tmp107,
_tmp104);{struct _tuple0**_tmp108=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,
struct _tuple13*k))Cyc_Dict_lookup_opt)(*_tmp105,(struct _tuple13*)& pair);if(
_tmp108 != 0){struct _tuple0*_tmp109=*_tmp108;_npop_handler(0);return _tmp109;}{
struct _tuple13*_tmpA90;struct _tuple13*_tmp10A=(_tmpA90=_cycalloc(sizeof(*_tmpA90)),((
_tmpA90->f1=x,((_tmpA90->f2=tag,_tmpA90)))));struct _dyneither_ptr*_tmpA93;struct
_tuple0*_tmpA92;struct _tuple0*res=(_tmpA92=_cycalloc(sizeof(*_tmpA92)),((_tmpA92->f1=(*
x).f1,((_tmpA92->f2=((_tmpA93=_cycalloc(sizeof(*_tmpA93)),((_tmpA93[0]=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*x).f2,(struct
_dyneither_ptr)tag),_tmpA93)))),_tmpA92)))));*_tmp105=((struct Cyc_Dict_Dict(*)(
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
_npop_handler(0);return _tmp11B;}};}}{struct Cyc_Int_pa_struct _tmpA9B;void*_tmpA9A[
1];const char*_tmpA99;struct _dyneither_ptr*_tmpA98;struct _dyneither_ptr*xname=(
_tmpA98=_cycalloc(sizeof(*_tmpA98)),((_tmpA98[0]=(struct _dyneither_ptr)((_tmpA9B.tag=
1,((_tmpA9B.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA9A[0]=& _tmpA9B,
Cyc_aprintf(((_tmpA99="_tuple%d",_tag_dyneither(_tmpA99,sizeof(char),9))),
_tag_dyneither(_tmpA9A,sizeof(void*),1)))))))),_tmpA98)));void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp11C=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmpA9E;struct Cyc_List_List*
_tmpA9D;_tmp11C=((_tmpA9D=_cycalloc(sizeof(*_tmpA9D)),((_tmpA9D->hd=((_tmpA9E=
_cycalloc(sizeof(*_tmpA9E)),((_tmpA9E->name=Cyc_Absyn_fieldname(i),((_tmpA9E->tq=
Cyc_Toc_mt_tq,((_tmpA9E->type=(void*)ts2->hd,((_tmpA9E->width=0,((_tmpA9E->attributes=
0,_tmpA9E)))))))))))),((_tmpA9D->tl=_tmp11C,_tmpA9D))))));}}_tmp11C=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp11C);{struct Cyc_Absyn_AggrdeclImpl*
_tmpAA3;struct _tuple0*_tmpAA2;struct Cyc_Absyn_Aggrdecl*_tmpAA1;struct Cyc_Absyn_Aggrdecl*
_tmp11F=(_tmpAA1=_cycalloc(sizeof(*_tmpAA1)),((_tmpAA1->kind=Cyc_Absyn_StructA,((
_tmpAA1->sc=Cyc_Absyn_Public,((_tmpAA1->name=((_tmpAA2=_cycalloc(sizeof(*_tmpAA2)),((
_tmpAA2->f1=Cyc_Absyn_Rel_n(0),((_tmpAA2->f2=xname,_tmpAA2)))))),((_tmpAA1->tvs=
0,((_tmpAA1->impl=((_tmpAA3=_cycalloc(sizeof(*_tmpAA3)),((_tmpAA3->exist_vars=0,((
_tmpAA3->rgn_po=0,((_tmpAA3->fields=_tmp11C,((_tmpAA3->tagged=0,_tmpAA3)))))))))),((
_tmpAA1->attributes=0,_tmpAA1)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*
_tmpAA9;struct Cyc_Absyn_Aggr_d_struct _tmpAA8;struct Cyc_List_List*_tmpAA7;Cyc_Toc_result_decls=((
_tmpAA7=_cycalloc(sizeof(*_tmpAA7)),((_tmpAA7->hd=Cyc_Absyn_new_decl((void*)((
_tmpAA9=_cycalloc(sizeof(*_tmpAA9)),((_tmpAA9[0]=((_tmpAA8.tag=6,((_tmpAA8.f1=
_tmp11F,_tmpAA8)))),_tmpAA9)))),0),((_tmpAA7->tl=Cyc_Toc_result_decls,_tmpAA7))))));}{
struct _tuple16*_tmpAAC;struct Cyc_List_List*_tmpAAB;*_tmp113=((_tmpAAB=
_region_malloc(d,sizeof(*_tmpAAB)),((_tmpAAB->hd=((_tmpAAC=_region_malloc(d,
sizeof(*_tmpAAC)),((_tmpAAC->f1=x,((_tmpAAC->f2=ts,_tmpAAC)))))),((_tmpAAB->tl=*
_tmp113,_tmpAAB))))));}{void*_tmp125=x;_npop_handler(0);return _tmp125;};};};;
_pop_dynregion(d);};}struct _tuple0*Cyc_Toc_temp_var(){int _tmp12D=Cyc_Toc_temp_var_counter
++;struct _dyneither_ptr*_tmpAB9;const char*_tmpAB8;void*_tmpAB7[1];struct Cyc_Int_pa_struct
_tmpAB6;struct _tuple0*_tmpAB5;struct _tuple0*res=(_tmpAB5=_cycalloc(sizeof(*
_tmpAB5)),((_tmpAB5->f1=Cyc_Absyn_Loc_n,((_tmpAB5->f2=((_tmpAB9=_cycalloc(
sizeof(*_tmpAB9)),((_tmpAB9[0]=(struct _dyneither_ptr)((_tmpAB6.tag=1,((_tmpAB6.f1=(
unsigned int)_tmp12D,((_tmpAB7[0]=& _tmpAB6,Cyc_aprintf(((_tmpAB8="_tmp%X",
_tag_dyneither(_tmpAB8,sizeof(char),7))),_tag_dyneither(_tmpAB7,sizeof(void*),1)))))))),
_tmpAB9)))),_tmpAB5)))));return res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp134;struct Cyc_Xarray_Xarray*_tmp135;struct Cyc_Toc_TocState
_tmp133=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp134=
_tmp133.dyn;_tmp135=_tmp133.temp_labels;{struct _DynRegionFrame _tmp136;struct
_RegionHandle*d=_open_dynregion(& _tmp136,_tmp134);{int _tmp137=Cyc_Toc_fresh_label_counter
++;if(_tmp137 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp135)){
struct _dyneither_ptr*_tmp138=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp135,_tmp137);_npop_handler(0);return _tmp138;}{struct Cyc_Int_pa_struct
_tmpAC1;void*_tmpAC0[1];const char*_tmpABF;struct _dyneither_ptr*_tmpABE;struct
_dyneither_ptr*res=(_tmpABE=_cycalloc(sizeof(*_tmpABE)),((_tmpABE[0]=(struct
_dyneither_ptr)((_tmpAC1.tag=1,((_tmpAC1.f1=(unsigned int)_tmp137,((_tmpAC0[0]=&
_tmpAC1,Cyc_aprintf(((_tmpABF="_LL%X",_tag_dyneither(_tmpABF,sizeof(char),6))),
_tag_dyneither(_tmpAC0,sizeof(void*),1)))))))),_tmpABE)));if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp135,res)!= _tmp137){const char*
_tmpAC4;void*_tmpAC3;(_tmpAC3=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAC4="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmpAC4,sizeof(char),43))),_tag_dyneither(_tmpAC3,sizeof(void*),0)));}{
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
_tmp142=_tmp141.f1;_tmp143=_tmp141.f2;_tmp144=_tmp141.f3;{struct _tuple7*_tmpAC5;
return(_tmpAC5=_cycalloc(sizeof(*_tmpAC5)),((_tmpAC5->f1=_tmp142,((_tmpAC5->f2=
_tmp143,((_tmpAC5->f3=Cyc_Toc_typ_to_c(_tmp144),_tmpAC5)))))));};}static struct
_tuple9*Cyc_Toc_typ_to_c_f(struct _tuple9*x){struct Cyc_Absyn_Tqual _tmp147;void*
_tmp148;struct _tuple9 _tmp146=*x;_tmp147=_tmp146.f1;_tmp148=_tmp146.f2;{struct
_tuple9*_tmpAC6;return(_tmpAC6=_cycalloc(sizeof(*_tmpAC6)),((_tmpAC6->f1=_tmp147,((
_tmpAC6->f2=Cyc_Toc_typ_to_c(_tmp148),_tmpAC6)))));};}static void*Cyc_Toc_typ_to_c_array(
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
f){struct Cyc_Absyn_Aggrfield*_tmpAC7;return(_tmpAC7=_cycalloc(sizeof(*_tmpAC7)),((
_tmpAC7->name=f->name,((_tmpAC7->tq=Cyc_Toc_mt_tq,((_tmpAC7->type=Cyc_Toc_typ_to_c(
f->type),((_tmpAC7->width=f->width,((_tmpAC7->attributes=f->attributes,_tmpAC7)))))))))));}
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){return;}static void*Cyc_Toc_char_star_typ(){
static void**cs=0;if(cs == 0){void**_tmpAC8;cs=((_tmpAC8=_cycalloc(sizeof(*_tmpAC8)),((
_tmpAC8[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpAC8))));}return*cs;}static void*Cyc_Toc_rgn_typ(){
static void**r=0;if(r == 0){void**_tmpAC9;r=((_tmpAC9=_cycalloc(sizeof(*_tmpAC9)),((
_tmpAC9[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),
_tmpAC9))));}return*r;}static void*Cyc_Toc_dyn_rgn_typ(){static void**r=0;if(r == 0){
void**_tmpACA;r=((_tmpACA=_cycalloc(sizeof(*_tmpACA)),((_tmpACA[0]=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpACA))));}return*
r;}static void*Cyc_Toc_typ_to_c(void*t){void*_tmp15A=t;struct Cyc_Core_Opt*_tmp15D;
struct Cyc_Core_Opt*_tmp15F;struct Cyc_Core_Opt _tmp160;void*_tmp161;struct Cyc_Absyn_Tvar*
_tmp163;struct Cyc_Absyn_DatatypeFieldInfo _tmp166;union Cyc_Absyn_DatatypeFieldInfoU
_tmp167;struct _tuple1 _tmp168;struct Cyc_Absyn_Datatypedecl*_tmp169;struct Cyc_Absyn_Datatypefield*
_tmp16A;struct Cyc_Absyn_PtrInfo _tmp16D;void*_tmp16E;struct Cyc_Absyn_Tqual _tmp16F;
struct Cyc_Absyn_PtrAtts _tmp170;union Cyc_Absyn_Constraint*_tmp171;struct Cyc_Absyn_ArrayInfo
_tmp176;void*_tmp177;struct Cyc_Absyn_Tqual _tmp178;struct Cyc_Absyn_Exp*_tmp179;
struct Cyc_Position_Segment*_tmp17A;struct Cyc_Absyn_FnInfo _tmp17C;void*_tmp17D;
struct Cyc_List_List*_tmp17E;int _tmp17F;struct Cyc_Absyn_VarargInfo*_tmp180;struct
Cyc_List_List*_tmp181;struct Cyc_List_List*_tmp183;enum Cyc_Absyn_AggrKind _tmp185;
struct Cyc_List_List*_tmp186;struct Cyc_Absyn_AggrInfo _tmp188;union Cyc_Absyn_AggrInfoU
_tmp189;struct Cyc_List_List*_tmp18A;struct _tuple0*_tmp18C;struct Cyc_List_List*
_tmp18E;struct _tuple0*_tmp190;struct Cyc_List_List*_tmp191;struct Cyc_Absyn_Typedefdecl*
_tmp192;void**_tmp193;void*_tmp196;struct Cyc_Absyn_TypeDecl*_tmp19F;struct Cyc_Absyn_TypeDecl
_tmp1A0;void*_tmp1A1;struct Cyc_Absyn_Aggrdecl*_tmp1A3;struct Cyc_Absyn_TypeDecl*
_tmp1A5;struct Cyc_Absyn_TypeDecl _tmp1A6;void*_tmp1A7;struct Cyc_Absyn_Enumdecl*
_tmp1A9;struct Cyc_Absyn_TypeDecl*_tmp1AB;struct Cyc_Absyn_TypeDecl _tmp1AC;void*
_tmp1AD;struct Cyc_Absyn_Datatypedecl*_tmp1AF;void**_tmp1B0;_LL3E: {struct Cyc_Absyn_VoidType_struct*
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
const char*_tmpACB;return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp16A->name,((
_tmpACB="_struct",_tag_dyneither(_tmpACB,sizeof(char),8)))));}_LL4A: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp16B=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp15A;if(_tmp16B->tag != 4)
goto _LL4C;}_LL4B: {const char*_tmpACE;void*_tmpACD;(_tmpACD=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpACE="unresolved DatatypeFieldType",
_tag_dyneither(_tmpACE,sizeof(char),29))),_tag_dyneither(_tmpACD,sizeof(void*),0)));}
_LL4C: {struct Cyc_Absyn_PointerType_struct*_tmp16C=(struct Cyc_Absyn_PointerType_struct*)
_tmp15A;if(_tmp16C->tag != 5)goto _LL4E;else{_tmp16D=_tmp16C->f1;_tmp16E=_tmp16D.elt_typ;
_tmp16F=_tmp16D.elt_tq;_tmp170=_tmp16D.ptr_atts;_tmp171=_tmp170.bounds;}}_LL4D:
_tmp16E=Cyc_Toc_typ_to_c_array(_tmp16E);{void*_tmp1B4=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp171);_LL7D: {
struct Cyc_Absyn_DynEither_b_struct*_tmp1B5=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp1B4;if(_tmp1B5->tag != 0)goto _LL7F;}_LL7E: return Cyc_Toc_dyneither_ptr_typ;
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
_tmp15A;if(_tmp17B->tag != 10)goto _LL58;else{_tmp17C=_tmp17B->f1;_tmp17D=_tmp17C.ret_typ;
_tmp17E=_tmp17C.args;_tmp17F=_tmp17C.c_varargs;_tmp180=_tmp17C.cyc_varargs;
_tmp181=_tmp17C.attributes;}}_LL57: {struct Cyc_List_List*_tmp1B6=0;for(0;_tmp181
!= 0;_tmp181=_tmp181->tl){void*_tmp1B7=(void*)_tmp181->hd;_LL82: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp1B8=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp1B7;if(_tmp1B8->tag != 4)goto
_LL84;}_LL83: goto _LL85;_LL84: {struct Cyc_Absyn_Const_att_struct*_tmp1B9=(struct
Cyc_Absyn_Const_att_struct*)_tmp1B7;if(_tmp1B9->tag != 5)goto _LL86;}_LL85: goto
_LL87;_LL86: {struct Cyc_Absyn_Format_att_struct*_tmp1BA=(struct Cyc_Absyn_Format_att_struct*)
_tmp1B7;if(_tmp1BA->tag != 19)goto _LL88;}_LL87: continue;_LL88: {struct Cyc_Absyn_Noliveunique_att_struct*
_tmp1BB=(struct Cyc_Absyn_Noliveunique_att_struct*)_tmp1B7;if(_tmp1BB->tag != 21)
goto _LL8A;}_LL89: continue;_LL8A: {struct Cyc_Absyn_Initializes_att_struct*_tmp1BC=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp1B7;if(_tmp1BC->tag != 20)goto _LL8C;}
_LL8B: continue;_LL8C:;_LL8D:{struct Cyc_List_List*_tmpACF;_tmp1B6=((_tmpACF=
_cycalloc(sizeof(*_tmpACF)),((_tmpACF->hd=(void*)_tmp181->hd,((_tmpACF->tl=
_tmp1B6,_tmpACF))))));}goto _LL81;_LL81:;}{struct Cyc_List_List*_tmp1BE=((struct
Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_arg_to_c,_tmp17E);if(_tmp180 != 0){void*_tmp1BF=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp180->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple7*_tmpAD0;struct _tuple7*_tmp1C0=(_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((
_tmpAD0->f1=_tmp180->name,((_tmpAD0->f2=_tmp180->tq,((_tmpAD0->f3=_tmp1BF,
_tmpAD0)))))));struct Cyc_List_List*_tmpAD1;_tmp1BE=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1BE,((
_tmpAD1=_cycalloc(sizeof(*_tmpAD1)),((_tmpAD1->hd=_tmp1C0,((_tmpAD1->tl=0,
_tmpAD1)))))));}{struct Cyc_Absyn_FnType_struct _tmpAD7;struct Cyc_Absyn_FnInfo
_tmpAD6;struct Cyc_Absyn_FnType_struct*_tmpAD5;return(void*)((_tmpAD5=_cycalloc(
sizeof(*_tmpAD5)),((_tmpAD5[0]=((_tmpAD7.tag=10,((_tmpAD7.f1=((_tmpAD6.tvars=0,((
_tmpAD6.effect=0,((_tmpAD6.ret_typ=Cyc_Toc_typ_to_c(_tmp17D),((_tmpAD6.args=
_tmp1BE,((_tmpAD6.c_varargs=_tmp17F,((_tmpAD6.cyc_varargs=0,((_tmpAD6.rgn_po=0,((
_tmpAD6.attributes=_tmp1B6,_tmpAD6)))))))))))))))),_tmpAD7)))),_tmpAD5))));};};}
_LL58: {struct Cyc_Absyn_TupleType_struct*_tmp182=(struct Cyc_Absyn_TupleType_struct*)
_tmp15A;if(_tmp182->tag != 11)goto _LL5A;else{_tmp183=_tmp182->f1;}}_LL59: _tmp183=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp183);return Cyc_Toc_add_tuple_type(_tmp183);
_LL5A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp184=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp15A;if(_tmp184->tag != 13)goto _LL5C;else{_tmp185=_tmp184->f1;_tmp186=_tmp184->f2;}}
_LL5B: {struct Cyc_Absyn_AnonAggrType_struct _tmpADA;struct Cyc_Absyn_AnonAggrType_struct*
_tmpAD9;return(void*)((_tmpAD9=_cycalloc(sizeof(*_tmpAD9)),((_tmpAD9[0]=((
_tmpADA.tag=13,((_tmpADA.f1=_tmp185,((_tmpADA.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp186),_tmpADA)))))),_tmpAD9))));}_LL5C: {
struct Cyc_Absyn_AggrType_struct*_tmp187=(struct Cyc_Absyn_AggrType_struct*)
_tmp15A;if(_tmp187->tag != 12)goto _LL5E;else{_tmp188=_tmp187->f1;_tmp189=_tmp188.aggr_info;
_tmp18A=_tmp188.targs;}}_LL5D:{union Cyc_Absyn_AggrInfoU _tmp1C8=_tmp189;struct
_tuple2 _tmp1C9;_LL8F: if((_tmp1C8.UnknownAggr).tag != 1)goto _LL91;_tmp1C9=(struct
_tuple2)(_tmp1C8.UnknownAggr).val;_LL90: return t;_LL91:;_LL92: goto _LL8E;_LL8E:;}{
struct Cyc_Absyn_Aggrdecl*_tmp1CA=Cyc_Absyn_get_known_aggrdecl(_tmp189);if(
_tmp1CA->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(_tmp1CA->name,Cyc_Absyn_unionq_typ);
else{return Cyc_Toc_aggrdecl_type(_tmp1CA->name,Cyc_Absyn_strctq);}};_LL5E: {
struct Cyc_Absyn_EnumType_struct*_tmp18B=(struct Cyc_Absyn_EnumType_struct*)
_tmp15A;if(_tmp18B->tag != 14)goto _LL60;else{_tmp18C=_tmp18B->f1;}}_LL5F: return t;
_LL60: {struct Cyc_Absyn_AnonEnumType_struct*_tmp18D=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp15A;if(_tmp18D->tag != 15)goto _LL62;else{_tmp18E=_tmp18D->f1;}}_LL61: Cyc_Toc_enumfields_to_c(
_tmp18E);return t;_LL62: {struct Cyc_Absyn_TypedefType_struct*_tmp18F=(struct Cyc_Absyn_TypedefType_struct*)
_tmp15A;if(_tmp18F->tag != 18)goto _LL64;else{_tmp190=_tmp18F->f1;_tmp191=_tmp18F->f2;
_tmp192=_tmp18F->f3;_tmp193=_tmp18F->f4;}}_LL63: if(_tmp193 == 0  || Cyc_noexpand_r){
if(_tmp191 != 0){struct Cyc_Absyn_TypedefType_struct _tmpADD;struct Cyc_Absyn_TypedefType_struct*
_tmpADC;return(void*)((_tmpADC=_cycalloc(sizeof(*_tmpADC)),((_tmpADC[0]=((
_tmpADD.tag=18,((_tmpADD.f1=_tmp190,((_tmpADD.f2=0,((_tmpADD.f3=_tmp192,((
_tmpADD.f4=0,_tmpADD)))))))))),_tmpADC))));}else{return t;}}else{struct Cyc_Absyn_TypedefType_struct
_tmpAE3;void**_tmpAE2;struct Cyc_Absyn_TypedefType_struct*_tmpAE1;return(void*)((
_tmpAE1=_cycalloc(sizeof(*_tmpAE1)),((_tmpAE1[0]=((_tmpAE3.tag=18,((_tmpAE3.f1=
_tmp190,((_tmpAE3.f2=0,((_tmpAE3.f3=_tmp192,((_tmpAE3.f4=((_tmpAE2=_cycalloc(
sizeof(*_tmpAE2)),((_tmpAE2[0]=Cyc_Toc_typ_to_c_array(*_tmp193),_tmpAE2)))),
_tmpAE3)))))))))),_tmpAE1))));}_LL64: {struct Cyc_Absyn_TagType_struct*_tmp194=(
struct Cyc_Absyn_TagType_struct*)_tmp15A;if(_tmp194->tag != 20)goto _LL66;}_LL65:
return Cyc_Absyn_uint_typ;_LL66: {struct Cyc_Absyn_RgnHandleType_struct*_tmp195=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp15A;if(_tmp195->tag != 16)goto _LL68;
else{_tmp196=(void*)_tmp195->f1;}}_LL67: return Cyc_Toc_rgn_typ();_LL68: {struct
Cyc_Absyn_DynRgnType_struct*_tmp197=(struct Cyc_Absyn_DynRgnType_struct*)_tmp15A;
if(_tmp197->tag != 17)goto _LL6A;}_LL69: return Cyc_Toc_dyn_rgn_typ();_LL6A: {struct
Cyc_Absyn_HeapRgn_struct*_tmp198=(struct Cyc_Absyn_HeapRgn_struct*)_tmp15A;if(
_tmp198->tag != 21)goto _LL6C;}_LL6B: {const char*_tmpAE6;void*_tmpAE5;(_tmpAE5=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAE6="Toc::typ_to_c: type translation passed the heap region",_tag_dyneither(
_tmpAE6,sizeof(char),55))),_tag_dyneither(_tmpAE5,sizeof(void*),0)));}_LL6C: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp199=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp15A;if(_tmp199->tag != 22)goto _LL6E;}_LL6D: {const char*_tmpAE9;void*_tmpAE8;(
_tmpAE8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAE9="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpAE9,sizeof(char),57))),_tag_dyneither(_tmpAE8,sizeof(void*),0)));}
_LL6E: {struct Cyc_Absyn_AccessEff_struct*_tmp19A=(struct Cyc_Absyn_AccessEff_struct*)
_tmp15A;if(_tmp19A->tag != 23)goto _LL70;}_LL6F: goto _LL71;_LL70: {struct Cyc_Absyn_JoinEff_struct*
_tmp19B=(struct Cyc_Absyn_JoinEff_struct*)_tmp15A;if(_tmp19B->tag != 24)goto _LL72;}
_LL71: goto _LL73;_LL72: {struct Cyc_Absyn_RgnsEff_struct*_tmp19C=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp15A;if(_tmp19C->tag != 25)goto _LL74;}_LL73: {const char*_tmpAEC;void*_tmpAEB;(
_tmpAEB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAEC="Toc::typ_to_c: type translation passed an effect",_tag_dyneither(
_tmpAEC,sizeof(char),49))),_tag_dyneither(_tmpAEB,sizeof(void*),0)));}_LL74: {
struct Cyc_Absyn_ValueofType_struct*_tmp19D=(struct Cyc_Absyn_ValueofType_struct*)
_tmp15A;if(_tmp19D->tag != 19)goto _LL76;}_LL75: {const char*_tmpAEF;void*_tmpAEE;(
_tmpAEE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAEF="Toc::typ_to_c: type translation passed a valueof_t",_tag_dyneither(
_tmpAEF,sizeof(char),51))),_tag_dyneither(_tmpAEE,sizeof(void*),0)));}_LL76: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp19E=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp15A;if(_tmp19E->tag != 26)goto _LL78;else{_tmp19F=_tmp19E->f1;_tmp1A0=*_tmp19F;
_tmp1A1=_tmp1A0.r;{struct Cyc_Absyn_Aggr_td_struct*_tmp1A2=(struct Cyc_Absyn_Aggr_td_struct*)
_tmp1A1;if(_tmp1A2->tag != 0)goto _LL78;else{_tmp1A3=_tmp1A2->f1;}};}}_LL77: Cyc_Toc_aggrdecl_to_c(
_tmp1A3,1);if(_tmp1A3->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(
_tmp1A3->name,Cyc_Absyn_unionq_typ);else{return Cyc_Toc_aggrdecl_type(_tmp1A3->name,
Cyc_Absyn_strctq);}_LL78: {struct Cyc_Absyn_TypeDeclType_struct*_tmp1A4=(struct
Cyc_Absyn_TypeDeclType_struct*)_tmp15A;if(_tmp1A4->tag != 26)goto _LL7A;else{
_tmp1A5=_tmp1A4->f1;_tmp1A6=*_tmp1A5;_tmp1A7=_tmp1A6.r;{struct Cyc_Absyn_Enum_td_struct*
_tmp1A8=(struct Cyc_Absyn_Enum_td_struct*)_tmp1A7;if(_tmp1A8->tag != 1)goto _LL7A;
else{_tmp1A9=_tmp1A8->f1;}};}}_LL79: Cyc_Toc_enumdecl_to_c(_tmp1A9);return t;_LL7A: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp1AA=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp15A;if(_tmp1AA->tag != 26)goto _LL3D;else{_tmp1AB=_tmp1AA->f1;_tmp1AC=*_tmp1AB;
_tmp1AD=_tmp1AC.r;{struct Cyc_Absyn_Datatype_td_struct*_tmp1AE=(struct Cyc_Absyn_Datatype_td_struct*)
_tmp1AD;if(_tmp1AE->tag != 2)goto _LL3D;else{_tmp1AF=_tmp1AE->f1;}};_tmp1B0=
_tmp1AA->f2;}}_LL7B: Cyc_Toc_datatypedecl_to_c(_tmp1AF);return Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp1B0)));_LL3D:;}static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(
void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmp1D8=t;struct
Cyc_Absyn_ArrayInfo _tmp1DA;void*_tmp1DB;struct Cyc_Absyn_Tqual _tmp1DC;_LL94: {
struct Cyc_Absyn_ArrayType_struct*_tmp1D9=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1D8;if(_tmp1D9->tag != 9)goto _LL96;else{_tmp1DA=_tmp1D9->f1;_tmp1DB=_tmp1DA.elt_type;
_tmp1DC=_tmp1DA.tq;}}_LL95: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp1DB,(
void*)& Cyc_Absyn_HeapRgn_val,_tmp1DC,Cyc_Absyn_false_conref),e);_LL96:;_LL97:
return Cyc_Toc_cast_it(t,e);_LL93:;}static int Cyc_Toc_atomic_typ(void*t){void*
_tmp1DD=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp1E8;void*_tmp1E9;
struct Cyc_Absyn_AggrInfo _tmp1EB;union Cyc_Absyn_AggrInfoU _tmp1EC;struct Cyc_List_List*
_tmp1EE;struct Cyc_Absyn_DatatypeFieldInfo _tmp1F0;union Cyc_Absyn_DatatypeFieldInfoU
_tmp1F1;struct _tuple1 _tmp1F2;struct Cyc_Absyn_Datatypedecl*_tmp1F3;struct Cyc_Absyn_Datatypefield*
_tmp1F4;struct Cyc_List_List*_tmp1F6;_LL99: {struct Cyc_Absyn_VoidType_struct*
_tmp1DE=(struct Cyc_Absyn_VoidType_struct*)_tmp1DD;if(_tmp1DE->tag != 0)goto _LL9B;}
_LL9A: return 1;_LL9B: {struct Cyc_Absyn_VarType_struct*_tmp1DF=(struct Cyc_Absyn_VarType_struct*)
_tmp1DD;if(_tmp1DF->tag != 2)goto _LL9D;}_LL9C: return 0;_LL9D: {struct Cyc_Absyn_IntType_struct*
_tmp1E0=(struct Cyc_Absyn_IntType_struct*)_tmp1DD;if(_tmp1E0->tag != 6)goto _LL9F;}
_LL9E: goto _LLA0;_LL9F: {struct Cyc_Absyn_EnumType_struct*_tmp1E1=(struct Cyc_Absyn_EnumType_struct*)
_tmp1DD;if(_tmp1E1->tag != 14)goto _LLA1;}_LLA0: goto _LLA2;_LLA1: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp1E2=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp1DD;if(_tmp1E2->tag != 15)goto
_LLA3;}_LLA2: goto _LLA4;_LLA3: {struct Cyc_Absyn_FloatType_struct*_tmp1E3=(struct
Cyc_Absyn_FloatType_struct*)_tmp1DD;if(_tmp1E3->tag != 7)goto _LLA5;}_LLA4: goto
_LLA6;_LLA5: {struct Cyc_Absyn_DoubleType_struct*_tmp1E4=(struct Cyc_Absyn_DoubleType_struct*)
_tmp1DD;if(_tmp1E4->tag != 8)goto _LLA7;}_LLA6: goto _LLA8;_LLA7: {struct Cyc_Absyn_FnType_struct*
_tmp1E5=(struct Cyc_Absyn_FnType_struct*)_tmp1DD;if(_tmp1E5->tag != 10)goto _LLA9;}
_LLA8: goto _LLAA;_LLA9: {struct Cyc_Absyn_TagType_struct*_tmp1E6=(struct Cyc_Absyn_TagType_struct*)
_tmp1DD;if(_tmp1E6->tag != 20)goto _LLAB;}_LLAA: return 1;_LLAB: {struct Cyc_Absyn_ArrayType_struct*
_tmp1E7=(struct Cyc_Absyn_ArrayType_struct*)_tmp1DD;if(_tmp1E7->tag != 9)goto _LLAD;
else{_tmp1E8=_tmp1E7->f1;_tmp1E9=_tmp1E8.elt_type;}}_LLAC: return Cyc_Toc_atomic_typ(
_tmp1E9);_LLAD: {struct Cyc_Absyn_AggrType_struct*_tmp1EA=(struct Cyc_Absyn_AggrType_struct*)
_tmp1DD;if(_tmp1EA->tag != 12)goto _LLAF;else{_tmp1EB=_tmp1EA->f1;_tmp1EC=_tmp1EB.aggr_info;}}
_LLAE:{union Cyc_Absyn_AggrInfoU _tmp1FB=_tmp1EC;struct _tuple2 _tmp1FC;_LLC0: if((
_tmp1FB.UnknownAggr).tag != 1)goto _LLC2;_tmp1FC=(struct _tuple2)(_tmp1FB.UnknownAggr).val;
_LLC1: return 0;_LLC2:;_LLC3: goto _LLBF;_LLBF:;}{struct Cyc_Absyn_Aggrdecl*_tmp1FD=
Cyc_Absyn_get_known_aggrdecl(_tmp1EC);if(_tmp1FD->impl == 0)return 0;{struct Cyc_List_List*
_tmp1FE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1FD->impl))->fields;for(
0;_tmp1FE != 0;_tmp1FE=_tmp1FE->tl){if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)
_tmp1FE->hd)->type))return 0;}}return 1;};_LLAF: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp1ED=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp1DD;if(_tmp1ED->tag != 13)goto
_LLB1;else{_tmp1EE=_tmp1ED->f2;}}_LLB0: for(0;_tmp1EE != 0;_tmp1EE=_tmp1EE->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1EE->hd)->type))return 0;}
return 1;_LLB1: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp1EF=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp1DD;if(_tmp1EF->tag != 4)goto _LLB3;else{_tmp1F0=_tmp1EF->f1;_tmp1F1=_tmp1F0.field_info;
if((_tmp1F1.KnownDatatypefield).tag != 2)goto _LLB3;_tmp1F2=(struct _tuple1)(
_tmp1F1.KnownDatatypefield).val;_tmp1F3=_tmp1F2.f1;_tmp1F4=_tmp1F2.f2;}}_LLB2:
_tmp1F6=_tmp1F4->typs;goto _LLB4;_LLB3: {struct Cyc_Absyn_TupleType_struct*_tmp1F5=(
struct Cyc_Absyn_TupleType_struct*)_tmp1DD;if(_tmp1F5->tag != 11)goto _LLB5;else{
_tmp1F6=_tmp1F5->f1;}}_LLB4: for(0;_tmp1F6 != 0;_tmp1F6=_tmp1F6->tl){if(!Cyc_Toc_atomic_typ((*((
struct _tuple9*)_tmp1F6->hd)).f2))return 0;}return 1;_LLB5: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1F7=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1DD;if(_tmp1F7->tag != 3)goto
_LLB7;}_LLB6: goto _LLB8;_LLB7: {struct Cyc_Absyn_PointerType_struct*_tmp1F8=(
struct Cyc_Absyn_PointerType_struct*)_tmp1DD;if(_tmp1F8->tag != 5)goto _LLB9;}_LLB8:
goto _LLBA;_LLB9: {struct Cyc_Absyn_DynRgnType_struct*_tmp1F9=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp1DD;if(_tmp1F9->tag != 17)goto _LLBB;}_LLBA: goto _LLBC;_LLBB: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp1FA=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp1DD;if(_tmp1FA->tag != 16)goto
_LLBD;}_LLBC: return 0;_LLBD:;_LLBE: {const char*_tmpAF3;void*_tmpAF2[1];struct Cyc_String_pa_struct
_tmpAF1;(_tmpAF1.tag=0,((_tmpAF1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpAF2[0]=& _tmpAF1,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF3="atomic_typ:  bad type %s",
_tag_dyneither(_tmpAF3,sizeof(char),25))),_tag_dyneither(_tmpAF2,sizeof(void*),1)))))));}
_LL98:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp202=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp204;void*_tmp205;_LLC5: {struct Cyc_Absyn_PointerType_struct*
_tmp203=(struct Cyc_Absyn_PointerType_struct*)_tmp202;if(_tmp203->tag != 5)goto
_LLC7;else{_tmp204=_tmp203->f1;_tmp205=_tmp204.elt_typ;}}_LLC6: {void*_tmp206=
Cyc_Tcutil_compress(_tmp205);_LLCA: {struct Cyc_Absyn_VoidType_struct*_tmp207=(
struct Cyc_Absyn_VoidType_struct*)_tmp206;if(_tmp207->tag != 0)goto _LLCC;}_LLCB:
return 1;_LLCC:;_LLCD: return 0;_LLC9:;}_LLC7:;_LLC8: return 0;_LLC4:;}static int Cyc_Toc_is_tvar(
void*t){void*_tmp208=Cyc_Tcutil_compress(t);_LLCF: {struct Cyc_Absyn_VarType_struct*
_tmp209=(struct Cyc_Absyn_VarType_struct*)_tmp208;if(_tmp209->tag != 2)goto _LLD1;}
_LLD0: return 1;_LLD1:;_LLD2: return 0;_LLCE:;}static int Cyc_Toc_is_void_star_or_tvar(
void*t){return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_tvar(t);}static int Cyc_Toc_is_poly_field(
void*t,struct _dyneither_ptr*f){void*_tmp20A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp20C;union Cyc_Absyn_AggrInfoU _tmp20D;struct Cyc_List_List*_tmp20F;_LLD4: {
struct Cyc_Absyn_AggrType_struct*_tmp20B=(struct Cyc_Absyn_AggrType_struct*)
_tmp20A;if(_tmp20B->tag != 12)goto _LLD6;else{_tmp20C=_tmp20B->f1;_tmp20D=_tmp20C.aggr_info;}}
_LLD5: {struct Cyc_Absyn_Aggrdecl*_tmp210=Cyc_Absyn_get_known_aggrdecl(_tmp20D);
if(_tmp210->impl == 0){const char*_tmpAF6;void*_tmpAF5;(_tmpAF5=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF6="is_poly_field: type missing fields",
_tag_dyneither(_tmpAF6,sizeof(char),35))),_tag_dyneither(_tmpAF5,sizeof(void*),0)));}
_tmp20F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp210->impl))->fields;goto
_LLD7;}_LLD6: {struct Cyc_Absyn_AnonAggrType_struct*_tmp20E=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp20A;if(_tmp20E->tag != 13)goto _LLD8;else{_tmp20F=_tmp20E->f2;}}_LLD7: {struct
Cyc_Absyn_Aggrfield*_tmp213=Cyc_Absyn_lookup_field(_tmp20F,f);if(_tmp213 == 0){
const char*_tmpAFA;void*_tmpAF9[1];struct Cyc_String_pa_struct _tmpAF8;(_tmpAF8.tag=
0,((_tmpAF8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpAF9[0]=&
_tmpAF8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAFA="is_poly_field: bad field %s",_tag_dyneither(_tmpAFA,sizeof(char),28))),
_tag_dyneither(_tmpAF9,sizeof(void*),1)))))));}return Cyc_Toc_is_void_star_or_tvar(
_tmp213->type);}_LLD8:;_LLD9: {const char*_tmpAFE;void*_tmpAFD[1];struct Cyc_String_pa_struct
_tmpAFC;(_tmpAFC.tag=0,((_tmpAFC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpAFD[0]=& _tmpAFC,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAFE="is_poly_field: bad type %s",
_tag_dyneither(_tmpAFE,sizeof(char),27))),_tag_dyneither(_tmpAFD,sizeof(void*),1)))))));}
_LLD3:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmp21A=e->r;
struct Cyc_Absyn_Exp*_tmp21C;struct _dyneither_ptr*_tmp21D;struct Cyc_Absyn_Exp*
_tmp21F;struct _dyneither_ptr*_tmp220;_LLDB: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp21B=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp21A;if(_tmp21B->tag != 22)goto
_LLDD;else{_tmp21C=_tmp21B->f1;_tmp21D=_tmp21B->f2;}}_LLDC: return Cyc_Toc_is_poly_field((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp21C->topt))->v,_tmp21D);_LLDD: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp21E=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp21A;if(_tmp21E->tag != 23)goto _LLDF;else{_tmp21F=_tmp21E->f1;_tmp220=_tmp21E->f2;}}
_LLDE: {void*_tmp221=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp21F->topt))->v);struct Cyc_Absyn_PtrInfo _tmp223;void*_tmp224;
_LLE2: {struct Cyc_Absyn_PointerType_struct*_tmp222=(struct Cyc_Absyn_PointerType_struct*)
_tmp221;if(_tmp222->tag != 5)goto _LLE4;else{_tmp223=_tmp222->f1;_tmp224=_tmp223.elt_typ;}}
_LLE3: return Cyc_Toc_is_poly_field(_tmp224,_tmp220);_LLE4:;_LLE5: {const char*
_tmpB02;void*_tmpB01[1];struct Cyc_String_pa_struct _tmpB00;(_tmpB00.tag=0,((
_tmpB00.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp21F->topt))->v)),((_tmpB01[0]=&
_tmpB00,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB02="is_poly_project: bad type %s",_tag_dyneither(_tmpB02,sizeof(char),29))),
_tag_dyneither(_tmpB01,sizeof(void*),1)))))));}_LLE1:;}_LLDF:;_LLE0: return 0;
_LLDA:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB03;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((
_tmpB03=_cycalloc(sizeof(*_tmpB03)),((_tmpB03->hd=s,((_tmpB03->tl=0,_tmpB03)))))),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB04;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((
_tmpB04=_cycalloc(sizeof(*_tmpB04)),((_tmpB04->hd=s,((_tmpB04->tl=0,_tmpB04)))))),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(
s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmpB05[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((
_tmpB05[1]=s,((_tmpB05[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpB05,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,
struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*
_tmpB06[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpB06[1]=n,((
_tmpB06[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB06,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{struct Cyc_Absyn_Exp*
_tmpB07[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpB07[1]=n,((
_tmpB07[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB07,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n){struct Cyc_Absyn_Exp*_tmpB08[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((
_tmpB08[2]=n,((_tmpB08[1]=s,((_tmpB08[0]=rgn,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB08,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmpB09;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmpB09=_cycalloc(sizeof(*_tmpB09)),((_tmpB09->hd=e,((_tmpB09->tl=0,_tmpB09)))))),
0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){struct Cyc_Absyn_Exp*
_tmpB0A[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpB0A,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*
_tmp230=e->r;union Cyc_Absyn_Cnst _tmp232;struct _dyneither_ptr _tmp233;_LLE7: {
struct Cyc_Absyn_Const_e_struct*_tmp231=(struct Cyc_Absyn_Const_e_struct*)_tmp230;
if(_tmp231->tag != 0)goto _LLE9;else{_tmp232=_tmp231->f1;if((_tmp232.String_c).tag
!= 7)goto _LLE9;_tmp233=(struct _dyneither_ptr)(_tmp232.String_c).val;}}_LLE8:
is_string=1;goto _LLE6;_LLE9:;_LLEA: goto _LLE6;_LLE6:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple0*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_struct*_tmpB10;struct
Cyc_Absyn_Var_d_struct _tmpB0F;struct Cyc_List_List*_tmpB0E;Cyc_Toc_result_decls=((
_tmpB0E=_cycalloc(sizeof(*_tmpB0E)),((_tmpB0E->hd=Cyc_Absyn_new_decl((void*)((
_tmpB10=_cycalloc(sizeof(*_tmpB10)),((_tmpB10[0]=((_tmpB0F.tag=0,((_tmpB0F.f1=vd,
_tmpB0F)))),_tmpB10)))),0),((_tmpB0E->tl=Cyc_Toc_result_decls,_tmpB0E))))));}
xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*urm_exp;{struct _tuple15*_tmpB17;
struct _tuple15*_tmpB16;struct _tuple15*_tmpB15;struct _tuple15*_tmpB14[3];urm_exp=
Cyc_Absyn_unresolvedmem_exp(0,((_tmpB14[2]=((_tmpB15=_cycalloc(sizeof(*_tmpB15)),((
_tmpB15->f1=0,((_tmpB15->f2=xplussz,_tmpB15)))))),((_tmpB14[1]=((_tmpB16=
_cycalloc(sizeof(*_tmpB16)),((_tmpB16->f1=0,((_tmpB16->f2=xexp,_tmpB16)))))),((
_tmpB14[0]=((_tmpB17=_cycalloc(sizeof(*_tmpB17)),((_tmpB17->f1=0,((_tmpB17->f2=
xexp,_tmpB17)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB14,sizeof(struct _tuple15*),3)))))))),0);}return urm_exp;};};}
struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*
binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct
_dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;struct _RegionHandle*rgn;};
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp23C;int
_tmp23D;struct Cyc_Toc_Env*_tmp23B=nv;_tmp23C=*_tmp23B;_tmp23D=_tmp23C.toplevel;
return _tmp23D;}static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*
nv,struct _tuple0*x){struct Cyc_Toc_Env _tmp23F;struct Cyc_Dict_Dict _tmp240;struct
Cyc_Toc_Env*_tmp23E=nv;_tmp23F=*_tmp23E;_tmp240=_tmp23F.varmap;return((struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp240,x);}static struct
Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){struct Cyc_Toc_Env*_tmpB18;
return(_tmpB18=_region_malloc(r,sizeof(*_tmpB18)),((_tmpB18->break_lab=(struct
_dyneither_ptr**)0,((_tmpB18->continue_lab=(struct _dyneither_ptr**)0,((_tmpB18->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpB18->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp),((_tmpB18->toplevel=(int)1,((_tmpB18->rgn=(struct
_RegionHandle*)r,_tmpB18)))))))))))));}static struct Cyc_Toc_Env*Cyc_Toc_share_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp243;struct
_dyneither_ptr**_tmp244;struct _dyneither_ptr**_tmp245;struct Cyc_Toc_FallthruInfo*
_tmp246;struct Cyc_Dict_Dict _tmp247;int _tmp248;struct Cyc_Toc_Env*_tmp242=e;
_tmp243=*_tmp242;_tmp244=_tmp243.break_lab;_tmp245=_tmp243.continue_lab;_tmp246=
_tmp243.fallthru_info;_tmp247=_tmp243.varmap;_tmp248=_tmp243.toplevel;{struct Cyc_Toc_Env*
_tmpB19;return(_tmpB19=_region_malloc(r,sizeof(*_tmpB19)),((_tmpB19->break_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp244),((_tmpB19->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp245),((_tmpB19->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp246,((_tmpB19->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp247),((_tmpB19->toplevel=(int)_tmp248,((_tmpB19->rgn=(struct _RegionHandle*)
r,_tmpB19)))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct
_RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp24B;struct
_dyneither_ptr**_tmp24C;struct _dyneither_ptr**_tmp24D;struct Cyc_Toc_FallthruInfo*
_tmp24E;struct Cyc_Dict_Dict _tmp24F;int _tmp250;struct Cyc_Toc_Env*_tmp24A=e;
_tmp24B=*_tmp24A;_tmp24C=_tmp24B.break_lab;_tmp24D=_tmp24B.continue_lab;_tmp24E=
_tmp24B.fallthru_info;_tmp24F=_tmp24B.varmap;_tmp250=_tmp24B.toplevel;{struct Cyc_Toc_Env*
_tmpB1A;return(_tmpB1A=_region_malloc(r,sizeof(*_tmpB1A)),((_tmpB1A->break_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp24C),((_tmpB1A->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp24D),((_tmpB1A->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp24E,((_tmpB1A->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp24F),((_tmpB1A->toplevel=(int)0,((_tmpB1A->rgn=(struct _RegionHandle*)r,
_tmpB1A)))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct
_RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp253;struct
_dyneither_ptr**_tmp254;struct _dyneither_ptr**_tmp255;struct Cyc_Toc_FallthruInfo*
_tmp256;struct Cyc_Dict_Dict _tmp257;int _tmp258;struct Cyc_Toc_Env*_tmp252=e;
_tmp253=*_tmp252;_tmp254=_tmp253.break_lab;_tmp255=_tmp253.continue_lab;_tmp256=
_tmp253.fallthru_info;_tmp257=_tmp253.varmap;_tmp258=_tmp253.toplevel;{struct Cyc_Toc_Env*
_tmpB1B;return(_tmpB1B=_region_malloc(r,sizeof(*_tmpB1B)),((_tmpB1B->break_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp254),((_tmpB1B->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp255),((_tmpB1B->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp256,((_tmpB1B->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp257),((_tmpB1B->toplevel=(int)1,((_tmpB1B->rgn=(struct _RegionHandle*)r,
_tmpB1B)))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct
_RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace _tmp25A=(*x).f1;struct Cyc_List_List*_tmp25B;_LLEC: if((
_tmp25A.Rel_n).tag != 1)goto _LLEE;_tmp25B=(struct Cyc_List_List*)(_tmp25A.Rel_n).val;
_LLED: {const char*_tmpB1F;void*_tmpB1E[1];struct Cyc_String_pa_struct _tmpB1D;(
_tmpB1D.tag=0,((_tmpB1D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
x)),((_tmpB1E[0]=& _tmpB1D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB1F="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(
_tmpB1F,sizeof(char),30))),_tag_dyneither(_tmpB1E,sizeof(void*),1)))))));}_LLEE:;
_LLEF: goto _LLEB;_LLEB:;}{struct Cyc_Toc_Env _tmp260;struct _dyneither_ptr**_tmp261;
struct _dyneither_ptr**_tmp262;struct Cyc_Toc_FallthruInfo*_tmp263;struct Cyc_Dict_Dict
_tmp264;int _tmp265;struct Cyc_Toc_Env*_tmp25F=e;_tmp260=*_tmp25F;_tmp261=_tmp260.break_lab;
_tmp262=_tmp260.continue_lab;_tmp263=_tmp260.fallthru_info;_tmp264=_tmp260.varmap;
_tmp265=_tmp260.toplevel;{struct Cyc_Dict_Dict _tmp266=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp264),x,y);struct Cyc_Toc_Env*_tmpB20;return(_tmpB20=_region_malloc(r,
sizeof(*_tmpB20)),((_tmpB20->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp261),((_tmpB20->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp262),((_tmpB20->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp263,((_tmpB20->varmap=(struct Cyc_Dict_Dict)_tmp266,((_tmpB20->toplevel=(int)
_tmp265,((_tmpB20->rgn=(struct _RegionHandle*)r,_tmpB20)))))))))))));};};}static
struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env _tmp269;struct _dyneither_ptr**_tmp26A;struct _dyneither_ptr**
_tmp26B;struct Cyc_Toc_FallthruInfo*_tmp26C;struct Cyc_Dict_Dict _tmp26D;int _tmp26E;
struct Cyc_Toc_Env*_tmp268=e;_tmp269=*_tmp268;_tmp26A=_tmp269.break_lab;_tmp26B=
_tmp269.continue_lab;_tmp26C=_tmp269.fallthru_info;_tmp26D=_tmp269.varmap;
_tmp26E=_tmp269.toplevel;{struct Cyc_Toc_Env*_tmpB21;return(_tmpB21=
_region_malloc(r,sizeof(*_tmpB21)),((_tmpB21->break_lab=(struct _dyneither_ptr**)
0,((_tmpB21->continue_lab=(struct _dyneither_ptr**)0,((_tmpB21->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp26C,((_tmpB21->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp26D),((_tmpB21->toplevel=(int)_tmp26E,((_tmpB21->rgn=(struct _RegionHandle*)
r,_tmpB21)))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmpB22;fallthru_vars=((
_tmpB22=_region_malloc(r,sizeof(*_tmpB22)),((_tmpB22->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmpB22->tl=fallthru_vars,_tmpB22))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp272;struct _dyneither_ptr**_tmp273;struct
_dyneither_ptr**_tmp274;struct Cyc_Toc_FallthruInfo*_tmp275;struct Cyc_Dict_Dict
_tmp276;int _tmp277;struct Cyc_Toc_Env*_tmp271=e;_tmp272=*_tmp271;_tmp273=_tmp272.break_lab;
_tmp274=_tmp272.continue_lab;_tmp275=_tmp272.fallthru_info;_tmp276=_tmp272.varmap;
_tmp277=_tmp272.toplevel;{struct Cyc_Toc_Env _tmp279;struct Cyc_Dict_Dict _tmp27A;
struct Cyc_Toc_Env*_tmp278=next_case_env;_tmp279=*_tmp278;_tmp27A=_tmp279.varmap;{
struct Cyc_Toc_FallthruInfo*_tmpB23;struct Cyc_Toc_FallthruInfo*fi=(_tmpB23=
_region_malloc(r,sizeof(*_tmpB23)),((_tmpB23->label=fallthru_l,((_tmpB23->binders=
fallthru_vars,((_tmpB23->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp27A),_tmpB23)))))));
struct _dyneither_ptr**_tmpB26;struct Cyc_Toc_Env*_tmpB25;return(_tmpB25=
_region_malloc(r,sizeof(*_tmpB25)),((_tmpB25->break_lab=(struct _dyneither_ptr**)((
_tmpB26=_region_malloc(r,sizeof(*_tmpB26)),((_tmpB26[0]=break_l,_tmpB26)))),((
_tmpB25->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp274),((
_tmpB25->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpB25->varmap=(struct
Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp276),((_tmpB25->toplevel=(int)_tmp277,((_tmpB25->rgn=(
struct _RegionHandle*)r,_tmpB25)))))))))))));};};};}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){struct
Cyc_Toc_Env _tmp27F;struct _dyneither_ptr**_tmp280;struct _dyneither_ptr**_tmp281;
struct Cyc_Toc_FallthruInfo*_tmp282;struct Cyc_Dict_Dict _tmp283;int _tmp284;struct
Cyc_Toc_Env*_tmp27E=e;_tmp27F=*_tmp27E;_tmp280=_tmp27F.break_lab;_tmp281=_tmp27F.continue_lab;
_tmp282=_tmp27F.fallthru_info;_tmp283=_tmp27F.varmap;_tmp284=_tmp27F.toplevel;{
struct _dyneither_ptr**_tmpB29;struct Cyc_Toc_Env*_tmpB28;return(_tmpB28=
_region_malloc(r,sizeof(*_tmpB28)),((_tmpB28->break_lab=(struct _dyneither_ptr**)((
_tmpB29=_region_malloc(r,sizeof(*_tmpB29)),((_tmpB29[0]=break_l,_tmpB29)))),((
_tmpB28->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp281),((
_tmpB28->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpB28->varmap=(struct
Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp283),((_tmpB28->toplevel=(int)_tmp284,((_tmpB28->rgn=(
struct _RegionHandle*)r,_tmpB28)))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp288;struct _dyneither_ptr**_tmp289;struct _dyneither_ptr**_tmp28A;struct Cyc_Toc_FallthruInfo*
_tmp28B;struct Cyc_Dict_Dict _tmp28C;int _tmp28D;struct Cyc_Toc_Env*_tmp287=e;
_tmp288=*_tmp287;_tmp289=_tmp288.break_lab;_tmp28A=_tmp288.continue_lab;_tmp28B=
_tmp288.fallthru_info;_tmp28C=_tmp288.varmap;_tmp28D=_tmp288.toplevel;{struct Cyc_Toc_FallthruInfo*
_tmpB2C;struct Cyc_Toc_Env*_tmpB2B;return(_tmpB2B=_region_malloc(r,sizeof(*
_tmpB2B)),((_tmpB2B->break_lab=(struct _dyneither_ptr**)0,((_tmpB2B->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp28A),((_tmpB2B->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)((_tmpB2C=_region_malloc(r,sizeof(*_tmpB2C)),((
_tmpB2C->label=next_l,((_tmpB2C->binders=0,((_tmpB2C->next_case_env=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp),_tmpB2C)))))))),((_tmpB2B->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp28C),((_tmpB2B->toplevel=(int)_tmp28D,((_tmpB2B->rgn=(struct _RegionHandle*)
r,_tmpB2B)))))))))))));};}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){void*_tmp290=e->annot;
_LLF1: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp291=(struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmp290;if(_tmp291->tag != Cyc_CfFlowInfo_UnknownZ)goto _LLF3;}_LLF2: return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLF3: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmp292=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmp290;if(_tmp292->tag != Cyc_CfFlowInfo_NotZero)
goto _LLF5;}_LLF4: return 0;_LLF5: {struct Cyc_CfFlowInfo_IsZero_struct*_tmp293=(
struct Cyc_CfFlowInfo_IsZero_struct*)_tmp290;if(_tmp293->tag != Cyc_CfFlowInfo_IsZero)
goto _LLF7;}_LLF6:{const char*_tmpB2F;void*_tmpB2E;(_tmpB2E=0,Cyc_Tcutil_terr(e->loc,((
_tmpB2F="dereference of NULL pointer",_tag_dyneither(_tmpB2F,sizeof(char),28))),
_tag_dyneither(_tmpB2E,sizeof(void*),0)));}return 0;_LLF7: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp294=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp290;if(_tmp294->tag != Cyc_Absyn_EmptyAnnot)
goto _LLF9;}_LLF8: return 0;_LLF9:;_LLFA: {const char*_tmpB32;void*_tmpB31;(_tmpB31=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB32="need_null_check",_tag_dyneither(_tmpB32,sizeof(char),16))),
_tag_dyneither(_tmpB31,sizeof(void*),0)));}_LLF0:;}static struct Cyc_List_List*Cyc_Toc_get_relns(
struct Cyc_Absyn_Exp*e){void*_tmp299=e->annot;struct Cyc_List_List*_tmp29B;struct
Cyc_List_List*_tmp29D;_LLFC: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp29A=(
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp299;if(_tmp29A->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLFE;else{_tmp29B=_tmp29A->f1;}}_LLFD: return _tmp29B;_LLFE: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmp29C=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmp299;if(_tmp29C->tag != Cyc_CfFlowInfo_NotZero)
goto _LL100;else{_tmp29D=_tmp29C->f1;}}_LLFF: return _tmp29D;_LL100: {struct Cyc_CfFlowInfo_IsZero_struct*
_tmp29E=(struct Cyc_CfFlowInfo_IsZero_struct*)_tmp299;if(_tmp29E->tag != Cyc_CfFlowInfo_IsZero)
goto _LL102;}_LL101:{const char*_tmpB35;void*_tmpB34;(_tmpB34=0,Cyc_Tcutil_terr(e->loc,((
_tmpB35="dereference of NULL pointer",_tag_dyneither(_tmpB35,sizeof(char),28))),
_tag_dyneither(_tmpB34,sizeof(void*),0)));}return 0;_LL102: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp29F=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp299;if(_tmp29F->tag != Cyc_Absyn_EmptyAnnot)
goto _LL104;}_LL103: return 0;_LL104:;_LL105: {const char*_tmpB38;void*_tmpB37;(
_tmpB37=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB38="get_relns",_tag_dyneither(_tmpB38,sizeof(char),10))),_tag_dyneither(
_tmpB37,sizeof(void*),0)));}_LLFB:;}static int Cyc_Toc_check_const_array(
unsigned int i,void*t){void*_tmp2A4=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp2A6;struct Cyc_Absyn_PtrAtts _tmp2A7;union Cyc_Absyn_Constraint*_tmp2A8;union
Cyc_Absyn_Constraint*_tmp2A9;struct Cyc_Absyn_ArrayInfo _tmp2AB;struct Cyc_Absyn_Exp*
_tmp2AC;_LL107: {struct Cyc_Absyn_PointerType_struct*_tmp2A5=(struct Cyc_Absyn_PointerType_struct*)
_tmp2A4;if(_tmp2A5->tag != 5)goto _LL109;else{_tmp2A6=_tmp2A5->f1;_tmp2A7=_tmp2A6.ptr_atts;
_tmp2A8=_tmp2A7.bounds;_tmp2A9=_tmp2A7.zero_term;}}_LL108: {void*_tmp2AD=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp2A8);struct Cyc_Absyn_Exp*_tmp2B0;_LL10E: {struct Cyc_Absyn_DynEither_b_struct*
_tmp2AE=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2AD;if(_tmp2AE->tag != 0)goto
_LL110;}_LL10F: return 0;_LL110: {struct Cyc_Absyn_Upper_b_struct*_tmp2AF=(struct
Cyc_Absyn_Upper_b_struct*)_tmp2AD;if(_tmp2AF->tag != 1)goto _LL10D;else{_tmp2B0=
_tmp2AF->f1;}}_LL111: {unsigned int _tmp2B2;int _tmp2B3;struct _tuple10 _tmp2B1=Cyc_Evexp_eval_const_uint_exp(
_tmp2B0);_tmp2B2=_tmp2B1.f1;_tmp2B3=_tmp2B1.f2;return _tmp2B3  && i <= _tmp2B2;}
_LL10D:;}_LL109: {struct Cyc_Absyn_ArrayType_struct*_tmp2AA=(struct Cyc_Absyn_ArrayType_struct*)
_tmp2A4;if(_tmp2AA->tag != 9)goto _LL10B;else{_tmp2AB=_tmp2AA->f1;_tmp2AC=_tmp2AB.num_elts;}}
_LL10A: if(_tmp2AC == 0)return 0;{unsigned int _tmp2B5;int _tmp2B6;struct _tuple10
_tmp2B4=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp2AC);_tmp2B5=
_tmp2B4.f1;_tmp2B6=_tmp2B4.f2;return _tmp2B6  && i <= _tmp2B5;};_LL10B:;_LL10C:
return 0;_LL106:;}static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2B7=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(
_tmp2B7->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp _tmp2B8=_tmp2B7->rop;struct
Cyc_Absyn_Vardecl*_tmp2B9;struct Cyc_Absyn_Vardecl*_tmp2BA;_LL113: if((_tmp2B8.LessNumelts).tag
!= 3)goto _LL115;_tmp2B9=(struct Cyc_Absyn_Vardecl*)(_tmp2B8.LessNumelts).val;
_LL114: _tmp2BA=_tmp2B9;goto _LL116;_LL115: if((_tmp2B8.LessEqNumelts).tag != 5)goto
_LL117;_tmp2BA=(struct Cyc_Absyn_Vardecl*)(_tmp2B8.LessEqNumelts).val;_LL116: if(
_tmp2BA == v)return 1;else{goto _LL112;}_LL117:;_LL118: continue;_LL112:;};}return 0;}
static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Exp*e,void*vtype){{void*_tmp2BB=e->r;void*_tmp2BD;struct Cyc_Absyn_Vardecl*
_tmp2BF;void*_tmp2C1;struct Cyc_Absyn_Vardecl*_tmp2C3;void*_tmp2C5;struct Cyc_Absyn_Vardecl*
_tmp2C7;void*_tmp2C9;struct Cyc_Absyn_Vardecl*_tmp2CB;union Cyc_Absyn_Cnst _tmp2CD;
struct _tuple5 _tmp2CE;enum Cyc_Absyn_Sign _tmp2CF;int _tmp2D0;union Cyc_Absyn_Cnst
_tmp2D2;struct _tuple5 _tmp2D3;enum Cyc_Absyn_Sign _tmp2D4;int _tmp2D5;union Cyc_Absyn_Cnst
_tmp2D7;struct _tuple5 _tmp2D8;enum Cyc_Absyn_Sign _tmp2D9;int _tmp2DA;enum Cyc_Absyn_Primop
_tmp2DC;struct Cyc_List_List*_tmp2DD;struct Cyc_List_List _tmp2DE;struct Cyc_Absyn_Exp*
_tmp2DF;_LL11A: {struct Cyc_Absyn_Var_e_struct*_tmp2BC=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BB;if(_tmp2BC->tag != 1)goto _LL11C;else{_tmp2BD=(void*)_tmp2BC->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2BE=(struct Cyc_Absyn_Pat_b_struct*)_tmp2BD;if(_tmp2BE->tag
!= 5)goto _LL11C;else{_tmp2BF=_tmp2BE->f1;}};}}_LL11B: _tmp2C3=_tmp2BF;goto _LL11D;
_LL11C: {struct Cyc_Absyn_Var_e_struct*_tmp2C0=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BB;if(_tmp2C0->tag != 1)goto _LL11E;else{_tmp2C1=(void*)_tmp2C0->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2C2=(struct Cyc_Absyn_Local_b_struct*)_tmp2C1;if(
_tmp2C2->tag != 4)goto _LL11E;else{_tmp2C3=_tmp2C2->f1;}};}}_LL11D: _tmp2C7=_tmp2C3;
goto _LL11F;_LL11E: {struct Cyc_Absyn_Var_e_struct*_tmp2C4=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BB;if(_tmp2C4->tag != 1)goto _LL120;else{_tmp2C5=(void*)_tmp2C4->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2C6=(struct Cyc_Absyn_Global_b_struct*)_tmp2C5;if(
_tmp2C6->tag != 1)goto _LL120;else{_tmp2C7=_tmp2C6->f1;}};}}_LL11F: _tmp2CB=_tmp2C7;
goto _LL121;_LL120: {struct Cyc_Absyn_Var_e_struct*_tmp2C8=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BB;if(_tmp2C8->tag != 1)goto _LL122;else{_tmp2C9=(void*)_tmp2C8->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp2CA=(struct Cyc_Absyn_Param_b_struct*)_tmp2C9;if(
_tmp2CA->tag != 3)goto _LL122;else{_tmp2CB=_tmp2CA->f1;}};}}_LL121: if(_tmp2CB->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp2CB))return 1;goto _LL119;_LL122: {
struct Cyc_Absyn_Const_e_struct*_tmp2CC=(struct Cyc_Absyn_Const_e_struct*)_tmp2BB;
if(_tmp2CC->tag != 0)goto _LL124;else{_tmp2CD=_tmp2CC->f1;if((_tmp2CD.Int_c).tag != 
4)goto _LL124;_tmp2CE=(struct _tuple5)(_tmp2CD.Int_c).val;_tmp2CF=_tmp2CE.f1;if(
_tmp2CF != Cyc_Absyn_None)goto _LL124;_tmp2D0=_tmp2CE.f2;}}_LL123: _tmp2D5=_tmp2D0;
goto _LL125;_LL124: {struct Cyc_Absyn_Const_e_struct*_tmp2D1=(struct Cyc_Absyn_Const_e_struct*)
_tmp2BB;if(_tmp2D1->tag != 0)goto _LL126;else{_tmp2D2=_tmp2D1->f1;if((_tmp2D2.Int_c).tag
!= 4)goto _LL126;_tmp2D3=(struct _tuple5)(_tmp2D2.Int_c).val;_tmp2D4=_tmp2D3.f1;
if(_tmp2D4 != Cyc_Absyn_Signed)goto _LL126;_tmp2D5=_tmp2D3.f2;}}_LL125: return
_tmp2D5 >= 0  && Cyc_Toc_check_const_array((unsigned int)_tmp2D5,vtype);_LL126: {
struct Cyc_Absyn_Const_e_struct*_tmp2D6=(struct Cyc_Absyn_Const_e_struct*)_tmp2BB;
if(_tmp2D6->tag != 0)goto _LL128;else{_tmp2D7=_tmp2D6->f1;if((_tmp2D7.Int_c).tag != 
4)goto _LL128;_tmp2D8=(struct _tuple5)(_tmp2D7.Int_c).val;_tmp2D9=_tmp2D8.f1;if(
_tmp2D9 != Cyc_Absyn_Unsigned)goto _LL128;_tmp2DA=_tmp2D8.f2;}}_LL127: return Cyc_Toc_check_const_array((
unsigned int)_tmp2DA,vtype);_LL128: {struct Cyc_Absyn_Primop_e_struct*_tmp2DB=(
struct Cyc_Absyn_Primop_e_struct*)_tmp2BB;if(_tmp2DB->tag != 3)goto _LL12A;else{
_tmp2DC=_tmp2DB->f1;if(_tmp2DC != Cyc_Absyn_Numelts)goto _LL12A;_tmp2DD=_tmp2DB->f2;
if(_tmp2DD == 0)goto _LL12A;_tmp2DE=*_tmp2DD;_tmp2DF=(struct Cyc_Absyn_Exp*)_tmp2DE.hd;}}
_LL129:{void*_tmp2E0=_tmp2DF->r;void*_tmp2E2;struct Cyc_Absyn_Vardecl*_tmp2E4;
void*_tmp2E6;struct Cyc_Absyn_Vardecl*_tmp2E8;void*_tmp2EA;struct Cyc_Absyn_Vardecl*
_tmp2EC;void*_tmp2EE;struct Cyc_Absyn_Vardecl*_tmp2F0;_LL12D: {struct Cyc_Absyn_Var_e_struct*
_tmp2E1=(struct Cyc_Absyn_Var_e_struct*)_tmp2E0;if(_tmp2E1->tag != 1)goto _LL12F;
else{_tmp2E2=(void*)_tmp2E1->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp2E3=(struct Cyc_Absyn_Pat_b_struct*)
_tmp2E2;if(_tmp2E3->tag != 5)goto _LL12F;else{_tmp2E4=_tmp2E3->f1;}};}}_LL12E:
_tmp2E8=_tmp2E4;goto _LL130;_LL12F: {struct Cyc_Absyn_Var_e_struct*_tmp2E5=(struct
Cyc_Absyn_Var_e_struct*)_tmp2E0;if(_tmp2E5->tag != 1)goto _LL131;else{_tmp2E6=(
void*)_tmp2E5->f2;{struct Cyc_Absyn_Local_b_struct*_tmp2E7=(struct Cyc_Absyn_Local_b_struct*)
_tmp2E6;if(_tmp2E7->tag != 4)goto _LL131;else{_tmp2E8=_tmp2E7->f1;}};}}_LL130:
_tmp2EC=_tmp2E8;goto _LL132;_LL131: {struct Cyc_Absyn_Var_e_struct*_tmp2E9=(struct
Cyc_Absyn_Var_e_struct*)_tmp2E0;if(_tmp2E9->tag != 1)goto _LL133;else{_tmp2EA=(
void*)_tmp2E9->f2;{struct Cyc_Absyn_Global_b_struct*_tmp2EB=(struct Cyc_Absyn_Global_b_struct*)
_tmp2EA;if(_tmp2EB->tag != 1)goto _LL133;else{_tmp2EC=_tmp2EB->f1;}};}}_LL132:
_tmp2F0=_tmp2EC;goto _LL134;_LL133: {struct Cyc_Absyn_Var_e_struct*_tmp2ED=(struct
Cyc_Absyn_Var_e_struct*)_tmp2E0;if(_tmp2ED->tag != 1)goto _LL135;else{_tmp2EE=(
void*)_tmp2ED->f2;{struct Cyc_Absyn_Param_b_struct*_tmp2EF=(struct Cyc_Absyn_Param_b_struct*)
_tmp2EE;if(_tmp2EF->tag != 3)goto _LL135;else{_tmp2F0=_tmp2EF->f1;}};}}_LL134:
return _tmp2F0 == v;_LL135:;_LL136: goto _LL12C;_LL12C:;}goto _LL119;_LL12A:;_LL12B:
goto _LL119;_LL119:;}return 0;}static char _tmp302[8]="*bogus*";struct _tuple17{void*
f1;void*f2;};static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,
struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){struct Cyc_Absyn_Vardecl*x;{void*
_tmp2F1=a->r;void*_tmp2F3;struct Cyc_Absyn_Vardecl*_tmp2F5;void*_tmp2F7;struct Cyc_Absyn_Vardecl*
_tmp2F9;void*_tmp2FB;struct Cyc_Absyn_Vardecl*_tmp2FD;void*_tmp2FF;struct Cyc_Absyn_Vardecl*
_tmp301;_LL138: {struct Cyc_Absyn_Var_e_struct*_tmp2F2=(struct Cyc_Absyn_Var_e_struct*)
_tmp2F1;if(_tmp2F2->tag != 1)goto _LL13A;else{_tmp2F3=(void*)_tmp2F2->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2F4=(struct Cyc_Absyn_Pat_b_struct*)_tmp2F3;if(_tmp2F4->tag
!= 5)goto _LL13A;else{_tmp2F5=_tmp2F4->f1;}};}}_LL139: _tmp2F9=_tmp2F5;goto _LL13B;
_LL13A: {struct Cyc_Absyn_Var_e_struct*_tmp2F6=(struct Cyc_Absyn_Var_e_struct*)
_tmp2F1;if(_tmp2F6->tag != 1)goto _LL13C;else{_tmp2F7=(void*)_tmp2F6->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2F8=(struct Cyc_Absyn_Local_b_struct*)_tmp2F7;if(
_tmp2F8->tag != 4)goto _LL13C;else{_tmp2F9=_tmp2F8->f1;}};}}_LL13B: _tmp2FD=_tmp2F9;
goto _LL13D;_LL13C: {struct Cyc_Absyn_Var_e_struct*_tmp2FA=(struct Cyc_Absyn_Var_e_struct*)
_tmp2F1;if(_tmp2FA->tag != 1)goto _LL13E;else{_tmp2FB=(void*)_tmp2FA->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2FC=(struct Cyc_Absyn_Global_b_struct*)_tmp2FB;if(
_tmp2FC->tag != 1)goto _LL13E;else{_tmp2FD=_tmp2FC->f1;}};}}_LL13D: _tmp301=_tmp2FD;
goto _LL13F;_LL13E: {struct Cyc_Absyn_Var_e_struct*_tmp2FE=(struct Cyc_Absyn_Var_e_struct*)
_tmp2F1;if(_tmp2FE->tag != 1)goto _LL140;else{_tmp2FF=(void*)_tmp2FE->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp300=(struct Cyc_Absyn_Param_b_struct*)_tmp2FF;if(
_tmp300->tag != 3)goto _LL140;else{_tmp301=_tmp300->f1;}};}}_LL13F: if(_tmp301->escapes)
goto _LL141;x=_tmp301;goto _LL137;_LL140:;_LL141: {static struct _dyneither_ptr
bogus_string={_tmp302,_tmp302,_tmp302 + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
4,0}},& bogus_string};static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,&
bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};x=& bogus_vardecl;
x->type=a_typ;}_LL137:;}{void*_tmp303=a_typ;inner_loop: {void*_tmp304=i->r;void*
_tmp306;struct Cyc_Absyn_Exp*_tmp307;union Cyc_Absyn_Cnst _tmp309;struct _tuple5
_tmp30A;enum Cyc_Absyn_Sign _tmp30B;int _tmp30C;union Cyc_Absyn_Cnst _tmp30E;struct
_tuple5 _tmp30F;enum Cyc_Absyn_Sign _tmp310;int _tmp311;union Cyc_Absyn_Cnst _tmp313;
struct _tuple5 _tmp314;enum Cyc_Absyn_Sign _tmp315;int _tmp316;enum Cyc_Absyn_Primop
_tmp318;struct Cyc_List_List*_tmp319;struct Cyc_List_List _tmp31A;struct Cyc_Absyn_Exp*
_tmp31B;struct Cyc_List_List*_tmp31C;struct Cyc_List_List _tmp31D;struct Cyc_Absyn_Exp*
_tmp31E;void*_tmp320;struct Cyc_Absyn_Vardecl*_tmp322;void*_tmp324;struct Cyc_Absyn_Vardecl*
_tmp326;void*_tmp328;struct Cyc_Absyn_Vardecl*_tmp32A;void*_tmp32C;struct Cyc_Absyn_Vardecl*
_tmp32E;_LL143: {struct Cyc_Absyn_Cast_e_struct*_tmp305=(struct Cyc_Absyn_Cast_e_struct*)
_tmp304;if(_tmp305->tag != 15)goto _LL145;else{_tmp306=(void*)_tmp305->f1;_tmp307=
_tmp305->f2;}}_LL144: i=_tmp307;goto inner_loop;_LL145: {struct Cyc_Absyn_Const_e_struct*
_tmp308=(struct Cyc_Absyn_Const_e_struct*)_tmp304;if(_tmp308->tag != 0)goto _LL147;
else{_tmp309=_tmp308->f1;if((_tmp309.Int_c).tag != 4)goto _LL147;_tmp30A=(struct
_tuple5)(_tmp309.Int_c).val;_tmp30B=_tmp30A.f1;if(_tmp30B != Cyc_Absyn_None)goto
_LL147;_tmp30C=_tmp30A.f2;}}_LL146: _tmp311=_tmp30C;goto _LL148;_LL147: {struct Cyc_Absyn_Const_e_struct*
_tmp30D=(struct Cyc_Absyn_Const_e_struct*)_tmp304;if(_tmp30D->tag != 0)goto _LL149;
else{_tmp30E=_tmp30D->f1;if((_tmp30E.Int_c).tag != 4)goto _LL149;_tmp30F=(struct
_tuple5)(_tmp30E.Int_c).val;_tmp310=_tmp30F.f1;if(_tmp310 != Cyc_Absyn_Signed)
goto _LL149;_tmp311=_tmp30F.f2;}}_LL148: return _tmp311 >= 0  && Cyc_Toc_check_const_array((
unsigned int)(_tmp311 + 1),_tmp303);_LL149: {struct Cyc_Absyn_Const_e_struct*
_tmp312=(struct Cyc_Absyn_Const_e_struct*)_tmp304;if(_tmp312->tag != 0)goto _LL14B;
else{_tmp313=_tmp312->f1;if((_tmp313.Int_c).tag != 4)goto _LL14B;_tmp314=(struct
_tuple5)(_tmp313.Int_c).val;_tmp315=_tmp314.f1;if(_tmp315 != Cyc_Absyn_Unsigned)
goto _LL14B;_tmp316=_tmp314.f2;}}_LL14A: return Cyc_Toc_check_const_array((
unsigned int)(_tmp316 + 1),_tmp303);_LL14B: {struct Cyc_Absyn_Primop_e_struct*
_tmp317=(struct Cyc_Absyn_Primop_e_struct*)_tmp304;if(_tmp317->tag != 3)goto _LL14D;
else{_tmp318=_tmp317->f1;if(_tmp318 != Cyc_Absyn_Mod)goto _LL14D;_tmp319=_tmp317->f2;
if(_tmp319 == 0)goto _LL14D;_tmp31A=*_tmp319;_tmp31B=(struct Cyc_Absyn_Exp*)_tmp31A.hd;
_tmp31C=_tmp31A.tl;if(_tmp31C == 0)goto _LL14D;_tmp31D=*_tmp31C;_tmp31E=(struct Cyc_Absyn_Exp*)
_tmp31D.hd;}}_LL14C: return Cyc_Toc_check_leq_size(relns,x,_tmp31E,_tmp303);_LL14D: {
struct Cyc_Absyn_Var_e_struct*_tmp31F=(struct Cyc_Absyn_Var_e_struct*)_tmp304;if(
_tmp31F->tag != 1)goto _LL14F;else{_tmp320=(void*)_tmp31F->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp321=(struct Cyc_Absyn_Pat_b_struct*)_tmp320;if(_tmp321->tag != 5)goto _LL14F;
else{_tmp322=_tmp321->f1;}};}}_LL14E: _tmp326=_tmp322;goto _LL150;_LL14F: {struct
Cyc_Absyn_Var_e_struct*_tmp323=(struct Cyc_Absyn_Var_e_struct*)_tmp304;if(_tmp323->tag
!= 1)goto _LL151;else{_tmp324=(void*)_tmp323->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp325=(struct Cyc_Absyn_Local_b_struct*)_tmp324;if(_tmp325->tag != 4)goto _LL151;
else{_tmp326=_tmp325->f1;}};}}_LL150: _tmp32A=_tmp326;goto _LL152;_LL151: {struct
Cyc_Absyn_Var_e_struct*_tmp327=(struct Cyc_Absyn_Var_e_struct*)_tmp304;if(_tmp327->tag
!= 1)goto _LL153;else{_tmp328=(void*)_tmp327->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp329=(struct Cyc_Absyn_Global_b_struct*)_tmp328;if(_tmp329->tag != 1)goto _LL153;
else{_tmp32A=_tmp329->f1;}};}}_LL152: _tmp32E=_tmp32A;goto _LL154;_LL153: {struct
Cyc_Absyn_Var_e_struct*_tmp32B=(struct Cyc_Absyn_Var_e_struct*)_tmp304;if(_tmp32B->tag
!= 1)goto _LL155;else{_tmp32C=(void*)_tmp32B->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp32D=(struct Cyc_Absyn_Param_b_struct*)_tmp32C;if(_tmp32D->tag != 3)goto _LL155;
else{_tmp32E=_tmp32D->f1;}};}}_LL154: if(_tmp32E->escapes)return 0;{struct Cyc_List_List*
_tmp32F=relns;for(0;_tmp32F != 0;_tmp32F=_tmp32F->tl){struct Cyc_CfFlowInfo_Reln*
_tmp330=(struct Cyc_CfFlowInfo_Reln*)_tmp32F->hd;if(_tmp330->vd == _tmp32E){union
Cyc_CfFlowInfo_RelnOp _tmp331=_tmp330->rop;struct Cyc_Absyn_Vardecl*_tmp332;struct
_tuple11 _tmp333;struct Cyc_Absyn_Vardecl*_tmp334;void*_tmp335;unsigned int _tmp336;
unsigned int _tmp337;_LL158: if((_tmp331.LessNumelts).tag != 3)goto _LL15A;_tmp332=(
struct Cyc_Absyn_Vardecl*)(_tmp331.LessNumelts).val;_LL159: if(x == _tmp332)return 1;
else{goto _LL157;}_LL15A: if((_tmp331.LessVar).tag != 2)goto _LL15C;_tmp333=(struct
_tuple11)(_tmp331.LessVar).val;_tmp334=_tmp333.f1;_tmp335=_tmp333.f2;_LL15B:{
struct _tuple17 _tmpB39;struct _tuple17 _tmp339=(_tmpB39.f1=Cyc_Tcutil_compress(
_tmp335),((_tmpB39.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(a->topt))->v),_tmpB39)));void*_tmp33A;void*_tmp33C;void*_tmp33D;
struct Cyc_Absyn_PtrInfo _tmp33F;struct Cyc_Absyn_PtrAtts _tmp340;union Cyc_Absyn_Constraint*
_tmp341;_LL163: _tmp33A=_tmp339.f1;{struct Cyc_Absyn_TagType_struct*_tmp33B=(
struct Cyc_Absyn_TagType_struct*)_tmp33A;if(_tmp33B->tag != 20)goto _LL165;else{
_tmp33C=(void*)_tmp33B->f1;}};_tmp33D=_tmp339.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp33E=(struct Cyc_Absyn_PointerType_struct*)_tmp33D;if(_tmp33E->tag != 5)goto
_LL165;else{_tmp33F=_tmp33E->f1;_tmp340=_tmp33F.ptr_atts;_tmp341=_tmp340.bounds;}};
_LL164:{void*_tmp342=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp341);struct Cyc_Absyn_Exp*_tmp344;_LL168: {struct Cyc_Absyn_Upper_b_struct*
_tmp343=(struct Cyc_Absyn_Upper_b_struct*)_tmp342;if(_tmp343->tag != 1)goto _LL16A;
else{_tmp344=_tmp343->f1;}}_LL169: {struct Cyc_Absyn_Exp*_tmp345=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp33C,0),0,Cyc_Absyn_No_coercion,0);
if(Cyc_Evexp_lte_const_exp(_tmp345,_tmp344))return 1;goto _LL167;}_LL16A:;_LL16B:
goto _LL167;_LL167:;}goto _LL162;_LL165:;_LL166: goto _LL162;_LL162:;}{struct Cyc_List_List*
_tmp346=relns;for(0;_tmp346 != 0;_tmp346=_tmp346->tl){struct Cyc_CfFlowInfo_Reln*
_tmp347=(struct Cyc_CfFlowInfo_Reln*)_tmp346->hd;if(_tmp347->vd == _tmp334){union
Cyc_CfFlowInfo_RelnOp _tmp348=_tmp347->rop;struct Cyc_Absyn_Vardecl*_tmp349;struct
Cyc_Absyn_Vardecl*_tmp34A;unsigned int _tmp34B;unsigned int _tmp34C;struct _tuple11
_tmp34D;struct Cyc_Absyn_Vardecl*_tmp34E;_LL16D: if((_tmp348.LessEqNumelts).tag != 
5)goto _LL16F;_tmp349=(struct Cyc_Absyn_Vardecl*)(_tmp348.LessEqNumelts).val;
_LL16E: _tmp34A=_tmp349;goto _LL170;_LL16F: if((_tmp348.LessNumelts).tag != 3)goto
_LL171;_tmp34A=(struct Cyc_Absyn_Vardecl*)(_tmp348.LessNumelts).val;_LL170: if(x == 
_tmp34A)return 1;goto _LL16C;_LL171: if((_tmp348.EqualConst).tag != 1)goto _LL173;
_tmp34B=(unsigned int)(_tmp348.EqualConst).val;_LL172: return Cyc_Toc_check_const_array(
_tmp34B,_tmp303);_LL173: if((_tmp348.LessEqConst).tag != 6)goto _LL175;_tmp34C=(
unsigned int)(_tmp348.LessEqConst).val;if(!(_tmp34C > 0))goto _LL175;_LL174: return
Cyc_Toc_check_const_array(_tmp34C,_tmp303);_LL175: if((_tmp348.LessVar).tag != 2)
goto _LL177;_tmp34D=(struct _tuple11)(_tmp348.LessVar).val;_tmp34E=_tmp34D.f1;
_LL176: if(Cyc_Toc_check_leq_size_var(relns,x,_tmp34E))return 1;goto _LL16C;_LL177:;
_LL178: goto _LL16C;_LL16C:;}}}goto _LL157;_LL15C: if((_tmp331.LessConst).tag != 4)
goto _LL15E;_tmp336=(unsigned int)(_tmp331.LessConst).val;_LL15D: return Cyc_Toc_check_const_array(
_tmp336,_tmp303);_LL15E: if((_tmp331.LessEqConst).tag != 6)goto _LL160;_tmp337=(
unsigned int)(_tmp331.LessEqConst).val;_LL15F: return Cyc_Toc_check_const_array(
_tmp337 + 1,_tmp303);_LL160:;_LL161: goto _LL157;_LL157:;}}}goto _LL142;_LL155:;
_LL156: goto _LL142;_LL142:;}return 0;};}static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*
e){if(e->topt == 0){const char*_tmpB3C;void*_tmpB3B;(_tmpB3B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3C="Missing type in primop ",
_tag_dyneither(_tmpB3C,sizeof(char),24))),_tag_dyneither(_tmpB3B,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*
_tmpB3F;void*_tmpB3E;(_tmpB3E=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3F="Missing type in primop ",
_tag_dyneither(_tmpB3F,sizeof(char),24))),_tag_dyneither(_tmpB3E,sizeof(void*),0)));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple9*
Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){struct _tuple9*_tmpB40;return(_tmpB40=
_cycalloc(sizeof(*_tmpB40)),((_tmpB40->f1=Cyc_Toc_mt_tq,((_tmpB40->f2=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmpB40)))));}static struct
_tuple15*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple15*_tmpB41;return(_tmpB41=_cycalloc(sizeof(*_tmpB41)),((
_tmpB41->f1=0,((_tmpB41->f2=e,_tmpB41)))));};}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(
struct Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int
pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;
if(pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp355=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0  || Cyc_Absyn_no_regions)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(
_tmp355): Cyc_Toc_malloc_ptr(_tmp355));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp355);}};}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp356=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp356 != 0;_tmp356=_tmp356->tl){struct _tuple15 _tmp358;struct Cyc_List_List*
_tmp359;struct Cyc_Absyn_Exp*_tmp35A;struct _tuple15*_tmp357=(struct _tuple15*)
_tmp356->hd;_tmp358=*_tmp357;_tmp359=_tmp358.f1;_tmp35A=_tmp358.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp359 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp359->tl != 0){const char*_tmpB44;void*_tmpB43;(_tmpB43=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB44="multiple designators in array",
_tag_dyneither(_tmpB44,sizeof(char),30))),_tag_dyneither(_tmpB43,sizeof(void*),0)));}{
void*_tmp35D=(void*)_tmp359->hd;void*_tmp35E=_tmp35D;struct Cyc_Absyn_Exp*_tmp360;
_LL17A: {struct Cyc_Absyn_ArrayElement_struct*_tmp35F=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp35E;if(_tmp35F->tag != 0)goto _LL17C;else{_tmp360=_tmp35F->f1;}}_LL17B: Cyc_Toc_exp_to_c(
nv,_tmp360);e_index=_tmp360;goto _LL179;_LL17C: {struct Cyc_Absyn_FieldName_struct*
_tmp361=(struct Cyc_Absyn_FieldName_struct*)_tmp35E;if(_tmp361->tag != 1)goto
_LL179;}_LL17D: {const char*_tmpB47;void*_tmpB46;(_tmpB46=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB47="field name designators in array",
_tag_dyneither(_tmpB47,sizeof(char),32))),_tag_dyneither(_tmpB46,sizeof(void*),0)));}
_LL179:;};}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*
_tmp364=_tmp35A->r;struct Cyc_List_List*_tmp366;struct Cyc_Absyn_Vardecl*_tmp368;
struct Cyc_Absyn_Exp*_tmp369;struct Cyc_Absyn_Exp*_tmp36A;int _tmp36B;void*_tmp36D;
struct Cyc_List_List*_tmp36E;_LL17F: {struct Cyc_Absyn_Array_e_struct*_tmp365=(
struct Cyc_Absyn_Array_e_struct*)_tmp364;if(_tmp365->tag != 27)goto _LL181;else{
_tmp366=_tmp365->f1;}}_LL180: s=Cyc_Toc_init_array(nv,lval,_tmp366,s);goto _LL17E;
_LL181: {struct Cyc_Absyn_Comprehension_e_struct*_tmp367=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp364;if(_tmp367->tag != 28)goto _LL183;else{_tmp368=_tmp367->f1;_tmp369=_tmp367->f2;
_tmp36A=_tmp367->f3;_tmp36B=_tmp367->f4;}}_LL182: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp368,_tmp369,_tmp36A,_tmp36B,s,0);goto _LL17E;_LL183: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp36C=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp364;if(_tmp36C->tag != 30)goto
_LL185;else{_tmp36D=(void*)_tmp36C->f1;_tmp36E=_tmp36C->f2;}}_LL184: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp36D,_tmp36E,s);goto _LL17E;_LL185:;_LL186: Cyc_Toc_exp_to_c(nv,_tmp35A);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
_tmp35A,0),s,0);goto _LL17E;_LL17E:;};};}}return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple0*_tmp36F=vd->name;void*_tmp370=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env _tmp372;struct _RegionHandle*_tmp373;
struct Cyc_Toc_Env*_tmp371=nv;_tmp372=*_tmp371;_tmp373=_tmp372.rgn;{struct Cyc_Absyn_Local_b_struct
_tmpB4A;struct Cyc_Absyn_Local_b_struct*_tmpB49;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
_tmp373,nv,_tmp36F,Cyc_Absyn_varb_exp(_tmp36F,(void*)((_tmpB49=_cycalloc(sizeof(*
_tmpB49)),((_tmpB49[0]=((_tmpB4A.tag=4,((_tmpB4A.f1=vd,_tmpB4A)))),_tmpB49)))),0));
struct _tuple0*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp36F,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp36F,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp36F,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp36F,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp374=e2->r;struct Cyc_List_List*_tmp376;
struct Cyc_Absyn_Vardecl*_tmp378;struct Cyc_Absyn_Exp*_tmp379;struct Cyc_Absyn_Exp*
_tmp37A;int _tmp37B;void*_tmp37D;struct Cyc_List_List*_tmp37E;_LL188: {struct Cyc_Absyn_Array_e_struct*
_tmp375=(struct Cyc_Absyn_Array_e_struct*)_tmp374;if(_tmp375->tag != 27)goto _LL18A;
else{_tmp376=_tmp375->f1;}}_LL189: body=Cyc_Toc_init_array(nv2,lval,_tmp376,Cyc_Toc_skip_stmt_dl());
goto _LL187;_LL18A: {struct Cyc_Absyn_Comprehension_e_struct*_tmp377=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp374;if(_tmp377->tag != 28)goto _LL18C;else{_tmp378=_tmp377->f1;_tmp379=_tmp377->f2;
_tmp37A=_tmp377->f3;_tmp37B=_tmp377->f4;}}_LL18B: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp378,_tmp379,_tmp37A,_tmp37B,Cyc_Toc_skip_stmt_dl(),0);goto _LL187;
_LL18C: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp37C=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp374;if(_tmp37C->tag != 30)goto _LL18E;else{_tmp37D=(void*)_tmp37C->f1;_tmp37E=
_tmp37C->f2;}}_LL18D: body=Cyc_Toc_init_anon_struct(nv,lval,_tmp37D,_tmp37E,Cyc_Toc_skip_stmt_dl());
goto _LL187;_LL18E:;_LL18F: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL187;_LL187:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp370,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp36F,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp381=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp381 != 0;_tmp381=_tmp381->tl){struct _tuple15 _tmp383;
struct Cyc_List_List*_tmp384;struct Cyc_Absyn_Exp*_tmp385;struct _tuple15*_tmp382=(
struct _tuple15*)_tmp381->hd;_tmp383=*_tmp382;_tmp384=_tmp383.f1;_tmp385=_tmp383.f2;
if(_tmp384 == 0){const char*_tmpB4D;void*_tmpB4C;(_tmpB4C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4D="empty designator list",
_tag_dyneither(_tmpB4D,sizeof(char),22))),_tag_dyneither(_tmpB4C,sizeof(void*),0)));}
if(_tmp384->tl != 0){const char*_tmpB50;void*_tmpB4F;(_tmpB4F=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB50="too many designators in anonymous struct",
_tag_dyneither(_tmpB50,sizeof(char),41))),_tag_dyneither(_tmpB4F,sizeof(void*),0)));}{
void*_tmp38A=(void*)_tmp384->hd;struct _dyneither_ptr*_tmp38C;_LL191: {struct Cyc_Absyn_FieldName_struct*
_tmp38B=(struct Cyc_Absyn_FieldName_struct*)_tmp38A;if(_tmp38B->tag != 1)goto
_LL193;else{_tmp38C=_tmp38B->f1;}}_LL192: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(
lhs,_tmp38C,0);{void*_tmp38D=_tmp385->r;struct Cyc_List_List*_tmp38F;struct Cyc_Absyn_Vardecl*
_tmp391;struct Cyc_Absyn_Exp*_tmp392;struct Cyc_Absyn_Exp*_tmp393;int _tmp394;void*
_tmp396;struct Cyc_List_List*_tmp397;_LL196: {struct Cyc_Absyn_Array_e_struct*
_tmp38E=(struct Cyc_Absyn_Array_e_struct*)_tmp38D;if(_tmp38E->tag != 27)goto _LL198;
else{_tmp38F=_tmp38E->f1;}}_LL197: s=Cyc_Toc_init_array(nv,lval,_tmp38F,s);goto
_LL195;_LL198: {struct Cyc_Absyn_Comprehension_e_struct*_tmp390=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp38D;if(_tmp390->tag != 28)goto _LL19A;else{_tmp391=_tmp390->f1;_tmp392=_tmp390->f2;
_tmp393=_tmp390->f3;_tmp394=_tmp390->f4;}}_LL199: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp391,_tmp392,_tmp393,_tmp394,s,0);goto _LL195;_LL19A: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp395=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp38D;if(_tmp395->tag != 30)goto
_LL19C;else{_tmp396=(void*)_tmp395->f1;_tmp397=_tmp395->f2;}}_LL19B: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp396,_tmp397,s);goto _LL195;_LL19C:;_LL19D: Cyc_Toc_exp_to_c(nv,_tmp385);
if(Cyc_Toc_is_poly_field(struct_type,_tmp38C))_tmp385=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp385);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp385,0),0),s,0);goto _LL195;_LL195:;}goto _LL190;}_LL193:;_LL194: {const char*
_tmpB53;void*_tmpB52;(_tmpB52=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB53="array designator in struct",
_tag_dyneither(_tmpB53,sizeof(char),27))),_tag_dyneither(_tmpB52,sizeof(void*),0)));}
_LL190:;};}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct Cyc_Toc_Env
_tmp39B;struct _RegionHandle*_tmp39C;struct Cyc_Toc_Env*_tmp39A=nv;_tmp39B=*
_tmp39A;_tmp39C=_tmp39B.rgn;{struct Cyc_List_List*_tmp39D=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_rmap)(_tmp39C,Cyc_Toc_tup_to_c,es);void*_tmp39E=Cyc_Toc_add_tuple_type(
_tmp39D);struct _tuple0*_tmp39F=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp3A0=
Cyc_Absyn_var_exp(_tmp39F,0);struct Cyc_Absyn_Stmt*_tmp3A1=Cyc_Absyn_exp_stmt(
_tmp3A0,0);struct Cyc_Absyn_Exp*(*_tmp3A2)(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp3A3=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp39C,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp3A3);for(0;_tmp3A3 != 0;(_tmp3A3=
_tmp3A3->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp3A3->hd;struct
Cyc_Absyn_Exp*lval=_tmp3A2(_tmp3A0,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp3A4=e->r;struct Cyc_List_List*_tmp3A6;struct Cyc_Absyn_Vardecl*_tmp3A8;
struct Cyc_Absyn_Exp*_tmp3A9;struct Cyc_Absyn_Exp*_tmp3AA;int _tmp3AB;_LL19F: {
struct Cyc_Absyn_Array_e_struct*_tmp3A5=(struct Cyc_Absyn_Array_e_struct*)_tmp3A4;
if(_tmp3A5->tag != 27)goto _LL1A1;else{_tmp3A6=_tmp3A5->f1;}}_LL1A0: _tmp3A1=Cyc_Toc_init_array(
nv,lval,_tmp3A6,_tmp3A1);goto _LL19E;_LL1A1: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp3A7=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp3A4;if(_tmp3A7->tag != 28)
goto _LL1A3;else{_tmp3A8=_tmp3A7->f1;_tmp3A9=_tmp3A7->f2;_tmp3AA=_tmp3A7->f3;
_tmp3AB=_tmp3A7->f4;}}_LL1A2: _tmp3A1=Cyc_Toc_init_comprehension(nv,lval,_tmp3A8,
_tmp3A9,_tmp3AA,_tmp3AB,_tmp3A1,0);goto _LL19E;_LL1A3:;_LL1A4: Cyc_Toc_exp_to_c(nv,
e);_tmp3A1=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3A2(
_tmp3A0,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp3A1,0);goto _LL19E;_LL19E:;};}}
return Cyc_Toc_make_struct(nv,_tmp39F,_tmp39E,_tmp3A1,pointer,rgnopt,is_atomic);};}
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct
_dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp3AC=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp3AC != 0;_tmp3AC=_tmp3AC->tl){struct Cyc_Absyn_Aggrfield*
_tmp3AD=(struct Cyc_Absyn_Aggrfield*)_tmp3AC->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp3AD->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}{
struct Cyc_String_pa_struct _tmpB5B;void*_tmpB5A[1];const char*_tmpB59;void*_tmpB58;(
_tmpB58=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)((_tmpB5B.tag=0,((_tmpB5B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmpB5A[0]=& _tmpB5B,Cyc_aprintf(((_tmpB59="get_member_offset %s failed",
_tag_dyneither(_tmpB59,sizeof(char),28))),_tag_dyneither(_tmpB5A,sizeof(void*),1)))))))),
_tag_dyneither(_tmpB58,sizeof(void*),0)));};}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){struct _tuple0*_tmp3B2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3B3=Cyc_Absyn_var_exp(_tmp3B2,0);struct Cyc_Absyn_Stmt*
_tmp3B4=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp3B3),0);struct Cyc_Absyn_Exp*(*
_tmp3B5)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*)=
pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;void*_tmp3B6=Cyc_Toc_aggrdecl_type(
tdn,Cyc_Absyn_strctq);int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp3B7=
Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp3B9;union Cyc_Absyn_AggrInfoU
_tmp3BA;_LL1A6: {struct Cyc_Absyn_AggrType_struct*_tmp3B8=(struct Cyc_Absyn_AggrType_struct*)
_tmp3B7;if(_tmp3B8->tag != 12)goto _LL1A8;else{_tmp3B9=_tmp3B8->f1;_tmp3BA=_tmp3B9.aggr_info;}}
_LL1A7: ad=Cyc_Absyn_get_known_aggrdecl(_tmp3BA);goto _LL1A5;_LL1A8:;_LL1A9: {
const char*_tmpB5E;void*_tmpB5D;(_tmpB5D=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB5E="init_struct: bad struct type",
_tag_dyneither(_tmpB5E,sizeof(char),29))),_tag_dyneither(_tmpB5D,sizeof(void*),0)));}
_LL1A5:;}{int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct Cyc_Toc_Env _tmp3BE;struct _RegionHandle*_tmp3BF;struct Cyc_Toc_Env*_tmp3BD=
nv;_tmp3BE=*_tmp3BD;_tmp3BF=_tmp3BE.rgn;{struct Cyc_List_List*_tmp3C0=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp3BF,dles);for(0;
_tmp3C0 != 0;_tmp3C0=_tmp3C0->tl){struct _tuple15 _tmp3C2;struct Cyc_List_List*
_tmp3C3;struct Cyc_Absyn_Exp*_tmp3C4;struct _tuple15*_tmp3C1=(struct _tuple15*)
_tmp3C0->hd;_tmp3C2=*_tmp3C1;_tmp3C3=_tmp3C2.f1;_tmp3C4=_tmp3C2.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3C4->topt))->v);
if(_tmp3C3 == 0){const char*_tmpB61;void*_tmpB60;(_tmpB60=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB61="empty designator list",
_tag_dyneither(_tmpB61,sizeof(char),22))),_tag_dyneither(_tmpB60,sizeof(void*),0)));}
if(_tmp3C3->tl != 0){struct _tuple0*_tmp3C7=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp3C8=Cyc_Absyn_var_exp(_tmp3C7,0);for(0;_tmp3C3 != 0;_tmp3C3=_tmp3C3->tl){void*
_tmp3C9=(void*)_tmp3C3->hd;struct _dyneither_ptr*_tmp3CB;_LL1AB: {struct Cyc_Absyn_FieldName_struct*
_tmp3CA=(struct Cyc_Absyn_FieldName_struct*)_tmp3C9;if(_tmp3CA->tag != 1)goto
_LL1AD;else{_tmp3CB=_tmp3CA->f1;}}_LL1AC: if(Cyc_Toc_is_poly_field(struct_type,
_tmp3CB))_tmp3C8=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3C8);_tmp3B4=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3B5(_tmp3B3,_tmp3CB,0),_tmp3C8,0),0),
_tmp3B4,0);goto _LL1AA;_LL1AD:;_LL1AE: {const char*_tmpB64;void*_tmpB63;(_tmpB63=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB64="array designator in struct",_tag_dyneither(_tmpB64,sizeof(char),27))),
_tag_dyneither(_tmpB63,sizeof(void*),0)));}_LL1AA:;}Cyc_Toc_exp_to_c(nv,_tmp3C4);
_tmp3B4=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3C8,
_tmp3C4,0),0),_tmp3B4,0);}else{void*_tmp3CE=(void*)_tmp3C3->hd;struct
_dyneither_ptr*_tmp3D0;_LL1B0: {struct Cyc_Absyn_FieldName_struct*_tmp3CF=(struct
Cyc_Absyn_FieldName_struct*)_tmp3CE;if(_tmp3CF->tag != 1)goto _LL1B2;else{_tmp3D0=
_tmp3CF->f1;}}_LL1B1: {struct Cyc_Absyn_Exp*lval=_tmp3B5(_tmp3B3,_tmp3D0,0);if(
is_tagged_union){int i=Cyc_Toc_get_member_offset(ad,_tmp3D0);struct Cyc_Absyn_Exp*
f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_aggrmember_exp(
lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,
f_tag_exp,0);_tmp3B4=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3B4,0);
lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{void*_tmp3D1=_tmp3C4->r;
struct Cyc_List_List*_tmp3D3;struct Cyc_Absyn_Vardecl*_tmp3D5;struct Cyc_Absyn_Exp*
_tmp3D6;struct Cyc_Absyn_Exp*_tmp3D7;int _tmp3D8;void*_tmp3DA;struct Cyc_List_List*
_tmp3DB;_LL1B5: {struct Cyc_Absyn_Array_e_struct*_tmp3D2=(struct Cyc_Absyn_Array_e_struct*)
_tmp3D1;if(_tmp3D2->tag != 27)goto _LL1B7;else{_tmp3D3=_tmp3D2->f1;}}_LL1B6:
_tmp3B4=Cyc_Toc_init_array(nv,lval,_tmp3D3,_tmp3B4);goto _LL1B4;_LL1B7: {struct
Cyc_Absyn_Comprehension_e_struct*_tmp3D4=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3D1;if(_tmp3D4->tag != 28)goto _LL1B9;else{_tmp3D5=_tmp3D4->f1;_tmp3D6=_tmp3D4->f2;
_tmp3D7=_tmp3D4->f3;_tmp3D8=_tmp3D4->f4;}}_LL1B8: _tmp3B4=Cyc_Toc_init_comprehension(
nv,lval,_tmp3D5,_tmp3D6,_tmp3D7,_tmp3D8,_tmp3B4,0);goto _LL1B4;_LL1B9: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3D9=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3D1;if(_tmp3D9->tag != 30)goto
_LL1BB;else{_tmp3DA=(void*)_tmp3D9->f1;_tmp3DB=_tmp3D9->f2;}}_LL1BA: _tmp3B4=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3DA,_tmp3DB,_tmp3B4);goto _LL1B4;_LL1BB:;_LL1BC: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3C4->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp3C4);{struct Cyc_Absyn_Aggrfield*_tmp3DC=Cyc_Absyn_lookup_decl_field(ad,
_tmp3D0);if(Cyc_Toc_is_poly_field(struct_type,_tmp3D0) && !was_ptr_type)_tmp3C4=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3C4);if(has_exists)_tmp3C4=Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3DC))->type),
_tmp3C4);_tmp3B4=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp3C4,0),0),_tmp3B4,0);goto _LL1B4;};}_LL1B4:;}goto _LL1AF;}_LL1B2:;_LL1B3: {
const char*_tmpB67;void*_tmpB66;(_tmpB66=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB67="array designator in struct",
_tag_dyneither(_tmpB67,sizeof(char),27))),_tag_dyneither(_tmpB66,sizeof(void*),0)));}
_LL1AF:;}}}return Cyc_Toc_make_struct(nv,_tmp3B2,_tmp3B6,_tmp3B4,pointer,rgnopt,
is_atomic);};}struct _tuple18{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct
_tuple18*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(
e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,
enum Cyc_Absyn_Incrementor incr){struct Cyc_Absyn_Increment_e_struct _tmpB6A;struct
Cyc_Absyn_Increment_e_struct*_tmpB69;return Cyc_Absyn_new_exp((void*)((_tmpB69=
_cycalloc(sizeof(*_tmpB69)),((_tmpB69[0]=((_tmpB6A.tag=5,((_tmpB6A.f1=e1,((
_tmpB6A.f2=incr,_tmpB6A)))))),_tmpB69)))),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp3E1=e1->r;struct Cyc_Absyn_Stmt*_tmp3E3;void*_tmp3E5;struct Cyc_Absyn_Exp*
_tmp3E6;struct Cyc_Absyn_Exp*_tmp3E8;struct _dyneither_ptr*_tmp3E9;int _tmp3EA;int
_tmp3EB;_LL1BE: {struct Cyc_Absyn_StmtExp_e_struct*_tmp3E2=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp3E1;if(_tmp3E2->tag != 37)goto _LL1C0;else{_tmp3E3=_tmp3E2->f1;}}_LL1BF: Cyc_Toc_lvalue_assign_stmt(
_tmp3E3,fs,f,f_env);goto _LL1BD;_LL1C0: {struct Cyc_Absyn_Cast_e_struct*_tmp3E4=(
struct Cyc_Absyn_Cast_e_struct*)_tmp3E1;if(_tmp3E4->tag != 15)goto _LL1C2;else{
_tmp3E5=(void*)_tmp3E4->f1;_tmp3E6=_tmp3E4->f2;}}_LL1C1: Cyc_Toc_lvalue_assign(
_tmp3E6,fs,f,f_env);goto _LL1BD;_LL1C2: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp3E7=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp3E1;if(_tmp3E7->tag != 22)goto
_LL1C4;else{_tmp3E8=_tmp3E7->f1;_tmp3E9=_tmp3E7->f2;_tmp3EA=_tmp3E7->f3;_tmp3EB=
_tmp3E7->f4;}}_LL1C3: e1->r=_tmp3E8->r;{struct Cyc_List_List*_tmpB6B;Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)((_tmpB6B=_cycalloc(sizeof(*_tmpB6B)),((_tmpB6B->hd=
_tmp3E9,((_tmpB6B->tl=fs,_tmpB6B)))))),f,f_env);}goto _LL1BD;_LL1C4:;_LL1C5: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;goto _LL1BD;}_LL1BD:;}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env){void*_tmp3ED=s->r;struct Cyc_Absyn_Exp*_tmp3EF;struct Cyc_Absyn_Decl*
_tmp3F1;struct Cyc_Absyn_Stmt*_tmp3F2;struct Cyc_Absyn_Stmt*_tmp3F4;_LL1C7: {
struct Cyc_Absyn_Exp_s_struct*_tmp3EE=(struct Cyc_Absyn_Exp_s_struct*)_tmp3ED;if(
_tmp3EE->tag != 1)goto _LL1C9;else{_tmp3EF=_tmp3EE->f1;}}_LL1C8: Cyc_Toc_lvalue_assign(
_tmp3EF,fs,f,f_env);goto _LL1C6;_LL1C9: {struct Cyc_Absyn_Decl_s_struct*_tmp3F0=(
struct Cyc_Absyn_Decl_s_struct*)_tmp3ED;if(_tmp3F0->tag != 12)goto _LL1CB;else{
_tmp3F1=_tmp3F0->f1;_tmp3F2=_tmp3F0->f2;}}_LL1CA: Cyc_Toc_lvalue_assign_stmt(
_tmp3F2,fs,f,f_env);goto _LL1C6;_LL1CB: {struct Cyc_Absyn_Seq_s_struct*_tmp3F3=(
struct Cyc_Absyn_Seq_s_struct*)_tmp3ED;if(_tmp3F3->tag != 2)goto _LL1CD;else{
_tmp3F4=_tmp3F3->f2;}}_LL1CC: Cyc_Toc_lvalue_assign_stmt(_tmp3F4,fs,f,f_env);goto
_LL1C6;_LL1CD:;_LL1CE: {const char*_tmpB6F;void*_tmpB6E[1];struct Cyc_String_pa_struct
_tmpB6D;(_tmpB6D.tag=0,((_tmpB6D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmpB6E[0]=& _tmpB6D,Cyc_Toc_toc_impos(((_tmpB6F="lvalue_assign_stmt: %s",
_tag_dyneither(_tmpB6F,sizeof(char),23))),_tag_dyneither(_tmpB6E,sizeof(void*),1)))))));}
_LL1C6:;}static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*
e){void*_tmp3F8=e->r;void*_tmp3FA;void**_tmp3FB;struct Cyc_Absyn_Exp*_tmp3FC;
struct Cyc_Absyn_Exp**_tmp3FD;struct Cyc_Absyn_Exp*_tmp3FF;_LL1D0: {struct Cyc_Absyn_Cast_e_struct*
_tmp3F9=(struct Cyc_Absyn_Cast_e_struct*)_tmp3F8;if(_tmp3F9->tag != 15)goto _LL1D2;
else{_tmp3FA=(void**)& _tmp3F9->f1;_tmp3FB=(void**)((void**)& _tmp3F9->f1);_tmp3FC=
_tmp3F9->f2;_tmp3FD=(struct Cyc_Absyn_Exp**)& _tmp3F9->f2;}}_LL1D1:*_tmp3FD=Cyc_Toc_push_address_exp(*
_tmp3FD);*_tmp3FB=Cyc_Absyn_cstar_typ(*_tmp3FB,Cyc_Toc_mt_tq);return e;_LL1D2: {
struct Cyc_Absyn_Deref_e_struct*_tmp3FE=(struct Cyc_Absyn_Deref_e_struct*)_tmp3F8;
if(_tmp3FE->tag != 21)goto _LL1D4;else{_tmp3FF=_tmp3FE->f1;}}_LL1D3: return _tmp3FF;
_LL1D4:;_LL1D5: if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{const
char*_tmpB73;void*_tmpB72[1];struct Cyc_String_pa_struct _tmpB71;(_tmpB71.tag=0,((
_tmpB71.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmpB72[0]=& _tmpB71,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB73="can't take & of %s",_tag_dyneither(_tmpB73,sizeof(char),19))),
_tag_dyneither(_tmpB72,sizeof(void*),1)))))));};_LL1CF:;}static struct Cyc_List_List*
Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x){struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0)return 0;{struct
Cyc_List_List*_tmpB74;result=((_tmpB74=_region_malloc(r2,sizeof(*_tmpB74)),((
_tmpB74->hd=(void*)f((void*)x->hd,env),((_tmpB74->tl=0,_tmpB74))))));}prev=
result;for(x=x->tl;x != 0;x=x->tl){{struct Cyc_List_List*_tmpB75;((struct Cyc_List_List*)
_check_null(prev))->tl=((_tmpB75=_region_malloc(r2,sizeof(*_tmpB75)),((_tmpB75->hd=(
void*)f((void*)x->hd,env),((_tmpB75->tl=0,_tmpB75))))));}prev=prev->tl;}return
result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,
struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
static struct _tuple15*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){struct _tuple15*
_tmpB76;return(_tmpB76=_cycalloc(sizeof(*_tmpB76)),((_tmpB76->f1=0,((_tmpB76->f2=
e,_tmpB76)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*
_tmp406=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp408;_LL1D7: {struct Cyc_Absyn_PointerType_struct*
_tmp407=(struct Cyc_Absyn_PointerType_struct*)_tmp406;if(_tmp407->tag != 5)goto
_LL1D9;else{_tmp408=_tmp407->f1;}}_LL1D8: return _tmp408;_LL1D9:;_LL1DA: {const
char*_tmpB79;void*_tmpB78;(_tmpB78=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB79="get_ptr_typ: not a pointer!",
_tag_dyneither(_tmpB79,sizeof(char),28))),_tag_dyneither(_tmpB78,sizeof(void*),0)));}
_LL1D6:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp40B=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp40E;enum Cyc_Absyn_Size_of
_tmp40F;enum Cyc_Absyn_Sign _tmp411;enum Cyc_Absyn_Size_of _tmp412;enum Cyc_Absyn_Sign
_tmp416;enum Cyc_Absyn_Size_of _tmp417;enum Cyc_Absyn_Sign _tmp419;enum Cyc_Absyn_Size_of
_tmp41A;enum Cyc_Absyn_Sign _tmp41C;enum Cyc_Absyn_Size_of _tmp41D;_LL1DC: {struct
Cyc_Absyn_PointerType_struct*_tmp40C=(struct Cyc_Absyn_PointerType_struct*)
_tmp40B;if(_tmp40C->tag != 5)goto _LL1DE;}_LL1DD: res=Cyc_Absyn_null_exp(0);goto
_LL1DB;_LL1DE: {struct Cyc_Absyn_IntType_struct*_tmp40D=(struct Cyc_Absyn_IntType_struct*)
_tmp40B;if(_tmp40D->tag != 6)goto _LL1E0;else{_tmp40E=_tmp40D->f1;_tmp40F=_tmp40D->f2;
if(_tmp40F != Cyc_Absyn_Char_sz)goto _LL1E0;}}_LL1DF: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(
_tmp40E,'\000'),0);goto _LL1DB;_LL1E0: {struct Cyc_Absyn_IntType_struct*_tmp410=(
struct Cyc_Absyn_IntType_struct*)_tmp40B;if(_tmp410->tag != 6)goto _LL1E2;else{
_tmp411=_tmp410->f1;_tmp412=_tmp410->f2;if(_tmp412 != Cyc_Absyn_Short_sz)goto
_LL1E2;}}_LL1E1: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp411,0),0);goto
_LL1DB;_LL1E2: {struct Cyc_Absyn_EnumType_struct*_tmp413=(struct Cyc_Absyn_EnumType_struct*)
_tmp40B;if(_tmp413->tag != 14)goto _LL1E4;}_LL1E3: goto _LL1E5;_LL1E4: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp414=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp40B;if(_tmp414->tag != 15)goto
_LL1E6;}_LL1E5: _tmp416=Cyc_Absyn_Unsigned;goto _LL1E7;_LL1E6: {struct Cyc_Absyn_IntType_struct*
_tmp415=(struct Cyc_Absyn_IntType_struct*)_tmp40B;if(_tmp415->tag != 6)goto _LL1E8;
else{_tmp416=_tmp415->f1;_tmp417=_tmp415->f2;if(_tmp417 != Cyc_Absyn_Int_sz)goto
_LL1E8;}}_LL1E7: _tmp419=_tmp416;goto _LL1E9;_LL1E8: {struct Cyc_Absyn_IntType_struct*
_tmp418=(struct Cyc_Absyn_IntType_struct*)_tmp40B;if(_tmp418->tag != 6)goto _LL1EA;
else{_tmp419=_tmp418->f1;_tmp41A=_tmp418->f2;if(_tmp41A != Cyc_Absyn_Long_sz)goto
_LL1EA;}}_LL1E9: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp419,0),0);goto _LL1DB;
_LL1EA: {struct Cyc_Absyn_IntType_struct*_tmp41B=(struct Cyc_Absyn_IntType_struct*)
_tmp40B;if(_tmp41B->tag != 6)goto _LL1EC;else{_tmp41C=_tmp41B->f1;_tmp41D=_tmp41B->f2;
if(_tmp41D != Cyc_Absyn_LongLong_sz)goto _LL1EC;}}_LL1EB: res=Cyc_Absyn_const_exp(
Cyc_Absyn_LongLong_c(_tmp41C,(long long)0),0);goto _LL1DB;_LL1EC: {struct Cyc_Absyn_FloatType_struct*
_tmp41E=(struct Cyc_Absyn_FloatType_struct*)_tmp40B;if(_tmp41E->tag != 7)goto
_LL1EE;}_LL1ED: goto _LL1EF;_LL1EE: {struct Cyc_Absyn_DoubleType_struct*_tmp41F=(
struct Cyc_Absyn_DoubleType_struct*)_tmp40B;if(_tmp41F->tag != 8)goto _LL1F0;}
_LL1EF:{const char*_tmpB7A;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB7A="0.0",
_tag_dyneither(_tmpB7A,sizeof(char),4)))),0);}goto _LL1DB;_LL1F0:;_LL1F1: {const
char*_tmpB7E;void*_tmpB7D[1];struct Cyc_String_pa_struct _tmpB7C;(_tmpB7C.tag=0,((
_tmpB7C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmpB7D[0]=& _tmpB7C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB7E="found non-zero type %s in generate_zero",_tag_dyneither(_tmpB7E,sizeof(
char),40))),_tag_dyneither(_tmpB7D,sizeof(void*),1)))))));}_LL1DB:;}{struct Cyc_Core_Opt*
_tmpB7F;res->topt=((_tmpB7F=_cycalloc(sizeof(*_tmpB7F)),((_tmpB7F->v=t,_tmpB7F))));}
return res;}static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dyneither,void*elt_type){void*fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);void*
_tmp425=Cyc_Toc_typ_to_c(elt_type);void*_tmp426=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp427=Cyc_Absyn_cstar_typ(_tmp425,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmpB80;struct Cyc_Core_Opt*_tmp428=(_tmpB80=_cycalloc(sizeof(*_tmpB80)),((
_tmpB80->v=_tmp427,_tmpB80)));struct Cyc_Absyn_Exp*xinit;{void*_tmp429=e1->r;
struct Cyc_Absyn_Exp*_tmp42B;struct Cyc_Absyn_Exp*_tmp42D;struct Cyc_Absyn_Exp*
_tmp42E;_LL1F3: {struct Cyc_Absyn_Deref_e_struct*_tmp42A=(struct Cyc_Absyn_Deref_e_struct*)
_tmp429;if(_tmp42A->tag != 21)goto _LL1F5;else{_tmp42B=_tmp42A->f1;}}_LL1F4: if(!
is_dyneither){_tmp42B=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp42B,0,Cyc_Absyn_Other_coercion,
0);{struct Cyc_Core_Opt*_tmpB81;_tmp42B->topt=((_tmpB81=_cycalloc(sizeof(*_tmpB81)),((
_tmpB81->v=fat_ptr_type,_tmpB81))));};}Cyc_Toc_exp_to_c(nv,_tmp42B);xinit=
_tmp42B;goto _LL1F2;_LL1F5: {struct Cyc_Absyn_Subscript_e_struct*_tmp42C=(struct
Cyc_Absyn_Subscript_e_struct*)_tmp429;if(_tmp42C->tag != 24)goto _LL1F7;else{
_tmp42D=_tmp42C->f1;_tmp42E=_tmp42C->f2;}}_LL1F6: if(!is_dyneither){_tmp42D=Cyc_Absyn_cast_exp(
fat_ptr_type,_tmp42D,0,Cyc_Absyn_Other_coercion,0);{struct Cyc_Core_Opt*_tmpB82;
_tmp42D->topt=((_tmpB82=_cycalloc(sizeof(*_tmpB82)),((_tmpB82->v=fat_ptr_type,
_tmpB82))));};}Cyc_Toc_exp_to_c(nv,_tmp42D);Cyc_Toc_exp_to_c(nv,_tmp42E);{struct
Cyc_Absyn_Exp*_tmpB83[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,((
_tmpB83[2]=_tmp42E,((_tmpB83[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmpB83[0]=_tmp42D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpB83,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1F2;_LL1F7:;_LL1F8: {const char*_tmpB86;void*_tmpB85;(_tmpB85=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB86="found bad lhs for zero-terminated pointer assignment",
_tag_dyneither(_tmpB86,sizeof(char),53))),_tag_dyneither(_tmpB85,sizeof(void*),0)));}
_LL1F2:;}{struct _tuple0*_tmp434=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp436;
struct _RegionHandle*_tmp437;struct Cyc_Toc_Env*_tmp435=nv;_tmp436=*_tmp435;
_tmp437=_tmp436.rgn;{struct Cyc_Toc_Env*_tmp438=Cyc_Toc_add_varmap(_tmp437,nv,
_tmp434,Cyc_Absyn_var_exp(_tmp434,0));struct Cyc_Absyn_Vardecl*_tmpB87;struct Cyc_Absyn_Vardecl*
_tmp439=(_tmpB87=_cycalloc(sizeof(*_tmpB87)),((_tmpB87->sc=Cyc_Absyn_Public,((
_tmpB87->name=_tmp434,((_tmpB87->tq=Cyc_Toc_mt_tq,((_tmpB87->type=_tmp426,((
_tmpB87->initializer=(struct Cyc_Absyn_Exp*)xinit,((_tmpB87->rgn=0,((_tmpB87->attributes=
0,((_tmpB87->escapes=0,_tmpB87)))))))))))))))));struct Cyc_Absyn_Local_b_struct
_tmpB8A;struct Cyc_Absyn_Local_b_struct*_tmpB89;struct Cyc_Absyn_Local_b_struct*
_tmp43A=(_tmpB89=_cycalloc(sizeof(*_tmpB89)),((_tmpB89[0]=((_tmpB8A.tag=4,((
_tmpB8A.f1=_tmp439,_tmpB8A)))),_tmpB89)));struct Cyc_Absyn_Exp*_tmp43B=Cyc_Absyn_varb_exp(
_tmp434,(void*)_tmp43A,0);{struct Cyc_Core_Opt*_tmpB8B;_tmp43B->topt=((_tmpB8B=
_cycalloc(sizeof(*_tmpB8B)),((_tmpB8B->v=fat_ptr_type,_tmpB8B))));}{struct Cyc_Absyn_Exp*
_tmp43D=Cyc_Absyn_deref_exp(_tmp43B,0);{struct Cyc_Core_Opt*_tmpB8C;_tmp43D->topt=((
_tmpB8C=_cycalloc(sizeof(*_tmpB8C)),((_tmpB8C->v=elt_type,_tmpB8C))));}Cyc_Toc_exp_to_c(
_tmp438,_tmp43D);{struct _tuple0*_tmp43F=Cyc_Toc_temp_var();_tmp438=Cyc_Toc_add_varmap(
_tmp437,_tmp438,_tmp43F,Cyc_Absyn_var_exp(_tmp43F,0));{struct Cyc_Absyn_Vardecl*
_tmpB8D;struct Cyc_Absyn_Vardecl*_tmp440=(_tmpB8D=_cycalloc(sizeof(*_tmpB8D)),((
_tmpB8D->sc=Cyc_Absyn_Public,((_tmpB8D->name=_tmp43F,((_tmpB8D->tq=Cyc_Toc_mt_tq,((
_tmpB8D->type=_tmp425,((_tmpB8D->initializer=(struct Cyc_Absyn_Exp*)_tmp43D,((
_tmpB8D->rgn=0,((_tmpB8D->attributes=0,((_tmpB8D->escapes=0,_tmpB8D)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpB90;struct Cyc_Absyn_Local_b_struct*_tmpB8F;
struct Cyc_Absyn_Local_b_struct*_tmp441=(_tmpB8F=_cycalloc(sizeof(*_tmpB8F)),((
_tmpB8F[0]=((_tmpB90.tag=4,((_tmpB90.f1=_tmp440,_tmpB90)))),_tmpB8F)));struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp442=Cyc_Absyn_varb_exp(_tmp43F,(
void*)_tmp441,0);_tmp442->topt=_tmp43D->topt;z_init=Cyc_Absyn_prim2_exp((enum 
Cyc_Absyn_Primop)popt->v,_tmp442,e2,0);z_init->topt=_tmp442->topt;}Cyc_Toc_exp_to_c(
_tmp438,z_init);{struct _tuple0*_tmp443=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmpB91;struct Cyc_Absyn_Vardecl*_tmp444=(_tmpB91=_cycalloc(sizeof(*_tmpB91)),((
_tmpB91->sc=Cyc_Absyn_Public,((_tmpB91->name=_tmp443,((_tmpB91->tq=Cyc_Toc_mt_tq,((
_tmpB91->type=_tmp425,((_tmpB91->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmpB91->rgn=0,((_tmpB91->attributes=0,((_tmpB91->escapes=0,_tmpB91)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpB94;struct Cyc_Absyn_Local_b_struct*_tmpB93;
struct Cyc_Absyn_Local_b_struct*_tmp445=(_tmpB93=_cycalloc(sizeof(*_tmpB93)),((
_tmpB93[0]=((_tmpB94.tag=4,((_tmpB94.f1=_tmp444,_tmpB94)))),_tmpB93)));_tmp438=
Cyc_Toc_add_varmap(_tmp437,_tmp438,_tmp443,Cyc_Absyn_var_exp(_tmp443,0));{struct
Cyc_Absyn_Exp*_tmp446=Cyc_Absyn_varb_exp(_tmp43F,(void*)_tmp441,0);_tmp446->topt=
_tmp43D->topt;{struct Cyc_Absyn_Exp*_tmp447=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp448=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp446,_tmp447,0);{
struct Cyc_Core_Opt*_tmpB95;_tmp448->topt=((_tmpB95=_cycalloc(sizeof(*_tmpB95)),((
_tmpB95->v=Cyc_Absyn_sint_typ,_tmpB95))));}Cyc_Toc_exp_to_c(_tmp438,_tmp448);{
struct Cyc_Absyn_Exp*_tmp44A=Cyc_Absyn_varb_exp(_tmp443,(void*)_tmp445,0);_tmp44A->topt=
_tmp43D->topt;{struct Cyc_Absyn_Exp*_tmp44B=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp44C=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp44A,_tmp44B,0);{
struct Cyc_Core_Opt*_tmpB96;_tmp44C->topt=((_tmpB96=_cycalloc(sizeof(*_tmpB96)),((
_tmpB96->v=Cyc_Absyn_sint_typ,_tmpB96))));}Cyc_Toc_exp_to_c(_tmp438,_tmp44C);{
struct Cyc_Absyn_Exp*_tmpB97[2];struct Cyc_List_List*_tmp44E=(_tmpB97[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0),((_tmpB97[0]=Cyc_Absyn_varb_exp(_tmp434,(void*)
_tmp43A,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB97,sizeof(struct Cyc_Absyn_Exp*),2)))));struct Cyc_Absyn_Exp*
_tmp44F=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;xsize=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp44E,0),
_tmp44F,0);{struct Cyc_Absyn_Exp*_tmp450=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp448,_tmp44C,0),0);struct Cyc_Absyn_Stmt*_tmp451=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp452=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp434,(void*)_tmp43A,0),Cyc_Toc_curr_sp,0);_tmp452=Cyc_Toc_cast_it(
_tmp427,_tmp452);{struct Cyc_Absyn_Exp*_tmp453=Cyc_Absyn_deref_exp(_tmp452,0);
struct Cyc_Absyn_Exp*_tmp454=Cyc_Absyn_assign_exp(_tmp453,Cyc_Absyn_var_exp(
_tmp443,0),0);struct Cyc_Absyn_Stmt*_tmp455=Cyc_Absyn_exp_stmt(_tmp454,0);_tmp455=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp450,_tmp451,Cyc_Absyn_skip_stmt(
0),0),_tmp455,0);{struct Cyc_Absyn_Var_d_struct*_tmpB9D;struct Cyc_Absyn_Var_d_struct
_tmpB9C;struct Cyc_Absyn_Decl*_tmpB9B;_tmp455=Cyc_Absyn_decl_stmt(((_tmpB9B=
_cycalloc(sizeof(*_tmpB9B)),((_tmpB9B->r=(void*)((_tmpB9D=_cycalloc(sizeof(*
_tmpB9D)),((_tmpB9D[0]=((_tmpB9C.tag=0,((_tmpB9C.f1=_tmp444,_tmpB9C)))),_tmpB9D)))),((
_tmpB9B->loc=0,_tmpB9B)))))),_tmp455,0);}{struct Cyc_Absyn_Var_d_struct*_tmpBA3;
struct Cyc_Absyn_Var_d_struct _tmpBA2;struct Cyc_Absyn_Decl*_tmpBA1;_tmp455=Cyc_Absyn_decl_stmt(((
_tmpBA1=_cycalloc(sizeof(*_tmpBA1)),((_tmpBA1->r=(void*)((_tmpBA3=_cycalloc(
sizeof(*_tmpBA3)),((_tmpBA3[0]=((_tmpBA2.tag=0,((_tmpBA2.f1=_tmp440,_tmpBA2)))),
_tmpBA3)))),((_tmpBA1->loc=0,_tmpBA1)))))),_tmp455,0);}{struct Cyc_Absyn_Var_d_struct*
_tmpBA9;struct Cyc_Absyn_Var_d_struct _tmpBA8;struct Cyc_Absyn_Decl*_tmpBA7;_tmp455=
Cyc_Absyn_decl_stmt(((_tmpBA7=_cycalloc(sizeof(*_tmpBA7)),((_tmpBA7->r=(void*)((
_tmpBA9=_cycalloc(sizeof(*_tmpBA9)),((_tmpBA9[0]=((_tmpBA8.tag=0,((_tmpBA8.f1=
_tmp439,_tmpBA8)))),_tmpBA9)))),((_tmpBA7->loc=0,_tmpBA7)))))),_tmp455,0);}e->r=
Cyc_Toc_stmt_exp_r(_tmp455);};};};};};};};};};};};};};}static void*Cyc_Toc_check_tagged_union(
struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,
struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,
struct Cyc_Position_Segment*)){struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp46A=Cyc_Tcutil_compress(
aggrtype);struct Cyc_Absyn_AggrInfo _tmp46C;union Cyc_Absyn_AggrInfoU _tmp46D;_LL1FA: {
struct Cyc_Absyn_AggrType_struct*_tmp46B=(struct Cyc_Absyn_AggrType_struct*)
_tmp46A;if(_tmp46B->tag != 12)goto _LL1FC;else{_tmp46C=_tmp46B->f1;_tmp46D=_tmp46C.aggr_info;}}
_LL1FB: ad=Cyc_Absyn_get_known_aggrdecl(_tmp46D);goto _LL1F9;_LL1FC:;_LL1FD: {
struct Cyc_String_pa_struct _tmpBB1;void*_tmpBB0[1];const char*_tmpBAF;void*_tmpBAE;(
_tmpBAE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((
struct _dyneither_ptr)((_tmpBB1.tag=0,((_tmpBB1.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpBB0[0]=& _tmpBB1,Cyc_aprintf(((
_tmpBAF="expecting union but found %s in check_tagged_union",_tag_dyneither(
_tmpBAF,sizeof(char),51))),_tag_dyneither(_tmpBB0,sizeof(void*),1)))))))),
_tag_dyneither(_tmpBAE,sizeof(void*),0)));}_LL1F9:;}{struct _tuple0*_tmp472=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp473=Cyc_Absyn_var_exp(_tmp472,0);struct Cyc_Absyn_Exp*
_tmp474=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp473,f,0),Cyc_Toc_tag_sp,0);struct
Cyc_Absyn_Exp*_tmp475=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
struct Cyc_Absyn_Exp*_tmp476=Cyc_Absyn_neq_exp(_tmp474,_tmp475,0);struct Cyc_Absyn_Exp*
_tmp477=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp473,f,0),Cyc_Toc_val_sp,0);struct
Cyc_Absyn_Stmt*_tmp478=Cyc_Absyn_exp_stmt(_tmp477,0);struct Cyc_Absyn_Stmt*
_tmp479=Cyc_Absyn_ifthenelse_stmt(_tmp476,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp47A=Cyc_Absyn_declare_stmt(_tmp472,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp479,_tmp478,0),0);return Cyc_Toc_stmt_exp_r(
_tmp47A);};}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read){void*_tmp47B=e->r;struct Cyc_Absyn_Exp*
_tmp47D;struct Cyc_Absyn_Exp*_tmp47F;struct _dyneither_ptr*_tmp480;int _tmp481;int*
_tmp482;struct Cyc_Absyn_Exp*_tmp484;struct _dyneither_ptr*_tmp485;int _tmp486;int*
_tmp487;_LL1FF: {struct Cyc_Absyn_Cast_e_struct*_tmp47C=(struct Cyc_Absyn_Cast_e_struct*)
_tmp47B;if(_tmp47C->tag != 15)goto _LL201;else{_tmp47D=_tmp47C->f2;}}_LL200: {
const char*_tmpBB4;void*_tmpBB3;(_tmpBB3=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB4="cast on lhs!",
_tag_dyneither(_tmpBB4,sizeof(char),13))),_tag_dyneither(_tmpBB3,sizeof(void*),0)));}
_LL201: {struct Cyc_Absyn_AggrMember_e_struct*_tmp47E=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp47B;if(_tmp47E->tag != 22)goto _LL203;else{_tmp47F=_tmp47E->f1;_tmp480=_tmp47E->f2;
_tmp481=_tmp47E->f4;_tmp482=(int*)& _tmp47E->f4;}}_LL202: {void*_tmp48A=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp47F->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp48C;union Cyc_Absyn_AggrInfoU _tmp48D;_LL208: {struct Cyc_Absyn_AggrType_struct*
_tmp48B=(struct Cyc_Absyn_AggrType_struct*)_tmp48A;if(_tmp48B->tag != 12)goto
_LL20A;else{_tmp48C=_tmp48B->f1;_tmp48D=_tmp48C.aggr_info;}}_LL209: {struct Cyc_Absyn_Aggrdecl*
_tmp48E=Cyc_Absyn_get_known_aggrdecl(_tmp48D);*f_tag=Cyc_Toc_get_member_offset(
_tmp48E,_tmp480);{const char*_tmpBB9;void*_tmpBB8[2];struct Cyc_String_pa_struct
_tmpBB7;struct Cyc_String_pa_struct _tmpBB6;struct _dyneither_ptr str=(struct
_dyneither_ptr)((_tmpBB6.tag=0,((_tmpBB6.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp480),((_tmpBB7.tag=0,((_tmpBB7.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp48E->name).f2),((_tmpBB8[0]=& _tmpBB7,((_tmpBB8[1]=&
_tmpBB6,Cyc_aprintf(((_tmpBB9="_union_%s_%s",_tag_dyneither(_tmpBB9,sizeof(char),
13))),_tag_dyneither(_tmpBB8,sizeof(void*),2))))))))))))));{struct _dyneither_ptr*
_tmpBBA;*tagged_member_type=Cyc_Absyn_strct(((_tmpBBA=_cycalloc(sizeof(*_tmpBBA)),((
_tmpBBA[0]=str,_tmpBBA)))));}if(clear_read)*_tmp482=0;return((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp48E->impl))->tagged;};}_LL20A:;_LL20B: return 0;_LL207:;}_LL203: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp483=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp47B;if(_tmp483->tag != 23)goto _LL205;else{_tmp484=_tmp483->f1;_tmp485=_tmp483->f2;
_tmp486=_tmp483->f4;_tmp487=(int*)& _tmp483->f4;}}_LL204: {void*_tmp494=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp484->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp496;void*_tmp497;_LL20D: {struct Cyc_Absyn_PointerType_struct*_tmp495=(struct
Cyc_Absyn_PointerType_struct*)_tmp494;if(_tmp495->tag != 5)goto _LL20F;else{
_tmp496=_tmp495->f1;_tmp497=_tmp496.elt_typ;}}_LL20E: {void*_tmp498=Cyc_Tcutil_compress(
_tmp497);struct Cyc_Absyn_AggrInfo _tmp49A;union Cyc_Absyn_AggrInfoU _tmp49B;_LL212: {
struct Cyc_Absyn_AggrType_struct*_tmp499=(struct Cyc_Absyn_AggrType_struct*)
_tmp498;if(_tmp499->tag != 12)goto _LL214;else{_tmp49A=_tmp499->f1;_tmp49B=_tmp49A.aggr_info;}}
_LL213: {struct Cyc_Absyn_Aggrdecl*_tmp49C=Cyc_Absyn_get_known_aggrdecl(_tmp49B);*
f_tag=Cyc_Toc_get_member_offset(_tmp49C,_tmp485);{const char*_tmpBBF;void*_tmpBBE[
2];struct Cyc_String_pa_struct _tmpBBD;struct Cyc_String_pa_struct _tmpBBC;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpBBC.tag=0,((_tmpBBC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp485),((_tmpBBD.tag=0,((_tmpBBD.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp49C->name).f2),((_tmpBBE[0]=&
_tmpBBD,((_tmpBBE[1]=& _tmpBBC,Cyc_aprintf(((_tmpBBF="_union_%s_%s",
_tag_dyneither(_tmpBBF,sizeof(char),13))),_tag_dyneither(_tmpBBE,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpBC0;*tagged_member_type=Cyc_Absyn_strct(((_tmpBC0=
_cycalloc(sizeof(*_tmpBC0)),((_tmpBC0[0]=str,_tmpBC0)))));}if(clear_read)*
_tmp487=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp49C->impl))->tagged;};}
_LL214:;_LL215: return 0;_LL211:;}_LL20F:;_LL210: return 0;_LL20C:;}_LL205:;_LL206:
return 0;_LL1FE:;}void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*
e,int tag,void*mem_type){struct _tuple0*_tmp4A2=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
temp_exp=Cyc_Absyn_var_exp(_tmp4A2,0);struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(
tag,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,
0),0);struct Cyc_Absyn_Exp*_tmp4A3=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_ifthenelse_stmt(_tmp4A3,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4A2,Cyc_Absyn_cstar_typ(
mem_type,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(
e)),Cyc_Absyn_seq_stmt(s2,s3,0),0);e->r=Cyc_Toc_stmt_exp_r(s1);}static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type){struct _tuple0*_tmp4A4=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4A4,0);struct Cyc_Absyn_Exp*
temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*
temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 
0)s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*
_tmp4A5=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp4A5,
Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=
Cyc_Absyn_declare_stmt(_tmp4A4,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(e1),Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple19{struct _tuple0*f1;void*f2;struct
Cyc_Absyn_Exp*f3;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*
f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void _tmpC13(unsigned int*_tmpC12,unsigned int*_tmpC11,struct _tuple0***
_tmpC0F){for(*_tmpC12=0;*_tmpC12 < *_tmpC11;(*_tmpC12)++){(*_tmpC0F)[*_tmpC12]=
Cyc_Toc_temp_var();}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){void*_tmp4A6=e->r;if(e->topt == 0){const char*_tmpBC4;void*_tmpBC3[1];struct Cyc_String_pa_struct
_tmpBC2;(_tmpBC2.tag=0,((_tmpBC2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpBC3[0]=& _tmpBC2,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC4="exp_to_c: no type for %s",
_tag_dyneither(_tmpBC4,sizeof(char),25))),_tag_dyneither(_tmpBC3,sizeof(void*),1)))))));}{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp4AA=
_tmp4A6;union Cyc_Absyn_Cnst _tmp4AC;int _tmp4AD;struct _tuple0*_tmp4B0;void*_tmp4B1;
struct _tuple0*_tmp4B3;enum Cyc_Absyn_Primop _tmp4B5;struct Cyc_List_List*_tmp4B6;
struct Cyc_Absyn_Exp*_tmp4B8;enum Cyc_Absyn_Incrementor _tmp4B9;struct Cyc_Absyn_Exp*
_tmp4BB;struct Cyc_Core_Opt*_tmp4BC;struct Cyc_Absyn_Exp*_tmp4BD;struct Cyc_Absyn_Exp*
_tmp4BF;struct Cyc_Absyn_Exp*_tmp4C0;struct Cyc_Absyn_Exp*_tmp4C1;struct Cyc_Absyn_Exp*
_tmp4C3;struct Cyc_Absyn_Exp*_tmp4C4;struct Cyc_Absyn_Exp*_tmp4C6;struct Cyc_Absyn_Exp*
_tmp4C7;struct Cyc_Absyn_Exp*_tmp4C9;struct Cyc_Absyn_Exp*_tmp4CA;struct Cyc_Absyn_Exp*
_tmp4CC;struct Cyc_List_List*_tmp4CD;struct Cyc_Absyn_Exp*_tmp4CF;struct Cyc_List_List*
_tmp4D0;struct Cyc_Absyn_VarargCallInfo*_tmp4D1;struct Cyc_Absyn_Exp*_tmp4D3;
struct Cyc_List_List*_tmp4D4;struct Cyc_Absyn_VarargCallInfo*_tmp4D5;struct Cyc_Absyn_VarargCallInfo
_tmp4D6;int _tmp4D7;struct Cyc_List_List*_tmp4D8;struct Cyc_Absyn_VarargInfo*
_tmp4D9;struct Cyc_Absyn_Exp*_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DD;struct Cyc_Absyn_Exp*
_tmp4DF;struct Cyc_List_List*_tmp4E0;void*_tmp4E2;void**_tmp4E3;struct Cyc_Absyn_Exp*
_tmp4E4;int _tmp4E5;enum Cyc_Absyn_Coercion _tmp4E6;struct Cyc_Absyn_Exp*_tmp4E8;
struct Cyc_Absyn_Exp*_tmp4EA;struct Cyc_Absyn_Exp*_tmp4EB;struct Cyc_Absyn_Exp*
_tmp4ED;void*_tmp4EF;void*_tmp4F1;void*_tmp4F2;struct _dyneither_ptr*_tmp4F4;void*
_tmp4F6;void*_tmp4F7;unsigned int _tmp4F9;struct Cyc_Absyn_Exp*_tmp4FB;struct Cyc_Absyn_Exp*
_tmp4FD;struct _dyneither_ptr*_tmp4FE;int _tmp4FF;int _tmp500;struct Cyc_Absyn_Exp*
_tmp502;struct _dyneither_ptr*_tmp503;int _tmp504;int _tmp505;struct Cyc_Absyn_Exp*
_tmp507;struct Cyc_Absyn_Exp*_tmp508;struct Cyc_List_List*_tmp50A;struct Cyc_List_List*
_tmp50C;struct Cyc_Absyn_Vardecl*_tmp50E;struct Cyc_Absyn_Exp*_tmp50F;struct Cyc_Absyn_Exp*
_tmp510;int _tmp511;struct _tuple0*_tmp513;struct Cyc_List_List*_tmp514;struct Cyc_List_List*
_tmp515;struct Cyc_Absyn_Aggrdecl*_tmp516;void*_tmp518;struct Cyc_List_List*
_tmp519;struct Cyc_List_List*_tmp51B;struct Cyc_Absyn_Datatypedecl*_tmp51C;struct
Cyc_Absyn_Datatypefield*_tmp51D;struct Cyc_Absyn_MallocInfo _tmp521;int _tmp522;
struct Cyc_Absyn_Exp*_tmp523;void**_tmp524;struct Cyc_Absyn_Exp*_tmp525;int _tmp526;
struct Cyc_Absyn_Exp*_tmp528;struct Cyc_Absyn_Exp*_tmp529;struct Cyc_Absyn_Exp*
_tmp52B;struct _dyneither_ptr*_tmp52C;struct Cyc_Absyn_Stmt*_tmp52E;_LL217: {
struct Cyc_Absyn_Const_e_struct*_tmp4AB=(struct Cyc_Absyn_Const_e_struct*)_tmp4AA;
if(_tmp4AB->tag != 0)goto _LL219;else{_tmp4AC=_tmp4AB->f1;if((_tmp4AC.Null_c).tag
!= 1)goto _LL219;_tmp4AD=(int)(_tmp4AC.Null_c).val;}}_LL218: {struct Cyc_Absyn_Exp*
_tmp533=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp533,
_tmp533))->r;else{struct Cyc_Absyn_Exp*_tmpBC5[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((
_tmpBC5[2]=_tmp533,((_tmpBC5[1]=_tmp533,((_tmpBC5[0]=_tmp533,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBC5,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL216;}_LL219: {struct Cyc_Absyn_Const_e_struct*
_tmp4AE=(struct Cyc_Absyn_Const_e_struct*)_tmp4AA;if(_tmp4AE->tag != 0)goto _LL21B;}
_LL21A: goto _LL216;_LL21B: {struct Cyc_Absyn_Var_e_struct*_tmp4AF=(struct Cyc_Absyn_Var_e_struct*)
_tmp4AA;if(_tmp4AF->tag != 1)goto _LL21D;else{_tmp4B0=_tmp4AF->f1;_tmp4B1=(void*)
_tmp4AF->f2;}}_LL21C:{struct _handler_cons _tmp535;_push_handler(& _tmp535);{int
_tmp537=0;if(setjmp(_tmp535.handler))_tmp537=1;if(!_tmp537){e->r=(Cyc_Toc_lookup_varmap(
nv,_tmp4B0))->r;;_pop_handler();}else{void*_tmp536=(void*)_exn_thrown;void*
_tmp539=_tmp536;_LL270: {struct Cyc_Dict_Absent_struct*_tmp53A=(struct Cyc_Dict_Absent_struct*)
_tmp539;if(_tmp53A->tag != Cyc_Dict_Absent)goto _LL272;}_LL271: {const char*_tmpBC9;
void*_tmpBC8[1];struct Cyc_String_pa_struct _tmpBC7;(_tmpBC7.tag=0,((_tmpBC7.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4B0)),((
_tmpBC8[0]=& _tmpBC7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBC9="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpBC9,sizeof(char),
32))),_tag_dyneither(_tmpBC8,sizeof(void*),1)))))));}_LL272:;_LL273:(void)_throw(
_tmp539);_LL26F:;}};}goto _LL216;_LL21D: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp4B2=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp4AA;if(_tmp4B2->tag != 2)goto
_LL21F;else{_tmp4B3=_tmp4B2->f1;}}_LL21E: {const char*_tmpBCC;void*_tmpBCB;(
_tmpBCB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBCC="unknownid",_tag_dyneither(_tmpBCC,sizeof(char),10))),_tag_dyneither(
_tmpBCB,sizeof(void*),0)));}_LL21F: {struct Cyc_Absyn_Primop_e_struct*_tmp4B4=(
struct Cyc_Absyn_Primop_e_struct*)_tmp4AA;if(_tmp4B4->tag != 3)goto _LL221;else{
_tmp4B5=_tmp4B4->f1;_tmp4B6=_tmp4B4->f2;}}_LL220: {struct Cyc_List_List*_tmp540=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4B6);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp4B6);switch(_tmp4B5){case Cyc_Absyn_Numelts: _LL274: {
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp4B6))->hd;{void*_tmp541=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo _tmp543;struct Cyc_Absyn_Exp*
_tmp544;struct Cyc_Absyn_PtrInfo _tmp546;void*_tmp547;struct Cyc_Absyn_PtrAtts
_tmp548;union Cyc_Absyn_Constraint*_tmp549;union Cyc_Absyn_Constraint*_tmp54A;
union Cyc_Absyn_Constraint*_tmp54B;_LL277: {struct Cyc_Absyn_ArrayType_struct*
_tmp542=(struct Cyc_Absyn_ArrayType_struct*)_tmp541;if(_tmp542->tag != 9)goto
_LL279;else{_tmp543=_tmp542->f1;_tmp544=_tmp543.num_elts;}}_LL278: if(!Cyc_Evexp_c_can_eval((
struct Cyc_Absyn_Exp*)_check_null(_tmp544))){const char*_tmpBCF;void*_tmpBCE;(
_tmpBCE=0,Cyc_Tcutil_terr(e->loc,((_tmpBCF="can't calculate numelts",
_tag_dyneither(_tmpBCF,sizeof(char),24))),_tag_dyneither(_tmpBCE,sizeof(void*),0)));}
e->r=_tmp544->r;goto _LL276;_LL279: {struct Cyc_Absyn_PointerType_struct*_tmp545=(
struct Cyc_Absyn_PointerType_struct*)_tmp541;if(_tmp545->tag != 5)goto _LL27B;else{
_tmp546=_tmp545->f1;_tmp547=_tmp546.elt_typ;_tmp548=_tmp546.ptr_atts;_tmp549=
_tmp548.nullable;_tmp54A=_tmp548.bounds;_tmp54B=_tmp548.zero_term;}}_LL27A:{void*
_tmp54E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp54A);struct Cyc_Absyn_Exp*_tmp551;_LL27E: {struct Cyc_Absyn_DynEither_b_struct*
_tmp54F=(struct Cyc_Absyn_DynEither_b_struct*)_tmp54E;if(_tmp54F->tag != 0)goto
_LL280;}_LL27F:{struct Cyc_Absyn_Exp*_tmpBD0[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,((
_tmpBD0[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp547),0),((_tmpBD0[0]=(
struct Cyc_Absyn_Exp*)_tmp4B6->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpBD0,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL27D;_LL280: {struct Cyc_Absyn_Upper_b_struct*_tmp550=(struct Cyc_Absyn_Upper_b_struct*)
_tmp54E;if(_tmp550->tag != 1)goto _LL27D;else{_tmp551=_tmp550->f1;}}_LL281: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp54B)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp4B6->hd);struct Cyc_Absyn_Exp*_tmpBD1[2];e->r=Cyc_Toc_fncall_exp_r(
function_e,((_tmpBD1[1]=_tmp551,((_tmpBD1[0]=(struct Cyc_Absyn_Exp*)_tmp4B6->hd,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpBD1,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp549)){if(!Cyc_Evexp_c_can_eval(_tmp551)){const
char*_tmpBD4;void*_tmpBD3;(_tmpBD3=0,Cyc_Tcutil_terr(e->loc,((_tmpBD4="can't calculate numelts",
_tag_dyneither(_tmpBD4,sizeof(char),24))),_tag_dyneither(_tmpBD3,sizeof(void*),0)));}
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp551,Cyc_Absyn_uint_exp(0,0));}else{e->r=
_tmp551->r;goto _LL27D;}}goto _LL27D;_LL27D:;}goto _LL276;_LL27B:;_LL27C: {const
char*_tmpBD9;void*_tmpBD8[2];struct Cyc_String_pa_struct _tmpBD7;struct Cyc_String_pa_struct
_tmpBD6;(_tmpBD6.tag=0,((_tmpBD6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((
_tmpBD7.tag=0,((_tmpBD7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmpBD8[0]=& _tmpBD7,((
_tmpBD8[1]=& _tmpBD6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBD9="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpBD9,
sizeof(char),41))),_tag_dyneither(_tmpBD8,sizeof(void*),2)))))))))))));}_LL276:;}
break;}case Cyc_Absyn_Plus: _LL275:{void*_tmp55A=Cyc_Tcutil_compress((void*)((
struct Cyc_List_List*)_check_null(_tmp540))->hd);struct Cyc_Absyn_PtrInfo _tmp55C;
void*_tmp55D;struct Cyc_Absyn_PtrAtts _tmp55E;union Cyc_Absyn_Constraint*_tmp55F;
union Cyc_Absyn_Constraint*_tmp560;_LL284: {struct Cyc_Absyn_PointerType_struct*
_tmp55B=(struct Cyc_Absyn_PointerType_struct*)_tmp55A;if(_tmp55B->tag != 5)goto
_LL286;else{_tmp55C=_tmp55B->f1;_tmp55D=_tmp55C.elt_typ;_tmp55E=_tmp55C.ptr_atts;
_tmp55F=_tmp55E.bounds;_tmp560=_tmp55E.zero_term;}}_LL285:{void*_tmp561=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp55F);struct Cyc_Absyn_Exp*_tmp564;_LL289: {struct Cyc_Absyn_DynEither_b_struct*
_tmp562=(struct Cyc_Absyn_DynEither_b_struct*)_tmp561;if(_tmp562->tag != 0)goto
_LL28B;}_LL28A: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp4B6))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp4B6->tl))->hd;{struct Cyc_Absyn_Exp*_tmpBDA[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBDA[2]=e2,((_tmpBDA[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp55D),0),((_tmpBDA[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBDA,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}goto _LL288;}_LL28B: {struct Cyc_Absyn_Upper_b_struct*_tmp563=(struct
Cyc_Absyn_Upper_b_struct*)_tmp561;if(_tmp563->tag != 1)goto _LL288;else{_tmp564=
_tmp563->f1;}}_LL28C: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp560)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp4B6))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp4B6->tl))->hd;struct Cyc_Absyn_Exp*_tmpBDB[3];e->r=(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
e1),((_tmpBDB[2]=e2,((_tmpBDB[1]=_tmp564,((_tmpBDB[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBDB,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r;}goto _LL288;_LL288:;}goto _LL283;_LL286:;_LL287: goto _LL283;
_LL283:;}break;case Cyc_Absyn_Minus: _LL282: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp540))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4B6))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4B6->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp540->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{struct Cyc_Absyn_Exp*_tmpBDC[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBDC[2]=Cyc_Absyn_prim1_exp(
Cyc_Absyn_Minus,e2,0),((_tmpBDC[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpBDC[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBDC,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}break;}case Cyc_Absyn_Eq:
_LL28D: goto _LL28E;case Cyc_Absyn_Neq: _LL28E: goto _LL28F;case Cyc_Absyn_Gt: _LL28F:
goto _LL290;case Cyc_Absyn_Gte: _LL290: goto _LL291;case Cyc_Absyn_Lt: _LL291: goto
_LL292;case Cyc_Absyn_Lte: _LL292: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4B6))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4B6->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp540))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp540->tl))->hd;void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& elt_typ))e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,
0));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp,0));break;}default: _LL293: break;}goto _LL216;}_LL221: {
struct Cyc_Absyn_Increment_e_struct*_tmp4B7=(struct Cyc_Absyn_Increment_e_struct*)
_tmp4AA;if(_tmp4B7->tag != 5)goto _LL223;else{_tmp4B8=_tmp4B7->f1;_tmp4B9=_tmp4B7->f2;}}
_LL222: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4B8->topt))->v;
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;const char*_tmpBDD;struct _dyneither_ptr incr_str=(_tmpBDD="increment",
_tag_dyneither(_tmpBDD,sizeof(char),10));if(_tmp4B9 == Cyc_Absyn_PreDec  || 
_tmp4B9 == Cyc_Absyn_PostDec){const char*_tmpBDE;incr_str=((_tmpBDE="decrement",
_tag_dyneither(_tmpBDE,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp4B8,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpBE2;void*_tmpBE1[1];
struct Cyc_String_pa_struct _tmpBE0;(_tmpBE0.tag=0,((_tmpBE0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBE1[0]=& _tmpBE0,Cyc_Tcutil_terr(
e->loc,((_tmpBE2="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpBE2,sizeof(char),74))),_tag_dyneither(_tmpBE1,sizeof(void*),1)))))));}{
const char*_tmpBE5;void*_tmpBE4;(_tmpBE4=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE5="in-place inc/dec on zero-term",
_tag_dyneither(_tmpBE5,sizeof(char),30))),_tag_dyneither(_tmpBE4,sizeof(void*),0)));};}{
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp4B8,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp56E=Cyc_Absyn_signed_int_exp(
1,0);{struct Cyc_Core_Opt*_tmpBE6;_tmp56E->topt=((_tmpBE6=_cycalloc(sizeof(*
_tmpBE6)),((_tmpBE6->v=Cyc_Absyn_sint_typ,_tmpBE6))));}switch(_tmp4B9){case Cyc_Absyn_PreInc:
_LL295:{struct Cyc_Absyn_AssignOp_e_struct _tmpBEC;struct Cyc_Core_Opt*_tmpBEB;
struct Cyc_Absyn_AssignOp_e_struct*_tmpBEA;e->r=(void*)((_tmpBEA=_cycalloc(
sizeof(*_tmpBEA)),((_tmpBEA[0]=((_tmpBEC.tag=4,((_tmpBEC.f1=_tmp4B8,((_tmpBEC.f2=((
_tmpBEB=_cycalloc_atomic(sizeof(*_tmpBEB)),((_tmpBEB->v=(void*)Cyc_Absyn_Plus,
_tmpBEB)))),((_tmpBEC.f3=_tmp56E,_tmpBEC)))))))),_tmpBEA))));}Cyc_Toc_exp_to_c(
nv,e);return;case Cyc_Absyn_PreDec: _LL296:{struct Cyc_Absyn_AssignOp_e_struct
_tmpBF2;struct Cyc_Core_Opt*_tmpBF1;struct Cyc_Absyn_AssignOp_e_struct*_tmpBF0;e->r=(
void*)((_tmpBF0=_cycalloc(sizeof(*_tmpBF0)),((_tmpBF0[0]=((_tmpBF2.tag=4,((
_tmpBF2.f1=_tmp4B8,((_tmpBF2.f2=((_tmpBF1=_cycalloc_atomic(sizeof(*_tmpBF1)),((
_tmpBF1->v=(void*)Cyc_Absyn_Minus,_tmpBF1)))),((_tmpBF2.f3=_tmp56E,_tmpBF2)))))))),
_tmpBF0))));}Cyc_Toc_exp_to_c(nv,e);return;default: _LL297:{const char*_tmpBF6;
void*_tmpBF5[1];struct Cyc_String_pa_struct _tmpBF4;(_tmpBF4.tag=0,((_tmpBF4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBF5[0]=& _tmpBF4,Cyc_Tcutil_terr(
e->loc,((_tmpBF6="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpBF6,sizeof(char),59))),_tag_dyneither(_tmpBF5,sizeof(void*),1)))))));}{
const char*_tmpBF9;void*_tmpBF8;(_tmpBF8=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF9="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpBF9,sizeof(char),34))),_tag_dyneither(_tmpBF8,sizeof(void*),0)));};}}
Cyc_Toc_exp_to_c(nv,_tmp4B8);{void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp4B9 == Cyc_Absyn_PostInc
 || _tmp4B9 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp4B9 == Cyc_Absyn_PreDec  || _tmp4B9 == Cyc_Absyn_PostDec)change=- 1;{struct
Cyc_Absyn_Exp*_tmpBFA[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBFA[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpBFA[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpBFA[0]=Cyc_Toc_push_address_exp(_tmp4B8),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBFA,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Toc_functionSet*_tmp57C=_tmp4B9 == Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:&
Cyc_Toc__zero_arr_inplace_plus_post_functionSet;struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(
_tmp57C,_tmp4B8);struct Cyc_Absyn_Exp*_tmpBFB[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((
_tmpBFB[1]=Cyc_Absyn_signed_int_exp(1,0),((_tmpBFB[0]=Cyc_Toc_push_address_exp(
_tmp4B8),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBFB,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(elt_typ == (
void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4B8)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(
_tmp4B8,0,Cyc_Toc_incr_lvalue,_tmp4B9);e->r=_tmp4B8->r;}}}goto _LL216;};};}_LL223: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp4BA=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp4AA;if(_tmp4BA->tag != 4)goto _LL225;else{_tmp4BB=_tmp4BA->f1;_tmp4BC=_tmp4BA->f2;
_tmp4BD=_tmp4BA->f3;}}_LL224: {void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4BB->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4BD->topt))->v;int f_tag=0;void*tagged_member_struct_type=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Toc_is_tagged_union_project(_tmp4BB,& f_tag,&
tagged_member_struct_type,1)){Cyc_Toc_exp_to_c(nv,_tmp4BB);Cyc_Toc_exp_to_c(nv,
_tmp4BD);e->r=Cyc_Toc_tagged_union_assignop(_tmp4BB,e1_old_typ,_tmp4BC,_tmp4BD,
e2_old_typ,f_tag,tagged_member_struct_type);return;}{void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp4BB,& ptr_type,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,
_tmp4BB,_tmp4BC,_tmp4BD,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp4BB);Cyc_Toc_exp_to_c(nv,_tmp4BB);Cyc_Toc_exp_to_c(nv,_tmp4BD);{int done=0;
if(_tmp4BC != 0){void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*change;switch((enum Cyc_Absyn_Primop)
_tmp4BC->v){case Cyc_Absyn_Plus: _LL299: change=_tmp4BD;break;case Cyc_Absyn_Minus:
_LL29A: change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4BD,0);break;default:
_LL29B: {const char*_tmpBFE;void*_tmpBFD;(_tmpBFD=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBFE="bad t ? pointer arithmetic",
_tag_dyneither(_tmpBFE,sizeof(char),27))),_tag_dyneither(_tmpBFD,sizeof(void*),0)));}}
done=1;{struct Cyc_Absyn_Exp*_tmp581=Cyc_Toc__dyneither_ptr_inplace_plus_e;struct
Cyc_Absyn_Exp*_tmpBFF[3];e->r=Cyc_Toc_fncall_exp_r(_tmp581,((_tmpBFF[2]=change,((
_tmpBFF[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBFF[0]=Cyc_Toc_push_address_exp(
_tmp4BB),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBFF,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ))switch((enum Cyc_Absyn_Primop)_tmp4BC->v){case Cyc_Absyn_Plus:
_LL29D: done=1;{struct Cyc_Absyn_Exp*_tmpC00[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4BB),((_tmpC00[1]=_tmp4BD,((
_tmpC00[0]=_tmp4BB,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC00,sizeof(struct Cyc_Absyn_Exp*),2)))))));}break;default:
_LL29E: {const char*_tmpC03;void*_tmpC02;(_tmpC02=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC03="bad zero-terminated pointer arithmetic",
_tag_dyneither(_tmpC03,sizeof(char),39))),_tag_dyneither(_tmpC02,sizeof(void*),0)));}}}}
if(!done){if(e1_poly)_tmp4BD->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp4BD->r,0));if(!Cyc_Absyn_is_lvalue(_tmp4BB)){{struct _tuple18 _tmpC06;struct
_tuple18*_tmpC05;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple18*),struct _tuple18*f_env))
Cyc_Toc_lvalue_assign)(_tmp4BB,0,Cyc_Toc_assignop_lvalue,((_tmpC05=_cycalloc(
sizeof(struct _tuple18)* 1),((_tmpC05[0]=((_tmpC06.f1=_tmp4BC,((_tmpC06.f2=
_tmp4BD,_tmpC06)))),_tmpC05)))));}e->r=_tmp4BB->r;}}goto _LL216;};};};}_LL225: {
struct Cyc_Absyn_Conditional_e_struct*_tmp4BE=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp4AA;if(_tmp4BE->tag != 6)goto _LL227;else{_tmp4BF=_tmp4BE->f1;_tmp4C0=_tmp4BE->f2;
_tmp4C1=_tmp4BE->f3;}}_LL226: Cyc_Toc_exp_to_c(nv,_tmp4BF);Cyc_Toc_exp_to_c(nv,
_tmp4C0);Cyc_Toc_exp_to_c(nv,_tmp4C1);goto _LL216;_LL227: {struct Cyc_Absyn_And_e_struct*
_tmp4C2=(struct Cyc_Absyn_And_e_struct*)_tmp4AA;if(_tmp4C2->tag != 7)goto _LL229;
else{_tmp4C3=_tmp4C2->f1;_tmp4C4=_tmp4C2->f2;}}_LL228: Cyc_Toc_exp_to_c(nv,
_tmp4C3);Cyc_Toc_exp_to_c(nv,_tmp4C4);goto _LL216;_LL229: {struct Cyc_Absyn_Or_e_struct*
_tmp4C5=(struct Cyc_Absyn_Or_e_struct*)_tmp4AA;if(_tmp4C5->tag != 8)goto _LL22B;
else{_tmp4C6=_tmp4C5->f1;_tmp4C7=_tmp4C5->f2;}}_LL22A: Cyc_Toc_exp_to_c(nv,
_tmp4C6);Cyc_Toc_exp_to_c(nv,_tmp4C7);goto _LL216;_LL22B: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp4C8=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp4AA;if(_tmp4C8->tag != 9)goto _LL22D;
else{_tmp4C9=_tmp4C8->f1;_tmp4CA=_tmp4C8->f2;}}_LL22C: Cyc_Toc_exp_to_c(nv,
_tmp4C9);Cyc_Toc_exp_to_c(nv,_tmp4CA);goto _LL216;_LL22D: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp4CB=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp4AA;if(_tmp4CB->tag != 10)goto
_LL22F;else{_tmp4CC=_tmp4CB->f1;_tmp4CD=_tmp4CB->f2;}}_LL22E: _tmp4CF=_tmp4CC;
_tmp4D0=_tmp4CD;goto _LL230;_LL22F: {struct Cyc_Absyn_FnCall_e_struct*_tmp4CE=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp4AA;if(_tmp4CE->tag != 11)goto _LL231;else{
_tmp4CF=_tmp4CE->f1;_tmp4D0=_tmp4CE->f2;_tmp4D1=_tmp4CE->f3;if(_tmp4D1 != 0)goto
_LL231;}}_LL230: Cyc_Toc_exp_to_c(nv,_tmp4CF);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp4D0);goto _LL216;_LL231: {struct Cyc_Absyn_FnCall_e_struct*
_tmp4D2=(struct Cyc_Absyn_FnCall_e_struct*)_tmp4AA;if(_tmp4D2->tag != 11)goto
_LL233;else{_tmp4D3=_tmp4D2->f1;_tmp4D4=_tmp4D2->f2;_tmp4D5=_tmp4D2->f3;if(
_tmp4D5 == 0)goto _LL233;_tmp4D6=*_tmp4D5;_tmp4D7=_tmp4D6.num_varargs;_tmp4D8=
_tmp4D6.injectors;_tmp4D9=_tmp4D6.vai;}}_LL232: {struct Cyc_Toc_Env _tmp589;struct
_RegionHandle*_tmp58A;struct Cyc_Toc_Env*_tmp588=nv;_tmp589=*_tmp588;_tmp58A=
_tmp589.rgn;{struct _tuple0*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=
Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp4D7,0);void*cva_type=Cyc_Toc_typ_to_c(_tmp4D9->type);void*
arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp4D4);int num_normargs=num_args - _tmp4D7;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp4D4=_tmp4D4->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4D4))->hd);{
struct Cyc_List_List*_tmpC07;new_args=((_tmpC07=_cycalloc(sizeof(*_tmpC07)),((
_tmpC07->hd=(struct Cyc_Absyn_Exp*)_tmp4D4->hd,((_tmpC07->tl=new_args,_tmpC07))))));};}}{
struct Cyc_Absyn_Exp*_tmpC0A[3];struct Cyc_List_List*_tmpC09;new_args=((_tmpC09=
_cycalloc(sizeof(*_tmpC09)),((_tmpC09->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((
_tmpC0A[2]=num_varargs_exp,((_tmpC0A[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0),((
_tmpC0A[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC0A,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpC09->tl=
new_args,_tmpC09))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp4D3);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4D3,new_args,0),0);if(_tmp4D9->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp58E=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp4D9->type));struct Cyc_Absyn_DatatypeInfo _tmp590;union Cyc_Absyn_DatatypeInfoU
_tmp591;struct Cyc_Absyn_Datatypedecl**_tmp592;struct Cyc_Absyn_Datatypedecl*
_tmp593;_LL2A1: {struct Cyc_Absyn_DatatypeType_struct*_tmp58F=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp58E;if(_tmp58F->tag != 3)goto _LL2A3;else{_tmp590=_tmp58F->f1;_tmp591=_tmp590.datatype_info;
if((_tmp591.KnownDatatype).tag != 2)goto _LL2A3;_tmp592=(struct Cyc_Absyn_Datatypedecl**)(
_tmp591.KnownDatatype).val;_tmp593=*_tmp592;}}_LL2A2: tud=_tmp593;goto _LL2A0;
_LL2A3:;_LL2A4: {const char*_tmpC0D;void*_tmpC0C;(_tmpC0C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC0D="toc: unknown datatype in vararg with inject",
_tag_dyneither(_tmpC0D,sizeof(char),44))),_tag_dyneither(_tmpC0C,sizeof(void*),0)));}
_LL2A0:;}{unsigned int _tmpC12;unsigned int _tmpC11;struct _dyneither_ptr _tmpC10;
struct _tuple0**_tmpC0F;unsigned int _tmpC0E;struct _dyneither_ptr vs=(_tmpC0E=(
unsigned int)_tmp4D7,((_tmpC0F=(struct _tuple0**)_region_malloc(_tmp58A,
_check_times(sizeof(struct _tuple0*),_tmpC0E)),((_tmpC10=_tag_dyneither(_tmpC0F,
sizeof(struct _tuple0*),_tmpC0E),((((_tmpC11=_tmpC0E,_tmpC13(& _tmpC12,& _tmpC11,&
_tmpC0F))),_tmpC10)))))));if(_tmp4D7 != 0){struct Cyc_List_List*_tmp596=0;{int i=
_tmp4D7 - 1;for(0;i >= 0;-- i){struct Cyc_List_List*_tmpC14;_tmp596=((_tmpC14=
_cycalloc(sizeof(*_tmpC14)),((_tmpC14->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct
_tuple0*),i)),0),0)),((_tmpC14->tl=_tmp596,_tmpC14))))));}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp596,0),s,0);{
int i=0;for(0;_tmp4D4 != 0;(((_tmp4D4=_tmp4D4->tl,_tmp4D8=_tmp4D8->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4D4->hd;void*arg_type=(void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct _tuple0*var=*((struct
_tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield _tmp599;struct
_tuple0*_tmp59A;struct Cyc_List_List*_tmp59B;struct Cyc_Absyn_Datatypefield*
_tmp598=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp4D8))->hd;_tmp599=*_tmp598;_tmp59A=_tmp599.name;_tmp59B=_tmp599.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)_check_null(
_tmp59B))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp59A),0),s,0);{const char*_tmpC15;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp59A,((_tmpC15="_struct",_tag_dyneither(_tmpC15,
sizeof(char),8))))),0,s,0);};};}};}else{struct _tuple15*_tmpC16[3];struct Cyc_List_List*
_tmp59D=(_tmpC16[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC16[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpC16[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC16,sizeof(struct _tuple15*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{{int i=0;for(0;_tmp4D4
!= 0;(_tmp4D4=_tmp4D4->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp4D4->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4D4->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL216;}_LL233: {struct Cyc_Absyn_Throw_e_struct*_tmp4DA=(struct Cyc_Absyn_Throw_e_struct*)
_tmp4AA;if(_tmp4DA->tag != 12)goto _LL235;else{_tmp4DB=_tmp4DA->f1;}}_LL234: Cyc_Toc_exp_to_c(
nv,_tmp4DB);e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(
_tmp4DB),0))->r;goto _LL216;_LL235: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp4DC=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp4AA;if(_tmp4DC->tag != 13)
goto _LL237;else{_tmp4DD=_tmp4DC->f1;}}_LL236: Cyc_Toc_exp_to_c(nv,_tmp4DD);goto
_LL216;_LL237: {struct Cyc_Absyn_Instantiate_e_struct*_tmp4DE=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp4AA;if(_tmp4DE->tag != 14)goto _LL239;else{_tmp4DF=_tmp4DE->f1;_tmp4E0=_tmp4DE->f2;}}
_LL238: Cyc_Toc_exp_to_c(nv,_tmp4DF);for(0;_tmp4E0 != 0;_tmp4E0=_tmp4E0->tl){enum 
Cyc_Absyn_KindQual _tmp5A3=(Cyc_Tcutil_typ_kind((void*)_tmp4E0->hd))->kind;if(
_tmp5A3 != Cyc_Absyn_EffKind  && _tmp5A3 != Cyc_Absyn_RgnKind){{void*_tmp5A4=Cyc_Tcutil_compress((
void*)_tmp4E0->hd);_LL2A6: {struct Cyc_Absyn_VarType_struct*_tmp5A5=(struct Cyc_Absyn_VarType_struct*)
_tmp5A4;if(_tmp5A5->tag != 2)goto _LL2A8;}_LL2A7: goto _LL2A9;_LL2A8: {struct Cyc_Absyn_DatatypeType_struct*
_tmp5A6=(struct Cyc_Absyn_DatatypeType_struct*)_tmp5A4;if(_tmp5A6->tag != 3)goto
_LL2AA;}_LL2A9: continue;_LL2AA:;_LL2AB: e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4DF,0))->r;goto _LL2A5;
_LL2A5:;}break;}}goto _LL216;_LL239: {struct Cyc_Absyn_Cast_e_struct*_tmp4E1=(
struct Cyc_Absyn_Cast_e_struct*)_tmp4AA;if(_tmp4E1->tag != 15)goto _LL23B;else{
_tmp4E2=(void**)& _tmp4E1->f1;_tmp4E3=(void**)((void**)& _tmp4E1->f1);_tmp4E4=
_tmp4E1->f2;_tmp4E5=_tmp4E1->f3;_tmp4E6=_tmp4E1->f4;}}_LL23A: {void*old_t2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4E4->topt))->v;void*new_typ=*_tmp4E3;*
_tmp4E3=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp4E4);{struct _tuple17
_tmpC17;struct _tuple17 _tmp5A8=(_tmpC17.f1=Cyc_Tcutil_compress(old_t2),((_tmpC17.f2=
Cyc_Tcutil_compress(new_typ),_tmpC17)));void*_tmp5A9;struct Cyc_Absyn_PtrInfo
_tmp5AB;void*_tmp5AC;struct Cyc_Absyn_PtrInfo _tmp5AE;void*_tmp5AF;struct Cyc_Absyn_PtrInfo
_tmp5B1;void*_tmp5B2;_LL2AD: _tmp5A9=_tmp5A8.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp5AA=(struct Cyc_Absyn_PointerType_struct*)_tmp5A9;if(_tmp5AA->tag != 5)goto
_LL2AF;else{_tmp5AB=_tmp5AA->f1;}};_tmp5AC=_tmp5A8.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp5AD=(struct Cyc_Absyn_PointerType_struct*)_tmp5AC;if(_tmp5AD->tag != 5)goto
_LL2AF;else{_tmp5AE=_tmp5AD->f1;}};_LL2AE: {int _tmp5B4=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp5AB.ptr_atts).nullable);int _tmp5B5=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5AE.ptr_atts).nullable);
void*_tmp5B6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp5AB.ptr_atts).bounds);void*_tmp5B7=((void*(*)(void*y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5AE.ptr_atts).bounds);
int _tmp5B8=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp5AB.ptr_atts).zero_term);int _tmp5B9=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp5AE.ptr_atts).zero_term);{struct _tuple17 _tmpC18;
struct _tuple17 _tmp5BB=(_tmpC18.f1=_tmp5B6,((_tmpC18.f2=_tmp5B7,_tmpC18)));void*
_tmp5BC;struct Cyc_Absyn_Exp*_tmp5BE;void*_tmp5BF;struct Cyc_Absyn_Exp*_tmp5C1;
void*_tmp5C2;struct Cyc_Absyn_Exp*_tmp5C4;void*_tmp5C5;void*_tmp5C7;void*_tmp5C9;
struct Cyc_Absyn_Exp*_tmp5CB;void*_tmp5CC;void*_tmp5CE;_LL2B4: _tmp5BC=_tmp5BB.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp5BD=(struct Cyc_Absyn_Upper_b_struct*)_tmp5BC;
if(_tmp5BD->tag != 1)goto _LL2B6;else{_tmp5BE=_tmp5BD->f1;}};_tmp5BF=_tmp5BB.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp5C0=(struct Cyc_Absyn_Upper_b_struct*)_tmp5BF;
if(_tmp5C0->tag != 1)goto _LL2B6;else{_tmp5C1=_tmp5C0->f1;}};_LL2B5: if((!Cyc_Evexp_c_can_eval(
_tmp5BE) || !Cyc_Evexp_c_can_eval(_tmp5C1)) && !Cyc_Evexp_same_const_exp(_tmp5BE,
_tmp5C1)){const char*_tmpC1B;void*_tmpC1A;(_tmpC1A=0,Cyc_Tcutil_terr(e->loc,((
_tmpC1B="can't validate cast due to potential size differences",_tag_dyneither(
_tmpC1B,sizeof(char),54))),_tag_dyneither(_tmpC1A,sizeof(void*),0)));}if(_tmp5B4
 && !_tmp5B5){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC1E;void*_tmpC1D;(
_tmpC1D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC1E="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC1E,
sizeof(char),44))),_tag_dyneither(_tmpC1D,sizeof(void*),0)));}if(_tmp4E6 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC22;void*_tmpC21[1];struct Cyc_String_pa_struct _tmpC20;(_tmpC20.tag=
0,((_tmpC20.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpC21[0]=& _tmpC20,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpC22="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpC22,sizeof(char),41))),_tag_dyneither(_tmpC21,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp4E4);if(do_null_check){if(!_tmp4E5){
const char*_tmpC25;void*_tmpC24;(_tmpC24=0,Cyc_Tcutil_warn(e->loc,((_tmpC25="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpC25,sizeof(char),58))),_tag_dyneither(_tmpC24,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC26;e->r=Cyc_Toc_cast_it_r(*_tmp4E3,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC26=_cycalloc(sizeof(*_tmpC26)),((_tmpC26->hd=
_tmp4E4,((_tmpC26->tl=0,_tmpC26)))))),0));};}};}goto _LL2B3;_LL2B6: _tmp5C2=
_tmp5BB.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp5C3=(struct Cyc_Absyn_Upper_b_struct*)
_tmp5C2;if(_tmp5C3->tag != 1)goto _LL2B8;else{_tmp5C4=_tmp5C3->f1;}};_tmp5C5=
_tmp5BB.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5C6=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5C5;if(_tmp5C6->tag != 0)goto _LL2B8;};_LL2B7: if(!Cyc_Evexp_c_can_eval(_tmp5C4)){
const char*_tmpC29;void*_tmpC28;(_tmpC28=0,Cyc_Tcutil_terr(e->loc,((_tmpC29="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC29,sizeof(char),71))),_tag_dyneither(_tmpC28,sizeof(void*),0)));}
if(_tmp4E6 == Cyc_Absyn_NonNull_to_Null){const char*_tmpC2D;void*_tmpC2C[1];struct
Cyc_String_pa_struct _tmpC2B;(_tmpC2B.tag=0,((_tmpC2B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC2C[0]=& _tmpC2B,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC2D="conversion mis-classified as null-check: %s",
_tag_dyneither(_tmpC2D,sizeof(char),44))),_tag_dyneither(_tmpC2C,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){if((_tmp5B8  && !(_tmp5AE.elt_tq).real_const) && !
_tmp5B9)_tmp5C4=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp5C4,Cyc_Absyn_uint_exp(1,
0),0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp5C4,_tmp4E4))->r;}else{
struct Cyc_Absyn_Exp*_tmp5DF=Cyc_Toc__tag_dyneither_e;if(_tmp5B8){struct _tuple0*
_tmp5E0=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp5E1=Cyc_Absyn_var_exp(_tmp5E0,
0);struct Cyc_Absyn_Exp*arg3;{void*_tmp5E2=_tmp4E4->r;union Cyc_Absyn_Cnst _tmp5E4;
struct _dyneither_ptr _tmp5E5;_LL2BD: {struct Cyc_Absyn_Const_e_struct*_tmp5E3=(
struct Cyc_Absyn_Const_e_struct*)_tmp5E2;if(_tmp5E3->tag != 0)goto _LL2BF;else{
_tmp5E4=_tmp5E3->f1;if((_tmp5E4.String_c).tag != 7)goto _LL2BF;_tmp5E5=(struct
_dyneither_ptr)(_tmp5E4.String_c).val;}}_LL2BE: arg3=_tmp5C4;goto _LL2BC;_LL2BF:;
_LL2C0:{struct Cyc_Absyn_Exp*_tmpC2E[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp4E4),((_tmpC2E[1]=_tmp5C4,((_tmpC2E[0]=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp5E1),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2E,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);}goto _LL2BC;_LL2BC:;}if(!_tmp5B9  && !(_tmp5AE.elt_tq).real_const)arg3=
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*
_tmp5E7=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5AE.elt_typ),0);struct Cyc_Absyn_Exp*
_tmpC2F[3];struct Cyc_Absyn_Exp*_tmp5E8=Cyc_Absyn_fncall_exp(_tmp5DF,((_tmpC2F[2]=
arg3,((_tmpC2F[1]=_tmp5E7,((_tmpC2F[0]=_tmp5E1,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2F,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);struct Cyc_Absyn_Stmt*_tmp5E9=Cyc_Absyn_exp_stmt(_tmp5E8,0);_tmp5E9=
Cyc_Absyn_declare_stmt(_tmp5E0,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)
_tmp4E4,_tmp5E9,0);e->r=Cyc_Toc_stmt_exp_r(_tmp5E9);};}else{struct Cyc_Absyn_Exp*
_tmpC30[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5DF,((_tmpC30[2]=_tmp5C4,((_tmpC30[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5AE.elt_typ),0),((_tmpC30[0]=
_tmp4E4,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC30,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2B3;
_LL2B8: _tmp5C7=_tmp5BB.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp5C8=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5C7;if(_tmp5C8->tag != 0)goto _LL2BA;};_tmp5C9=_tmp5BB.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp5CA=(struct Cyc_Absyn_Upper_b_struct*)_tmp5C9;if(_tmp5CA->tag != 1)goto _LL2BA;
else{_tmp5CB=_tmp5CA->f1;}};_LL2B9: if(!Cyc_Evexp_c_can_eval(_tmp5CB)){const char*
_tmpC33;void*_tmpC32;(_tmpC32=0,Cyc_Tcutil_terr(e->loc,((_tmpC33="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC33,sizeof(char),71))),_tag_dyneither(_tmpC32,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC36;void*_tmpC35;(_tmpC35=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC36="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpC36,sizeof(char),45))),_tag_dyneither(_tmpC35,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5F0=_tmp5CB;if(_tmp5B8  && !_tmp5B9)_tmp5F0=Cyc_Absyn_add_exp(
_tmp5CB,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp5F1=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC37[3];struct Cyc_Absyn_Exp*_tmp5F2=Cyc_Absyn_fncall_exp(
_tmp5F1,((_tmpC37[2]=_tmp5F0,((_tmpC37[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp5AB.elt_typ),0),((_tmpC37[0]=_tmp4E4,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC37,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp5B5){struct Cyc_List_List*_tmpC38;_tmp5F2->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_null_e,((_tmpC38=_cycalloc(sizeof(*_tmpC38)),((_tmpC38->hd=Cyc_Absyn_copy_exp(
_tmp5F2),((_tmpC38->tl=0,_tmpC38)))))));}e->r=Cyc_Toc_cast_it_r(*_tmp4E3,_tmp5F2);
goto _LL2B3;};};_LL2BA: _tmp5CC=_tmp5BB.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp5CD=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5CC;if(_tmp5CD->tag != 0)goto
_LL2B3;};_tmp5CE=_tmp5BB.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5CF=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp5CE;if(_tmp5CF->tag != 0)goto _LL2B3;};_LL2BB:
DynCast: if((_tmp5B8  && !_tmp5B9) && !(_tmp5AE.elt_tq).real_const){if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpC3B;void*_tmpC3A;(_tmpC3A=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC3B="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpC3B,sizeof(char),70))),_tag_dyneither(_tmpC3A,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5F7=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpC3C[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5F7,((_tmpC3C[2]=Cyc_Absyn_uint_exp(1,0),((
_tmpC3C[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5AB.elt_typ),0),((
_tmpC3C[0]=_tmp4E4,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC3C,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}goto _LL2B3;
_LL2B3:;}goto _LL2AC;}_LL2AF: _tmp5AF=_tmp5A8.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp5B0=(struct Cyc_Absyn_PointerType_struct*)_tmp5AF;if(_tmp5B0->tag != 5)goto
_LL2B1;else{_tmp5B1=_tmp5B0->f1;}};_tmp5B2=_tmp5A8.f2;{struct Cyc_Absyn_IntType_struct*
_tmp5B3=(struct Cyc_Absyn_IntType_struct*)_tmp5B2;if(_tmp5B3->tag != 6)goto _LL2B1;};
_LL2B0:{void*_tmp5F9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp5B1.ptr_atts).bounds);_LL2C2: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5FA=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5F9;if(_tmp5FA->tag != 0)goto
_LL2C4;}_LL2C3: _tmp4E4->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp4E4->r,
_tmp4E4->loc),Cyc_Toc_curr_sp);goto _LL2C1;_LL2C4:;_LL2C5: goto _LL2C1;_LL2C1:;}
goto _LL2AC;_LL2B1:;_LL2B2: goto _LL2AC;_LL2AC:;}goto _LL216;}_LL23B: {struct Cyc_Absyn_Address_e_struct*
_tmp4E7=(struct Cyc_Absyn_Address_e_struct*)_tmp4AA;if(_tmp4E7->tag != 16)goto
_LL23D;else{_tmp4E8=_tmp4E7->f1;}}_LL23C:{void*_tmp5FB=_tmp4E8->r;struct _tuple0*
_tmp5FD;struct Cyc_List_List*_tmp5FE;struct Cyc_List_List*_tmp5FF;struct Cyc_List_List*
_tmp601;_LL2C7: {struct Cyc_Absyn_Aggregate_e_struct*_tmp5FC=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp5FB;if(_tmp5FC->tag != 29)goto _LL2C9;else{_tmp5FD=_tmp5FC->f1;_tmp5FE=_tmp5FC->f2;
_tmp5FF=_tmp5FC->f3;}}_LL2C8: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC40;void*
_tmpC3F[1];struct Cyc_String_pa_struct _tmpC3E;(_tmpC3E.tag=0,((_tmpC3E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4E8->loc)),((
_tmpC3F[0]=& _tmpC3E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC40="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC40,
sizeof(char),42))),_tag_dyneither(_tmpC3F,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4E8->topt))->v,_tmp5FE != 0,1,0,
_tmp5FF,_tmp5FD))->r;goto _LL2C6;_LL2C9: {struct Cyc_Absyn_Tuple_e_struct*_tmp600=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp5FB;if(_tmp600->tag != 25)goto _LL2CB;else{
_tmp601=_tmp600->f1;}}_LL2CA: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC44;void*
_tmpC43[1];struct Cyc_String_pa_struct _tmpC42;(_tmpC42.tag=0,((_tmpC42.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4E8->loc)),((
_tmpC43[0]=& _tmpC42,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC44="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC44,
sizeof(char),42))),_tag_dyneither(_tmpC43,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_tuple(
nv,1,0,_tmp601))->r;goto _LL2C6;_LL2CB:;_LL2CC: Cyc_Toc_exp_to_c(nv,_tmp4E8);if(!
Cyc_Absyn_is_lvalue(_tmp4E8)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp4E8,0,Cyc_Toc_address_lvalue,1);e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4E8);}goto _LL2C6;
_LL2C6:;}goto _LL216;_LL23D: {struct Cyc_Absyn_New_e_struct*_tmp4E9=(struct Cyc_Absyn_New_e_struct*)
_tmp4AA;if(_tmp4E9->tag != 17)goto _LL23F;else{_tmp4EA=_tmp4E9->f1;_tmp4EB=_tmp4E9->f2;}}
_LL23E: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC48;void*_tmpC47[1];struct Cyc_String_pa_struct
_tmpC46;(_tmpC46.tag=0,((_tmpC46.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4EB->loc)),((_tmpC47[0]=& _tmpC46,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC48="%s: new at top-level",
_tag_dyneither(_tmpC48,sizeof(char),21))),_tag_dyneither(_tmpC47,sizeof(void*),1)))))));}{
void*_tmp60B=_tmp4EB->r;struct Cyc_List_List*_tmp60D;struct Cyc_Absyn_Vardecl*
_tmp60F;struct Cyc_Absyn_Exp*_tmp610;struct Cyc_Absyn_Exp*_tmp611;int _tmp612;
struct _tuple0*_tmp614;struct Cyc_List_List*_tmp615;struct Cyc_List_List*_tmp616;
struct Cyc_Absyn_Aggrdecl*_tmp617;struct Cyc_List_List*_tmp619;_LL2CE: {struct Cyc_Absyn_Array_e_struct*
_tmp60C=(struct Cyc_Absyn_Array_e_struct*)_tmp60B;if(_tmp60C->tag != 27)goto _LL2D0;
else{_tmp60D=_tmp60C->f1;}}_LL2CF: {struct _tuple0*_tmp61A=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp61B=Cyc_Absyn_var_exp(_tmp61A,0);struct Cyc_Absyn_Stmt*
_tmp61C=Cyc_Toc_init_array(nv,_tmp61B,_tmp60D,Cyc_Absyn_exp_stmt(_tmp61B,0));
void*old_elt_typ;{void*_tmp61D=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp61F;void*_tmp620;struct Cyc_Absyn_Tqual _tmp621;struct Cyc_Absyn_PtrAtts _tmp622;
union Cyc_Absyn_Constraint*_tmp623;_LL2D9: {struct Cyc_Absyn_PointerType_struct*
_tmp61E=(struct Cyc_Absyn_PointerType_struct*)_tmp61D;if(_tmp61E->tag != 5)goto
_LL2DB;else{_tmp61F=_tmp61E->f1;_tmp620=_tmp61F.elt_typ;_tmp621=_tmp61F.elt_tq;
_tmp622=_tmp61F.ptr_atts;_tmp623=_tmp622.zero_term;}}_LL2DA: old_elt_typ=_tmp620;
goto _LL2D8;_LL2DB:;_LL2DC: {const char*_tmpC4B;void*_tmpC4A;old_elt_typ=((_tmpC4A=
0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC4B="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(
_tmpC4B,sizeof(char),52))),_tag_dyneither(_tmpC4A,sizeof(void*),0))));}_LL2D8:;}{
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp626=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp627=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp60D),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp4EA == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp627);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp4EA;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp627);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp61A,_tmp626,(struct Cyc_Absyn_Exp*)e1,_tmp61C,0));goto
_LL2CD;};}_LL2D0: {struct Cyc_Absyn_Comprehension_e_struct*_tmp60E=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp60B;if(_tmp60E->tag != 28)goto _LL2D2;else{_tmp60F=_tmp60E->f1;_tmp610=_tmp60E->f2;
_tmp611=_tmp60E->f3;_tmp612=_tmp60E->f4;}}_LL2D1: {int is_dyneither_ptr=0;{void*
_tmp628=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp62A;void*_tmp62B;
struct Cyc_Absyn_Tqual _tmp62C;struct Cyc_Absyn_PtrAtts _tmp62D;union Cyc_Absyn_Constraint*
_tmp62E;union Cyc_Absyn_Constraint*_tmp62F;_LL2DE: {struct Cyc_Absyn_PointerType_struct*
_tmp629=(struct Cyc_Absyn_PointerType_struct*)_tmp628;if(_tmp629->tag != 5)goto
_LL2E0;else{_tmp62A=_tmp629->f1;_tmp62B=_tmp62A.elt_typ;_tmp62C=_tmp62A.elt_tq;
_tmp62D=_tmp62A.ptr_atts;_tmp62E=_tmp62D.bounds;_tmp62F=_tmp62D.zero_term;}}
_LL2DF: is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp62E)== (void*)& Cyc_Absyn_DynEither_b_val;goto _LL2DD;
_LL2E0:;_LL2E1: {const char*_tmpC4E;void*_tmpC4D;(_tmpC4D=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC4E="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpC4E,sizeof(char),42))),_tag_dyneither(_tmpC4D,sizeof(void*),0)));}
_LL2DD:;}{struct _tuple0*max=Cyc_Toc_temp_var();struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp611->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp610);{struct Cyc_Absyn_Exp*_tmp632=
Cyc_Absyn_var_exp(max,0);if(_tmp612)_tmp632=Cyc_Absyn_add_exp(_tmp632,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp60F,Cyc_Absyn_var_exp(max,0),_tmp611,_tmp612,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp634;struct _RegionHandle*_tmp635;struct Cyc_Toc_Env*_tmp633=
nv;_tmp634=*_tmp633;_tmp635=_tmp634.rgn;{struct _tuple19*_tmpC51;struct Cyc_List_List*
_tmpC50;struct Cyc_List_List*decls=(_tmpC50=_region_malloc(_tmp635,sizeof(*
_tmpC50)),((_tmpC50->hd=((_tmpC51=_region_malloc(_tmp635,sizeof(*_tmpC51)),((
_tmpC51->f1=max,((_tmpC51->f2=Cyc_Absyn_uint_typ,((_tmpC51->f3=(struct Cyc_Absyn_Exp*)
_tmp610,_tmpC51)))))))),((_tmpC50->tl=0,_tmpC50)))));struct Cyc_Absyn_Exp*ai;if(
_tmp4EA == 0  || Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*_tmpC52[2];ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((_tmpC52[1]=_tmp632,((
_tmpC52[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC52,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4EA;Cyc_Toc_exp_to_c(
nv,r);{struct Cyc_Absyn_Exp*_tmpC53[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpC53[1]=_tmp632,((_tmpC53[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC53,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct _tuple19*_tmpC56;struct Cyc_List_List*
_tmpC55;decls=((_tmpC55=_region_malloc(_tmp635,sizeof(*_tmpC55)),((_tmpC55->hd=((
_tmpC56=_region_malloc(_tmp635,sizeof(*_tmpC56)),((_tmpC56->f1=a,((_tmpC56->f2=
ptr_typ,((_tmpC56->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpC56)))))))),((_tmpC55->tl=
decls,_tmpC55))))));}if(is_dyneither_ptr){struct _tuple0*_tmp63A=Cyc_Toc_temp_var();
void*_tmp63B=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp63C=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC57[3];struct Cyc_Absyn_Exp*_tmp63D=Cyc_Absyn_fncall_exp(
_tmp63C,((_tmpC57[2]=_tmp632,((_tmpC57[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((
_tmpC57[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC57,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{
struct _tuple19*_tmpC5A;struct Cyc_List_List*_tmpC59;decls=((_tmpC59=
_region_malloc(_tmp635,sizeof(*_tmpC59)),((_tmpC59->hd=((_tmpC5A=_region_malloc(
_tmp635,sizeof(*_tmpC5A)),((_tmpC5A->f1=_tmp63A,((_tmpC5A->f2=_tmp63B,((_tmpC5A->f3=(
struct Cyc_Absyn_Exp*)_tmp63D,_tmpC5A)))))))),((_tmpC59->tl=decls,_tmpC59))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp63A,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp641=decls;for(0;_tmp641 != 0;_tmp641=_tmp641->tl){struct
_tuple0*_tmp643;void*_tmp644;struct Cyc_Absyn_Exp*_tmp645;struct _tuple19 _tmp642=*((
struct _tuple19*)_tmp641->hd);_tmp643=_tmp642.f1;_tmp644=_tmp642.f2;_tmp645=
_tmp642.f3;s=Cyc_Absyn_declare_stmt(_tmp643,_tmp644,_tmp645,s,0);}}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL2CD;};};};}_LL2D2: {struct Cyc_Absyn_Aggregate_e_struct*_tmp613=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp60B;if(_tmp613->tag != 29)goto _LL2D4;
else{_tmp614=_tmp613->f1;_tmp615=_tmp613->f2;_tmp616=_tmp613->f3;_tmp617=_tmp613->f4;}}
_LL2D3: e->r=(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4EB->topt))->v,_tmp615 != 0,1,_tmp4EA,_tmp616,_tmp614))->r;goto _LL2CD;_LL2D4: {
struct Cyc_Absyn_Tuple_e_struct*_tmp618=(struct Cyc_Absyn_Tuple_e_struct*)_tmp60B;
if(_tmp618->tag != 25)goto _LL2D6;else{_tmp619=_tmp618->f1;}}_LL2D5: e->r=(Cyc_Toc_init_tuple(
nv,1,_tmp4EA,_tmp619))->r;goto _LL2CD;_LL2D6:;_LL2D7: {void*old_elt_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4EB->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);struct _tuple0*_tmp648=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp649=Cyc_Absyn_var_exp(_tmp648,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(_tmp649,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp4EA
== 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4EA;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp64A=_tmp4EB->r;void*_tmp64C;
struct Cyc_Absyn_Exp*_tmp64D;_LL2E3: {struct Cyc_Absyn_Cast_e_struct*_tmp64B=(
struct Cyc_Absyn_Cast_e_struct*)_tmp64A;if(_tmp64B->tag != 15)goto _LL2E5;else{
_tmp64C=(void*)_tmp64B->f1;_tmp64D=_tmp64B->f2;}}_LL2E4:{struct _tuple17 _tmpC5B;
struct _tuple17 _tmp64F=(_tmpC5B.f1=Cyc_Tcutil_compress(_tmp64C),((_tmpC5B.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp64D->topt))->v),_tmpC5B)));void*
_tmp650;struct Cyc_Absyn_PtrInfo _tmp652;void*_tmp653;struct Cyc_Absyn_PtrAtts
_tmp654;union Cyc_Absyn_Constraint*_tmp655;void*_tmp656;struct Cyc_Absyn_PtrInfo
_tmp658;struct Cyc_Absyn_PtrAtts _tmp659;union Cyc_Absyn_Constraint*_tmp65A;_LL2E8:
_tmp650=_tmp64F.f1;{struct Cyc_Absyn_PointerType_struct*_tmp651=(struct Cyc_Absyn_PointerType_struct*)
_tmp650;if(_tmp651->tag != 5)goto _LL2EA;else{_tmp652=_tmp651->f1;_tmp653=_tmp652.elt_typ;
_tmp654=_tmp652.ptr_atts;_tmp655=_tmp654.bounds;}};_tmp656=_tmp64F.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp657=(struct Cyc_Absyn_PointerType_struct*)_tmp656;if(_tmp657->tag != 5)goto
_LL2EA;else{_tmp658=_tmp657->f1;_tmp659=_tmp658.ptr_atts;_tmp65A=_tmp659.bounds;}};
_LL2E9:{struct _tuple17 _tmpC5C;struct _tuple17 _tmp65C=(_tmpC5C.f1=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp655),((_tmpC5C.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp65A),_tmpC5C)));void*_tmp65D;void*_tmp65F;struct Cyc_Absyn_Exp*
_tmp661;_LL2ED: _tmp65D=_tmp65C.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp65E=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp65D;if(_tmp65E->tag != 0)goto _LL2EF;};
_tmp65F=_tmp65C.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp660=(struct Cyc_Absyn_Upper_b_struct*)
_tmp65F;if(_tmp660->tag != 1)goto _LL2EF;else{_tmp661=_tmp660->f1;}};_LL2EE: Cyc_Toc_exp_to_c(
nv,_tmp64D);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp662=Cyc_Toc__init_dyneither_ptr_e;{struct Cyc_Absyn_Exp*_tmpC5D[4];e->r=Cyc_Toc_fncall_exp_r(
_tmp662,((_tmpC5D[3]=_tmp661,((_tmpC5D[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp653),0),((_tmpC5D[1]=_tmp64D,((_tmpC5D[0]=mexp,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5D,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))));}goto _LL2EC;};_LL2EF:;_LL2F0: goto _LL2EC;_LL2EC:;}goto _LL2E7;_LL2EA:;
_LL2EB: goto _LL2E7;_LL2E7:;}goto _LL2E2;_LL2E5:;_LL2E6: goto _LL2E2;_LL2E2:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp664=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp649),
0);struct Cyc_Absyn_Exp*_tmp665=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,
_tmp4EB);_tmp664=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
_tmp649,_tmp665,0),_tmp4EB,0),_tmp664,0);{void*_tmp666=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp648,
_tmp666,(struct Cyc_Absyn_Exp*)mexp,_tmp664,0));};}goto _LL2CD;};}_LL2CD:;}goto
_LL216;_LL23F: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp4EC=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp4AA;if(_tmp4EC->tag != 19)goto _LL241;else{_tmp4ED=_tmp4EC->f1;}}_LL240: Cyc_Toc_exp_to_c(
nv,_tmp4ED);goto _LL216;_LL241: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4EE=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp4AA;if(_tmp4EE->tag != 18)goto _LL243;
else{_tmp4EF=(void*)_tmp4EE->f1;}}_LL242:{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpC60;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpC5F;e->r=(void*)((_tmpC5F=
_cycalloc(sizeof(*_tmpC5F)),((_tmpC5F[0]=((_tmpC60.tag=18,((_tmpC60.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4EF),_tmpC60)))),_tmpC5F))));}goto _LL216;_LL243: {
struct Cyc_Absyn_Offsetof_e_struct*_tmp4F0=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp4AA;if(_tmp4F0->tag != 20)goto _LL245;else{_tmp4F1=(void*)_tmp4F0->f1;_tmp4F2=(
void*)_tmp4F0->f2;{struct Cyc_Absyn_StructField_struct*_tmp4F3=(struct Cyc_Absyn_StructField_struct*)
_tmp4F2;if(_tmp4F3->tag != 0)goto _LL245;else{_tmp4F4=_tmp4F3->f1;}};}}_LL244:{
struct Cyc_Absyn_Offsetof_e_struct _tmpC6A;struct Cyc_Absyn_StructField_struct
_tmpC69;struct Cyc_Absyn_StructField_struct*_tmpC68;struct Cyc_Absyn_Offsetof_e_struct*
_tmpC67;e->r=(void*)((_tmpC67=_cycalloc(sizeof(*_tmpC67)),((_tmpC67[0]=((_tmpC6A.tag=
20,((_tmpC6A.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4F1),((_tmpC6A.f2=(void*)((
void*)((_tmpC68=_cycalloc(sizeof(*_tmpC68)),((_tmpC68[0]=((_tmpC69.tag=0,((
_tmpC69.f1=_tmp4F4,_tmpC69)))),_tmpC68))))),_tmpC6A)))))),_tmpC67))));}goto
_LL216;_LL245: {struct Cyc_Absyn_Offsetof_e_struct*_tmp4F5=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp4AA;if(_tmp4F5->tag != 20)goto _LL247;else{_tmp4F6=(void*)_tmp4F5->f1;_tmp4F7=(
void*)_tmp4F5->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp4F8=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp4F7;if(_tmp4F8->tag != 1)goto _LL247;else{_tmp4F9=_tmp4F8->f1;}};}}_LL246:{
void*_tmp66D=Cyc_Tcutil_compress(_tmp4F6);struct Cyc_Absyn_AggrInfo _tmp66F;union
Cyc_Absyn_AggrInfoU _tmp670;struct Cyc_List_List*_tmp672;_LL2F2: {struct Cyc_Absyn_AggrType_struct*
_tmp66E=(struct Cyc_Absyn_AggrType_struct*)_tmp66D;if(_tmp66E->tag != 12)goto
_LL2F4;else{_tmp66F=_tmp66E->f1;_tmp670=_tmp66F.aggr_info;}}_LL2F3: {struct Cyc_Absyn_Aggrdecl*
_tmp675=Cyc_Absyn_get_known_aggrdecl(_tmp670);if(_tmp675->impl == 0){const char*
_tmpC6D;void*_tmpC6C;(_tmpC6C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC6D="struct fields must be known",
_tag_dyneither(_tmpC6D,sizeof(char),28))),_tag_dyneither(_tmpC6C,sizeof(void*),0)));}
_tmp672=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp675->impl))->fields;goto
_LL2F5;}_LL2F4: {struct Cyc_Absyn_AnonAggrType_struct*_tmp671=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp66D;if(_tmp671->tag != 13)goto _LL2F6;else{_tmp672=_tmp671->f2;}}_LL2F5: {
struct Cyc_Absyn_Aggrfield*_tmp678=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp672,(int)_tmp4F9);{struct Cyc_Absyn_Offsetof_e_struct
_tmpC77;struct Cyc_Absyn_StructField_struct _tmpC76;struct Cyc_Absyn_StructField_struct*
_tmpC75;struct Cyc_Absyn_Offsetof_e_struct*_tmpC74;e->r=(void*)((_tmpC74=
_cycalloc(sizeof(*_tmpC74)),((_tmpC74[0]=((_tmpC77.tag=20,((_tmpC77.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4F6),((_tmpC77.f2=(void*)((void*)((_tmpC75=_cycalloc(
sizeof(*_tmpC75)),((_tmpC75[0]=((_tmpC76.tag=0,((_tmpC76.f1=_tmp678->name,
_tmpC76)))),_tmpC75))))),_tmpC77)))))),_tmpC74))));}goto _LL2F1;}_LL2F6: {struct
Cyc_Absyn_TupleType_struct*_tmp673=(struct Cyc_Absyn_TupleType_struct*)_tmp66D;
if(_tmp673->tag != 11)goto _LL2F8;}_LL2F7:{struct Cyc_Absyn_Offsetof_e_struct
_tmpC81;struct Cyc_Absyn_StructField_struct _tmpC80;struct Cyc_Absyn_StructField_struct*
_tmpC7F;struct Cyc_Absyn_Offsetof_e_struct*_tmpC7E;e->r=(void*)((_tmpC7E=
_cycalloc(sizeof(*_tmpC7E)),((_tmpC7E[0]=((_tmpC81.tag=20,((_tmpC81.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4F6),((_tmpC81.f2=(void*)((void*)((_tmpC7F=_cycalloc(
sizeof(*_tmpC7F)),((_tmpC7F[0]=((_tmpC80.tag=0,((_tmpC80.f1=Cyc_Absyn_fieldname((
int)(_tmp4F9 + 1)),_tmpC80)))),_tmpC7F))))),_tmpC81)))))),_tmpC7E))));}goto _LL2F1;
_LL2F8: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp674=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp66D;if(_tmp674->tag != 4)goto _LL2FA;}_LL2F9: if(_tmp4F9 == 0){struct Cyc_Absyn_Offsetof_e_struct
_tmpC8B;struct Cyc_Absyn_StructField_struct _tmpC8A;struct Cyc_Absyn_StructField_struct*
_tmpC89;struct Cyc_Absyn_Offsetof_e_struct*_tmpC88;e->r=(void*)((_tmpC88=
_cycalloc(sizeof(*_tmpC88)),((_tmpC88[0]=((_tmpC8B.tag=20,((_tmpC8B.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4F6),((_tmpC8B.f2=(void*)((void*)((_tmpC89=_cycalloc(
sizeof(*_tmpC89)),((_tmpC89[0]=((_tmpC8A.tag=0,((_tmpC8A.f1=Cyc_Toc_tag_sp,
_tmpC8A)))),_tmpC89))))),_tmpC8B)))))),_tmpC88))));}else{struct Cyc_Absyn_Offsetof_e_struct
_tmpC95;struct Cyc_Absyn_StructField_struct _tmpC94;struct Cyc_Absyn_StructField_struct*
_tmpC93;struct Cyc_Absyn_Offsetof_e_struct*_tmpC92;e->r=(void*)((_tmpC92=
_cycalloc(sizeof(*_tmpC92)),((_tmpC92[0]=((_tmpC95.tag=20,((_tmpC95.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4F6),((_tmpC95.f2=(void*)((void*)((_tmpC93=_cycalloc(
sizeof(*_tmpC93)),((_tmpC93[0]=((_tmpC94.tag=0,((_tmpC94.f1=Cyc_Absyn_fieldname((
int)_tmp4F9),_tmpC94)))),_tmpC93))))),_tmpC95)))))),_tmpC92))));}goto _LL2F1;
_LL2FA:;_LL2FB: {const char*_tmpC98;void*_tmpC97;(_tmpC97=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC98="impossible type for offsetof tuple index",
_tag_dyneither(_tmpC98,sizeof(char),41))),_tag_dyneither(_tmpC97,sizeof(void*),0)));}
_LL2F1:;}goto _LL216;_LL247: {struct Cyc_Absyn_Deref_e_struct*_tmp4FA=(struct Cyc_Absyn_Deref_e_struct*)
_tmp4AA;if(_tmp4FA->tag != 21)goto _LL249;else{_tmp4FB=_tmp4FA->f1;}}_LL248: {void*
_tmp68B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp4FB->topt))->v);{
void*_tmp68C=_tmp68B;struct Cyc_Absyn_PtrInfo _tmp68E;void*_tmp68F;struct Cyc_Absyn_Tqual
_tmp690;struct Cyc_Absyn_PtrAtts _tmp691;void*_tmp692;union Cyc_Absyn_Constraint*
_tmp693;union Cyc_Absyn_Constraint*_tmp694;union Cyc_Absyn_Constraint*_tmp695;
_LL2FD: {struct Cyc_Absyn_PointerType_struct*_tmp68D=(struct Cyc_Absyn_PointerType_struct*)
_tmp68C;if(_tmp68D->tag != 5)goto _LL2FF;else{_tmp68E=_tmp68D->f1;_tmp68F=_tmp68E.elt_typ;
_tmp690=_tmp68E.elt_tq;_tmp691=_tmp68E.ptr_atts;_tmp692=_tmp691.rgn;_tmp693=
_tmp691.nullable;_tmp694=_tmp691.bounds;_tmp695=_tmp691.zero_term;}}_LL2FE:{void*
_tmp696=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp694);struct Cyc_Absyn_Exp*_tmp698;_LL302: {struct Cyc_Absyn_Upper_b_struct*
_tmp697=(struct Cyc_Absyn_Upper_b_struct*)_tmp696;if(_tmp697->tag != 1)goto _LL304;
else{_tmp698=_tmp697->f1;}}_LL303: {int do_null_check=Cyc_Toc_need_null_check(
_tmp4FB);Cyc_Toc_exp_to_c(nv,_tmp4FB);if(do_null_check){if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC9B;void*_tmpC9A;(_tmpC9A=0,Cyc_Tcutil_warn(e->loc,((_tmpC9B="inserted null check due to dereference",
_tag_dyneither(_tmpC9B,sizeof(char),39))),_tag_dyneither(_tmpC9A,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC9C;_tmp4FB->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp68B),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpC9C=_cycalloc(sizeof(*
_tmpC9C)),((_tmpC9C->hd=Cyc_Absyn_copy_exp(_tmp4FB),((_tmpC9C->tl=0,_tmpC9C)))))),
0));};}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp695)){unsigned int _tmp69E;int _tmp69F;struct _tuple10 _tmp69D=Cyc_Evexp_eval_const_uint_exp(
_tmp698);_tmp69E=_tmp69D.f1;_tmp69F=_tmp69D.f2;if(!_tmp69F  || _tmp69E <= 0){const
char*_tmpC9F;void*_tmpC9E;(_tmpC9E=0,Cyc_Tcutil_terr(e->loc,((_tmpC9F="cannot determine dereference is in bounds",
_tag_dyneither(_tmpC9F,sizeof(char),42))),_tag_dyneither(_tmpC9E,sizeof(void*),0)));}}
goto _LL301;}_LL304: {struct Cyc_Absyn_DynEither_b_struct*_tmp699=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp696;if(_tmp699->tag != 0)goto _LL301;}_LL305: {struct Cyc_Absyn_Exp*_tmp6A2=Cyc_Absyn_uint_exp(
0,0);{struct Cyc_Core_Opt*_tmpCA0;_tmp6A2->topt=((_tmpCA0=_cycalloc(sizeof(*
_tmpCA0)),((_tmpCA0->v=Cyc_Absyn_uint_typ,_tmpCA0))));}e->r=Cyc_Toc_subscript_exp_r(
_tmp4FB,_tmp6A2);Cyc_Toc_exp_to_c(nv,e);goto _LL301;}_LL301:;}goto _LL2FC;_LL2FF:;
_LL300: {const char*_tmpCA3;void*_tmpCA2;(_tmpCA2=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA3="exp_to_c: Deref: non-pointer",
_tag_dyneither(_tmpCA3,sizeof(char),29))),_tag_dyneither(_tmpCA2,sizeof(void*),0)));}
_LL2FC:;}goto _LL216;}_LL249: {struct Cyc_Absyn_AggrMember_e_struct*_tmp4FC=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp4AA;if(_tmp4FC->tag != 22)goto _LL24B;
else{_tmp4FD=_tmp4FC->f1;_tmp4FE=_tmp4FC->f2;_tmp4FF=_tmp4FC->f3;_tmp500=_tmp4FC->f4;}}
_LL24A: {int is_poly=Cyc_Toc_is_poly_project(e);void*e1_cyc_type=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4FD->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp4FD);if(
_tmp4FF  && _tmp500)e->r=Cyc_Toc_check_tagged_union(_tmp4FD,Cyc_Toc_typ_to_c(
e1_cyc_type),e1_cyc_type,_tmp4FE,Cyc_Absyn_aggrmember_exp);if(is_poly)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL216;}_LL24B: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp501=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp4AA;if(_tmp501->tag != 23)goto _LL24D;
else{_tmp502=_tmp501->f1;_tmp503=_tmp501->f2;_tmp504=_tmp501->f3;_tmp505=_tmp501->f4;}}
_LL24C: {void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp502->topt))->v);int do_null_check=Cyc_Toc_need_null_check(_tmp502);Cyc_Toc_exp_to_c(
nv,_tmp502);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp6A7;struct Cyc_Absyn_Tqual
_tmp6A8;struct Cyc_Absyn_PtrAtts _tmp6A9;void*_tmp6AA;union Cyc_Absyn_Constraint*
_tmp6AB;union Cyc_Absyn_Constraint*_tmp6AC;union Cyc_Absyn_Constraint*_tmp6AD;
struct Cyc_Absyn_PtrInfo _tmp6A6=Cyc_Toc_get_ptr_type(e1typ);_tmp6A7=_tmp6A6.elt_typ;
_tmp6A8=_tmp6A6.elt_tq;_tmp6A9=_tmp6A6.ptr_atts;_tmp6AA=_tmp6A9.rgn;_tmp6AB=
_tmp6A9.nullable;_tmp6AC=_tmp6A9.bounds;_tmp6AD=_tmp6A9.zero_term;{void*_tmp6AE=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6AC);struct Cyc_Absyn_Exp*_tmp6B0;_LL307: {struct Cyc_Absyn_Upper_b_struct*
_tmp6AF=(struct Cyc_Absyn_Upper_b_struct*)_tmp6AE;if(_tmp6AF->tag != 1)goto _LL309;
else{_tmp6B0=_tmp6AF->f1;}}_LL308: {unsigned int _tmp6B3;int _tmp6B4;struct
_tuple10 _tmp6B2=Cyc_Evexp_eval_const_uint_exp(_tmp6B0);_tmp6B3=_tmp6B2.f1;
_tmp6B4=_tmp6B2.f2;if(_tmp6B4){if(_tmp6B3 < 1){const char*_tmpCA6;void*_tmpCA5;(
_tmpCA5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpCA6="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpCA6,
sizeof(char),44))),_tag_dyneither(_tmpCA5,sizeof(void*),0)));}if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCA9;void*_tmpCA8;(_tmpCA8=0,Cyc_Tcutil_warn(
e->loc,((_tmpCA9="inserted null check due to dereference",_tag_dyneither(_tmpCA9,
sizeof(char),39))),_tag_dyneither(_tmpCA8,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCAA[1];_tmp502->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpCAA[0]=Cyc_Absyn_new_exp(_tmp502->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAA,sizeof(struct Cyc_Absyn_Exp*),
1)))),0));};}}else{if(!Cyc_Evexp_c_can_eval(_tmp6B0)){const char*_tmpCAD;void*
_tmpCAC;(_tmpCAC=0,Cyc_Tcutil_terr(e->loc,((_tmpCAD="cannot determine pointer dereference in bounds",
_tag_dyneither(_tmpCAD,sizeof(char),47))),_tag_dyneither(_tmpCAC,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpCAE[4];_tmp502->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((_tmpCAE[3]=
Cyc_Absyn_uint_exp(0,0),((_tmpCAE[2]=Cyc_Absyn_sizeoftyp_exp(_tmp6A7,0),((
_tmpCAE[1]=_tmp6B0,((_tmpCAE[0]=Cyc_Absyn_new_exp(_tmp502->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAE,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0));};}goto _LL306;}_LL309: {struct Cyc_Absyn_DynEither_b_struct*
_tmp6B1=(struct Cyc_Absyn_DynEither_b_struct*)_tmp6AE;if(_tmp6B1->tag != 0)goto
_LL306;}_LL30A: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp6A7);{struct Cyc_Absyn_Exp*
_tmpCAF[3];_tmp502->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp6A8),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_dyneither_subscript_e,((_tmpCAF[2]=Cyc_Absyn_uint_exp(0,0),((
_tmpCAF[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCAF[0]=Cyc_Absyn_new_exp(_tmp502->r,
0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCAF,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}goto _LL306;}_LL306:;}if(
_tmp504  && _tmp505)e->r=Cyc_Toc_check_tagged_union(_tmp502,Cyc_Toc_typ_to_c(
e1typ),_tmp6A7,_tmp503,Cyc_Absyn_aggrarrow_exp);if(is_poly  && _tmp505)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL216;};}_LL24D: {struct Cyc_Absyn_Subscript_e_struct*_tmp506=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp4AA;if(_tmp506->tag != 24)goto _LL24F;
else{_tmp507=_tmp506->f1;_tmp508=_tmp506->f2;}}_LL24E: {void*_tmp6BE=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp507->topt))->v);{void*_tmp6BF=
_tmp6BE;struct Cyc_List_List*_tmp6C1;struct Cyc_Absyn_PtrInfo _tmp6C3;void*_tmp6C4;
struct Cyc_Absyn_Tqual _tmp6C5;struct Cyc_Absyn_PtrAtts _tmp6C6;void*_tmp6C7;union
Cyc_Absyn_Constraint*_tmp6C8;union Cyc_Absyn_Constraint*_tmp6C9;union Cyc_Absyn_Constraint*
_tmp6CA;_LL30C: {struct Cyc_Absyn_TupleType_struct*_tmp6C0=(struct Cyc_Absyn_TupleType_struct*)
_tmp6BF;if(_tmp6C0->tag != 11)goto _LL30E;else{_tmp6C1=_tmp6C0->f1;}}_LL30D: Cyc_Toc_exp_to_c(
nv,_tmp507);Cyc_Toc_exp_to_c(nv,_tmp508);{unsigned int _tmp6CC;int _tmp6CD;struct
_tuple10 _tmp6CB=Cyc_Evexp_eval_const_uint_exp(_tmp508);_tmp6CC=_tmp6CB.f1;
_tmp6CD=_tmp6CB.f2;if(!_tmp6CD){const char*_tmpCB2;void*_tmpCB1;(_tmpCB1=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCB2="unknown tuple subscript in translation to C",
_tag_dyneither(_tmpCB2,sizeof(char),44))),_tag_dyneither(_tmpCB1,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp507,Cyc_Absyn_fieldname((int)(_tmp6CC + 1)));
goto _LL30B;};_LL30E: {struct Cyc_Absyn_PointerType_struct*_tmp6C2=(struct Cyc_Absyn_PointerType_struct*)
_tmp6BF;if(_tmp6C2->tag != 5)goto _LL310;else{_tmp6C3=_tmp6C2->f1;_tmp6C4=_tmp6C3.elt_typ;
_tmp6C5=_tmp6C3.elt_tq;_tmp6C6=_tmp6C3.ptr_atts;_tmp6C7=_tmp6C6.rgn;_tmp6C8=
_tmp6C6.nullable;_tmp6C9=_tmp6C6.bounds;_tmp6CA=_tmp6C6.zero_term;}}_LL30F: {
struct Cyc_List_List*_tmp6D0=Cyc_Toc_get_relns(_tmp507);int in_bnds=0;{void*
_tmp6D1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6C9);_LL313:;_LL314: in_bnds=Cyc_Toc_check_bounds(_tmp6BE,_tmp6D0,_tmp507,
_tmp508);if(Cyc_Toc_warn_bounds_checks  && !in_bnds){const char*_tmpCB6;void*
_tmpCB5[1];struct Cyc_String_pa_struct _tmpCB4;(_tmpCB4.tag=0,((_tmpCB4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCB5[0]=&
_tmpCB4,Cyc_Tcutil_warn(e->loc,((_tmpCB6="bounds check necessary for %s",
_tag_dyneither(_tmpCB6,sizeof(char),30))),_tag_dyneither(_tmpCB5,sizeof(void*),1)))))));}
_LL312:;}Cyc_Toc_exp_to_c(nv,_tmp507);Cyc_Toc_exp_to_c(nv,_tmp508);++ Cyc_Toc_total_bounds_checks;{
void*_tmp6D5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp6C9);struct Cyc_Absyn_Exp*_tmp6D7;_LL316: {struct Cyc_Absyn_Upper_b_struct*
_tmp6D6=(struct Cyc_Absyn_Upper_b_struct*)_tmp6D5;if(_tmp6D6->tag != 1)goto _LL318;
else{_tmp6D7=_tmp6D6->f1;}}_LL317: {int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp6C8);void*ta1=Cyc_Toc_typ_to_c(_tmp6C4);void*ta2=
Cyc_Absyn_cstar_typ(ta1,_tmp6C5);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6CA)){if(!Cyc_Evexp_c_can_eval(_tmp6D7)){const char*_tmpCB9;void*_tmpCB8;(
_tmpCB8=0,Cyc_Tcutil_terr(e->loc,((_tmpCB9="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCB9,sizeof(char),40))),_tag_dyneither(_tmpCB8,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp507);struct Cyc_Absyn_Exp*_tmpCBA[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpCBA[2]=_tmp508,((_tmpCBA[1]=_tmp6D7,((
_tmpCBA[0]=_tmp507,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCBA,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp6D7)){const char*_tmpCBD;void*_tmpCBC;(
_tmpCBC=0,Cyc_Tcutil_terr(e->loc,((_tmpCBD="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCBD,sizeof(char),40))),_tag_dyneither(_tmpCBC,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCC0;void*_tmpCBF;(_tmpCBF=0,Cyc_Tcutil_warn(
e->loc,((_tmpCC0="inserted null check due to dereference",_tag_dyneither(_tmpCC0,
sizeof(char),39))),_tag_dyneither(_tmpCBF,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCC1[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((
_tmpCC1[3]=_tmp508,((_tmpCC1[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCC1[1]=
_tmp6D7,((_tmpCC1[0]=_tmp507,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCC1,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{if(
!Cyc_Evexp_c_can_eval(_tmp6D7)){const char*_tmpCC4;void*_tmpCC3;(_tmpCC3=0,Cyc_Tcutil_terr(
e->loc,((_tmpCC4="cannot determine subscript is in bounds",_tag_dyneither(
_tmpCC4,sizeof(char),40))),_tag_dyneither(_tmpCC3,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCC5[2];_tmp508->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((
_tmpCC5[1]=Cyc_Absyn_copy_exp(_tmp508),((_tmpCC5[0]=_tmp6D7,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCC5,sizeof(struct Cyc_Absyn_Exp*),
2)))))));};}}}goto _LL315;}_LL318: {struct Cyc_Absyn_DynEither_b_struct*_tmp6D8=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp6D5;if(_tmp6D8->tag != 0)goto _LL315;}
_LL319: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp6C4);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6C5),
Cyc_Absyn_aggrmember_exp(_tmp507,Cyc_Toc_curr_sp,0)),_tmp508);}else{struct Cyc_Absyn_Exp*
_tmp6E4=Cyc_Toc__check_dyneither_subscript_e;struct Cyc_Absyn_Exp*_tmpCC6[3];e->r=
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6C5),Cyc_Absyn_fncall_exp(
_tmp6E4,((_tmpCC6[2]=_tmp508,((_tmpCC6[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((
_tmpCC6[0]=_tmp507,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCC6,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL315;}
_LL315:;}goto _LL30B;}_LL310:;_LL311: {const char*_tmpCC9;void*_tmpCC8;(_tmpCC8=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCC9="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(
_tmpCC9,sizeof(char),49))),_tag_dyneither(_tmpCC8,sizeof(void*),0)));}_LL30B:;}
goto _LL216;}_LL24F: {struct Cyc_Absyn_Tuple_e_struct*_tmp509=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp4AA;if(_tmp509->tag != 25)goto _LL251;else{_tmp50A=_tmp509->f1;}}_LL250: if(!
Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp50A))->r;else{struct
Cyc_List_List*_tmp6E8=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp50A);void*_tmp6E9=Cyc_Toc_add_tuple_type(
_tmp6E8);struct Cyc_List_List*dles=0;{int i=1;for(0;_tmp50A != 0;(_tmp50A=_tmp50A->tl,
i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp50A->hd);{struct _tuple15*
_tmpCCC;struct Cyc_List_List*_tmpCCB;dles=((_tmpCCB=_cycalloc(sizeof(*_tmpCCB)),((
_tmpCCB->hd=((_tmpCCC=_cycalloc(sizeof(*_tmpCCC)),((_tmpCCC->f1=0,((_tmpCCC->f2=(
struct Cyc_Absyn_Exp*)_tmp50A->hd,_tmpCCC)))))),((_tmpCCB->tl=dles,_tmpCCB))))));};}}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);e->r=
Cyc_Toc_unresolvedmem_exp_r(0,dles);}goto _LL216;_LL251: {struct Cyc_Absyn_Array_e_struct*
_tmp50B=(struct Cyc_Absyn_Array_e_struct*)_tmp4AA;if(_tmp50B->tag != 27)goto _LL253;
else{_tmp50C=_tmp50B->f1;}}_LL252: e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp50C);{
struct Cyc_List_List*_tmp6EC=_tmp50C;for(0;_tmp6EC != 0;_tmp6EC=_tmp6EC->tl){
struct _tuple15 _tmp6EE;struct Cyc_Absyn_Exp*_tmp6EF;struct _tuple15*_tmp6ED=(struct
_tuple15*)_tmp6EC->hd;_tmp6EE=*_tmp6ED;_tmp6EF=_tmp6EE.f2;Cyc_Toc_exp_to_c(nv,
_tmp6EF);}}goto _LL216;_LL253: {struct Cyc_Absyn_Comprehension_e_struct*_tmp50D=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmp4AA;if(_tmp50D->tag != 28)goto _LL255;
else{_tmp50E=_tmp50D->f1;_tmp50F=_tmp50D->f2;_tmp510=_tmp50D->f3;_tmp511=_tmp50D->f4;}}
_LL254: {unsigned int _tmp6F1;int _tmp6F2;struct _tuple10 _tmp6F0=Cyc_Evexp_eval_const_uint_exp(
_tmp50F);_tmp6F1=_tmp6F0.f1;_tmp6F2=_tmp6F0.f2;{void*_tmp6F3=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp510->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp510);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp510)){if(!_tmp6F2){
const char*_tmpCCF;void*_tmpCCE;(_tmpCCE=0,Cyc_Tcutil_terr(_tmp50F->loc,((_tmpCCF="cannot determine value of constant",
_tag_dyneither(_tmpCCF,sizeof(char),35))),_tag_dyneither(_tmpCCE,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp6F1;++ i){struct _tuple15*_tmpCD2;struct Cyc_List_List*
_tmpCD1;es=((_tmpCD1=_cycalloc(sizeof(*_tmpCD1)),((_tmpCD1->hd=((_tmpCD2=
_cycalloc(sizeof(*_tmpCD2)),((_tmpCD2->f1=0,((_tmpCD2->f2=_tmp510,_tmpCD2)))))),((
_tmpCD1->tl=es,_tmpCD1))))));}}if(_tmp511){struct Cyc_Absyn_Exp*_tmp6F8=Cyc_Toc_cast_it(
_tmp6F3,Cyc_Absyn_uint_exp(0,0));struct _tuple15*_tmpCD5;struct Cyc_List_List*
_tmpCD4;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,((_tmpCD4=_cycalloc(sizeof(*_tmpCD4)),((_tmpCD4->hd=((
_tmpCD5=_cycalloc(sizeof(*_tmpCD5)),((_tmpCD5->f1=0,((_tmpCD5->f2=_tmp6F8,
_tmpCD5)))))),((_tmpCD4->tl=0,_tmpCD4)))))));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL216;};};}_LL255: {struct Cyc_Absyn_Aggregate_e_struct*_tmp512=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp4AA;if(_tmp512->tag != 29)goto _LL257;
else{_tmp513=_tmp512->f1;_tmp514=_tmp512->f2;_tmp515=_tmp512->f3;_tmp516=_tmp512->f4;}}
_LL256: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp514
!= 0,0,0,_tmp515,_tmp513))->r;else{if(_tmp516 == 0){const char*_tmpCD8;void*
_tmpCD7;(_tmpCD7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCD8="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpCD8,sizeof(
char),38))),_tag_dyneither(_tmpCD7,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp516;struct Cyc_Toc_Env _tmp6FE;struct
_RegionHandle*_tmp6FF;struct Cyc_Toc_Env*_tmp6FD=nv;_tmp6FE=*_tmp6FD;_tmp6FF=
_tmp6FE.rgn;{struct Cyc_List_List*_tmp700=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum 
Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp6FF,e->loc,_tmp515,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp516->impl))->tagged){struct
_tuple20 _tmp702;struct Cyc_Absyn_Aggrfield*_tmp703;struct Cyc_Absyn_Exp*_tmp704;
struct _tuple20*_tmp701=(struct _tuple20*)((struct Cyc_List_List*)_check_null(
_tmp700))->hd;_tmp702=*_tmp701;_tmp703=_tmp702.f1;_tmp704=_tmp702.f2;{void*
_tmp705=_tmp703->type;Cyc_Toc_exp_to_c(nv,_tmp704);if(Cyc_Toc_is_void_star_or_tvar(
_tmp705))_tmp704->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp704->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp516,_tmp703->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple15*_tmpCDD;struct _tuple15*_tmpCDC;struct _tuple15*_tmpCDB[2];
struct Cyc_List_List*_tmp706=(_tmpCDB[1]=((_tmpCDC=_cycalloc(sizeof(*_tmpCDC)),((
_tmpCDC->f1=0,((_tmpCDC->f2=_tmp704,_tmpCDC)))))),((_tmpCDB[0]=((_tmpCDD=
_cycalloc(sizeof(*_tmpCDD)),((_tmpCDD->f1=0,((_tmpCDD->f2=field_tag_exp,_tmpCDD)))))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCDB,sizeof(struct _tuple15*),2)))));struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(
0,_tmp706,0);struct Cyc_Absyn_FieldName_struct*_tmpCE3;struct Cyc_Absyn_FieldName_struct
_tmpCE2;void*_tmpCE1[1];struct Cyc_List_List*ds=(_tmpCE1[0]=(void*)((_tmpCE3=
_cycalloc(sizeof(*_tmpCE3)),((_tmpCE3[0]=((_tmpCE2.tag=1,((_tmpCE2.f1=_tmp703->name,
_tmpCE2)))),_tmpCE3)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCE1,sizeof(void*),1)));struct _tuple15*_tmpCE6;struct _tuple15*
_tmpCE5[1];struct Cyc_List_List*dles=(_tmpCE5[0]=((_tmpCE6=_cycalloc(sizeof(*
_tmpCE6)),((_tmpCE6->f1=ds,((_tmpCE6->f2=umem,_tmpCE6)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE5,sizeof(struct
_tuple15*),1)));e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{struct Cyc_List_List*
_tmp70F=0;struct Cyc_List_List*_tmp710=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp710 != 0;_tmp710=_tmp710->tl){struct Cyc_List_List*
_tmp711=_tmp700;for(0;_tmp711 != 0;_tmp711=_tmp711->tl){if((*((struct _tuple20*)
_tmp711->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp710->hd){struct _tuple20
_tmp713;struct Cyc_Absyn_Aggrfield*_tmp714;struct Cyc_Absyn_Exp*_tmp715;struct
_tuple20*_tmp712=(struct _tuple20*)_tmp711->hd;_tmp713=*_tmp712;_tmp714=_tmp713.f1;
_tmp715=_tmp713.f2;{void*_tmp716=_tmp714->type;Cyc_Toc_exp_to_c(nv,_tmp715);if(
Cyc_Toc_is_void_star_or_tvar(_tmp716))_tmp715->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp715->r,0));{struct _tuple15*_tmpCE9;struct Cyc_List_List*
_tmpCE8;_tmp70F=((_tmpCE8=_cycalloc(sizeof(*_tmpCE8)),((_tmpCE8->hd=((_tmpCE9=
_cycalloc(sizeof(*_tmpCE9)),((_tmpCE9->f1=0,((_tmpCE9->f2=_tmp715,_tmpCE9)))))),((
_tmpCE8->tl=_tmp70F,_tmpCE8))))));}break;};}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp70F));}};};}
goto _LL216;_LL257: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp517=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp4AA;if(_tmp517->tag != 30)goto _LL259;else{_tmp518=(void*)_tmp517->f1;_tmp519=
_tmp517->f2;}}_LL258: {struct Cyc_List_List*fs;{void*_tmp719=Cyc_Tcutil_compress(
_tmp518);struct Cyc_List_List*_tmp71B;_LL31B: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp71A=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp719;if(_tmp71A->tag != 13)goto
_LL31D;else{_tmp71B=_tmp71A->f2;}}_LL31C: fs=_tmp71B;goto _LL31A;_LL31D:;_LL31E: {
const char*_tmpCED;void*_tmpCEC[1];struct Cyc_String_pa_struct _tmpCEB;(_tmpCEB.tag=
0,((_tmpCEB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp518)),((_tmpCEC[0]=& _tmpCEB,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCED="anon struct has type %s",
_tag_dyneither(_tmpCED,sizeof(char),24))),_tag_dyneither(_tmpCEC,sizeof(void*),1)))))));}
_LL31A:;}{struct Cyc_Toc_Env _tmp720;struct _RegionHandle*_tmp721;struct Cyc_Toc_Env*
_tmp71F=nv;_tmp720=*_tmp71F;_tmp721=_tmp720.rgn;{struct Cyc_List_List*_tmp722=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp721,e->loc,_tmp519,Cyc_Absyn_StructA,fs);for(0;_tmp722 != 0;_tmp722=_tmp722->tl){
struct _tuple20 _tmp724;struct Cyc_Absyn_Aggrfield*_tmp725;struct Cyc_Absyn_Exp*
_tmp726;struct _tuple20*_tmp723=(struct _tuple20*)_tmp722->hd;_tmp724=*_tmp723;
_tmp725=_tmp724.f1;_tmp726=_tmp724.f2;{void*_tmp727=_tmp725->type;Cyc_Toc_exp_to_c(
nv,_tmp726);if(Cyc_Toc_is_void_star_or_tvar(_tmp727))_tmp726->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp726->r,0));};}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp519);}goto _LL216;};}_LL259: {struct Cyc_Absyn_Datatype_e_struct*_tmp51A=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp4AA;if(_tmp51A->tag != 31)goto _LL25B;else{
_tmp51B=_tmp51A->f1;_tmp51C=_tmp51A->f2;_tmp51D=_tmp51A->f3;}}_LL25A: {void*
datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple0*_tmp728=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp729=Cyc_Absyn_var_exp(_tmp728,0);struct Cyc_Absyn_Exp*
member_exp;{const char*_tmpCEE;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp51D->name,((_tmpCEE="_struct",_tag_dyneither(_tmpCEE,sizeof(char),8)))));}
tag_exp=_tmp51C->is_extensible?Cyc_Absyn_var_exp(_tmp51D->name,0): Cyc_Toc_datatype_tag(
_tmp51C,_tmp51D->name);member_exp=_tmp729;{struct Cyc_List_List*_tmp72B=_tmp51D->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp51B != 0;(
_tmp51B=_tmp51B->tl,_tmp72B=_tmp72B->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp51B->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp72B))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct _tuple15*_tmpCF1;struct
Cyc_List_List*_tmpCF0;dles=((_tmpCF0=_cycalloc(sizeof(*_tmpCF0)),((_tmpCF0->hd=((
_tmpCF1=_cycalloc(sizeof(*_tmpCF1)),((_tmpCF1->f1=0,((_tmpCF1->f2=cur_e,_tmpCF1)))))),((
_tmpCF0->tl=dles,_tmpCF0))))));};}{struct _tuple15*_tmpCF4;struct Cyc_List_List*
_tmpCF3;dles=((_tmpCF3=_cycalloc(sizeof(*_tmpCF3)),((_tmpCF3->hd=((_tmpCF4=
_cycalloc(sizeof(*_tmpCF4)),((_tmpCF4->f1=0,((_tmpCF4->f2=tag_exp,_tmpCF4)))))),((
_tmpCF3->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles),_tmpCF3))))));}e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{struct Cyc_List_List*
_tmpCF5;struct Cyc_List_List*_tmp730=(_tmpCF5=_cycalloc(sizeof(*_tmpCF5)),((
_tmpCF5->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,
0),tag_exp,0),((_tmpCF5->tl=0,_tmpCF5)))));{int i=1;for(0;_tmp51B != 0;(((_tmp51B=
_tmp51B->tl,i ++)),_tmp72B=_tmp72B->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp51B->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp72B))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp731=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpCF6;_tmp730=((_tmpCF6=_cycalloc(sizeof(*
_tmpCF6)),((_tmpCF6->hd=_tmp731,((_tmpCF6->tl=_tmp730,_tmpCF6))))));};}}{struct
Cyc_Absyn_Stmt*_tmp733=Cyc_Absyn_exp_stmt(_tmp729,0);struct Cyc_List_List*_tmpCF7;
struct Cyc_Absyn_Stmt*_tmp734=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpCF7=_cycalloc(sizeof(*_tmpCF7)),((
_tmpCF7->hd=_tmp733,((_tmpCF7->tl=_tmp730,_tmpCF7))))))),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp728,datatype_ctype,0,_tmp734,0));};}goto _LL216;};}
_LL25B: {struct Cyc_Absyn_Enum_e_struct*_tmp51E=(struct Cyc_Absyn_Enum_e_struct*)
_tmp4AA;if(_tmp51E->tag != 32)goto _LL25D;}_LL25C: goto _LL25E;_LL25D: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp51F=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp4AA;if(_tmp51F->tag != 33)goto
_LL25F;}_LL25E: goto _LL216;_LL25F: {struct Cyc_Absyn_Malloc_e_struct*_tmp520=(
struct Cyc_Absyn_Malloc_e_struct*)_tmp4AA;if(_tmp520->tag != 34)goto _LL261;else{
_tmp521=_tmp520->f1;_tmp522=_tmp521.is_calloc;_tmp523=_tmp521.rgn;_tmp524=
_tmp521.elt_type;_tmp525=_tmp521.num_elts;_tmp526=_tmp521.fat_result;}}_LL260: {
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp524)));Cyc_Toc_exp_to_c(nv,
_tmp525);if(_tmp526){struct _tuple0*_tmp737=Cyc_Toc_temp_var();struct _tuple0*
_tmp738=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp522){xexp=_tmp525;if(_tmp523 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp523;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp737,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp524,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp737,0));}{struct Cyc_Absyn_Exp*_tmpCF8[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpCF8[2]=Cyc_Absyn_var_exp(_tmp737,0),((_tmpCF8[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpCF8[0]=Cyc_Absyn_var_exp(_tmp738,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCF8,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{xexp=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp525,0);if(_tmp523 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp523;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp737,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp524,Cyc_Absyn_var_exp(_tmp737,0));}{struct Cyc_Absyn_Exp*_tmpCF9[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpCF9[2]=Cyc_Absyn_var_exp(_tmp737,0),((_tmpCF9[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpCF9[0]=Cyc_Absyn_var_exp(_tmp738,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCF9,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);};}{struct Cyc_Absyn_Stmt*_tmp73B=Cyc_Absyn_declare_stmt(_tmp737,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp738,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp73B);};}else{struct Cyc_Absyn_Exp*_tmp73C=Cyc_Absyn_sizeoftyp_exp(
t_c,0);{void*_tmp73D=_tmp525->r;union Cyc_Absyn_Cnst _tmp73F;struct _tuple5 _tmp740;
int _tmp741;_LL320: {struct Cyc_Absyn_Const_e_struct*_tmp73E=(struct Cyc_Absyn_Const_e_struct*)
_tmp73D;if(_tmp73E->tag != 0)goto _LL322;else{_tmp73F=_tmp73E->f1;if((_tmp73F.Int_c).tag
!= 4)goto _LL322;_tmp740=(struct _tuple5)(_tmp73F.Int_c).val;_tmp741=_tmp740.f2;
if(_tmp741 != 1)goto _LL322;}}_LL321: goto _LL31F;_LL322:;_LL323: _tmp73C=Cyc_Absyn_times_exp(
_tmp73C,_tmp525,0);goto _LL31F;_LL31F:;}if(_tmp523 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp523;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rmalloc_exp(rgn,_tmp73C))->r;}else{e->r=(Cyc_Toc_malloc_exp(*_tmp524,
_tmp73C))->r;}}goto _LL216;}_LL261: {struct Cyc_Absyn_Swap_e_struct*_tmp527=(
struct Cyc_Absyn_Swap_e_struct*)_tmp4AA;if(_tmp527->tag != 35)goto _LL263;else{
_tmp528=_tmp527->f1;_tmp529=_tmp527->f2;}}_LL262: {int is_dyneither_ptr=0;void*
e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp528->topt))->v;void*
e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp529->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(
e1_old_typ,& is_dyneither_ptr)){const char*_tmpCFC;void*_tmpCFB;(_tmpCFB=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCFC="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpCFC,sizeof(char),57))),_tag_dyneither(_tmpCFB,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}{int f1_tag=0;void*tagged_mem_type1=(void*)&
Cyc_Absyn_VoidType_val;int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp528,&
f1_tag,& tagged_mem_type1,1);int f2_tag=0;void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp529,& f2_tag,& tagged_mem_type2,1);
Cyc_Toc_exp_to_c(nv,_tmp528);Cyc_Toc_exp_to_c(nv,_tmp529);if(e1_tmem)Cyc_Toc_add_tagged_union_check_for_swap(
_tmp528,f1_tag,tagged_mem_type1);else{_tmp528=Cyc_Toc_push_address_exp(_tmp528);}
if(e2_tmem)Cyc_Toc_add_tagged_union_check_for_swap(_tmp529,f2_tag,
tagged_mem_type2);else{_tmp529=Cyc_Toc_push_address_exp(_tmp529);}{struct Cyc_Absyn_Exp*
_tmpCFD[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpCFD[1]=_tmp529,((_tmpCFD[0]=
_tmp528,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCFD,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL216;};};}
_LL263: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp52A=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp4AA;if(_tmp52A->tag != 38)goto _LL265;else{_tmp52B=_tmp52A->f1;_tmp52C=_tmp52A->f2;}}
_LL264: {void*_tmp745=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp52B->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp52B);{void*_tmp746=
_tmp745;struct Cyc_Absyn_AggrInfo _tmp748;union Cyc_Absyn_AggrInfoU _tmp749;struct
Cyc_Absyn_Aggrdecl**_tmp74A;struct Cyc_Absyn_Aggrdecl*_tmp74B;_LL325: {struct Cyc_Absyn_AggrType_struct*
_tmp747=(struct Cyc_Absyn_AggrType_struct*)_tmp746;if(_tmp747->tag != 12)goto
_LL327;else{_tmp748=_tmp747->f1;_tmp749=_tmp748.aggr_info;if((_tmp749.KnownAggr).tag
!= 2)goto _LL327;_tmp74A=(struct Cyc_Absyn_Aggrdecl**)(_tmp749.KnownAggr).val;
_tmp74B=*_tmp74A;}}_LL326: {struct Cyc_Absyn_Exp*_tmp74C=Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp74B,_tmp52C),0);struct Cyc_Absyn_Exp*_tmp74D=Cyc_Absyn_aggrmember_exp(
_tmp52B,_tmp52C,0);struct Cyc_Absyn_Exp*_tmp74E=Cyc_Absyn_aggrmember_exp(_tmp74D,
Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(_tmp74E,_tmp74C,0))->r;goto _LL324;}
_LL327:;_LL328: {const char*_tmpD00;void*_tmpCFF;(_tmpCFF=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD00="non-aggregate type in tagcheck",
_tag_dyneither(_tmpD00,sizeof(char),31))),_tag_dyneither(_tmpCFF,sizeof(void*),0)));}
_LL324:;}goto _LL216;}_LL265: {struct Cyc_Absyn_StmtExp_e_struct*_tmp52D=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp4AA;if(_tmp52D->tag != 37)goto _LL267;else{_tmp52E=
_tmp52D->f1;}}_LL266: Cyc_Toc_stmt_to_c(nv,_tmp52E);goto _LL216;_LL267: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp52F=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp4AA;if(_tmp52F->tag != 36)
goto _LL269;}_LL268: {const char*_tmpD03;void*_tmpD02;(_tmpD02=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD03="UnresolvedMem",
_tag_dyneither(_tmpD03,sizeof(char),14))),_tag_dyneither(_tmpD02,sizeof(void*),0)));}
_LL269: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp530=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp4AA;if(_tmp530->tag != 26)goto _LL26B;}_LL26A: {const char*_tmpD06;void*_tmpD05;(
_tmpD05=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpD06="compoundlit",_tag_dyneither(_tmpD06,sizeof(char),12))),_tag_dyneither(
_tmpD05,sizeof(void*),0)));}_LL26B: {struct Cyc_Absyn_Valueof_e_struct*_tmp531=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp4AA;if(_tmp531->tag != 39)goto _LL26D;}
_LL26C: {const char*_tmpD09;void*_tmpD08;(_tmpD08=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD09="valueof(-)",
_tag_dyneither(_tmpD09,sizeof(char),11))),_tag_dyneither(_tmpD08,sizeof(void*),0)));}
_LL26D: {struct Cyc_Absyn_Asm_e_struct*_tmp532=(struct Cyc_Absyn_Asm_e_struct*)
_tmp4AA;if(_tmp532->tag != 40)goto _LL216;}_LL26E: {const char*_tmpD0C;void*_tmpD0B;(
_tmpD0B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD0C="__asm__",_tag_dyneither(_tmpD0C,sizeof(char),8))),_tag_dyneither(
_tmpD0B,sizeof(void*),0)));}_LL216:;};}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0);}struct _tuple21{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*
f3;};struct _tuple22{struct _tuple0*f1;void*f2;};struct _tuple23{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple21 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*
nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
struct Cyc_Absyn_Stmt*s;{void*_tmp759=p->r;struct Cyc_Absyn_Vardecl*_tmp75B;struct
Cyc_Absyn_Vardecl _tmp75C;struct _tuple0*_tmp75D;struct Cyc_Absyn_Pat*_tmp75E;
struct Cyc_Absyn_Vardecl*_tmp760;struct Cyc_Absyn_Vardecl _tmp761;struct _tuple0*
_tmp762;struct Cyc_Absyn_Vardecl*_tmp765;struct Cyc_Absyn_Pat*_tmp766;enum Cyc_Absyn_Sign
_tmp769;int _tmp76A;char _tmp76C;struct _dyneither_ptr _tmp76E;struct Cyc_Absyn_Enumdecl*
_tmp770;struct Cyc_Absyn_Enumfield*_tmp771;void*_tmp773;struct Cyc_Absyn_Enumfield*
_tmp774;struct Cyc_Absyn_Pat*_tmp776;struct Cyc_Absyn_Pat _tmp777;void*_tmp778;
struct Cyc_Absyn_Datatypedecl*_tmp77A;struct Cyc_Absyn_Datatypefield*_tmp77B;
struct Cyc_List_List*_tmp77C;struct Cyc_List_List*_tmp77E;struct Cyc_List_List*
_tmp780;struct Cyc_Absyn_AggrInfo*_tmp782;struct Cyc_Absyn_AggrInfo*_tmp784;struct
Cyc_Absyn_AggrInfo _tmp785;union Cyc_Absyn_AggrInfoU _tmp786;struct Cyc_List_List*
_tmp787;struct Cyc_Absyn_Pat*_tmp789;_LL32A: {struct Cyc_Absyn_Var_p_struct*
_tmp75A=(struct Cyc_Absyn_Var_p_struct*)_tmp759;if(_tmp75A->tag != 1)goto _LL32C;
else{_tmp75B=_tmp75A->f1;_tmp75C=*_tmp75B;_tmp75D=_tmp75C.name;_tmp75E=_tmp75A->f2;}}
_LL32B: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp75D,r),rgn,t,r,path,
_tmp75E,fail_stmt,decls);_LL32C: {struct Cyc_Absyn_TagInt_p_struct*_tmp75F=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp759;if(_tmp75F->tag != 3)goto _LL32E;else{
_tmp760=_tmp75F->f2;_tmp761=*_tmp760;_tmp762=_tmp761.name;}}_LL32D: nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp762,r);goto _LL32F;_LL32E: {struct Cyc_Absyn_Wild_p_struct*_tmp763=(
struct Cyc_Absyn_Wild_p_struct*)_tmp759;if(_tmp763->tag != 0)goto _LL330;}_LL32F: s=
Cyc_Toc_skip_stmt_dl();goto _LL329;_LL330: {struct Cyc_Absyn_Reference_p_struct*
_tmp764=(struct Cyc_Absyn_Reference_p_struct*)_tmp759;if(_tmp764->tag != 2)goto
_LL332;else{_tmp765=_tmp764->f1;_tmp766=_tmp764->f2;}}_LL331: {struct _tuple0*
_tmp78D=Cyc_Toc_temp_var();{struct _tuple22*_tmpD0F;struct Cyc_List_List*_tmpD0E;
decls=((_tmpD0E=_region_malloc(rgn,sizeof(*_tmpD0E)),((_tmpD0E->hd=((_tmpD0F=
_region_malloc(rgn,sizeof(*_tmpD0F)),((_tmpD0F->f1=_tmp78D,((_tmpD0F->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpD0F)))))),((_tmpD0E->tl=decls,_tmpD0E))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp765->name,Cyc_Absyn_var_exp(_tmp78D,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp78D,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Toc_push_address_exp(path)),0);{struct _tuple21 _tmp790=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp766,fail_stmt,decls);_tmp790.f3=Cyc_Absyn_seq_stmt(s,_tmp790.f3,
0);return _tmp790;};}_LL332: {struct Cyc_Absyn_Null_p_struct*_tmp767=(struct Cyc_Absyn_Null_p_struct*)
_tmp759;if(_tmp767->tag != 8)goto _LL334;}_LL333: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),fail_stmt);goto _LL329;_LL334: {struct Cyc_Absyn_Int_p_struct*_tmp768=(struct
Cyc_Absyn_Int_p_struct*)_tmp759;if(_tmp768->tag != 9)goto _LL336;else{_tmp769=
_tmp768->f1;_tmp76A=_tmp768->f2;}}_LL335: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp769,_tmp76A,0),fail_stmt);goto _LL329;_LL336: {struct Cyc_Absyn_Char_p_struct*
_tmp76B=(struct Cyc_Absyn_Char_p_struct*)_tmp759;if(_tmp76B->tag != 10)goto _LL338;
else{_tmp76C=_tmp76B->f1;}}_LL337: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(
_tmp76C,0),fail_stmt);goto _LL329;_LL338: {struct Cyc_Absyn_Float_p_struct*_tmp76D=(
struct Cyc_Absyn_Float_p_struct*)_tmp759;if(_tmp76D->tag != 11)goto _LL33A;else{
_tmp76E=_tmp76D->f1;}}_LL339: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp76E,
0),fail_stmt);goto _LL329;_LL33A: {struct Cyc_Absyn_Enum_p_struct*_tmp76F=(struct
Cyc_Absyn_Enum_p_struct*)_tmp759;if(_tmp76F->tag != 12)goto _LL33C;else{_tmp770=
_tmp76F->f1;_tmp771=_tmp76F->f2;}}_LL33B:{struct Cyc_Absyn_Enum_e_struct _tmpD12;
struct Cyc_Absyn_Enum_e_struct*_tmpD11;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD11=_cycalloc(sizeof(*_tmpD11)),((_tmpD11[0]=((_tmpD12.tag=32,((
_tmpD12.f1=_tmp771->name,((_tmpD12.f2=(struct Cyc_Absyn_Enumdecl*)_tmp770,((
_tmpD12.f3=(struct Cyc_Absyn_Enumfield*)_tmp771,_tmpD12)))))))),_tmpD11)))),0),
fail_stmt);}goto _LL329;_LL33C: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp772=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp759;if(_tmp772->tag != 13)goto _LL33E;else{
_tmp773=(void*)_tmp772->f1;_tmp774=_tmp772->f2;}}_LL33D:{struct Cyc_Absyn_AnonEnum_e_struct
_tmpD15;struct Cyc_Absyn_AnonEnum_e_struct*_tmpD14;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD14=_cycalloc(sizeof(*_tmpD14)),((_tmpD14[0]=((_tmpD15.tag=33,((
_tmpD15.f1=_tmp774->name,((_tmpD15.f2=(void*)_tmp773,((_tmpD15.f3=(struct Cyc_Absyn_Enumfield*)
_tmp774,_tmpD15)))))))),_tmpD14)))),0),fail_stmt);}goto _LL329;_LL33E: {struct Cyc_Absyn_Pointer_p_struct*
_tmp775=(struct Cyc_Absyn_Pointer_p_struct*)_tmp759;if(_tmp775->tag != 5)goto
_LL340;else{_tmp776=_tmp775->f1;_tmp777=*_tmp776;_tmp778=_tmp777.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp779=(struct Cyc_Absyn_Datatype_p_struct*)_tmp778;if(_tmp779->tag != 7)goto
_LL340;else{_tmp77A=_tmp779->f1;_tmp77B=_tmp779->f2;_tmp77C=_tmp779->f3;}};}}
_LL33F: s=Cyc_Toc_skip_stmt_dl();{struct _tuple0*_tmp795=Cyc_Toc_temp_var();const
char*_tmpD16;struct _tuple0*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp77B->name,((
_tmpD16="_struct",_tag_dyneither(_tmpD16,sizeof(char),8))));void*_tmp796=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);int cnt=1;struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
_tmp796,r);struct Cyc_List_List*_tmp797=_tmp77B->typs;for(0;_tmp77C != 0;(((
_tmp77C=_tmp77C->tl,_tmp797=((struct Cyc_List_List*)_check_null(_tmp797))->tl)),
++ cnt)){struct Cyc_Absyn_Pat*_tmp798=(struct Cyc_Absyn_Pat*)_tmp77C->hd;if(_tmp798->r
== (void*)& Cyc_Absyn_Wild_p_val)continue;{void*_tmp799=(*((struct _tuple9*)((
struct Cyc_List_List*)_check_null(_tmp797))->hd)).f2;struct _tuple0*_tmp79A=Cyc_Toc_temp_var();
void*_tmp79B=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp798->topt))->v;void*
_tmp79C=Cyc_Toc_typ_to_c(_tmp79B);struct Cyc_Absyn_Exp*_tmp79D=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp795,0),Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_tvar(
Cyc_Toc_typ_to_c(_tmp799)))_tmp79D=Cyc_Toc_cast_it(_tmp79C,_tmp79D);{struct
_tuple22*_tmpD19;struct Cyc_List_List*_tmpD18;decls=((_tmpD18=_region_malloc(rgn,
sizeof(*_tmpD18)),((_tmpD18->hd=((_tmpD19=_region_malloc(rgn,sizeof(*_tmpD19)),((
_tmpD19->f1=_tmp79A,((_tmpD19->f2=_tmp79C,_tmpD19)))))),((_tmpD18->tl=decls,
_tmpD18))))));}{struct _tuple21 _tmp7A0=Cyc_Toc_xlate_pat(nv,rgn,_tmp79B,Cyc_Absyn_var_exp(
_tmp79A,0),_tmp79D,_tmp798,fail_stmt,decls);nv=_tmp7A0.f1;decls=_tmp7A0.f2;{
struct Cyc_Absyn_Stmt*_tmp7A1=_tmp7A0.f3;struct Cyc_Absyn_Stmt*_tmp7A2=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp79A,0),_tmp79D,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp7A2,_tmp7A1,0),0);};};};}{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp795,0),Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*tag_exp=
_tmp77A->is_extensible?Cyc_Absyn_var_exp(_tmp77B->name,0): Cyc_Toc_datatype_tag(
_tmp77A,_tmp77B->name);struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,
tag_exp,0);s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);if(Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v))s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp795,0),Cyc_Absyn_uint_exp(0,0),0),
fail_stmt,s,0);s=Cyc_Absyn_declare_stmt(_tmp795,_tmp796,(struct Cyc_Absyn_Exp*)
rcast,s,0);goto _LL329;};};_LL340: {struct Cyc_Absyn_Datatype_p_struct*_tmp77D=(
struct Cyc_Absyn_Datatype_p_struct*)_tmp759;if(_tmp77D->tag != 7)goto _LL342;else{
_tmp77E=_tmp77D->f3;}}_LL341: _tmp780=_tmp77E;goto _LL343;_LL342: {struct Cyc_Absyn_Tuple_p_struct*
_tmp77F=(struct Cyc_Absyn_Tuple_p_struct*)_tmp759;if(_tmp77F->tag != 4)goto _LL344;
else{_tmp780=_tmp77F->f1;}}_LL343: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;
_tmp780 != 0;(_tmp780=_tmp780->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp7A4=(struct Cyc_Absyn_Pat*)
_tmp780->hd;if(_tmp7A4->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{struct _tuple0*
_tmp7A5=Cyc_Toc_temp_var();void*_tmp7A6=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7A4->topt))->v;{struct _tuple22*_tmpD1C;struct Cyc_List_List*_tmpD1B;decls=((
_tmpD1B=_region_malloc(rgn,sizeof(*_tmpD1B)),((_tmpD1B->hd=((_tmpD1C=
_region_malloc(rgn,sizeof(*_tmpD1C)),((_tmpD1C->f1=_tmp7A5,((_tmpD1C->f2=Cyc_Toc_typ_to_c(
_tmp7A6),_tmpD1C)))))),((_tmpD1B->tl=decls,_tmpD1B))))));}{struct _tuple21 _tmp7A9=
Cyc_Toc_xlate_pat(nv,rgn,_tmp7A6,Cyc_Absyn_var_exp(_tmp7A5,0),Cyc_Absyn_aggrmember_exp(
path,Cyc_Absyn_fieldname(cnt),0),_tmp7A4,fail_stmt,decls);nv=_tmp7A9.f1;decls=
_tmp7A9.f2;{struct Cyc_Absyn_Stmt*_tmp7AA=_tmp7A9.f3;struct Cyc_Absyn_Stmt*_tmp7AB=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7A5,0),Cyc_Absyn_aggrmember_exp(r,
Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7AB,
_tmp7AA,0),0);};};};}goto _LL329;};_LL344: {struct Cyc_Absyn_Aggr_p_struct*_tmp781=(
struct Cyc_Absyn_Aggr_p_struct*)_tmp759;if(_tmp781->tag != 6)goto _LL346;else{
_tmp782=_tmp781->f1;if(_tmp782 != 0)goto _LL346;}}_LL345: {const char*_tmpD1F;void*
_tmpD1E;(_tmpD1E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD1F="unresolved aggregate pattern!",_tag_dyneither(_tmpD1F,sizeof(char),30))),
_tag_dyneither(_tmpD1E,sizeof(void*),0)));}_LL346: {struct Cyc_Absyn_Aggr_p_struct*
_tmp783=(struct Cyc_Absyn_Aggr_p_struct*)_tmp759;if(_tmp783->tag != 6)goto _LL348;
else{_tmp784=_tmp783->f1;if(_tmp784 == 0)goto _LL348;_tmp785=*_tmp784;_tmp786=
_tmp785.aggr_info;_tmp787=_tmp783->f3;}}_LL347: {struct Cyc_Absyn_Aggrdecl*
_tmp7AE=Cyc_Absyn_get_known_aggrdecl(_tmp786);if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp7AE->impl))->tagged){struct _tuple23 _tmp7B0;struct Cyc_List_List*
_tmp7B1;struct Cyc_Absyn_Pat*_tmp7B2;struct _tuple23*_tmp7AF=(struct _tuple23*)((
struct Cyc_List_List*)_check_null(_tmp787))->hd;_tmp7B0=*_tmp7AF;_tmp7B1=_tmp7B0.f1;
_tmp7B2=_tmp7B0.f2;{struct _dyneither_ptr*f;{void*_tmp7B3=(void*)((struct Cyc_List_List*)
_check_null(_tmp7B1))->hd;struct _dyneither_ptr*_tmp7B5;_LL351: {struct Cyc_Absyn_FieldName_struct*
_tmp7B4=(struct Cyc_Absyn_FieldName_struct*)_tmp7B3;if(_tmp7B4->tag != 1)goto
_LL353;else{_tmp7B5=_tmp7B4->f1;}}_LL352: f=_tmp7B5;goto _LL350;_LL353:;_LL354: {
const char*_tmpD22;void*_tmpD21;(_tmpD21=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD22="no field name in tagged union pattern",
_tag_dyneither(_tmpD22,sizeof(char),38))),_tag_dyneither(_tmpD21,sizeof(void*),0)));}
_LL350:;}{struct _tuple0*_tmp7B8=Cyc_Toc_temp_var();void*_tmp7B9=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp7B2->topt))->v;void*_tmp7BA=Cyc_Toc_typ_to_c(
_tmp7B9);{struct _tuple22*_tmpD25;struct Cyc_List_List*_tmpD24;decls=((_tmpD24=
_region_malloc(rgn,sizeof(*_tmpD24)),((_tmpD24->hd=((_tmpD25=_region_malloc(rgn,
sizeof(*_tmpD25)),((_tmpD25->f1=_tmp7B8,((_tmpD25->f2=_tmp7BA,_tmpD25)))))),((
_tmpD24->tl=decls,_tmpD24))))));}{struct Cyc_Absyn_Exp*_tmp7BD=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp7BE=
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp7BE=Cyc_Toc_cast_it(_tmp7BA,_tmp7BE);_tmp7BD=Cyc_Toc_cast_it(_tmp7BA,_tmp7BD);{
struct _tuple21 _tmp7BF=Cyc_Toc_xlate_pat(nv,rgn,_tmp7B9,Cyc_Absyn_var_exp(_tmp7B8,
0),_tmp7BD,_tmp7B2,fail_stmt,decls);nv=_tmp7BF.f1;decls=_tmp7BF.f2;{struct Cyc_Absyn_Stmt*
_tmp7C0=_tmp7BF.f3;struct Cyc_Absyn_Stmt*_tmp7C1=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp7AE,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp7C2=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp7B8,0),_tmp7BE,0);s=Cyc_Absyn_seq_stmt(_tmp7C1,Cyc_Absyn_seq_stmt(_tmp7C2,
_tmp7C0,0),0);};};};};};}else{s=Cyc_Toc_skip_stmt_dl();for(0;_tmp787 != 0;_tmp787=
_tmp787->tl){struct _tuple23*_tmp7C3=(struct _tuple23*)_tmp787->hd;struct Cyc_Absyn_Pat*
_tmp7C4=(*_tmp7C3).f2;if(_tmp7C4->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _dyneither_ptr*f;{void*_tmp7C5=(void*)((struct Cyc_List_List*)_check_null((*
_tmp7C3).f1))->hd;struct _dyneither_ptr*_tmp7C7;_LL356: {struct Cyc_Absyn_FieldName_struct*
_tmp7C6=(struct Cyc_Absyn_FieldName_struct*)_tmp7C5;if(_tmp7C6->tag != 1)goto
_LL358;else{_tmp7C7=_tmp7C6->f1;}}_LL357: f=_tmp7C7;goto _LL355;_LL358:;_LL359: {
struct Cyc_Toc_Match_error_struct _tmpD28;struct Cyc_Toc_Match_error_struct*_tmpD27;(
int)_throw((void*)((_tmpD27=_cycalloc_atomic(sizeof(*_tmpD27)),((_tmpD27[0]=((
_tmpD28.tag=Cyc_Toc_Match_error,_tmpD28)),_tmpD27)))));}_LL355:;}{struct _tuple0*
_tmp7CA=Cyc_Toc_temp_var();void*_tmp7CB=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7C4->topt))->v;void*_tmp7CC=Cyc_Toc_typ_to_c(_tmp7CB);{struct _tuple22*
_tmpD2B;struct Cyc_List_List*_tmpD2A;decls=((_tmpD2A=_region_malloc(rgn,sizeof(*
_tmpD2A)),((_tmpD2A->hd=((_tmpD2B=_region_malloc(rgn,sizeof(*_tmpD2B)),((_tmpD2B->f1=
_tmp7CA,((_tmpD2B->f2=_tmp7CC,_tmpD2B)))))),((_tmpD2A->tl=decls,_tmpD2A))))));}{
struct _tuple21 _tmp7CF=Cyc_Toc_xlate_pat(nv,rgn,_tmp7CB,Cyc_Absyn_var_exp(_tmp7CA,
0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp7C4,fail_stmt,decls);nv=_tmp7CF.f1;
decls=_tmp7CF.f2;{struct Cyc_Absyn_Exp*_tmp7D0=Cyc_Absyn_aggrmember_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7AE->impl))->fields,f)))->type))
_tmp7D0=Cyc_Toc_cast_it(_tmp7CC,_tmp7D0);{struct Cyc_Absyn_Stmt*_tmp7D1=_tmp7CF.f3;
struct Cyc_Absyn_Stmt*_tmp7D2=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7CA,0),
_tmp7D0,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7D2,_tmp7D1,0),0);};};};};};}}
goto _LL329;}_LL348: {struct Cyc_Absyn_Pointer_p_struct*_tmp788=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp759;if(_tmp788->tag != 5)goto _LL34A;else{_tmp789=_tmp788->f1;}}_LL349: {
struct _tuple0*_tmp7D3=Cyc_Toc_temp_var();void*_tmp7D4=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp789->topt))->v;{struct _tuple22*_tmpD2E;struct Cyc_List_List*
_tmpD2D;decls=((_tmpD2D=_region_malloc(rgn,sizeof(*_tmpD2D)),((_tmpD2D->hd=((
_tmpD2E=_region_malloc(rgn,sizeof(*_tmpD2E)),((_tmpD2E->f1=_tmp7D3,((_tmpD2E->f2=
Cyc_Toc_typ_to_c(_tmp7D4),_tmpD2E)))))),((_tmpD2D->tl=decls,_tmpD2D))))));}{
struct _tuple21 _tmp7D7=Cyc_Toc_xlate_pat(nv,rgn,_tmp7D4,Cyc_Absyn_var_exp(_tmp7D3,
0),Cyc_Absyn_deref_exp(path,0),_tmp789,fail_stmt,decls);nv=_tmp7D7.f1;decls=
_tmp7D7.f2;{struct Cyc_Absyn_Stmt*_tmp7D8=_tmp7D7.f3;struct Cyc_Absyn_Stmt*_tmp7D9=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7D3,0),Cyc_Absyn_deref_exp(
r,0),0),_tmp7D8,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp7D9,0);else{s=_tmp7D9;}goto _LL329;};};}_LL34A: {struct Cyc_Absyn_UnknownId_p_struct*
_tmp78A=(struct Cyc_Absyn_UnknownId_p_struct*)_tmp759;if(_tmp78A->tag != 14)goto
_LL34C;}_LL34B: {const char*_tmpD31;void*_tmpD30;(_tmpD30=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD31="unknownid pat",
_tag_dyneither(_tmpD31,sizeof(char),14))),_tag_dyneither(_tmpD30,sizeof(void*),0)));}
_LL34C: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp78B=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp759;if(_tmp78B->tag != 15)goto _LL34E;}_LL34D: {const char*_tmpD34;void*_tmpD33;(
_tmpD33=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD34="unknowncall pat",_tag_dyneither(_tmpD34,sizeof(char),16))),
_tag_dyneither(_tmpD33,sizeof(void*),0)));}_LL34E: {struct Cyc_Absyn_Exp_p_struct*
_tmp78C=(struct Cyc_Absyn_Exp_p_struct*)_tmp759;if(_tmp78C->tag != 16)goto _LL329;}
_LL34F: {const char*_tmpD37;void*_tmpD36;(_tmpD36=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD37="exp pat",
_tag_dyneither(_tmpD37,sizeof(char),8))),_tag_dyneither(_tmpD36,sizeof(void*),0)));}
_LL329:;}{struct _tuple21 _tmpD38;return(_tmpD38.f1=nv,((_tmpD38.f2=decls,((
_tmpD38.f3=s,_tmpD38)))));};}struct _tuple24{struct _dyneither_ptr*f1;struct
_dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){struct _tuple24*_tmpD39;
return(_tmpD39=_region_malloc(r,sizeof(*_tmpD39)),((_tmpD39->f1=Cyc_Toc_fresh_label(),((
_tmpD39->f2=Cyc_Toc_fresh_label(),((_tmpD39->f3=sc,_tmpD39)))))));}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*_tmp7E2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;int leave_as_switch;{void*_tmp7E3=Cyc_Tcutil_compress(
_tmp7E2);_LL35B: {struct Cyc_Absyn_IntType_struct*_tmp7E4=(struct Cyc_Absyn_IntType_struct*)
_tmp7E3;if(_tmp7E4->tag != 6)goto _LL35D;}_LL35C: goto _LL35E;_LL35D: {struct Cyc_Absyn_EnumType_struct*
_tmp7E5=(struct Cyc_Absyn_EnumType_struct*)_tmp7E3;if(_tmp7E5->tag != 14)goto
_LL35F;}_LL35E: leave_as_switch=1;goto _LL35A;_LL35F:;_LL360: leave_as_switch=0;
goto _LL35A;_LL35A:;}{struct Cyc_List_List*_tmp7E6=scs;for(0;_tmp7E6 != 0;_tmp7E6=
_tmp7E6->tl){if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Switch_clause*)_tmp7E6->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)
_tmp7E6->hd)->where_clause != 0){leave_as_switch=0;break;}}}if(leave_as_switch){
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{struct Cyc_List_List*_tmp7E7=
scs;for(0;_tmp7E7 != 0;_tmp7E7=_tmp7E7->tl){struct Cyc_Absyn_Stmt*_tmp7E8=((struct
Cyc_Absyn_Switch_clause*)_tmp7E7->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp7E7->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp7E8,0);next_l=Cyc_Toc_fresh_label();{
struct Cyc_Toc_Env _tmp7EA;struct _RegionHandle*_tmp7EB;struct Cyc_Toc_Env*_tmp7E9=
nv;_tmp7EA=*_tmp7E9;_tmp7EB=_tmp7EA.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
_tmp7EB,nv,next_l),_tmp7E8);};}}return;}{struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp7ED;
struct _RegionHandle*_tmp7EE;struct Cyc_Toc_Env*_tmp7EC=nv;_tmp7ED=*_tmp7EC;
_tmp7EE=_tmp7ED.rgn;{struct Cyc_Toc_Env*_tmp7EF=Cyc_Toc_share_env(_tmp7EE,nv);
struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple24*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp7EE,Cyc_Toc_gen_label,
_tmp7EE,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct
Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp7F0=lscs;for(0;_tmp7F0 != 0;
_tmp7F0=_tmp7F0->tl){struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple24*)
_tmp7F0->hd)).f3;struct _dyneither_ptr*fail_lab=_tmp7F0->tl == 0?end_l:(*((struct
_tuple24*)((struct Cyc_List_List*)_check_null(_tmp7F0->tl))->hd)).f1;struct Cyc_Toc_Env*
_tmp7F2;struct Cyc_List_List*_tmp7F3;struct Cyc_Absyn_Stmt*_tmp7F4;struct _tuple21
_tmp7F1=Cyc_Toc_xlate_pat(_tmp7EF,_tmp7EE,_tmp7E2,r,path,sc->pattern,Cyc_Absyn_goto_stmt(
fail_lab,0),decls);_tmp7F2=_tmp7F1.f1;_tmp7F3=_tmp7F1.f2;_tmp7F4=_tmp7F1.f3;if(
sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp7F5=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp7F2,_tmp7F5);_tmp7F4=Cyc_Absyn_seq_stmt(
_tmp7F4,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp7F5,0),
Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp7F3;{
struct Cyc_List_List*_tmpD3A;nvs=((_tmpD3A=_region_malloc(_tmp7EE,sizeof(*_tmpD3A)),((
_tmpD3A->hd=_tmp7F2,((_tmpD3A->tl=nvs,_tmpD3A))))));}{struct Cyc_List_List*
_tmpD3B;test_stmts=((_tmpD3B=_region_malloc(_tmp7EE,sizeof(*_tmpD3B)),((_tmpD3B->hd=
_tmp7F4,((_tmpD3B->tl=test_stmts,_tmpD3B))))));};}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple24 _tmp7F9;struct _dyneither_ptr*_tmp7FA;struct _dyneither_ptr*_tmp7FB;
struct Cyc_Absyn_Switch_clause*_tmp7FC;struct _tuple24*_tmp7F8=(struct _tuple24*)
lscs->hd;_tmp7F9=*_tmp7F8;_tmp7FA=_tmp7F9.f1;_tmp7FB=_tmp7F9.f2;_tmp7FC=_tmp7F9.f3;{
struct Cyc_Toc_Env*_tmp7FD=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp7FC->body;struct Cyc_Toc_Env _tmp7FF;struct
_RegionHandle*_tmp800;struct Cyc_Toc_Env*_tmp7FE=_tmp7EF;_tmp7FF=*_tmp7FE;_tmp800=
_tmp7FF.rgn;if(lscs->tl != 0){struct _tuple24 _tmp802;struct _dyneither_ptr*_tmp803;
struct Cyc_Absyn_Switch_clause*_tmp804;struct _tuple24*_tmp801=(struct _tuple24*)((
struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp802=*_tmp801;_tmp803=_tmp802.f2;
_tmp804=_tmp802.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp800,
_tmp7FD,end_l,_tmp803,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp804->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp800,
_tmp7FD,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp7FA,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp7FB,s,0),0);{struct Cyc_List_List*_tmpD3C;stmts=((_tmpD3C=_region_malloc(
_tmp7EE,sizeof(*_tmpD3C)),((_tmpD3C->hd=s,((_tmpD3C->tl=stmts,_tmpD3C))))));};};}{
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,
Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple22
_tmp807;struct _tuple0*_tmp808;void*_tmp809;struct _tuple22*_tmp806=(struct
_tuple22*)decls->hd;_tmp807=*_tmp806;_tmp808=_tmp807.f1;_tmp809=_tmp807.f2;res=
Cyc_Absyn_declare_stmt(_tmp808,_tmp809,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(
v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(
struct Cyc_Absyn_Exp*)e,res,0))->r;};};};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){struct Cyc_List_List*
_tmpD3D;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,((
_tmpD3D=_cycalloc(sizeof(*_tmpD3D)),((_tmpD3D->hd=Cyc_Absyn_uint_exp((
unsigned int)(n - 1),0),((_tmpD3D->tl=0,_tmpD3D)))))),0),0);}void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s){while(1){void*_tmp80B=s->r;struct Cyc_Absyn_Exp*_tmp80E;
struct Cyc_Absyn_Stmt*_tmp810;struct Cyc_Absyn_Stmt*_tmp811;struct Cyc_Absyn_Exp*
_tmp813;struct Cyc_Absyn_Exp*_tmp815;struct Cyc_Absyn_Stmt*_tmp816;struct Cyc_Absyn_Stmt*
_tmp817;struct _tuple8 _tmp819;struct Cyc_Absyn_Exp*_tmp81A;struct Cyc_Absyn_Stmt*
_tmp81B;struct Cyc_Absyn_Stmt*_tmp81D;struct Cyc_Absyn_Stmt*_tmp81F;struct Cyc_Absyn_Stmt*
_tmp821;struct Cyc_Absyn_Exp*_tmp823;struct _tuple8 _tmp824;struct Cyc_Absyn_Exp*
_tmp825;struct _tuple8 _tmp826;struct Cyc_Absyn_Exp*_tmp827;struct Cyc_Absyn_Stmt*
_tmp828;struct Cyc_Absyn_Exp*_tmp82A;struct Cyc_List_List*_tmp82B;struct Cyc_List_List*
_tmp82D;struct Cyc_Absyn_Switch_clause**_tmp82E;struct Cyc_Absyn_Decl*_tmp830;
struct Cyc_Absyn_Stmt*_tmp831;struct _dyneither_ptr*_tmp833;struct Cyc_Absyn_Stmt*
_tmp834;struct Cyc_Absyn_Stmt*_tmp836;struct _tuple8 _tmp837;struct Cyc_Absyn_Exp*
_tmp838;struct Cyc_Absyn_Stmt*_tmp83A;struct Cyc_List_List*_tmp83B;struct Cyc_Absyn_Exp*
_tmp83D;_LL362: {struct Cyc_Absyn_Skip_s_struct*_tmp80C=(struct Cyc_Absyn_Skip_s_struct*)
_tmp80B;if(_tmp80C->tag != 0)goto _LL364;}_LL363: return;_LL364: {struct Cyc_Absyn_Exp_s_struct*
_tmp80D=(struct Cyc_Absyn_Exp_s_struct*)_tmp80B;if(_tmp80D->tag != 1)goto _LL366;
else{_tmp80E=_tmp80D->f1;}}_LL365: Cyc_Toc_exp_to_c(nv,_tmp80E);return;_LL366: {
struct Cyc_Absyn_Seq_s_struct*_tmp80F=(struct Cyc_Absyn_Seq_s_struct*)_tmp80B;if(
_tmp80F->tag != 2)goto _LL368;else{_tmp810=_tmp80F->f1;_tmp811=_tmp80F->f2;}}
_LL367: Cyc_Toc_stmt_to_c(nv,_tmp810);s=_tmp811;continue;_LL368: {struct Cyc_Absyn_Return_s_struct*
_tmp812=(struct Cyc_Absyn_Return_s_struct*)_tmp80B;if(_tmp812->tag != 3)goto _LL36A;
else{_tmp813=_tmp812->f1;}}_LL369: {struct Cyc_Core_Opt*topt=0;if(_tmp813 != 0){{
struct Cyc_Core_Opt*_tmpD3E;topt=((_tmpD3E=_cycalloc(sizeof(*_tmpD3E)),((_tmpD3E->v=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp813->topt))->v),
_tmpD3E))));}Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp813);}if(s->try_depth
> 0){if(topt != 0){struct _tuple0*_tmp83F=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*
_tmp840=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp83F,0),
0);s->r=(Cyc_Absyn_declare_stmt(_tmp83F,(void*)topt->v,_tmp813,Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler(s->try_depth),_tmp840,0),0))->r;}else{Cyc_Toc_do_npop_before(
s->try_depth,s);}}return;}_LL36A: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp814=(
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp80B;if(_tmp814->tag != 4)goto _LL36C;
else{_tmp815=_tmp814->f1;_tmp816=_tmp814->f2;_tmp817=_tmp814->f3;}}_LL36B: Cyc_Toc_exp_to_c(
nv,_tmp815);Cyc_Toc_stmt_to_c(nv,_tmp816);s=_tmp817;continue;_LL36C: {struct Cyc_Absyn_While_s_struct*
_tmp818=(struct Cyc_Absyn_While_s_struct*)_tmp80B;if(_tmp818->tag != 5)goto _LL36E;
else{_tmp819=_tmp818->f1;_tmp81A=_tmp819.f1;_tmp81B=_tmp818->f2;}}_LL36D: Cyc_Toc_exp_to_c(
nv,_tmp81A);{struct Cyc_Toc_Env _tmp842;struct _RegionHandle*_tmp843;struct Cyc_Toc_Env*
_tmp841=nv;_tmp842=*_tmp841;_tmp843=_tmp842.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp843,nv),_tmp81B);return;};_LL36E: {struct Cyc_Absyn_Break_s_struct*_tmp81C=(
struct Cyc_Absyn_Break_s_struct*)_tmp80B;if(_tmp81C->tag != 6)goto _LL370;else{
_tmp81D=_tmp81C->f1;}}_LL36F: {struct Cyc_Toc_Env _tmp845;struct _dyneither_ptr**
_tmp846;struct Cyc_Toc_Env*_tmp844=nv;_tmp845=*_tmp844;_tmp846=_tmp845.break_lab;
if(_tmp846 != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp846,0);{int dest_depth=_tmp81D == 0?0:
_tmp81D->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;};}
_LL370: {struct Cyc_Absyn_Continue_s_struct*_tmp81E=(struct Cyc_Absyn_Continue_s_struct*)
_tmp80B;if(_tmp81E->tag != 7)goto _LL372;else{_tmp81F=_tmp81E->f1;}}_LL371: {
struct Cyc_Toc_Env _tmp848;struct _dyneither_ptr**_tmp849;struct Cyc_Toc_Env*_tmp847=
nv;_tmp848=*_tmp847;_tmp849=_tmp848.continue_lab;if(_tmp849 != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp849,0);_tmp821=_tmp81F;goto _LL373;}_LL372: {struct Cyc_Absyn_Goto_s_struct*
_tmp820=(struct Cyc_Absyn_Goto_s_struct*)_tmp80B;if(_tmp820->tag != 8)goto _LL374;
else{_tmp821=_tmp820->f2;}}_LL373: Cyc_Toc_do_npop_before(s->try_depth - ((struct
Cyc_Absyn_Stmt*)_check_null(_tmp821))->try_depth,s);return;_LL374: {struct Cyc_Absyn_For_s_struct*
_tmp822=(struct Cyc_Absyn_For_s_struct*)_tmp80B;if(_tmp822->tag != 9)goto _LL376;
else{_tmp823=_tmp822->f1;_tmp824=_tmp822->f2;_tmp825=_tmp824.f1;_tmp826=_tmp822->f3;
_tmp827=_tmp826.f1;_tmp828=_tmp822->f4;}}_LL375: Cyc_Toc_exp_to_c(nv,_tmp823);Cyc_Toc_exp_to_c(
nv,_tmp825);Cyc_Toc_exp_to_c(nv,_tmp827);{struct Cyc_Toc_Env _tmp84B;struct
_RegionHandle*_tmp84C;struct Cyc_Toc_Env*_tmp84A=nv;_tmp84B=*_tmp84A;_tmp84C=
_tmp84B.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp84C,nv),_tmp828);return;};
_LL376: {struct Cyc_Absyn_Switch_s_struct*_tmp829=(struct Cyc_Absyn_Switch_s_struct*)
_tmp80B;if(_tmp829->tag != 10)goto _LL378;else{_tmp82A=_tmp829->f1;_tmp82B=_tmp829->f2;}}
_LL377: Cyc_Toc_xlate_switch(nv,s,_tmp82A,_tmp82B);return;_LL378: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp82C=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp80B;if(_tmp82C->tag != 11)goto
_LL37A;else{_tmp82D=_tmp82C->f1;_tmp82E=_tmp82C->f2;}}_LL379: {struct Cyc_Toc_Env
_tmp84E;struct Cyc_Toc_FallthruInfo*_tmp84F;struct Cyc_Toc_Env*_tmp84D=nv;_tmp84E=*
_tmp84D;_tmp84F=_tmp84E.fallthru_info;if(_tmp84F == 0){const char*_tmpD41;void*
_tmpD40;(_tmpD40=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD41="fallthru in unexpected place",_tag_dyneither(_tmpD41,sizeof(char),29))),
_tag_dyneither(_tmpD40,sizeof(void*),0)));}{struct _dyneither_ptr*_tmp853;struct
Cyc_List_List*_tmp854;struct Cyc_Dict_Dict _tmp855;struct Cyc_Toc_FallthruInfo
_tmp852=*_tmp84F;_tmp853=_tmp852.label;_tmp854=_tmp852.binders;_tmp855=_tmp852.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp853,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp82E)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp856=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp854);struct Cyc_List_List*_tmp857=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp82D);for(0;_tmp856 != 0;(_tmp856=_tmp856->tl,
_tmp857=_tmp857->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp857))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp855,(
struct _tuple0*)_tmp856->hd),(struct Cyc_Absyn_Exp*)_tmp857->hd,0),s2,0);}s->r=s2->r;
return;};};};}_LL37A: {struct Cyc_Absyn_Decl_s_struct*_tmp82F=(struct Cyc_Absyn_Decl_s_struct*)
_tmp80B;if(_tmp82F->tag != 12)goto _LL37C;else{_tmp830=_tmp82F->f1;_tmp831=_tmp82F->f2;}}
_LL37B:{void*_tmp858=_tmp830->r;struct Cyc_Absyn_Vardecl*_tmp85A;struct Cyc_Absyn_Pat*
_tmp85C;struct Cyc_Absyn_Exp*_tmp85D;struct Cyc_List_List*_tmp85F;struct Cyc_Absyn_Fndecl*
_tmp861;struct Cyc_Absyn_Tvar*_tmp863;struct Cyc_Absyn_Vardecl*_tmp864;int _tmp865;
struct Cyc_Absyn_Exp*_tmp866;struct Cyc_Absyn_Exp*_tmp868;struct Cyc_Absyn_Tvar*
_tmp869;struct Cyc_Absyn_Vardecl*_tmp86A;_LL385: {struct Cyc_Absyn_Var_d_struct*
_tmp859=(struct Cyc_Absyn_Var_d_struct*)_tmp858;if(_tmp859->tag != 0)goto _LL387;
else{_tmp85A=_tmp859->f1;}}_LL386: {struct Cyc_Toc_Env _tmp86C;struct _RegionHandle*
_tmp86D;struct Cyc_Toc_Env*_tmp86B=nv;_tmp86C=*_tmp86B;_tmp86D=_tmp86C.rgn;{
struct Cyc_Absyn_Local_b_struct _tmpD44;struct Cyc_Absyn_Local_b_struct*_tmpD43;
struct Cyc_Toc_Env*_tmp86E=Cyc_Toc_add_varmap(_tmp86D,nv,_tmp85A->name,Cyc_Absyn_varb_exp(
_tmp85A->name,(void*)((_tmpD43=_cycalloc(sizeof(*_tmpD43)),((_tmpD43[0]=((
_tmpD44.tag=4,((_tmpD44.f1=_tmp85A,_tmpD44)))),_tmpD43)))),0));Cyc_Toc_local_decl_to_c(
_tmp86E,_tmp86E,_tmp85A,_tmp831);}goto _LL384;}_LL387: {struct Cyc_Absyn_Let_d_struct*
_tmp85B=(struct Cyc_Absyn_Let_d_struct*)_tmp858;if(_tmp85B->tag != 2)goto _LL389;
else{_tmp85C=_tmp85B->f1;_tmp85D=_tmp85B->f3;}}_LL388:{void*_tmp871=_tmp85C->r;
struct Cyc_Absyn_Vardecl*_tmp873;struct Cyc_Absyn_Pat*_tmp874;struct Cyc_Absyn_Pat
_tmp875;void*_tmp876;_LL394: {struct Cyc_Absyn_Var_p_struct*_tmp872=(struct Cyc_Absyn_Var_p_struct*)
_tmp871;if(_tmp872->tag != 1)goto _LL396;else{_tmp873=_tmp872->f1;_tmp874=_tmp872->f2;
_tmp875=*_tmp874;_tmp876=_tmp875.r;{struct Cyc_Absyn_Wild_p_struct*_tmp877=(
struct Cyc_Absyn_Wild_p_struct*)_tmp876;if(_tmp877->tag != 0)goto _LL396;};}}_LL395: {
struct _tuple0*old_name=_tmp873->name;struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp873->name=new_name;_tmp873->initializer=(struct Cyc_Absyn_Exp*)_tmp85D;{
struct Cyc_Absyn_Var_d_struct _tmpD47;struct Cyc_Absyn_Var_d_struct*_tmpD46;_tmp830->r=(
void*)((_tmpD46=_cycalloc(sizeof(*_tmpD46)),((_tmpD46[0]=((_tmpD47.tag=0,((
_tmpD47.f1=_tmp873,_tmpD47)))),_tmpD46))));}{struct Cyc_Toc_Env _tmp87B;struct
_RegionHandle*_tmp87C;struct Cyc_Toc_Env*_tmp87A=nv;_tmp87B=*_tmp87A;_tmp87C=
_tmp87B.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD4A;struct Cyc_Absyn_Local_b_struct*
_tmpD49;struct Cyc_Toc_Env*_tmp87D=Cyc_Toc_add_varmap(_tmp87C,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD49=_cycalloc(sizeof(*_tmpD49)),((_tmpD49[0]=((_tmpD4A.tag=
4,((_tmpD4A.f1=_tmp873,_tmpD4A)))),_tmpD49)))),0));Cyc_Toc_local_decl_to_c(
_tmp87D,nv,_tmp873,_tmp831);}goto _LL393;};}_LL396:;_LL397: s->r=(Cyc_Toc_letdecl_to_c(
nv,_tmp85C,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp85D->topt))->v,_tmp85D,
_tmp831))->r;goto _LL393;_LL393:;}goto _LL384;_LL389: {struct Cyc_Absyn_Letv_d_struct*
_tmp85E=(struct Cyc_Absyn_Letv_d_struct*)_tmp858;if(_tmp85E->tag != 3)goto _LL38B;
else{_tmp85F=_tmp85E->f1;}}_LL38A: {struct Cyc_List_List*_tmp880=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp85F);if(_tmp880 == 0){const char*_tmpD4D;
void*_tmpD4C;(_tmpD4C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpD4D="empty Letv_d",_tag_dyneither(_tmpD4D,sizeof(char),
13))),_tag_dyneither(_tmpD4C,sizeof(void*),0)));}{struct Cyc_Absyn_Var_d_struct
_tmpD50;struct Cyc_Absyn_Var_d_struct*_tmpD4F;_tmp830->r=(void*)((_tmpD4F=
_cycalloc(sizeof(*_tmpD4F)),((_tmpD4F[0]=((_tmpD50.tag=0,((_tmpD50.f1=(struct Cyc_Absyn_Vardecl*)
_tmp880->hd,_tmpD50)))),_tmpD4F))));}_tmp880=_tmp880->tl;for(0;_tmp880 != 0;
_tmp880=_tmp880->tl){struct Cyc_Absyn_Var_d_struct _tmpD53;struct Cyc_Absyn_Var_d_struct*
_tmpD52;struct Cyc_Absyn_Decl*_tmp885=Cyc_Absyn_new_decl((void*)((_tmpD52=
_cycalloc(sizeof(*_tmpD52)),((_tmpD52[0]=((_tmpD53.tag=0,((_tmpD53.f1=(struct Cyc_Absyn_Vardecl*)
_tmp880->hd,_tmpD53)))),_tmpD52)))),0);s->r=(Cyc_Absyn_decl_stmt(_tmp885,Cyc_Absyn_new_stmt(
s->r,0),0))->r;}Cyc_Toc_stmt_to_c(nv,s);goto _LL384;}_LL38B: {struct Cyc_Absyn_Fn_d_struct*
_tmp860=(struct Cyc_Absyn_Fn_d_struct*)_tmp858;if(_tmp860->tag != 1)goto _LL38D;
else{_tmp861=_tmp860->f1;}}_LL38C: {struct _tuple0*_tmp888=_tmp861->name;struct
Cyc_Toc_Env _tmp88A;struct _RegionHandle*_tmp88B;struct Cyc_Toc_Env*_tmp889=nv;
_tmp88A=*_tmp889;_tmp88B=_tmp88A.rgn;{struct Cyc_Toc_Env*_tmp88C=Cyc_Toc_add_varmap(
_tmp88B,nv,_tmp861->name,Cyc_Absyn_var_exp(_tmp888,0));Cyc_Toc_fndecl_to_c(
_tmp88C,_tmp861,0);Cyc_Toc_stmt_to_c(_tmp88C,_tmp831);}goto _LL384;}_LL38D: {
struct Cyc_Absyn_Region_d_struct*_tmp862=(struct Cyc_Absyn_Region_d_struct*)
_tmp858;if(_tmp862->tag != 4)goto _LL38F;else{_tmp863=_tmp862->f1;_tmp864=_tmp862->f2;
_tmp865=_tmp862->f3;_tmp866=_tmp862->f4;}}_LL38E: {struct Cyc_Absyn_Stmt*_tmp88D=
_tmp831;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple0*
rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp864->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp88F;struct _RegionHandle*_tmp890;struct Cyc_Toc_Env*
_tmp88E=nv;_tmp88F=*_tmp88E;_tmp890=_tmp88F.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp890,nv,x_var,x_exp),_tmp88D);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp88D,0))->r;
else{if(_tmp866 == 0){struct Cyc_Absyn_Exp*_tmpD56[1];struct Cyc_Absyn_Exp*_tmpD55[
1];struct Cyc_List_List*_tmpD54;s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpD54=
_cycalloc(sizeof(*_tmpD54)),((_tmpD54->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0),((_tmpD54->tl=0,_tmpD54)))))),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD55[0]=
x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD55,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp88D,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD56[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD56,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp866);{struct Cyc_Absyn_Exp*
_tmpD58[1];struct Cyc_Absyn_Exp*_tmpD57[2];s->r=(Cyc_Absyn_declare_stmt(rh_var,
Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpD57[1]=(struct Cyc_Absyn_Exp*)_tmp866,((_tmpD57[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD57,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp88D,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpD58[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD58,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
return;}_LL38F: {struct Cyc_Absyn_Alias_d_struct*_tmp867=(struct Cyc_Absyn_Alias_d_struct*)
_tmp858;if(_tmp867->tag != 5)goto _LL391;else{_tmp868=_tmp867->f1;_tmp869=_tmp867->f2;
_tmp86A=_tmp867->f3;}}_LL390: {struct _tuple0*old_name=_tmp86A->name;struct
_tuple0*new_name=Cyc_Toc_temp_var();_tmp86A->name=new_name;_tmp86A->initializer=(
struct Cyc_Absyn_Exp*)_tmp868;{struct Cyc_Absyn_Decl_s_struct _tmpD67;struct Cyc_Absyn_Var_d_struct*
_tmpD66;struct Cyc_Absyn_Var_d_struct _tmpD65;struct Cyc_Absyn_Decl*_tmpD64;struct
Cyc_Absyn_Decl_s_struct*_tmpD63;s->r=(void*)((_tmpD63=_cycalloc(sizeof(*_tmpD63)),((
_tmpD63[0]=((_tmpD67.tag=12,((_tmpD67.f1=((_tmpD64=_cycalloc(sizeof(*_tmpD64)),((
_tmpD64->r=(void*)((_tmpD66=_cycalloc(sizeof(*_tmpD66)),((_tmpD66[0]=((_tmpD65.tag=
0,((_tmpD65.f1=_tmp86A,_tmpD65)))),_tmpD66)))),((_tmpD64->loc=0,_tmpD64)))))),((
_tmpD67.f2=_tmp831,_tmpD67)))))),_tmpD63))));}{struct Cyc_Toc_Env _tmp89C;struct
_RegionHandle*_tmp89D;struct Cyc_Toc_Env*_tmp89B=nv;_tmp89C=*_tmp89B;_tmp89D=
_tmp89C.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD6A;struct Cyc_Absyn_Local_b_struct*
_tmpD69;struct Cyc_Toc_Env*_tmp89E=Cyc_Toc_add_varmap(_tmp89D,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD69=_cycalloc(sizeof(*_tmpD69)),((_tmpD69[0]=((_tmpD6A.tag=
4,((_tmpD6A.f1=_tmp86A,_tmpD6A)))),_tmpD69)))),0));Cyc_Toc_local_decl_to_c(
_tmp89E,nv,_tmp86A,_tmp831);}return;};}_LL391:;_LL392: {const char*_tmpD6D;void*
_tmpD6C;(_tmpD6C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD6D="bad nested declaration within function",_tag_dyneither(_tmpD6D,sizeof(
char),39))),_tag_dyneither(_tmpD6C,sizeof(void*),0)));}_LL384:;}return;_LL37C: {
struct Cyc_Absyn_Label_s_struct*_tmp832=(struct Cyc_Absyn_Label_s_struct*)_tmp80B;
if(_tmp832->tag != 13)goto _LL37E;else{_tmp833=_tmp832->f1;_tmp834=_tmp832->f2;}}
_LL37D: s=_tmp834;continue;_LL37E: {struct Cyc_Absyn_Do_s_struct*_tmp835=(struct
Cyc_Absyn_Do_s_struct*)_tmp80B;if(_tmp835->tag != 14)goto _LL380;else{_tmp836=
_tmp835->f1;_tmp837=_tmp835->f2;_tmp838=_tmp837.f1;}}_LL37F: {struct Cyc_Toc_Env
_tmp8A4;struct _RegionHandle*_tmp8A5;struct Cyc_Toc_Env*_tmp8A3=nv;_tmp8A4=*
_tmp8A3;_tmp8A5=_tmp8A4.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp8A5,nv),
_tmp836);Cyc_Toc_exp_to_c(nv,_tmp838);return;}_LL380: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp839=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp80B;if(_tmp839->tag != 15)goto
_LL382;else{_tmp83A=_tmp839->f1;_tmp83B=_tmp839->f2;}}_LL381: {struct _tuple0*
h_var=Cyc_Toc_temp_var();struct _tuple0*e_var=Cyc_Toc_temp_var();struct _tuple0*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);{struct Cyc_Core_Opt*_tmpD6E;
e_exp->topt=((_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((_tmpD6E->v=e_typ,_tmpD6E))));}{
struct Cyc_Toc_Env _tmp8A8;struct _RegionHandle*_tmp8A9;struct Cyc_Toc_Env*_tmp8A7=
nv;_tmp8A8=*_tmp8A7;_tmp8A9=_tmp8A8.rgn;{struct Cyc_Toc_Env*_tmp8AA=Cyc_Toc_add_varmap(
_tmp8A9,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp8AA,_tmp83A);{struct Cyc_Absyn_Stmt*
_tmp8AB=Cyc_Absyn_seq_stmt(_tmp83A,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple0*_tmp8AC=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp8AD=
Cyc_Absyn_var_exp(_tmp8AC,0);struct Cyc_Absyn_Vardecl*_tmp8AE=Cyc_Absyn_new_vardecl(
_tmp8AC,Cyc_Absyn_exn_typ(),0);{struct Cyc_Core_Opt*_tmpD6F;_tmp8AD->topt=((
_tmpD6F=_cycalloc(sizeof(*_tmpD6F)),((_tmpD6F->v=Cyc_Absyn_exn_typ(),_tmpD6F))));}{
struct Cyc_Core_Opt*_tmpD80;struct Cyc_Absyn_Var_p_struct*_tmpD7F;struct Cyc_Absyn_Pat*
_tmpD7E;struct Cyc_Core_Opt*_tmpD7D;struct Cyc_Absyn_Var_p_struct _tmpD7C;struct Cyc_Absyn_Pat*
_tmpD7B;struct Cyc_Absyn_Pat*_tmp8B0=(_tmpD7B=_cycalloc(sizeof(*_tmpD7B)),((
_tmpD7B->r=(void*)((_tmpD7F=_cycalloc(sizeof(*_tmpD7F)),((_tmpD7F[0]=((_tmpD7C.tag=
1,((_tmpD7C.f1=_tmp8AE,((_tmpD7C.f2=((_tmpD7E=_cycalloc(sizeof(*_tmpD7E)),((
_tmpD7E->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpD7E->topt=((_tmpD7D=_cycalloc(
sizeof(*_tmpD7D)),((_tmpD7D->v=Cyc_Absyn_exn_typ(),_tmpD7D)))),((_tmpD7E->loc=0,
_tmpD7E)))))))),_tmpD7C)))))),_tmpD7F)))),((_tmpD7B->topt=((_tmpD80=_cycalloc(
sizeof(*_tmpD80)),((_tmpD80->v=Cyc_Absyn_exn_typ(),_tmpD80)))),((_tmpD7B->loc=0,
_tmpD7B)))))));struct Cyc_Absyn_Exp*_tmp8B1=Cyc_Absyn_throw_exp(_tmp8AD,0);{
struct Cyc_Core_Opt*_tmpD81;_tmp8B1->topt=((_tmpD81=_cycalloc(sizeof(*_tmpD81)),((
_tmpD81->v=(void*)& Cyc_Absyn_VoidType_val,_tmpD81))));}{struct Cyc_Absyn_Stmt*
_tmp8B3=Cyc_Absyn_exp_stmt(_tmp8B1,0);struct Cyc_Core_Opt*_tmpD87;struct Cyc_List_List*
_tmpD86;struct Cyc_Absyn_Switch_clause*_tmpD85;struct Cyc_Absyn_Switch_clause*
_tmp8B4=(_tmpD85=_cycalloc(sizeof(*_tmpD85)),((_tmpD85->pattern=_tmp8B0,((
_tmpD85->pat_vars=((_tmpD87=_cycalloc(sizeof(*_tmpD87)),((_tmpD87->v=((_tmpD86=
_cycalloc(sizeof(*_tmpD86)),((_tmpD86->hd=_tmp8AE,((_tmpD86->tl=0,_tmpD86)))))),
_tmpD87)))),((_tmpD85->where_clause=0,((_tmpD85->body=_tmp8B3,((_tmpD85->loc=0,
_tmpD85)))))))))));struct Cyc_List_List*_tmpD88;struct Cyc_Absyn_Stmt*_tmp8B5=Cyc_Absyn_switch_stmt(
e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp83B,((_tmpD88=_cycalloc(sizeof(*_tmpD88)),((_tmpD88->hd=_tmp8B4,((_tmpD88->tl=
0,_tmpD88))))))),0);Cyc_Toc_stmt_to_c(_tmp8AA,_tmp8B5);{struct Cyc_List_List*
_tmpD89;struct Cyc_Absyn_Exp*_tmp8B6=Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,((
_tmpD89=_cycalloc(sizeof(*_tmpD89)),((_tmpD89->hd=Cyc_Absyn_aggrmember_exp(h_exp,
Cyc_Toc_handler_sp,0),((_tmpD89->tl=0,_tmpD89)))))),0);struct Cyc_List_List*
_tmpD8A;struct Cyc_Absyn_Stmt*_tmp8B7=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpD8A=_cycalloc(sizeof(*_tmpD8A)),((_tmpD8A->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpD8A->tl=0,_tmpD8A)))))),0),0);struct Cyc_Absyn_Exp*_tmp8B8=Cyc_Absyn_int_exp(
Cyc_Absyn_Signed,0,0);struct Cyc_Absyn_Exp*_tmp8B9=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
1,0);s->r=(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp8B7,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp8B8,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp8B6,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp8B9,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp8AB,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp8B5,0),0),0),0),0),0))->r;};};};};}
return;};}_LL382: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp83C=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp80B;if(_tmp83C->tag != 16)goto _LL361;else{_tmp83D=_tmp83C->f1;}}_LL383: if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp83D);{struct Cyc_List_List*
_tmpD8B;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((
_tmpD8B=_cycalloc(sizeof(*_tmpD8B)),((_tmpD8B->hd=_tmp83D,((_tmpD8B->tl=0,
_tmpD8B)))))),0));};}return;_LL361:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);struct _tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8C7=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8C7;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp8C8=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp8C9=f->args;for(0;_tmp8C9 != 0;_tmp8C9=_tmp8C9->tl){struct
_tuple0*_tmpD8C;struct _tuple0*_tmp8CA=(_tmpD8C=_cycalloc(sizeof(*_tmpD8C)),((
_tmpD8C->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpD8C->f2=(*((struct
_tuple25*)_tmp8C9->hd)).f1,_tmpD8C)))));(*((struct _tuple25*)_tmp8C9->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple25*)_tmp8C9->hd)).f3);_tmp8C8=Cyc_Toc_add_varmap(frgn,_tmp8C8,
_tmp8CA,Cyc_Absyn_var_exp(_tmp8CA,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp8CD;struct Cyc_Absyn_Tqual
_tmp8CE;void*_tmp8CF;int _tmp8D0;struct Cyc_Absyn_VarargInfo _tmp8CC=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp8CD=_tmp8CC.name;_tmp8CE=_tmp8CC.tq;_tmp8CF=
_tmp8CC.type;_tmp8D0=_tmp8CC.inject;{void*_tmp8D1=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp8CF,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8CE,Cyc_Absyn_false_conref));struct
_tuple0*_tmpD8D;struct _tuple0*_tmp8D2=(_tmpD8D=_cycalloc(sizeof(*_tmpD8D)),((
_tmpD8D->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpD8D->f2=(struct
_dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null(_tmp8CD))->v,_tmpD8D)))));{
struct _tuple25*_tmpD90;struct Cyc_List_List*_tmpD8F;f->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpD8F=
_cycalloc(sizeof(*_tmpD8F)),((_tmpD8F->hd=((_tmpD90=_cycalloc(sizeof(*_tmpD90)),((
_tmpD90->f1=(struct _dyneither_ptr*)_tmp8CD->v,((_tmpD90->f2=_tmp8CE,((_tmpD90->f3=
_tmp8D1,_tmpD90)))))))),((_tmpD8F->tl=0,_tmpD8F)))))));}_tmp8C8=Cyc_Toc_add_varmap(
frgn,_tmp8C8,_tmp8D2,Cyc_Absyn_var_exp(_tmp8D2,0));f->cyc_varargs=0;};}{struct
Cyc_List_List*_tmp8D6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp8D6 != 0;_tmp8D6=_tmp8D6->tl){((struct Cyc_Absyn_Vardecl*)_tmp8D6->hd)->type=
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp8D6->hd)->type);}}Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel(frgn,_tmp8C8),f->body);};_pop_region(frgn);};}static enum 
Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){switch(s){case Cyc_Absyn_Abstract:
_LL398: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL399: return Cyc_Absyn_Extern;
default: _LL39A: return s;}}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad,int add_to_decls){struct _tuple0*_tmp8D7=ad->name;struct _DynRegionHandle*
_tmp8D9;struct Cyc_Dict_Dict*_tmp8DA;struct Cyc_Toc_TocState _tmp8D8=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp8D9=_tmp8D8.dyn;_tmp8DA=_tmp8D8.aggrs_so_far;{
struct _DynRegionFrame _tmp8DB;struct _RegionHandle*d=_open_dynregion(& _tmp8DB,
_tmp8D9);{int seen_defn_before;struct _tuple14**_tmp8DC=((struct _tuple14**(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp8DA,_tmp8D7);if(
_tmp8DC == 0){seen_defn_before=0;{struct _tuple14*v;if(ad->kind == Cyc_Absyn_StructA){
struct _tuple14*_tmpD91;v=((_tmpD91=_region_malloc(d,sizeof(*_tmpD91)),((_tmpD91->f1=
ad,((_tmpD91->f2=Cyc_Absyn_strctq(_tmp8D7),_tmpD91))))));}else{struct _tuple14*
_tmpD92;v=((_tmpD92=_region_malloc(d,sizeof(*_tmpD92)),((_tmpD92->f1=ad,((
_tmpD92->f2=Cyc_Absyn_unionq_typ(_tmp8D7),_tmpD92))))));}*_tmp8DA=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp8DA,
_tmp8D7,v);};}else{struct _tuple14 _tmp8E0;struct Cyc_Absyn_Aggrdecl*_tmp8E1;void*
_tmp8E2;struct _tuple14*_tmp8DF=*_tmp8DC;_tmp8E0=*_tmp8DF;_tmp8E1=_tmp8E0.f1;
_tmp8E2=_tmp8E0.f2;if(_tmp8E1->impl == 0){{struct _tuple14*_tmpD93;*_tmp8DA=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))
Cyc_Dict_insert)(*_tmp8DA,_tmp8D7,((_tmpD93=_region_malloc(d,sizeof(*_tmpD93)),((
_tmpD93->f1=ad,((_tmpD93->f2=_tmp8E2,_tmpD93)))))));}seen_defn_before=0;}else{
seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*_tmpD94;struct Cyc_Absyn_Aggrdecl*
new_ad=(_tmpD94=_cycalloc(sizeof(*_tmpD94)),((_tmpD94->kind=ad->kind,((_tmpD94->sc=
Cyc_Absyn_Public,((_tmpD94->name=ad->name,((_tmpD94->tvs=0,((_tmpD94->impl=0,((
_tmpD94->attributes=ad->attributes,_tmpD94)))))))))))));if(ad->impl != 0  && !
seen_defn_before){{struct Cyc_Absyn_AggrdeclImpl*_tmpD95;new_ad->impl=((_tmpD95=
_cycalloc(sizeof(*_tmpD95)),((_tmpD95->exist_vars=0,((_tmpD95->rgn_po=0,((
_tmpD95->fields=0,((_tmpD95->tagged=0,_tmpD95))))))))));}{struct Cyc_List_List*
new_fields=0;{struct Cyc_List_List*_tmp8E5=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp8E5 != 0;_tmp8E5=_tmp8E5->tl){struct Cyc_Absyn_Aggrfield*
_tmp8E6=(struct Cyc_Absyn_Aggrfield*)_tmp8E5->hd;struct Cyc_Absyn_Aggrfield*
_tmpD96;struct Cyc_Absyn_Aggrfield*_tmp8E7=(_tmpD96=_cycalloc(sizeof(*_tmpD96)),((
_tmpD96->name=_tmp8E6->name,((_tmpD96->tq=Cyc_Toc_mt_tq,((_tmpD96->type=Cyc_Toc_typ_to_c_array(
_tmp8E6->type),((_tmpD96->width=_tmp8E6->width,((_tmpD96->attributes=_tmp8E6->attributes,
_tmpD96)))))))))));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp8E8=_tmp8E7->type;struct _dyneither_ptr*_tmp8E9=_tmp8E7->name;const char*
_tmpD9B;void*_tmpD9A[2];struct Cyc_String_pa_struct _tmpD99;struct Cyc_String_pa_struct
_tmpD98;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpD98.tag=0,((_tmpD98.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8E9),((_tmpD99.tag=0,((_tmpD99.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpD9A[0]=&
_tmpD99,((_tmpD9A[1]=& _tmpD98,Cyc_aprintf(((_tmpD9B="_union_%s_%s",
_tag_dyneither(_tmpD9B,sizeof(char),13))),_tag_dyneither(_tmpD9A,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpD9C;struct _dyneither_ptr*str=(_tmpD9C=_cycalloc(sizeof(*
_tmpD9C)),((_tmpD9C[0]=s,_tmpD9C)));struct Cyc_Absyn_Aggrfield*_tmpD9D;struct Cyc_Absyn_Aggrfield*
_tmp8EA=(_tmpD9D=_cycalloc(sizeof(*_tmpD9D)),((_tmpD9D->name=Cyc_Toc_val_sp,((
_tmpD9D->tq=Cyc_Toc_mt_tq,((_tmpD9D->type=_tmp8E8,((_tmpD9D->width=0,((_tmpD9D->attributes=
0,_tmpD9D)))))))))));struct Cyc_Absyn_Aggrfield*_tmpD9E;struct Cyc_Absyn_Aggrfield*
_tmp8EB=(_tmpD9E=_cycalloc(sizeof(*_tmpD9E)),((_tmpD9E->name=Cyc_Toc_tag_sp,((
_tmpD9E->tq=Cyc_Toc_mt_tq,((_tmpD9E->type=Cyc_Absyn_sint_typ,((_tmpD9E->width=0,((
_tmpD9E->attributes=0,_tmpD9E)))))))))));struct Cyc_Absyn_Aggrfield*_tmpD9F[2];
struct Cyc_List_List*_tmp8EC=(_tmpD9F[1]=_tmp8EA,((_tmpD9F[0]=_tmp8EB,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD9F,sizeof(struct Cyc_Absyn_Aggrfield*),
2)))));struct Cyc_Absyn_AggrdeclImpl*_tmpDA4;struct _tuple0*_tmpDA3;struct Cyc_Absyn_Aggrdecl*
_tmpDA2;struct Cyc_Absyn_Aggrdecl*_tmp8ED=(_tmpDA2=_cycalloc(sizeof(*_tmpDA2)),((
_tmpDA2->kind=Cyc_Absyn_StructA,((_tmpDA2->sc=Cyc_Absyn_Public,((_tmpDA2->name=((
_tmpDA3=_cycalloc(sizeof(*_tmpDA3)),((_tmpDA3->f1=Cyc_Absyn_Loc_n,((_tmpDA3->f2=
str,_tmpDA3)))))),((_tmpDA2->tvs=0,((_tmpDA2->impl=((_tmpDA4=_cycalloc(sizeof(*
_tmpDA4)),((_tmpDA4->exist_vars=0,((_tmpDA4->rgn_po=0,((_tmpDA4->fields=_tmp8EC,((
_tmpDA4->tagged=0,_tmpDA4)))))))))),((_tmpDA2->attributes=0,_tmpDA2)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmpDAA;struct Cyc_Absyn_Aggr_d_struct _tmpDA9;
struct Cyc_List_List*_tmpDA8;Cyc_Toc_result_decls=((_tmpDA8=_cycalloc(sizeof(*
_tmpDA8)),((_tmpDA8->hd=Cyc_Absyn_new_decl((void*)((_tmpDAA=_cycalloc(sizeof(*
_tmpDAA)),((_tmpDAA[0]=((_tmpDA9.tag=6,((_tmpDA9.f1=_tmp8ED,_tmpDA9)))),_tmpDAA)))),
0),((_tmpDA8->tl=Cyc_Toc_result_decls,_tmpDA8))))));}_tmp8E7->type=Cyc_Absyn_strct(
str);}{struct Cyc_List_List*_tmpDAB;new_fields=((_tmpDAB=_cycalloc(sizeof(*
_tmpDAB)),((_tmpDAB->hd=_tmp8E7,((_tmpDAB->tl=new_fields,_tmpDAB))))));};}}(
new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_fields);};}if(add_to_decls){struct Cyc_Absyn_Decl*_tmpDB5;struct Cyc_Absyn_Aggr_d_struct
_tmpDB4;struct Cyc_Absyn_Aggr_d_struct*_tmpDB3;struct Cyc_List_List*_tmpDB2;Cyc_Toc_result_decls=((
_tmpDB2=_cycalloc(sizeof(*_tmpDB2)),((_tmpDB2->hd=((_tmpDB5=_cycalloc(sizeof(*
_tmpDB5)),((_tmpDB5->r=(void*)((_tmpDB3=_cycalloc(sizeof(*_tmpDB3)),((_tmpDB3[0]=((
_tmpDB4.tag=6,((_tmpDB4.f1=new_ad,_tmpDB4)))),_tmpDB3)))),((_tmpDB5->loc=0,
_tmpDB5)))))),((_tmpDB2->tl=Cyc_Toc_result_decls,_tmpDB2))))));}};};
_pop_dynregion(d);};}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud){struct _DynRegionHandle*_tmp904;struct Cyc_Set_Set**_tmp905;struct Cyc_Toc_TocState
_tmp903=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp904=
_tmp903.dyn;_tmp905=_tmp903.datatypes_so_far;{struct _DynRegionFrame _tmp906;
struct _RegionHandle*d=_open_dynregion(& _tmp906,_tmp904);{struct _tuple0*_tmp907=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))
Cyc_Set_member)(*_tmp905,_tmp907)){_npop_handler(0);return;}*_tmp905=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*
_tmp905,_tmp907);}{struct Cyc_List_List*_tmp908=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp908 != 0;_tmp908=_tmp908->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp908->hd;struct Cyc_List_List*_tmp909=0;int i=
1;{struct Cyc_List_List*_tmp90A=f->typs;for(0;_tmp90A != 0;(_tmp90A=_tmp90A->tl,i
++)){struct _dyneither_ptr*_tmp90B=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpDB6;struct Cyc_Absyn_Aggrfield*_tmp90C=(_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((
_tmpDB6->name=_tmp90B,((_tmpDB6->tq=(*((struct _tuple9*)_tmp90A->hd)).f1,((
_tmpDB6->type=Cyc_Toc_typ_to_c_array((*((struct _tuple9*)_tmp90A->hd)).f2),((
_tmpDB6->width=0,((_tmpDB6->attributes=0,_tmpDB6)))))))))));struct Cyc_List_List*
_tmpDB7;_tmp909=((_tmpDB7=_cycalloc(sizeof(*_tmpDB7)),((_tmpDB7->hd=_tmp90C,((
_tmpDB7->tl=_tmp909,_tmpDB7))))));}}{struct Cyc_Absyn_Aggrfield*_tmpDBA;struct Cyc_List_List*
_tmpDB9;_tmp909=((_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9->hd=((_tmpDBA=
_cycalloc(sizeof(*_tmpDBA)),((_tmpDBA->name=Cyc_Toc_tag_sp,((_tmpDBA->tq=Cyc_Toc_mt_tq,((
_tmpDBA->type=Cyc_Absyn_sint_typ,((_tmpDBA->width=0,((_tmpDBA->attributes=0,
_tmpDBA)))))))))))),((_tmpDB9->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp909),_tmpDB9))))));}{struct Cyc_Absyn_AggrdeclImpl*
_tmpDBF;const char*_tmpDBE;struct Cyc_Absyn_Aggrdecl*_tmpDBD;struct Cyc_Absyn_Aggrdecl*
_tmp911=(_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD->kind=Cyc_Absyn_StructA,((
_tmpDBD->sc=Cyc_Absyn_Public,((_tmpDBD->name=Cyc_Toc_collapse_qvar_tag(f->name,((
_tmpDBE="_struct",_tag_dyneither(_tmpDBE,sizeof(char),8)))),((_tmpDBD->tvs=0,((
_tmpDBD->impl=((_tmpDBF=_cycalloc(sizeof(*_tmpDBF)),((_tmpDBF->exist_vars=0,((
_tmpDBF->rgn_po=0,((_tmpDBF->fields=_tmp909,((_tmpDBF->tagged=0,_tmpDBF)))))))))),((
_tmpDBD->attributes=0,_tmpDBD)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpDC5;
struct Cyc_Absyn_Aggr_d_struct _tmpDC4;struct Cyc_List_List*_tmpDC3;Cyc_Toc_result_decls=((
_tmpDC3=_cycalloc(sizeof(*_tmpDC3)),((_tmpDC3->hd=Cyc_Absyn_new_decl((void*)((
_tmpDC5=_cycalloc(sizeof(*_tmpDC5)),((_tmpDC5[0]=((_tmpDC4.tag=6,((_tmpDC4.f1=
_tmp911,_tmpDC4)))),_tmpDC5)))),0),((_tmpDC3->tl=Cyc_Toc_result_decls,_tmpDC3))))));};}};;
_pop_dynregion(d);};}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd){if(xd->fields == 0)return;{struct _DynRegionHandle*_tmp919;struct Cyc_Dict_Dict*
_tmp91A;struct Cyc_Toc_TocState _tmp918=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp919=_tmp918.dyn;_tmp91A=_tmp918.xdatatypes_so_far;{struct _DynRegionFrame
_tmp91B;struct _RegionHandle*d=_open_dynregion(& _tmp91B,_tmp919);{struct _tuple0*
_tmp91C=xd->name;struct Cyc_List_List*_tmp91D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp91D != 0;_tmp91D=_tmp91D->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp91D->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp91E=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp91F=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp91E,Cyc_Absyn_false_conref,0);int*_tmp920=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp91A,f->name);int
_tmp921;_LL39D: if(_tmp920 != 0)goto _LL39F;_LL39E: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp922=Cyc_Absyn_new_vardecl(f->name,_tmp91F,
initopt);_tmp922->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*_tmpDCB;struct Cyc_Absyn_Var_d_struct
_tmpDCA;struct Cyc_List_List*_tmpDC9;Cyc_Toc_result_decls=((_tmpDC9=_cycalloc(
sizeof(*_tmpDC9)),((_tmpDC9->hd=Cyc_Absyn_new_decl((void*)((_tmpDCB=_cycalloc(
sizeof(*_tmpDCB)),((_tmpDCB[0]=((_tmpDCA.tag=0,((_tmpDCA.f1=_tmp922,_tmpDCA)))),
_tmpDCB)))),0),((_tmpDC9->tl=Cyc_Toc_result_decls,_tmpDC9))))));}*_tmp91A=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp91A,f->name,f->sc != Cyc_Absyn_Extern);{struct Cyc_List_List*fields=0;int i=1;{
struct Cyc_List_List*_tmp926=f->typs;for(0;_tmp926 != 0;(_tmp926=_tmp926->tl,i ++)){
struct Cyc_Int_pa_struct _tmpDD3;void*_tmpDD2[1];const char*_tmpDD1;struct
_dyneither_ptr*_tmpDD0;struct _dyneither_ptr*_tmp927=(_tmpDD0=_cycalloc(sizeof(*
_tmpDD0)),((_tmpDD0[0]=(struct _dyneither_ptr)((_tmpDD3.tag=1,((_tmpDD3.f1=(
unsigned long)i,((_tmpDD2[0]=& _tmpDD3,Cyc_aprintf(((_tmpDD1="f%d",_tag_dyneither(
_tmpDD1,sizeof(char),4))),_tag_dyneither(_tmpDD2,sizeof(void*),1)))))))),_tmpDD0)));
struct Cyc_Absyn_Aggrfield*_tmpDD4;struct Cyc_Absyn_Aggrfield*_tmp928=(_tmpDD4=
_cycalloc(sizeof(*_tmpDD4)),((_tmpDD4->name=_tmp927,((_tmpDD4->tq=(*((struct
_tuple9*)_tmp926->hd)).f1,((_tmpDD4->type=Cyc_Toc_typ_to_c_array((*((struct
_tuple9*)_tmp926->hd)).f2),((_tmpDD4->width=0,((_tmpDD4->attributes=0,_tmpDD4)))))))))));
struct Cyc_List_List*_tmpDD5;fields=((_tmpDD5=_cycalloc(sizeof(*_tmpDD5)),((
_tmpDD5->hd=_tmp928,((_tmpDD5->tl=fields,_tmpDD5))))));}}{struct Cyc_Absyn_Aggrfield*
_tmpDD8;struct Cyc_List_List*_tmpDD7;fields=((_tmpDD7=_cycalloc(sizeof(*_tmpDD7)),((
_tmpDD7->hd=((_tmpDD8=_cycalloc(sizeof(*_tmpDD8)),((_tmpDD8->name=Cyc_Toc_tag_sp,((
_tmpDD8->tq=Cyc_Toc_mt_tq,((_tmpDD8->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq),((_tmpDD8->width=0,((_tmpDD8->attributes=0,_tmpDD8)))))))))))),((
_tmpDD7->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmpDD7))))));}{struct Cyc_Absyn_AggrdeclImpl*_tmpDDD;const char*_tmpDDC;
struct Cyc_Absyn_Aggrdecl*_tmpDDB;struct Cyc_Absyn_Aggrdecl*_tmp931=(_tmpDDB=
_cycalloc(sizeof(*_tmpDDB)),((_tmpDDB->kind=Cyc_Absyn_StructA,((_tmpDDB->sc=Cyc_Absyn_Public,((
_tmpDDB->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpDDC="_struct",
_tag_dyneither(_tmpDDC,sizeof(char),8)))),((_tmpDDB->tvs=0,((_tmpDDB->impl=((
_tmpDDD=_cycalloc(sizeof(*_tmpDDD)),((_tmpDDD->exist_vars=0,((_tmpDDD->rgn_po=0,((
_tmpDDD->fields=fields,((_tmpDDD->tagged=0,_tmpDDD)))))))))),((_tmpDDB->attributes=
0,_tmpDDB)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpDE3;struct Cyc_Absyn_Aggr_d_struct
_tmpDE2;struct Cyc_List_List*_tmpDE1;Cyc_Toc_result_decls=((_tmpDE1=_cycalloc(
sizeof(*_tmpDE1)),((_tmpDE1->hd=Cyc_Absyn_new_decl((void*)((_tmpDE3=_cycalloc(
sizeof(*_tmpDE3)),((_tmpDE3[0]=((_tmpDE2.tag=6,((_tmpDE2.f1=_tmp931,_tmpDE2)))),
_tmpDE3)))),0),((_tmpDE1->tl=Cyc_Toc_result_decls,_tmpDE1))))));}goto _LL39C;};};};}
_LL39F: if(_tmp920 == 0)goto _LL3A1;_tmp921=*_tmp920;if(_tmp921 != 0)goto _LL3A1;
_LL3A0: if(f->sc != Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*_tmp938=Cyc_Absyn_string_exp(*
fn,0);struct Cyc_Absyn_Vardecl*_tmp939=Cyc_Absyn_new_vardecl(f->name,_tmp91F,(
struct Cyc_Absyn_Exp*)_tmp938);_tmp939->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*
_tmpDE9;struct Cyc_Absyn_Var_d_struct _tmpDE8;struct Cyc_List_List*_tmpDE7;Cyc_Toc_result_decls=((
_tmpDE7=_cycalloc(sizeof(*_tmpDE7)),((_tmpDE7->hd=Cyc_Absyn_new_decl((void*)((
_tmpDE9=_cycalloc(sizeof(*_tmpDE9)),((_tmpDE9[0]=((_tmpDE8.tag=0,((_tmpDE8.f1=
_tmp939,_tmpDE8)))),_tmpDE9)))),0),((_tmpDE7->tl=Cyc_Toc_result_decls,_tmpDE7))))));}*
_tmp91A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp91A,f->name,1);}goto _LL39C;_LL3A1:;_LL3A2: goto _LL39C;_LL39C:;}};
_pop_dynregion(d);};};}static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*
ed){ed->sc=Cyc_Absyn_Public;if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(
struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,
struct Cyc_Absyn_Stmt*s){void*old_typ=vd->type;vd->type=Cyc_Toc_typ_to_c_array(
old_typ);if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ))vd->sc=Cyc_Absyn_Public;Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp93D=init->r;struct Cyc_Absyn_Vardecl*_tmp93F;struct Cyc_Absyn_Exp*_tmp940;
struct Cyc_Absyn_Exp*_tmp941;int _tmp942;_LL3A4: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp93E=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp93D;if(_tmp93E->tag != 28)
goto _LL3A6;else{_tmp93F=_tmp93E->f1;_tmp940=_tmp93E->f2;_tmp941=_tmp93E->f3;
_tmp942=_tmp93E->f4;}}_LL3A5: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp93F,_tmp940,_tmp941,_tmp942,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL3A3;_LL3A6:;_LL3A7: if(vd->sc == Cyc_Absyn_Static){struct Cyc_Toc_Env
_tmp944;struct _RegionHandle*_tmp945;struct Cyc_Toc_Env*_tmp943=init_nv;_tmp944=*
_tmp943;_tmp945=_tmp944.rgn;{struct Cyc_Toc_Env*_tmp946=Cyc_Toc_set_toplevel(
_tmp945,init_nv);Cyc_Toc_exp_to_c(_tmp946,init);};}else{Cyc_Toc_exp_to_c(init_nv,
init);}goto _LL3A3;_LL3A3:;}else{void*_tmp947=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_ArrayInfo _tmp949;void*_tmp94A;struct Cyc_Absyn_Exp*_tmp94B;union Cyc_Absyn_Constraint*
_tmp94C;_LL3A9:{struct Cyc_Absyn_ArrayType_struct*_tmp948=(struct Cyc_Absyn_ArrayType_struct*)
_tmp947;if(_tmp948->tag != 9)goto _LL3AB;else{_tmp949=_tmp948->f1;_tmp94A=_tmp949.elt_type;
_tmp94B=_tmp949.num_elts;_tmp94C=_tmp949.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp94C))goto _LL3AB;_LL3AA: if(_tmp94B == 0){const char*
_tmpDEC;void*_tmpDEB;(_tmpDEB=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDEC="can't initialize zero-terminated array -- size unknown",
_tag_dyneither(_tmpDEC,sizeof(char),55))),_tag_dyneither(_tmpDEB,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp94B;struct Cyc_Absyn_Exp*
_tmp94F=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp950=Cyc_Absyn_signed_int_exp(
0,0);s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp94F,
_tmp950,0),0),Cyc_Absyn_new_stmt(s->r,0));goto _LL3A8;};_LL3AB:;_LL3AC: goto _LL3A8;
_LL3A8:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple0*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp952;struct
_RegionHandle*_tmp953;struct Cyc_Toc_Env*_tmp951=nv;_tmp952=*_tmp951;_tmp953=
_tmp952.rgn;{struct Cyc_Absyn_Stmt*_tmp954=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp955=Cyc_Toc_share_env(_tmp953,nv);struct Cyc_Toc_Env*_tmp957;struct Cyc_List_List*
_tmp958;struct Cyc_Absyn_Stmt*_tmp959;struct _tuple21 _tmp956=Cyc_Toc_xlate_pat(
_tmp955,_tmp953,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp957=_tmp956.f1;_tmp958=_tmp956.f2;_tmp959=_tmp956.f3;Cyc_Toc_stmt_to_c(
_tmp957,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp959,s,0),0);for(0;_tmp958 != 0;_tmp958=_tmp958->tl){
struct _tuple22 _tmp95B;struct _tuple0*_tmp95C;void*_tmp95D;struct _tuple22*_tmp95A=(
struct _tuple22*)_tmp958->hd;_tmp95B=*_tmp95A;_tmp95C=_tmp95B.f1;_tmp95D=_tmp95B.f2;
s=Cyc_Absyn_declare_stmt(_tmp95C,_tmp95D,0,s,0);}}return s;};}static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e){void*_tmp95E=e->r;struct Cyc_Absyn_Exp*_tmp960;struct Cyc_Absyn_Exp*
_tmp962;struct Cyc_Absyn_Exp*_tmp964;struct Cyc_Absyn_Exp*_tmp966;struct Cyc_Absyn_Exp*
_tmp968;struct Cyc_Absyn_Exp*_tmp96A;struct Cyc_Absyn_Exp*_tmp96C;struct Cyc_Absyn_Exp*
_tmp96E;struct Cyc_List_List*_tmp970;struct Cyc_Absyn_Exp*_tmp972;struct Cyc_Absyn_Exp*
_tmp973;struct Cyc_Absyn_Exp*_tmp975;struct Cyc_Absyn_Exp*_tmp976;struct Cyc_Absyn_Exp*
_tmp978;struct Cyc_Absyn_Exp*_tmp979;struct Cyc_Absyn_Exp*_tmp97B;struct Cyc_Absyn_Exp*
_tmp97C;struct Cyc_Absyn_Exp*_tmp97E;struct Cyc_Absyn_Exp*_tmp97F;struct Cyc_Absyn_Exp*
_tmp981;struct Cyc_Absyn_Exp*_tmp982;struct Cyc_Absyn_Exp*_tmp984;struct Cyc_Absyn_Exp*
_tmp985;struct Cyc_Absyn_Exp*_tmp986;struct Cyc_Absyn_Exp*_tmp988;struct Cyc_List_List*
_tmp989;struct Cyc_Absyn_Exp*_tmp98B;struct Cyc_List_List*_tmp98C;void*_tmp98E;
void**_tmp98F;struct Cyc_Absyn_Exp*_tmp990;struct _tuple7*_tmp992;struct _tuple7
_tmp993;void*_tmp994;void**_tmp995;struct Cyc_List_List*_tmp996;struct Cyc_List_List*
_tmp998;struct Cyc_List_List*_tmp99A;void*_tmp99C;void**_tmp99D;void*_tmp99F;void**
_tmp9A0;struct Cyc_Absyn_Stmt*_tmp9A2;struct Cyc_Absyn_MallocInfo _tmp9A4;struct Cyc_Absyn_MallocInfo*
_tmp9A5;_LL3AE: {struct Cyc_Absyn_Deref_e_struct*_tmp95F=(struct Cyc_Absyn_Deref_e_struct*)
_tmp95E;if(_tmp95F->tag != 21)goto _LL3B0;else{_tmp960=_tmp95F->f1;}}_LL3AF:
_tmp962=_tmp960;goto _LL3B1;_LL3B0: {struct Cyc_Absyn_AggrMember_e_struct*_tmp961=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp95E;if(_tmp961->tag != 22)goto _LL3B2;
else{_tmp962=_tmp961->f1;}}_LL3B1: _tmp964=_tmp962;goto _LL3B3;_LL3B2: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp963=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp95E;if(_tmp963->tag != 23)goto
_LL3B4;else{_tmp964=_tmp963->f1;}}_LL3B3: _tmp966=_tmp964;goto _LL3B5;_LL3B4: {
struct Cyc_Absyn_Address_e_struct*_tmp965=(struct Cyc_Absyn_Address_e_struct*)
_tmp95E;if(_tmp965->tag != 16)goto _LL3B6;else{_tmp966=_tmp965->f1;}}_LL3B5:
_tmp968=_tmp966;goto _LL3B7;_LL3B6: {struct Cyc_Absyn_Throw_e_struct*_tmp967=(
struct Cyc_Absyn_Throw_e_struct*)_tmp95E;if(_tmp967->tag != 12)goto _LL3B8;else{
_tmp968=_tmp967->f1;}}_LL3B7: _tmp96A=_tmp968;goto _LL3B9;_LL3B8: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp969=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp95E;if(_tmp969->tag != 13)
goto _LL3BA;else{_tmp96A=_tmp969->f1;}}_LL3B9: _tmp96C=_tmp96A;goto _LL3BB;_LL3BA: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp96B=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp95E;if(_tmp96B->tag != 19)goto _LL3BC;else{_tmp96C=_tmp96B->f1;}}_LL3BB:
_tmp96E=_tmp96C;goto _LL3BD;_LL3BC: {struct Cyc_Absyn_Increment_e_struct*_tmp96D=(
struct Cyc_Absyn_Increment_e_struct*)_tmp95E;if(_tmp96D->tag != 5)goto _LL3BE;else{
_tmp96E=_tmp96D->f1;}}_LL3BD: Cyc_Toc_exptypes_to_c(_tmp96E);goto _LL3AD;_LL3BE: {
struct Cyc_Absyn_Primop_e_struct*_tmp96F=(struct Cyc_Absyn_Primop_e_struct*)
_tmp95E;if(_tmp96F->tag != 3)goto _LL3C0;else{_tmp970=_tmp96F->f2;}}_LL3BF:((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,
_tmp970);goto _LL3AD;_LL3C0: {struct Cyc_Absyn_And_e_struct*_tmp971=(struct Cyc_Absyn_And_e_struct*)
_tmp95E;if(_tmp971->tag != 7)goto _LL3C2;else{_tmp972=_tmp971->f1;_tmp973=_tmp971->f2;}}
_LL3C1: _tmp975=_tmp972;_tmp976=_tmp973;goto _LL3C3;_LL3C2: {struct Cyc_Absyn_Or_e_struct*
_tmp974=(struct Cyc_Absyn_Or_e_struct*)_tmp95E;if(_tmp974->tag != 8)goto _LL3C4;
else{_tmp975=_tmp974->f1;_tmp976=_tmp974->f2;}}_LL3C3: _tmp978=_tmp975;_tmp979=
_tmp976;goto _LL3C5;_LL3C4: {struct Cyc_Absyn_SeqExp_e_struct*_tmp977=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp95E;if(_tmp977->tag != 9)goto _LL3C6;else{_tmp978=_tmp977->f1;_tmp979=_tmp977->f2;}}
_LL3C5: _tmp97B=_tmp978;_tmp97C=_tmp979;goto _LL3C7;_LL3C6: {struct Cyc_Absyn_Subscript_e_struct*
_tmp97A=(struct Cyc_Absyn_Subscript_e_struct*)_tmp95E;if(_tmp97A->tag != 24)goto
_LL3C8;else{_tmp97B=_tmp97A->f1;_tmp97C=_tmp97A->f2;}}_LL3C7: _tmp97E=_tmp97B;
_tmp97F=_tmp97C;goto _LL3C9;_LL3C8: {struct Cyc_Absyn_Swap_e_struct*_tmp97D=(
struct Cyc_Absyn_Swap_e_struct*)_tmp95E;if(_tmp97D->tag != 35)goto _LL3CA;else{
_tmp97E=_tmp97D->f1;_tmp97F=_tmp97D->f2;}}_LL3C9: _tmp981=_tmp97E;_tmp982=_tmp97F;
goto _LL3CB;_LL3CA: {struct Cyc_Absyn_AssignOp_e_struct*_tmp980=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp95E;if(_tmp980->tag != 4)goto _LL3CC;else{_tmp981=_tmp980->f1;_tmp982=_tmp980->f3;}}
_LL3CB: Cyc_Toc_exptypes_to_c(_tmp981);Cyc_Toc_exptypes_to_c(_tmp982);goto _LL3AD;
_LL3CC: {struct Cyc_Absyn_Conditional_e_struct*_tmp983=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp95E;if(_tmp983->tag != 6)goto _LL3CE;else{_tmp984=_tmp983->f1;_tmp985=_tmp983->f2;
_tmp986=_tmp983->f3;}}_LL3CD: Cyc_Toc_exptypes_to_c(_tmp984);Cyc_Toc_exptypes_to_c(
_tmp985);Cyc_Toc_exptypes_to_c(_tmp986);goto _LL3AD;_LL3CE: {struct Cyc_Absyn_FnCall_e_struct*
_tmp987=(struct Cyc_Absyn_FnCall_e_struct*)_tmp95E;if(_tmp987->tag != 11)goto
_LL3D0;else{_tmp988=_tmp987->f1;_tmp989=_tmp987->f2;}}_LL3CF: _tmp98B=_tmp988;
_tmp98C=_tmp989;goto _LL3D1;_LL3D0: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp98A=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp95E;if(_tmp98A->tag != 10)goto _LL3D2;
else{_tmp98B=_tmp98A->f1;_tmp98C=_tmp98A->f2;}}_LL3D1: Cyc_Toc_exptypes_to_c(
_tmp98B);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp98C);goto _LL3AD;_LL3D2: {struct Cyc_Absyn_Cast_e_struct*
_tmp98D=(struct Cyc_Absyn_Cast_e_struct*)_tmp95E;if(_tmp98D->tag != 15)goto _LL3D4;
else{_tmp98E=(void**)& _tmp98D->f1;_tmp98F=(void**)((void**)& _tmp98D->f1);_tmp990=
_tmp98D->f2;}}_LL3D3:*_tmp98F=Cyc_Toc_typ_to_c(*_tmp98F);Cyc_Toc_exptypes_to_c(
_tmp990);goto _LL3AD;_LL3D4: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp991=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp95E;if(_tmp991->tag != 26)goto _LL3D6;
else{_tmp992=_tmp991->f1;_tmp993=*_tmp992;_tmp994=_tmp993.f3;_tmp995=(void**)&(*
_tmp991->f1).f3;_tmp996=_tmp991->f2;}}_LL3D5:*_tmp995=Cyc_Toc_typ_to_c(*_tmp995);
_tmp998=_tmp996;goto _LL3D7;_LL3D6: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp997=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp95E;if(_tmp997->tag != 36)
goto _LL3D8;else{_tmp998=_tmp997->f2;}}_LL3D7: _tmp99A=_tmp998;goto _LL3D9;_LL3D8: {
struct Cyc_Absyn_Array_e_struct*_tmp999=(struct Cyc_Absyn_Array_e_struct*)_tmp95E;
if(_tmp999->tag != 27)goto _LL3DA;else{_tmp99A=_tmp999->f1;}}_LL3D9: for(0;_tmp99A
!= 0;_tmp99A=_tmp99A->tl){struct Cyc_Absyn_Exp*_tmp9B6;struct _tuple15 _tmp9B5=*((
struct _tuple15*)_tmp99A->hd);_tmp9B6=_tmp9B5.f2;Cyc_Toc_exptypes_to_c(_tmp9B6);}
goto _LL3AD;_LL3DA: {struct Cyc_Absyn_Offsetof_e_struct*_tmp99B=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp95E;if(_tmp99B->tag != 20)goto _LL3DC;else{_tmp99C=(void**)& _tmp99B->f1;
_tmp99D=(void**)((void**)& _tmp99B->f1);}}_LL3DB: _tmp9A0=_tmp99D;goto _LL3DD;
_LL3DC: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp99E=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp95E;if(_tmp99E->tag != 18)goto _LL3DE;else{_tmp99F=(void**)& _tmp99E->f1;
_tmp9A0=(void**)((void**)& _tmp99E->f1);}}_LL3DD:*_tmp9A0=Cyc_Toc_typ_to_c(*
_tmp9A0);goto _LL3AD;_LL3DE: {struct Cyc_Absyn_StmtExp_e_struct*_tmp9A1=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp95E;if(_tmp9A1->tag != 37)goto _LL3E0;else{_tmp9A2=_tmp9A1->f1;}}_LL3DF: Cyc_Toc_stmttypes_to_c(
_tmp9A2);goto _LL3AD;_LL3E0: {struct Cyc_Absyn_Malloc_e_struct*_tmp9A3=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp95E;if(_tmp9A3->tag != 34)goto _LL3E2;else{_tmp9A4=_tmp9A3->f1;_tmp9A5=(struct
Cyc_Absyn_MallocInfo*)& _tmp9A3->f1;}}_LL3E1: if(_tmp9A5->elt_type != 0){void**
_tmpDED;_tmp9A5->elt_type=((_tmpDED=_cycalloc(sizeof(*_tmpDED)),((_tmpDED[0]=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp9A5->elt_type))),_tmpDED))));}Cyc_Toc_exptypes_to_c(
_tmp9A5->num_elts);goto _LL3AD;_LL3E2: {struct Cyc_Absyn_Const_e_struct*_tmp9A6=(
struct Cyc_Absyn_Const_e_struct*)_tmp95E;if(_tmp9A6->tag != 0)goto _LL3E4;}_LL3E3:
goto _LL3E5;_LL3E4: {struct Cyc_Absyn_Var_e_struct*_tmp9A7=(struct Cyc_Absyn_Var_e_struct*)
_tmp95E;if(_tmp9A7->tag != 1)goto _LL3E6;}_LL3E5: goto _LL3E7;_LL3E6: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp9A8=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp95E;if(_tmp9A8->tag != 2)goto
_LL3E8;}_LL3E7: goto _LL3E9;_LL3E8: {struct Cyc_Absyn_Enum_e_struct*_tmp9A9=(struct
Cyc_Absyn_Enum_e_struct*)_tmp95E;if(_tmp9A9->tag != 32)goto _LL3EA;}_LL3E9: goto
_LL3EB;_LL3EA: {struct Cyc_Absyn_Asm_e_struct*_tmp9AA=(struct Cyc_Absyn_Asm_e_struct*)
_tmp95E;if(_tmp9AA->tag != 40)goto _LL3EC;}_LL3EB: goto _LL3ED;_LL3EC: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp9AB=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp95E;if(_tmp9AB->tag != 33)goto
_LL3EE;}_LL3ED: goto _LL3AD;_LL3EE: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp9AC=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp95E;if(_tmp9AC->tag != 30)goto _LL3F0;}
_LL3EF: goto _LL3F1;_LL3F0: {struct Cyc_Absyn_Datatype_e_struct*_tmp9AD=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp95E;if(_tmp9AD->tag != 31)goto _LL3F2;}_LL3F1: goto _LL3F3;_LL3F2: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp9AE=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp95E;if(_tmp9AE->tag != 29)goto
_LL3F4;}_LL3F3: goto _LL3F5;_LL3F4: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp9AF=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp95E;if(_tmp9AF->tag != 28)
goto _LL3F6;}_LL3F5: goto _LL3F7;_LL3F6: {struct Cyc_Absyn_Tuple_e_struct*_tmp9B0=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp95E;if(_tmp9B0->tag != 25)goto _LL3F8;}_LL3F7:
goto _LL3F9;_LL3F8: {struct Cyc_Absyn_Instantiate_e_struct*_tmp9B1=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp95E;if(_tmp9B1->tag != 14)goto _LL3FA;}_LL3F9: goto _LL3FB;_LL3FA: {struct Cyc_Absyn_New_e_struct*
_tmp9B2=(struct Cyc_Absyn_New_e_struct*)_tmp95E;if(_tmp9B2->tag != 17)goto _LL3FC;}
_LL3FB: goto _LL3FD;_LL3FC: {struct Cyc_Absyn_Valueof_e_struct*_tmp9B3=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp95E;if(_tmp9B3->tag != 39)goto _LL3FE;}_LL3FD: goto _LL3FF;_LL3FE: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp9B4=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp95E;if(_tmp9B4->tag != 38)goto
_LL3AD;}_LL3FF:{const char*_tmpDF0;void*_tmpDEF;(_tmpDEF=0,Cyc_Tcutil_terr(e->loc,((
_tmpDF0="Cyclone expression within C code",_tag_dyneither(_tmpDF0,sizeof(char),
33))),_tag_dyneither(_tmpDEF,sizeof(void*),0)));}goto _LL3AD;_LL3AD:;}static void
Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){void*_tmp9BA=d->r;struct Cyc_Absyn_Vardecl*
_tmp9BC;struct Cyc_Absyn_Fndecl*_tmp9BE;struct Cyc_Absyn_Aggrdecl*_tmp9C0;struct
Cyc_Absyn_Enumdecl*_tmp9C2;struct Cyc_Absyn_Typedefdecl*_tmp9C4;_LL401: {struct
Cyc_Absyn_Var_d_struct*_tmp9BB=(struct Cyc_Absyn_Var_d_struct*)_tmp9BA;if(_tmp9BB->tag
!= 0)goto _LL403;else{_tmp9BC=_tmp9BB->f1;}}_LL402: _tmp9BC->type=Cyc_Toc_typ_to_c(
_tmp9BC->type);if(_tmp9BC->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp9BC->initializer));goto _LL400;_LL403: {struct Cyc_Absyn_Fn_d_struct*
_tmp9BD=(struct Cyc_Absyn_Fn_d_struct*)_tmp9BA;if(_tmp9BD->tag != 1)goto _LL405;
else{_tmp9BE=_tmp9BD->f1;}}_LL404: _tmp9BE->ret_type=Cyc_Toc_typ_to_c(_tmp9BE->ret_type);{
struct Cyc_List_List*_tmp9D0=_tmp9BE->args;for(0;_tmp9D0 != 0;_tmp9D0=_tmp9D0->tl){(*((
struct _tuple25*)_tmp9D0->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple25*)_tmp9D0->hd)).f3);}}
goto _LL400;_LL405: {struct Cyc_Absyn_Aggr_d_struct*_tmp9BF=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp9BA;if(_tmp9BF->tag != 6)goto _LL407;else{_tmp9C0=_tmp9BF->f1;}}_LL406: Cyc_Toc_aggrdecl_to_c(
_tmp9C0,1);goto _LL400;_LL407: {struct Cyc_Absyn_Enum_d_struct*_tmp9C1=(struct Cyc_Absyn_Enum_d_struct*)
_tmp9BA;if(_tmp9C1->tag != 8)goto _LL409;else{_tmp9C2=_tmp9C1->f1;}}_LL408: if(
_tmp9C2->fields != 0){struct Cyc_List_List*_tmp9D1=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp9C2->fields))->v;for(0;_tmp9D1 != 0;_tmp9D1=_tmp9D1->tl){
struct Cyc_Absyn_Enumfield*_tmp9D2=(struct Cyc_Absyn_Enumfield*)_tmp9D1->hd;if(
_tmp9D2->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9D2->tag));}}
goto _LL400;_LL409: {struct Cyc_Absyn_Typedef_d_struct*_tmp9C3=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp9BA;if(_tmp9C3->tag != 9)goto _LL40B;else{_tmp9C4=_tmp9C3->f1;}}_LL40A:{struct
Cyc_Core_Opt*_tmpDF1;_tmp9C4->defn=((_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((
_tmpDF1->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp9C4->defn))->v),_tmpDF1))));}goto _LL400;_LL40B: {struct Cyc_Absyn_Let_d_struct*
_tmp9C5=(struct Cyc_Absyn_Let_d_struct*)_tmp9BA;if(_tmp9C5->tag != 2)goto _LL40D;}
_LL40C: goto _LL40E;_LL40D: {struct Cyc_Absyn_Letv_d_struct*_tmp9C6=(struct Cyc_Absyn_Letv_d_struct*)
_tmp9BA;if(_tmp9C6->tag != 3)goto _LL40F;}_LL40E: goto _LL410;_LL40F: {struct Cyc_Absyn_Datatype_d_struct*
_tmp9C7=(struct Cyc_Absyn_Datatype_d_struct*)_tmp9BA;if(_tmp9C7->tag != 7)goto
_LL411;}_LL410: goto _LL412;_LL411: {struct Cyc_Absyn_Namespace_d_struct*_tmp9C8=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp9BA;if(_tmp9C8->tag != 10)goto _LL413;}
_LL412: goto _LL414;_LL413: {struct Cyc_Absyn_Using_d_struct*_tmp9C9=(struct Cyc_Absyn_Using_d_struct*)
_tmp9BA;if(_tmp9C9->tag != 11)goto _LL415;}_LL414: goto _LL416;_LL415: {struct Cyc_Absyn_ExternC_d_struct*
_tmp9CA=(struct Cyc_Absyn_ExternC_d_struct*)_tmp9BA;if(_tmp9CA->tag != 12)goto
_LL417;}_LL416: goto _LL418;_LL417: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp9CB=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9BA;if(_tmp9CB->tag != 13)
goto _LL419;}_LL418: goto _LL41A;_LL419: {struct Cyc_Absyn_Region_d_struct*_tmp9CC=(
struct Cyc_Absyn_Region_d_struct*)_tmp9BA;if(_tmp9CC->tag != 4)goto _LL41B;}_LL41A:
goto _LL41C;_LL41B: {struct Cyc_Absyn_Alias_d_struct*_tmp9CD=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9BA;if(_tmp9CD->tag != 5)goto _LL41D;}_LL41C:{const char*_tmpDF4;void*_tmpDF3;(
_tmpDF3=0,Cyc_Tcutil_terr(d->loc,((_tmpDF4="Cyclone declaration within C code",
_tag_dyneither(_tmpDF4,sizeof(char),34))),_tag_dyneither(_tmpDF3,sizeof(void*),0)));}
goto _LL400;_LL41D: {struct Cyc_Absyn_Porton_d_struct*_tmp9CE=(struct Cyc_Absyn_Porton_d_struct*)
_tmp9BA;if(_tmp9CE->tag != 14)goto _LL41F;}_LL41E: goto _LL420;_LL41F: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9CF=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9BA;if(_tmp9CF->tag != 15)goto
_LL400;}_LL420: goto _LL400;_LL400:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s){void*_tmp9D6=s->r;struct Cyc_Absyn_Exp*_tmp9D8;struct Cyc_Absyn_Stmt*_tmp9DA;
struct Cyc_Absyn_Stmt*_tmp9DB;struct Cyc_Absyn_Exp*_tmp9DD;struct Cyc_Absyn_Exp*
_tmp9DF;struct Cyc_Absyn_Stmt*_tmp9E0;struct Cyc_Absyn_Stmt*_tmp9E1;struct _tuple8
_tmp9E3;struct Cyc_Absyn_Exp*_tmp9E4;struct Cyc_Absyn_Stmt*_tmp9E5;struct Cyc_Absyn_Exp*
_tmp9E7;struct _tuple8 _tmp9E8;struct Cyc_Absyn_Exp*_tmp9E9;struct _tuple8 _tmp9EA;
struct Cyc_Absyn_Exp*_tmp9EB;struct Cyc_Absyn_Stmt*_tmp9EC;struct Cyc_Absyn_Exp*
_tmp9EE;struct Cyc_List_List*_tmp9EF;struct Cyc_Absyn_Decl*_tmp9F1;struct Cyc_Absyn_Stmt*
_tmp9F2;struct Cyc_Absyn_Stmt*_tmp9F4;struct _tuple8 _tmp9F5;struct Cyc_Absyn_Exp*
_tmp9F6;_LL422: {struct Cyc_Absyn_Exp_s_struct*_tmp9D7=(struct Cyc_Absyn_Exp_s_struct*)
_tmp9D6;if(_tmp9D7->tag != 1)goto _LL424;else{_tmp9D8=_tmp9D7->f1;}}_LL423: Cyc_Toc_exptypes_to_c(
_tmp9D8);goto _LL421;_LL424: {struct Cyc_Absyn_Seq_s_struct*_tmp9D9=(struct Cyc_Absyn_Seq_s_struct*)
_tmp9D6;if(_tmp9D9->tag != 2)goto _LL426;else{_tmp9DA=_tmp9D9->f1;_tmp9DB=_tmp9D9->f2;}}
_LL425: Cyc_Toc_stmttypes_to_c(_tmp9DA);Cyc_Toc_stmttypes_to_c(_tmp9DB);goto
_LL421;_LL426: {struct Cyc_Absyn_Return_s_struct*_tmp9DC=(struct Cyc_Absyn_Return_s_struct*)
_tmp9D6;if(_tmp9DC->tag != 3)goto _LL428;else{_tmp9DD=_tmp9DC->f1;}}_LL427: if(
_tmp9DD != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmp9DD);goto _LL421;
_LL428: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp9DE=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp9D6;if(_tmp9DE->tag != 4)goto _LL42A;else{_tmp9DF=_tmp9DE->f1;_tmp9E0=_tmp9DE->f2;
_tmp9E1=_tmp9DE->f3;}}_LL429: Cyc_Toc_exptypes_to_c(_tmp9DF);Cyc_Toc_stmttypes_to_c(
_tmp9E0);Cyc_Toc_stmttypes_to_c(_tmp9E1);goto _LL421;_LL42A: {struct Cyc_Absyn_While_s_struct*
_tmp9E2=(struct Cyc_Absyn_While_s_struct*)_tmp9D6;if(_tmp9E2->tag != 5)goto _LL42C;
else{_tmp9E3=_tmp9E2->f1;_tmp9E4=_tmp9E3.f1;_tmp9E5=_tmp9E2->f2;}}_LL42B: Cyc_Toc_exptypes_to_c(
_tmp9E4);Cyc_Toc_stmttypes_to_c(_tmp9E5);goto _LL421;_LL42C: {struct Cyc_Absyn_For_s_struct*
_tmp9E6=(struct Cyc_Absyn_For_s_struct*)_tmp9D6;if(_tmp9E6->tag != 9)goto _LL42E;
else{_tmp9E7=_tmp9E6->f1;_tmp9E8=_tmp9E6->f2;_tmp9E9=_tmp9E8.f1;_tmp9EA=_tmp9E6->f3;
_tmp9EB=_tmp9EA.f1;_tmp9EC=_tmp9E6->f4;}}_LL42D: Cyc_Toc_exptypes_to_c(_tmp9E7);
Cyc_Toc_exptypes_to_c(_tmp9E9);Cyc_Toc_exptypes_to_c(_tmp9EB);Cyc_Toc_stmttypes_to_c(
_tmp9EC);goto _LL421;_LL42E: {struct Cyc_Absyn_Switch_s_struct*_tmp9ED=(struct Cyc_Absyn_Switch_s_struct*)
_tmp9D6;if(_tmp9ED->tag != 10)goto _LL430;else{_tmp9EE=_tmp9ED->f1;_tmp9EF=_tmp9ED->f2;}}
_LL42F: Cyc_Toc_exptypes_to_c(_tmp9EE);for(0;_tmp9EF != 0;_tmp9EF=_tmp9EF->tl){Cyc_Toc_stmttypes_to_c(((
struct Cyc_Absyn_Switch_clause*)_tmp9EF->hd)->body);}goto _LL421;_LL430: {struct
Cyc_Absyn_Decl_s_struct*_tmp9F0=(struct Cyc_Absyn_Decl_s_struct*)_tmp9D6;if(
_tmp9F0->tag != 12)goto _LL432;else{_tmp9F1=_tmp9F0->f1;_tmp9F2=_tmp9F0->f2;}}
_LL431: Cyc_Toc_decltypes_to_c(_tmp9F1);Cyc_Toc_stmttypes_to_c(_tmp9F2);goto
_LL421;_LL432: {struct Cyc_Absyn_Do_s_struct*_tmp9F3=(struct Cyc_Absyn_Do_s_struct*)
_tmp9D6;if(_tmp9F3->tag != 14)goto _LL434;else{_tmp9F4=_tmp9F3->f1;_tmp9F5=_tmp9F3->f2;
_tmp9F6=_tmp9F5.f1;}}_LL433: Cyc_Toc_stmttypes_to_c(_tmp9F4);Cyc_Toc_exptypes_to_c(
_tmp9F6);goto _LL421;_LL434: {struct Cyc_Absyn_Skip_s_struct*_tmp9F7=(struct Cyc_Absyn_Skip_s_struct*)
_tmp9D6;if(_tmp9F7->tag != 0)goto _LL436;}_LL435: goto _LL437;_LL436: {struct Cyc_Absyn_Break_s_struct*
_tmp9F8=(struct Cyc_Absyn_Break_s_struct*)_tmp9D6;if(_tmp9F8->tag != 6)goto _LL438;}
_LL437: goto _LL439;_LL438: {struct Cyc_Absyn_Continue_s_struct*_tmp9F9=(struct Cyc_Absyn_Continue_s_struct*)
_tmp9D6;if(_tmp9F9->tag != 7)goto _LL43A;}_LL439: goto _LL43B;_LL43A: {struct Cyc_Absyn_Goto_s_struct*
_tmp9FA=(struct Cyc_Absyn_Goto_s_struct*)_tmp9D6;if(_tmp9FA->tag != 8)goto _LL43C;}
_LL43B: goto _LL421;_LL43C: {struct Cyc_Absyn_Fallthru_s_struct*_tmp9FB=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp9D6;if(_tmp9FB->tag != 11)goto _LL43E;}_LL43D: goto _LL43F;_LL43E: {struct Cyc_Absyn_Label_s_struct*
_tmp9FC=(struct Cyc_Absyn_Label_s_struct*)_tmp9D6;if(_tmp9FC->tag != 13)goto _LL440;}
_LL43F: goto _LL441;_LL440: {struct Cyc_Absyn_TryCatch_s_struct*_tmp9FD=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp9D6;if(_tmp9FD->tag != 15)goto _LL442;}_LL441: goto _LL443;_LL442: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp9FE=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp9D6;if(_tmp9FE->tag != 16)goto
_LL421;}_LL443:{const char*_tmpDF7;void*_tmpDF6;(_tmpDF6=0,Cyc_Tcutil_terr(s->loc,((
_tmpDF7="Cyclone statement in C code",_tag_dyneither(_tmpDF7,sizeof(char),28))),
_tag_dyneither(_tmpDF6,sizeof(void*),0)));}goto _LL421;_LL421:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude){for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpDFA;void*_tmpDF9;(_tmpDF9=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDFA="decls_to_c: not at toplevel!",
_tag_dyneither(_tmpDFA,sizeof(char),29))),_tag_dyneither(_tmpDF9,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmpA03=d->r;struct Cyc_Absyn_Vardecl*
_tmpA05;struct Cyc_Absyn_Fndecl*_tmpA07;struct Cyc_Absyn_Aggrdecl*_tmpA0D;struct
Cyc_Absyn_Datatypedecl*_tmpA0F;struct Cyc_Absyn_Enumdecl*_tmpA11;struct Cyc_Absyn_Typedefdecl*
_tmpA13;struct Cyc_List_List*_tmpA17;struct Cyc_List_List*_tmpA19;struct Cyc_List_List*
_tmpA1B;struct Cyc_List_List*_tmpA1D;_LL445: {struct Cyc_Absyn_Var_d_struct*
_tmpA04=(struct Cyc_Absyn_Var_d_struct*)_tmpA03;if(_tmpA04->tag != 0)goto _LL447;
else{_tmpA05=_tmpA04->f1;}}_LL446: {struct _tuple0*_tmpA1E=_tmpA05->name;if(
_tmpA05->sc == Cyc_Absyn_ExternC){struct _tuple0*_tmpDFB;_tmpA1E=((_tmpDFB=
_cycalloc(sizeof(*_tmpDFB)),((_tmpDFB->f1=Cyc_Absyn_Rel_n(0),((_tmpDFB->f2=(*
_tmpA1E).f2,_tmpDFB))))));}if(_tmpA05->initializer != 0){if(cinclude)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmpA05->initializer));else{Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_check_null(_tmpA05->initializer));}}{struct Cyc_Absyn_Global_b_struct
_tmpDFE;struct Cyc_Absyn_Global_b_struct*_tmpDFD;nv=Cyc_Toc_add_varmap(r,nv,
_tmpA05->name,Cyc_Absyn_varb_exp(_tmpA1E,(void*)((_tmpDFD=_cycalloc(sizeof(*
_tmpDFD)),((_tmpDFD[0]=((_tmpDFE.tag=1,((_tmpDFE.f1=_tmpA05,_tmpDFE)))),_tmpDFD)))),
0));}_tmpA05->name=_tmpA1E;_tmpA05->sc=Cyc_Toc_scope_to_c(_tmpA05->sc);_tmpA05->type=
Cyc_Toc_typ_to_c_array(_tmpA05->type);{struct Cyc_List_List*_tmpDFF;Cyc_Toc_result_decls=((
_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((_tmpDFF->hd=d,((_tmpDFF->tl=Cyc_Toc_result_decls,
_tmpDFF))))));}goto _LL444;}_LL447: {struct Cyc_Absyn_Fn_d_struct*_tmpA06=(struct
Cyc_Absyn_Fn_d_struct*)_tmpA03;if(_tmpA06->tag != 1)goto _LL449;else{_tmpA07=
_tmpA06->f1;}}_LL448: {struct _tuple0*_tmpA23=_tmpA07->name;if(_tmpA07->sc == Cyc_Absyn_ExternC){{
struct _tuple0*_tmpE00;_tmpA23=((_tmpE00=_cycalloc(sizeof(*_tmpE00)),((_tmpE00->f1=
Cyc_Absyn_Rel_n(0),((_tmpE00->f2=(*_tmpA23).f2,_tmpE00))))));}_tmpA07->sc=Cyc_Absyn_Public;}
nv=Cyc_Toc_add_varmap(r,nv,_tmpA07->name,Cyc_Absyn_var_exp(_tmpA23,0));_tmpA07->name=
_tmpA23;Cyc_Toc_fndecl_to_c(nv,_tmpA07,cinclude);{struct Cyc_List_List*_tmpE01;
Cyc_Toc_result_decls=((_tmpE01=_cycalloc(sizeof(*_tmpE01)),((_tmpE01->hd=d,((
_tmpE01->tl=Cyc_Toc_result_decls,_tmpE01))))));}goto _LL444;}_LL449: {struct Cyc_Absyn_Let_d_struct*
_tmpA08=(struct Cyc_Absyn_Let_d_struct*)_tmpA03;if(_tmpA08->tag != 2)goto _LL44B;}
_LL44A: goto _LL44C;_LL44B: {struct Cyc_Absyn_Letv_d_struct*_tmpA09=(struct Cyc_Absyn_Letv_d_struct*)
_tmpA03;if(_tmpA09->tag != 3)goto _LL44D;}_LL44C: {const char*_tmpE04;void*_tmpE03;(
_tmpE03=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE04="letdecl at toplevel",_tag_dyneither(_tmpE04,sizeof(char),20))),
_tag_dyneither(_tmpE03,sizeof(void*),0)));}_LL44D: {struct Cyc_Absyn_Region_d_struct*
_tmpA0A=(struct Cyc_Absyn_Region_d_struct*)_tmpA03;if(_tmpA0A->tag != 4)goto _LL44F;}
_LL44E: {const char*_tmpE07;void*_tmpE06;(_tmpE06=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE07="region decl at toplevel",
_tag_dyneither(_tmpE07,sizeof(char),24))),_tag_dyneither(_tmpE06,sizeof(void*),0)));}
_LL44F: {struct Cyc_Absyn_Alias_d_struct*_tmpA0B=(struct Cyc_Absyn_Alias_d_struct*)
_tmpA03;if(_tmpA0B->tag != 5)goto _LL451;}_LL450: {const char*_tmpE0A;void*_tmpE09;(
_tmpE09=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE0A="alias decl at toplevel",_tag_dyneither(_tmpE0A,sizeof(char),23))),
_tag_dyneither(_tmpE09,sizeof(void*),0)));}_LL451: {struct Cyc_Absyn_Aggr_d_struct*
_tmpA0C=(struct Cyc_Absyn_Aggr_d_struct*)_tmpA03;if(_tmpA0C->tag != 6)goto _LL453;
else{_tmpA0D=_tmpA0C->f1;}}_LL452: Cyc_Toc_aggrdecl_to_c(_tmpA0D,1);goto _LL444;
_LL453: {struct Cyc_Absyn_Datatype_d_struct*_tmpA0E=(struct Cyc_Absyn_Datatype_d_struct*)
_tmpA03;if(_tmpA0E->tag != 7)goto _LL455;else{_tmpA0F=_tmpA0E->f1;}}_LL454: if(
_tmpA0F->is_extensible)Cyc_Toc_xdatatypedecl_to_c(_tmpA0F);else{Cyc_Toc_datatypedecl_to_c(
_tmpA0F);}goto _LL444;_LL455: {struct Cyc_Absyn_Enum_d_struct*_tmpA10=(struct Cyc_Absyn_Enum_d_struct*)
_tmpA03;if(_tmpA10->tag != 8)goto _LL457;else{_tmpA11=_tmpA10->f1;}}_LL456: Cyc_Toc_enumdecl_to_c(
_tmpA11);{struct Cyc_List_List*_tmpE0B;Cyc_Toc_result_decls=((_tmpE0B=_cycalloc(
sizeof(*_tmpE0B)),((_tmpE0B->hd=d,((_tmpE0B->tl=Cyc_Toc_result_decls,_tmpE0B))))));}
goto _LL444;_LL457: {struct Cyc_Absyn_Typedef_d_struct*_tmpA12=(struct Cyc_Absyn_Typedef_d_struct*)
_tmpA03;if(_tmpA12->tag != 9)goto _LL459;else{_tmpA13=_tmpA12->f1;}}_LL458: _tmpA13->name=
_tmpA13->name;_tmpA13->tvs=0;if(_tmpA13->defn != 0){struct Cyc_Core_Opt*_tmpE0C;
_tmpA13->defn=((_tmpE0C=_cycalloc(sizeof(*_tmpE0C)),((_tmpE0C->v=Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA13->defn))->v),_tmpE0C))));}else{
switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmpA13->kind))->v)->kind){
case Cyc_Absyn_BoxKind: _LL465:{struct Cyc_Core_Opt*_tmpE0D;_tmpA13->defn=((_tmpE0D=
_cycalloc(sizeof(*_tmpE0D)),((_tmpE0D->v=Cyc_Absyn_void_star_typ(),_tmpE0D))));}
break;default: _LL466:{struct Cyc_Core_Opt*_tmpE0E;_tmpA13->defn=((_tmpE0E=
_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E->v=(void*)& Cyc_Absyn_VoidType_val,_tmpE0E))));}
break;}}{struct Cyc_List_List*_tmpE0F;Cyc_Toc_result_decls=((_tmpE0F=_cycalloc(
sizeof(*_tmpE0F)),((_tmpE0F->hd=d,((_tmpE0F->tl=Cyc_Toc_result_decls,_tmpE0F))))));}
goto _LL444;_LL459: {struct Cyc_Absyn_Porton_d_struct*_tmpA14=(struct Cyc_Absyn_Porton_d_struct*)
_tmpA03;if(_tmpA14->tag != 14)goto _LL45B;}_LL45A: goto _LL45C;_LL45B: {struct Cyc_Absyn_Portoff_d_struct*
_tmpA15=(struct Cyc_Absyn_Portoff_d_struct*)_tmpA03;if(_tmpA15->tag != 15)goto
_LL45D;}_LL45C: goto _LL444;_LL45D: {struct Cyc_Absyn_Namespace_d_struct*_tmpA16=(
struct Cyc_Absyn_Namespace_d_struct*)_tmpA03;if(_tmpA16->tag != 10)goto _LL45F;
else{_tmpA17=_tmpA16->f2;}}_LL45E: _tmpA19=_tmpA17;goto _LL460;_LL45F: {struct Cyc_Absyn_Using_d_struct*
_tmpA18=(struct Cyc_Absyn_Using_d_struct*)_tmpA03;if(_tmpA18->tag != 11)goto _LL461;
else{_tmpA19=_tmpA18->f2;}}_LL460: _tmpA1B=_tmpA19;goto _LL462;_LL461: {struct Cyc_Absyn_ExternC_d_struct*
_tmpA1A=(struct Cyc_Absyn_ExternC_d_struct*)_tmpA03;if(_tmpA1A->tag != 12)goto
_LL463;else{_tmpA1B=_tmpA1A->f1;}}_LL462: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA1B,top,
cinclude);goto _LL444;_LL463: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA1C=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmpA03;if(_tmpA1C->tag != 13)goto _LL444;
else{_tmpA1D=_tmpA1C->f1;}}_LL464: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA1D,top,1);goto
_LL444;_LL444:;};}return nv;}static void Cyc_Toc_init(){struct _DynRegionHandle*
_tmpA32;struct Cyc_Core_NewRegion _tmpA31=Cyc_Core_new_dynregion();_tmpA32=_tmpA31.dynregion;{
struct _DynRegionFrame _tmpA33;struct _RegionHandle*d=_open_dynregion(& _tmpA33,
_tmpA32);{struct Cyc_Dict_Dict*_tmpE1A;struct Cyc_Dict_Dict*_tmpE19;struct Cyc_Set_Set**
_tmpE18;struct Cyc_Dict_Dict*_tmpE17;struct Cyc_List_List**_tmpE16;struct Cyc_Toc_TocState*
_tmpE15;Cyc_Toc_toc_state=((_tmpE15=_cycalloc(sizeof(*_tmpE15)),((_tmpE15->dyn=(
struct _DynRegionHandle*)_tmpA32,((_tmpE15->tuple_types=(struct Cyc_List_List**)((
_tmpE16=_region_malloc(d,sizeof(*_tmpE16)),((_tmpE16[0]=0,_tmpE16)))),((_tmpE15->aggrs_so_far=(
struct Cyc_Dict_Dict*)((_tmpE17=_region_malloc(d,sizeof(*_tmpE17)),((_tmpE17[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE17)))),((_tmpE15->datatypes_so_far=(
struct Cyc_Set_Set**)((_tmpE18=_region_malloc(d,sizeof(*_tmpE18)),((_tmpE18[0]=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE18)))),((_tmpE15->xdatatypes_so_far=(
struct Cyc_Dict_Dict*)((_tmpE19=_region_malloc(d,sizeof(*_tmpE19)),((_tmpE19[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE19)))),((_tmpE15->qvar_tags=(
struct Cyc_Dict_Dict*)((_tmpE1A=_region_malloc(d,sizeof(*_tmpE1A)),((_tmpE1A[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple13*,struct
_tuple13*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpE1A)))),((_tmpE15->temp_labels=(
struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(
d),_tmpE15))))))))))))))));}Cyc_Toc_result_decls=0;Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=0;Cyc_Toc_total_bounds_checks=
0;Cyc_Toc_bounds_checks_eliminated=0;{struct _dyneither_ptr**_tmpE1B;Cyc_Toc_globals=
_tag_dyneither(((_tmpE1B=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpE1B[
0]=& Cyc_Toc__throw_str,((_tmpE1B[1]=& Cyc_Toc_setjmp_str,((_tmpE1B[2]=& Cyc_Toc__push_handler_str,((
_tmpE1B[3]=& Cyc_Toc__pop_handler_str,((_tmpE1B[4]=& Cyc_Toc__exn_thrown_str,((
_tmpE1B[5]=& Cyc_Toc__npop_handler_str,((_tmpE1B[6]=& Cyc_Toc__check_null_str,((
_tmpE1B[7]=& Cyc_Toc__check_known_subscript_null_str,((_tmpE1B[8]=& Cyc_Toc__check_known_subscript_notnull_str,((
_tmpE1B[9]=& Cyc_Toc__check_dyneither_subscript_str,((_tmpE1B[10]=& Cyc_Toc__dyneither_ptr_str,((
_tmpE1B[11]=& Cyc_Toc__tag_dyneither_str,((_tmpE1B[12]=& Cyc_Toc__init_dyneither_ptr_str,((
_tmpE1B[13]=& Cyc_Toc__untag_dyneither_ptr_str,((_tmpE1B[14]=& Cyc_Toc__get_dyneither_size_str,((
_tmpE1B[15]=& Cyc_Toc__get_zero_arr_size_str,((_tmpE1B[16]=& Cyc_Toc__dyneither_ptr_plus_str,((
_tmpE1B[17]=& Cyc_Toc__zero_arr_plus_str,((_tmpE1B[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((
_tmpE1B[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpE1B[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((
_tmpE1B[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpE1B[22]=& Cyc_Toc__cycalloc_str,((
_tmpE1B[23]=& Cyc_Toc__cyccalloc_str,((_tmpE1B[24]=& Cyc_Toc__cycalloc_atomic_str,((
_tmpE1B[25]=& Cyc_Toc__cyccalloc_atomic_str,((_tmpE1B[26]=& Cyc_Toc__region_malloc_str,((
_tmpE1B[27]=& Cyc_Toc__region_calloc_str,((_tmpE1B[28]=& Cyc_Toc__check_times_str,((
_tmpE1B[29]=& Cyc_Toc__new_region_str,((_tmpE1B[30]=& Cyc_Toc__push_region_str,((
_tmpE1B[31]=& Cyc_Toc__pop_region_str,((_tmpE1B[32]=& Cyc_Toc__open_dynregion_str,((
_tmpE1B[33]=& Cyc_Toc__push_dynregion_str,((_tmpE1B[34]=& Cyc_Toc__pop_dynregion_str,((
_tmpE1B[35]=& Cyc_Toc__reset_region_str,((_tmpE1B[36]=& Cyc_Toc__throw_arraybounds_str,((
_tmpE1B[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpE1B[38]=& Cyc_Toc__throw_match_str,((
_tmpE1B[39]=& Cyc_Toc__swap_word_str,((_tmpE1B[40]=& Cyc_Toc__swap_dyneither_str,
_tmpE1B)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),41);};;_pop_dynregion(d);};}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();{struct _RegionHandle _tmpA3B=
_new_region("start");struct _RegionHandle*start=& _tmpA3B;_push_region(start);Cyc_Toc_decls_to_c(
start,Cyc_Toc_empty_env(start),ds,1,0);{struct _DynRegionHandle*_tmpA3D;struct Cyc_Toc_TocState
_tmpA3C=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA3D=
_tmpA3C.dyn;Cyc_Core_free_dynregion(_tmpA3D);};{struct Cyc_List_List*_tmpA3E=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);
_npop_handler(0);return _tmpA3E;};;_pop_region(start);};}
