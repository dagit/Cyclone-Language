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
int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*
ds);struct _tuple0*Cyc_Toc_temp_var();extern struct _dyneither_ptr Cyc_Toc_globals;
extern int Cyc_noexpand_r;int Cyc_Toc_warn_bounds_checks=0;int Cyc_Toc_warn_all_null_deref=
0;unsigned int Cyc_Toc_total_bounds_checks=0;unsigned int Cyc_Toc_bounds_checks_eliminated=
0;static struct Cyc_List_List*Cyc_Toc_result_decls=0;struct Cyc_Toc_TocState{struct
_DynRegionHandle*dyn;struct Cyc_List_List**tuple_types;struct Cyc_Dict_Dict*
aggrs_so_far;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*
xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*
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
ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const char*_tmpA6C;void*_tmpA6B;(_tmpA6B=0,
Cyc_fprintf(Cyc_stderr,((_tmpA6C="\n",_tag_dyneither(_tmpA6C,sizeof(char),2))),
_tag_dyneither(_tmpA6B,sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_struct _tmpA6F;struct Cyc_Toc_Toc_Unimplemented_struct*
_tmpA6E;(int)_throw((void*)((_tmpA6E=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpA6E)),((_tmpA6E[0]=((_tmpA6F.tag=Cyc_Toc_Toc_Unimplemented,_tmpA6F)),
_tmpA6E)))));};}static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const char*_tmpA72;void*
_tmpA71;(_tmpA71=0,Cyc_fprintf(Cyc_stderr,((_tmpA72="\n",_tag_dyneither(_tmpA72,
sizeof(char),2))),_tag_dyneither(_tmpA71,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Impossible_struct _tmpA75;struct Cyc_Toc_Toc_Impossible_struct*
_tmpA74;(int)_throw((void*)((_tmpA74=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpA74)),((_tmpA74[0]=((_tmpA75.tag=Cyc_Toc_Toc_Impossible,_tmpA75)),
_tmpA74)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_struct{
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
_tmpA76;skip_stmt_opt=((_tmpA76=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpA76)),((_tmpA76[0]=Cyc_Absyn_skip_stmt(0),_tmpA76))));}return*skip_stmt_opt;}
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){return
Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}static void*Cyc_Toc_cast_it_r(
void*t,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Cast_e_struct _tmpA79;struct Cyc_Absyn_Cast_e_struct*
_tmpA78;return(void*)((_tmpA78=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpA78)),((_tmpA78[0]=((_tmpA79.tag=15,((_tmpA79.f1=(void*)t,((_tmpA79.f2=e,((
_tmpA79.f3=0,((_tmpA79.f4=Cyc_Absyn_No_coercion,_tmpA79)))))))))),_tmpA78))));}
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Deref_e_struct
_tmpA7C;struct Cyc_Absyn_Deref_e_struct*_tmpA7B;return(void*)((_tmpA7B=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA7B)),((_tmpA7B[0]=((_tmpA7C.tag=
21,((_tmpA7C.f1=e,_tmpA7C)))),_tmpA7B))));}static void*Cyc_Toc_subscript_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct Cyc_Absyn_Subscript_e_struct
_tmpA7F;struct Cyc_Absyn_Subscript_e_struct*_tmpA7E;return(void*)((_tmpA7E=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA7E)),((_tmpA7E[0]=((_tmpA7F.tag=
24,((_tmpA7F.f1=e1,((_tmpA7F.f2=e2,_tmpA7F)))))),_tmpA7E))));}static void*Cyc_Toc_stmt_exp_r(
struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct _tmpA82;struct Cyc_Absyn_StmtExp_e_struct*
_tmpA81;return(void*)((_tmpA81=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpA81)),((_tmpA81[0]=((_tmpA82.tag=37,((_tmpA82.f1=s,_tmpA82)))),_tmpA81))));}
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpA85;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpA84;return(void*)((_tmpA84=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA84)),((_tmpA84[0]=((_tmpA85.tag=
18,((_tmpA85.f1=(void*)t,_tmpA85)))),_tmpA84))));}static void*Cyc_Toc_fncall_exp_r(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct
_tmpA88;struct Cyc_Absyn_FnCall_e_struct*_tmpA87;return(void*)((_tmpA87=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA87)),((_tmpA87[0]=((_tmpA88.tag=
11,((_tmpA88.f1=e,((_tmpA88.f2=es,((_tmpA88.f3=0,_tmpA88)))))))),_tmpA87))));}
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_struct
_tmpA8B;struct Cyc_Absyn_Exp_s_struct*_tmpA8A;return(void*)((_tmpA8A=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA8A)),((_tmpA8A[0]=((_tmpA8B.tag=
1,((_tmpA8B.f1=e,_tmpA8B)))),_tmpA8A))));}static void*Cyc_Toc_seq_stmt_r(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){struct Cyc_Absyn_Seq_s_struct _tmpA8E;
struct Cyc_Absyn_Seq_s_struct*_tmpA8D;return(void*)((_tmpA8D=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpA8D)),((_tmpA8D[0]=((_tmpA8E.tag=2,((_tmpA8E.f1=s1,((_tmpA8E.f2=s2,
_tmpA8E)))))),_tmpA8D))));}static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct
_tmpA91;struct Cyc_Absyn_Conditional_e_struct*_tmpA90;return(void*)((_tmpA90=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA90)),((_tmpA90[0]=((_tmpA91.tag=
6,((_tmpA91.f1=e1,((_tmpA91.f2=e2,((_tmpA91.f3=e3,_tmpA91)))))))),_tmpA90))));}
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrMember_e_struct _tmpA94;struct Cyc_Absyn_AggrMember_e_struct*
_tmpA93;return(void*)((_tmpA93=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpA93)),((_tmpA93[0]=((_tmpA94.tag=22,((_tmpA94.f1=e,((_tmpA94.f2=n,((_tmpA94.f3=
0,((_tmpA94.f4=0,_tmpA94)))))))))),_tmpA93))));}static void*Cyc_Toc_aggrarrow_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrArrow_e_struct
_tmpA97;struct Cyc_Absyn_AggrArrow_e_struct*_tmpA96;return(void*)((_tmpA96=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA96)),((_tmpA96[0]=((_tmpA97.tag=
23,((_tmpA97.f1=e,((_tmpA97.f2=n,((_tmpA97.f3=0,((_tmpA97.f4=0,_tmpA97)))))))))),
_tmpA96))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,
struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmpA9A;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpA99;return(void*)((_tmpA99=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpA99)),((_tmpA99[0]=((_tmpA9A.tag=36,((_tmpA9A.f1=tdopt,((_tmpA9A.f2=ds,
_tmpA9A)))))),_tmpA99))));}static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,
struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_struct _tmpA9D;struct Cyc_Absyn_Goto_s_struct*
_tmpA9C;return(void*)((_tmpA9C=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpA9C)),((_tmpA9C[0]=((_tmpA9D.tag=8,((_tmpA9D.f1=v,((_tmpA9D.f2=s,_tmpA9D)))))),
_tmpA9C))));}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,{.Int_c={4,{
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
_tmpAA3;const char*_tmpAA2;struct Cyc_Core_Impossible_struct*_tmpAA1;(int)_throw((
void*)((_tmpAA1=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAA1)),((
_tmpAA1[0]=((_tmpAA3.tag=Cyc_Core_Impossible,((_tmpAA3.f1=((_tmpAA2="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpAA2,sizeof(char),44))),_tmpAA3)))),_tmpAA1)))));}}goto _LL0;
_LL3: {struct Cyc_Absyn_FloatType_struct*_tmpC9=(struct Cyc_Absyn_FloatType_struct*)
_tmpC6;if(_tmpC9->tag != 7)goto _LL5;}_LL4: function=fS->ffloat;goto _LL0;_LL5: {
struct Cyc_Absyn_DoubleType_struct*_tmpCA=(struct Cyc_Absyn_DoubleType_struct*)
_tmpC6;if(_tmpCA->tag != 8)goto _LL7;else{_tmpCB=_tmpCA->f1;}}_LL6: switch(_tmpCB){
case 1: _LL10: function=fS->flongdouble;break;default: _LL11: function=fS->fdouble;}
goto _LL0;_LL7: {struct Cyc_Absyn_PointerType_struct*_tmpCC=(struct Cyc_Absyn_PointerType_struct*)
_tmpC6;if(_tmpCC->tag != 5)goto _LL9;}_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;
_LLA: {struct Cyc_Core_Impossible_struct _tmpAB0;const char*_tmpAAF;void*_tmpAAE[1];
struct Cyc_String_pa_struct _tmpAAD;struct Cyc_Core_Impossible_struct*_tmpAAC;(int)
_throw((void*)((_tmpAAC=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAAC)),((
_tmpAAC[0]=((_tmpAB0.tag=Cyc_Core_Impossible,((_tmpAB0.f1=(struct _dyneither_ptr)((
_tmpAAD.tag=0,((_tmpAAD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmpAAE[0]=& _tmpAAD,Cyc_aprintf(((_tmpAAF="impossible expression type %s (not int, float, double, or pointer)",
_tag_dyneither(_tmpAAF,sizeof(char),67))),_tag_dyneither(_tmpAAE,sizeof(void*),1)))))))),
_tmpAB0)))),_tmpAAC)))));}_LL0:;}return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(
fS,(void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v);}struct Cyc_Absyn_Exp*
Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*
arr){void*_tmpD5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
arr->topt))->v);struct Cyc_Absyn_PtrInfo _tmpD7;void*_tmpD8;_LL14: {struct Cyc_Absyn_PointerType_struct*
_tmpD6=(struct Cyc_Absyn_PointerType_struct*)_tmpD5;if(_tmpD6->tag != 5)goto _LL16;
else{_tmpD7=_tmpD6->f1;_tmpD8=_tmpD7.elt_typ;}}_LL15: return Cyc_Toc_getFunctionType(
fS,_tmpD8);_LL16:;_LL17: {struct Cyc_Core_Impossible_struct _tmpAB6;const char*
_tmpAB5;struct Cyc_Core_Impossible_struct*_tmpAB4;(int)_throw((void*)((_tmpAB4=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAB4)),((_tmpAB4[0]=((_tmpAB6.tag=
Cyc_Core_Impossible,((_tmpAB6.f1=((_tmpAB5="impossible type (not pointer)",
_tag_dyneither(_tmpAB5,sizeof(char),30))),_tmpAB6)))),_tmpAB4)))));}_LL13:;}
struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmpDC=e->r;union Cyc_Absyn_Cnst _tmpDE;struct _tuple3
_tmpDF;char _tmpE0;union Cyc_Absyn_Cnst _tmpE2;struct _tuple4 _tmpE3;short _tmpE4;
union Cyc_Absyn_Cnst _tmpE6;struct _tuple5 _tmpE7;int _tmpE8;union Cyc_Absyn_Cnst
_tmpEA;struct _tuple6 _tmpEB;long long _tmpEC;union Cyc_Absyn_Cnst _tmpEE;int _tmpEF;
struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_List_List*_tmpF3;struct Cyc_List_List*_tmpF5;
struct Cyc_List_List*_tmpF7;struct Cyc_List_List*_tmpF9;struct Cyc_List_List*_tmpFB;
_LL19: {struct Cyc_Absyn_Const_e_struct*_tmpDD=(struct Cyc_Absyn_Const_e_struct*)
_tmpDC;if(_tmpDD->tag != 0)goto _LL1B;else{_tmpDE=_tmpDD->f1;if((_tmpDE.Char_c).tag
!= 2)goto _LL1B;_tmpDF=(struct _tuple3)(_tmpDE.Char_c).val;_tmpE0=_tmpDF.f2;}}
_LL1A: return _tmpE0 == '\000';_LL1B: {struct Cyc_Absyn_Const_e_struct*_tmpE1=(
struct Cyc_Absyn_Const_e_struct*)_tmpDC;if(_tmpE1->tag != 0)goto _LL1D;else{_tmpE2=
_tmpE1->f1;if((_tmpE2.Short_c).tag != 3)goto _LL1D;_tmpE3=(struct _tuple4)(_tmpE2.Short_c).val;
_tmpE4=_tmpE3.f2;}}_LL1C: return _tmpE4 == 0;_LL1D: {struct Cyc_Absyn_Const_e_struct*
_tmpE5=(struct Cyc_Absyn_Const_e_struct*)_tmpDC;if(_tmpE5->tag != 0)goto _LL1F;
else{_tmpE6=_tmpE5->f1;if((_tmpE6.Int_c).tag != 4)goto _LL1F;_tmpE7=(struct _tuple5)(
_tmpE6.Int_c).val;_tmpE8=_tmpE7.f2;}}_LL1E: return _tmpE8 == 0;_LL1F: {struct Cyc_Absyn_Const_e_struct*
_tmpE9=(struct Cyc_Absyn_Const_e_struct*)_tmpDC;if(_tmpE9->tag != 0)goto _LL21;
else{_tmpEA=_tmpE9->f1;if((_tmpEA.LongLong_c).tag != 5)goto _LL21;_tmpEB=(struct
_tuple6)(_tmpEA.LongLong_c).val;_tmpEC=_tmpEB.f2;}}_LL20: return _tmpEC == 0;_LL21: {
struct Cyc_Absyn_Const_e_struct*_tmpED=(struct Cyc_Absyn_Const_e_struct*)_tmpDC;
if(_tmpED->tag != 0)goto _LL23;else{_tmpEE=_tmpED->f1;if((_tmpEE.Null_c).tag != 1)
goto _LL23;_tmpEF=(int)(_tmpEE.Null_c).val;}}_LL22: return 1;_LL23: {struct Cyc_Absyn_Cast_e_struct*
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
Cyc_Absyn_conref_def)(0,_tmp100);_LL34:;_LL35: {const char*_tmpAB9;void*_tmpAB8;(
_tmpAB8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAB9="is_nullable",_tag_dyneither(_tmpAB9,sizeof(char),12))),_tag_dyneither(
_tmpAB8,sizeof(void*),0)));}_LL31:;}static char _tmp10F[1]="";static char _tmp110[8]="*bogus*";
static struct _tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct
_dyneither_ptr tag){struct _DynRegionHandle*_tmp104;struct Cyc_Dict_Dict*_tmp105;
struct Cyc_Toc_TocState _tmp103=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp104=_tmp103.dyn;_tmp105=_tmp103.qvar_tags;{static struct _dyneither_ptr
bogus_string={_tmp110,_tmp110,_tmp110 + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
4,0}},& bogus_string};static struct _tuple13 pair={& bogus_qvar,{_tmp10F,_tmp10F,
_tmp10F + 1}};{struct _tuple13 _tmpABA;pair=((_tmpABA.f1=x,((_tmpABA.f2=tag,_tmpABA))));}{
struct _DynRegionFrame _tmp107;struct _RegionHandle*d=_open_dynregion(& _tmp107,
_tmp104);{struct _tuple0**_tmp108=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,
struct _tuple13*k))Cyc_Dict_lookup_opt)(*_tmp105,(struct _tuple13*)& pair);if(
_tmp108 != 0){struct _tuple0*_tmp109=*_tmp108;_npop_handler(0);return _tmp109;}{
struct _tuple13*_tmpABB;struct _tuple13*_tmp10A=(_tmpABB=_cycalloc(sizeof(*_tmpABB)),((
_tmpABB->f1=x,((_tmpABB->f2=tag,_tmpABB)))));struct _dyneither_ptr*_tmpABE;struct
_tuple0*_tmpABD;struct _tuple0*res=(_tmpABD=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpABD)),((_tmpABD->f1=(*x).f1,((_tmpABD->f2=((_tmpABE=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmpABE)),((_tmpABE[0]=(struct _dyneither_ptr)Cyc_strconcat((
struct _dyneither_ptr)*(*x).f2,(struct _dyneither_ptr)tag),_tmpABE)))),_tmpABD)))));*
_tmp105=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple13*k,struct
_tuple0*v))Cyc_Dict_insert)(*_tmp105,(struct _tuple13*)_tmp10A,res);{struct
_tuple0*_tmp10B=res;_npop_handler(0);return _tmp10B;};};};_pop_dynregion(d);};};}
struct _tuple16{void*f1;struct Cyc_List_List*f2;};static void*Cyc_Toc_add_tuple_type(
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
_npop_handler(0);return _tmp11B;}};}}{struct Cyc_Int_pa_struct _tmpAC6;void*_tmpAC5[
1];const char*_tmpAC4;struct _dyneither_ptr*_tmpAC3;struct _dyneither_ptr*xname=(
_tmpAC3=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAC3)),((_tmpAC3[0]=(
struct _dyneither_ptr)((_tmpAC6.tag=1,((_tmpAC6.f1=(unsigned long)Cyc_Toc_tuple_type_counter
++,((_tmpAC5[0]=& _tmpAC6,Cyc_aprintf(((_tmpAC4="_tuple%d",_tag_dyneither(_tmpAC4,
sizeof(char),9))),_tag_dyneither(_tmpAC5,sizeof(void*),1)))))))),_tmpAC3)));void*
x=Cyc_Absyn_strct(xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_rmap)(d,
Cyc_Tcutil_snd_tqt,tqs0);struct Cyc_List_List*_tmp11C=0;struct Cyc_List_List*ts2=
ts;{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmpAC9;
struct Cyc_List_List*_tmpAC8;_tmp11C=((_tmpAC8=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpAC8)),((_tmpAC8->hd=((_tmpAC9=_cycalloc(sizeof(*_tmpAC9)),((_tmpAC9->name=
Cyc_Absyn_fieldname(i),((_tmpAC9->tq=Cyc_Toc_mt_tq,((_tmpAC9->type=(void*)ts2->hd,((
_tmpAC9->width=0,((_tmpAC9->attributes=0,_tmpAC9)))))))))))),((_tmpAC8->tl=
_tmp11C,_tmpAC8))))));}}_tmp11C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp11C);{struct Cyc_Absyn_AggrdeclImpl*_tmpACE;struct _tuple0*
_tmpACD;struct Cyc_Absyn_Aggrdecl*_tmpACC;struct Cyc_Absyn_Aggrdecl*_tmp11F=(
_tmpACC=_cycalloc(sizeof(*_tmpACC)),((_tmpACC->kind=Cyc_Absyn_StructA,((_tmpACC->sc=
Cyc_Absyn_Public,((_tmpACC->name=((_tmpACD=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpACD)),((_tmpACD->f1=Cyc_Absyn_Rel_n(0),((_tmpACD->f2=xname,_tmpACD)))))),((
_tmpACC->tvs=0,((_tmpACC->impl=((_tmpACE=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpACE)),((_tmpACE->exist_vars=0,((_tmpACE->rgn_po=0,((_tmpACE->fields=
_tmp11C,((_tmpACE->tagged=0,_tmpACE)))))))))),((_tmpACC->attributes=0,_tmpACC)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmpAD4;struct Cyc_Absyn_Aggr_d_struct _tmpAD3;
struct Cyc_List_List*_tmpAD2;Cyc_Toc_result_decls=((_tmpAD2=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpAD2)),((_tmpAD2->hd=Cyc_Absyn_new_decl((void*)((_tmpAD4=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAD4)),((_tmpAD4[0]=((_tmpAD3.tag=
6,((_tmpAD3.f1=_tmp11F,_tmpAD3)))),_tmpAD4)))),0),((_tmpAD2->tl=Cyc_Toc_result_decls,
_tmpAD2))))));}{struct _tuple16*_tmpAD7;struct Cyc_List_List*_tmpAD6;*_tmp113=((
_tmpAD6=_region_malloc(d,sizeof(*_tmpAD6)),((_tmpAD6->hd=((_tmpAD7=
_region_malloc(d,sizeof(*_tmpAD7)),((_tmpAD7->f1=x,((_tmpAD7->f2=ts,_tmpAD7)))))),((
_tmpAD6->tl=*_tmp113,_tmpAD6))))));}{void*_tmp125=x;_npop_handler(0);return
_tmp125;};};};;_pop_dynregion(d);};}struct _tuple0*Cyc_Toc_temp_var(){int _tmp12D=
Cyc_Toc_temp_var_counter ++;struct _dyneither_ptr*_tmpAE4;const char*_tmpAE3;void*
_tmpAE2[1];struct Cyc_Int_pa_struct _tmpAE1;struct _tuple0*_tmpAE0;struct _tuple0*
res=(_tmpAE0=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAE0)),((_tmpAE0->f1=
Cyc_Absyn_Loc_n,((_tmpAE0->f2=((_tmpAE4=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpAE4)),((_tmpAE4[0]=(struct _dyneither_ptr)((_tmpAE1.tag=1,((_tmpAE1.f1=(
unsigned int)_tmp12D,((_tmpAE2[0]=& _tmpAE1,Cyc_aprintf(((_tmpAE3="_tmp%X",
_tag_dyneither(_tmpAE3,sizeof(char),7))),_tag_dyneither(_tmpAE2,sizeof(void*),1)))))))),
_tmpAE4)))),_tmpAE0)))));return res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp134;struct Cyc_Xarray_Xarray*_tmp135;struct Cyc_Toc_TocState
_tmp133=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp134=
_tmp133.dyn;_tmp135=_tmp133.temp_labels;{struct _DynRegionFrame _tmp136;struct
_RegionHandle*d=_open_dynregion(& _tmp136,_tmp134);{int _tmp137=Cyc_Toc_fresh_label_counter
++;if(_tmp137 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp135)){
struct _dyneither_ptr*_tmp138=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp135,_tmp137);_npop_handler(0);return _tmp138;}{struct Cyc_Int_pa_struct
_tmpAEC;void*_tmpAEB[1];const char*_tmpAEA;struct _dyneither_ptr*_tmpAE9;struct
_dyneither_ptr*res=(_tmpAE9=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpAE9)),((_tmpAE9[0]=(struct _dyneither_ptr)((_tmpAEC.tag=1,((_tmpAEC.f1=(
unsigned int)_tmp137,((_tmpAEB[0]=& _tmpAEC,Cyc_aprintf(((_tmpAEA="_LL%X",
_tag_dyneither(_tmpAEA,sizeof(char),6))),_tag_dyneither(_tmpAEB,sizeof(void*),1)))))))),
_tmpAE9)));if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(
_tmp135,res)!= _tmp137){const char*_tmpAEF;void*_tmpAEE;(_tmpAEE=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAEF="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmpAEF,sizeof(char),43))),_tag_dyneither(_tmpAEE,sizeof(void*),0)));}{
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
_tmp142=_tmp141.f1;_tmp143=_tmp141.f2;_tmp144=_tmp141.f3;{struct _tuple7*_tmpAF0;
return(_tmpAF0=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAF0)),((_tmpAF0->f1=
_tmp142,((_tmpAF0->f2=_tmp143,((_tmpAF0->f3=Cyc_Toc_typ_to_c(_tmp144),_tmpAF0)))))));};}
static struct _tuple9*Cyc_Toc_typ_to_c_f(struct _tuple9*x){struct Cyc_Absyn_Tqual
_tmp147;void*_tmp148;struct _tuple9 _tmp146=*x;_tmp147=_tmp146.f1;_tmp148=_tmp146.f2;{
struct _tuple9*_tmpAF1;return(_tmpAF1=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpAF1)),((_tmpAF1->f1=_tmp147,((_tmpAF1->f2=Cyc_Toc_typ_to_c(_tmp148),
_tmpAF1)))));};}static void*Cyc_Toc_typ_to_c_array(void*t){void*_tmp14A=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmp14C;void*_tmp14D;struct Cyc_Absyn_Tqual _tmp14E;
struct Cyc_Absyn_Exp*_tmp14F;union Cyc_Absyn_Constraint*_tmp150;struct Cyc_Position_Segment*
_tmp151;struct Cyc_Core_Opt*_tmp153;struct Cyc_Core_Opt _tmp154;void*_tmp155;_LL37: {
struct Cyc_Absyn_ArrayType_struct*_tmp14B=(struct Cyc_Absyn_ArrayType_struct*)
_tmp14A;if(_tmp14B->tag != 9)goto _LL39;else{_tmp14C=_tmp14B->f1;_tmp14D=_tmp14C.elt_type;
_tmp14E=_tmp14C.tq;_tmp14F=_tmp14C.num_elts;_tmp150=_tmp14C.zero_term;_tmp151=
_tmp14C.zt_loc;}}_LL38: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp14D),
_tmp14E,_tmp14F,Cyc_Absyn_false_conref,_tmp151);_LL39: {struct Cyc_Absyn_Evar_struct*
_tmp152=(struct Cyc_Absyn_Evar_struct*)_tmp14A;if(_tmp152->tag != 1)goto _LL3B;
else{_tmp153=_tmp152->f2;if(_tmp153 == 0)goto _LL3B;_tmp154=*_tmp153;_tmp155=(void*)
_tmp154.v;}}_LL3A: return Cyc_Toc_typ_to_c_array(_tmp155);_LL3B:;_LL3C: return Cyc_Toc_typ_to_c(
t);_LL36:;}static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*
f){struct Cyc_Absyn_Aggrfield*_tmpAF2;return(_tmpAF2=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpAF2)),((_tmpAF2->name=f->name,((_tmpAF2->tq=Cyc_Toc_mt_tq,((_tmpAF2->type=
Cyc_Toc_typ_to_c(f->type),((_tmpAF2->width=f->width,((_tmpAF2->attributes=f->attributes,
_tmpAF2)))))))))));}static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
return;}static void*Cyc_Toc_char_star_typ(){static void**cs=0;if(cs == 0){void**
_tmpAF3;cs=((_tmpAF3=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAF3)),((
_tmpAF3[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpAF3))));}return*cs;}static void*Cyc_Toc_rgn_typ(){
static void**r=0;if(r == 0){void**_tmpAF4;r=((_tmpAF4=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpAF4)),((_tmpAF4[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq),_tmpAF4))));}return*r;}static void*Cyc_Toc_dyn_rgn_typ(){static
void**r=0;if(r == 0){void**_tmpAF5;r=((_tmpAF5=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpAF5)),((_tmpAF5[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),
Cyc_Toc_mt_tq),_tmpAF5))));}return*r;}static void*Cyc_Toc_typ_to_c(void*t){void*
_tmp15A=t;struct Cyc_Core_Opt*_tmp15D;struct Cyc_Core_Opt*_tmp15F;struct Cyc_Core_Opt
_tmp160;void*_tmp161;struct Cyc_Absyn_Tvar*_tmp163;struct Cyc_Absyn_DatatypeFieldInfo
_tmp166;union Cyc_Absyn_DatatypeFieldInfoU _tmp167;struct _tuple1 _tmp168;struct Cyc_Absyn_Datatypedecl*
_tmp169;struct Cyc_Absyn_Datatypefield*_tmp16A;struct Cyc_Absyn_PtrInfo _tmp16D;
void*_tmp16E;struct Cyc_Absyn_Tqual _tmp16F;struct Cyc_Absyn_PtrAtts _tmp170;union
Cyc_Absyn_Constraint*_tmp171;struct Cyc_Absyn_ArrayInfo _tmp176;void*_tmp177;
struct Cyc_Absyn_Tqual _tmp178;struct Cyc_Absyn_Exp*_tmp179;struct Cyc_Position_Segment*
_tmp17A;struct Cyc_Absyn_FnInfo _tmp17C;struct Cyc_Absyn_Tqual _tmp17D;void*_tmp17E;
struct Cyc_List_List*_tmp17F;int _tmp180;struct Cyc_Absyn_VarargInfo*_tmp181;struct
Cyc_List_List*_tmp182;struct Cyc_List_List*_tmp184;enum Cyc_Absyn_AggrKind _tmp186;
struct Cyc_List_List*_tmp187;struct Cyc_Absyn_AggrInfo _tmp189;union Cyc_Absyn_AggrInfoU
_tmp18A;struct Cyc_List_List*_tmp18B;struct _tuple0*_tmp18D;struct Cyc_List_List*
_tmp18F;struct _tuple0*_tmp191;struct Cyc_List_List*_tmp192;struct Cyc_Absyn_Typedefdecl*
_tmp193;void**_tmp194;void*_tmp197;struct Cyc_Absyn_TypeDecl*_tmp1A1;struct Cyc_Absyn_TypeDecl
_tmp1A2;void*_tmp1A3;struct Cyc_Absyn_Aggrdecl*_tmp1A5;struct Cyc_Absyn_TypeDecl*
_tmp1A7;struct Cyc_Absyn_TypeDecl _tmp1A8;void*_tmp1A9;struct Cyc_Absyn_Enumdecl*
_tmp1AB;struct Cyc_Absyn_TypeDecl*_tmp1AD;struct Cyc_Absyn_TypeDecl _tmp1AE;void*
_tmp1AF;struct Cyc_Absyn_Datatypedecl*_tmp1B1;void**_tmp1B2;_LL3E: {struct Cyc_Absyn_VoidType_struct*
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
const char*_tmpAF6;return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp16A->name,((
_tmpAF6="_struct",_tag_dyneither(_tmpAF6,sizeof(char),8)))));}_LL4A: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp16B=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp15A;if(_tmp16B->tag != 4)
goto _LL4C;}_LL4B: {const char*_tmpAF9;void*_tmpAF8;(_tmpAF8=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF9="unresolved DatatypeFieldType",
_tag_dyneither(_tmpAF9,sizeof(char),29))),_tag_dyneither(_tmpAF8,sizeof(void*),0)));}
_LL4C: {struct Cyc_Absyn_PointerType_struct*_tmp16C=(struct Cyc_Absyn_PointerType_struct*)
_tmp15A;if(_tmp16C->tag != 5)goto _LL4E;else{_tmp16D=_tmp16C->f1;_tmp16E=_tmp16D.elt_typ;
_tmp16F=_tmp16D.elt_tq;_tmp170=_tmp16D.ptr_atts;_tmp171=_tmp170.bounds;}}_LL4D:
_tmp16E=Cyc_Toc_typ_to_c_array(_tmp16E);{void*_tmp1B6=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp171);_LL7F: {
struct Cyc_Absyn_DynEither_b_struct*_tmp1B7=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp1B6;if(_tmp1B7->tag != 0)goto _LL81;}_LL80: return Cyc_Toc_dyneither_ptr_typ;
_LL81:;_LL82: return Cyc_Absyn_star_typ(_tmp16E,(void*)& Cyc_Absyn_HeapRgn_val,
_tmp16F,Cyc_Absyn_false_conref);_LL7E:;};_LL4E: {struct Cyc_Absyn_IntType_struct*
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
_tmp1B8=0;for(0;_tmp182 != 0;_tmp182=_tmp182->tl){void*_tmp1B9=(void*)_tmp182->hd;
_LL84: {struct Cyc_Absyn_Noreturn_att_struct*_tmp1BA=(struct Cyc_Absyn_Noreturn_att_struct*)
_tmp1B9;if(_tmp1BA->tag != 4)goto _LL86;}_LL85: goto _LL87;_LL86: {struct Cyc_Absyn_Const_att_struct*
_tmp1BB=(struct Cyc_Absyn_Const_att_struct*)_tmp1B9;if(_tmp1BB->tag != 5)goto _LL88;}
_LL87: goto _LL89;_LL88: {struct Cyc_Absyn_Format_att_struct*_tmp1BC=(struct Cyc_Absyn_Format_att_struct*)
_tmp1B9;if(_tmp1BC->tag != 19)goto _LL8A;}_LL89: continue;_LL8A: {struct Cyc_Absyn_Noliveunique_att_struct*
_tmp1BD=(struct Cyc_Absyn_Noliveunique_att_struct*)_tmp1B9;if(_tmp1BD->tag != 21)
goto _LL8C;}_LL8B: continue;_LL8C: {struct Cyc_Absyn_Initializes_att_struct*_tmp1BE=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp1B9;if(_tmp1BE->tag != 20)goto _LL8E;}
_LL8D: continue;_LL8E:;_LL8F:{struct Cyc_List_List*_tmpAFA;_tmp1B8=((_tmpAFA=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAFA)),((_tmpAFA->hd=(void*)
_tmp182->hd,((_tmpAFA->tl=_tmp1B8,_tmpAFA))))));}goto _LL83;_LL83:;}{struct Cyc_List_List*
_tmp1C0=((struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple7*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp17F);if(_tmp181 != 0){void*_tmp1C1=Cyc_Toc_typ_to_c(
Cyc_Absyn_dyneither_typ(_tmp181->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,
Cyc_Absyn_false_conref));struct _tuple7*_tmpAFB;struct _tuple7*_tmp1C2=(_tmpAFB=
_cycalloc(sizeof(*_tmpAFB)),((_tmpAFB->f1=_tmp181->name,((_tmpAFB->f2=_tmp181->tq,((
_tmpAFB->f3=_tmp1C1,_tmpAFB)))))));struct Cyc_List_List*_tmpAFC;_tmp1C0=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp1C0,((_tmpAFC=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAFC)),((
_tmpAFC->hd=_tmp1C2,((_tmpAFC->tl=0,_tmpAFC)))))));}{struct Cyc_Absyn_FnType_struct
_tmpB02;struct Cyc_Absyn_FnInfo _tmpB01;struct Cyc_Absyn_FnType_struct*_tmpB00;
return(void*)((_tmpB00=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB00)),((
_tmpB00[0]=((_tmpB02.tag=10,((_tmpB02.f1=((_tmpB01.tvars=0,((_tmpB01.effect=0,((
_tmpB01.ret_tqual=_tmp17D,((_tmpB01.ret_typ=Cyc_Toc_typ_to_c(_tmp17E),((_tmpB01.args=
_tmp1C0,((_tmpB01.c_varargs=_tmp180,((_tmpB01.cyc_varargs=0,((_tmpB01.rgn_po=0,((
_tmpB01.attributes=_tmp1B8,_tmpB01)))))))))))))))))),_tmpB02)))),_tmpB00))));};};}
_LL58: {struct Cyc_Absyn_TupleType_struct*_tmp183=(struct Cyc_Absyn_TupleType_struct*)
_tmp15A;if(_tmp183->tag != 11)goto _LL5A;else{_tmp184=_tmp183->f1;}}_LL59: _tmp184=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp184);return Cyc_Toc_add_tuple_type(_tmp184);
_LL5A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp185=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp15A;if(_tmp185->tag != 13)goto _LL5C;else{_tmp186=_tmp185->f1;_tmp187=_tmp185->f2;}}
_LL5B: {struct Cyc_Absyn_AnonAggrType_struct _tmpB05;struct Cyc_Absyn_AnonAggrType_struct*
_tmpB04;return(void*)((_tmpB04=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpB04)),((_tmpB04[0]=((_tmpB05.tag=13,((_tmpB05.f1=_tmp186,((_tmpB05.f2=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp187),_tmpB05)))))),
_tmpB04))));}_LL5C: {struct Cyc_Absyn_AggrType_struct*_tmp188=(struct Cyc_Absyn_AggrType_struct*)
_tmp15A;if(_tmp188->tag != 12)goto _LL5E;else{_tmp189=_tmp188->f1;_tmp18A=_tmp189.aggr_info;
_tmp18B=_tmp189.targs;}}_LL5D:{union Cyc_Absyn_AggrInfoU _tmp1CA=_tmp18A;struct
_tuple2 _tmp1CB;_LL91: if((_tmp1CA.UnknownAggr).tag != 1)goto _LL93;_tmp1CB=(struct
_tuple2)(_tmp1CA.UnknownAggr).val;_LL92: return t;_LL93:;_LL94: goto _LL90;_LL90:;}{
struct Cyc_Absyn_Aggrdecl*_tmp1CC=Cyc_Absyn_get_known_aggrdecl(_tmp18A);if(
_tmp1CC->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(_tmp1CC->name,Cyc_Absyn_unionq_typ);
else{return Cyc_Toc_aggrdecl_type(_tmp1CC->name,Cyc_Absyn_strctq);}};_LL5E: {
struct Cyc_Absyn_EnumType_struct*_tmp18C=(struct Cyc_Absyn_EnumType_struct*)
_tmp15A;if(_tmp18C->tag != 14)goto _LL60;else{_tmp18D=_tmp18C->f1;}}_LL5F: return t;
_LL60: {struct Cyc_Absyn_AnonEnumType_struct*_tmp18E=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp15A;if(_tmp18E->tag != 15)goto _LL62;else{_tmp18F=_tmp18E->f1;}}_LL61: Cyc_Toc_enumfields_to_c(
_tmp18F);return t;_LL62: {struct Cyc_Absyn_TypedefType_struct*_tmp190=(struct Cyc_Absyn_TypedefType_struct*)
_tmp15A;if(_tmp190->tag != 18)goto _LL64;else{_tmp191=_tmp190->f1;_tmp192=_tmp190->f2;
_tmp193=_tmp190->f3;_tmp194=_tmp190->f4;}}_LL63: if(_tmp194 == 0  || Cyc_noexpand_r){
if(_tmp192 != 0){struct Cyc_Absyn_TypedefType_struct _tmpB08;struct Cyc_Absyn_TypedefType_struct*
_tmpB07;return(void*)((_tmpB07=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpB07)),((_tmpB07[0]=((_tmpB08.tag=18,((_tmpB08.f1=_tmp191,((_tmpB08.f2=0,((
_tmpB08.f3=_tmp193,((_tmpB08.f4=0,_tmpB08)))))))))),_tmpB07))));}else{return t;}}
else{struct Cyc_Absyn_TypedefType_struct _tmpB0E;void**_tmpB0D;struct Cyc_Absyn_TypedefType_struct*
_tmpB0C;return(void*)((_tmpB0C=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpB0C)),((_tmpB0C[0]=((_tmpB0E.tag=18,((_tmpB0E.f1=_tmp191,((_tmpB0E.f2=0,((
_tmpB0E.f3=_tmp193,((_tmpB0E.f4=((_tmpB0D=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB0D)),((_tmpB0D[0]=Cyc_Toc_typ_to_c_array(*_tmp194),_tmpB0D)))),
_tmpB0E)))))))))),_tmpB0C))));}_LL64: {struct Cyc_Absyn_TagType_struct*_tmp195=(
struct Cyc_Absyn_TagType_struct*)_tmp15A;if(_tmp195->tag != 20)goto _LL66;}_LL65:
return Cyc_Absyn_uint_typ;_LL66: {struct Cyc_Absyn_RgnHandleType_struct*_tmp196=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp15A;if(_tmp196->tag != 16)goto _LL68;
else{_tmp197=(void*)_tmp196->f1;}}_LL67: return Cyc_Toc_rgn_typ();_LL68: {struct
Cyc_Absyn_DynRgnType_struct*_tmp198=(struct Cyc_Absyn_DynRgnType_struct*)_tmp15A;
if(_tmp198->tag != 17)goto _LL6A;}_LL69: return Cyc_Toc_dyn_rgn_typ();_LL6A: {struct
Cyc_Absyn_HeapRgn_struct*_tmp199=(struct Cyc_Absyn_HeapRgn_struct*)_tmp15A;if(
_tmp199->tag != 21)goto _LL6C;}_LL6B: {const char*_tmpB11;void*_tmpB10;(_tmpB10=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB11="Toc::typ_to_c: type translation passed the heap region",_tag_dyneither(
_tmpB11,sizeof(char),55))),_tag_dyneither(_tmpB10,sizeof(void*),0)));}_LL6C: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp19A=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp15A;if(_tmp19A->tag != 22)goto _LL6E;}_LL6D: {const char*_tmpB14;void*_tmpB13;(
_tmpB13=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB14="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpB14,sizeof(char),57))),_tag_dyneither(_tmpB13,sizeof(void*),0)));}
_LL6E: {struct Cyc_Absyn_RefCntRgn_struct*_tmp19B=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp15A;if(_tmp19B->tag != 23)goto _LL70;}_LL6F: {const char*_tmpB17;void*_tmpB16;(
_tmpB16=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB17="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpB17,sizeof(char),57))),_tag_dyneither(_tmpB16,sizeof(void*),0)));}
_LL70: {struct Cyc_Absyn_AccessEff_struct*_tmp19C=(struct Cyc_Absyn_AccessEff_struct*)
_tmp15A;if(_tmp19C->tag != 24)goto _LL72;}_LL71: goto _LL73;_LL72: {struct Cyc_Absyn_JoinEff_struct*
_tmp19D=(struct Cyc_Absyn_JoinEff_struct*)_tmp15A;if(_tmp19D->tag != 25)goto _LL74;}
_LL73: goto _LL75;_LL74: {struct Cyc_Absyn_RgnsEff_struct*_tmp19E=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp15A;if(_tmp19E->tag != 26)goto _LL76;}_LL75: {const char*_tmpB1A;void*_tmpB19;(
_tmpB19=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB1A="Toc::typ_to_c: type translation passed an effect",_tag_dyneither(
_tmpB1A,sizeof(char),49))),_tag_dyneither(_tmpB19,sizeof(void*),0)));}_LL76: {
struct Cyc_Absyn_ValueofType_struct*_tmp19F=(struct Cyc_Absyn_ValueofType_struct*)
_tmp15A;if(_tmp19F->tag != 19)goto _LL78;}_LL77: {const char*_tmpB1D;void*_tmpB1C;(
_tmpB1C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB1D="Toc::typ_to_c: type translation passed a valueof_t",_tag_dyneither(
_tmpB1D,sizeof(char),51))),_tag_dyneither(_tmpB1C,sizeof(void*),0)));}_LL78: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp1A0=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp15A;if(_tmp1A0->tag != 27)goto _LL7A;else{_tmp1A1=_tmp1A0->f1;_tmp1A2=*_tmp1A1;
_tmp1A3=_tmp1A2.r;{struct Cyc_Absyn_Aggr_td_struct*_tmp1A4=(struct Cyc_Absyn_Aggr_td_struct*)
_tmp1A3;if(_tmp1A4->tag != 0)goto _LL7A;else{_tmp1A5=_tmp1A4->f1;}};}}_LL79: Cyc_Toc_aggrdecl_to_c(
_tmp1A5,1);if(_tmp1A5->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(
_tmp1A5->name,Cyc_Absyn_unionq_typ);else{return Cyc_Toc_aggrdecl_type(_tmp1A5->name,
Cyc_Absyn_strctq);}_LL7A: {struct Cyc_Absyn_TypeDeclType_struct*_tmp1A6=(struct
Cyc_Absyn_TypeDeclType_struct*)_tmp15A;if(_tmp1A6->tag != 27)goto _LL7C;else{
_tmp1A7=_tmp1A6->f1;_tmp1A8=*_tmp1A7;_tmp1A9=_tmp1A8.r;{struct Cyc_Absyn_Enum_td_struct*
_tmp1AA=(struct Cyc_Absyn_Enum_td_struct*)_tmp1A9;if(_tmp1AA->tag != 1)goto _LL7C;
else{_tmp1AB=_tmp1AA->f1;}};}}_LL7B: Cyc_Toc_enumdecl_to_c(_tmp1AB);return t;_LL7C: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp1AC=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp15A;if(_tmp1AC->tag != 27)goto _LL3D;else{_tmp1AD=_tmp1AC->f1;_tmp1AE=*_tmp1AD;
_tmp1AF=_tmp1AE.r;{struct Cyc_Absyn_Datatype_td_struct*_tmp1B0=(struct Cyc_Absyn_Datatype_td_struct*)
_tmp1AF;if(_tmp1B0->tag != 2)goto _LL3D;else{_tmp1B1=_tmp1B0->f1;}};_tmp1B2=
_tmp1AC->f2;}}_LL7D: Cyc_Toc_datatypedecl_to_c(_tmp1B1);return Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp1B2)));_LL3D:;}static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(
void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmp1DC=t;struct
Cyc_Absyn_ArrayInfo _tmp1DE;void*_tmp1DF;struct Cyc_Absyn_Tqual _tmp1E0;_LL96: {
struct Cyc_Absyn_ArrayType_struct*_tmp1DD=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1DC;if(_tmp1DD->tag != 9)goto _LL98;else{_tmp1DE=_tmp1DD->f1;_tmp1DF=_tmp1DE.elt_type;
_tmp1E0=_tmp1DE.tq;}}_LL97: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp1DF,(
void*)& Cyc_Absyn_HeapRgn_val,_tmp1E0,Cyc_Absyn_false_conref),e);_LL98:;_LL99:
return Cyc_Toc_cast_it(t,e);_LL95:;}static int Cyc_Toc_atomic_typ(void*t){void*
_tmp1E1=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp1EC;void*_tmp1ED;
struct Cyc_Absyn_AggrInfo _tmp1EF;union Cyc_Absyn_AggrInfoU _tmp1F0;struct Cyc_List_List*
_tmp1F2;struct Cyc_Absyn_DatatypeFieldInfo _tmp1F4;union Cyc_Absyn_DatatypeFieldInfoU
_tmp1F5;struct _tuple1 _tmp1F6;struct Cyc_Absyn_Datatypedecl*_tmp1F7;struct Cyc_Absyn_Datatypefield*
_tmp1F8;struct Cyc_List_List*_tmp1FA;_LL9B: {struct Cyc_Absyn_VoidType_struct*
_tmp1E2=(struct Cyc_Absyn_VoidType_struct*)_tmp1E1;if(_tmp1E2->tag != 0)goto _LL9D;}
_LL9C: return 1;_LL9D: {struct Cyc_Absyn_VarType_struct*_tmp1E3=(struct Cyc_Absyn_VarType_struct*)
_tmp1E1;if(_tmp1E3->tag != 2)goto _LL9F;}_LL9E: return 0;_LL9F: {struct Cyc_Absyn_IntType_struct*
_tmp1E4=(struct Cyc_Absyn_IntType_struct*)_tmp1E1;if(_tmp1E4->tag != 6)goto _LLA1;}
_LLA0: goto _LLA2;_LLA1: {struct Cyc_Absyn_EnumType_struct*_tmp1E5=(struct Cyc_Absyn_EnumType_struct*)
_tmp1E1;if(_tmp1E5->tag != 14)goto _LLA3;}_LLA2: goto _LLA4;_LLA3: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp1E6=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp1E1;if(_tmp1E6->tag != 15)goto
_LLA5;}_LLA4: goto _LLA6;_LLA5: {struct Cyc_Absyn_FloatType_struct*_tmp1E7=(struct
Cyc_Absyn_FloatType_struct*)_tmp1E1;if(_tmp1E7->tag != 7)goto _LLA7;}_LLA6: goto
_LLA8;_LLA7: {struct Cyc_Absyn_DoubleType_struct*_tmp1E8=(struct Cyc_Absyn_DoubleType_struct*)
_tmp1E1;if(_tmp1E8->tag != 8)goto _LLA9;}_LLA8: goto _LLAA;_LLA9: {struct Cyc_Absyn_FnType_struct*
_tmp1E9=(struct Cyc_Absyn_FnType_struct*)_tmp1E1;if(_tmp1E9->tag != 10)goto _LLAB;}
_LLAA: goto _LLAC;_LLAB: {struct Cyc_Absyn_TagType_struct*_tmp1EA=(struct Cyc_Absyn_TagType_struct*)
_tmp1E1;if(_tmp1EA->tag != 20)goto _LLAD;}_LLAC: return 1;_LLAD: {struct Cyc_Absyn_ArrayType_struct*
_tmp1EB=(struct Cyc_Absyn_ArrayType_struct*)_tmp1E1;if(_tmp1EB->tag != 9)goto _LLAF;
else{_tmp1EC=_tmp1EB->f1;_tmp1ED=_tmp1EC.elt_type;}}_LLAE: return Cyc_Toc_atomic_typ(
_tmp1ED);_LLAF: {struct Cyc_Absyn_AggrType_struct*_tmp1EE=(struct Cyc_Absyn_AggrType_struct*)
_tmp1E1;if(_tmp1EE->tag != 12)goto _LLB1;else{_tmp1EF=_tmp1EE->f1;_tmp1F0=_tmp1EF.aggr_info;}}
_LLB0:{union Cyc_Absyn_AggrInfoU _tmp1FF=_tmp1F0;struct _tuple2 _tmp200;_LLC2: if((
_tmp1FF.UnknownAggr).tag != 1)goto _LLC4;_tmp200=(struct _tuple2)(_tmp1FF.UnknownAggr).val;
_LLC3: return 0;_LLC4:;_LLC5: goto _LLC1;_LLC1:;}{struct Cyc_Absyn_Aggrdecl*_tmp201=
Cyc_Absyn_get_known_aggrdecl(_tmp1F0);if(_tmp201->impl == 0)return 0;{struct Cyc_List_List*
_tmp202=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp201->impl))->fields;for(
0;_tmp202 != 0;_tmp202=_tmp202->tl){if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)
_tmp202->hd)->type))return 0;}}return 1;};_LLB1: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp1F1=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp1E1;if(_tmp1F1->tag != 13)goto
_LLB3;else{_tmp1F2=_tmp1F1->f2;}}_LLB2: for(0;_tmp1F2 != 0;_tmp1F2=_tmp1F2->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1F2->hd)->type))return 0;}
return 1;_LLB3: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp1F3=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp1E1;if(_tmp1F3->tag != 4)goto _LLB5;else{_tmp1F4=_tmp1F3->f1;_tmp1F5=_tmp1F4.field_info;
if((_tmp1F5.KnownDatatypefield).tag != 2)goto _LLB5;_tmp1F6=(struct _tuple1)(
_tmp1F5.KnownDatatypefield).val;_tmp1F7=_tmp1F6.f1;_tmp1F8=_tmp1F6.f2;}}_LLB4:
_tmp1FA=_tmp1F8->typs;goto _LLB6;_LLB5: {struct Cyc_Absyn_TupleType_struct*_tmp1F9=(
struct Cyc_Absyn_TupleType_struct*)_tmp1E1;if(_tmp1F9->tag != 11)goto _LLB7;else{
_tmp1FA=_tmp1F9->f1;}}_LLB6: for(0;_tmp1FA != 0;_tmp1FA=_tmp1FA->tl){if(!Cyc_Toc_atomic_typ((*((
struct _tuple9*)_tmp1FA->hd)).f2))return 0;}return 1;_LLB7: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1FB=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1E1;if(_tmp1FB->tag != 3)goto
_LLB9;}_LLB8: goto _LLBA;_LLB9: {struct Cyc_Absyn_PointerType_struct*_tmp1FC=(
struct Cyc_Absyn_PointerType_struct*)_tmp1E1;if(_tmp1FC->tag != 5)goto _LLBB;}_LLBA:
goto _LLBC;_LLBB: {struct Cyc_Absyn_DynRgnType_struct*_tmp1FD=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp1E1;if(_tmp1FD->tag != 17)goto _LLBD;}_LLBC: goto _LLBE;_LLBD: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp1FE=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp1E1;if(_tmp1FE->tag != 16)goto
_LLBF;}_LLBE: return 0;_LLBF:;_LLC0: {const char*_tmpB21;void*_tmpB20[1];struct Cyc_String_pa_struct
_tmpB1F;(_tmpB1F.tag=0,((_tmpB1F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpB20[0]=& _tmpB1F,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB21="atomic_typ:  bad type %s",
_tag_dyneither(_tmpB21,sizeof(char),25))),_tag_dyneither(_tmpB20,sizeof(void*),1)))))));}
_LL9A:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp206=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp208;void*_tmp209;_LLC7: {struct Cyc_Absyn_PointerType_struct*
_tmp207=(struct Cyc_Absyn_PointerType_struct*)_tmp206;if(_tmp207->tag != 5)goto
_LLC9;else{_tmp208=_tmp207->f1;_tmp209=_tmp208.elt_typ;}}_LLC8: {void*_tmp20A=
Cyc_Tcutil_compress(_tmp209);_LLCC: {struct Cyc_Absyn_VoidType_struct*_tmp20B=(
struct Cyc_Absyn_VoidType_struct*)_tmp20A;if(_tmp20B->tag != 0)goto _LLCE;}_LLCD:
return 1;_LLCE:;_LLCF: return 0;_LLCB:;}_LLC9:;_LLCA: return 0;_LLC6:;}static int Cyc_Toc_is_tvar(
void*t){void*_tmp20C=Cyc_Tcutil_compress(t);_LLD1: {struct Cyc_Absyn_VarType_struct*
_tmp20D=(struct Cyc_Absyn_VarType_struct*)_tmp20C;if(_tmp20D->tag != 2)goto _LLD3;}
_LLD2: return 1;_LLD3:;_LLD4: return 0;_LLD0:;}static int Cyc_Toc_is_void_star_or_tvar(
void*t){return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_tvar(t);}static int Cyc_Toc_is_poly_field(
void*t,struct _dyneither_ptr*f){void*_tmp20E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp210;union Cyc_Absyn_AggrInfoU _tmp211;struct Cyc_List_List*_tmp213;_LLD6: {
struct Cyc_Absyn_AggrType_struct*_tmp20F=(struct Cyc_Absyn_AggrType_struct*)
_tmp20E;if(_tmp20F->tag != 12)goto _LLD8;else{_tmp210=_tmp20F->f1;_tmp211=_tmp210.aggr_info;}}
_LLD7: {struct Cyc_Absyn_Aggrdecl*_tmp214=Cyc_Absyn_get_known_aggrdecl(_tmp211);
if(_tmp214->impl == 0){const char*_tmpB24;void*_tmpB23;(_tmpB23=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB24="is_poly_field: type missing fields",
_tag_dyneither(_tmpB24,sizeof(char),35))),_tag_dyneither(_tmpB23,sizeof(void*),0)));}
_tmp213=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp214->impl))->fields;goto
_LLD9;}_LLD8: {struct Cyc_Absyn_AnonAggrType_struct*_tmp212=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp20E;if(_tmp212->tag != 13)goto _LLDA;else{_tmp213=_tmp212->f2;}}_LLD9: {struct
Cyc_Absyn_Aggrfield*_tmp217=Cyc_Absyn_lookup_field(_tmp213,f);if(_tmp217 == 0){
const char*_tmpB28;void*_tmpB27[1];struct Cyc_String_pa_struct _tmpB26;(_tmpB26.tag=
0,((_tmpB26.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB27[0]=&
_tmpB26,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB28="is_poly_field: bad field %s",_tag_dyneither(_tmpB28,sizeof(char),28))),
_tag_dyneither(_tmpB27,sizeof(void*),1)))))));}return Cyc_Toc_is_void_star_or_tvar(
_tmp217->type);}_LLDA:;_LLDB: {const char*_tmpB2C;void*_tmpB2B[1];struct Cyc_String_pa_struct
_tmpB2A;(_tmpB2A.tag=0,((_tmpB2A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpB2B[0]=& _tmpB2A,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2C="is_poly_field: bad type %s",
_tag_dyneither(_tmpB2C,sizeof(char),27))),_tag_dyneither(_tmpB2B,sizeof(void*),1)))))));}
_LLD5:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmp21E=e->r;
struct Cyc_Absyn_Exp*_tmp220;struct _dyneither_ptr*_tmp221;struct Cyc_Absyn_Exp*
_tmp223;struct _dyneither_ptr*_tmp224;_LLDD: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp21F=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp21E;if(_tmp21F->tag != 22)goto
_LLDF;else{_tmp220=_tmp21F->f1;_tmp221=_tmp21F->f2;}}_LLDE: return Cyc_Toc_is_poly_field((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp220->topt))->v,_tmp221);_LLDF: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp222=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp21E;if(_tmp222->tag != 23)goto _LLE1;else{_tmp223=_tmp222->f1;_tmp224=_tmp222->f2;}}
_LLE0: {void*_tmp225=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp223->topt))->v);struct Cyc_Absyn_PtrInfo _tmp227;void*_tmp228;
_LLE4: {struct Cyc_Absyn_PointerType_struct*_tmp226=(struct Cyc_Absyn_PointerType_struct*)
_tmp225;if(_tmp226->tag != 5)goto _LLE6;else{_tmp227=_tmp226->f1;_tmp228=_tmp227.elt_typ;}}
_LLE5: return Cyc_Toc_is_poly_field(_tmp228,_tmp224);_LLE6:;_LLE7: {const char*
_tmpB30;void*_tmpB2F[1];struct Cyc_String_pa_struct _tmpB2E;(_tmpB2E.tag=0,((
_tmpB2E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp223->topt))->v)),((_tmpB2F[0]=&
_tmpB2E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB30="is_poly_project: bad type %s",_tag_dyneither(_tmpB30,sizeof(char),29))),
_tag_dyneither(_tmpB2F,sizeof(void*),1)))))));}_LLE3:;}_LLE1:;_LLE2: return 0;
_LLDC:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB31;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((
_tmpB31=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB31)),((_tmpB31->hd=s,((
_tmpB31->tl=0,_tmpB31)))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp*s){struct Cyc_List_List*_tmpB32;return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_atomic_e,((_tmpB32=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB32)),((_tmpB32->hd=s,((_tmpB32->tl=0,_tmpB32)))))),0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(
t))return Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*
Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*
_tmpB33[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpB33[1]=s,((
_tmpB33[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB33,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*_tmpB34[2];return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_atomic_e,((_tmpB34[1]=n,((_tmpB34[0]=s,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB34,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);}else{struct Cyc_Absyn_Exp*_tmpB35[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((
_tmpB35[1]=n,((_tmpB35[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB35,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n){struct Cyc_Absyn_Exp*_tmpB36[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((
_tmpB36[2]=n,((_tmpB36[1]=s,((_tmpB36[0]=rgn,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB36,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmpB37;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmpB37=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB37)),((_tmpB37->hd=e,((
_tmpB37->tl=0,_tmpB37)))))),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
struct Cyc_Absyn_Exp*_tmpB38[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpB38,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*
_tmp234=e->r;union Cyc_Absyn_Cnst _tmp236;struct _dyneither_ptr _tmp237;_LLE9: {
struct Cyc_Absyn_Const_e_struct*_tmp235=(struct Cyc_Absyn_Const_e_struct*)_tmp234;
if(_tmp235->tag != 0)goto _LLEB;else{_tmp236=_tmp235->f1;if((_tmp236.String_c).tag
!= 7)goto _LLEB;_tmp237=(struct _dyneither_ptr)(_tmp236.String_c).val;}}_LLEA:
is_string=1;goto _LLE8;_LLEB:;_LLEC: goto _LLE8;_LLE8:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple0*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_struct*_tmpB3E;struct
Cyc_Absyn_Var_d_struct _tmpB3D;struct Cyc_List_List*_tmpB3C;Cyc_Toc_result_decls=((
_tmpB3C=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB3C)),((_tmpB3C->hd=
Cyc_Absyn_new_decl((void*)((_tmpB3E=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB3E)),((_tmpB3E[0]=((_tmpB3D.tag=0,((_tmpB3D.f1=vd,_tmpB3D)))),
_tmpB3E)))),0),((_tmpB3C->tl=Cyc_Toc_result_decls,_tmpB3C))))));}xexp=Cyc_Absyn_var_exp(
x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
struct Cyc_Absyn_Exp*urm_exp;{struct _tuple15*_tmpB45;struct _tuple15*_tmpB44;
struct _tuple15*_tmpB43;struct _tuple15*_tmpB42[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(
0,((_tmpB42[2]=((_tmpB43=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB43)),((
_tmpB43->f1=0,((_tmpB43->f2=xplussz,_tmpB43)))))),((_tmpB42[1]=((_tmpB44=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB44)),((_tmpB44->f1=0,((
_tmpB44->f2=xexp,_tmpB44)))))),((_tmpB42[0]=((_tmpB45=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB45)),((_tmpB45->f1=0,((_tmpB45->f2=xexp,_tmpB45)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB42,sizeof(struct
_tuple15*),3)))))))),0);}return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct
_dyneither_ptr*label;struct Cyc_List_List*binders;struct Cyc_Dict_Dict
next_case_env;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct
_dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;struct Cyc_Dict_Dict
varmap;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};static int Cyc_Toc_is_toplevel(
struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp240;int _tmp241;struct Cyc_Toc_Env*
_tmp23F=nv;_tmp240=*_tmp23F;_tmp241=_tmp240.toplevel;return _tmp241;}static int Cyc_Toc_in_lhs(
struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp243;int*_tmp244;struct Cyc_Toc_Env*
_tmp242=nv;_tmp243=*_tmp242;_tmp244=_tmp243.in_lhs;return*_tmp244;}static struct
Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple0*x){struct
Cyc_Toc_Env _tmp246;struct Cyc_Dict_Dict _tmp247;struct Cyc_Toc_Env*_tmp245=nv;
_tmp246=*_tmp245;_tmp247=_tmp246.varmap;return((struct Cyc_Absyn_Exp*(*)(struct
Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp247,x);}static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(struct _RegionHandle*r){int*_tmpB48;struct Cyc_Toc_Env*_tmpB47;
return(_tmpB47=_region_malloc(r,sizeof(*_tmpB47)),((_tmpB47->break_lab=(struct
_dyneither_ptr**)0,((_tmpB47->continue_lab=(struct _dyneither_ptr**)0,((_tmpB47->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpB47->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp),((_tmpB47->toplevel=(int)1,((_tmpB47->in_lhs=(int*)((
_tmpB48=_region_malloc(r,sizeof(*_tmpB48)),((_tmpB48[0]=0,_tmpB48)))),((_tmpB47->rgn=(
struct _RegionHandle*)r,_tmpB47)))))))))))))));}static struct Cyc_Toc_Env*Cyc_Toc_share_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp24B;struct
_dyneither_ptr**_tmp24C;struct _dyneither_ptr**_tmp24D;struct Cyc_Toc_FallthruInfo*
_tmp24E;struct Cyc_Dict_Dict _tmp24F;int _tmp250;int*_tmp251;struct Cyc_Toc_Env*
_tmp24A=e;_tmp24B=*_tmp24A;_tmp24C=_tmp24B.break_lab;_tmp24D=_tmp24B.continue_lab;
_tmp24E=_tmp24B.fallthru_info;_tmp24F=_tmp24B.varmap;_tmp250=_tmp24B.toplevel;
_tmp251=_tmp24B.in_lhs;{struct Cyc_Toc_Env*_tmpB49;return(_tmpB49=_region_malloc(
r,sizeof(*_tmpB49)),((_tmpB49->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp24C),((_tmpB49->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp24D),((_tmpB49->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp24E,((_tmpB49->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp24F),((_tmpB49->toplevel=(
int)_tmp250,((_tmpB49->in_lhs=(int*)_tmp251,((_tmpB49->rgn=(struct _RegionHandle*)
r,_tmpB49)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp254;struct
_dyneither_ptr**_tmp255;struct _dyneither_ptr**_tmp256;struct Cyc_Toc_FallthruInfo*
_tmp257;struct Cyc_Dict_Dict _tmp258;int _tmp259;int*_tmp25A;struct Cyc_Toc_Env*
_tmp253=e;_tmp254=*_tmp253;_tmp255=_tmp254.break_lab;_tmp256=_tmp254.continue_lab;
_tmp257=_tmp254.fallthru_info;_tmp258=_tmp254.varmap;_tmp259=_tmp254.toplevel;
_tmp25A=_tmp254.in_lhs;{struct Cyc_Toc_Env*_tmpB4A;return(_tmpB4A=_region_malloc(
r,sizeof(*_tmpB4A)),((_tmpB4A->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp255),((_tmpB4A->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp256),((_tmpB4A->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp257,((_tmpB4A->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp258),((_tmpB4A->toplevel=(
int)0,((_tmpB4A->in_lhs=(int*)_tmp25A,((_tmpB4A->rgn=(struct _RegionHandle*)r,
_tmpB4A)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct
_RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp25D;struct
_dyneither_ptr**_tmp25E;struct _dyneither_ptr**_tmp25F;struct Cyc_Toc_FallthruInfo*
_tmp260;struct Cyc_Dict_Dict _tmp261;int _tmp262;int*_tmp263;struct Cyc_Toc_Env*
_tmp25C=e;_tmp25D=*_tmp25C;_tmp25E=_tmp25D.break_lab;_tmp25F=_tmp25D.continue_lab;
_tmp260=_tmp25D.fallthru_info;_tmp261=_tmp25D.varmap;_tmp262=_tmp25D.toplevel;
_tmp263=_tmp25D.in_lhs;{struct Cyc_Toc_Env*_tmpB4B;return(_tmpB4B=_region_malloc(
r,sizeof(*_tmpB4B)),((_tmpB4B->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp25E),((_tmpB4B->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp25F),((_tmpB4B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp260,((_tmpB4B->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp261),((_tmpB4B->toplevel=(
int)1,((_tmpB4B->in_lhs=(int*)_tmp263,((_tmpB4B->rgn=(struct _RegionHandle*)r,
_tmpB4B)))))))))))))));};}static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env _tmp266;int*_tmp267;struct Cyc_Toc_Env*_tmp265=e;_tmp266=*
_tmp265;_tmp267=_tmp266.in_lhs;*_tmp267=b;}static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace _tmp268=(*x).f1;struct Cyc_List_List*_tmp269;_LLEE: if((
_tmp268.Rel_n).tag != 1)goto _LLF0;_tmp269=(struct Cyc_List_List*)(_tmp268.Rel_n).val;
_LLEF: {const char*_tmpB4F;void*_tmpB4E[1];struct Cyc_String_pa_struct _tmpB4D;(
_tmpB4D.tag=0,((_tmpB4D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
x)),((_tmpB4E[0]=& _tmpB4D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB4F="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(
_tmpB4F,sizeof(char),30))),_tag_dyneither(_tmpB4E,sizeof(void*),1)))))));}_LLF0:;
_LLF1: goto _LLED;_LLED:;}{struct Cyc_Toc_Env _tmp26E;struct _dyneither_ptr**_tmp26F;
struct _dyneither_ptr**_tmp270;struct Cyc_Toc_FallthruInfo*_tmp271;struct Cyc_Dict_Dict
_tmp272;int _tmp273;int*_tmp274;struct Cyc_Toc_Env*_tmp26D=e;_tmp26E=*_tmp26D;
_tmp26F=_tmp26E.break_lab;_tmp270=_tmp26E.continue_lab;_tmp271=_tmp26E.fallthru_info;
_tmp272=_tmp26E.varmap;_tmp273=_tmp26E.toplevel;_tmp274=_tmp26E.in_lhs;{struct
Cyc_Dict_Dict _tmp275=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*
k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp272),x,y);struct Cyc_Toc_Env*
_tmpB50;return(_tmpB50=_region_malloc(r,sizeof(*_tmpB50)),((_tmpB50->break_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp26F),((_tmpB50->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp270),((_tmpB50->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp271,((_tmpB50->varmap=(struct Cyc_Dict_Dict)
_tmp275,((_tmpB50->toplevel=(int)_tmp273,((_tmpB50->in_lhs=(int*)_tmp274,((
_tmpB50->rgn=(struct _RegionHandle*)r,_tmpB50)))))))))))))));};};}static struct Cyc_Toc_Env*
Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp278;struct _dyneither_ptr**_tmp279;struct _dyneither_ptr**_tmp27A;struct Cyc_Toc_FallthruInfo*
_tmp27B;struct Cyc_Dict_Dict _tmp27C;int _tmp27D;int*_tmp27E;struct Cyc_Toc_Env*
_tmp277=e;_tmp278=*_tmp277;_tmp279=_tmp278.break_lab;_tmp27A=_tmp278.continue_lab;
_tmp27B=_tmp278.fallthru_info;_tmp27C=_tmp278.varmap;_tmp27D=_tmp278.toplevel;
_tmp27E=_tmp278.in_lhs;{struct Cyc_Toc_Env*_tmpB51;return(_tmpB51=_region_malloc(
r,sizeof(*_tmpB51)),((_tmpB51->break_lab=(struct _dyneither_ptr**)0,((_tmpB51->continue_lab=(
struct _dyneither_ptr**)0,((_tmpB51->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp27B,((_tmpB51->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp27C),((_tmpB51->toplevel=(
int)_tmp27D,((_tmpB51->in_lhs=(int*)_tmp27E,((_tmpB51->rgn=(struct _RegionHandle*)
r,_tmpB51)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmpB52;fallthru_vars=((
_tmpB52=_region_malloc(r,sizeof(*_tmpB52)),((_tmpB52->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmpB52->tl=fallthru_vars,_tmpB52))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp282;struct _dyneither_ptr**_tmp283;struct
_dyneither_ptr**_tmp284;struct Cyc_Toc_FallthruInfo*_tmp285;struct Cyc_Dict_Dict
_tmp286;int _tmp287;int*_tmp288;struct Cyc_Toc_Env*_tmp281=e;_tmp282=*_tmp281;
_tmp283=_tmp282.break_lab;_tmp284=_tmp282.continue_lab;_tmp285=_tmp282.fallthru_info;
_tmp286=_tmp282.varmap;_tmp287=_tmp282.toplevel;_tmp288=_tmp282.in_lhs;{struct
Cyc_Toc_Env _tmp28A;struct Cyc_Dict_Dict _tmp28B;struct Cyc_Toc_Env*_tmp289=
next_case_env;_tmp28A=*_tmp289;_tmp28B=_tmp28A.varmap;{struct Cyc_Toc_FallthruInfo*
_tmpB53;struct Cyc_Toc_FallthruInfo*fi=(_tmpB53=_region_malloc(r,sizeof(*_tmpB53)),((
_tmpB53->label=fallthru_l,((_tmpB53->binders=fallthru_vars,((_tmpB53->next_case_env=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp28B),_tmpB53)))))));struct _dyneither_ptr**_tmpB56;struct Cyc_Toc_Env*
_tmpB55;return(_tmpB55=_region_malloc(r,sizeof(*_tmpB55)),((_tmpB55->break_lab=(
struct _dyneither_ptr**)((_tmpB56=_region_malloc(r,sizeof(*_tmpB56)),((_tmpB56[0]=
break_l,_tmpB56)))),((_tmpB55->continue_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp284),((_tmpB55->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
fi,((_tmpB55->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp286),((_tmpB55->toplevel=(
int)_tmp287,((_tmpB55->in_lhs=(int*)((int*)_tmp288),((_tmpB55->rgn=(struct
_RegionHandle*)r,_tmpB55)))))))))))))));};};};}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){struct
Cyc_Toc_Env _tmp290;struct _dyneither_ptr**_tmp291;struct _dyneither_ptr**_tmp292;
struct Cyc_Toc_FallthruInfo*_tmp293;struct Cyc_Dict_Dict _tmp294;int _tmp295;int*
_tmp296;struct Cyc_Toc_Env*_tmp28F=e;_tmp290=*_tmp28F;_tmp291=_tmp290.break_lab;
_tmp292=_tmp290.continue_lab;_tmp293=_tmp290.fallthru_info;_tmp294=_tmp290.varmap;
_tmp295=_tmp290.toplevel;_tmp296=_tmp290.in_lhs;{struct _dyneither_ptr**_tmpB59;
struct Cyc_Toc_Env*_tmpB58;return(_tmpB58=_region_malloc(r,sizeof(*_tmpB58)),((
_tmpB58->break_lab=(struct _dyneither_ptr**)((_tmpB59=_region_malloc(r,sizeof(*
_tmpB59)),((_tmpB59[0]=break_l,_tmpB59)))),((_tmpB58->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp292),((_tmpB58->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpB58->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp294),((_tmpB58->toplevel=(
int)_tmp295,((_tmpB58->in_lhs=(int*)_tmp296,((_tmpB58->rgn=(struct _RegionHandle*)
r,_tmpB58)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp29A;struct _dyneither_ptr**_tmp29B;struct _dyneither_ptr**_tmp29C;struct Cyc_Toc_FallthruInfo*
_tmp29D;struct Cyc_Dict_Dict _tmp29E;int _tmp29F;int*_tmp2A0;struct Cyc_Toc_Env*
_tmp299=e;_tmp29A=*_tmp299;_tmp29B=_tmp29A.break_lab;_tmp29C=_tmp29A.continue_lab;
_tmp29D=_tmp29A.fallthru_info;_tmp29E=_tmp29A.varmap;_tmp29F=_tmp29A.toplevel;
_tmp2A0=_tmp29A.in_lhs;{struct Cyc_Toc_FallthruInfo*_tmpB5C;struct Cyc_Toc_Env*
_tmpB5B;return(_tmpB5B=_region_malloc(r,sizeof(*_tmpB5B)),((_tmpB5B->break_lab=(
struct _dyneither_ptr**)0,((_tmpB5B->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp29C),((_tmpB5B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((
_tmpB5C=_region_malloc(r,sizeof(*_tmpB5C)),((_tmpB5C->label=next_l,((_tmpB5C->binders=
0,((_tmpB5C->next_case_env=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*
cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),
_tmpB5C)))))))),((_tmpB5B->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29E),((_tmpB5B->toplevel=(
int)_tmp29F,((_tmpB5B->in_lhs=(int*)((int*)_tmp2A0),((_tmpB5B->rgn=(struct
_RegionHandle*)r,_tmpB5B)))))))))))))));};}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp2A3=e->annot;_LLF3: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2A4=(
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp2A3;if(_tmp2A4->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLF5;}_LLF4: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLF5: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp2A5=(
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp2A3;if(_tmp2A5->tag != Cyc_CfFlowInfo_NotZero)
goto _LLF7;}_LLF6: return 0;_LLF7: {struct Cyc_CfFlowInfo_IsZero_struct*_tmp2A6=(
struct Cyc_CfFlowInfo_IsZero_struct*)_tmp2A3;if(_tmp2A6->tag != Cyc_CfFlowInfo_IsZero)
goto _LLF9;}_LLF8:{const char*_tmpB5F;void*_tmpB5E;(_tmpB5E=0,Cyc_Tcutil_terr(e->loc,((
_tmpB5F="dereference of NULL pointer",_tag_dyneither(_tmpB5F,sizeof(char),28))),
_tag_dyneither(_tmpB5E,sizeof(void*),0)));}return 0;_LLF9: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp2A7=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp2A3;if(_tmp2A7->tag != Cyc_Absyn_EmptyAnnot)
goto _LLFB;}_LLFA: return 0;_LLFB:;_LLFC: {const char*_tmpB62;void*_tmpB61;(_tmpB61=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB62="need_null_check",_tag_dyneither(_tmpB62,sizeof(char),16))),
_tag_dyneither(_tmpB61,sizeof(void*),0)));}_LLF2:;}static struct Cyc_List_List*Cyc_Toc_get_relns(
struct Cyc_Absyn_Exp*e){void*_tmp2AC=e->annot;struct Cyc_List_List*_tmp2AE;struct
Cyc_List_List*_tmp2B0;_LLFE: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2AD=(
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp2AC;if(_tmp2AD->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LL100;else{_tmp2AE=_tmp2AD->f1;}}_LLFF: return _tmp2AE;_LL100: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmp2AF=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmp2AC;if(_tmp2AF->tag != Cyc_CfFlowInfo_NotZero)
goto _LL102;else{_tmp2B0=_tmp2AF->f1;}}_LL101: return _tmp2B0;_LL102: {struct Cyc_CfFlowInfo_IsZero_struct*
_tmp2B1=(struct Cyc_CfFlowInfo_IsZero_struct*)_tmp2AC;if(_tmp2B1->tag != Cyc_CfFlowInfo_IsZero)
goto _LL104;}_LL103:{const char*_tmpB65;void*_tmpB64;(_tmpB64=0,Cyc_Tcutil_terr(e->loc,((
_tmpB65="dereference of NULL pointer",_tag_dyneither(_tmpB65,sizeof(char),28))),
_tag_dyneither(_tmpB64,sizeof(void*),0)));}return 0;_LL104: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp2B2=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp2AC;if(_tmp2B2->tag != Cyc_Absyn_EmptyAnnot)
goto _LL106;}_LL105: return 0;_LL106:;_LL107: {const char*_tmpB68;void*_tmpB67;(
_tmpB67=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB68="get_relns",_tag_dyneither(_tmpB68,sizeof(char),10))),_tag_dyneither(
_tmpB67,sizeof(void*),0)));}_LLFD:;}static int Cyc_Toc_check_const_array(
unsigned int i,void*t){void*_tmp2B7=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp2B9;struct Cyc_Absyn_PtrAtts _tmp2BA;union Cyc_Absyn_Constraint*_tmp2BB;union
Cyc_Absyn_Constraint*_tmp2BC;struct Cyc_Absyn_ArrayInfo _tmp2BE;struct Cyc_Absyn_Exp*
_tmp2BF;_LL109: {struct Cyc_Absyn_PointerType_struct*_tmp2B8=(struct Cyc_Absyn_PointerType_struct*)
_tmp2B7;if(_tmp2B8->tag != 5)goto _LL10B;else{_tmp2B9=_tmp2B8->f1;_tmp2BA=_tmp2B9.ptr_atts;
_tmp2BB=_tmp2BA.bounds;_tmp2BC=_tmp2BA.zero_term;}}_LL10A: {void*_tmp2C0=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp2BB);struct Cyc_Absyn_Exp*_tmp2C3;_LL110: {struct Cyc_Absyn_DynEither_b_struct*
_tmp2C1=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2C0;if(_tmp2C1->tag != 0)goto
_LL112;}_LL111: return 0;_LL112: {struct Cyc_Absyn_Upper_b_struct*_tmp2C2=(struct
Cyc_Absyn_Upper_b_struct*)_tmp2C0;if(_tmp2C2->tag != 1)goto _LL10F;else{_tmp2C3=
_tmp2C2->f1;}}_LL113: {unsigned int _tmp2C5;int _tmp2C6;struct _tuple10 _tmp2C4=Cyc_Evexp_eval_const_uint_exp(
_tmp2C3);_tmp2C5=_tmp2C4.f1;_tmp2C6=_tmp2C4.f2;return _tmp2C6  && i <= _tmp2C5;}
_LL10F:;}_LL10B: {struct Cyc_Absyn_ArrayType_struct*_tmp2BD=(struct Cyc_Absyn_ArrayType_struct*)
_tmp2B7;if(_tmp2BD->tag != 9)goto _LL10D;else{_tmp2BE=_tmp2BD->f1;_tmp2BF=_tmp2BE.num_elts;}}
_LL10C: if(_tmp2BF == 0)return 0;{unsigned int _tmp2C8;int _tmp2C9;struct _tuple10
_tmp2C7=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp2BF);_tmp2C8=
_tmp2C7.f1;_tmp2C9=_tmp2C7.f2;return _tmp2C9  && i <= _tmp2C8;};_LL10D:;_LL10E:
return 0;_LL108:;}static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2CA=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(
_tmp2CA->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp _tmp2CB=_tmp2CA->rop;struct
Cyc_Absyn_Vardecl*_tmp2CC;struct Cyc_Absyn_Vardecl*_tmp2CD;_LL115: if((_tmp2CB.LessNumelts).tag
!= 3)goto _LL117;_tmp2CC=(struct Cyc_Absyn_Vardecl*)(_tmp2CB.LessNumelts).val;
_LL116: _tmp2CD=_tmp2CC;goto _LL118;_LL117: if((_tmp2CB.LessEqNumelts).tag != 5)goto
_LL119;_tmp2CD=(struct Cyc_Absyn_Vardecl*)(_tmp2CB.LessEqNumelts).val;_LL118: if(
_tmp2CD == v)return 1;else{goto _LL114;}_LL119:;_LL11A: continue;_LL114:;};}return 0;}
static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Exp*e,void*vtype){{void*_tmp2CE=e->r;void*_tmp2D0;struct Cyc_Absyn_Vardecl*
_tmp2D2;void*_tmp2D4;struct Cyc_Absyn_Vardecl*_tmp2D6;void*_tmp2D8;struct Cyc_Absyn_Vardecl*
_tmp2DA;void*_tmp2DC;struct Cyc_Absyn_Vardecl*_tmp2DE;union Cyc_Absyn_Cnst _tmp2E0;
struct _tuple5 _tmp2E1;enum Cyc_Absyn_Sign _tmp2E2;int _tmp2E3;union Cyc_Absyn_Cnst
_tmp2E5;struct _tuple5 _tmp2E6;enum Cyc_Absyn_Sign _tmp2E7;int _tmp2E8;union Cyc_Absyn_Cnst
_tmp2EA;struct _tuple5 _tmp2EB;enum Cyc_Absyn_Sign _tmp2EC;int _tmp2ED;enum Cyc_Absyn_Primop
_tmp2EF;struct Cyc_List_List*_tmp2F0;struct Cyc_List_List _tmp2F1;struct Cyc_Absyn_Exp*
_tmp2F2;_LL11C: {struct Cyc_Absyn_Var_e_struct*_tmp2CF=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CE;if(_tmp2CF->tag != 1)goto _LL11E;else{_tmp2D0=(void*)_tmp2CF->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2D1=(struct Cyc_Absyn_Pat_b_struct*)_tmp2D0;if(_tmp2D1->tag
!= 5)goto _LL11E;else{_tmp2D2=_tmp2D1->f1;}};}}_LL11D: _tmp2D6=_tmp2D2;goto _LL11F;
_LL11E: {struct Cyc_Absyn_Var_e_struct*_tmp2D3=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CE;if(_tmp2D3->tag != 1)goto _LL120;else{_tmp2D4=(void*)_tmp2D3->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2D5=(struct Cyc_Absyn_Local_b_struct*)_tmp2D4;if(
_tmp2D5->tag != 4)goto _LL120;else{_tmp2D6=_tmp2D5->f1;}};}}_LL11F: _tmp2DA=_tmp2D6;
goto _LL121;_LL120: {struct Cyc_Absyn_Var_e_struct*_tmp2D7=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CE;if(_tmp2D7->tag != 1)goto _LL122;else{_tmp2D8=(void*)_tmp2D7->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2D9=(struct Cyc_Absyn_Global_b_struct*)_tmp2D8;if(
_tmp2D9->tag != 1)goto _LL122;else{_tmp2DA=_tmp2D9->f1;}};}}_LL121: _tmp2DE=_tmp2DA;
goto _LL123;_LL122: {struct Cyc_Absyn_Var_e_struct*_tmp2DB=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CE;if(_tmp2DB->tag != 1)goto _LL124;else{_tmp2DC=(void*)_tmp2DB->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp2DD=(struct Cyc_Absyn_Param_b_struct*)_tmp2DC;if(
_tmp2DD->tag != 3)goto _LL124;else{_tmp2DE=_tmp2DD->f1;}};}}_LL123: if(_tmp2DE->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp2DE))return 1;goto _LL11B;_LL124: {
struct Cyc_Absyn_Const_e_struct*_tmp2DF=(struct Cyc_Absyn_Const_e_struct*)_tmp2CE;
if(_tmp2DF->tag != 0)goto _LL126;else{_tmp2E0=_tmp2DF->f1;if((_tmp2E0.Int_c).tag != 
4)goto _LL126;_tmp2E1=(struct _tuple5)(_tmp2E0.Int_c).val;_tmp2E2=_tmp2E1.f1;if(
_tmp2E2 != Cyc_Absyn_None)goto _LL126;_tmp2E3=_tmp2E1.f2;}}_LL125: _tmp2E8=_tmp2E3;
goto _LL127;_LL126: {struct Cyc_Absyn_Const_e_struct*_tmp2E4=(struct Cyc_Absyn_Const_e_struct*)
_tmp2CE;if(_tmp2E4->tag != 0)goto _LL128;else{_tmp2E5=_tmp2E4->f1;if((_tmp2E5.Int_c).tag
!= 4)goto _LL128;_tmp2E6=(struct _tuple5)(_tmp2E5.Int_c).val;_tmp2E7=_tmp2E6.f1;
if(_tmp2E7 != Cyc_Absyn_Signed)goto _LL128;_tmp2E8=_tmp2E6.f2;}}_LL127: return
_tmp2E8 >= 0  && Cyc_Toc_check_const_array((unsigned int)_tmp2E8,vtype);_LL128: {
struct Cyc_Absyn_Const_e_struct*_tmp2E9=(struct Cyc_Absyn_Const_e_struct*)_tmp2CE;
if(_tmp2E9->tag != 0)goto _LL12A;else{_tmp2EA=_tmp2E9->f1;if((_tmp2EA.Int_c).tag != 
4)goto _LL12A;_tmp2EB=(struct _tuple5)(_tmp2EA.Int_c).val;_tmp2EC=_tmp2EB.f1;if(
_tmp2EC != Cyc_Absyn_Unsigned)goto _LL12A;_tmp2ED=_tmp2EB.f2;}}_LL129: return Cyc_Toc_check_const_array((
unsigned int)_tmp2ED,vtype);_LL12A: {struct Cyc_Absyn_Primop_e_struct*_tmp2EE=(
struct Cyc_Absyn_Primop_e_struct*)_tmp2CE;if(_tmp2EE->tag != 3)goto _LL12C;else{
_tmp2EF=_tmp2EE->f1;if(_tmp2EF != Cyc_Absyn_Numelts)goto _LL12C;_tmp2F0=_tmp2EE->f2;
if(_tmp2F0 == 0)goto _LL12C;_tmp2F1=*_tmp2F0;_tmp2F2=(struct Cyc_Absyn_Exp*)_tmp2F1.hd;}}
_LL12B:{void*_tmp2F3=_tmp2F2->r;void*_tmp2F5;struct Cyc_Absyn_Vardecl*_tmp2F7;
void*_tmp2F9;struct Cyc_Absyn_Vardecl*_tmp2FB;void*_tmp2FD;struct Cyc_Absyn_Vardecl*
_tmp2FF;void*_tmp301;struct Cyc_Absyn_Vardecl*_tmp303;_LL12F: {struct Cyc_Absyn_Var_e_struct*
_tmp2F4=(struct Cyc_Absyn_Var_e_struct*)_tmp2F3;if(_tmp2F4->tag != 1)goto _LL131;
else{_tmp2F5=(void*)_tmp2F4->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp2F6=(struct Cyc_Absyn_Pat_b_struct*)
_tmp2F5;if(_tmp2F6->tag != 5)goto _LL131;else{_tmp2F7=_tmp2F6->f1;}};}}_LL130:
_tmp2FB=_tmp2F7;goto _LL132;_LL131: {struct Cyc_Absyn_Var_e_struct*_tmp2F8=(struct
Cyc_Absyn_Var_e_struct*)_tmp2F3;if(_tmp2F8->tag != 1)goto _LL133;else{_tmp2F9=(
void*)_tmp2F8->f2;{struct Cyc_Absyn_Local_b_struct*_tmp2FA=(struct Cyc_Absyn_Local_b_struct*)
_tmp2F9;if(_tmp2FA->tag != 4)goto _LL133;else{_tmp2FB=_tmp2FA->f1;}};}}_LL132:
_tmp2FF=_tmp2FB;goto _LL134;_LL133: {struct Cyc_Absyn_Var_e_struct*_tmp2FC=(struct
Cyc_Absyn_Var_e_struct*)_tmp2F3;if(_tmp2FC->tag != 1)goto _LL135;else{_tmp2FD=(
void*)_tmp2FC->f2;{struct Cyc_Absyn_Global_b_struct*_tmp2FE=(struct Cyc_Absyn_Global_b_struct*)
_tmp2FD;if(_tmp2FE->tag != 1)goto _LL135;else{_tmp2FF=_tmp2FE->f1;}};}}_LL134:
_tmp303=_tmp2FF;goto _LL136;_LL135: {struct Cyc_Absyn_Var_e_struct*_tmp300=(struct
Cyc_Absyn_Var_e_struct*)_tmp2F3;if(_tmp300->tag != 1)goto _LL137;else{_tmp301=(
void*)_tmp300->f2;{struct Cyc_Absyn_Param_b_struct*_tmp302=(struct Cyc_Absyn_Param_b_struct*)
_tmp301;if(_tmp302->tag != 3)goto _LL137;else{_tmp303=_tmp302->f1;}};}}_LL136:
return _tmp303 == v;_LL137:;_LL138: goto _LL12E;_LL12E:;}goto _LL11B;_LL12C:;_LL12D:
goto _LL11B;_LL11B:;}return 0;}static char _tmp315[8]="*bogus*";struct _tuple17{void*
f1;void*f2;};static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,
struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){struct Cyc_Absyn_Vardecl*x;{void*
_tmp304=a->r;void*_tmp306;struct Cyc_Absyn_Vardecl*_tmp308;void*_tmp30A;struct Cyc_Absyn_Vardecl*
_tmp30C;void*_tmp30E;struct Cyc_Absyn_Vardecl*_tmp310;void*_tmp312;struct Cyc_Absyn_Vardecl*
_tmp314;_LL13A: {struct Cyc_Absyn_Var_e_struct*_tmp305=(struct Cyc_Absyn_Var_e_struct*)
_tmp304;if(_tmp305->tag != 1)goto _LL13C;else{_tmp306=(void*)_tmp305->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp307=(struct Cyc_Absyn_Pat_b_struct*)_tmp306;if(_tmp307->tag
!= 5)goto _LL13C;else{_tmp308=_tmp307->f1;}};}}_LL13B: _tmp30C=_tmp308;goto _LL13D;
_LL13C: {struct Cyc_Absyn_Var_e_struct*_tmp309=(struct Cyc_Absyn_Var_e_struct*)
_tmp304;if(_tmp309->tag != 1)goto _LL13E;else{_tmp30A=(void*)_tmp309->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp30B=(struct Cyc_Absyn_Local_b_struct*)_tmp30A;if(
_tmp30B->tag != 4)goto _LL13E;else{_tmp30C=_tmp30B->f1;}};}}_LL13D: _tmp310=_tmp30C;
goto _LL13F;_LL13E: {struct Cyc_Absyn_Var_e_struct*_tmp30D=(struct Cyc_Absyn_Var_e_struct*)
_tmp304;if(_tmp30D->tag != 1)goto _LL140;else{_tmp30E=(void*)_tmp30D->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp30F=(struct Cyc_Absyn_Global_b_struct*)_tmp30E;if(
_tmp30F->tag != 1)goto _LL140;else{_tmp310=_tmp30F->f1;}};}}_LL13F: _tmp314=_tmp310;
goto _LL141;_LL140: {struct Cyc_Absyn_Var_e_struct*_tmp311=(struct Cyc_Absyn_Var_e_struct*)
_tmp304;if(_tmp311->tag != 1)goto _LL142;else{_tmp312=(void*)_tmp311->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp313=(struct Cyc_Absyn_Param_b_struct*)_tmp312;if(
_tmp313->tag != 3)goto _LL142;else{_tmp314=_tmp313->f1;}};}}_LL141: if(_tmp314->escapes)
goto _LL143;x=_tmp314;goto _LL139;_LL142:;_LL143: {static struct _dyneither_ptr
bogus_string={_tmp315,_tmp315,_tmp315 + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
4,0}},& bogus_string};static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,&
bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};x=& bogus_vardecl;
x->type=a_typ;}_LL139:;}{void*_tmp316=a_typ;inner_loop: {void*_tmp317=i->r;void*
_tmp319;struct Cyc_Absyn_Exp*_tmp31A;union Cyc_Absyn_Cnst _tmp31C;struct _tuple5
_tmp31D;enum Cyc_Absyn_Sign _tmp31E;int _tmp31F;union Cyc_Absyn_Cnst _tmp321;struct
_tuple5 _tmp322;enum Cyc_Absyn_Sign _tmp323;int _tmp324;union Cyc_Absyn_Cnst _tmp326;
struct _tuple5 _tmp327;enum Cyc_Absyn_Sign _tmp328;int _tmp329;enum Cyc_Absyn_Primop
_tmp32B;struct Cyc_List_List*_tmp32C;struct Cyc_List_List _tmp32D;struct Cyc_Absyn_Exp*
_tmp32E;struct Cyc_List_List*_tmp32F;struct Cyc_List_List _tmp330;struct Cyc_Absyn_Exp*
_tmp331;void*_tmp333;struct Cyc_Absyn_Vardecl*_tmp335;void*_tmp337;struct Cyc_Absyn_Vardecl*
_tmp339;void*_tmp33B;struct Cyc_Absyn_Vardecl*_tmp33D;void*_tmp33F;struct Cyc_Absyn_Vardecl*
_tmp341;_LL145: {struct Cyc_Absyn_Cast_e_struct*_tmp318=(struct Cyc_Absyn_Cast_e_struct*)
_tmp317;if(_tmp318->tag != 15)goto _LL147;else{_tmp319=(void*)_tmp318->f1;_tmp31A=
_tmp318->f2;}}_LL146: i=_tmp31A;goto inner_loop;_LL147: {struct Cyc_Absyn_Const_e_struct*
_tmp31B=(struct Cyc_Absyn_Const_e_struct*)_tmp317;if(_tmp31B->tag != 0)goto _LL149;
else{_tmp31C=_tmp31B->f1;if((_tmp31C.Int_c).tag != 4)goto _LL149;_tmp31D=(struct
_tuple5)(_tmp31C.Int_c).val;_tmp31E=_tmp31D.f1;if(_tmp31E != Cyc_Absyn_None)goto
_LL149;_tmp31F=_tmp31D.f2;}}_LL148: _tmp324=_tmp31F;goto _LL14A;_LL149: {struct Cyc_Absyn_Const_e_struct*
_tmp320=(struct Cyc_Absyn_Const_e_struct*)_tmp317;if(_tmp320->tag != 0)goto _LL14B;
else{_tmp321=_tmp320->f1;if((_tmp321.Int_c).tag != 4)goto _LL14B;_tmp322=(struct
_tuple5)(_tmp321.Int_c).val;_tmp323=_tmp322.f1;if(_tmp323 != Cyc_Absyn_Signed)
goto _LL14B;_tmp324=_tmp322.f2;}}_LL14A: return _tmp324 >= 0  && Cyc_Toc_check_const_array((
unsigned int)(_tmp324 + 1),_tmp316);_LL14B: {struct Cyc_Absyn_Const_e_struct*
_tmp325=(struct Cyc_Absyn_Const_e_struct*)_tmp317;if(_tmp325->tag != 0)goto _LL14D;
else{_tmp326=_tmp325->f1;if((_tmp326.Int_c).tag != 4)goto _LL14D;_tmp327=(struct
_tuple5)(_tmp326.Int_c).val;_tmp328=_tmp327.f1;if(_tmp328 != Cyc_Absyn_Unsigned)
goto _LL14D;_tmp329=_tmp327.f2;}}_LL14C: return Cyc_Toc_check_const_array((
unsigned int)(_tmp329 + 1),_tmp316);_LL14D: {struct Cyc_Absyn_Primop_e_struct*
_tmp32A=(struct Cyc_Absyn_Primop_e_struct*)_tmp317;if(_tmp32A->tag != 3)goto _LL14F;
else{_tmp32B=_tmp32A->f1;if(_tmp32B != Cyc_Absyn_Mod)goto _LL14F;_tmp32C=_tmp32A->f2;
if(_tmp32C == 0)goto _LL14F;_tmp32D=*_tmp32C;_tmp32E=(struct Cyc_Absyn_Exp*)_tmp32D.hd;
_tmp32F=_tmp32D.tl;if(_tmp32F == 0)goto _LL14F;_tmp330=*_tmp32F;_tmp331=(struct Cyc_Absyn_Exp*)
_tmp330.hd;}}_LL14E: return Cyc_Toc_check_leq_size(relns,x,_tmp331,_tmp316);_LL14F: {
struct Cyc_Absyn_Var_e_struct*_tmp332=(struct Cyc_Absyn_Var_e_struct*)_tmp317;if(
_tmp332->tag != 1)goto _LL151;else{_tmp333=(void*)_tmp332->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp334=(struct Cyc_Absyn_Pat_b_struct*)_tmp333;if(_tmp334->tag != 5)goto _LL151;
else{_tmp335=_tmp334->f1;}};}}_LL150: _tmp339=_tmp335;goto _LL152;_LL151: {struct
Cyc_Absyn_Var_e_struct*_tmp336=(struct Cyc_Absyn_Var_e_struct*)_tmp317;if(_tmp336->tag
!= 1)goto _LL153;else{_tmp337=(void*)_tmp336->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp338=(struct Cyc_Absyn_Local_b_struct*)_tmp337;if(_tmp338->tag != 4)goto _LL153;
else{_tmp339=_tmp338->f1;}};}}_LL152: _tmp33D=_tmp339;goto _LL154;_LL153: {struct
Cyc_Absyn_Var_e_struct*_tmp33A=(struct Cyc_Absyn_Var_e_struct*)_tmp317;if(_tmp33A->tag
!= 1)goto _LL155;else{_tmp33B=(void*)_tmp33A->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp33C=(struct Cyc_Absyn_Global_b_struct*)_tmp33B;if(_tmp33C->tag != 1)goto _LL155;
else{_tmp33D=_tmp33C->f1;}};}}_LL154: _tmp341=_tmp33D;goto _LL156;_LL155: {struct
Cyc_Absyn_Var_e_struct*_tmp33E=(struct Cyc_Absyn_Var_e_struct*)_tmp317;if(_tmp33E->tag
!= 1)goto _LL157;else{_tmp33F=(void*)_tmp33E->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp340=(struct Cyc_Absyn_Param_b_struct*)_tmp33F;if(_tmp340->tag != 3)goto _LL157;
else{_tmp341=_tmp340->f1;}};}}_LL156: if(_tmp341->escapes)return 0;{struct Cyc_List_List*
_tmp342=relns;for(0;_tmp342 != 0;_tmp342=_tmp342->tl){struct Cyc_CfFlowInfo_Reln*
_tmp343=(struct Cyc_CfFlowInfo_Reln*)_tmp342->hd;if(_tmp343->vd == _tmp341){union
Cyc_CfFlowInfo_RelnOp _tmp344=_tmp343->rop;struct Cyc_Absyn_Vardecl*_tmp345;struct
_tuple11 _tmp346;struct Cyc_Absyn_Vardecl*_tmp347;void*_tmp348;unsigned int _tmp349;
unsigned int _tmp34A;_LL15A: if((_tmp344.LessNumelts).tag != 3)goto _LL15C;_tmp345=(
struct Cyc_Absyn_Vardecl*)(_tmp344.LessNumelts).val;_LL15B: if(x == _tmp345)return 1;
else{goto _LL159;}_LL15C: if((_tmp344.LessVar).tag != 2)goto _LL15E;_tmp346=(struct
_tuple11)(_tmp344.LessVar).val;_tmp347=_tmp346.f1;_tmp348=_tmp346.f2;_LL15D:{
struct _tuple17 _tmpB69;struct _tuple17 _tmp34C=(_tmpB69.f1=Cyc_Tcutil_compress(
_tmp348),((_tmpB69.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(a->topt))->v),_tmpB69)));void*_tmp34D;void*_tmp34F;void*_tmp350;
struct Cyc_Absyn_PtrInfo _tmp352;struct Cyc_Absyn_PtrAtts _tmp353;union Cyc_Absyn_Constraint*
_tmp354;_LL165: _tmp34D=_tmp34C.f1;{struct Cyc_Absyn_TagType_struct*_tmp34E=(
struct Cyc_Absyn_TagType_struct*)_tmp34D;if(_tmp34E->tag != 20)goto _LL167;else{
_tmp34F=(void*)_tmp34E->f1;}};_tmp350=_tmp34C.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp351=(struct Cyc_Absyn_PointerType_struct*)_tmp350;if(_tmp351->tag != 5)goto
_LL167;else{_tmp352=_tmp351->f1;_tmp353=_tmp352.ptr_atts;_tmp354=_tmp353.bounds;}};
_LL166:{void*_tmp355=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp354);struct Cyc_Absyn_Exp*_tmp357;_LL16A: {struct Cyc_Absyn_Upper_b_struct*
_tmp356=(struct Cyc_Absyn_Upper_b_struct*)_tmp355;if(_tmp356->tag != 1)goto _LL16C;
else{_tmp357=_tmp356->f1;}}_LL16B: {struct Cyc_Absyn_Exp*_tmp358=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp34F,0),0,Cyc_Absyn_No_coercion,0);
if(Cyc_Evexp_lte_const_exp(_tmp358,_tmp357))return 1;goto _LL169;}_LL16C:;_LL16D:
goto _LL169;_LL169:;}goto _LL164;_LL167:;_LL168: goto _LL164;_LL164:;}{struct Cyc_List_List*
_tmp359=relns;for(0;_tmp359 != 0;_tmp359=_tmp359->tl){struct Cyc_CfFlowInfo_Reln*
_tmp35A=(struct Cyc_CfFlowInfo_Reln*)_tmp359->hd;if(_tmp35A->vd == _tmp347){union
Cyc_CfFlowInfo_RelnOp _tmp35B=_tmp35A->rop;struct Cyc_Absyn_Vardecl*_tmp35C;struct
Cyc_Absyn_Vardecl*_tmp35D;unsigned int _tmp35E;unsigned int _tmp35F;struct _tuple11
_tmp360;struct Cyc_Absyn_Vardecl*_tmp361;_LL16F: if((_tmp35B.LessEqNumelts).tag != 
5)goto _LL171;_tmp35C=(struct Cyc_Absyn_Vardecl*)(_tmp35B.LessEqNumelts).val;
_LL170: _tmp35D=_tmp35C;goto _LL172;_LL171: if((_tmp35B.LessNumelts).tag != 3)goto
_LL173;_tmp35D=(struct Cyc_Absyn_Vardecl*)(_tmp35B.LessNumelts).val;_LL172: if(x == 
_tmp35D)return 1;goto _LL16E;_LL173: if((_tmp35B.EqualConst).tag != 1)goto _LL175;
_tmp35E=(unsigned int)(_tmp35B.EqualConst).val;_LL174: return Cyc_Toc_check_const_array(
_tmp35E,_tmp316);_LL175: if((_tmp35B.LessEqConst).tag != 6)goto _LL177;_tmp35F=(
unsigned int)(_tmp35B.LessEqConst).val;if(!(_tmp35F > 0))goto _LL177;_LL176: return
Cyc_Toc_check_const_array(_tmp35F,_tmp316);_LL177: if((_tmp35B.LessVar).tag != 2)
goto _LL179;_tmp360=(struct _tuple11)(_tmp35B.LessVar).val;_tmp361=_tmp360.f1;
_LL178: if(Cyc_Toc_check_leq_size_var(relns,x,_tmp361))return 1;goto _LL16E;_LL179:;
_LL17A: goto _LL16E;_LL16E:;}}}goto _LL159;_LL15E: if((_tmp344.LessConst).tag != 4)
goto _LL160;_tmp349=(unsigned int)(_tmp344.LessConst).val;_LL15F: return Cyc_Toc_check_const_array(
_tmp349,_tmp316);_LL160: if((_tmp344.LessEqConst).tag != 6)goto _LL162;_tmp34A=(
unsigned int)(_tmp344.LessEqConst).val;_LL161: return Cyc_Toc_check_const_array(
_tmp34A + 1,_tmp316);_LL162:;_LL163: goto _LL159;_LL159:;}}}goto _LL144;_LL157:;
_LL158: goto _LL144;_LL144:;}return 0;};}static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*
e){if(e->topt == 0){const char*_tmpB6C;void*_tmpB6B;(_tmpB6B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6C="Missing type in primop ",
_tag_dyneither(_tmpB6C,sizeof(char),24))),_tag_dyneither(_tmpB6B,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*
_tmpB6F;void*_tmpB6E;(_tmpB6E=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6F="Missing type in primop ",
_tag_dyneither(_tmpB6F,sizeof(char),24))),_tag_dyneither(_tmpB6E,sizeof(void*),0)));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple9*
Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){struct _tuple9*_tmpB70;return(_tmpB70=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB70)),((_tmpB70->f1=Cyc_Toc_mt_tq,((
_tmpB70->f2=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmpB70)))));}static struct _tuple15*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(nv,e);{struct _tuple15*_tmpB71;return(
_tmpB71=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB71)),((_tmpB71->f1=0,((
_tmpB71->f2=e,_tmpB71)))));};}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(
struct Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int
pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;
if(pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp368=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0  || Cyc_Absyn_no_regions)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(
_tmp368): Cyc_Toc_malloc_ptr(_tmp368));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp368);}};}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp369=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp369 != 0;_tmp369=_tmp369->tl){struct _tuple15 _tmp36B;struct Cyc_List_List*
_tmp36C;struct Cyc_Absyn_Exp*_tmp36D;struct _tuple15*_tmp36A=(struct _tuple15*)
_tmp369->hd;_tmp36B=*_tmp36A;_tmp36C=_tmp36B.f1;_tmp36D=_tmp36B.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp36C == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp36C->tl != 0){const char*_tmpB74;void*_tmpB73;(_tmpB73=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB74="multiple designators in array",
_tag_dyneither(_tmpB74,sizeof(char),30))),_tag_dyneither(_tmpB73,sizeof(void*),0)));}{
void*_tmp370=(void*)_tmp36C->hd;void*_tmp371=_tmp370;struct Cyc_Absyn_Exp*_tmp373;
_LL17C: {struct Cyc_Absyn_ArrayElement_struct*_tmp372=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp371;if(_tmp372->tag != 0)goto _LL17E;else{_tmp373=_tmp372->f1;}}_LL17D: Cyc_Toc_exp_to_c(
nv,_tmp373);e_index=_tmp373;goto _LL17B;_LL17E: {struct Cyc_Absyn_FieldName_struct*
_tmp374=(struct Cyc_Absyn_FieldName_struct*)_tmp371;if(_tmp374->tag != 1)goto
_LL17B;}_LL17F: {const char*_tmpB77;void*_tmpB76;(_tmpB76=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB77="field name designators in array",
_tag_dyneither(_tmpB77,sizeof(char),32))),_tag_dyneither(_tmpB76,sizeof(void*),0)));}
_LL17B:;};}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*
_tmp377=_tmp36D->r;struct Cyc_List_List*_tmp379;struct Cyc_Absyn_Vardecl*_tmp37B;
struct Cyc_Absyn_Exp*_tmp37C;struct Cyc_Absyn_Exp*_tmp37D;int _tmp37E;void*_tmp380;
struct Cyc_List_List*_tmp381;_LL181: {struct Cyc_Absyn_Array_e_struct*_tmp378=(
struct Cyc_Absyn_Array_e_struct*)_tmp377;if(_tmp378->tag != 27)goto _LL183;else{
_tmp379=_tmp378->f1;}}_LL182: s=Cyc_Toc_init_array(nv,lval,_tmp379,s);goto _LL180;
_LL183: {struct Cyc_Absyn_Comprehension_e_struct*_tmp37A=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp377;if(_tmp37A->tag != 28)goto _LL185;else{_tmp37B=_tmp37A->f1;_tmp37C=_tmp37A->f2;
_tmp37D=_tmp37A->f3;_tmp37E=_tmp37A->f4;}}_LL184: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp37B,_tmp37C,_tmp37D,_tmp37E,s,0);goto _LL180;_LL185: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp37F=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp377;if(_tmp37F->tag != 30)goto
_LL187;else{_tmp380=(void*)_tmp37F->f1;_tmp381=_tmp37F->f2;}}_LL186: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp380,_tmp381,s);goto _LL180;_LL187:;_LL188: Cyc_Toc_exp_to_c(nv,_tmp36D);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
_tmp36D,0),s,0);goto _LL180;_LL180:;};};}}return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple0*_tmp382=vd->name;void*_tmp383=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env _tmp385;struct _RegionHandle*_tmp386;
struct Cyc_Toc_Env*_tmp384=nv;_tmp385=*_tmp384;_tmp386=_tmp385.rgn;{struct Cyc_Absyn_Local_b_struct
_tmpB7A;struct Cyc_Absyn_Local_b_struct*_tmpB79;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
_tmp386,nv,_tmp382,Cyc_Absyn_varb_exp(_tmp382,(void*)((_tmpB79=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmpB79)),((_tmpB79[0]=((_tmpB7A.tag=4,((_tmpB7A.f1=
vd,_tmpB7A)))),_tmpB79)))),0));struct _tuple0*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp382,0),Cyc_Absyn_signed_int_exp(0,
0),0);struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp382,0),Cyc_Absyn_var_exp(
max,0),0);struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(
_tmp382,0),0);struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(
_tmp382,0),0);struct Cyc_Absyn_Stmt*body;{void*_tmp387=e2->r;struct Cyc_List_List*
_tmp389;struct Cyc_Absyn_Vardecl*_tmp38B;struct Cyc_Absyn_Exp*_tmp38C;struct Cyc_Absyn_Exp*
_tmp38D;int _tmp38E;void*_tmp390;struct Cyc_List_List*_tmp391;_LL18A: {struct Cyc_Absyn_Array_e_struct*
_tmp388=(struct Cyc_Absyn_Array_e_struct*)_tmp387;if(_tmp388->tag != 27)goto _LL18C;
else{_tmp389=_tmp388->f1;}}_LL18B: body=Cyc_Toc_init_array(nv2,lval,_tmp389,Cyc_Toc_skip_stmt_dl());
goto _LL189;_LL18C: {struct Cyc_Absyn_Comprehension_e_struct*_tmp38A=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp387;if(_tmp38A->tag != 28)goto _LL18E;else{_tmp38B=_tmp38A->f1;_tmp38C=_tmp38A->f2;
_tmp38D=_tmp38A->f3;_tmp38E=_tmp38A->f4;}}_LL18D: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp38B,_tmp38C,_tmp38D,_tmp38E,Cyc_Toc_skip_stmt_dl(),0);goto _LL189;
_LL18E: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp38F=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp387;if(_tmp38F->tag != 30)goto _LL190;else{_tmp390=(void*)_tmp38F->f1;_tmp391=
_tmp38F->f2;}}_LL18F: body=Cyc_Toc_init_anon_struct(nv,lval,_tmp390,_tmp391,Cyc_Toc_skip_stmt_dl());
goto _LL189;_LL190:;_LL191: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL189;_LL189:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp383,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp382,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp394=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp394 != 0;_tmp394=_tmp394->tl){struct _tuple15 _tmp396;
struct Cyc_List_List*_tmp397;struct Cyc_Absyn_Exp*_tmp398;struct _tuple15*_tmp395=(
struct _tuple15*)_tmp394->hd;_tmp396=*_tmp395;_tmp397=_tmp396.f1;_tmp398=_tmp396.f2;
if(_tmp397 == 0){const char*_tmpB7D;void*_tmpB7C;(_tmpB7C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB7D="empty designator list",
_tag_dyneither(_tmpB7D,sizeof(char),22))),_tag_dyneither(_tmpB7C,sizeof(void*),0)));}
if(_tmp397->tl != 0){const char*_tmpB80;void*_tmpB7F;(_tmpB7F=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB80="too many designators in anonymous struct",
_tag_dyneither(_tmpB80,sizeof(char),41))),_tag_dyneither(_tmpB7F,sizeof(void*),0)));}{
void*_tmp39D=(void*)_tmp397->hd;struct _dyneither_ptr*_tmp39F;_LL193: {struct Cyc_Absyn_FieldName_struct*
_tmp39E=(struct Cyc_Absyn_FieldName_struct*)_tmp39D;if(_tmp39E->tag != 1)goto
_LL195;else{_tmp39F=_tmp39E->f1;}}_LL194: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(
lhs,_tmp39F,0);{void*_tmp3A0=_tmp398->r;struct Cyc_List_List*_tmp3A2;struct Cyc_Absyn_Vardecl*
_tmp3A4;struct Cyc_Absyn_Exp*_tmp3A5;struct Cyc_Absyn_Exp*_tmp3A6;int _tmp3A7;void*
_tmp3A9;struct Cyc_List_List*_tmp3AA;_LL198: {struct Cyc_Absyn_Array_e_struct*
_tmp3A1=(struct Cyc_Absyn_Array_e_struct*)_tmp3A0;if(_tmp3A1->tag != 27)goto _LL19A;
else{_tmp3A2=_tmp3A1->f1;}}_LL199: s=Cyc_Toc_init_array(nv,lval,_tmp3A2,s);goto
_LL197;_LL19A: {struct Cyc_Absyn_Comprehension_e_struct*_tmp3A3=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3A0;if(_tmp3A3->tag != 28)goto _LL19C;else{_tmp3A4=_tmp3A3->f1;_tmp3A5=_tmp3A3->f2;
_tmp3A6=_tmp3A3->f3;_tmp3A7=_tmp3A3->f4;}}_LL19B: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp3A4,_tmp3A5,_tmp3A6,_tmp3A7,s,0);goto _LL197;_LL19C: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3A8=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3A0;if(_tmp3A8->tag != 30)goto
_LL19E;else{_tmp3A9=(void*)_tmp3A8->f1;_tmp3AA=_tmp3A8->f2;}}_LL19D: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3A9,_tmp3AA,s);goto _LL197;_LL19E:;_LL19F: Cyc_Toc_exp_to_c(nv,_tmp398);
if(Cyc_Toc_is_poly_field(struct_type,_tmp39F))_tmp398=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp398);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp398,0),0),s,0);goto _LL197;_LL197:;}goto _LL192;}_LL195:;_LL196: {const char*
_tmpB83;void*_tmpB82;(_tmpB82=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB83="array designator in struct",
_tag_dyneither(_tmpB83,sizeof(char),27))),_tag_dyneither(_tmpB82,sizeof(void*),0)));}
_LL192:;};}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct Cyc_Toc_Env
_tmp3AE;struct _RegionHandle*_tmp3AF;struct Cyc_Toc_Env*_tmp3AD=nv;_tmp3AE=*
_tmp3AD;_tmp3AF=_tmp3AE.rgn;{struct Cyc_List_List*_tmp3B0=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_rmap)(_tmp3AF,Cyc_Toc_tup_to_c,es);void*_tmp3B1=Cyc_Toc_add_tuple_type(
_tmp3B0);struct _tuple0*_tmp3B2=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp3B3=
Cyc_Absyn_var_exp(_tmp3B2,0);struct Cyc_Absyn_Stmt*_tmp3B4=Cyc_Absyn_exp_stmt(
_tmp3B3,0);struct Cyc_Absyn_Exp*(*_tmp3B5)(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp3B6=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp3AF,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp3B6);for(0;_tmp3B6 != 0;(_tmp3B6=
_tmp3B6->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp3B6->hd;struct
Cyc_Absyn_Exp*lval=_tmp3B5(_tmp3B3,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp3B7=e->r;struct Cyc_List_List*_tmp3B9;struct Cyc_Absyn_Vardecl*_tmp3BB;
struct Cyc_Absyn_Exp*_tmp3BC;struct Cyc_Absyn_Exp*_tmp3BD;int _tmp3BE;_LL1A1: {
struct Cyc_Absyn_Array_e_struct*_tmp3B8=(struct Cyc_Absyn_Array_e_struct*)_tmp3B7;
if(_tmp3B8->tag != 27)goto _LL1A3;else{_tmp3B9=_tmp3B8->f1;}}_LL1A2: _tmp3B4=Cyc_Toc_init_array(
nv,lval,_tmp3B9,_tmp3B4);goto _LL1A0;_LL1A3: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp3BA=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp3B7;if(_tmp3BA->tag != 28)
goto _LL1A5;else{_tmp3BB=_tmp3BA->f1;_tmp3BC=_tmp3BA->f2;_tmp3BD=_tmp3BA->f3;
_tmp3BE=_tmp3BA->f4;}}_LL1A4: _tmp3B4=Cyc_Toc_init_comprehension(nv,lval,_tmp3BB,
_tmp3BC,_tmp3BD,_tmp3BE,_tmp3B4,0);goto _LL1A0;_LL1A5:;_LL1A6: Cyc_Toc_exp_to_c(nv,
e);_tmp3B4=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3B5(
_tmp3B3,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp3B4,0);goto _LL1A0;_LL1A0:;};}}
return Cyc_Toc_make_struct(nv,_tmp3B2,_tmp3B1,_tmp3B4,pointer,rgnopt,is_atomic);};}
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct
_dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp3BF=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp3BF != 0;_tmp3BF=_tmp3BF->tl){struct Cyc_Absyn_Aggrfield*
_tmp3C0=(struct Cyc_Absyn_Aggrfield*)_tmp3BF->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp3C0->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}{
struct Cyc_String_pa_struct _tmpB8B;void*_tmpB8A[1];const char*_tmpB89;void*_tmpB88;(
_tmpB88=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)((_tmpB8B.tag=0,((_tmpB8B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmpB8A[0]=& _tmpB8B,Cyc_aprintf(((_tmpB89="get_member_offset %s failed",
_tag_dyneither(_tmpB89,sizeof(char),28))),_tag_dyneither(_tmpB8A,sizeof(void*),1)))))))),
_tag_dyneither(_tmpB88,sizeof(void*),0)));};}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){struct _tuple0*_tmp3C5=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3C6=Cyc_Absyn_var_exp(_tmp3C5,0);struct Cyc_Absyn_Stmt*
_tmp3C7=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp3C6),0);struct Cyc_Absyn_Exp*(*
_tmp3C8)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*)=
pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;void*_tmp3C9=Cyc_Toc_aggrdecl_type(
tdn,Cyc_Absyn_strctq);int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp3CA=
Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp3CC;union Cyc_Absyn_AggrInfoU
_tmp3CD;_LL1A8: {struct Cyc_Absyn_AggrType_struct*_tmp3CB=(struct Cyc_Absyn_AggrType_struct*)
_tmp3CA;if(_tmp3CB->tag != 12)goto _LL1AA;else{_tmp3CC=_tmp3CB->f1;_tmp3CD=_tmp3CC.aggr_info;}}
_LL1A9: ad=Cyc_Absyn_get_known_aggrdecl(_tmp3CD);goto _LL1A7;_LL1AA:;_LL1AB: {
const char*_tmpB8E;void*_tmpB8D;(_tmpB8D=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8E="init_struct: bad struct type",
_tag_dyneither(_tmpB8E,sizeof(char),29))),_tag_dyneither(_tmpB8D,sizeof(void*),0)));}
_LL1A7:;}{int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct Cyc_Toc_Env _tmp3D1;struct _RegionHandle*_tmp3D2;struct Cyc_Toc_Env*_tmp3D0=
nv;_tmp3D1=*_tmp3D0;_tmp3D2=_tmp3D1.rgn;{struct Cyc_List_List*_tmp3D3=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp3D2,dles);for(0;
_tmp3D3 != 0;_tmp3D3=_tmp3D3->tl){struct _tuple15 _tmp3D5;struct Cyc_List_List*
_tmp3D6;struct Cyc_Absyn_Exp*_tmp3D7;struct _tuple15*_tmp3D4=(struct _tuple15*)
_tmp3D3->hd;_tmp3D5=*_tmp3D4;_tmp3D6=_tmp3D5.f1;_tmp3D7=_tmp3D5.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3D7->topt))->v);
if(_tmp3D6 == 0){const char*_tmpB91;void*_tmpB90;(_tmpB90=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB91="empty designator list",
_tag_dyneither(_tmpB91,sizeof(char),22))),_tag_dyneither(_tmpB90,sizeof(void*),0)));}
if(_tmp3D6->tl != 0){struct _tuple0*_tmp3DA=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp3DB=Cyc_Absyn_var_exp(_tmp3DA,0);for(0;_tmp3D6 != 0;_tmp3D6=_tmp3D6->tl){void*
_tmp3DC=(void*)_tmp3D6->hd;struct _dyneither_ptr*_tmp3DE;_LL1AD: {struct Cyc_Absyn_FieldName_struct*
_tmp3DD=(struct Cyc_Absyn_FieldName_struct*)_tmp3DC;if(_tmp3DD->tag != 1)goto
_LL1AF;else{_tmp3DE=_tmp3DD->f1;}}_LL1AE: if(Cyc_Toc_is_poly_field(struct_type,
_tmp3DE))_tmp3DB=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3DB);_tmp3C7=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3C8(_tmp3C6,_tmp3DE,0),_tmp3DB,0),0),
_tmp3C7,0);goto _LL1AC;_LL1AF:;_LL1B0: {const char*_tmpB94;void*_tmpB93;(_tmpB93=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB94="array designator in struct",_tag_dyneither(_tmpB94,sizeof(char),27))),
_tag_dyneither(_tmpB93,sizeof(void*),0)));}_LL1AC:;}Cyc_Toc_exp_to_c(nv,_tmp3D7);
_tmp3C7=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3DB,
_tmp3D7,0),0),_tmp3C7,0);}else{void*_tmp3E1=(void*)_tmp3D6->hd;struct
_dyneither_ptr*_tmp3E3;_LL1B2: {struct Cyc_Absyn_FieldName_struct*_tmp3E2=(struct
Cyc_Absyn_FieldName_struct*)_tmp3E1;if(_tmp3E2->tag != 1)goto _LL1B4;else{_tmp3E3=
_tmp3E2->f1;}}_LL1B3: {struct Cyc_Absyn_Exp*lval=_tmp3C8(_tmp3C6,_tmp3E3,0);if(
is_tagged_union){int i=Cyc_Toc_get_member_offset(ad,_tmp3E3);struct Cyc_Absyn_Exp*
f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_aggrmember_exp(
lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,
f_tag_exp,0);_tmp3C7=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3C7,0);
lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{void*_tmp3E4=_tmp3D7->r;
struct Cyc_List_List*_tmp3E6;struct Cyc_Absyn_Vardecl*_tmp3E8;struct Cyc_Absyn_Exp*
_tmp3E9;struct Cyc_Absyn_Exp*_tmp3EA;int _tmp3EB;void*_tmp3ED;struct Cyc_List_List*
_tmp3EE;_LL1B7: {struct Cyc_Absyn_Array_e_struct*_tmp3E5=(struct Cyc_Absyn_Array_e_struct*)
_tmp3E4;if(_tmp3E5->tag != 27)goto _LL1B9;else{_tmp3E6=_tmp3E5->f1;}}_LL1B8:
_tmp3C7=Cyc_Toc_init_array(nv,lval,_tmp3E6,_tmp3C7);goto _LL1B6;_LL1B9: {struct
Cyc_Absyn_Comprehension_e_struct*_tmp3E7=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3E4;if(_tmp3E7->tag != 28)goto _LL1BB;else{_tmp3E8=_tmp3E7->f1;_tmp3E9=_tmp3E7->f2;
_tmp3EA=_tmp3E7->f3;_tmp3EB=_tmp3E7->f4;}}_LL1BA: _tmp3C7=Cyc_Toc_init_comprehension(
nv,lval,_tmp3E8,_tmp3E9,_tmp3EA,_tmp3EB,_tmp3C7,0);goto _LL1B6;_LL1BB: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3EC=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3E4;if(_tmp3EC->tag != 30)goto
_LL1BD;else{_tmp3ED=(void*)_tmp3EC->f1;_tmp3EE=_tmp3EC->f2;}}_LL1BC: _tmp3C7=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3ED,_tmp3EE,_tmp3C7);goto _LL1B6;_LL1BD:;_LL1BE: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3D7->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp3D7);{struct Cyc_Absyn_Aggrfield*_tmp3EF=Cyc_Absyn_lookup_decl_field(ad,
_tmp3E3);if(Cyc_Toc_is_poly_field(struct_type,_tmp3E3) && !was_ptr_type)_tmp3D7=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3D7);if(has_exists)_tmp3D7=Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3EF))->type),
_tmp3D7);_tmp3C7=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp3D7,0),0),_tmp3C7,0);goto _LL1B6;};}_LL1B6:;}goto _LL1B1;}_LL1B4:;_LL1B5: {
const char*_tmpB97;void*_tmpB96;(_tmpB96=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB97="array designator in struct",
_tag_dyneither(_tmpB97,sizeof(char),27))),_tag_dyneither(_tmpB96,sizeof(void*),0)));}
_LL1B1:;}}}return Cyc_Toc_make_struct(nv,_tmp3C5,_tmp3C9,_tmp3C7,pointer,rgnopt,
is_atomic);};}struct _tuple18{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct
_tuple18*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(
e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,
enum Cyc_Absyn_Incrementor incr){struct Cyc_Absyn_Increment_e_struct _tmpB9A;struct
Cyc_Absyn_Increment_e_struct*_tmpB99;return Cyc_Absyn_new_exp((void*)((_tmpB99=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB99)),((_tmpB99[0]=((_tmpB9A.tag=
5,((_tmpB9A.f1=e1,((_tmpB9A.f2=incr,_tmpB9A)))))),_tmpB99)))),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp3F4=e1->r;struct Cyc_Absyn_Stmt*_tmp3F6;void*_tmp3F8;struct Cyc_Absyn_Exp*
_tmp3F9;struct Cyc_Absyn_Exp*_tmp3FB;struct _dyneither_ptr*_tmp3FC;int _tmp3FD;int
_tmp3FE;_LL1C0: {struct Cyc_Absyn_StmtExp_e_struct*_tmp3F5=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp3F4;if(_tmp3F5->tag != 37)goto _LL1C2;else{_tmp3F6=_tmp3F5->f1;}}_LL1C1: Cyc_Toc_lvalue_assign_stmt(
_tmp3F6,fs,f,f_env);goto _LL1BF;_LL1C2: {struct Cyc_Absyn_Cast_e_struct*_tmp3F7=(
struct Cyc_Absyn_Cast_e_struct*)_tmp3F4;if(_tmp3F7->tag != 15)goto _LL1C4;else{
_tmp3F8=(void*)_tmp3F7->f1;_tmp3F9=_tmp3F7->f2;}}_LL1C3: Cyc_Toc_lvalue_assign(
_tmp3F9,fs,f,f_env);goto _LL1BF;_LL1C4: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp3FA=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp3F4;if(_tmp3FA->tag != 22)goto
_LL1C6;else{_tmp3FB=_tmp3FA->f1;_tmp3FC=_tmp3FA->f2;_tmp3FD=_tmp3FA->f3;_tmp3FE=
_tmp3FA->f4;}}_LL1C5: e1->r=_tmp3FB->r;{struct Cyc_List_List*_tmpB9B;Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)((_tmpB9B=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpB9B)),((_tmpB9B->hd=_tmp3FC,((_tmpB9B->tl=fs,_tmpB9B)))))),f,f_env);}goto
_LL1BF;_LL1C6:;_LL1C7: {struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;
fs != 0;fs=fs->tl){e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _dyneither_ptr*)
fs->hd,e1_copy->loc);}e1->r=(f(e1_copy,f_env))->r;goto _LL1BF;}_LL1BF:;}static
void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp400=s->r;
struct Cyc_Absyn_Exp*_tmp402;struct Cyc_Absyn_Decl*_tmp404;struct Cyc_Absyn_Stmt*
_tmp405;struct Cyc_Absyn_Stmt*_tmp407;_LL1C9: {struct Cyc_Absyn_Exp_s_struct*
_tmp401=(struct Cyc_Absyn_Exp_s_struct*)_tmp400;if(_tmp401->tag != 1)goto _LL1CB;
else{_tmp402=_tmp401->f1;}}_LL1CA: Cyc_Toc_lvalue_assign(_tmp402,fs,f,f_env);goto
_LL1C8;_LL1CB: {struct Cyc_Absyn_Decl_s_struct*_tmp403=(struct Cyc_Absyn_Decl_s_struct*)
_tmp400;if(_tmp403->tag != 12)goto _LL1CD;else{_tmp404=_tmp403->f1;_tmp405=_tmp403->f2;}}
_LL1CC: Cyc_Toc_lvalue_assign_stmt(_tmp405,fs,f,f_env);goto _LL1C8;_LL1CD: {struct
Cyc_Absyn_Seq_s_struct*_tmp406=(struct Cyc_Absyn_Seq_s_struct*)_tmp400;if(_tmp406->tag
!= 2)goto _LL1CF;else{_tmp407=_tmp406->f2;}}_LL1CE: Cyc_Toc_lvalue_assign_stmt(
_tmp407,fs,f,f_env);goto _LL1C8;_LL1CF:;_LL1D0: {const char*_tmpB9F;void*_tmpB9E[1];
struct Cyc_String_pa_struct _tmpB9D;(_tmpB9D.tag=0,((_tmpB9D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB9E[0]=&
_tmpB9D,Cyc_Toc_toc_impos(((_tmpB9F="lvalue_assign_stmt: %s",_tag_dyneither(
_tmpB9F,sizeof(char),23))),_tag_dyneither(_tmpB9E,sizeof(void*),1)))))));}_LL1C8:;}
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Exp*
Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){void*_tmp40B=e->r;void*_tmp40D;
void**_tmp40E;struct Cyc_Absyn_Exp*_tmp40F;struct Cyc_Absyn_Exp**_tmp410;struct Cyc_Absyn_Exp*
_tmp412;struct Cyc_Absyn_Stmt*_tmp414;_LL1D2: {struct Cyc_Absyn_Cast_e_struct*
_tmp40C=(struct Cyc_Absyn_Cast_e_struct*)_tmp40B;if(_tmp40C->tag != 15)goto _LL1D4;
else{_tmp40D=(void**)& _tmp40C->f1;_tmp40E=(void**)((void**)& _tmp40C->f1);_tmp40F=
_tmp40C->f2;_tmp410=(struct Cyc_Absyn_Exp**)& _tmp40C->f2;}}_LL1D3:*_tmp410=Cyc_Toc_push_address_exp(*
_tmp410);*_tmp40E=Cyc_Absyn_cstar_typ(*_tmp40E,Cyc_Toc_mt_tq);return e;_LL1D4: {
struct Cyc_Absyn_Deref_e_struct*_tmp411=(struct Cyc_Absyn_Deref_e_struct*)_tmp40B;
if(_tmp411->tag != 21)goto _LL1D6;else{_tmp412=_tmp411->f1;}}_LL1D5: return _tmp412;
_LL1D6: {struct Cyc_Absyn_StmtExp_e_struct*_tmp413=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp40B;if(_tmp413->tag != 37)goto _LL1D8;else{_tmp414=_tmp413->f1;}}_LL1D7: Cyc_Toc_push_address_stmt(
_tmp414);return e;_LL1D8:;_LL1D9: if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(
e,0);{const char*_tmpBA3;void*_tmpBA2[1];struct Cyc_String_pa_struct _tmpBA1;(
_tmpBA1.tag=0,((_tmpBA1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpBA2[0]=& _tmpBA1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpBA3="can't take & of exp %s",_tag_dyneither(_tmpBA3,
sizeof(char),23))),_tag_dyneither(_tmpBA2,sizeof(void*),1)))))));};_LL1D1:;}
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){void*_tmp418=s->r;
struct Cyc_Absyn_Stmt*_tmp41A;struct Cyc_Absyn_Stmt*_tmp41C;struct Cyc_Absyn_Exp*
_tmp41E;struct Cyc_Absyn_Exp**_tmp41F;_LL1DB: {struct Cyc_Absyn_Seq_s_struct*
_tmp419=(struct Cyc_Absyn_Seq_s_struct*)_tmp418;if(_tmp419->tag != 2)goto _LL1DD;
else{_tmp41A=_tmp419->f2;}}_LL1DC: _tmp41C=_tmp41A;goto _LL1DE;_LL1DD: {struct Cyc_Absyn_Decl_s_struct*
_tmp41B=(struct Cyc_Absyn_Decl_s_struct*)_tmp418;if(_tmp41B->tag != 12)goto _LL1DF;
else{_tmp41C=_tmp41B->f2;}}_LL1DE: Cyc_Toc_push_address_stmt(_tmp41C);goto _LL1DA;
_LL1DF: {struct Cyc_Absyn_Exp_s_struct*_tmp41D=(struct Cyc_Absyn_Exp_s_struct*)
_tmp418;if(_tmp41D->tag != 1)goto _LL1E1;else{_tmp41E=_tmp41D->f1;_tmp41F=(struct
Cyc_Absyn_Exp**)& _tmp41D->f1;}}_LL1E0:*_tmp41F=Cyc_Toc_push_address_exp(*_tmp41F);
goto _LL1DA;_LL1E1:;_LL1E2: {const char*_tmpBA7;void*_tmpBA6[1];struct Cyc_String_pa_struct
_tmpBA5;(_tmpBA5.tag=0,((_tmpBA5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmpBA6[0]=& _tmpBA5,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA7="can't take & of stmt %s",
_tag_dyneither(_tmpBA7,sizeof(char),24))),_tag_dyneither(_tmpBA6,sizeof(void*),1)))))));}
_LL1DA:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;
struct Cyc_List_List*prev;if(x == 0)return 0;{struct Cyc_List_List*_tmpBA8;result=((
_tmpBA8=_region_malloc(r2,sizeof(*_tmpBA8)),((_tmpBA8->hd=(void*)f((void*)x->hd,
env),((_tmpBA8->tl=0,_tmpBA8))))));}prev=result;for(x=x->tl;x != 0;x=x->tl){{
struct Cyc_List_List*_tmpBA9;((struct Cyc_List_List*)_check_null(prev))->tl=((
_tmpBA9=_region_malloc(r2,sizeof(*_tmpBA9)),((_tmpBA9->hd=(void*)f((void*)x->hd,
env),((_tmpBA9->tl=0,_tmpBA9))))));}prev=prev->tl;}return result;}static struct Cyc_List_List*
Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region,f,env,x);}static struct _tuple15*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*
e){struct _tuple15*_tmpBAA;return(_tmpBAA=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpBAA)),((_tmpBAA->f1=0,((_tmpBAA->f2=e,_tmpBAA)))));}static struct Cyc_Absyn_PtrInfo
Cyc_Toc_get_ptr_type(void*t){void*_tmp426=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp428;_LL1E4: {struct Cyc_Absyn_PointerType_struct*_tmp427=(struct Cyc_Absyn_PointerType_struct*)
_tmp426;if(_tmp427->tag != 5)goto _LL1E6;else{_tmp428=_tmp427->f1;}}_LL1E5: return
_tmp428;_LL1E6:;_LL1E7: {const char*_tmpBAD;void*_tmpBAC;(_tmpBAC=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBAD="get_ptr_typ: not a pointer!",
_tag_dyneither(_tmpBAD,sizeof(char),28))),_tag_dyneither(_tmpBAC,sizeof(void*),0)));}
_LL1E3:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp42B=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp42E;enum Cyc_Absyn_Size_of
_tmp42F;enum Cyc_Absyn_Sign _tmp431;enum Cyc_Absyn_Size_of _tmp432;enum Cyc_Absyn_Sign
_tmp436;enum Cyc_Absyn_Size_of _tmp437;enum Cyc_Absyn_Sign _tmp439;enum Cyc_Absyn_Size_of
_tmp43A;enum Cyc_Absyn_Sign _tmp43C;enum Cyc_Absyn_Size_of _tmp43D;_LL1E9: {struct
Cyc_Absyn_PointerType_struct*_tmp42C=(struct Cyc_Absyn_PointerType_struct*)
_tmp42B;if(_tmp42C->tag != 5)goto _LL1EB;}_LL1EA: res=Cyc_Absyn_null_exp(0);goto
_LL1E8;_LL1EB: {struct Cyc_Absyn_IntType_struct*_tmp42D=(struct Cyc_Absyn_IntType_struct*)
_tmp42B;if(_tmp42D->tag != 6)goto _LL1ED;else{_tmp42E=_tmp42D->f1;_tmp42F=_tmp42D->f2;
if(_tmp42F != Cyc_Absyn_Char_sz)goto _LL1ED;}}_LL1EC: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(
_tmp42E,'\000'),0);goto _LL1E8;_LL1ED: {struct Cyc_Absyn_IntType_struct*_tmp430=(
struct Cyc_Absyn_IntType_struct*)_tmp42B;if(_tmp430->tag != 6)goto _LL1EF;else{
_tmp431=_tmp430->f1;_tmp432=_tmp430->f2;if(_tmp432 != Cyc_Absyn_Short_sz)goto
_LL1EF;}}_LL1EE: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp431,0),0);goto
_LL1E8;_LL1EF: {struct Cyc_Absyn_EnumType_struct*_tmp433=(struct Cyc_Absyn_EnumType_struct*)
_tmp42B;if(_tmp433->tag != 14)goto _LL1F1;}_LL1F0: goto _LL1F2;_LL1F1: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp434=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp42B;if(_tmp434->tag != 15)goto
_LL1F3;}_LL1F2: _tmp436=Cyc_Absyn_Unsigned;goto _LL1F4;_LL1F3: {struct Cyc_Absyn_IntType_struct*
_tmp435=(struct Cyc_Absyn_IntType_struct*)_tmp42B;if(_tmp435->tag != 6)goto _LL1F5;
else{_tmp436=_tmp435->f1;_tmp437=_tmp435->f2;if(_tmp437 != Cyc_Absyn_Int_sz)goto
_LL1F5;}}_LL1F4: _tmp439=_tmp436;goto _LL1F6;_LL1F5: {struct Cyc_Absyn_IntType_struct*
_tmp438=(struct Cyc_Absyn_IntType_struct*)_tmp42B;if(_tmp438->tag != 6)goto _LL1F7;
else{_tmp439=_tmp438->f1;_tmp43A=_tmp438->f2;if(_tmp43A != Cyc_Absyn_Long_sz)goto
_LL1F7;}}_LL1F6: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp439,0),0);goto _LL1E8;
_LL1F7: {struct Cyc_Absyn_IntType_struct*_tmp43B=(struct Cyc_Absyn_IntType_struct*)
_tmp42B;if(_tmp43B->tag != 6)goto _LL1F9;else{_tmp43C=_tmp43B->f1;_tmp43D=_tmp43B->f2;
if(_tmp43D != Cyc_Absyn_LongLong_sz)goto _LL1F9;}}_LL1F8: res=Cyc_Absyn_const_exp(
Cyc_Absyn_LongLong_c(_tmp43C,(long long)0),0);goto _LL1E8;_LL1F9: {struct Cyc_Absyn_FloatType_struct*
_tmp43E=(struct Cyc_Absyn_FloatType_struct*)_tmp42B;if(_tmp43E->tag != 7)goto
_LL1FB;}_LL1FA: goto _LL1FC;_LL1FB: {struct Cyc_Absyn_DoubleType_struct*_tmp43F=(
struct Cyc_Absyn_DoubleType_struct*)_tmp42B;if(_tmp43F->tag != 8)goto _LL1FD;}
_LL1FC:{const char*_tmpBAE;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBAE="0.0",
_tag_dyneither(_tmpBAE,sizeof(char),4)))),0);}goto _LL1E8;_LL1FD:;_LL1FE: {const
char*_tmpBB2;void*_tmpBB1[1];struct Cyc_String_pa_struct _tmpBB0;(_tmpBB0.tag=0,((
_tmpBB0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmpBB1[0]=& _tmpBB0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBB2="found non-zero type %s in generate_zero",_tag_dyneither(_tmpBB2,sizeof(
char),40))),_tag_dyneither(_tmpBB1,sizeof(void*),1)))))));}_LL1E8:;}{struct Cyc_Core_Opt*
_tmpBB3;res->topt=((_tmpBB3=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpBB3)),((_tmpBB3->v=t,_tmpBB3))));}return res;}static void Cyc_Toc_zero_ptr_assign_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){void*
fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,
Cyc_Absyn_true_conref);void*_tmp445=Cyc_Toc_typ_to_c(elt_type);void*_tmp446=Cyc_Toc_typ_to_c(
fat_ptr_type);void*_tmp447=Cyc_Absyn_cstar_typ(_tmp445,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmpBB4;struct Cyc_Core_Opt*_tmp448=(_tmpBB4=_cycalloc(sizeof(*_tmpBB4)),((
_tmpBB4->v=_tmp447,_tmpBB4)));struct Cyc_Absyn_Exp*xinit;{void*_tmp449=e1->r;
struct Cyc_Absyn_Exp*_tmp44B;struct Cyc_Absyn_Exp*_tmp44D;struct Cyc_Absyn_Exp*
_tmp44E;_LL200: {struct Cyc_Absyn_Deref_e_struct*_tmp44A=(struct Cyc_Absyn_Deref_e_struct*)
_tmp449;if(_tmp44A->tag != 21)goto _LL202;else{_tmp44B=_tmp44A->f1;}}_LL201: if(!
is_dyneither){_tmp44B=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp44B,0,Cyc_Absyn_Other_coercion,
0);{struct Cyc_Core_Opt*_tmpBB5;_tmp44B->topt=((_tmpBB5=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpBB5)),((_tmpBB5->v=fat_ptr_type,_tmpBB5))));};}Cyc_Toc_exp_to_c(nv,
_tmp44B);xinit=_tmp44B;goto _LL1FF;_LL202: {struct Cyc_Absyn_Subscript_e_struct*
_tmp44C=(struct Cyc_Absyn_Subscript_e_struct*)_tmp449;if(_tmp44C->tag != 24)goto
_LL204;else{_tmp44D=_tmp44C->f1;_tmp44E=_tmp44C->f2;}}_LL203: if(!is_dyneither){
_tmp44D=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp44D,0,Cyc_Absyn_Other_coercion,0);{
struct Cyc_Core_Opt*_tmpBB6;_tmp44D->topt=((_tmpBB6=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpBB6)),((_tmpBB6->v=fat_ptr_type,_tmpBB6))));};}Cyc_Toc_exp_to_c(nv,
_tmp44D);Cyc_Toc_exp_to_c(nv,_tmp44E);{struct Cyc_Absyn_Exp*_tmpBB7[3];xinit=Cyc_Absyn_fncall_exp(
Cyc_Toc__dyneither_ptr_plus_e,((_tmpBB7[2]=_tmp44E,((_tmpBB7[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0),((_tmpBB7[0]=_tmp44D,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB7,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);}goto _LL1FF;_LL204:;_LL205: {const char*_tmpBBA;void*_tmpBB9;(
_tmpBB9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBBA="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(
_tmpBBA,sizeof(char),53))),_tag_dyneither(_tmpBB9,sizeof(void*),0)));}_LL1FF:;}{
struct _tuple0*_tmp454=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp456;struct
_RegionHandle*_tmp457;struct Cyc_Toc_Env*_tmp455=nv;_tmp456=*_tmp455;_tmp457=
_tmp456.rgn;{struct Cyc_Toc_Env*_tmp458=Cyc_Toc_add_varmap(_tmp457,nv,_tmp454,Cyc_Absyn_var_exp(
_tmp454,0));struct Cyc_Absyn_Vardecl*_tmpBBB;struct Cyc_Absyn_Vardecl*_tmp459=(
_tmpBBB=_cycalloc(sizeof(*_tmpBBB)),((_tmpBBB->sc=Cyc_Absyn_Public,((_tmpBBB->name=
_tmp454,((_tmpBBB->tq=Cyc_Toc_mt_tq,((_tmpBBB->type=_tmp446,((_tmpBBB->initializer=(
struct Cyc_Absyn_Exp*)xinit,((_tmpBBB->rgn=0,((_tmpBBB->attributes=0,((_tmpBBB->escapes=
0,_tmpBBB)))))))))))))))));struct Cyc_Absyn_Local_b_struct _tmpBBE;struct Cyc_Absyn_Local_b_struct*
_tmpBBD;struct Cyc_Absyn_Local_b_struct*_tmp45A=(_tmpBBD=_cycalloc(sizeof(*
_tmpBBD)),((_tmpBBD[0]=((_tmpBBE.tag=4,((_tmpBBE.f1=_tmp459,_tmpBBE)))),_tmpBBD)));
struct Cyc_Absyn_Exp*_tmp45B=Cyc_Absyn_varb_exp(_tmp454,(void*)_tmp45A,0);{struct
Cyc_Core_Opt*_tmpBBF;_tmp45B->topt=((_tmpBBF=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpBBF)),((_tmpBBF->v=fat_ptr_type,_tmpBBF))));}{struct Cyc_Absyn_Exp*
_tmp45D=Cyc_Absyn_deref_exp(_tmp45B,0);{struct Cyc_Core_Opt*_tmpBC0;_tmp45D->topt=((
_tmpBC0=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpBC0)),((_tmpBC0->v=
elt_type,_tmpBC0))));}Cyc_Toc_exp_to_c(_tmp458,_tmp45D);{struct _tuple0*_tmp45F=
Cyc_Toc_temp_var();_tmp458=Cyc_Toc_add_varmap(_tmp457,_tmp458,_tmp45F,Cyc_Absyn_var_exp(
_tmp45F,0));{struct Cyc_Absyn_Vardecl*_tmpBC1;struct Cyc_Absyn_Vardecl*_tmp460=(
_tmpBC1=_cycalloc(sizeof(*_tmpBC1)),((_tmpBC1->sc=Cyc_Absyn_Public,((_tmpBC1->name=
_tmp45F,((_tmpBC1->tq=Cyc_Toc_mt_tq,((_tmpBC1->type=_tmp445,((_tmpBC1->initializer=(
struct Cyc_Absyn_Exp*)_tmp45D,((_tmpBC1->rgn=0,((_tmpBC1->attributes=0,((_tmpBC1->escapes=
0,_tmpBC1)))))))))))))))));struct Cyc_Absyn_Local_b_struct _tmpBC4;struct Cyc_Absyn_Local_b_struct*
_tmpBC3;struct Cyc_Absyn_Local_b_struct*_tmp461=(_tmpBC3=_cycalloc(sizeof(*
_tmpBC3)),((_tmpBC3[0]=((_tmpBC4.tag=4,((_tmpBC4.f1=_tmp460,_tmpBC4)))),_tmpBC3)));
struct Cyc_Absyn_Exp*z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp462=Cyc_Absyn_varb_exp(
_tmp45F,(void*)_tmp461,0);_tmp462->topt=_tmp45D->topt;z_init=Cyc_Absyn_prim2_exp((
enum Cyc_Absyn_Primop)popt->v,_tmp462,e2,0);z_init->topt=_tmp462->topt;}Cyc_Toc_exp_to_c(
_tmp458,z_init);{struct _tuple0*_tmp463=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmpBC5;struct Cyc_Absyn_Vardecl*_tmp464=(_tmpBC5=_cycalloc(sizeof(*_tmpBC5)),((
_tmpBC5->sc=Cyc_Absyn_Public,((_tmpBC5->name=_tmp463,((_tmpBC5->tq=Cyc_Toc_mt_tq,((
_tmpBC5->type=_tmp445,((_tmpBC5->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmpBC5->rgn=0,((_tmpBC5->attributes=0,((_tmpBC5->escapes=0,_tmpBC5)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpBC8;struct Cyc_Absyn_Local_b_struct*_tmpBC7;
struct Cyc_Absyn_Local_b_struct*_tmp465=(_tmpBC7=_cycalloc(sizeof(*_tmpBC7)),((
_tmpBC7[0]=((_tmpBC8.tag=4,((_tmpBC8.f1=_tmp464,_tmpBC8)))),_tmpBC7)));_tmp458=
Cyc_Toc_add_varmap(_tmp457,_tmp458,_tmp463,Cyc_Absyn_var_exp(_tmp463,0));{struct
Cyc_Absyn_Exp*_tmp466=Cyc_Absyn_varb_exp(_tmp45F,(void*)_tmp461,0);_tmp466->topt=
_tmp45D->topt;{struct Cyc_Absyn_Exp*_tmp467=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp468=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp466,_tmp467,0);{
struct Cyc_Core_Opt*_tmpBC9;_tmp468->topt=((_tmpBC9=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpBC9)),((_tmpBC9->v=Cyc_Absyn_sint_typ,_tmpBC9))));}Cyc_Toc_exp_to_c(
_tmp458,_tmp468);{struct Cyc_Absyn_Exp*_tmp46A=Cyc_Absyn_varb_exp(_tmp463,(void*)
_tmp465,0);_tmp46A->topt=_tmp45D->topt;{struct Cyc_Absyn_Exp*_tmp46B=Cyc_Toc_generate_zero(
elt_type);struct Cyc_Absyn_Exp*_tmp46C=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp46A,
_tmp46B,0);{struct Cyc_Core_Opt*_tmpBCA;_tmp46C->topt=((_tmpBCA=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmpBCA)),((_tmpBCA->v=Cyc_Absyn_sint_typ,_tmpBCA))));}
Cyc_Toc_exp_to_c(_tmp458,_tmp46C);{struct Cyc_Absyn_Exp*_tmpBCB[2];struct Cyc_List_List*
_tmp46E=(_tmpBCB[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((
_tmpBCB[0]=Cyc_Absyn_varb_exp(_tmp454,(void*)_tmp45A,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBCB,sizeof(struct Cyc_Absyn_Exp*),
2)))));struct Cyc_Absyn_Exp*_tmp46F=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*
xsize;xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,
_tmp46E,0),_tmp46F,0);{struct Cyc_Absyn_Exp*_tmp470=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp468,_tmp46C,0),0);struct Cyc_Absyn_Stmt*_tmp471=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp472=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp454,(void*)_tmp45A,0),Cyc_Toc_curr_sp,0);_tmp472=Cyc_Toc_cast_it(
_tmp447,_tmp472);{struct Cyc_Absyn_Exp*_tmp473=Cyc_Absyn_deref_exp(_tmp472,0);
struct Cyc_Absyn_Exp*_tmp474=Cyc_Absyn_assign_exp(_tmp473,Cyc_Absyn_var_exp(
_tmp463,0),0);struct Cyc_Absyn_Stmt*_tmp475=Cyc_Absyn_exp_stmt(_tmp474,0);_tmp475=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp470,_tmp471,Cyc_Absyn_skip_stmt(
0),0),_tmp475,0);{struct Cyc_Absyn_Var_d_struct*_tmpBD1;struct Cyc_Absyn_Var_d_struct
_tmpBD0;struct Cyc_Absyn_Decl*_tmpBCF;_tmp475=Cyc_Absyn_decl_stmt(((_tmpBCF=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpBCF)),((_tmpBCF->r=(void*)((
_tmpBD1=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpBD1)),((_tmpBD1[0]=((
_tmpBD0.tag=0,((_tmpBD0.f1=_tmp464,_tmpBD0)))),_tmpBD1)))),((_tmpBCF->loc=0,
_tmpBCF)))))),_tmp475,0);}{struct Cyc_Absyn_Var_d_struct*_tmpBD7;struct Cyc_Absyn_Var_d_struct
_tmpBD6;struct Cyc_Absyn_Decl*_tmpBD5;_tmp475=Cyc_Absyn_decl_stmt(((_tmpBD5=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpBD5)),((_tmpBD5->r=(void*)((
_tmpBD7=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpBD7)),((_tmpBD7[0]=((
_tmpBD6.tag=0,((_tmpBD6.f1=_tmp460,_tmpBD6)))),_tmpBD7)))),((_tmpBD5->loc=0,
_tmpBD5)))))),_tmp475,0);}{struct Cyc_Absyn_Var_d_struct*_tmpBDD;struct Cyc_Absyn_Var_d_struct
_tmpBDC;struct Cyc_Absyn_Decl*_tmpBDB;_tmp475=Cyc_Absyn_decl_stmt(((_tmpBDB=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpBDB)),((_tmpBDB->r=(void*)((
_tmpBDD=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpBDD)),((_tmpBDD[0]=((
_tmpBDC.tag=0,((_tmpBDC.f1=_tmp459,_tmpBDC)))),_tmpBDD)))),((_tmpBDB->loc=0,
_tmpBDB)))))),_tmp475,0);}e->r=Cyc_Toc_stmt_exp_r(_tmp475);};};};};};};};};};};};};};}
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*
aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct
Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*)){struct Cyc_Absyn_Aggrdecl*
ad;{void*_tmp48A=Cyc_Tcutil_compress(aggrtype);struct Cyc_Absyn_AggrInfo _tmp48C;
union Cyc_Absyn_AggrInfoU _tmp48D;_LL207: {struct Cyc_Absyn_AggrType_struct*_tmp48B=(
struct Cyc_Absyn_AggrType_struct*)_tmp48A;if(_tmp48B->tag != 12)goto _LL209;else{
_tmp48C=_tmp48B->f1;_tmp48D=_tmp48C.aggr_info;}}_LL208: ad=Cyc_Absyn_get_known_aggrdecl(
_tmp48D);goto _LL206;_LL209:;_LL20A: {struct Cyc_String_pa_struct _tmpBE5;void*
_tmpBE4[1];const char*_tmpBE3;void*_tmpBE2;(_tmpBE2=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct
_dyneither_ptr)((_tmpBE5.tag=0,((_tmpBE5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpBE4[0]=& _tmpBE5,Cyc_aprintf(((
_tmpBE3="expecting union but found %s in check_tagged_union",_tag_dyneither(
_tmpBE3,sizeof(char),51))),_tag_dyneither(_tmpBE4,sizeof(void*),1)))))))),
_tag_dyneither(_tmpBE2,sizeof(void*),0)));}_LL206:;}{struct _tuple0*_tmp492=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp493=Cyc_Absyn_var_exp(_tmp492,0);struct Cyc_Absyn_Exp*
_tmp494=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);if(in_lhs){
struct Cyc_Absyn_Exp*_tmp495=Cyc_Absyn_aggrarrow_exp(_tmp493,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp496=Cyc_Absyn_neq_exp(_tmp495,_tmp494,0);struct Cyc_Absyn_Exp*
_tmp497=Cyc_Absyn_aggrarrow_exp(_tmp493,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Stmt*
_tmp498=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp497,0),0);struct Cyc_Absyn_Stmt*
_tmp499=Cyc_Absyn_ifthenelse_stmt(_tmp496,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);void*_tmp49A=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp49B=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);struct Cyc_Absyn_Stmt*_tmp49C=
Cyc_Absyn_declare_stmt(_tmp492,_tmp49A,(struct Cyc_Absyn_Exp*)_tmp49B,Cyc_Absyn_seq_stmt(
_tmp499,_tmp498,0),0);return Cyc_Toc_stmt_exp_r(_tmp49C);}else{struct Cyc_Absyn_Exp*
_tmp49D=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp493,f,0),Cyc_Toc_tag_sp,0);struct
Cyc_Absyn_Exp*_tmp49E=Cyc_Absyn_neq_exp(_tmp49D,_tmp494,0);struct Cyc_Absyn_Exp*
_tmp49F=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp493,f,0),Cyc_Toc_val_sp,0);struct
Cyc_Absyn_Stmt*_tmp4A0=Cyc_Absyn_exp_stmt(_tmp49F,0);struct Cyc_Absyn_Stmt*
_tmp4A1=Cyc_Absyn_ifthenelse_stmt(_tmp49E,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp4A2=Cyc_Absyn_declare_stmt(_tmp492,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp4A1,_tmp4A0,0),0);return Cyc_Toc_stmt_exp_r(
_tmp4A2);}};}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read){void*_tmp4A3=e->r;struct Cyc_Absyn_Exp*
_tmp4A5;struct Cyc_Absyn_Exp*_tmp4A7;struct _dyneither_ptr*_tmp4A8;int _tmp4A9;int*
_tmp4AA;struct Cyc_Absyn_Exp*_tmp4AC;struct _dyneither_ptr*_tmp4AD;int _tmp4AE;int*
_tmp4AF;_LL20C: {struct Cyc_Absyn_Cast_e_struct*_tmp4A4=(struct Cyc_Absyn_Cast_e_struct*)
_tmp4A3;if(_tmp4A4->tag != 15)goto _LL20E;else{_tmp4A5=_tmp4A4->f2;}}_LL20D: {
const char*_tmpBE8;void*_tmpBE7;(_tmpBE7=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE8="cast on lhs!",
_tag_dyneither(_tmpBE8,sizeof(char),13))),_tag_dyneither(_tmpBE7,sizeof(void*),0)));}
_LL20E: {struct Cyc_Absyn_AggrMember_e_struct*_tmp4A6=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp4A3;if(_tmp4A6->tag != 22)goto _LL210;else{_tmp4A7=_tmp4A6->f1;_tmp4A8=_tmp4A6->f2;
_tmp4A9=_tmp4A6->f4;_tmp4AA=(int*)& _tmp4A6->f4;}}_LL20F: {void*_tmp4B2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4A7->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp4B4;union Cyc_Absyn_AggrInfoU _tmp4B5;_LL215: {struct Cyc_Absyn_AggrType_struct*
_tmp4B3=(struct Cyc_Absyn_AggrType_struct*)_tmp4B2;if(_tmp4B3->tag != 12)goto
_LL217;else{_tmp4B4=_tmp4B3->f1;_tmp4B5=_tmp4B4.aggr_info;}}_LL216: {struct Cyc_Absyn_Aggrdecl*
_tmp4B6=Cyc_Absyn_get_known_aggrdecl(_tmp4B5);*f_tag=Cyc_Toc_get_member_offset(
_tmp4B6,_tmp4A8);{const char*_tmpBED;void*_tmpBEC[2];struct Cyc_String_pa_struct
_tmpBEB;struct Cyc_String_pa_struct _tmpBEA;struct _dyneither_ptr str=(struct
_dyneither_ptr)((_tmpBEA.tag=0,((_tmpBEA.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp4A8),((_tmpBEB.tag=0,((_tmpBEB.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp4B6->name).f2),((_tmpBEC[0]=& _tmpBEB,((_tmpBEC[1]=&
_tmpBEA,Cyc_aprintf(((_tmpBED="_union_%s_%s",_tag_dyneither(_tmpBED,sizeof(char),
13))),_tag_dyneither(_tmpBEC,sizeof(void*),2))))))))))))));{struct _dyneither_ptr*
_tmpBEE;*tagged_member_type=Cyc_Absyn_strct(((_tmpBEE=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpBEE)),((_tmpBEE[0]=str,_tmpBEE)))));}if(clear_read)*_tmp4AA=0;return((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4B6->impl))->tagged;};}_LL217:;
_LL218: return 0;_LL214:;}_LL210: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp4AB=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp4A3;if(_tmp4AB->tag != 23)goto _LL212;
else{_tmp4AC=_tmp4AB->f1;_tmp4AD=_tmp4AB->f2;_tmp4AE=_tmp4AB->f4;_tmp4AF=(int*)&
_tmp4AB->f4;}}_LL211: {void*_tmp4BC=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4AC->topt))->v);struct Cyc_Absyn_PtrInfo _tmp4BE;void*_tmp4BF;
_LL21A: {struct Cyc_Absyn_PointerType_struct*_tmp4BD=(struct Cyc_Absyn_PointerType_struct*)
_tmp4BC;if(_tmp4BD->tag != 5)goto _LL21C;else{_tmp4BE=_tmp4BD->f1;_tmp4BF=_tmp4BE.elt_typ;}}
_LL21B: {void*_tmp4C0=Cyc_Tcutil_compress(_tmp4BF);struct Cyc_Absyn_AggrInfo
_tmp4C2;union Cyc_Absyn_AggrInfoU _tmp4C3;_LL21F: {struct Cyc_Absyn_AggrType_struct*
_tmp4C1=(struct Cyc_Absyn_AggrType_struct*)_tmp4C0;if(_tmp4C1->tag != 12)goto
_LL221;else{_tmp4C2=_tmp4C1->f1;_tmp4C3=_tmp4C2.aggr_info;}}_LL220: {struct Cyc_Absyn_Aggrdecl*
_tmp4C4=Cyc_Absyn_get_known_aggrdecl(_tmp4C3);*f_tag=Cyc_Toc_get_member_offset(
_tmp4C4,_tmp4AD);{const char*_tmpBF3;void*_tmpBF2[2];struct Cyc_String_pa_struct
_tmpBF1;struct Cyc_String_pa_struct _tmpBF0;struct _dyneither_ptr str=(struct
_dyneither_ptr)((_tmpBF0.tag=0,((_tmpBF0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp4AD),((_tmpBF1.tag=0,((_tmpBF1.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp4C4->name).f2),((_tmpBF2[0]=& _tmpBF1,((_tmpBF2[1]=&
_tmpBF0,Cyc_aprintf(((_tmpBF3="_union_%s_%s",_tag_dyneither(_tmpBF3,sizeof(char),
13))),_tag_dyneither(_tmpBF2,sizeof(void*),2))))))))))))));{struct _dyneither_ptr*
_tmpBF4;*tagged_member_type=Cyc_Absyn_strct(((_tmpBF4=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpBF4)),((_tmpBF4[0]=str,_tmpBF4)))));}if(clear_read)*_tmp4AF=0;return((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4C4->impl))->tagged;};}_LL221:;
_LL222: return 0;_LL21E:;}_LL21C:;_LL21D: return 0;_LL219:;}_LL212:;_LL213: return 0;
_LL20B:;}void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int
tag,void*mem_type){struct _tuple0*_tmp4CA=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
temp_exp=Cyc_Absyn_var_exp(_tmp4CA,0);struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(
tag,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,
0),0);struct Cyc_Absyn_Exp*_tmp4CB=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_ifthenelse_stmt(_tmp4CB,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4CA,Cyc_Absyn_cstar_typ(
mem_type,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(
e)),Cyc_Absyn_seq_stmt(s2,s3,0),0);e->r=Cyc_Toc_stmt_exp_r(s1);}static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type){struct _tuple0*_tmp4CC=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4CC,0);struct Cyc_Absyn_Exp*
temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*
temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 
0)s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*
_tmp4CD=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp4CD,
Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=
Cyc_Absyn_declare_stmt(_tmp4CC,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(e1),Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple19{struct _tuple0*f1;void*f2;struct
Cyc_Absyn_Exp*f3;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*
f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void _tmpC47(unsigned int*_tmpC46,unsigned int*_tmpC45,struct _tuple0***
_tmpC43){for(*_tmpC46=0;*_tmpC46 < *_tmpC45;(*_tmpC46)++){(*_tmpC43)[*_tmpC46]=
Cyc_Toc_temp_var();}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){void*_tmp4CE=e->r;if(e->topt == 0){const char*_tmpBF8;void*_tmpBF7[1];struct Cyc_String_pa_struct
_tmpBF6;(_tmpBF6.tag=0,((_tmpBF6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpBF7[0]=& _tmpBF6,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF8="exp_to_c: no type for %s",
_tag_dyneither(_tmpBF8,sizeof(char),25))),_tag_dyneither(_tmpBF7,sizeof(void*),1)))))));}{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp4D2=
_tmp4CE;union Cyc_Absyn_Cnst _tmp4D4;int _tmp4D5;struct _tuple0*_tmp4D8;void*_tmp4D9;
struct _tuple0*_tmp4DB;enum Cyc_Absyn_Primop _tmp4DD;struct Cyc_List_List*_tmp4DE;
struct Cyc_Absyn_Exp*_tmp4E0;enum Cyc_Absyn_Incrementor _tmp4E1;struct Cyc_Absyn_Exp*
_tmp4E3;struct Cyc_Core_Opt*_tmp4E4;struct Cyc_Absyn_Exp*_tmp4E5;struct Cyc_Absyn_Exp*
_tmp4E7;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_Absyn_Exp*_tmp4E9;struct Cyc_Absyn_Exp*
_tmp4EB;struct Cyc_Absyn_Exp*_tmp4EC;struct Cyc_Absyn_Exp*_tmp4EE;struct Cyc_Absyn_Exp*
_tmp4EF;struct Cyc_Absyn_Exp*_tmp4F1;struct Cyc_Absyn_Exp*_tmp4F2;struct Cyc_Absyn_Exp*
_tmp4F4;struct Cyc_List_List*_tmp4F5;struct Cyc_Absyn_Exp*_tmp4F7;struct Cyc_List_List*
_tmp4F8;struct Cyc_Absyn_VarargCallInfo*_tmp4F9;struct Cyc_Absyn_Exp*_tmp4FB;
struct Cyc_List_List*_tmp4FC;struct Cyc_Absyn_VarargCallInfo*_tmp4FD;struct Cyc_Absyn_VarargCallInfo
_tmp4FE;int _tmp4FF;struct Cyc_List_List*_tmp500;struct Cyc_Absyn_VarargInfo*
_tmp501;struct Cyc_Absyn_Exp*_tmp503;struct Cyc_Absyn_Exp*_tmp505;struct Cyc_Absyn_Exp*
_tmp507;struct Cyc_List_List*_tmp508;void*_tmp50A;void**_tmp50B;struct Cyc_Absyn_Exp*
_tmp50C;int _tmp50D;enum Cyc_Absyn_Coercion _tmp50E;struct Cyc_Absyn_Exp*_tmp510;
struct Cyc_Absyn_Exp*_tmp512;struct Cyc_Absyn_Exp*_tmp513;struct Cyc_Absyn_Exp*
_tmp515;void*_tmp517;void*_tmp519;void*_tmp51A;struct _dyneither_ptr*_tmp51C;void*
_tmp51E;void*_tmp51F;unsigned int _tmp521;struct Cyc_Absyn_Exp*_tmp523;struct Cyc_Absyn_Exp*
_tmp525;struct _dyneither_ptr*_tmp526;int _tmp527;int _tmp528;struct Cyc_Absyn_Exp*
_tmp52A;struct _dyneither_ptr*_tmp52B;int _tmp52C;int _tmp52D;struct Cyc_Absyn_Exp*
_tmp52F;struct Cyc_Absyn_Exp*_tmp530;struct Cyc_List_List*_tmp532;struct Cyc_List_List*
_tmp534;struct Cyc_Absyn_Vardecl*_tmp536;struct Cyc_Absyn_Exp*_tmp537;struct Cyc_Absyn_Exp*
_tmp538;int _tmp539;struct _tuple0*_tmp53B;struct Cyc_List_List*_tmp53C;struct Cyc_List_List*
_tmp53D;struct Cyc_Absyn_Aggrdecl*_tmp53E;void*_tmp540;struct Cyc_List_List*
_tmp541;struct Cyc_List_List*_tmp543;struct Cyc_Absyn_Datatypedecl*_tmp544;struct
Cyc_Absyn_Datatypefield*_tmp545;struct Cyc_Absyn_MallocInfo _tmp549;int _tmp54A;
struct Cyc_Absyn_Exp*_tmp54B;void**_tmp54C;struct Cyc_Absyn_Exp*_tmp54D;int _tmp54E;
struct Cyc_Absyn_Exp*_tmp550;struct Cyc_Absyn_Exp*_tmp551;struct Cyc_Absyn_Exp*
_tmp553;struct _dyneither_ptr*_tmp554;struct Cyc_Absyn_Stmt*_tmp556;_LL224: {
struct Cyc_Absyn_Const_e_struct*_tmp4D3=(struct Cyc_Absyn_Const_e_struct*)_tmp4D2;
if(_tmp4D3->tag != 0)goto _LL226;else{_tmp4D4=_tmp4D3->f1;if((_tmp4D4.Null_c).tag
!= 1)goto _LL226;_tmp4D5=(int)(_tmp4D4.Null_c).val;}}_LL225: {struct Cyc_Absyn_Exp*
_tmp55B=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp55B,
_tmp55B))->r;else{struct Cyc_Absyn_Exp*_tmpBF9[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((
_tmpBF9[2]=_tmp55B,((_tmpBF9[1]=_tmp55B,((_tmpBF9[0]=_tmp55B,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF9,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL223;}_LL226: {struct Cyc_Absyn_Const_e_struct*
_tmp4D6=(struct Cyc_Absyn_Const_e_struct*)_tmp4D2;if(_tmp4D6->tag != 0)goto _LL228;}
_LL227: goto _LL223;_LL228: {struct Cyc_Absyn_Var_e_struct*_tmp4D7=(struct Cyc_Absyn_Var_e_struct*)
_tmp4D2;if(_tmp4D7->tag != 1)goto _LL22A;else{_tmp4D8=_tmp4D7->f1;_tmp4D9=(void*)
_tmp4D7->f2;}}_LL229:{struct _handler_cons _tmp55D;_push_handler(& _tmp55D);{int
_tmp55F=0;if(setjmp(_tmp55D.handler))_tmp55F=1;if(!_tmp55F){e->r=(Cyc_Toc_lookup_varmap(
nv,_tmp4D8))->r;;_pop_handler();}else{void*_tmp55E=(void*)_exn_thrown;void*
_tmp561=_tmp55E;_LL27D: {struct Cyc_Dict_Absent_struct*_tmp562=(struct Cyc_Dict_Absent_struct*)
_tmp561;if(_tmp562->tag != Cyc_Dict_Absent)goto _LL27F;}_LL27E: {const char*_tmpBFD;
void*_tmpBFC[1];struct Cyc_String_pa_struct _tmpBFB;(_tmpBFB.tag=0,((_tmpBFB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4D8)),((
_tmpBFC[0]=& _tmpBFB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBFD="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpBFD,sizeof(char),
32))),_tag_dyneither(_tmpBFC,sizeof(void*),1)))))));}_LL27F:;_LL280:(void)_throw(
_tmp561);_LL27C:;}};}goto _LL223;_LL22A: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp4DA=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp4D2;if(_tmp4DA->tag != 2)goto
_LL22C;else{_tmp4DB=_tmp4DA->f1;}}_LL22B: {const char*_tmpC00;void*_tmpBFF;(
_tmpBFF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC00="unknownid",_tag_dyneither(_tmpC00,sizeof(char),10))),_tag_dyneither(
_tmpBFF,sizeof(void*),0)));}_LL22C: {struct Cyc_Absyn_Primop_e_struct*_tmp4DC=(
struct Cyc_Absyn_Primop_e_struct*)_tmp4D2;if(_tmp4DC->tag != 3)goto _LL22E;else{
_tmp4DD=_tmp4DC->f1;_tmp4DE=_tmp4DC->f2;}}_LL22D: {struct Cyc_List_List*_tmp568=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4DE);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp4DE);switch(_tmp4DD){case Cyc_Absyn_Numelts: _LL281: {
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp4DE))->hd;{void*_tmp569=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo _tmp56B;struct Cyc_Absyn_Exp*
_tmp56C;struct Cyc_Absyn_PtrInfo _tmp56E;void*_tmp56F;struct Cyc_Absyn_PtrAtts
_tmp570;union Cyc_Absyn_Constraint*_tmp571;union Cyc_Absyn_Constraint*_tmp572;
union Cyc_Absyn_Constraint*_tmp573;_LL284: {struct Cyc_Absyn_ArrayType_struct*
_tmp56A=(struct Cyc_Absyn_ArrayType_struct*)_tmp569;if(_tmp56A->tag != 9)goto
_LL286;else{_tmp56B=_tmp56A->f1;_tmp56C=_tmp56B.num_elts;}}_LL285: if(!Cyc_Evexp_c_can_eval((
struct Cyc_Absyn_Exp*)_check_null(_tmp56C))){const char*_tmpC03;void*_tmpC02;(
_tmpC02=0,Cyc_Tcutil_terr(e->loc,((_tmpC03="can't calculate numelts",
_tag_dyneither(_tmpC03,sizeof(char),24))),_tag_dyneither(_tmpC02,sizeof(void*),0)));}
e->r=_tmp56C->r;goto _LL283;_LL286: {struct Cyc_Absyn_PointerType_struct*_tmp56D=(
struct Cyc_Absyn_PointerType_struct*)_tmp569;if(_tmp56D->tag != 5)goto _LL288;else{
_tmp56E=_tmp56D->f1;_tmp56F=_tmp56E.elt_typ;_tmp570=_tmp56E.ptr_atts;_tmp571=
_tmp570.nullable;_tmp572=_tmp570.bounds;_tmp573=_tmp570.zero_term;}}_LL287:{void*
_tmp576=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp572);struct Cyc_Absyn_Exp*_tmp579;_LL28B: {struct Cyc_Absyn_DynEither_b_struct*
_tmp577=(struct Cyc_Absyn_DynEither_b_struct*)_tmp576;if(_tmp577->tag != 0)goto
_LL28D;}_LL28C:{struct Cyc_Absyn_Exp*_tmpC04[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,((
_tmpC04[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp56F),0),((_tmpC04[0]=(
struct Cyc_Absyn_Exp*)_tmp4DE->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC04,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL28A;_LL28D: {struct Cyc_Absyn_Upper_b_struct*_tmp578=(struct Cyc_Absyn_Upper_b_struct*)
_tmp576;if(_tmp578->tag != 1)goto _LL28A;else{_tmp579=_tmp578->f1;}}_LL28E: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp573)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp4DE->hd);struct Cyc_Absyn_Exp*_tmpC05[2];e->r=Cyc_Toc_fncall_exp_r(
function_e,((_tmpC05[1]=_tmp579,((_tmpC05[0]=(struct Cyc_Absyn_Exp*)_tmp4DE->hd,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC05,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp571)){if(!Cyc_Evexp_c_can_eval(_tmp579)){const
char*_tmpC08;void*_tmpC07;(_tmpC07=0,Cyc_Tcutil_terr(e->loc,((_tmpC08="can't calculate numelts",
_tag_dyneither(_tmpC08,sizeof(char),24))),_tag_dyneither(_tmpC07,sizeof(void*),0)));}
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp579,Cyc_Absyn_uint_exp(0,0));}else{e->r=
_tmp579->r;goto _LL28A;}}goto _LL28A;_LL28A:;}goto _LL283;_LL288:;_LL289: {const
char*_tmpC0D;void*_tmpC0C[2];struct Cyc_String_pa_struct _tmpC0B;struct Cyc_String_pa_struct
_tmpC0A;(_tmpC0A.tag=0,((_tmpC0A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((
_tmpC0B.tag=0,((_tmpC0B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmpC0C[0]=& _tmpC0B,((
_tmpC0C[1]=& _tmpC0A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC0D="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpC0D,
sizeof(char),41))),_tag_dyneither(_tmpC0C,sizeof(void*),2)))))))))))));}_LL283:;}
break;}case Cyc_Absyn_Plus: _LL282:{void*_tmp582=Cyc_Tcutil_compress((void*)((
struct Cyc_List_List*)_check_null(_tmp568))->hd);struct Cyc_Absyn_PtrInfo _tmp584;
void*_tmp585;struct Cyc_Absyn_PtrAtts _tmp586;union Cyc_Absyn_Constraint*_tmp587;
union Cyc_Absyn_Constraint*_tmp588;_LL291: {struct Cyc_Absyn_PointerType_struct*
_tmp583=(struct Cyc_Absyn_PointerType_struct*)_tmp582;if(_tmp583->tag != 5)goto
_LL293;else{_tmp584=_tmp583->f1;_tmp585=_tmp584.elt_typ;_tmp586=_tmp584.ptr_atts;
_tmp587=_tmp586.bounds;_tmp588=_tmp586.zero_term;}}_LL292:{void*_tmp589=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp587);struct Cyc_Absyn_Exp*_tmp58C;_LL296: {struct Cyc_Absyn_DynEither_b_struct*
_tmp58A=(struct Cyc_Absyn_DynEither_b_struct*)_tmp589;if(_tmp58A->tag != 0)goto
_LL298;}_LL297: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp4DE))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp4DE->tl))->hd;{struct Cyc_Absyn_Exp*_tmpC0E[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpC0E[2]=e2,((_tmpC0E[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp585),0),((_tmpC0E[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC0E,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}goto _LL295;}_LL298: {struct Cyc_Absyn_Upper_b_struct*_tmp58B=(struct
Cyc_Absyn_Upper_b_struct*)_tmp589;if(_tmp58B->tag != 1)goto _LL295;else{_tmp58C=
_tmp58B->f1;}}_LL299: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp588)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp4DE))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp4DE->tl))->hd;struct Cyc_Absyn_Exp*_tmpC0F[3];e->r=(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
e1),((_tmpC0F[2]=e2,((_tmpC0F[1]=_tmp58C,((_tmpC0F[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC0F,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r;}goto _LL295;_LL295:;}goto _LL290;_LL293:;_LL294: goto _LL290;
_LL290:;}break;case Cyc_Absyn_Minus: _LL28F: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp568))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4DE))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4DE->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp568->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{struct Cyc_Absyn_Exp*_tmpC10[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpC10[2]=Cyc_Absyn_prim1_exp(
Cyc_Absyn_Minus,e2,0),((_tmpC10[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpC10[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC10,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}break;}case Cyc_Absyn_Eq:
_LL29A: goto _LL29B;case Cyc_Absyn_Neq: _LL29B: goto _LL29C;case Cyc_Absyn_Gt: _LL29C:
goto _LL29D;case Cyc_Absyn_Gte: _LL29D: goto _LL29E;case Cyc_Absyn_Lt: _LL29E: goto
_LL29F;case Cyc_Absyn_Lte: _LL29F: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4DE))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4DE->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp568))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp568->tl))->hd;void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& elt_typ))e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,
0));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp,0));break;}default: _LL2A0: break;}goto _LL223;}_LL22E: {
struct Cyc_Absyn_Increment_e_struct*_tmp4DF=(struct Cyc_Absyn_Increment_e_struct*)
_tmp4D2;if(_tmp4DF->tag != 5)goto _LL230;else{_tmp4E0=_tmp4DF->f1;_tmp4E1=_tmp4DF->f2;}}
_LL22F: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4E0->topt))->v;
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;const char*_tmpC11;struct _dyneither_ptr incr_str=(_tmpC11="increment",
_tag_dyneither(_tmpC11,sizeof(char),10));if(_tmp4E1 == Cyc_Absyn_PreDec  || 
_tmp4E1 == Cyc_Absyn_PostDec){const char*_tmpC12;incr_str=((_tmpC12="decrement",
_tag_dyneither(_tmpC12,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp4E0,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpC16;void*_tmpC15[1];
struct Cyc_String_pa_struct _tmpC14;(_tmpC14.tag=0,((_tmpC14.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC15[0]=& _tmpC14,Cyc_Tcutil_terr(
e->loc,((_tmpC16="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpC16,sizeof(char),74))),_tag_dyneither(_tmpC15,sizeof(void*),1)))))));}{
const char*_tmpC19;void*_tmpC18;(_tmpC18=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC19="in-place inc/dec on zero-term",
_tag_dyneither(_tmpC19,sizeof(char),30))),_tag_dyneither(_tmpC18,sizeof(void*),0)));};}{
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp4E0,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp596=Cyc_Absyn_signed_int_exp(
1,0);{struct Cyc_Core_Opt*_tmpC1A;_tmp596->topt=((_tmpC1A=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpC1A)),((_tmpC1A->v=Cyc_Absyn_sint_typ,_tmpC1A))));}switch(_tmp4E1){
case Cyc_Absyn_PreInc: _LL2A2:{struct Cyc_Absyn_AssignOp_e_struct _tmpC20;struct Cyc_Core_Opt*
_tmpC1F;struct Cyc_Absyn_AssignOp_e_struct*_tmpC1E;e->r=(void*)((_tmpC1E=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC1E)),((_tmpC1E[0]=((_tmpC20.tag=
4,((_tmpC20.f1=_tmp4E0,((_tmpC20.f2=((_tmpC1F=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpC1F)),((_tmpC1F->v=(void*)Cyc_Absyn_Plus,_tmpC1F)))),((_tmpC20.f3=
_tmp596,_tmpC20)))))))),_tmpC1E))));}Cyc_Toc_exp_to_c(nv,e);return;case Cyc_Absyn_PreDec:
_LL2A3:{struct Cyc_Absyn_AssignOp_e_struct _tmpC26;struct Cyc_Core_Opt*_tmpC25;
struct Cyc_Absyn_AssignOp_e_struct*_tmpC24;e->r=(void*)((_tmpC24=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmpC24)),((_tmpC24[0]=((_tmpC26.tag=4,((_tmpC26.f1=
_tmp4E0,((_tmpC26.f2=((_tmpC25=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpC25)),((_tmpC25->v=(void*)Cyc_Absyn_Minus,_tmpC25)))),((_tmpC26.f3=_tmp596,
_tmpC26)))))))),_tmpC24))));}Cyc_Toc_exp_to_c(nv,e);return;default: _LL2A4:{const
char*_tmpC2A;void*_tmpC29[1];struct Cyc_String_pa_struct _tmpC28;(_tmpC28.tag=0,((
_tmpC28.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC29[0]=&
_tmpC28,Cyc_Tcutil_terr(e->loc,((_tmpC2A="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpC2A,sizeof(char),59))),_tag_dyneither(_tmpC29,sizeof(void*),1)))))));}{
const char*_tmpC2D;void*_tmpC2C;(_tmpC2C=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC2D="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpC2D,sizeof(char),34))),_tag_dyneither(_tmpC2C,sizeof(void*),0)));};}}
Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp4E0);Cyc_Toc_set_lhs(nv,0);{void*
elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp4E1 == Cyc_Absyn_PostInc
 || _tmp4E1 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp4E1 == Cyc_Absyn_PreDec  || _tmp4E1 == Cyc_Absyn_PostDec)change=- 1;{struct
Cyc_Absyn_Exp*_tmpC2E[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC2E[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpC2E[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpC2E[0]=Cyc_Toc_push_address_exp(_tmp4E0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2E,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Toc_functionSet*_tmp5A4=_tmp4E1 == Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:&
Cyc_Toc__zero_arr_inplace_plus_post_functionSet;struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(
_tmp5A4,_tmp4E0);struct Cyc_Absyn_Exp*_tmpC2F[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((
_tmpC2F[1]=Cyc_Absyn_signed_int_exp(1,0),((_tmpC2F[0]=Cyc_Toc_push_address_exp(
_tmp4E0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC2F,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(elt_typ == (
void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4E0)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(
_tmp4E0,0,Cyc_Toc_incr_lvalue,_tmp4E1);e->r=_tmp4E0->r;}}}goto _LL223;};};}_LL230: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp4E2=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp4D2;if(_tmp4E2->tag != 4)goto _LL232;else{_tmp4E3=_tmp4E2->f1;_tmp4E4=_tmp4E2->f2;
_tmp4E5=_tmp4E2->f3;}}_LL231: {void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4E3->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4E5->topt))->v;int f_tag=0;void*tagged_member_struct_type=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Toc_is_tagged_union_project(_tmp4E3,& f_tag,&
tagged_member_struct_type,1)){Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp4E3);
Cyc_Toc_set_lhs(nv,0);Cyc_Toc_exp_to_c(nv,_tmp4E5);e->r=Cyc_Toc_tagged_union_assignop(
_tmp4E3,e1_old_typ,_tmp4E4,_tmp4E5,e2_old_typ,f_tag,tagged_member_struct_type);
return;}{void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4E3,& ptr_type,&
is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4E3,_tmp4E4,
_tmp4E5,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp4E3);Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp4E3);Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4E5);{int done=0;if(_tmp4E4 != 0){void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;switch((enum Cyc_Absyn_Primop)_tmp4E4->v){case Cyc_Absyn_Plus: _LL2A6:
change=_tmp4E5;break;case Cyc_Absyn_Minus: _LL2A7: change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,
_tmp4E5,0);break;default: _LL2A8: {const char*_tmpC32;void*_tmpC31;(_tmpC31=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC32="bad t ? pointer arithmetic",_tag_dyneither(_tmpC32,sizeof(char),27))),
_tag_dyneither(_tmpC31,sizeof(void*),0)));}}done=1;{struct Cyc_Absyn_Exp*_tmp5A9=
Cyc_Toc__dyneither_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*_tmpC33[3];e->r=Cyc_Toc_fncall_exp_r(
_tmp5A9,((_tmpC33[2]=change,((_tmpC33[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpC33[0]=Cyc_Toc_push_address_exp(_tmp4E3),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC33,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))
switch((enum Cyc_Absyn_Primop)_tmp4E4->v){case Cyc_Absyn_Plus: _LL2AA: done=1;{
struct Cyc_Absyn_Exp*_tmpC34[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4E3),((_tmpC34[1]=_tmp4E5,((
_tmpC34[0]=_tmp4E3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC34,sizeof(struct Cyc_Absyn_Exp*),2)))))));}break;default:
_LL2AB: {const char*_tmpC37;void*_tmpC36;(_tmpC36=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC37="bad zero-terminated pointer arithmetic",
_tag_dyneither(_tmpC37,sizeof(char),39))),_tag_dyneither(_tmpC36,sizeof(void*),0)));}}}}
if(!done){if(e1_poly)_tmp4E5->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp4E5->r,0));if(!Cyc_Absyn_is_lvalue(_tmp4E3)){{struct _tuple18 _tmpC3A;struct
_tuple18*_tmpC39;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple18*),struct _tuple18*f_env))
Cyc_Toc_lvalue_assign)(_tmp4E3,0,Cyc_Toc_assignop_lvalue,((_tmpC39=
_region_malloc(Cyc_Core_unique_region,sizeof(struct _tuple18)* 1),((_tmpC39[0]=((
_tmpC3A.f1=_tmp4E4,((_tmpC3A.f2=_tmp4E5,_tmpC3A)))),_tmpC39)))));}e->r=_tmp4E3->r;}}
goto _LL223;};};};}_LL232: {struct Cyc_Absyn_Conditional_e_struct*_tmp4E6=(struct
Cyc_Absyn_Conditional_e_struct*)_tmp4D2;if(_tmp4E6->tag != 6)goto _LL234;else{
_tmp4E7=_tmp4E6->f1;_tmp4E8=_tmp4E6->f2;_tmp4E9=_tmp4E6->f3;}}_LL233: Cyc_Toc_exp_to_c(
nv,_tmp4E7);Cyc_Toc_exp_to_c(nv,_tmp4E8);Cyc_Toc_exp_to_c(nv,_tmp4E9);goto _LL223;
_LL234: {struct Cyc_Absyn_And_e_struct*_tmp4EA=(struct Cyc_Absyn_And_e_struct*)
_tmp4D2;if(_tmp4EA->tag != 7)goto _LL236;else{_tmp4EB=_tmp4EA->f1;_tmp4EC=_tmp4EA->f2;}}
_LL235: Cyc_Toc_exp_to_c(nv,_tmp4EB);Cyc_Toc_exp_to_c(nv,_tmp4EC);goto _LL223;
_LL236: {struct Cyc_Absyn_Or_e_struct*_tmp4ED=(struct Cyc_Absyn_Or_e_struct*)
_tmp4D2;if(_tmp4ED->tag != 8)goto _LL238;else{_tmp4EE=_tmp4ED->f1;_tmp4EF=_tmp4ED->f2;}}
_LL237: Cyc_Toc_exp_to_c(nv,_tmp4EE);Cyc_Toc_exp_to_c(nv,_tmp4EF);goto _LL223;
_LL238: {struct Cyc_Absyn_SeqExp_e_struct*_tmp4F0=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp4D2;if(_tmp4F0->tag != 9)goto _LL23A;else{_tmp4F1=_tmp4F0->f1;_tmp4F2=_tmp4F0->f2;}}
_LL239: Cyc_Toc_exp_to_c(nv,_tmp4F1);Cyc_Toc_exp_to_c(nv,_tmp4F2);goto _LL223;
_LL23A: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp4F3=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp4D2;if(_tmp4F3->tag != 10)goto _LL23C;else{_tmp4F4=_tmp4F3->f1;_tmp4F5=_tmp4F3->f2;}}
_LL23B: _tmp4F7=_tmp4F4;_tmp4F8=_tmp4F5;goto _LL23D;_LL23C: {struct Cyc_Absyn_FnCall_e_struct*
_tmp4F6=(struct Cyc_Absyn_FnCall_e_struct*)_tmp4D2;if(_tmp4F6->tag != 11)goto
_LL23E;else{_tmp4F7=_tmp4F6->f1;_tmp4F8=_tmp4F6->f2;_tmp4F9=_tmp4F6->f3;if(
_tmp4F9 != 0)goto _LL23E;}}_LL23D: Cyc_Toc_exp_to_c(nv,_tmp4F7);((void(*)(void(*f)(
struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4F8);goto _LL223;_LL23E: {struct Cyc_Absyn_FnCall_e_struct*
_tmp4FA=(struct Cyc_Absyn_FnCall_e_struct*)_tmp4D2;if(_tmp4FA->tag != 11)goto
_LL240;else{_tmp4FB=_tmp4FA->f1;_tmp4FC=_tmp4FA->f2;_tmp4FD=_tmp4FA->f3;if(
_tmp4FD == 0)goto _LL240;_tmp4FE=*_tmp4FD;_tmp4FF=_tmp4FE.num_varargs;_tmp500=
_tmp4FE.injectors;_tmp501=_tmp4FE.vai;}}_LL23F: {struct Cyc_Toc_Env _tmp5B1;struct
_RegionHandle*_tmp5B2;struct Cyc_Toc_Env*_tmp5B0=nv;_tmp5B1=*_tmp5B0;_tmp5B2=
_tmp5B1.rgn;{struct _tuple0*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=
Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp4FF,0);void*cva_type=Cyc_Toc_typ_to_c(_tmp501->type);void*
arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp4FC);int num_normargs=num_args - _tmp4FF;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp4FC=_tmp4FC->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4FC))->hd);{
struct Cyc_List_List*_tmpC3B;new_args=((_tmpC3B=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpC3B)),((_tmpC3B->hd=(struct Cyc_Absyn_Exp*)_tmp4FC->hd,((_tmpC3B->tl=
new_args,_tmpC3B))))));};}}{struct Cyc_Absyn_Exp*_tmpC3E[3];struct Cyc_List_List*
_tmpC3D;new_args=((_tmpC3D=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC3D)),((
_tmpC3D->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpC3E[2]=
num_varargs_exp,((_tmpC3E[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpC3E[0]=
argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC3E,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpC3D->tl=
new_args,_tmpC3D))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp4FB);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4FB,new_args,0),0);if(_tmp501->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp5B6=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp501->type));struct Cyc_Absyn_DatatypeInfo _tmp5B8;union Cyc_Absyn_DatatypeInfoU
_tmp5B9;struct Cyc_Absyn_Datatypedecl**_tmp5BA;struct Cyc_Absyn_Datatypedecl*
_tmp5BB;_LL2AE: {struct Cyc_Absyn_DatatypeType_struct*_tmp5B7=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp5B6;if(_tmp5B7->tag != 3)goto _LL2B0;else{_tmp5B8=_tmp5B7->f1;_tmp5B9=_tmp5B8.datatype_info;
if((_tmp5B9.KnownDatatype).tag != 2)goto _LL2B0;_tmp5BA=(struct Cyc_Absyn_Datatypedecl**)(
_tmp5B9.KnownDatatype).val;_tmp5BB=*_tmp5BA;}}_LL2AF: tud=_tmp5BB;goto _LL2AD;
_LL2B0:;_LL2B1: {const char*_tmpC41;void*_tmpC40;(_tmpC40=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC41="toc: unknown datatype in vararg with inject",
_tag_dyneither(_tmpC41,sizeof(char),44))),_tag_dyneither(_tmpC40,sizeof(void*),0)));}
_LL2AD:;}{unsigned int _tmpC46;unsigned int _tmpC45;struct _dyneither_ptr _tmpC44;
struct _tuple0**_tmpC43;unsigned int _tmpC42;struct _dyneither_ptr vs=(_tmpC42=(
unsigned int)_tmp4FF,((_tmpC43=(struct _tuple0**)_region_malloc(_tmp5B2,
_check_times(sizeof(struct _tuple0*),_tmpC42)),((_tmpC44=_tag_dyneither(_tmpC43,
sizeof(struct _tuple0*),_tmpC42),((((_tmpC45=_tmpC42,_tmpC47(& _tmpC46,& _tmpC45,&
_tmpC43))),_tmpC44)))))));if(_tmp4FF != 0){struct Cyc_List_List*_tmp5BE=0;{int i=
_tmp4FF - 1;for(0;i >= 0;-- i){struct Cyc_List_List*_tmpC48;_tmp5BE=((_tmpC48=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC48)),((_tmpC48->hd=Cyc_Toc_make_dle(
Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple0**)
_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i)),0),0)),((_tmpC48->tl=
_tmp5BE,_tmpC48))))));}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_unresolvedmem_exp(0,_tmp5BE,0),s,0);{int i=0;for(0;_tmp4FC != 0;(((
_tmp4FC=_tmp4FC->tl,_tmp500=_tmp500->tl)),++ i)){struct Cyc_Absyn_Exp*arg=(struct
Cyc_Absyn_Exp*)_tmp4FC->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple0*var=*((struct _tuple0**)
_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield _tmp5C1;struct
_tuple0*_tmp5C2;struct Cyc_List_List*_tmp5C3;struct Cyc_Absyn_Datatypefield*
_tmp5C0=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp500))->hd;_tmp5C1=*_tmp5C0;_tmp5C2=_tmp5C1.name;_tmp5C3=_tmp5C1.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)_check_null(
_tmp5C3))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp5C2),0),s,0);{const char*_tmpC49;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp5C2,((_tmpC49="_struct",_tag_dyneither(_tmpC49,
sizeof(char),8))))),0,s,0);};};}};}else{struct _tuple15*_tmpC4A[3];struct Cyc_List_List*
_tmp5C5=(_tmpC4A[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC4A[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpC4A[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC4A,sizeof(struct _tuple15*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{{int i=0;for(0;_tmp4FC
!= 0;(_tmp4FC=_tmp4FC->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp4FC->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4FC->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL223;}_LL240: {struct Cyc_Absyn_Throw_e_struct*_tmp502=(struct Cyc_Absyn_Throw_e_struct*)
_tmp4D2;if(_tmp502->tag != 12)goto _LL242;else{_tmp503=_tmp502->f1;}}_LL241: Cyc_Toc_exp_to_c(
nv,_tmp503);e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(
_tmp503),0))->r;goto _LL223;_LL242: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp504=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp4D2;if(_tmp504->tag != 13)
goto _LL244;else{_tmp505=_tmp504->f1;}}_LL243: Cyc_Toc_exp_to_c(nv,_tmp505);goto
_LL223;_LL244: {struct Cyc_Absyn_Instantiate_e_struct*_tmp506=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp4D2;if(_tmp506->tag != 14)goto _LL246;else{_tmp507=_tmp506->f1;_tmp508=_tmp506->f2;}}
_LL245: Cyc_Toc_exp_to_c(nv,_tmp507);for(0;_tmp508 != 0;_tmp508=_tmp508->tl){enum 
Cyc_Absyn_KindQual _tmp5CB=(Cyc_Tcutil_typ_kind((void*)_tmp508->hd))->kind;if(
_tmp5CB != Cyc_Absyn_EffKind  && _tmp5CB != Cyc_Absyn_RgnKind){{void*_tmp5CC=Cyc_Tcutil_compress((
void*)_tmp508->hd);_LL2B3: {struct Cyc_Absyn_VarType_struct*_tmp5CD=(struct Cyc_Absyn_VarType_struct*)
_tmp5CC;if(_tmp5CD->tag != 2)goto _LL2B5;}_LL2B4: goto _LL2B6;_LL2B5: {struct Cyc_Absyn_DatatypeType_struct*
_tmp5CE=(struct Cyc_Absyn_DatatypeType_struct*)_tmp5CC;if(_tmp5CE->tag != 3)goto
_LL2B7;}_LL2B6: continue;_LL2B7:;_LL2B8: e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp507,0))->r;goto _LL2B2;
_LL2B2:;}break;}}goto _LL223;_LL246: {struct Cyc_Absyn_Cast_e_struct*_tmp509=(
struct Cyc_Absyn_Cast_e_struct*)_tmp4D2;if(_tmp509->tag != 15)goto _LL248;else{
_tmp50A=(void**)& _tmp509->f1;_tmp50B=(void**)((void**)& _tmp509->f1);_tmp50C=
_tmp509->f2;_tmp50D=_tmp509->f3;_tmp50E=_tmp509->f4;}}_LL247: {void*old_t2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp50C->topt))->v;void*new_typ=*_tmp50B;*
_tmp50B=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp50C);{struct _tuple17
_tmpC4B;struct _tuple17 _tmp5D0=(_tmpC4B.f1=Cyc_Tcutil_compress(old_t2),((_tmpC4B.f2=
Cyc_Tcutil_compress(new_typ),_tmpC4B)));void*_tmp5D1;struct Cyc_Absyn_PtrInfo
_tmp5D3;void*_tmp5D4;struct Cyc_Absyn_PtrInfo _tmp5D6;void*_tmp5D7;struct Cyc_Absyn_PtrInfo
_tmp5D9;void*_tmp5DA;_LL2BA: _tmp5D1=_tmp5D0.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp5D2=(struct Cyc_Absyn_PointerType_struct*)_tmp5D1;if(_tmp5D2->tag != 5)goto
_LL2BC;else{_tmp5D3=_tmp5D2->f1;}};_tmp5D4=_tmp5D0.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp5D5=(struct Cyc_Absyn_PointerType_struct*)_tmp5D4;if(_tmp5D5->tag != 5)goto
_LL2BC;else{_tmp5D6=_tmp5D5->f1;}};_LL2BB: {int _tmp5DC=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp5D3.ptr_atts).nullable);int _tmp5DD=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5D6.ptr_atts).nullable);
void*_tmp5DE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp5D3.ptr_atts).bounds);void*_tmp5DF=((void*(*)(void*y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5D6.ptr_atts).bounds);
int _tmp5E0=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp5D3.ptr_atts).zero_term);int _tmp5E1=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp5D6.ptr_atts).zero_term);{struct _tuple17 _tmpC4C;
struct _tuple17 _tmp5E3=(_tmpC4C.f1=_tmp5DE,((_tmpC4C.f2=_tmp5DF,_tmpC4C)));void*
_tmp5E4;struct Cyc_Absyn_Exp*_tmp5E6;void*_tmp5E7;struct Cyc_Absyn_Exp*_tmp5E9;
void*_tmp5EA;struct Cyc_Absyn_Exp*_tmp5EC;void*_tmp5ED;void*_tmp5EF;void*_tmp5F1;
struct Cyc_Absyn_Exp*_tmp5F3;void*_tmp5F4;void*_tmp5F6;_LL2C1: _tmp5E4=_tmp5E3.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp5E5=(struct Cyc_Absyn_Upper_b_struct*)_tmp5E4;
if(_tmp5E5->tag != 1)goto _LL2C3;else{_tmp5E6=_tmp5E5->f1;}};_tmp5E7=_tmp5E3.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp5E8=(struct Cyc_Absyn_Upper_b_struct*)_tmp5E7;
if(_tmp5E8->tag != 1)goto _LL2C3;else{_tmp5E9=_tmp5E8->f1;}};_LL2C2: if((!Cyc_Evexp_c_can_eval(
_tmp5E6) || !Cyc_Evexp_c_can_eval(_tmp5E9)) && !Cyc_Evexp_same_const_exp(_tmp5E6,
_tmp5E9)){const char*_tmpC4F;void*_tmpC4E;(_tmpC4E=0,Cyc_Tcutil_terr(e->loc,((
_tmpC4F="can't validate cast due to potential size differences",_tag_dyneither(
_tmpC4F,sizeof(char),54))),_tag_dyneither(_tmpC4E,sizeof(void*),0)));}if(_tmp5DC
 && !_tmp5DD){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC52;void*_tmpC51;(
_tmpC51=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC52="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC52,
sizeof(char),44))),_tag_dyneither(_tmpC51,sizeof(void*),0)));}if(_tmp50E != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC56;void*_tmpC55[1];struct Cyc_String_pa_struct _tmpC54;(_tmpC54.tag=
0,((_tmpC54.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpC55[0]=& _tmpC54,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpC56="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpC56,sizeof(char),41))),_tag_dyneither(_tmpC55,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp50C);if(do_null_check){if(!_tmp50D){
const char*_tmpC59;void*_tmpC58;(_tmpC58=0,Cyc_Tcutil_warn(e->loc,((_tmpC59="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpC59,sizeof(char),58))),_tag_dyneither(_tmpC58,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC5A;e->r=Cyc_Toc_cast_it_r(*_tmp50B,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC5A=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpC5A)),((_tmpC5A->hd=_tmp50C,((_tmpC5A->tl=0,_tmpC5A)))))),0));};}};}goto
_LL2C0;_LL2C3: _tmp5EA=_tmp5E3.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp5EB=(struct
Cyc_Absyn_Upper_b_struct*)_tmp5EA;if(_tmp5EB->tag != 1)goto _LL2C5;else{_tmp5EC=
_tmp5EB->f1;}};_tmp5ED=_tmp5E3.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5EE=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp5ED;if(_tmp5EE->tag != 0)goto _LL2C5;};
_LL2C4: if(!Cyc_Evexp_c_can_eval(_tmp5EC)){const char*_tmpC5D;void*_tmpC5C;(
_tmpC5C=0,Cyc_Tcutil_terr(e->loc,((_tmpC5D="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC5D,sizeof(char),71))),_tag_dyneither(_tmpC5C,sizeof(void*),0)));}
if(_tmp50E == Cyc_Absyn_NonNull_to_Null){const char*_tmpC61;void*_tmpC60[1];struct
Cyc_String_pa_struct _tmpC5F;(_tmpC5F.tag=0,((_tmpC5F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC60[0]=& _tmpC5F,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC61="conversion mis-classified as null-check: %s",
_tag_dyneither(_tmpC61,sizeof(char),44))),_tag_dyneither(_tmpC60,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){if((_tmp5E0  && !(_tmp5D6.elt_tq).real_const) && !
_tmp5E1)_tmp5EC=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp5EC,Cyc_Absyn_uint_exp(1,
0),0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp5EC,_tmp50C))->r;}else{
struct Cyc_Absyn_Exp*_tmp607=Cyc_Toc__tag_dyneither_e;if(_tmp5E0){struct _tuple0*
_tmp608=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp609=Cyc_Absyn_var_exp(_tmp608,
0);struct Cyc_Absyn_Exp*arg3;{void*_tmp60A=_tmp50C->r;union Cyc_Absyn_Cnst _tmp60C;
struct _dyneither_ptr _tmp60D;_LL2CA: {struct Cyc_Absyn_Const_e_struct*_tmp60B=(
struct Cyc_Absyn_Const_e_struct*)_tmp60A;if(_tmp60B->tag != 0)goto _LL2CC;else{
_tmp60C=_tmp60B->f1;if((_tmp60C.String_c).tag != 7)goto _LL2CC;_tmp60D=(struct
_dyneither_ptr)(_tmp60C.String_c).val;}}_LL2CB: arg3=_tmp5EC;goto _LL2C9;_LL2CC:;
_LL2CD:{struct Cyc_Absyn_Exp*_tmpC62[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp50C),((_tmpC62[1]=_tmp5EC,((_tmpC62[0]=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp609),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC62,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);}goto _LL2C9;_LL2C9:;}if(!_tmp5E1  && !(_tmp5D6.elt_tq).real_const)arg3=
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*
_tmp60F=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5D6.elt_typ),0);struct Cyc_Absyn_Exp*
_tmpC63[3];struct Cyc_Absyn_Exp*_tmp610=Cyc_Absyn_fncall_exp(_tmp607,((_tmpC63[2]=
arg3,((_tmpC63[1]=_tmp60F,((_tmpC63[0]=_tmp609,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC63,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);struct Cyc_Absyn_Stmt*_tmp611=Cyc_Absyn_exp_stmt(_tmp610,0);_tmp611=
Cyc_Absyn_declare_stmt(_tmp608,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)
_tmp50C,_tmp611,0);e->r=Cyc_Toc_stmt_exp_r(_tmp611);};}else{struct Cyc_Absyn_Exp*
_tmpC64[3];e->r=Cyc_Toc_fncall_exp_r(_tmp607,((_tmpC64[2]=_tmp5EC,((_tmpC64[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5D6.elt_typ),0),((_tmpC64[0]=
_tmp50C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC64,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2C0;
_LL2C5: _tmp5EF=_tmp5E3.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp5F0=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5EF;if(_tmp5F0->tag != 0)goto _LL2C7;};_tmp5F1=_tmp5E3.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp5F2=(struct Cyc_Absyn_Upper_b_struct*)_tmp5F1;if(_tmp5F2->tag != 1)goto _LL2C7;
else{_tmp5F3=_tmp5F2->f1;}};_LL2C6: if(!Cyc_Evexp_c_can_eval(_tmp5F3)){const char*
_tmpC67;void*_tmpC66;(_tmpC66=0,Cyc_Tcutil_terr(e->loc,((_tmpC67="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC67,sizeof(char),71))),_tag_dyneither(_tmpC66,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC6A;void*_tmpC69;(_tmpC69=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC6A="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpC6A,sizeof(char),45))),_tag_dyneither(_tmpC69,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp618=_tmp5F3;if(_tmp5E0  && !_tmp5E1)_tmp618=Cyc_Absyn_add_exp(
_tmp5F3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp619=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC6B[3];struct Cyc_Absyn_Exp*_tmp61A=Cyc_Absyn_fncall_exp(
_tmp619,((_tmpC6B[2]=_tmp618,((_tmpC6B[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp5D3.elt_typ),0),((_tmpC6B[0]=_tmp50C,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC6B,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp5DD){struct Cyc_List_List*_tmpC6C;_tmp61A->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_null_e,((_tmpC6C=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpC6C)),((_tmpC6C->hd=Cyc_Absyn_copy_exp(_tmp61A),((_tmpC6C->tl=0,_tmpC6C)))))));}
e->r=Cyc_Toc_cast_it_r(*_tmp50B,_tmp61A);goto _LL2C0;};};_LL2C7: _tmp5F4=_tmp5E3.f1;{
struct Cyc_Absyn_DynEither_b_struct*_tmp5F5=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5F4;if(_tmp5F5->tag != 0)goto _LL2C0;};_tmp5F6=_tmp5E3.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp5F7=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5F6;if(_tmp5F7->tag != 0)goto
_LL2C0;};_LL2C8: DynCast: if((_tmp5E0  && !_tmp5E1) && !(_tmp5D6.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC6F;void*_tmpC6E;(_tmpC6E=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC6F="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpC6F,sizeof(char),70))),_tag_dyneither(_tmpC6E,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp61F=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpC70[3];e->r=Cyc_Toc_fncall_exp_r(_tmp61F,((_tmpC70[2]=Cyc_Absyn_uint_exp(1,0),((
_tmpC70[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5D3.elt_typ),0),((
_tmpC70[0]=_tmp50C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC70,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}goto _LL2C0;
_LL2C0:;}goto _LL2B9;}_LL2BC: _tmp5D7=_tmp5D0.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp5D8=(struct Cyc_Absyn_PointerType_struct*)_tmp5D7;if(_tmp5D8->tag != 5)goto
_LL2BE;else{_tmp5D9=_tmp5D8->f1;}};_tmp5DA=_tmp5D0.f2;{struct Cyc_Absyn_IntType_struct*
_tmp5DB=(struct Cyc_Absyn_IntType_struct*)_tmp5DA;if(_tmp5DB->tag != 6)goto _LL2BE;};
_LL2BD:{void*_tmp621=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp5D9.ptr_atts).bounds);_LL2CF: {struct Cyc_Absyn_DynEither_b_struct*
_tmp622=(struct Cyc_Absyn_DynEither_b_struct*)_tmp621;if(_tmp622->tag != 0)goto
_LL2D1;}_LL2D0: _tmp50C->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp50C->r,
_tmp50C->loc),Cyc_Toc_curr_sp);goto _LL2CE;_LL2D1:;_LL2D2: goto _LL2CE;_LL2CE:;}
goto _LL2B9;_LL2BE:;_LL2BF: goto _LL2B9;_LL2B9:;}goto _LL223;}_LL248: {struct Cyc_Absyn_Address_e_struct*
_tmp50F=(struct Cyc_Absyn_Address_e_struct*)_tmp4D2;if(_tmp50F->tag != 16)goto
_LL24A;else{_tmp510=_tmp50F->f1;}}_LL249:{void*_tmp623=_tmp510->r;struct _tuple0*
_tmp625;struct Cyc_List_List*_tmp626;struct Cyc_List_List*_tmp627;struct Cyc_List_List*
_tmp629;_LL2D4: {struct Cyc_Absyn_Aggregate_e_struct*_tmp624=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp623;if(_tmp624->tag != 29)goto _LL2D6;else{_tmp625=_tmp624->f1;_tmp626=_tmp624->f2;
_tmp627=_tmp624->f3;}}_LL2D5: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC74;void*
_tmpC73[1];struct Cyc_String_pa_struct _tmpC72;(_tmpC72.tag=0,((_tmpC72.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp510->loc)),((
_tmpC73[0]=& _tmpC72,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC74="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC74,
sizeof(char),42))),_tag_dyneither(_tmpC73,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp510->topt))->v,_tmp626 != 0,1,0,
_tmp627,_tmp625))->r;goto _LL2D3;_LL2D6: {struct Cyc_Absyn_Tuple_e_struct*_tmp628=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp623;if(_tmp628->tag != 25)goto _LL2D8;else{
_tmp629=_tmp628->f1;}}_LL2D7: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC78;void*
_tmpC77[1];struct Cyc_String_pa_struct _tmpC76;(_tmpC76.tag=0,((_tmpC76.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp510->loc)),((
_tmpC77[0]=& _tmpC76,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC78="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC78,
sizeof(char),42))),_tag_dyneither(_tmpC77,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_tuple(
nv,1,0,_tmp629))->r;goto _LL2D3;_LL2D8:;_LL2D9: Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(
nv,_tmp510);Cyc_Toc_set_lhs(nv,0);if(!Cyc_Absyn_is_lvalue(_tmp510)){((void(*)(
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
int),int f_env))Cyc_Toc_lvalue_assign)(_tmp510,0,Cyc_Toc_address_lvalue,1);e->r=
Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmp510);}goto _LL2D3;_LL2D3:;}goto _LL223;_LL24A: {struct Cyc_Absyn_New_e_struct*
_tmp511=(struct Cyc_Absyn_New_e_struct*)_tmp4D2;if(_tmp511->tag != 17)goto _LL24C;
else{_tmp512=_tmp511->f1;_tmp513=_tmp511->f2;}}_LL24B: if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC7C;void*_tmpC7B[1];struct Cyc_String_pa_struct _tmpC7A;(_tmpC7A.tag=
0,((_tmpC7A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
_tmp513->loc)),((_tmpC7B[0]=& _tmpC7A,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC7C="%s: new at top-level",_tag_dyneither(
_tmpC7C,sizeof(char),21))),_tag_dyneither(_tmpC7B,sizeof(void*),1)))))));}{void*
_tmp633=_tmp513->r;struct Cyc_List_List*_tmp635;struct Cyc_Absyn_Vardecl*_tmp637;
struct Cyc_Absyn_Exp*_tmp638;struct Cyc_Absyn_Exp*_tmp639;int _tmp63A;struct _tuple0*
_tmp63C;struct Cyc_List_List*_tmp63D;struct Cyc_List_List*_tmp63E;struct Cyc_Absyn_Aggrdecl*
_tmp63F;struct Cyc_List_List*_tmp641;_LL2DB: {struct Cyc_Absyn_Array_e_struct*
_tmp634=(struct Cyc_Absyn_Array_e_struct*)_tmp633;if(_tmp634->tag != 27)goto _LL2DD;
else{_tmp635=_tmp634->f1;}}_LL2DC: {struct _tuple0*_tmp642=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp643=Cyc_Absyn_var_exp(_tmp642,0);struct Cyc_Absyn_Stmt*
_tmp644=Cyc_Toc_init_array(nv,_tmp643,_tmp635,Cyc_Absyn_exp_stmt(_tmp643,0));
void*old_elt_typ;{void*_tmp645=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp647;void*_tmp648;struct Cyc_Absyn_Tqual _tmp649;struct Cyc_Absyn_PtrAtts _tmp64A;
union Cyc_Absyn_Constraint*_tmp64B;_LL2E6: {struct Cyc_Absyn_PointerType_struct*
_tmp646=(struct Cyc_Absyn_PointerType_struct*)_tmp645;if(_tmp646->tag != 5)goto
_LL2E8;else{_tmp647=_tmp646->f1;_tmp648=_tmp647.elt_typ;_tmp649=_tmp647.elt_tq;
_tmp64A=_tmp647.ptr_atts;_tmp64B=_tmp64A.zero_term;}}_LL2E7: old_elt_typ=_tmp648;
goto _LL2E5;_LL2E8:;_LL2E9: {const char*_tmpC7F;void*_tmpC7E;old_elt_typ=((_tmpC7E=
0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC7F="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(
_tmpC7F,sizeof(char),52))),_tag_dyneither(_tmpC7E,sizeof(void*),0))));}_LL2E5:;}{
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp64E=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp64F=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp635),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp512 == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp64F);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp512;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp64F);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp642,_tmp64E,(struct Cyc_Absyn_Exp*)e1,_tmp644,0));goto
_LL2DA;};}_LL2DD: {struct Cyc_Absyn_Comprehension_e_struct*_tmp636=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp633;if(_tmp636->tag != 28)goto _LL2DF;else{_tmp637=_tmp636->f1;_tmp638=_tmp636->f2;
_tmp639=_tmp636->f3;_tmp63A=_tmp636->f4;}}_LL2DE: {int is_dyneither_ptr=0;{void*
_tmp650=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp652;void*_tmp653;
struct Cyc_Absyn_Tqual _tmp654;struct Cyc_Absyn_PtrAtts _tmp655;union Cyc_Absyn_Constraint*
_tmp656;union Cyc_Absyn_Constraint*_tmp657;_LL2EB: {struct Cyc_Absyn_PointerType_struct*
_tmp651=(struct Cyc_Absyn_PointerType_struct*)_tmp650;if(_tmp651->tag != 5)goto
_LL2ED;else{_tmp652=_tmp651->f1;_tmp653=_tmp652.elt_typ;_tmp654=_tmp652.elt_tq;
_tmp655=_tmp652.ptr_atts;_tmp656=_tmp655.bounds;_tmp657=_tmp655.zero_term;}}
_LL2EC: is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp656)== (void*)& Cyc_Absyn_DynEither_b_val;goto _LL2EA;
_LL2ED:;_LL2EE: {const char*_tmpC82;void*_tmpC81;(_tmpC81=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC82="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpC82,sizeof(char),42))),_tag_dyneither(_tmpC81,sizeof(void*),0)));}
_LL2EA:;}{struct _tuple0*max=Cyc_Toc_temp_var();struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp639->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp638);{struct Cyc_Absyn_Exp*_tmp65A=
Cyc_Absyn_var_exp(max,0);if(_tmp63A)_tmp65A=Cyc_Absyn_add_exp(_tmp65A,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp637,Cyc_Absyn_var_exp(max,0),_tmp639,_tmp63A,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp65C;struct _RegionHandle*_tmp65D;struct Cyc_Toc_Env*_tmp65B=
nv;_tmp65C=*_tmp65B;_tmp65D=_tmp65C.rgn;{struct _tuple19*_tmpC85;struct Cyc_List_List*
_tmpC84;struct Cyc_List_List*decls=(_tmpC84=_region_malloc(_tmp65D,sizeof(*
_tmpC84)),((_tmpC84->hd=((_tmpC85=_region_malloc(_tmp65D,sizeof(*_tmpC85)),((
_tmpC85->f1=max,((_tmpC85->f2=Cyc_Absyn_uint_typ,((_tmpC85->f3=(struct Cyc_Absyn_Exp*)
_tmp638,_tmpC85)))))))),((_tmpC84->tl=0,_tmpC84)))));struct Cyc_Absyn_Exp*ai;if(
_tmp512 == 0  || Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*_tmpC86[2];ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((_tmpC86[1]=_tmp65A,((
_tmpC86[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC86,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp512;Cyc_Toc_exp_to_c(
nv,r);{struct Cyc_Absyn_Exp*_tmpC87[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpC87[1]=_tmp65A,((_tmpC87[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC87,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct _tuple19*_tmpC8A;struct Cyc_List_List*
_tmpC89;decls=((_tmpC89=_region_malloc(_tmp65D,sizeof(*_tmpC89)),((_tmpC89->hd=((
_tmpC8A=_region_malloc(_tmp65D,sizeof(*_tmpC8A)),((_tmpC8A->f1=a,((_tmpC8A->f2=
ptr_typ,((_tmpC8A->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpC8A)))))))),((_tmpC89->tl=
decls,_tmpC89))))));}if(is_dyneither_ptr){struct _tuple0*_tmp662=Cyc_Toc_temp_var();
void*_tmp663=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp664=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC8B[3];struct Cyc_Absyn_Exp*_tmp665=Cyc_Absyn_fncall_exp(
_tmp664,((_tmpC8B[2]=_tmp65A,((_tmpC8B[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((
_tmpC8B[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC8B,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{
struct _tuple19*_tmpC8E;struct Cyc_List_List*_tmpC8D;decls=((_tmpC8D=
_region_malloc(_tmp65D,sizeof(*_tmpC8D)),((_tmpC8D->hd=((_tmpC8E=_region_malloc(
_tmp65D,sizeof(*_tmpC8E)),((_tmpC8E->f1=_tmp662,((_tmpC8E->f2=_tmp663,((_tmpC8E->f3=(
struct Cyc_Absyn_Exp*)_tmp665,_tmpC8E)))))))),((_tmpC8D->tl=decls,_tmpC8D))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp662,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp669=decls;for(0;_tmp669 != 0;_tmp669=_tmp669->tl){struct
_tuple0*_tmp66B;void*_tmp66C;struct Cyc_Absyn_Exp*_tmp66D;struct _tuple19 _tmp66A=*((
struct _tuple19*)_tmp669->hd);_tmp66B=_tmp66A.f1;_tmp66C=_tmp66A.f2;_tmp66D=
_tmp66A.f3;s=Cyc_Absyn_declare_stmt(_tmp66B,_tmp66C,_tmp66D,s,0);}}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL2DA;};};};}_LL2DF: {struct Cyc_Absyn_Aggregate_e_struct*_tmp63B=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp633;if(_tmp63B->tag != 29)goto _LL2E1;
else{_tmp63C=_tmp63B->f1;_tmp63D=_tmp63B->f2;_tmp63E=_tmp63B->f3;_tmp63F=_tmp63B->f4;}}
_LL2E0: e->r=(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp513->topt))->v,_tmp63D != 0,1,_tmp512,_tmp63E,_tmp63C))->r;goto _LL2DA;_LL2E1: {
struct Cyc_Absyn_Tuple_e_struct*_tmp640=(struct Cyc_Absyn_Tuple_e_struct*)_tmp633;
if(_tmp640->tag != 25)goto _LL2E3;else{_tmp641=_tmp640->f1;}}_LL2E2: e->r=(Cyc_Toc_init_tuple(
nv,1,_tmp512,_tmp641))->r;goto _LL2DA;_LL2E3:;_LL2E4: {void*old_elt_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp513->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);struct _tuple0*_tmp670=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp671=Cyc_Absyn_var_exp(_tmp670,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(_tmp671,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp512
== 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp512;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp672=_tmp513->r;void*_tmp674;
struct Cyc_Absyn_Exp*_tmp675;_LL2F0: {struct Cyc_Absyn_Cast_e_struct*_tmp673=(
struct Cyc_Absyn_Cast_e_struct*)_tmp672;if(_tmp673->tag != 15)goto _LL2F2;else{
_tmp674=(void*)_tmp673->f1;_tmp675=_tmp673->f2;}}_LL2F1:{struct _tuple17 _tmpC8F;
struct _tuple17 _tmp677=(_tmpC8F.f1=Cyc_Tcutil_compress(_tmp674),((_tmpC8F.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp675->topt))->v),_tmpC8F)));void*
_tmp678;struct Cyc_Absyn_PtrInfo _tmp67A;void*_tmp67B;struct Cyc_Absyn_PtrAtts
_tmp67C;union Cyc_Absyn_Constraint*_tmp67D;void*_tmp67E;struct Cyc_Absyn_PtrInfo
_tmp680;struct Cyc_Absyn_PtrAtts _tmp681;union Cyc_Absyn_Constraint*_tmp682;_LL2F5:
_tmp678=_tmp677.f1;{struct Cyc_Absyn_PointerType_struct*_tmp679=(struct Cyc_Absyn_PointerType_struct*)
_tmp678;if(_tmp679->tag != 5)goto _LL2F7;else{_tmp67A=_tmp679->f1;_tmp67B=_tmp67A.elt_typ;
_tmp67C=_tmp67A.ptr_atts;_tmp67D=_tmp67C.bounds;}};_tmp67E=_tmp677.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp67F=(struct Cyc_Absyn_PointerType_struct*)_tmp67E;if(_tmp67F->tag != 5)goto
_LL2F7;else{_tmp680=_tmp67F->f1;_tmp681=_tmp680.ptr_atts;_tmp682=_tmp681.bounds;}};
_LL2F6:{struct _tuple17 _tmpC90;struct _tuple17 _tmp684=(_tmpC90.f1=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp67D),((_tmpC90.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp682),_tmpC90)));void*_tmp685;void*_tmp687;struct Cyc_Absyn_Exp*
_tmp689;_LL2FA: _tmp685=_tmp684.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp686=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp685;if(_tmp686->tag != 0)goto _LL2FC;};
_tmp687=_tmp684.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp688=(struct Cyc_Absyn_Upper_b_struct*)
_tmp687;if(_tmp688->tag != 1)goto _LL2FC;else{_tmp689=_tmp688->f1;}};_LL2FB: Cyc_Toc_exp_to_c(
nv,_tmp675);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp68A=Cyc_Toc__init_dyneither_ptr_e;{struct Cyc_Absyn_Exp*_tmpC91[4];e->r=Cyc_Toc_fncall_exp_r(
_tmp68A,((_tmpC91[3]=_tmp689,((_tmpC91[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp67B),0),((_tmpC91[1]=_tmp675,((_tmpC91[0]=mexp,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC91,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))));}goto _LL2F9;};_LL2FC:;_LL2FD: goto _LL2F9;_LL2F9:;}goto _LL2F4;_LL2F7:;
_LL2F8: goto _LL2F4;_LL2F4:;}goto _LL2EF;_LL2F2:;_LL2F3: goto _LL2EF;_LL2EF:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp68C=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp671),
0);struct Cyc_Absyn_Exp*_tmp68D=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,
_tmp513);_tmp68C=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
_tmp671,_tmp68D,0),_tmp513,0),_tmp68C,0);{void*_tmp68E=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp670,
_tmp68E,(struct Cyc_Absyn_Exp*)mexp,_tmp68C,0));};}goto _LL2DA;};}_LL2DA:;}goto
_LL223;_LL24C: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp514=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp4D2;if(_tmp514->tag != 19)goto _LL24E;else{_tmp515=_tmp514->f1;}}_LL24D: Cyc_Toc_exp_to_c(
nv,_tmp515);goto _LL223;_LL24E: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp516=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp4D2;if(_tmp516->tag != 18)goto _LL250;
else{_tmp517=(void*)_tmp516->f1;}}_LL24F:{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpC94;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpC93;e->r=(void*)((_tmpC93=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC93)),((_tmpC93[0]=((_tmpC94.tag=
18,((_tmpC94.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp517),_tmpC94)))),_tmpC93))));}
goto _LL223;_LL250: {struct Cyc_Absyn_Offsetof_e_struct*_tmp518=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp4D2;if(_tmp518->tag != 20)goto _LL252;else{_tmp519=(void*)_tmp518->f1;_tmp51A=(
void*)_tmp518->f2;{struct Cyc_Absyn_StructField_struct*_tmp51B=(struct Cyc_Absyn_StructField_struct*)
_tmp51A;if(_tmp51B->tag != 0)goto _LL252;else{_tmp51C=_tmp51B->f1;}};}}_LL251:{
struct Cyc_Absyn_Offsetof_e_struct _tmpC9E;struct Cyc_Absyn_StructField_struct
_tmpC9D;struct Cyc_Absyn_StructField_struct*_tmpC9C;struct Cyc_Absyn_Offsetof_e_struct*
_tmpC9B;e->r=(void*)((_tmpC9B=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpC9B)),((_tmpC9B[0]=((_tmpC9E.tag=20,((_tmpC9E.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp519),((_tmpC9E.f2=(void*)((void*)((_tmpC9C=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpC9C)),((_tmpC9C[0]=((_tmpC9D.tag=0,((_tmpC9D.f1=_tmp51C,_tmpC9D)))),
_tmpC9C))))),_tmpC9E)))))),_tmpC9B))));}goto _LL223;_LL252: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp51D=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp4D2;if(_tmp51D->tag != 20)goto
_LL254;else{_tmp51E=(void*)_tmp51D->f1;_tmp51F=(void*)_tmp51D->f2;{struct Cyc_Absyn_TupleIndex_struct*
_tmp520=(struct Cyc_Absyn_TupleIndex_struct*)_tmp51F;if(_tmp520->tag != 1)goto
_LL254;else{_tmp521=_tmp520->f1;}};}}_LL253:{void*_tmp695=Cyc_Tcutil_compress(
_tmp51E);struct Cyc_Absyn_AggrInfo _tmp697;union Cyc_Absyn_AggrInfoU _tmp698;struct
Cyc_List_List*_tmp69A;_LL2FF: {struct Cyc_Absyn_AggrType_struct*_tmp696=(struct
Cyc_Absyn_AggrType_struct*)_tmp695;if(_tmp696->tag != 12)goto _LL301;else{_tmp697=
_tmp696->f1;_tmp698=_tmp697.aggr_info;}}_LL300: {struct Cyc_Absyn_Aggrdecl*
_tmp69D=Cyc_Absyn_get_known_aggrdecl(_tmp698);if(_tmp69D->impl == 0){const char*
_tmpCA1;void*_tmpCA0;(_tmpCA0=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA1="struct fields must be known",
_tag_dyneither(_tmpCA1,sizeof(char),28))),_tag_dyneither(_tmpCA0,sizeof(void*),0)));}
_tmp69A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp69D->impl))->fields;goto
_LL302;}_LL301: {struct Cyc_Absyn_AnonAggrType_struct*_tmp699=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp695;if(_tmp699->tag != 13)goto _LL303;else{_tmp69A=_tmp699->f2;}}_LL302: {
struct Cyc_Absyn_Aggrfield*_tmp6A0=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp69A,(int)_tmp521);{struct Cyc_Absyn_Offsetof_e_struct
_tmpCAB;struct Cyc_Absyn_StructField_struct _tmpCAA;struct Cyc_Absyn_StructField_struct*
_tmpCA9;struct Cyc_Absyn_Offsetof_e_struct*_tmpCA8;e->r=(void*)((_tmpCA8=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpCA8)),((_tmpCA8[0]=((_tmpCAB.tag=
20,((_tmpCAB.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp51E),((_tmpCAB.f2=(void*)((
void*)((_tmpCA9=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpCA9)),((
_tmpCA9[0]=((_tmpCAA.tag=0,((_tmpCAA.f1=_tmp6A0->name,_tmpCAA)))),_tmpCA9))))),
_tmpCAB)))))),_tmpCA8))));}goto _LL2FE;}_LL303: {struct Cyc_Absyn_TupleType_struct*
_tmp69B=(struct Cyc_Absyn_TupleType_struct*)_tmp695;if(_tmp69B->tag != 11)goto
_LL305;}_LL304:{struct Cyc_Absyn_Offsetof_e_struct _tmpCB5;struct Cyc_Absyn_StructField_struct
_tmpCB4;struct Cyc_Absyn_StructField_struct*_tmpCB3;struct Cyc_Absyn_Offsetof_e_struct*
_tmpCB2;e->r=(void*)((_tmpCB2=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpCB2)),((_tmpCB2[0]=((_tmpCB5.tag=20,((_tmpCB5.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp51E),((_tmpCB5.f2=(void*)((void*)((_tmpCB3=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpCB3)),((_tmpCB3[0]=((_tmpCB4.tag=0,((_tmpCB4.f1=Cyc_Absyn_fieldname((
int)(_tmp521 + 1)),_tmpCB4)))),_tmpCB3))))),_tmpCB5)))))),_tmpCB2))));}goto _LL2FE;
_LL305: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp69C=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp695;if(_tmp69C->tag != 4)goto _LL307;}_LL306: if(_tmp521 == 0){struct Cyc_Absyn_Offsetof_e_struct
_tmpCBF;struct Cyc_Absyn_StructField_struct _tmpCBE;struct Cyc_Absyn_StructField_struct*
_tmpCBD;struct Cyc_Absyn_Offsetof_e_struct*_tmpCBC;e->r=(void*)((_tmpCBC=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpCBC)),((_tmpCBC[0]=((_tmpCBF.tag=
20,((_tmpCBF.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp51E),((_tmpCBF.f2=(void*)((
void*)((_tmpCBD=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpCBD)),((
_tmpCBD[0]=((_tmpCBE.tag=0,((_tmpCBE.f1=Cyc_Toc_tag_sp,_tmpCBE)))),_tmpCBD))))),
_tmpCBF)))))),_tmpCBC))));}else{struct Cyc_Absyn_Offsetof_e_struct _tmpCC9;struct
Cyc_Absyn_StructField_struct _tmpCC8;struct Cyc_Absyn_StructField_struct*_tmpCC7;
struct Cyc_Absyn_Offsetof_e_struct*_tmpCC6;e->r=(void*)((_tmpCC6=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmpCC6)),((_tmpCC6[0]=((_tmpCC9.tag=20,((_tmpCC9.f1=(
void*)Cyc_Toc_typ_to_c_array(_tmp51E),((_tmpCC9.f2=(void*)((void*)((_tmpCC7=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpCC7)),((_tmpCC7[0]=((_tmpCC8.tag=
0,((_tmpCC8.f1=Cyc_Absyn_fieldname((int)_tmp521),_tmpCC8)))),_tmpCC7))))),
_tmpCC9)))))),_tmpCC6))));}goto _LL2FE;_LL307:;_LL308: {const char*_tmpCCC;void*
_tmpCCB;(_tmpCCB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCCC="impossible type for offsetof tuple index",_tag_dyneither(_tmpCCC,
sizeof(char),41))),_tag_dyneither(_tmpCCB,sizeof(void*),0)));}_LL2FE:;}goto
_LL223;_LL254: {struct Cyc_Absyn_Deref_e_struct*_tmp522=(struct Cyc_Absyn_Deref_e_struct*)
_tmp4D2;if(_tmp522->tag != 21)goto _LL256;else{_tmp523=_tmp522->f1;}}_LL255: {int
_tmp6B3=Cyc_Toc_in_lhs(nv);Cyc_Toc_set_lhs(nv,0);{void*_tmp6B4=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp523->topt))->v);{void*_tmp6B5=
_tmp6B4;struct Cyc_Absyn_PtrInfo _tmp6B7;void*_tmp6B8;struct Cyc_Absyn_Tqual _tmp6B9;
struct Cyc_Absyn_PtrAtts _tmp6BA;void*_tmp6BB;union Cyc_Absyn_Constraint*_tmp6BC;
union Cyc_Absyn_Constraint*_tmp6BD;union Cyc_Absyn_Constraint*_tmp6BE;_LL30A: {
struct Cyc_Absyn_PointerType_struct*_tmp6B6=(struct Cyc_Absyn_PointerType_struct*)
_tmp6B5;if(_tmp6B6->tag != 5)goto _LL30C;else{_tmp6B7=_tmp6B6->f1;_tmp6B8=_tmp6B7.elt_typ;
_tmp6B9=_tmp6B7.elt_tq;_tmp6BA=_tmp6B7.ptr_atts;_tmp6BB=_tmp6BA.rgn;_tmp6BC=
_tmp6BA.nullable;_tmp6BD=_tmp6BA.bounds;_tmp6BE=_tmp6BA.zero_term;}}_LL30B:{void*
_tmp6BF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6BD);struct Cyc_Absyn_Exp*_tmp6C1;_LL30F: {struct Cyc_Absyn_Upper_b_struct*
_tmp6C0=(struct Cyc_Absyn_Upper_b_struct*)_tmp6BF;if(_tmp6C0->tag != 1)goto _LL311;
else{_tmp6C1=_tmp6C0->f1;}}_LL310: {int do_null_check=Cyc_Toc_need_null_check(
_tmp523);Cyc_Toc_exp_to_c(nv,_tmp523);if(do_null_check){if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCCF;void*_tmpCCE;(_tmpCCE=0,Cyc_Tcutil_warn(e->loc,((_tmpCCF="inserted null check due to dereference",
_tag_dyneither(_tmpCCF,sizeof(char),39))),_tag_dyneither(_tmpCCE,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpCD0;_tmp523->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp6B4),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpCD0=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpCD0)),((_tmpCD0->hd=Cyc_Absyn_copy_exp(_tmp523),((_tmpCD0->tl=0,
_tmpCD0)))))),0));};}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp6BE)){unsigned int _tmp6C7;int _tmp6C8;struct _tuple10 _tmp6C6=Cyc_Evexp_eval_const_uint_exp(
_tmp6C1);_tmp6C7=_tmp6C6.f1;_tmp6C8=_tmp6C6.f2;if(!_tmp6C8  || _tmp6C7 <= 0){const
char*_tmpCD3;void*_tmpCD2;(_tmpCD2=0,Cyc_Tcutil_terr(e->loc,((_tmpCD3="cannot determine dereference is in bounds",
_tag_dyneither(_tmpCD3,sizeof(char),42))),_tag_dyneither(_tmpCD2,sizeof(void*),0)));}}
goto _LL30E;}_LL311: {struct Cyc_Absyn_DynEither_b_struct*_tmp6C2=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp6BF;if(_tmp6C2->tag != 0)goto _LL30E;}_LL312: {struct Cyc_Absyn_Exp*_tmp6CB=Cyc_Absyn_uint_exp(
0,0);{struct Cyc_Core_Opt*_tmpCD4;_tmp6CB->topt=((_tmpCD4=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpCD4)),((_tmpCD4->v=Cyc_Absyn_uint_typ,_tmpCD4))));}e->r=Cyc_Toc_subscript_exp_r(
_tmp523,_tmp6CB);Cyc_Toc_exp_to_c(nv,e);goto _LL30E;}_LL30E:;}goto _LL309;_LL30C:;
_LL30D: {const char*_tmpCD7;void*_tmpCD6;(_tmpCD6=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD7="exp_to_c: Deref: non-pointer",
_tag_dyneither(_tmpCD7,sizeof(char),29))),_tag_dyneither(_tmpCD6,sizeof(void*),0)));}
_LL309:;}Cyc_Toc_set_lhs(nv,_tmp6B3);goto _LL223;};}_LL256: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp524=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp4D2;if(_tmp524->tag != 22)goto
_LL258;else{_tmp525=_tmp524->f1;_tmp526=_tmp524->f2;_tmp527=_tmp524->f3;_tmp528=
_tmp524->f4;}}_LL257: {int is_poly=Cyc_Toc_is_poly_project(e);void*e1_cyc_type=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp525->topt))->v;Cyc_Toc_exp_to_c(nv,
_tmp525);if(_tmp527  && _tmp528)e->r=Cyc_Toc_check_tagged_union(_tmp525,Cyc_Toc_typ_to_c(
e1_cyc_type),e1_cyc_type,_tmp526,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrmember_exp);
if(is_poly)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp(e->r,0),0))->r;goto _LL223;}_LL258: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp529=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp4D2;if(_tmp529->tag != 23)goto _LL25A;else{_tmp52A=_tmp529->f1;_tmp52B=_tmp529->f2;
_tmp52C=_tmp529->f3;_tmp52D=_tmp529->f4;}}_LL259: {int _tmp6CF=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp52A->topt))->v);int do_null_check=Cyc_Toc_need_null_check(_tmp52A);
Cyc_Toc_exp_to_c(nv,_tmp52A);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp6D1;
struct Cyc_Absyn_Tqual _tmp6D2;struct Cyc_Absyn_PtrAtts _tmp6D3;void*_tmp6D4;union
Cyc_Absyn_Constraint*_tmp6D5;union Cyc_Absyn_Constraint*_tmp6D6;union Cyc_Absyn_Constraint*
_tmp6D7;struct Cyc_Absyn_PtrInfo _tmp6D0=Cyc_Toc_get_ptr_type(e1typ);_tmp6D1=
_tmp6D0.elt_typ;_tmp6D2=_tmp6D0.elt_tq;_tmp6D3=_tmp6D0.ptr_atts;_tmp6D4=_tmp6D3.rgn;
_tmp6D5=_tmp6D3.nullable;_tmp6D6=_tmp6D3.bounds;_tmp6D7=_tmp6D3.zero_term;{void*
_tmp6D8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6D6);struct Cyc_Absyn_Exp*_tmp6DA;_LL314: {struct Cyc_Absyn_Upper_b_struct*
_tmp6D9=(struct Cyc_Absyn_Upper_b_struct*)_tmp6D8;if(_tmp6D9->tag != 1)goto _LL316;
else{_tmp6DA=_tmp6D9->f1;}}_LL315: {unsigned int _tmp6DD;int _tmp6DE;struct
_tuple10 _tmp6DC=Cyc_Evexp_eval_const_uint_exp(_tmp6DA);_tmp6DD=_tmp6DC.f1;
_tmp6DE=_tmp6DC.f2;if(_tmp6DE){if(_tmp6DD < 1){const char*_tmpCDA;void*_tmpCD9;(
_tmpCD9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpCDA="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpCDA,
sizeof(char),44))),_tag_dyneither(_tmpCD9,sizeof(void*),0)));}if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCDD;void*_tmpCDC;(_tmpCDC=0,Cyc_Tcutil_warn(
e->loc,((_tmpCDD="inserted null check due to dereference",_tag_dyneither(_tmpCDD,
sizeof(char),39))),_tag_dyneither(_tmpCDC,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCDE[1];_tmp52A->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpCDE[0]=Cyc_Absyn_new_exp(_tmp52A->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCDE,sizeof(struct Cyc_Absyn_Exp*),
1)))),0));};}}else{if(!Cyc_Evexp_c_can_eval(_tmp6DA)){const char*_tmpCE1;void*
_tmpCE0;(_tmpCE0=0,Cyc_Tcutil_terr(e->loc,((_tmpCE1="cannot determine pointer dereference in bounds",
_tag_dyneither(_tmpCE1,sizeof(char),47))),_tag_dyneither(_tmpCE0,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpCE2[4];_tmp52A->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((_tmpCE2[3]=
Cyc_Absyn_uint_exp(0,0),((_tmpCE2[2]=Cyc_Absyn_sizeoftyp_exp(_tmp6D1,0),((
_tmpCE2[1]=_tmp6DA,((_tmpCE2[0]=Cyc_Absyn_new_exp(_tmp52A->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE2,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0));};}goto _LL313;}_LL316: {struct Cyc_Absyn_DynEither_b_struct*
_tmp6DB=(struct Cyc_Absyn_DynEither_b_struct*)_tmp6D8;if(_tmp6DB->tag != 0)goto
_LL313;}_LL317: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp6D1);{struct Cyc_Absyn_Exp*
_tmpCE3[3];_tmp52A->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp6D2),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_dyneither_subscript_e,((_tmpCE3[2]=Cyc_Absyn_uint_exp(0,0),((
_tmpCE3[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCE3[0]=Cyc_Absyn_new_exp(_tmp52A->r,
0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCE3,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}goto _LL313;}_LL313:;}if(
_tmp52C  && _tmp52D)e->r=Cyc_Toc_check_tagged_union(_tmp52A,Cyc_Toc_typ_to_c(
e1typ),_tmp6D1,_tmp52B,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);if(is_poly
 && _tmp52D)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp(e->r,0),0))->r;Cyc_Toc_set_lhs(nv,
_tmp6CF);goto _LL223;};};}_LL25A: {struct Cyc_Absyn_Subscript_e_struct*_tmp52E=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp4D2;if(_tmp52E->tag != 24)goto _LL25C;
else{_tmp52F=_tmp52E->f1;_tmp530=_tmp52E->f2;}}_LL25B: {int _tmp6E8=Cyc_Toc_in_lhs(
nv);Cyc_Toc_set_lhs(nv,0);{void*_tmp6E9=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp52F->topt))->v);{void*_tmp6EA=_tmp6E9;struct Cyc_List_List*
_tmp6EC;struct Cyc_Absyn_PtrInfo _tmp6EE;void*_tmp6EF;struct Cyc_Absyn_Tqual _tmp6F0;
struct Cyc_Absyn_PtrAtts _tmp6F1;void*_tmp6F2;union Cyc_Absyn_Constraint*_tmp6F3;
union Cyc_Absyn_Constraint*_tmp6F4;union Cyc_Absyn_Constraint*_tmp6F5;_LL319: {
struct Cyc_Absyn_TupleType_struct*_tmp6EB=(struct Cyc_Absyn_TupleType_struct*)
_tmp6EA;if(_tmp6EB->tag != 11)goto _LL31B;else{_tmp6EC=_tmp6EB->f1;}}_LL31A: Cyc_Toc_exp_to_c(
nv,_tmp52F);Cyc_Toc_exp_to_c(nv,_tmp530);{unsigned int _tmp6F7;int _tmp6F8;struct
_tuple10 _tmp6F6=Cyc_Evexp_eval_const_uint_exp(_tmp530);_tmp6F7=_tmp6F6.f1;
_tmp6F8=_tmp6F6.f2;if(!_tmp6F8){const char*_tmpCE6;void*_tmpCE5;(_tmpCE5=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCE6="unknown tuple subscript in translation to C",
_tag_dyneither(_tmpCE6,sizeof(char),44))),_tag_dyneither(_tmpCE5,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp52F,Cyc_Absyn_fieldname((int)(_tmp6F7 + 1)));
goto _LL318;};_LL31B: {struct Cyc_Absyn_PointerType_struct*_tmp6ED=(struct Cyc_Absyn_PointerType_struct*)
_tmp6EA;if(_tmp6ED->tag != 5)goto _LL31D;else{_tmp6EE=_tmp6ED->f1;_tmp6EF=_tmp6EE.elt_typ;
_tmp6F0=_tmp6EE.elt_tq;_tmp6F1=_tmp6EE.ptr_atts;_tmp6F2=_tmp6F1.rgn;_tmp6F3=
_tmp6F1.nullable;_tmp6F4=_tmp6F1.bounds;_tmp6F5=_tmp6F1.zero_term;}}_LL31C: {
struct Cyc_List_List*_tmp6FB=Cyc_Toc_get_relns(_tmp52F);int in_bnds=0;{void*
_tmp6FC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6F4);_LL320:;_LL321: in_bnds=Cyc_Toc_check_bounds(_tmp6E9,_tmp6FB,_tmp52F,
_tmp530);if(Cyc_Toc_warn_bounds_checks  && !in_bnds){const char*_tmpCEA;void*
_tmpCE9[1];struct Cyc_String_pa_struct _tmpCE8;(_tmpCE8.tag=0,((_tmpCE8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCE9[0]=&
_tmpCE8,Cyc_Tcutil_warn(e->loc,((_tmpCEA="bounds check necessary for %s",
_tag_dyneither(_tmpCEA,sizeof(char),30))),_tag_dyneither(_tmpCE9,sizeof(void*),1)))))));}
_LL31F:;}Cyc_Toc_exp_to_c(nv,_tmp52F);Cyc_Toc_exp_to_c(nv,_tmp530);++ Cyc_Toc_total_bounds_checks;{
void*_tmp700=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp6F4);struct Cyc_Absyn_Exp*_tmp702;_LL323: {struct Cyc_Absyn_Upper_b_struct*
_tmp701=(struct Cyc_Absyn_Upper_b_struct*)_tmp700;if(_tmp701->tag != 1)goto _LL325;
else{_tmp702=_tmp701->f1;}}_LL324: {int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp6F3);void*ta1=Cyc_Toc_typ_to_c(_tmp6EF);void*ta2=
Cyc_Absyn_cstar_typ(ta1,_tmp6F0);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6F5)){if(!Cyc_Evexp_c_can_eval(_tmp702)){const char*_tmpCED;void*_tmpCEC;(
_tmpCEC=0,Cyc_Tcutil_terr(e->loc,((_tmpCED="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCED,sizeof(char),40))),_tag_dyneither(_tmpCEC,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp52F);struct Cyc_Absyn_Exp*_tmpCEE[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpCEE[2]=_tmp530,((_tmpCEE[1]=_tmp702,((
_tmpCEE[0]=_tmp52F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCEE,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp702)){const char*_tmpCF1;void*_tmpCF0;(
_tmpCF0=0,Cyc_Tcutil_terr(e->loc,((_tmpCF1="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCF1,sizeof(char),40))),_tag_dyneither(_tmpCF0,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCF4;void*_tmpCF3;(_tmpCF3=0,Cyc_Tcutil_warn(
e->loc,((_tmpCF4="inserted null check due to dereference",_tag_dyneither(_tmpCF4,
sizeof(char),39))),_tag_dyneither(_tmpCF3,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCF5[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((
_tmpCF5[3]=_tmp530,((_tmpCF5[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCF5[1]=
_tmp702,((_tmpCF5[0]=_tmp52F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCF5,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{if(
!Cyc_Evexp_c_can_eval(_tmp702)){const char*_tmpCF8;void*_tmpCF7;(_tmpCF7=0,Cyc_Tcutil_terr(
e->loc,((_tmpCF8="cannot determine subscript is in bounds",_tag_dyneither(
_tmpCF8,sizeof(char),40))),_tag_dyneither(_tmpCF7,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCF9[2];_tmp530->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((
_tmpCF9[1]=Cyc_Absyn_copy_exp(_tmp530),((_tmpCF9[0]=_tmp702,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCF9,sizeof(struct Cyc_Absyn_Exp*),
2)))))));};}}}goto _LL322;}_LL325: {struct Cyc_Absyn_DynEither_b_struct*_tmp703=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp700;if(_tmp703->tag != 0)goto _LL322;}
_LL326: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp6EF);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6F0),
Cyc_Absyn_aggrmember_exp(_tmp52F,Cyc_Toc_curr_sp,0)),_tmp530);}else{struct Cyc_Absyn_Exp*
_tmp70F=Cyc_Toc__check_dyneither_subscript_e;struct Cyc_Absyn_Exp*_tmpCFA[3];e->r=
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6F0),Cyc_Absyn_fncall_exp(
_tmp70F,((_tmpCFA[2]=_tmp530,((_tmpCFA[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((
_tmpCFA[0]=_tmp52F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCFA,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL322;}
_LL322:;}goto _LL318;}_LL31D:;_LL31E: {const char*_tmpCFD;void*_tmpCFC;(_tmpCFC=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCFD="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(
_tmpCFD,sizeof(char),49))),_tag_dyneither(_tmpCFC,sizeof(void*),0)));}_LL318:;}
Cyc_Toc_set_lhs(nv,_tmp6E8);goto _LL223;};}_LL25C: {struct Cyc_Absyn_Tuple_e_struct*
_tmp531=(struct Cyc_Absyn_Tuple_e_struct*)_tmp4D2;if(_tmp531->tag != 25)goto _LL25E;
else{_tmp532=_tmp531->f1;}}_LL25D: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(
nv,0,0,_tmp532))->r;else{struct Cyc_List_List*_tmp713=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_tup_to_c,_tmp532);void*_tmp714=Cyc_Toc_add_tuple_type(_tmp713);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp532 != 0;(_tmp532=_tmp532->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp532->hd);{struct _tuple15*_tmpD00;struct Cyc_List_List*
_tmpCFF;dles=((_tmpCFF=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpCFF)),((
_tmpCFF->hd=((_tmpD00=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD00)),((
_tmpD00->f1=0,((_tmpD00->f2=(struct Cyc_Absyn_Exp*)_tmp532->hd,_tmpD00)))))),((
_tmpCFF->tl=dles,_tmpCFF))))));};}}dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(dles);e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}goto _LL223;
_LL25E: {struct Cyc_Absyn_Array_e_struct*_tmp533=(struct Cyc_Absyn_Array_e_struct*)
_tmp4D2;if(_tmp533->tag != 27)goto _LL260;else{_tmp534=_tmp533->f1;}}_LL25F: e->r=
Cyc_Toc_unresolvedmem_exp_r(0,_tmp534);{struct Cyc_List_List*_tmp717=_tmp534;for(
0;_tmp717 != 0;_tmp717=_tmp717->tl){struct _tuple15 _tmp719;struct Cyc_Absyn_Exp*
_tmp71A;struct _tuple15*_tmp718=(struct _tuple15*)_tmp717->hd;_tmp719=*_tmp718;
_tmp71A=_tmp719.f2;Cyc_Toc_exp_to_c(nv,_tmp71A);}}goto _LL223;_LL260: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp535=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp4D2;if(_tmp535->tag != 28)
goto _LL262;else{_tmp536=_tmp535->f1;_tmp537=_tmp535->f2;_tmp538=_tmp535->f3;
_tmp539=_tmp535->f4;}}_LL261: {unsigned int _tmp71C;int _tmp71D;struct _tuple10
_tmp71B=Cyc_Evexp_eval_const_uint_exp(_tmp537);_tmp71C=_tmp71B.f1;_tmp71D=
_tmp71B.f2;{void*_tmp71E=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp538->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp538);{struct Cyc_List_List*
es=0;if(!Cyc_Toc_is_zero(_tmp538)){if(!_tmp71D){const char*_tmpD03;void*_tmpD02;(
_tmpD02=0,Cyc_Tcutil_terr(_tmp537->loc,((_tmpD03="cannot determine value of constant",
_tag_dyneither(_tmpD03,sizeof(char),35))),_tag_dyneither(_tmpD02,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp71C;++ i){struct _tuple15*_tmpD06;struct Cyc_List_List*
_tmpD05;es=((_tmpD05=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD05)),((
_tmpD05->hd=((_tmpD06=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD06)),((
_tmpD06->f1=0,((_tmpD06->f2=_tmp538,_tmpD06)))))),((_tmpD05->tl=es,_tmpD05))))));}}
if(_tmp539){struct Cyc_Absyn_Exp*_tmp723=Cyc_Toc_cast_it(_tmp71E,Cyc_Absyn_uint_exp(
0,0));struct _tuple15*_tmpD09;struct Cyc_List_List*_tmpD08;es=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpD08=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD08)),((_tmpD08->hd=((_tmpD09=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD09)),((_tmpD09->f1=0,((
_tmpD09->f2=_tmp723,_tmpD09)))))),((_tmpD08->tl=0,_tmpD08)))))));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL223;};};}_LL262: {struct Cyc_Absyn_Aggregate_e_struct*_tmp53A=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp4D2;if(_tmp53A->tag != 29)goto _LL264;
else{_tmp53B=_tmp53A->f1;_tmp53C=_tmp53A->f2;_tmp53D=_tmp53A->f3;_tmp53E=_tmp53A->f4;}}
_LL263: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp53C
!= 0,0,0,_tmp53D,_tmp53B))->r;else{if(_tmp53E == 0){const char*_tmpD0C;void*
_tmpD0B;(_tmpD0B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD0C="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpD0C,sizeof(
char),38))),_tag_dyneither(_tmpD0B,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp53E;struct Cyc_Toc_Env _tmp729;struct
_RegionHandle*_tmp72A;struct Cyc_Toc_Env*_tmp728=nv;_tmp729=*_tmp728;_tmp72A=
_tmp729.rgn;{struct Cyc_List_List*_tmp72B=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum 
Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp72A,e->loc,_tmp53D,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp53E->impl))->tagged){struct
_tuple20 _tmp72D;struct Cyc_Absyn_Aggrfield*_tmp72E;struct Cyc_Absyn_Exp*_tmp72F;
struct _tuple20*_tmp72C=(struct _tuple20*)((struct Cyc_List_List*)_check_null(
_tmp72B))->hd;_tmp72D=*_tmp72C;_tmp72E=_tmp72D.f1;_tmp72F=_tmp72D.f2;{void*
_tmp730=_tmp72E->type;Cyc_Toc_exp_to_c(nv,_tmp72F);if(Cyc_Toc_is_void_star_or_tvar(
_tmp730))_tmp72F->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp72F->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp53E,_tmp72E->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple15*_tmpD11;struct _tuple15*_tmpD10;struct _tuple15*_tmpD0F[2];
struct Cyc_List_List*_tmp731=(_tmpD0F[1]=((_tmpD10=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD10)),((_tmpD10->f1=0,((_tmpD10->f2=_tmp72F,_tmpD10)))))),((_tmpD0F[
0]=((_tmpD11=_cycalloc(sizeof(*_tmpD11)),((_tmpD11->f1=0,((_tmpD11->f2=
field_tag_exp,_tmpD11)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD0F,sizeof(struct _tuple15*),2)))));struct Cyc_Absyn_Exp*umem=
Cyc_Absyn_unresolvedmem_exp(0,_tmp731,0);struct Cyc_Absyn_FieldName_struct*
_tmpD17;struct Cyc_Absyn_FieldName_struct _tmpD16;void*_tmpD15[1];struct Cyc_List_List*
ds=(_tmpD15[0]=(void*)((_tmpD17=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpD17)),((_tmpD17[0]=((_tmpD16.tag=1,((_tmpD16.f1=_tmp72E->name,_tmpD16)))),
_tmpD17)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD15,sizeof(void*),1)));struct _tuple15*_tmpD1A;struct _tuple15*
_tmpD19[1];struct Cyc_List_List*dles=(_tmpD19[0]=((_tmpD1A=_cycalloc(sizeof(*
_tmpD1A)),((_tmpD1A->f1=ds,((_tmpD1A->f2=umem,_tmpD1A)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD19,sizeof(struct
_tuple15*),1)));e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{struct Cyc_List_List*
_tmp73A=0;struct Cyc_List_List*_tmp73B=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp73B != 0;_tmp73B=_tmp73B->tl){struct Cyc_List_List*
_tmp73C=_tmp72B;for(0;_tmp73C != 0;_tmp73C=_tmp73C->tl){if((*((struct _tuple20*)
_tmp73C->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp73B->hd){struct _tuple20
_tmp73E;struct Cyc_Absyn_Aggrfield*_tmp73F;struct Cyc_Absyn_Exp*_tmp740;struct
_tuple20*_tmp73D=(struct _tuple20*)_tmp73C->hd;_tmp73E=*_tmp73D;_tmp73F=_tmp73E.f1;
_tmp740=_tmp73E.f2;{void*_tmp741=_tmp73F->type;Cyc_Toc_exp_to_c(nv,_tmp740);if(
Cyc_Toc_is_void_star_or_tvar(_tmp741))_tmp740->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp740->r,0));{struct _tuple15*_tmpD1D;struct Cyc_List_List*
_tmpD1C;_tmp73A=((_tmpD1C=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD1C)),((
_tmpD1C->hd=((_tmpD1D=_cycalloc(sizeof(*_tmpD1D)),((_tmpD1D->f1=0,((_tmpD1D->f2=
_tmp740,_tmpD1D)))))),((_tmpD1C->tl=_tmp73A,_tmpD1C))))));}break;};}}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp73A));}};};}
goto _LL223;_LL264: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp53F=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp4D2;if(_tmp53F->tag != 30)goto _LL266;else{_tmp540=(void*)_tmp53F->f1;_tmp541=
_tmp53F->f2;}}_LL265: {struct Cyc_List_List*fs;{void*_tmp744=Cyc_Tcutil_compress(
_tmp540);struct Cyc_List_List*_tmp746;_LL328: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp745=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp744;if(_tmp745->tag != 13)goto
_LL32A;else{_tmp746=_tmp745->f2;}}_LL329: fs=_tmp746;goto _LL327;_LL32A:;_LL32B: {
const char*_tmpD21;void*_tmpD20[1];struct Cyc_String_pa_struct _tmpD1F;(_tmpD1F.tag=
0,((_tmpD1F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp540)),((_tmpD20[0]=& _tmpD1F,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD21="anon struct has type %s",
_tag_dyneither(_tmpD21,sizeof(char),24))),_tag_dyneither(_tmpD20,sizeof(void*),1)))))));}
_LL327:;}{struct Cyc_Toc_Env _tmp74B;struct _RegionHandle*_tmp74C;struct Cyc_Toc_Env*
_tmp74A=nv;_tmp74B=*_tmp74A;_tmp74C=_tmp74B.rgn;{struct Cyc_List_List*_tmp74D=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp74C,e->loc,_tmp541,Cyc_Absyn_StructA,fs);for(0;_tmp74D != 0;_tmp74D=_tmp74D->tl){
struct _tuple20 _tmp74F;struct Cyc_Absyn_Aggrfield*_tmp750;struct Cyc_Absyn_Exp*
_tmp751;struct _tuple20*_tmp74E=(struct _tuple20*)_tmp74D->hd;_tmp74F=*_tmp74E;
_tmp750=_tmp74F.f1;_tmp751=_tmp74F.f2;{void*_tmp752=_tmp750->type;Cyc_Toc_exp_to_c(
nv,_tmp751);if(Cyc_Toc_is_void_star_or_tvar(_tmp752))_tmp751->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp751->r,0));};}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp541);}goto _LL223;};}_LL266: {struct Cyc_Absyn_Datatype_e_struct*_tmp542=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp4D2;if(_tmp542->tag != 31)goto _LL268;else{
_tmp543=_tmp542->f1;_tmp544=_tmp542->f2;_tmp545=_tmp542->f3;}}_LL267: {void*
datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple0*_tmp753=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp754=Cyc_Absyn_var_exp(_tmp753,0);struct Cyc_Absyn_Exp*
member_exp;{const char*_tmpD22;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp545->name,((_tmpD22="_struct",_tag_dyneither(_tmpD22,sizeof(char),8)))));}
tag_exp=_tmp544->is_extensible?Cyc_Absyn_var_exp(_tmp545->name,0): Cyc_Toc_datatype_tag(
_tmp544,_tmp545->name);member_exp=_tmp754;{struct Cyc_List_List*_tmp756=_tmp545->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp543 != 0;(
_tmp543=_tmp543->tl,_tmp756=_tmp756->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp543->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp756))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct _tuple15*_tmpD25;struct
Cyc_List_List*_tmpD24;dles=((_tmpD24=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD24)),((_tmpD24->hd=((_tmpD25=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD25)),((_tmpD25->f1=0,((_tmpD25->f2=cur_e,_tmpD25)))))),((_tmpD24->tl=
dles,_tmpD24))))));};}{struct _tuple15*_tmpD28;struct Cyc_List_List*_tmpD27;dles=((
_tmpD27=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD27)),((_tmpD27->hd=((
_tmpD28=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD28)),((_tmpD28->f1=0,((
_tmpD28->f2=tag_exp,_tmpD28)))))),((_tmpD27->tl=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpD27))))));}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,dles);}else{struct Cyc_List_List*_tmpD29;struct Cyc_List_List*_tmp75B=(_tmpD29=
_cycalloc(sizeof(*_tmpD29)),((_tmpD29->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
member_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpD29->tl=0,_tmpD29)))));{int i=1;
for(0;_tmp543 != 0;(((_tmp543=_tmp543->tl,i ++)),_tmp756=_tmp756->tl)){struct Cyc_Absyn_Exp*
cur_e=(struct Cyc_Absyn_Exp*)_tmp543->hd;void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp756))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(field_typ))cur_e=Cyc_Toc_cast_it(
field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp75C=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
member_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);struct Cyc_List_List*_tmpD2A;
_tmp75B=((_tmpD2A=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD2A)),((
_tmpD2A->hd=_tmp75C,((_tmpD2A->tl=_tmp75B,_tmpD2A))))));};}}{struct Cyc_Absyn_Stmt*
_tmp75E=Cyc_Absyn_exp_stmt(_tmp754,0);struct Cyc_List_List*_tmpD2B;struct Cyc_Absyn_Stmt*
_tmp75F=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((
_tmpD2B=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD2B)),((_tmpD2B->hd=
_tmp75E,((_tmpD2B->tl=_tmp75B,_tmpD2B))))))),0);e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(
_tmp753,datatype_ctype,0,_tmp75F,0));};}goto _LL223;};}_LL268: {struct Cyc_Absyn_Enum_e_struct*
_tmp546=(struct Cyc_Absyn_Enum_e_struct*)_tmp4D2;if(_tmp546->tag != 32)goto _LL26A;}
_LL269: goto _LL26B;_LL26A: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp547=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp4D2;if(_tmp547->tag != 33)goto _LL26C;}_LL26B: goto _LL223;_LL26C: {struct Cyc_Absyn_Malloc_e_struct*
_tmp548=(struct Cyc_Absyn_Malloc_e_struct*)_tmp4D2;if(_tmp548->tag != 34)goto
_LL26E;else{_tmp549=_tmp548->f1;_tmp54A=_tmp549.is_calloc;_tmp54B=_tmp549.rgn;
_tmp54C=_tmp549.elt_type;_tmp54D=_tmp549.num_elts;_tmp54E=_tmp549.fat_result;}}
_LL26D: {void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp54C)));Cyc_Toc_exp_to_c(
nv,_tmp54D);if(_tmp54E){struct _tuple0*_tmp762=Cyc_Toc_temp_var();struct _tuple0*
_tmp763=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp54A){xexp=_tmp54D;if(_tmp54B != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp54B;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp762,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp54C,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp762,0));}{struct Cyc_Absyn_Exp*_tmpD2C[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpD2C[2]=Cyc_Absyn_var_exp(_tmp762,0),((_tmpD2C[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpD2C[0]=Cyc_Absyn_var_exp(_tmp763,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD2C,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{xexp=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp54D,0);if(_tmp54B != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp54B;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp762,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp54C,Cyc_Absyn_var_exp(_tmp762,0));}{struct Cyc_Absyn_Exp*_tmpD2D[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpD2D[2]=Cyc_Absyn_var_exp(_tmp762,0),((_tmpD2D[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpD2D[0]=Cyc_Absyn_var_exp(_tmp763,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD2D,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);};}{struct Cyc_Absyn_Stmt*_tmp766=Cyc_Absyn_declare_stmt(_tmp762,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp763,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp766);};}else{struct Cyc_Absyn_Exp*_tmp767=Cyc_Absyn_sizeoftyp_exp(
t_c,0);{void*_tmp768=_tmp54D->r;union Cyc_Absyn_Cnst _tmp76A;struct _tuple5 _tmp76B;
int _tmp76C;_LL32D: {struct Cyc_Absyn_Const_e_struct*_tmp769=(struct Cyc_Absyn_Const_e_struct*)
_tmp768;if(_tmp769->tag != 0)goto _LL32F;else{_tmp76A=_tmp769->f1;if((_tmp76A.Int_c).tag
!= 4)goto _LL32F;_tmp76B=(struct _tuple5)(_tmp76A.Int_c).val;_tmp76C=_tmp76B.f2;
if(_tmp76C != 1)goto _LL32F;}}_LL32E: goto _LL32C;_LL32F:;_LL330: _tmp767=Cyc_Absyn_times_exp(
_tmp767,_tmp54D,0);goto _LL32C;_LL32C:;}if(_tmp54B != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp54B;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rmalloc_exp(rgn,_tmp767))->r;}else{e->r=(Cyc_Toc_malloc_exp(*_tmp54C,
_tmp767))->r;}}goto _LL223;}_LL26E: {struct Cyc_Absyn_Swap_e_struct*_tmp54F=(
struct Cyc_Absyn_Swap_e_struct*)_tmp4D2;if(_tmp54F->tag != 35)goto _LL270;else{
_tmp550=_tmp54F->f1;_tmp551=_tmp54F->f2;}}_LL26F: {int is_dyneither_ptr=0;void*
e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp550->topt))->v;void*
e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp551->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(
e1_old_typ,& is_dyneither_ptr)){const char*_tmpD30;void*_tmpD2F;(_tmpD2F=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD30="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpD30,sizeof(char),57))),_tag_dyneither(_tmpD2F,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}{int f1_tag=0;void*tagged_mem_type1=(void*)&
Cyc_Absyn_VoidType_val;int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp550,&
f1_tag,& tagged_mem_type1,1);int f2_tag=0;void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp551,& f2_tag,& tagged_mem_type2,1);
Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp550);Cyc_Toc_exp_to_c(nv,_tmp551);
Cyc_Toc_set_lhs(nv,0);if(e1_tmem)Cyc_Toc_add_tagged_union_check_for_swap(_tmp550,
f1_tag,tagged_mem_type1);else{_tmp550=Cyc_Toc_push_address_exp(_tmp550);}if(
e2_tmem)Cyc_Toc_add_tagged_union_check_for_swap(_tmp551,f2_tag,tagged_mem_type2);
else{_tmp551=Cyc_Toc_push_address_exp(_tmp551);}{struct Cyc_Absyn_Exp*_tmpD31[2];
e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpD31[1]=_tmp551,((_tmpD31[0]=_tmp550,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD31,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL223;};};}_LL270: {struct
Cyc_Absyn_Tagcheck_e_struct*_tmp552=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp4D2;
if(_tmp552->tag != 38)goto _LL272;else{_tmp553=_tmp552->f1;_tmp554=_tmp552->f2;}}
_LL271: {void*_tmp770=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp553->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp553);{void*_tmp771=
_tmp770;struct Cyc_Absyn_AggrInfo _tmp773;union Cyc_Absyn_AggrInfoU _tmp774;struct
Cyc_Absyn_Aggrdecl**_tmp775;struct Cyc_Absyn_Aggrdecl*_tmp776;_LL332: {struct Cyc_Absyn_AggrType_struct*
_tmp772=(struct Cyc_Absyn_AggrType_struct*)_tmp771;if(_tmp772->tag != 12)goto
_LL334;else{_tmp773=_tmp772->f1;_tmp774=_tmp773.aggr_info;if((_tmp774.KnownAggr).tag
!= 2)goto _LL334;_tmp775=(struct Cyc_Absyn_Aggrdecl**)(_tmp774.KnownAggr).val;
_tmp776=*_tmp775;}}_LL333: {struct Cyc_Absyn_Exp*_tmp777=Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp776,_tmp554),0);struct Cyc_Absyn_Exp*_tmp778=Cyc_Absyn_aggrmember_exp(
_tmp553,_tmp554,0);struct Cyc_Absyn_Exp*_tmp779=Cyc_Absyn_aggrmember_exp(_tmp778,
Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(_tmp779,_tmp777,0))->r;goto _LL331;}
_LL334:;_LL335: {const char*_tmpD34;void*_tmpD33;(_tmpD33=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD34="non-aggregate type in tagcheck",
_tag_dyneither(_tmpD34,sizeof(char),31))),_tag_dyneither(_tmpD33,sizeof(void*),0)));}
_LL331:;}goto _LL223;}_LL272: {struct Cyc_Absyn_StmtExp_e_struct*_tmp555=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp4D2;if(_tmp555->tag != 37)goto _LL274;else{_tmp556=
_tmp555->f1;}}_LL273: Cyc_Toc_stmt_to_c(nv,_tmp556);goto _LL223;_LL274: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp557=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp4D2;if(_tmp557->tag != 36)
goto _LL276;}_LL275: {const char*_tmpD37;void*_tmpD36;(_tmpD36=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD37="UnresolvedMem",
_tag_dyneither(_tmpD37,sizeof(char),14))),_tag_dyneither(_tmpD36,sizeof(void*),0)));}
_LL276: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp558=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp4D2;if(_tmp558->tag != 26)goto _LL278;}_LL277: {const char*_tmpD3A;void*_tmpD39;(
_tmpD39=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpD3A="compoundlit",_tag_dyneither(_tmpD3A,sizeof(char),12))),_tag_dyneither(
_tmpD39,sizeof(void*),0)));}_LL278: {struct Cyc_Absyn_Valueof_e_struct*_tmp559=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp4D2;if(_tmp559->tag != 39)goto _LL27A;}
_LL279: {const char*_tmpD3D;void*_tmpD3C;(_tmpD3C=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD3D="valueof(-)",
_tag_dyneither(_tmpD3D,sizeof(char),11))),_tag_dyneither(_tmpD3C,sizeof(void*),0)));}
_LL27A: {struct Cyc_Absyn_Asm_e_struct*_tmp55A=(struct Cyc_Absyn_Asm_e_struct*)
_tmp4D2;if(_tmp55A->tag != 40)goto _LL223;}_LL27B: {const char*_tmpD40;void*_tmpD3F;(
_tmpD3F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD40="__asm__",_tag_dyneither(_tmpD40,sizeof(char),8))),_tag_dyneither(
_tmpD3F,sizeof(void*),0)));}_LL223:;};}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0);}struct _tuple21{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*
f3;};struct _tuple22{struct _tuple0*f1;void*f2;};struct _tuple23{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple21 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*
nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
struct Cyc_Absyn_Stmt*s;{void*_tmp784=p->r;struct Cyc_Absyn_Vardecl*_tmp786;struct
Cyc_Absyn_Vardecl _tmp787;struct _tuple0*_tmp788;struct Cyc_Absyn_Pat*_tmp789;
struct Cyc_Absyn_Vardecl*_tmp78B;struct Cyc_Absyn_Vardecl _tmp78C;struct _tuple0*
_tmp78D;struct Cyc_Absyn_Vardecl*_tmp790;struct Cyc_Absyn_Pat*_tmp791;enum Cyc_Absyn_Sign
_tmp794;int _tmp795;char _tmp797;struct _dyneither_ptr _tmp799;struct Cyc_Absyn_Enumdecl*
_tmp79B;struct Cyc_Absyn_Enumfield*_tmp79C;void*_tmp79E;struct Cyc_Absyn_Enumfield*
_tmp79F;struct Cyc_Absyn_Pat*_tmp7A1;struct Cyc_Absyn_Pat _tmp7A2;void*_tmp7A3;
struct Cyc_Absyn_Datatypedecl*_tmp7A5;struct Cyc_Absyn_Datatypefield*_tmp7A6;
struct Cyc_List_List*_tmp7A7;struct Cyc_List_List*_tmp7A9;struct Cyc_List_List*
_tmp7AB;struct Cyc_Absyn_AggrInfo*_tmp7AD;struct Cyc_Absyn_AggrInfo*_tmp7AF;struct
Cyc_Absyn_AggrInfo _tmp7B0;union Cyc_Absyn_AggrInfoU _tmp7B1;struct Cyc_List_List*
_tmp7B2;struct Cyc_Absyn_Pat*_tmp7B4;_LL337: {struct Cyc_Absyn_Var_p_struct*
_tmp785=(struct Cyc_Absyn_Var_p_struct*)_tmp784;if(_tmp785->tag != 1)goto _LL339;
else{_tmp786=_tmp785->f1;_tmp787=*_tmp786;_tmp788=_tmp787.name;_tmp789=_tmp785->f2;}}
_LL338: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp788,r),rgn,t,r,path,
_tmp789,fail_stmt,decls);_LL339: {struct Cyc_Absyn_TagInt_p_struct*_tmp78A=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp784;if(_tmp78A->tag != 3)goto _LL33B;else{
_tmp78B=_tmp78A->f2;_tmp78C=*_tmp78B;_tmp78D=_tmp78C.name;}}_LL33A: nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp78D,r);goto _LL33C;_LL33B: {struct Cyc_Absyn_Wild_p_struct*_tmp78E=(
struct Cyc_Absyn_Wild_p_struct*)_tmp784;if(_tmp78E->tag != 0)goto _LL33D;}_LL33C: s=
Cyc_Toc_skip_stmt_dl();goto _LL336;_LL33D: {struct Cyc_Absyn_Reference_p_struct*
_tmp78F=(struct Cyc_Absyn_Reference_p_struct*)_tmp784;if(_tmp78F->tag != 2)goto
_LL33F;else{_tmp790=_tmp78F->f1;_tmp791=_tmp78F->f2;}}_LL33E: {struct _tuple0*
_tmp7B8=Cyc_Toc_temp_var();{struct _tuple22*_tmpD43;struct Cyc_List_List*_tmpD42;
decls=((_tmpD42=_region_malloc(rgn,sizeof(*_tmpD42)),((_tmpD42->hd=((_tmpD43=
_region_malloc(rgn,sizeof(*_tmpD43)),((_tmpD43->f1=_tmp7B8,((_tmpD43->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpD43)))))),((_tmpD42->tl=decls,_tmpD42))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp790->name,Cyc_Absyn_var_exp(_tmp7B8,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp7B8,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Toc_push_address_exp(path)),0);{struct _tuple21 _tmp7BB=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp791,fail_stmt,decls);_tmp7BB.f3=Cyc_Absyn_seq_stmt(s,_tmp7BB.f3,
0);return _tmp7BB;};}_LL33F: {struct Cyc_Absyn_Null_p_struct*_tmp792=(struct Cyc_Absyn_Null_p_struct*)
_tmp784;if(_tmp792->tag != 8)goto _LL341;}_LL340: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),fail_stmt);goto _LL336;_LL341: {struct Cyc_Absyn_Int_p_struct*_tmp793=(struct
Cyc_Absyn_Int_p_struct*)_tmp784;if(_tmp793->tag != 9)goto _LL343;else{_tmp794=
_tmp793->f1;_tmp795=_tmp793->f2;}}_LL342: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp794,_tmp795,0),fail_stmt);goto _LL336;_LL343: {struct Cyc_Absyn_Char_p_struct*
_tmp796=(struct Cyc_Absyn_Char_p_struct*)_tmp784;if(_tmp796->tag != 10)goto _LL345;
else{_tmp797=_tmp796->f1;}}_LL344: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(
_tmp797,0),fail_stmt);goto _LL336;_LL345: {struct Cyc_Absyn_Float_p_struct*_tmp798=(
struct Cyc_Absyn_Float_p_struct*)_tmp784;if(_tmp798->tag != 11)goto _LL347;else{
_tmp799=_tmp798->f1;}}_LL346: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp799,
0),fail_stmt);goto _LL336;_LL347: {struct Cyc_Absyn_Enum_p_struct*_tmp79A=(struct
Cyc_Absyn_Enum_p_struct*)_tmp784;if(_tmp79A->tag != 12)goto _LL349;else{_tmp79B=
_tmp79A->f1;_tmp79C=_tmp79A->f2;}}_LL348:{struct Cyc_Absyn_Enum_e_struct _tmpD46;
struct Cyc_Absyn_Enum_e_struct*_tmpD45;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD45=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD45)),((
_tmpD45[0]=((_tmpD46.tag=32,((_tmpD46.f1=_tmp79C->name,((_tmpD46.f2=(struct Cyc_Absyn_Enumdecl*)
_tmp79B,((_tmpD46.f3=(struct Cyc_Absyn_Enumfield*)_tmp79C,_tmpD46)))))))),_tmpD45)))),
0),fail_stmt);}goto _LL336;_LL349: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp79D=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp784;if(_tmp79D->tag != 13)goto _LL34B;else{
_tmp79E=(void*)_tmp79D->f1;_tmp79F=_tmp79D->f2;}}_LL34A:{struct Cyc_Absyn_AnonEnum_e_struct
_tmpD49;struct Cyc_Absyn_AnonEnum_e_struct*_tmpD48;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD48=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD48)),((
_tmpD48[0]=((_tmpD49.tag=33,((_tmpD49.f1=_tmp79F->name,((_tmpD49.f2=(void*)
_tmp79E,((_tmpD49.f3=(struct Cyc_Absyn_Enumfield*)_tmp79F,_tmpD49)))))))),_tmpD48)))),
0),fail_stmt);}goto _LL336;_LL34B: {struct Cyc_Absyn_Pointer_p_struct*_tmp7A0=(
struct Cyc_Absyn_Pointer_p_struct*)_tmp784;if(_tmp7A0->tag != 5)goto _LL34D;else{
_tmp7A1=_tmp7A0->f1;_tmp7A2=*_tmp7A1;_tmp7A3=_tmp7A2.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp7A4=(struct Cyc_Absyn_Datatype_p_struct*)_tmp7A3;if(_tmp7A4->tag != 7)goto
_LL34D;else{_tmp7A5=_tmp7A4->f1;_tmp7A6=_tmp7A4->f2;_tmp7A7=_tmp7A4->f3;}};}}
_LL34C: s=Cyc_Toc_skip_stmt_dl();{struct _tuple0*_tmp7C0=Cyc_Toc_temp_var();const
char*_tmpD4A;struct _tuple0*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp7A6->name,((
_tmpD4A="_struct",_tag_dyneither(_tmpD4A,sizeof(char),8))));void*_tmp7C1=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);int cnt=1;struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
_tmp7C1,r);struct Cyc_List_List*_tmp7C2=_tmp7A6->typs;for(0;_tmp7A7 != 0;(((
_tmp7A7=_tmp7A7->tl,_tmp7C2=((struct Cyc_List_List*)_check_null(_tmp7C2))->tl)),
++ cnt)){struct Cyc_Absyn_Pat*_tmp7C3=(struct Cyc_Absyn_Pat*)_tmp7A7->hd;if(_tmp7C3->r
== (void*)& Cyc_Absyn_Wild_p_val)continue;{void*_tmp7C4=(*((struct _tuple9*)((
struct Cyc_List_List*)_check_null(_tmp7C2))->hd)).f2;struct _tuple0*_tmp7C5=Cyc_Toc_temp_var();
void*_tmp7C6=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp7C3->topt))->v;void*
_tmp7C7=Cyc_Toc_typ_to_c(_tmp7C6);struct Cyc_Absyn_Exp*_tmp7C8=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp7C0,0),Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_tvar(
Cyc_Toc_typ_to_c(_tmp7C4)))_tmp7C8=Cyc_Toc_cast_it(_tmp7C7,_tmp7C8);{struct
_tuple22*_tmpD4D;struct Cyc_List_List*_tmpD4C;decls=((_tmpD4C=_region_malloc(rgn,
sizeof(*_tmpD4C)),((_tmpD4C->hd=((_tmpD4D=_region_malloc(rgn,sizeof(*_tmpD4D)),((
_tmpD4D->f1=_tmp7C5,((_tmpD4D->f2=_tmp7C7,_tmpD4D)))))),((_tmpD4C->tl=decls,
_tmpD4C))))));}{struct _tuple21 _tmp7CB=Cyc_Toc_xlate_pat(nv,rgn,_tmp7C6,Cyc_Absyn_var_exp(
_tmp7C5,0),_tmp7C8,_tmp7C3,fail_stmt,decls);nv=_tmp7CB.f1;decls=_tmp7CB.f2;{
struct Cyc_Absyn_Stmt*_tmp7CC=_tmp7CB.f3;struct Cyc_Absyn_Stmt*_tmp7CD=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp7C5,0),_tmp7C8,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp7CD,_tmp7CC,0),0);};};};}{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp7C0,0),Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*tag_exp=
_tmp7A5->is_extensible?Cyc_Absyn_var_exp(_tmp7A6->name,0): Cyc_Toc_datatype_tag(
_tmp7A5,_tmp7A6->name);struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,
tag_exp,0);s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);if(Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v))s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp7C0,0),Cyc_Absyn_uint_exp(0,0),0),
fail_stmt,s,0);s=Cyc_Absyn_declare_stmt(_tmp7C0,_tmp7C1,(struct Cyc_Absyn_Exp*)
rcast,s,0);goto _LL336;};};_LL34D: {struct Cyc_Absyn_Datatype_p_struct*_tmp7A8=(
struct Cyc_Absyn_Datatype_p_struct*)_tmp784;if(_tmp7A8->tag != 7)goto _LL34F;else{
_tmp7A9=_tmp7A8->f3;}}_LL34E: _tmp7AB=_tmp7A9;goto _LL350;_LL34F: {struct Cyc_Absyn_Tuple_p_struct*
_tmp7AA=(struct Cyc_Absyn_Tuple_p_struct*)_tmp784;if(_tmp7AA->tag != 4)goto _LL351;
else{_tmp7AB=_tmp7AA->f1;}}_LL350: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;
_tmp7AB != 0;(_tmp7AB=_tmp7AB->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp7CF=(struct Cyc_Absyn_Pat*)
_tmp7AB->hd;if(_tmp7CF->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{struct _tuple0*
_tmp7D0=Cyc_Toc_temp_var();void*_tmp7D1=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7CF->topt))->v;{struct _tuple22*_tmpD50;struct Cyc_List_List*_tmpD4F;decls=((
_tmpD4F=_region_malloc(rgn,sizeof(*_tmpD4F)),((_tmpD4F->hd=((_tmpD50=
_region_malloc(rgn,sizeof(*_tmpD50)),((_tmpD50->f1=_tmp7D0,((_tmpD50->f2=Cyc_Toc_typ_to_c(
_tmp7D1),_tmpD50)))))),((_tmpD4F->tl=decls,_tmpD4F))))));}{struct _tuple21 _tmp7D4=
Cyc_Toc_xlate_pat(nv,rgn,_tmp7D1,Cyc_Absyn_var_exp(_tmp7D0,0),Cyc_Absyn_aggrmember_exp(
path,Cyc_Absyn_fieldname(cnt),0),_tmp7CF,fail_stmt,decls);nv=_tmp7D4.f1;decls=
_tmp7D4.f2;{struct Cyc_Absyn_Stmt*_tmp7D5=_tmp7D4.f3;struct Cyc_Absyn_Stmt*_tmp7D6=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7D0,0),Cyc_Absyn_aggrmember_exp(r,
Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7D6,
_tmp7D5,0),0);};};};}goto _LL336;};_LL351: {struct Cyc_Absyn_Aggr_p_struct*_tmp7AC=(
struct Cyc_Absyn_Aggr_p_struct*)_tmp784;if(_tmp7AC->tag != 6)goto _LL353;else{
_tmp7AD=_tmp7AC->f1;if(_tmp7AD != 0)goto _LL353;}}_LL352: {const char*_tmpD53;void*
_tmpD52;(_tmpD52=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD53="unresolved aggregate pattern!",_tag_dyneither(_tmpD53,sizeof(char),30))),
_tag_dyneither(_tmpD52,sizeof(void*),0)));}_LL353: {struct Cyc_Absyn_Aggr_p_struct*
_tmp7AE=(struct Cyc_Absyn_Aggr_p_struct*)_tmp784;if(_tmp7AE->tag != 6)goto _LL355;
else{_tmp7AF=_tmp7AE->f1;if(_tmp7AF == 0)goto _LL355;_tmp7B0=*_tmp7AF;_tmp7B1=
_tmp7B0.aggr_info;_tmp7B2=_tmp7AE->f3;}}_LL354: {struct Cyc_Absyn_Aggrdecl*
_tmp7D9=Cyc_Absyn_get_known_aggrdecl(_tmp7B1);if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp7D9->impl))->tagged){struct _tuple23 _tmp7DB;struct Cyc_List_List*
_tmp7DC;struct Cyc_Absyn_Pat*_tmp7DD;struct _tuple23*_tmp7DA=(struct _tuple23*)((
struct Cyc_List_List*)_check_null(_tmp7B2))->hd;_tmp7DB=*_tmp7DA;_tmp7DC=_tmp7DB.f1;
_tmp7DD=_tmp7DB.f2;{struct _dyneither_ptr*f;{void*_tmp7DE=(void*)((struct Cyc_List_List*)
_check_null(_tmp7DC))->hd;struct _dyneither_ptr*_tmp7E0;_LL35E: {struct Cyc_Absyn_FieldName_struct*
_tmp7DF=(struct Cyc_Absyn_FieldName_struct*)_tmp7DE;if(_tmp7DF->tag != 1)goto
_LL360;else{_tmp7E0=_tmp7DF->f1;}}_LL35F: f=_tmp7E0;goto _LL35D;_LL360:;_LL361: {
const char*_tmpD56;void*_tmpD55;(_tmpD55=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD56="no field name in tagged union pattern",
_tag_dyneither(_tmpD56,sizeof(char),38))),_tag_dyneither(_tmpD55,sizeof(void*),0)));}
_LL35D:;}{struct _tuple0*_tmp7E3=Cyc_Toc_temp_var();void*_tmp7E4=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp7DD->topt))->v;void*_tmp7E5=Cyc_Toc_typ_to_c(
_tmp7E4);{struct _tuple22*_tmpD59;struct Cyc_List_List*_tmpD58;decls=((_tmpD58=
_region_malloc(rgn,sizeof(*_tmpD58)),((_tmpD58->hd=((_tmpD59=_region_malloc(rgn,
sizeof(*_tmpD59)),((_tmpD59->f1=_tmp7E3,((_tmpD59->f2=_tmp7E5,_tmpD59)))))),((
_tmpD58->tl=decls,_tmpD58))))));}{struct Cyc_Absyn_Exp*_tmp7E8=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp7E9=
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp7E9=Cyc_Toc_cast_it(_tmp7E5,_tmp7E9);_tmp7E8=Cyc_Toc_cast_it(_tmp7E5,_tmp7E8);{
struct _tuple21 _tmp7EA=Cyc_Toc_xlate_pat(nv,rgn,_tmp7E4,Cyc_Absyn_var_exp(_tmp7E3,
0),_tmp7E8,_tmp7DD,fail_stmt,decls);nv=_tmp7EA.f1;decls=_tmp7EA.f2;{struct Cyc_Absyn_Stmt*
_tmp7EB=_tmp7EA.f3;struct Cyc_Absyn_Stmt*_tmp7EC=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp7D9,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp7ED=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp7E3,0),_tmp7E9,0);s=Cyc_Absyn_seq_stmt(_tmp7EC,Cyc_Absyn_seq_stmt(_tmp7ED,
_tmp7EB,0),0);};};};};};}else{s=Cyc_Toc_skip_stmt_dl();for(0;_tmp7B2 != 0;_tmp7B2=
_tmp7B2->tl){struct _tuple23*_tmp7EE=(struct _tuple23*)_tmp7B2->hd;struct Cyc_Absyn_Pat*
_tmp7EF=(*_tmp7EE).f2;if(_tmp7EF->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _dyneither_ptr*f;{void*_tmp7F0=(void*)((struct Cyc_List_List*)_check_null((*
_tmp7EE).f1))->hd;struct _dyneither_ptr*_tmp7F2;_LL363: {struct Cyc_Absyn_FieldName_struct*
_tmp7F1=(struct Cyc_Absyn_FieldName_struct*)_tmp7F0;if(_tmp7F1->tag != 1)goto
_LL365;else{_tmp7F2=_tmp7F1->f1;}}_LL364: f=_tmp7F2;goto _LL362;_LL365:;_LL366: {
struct Cyc_Toc_Match_error_struct _tmpD5C;struct Cyc_Toc_Match_error_struct*_tmpD5B;(
int)_throw((void*)((_tmpD5B=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpD5B)),((_tmpD5B[0]=((_tmpD5C.tag=Cyc_Toc_Match_error,_tmpD5C)),_tmpD5B)))));}
_LL362:;}{struct _tuple0*_tmp7F5=Cyc_Toc_temp_var();void*_tmp7F6=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp7EF->topt))->v;void*_tmp7F7=Cyc_Toc_typ_to_c(
_tmp7F6);{struct _tuple22*_tmpD5F;struct Cyc_List_List*_tmpD5E;decls=((_tmpD5E=
_region_malloc(rgn,sizeof(*_tmpD5E)),((_tmpD5E->hd=((_tmpD5F=_region_malloc(rgn,
sizeof(*_tmpD5F)),((_tmpD5F->f1=_tmp7F5,((_tmpD5F->f2=_tmp7F7,_tmpD5F)))))),((
_tmpD5E->tl=decls,_tmpD5E))))));}{struct _tuple21 _tmp7FA=Cyc_Toc_xlate_pat(nv,rgn,
_tmp7F6,Cyc_Absyn_var_exp(_tmp7F5,0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp7EF,
fail_stmt,decls);nv=_tmp7FA.f1;decls=_tmp7FA.f2;{struct Cyc_Absyn_Exp*_tmp7FB=Cyc_Absyn_aggrmember_exp(
r,f,0);if(Cyc_Toc_is_void_star_or_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(
Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7D9->impl))->fields,
f)))->type))_tmp7FB=Cyc_Toc_cast_it(_tmp7F7,_tmp7FB);{struct Cyc_Absyn_Stmt*
_tmp7FC=_tmp7FA.f3;struct Cyc_Absyn_Stmt*_tmp7FD=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp7F5,0),_tmp7FB,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7FD,_tmp7FC,
0),0);};};};};};}}goto _LL336;}_LL355: {struct Cyc_Absyn_Pointer_p_struct*_tmp7B3=(
struct Cyc_Absyn_Pointer_p_struct*)_tmp784;if(_tmp7B3->tag != 5)goto _LL357;else{
_tmp7B4=_tmp7B3->f1;}}_LL356: {struct _tuple0*_tmp7FE=Cyc_Toc_temp_var();void*
_tmp7FF=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp7B4->topt))->v;{struct
_tuple22*_tmpD62;struct Cyc_List_List*_tmpD61;decls=((_tmpD61=_region_malloc(rgn,
sizeof(*_tmpD61)),((_tmpD61->hd=((_tmpD62=_region_malloc(rgn,sizeof(*_tmpD62)),((
_tmpD62->f1=_tmp7FE,((_tmpD62->f2=Cyc_Toc_typ_to_c(_tmp7FF),_tmpD62)))))),((
_tmpD61->tl=decls,_tmpD61))))));}{struct _tuple21 _tmp802=Cyc_Toc_xlate_pat(nv,rgn,
_tmp7FF,Cyc_Absyn_var_exp(_tmp7FE,0),Cyc_Absyn_deref_exp(path,0),_tmp7B4,
fail_stmt,decls);nv=_tmp802.f1;decls=_tmp802.f2;{struct Cyc_Absyn_Stmt*_tmp803=
_tmp802.f3;struct Cyc_Absyn_Stmt*_tmp804=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp7FE,0),Cyc_Absyn_deref_exp(r,0),0),_tmp803,0);if(Cyc_Toc_is_nullable(
t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(
0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),_tmp804,0);else{s=_tmp804;}goto
_LL336;};};}_LL357: {struct Cyc_Absyn_UnknownId_p_struct*_tmp7B5=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp784;if(_tmp7B5->tag != 14)goto _LL359;}_LL358: {const char*_tmpD65;void*_tmpD64;(
_tmpD64=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD65="unknownid pat",_tag_dyneither(_tmpD65,sizeof(char),14))),_tag_dyneither(
_tmpD64,sizeof(void*),0)));}_LL359: {struct Cyc_Absyn_UnknownCall_p_struct*
_tmp7B6=(struct Cyc_Absyn_UnknownCall_p_struct*)_tmp784;if(_tmp7B6->tag != 15)goto
_LL35B;}_LL35A: {const char*_tmpD68;void*_tmpD67;(_tmpD67=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD68="unknowncall pat",
_tag_dyneither(_tmpD68,sizeof(char),16))),_tag_dyneither(_tmpD67,sizeof(void*),0)));}
_LL35B: {struct Cyc_Absyn_Exp_p_struct*_tmp7B7=(struct Cyc_Absyn_Exp_p_struct*)
_tmp784;if(_tmp7B7->tag != 16)goto _LL336;}_LL35C: {const char*_tmpD6B;void*_tmpD6A;(
_tmpD6A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD6B="exp pat",_tag_dyneither(_tmpD6B,sizeof(char),8))),_tag_dyneither(
_tmpD6A,sizeof(void*),0)));}_LL336:;}{struct _tuple21 _tmpD6C;return(_tmpD6C.f1=nv,((
_tmpD6C.f2=decls,((_tmpD6C.f3=s,_tmpD6C)))));};}struct _tuple24{struct
_dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
static struct _tuple24*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){struct _tuple24*_tmpD6D;return(_tmpD6D=_region_malloc(r,sizeof(*_tmpD6D)),((
_tmpD6D->f1=Cyc_Toc_fresh_label(),((_tmpD6D->f2=Cyc_Toc_fresh_label(),((_tmpD6D->f3=
sc,_tmpD6D)))))));}static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{
void*_tmp80D=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int
leave_as_switch;{void*_tmp80E=Cyc_Tcutil_compress(_tmp80D);_LL368: {struct Cyc_Absyn_IntType_struct*
_tmp80F=(struct Cyc_Absyn_IntType_struct*)_tmp80E;if(_tmp80F->tag != 6)goto _LL36A;}
_LL369: goto _LL36B;_LL36A: {struct Cyc_Absyn_EnumType_struct*_tmp810=(struct Cyc_Absyn_EnumType_struct*)
_tmp80E;if(_tmp810->tag != 14)goto _LL36C;}_LL36B: leave_as_switch=1;goto _LL367;
_LL36C:;_LL36D: leave_as_switch=0;goto _LL367;_LL367:;}{struct Cyc_List_List*
_tmp811=scs;for(0;_tmp811 != 0;_tmp811=_tmp811->tl){if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp811->hd)->pat_vars))->v
!= 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp811->hd)->where_clause != 0){
leave_as_switch=0;break;}}}if(leave_as_switch){struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp812=scs;for(0;_tmp812 != 0;_tmp812=_tmp812->tl){struct Cyc_Absyn_Stmt*
_tmp813=((struct Cyc_Absyn_Switch_clause*)_tmp812->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp812->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp813,0);next_l=Cyc_Toc_fresh_label();{
struct Cyc_Toc_Env _tmp815;struct _RegionHandle*_tmp816;struct Cyc_Toc_Env*_tmp814=
nv;_tmp815=*_tmp814;_tmp816=_tmp815.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
_tmp816,nv,next_l),_tmp813);};}}return;}{struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp818;
struct _RegionHandle*_tmp819;struct Cyc_Toc_Env*_tmp817=nv;_tmp818=*_tmp817;
_tmp819=_tmp818.rgn;{struct Cyc_Toc_Env*_tmp81A=Cyc_Toc_share_env(_tmp819,nv);
struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple24*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp819,Cyc_Toc_gen_label,
_tmp819,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct
Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp81B=lscs;for(0;_tmp81B != 0;
_tmp81B=_tmp81B->tl){struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple24*)
_tmp81B->hd)).f3;struct _dyneither_ptr*fail_lab=_tmp81B->tl == 0?end_l:(*((struct
_tuple24*)((struct Cyc_List_List*)_check_null(_tmp81B->tl))->hd)).f1;struct Cyc_Toc_Env*
_tmp81D;struct Cyc_List_List*_tmp81E;struct Cyc_Absyn_Stmt*_tmp81F;struct _tuple21
_tmp81C=Cyc_Toc_xlate_pat(_tmp81A,_tmp819,_tmp80D,r,path,sc->pattern,Cyc_Absyn_goto_stmt(
fail_lab,0),decls);_tmp81D=_tmp81C.f1;_tmp81E=_tmp81C.f2;_tmp81F=_tmp81C.f3;if(
sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp820=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp81D,_tmp820);_tmp81F=Cyc_Absyn_seq_stmt(
_tmp81F,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp820,0),
Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp81E;{
struct Cyc_List_List*_tmpD6E;nvs=((_tmpD6E=_region_malloc(_tmp819,sizeof(*_tmpD6E)),((
_tmpD6E->hd=_tmp81D,((_tmpD6E->tl=nvs,_tmpD6E))))));}{struct Cyc_List_List*
_tmpD6F;test_stmts=((_tmpD6F=_region_malloc(_tmp819,sizeof(*_tmpD6F)),((_tmpD6F->hd=
_tmp81F,((_tmpD6F->tl=test_stmts,_tmpD6F))))));};}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple24 _tmp824;struct _dyneither_ptr*_tmp825;struct _dyneither_ptr*_tmp826;
struct Cyc_Absyn_Switch_clause*_tmp827;struct _tuple24*_tmp823=(struct _tuple24*)
lscs->hd;_tmp824=*_tmp823;_tmp825=_tmp824.f1;_tmp826=_tmp824.f2;_tmp827=_tmp824.f3;{
struct Cyc_Toc_Env*_tmp828=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp827->body;struct Cyc_Toc_Env _tmp82A;struct
_RegionHandle*_tmp82B;struct Cyc_Toc_Env*_tmp829=_tmp81A;_tmp82A=*_tmp829;_tmp82B=
_tmp82A.rgn;if(lscs->tl != 0){struct _tuple24 _tmp82D;struct _dyneither_ptr*_tmp82E;
struct Cyc_Absyn_Switch_clause*_tmp82F;struct _tuple24*_tmp82C=(struct _tuple24*)((
struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp82D=*_tmp82C;_tmp82E=_tmp82D.f2;
_tmp82F=_tmp82D.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp82B,
_tmp828,end_l,_tmp82E,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp82F->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp82B,
_tmp828,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp825,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp826,s,0),0);{struct Cyc_List_List*_tmpD70;stmts=((_tmpD70=_region_malloc(
_tmp819,sizeof(*_tmpD70)),((_tmpD70->hd=s,((_tmpD70->tl=stmts,_tmpD70))))));};};}{
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,
Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple22
_tmp832;struct _tuple0*_tmp833;void*_tmp834;struct _tuple22*_tmp831=(struct
_tuple22*)decls->hd;_tmp832=*_tmp831;_tmp833=_tmp832.f1;_tmp834=_tmp832.f2;res=
Cyc_Absyn_declare_stmt(_tmp833,_tmp834,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(
v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(
struct Cyc_Absyn_Exp*)e,res,0))->r;};};};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){struct Cyc_List_List*
_tmpD71;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,((
_tmpD71=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD71)),((_tmpD71->hd=
Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpD71->tl=0,_tmpD71)))))),0),0);}
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(
Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){while(1){void*_tmp836=s->r;struct
Cyc_Absyn_Exp*_tmp839;struct Cyc_Absyn_Stmt*_tmp83B;struct Cyc_Absyn_Stmt*_tmp83C;
struct Cyc_Absyn_Exp*_tmp83E;struct Cyc_Absyn_Exp*_tmp840;struct Cyc_Absyn_Stmt*
_tmp841;struct Cyc_Absyn_Stmt*_tmp842;struct _tuple8 _tmp844;struct Cyc_Absyn_Exp*
_tmp845;struct Cyc_Absyn_Stmt*_tmp846;struct Cyc_Absyn_Stmt*_tmp848;struct Cyc_Absyn_Stmt*
_tmp84A;struct Cyc_Absyn_Stmt*_tmp84C;struct Cyc_Absyn_Exp*_tmp84E;struct _tuple8
_tmp84F;struct Cyc_Absyn_Exp*_tmp850;struct _tuple8 _tmp851;struct Cyc_Absyn_Exp*
_tmp852;struct Cyc_Absyn_Stmt*_tmp853;struct Cyc_Absyn_Exp*_tmp855;struct Cyc_List_List*
_tmp856;struct Cyc_List_List*_tmp858;struct Cyc_Absyn_Switch_clause**_tmp859;
struct Cyc_Absyn_Decl*_tmp85B;struct Cyc_Absyn_Stmt*_tmp85C;struct _dyneither_ptr*
_tmp85E;struct Cyc_Absyn_Stmt*_tmp85F;struct Cyc_Absyn_Stmt*_tmp861;struct _tuple8
_tmp862;struct Cyc_Absyn_Exp*_tmp863;struct Cyc_Absyn_Stmt*_tmp865;struct Cyc_List_List*
_tmp866;struct Cyc_Absyn_Exp*_tmp868;_LL36F: {struct Cyc_Absyn_Skip_s_struct*
_tmp837=(struct Cyc_Absyn_Skip_s_struct*)_tmp836;if(_tmp837->tag != 0)goto _LL371;}
_LL370: return;_LL371: {struct Cyc_Absyn_Exp_s_struct*_tmp838=(struct Cyc_Absyn_Exp_s_struct*)
_tmp836;if(_tmp838->tag != 1)goto _LL373;else{_tmp839=_tmp838->f1;}}_LL372: Cyc_Toc_exp_to_c(
nv,_tmp839);return;_LL373: {struct Cyc_Absyn_Seq_s_struct*_tmp83A=(struct Cyc_Absyn_Seq_s_struct*)
_tmp836;if(_tmp83A->tag != 2)goto _LL375;else{_tmp83B=_tmp83A->f1;_tmp83C=_tmp83A->f2;}}
_LL374: Cyc_Toc_stmt_to_c(nv,_tmp83B);s=_tmp83C;continue;_LL375: {struct Cyc_Absyn_Return_s_struct*
_tmp83D=(struct Cyc_Absyn_Return_s_struct*)_tmp836;if(_tmp83D->tag != 3)goto _LL377;
else{_tmp83E=_tmp83D->f1;}}_LL376: {struct Cyc_Core_Opt*topt=0;if(_tmp83E != 0){{
struct Cyc_Core_Opt*_tmpD72;topt=((_tmpD72=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD72)),((_tmpD72->v=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp83E->topt))->v),_tmpD72))));}Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp83E);}if(s->try_depth > 0){if(topt != 0){struct _tuple0*_tmp86A=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp86B=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(
_tmp86A,0),0);s->r=(Cyc_Absyn_declare_stmt(_tmp86A,(void*)topt->v,_tmp83E,Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler(s->try_depth),_tmp86B,0),0))->r;}else{Cyc_Toc_do_npop_before(
s->try_depth,s);}}return;}_LL377: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp83F=(
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp836;if(_tmp83F->tag != 4)goto _LL379;
else{_tmp840=_tmp83F->f1;_tmp841=_tmp83F->f2;_tmp842=_tmp83F->f3;}}_LL378: Cyc_Toc_exp_to_c(
nv,_tmp840);Cyc_Toc_stmt_to_c(nv,_tmp841);s=_tmp842;continue;_LL379: {struct Cyc_Absyn_While_s_struct*
_tmp843=(struct Cyc_Absyn_While_s_struct*)_tmp836;if(_tmp843->tag != 5)goto _LL37B;
else{_tmp844=_tmp843->f1;_tmp845=_tmp844.f1;_tmp846=_tmp843->f2;}}_LL37A: Cyc_Toc_exp_to_c(
nv,_tmp845);{struct Cyc_Toc_Env _tmp86D;struct _RegionHandle*_tmp86E;struct Cyc_Toc_Env*
_tmp86C=nv;_tmp86D=*_tmp86C;_tmp86E=_tmp86D.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp86E,nv),_tmp846);return;};_LL37B: {struct Cyc_Absyn_Break_s_struct*_tmp847=(
struct Cyc_Absyn_Break_s_struct*)_tmp836;if(_tmp847->tag != 6)goto _LL37D;else{
_tmp848=_tmp847->f1;}}_LL37C: {struct Cyc_Toc_Env _tmp870;struct _dyneither_ptr**
_tmp871;struct Cyc_Toc_Env*_tmp86F=nv;_tmp870=*_tmp86F;_tmp871=_tmp870.break_lab;
if(_tmp871 != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp871,0);{int dest_depth=_tmp848 == 0?0:
_tmp848->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;};}
_LL37D: {struct Cyc_Absyn_Continue_s_struct*_tmp849=(struct Cyc_Absyn_Continue_s_struct*)
_tmp836;if(_tmp849->tag != 7)goto _LL37F;else{_tmp84A=_tmp849->f1;}}_LL37E: {
struct Cyc_Toc_Env _tmp873;struct _dyneither_ptr**_tmp874;struct Cyc_Toc_Env*_tmp872=
nv;_tmp873=*_tmp872;_tmp874=_tmp873.continue_lab;if(_tmp874 != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp874,0);_tmp84C=_tmp84A;goto _LL380;}_LL37F: {struct Cyc_Absyn_Goto_s_struct*
_tmp84B=(struct Cyc_Absyn_Goto_s_struct*)_tmp836;if(_tmp84B->tag != 8)goto _LL381;
else{_tmp84C=_tmp84B->f2;}}_LL380: Cyc_Toc_do_npop_before(s->try_depth - ((struct
Cyc_Absyn_Stmt*)_check_null(_tmp84C))->try_depth,s);return;_LL381: {struct Cyc_Absyn_For_s_struct*
_tmp84D=(struct Cyc_Absyn_For_s_struct*)_tmp836;if(_tmp84D->tag != 9)goto _LL383;
else{_tmp84E=_tmp84D->f1;_tmp84F=_tmp84D->f2;_tmp850=_tmp84F.f1;_tmp851=_tmp84D->f3;
_tmp852=_tmp851.f1;_tmp853=_tmp84D->f4;}}_LL382: Cyc_Toc_exp_to_c(nv,_tmp84E);Cyc_Toc_exp_to_c(
nv,_tmp850);Cyc_Toc_exp_to_c(nv,_tmp852);{struct Cyc_Toc_Env _tmp876;struct
_RegionHandle*_tmp877;struct Cyc_Toc_Env*_tmp875=nv;_tmp876=*_tmp875;_tmp877=
_tmp876.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp877,nv),_tmp853);return;};
_LL383: {struct Cyc_Absyn_Switch_s_struct*_tmp854=(struct Cyc_Absyn_Switch_s_struct*)
_tmp836;if(_tmp854->tag != 10)goto _LL385;else{_tmp855=_tmp854->f1;_tmp856=_tmp854->f2;}}
_LL384: Cyc_Toc_xlate_switch(nv,s,_tmp855,_tmp856);return;_LL385: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp857=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp836;if(_tmp857->tag != 11)goto
_LL387;else{_tmp858=_tmp857->f1;_tmp859=_tmp857->f2;}}_LL386: {struct Cyc_Toc_Env
_tmp879;struct Cyc_Toc_FallthruInfo*_tmp87A;struct Cyc_Toc_Env*_tmp878=nv;_tmp879=*
_tmp878;_tmp87A=_tmp879.fallthru_info;if(_tmp87A == 0){const char*_tmpD75;void*
_tmpD74;(_tmpD74=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD75="fallthru in unexpected place",_tag_dyneither(_tmpD75,sizeof(char),29))),
_tag_dyneither(_tmpD74,sizeof(void*),0)));}{struct _dyneither_ptr*_tmp87E;struct
Cyc_List_List*_tmp87F;struct Cyc_Dict_Dict _tmp880;struct Cyc_Toc_FallthruInfo
_tmp87D=*_tmp87A;_tmp87E=_tmp87D.label;_tmp87F=_tmp87D.binders;_tmp880=_tmp87D.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp87E,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp859)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp881=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp87F);struct Cyc_List_List*_tmp882=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp858);for(0;_tmp881 != 0;(_tmp881=_tmp881->tl,
_tmp882=_tmp882->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp882))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp880,(
struct _tuple0*)_tmp881->hd),(struct Cyc_Absyn_Exp*)_tmp882->hd,0),s2,0);}s->r=s2->r;
return;};};};}_LL387: {struct Cyc_Absyn_Decl_s_struct*_tmp85A=(struct Cyc_Absyn_Decl_s_struct*)
_tmp836;if(_tmp85A->tag != 12)goto _LL389;else{_tmp85B=_tmp85A->f1;_tmp85C=_tmp85A->f2;}}
_LL388:{void*_tmp883=_tmp85B->r;struct Cyc_Absyn_Vardecl*_tmp885;struct Cyc_Absyn_Pat*
_tmp887;struct Cyc_Absyn_Exp*_tmp888;struct Cyc_List_List*_tmp88A;struct Cyc_Absyn_Fndecl*
_tmp88C;struct Cyc_Absyn_Tvar*_tmp88E;struct Cyc_Absyn_Vardecl*_tmp88F;int _tmp890;
struct Cyc_Absyn_Exp*_tmp891;struct Cyc_Absyn_Exp*_tmp893;struct Cyc_Absyn_Tvar*
_tmp894;struct Cyc_Absyn_Vardecl*_tmp895;_LL392: {struct Cyc_Absyn_Var_d_struct*
_tmp884=(struct Cyc_Absyn_Var_d_struct*)_tmp883;if(_tmp884->tag != 0)goto _LL394;
else{_tmp885=_tmp884->f1;}}_LL393: {struct Cyc_Toc_Env _tmp897;struct _RegionHandle*
_tmp898;struct Cyc_Toc_Env*_tmp896=nv;_tmp897=*_tmp896;_tmp898=_tmp897.rgn;{
struct Cyc_Absyn_Local_b_struct _tmpD78;struct Cyc_Absyn_Local_b_struct*_tmpD77;
struct Cyc_Toc_Env*_tmp899=Cyc_Toc_add_varmap(_tmp898,nv,_tmp885->name,Cyc_Absyn_varb_exp(
_tmp885->name,(void*)((_tmpD77=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpD77)),((_tmpD77[0]=((_tmpD78.tag=4,((_tmpD78.f1=_tmp885,_tmpD78)))),_tmpD77)))),
0));Cyc_Toc_local_decl_to_c(_tmp899,_tmp899,_tmp885,_tmp85C);}goto _LL391;}_LL394: {
struct Cyc_Absyn_Let_d_struct*_tmp886=(struct Cyc_Absyn_Let_d_struct*)_tmp883;if(
_tmp886->tag != 2)goto _LL396;else{_tmp887=_tmp886->f1;_tmp888=_tmp886->f3;}}
_LL395:{void*_tmp89C=_tmp887->r;struct Cyc_Absyn_Vardecl*_tmp89E;struct Cyc_Absyn_Pat*
_tmp89F;struct Cyc_Absyn_Pat _tmp8A0;void*_tmp8A1;_LL3A1: {struct Cyc_Absyn_Var_p_struct*
_tmp89D=(struct Cyc_Absyn_Var_p_struct*)_tmp89C;if(_tmp89D->tag != 1)goto _LL3A3;
else{_tmp89E=_tmp89D->f1;_tmp89F=_tmp89D->f2;_tmp8A0=*_tmp89F;_tmp8A1=_tmp8A0.r;{
struct Cyc_Absyn_Wild_p_struct*_tmp8A2=(struct Cyc_Absyn_Wild_p_struct*)_tmp8A1;
if(_tmp8A2->tag != 0)goto _LL3A3;};}}_LL3A2: {struct _tuple0*old_name=_tmp89E->name;
struct _tuple0*new_name=Cyc_Toc_temp_var();_tmp89E->name=new_name;_tmp89E->initializer=(
struct Cyc_Absyn_Exp*)_tmp888;{struct Cyc_Absyn_Var_d_struct _tmpD7B;struct Cyc_Absyn_Var_d_struct*
_tmpD7A;_tmp85B->r=(void*)((_tmpD7A=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD7A)),((_tmpD7A[0]=((_tmpD7B.tag=0,((_tmpD7B.f1=_tmp89E,_tmpD7B)))),
_tmpD7A))));}{struct Cyc_Toc_Env _tmp8A6;struct _RegionHandle*_tmp8A7;struct Cyc_Toc_Env*
_tmp8A5=nv;_tmp8A6=*_tmp8A5;_tmp8A7=_tmp8A6.rgn;{struct Cyc_Absyn_Local_b_struct
_tmpD7E;struct Cyc_Absyn_Local_b_struct*_tmpD7D;struct Cyc_Toc_Env*_tmp8A8=Cyc_Toc_add_varmap(
_tmp8A7,nv,old_name,Cyc_Absyn_varb_exp(new_name,(void*)((_tmpD7D=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmpD7D)),((_tmpD7D[0]=((_tmpD7E.tag=4,((_tmpD7E.f1=
_tmp89E,_tmpD7E)))),_tmpD7D)))),0));Cyc_Toc_local_decl_to_c(_tmp8A8,nv,_tmp89E,
_tmp85C);}goto _LL3A0;};}_LL3A3:;_LL3A4: s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp887,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp888->topt))->v,_tmp888,_tmp85C))->r;
goto _LL3A0;_LL3A0:;}goto _LL391;_LL396: {struct Cyc_Absyn_Letv_d_struct*_tmp889=(
struct Cyc_Absyn_Letv_d_struct*)_tmp883;if(_tmp889->tag != 3)goto _LL398;else{
_tmp88A=_tmp889->f1;}}_LL397: {struct Cyc_List_List*_tmp8AB=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp88A);if(_tmp8AB == 0){const char*_tmpD81;
void*_tmpD80;(_tmpD80=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpD81="empty Letv_d",_tag_dyneither(_tmpD81,sizeof(char),
13))),_tag_dyneither(_tmpD80,sizeof(void*),0)));}{struct Cyc_Absyn_Var_d_struct
_tmpD84;struct Cyc_Absyn_Var_d_struct*_tmpD83;_tmp85B->r=(void*)((_tmpD83=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD83)),((_tmpD83[0]=((_tmpD84.tag=
0,((_tmpD84.f1=(struct Cyc_Absyn_Vardecl*)_tmp8AB->hd,_tmpD84)))),_tmpD83))));}
_tmp8AB=_tmp8AB->tl;for(0;_tmp8AB != 0;_tmp8AB=_tmp8AB->tl){struct Cyc_Absyn_Var_d_struct
_tmpD87;struct Cyc_Absyn_Var_d_struct*_tmpD86;struct Cyc_Absyn_Decl*_tmp8B0=Cyc_Absyn_new_decl((
void*)((_tmpD86=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD86)),((
_tmpD86[0]=((_tmpD87.tag=0,((_tmpD87.f1=(struct Cyc_Absyn_Vardecl*)_tmp8AB->hd,
_tmpD87)))),_tmpD86)))),0);s->r=(Cyc_Absyn_decl_stmt(_tmp8B0,Cyc_Absyn_new_stmt(
s->r,0),0))->r;}Cyc_Toc_stmt_to_c(nv,s);goto _LL391;}_LL398: {struct Cyc_Absyn_Fn_d_struct*
_tmp88B=(struct Cyc_Absyn_Fn_d_struct*)_tmp883;if(_tmp88B->tag != 1)goto _LL39A;
else{_tmp88C=_tmp88B->f1;}}_LL399: {struct _tuple0*_tmp8B3=_tmp88C->name;struct
Cyc_Toc_Env _tmp8B5;struct _RegionHandle*_tmp8B6;struct Cyc_Toc_Env*_tmp8B4=nv;
_tmp8B5=*_tmp8B4;_tmp8B6=_tmp8B5.rgn;{struct Cyc_Toc_Env*_tmp8B7=Cyc_Toc_add_varmap(
_tmp8B6,nv,_tmp88C->name,Cyc_Absyn_var_exp(_tmp8B3,0));Cyc_Toc_fndecl_to_c(
_tmp8B7,_tmp88C,0);Cyc_Toc_stmt_to_c(_tmp8B7,_tmp85C);}goto _LL391;}_LL39A: {
struct Cyc_Absyn_Region_d_struct*_tmp88D=(struct Cyc_Absyn_Region_d_struct*)
_tmp883;if(_tmp88D->tag != 4)goto _LL39C;else{_tmp88E=_tmp88D->f1;_tmp88F=_tmp88D->f2;
_tmp890=_tmp88D->f3;_tmp891=_tmp88D->f4;}}_LL39B: {struct Cyc_Absyn_Stmt*_tmp8B8=
_tmp85C;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple0*
rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp88F->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp8BA;struct _RegionHandle*_tmp8BB;struct Cyc_Toc_Env*
_tmp8B9=nv;_tmp8BA=*_tmp8B9;_tmp8BB=_tmp8BA.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp8BB,nv,x_var,x_exp),_tmp8B8);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp8B8,0))->r;
else{if(_tmp891 == 0){struct Cyc_Absyn_Exp*_tmpD8A[1];struct Cyc_Absyn_Exp*_tmpD89[
1];struct Cyc_List_List*_tmpD88;s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpD88=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD88)),((_tmpD88->hd=Cyc_Absyn_string_exp(
Cyc_Absynpp_qvar2string(x_var),0),((_tmpD88->tl=0,_tmpD88)))))),0),Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((
_tmpD89[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD89,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp8B8,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD8A[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD8A,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp891);{struct Cyc_Absyn_Exp*
_tmpD8C[1];struct Cyc_Absyn_Exp*_tmpD8B[2];s->r=(Cyc_Absyn_declare_stmt(rh_var,
Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpD8B[1]=(struct Cyc_Absyn_Exp*)_tmp891,((_tmpD8B[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD8B,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp8B8,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpD8C[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD8C,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
return;}_LL39C: {struct Cyc_Absyn_Alias_d_struct*_tmp892=(struct Cyc_Absyn_Alias_d_struct*)
_tmp883;if(_tmp892->tag != 5)goto _LL39E;else{_tmp893=_tmp892->f1;_tmp894=_tmp892->f2;
_tmp895=_tmp892->f3;}}_LL39D: {struct _tuple0*old_name=_tmp895->name;struct
_tuple0*new_name=Cyc_Toc_temp_var();_tmp895->name=new_name;_tmp895->initializer=(
struct Cyc_Absyn_Exp*)_tmp893;{struct Cyc_Absyn_Decl_s_struct _tmpD9B;struct Cyc_Absyn_Var_d_struct*
_tmpD9A;struct Cyc_Absyn_Var_d_struct _tmpD99;struct Cyc_Absyn_Decl*_tmpD98;struct
Cyc_Absyn_Decl_s_struct*_tmpD97;s->r=(void*)((_tmpD97=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD97)),((_tmpD97[0]=((_tmpD9B.tag=12,((_tmpD9B.f1=((_tmpD98=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD98)),((_tmpD98->r=(void*)((
_tmpD9A=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD9A)),((_tmpD9A[0]=((
_tmpD99.tag=0,((_tmpD99.f1=_tmp895,_tmpD99)))),_tmpD9A)))),((_tmpD98->loc=0,
_tmpD98)))))),((_tmpD9B.f2=_tmp85C,_tmpD9B)))))),_tmpD97))));}{struct Cyc_Toc_Env
_tmp8C7;struct _RegionHandle*_tmp8C8;struct Cyc_Toc_Env*_tmp8C6=nv;_tmp8C7=*
_tmp8C6;_tmp8C8=_tmp8C7.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD9E;struct Cyc_Absyn_Local_b_struct*
_tmpD9D;struct Cyc_Toc_Env*_tmp8C9=Cyc_Toc_add_varmap(_tmp8C8,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD9D=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD9D)),((
_tmpD9D[0]=((_tmpD9E.tag=4,((_tmpD9E.f1=_tmp895,_tmpD9E)))),_tmpD9D)))),0));Cyc_Toc_local_decl_to_c(
_tmp8C9,nv,_tmp895,_tmp85C);}return;};}_LL39E:;_LL39F: {const char*_tmpDA1;void*
_tmpDA0;(_tmpDA0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpDA1="bad nested declaration within function",_tag_dyneither(_tmpDA1,sizeof(
char),39))),_tag_dyneither(_tmpDA0,sizeof(void*),0)));}_LL391:;}return;_LL389: {
struct Cyc_Absyn_Label_s_struct*_tmp85D=(struct Cyc_Absyn_Label_s_struct*)_tmp836;
if(_tmp85D->tag != 13)goto _LL38B;else{_tmp85E=_tmp85D->f1;_tmp85F=_tmp85D->f2;}}
_LL38A: s=_tmp85F;continue;_LL38B: {struct Cyc_Absyn_Do_s_struct*_tmp860=(struct
Cyc_Absyn_Do_s_struct*)_tmp836;if(_tmp860->tag != 14)goto _LL38D;else{_tmp861=
_tmp860->f1;_tmp862=_tmp860->f2;_tmp863=_tmp862.f1;}}_LL38C: {struct Cyc_Toc_Env
_tmp8CF;struct _RegionHandle*_tmp8D0;struct Cyc_Toc_Env*_tmp8CE=nv;_tmp8CF=*
_tmp8CE;_tmp8D0=_tmp8CF.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp8D0,nv),
_tmp861);Cyc_Toc_exp_to_c(nv,_tmp863);return;}_LL38D: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp864=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp836;if(_tmp864->tag != 15)goto
_LL38F;else{_tmp865=_tmp864->f1;_tmp866=_tmp864->f2;}}_LL38E: {struct _tuple0*
h_var=Cyc_Toc_temp_var();struct _tuple0*e_var=Cyc_Toc_temp_var();struct _tuple0*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);{struct Cyc_Core_Opt*_tmpDA2;
e_exp->topt=((_tmpDA2=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDA2)),((
_tmpDA2->v=e_typ,_tmpDA2))));}{struct Cyc_Toc_Env _tmp8D3;struct _RegionHandle*
_tmp8D4;struct Cyc_Toc_Env*_tmp8D2=nv;_tmp8D3=*_tmp8D2;_tmp8D4=_tmp8D3.rgn;{
struct Cyc_Toc_Env*_tmp8D5=Cyc_Toc_add_varmap(_tmp8D4,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(
_tmp8D5,_tmp865);{struct Cyc_Absyn_Stmt*_tmp8D6=Cyc_Absyn_seq_stmt(_tmp865,Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);struct _tuple0*_tmp8D7=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp8D8=Cyc_Absyn_var_exp(_tmp8D7,0);struct Cyc_Absyn_Vardecl*
_tmp8D9=Cyc_Absyn_new_vardecl(_tmp8D7,Cyc_Absyn_exn_typ(),0);{struct Cyc_Core_Opt*
_tmpDA3;_tmp8D8->topt=((_tmpDA3=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpDA3)),((_tmpDA3->v=Cyc_Absyn_exn_typ(),_tmpDA3))));}{struct Cyc_Core_Opt*
_tmpDB4;struct Cyc_Absyn_Var_p_struct*_tmpDB3;struct Cyc_Absyn_Pat*_tmpDB2;struct
Cyc_Core_Opt*_tmpDB1;struct Cyc_Absyn_Var_p_struct _tmpDB0;struct Cyc_Absyn_Pat*
_tmpDAF;struct Cyc_Absyn_Pat*_tmp8DB=(_tmpDAF=_cycalloc(sizeof(*_tmpDAF)),((
_tmpDAF->r=(void*)((_tmpDB3=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpDB3)),((_tmpDB3[0]=((_tmpDB0.tag=1,((_tmpDB0.f1=_tmp8D9,((_tmpDB0.f2=((
_tmpDB2=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDB2)),((_tmpDB2->r=(
void*)& Cyc_Absyn_Wild_p_val,((_tmpDB2->topt=((_tmpDB1=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpDB1)),((_tmpDB1->v=Cyc_Absyn_exn_typ(),_tmpDB1)))),((_tmpDB2->loc=0,
_tmpDB2)))))))),_tmpDB0)))))),_tmpDB3)))),((_tmpDAF->topt=((_tmpDB4=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDB4)),((_tmpDB4->v=Cyc_Absyn_exn_typ(),
_tmpDB4)))),((_tmpDAF->loc=0,_tmpDAF)))))));struct Cyc_Absyn_Exp*_tmp8DC=Cyc_Absyn_throw_exp(
_tmp8D8,0);{struct Cyc_Core_Opt*_tmpDB5;_tmp8DC->topt=((_tmpDB5=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmpDB5)),((_tmpDB5->v=(void*)& Cyc_Absyn_VoidType_val,
_tmpDB5))));}{struct Cyc_Absyn_Stmt*_tmp8DE=Cyc_Absyn_exp_stmt(_tmp8DC,0);struct
Cyc_Core_Opt*_tmpDBB;struct Cyc_List_List*_tmpDBA;struct Cyc_Absyn_Switch_clause*
_tmpDB9;struct Cyc_Absyn_Switch_clause*_tmp8DF=(_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((
_tmpDB9->pattern=_tmp8DB,((_tmpDB9->pat_vars=((_tmpDBB=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpDBB)),((_tmpDBB->v=((_tmpDBA=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpDBA)),((_tmpDBA->hd=_tmp8D9,((_tmpDBA->tl=0,_tmpDBA)))))),_tmpDBB)))),((
_tmpDB9->where_clause=0,((_tmpDB9->body=_tmp8DE,((_tmpDB9->loc=0,_tmpDB9)))))))))));
struct Cyc_List_List*_tmpDBC;struct Cyc_Absyn_Stmt*_tmp8E0=Cyc_Absyn_switch_stmt(
e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp866,((_tmpDBC=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDBC)),((
_tmpDBC->hd=_tmp8DF,((_tmpDBC->tl=0,_tmpDBC))))))),0);Cyc_Toc_stmt_to_c(_tmp8D5,
_tmp8E0);{struct Cyc_List_List*_tmpDBD;struct Cyc_Absyn_Exp*_tmp8E1=Cyc_Absyn_fncall_exp(
Cyc_Toc_setjmp_e,((_tmpDBD=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDBD)),((
_tmpDBD->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpDBD->tl=0,
_tmpDBD)))))),0);struct Cyc_List_List*_tmpDBE;struct Cyc_Absyn_Stmt*_tmp8E2=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((_tmpDBE=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpDBE)),((_tmpDBE->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpDBE->tl=0,
_tmpDBE)))))),0),0);struct Cyc_Absyn_Exp*_tmp8E3=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
0,0);struct Cyc_Absyn_Exp*_tmp8E4=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp8E2,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp8E3,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp8E1,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp8E4,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp8D6,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp8E0,0),0),0),0),0),0))->r;};};};};}
return;};}_LL38F: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp867=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp836;if(_tmp867->tag != 16)goto _LL36E;else{_tmp868=_tmp867->f1;}}_LL390: if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp868);{struct Cyc_List_List*
_tmpDBF;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((
_tmpDBF=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDBF)),((_tmpDBF->hd=
_tmp868,((_tmpDBF->tl=0,_tmpDBF)))))),0));};}return;_LL36E:;}}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s);struct _tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f,int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8F2=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8F2;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp8F3=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp8F4=f->args;for(0;_tmp8F4 != 0;_tmp8F4=_tmp8F4->tl){struct
_tuple0*_tmpDC0;struct _tuple0*_tmp8F5=(_tmpDC0=_cycalloc(sizeof(*_tmpDC0)),((
_tmpDC0->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpDC0->f2=(*((struct
_tuple25*)_tmp8F4->hd)).f1,_tmpDC0)))));(*((struct _tuple25*)_tmp8F4->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple25*)_tmp8F4->hd)).f3);_tmp8F3=Cyc_Toc_add_varmap(frgn,_tmp8F3,
_tmp8F5,Cyc_Absyn_var_exp(_tmp8F5,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp8F8;struct Cyc_Absyn_Tqual
_tmp8F9;void*_tmp8FA;int _tmp8FB;struct Cyc_Absyn_VarargInfo _tmp8F7=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp8F8=_tmp8F7.name;_tmp8F9=_tmp8F7.tq;_tmp8FA=
_tmp8F7.type;_tmp8FB=_tmp8F7.inject;{void*_tmp8FC=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp8FA,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8F9,Cyc_Absyn_false_conref));struct
_tuple0*_tmpDC1;struct _tuple0*_tmp8FD=(_tmpDC1=_cycalloc(sizeof(*_tmpDC1)),((
_tmpDC1->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpDC1->f2=(struct
_dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null(_tmp8F8))->v,_tmpDC1)))));{
struct _tuple25*_tmpDC4;struct Cyc_List_List*_tmpDC3;f->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpDC3=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDC3)),((_tmpDC3->hd=((_tmpDC4=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDC4)),((_tmpDC4->f1=(struct
_dyneither_ptr*)_tmp8F8->v,((_tmpDC4->f2=_tmp8F9,((_tmpDC4->f3=_tmp8FC,_tmpDC4)))))))),((
_tmpDC3->tl=0,_tmpDC3)))))));}_tmp8F3=Cyc_Toc_add_varmap(frgn,_tmp8F3,_tmp8FD,
Cyc_Absyn_var_exp(_tmp8FD,0));f->cyc_varargs=0;};}{struct Cyc_List_List*_tmp901=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp901 != 0;_tmp901=_tmp901->tl){((struct Cyc_Absyn_Vardecl*)_tmp901->hd)->type=
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp901->hd)->type);}}Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel(frgn,_tmp8F3),f->body);};_pop_region(frgn);};}static enum 
Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){switch(s){case Cyc_Absyn_Abstract:
_LL3A5: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3A6: return Cyc_Absyn_Extern;
default: _LL3A7: return s;}}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad,int add_to_decls){struct _tuple0*_tmp902=ad->name;struct _DynRegionHandle*
_tmp904;struct Cyc_Dict_Dict*_tmp905;struct Cyc_Toc_TocState _tmp903=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp904=_tmp903.dyn;_tmp905=_tmp903.aggrs_so_far;{
struct _DynRegionFrame _tmp906;struct _RegionHandle*d=_open_dynregion(& _tmp906,
_tmp904);{int seen_defn_before;struct _tuple14**_tmp907=((struct _tuple14**(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp905,_tmp902);if(
_tmp907 == 0){seen_defn_before=0;{struct _tuple14*v;if(ad->kind == Cyc_Absyn_StructA){
struct _tuple14*_tmpDC5;v=((_tmpDC5=_region_malloc(d,sizeof(*_tmpDC5)),((_tmpDC5->f1=
ad,((_tmpDC5->f2=Cyc_Absyn_strctq(_tmp902),_tmpDC5))))));}else{struct _tuple14*
_tmpDC6;v=((_tmpDC6=_region_malloc(d,sizeof(*_tmpDC6)),((_tmpDC6->f1=ad,((
_tmpDC6->f2=Cyc_Absyn_unionq_typ(_tmp902),_tmpDC6))))));}*_tmp905=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp905,
_tmp902,v);};}else{struct _tuple14 _tmp90B;struct Cyc_Absyn_Aggrdecl*_tmp90C;void*
_tmp90D;struct _tuple14*_tmp90A=*_tmp907;_tmp90B=*_tmp90A;_tmp90C=_tmp90B.f1;
_tmp90D=_tmp90B.f2;if(_tmp90C->impl == 0){{struct _tuple14*_tmpDC7;*_tmp905=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))
Cyc_Dict_insert)(*_tmp905,_tmp902,((_tmpDC7=_region_malloc(d,sizeof(*_tmpDC7)),((
_tmpDC7->f1=ad,((_tmpDC7->f2=_tmp90D,_tmpDC7)))))));}seen_defn_before=0;}else{
seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*_tmpDC8;struct Cyc_Absyn_Aggrdecl*
new_ad=(_tmpDC8=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDC8)),((
_tmpDC8->kind=ad->kind,((_tmpDC8->sc=Cyc_Absyn_Public,((_tmpDC8->name=ad->name,((
_tmpDC8->tvs=0,((_tmpDC8->impl=0,((_tmpDC8->attributes=ad->attributes,_tmpDC8)))))))))))));
if(ad->impl != 0  && !seen_defn_before){{struct Cyc_Absyn_AggrdeclImpl*_tmpDC9;
new_ad->impl=((_tmpDC9=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDC9)),((
_tmpDC9->exist_vars=0,((_tmpDC9->rgn_po=0,((_tmpDC9->fields=0,((_tmpDC9->tagged=
0,_tmpDC9))))))))));}{struct Cyc_List_List*new_fields=0;{struct Cyc_List_List*
_tmp910=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;
_tmp910 != 0;_tmp910=_tmp910->tl){struct Cyc_Absyn_Aggrfield*_tmp911=(struct Cyc_Absyn_Aggrfield*)
_tmp910->hd;struct Cyc_Absyn_Aggrfield*_tmpDCA;struct Cyc_Absyn_Aggrfield*_tmp912=(
_tmpDCA=_cycalloc(sizeof(*_tmpDCA)),((_tmpDCA->name=_tmp911->name,((_tmpDCA->tq=
Cyc_Toc_mt_tq,((_tmpDCA->type=Cyc_Toc_typ_to_c_array(_tmp911->type),((_tmpDCA->width=
_tmp911->width,((_tmpDCA->attributes=_tmp911->attributes,_tmpDCA)))))))))));if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){void*_tmp913=
_tmp912->type;struct _dyneither_ptr*_tmp914=_tmp912->name;const char*_tmpDCF;void*
_tmpDCE[2];struct Cyc_String_pa_struct _tmpDCD;struct Cyc_String_pa_struct _tmpDCC;
struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpDCC.tag=0,((_tmpDCC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp914),((_tmpDCD.tag=0,((_tmpDCD.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpDCE[0]=&
_tmpDCD,((_tmpDCE[1]=& _tmpDCC,Cyc_aprintf(((_tmpDCF="_union_%s_%s",
_tag_dyneither(_tmpDCF,sizeof(char),13))),_tag_dyneither(_tmpDCE,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpDD0;struct _dyneither_ptr*str=(_tmpDD0=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmpDD0)),((_tmpDD0[0]=s,_tmpDD0)));struct Cyc_Absyn_Aggrfield*
_tmpDD1;struct Cyc_Absyn_Aggrfield*_tmp915=(_tmpDD1=_cycalloc(sizeof(*_tmpDD1)),((
_tmpDD1->name=Cyc_Toc_val_sp,((_tmpDD1->tq=Cyc_Toc_mt_tq,((_tmpDD1->type=_tmp913,((
_tmpDD1->width=0,((_tmpDD1->attributes=0,_tmpDD1)))))))))));struct Cyc_Absyn_Aggrfield*
_tmpDD2;struct Cyc_Absyn_Aggrfield*_tmp916=(_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((
_tmpDD2->name=Cyc_Toc_tag_sp,((_tmpDD2->tq=Cyc_Toc_mt_tq,((_tmpDD2->type=Cyc_Absyn_sint_typ,((
_tmpDD2->width=0,((_tmpDD2->attributes=0,_tmpDD2)))))))))));struct Cyc_Absyn_Aggrfield*
_tmpDD3[2];struct Cyc_List_List*_tmp917=(_tmpDD3[1]=_tmp915,((_tmpDD3[0]=_tmp916,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpDD3,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));struct Cyc_Absyn_AggrdeclImpl*
_tmpDD8;struct _tuple0*_tmpDD7;struct Cyc_Absyn_Aggrdecl*_tmpDD6;struct Cyc_Absyn_Aggrdecl*
_tmp918=(_tmpDD6=_cycalloc(sizeof(*_tmpDD6)),((_tmpDD6->kind=Cyc_Absyn_StructA,((
_tmpDD6->sc=Cyc_Absyn_Public,((_tmpDD6->name=((_tmpDD7=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpDD7)),((_tmpDD7->f1=Cyc_Absyn_Loc_n,((_tmpDD7->f2=str,_tmpDD7)))))),((
_tmpDD6->tvs=0,((_tmpDD6->impl=((_tmpDD8=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpDD8)),((_tmpDD8->exist_vars=0,((_tmpDD8->rgn_po=0,((_tmpDD8->fields=
_tmp917,((_tmpDD8->tagged=0,_tmpDD8)))))))))),((_tmpDD6->attributes=0,_tmpDD6)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmpDDE;struct Cyc_Absyn_Aggr_d_struct _tmpDDD;
struct Cyc_List_List*_tmpDDC;Cyc_Toc_result_decls=((_tmpDDC=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpDDC)),((_tmpDDC->hd=Cyc_Absyn_new_decl((void*)((_tmpDDE=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDDE)),((_tmpDDE[0]=((_tmpDDD.tag=
6,((_tmpDDD.f1=_tmp918,_tmpDDD)))),_tmpDDE)))),0),((_tmpDDC->tl=Cyc_Toc_result_decls,
_tmpDDC))))));}_tmp912->type=Cyc_Absyn_strct(str);}{struct Cyc_List_List*_tmpDDF;
new_fields=((_tmpDDF=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDDF)),((
_tmpDDF->hd=_tmp912,((_tmpDDF->tl=new_fields,_tmpDDF))))));};}}(new_ad->impl)->fields=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
if(add_to_decls){struct Cyc_Absyn_Decl*_tmpDE9;struct Cyc_Absyn_Aggr_d_struct
_tmpDE8;struct Cyc_Absyn_Aggr_d_struct*_tmpDE7;struct Cyc_List_List*_tmpDE6;Cyc_Toc_result_decls=((
_tmpDE6=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDE6)),((_tmpDE6->hd=((
_tmpDE9=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDE9)),((_tmpDE9->r=(
void*)((_tmpDE7=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDE7)),((
_tmpDE7[0]=((_tmpDE8.tag=6,((_tmpDE8.f1=new_ad,_tmpDE8)))),_tmpDE7)))),((_tmpDE9->loc=
0,_tmpDE9)))))),((_tmpDE6->tl=Cyc_Toc_result_decls,_tmpDE6))))));}};};
_pop_dynregion(d);};}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud){struct _DynRegionHandle*_tmp92F;struct Cyc_Set_Set**_tmp930;struct Cyc_Toc_TocState
_tmp92E=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp92F=
_tmp92E.dyn;_tmp930=_tmp92E.datatypes_so_far;{struct _DynRegionFrame _tmp931;
struct _RegionHandle*d=_open_dynregion(& _tmp931,_tmp92F);{struct _tuple0*_tmp932=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))
Cyc_Set_member)(*_tmp930,_tmp932)){_npop_handler(0);return;}*_tmp930=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*
_tmp930,_tmp932);}{struct Cyc_List_List*_tmp933=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp933 != 0;_tmp933=_tmp933->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp933->hd;struct Cyc_List_List*_tmp934=0;int i=
1;{struct Cyc_List_List*_tmp935=f->typs;for(0;_tmp935 != 0;(_tmp935=_tmp935->tl,i
++)){struct _dyneither_ptr*_tmp936=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpDEA;struct Cyc_Absyn_Aggrfield*_tmp937=(_tmpDEA=_cycalloc(sizeof(*_tmpDEA)),((
_tmpDEA->name=_tmp936,((_tmpDEA->tq=(*((struct _tuple9*)_tmp935->hd)).f1,((
_tmpDEA->type=Cyc_Toc_typ_to_c_array((*((struct _tuple9*)_tmp935->hd)).f2),((
_tmpDEA->width=0,((_tmpDEA->attributes=0,_tmpDEA)))))))))));struct Cyc_List_List*
_tmpDEB;_tmp934=((_tmpDEB=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDEB)),((
_tmpDEB->hd=_tmp937,((_tmpDEB->tl=_tmp934,_tmpDEB))))));}}{struct Cyc_Absyn_Aggrfield*
_tmpDEE;struct Cyc_List_List*_tmpDED;_tmp934=((_tmpDED=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpDED)),((_tmpDED->hd=((_tmpDEE=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpDEE)),((_tmpDEE->name=Cyc_Toc_tag_sp,((_tmpDEE->tq=Cyc_Toc_mt_tq,((
_tmpDEE->type=Cyc_Absyn_sint_typ,((_tmpDEE->width=0,((_tmpDEE->attributes=0,
_tmpDEE)))))))))))),((_tmpDED->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp934),_tmpDED))))));}{struct Cyc_Absyn_AggrdeclImpl*
_tmpDF3;const char*_tmpDF2;struct Cyc_Absyn_Aggrdecl*_tmpDF1;struct Cyc_Absyn_Aggrdecl*
_tmp93C=(_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((_tmpDF1->kind=Cyc_Absyn_StructA,((
_tmpDF1->sc=Cyc_Absyn_Public,((_tmpDF1->name=Cyc_Toc_collapse_qvar_tag(f->name,((
_tmpDF2="_struct",_tag_dyneither(_tmpDF2,sizeof(char),8)))),((_tmpDF1->tvs=0,((
_tmpDF1->impl=((_tmpDF3=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDF3)),((
_tmpDF3->exist_vars=0,((_tmpDF3->rgn_po=0,((_tmpDF3->fields=_tmp934,((_tmpDF3->tagged=
0,_tmpDF3)))))))))),((_tmpDF1->attributes=0,_tmpDF1)))))))))))));struct Cyc_Absyn_Aggr_d_struct*
_tmpDF9;struct Cyc_Absyn_Aggr_d_struct _tmpDF8;struct Cyc_List_List*_tmpDF7;Cyc_Toc_result_decls=((
_tmpDF7=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDF7)),((_tmpDF7->hd=
Cyc_Absyn_new_decl((void*)((_tmpDF9=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpDF9)),((_tmpDF9[0]=((_tmpDF8.tag=6,((_tmpDF8.f1=_tmp93C,_tmpDF8)))),
_tmpDF9)))),0),((_tmpDF7->tl=Cyc_Toc_result_decls,_tmpDF7))))));};}};;
_pop_dynregion(d);};}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd){if(xd->fields == 0)return;{struct _DynRegionHandle*_tmp944;struct Cyc_Dict_Dict*
_tmp945;struct Cyc_Toc_TocState _tmp943=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp944=_tmp943.dyn;_tmp945=_tmp943.xdatatypes_so_far;{struct _DynRegionFrame
_tmp946;struct _RegionHandle*d=_open_dynregion(& _tmp946,_tmp944);{struct _tuple0*
_tmp947=xd->name;struct Cyc_List_List*_tmp948=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp948 != 0;_tmp948=_tmp948->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp948->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp949=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp94A=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp949,Cyc_Absyn_false_conref,0);int*_tmp94B=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp945,f->name);int
_tmp94C;_LL3AA: if(_tmp94B != 0)goto _LL3AC;_LL3AB: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp94D=Cyc_Absyn_new_vardecl(f->name,_tmp94A,
initopt);_tmp94D->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*_tmpDFF;struct Cyc_Absyn_Var_d_struct
_tmpDFE;struct Cyc_List_List*_tmpDFD;Cyc_Toc_result_decls=((_tmpDFD=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDFD)),((_tmpDFD->hd=Cyc_Absyn_new_decl((
void*)((_tmpDFF=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDFF)),((
_tmpDFF[0]=((_tmpDFE.tag=0,((_tmpDFE.f1=_tmp94D,_tmpDFE)))),_tmpDFF)))),0),((
_tmpDFD->tl=Cyc_Toc_result_decls,_tmpDFD))))));}*_tmp945=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp945,f->name,f->sc
!= Cyc_Absyn_Extern);{struct Cyc_List_List*fields=0;int i=1;{struct Cyc_List_List*
_tmp951=f->typs;for(0;_tmp951 != 0;(_tmp951=_tmp951->tl,i ++)){struct Cyc_Int_pa_struct
_tmpE07;void*_tmpE06[1];const char*_tmpE05;struct _dyneither_ptr*_tmpE04;struct
_dyneither_ptr*_tmp952=(_tmpE04=_cycalloc(sizeof(*_tmpE04)),((_tmpE04[0]=(struct
_dyneither_ptr)((_tmpE07.tag=1,((_tmpE07.f1=(unsigned long)i,((_tmpE06[0]=&
_tmpE07,Cyc_aprintf(((_tmpE05="f%d",_tag_dyneither(_tmpE05,sizeof(char),4))),
_tag_dyneither(_tmpE06,sizeof(void*),1)))))))),_tmpE04)));struct Cyc_Absyn_Aggrfield*
_tmpE08;struct Cyc_Absyn_Aggrfield*_tmp953=(_tmpE08=_cycalloc(sizeof(*_tmpE08)),((
_tmpE08->name=_tmp952,((_tmpE08->tq=(*((struct _tuple9*)_tmp951->hd)).f1,((
_tmpE08->type=Cyc_Toc_typ_to_c_array((*((struct _tuple9*)_tmp951->hd)).f2),((
_tmpE08->width=0,((_tmpE08->attributes=0,_tmpE08)))))))))));struct Cyc_List_List*
_tmpE09;fields=((_tmpE09=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE09)),((
_tmpE09->hd=_tmp953,((_tmpE09->tl=fields,_tmpE09))))));}}{struct Cyc_Absyn_Aggrfield*
_tmpE0C;struct Cyc_List_List*_tmpE0B;fields=((_tmpE0B=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE0B)),((_tmpE0B->hd=((_tmpE0C=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE0C)),((_tmpE0C->name=Cyc_Toc_tag_sp,((_tmpE0C->tq=Cyc_Toc_mt_tq,((
_tmpE0C->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpE0C->width=
0,((_tmpE0C->attributes=0,_tmpE0C)))))))))))),((_tmpE0B->tl=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpE0B))))));}{struct Cyc_Absyn_AggrdeclImpl*
_tmpE11;const char*_tmpE10;struct Cyc_Absyn_Aggrdecl*_tmpE0F;struct Cyc_Absyn_Aggrdecl*
_tmp95C=(_tmpE0F=_cycalloc(sizeof(*_tmpE0F)),((_tmpE0F->kind=Cyc_Absyn_StructA,((
_tmpE0F->sc=Cyc_Absyn_Public,((_tmpE0F->name=Cyc_Toc_collapse_qvar_tag(f->name,((
_tmpE10="_struct",_tag_dyneither(_tmpE10,sizeof(char),8)))),((_tmpE0F->tvs=0,((
_tmpE0F->impl=((_tmpE11=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE11)),((
_tmpE11->exist_vars=0,((_tmpE11->rgn_po=0,((_tmpE11->fields=fields,((_tmpE11->tagged=
0,_tmpE11)))))))))),((_tmpE0F->attributes=0,_tmpE0F)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*
_tmpE17;struct Cyc_Absyn_Aggr_d_struct _tmpE16;struct Cyc_List_List*_tmpE15;Cyc_Toc_result_decls=((
_tmpE15=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE15)),((_tmpE15->hd=
Cyc_Absyn_new_decl((void*)((_tmpE17=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE17)),((_tmpE17[0]=((_tmpE16.tag=6,((_tmpE16.f1=_tmp95C,_tmpE16)))),
_tmpE17)))),0),((_tmpE15->tl=Cyc_Toc_result_decls,_tmpE15))))));}goto _LL3A9;};};};}
_LL3AC: if(_tmp94B == 0)goto _LL3AE;_tmp94C=*_tmp94B;if(_tmp94C != 0)goto _LL3AE;
_LL3AD: if(f->sc != Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*_tmp963=Cyc_Absyn_string_exp(*
fn,0);struct Cyc_Absyn_Vardecl*_tmp964=Cyc_Absyn_new_vardecl(f->name,_tmp94A,(
struct Cyc_Absyn_Exp*)_tmp963);_tmp964->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*
_tmpE1D;struct Cyc_Absyn_Var_d_struct _tmpE1C;struct Cyc_List_List*_tmpE1B;Cyc_Toc_result_decls=((
_tmpE1B=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE1B)),((_tmpE1B->hd=
Cyc_Absyn_new_decl((void*)((_tmpE1D=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE1D)),((_tmpE1D[0]=((_tmpE1C.tag=0,((_tmpE1C.f1=_tmp964,_tmpE1C)))),
_tmpE1D)))),0),((_tmpE1B->tl=Cyc_Toc_result_decls,_tmpE1B))))));}*_tmp945=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp945,f->name,1);}goto _LL3A9;_LL3AE:;_LL3AF: goto _LL3A9;_LL3A9:;}};
_pop_dynregion(d);};};}static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*
ed){ed->sc=Cyc_Absyn_Public;if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(
struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,
struct Cyc_Absyn_Stmt*s){void*old_typ=vd->type;vd->type=Cyc_Toc_typ_to_c_array(
old_typ);if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ))vd->sc=Cyc_Absyn_Public;Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp968=init->r;struct Cyc_Absyn_Vardecl*_tmp96A;struct Cyc_Absyn_Exp*_tmp96B;
struct Cyc_Absyn_Exp*_tmp96C;int _tmp96D;_LL3B1: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp969=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp968;if(_tmp969->tag != 28)
goto _LL3B3;else{_tmp96A=_tmp969->f1;_tmp96B=_tmp969->f2;_tmp96C=_tmp969->f3;
_tmp96D=_tmp969->f4;}}_LL3B2: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp96A,_tmp96B,_tmp96C,_tmp96D,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL3B0;_LL3B3:;_LL3B4: if(vd->sc == Cyc_Absyn_Static){struct Cyc_Toc_Env
_tmp96F;struct _RegionHandle*_tmp970;struct Cyc_Toc_Env*_tmp96E=init_nv;_tmp96F=*
_tmp96E;_tmp970=_tmp96F.rgn;{struct Cyc_Toc_Env*_tmp971=Cyc_Toc_set_toplevel(
_tmp970,init_nv);Cyc_Toc_exp_to_c(_tmp971,init);};}else{Cyc_Toc_exp_to_c(init_nv,
init);}goto _LL3B0;_LL3B0:;}else{void*_tmp972=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_ArrayInfo _tmp974;void*_tmp975;struct Cyc_Absyn_Exp*_tmp976;union Cyc_Absyn_Constraint*
_tmp977;_LL3B6:{struct Cyc_Absyn_ArrayType_struct*_tmp973=(struct Cyc_Absyn_ArrayType_struct*)
_tmp972;if(_tmp973->tag != 9)goto _LL3B8;else{_tmp974=_tmp973->f1;_tmp975=_tmp974.elt_type;
_tmp976=_tmp974.num_elts;_tmp977=_tmp974.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp977))goto _LL3B8;_LL3B7: if(_tmp976 == 0){const char*
_tmpE20;void*_tmpE1F;(_tmpE1F=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE20="can't initialize zero-terminated array -- size unknown",
_tag_dyneither(_tmpE20,sizeof(char),55))),_tag_dyneither(_tmpE1F,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp976;struct Cyc_Absyn_Exp*
_tmp97A=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp97B=Cyc_Absyn_signed_int_exp(
0,0);s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp97A,
_tmp97B,0),0),Cyc_Absyn_new_stmt(s->r,0));goto _LL3B5;};_LL3B8:;_LL3B9: goto _LL3B5;
_LL3B5:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple0*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp97D;struct
_RegionHandle*_tmp97E;struct Cyc_Toc_Env*_tmp97C=nv;_tmp97D=*_tmp97C;_tmp97E=
_tmp97D.rgn;{struct Cyc_Absyn_Stmt*_tmp97F=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp980=Cyc_Toc_share_env(_tmp97E,nv);struct Cyc_Toc_Env*_tmp982;struct Cyc_List_List*
_tmp983;struct Cyc_Absyn_Stmt*_tmp984;struct _tuple21 _tmp981=Cyc_Toc_xlate_pat(
_tmp980,_tmp97E,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp982=_tmp981.f1;_tmp983=_tmp981.f2;_tmp984=_tmp981.f3;Cyc_Toc_stmt_to_c(
_tmp982,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp984,s,0),0);for(0;_tmp983 != 0;_tmp983=_tmp983->tl){
struct _tuple22 _tmp986;struct _tuple0*_tmp987;void*_tmp988;struct _tuple22*_tmp985=(
struct _tuple22*)_tmp983->hd;_tmp986=*_tmp985;_tmp987=_tmp986.f1;_tmp988=_tmp986.f2;
s=Cyc_Absyn_declare_stmt(_tmp987,_tmp988,0,s,0);}}return s;};}static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e){void*_tmp989=e->r;struct Cyc_Absyn_Exp*_tmp98B;struct Cyc_Absyn_Exp*
_tmp98D;struct Cyc_Absyn_Exp*_tmp98F;struct Cyc_Absyn_Exp*_tmp991;struct Cyc_Absyn_Exp*
_tmp993;struct Cyc_Absyn_Exp*_tmp995;struct Cyc_Absyn_Exp*_tmp997;struct Cyc_Absyn_Exp*
_tmp999;struct Cyc_List_List*_tmp99B;struct Cyc_Absyn_Exp*_tmp99D;struct Cyc_Absyn_Exp*
_tmp99E;struct Cyc_Absyn_Exp*_tmp9A0;struct Cyc_Absyn_Exp*_tmp9A1;struct Cyc_Absyn_Exp*
_tmp9A3;struct Cyc_Absyn_Exp*_tmp9A4;struct Cyc_Absyn_Exp*_tmp9A6;struct Cyc_Absyn_Exp*
_tmp9A7;struct Cyc_Absyn_Exp*_tmp9A9;struct Cyc_Absyn_Exp*_tmp9AA;struct Cyc_Absyn_Exp*
_tmp9AC;struct Cyc_Absyn_Exp*_tmp9AD;struct Cyc_Absyn_Exp*_tmp9AF;struct Cyc_Absyn_Exp*
_tmp9B0;struct Cyc_Absyn_Exp*_tmp9B1;struct Cyc_Absyn_Exp*_tmp9B3;struct Cyc_List_List*
_tmp9B4;struct Cyc_Absyn_Exp*_tmp9B6;struct Cyc_List_List*_tmp9B7;void*_tmp9B9;
void**_tmp9BA;struct Cyc_Absyn_Exp*_tmp9BB;struct _tuple7*_tmp9BD;struct _tuple7
_tmp9BE;void*_tmp9BF;void**_tmp9C0;struct Cyc_List_List*_tmp9C1;struct Cyc_List_List*
_tmp9C3;struct Cyc_List_List*_tmp9C5;void*_tmp9C7;void**_tmp9C8;void*_tmp9CA;void**
_tmp9CB;struct Cyc_Absyn_Stmt*_tmp9CD;struct Cyc_Absyn_MallocInfo _tmp9CF;struct Cyc_Absyn_MallocInfo*
_tmp9D0;_LL3BB: {struct Cyc_Absyn_Deref_e_struct*_tmp98A=(struct Cyc_Absyn_Deref_e_struct*)
_tmp989;if(_tmp98A->tag != 21)goto _LL3BD;else{_tmp98B=_tmp98A->f1;}}_LL3BC:
_tmp98D=_tmp98B;goto _LL3BE;_LL3BD: {struct Cyc_Absyn_AggrMember_e_struct*_tmp98C=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp989;if(_tmp98C->tag != 22)goto _LL3BF;
else{_tmp98D=_tmp98C->f1;}}_LL3BE: _tmp98F=_tmp98D;goto _LL3C0;_LL3BF: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp98E=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp989;if(_tmp98E->tag != 23)goto
_LL3C1;else{_tmp98F=_tmp98E->f1;}}_LL3C0: _tmp991=_tmp98F;goto _LL3C2;_LL3C1: {
struct Cyc_Absyn_Address_e_struct*_tmp990=(struct Cyc_Absyn_Address_e_struct*)
_tmp989;if(_tmp990->tag != 16)goto _LL3C3;else{_tmp991=_tmp990->f1;}}_LL3C2:
_tmp993=_tmp991;goto _LL3C4;_LL3C3: {struct Cyc_Absyn_Throw_e_struct*_tmp992=(
struct Cyc_Absyn_Throw_e_struct*)_tmp989;if(_tmp992->tag != 12)goto _LL3C5;else{
_tmp993=_tmp992->f1;}}_LL3C4: _tmp995=_tmp993;goto _LL3C6;_LL3C5: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp994=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp989;if(_tmp994->tag != 13)
goto _LL3C7;else{_tmp995=_tmp994->f1;}}_LL3C6: _tmp997=_tmp995;goto _LL3C8;_LL3C7: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp996=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp989;if(_tmp996->tag != 19)goto _LL3C9;else{_tmp997=_tmp996->f1;}}_LL3C8:
_tmp999=_tmp997;goto _LL3CA;_LL3C9: {struct Cyc_Absyn_Increment_e_struct*_tmp998=(
struct Cyc_Absyn_Increment_e_struct*)_tmp989;if(_tmp998->tag != 5)goto _LL3CB;else{
_tmp999=_tmp998->f1;}}_LL3CA: Cyc_Toc_exptypes_to_c(_tmp999);goto _LL3BA;_LL3CB: {
struct Cyc_Absyn_Primop_e_struct*_tmp99A=(struct Cyc_Absyn_Primop_e_struct*)
_tmp989;if(_tmp99A->tag != 3)goto _LL3CD;else{_tmp99B=_tmp99A->f2;}}_LL3CC:((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,
_tmp99B);goto _LL3BA;_LL3CD: {struct Cyc_Absyn_And_e_struct*_tmp99C=(struct Cyc_Absyn_And_e_struct*)
_tmp989;if(_tmp99C->tag != 7)goto _LL3CF;else{_tmp99D=_tmp99C->f1;_tmp99E=_tmp99C->f2;}}
_LL3CE: _tmp9A0=_tmp99D;_tmp9A1=_tmp99E;goto _LL3D0;_LL3CF: {struct Cyc_Absyn_Or_e_struct*
_tmp99F=(struct Cyc_Absyn_Or_e_struct*)_tmp989;if(_tmp99F->tag != 8)goto _LL3D1;
else{_tmp9A0=_tmp99F->f1;_tmp9A1=_tmp99F->f2;}}_LL3D0: _tmp9A3=_tmp9A0;_tmp9A4=
_tmp9A1;goto _LL3D2;_LL3D1: {struct Cyc_Absyn_SeqExp_e_struct*_tmp9A2=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp989;if(_tmp9A2->tag != 9)goto _LL3D3;else{_tmp9A3=_tmp9A2->f1;_tmp9A4=_tmp9A2->f2;}}
_LL3D2: _tmp9A6=_tmp9A3;_tmp9A7=_tmp9A4;goto _LL3D4;_LL3D3: {struct Cyc_Absyn_Subscript_e_struct*
_tmp9A5=(struct Cyc_Absyn_Subscript_e_struct*)_tmp989;if(_tmp9A5->tag != 24)goto
_LL3D5;else{_tmp9A6=_tmp9A5->f1;_tmp9A7=_tmp9A5->f2;}}_LL3D4: _tmp9A9=_tmp9A6;
_tmp9AA=_tmp9A7;goto _LL3D6;_LL3D5: {struct Cyc_Absyn_Swap_e_struct*_tmp9A8=(
struct Cyc_Absyn_Swap_e_struct*)_tmp989;if(_tmp9A8->tag != 35)goto _LL3D7;else{
_tmp9A9=_tmp9A8->f1;_tmp9AA=_tmp9A8->f2;}}_LL3D6: _tmp9AC=_tmp9A9;_tmp9AD=_tmp9AA;
goto _LL3D8;_LL3D7: {struct Cyc_Absyn_AssignOp_e_struct*_tmp9AB=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp989;if(_tmp9AB->tag != 4)goto _LL3D9;else{_tmp9AC=_tmp9AB->f1;_tmp9AD=_tmp9AB->f3;}}
_LL3D8: Cyc_Toc_exptypes_to_c(_tmp9AC);Cyc_Toc_exptypes_to_c(_tmp9AD);goto _LL3BA;
_LL3D9: {struct Cyc_Absyn_Conditional_e_struct*_tmp9AE=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp989;if(_tmp9AE->tag != 6)goto _LL3DB;else{_tmp9AF=_tmp9AE->f1;_tmp9B0=_tmp9AE->f2;
_tmp9B1=_tmp9AE->f3;}}_LL3DA: Cyc_Toc_exptypes_to_c(_tmp9AF);Cyc_Toc_exptypes_to_c(
_tmp9B0);Cyc_Toc_exptypes_to_c(_tmp9B1);goto _LL3BA;_LL3DB: {struct Cyc_Absyn_FnCall_e_struct*
_tmp9B2=(struct Cyc_Absyn_FnCall_e_struct*)_tmp989;if(_tmp9B2->tag != 11)goto
_LL3DD;else{_tmp9B3=_tmp9B2->f1;_tmp9B4=_tmp9B2->f2;}}_LL3DC: _tmp9B6=_tmp9B3;
_tmp9B7=_tmp9B4;goto _LL3DE;_LL3DD: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp9B5=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp989;if(_tmp9B5->tag != 10)goto _LL3DF;
else{_tmp9B6=_tmp9B5->f1;_tmp9B7=_tmp9B5->f2;}}_LL3DE: Cyc_Toc_exptypes_to_c(
_tmp9B6);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp9B7);goto _LL3BA;_LL3DF: {struct Cyc_Absyn_Cast_e_struct*
_tmp9B8=(struct Cyc_Absyn_Cast_e_struct*)_tmp989;if(_tmp9B8->tag != 15)goto _LL3E1;
else{_tmp9B9=(void**)& _tmp9B8->f1;_tmp9BA=(void**)((void**)& _tmp9B8->f1);_tmp9BB=
_tmp9B8->f2;}}_LL3E0:*_tmp9BA=Cyc_Toc_typ_to_c(*_tmp9BA);Cyc_Toc_exptypes_to_c(
_tmp9BB);goto _LL3BA;_LL3E1: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp9BC=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp989;if(_tmp9BC->tag != 26)goto _LL3E3;
else{_tmp9BD=_tmp9BC->f1;_tmp9BE=*_tmp9BD;_tmp9BF=_tmp9BE.f3;_tmp9C0=(void**)&(*
_tmp9BC->f1).f3;_tmp9C1=_tmp9BC->f2;}}_LL3E2:*_tmp9C0=Cyc_Toc_typ_to_c(*_tmp9C0);
_tmp9C3=_tmp9C1;goto _LL3E4;_LL3E3: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp9C2=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp989;if(_tmp9C2->tag != 36)
goto _LL3E5;else{_tmp9C3=_tmp9C2->f2;}}_LL3E4: _tmp9C5=_tmp9C3;goto _LL3E6;_LL3E5: {
struct Cyc_Absyn_Array_e_struct*_tmp9C4=(struct Cyc_Absyn_Array_e_struct*)_tmp989;
if(_tmp9C4->tag != 27)goto _LL3E7;else{_tmp9C5=_tmp9C4->f1;}}_LL3E6: for(0;_tmp9C5
!= 0;_tmp9C5=_tmp9C5->tl){struct Cyc_Absyn_Exp*_tmp9E1;struct _tuple15 _tmp9E0=*((
struct _tuple15*)_tmp9C5->hd);_tmp9E1=_tmp9E0.f2;Cyc_Toc_exptypes_to_c(_tmp9E1);}
goto _LL3BA;_LL3E7: {struct Cyc_Absyn_Offsetof_e_struct*_tmp9C6=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp989;if(_tmp9C6->tag != 20)goto _LL3E9;else{_tmp9C7=(void**)& _tmp9C6->f1;
_tmp9C8=(void**)((void**)& _tmp9C6->f1);}}_LL3E8: _tmp9CB=_tmp9C8;goto _LL3EA;
_LL3E9: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp9C9=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp989;if(_tmp9C9->tag != 18)goto _LL3EB;else{_tmp9CA=(void**)& _tmp9C9->f1;
_tmp9CB=(void**)((void**)& _tmp9C9->f1);}}_LL3EA:*_tmp9CB=Cyc_Toc_typ_to_c(*
_tmp9CB);goto _LL3BA;_LL3EB: {struct Cyc_Absyn_StmtExp_e_struct*_tmp9CC=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp989;if(_tmp9CC->tag != 37)goto _LL3ED;else{_tmp9CD=_tmp9CC->f1;}}_LL3EC: Cyc_Toc_stmttypes_to_c(
_tmp9CD);goto _LL3BA;_LL3ED: {struct Cyc_Absyn_Malloc_e_struct*_tmp9CE=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp989;if(_tmp9CE->tag != 34)goto _LL3EF;else{_tmp9CF=_tmp9CE->f1;_tmp9D0=(struct
Cyc_Absyn_MallocInfo*)& _tmp9CE->f1;}}_LL3EE: if(_tmp9D0->elt_type != 0){void**
_tmpE21;_tmp9D0->elt_type=((_tmpE21=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE21)),((_tmpE21[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp9D0->elt_type))),
_tmpE21))));}Cyc_Toc_exptypes_to_c(_tmp9D0->num_elts);goto _LL3BA;_LL3EF: {struct
Cyc_Absyn_Const_e_struct*_tmp9D1=(struct Cyc_Absyn_Const_e_struct*)_tmp989;if(
_tmp9D1->tag != 0)goto _LL3F1;}_LL3F0: goto _LL3F2;_LL3F1: {struct Cyc_Absyn_Var_e_struct*
_tmp9D2=(struct Cyc_Absyn_Var_e_struct*)_tmp989;if(_tmp9D2->tag != 1)goto _LL3F3;}
_LL3F2: goto _LL3F4;_LL3F3: {struct Cyc_Absyn_UnknownId_e_struct*_tmp9D3=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp989;if(_tmp9D3->tag != 2)goto _LL3F5;}_LL3F4: goto _LL3F6;_LL3F5: {struct Cyc_Absyn_Enum_e_struct*
_tmp9D4=(struct Cyc_Absyn_Enum_e_struct*)_tmp989;if(_tmp9D4->tag != 32)goto _LL3F7;}
_LL3F6: goto _LL3F8;_LL3F7: {struct Cyc_Absyn_Asm_e_struct*_tmp9D5=(struct Cyc_Absyn_Asm_e_struct*)
_tmp989;if(_tmp9D5->tag != 40)goto _LL3F9;}_LL3F8: goto _LL3FA;_LL3F9: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp9D6=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp989;if(_tmp9D6->tag != 33)goto
_LL3FB;}_LL3FA: goto _LL3BA;_LL3FB: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp9D7=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp989;if(_tmp9D7->tag != 30)goto _LL3FD;}
_LL3FC: goto _LL3FE;_LL3FD: {struct Cyc_Absyn_Datatype_e_struct*_tmp9D8=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp989;if(_tmp9D8->tag != 31)goto _LL3FF;}_LL3FE: goto _LL400;_LL3FF: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp9D9=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp989;if(_tmp9D9->tag != 29)goto
_LL401;}_LL400: goto _LL402;_LL401: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp9DA=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp989;if(_tmp9DA->tag != 28)
goto _LL403;}_LL402: goto _LL404;_LL403: {struct Cyc_Absyn_Tuple_e_struct*_tmp9DB=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp989;if(_tmp9DB->tag != 25)goto _LL405;}_LL404:
goto _LL406;_LL405: {struct Cyc_Absyn_Instantiate_e_struct*_tmp9DC=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp989;if(_tmp9DC->tag != 14)goto _LL407;}_LL406: goto _LL408;_LL407: {struct Cyc_Absyn_New_e_struct*
_tmp9DD=(struct Cyc_Absyn_New_e_struct*)_tmp989;if(_tmp9DD->tag != 17)goto _LL409;}
_LL408: goto _LL40A;_LL409: {struct Cyc_Absyn_Valueof_e_struct*_tmp9DE=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp989;if(_tmp9DE->tag != 39)goto _LL40B;}_LL40A: goto _LL40C;_LL40B: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp9DF=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp989;if(_tmp9DF->tag != 38)goto
_LL3BA;}_LL40C:{const char*_tmpE24;void*_tmpE23;(_tmpE23=0,Cyc_Tcutil_terr(e->loc,((
_tmpE24="Cyclone expression within C code",_tag_dyneither(_tmpE24,sizeof(char),
33))),_tag_dyneither(_tmpE23,sizeof(void*),0)));}goto _LL3BA;_LL3BA:;}static void
Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){void*_tmp9E5=d->r;struct Cyc_Absyn_Vardecl*
_tmp9E7;struct Cyc_Absyn_Fndecl*_tmp9E9;struct Cyc_Absyn_Aggrdecl*_tmp9EB;struct
Cyc_Absyn_Enumdecl*_tmp9ED;struct Cyc_Absyn_Typedefdecl*_tmp9EF;_LL40E: {struct
Cyc_Absyn_Var_d_struct*_tmp9E6=(struct Cyc_Absyn_Var_d_struct*)_tmp9E5;if(_tmp9E6->tag
!= 0)goto _LL410;else{_tmp9E7=_tmp9E6->f1;}}_LL40F: _tmp9E7->type=Cyc_Toc_typ_to_c(
_tmp9E7->type);if(_tmp9E7->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp9E7->initializer));goto _LL40D;_LL410: {struct Cyc_Absyn_Fn_d_struct*
_tmp9E8=(struct Cyc_Absyn_Fn_d_struct*)_tmp9E5;if(_tmp9E8->tag != 1)goto _LL412;
else{_tmp9E9=_tmp9E8->f1;}}_LL411: _tmp9E9->ret_type=Cyc_Toc_typ_to_c(_tmp9E9->ret_type);{
struct Cyc_List_List*_tmp9FB=_tmp9E9->args;for(0;_tmp9FB != 0;_tmp9FB=_tmp9FB->tl){(*((
struct _tuple25*)_tmp9FB->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple25*)_tmp9FB->hd)).f3);}}
goto _LL40D;_LL412: {struct Cyc_Absyn_Aggr_d_struct*_tmp9EA=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp9E5;if(_tmp9EA->tag != 6)goto _LL414;else{_tmp9EB=_tmp9EA->f1;}}_LL413: Cyc_Toc_aggrdecl_to_c(
_tmp9EB,1);goto _LL40D;_LL414: {struct Cyc_Absyn_Enum_d_struct*_tmp9EC=(struct Cyc_Absyn_Enum_d_struct*)
_tmp9E5;if(_tmp9EC->tag != 8)goto _LL416;else{_tmp9ED=_tmp9EC->f1;}}_LL415: if(
_tmp9ED->fields != 0){struct Cyc_List_List*_tmp9FC=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp9ED->fields))->v;for(0;_tmp9FC != 0;_tmp9FC=_tmp9FC->tl){
struct Cyc_Absyn_Enumfield*_tmp9FD=(struct Cyc_Absyn_Enumfield*)_tmp9FC->hd;if(
_tmp9FD->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9FD->tag));}}
goto _LL40D;_LL416: {struct Cyc_Absyn_Typedef_d_struct*_tmp9EE=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp9E5;if(_tmp9EE->tag != 9)goto _LL418;else{_tmp9EF=_tmp9EE->f1;}}_LL417:{struct
Cyc_Core_Opt*_tmpE25;_tmp9EF->defn=((_tmpE25=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE25)),((_tmpE25->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp9EF->defn))->v),_tmpE25))));}goto _LL40D;_LL418: {struct Cyc_Absyn_Let_d_struct*
_tmp9F0=(struct Cyc_Absyn_Let_d_struct*)_tmp9E5;if(_tmp9F0->tag != 2)goto _LL41A;}
_LL419: goto _LL41B;_LL41A: {struct Cyc_Absyn_Letv_d_struct*_tmp9F1=(struct Cyc_Absyn_Letv_d_struct*)
_tmp9E5;if(_tmp9F1->tag != 3)goto _LL41C;}_LL41B: goto _LL41D;_LL41C: {struct Cyc_Absyn_Datatype_d_struct*
_tmp9F2=(struct Cyc_Absyn_Datatype_d_struct*)_tmp9E5;if(_tmp9F2->tag != 7)goto
_LL41E;}_LL41D: goto _LL41F;_LL41E: {struct Cyc_Absyn_Namespace_d_struct*_tmp9F3=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp9E5;if(_tmp9F3->tag != 10)goto _LL420;}
_LL41F: goto _LL421;_LL420: {struct Cyc_Absyn_Using_d_struct*_tmp9F4=(struct Cyc_Absyn_Using_d_struct*)
_tmp9E5;if(_tmp9F4->tag != 11)goto _LL422;}_LL421: goto _LL423;_LL422: {struct Cyc_Absyn_ExternC_d_struct*
_tmp9F5=(struct Cyc_Absyn_ExternC_d_struct*)_tmp9E5;if(_tmp9F5->tag != 12)goto
_LL424;}_LL423: goto _LL425;_LL424: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp9F6=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9E5;if(_tmp9F6->tag != 13)
goto _LL426;}_LL425: goto _LL427;_LL426: {struct Cyc_Absyn_Region_d_struct*_tmp9F7=(
struct Cyc_Absyn_Region_d_struct*)_tmp9E5;if(_tmp9F7->tag != 4)goto _LL428;}_LL427:
goto _LL429;_LL428: {struct Cyc_Absyn_Alias_d_struct*_tmp9F8=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9E5;if(_tmp9F8->tag != 5)goto _LL42A;}_LL429:{const char*_tmpE28;void*_tmpE27;(
_tmpE27=0,Cyc_Tcutil_terr(d->loc,((_tmpE28="Cyclone declaration within C code",
_tag_dyneither(_tmpE28,sizeof(char),34))),_tag_dyneither(_tmpE27,sizeof(void*),0)));}
goto _LL40D;_LL42A: {struct Cyc_Absyn_Porton_d_struct*_tmp9F9=(struct Cyc_Absyn_Porton_d_struct*)
_tmp9E5;if(_tmp9F9->tag != 14)goto _LL42C;}_LL42B: goto _LL42D;_LL42C: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9FA=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9E5;if(_tmp9FA->tag != 15)goto
_LL40D;}_LL42D: goto _LL40D;_LL40D:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s){void*_tmpA01=s->r;struct Cyc_Absyn_Exp*_tmpA03;struct Cyc_Absyn_Stmt*_tmpA05;
struct Cyc_Absyn_Stmt*_tmpA06;struct Cyc_Absyn_Exp*_tmpA08;struct Cyc_Absyn_Exp*
_tmpA0A;struct Cyc_Absyn_Stmt*_tmpA0B;struct Cyc_Absyn_Stmt*_tmpA0C;struct _tuple8
_tmpA0E;struct Cyc_Absyn_Exp*_tmpA0F;struct Cyc_Absyn_Stmt*_tmpA10;struct Cyc_Absyn_Exp*
_tmpA12;struct _tuple8 _tmpA13;struct Cyc_Absyn_Exp*_tmpA14;struct _tuple8 _tmpA15;
struct Cyc_Absyn_Exp*_tmpA16;struct Cyc_Absyn_Stmt*_tmpA17;struct Cyc_Absyn_Exp*
_tmpA19;struct Cyc_List_List*_tmpA1A;struct Cyc_Absyn_Decl*_tmpA1C;struct Cyc_Absyn_Stmt*
_tmpA1D;struct Cyc_Absyn_Stmt*_tmpA1F;struct _tuple8 _tmpA20;struct Cyc_Absyn_Exp*
_tmpA21;_LL42F: {struct Cyc_Absyn_Exp_s_struct*_tmpA02=(struct Cyc_Absyn_Exp_s_struct*)
_tmpA01;if(_tmpA02->tag != 1)goto _LL431;else{_tmpA03=_tmpA02->f1;}}_LL430: Cyc_Toc_exptypes_to_c(
_tmpA03);goto _LL42E;_LL431: {struct Cyc_Absyn_Seq_s_struct*_tmpA04=(struct Cyc_Absyn_Seq_s_struct*)
_tmpA01;if(_tmpA04->tag != 2)goto _LL433;else{_tmpA05=_tmpA04->f1;_tmpA06=_tmpA04->f2;}}
_LL432: Cyc_Toc_stmttypes_to_c(_tmpA05);Cyc_Toc_stmttypes_to_c(_tmpA06);goto
_LL42E;_LL433: {struct Cyc_Absyn_Return_s_struct*_tmpA07=(struct Cyc_Absyn_Return_s_struct*)
_tmpA01;if(_tmpA07->tag != 3)goto _LL435;else{_tmpA08=_tmpA07->f1;}}_LL434: if(
_tmpA08 != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmpA08);goto _LL42E;
_LL435: {struct Cyc_Absyn_IfThenElse_s_struct*_tmpA09=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmpA01;if(_tmpA09->tag != 4)goto _LL437;else{_tmpA0A=_tmpA09->f1;_tmpA0B=_tmpA09->f2;
_tmpA0C=_tmpA09->f3;}}_LL436: Cyc_Toc_exptypes_to_c(_tmpA0A);Cyc_Toc_stmttypes_to_c(
_tmpA0B);Cyc_Toc_stmttypes_to_c(_tmpA0C);goto _LL42E;_LL437: {struct Cyc_Absyn_While_s_struct*
_tmpA0D=(struct Cyc_Absyn_While_s_struct*)_tmpA01;if(_tmpA0D->tag != 5)goto _LL439;
else{_tmpA0E=_tmpA0D->f1;_tmpA0F=_tmpA0E.f1;_tmpA10=_tmpA0D->f2;}}_LL438: Cyc_Toc_exptypes_to_c(
_tmpA0F);Cyc_Toc_stmttypes_to_c(_tmpA10);goto _LL42E;_LL439: {struct Cyc_Absyn_For_s_struct*
_tmpA11=(struct Cyc_Absyn_For_s_struct*)_tmpA01;if(_tmpA11->tag != 9)goto _LL43B;
else{_tmpA12=_tmpA11->f1;_tmpA13=_tmpA11->f2;_tmpA14=_tmpA13.f1;_tmpA15=_tmpA11->f3;
_tmpA16=_tmpA15.f1;_tmpA17=_tmpA11->f4;}}_LL43A: Cyc_Toc_exptypes_to_c(_tmpA12);
Cyc_Toc_exptypes_to_c(_tmpA14);Cyc_Toc_exptypes_to_c(_tmpA16);Cyc_Toc_stmttypes_to_c(
_tmpA17);goto _LL42E;_LL43B: {struct Cyc_Absyn_Switch_s_struct*_tmpA18=(struct Cyc_Absyn_Switch_s_struct*)
_tmpA01;if(_tmpA18->tag != 10)goto _LL43D;else{_tmpA19=_tmpA18->f1;_tmpA1A=_tmpA18->f2;}}
_LL43C: Cyc_Toc_exptypes_to_c(_tmpA19);for(0;_tmpA1A != 0;_tmpA1A=_tmpA1A->tl){Cyc_Toc_stmttypes_to_c(((
struct Cyc_Absyn_Switch_clause*)_tmpA1A->hd)->body);}goto _LL42E;_LL43D: {struct
Cyc_Absyn_Decl_s_struct*_tmpA1B=(struct Cyc_Absyn_Decl_s_struct*)_tmpA01;if(
_tmpA1B->tag != 12)goto _LL43F;else{_tmpA1C=_tmpA1B->f1;_tmpA1D=_tmpA1B->f2;}}
_LL43E: Cyc_Toc_decltypes_to_c(_tmpA1C);Cyc_Toc_stmttypes_to_c(_tmpA1D);goto
_LL42E;_LL43F: {struct Cyc_Absyn_Do_s_struct*_tmpA1E=(struct Cyc_Absyn_Do_s_struct*)
_tmpA01;if(_tmpA1E->tag != 14)goto _LL441;else{_tmpA1F=_tmpA1E->f1;_tmpA20=_tmpA1E->f2;
_tmpA21=_tmpA20.f1;}}_LL440: Cyc_Toc_stmttypes_to_c(_tmpA1F);Cyc_Toc_exptypes_to_c(
_tmpA21);goto _LL42E;_LL441: {struct Cyc_Absyn_Skip_s_struct*_tmpA22=(struct Cyc_Absyn_Skip_s_struct*)
_tmpA01;if(_tmpA22->tag != 0)goto _LL443;}_LL442: goto _LL444;_LL443: {struct Cyc_Absyn_Break_s_struct*
_tmpA23=(struct Cyc_Absyn_Break_s_struct*)_tmpA01;if(_tmpA23->tag != 6)goto _LL445;}
_LL444: goto _LL446;_LL445: {struct Cyc_Absyn_Continue_s_struct*_tmpA24=(struct Cyc_Absyn_Continue_s_struct*)
_tmpA01;if(_tmpA24->tag != 7)goto _LL447;}_LL446: goto _LL448;_LL447: {struct Cyc_Absyn_Goto_s_struct*
_tmpA25=(struct Cyc_Absyn_Goto_s_struct*)_tmpA01;if(_tmpA25->tag != 8)goto _LL449;}
_LL448: goto _LL42E;_LL449: {struct Cyc_Absyn_Fallthru_s_struct*_tmpA26=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmpA01;if(_tmpA26->tag != 11)goto _LL44B;}_LL44A: goto _LL44C;_LL44B: {struct Cyc_Absyn_Label_s_struct*
_tmpA27=(struct Cyc_Absyn_Label_s_struct*)_tmpA01;if(_tmpA27->tag != 13)goto _LL44D;}
_LL44C: goto _LL44E;_LL44D: {struct Cyc_Absyn_TryCatch_s_struct*_tmpA28=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmpA01;if(_tmpA28->tag != 15)goto _LL44F;}_LL44E: goto _LL450;_LL44F: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmpA29=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmpA01;if(_tmpA29->tag != 16)goto
_LL42E;}_LL450:{const char*_tmpE2B;void*_tmpE2A;(_tmpE2A=0,Cyc_Tcutil_terr(s->loc,((
_tmpE2B="Cyclone statement in C code",_tag_dyneither(_tmpE2B,sizeof(char),28))),
_tag_dyneither(_tmpE2A,sizeof(void*),0)));}goto _LL42E;_LL42E:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude){for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpE2E;void*_tmpE2D;(_tmpE2D=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE2E="decls_to_c: not at toplevel!",
_tag_dyneither(_tmpE2E,sizeof(char),29))),_tag_dyneither(_tmpE2D,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmpA2E=d->r;struct Cyc_Absyn_Vardecl*
_tmpA30;struct Cyc_Absyn_Fndecl*_tmpA32;struct Cyc_Absyn_Aggrdecl*_tmpA38;struct
Cyc_Absyn_Datatypedecl*_tmpA3A;struct Cyc_Absyn_Enumdecl*_tmpA3C;struct Cyc_Absyn_Typedefdecl*
_tmpA3E;struct Cyc_List_List*_tmpA42;struct Cyc_List_List*_tmpA44;struct Cyc_List_List*
_tmpA46;struct Cyc_List_List*_tmpA48;_LL452: {struct Cyc_Absyn_Var_d_struct*
_tmpA2F=(struct Cyc_Absyn_Var_d_struct*)_tmpA2E;if(_tmpA2F->tag != 0)goto _LL454;
else{_tmpA30=_tmpA2F->f1;}}_LL453: {struct _tuple0*_tmpA49=_tmpA30->name;if(
_tmpA30->sc == Cyc_Absyn_ExternC){struct _tuple0*_tmpE2F;_tmpA49=((_tmpE2F=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE2F)),((_tmpE2F->f1=Cyc_Absyn_Rel_n(
0),((_tmpE2F->f2=(*_tmpA49).f2,_tmpE2F))))));}if(_tmpA30->initializer != 0){if(
cinclude)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmpA30->initializer));
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmpA30->initializer));}}{
struct Cyc_Absyn_Global_b_struct _tmpE32;struct Cyc_Absyn_Global_b_struct*_tmpE31;
nv=Cyc_Toc_add_varmap(r,nv,_tmpA30->name,Cyc_Absyn_varb_exp(_tmpA49,(void*)((
_tmpE31=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE31)),((_tmpE31[0]=((
_tmpE32.tag=1,((_tmpE32.f1=_tmpA30,_tmpE32)))),_tmpE31)))),0));}_tmpA30->name=
_tmpA49;_tmpA30->sc=Cyc_Toc_scope_to_c(_tmpA30->sc);_tmpA30->type=Cyc_Toc_typ_to_c_array(
_tmpA30->type);{struct Cyc_List_List*_tmpE33;Cyc_Toc_result_decls=((_tmpE33=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE33)),((_tmpE33->hd=d,((
_tmpE33->tl=Cyc_Toc_result_decls,_tmpE33))))));}goto _LL451;}_LL454: {struct Cyc_Absyn_Fn_d_struct*
_tmpA31=(struct Cyc_Absyn_Fn_d_struct*)_tmpA2E;if(_tmpA31->tag != 1)goto _LL456;
else{_tmpA32=_tmpA31->f1;}}_LL455: {struct _tuple0*_tmpA4E=_tmpA32->name;if(
_tmpA32->sc == Cyc_Absyn_ExternC){{struct _tuple0*_tmpE34;_tmpA4E=((_tmpE34=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE34)),((_tmpE34->f1=Cyc_Absyn_Rel_n(
0),((_tmpE34->f2=(*_tmpA4E).f2,_tmpE34))))));}_tmpA32->sc=Cyc_Absyn_Public;}nv=
Cyc_Toc_add_varmap(r,nv,_tmpA32->name,Cyc_Absyn_var_exp(_tmpA4E,0));_tmpA32->name=
_tmpA4E;Cyc_Toc_fndecl_to_c(nv,_tmpA32,cinclude);{struct Cyc_List_List*_tmpE35;
Cyc_Toc_result_decls=((_tmpE35=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpE35)),((_tmpE35->hd=d,((_tmpE35->tl=Cyc_Toc_result_decls,_tmpE35))))));}goto
_LL451;}_LL456: {struct Cyc_Absyn_Let_d_struct*_tmpA33=(struct Cyc_Absyn_Let_d_struct*)
_tmpA2E;if(_tmpA33->tag != 2)goto _LL458;}_LL457: goto _LL459;_LL458: {struct Cyc_Absyn_Letv_d_struct*
_tmpA34=(struct Cyc_Absyn_Letv_d_struct*)_tmpA2E;if(_tmpA34->tag != 3)goto _LL45A;}
_LL459: {const char*_tmpE38;void*_tmpE37;(_tmpE37=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE38="letdecl at toplevel",
_tag_dyneither(_tmpE38,sizeof(char),20))),_tag_dyneither(_tmpE37,sizeof(void*),0)));}
_LL45A: {struct Cyc_Absyn_Region_d_struct*_tmpA35=(struct Cyc_Absyn_Region_d_struct*)
_tmpA2E;if(_tmpA35->tag != 4)goto _LL45C;}_LL45B: {const char*_tmpE3B;void*_tmpE3A;(
_tmpE3A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE3B="region decl at toplevel",_tag_dyneither(_tmpE3B,sizeof(char),24))),
_tag_dyneither(_tmpE3A,sizeof(void*),0)));}_LL45C: {struct Cyc_Absyn_Alias_d_struct*
_tmpA36=(struct Cyc_Absyn_Alias_d_struct*)_tmpA2E;if(_tmpA36->tag != 5)goto _LL45E;}
_LL45D: {const char*_tmpE3E;void*_tmpE3D;(_tmpE3D=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE3E="alias decl at toplevel",
_tag_dyneither(_tmpE3E,sizeof(char),23))),_tag_dyneither(_tmpE3D,sizeof(void*),0)));}
_LL45E: {struct Cyc_Absyn_Aggr_d_struct*_tmpA37=(struct Cyc_Absyn_Aggr_d_struct*)
_tmpA2E;if(_tmpA37->tag != 6)goto _LL460;else{_tmpA38=_tmpA37->f1;}}_LL45F: Cyc_Toc_aggrdecl_to_c(
_tmpA38,1);goto _LL451;_LL460: {struct Cyc_Absyn_Datatype_d_struct*_tmpA39=(struct
Cyc_Absyn_Datatype_d_struct*)_tmpA2E;if(_tmpA39->tag != 7)goto _LL462;else{_tmpA3A=
_tmpA39->f1;}}_LL461: if(_tmpA3A->is_extensible)Cyc_Toc_xdatatypedecl_to_c(
_tmpA3A);else{Cyc_Toc_datatypedecl_to_c(_tmpA3A);}goto _LL451;_LL462: {struct Cyc_Absyn_Enum_d_struct*
_tmpA3B=(struct Cyc_Absyn_Enum_d_struct*)_tmpA2E;if(_tmpA3B->tag != 8)goto _LL464;
else{_tmpA3C=_tmpA3B->f1;}}_LL463: Cyc_Toc_enumdecl_to_c(_tmpA3C);{struct Cyc_List_List*
_tmpE3F;Cyc_Toc_result_decls=((_tmpE3F=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE3F)),((_tmpE3F->hd=d,((_tmpE3F->tl=Cyc_Toc_result_decls,_tmpE3F))))));}
goto _LL451;_LL464: {struct Cyc_Absyn_Typedef_d_struct*_tmpA3D=(struct Cyc_Absyn_Typedef_d_struct*)
_tmpA2E;if(_tmpA3D->tag != 9)goto _LL466;else{_tmpA3E=_tmpA3D->f1;}}_LL465: _tmpA3E->name=
_tmpA3E->name;_tmpA3E->tvs=0;if(_tmpA3E->defn != 0){struct Cyc_Core_Opt*_tmpE40;
_tmpA3E->defn=((_tmpE40=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE40)),((
_tmpE40->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA3E->defn))->v),_tmpE40))));}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)
_check_null(_tmpA3E->kind))->v)->kind){case Cyc_Absyn_BoxKind: _LL472:{struct Cyc_Core_Opt*
_tmpE41;_tmpA3E->defn=((_tmpE41=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpE41)),((_tmpE41->v=Cyc_Absyn_void_star_typ(),_tmpE41))));}break;default:
_LL473:{struct Cyc_Core_Opt*_tmpE42;_tmpA3E->defn=((_tmpE42=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE42)),((_tmpE42->v=(void*)& Cyc_Absyn_VoidType_val,_tmpE42))));}
break;}}{struct Cyc_List_List*_tmpE43;Cyc_Toc_result_decls=((_tmpE43=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE43)),((_tmpE43->hd=d,((
_tmpE43->tl=Cyc_Toc_result_decls,_tmpE43))))));}goto _LL451;_LL466: {struct Cyc_Absyn_Porton_d_struct*
_tmpA3F=(struct Cyc_Absyn_Porton_d_struct*)_tmpA2E;if(_tmpA3F->tag != 14)goto
_LL468;}_LL467: goto _LL469;_LL468: {struct Cyc_Absyn_Portoff_d_struct*_tmpA40=(
struct Cyc_Absyn_Portoff_d_struct*)_tmpA2E;if(_tmpA40->tag != 15)goto _LL46A;}
_LL469: goto _LL451;_LL46A: {struct Cyc_Absyn_Namespace_d_struct*_tmpA41=(struct Cyc_Absyn_Namespace_d_struct*)
_tmpA2E;if(_tmpA41->tag != 10)goto _LL46C;else{_tmpA42=_tmpA41->f2;}}_LL46B:
_tmpA44=_tmpA42;goto _LL46D;_LL46C: {struct Cyc_Absyn_Using_d_struct*_tmpA43=(
struct Cyc_Absyn_Using_d_struct*)_tmpA2E;if(_tmpA43->tag != 11)goto _LL46E;else{
_tmpA44=_tmpA43->f2;}}_LL46D: _tmpA46=_tmpA44;goto _LL46F;_LL46E: {struct Cyc_Absyn_ExternC_d_struct*
_tmpA45=(struct Cyc_Absyn_ExternC_d_struct*)_tmpA2E;if(_tmpA45->tag != 12)goto
_LL470;else{_tmpA46=_tmpA45->f1;}}_LL46F: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA46,top,
cinclude);goto _LL451;_LL470: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA47=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmpA2E;if(_tmpA47->tag != 13)goto _LL451;
else{_tmpA48=_tmpA47->f1;}}_LL471: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA48,top,1);goto
_LL451;_LL451:;};}return nv;}static void Cyc_Toc_init(){struct _DynRegionHandle*
_tmpA5D;struct Cyc_Core_NewRegion _tmpA5C=Cyc_Core__new_dynregion((const char*)"internal-error",
0);_tmpA5D=_tmpA5C.dynregion;{struct _DynRegionFrame _tmpA5E;struct _RegionHandle*d=
_open_dynregion(& _tmpA5E,_tmpA5D);{struct Cyc_Dict_Dict*_tmpE4E;struct Cyc_Dict_Dict*
_tmpE4D;struct Cyc_Set_Set**_tmpE4C;struct Cyc_Dict_Dict*_tmpE4B;struct Cyc_List_List**
_tmpE4A;struct Cyc_Toc_TocState*_tmpE49;Cyc_Toc_toc_state=((_tmpE49=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE49)),((_tmpE49->dyn=(struct
_DynRegionHandle*)_tmpA5D,((_tmpE49->tuple_types=(struct Cyc_List_List**)((
_tmpE4A=_region_malloc(d,sizeof(*_tmpE4A)),((_tmpE4A[0]=0,_tmpE4A)))),((_tmpE49->aggrs_so_far=(
struct Cyc_Dict_Dict*)((_tmpE4B=_region_malloc(d,sizeof(*_tmpE4B)),((_tmpE4B[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE4B)))),((_tmpE49->datatypes_so_far=(
struct Cyc_Set_Set**)((_tmpE4C=_region_malloc(d,sizeof(*_tmpE4C)),((_tmpE4C[0]=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE4C)))),((_tmpE49->xdatatypes_so_far=(
struct Cyc_Dict_Dict*)((_tmpE4D=_region_malloc(d,sizeof(*_tmpE4D)),((_tmpE4D[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE4D)))),((_tmpE49->qvar_tags=(
struct Cyc_Dict_Dict*)((_tmpE4E=_region_malloc(d,sizeof(*_tmpE4E)),((_tmpE4E[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple13*,struct
_tuple13*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpE4E)))),((_tmpE49->temp_labels=(
struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(
d),_tmpE49))))))))))))))));}Cyc_Toc_result_decls=0;Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=0;Cyc_Toc_total_bounds_checks=
0;Cyc_Toc_bounds_checks_eliminated=0;{struct _dyneither_ptr**_tmpE4F;Cyc_Toc_globals=
_tag_dyneither(((_tmpE4F=_region_malloc(Cyc_Core_unique_region,sizeof(struct
_dyneither_ptr*)* 41),((_tmpE4F[0]=& Cyc_Toc__throw_str,((_tmpE4F[1]=& Cyc_Toc_setjmp_str,((
_tmpE4F[2]=& Cyc_Toc__push_handler_str,((_tmpE4F[3]=& Cyc_Toc__pop_handler_str,((
_tmpE4F[4]=& Cyc_Toc__exn_thrown_str,((_tmpE4F[5]=& Cyc_Toc__npop_handler_str,((
_tmpE4F[6]=& Cyc_Toc__check_null_str,((_tmpE4F[7]=& Cyc_Toc__check_known_subscript_null_str,((
_tmpE4F[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpE4F[9]=& Cyc_Toc__check_dyneither_subscript_str,((
_tmpE4F[10]=& Cyc_Toc__dyneither_ptr_str,((_tmpE4F[11]=& Cyc_Toc__tag_dyneither_str,((
_tmpE4F[12]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpE4F[13]=& Cyc_Toc__untag_dyneither_ptr_str,((
_tmpE4F[14]=& Cyc_Toc__get_dyneither_size_str,((_tmpE4F[15]=& Cyc_Toc__get_zero_arr_size_str,((
_tmpE4F[16]=& Cyc_Toc__dyneither_ptr_plus_str,((_tmpE4F[17]=& Cyc_Toc__zero_arr_plus_str,((
_tmpE4F[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpE4F[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((
_tmpE4F[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpE4F[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((
_tmpE4F[22]=& Cyc_Toc__cycalloc_str,((_tmpE4F[23]=& Cyc_Toc__cyccalloc_str,((
_tmpE4F[24]=& Cyc_Toc__cycalloc_atomic_str,((_tmpE4F[25]=& Cyc_Toc__cyccalloc_atomic_str,((
_tmpE4F[26]=& Cyc_Toc__region_malloc_str,((_tmpE4F[27]=& Cyc_Toc__region_calloc_str,((
_tmpE4F[28]=& Cyc_Toc__check_times_str,((_tmpE4F[29]=& Cyc_Toc__new_region_str,((
_tmpE4F[30]=& Cyc_Toc__push_region_str,((_tmpE4F[31]=& Cyc_Toc__pop_region_str,((
_tmpE4F[32]=& Cyc_Toc__open_dynregion_str,((_tmpE4F[33]=& Cyc_Toc__push_dynregion_str,((
_tmpE4F[34]=& Cyc_Toc__pop_dynregion_str,((_tmpE4F[35]=& Cyc_Toc__reset_region_str,((
_tmpE4F[36]=& Cyc_Toc__throw_arraybounds_str,((_tmpE4F[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,((
_tmpE4F[38]=& Cyc_Toc__throw_match_str,((_tmpE4F[39]=& Cyc_Toc__swap_word_str,((
_tmpE4F[40]=& Cyc_Toc__swap_dyneither_str,_tmpE4F)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),41);};;_pop_dynregion(d);};}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();{struct _RegionHandle _tmpA66=
_new_region("start");struct _RegionHandle*start=& _tmpA66;_push_region(start);Cyc_Toc_decls_to_c(
start,Cyc_Toc_empty_env(start),ds,1,0);{struct _DynRegionHandle*_tmpA68;struct Cyc_Toc_TocState
_tmpA67=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA68=
_tmpA67.dyn;Cyc_Core_free_dynregion(_tmpA68);};{struct Cyc_List_List*_tmpA69=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);
_npop_handler(0);return _tmpA69;};;_pop_region(start);};}
