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
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;}
;struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
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
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*
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
zt_loc;};struct Cyc_Absyn_VoidType_struct{int tag;};struct Cyc_Absyn_Evar_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;}
;struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_struct{
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
extern struct Cyc_Absyn_HeapRgn_struct Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_VoidType_struct
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
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
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
non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_struct{int tag;}
;struct Cyc_Absyn_Var_p_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*
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
struct Cyc_Tcenv_CList{void*hd;struct Cyc_Tcenv_CList*tl;};struct Cyc_Tcenv_VarRes_struct{
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
ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const char*_tmpA2E;void*_tmpA2D;(_tmpA2D=0,
Cyc_fprintf(Cyc_stderr,((_tmpA2E="\n",_tag_dyneither(_tmpA2E,sizeof(char),2))),
_tag_dyneither(_tmpA2D,sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_struct _tmpA31;struct Cyc_Toc_Toc_Unimplemented_struct*
_tmpA30;(int)_throw((void*)((_tmpA30=_cycalloc_atomic(sizeof(*_tmpA30)),((
_tmpA30[0]=((_tmpA31.tag=Cyc_Toc_Toc_Unimplemented,_tmpA31)),_tmpA30)))));};}
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(
Cyc_stderr,fmt,ap);{const char*_tmpA34;void*_tmpA33;(_tmpA33=0,Cyc_fprintf(Cyc_stderr,((
_tmpA34="\n",_tag_dyneither(_tmpA34,sizeof(char),2))),_tag_dyneither(_tmpA33,
sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Impossible_struct
_tmpA37;struct Cyc_Toc_Toc_Impossible_struct*_tmpA36;(int)_throw((void*)((_tmpA36=
_cycalloc_atomic(sizeof(*_tmpA36)),((_tmpA36[0]=((_tmpA37.tag=Cyc_Toc_Toc_Impossible,
_tmpA37)),_tmpA36)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_struct{
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
_tmpA38;skip_stmt_opt=((_tmpA38=_cycalloc(sizeof(*_tmpA38)),((_tmpA38[0]=Cyc_Absyn_skip_stmt(
0),_tmpA38))));}return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(
void*t,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,
0);}static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Cast_e_struct
_tmpA3B;struct Cyc_Absyn_Cast_e_struct*_tmpA3A;return(void*)((_tmpA3A=_cycalloc(
sizeof(*_tmpA3A)),((_tmpA3A[0]=((_tmpA3B.tag=15,((_tmpA3B.f1=(void*)t,((_tmpA3B.f2=
e,((_tmpA3B.f3=0,((_tmpA3B.f4=Cyc_Absyn_No_coercion,_tmpA3B)))))))))),_tmpA3A))));}
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Deref_e_struct
_tmpA3E;struct Cyc_Absyn_Deref_e_struct*_tmpA3D;return(void*)((_tmpA3D=_cycalloc(
sizeof(*_tmpA3D)),((_tmpA3D[0]=((_tmpA3E.tag=21,((_tmpA3E.f1=e,_tmpA3E)))),
_tmpA3D))));}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){struct Cyc_Absyn_Subscript_e_struct _tmpA41;struct Cyc_Absyn_Subscript_e_struct*
_tmpA40;return(void*)((_tmpA40=_cycalloc(sizeof(*_tmpA40)),((_tmpA40[0]=((
_tmpA41.tag=24,((_tmpA41.f1=e1,((_tmpA41.f2=e2,_tmpA41)))))),_tmpA40))));}static
void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct
_tmpA44;struct Cyc_Absyn_StmtExp_e_struct*_tmpA43;return(void*)((_tmpA43=
_cycalloc(sizeof(*_tmpA43)),((_tmpA43[0]=((_tmpA44.tag=37,((_tmpA44.f1=s,_tmpA44)))),
_tmpA43))));}static void*Cyc_Toc_sizeoftyp_exp_r(void*t){struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpA47;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpA46;return(void*)((_tmpA46=
_cycalloc(sizeof(*_tmpA46)),((_tmpA46[0]=((_tmpA47.tag=18,((_tmpA47.f1=(void*)t,
_tmpA47)))),_tmpA46))));}static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct _tmpA4A;struct Cyc_Absyn_FnCall_e_struct*
_tmpA49;return(void*)((_tmpA49=_cycalloc(sizeof(*_tmpA49)),((_tmpA49[0]=((
_tmpA4A.tag=11,((_tmpA4A.f1=e,((_tmpA4A.f2=es,((_tmpA4A.f3=0,_tmpA4A)))))))),
_tmpA49))));}static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_struct
_tmpA4D;struct Cyc_Absyn_Exp_s_struct*_tmpA4C;return(void*)((_tmpA4C=_cycalloc(
sizeof(*_tmpA4C)),((_tmpA4C[0]=((_tmpA4D.tag=1,((_tmpA4D.f1=e,_tmpA4D)))),
_tmpA4C))));}static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2){struct Cyc_Absyn_Seq_s_struct _tmpA50;struct Cyc_Absyn_Seq_s_struct*_tmpA4F;
return(void*)((_tmpA4F=_cycalloc(sizeof(*_tmpA4F)),((_tmpA4F[0]=((_tmpA50.tag=2,((
_tmpA50.f1=s1,((_tmpA50.f2=s2,_tmpA50)))))),_tmpA4F))));}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct
_tmpA53;struct Cyc_Absyn_Conditional_e_struct*_tmpA52;return(void*)((_tmpA52=
_cycalloc(sizeof(*_tmpA52)),((_tmpA52[0]=((_tmpA53.tag=6,((_tmpA53.f1=e1,((
_tmpA53.f2=e2,((_tmpA53.f3=e3,_tmpA53)))))))),_tmpA52))));}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrMember_e_struct
_tmpA56;struct Cyc_Absyn_AggrMember_e_struct*_tmpA55;return(void*)((_tmpA55=
_cycalloc(sizeof(*_tmpA55)),((_tmpA55[0]=((_tmpA56.tag=22,((_tmpA56.f1=e,((
_tmpA56.f2=n,((_tmpA56.f3=0,((_tmpA56.f4=0,_tmpA56)))))))))),_tmpA55))));}static
void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_struct _tmpA59;struct Cyc_Absyn_AggrArrow_e_struct*
_tmpA58;return(void*)((_tmpA58=_cycalloc(sizeof(*_tmpA58)),((_tmpA58[0]=((
_tmpA59.tag=23,((_tmpA59.f1=e,((_tmpA59.f2=n,((_tmpA59.f3=0,((_tmpA59.f4=0,
_tmpA59)))))))))),_tmpA58))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmpA5C;
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpA5B;return(void*)((_tmpA5B=_cycalloc(
sizeof(*_tmpA5B)),((_tmpA5B[0]=((_tmpA5C.tag=36,((_tmpA5C.f1=tdopt,((_tmpA5C.f2=
ds,_tmpA5C)))))),_tmpA5B))));}static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_struct _tmpA5F;
struct Cyc_Absyn_Goto_s_struct*_tmpA5E;return(void*)((_tmpA5E=_cycalloc(sizeof(*
_tmpA5E)),((_tmpA5E[0]=((_tmpA5F.tag=8,((_tmpA5F.f1=v,((_tmpA5F.f2=s,_tmpA5F)))))),
_tmpA5E))));}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,{.Int_c={4,{
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
_tmpA65;const char*_tmpA64;struct Cyc_Core_Impossible_struct*_tmpA63;(int)_throw((
void*)((_tmpA63=_cycalloc(sizeof(*_tmpA63)),((_tmpA63[0]=((_tmpA65.tag=Cyc_Core_Impossible,((
_tmpA65.f1=((_tmpA64="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpA64,sizeof(char),44))),_tmpA65)))),_tmpA63)))));}}goto _LL0;
_LL3: {struct Cyc_Absyn_FloatType_struct*_tmpC9=(struct Cyc_Absyn_FloatType_struct*)
_tmpC6;if(_tmpC9->tag != 7)goto _LL5;}_LL4: function=fS->ffloat;goto _LL0;_LL5: {
struct Cyc_Absyn_DoubleType_struct*_tmpCA=(struct Cyc_Absyn_DoubleType_struct*)
_tmpC6;if(_tmpCA->tag != 8)goto _LL7;else{_tmpCB=_tmpCA->f1;}}_LL6: switch(_tmpCB){
case 1: _LL10: function=fS->flongdouble;break;default: _LL11: function=fS->fdouble;}
goto _LL0;_LL7: {struct Cyc_Absyn_PointerType_struct*_tmpCC=(struct Cyc_Absyn_PointerType_struct*)
_tmpC6;if(_tmpCC->tag != 5)goto _LL9;}_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;
_LLA: {struct Cyc_Core_Impossible_struct _tmpA72;const char*_tmpA71;void*_tmpA70[1];
struct Cyc_String_pa_struct _tmpA6F;struct Cyc_Core_Impossible_struct*_tmpA6E;(int)
_throw((void*)((_tmpA6E=_cycalloc(sizeof(*_tmpA6E)),((_tmpA6E[0]=((_tmpA72.tag=
Cyc_Core_Impossible,((_tmpA72.f1=(struct _dyneither_ptr)((_tmpA6F.tag=0,((_tmpA6F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA70[
0]=& _tmpA6F,Cyc_aprintf(((_tmpA71="impossible expression type %s (not int, float, double, or pointer)",
_tag_dyneither(_tmpA71,sizeof(char),67))),_tag_dyneither(_tmpA70,sizeof(void*),1)))))))),
_tmpA72)))),_tmpA6E)))));}_LL0:;}return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(
fS,(void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v);}struct Cyc_Absyn_Exp*
Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*
arr){void*_tmpD5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
arr->topt))->v);struct Cyc_Absyn_PtrInfo _tmpD7;void*_tmpD8;_LL14: {struct Cyc_Absyn_PointerType_struct*
_tmpD6=(struct Cyc_Absyn_PointerType_struct*)_tmpD5;if(_tmpD6->tag != 5)goto _LL16;
else{_tmpD7=_tmpD6->f1;_tmpD8=_tmpD7.elt_typ;}}_LL15: return Cyc_Toc_getFunctionType(
fS,_tmpD8);_LL16:;_LL17: {struct Cyc_Core_Impossible_struct _tmpA78;const char*
_tmpA77;struct Cyc_Core_Impossible_struct*_tmpA76;(int)_throw((void*)((_tmpA76=
_cycalloc(sizeof(*_tmpA76)),((_tmpA76[0]=((_tmpA78.tag=Cyc_Core_Impossible,((
_tmpA78.f1=((_tmpA77="impossible type (not pointer)",_tag_dyneither(_tmpA77,
sizeof(char),30))),_tmpA78)))),_tmpA76)))));}_LL13:;}struct _tuple15{struct Cyc_List_List*
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
Cyc_Absyn_conref_def)(0,_tmp100);_LL34:;_LL35: {const char*_tmpA7B;void*_tmpA7A;(
_tmpA7A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpA7B="is_nullable",_tag_dyneither(_tmpA7B,sizeof(char),12))),_tag_dyneither(
_tmpA7A,sizeof(void*),0)));}_LL31:;}static char _tmp10F[1]="";static char _tmp110[8]="*bogus*";
static struct _tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct
_dyneither_ptr tag){struct _DynRegionHandle*_tmp104;struct Cyc_Dict_Dict*_tmp105;
struct Cyc_Toc_TocState _tmp103=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp104=_tmp103.dyn;_tmp105=_tmp103.qvar_tags;{static struct _dyneither_ptr
bogus_string={_tmp110,_tmp110,_tmp110 + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
4,0}},& bogus_string};static struct _tuple13 pair={& bogus_qvar,{_tmp10F,_tmp10F,
_tmp10F + 1}};{struct _tuple13 _tmpA7C;pair=((_tmpA7C.f1=x,((_tmpA7C.f2=tag,_tmpA7C))));}{
struct _DynRegionFrame _tmp107;struct _RegionHandle*d=_open_dynregion(& _tmp107,
_tmp104);{struct _tuple0**_tmp108=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,
struct _tuple13*k))Cyc_Dict_lookup_opt)(*_tmp105,(struct _tuple13*)& pair);if(
_tmp108 != 0){struct _tuple0*_tmp109=*_tmp108;_npop_handler(0);return _tmp109;}{
struct _tuple13*_tmpA7D;struct _tuple13*_tmp10A=(_tmpA7D=_cycalloc(sizeof(*_tmpA7D)),((
_tmpA7D->f1=x,((_tmpA7D->f2=tag,_tmpA7D)))));struct _dyneither_ptr*_tmpA80;struct
_tuple0*_tmpA7F;struct _tuple0*res=(_tmpA7F=_cycalloc(sizeof(*_tmpA7F)),((_tmpA7F->f1=(*
x).f1,((_tmpA7F->f2=((_tmpA80=_cycalloc(sizeof(*_tmpA80)),((_tmpA80[0]=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*x).f2,(struct
_dyneither_ptr)tag),_tmpA80)))),_tmpA7F)))));*_tmp105=((struct Cyc_Dict_Dict(*)(
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
_npop_handler(0);return _tmp11B;}};}}{struct Cyc_Int_pa_struct _tmpA88;void*_tmpA87[
1];const char*_tmpA86;struct _dyneither_ptr*_tmpA85;struct _dyneither_ptr*xname=(
_tmpA85=_cycalloc(sizeof(*_tmpA85)),((_tmpA85[0]=(struct _dyneither_ptr)((_tmpA88.tag=
1,((_tmpA88.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA87[0]=& _tmpA88,
Cyc_aprintf(((_tmpA86="_tuple%d",_tag_dyneither(_tmpA86,sizeof(char),9))),
_tag_dyneither(_tmpA87,sizeof(void*),1)))))))),_tmpA85)));void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp11C=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmpA8B;struct Cyc_List_List*
_tmpA8A;_tmp11C=((_tmpA8A=_cycalloc(sizeof(*_tmpA8A)),((_tmpA8A->hd=((_tmpA8B=
_cycalloc(sizeof(*_tmpA8B)),((_tmpA8B->name=Cyc_Absyn_fieldname(i),((_tmpA8B->tq=
Cyc_Toc_mt_tq,((_tmpA8B->type=(void*)ts2->hd,((_tmpA8B->width=0,((_tmpA8B->attributes=
0,_tmpA8B)))))))))))),((_tmpA8A->tl=_tmp11C,_tmpA8A))))));}}_tmp11C=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp11C);{struct Cyc_Absyn_AggrdeclImpl*
_tmpA90;struct _tuple0*_tmpA8F;struct Cyc_Absyn_Aggrdecl*_tmpA8E;struct Cyc_Absyn_Aggrdecl*
_tmp11F=(_tmpA8E=_cycalloc(sizeof(*_tmpA8E)),((_tmpA8E->kind=Cyc_Absyn_StructA,((
_tmpA8E->sc=Cyc_Absyn_Public,((_tmpA8E->name=((_tmpA8F=_cycalloc(sizeof(*_tmpA8F)),((
_tmpA8F->f1=Cyc_Absyn_Rel_n(0),((_tmpA8F->f2=xname,_tmpA8F)))))),((_tmpA8E->tvs=
0,((_tmpA8E->impl=((_tmpA90=_cycalloc(sizeof(*_tmpA90)),((_tmpA90->exist_vars=0,((
_tmpA90->rgn_po=0,((_tmpA90->fields=_tmp11C,((_tmpA90->tagged=0,_tmpA90)))))))))),((
_tmpA8E->attributes=0,_tmpA8E)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*
_tmpA96;struct Cyc_Absyn_Aggr_d_struct _tmpA95;struct Cyc_List_List*_tmpA94;Cyc_Toc_result_decls=((
_tmpA94=_cycalloc(sizeof(*_tmpA94)),((_tmpA94->hd=Cyc_Absyn_new_decl((void*)((
_tmpA96=_cycalloc(sizeof(*_tmpA96)),((_tmpA96[0]=((_tmpA95.tag=6,((_tmpA95.f1=
_tmp11F,_tmpA95)))),_tmpA96)))),0),((_tmpA94->tl=Cyc_Toc_result_decls,_tmpA94))))));}{
struct _tuple16*_tmpA99;struct Cyc_List_List*_tmpA98;*_tmp113=((_tmpA98=
_region_malloc(d,sizeof(*_tmpA98)),((_tmpA98->hd=((_tmpA99=_region_malloc(d,
sizeof(*_tmpA99)),((_tmpA99->f1=x,((_tmpA99->f2=ts,_tmpA99)))))),((_tmpA98->tl=*
_tmp113,_tmpA98))))));}{void*_tmp125=x;_npop_handler(0);return _tmp125;};};};;
_pop_dynregion(d);};}struct _tuple0*Cyc_Toc_temp_var(){int _tmp12D=Cyc_Toc_temp_var_counter
++;struct _dyneither_ptr*_tmpAA6;const char*_tmpAA5;void*_tmpAA4[1];struct Cyc_Int_pa_struct
_tmpAA3;struct _tuple0*_tmpAA2;struct _tuple0*res=(_tmpAA2=_cycalloc(sizeof(*
_tmpAA2)),((_tmpAA2->f1=Cyc_Absyn_Loc_n,((_tmpAA2->f2=((_tmpAA6=_cycalloc(
sizeof(*_tmpAA6)),((_tmpAA6[0]=(struct _dyneither_ptr)((_tmpAA3.tag=1,((_tmpAA3.f1=(
unsigned int)_tmp12D,((_tmpAA4[0]=& _tmpAA3,Cyc_aprintf(((_tmpAA5="_tmp%X",
_tag_dyneither(_tmpAA5,sizeof(char),7))),_tag_dyneither(_tmpAA4,sizeof(void*),1)))))))),
_tmpAA6)))),_tmpAA2)))));return res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp134;struct Cyc_Xarray_Xarray*_tmp135;struct Cyc_Toc_TocState
_tmp133=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp134=
_tmp133.dyn;_tmp135=_tmp133.temp_labels;{struct _DynRegionFrame _tmp136;struct
_RegionHandle*d=_open_dynregion(& _tmp136,_tmp134);{int _tmp137=Cyc_Toc_fresh_label_counter
++;if(_tmp137 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp135)){
struct _dyneither_ptr*_tmp138=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp135,_tmp137);_npop_handler(0);return _tmp138;}{struct Cyc_Int_pa_struct
_tmpAAE;void*_tmpAAD[1];const char*_tmpAAC;struct _dyneither_ptr*_tmpAAB;struct
_dyneither_ptr*res=(_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB[0]=(struct
_dyneither_ptr)((_tmpAAE.tag=1,((_tmpAAE.f1=(unsigned int)_tmp137,((_tmpAAD[0]=&
_tmpAAE,Cyc_aprintf(((_tmpAAC="_LL%X",_tag_dyneither(_tmpAAC,sizeof(char),6))),
_tag_dyneither(_tmpAAD,sizeof(void*),1)))))))),_tmpAAB)));if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp135,res)!= _tmp137){const char*
_tmpAB1;void*_tmpAB0;(_tmpAB0=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB1="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmpAB1,sizeof(char),43))),_tag_dyneither(_tmpAB0,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp13B=res;_npop_handler(0);return _tmp13B;};};};
_pop_dynregion(d);};}static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*
td,struct _tuple0*name){int ans=0;struct Cyc_List_List*_tmp140=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((
struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp140))->hd)->name)
!= 0){++ ans;_tmp140=_tmp140->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple7*Cyc_Toc_arg_to_c(struct
_tuple7*a){struct Cyc_Core_Opt*_tmp142;struct Cyc_Absyn_Tqual _tmp143;void*_tmp144;
struct _tuple7 _tmp141=*a;_tmp142=_tmp141.f1;_tmp143=_tmp141.f2;_tmp144=_tmp141.f3;{
struct _tuple7*_tmpAB2;return(_tmpAB2=_cycalloc(sizeof(*_tmpAB2)),((_tmpAB2->f1=
_tmp142,((_tmpAB2->f2=_tmp143,((_tmpAB2->f3=Cyc_Toc_typ_to_c(_tmp144),_tmpAB2)))))));};}
static struct _tuple9*Cyc_Toc_typ_to_c_f(struct _tuple9*x){struct Cyc_Absyn_Tqual
_tmp147;void*_tmp148;struct _tuple9 _tmp146=*x;_tmp147=_tmp146.f1;_tmp148=_tmp146.f2;{
struct _tuple9*_tmpAB3;return(_tmpAB3=_cycalloc(sizeof(*_tmpAB3)),((_tmpAB3->f1=
_tmp147,((_tmpAB3->f2=Cyc_Toc_typ_to_c(_tmp148),_tmpAB3)))));};}static void*Cyc_Toc_typ_to_c_array(
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
f){struct Cyc_Absyn_Aggrfield*_tmpAB4;return(_tmpAB4=_cycalloc(sizeof(*_tmpAB4)),((
_tmpAB4->name=f->name,((_tmpAB4->tq=Cyc_Toc_mt_tq,((_tmpAB4->type=Cyc_Toc_typ_to_c(
f->type),((_tmpAB4->width=f->width,((_tmpAB4->attributes=f->attributes,_tmpAB4)))))))))));}
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){return;}static void*Cyc_Toc_char_star_typ(){
static void**cs=0;if(cs == 0){void**_tmpAB5;cs=((_tmpAB5=_cycalloc(sizeof(*_tmpAB5)),((
_tmpAB5[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpAB5))));}return*cs;}static void*Cyc_Toc_rgn_typ(){
static void**r=0;if(r == 0){void**_tmpAB6;r=((_tmpAB6=_cycalloc(sizeof(*_tmpAB6)),((
_tmpAB6[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),
_tmpAB6))));}return*r;}static void*Cyc_Toc_dyn_rgn_typ(){static void**r=0;if(r == 0){
void**_tmpAB7;r=((_tmpAB7=_cycalloc(sizeof(*_tmpAB7)),((_tmpAB7[0]=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpAB7))));}return*
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
_tmp192;void**_tmp193;void*_tmp196;_LL3E: {struct Cyc_Absyn_VoidType_struct*
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
const char*_tmpAB8;return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp16A->name,((
_tmpAB8="_struct",_tag_dyneither(_tmpAB8,sizeof(char),8)))));}_LL4A: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp16B=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp15A;if(_tmp16B->tag != 4)
goto _LL4C;}_LL4B: {const char*_tmpABB;void*_tmpABA;(_tmpABA=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpABB="unresolved DatatypeFieldType",
_tag_dyneither(_tmpABB,sizeof(char),29))),_tag_dyneither(_tmpABA,sizeof(void*),0)));}
_LL4C: {struct Cyc_Absyn_PointerType_struct*_tmp16C=(struct Cyc_Absyn_PointerType_struct*)
_tmp15A;if(_tmp16C->tag != 5)goto _LL4E;else{_tmp16D=_tmp16C->f1;_tmp16E=_tmp16D.elt_typ;
_tmp16F=_tmp16D.elt_tq;_tmp170=_tmp16D.ptr_atts;_tmp171=_tmp170.bounds;}}_LL4D:
_tmp16E=Cyc_Toc_typ_to_c_array(_tmp16E);{void*_tmp1A1=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp171);_LL77: {
struct Cyc_Absyn_DynEither_b_struct*_tmp1A2=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp1A1;if(_tmp1A2->tag != 0)goto _LL79;}_LL78: return Cyc_Toc_dyneither_ptr_typ;
_LL79:;_LL7A: return Cyc_Absyn_star_typ(_tmp16E,(void*)& Cyc_Absyn_HeapRgn_val,
_tmp16F,Cyc_Absyn_false_conref);_LL76:;};_LL4E: {struct Cyc_Absyn_IntType_struct*
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
_tmp181=_tmp17C.attributes;}}_LL57: {struct Cyc_List_List*_tmp1A3=0;for(0;_tmp181
!= 0;_tmp181=_tmp181->tl){void*_tmp1A4=(void*)_tmp181->hd;_LL7C: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp1A5=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp1A4;if(_tmp1A5->tag != 4)goto
_LL7E;}_LL7D: goto _LL7F;_LL7E: {struct Cyc_Absyn_Const_att_struct*_tmp1A6=(struct
Cyc_Absyn_Const_att_struct*)_tmp1A4;if(_tmp1A6->tag != 5)goto _LL80;}_LL7F: goto
_LL81;_LL80: {struct Cyc_Absyn_Format_att_struct*_tmp1A7=(struct Cyc_Absyn_Format_att_struct*)
_tmp1A4;if(_tmp1A7->tag != 19)goto _LL82;}_LL81: continue;_LL82: {struct Cyc_Absyn_Noliveunique_att_struct*
_tmp1A8=(struct Cyc_Absyn_Noliveunique_att_struct*)_tmp1A4;if(_tmp1A8->tag != 21)
goto _LL84;}_LL83: continue;_LL84: {struct Cyc_Absyn_Initializes_att_struct*_tmp1A9=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp1A4;if(_tmp1A9->tag != 20)goto _LL86;}
_LL85: continue;_LL86:;_LL87:{struct Cyc_List_List*_tmpABC;_tmp1A3=((_tmpABC=
_cycalloc(sizeof(*_tmpABC)),((_tmpABC->hd=(void*)_tmp181->hd,((_tmpABC->tl=
_tmp1A3,_tmpABC))))));}goto _LL7B;_LL7B:;}{struct Cyc_List_List*_tmp1AB=((struct
Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_arg_to_c,_tmp17E);if(_tmp180 != 0){void*_tmp1AC=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp180->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple7*_tmpABD;struct _tuple7*_tmp1AD=(_tmpABD=_cycalloc(sizeof(*_tmpABD)),((
_tmpABD->f1=_tmp180->name,((_tmpABD->f2=_tmp180->tq,((_tmpABD->f3=_tmp1AC,
_tmpABD)))))));struct Cyc_List_List*_tmpABE;_tmp1AB=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1AB,((
_tmpABE=_cycalloc(sizeof(*_tmpABE)),((_tmpABE->hd=_tmp1AD,((_tmpABE->tl=0,
_tmpABE)))))));}{struct Cyc_Absyn_FnType_struct _tmpAC4;struct Cyc_Absyn_FnInfo
_tmpAC3;struct Cyc_Absyn_FnType_struct*_tmpAC2;return(void*)((_tmpAC2=_cycalloc(
sizeof(*_tmpAC2)),((_tmpAC2[0]=((_tmpAC4.tag=10,((_tmpAC4.f1=((_tmpAC3.tvars=0,((
_tmpAC3.effect=0,((_tmpAC3.ret_typ=Cyc_Toc_typ_to_c(_tmp17D),((_tmpAC3.args=
_tmp1AB,((_tmpAC3.c_varargs=_tmp17F,((_tmpAC3.cyc_varargs=0,((_tmpAC3.rgn_po=0,((
_tmpAC3.attributes=_tmp1A3,_tmpAC3)))))))))))))))),_tmpAC4)))),_tmpAC2))));};};}
_LL58: {struct Cyc_Absyn_TupleType_struct*_tmp182=(struct Cyc_Absyn_TupleType_struct*)
_tmp15A;if(_tmp182->tag != 11)goto _LL5A;else{_tmp183=_tmp182->f1;}}_LL59: _tmp183=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp183);return Cyc_Toc_add_tuple_type(_tmp183);
_LL5A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp184=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp15A;if(_tmp184->tag != 13)goto _LL5C;else{_tmp185=_tmp184->f1;_tmp186=_tmp184->f2;}}
_LL5B: {struct Cyc_Absyn_AnonAggrType_struct _tmpAC7;struct Cyc_Absyn_AnonAggrType_struct*
_tmpAC6;return(void*)((_tmpAC6=_cycalloc(sizeof(*_tmpAC6)),((_tmpAC6[0]=((
_tmpAC7.tag=13,((_tmpAC7.f1=_tmp185,((_tmpAC7.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp186),_tmpAC7)))))),_tmpAC6))));}_LL5C: {
struct Cyc_Absyn_AggrType_struct*_tmp187=(struct Cyc_Absyn_AggrType_struct*)
_tmp15A;if(_tmp187->tag != 12)goto _LL5E;else{_tmp188=_tmp187->f1;_tmp189=_tmp188.aggr_info;
_tmp18A=_tmp188.targs;}}_LL5D:{union Cyc_Absyn_AggrInfoU _tmp1B5=_tmp189;struct
_tuple2 _tmp1B6;_LL89: if((_tmp1B5.UnknownAggr).tag != 1)goto _LL8B;_tmp1B6=(struct
_tuple2)(_tmp1B5.UnknownAggr).val;_LL8A: return t;_LL8B:;_LL8C: goto _LL88;_LL88:;}{
struct Cyc_Absyn_Aggrdecl*_tmp1B7=Cyc_Absyn_get_known_aggrdecl(_tmp189);if(
_tmp1B7->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(_tmp1B7->name,Cyc_Absyn_unionq_typ);
else{return Cyc_Toc_aggrdecl_type(_tmp1B7->name,Cyc_Absyn_strctq);}};_LL5E: {
struct Cyc_Absyn_EnumType_struct*_tmp18B=(struct Cyc_Absyn_EnumType_struct*)
_tmp15A;if(_tmp18B->tag != 14)goto _LL60;else{_tmp18C=_tmp18B->f1;}}_LL5F: return t;
_LL60: {struct Cyc_Absyn_AnonEnumType_struct*_tmp18D=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp15A;if(_tmp18D->tag != 15)goto _LL62;else{_tmp18E=_tmp18D->f1;}}_LL61: Cyc_Toc_enumfields_to_c(
_tmp18E);return t;_LL62: {struct Cyc_Absyn_TypedefType_struct*_tmp18F=(struct Cyc_Absyn_TypedefType_struct*)
_tmp15A;if(_tmp18F->tag != 18)goto _LL64;else{_tmp190=_tmp18F->f1;_tmp191=_tmp18F->f2;
_tmp192=_tmp18F->f3;_tmp193=_tmp18F->f4;}}_LL63: if(_tmp193 == 0  || Cyc_noexpand_r){
if(_tmp191 != 0){struct Cyc_Absyn_TypedefType_struct _tmpACA;struct Cyc_Absyn_TypedefType_struct*
_tmpAC9;return(void*)((_tmpAC9=_cycalloc(sizeof(*_tmpAC9)),((_tmpAC9[0]=((
_tmpACA.tag=18,((_tmpACA.f1=_tmp190,((_tmpACA.f2=0,((_tmpACA.f3=_tmp192,((
_tmpACA.f4=0,_tmpACA)))))))))),_tmpAC9))));}else{return t;}}else{struct Cyc_Absyn_TypedefType_struct
_tmpAD0;void**_tmpACF;struct Cyc_Absyn_TypedefType_struct*_tmpACE;return(void*)((
_tmpACE=_cycalloc(sizeof(*_tmpACE)),((_tmpACE[0]=((_tmpAD0.tag=18,((_tmpAD0.f1=
_tmp190,((_tmpAD0.f2=0,((_tmpAD0.f3=_tmp192,((_tmpAD0.f4=((_tmpACF=_cycalloc(
sizeof(*_tmpACF)),((_tmpACF[0]=Cyc_Toc_typ_to_c_array(*_tmp193),_tmpACF)))),
_tmpAD0)))))))))),_tmpACE))));}_LL64: {struct Cyc_Absyn_TagType_struct*_tmp194=(
struct Cyc_Absyn_TagType_struct*)_tmp15A;if(_tmp194->tag != 20)goto _LL66;}_LL65:
return Cyc_Absyn_uint_typ;_LL66: {struct Cyc_Absyn_RgnHandleType_struct*_tmp195=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp15A;if(_tmp195->tag != 16)goto _LL68;
else{_tmp196=(void*)_tmp195->f1;}}_LL67: return Cyc_Toc_rgn_typ();_LL68: {struct
Cyc_Absyn_DynRgnType_struct*_tmp197=(struct Cyc_Absyn_DynRgnType_struct*)_tmp15A;
if(_tmp197->tag != 17)goto _LL6A;}_LL69: return Cyc_Toc_dyn_rgn_typ();_LL6A: {struct
Cyc_Absyn_HeapRgn_struct*_tmp198=(struct Cyc_Absyn_HeapRgn_struct*)_tmp15A;if(
_tmp198->tag != 21)goto _LL6C;}_LL6B: {const char*_tmpAD3;void*_tmpAD2;(_tmpAD2=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAD3="Toc::typ_to_c: type translation passed the heap region",_tag_dyneither(
_tmpAD3,sizeof(char),55))),_tag_dyneither(_tmpAD2,sizeof(void*),0)));}_LL6C: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp199=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp15A;if(_tmp199->tag != 22)goto _LL6E;}_LL6D: {const char*_tmpAD6;void*_tmpAD5;(
_tmpAD5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAD6="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpAD6,sizeof(char),57))),_tag_dyneither(_tmpAD5,sizeof(void*),0)));}
_LL6E: {struct Cyc_Absyn_AccessEff_struct*_tmp19A=(struct Cyc_Absyn_AccessEff_struct*)
_tmp15A;if(_tmp19A->tag != 23)goto _LL70;}_LL6F: goto _LL71;_LL70: {struct Cyc_Absyn_JoinEff_struct*
_tmp19B=(struct Cyc_Absyn_JoinEff_struct*)_tmp15A;if(_tmp19B->tag != 24)goto _LL72;}
_LL71: goto _LL73;_LL72: {struct Cyc_Absyn_RgnsEff_struct*_tmp19C=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp15A;if(_tmp19C->tag != 25)goto _LL74;}_LL73: {const char*_tmpAD9;void*_tmpAD8;(
_tmpAD8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAD9="Toc::typ_to_c: type translation passed an effect",_tag_dyneither(
_tmpAD9,sizeof(char),49))),_tag_dyneither(_tmpAD8,sizeof(void*),0)));}_LL74: {
struct Cyc_Absyn_ValueofType_struct*_tmp19D=(struct Cyc_Absyn_ValueofType_struct*)
_tmp15A;if(_tmp19D->tag != 19)goto _LL3D;}_LL75: {const char*_tmpADC;void*_tmpADB;(
_tmpADB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpADC="Toc::typ_to_c: type translation passed a valueof_t",_tag_dyneither(
_tmpADC,sizeof(char),51))),_tag_dyneither(_tmpADB,sizeof(void*),0)));}_LL3D:;}
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*l){void*_tmp1C5=t;struct Cyc_Absyn_ArrayInfo _tmp1C7;
void*_tmp1C8;struct Cyc_Absyn_Tqual _tmp1C9;_LL8E: {struct Cyc_Absyn_ArrayType_struct*
_tmp1C6=(struct Cyc_Absyn_ArrayType_struct*)_tmp1C5;if(_tmp1C6->tag != 9)goto _LL90;
else{_tmp1C7=_tmp1C6->f1;_tmp1C8=_tmp1C7.elt_type;_tmp1C9=_tmp1C7.tq;}}_LL8F:
return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp1C8,(void*)& Cyc_Absyn_HeapRgn_val,
_tmp1C9,Cyc_Absyn_false_conref),e);_LL90:;_LL91: return Cyc_Toc_cast_it(t,e);_LL8D:;}
static int Cyc_Toc_atomic_typ(void*t){void*_tmp1CA=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_ArrayInfo _tmp1D5;void*_tmp1D6;struct Cyc_Absyn_AggrInfo _tmp1D8;union Cyc_Absyn_AggrInfoU
_tmp1D9;struct Cyc_List_List*_tmp1DB;struct Cyc_Absyn_DatatypeFieldInfo _tmp1DD;
union Cyc_Absyn_DatatypeFieldInfoU _tmp1DE;struct _tuple1 _tmp1DF;struct Cyc_Absyn_Datatypedecl*
_tmp1E0;struct Cyc_Absyn_Datatypefield*_tmp1E1;struct Cyc_List_List*_tmp1E3;_LL93: {
struct Cyc_Absyn_VoidType_struct*_tmp1CB=(struct Cyc_Absyn_VoidType_struct*)
_tmp1CA;if(_tmp1CB->tag != 0)goto _LL95;}_LL94: return 1;_LL95: {struct Cyc_Absyn_VarType_struct*
_tmp1CC=(struct Cyc_Absyn_VarType_struct*)_tmp1CA;if(_tmp1CC->tag != 2)goto _LL97;}
_LL96: return 0;_LL97: {struct Cyc_Absyn_IntType_struct*_tmp1CD=(struct Cyc_Absyn_IntType_struct*)
_tmp1CA;if(_tmp1CD->tag != 6)goto _LL99;}_LL98: goto _LL9A;_LL99: {struct Cyc_Absyn_EnumType_struct*
_tmp1CE=(struct Cyc_Absyn_EnumType_struct*)_tmp1CA;if(_tmp1CE->tag != 14)goto _LL9B;}
_LL9A: goto _LL9C;_LL9B: {struct Cyc_Absyn_AnonEnumType_struct*_tmp1CF=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp1CA;if(_tmp1CF->tag != 15)goto _LL9D;}_LL9C: goto _LL9E;_LL9D: {struct Cyc_Absyn_FloatType_struct*
_tmp1D0=(struct Cyc_Absyn_FloatType_struct*)_tmp1CA;if(_tmp1D0->tag != 7)goto _LL9F;}
_LL9E: goto _LLA0;_LL9F: {struct Cyc_Absyn_DoubleType_struct*_tmp1D1=(struct Cyc_Absyn_DoubleType_struct*)
_tmp1CA;if(_tmp1D1->tag != 8)goto _LLA1;}_LLA0: goto _LLA2;_LLA1: {struct Cyc_Absyn_FnType_struct*
_tmp1D2=(struct Cyc_Absyn_FnType_struct*)_tmp1CA;if(_tmp1D2->tag != 10)goto _LLA3;}
_LLA2: goto _LLA4;_LLA3: {struct Cyc_Absyn_TagType_struct*_tmp1D3=(struct Cyc_Absyn_TagType_struct*)
_tmp1CA;if(_tmp1D3->tag != 20)goto _LLA5;}_LLA4: return 1;_LLA5: {struct Cyc_Absyn_ArrayType_struct*
_tmp1D4=(struct Cyc_Absyn_ArrayType_struct*)_tmp1CA;if(_tmp1D4->tag != 9)goto _LLA7;
else{_tmp1D5=_tmp1D4->f1;_tmp1D6=_tmp1D5.elt_type;}}_LLA6: return Cyc_Toc_atomic_typ(
_tmp1D6);_LLA7: {struct Cyc_Absyn_AggrType_struct*_tmp1D7=(struct Cyc_Absyn_AggrType_struct*)
_tmp1CA;if(_tmp1D7->tag != 12)goto _LLA9;else{_tmp1D8=_tmp1D7->f1;_tmp1D9=_tmp1D8.aggr_info;}}
_LLA8:{union Cyc_Absyn_AggrInfoU _tmp1E8=_tmp1D9;struct _tuple2 _tmp1E9;_LLBA: if((
_tmp1E8.UnknownAggr).tag != 1)goto _LLBC;_tmp1E9=(struct _tuple2)(_tmp1E8.UnknownAggr).val;
_LLBB: return 0;_LLBC:;_LLBD: goto _LLB9;_LLB9:;}{struct Cyc_Absyn_Aggrdecl*_tmp1EA=
Cyc_Absyn_get_known_aggrdecl(_tmp1D9);if(_tmp1EA->impl == 0)return 0;{struct Cyc_List_List*
_tmp1EB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1EA->impl))->fields;for(
0;_tmp1EB != 0;_tmp1EB=_tmp1EB->tl){if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)
_tmp1EB->hd)->type))return 0;}}return 1;};_LLA9: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp1DA=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp1CA;if(_tmp1DA->tag != 13)goto
_LLAB;else{_tmp1DB=_tmp1DA->f2;}}_LLAA: for(0;_tmp1DB != 0;_tmp1DB=_tmp1DB->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1DB->hd)->type))return 0;}
return 1;_LLAB: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp1DC=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp1CA;if(_tmp1DC->tag != 4)goto _LLAD;else{_tmp1DD=_tmp1DC->f1;_tmp1DE=_tmp1DD.field_info;
if((_tmp1DE.KnownDatatypefield).tag != 2)goto _LLAD;_tmp1DF=(struct _tuple1)(
_tmp1DE.KnownDatatypefield).val;_tmp1E0=_tmp1DF.f1;_tmp1E1=_tmp1DF.f2;}}_LLAC:
_tmp1E3=_tmp1E1->typs;goto _LLAE;_LLAD: {struct Cyc_Absyn_TupleType_struct*_tmp1E2=(
struct Cyc_Absyn_TupleType_struct*)_tmp1CA;if(_tmp1E2->tag != 11)goto _LLAF;else{
_tmp1E3=_tmp1E2->f1;}}_LLAE: for(0;_tmp1E3 != 0;_tmp1E3=_tmp1E3->tl){if(!Cyc_Toc_atomic_typ((*((
struct _tuple9*)_tmp1E3->hd)).f2))return 0;}return 1;_LLAF: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1E4=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1CA;if(_tmp1E4->tag != 3)goto
_LLB1;}_LLB0: goto _LLB2;_LLB1: {struct Cyc_Absyn_PointerType_struct*_tmp1E5=(
struct Cyc_Absyn_PointerType_struct*)_tmp1CA;if(_tmp1E5->tag != 5)goto _LLB3;}_LLB2:
goto _LLB4;_LLB3: {struct Cyc_Absyn_DynRgnType_struct*_tmp1E6=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp1CA;if(_tmp1E6->tag != 17)goto _LLB5;}_LLB4: goto _LLB6;_LLB5: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp1E7=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp1CA;if(_tmp1E7->tag != 16)goto
_LLB7;}_LLB6: return 0;_LLB7:;_LLB8: {const char*_tmpAE0;void*_tmpADF[1];struct Cyc_String_pa_struct
_tmpADE;(_tmpADE.tag=0,((_tmpADE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpADF[0]=& _tmpADE,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE0="atomic_typ:  bad type %s",
_tag_dyneither(_tmpAE0,sizeof(char),25))),_tag_dyneither(_tmpADF,sizeof(void*),1)))))));}
_LL92:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp1EF=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp1F1;void*_tmp1F2;_LLBF: {struct Cyc_Absyn_PointerType_struct*
_tmp1F0=(struct Cyc_Absyn_PointerType_struct*)_tmp1EF;if(_tmp1F0->tag != 5)goto
_LLC1;else{_tmp1F1=_tmp1F0->f1;_tmp1F2=_tmp1F1.elt_typ;}}_LLC0: {void*_tmp1F3=
Cyc_Tcutil_compress(_tmp1F2);_LLC4: {struct Cyc_Absyn_VoidType_struct*_tmp1F4=(
struct Cyc_Absyn_VoidType_struct*)_tmp1F3;if(_tmp1F4->tag != 0)goto _LLC6;}_LLC5:
return 1;_LLC6:;_LLC7: return 0;_LLC3:;}_LLC1:;_LLC2: return 0;_LLBE:;}static int Cyc_Toc_is_tvar(
void*t){void*_tmp1F5=Cyc_Tcutil_compress(t);_LLC9: {struct Cyc_Absyn_VarType_struct*
_tmp1F6=(struct Cyc_Absyn_VarType_struct*)_tmp1F5;if(_tmp1F6->tag != 2)goto _LLCB;}
_LLCA: return 1;_LLCB:;_LLCC: return 0;_LLC8:;}static int Cyc_Toc_is_void_star_or_tvar(
void*t){return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_tvar(t);}static int Cyc_Toc_is_poly_field(
void*t,struct _dyneither_ptr*f){void*_tmp1F7=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp1F9;union Cyc_Absyn_AggrInfoU _tmp1FA;struct Cyc_List_List*_tmp1FC;_LLCE: {
struct Cyc_Absyn_AggrType_struct*_tmp1F8=(struct Cyc_Absyn_AggrType_struct*)
_tmp1F7;if(_tmp1F8->tag != 12)goto _LLD0;else{_tmp1F9=_tmp1F8->f1;_tmp1FA=_tmp1F9.aggr_info;}}
_LLCF: {struct Cyc_Absyn_Aggrdecl*_tmp1FD=Cyc_Absyn_get_known_aggrdecl(_tmp1FA);
if(_tmp1FD->impl == 0){const char*_tmpAE3;void*_tmpAE2;(_tmpAE2=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE3="is_poly_field: type missing fields",
_tag_dyneither(_tmpAE3,sizeof(char),35))),_tag_dyneither(_tmpAE2,sizeof(void*),0)));}
_tmp1FC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1FD->impl))->fields;goto
_LLD1;}_LLD0: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1FB=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1F7;if(_tmp1FB->tag != 13)goto _LLD2;else{_tmp1FC=_tmp1FB->f2;}}_LLD1: {struct
Cyc_Absyn_Aggrfield*_tmp200=Cyc_Absyn_lookup_field(_tmp1FC,f);if(_tmp200 == 0){
const char*_tmpAE7;void*_tmpAE6[1];struct Cyc_String_pa_struct _tmpAE5;(_tmpAE5.tag=
0,((_tmpAE5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpAE6[0]=&
_tmpAE5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAE7="is_poly_field: bad field %s",_tag_dyneither(_tmpAE7,sizeof(char),28))),
_tag_dyneither(_tmpAE6,sizeof(void*),1)))))));}return Cyc_Toc_is_void_star_or_tvar(
_tmp200->type);}_LLD2:;_LLD3: {const char*_tmpAEB;void*_tmpAEA[1];struct Cyc_String_pa_struct
_tmpAE9;(_tmpAE9.tag=0,((_tmpAE9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpAEA[0]=& _tmpAE9,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAEB="is_poly_field: bad type %s",
_tag_dyneither(_tmpAEB,sizeof(char),27))),_tag_dyneither(_tmpAEA,sizeof(void*),1)))))));}
_LLCD:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmp207=e->r;
struct Cyc_Absyn_Exp*_tmp209;struct _dyneither_ptr*_tmp20A;struct Cyc_Absyn_Exp*
_tmp20C;struct _dyneither_ptr*_tmp20D;_LLD5: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp208=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp207;if(_tmp208->tag != 22)goto
_LLD7;else{_tmp209=_tmp208->f1;_tmp20A=_tmp208->f2;}}_LLD6: return Cyc_Toc_is_poly_field((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp209->topt))->v,_tmp20A);_LLD7: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp20B=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp207;if(_tmp20B->tag != 23)goto _LLD9;else{_tmp20C=_tmp20B->f1;_tmp20D=_tmp20B->f2;}}
_LLD8: {void*_tmp20E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp20C->topt))->v);struct Cyc_Absyn_PtrInfo _tmp210;void*_tmp211;
_LLDC: {struct Cyc_Absyn_PointerType_struct*_tmp20F=(struct Cyc_Absyn_PointerType_struct*)
_tmp20E;if(_tmp20F->tag != 5)goto _LLDE;else{_tmp210=_tmp20F->f1;_tmp211=_tmp210.elt_typ;}}
_LLDD: return Cyc_Toc_is_poly_field(_tmp211,_tmp20D);_LLDE:;_LLDF: {const char*
_tmpAEF;void*_tmpAEE[1];struct Cyc_String_pa_struct _tmpAED;(_tmpAED.tag=0,((
_tmpAED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp20C->topt))->v)),((_tmpAEE[0]=&
_tmpAED,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAEF="is_poly_project: bad type %s",_tag_dyneither(_tmpAEF,sizeof(char),29))),
_tag_dyneither(_tmpAEE,sizeof(void*),1)))))));}_LLDB:;}_LLD9:;_LLDA: return 0;
_LLD4:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpAF0;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((
_tmpAF0=_cycalloc(sizeof(*_tmpAF0)),((_tmpAF0->hd=s,((_tmpAF0->tl=0,_tmpAF0)))))),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpAF1;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((
_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1->hd=s,((_tmpAF1->tl=0,_tmpAF1)))))),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(
s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmpAF2[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((
_tmpAF2[1]=s,((_tmpAF2[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpAF2,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,
struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*
_tmpAF3[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpAF3[1]=n,((
_tmpAF3[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAF3,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{struct Cyc_Absyn_Exp*
_tmpAF4[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpAF4[1]=n,((
_tmpAF4[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAF4,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n){struct Cyc_Absyn_Exp*_tmpAF5[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((
_tmpAF5[2]=n,((_tmpAF5[1]=s,((_tmpAF5[0]=rgn,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAF5,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmpAF6;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmpAF6=_cycalloc(sizeof(*_tmpAF6)),((_tmpAF6->hd=e,((_tmpAF6->tl=0,_tmpAF6)))))),
0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){struct Cyc_Absyn_Exp*
_tmpAF7[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpAF7,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*
_tmp21D=e->r;union Cyc_Absyn_Cnst _tmp21F;struct _dyneither_ptr _tmp220;_LLE1: {
struct Cyc_Absyn_Const_e_struct*_tmp21E=(struct Cyc_Absyn_Const_e_struct*)_tmp21D;
if(_tmp21E->tag != 0)goto _LLE3;else{_tmp21F=_tmp21E->f1;if((_tmp21F.String_c).tag
!= 7)goto _LLE3;_tmp220=(struct _dyneither_ptr)(_tmp21F.String_c).val;}}_LLE2:
is_string=1;goto _LLE0;_LLE3:;_LLE4: goto _LLE0;_LLE0:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple0*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_struct*_tmpAFD;struct
Cyc_Absyn_Var_d_struct _tmpAFC;struct Cyc_List_List*_tmpAFB;Cyc_Toc_result_decls=((
_tmpAFB=_cycalloc(sizeof(*_tmpAFB)),((_tmpAFB->hd=Cyc_Absyn_new_decl((void*)((
_tmpAFD=_cycalloc(sizeof(*_tmpAFD)),((_tmpAFD[0]=((_tmpAFC.tag=0,((_tmpAFC.f1=vd,
_tmpAFC)))),_tmpAFD)))),0),((_tmpAFB->tl=Cyc_Toc_result_decls,_tmpAFB))))));}
xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*urm_exp;{struct _tuple15*_tmpB04;
struct _tuple15*_tmpB03;struct _tuple15*_tmpB02;struct _tuple15*_tmpB01[3];urm_exp=
Cyc_Absyn_unresolvedmem_exp(0,((_tmpB01[2]=((_tmpB02=_cycalloc(sizeof(*_tmpB02)),((
_tmpB02->f1=0,((_tmpB02->f2=xplussz,_tmpB02)))))),((_tmpB01[1]=((_tmpB03=
_cycalloc(sizeof(*_tmpB03)),((_tmpB03->f1=0,((_tmpB03->f2=xexp,_tmpB03)))))),((
_tmpB01[0]=((_tmpB04=_cycalloc(sizeof(*_tmpB04)),((_tmpB04->f1=0,((_tmpB04->f2=
xexp,_tmpB04)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB01,sizeof(struct _tuple15*),3)))))))),0);}return urm_exp;};};}
struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*
binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct
_dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;struct _RegionHandle*rgn;};
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp229;int
_tmp22A;struct Cyc_Toc_Env*_tmp228=nv;_tmp229=*_tmp228;_tmp22A=_tmp229.toplevel;
return _tmp22A;}static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*
nv,struct _tuple0*x){struct Cyc_Toc_Env _tmp22C;struct Cyc_Dict_Dict _tmp22D;struct
Cyc_Toc_Env*_tmp22B=nv;_tmp22C=*_tmp22B;_tmp22D=_tmp22C.varmap;return((struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp22D,x);}static struct
Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){struct Cyc_Toc_Env*_tmpB05;
return(_tmpB05=_region_malloc(r,sizeof(*_tmpB05)),((_tmpB05->break_lab=(struct
_dyneither_ptr**)0,((_tmpB05->continue_lab=(struct _dyneither_ptr**)0,((_tmpB05->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpB05->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp),((_tmpB05->toplevel=(int)1,((_tmpB05->rgn=(struct
_RegionHandle*)r,_tmpB05)))))))))))));}static struct Cyc_Toc_Env*Cyc_Toc_share_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp230;struct
_dyneither_ptr**_tmp231;struct _dyneither_ptr**_tmp232;struct Cyc_Toc_FallthruInfo*
_tmp233;struct Cyc_Dict_Dict _tmp234;int _tmp235;struct Cyc_Toc_Env*_tmp22F=e;
_tmp230=*_tmp22F;_tmp231=_tmp230.break_lab;_tmp232=_tmp230.continue_lab;_tmp233=
_tmp230.fallthru_info;_tmp234=_tmp230.varmap;_tmp235=_tmp230.toplevel;{struct Cyc_Toc_Env*
_tmpB06;return(_tmpB06=_region_malloc(r,sizeof(*_tmpB06)),((_tmpB06->break_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp231),((_tmpB06->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp232),((_tmpB06->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp233,((_tmpB06->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp234),((_tmpB06->toplevel=(int)_tmp235,((_tmpB06->rgn=(struct _RegionHandle*)
r,_tmpB06)))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct
_RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp238;struct
_dyneither_ptr**_tmp239;struct _dyneither_ptr**_tmp23A;struct Cyc_Toc_FallthruInfo*
_tmp23B;struct Cyc_Dict_Dict _tmp23C;int _tmp23D;struct Cyc_Toc_Env*_tmp237=e;
_tmp238=*_tmp237;_tmp239=_tmp238.break_lab;_tmp23A=_tmp238.continue_lab;_tmp23B=
_tmp238.fallthru_info;_tmp23C=_tmp238.varmap;_tmp23D=_tmp238.toplevel;{struct Cyc_Toc_Env*
_tmpB07;return(_tmpB07=_region_malloc(r,sizeof(*_tmpB07)),((_tmpB07->break_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp239),((_tmpB07->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp23A),((_tmpB07->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp23B,((_tmpB07->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp23C),((_tmpB07->toplevel=(int)0,((_tmpB07->rgn=(struct _RegionHandle*)r,
_tmpB07)))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct
_RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp240;struct
_dyneither_ptr**_tmp241;struct _dyneither_ptr**_tmp242;struct Cyc_Toc_FallthruInfo*
_tmp243;struct Cyc_Dict_Dict _tmp244;int _tmp245;struct Cyc_Toc_Env*_tmp23F=e;
_tmp240=*_tmp23F;_tmp241=_tmp240.break_lab;_tmp242=_tmp240.continue_lab;_tmp243=
_tmp240.fallthru_info;_tmp244=_tmp240.varmap;_tmp245=_tmp240.toplevel;{struct Cyc_Toc_Env*
_tmpB08;return(_tmpB08=_region_malloc(r,sizeof(*_tmpB08)),((_tmpB08->break_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp241),((_tmpB08->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp242),((_tmpB08->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp243,((_tmpB08->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp244),((_tmpB08->toplevel=(int)1,((_tmpB08->rgn=(struct _RegionHandle*)r,
_tmpB08)))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct
_RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace _tmp247=(*x).f1;struct Cyc_List_List*_tmp248;_LLE6: if((
_tmp247.Rel_n).tag != 1)goto _LLE8;_tmp248=(struct Cyc_List_List*)(_tmp247.Rel_n).val;
_LLE7: {const char*_tmpB0C;void*_tmpB0B[1];struct Cyc_String_pa_struct _tmpB0A;(
_tmpB0A.tag=0,((_tmpB0A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
x)),((_tmpB0B[0]=& _tmpB0A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB0C="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(
_tmpB0C,sizeof(char),30))),_tag_dyneither(_tmpB0B,sizeof(void*),1)))))));}_LLE8:;
_LLE9: goto _LLE5;_LLE5:;}{struct Cyc_Toc_Env _tmp24D;struct _dyneither_ptr**_tmp24E;
struct _dyneither_ptr**_tmp24F;struct Cyc_Toc_FallthruInfo*_tmp250;struct Cyc_Dict_Dict
_tmp251;int _tmp252;struct Cyc_Toc_Env*_tmp24C=e;_tmp24D=*_tmp24C;_tmp24E=_tmp24D.break_lab;
_tmp24F=_tmp24D.continue_lab;_tmp250=_tmp24D.fallthru_info;_tmp251=_tmp24D.varmap;
_tmp252=_tmp24D.toplevel;{struct Cyc_Dict_Dict _tmp253=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp251),x,y);struct Cyc_Toc_Env*_tmpB0D;return(_tmpB0D=_region_malloc(r,
sizeof(*_tmpB0D)),((_tmpB0D->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp24E),((_tmpB0D->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp24F),((_tmpB0D->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp250,((_tmpB0D->varmap=(struct Cyc_Dict_Dict)_tmp253,((_tmpB0D->toplevel=(int)
_tmp252,((_tmpB0D->rgn=(struct _RegionHandle*)r,_tmpB0D)))))))))))));};};}static
struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env _tmp256;struct _dyneither_ptr**_tmp257;struct _dyneither_ptr**
_tmp258;struct Cyc_Toc_FallthruInfo*_tmp259;struct Cyc_Dict_Dict _tmp25A;int _tmp25B;
struct Cyc_Toc_Env*_tmp255=e;_tmp256=*_tmp255;_tmp257=_tmp256.break_lab;_tmp258=
_tmp256.continue_lab;_tmp259=_tmp256.fallthru_info;_tmp25A=_tmp256.varmap;
_tmp25B=_tmp256.toplevel;{struct Cyc_Toc_Env*_tmpB0E;return(_tmpB0E=
_region_malloc(r,sizeof(*_tmpB0E)),((_tmpB0E->break_lab=(struct _dyneither_ptr**)
0,((_tmpB0E->continue_lab=(struct _dyneither_ptr**)0,((_tmpB0E->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp259,((_tmpB0E->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp25A),((_tmpB0E->toplevel=(int)_tmp25B,((_tmpB0E->rgn=(struct _RegionHandle*)
r,_tmpB0E)))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmpB0F;fallthru_vars=((
_tmpB0F=_region_malloc(r,sizeof(*_tmpB0F)),((_tmpB0F->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmpB0F->tl=fallthru_vars,_tmpB0F))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp25F;struct _dyneither_ptr**_tmp260;struct
_dyneither_ptr**_tmp261;struct Cyc_Toc_FallthruInfo*_tmp262;struct Cyc_Dict_Dict
_tmp263;int _tmp264;struct Cyc_Toc_Env*_tmp25E=e;_tmp25F=*_tmp25E;_tmp260=_tmp25F.break_lab;
_tmp261=_tmp25F.continue_lab;_tmp262=_tmp25F.fallthru_info;_tmp263=_tmp25F.varmap;
_tmp264=_tmp25F.toplevel;{struct Cyc_Toc_Env _tmp266;struct Cyc_Dict_Dict _tmp267;
struct Cyc_Toc_Env*_tmp265=next_case_env;_tmp266=*_tmp265;_tmp267=_tmp266.varmap;{
struct Cyc_Toc_FallthruInfo*_tmpB10;struct Cyc_Toc_FallthruInfo*fi=(_tmpB10=
_region_malloc(r,sizeof(*_tmpB10)),((_tmpB10->label=fallthru_l,((_tmpB10->binders=
fallthru_vars,((_tmpB10->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp267),_tmpB10)))))));
struct _dyneither_ptr**_tmpB13;struct Cyc_Toc_Env*_tmpB12;return(_tmpB12=
_region_malloc(r,sizeof(*_tmpB12)),((_tmpB12->break_lab=(struct _dyneither_ptr**)((
_tmpB13=_region_malloc(r,sizeof(*_tmpB13)),((_tmpB13[0]=break_l,_tmpB13)))),((
_tmpB12->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp261),((
_tmpB12->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpB12->varmap=(struct
Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp263),((_tmpB12->toplevel=(int)_tmp264,((_tmpB12->rgn=(
struct _RegionHandle*)r,_tmpB12)))))))))))));};};};}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){struct
Cyc_Toc_Env _tmp26C;struct _dyneither_ptr**_tmp26D;struct _dyneither_ptr**_tmp26E;
struct Cyc_Toc_FallthruInfo*_tmp26F;struct Cyc_Dict_Dict _tmp270;int _tmp271;struct
Cyc_Toc_Env*_tmp26B=e;_tmp26C=*_tmp26B;_tmp26D=_tmp26C.break_lab;_tmp26E=_tmp26C.continue_lab;
_tmp26F=_tmp26C.fallthru_info;_tmp270=_tmp26C.varmap;_tmp271=_tmp26C.toplevel;{
struct _dyneither_ptr**_tmpB16;struct Cyc_Toc_Env*_tmpB15;return(_tmpB15=
_region_malloc(r,sizeof(*_tmpB15)),((_tmpB15->break_lab=(struct _dyneither_ptr**)((
_tmpB16=_region_malloc(r,sizeof(*_tmpB16)),((_tmpB16[0]=break_l,_tmpB16)))),((
_tmpB15->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp26E),((
_tmpB15->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpB15->varmap=(struct
Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp270),((_tmpB15->toplevel=(int)_tmp271,((_tmpB15->rgn=(
struct _RegionHandle*)r,_tmpB15)))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp275;struct _dyneither_ptr**_tmp276;struct _dyneither_ptr**_tmp277;struct Cyc_Toc_FallthruInfo*
_tmp278;struct Cyc_Dict_Dict _tmp279;int _tmp27A;struct Cyc_Toc_Env*_tmp274=e;
_tmp275=*_tmp274;_tmp276=_tmp275.break_lab;_tmp277=_tmp275.continue_lab;_tmp278=
_tmp275.fallthru_info;_tmp279=_tmp275.varmap;_tmp27A=_tmp275.toplevel;{struct Cyc_Toc_FallthruInfo*
_tmpB19;struct Cyc_Toc_Env*_tmpB18;return(_tmpB18=_region_malloc(r,sizeof(*
_tmpB18)),((_tmpB18->break_lab=(struct _dyneither_ptr**)0,((_tmpB18->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp277),((_tmpB18->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)((_tmpB19=_region_malloc(r,sizeof(*_tmpB19)),((
_tmpB19->label=next_l,((_tmpB19->binders=0,((_tmpB19->next_case_env=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp),_tmpB19)))))))),((_tmpB18->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp279),((_tmpB18->toplevel=(int)_tmp27A,((_tmpB18->rgn=(struct _RegionHandle*)
r,_tmpB18)))))))))))));};}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){void*_tmp27D=e->annot;
_LLEB: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp27E=(struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmp27D;if(_tmp27E->tag != Cyc_CfFlowInfo_UnknownZ)goto _LLED;}_LLEC: return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLED: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmp27F=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmp27D;if(_tmp27F->tag != Cyc_CfFlowInfo_NotZero)
goto _LLEF;}_LLEE: return 0;_LLEF: {struct Cyc_CfFlowInfo_IsZero_struct*_tmp280=(
struct Cyc_CfFlowInfo_IsZero_struct*)_tmp27D;if(_tmp280->tag != Cyc_CfFlowInfo_IsZero)
goto _LLF1;}_LLF0:{const char*_tmpB1C;void*_tmpB1B;(_tmpB1B=0,Cyc_Tcutil_terr(e->loc,((
_tmpB1C="dereference of NULL pointer",_tag_dyneither(_tmpB1C,sizeof(char),28))),
_tag_dyneither(_tmpB1B,sizeof(void*),0)));}return 0;_LLF1: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp281=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp27D;if(_tmp281->tag != Cyc_Absyn_EmptyAnnot)
goto _LLF3;}_LLF2: return 0;_LLF3:;_LLF4: {const char*_tmpB1F;void*_tmpB1E;(_tmpB1E=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB1F="need_null_check",_tag_dyneither(_tmpB1F,sizeof(char),16))),
_tag_dyneither(_tmpB1E,sizeof(void*),0)));}_LLEA:;}static struct Cyc_List_List*Cyc_Toc_get_relns(
struct Cyc_Absyn_Exp*e){void*_tmp286=e->annot;struct Cyc_List_List*_tmp288;struct
Cyc_List_List*_tmp28A;_LLF6: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp287=(
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp286;if(_tmp287->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLF8;else{_tmp288=_tmp287->f1;}}_LLF7: return _tmp288;_LLF8: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmp289=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmp286;if(_tmp289->tag != Cyc_CfFlowInfo_NotZero)
goto _LLFA;else{_tmp28A=_tmp289->f1;}}_LLF9: return _tmp28A;_LLFA: {struct Cyc_CfFlowInfo_IsZero_struct*
_tmp28B=(struct Cyc_CfFlowInfo_IsZero_struct*)_tmp286;if(_tmp28B->tag != Cyc_CfFlowInfo_IsZero)
goto _LLFC;}_LLFB:{const char*_tmpB22;void*_tmpB21;(_tmpB21=0,Cyc_Tcutil_terr(e->loc,((
_tmpB22="dereference of NULL pointer",_tag_dyneither(_tmpB22,sizeof(char),28))),
_tag_dyneither(_tmpB21,sizeof(void*),0)));}return 0;_LLFC: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp28C=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp286;if(_tmp28C->tag != Cyc_Absyn_EmptyAnnot)
goto _LLFE;}_LLFD: return 0;_LLFE:;_LLFF: {const char*_tmpB25;void*_tmpB24;(_tmpB24=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB25="get_relns",_tag_dyneither(_tmpB25,sizeof(char),10))),_tag_dyneither(
_tmpB24,sizeof(void*),0)));}_LLF5:;}static int Cyc_Toc_check_const_array(
unsigned int i,void*t){void*_tmp291=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp293;struct Cyc_Absyn_PtrAtts _tmp294;union Cyc_Absyn_Constraint*_tmp295;union
Cyc_Absyn_Constraint*_tmp296;struct Cyc_Absyn_ArrayInfo _tmp298;struct Cyc_Absyn_Exp*
_tmp299;_LL101: {struct Cyc_Absyn_PointerType_struct*_tmp292=(struct Cyc_Absyn_PointerType_struct*)
_tmp291;if(_tmp292->tag != 5)goto _LL103;else{_tmp293=_tmp292->f1;_tmp294=_tmp293.ptr_atts;
_tmp295=_tmp294.bounds;_tmp296=_tmp294.zero_term;}}_LL102: {void*_tmp29A=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp295);struct Cyc_Absyn_Exp*_tmp29D;_LL108: {struct Cyc_Absyn_DynEither_b_struct*
_tmp29B=(struct Cyc_Absyn_DynEither_b_struct*)_tmp29A;if(_tmp29B->tag != 0)goto
_LL10A;}_LL109: return 0;_LL10A: {struct Cyc_Absyn_Upper_b_struct*_tmp29C=(struct
Cyc_Absyn_Upper_b_struct*)_tmp29A;if(_tmp29C->tag != 1)goto _LL107;else{_tmp29D=
_tmp29C->f1;}}_LL10B: {unsigned int _tmp29F;int _tmp2A0;struct _tuple10 _tmp29E=Cyc_Evexp_eval_const_uint_exp(
_tmp29D);_tmp29F=_tmp29E.f1;_tmp2A0=_tmp29E.f2;return _tmp2A0  && i <= _tmp29F;}
_LL107:;}_LL103: {struct Cyc_Absyn_ArrayType_struct*_tmp297=(struct Cyc_Absyn_ArrayType_struct*)
_tmp291;if(_tmp297->tag != 9)goto _LL105;else{_tmp298=_tmp297->f1;_tmp299=_tmp298.num_elts;}}
_LL104: if(_tmp299 == 0)return 0;{unsigned int _tmp2A2;int _tmp2A3;struct _tuple10
_tmp2A1=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp299);_tmp2A2=
_tmp2A1.f1;_tmp2A3=_tmp2A1.f2;return _tmp2A3  && i <= _tmp2A2;};_LL105:;_LL106:
return 0;_LL100:;}static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2A4=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(
_tmp2A4->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp _tmp2A5=_tmp2A4->rop;struct
Cyc_Absyn_Vardecl*_tmp2A6;struct Cyc_Absyn_Vardecl*_tmp2A7;_LL10D: if((_tmp2A5.LessNumelts).tag
!= 3)goto _LL10F;_tmp2A6=(struct Cyc_Absyn_Vardecl*)(_tmp2A5.LessNumelts).val;
_LL10E: _tmp2A7=_tmp2A6;goto _LL110;_LL10F: if((_tmp2A5.LessEqNumelts).tag != 5)goto
_LL111;_tmp2A7=(struct Cyc_Absyn_Vardecl*)(_tmp2A5.LessEqNumelts).val;_LL110: if(
_tmp2A7 == v)return 1;else{goto _LL10C;}_LL111:;_LL112: continue;_LL10C:;};}return 0;}
static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Exp*e,void*vtype){{void*_tmp2A8=e->r;void*_tmp2AA;struct Cyc_Absyn_Vardecl*
_tmp2AC;void*_tmp2AE;struct Cyc_Absyn_Vardecl*_tmp2B0;void*_tmp2B2;struct Cyc_Absyn_Vardecl*
_tmp2B4;void*_tmp2B6;struct Cyc_Absyn_Vardecl*_tmp2B8;union Cyc_Absyn_Cnst _tmp2BA;
struct _tuple5 _tmp2BB;enum Cyc_Absyn_Sign _tmp2BC;int _tmp2BD;union Cyc_Absyn_Cnst
_tmp2BF;struct _tuple5 _tmp2C0;enum Cyc_Absyn_Sign _tmp2C1;int _tmp2C2;union Cyc_Absyn_Cnst
_tmp2C4;struct _tuple5 _tmp2C5;enum Cyc_Absyn_Sign _tmp2C6;int _tmp2C7;enum Cyc_Absyn_Primop
_tmp2C9;struct Cyc_List_List*_tmp2CA;struct Cyc_List_List _tmp2CB;struct Cyc_Absyn_Exp*
_tmp2CC;_LL114: {struct Cyc_Absyn_Var_e_struct*_tmp2A9=(struct Cyc_Absyn_Var_e_struct*)
_tmp2A8;if(_tmp2A9->tag != 1)goto _LL116;else{_tmp2AA=(void*)_tmp2A9->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2AB=(struct Cyc_Absyn_Pat_b_struct*)_tmp2AA;if(_tmp2AB->tag
!= 5)goto _LL116;else{_tmp2AC=_tmp2AB->f1;}};}}_LL115: _tmp2B0=_tmp2AC;goto _LL117;
_LL116: {struct Cyc_Absyn_Var_e_struct*_tmp2AD=(struct Cyc_Absyn_Var_e_struct*)
_tmp2A8;if(_tmp2AD->tag != 1)goto _LL118;else{_tmp2AE=(void*)_tmp2AD->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2AF=(struct Cyc_Absyn_Local_b_struct*)_tmp2AE;if(
_tmp2AF->tag != 4)goto _LL118;else{_tmp2B0=_tmp2AF->f1;}};}}_LL117: _tmp2B4=_tmp2B0;
goto _LL119;_LL118: {struct Cyc_Absyn_Var_e_struct*_tmp2B1=(struct Cyc_Absyn_Var_e_struct*)
_tmp2A8;if(_tmp2B1->tag != 1)goto _LL11A;else{_tmp2B2=(void*)_tmp2B1->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2B3=(struct Cyc_Absyn_Global_b_struct*)_tmp2B2;if(
_tmp2B3->tag != 1)goto _LL11A;else{_tmp2B4=_tmp2B3->f1;}};}}_LL119: _tmp2B8=_tmp2B4;
goto _LL11B;_LL11A: {struct Cyc_Absyn_Var_e_struct*_tmp2B5=(struct Cyc_Absyn_Var_e_struct*)
_tmp2A8;if(_tmp2B5->tag != 1)goto _LL11C;else{_tmp2B6=(void*)_tmp2B5->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp2B7=(struct Cyc_Absyn_Param_b_struct*)_tmp2B6;if(
_tmp2B7->tag != 3)goto _LL11C;else{_tmp2B8=_tmp2B7->f1;}};}}_LL11B: if(_tmp2B8->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp2B8))return 1;goto _LL113;_LL11C: {
struct Cyc_Absyn_Const_e_struct*_tmp2B9=(struct Cyc_Absyn_Const_e_struct*)_tmp2A8;
if(_tmp2B9->tag != 0)goto _LL11E;else{_tmp2BA=_tmp2B9->f1;if((_tmp2BA.Int_c).tag != 
4)goto _LL11E;_tmp2BB=(struct _tuple5)(_tmp2BA.Int_c).val;_tmp2BC=_tmp2BB.f1;if(
_tmp2BC != Cyc_Absyn_None)goto _LL11E;_tmp2BD=_tmp2BB.f2;}}_LL11D: _tmp2C2=_tmp2BD;
goto _LL11F;_LL11E: {struct Cyc_Absyn_Const_e_struct*_tmp2BE=(struct Cyc_Absyn_Const_e_struct*)
_tmp2A8;if(_tmp2BE->tag != 0)goto _LL120;else{_tmp2BF=_tmp2BE->f1;if((_tmp2BF.Int_c).tag
!= 4)goto _LL120;_tmp2C0=(struct _tuple5)(_tmp2BF.Int_c).val;_tmp2C1=_tmp2C0.f1;
if(_tmp2C1 != Cyc_Absyn_Signed)goto _LL120;_tmp2C2=_tmp2C0.f2;}}_LL11F: return
_tmp2C2 >= 0  && Cyc_Toc_check_const_array((unsigned int)_tmp2C2,vtype);_LL120: {
struct Cyc_Absyn_Const_e_struct*_tmp2C3=(struct Cyc_Absyn_Const_e_struct*)_tmp2A8;
if(_tmp2C3->tag != 0)goto _LL122;else{_tmp2C4=_tmp2C3->f1;if((_tmp2C4.Int_c).tag != 
4)goto _LL122;_tmp2C5=(struct _tuple5)(_tmp2C4.Int_c).val;_tmp2C6=_tmp2C5.f1;if(
_tmp2C6 != Cyc_Absyn_Unsigned)goto _LL122;_tmp2C7=_tmp2C5.f2;}}_LL121: return Cyc_Toc_check_const_array((
unsigned int)_tmp2C7,vtype);_LL122: {struct Cyc_Absyn_Primop_e_struct*_tmp2C8=(
struct Cyc_Absyn_Primop_e_struct*)_tmp2A8;if(_tmp2C8->tag != 3)goto _LL124;else{
_tmp2C9=_tmp2C8->f1;if(_tmp2C9 != Cyc_Absyn_Numelts)goto _LL124;_tmp2CA=_tmp2C8->f2;
if(_tmp2CA == 0)goto _LL124;_tmp2CB=*_tmp2CA;_tmp2CC=(struct Cyc_Absyn_Exp*)_tmp2CB.hd;}}
_LL123:{void*_tmp2CD=_tmp2CC->r;void*_tmp2CF;struct Cyc_Absyn_Vardecl*_tmp2D1;
void*_tmp2D3;struct Cyc_Absyn_Vardecl*_tmp2D5;void*_tmp2D7;struct Cyc_Absyn_Vardecl*
_tmp2D9;void*_tmp2DB;struct Cyc_Absyn_Vardecl*_tmp2DD;_LL127: {struct Cyc_Absyn_Var_e_struct*
_tmp2CE=(struct Cyc_Absyn_Var_e_struct*)_tmp2CD;if(_tmp2CE->tag != 1)goto _LL129;
else{_tmp2CF=(void*)_tmp2CE->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp2D0=(struct Cyc_Absyn_Pat_b_struct*)
_tmp2CF;if(_tmp2D0->tag != 5)goto _LL129;else{_tmp2D1=_tmp2D0->f1;}};}}_LL128:
_tmp2D5=_tmp2D1;goto _LL12A;_LL129: {struct Cyc_Absyn_Var_e_struct*_tmp2D2=(struct
Cyc_Absyn_Var_e_struct*)_tmp2CD;if(_tmp2D2->tag != 1)goto _LL12B;else{_tmp2D3=(
void*)_tmp2D2->f2;{struct Cyc_Absyn_Local_b_struct*_tmp2D4=(struct Cyc_Absyn_Local_b_struct*)
_tmp2D3;if(_tmp2D4->tag != 4)goto _LL12B;else{_tmp2D5=_tmp2D4->f1;}};}}_LL12A:
_tmp2D9=_tmp2D5;goto _LL12C;_LL12B: {struct Cyc_Absyn_Var_e_struct*_tmp2D6=(struct
Cyc_Absyn_Var_e_struct*)_tmp2CD;if(_tmp2D6->tag != 1)goto _LL12D;else{_tmp2D7=(
void*)_tmp2D6->f2;{struct Cyc_Absyn_Global_b_struct*_tmp2D8=(struct Cyc_Absyn_Global_b_struct*)
_tmp2D7;if(_tmp2D8->tag != 1)goto _LL12D;else{_tmp2D9=_tmp2D8->f1;}};}}_LL12C:
_tmp2DD=_tmp2D9;goto _LL12E;_LL12D: {struct Cyc_Absyn_Var_e_struct*_tmp2DA=(struct
Cyc_Absyn_Var_e_struct*)_tmp2CD;if(_tmp2DA->tag != 1)goto _LL12F;else{_tmp2DB=(
void*)_tmp2DA->f2;{struct Cyc_Absyn_Param_b_struct*_tmp2DC=(struct Cyc_Absyn_Param_b_struct*)
_tmp2DB;if(_tmp2DC->tag != 3)goto _LL12F;else{_tmp2DD=_tmp2DC->f1;}};}}_LL12E:
return _tmp2DD == v;_LL12F:;_LL130: goto _LL126;_LL126:;}goto _LL113;_LL124:;_LL125:
goto _LL113;_LL113:;}return 0;}static char _tmp2EF[8]="*bogus*";struct _tuple17{void*
f1;void*f2;};static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,
struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){struct Cyc_Absyn_Vardecl*x;{void*
_tmp2DE=a->r;void*_tmp2E0;struct Cyc_Absyn_Vardecl*_tmp2E2;void*_tmp2E4;struct Cyc_Absyn_Vardecl*
_tmp2E6;void*_tmp2E8;struct Cyc_Absyn_Vardecl*_tmp2EA;void*_tmp2EC;struct Cyc_Absyn_Vardecl*
_tmp2EE;_LL132: {struct Cyc_Absyn_Var_e_struct*_tmp2DF=(struct Cyc_Absyn_Var_e_struct*)
_tmp2DE;if(_tmp2DF->tag != 1)goto _LL134;else{_tmp2E0=(void*)_tmp2DF->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2E1=(struct Cyc_Absyn_Pat_b_struct*)_tmp2E0;if(_tmp2E1->tag
!= 5)goto _LL134;else{_tmp2E2=_tmp2E1->f1;}};}}_LL133: _tmp2E6=_tmp2E2;goto _LL135;
_LL134: {struct Cyc_Absyn_Var_e_struct*_tmp2E3=(struct Cyc_Absyn_Var_e_struct*)
_tmp2DE;if(_tmp2E3->tag != 1)goto _LL136;else{_tmp2E4=(void*)_tmp2E3->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2E5=(struct Cyc_Absyn_Local_b_struct*)_tmp2E4;if(
_tmp2E5->tag != 4)goto _LL136;else{_tmp2E6=_tmp2E5->f1;}};}}_LL135: _tmp2EA=_tmp2E6;
goto _LL137;_LL136: {struct Cyc_Absyn_Var_e_struct*_tmp2E7=(struct Cyc_Absyn_Var_e_struct*)
_tmp2DE;if(_tmp2E7->tag != 1)goto _LL138;else{_tmp2E8=(void*)_tmp2E7->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2E9=(struct Cyc_Absyn_Global_b_struct*)_tmp2E8;if(
_tmp2E9->tag != 1)goto _LL138;else{_tmp2EA=_tmp2E9->f1;}};}}_LL137: _tmp2EE=_tmp2EA;
goto _LL139;_LL138: {struct Cyc_Absyn_Var_e_struct*_tmp2EB=(struct Cyc_Absyn_Var_e_struct*)
_tmp2DE;if(_tmp2EB->tag != 1)goto _LL13A;else{_tmp2EC=(void*)_tmp2EB->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp2ED=(struct Cyc_Absyn_Param_b_struct*)_tmp2EC;if(
_tmp2ED->tag != 3)goto _LL13A;else{_tmp2EE=_tmp2ED->f1;}};}}_LL139: if(_tmp2EE->escapes)
goto _LL13B;x=_tmp2EE;goto _LL131;_LL13A:;_LL13B: {static struct _dyneither_ptr
bogus_string={_tmp2EF,_tmp2EF,_tmp2EF + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
4,0}},& bogus_string};static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,&
bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};x=& bogus_vardecl;
x->type=a_typ;}_LL131:;}{void*_tmp2F0=a_typ;inner_loop: {void*_tmp2F1=i->r;void*
_tmp2F3;struct Cyc_Absyn_Exp*_tmp2F4;union Cyc_Absyn_Cnst _tmp2F6;struct _tuple5
_tmp2F7;enum Cyc_Absyn_Sign _tmp2F8;int _tmp2F9;union Cyc_Absyn_Cnst _tmp2FB;struct
_tuple5 _tmp2FC;enum Cyc_Absyn_Sign _tmp2FD;int _tmp2FE;union Cyc_Absyn_Cnst _tmp300;
struct _tuple5 _tmp301;enum Cyc_Absyn_Sign _tmp302;int _tmp303;enum Cyc_Absyn_Primop
_tmp305;struct Cyc_List_List*_tmp306;struct Cyc_List_List _tmp307;struct Cyc_Absyn_Exp*
_tmp308;struct Cyc_List_List*_tmp309;struct Cyc_List_List _tmp30A;struct Cyc_Absyn_Exp*
_tmp30B;void*_tmp30D;struct Cyc_Absyn_Vardecl*_tmp30F;void*_tmp311;struct Cyc_Absyn_Vardecl*
_tmp313;void*_tmp315;struct Cyc_Absyn_Vardecl*_tmp317;void*_tmp319;struct Cyc_Absyn_Vardecl*
_tmp31B;_LL13D: {struct Cyc_Absyn_Cast_e_struct*_tmp2F2=(struct Cyc_Absyn_Cast_e_struct*)
_tmp2F1;if(_tmp2F2->tag != 15)goto _LL13F;else{_tmp2F3=(void*)_tmp2F2->f1;_tmp2F4=
_tmp2F2->f2;}}_LL13E: i=_tmp2F4;goto inner_loop;_LL13F: {struct Cyc_Absyn_Const_e_struct*
_tmp2F5=(struct Cyc_Absyn_Const_e_struct*)_tmp2F1;if(_tmp2F5->tag != 0)goto _LL141;
else{_tmp2F6=_tmp2F5->f1;if((_tmp2F6.Int_c).tag != 4)goto _LL141;_tmp2F7=(struct
_tuple5)(_tmp2F6.Int_c).val;_tmp2F8=_tmp2F7.f1;if(_tmp2F8 != Cyc_Absyn_None)goto
_LL141;_tmp2F9=_tmp2F7.f2;}}_LL140: _tmp2FE=_tmp2F9;goto _LL142;_LL141: {struct Cyc_Absyn_Const_e_struct*
_tmp2FA=(struct Cyc_Absyn_Const_e_struct*)_tmp2F1;if(_tmp2FA->tag != 0)goto _LL143;
else{_tmp2FB=_tmp2FA->f1;if((_tmp2FB.Int_c).tag != 4)goto _LL143;_tmp2FC=(struct
_tuple5)(_tmp2FB.Int_c).val;_tmp2FD=_tmp2FC.f1;if(_tmp2FD != Cyc_Absyn_Signed)
goto _LL143;_tmp2FE=_tmp2FC.f2;}}_LL142: return _tmp2FE >= 0  && Cyc_Toc_check_const_array((
unsigned int)(_tmp2FE + 1),_tmp2F0);_LL143: {struct Cyc_Absyn_Const_e_struct*
_tmp2FF=(struct Cyc_Absyn_Const_e_struct*)_tmp2F1;if(_tmp2FF->tag != 0)goto _LL145;
else{_tmp300=_tmp2FF->f1;if((_tmp300.Int_c).tag != 4)goto _LL145;_tmp301=(struct
_tuple5)(_tmp300.Int_c).val;_tmp302=_tmp301.f1;if(_tmp302 != Cyc_Absyn_Unsigned)
goto _LL145;_tmp303=_tmp301.f2;}}_LL144: return Cyc_Toc_check_const_array((
unsigned int)(_tmp303 + 1),_tmp2F0);_LL145: {struct Cyc_Absyn_Primop_e_struct*
_tmp304=(struct Cyc_Absyn_Primop_e_struct*)_tmp2F1;if(_tmp304->tag != 3)goto _LL147;
else{_tmp305=_tmp304->f1;if(_tmp305 != Cyc_Absyn_Mod)goto _LL147;_tmp306=_tmp304->f2;
if(_tmp306 == 0)goto _LL147;_tmp307=*_tmp306;_tmp308=(struct Cyc_Absyn_Exp*)_tmp307.hd;
_tmp309=_tmp307.tl;if(_tmp309 == 0)goto _LL147;_tmp30A=*_tmp309;_tmp30B=(struct Cyc_Absyn_Exp*)
_tmp30A.hd;}}_LL146: return Cyc_Toc_check_leq_size(relns,x,_tmp30B,_tmp2F0);_LL147: {
struct Cyc_Absyn_Var_e_struct*_tmp30C=(struct Cyc_Absyn_Var_e_struct*)_tmp2F1;if(
_tmp30C->tag != 1)goto _LL149;else{_tmp30D=(void*)_tmp30C->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp30E=(struct Cyc_Absyn_Pat_b_struct*)_tmp30D;if(_tmp30E->tag != 5)goto _LL149;
else{_tmp30F=_tmp30E->f1;}};}}_LL148: _tmp313=_tmp30F;goto _LL14A;_LL149: {struct
Cyc_Absyn_Var_e_struct*_tmp310=(struct Cyc_Absyn_Var_e_struct*)_tmp2F1;if(_tmp310->tag
!= 1)goto _LL14B;else{_tmp311=(void*)_tmp310->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp312=(struct Cyc_Absyn_Local_b_struct*)_tmp311;if(_tmp312->tag != 4)goto _LL14B;
else{_tmp313=_tmp312->f1;}};}}_LL14A: _tmp317=_tmp313;goto _LL14C;_LL14B: {struct
Cyc_Absyn_Var_e_struct*_tmp314=(struct Cyc_Absyn_Var_e_struct*)_tmp2F1;if(_tmp314->tag
!= 1)goto _LL14D;else{_tmp315=(void*)_tmp314->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp316=(struct Cyc_Absyn_Global_b_struct*)_tmp315;if(_tmp316->tag != 1)goto _LL14D;
else{_tmp317=_tmp316->f1;}};}}_LL14C: _tmp31B=_tmp317;goto _LL14E;_LL14D: {struct
Cyc_Absyn_Var_e_struct*_tmp318=(struct Cyc_Absyn_Var_e_struct*)_tmp2F1;if(_tmp318->tag
!= 1)goto _LL14F;else{_tmp319=(void*)_tmp318->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp31A=(struct Cyc_Absyn_Param_b_struct*)_tmp319;if(_tmp31A->tag != 3)goto _LL14F;
else{_tmp31B=_tmp31A->f1;}};}}_LL14E: if(_tmp31B->escapes)return 0;{struct Cyc_List_List*
_tmp31C=relns;for(0;_tmp31C != 0;_tmp31C=_tmp31C->tl){struct Cyc_CfFlowInfo_Reln*
_tmp31D=(struct Cyc_CfFlowInfo_Reln*)_tmp31C->hd;if(_tmp31D->vd == _tmp31B){union
Cyc_CfFlowInfo_RelnOp _tmp31E=_tmp31D->rop;struct Cyc_Absyn_Vardecl*_tmp31F;struct
_tuple11 _tmp320;struct Cyc_Absyn_Vardecl*_tmp321;void*_tmp322;unsigned int _tmp323;
unsigned int _tmp324;_LL152: if((_tmp31E.LessNumelts).tag != 3)goto _LL154;_tmp31F=(
struct Cyc_Absyn_Vardecl*)(_tmp31E.LessNumelts).val;_LL153: if(x == _tmp31F)return 1;
else{goto _LL151;}_LL154: if((_tmp31E.LessVar).tag != 2)goto _LL156;_tmp320=(struct
_tuple11)(_tmp31E.LessVar).val;_tmp321=_tmp320.f1;_tmp322=_tmp320.f2;_LL155:{
struct _tuple17 _tmpB26;struct _tuple17 _tmp326=(_tmpB26.f1=Cyc_Tcutil_compress(
_tmp322),((_tmpB26.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(a->topt))->v),_tmpB26)));void*_tmp327;void*_tmp329;void*_tmp32A;
struct Cyc_Absyn_PtrInfo _tmp32C;struct Cyc_Absyn_PtrAtts _tmp32D;union Cyc_Absyn_Constraint*
_tmp32E;_LL15D: _tmp327=_tmp326.f1;{struct Cyc_Absyn_TagType_struct*_tmp328=(
struct Cyc_Absyn_TagType_struct*)_tmp327;if(_tmp328->tag != 20)goto _LL15F;else{
_tmp329=(void*)_tmp328->f1;}};_tmp32A=_tmp326.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp32B=(struct Cyc_Absyn_PointerType_struct*)_tmp32A;if(_tmp32B->tag != 5)goto
_LL15F;else{_tmp32C=_tmp32B->f1;_tmp32D=_tmp32C.ptr_atts;_tmp32E=_tmp32D.bounds;}};
_LL15E:{void*_tmp32F=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp32E);struct Cyc_Absyn_Exp*_tmp331;_LL162: {struct Cyc_Absyn_Upper_b_struct*
_tmp330=(struct Cyc_Absyn_Upper_b_struct*)_tmp32F;if(_tmp330->tag != 1)goto _LL164;
else{_tmp331=_tmp330->f1;}}_LL163: {struct Cyc_Absyn_Exp*_tmp332=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp329,0),0,Cyc_Absyn_No_coercion,0);
if(Cyc_Evexp_lte_const_exp(_tmp332,_tmp331))return 1;goto _LL161;}_LL164:;_LL165:
goto _LL161;_LL161:;}goto _LL15C;_LL15F:;_LL160: goto _LL15C;_LL15C:;}{struct Cyc_List_List*
_tmp333=relns;for(0;_tmp333 != 0;_tmp333=_tmp333->tl){struct Cyc_CfFlowInfo_Reln*
_tmp334=(struct Cyc_CfFlowInfo_Reln*)_tmp333->hd;if(_tmp334->vd == _tmp321){union
Cyc_CfFlowInfo_RelnOp _tmp335=_tmp334->rop;struct Cyc_Absyn_Vardecl*_tmp336;struct
Cyc_Absyn_Vardecl*_tmp337;unsigned int _tmp338;unsigned int _tmp339;struct _tuple11
_tmp33A;struct Cyc_Absyn_Vardecl*_tmp33B;_LL167: if((_tmp335.LessEqNumelts).tag != 
5)goto _LL169;_tmp336=(struct Cyc_Absyn_Vardecl*)(_tmp335.LessEqNumelts).val;
_LL168: _tmp337=_tmp336;goto _LL16A;_LL169: if((_tmp335.LessNumelts).tag != 3)goto
_LL16B;_tmp337=(struct Cyc_Absyn_Vardecl*)(_tmp335.LessNumelts).val;_LL16A: if(x == 
_tmp337)return 1;goto _LL166;_LL16B: if((_tmp335.EqualConst).tag != 1)goto _LL16D;
_tmp338=(unsigned int)(_tmp335.EqualConst).val;_LL16C: return Cyc_Toc_check_const_array(
_tmp338,_tmp2F0);_LL16D: if((_tmp335.LessEqConst).tag != 6)goto _LL16F;_tmp339=(
unsigned int)(_tmp335.LessEqConst).val;if(!(_tmp339 > 0))goto _LL16F;_LL16E: return
Cyc_Toc_check_const_array(_tmp339,_tmp2F0);_LL16F: if((_tmp335.LessVar).tag != 2)
goto _LL171;_tmp33A=(struct _tuple11)(_tmp335.LessVar).val;_tmp33B=_tmp33A.f1;
_LL170: if(Cyc_Toc_check_leq_size_var(relns,x,_tmp33B))return 1;goto _LL166;_LL171:;
_LL172: goto _LL166;_LL166:;}}}goto _LL151;_LL156: if((_tmp31E.LessConst).tag != 4)
goto _LL158;_tmp323=(unsigned int)(_tmp31E.LessConst).val;_LL157: return Cyc_Toc_check_const_array(
_tmp323,_tmp2F0);_LL158: if((_tmp31E.LessEqConst).tag != 6)goto _LL15A;_tmp324=(
unsigned int)(_tmp31E.LessEqConst).val;_LL159: return Cyc_Toc_check_const_array(
_tmp324 + 1,_tmp2F0);_LL15A:;_LL15B: goto _LL151;_LL151:;}}}goto _LL13C;_LL14F:;
_LL150: goto _LL13C;_LL13C:;}return 0;};}static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*
e){if(e->topt == 0){const char*_tmpB29;void*_tmpB28;(_tmpB28=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB29="Missing type in primop ",
_tag_dyneither(_tmpB29,sizeof(char),24))),_tag_dyneither(_tmpB28,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*
_tmpB2C;void*_tmpB2B;(_tmpB2B=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2C="Missing type in primop ",
_tag_dyneither(_tmpB2C,sizeof(char),24))),_tag_dyneither(_tmpB2B,sizeof(void*),0)));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple9*
Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){struct _tuple9*_tmpB2D;return(_tmpB2D=
_cycalloc(sizeof(*_tmpB2D)),((_tmpB2D->f1=Cyc_Toc_mt_tq,((_tmpB2D->f2=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmpB2D)))));}static struct
_tuple15*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple15*_tmpB2E;return(_tmpB2E=_cycalloc(sizeof(*_tmpB2E)),((
_tmpB2E->f1=0,((_tmpB2E->f2=e,_tmpB2E)))));};}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(
struct Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int
pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;
if(pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp342=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0  || Cyc_Absyn_no_regions)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(
_tmp342): Cyc_Toc_malloc_ptr(_tmp342));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp342);}};}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp343=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp343 != 0;_tmp343=_tmp343->tl){struct _tuple15 _tmp345;struct Cyc_List_List*
_tmp346;struct Cyc_Absyn_Exp*_tmp347;struct _tuple15*_tmp344=(struct _tuple15*)
_tmp343->hd;_tmp345=*_tmp344;_tmp346=_tmp345.f1;_tmp347=_tmp345.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp346 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp346->tl != 0){const char*_tmpB31;void*_tmpB30;(_tmpB30=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB31="multiple designators in array",
_tag_dyneither(_tmpB31,sizeof(char),30))),_tag_dyneither(_tmpB30,sizeof(void*),0)));}{
void*_tmp34A=(void*)_tmp346->hd;void*_tmp34B=_tmp34A;struct Cyc_Absyn_Exp*_tmp34D;
_LL174: {struct Cyc_Absyn_ArrayElement_struct*_tmp34C=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp34B;if(_tmp34C->tag != 0)goto _LL176;else{_tmp34D=_tmp34C->f1;}}_LL175: Cyc_Toc_exp_to_c(
nv,_tmp34D);e_index=_tmp34D;goto _LL173;_LL176: {struct Cyc_Absyn_FieldName_struct*
_tmp34E=(struct Cyc_Absyn_FieldName_struct*)_tmp34B;if(_tmp34E->tag != 1)goto
_LL173;}_LL177: {const char*_tmpB34;void*_tmpB33;(_tmpB33=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB34="field name designators in array",
_tag_dyneither(_tmpB34,sizeof(char),32))),_tag_dyneither(_tmpB33,sizeof(void*),0)));}
_LL173:;};}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*
_tmp351=_tmp347->r;struct Cyc_List_List*_tmp353;struct Cyc_Absyn_Vardecl*_tmp355;
struct Cyc_Absyn_Exp*_tmp356;struct Cyc_Absyn_Exp*_tmp357;int _tmp358;void*_tmp35A;
struct Cyc_List_List*_tmp35B;_LL179: {struct Cyc_Absyn_Array_e_struct*_tmp352=(
struct Cyc_Absyn_Array_e_struct*)_tmp351;if(_tmp352->tag != 27)goto _LL17B;else{
_tmp353=_tmp352->f1;}}_LL17A: s=Cyc_Toc_init_array(nv,lval,_tmp353,s);goto _LL178;
_LL17B: {struct Cyc_Absyn_Comprehension_e_struct*_tmp354=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp351;if(_tmp354->tag != 28)goto _LL17D;else{_tmp355=_tmp354->f1;_tmp356=_tmp354->f2;
_tmp357=_tmp354->f3;_tmp358=_tmp354->f4;}}_LL17C: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp355,_tmp356,_tmp357,_tmp358,s,0);goto _LL178;_LL17D: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp359=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp351;if(_tmp359->tag != 30)goto
_LL17F;else{_tmp35A=(void*)_tmp359->f1;_tmp35B=_tmp359->f2;}}_LL17E: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp35A,_tmp35B,s);goto _LL178;_LL17F:;_LL180: Cyc_Toc_exp_to_c(nv,_tmp347);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
_tmp347,0),s,0);goto _LL178;_LL178:;};};}}return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple0*_tmp35C=vd->name;void*_tmp35D=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env _tmp35F;struct _RegionHandle*_tmp360;
struct Cyc_Toc_Env*_tmp35E=nv;_tmp35F=*_tmp35E;_tmp360=_tmp35F.rgn;{struct Cyc_Absyn_Local_b_struct
_tmpB37;struct Cyc_Absyn_Local_b_struct*_tmpB36;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
_tmp360,nv,_tmp35C,Cyc_Absyn_varb_exp(_tmp35C,(void*)((_tmpB36=_cycalloc(sizeof(*
_tmpB36)),((_tmpB36[0]=((_tmpB37.tag=4,((_tmpB37.f1=vd,_tmpB37)))),_tmpB36)))),0));
struct _tuple0*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp35C,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp35C,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp35C,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp35C,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp361=e2->r;struct Cyc_List_List*_tmp363;
struct Cyc_Absyn_Vardecl*_tmp365;struct Cyc_Absyn_Exp*_tmp366;struct Cyc_Absyn_Exp*
_tmp367;int _tmp368;void*_tmp36A;struct Cyc_List_List*_tmp36B;_LL182: {struct Cyc_Absyn_Array_e_struct*
_tmp362=(struct Cyc_Absyn_Array_e_struct*)_tmp361;if(_tmp362->tag != 27)goto _LL184;
else{_tmp363=_tmp362->f1;}}_LL183: body=Cyc_Toc_init_array(nv2,lval,_tmp363,Cyc_Toc_skip_stmt_dl());
goto _LL181;_LL184: {struct Cyc_Absyn_Comprehension_e_struct*_tmp364=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp361;if(_tmp364->tag != 28)goto _LL186;else{_tmp365=_tmp364->f1;_tmp366=_tmp364->f2;
_tmp367=_tmp364->f3;_tmp368=_tmp364->f4;}}_LL185: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp365,_tmp366,_tmp367,_tmp368,Cyc_Toc_skip_stmt_dl(),0);goto _LL181;
_LL186: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp369=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp361;if(_tmp369->tag != 30)goto _LL188;else{_tmp36A=(void*)_tmp369->f1;_tmp36B=
_tmp369->f2;}}_LL187: body=Cyc_Toc_init_anon_struct(nv,lval,_tmp36A,_tmp36B,Cyc_Toc_skip_stmt_dl());
goto _LL181;_LL188:;_LL189: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL181;_LL181:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp35D,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp35C,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp36E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp36E != 0;_tmp36E=_tmp36E->tl){struct _tuple15 _tmp370;
struct Cyc_List_List*_tmp371;struct Cyc_Absyn_Exp*_tmp372;struct _tuple15*_tmp36F=(
struct _tuple15*)_tmp36E->hd;_tmp370=*_tmp36F;_tmp371=_tmp370.f1;_tmp372=_tmp370.f2;
if(_tmp371 == 0){const char*_tmpB3A;void*_tmpB39;(_tmpB39=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3A="empty designator list",
_tag_dyneither(_tmpB3A,sizeof(char),22))),_tag_dyneither(_tmpB39,sizeof(void*),0)));}
if(_tmp371->tl != 0){const char*_tmpB3D;void*_tmpB3C;(_tmpB3C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3D="too many designators in anonymous struct",
_tag_dyneither(_tmpB3D,sizeof(char),41))),_tag_dyneither(_tmpB3C,sizeof(void*),0)));}{
void*_tmp377=(void*)_tmp371->hd;struct _dyneither_ptr*_tmp379;_LL18B: {struct Cyc_Absyn_FieldName_struct*
_tmp378=(struct Cyc_Absyn_FieldName_struct*)_tmp377;if(_tmp378->tag != 1)goto
_LL18D;else{_tmp379=_tmp378->f1;}}_LL18C: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(
lhs,_tmp379,0);{void*_tmp37A=_tmp372->r;struct Cyc_List_List*_tmp37C;struct Cyc_Absyn_Vardecl*
_tmp37E;struct Cyc_Absyn_Exp*_tmp37F;struct Cyc_Absyn_Exp*_tmp380;int _tmp381;void*
_tmp383;struct Cyc_List_List*_tmp384;_LL190: {struct Cyc_Absyn_Array_e_struct*
_tmp37B=(struct Cyc_Absyn_Array_e_struct*)_tmp37A;if(_tmp37B->tag != 27)goto _LL192;
else{_tmp37C=_tmp37B->f1;}}_LL191: s=Cyc_Toc_init_array(nv,lval,_tmp37C,s);goto
_LL18F;_LL192: {struct Cyc_Absyn_Comprehension_e_struct*_tmp37D=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp37A;if(_tmp37D->tag != 28)goto _LL194;else{_tmp37E=_tmp37D->f1;_tmp37F=_tmp37D->f2;
_tmp380=_tmp37D->f3;_tmp381=_tmp37D->f4;}}_LL193: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp37E,_tmp37F,_tmp380,_tmp381,s,0);goto _LL18F;_LL194: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp382=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp37A;if(_tmp382->tag != 30)goto
_LL196;else{_tmp383=(void*)_tmp382->f1;_tmp384=_tmp382->f2;}}_LL195: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp383,_tmp384,s);goto _LL18F;_LL196:;_LL197: Cyc_Toc_exp_to_c(nv,_tmp372);
if(Cyc_Toc_is_poly_field(struct_type,_tmp379))_tmp372=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp372);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp372,0),0),s,0);goto _LL18F;_LL18F:;}goto _LL18A;}_LL18D:;_LL18E: {const char*
_tmpB40;void*_tmpB3F;(_tmpB3F=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB40="array designator in struct",
_tag_dyneither(_tmpB40,sizeof(char),27))),_tag_dyneither(_tmpB3F,sizeof(void*),0)));}
_LL18A:;};}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct Cyc_Toc_Env
_tmp388;struct _RegionHandle*_tmp389;struct Cyc_Toc_Env*_tmp387=nv;_tmp388=*
_tmp387;_tmp389=_tmp388.rgn;{struct Cyc_List_List*_tmp38A=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_rmap)(_tmp389,Cyc_Toc_tup_to_c,es);void*_tmp38B=Cyc_Toc_add_tuple_type(
_tmp38A);struct _tuple0*_tmp38C=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp38D=
Cyc_Absyn_var_exp(_tmp38C,0);struct Cyc_Absyn_Stmt*_tmp38E=Cyc_Absyn_exp_stmt(
_tmp38D,0);struct Cyc_Absyn_Exp*(*_tmp38F)(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp390=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp389,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp390);for(0;_tmp390 != 0;(_tmp390=
_tmp390->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp390->hd;struct
Cyc_Absyn_Exp*lval=_tmp38F(_tmp38D,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp391=e->r;struct Cyc_List_List*_tmp393;struct Cyc_Absyn_Vardecl*_tmp395;
struct Cyc_Absyn_Exp*_tmp396;struct Cyc_Absyn_Exp*_tmp397;int _tmp398;_LL199: {
struct Cyc_Absyn_Array_e_struct*_tmp392=(struct Cyc_Absyn_Array_e_struct*)_tmp391;
if(_tmp392->tag != 27)goto _LL19B;else{_tmp393=_tmp392->f1;}}_LL19A: _tmp38E=Cyc_Toc_init_array(
nv,lval,_tmp393,_tmp38E);goto _LL198;_LL19B: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp394=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp391;if(_tmp394->tag != 28)
goto _LL19D;else{_tmp395=_tmp394->f1;_tmp396=_tmp394->f2;_tmp397=_tmp394->f3;
_tmp398=_tmp394->f4;}}_LL19C: _tmp38E=Cyc_Toc_init_comprehension(nv,lval,_tmp395,
_tmp396,_tmp397,_tmp398,_tmp38E,0);goto _LL198;_LL19D:;_LL19E: Cyc_Toc_exp_to_c(nv,
e);_tmp38E=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp38F(
_tmp38D,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp38E,0);goto _LL198;_LL198:;};}}
return Cyc_Toc_make_struct(nv,_tmp38C,_tmp38B,_tmp38E,pointer,rgnopt,is_atomic);};}
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct
_dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp399=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp399 != 0;_tmp399=_tmp399->tl){struct Cyc_Absyn_Aggrfield*
_tmp39A=(struct Cyc_Absyn_Aggrfield*)_tmp399->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp39A->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}{
struct Cyc_String_pa_struct _tmpB48;void*_tmpB47[1];const char*_tmpB46;void*_tmpB45;(
_tmpB45=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)((_tmpB48.tag=0,((_tmpB48.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmpB47[0]=& _tmpB48,Cyc_aprintf(((_tmpB46="get_member_offset %s failed",
_tag_dyneither(_tmpB46,sizeof(char),28))),_tag_dyneither(_tmpB47,sizeof(void*),1)))))))),
_tag_dyneither(_tmpB45,sizeof(void*),0)));};}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){struct _tuple0*_tmp39F=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3A0=Cyc_Absyn_var_exp(_tmp39F,0);struct Cyc_Absyn_Stmt*
_tmp3A1=Cyc_Absyn_exp_stmt(_tmp3A0,0);struct Cyc_Absyn_Exp*(*_tmp3A2)(struct Cyc_Absyn_Exp*,
struct _dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;void*_tmp3A3=Cyc_Toc_aggrdecl_type(tdn,Cyc_Absyn_strctq);
int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp3A4=Cyc_Tcutil_compress(
struct_type);struct Cyc_Absyn_AggrInfo _tmp3A6;union Cyc_Absyn_AggrInfoU _tmp3A7;
_LL1A0: {struct Cyc_Absyn_AggrType_struct*_tmp3A5=(struct Cyc_Absyn_AggrType_struct*)
_tmp3A4;if(_tmp3A5->tag != 12)goto _LL1A2;else{_tmp3A6=_tmp3A5->f1;_tmp3A7=_tmp3A6.aggr_info;}}
_LL1A1: ad=Cyc_Absyn_get_known_aggrdecl(_tmp3A7);goto _LL19F;_LL1A2:;_LL1A3: {
const char*_tmpB4B;void*_tmpB4A;(_tmpB4A=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4B="init_struct: bad struct type",
_tag_dyneither(_tmpB4B,sizeof(char),29))),_tag_dyneither(_tmpB4A,sizeof(void*),0)));}
_LL19F:;}{int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct Cyc_Toc_Env _tmp3AB;struct _RegionHandle*_tmp3AC;struct Cyc_Toc_Env*_tmp3AA=
nv;_tmp3AB=*_tmp3AA;_tmp3AC=_tmp3AB.rgn;{struct Cyc_List_List*_tmp3AD=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp3AC,dles);for(0;
_tmp3AD != 0;_tmp3AD=_tmp3AD->tl){struct _tuple15 _tmp3AF;struct Cyc_List_List*
_tmp3B0;struct Cyc_Absyn_Exp*_tmp3B1;struct _tuple15*_tmp3AE=(struct _tuple15*)
_tmp3AD->hd;_tmp3AF=*_tmp3AE;_tmp3B0=_tmp3AF.f1;_tmp3B1=_tmp3AF.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3B1->topt))->v);
if(_tmp3B0 == 0){const char*_tmpB4E;void*_tmpB4D;(_tmpB4D=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4E="empty designator list",
_tag_dyneither(_tmpB4E,sizeof(char),22))),_tag_dyneither(_tmpB4D,sizeof(void*),0)));}
if(_tmp3B0->tl != 0){struct _tuple0*_tmp3B4=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp3B5=Cyc_Absyn_var_exp(_tmp3B4,0);for(0;_tmp3B0 != 0;_tmp3B0=_tmp3B0->tl){void*
_tmp3B6=(void*)_tmp3B0->hd;struct _dyneither_ptr*_tmp3B8;_LL1A5: {struct Cyc_Absyn_FieldName_struct*
_tmp3B7=(struct Cyc_Absyn_FieldName_struct*)_tmp3B6;if(_tmp3B7->tag != 1)goto
_LL1A7;else{_tmp3B8=_tmp3B7->f1;}}_LL1A6: if(Cyc_Toc_is_poly_field(struct_type,
_tmp3B8))_tmp3B5=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3B5);_tmp3A1=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3A2(_tmp3A0,_tmp3B8,0),_tmp3B5,0),0),
_tmp3A1,0);goto _LL1A4;_LL1A7:;_LL1A8: {const char*_tmpB51;void*_tmpB50;(_tmpB50=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB51="array designator in struct",_tag_dyneither(_tmpB51,sizeof(char),27))),
_tag_dyneither(_tmpB50,sizeof(void*),0)));}_LL1A4:;}Cyc_Toc_exp_to_c(nv,_tmp3B1);
_tmp3A1=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3B5,
_tmp3B1,0),0),_tmp3A1,0);}else{void*_tmp3BB=(void*)_tmp3B0->hd;struct
_dyneither_ptr*_tmp3BD;_LL1AA: {struct Cyc_Absyn_FieldName_struct*_tmp3BC=(struct
Cyc_Absyn_FieldName_struct*)_tmp3BB;if(_tmp3BC->tag != 1)goto _LL1AC;else{_tmp3BD=
_tmp3BC->f1;}}_LL1AB: {struct Cyc_Absyn_Exp*lval=_tmp3A2(_tmp3A0,_tmp3BD,0);if(
is_tagged_union){int i=Cyc_Toc_get_member_offset(ad,_tmp3BD);struct Cyc_Absyn_Exp*
f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_aggrmember_exp(
lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,
f_tag_exp,0);_tmp3A1=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3A1,0);
lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{void*_tmp3BE=_tmp3B1->r;
struct Cyc_List_List*_tmp3C0;struct Cyc_Absyn_Vardecl*_tmp3C2;struct Cyc_Absyn_Exp*
_tmp3C3;struct Cyc_Absyn_Exp*_tmp3C4;int _tmp3C5;void*_tmp3C7;struct Cyc_List_List*
_tmp3C8;_LL1AF: {struct Cyc_Absyn_Array_e_struct*_tmp3BF=(struct Cyc_Absyn_Array_e_struct*)
_tmp3BE;if(_tmp3BF->tag != 27)goto _LL1B1;else{_tmp3C0=_tmp3BF->f1;}}_LL1B0:
_tmp3A1=Cyc_Toc_init_array(nv,lval,_tmp3C0,_tmp3A1);goto _LL1AE;_LL1B1: {struct
Cyc_Absyn_Comprehension_e_struct*_tmp3C1=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3BE;if(_tmp3C1->tag != 28)goto _LL1B3;else{_tmp3C2=_tmp3C1->f1;_tmp3C3=_tmp3C1->f2;
_tmp3C4=_tmp3C1->f3;_tmp3C5=_tmp3C1->f4;}}_LL1B2: _tmp3A1=Cyc_Toc_init_comprehension(
nv,lval,_tmp3C2,_tmp3C3,_tmp3C4,_tmp3C5,_tmp3A1,0);goto _LL1AE;_LL1B3: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3C6=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3BE;if(_tmp3C6->tag != 30)goto
_LL1B5;else{_tmp3C7=(void*)_tmp3C6->f1;_tmp3C8=_tmp3C6->f2;}}_LL1B4: _tmp3A1=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3C7,_tmp3C8,_tmp3A1);goto _LL1AE;_LL1B5:;_LL1B6: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3B1->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp3B1);{struct Cyc_Absyn_Aggrfield*_tmp3C9=Cyc_Absyn_lookup_decl_field(ad,
_tmp3BD);if(Cyc_Toc_is_poly_field(struct_type,_tmp3BD) && !was_ptr_type)_tmp3B1=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3B1);if(has_exists)_tmp3B1=Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3C9))->type),
_tmp3B1);_tmp3A1=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp3B1,0),0),_tmp3A1,0);goto _LL1AE;};}_LL1AE:;}goto _LL1A9;}_LL1AC:;_LL1AD: {
const char*_tmpB54;void*_tmpB53;(_tmpB53=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB54="array designator in struct",
_tag_dyneither(_tmpB54,sizeof(char),27))),_tag_dyneither(_tmpB53,sizeof(void*),0)));}
_LL1A9:;}}}return Cyc_Toc_make_struct(nv,_tmp39F,_tmp3A3,_tmp3A1,pointer,rgnopt,
is_atomic);};}struct _tuple18{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct
_tuple18*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(
e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,
enum Cyc_Absyn_Incrementor incr){struct Cyc_Absyn_Increment_e_struct _tmpB57;struct
Cyc_Absyn_Increment_e_struct*_tmpB56;return Cyc_Absyn_new_exp((void*)((_tmpB56=
_cycalloc(sizeof(*_tmpB56)),((_tmpB56[0]=((_tmpB57.tag=5,((_tmpB57.f1=e1,((
_tmpB57.f2=incr,_tmpB57)))))),_tmpB56)))),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp3CE=e1->r;struct Cyc_Absyn_Stmt*_tmp3D0;void*_tmp3D2;struct Cyc_Absyn_Exp*
_tmp3D3;struct Cyc_Absyn_Exp*_tmp3D5;struct _dyneither_ptr*_tmp3D6;int _tmp3D7;int
_tmp3D8;_LL1B8: {struct Cyc_Absyn_StmtExp_e_struct*_tmp3CF=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp3CE;if(_tmp3CF->tag != 37)goto _LL1BA;else{_tmp3D0=_tmp3CF->f1;}}_LL1B9: Cyc_Toc_lvalue_assign_stmt(
_tmp3D0,fs,f,f_env);goto _LL1B7;_LL1BA: {struct Cyc_Absyn_Cast_e_struct*_tmp3D1=(
struct Cyc_Absyn_Cast_e_struct*)_tmp3CE;if(_tmp3D1->tag != 15)goto _LL1BC;else{
_tmp3D2=(void*)_tmp3D1->f1;_tmp3D3=_tmp3D1->f2;}}_LL1BB: Cyc_Toc_lvalue_assign(
_tmp3D3,fs,f,f_env);goto _LL1B7;_LL1BC: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp3D4=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp3CE;if(_tmp3D4->tag != 22)goto
_LL1BE;else{_tmp3D5=_tmp3D4->f1;_tmp3D6=_tmp3D4->f2;_tmp3D7=_tmp3D4->f3;_tmp3D8=
_tmp3D4->f4;}}_LL1BD: e1->r=_tmp3D5->r;{struct Cyc_List_List*_tmpB58;Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)((_tmpB58=_cycalloc(sizeof(*_tmpB58)),((_tmpB58->hd=
_tmp3D6,((_tmpB58->tl=fs,_tmpB58)))))),f,f_env);}goto _LL1B7;_LL1BE:;_LL1BF: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;goto _LL1B7;}_LL1B7:;}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env){void*_tmp3DA=s->r;struct Cyc_Absyn_Exp*_tmp3DC;struct Cyc_Absyn_Decl*
_tmp3DE;struct Cyc_Absyn_Stmt*_tmp3DF;struct Cyc_Absyn_Stmt*_tmp3E1;_LL1C1: {
struct Cyc_Absyn_Exp_s_struct*_tmp3DB=(struct Cyc_Absyn_Exp_s_struct*)_tmp3DA;if(
_tmp3DB->tag != 1)goto _LL1C3;else{_tmp3DC=_tmp3DB->f1;}}_LL1C2: Cyc_Toc_lvalue_assign(
_tmp3DC,fs,f,f_env);goto _LL1C0;_LL1C3: {struct Cyc_Absyn_Decl_s_struct*_tmp3DD=(
struct Cyc_Absyn_Decl_s_struct*)_tmp3DA;if(_tmp3DD->tag != 12)goto _LL1C5;else{
_tmp3DE=_tmp3DD->f1;_tmp3DF=_tmp3DD->f2;}}_LL1C4: Cyc_Toc_lvalue_assign_stmt(
_tmp3DF,fs,f,f_env);goto _LL1C0;_LL1C5: {struct Cyc_Absyn_Seq_s_struct*_tmp3E0=(
struct Cyc_Absyn_Seq_s_struct*)_tmp3DA;if(_tmp3E0->tag != 2)goto _LL1C7;else{
_tmp3E1=_tmp3E0->f2;}}_LL1C6: Cyc_Toc_lvalue_assign_stmt(_tmp3E1,fs,f,f_env);goto
_LL1C0;_LL1C7:;_LL1C8: {const char*_tmpB5C;void*_tmpB5B[1];struct Cyc_String_pa_struct
_tmpB5A;(_tmpB5A.tag=0,((_tmpB5A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmpB5B[0]=& _tmpB5A,Cyc_Toc_toc_impos(((_tmpB5C="lvalue_assign_stmt: %s",
_tag_dyneither(_tmpB5C,sizeof(char),23))),_tag_dyneither(_tmpB5B,sizeof(void*),1)))))));}
_LL1C0:;}static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*
e){void*_tmp3E5=e->r;void*_tmp3E7;void**_tmp3E8;struct Cyc_Absyn_Exp*_tmp3E9;
struct Cyc_Absyn_Exp**_tmp3EA;struct Cyc_Absyn_Exp*_tmp3EC;_LL1CA: {struct Cyc_Absyn_Cast_e_struct*
_tmp3E6=(struct Cyc_Absyn_Cast_e_struct*)_tmp3E5;if(_tmp3E6->tag != 15)goto _LL1CC;
else{_tmp3E7=(void**)& _tmp3E6->f1;_tmp3E8=(void**)((void**)& _tmp3E6->f1);_tmp3E9=
_tmp3E6->f2;_tmp3EA=(struct Cyc_Absyn_Exp**)& _tmp3E6->f2;}}_LL1CB:*_tmp3EA=Cyc_Toc_push_address_exp(*
_tmp3EA);*_tmp3E8=Cyc_Absyn_cstar_typ(*_tmp3E8,Cyc_Toc_mt_tq);return e;_LL1CC: {
struct Cyc_Absyn_Deref_e_struct*_tmp3EB=(struct Cyc_Absyn_Deref_e_struct*)_tmp3E5;
if(_tmp3EB->tag != 21)goto _LL1CE;else{_tmp3EC=_tmp3EB->f1;}}_LL1CD: return _tmp3EC;
_LL1CE:;_LL1CF: if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{const
char*_tmpB60;void*_tmpB5F[1];struct Cyc_String_pa_struct _tmpB5E;(_tmpB5E.tag=0,((
_tmpB5E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmpB5F[0]=& _tmpB5E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB60="can't take & of %s",_tag_dyneither(_tmpB60,sizeof(char),19))),
_tag_dyneither(_tmpB5F,sizeof(void*),1)))))));};_LL1C9:;}static struct Cyc_List_List*
Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x){struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0)return 0;{struct
Cyc_List_List*_tmpB61;result=((_tmpB61=_region_malloc(r2,sizeof(*_tmpB61)),((
_tmpB61->hd=(void*)f((void*)x->hd,env),((_tmpB61->tl=0,_tmpB61))))));}prev=
result;for(x=x->tl;x != 0;x=x->tl){{struct Cyc_List_List*_tmpB62;((struct Cyc_List_List*)
_check_null(prev))->tl=((_tmpB62=_region_malloc(r2,sizeof(*_tmpB62)),((_tmpB62->hd=(
void*)f((void*)x->hd,env),((_tmpB62->tl=0,_tmpB62))))));}prev=prev->tl;}return
result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,
struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
static struct _tuple15*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){struct _tuple15*
_tmpB63;return(_tmpB63=_cycalloc(sizeof(*_tmpB63)),((_tmpB63->f1=0,((_tmpB63->f2=
e,_tmpB63)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*
_tmp3F3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp3F5;_LL1D1: {struct Cyc_Absyn_PointerType_struct*
_tmp3F4=(struct Cyc_Absyn_PointerType_struct*)_tmp3F3;if(_tmp3F4->tag != 5)goto
_LL1D3;else{_tmp3F5=_tmp3F4->f1;}}_LL1D2: return _tmp3F5;_LL1D3:;_LL1D4: {const
char*_tmpB66;void*_tmpB65;(_tmpB65=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB66="get_ptr_typ: not a pointer!",
_tag_dyneither(_tmpB66,sizeof(char),28))),_tag_dyneither(_tmpB65,sizeof(void*),0)));}
_LL1D0:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp3F8=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp3FB;enum Cyc_Absyn_Size_of
_tmp3FC;enum Cyc_Absyn_Sign _tmp3FE;enum Cyc_Absyn_Size_of _tmp3FF;enum Cyc_Absyn_Sign
_tmp403;enum Cyc_Absyn_Size_of _tmp404;enum Cyc_Absyn_Sign _tmp406;enum Cyc_Absyn_Size_of
_tmp407;enum Cyc_Absyn_Sign _tmp409;enum Cyc_Absyn_Size_of _tmp40A;_LL1D6: {struct
Cyc_Absyn_PointerType_struct*_tmp3F9=(struct Cyc_Absyn_PointerType_struct*)
_tmp3F8;if(_tmp3F9->tag != 5)goto _LL1D8;}_LL1D7: res=Cyc_Absyn_null_exp(0);goto
_LL1D5;_LL1D8: {struct Cyc_Absyn_IntType_struct*_tmp3FA=(struct Cyc_Absyn_IntType_struct*)
_tmp3F8;if(_tmp3FA->tag != 6)goto _LL1DA;else{_tmp3FB=_tmp3FA->f1;_tmp3FC=_tmp3FA->f2;
if(_tmp3FC != Cyc_Absyn_Char_sz)goto _LL1DA;}}_LL1D9: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(
_tmp3FB,'\000'),0);goto _LL1D5;_LL1DA: {struct Cyc_Absyn_IntType_struct*_tmp3FD=(
struct Cyc_Absyn_IntType_struct*)_tmp3F8;if(_tmp3FD->tag != 6)goto _LL1DC;else{
_tmp3FE=_tmp3FD->f1;_tmp3FF=_tmp3FD->f2;if(_tmp3FF != Cyc_Absyn_Short_sz)goto
_LL1DC;}}_LL1DB: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp3FE,0),0);goto
_LL1D5;_LL1DC: {struct Cyc_Absyn_EnumType_struct*_tmp400=(struct Cyc_Absyn_EnumType_struct*)
_tmp3F8;if(_tmp400->tag != 14)goto _LL1DE;}_LL1DD: goto _LL1DF;_LL1DE: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp401=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp3F8;if(_tmp401->tag != 15)goto
_LL1E0;}_LL1DF: _tmp403=Cyc_Absyn_Unsigned;goto _LL1E1;_LL1E0: {struct Cyc_Absyn_IntType_struct*
_tmp402=(struct Cyc_Absyn_IntType_struct*)_tmp3F8;if(_tmp402->tag != 6)goto _LL1E2;
else{_tmp403=_tmp402->f1;_tmp404=_tmp402->f2;if(_tmp404 != Cyc_Absyn_Int_sz)goto
_LL1E2;}}_LL1E1: _tmp406=_tmp403;goto _LL1E3;_LL1E2: {struct Cyc_Absyn_IntType_struct*
_tmp405=(struct Cyc_Absyn_IntType_struct*)_tmp3F8;if(_tmp405->tag != 6)goto _LL1E4;
else{_tmp406=_tmp405->f1;_tmp407=_tmp405->f2;if(_tmp407 != Cyc_Absyn_Long_sz)goto
_LL1E4;}}_LL1E3: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp406,0),0);goto _LL1D5;
_LL1E4: {struct Cyc_Absyn_IntType_struct*_tmp408=(struct Cyc_Absyn_IntType_struct*)
_tmp3F8;if(_tmp408->tag != 6)goto _LL1E6;else{_tmp409=_tmp408->f1;_tmp40A=_tmp408->f2;
if(_tmp40A != Cyc_Absyn_LongLong_sz)goto _LL1E6;}}_LL1E5: res=Cyc_Absyn_const_exp(
Cyc_Absyn_LongLong_c(_tmp409,(long long)0),0);goto _LL1D5;_LL1E6: {struct Cyc_Absyn_FloatType_struct*
_tmp40B=(struct Cyc_Absyn_FloatType_struct*)_tmp3F8;if(_tmp40B->tag != 7)goto
_LL1E8;}_LL1E7: goto _LL1E9;_LL1E8: {struct Cyc_Absyn_DoubleType_struct*_tmp40C=(
struct Cyc_Absyn_DoubleType_struct*)_tmp3F8;if(_tmp40C->tag != 8)goto _LL1EA;}
_LL1E9:{const char*_tmpB67;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB67="0.0",
_tag_dyneither(_tmpB67,sizeof(char),4)))),0);}goto _LL1D5;_LL1EA:;_LL1EB: {const
char*_tmpB6B;void*_tmpB6A[1];struct Cyc_String_pa_struct _tmpB69;(_tmpB69.tag=0,((
_tmpB69.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmpB6A[0]=& _tmpB69,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB6B="found non-zero type %s in generate_zero",_tag_dyneither(_tmpB6B,sizeof(
char),40))),_tag_dyneither(_tmpB6A,sizeof(void*),1)))))));}_LL1D5:;}{struct Cyc_Core_Opt*
_tmpB6C;res->topt=((_tmpB6C=_cycalloc(sizeof(*_tmpB6C)),((_tmpB6C->v=t,_tmpB6C))));}
return res;}static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dyneither,void*elt_type){void*fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);void*
_tmp412=Cyc_Toc_typ_to_c(elt_type);void*_tmp413=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp414=Cyc_Absyn_cstar_typ(_tmp412,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmpB6D;struct Cyc_Core_Opt*_tmp415=(_tmpB6D=_cycalloc(sizeof(*_tmpB6D)),((
_tmpB6D->v=_tmp414,_tmpB6D)));struct Cyc_Absyn_Exp*xinit;{void*_tmp416=e1->r;
struct Cyc_Absyn_Exp*_tmp418;struct Cyc_Absyn_Exp*_tmp41A;struct Cyc_Absyn_Exp*
_tmp41B;_LL1ED: {struct Cyc_Absyn_Deref_e_struct*_tmp417=(struct Cyc_Absyn_Deref_e_struct*)
_tmp416;if(_tmp417->tag != 21)goto _LL1EF;else{_tmp418=_tmp417->f1;}}_LL1EE: if(!
is_dyneither){_tmp418=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp418,0,Cyc_Absyn_Other_coercion,
0);{struct Cyc_Core_Opt*_tmpB6E;_tmp418->topt=((_tmpB6E=_cycalloc(sizeof(*_tmpB6E)),((
_tmpB6E->v=fat_ptr_type,_tmpB6E))));};}Cyc_Toc_exp_to_c(nv,_tmp418);xinit=
_tmp418;goto _LL1EC;_LL1EF: {struct Cyc_Absyn_Subscript_e_struct*_tmp419=(struct
Cyc_Absyn_Subscript_e_struct*)_tmp416;if(_tmp419->tag != 24)goto _LL1F1;else{
_tmp41A=_tmp419->f1;_tmp41B=_tmp419->f2;}}_LL1F0: if(!is_dyneither){_tmp41A=Cyc_Absyn_cast_exp(
fat_ptr_type,_tmp41A,0,Cyc_Absyn_Other_coercion,0);{struct Cyc_Core_Opt*_tmpB6F;
_tmp41A->topt=((_tmpB6F=_cycalloc(sizeof(*_tmpB6F)),((_tmpB6F->v=fat_ptr_type,
_tmpB6F))));};}Cyc_Toc_exp_to_c(nv,_tmp41A);Cyc_Toc_exp_to_c(nv,_tmp41B);{struct
Cyc_Absyn_Exp*_tmpB70[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,((
_tmpB70[2]=_tmp41B,((_tmpB70[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmpB70[0]=_tmp41A,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpB70,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1EC;_LL1F1:;_LL1F2: {const char*_tmpB73;void*_tmpB72;(_tmpB72=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB73="found bad lhs for zero-terminated pointer assignment",
_tag_dyneither(_tmpB73,sizeof(char),53))),_tag_dyneither(_tmpB72,sizeof(void*),0)));}
_LL1EC:;}{struct _tuple0*_tmp421=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp423;
struct _RegionHandle*_tmp424;struct Cyc_Toc_Env*_tmp422=nv;_tmp423=*_tmp422;
_tmp424=_tmp423.rgn;{struct Cyc_Toc_Env*_tmp425=Cyc_Toc_add_varmap(_tmp424,nv,
_tmp421,Cyc_Absyn_var_exp(_tmp421,0));struct Cyc_Absyn_Vardecl*_tmpB74;struct Cyc_Absyn_Vardecl*
_tmp426=(_tmpB74=_cycalloc(sizeof(*_tmpB74)),((_tmpB74->sc=Cyc_Absyn_Public,((
_tmpB74->name=_tmp421,((_tmpB74->tq=Cyc_Toc_mt_tq,((_tmpB74->type=_tmp413,((
_tmpB74->initializer=(struct Cyc_Absyn_Exp*)xinit,((_tmpB74->rgn=0,((_tmpB74->attributes=
0,((_tmpB74->escapes=0,_tmpB74)))))))))))))))));struct Cyc_Absyn_Local_b_struct
_tmpB77;struct Cyc_Absyn_Local_b_struct*_tmpB76;struct Cyc_Absyn_Local_b_struct*
_tmp427=(_tmpB76=_cycalloc(sizeof(*_tmpB76)),((_tmpB76[0]=((_tmpB77.tag=4,((
_tmpB77.f1=_tmp426,_tmpB77)))),_tmpB76)));struct Cyc_Absyn_Exp*_tmp428=Cyc_Absyn_varb_exp(
_tmp421,(void*)_tmp427,0);{struct Cyc_Core_Opt*_tmpB78;_tmp428->topt=((_tmpB78=
_cycalloc(sizeof(*_tmpB78)),((_tmpB78->v=fat_ptr_type,_tmpB78))));}{struct Cyc_Absyn_Exp*
_tmp42A=Cyc_Absyn_deref_exp(_tmp428,0);{struct Cyc_Core_Opt*_tmpB79;_tmp42A->topt=((
_tmpB79=_cycalloc(sizeof(*_tmpB79)),((_tmpB79->v=elt_type,_tmpB79))));}Cyc_Toc_exp_to_c(
_tmp425,_tmp42A);{struct _tuple0*_tmp42C=Cyc_Toc_temp_var();_tmp425=Cyc_Toc_add_varmap(
_tmp424,_tmp425,_tmp42C,Cyc_Absyn_var_exp(_tmp42C,0));{struct Cyc_Absyn_Vardecl*
_tmpB7A;struct Cyc_Absyn_Vardecl*_tmp42D=(_tmpB7A=_cycalloc(sizeof(*_tmpB7A)),((
_tmpB7A->sc=Cyc_Absyn_Public,((_tmpB7A->name=_tmp42C,((_tmpB7A->tq=Cyc_Toc_mt_tq,((
_tmpB7A->type=_tmp412,((_tmpB7A->initializer=(struct Cyc_Absyn_Exp*)_tmp42A,((
_tmpB7A->rgn=0,((_tmpB7A->attributes=0,((_tmpB7A->escapes=0,_tmpB7A)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpB7D;struct Cyc_Absyn_Local_b_struct*_tmpB7C;
struct Cyc_Absyn_Local_b_struct*_tmp42E=(_tmpB7C=_cycalloc(sizeof(*_tmpB7C)),((
_tmpB7C[0]=((_tmpB7D.tag=4,((_tmpB7D.f1=_tmp42D,_tmpB7D)))),_tmpB7C)));struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp42F=Cyc_Absyn_varb_exp(_tmp42C,(
void*)_tmp42E,0);_tmp42F->topt=_tmp42A->topt;z_init=Cyc_Absyn_prim2_exp((enum 
Cyc_Absyn_Primop)popt->v,_tmp42F,e2,0);z_init->topt=_tmp42F->topt;}Cyc_Toc_exp_to_c(
_tmp425,z_init);{struct _tuple0*_tmp430=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmpB7E;struct Cyc_Absyn_Vardecl*_tmp431=(_tmpB7E=_cycalloc(sizeof(*_tmpB7E)),((
_tmpB7E->sc=Cyc_Absyn_Public,((_tmpB7E->name=_tmp430,((_tmpB7E->tq=Cyc_Toc_mt_tq,((
_tmpB7E->type=_tmp412,((_tmpB7E->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmpB7E->rgn=0,((_tmpB7E->attributes=0,((_tmpB7E->escapes=0,_tmpB7E)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpB81;struct Cyc_Absyn_Local_b_struct*_tmpB80;
struct Cyc_Absyn_Local_b_struct*_tmp432=(_tmpB80=_cycalloc(sizeof(*_tmpB80)),((
_tmpB80[0]=((_tmpB81.tag=4,((_tmpB81.f1=_tmp431,_tmpB81)))),_tmpB80)));_tmp425=
Cyc_Toc_add_varmap(_tmp424,_tmp425,_tmp430,Cyc_Absyn_var_exp(_tmp430,0));{struct
Cyc_Absyn_Exp*_tmp433=Cyc_Absyn_varb_exp(_tmp42C,(void*)_tmp42E,0);_tmp433->topt=
_tmp42A->topt;{struct Cyc_Absyn_Exp*_tmp434=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp435=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp433,_tmp434,0);{
struct Cyc_Core_Opt*_tmpB82;_tmp435->topt=((_tmpB82=_cycalloc(sizeof(*_tmpB82)),((
_tmpB82->v=Cyc_Absyn_sint_typ,_tmpB82))));}Cyc_Toc_exp_to_c(_tmp425,_tmp435);{
struct Cyc_Absyn_Exp*_tmp437=Cyc_Absyn_varb_exp(_tmp430,(void*)_tmp432,0);_tmp437->topt=
_tmp42A->topt;{struct Cyc_Absyn_Exp*_tmp438=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp439=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp437,_tmp438,0);{
struct Cyc_Core_Opt*_tmpB83;_tmp439->topt=((_tmpB83=_cycalloc(sizeof(*_tmpB83)),((
_tmpB83->v=Cyc_Absyn_sint_typ,_tmpB83))));}Cyc_Toc_exp_to_c(_tmp425,_tmp439);{
struct Cyc_Absyn_Exp*_tmpB84[2];struct Cyc_List_List*_tmp43B=(_tmpB84[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0),((_tmpB84[0]=Cyc_Absyn_varb_exp(_tmp421,(void*)
_tmp427,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB84,sizeof(struct Cyc_Absyn_Exp*),2)))));struct Cyc_Absyn_Exp*
_tmp43C=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;xsize=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp43B,0),
_tmp43C,0);{struct Cyc_Absyn_Exp*_tmp43D=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp435,_tmp439,0),0);struct Cyc_Absyn_Stmt*_tmp43E=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp43F=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp421,(void*)_tmp427,0),Cyc_Toc_curr_sp,0);_tmp43F=Cyc_Toc_cast_it(
_tmp414,_tmp43F);{struct Cyc_Absyn_Exp*_tmp440=Cyc_Absyn_deref_exp(_tmp43F,0);
struct Cyc_Absyn_Exp*_tmp441=Cyc_Absyn_assign_exp(_tmp440,Cyc_Absyn_var_exp(
_tmp430,0),0);struct Cyc_Absyn_Stmt*_tmp442=Cyc_Absyn_exp_stmt(_tmp441,0);_tmp442=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp43D,_tmp43E,Cyc_Absyn_skip_stmt(
0),0),_tmp442,0);{struct Cyc_Absyn_Var_d_struct*_tmpB8A;struct Cyc_Absyn_Var_d_struct
_tmpB89;struct Cyc_Absyn_Decl*_tmpB88;_tmp442=Cyc_Absyn_decl_stmt(((_tmpB88=
_cycalloc(sizeof(*_tmpB88)),((_tmpB88->r=(void*)((_tmpB8A=_cycalloc(sizeof(*
_tmpB8A)),((_tmpB8A[0]=((_tmpB89.tag=0,((_tmpB89.f1=_tmp431,_tmpB89)))),_tmpB8A)))),((
_tmpB88->loc=0,_tmpB88)))))),_tmp442,0);}{struct Cyc_Absyn_Var_d_struct*_tmpB90;
struct Cyc_Absyn_Var_d_struct _tmpB8F;struct Cyc_Absyn_Decl*_tmpB8E;_tmp442=Cyc_Absyn_decl_stmt(((
_tmpB8E=_cycalloc(sizeof(*_tmpB8E)),((_tmpB8E->r=(void*)((_tmpB90=_cycalloc(
sizeof(*_tmpB90)),((_tmpB90[0]=((_tmpB8F.tag=0,((_tmpB8F.f1=_tmp42D,_tmpB8F)))),
_tmpB90)))),((_tmpB8E->loc=0,_tmpB8E)))))),_tmp442,0);}{struct Cyc_Absyn_Var_d_struct*
_tmpB96;struct Cyc_Absyn_Var_d_struct _tmpB95;struct Cyc_Absyn_Decl*_tmpB94;_tmp442=
Cyc_Absyn_decl_stmt(((_tmpB94=_cycalloc(sizeof(*_tmpB94)),((_tmpB94->r=(void*)((
_tmpB96=_cycalloc(sizeof(*_tmpB96)),((_tmpB96[0]=((_tmpB95.tag=0,((_tmpB95.f1=
_tmp426,_tmpB95)))),_tmpB96)))),((_tmpB94->loc=0,_tmpB94)))))),_tmp442,0);}e->r=
Cyc_Toc_stmt_exp_r(_tmp442);};};};};};};};};};};};};};}static void*Cyc_Toc_check_tagged_union(
struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,
struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,
struct Cyc_Position_Segment*)){struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp457=Cyc_Tcutil_compress(
aggrtype);struct Cyc_Absyn_AggrInfo _tmp459;union Cyc_Absyn_AggrInfoU _tmp45A;_LL1F4: {
struct Cyc_Absyn_AggrType_struct*_tmp458=(struct Cyc_Absyn_AggrType_struct*)
_tmp457;if(_tmp458->tag != 12)goto _LL1F6;else{_tmp459=_tmp458->f1;_tmp45A=_tmp459.aggr_info;}}
_LL1F5: ad=Cyc_Absyn_get_known_aggrdecl(_tmp45A);goto _LL1F3;_LL1F6:;_LL1F7: {
struct Cyc_String_pa_struct _tmpB9E;void*_tmpB9D[1];const char*_tmpB9C;void*_tmpB9B;(
_tmpB9B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((
struct _dyneither_ptr)((_tmpB9E.tag=0,((_tmpB9E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpB9D[0]=& _tmpB9E,Cyc_aprintf(((
_tmpB9C="expecting union but found %s in check_tagged_union",_tag_dyneither(
_tmpB9C,sizeof(char),51))),_tag_dyneither(_tmpB9D,sizeof(void*),1)))))))),
_tag_dyneither(_tmpB9B,sizeof(void*),0)));}_LL1F3:;}{struct _tuple0*_tmp45F=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp460=Cyc_Absyn_var_exp(_tmp45F,0);struct Cyc_Absyn_Exp*
_tmp461=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp460,f,0),Cyc_Toc_tag_sp,0);struct
Cyc_Absyn_Exp*_tmp462=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
struct Cyc_Absyn_Exp*_tmp463=Cyc_Absyn_neq_exp(_tmp461,_tmp462,0);struct Cyc_Absyn_Exp*
_tmp464=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp460,f,0),Cyc_Toc_val_sp,0);struct
Cyc_Absyn_Stmt*_tmp465=Cyc_Absyn_exp_stmt(_tmp464,0);struct Cyc_Absyn_Stmt*
_tmp466=Cyc_Absyn_ifthenelse_stmt(_tmp463,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp467=Cyc_Absyn_declare_stmt(_tmp45F,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp466,_tmp465,0),0);return Cyc_Toc_stmt_exp_r(
_tmp467);};}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read){void*_tmp468=e->r;struct Cyc_Absyn_Exp*
_tmp46A;struct Cyc_Absyn_Exp*_tmp46C;struct _dyneither_ptr*_tmp46D;int _tmp46E;int*
_tmp46F;struct Cyc_Absyn_Exp*_tmp471;struct _dyneither_ptr*_tmp472;int _tmp473;int*
_tmp474;_LL1F9: {struct Cyc_Absyn_Cast_e_struct*_tmp469=(struct Cyc_Absyn_Cast_e_struct*)
_tmp468;if(_tmp469->tag != 15)goto _LL1FB;else{_tmp46A=_tmp469->f2;}}_LL1FA: {
const char*_tmpBA1;void*_tmpBA0;(_tmpBA0=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA1="cast on lhs!",
_tag_dyneither(_tmpBA1,sizeof(char),13))),_tag_dyneither(_tmpBA0,sizeof(void*),0)));}
_LL1FB: {struct Cyc_Absyn_AggrMember_e_struct*_tmp46B=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp468;if(_tmp46B->tag != 22)goto _LL1FD;else{_tmp46C=_tmp46B->f1;_tmp46D=_tmp46B->f2;
_tmp46E=_tmp46B->f4;_tmp46F=(int*)& _tmp46B->f4;}}_LL1FC: {void*_tmp477=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp46C->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp479;union Cyc_Absyn_AggrInfoU _tmp47A;_LL202: {struct Cyc_Absyn_AggrType_struct*
_tmp478=(struct Cyc_Absyn_AggrType_struct*)_tmp477;if(_tmp478->tag != 12)goto
_LL204;else{_tmp479=_tmp478->f1;_tmp47A=_tmp479.aggr_info;}}_LL203: {struct Cyc_Absyn_Aggrdecl*
_tmp47B=Cyc_Absyn_get_known_aggrdecl(_tmp47A);*f_tag=Cyc_Toc_get_member_offset(
_tmp47B,_tmp46D);{const char*_tmpBA6;void*_tmpBA5[2];struct Cyc_String_pa_struct
_tmpBA4;struct Cyc_String_pa_struct _tmpBA3;struct _dyneither_ptr str=(struct
_dyneither_ptr)((_tmpBA3.tag=0,((_tmpBA3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp46D),((_tmpBA4.tag=0,((_tmpBA4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp47B->name).f2),((_tmpBA5[0]=& _tmpBA4,((_tmpBA5[1]=&
_tmpBA3,Cyc_aprintf(((_tmpBA6="_union_%s_%s",_tag_dyneither(_tmpBA6,sizeof(char),
13))),_tag_dyneither(_tmpBA5,sizeof(void*),2))))))))))))));{struct _dyneither_ptr*
_tmpBA7;*tagged_member_type=Cyc_Absyn_strct(((_tmpBA7=_cycalloc(sizeof(*_tmpBA7)),((
_tmpBA7[0]=str,_tmpBA7)))));}if(clear_read)*_tmp46F=0;return((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp47B->impl))->tagged;};}_LL204:;_LL205: return 0;_LL201:;}_LL1FD: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp470=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp468;if(_tmp470->tag != 23)goto _LL1FF;else{_tmp471=_tmp470->f1;_tmp472=_tmp470->f2;
_tmp473=_tmp470->f4;_tmp474=(int*)& _tmp470->f4;}}_LL1FE: {void*_tmp481=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp471->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp483;void*_tmp484;_LL207: {struct Cyc_Absyn_PointerType_struct*_tmp482=(struct
Cyc_Absyn_PointerType_struct*)_tmp481;if(_tmp482->tag != 5)goto _LL209;else{
_tmp483=_tmp482->f1;_tmp484=_tmp483.elt_typ;}}_LL208: {void*_tmp485=Cyc_Tcutil_compress(
_tmp484);struct Cyc_Absyn_AggrInfo _tmp487;union Cyc_Absyn_AggrInfoU _tmp488;_LL20C: {
struct Cyc_Absyn_AggrType_struct*_tmp486=(struct Cyc_Absyn_AggrType_struct*)
_tmp485;if(_tmp486->tag != 12)goto _LL20E;else{_tmp487=_tmp486->f1;_tmp488=_tmp487.aggr_info;}}
_LL20D: {struct Cyc_Absyn_Aggrdecl*_tmp489=Cyc_Absyn_get_known_aggrdecl(_tmp488);*
f_tag=Cyc_Toc_get_member_offset(_tmp489,_tmp472);{const char*_tmpBAC;void*_tmpBAB[
2];struct Cyc_String_pa_struct _tmpBAA;struct Cyc_String_pa_struct _tmpBA9;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpBA9.tag=0,((_tmpBA9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp472),((_tmpBAA.tag=0,((_tmpBAA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp489->name).f2),((_tmpBAB[0]=&
_tmpBAA,((_tmpBAB[1]=& _tmpBA9,Cyc_aprintf(((_tmpBAC="_union_%s_%s",
_tag_dyneither(_tmpBAC,sizeof(char),13))),_tag_dyneither(_tmpBAB,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpBAD;*tagged_member_type=Cyc_Absyn_strct(((_tmpBAD=
_cycalloc(sizeof(*_tmpBAD)),((_tmpBAD[0]=str,_tmpBAD)))));}if(clear_read)*
_tmp474=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp489->impl))->tagged;};}
_LL20E:;_LL20F: return 0;_LL20B:;}_LL209:;_LL20A: return 0;_LL206:;}_LL1FF:;_LL200:
return 0;_LL1F8:;}void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*
e,int tag,void*mem_type){struct _tuple0*_tmp48F=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
temp_exp=Cyc_Absyn_var_exp(_tmp48F,0);struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(
tag,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,
0),0);struct Cyc_Absyn_Exp*_tmp490=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_ifthenelse_stmt(_tmp490,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp48F,Cyc_Absyn_cstar_typ(
mem_type,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(
e)),Cyc_Absyn_seq_stmt(s2,s3,0),0);e->r=Cyc_Toc_stmt_exp_r(s1);}static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type){struct _tuple0*_tmp491=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp491,0);struct Cyc_Absyn_Exp*
temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*
temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 
0)s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*
_tmp492=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp492,
Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=
Cyc_Absyn_declare_stmt(_tmp491,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(e1),Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple19{struct _tuple0*f1;void*f2;struct
Cyc_Absyn_Exp*f3;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*
f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void _tmpC00(unsigned int*_tmpBFF,unsigned int*_tmpBFE,struct _tuple0***
_tmpBFC){for(*_tmpBFF=0;*_tmpBFF < *_tmpBFE;(*_tmpBFF)++){(*_tmpBFC)[*_tmpBFF]=
Cyc_Toc_temp_var();}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){void*_tmp493=e->r;if(e->topt == 0){const char*_tmpBB1;void*_tmpBB0[1];struct Cyc_String_pa_struct
_tmpBAF;(_tmpBAF.tag=0,((_tmpBAF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpBB0[0]=& _tmpBAF,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB1="exp_to_c: no type for %s",
_tag_dyneither(_tmpBB1,sizeof(char),25))),_tag_dyneither(_tmpBB0,sizeof(void*),1)))))));}{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp497=
_tmp493;union Cyc_Absyn_Cnst _tmp499;int _tmp49A;struct _tuple0*_tmp49D;void*_tmp49E;
struct _tuple0*_tmp4A0;enum Cyc_Absyn_Primop _tmp4A2;struct Cyc_List_List*_tmp4A3;
struct Cyc_Absyn_Exp*_tmp4A5;enum Cyc_Absyn_Incrementor _tmp4A6;struct Cyc_Absyn_Exp*
_tmp4A8;struct Cyc_Core_Opt*_tmp4A9;struct Cyc_Absyn_Exp*_tmp4AA;struct Cyc_Absyn_Exp*
_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AE;struct Cyc_Absyn_Exp*
_tmp4B0;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_Absyn_Exp*
_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B6;struct Cyc_Absyn_Exp*_tmp4B7;struct Cyc_Absyn_Exp*
_tmp4B9;struct Cyc_List_List*_tmp4BA;struct Cyc_Absyn_Exp*_tmp4BC;struct Cyc_List_List*
_tmp4BD;struct Cyc_Absyn_VarargCallInfo*_tmp4BE;struct Cyc_Absyn_Exp*_tmp4C0;
struct Cyc_List_List*_tmp4C1;struct Cyc_Absyn_VarargCallInfo*_tmp4C2;struct Cyc_Absyn_VarargCallInfo
_tmp4C3;int _tmp4C4;struct Cyc_List_List*_tmp4C5;struct Cyc_Absyn_VarargInfo*
_tmp4C6;struct Cyc_Absyn_Exp*_tmp4C8;struct Cyc_Absyn_Exp*_tmp4CA;struct Cyc_Absyn_Exp*
_tmp4CC;struct Cyc_List_List*_tmp4CD;void*_tmp4CF;void**_tmp4D0;struct Cyc_Absyn_Exp*
_tmp4D1;int _tmp4D2;enum Cyc_Absyn_Coercion _tmp4D3;struct Cyc_Absyn_Exp*_tmp4D5;
struct Cyc_Absyn_Exp*_tmp4D7;struct Cyc_Absyn_Exp*_tmp4D8;struct Cyc_Absyn_Exp*
_tmp4DA;void*_tmp4DC;void*_tmp4DE;void*_tmp4DF;struct _dyneither_ptr*_tmp4E1;void*
_tmp4E3;void*_tmp4E4;unsigned int _tmp4E6;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_Absyn_Exp*
_tmp4EA;struct _dyneither_ptr*_tmp4EB;int _tmp4EC;int _tmp4ED;struct Cyc_Absyn_Exp*
_tmp4EF;struct _dyneither_ptr*_tmp4F0;int _tmp4F1;int _tmp4F2;struct Cyc_Absyn_Exp*
_tmp4F4;struct Cyc_Absyn_Exp*_tmp4F5;struct Cyc_List_List*_tmp4F7;struct Cyc_List_List*
_tmp4F9;struct Cyc_Absyn_Vardecl*_tmp4FB;struct Cyc_Absyn_Exp*_tmp4FC;struct Cyc_Absyn_Exp*
_tmp4FD;int _tmp4FE;struct _tuple0*_tmp500;struct Cyc_List_List*_tmp501;struct Cyc_List_List*
_tmp502;struct Cyc_Absyn_Aggrdecl*_tmp503;void*_tmp505;struct Cyc_List_List*
_tmp506;struct Cyc_List_List*_tmp508;struct Cyc_Absyn_Datatypedecl*_tmp509;struct
Cyc_Absyn_Datatypefield*_tmp50A;struct Cyc_Absyn_MallocInfo _tmp50E;int _tmp50F;
struct Cyc_Absyn_Exp*_tmp510;void**_tmp511;struct Cyc_Absyn_Exp*_tmp512;int _tmp513;
struct Cyc_Absyn_Exp*_tmp515;struct Cyc_Absyn_Exp*_tmp516;struct Cyc_Absyn_Exp*
_tmp518;struct _dyneither_ptr*_tmp519;struct Cyc_Absyn_Stmt*_tmp51B;_LL211: {
struct Cyc_Absyn_Const_e_struct*_tmp498=(struct Cyc_Absyn_Const_e_struct*)_tmp497;
if(_tmp498->tag != 0)goto _LL213;else{_tmp499=_tmp498->f1;if((_tmp499.Null_c).tag
!= 1)goto _LL213;_tmp49A=(int)(_tmp499.Null_c).val;}}_LL212: {struct Cyc_Absyn_Exp*
_tmp520=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp520,
_tmp520))->r;else{struct Cyc_Absyn_Exp*_tmpBB2[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((
_tmpBB2[2]=_tmp520,((_tmpBB2[1]=_tmp520,((_tmpBB2[0]=_tmp520,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB2,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL210;}_LL213: {struct Cyc_Absyn_Const_e_struct*
_tmp49B=(struct Cyc_Absyn_Const_e_struct*)_tmp497;if(_tmp49B->tag != 0)goto _LL215;}
_LL214: goto _LL210;_LL215: {struct Cyc_Absyn_Var_e_struct*_tmp49C=(struct Cyc_Absyn_Var_e_struct*)
_tmp497;if(_tmp49C->tag != 1)goto _LL217;else{_tmp49D=_tmp49C->f1;_tmp49E=(void*)
_tmp49C->f2;}}_LL216:{struct _handler_cons _tmp522;_push_handler(& _tmp522);{int
_tmp524=0;if(setjmp(_tmp522.handler))_tmp524=1;if(!_tmp524){e->r=(Cyc_Toc_lookup_varmap(
nv,_tmp49D))->r;;_pop_handler();}else{void*_tmp523=(void*)_exn_thrown;void*
_tmp526=_tmp523;_LL26A: {struct Cyc_Dict_Absent_struct*_tmp527=(struct Cyc_Dict_Absent_struct*)
_tmp526;if(_tmp527->tag != Cyc_Dict_Absent)goto _LL26C;}_LL26B: {const char*_tmpBB6;
void*_tmpBB5[1];struct Cyc_String_pa_struct _tmpBB4;(_tmpBB4.tag=0,((_tmpBB4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp49D)),((
_tmpBB5[0]=& _tmpBB4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBB6="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpBB6,sizeof(char),
32))),_tag_dyneither(_tmpBB5,sizeof(void*),1)))))));}_LL26C:;_LL26D:(void)_throw(
_tmp526);_LL269:;}};}goto _LL210;_LL217: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp49F=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp497;if(_tmp49F->tag != 2)goto
_LL219;else{_tmp4A0=_tmp49F->f1;}}_LL218: {const char*_tmpBB9;void*_tmpBB8;(
_tmpBB8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBB9="unknownid",_tag_dyneither(_tmpBB9,sizeof(char),10))),_tag_dyneither(
_tmpBB8,sizeof(void*),0)));}_LL219: {struct Cyc_Absyn_Primop_e_struct*_tmp4A1=(
struct Cyc_Absyn_Primop_e_struct*)_tmp497;if(_tmp4A1->tag != 3)goto _LL21B;else{
_tmp4A2=_tmp4A1->f1;_tmp4A3=_tmp4A1->f2;}}_LL21A: {struct Cyc_List_List*_tmp52D=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4A3);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp4A3);switch(_tmp4A2){case Cyc_Absyn_Numelts: _LL26E: {
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp4A3))->hd;{void*_tmp52E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo _tmp530;struct Cyc_Absyn_Exp*
_tmp531;struct Cyc_Absyn_PtrInfo _tmp533;void*_tmp534;struct Cyc_Absyn_PtrAtts
_tmp535;union Cyc_Absyn_Constraint*_tmp536;union Cyc_Absyn_Constraint*_tmp537;
union Cyc_Absyn_Constraint*_tmp538;_LL271: {struct Cyc_Absyn_ArrayType_struct*
_tmp52F=(struct Cyc_Absyn_ArrayType_struct*)_tmp52E;if(_tmp52F->tag != 9)goto
_LL273;else{_tmp530=_tmp52F->f1;_tmp531=_tmp530.num_elts;}}_LL272: if(!Cyc_Evexp_c_can_eval((
struct Cyc_Absyn_Exp*)_check_null(_tmp531))){const char*_tmpBBC;void*_tmpBBB;(
_tmpBBB=0,Cyc_Tcutil_terr(e->loc,((_tmpBBC="can't calculate numelts",
_tag_dyneither(_tmpBBC,sizeof(char),24))),_tag_dyneither(_tmpBBB,sizeof(void*),0)));}
e->r=_tmp531->r;goto _LL270;_LL273: {struct Cyc_Absyn_PointerType_struct*_tmp532=(
struct Cyc_Absyn_PointerType_struct*)_tmp52E;if(_tmp532->tag != 5)goto _LL275;else{
_tmp533=_tmp532->f1;_tmp534=_tmp533.elt_typ;_tmp535=_tmp533.ptr_atts;_tmp536=
_tmp535.nullable;_tmp537=_tmp535.bounds;_tmp538=_tmp535.zero_term;}}_LL274:{void*
_tmp53B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp537);struct Cyc_Absyn_Exp*_tmp53E;_LL278: {struct Cyc_Absyn_DynEither_b_struct*
_tmp53C=(struct Cyc_Absyn_DynEither_b_struct*)_tmp53B;if(_tmp53C->tag != 0)goto
_LL27A;}_LL279:{struct Cyc_Absyn_Exp*_tmpBBD[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,((
_tmpBBD[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp534),0),((_tmpBBD[0]=(
struct Cyc_Absyn_Exp*)_tmp4A3->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpBBD,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL277;_LL27A: {struct Cyc_Absyn_Upper_b_struct*_tmp53D=(struct Cyc_Absyn_Upper_b_struct*)
_tmp53B;if(_tmp53D->tag != 1)goto _LL277;else{_tmp53E=_tmp53D->f1;}}_LL27B: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp538)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp4A3->hd);struct Cyc_Absyn_Exp*_tmpBBE[2];e->r=Cyc_Toc_fncall_exp_r(
function_e,((_tmpBBE[1]=_tmp53E,((_tmpBBE[0]=(struct Cyc_Absyn_Exp*)_tmp4A3->hd,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpBBE,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp536)){if(!Cyc_Evexp_c_can_eval(_tmp53E)){const
char*_tmpBC1;void*_tmpBC0;(_tmpBC0=0,Cyc_Tcutil_terr(e->loc,((_tmpBC1="can't calculate numelts",
_tag_dyneither(_tmpBC1,sizeof(char),24))),_tag_dyneither(_tmpBC0,sizeof(void*),0)));}
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp53E,Cyc_Absyn_uint_exp(0,0));}else{e->r=
_tmp53E->r;goto _LL277;}}goto _LL277;_LL277:;}goto _LL270;_LL275:;_LL276: {const
char*_tmpBC6;void*_tmpBC5[2];struct Cyc_String_pa_struct _tmpBC4;struct Cyc_String_pa_struct
_tmpBC3;(_tmpBC3.tag=0,((_tmpBC3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((
_tmpBC4.tag=0,((_tmpBC4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmpBC5[0]=& _tmpBC4,((
_tmpBC5[1]=& _tmpBC3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBC6="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpBC6,
sizeof(char),41))),_tag_dyneither(_tmpBC5,sizeof(void*),2)))))))))))));}_LL270:;}
break;}case Cyc_Absyn_Plus: _LL26F:{void*_tmp547=Cyc_Tcutil_compress((void*)((
struct Cyc_List_List*)_check_null(_tmp52D))->hd);struct Cyc_Absyn_PtrInfo _tmp549;
void*_tmp54A;struct Cyc_Absyn_PtrAtts _tmp54B;union Cyc_Absyn_Constraint*_tmp54C;
union Cyc_Absyn_Constraint*_tmp54D;_LL27E: {struct Cyc_Absyn_PointerType_struct*
_tmp548=(struct Cyc_Absyn_PointerType_struct*)_tmp547;if(_tmp548->tag != 5)goto
_LL280;else{_tmp549=_tmp548->f1;_tmp54A=_tmp549.elt_typ;_tmp54B=_tmp549.ptr_atts;
_tmp54C=_tmp54B.bounds;_tmp54D=_tmp54B.zero_term;}}_LL27F:{void*_tmp54E=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp54C);struct Cyc_Absyn_Exp*_tmp551;_LL283: {struct Cyc_Absyn_DynEither_b_struct*
_tmp54F=(struct Cyc_Absyn_DynEither_b_struct*)_tmp54E;if(_tmp54F->tag != 0)goto
_LL285;}_LL284: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp4A3))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp4A3->tl))->hd;{struct Cyc_Absyn_Exp*_tmpBC7[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBC7[2]=e2,((_tmpBC7[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp54A),0),((_tmpBC7[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBC7,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}goto _LL282;}_LL285: {struct Cyc_Absyn_Upper_b_struct*_tmp550=(struct
Cyc_Absyn_Upper_b_struct*)_tmp54E;if(_tmp550->tag != 1)goto _LL282;else{_tmp551=
_tmp550->f1;}}_LL286: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp54D)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp4A3))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp4A3->tl))->hd;struct Cyc_Absyn_Exp*_tmpBC8[3];e->r=(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
e1),((_tmpBC8[2]=e2,((_tmpBC8[1]=_tmp551,((_tmpBC8[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBC8,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r;}goto _LL282;_LL282:;}goto _LL27D;_LL280:;_LL281: goto _LL27D;
_LL27D:;}break;case Cyc_Absyn_Minus: _LL27C: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp52D))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4A3))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4A3->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp52D->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{struct Cyc_Absyn_Exp*_tmpBC9[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBC9[2]=Cyc_Absyn_prim1_exp(
Cyc_Absyn_Minus,e2,0),((_tmpBC9[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpBC9[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBC9,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}break;}case Cyc_Absyn_Eq:
_LL287: goto _LL288;case Cyc_Absyn_Neq: _LL288: goto _LL289;case Cyc_Absyn_Gt: _LL289:
goto _LL28A;case Cyc_Absyn_Gte: _LL28A: goto _LL28B;case Cyc_Absyn_Lt: _LL28B: goto
_LL28C;case Cyc_Absyn_Lte: _LL28C: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4A3))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4A3->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp52D))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp52D->tl))->hd;void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& elt_typ))e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,
0));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp,0));break;}default: _LL28D: break;}goto _LL210;}_LL21B: {
struct Cyc_Absyn_Increment_e_struct*_tmp4A4=(struct Cyc_Absyn_Increment_e_struct*)
_tmp497;if(_tmp4A4->tag != 5)goto _LL21D;else{_tmp4A5=_tmp4A4->f1;_tmp4A6=_tmp4A4->f2;}}
_LL21C: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4A5->topt))->v;
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;const char*_tmpBCA;struct _dyneither_ptr incr_str=(_tmpBCA="increment",
_tag_dyneither(_tmpBCA,sizeof(char),10));if(_tmp4A6 == Cyc_Absyn_PreDec  || 
_tmp4A6 == Cyc_Absyn_PostDec){const char*_tmpBCB;incr_str=((_tmpBCB="decrement",
_tag_dyneither(_tmpBCB,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp4A5,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpBCF;void*_tmpBCE[1];
struct Cyc_String_pa_struct _tmpBCD;(_tmpBCD.tag=0,((_tmpBCD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBCE[0]=& _tmpBCD,Cyc_Tcutil_terr(
e->loc,((_tmpBCF="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpBCF,sizeof(char),74))),_tag_dyneither(_tmpBCE,sizeof(void*),1)))))));}{
const char*_tmpBD2;void*_tmpBD1;(_tmpBD1=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD2="in-place inc/dec on zero-term",
_tag_dyneither(_tmpBD2,sizeof(char),30))),_tag_dyneither(_tmpBD1,sizeof(void*),0)));};}{
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp4A5,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp55B=Cyc_Absyn_signed_int_exp(
1,0);{struct Cyc_Core_Opt*_tmpBD3;_tmp55B->topt=((_tmpBD3=_cycalloc(sizeof(*
_tmpBD3)),((_tmpBD3->v=Cyc_Absyn_sint_typ,_tmpBD3))));}switch(_tmp4A6){case Cyc_Absyn_PreInc:
_LL28F:{struct Cyc_Absyn_AssignOp_e_struct _tmpBD9;struct Cyc_Core_Opt*_tmpBD8;
struct Cyc_Absyn_AssignOp_e_struct*_tmpBD7;e->r=(void*)((_tmpBD7=_cycalloc(
sizeof(*_tmpBD7)),((_tmpBD7[0]=((_tmpBD9.tag=4,((_tmpBD9.f1=_tmp4A5,((_tmpBD9.f2=((
_tmpBD8=_cycalloc_atomic(sizeof(*_tmpBD8)),((_tmpBD8->v=(void*)Cyc_Absyn_Plus,
_tmpBD8)))),((_tmpBD9.f3=_tmp55B,_tmpBD9)))))))),_tmpBD7))));}Cyc_Toc_exp_to_c(
nv,e);return;case Cyc_Absyn_PreDec: _LL290:{struct Cyc_Absyn_AssignOp_e_struct
_tmpBDF;struct Cyc_Core_Opt*_tmpBDE;struct Cyc_Absyn_AssignOp_e_struct*_tmpBDD;e->r=(
void*)((_tmpBDD=_cycalloc(sizeof(*_tmpBDD)),((_tmpBDD[0]=((_tmpBDF.tag=4,((
_tmpBDF.f1=_tmp4A5,((_tmpBDF.f2=((_tmpBDE=_cycalloc_atomic(sizeof(*_tmpBDE)),((
_tmpBDE->v=(void*)Cyc_Absyn_Minus,_tmpBDE)))),((_tmpBDF.f3=_tmp55B,_tmpBDF)))))))),
_tmpBDD))));}Cyc_Toc_exp_to_c(nv,e);return;default: _LL291:{const char*_tmpBE3;
void*_tmpBE2[1];struct Cyc_String_pa_struct _tmpBE1;(_tmpBE1.tag=0,((_tmpBE1.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBE2[0]=& _tmpBE1,Cyc_Tcutil_terr(
e->loc,((_tmpBE3="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpBE3,sizeof(char),59))),_tag_dyneither(_tmpBE2,sizeof(void*),1)))))));}{
const char*_tmpBE6;void*_tmpBE5;(_tmpBE5=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE6="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpBE6,sizeof(char),34))),_tag_dyneither(_tmpBE5,sizeof(void*),0)));};}}
Cyc_Toc_exp_to_c(nv,_tmp4A5);{void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp4A6 == Cyc_Absyn_PostInc
 || _tmp4A6 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp4A6 == Cyc_Absyn_PreDec  || _tmp4A6 == Cyc_Absyn_PostDec)change=- 1;{struct
Cyc_Absyn_Exp*_tmpBE7[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBE7[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpBE7[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpBE7[0]=Cyc_Toc_push_address_exp(_tmp4A5),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBE7,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Toc_functionSet*_tmp569=_tmp4A6 == Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:&
Cyc_Toc__zero_arr_inplace_plus_post_functionSet;struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(
_tmp569,_tmp4A5);struct Cyc_Absyn_Exp*_tmpBE8[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((
_tmpBE8[1]=Cyc_Absyn_signed_int_exp(1,0),((_tmpBE8[0]=Cyc_Toc_push_address_exp(
_tmp4A5),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBE8,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(elt_typ == (
void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4A5)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(
_tmp4A5,0,Cyc_Toc_incr_lvalue,_tmp4A6);e->r=_tmp4A5->r;}}}goto _LL210;};};}_LL21D: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp4A7=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp497;if(_tmp4A7->tag != 4)goto _LL21F;else{_tmp4A8=_tmp4A7->f1;_tmp4A9=_tmp4A7->f2;
_tmp4AA=_tmp4A7->f3;}}_LL21E: {void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4A8->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4AA->topt))->v;int f_tag=0;void*tagged_member_struct_type=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Toc_is_tagged_union_project(_tmp4A8,& f_tag,&
tagged_member_struct_type,1)){Cyc_Toc_exp_to_c(nv,_tmp4A8);Cyc_Toc_exp_to_c(nv,
_tmp4AA);e->r=Cyc_Toc_tagged_union_assignop(_tmp4A8,e1_old_typ,_tmp4A9,_tmp4AA,
e2_old_typ,f_tag,tagged_member_struct_type);return;}{void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp4A8,& ptr_type,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,
_tmp4A8,_tmp4A9,_tmp4AA,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp4A8);Cyc_Toc_exp_to_c(nv,_tmp4A8);Cyc_Toc_exp_to_c(nv,_tmp4AA);{int done=0;
if(_tmp4A9 != 0){void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*change;switch((enum Cyc_Absyn_Primop)
_tmp4A9->v){case Cyc_Absyn_Plus: _LL293: change=_tmp4AA;break;case Cyc_Absyn_Minus:
_LL294: change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4AA,0);break;default:
_LL295: {const char*_tmpBEB;void*_tmpBEA;(_tmpBEA=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBEB="bad t ? pointer arithmetic",
_tag_dyneither(_tmpBEB,sizeof(char),27))),_tag_dyneither(_tmpBEA,sizeof(void*),0)));}}
done=1;{struct Cyc_Absyn_Exp*_tmp56E=Cyc_Toc__dyneither_ptr_inplace_plus_e;struct
Cyc_Absyn_Exp*_tmpBEC[3];e->r=Cyc_Toc_fncall_exp_r(_tmp56E,((_tmpBEC[2]=change,((
_tmpBEC[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBEC[0]=Cyc_Toc_push_address_exp(
_tmp4A8),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBEC,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ))switch((enum Cyc_Absyn_Primop)_tmp4A9->v){case Cyc_Absyn_Plus:
_LL297: done=1;{struct Cyc_Absyn_Exp*_tmpBED[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4A8),((_tmpBED[1]=_tmp4AA,((
_tmpBED[0]=_tmp4A8,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBED,sizeof(struct Cyc_Absyn_Exp*),2)))))));}break;default:
_LL298: {const char*_tmpBF0;void*_tmpBEF;(_tmpBEF=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF0="bad zero-terminated pointer arithmetic",
_tag_dyneither(_tmpBF0,sizeof(char),39))),_tag_dyneither(_tmpBEF,sizeof(void*),0)));}}}}
if(!done){if(e1_poly)_tmp4AA->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp4AA->r,0));if(!Cyc_Absyn_is_lvalue(_tmp4A8)){{struct _tuple18 _tmpBF3;struct
_tuple18*_tmpBF2;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple18*),struct _tuple18*f_env))
Cyc_Toc_lvalue_assign)(_tmp4A8,0,Cyc_Toc_assignop_lvalue,((_tmpBF2=_cycalloc(
sizeof(struct _tuple18)* 1),((_tmpBF2[0]=((_tmpBF3.f1=_tmp4A9,((_tmpBF3.f2=
_tmp4AA,_tmpBF3)))),_tmpBF2)))));}e->r=_tmp4A8->r;}}goto _LL210;};};};}_LL21F: {
struct Cyc_Absyn_Conditional_e_struct*_tmp4AB=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp497;if(_tmp4AB->tag != 6)goto _LL221;else{_tmp4AC=_tmp4AB->f1;_tmp4AD=_tmp4AB->f2;
_tmp4AE=_tmp4AB->f3;}}_LL220: Cyc_Toc_exp_to_c(nv,_tmp4AC);Cyc_Toc_exp_to_c(nv,
_tmp4AD);Cyc_Toc_exp_to_c(nv,_tmp4AE);goto _LL210;_LL221: {struct Cyc_Absyn_And_e_struct*
_tmp4AF=(struct Cyc_Absyn_And_e_struct*)_tmp497;if(_tmp4AF->tag != 7)goto _LL223;
else{_tmp4B0=_tmp4AF->f1;_tmp4B1=_tmp4AF->f2;}}_LL222: Cyc_Toc_exp_to_c(nv,
_tmp4B0);Cyc_Toc_exp_to_c(nv,_tmp4B1);goto _LL210;_LL223: {struct Cyc_Absyn_Or_e_struct*
_tmp4B2=(struct Cyc_Absyn_Or_e_struct*)_tmp497;if(_tmp4B2->tag != 8)goto _LL225;
else{_tmp4B3=_tmp4B2->f1;_tmp4B4=_tmp4B2->f2;}}_LL224: Cyc_Toc_exp_to_c(nv,
_tmp4B3);Cyc_Toc_exp_to_c(nv,_tmp4B4);goto _LL210;_LL225: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp4B5=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp497;if(_tmp4B5->tag != 9)goto _LL227;
else{_tmp4B6=_tmp4B5->f1;_tmp4B7=_tmp4B5->f2;}}_LL226: Cyc_Toc_exp_to_c(nv,
_tmp4B6);Cyc_Toc_exp_to_c(nv,_tmp4B7);goto _LL210;_LL227: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp4B8=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp497;if(_tmp4B8->tag != 10)goto
_LL229;else{_tmp4B9=_tmp4B8->f1;_tmp4BA=_tmp4B8->f2;}}_LL228: _tmp4BC=_tmp4B9;
_tmp4BD=_tmp4BA;goto _LL22A;_LL229: {struct Cyc_Absyn_FnCall_e_struct*_tmp4BB=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp497;if(_tmp4BB->tag != 11)goto _LL22B;else{
_tmp4BC=_tmp4BB->f1;_tmp4BD=_tmp4BB->f2;_tmp4BE=_tmp4BB->f3;if(_tmp4BE != 0)goto
_LL22B;}}_LL22A: Cyc_Toc_exp_to_c(nv,_tmp4BC);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp4BD);goto _LL210;_LL22B: {struct Cyc_Absyn_FnCall_e_struct*
_tmp4BF=(struct Cyc_Absyn_FnCall_e_struct*)_tmp497;if(_tmp4BF->tag != 11)goto
_LL22D;else{_tmp4C0=_tmp4BF->f1;_tmp4C1=_tmp4BF->f2;_tmp4C2=_tmp4BF->f3;if(
_tmp4C2 == 0)goto _LL22D;_tmp4C3=*_tmp4C2;_tmp4C4=_tmp4C3.num_varargs;_tmp4C5=
_tmp4C3.injectors;_tmp4C6=_tmp4C3.vai;}}_LL22C: {struct Cyc_Toc_Env _tmp576;struct
_RegionHandle*_tmp577;struct Cyc_Toc_Env*_tmp575=nv;_tmp576=*_tmp575;_tmp577=
_tmp576.rgn;{struct _tuple0*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=
Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp4C4,0);void*cva_type=Cyc_Toc_typ_to_c(_tmp4C6->type);void*
arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp4C1);int num_normargs=num_args - _tmp4C4;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp4C1=_tmp4C1->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4C1))->hd);{
struct Cyc_List_List*_tmpBF4;new_args=((_tmpBF4=_cycalloc(sizeof(*_tmpBF4)),((
_tmpBF4->hd=(struct Cyc_Absyn_Exp*)_tmp4C1->hd,((_tmpBF4->tl=new_args,_tmpBF4))))));};}}{
struct Cyc_Absyn_Exp*_tmpBF7[3];struct Cyc_List_List*_tmpBF6;new_args=((_tmpBF6=
_cycalloc(sizeof(*_tmpBF6)),((_tmpBF6->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((
_tmpBF7[2]=num_varargs_exp,((_tmpBF7[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0),((
_tmpBF7[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBF7,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpBF6->tl=
new_args,_tmpBF6))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp4C0);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4C0,new_args,0),0);if(_tmp4C6->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp57B=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp4C6->type));struct Cyc_Absyn_DatatypeInfo _tmp57D;union Cyc_Absyn_DatatypeInfoU
_tmp57E;struct Cyc_Absyn_Datatypedecl**_tmp57F;struct Cyc_Absyn_Datatypedecl*
_tmp580;_LL29B: {struct Cyc_Absyn_DatatypeType_struct*_tmp57C=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp57B;if(_tmp57C->tag != 3)goto _LL29D;else{_tmp57D=_tmp57C->f1;_tmp57E=_tmp57D.datatype_info;
if((_tmp57E.KnownDatatype).tag != 2)goto _LL29D;_tmp57F=(struct Cyc_Absyn_Datatypedecl**)(
_tmp57E.KnownDatatype).val;_tmp580=*_tmp57F;}}_LL29C: tud=_tmp580;goto _LL29A;
_LL29D:;_LL29E: {const char*_tmpBFA;void*_tmpBF9;(_tmpBF9=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBFA="toc: unknown datatype in vararg with inject",
_tag_dyneither(_tmpBFA,sizeof(char),44))),_tag_dyneither(_tmpBF9,sizeof(void*),0)));}
_LL29A:;}{unsigned int _tmpBFF;unsigned int _tmpBFE;struct _dyneither_ptr _tmpBFD;
struct _tuple0**_tmpBFC;unsigned int _tmpBFB;struct _dyneither_ptr vs=(_tmpBFB=(
unsigned int)_tmp4C4,((_tmpBFC=(struct _tuple0**)_region_malloc(_tmp577,
_check_times(sizeof(struct _tuple0*),_tmpBFB)),((_tmpBFD=_tag_dyneither(_tmpBFC,
sizeof(struct _tuple0*),_tmpBFB),((((_tmpBFE=_tmpBFB,_tmpC00(& _tmpBFF,& _tmpBFE,&
_tmpBFC))),_tmpBFD)))))));if(_tmp4C4 != 0){struct Cyc_List_List*_tmp583=0;{int i=
_tmp4C4 - 1;for(0;i >= 0;-- i){struct Cyc_List_List*_tmpC01;_tmp583=((_tmpC01=
_cycalloc(sizeof(*_tmpC01)),((_tmpC01->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct
_tuple0*),i)),0),0)),((_tmpC01->tl=_tmp583,_tmpC01))))));}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp583,0),s,0);{
int i=0;for(0;_tmp4C1 != 0;(((_tmp4C1=_tmp4C1->tl,_tmp4C5=_tmp4C5->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4C1->hd;void*arg_type=(void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct _tuple0*var=*((struct
_tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield _tmp586;struct
_tuple0*_tmp587;struct Cyc_List_List*_tmp588;struct Cyc_Absyn_Datatypefield*
_tmp585=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp4C5))->hd;_tmp586=*_tmp585;_tmp587=_tmp586.name;_tmp588=_tmp586.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)_check_null(
_tmp588))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp587),0),s,0);{const char*_tmpC02;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp587,((_tmpC02="_struct",_tag_dyneither(_tmpC02,
sizeof(char),8))))),0,s,0);};};}};}else{struct _tuple15*_tmpC03[3];struct Cyc_List_List*
_tmp58A=(_tmpC03[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC03[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpC03[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC03,sizeof(struct _tuple15*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{{int i=0;for(0;_tmp4C1
!= 0;(_tmp4C1=_tmp4C1->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp4C1->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4C1->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL210;}_LL22D: {struct Cyc_Absyn_Throw_e_struct*_tmp4C7=(struct Cyc_Absyn_Throw_e_struct*)
_tmp497;if(_tmp4C7->tag != 12)goto _LL22F;else{_tmp4C8=_tmp4C7->f1;}}_LL22E: Cyc_Toc_exp_to_c(
nv,_tmp4C8);e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(
_tmp4C8),0))->r;goto _LL210;_LL22F: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp4C9=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp497;if(_tmp4C9->tag != 13)
goto _LL231;else{_tmp4CA=_tmp4C9->f1;}}_LL230: Cyc_Toc_exp_to_c(nv,_tmp4CA);goto
_LL210;_LL231: {struct Cyc_Absyn_Instantiate_e_struct*_tmp4CB=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp497;if(_tmp4CB->tag != 14)goto _LL233;else{_tmp4CC=_tmp4CB->f1;_tmp4CD=_tmp4CB->f2;}}
_LL232: Cyc_Toc_exp_to_c(nv,_tmp4CC);for(0;_tmp4CD != 0;_tmp4CD=_tmp4CD->tl){enum 
Cyc_Absyn_KindQual _tmp590=(Cyc_Tcutil_typ_kind((void*)_tmp4CD->hd))->kind;if(
_tmp590 != Cyc_Absyn_EffKind  && _tmp590 != Cyc_Absyn_RgnKind){{void*_tmp591=Cyc_Tcutil_compress((
void*)_tmp4CD->hd);_LL2A0: {struct Cyc_Absyn_VarType_struct*_tmp592=(struct Cyc_Absyn_VarType_struct*)
_tmp591;if(_tmp592->tag != 2)goto _LL2A2;}_LL2A1: goto _LL2A3;_LL2A2: {struct Cyc_Absyn_DatatypeType_struct*
_tmp593=(struct Cyc_Absyn_DatatypeType_struct*)_tmp591;if(_tmp593->tag != 3)goto
_LL2A4;}_LL2A3: continue;_LL2A4:;_LL2A5: e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4CC,0))->r;goto _LL29F;
_LL29F:;}break;}}goto _LL210;_LL233: {struct Cyc_Absyn_Cast_e_struct*_tmp4CE=(
struct Cyc_Absyn_Cast_e_struct*)_tmp497;if(_tmp4CE->tag != 15)goto _LL235;else{
_tmp4CF=(void**)& _tmp4CE->f1;_tmp4D0=(void**)((void**)& _tmp4CE->f1);_tmp4D1=
_tmp4CE->f2;_tmp4D2=_tmp4CE->f3;_tmp4D3=_tmp4CE->f4;}}_LL234: {void*old_t2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4D1->topt))->v;void*new_typ=*_tmp4D0;*
_tmp4D0=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp4D1);{struct _tuple17
_tmpC04;struct _tuple17 _tmp595=(_tmpC04.f1=Cyc_Tcutil_compress(old_t2),((_tmpC04.f2=
Cyc_Tcutil_compress(new_typ),_tmpC04)));void*_tmp596;struct Cyc_Absyn_PtrInfo
_tmp598;void*_tmp599;struct Cyc_Absyn_PtrInfo _tmp59B;void*_tmp59C;struct Cyc_Absyn_PtrInfo
_tmp59E;void*_tmp59F;_LL2A7: _tmp596=_tmp595.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp597=(struct Cyc_Absyn_PointerType_struct*)_tmp596;if(_tmp597->tag != 5)goto
_LL2A9;else{_tmp598=_tmp597->f1;}};_tmp599=_tmp595.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp59A=(struct Cyc_Absyn_PointerType_struct*)_tmp599;if(_tmp59A->tag != 5)goto
_LL2A9;else{_tmp59B=_tmp59A->f1;}};_LL2A8: {int _tmp5A1=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp598.ptr_atts).nullable);int _tmp5A2=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp59B.ptr_atts).nullable);
void*_tmp5A3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp598.ptr_atts).bounds);void*_tmp5A4=((void*(*)(void*y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp59B.ptr_atts).bounds);
int _tmp5A5=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp598.ptr_atts).zero_term);int _tmp5A6=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp59B.ptr_atts).zero_term);{struct _tuple17 _tmpC05;
struct _tuple17 _tmp5A8=(_tmpC05.f1=_tmp5A3,((_tmpC05.f2=_tmp5A4,_tmpC05)));void*
_tmp5A9;struct Cyc_Absyn_Exp*_tmp5AB;void*_tmp5AC;struct Cyc_Absyn_Exp*_tmp5AE;
void*_tmp5AF;struct Cyc_Absyn_Exp*_tmp5B1;void*_tmp5B2;void*_tmp5B4;void*_tmp5B6;
struct Cyc_Absyn_Exp*_tmp5B8;void*_tmp5B9;void*_tmp5BB;_LL2AE: _tmp5A9=_tmp5A8.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp5AA=(struct Cyc_Absyn_Upper_b_struct*)_tmp5A9;
if(_tmp5AA->tag != 1)goto _LL2B0;else{_tmp5AB=_tmp5AA->f1;}};_tmp5AC=_tmp5A8.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp5AD=(struct Cyc_Absyn_Upper_b_struct*)_tmp5AC;
if(_tmp5AD->tag != 1)goto _LL2B0;else{_tmp5AE=_tmp5AD->f1;}};_LL2AF: if((!Cyc_Evexp_c_can_eval(
_tmp5AB) || !Cyc_Evexp_c_can_eval(_tmp5AE)) && !Cyc_Evexp_same_const_exp(_tmp5AB,
_tmp5AE)){const char*_tmpC08;void*_tmpC07;(_tmpC07=0,Cyc_Tcutil_terr(e->loc,((
_tmpC08="can't validate cast due to potential size differences",_tag_dyneither(
_tmpC08,sizeof(char),54))),_tag_dyneither(_tmpC07,sizeof(void*),0)));}if(_tmp5A1
 && !_tmp5A2){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC0B;void*_tmpC0A;(
_tmpC0A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC0B="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC0B,
sizeof(char),44))),_tag_dyneither(_tmpC0A,sizeof(void*),0)));}if(_tmp4D3 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC0F;void*_tmpC0E[1];struct Cyc_String_pa_struct _tmpC0D;(_tmpC0D.tag=
0,((_tmpC0D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpC0E[0]=& _tmpC0D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpC0F="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpC0F,sizeof(char),41))),_tag_dyneither(_tmpC0E,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp4D1);if(do_null_check){if(!_tmp4D2){
const char*_tmpC12;void*_tmpC11;(_tmpC11=0,Cyc_Tcutil_warn(e->loc,((_tmpC12="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpC12,sizeof(char),58))),_tag_dyneither(_tmpC11,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC13;e->r=Cyc_Toc_cast_it_r(*_tmp4D0,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC13=_cycalloc(sizeof(*_tmpC13)),((_tmpC13->hd=
_tmp4D1,((_tmpC13->tl=0,_tmpC13)))))),0));};}};}goto _LL2AD;_LL2B0: _tmp5AF=
_tmp5A8.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp5B0=(struct Cyc_Absyn_Upper_b_struct*)
_tmp5AF;if(_tmp5B0->tag != 1)goto _LL2B2;else{_tmp5B1=_tmp5B0->f1;}};_tmp5B2=
_tmp5A8.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5B3=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5B2;if(_tmp5B3->tag != 0)goto _LL2B2;};_LL2B1: if(!Cyc_Evexp_c_can_eval(_tmp5B1)){
const char*_tmpC16;void*_tmpC15;(_tmpC15=0,Cyc_Tcutil_terr(e->loc,((_tmpC16="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC16,sizeof(char),71))),_tag_dyneither(_tmpC15,sizeof(void*),0)));}
if(_tmp4D3 == Cyc_Absyn_NonNull_to_Null){const char*_tmpC1A;void*_tmpC19[1];struct
Cyc_String_pa_struct _tmpC18;(_tmpC18.tag=0,((_tmpC18.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC19[0]=& _tmpC18,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC1A="conversion mis-classified as null-check: %s",
_tag_dyneither(_tmpC1A,sizeof(char),44))),_tag_dyneither(_tmpC19,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){if((_tmp5A5  && !(_tmp59B.elt_tq).real_const) && !
_tmp5A6)_tmp5B1=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp5B1,Cyc_Absyn_uint_exp(1,
0),0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp5B1,_tmp4D1))->r;}else{
struct Cyc_Absyn_Exp*_tmp5CC=Cyc_Toc__tag_dyneither_e;if(_tmp5A5){struct _tuple0*
_tmp5CD=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp5CE=Cyc_Absyn_var_exp(_tmp5CD,
0);struct Cyc_Absyn_Exp*arg3;{void*_tmp5CF=_tmp4D1->r;union Cyc_Absyn_Cnst _tmp5D1;
struct _dyneither_ptr _tmp5D2;_LL2B7: {struct Cyc_Absyn_Const_e_struct*_tmp5D0=(
struct Cyc_Absyn_Const_e_struct*)_tmp5CF;if(_tmp5D0->tag != 0)goto _LL2B9;else{
_tmp5D1=_tmp5D0->f1;if((_tmp5D1.String_c).tag != 7)goto _LL2B9;_tmp5D2=(struct
_dyneither_ptr)(_tmp5D1.String_c).val;}}_LL2B8: arg3=_tmp5B1;goto _LL2B6;_LL2B9:;
_LL2BA:{struct Cyc_Absyn_Exp*_tmpC1B[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp4D1),((_tmpC1B[1]=_tmp5B1,((_tmpC1B[0]=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp5CE),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1B,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);}goto _LL2B6;_LL2B6:;}if(!_tmp5A6  && !(_tmp59B.elt_tq).real_const)arg3=
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*
_tmp5D4=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp59B.elt_typ),0);struct Cyc_Absyn_Exp*
_tmpC1C[3];struct Cyc_Absyn_Exp*_tmp5D5=Cyc_Absyn_fncall_exp(_tmp5CC,((_tmpC1C[2]=
arg3,((_tmpC1C[1]=_tmp5D4,((_tmpC1C[0]=_tmp5CE,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1C,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);struct Cyc_Absyn_Stmt*_tmp5D6=Cyc_Absyn_exp_stmt(_tmp5D5,0);_tmp5D6=
Cyc_Absyn_declare_stmt(_tmp5CD,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)
_tmp4D1,_tmp5D6,0);e->r=Cyc_Toc_stmt_exp_r(_tmp5D6);};}else{struct Cyc_Absyn_Exp*
_tmpC1D[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5CC,((_tmpC1D[2]=_tmp5B1,((_tmpC1D[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp59B.elt_typ),0),((_tmpC1D[0]=
_tmp4D1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC1D,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2AD;
_LL2B2: _tmp5B4=_tmp5A8.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp5B5=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5B4;if(_tmp5B5->tag != 0)goto _LL2B4;};_tmp5B6=_tmp5A8.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp5B7=(struct Cyc_Absyn_Upper_b_struct*)_tmp5B6;if(_tmp5B7->tag != 1)goto _LL2B4;
else{_tmp5B8=_tmp5B7->f1;}};_LL2B3: if(!Cyc_Evexp_c_can_eval(_tmp5B8)){const char*
_tmpC20;void*_tmpC1F;(_tmpC1F=0,Cyc_Tcutil_terr(e->loc,((_tmpC20="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC20,sizeof(char),71))),_tag_dyneither(_tmpC1F,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC23;void*_tmpC22;(_tmpC22=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC23="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpC23,sizeof(char),45))),_tag_dyneither(_tmpC22,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5DD=_tmp5B8;if(_tmp5A5  && !_tmp5A6)_tmp5DD=Cyc_Absyn_add_exp(
_tmp5B8,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp5DE=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC24[3];struct Cyc_Absyn_Exp*_tmp5DF=Cyc_Absyn_fncall_exp(
_tmp5DE,((_tmpC24[2]=_tmp5DD,((_tmpC24[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp598.elt_typ),0),((_tmpC24[0]=_tmp4D1,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC24,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp5A2){struct Cyc_List_List*_tmpC25;_tmp5DF->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_null_e,((_tmpC25=_cycalloc(sizeof(*_tmpC25)),((_tmpC25->hd=Cyc_Absyn_copy_exp(
_tmp5DF),((_tmpC25->tl=0,_tmpC25)))))));}e->r=Cyc_Toc_cast_it_r(*_tmp4D0,_tmp5DF);
goto _LL2AD;};};_LL2B4: _tmp5B9=_tmp5A8.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp5BA=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5B9;if(_tmp5BA->tag != 0)goto
_LL2AD;};_tmp5BB=_tmp5A8.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5BC=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp5BB;if(_tmp5BC->tag != 0)goto _LL2AD;};_LL2B5:
DynCast: if((_tmp5A5  && !_tmp5A6) && !(_tmp59B.elt_tq).real_const){if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpC28;void*_tmpC27;(_tmpC27=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC28="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpC28,sizeof(char),70))),_tag_dyneither(_tmpC27,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5E4=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpC29[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5E4,((_tmpC29[2]=Cyc_Absyn_uint_exp(1,0),((
_tmpC29[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp598.elt_typ),0),((
_tmpC29[0]=_tmp4D1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC29,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}goto _LL2AD;
_LL2AD:;}goto _LL2A6;}_LL2A9: _tmp59C=_tmp595.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp59D=(struct Cyc_Absyn_PointerType_struct*)_tmp59C;if(_tmp59D->tag != 5)goto
_LL2AB;else{_tmp59E=_tmp59D->f1;}};_tmp59F=_tmp595.f2;{struct Cyc_Absyn_IntType_struct*
_tmp5A0=(struct Cyc_Absyn_IntType_struct*)_tmp59F;if(_tmp5A0->tag != 6)goto _LL2AB;};
_LL2AA:{void*_tmp5E6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp59E.ptr_atts).bounds);_LL2BC: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5E7=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5E6;if(_tmp5E7->tag != 0)goto
_LL2BE;}_LL2BD: _tmp4D1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp4D1->r,
_tmp4D1->loc),Cyc_Toc_curr_sp);goto _LL2BB;_LL2BE:;_LL2BF: goto _LL2BB;_LL2BB:;}
goto _LL2A6;_LL2AB:;_LL2AC: goto _LL2A6;_LL2A6:;}goto _LL210;}_LL235: {struct Cyc_Absyn_Address_e_struct*
_tmp4D4=(struct Cyc_Absyn_Address_e_struct*)_tmp497;if(_tmp4D4->tag != 16)goto
_LL237;else{_tmp4D5=_tmp4D4->f1;}}_LL236:{void*_tmp5E8=_tmp4D5->r;struct _tuple0*
_tmp5EA;struct Cyc_List_List*_tmp5EB;struct Cyc_List_List*_tmp5EC;struct Cyc_List_List*
_tmp5EE;_LL2C1: {struct Cyc_Absyn_Aggregate_e_struct*_tmp5E9=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp5E8;if(_tmp5E9->tag != 29)goto _LL2C3;else{_tmp5EA=_tmp5E9->f1;_tmp5EB=_tmp5E9->f2;
_tmp5EC=_tmp5E9->f3;}}_LL2C2: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC2D;void*
_tmpC2C[1];struct Cyc_String_pa_struct _tmpC2B;(_tmpC2B.tag=0,((_tmpC2B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4D5->loc)),((
_tmpC2C[0]=& _tmpC2B,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC2D="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC2D,
sizeof(char),42))),_tag_dyneither(_tmpC2C,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4D5->topt))->v,_tmp5EB != 0,1,0,
_tmp5EC,_tmp5EA))->r;goto _LL2C0;_LL2C3: {struct Cyc_Absyn_Tuple_e_struct*_tmp5ED=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp5E8;if(_tmp5ED->tag != 25)goto _LL2C5;else{
_tmp5EE=_tmp5ED->f1;}}_LL2C4: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC31;void*
_tmpC30[1];struct Cyc_String_pa_struct _tmpC2F;(_tmpC2F.tag=0,((_tmpC2F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4D5->loc)),((
_tmpC30[0]=& _tmpC2F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC31="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC31,
sizeof(char),42))),_tag_dyneither(_tmpC30,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_tuple(
nv,1,0,_tmp5EE))->r;goto _LL2C0;_LL2C5:;_LL2C6: Cyc_Toc_exp_to_c(nv,_tmp4D5);if(!
Cyc_Absyn_is_lvalue(_tmp4D5)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp4D5,0,Cyc_Toc_address_lvalue,1);e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4D5);}goto _LL2C0;
_LL2C0:;}goto _LL210;_LL237: {struct Cyc_Absyn_New_e_struct*_tmp4D6=(struct Cyc_Absyn_New_e_struct*)
_tmp497;if(_tmp4D6->tag != 17)goto _LL239;else{_tmp4D7=_tmp4D6->f1;_tmp4D8=_tmp4D6->f2;}}
_LL238: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC35;void*_tmpC34[1];struct Cyc_String_pa_struct
_tmpC33;(_tmpC33.tag=0,((_tmpC33.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4D8->loc)),((_tmpC34[0]=& _tmpC33,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC35="%s: new at top-level",
_tag_dyneither(_tmpC35,sizeof(char),21))),_tag_dyneither(_tmpC34,sizeof(void*),1)))))));}{
void*_tmp5F8=_tmp4D8->r;struct Cyc_List_List*_tmp5FA;struct Cyc_Absyn_Vardecl*
_tmp5FC;struct Cyc_Absyn_Exp*_tmp5FD;struct Cyc_Absyn_Exp*_tmp5FE;int _tmp5FF;
struct _tuple0*_tmp601;struct Cyc_List_List*_tmp602;struct Cyc_List_List*_tmp603;
struct Cyc_Absyn_Aggrdecl*_tmp604;struct Cyc_List_List*_tmp606;_LL2C8: {struct Cyc_Absyn_Array_e_struct*
_tmp5F9=(struct Cyc_Absyn_Array_e_struct*)_tmp5F8;if(_tmp5F9->tag != 27)goto _LL2CA;
else{_tmp5FA=_tmp5F9->f1;}}_LL2C9: {struct _tuple0*_tmp607=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp608=Cyc_Absyn_var_exp(_tmp607,0);struct Cyc_Absyn_Stmt*
_tmp609=Cyc_Toc_init_array(nv,_tmp608,_tmp5FA,Cyc_Absyn_exp_stmt(_tmp608,0));
void*old_elt_typ;{void*_tmp60A=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp60C;void*_tmp60D;struct Cyc_Absyn_Tqual _tmp60E;struct Cyc_Absyn_PtrAtts _tmp60F;
union Cyc_Absyn_Constraint*_tmp610;_LL2D3: {struct Cyc_Absyn_PointerType_struct*
_tmp60B=(struct Cyc_Absyn_PointerType_struct*)_tmp60A;if(_tmp60B->tag != 5)goto
_LL2D5;else{_tmp60C=_tmp60B->f1;_tmp60D=_tmp60C.elt_typ;_tmp60E=_tmp60C.elt_tq;
_tmp60F=_tmp60C.ptr_atts;_tmp610=_tmp60F.zero_term;}}_LL2D4: old_elt_typ=_tmp60D;
goto _LL2D2;_LL2D5:;_LL2D6: {const char*_tmpC38;void*_tmpC37;old_elt_typ=((_tmpC37=
0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC38="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(
_tmpC38,sizeof(char),52))),_tag_dyneither(_tmpC37,sizeof(void*),0))));}_LL2D2:;}{
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp613=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp614=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp5FA),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp4D7 == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp614);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp4D7;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp614);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp607,_tmp613,(struct Cyc_Absyn_Exp*)e1,_tmp609,0));goto
_LL2C7;};}_LL2CA: {struct Cyc_Absyn_Comprehension_e_struct*_tmp5FB=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp5F8;if(_tmp5FB->tag != 28)goto _LL2CC;else{_tmp5FC=_tmp5FB->f1;_tmp5FD=_tmp5FB->f2;
_tmp5FE=_tmp5FB->f3;_tmp5FF=_tmp5FB->f4;}}_LL2CB: {int is_dyneither_ptr=0;{void*
_tmp615=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp617;void*_tmp618;
struct Cyc_Absyn_Tqual _tmp619;struct Cyc_Absyn_PtrAtts _tmp61A;union Cyc_Absyn_Constraint*
_tmp61B;union Cyc_Absyn_Constraint*_tmp61C;_LL2D8: {struct Cyc_Absyn_PointerType_struct*
_tmp616=(struct Cyc_Absyn_PointerType_struct*)_tmp615;if(_tmp616->tag != 5)goto
_LL2DA;else{_tmp617=_tmp616->f1;_tmp618=_tmp617.elt_typ;_tmp619=_tmp617.elt_tq;
_tmp61A=_tmp617.ptr_atts;_tmp61B=_tmp61A.bounds;_tmp61C=_tmp61A.zero_term;}}
_LL2D9: is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp61B)== (void*)& Cyc_Absyn_DynEither_b_val;goto _LL2D7;
_LL2DA:;_LL2DB: {const char*_tmpC3B;void*_tmpC3A;(_tmpC3A=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC3B="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpC3B,sizeof(char),42))),_tag_dyneither(_tmpC3A,sizeof(void*),0)));}
_LL2D7:;}{struct _tuple0*max=Cyc_Toc_temp_var();struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5FE->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp5FD);{struct Cyc_Absyn_Exp*_tmp61F=
Cyc_Absyn_var_exp(max,0);if(_tmp5FF)_tmp61F=Cyc_Absyn_add_exp(_tmp61F,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp5FC,Cyc_Absyn_var_exp(max,0),_tmp5FE,_tmp5FF,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp621;struct _RegionHandle*_tmp622;struct Cyc_Toc_Env*_tmp620=
nv;_tmp621=*_tmp620;_tmp622=_tmp621.rgn;{struct _tuple19*_tmpC3E;struct Cyc_List_List*
_tmpC3D;struct Cyc_List_List*decls=(_tmpC3D=_region_malloc(_tmp622,sizeof(*
_tmpC3D)),((_tmpC3D->hd=((_tmpC3E=_region_malloc(_tmp622,sizeof(*_tmpC3E)),((
_tmpC3E->f1=max,((_tmpC3E->f2=Cyc_Absyn_uint_typ,((_tmpC3E->f3=(struct Cyc_Absyn_Exp*)
_tmp5FD,_tmpC3E)))))))),((_tmpC3D->tl=0,_tmpC3D)))));struct Cyc_Absyn_Exp*ai;if(
_tmp4D7 == 0  || Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*_tmpC3F[2];ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((_tmpC3F[1]=_tmp61F,((
_tmpC3F[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC3F,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4D7;Cyc_Toc_exp_to_c(
nv,r);{struct Cyc_Absyn_Exp*_tmpC40[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpC40[1]=_tmp61F,((_tmpC40[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC40,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct _tuple19*_tmpC43;struct Cyc_List_List*
_tmpC42;decls=((_tmpC42=_region_malloc(_tmp622,sizeof(*_tmpC42)),((_tmpC42->hd=((
_tmpC43=_region_malloc(_tmp622,sizeof(*_tmpC43)),((_tmpC43->f1=a,((_tmpC43->f2=
ptr_typ,((_tmpC43->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpC43)))))))),((_tmpC42->tl=
decls,_tmpC42))))));}if(is_dyneither_ptr){struct _tuple0*_tmp627=Cyc_Toc_temp_var();
void*_tmp628=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp629=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC44[3];struct Cyc_Absyn_Exp*_tmp62A=Cyc_Absyn_fncall_exp(
_tmp629,((_tmpC44[2]=_tmp61F,((_tmpC44[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((
_tmpC44[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC44,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{
struct _tuple19*_tmpC47;struct Cyc_List_List*_tmpC46;decls=((_tmpC46=
_region_malloc(_tmp622,sizeof(*_tmpC46)),((_tmpC46->hd=((_tmpC47=_region_malloc(
_tmp622,sizeof(*_tmpC47)),((_tmpC47->f1=_tmp627,((_tmpC47->f2=_tmp628,((_tmpC47->f3=(
struct Cyc_Absyn_Exp*)_tmp62A,_tmpC47)))))))),((_tmpC46->tl=decls,_tmpC46))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp627,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp62E=decls;for(0;_tmp62E != 0;_tmp62E=_tmp62E->tl){struct
_tuple0*_tmp630;void*_tmp631;struct Cyc_Absyn_Exp*_tmp632;struct _tuple19 _tmp62F=*((
struct _tuple19*)_tmp62E->hd);_tmp630=_tmp62F.f1;_tmp631=_tmp62F.f2;_tmp632=
_tmp62F.f3;s=Cyc_Absyn_declare_stmt(_tmp630,_tmp631,_tmp632,s,0);}}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL2C7;};};};}_LL2CC: {struct Cyc_Absyn_Aggregate_e_struct*_tmp600=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp5F8;if(_tmp600->tag != 29)goto _LL2CE;
else{_tmp601=_tmp600->f1;_tmp602=_tmp600->f2;_tmp603=_tmp600->f3;_tmp604=_tmp600->f4;}}
_LL2CD: e->r=(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4D8->topt))->v,_tmp602 != 0,1,_tmp4D7,_tmp603,_tmp601))->r;goto _LL2C7;_LL2CE: {
struct Cyc_Absyn_Tuple_e_struct*_tmp605=(struct Cyc_Absyn_Tuple_e_struct*)_tmp5F8;
if(_tmp605->tag != 25)goto _LL2D0;else{_tmp606=_tmp605->f1;}}_LL2CF: e->r=(Cyc_Toc_init_tuple(
nv,1,_tmp4D7,_tmp606))->r;goto _LL2C7;_LL2D0:;_LL2D1: {void*old_elt_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4D8->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);struct _tuple0*_tmp635=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp636=Cyc_Absyn_var_exp(_tmp635,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(_tmp636,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp4D7
== 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4D7;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp637=_tmp4D8->r;void*_tmp639;
struct Cyc_Absyn_Exp*_tmp63A;_LL2DD: {struct Cyc_Absyn_Cast_e_struct*_tmp638=(
struct Cyc_Absyn_Cast_e_struct*)_tmp637;if(_tmp638->tag != 15)goto _LL2DF;else{
_tmp639=(void*)_tmp638->f1;_tmp63A=_tmp638->f2;}}_LL2DE:{struct _tuple17 _tmpC48;
struct _tuple17 _tmp63C=(_tmpC48.f1=Cyc_Tcutil_compress(_tmp639),((_tmpC48.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp63A->topt))->v),_tmpC48)));void*
_tmp63D;struct Cyc_Absyn_PtrInfo _tmp63F;void*_tmp640;struct Cyc_Absyn_PtrAtts
_tmp641;union Cyc_Absyn_Constraint*_tmp642;void*_tmp643;struct Cyc_Absyn_PtrInfo
_tmp645;struct Cyc_Absyn_PtrAtts _tmp646;union Cyc_Absyn_Constraint*_tmp647;_LL2E2:
_tmp63D=_tmp63C.f1;{struct Cyc_Absyn_PointerType_struct*_tmp63E=(struct Cyc_Absyn_PointerType_struct*)
_tmp63D;if(_tmp63E->tag != 5)goto _LL2E4;else{_tmp63F=_tmp63E->f1;_tmp640=_tmp63F.elt_typ;
_tmp641=_tmp63F.ptr_atts;_tmp642=_tmp641.bounds;}};_tmp643=_tmp63C.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp644=(struct Cyc_Absyn_PointerType_struct*)_tmp643;if(_tmp644->tag != 5)goto
_LL2E4;else{_tmp645=_tmp644->f1;_tmp646=_tmp645.ptr_atts;_tmp647=_tmp646.bounds;}};
_LL2E3:{struct _tuple17 _tmpC49;struct _tuple17 _tmp649=(_tmpC49.f1=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp642),((_tmpC49.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp647),_tmpC49)));void*_tmp64A;void*_tmp64C;struct Cyc_Absyn_Exp*
_tmp64E;_LL2E7: _tmp64A=_tmp649.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp64B=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp64A;if(_tmp64B->tag != 0)goto _LL2E9;};
_tmp64C=_tmp649.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp64D=(struct Cyc_Absyn_Upper_b_struct*)
_tmp64C;if(_tmp64D->tag != 1)goto _LL2E9;else{_tmp64E=_tmp64D->f1;}};_LL2E8: Cyc_Toc_exp_to_c(
nv,_tmp63A);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp64F=Cyc_Toc__init_dyneither_ptr_e;{struct Cyc_Absyn_Exp*_tmpC4A[4];e->r=Cyc_Toc_fncall_exp_r(
_tmp64F,((_tmpC4A[3]=_tmp64E,((_tmpC4A[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp640),0),((_tmpC4A[1]=_tmp63A,((_tmpC4A[0]=mexp,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC4A,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))));}goto _LL2E6;};_LL2E9:;_LL2EA: goto _LL2E6;_LL2E6:;}goto _LL2E1;_LL2E4:;
_LL2E5: goto _LL2E1;_LL2E1:;}goto _LL2DC;_LL2DF:;_LL2E0: goto _LL2DC;_LL2DC:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp651=Cyc_Absyn_exp_stmt(_tmp636,0);struct Cyc_Absyn_Exp*
_tmp652=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp4D8);_tmp651=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp636,_tmp652,0),_tmp4D8,0),
_tmp651,0);{void*_tmp653=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp635,_tmp653,(struct Cyc_Absyn_Exp*)mexp,_tmp651,0));};}
goto _LL2C7;};}_LL2C7:;}goto _LL210;_LL239: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp4D9=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp497;if(_tmp4D9->tag != 19)goto
_LL23B;else{_tmp4DA=_tmp4D9->f1;}}_LL23A: Cyc_Toc_exp_to_c(nv,_tmp4DA);goto _LL210;
_LL23B: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4DB=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp497;if(_tmp4DB->tag != 18)goto _LL23D;else{_tmp4DC=(void*)_tmp4DB->f1;}}_LL23C:{
struct Cyc_Absyn_Sizeoftyp_e_struct _tmpC4D;struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpC4C;e->r=(void*)((_tmpC4C=_cycalloc(sizeof(*_tmpC4C)),((_tmpC4C[0]=((_tmpC4D.tag=
18,((_tmpC4D.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4DC),_tmpC4D)))),_tmpC4C))));}
goto _LL210;_LL23D: {struct Cyc_Absyn_Offsetof_e_struct*_tmp4DD=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp497;if(_tmp4DD->tag != 20)goto _LL23F;else{_tmp4DE=(void*)_tmp4DD->f1;_tmp4DF=(
void*)_tmp4DD->f2;{struct Cyc_Absyn_StructField_struct*_tmp4E0=(struct Cyc_Absyn_StructField_struct*)
_tmp4DF;if(_tmp4E0->tag != 0)goto _LL23F;else{_tmp4E1=_tmp4E0->f1;}};}}_LL23E:{
struct Cyc_Absyn_Offsetof_e_struct _tmpC57;struct Cyc_Absyn_StructField_struct
_tmpC56;struct Cyc_Absyn_StructField_struct*_tmpC55;struct Cyc_Absyn_Offsetof_e_struct*
_tmpC54;e->r=(void*)((_tmpC54=_cycalloc(sizeof(*_tmpC54)),((_tmpC54[0]=((_tmpC57.tag=
20,((_tmpC57.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4DE),((_tmpC57.f2=(void*)((
void*)((_tmpC55=_cycalloc(sizeof(*_tmpC55)),((_tmpC55[0]=((_tmpC56.tag=0,((
_tmpC56.f1=_tmp4E1,_tmpC56)))),_tmpC55))))),_tmpC57)))))),_tmpC54))));}goto
_LL210;_LL23F: {struct Cyc_Absyn_Offsetof_e_struct*_tmp4E2=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp497;if(_tmp4E2->tag != 20)goto _LL241;else{_tmp4E3=(void*)_tmp4E2->f1;_tmp4E4=(
void*)_tmp4E2->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp4E5=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp4E4;if(_tmp4E5->tag != 1)goto _LL241;else{_tmp4E6=_tmp4E5->f1;}};}}_LL240:{
void*_tmp65A=Cyc_Tcutil_compress(_tmp4E3);struct Cyc_Absyn_AggrInfo _tmp65C;union
Cyc_Absyn_AggrInfoU _tmp65D;struct Cyc_List_List*_tmp65F;_LL2EC: {struct Cyc_Absyn_AggrType_struct*
_tmp65B=(struct Cyc_Absyn_AggrType_struct*)_tmp65A;if(_tmp65B->tag != 12)goto
_LL2EE;else{_tmp65C=_tmp65B->f1;_tmp65D=_tmp65C.aggr_info;}}_LL2ED: {struct Cyc_Absyn_Aggrdecl*
_tmp662=Cyc_Absyn_get_known_aggrdecl(_tmp65D);if(_tmp662->impl == 0){const char*
_tmpC5A;void*_tmpC59;(_tmpC59=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC5A="struct fields must be known",
_tag_dyneither(_tmpC5A,sizeof(char),28))),_tag_dyneither(_tmpC59,sizeof(void*),0)));}
_tmp65F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp662->impl))->fields;goto
_LL2EF;}_LL2EE: {struct Cyc_Absyn_AnonAggrType_struct*_tmp65E=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp65A;if(_tmp65E->tag != 13)goto _LL2F0;else{_tmp65F=_tmp65E->f2;}}_LL2EF: {
struct Cyc_Absyn_Aggrfield*_tmp665=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp65F,(int)_tmp4E6);{struct Cyc_Absyn_Offsetof_e_struct
_tmpC64;struct Cyc_Absyn_StructField_struct _tmpC63;struct Cyc_Absyn_StructField_struct*
_tmpC62;struct Cyc_Absyn_Offsetof_e_struct*_tmpC61;e->r=(void*)((_tmpC61=
_cycalloc(sizeof(*_tmpC61)),((_tmpC61[0]=((_tmpC64.tag=20,((_tmpC64.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4E3),((_tmpC64.f2=(void*)((void*)((_tmpC62=_cycalloc(
sizeof(*_tmpC62)),((_tmpC62[0]=((_tmpC63.tag=0,((_tmpC63.f1=_tmp665->name,
_tmpC63)))),_tmpC62))))),_tmpC64)))))),_tmpC61))));}goto _LL2EB;}_LL2F0: {struct
Cyc_Absyn_TupleType_struct*_tmp660=(struct Cyc_Absyn_TupleType_struct*)_tmp65A;
if(_tmp660->tag != 11)goto _LL2F2;}_LL2F1:{struct Cyc_Absyn_Offsetof_e_struct
_tmpC6E;struct Cyc_Absyn_StructField_struct _tmpC6D;struct Cyc_Absyn_StructField_struct*
_tmpC6C;struct Cyc_Absyn_Offsetof_e_struct*_tmpC6B;e->r=(void*)((_tmpC6B=
_cycalloc(sizeof(*_tmpC6B)),((_tmpC6B[0]=((_tmpC6E.tag=20,((_tmpC6E.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4E3),((_tmpC6E.f2=(void*)((void*)((_tmpC6C=_cycalloc(
sizeof(*_tmpC6C)),((_tmpC6C[0]=((_tmpC6D.tag=0,((_tmpC6D.f1=Cyc_Absyn_fieldname((
int)(_tmp4E6 + 1)),_tmpC6D)))),_tmpC6C))))),_tmpC6E)))))),_tmpC6B))));}goto _LL2EB;
_LL2F2: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp661=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp65A;if(_tmp661->tag != 4)goto _LL2F4;}_LL2F3: if(_tmp4E6 == 0){struct Cyc_Absyn_Offsetof_e_struct
_tmpC78;struct Cyc_Absyn_StructField_struct _tmpC77;struct Cyc_Absyn_StructField_struct*
_tmpC76;struct Cyc_Absyn_Offsetof_e_struct*_tmpC75;e->r=(void*)((_tmpC75=
_cycalloc(sizeof(*_tmpC75)),((_tmpC75[0]=((_tmpC78.tag=20,((_tmpC78.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4E3),((_tmpC78.f2=(void*)((void*)((_tmpC76=_cycalloc(
sizeof(*_tmpC76)),((_tmpC76[0]=((_tmpC77.tag=0,((_tmpC77.f1=Cyc_Toc_tag_sp,
_tmpC77)))),_tmpC76))))),_tmpC78)))))),_tmpC75))));}else{struct Cyc_Absyn_Offsetof_e_struct
_tmpC82;struct Cyc_Absyn_StructField_struct _tmpC81;struct Cyc_Absyn_StructField_struct*
_tmpC80;struct Cyc_Absyn_Offsetof_e_struct*_tmpC7F;e->r=(void*)((_tmpC7F=
_cycalloc(sizeof(*_tmpC7F)),((_tmpC7F[0]=((_tmpC82.tag=20,((_tmpC82.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4E3),((_tmpC82.f2=(void*)((void*)((_tmpC80=_cycalloc(
sizeof(*_tmpC80)),((_tmpC80[0]=((_tmpC81.tag=0,((_tmpC81.f1=Cyc_Absyn_fieldname((
int)_tmp4E6),_tmpC81)))),_tmpC80))))),_tmpC82)))))),_tmpC7F))));}goto _LL2EB;
_LL2F4:;_LL2F5: {const char*_tmpC85;void*_tmpC84;(_tmpC84=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC85="impossible type for offsetof tuple index",
_tag_dyneither(_tmpC85,sizeof(char),41))),_tag_dyneither(_tmpC84,sizeof(void*),0)));}
_LL2EB:;}goto _LL210;_LL241: {struct Cyc_Absyn_Deref_e_struct*_tmp4E7=(struct Cyc_Absyn_Deref_e_struct*)
_tmp497;if(_tmp4E7->tag != 21)goto _LL243;else{_tmp4E8=_tmp4E7->f1;}}_LL242: {void*
_tmp678=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp4E8->topt))->v);{
void*_tmp679=_tmp678;struct Cyc_Absyn_PtrInfo _tmp67B;void*_tmp67C;struct Cyc_Absyn_Tqual
_tmp67D;struct Cyc_Absyn_PtrAtts _tmp67E;void*_tmp67F;union Cyc_Absyn_Constraint*
_tmp680;union Cyc_Absyn_Constraint*_tmp681;union Cyc_Absyn_Constraint*_tmp682;
_LL2F7: {struct Cyc_Absyn_PointerType_struct*_tmp67A=(struct Cyc_Absyn_PointerType_struct*)
_tmp679;if(_tmp67A->tag != 5)goto _LL2F9;else{_tmp67B=_tmp67A->f1;_tmp67C=_tmp67B.elt_typ;
_tmp67D=_tmp67B.elt_tq;_tmp67E=_tmp67B.ptr_atts;_tmp67F=_tmp67E.rgn;_tmp680=
_tmp67E.nullable;_tmp681=_tmp67E.bounds;_tmp682=_tmp67E.zero_term;}}_LL2F8:{void*
_tmp683=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp681);struct Cyc_Absyn_Exp*_tmp685;_LL2FC: {struct Cyc_Absyn_Upper_b_struct*
_tmp684=(struct Cyc_Absyn_Upper_b_struct*)_tmp683;if(_tmp684->tag != 1)goto _LL2FE;
else{_tmp685=_tmp684->f1;}}_LL2FD: {int do_null_check=Cyc_Toc_need_null_check(
_tmp4E8);Cyc_Toc_exp_to_c(nv,_tmp4E8);if(do_null_check){if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC88;void*_tmpC87;(_tmpC87=0,Cyc_Tcutil_warn(e->loc,((_tmpC88="inserted null check due to dereference",
_tag_dyneither(_tmpC88,sizeof(char),39))),_tag_dyneither(_tmpC87,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC89;_tmp4E8->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp678),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpC89=_cycalloc(sizeof(*
_tmpC89)),((_tmpC89->hd=Cyc_Absyn_copy_exp(_tmp4E8),((_tmpC89->tl=0,_tmpC89)))))),
0));};}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp682)){unsigned int _tmp68B;int _tmp68C;struct _tuple10 _tmp68A=Cyc_Evexp_eval_const_uint_exp(
_tmp685);_tmp68B=_tmp68A.f1;_tmp68C=_tmp68A.f2;if(!_tmp68C  || _tmp68B <= 0){const
char*_tmpC8C;void*_tmpC8B;(_tmpC8B=0,Cyc_Tcutil_terr(e->loc,((_tmpC8C="cannot determine dereference is in bounds",
_tag_dyneither(_tmpC8C,sizeof(char),42))),_tag_dyneither(_tmpC8B,sizeof(void*),0)));}}
goto _LL2FB;}_LL2FE: {struct Cyc_Absyn_DynEither_b_struct*_tmp686=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp683;if(_tmp686->tag != 0)goto _LL2FB;}_LL2FF: {struct Cyc_Absyn_Exp*_tmp68F=Cyc_Absyn_uint_exp(
0,0);{struct Cyc_Core_Opt*_tmpC8D;_tmp68F->topt=((_tmpC8D=_cycalloc(sizeof(*
_tmpC8D)),((_tmpC8D->v=Cyc_Absyn_uint_typ,_tmpC8D))));}e->r=Cyc_Toc_subscript_exp_r(
_tmp4E8,_tmp68F);Cyc_Toc_exp_to_c(nv,e);goto _LL2FB;}_LL2FB:;}goto _LL2F6;_LL2F9:;
_LL2FA: {const char*_tmpC90;void*_tmpC8F;(_tmpC8F=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC90="exp_to_c: Deref: non-pointer",
_tag_dyneither(_tmpC90,sizeof(char),29))),_tag_dyneither(_tmpC8F,sizeof(void*),0)));}
_LL2F6:;}goto _LL210;}_LL243: {struct Cyc_Absyn_AggrMember_e_struct*_tmp4E9=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp497;if(_tmp4E9->tag != 22)goto _LL245;
else{_tmp4EA=_tmp4E9->f1;_tmp4EB=_tmp4E9->f2;_tmp4EC=_tmp4E9->f3;_tmp4ED=_tmp4E9->f4;}}
_LL244: {int is_poly=Cyc_Toc_is_poly_project(e);void*e1_cyc_type=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4EA->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp4EA);if(
_tmp4EC  && _tmp4ED)e->r=Cyc_Toc_check_tagged_union(_tmp4EA,Cyc_Toc_typ_to_c(
e1_cyc_type),e1_cyc_type,_tmp4EB,Cyc_Absyn_aggrmember_exp);if(is_poly)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL210;}_LL245: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp4EE=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp497;if(_tmp4EE->tag != 23)goto _LL247;
else{_tmp4EF=_tmp4EE->f1;_tmp4F0=_tmp4EE->f2;_tmp4F1=_tmp4EE->f3;_tmp4F2=_tmp4EE->f4;}}
_LL246: {void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4EF->topt))->v);int do_null_check=Cyc_Toc_need_null_check(_tmp4EF);Cyc_Toc_exp_to_c(
nv,_tmp4EF);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp694;struct Cyc_Absyn_Tqual
_tmp695;struct Cyc_Absyn_PtrAtts _tmp696;void*_tmp697;union Cyc_Absyn_Constraint*
_tmp698;union Cyc_Absyn_Constraint*_tmp699;union Cyc_Absyn_Constraint*_tmp69A;
struct Cyc_Absyn_PtrInfo _tmp693=Cyc_Toc_get_ptr_type(e1typ);_tmp694=_tmp693.elt_typ;
_tmp695=_tmp693.elt_tq;_tmp696=_tmp693.ptr_atts;_tmp697=_tmp696.rgn;_tmp698=
_tmp696.nullable;_tmp699=_tmp696.bounds;_tmp69A=_tmp696.zero_term;{void*_tmp69B=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp699);struct Cyc_Absyn_Exp*_tmp69D;_LL301: {struct Cyc_Absyn_Upper_b_struct*
_tmp69C=(struct Cyc_Absyn_Upper_b_struct*)_tmp69B;if(_tmp69C->tag != 1)goto _LL303;
else{_tmp69D=_tmp69C->f1;}}_LL302: {unsigned int _tmp6A0;int _tmp6A1;struct
_tuple10 _tmp69F=Cyc_Evexp_eval_const_uint_exp(_tmp69D);_tmp6A0=_tmp69F.f1;
_tmp6A1=_tmp69F.f2;if(_tmp6A1){if(_tmp6A0 < 1){const char*_tmpC93;void*_tmpC92;(
_tmpC92=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpC93="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpC93,
sizeof(char),44))),_tag_dyneither(_tmpC92,sizeof(void*),0)));}if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){const char*_tmpC96;void*_tmpC95;(_tmpC95=0,Cyc_Tcutil_warn(
e->loc,((_tmpC96="inserted null check due to dereference",_tag_dyneither(_tmpC96,
sizeof(char),39))),_tag_dyneither(_tmpC95,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpC97[1];_tmp4EF->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC97[0]=Cyc_Absyn_new_exp(_tmp4EF->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC97,sizeof(struct Cyc_Absyn_Exp*),
1)))),0));};}}else{if(!Cyc_Evexp_c_can_eval(_tmp69D)){const char*_tmpC9A;void*
_tmpC99;(_tmpC99=0,Cyc_Tcutil_terr(e->loc,((_tmpC9A="cannot determine pointer dereference in bounds",
_tag_dyneither(_tmpC9A,sizeof(char),47))),_tag_dyneither(_tmpC99,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpC9B[4];_tmp4EF->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((_tmpC9B[3]=
Cyc_Absyn_uint_exp(0,0),((_tmpC9B[2]=Cyc_Absyn_sizeoftyp_exp(_tmp694,0),((
_tmpC9B[1]=_tmp69D,((_tmpC9B[0]=Cyc_Absyn_new_exp(_tmp4EF->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC9B,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0));};}goto _LL300;}_LL303: {struct Cyc_Absyn_DynEither_b_struct*
_tmp69E=(struct Cyc_Absyn_DynEither_b_struct*)_tmp69B;if(_tmp69E->tag != 0)goto
_LL300;}_LL304: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp694);{struct Cyc_Absyn_Exp*
_tmpC9C[3];_tmp4EF->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp695),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_dyneither_subscript_e,((_tmpC9C[2]=Cyc_Absyn_uint_exp(0,0),((
_tmpC9C[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC9C[0]=Cyc_Absyn_new_exp(_tmp4EF->r,
0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC9C,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}goto _LL300;}_LL300:;}if(
_tmp4F1  && _tmp4F2)e->r=Cyc_Toc_check_tagged_union(_tmp4EF,Cyc_Toc_typ_to_c(
e1typ),_tmp694,_tmp4F0,Cyc_Absyn_aggrarrow_exp);if(is_poly  && _tmp4F2)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL210;};}_LL247: {struct Cyc_Absyn_Subscript_e_struct*_tmp4F3=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp497;if(_tmp4F3->tag != 24)goto _LL249;
else{_tmp4F4=_tmp4F3->f1;_tmp4F5=_tmp4F3->f2;}}_LL248: {void*_tmp6AB=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4F4->topt))->v);{void*_tmp6AC=
_tmp6AB;struct Cyc_List_List*_tmp6AE;struct Cyc_Absyn_PtrInfo _tmp6B0;void*_tmp6B1;
struct Cyc_Absyn_Tqual _tmp6B2;struct Cyc_Absyn_PtrAtts _tmp6B3;void*_tmp6B4;union
Cyc_Absyn_Constraint*_tmp6B5;union Cyc_Absyn_Constraint*_tmp6B6;union Cyc_Absyn_Constraint*
_tmp6B7;_LL306: {struct Cyc_Absyn_TupleType_struct*_tmp6AD=(struct Cyc_Absyn_TupleType_struct*)
_tmp6AC;if(_tmp6AD->tag != 11)goto _LL308;else{_tmp6AE=_tmp6AD->f1;}}_LL307: Cyc_Toc_exp_to_c(
nv,_tmp4F4);Cyc_Toc_exp_to_c(nv,_tmp4F5);{unsigned int _tmp6B9;int _tmp6BA;struct
_tuple10 _tmp6B8=Cyc_Evexp_eval_const_uint_exp(_tmp4F5);_tmp6B9=_tmp6B8.f1;
_tmp6BA=_tmp6B8.f2;if(!_tmp6BA){const char*_tmpC9F;void*_tmpC9E;(_tmpC9E=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC9F="unknown tuple subscript in translation to C",
_tag_dyneither(_tmpC9F,sizeof(char),44))),_tag_dyneither(_tmpC9E,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp4F4,Cyc_Absyn_fieldname((int)(_tmp6B9 + 1)));
goto _LL305;};_LL308: {struct Cyc_Absyn_PointerType_struct*_tmp6AF=(struct Cyc_Absyn_PointerType_struct*)
_tmp6AC;if(_tmp6AF->tag != 5)goto _LL30A;else{_tmp6B0=_tmp6AF->f1;_tmp6B1=_tmp6B0.elt_typ;
_tmp6B2=_tmp6B0.elt_tq;_tmp6B3=_tmp6B0.ptr_atts;_tmp6B4=_tmp6B3.rgn;_tmp6B5=
_tmp6B3.nullable;_tmp6B6=_tmp6B3.bounds;_tmp6B7=_tmp6B3.zero_term;}}_LL309: {
struct Cyc_List_List*_tmp6BD=Cyc_Toc_get_relns(_tmp4F4);int in_bnds=0;{void*
_tmp6BE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6B6);_LL30D:;_LL30E: in_bnds=Cyc_Toc_check_bounds(_tmp6AB,_tmp6BD,_tmp4F4,
_tmp4F5);if(Cyc_Toc_warn_bounds_checks  && !in_bnds){const char*_tmpCA3;void*
_tmpCA2[1];struct Cyc_String_pa_struct _tmpCA1;(_tmpCA1.tag=0,((_tmpCA1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCA2[0]=&
_tmpCA1,Cyc_Tcutil_warn(e->loc,((_tmpCA3="bounds check necessary for %s",
_tag_dyneither(_tmpCA3,sizeof(char),30))),_tag_dyneither(_tmpCA2,sizeof(void*),1)))))));}
_LL30C:;}Cyc_Toc_exp_to_c(nv,_tmp4F4);Cyc_Toc_exp_to_c(nv,_tmp4F5);++ Cyc_Toc_total_bounds_checks;{
void*_tmp6C2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp6B6);struct Cyc_Absyn_Exp*_tmp6C4;_LL310: {struct Cyc_Absyn_Upper_b_struct*
_tmp6C3=(struct Cyc_Absyn_Upper_b_struct*)_tmp6C2;if(_tmp6C3->tag != 1)goto _LL312;
else{_tmp6C4=_tmp6C3->f1;}}_LL311: {int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp6B5);void*ta1=Cyc_Toc_typ_to_c(_tmp6B1);void*ta2=
Cyc_Absyn_cstar_typ(ta1,_tmp6B2);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6B7)){if(!Cyc_Evexp_c_can_eval(_tmp6C4)){const char*_tmpCA6;void*_tmpCA5;(
_tmpCA5=0,Cyc_Tcutil_terr(e->loc,((_tmpCA6="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCA6,sizeof(char),40))),_tag_dyneither(_tmpCA5,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp4F4);struct Cyc_Absyn_Exp*_tmpCA7[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpCA7[2]=_tmp4F5,((_tmpCA7[1]=_tmp6C4,((
_tmpCA7[0]=_tmp4F4,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCA7,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp6C4)){const char*_tmpCAA;void*_tmpCA9;(
_tmpCA9=0,Cyc_Tcutil_terr(e->loc,((_tmpCAA="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCAA,sizeof(char),40))),_tag_dyneither(_tmpCA9,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCAD;void*_tmpCAC;(_tmpCAC=0,Cyc_Tcutil_warn(
e->loc,((_tmpCAD="inserted null check due to dereference",_tag_dyneither(_tmpCAD,
sizeof(char),39))),_tag_dyneither(_tmpCAC,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCAE[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((
_tmpCAE[3]=_tmp4F5,((_tmpCAE[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCAE[1]=
_tmp6C4,((_tmpCAE[0]=_tmp4F4,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCAE,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{if(
!Cyc_Evexp_c_can_eval(_tmp6C4)){const char*_tmpCB1;void*_tmpCB0;(_tmpCB0=0,Cyc_Tcutil_terr(
e->loc,((_tmpCB1="cannot determine subscript is in bounds",_tag_dyneither(
_tmpCB1,sizeof(char),40))),_tag_dyneither(_tmpCB0,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCB2[2];_tmp4F5->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((
_tmpCB2[1]=Cyc_Absyn_copy_exp(_tmp4F5),((_tmpCB2[0]=_tmp6C4,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB2,sizeof(struct Cyc_Absyn_Exp*),
2)))))));};}}}goto _LL30F;}_LL312: {struct Cyc_Absyn_DynEither_b_struct*_tmp6C5=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp6C2;if(_tmp6C5->tag != 0)goto _LL30F;}
_LL313: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp6B1);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6B2),
Cyc_Absyn_aggrmember_exp(_tmp4F4,Cyc_Toc_curr_sp,0)),_tmp4F5);}else{struct Cyc_Absyn_Exp*
_tmp6D1=Cyc_Toc__check_dyneither_subscript_e;struct Cyc_Absyn_Exp*_tmpCB3[3];e->r=
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6B2),Cyc_Absyn_fncall_exp(
_tmp6D1,((_tmpCB3[2]=_tmp4F5,((_tmpCB3[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((
_tmpCB3[0]=_tmp4F4,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCB3,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL30F;}
_LL30F:;}goto _LL305;}_LL30A:;_LL30B: {const char*_tmpCB6;void*_tmpCB5;(_tmpCB5=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCB6="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(
_tmpCB6,sizeof(char),49))),_tag_dyneither(_tmpCB5,sizeof(void*),0)));}_LL305:;}
goto _LL210;}_LL249: {struct Cyc_Absyn_Tuple_e_struct*_tmp4F6=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp497;if(_tmp4F6->tag != 25)goto _LL24B;else{_tmp4F7=_tmp4F6->f1;}}_LL24A: if(!
Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp4F7))->r;else{struct
Cyc_List_List*_tmp6D5=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp4F7);void*_tmp6D6=Cyc_Toc_add_tuple_type(
_tmp6D5);struct Cyc_List_List*dles=0;{int i=1;for(0;_tmp4F7 != 0;(_tmp4F7=_tmp4F7->tl,
i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4F7->hd);{struct _tuple15*
_tmpCB9;struct Cyc_List_List*_tmpCB8;dles=((_tmpCB8=_cycalloc(sizeof(*_tmpCB8)),((
_tmpCB8->hd=((_tmpCB9=_cycalloc(sizeof(*_tmpCB9)),((_tmpCB9->f1=0,((_tmpCB9->f2=(
struct Cyc_Absyn_Exp*)_tmp4F7->hd,_tmpCB9)))))),((_tmpCB8->tl=dles,_tmpCB8))))));};}}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);e->r=
Cyc_Toc_unresolvedmem_exp_r(0,dles);}goto _LL210;_LL24B: {struct Cyc_Absyn_Array_e_struct*
_tmp4F8=(struct Cyc_Absyn_Array_e_struct*)_tmp497;if(_tmp4F8->tag != 27)goto _LL24D;
else{_tmp4F9=_tmp4F8->f1;}}_LL24C: e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4F9);{
struct Cyc_List_List*_tmp6D9=_tmp4F9;for(0;_tmp6D9 != 0;_tmp6D9=_tmp6D9->tl){
struct _tuple15 _tmp6DB;struct Cyc_Absyn_Exp*_tmp6DC;struct _tuple15*_tmp6DA=(struct
_tuple15*)_tmp6D9->hd;_tmp6DB=*_tmp6DA;_tmp6DC=_tmp6DB.f2;Cyc_Toc_exp_to_c(nv,
_tmp6DC);}}goto _LL210;_LL24D: {struct Cyc_Absyn_Comprehension_e_struct*_tmp4FA=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmp497;if(_tmp4FA->tag != 28)goto _LL24F;
else{_tmp4FB=_tmp4FA->f1;_tmp4FC=_tmp4FA->f2;_tmp4FD=_tmp4FA->f3;_tmp4FE=_tmp4FA->f4;}}
_LL24E: {unsigned int _tmp6DE;int _tmp6DF;struct _tuple10 _tmp6DD=Cyc_Evexp_eval_const_uint_exp(
_tmp4FC);_tmp6DE=_tmp6DD.f1;_tmp6DF=_tmp6DD.f2;{void*_tmp6E0=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4FD->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp4FD);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp4FD)){if(!_tmp6DF){
const char*_tmpCBC;void*_tmpCBB;(_tmpCBB=0,Cyc_Tcutil_terr(_tmp4FC->loc,((_tmpCBC="cannot determine value of constant",
_tag_dyneither(_tmpCBC,sizeof(char),35))),_tag_dyneither(_tmpCBB,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp6DE;++ i){struct _tuple15*_tmpCBF;struct Cyc_List_List*
_tmpCBE;es=((_tmpCBE=_cycalloc(sizeof(*_tmpCBE)),((_tmpCBE->hd=((_tmpCBF=
_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF->f1=0,((_tmpCBF->f2=_tmp4FD,_tmpCBF)))))),((
_tmpCBE->tl=es,_tmpCBE))))));}}if(_tmp4FE){struct Cyc_Absyn_Exp*_tmp6E5=Cyc_Toc_cast_it(
_tmp6E0,Cyc_Absyn_uint_exp(0,0));struct _tuple15*_tmpCC2;struct Cyc_List_List*
_tmpCC1;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,((_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((_tmpCC1->hd=((
_tmpCC2=_cycalloc(sizeof(*_tmpCC2)),((_tmpCC2->f1=0,((_tmpCC2->f2=_tmp6E5,
_tmpCC2)))))),((_tmpCC1->tl=0,_tmpCC1)))))));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL210;};};}_LL24F: {struct Cyc_Absyn_Aggregate_e_struct*_tmp4FF=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp497;if(_tmp4FF->tag != 29)goto _LL251;
else{_tmp500=_tmp4FF->f1;_tmp501=_tmp4FF->f2;_tmp502=_tmp4FF->f3;_tmp503=_tmp4FF->f4;}}
_LL250: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp501
!= 0,0,0,_tmp502,_tmp500))->r;else{if(_tmp503 == 0){const char*_tmpCC5;void*
_tmpCC4;(_tmpCC4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCC5="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpCC5,sizeof(
char),38))),_tag_dyneither(_tmpCC4,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp503;struct Cyc_Toc_Env _tmp6EB;struct
_RegionHandle*_tmp6EC;struct Cyc_Toc_Env*_tmp6EA=nv;_tmp6EB=*_tmp6EA;_tmp6EC=
_tmp6EB.rgn;{struct Cyc_List_List*_tmp6ED=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum 
Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp6EC,e->loc,_tmp502,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp503->impl))->tagged){struct
_tuple20 _tmp6EF;struct Cyc_Absyn_Aggrfield*_tmp6F0;struct Cyc_Absyn_Exp*_tmp6F1;
struct _tuple20*_tmp6EE=(struct _tuple20*)((struct Cyc_List_List*)_check_null(
_tmp6ED))->hd;_tmp6EF=*_tmp6EE;_tmp6F0=_tmp6EF.f1;_tmp6F1=_tmp6EF.f2;{void*
_tmp6F2=_tmp6F0->type;Cyc_Toc_exp_to_c(nv,_tmp6F1);if(Cyc_Toc_is_void_star_or_tvar(
_tmp6F2))_tmp6F1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp6F1->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp503,_tmp6F0->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple15*_tmpCCA;struct _tuple15*_tmpCC9;struct _tuple15*_tmpCC8[2];
struct Cyc_List_List*_tmp6F3=(_tmpCC8[1]=((_tmpCC9=_cycalloc(sizeof(*_tmpCC9)),((
_tmpCC9->f1=0,((_tmpCC9->f2=_tmp6F1,_tmpCC9)))))),((_tmpCC8[0]=((_tmpCCA=
_cycalloc(sizeof(*_tmpCCA)),((_tmpCCA->f1=0,((_tmpCCA->f2=field_tag_exp,_tmpCCA)))))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCC8,sizeof(struct _tuple15*),2)))));struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(
0,_tmp6F3,0);struct Cyc_Absyn_FieldName_struct*_tmpCD0;struct Cyc_Absyn_FieldName_struct
_tmpCCF;void*_tmpCCE[1];struct Cyc_List_List*ds=(_tmpCCE[0]=(void*)((_tmpCD0=
_cycalloc(sizeof(*_tmpCD0)),((_tmpCD0[0]=((_tmpCCF.tag=1,((_tmpCCF.f1=_tmp6F0->name,
_tmpCCF)))),_tmpCD0)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCCE,sizeof(void*),1)));struct _tuple15*_tmpCD3;struct _tuple15*
_tmpCD2[1];struct Cyc_List_List*dles=(_tmpCD2[0]=((_tmpCD3=_cycalloc(sizeof(*
_tmpCD3)),((_tmpCD3->f1=ds,((_tmpCD3->f2=umem,_tmpCD3)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD2,sizeof(struct
_tuple15*),1)));e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{struct Cyc_List_List*
_tmp6FC=0;struct Cyc_List_List*_tmp6FD=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp6FD != 0;_tmp6FD=_tmp6FD->tl){struct Cyc_List_List*
_tmp6FE=_tmp6ED;for(0;_tmp6FE != 0;_tmp6FE=_tmp6FE->tl){if((*((struct _tuple20*)
_tmp6FE->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp6FD->hd){struct _tuple20
_tmp700;struct Cyc_Absyn_Aggrfield*_tmp701;struct Cyc_Absyn_Exp*_tmp702;struct
_tuple20*_tmp6FF=(struct _tuple20*)_tmp6FE->hd;_tmp700=*_tmp6FF;_tmp701=_tmp700.f1;
_tmp702=_tmp700.f2;{void*_tmp703=_tmp701->type;Cyc_Toc_exp_to_c(nv,_tmp702);if(
Cyc_Toc_is_void_star_or_tvar(_tmp703))_tmp702->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp702->r,0));{struct _tuple15*_tmpCD6;struct Cyc_List_List*
_tmpCD5;_tmp6FC=((_tmpCD5=_cycalloc(sizeof(*_tmpCD5)),((_tmpCD5->hd=((_tmpCD6=
_cycalloc(sizeof(*_tmpCD6)),((_tmpCD6->f1=0,((_tmpCD6->f2=_tmp702,_tmpCD6)))))),((
_tmpCD5->tl=_tmp6FC,_tmpCD5))))));}break;};}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6FC));}};};}
goto _LL210;_LL251: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp504=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp497;if(_tmp504->tag != 30)goto _LL253;else{_tmp505=(void*)_tmp504->f1;_tmp506=
_tmp504->f2;}}_LL252: {struct Cyc_List_List*fs;{void*_tmp706=Cyc_Tcutil_compress(
_tmp505);struct Cyc_List_List*_tmp708;_LL315: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp707=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp706;if(_tmp707->tag != 13)goto
_LL317;else{_tmp708=_tmp707->f2;}}_LL316: fs=_tmp708;goto _LL314;_LL317:;_LL318: {
const char*_tmpCDA;void*_tmpCD9[1];struct Cyc_String_pa_struct _tmpCD8;(_tmpCD8.tag=
0,((_tmpCD8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp505)),((_tmpCD9[0]=& _tmpCD8,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCDA="anon struct has type %s",
_tag_dyneither(_tmpCDA,sizeof(char),24))),_tag_dyneither(_tmpCD9,sizeof(void*),1)))))));}
_LL314:;}{struct Cyc_Toc_Env _tmp70D;struct _RegionHandle*_tmp70E;struct Cyc_Toc_Env*
_tmp70C=nv;_tmp70D=*_tmp70C;_tmp70E=_tmp70D.rgn;{struct Cyc_List_List*_tmp70F=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp70E,e->loc,_tmp506,Cyc_Absyn_StructA,fs);for(0;_tmp70F != 0;_tmp70F=_tmp70F->tl){
struct _tuple20 _tmp711;struct Cyc_Absyn_Aggrfield*_tmp712;struct Cyc_Absyn_Exp*
_tmp713;struct _tuple20*_tmp710=(struct _tuple20*)_tmp70F->hd;_tmp711=*_tmp710;
_tmp712=_tmp711.f1;_tmp713=_tmp711.f2;{void*_tmp714=_tmp712->type;Cyc_Toc_exp_to_c(
nv,_tmp713);if(Cyc_Toc_is_void_star_or_tvar(_tmp714))_tmp713->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp713->r,0));};}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp506);}goto _LL210;};}_LL253: {struct Cyc_Absyn_Datatype_e_struct*_tmp507=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp497;if(_tmp507->tag != 31)goto _LL255;else{
_tmp508=_tmp507->f1;_tmp509=_tmp507->f2;_tmp50A=_tmp507->f3;}}_LL254: {void*
datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple0*_tmp715=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp716=Cyc_Absyn_var_exp(_tmp715,0);struct Cyc_Absyn_Exp*
member_exp;{const char*_tmpCDB;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp50A->name,((_tmpCDB="_struct",_tag_dyneither(_tmpCDB,sizeof(char),8)))));}
tag_exp=_tmp509->is_extensible?Cyc_Absyn_var_exp(_tmp50A->name,0): Cyc_Toc_datatype_tag(
_tmp509,_tmp50A->name);member_exp=_tmp716;{struct Cyc_List_List*_tmp718=_tmp50A->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp508 != 0;(
_tmp508=_tmp508->tl,_tmp718=_tmp718->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp508->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp718))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct _tuple15*_tmpCDE;struct
Cyc_List_List*_tmpCDD;dles=((_tmpCDD=_cycalloc(sizeof(*_tmpCDD)),((_tmpCDD->hd=((
_tmpCDE=_cycalloc(sizeof(*_tmpCDE)),((_tmpCDE->f1=0,((_tmpCDE->f2=cur_e,_tmpCDE)))))),((
_tmpCDD->tl=dles,_tmpCDD))))));};}{struct _tuple15*_tmpCE1;struct Cyc_List_List*
_tmpCE0;dles=((_tmpCE0=_cycalloc(sizeof(*_tmpCE0)),((_tmpCE0->hd=((_tmpCE1=
_cycalloc(sizeof(*_tmpCE1)),((_tmpCE1->f1=0,((_tmpCE1->f2=tag_exp,_tmpCE1)))))),((
_tmpCE0->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles),_tmpCE0))))));}e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{struct Cyc_List_List*
_tmpCE2;struct Cyc_List_List*_tmp71D=(_tmpCE2=_cycalloc(sizeof(*_tmpCE2)),((
_tmpCE2->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,
0),tag_exp,0),((_tmpCE2->tl=0,_tmpCE2)))));{int i=1;for(0;_tmp508 != 0;(((_tmp508=
_tmp508->tl,i ++)),_tmp718=_tmp718->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp508->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp718))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp71E=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpCE3;_tmp71D=((_tmpCE3=_cycalloc(sizeof(*
_tmpCE3)),((_tmpCE3->hd=_tmp71E,((_tmpCE3->tl=_tmp71D,_tmpCE3))))));};}}{struct
Cyc_Absyn_Stmt*_tmp720=Cyc_Absyn_exp_stmt(_tmp716,0);struct Cyc_List_List*_tmpCE4;
struct Cyc_Absyn_Stmt*_tmp721=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpCE4=_cycalloc(sizeof(*_tmpCE4)),((
_tmpCE4->hd=_tmp720,((_tmpCE4->tl=_tmp71D,_tmpCE4))))))),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp715,datatype_ctype,0,_tmp721,0));};}goto _LL210;};}
_LL255: {struct Cyc_Absyn_Enum_e_struct*_tmp50B=(struct Cyc_Absyn_Enum_e_struct*)
_tmp497;if(_tmp50B->tag != 32)goto _LL257;}_LL256: goto _LL258;_LL257: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp50C=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp497;if(_tmp50C->tag != 33)goto
_LL259;}_LL258: goto _LL210;_LL259: {struct Cyc_Absyn_Malloc_e_struct*_tmp50D=(
struct Cyc_Absyn_Malloc_e_struct*)_tmp497;if(_tmp50D->tag != 34)goto _LL25B;else{
_tmp50E=_tmp50D->f1;_tmp50F=_tmp50E.is_calloc;_tmp510=_tmp50E.rgn;_tmp511=
_tmp50E.elt_type;_tmp512=_tmp50E.num_elts;_tmp513=_tmp50E.fat_result;}}_LL25A: {
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp511)));Cyc_Toc_exp_to_c(nv,
_tmp512);if(_tmp513){struct _tuple0*_tmp724=Cyc_Toc_temp_var();struct _tuple0*
_tmp725=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp50F){xexp=_tmp512;if(_tmp510 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp510;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp724,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp511,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp724,0));}{struct Cyc_Absyn_Exp*_tmpCE5[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpCE5[2]=Cyc_Absyn_var_exp(_tmp724,0),((_tmpCE5[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpCE5[0]=Cyc_Absyn_var_exp(_tmp725,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCE5,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{xexp=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp512,0);if(_tmp510 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp510;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp724,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp511,Cyc_Absyn_var_exp(_tmp724,0));}{struct Cyc_Absyn_Exp*_tmpCE6[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpCE6[2]=Cyc_Absyn_var_exp(_tmp724,0),((_tmpCE6[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpCE6[0]=Cyc_Absyn_var_exp(_tmp725,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE6,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);};}{struct Cyc_Absyn_Stmt*_tmp728=Cyc_Absyn_declare_stmt(_tmp724,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp725,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp728);};}else{struct Cyc_Absyn_Exp*_tmp729=Cyc_Absyn_sizeoftyp_exp(
t_c,0);{void*_tmp72A=_tmp512->r;union Cyc_Absyn_Cnst _tmp72C;struct _tuple5 _tmp72D;
int _tmp72E;_LL31A: {struct Cyc_Absyn_Const_e_struct*_tmp72B=(struct Cyc_Absyn_Const_e_struct*)
_tmp72A;if(_tmp72B->tag != 0)goto _LL31C;else{_tmp72C=_tmp72B->f1;if((_tmp72C.Int_c).tag
!= 4)goto _LL31C;_tmp72D=(struct _tuple5)(_tmp72C.Int_c).val;_tmp72E=_tmp72D.f2;
if(_tmp72E != 1)goto _LL31C;}}_LL31B: goto _LL319;_LL31C:;_LL31D: _tmp729=Cyc_Absyn_times_exp(
_tmp729,_tmp512,0);goto _LL319;_LL319:;}if(_tmp510 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp510;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rmalloc_exp(rgn,_tmp729))->r;}else{e->r=(Cyc_Toc_malloc_exp(*_tmp511,
_tmp729))->r;}}goto _LL210;}_LL25B: {struct Cyc_Absyn_Swap_e_struct*_tmp514=(
struct Cyc_Absyn_Swap_e_struct*)_tmp497;if(_tmp514->tag != 35)goto _LL25D;else{
_tmp515=_tmp514->f1;_tmp516=_tmp514->f2;}}_LL25C: {int is_dyneither_ptr=0;void*
e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp515->topt))->v;void*
e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp516->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(
e1_old_typ,& is_dyneither_ptr)){const char*_tmpCE9;void*_tmpCE8;(_tmpCE8=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCE9="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpCE9,sizeof(char),57))),_tag_dyneither(_tmpCE8,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}{int f1_tag=0;void*tagged_mem_type1=(void*)&
Cyc_Absyn_VoidType_val;int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp515,&
f1_tag,& tagged_mem_type1,1);int f2_tag=0;void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp516,& f2_tag,& tagged_mem_type2,1);
Cyc_Toc_exp_to_c(nv,_tmp515);Cyc_Toc_exp_to_c(nv,_tmp516);if(e1_tmem)Cyc_Toc_add_tagged_union_check_for_swap(
_tmp515,f1_tag,tagged_mem_type1);else{_tmp515=Cyc_Toc_push_address_exp(_tmp515);}
if(e2_tmem)Cyc_Toc_add_tagged_union_check_for_swap(_tmp516,f2_tag,
tagged_mem_type2);else{_tmp516=Cyc_Toc_push_address_exp(_tmp516);}{struct Cyc_Absyn_Exp*
_tmpCEA[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpCEA[1]=_tmp516,((_tmpCEA[0]=
_tmp515,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCEA,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL210;};};}
_LL25D: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp517=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp497;if(_tmp517->tag != 38)goto _LL25F;else{_tmp518=_tmp517->f1;_tmp519=_tmp517->f2;}}
_LL25E: {void*_tmp732=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp518->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp518);{void*_tmp733=
_tmp732;struct Cyc_Absyn_AggrInfo _tmp735;union Cyc_Absyn_AggrInfoU _tmp736;struct
Cyc_Absyn_Aggrdecl**_tmp737;struct Cyc_Absyn_Aggrdecl*_tmp738;_LL31F: {struct Cyc_Absyn_AggrType_struct*
_tmp734=(struct Cyc_Absyn_AggrType_struct*)_tmp733;if(_tmp734->tag != 12)goto
_LL321;else{_tmp735=_tmp734->f1;_tmp736=_tmp735.aggr_info;if((_tmp736.KnownAggr).tag
!= 2)goto _LL321;_tmp737=(struct Cyc_Absyn_Aggrdecl**)(_tmp736.KnownAggr).val;
_tmp738=*_tmp737;}}_LL320: {struct Cyc_Absyn_Exp*_tmp739=Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp738,_tmp519),0);struct Cyc_Absyn_Exp*_tmp73A=Cyc_Absyn_aggrmember_exp(
_tmp518,_tmp519,0);struct Cyc_Absyn_Exp*_tmp73B=Cyc_Absyn_aggrmember_exp(_tmp73A,
Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(_tmp73B,_tmp739,0))->r;goto _LL31E;}
_LL321:;_LL322: {const char*_tmpCED;void*_tmpCEC;(_tmpCEC=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCED="non-aggregate type in tagcheck",
_tag_dyneither(_tmpCED,sizeof(char),31))),_tag_dyneither(_tmpCEC,sizeof(void*),0)));}
_LL31E:;}goto _LL210;}_LL25F: {struct Cyc_Absyn_StmtExp_e_struct*_tmp51A=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp497;if(_tmp51A->tag != 37)goto _LL261;else{_tmp51B=
_tmp51A->f1;}}_LL260: Cyc_Toc_stmt_to_c(nv,_tmp51B);goto _LL210;_LL261: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp51C=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp497;if(_tmp51C->tag != 36)
goto _LL263;}_LL262: {const char*_tmpCF0;void*_tmpCEF;(_tmpCEF=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF0="UnresolvedMem",
_tag_dyneither(_tmpCF0,sizeof(char),14))),_tag_dyneither(_tmpCEF,sizeof(void*),0)));}
_LL263: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp51D=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp497;if(_tmp51D->tag != 26)goto _LL265;}_LL264: {const char*_tmpCF3;void*_tmpCF2;(
_tmpCF2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpCF3="compoundlit",_tag_dyneither(_tmpCF3,sizeof(char),12))),_tag_dyneither(
_tmpCF2,sizeof(void*),0)));}_LL265: {struct Cyc_Absyn_Valueof_e_struct*_tmp51E=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp497;if(_tmp51E->tag != 39)goto _LL267;}
_LL266: {const char*_tmpCF6;void*_tmpCF5;(_tmpCF5=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF6="valueof(-)",
_tag_dyneither(_tmpCF6,sizeof(char),11))),_tag_dyneither(_tmpCF5,sizeof(void*),0)));}
_LL267: {struct Cyc_Absyn_Asm_e_struct*_tmp51F=(struct Cyc_Absyn_Asm_e_struct*)
_tmp497;if(_tmp51F->tag != 40)goto _LL210;}_LL268: {const char*_tmpCF9;void*_tmpCF8;(
_tmpCF8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCF9="__asm__",_tag_dyneither(_tmpCF9,sizeof(char),8))),_tag_dyneither(
_tmpCF8,sizeof(void*),0)));}_LL210:;};}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0);}struct _tuple21{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*
f3;};struct _tuple22{struct _tuple0*f1;void*f2;};struct _tuple23{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple21 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*
nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
struct Cyc_Absyn_Stmt*s;{void*_tmp746=p->r;struct Cyc_Absyn_Vardecl*_tmp748;struct
Cyc_Absyn_Vardecl _tmp749;struct _tuple0*_tmp74A;struct Cyc_Absyn_Pat*_tmp74B;
struct Cyc_Absyn_Vardecl*_tmp74D;struct Cyc_Absyn_Vardecl _tmp74E;struct _tuple0*
_tmp74F;struct Cyc_Absyn_Vardecl*_tmp752;struct Cyc_Absyn_Pat*_tmp753;enum Cyc_Absyn_Sign
_tmp756;int _tmp757;char _tmp759;struct _dyneither_ptr _tmp75B;struct Cyc_Absyn_Enumdecl*
_tmp75D;struct Cyc_Absyn_Enumfield*_tmp75E;void*_tmp760;struct Cyc_Absyn_Enumfield*
_tmp761;struct Cyc_Absyn_Pat*_tmp763;struct Cyc_Absyn_Pat _tmp764;void*_tmp765;
struct Cyc_Absyn_Datatypedecl*_tmp767;struct Cyc_Absyn_Datatypefield*_tmp768;
struct Cyc_List_List*_tmp769;struct Cyc_List_List*_tmp76B;struct Cyc_List_List*
_tmp76D;struct Cyc_Absyn_AggrInfo*_tmp76F;struct Cyc_Absyn_AggrInfo*_tmp771;struct
Cyc_Absyn_AggrInfo _tmp772;union Cyc_Absyn_AggrInfoU _tmp773;struct Cyc_List_List*
_tmp774;struct Cyc_Absyn_Pat*_tmp776;_LL324: {struct Cyc_Absyn_Var_p_struct*
_tmp747=(struct Cyc_Absyn_Var_p_struct*)_tmp746;if(_tmp747->tag != 1)goto _LL326;
else{_tmp748=_tmp747->f1;_tmp749=*_tmp748;_tmp74A=_tmp749.name;_tmp74B=_tmp747->f2;}}
_LL325: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp74A,r),rgn,t,r,path,
_tmp74B,fail_stmt,decls);_LL326: {struct Cyc_Absyn_TagInt_p_struct*_tmp74C=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp746;if(_tmp74C->tag != 3)goto _LL328;else{
_tmp74D=_tmp74C->f2;_tmp74E=*_tmp74D;_tmp74F=_tmp74E.name;}}_LL327: nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp74F,r);goto _LL329;_LL328: {struct Cyc_Absyn_Wild_p_struct*_tmp750=(
struct Cyc_Absyn_Wild_p_struct*)_tmp746;if(_tmp750->tag != 0)goto _LL32A;}_LL329: s=
Cyc_Toc_skip_stmt_dl();goto _LL323;_LL32A: {struct Cyc_Absyn_Reference_p_struct*
_tmp751=(struct Cyc_Absyn_Reference_p_struct*)_tmp746;if(_tmp751->tag != 2)goto
_LL32C;else{_tmp752=_tmp751->f1;_tmp753=_tmp751->f2;}}_LL32B: {struct _tuple0*
_tmp77A=Cyc_Toc_temp_var();{struct _tuple22*_tmpCFC;struct Cyc_List_List*_tmpCFB;
decls=((_tmpCFB=_region_malloc(rgn,sizeof(*_tmpCFB)),((_tmpCFB->hd=((_tmpCFC=
_region_malloc(rgn,sizeof(*_tmpCFC)),((_tmpCFC->f1=_tmp77A,((_tmpCFC->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpCFC)))))),((_tmpCFB->tl=decls,_tmpCFB))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp752->name,Cyc_Absyn_var_exp(_tmp77A,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp77A,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Toc_push_address_exp(path)),0);{struct _tuple21 _tmp77D=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp753,fail_stmt,decls);_tmp77D.f3=Cyc_Absyn_seq_stmt(s,_tmp77D.f3,
0);return _tmp77D;};}_LL32C: {struct Cyc_Absyn_Null_p_struct*_tmp754=(struct Cyc_Absyn_Null_p_struct*)
_tmp746;if(_tmp754->tag != 8)goto _LL32E;}_LL32D: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),fail_stmt);goto _LL323;_LL32E: {struct Cyc_Absyn_Int_p_struct*_tmp755=(struct
Cyc_Absyn_Int_p_struct*)_tmp746;if(_tmp755->tag != 9)goto _LL330;else{_tmp756=
_tmp755->f1;_tmp757=_tmp755->f2;}}_LL32F: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp756,_tmp757,0),fail_stmt);goto _LL323;_LL330: {struct Cyc_Absyn_Char_p_struct*
_tmp758=(struct Cyc_Absyn_Char_p_struct*)_tmp746;if(_tmp758->tag != 10)goto _LL332;
else{_tmp759=_tmp758->f1;}}_LL331: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(
_tmp759,0),fail_stmt);goto _LL323;_LL332: {struct Cyc_Absyn_Float_p_struct*_tmp75A=(
struct Cyc_Absyn_Float_p_struct*)_tmp746;if(_tmp75A->tag != 11)goto _LL334;else{
_tmp75B=_tmp75A->f1;}}_LL333: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp75B,
0),fail_stmt);goto _LL323;_LL334: {struct Cyc_Absyn_Enum_p_struct*_tmp75C=(struct
Cyc_Absyn_Enum_p_struct*)_tmp746;if(_tmp75C->tag != 12)goto _LL336;else{_tmp75D=
_tmp75C->f1;_tmp75E=_tmp75C->f2;}}_LL335:{struct Cyc_Absyn_Enum_e_struct _tmpCFF;
struct Cyc_Absyn_Enum_e_struct*_tmpCFE;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpCFE=_cycalloc(sizeof(*_tmpCFE)),((_tmpCFE[0]=((_tmpCFF.tag=32,((
_tmpCFF.f1=_tmp75E->name,((_tmpCFF.f2=(struct Cyc_Absyn_Enumdecl*)_tmp75D,((
_tmpCFF.f3=(struct Cyc_Absyn_Enumfield*)_tmp75E,_tmpCFF)))))))),_tmpCFE)))),0),
fail_stmt);}goto _LL323;_LL336: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp75F=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp746;if(_tmp75F->tag != 13)goto _LL338;else{
_tmp760=(void*)_tmp75F->f1;_tmp761=_tmp75F->f2;}}_LL337:{struct Cyc_Absyn_AnonEnum_e_struct
_tmpD02;struct Cyc_Absyn_AnonEnum_e_struct*_tmpD01;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD01=_cycalloc(sizeof(*_tmpD01)),((_tmpD01[0]=((_tmpD02.tag=33,((
_tmpD02.f1=_tmp761->name,((_tmpD02.f2=(void*)_tmp760,((_tmpD02.f3=(struct Cyc_Absyn_Enumfield*)
_tmp761,_tmpD02)))))))),_tmpD01)))),0),fail_stmt);}goto _LL323;_LL338: {struct Cyc_Absyn_Pointer_p_struct*
_tmp762=(struct Cyc_Absyn_Pointer_p_struct*)_tmp746;if(_tmp762->tag != 5)goto
_LL33A;else{_tmp763=_tmp762->f1;_tmp764=*_tmp763;_tmp765=_tmp764.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp766=(struct Cyc_Absyn_Datatype_p_struct*)_tmp765;if(_tmp766->tag != 7)goto
_LL33A;else{_tmp767=_tmp766->f1;_tmp768=_tmp766->f2;_tmp769=_tmp766->f3;}};}}
_LL339: s=Cyc_Toc_skip_stmt_dl();{struct _tuple0*_tmp782=Cyc_Toc_temp_var();const
char*_tmpD03;struct _tuple0*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp768->name,((
_tmpD03="_struct",_tag_dyneither(_tmpD03,sizeof(char),8))));void*_tmp783=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);int cnt=1;struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
_tmp783,r);struct Cyc_List_List*_tmp784=_tmp768->typs;for(0;_tmp769 != 0;(((
_tmp769=_tmp769->tl,_tmp784=((struct Cyc_List_List*)_check_null(_tmp784))->tl)),
++ cnt)){struct Cyc_Absyn_Pat*_tmp785=(struct Cyc_Absyn_Pat*)_tmp769->hd;if(_tmp785->r
== (void*)& Cyc_Absyn_Wild_p_val)continue;{void*_tmp786=(*((struct _tuple9*)((
struct Cyc_List_List*)_check_null(_tmp784))->hd)).f2;struct _tuple0*_tmp787=Cyc_Toc_temp_var();
void*_tmp788=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp785->topt))->v;void*
_tmp789=Cyc_Toc_typ_to_c(_tmp788);struct Cyc_Absyn_Exp*_tmp78A=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp782,0),Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_tvar(
Cyc_Toc_typ_to_c(_tmp786)))_tmp78A=Cyc_Toc_cast_it(_tmp789,_tmp78A);{struct
_tuple22*_tmpD06;struct Cyc_List_List*_tmpD05;decls=((_tmpD05=_region_malloc(rgn,
sizeof(*_tmpD05)),((_tmpD05->hd=((_tmpD06=_region_malloc(rgn,sizeof(*_tmpD06)),((
_tmpD06->f1=_tmp787,((_tmpD06->f2=_tmp789,_tmpD06)))))),((_tmpD05->tl=decls,
_tmpD05))))));}{struct _tuple21 _tmp78D=Cyc_Toc_xlate_pat(nv,rgn,_tmp788,Cyc_Absyn_var_exp(
_tmp787,0),_tmp78A,_tmp785,fail_stmt,decls);nv=_tmp78D.f1;decls=_tmp78D.f2;{
struct Cyc_Absyn_Stmt*_tmp78E=_tmp78D.f3;struct Cyc_Absyn_Stmt*_tmp78F=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp787,0),_tmp78A,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp78F,_tmp78E,0),0);};};};}{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp782,0),Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*tag_exp=
_tmp767->is_extensible?Cyc_Absyn_var_exp(_tmp768->name,0): Cyc_Toc_datatype_tag(
_tmp767,_tmp768->name);struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,
tag_exp,0);s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);if(Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v))s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp782,0),Cyc_Absyn_uint_exp(0,0),0),
fail_stmt,s,0);s=Cyc_Absyn_declare_stmt(_tmp782,_tmp783,(struct Cyc_Absyn_Exp*)
rcast,s,0);goto _LL323;};};_LL33A: {struct Cyc_Absyn_Datatype_p_struct*_tmp76A=(
struct Cyc_Absyn_Datatype_p_struct*)_tmp746;if(_tmp76A->tag != 7)goto _LL33C;else{
_tmp76B=_tmp76A->f3;}}_LL33B: _tmp76D=_tmp76B;goto _LL33D;_LL33C: {struct Cyc_Absyn_Tuple_p_struct*
_tmp76C=(struct Cyc_Absyn_Tuple_p_struct*)_tmp746;if(_tmp76C->tag != 4)goto _LL33E;
else{_tmp76D=_tmp76C->f1;}}_LL33D: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;
_tmp76D != 0;(_tmp76D=_tmp76D->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp791=(struct Cyc_Absyn_Pat*)
_tmp76D->hd;if(_tmp791->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{struct _tuple0*
_tmp792=Cyc_Toc_temp_var();void*_tmp793=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp791->topt))->v;{struct _tuple22*_tmpD09;struct Cyc_List_List*_tmpD08;decls=((
_tmpD08=_region_malloc(rgn,sizeof(*_tmpD08)),((_tmpD08->hd=((_tmpD09=
_region_malloc(rgn,sizeof(*_tmpD09)),((_tmpD09->f1=_tmp792,((_tmpD09->f2=Cyc_Toc_typ_to_c(
_tmp793),_tmpD09)))))),((_tmpD08->tl=decls,_tmpD08))))));}{struct _tuple21 _tmp796=
Cyc_Toc_xlate_pat(nv,rgn,_tmp793,Cyc_Absyn_var_exp(_tmp792,0),Cyc_Absyn_aggrmember_exp(
path,Cyc_Absyn_fieldname(cnt),0),_tmp791,fail_stmt,decls);nv=_tmp796.f1;decls=
_tmp796.f2;{struct Cyc_Absyn_Stmt*_tmp797=_tmp796.f3;struct Cyc_Absyn_Stmt*_tmp798=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp792,0),Cyc_Absyn_aggrmember_exp(r,
Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp798,
_tmp797,0),0);};};};}goto _LL323;};_LL33E: {struct Cyc_Absyn_Aggr_p_struct*_tmp76E=(
struct Cyc_Absyn_Aggr_p_struct*)_tmp746;if(_tmp76E->tag != 6)goto _LL340;else{
_tmp76F=_tmp76E->f1;if(_tmp76F != 0)goto _LL340;}}_LL33F: {const char*_tmpD0C;void*
_tmpD0B;(_tmpD0B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD0C="unresolved aggregate pattern!",_tag_dyneither(_tmpD0C,sizeof(char),30))),
_tag_dyneither(_tmpD0B,sizeof(void*),0)));}_LL340: {struct Cyc_Absyn_Aggr_p_struct*
_tmp770=(struct Cyc_Absyn_Aggr_p_struct*)_tmp746;if(_tmp770->tag != 6)goto _LL342;
else{_tmp771=_tmp770->f1;if(_tmp771 == 0)goto _LL342;_tmp772=*_tmp771;_tmp773=
_tmp772.aggr_info;_tmp774=_tmp770->f3;}}_LL341: {struct Cyc_Absyn_Aggrdecl*
_tmp79B=Cyc_Absyn_get_known_aggrdecl(_tmp773);if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp79B->impl))->tagged){struct _tuple23 _tmp79D;struct Cyc_List_List*
_tmp79E;struct Cyc_Absyn_Pat*_tmp79F;struct _tuple23*_tmp79C=(struct _tuple23*)((
struct Cyc_List_List*)_check_null(_tmp774))->hd;_tmp79D=*_tmp79C;_tmp79E=_tmp79D.f1;
_tmp79F=_tmp79D.f2;{struct _dyneither_ptr*f;{void*_tmp7A0=(void*)((struct Cyc_List_List*)
_check_null(_tmp79E))->hd;struct _dyneither_ptr*_tmp7A2;_LL34B: {struct Cyc_Absyn_FieldName_struct*
_tmp7A1=(struct Cyc_Absyn_FieldName_struct*)_tmp7A0;if(_tmp7A1->tag != 1)goto
_LL34D;else{_tmp7A2=_tmp7A1->f1;}}_LL34C: f=_tmp7A2;goto _LL34A;_LL34D:;_LL34E: {
const char*_tmpD0F;void*_tmpD0E;(_tmpD0E=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD0F="no field name in tagged union pattern",
_tag_dyneither(_tmpD0F,sizeof(char),38))),_tag_dyneither(_tmpD0E,sizeof(void*),0)));}
_LL34A:;}{struct _tuple0*_tmp7A5=Cyc_Toc_temp_var();void*_tmp7A6=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp79F->topt))->v;void*_tmp7A7=Cyc_Toc_typ_to_c(
_tmp7A6);{struct _tuple22*_tmpD12;struct Cyc_List_List*_tmpD11;decls=((_tmpD11=
_region_malloc(rgn,sizeof(*_tmpD11)),((_tmpD11->hd=((_tmpD12=_region_malloc(rgn,
sizeof(*_tmpD12)),((_tmpD12->f1=_tmp7A5,((_tmpD12->f2=_tmp7A7,_tmpD12)))))),((
_tmpD11->tl=decls,_tmpD11))))));}{struct Cyc_Absyn_Exp*_tmp7AA=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp7AB=
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp7AB=Cyc_Toc_cast_it(_tmp7A7,_tmp7AB);_tmp7AA=Cyc_Toc_cast_it(_tmp7A7,_tmp7AA);{
struct _tuple21 _tmp7AC=Cyc_Toc_xlate_pat(nv,rgn,_tmp7A6,Cyc_Absyn_var_exp(_tmp7A5,
0),_tmp7AA,_tmp79F,fail_stmt,decls);nv=_tmp7AC.f1;decls=_tmp7AC.f2;{struct Cyc_Absyn_Stmt*
_tmp7AD=_tmp7AC.f3;struct Cyc_Absyn_Stmt*_tmp7AE=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp79B,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp7AF=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp7A5,0),_tmp7AB,0);s=Cyc_Absyn_seq_stmt(_tmp7AE,Cyc_Absyn_seq_stmt(_tmp7AF,
_tmp7AD,0),0);};};};};};}else{s=Cyc_Toc_skip_stmt_dl();for(0;_tmp774 != 0;_tmp774=
_tmp774->tl){struct _tuple23*_tmp7B0=(struct _tuple23*)_tmp774->hd;struct Cyc_Absyn_Pat*
_tmp7B1=(*_tmp7B0).f2;if(_tmp7B1->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _dyneither_ptr*f;{void*_tmp7B2=(void*)((struct Cyc_List_List*)_check_null((*
_tmp7B0).f1))->hd;struct _dyneither_ptr*_tmp7B4;_LL350: {struct Cyc_Absyn_FieldName_struct*
_tmp7B3=(struct Cyc_Absyn_FieldName_struct*)_tmp7B2;if(_tmp7B3->tag != 1)goto
_LL352;else{_tmp7B4=_tmp7B3->f1;}}_LL351: f=_tmp7B4;goto _LL34F;_LL352:;_LL353: {
struct Cyc_Toc_Match_error_struct _tmpD15;struct Cyc_Toc_Match_error_struct*_tmpD14;(
int)_throw((void*)((_tmpD14=_cycalloc_atomic(sizeof(*_tmpD14)),((_tmpD14[0]=((
_tmpD15.tag=Cyc_Toc_Match_error,_tmpD15)),_tmpD14)))));}_LL34F:;}{struct _tuple0*
_tmp7B7=Cyc_Toc_temp_var();void*_tmp7B8=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7B1->topt))->v;void*_tmp7B9=Cyc_Toc_typ_to_c(_tmp7B8);{struct _tuple22*
_tmpD18;struct Cyc_List_List*_tmpD17;decls=((_tmpD17=_region_malloc(rgn,sizeof(*
_tmpD17)),((_tmpD17->hd=((_tmpD18=_region_malloc(rgn,sizeof(*_tmpD18)),((_tmpD18->f1=
_tmp7B7,((_tmpD18->f2=_tmp7B9,_tmpD18)))))),((_tmpD17->tl=decls,_tmpD17))))));}{
struct _tuple21 _tmp7BC=Cyc_Toc_xlate_pat(nv,rgn,_tmp7B8,Cyc_Absyn_var_exp(_tmp7B7,
0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp7B1,fail_stmt,decls);nv=_tmp7BC.f1;
decls=_tmp7BC.f2;{struct Cyc_Absyn_Exp*_tmp7BD=Cyc_Absyn_aggrmember_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp79B->impl))->fields,f)))->type))
_tmp7BD=Cyc_Toc_cast_it(_tmp7B9,_tmp7BD);{struct Cyc_Absyn_Stmt*_tmp7BE=_tmp7BC.f3;
struct Cyc_Absyn_Stmt*_tmp7BF=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7B7,0),
_tmp7BD,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7BF,_tmp7BE,0),0);};};};};};}}
goto _LL323;}_LL342: {struct Cyc_Absyn_Pointer_p_struct*_tmp775=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp746;if(_tmp775->tag != 5)goto _LL344;else{_tmp776=_tmp775->f1;}}_LL343: {
struct _tuple0*_tmp7C0=Cyc_Toc_temp_var();void*_tmp7C1=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp776->topt))->v;{struct _tuple22*_tmpD1B;struct Cyc_List_List*
_tmpD1A;decls=((_tmpD1A=_region_malloc(rgn,sizeof(*_tmpD1A)),((_tmpD1A->hd=((
_tmpD1B=_region_malloc(rgn,sizeof(*_tmpD1B)),((_tmpD1B->f1=_tmp7C0,((_tmpD1B->f2=
Cyc_Toc_typ_to_c(_tmp7C1),_tmpD1B)))))),((_tmpD1A->tl=decls,_tmpD1A))))));}{
struct _tuple21 _tmp7C4=Cyc_Toc_xlate_pat(nv,rgn,_tmp7C1,Cyc_Absyn_var_exp(_tmp7C0,
0),Cyc_Absyn_deref_exp(path,0),_tmp776,fail_stmt,decls);nv=_tmp7C4.f1;decls=
_tmp7C4.f2;{struct Cyc_Absyn_Stmt*_tmp7C5=_tmp7C4.f3;struct Cyc_Absyn_Stmt*_tmp7C6=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7C0,0),Cyc_Absyn_deref_exp(
r,0),0),_tmp7C5,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp7C6,0);else{s=_tmp7C6;}goto _LL323;};};}_LL344: {struct Cyc_Absyn_UnknownId_p_struct*
_tmp777=(struct Cyc_Absyn_UnknownId_p_struct*)_tmp746;if(_tmp777->tag != 14)goto
_LL346;}_LL345: {const char*_tmpD1E;void*_tmpD1D;(_tmpD1D=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD1E="unknownid pat",
_tag_dyneither(_tmpD1E,sizeof(char),14))),_tag_dyneither(_tmpD1D,sizeof(void*),0)));}
_LL346: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp778=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp746;if(_tmp778->tag != 15)goto _LL348;}_LL347: {const char*_tmpD21;void*_tmpD20;(
_tmpD20=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD21="unknowncall pat",_tag_dyneither(_tmpD21,sizeof(char),16))),
_tag_dyneither(_tmpD20,sizeof(void*),0)));}_LL348: {struct Cyc_Absyn_Exp_p_struct*
_tmp779=(struct Cyc_Absyn_Exp_p_struct*)_tmp746;if(_tmp779->tag != 16)goto _LL323;}
_LL349: {const char*_tmpD24;void*_tmpD23;(_tmpD23=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD24="exp pat",
_tag_dyneither(_tmpD24,sizeof(char),8))),_tag_dyneither(_tmpD23,sizeof(void*),0)));}
_LL323:;}{struct _tuple21 _tmpD25;return(_tmpD25.f1=nv,((_tmpD25.f2=decls,((
_tmpD25.f3=s,_tmpD25)))));};}struct _tuple24{struct _dyneither_ptr*f1;struct
_dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){struct _tuple24*_tmpD26;
return(_tmpD26=_region_malloc(r,sizeof(*_tmpD26)),((_tmpD26->f1=Cyc_Toc_fresh_label(),((
_tmpD26->f2=Cyc_Toc_fresh_label(),((_tmpD26->f3=sc,_tmpD26)))))));}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*_tmp7CF=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;int leave_as_switch;{void*_tmp7D0=Cyc_Tcutil_compress(
_tmp7CF);_LL355: {struct Cyc_Absyn_IntType_struct*_tmp7D1=(struct Cyc_Absyn_IntType_struct*)
_tmp7D0;if(_tmp7D1->tag != 6)goto _LL357;}_LL356: goto _LL358;_LL357: {struct Cyc_Absyn_EnumType_struct*
_tmp7D2=(struct Cyc_Absyn_EnumType_struct*)_tmp7D0;if(_tmp7D2->tag != 14)goto
_LL359;}_LL358: leave_as_switch=1;goto _LL354;_LL359:;_LL35A: leave_as_switch=0;
goto _LL354;_LL354:;}{struct Cyc_List_List*_tmp7D3=scs;for(0;_tmp7D3 != 0;_tmp7D3=
_tmp7D3->tl){if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Switch_clause*)_tmp7D3->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)
_tmp7D3->hd)->where_clause != 0){leave_as_switch=0;break;}}}if(leave_as_switch){
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{struct Cyc_List_List*_tmp7D4=
scs;for(0;_tmp7D4 != 0;_tmp7D4=_tmp7D4->tl){struct Cyc_Absyn_Stmt*_tmp7D5=((struct
Cyc_Absyn_Switch_clause*)_tmp7D4->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp7D4->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp7D5,0);next_l=Cyc_Toc_fresh_label();{
struct Cyc_Toc_Env _tmp7D7;struct _RegionHandle*_tmp7D8;struct Cyc_Toc_Env*_tmp7D6=
nv;_tmp7D7=*_tmp7D6;_tmp7D8=_tmp7D7.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
_tmp7D8,nv,next_l),_tmp7D5);};}}return;}{struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp7DA;
struct _RegionHandle*_tmp7DB;struct Cyc_Toc_Env*_tmp7D9=nv;_tmp7DA=*_tmp7D9;
_tmp7DB=_tmp7DA.rgn;{struct Cyc_Toc_Env*_tmp7DC=Cyc_Toc_share_env(_tmp7DB,nv);
struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple24*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp7DB,Cyc_Toc_gen_label,
_tmp7DB,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct
Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp7DD=lscs;for(0;_tmp7DD != 0;
_tmp7DD=_tmp7DD->tl){struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple24*)
_tmp7DD->hd)).f3;struct _dyneither_ptr*fail_lab=_tmp7DD->tl == 0?end_l:(*((struct
_tuple24*)((struct Cyc_List_List*)_check_null(_tmp7DD->tl))->hd)).f1;struct Cyc_Toc_Env*
_tmp7DF;struct Cyc_List_List*_tmp7E0;struct Cyc_Absyn_Stmt*_tmp7E1;struct _tuple21
_tmp7DE=Cyc_Toc_xlate_pat(_tmp7DC,_tmp7DB,_tmp7CF,r,path,sc->pattern,Cyc_Absyn_goto_stmt(
fail_lab,0),decls);_tmp7DF=_tmp7DE.f1;_tmp7E0=_tmp7DE.f2;_tmp7E1=_tmp7DE.f3;if(
sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp7E2=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp7DF,_tmp7E2);_tmp7E1=Cyc_Absyn_seq_stmt(
_tmp7E1,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp7E2,0),
Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp7E0;{
struct Cyc_List_List*_tmpD27;nvs=((_tmpD27=_region_malloc(_tmp7DB,sizeof(*_tmpD27)),((
_tmpD27->hd=_tmp7DF,((_tmpD27->tl=nvs,_tmpD27))))));}{struct Cyc_List_List*
_tmpD28;test_stmts=((_tmpD28=_region_malloc(_tmp7DB,sizeof(*_tmpD28)),((_tmpD28->hd=
_tmp7E1,((_tmpD28->tl=test_stmts,_tmpD28))))));};}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple24 _tmp7E6;struct _dyneither_ptr*_tmp7E7;struct _dyneither_ptr*_tmp7E8;
struct Cyc_Absyn_Switch_clause*_tmp7E9;struct _tuple24*_tmp7E5=(struct _tuple24*)
lscs->hd;_tmp7E6=*_tmp7E5;_tmp7E7=_tmp7E6.f1;_tmp7E8=_tmp7E6.f2;_tmp7E9=_tmp7E6.f3;{
struct Cyc_Toc_Env*_tmp7EA=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp7E9->body;struct Cyc_Toc_Env _tmp7EC;struct
_RegionHandle*_tmp7ED;struct Cyc_Toc_Env*_tmp7EB=_tmp7DC;_tmp7EC=*_tmp7EB;_tmp7ED=
_tmp7EC.rgn;if(lscs->tl != 0){struct _tuple24 _tmp7EF;struct _dyneither_ptr*_tmp7F0;
struct Cyc_Absyn_Switch_clause*_tmp7F1;struct _tuple24*_tmp7EE=(struct _tuple24*)((
struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp7EF=*_tmp7EE;_tmp7F0=_tmp7EF.f2;
_tmp7F1=_tmp7EF.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp7ED,
_tmp7EA,end_l,_tmp7F0,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp7F1->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp7ED,
_tmp7EA,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp7E7,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp7E8,s,0),0);{struct Cyc_List_List*_tmpD29;stmts=((_tmpD29=_region_malloc(
_tmp7DB,sizeof(*_tmpD29)),((_tmpD29->hd=s,((_tmpD29->tl=stmts,_tmpD29))))));};};}{
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,
Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple22
_tmp7F4;struct _tuple0*_tmp7F5;void*_tmp7F6;struct _tuple22*_tmp7F3=(struct
_tuple22*)decls->hd;_tmp7F4=*_tmp7F3;_tmp7F5=_tmp7F4.f1;_tmp7F6=_tmp7F4.f2;res=
Cyc_Absyn_declare_stmt(_tmp7F5,_tmp7F6,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(
v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(
struct Cyc_Absyn_Exp*)e,res,0))->r;};};};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){struct Cyc_List_List*
_tmpD2A;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,((
_tmpD2A=_cycalloc(sizeof(*_tmpD2A)),((_tmpD2A->hd=Cyc_Absyn_uint_exp((
unsigned int)(n - 1),0),((_tmpD2A->tl=0,_tmpD2A)))))),0),0);}void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s){while(1){void*_tmp7F8=s->r;struct Cyc_Absyn_Exp*_tmp7FB;
struct Cyc_Absyn_Stmt*_tmp7FD;struct Cyc_Absyn_Stmt*_tmp7FE;struct Cyc_Absyn_Exp*
_tmp800;struct Cyc_Absyn_Exp*_tmp802;struct Cyc_Absyn_Stmt*_tmp803;struct Cyc_Absyn_Stmt*
_tmp804;struct _tuple8 _tmp806;struct Cyc_Absyn_Exp*_tmp807;struct Cyc_Absyn_Stmt*
_tmp808;struct Cyc_Absyn_Stmt*_tmp80A;struct Cyc_Absyn_Stmt*_tmp80C;struct Cyc_Absyn_Stmt*
_tmp80E;struct Cyc_Absyn_Exp*_tmp810;struct _tuple8 _tmp811;struct Cyc_Absyn_Exp*
_tmp812;struct _tuple8 _tmp813;struct Cyc_Absyn_Exp*_tmp814;struct Cyc_Absyn_Stmt*
_tmp815;struct Cyc_Absyn_Exp*_tmp817;struct Cyc_List_List*_tmp818;struct Cyc_List_List*
_tmp81A;struct Cyc_Absyn_Switch_clause**_tmp81B;struct Cyc_Absyn_Decl*_tmp81D;
struct Cyc_Absyn_Stmt*_tmp81E;struct _dyneither_ptr*_tmp820;struct Cyc_Absyn_Stmt*
_tmp821;struct Cyc_Absyn_Stmt*_tmp823;struct _tuple8 _tmp824;struct Cyc_Absyn_Exp*
_tmp825;struct Cyc_Absyn_Stmt*_tmp827;struct Cyc_List_List*_tmp828;struct Cyc_Absyn_Exp*
_tmp82A;_LL35C: {struct Cyc_Absyn_Skip_s_struct*_tmp7F9=(struct Cyc_Absyn_Skip_s_struct*)
_tmp7F8;if(_tmp7F9->tag != 0)goto _LL35E;}_LL35D: return;_LL35E: {struct Cyc_Absyn_Exp_s_struct*
_tmp7FA=(struct Cyc_Absyn_Exp_s_struct*)_tmp7F8;if(_tmp7FA->tag != 1)goto _LL360;
else{_tmp7FB=_tmp7FA->f1;}}_LL35F: Cyc_Toc_exp_to_c(nv,_tmp7FB);return;_LL360: {
struct Cyc_Absyn_Seq_s_struct*_tmp7FC=(struct Cyc_Absyn_Seq_s_struct*)_tmp7F8;if(
_tmp7FC->tag != 2)goto _LL362;else{_tmp7FD=_tmp7FC->f1;_tmp7FE=_tmp7FC->f2;}}
_LL361: Cyc_Toc_stmt_to_c(nv,_tmp7FD);s=_tmp7FE;continue;_LL362: {struct Cyc_Absyn_Return_s_struct*
_tmp7FF=(struct Cyc_Absyn_Return_s_struct*)_tmp7F8;if(_tmp7FF->tag != 3)goto _LL364;
else{_tmp800=_tmp7FF->f1;}}_LL363: {struct Cyc_Core_Opt*topt=0;if(_tmp800 != 0){{
struct Cyc_Core_Opt*_tmpD2B;topt=((_tmpD2B=_cycalloc(sizeof(*_tmpD2B)),((_tmpD2B->v=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp800->topt))->v),
_tmpD2B))));}Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp800);}if(s->try_depth
> 0){if(topt != 0){struct _tuple0*_tmp82C=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*
_tmp82D=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp82C,0),
0);s->r=(Cyc_Absyn_declare_stmt(_tmp82C,(void*)topt->v,_tmp800,Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler(s->try_depth),_tmp82D,0),0))->r;}else{Cyc_Toc_do_npop_before(
s->try_depth,s);}}return;}_LL364: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp801=(
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp7F8;if(_tmp801->tag != 4)goto _LL366;
else{_tmp802=_tmp801->f1;_tmp803=_tmp801->f2;_tmp804=_tmp801->f3;}}_LL365: Cyc_Toc_exp_to_c(
nv,_tmp802);Cyc_Toc_stmt_to_c(nv,_tmp803);s=_tmp804;continue;_LL366: {struct Cyc_Absyn_While_s_struct*
_tmp805=(struct Cyc_Absyn_While_s_struct*)_tmp7F8;if(_tmp805->tag != 5)goto _LL368;
else{_tmp806=_tmp805->f1;_tmp807=_tmp806.f1;_tmp808=_tmp805->f2;}}_LL367: Cyc_Toc_exp_to_c(
nv,_tmp807);{struct Cyc_Toc_Env _tmp82F;struct _RegionHandle*_tmp830;struct Cyc_Toc_Env*
_tmp82E=nv;_tmp82F=*_tmp82E;_tmp830=_tmp82F.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp830,nv),_tmp808);return;};_LL368: {struct Cyc_Absyn_Break_s_struct*_tmp809=(
struct Cyc_Absyn_Break_s_struct*)_tmp7F8;if(_tmp809->tag != 6)goto _LL36A;else{
_tmp80A=_tmp809->f1;}}_LL369: {struct Cyc_Toc_Env _tmp832;struct _dyneither_ptr**
_tmp833;struct Cyc_Toc_Env*_tmp831=nv;_tmp832=*_tmp831;_tmp833=_tmp832.break_lab;
if(_tmp833 != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp833,0);{int dest_depth=_tmp80A == 0?0:
_tmp80A->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;};}
_LL36A: {struct Cyc_Absyn_Continue_s_struct*_tmp80B=(struct Cyc_Absyn_Continue_s_struct*)
_tmp7F8;if(_tmp80B->tag != 7)goto _LL36C;else{_tmp80C=_tmp80B->f1;}}_LL36B: {
struct Cyc_Toc_Env _tmp835;struct _dyneither_ptr**_tmp836;struct Cyc_Toc_Env*_tmp834=
nv;_tmp835=*_tmp834;_tmp836=_tmp835.continue_lab;if(_tmp836 != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp836,0);_tmp80E=_tmp80C;goto _LL36D;}_LL36C: {struct Cyc_Absyn_Goto_s_struct*
_tmp80D=(struct Cyc_Absyn_Goto_s_struct*)_tmp7F8;if(_tmp80D->tag != 8)goto _LL36E;
else{_tmp80E=_tmp80D->f2;}}_LL36D: Cyc_Toc_do_npop_before(s->try_depth - ((struct
Cyc_Absyn_Stmt*)_check_null(_tmp80E))->try_depth,s);return;_LL36E: {struct Cyc_Absyn_For_s_struct*
_tmp80F=(struct Cyc_Absyn_For_s_struct*)_tmp7F8;if(_tmp80F->tag != 9)goto _LL370;
else{_tmp810=_tmp80F->f1;_tmp811=_tmp80F->f2;_tmp812=_tmp811.f1;_tmp813=_tmp80F->f3;
_tmp814=_tmp813.f1;_tmp815=_tmp80F->f4;}}_LL36F: Cyc_Toc_exp_to_c(nv,_tmp810);Cyc_Toc_exp_to_c(
nv,_tmp812);Cyc_Toc_exp_to_c(nv,_tmp814);{struct Cyc_Toc_Env _tmp838;struct
_RegionHandle*_tmp839;struct Cyc_Toc_Env*_tmp837=nv;_tmp838=*_tmp837;_tmp839=
_tmp838.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp839,nv),_tmp815);return;};
_LL370: {struct Cyc_Absyn_Switch_s_struct*_tmp816=(struct Cyc_Absyn_Switch_s_struct*)
_tmp7F8;if(_tmp816->tag != 10)goto _LL372;else{_tmp817=_tmp816->f1;_tmp818=_tmp816->f2;}}
_LL371: Cyc_Toc_xlate_switch(nv,s,_tmp817,_tmp818);return;_LL372: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp819=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp7F8;if(_tmp819->tag != 11)goto
_LL374;else{_tmp81A=_tmp819->f1;_tmp81B=_tmp819->f2;}}_LL373: {struct Cyc_Toc_Env
_tmp83B;struct Cyc_Toc_FallthruInfo*_tmp83C;struct Cyc_Toc_Env*_tmp83A=nv;_tmp83B=*
_tmp83A;_tmp83C=_tmp83B.fallthru_info;if(_tmp83C == 0){const char*_tmpD2E;void*
_tmpD2D;(_tmpD2D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD2E="fallthru in unexpected place",_tag_dyneither(_tmpD2E,sizeof(char),29))),
_tag_dyneither(_tmpD2D,sizeof(void*),0)));}{struct _dyneither_ptr*_tmp840;struct
Cyc_List_List*_tmp841;struct Cyc_Dict_Dict _tmp842;struct Cyc_Toc_FallthruInfo
_tmp83F=*_tmp83C;_tmp840=_tmp83F.label;_tmp841=_tmp83F.binders;_tmp842=_tmp83F.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp840,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp81B)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp843=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp841);struct Cyc_List_List*_tmp844=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp81A);for(0;_tmp843 != 0;(_tmp843=_tmp843->tl,
_tmp844=_tmp844->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp844))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp842,(
struct _tuple0*)_tmp843->hd),(struct Cyc_Absyn_Exp*)_tmp844->hd,0),s2,0);}s->r=s2->r;
return;};};};}_LL374: {struct Cyc_Absyn_Decl_s_struct*_tmp81C=(struct Cyc_Absyn_Decl_s_struct*)
_tmp7F8;if(_tmp81C->tag != 12)goto _LL376;else{_tmp81D=_tmp81C->f1;_tmp81E=_tmp81C->f2;}}
_LL375:{void*_tmp845=_tmp81D->r;struct Cyc_Absyn_Vardecl*_tmp847;struct Cyc_Absyn_Pat*
_tmp849;struct Cyc_Absyn_Exp*_tmp84A;struct Cyc_List_List*_tmp84C;struct Cyc_Absyn_Fndecl*
_tmp84E;struct Cyc_Absyn_Tvar*_tmp850;struct Cyc_Absyn_Vardecl*_tmp851;int _tmp852;
struct Cyc_Absyn_Exp*_tmp853;struct Cyc_Absyn_Exp*_tmp855;struct Cyc_Absyn_Tvar*
_tmp856;struct Cyc_Absyn_Vardecl*_tmp857;_LL37F: {struct Cyc_Absyn_Var_d_struct*
_tmp846=(struct Cyc_Absyn_Var_d_struct*)_tmp845;if(_tmp846->tag != 0)goto _LL381;
else{_tmp847=_tmp846->f1;}}_LL380: {struct Cyc_Toc_Env _tmp859;struct _RegionHandle*
_tmp85A;struct Cyc_Toc_Env*_tmp858=nv;_tmp859=*_tmp858;_tmp85A=_tmp859.rgn;{
struct Cyc_Absyn_Local_b_struct _tmpD31;struct Cyc_Absyn_Local_b_struct*_tmpD30;
struct Cyc_Toc_Env*_tmp85B=Cyc_Toc_add_varmap(_tmp85A,nv,_tmp847->name,Cyc_Absyn_varb_exp(
_tmp847->name,(void*)((_tmpD30=_cycalloc(sizeof(*_tmpD30)),((_tmpD30[0]=((
_tmpD31.tag=4,((_tmpD31.f1=_tmp847,_tmpD31)))),_tmpD30)))),0));Cyc_Toc_local_decl_to_c(
_tmp85B,_tmp85B,_tmp847,_tmp81E);}goto _LL37E;}_LL381: {struct Cyc_Absyn_Let_d_struct*
_tmp848=(struct Cyc_Absyn_Let_d_struct*)_tmp845;if(_tmp848->tag != 2)goto _LL383;
else{_tmp849=_tmp848->f1;_tmp84A=_tmp848->f3;}}_LL382:{void*_tmp85E=_tmp849->r;
struct Cyc_Absyn_Vardecl*_tmp860;struct Cyc_Absyn_Pat*_tmp861;struct Cyc_Absyn_Pat
_tmp862;void*_tmp863;_LL38E: {struct Cyc_Absyn_Var_p_struct*_tmp85F=(struct Cyc_Absyn_Var_p_struct*)
_tmp85E;if(_tmp85F->tag != 1)goto _LL390;else{_tmp860=_tmp85F->f1;_tmp861=_tmp85F->f2;
_tmp862=*_tmp861;_tmp863=_tmp862.r;{struct Cyc_Absyn_Wild_p_struct*_tmp864=(
struct Cyc_Absyn_Wild_p_struct*)_tmp863;if(_tmp864->tag != 0)goto _LL390;};}}_LL38F: {
struct _tuple0*old_name=_tmp860->name;struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp860->name=new_name;_tmp860->initializer=(struct Cyc_Absyn_Exp*)_tmp84A;{
struct Cyc_Absyn_Var_d_struct _tmpD34;struct Cyc_Absyn_Var_d_struct*_tmpD33;_tmp81D->r=(
void*)((_tmpD33=_cycalloc(sizeof(*_tmpD33)),((_tmpD33[0]=((_tmpD34.tag=0,((
_tmpD34.f1=_tmp860,_tmpD34)))),_tmpD33))));}{struct Cyc_Toc_Env _tmp868;struct
_RegionHandle*_tmp869;struct Cyc_Toc_Env*_tmp867=nv;_tmp868=*_tmp867;_tmp869=
_tmp868.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD37;struct Cyc_Absyn_Local_b_struct*
_tmpD36;struct Cyc_Toc_Env*_tmp86A=Cyc_Toc_add_varmap(_tmp869,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD36=_cycalloc(sizeof(*_tmpD36)),((_tmpD36[0]=((_tmpD37.tag=
4,((_tmpD37.f1=_tmp860,_tmpD37)))),_tmpD36)))),0));Cyc_Toc_local_decl_to_c(
_tmp86A,nv,_tmp860,_tmp81E);}goto _LL38D;};}_LL390:;_LL391: s->r=(Cyc_Toc_letdecl_to_c(
nv,_tmp849,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp84A->topt))->v,_tmp84A,
_tmp81E))->r;goto _LL38D;_LL38D:;}goto _LL37E;_LL383: {struct Cyc_Absyn_Letv_d_struct*
_tmp84B=(struct Cyc_Absyn_Letv_d_struct*)_tmp845;if(_tmp84B->tag != 3)goto _LL385;
else{_tmp84C=_tmp84B->f1;}}_LL384: {struct Cyc_List_List*_tmp86D=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp84C);if(_tmp86D == 0){const char*_tmpD3A;
void*_tmpD39;(_tmpD39=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpD3A="empty Letv_d",_tag_dyneither(_tmpD3A,sizeof(char),
13))),_tag_dyneither(_tmpD39,sizeof(void*),0)));}{struct Cyc_Absyn_Var_d_struct
_tmpD3D;struct Cyc_Absyn_Var_d_struct*_tmpD3C;_tmp81D->r=(void*)((_tmpD3C=
_cycalloc(sizeof(*_tmpD3C)),((_tmpD3C[0]=((_tmpD3D.tag=0,((_tmpD3D.f1=(struct Cyc_Absyn_Vardecl*)
_tmp86D->hd,_tmpD3D)))),_tmpD3C))));}_tmp86D=_tmp86D->tl;for(0;_tmp86D != 0;
_tmp86D=_tmp86D->tl){struct Cyc_Absyn_Var_d_struct _tmpD40;struct Cyc_Absyn_Var_d_struct*
_tmpD3F;struct Cyc_Absyn_Decl*_tmp872=Cyc_Absyn_new_decl((void*)((_tmpD3F=
_cycalloc(sizeof(*_tmpD3F)),((_tmpD3F[0]=((_tmpD40.tag=0,((_tmpD40.f1=(struct Cyc_Absyn_Vardecl*)
_tmp86D->hd,_tmpD40)))),_tmpD3F)))),0);s->r=(Cyc_Absyn_decl_stmt(_tmp872,Cyc_Absyn_new_stmt(
s->r,0),0))->r;}Cyc_Toc_stmt_to_c(nv,s);goto _LL37E;}_LL385: {struct Cyc_Absyn_Fn_d_struct*
_tmp84D=(struct Cyc_Absyn_Fn_d_struct*)_tmp845;if(_tmp84D->tag != 1)goto _LL387;
else{_tmp84E=_tmp84D->f1;}}_LL386: {struct _tuple0*_tmp875=_tmp84E->name;struct
Cyc_Toc_Env _tmp877;struct _RegionHandle*_tmp878;struct Cyc_Toc_Env*_tmp876=nv;
_tmp877=*_tmp876;_tmp878=_tmp877.rgn;{struct Cyc_Toc_Env*_tmp879=Cyc_Toc_add_varmap(
_tmp878,nv,_tmp84E->name,Cyc_Absyn_var_exp(_tmp875,0));Cyc_Toc_fndecl_to_c(
_tmp879,_tmp84E,0);Cyc_Toc_stmt_to_c(_tmp879,_tmp81E);}goto _LL37E;}_LL387: {
struct Cyc_Absyn_Region_d_struct*_tmp84F=(struct Cyc_Absyn_Region_d_struct*)
_tmp845;if(_tmp84F->tag != 4)goto _LL389;else{_tmp850=_tmp84F->f1;_tmp851=_tmp84F->f2;
_tmp852=_tmp84F->f3;_tmp853=_tmp84F->f4;}}_LL388: {struct Cyc_Absyn_Stmt*_tmp87A=
_tmp81E;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple0*
rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp851->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp87C;struct _RegionHandle*_tmp87D;struct Cyc_Toc_Env*
_tmp87B=nv;_tmp87C=*_tmp87B;_tmp87D=_tmp87C.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp87D,nv,x_var,x_exp),_tmp87A);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp87A,0))->r;
else{if(_tmp853 == 0){struct Cyc_Absyn_Exp*_tmpD43[1];struct Cyc_Absyn_Exp*_tmpD42[
1];struct Cyc_List_List*_tmpD41;s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpD41=
_cycalloc(sizeof(*_tmpD41)),((_tmpD41->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0),((_tmpD41->tl=0,_tmpD41)))))),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD42[0]=
x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD42,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp87A,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD43[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD43,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp853);{struct Cyc_Absyn_Exp*
_tmpD45[1];struct Cyc_Absyn_Exp*_tmpD44[2];s->r=(Cyc_Absyn_declare_stmt(rh_var,
Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpD44[1]=(struct Cyc_Absyn_Exp*)_tmp853,((_tmpD44[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD44,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp87A,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpD45[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD45,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
return;}_LL389: {struct Cyc_Absyn_Alias_d_struct*_tmp854=(struct Cyc_Absyn_Alias_d_struct*)
_tmp845;if(_tmp854->tag != 5)goto _LL38B;else{_tmp855=_tmp854->f1;_tmp856=_tmp854->f2;
_tmp857=_tmp854->f3;}}_LL38A: {struct _tuple0*old_name=_tmp857->name;struct
_tuple0*new_name=Cyc_Toc_temp_var();_tmp857->name=new_name;_tmp857->initializer=(
struct Cyc_Absyn_Exp*)_tmp855;{struct Cyc_Absyn_Decl_s_struct _tmpD54;struct Cyc_Absyn_Var_d_struct*
_tmpD53;struct Cyc_Absyn_Var_d_struct _tmpD52;struct Cyc_Absyn_Decl*_tmpD51;struct
Cyc_Absyn_Decl_s_struct*_tmpD50;s->r=(void*)((_tmpD50=_cycalloc(sizeof(*_tmpD50)),((
_tmpD50[0]=((_tmpD54.tag=12,((_tmpD54.f1=((_tmpD51=_cycalloc(sizeof(*_tmpD51)),((
_tmpD51->r=(void*)((_tmpD53=_cycalloc(sizeof(*_tmpD53)),((_tmpD53[0]=((_tmpD52.tag=
0,((_tmpD52.f1=_tmp857,_tmpD52)))),_tmpD53)))),((_tmpD51->loc=0,_tmpD51)))))),((
_tmpD54.f2=_tmp81E,_tmpD54)))))),_tmpD50))));}{struct Cyc_Toc_Env _tmp889;struct
_RegionHandle*_tmp88A;struct Cyc_Toc_Env*_tmp888=nv;_tmp889=*_tmp888;_tmp88A=
_tmp889.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD57;struct Cyc_Absyn_Local_b_struct*
_tmpD56;struct Cyc_Toc_Env*_tmp88B=Cyc_Toc_add_varmap(_tmp88A,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD56=_cycalloc(sizeof(*_tmpD56)),((_tmpD56[0]=((_tmpD57.tag=
4,((_tmpD57.f1=_tmp857,_tmpD57)))),_tmpD56)))),0));Cyc_Toc_local_decl_to_c(
_tmp88B,nv,_tmp857,_tmp81E);}return;};}_LL38B:;_LL38C: {const char*_tmpD5A;void*
_tmpD59;(_tmpD59=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD5A="bad nested declaration within function",_tag_dyneither(_tmpD5A,sizeof(
char),39))),_tag_dyneither(_tmpD59,sizeof(void*),0)));}_LL37E:;}return;_LL376: {
struct Cyc_Absyn_Label_s_struct*_tmp81F=(struct Cyc_Absyn_Label_s_struct*)_tmp7F8;
if(_tmp81F->tag != 13)goto _LL378;else{_tmp820=_tmp81F->f1;_tmp821=_tmp81F->f2;}}
_LL377: s=_tmp821;continue;_LL378: {struct Cyc_Absyn_Do_s_struct*_tmp822=(struct
Cyc_Absyn_Do_s_struct*)_tmp7F8;if(_tmp822->tag != 14)goto _LL37A;else{_tmp823=
_tmp822->f1;_tmp824=_tmp822->f2;_tmp825=_tmp824.f1;}}_LL379: {struct Cyc_Toc_Env
_tmp891;struct _RegionHandle*_tmp892;struct Cyc_Toc_Env*_tmp890=nv;_tmp891=*
_tmp890;_tmp892=_tmp891.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp892,nv),
_tmp823);Cyc_Toc_exp_to_c(nv,_tmp825);return;}_LL37A: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp826=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp7F8;if(_tmp826->tag != 15)goto
_LL37C;else{_tmp827=_tmp826->f1;_tmp828=_tmp826->f2;}}_LL37B: {struct _tuple0*
h_var=Cyc_Toc_temp_var();struct _tuple0*e_var=Cyc_Toc_temp_var();struct _tuple0*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);{struct Cyc_Core_Opt*_tmpD5B;
e_exp->topt=((_tmpD5B=_cycalloc(sizeof(*_tmpD5B)),((_tmpD5B->v=e_typ,_tmpD5B))));}{
struct Cyc_Toc_Env _tmp895;struct _RegionHandle*_tmp896;struct Cyc_Toc_Env*_tmp894=
nv;_tmp895=*_tmp894;_tmp896=_tmp895.rgn;{struct Cyc_Toc_Env*_tmp897=Cyc_Toc_add_varmap(
_tmp896,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp897,_tmp827);{struct Cyc_Absyn_Stmt*
_tmp898=Cyc_Absyn_seq_stmt(_tmp827,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple0*_tmp899=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp89A=
Cyc_Absyn_var_exp(_tmp899,0);struct Cyc_Absyn_Vardecl*_tmp89B=Cyc_Absyn_new_vardecl(
_tmp899,Cyc_Absyn_exn_typ(),0);{struct Cyc_Core_Opt*_tmpD5C;_tmp89A->topt=((
_tmpD5C=_cycalloc(sizeof(*_tmpD5C)),((_tmpD5C->v=Cyc_Absyn_exn_typ(),_tmpD5C))));}{
struct Cyc_Core_Opt*_tmpD6D;struct Cyc_Absyn_Var_p_struct*_tmpD6C;struct Cyc_Absyn_Pat*
_tmpD6B;struct Cyc_Core_Opt*_tmpD6A;struct Cyc_Absyn_Var_p_struct _tmpD69;struct Cyc_Absyn_Pat*
_tmpD68;struct Cyc_Absyn_Pat*_tmp89D=(_tmpD68=_cycalloc(sizeof(*_tmpD68)),((
_tmpD68->r=(void*)((_tmpD6C=_cycalloc(sizeof(*_tmpD6C)),((_tmpD6C[0]=((_tmpD69.tag=
1,((_tmpD69.f1=_tmp89B,((_tmpD69.f2=((_tmpD6B=_cycalloc(sizeof(*_tmpD6B)),((
_tmpD6B->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpD6B->topt=((_tmpD6A=_cycalloc(
sizeof(*_tmpD6A)),((_tmpD6A->v=Cyc_Absyn_exn_typ(),_tmpD6A)))),((_tmpD6B->loc=0,
_tmpD6B)))))))),_tmpD69)))))),_tmpD6C)))),((_tmpD68->topt=((_tmpD6D=_cycalloc(
sizeof(*_tmpD6D)),((_tmpD6D->v=Cyc_Absyn_exn_typ(),_tmpD6D)))),((_tmpD68->loc=0,
_tmpD68)))))));struct Cyc_Absyn_Exp*_tmp89E=Cyc_Absyn_throw_exp(_tmp89A,0);{
struct Cyc_Core_Opt*_tmpD6E;_tmp89E->topt=((_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((
_tmpD6E->v=(void*)& Cyc_Absyn_VoidType_val,_tmpD6E))));}{struct Cyc_Absyn_Stmt*
_tmp8A0=Cyc_Absyn_exp_stmt(_tmp89E,0);struct Cyc_Core_Opt*_tmpD74;struct Cyc_List_List*
_tmpD73;struct Cyc_Absyn_Switch_clause*_tmpD72;struct Cyc_Absyn_Switch_clause*
_tmp8A1=(_tmpD72=_cycalloc(sizeof(*_tmpD72)),((_tmpD72->pattern=_tmp89D,((
_tmpD72->pat_vars=((_tmpD74=_cycalloc(sizeof(*_tmpD74)),((_tmpD74->v=((_tmpD73=
_cycalloc(sizeof(*_tmpD73)),((_tmpD73->hd=_tmp89B,((_tmpD73->tl=0,_tmpD73)))))),
_tmpD74)))),((_tmpD72->where_clause=0,((_tmpD72->body=_tmp8A0,((_tmpD72->loc=0,
_tmpD72)))))))))));struct Cyc_List_List*_tmpD75;struct Cyc_Absyn_Stmt*_tmp8A2=Cyc_Absyn_switch_stmt(
e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp828,((_tmpD75=_cycalloc(sizeof(*_tmpD75)),((_tmpD75->hd=_tmp8A1,((_tmpD75->tl=
0,_tmpD75))))))),0);Cyc_Toc_stmt_to_c(_tmp897,_tmp8A2);{struct Cyc_List_List*
_tmpD76;struct Cyc_Absyn_Exp*_tmp8A3=Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,((
_tmpD76=_cycalloc(sizeof(*_tmpD76)),((_tmpD76->hd=Cyc_Absyn_aggrmember_exp(h_exp,
Cyc_Toc_handler_sp,0),((_tmpD76->tl=0,_tmpD76)))))),0);struct Cyc_List_List*
_tmpD77;struct Cyc_Absyn_Stmt*_tmp8A4=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpD77=_cycalloc(sizeof(*_tmpD77)),((_tmpD77->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpD77->tl=0,_tmpD77)))))),0),0);struct Cyc_Absyn_Exp*_tmp8A5=Cyc_Absyn_int_exp(
Cyc_Absyn_Signed,0,0);struct Cyc_Absyn_Exp*_tmp8A6=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
1,0);s->r=(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp8A4,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp8A5,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp8A3,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp8A6,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp898,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp8A2,0),0),0),0),0),0))->r;};};};};}
return;};}_LL37C: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp829=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp7F8;if(_tmp829->tag != 16)goto _LL35B;else{_tmp82A=_tmp829->f1;}}_LL37D: if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp82A);{struct Cyc_List_List*
_tmpD78;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((
_tmpD78=_cycalloc(sizeof(*_tmpD78)),((_tmpD78->hd=_tmp82A,((_tmpD78->tl=0,
_tmpD78)))))),0));};}return;_LL35B:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);struct _tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8B4=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8B4;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp8B5=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp8B6=f->args;for(0;_tmp8B6 != 0;_tmp8B6=_tmp8B6->tl){struct
_tuple0*_tmpD79;struct _tuple0*_tmp8B7=(_tmpD79=_cycalloc(sizeof(*_tmpD79)),((
_tmpD79->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpD79->f2=(*((struct
_tuple25*)_tmp8B6->hd)).f1,_tmpD79)))));(*((struct _tuple25*)_tmp8B6->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple25*)_tmp8B6->hd)).f3);_tmp8B5=Cyc_Toc_add_varmap(frgn,_tmp8B5,
_tmp8B7,Cyc_Absyn_var_exp(_tmp8B7,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp8BA;struct Cyc_Absyn_Tqual
_tmp8BB;void*_tmp8BC;int _tmp8BD;struct Cyc_Absyn_VarargInfo _tmp8B9=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp8BA=_tmp8B9.name;_tmp8BB=_tmp8B9.tq;_tmp8BC=
_tmp8B9.type;_tmp8BD=_tmp8B9.inject;{void*_tmp8BE=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp8BC,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8BB,Cyc_Absyn_false_conref));struct
_tuple0*_tmpD7A;struct _tuple0*_tmp8BF=(_tmpD7A=_cycalloc(sizeof(*_tmpD7A)),((
_tmpD7A->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpD7A->f2=(struct
_dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null(_tmp8BA))->v,_tmpD7A)))));{
struct _tuple25*_tmpD7D;struct Cyc_List_List*_tmpD7C;f->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpD7C=
_cycalloc(sizeof(*_tmpD7C)),((_tmpD7C->hd=((_tmpD7D=_cycalloc(sizeof(*_tmpD7D)),((
_tmpD7D->f1=(struct _dyneither_ptr*)_tmp8BA->v,((_tmpD7D->f2=_tmp8BB,((_tmpD7D->f3=
_tmp8BE,_tmpD7D)))))))),((_tmpD7C->tl=0,_tmpD7C)))))));}_tmp8B5=Cyc_Toc_add_varmap(
frgn,_tmp8B5,_tmp8BF,Cyc_Absyn_var_exp(_tmp8BF,0));f->cyc_varargs=0;};}{struct
Cyc_List_List*_tmp8C3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp8C3 != 0;_tmp8C3=_tmp8C3->tl){((struct Cyc_Absyn_Vardecl*)_tmp8C3->hd)->type=
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp8C3->hd)->type);}}Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel(frgn,_tmp8B5),f->body);};_pop_region(frgn);};}static enum 
Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){switch(s){case Cyc_Absyn_Abstract:
_LL392: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL393: return Cyc_Absyn_Extern;
default: _LL394: return s;}}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad){struct _tuple0*_tmp8C4=ad->name;struct _DynRegionHandle*_tmp8C6;struct Cyc_Dict_Dict*
_tmp8C7;struct Cyc_Toc_TocState _tmp8C5=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp8C6=_tmp8C5.dyn;_tmp8C7=_tmp8C5.aggrs_so_far;{struct _DynRegionFrame _tmp8C8;
struct _RegionHandle*d=_open_dynregion(& _tmp8C8,_tmp8C6);{int seen_defn_before;
struct _tuple14**_tmp8C9=((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*
k))Cyc_Dict_lookup_opt)(*_tmp8C7,_tmp8C4);if(_tmp8C9 == 0){seen_defn_before=0;{
struct _tuple14*v;if(ad->kind == Cyc_Absyn_StructA){struct _tuple14*_tmpD7E;v=((
_tmpD7E=_region_malloc(d,sizeof(*_tmpD7E)),((_tmpD7E->f1=ad,((_tmpD7E->f2=Cyc_Absyn_strctq(
_tmp8C4),_tmpD7E))))));}else{struct _tuple14*_tmpD7F;v=((_tmpD7F=_region_malloc(d,
sizeof(*_tmpD7F)),((_tmpD7F->f1=ad,((_tmpD7F->f2=Cyc_Absyn_unionq_typ(_tmp8C4),
_tmpD7F))))));}*_tmp8C7=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp8C7,_tmp8C4,v);};}else{struct
_tuple14 _tmp8CD;struct Cyc_Absyn_Aggrdecl*_tmp8CE;void*_tmp8CF;struct _tuple14*
_tmp8CC=*_tmp8C9;_tmp8CD=*_tmp8CC;_tmp8CE=_tmp8CD.f1;_tmp8CF=_tmp8CD.f2;if(
_tmp8CE->impl == 0){{struct _tuple14*_tmpD80;*_tmp8C7=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp8C7,
_tmp8C4,((_tmpD80=_region_malloc(d,sizeof(*_tmpD80)),((_tmpD80->f1=ad,((_tmpD80->f2=
_tmp8CF,_tmpD80)))))));}seen_defn_before=0;}else{seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*
_tmpD81;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpD81=_cycalloc(sizeof(*_tmpD81)),((
_tmpD81->kind=ad->kind,((_tmpD81->sc=Cyc_Absyn_Public,((_tmpD81->name=ad->name,((
_tmpD81->tvs=0,((_tmpD81->impl=0,((_tmpD81->attributes=ad->attributes,_tmpD81)))))))))))));
if(ad->impl != 0  && !seen_defn_before){{struct Cyc_Absyn_AggrdeclImpl*_tmpD82;
new_ad->impl=((_tmpD82=_cycalloc(sizeof(*_tmpD82)),((_tmpD82->exist_vars=0,((
_tmpD82->rgn_po=0,((_tmpD82->fields=0,((_tmpD82->tagged=0,_tmpD82))))))))));}{
struct Cyc_List_List*new_fields=0;{struct Cyc_List_List*_tmp8D2=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp8D2 != 0;_tmp8D2=_tmp8D2->tl){struct Cyc_Absyn_Aggrfield*
_tmp8D3=(struct Cyc_Absyn_Aggrfield*)_tmp8D2->hd;struct Cyc_Absyn_Aggrfield*
_tmpD83;struct Cyc_Absyn_Aggrfield*_tmp8D4=(_tmpD83=_cycalloc(sizeof(*_tmpD83)),((
_tmpD83->name=_tmp8D3->name,((_tmpD83->tq=Cyc_Toc_mt_tq,((_tmpD83->type=Cyc_Toc_typ_to_c_array(
_tmp8D3->type),((_tmpD83->width=_tmp8D3->width,((_tmpD83->attributes=_tmp8D3->attributes,
_tmpD83)))))))))));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp8D5=_tmp8D4->type;struct _dyneither_ptr*_tmp8D6=_tmp8D4->name;const char*
_tmpD88;void*_tmpD87[2];struct Cyc_String_pa_struct _tmpD86;struct Cyc_String_pa_struct
_tmpD85;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpD85.tag=0,((_tmpD85.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8D6),((_tmpD86.tag=0,((_tmpD86.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpD87[0]=&
_tmpD86,((_tmpD87[1]=& _tmpD85,Cyc_aprintf(((_tmpD88="_union_%s_%s",
_tag_dyneither(_tmpD88,sizeof(char),13))),_tag_dyneither(_tmpD87,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpD89;struct _dyneither_ptr*str=(_tmpD89=_cycalloc(sizeof(*
_tmpD89)),((_tmpD89[0]=s,_tmpD89)));struct Cyc_Absyn_Aggrfield*_tmpD8A;struct Cyc_Absyn_Aggrfield*
_tmp8D7=(_tmpD8A=_cycalloc(sizeof(*_tmpD8A)),((_tmpD8A->name=Cyc_Toc_val_sp,((
_tmpD8A->tq=Cyc_Toc_mt_tq,((_tmpD8A->type=_tmp8D5,((_tmpD8A->width=0,((_tmpD8A->attributes=
0,_tmpD8A)))))))))));struct Cyc_Absyn_Aggrfield*_tmpD8B;struct Cyc_Absyn_Aggrfield*
_tmp8D8=(_tmpD8B=_cycalloc(sizeof(*_tmpD8B)),((_tmpD8B->name=Cyc_Toc_tag_sp,((
_tmpD8B->tq=Cyc_Toc_mt_tq,((_tmpD8B->type=Cyc_Absyn_sint_typ,((_tmpD8B->width=0,((
_tmpD8B->attributes=0,_tmpD8B)))))))))));struct Cyc_Absyn_Aggrfield*_tmpD8C[2];
struct Cyc_List_List*_tmp8D9=(_tmpD8C[1]=_tmp8D7,((_tmpD8C[0]=_tmp8D8,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD8C,sizeof(struct Cyc_Absyn_Aggrfield*),
2)))));struct Cyc_Absyn_AggrdeclImpl*_tmpD91;struct _tuple0*_tmpD90;struct Cyc_Absyn_Aggrdecl*
_tmpD8F;struct Cyc_Absyn_Aggrdecl*_tmp8DA=(_tmpD8F=_cycalloc(sizeof(*_tmpD8F)),((
_tmpD8F->kind=Cyc_Absyn_StructA,((_tmpD8F->sc=Cyc_Absyn_Public,((_tmpD8F->name=((
_tmpD90=_cycalloc(sizeof(*_tmpD90)),((_tmpD90->f1=Cyc_Absyn_Loc_n,((_tmpD90->f2=
str,_tmpD90)))))),((_tmpD8F->tvs=0,((_tmpD8F->impl=((_tmpD91=_cycalloc(sizeof(*
_tmpD91)),((_tmpD91->exist_vars=0,((_tmpD91->rgn_po=0,((_tmpD91->fields=_tmp8D9,((
_tmpD91->tagged=0,_tmpD91)))))))))),((_tmpD8F->attributes=0,_tmpD8F)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmpD97;struct Cyc_Absyn_Aggr_d_struct _tmpD96;
struct Cyc_List_List*_tmpD95;Cyc_Toc_result_decls=((_tmpD95=_cycalloc(sizeof(*
_tmpD95)),((_tmpD95->hd=Cyc_Absyn_new_decl((void*)((_tmpD97=_cycalloc(sizeof(*
_tmpD97)),((_tmpD97[0]=((_tmpD96.tag=6,((_tmpD96.f1=_tmp8DA,_tmpD96)))),_tmpD97)))),
0),((_tmpD95->tl=Cyc_Toc_result_decls,_tmpD95))))));}_tmp8D4->type=Cyc_Absyn_strct(
str);}{struct Cyc_List_List*_tmpD98;new_fields=((_tmpD98=_cycalloc(sizeof(*
_tmpD98)),((_tmpD98->hd=_tmp8D4,((_tmpD98->tl=new_fields,_tmpD98))))));};}}(
new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_fields);};}{struct Cyc_Absyn_Decl*_tmpDA2;struct Cyc_Absyn_Aggr_d_struct
_tmpDA1;struct Cyc_Absyn_Aggr_d_struct*_tmpDA0;struct Cyc_List_List*_tmpD9F;Cyc_Toc_result_decls=((
_tmpD9F=_cycalloc(sizeof(*_tmpD9F)),((_tmpD9F->hd=((_tmpDA2=_cycalloc(sizeof(*
_tmpDA2)),((_tmpDA2->r=(void*)((_tmpDA0=_cycalloc(sizeof(*_tmpDA0)),((_tmpDA0[0]=((
_tmpDA1.tag=6,((_tmpDA1.f1=new_ad,_tmpDA1)))),_tmpDA0)))),((_tmpDA2->loc=0,
_tmpDA2)))))),((_tmpD9F->tl=Cyc_Toc_result_decls,_tmpD9F))))));};};};
_pop_dynregion(d);};}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud){struct _DynRegionHandle*_tmp8F1;struct Cyc_Set_Set**_tmp8F2;struct Cyc_Toc_TocState
_tmp8F0=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8F1=
_tmp8F0.dyn;_tmp8F2=_tmp8F0.datatypes_so_far;{struct _DynRegionFrame _tmp8F3;
struct _RegionHandle*d=_open_dynregion(& _tmp8F3,_tmp8F1);{struct _tuple0*_tmp8F4=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))
Cyc_Set_member)(*_tmp8F2,_tmp8F4)){_npop_handler(0);return;}*_tmp8F2=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*
_tmp8F2,_tmp8F4);}{struct Cyc_List_List*_tmp8F5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp8F5 != 0;_tmp8F5=_tmp8F5->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp8F5->hd;struct Cyc_List_List*_tmp8F6=0;int i=
1;{struct Cyc_List_List*_tmp8F7=f->typs;for(0;_tmp8F7 != 0;(_tmp8F7=_tmp8F7->tl,i
++)){struct _dyneither_ptr*_tmp8F8=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpDA3;struct Cyc_Absyn_Aggrfield*_tmp8F9=(_tmpDA3=_cycalloc(sizeof(*_tmpDA3)),((
_tmpDA3->name=_tmp8F8,((_tmpDA3->tq=(*((struct _tuple9*)_tmp8F7->hd)).f1,((
_tmpDA3->type=Cyc_Toc_typ_to_c_array((*((struct _tuple9*)_tmp8F7->hd)).f2),((
_tmpDA3->width=0,((_tmpDA3->attributes=0,_tmpDA3)))))))))));struct Cyc_List_List*
_tmpDA4;_tmp8F6=((_tmpDA4=_cycalloc(sizeof(*_tmpDA4)),((_tmpDA4->hd=_tmp8F9,((
_tmpDA4->tl=_tmp8F6,_tmpDA4))))));}}{struct Cyc_Absyn_Aggrfield*_tmpDA7;struct Cyc_List_List*
_tmpDA6;_tmp8F6=((_tmpDA6=_cycalloc(sizeof(*_tmpDA6)),((_tmpDA6->hd=((_tmpDA7=
_cycalloc(sizeof(*_tmpDA7)),((_tmpDA7->name=Cyc_Toc_tag_sp,((_tmpDA7->tq=Cyc_Toc_mt_tq,((
_tmpDA7->type=Cyc_Absyn_sint_typ,((_tmpDA7->width=0,((_tmpDA7->attributes=0,
_tmpDA7)))))))))))),((_tmpDA6->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp8F6),_tmpDA6))))));}{struct Cyc_Absyn_AggrdeclImpl*
_tmpDAC;const char*_tmpDAB;struct Cyc_Absyn_Aggrdecl*_tmpDAA;struct Cyc_Absyn_Aggrdecl*
_tmp8FE=(_tmpDAA=_cycalloc(sizeof(*_tmpDAA)),((_tmpDAA->kind=Cyc_Absyn_StructA,((
_tmpDAA->sc=Cyc_Absyn_Public,((_tmpDAA->name=Cyc_Toc_collapse_qvar_tag(f->name,((
_tmpDAB="_struct",_tag_dyneither(_tmpDAB,sizeof(char),8)))),((_tmpDAA->tvs=0,((
_tmpDAA->impl=((_tmpDAC=_cycalloc(sizeof(*_tmpDAC)),((_tmpDAC->exist_vars=0,((
_tmpDAC->rgn_po=0,((_tmpDAC->fields=_tmp8F6,((_tmpDAC->tagged=0,_tmpDAC)))))))))),((
_tmpDAA->attributes=0,_tmpDAA)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpDB2;
struct Cyc_Absyn_Aggr_d_struct _tmpDB1;struct Cyc_List_List*_tmpDB0;Cyc_Toc_result_decls=((
_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((_tmpDB0->hd=Cyc_Absyn_new_decl((void*)((
_tmpDB2=_cycalloc(sizeof(*_tmpDB2)),((_tmpDB2[0]=((_tmpDB1.tag=6,((_tmpDB1.f1=
_tmp8FE,_tmpDB1)))),_tmpDB2)))),0),((_tmpDB0->tl=Cyc_Toc_result_decls,_tmpDB0))))));};}};;
_pop_dynregion(d);};}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd){if(xd->fields == 0)return;{struct _DynRegionHandle*_tmp906;struct Cyc_Dict_Dict*
_tmp907;struct Cyc_Toc_TocState _tmp905=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp906=_tmp905.dyn;_tmp907=_tmp905.xdatatypes_so_far;{struct _DynRegionFrame
_tmp908;struct _RegionHandle*d=_open_dynregion(& _tmp908,_tmp906);{struct _tuple0*
_tmp909=xd->name;struct Cyc_List_List*_tmp90A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp90A != 0;_tmp90A=_tmp90A->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp90A->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp90B=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp90C=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp90B,Cyc_Absyn_false_conref,0);int*_tmp90D=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp907,f->name);int
_tmp90E;_LL397: if(_tmp90D != 0)goto _LL399;_LL398: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp90F=Cyc_Absyn_new_vardecl(f->name,_tmp90C,
initopt);_tmp90F->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*_tmpDB8;struct Cyc_Absyn_Var_d_struct
_tmpDB7;struct Cyc_List_List*_tmpDB6;Cyc_Toc_result_decls=((_tmpDB6=_cycalloc(
sizeof(*_tmpDB6)),((_tmpDB6->hd=Cyc_Absyn_new_decl((void*)((_tmpDB8=_cycalloc(
sizeof(*_tmpDB8)),((_tmpDB8[0]=((_tmpDB7.tag=0,((_tmpDB7.f1=_tmp90F,_tmpDB7)))),
_tmpDB8)))),0),((_tmpDB6->tl=Cyc_Toc_result_decls,_tmpDB6))))));}*_tmp907=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp907,f->name,f->sc != Cyc_Absyn_Extern);{struct Cyc_List_List*fields=0;int i=1;{
struct Cyc_List_List*_tmp913=f->typs;for(0;_tmp913 != 0;(_tmp913=_tmp913->tl,i ++)){
struct Cyc_Int_pa_struct _tmpDC0;void*_tmpDBF[1];const char*_tmpDBE;struct
_dyneither_ptr*_tmpDBD;struct _dyneither_ptr*_tmp914=(_tmpDBD=_cycalloc(sizeof(*
_tmpDBD)),((_tmpDBD[0]=(struct _dyneither_ptr)((_tmpDC0.tag=1,((_tmpDC0.f1=(
unsigned long)i,((_tmpDBF[0]=& _tmpDC0,Cyc_aprintf(((_tmpDBE="f%d",_tag_dyneither(
_tmpDBE,sizeof(char),4))),_tag_dyneither(_tmpDBF,sizeof(void*),1)))))))),_tmpDBD)));
struct Cyc_Absyn_Aggrfield*_tmpDC1;struct Cyc_Absyn_Aggrfield*_tmp915=(_tmpDC1=
_cycalloc(sizeof(*_tmpDC1)),((_tmpDC1->name=_tmp914,((_tmpDC1->tq=(*((struct
_tuple9*)_tmp913->hd)).f1,((_tmpDC1->type=Cyc_Toc_typ_to_c_array((*((struct
_tuple9*)_tmp913->hd)).f2),((_tmpDC1->width=0,((_tmpDC1->attributes=0,_tmpDC1)))))))))));
struct Cyc_List_List*_tmpDC2;fields=((_tmpDC2=_cycalloc(sizeof(*_tmpDC2)),((
_tmpDC2->hd=_tmp915,((_tmpDC2->tl=fields,_tmpDC2))))));}}{struct Cyc_Absyn_Aggrfield*
_tmpDC5;struct Cyc_List_List*_tmpDC4;fields=((_tmpDC4=_cycalloc(sizeof(*_tmpDC4)),((
_tmpDC4->hd=((_tmpDC5=_cycalloc(sizeof(*_tmpDC5)),((_tmpDC5->name=Cyc_Toc_tag_sp,((
_tmpDC5->tq=Cyc_Toc_mt_tq,((_tmpDC5->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq),((_tmpDC5->width=0,((_tmpDC5->attributes=0,_tmpDC5)))))))))))),((
_tmpDC4->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmpDC4))))));}{struct Cyc_Absyn_AggrdeclImpl*_tmpDCA;const char*_tmpDC9;
struct Cyc_Absyn_Aggrdecl*_tmpDC8;struct Cyc_Absyn_Aggrdecl*_tmp91E=(_tmpDC8=
_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8->kind=Cyc_Absyn_StructA,((_tmpDC8->sc=Cyc_Absyn_Public,((
_tmpDC8->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpDC9="_struct",
_tag_dyneither(_tmpDC9,sizeof(char),8)))),((_tmpDC8->tvs=0,((_tmpDC8->impl=((
_tmpDCA=_cycalloc(sizeof(*_tmpDCA)),((_tmpDCA->exist_vars=0,((_tmpDCA->rgn_po=0,((
_tmpDCA->fields=fields,((_tmpDCA->tagged=0,_tmpDCA)))))))))),((_tmpDC8->attributes=
0,_tmpDC8)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpDD0;struct Cyc_Absyn_Aggr_d_struct
_tmpDCF;struct Cyc_List_List*_tmpDCE;Cyc_Toc_result_decls=((_tmpDCE=_cycalloc(
sizeof(*_tmpDCE)),((_tmpDCE->hd=Cyc_Absyn_new_decl((void*)((_tmpDD0=_cycalloc(
sizeof(*_tmpDD0)),((_tmpDD0[0]=((_tmpDCF.tag=6,((_tmpDCF.f1=_tmp91E,_tmpDCF)))),
_tmpDD0)))),0),((_tmpDCE->tl=Cyc_Toc_result_decls,_tmpDCE))))));}goto _LL396;};};};}
_LL399: if(_tmp90D == 0)goto _LL39B;_tmp90E=*_tmp90D;if(_tmp90E != 0)goto _LL39B;
_LL39A: if(f->sc != Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*_tmp925=Cyc_Absyn_string_exp(*
fn,0);struct Cyc_Absyn_Vardecl*_tmp926=Cyc_Absyn_new_vardecl(f->name,_tmp90C,(
struct Cyc_Absyn_Exp*)_tmp925);_tmp926->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*
_tmpDD6;struct Cyc_Absyn_Var_d_struct _tmpDD5;struct Cyc_List_List*_tmpDD4;Cyc_Toc_result_decls=((
_tmpDD4=_cycalloc(sizeof(*_tmpDD4)),((_tmpDD4->hd=Cyc_Absyn_new_decl((void*)((
_tmpDD6=_cycalloc(sizeof(*_tmpDD6)),((_tmpDD6[0]=((_tmpDD5.tag=0,((_tmpDD5.f1=
_tmp926,_tmpDD5)))),_tmpDD6)))),0),((_tmpDD4->tl=Cyc_Toc_result_decls,_tmpDD4))))));}*
_tmp907=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp907,f->name,1);}goto _LL396;_LL39B:;_LL39C: goto _LL396;_LL396:;}};
_pop_dynregion(d);};};}static void Cyc_Toc_enumdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Enumdecl*ed){ed->sc=Cyc_Absyn_Public;if(ed->fields != 0)Cyc_Toc_enumfields_to_c((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}static
void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,
struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){void*old_typ=vd->type;vd->type=
Cyc_Toc_typ_to_c_array(old_typ);if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ))vd->sc=Cyc_Absyn_Public;Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp92A=init->r;struct Cyc_Absyn_Vardecl*_tmp92C;struct Cyc_Absyn_Exp*_tmp92D;
struct Cyc_Absyn_Exp*_tmp92E;int _tmp92F;_LL39E: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp92B=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp92A;if(_tmp92B->tag != 28)
goto _LL3A0;else{_tmp92C=_tmp92B->f1;_tmp92D=_tmp92B->f2;_tmp92E=_tmp92B->f3;
_tmp92F=_tmp92B->f4;}}_LL39F: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp92C,_tmp92D,_tmp92E,_tmp92F,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL39D;_LL3A0:;_LL3A1: if(vd->sc == Cyc_Absyn_Static){struct Cyc_Toc_Env
_tmp931;struct _RegionHandle*_tmp932;struct Cyc_Toc_Env*_tmp930=init_nv;_tmp931=*
_tmp930;_tmp932=_tmp931.rgn;{struct Cyc_Toc_Env*_tmp933=Cyc_Toc_set_toplevel(
_tmp932,init_nv);Cyc_Toc_exp_to_c(_tmp933,init);};}else{Cyc_Toc_exp_to_c(init_nv,
init);}goto _LL39D;_LL39D:;}else{void*_tmp934=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_ArrayInfo _tmp936;void*_tmp937;struct Cyc_Absyn_Exp*_tmp938;union Cyc_Absyn_Constraint*
_tmp939;_LL3A3:{struct Cyc_Absyn_ArrayType_struct*_tmp935=(struct Cyc_Absyn_ArrayType_struct*)
_tmp934;if(_tmp935->tag != 9)goto _LL3A5;else{_tmp936=_tmp935->f1;_tmp937=_tmp936.elt_type;
_tmp938=_tmp936.num_elts;_tmp939=_tmp936.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp939))goto _LL3A5;_LL3A4: if(_tmp938 == 0){const char*
_tmpDD9;void*_tmpDD8;(_tmpDD8=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDD9="can't initialize zero-terminated array -- size unknown",
_tag_dyneither(_tmpDD9,sizeof(char),55))),_tag_dyneither(_tmpDD8,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp938;struct Cyc_Absyn_Exp*
_tmp93C=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp93D=Cyc_Absyn_signed_int_exp(
0,0);s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp93C,
_tmp93D,0),0),Cyc_Absyn_new_stmt(s->r,0));goto _LL3A2;};_LL3A5:;_LL3A6: goto _LL3A2;
_LL3A2:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple0*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp93F;struct
_RegionHandle*_tmp940;struct Cyc_Toc_Env*_tmp93E=nv;_tmp93F=*_tmp93E;_tmp940=
_tmp93F.rgn;{struct Cyc_Absyn_Stmt*_tmp941=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp942=Cyc_Toc_share_env(_tmp940,nv);struct Cyc_Toc_Env*_tmp944;struct Cyc_List_List*
_tmp945;struct Cyc_Absyn_Stmt*_tmp946;struct _tuple21 _tmp943=Cyc_Toc_xlate_pat(
_tmp942,_tmp940,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp944=_tmp943.f1;_tmp945=_tmp943.f2;_tmp946=_tmp943.f3;Cyc_Toc_stmt_to_c(
_tmp944,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp946,s,0),0);for(0;_tmp945 != 0;_tmp945=_tmp945->tl){
struct _tuple22 _tmp948;struct _tuple0*_tmp949;void*_tmp94A;struct _tuple22*_tmp947=(
struct _tuple22*)_tmp945->hd;_tmp948=*_tmp947;_tmp949=_tmp948.f1;_tmp94A=_tmp948.f2;
s=Cyc_Absyn_declare_stmt(_tmp949,_tmp94A,0,s,0);}}return s;};}static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e){void*_tmp94B=e->r;struct Cyc_Absyn_Exp*_tmp94D;struct Cyc_Absyn_Exp*
_tmp94F;struct Cyc_Absyn_Exp*_tmp951;struct Cyc_Absyn_Exp*_tmp953;struct Cyc_Absyn_Exp*
_tmp955;struct Cyc_Absyn_Exp*_tmp957;struct Cyc_Absyn_Exp*_tmp959;struct Cyc_Absyn_Exp*
_tmp95B;struct Cyc_List_List*_tmp95D;struct Cyc_Absyn_Exp*_tmp95F;struct Cyc_Absyn_Exp*
_tmp960;struct Cyc_Absyn_Exp*_tmp962;struct Cyc_Absyn_Exp*_tmp963;struct Cyc_Absyn_Exp*
_tmp965;struct Cyc_Absyn_Exp*_tmp966;struct Cyc_Absyn_Exp*_tmp968;struct Cyc_Absyn_Exp*
_tmp969;struct Cyc_Absyn_Exp*_tmp96B;struct Cyc_Absyn_Exp*_tmp96C;struct Cyc_Absyn_Exp*
_tmp96E;struct Cyc_Absyn_Exp*_tmp96F;struct Cyc_Absyn_Exp*_tmp971;struct Cyc_Absyn_Exp*
_tmp972;struct Cyc_Absyn_Exp*_tmp973;struct Cyc_Absyn_Exp*_tmp975;struct Cyc_List_List*
_tmp976;struct Cyc_Absyn_Exp*_tmp978;struct Cyc_List_List*_tmp979;void*_tmp97B;
void**_tmp97C;struct Cyc_Absyn_Exp*_tmp97D;struct _tuple7*_tmp97F;struct _tuple7
_tmp980;void*_tmp981;void**_tmp982;struct Cyc_List_List*_tmp983;struct Cyc_List_List*
_tmp985;struct Cyc_List_List*_tmp987;void*_tmp989;void**_tmp98A;void*_tmp98C;void**
_tmp98D;struct Cyc_Absyn_Stmt*_tmp98F;struct Cyc_Absyn_MallocInfo _tmp991;struct Cyc_Absyn_MallocInfo*
_tmp992;_LL3A8: {struct Cyc_Absyn_Deref_e_struct*_tmp94C=(struct Cyc_Absyn_Deref_e_struct*)
_tmp94B;if(_tmp94C->tag != 21)goto _LL3AA;else{_tmp94D=_tmp94C->f1;}}_LL3A9:
_tmp94F=_tmp94D;goto _LL3AB;_LL3AA: {struct Cyc_Absyn_AggrMember_e_struct*_tmp94E=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp94B;if(_tmp94E->tag != 22)goto _LL3AC;
else{_tmp94F=_tmp94E->f1;}}_LL3AB: _tmp951=_tmp94F;goto _LL3AD;_LL3AC: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp950=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp94B;if(_tmp950->tag != 23)goto
_LL3AE;else{_tmp951=_tmp950->f1;}}_LL3AD: _tmp953=_tmp951;goto _LL3AF;_LL3AE: {
struct Cyc_Absyn_Address_e_struct*_tmp952=(struct Cyc_Absyn_Address_e_struct*)
_tmp94B;if(_tmp952->tag != 16)goto _LL3B0;else{_tmp953=_tmp952->f1;}}_LL3AF:
_tmp955=_tmp953;goto _LL3B1;_LL3B0: {struct Cyc_Absyn_Throw_e_struct*_tmp954=(
struct Cyc_Absyn_Throw_e_struct*)_tmp94B;if(_tmp954->tag != 12)goto _LL3B2;else{
_tmp955=_tmp954->f1;}}_LL3B1: _tmp957=_tmp955;goto _LL3B3;_LL3B2: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp956=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp94B;if(_tmp956->tag != 13)
goto _LL3B4;else{_tmp957=_tmp956->f1;}}_LL3B3: _tmp959=_tmp957;goto _LL3B5;_LL3B4: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp958=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp94B;if(_tmp958->tag != 19)goto _LL3B6;else{_tmp959=_tmp958->f1;}}_LL3B5:
_tmp95B=_tmp959;goto _LL3B7;_LL3B6: {struct Cyc_Absyn_Increment_e_struct*_tmp95A=(
struct Cyc_Absyn_Increment_e_struct*)_tmp94B;if(_tmp95A->tag != 5)goto _LL3B8;else{
_tmp95B=_tmp95A->f1;}}_LL3B7: Cyc_Toc_exptypes_to_c(_tmp95B);goto _LL3A7;_LL3B8: {
struct Cyc_Absyn_Primop_e_struct*_tmp95C=(struct Cyc_Absyn_Primop_e_struct*)
_tmp94B;if(_tmp95C->tag != 3)goto _LL3BA;else{_tmp95D=_tmp95C->f2;}}_LL3B9:((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,
_tmp95D);goto _LL3A7;_LL3BA: {struct Cyc_Absyn_And_e_struct*_tmp95E=(struct Cyc_Absyn_And_e_struct*)
_tmp94B;if(_tmp95E->tag != 7)goto _LL3BC;else{_tmp95F=_tmp95E->f1;_tmp960=_tmp95E->f2;}}
_LL3BB: _tmp962=_tmp95F;_tmp963=_tmp960;goto _LL3BD;_LL3BC: {struct Cyc_Absyn_Or_e_struct*
_tmp961=(struct Cyc_Absyn_Or_e_struct*)_tmp94B;if(_tmp961->tag != 8)goto _LL3BE;
else{_tmp962=_tmp961->f1;_tmp963=_tmp961->f2;}}_LL3BD: _tmp965=_tmp962;_tmp966=
_tmp963;goto _LL3BF;_LL3BE: {struct Cyc_Absyn_SeqExp_e_struct*_tmp964=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp94B;if(_tmp964->tag != 9)goto _LL3C0;else{_tmp965=_tmp964->f1;_tmp966=_tmp964->f2;}}
_LL3BF: _tmp968=_tmp965;_tmp969=_tmp966;goto _LL3C1;_LL3C0: {struct Cyc_Absyn_Subscript_e_struct*
_tmp967=(struct Cyc_Absyn_Subscript_e_struct*)_tmp94B;if(_tmp967->tag != 24)goto
_LL3C2;else{_tmp968=_tmp967->f1;_tmp969=_tmp967->f2;}}_LL3C1: _tmp96B=_tmp968;
_tmp96C=_tmp969;goto _LL3C3;_LL3C2: {struct Cyc_Absyn_Swap_e_struct*_tmp96A=(
struct Cyc_Absyn_Swap_e_struct*)_tmp94B;if(_tmp96A->tag != 35)goto _LL3C4;else{
_tmp96B=_tmp96A->f1;_tmp96C=_tmp96A->f2;}}_LL3C3: _tmp96E=_tmp96B;_tmp96F=_tmp96C;
goto _LL3C5;_LL3C4: {struct Cyc_Absyn_AssignOp_e_struct*_tmp96D=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp94B;if(_tmp96D->tag != 4)goto _LL3C6;else{_tmp96E=_tmp96D->f1;_tmp96F=_tmp96D->f3;}}
_LL3C5: Cyc_Toc_exptypes_to_c(_tmp96E);Cyc_Toc_exptypes_to_c(_tmp96F);goto _LL3A7;
_LL3C6: {struct Cyc_Absyn_Conditional_e_struct*_tmp970=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp94B;if(_tmp970->tag != 6)goto _LL3C8;else{_tmp971=_tmp970->f1;_tmp972=_tmp970->f2;
_tmp973=_tmp970->f3;}}_LL3C7: Cyc_Toc_exptypes_to_c(_tmp971);Cyc_Toc_exptypes_to_c(
_tmp972);Cyc_Toc_exptypes_to_c(_tmp973);goto _LL3A7;_LL3C8: {struct Cyc_Absyn_FnCall_e_struct*
_tmp974=(struct Cyc_Absyn_FnCall_e_struct*)_tmp94B;if(_tmp974->tag != 11)goto
_LL3CA;else{_tmp975=_tmp974->f1;_tmp976=_tmp974->f2;}}_LL3C9: _tmp978=_tmp975;
_tmp979=_tmp976;goto _LL3CB;_LL3CA: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp977=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp94B;if(_tmp977->tag != 10)goto _LL3CC;
else{_tmp978=_tmp977->f1;_tmp979=_tmp977->f2;}}_LL3CB: Cyc_Toc_exptypes_to_c(
_tmp978);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp979);goto _LL3A7;_LL3CC: {struct Cyc_Absyn_Cast_e_struct*
_tmp97A=(struct Cyc_Absyn_Cast_e_struct*)_tmp94B;if(_tmp97A->tag != 15)goto _LL3CE;
else{_tmp97B=(void**)& _tmp97A->f1;_tmp97C=(void**)((void**)& _tmp97A->f1);_tmp97D=
_tmp97A->f2;}}_LL3CD:*_tmp97C=Cyc_Toc_typ_to_c(*_tmp97C);Cyc_Toc_exptypes_to_c(
_tmp97D);goto _LL3A7;_LL3CE: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp97E=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp94B;if(_tmp97E->tag != 26)goto _LL3D0;
else{_tmp97F=_tmp97E->f1;_tmp980=*_tmp97F;_tmp981=_tmp980.f3;_tmp982=(void**)&(*
_tmp97E->f1).f3;_tmp983=_tmp97E->f2;}}_LL3CF:*_tmp982=Cyc_Toc_typ_to_c(*_tmp982);
_tmp985=_tmp983;goto _LL3D1;_LL3D0: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp984=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp94B;if(_tmp984->tag != 36)
goto _LL3D2;else{_tmp985=_tmp984->f2;}}_LL3D1: _tmp987=_tmp985;goto _LL3D3;_LL3D2: {
struct Cyc_Absyn_Array_e_struct*_tmp986=(struct Cyc_Absyn_Array_e_struct*)_tmp94B;
if(_tmp986->tag != 27)goto _LL3D4;else{_tmp987=_tmp986->f1;}}_LL3D3: for(0;_tmp987
!= 0;_tmp987=_tmp987->tl){struct Cyc_Absyn_Exp*_tmp9A3;struct _tuple15 _tmp9A2=*((
struct _tuple15*)_tmp987->hd);_tmp9A3=_tmp9A2.f2;Cyc_Toc_exptypes_to_c(_tmp9A3);}
goto _LL3A7;_LL3D4: {struct Cyc_Absyn_Offsetof_e_struct*_tmp988=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp94B;if(_tmp988->tag != 20)goto _LL3D6;else{_tmp989=(void**)& _tmp988->f1;
_tmp98A=(void**)((void**)& _tmp988->f1);}}_LL3D5: _tmp98D=_tmp98A;goto _LL3D7;
_LL3D6: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp98B=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp94B;if(_tmp98B->tag != 18)goto _LL3D8;else{_tmp98C=(void**)& _tmp98B->f1;
_tmp98D=(void**)((void**)& _tmp98B->f1);}}_LL3D7:*_tmp98D=Cyc_Toc_typ_to_c(*
_tmp98D);goto _LL3A7;_LL3D8: {struct Cyc_Absyn_StmtExp_e_struct*_tmp98E=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp94B;if(_tmp98E->tag != 37)goto _LL3DA;else{_tmp98F=_tmp98E->f1;}}_LL3D9: Cyc_Toc_stmttypes_to_c(
_tmp98F);goto _LL3A7;_LL3DA: {struct Cyc_Absyn_Malloc_e_struct*_tmp990=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp94B;if(_tmp990->tag != 34)goto _LL3DC;else{_tmp991=_tmp990->f1;_tmp992=(struct
Cyc_Absyn_MallocInfo*)& _tmp990->f1;}}_LL3DB: if(_tmp992->elt_type != 0){void**
_tmpDDA;_tmp992->elt_type=((_tmpDDA=_cycalloc(sizeof(*_tmpDDA)),((_tmpDDA[0]=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp992->elt_type))),_tmpDDA))));}Cyc_Toc_exptypes_to_c(
_tmp992->num_elts);goto _LL3A7;_LL3DC: {struct Cyc_Absyn_Const_e_struct*_tmp993=(
struct Cyc_Absyn_Const_e_struct*)_tmp94B;if(_tmp993->tag != 0)goto _LL3DE;}_LL3DD:
goto _LL3DF;_LL3DE: {struct Cyc_Absyn_Var_e_struct*_tmp994=(struct Cyc_Absyn_Var_e_struct*)
_tmp94B;if(_tmp994->tag != 1)goto _LL3E0;}_LL3DF: goto _LL3E1;_LL3E0: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp995=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp94B;if(_tmp995->tag != 2)goto
_LL3E2;}_LL3E1: goto _LL3E3;_LL3E2: {struct Cyc_Absyn_Enum_e_struct*_tmp996=(struct
Cyc_Absyn_Enum_e_struct*)_tmp94B;if(_tmp996->tag != 32)goto _LL3E4;}_LL3E3: goto
_LL3E5;_LL3E4: {struct Cyc_Absyn_Asm_e_struct*_tmp997=(struct Cyc_Absyn_Asm_e_struct*)
_tmp94B;if(_tmp997->tag != 40)goto _LL3E6;}_LL3E5: goto _LL3E7;_LL3E6: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp998=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp94B;if(_tmp998->tag != 33)goto
_LL3E8;}_LL3E7: goto _LL3A7;_LL3E8: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp999=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp94B;if(_tmp999->tag != 30)goto _LL3EA;}
_LL3E9: goto _LL3EB;_LL3EA: {struct Cyc_Absyn_Datatype_e_struct*_tmp99A=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp94B;if(_tmp99A->tag != 31)goto _LL3EC;}_LL3EB: goto _LL3ED;_LL3EC: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp99B=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp94B;if(_tmp99B->tag != 29)goto
_LL3EE;}_LL3ED: goto _LL3EF;_LL3EE: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp99C=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp94B;if(_tmp99C->tag != 28)
goto _LL3F0;}_LL3EF: goto _LL3F1;_LL3F0: {struct Cyc_Absyn_Tuple_e_struct*_tmp99D=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp94B;if(_tmp99D->tag != 25)goto _LL3F2;}_LL3F1:
goto _LL3F3;_LL3F2: {struct Cyc_Absyn_Instantiate_e_struct*_tmp99E=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp94B;if(_tmp99E->tag != 14)goto _LL3F4;}_LL3F3: goto _LL3F5;_LL3F4: {struct Cyc_Absyn_New_e_struct*
_tmp99F=(struct Cyc_Absyn_New_e_struct*)_tmp94B;if(_tmp99F->tag != 17)goto _LL3F6;}
_LL3F5: goto _LL3F7;_LL3F6: {struct Cyc_Absyn_Valueof_e_struct*_tmp9A0=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp94B;if(_tmp9A0->tag != 39)goto _LL3F8;}_LL3F7: goto _LL3F9;_LL3F8: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp9A1=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp94B;if(_tmp9A1->tag != 38)goto
_LL3A7;}_LL3F9:{const char*_tmpDDD;void*_tmpDDC;(_tmpDDC=0,Cyc_Tcutil_terr(e->loc,((
_tmpDDD="Cyclone expression within C code",_tag_dyneither(_tmpDDD,sizeof(char),
33))),_tag_dyneither(_tmpDDC,sizeof(void*),0)));}goto _LL3A7;_LL3A7:;}static void
Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){void*_tmp9A7=d->r;struct Cyc_Absyn_Vardecl*
_tmp9A9;struct Cyc_Absyn_Fndecl*_tmp9AB;struct Cyc_Absyn_Aggrdecl*_tmp9AD;struct
Cyc_Absyn_Enumdecl*_tmp9AF;struct Cyc_Absyn_Typedefdecl*_tmp9B1;_LL3FB: {struct
Cyc_Absyn_Var_d_struct*_tmp9A8=(struct Cyc_Absyn_Var_d_struct*)_tmp9A7;if(_tmp9A8->tag
!= 0)goto _LL3FD;else{_tmp9A9=_tmp9A8->f1;}}_LL3FC: _tmp9A9->type=Cyc_Toc_typ_to_c(
_tmp9A9->type);if(_tmp9A9->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp9A9->initializer));goto _LL3FA;_LL3FD: {struct Cyc_Absyn_Fn_d_struct*
_tmp9AA=(struct Cyc_Absyn_Fn_d_struct*)_tmp9A7;if(_tmp9AA->tag != 1)goto _LL3FF;
else{_tmp9AB=_tmp9AA->f1;}}_LL3FE: _tmp9AB->ret_type=Cyc_Toc_typ_to_c(_tmp9AB->ret_type);{
struct Cyc_List_List*_tmp9BD=_tmp9AB->args;for(0;_tmp9BD != 0;_tmp9BD=_tmp9BD->tl){(*((
struct _tuple25*)_tmp9BD->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple25*)_tmp9BD->hd)).f3);}}
goto _LL3FA;_LL3FF: {struct Cyc_Absyn_Aggr_d_struct*_tmp9AC=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp9A7;if(_tmp9AC->tag != 6)goto _LL401;else{_tmp9AD=_tmp9AC->f1;}}_LL400: Cyc_Toc_aggrdecl_to_c(
_tmp9AD);goto _LL3FA;_LL401: {struct Cyc_Absyn_Enum_d_struct*_tmp9AE=(struct Cyc_Absyn_Enum_d_struct*)
_tmp9A7;if(_tmp9AE->tag != 8)goto _LL403;else{_tmp9AF=_tmp9AE->f1;}}_LL402: if(
_tmp9AF->fields != 0){struct Cyc_List_List*_tmp9BE=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp9AF->fields))->v;for(0;_tmp9BE != 0;_tmp9BE=_tmp9BE->tl){
struct Cyc_Absyn_Enumfield*_tmp9BF=(struct Cyc_Absyn_Enumfield*)_tmp9BE->hd;if(
_tmp9BF->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9BF->tag));}}
goto _LL3FA;_LL403: {struct Cyc_Absyn_Typedef_d_struct*_tmp9B0=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp9A7;if(_tmp9B0->tag != 9)goto _LL405;else{_tmp9B1=_tmp9B0->f1;}}_LL404:{struct
Cyc_Core_Opt*_tmpDDE;_tmp9B1->defn=((_tmpDDE=_cycalloc(sizeof(*_tmpDDE)),((
_tmpDDE->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp9B1->defn))->v),_tmpDDE))));}goto _LL3FA;_LL405: {struct Cyc_Absyn_Let_d_struct*
_tmp9B2=(struct Cyc_Absyn_Let_d_struct*)_tmp9A7;if(_tmp9B2->tag != 2)goto _LL407;}
_LL406: goto _LL408;_LL407: {struct Cyc_Absyn_Letv_d_struct*_tmp9B3=(struct Cyc_Absyn_Letv_d_struct*)
_tmp9A7;if(_tmp9B3->tag != 3)goto _LL409;}_LL408: goto _LL40A;_LL409: {struct Cyc_Absyn_Datatype_d_struct*
_tmp9B4=(struct Cyc_Absyn_Datatype_d_struct*)_tmp9A7;if(_tmp9B4->tag != 7)goto
_LL40B;}_LL40A: goto _LL40C;_LL40B: {struct Cyc_Absyn_Namespace_d_struct*_tmp9B5=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp9A7;if(_tmp9B5->tag != 10)goto _LL40D;}
_LL40C: goto _LL40E;_LL40D: {struct Cyc_Absyn_Using_d_struct*_tmp9B6=(struct Cyc_Absyn_Using_d_struct*)
_tmp9A7;if(_tmp9B6->tag != 11)goto _LL40F;}_LL40E: goto _LL410;_LL40F: {struct Cyc_Absyn_ExternC_d_struct*
_tmp9B7=(struct Cyc_Absyn_ExternC_d_struct*)_tmp9A7;if(_tmp9B7->tag != 12)goto
_LL411;}_LL410: goto _LL412;_LL411: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp9B8=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9A7;if(_tmp9B8->tag != 13)
goto _LL413;}_LL412: goto _LL414;_LL413: {struct Cyc_Absyn_Region_d_struct*_tmp9B9=(
struct Cyc_Absyn_Region_d_struct*)_tmp9A7;if(_tmp9B9->tag != 4)goto _LL415;}_LL414:
goto _LL416;_LL415: {struct Cyc_Absyn_Alias_d_struct*_tmp9BA=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9A7;if(_tmp9BA->tag != 5)goto _LL417;}_LL416:{const char*_tmpDE1;void*_tmpDE0;(
_tmpDE0=0,Cyc_Tcutil_terr(d->loc,((_tmpDE1="Cyclone declaration within C code",
_tag_dyneither(_tmpDE1,sizeof(char),34))),_tag_dyneither(_tmpDE0,sizeof(void*),0)));}
goto _LL3FA;_LL417: {struct Cyc_Absyn_Porton_d_struct*_tmp9BB=(struct Cyc_Absyn_Porton_d_struct*)
_tmp9A7;if(_tmp9BB->tag != 14)goto _LL419;}_LL418: goto _LL41A;_LL419: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9BC=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9A7;if(_tmp9BC->tag != 15)goto
_LL3FA;}_LL41A: goto _LL3FA;_LL3FA:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s){void*_tmp9C3=s->r;struct Cyc_Absyn_Exp*_tmp9C5;struct Cyc_Absyn_Stmt*_tmp9C7;
struct Cyc_Absyn_Stmt*_tmp9C8;struct Cyc_Absyn_Exp*_tmp9CA;struct Cyc_Absyn_Exp*
_tmp9CC;struct Cyc_Absyn_Stmt*_tmp9CD;struct Cyc_Absyn_Stmt*_tmp9CE;struct _tuple8
_tmp9D0;struct Cyc_Absyn_Exp*_tmp9D1;struct Cyc_Absyn_Stmt*_tmp9D2;struct Cyc_Absyn_Exp*
_tmp9D4;struct _tuple8 _tmp9D5;struct Cyc_Absyn_Exp*_tmp9D6;struct _tuple8 _tmp9D7;
struct Cyc_Absyn_Exp*_tmp9D8;struct Cyc_Absyn_Stmt*_tmp9D9;struct Cyc_Absyn_Exp*
_tmp9DB;struct Cyc_List_List*_tmp9DC;struct Cyc_Absyn_Decl*_tmp9DE;struct Cyc_Absyn_Stmt*
_tmp9DF;struct Cyc_Absyn_Stmt*_tmp9E1;struct _tuple8 _tmp9E2;struct Cyc_Absyn_Exp*
_tmp9E3;_LL41C: {struct Cyc_Absyn_Exp_s_struct*_tmp9C4=(struct Cyc_Absyn_Exp_s_struct*)
_tmp9C3;if(_tmp9C4->tag != 1)goto _LL41E;else{_tmp9C5=_tmp9C4->f1;}}_LL41D: Cyc_Toc_exptypes_to_c(
_tmp9C5);goto _LL41B;_LL41E: {struct Cyc_Absyn_Seq_s_struct*_tmp9C6=(struct Cyc_Absyn_Seq_s_struct*)
_tmp9C3;if(_tmp9C6->tag != 2)goto _LL420;else{_tmp9C7=_tmp9C6->f1;_tmp9C8=_tmp9C6->f2;}}
_LL41F: Cyc_Toc_stmttypes_to_c(_tmp9C7);Cyc_Toc_stmttypes_to_c(_tmp9C8);goto
_LL41B;_LL420: {struct Cyc_Absyn_Return_s_struct*_tmp9C9=(struct Cyc_Absyn_Return_s_struct*)
_tmp9C3;if(_tmp9C9->tag != 3)goto _LL422;else{_tmp9CA=_tmp9C9->f1;}}_LL421: if(
_tmp9CA != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmp9CA);goto _LL41B;
_LL422: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp9CB=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp9C3;if(_tmp9CB->tag != 4)goto _LL424;else{_tmp9CC=_tmp9CB->f1;_tmp9CD=_tmp9CB->f2;
_tmp9CE=_tmp9CB->f3;}}_LL423: Cyc_Toc_exptypes_to_c(_tmp9CC);Cyc_Toc_stmttypes_to_c(
_tmp9CD);Cyc_Toc_stmttypes_to_c(_tmp9CE);goto _LL41B;_LL424: {struct Cyc_Absyn_While_s_struct*
_tmp9CF=(struct Cyc_Absyn_While_s_struct*)_tmp9C3;if(_tmp9CF->tag != 5)goto _LL426;
else{_tmp9D0=_tmp9CF->f1;_tmp9D1=_tmp9D0.f1;_tmp9D2=_tmp9CF->f2;}}_LL425: Cyc_Toc_exptypes_to_c(
_tmp9D1);Cyc_Toc_stmttypes_to_c(_tmp9D2);goto _LL41B;_LL426: {struct Cyc_Absyn_For_s_struct*
_tmp9D3=(struct Cyc_Absyn_For_s_struct*)_tmp9C3;if(_tmp9D3->tag != 9)goto _LL428;
else{_tmp9D4=_tmp9D3->f1;_tmp9D5=_tmp9D3->f2;_tmp9D6=_tmp9D5.f1;_tmp9D7=_tmp9D3->f3;
_tmp9D8=_tmp9D7.f1;_tmp9D9=_tmp9D3->f4;}}_LL427: Cyc_Toc_exptypes_to_c(_tmp9D4);
Cyc_Toc_exptypes_to_c(_tmp9D6);Cyc_Toc_exptypes_to_c(_tmp9D8);Cyc_Toc_stmttypes_to_c(
_tmp9D9);goto _LL41B;_LL428: {struct Cyc_Absyn_Switch_s_struct*_tmp9DA=(struct Cyc_Absyn_Switch_s_struct*)
_tmp9C3;if(_tmp9DA->tag != 10)goto _LL42A;else{_tmp9DB=_tmp9DA->f1;_tmp9DC=_tmp9DA->f2;}}
_LL429: Cyc_Toc_exptypes_to_c(_tmp9DB);for(0;_tmp9DC != 0;_tmp9DC=_tmp9DC->tl){Cyc_Toc_stmttypes_to_c(((
struct Cyc_Absyn_Switch_clause*)_tmp9DC->hd)->body);}goto _LL41B;_LL42A: {struct
Cyc_Absyn_Decl_s_struct*_tmp9DD=(struct Cyc_Absyn_Decl_s_struct*)_tmp9C3;if(
_tmp9DD->tag != 12)goto _LL42C;else{_tmp9DE=_tmp9DD->f1;_tmp9DF=_tmp9DD->f2;}}
_LL42B: Cyc_Toc_decltypes_to_c(_tmp9DE);Cyc_Toc_stmttypes_to_c(_tmp9DF);goto
_LL41B;_LL42C: {struct Cyc_Absyn_Do_s_struct*_tmp9E0=(struct Cyc_Absyn_Do_s_struct*)
_tmp9C3;if(_tmp9E0->tag != 14)goto _LL42E;else{_tmp9E1=_tmp9E0->f1;_tmp9E2=_tmp9E0->f2;
_tmp9E3=_tmp9E2.f1;}}_LL42D: Cyc_Toc_stmttypes_to_c(_tmp9E1);Cyc_Toc_exptypes_to_c(
_tmp9E3);goto _LL41B;_LL42E: {struct Cyc_Absyn_Skip_s_struct*_tmp9E4=(struct Cyc_Absyn_Skip_s_struct*)
_tmp9C3;if(_tmp9E4->tag != 0)goto _LL430;}_LL42F: goto _LL431;_LL430: {struct Cyc_Absyn_Break_s_struct*
_tmp9E5=(struct Cyc_Absyn_Break_s_struct*)_tmp9C3;if(_tmp9E5->tag != 6)goto _LL432;}
_LL431: goto _LL433;_LL432: {struct Cyc_Absyn_Continue_s_struct*_tmp9E6=(struct Cyc_Absyn_Continue_s_struct*)
_tmp9C3;if(_tmp9E6->tag != 7)goto _LL434;}_LL433: goto _LL435;_LL434: {struct Cyc_Absyn_Goto_s_struct*
_tmp9E7=(struct Cyc_Absyn_Goto_s_struct*)_tmp9C3;if(_tmp9E7->tag != 8)goto _LL436;}
_LL435: goto _LL41B;_LL436: {struct Cyc_Absyn_Fallthru_s_struct*_tmp9E8=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp9C3;if(_tmp9E8->tag != 11)goto _LL438;}_LL437: goto _LL439;_LL438: {struct Cyc_Absyn_Label_s_struct*
_tmp9E9=(struct Cyc_Absyn_Label_s_struct*)_tmp9C3;if(_tmp9E9->tag != 13)goto _LL43A;}
_LL439: goto _LL43B;_LL43A: {struct Cyc_Absyn_TryCatch_s_struct*_tmp9EA=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp9C3;if(_tmp9EA->tag != 15)goto _LL43C;}_LL43B: goto _LL43D;_LL43C: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp9EB=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp9C3;if(_tmp9EB->tag != 16)goto
_LL41B;}_LL43D:{const char*_tmpDE4;void*_tmpDE3;(_tmpDE3=0,Cyc_Tcutil_terr(s->loc,((
_tmpDE4="Cyclone statement in C code",_tag_dyneither(_tmpDE4,sizeof(char),28))),
_tag_dyneither(_tmpDE3,sizeof(void*),0)));}goto _LL41B;_LL41B:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude){for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpDE7;void*_tmpDE6;(_tmpDE6=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDE7="decls_to_c: not at toplevel!",
_tag_dyneither(_tmpDE7,sizeof(char),29))),_tag_dyneither(_tmpDE6,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmp9F0=d->r;struct Cyc_Absyn_Vardecl*
_tmp9F2;struct Cyc_Absyn_Fndecl*_tmp9F4;struct Cyc_Absyn_Aggrdecl*_tmp9FA;struct
Cyc_Absyn_Datatypedecl*_tmp9FC;struct Cyc_Absyn_Enumdecl*_tmp9FE;struct Cyc_Absyn_Typedefdecl*
_tmpA00;struct Cyc_List_List*_tmpA04;struct Cyc_List_List*_tmpA06;struct Cyc_List_List*
_tmpA08;struct Cyc_List_List*_tmpA0A;_LL43F: {struct Cyc_Absyn_Var_d_struct*
_tmp9F1=(struct Cyc_Absyn_Var_d_struct*)_tmp9F0;if(_tmp9F1->tag != 0)goto _LL441;
else{_tmp9F2=_tmp9F1->f1;}}_LL440: {struct _tuple0*_tmpA0B=_tmp9F2->name;if(
_tmp9F2->sc == Cyc_Absyn_ExternC){struct _tuple0*_tmpDE8;_tmpA0B=((_tmpDE8=
_cycalloc(sizeof(*_tmpDE8)),((_tmpDE8->f1=Cyc_Absyn_Rel_n(0),((_tmpDE8->f2=(*
_tmpA0B).f2,_tmpDE8))))));}if(_tmp9F2->initializer != 0){if(cinclude)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmp9F2->initializer));else{Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_check_null(_tmp9F2->initializer));}}{struct Cyc_Absyn_Global_b_struct
_tmpDEB;struct Cyc_Absyn_Global_b_struct*_tmpDEA;nv=Cyc_Toc_add_varmap(r,nv,
_tmp9F2->name,Cyc_Absyn_varb_exp(_tmpA0B,(void*)((_tmpDEA=_cycalloc(sizeof(*
_tmpDEA)),((_tmpDEA[0]=((_tmpDEB.tag=1,((_tmpDEB.f1=_tmp9F2,_tmpDEB)))),_tmpDEA)))),
0));}_tmp9F2->name=_tmpA0B;_tmp9F2->sc=Cyc_Toc_scope_to_c(_tmp9F2->sc);_tmp9F2->type=
Cyc_Toc_typ_to_c_array(_tmp9F2->type);{struct Cyc_List_List*_tmpDEC;Cyc_Toc_result_decls=((
_tmpDEC=_cycalloc(sizeof(*_tmpDEC)),((_tmpDEC->hd=d,((_tmpDEC->tl=Cyc_Toc_result_decls,
_tmpDEC))))));}goto _LL43E;}_LL441: {struct Cyc_Absyn_Fn_d_struct*_tmp9F3=(struct
Cyc_Absyn_Fn_d_struct*)_tmp9F0;if(_tmp9F3->tag != 1)goto _LL443;else{_tmp9F4=
_tmp9F3->f1;}}_LL442: {struct _tuple0*_tmpA10=_tmp9F4->name;if(_tmp9F4->sc == Cyc_Absyn_ExternC){{
struct _tuple0*_tmpDED;_tmpA10=((_tmpDED=_cycalloc(sizeof(*_tmpDED)),((_tmpDED->f1=
Cyc_Absyn_Rel_n(0),((_tmpDED->f2=(*_tmpA10).f2,_tmpDED))))));}_tmp9F4->sc=Cyc_Absyn_Public;}
nv=Cyc_Toc_add_varmap(r,nv,_tmp9F4->name,Cyc_Absyn_var_exp(_tmpA10,0));_tmp9F4->name=
_tmpA10;Cyc_Toc_fndecl_to_c(nv,_tmp9F4,cinclude);{struct Cyc_List_List*_tmpDEE;
Cyc_Toc_result_decls=((_tmpDEE=_cycalloc(sizeof(*_tmpDEE)),((_tmpDEE->hd=d,((
_tmpDEE->tl=Cyc_Toc_result_decls,_tmpDEE))))));}goto _LL43E;}_LL443: {struct Cyc_Absyn_Let_d_struct*
_tmp9F5=(struct Cyc_Absyn_Let_d_struct*)_tmp9F0;if(_tmp9F5->tag != 2)goto _LL445;}
_LL444: goto _LL446;_LL445: {struct Cyc_Absyn_Letv_d_struct*_tmp9F6=(struct Cyc_Absyn_Letv_d_struct*)
_tmp9F0;if(_tmp9F6->tag != 3)goto _LL447;}_LL446: {const char*_tmpDF1;void*_tmpDF0;(
_tmpDF0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDF1="letdecl at toplevel",_tag_dyneither(_tmpDF1,sizeof(char),20))),
_tag_dyneither(_tmpDF0,sizeof(void*),0)));}_LL447: {struct Cyc_Absyn_Region_d_struct*
_tmp9F7=(struct Cyc_Absyn_Region_d_struct*)_tmp9F0;if(_tmp9F7->tag != 4)goto _LL449;}
_LL448: {const char*_tmpDF4;void*_tmpDF3;(_tmpDF3=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDF4="region decl at toplevel",
_tag_dyneither(_tmpDF4,sizeof(char),24))),_tag_dyneither(_tmpDF3,sizeof(void*),0)));}
_LL449: {struct Cyc_Absyn_Alias_d_struct*_tmp9F8=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9F0;if(_tmp9F8->tag != 5)goto _LL44B;}_LL44A: {const char*_tmpDF7;void*_tmpDF6;(
_tmpDF6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDF7="alias decl at toplevel",_tag_dyneither(_tmpDF7,sizeof(char),23))),
_tag_dyneither(_tmpDF6,sizeof(void*),0)));}_LL44B: {struct Cyc_Absyn_Aggr_d_struct*
_tmp9F9=(struct Cyc_Absyn_Aggr_d_struct*)_tmp9F0;if(_tmp9F9->tag != 6)goto _LL44D;
else{_tmp9FA=_tmp9F9->f1;}}_LL44C: Cyc_Toc_aggrdecl_to_c(_tmp9FA);goto _LL43E;
_LL44D: {struct Cyc_Absyn_Datatype_d_struct*_tmp9FB=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp9F0;if(_tmp9FB->tag != 7)goto _LL44F;else{_tmp9FC=_tmp9FB->f1;}}_LL44E: if(
_tmp9FC->is_extensible)Cyc_Toc_xdatatypedecl_to_c(_tmp9FC);else{Cyc_Toc_datatypedecl_to_c(
_tmp9FC);}goto _LL43E;_LL44F: {struct Cyc_Absyn_Enum_d_struct*_tmp9FD=(struct Cyc_Absyn_Enum_d_struct*)
_tmp9F0;if(_tmp9FD->tag != 8)goto _LL451;else{_tmp9FE=_tmp9FD->f1;}}_LL450: Cyc_Toc_enumdecl_to_c(
nv,_tmp9FE);{struct Cyc_List_List*_tmpDF8;Cyc_Toc_result_decls=((_tmpDF8=
_cycalloc(sizeof(*_tmpDF8)),((_tmpDF8->hd=d,((_tmpDF8->tl=Cyc_Toc_result_decls,
_tmpDF8))))));}goto _LL43E;_LL451: {struct Cyc_Absyn_Typedef_d_struct*_tmp9FF=(
struct Cyc_Absyn_Typedef_d_struct*)_tmp9F0;if(_tmp9FF->tag != 9)goto _LL453;else{
_tmpA00=_tmp9FF->f1;}}_LL452: _tmpA00->name=_tmpA00->name;_tmpA00->tvs=0;if(
_tmpA00->defn != 0){struct Cyc_Core_Opt*_tmpDF9;_tmpA00->defn=((_tmpDF9=_cycalloc(
sizeof(*_tmpDF9)),((_tmpDF9->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA00->defn))->v),_tmpDF9))));}else{switch(((struct Cyc_Absyn_Kind*)((
struct Cyc_Core_Opt*)_check_null(_tmpA00->kind))->v)->kind){case Cyc_Absyn_BoxKind:
_LL45F:{struct Cyc_Core_Opt*_tmpDFA;_tmpA00->defn=((_tmpDFA=_cycalloc(sizeof(*
_tmpDFA)),((_tmpDFA->v=Cyc_Absyn_void_star_typ(),_tmpDFA))));}break;default:
_LL460:{struct Cyc_Core_Opt*_tmpDFB;_tmpA00->defn=((_tmpDFB=_cycalloc(sizeof(*
_tmpDFB)),((_tmpDFB->v=(void*)& Cyc_Absyn_VoidType_val,_tmpDFB))));}break;}}{
struct Cyc_List_List*_tmpDFC;Cyc_Toc_result_decls=((_tmpDFC=_cycalloc(sizeof(*
_tmpDFC)),((_tmpDFC->hd=d,((_tmpDFC->tl=Cyc_Toc_result_decls,_tmpDFC))))));}goto
_LL43E;_LL453: {struct Cyc_Absyn_Porton_d_struct*_tmpA01=(struct Cyc_Absyn_Porton_d_struct*)
_tmp9F0;if(_tmpA01->tag != 14)goto _LL455;}_LL454: goto _LL456;_LL455: {struct Cyc_Absyn_Portoff_d_struct*
_tmpA02=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9F0;if(_tmpA02->tag != 15)goto
_LL457;}_LL456: goto _LL43E;_LL457: {struct Cyc_Absyn_Namespace_d_struct*_tmpA03=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp9F0;if(_tmpA03->tag != 10)goto _LL459;
else{_tmpA04=_tmpA03->f2;}}_LL458: _tmpA06=_tmpA04;goto _LL45A;_LL459: {struct Cyc_Absyn_Using_d_struct*
_tmpA05=(struct Cyc_Absyn_Using_d_struct*)_tmp9F0;if(_tmpA05->tag != 11)goto _LL45B;
else{_tmpA06=_tmpA05->f2;}}_LL45A: _tmpA08=_tmpA06;goto _LL45C;_LL45B: {struct Cyc_Absyn_ExternC_d_struct*
_tmpA07=(struct Cyc_Absyn_ExternC_d_struct*)_tmp9F0;if(_tmpA07->tag != 12)goto
_LL45D;else{_tmpA08=_tmpA07->f1;}}_LL45C: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA08,top,
cinclude);goto _LL43E;_LL45D: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA09=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9F0;if(_tmpA09->tag != 13)goto _LL43E;
else{_tmpA0A=_tmpA09->f1;}}_LL45E: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA0A,top,1);goto
_LL43E;_LL43E:;};}return nv;}static void Cyc_Toc_init(){struct _DynRegionHandle*
_tmpA1F;struct Cyc_Core_NewRegion _tmpA1E=Cyc_Core_new_dynregion();_tmpA1F=_tmpA1E.dynregion;{
struct _DynRegionFrame _tmpA20;struct _RegionHandle*d=_open_dynregion(& _tmpA20,
_tmpA1F);{struct Cyc_Dict_Dict*_tmpE07;struct Cyc_Dict_Dict*_tmpE06;struct Cyc_Set_Set**
_tmpE05;struct Cyc_Dict_Dict*_tmpE04;struct Cyc_List_List**_tmpE03;struct Cyc_Toc_TocState*
_tmpE02;Cyc_Toc_toc_state=((_tmpE02=_cycalloc(sizeof(*_tmpE02)),((_tmpE02->dyn=(
struct _DynRegionHandle*)_tmpA1F,((_tmpE02->tuple_types=(struct Cyc_List_List**)((
_tmpE03=_region_malloc(d,sizeof(*_tmpE03)),((_tmpE03[0]=0,_tmpE03)))),((_tmpE02->aggrs_so_far=(
struct Cyc_Dict_Dict*)((_tmpE04=_region_malloc(d,sizeof(*_tmpE04)),((_tmpE04[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE04)))),((_tmpE02->datatypes_so_far=(
struct Cyc_Set_Set**)((_tmpE05=_region_malloc(d,sizeof(*_tmpE05)),((_tmpE05[0]=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE05)))),((_tmpE02->xdatatypes_so_far=(
struct Cyc_Dict_Dict*)((_tmpE06=_region_malloc(d,sizeof(*_tmpE06)),((_tmpE06[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE06)))),((_tmpE02->qvar_tags=(
struct Cyc_Dict_Dict*)((_tmpE07=_region_malloc(d,sizeof(*_tmpE07)),((_tmpE07[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple13*,struct
_tuple13*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpE07)))),((_tmpE02->temp_labels=(
struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(
d),_tmpE02))))))))))))))));}Cyc_Toc_result_decls=0;Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=0;Cyc_Toc_total_bounds_checks=
0;Cyc_Toc_bounds_checks_eliminated=0;{struct _dyneither_ptr**_tmpE08;Cyc_Toc_globals=
_tag_dyneither(((_tmpE08=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpE08[
0]=& Cyc_Toc__throw_str,((_tmpE08[1]=& Cyc_Toc_setjmp_str,((_tmpE08[2]=& Cyc_Toc__push_handler_str,((
_tmpE08[3]=& Cyc_Toc__pop_handler_str,((_tmpE08[4]=& Cyc_Toc__exn_thrown_str,((
_tmpE08[5]=& Cyc_Toc__npop_handler_str,((_tmpE08[6]=& Cyc_Toc__check_null_str,((
_tmpE08[7]=& Cyc_Toc__check_known_subscript_null_str,((_tmpE08[8]=& Cyc_Toc__check_known_subscript_notnull_str,((
_tmpE08[9]=& Cyc_Toc__check_dyneither_subscript_str,((_tmpE08[10]=& Cyc_Toc__dyneither_ptr_str,((
_tmpE08[11]=& Cyc_Toc__tag_dyneither_str,((_tmpE08[12]=& Cyc_Toc__init_dyneither_ptr_str,((
_tmpE08[13]=& Cyc_Toc__untag_dyneither_ptr_str,((_tmpE08[14]=& Cyc_Toc__get_dyneither_size_str,((
_tmpE08[15]=& Cyc_Toc__get_zero_arr_size_str,((_tmpE08[16]=& Cyc_Toc__dyneither_ptr_plus_str,((
_tmpE08[17]=& Cyc_Toc__zero_arr_plus_str,((_tmpE08[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((
_tmpE08[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpE08[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((
_tmpE08[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpE08[22]=& Cyc_Toc__cycalloc_str,((
_tmpE08[23]=& Cyc_Toc__cyccalloc_str,((_tmpE08[24]=& Cyc_Toc__cycalloc_atomic_str,((
_tmpE08[25]=& Cyc_Toc__cyccalloc_atomic_str,((_tmpE08[26]=& Cyc_Toc__region_malloc_str,((
_tmpE08[27]=& Cyc_Toc__region_calloc_str,((_tmpE08[28]=& Cyc_Toc__check_times_str,((
_tmpE08[29]=& Cyc_Toc__new_region_str,((_tmpE08[30]=& Cyc_Toc__push_region_str,((
_tmpE08[31]=& Cyc_Toc__pop_region_str,((_tmpE08[32]=& Cyc_Toc__open_dynregion_str,((
_tmpE08[33]=& Cyc_Toc__push_dynregion_str,((_tmpE08[34]=& Cyc_Toc__pop_dynregion_str,((
_tmpE08[35]=& Cyc_Toc__reset_region_str,((_tmpE08[36]=& Cyc_Toc__throw_arraybounds_str,((
_tmpE08[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpE08[38]=& Cyc_Toc__throw_match_str,((
_tmpE08[39]=& Cyc_Toc__swap_word_str,((_tmpE08[40]=& Cyc_Toc__swap_dyneither_str,
_tmpE08)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),41);};;_pop_dynregion(d);};}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();{struct _RegionHandle _tmpA28=
_new_region("start");struct _RegionHandle*start=& _tmpA28;_push_region(start);Cyc_Toc_decls_to_c(
start,Cyc_Toc_empty_env(start),ds,1,0);{struct _DynRegionHandle*_tmpA2A;struct Cyc_Toc_TocState
_tmpA29=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA2A=
_tmpA29.dyn;Cyc_Core_free_dynregion(_tmpA2A);};{struct Cyc_List_List*_tmpA2B=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);
_npop_handler(0);return _tmpA2B;};;_pop_region(start);};}
