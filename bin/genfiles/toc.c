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
struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int
tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};
struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6
val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct
_union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct
_union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct
_union_Cnst_Wstring_c Wstring_c;};union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,
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
ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const char*_tmpA9B;void*_tmpA9A;(_tmpA9A=0,
Cyc_fprintf(Cyc_stderr,((_tmpA9B="\n",_tag_dyneither(_tmpA9B,sizeof(char),2))),
_tag_dyneither(_tmpA9A,sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_struct _tmpA9E;struct Cyc_Toc_Toc_Unimplemented_struct*
_tmpA9D;(int)_throw((void*)((_tmpA9D=_cycalloc_atomic(sizeof(*_tmpA9D)),((
_tmpA9D[0]=((_tmpA9E.tag=Cyc_Toc_Toc_Unimplemented,_tmpA9E)),_tmpA9D)))));};}
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(
Cyc_stderr,fmt,ap);{const char*_tmpAA1;void*_tmpAA0;(_tmpAA0=0,Cyc_fprintf(Cyc_stderr,((
_tmpAA1="\n",_tag_dyneither(_tmpAA1,sizeof(char),2))),_tag_dyneither(_tmpAA0,
sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Impossible_struct
_tmpAA4;struct Cyc_Toc_Toc_Impossible_struct*_tmpAA3;(int)_throw((void*)((_tmpAA3=
_cycalloc_atomic(sizeof(*_tmpAA3)),((_tmpAA3[0]=((_tmpAA4.tag=Cyc_Toc_Toc_Impossible,
_tmpAA4)),_tmpAA3)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_struct{
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
_tmpAA5;skip_stmt_opt=((_tmpAA5=_cycalloc(sizeof(*_tmpAA5)),((_tmpAA5[0]=Cyc_Absyn_skip_stmt(
0),_tmpAA5))));}return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(
void*t,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,
0);}static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Cast_e_struct
_tmpAA8;struct Cyc_Absyn_Cast_e_struct*_tmpAA7;return(void*)((_tmpAA7=_cycalloc(
sizeof(*_tmpAA7)),((_tmpAA7[0]=((_tmpAA8.tag=15,((_tmpAA8.f1=(void*)t,((_tmpAA8.f2=
e,((_tmpAA8.f3=0,((_tmpAA8.f4=Cyc_Absyn_No_coercion,_tmpAA8)))))))))),_tmpAA7))));}
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Deref_e_struct
_tmpAAB;struct Cyc_Absyn_Deref_e_struct*_tmpAAA;return(void*)((_tmpAAA=_cycalloc(
sizeof(*_tmpAAA)),((_tmpAAA[0]=((_tmpAAB.tag=21,((_tmpAAB.f1=e,_tmpAAB)))),
_tmpAAA))));}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){struct Cyc_Absyn_Subscript_e_struct _tmpAAE;struct Cyc_Absyn_Subscript_e_struct*
_tmpAAD;return(void*)((_tmpAAD=_cycalloc(sizeof(*_tmpAAD)),((_tmpAAD[0]=((
_tmpAAE.tag=24,((_tmpAAE.f1=e1,((_tmpAAE.f2=e2,_tmpAAE)))))),_tmpAAD))));}static
void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct
_tmpAB1;struct Cyc_Absyn_StmtExp_e_struct*_tmpAB0;return(void*)((_tmpAB0=
_cycalloc(sizeof(*_tmpAB0)),((_tmpAB0[0]=((_tmpAB1.tag=37,((_tmpAB1.f1=s,_tmpAB1)))),
_tmpAB0))));}static void*Cyc_Toc_sizeoftyp_exp_r(void*t){struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpAB4;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpAB3;return(void*)((_tmpAB3=
_cycalloc(sizeof(*_tmpAB3)),((_tmpAB3[0]=((_tmpAB4.tag=18,((_tmpAB4.f1=(void*)t,
_tmpAB4)))),_tmpAB3))));}static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct _tmpAB7;struct Cyc_Absyn_FnCall_e_struct*
_tmpAB6;return(void*)((_tmpAB6=_cycalloc(sizeof(*_tmpAB6)),((_tmpAB6[0]=((
_tmpAB7.tag=11,((_tmpAB7.f1=e,((_tmpAB7.f2=es,((_tmpAB7.f3=0,_tmpAB7)))))))),
_tmpAB6))));}static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_struct
_tmpABA;struct Cyc_Absyn_Exp_s_struct*_tmpAB9;return(void*)((_tmpAB9=_cycalloc(
sizeof(*_tmpAB9)),((_tmpAB9[0]=((_tmpABA.tag=1,((_tmpABA.f1=e,_tmpABA)))),
_tmpAB9))));}static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2){struct Cyc_Absyn_Seq_s_struct _tmpABD;struct Cyc_Absyn_Seq_s_struct*_tmpABC;
return(void*)((_tmpABC=_cycalloc(sizeof(*_tmpABC)),((_tmpABC[0]=((_tmpABD.tag=2,((
_tmpABD.f1=s1,((_tmpABD.f2=s2,_tmpABD)))))),_tmpABC))));}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct
_tmpAC0;struct Cyc_Absyn_Conditional_e_struct*_tmpABF;return(void*)((_tmpABF=
_cycalloc(sizeof(*_tmpABF)),((_tmpABF[0]=((_tmpAC0.tag=6,((_tmpAC0.f1=e1,((
_tmpAC0.f2=e2,((_tmpAC0.f3=e3,_tmpAC0)))))))),_tmpABF))));}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrMember_e_struct
_tmpAC3;struct Cyc_Absyn_AggrMember_e_struct*_tmpAC2;return(void*)((_tmpAC2=
_cycalloc(sizeof(*_tmpAC2)),((_tmpAC2[0]=((_tmpAC3.tag=22,((_tmpAC3.f1=e,((
_tmpAC3.f2=n,((_tmpAC3.f3=0,((_tmpAC3.f4=0,_tmpAC3)))))))))),_tmpAC2))));}static
void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_struct _tmpAC6;struct Cyc_Absyn_AggrArrow_e_struct*
_tmpAC5;return(void*)((_tmpAC5=_cycalloc(sizeof(*_tmpAC5)),((_tmpAC5[0]=((
_tmpAC6.tag=23,((_tmpAC6.f1=e,((_tmpAC6.f2=n,((_tmpAC6.f3=0,((_tmpAC6.f4=0,
_tmpAC6)))))))))),_tmpAC5))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmpAC9;
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpAC8;return(void*)((_tmpAC8=_cycalloc(
sizeof(*_tmpAC8)),((_tmpAC8[0]=((_tmpAC9.tag=36,((_tmpAC9.f1=tdopt,((_tmpAC9.f2=
ds,_tmpAC9)))))),_tmpAC8))));}static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_struct _tmpACC;
struct Cyc_Absyn_Goto_s_struct*_tmpACB;return(void*)((_tmpACB=_cycalloc(sizeof(*
_tmpACB)),((_tmpACB[0]=((_tmpACC.tag=8,((_tmpACC.f1=v,((_tmpACC.f2=s,_tmpACC)))))),
_tmpACB))));}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{
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
_tmpAD2;const char*_tmpAD1;struct Cyc_Core_Impossible_struct*_tmpAD0;(int)_throw((
void*)((_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((_tmpAD0[0]=((_tmpAD2.tag=Cyc_Core_Impossible,((
_tmpAD2.f1=((_tmpAD1="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpAD1,sizeof(char),44))),_tmpAD2)))),_tmpAD0)))));}}goto _LL0;
_LL3: {struct Cyc_Absyn_FloatType_struct*_tmpC9=(struct Cyc_Absyn_FloatType_struct*)
_tmpC6;if(_tmpC9->tag != 7)goto _LL5;}_LL4: function=fS->ffloat;goto _LL0;_LL5: {
struct Cyc_Absyn_DoubleType_struct*_tmpCA=(struct Cyc_Absyn_DoubleType_struct*)
_tmpC6;if(_tmpCA->tag != 8)goto _LL7;else{_tmpCB=_tmpCA->f1;}}_LL6: switch(_tmpCB){
case 1: _LL10: function=fS->flongdouble;break;default: _LL11: function=fS->fdouble;}
goto _LL0;_LL7: {struct Cyc_Absyn_PointerType_struct*_tmpCC=(struct Cyc_Absyn_PointerType_struct*)
_tmpC6;if(_tmpCC->tag != 5)goto _LL9;}_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;
_LLA: {struct Cyc_Core_Impossible_struct _tmpADF;const char*_tmpADE;void*_tmpADD[1];
struct Cyc_String_pa_struct _tmpADC;struct Cyc_Core_Impossible_struct*_tmpADB;(int)
_throw((void*)((_tmpADB=_cycalloc(sizeof(*_tmpADB)),((_tmpADB[0]=((_tmpADF.tag=
Cyc_Core_Impossible,((_tmpADF.f1=(struct _dyneither_ptr)((_tmpADC.tag=0,((_tmpADC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpADD[
0]=& _tmpADC,Cyc_aprintf(((_tmpADE="impossible expression type %s (not int, float, double, or pointer)",
_tag_dyneither(_tmpADE,sizeof(char),67))),_tag_dyneither(_tmpADD,sizeof(void*),1)))))))),
_tmpADF)))),_tmpADB)))));}_LL0:;}return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(
fS,(void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v);}struct Cyc_Absyn_Exp*
Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*
arr){void*_tmpD5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
arr->topt))->v);struct Cyc_Absyn_PtrInfo _tmpD7;void*_tmpD8;_LL14: {struct Cyc_Absyn_PointerType_struct*
_tmpD6=(struct Cyc_Absyn_PointerType_struct*)_tmpD5;if(_tmpD6->tag != 5)goto _LL16;
else{_tmpD7=_tmpD6->f1;_tmpD8=_tmpD7.elt_typ;}}_LL15: return Cyc_Toc_getFunctionType(
fS,_tmpD8);_LL16:;_LL17: {struct Cyc_Core_Impossible_struct _tmpAE5;const char*
_tmpAE4;struct Cyc_Core_Impossible_struct*_tmpAE3;(int)_throw((void*)((_tmpAE3=
_cycalloc(sizeof(*_tmpAE3)),((_tmpAE3[0]=((_tmpAE5.tag=Cyc_Core_Impossible,((
_tmpAE5.f1=((_tmpAE4="impossible type (not pointer)",_tag_dyneither(_tmpAE4,
sizeof(char),30))),_tmpAE5)))),_tmpAE3)))));}_LL13:;}struct _tuple15{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){void*
_tmpDC=e->r;union Cyc_Absyn_Cnst _tmpDE;struct _tuple3 _tmpDF;char _tmpE0;union Cyc_Absyn_Cnst
_tmpE2;struct _dyneither_ptr _tmpE3;union Cyc_Absyn_Cnst _tmpE5;struct _tuple4 _tmpE6;
short _tmpE7;union Cyc_Absyn_Cnst _tmpE9;struct _tuple5 _tmpEA;int _tmpEB;union Cyc_Absyn_Cnst
_tmpED;struct _tuple6 _tmpEE;long long _tmpEF;union Cyc_Absyn_Cnst _tmpF1;int _tmpF2;
struct Cyc_Absyn_Exp*_tmpF4;struct Cyc_List_List*_tmpF6;struct Cyc_List_List*_tmpF8;
struct Cyc_List_List*_tmpFA;struct Cyc_List_List*_tmpFC;struct Cyc_List_List*_tmpFE;
_LL19: {struct Cyc_Absyn_Const_e_struct*_tmpDD=(struct Cyc_Absyn_Const_e_struct*)
_tmpDC;if(_tmpDD->tag != 0)goto _LL1B;else{_tmpDE=_tmpDD->f1;if((_tmpDE.Char_c).tag
!= 2)goto _LL1B;_tmpDF=(struct _tuple3)(_tmpDE.Char_c).val;_tmpE0=_tmpDF.f2;}}
_LL1A: return _tmpE0 == '\000';_LL1B: {struct Cyc_Absyn_Const_e_struct*_tmpE1=(
struct Cyc_Absyn_Const_e_struct*)_tmpDC;if(_tmpE1->tag != 0)goto _LL1D;else{_tmpE2=
_tmpE1->f1;if((_tmpE2.Wchar_c).tag != 3)goto _LL1D;_tmpE3=(struct _dyneither_ptr)(
_tmpE2.Wchar_c).val;}}_LL1C: {unsigned long _tmpFF=Cyc_strlen((struct
_dyneither_ptr)_tmpE3);int i=0;if(_tmpFF >= 2  && *((const char*)
_check_dyneither_subscript(_tmpE3,sizeof(char),0))== '\\'){if(*((const char*)
_check_dyneither_subscript(_tmpE3,sizeof(char),1))== '0')i=2;else{if((*((const
char*)_check_dyneither_subscript(_tmpE3,sizeof(char),1))== 'x'  && _tmpFF >= 3)
 && *((const char*)_check_dyneither_subscript(_tmpE3,sizeof(char),2))== '0')i=3;
else{return 0;}}for(0;i < _tmpFF;++ i){if(*((const char*)_check_dyneither_subscript(
_tmpE3,sizeof(char),i))!= '0')return 0;}return 1;}else{return 0;}}_LL1D: {struct Cyc_Absyn_Const_e_struct*
_tmpE4=(struct Cyc_Absyn_Const_e_struct*)_tmpDC;if(_tmpE4->tag != 0)goto _LL1F;
else{_tmpE5=_tmpE4->f1;if((_tmpE5.Short_c).tag != 4)goto _LL1F;_tmpE6=(struct
_tuple4)(_tmpE5.Short_c).val;_tmpE7=_tmpE6.f2;}}_LL1E: return _tmpE7 == 0;_LL1F: {
struct Cyc_Absyn_Const_e_struct*_tmpE8=(struct Cyc_Absyn_Const_e_struct*)_tmpDC;
if(_tmpE8->tag != 0)goto _LL21;else{_tmpE9=_tmpE8->f1;if((_tmpE9.Int_c).tag != 5)
goto _LL21;_tmpEA=(struct _tuple5)(_tmpE9.Int_c).val;_tmpEB=_tmpEA.f2;}}_LL20:
return _tmpEB == 0;_LL21: {struct Cyc_Absyn_Const_e_struct*_tmpEC=(struct Cyc_Absyn_Const_e_struct*)
_tmpDC;if(_tmpEC->tag != 0)goto _LL23;else{_tmpED=_tmpEC->f1;if((_tmpED.LongLong_c).tag
!= 6)goto _LL23;_tmpEE=(struct _tuple6)(_tmpED.LongLong_c).val;_tmpEF=_tmpEE.f2;}}
_LL22: return _tmpEF == 0;_LL23: {struct Cyc_Absyn_Const_e_struct*_tmpF0=(struct Cyc_Absyn_Const_e_struct*)
_tmpDC;if(_tmpF0->tag != 0)goto _LL25;else{_tmpF1=_tmpF0->f1;if((_tmpF1.Null_c).tag
!= 1)goto _LL25;_tmpF2=(int)(_tmpF1.Null_c).val;}}_LL24: return 1;_LL25: {struct Cyc_Absyn_Cast_e_struct*
_tmpF3=(struct Cyc_Absyn_Cast_e_struct*)_tmpDC;if(_tmpF3->tag != 15)goto _LL27;
else{_tmpF4=_tmpF3->f2;}}_LL26: return Cyc_Toc_is_zero(_tmpF4);_LL27: {struct Cyc_Absyn_Tuple_e_struct*
_tmpF5=(struct Cyc_Absyn_Tuple_e_struct*)_tmpDC;if(_tmpF5->tag != 25)goto _LL29;
else{_tmpF6=_tmpF5->f1;}}_LL28: return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpF6);_LL29: {struct
Cyc_Absyn_Array_e_struct*_tmpF7=(struct Cyc_Absyn_Array_e_struct*)_tmpDC;if(
_tmpF7->tag != 27)goto _LL2B;else{_tmpF8=_tmpF7->f1;}}_LL2A: _tmpFA=_tmpF8;goto
_LL2C;_LL2B: {struct Cyc_Absyn_Aggregate_e_struct*_tmpF9=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpDC;if(_tmpF9->tag != 29)goto _LL2D;else{_tmpFA=_tmpF9->f3;}}_LL2C: _tmpFC=
_tmpFA;goto _LL2E;_LL2D: {struct Cyc_Absyn_CompoundLit_e_struct*_tmpFB=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpDC;if(_tmpFB->tag != 26)goto _LL2F;else{_tmpFC=_tmpFB->f2;}}_LL2E: _tmpFE=
_tmpFC;goto _LL30;_LL2F: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpFD=(struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmpDC;if(_tmpFD->tag != 36)goto _LL31;else{
_tmpFE=_tmpFD->f2;}}_LL30: for(0;_tmpFE != 0;_tmpFE=_tmpFE->tl){if(!Cyc_Toc_is_zero((*((
struct _tuple15*)_tmpFE->hd)).f2))return 0;}return 1;_LL31:;_LL32: return 0;_LL18:;}
static int Cyc_Toc_is_nullable(void*t){void*_tmp100=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_PtrInfo _tmp102;struct Cyc_Absyn_PtrAtts _tmp103;union Cyc_Absyn_Constraint*
_tmp104;_LL34: {struct Cyc_Absyn_PointerType_struct*_tmp101=(struct Cyc_Absyn_PointerType_struct*)
_tmp100;if(_tmp101->tag != 5)goto _LL36;else{_tmp102=_tmp101->f1;_tmp103=_tmp102.ptr_atts;
_tmp104=_tmp103.nullable;}}_LL35: return((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp104);_LL36:;_LL37: {const char*_tmpAE8;void*_tmpAE7;(
_tmpAE7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAE8="is_nullable",_tag_dyneither(_tmpAE8,sizeof(char),12))),_tag_dyneither(
_tmpAE7,sizeof(void*),0)));}_LL33:;}static char _tmp113[1]="";static char _tmp114[8]="*bogus*";
static struct _tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct
_dyneither_ptr tag){struct _DynRegionHandle*_tmp108;struct Cyc_Dict_Dict*_tmp109;
struct Cyc_Toc_TocState _tmp107=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp108=_tmp107.dyn;_tmp109=_tmp107.qvar_tags;{static struct _dyneither_ptr
bogus_string={_tmp114,_tmp114,_tmp114 + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
4,0}},& bogus_string};static struct _tuple13 pair={& bogus_qvar,{_tmp113,_tmp113,
_tmp113 + 1}};{struct _tuple13 _tmpAE9;pair=((_tmpAE9.f1=x,((_tmpAE9.f2=tag,_tmpAE9))));}{
struct _DynRegionFrame _tmp10B;struct _RegionHandle*d=_open_dynregion(& _tmp10B,
_tmp108);{struct _tuple0**_tmp10C=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,
struct _tuple13*k))Cyc_Dict_lookup_opt)(*_tmp109,(struct _tuple13*)& pair);if(
_tmp10C != 0){struct _tuple0*_tmp10D=*_tmp10C;_npop_handler(0);return _tmp10D;}{
struct _tuple13*_tmpAEA;struct _tuple13*_tmp10E=(_tmpAEA=_cycalloc(sizeof(*_tmpAEA)),((
_tmpAEA->f1=x,((_tmpAEA->f2=tag,_tmpAEA)))));struct _dyneither_ptr*_tmpAED;struct
_tuple0*_tmpAEC;struct _tuple0*res=(_tmpAEC=_cycalloc(sizeof(*_tmpAEC)),((_tmpAEC->f1=(*
x).f1,((_tmpAEC->f2=((_tmpAED=_cycalloc(sizeof(*_tmpAED)),((_tmpAED[0]=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*x).f2,(struct
_dyneither_ptr)tag),_tmpAED)))),_tmpAEC)))));*_tmp109=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple13*k,struct _tuple0*v))Cyc_Dict_insert)(*_tmp109,(
struct _tuple13*)_tmp10E,res);{struct _tuple0*_tmp10F=res;_npop_handler(0);return
_tmp10F;};};};_pop_dynregion(d);};};}static void*Cyc_Toc_typ_to_c(void*t);struct
_tuple16{void*f1;struct Cyc_List_List*f2;};static void*Cyc_Toc_add_tuple_type(
struct Cyc_List_List*tqs0){struct _DynRegionHandle*_tmp116;struct Cyc_List_List**
_tmp117;struct Cyc_Toc_TocState _tmp115=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp116=_tmp115.dyn;_tmp117=_tmp115.tuple_types;{struct _DynRegionFrame _tmp118;
struct _RegionHandle*d=_open_dynregion(& _tmp118,_tmp116);{struct Cyc_List_List*
_tmp119=*_tmp117;for(0;_tmp119 != 0;_tmp119=_tmp119->tl){struct _tuple16 _tmp11B;
void*_tmp11C;struct Cyc_List_List*_tmp11D;struct _tuple16*_tmp11A=(struct _tuple16*)
_tmp119->hd;_tmp11B=*_tmp11A;_tmp11C=_tmp11B.f1;_tmp11D=_tmp11B.f2;{struct Cyc_List_List*
_tmp11E=tqs0;for(0;_tmp11E != 0  && _tmp11D != 0;(_tmp11E=_tmp11E->tl,_tmp11D=
_tmp11D->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple9*)_tmp11E->hd)).f2,(void*)
_tmp11D->hd))break;}if(_tmp11E == 0  && _tmp11D == 0){void*_tmp11F=_tmp11C;
_npop_handler(0);return _tmp11F;}};}}{struct Cyc_Int_pa_struct _tmpAF5;void*_tmpAF4[
1];const char*_tmpAF3;struct _dyneither_ptr*_tmpAF2;struct _dyneither_ptr*xname=(
_tmpAF2=_cycalloc(sizeof(*_tmpAF2)),((_tmpAF2[0]=(struct _dyneither_ptr)((_tmpAF5.tag=
1,((_tmpAF5.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAF4[0]=& _tmpAF5,
Cyc_aprintf(((_tmpAF3="_tuple%d",_tag_dyneither(_tmpAF3,sizeof(char),9))),
_tag_dyneither(_tmpAF4,sizeof(void*),1)))))))),_tmpAF2)));void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp120=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmpAF8;struct Cyc_List_List*
_tmpAF7;_tmp120=((_tmpAF7=_cycalloc(sizeof(*_tmpAF7)),((_tmpAF7->hd=((_tmpAF8=
_cycalloc(sizeof(*_tmpAF8)),((_tmpAF8->name=Cyc_Absyn_fieldname(i),((_tmpAF8->tq=
Cyc_Toc_mt_tq,((_tmpAF8->type=(void*)ts2->hd,((_tmpAF8->width=0,((_tmpAF8->attributes=
0,_tmpAF8)))))))))))),((_tmpAF7->tl=_tmp120,_tmpAF7))))));}}_tmp120=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp120);{struct Cyc_Absyn_AggrdeclImpl*
_tmpAFD;struct _tuple0*_tmpAFC;struct Cyc_Absyn_Aggrdecl*_tmpAFB;struct Cyc_Absyn_Aggrdecl*
_tmp123=(_tmpAFB=_cycalloc(sizeof(*_tmpAFB)),((_tmpAFB->kind=Cyc_Absyn_StructA,((
_tmpAFB->sc=Cyc_Absyn_Public,((_tmpAFB->name=((_tmpAFC=_cycalloc(sizeof(*_tmpAFC)),((
_tmpAFC->f1=Cyc_Absyn_Rel_n(0),((_tmpAFC->f2=xname,_tmpAFC)))))),((_tmpAFB->tvs=
0,((_tmpAFB->impl=((_tmpAFD=_cycalloc(sizeof(*_tmpAFD)),((_tmpAFD->exist_vars=0,((
_tmpAFD->rgn_po=0,((_tmpAFD->fields=_tmp120,((_tmpAFD->tagged=0,_tmpAFD)))))))))),((
_tmpAFB->attributes=0,_tmpAFB)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*
_tmpB03;struct Cyc_Absyn_Aggr_d_struct _tmpB02;struct Cyc_List_List*_tmpB01;Cyc_Toc_result_decls=((
_tmpB01=_cycalloc(sizeof(*_tmpB01)),((_tmpB01->hd=Cyc_Absyn_new_decl((void*)((
_tmpB03=_cycalloc(sizeof(*_tmpB03)),((_tmpB03[0]=((_tmpB02.tag=6,((_tmpB02.f1=
_tmp123,_tmpB02)))),_tmpB03)))),0),((_tmpB01->tl=Cyc_Toc_result_decls,_tmpB01))))));}{
struct _tuple16*_tmpB06;struct Cyc_List_List*_tmpB05;*_tmp117=((_tmpB05=
_region_malloc(d,sizeof(*_tmpB05)),((_tmpB05->hd=((_tmpB06=_region_malloc(d,
sizeof(*_tmpB06)),((_tmpB06->f1=x,((_tmpB06->f2=ts,_tmpB06)))))),((_tmpB05->tl=*
_tmp117,_tmpB05))))));}{void*_tmp129=x;_npop_handler(0);return _tmp129;};};};;
_pop_dynregion(d);};}struct _tuple17{struct _tuple0*f1;struct Cyc_List_List*f2;void*
f3;};static void*Cyc_Toc_add_struct_type(struct _tuple0*struct_name,struct Cyc_List_List*
type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){struct
_DynRegionHandle*_tmp132;struct Cyc_List_List**_tmp133;struct Cyc_Toc_TocState
_tmp131=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp132=
_tmp131.dyn;_tmp133=_tmp131.abs_struct_types;{struct _DynRegionFrame _tmp134;
struct _RegionHandle*d=_open_dynregion(& _tmp134,_tmp132);{struct Cyc_List_List*
_tmp135=*_tmp133;for(0;_tmp135 != 0;_tmp135=_tmp135->tl){struct _tuple17 _tmp137;
struct _tuple0*_tmp138;struct Cyc_List_List*_tmp139;void*_tmp13A;struct _tuple17*
_tmp136=(struct _tuple17*)_tmp135->hd;_tmp137=*_tmp136;_tmp138=_tmp137.f1;_tmp139=
_tmp137.f2;_tmp13A=_tmp137.f3;if(Cyc_Absyn_qvar_cmp(_tmp138,struct_name)== 0  && ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(type_args)== ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp139)){int okay=1;{struct Cyc_List_List*_tmp13B=type_args;
for(0;_tmp13B != 0;(_tmp13B=_tmp13B->tl,_tmp139=_tmp139->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp13B->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp139))->hd)){okay=
0;break;}}}if(okay){void*_tmp13C=_tmp13A;_npop_handler(0);return _tmp13C;}}}}{
struct Cyc_Int_pa_struct _tmpB0E;void*_tmpB0D[1];const char*_tmpB0C;struct
_dyneither_ptr*_tmpB0B;struct _dyneither_ptr*xname=(_tmpB0B=_cycalloc(sizeof(*
_tmpB0B)),((_tmpB0B[0]=(struct _dyneither_ptr)((_tmpB0E.tag=1,((_tmpB0E.f1=(
unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB0D[0]=& _tmpB0E,Cyc_aprintf(((
_tmpB0C="_tuple%d",_tag_dyneither(_tmpB0C,sizeof(char),9))),_tag_dyneither(
_tmpB0D,sizeof(void*),1)))))))),_tmpB0B)));void*x=Cyc_Absyn_strct(xname);struct
Cyc_List_List*_tmp13D=0;{struct _tuple17*_tmpB11;struct Cyc_List_List*_tmpB10;*
_tmp133=((_tmpB10=_region_malloc(d,sizeof(*_tmpB10)),((_tmpB10->hd=((_tmpB11=
_region_malloc(d,sizeof(*_tmpB11)),((_tmpB11->f1=struct_name,((_tmpB11->f2=
type_args,((_tmpB11->f3=x,_tmpB11)))))))),((_tmpB10->tl=*_tmp133,_tmpB10))))));}{
struct _RegionHandle _tmp140=_new_region("r");struct _RegionHandle*r=& _tmp140;
_push_region(r);{struct Cyc_List_List*_tmp141=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,type_vars,type_args);for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp142=(struct Cyc_Absyn_Aggrfield*)fields->hd;void*t=
_tmp142->type;t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp141,t));if(Cyc_Tcutil_unify(
t,(void*)& Cyc_Absyn_VoidType_val))break;t=Cyc_Toc_typ_to_c(t);{struct Cyc_Absyn_Aggrfield*
_tmpB14;struct Cyc_List_List*_tmpB13;_tmp13D=((_tmpB13=_cycalloc(sizeof(*_tmpB13)),((
_tmpB13->hd=((_tmpB14=_cycalloc(sizeof(*_tmpB14)),((_tmpB14->name=_tmp142->name,((
_tmpB14->tq=Cyc_Toc_mt_tq,((_tmpB14->type=t,((_tmpB14->width=_tmp142->width,((
_tmpB14->attributes=_tmp142->attributes,_tmpB14)))))))))))),((_tmpB13->tl=
_tmp13D,_tmpB13))))));};}_tmp13D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp13D);{struct Cyc_Absyn_AggrdeclImpl*_tmpB19;struct _tuple0*
_tmpB18;struct Cyc_Absyn_Aggrdecl*_tmpB17;struct Cyc_Absyn_Aggrdecl*_tmp145=(
_tmpB17=_cycalloc(sizeof(*_tmpB17)),((_tmpB17->kind=Cyc_Absyn_StructA,((_tmpB17->sc=
Cyc_Absyn_Public,((_tmpB17->name=((_tmpB18=_cycalloc(sizeof(*_tmpB18)),((_tmpB18->f1=
Cyc_Absyn_Rel_n(0),((_tmpB18->f2=xname,_tmpB18)))))),((_tmpB17->tvs=0,((_tmpB17->impl=((
_tmpB19=_cycalloc(sizeof(*_tmpB19)),((_tmpB19->exist_vars=0,((_tmpB19->rgn_po=0,((
_tmpB19->fields=_tmp13D,((_tmpB19->tagged=0,_tmpB19)))))))))),((_tmpB17->attributes=
0,_tmpB17)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpB1F;struct Cyc_Absyn_Aggr_d_struct
_tmpB1E;struct Cyc_List_List*_tmpB1D;Cyc_Toc_result_decls=((_tmpB1D=_cycalloc(
sizeof(*_tmpB1D)),((_tmpB1D->hd=Cyc_Absyn_new_decl((void*)((_tmpB1F=_cycalloc(
sizeof(*_tmpB1F)),((_tmpB1F[0]=((_tmpB1E.tag=6,((_tmpB1E.f1=_tmp145,_tmpB1E)))),
_tmpB1F)))),0),((_tmpB1D->tl=Cyc_Toc_result_decls,_tmpB1D))))));}{void*_tmp149=x;
_npop_handler(1);return _tmp149;};};};_pop_region(r);};};;_pop_dynregion(d);};}
struct _tuple0*Cyc_Toc_temp_var(){int _tmp151=Cyc_Toc_temp_var_counter ++;struct
_dyneither_ptr*_tmpB2C;const char*_tmpB2B;void*_tmpB2A[1];struct Cyc_Int_pa_struct
_tmpB29;struct _tuple0*_tmpB28;struct _tuple0*res=(_tmpB28=_cycalloc(sizeof(*
_tmpB28)),((_tmpB28->f1=Cyc_Absyn_Loc_n,((_tmpB28->f2=((_tmpB2C=_cycalloc(
sizeof(*_tmpB2C)),((_tmpB2C[0]=(struct _dyneither_ptr)((_tmpB29.tag=1,((_tmpB29.f1=(
unsigned int)_tmp151,((_tmpB2A[0]=& _tmpB29,Cyc_aprintf(((_tmpB2B="_tmp%X",
_tag_dyneither(_tmpB2B,sizeof(char),7))),_tag_dyneither(_tmpB2A,sizeof(void*),1)))))))),
_tmpB2C)))),_tmpB28)))));return res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp158;struct Cyc_Xarray_Xarray*_tmp159;struct Cyc_Toc_TocState
_tmp157=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp158=
_tmp157.dyn;_tmp159=_tmp157.temp_labels;{struct _DynRegionFrame _tmp15A;struct
_RegionHandle*d=_open_dynregion(& _tmp15A,_tmp158);{int _tmp15B=Cyc_Toc_fresh_label_counter
++;if(_tmp15B < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp159)){
struct _dyneither_ptr*_tmp15C=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp159,_tmp15B);_npop_handler(0);return _tmp15C;}{struct Cyc_Int_pa_struct
_tmpB34;void*_tmpB33[1];const char*_tmpB32;struct _dyneither_ptr*_tmpB31;struct
_dyneither_ptr*res=(_tmpB31=_cycalloc(sizeof(*_tmpB31)),((_tmpB31[0]=(struct
_dyneither_ptr)((_tmpB34.tag=1,((_tmpB34.f1=(unsigned int)_tmp15B,((_tmpB33[0]=&
_tmpB34,Cyc_aprintf(((_tmpB32="_LL%X",_tag_dyneither(_tmpB32,sizeof(char),6))),
_tag_dyneither(_tmpB33,sizeof(void*),1)))))))),_tmpB31)));if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp159,res)!= _tmp15B){const char*
_tmpB37;void*_tmpB36;(_tmpB36=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB37="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmpB37,sizeof(char),43))),_tag_dyneither(_tmpB36,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp15F=res;_npop_handler(0);return _tmp15F;};};};
_pop_dynregion(d);};}static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*
td,struct _tuple0*name){int ans=0;struct Cyc_List_List*_tmp164=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((
struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp164))->hd)->name)
!= 0){++ ans;_tmp164=_tmp164->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);static void Cyc_Toc_aggrdecl_to_c(
struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);static void Cyc_Toc_datatypedecl_to_c(
struct Cyc_Absyn_Datatypedecl*tud);static struct _tuple7*Cyc_Toc_arg_to_c(struct
_tuple7*a){struct Cyc_Core_Opt*_tmp166;struct Cyc_Absyn_Tqual _tmp167;void*_tmp168;
struct _tuple7 _tmp165=*a;_tmp166=_tmp165.f1;_tmp167=_tmp165.f2;_tmp168=_tmp165.f3;{
struct _tuple7*_tmpB38;return(_tmpB38=_cycalloc(sizeof(*_tmpB38)),((_tmpB38->f1=
_tmp166,((_tmpB38->f2=_tmp167,((_tmpB38->f3=Cyc_Toc_typ_to_c(_tmp168),_tmpB38)))))));};}
static struct _tuple9*Cyc_Toc_typ_to_c_f(struct _tuple9*x){struct Cyc_Absyn_Tqual
_tmp16B;void*_tmp16C;struct _tuple9 _tmp16A=*x;_tmp16B=_tmp16A.f1;_tmp16C=_tmp16A.f2;{
struct _tuple9*_tmpB39;return(_tmpB39=_cycalloc(sizeof(*_tmpB39)),((_tmpB39->f1=
_tmp16B,((_tmpB39->f2=Cyc_Toc_typ_to_c(_tmp16C),_tmpB39)))));};}static void*Cyc_Toc_typ_to_c_array(
void*t){void*_tmp16E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp170;
void*_tmp171;struct Cyc_Absyn_Tqual _tmp172;struct Cyc_Absyn_Exp*_tmp173;union Cyc_Absyn_Constraint*
_tmp174;struct Cyc_Position_Segment*_tmp175;struct Cyc_Core_Opt*_tmp177;struct Cyc_Core_Opt
_tmp178;void*_tmp179;_LL39: {struct Cyc_Absyn_ArrayType_struct*_tmp16F=(struct Cyc_Absyn_ArrayType_struct*)
_tmp16E;if(_tmp16F->tag != 9)goto _LL3B;else{_tmp170=_tmp16F->f1;_tmp171=_tmp170.elt_type;
_tmp172=_tmp170.tq;_tmp173=_tmp170.num_elts;_tmp174=_tmp170.zero_term;_tmp175=
_tmp170.zt_loc;}}_LL3A: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp171),
_tmp172,_tmp173,Cyc_Absyn_false_conref,_tmp175);_LL3B: {struct Cyc_Absyn_Evar_struct*
_tmp176=(struct Cyc_Absyn_Evar_struct*)_tmp16E;if(_tmp176->tag != 1)goto _LL3D;
else{_tmp177=_tmp176->f2;if(_tmp177 == 0)goto _LL3D;_tmp178=*_tmp177;_tmp179=(void*)
_tmp178.v;}}_LL3C: return Cyc_Toc_typ_to_c_array(_tmp179);_LL3D:;_LL3E: return Cyc_Toc_typ_to_c(
t);_LL38:;}static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*
f){struct Cyc_Absyn_Aggrfield*_tmpB3A;return(_tmpB3A=_cycalloc(sizeof(*_tmpB3A)),((
_tmpB3A->name=f->name,((_tmpB3A->tq=Cyc_Toc_mt_tq,((_tmpB3A->type=Cyc_Toc_typ_to_c(
f->type),((_tmpB3A->width=f->width,((_tmpB3A->attributes=f->attributes,_tmpB3A)))))))))));}
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){return;}static void*Cyc_Toc_char_star_typ(){
static void**cs=0;if(cs == 0){void**_tmpB3B;cs=((_tmpB3B=_cycalloc(sizeof(*_tmpB3B)),((
_tmpB3B[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpB3B))));}return*cs;}static void*Cyc_Toc_rgn_typ(){
static void**r=0;if(r == 0){void**_tmpB3C;r=((_tmpB3C=_cycalloc(sizeof(*_tmpB3C)),((
_tmpB3C[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),
_tmpB3C))));}return*r;}static void*Cyc_Toc_dyn_rgn_typ(){static void**r=0;if(r == 0){
void**_tmpB3D;r=((_tmpB3D=_cycalloc(sizeof(*_tmpB3D)),((_tmpB3D[0]=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpB3D))));}return*
r;}static int Cyc_Toc_is_boxed_tvar(void*t){void*_tmp17E=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_Tvar*_tmp180;_LL40: {struct Cyc_Absyn_VarType_struct*_tmp17F=(
struct Cyc_Absyn_VarType_struct*)_tmp17E;if(_tmp17F->tag != 2)goto _LL42;else{
_tmp180=_tmp17F->f1;}}_LL41: return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),&
Cyc_Tcutil_tbk);_LL42:;_LL43: return 0;_LL3F:;}static int Cyc_Toc_is_abs_tvar(void*t){
void*_tmp181=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp183;_LL45: {struct
Cyc_Absyn_VarType_struct*_tmp182=(struct Cyc_Absyn_VarType_struct*)_tmp181;if(
_tmp182->tag != 2)goto _LL47;else{_tmp183=_tmp182->f1;}}_LL46: return Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t));_LL47:;_LL48: return 0;_LL44:;}static void*
Cyc_Toc_typ_to_c(void*t){void*_tmp184=t;struct Cyc_Core_Opt*_tmp187;struct Cyc_Core_Opt*
_tmp189;struct Cyc_Core_Opt _tmp18A;void*_tmp18B;struct Cyc_Absyn_Tvar*_tmp18D;
struct Cyc_Absyn_DatatypeFieldInfo _tmp190;union Cyc_Absyn_DatatypeFieldInfoU
_tmp191;struct _tuple1 _tmp192;struct Cyc_Absyn_Datatypedecl*_tmp193;struct Cyc_Absyn_Datatypefield*
_tmp194;struct Cyc_Absyn_PtrInfo _tmp197;void*_tmp198;struct Cyc_Absyn_Tqual _tmp199;
struct Cyc_Absyn_PtrAtts _tmp19A;union Cyc_Absyn_Constraint*_tmp19B;struct Cyc_Absyn_ArrayInfo
_tmp1A0;void*_tmp1A1;struct Cyc_Absyn_Tqual _tmp1A2;struct Cyc_Absyn_Exp*_tmp1A3;
struct Cyc_Position_Segment*_tmp1A4;struct Cyc_Absyn_FnInfo _tmp1A6;struct Cyc_Absyn_Tqual
_tmp1A7;void*_tmp1A8;struct Cyc_List_List*_tmp1A9;int _tmp1AA;struct Cyc_Absyn_VarargInfo*
_tmp1AB;struct Cyc_List_List*_tmp1AC;struct Cyc_List_List*_tmp1AE;enum Cyc_Absyn_AggrKind
_tmp1B0;struct Cyc_List_List*_tmp1B1;struct Cyc_Absyn_AggrInfo _tmp1B3;union Cyc_Absyn_AggrInfoU
_tmp1B4;struct Cyc_List_List*_tmp1B5;struct _tuple0*_tmp1B7;struct Cyc_List_List*
_tmp1B9;struct _tuple0*_tmp1BB;struct Cyc_List_List*_tmp1BC;struct Cyc_Absyn_Typedefdecl*
_tmp1BD;void**_tmp1BE;void*_tmp1C1;struct Cyc_Absyn_TypeDecl*_tmp1CB;struct Cyc_Absyn_TypeDecl
_tmp1CC;void*_tmp1CD;struct Cyc_Absyn_Aggrdecl*_tmp1CF;struct Cyc_Absyn_TypeDecl*
_tmp1D1;struct Cyc_Absyn_TypeDecl _tmp1D2;void*_tmp1D3;struct Cyc_Absyn_Enumdecl*
_tmp1D5;struct Cyc_Absyn_TypeDecl*_tmp1D7;struct Cyc_Absyn_TypeDecl _tmp1D8;void*
_tmp1D9;struct Cyc_Absyn_Datatypedecl*_tmp1DB;void**_tmp1DC;_LL4A: {struct Cyc_Absyn_VoidType_struct*
_tmp185=(struct Cyc_Absyn_VoidType_struct*)_tmp184;if(_tmp185->tag != 0)goto _LL4C;}
_LL4B: return t;_LL4C: {struct Cyc_Absyn_Evar_struct*_tmp186=(struct Cyc_Absyn_Evar_struct*)
_tmp184;if(_tmp186->tag != 1)goto _LL4E;else{_tmp187=_tmp186->f2;if(_tmp187 != 0)
goto _LL4E;}}_LL4D: return Cyc_Absyn_sint_typ;_LL4E: {struct Cyc_Absyn_Evar_struct*
_tmp188=(struct Cyc_Absyn_Evar_struct*)_tmp184;if(_tmp188->tag != 1)goto _LL50;
else{_tmp189=_tmp188->f2;if(_tmp189 == 0)goto _LL50;_tmp18A=*_tmp189;_tmp18B=(void*)
_tmp18A.v;}}_LL4F: return Cyc_Toc_typ_to_c(_tmp18B);_LL50: {struct Cyc_Absyn_VarType_struct*
_tmp18C=(struct Cyc_Absyn_VarType_struct*)_tmp184;if(_tmp18C->tag != 2)goto _LL52;
else{_tmp18D=_tmp18C->f1;}}_LL51: if((Cyc_Tcutil_tvar_kind(_tmp18D,& Cyc_Tcutil_bk))->kind
== Cyc_Absyn_AnyKind)return(void*)& Cyc_Absyn_VoidType_val;else{return Cyc_Absyn_void_star_typ();}
_LL52: {struct Cyc_Absyn_DatatypeType_struct*_tmp18E=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp184;if(_tmp18E->tag != 3)goto _LL54;}_LL53: return(void*)& Cyc_Absyn_VoidType_val;
_LL54: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp18F=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp184;if(_tmp18F->tag != 4)goto _LL56;else{_tmp190=_tmp18F->f1;_tmp191=_tmp190.field_info;
if((_tmp191.KnownDatatypefield).tag != 2)goto _LL56;_tmp192=(struct _tuple1)(
_tmp191.KnownDatatypefield).val;_tmp193=_tmp192.f1;_tmp194=_tmp192.f2;}}_LL55: {
const char*_tmpB3E;return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp194->name,((
_tmpB3E="_struct",_tag_dyneither(_tmpB3E,sizeof(char),8)))));}_LL56: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp195=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp184;if(_tmp195->tag != 4)
goto _LL58;}_LL57: {const char*_tmpB41;void*_tmpB40;(_tmpB40=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB41="unresolved DatatypeFieldType",
_tag_dyneither(_tmpB41,sizeof(char),29))),_tag_dyneither(_tmpB40,sizeof(void*),0)));}
_LL58: {struct Cyc_Absyn_PointerType_struct*_tmp196=(struct Cyc_Absyn_PointerType_struct*)
_tmp184;if(_tmp196->tag != 5)goto _LL5A;else{_tmp197=_tmp196->f1;_tmp198=_tmp197.elt_typ;
_tmp199=_tmp197.elt_tq;_tmp19A=_tmp197.ptr_atts;_tmp19B=_tmp19A.bounds;}}_LL59:
_tmp198=Cyc_Toc_typ_to_c_array(_tmp198);{void*_tmp1E0=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp19B);_LL8B: {
struct Cyc_Absyn_DynEither_b_struct*_tmp1E1=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp1E0;if(_tmp1E1->tag != 0)goto _LL8D;}_LL8C: return Cyc_Toc_dyneither_ptr_typ;
_LL8D:;_LL8E: return Cyc_Absyn_star_typ(_tmp198,(void*)& Cyc_Absyn_HeapRgn_val,
_tmp199,Cyc_Absyn_false_conref);_LL8A:;};_LL5A: {struct Cyc_Absyn_IntType_struct*
_tmp19C=(struct Cyc_Absyn_IntType_struct*)_tmp184;if(_tmp19C->tag != 6)goto _LL5C;}
_LL5B: goto _LL5D;_LL5C: {struct Cyc_Absyn_FloatType_struct*_tmp19D=(struct Cyc_Absyn_FloatType_struct*)
_tmp184;if(_tmp19D->tag != 7)goto _LL5E;}_LL5D: goto _LL5F;_LL5E: {struct Cyc_Absyn_DoubleType_struct*
_tmp19E=(struct Cyc_Absyn_DoubleType_struct*)_tmp184;if(_tmp19E->tag != 8)goto
_LL60;}_LL5F: return t;_LL60: {struct Cyc_Absyn_ArrayType_struct*_tmp19F=(struct Cyc_Absyn_ArrayType_struct*)
_tmp184;if(_tmp19F->tag != 9)goto _LL62;else{_tmp1A0=_tmp19F->f1;_tmp1A1=_tmp1A0.elt_type;
_tmp1A2=_tmp1A0.tq;_tmp1A3=_tmp1A0.num_elts;_tmp1A4=_tmp1A0.zt_loc;}}_LL61:
return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp1A1),_tmp1A2,_tmp1A3,Cyc_Absyn_false_conref,
_tmp1A4);_LL62: {struct Cyc_Absyn_FnType_struct*_tmp1A5=(struct Cyc_Absyn_FnType_struct*)
_tmp184;if(_tmp1A5->tag != 10)goto _LL64;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A6.ret_tqual;
_tmp1A8=_tmp1A6.ret_typ;_tmp1A9=_tmp1A6.args;_tmp1AA=_tmp1A6.c_varargs;_tmp1AB=
_tmp1A6.cyc_varargs;_tmp1AC=_tmp1A6.attributes;}}_LL63: {struct Cyc_List_List*
_tmp1E2=0;for(0;_tmp1AC != 0;_tmp1AC=_tmp1AC->tl){void*_tmp1E3=(void*)_tmp1AC->hd;
_LL90: {struct Cyc_Absyn_Noreturn_att_struct*_tmp1E4=(struct Cyc_Absyn_Noreturn_att_struct*)
_tmp1E3;if(_tmp1E4->tag != 4)goto _LL92;}_LL91: goto _LL93;_LL92: {struct Cyc_Absyn_Const_att_struct*
_tmp1E5=(struct Cyc_Absyn_Const_att_struct*)_tmp1E3;if(_tmp1E5->tag != 5)goto _LL94;}
_LL93: goto _LL95;_LL94: {struct Cyc_Absyn_Format_att_struct*_tmp1E6=(struct Cyc_Absyn_Format_att_struct*)
_tmp1E3;if(_tmp1E6->tag != 19)goto _LL96;}_LL95: continue;_LL96: {struct Cyc_Absyn_Noliveunique_att_struct*
_tmp1E7=(struct Cyc_Absyn_Noliveunique_att_struct*)_tmp1E3;if(_tmp1E7->tag != 21)
goto _LL98;}_LL97: continue;_LL98: {struct Cyc_Absyn_Initializes_att_struct*_tmp1E8=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp1E3;if(_tmp1E8->tag != 20)goto _LL9A;}
_LL99: continue;_LL9A:;_LL9B:{struct Cyc_List_List*_tmpB42;_tmp1E2=((_tmpB42=
_cycalloc(sizeof(*_tmpB42)),((_tmpB42->hd=(void*)_tmp1AC->hd,((_tmpB42->tl=
_tmp1E2,_tmpB42))))));}goto _LL8F;_LL8F:;}{struct Cyc_List_List*_tmp1EA=((struct
Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_arg_to_c,_tmp1A9);if(_tmp1AB != 0){void*_tmp1EB=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp1AB->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple7*_tmpB43;struct _tuple7*_tmp1EC=(_tmpB43=_cycalloc(sizeof(*_tmpB43)),((
_tmpB43->f1=_tmp1AB->name,((_tmpB43->f2=_tmp1AB->tq,((_tmpB43->f3=_tmp1EB,
_tmpB43)))))));struct Cyc_List_List*_tmpB44;_tmp1EA=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1EA,((
_tmpB44=_cycalloc(sizeof(*_tmpB44)),((_tmpB44->hd=_tmp1EC,((_tmpB44->tl=0,
_tmpB44)))))));}{struct Cyc_Absyn_FnType_struct _tmpB4A;struct Cyc_Absyn_FnInfo
_tmpB49;struct Cyc_Absyn_FnType_struct*_tmpB48;return(void*)((_tmpB48=_cycalloc(
sizeof(*_tmpB48)),((_tmpB48[0]=((_tmpB4A.tag=10,((_tmpB4A.f1=((_tmpB49.tvars=0,((
_tmpB49.effect=0,((_tmpB49.ret_tqual=_tmp1A7,((_tmpB49.ret_typ=Cyc_Toc_typ_to_c(
_tmp1A8),((_tmpB49.args=_tmp1EA,((_tmpB49.c_varargs=_tmp1AA,((_tmpB49.cyc_varargs=
0,((_tmpB49.rgn_po=0,((_tmpB49.attributes=_tmp1E2,_tmpB49)))))))))))))))))),
_tmpB4A)))),_tmpB48))));};};}_LL64: {struct Cyc_Absyn_TupleType_struct*_tmp1AD=(
struct Cyc_Absyn_TupleType_struct*)_tmp184;if(_tmp1AD->tag != 11)goto _LL66;else{
_tmp1AE=_tmp1AD->f1;}}_LL65: _tmp1AE=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(
struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1AE);
return Cyc_Toc_add_tuple_type(_tmp1AE);_LL66: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp1AF=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp184;if(_tmp1AF->tag != 13)goto
_LL68;else{_tmp1B0=_tmp1AF->f1;_tmp1B1=_tmp1AF->f2;}}_LL67: {struct Cyc_Absyn_AnonAggrType_struct
_tmpB4D;struct Cyc_Absyn_AnonAggrType_struct*_tmpB4C;return(void*)((_tmpB4C=
_cycalloc(sizeof(*_tmpB4C)),((_tmpB4C[0]=((_tmpB4D.tag=13,((_tmpB4D.f1=_tmp1B0,((
_tmpB4D.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1B1),_tmpB4D)))))),
_tmpB4C))));}_LL68: {struct Cyc_Absyn_AggrType_struct*_tmp1B2=(struct Cyc_Absyn_AggrType_struct*)
_tmp184;if(_tmp1B2->tag != 12)goto _LL6A;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B3.aggr_info;
_tmp1B5=_tmp1B3.targs;}}_LL69:{union Cyc_Absyn_AggrInfoU _tmp1F4=_tmp1B4;struct
_tuple2 _tmp1F5;_LL9D: if((_tmp1F4.UnknownAggr).tag != 1)goto _LL9F;_tmp1F5=(struct
_tuple2)(_tmp1F4.UnknownAggr).val;_LL9E: return t;_LL9F:;_LLA0: goto _LL9C;_LL9C:;}{
struct Cyc_Absyn_Aggrdecl*_tmp1F6=Cyc_Absyn_get_known_aggrdecl(_tmp1B4);if(
_tmp1F6->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(_tmp1F6->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp1F7=_tmp1F6->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1F6->impl))->fields;if(_tmp1F7 == 0)return Cyc_Toc_aggrdecl_type(
_tmp1F6->name,Cyc_Absyn_strctq);for(0;_tmp1F7->tl != 0;_tmp1F7=_tmp1F7->tl){;}{
void*_tmp1F8=((struct Cyc_Absyn_Aggrfield*)_tmp1F7->hd)->type;if(Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp1F8))){struct _RegionHandle _tmp1F9=
_new_region("r");struct _RegionHandle*r=& _tmp1F9;_push_region(r);{void*_tmp1FA=
Cyc_Tcutil_rsubstitute(r,((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,
_tmp1F6->tvs,_tmp1B5),_tmp1F8);if(Cyc_Toc_is_abs_tvar(_tmp1FA)){void*_tmp1FB=Cyc_Toc_aggrdecl_type(
_tmp1F6->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp1FB;}{void*_tmp1FC=
Cyc_Toc_add_struct_type(_tmp1F6->name,_tmp1F6->tvs,_tmp1B5,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1F6->impl))->fields);_npop_handler(0);return _tmp1FC;};};
_pop_region(r);}return Cyc_Toc_aggrdecl_type(_tmp1F6->name,Cyc_Absyn_strctq);};};};
_LL6A: {struct Cyc_Absyn_EnumType_struct*_tmp1B6=(struct Cyc_Absyn_EnumType_struct*)
_tmp184;if(_tmp1B6->tag != 14)goto _LL6C;else{_tmp1B7=_tmp1B6->f1;}}_LL6B: return t;
_LL6C: {struct Cyc_Absyn_AnonEnumType_struct*_tmp1B8=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp184;if(_tmp1B8->tag != 15)goto _LL6E;else{_tmp1B9=_tmp1B8->f1;}}_LL6D: Cyc_Toc_enumfields_to_c(
_tmp1B9);return t;_LL6E: {struct Cyc_Absyn_TypedefType_struct*_tmp1BA=(struct Cyc_Absyn_TypedefType_struct*)
_tmp184;if(_tmp1BA->tag != 18)goto _LL70;else{_tmp1BB=_tmp1BA->f1;_tmp1BC=_tmp1BA->f2;
_tmp1BD=_tmp1BA->f3;_tmp1BE=_tmp1BA->f4;}}_LL6F: if(_tmp1BE == 0  || Cyc_noexpand_r){
if(_tmp1BC != 0){struct Cyc_Absyn_TypedefType_struct _tmpB50;struct Cyc_Absyn_TypedefType_struct*
_tmpB4F;return(void*)((_tmpB4F=_cycalloc(sizeof(*_tmpB4F)),((_tmpB4F[0]=((
_tmpB50.tag=18,((_tmpB50.f1=_tmp1BB,((_tmpB50.f2=0,((_tmpB50.f3=_tmp1BD,((
_tmpB50.f4=0,_tmpB50)))))))))),_tmpB4F))));}else{return t;}}else{struct Cyc_Absyn_TypedefType_struct
_tmpB56;void**_tmpB55;struct Cyc_Absyn_TypedefType_struct*_tmpB54;return(void*)((
_tmpB54=_cycalloc(sizeof(*_tmpB54)),((_tmpB54[0]=((_tmpB56.tag=18,((_tmpB56.f1=
_tmp1BB,((_tmpB56.f2=0,((_tmpB56.f3=_tmp1BD,((_tmpB56.f4=((_tmpB55=_cycalloc(
sizeof(*_tmpB55)),((_tmpB55[0]=Cyc_Toc_typ_to_c_array(*_tmp1BE),_tmpB55)))),
_tmpB56)))))))))),_tmpB54))));}_LL70: {struct Cyc_Absyn_TagType_struct*_tmp1BF=(
struct Cyc_Absyn_TagType_struct*)_tmp184;if(_tmp1BF->tag != 20)goto _LL72;}_LL71:
return Cyc_Absyn_uint_typ;_LL72: {struct Cyc_Absyn_RgnHandleType_struct*_tmp1C0=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp184;if(_tmp1C0->tag != 16)goto _LL74;
else{_tmp1C1=(void*)_tmp1C0->f1;}}_LL73: return Cyc_Toc_rgn_typ();_LL74: {struct
Cyc_Absyn_DynRgnType_struct*_tmp1C2=(struct Cyc_Absyn_DynRgnType_struct*)_tmp184;
if(_tmp1C2->tag != 17)goto _LL76;}_LL75: return Cyc_Toc_dyn_rgn_typ();_LL76: {struct
Cyc_Absyn_HeapRgn_struct*_tmp1C3=(struct Cyc_Absyn_HeapRgn_struct*)_tmp184;if(
_tmp1C3->tag != 21)goto _LL78;}_LL77: {const char*_tmpB59;void*_tmpB58;(_tmpB58=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB59="Toc::typ_to_c: type translation passed the heap region",_tag_dyneither(
_tmpB59,sizeof(char),55))),_tag_dyneither(_tmpB58,sizeof(void*),0)));}_LL78: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp1C4=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp184;if(_tmp1C4->tag != 22)goto _LL7A;}_LL79: {const char*_tmpB5C;void*_tmpB5B;(
_tmpB5B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB5C="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpB5C,sizeof(char),57))),_tag_dyneither(_tmpB5B,sizeof(void*),0)));}
_LL7A: {struct Cyc_Absyn_RefCntRgn_struct*_tmp1C5=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp184;if(_tmp1C5->tag != 23)goto _LL7C;}_LL7B: {const char*_tmpB5F;void*_tmpB5E;(
_tmpB5E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB5F="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpB5F,sizeof(char),57))),_tag_dyneither(_tmpB5E,sizeof(void*),0)));}
_LL7C: {struct Cyc_Absyn_AccessEff_struct*_tmp1C6=(struct Cyc_Absyn_AccessEff_struct*)
_tmp184;if(_tmp1C6->tag != 24)goto _LL7E;}_LL7D: goto _LL7F;_LL7E: {struct Cyc_Absyn_JoinEff_struct*
_tmp1C7=(struct Cyc_Absyn_JoinEff_struct*)_tmp184;if(_tmp1C7->tag != 25)goto _LL80;}
_LL7F: goto _LL81;_LL80: {struct Cyc_Absyn_RgnsEff_struct*_tmp1C8=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp184;if(_tmp1C8->tag != 26)goto _LL82;}_LL81: {const char*_tmpB62;void*_tmpB61;(
_tmpB61=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB62="Toc::typ_to_c: type translation passed an effect",_tag_dyneither(
_tmpB62,sizeof(char),49))),_tag_dyneither(_tmpB61,sizeof(void*),0)));}_LL82: {
struct Cyc_Absyn_ValueofType_struct*_tmp1C9=(struct Cyc_Absyn_ValueofType_struct*)
_tmp184;if(_tmp1C9->tag != 19)goto _LL84;}_LL83: {const char*_tmpB65;void*_tmpB64;(
_tmpB64=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB65="Toc::typ_to_c: type translation passed a valueof_t",_tag_dyneither(
_tmpB65,sizeof(char),51))),_tag_dyneither(_tmpB64,sizeof(void*),0)));}_LL84: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp1CA=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp184;if(_tmp1CA->tag != 27)goto _LL86;else{_tmp1CB=_tmp1CA->f1;_tmp1CC=*_tmp1CB;
_tmp1CD=_tmp1CC.r;{struct Cyc_Absyn_Aggr_td_struct*_tmp1CE=(struct Cyc_Absyn_Aggr_td_struct*)
_tmp1CD;if(_tmp1CE->tag != 0)goto _LL86;else{_tmp1CF=_tmp1CE->f1;}};}}_LL85: Cyc_Toc_aggrdecl_to_c(
_tmp1CF,1);if(_tmp1CF->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(
_tmp1CF->name,Cyc_Absyn_unionq_typ);else{return Cyc_Toc_aggrdecl_type(_tmp1CF->name,
Cyc_Absyn_strctq);}_LL86: {struct Cyc_Absyn_TypeDeclType_struct*_tmp1D0=(struct
Cyc_Absyn_TypeDeclType_struct*)_tmp184;if(_tmp1D0->tag != 27)goto _LL88;else{
_tmp1D1=_tmp1D0->f1;_tmp1D2=*_tmp1D1;_tmp1D3=_tmp1D2.r;{struct Cyc_Absyn_Enum_td_struct*
_tmp1D4=(struct Cyc_Absyn_Enum_td_struct*)_tmp1D3;if(_tmp1D4->tag != 1)goto _LL88;
else{_tmp1D5=_tmp1D4->f1;}};}}_LL87: Cyc_Toc_enumdecl_to_c(_tmp1D5);return t;_LL88: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp1D6=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp184;if(_tmp1D6->tag != 27)goto _LL49;else{_tmp1D7=_tmp1D6->f1;_tmp1D8=*_tmp1D7;
_tmp1D9=_tmp1D8.r;{struct Cyc_Absyn_Datatype_td_struct*_tmp1DA=(struct Cyc_Absyn_Datatype_td_struct*)
_tmp1D9;if(_tmp1DA->tag != 2)goto _LL49;else{_tmp1DB=_tmp1DA->f1;}};_tmp1DC=
_tmp1D6->f2;}}_LL89: Cyc_Toc_datatypedecl_to_c(_tmp1DB);return Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp1DC)));_LL49:;}static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(
void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmp20C=t;struct
Cyc_Absyn_ArrayInfo _tmp20E;void*_tmp20F;struct Cyc_Absyn_Tqual _tmp210;_LLA2: {
struct Cyc_Absyn_ArrayType_struct*_tmp20D=(struct Cyc_Absyn_ArrayType_struct*)
_tmp20C;if(_tmp20D->tag != 9)goto _LLA4;else{_tmp20E=_tmp20D->f1;_tmp20F=_tmp20E.elt_type;
_tmp210=_tmp20E.tq;}}_LLA3: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp20F,(
void*)& Cyc_Absyn_HeapRgn_val,_tmp210,Cyc_Absyn_false_conref),e);_LLA4:;_LLA5:
return Cyc_Toc_cast_it(t,e);_LLA1:;}static int Cyc_Toc_atomic_typ(void*t){void*
_tmp211=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp21C;void*_tmp21D;
struct Cyc_Absyn_AggrInfo _tmp21F;union Cyc_Absyn_AggrInfoU _tmp220;struct Cyc_List_List*
_tmp222;struct Cyc_Absyn_DatatypeFieldInfo _tmp224;union Cyc_Absyn_DatatypeFieldInfoU
_tmp225;struct _tuple1 _tmp226;struct Cyc_Absyn_Datatypedecl*_tmp227;struct Cyc_Absyn_Datatypefield*
_tmp228;struct Cyc_List_List*_tmp22A;_LLA7: {struct Cyc_Absyn_VoidType_struct*
_tmp212=(struct Cyc_Absyn_VoidType_struct*)_tmp211;if(_tmp212->tag != 0)goto _LLA9;}
_LLA8: return 1;_LLA9: {struct Cyc_Absyn_VarType_struct*_tmp213=(struct Cyc_Absyn_VarType_struct*)
_tmp211;if(_tmp213->tag != 2)goto _LLAB;}_LLAA: return 0;_LLAB: {struct Cyc_Absyn_IntType_struct*
_tmp214=(struct Cyc_Absyn_IntType_struct*)_tmp211;if(_tmp214->tag != 6)goto _LLAD;}
_LLAC: goto _LLAE;_LLAD: {struct Cyc_Absyn_EnumType_struct*_tmp215=(struct Cyc_Absyn_EnumType_struct*)
_tmp211;if(_tmp215->tag != 14)goto _LLAF;}_LLAE: goto _LLB0;_LLAF: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp216=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp211;if(_tmp216->tag != 15)goto
_LLB1;}_LLB0: goto _LLB2;_LLB1: {struct Cyc_Absyn_FloatType_struct*_tmp217=(struct
Cyc_Absyn_FloatType_struct*)_tmp211;if(_tmp217->tag != 7)goto _LLB3;}_LLB2: goto
_LLB4;_LLB3: {struct Cyc_Absyn_DoubleType_struct*_tmp218=(struct Cyc_Absyn_DoubleType_struct*)
_tmp211;if(_tmp218->tag != 8)goto _LLB5;}_LLB4: goto _LLB6;_LLB5: {struct Cyc_Absyn_FnType_struct*
_tmp219=(struct Cyc_Absyn_FnType_struct*)_tmp211;if(_tmp219->tag != 10)goto _LLB7;}
_LLB6: goto _LLB8;_LLB7: {struct Cyc_Absyn_TagType_struct*_tmp21A=(struct Cyc_Absyn_TagType_struct*)
_tmp211;if(_tmp21A->tag != 20)goto _LLB9;}_LLB8: return 1;_LLB9: {struct Cyc_Absyn_ArrayType_struct*
_tmp21B=(struct Cyc_Absyn_ArrayType_struct*)_tmp211;if(_tmp21B->tag != 9)goto _LLBB;
else{_tmp21C=_tmp21B->f1;_tmp21D=_tmp21C.elt_type;}}_LLBA: return Cyc_Toc_atomic_typ(
_tmp21D);_LLBB: {struct Cyc_Absyn_AggrType_struct*_tmp21E=(struct Cyc_Absyn_AggrType_struct*)
_tmp211;if(_tmp21E->tag != 12)goto _LLBD;else{_tmp21F=_tmp21E->f1;_tmp220=_tmp21F.aggr_info;}}
_LLBC:{union Cyc_Absyn_AggrInfoU _tmp22F=_tmp220;struct _tuple2 _tmp230;_LLCE: if((
_tmp22F.UnknownAggr).tag != 1)goto _LLD0;_tmp230=(struct _tuple2)(_tmp22F.UnknownAggr).val;
_LLCF: return 0;_LLD0:;_LLD1: goto _LLCD;_LLCD:;}{struct Cyc_Absyn_Aggrdecl*_tmp231=
Cyc_Absyn_get_known_aggrdecl(_tmp220);if(_tmp231->impl == 0)return 0;{struct Cyc_List_List*
_tmp232=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp231->impl))->fields;for(
0;_tmp232 != 0;_tmp232=_tmp232->tl){if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)
_tmp232->hd)->type))return 0;}}return 1;};_LLBD: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp221=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp211;if(_tmp221->tag != 13)goto
_LLBF;else{_tmp222=_tmp221->f2;}}_LLBE: for(0;_tmp222 != 0;_tmp222=_tmp222->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp222->hd)->type))return 0;}
return 1;_LLBF: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp223=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp211;if(_tmp223->tag != 4)goto _LLC1;else{_tmp224=_tmp223->f1;_tmp225=_tmp224.field_info;
if((_tmp225.KnownDatatypefield).tag != 2)goto _LLC1;_tmp226=(struct _tuple1)(
_tmp225.KnownDatatypefield).val;_tmp227=_tmp226.f1;_tmp228=_tmp226.f2;}}_LLC0:
_tmp22A=_tmp228->typs;goto _LLC2;_LLC1: {struct Cyc_Absyn_TupleType_struct*_tmp229=(
struct Cyc_Absyn_TupleType_struct*)_tmp211;if(_tmp229->tag != 11)goto _LLC3;else{
_tmp22A=_tmp229->f1;}}_LLC2: for(0;_tmp22A != 0;_tmp22A=_tmp22A->tl){if(!Cyc_Toc_atomic_typ((*((
struct _tuple9*)_tmp22A->hd)).f2))return 0;}return 1;_LLC3: {struct Cyc_Absyn_DatatypeType_struct*
_tmp22B=(struct Cyc_Absyn_DatatypeType_struct*)_tmp211;if(_tmp22B->tag != 3)goto
_LLC5;}_LLC4: goto _LLC6;_LLC5: {struct Cyc_Absyn_PointerType_struct*_tmp22C=(
struct Cyc_Absyn_PointerType_struct*)_tmp211;if(_tmp22C->tag != 5)goto _LLC7;}_LLC6:
goto _LLC8;_LLC7: {struct Cyc_Absyn_DynRgnType_struct*_tmp22D=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp211;if(_tmp22D->tag != 17)goto _LLC9;}_LLC8: goto _LLCA;_LLC9: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp22E=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp211;if(_tmp22E->tag != 16)goto
_LLCB;}_LLCA: return 0;_LLCB:;_LLCC: {const char*_tmpB69;void*_tmpB68[1];struct Cyc_String_pa_struct
_tmpB67;(_tmpB67.tag=0,((_tmpB67.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpB68[0]=& _tmpB67,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB69="atomic_typ:  bad type %s",
_tag_dyneither(_tmpB69,sizeof(char),25))),_tag_dyneither(_tmpB68,sizeof(void*),1)))))));}
_LLA6:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp236=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp238;void*_tmp239;_LLD3: {struct Cyc_Absyn_PointerType_struct*
_tmp237=(struct Cyc_Absyn_PointerType_struct*)_tmp236;if(_tmp237->tag != 5)goto
_LLD5;else{_tmp238=_tmp237->f1;_tmp239=_tmp238.elt_typ;}}_LLD4: {void*_tmp23A=
Cyc_Tcutil_compress(_tmp239);_LLD8: {struct Cyc_Absyn_VoidType_struct*_tmp23B=(
struct Cyc_Absyn_VoidType_struct*)_tmp23A;if(_tmp23B->tag != 0)goto _LLDA;}_LLD9:
return 1;_LLDA:;_LLDB: return 0;_LLD7:;}_LLD5:;_LLD6: return 0;_LLD2:;}static int Cyc_Toc_is_void_star_or_boxed_tvar(
void*t){return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}static int Cyc_Toc_is_poly_field(
void*t,struct _dyneither_ptr*f){void*_tmp23C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp23E;union Cyc_Absyn_AggrInfoU _tmp23F;struct Cyc_List_List*_tmp241;_LLDD: {
struct Cyc_Absyn_AggrType_struct*_tmp23D=(struct Cyc_Absyn_AggrType_struct*)
_tmp23C;if(_tmp23D->tag != 12)goto _LLDF;else{_tmp23E=_tmp23D->f1;_tmp23F=_tmp23E.aggr_info;}}
_LLDE: {struct Cyc_Absyn_Aggrdecl*_tmp242=Cyc_Absyn_get_known_aggrdecl(_tmp23F);
if(_tmp242->impl == 0){const char*_tmpB6C;void*_tmpB6B;(_tmpB6B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6C="is_poly_field: type missing fields",
_tag_dyneither(_tmpB6C,sizeof(char),35))),_tag_dyneither(_tmpB6B,sizeof(void*),0)));}
_tmp241=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp242->impl))->fields;goto
_LLE0;}_LLDF: {struct Cyc_Absyn_AnonAggrType_struct*_tmp240=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp23C;if(_tmp240->tag != 13)goto _LLE1;else{_tmp241=_tmp240->f2;}}_LLE0: {struct
Cyc_Absyn_Aggrfield*_tmp245=Cyc_Absyn_lookup_field(_tmp241,f);if(_tmp245 == 0){
const char*_tmpB70;void*_tmpB6F[1];struct Cyc_String_pa_struct _tmpB6E;(_tmpB6E.tag=
0,((_tmpB6E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB6F[0]=&
_tmpB6E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB70="is_poly_field: bad field %s",_tag_dyneither(_tmpB70,sizeof(char),28))),
_tag_dyneither(_tmpB6F,sizeof(void*),1)))))));}return Cyc_Toc_is_void_star_or_boxed_tvar(
_tmp245->type);}_LLE1:;_LLE2: {const char*_tmpB74;void*_tmpB73[1];struct Cyc_String_pa_struct
_tmpB72;(_tmpB72.tag=0,((_tmpB72.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpB73[0]=& _tmpB72,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB74="is_poly_field: bad type %s",
_tag_dyneither(_tmpB74,sizeof(char),27))),_tag_dyneither(_tmpB73,sizeof(void*),1)))))));}
_LLDC:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmp24C=e->r;
struct Cyc_Absyn_Exp*_tmp24E;struct _dyneither_ptr*_tmp24F;struct Cyc_Absyn_Exp*
_tmp251;struct _dyneither_ptr*_tmp252;_LLE4: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp24D=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp24C;if(_tmp24D->tag != 22)goto
_LLE6;else{_tmp24E=_tmp24D->f1;_tmp24F=_tmp24D->f2;}}_LLE5: return Cyc_Toc_is_poly_field((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp24E->topt))->v,_tmp24F);_LLE6: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp250=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp24C;if(_tmp250->tag != 23)goto _LLE8;else{_tmp251=_tmp250->f1;_tmp252=_tmp250->f2;}}
_LLE7: {void*_tmp253=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp251->topt))->v);struct Cyc_Absyn_PtrInfo _tmp255;void*_tmp256;
_LLEB: {struct Cyc_Absyn_PointerType_struct*_tmp254=(struct Cyc_Absyn_PointerType_struct*)
_tmp253;if(_tmp254->tag != 5)goto _LLED;else{_tmp255=_tmp254->f1;_tmp256=_tmp255.elt_typ;}}
_LLEC: return Cyc_Toc_is_poly_field(_tmp256,_tmp252);_LLED:;_LLEE: {const char*
_tmpB78;void*_tmpB77[1];struct Cyc_String_pa_struct _tmpB76;(_tmpB76.tag=0,((
_tmpB76.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp251->topt))->v)),((_tmpB77[0]=&
_tmpB76,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB78="is_poly_project: bad type %s",_tag_dyneither(_tmpB78,sizeof(char),29))),
_tag_dyneither(_tmpB77,sizeof(void*),1)))))));}_LLEA:;}_LLE8:;_LLE9: return 0;
_LLE3:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB79;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((
_tmpB79=_cycalloc(sizeof(*_tmpB79)),((_tmpB79->hd=s,((_tmpB79->tl=0,_tmpB79)))))),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB7A;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((
_tmpB7A=_cycalloc(sizeof(*_tmpB7A)),((_tmpB7A->hd=s,((_tmpB7A->tl=0,_tmpB7A)))))),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(
s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmpB7B[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((
_tmpB7B[1]=s,((_tmpB7B[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpB7B,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,
struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*
_tmpB7C[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpB7C[1]=n,((
_tmpB7C[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB7C,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{struct Cyc_Absyn_Exp*
_tmpB7D[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpB7D[1]=n,((
_tmpB7D[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB7D,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n){struct Cyc_Absyn_Exp*_tmpB7E[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((
_tmpB7E[2]=n,((_tmpB7E[1]=s,((_tmpB7E[0]=rgn,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB7E,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmpB7F;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmpB7F=_cycalloc(sizeof(*_tmpB7F)),((_tmpB7F->hd=e,((_tmpB7F->tl=0,_tmpB7F)))))),
0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){struct Cyc_Absyn_Exp*
_tmpB80[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpB80,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*
_tmp262=e->r;union Cyc_Absyn_Cnst _tmp264;struct _dyneither_ptr _tmp265;union Cyc_Absyn_Cnst
_tmp267;struct _dyneither_ptr _tmp268;_LLF0: {struct Cyc_Absyn_Const_e_struct*
_tmp263=(struct Cyc_Absyn_Const_e_struct*)_tmp262;if(_tmp263->tag != 0)goto _LLF2;
else{_tmp264=_tmp263->f1;if((_tmp264.String_c).tag != 8)goto _LLF2;_tmp265=(struct
_dyneither_ptr)(_tmp264.String_c).val;}}_LLF1: is_string=1;goto _LLEF;_LLF2: {
struct Cyc_Absyn_Const_e_struct*_tmp266=(struct Cyc_Absyn_Const_e_struct*)_tmp262;
if(_tmp266->tag != 0)goto _LLF4;else{_tmp267=_tmp266->f1;if((_tmp267.Wstring_c).tag
!= 9)goto _LLF4;_tmp268=(struct _dyneither_ptr)(_tmp267.Wstring_c).val;}}_LLF3:
is_string=1;goto _LLEF;_LLF4:;_LLF5: goto _LLEF;_LLEF:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple0*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_struct*_tmpB86;struct
Cyc_Absyn_Var_d_struct _tmpB85;struct Cyc_List_List*_tmpB84;Cyc_Toc_result_decls=((
_tmpB84=_cycalloc(sizeof(*_tmpB84)),((_tmpB84->hd=Cyc_Absyn_new_decl((void*)((
_tmpB86=_cycalloc(sizeof(*_tmpB86)),((_tmpB86[0]=((_tmpB85.tag=0,((_tmpB85.f1=vd,
_tmpB85)))),_tmpB86)))),0),((_tmpB84->tl=Cyc_Toc_result_decls,_tmpB84))))));}
xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*urm_exp;{struct _tuple15*_tmpB8D;
struct _tuple15*_tmpB8C;struct _tuple15*_tmpB8B;struct _tuple15*_tmpB8A[3];urm_exp=
Cyc_Absyn_unresolvedmem_exp(0,((_tmpB8A[2]=((_tmpB8B=_cycalloc(sizeof(*_tmpB8B)),((
_tmpB8B->f1=0,((_tmpB8B->f2=xplussz,_tmpB8B)))))),((_tmpB8A[1]=((_tmpB8C=
_cycalloc(sizeof(*_tmpB8C)),((_tmpB8C->f1=0,((_tmpB8C->f2=xexp,_tmpB8C)))))),((
_tmpB8A[0]=((_tmpB8D=_cycalloc(sizeof(*_tmpB8D)),((_tmpB8D->f1=0,((_tmpB8D->f2=
xexp,_tmpB8D)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB8A,sizeof(struct _tuple15*),3)))))))),0);}return urm_exp;};};}
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
_tmpB90;struct Cyc_Toc_Env*_tmpB8F;return(_tmpB8F=_region_malloc(r,sizeof(*
_tmpB8F)),((_tmpB8F->break_lab=(struct _dyneither_ptr**)0,((_tmpB8F->continue_lab=(
struct _dyneither_ptr**)0,((_tmpB8F->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((
_tmpB8F->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((
_tmpB8F->toplevel=(int)1,((_tmpB8F->in_lhs=(int*)((_tmpB90=_region_malloc(r,
sizeof(*_tmpB90)),((_tmpB90[0]=0,_tmpB90)))),((_tmpB8F->rgn=(struct _RegionHandle*)
r,_tmpB8F)))))))))))))));}static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp27C;struct
_dyneither_ptr**_tmp27D;struct _dyneither_ptr**_tmp27E;struct Cyc_Toc_FallthruInfo*
_tmp27F;struct Cyc_Dict_Dict _tmp280;int _tmp281;int*_tmp282;struct Cyc_Toc_Env*
_tmp27B=e;_tmp27C=*_tmp27B;_tmp27D=_tmp27C.break_lab;_tmp27E=_tmp27C.continue_lab;
_tmp27F=_tmp27C.fallthru_info;_tmp280=_tmp27C.varmap;_tmp281=_tmp27C.toplevel;
_tmp282=_tmp27C.in_lhs;{struct Cyc_Toc_Env*_tmpB91;return(_tmpB91=_region_malloc(
r,sizeof(*_tmpB91)),((_tmpB91->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp27D),((_tmpB91->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp27E),((_tmpB91->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp27F,((_tmpB91->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp280),((_tmpB91->toplevel=(
int)_tmp281,((_tmpB91->in_lhs=(int*)_tmp282,((_tmpB91->rgn=(struct _RegionHandle*)
r,_tmpB91)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp285;struct
_dyneither_ptr**_tmp286;struct _dyneither_ptr**_tmp287;struct Cyc_Toc_FallthruInfo*
_tmp288;struct Cyc_Dict_Dict _tmp289;int _tmp28A;int*_tmp28B;struct Cyc_Toc_Env*
_tmp284=e;_tmp285=*_tmp284;_tmp286=_tmp285.break_lab;_tmp287=_tmp285.continue_lab;
_tmp288=_tmp285.fallthru_info;_tmp289=_tmp285.varmap;_tmp28A=_tmp285.toplevel;
_tmp28B=_tmp285.in_lhs;{struct Cyc_Toc_Env*_tmpB92;return(_tmpB92=_region_malloc(
r,sizeof(*_tmpB92)),((_tmpB92->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp286),((_tmpB92->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp287),((_tmpB92->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp288,((_tmpB92->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp289),((_tmpB92->toplevel=(
int)0,((_tmpB92->in_lhs=(int*)_tmp28B,((_tmpB92->rgn=(struct _RegionHandle*)r,
_tmpB92)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct
_RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp28E;struct
_dyneither_ptr**_tmp28F;struct _dyneither_ptr**_tmp290;struct Cyc_Toc_FallthruInfo*
_tmp291;struct Cyc_Dict_Dict _tmp292;int _tmp293;int*_tmp294;struct Cyc_Toc_Env*
_tmp28D=e;_tmp28E=*_tmp28D;_tmp28F=_tmp28E.break_lab;_tmp290=_tmp28E.continue_lab;
_tmp291=_tmp28E.fallthru_info;_tmp292=_tmp28E.varmap;_tmp293=_tmp28E.toplevel;
_tmp294=_tmp28E.in_lhs;{struct Cyc_Toc_Env*_tmpB93;return(_tmpB93=_region_malloc(
r,sizeof(*_tmpB93)),((_tmpB93->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp28F),((_tmpB93->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp290),((_tmpB93->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp291,((_tmpB93->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp292),((_tmpB93->toplevel=(
int)1,((_tmpB93->in_lhs=(int*)_tmp294,((_tmpB93->rgn=(struct _RegionHandle*)r,
_tmpB93)))))))))))))));};}static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env _tmp297;int*_tmp298;struct Cyc_Toc_Env*_tmp296=e;_tmp297=*
_tmp296;_tmp298=_tmp297.in_lhs;*_tmp298=b;}static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace _tmp299=(*x).f1;struct Cyc_List_List*_tmp29A;_LLF7: if((
_tmp299.Rel_n).tag != 1)goto _LLF9;_tmp29A=(struct Cyc_List_List*)(_tmp299.Rel_n).val;
_LLF8: {const char*_tmpB97;void*_tmpB96[1];struct Cyc_String_pa_struct _tmpB95;(
_tmpB95.tag=0,((_tmpB95.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
x)),((_tmpB96[0]=& _tmpB95,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB97="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(
_tmpB97,sizeof(char),30))),_tag_dyneither(_tmpB96,sizeof(void*),1)))))));}_LLF9:;
_LLFA: goto _LLF6;_LLF6:;}{struct Cyc_Toc_Env _tmp29F;struct _dyneither_ptr**_tmp2A0;
struct _dyneither_ptr**_tmp2A1;struct Cyc_Toc_FallthruInfo*_tmp2A2;struct Cyc_Dict_Dict
_tmp2A3;int _tmp2A4;int*_tmp2A5;struct Cyc_Toc_Env*_tmp29E=e;_tmp29F=*_tmp29E;
_tmp2A0=_tmp29F.break_lab;_tmp2A1=_tmp29F.continue_lab;_tmp2A2=_tmp29F.fallthru_info;
_tmp2A3=_tmp29F.varmap;_tmp2A4=_tmp29F.toplevel;_tmp2A5=_tmp29F.in_lhs;{struct
Cyc_Dict_Dict _tmp2A6=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*
k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2A3),x,y);struct Cyc_Toc_Env*
_tmpB98;return(_tmpB98=_region_malloc(r,sizeof(*_tmpB98)),((_tmpB98->break_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp2A0),((_tmpB98->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp2A1),((_tmpB98->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp2A2,((_tmpB98->varmap=(struct Cyc_Dict_Dict)
_tmp2A6,((_tmpB98->toplevel=(int)_tmp2A4,((_tmpB98->in_lhs=(int*)_tmp2A5,((
_tmpB98->rgn=(struct _RegionHandle*)r,_tmpB98)))))))))))))));};};}static struct Cyc_Toc_Env*
Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp2A9;struct _dyneither_ptr**_tmp2AA;struct _dyneither_ptr**_tmp2AB;struct Cyc_Toc_FallthruInfo*
_tmp2AC;struct Cyc_Dict_Dict _tmp2AD;int _tmp2AE;int*_tmp2AF;struct Cyc_Toc_Env*
_tmp2A8=e;_tmp2A9=*_tmp2A8;_tmp2AA=_tmp2A9.break_lab;_tmp2AB=_tmp2A9.continue_lab;
_tmp2AC=_tmp2A9.fallthru_info;_tmp2AD=_tmp2A9.varmap;_tmp2AE=_tmp2A9.toplevel;
_tmp2AF=_tmp2A9.in_lhs;{struct Cyc_Toc_Env*_tmpB99;return(_tmpB99=_region_malloc(
r,sizeof(*_tmpB99)),((_tmpB99->break_lab=(struct _dyneither_ptr**)0,((_tmpB99->continue_lab=(
struct _dyneither_ptr**)0,((_tmpB99->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp2AC,((_tmpB99->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2AD),((_tmpB99->toplevel=(
int)_tmp2AE,((_tmpB99->in_lhs=(int*)_tmp2AF,((_tmpB99->rgn=(struct _RegionHandle*)
r,_tmpB99)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmpB9A;fallthru_vars=((
_tmpB9A=_region_malloc(r,sizeof(*_tmpB9A)),((_tmpB9A->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmpB9A->tl=fallthru_vars,_tmpB9A))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp2B3;struct _dyneither_ptr**_tmp2B4;struct
_dyneither_ptr**_tmp2B5;struct Cyc_Toc_FallthruInfo*_tmp2B6;struct Cyc_Dict_Dict
_tmp2B7;int _tmp2B8;int*_tmp2B9;struct Cyc_Toc_Env*_tmp2B2=e;_tmp2B3=*_tmp2B2;
_tmp2B4=_tmp2B3.break_lab;_tmp2B5=_tmp2B3.continue_lab;_tmp2B6=_tmp2B3.fallthru_info;
_tmp2B7=_tmp2B3.varmap;_tmp2B8=_tmp2B3.toplevel;_tmp2B9=_tmp2B3.in_lhs;{struct
Cyc_Toc_Env _tmp2BB;struct Cyc_Dict_Dict _tmp2BC;struct Cyc_Toc_Env*_tmp2BA=
next_case_env;_tmp2BB=*_tmp2BA;_tmp2BC=_tmp2BB.varmap;{struct Cyc_Toc_FallthruInfo*
_tmpB9B;struct Cyc_Toc_FallthruInfo*fi=(_tmpB9B=_region_malloc(r,sizeof(*_tmpB9B)),((
_tmpB9B->label=fallthru_l,((_tmpB9B->binders=fallthru_vars,((_tmpB9B->next_case_env=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp2BC),_tmpB9B)))))));struct _dyneither_ptr**_tmpB9E;struct Cyc_Toc_Env*
_tmpB9D;return(_tmpB9D=_region_malloc(r,sizeof(*_tmpB9D)),((_tmpB9D->break_lab=(
struct _dyneither_ptr**)((_tmpB9E=_region_malloc(r,sizeof(*_tmpB9E)),((_tmpB9E[0]=
break_l,_tmpB9E)))),((_tmpB9D->continue_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp2B5),((_tmpB9D->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
fi,((_tmpB9D->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2B7),((_tmpB9D->toplevel=(
int)_tmp2B8,((_tmpB9D->in_lhs=(int*)((int*)_tmp2B9),((_tmpB9D->rgn=(struct
_RegionHandle*)r,_tmpB9D)))))))))))))));};};};}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){struct
Cyc_Toc_Env _tmp2C1;struct _dyneither_ptr**_tmp2C2;struct _dyneither_ptr**_tmp2C3;
struct Cyc_Toc_FallthruInfo*_tmp2C4;struct Cyc_Dict_Dict _tmp2C5;int _tmp2C6;int*
_tmp2C7;struct Cyc_Toc_Env*_tmp2C0=e;_tmp2C1=*_tmp2C0;_tmp2C2=_tmp2C1.break_lab;
_tmp2C3=_tmp2C1.continue_lab;_tmp2C4=_tmp2C1.fallthru_info;_tmp2C5=_tmp2C1.varmap;
_tmp2C6=_tmp2C1.toplevel;_tmp2C7=_tmp2C1.in_lhs;{struct _dyneither_ptr**_tmpBA1;
struct Cyc_Toc_Env*_tmpBA0;return(_tmpBA0=_region_malloc(r,sizeof(*_tmpBA0)),((
_tmpBA0->break_lab=(struct _dyneither_ptr**)((_tmpBA1=_region_malloc(r,sizeof(*
_tmpBA1)),((_tmpBA1[0]=break_l,_tmpBA1)))),((_tmpBA0->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp2C3),((_tmpBA0->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpBA0->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2C5),((_tmpBA0->toplevel=(
int)_tmp2C6,((_tmpBA0->in_lhs=(int*)_tmp2C7,((_tmpBA0->rgn=(struct _RegionHandle*)
r,_tmpBA0)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp2CB;struct _dyneither_ptr**_tmp2CC;struct _dyneither_ptr**_tmp2CD;struct Cyc_Toc_FallthruInfo*
_tmp2CE;struct Cyc_Dict_Dict _tmp2CF;int _tmp2D0;int*_tmp2D1;struct Cyc_Toc_Env*
_tmp2CA=e;_tmp2CB=*_tmp2CA;_tmp2CC=_tmp2CB.break_lab;_tmp2CD=_tmp2CB.continue_lab;
_tmp2CE=_tmp2CB.fallthru_info;_tmp2CF=_tmp2CB.varmap;_tmp2D0=_tmp2CB.toplevel;
_tmp2D1=_tmp2CB.in_lhs;{struct Cyc_Toc_FallthruInfo*_tmpBA4;struct Cyc_Toc_Env*
_tmpBA3;return(_tmpBA3=_region_malloc(r,sizeof(*_tmpBA3)),((_tmpBA3->break_lab=(
struct _dyneither_ptr**)0,((_tmpBA3->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp2CD),((_tmpBA3->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((
_tmpBA4=_region_malloc(r,sizeof(*_tmpBA4)),((_tmpBA4->label=next_l,((_tmpBA4->binders=
0,((_tmpBA4->next_case_env=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*
cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),
_tmpBA4)))))))),((_tmpBA3->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2CF),((_tmpBA3->toplevel=(
int)_tmp2D0,((_tmpBA3->in_lhs=(int*)((int*)_tmp2D1),((_tmpBA3->rgn=(struct
_RegionHandle*)r,_tmpBA3)))))))))))))));};}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp2D4=e->annot;_LLFC: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2D5=(
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp2D4;if(_tmp2D5->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLFE;}_LLFD: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLFE: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp2D6=(
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp2D4;if(_tmp2D6->tag != Cyc_CfFlowInfo_NotZero)
goto _LL100;}_LLFF: return 0;_LL100: {struct Cyc_CfFlowInfo_IsZero_struct*_tmp2D7=(
struct Cyc_CfFlowInfo_IsZero_struct*)_tmp2D4;if(_tmp2D7->tag != Cyc_CfFlowInfo_IsZero)
goto _LL102;}_LL101:{const char*_tmpBA7;void*_tmpBA6;(_tmpBA6=0,Cyc_Tcutil_terr(e->loc,((
_tmpBA7="dereference of NULL pointer",_tag_dyneither(_tmpBA7,sizeof(char),28))),
_tag_dyneither(_tmpBA6,sizeof(void*),0)));}return 0;_LL102: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp2D8=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp2D4;if(_tmp2D8->tag != Cyc_Absyn_EmptyAnnot)
goto _LL104;}_LL103: return 0;_LL104:;_LL105: {const char*_tmpBAA;void*_tmpBA9;(
_tmpBA9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBAA="need_null_check",_tag_dyneither(_tmpBAA,sizeof(char),16))),
_tag_dyneither(_tmpBA9,sizeof(void*),0)));}_LLFB:;}static struct Cyc_List_List*Cyc_Toc_get_relns(
struct Cyc_Absyn_Exp*e){void*_tmp2DD=e->annot;struct Cyc_List_List*_tmp2DF;struct
Cyc_List_List*_tmp2E1;_LL107: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2DE=(
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp2DD;if(_tmp2DE->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LL109;else{_tmp2DF=_tmp2DE->f1;}}_LL108: return _tmp2DF;_LL109: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmp2E0=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmp2DD;if(_tmp2E0->tag != Cyc_CfFlowInfo_NotZero)
goto _LL10B;else{_tmp2E1=_tmp2E0->f1;}}_LL10A: return _tmp2E1;_LL10B: {struct Cyc_CfFlowInfo_IsZero_struct*
_tmp2E2=(struct Cyc_CfFlowInfo_IsZero_struct*)_tmp2DD;if(_tmp2E2->tag != Cyc_CfFlowInfo_IsZero)
goto _LL10D;}_LL10C:{const char*_tmpBAD;void*_tmpBAC;(_tmpBAC=0,Cyc_Tcutil_terr(e->loc,((
_tmpBAD="dereference of NULL pointer",_tag_dyneither(_tmpBAD,sizeof(char),28))),
_tag_dyneither(_tmpBAC,sizeof(void*),0)));}return 0;_LL10D: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp2E3=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp2DD;if(_tmp2E3->tag != Cyc_Absyn_EmptyAnnot)
goto _LL10F;}_LL10E: return 0;_LL10F:;_LL110: {const char*_tmpBB0;void*_tmpBAF;(
_tmpBAF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBB0="get_relns",_tag_dyneither(_tmpBB0,sizeof(char),10))),_tag_dyneither(
_tmpBAF,sizeof(void*),0)));}_LL106:;}static int Cyc_Toc_check_const_array(
unsigned int i,void*t){void*_tmp2E8=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp2EA;struct Cyc_Absyn_PtrAtts _tmp2EB;union Cyc_Absyn_Constraint*_tmp2EC;union
Cyc_Absyn_Constraint*_tmp2ED;struct Cyc_Absyn_ArrayInfo _tmp2EF;struct Cyc_Absyn_Exp*
_tmp2F0;_LL112: {struct Cyc_Absyn_PointerType_struct*_tmp2E9=(struct Cyc_Absyn_PointerType_struct*)
_tmp2E8;if(_tmp2E9->tag != 5)goto _LL114;else{_tmp2EA=_tmp2E9->f1;_tmp2EB=_tmp2EA.ptr_atts;
_tmp2EC=_tmp2EB.bounds;_tmp2ED=_tmp2EB.zero_term;}}_LL113: {void*_tmp2F1=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp2EC);struct Cyc_Absyn_Exp*_tmp2F4;_LL119: {struct Cyc_Absyn_DynEither_b_struct*
_tmp2F2=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2F1;if(_tmp2F2->tag != 0)goto
_LL11B;}_LL11A: return 0;_LL11B: {struct Cyc_Absyn_Upper_b_struct*_tmp2F3=(struct
Cyc_Absyn_Upper_b_struct*)_tmp2F1;if(_tmp2F3->tag != 1)goto _LL118;else{_tmp2F4=
_tmp2F3->f1;}}_LL11C: {unsigned int _tmp2F6;int _tmp2F7;struct _tuple10 _tmp2F5=Cyc_Evexp_eval_const_uint_exp(
_tmp2F4);_tmp2F6=_tmp2F5.f1;_tmp2F7=_tmp2F5.f2;return _tmp2F7  && i <= _tmp2F6;}
_LL118:;}_LL114: {struct Cyc_Absyn_ArrayType_struct*_tmp2EE=(struct Cyc_Absyn_ArrayType_struct*)
_tmp2E8;if(_tmp2EE->tag != 9)goto _LL116;else{_tmp2EF=_tmp2EE->f1;_tmp2F0=_tmp2EF.num_elts;}}
_LL115: if(_tmp2F0 == 0)return 0;{unsigned int _tmp2F9;int _tmp2FA;struct _tuple10
_tmp2F8=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp2F0);_tmp2F9=
_tmp2F8.f1;_tmp2FA=_tmp2F8.f2;return _tmp2FA  && i <= _tmp2F9;};_LL116:;_LL117:
return 0;_LL111:;}static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2FB=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(
_tmp2FB->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp _tmp2FC=_tmp2FB->rop;struct
Cyc_Absyn_Vardecl*_tmp2FD;struct Cyc_Absyn_Vardecl*_tmp2FE;_LL11E: if((_tmp2FC.LessNumelts).tag
!= 3)goto _LL120;_tmp2FD=(struct Cyc_Absyn_Vardecl*)(_tmp2FC.LessNumelts).val;
_LL11F: _tmp2FE=_tmp2FD;goto _LL121;_LL120: if((_tmp2FC.LessEqNumelts).tag != 5)goto
_LL122;_tmp2FE=(struct Cyc_Absyn_Vardecl*)(_tmp2FC.LessEqNumelts).val;_LL121: if(
_tmp2FE == v)return 1;else{goto _LL11D;}_LL122:;_LL123: continue;_LL11D:;};}return 0;}
static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Exp*e,void*vtype){{void*_tmp2FF=e->r;void*_tmp301;struct Cyc_Absyn_Vardecl*
_tmp303;void*_tmp305;struct Cyc_Absyn_Vardecl*_tmp307;void*_tmp309;struct Cyc_Absyn_Vardecl*
_tmp30B;void*_tmp30D;struct Cyc_Absyn_Vardecl*_tmp30F;union Cyc_Absyn_Cnst _tmp311;
struct _tuple5 _tmp312;enum Cyc_Absyn_Sign _tmp313;int _tmp314;union Cyc_Absyn_Cnst
_tmp316;struct _tuple5 _tmp317;enum Cyc_Absyn_Sign _tmp318;int _tmp319;union Cyc_Absyn_Cnst
_tmp31B;struct _tuple5 _tmp31C;enum Cyc_Absyn_Sign _tmp31D;int _tmp31E;enum Cyc_Absyn_Primop
_tmp320;struct Cyc_List_List*_tmp321;struct Cyc_List_List _tmp322;struct Cyc_Absyn_Exp*
_tmp323;_LL125: {struct Cyc_Absyn_Var_e_struct*_tmp300=(struct Cyc_Absyn_Var_e_struct*)
_tmp2FF;if(_tmp300->tag != 1)goto _LL127;else{_tmp301=(void*)_tmp300->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp302=(struct Cyc_Absyn_Pat_b_struct*)_tmp301;if(_tmp302->tag
!= 5)goto _LL127;else{_tmp303=_tmp302->f1;}};}}_LL126: _tmp307=_tmp303;goto _LL128;
_LL127: {struct Cyc_Absyn_Var_e_struct*_tmp304=(struct Cyc_Absyn_Var_e_struct*)
_tmp2FF;if(_tmp304->tag != 1)goto _LL129;else{_tmp305=(void*)_tmp304->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp306=(struct Cyc_Absyn_Local_b_struct*)_tmp305;if(
_tmp306->tag != 4)goto _LL129;else{_tmp307=_tmp306->f1;}};}}_LL128: _tmp30B=_tmp307;
goto _LL12A;_LL129: {struct Cyc_Absyn_Var_e_struct*_tmp308=(struct Cyc_Absyn_Var_e_struct*)
_tmp2FF;if(_tmp308->tag != 1)goto _LL12B;else{_tmp309=(void*)_tmp308->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp30A=(struct Cyc_Absyn_Global_b_struct*)_tmp309;if(
_tmp30A->tag != 1)goto _LL12B;else{_tmp30B=_tmp30A->f1;}};}}_LL12A: _tmp30F=_tmp30B;
goto _LL12C;_LL12B: {struct Cyc_Absyn_Var_e_struct*_tmp30C=(struct Cyc_Absyn_Var_e_struct*)
_tmp2FF;if(_tmp30C->tag != 1)goto _LL12D;else{_tmp30D=(void*)_tmp30C->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp30E=(struct Cyc_Absyn_Param_b_struct*)_tmp30D;if(
_tmp30E->tag != 3)goto _LL12D;else{_tmp30F=_tmp30E->f1;}};}}_LL12C: if(_tmp30F->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp30F))return 1;goto _LL124;_LL12D: {
struct Cyc_Absyn_Const_e_struct*_tmp310=(struct Cyc_Absyn_Const_e_struct*)_tmp2FF;
if(_tmp310->tag != 0)goto _LL12F;else{_tmp311=_tmp310->f1;if((_tmp311.Int_c).tag != 
5)goto _LL12F;_tmp312=(struct _tuple5)(_tmp311.Int_c).val;_tmp313=_tmp312.f1;if(
_tmp313 != Cyc_Absyn_None)goto _LL12F;_tmp314=_tmp312.f2;}}_LL12E: _tmp319=_tmp314;
goto _LL130;_LL12F: {struct Cyc_Absyn_Const_e_struct*_tmp315=(struct Cyc_Absyn_Const_e_struct*)
_tmp2FF;if(_tmp315->tag != 0)goto _LL131;else{_tmp316=_tmp315->f1;if((_tmp316.Int_c).tag
!= 5)goto _LL131;_tmp317=(struct _tuple5)(_tmp316.Int_c).val;_tmp318=_tmp317.f1;
if(_tmp318 != Cyc_Absyn_Signed)goto _LL131;_tmp319=_tmp317.f2;}}_LL130: return
_tmp319 >= 0  && Cyc_Toc_check_const_array((unsigned int)_tmp319,vtype);_LL131: {
struct Cyc_Absyn_Const_e_struct*_tmp31A=(struct Cyc_Absyn_Const_e_struct*)_tmp2FF;
if(_tmp31A->tag != 0)goto _LL133;else{_tmp31B=_tmp31A->f1;if((_tmp31B.Int_c).tag != 
5)goto _LL133;_tmp31C=(struct _tuple5)(_tmp31B.Int_c).val;_tmp31D=_tmp31C.f1;if(
_tmp31D != Cyc_Absyn_Unsigned)goto _LL133;_tmp31E=_tmp31C.f2;}}_LL132: return Cyc_Toc_check_const_array((
unsigned int)_tmp31E,vtype);_LL133: {struct Cyc_Absyn_Primop_e_struct*_tmp31F=(
struct Cyc_Absyn_Primop_e_struct*)_tmp2FF;if(_tmp31F->tag != 3)goto _LL135;else{
_tmp320=_tmp31F->f1;if(_tmp320 != Cyc_Absyn_Numelts)goto _LL135;_tmp321=_tmp31F->f2;
if(_tmp321 == 0)goto _LL135;_tmp322=*_tmp321;_tmp323=(struct Cyc_Absyn_Exp*)_tmp322.hd;}}
_LL134:{void*_tmp324=_tmp323->r;void*_tmp326;struct Cyc_Absyn_Vardecl*_tmp328;
void*_tmp32A;struct Cyc_Absyn_Vardecl*_tmp32C;void*_tmp32E;struct Cyc_Absyn_Vardecl*
_tmp330;void*_tmp332;struct Cyc_Absyn_Vardecl*_tmp334;_LL138: {struct Cyc_Absyn_Var_e_struct*
_tmp325=(struct Cyc_Absyn_Var_e_struct*)_tmp324;if(_tmp325->tag != 1)goto _LL13A;
else{_tmp326=(void*)_tmp325->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp327=(struct Cyc_Absyn_Pat_b_struct*)
_tmp326;if(_tmp327->tag != 5)goto _LL13A;else{_tmp328=_tmp327->f1;}};}}_LL139:
_tmp32C=_tmp328;goto _LL13B;_LL13A: {struct Cyc_Absyn_Var_e_struct*_tmp329=(struct
Cyc_Absyn_Var_e_struct*)_tmp324;if(_tmp329->tag != 1)goto _LL13C;else{_tmp32A=(
void*)_tmp329->f2;{struct Cyc_Absyn_Local_b_struct*_tmp32B=(struct Cyc_Absyn_Local_b_struct*)
_tmp32A;if(_tmp32B->tag != 4)goto _LL13C;else{_tmp32C=_tmp32B->f1;}};}}_LL13B:
_tmp330=_tmp32C;goto _LL13D;_LL13C: {struct Cyc_Absyn_Var_e_struct*_tmp32D=(struct
Cyc_Absyn_Var_e_struct*)_tmp324;if(_tmp32D->tag != 1)goto _LL13E;else{_tmp32E=(
void*)_tmp32D->f2;{struct Cyc_Absyn_Global_b_struct*_tmp32F=(struct Cyc_Absyn_Global_b_struct*)
_tmp32E;if(_tmp32F->tag != 1)goto _LL13E;else{_tmp330=_tmp32F->f1;}};}}_LL13D:
_tmp334=_tmp330;goto _LL13F;_LL13E: {struct Cyc_Absyn_Var_e_struct*_tmp331=(struct
Cyc_Absyn_Var_e_struct*)_tmp324;if(_tmp331->tag != 1)goto _LL140;else{_tmp332=(
void*)_tmp331->f2;{struct Cyc_Absyn_Param_b_struct*_tmp333=(struct Cyc_Absyn_Param_b_struct*)
_tmp332;if(_tmp333->tag != 3)goto _LL140;else{_tmp334=_tmp333->f1;}};}}_LL13F:
return _tmp334 == v;_LL140:;_LL141: goto _LL137;_LL137:;}goto _LL124;_LL135:;_LL136:
goto _LL124;_LL124:;}return 0;}static char _tmp346[8]="*bogus*";struct _tuple18{void*
f1;void*f2;};static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,
struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){struct Cyc_Absyn_Vardecl*x;{void*
_tmp335=a->r;void*_tmp337;struct Cyc_Absyn_Vardecl*_tmp339;void*_tmp33B;struct Cyc_Absyn_Vardecl*
_tmp33D;void*_tmp33F;struct Cyc_Absyn_Vardecl*_tmp341;void*_tmp343;struct Cyc_Absyn_Vardecl*
_tmp345;_LL143: {struct Cyc_Absyn_Var_e_struct*_tmp336=(struct Cyc_Absyn_Var_e_struct*)
_tmp335;if(_tmp336->tag != 1)goto _LL145;else{_tmp337=(void*)_tmp336->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp338=(struct Cyc_Absyn_Pat_b_struct*)_tmp337;if(_tmp338->tag
!= 5)goto _LL145;else{_tmp339=_tmp338->f1;}};}}_LL144: _tmp33D=_tmp339;goto _LL146;
_LL145: {struct Cyc_Absyn_Var_e_struct*_tmp33A=(struct Cyc_Absyn_Var_e_struct*)
_tmp335;if(_tmp33A->tag != 1)goto _LL147;else{_tmp33B=(void*)_tmp33A->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp33C=(struct Cyc_Absyn_Local_b_struct*)_tmp33B;if(
_tmp33C->tag != 4)goto _LL147;else{_tmp33D=_tmp33C->f1;}};}}_LL146: _tmp341=_tmp33D;
goto _LL148;_LL147: {struct Cyc_Absyn_Var_e_struct*_tmp33E=(struct Cyc_Absyn_Var_e_struct*)
_tmp335;if(_tmp33E->tag != 1)goto _LL149;else{_tmp33F=(void*)_tmp33E->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp340=(struct Cyc_Absyn_Global_b_struct*)_tmp33F;if(
_tmp340->tag != 1)goto _LL149;else{_tmp341=_tmp340->f1;}};}}_LL148: _tmp345=_tmp341;
goto _LL14A;_LL149: {struct Cyc_Absyn_Var_e_struct*_tmp342=(struct Cyc_Absyn_Var_e_struct*)
_tmp335;if(_tmp342->tag != 1)goto _LL14B;else{_tmp343=(void*)_tmp342->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp344=(struct Cyc_Absyn_Param_b_struct*)_tmp343;if(
_tmp344->tag != 3)goto _LL14B;else{_tmp345=_tmp344->f1;}};}}_LL14A: if(_tmp345->escapes)
goto _LL14C;x=_tmp345;goto _LL142;_LL14B:;_LL14C: {static struct _dyneither_ptr
bogus_string={_tmp346,_tmp346,_tmp346 + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
4,0}},& bogus_string};static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,&
bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};x=& bogus_vardecl;
x->type=a_typ;}_LL142:;}{void*_tmp347=a_typ;inner_loop: {void*_tmp348=i->r;void*
_tmp34A;struct Cyc_Absyn_Exp*_tmp34B;union Cyc_Absyn_Cnst _tmp34D;struct _tuple5
_tmp34E;enum Cyc_Absyn_Sign _tmp34F;int _tmp350;union Cyc_Absyn_Cnst _tmp352;struct
_tuple5 _tmp353;enum Cyc_Absyn_Sign _tmp354;int _tmp355;union Cyc_Absyn_Cnst _tmp357;
struct _tuple5 _tmp358;enum Cyc_Absyn_Sign _tmp359;int _tmp35A;enum Cyc_Absyn_Primop
_tmp35C;struct Cyc_List_List*_tmp35D;struct Cyc_List_List _tmp35E;struct Cyc_Absyn_Exp*
_tmp35F;struct Cyc_List_List*_tmp360;struct Cyc_List_List _tmp361;struct Cyc_Absyn_Exp*
_tmp362;void*_tmp364;struct Cyc_Absyn_Vardecl*_tmp366;void*_tmp368;struct Cyc_Absyn_Vardecl*
_tmp36A;void*_tmp36C;struct Cyc_Absyn_Vardecl*_tmp36E;void*_tmp370;struct Cyc_Absyn_Vardecl*
_tmp372;_LL14E: {struct Cyc_Absyn_Cast_e_struct*_tmp349=(struct Cyc_Absyn_Cast_e_struct*)
_tmp348;if(_tmp349->tag != 15)goto _LL150;else{_tmp34A=(void*)_tmp349->f1;_tmp34B=
_tmp349->f2;}}_LL14F: i=_tmp34B;goto inner_loop;_LL150: {struct Cyc_Absyn_Const_e_struct*
_tmp34C=(struct Cyc_Absyn_Const_e_struct*)_tmp348;if(_tmp34C->tag != 0)goto _LL152;
else{_tmp34D=_tmp34C->f1;if((_tmp34D.Int_c).tag != 5)goto _LL152;_tmp34E=(struct
_tuple5)(_tmp34D.Int_c).val;_tmp34F=_tmp34E.f1;if(_tmp34F != Cyc_Absyn_None)goto
_LL152;_tmp350=_tmp34E.f2;}}_LL151: _tmp355=_tmp350;goto _LL153;_LL152: {struct Cyc_Absyn_Const_e_struct*
_tmp351=(struct Cyc_Absyn_Const_e_struct*)_tmp348;if(_tmp351->tag != 0)goto _LL154;
else{_tmp352=_tmp351->f1;if((_tmp352.Int_c).tag != 5)goto _LL154;_tmp353=(struct
_tuple5)(_tmp352.Int_c).val;_tmp354=_tmp353.f1;if(_tmp354 != Cyc_Absyn_Signed)
goto _LL154;_tmp355=_tmp353.f2;}}_LL153: return _tmp355 >= 0  && Cyc_Toc_check_const_array((
unsigned int)(_tmp355 + 1),_tmp347);_LL154: {struct Cyc_Absyn_Const_e_struct*
_tmp356=(struct Cyc_Absyn_Const_e_struct*)_tmp348;if(_tmp356->tag != 0)goto _LL156;
else{_tmp357=_tmp356->f1;if((_tmp357.Int_c).tag != 5)goto _LL156;_tmp358=(struct
_tuple5)(_tmp357.Int_c).val;_tmp359=_tmp358.f1;if(_tmp359 != Cyc_Absyn_Unsigned)
goto _LL156;_tmp35A=_tmp358.f2;}}_LL155: return Cyc_Toc_check_const_array((
unsigned int)(_tmp35A + 1),_tmp347);_LL156: {struct Cyc_Absyn_Primop_e_struct*
_tmp35B=(struct Cyc_Absyn_Primop_e_struct*)_tmp348;if(_tmp35B->tag != 3)goto _LL158;
else{_tmp35C=_tmp35B->f1;if(_tmp35C != Cyc_Absyn_Mod)goto _LL158;_tmp35D=_tmp35B->f2;
if(_tmp35D == 0)goto _LL158;_tmp35E=*_tmp35D;_tmp35F=(struct Cyc_Absyn_Exp*)_tmp35E.hd;
_tmp360=_tmp35E.tl;if(_tmp360 == 0)goto _LL158;_tmp361=*_tmp360;_tmp362=(struct Cyc_Absyn_Exp*)
_tmp361.hd;}}_LL157: return Cyc_Toc_check_leq_size(relns,x,_tmp362,_tmp347);_LL158: {
struct Cyc_Absyn_Var_e_struct*_tmp363=(struct Cyc_Absyn_Var_e_struct*)_tmp348;if(
_tmp363->tag != 1)goto _LL15A;else{_tmp364=(void*)_tmp363->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp365=(struct Cyc_Absyn_Pat_b_struct*)_tmp364;if(_tmp365->tag != 5)goto _LL15A;
else{_tmp366=_tmp365->f1;}};}}_LL159: _tmp36A=_tmp366;goto _LL15B;_LL15A: {struct
Cyc_Absyn_Var_e_struct*_tmp367=(struct Cyc_Absyn_Var_e_struct*)_tmp348;if(_tmp367->tag
!= 1)goto _LL15C;else{_tmp368=(void*)_tmp367->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp369=(struct Cyc_Absyn_Local_b_struct*)_tmp368;if(_tmp369->tag != 4)goto _LL15C;
else{_tmp36A=_tmp369->f1;}};}}_LL15B: _tmp36E=_tmp36A;goto _LL15D;_LL15C: {struct
Cyc_Absyn_Var_e_struct*_tmp36B=(struct Cyc_Absyn_Var_e_struct*)_tmp348;if(_tmp36B->tag
!= 1)goto _LL15E;else{_tmp36C=(void*)_tmp36B->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp36D=(struct Cyc_Absyn_Global_b_struct*)_tmp36C;if(_tmp36D->tag != 1)goto _LL15E;
else{_tmp36E=_tmp36D->f1;}};}}_LL15D: _tmp372=_tmp36E;goto _LL15F;_LL15E: {struct
Cyc_Absyn_Var_e_struct*_tmp36F=(struct Cyc_Absyn_Var_e_struct*)_tmp348;if(_tmp36F->tag
!= 1)goto _LL160;else{_tmp370=(void*)_tmp36F->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp371=(struct Cyc_Absyn_Param_b_struct*)_tmp370;if(_tmp371->tag != 3)goto _LL160;
else{_tmp372=_tmp371->f1;}};}}_LL15F: if(_tmp372->escapes)return 0;{struct Cyc_List_List*
_tmp373=relns;for(0;_tmp373 != 0;_tmp373=_tmp373->tl){struct Cyc_CfFlowInfo_Reln*
_tmp374=(struct Cyc_CfFlowInfo_Reln*)_tmp373->hd;if(_tmp374->vd == _tmp372){union
Cyc_CfFlowInfo_RelnOp _tmp375=_tmp374->rop;struct Cyc_Absyn_Vardecl*_tmp376;struct
_tuple11 _tmp377;struct Cyc_Absyn_Vardecl*_tmp378;void*_tmp379;unsigned int _tmp37A;
unsigned int _tmp37B;_LL163: if((_tmp375.LessNumelts).tag != 3)goto _LL165;_tmp376=(
struct Cyc_Absyn_Vardecl*)(_tmp375.LessNumelts).val;_LL164: if(x == _tmp376)return 1;
else{goto _LL162;}_LL165: if((_tmp375.LessVar).tag != 2)goto _LL167;_tmp377=(struct
_tuple11)(_tmp375.LessVar).val;_tmp378=_tmp377.f1;_tmp379=_tmp377.f2;_LL166:{
struct _tuple18 _tmpBB1;struct _tuple18 _tmp37D=(_tmpBB1.f1=Cyc_Tcutil_compress(
_tmp379),((_tmpBB1.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(a->topt))->v),_tmpBB1)));void*_tmp37E;void*_tmp380;void*_tmp381;
struct Cyc_Absyn_PtrInfo _tmp383;struct Cyc_Absyn_PtrAtts _tmp384;union Cyc_Absyn_Constraint*
_tmp385;_LL16E: _tmp37E=_tmp37D.f1;{struct Cyc_Absyn_TagType_struct*_tmp37F=(
struct Cyc_Absyn_TagType_struct*)_tmp37E;if(_tmp37F->tag != 20)goto _LL170;else{
_tmp380=(void*)_tmp37F->f1;}};_tmp381=_tmp37D.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp382=(struct Cyc_Absyn_PointerType_struct*)_tmp381;if(_tmp382->tag != 5)goto
_LL170;else{_tmp383=_tmp382->f1;_tmp384=_tmp383.ptr_atts;_tmp385=_tmp384.bounds;}};
_LL16F:{void*_tmp386=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp385);struct Cyc_Absyn_Exp*_tmp388;_LL173: {struct Cyc_Absyn_Upper_b_struct*
_tmp387=(struct Cyc_Absyn_Upper_b_struct*)_tmp386;if(_tmp387->tag != 1)goto _LL175;
else{_tmp388=_tmp387->f1;}}_LL174: {struct Cyc_Absyn_Exp*_tmp389=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp380,0),0,Cyc_Absyn_No_coercion,0);
if(Cyc_Evexp_lte_const_exp(_tmp389,_tmp388))return 1;goto _LL172;}_LL175:;_LL176:
goto _LL172;_LL172:;}goto _LL16D;_LL170:;_LL171: goto _LL16D;_LL16D:;}{struct Cyc_List_List*
_tmp38A=relns;for(0;_tmp38A != 0;_tmp38A=_tmp38A->tl){struct Cyc_CfFlowInfo_Reln*
_tmp38B=(struct Cyc_CfFlowInfo_Reln*)_tmp38A->hd;if(_tmp38B->vd == _tmp378){union
Cyc_CfFlowInfo_RelnOp _tmp38C=_tmp38B->rop;struct Cyc_Absyn_Vardecl*_tmp38D;struct
Cyc_Absyn_Vardecl*_tmp38E;unsigned int _tmp38F;unsigned int _tmp390;struct _tuple11
_tmp391;struct Cyc_Absyn_Vardecl*_tmp392;_LL178: if((_tmp38C.LessEqNumelts).tag != 
5)goto _LL17A;_tmp38D=(struct Cyc_Absyn_Vardecl*)(_tmp38C.LessEqNumelts).val;
_LL179: _tmp38E=_tmp38D;goto _LL17B;_LL17A: if((_tmp38C.LessNumelts).tag != 3)goto
_LL17C;_tmp38E=(struct Cyc_Absyn_Vardecl*)(_tmp38C.LessNumelts).val;_LL17B: if(x == 
_tmp38E)return 1;goto _LL177;_LL17C: if((_tmp38C.EqualConst).tag != 1)goto _LL17E;
_tmp38F=(unsigned int)(_tmp38C.EqualConst).val;_LL17D: return Cyc_Toc_check_const_array(
_tmp38F,_tmp347);_LL17E: if((_tmp38C.LessEqConst).tag != 6)goto _LL180;_tmp390=(
unsigned int)(_tmp38C.LessEqConst).val;if(!(_tmp390 > 0))goto _LL180;_LL17F: return
Cyc_Toc_check_const_array(_tmp390,_tmp347);_LL180: if((_tmp38C.LessVar).tag != 2)
goto _LL182;_tmp391=(struct _tuple11)(_tmp38C.LessVar).val;_tmp392=_tmp391.f1;
_LL181: if(Cyc_Toc_check_leq_size_var(relns,x,_tmp392))return 1;goto _LL177;_LL182:;
_LL183: goto _LL177;_LL177:;}}}goto _LL162;_LL167: if((_tmp375.LessConst).tag != 4)
goto _LL169;_tmp37A=(unsigned int)(_tmp375.LessConst).val;_LL168: return Cyc_Toc_check_const_array(
_tmp37A,_tmp347);_LL169: if((_tmp375.LessEqConst).tag != 6)goto _LL16B;_tmp37B=(
unsigned int)(_tmp375.LessEqConst).val;_LL16A: return Cyc_Toc_check_const_array(
_tmp37B + 1,_tmp347);_LL16B:;_LL16C: goto _LL162;_LL162:;}}}goto _LL14D;_LL160:;
_LL161: goto _LL14D;_LL14D:;}return 0;};}static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*
e){if(e->topt == 0){const char*_tmpBB4;void*_tmpBB3;(_tmpBB3=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB4="Missing type in primop ",
_tag_dyneither(_tmpBB4,sizeof(char),24))),_tag_dyneither(_tmpBB3,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*
_tmpBB7;void*_tmpBB6;(_tmpBB6=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB7="Missing type in primop ",
_tag_dyneither(_tmpBB7,sizeof(char),24))),_tag_dyneither(_tmpBB6,sizeof(void*),0)));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple9*
Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){struct _tuple9*_tmpBB8;return(_tmpBB8=
_cycalloc(sizeof(*_tmpBB8)),((_tmpBB8->f1=Cyc_Toc_mt_tq,((_tmpBB8->f2=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmpBB8)))));}static struct
_tuple15*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple15*_tmpBB9;return(_tmpBB9=_cycalloc(sizeof(*_tmpBB9)),((
_tmpBB9->f1=0,((_tmpBB9->f2=e,_tmpBB9)))));};}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(
struct Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int
pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;
if(pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp399=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0  || Cyc_Absyn_no_regions)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(
_tmp399): Cyc_Toc_malloc_ptr(_tmp399));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp399);}};}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp39A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp39A != 0;_tmp39A=_tmp39A->tl){struct _tuple15 _tmp39C;struct Cyc_List_List*
_tmp39D;struct Cyc_Absyn_Exp*_tmp39E;struct _tuple15*_tmp39B=(struct _tuple15*)
_tmp39A->hd;_tmp39C=*_tmp39B;_tmp39D=_tmp39C.f1;_tmp39E=_tmp39C.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp39D == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp39D->tl != 0){const char*_tmpBBC;void*_tmpBBB;(_tmpBBB=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBBC="multiple designators in array",
_tag_dyneither(_tmpBBC,sizeof(char),30))),_tag_dyneither(_tmpBBB,sizeof(void*),0)));}{
void*_tmp3A1=(void*)_tmp39D->hd;void*_tmp3A2=_tmp3A1;struct Cyc_Absyn_Exp*_tmp3A4;
_LL185: {struct Cyc_Absyn_ArrayElement_struct*_tmp3A3=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp3A2;if(_tmp3A3->tag != 0)goto _LL187;else{_tmp3A4=_tmp3A3->f1;}}_LL186: Cyc_Toc_exp_to_c(
nv,_tmp3A4);e_index=_tmp3A4;goto _LL184;_LL187: {struct Cyc_Absyn_FieldName_struct*
_tmp3A5=(struct Cyc_Absyn_FieldName_struct*)_tmp3A2;if(_tmp3A5->tag != 1)goto
_LL184;}_LL188: {const char*_tmpBBF;void*_tmpBBE;(_tmpBBE=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBBF="field name designators in array",
_tag_dyneither(_tmpBBF,sizeof(char),32))),_tag_dyneither(_tmpBBE,sizeof(void*),0)));}
_LL184:;};}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*
_tmp3A8=_tmp39E->r;struct Cyc_List_List*_tmp3AA;struct Cyc_Absyn_Vardecl*_tmp3AC;
struct Cyc_Absyn_Exp*_tmp3AD;struct Cyc_Absyn_Exp*_tmp3AE;int _tmp3AF;void*_tmp3B1;
struct Cyc_List_List*_tmp3B2;_LL18A: {struct Cyc_Absyn_Array_e_struct*_tmp3A9=(
struct Cyc_Absyn_Array_e_struct*)_tmp3A8;if(_tmp3A9->tag != 27)goto _LL18C;else{
_tmp3AA=_tmp3A9->f1;}}_LL18B: s=Cyc_Toc_init_array(nv,lval,_tmp3AA,s);goto _LL189;
_LL18C: {struct Cyc_Absyn_Comprehension_e_struct*_tmp3AB=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3A8;if(_tmp3AB->tag != 28)goto _LL18E;else{_tmp3AC=_tmp3AB->f1;_tmp3AD=_tmp3AB->f2;
_tmp3AE=_tmp3AB->f3;_tmp3AF=_tmp3AB->f4;}}_LL18D: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp3AC,_tmp3AD,_tmp3AE,_tmp3AF,s,0);goto _LL189;_LL18E: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3B0=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3A8;if(_tmp3B0->tag != 30)goto
_LL190;else{_tmp3B1=(void*)_tmp3B0->f1;_tmp3B2=_tmp3B0->f2;}}_LL18F: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3B1,_tmp3B2,s);goto _LL189;_LL190:;_LL191: Cyc_Toc_exp_to_c(nv,_tmp39E);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
_tmp39E,0),s,0);goto _LL189;_LL189:;};};}}return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple0*_tmp3B3=vd->name;void*_tmp3B4=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env _tmp3B6;struct _RegionHandle*_tmp3B7;
struct Cyc_Toc_Env*_tmp3B5=nv;_tmp3B6=*_tmp3B5;_tmp3B7=_tmp3B6.rgn;{struct Cyc_Absyn_Local_b_struct
_tmpBC2;struct Cyc_Absyn_Local_b_struct*_tmpBC1;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
_tmp3B7,nv,_tmp3B3,Cyc_Absyn_varb_exp(_tmp3B3,(void*)((_tmpBC1=_cycalloc(sizeof(*
_tmpBC1)),((_tmpBC1[0]=((_tmpBC2.tag=4,((_tmpBC2.f1=vd,_tmpBC2)))),_tmpBC1)))),0));
struct _tuple0*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp3B3,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp3B3,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp3B3,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp3B3,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp3B8=e2->r;struct Cyc_List_List*_tmp3BA;
struct Cyc_Absyn_Vardecl*_tmp3BC;struct Cyc_Absyn_Exp*_tmp3BD;struct Cyc_Absyn_Exp*
_tmp3BE;int _tmp3BF;void*_tmp3C1;struct Cyc_List_List*_tmp3C2;_LL193: {struct Cyc_Absyn_Array_e_struct*
_tmp3B9=(struct Cyc_Absyn_Array_e_struct*)_tmp3B8;if(_tmp3B9->tag != 27)goto _LL195;
else{_tmp3BA=_tmp3B9->f1;}}_LL194: body=Cyc_Toc_init_array(nv2,lval,_tmp3BA,Cyc_Toc_skip_stmt_dl());
goto _LL192;_LL195: {struct Cyc_Absyn_Comprehension_e_struct*_tmp3BB=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3B8;if(_tmp3BB->tag != 28)goto _LL197;else{_tmp3BC=_tmp3BB->f1;_tmp3BD=_tmp3BB->f2;
_tmp3BE=_tmp3BB->f3;_tmp3BF=_tmp3BB->f4;}}_LL196: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp3BC,_tmp3BD,_tmp3BE,_tmp3BF,Cyc_Toc_skip_stmt_dl(),0);goto _LL192;
_LL197: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp3C0=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp3B8;if(_tmp3C0->tag != 30)goto _LL199;else{_tmp3C1=(void*)_tmp3C0->f1;_tmp3C2=
_tmp3C0->f2;}}_LL198: body=Cyc_Toc_init_anon_struct(nv,lval,_tmp3C1,_tmp3C2,Cyc_Toc_skip_stmt_dl());
goto _LL192;_LL199:;_LL19A: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL192;_LL192:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp3B4,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp3B3,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp3C5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp3C5 != 0;_tmp3C5=_tmp3C5->tl){struct _tuple15 _tmp3C7;
struct Cyc_List_List*_tmp3C8;struct Cyc_Absyn_Exp*_tmp3C9;struct _tuple15*_tmp3C6=(
struct _tuple15*)_tmp3C5->hd;_tmp3C7=*_tmp3C6;_tmp3C8=_tmp3C7.f1;_tmp3C9=_tmp3C7.f2;
if(_tmp3C8 == 0){const char*_tmpBC5;void*_tmpBC4;(_tmpBC4=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC5="empty designator list",
_tag_dyneither(_tmpBC5,sizeof(char),22))),_tag_dyneither(_tmpBC4,sizeof(void*),0)));}
if(_tmp3C8->tl != 0){const char*_tmpBC8;void*_tmpBC7;(_tmpBC7=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC8="too many designators in anonymous struct",
_tag_dyneither(_tmpBC8,sizeof(char),41))),_tag_dyneither(_tmpBC7,sizeof(void*),0)));}{
void*_tmp3CE=(void*)_tmp3C8->hd;struct _dyneither_ptr*_tmp3D0;_LL19C: {struct Cyc_Absyn_FieldName_struct*
_tmp3CF=(struct Cyc_Absyn_FieldName_struct*)_tmp3CE;if(_tmp3CF->tag != 1)goto
_LL19E;else{_tmp3D0=_tmp3CF->f1;}}_LL19D: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(
lhs,_tmp3D0,0);{void*_tmp3D1=_tmp3C9->r;struct Cyc_List_List*_tmp3D3;struct Cyc_Absyn_Vardecl*
_tmp3D5;struct Cyc_Absyn_Exp*_tmp3D6;struct Cyc_Absyn_Exp*_tmp3D7;int _tmp3D8;void*
_tmp3DA;struct Cyc_List_List*_tmp3DB;_LL1A1: {struct Cyc_Absyn_Array_e_struct*
_tmp3D2=(struct Cyc_Absyn_Array_e_struct*)_tmp3D1;if(_tmp3D2->tag != 27)goto _LL1A3;
else{_tmp3D3=_tmp3D2->f1;}}_LL1A2: s=Cyc_Toc_init_array(nv,lval,_tmp3D3,s);goto
_LL1A0;_LL1A3: {struct Cyc_Absyn_Comprehension_e_struct*_tmp3D4=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3D1;if(_tmp3D4->tag != 28)goto _LL1A5;else{_tmp3D5=_tmp3D4->f1;_tmp3D6=_tmp3D4->f2;
_tmp3D7=_tmp3D4->f3;_tmp3D8=_tmp3D4->f4;}}_LL1A4: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp3D5,_tmp3D6,_tmp3D7,_tmp3D8,s,0);goto _LL1A0;_LL1A5: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3D9=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3D1;if(_tmp3D9->tag != 30)goto
_LL1A7;else{_tmp3DA=(void*)_tmp3D9->f1;_tmp3DB=_tmp3D9->f2;}}_LL1A6: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3DA,_tmp3DB,s);goto _LL1A0;_LL1A7:;_LL1A8: Cyc_Toc_exp_to_c(nv,_tmp3C9);
if(Cyc_Toc_is_poly_field(struct_type,_tmp3D0))_tmp3C9=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp3C9);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp3C9,0),0),s,0);goto _LL1A0;_LL1A0:;}goto _LL19B;}_LL19E:;_LL19F: {const char*
_tmpBCB;void*_tmpBCA;(_tmpBCA=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBCB="array designator in struct",
_tag_dyneither(_tmpBCB,sizeof(char),27))),_tag_dyneither(_tmpBCA,sizeof(void*),0)));}
_LL19B:;};}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct Cyc_Toc_Env
_tmp3DF;struct _RegionHandle*_tmp3E0;struct Cyc_Toc_Env*_tmp3DE=nv;_tmp3DF=*
_tmp3DE;_tmp3E0=_tmp3DF.rgn;{struct Cyc_List_List*_tmp3E1=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
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
struct Cyc_Absyn_Exp*_tmp3ED;struct Cyc_Absyn_Exp*_tmp3EE;int _tmp3EF;_LL1AA: {
struct Cyc_Absyn_Array_e_struct*_tmp3E9=(struct Cyc_Absyn_Array_e_struct*)_tmp3E8;
if(_tmp3E9->tag != 27)goto _LL1AC;else{_tmp3EA=_tmp3E9->f1;}}_LL1AB: _tmp3E5=Cyc_Toc_init_array(
nv,lval,_tmp3EA,_tmp3E5);goto _LL1A9;_LL1AC: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp3EB=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp3E8;if(_tmp3EB->tag != 28)
goto _LL1AE;else{_tmp3EC=_tmp3EB->f1;_tmp3ED=_tmp3EB->f2;_tmp3EE=_tmp3EB->f3;
_tmp3EF=_tmp3EB->f4;}}_LL1AD: _tmp3E5=Cyc_Toc_init_comprehension(nv,lval,_tmp3EC,
_tmp3ED,_tmp3EE,_tmp3EF,_tmp3E5,0);goto _LL1A9;_LL1AE:;_LL1AF: Cyc_Toc_exp_to_c(nv,
e);_tmp3E5=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3E6(
_tmp3E4,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp3E5,0);goto _LL1A9;_LL1A9:;};}}
return Cyc_Toc_make_struct(nv,_tmp3E3,_tmp3E2,_tmp3E5,pointer,rgnopt,is_atomic);};}
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct
_dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp3F0=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp3F0 != 0;_tmp3F0=_tmp3F0->tl){struct Cyc_Absyn_Aggrfield*
_tmp3F1=(struct Cyc_Absyn_Aggrfield*)_tmp3F0->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp3F1->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}{
struct Cyc_String_pa_struct _tmpBD3;void*_tmpBD2[1];const char*_tmpBD1;void*_tmpBD0;(
_tmpBD0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)((_tmpBD3.tag=0,((_tmpBD3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmpBD2[0]=& _tmpBD3,Cyc_aprintf(((_tmpBD1="get_member_offset %s failed",
_tag_dyneither(_tmpBD1,sizeof(char),28))),_tag_dyneither(_tmpBD2,sizeof(void*),1)))))))),
_tag_dyneither(_tmpBD0,sizeof(void*),0)));};}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){struct _tuple0*_tmp3F6=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3F7=Cyc_Absyn_var_exp(_tmp3F6,0);struct Cyc_Absyn_Stmt*
_tmp3F8=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp3F7),0);struct Cyc_Absyn_Exp*(*
_tmp3F9)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*)=
pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;void*_tmp3FA=Cyc_Toc_typ_to_c(
struct_type);int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp3FB=Cyc_Tcutil_compress(
struct_type);struct Cyc_Absyn_AggrInfo _tmp3FD;union Cyc_Absyn_AggrInfoU _tmp3FE;
_LL1B1: {struct Cyc_Absyn_AggrType_struct*_tmp3FC=(struct Cyc_Absyn_AggrType_struct*)
_tmp3FB;if(_tmp3FC->tag != 12)goto _LL1B3;else{_tmp3FD=_tmp3FC->f1;_tmp3FE=_tmp3FD.aggr_info;}}
_LL1B2: ad=Cyc_Absyn_get_known_aggrdecl(_tmp3FE);goto _LL1B0;_LL1B3:;_LL1B4: {
const char*_tmpBD6;void*_tmpBD5;(_tmpBD5=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD6="init_struct: bad struct type",
_tag_dyneither(_tmpBD6,sizeof(char),29))),_tag_dyneither(_tmpBD5,sizeof(void*),0)));}
_LL1B0:;}{int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct Cyc_Toc_Env _tmp402;struct _RegionHandle*_tmp403;struct Cyc_Toc_Env*_tmp401=
nv;_tmp402=*_tmp401;_tmp403=_tmp402.rgn;{struct Cyc_List_List*_tmp404=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp403,dles);for(0;
_tmp404 != 0;_tmp404=_tmp404->tl){struct _tuple15 _tmp406;struct Cyc_List_List*
_tmp407;struct Cyc_Absyn_Exp*_tmp408;struct _tuple15*_tmp405=(struct _tuple15*)
_tmp404->hd;_tmp406=*_tmp405;_tmp407=_tmp406.f1;_tmp408=_tmp406.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp408->topt))->v);
if(_tmp407 == 0){const char*_tmpBD9;void*_tmpBD8;(_tmpBD8=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD9="empty designator list",
_tag_dyneither(_tmpBD9,sizeof(char),22))),_tag_dyneither(_tmpBD8,sizeof(void*),0)));}
if(_tmp407->tl != 0){struct _tuple0*_tmp40B=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp40C=Cyc_Absyn_var_exp(_tmp40B,0);for(0;_tmp407 != 0;_tmp407=_tmp407->tl){void*
_tmp40D=(void*)_tmp407->hd;struct _dyneither_ptr*_tmp40F;_LL1B6: {struct Cyc_Absyn_FieldName_struct*
_tmp40E=(struct Cyc_Absyn_FieldName_struct*)_tmp40D;if(_tmp40E->tag != 1)goto
_LL1B8;else{_tmp40F=_tmp40E->f1;}}_LL1B7: if(Cyc_Toc_is_poly_field(struct_type,
_tmp40F))_tmp40C=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp40C);_tmp3F8=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3F9(_tmp3F7,_tmp40F,0),_tmp40C,0),0),
_tmp3F8,0);goto _LL1B5;_LL1B8:;_LL1B9: {const char*_tmpBDC;void*_tmpBDB;(_tmpBDB=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBDC="array designator in struct",_tag_dyneither(_tmpBDC,sizeof(char),27))),
_tag_dyneither(_tmpBDB,sizeof(void*),0)));}_LL1B5:;}Cyc_Toc_exp_to_c(nv,_tmp408);
_tmp3F8=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp40C,
_tmp408,0),0),_tmp3F8,0);}else{void*_tmp412=(void*)_tmp407->hd;struct
_dyneither_ptr*_tmp414;_LL1BB: {struct Cyc_Absyn_FieldName_struct*_tmp413=(struct
Cyc_Absyn_FieldName_struct*)_tmp412;if(_tmp413->tag != 1)goto _LL1BD;else{_tmp414=
_tmp413->f1;}}_LL1BC: {struct Cyc_Absyn_Exp*lval=_tmp3F9(_tmp3F7,_tmp414,0);if(
is_tagged_union){int i=Cyc_Toc_get_member_offset(ad,_tmp414);struct Cyc_Absyn_Exp*
f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_aggrmember_exp(
lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,
f_tag_exp,0);_tmp3F8=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3F8,0);
lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{void*_tmp415=_tmp408->r;
struct Cyc_List_List*_tmp417;struct Cyc_Absyn_Vardecl*_tmp419;struct Cyc_Absyn_Exp*
_tmp41A;struct Cyc_Absyn_Exp*_tmp41B;int _tmp41C;void*_tmp41E;struct Cyc_List_List*
_tmp41F;_LL1C0: {struct Cyc_Absyn_Array_e_struct*_tmp416=(struct Cyc_Absyn_Array_e_struct*)
_tmp415;if(_tmp416->tag != 27)goto _LL1C2;else{_tmp417=_tmp416->f1;}}_LL1C1:
_tmp3F8=Cyc_Toc_init_array(nv,lval,_tmp417,_tmp3F8);goto _LL1BF;_LL1C2: {struct
Cyc_Absyn_Comprehension_e_struct*_tmp418=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp415;if(_tmp418->tag != 28)goto _LL1C4;else{_tmp419=_tmp418->f1;_tmp41A=_tmp418->f2;
_tmp41B=_tmp418->f3;_tmp41C=_tmp418->f4;}}_LL1C3: _tmp3F8=Cyc_Toc_init_comprehension(
nv,lval,_tmp419,_tmp41A,_tmp41B,_tmp41C,_tmp3F8,0);goto _LL1BF;_LL1C4: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp41D=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp415;if(_tmp41D->tag != 30)goto
_LL1C6;else{_tmp41E=(void*)_tmp41D->f1;_tmp41F=_tmp41D->f2;}}_LL1C5: _tmp3F8=Cyc_Toc_init_anon_struct(
nv,lval,_tmp41E,_tmp41F,_tmp3F8);goto _LL1BF;_LL1C6:;_LL1C7: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp408->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp408);{struct Cyc_Absyn_Aggrfield*_tmp420=Cyc_Absyn_lookup_decl_field(ad,
_tmp414);if(Cyc_Toc_is_poly_field(struct_type,_tmp414) && !was_ptr_type)_tmp408=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp408);if(has_exists)_tmp408=Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp420))->type),
_tmp408);_tmp3F8=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp408,0),0),_tmp3F8,0);goto _LL1BF;};}_LL1BF:;}goto _LL1BA;}_LL1BD:;_LL1BE: {
const char*_tmpBDF;void*_tmpBDE;(_tmpBDE=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBDF="array designator in struct",
_tag_dyneither(_tmpBDF,sizeof(char),27))),_tag_dyneither(_tmpBDE,sizeof(void*),0)));}
_LL1BA:;}}}return Cyc_Toc_make_struct(nv,_tmp3F6,_tmp3FA,_tmp3F8,pointer,rgnopt,
is_atomic);};}struct _tuple19{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct
_tuple19*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(
e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,
enum Cyc_Absyn_Incrementor incr){struct Cyc_Absyn_Increment_e_struct _tmpBE2;struct
Cyc_Absyn_Increment_e_struct*_tmpBE1;return Cyc_Absyn_new_exp((void*)((_tmpBE1=
_cycalloc(sizeof(*_tmpBE1)),((_tmpBE1[0]=((_tmpBE2.tag=5,((_tmpBE2.f1=e1,((
_tmpBE2.f2=incr,_tmpBE2)))))),_tmpBE1)))),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp425=e1->r;struct Cyc_Absyn_Stmt*_tmp427;void*_tmp429;struct Cyc_Absyn_Exp*
_tmp42A;struct Cyc_Absyn_Exp*_tmp42C;struct _dyneither_ptr*_tmp42D;int _tmp42E;int
_tmp42F;_LL1C9: {struct Cyc_Absyn_StmtExp_e_struct*_tmp426=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp425;if(_tmp426->tag != 37)goto _LL1CB;else{_tmp427=_tmp426->f1;}}_LL1CA: Cyc_Toc_lvalue_assign_stmt(
_tmp427,fs,f,f_env);goto _LL1C8;_LL1CB: {struct Cyc_Absyn_Cast_e_struct*_tmp428=(
struct Cyc_Absyn_Cast_e_struct*)_tmp425;if(_tmp428->tag != 15)goto _LL1CD;else{
_tmp429=(void*)_tmp428->f1;_tmp42A=_tmp428->f2;}}_LL1CC: Cyc_Toc_lvalue_assign(
_tmp42A,fs,f,f_env);goto _LL1C8;_LL1CD: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp42B=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp425;if(_tmp42B->tag != 22)goto
_LL1CF;else{_tmp42C=_tmp42B->f1;_tmp42D=_tmp42B->f2;_tmp42E=_tmp42B->f3;_tmp42F=
_tmp42B->f4;}}_LL1CE: e1->r=_tmp42C->r;{struct Cyc_List_List*_tmpBE3;Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)((_tmpBE3=_cycalloc(sizeof(*_tmpBE3)),((_tmpBE3->hd=
_tmp42D,((_tmpBE3->tl=fs,_tmpBE3)))))),f,f_env);}goto _LL1C8;_LL1CF:;_LL1D0: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;goto _LL1C8;}_LL1C8:;}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env){void*_tmp431=s->r;struct Cyc_Absyn_Exp*_tmp433;struct Cyc_Absyn_Decl*
_tmp435;struct Cyc_Absyn_Stmt*_tmp436;struct Cyc_Absyn_Stmt*_tmp438;_LL1D2: {
struct Cyc_Absyn_Exp_s_struct*_tmp432=(struct Cyc_Absyn_Exp_s_struct*)_tmp431;if(
_tmp432->tag != 1)goto _LL1D4;else{_tmp433=_tmp432->f1;}}_LL1D3: Cyc_Toc_lvalue_assign(
_tmp433,fs,f,f_env);goto _LL1D1;_LL1D4: {struct Cyc_Absyn_Decl_s_struct*_tmp434=(
struct Cyc_Absyn_Decl_s_struct*)_tmp431;if(_tmp434->tag != 12)goto _LL1D6;else{
_tmp435=_tmp434->f1;_tmp436=_tmp434->f2;}}_LL1D5: Cyc_Toc_lvalue_assign_stmt(
_tmp436,fs,f,f_env);goto _LL1D1;_LL1D6: {struct Cyc_Absyn_Seq_s_struct*_tmp437=(
struct Cyc_Absyn_Seq_s_struct*)_tmp431;if(_tmp437->tag != 2)goto _LL1D8;else{
_tmp438=_tmp437->f2;}}_LL1D7: Cyc_Toc_lvalue_assign_stmt(_tmp438,fs,f,f_env);goto
_LL1D1;_LL1D8:;_LL1D9: {const char*_tmpBE7;void*_tmpBE6[1];struct Cyc_String_pa_struct
_tmpBE5;(_tmpBE5.tag=0,((_tmpBE5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmpBE6[0]=& _tmpBE5,Cyc_Toc_toc_impos(((_tmpBE7="lvalue_assign_stmt: %s",
_tag_dyneither(_tmpBE7,sizeof(char),23))),_tag_dyneither(_tmpBE6,sizeof(void*),1)))))));}
_LL1D1:;}static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);static
struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){void*_tmp43C=
e->r;void*_tmp43E;void**_tmp43F;struct Cyc_Absyn_Exp*_tmp440;struct Cyc_Absyn_Exp**
_tmp441;struct Cyc_Absyn_Exp*_tmp443;struct Cyc_Absyn_Stmt*_tmp445;_LL1DB: {struct
Cyc_Absyn_Cast_e_struct*_tmp43D=(struct Cyc_Absyn_Cast_e_struct*)_tmp43C;if(
_tmp43D->tag != 15)goto _LL1DD;else{_tmp43E=(void**)& _tmp43D->f1;_tmp43F=(void**)((
void**)& _tmp43D->f1);_tmp440=_tmp43D->f2;_tmp441=(struct Cyc_Absyn_Exp**)& _tmp43D->f2;}}
_LL1DC:*_tmp441=Cyc_Toc_push_address_exp(*_tmp441);*_tmp43F=Cyc_Absyn_cstar_typ(*
_tmp43F,Cyc_Toc_mt_tq);return e;_LL1DD: {struct Cyc_Absyn_Deref_e_struct*_tmp442=(
struct Cyc_Absyn_Deref_e_struct*)_tmp43C;if(_tmp442->tag != 21)goto _LL1DF;else{
_tmp443=_tmp442->f1;}}_LL1DE: return _tmp443;_LL1DF: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp444=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp43C;if(_tmp444->tag != 37)goto
_LL1E1;else{_tmp445=_tmp444->f1;}}_LL1E0: Cyc_Toc_push_address_stmt(_tmp445);
return e;_LL1E1:;_LL1E2: if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpBEB;void*_tmpBEA[1];struct Cyc_String_pa_struct _tmpBE9;(_tmpBE9.tag=
0,((_tmpBE9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpBEA[0]=& _tmpBE9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpBEB="can't take & of exp %s",_tag_dyneither(_tmpBEB,
sizeof(char),23))),_tag_dyneither(_tmpBEA,sizeof(void*),1)))))));};_LL1DA:;}
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){void*_tmp449=s->r;
struct Cyc_Absyn_Stmt*_tmp44B;struct Cyc_Absyn_Stmt*_tmp44D;struct Cyc_Absyn_Exp*
_tmp44F;struct Cyc_Absyn_Exp**_tmp450;_LL1E4: {struct Cyc_Absyn_Seq_s_struct*
_tmp44A=(struct Cyc_Absyn_Seq_s_struct*)_tmp449;if(_tmp44A->tag != 2)goto _LL1E6;
else{_tmp44B=_tmp44A->f2;}}_LL1E5: _tmp44D=_tmp44B;goto _LL1E7;_LL1E6: {struct Cyc_Absyn_Decl_s_struct*
_tmp44C=(struct Cyc_Absyn_Decl_s_struct*)_tmp449;if(_tmp44C->tag != 12)goto _LL1E8;
else{_tmp44D=_tmp44C->f2;}}_LL1E7: Cyc_Toc_push_address_stmt(_tmp44D);goto _LL1E3;
_LL1E8: {struct Cyc_Absyn_Exp_s_struct*_tmp44E=(struct Cyc_Absyn_Exp_s_struct*)
_tmp449;if(_tmp44E->tag != 1)goto _LL1EA;else{_tmp44F=_tmp44E->f1;_tmp450=(struct
Cyc_Absyn_Exp**)& _tmp44E->f1;}}_LL1E9:*_tmp450=Cyc_Toc_push_address_exp(*_tmp450);
goto _LL1E3;_LL1EA:;_LL1EB: {const char*_tmpBEF;void*_tmpBEE[1];struct Cyc_String_pa_struct
_tmpBED;(_tmpBED.tag=0,((_tmpBED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmpBEE[0]=& _tmpBED,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBEF="can't take & of stmt %s",
_tag_dyneither(_tmpBEF,sizeof(char),24))),_tag_dyneither(_tmpBEE,sizeof(void*),1)))))));}
_LL1E3:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;
struct Cyc_List_List*prev;if(x == 0)return 0;{struct Cyc_List_List*_tmpBF0;result=((
_tmpBF0=_region_malloc(r2,sizeof(*_tmpBF0)),((_tmpBF0->hd=(void*)f((void*)x->hd,
env),((_tmpBF0->tl=0,_tmpBF0))))));}prev=result;for(x=x->tl;x != 0;x=x->tl){{
struct Cyc_List_List*_tmpBF1;((struct Cyc_List_List*)_check_null(prev))->tl=((
_tmpBF1=_region_malloc(r2,sizeof(*_tmpBF1)),((_tmpBF1->hd=(void*)f((void*)x->hd,
env),((_tmpBF1->tl=0,_tmpBF1))))));}prev=prev->tl;}return result;}static struct Cyc_List_List*
Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region,f,env,x);}static struct _tuple15*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*
e){struct _tuple15*_tmpBF2;return(_tmpBF2=_cycalloc(sizeof(*_tmpBF2)),((_tmpBF2->f1=
0,((_tmpBF2->f2=e,_tmpBF2)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(
void*t){void*_tmp457=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp459;
_LL1ED: {struct Cyc_Absyn_PointerType_struct*_tmp458=(struct Cyc_Absyn_PointerType_struct*)
_tmp457;if(_tmp458->tag != 5)goto _LL1EF;else{_tmp459=_tmp458->f1;}}_LL1EE: return
_tmp459;_LL1EF:;_LL1F0: {const char*_tmpBF5;void*_tmpBF4;(_tmpBF4=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF5="get_ptr_typ: not a pointer!",
_tag_dyneither(_tmpBF5,sizeof(char),28))),_tag_dyneither(_tmpBF4,sizeof(void*),0)));}
_LL1EC:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp45C=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp45F;enum Cyc_Absyn_Size_of
_tmp460;enum Cyc_Absyn_Sign _tmp462;enum Cyc_Absyn_Size_of _tmp463;enum Cyc_Absyn_Sign
_tmp467;enum Cyc_Absyn_Size_of _tmp468;enum Cyc_Absyn_Sign _tmp46A;enum Cyc_Absyn_Size_of
_tmp46B;enum Cyc_Absyn_Sign _tmp46D;enum Cyc_Absyn_Size_of _tmp46E;_LL1F2: {struct
Cyc_Absyn_PointerType_struct*_tmp45D=(struct Cyc_Absyn_PointerType_struct*)
_tmp45C;if(_tmp45D->tag != 5)goto _LL1F4;}_LL1F3: res=Cyc_Absyn_null_exp(0);goto
_LL1F1;_LL1F4: {struct Cyc_Absyn_IntType_struct*_tmp45E=(struct Cyc_Absyn_IntType_struct*)
_tmp45C;if(_tmp45E->tag != 6)goto _LL1F6;else{_tmp45F=_tmp45E->f1;_tmp460=_tmp45E->f2;
if(_tmp460 != Cyc_Absyn_Char_sz)goto _LL1F6;}}_LL1F5: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(
_tmp45F,'\000'),0);goto _LL1F1;_LL1F6: {struct Cyc_Absyn_IntType_struct*_tmp461=(
struct Cyc_Absyn_IntType_struct*)_tmp45C;if(_tmp461->tag != 6)goto _LL1F8;else{
_tmp462=_tmp461->f1;_tmp463=_tmp461->f2;if(_tmp463 != Cyc_Absyn_Short_sz)goto
_LL1F8;}}_LL1F7: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp462,0),0);goto
_LL1F1;_LL1F8: {struct Cyc_Absyn_EnumType_struct*_tmp464=(struct Cyc_Absyn_EnumType_struct*)
_tmp45C;if(_tmp464->tag != 14)goto _LL1FA;}_LL1F9: goto _LL1FB;_LL1FA: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp465=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp45C;if(_tmp465->tag != 15)goto
_LL1FC;}_LL1FB: _tmp467=Cyc_Absyn_Unsigned;goto _LL1FD;_LL1FC: {struct Cyc_Absyn_IntType_struct*
_tmp466=(struct Cyc_Absyn_IntType_struct*)_tmp45C;if(_tmp466->tag != 6)goto _LL1FE;
else{_tmp467=_tmp466->f1;_tmp468=_tmp466->f2;if(_tmp468 != Cyc_Absyn_Int_sz)goto
_LL1FE;}}_LL1FD: _tmp46A=_tmp467;goto _LL1FF;_LL1FE: {struct Cyc_Absyn_IntType_struct*
_tmp469=(struct Cyc_Absyn_IntType_struct*)_tmp45C;if(_tmp469->tag != 6)goto _LL200;
else{_tmp46A=_tmp469->f1;_tmp46B=_tmp469->f2;if(_tmp46B != Cyc_Absyn_Long_sz)goto
_LL200;}}_LL1FF: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp46A,0),0);goto _LL1F1;
_LL200: {struct Cyc_Absyn_IntType_struct*_tmp46C=(struct Cyc_Absyn_IntType_struct*)
_tmp45C;if(_tmp46C->tag != 6)goto _LL202;else{_tmp46D=_tmp46C->f1;_tmp46E=_tmp46C->f2;
if(_tmp46E != Cyc_Absyn_LongLong_sz)goto _LL202;}}_LL201: res=Cyc_Absyn_const_exp(
Cyc_Absyn_LongLong_c(_tmp46D,(long long)0),0);goto _LL1F1;_LL202: {struct Cyc_Absyn_FloatType_struct*
_tmp46F=(struct Cyc_Absyn_FloatType_struct*)_tmp45C;if(_tmp46F->tag != 7)goto
_LL204;}_LL203: goto _LL205;_LL204: {struct Cyc_Absyn_DoubleType_struct*_tmp470=(
struct Cyc_Absyn_DoubleType_struct*)_tmp45C;if(_tmp470->tag != 8)goto _LL206;}
_LL205:{const char*_tmpBF6;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBF6="0.0",
_tag_dyneither(_tmpBF6,sizeof(char),4)))),0);}goto _LL1F1;_LL206:;_LL207: {const
char*_tmpBFA;void*_tmpBF9[1];struct Cyc_String_pa_struct _tmpBF8;(_tmpBF8.tag=0,((
_tmpBF8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmpBF9[0]=& _tmpBF8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBFA="found non-zero type %s in generate_zero",_tag_dyneither(_tmpBFA,sizeof(
char),40))),_tag_dyneither(_tmpBF9,sizeof(void*),1)))))));}_LL1F1:;}{struct Cyc_Core_Opt*
_tmpBFB;res->topt=((_tmpBFB=_cycalloc(sizeof(*_tmpBFB)),((_tmpBFB->v=t,_tmpBFB))));}
return res;}static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dyneither,void*elt_type){void*fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);void*
_tmp476=Cyc_Toc_typ_to_c(elt_type);void*_tmp477=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp478=Cyc_Absyn_cstar_typ(_tmp476,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmpBFC;struct Cyc_Core_Opt*_tmp479=(_tmpBFC=_cycalloc(sizeof(*_tmpBFC)),((
_tmpBFC->v=_tmp478,_tmpBFC)));struct Cyc_Absyn_Exp*xinit;{void*_tmp47A=e1->r;
struct Cyc_Absyn_Exp*_tmp47C;struct Cyc_Absyn_Exp*_tmp47E;struct Cyc_Absyn_Exp*
_tmp47F;_LL209: {struct Cyc_Absyn_Deref_e_struct*_tmp47B=(struct Cyc_Absyn_Deref_e_struct*)
_tmp47A;if(_tmp47B->tag != 21)goto _LL20B;else{_tmp47C=_tmp47B->f1;}}_LL20A: if(!
is_dyneither){_tmp47C=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp47C,0,Cyc_Absyn_Other_coercion,
0);{struct Cyc_Core_Opt*_tmpBFD;_tmp47C->topt=((_tmpBFD=_cycalloc(sizeof(*_tmpBFD)),((
_tmpBFD->v=fat_ptr_type,_tmpBFD))));};}Cyc_Toc_exp_to_c(nv,_tmp47C);xinit=
_tmp47C;goto _LL208;_LL20B: {struct Cyc_Absyn_Subscript_e_struct*_tmp47D=(struct
Cyc_Absyn_Subscript_e_struct*)_tmp47A;if(_tmp47D->tag != 24)goto _LL20D;else{
_tmp47E=_tmp47D->f1;_tmp47F=_tmp47D->f2;}}_LL20C: if(!is_dyneither){_tmp47E=Cyc_Absyn_cast_exp(
fat_ptr_type,_tmp47E,0,Cyc_Absyn_Other_coercion,0);{struct Cyc_Core_Opt*_tmpBFE;
_tmp47E->topt=((_tmpBFE=_cycalloc(sizeof(*_tmpBFE)),((_tmpBFE->v=fat_ptr_type,
_tmpBFE))));};}Cyc_Toc_exp_to_c(nv,_tmp47E);Cyc_Toc_exp_to_c(nv,_tmp47F);{struct
Cyc_Absyn_Exp*_tmpBFF[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,((
_tmpBFF[2]=_tmp47F,((_tmpBFF[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmpBFF[0]=_tmp47E,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpBFF,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL208;_LL20D:;_LL20E: {const char*_tmpC02;void*_tmpC01;(_tmpC01=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC02="found bad lhs for zero-terminated pointer assignment",
_tag_dyneither(_tmpC02,sizeof(char),53))),_tag_dyneither(_tmpC01,sizeof(void*),0)));}
_LL208:;}{struct _tuple0*_tmp485=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp487;
struct _RegionHandle*_tmp488;struct Cyc_Toc_Env*_tmp486=nv;_tmp487=*_tmp486;
_tmp488=_tmp487.rgn;{struct Cyc_Toc_Env*_tmp489=Cyc_Toc_add_varmap(_tmp488,nv,
_tmp485,Cyc_Absyn_var_exp(_tmp485,0));struct Cyc_Absyn_Vardecl*_tmpC03;struct Cyc_Absyn_Vardecl*
_tmp48A=(_tmpC03=_cycalloc(sizeof(*_tmpC03)),((_tmpC03->sc=Cyc_Absyn_Public,((
_tmpC03->name=_tmp485,((_tmpC03->tq=Cyc_Toc_mt_tq,((_tmpC03->type=_tmp477,((
_tmpC03->initializer=(struct Cyc_Absyn_Exp*)xinit,((_tmpC03->rgn=0,((_tmpC03->attributes=
0,((_tmpC03->escapes=0,_tmpC03)))))))))))))))));struct Cyc_Absyn_Local_b_struct
_tmpC06;struct Cyc_Absyn_Local_b_struct*_tmpC05;struct Cyc_Absyn_Local_b_struct*
_tmp48B=(_tmpC05=_cycalloc(sizeof(*_tmpC05)),((_tmpC05[0]=((_tmpC06.tag=4,((
_tmpC06.f1=_tmp48A,_tmpC06)))),_tmpC05)));struct Cyc_Absyn_Exp*_tmp48C=Cyc_Absyn_varb_exp(
_tmp485,(void*)_tmp48B,0);{struct Cyc_Core_Opt*_tmpC07;_tmp48C->topt=((_tmpC07=
_cycalloc(sizeof(*_tmpC07)),((_tmpC07->v=fat_ptr_type,_tmpC07))));}{struct Cyc_Absyn_Exp*
_tmp48E=Cyc_Absyn_deref_exp(_tmp48C,0);{struct Cyc_Core_Opt*_tmpC08;_tmp48E->topt=((
_tmpC08=_cycalloc(sizeof(*_tmpC08)),((_tmpC08->v=elt_type,_tmpC08))));}Cyc_Toc_exp_to_c(
_tmp489,_tmp48E);{struct _tuple0*_tmp490=Cyc_Toc_temp_var();_tmp489=Cyc_Toc_add_varmap(
_tmp488,_tmp489,_tmp490,Cyc_Absyn_var_exp(_tmp490,0));{struct Cyc_Absyn_Vardecl*
_tmpC09;struct Cyc_Absyn_Vardecl*_tmp491=(_tmpC09=_cycalloc(sizeof(*_tmpC09)),((
_tmpC09->sc=Cyc_Absyn_Public,((_tmpC09->name=_tmp490,((_tmpC09->tq=Cyc_Toc_mt_tq,((
_tmpC09->type=_tmp476,((_tmpC09->initializer=(struct Cyc_Absyn_Exp*)_tmp48E,((
_tmpC09->rgn=0,((_tmpC09->attributes=0,((_tmpC09->escapes=0,_tmpC09)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpC0C;struct Cyc_Absyn_Local_b_struct*_tmpC0B;
struct Cyc_Absyn_Local_b_struct*_tmp492=(_tmpC0B=_cycalloc(sizeof(*_tmpC0B)),((
_tmpC0B[0]=((_tmpC0C.tag=4,((_tmpC0C.f1=_tmp491,_tmpC0C)))),_tmpC0B)));struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp493=Cyc_Absyn_varb_exp(_tmp490,(
void*)_tmp492,0);_tmp493->topt=_tmp48E->topt;z_init=Cyc_Absyn_prim2_exp((enum 
Cyc_Absyn_Primop)popt->v,_tmp493,e2,0);z_init->topt=_tmp493->topt;}Cyc_Toc_exp_to_c(
_tmp489,z_init);{struct _tuple0*_tmp494=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmpC0D;struct Cyc_Absyn_Vardecl*_tmp495=(_tmpC0D=_cycalloc(sizeof(*_tmpC0D)),((
_tmpC0D->sc=Cyc_Absyn_Public,((_tmpC0D->name=_tmp494,((_tmpC0D->tq=Cyc_Toc_mt_tq,((
_tmpC0D->type=_tmp476,((_tmpC0D->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmpC0D->rgn=0,((_tmpC0D->attributes=0,((_tmpC0D->escapes=0,_tmpC0D)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpC10;struct Cyc_Absyn_Local_b_struct*_tmpC0F;
struct Cyc_Absyn_Local_b_struct*_tmp496=(_tmpC0F=_cycalloc(sizeof(*_tmpC0F)),((
_tmpC0F[0]=((_tmpC10.tag=4,((_tmpC10.f1=_tmp495,_tmpC10)))),_tmpC0F)));_tmp489=
Cyc_Toc_add_varmap(_tmp488,_tmp489,_tmp494,Cyc_Absyn_var_exp(_tmp494,0));{struct
Cyc_Absyn_Exp*_tmp497=Cyc_Absyn_varb_exp(_tmp490,(void*)_tmp492,0);_tmp497->topt=
_tmp48E->topt;{struct Cyc_Absyn_Exp*_tmp498=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp499=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp497,_tmp498,0);{
struct Cyc_Core_Opt*_tmpC11;_tmp499->topt=((_tmpC11=_cycalloc(sizeof(*_tmpC11)),((
_tmpC11->v=Cyc_Absyn_sint_typ,_tmpC11))));}Cyc_Toc_exp_to_c(_tmp489,_tmp499);{
struct Cyc_Absyn_Exp*_tmp49B=Cyc_Absyn_varb_exp(_tmp494,(void*)_tmp496,0);_tmp49B->topt=
_tmp48E->topt;{struct Cyc_Absyn_Exp*_tmp49C=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp49D=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp49B,_tmp49C,0);{
struct Cyc_Core_Opt*_tmpC12;_tmp49D->topt=((_tmpC12=_cycalloc(sizeof(*_tmpC12)),((
_tmpC12->v=Cyc_Absyn_sint_typ,_tmpC12))));}Cyc_Toc_exp_to_c(_tmp489,_tmp49D);{
struct Cyc_Absyn_Exp*_tmpC13[2];struct Cyc_List_List*_tmp49F=(_tmpC13[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0),((_tmpC13[0]=Cyc_Absyn_varb_exp(_tmp485,(void*)
_tmp48B,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC13,sizeof(struct Cyc_Absyn_Exp*),2)))));struct Cyc_Absyn_Exp*
_tmp4A0=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;xsize=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp49F,0),
_tmp4A0,0);{struct Cyc_Absyn_Exp*_tmp4A1=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp499,_tmp49D,0),0);struct Cyc_Absyn_Stmt*_tmp4A2=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp4A3=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp485,(void*)_tmp48B,0),Cyc_Toc_curr_sp,0);_tmp4A3=Cyc_Toc_cast_it(
_tmp478,_tmp4A3);{struct Cyc_Absyn_Exp*_tmp4A4=Cyc_Absyn_deref_exp(_tmp4A3,0);
struct Cyc_Absyn_Exp*_tmp4A5=Cyc_Absyn_assign_exp(_tmp4A4,Cyc_Absyn_var_exp(
_tmp494,0),0);struct Cyc_Absyn_Stmt*_tmp4A6=Cyc_Absyn_exp_stmt(_tmp4A5,0);_tmp4A6=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp4A1,_tmp4A2,Cyc_Absyn_skip_stmt(
0),0),_tmp4A6,0);{struct Cyc_Absyn_Var_d_struct*_tmpC19;struct Cyc_Absyn_Var_d_struct
_tmpC18;struct Cyc_Absyn_Decl*_tmpC17;_tmp4A6=Cyc_Absyn_decl_stmt(((_tmpC17=
_cycalloc(sizeof(*_tmpC17)),((_tmpC17->r=(void*)((_tmpC19=_cycalloc(sizeof(*
_tmpC19)),((_tmpC19[0]=((_tmpC18.tag=0,((_tmpC18.f1=_tmp495,_tmpC18)))),_tmpC19)))),((
_tmpC17->loc=0,_tmpC17)))))),_tmp4A6,0);}{struct Cyc_Absyn_Var_d_struct*_tmpC1F;
struct Cyc_Absyn_Var_d_struct _tmpC1E;struct Cyc_Absyn_Decl*_tmpC1D;_tmp4A6=Cyc_Absyn_decl_stmt(((
_tmpC1D=_cycalloc(sizeof(*_tmpC1D)),((_tmpC1D->r=(void*)((_tmpC1F=_cycalloc(
sizeof(*_tmpC1F)),((_tmpC1F[0]=((_tmpC1E.tag=0,((_tmpC1E.f1=_tmp491,_tmpC1E)))),
_tmpC1F)))),((_tmpC1D->loc=0,_tmpC1D)))))),_tmp4A6,0);}{struct Cyc_Absyn_Var_d_struct*
_tmpC25;struct Cyc_Absyn_Var_d_struct _tmpC24;struct Cyc_Absyn_Decl*_tmpC23;_tmp4A6=
Cyc_Absyn_decl_stmt(((_tmpC23=_cycalloc(sizeof(*_tmpC23)),((_tmpC23->r=(void*)((
_tmpC25=_cycalloc(sizeof(*_tmpC25)),((_tmpC25[0]=((_tmpC24.tag=0,((_tmpC24.f1=
_tmp48A,_tmpC24)))),_tmpC25)))),((_tmpC23->loc=0,_tmpC23)))))),_tmp4A6,0);}e->r=
Cyc_Toc_stmt_exp_r(_tmp4A6);};};};};};};};};};};};};};}static void*Cyc_Toc_check_tagged_union(
struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int
in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,
struct Cyc_Position_Segment*)){struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp4BB=Cyc_Tcutil_compress(
aggrtype);struct Cyc_Absyn_AggrInfo _tmp4BD;union Cyc_Absyn_AggrInfoU _tmp4BE;_LL210: {
struct Cyc_Absyn_AggrType_struct*_tmp4BC=(struct Cyc_Absyn_AggrType_struct*)
_tmp4BB;if(_tmp4BC->tag != 12)goto _LL212;else{_tmp4BD=_tmp4BC->f1;_tmp4BE=_tmp4BD.aggr_info;}}
_LL211: ad=Cyc_Absyn_get_known_aggrdecl(_tmp4BE);goto _LL20F;_LL212:;_LL213: {
struct Cyc_String_pa_struct _tmpC2D;void*_tmpC2C[1];const char*_tmpC2B;void*_tmpC2A;(
_tmpC2A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((
struct _dyneither_ptr)((_tmpC2D.tag=0,((_tmpC2D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpC2C[0]=& _tmpC2D,Cyc_aprintf(((
_tmpC2B="expecting union but found %s in check_tagged_union",_tag_dyneither(
_tmpC2B,sizeof(char),51))),_tag_dyneither(_tmpC2C,sizeof(void*),1)))))))),
_tag_dyneither(_tmpC2A,sizeof(void*),0)));}_LL20F:;}{struct _tuple0*_tmp4C3=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4C4=Cyc_Absyn_var_exp(_tmp4C3,0);struct Cyc_Absyn_Exp*
_tmp4C5=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);if(in_lhs){
struct Cyc_Absyn_Exp*_tmp4C6=Cyc_Absyn_aggrarrow_exp(_tmp4C4,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4C7=Cyc_Absyn_neq_exp(_tmp4C6,_tmp4C5,0);struct Cyc_Absyn_Exp*
_tmp4C8=Cyc_Absyn_aggrarrow_exp(_tmp4C4,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Stmt*
_tmp4C9=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp4C8,0),0);struct Cyc_Absyn_Stmt*
_tmp4CA=Cyc_Absyn_ifthenelse_stmt(_tmp4C7,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);void*_tmp4CB=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp4CC=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);struct Cyc_Absyn_Stmt*_tmp4CD=
Cyc_Absyn_declare_stmt(_tmp4C3,_tmp4CB,(struct Cyc_Absyn_Exp*)_tmp4CC,Cyc_Absyn_seq_stmt(
_tmp4CA,_tmp4C9,0),0);return Cyc_Toc_stmt_exp_r(_tmp4CD);}else{struct Cyc_Absyn_Exp*
_tmp4CE=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp4C4,f,0),Cyc_Toc_tag_sp,0);struct
Cyc_Absyn_Exp*_tmp4CF=Cyc_Absyn_neq_exp(_tmp4CE,_tmp4C5,0);struct Cyc_Absyn_Exp*
_tmp4D0=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp4C4,f,0),Cyc_Toc_val_sp,0);struct
Cyc_Absyn_Stmt*_tmp4D1=Cyc_Absyn_exp_stmt(_tmp4D0,0);struct Cyc_Absyn_Stmt*
_tmp4D2=Cyc_Absyn_ifthenelse_stmt(_tmp4CF,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp4D3=Cyc_Absyn_declare_stmt(_tmp4C3,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp4D2,_tmp4D1,0),0);return Cyc_Toc_stmt_exp_r(
_tmp4D3);}};}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read){void*_tmp4D4=e->r;struct Cyc_Absyn_Exp*
_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D8;struct _dyneither_ptr*_tmp4D9;int _tmp4DA;int*
_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DD;struct _dyneither_ptr*_tmp4DE;int _tmp4DF;int*
_tmp4E0;_LL215: {struct Cyc_Absyn_Cast_e_struct*_tmp4D5=(struct Cyc_Absyn_Cast_e_struct*)
_tmp4D4;if(_tmp4D5->tag != 15)goto _LL217;else{_tmp4D6=_tmp4D5->f2;}}_LL216: {
const char*_tmpC30;void*_tmpC2F;(_tmpC2F=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC30="cast on lhs!",
_tag_dyneither(_tmpC30,sizeof(char),13))),_tag_dyneither(_tmpC2F,sizeof(void*),0)));}
_LL217: {struct Cyc_Absyn_AggrMember_e_struct*_tmp4D7=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp4D4;if(_tmp4D7->tag != 22)goto _LL219;else{_tmp4D8=_tmp4D7->f1;_tmp4D9=_tmp4D7->f2;
_tmp4DA=_tmp4D7->f4;_tmp4DB=(int*)& _tmp4D7->f4;}}_LL218: {void*_tmp4E3=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4D8->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp4E5;union Cyc_Absyn_AggrInfoU _tmp4E6;_LL21E: {struct Cyc_Absyn_AggrType_struct*
_tmp4E4=(struct Cyc_Absyn_AggrType_struct*)_tmp4E3;if(_tmp4E4->tag != 12)goto
_LL220;else{_tmp4E5=_tmp4E4->f1;_tmp4E6=_tmp4E5.aggr_info;}}_LL21F: {struct Cyc_Absyn_Aggrdecl*
_tmp4E7=Cyc_Absyn_get_known_aggrdecl(_tmp4E6);*f_tag=Cyc_Toc_get_member_offset(
_tmp4E7,_tmp4D9);{const char*_tmpC35;void*_tmpC34[2];struct Cyc_String_pa_struct
_tmpC33;struct Cyc_String_pa_struct _tmpC32;struct _dyneither_ptr str=(struct
_dyneither_ptr)((_tmpC32.tag=0,((_tmpC32.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp4D9),((_tmpC33.tag=0,((_tmpC33.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp4E7->name).f2),((_tmpC34[0]=& _tmpC33,((_tmpC34[1]=&
_tmpC32,Cyc_aprintf(((_tmpC35="_union_%s_%s",_tag_dyneither(_tmpC35,sizeof(char),
13))),_tag_dyneither(_tmpC34,sizeof(void*),2))))))))))))));{struct _dyneither_ptr*
_tmpC36;*tagged_member_type=Cyc_Absyn_strct(((_tmpC36=_cycalloc(sizeof(*_tmpC36)),((
_tmpC36[0]=str,_tmpC36)))));}if(clear_read)*_tmp4DB=0;return((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4E7->impl))->tagged;};}_LL220:;_LL221: return 0;_LL21D:;}_LL219: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp4DC=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp4D4;if(_tmp4DC->tag != 23)goto _LL21B;else{_tmp4DD=_tmp4DC->f1;_tmp4DE=_tmp4DC->f2;
_tmp4DF=_tmp4DC->f4;_tmp4E0=(int*)& _tmp4DC->f4;}}_LL21A: {void*_tmp4ED=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4DD->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp4EF;void*_tmp4F0;_LL223: {struct Cyc_Absyn_PointerType_struct*_tmp4EE=(struct
Cyc_Absyn_PointerType_struct*)_tmp4ED;if(_tmp4EE->tag != 5)goto _LL225;else{
_tmp4EF=_tmp4EE->f1;_tmp4F0=_tmp4EF.elt_typ;}}_LL224: {void*_tmp4F1=Cyc_Tcutil_compress(
_tmp4F0);struct Cyc_Absyn_AggrInfo _tmp4F3;union Cyc_Absyn_AggrInfoU _tmp4F4;_LL228: {
struct Cyc_Absyn_AggrType_struct*_tmp4F2=(struct Cyc_Absyn_AggrType_struct*)
_tmp4F1;if(_tmp4F2->tag != 12)goto _LL22A;else{_tmp4F3=_tmp4F2->f1;_tmp4F4=_tmp4F3.aggr_info;}}
_LL229: {struct Cyc_Absyn_Aggrdecl*_tmp4F5=Cyc_Absyn_get_known_aggrdecl(_tmp4F4);*
f_tag=Cyc_Toc_get_member_offset(_tmp4F5,_tmp4DE);{const char*_tmpC3B;void*_tmpC3A[
2];struct Cyc_String_pa_struct _tmpC39;struct Cyc_String_pa_struct _tmpC38;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpC38.tag=0,((_tmpC38.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp4DE),((_tmpC39.tag=0,((_tmpC39.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4F5->name).f2),((_tmpC3A[0]=&
_tmpC39,((_tmpC3A[1]=& _tmpC38,Cyc_aprintf(((_tmpC3B="_union_%s_%s",
_tag_dyneither(_tmpC3B,sizeof(char),13))),_tag_dyneither(_tmpC3A,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpC3C;*tagged_member_type=Cyc_Absyn_strct(((_tmpC3C=
_cycalloc(sizeof(*_tmpC3C)),((_tmpC3C[0]=str,_tmpC3C)))));}if(clear_read)*
_tmp4E0=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4F5->impl))->tagged;};}
_LL22A:;_LL22B: return 0;_LL227:;}_LL225:;_LL226: return 0;_LL222:;}_LL21B:;_LL21C:
return 0;_LL214:;}void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*
e,int tag,void*mem_type){struct _tuple0*_tmp4FB=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
temp_exp=Cyc_Absyn_var_exp(_tmp4FB,0);struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(
tag,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,
0),0);struct Cyc_Absyn_Exp*_tmp4FC=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_ifthenelse_stmt(_tmp4FC,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4FB,Cyc_Absyn_cstar_typ(
mem_type,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(
e)),Cyc_Absyn_seq_stmt(s2,s3,0),0);e->r=Cyc_Toc_stmt_exp_r(s1);}static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type){struct _tuple0*_tmp4FD=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4FD,0);struct Cyc_Absyn_Exp*
temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*
temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 
0)s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*
_tmp4FE=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp4FE,
Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=
Cyc_Absyn_declare_stmt(_tmp4FD,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(e1),Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple20{struct _tuple0*f1;void*f2;struct
Cyc_Absyn_Exp*f3;};struct _tuple21{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*
f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void _tmpC8F(unsigned int*_tmpC8E,unsigned int*_tmpC8D,struct _tuple0***
_tmpC8B){for(*_tmpC8E=0;*_tmpC8E < *_tmpC8D;(*_tmpC8E)++){(*_tmpC8B)[*_tmpC8E]=
Cyc_Toc_temp_var();}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){void*_tmp4FF=e->r;if(e->topt == 0){const char*_tmpC40;void*_tmpC3F[1];struct Cyc_String_pa_struct
_tmpC3E;(_tmpC3E.tag=0,((_tmpC3E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpC3F[0]=& _tmpC3E,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC40="exp_to_c: no type for %s",
_tag_dyneither(_tmpC40,sizeof(char),25))),_tag_dyneither(_tmpC3F,sizeof(void*),1)))))));}{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp503=
_tmp4FF;union Cyc_Absyn_Cnst _tmp505;int _tmp506;struct _tuple0*_tmp509;void*_tmp50A;
struct _tuple0*_tmp50C;enum Cyc_Absyn_Primop _tmp50E;struct Cyc_List_List*_tmp50F;
struct Cyc_Absyn_Exp*_tmp511;enum Cyc_Absyn_Incrementor _tmp512;struct Cyc_Absyn_Exp*
_tmp514;struct Cyc_Core_Opt*_tmp515;struct Cyc_Absyn_Exp*_tmp516;struct Cyc_Absyn_Exp*
_tmp518;struct Cyc_Absyn_Exp*_tmp519;struct Cyc_Absyn_Exp*_tmp51A;struct Cyc_Absyn_Exp*
_tmp51C;struct Cyc_Absyn_Exp*_tmp51D;struct Cyc_Absyn_Exp*_tmp51F;struct Cyc_Absyn_Exp*
_tmp520;struct Cyc_Absyn_Exp*_tmp522;struct Cyc_Absyn_Exp*_tmp523;struct Cyc_Absyn_Exp*
_tmp525;struct Cyc_List_List*_tmp526;struct Cyc_Absyn_Exp*_tmp528;struct Cyc_List_List*
_tmp529;struct Cyc_Absyn_VarargCallInfo*_tmp52A;struct Cyc_Absyn_Exp*_tmp52C;
struct Cyc_List_List*_tmp52D;struct Cyc_Absyn_VarargCallInfo*_tmp52E;struct Cyc_Absyn_VarargCallInfo
_tmp52F;int _tmp530;struct Cyc_List_List*_tmp531;struct Cyc_Absyn_VarargInfo*
_tmp532;struct Cyc_Absyn_Exp*_tmp534;struct Cyc_Absyn_Exp*_tmp536;struct Cyc_Absyn_Exp*
_tmp538;struct Cyc_List_List*_tmp539;void*_tmp53B;void**_tmp53C;struct Cyc_Absyn_Exp*
_tmp53D;int _tmp53E;enum Cyc_Absyn_Coercion _tmp53F;struct Cyc_Absyn_Exp*_tmp541;
struct Cyc_Absyn_Exp*_tmp543;struct Cyc_Absyn_Exp*_tmp544;struct Cyc_Absyn_Exp*
_tmp546;void*_tmp548;void*_tmp54A;void*_tmp54B;struct _dyneither_ptr*_tmp54D;void*
_tmp54F;void*_tmp550;unsigned int _tmp552;struct Cyc_Absyn_Exp*_tmp554;struct Cyc_Absyn_Exp*
_tmp556;struct _dyneither_ptr*_tmp557;int _tmp558;int _tmp559;struct Cyc_Absyn_Exp*
_tmp55B;struct _dyneither_ptr*_tmp55C;int _tmp55D;int _tmp55E;struct Cyc_Absyn_Exp*
_tmp560;struct Cyc_Absyn_Exp*_tmp561;struct Cyc_List_List*_tmp563;struct Cyc_List_List*
_tmp565;struct Cyc_Absyn_Vardecl*_tmp567;struct Cyc_Absyn_Exp*_tmp568;struct Cyc_Absyn_Exp*
_tmp569;int _tmp56A;struct _tuple0*_tmp56C;struct Cyc_List_List*_tmp56D;struct Cyc_List_List*
_tmp56E;struct Cyc_Absyn_Aggrdecl*_tmp56F;void*_tmp571;struct Cyc_List_List*
_tmp572;struct Cyc_List_List*_tmp574;struct Cyc_Absyn_Datatypedecl*_tmp575;struct
Cyc_Absyn_Datatypefield*_tmp576;struct Cyc_Absyn_MallocInfo _tmp57A;int _tmp57B;
struct Cyc_Absyn_Exp*_tmp57C;void**_tmp57D;struct Cyc_Absyn_Exp*_tmp57E;int _tmp57F;
struct Cyc_Absyn_Exp*_tmp581;struct Cyc_Absyn_Exp*_tmp582;struct Cyc_Absyn_Exp*
_tmp584;struct _dyneither_ptr*_tmp585;struct Cyc_Absyn_Stmt*_tmp587;_LL22D: {
struct Cyc_Absyn_Const_e_struct*_tmp504=(struct Cyc_Absyn_Const_e_struct*)_tmp503;
if(_tmp504->tag != 0)goto _LL22F;else{_tmp505=_tmp504->f1;if((_tmp505.Null_c).tag
!= 1)goto _LL22F;_tmp506=(int)(_tmp505.Null_c).val;}}_LL22E: {struct Cyc_Absyn_Exp*
_tmp58C=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp58C,
_tmp58C))->r;else{struct Cyc_Absyn_Exp*_tmpC41[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((
_tmpC41[2]=_tmp58C,((_tmpC41[1]=_tmp58C,((_tmpC41[0]=_tmp58C,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC41,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL22C;}_LL22F: {struct Cyc_Absyn_Const_e_struct*
_tmp507=(struct Cyc_Absyn_Const_e_struct*)_tmp503;if(_tmp507->tag != 0)goto _LL231;}
_LL230: goto _LL22C;_LL231: {struct Cyc_Absyn_Var_e_struct*_tmp508=(struct Cyc_Absyn_Var_e_struct*)
_tmp503;if(_tmp508->tag != 1)goto _LL233;else{_tmp509=_tmp508->f1;_tmp50A=(void*)
_tmp508->f2;}}_LL232:{struct _handler_cons _tmp58E;_push_handler(& _tmp58E);{int
_tmp590=0;if(setjmp(_tmp58E.handler))_tmp590=1;if(!_tmp590){e->r=(Cyc_Toc_lookup_varmap(
nv,_tmp509))->r;;_pop_handler();}else{void*_tmp58F=(void*)_exn_thrown;void*
_tmp592=_tmp58F;_LL286: {struct Cyc_Dict_Absent_struct*_tmp593=(struct Cyc_Dict_Absent_struct*)
_tmp592;if(_tmp593->tag != Cyc_Dict_Absent)goto _LL288;}_LL287: {const char*_tmpC45;
void*_tmpC44[1];struct Cyc_String_pa_struct _tmpC43;(_tmpC43.tag=0,((_tmpC43.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp509)),((
_tmpC44[0]=& _tmpC43,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC45="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpC45,sizeof(char),
32))),_tag_dyneither(_tmpC44,sizeof(void*),1)))))));}_LL288:;_LL289:(void)_throw(
_tmp592);_LL285:;}};}goto _LL22C;_LL233: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp50B=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp503;if(_tmp50B->tag != 2)goto
_LL235;else{_tmp50C=_tmp50B->f1;}}_LL234: {const char*_tmpC48;void*_tmpC47;(
_tmpC47=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC48="unknownid",_tag_dyneither(_tmpC48,sizeof(char),10))),_tag_dyneither(
_tmpC47,sizeof(void*),0)));}_LL235: {struct Cyc_Absyn_Primop_e_struct*_tmp50D=(
struct Cyc_Absyn_Primop_e_struct*)_tmp503;if(_tmp50D->tag != 3)goto _LL237;else{
_tmp50E=_tmp50D->f1;_tmp50F=_tmp50D->f2;}}_LL236: {struct Cyc_List_List*_tmp599=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp50F);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp50F);switch(_tmp50E){case Cyc_Absyn_Numelts: _LL28A: {
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp50F))->hd;{void*_tmp59A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo _tmp59C;struct Cyc_Absyn_Exp*
_tmp59D;struct Cyc_Absyn_PtrInfo _tmp59F;void*_tmp5A0;struct Cyc_Absyn_PtrAtts
_tmp5A1;union Cyc_Absyn_Constraint*_tmp5A2;union Cyc_Absyn_Constraint*_tmp5A3;
union Cyc_Absyn_Constraint*_tmp5A4;_LL28D: {struct Cyc_Absyn_ArrayType_struct*
_tmp59B=(struct Cyc_Absyn_ArrayType_struct*)_tmp59A;if(_tmp59B->tag != 9)goto
_LL28F;else{_tmp59C=_tmp59B->f1;_tmp59D=_tmp59C.num_elts;}}_LL28E: if(!Cyc_Evexp_c_can_eval((
struct Cyc_Absyn_Exp*)_check_null(_tmp59D))){const char*_tmpC4B;void*_tmpC4A;(
_tmpC4A=0,Cyc_Tcutil_terr(e->loc,((_tmpC4B="can't calculate numelts",
_tag_dyneither(_tmpC4B,sizeof(char),24))),_tag_dyneither(_tmpC4A,sizeof(void*),0)));}
e->r=_tmp59D->r;goto _LL28C;_LL28F: {struct Cyc_Absyn_PointerType_struct*_tmp59E=(
struct Cyc_Absyn_PointerType_struct*)_tmp59A;if(_tmp59E->tag != 5)goto _LL291;else{
_tmp59F=_tmp59E->f1;_tmp5A0=_tmp59F.elt_typ;_tmp5A1=_tmp59F.ptr_atts;_tmp5A2=
_tmp5A1.nullable;_tmp5A3=_tmp5A1.bounds;_tmp5A4=_tmp5A1.zero_term;}}_LL290:{void*
_tmp5A7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp5A3);struct Cyc_Absyn_Exp*_tmp5AA;_LL294: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5A8=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5A7;if(_tmp5A8->tag != 0)goto
_LL296;}_LL295:{struct Cyc_Absyn_Exp*_tmpC4C[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,((
_tmpC4C[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A0),0),((_tmpC4C[0]=(
struct Cyc_Absyn_Exp*)_tmp50F->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC4C,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL293;_LL296: {struct Cyc_Absyn_Upper_b_struct*_tmp5A9=(struct Cyc_Absyn_Upper_b_struct*)
_tmp5A7;if(_tmp5A9->tag != 1)goto _LL293;else{_tmp5AA=_tmp5A9->f1;}}_LL297: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5A4)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp50F->hd);struct Cyc_Absyn_Exp*_tmpC4D[2];e->r=Cyc_Toc_fncall_exp_r(
function_e,((_tmpC4D[1]=_tmp5AA,((_tmpC4D[0]=(struct Cyc_Absyn_Exp*)_tmp50F->hd,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC4D,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp5A2)){if(!Cyc_Evexp_c_can_eval(_tmp5AA)){const
char*_tmpC50;void*_tmpC4F;(_tmpC4F=0,Cyc_Tcutil_terr(e->loc,((_tmpC50="can't calculate numelts",
_tag_dyneither(_tmpC50,sizeof(char),24))),_tag_dyneither(_tmpC4F,sizeof(void*),0)));}
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp5AA,Cyc_Absyn_uint_exp(0,0));}else{e->r=
_tmp5AA->r;goto _LL293;}}goto _LL293;_LL293:;}goto _LL28C;_LL291:;_LL292: {const
char*_tmpC55;void*_tmpC54[2];struct Cyc_String_pa_struct _tmpC53;struct Cyc_String_pa_struct
_tmpC52;(_tmpC52.tag=0,((_tmpC52.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((
_tmpC53.tag=0,((_tmpC53.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmpC54[0]=& _tmpC53,((
_tmpC54[1]=& _tmpC52,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC55="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpC55,
sizeof(char),41))),_tag_dyneither(_tmpC54,sizeof(void*),2)))))))))))));}_LL28C:;}
break;}case Cyc_Absyn_Plus: _LL28B:{void*_tmp5B3=Cyc_Tcutil_compress((void*)((
struct Cyc_List_List*)_check_null(_tmp599))->hd);struct Cyc_Absyn_PtrInfo _tmp5B5;
void*_tmp5B6;struct Cyc_Absyn_PtrAtts _tmp5B7;union Cyc_Absyn_Constraint*_tmp5B8;
union Cyc_Absyn_Constraint*_tmp5B9;_LL29A: {struct Cyc_Absyn_PointerType_struct*
_tmp5B4=(struct Cyc_Absyn_PointerType_struct*)_tmp5B3;if(_tmp5B4->tag != 5)goto
_LL29C;else{_tmp5B5=_tmp5B4->f1;_tmp5B6=_tmp5B5.elt_typ;_tmp5B7=_tmp5B5.ptr_atts;
_tmp5B8=_tmp5B7.bounds;_tmp5B9=_tmp5B7.zero_term;}}_LL29B:{void*_tmp5BA=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp5B8);struct Cyc_Absyn_Exp*_tmp5BD;_LL29F: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5BB=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5BA;if(_tmp5BB->tag != 0)goto
_LL2A1;}_LL2A0: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp50F))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp50F->tl))->hd;{struct Cyc_Absyn_Exp*_tmpC56[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpC56[2]=e2,((_tmpC56[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5B6),0),((_tmpC56[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC56,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}goto _LL29E;}_LL2A1: {struct Cyc_Absyn_Upper_b_struct*_tmp5BC=(struct
Cyc_Absyn_Upper_b_struct*)_tmp5BA;if(_tmp5BC->tag != 1)goto _LL29E;else{_tmp5BD=
_tmp5BC->f1;}}_LL2A2: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp5B9)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp50F))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp50F->tl))->hd;struct Cyc_Absyn_Exp*_tmpC57[3];e->r=(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
e1),((_tmpC57[2]=e2,((_tmpC57[1]=_tmp5BD,((_tmpC57[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC57,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r;}goto _LL29E;_LL29E:;}goto _LL299;_LL29C:;_LL29D: goto _LL299;
_LL299:;}break;case Cyc_Absyn_Minus: _LL298: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp599))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp50F))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp50F->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp599->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{struct Cyc_Absyn_Exp*_tmpC58[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpC58[2]=Cyc_Absyn_prim1_exp(
Cyc_Absyn_Minus,e2,0),((_tmpC58[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpC58[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC58,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}break;}case Cyc_Absyn_Eq:
_LL2A3: goto _LL2A4;case Cyc_Absyn_Neq: _LL2A4: goto _LL2A5;case Cyc_Absyn_Gt: _LL2A5:
goto _LL2A6;case Cyc_Absyn_Gte: _LL2A6: goto _LL2A7;case Cyc_Absyn_Lt: _LL2A7: goto
_LL2A8;case Cyc_Absyn_Lte: _LL2A8: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp50F))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp50F->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp599))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp599->tl))->hd;void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& elt_typ))e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,
0));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp,0));break;}default: _LL2A9: break;}goto _LL22C;}_LL237: {
struct Cyc_Absyn_Increment_e_struct*_tmp510=(struct Cyc_Absyn_Increment_e_struct*)
_tmp503;if(_tmp510->tag != 5)goto _LL239;else{_tmp511=_tmp510->f1;_tmp512=_tmp510->f2;}}
_LL238: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp511->topt))->v;
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;const char*_tmpC59;struct _dyneither_ptr incr_str=(_tmpC59="increment",
_tag_dyneither(_tmpC59,sizeof(char),10));if(_tmp512 == Cyc_Absyn_PreDec  || 
_tmp512 == Cyc_Absyn_PostDec){const char*_tmpC5A;incr_str=((_tmpC5A="decrement",
_tag_dyneither(_tmpC5A,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp511,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpC5E;void*_tmpC5D[1];
struct Cyc_String_pa_struct _tmpC5C;(_tmpC5C.tag=0,((_tmpC5C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC5D[0]=& _tmpC5C,Cyc_Tcutil_terr(
e->loc,((_tmpC5E="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpC5E,sizeof(char),74))),_tag_dyneither(_tmpC5D,sizeof(void*),1)))))));}{
const char*_tmpC61;void*_tmpC60;(_tmpC60=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC61="in-place inc/dec on zero-term",
_tag_dyneither(_tmpC61,sizeof(char),30))),_tag_dyneither(_tmpC60,sizeof(void*),0)));};}{
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp511,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp5C7=Cyc_Absyn_signed_int_exp(
1,0);{struct Cyc_Core_Opt*_tmpC62;_tmp5C7->topt=((_tmpC62=_cycalloc(sizeof(*
_tmpC62)),((_tmpC62->v=Cyc_Absyn_sint_typ,_tmpC62))));}switch(_tmp512){case Cyc_Absyn_PreInc:
_LL2AB:{struct Cyc_Absyn_AssignOp_e_struct _tmpC68;struct Cyc_Core_Opt*_tmpC67;
struct Cyc_Absyn_AssignOp_e_struct*_tmpC66;e->r=(void*)((_tmpC66=_cycalloc(
sizeof(*_tmpC66)),((_tmpC66[0]=((_tmpC68.tag=4,((_tmpC68.f1=_tmp511,((_tmpC68.f2=((
_tmpC67=_cycalloc_atomic(sizeof(*_tmpC67)),((_tmpC67->v=(void*)Cyc_Absyn_Plus,
_tmpC67)))),((_tmpC68.f3=_tmp5C7,_tmpC68)))))))),_tmpC66))));}Cyc_Toc_exp_to_c(
nv,e);return;case Cyc_Absyn_PreDec: _LL2AC:{struct Cyc_Absyn_AssignOp_e_struct
_tmpC6E;struct Cyc_Core_Opt*_tmpC6D;struct Cyc_Absyn_AssignOp_e_struct*_tmpC6C;e->r=(
void*)((_tmpC6C=_cycalloc(sizeof(*_tmpC6C)),((_tmpC6C[0]=((_tmpC6E.tag=4,((
_tmpC6E.f1=_tmp511,((_tmpC6E.f2=((_tmpC6D=_cycalloc_atomic(sizeof(*_tmpC6D)),((
_tmpC6D->v=(void*)Cyc_Absyn_Minus,_tmpC6D)))),((_tmpC6E.f3=_tmp5C7,_tmpC6E)))))))),
_tmpC6C))));}Cyc_Toc_exp_to_c(nv,e);return;default: _LL2AD:{const char*_tmpC72;
void*_tmpC71[1];struct Cyc_String_pa_struct _tmpC70;(_tmpC70.tag=0,((_tmpC70.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC71[0]=& _tmpC70,Cyc_Tcutil_terr(
e->loc,((_tmpC72="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpC72,sizeof(char),59))),_tag_dyneither(_tmpC71,sizeof(void*),1)))))));}{
const char*_tmpC75;void*_tmpC74;(_tmpC74=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC75="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpC75,sizeof(char),34))),_tag_dyneither(_tmpC74,sizeof(void*),0)));};}}
Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp511);Cyc_Toc_set_lhs(nv,0);{void*
elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp512 == Cyc_Absyn_PostInc
 || _tmp512 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp512 == Cyc_Absyn_PreDec  || _tmp512 == Cyc_Absyn_PostDec)change=- 1;{struct
Cyc_Absyn_Exp*_tmpC76[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC76[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpC76[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpC76[0]=Cyc_Toc_push_address_exp(_tmp511),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC76,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Toc_functionSet*_tmp5D5=_tmp512 == Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:&
Cyc_Toc__zero_arr_inplace_plus_post_functionSet;struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(
_tmp5D5,_tmp511);struct Cyc_Absyn_Exp*_tmpC77[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((
_tmpC77[1]=Cyc_Absyn_signed_int_exp(1,0),((_tmpC77[0]=Cyc_Toc_push_address_exp(
_tmp511),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC77,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(elt_typ == (
void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp511)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(
_tmp511,0,Cyc_Toc_incr_lvalue,_tmp512);e->r=_tmp511->r;}}}goto _LL22C;};};}_LL239: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp513=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp503;if(_tmp513->tag != 4)goto _LL23B;else{_tmp514=_tmp513->f1;_tmp515=_tmp513->f2;
_tmp516=_tmp513->f3;}}_LL23A: {void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp514->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp516->topt))->v;int f_tag=0;void*tagged_member_struct_type=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Toc_is_tagged_union_project(_tmp514,& f_tag,&
tagged_member_struct_type,1)){Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp514);
Cyc_Toc_set_lhs(nv,0);Cyc_Toc_exp_to_c(nv,_tmp516);e->r=Cyc_Toc_tagged_union_assignop(
_tmp514,e1_old_typ,_tmp515,_tmp516,e2_old_typ,f_tag,tagged_member_struct_type);
return;}{void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(_tmp514,& ptr_type,&
is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp514,_tmp515,
_tmp516,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp514);Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp514);Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp516);{int done=0;if(_tmp515 != 0){void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;switch((enum Cyc_Absyn_Primop)_tmp515->v){case Cyc_Absyn_Plus: _LL2AF:
change=_tmp516;break;case Cyc_Absyn_Minus: _LL2B0: change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,
_tmp516,0);break;default: _LL2B1: {const char*_tmpC7A;void*_tmpC79;(_tmpC79=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC7A="bad t ? pointer arithmetic",_tag_dyneither(_tmpC7A,sizeof(char),27))),
_tag_dyneither(_tmpC79,sizeof(void*),0)));}}done=1;{struct Cyc_Absyn_Exp*_tmp5DA=
Cyc_Toc__dyneither_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*_tmpC7B[3];e->r=Cyc_Toc_fncall_exp_r(
_tmp5DA,((_tmpC7B[2]=change,((_tmpC7B[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpC7B[0]=Cyc_Toc_push_address_exp(_tmp514),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC7B,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))
switch((enum Cyc_Absyn_Primop)_tmp515->v){case Cyc_Absyn_Plus: _LL2B3: done=1;{
struct Cyc_Absyn_Exp*_tmpC7C[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp514),((_tmpC7C[1]=_tmp516,((
_tmpC7C[0]=_tmp514,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC7C,sizeof(struct Cyc_Absyn_Exp*),2)))))));}break;default:
_LL2B4: {const char*_tmpC7F;void*_tmpC7E;(_tmpC7E=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC7F="bad zero-terminated pointer arithmetic",
_tag_dyneither(_tmpC7F,sizeof(char),39))),_tag_dyneither(_tmpC7E,sizeof(void*),0)));}}}}
if(!done){if(e1_poly)_tmp516->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp516->r,0));if(!Cyc_Absyn_is_lvalue(_tmp514)){{struct _tuple19 _tmpC82;struct
_tuple19*_tmpC81;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple19*),struct _tuple19*f_env))
Cyc_Toc_lvalue_assign)(_tmp514,0,Cyc_Toc_assignop_lvalue,((_tmpC81=_cycalloc(
sizeof(struct _tuple19)* 1),((_tmpC81[0]=((_tmpC82.f1=_tmp515,((_tmpC82.f2=
_tmp516,_tmpC82)))),_tmpC81)))));}e->r=_tmp514->r;}}goto _LL22C;};};};}_LL23B: {
struct Cyc_Absyn_Conditional_e_struct*_tmp517=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp503;if(_tmp517->tag != 6)goto _LL23D;else{_tmp518=_tmp517->f1;_tmp519=_tmp517->f2;
_tmp51A=_tmp517->f3;}}_LL23C: Cyc_Toc_exp_to_c(nv,_tmp518);Cyc_Toc_exp_to_c(nv,
_tmp519);Cyc_Toc_exp_to_c(nv,_tmp51A);goto _LL22C;_LL23D: {struct Cyc_Absyn_And_e_struct*
_tmp51B=(struct Cyc_Absyn_And_e_struct*)_tmp503;if(_tmp51B->tag != 7)goto _LL23F;
else{_tmp51C=_tmp51B->f1;_tmp51D=_tmp51B->f2;}}_LL23E: Cyc_Toc_exp_to_c(nv,
_tmp51C);Cyc_Toc_exp_to_c(nv,_tmp51D);goto _LL22C;_LL23F: {struct Cyc_Absyn_Or_e_struct*
_tmp51E=(struct Cyc_Absyn_Or_e_struct*)_tmp503;if(_tmp51E->tag != 8)goto _LL241;
else{_tmp51F=_tmp51E->f1;_tmp520=_tmp51E->f2;}}_LL240: Cyc_Toc_exp_to_c(nv,
_tmp51F);Cyc_Toc_exp_to_c(nv,_tmp520);goto _LL22C;_LL241: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp521=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp503;if(_tmp521->tag != 9)goto _LL243;
else{_tmp522=_tmp521->f1;_tmp523=_tmp521->f2;}}_LL242: Cyc_Toc_exp_to_c(nv,
_tmp522);Cyc_Toc_exp_to_c(nv,_tmp523);goto _LL22C;_LL243: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp524=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp503;if(_tmp524->tag != 10)goto
_LL245;else{_tmp525=_tmp524->f1;_tmp526=_tmp524->f2;}}_LL244: _tmp528=_tmp525;
_tmp529=_tmp526;goto _LL246;_LL245: {struct Cyc_Absyn_FnCall_e_struct*_tmp527=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp503;if(_tmp527->tag != 11)goto _LL247;else{
_tmp528=_tmp527->f1;_tmp529=_tmp527->f2;_tmp52A=_tmp527->f3;if(_tmp52A != 0)goto
_LL247;}}_LL246: Cyc_Toc_exp_to_c(nv,_tmp528);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp529);goto _LL22C;_LL247: {struct Cyc_Absyn_FnCall_e_struct*
_tmp52B=(struct Cyc_Absyn_FnCall_e_struct*)_tmp503;if(_tmp52B->tag != 11)goto
_LL249;else{_tmp52C=_tmp52B->f1;_tmp52D=_tmp52B->f2;_tmp52E=_tmp52B->f3;if(
_tmp52E == 0)goto _LL249;_tmp52F=*_tmp52E;_tmp530=_tmp52F.num_varargs;_tmp531=
_tmp52F.injectors;_tmp532=_tmp52F.vai;}}_LL248: {struct Cyc_Toc_Env _tmp5E2;struct
_RegionHandle*_tmp5E3;struct Cyc_Toc_Env*_tmp5E1=nv;_tmp5E2=*_tmp5E1;_tmp5E3=
_tmp5E2.rgn;{struct _tuple0*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=
Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp530,0);void*cva_type=Cyc_Toc_typ_to_c(_tmp532->type);void*
arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp52D);int num_normargs=num_args - _tmp530;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp52D=_tmp52D->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp52D))->hd);{
struct Cyc_List_List*_tmpC83;new_args=((_tmpC83=_cycalloc(sizeof(*_tmpC83)),((
_tmpC83->hd=(struct Cyc_Absyn_Exp*)_tmp52D->hd,((_tmpC83->tl=new_args,_tmpC83))))));};}}{
struct Cyc_Absyn_Exp*_tmpC86[3];struct Cyc_List_List*_tmpC85;new_args=((_tmpC85=
_cycalloc(sizeof(*_tmpC85)),((_tmpC85->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((
_tmpC86[2]=num_varargs_exp,((_tmpC86[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0),((
_tmpC86[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC86,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpC85->tl=
new_args,_tmpC85))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp52C);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp52C,new_args,0),0);if(_tmp532->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp5E7=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp532->type));struct Cyc_Absyn_DatatypeInfo _tmp5E9;union Cyc_Absyn_DatatypeInfoU
_tmp5EA;struct Cyc_Absyn_Datatypedecl**_tmp5EB;struct Cyc_Absyn_Datatypedecl*
_tmp5EC;_LL2B7: {struct Cyc_Absyn_DatatypeType_struct*_tmp5E8=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp5E7;if(_tmp5E8->tag != 3)goto _LL2B9;else{_tmp5E9=_tmp5E8->f1;_tmp5EA=_tmp5E9.datatype_info;
if((_tmp5EA.KnownDatatype).tag != 2)goto _LL2B9;_tmp5EB=(struct Cyc_Absyn_Datatypedecl**)(
_tmp5EA.KnownDatatype).val;_tmp5EC=*_tmp5EB;}}_LL2B8: tud=_tmp5EC;goto _LL2B6;
_LL2B9:;_LL2BA: {const char*_tmpC89;void*_tmpC88;(_tmpC88=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC89="toc: unknown datatype in vararg with inject",
_tag_dyneither(_tmpC89,sizeof(char),44))),_tag_dyneither(_tmpC88,sizeof(void*),0)));}
_LL2B6:;}{unsigned int _tmpC8E;unsigned int _tmpC8D;struct _dyneither_ptr _tmpC8C;
struct _tuple0**_tmpC8B;unsigned int _tmpC8A;struct _dyneither_ptr vs=(_tmpC8A=(
unsigned int)_tmp530,((_tmpC8B=(struct _tuple0**)_region_malloc(_tmp5E3,
_check_times(sizeof(struct _tuple0*),_tmpC8A)),((_tmpC8C=_tag_dyneither(_tmpC8B,
sizeof(struct _tuple0*),_tmpC8A),((((_tmpC8D=_tmpC8A,_tmpC8F(& _tmpC8E,& _tmpC8D,&
_tmpC8B))),_tmpC8C)))))));if(_tmp530 != 0){struct Cyc_List_List*_tmp5EF=0;{int i=
_tmp530 - 1;for(0;i >= 0;-- i){struct Cyc_List_List*_tmpC90;_tmp5EF=((_tmpC90=
_cycalloc(sizeof(*_tmpC90)),((_tmpC90->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct
_tuple0*),i)),0),0)),((_tmpC90->tl=_tmp5EF,_tmpC90))))));}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp5EF,0),s,0);{
int i=0;for(0;_tmp52D != 0;(((_tmp52D=_tmp52D->tl,_tmp531=_tmp531->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp52D->hd;void*arg_type=(void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct _tuple0*var=*((struct
_tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield _tmp5F2;struct
_tuple0*_tmp5F3;struct Cyc_List_List*_tmp5F4;struct Cyc_Absyn_Datatypefield*
_tmp5F1=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp531))->hd;_tmp5F2=*_tmp5F1;_tmp5F3=_tmp5F2.name;_tmp5F4=_tmp5F2.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)_check_null(
_tmp5F4))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_boxed_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp5F3),0),s,0);{const char*_tmpC91;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp5F3,((_tmpC91="_struct",_tag_dyneither(_tmpC91,
sizeof(char),8))))),0,s,0);};};}};}else{struct _tuple15*_tmpC92[3];struct Cyc_List_List*
_tmp5F6=(_tmpC92[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC92[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpC92[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC92,sizeof(struct _tuple15*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{{int i=0;for(0;_tmp52D
!= 0;(_tmp52D=_tmp52D->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp52D->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp52D->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL22C;}_LL249: {struct Cyc_Absyn_Throw_e_struct*_tmp533=(struct Cyc_Absyn_Throw_e_struct*)
_tmp503;if(_tmp533->tag != 12)goto _LL24B;else{_tmp534=_tmp533->f1;}}_LL24A: Cyc_Toc_exp_to_c(
nv,_tmp534);e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(
_tmp534),0))->r;goto _LL22C;_LL24B: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp535=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp503;if(_tmp535->tag != 13)
goto _LL24D;else{_tmp536=_tmp535->f1;}}_LL24C: Cyc_Toc_exp_to_c(nv,_tmp536);goto
_LL22C;_LL24D: {struct Cyc_Absyn_Instantiate_e_struct*_tmp537=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp503;if(_tmp537->tag != 14)goto _LL24F;else{_tmp538=_tmp537->f1;_tmp539=_tmp537->f2;}}
_LL24E: Cyc_Toc_exp_to_c(nv,_tmp538);for(0;_tmp539 != 0;_tmp539=_tmp539->tl){enum 
Cyc_Absyn_KindQual _tmp5FC=(Cyc_Tcutil_typ_kind((void*)_tmp539->hd))->kind;if(
_tmp5FC != Cyc_Absyn_EffKind  && _tmp5FC != Cyc_Absyn_RgnKind){{void*_tmp5FD=Cyc_Tcutil_compress((
void*)_tmp539->hd);_LL2BC: {struct Cyc_Absyn_VarType_struct*_tmp5FE=(struct Cyc_Absyn_VarType_struct*)
_tmp5FD;if(_tmp5FE->tag != 2)goto _LL2BE;}_LL2BD: goto _LL2BF;_LL2BE: {struct Cyc_Absyn_DatatypeType_struct*
_tmp5FF=(struct Cyc_Absyn_DatatypeType_struct*)_tmp5FD;if(_tmp5FF->tag != 3)goto
_LL2C0;}_LL2BF: continue;_LL2C0:;_LL2C1: e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp538,0))->r;goto _LL2BB;
_LL2BB:;}break;}}goto _LL22C;_LL24F: {struct Cyc_Absyn_Cast_e_struct*_tmp53A=(
struct Cyc_Absyn_Cast_e_struct*)_tmp503;if(_tmp53A->tag != 15)goto _LL251;else{
_tmp53B=(void**)& _tmp53A->f1;_tmp53C=(void**)((void**)& _tmp53A->f1);_tmp53D=
_tmp53A->f2;_tmp53E=_tmp53A->f3;_tmp53F=_tmp53A->f4;}}_LL250: {void*old_t2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp53D->topt))->v;void*new_typ=*_tmp53C;*
_tmp53C=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp53D);{struct _tuple18
_tmpC93;struct _tuple18 _tmp601=(_tmpC93.f1=Cyc_Tcutil_compress(old_t2),((_tmpC93.f2=
Cyc_Tcutil_compress(new_typ),_tmpC93)));void*_tmp602;struct Cyc_Absyn_PtrInfo
_tmp604;void*_tmp605;struct Cyc_Absyn_PtrInfo _tmp607;void*_tmp608;struct Cyc_Absyn_PtrInfo
_tmp60A;void*_tmp60B;_LL2C3: _tmp602=_tmp601.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp603=(struct Cyc_Absyn_PointerType_struct*)_tmp602;if(_tmp603->tag != 5)goto
_LL2C5;else{_tmp604=_tmp603->f1;}};_tmp605=_tmp601.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp606=(struct Cyc_Absyn_PointerType_struct*)_tmp605;if(_tmp606->tag != 5)goto
_LL2C5;else{_tmp607=_tmp606->f1;}};_LL2C4: {int _tmp60D=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp604.ptr_atts).nullable);int _tmp60E=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp607.ptr_atts).nullable);
void*_tmp60F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp604.ptr_atts).bounds);void*_tmp610=((void*(*)(void*y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp607.ptr_atts).bounds);
int _tmp611=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp604.ptr_atts).zero_term);int _tmp612=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp607.ptr_atts).zero_term);{struct _tuple18 _tmpC94;
struct _tuple18 _tmp614=(_tmpC94.f1=_tmp60F,((_tmpC94.f2=_tmp610,_tmpC94)));void*
_tmp615;struct Cyc_Absyn_Exp*_tmp617;void*_tmp618;struct Cyc_Absyn_Exp*_tmp61A;
void*_tmp61B;struct Cyc_Absyn_Exp*_tmp61D;void*_tmp61E;void*_tmp620;void*_tmp622;
struct Cyc_Absyn_Exp*_tmp624;void*_tmp625;void*_tmp627;_LL2CA: _tmp615=_tmp614.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp616=(struct Cyc_Absyn_Upper_b_struct*)_tmp615;
if(_tmp616->tag != 1)goto _LL2CC;else{_tmp617=_tmp616->f1;}};_tmp618=_tmp614.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp619=(struct Cyc_Absyn_Upper_b_struct*)_tmp618;
if(_tmp619->tag != 1)goto _LL2CC;else{_tmp61A=_tmp619->f1;}};_LL2CB: if((!Cyc_Evexp_c_can_eval(
_tmp617) || !Cyc_Evexp_c_can_eval(_tmp61A)) && !Cyc_Evexp_same_const_exp(_tmp617,
_tmp61A)){const char*_tmpC97;void*_tmpC96;(_tmpC96=0,Cyc_Tcutil_terr(e->loc,((
_tmpC97="can't validate cast due to potential size differences",_tag_dyneither(
_tmpC97,sizeof(char),54))),_tag_dyneither(_tmpC96,sizeof(void*),0)));}if(_tmp60D
 && !_tmp60E){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC9A;void*_tmpC99;(
_tmpC99=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC9A="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC9A,
sizeof(char),44))),_tag_dyneither(_tmpC99,sizeof(void*),0)));}if(_tmp53F != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC9E;void*_tmpC9D[1];struct Cyc_String_pa_struct _tmpC9C;(_tmpC9C.tag=
0,((_tmpC9C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpC9D[0]=& _tmpC9C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpC9E="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpC9E,sizeof(char),41))),_tag_dyneither(_tmpC9D,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp53D);if(do_null_check){if(!_tmp53E){
const char*_tmpCA1;void*_tmpCA0;(_tmpCA0=0,Cyc_Tcutil_warn(e->loc,((_tmpCA1="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpCA1,sizeof(char),58))),_tag_dyneither(_tmpCA0,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpCA2;e->r=Cyc_Toc_cast_it_r(*_tmp53C,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpCA2=_cycalloc(sizeof(*_tmpCA2)),((_tmpCA2->hd=
_tmp53D,((_tmpCA2->tl=0,_tmpCA2)))))),0));};}};}goto _LL2C9;_LL2CC: _tmp61B=
_tmp614.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp61C=(struct Cyc_Absyn_Upper_b_struct*)
_tmp61B;if(_tmp61C->tag != 1)goto _LL2CE;else{_tmp61D=_tmp61C->f1;}};_tmp61E=
_tmp614.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp61F=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp61E;if(_tmp61F->tag != 0)goto _LL2CE;};_LL2CD: if(!Cyc_Evexp_c_can_eval(_tmp61D)){
const char*_tmpCA5;void*_tmpCA4;(_tmpCA4=0,Cyc_Tcutil_terr(e->loc,((_tmpCA5="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpCA5,sizeof(char),71))),_tag_dyneither(_tmpCA4,sizeof(void*),0)));}
if(_tmp53F == Cyc_Absyn_NonNull_to_Null){const char*_tmpCA9;void*_tmpCA8[1];struct
Cyc_String_pa_struct _tmpCA7;(_tmpCA7.tag=0,((_tmpCA7.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCA8[0]=& _tmpCA7,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCA9="conversion mis-classified as null-check: %s",
_tag_dyneither(_tmpCA9,sizeof(char),44))),_tag_dyneither(_tmpCA8,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){if((_tmp611  && !(_tmp607.elt_tq).real_const) && !
_tmp612)_tmp61D=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp61D,Cyc_Absyn_uint_exp(1,
0),0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp61D,_tmp53D))->r;}else{
struct Cyc_Absyn_Exp*_tmp638=Cyc_Toc__tag_dyneither_e;if(_tmp611){struct _tuple0*
_tmp639=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp63A=Cyc_Absyn_var_exp(_tmp639,
0);struct Cyc_Absyn_Exp*arg3;{void*_tmp63B=_tmp53D->r;union Cyc_Absyn_Cnst _tmp63D;
struct _dyneither_ptr _tmp63E;union Cyc_Absyn_Cnst _tmp640;struct _dyneither_ptr
_tmp641;_LL2D3: {struct Cyc_Absyn_Const_e_struct*_tmp63C=(struct Cyc_Absyn_Const_e_struct*)
_tmp63B;if(_tmp63C->tag != 0)goto _LL2D5;else{_tmp63D=_tmp63C->f1;if((_tmp63D.String_c).tag
!= 8)goto _LL2D5;_tmp63E=(struct _dyneither_ptr)(_tmp63D.String_c).val;}}_LL2D4:
arg3=_tmp61D;goto _LL2D2;_LL2D5: {struct Cyc_Absyn_Const_e_struct*_tmp63F=(struct
Cyc_Absyn_Const_e_struct*)_tmp63B;if(_tmp63F->tag != 0)goto _LL2D7;else{_tmp640=
_tmp63F->f1;if((_tmp640.Wstring_c).tag != 9)goto _LL2D7;_tmp641=(struct
_dyneither_ptr)(_tmp640.Wstring_c).val;}}_LL2D6: arg3=_tmp61D;goto _LL2D2;_LL2D7:;
_LL2D8:{struct Cyc_Absyn_Exp*_tmpCAA[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp53D),((_tmpCAA[1]=_tmp61D,((_tmpCAA[0]=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp63A),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAA,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);}goto _LL2D2;_LL2D2:;}if(!_tmp612  && !(_tmp607.elt_tq).real_const)arg3=
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*
_tmp643=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp607.elt_typ),0);struct Cyc_Absyn_Exp*
_tmpCAB[3];struct Cyc_Absyn_Exp*_tmp644=Cyc_Absyn_fncall_exp(_tmp638,((_tmpCAB[2]=
arg3,((_tmpCAB[1]=_tmp643,((_tmpCAB[0]=_tmp63A,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAB,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);struct Cyc_Absyn_Stmt*_tmp645=Cyc_Absyn_exp_stmt(_tmp644,0);_tmp645=
Cyc_Absyn_declare_stmt(_tmp639,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)
_tmp53D,_tmp645,0);e->r=Cyc_Toc_stmt_exp_r(_tmp645);};}else{struct Cyc_Absyn_Exp*
_tmpCAC[3];e->r=Cyc_Toc_fncall_exp_r(_tmp638,((_tmpCAC[2]=_tmp61D,((_tmpCAC[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp607.elt_typ),0),((_tmpCAC[0]=
_tmp53D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCAC,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2C9;
_LL2CE: _tmp620=_tmp614.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp621=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp620;if(_tmp621->tag != 0)goto _LL2D0;};_tmp622=_tmp614.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp623=(struct Cyc_Absyn_Upper_b_struct*)_tmp622;if(_tmp623->tag != 1)goto _LL2D0;
else{_tmp624=_tmp623->f1;}};_LL2CF: if(!Cyc_Evexp_c_can_eval(_tmp624)){const char*
_tmpCAF;void*_tmpCAE;(_tmpCAE=0,Cyc_Tcutil_terr(e->loc,((_tmpCAF="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpCAF,sizeof(char),71))),_tag_dyneither(_tmpCAE,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpCB2;void*_tmpCB1;(_tmpCB1=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCB2="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpCB2,sizeof(char),45))),_tag_dyneither(_tmpCB1,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp64C=_tmp624;if(_tmp611  && !_tmp612)_tmp64C=Cyc_Absyn_add_exp(
_tmp624,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp64D=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpCB3[3];struct Cyc_Absyn_Exp*_tmp64E=Cyc_Absyn_fncall_exp(
_tmp64D,((_tmpCB3[2]=_tmp64C,((_tmpCB3[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp604.elt_typ),0),((_tmpCB3[0]=_tmp53D,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB3,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp60E){struct Cyc_List_List*_tmpCB4;_tmp64E->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_null_e,((_tmpCB4=_cycalloc(sizeof(*_tmpCB4)),((_tmpCB4->hd=Cyc_Absyn_copy_exp(
_tmp64E),((_tmpCB4->tl=0,_tmpCB4)))))));}e->r=Cyc_Toc_cast_it_r(*_tmp53C,_tmp64E);
goto _LL2C9;};};_LL2D0: _tmp625=_tmp614.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp626=(struct Cyc_Absyn_DynEither_b_struct*)_tmp625;if(_tmp626->tag != 0)goto
_LL2C9;};_tmp627=_tmp614.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp628=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp627;if(_tmp628->tag != 0)goto _LL2C9;};_LL2D1:
DynCast: if((_tmp611  && !_tmp612) && !(_tmp607.elt_tq).real_const){if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpCB7;void*_tmpCB6;(_tmpCB6=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCB7="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpCB7,sizeof(char),70))),_tag_dyneither(_tmpCB6,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp653=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpCB8[3];e->r=Cyc_Toc_fncall_exp_r(_tmp653,((_tmpCB8[2]=Cyc_Absyn_uint_exp(1,0),((
_tmpCB8[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp604.elt_typ),0),((
_tmpCB8[0]=_tmp53D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCB8,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}goto _LL2C9;
_LL2C9:;}goto _LL2C2;}_LL2C5: _tmp608=_tmp601.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp609=(struct Cyc_Absyn_PointerType_struct*)_tmp608;if(_tmp609->tag != 5)goto
_LL2C7;else{_tmp60A=_tmp609->f1;}};_tmp60B=_tmp601.f2;{struct Cyc_Absyn_IntType_struct*
_tmp60C=(struct Cyc_Absyn_IntType_struct*)_tmp60B;if(_tmp60C->tag != 6)goto _LL2C7;};
_LL2C6:{void*_tmp655=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp60A.ptr_atts).bounds);_LL2DA: {struct Cyc_Absyn_DynEither_b_struct*
_tmp656=(struct Cyc_Absyn_DynEither_b_struct*)_tmp655;if(_tmp656->tag != 0)goto
_LL2DC;}_LL2DB: _tmp53D->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp53D->r,
_tmp53D->loc),Cyc_Toc_curr_sp);goto _LL2D9;_LL2DC:;_LL2DD: goto _LL2D9;_LL2D9:;}
goto _LL2C2;_LL2C7:;_LL2C8: goto _LL2C2;_LL2C2:;}goto _LL22C;}_LL251: {struct Cyc_Absyn_Address_e_struct*
_tmp540=(struct Cyc_Absyn_Address_e_struct*)_tmp503;if(_tmp540->tag != 16)goto
_LL253;else{_tmp541=_tmp540->f1;}}_LL252:{void*_tmp657=_tmp541->r;struct _tuple0*
_tmp659;struct Cyc_List_List*_tmp65A;struct Cyc_List_List*_tmp65B;struct Cyc_List_List*
_tmp65D;_LL2DF: {struct Cyc_Absyn_Aggregate_e_struct*_tmp658=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp657;if(_tmp658->tag != 29)goto _LL2E1;else{_tmp659=_tmp658->f1;_tmp65A=_tmp658->f2;
_tmp65B=_tmp658->f3;}}_LL2E0: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpCBC;void*
_tmpCBB[1];struct Cyc_String_pa_struct _tmpCBA;(_tmpCBA.tag=0,((_tmpCBA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp541->loc)),((
_tmpCBB[0]=& _tmpCBA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpCBC="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpCBC,
sizeof(char),42))),_tag_dyneither(_tmpCBB,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp541->topt))->v,_tmp65A != 0,1,0,
_tmp65B,_tmp659))->r;goto _LL2DE;_LL2E1: {struct Cyc_Absyn_Tuple_e_struct*_tmp65C=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp657;if(_tmp65C->tag != 25)goto _LL2E3;else{
_tmp65D=_tmp65C->f1;}}_LL2E2: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpCC0;void*
_tmpCBF[1];struct Cyc_String_pa_struct _tmpCBE;(_tmpCBE.tag=0,((_tmpCBE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp541->loc)),((
_tmpCBF[0]=& _tmpCBE,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpCC0="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpCC0,
sizeof(char),42))),_tag_dyneither(_tmpCBF,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_tuple(
nv,1,0,_tmp65D))->r;goto _LL2DE;_LL2E3:;_LL2E4: Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(
nv,_tmp541);Cyc_Toc_set_lhs(nv,0);if(!Cyc_Absyn_is_lvalue(_tmp541)){((void(*)(
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
int),int f_env))Cyc_Toc_lvalue_assign)(_tmp541,0,Cyc_Toc_address_lvalue,1);e->r=
Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmp541);}goto _LL2DE;_LL2DE:;}goto _LL22C;_LL253: {struct Cyc_Absyn_New_e_struct*
_tmp542=(struct Cyc_Absyn_New_e_struct*)_tmp503;if(_tmp542->tag != 17)goto _LL255;
else{_tmp543=_tmp542->f1;_tmp544=_tmp542->f2;}}_LL254: if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCC4;void*_tmpCC3[1];struct Cyc_String_pa_struct _tmpCC2;(_tmpCC2.tag=
0,((_tmpCC2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
_tmp544->loc)),((_tmpCC3[0]=& _tmpCC2,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCC4="%s: new at top-level",_tag_dyneither(
_tmpCC4,sizeof(char),21))),_tag_dyneither(_tmpCC3,sizeof(void*),1)))))));}{void*
_tmp667=_tmp544->r;struct Cyc_List_List*_tmp669;struct Cyc_Absyn_Vardecl*_tmp66B;
struct Cyc_Absyn_Exp*_tmp66C;struct Cyc_Absyn_Exp*_tmp66D;int _tmp66E;struct _tuple0*
_tmp670;struct Cyc_List_List*_tmp671;struct Cyc_List_List*_tmp672;struct Cyc_Absyn_Aggrdecl*
_tmp673;struct Cyc_List_List*_tmp675;_LL2E6: {struct Cyc_Absyn_Array_e_struct*
_tmp668=(struct Cyc_Absyn_Array_e_struct*)_tmp667;if(_tmp668->tag != 27)goto _LL2E8;
else{_tmp669=_tmp668->f1;}}_LL2E7: {struct _tuple0*_tmp676=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp677=Cyc_Absyn_var_exp(_tmp676,0);struct Cyc_Absyn_Stmt*
_tmp678=Cyc_Toc_init_array(nv,_tmp677,_tmp669,Cyc_Absyn_exp_stmt(_tmp677,0));
void*old_elt_typ;{void*_tmp679=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp67B;void*_tmp67C;struct Cyc_Absyn_Tqual _tmp67D;struct Cyc_Absyn_PtrAtts _tmp67E;
union Cyc_Absyn_Constraint*_tmp67F;_LL2F1: {struct Cyc_Absyn_PointerType_struct*
_tmp67A=(struct Cyc_Absyn_PointerType_struct*)_tmp679;if(_tmp67A->tag != 5)goto
_LL2F3;else{_tmp67B=_tmp67A->f1;_tmp67C=_tmp67B.elt_typ;_tmp67D=_tmp67B.elt_tq;
_tmp67E=_tmp67B.ptr_atts;_tmp67F=_tmp67E.zero_term;}}_LL2F2: old_elt_typ=_tmp67C;
goto _LL2F0;_LL2F3:;_LL2F4: {const char*_tmpCC7;void*_tmpCC6;old_elt_typ=((_tmpCC6=
0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCC7="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(
_tmpCC7,sizeof(char),52))),_tag_dyneither(_tmpCC6,sizeof(void*),0))));}_LL2F0:;}{
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp682=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp683=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp669),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp543 == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp683);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp543;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp683);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp676,_tmp682,(struct Cyc_Absyn_Exp*)e1,_tmp678,0));goto
_LL2E5;};}_LL2E8: {struct Cyc_Absyn_Comprehension_e_struct*_tmp66A=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp667;if(_tmp66A->tag != 28)goto _LL2EA;else{_tmp66B=_tmp66A->f1;_tmp66C=_tmp66A->f2;
_tmp66D=_tmp66A->f3;_tmp66E=_tmp66A->f4;}}_LL2E9: {int is_dyneither_ptr=0;{void*
_tmp684=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp686;void*_tmp687;
struct Cyc_Absyn_Tqual _tmp688;struct Cyc_Absyn_PtrAtts _tmp689;union Cyc_Absyn_Constraint*
_tmp68A;union Cyc_Absyn_Constraint*_tmp68B;_LL2F6: {struct Cyc_Absyn_PointerType_struct*
_tmp685=(struct Cyc_Absyn_PointerType_struct*)_tmp684;if(_tmp685->tag != 5)goto
_LL2F8;else{_tmp686=_tmp685->f1;_tmp687=_tmp686.elt_typ;_tmp688=_tmp686.elt_tq;
_tmp689=_tmp686.ptr_atts;_tmp68A=_tmp689.bounds;_tmp68B=_tmp689.zero_term;}}
_LL2F7: is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp68A)== (void*)& Cyc_Absyn_DynEither_b_val;goto _LL2F5;
_LL2F8:;_LL2F9: {const char*_tmpCCA;void*_tmpCC9;(_tmpCC9=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCCA="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpCCA,sizeof(char),42))),_tag_dyneither(_tmpCC9,sizeof(void*),0)));}
_LL2F5:;}{struct _tuple0*max=Cyc_Toc_temp_var();struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp66D->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp66C);{struct Cyc_Absyn_Exp*_tmp68E=
Cyc_Absyn_var_exp(max,0);if(_tmp66E)_tmp68E=Cyc_Absyn_add_exp(_tmp68E,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp66B,Cyc_Absyn_var_exp(max,0),_tmp66D,_tmp66E,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp690;struct _RegionHandle*_tmp691;struct Cyc_Toc_Env*_tmp68F=
nv;_tmp690=*_tmp68F;_tmp691=_tmp690.rgn;{struct _tuple20*_tmpCCD;struct Cyc_List_List*
_tmpCCC;struct Cyc_List_List*decls=(_tmpCCC=_region_malloc(_tmp691,sizeof(*
_tmpCCC)),((_tmpCCC->hd=((_tmpCCD=_region_malloc(_tmp691,sizeof(*_tmpCCD)),((
_tmpCCD->f1=max,((_tmpCCD->f2=Cyc_Absyn_uint_typ,((_tmpCCD->f3=(struct Cyc_Absyn_Exp*)
_tmp66C,_tmpCCD)))))))),((_tmpCCC->tl=0,_tmpCCC)))));struct Cyc_Absyn_Exp*ai;if(
_tmp543 == 0  || Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*_tmpCCE[2];ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((_tmpCCE[1]=_tmp68E,((
_tmpCCE[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCCE,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp543;Cyc_Toc_exp_to_c(
nv,r);{struct Cyc_Absyn_Exp*_tmpCCF[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpCCF[1]=_tmp68E,((_tmpCCF[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCCF,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct _tuple20*_tmpCD2;struct Cyc_List_List*
_tmpCD1;decls=((_tmpCD1=_region_malloc(_tmp691,sizeof(*_tmpCD1)),((_tmpCD1->hd=((
_tmpCD2=_region_malloc(_tmp691,sizeof(*_tmpCD2)),((_tmpCD2->f1=a,((_tmpCD2->f2=
ptr_typ,((_tmpCD2->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpCD2)))))))),((_tmpCD1->tl=
decls,_tmpCD1))))));}if(is_dyneither_ptr){struct _tuple0*_tmp696=Cyc_Toc_temp_var();
void*_tmp697=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp698=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpCD3[3];struct Cyc_Absyn_Exp*_tmp699=Cyc_Absyn_fncall_exp(
_tmp698,((_tmpCD3[2]=_tmp68E,((_tmpCD3[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((
_tmpCD3[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpCD3,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{
struct _tuple20*_tmpCD6;struct Cyc_List_List*_tmpCD5;decls=((_tmpCD5=
_region_malloc(_tmp691,sizeof(*_tmpCD5)),((_tmpCD5->hd=((_tmpCD6=_region_malloc(
_tmp691,sizeof(*_tmpCD6)),((_tmpCD6->f1=_tmp696,((_tmpCD6->f2=_tmp697,((_tmpCD6->f3=(
struct Cyc_Absyn_Exp*)_tmp699,_tmpCD6)))))))),((_tmpCD5->tl=decls,_tmpCD5))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp696,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp69D=decls;for(0;_tmp69D != 0;_tmp69D=_tmp69D->tl){struct
_tuple0*_tmp69F;void*_tmp6A0;struct Cyc_Absyn_Exp*_tmp6A1;struct _tuple20 _tmp69E=*((
struct _tuple20*)_tmp69D->hd);_tmp69F=_tmp69E.f1;_tmp6A0=_tmp69E.f2;_tmp6A1=
_tmp69E.f3;s=Cyc_Absyn_declare_stmt(_tmp69F,_tmp6A0,_tmp6A1,s,0);}}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL2E5;};};};}_LL2EA: {struct Cyc_Absyn_Aggregate_e_struct*_tmp66F=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp667;if(_tmp66F->tag != 29)goto _LL2EC;
else{_tmp670=_tmp66F->f1;_tmp671=_tmp66F->f2;_tmp672=_tmp66F->f3;_tmp673=_tmp66F->f4;}}
_LL2EB: e->r=(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp544->topt))->v,_tmp671 != 0,1,_tmp543,_tmp672,_tmp670))->r;goto _LL2E5;_LL2EC: {
struct Cyc_Absyn_Tuple_e_struct*_tmp674=(struct Cyc_Absyn_Tuple_e_struct*)_tmp667;
if(_tmp674->tag != 25)goto _LL2EE;else{_tmp675=_tmp674->f1;}}_LL2ED: e->r=(Cyc_Toc_init_tuple(
nv,1,_tmp543,_tmp675))->r;goto _LL2E5;_LL2EE:;_LL2EF: {void*old_elt_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp544->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);struct _tuple0*_tmp6A4=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp6A5=Cyc_Absyn_var_exp(_tmp6A4,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(_tmp6A5,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp543
== 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp543;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp6A6=_tmp544->r;void*_tmp6A8;
struct Cyc_Absyn_Exp*_tmp6A9;_LL2FB: {struct Cyc_Absyn_Cast_e_struct*_tmp6A7=(
struct Cyc_Absyn_Cast_e_struct*)_tmp6A6;if(_tmp6A7->tag != 15)goto _LL2FD;else{
_tmp6A8=(void*)_tmp6A7->f1;_tmp6A9=_tmp6A7->f2;}}_LL2FC:{struct _tuple18 _tmpCD7;
struct _tuple18 _tmp6AB=(_tmpCD7.f1=Cyc_Tcutil_compress(_tmp6A8),((_tmpCD7.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp6A9->topt))->v),_tmpCD7)));void*
_tmp6AC;struct Cyc_Absyn_PtrInfo _tmp6AE;void*_tmp6AF;struct Cyc_Absyn_PtrAtts
_tmp6B0;union Cyc_Absyn_Constraint*_tmp6B1;void*_tmp6B2;struct Cyc_Absyn_PtrInfo
_tmp6B4;struct Cyc_Absyn_PtrAtts _tmp6B5;union Cyc_Absyn_Constraint*_tmp6B6;_LL300:
_tmp6AC=_tmp6AB.f1;{struct Cyc_Absyn_PointerType_struct*_tmp6AD=(struct Cyc_Absyn_PointerType_struct*)
_tmp6AC;if(_tmp6AD->tag != 5)goto _LL302;else{_tmp6AE=_tmp6AD->f1;_tmp6AF=_tmp6AE.elt_typ;
_tmp6B0=_tmp6AE.ptr_atts;_tmp6B1=_tmp6B0.bounds;}};_tmp6B2=_tmp6AB.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp6B3=(struct Cyc_Absyn_PointerType_struct*)_tmp6B2;if(_tmp6B3->tag != 5)goto
_LL302;else{_tmp6B4=_tmp6B3->f1;_tmp6B5=_tmp6B4.ptr_atts;_tmp6B6=_tmp6B5.bounds;}};
_LL301:{struct _tuple18 _tmpCD8;struct _tuple18 _tmp6B8=(_tmpCD8.f1=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6B1),((_tmpCD8.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp6B6),_tmpCD8)));void*_tmp6B9;void*_tmp6BB;struct Cyc_Absyn_Exp*
_tmp6BD;_LL305: _tmp6B9=_tmp6B8.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp6BA=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp6B9;if(_tmp6BA->tag != 0)goto _LL307;};
_tmp6BB=_tmp6B8.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp6BC=(struct Cyc_Absyn_Upper_b_struct*)
_tmp6BB;if(_tmp6BC->tag != 1)goto _LL307;else{_tmp6BD=_tmp6BC->f1;}};_LL306: Cyc_Toc_exp_to_c(
nv,_tmp6A9);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp6BE=Cyc_Toc__init_dyneither_ptr_e;{struct Cyc_Absyn_Exp*_tmpCD9[4];e->r=Cyc_Toc_fncall_exp_r(
_tmp6BE,((_tmpCD9[3]=_tmp6BD,((_tmpCD9[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp6AF),0),((_tmpCD9[1]=_tmp6A9,((_tmpCD9[0]=mexp,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD9,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))));}goto _LL304;};_LL307:;_LL308: goto _LL304;_LL304:;}goto _LL2FF;_LL302:;
_LL303: goto _LL2FF;_LL2FF:;}goto _LL2FA;_LL2FD:;_LL2FE: goto _LL2FA;_LL2FA:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp6C0=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp6A5),
0);struct Cyc_Absyn_Exp*_tmp6C1=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,
_tmp544);_tmp6C0=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
_tmp6A5,_tmp6C1,0),_tmp544,0),_tmp6C0,0);{void*_tmp6C2=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp6A4,
_tmp6C2,(struct Cyc_Absyn_Exp*)mexp,_tmp6C0,0));};}goto _LL2E5;};}_LL2E5:;}goto
_LL22C;_LL255: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp545=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp503;if(_tmp545->tag != 19)goto _LL257;else{_tmp546=_tmp545->f1;}}_LL256: Cyc_Toc_exp_to_c(
nv,_tmp546);goto _LL22C;_LL257: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp547=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp503;if(_tmp547->tag != 18)goto _LL259;
else{_tmp548=(void*)_tmp547->f1;}}_LL258:{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpCDC;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpCDB;e->r=(void*)((_tmpCDB=
_cycalloc(sizeof(*_tmpCDB)),((_tmpCDB[0]=((_tmpCDC.tag=18,((_tmpCDC.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp548),_tmpCDC)))),_tmpCDB))));}goto _LL22C;_LL259: {
struct Cyc_Absyn_Offsetof_e_struct*_tmp549=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp503;if(_tmp549->tag != 20)goto _LL25B;else{_tmp54A=(void*)_tmp549->f1;_tmp54B=(
void*)_tmp549->f2;{struct Cyc_Absyn_StructField_struct*_tmp54C=(struct Cyc_Absyn_StructField_struct*)
_tmp54B;if(_tmp54C->tag != 0)goto _LL25B;else{_tmp54D=_tmp54C->f1;}};}}_LL25A:{
struct Cyc_Absyn_Offsetof_e_struct _tmpCE6;struct Cyc_Absyn_StructField_struct
_tmpCE5;struct Cyc_Absyn_StructField_struct*_tmpCE4;struct Cyc_Absyn_Offsetof_e_struct*
_tmpCE3;e->r=(void*)((_tmpCE3=_cycalloc(sizeof(*_tmpCE3)),((_tmpCE3[0]=((_tmpCE6.tag=
20,((_tmpCE6.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp54A),((_tmpCE6.f2=(void*)((
void*)((_tmpCE4=_cycalloc(sizeof(*_tmpCE4)),((_tmpCE4[0]=((_tmpCE5.tag=0,((
_tmpCE5.f1=_tmp54D,_tmpCE5)))),_tmpCE4))))),_tmpCE6)))))),_tmpCE3))));}goto
_LL22C;_LL25B: {struct Cyc_Absyn_Offsetof_e_struct*_tmp54E=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp503;if(_tmp54E->tag != 20)goto _LL25D;else{_tmp54F=(void*)_tmp54E->f1;_tmp550=(
void*)_tmp54E->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp551=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp550;if(_tmp551->tag != 1)goto _LL25D;else{_tmp552=_tmp551->f1;}};}}_LL25C:{
void*_tmp6C9=Cyc_Tcutil_compress(_tmp54F);struct Cyc_Absyn_AggrInfo _tmp6CB;union
Cyc_Absyn_AggrInfoU _tmp6CC;struct Cyc_List_List*_tmp6CE;_LL30A: {struct Cyc_Absyn_AggrType_struct*
_tmp6CA=(struct Cyc_Absyn_AggrType_struct*)_tmp6C9;if(_tmp6CA->tag != 12)goto
_LL30C;else{_tmp6CB=_tmp6CA->f1;_tmp6CC=_tmp6CB.aggr_info;}}_LL30B: {struct Cyc_Absyn_Aggrdecl*
_tmp6D1=Cyc_Absyn_get_known_aggrdecl(_tmp6CC);if(_tmp6D1->impl == 0){const char*
_tmpCE9;void*_tmpCE8;(_tmpCE8=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCE9="struct fields must be known",
_tag_dyneither(_tmpCE9,sizeof(char),28))),_tag_dyneither(_tmpCE8,sizeof(void*),0)));}
_tmp6CE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6D1->impl))->fields;goto
_LL30D;}_LL30C: {struct Cyc_Absyn_AnonAggrType_struct*_tmp6CD=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp6C9;if(_tmp6CD->tag != 13)goto _LL30E;else{_tmp6CE=_tmp6CD->f2;}}_LL30D: {
struct Cyc_Absyn_Aggrfield*_tmp6D4=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp6CE,(int)_tmp552);{struct Cyc_Absyn_Offsetof_e_struct
_tmpCF3;struct Cyc_Absyn_StructField_struct _tmpCF2;struct Cyc_Absyn_StructField_struct*
_tmpCF1;struct Cyc_Absyn_Offsetof_e_struct*_tmpCF0;e->r=(void*)((_tmpCF0=
_cycalloc(sizeof(*_tmpCF0)),((_tmpCF0[0]=((_tmpCF3.tag=20,((_tmpCF3.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp54F),((_tmpCF3.f2=(void*)((void*)((_tmpCF1=_cycalloc(
sizeof(*_tmpCF1)),((_tmpCF1[0]=((_tmpCF2.tag=0,((_tmpCF2.f1=_tmp6D4->name,
_tmpCF2)))),_tmpCF1))))),_tmpCF3)))))),_tmpCF0))));}goto _LL309;}_LL30E: {struct
Cyc_Absyn_TupleType_struct*_tmp6CF=(struct Cyc_Absyn_TupleType_struct*)_tmp6C9;
if(_tmp6CF->tag != 11)goto _LL310;}_LL30F:{struct Cyc_Absyn_Offsetof_e_struct
_tmpCFD;struct Cyc_Absyn_StructField_struct _tmpCFC;struct Cyc_Absyn_StructField_struct*
_tmpCFB;struct Cyc_Absyn_Offsetof_e_struct*_tmpCFA;e->r=(void*)((_tmpCFA=
_cycalloc(sizeof(*_tmpCFA)),((_tmpCFA[0]=((_tmpCFD.tag=20,((_tmpCFD.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp54F),((_tmpCFD.f2=(void*)((void*)((_tmpCFB=_cycalloc(
sizeof(*_tmpCFB)),((_tmpCFB[0]=((_tmpCFC.tag=0,((_tmpCFC.f1=Cyc_Absyn_fieldname((
int)(_tmp552 + 1)),_tmpCFC)))),_tmpCFB))))),_tmpCFD)))))),_tmpCFA))));}goto _LL309;
_LL310: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp6D0=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp6C9;if(_tmp6D0->tag != 4)goto _LL312;}_LL311: if(_tmp552 == 0){struct Cyc_Absyn_Offsetof_e_struct
_tmpD07;struct Cyc_Absyn_StructField_struct _tmpD06;struct Cyc_Absyn_StructField_struct*
_tmpD05;struct Cyc_Absyn_Offsetof_e_struct*_tmpD04;e->r=(void*)((_tmpD04=
_cycalloc(sizeof(*_tmpD04)),((_tmpD04[0]=((_tmpD07.tag=20,((_tmpD07.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp54F),((_tmpD07.f2=(void*)((void*)((_tmpD05=_cycalloc(
sizeof(*_tmpD05)),((_tmpD05[0]=((_tmpD06.tag=0,((_tmpD06.f1=Cyc_Toc_tag_sp,
_tmpD06)))),_tmpD05))))),_tmpD07)))))),_tmpD04))));}else{struct Cyc_Absyn_Offsetof_e_struct
_tmpD11;struct Cyc_Absyn_StructField_struct _tmpD10;struct Cyc_Absyn_StructField_struct*
_tmpD0F;struct Cyc_Absyn_Offsetof_e_struct*_tmpD0E;e->r=(void*)((_tmpD0E=
_cycalloc(sizeof(*_tmpD0E)),((_tmpD0E[0]=((_tmpD11.tag=20,((_tmpD11.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp54F),((_tmpD11.f2=(void*)((void*)((_tmpD0F=_cycalloc(
sizeof(*_tmpD0F)),((_tmpD0F[0]=((_tmpD10.tag=0,((_tmpD10.f1=Cyc_Absyn_fieldname((
int)_tmp552),_tmpD10)))),_tmpD0F))))),_tmpD11)))))),_tmpD0E))));}goto _LL309;
_LL312:;_LL313: {const char*_tmpD14;void*_tmpD13;(_tmpD13=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD14="impossible type for offsetof tuple index",
_tag_dyneither(_tmpD14,sizeof(char),41))),_tag_dyneither(_tmpD13,sizeof(void*),0)));}
_LL309:;}goto _LL22C;_LL25D: {struct Cyc_Absyn_Deref_e_struct*_tmp553=(struct Cyc_Absyn_Deref_e_struct*)
_tmp503;if(_tmp553->tag != 21)goto _LL25F;else{_tmp554=_tmp553->f1;}}_LL25E: {int
_tmp6E7=Cyc_Toc_in_lhs(nv);Cyc_Toc_set_lhs(nv,0);{void*_tmp6E8=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp554->topt))->v);{void*_tmp6E9=
_tmp6E8;struct Cyc_Absyn_PtrInfo _tmp6EB;void*_tmp6EC;struct Cyc_Absyn_Tqual _tmp6ED;
struct Cyc_Absyn_PtrAtts _tmp6EE;void*_tmp6EF;union Cyc_Absyn_Constraint*_tmp6F0;
union Cyc_Absyn_Constraint*_tmp6F1;union Cyc_Absyn_Constraint*_tmp6F2;_LL315: {
struct Cyc_Absyn_PointerType_struct*_tmp6EA=(struct Cyc_Absyn_PointerType_struct*)
_tmp6E9;if(_tmp6EA->tag != 5)goto _LL317;else{_tmp6EB=_tmp6EA->f1;_tmp6EC=_tmp6EB.elt_typ;
_tmp6ED=_tmp6EB.elt_tq;_tmp6EE=_tmp6EB.ptr_atts;_tmp6EF=_tmp6EE.rgn;_tmp6F0=
_tmp6EE.nullable;_tmp6F1=_tmp6EE.bounds;_tmp6F2=_tmp6EE.zero_term;}}_LL316:{void*
_tmp6F3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6F1);struct Cyc_Absyn_Exp*_tmp6F5;_LL31A: {struct Cyc_Absyn_Upper_b_struct*
_tmp6F4=(struct Cyc_Absyn_Upper_b_struct*)_tmp6F3;if(_tmp6F4->tag != 1)goto _LL31C;
else{_tmp6F5=_tmp6F4->f1;}}_LL31B: {int do_null_check=Cyc_Toc_need_null_check(
_tmp554);Cyc_Toc_exp_to_c(nv,_tmp554);if(do_null_check){if(Cyc_Toc_warn_all_null_deref){
const char*_tmpD17;void*_tmpD16;(_tmpD16=0,Cyc_Tcutil_warn(e->loc,((_tmpD17="inserted null check due to dereference",
_tag_dyneither(_tmpD17,sizeof(char),39))),_tag_dyneither(_tmpD16,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpD18;_tmp554->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp6E8),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpD18=_cycalloc(sizeof(*
_tmpD18)),((_tmpD18->hd=Cyc_Absyn_copy_exp(_tmp554),((_tmpD18->tl=0,_tmpD18)))))),
0));};}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6F2)){unsigned int _tmp6FB;int _tmp6FC;struct _tuple10 _tmp6FA=Cyc_Evexp_eval_const_uint_exp(
_tmp6F5);_tmp6FB=_tmp6FA.f1;_tmp6FC=_tmp6FA.f2;if(!_tmp6FC  || _tmp6FB <= 0){const
char*_tmpD1B;void*_tmpD1A;(_tmpD1A=0,Cyc_Tcutil_terr(e->loc,((_tmpD1B="cannot determine dereference is in bounds",
_tag_dyneither(_tmpD1B,sizeof(char),42))),_tag_dyneither(_tmpD1A,sizeof(void*),0)));}}
goto _LL319;}_LL31C: {struct Cyc_Absyn_DynEither_b_struct*_tmp6F6=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp6F3;if(_tmp6F6->tag != 0)goto _LL319;}_LL31D: {struct Cyc_Absyn_Exp*_tmp6FF=Cyc_Absyn_uint_exp(
0,0);{struct Cyc_Core_Opt*_tmpD1C;_tmp6FF->topt=((_tmpD1C=_cycalloc(sizeof(*
_tmpD1C)),((_tmpD1C->v=Cyc_Absyn_uint_typ,_tmpD1C))));}e->r=Cyc_Toc_subscript_exp_r(
_tmp554,_tmp6FF);Cyc_Toc_exp_to_c(nv,e);goto _LL319;}_LL319:;}goto _LL314;_LL317:;
_LL318: {const char*_tmpD1F;void*_tmpD1E;(_tmpD1E=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD1F="exp_to_c: Deref: non-pointer",
_tag_dyneither(_tmpD1F,sizeof(char),29))),_tag_dyneither(_tmpD1E,sizeof(void*),0)));}
_LL314:;}Cyc_Toc_set_lhs(nv,_tmp6E7);goto _LL22C;};}_LL25F: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp555=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp503;if(_tmp555->tag != 22)goto
_LL261;else{_tmp556=_tmp555->f1;_tmp557=_tmp555->f2;_tmp558=_tmp555->f3;_tmp559=
_tmp555->f4;}}_LL260: {int is_poly=Cyc_Toc_is_poly_project(e);void*e1_cyc_type=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp556->topt))->v;Cyc_Toc_exp_to_c(nv,
_tmp556);if(_tmp558  && _tmp559)e->r=Cyc_Toc_check_tagged_union(_tmp556,Cyc_Toc_typ_to_c(
e1_cyc_type),e1_cyc_type,_tmp557,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrmember_exp);
if(is_poly)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp(e->r,0),0))->r;goto _LL22C;}_LL261: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp55A=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp503;if(_tmp55A->tag != 23)goto _LL263;else{_tmp55B=_tmp55A->f1;_tmp55C=_tmp55A->f2;
_tmp55D=_tmp55A->f3;_tmp55E=_tmp55A->f4;}}_LL262: {int _tmp703=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp55B->topt))->v);int do_null_check=Cyc_Toc_need_null_check(_tmp55B);
Cyc_Toc_exp_to_c(nv,_tmp55B);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp705;
struct Cyc_Absyn_Tqual _tmp706;struct Cyc_Absyn_PtrAtts _tmp707;void*_tmp708;union
Cyc_Absyn_Constraint*_tmp709;union Cyc_Absyn_Constraint*_tmp70A;union Cyc_Absyn_Constraint*
_tmp70B;struct Cyc_Absyn_PtrInfo _tmp704=Cyc_Toc_get_ptr_type(e1typ);_tmp705=
_tmp704.elt_typ;_tmp706=_tmp704.elt_tq;_tmp707=_tmp704.ptr_atts;_tmp708=_tmp707.rgn;
_tmp709=_tmp707.nullable;_tmp70A=_tmp707.bounds;_tmp70B=_tmp707.zero_term;{void*
_tmp70C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp70A);struct Cyc_Absyn_Exp*_tmp70E;_LL31F: {struct Cyc_Absyn_Upper_b_struct*
_tmp70D=(struct Cyc_Absyn_Upper_b_struct*)_tmp70C;if(_tmp70D->tag != 1)goto _LL321;
else{_tmp70E=_tmp70D->f1;}}_LL320: {unsigned int _tmp711;int _tmp712;struct
_tuple10 _tmp710=Cyc_Evexp_eval_const_uint_exp(_tmp70E);_tmp711=_tmp710.f1;
_tmp712=_tmp710.f2;if(_tmp712){if(_tmp711 < 1){const char*_tmpD22;void*_tmpD21;(
_tmpD21=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD22="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpD22,
sizeof(char),44))),_tag_dyneither(_tmpD21,sizeof(void*),0)));}if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){const char*_tmpD25;void*_tmpD24;(_tmpD24=0,Cyc_Tcutil_warn(
e->loc,((_tmpD25="inserted null check due to dereference",_tag_dyneither(_tmpD25,
sizeof(char),39))),_tag_dyneither(_tmpD24,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpD26[1];_tmp55B->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpD26[0]=Cyc_Absyn_new_exp(_tmp55B->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD26,sizeof(struct Cyc_Absyn_Exp*),
1)))),0));};}}else{if(!Cyc_Evexp_c_can_eval(_tmp70E)){const char*_tmpD29;void*
_tmpD28;(_tmpD28=0,Cyc_Tcutil_terr(e->loc,((_tmpD29="cannot determine pointer dereference in bounds",
_tag_dyneither(_tmpD29,sizeof(char),47))),_tag_dyneither(_tmpD28,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpD2A[4];_tmp55B->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((_tmpD2A[3]=
Cyc_Absyn_uint_exp(0,0),((_tmpD2A[2]=Cyc_Absyn_sizeoftyp_exp(_tmp705,0),((
_tmpD2A[1]=_tmp70E,((_tmpD2A[0]=Cyc_Absyn_new_exp(_tmp55B->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD2A,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0));};}goto _LL31E;}_LL321: {struct Cyc_Absyn_DynEither_b_struct*
_tmp70F=(struct Cyc_Absyn_DynEither_b_struct*)_tmp70C;if(_tmp70F->tag != 0)goto
_LL31E;}_LL322: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp705);{struct Cyc_Absyn_Exp*
_tmpD2B[3];_tmp55B->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp706),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_dyneither_subscript_e,((_tmpD2B[2]=Cyc_Absyn_uint_exp(0,0),((
_tmpD2B[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD2B[0]=Cyc_Absyn_new_exp(_tmp55B->r,
0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD2B,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}goto _LL31E;}_LL31E:;}if(
_tmp55D  && _tmp55E)e->r=Cyc_Toc_check_tagged_union(_tmp55B,Cyc_Toc_typ_to_c(
e1typ),_tmp705,_tmp55C,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);if(is_poly
 && _tmp55E)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp(e->r,0),0))->r;Cyc_Toc_set_lhs(nv,
_tmp703);goto _LL22C;};};}_LL263: {struct Cyc_Absyn_Subscript_e_struct*_tmp55F=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp503;if(_tmp55F->tag != 24)goto _LL265;
else{_tmp560=_tmp55F->f1;_tmp561=_tmp55F->f2;}}_LL264: {int _tmp71C=Cyc_Toc_in_lhs(
nv);Cyc_Toc_set_lhs(nv,0);{void*_tmp71D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp560->topt))->v);{void*_tmp71E=_tmp71D;struct Cyc_List_List*
_tmp720;struct Cyc_Absyn_PtrInfo _tmp722;void*_tmp723;struct Cyc_Absyn_Tqual _tmp724;
struct Cyc_Absyn_PtrAtts _tmp725;void*_tmp726;union Cyc_Absyn_Constraint*_tmp727;
union Cyc_Absyn_Constraint*_tmp728;union Cyc_Absyn_Constraint*_tmp729;_LL324: {
struct Cyc_Absyn_TupleType_struct*_tmp71F=(struct Cyc_Absyn_TupleType_struct*)
_tmp71E;if(_tmp71F->tag != 11)goto _LL326;else{_tmp720=_tmp71F->f1;}}_LL325: Cyc_Toc_exp_to_c(
nv,_tmp560);Cyc_Toc_exp_to_c(nv,_tmp561);{unsigned int _tmp72B;int _tmp72C;struct
_tuple10 _tmp72A=Cyc_Evexp_eval_const_uint_exp(_tmp561);_tmp72B=_tmp72A.f1;
_tmp72C=_tmp72A.f2;if(!_tmp72C){const char*_tmpD2E;void*_tmpD2D;(_tmpD2D=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD2E="unknown tuple subscript in translation to C",
_tag_dyneither(_tmpD2E,sizeof(char),44))),_tag_dyneither(_tmpD2D,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp560,Cyc_Absyn_fieldname((int)(_tmp72B + 1)));
goto _LL323;};_LL326: {struct Cyc_Absyn_PointerType_struct*_tmp721=(struct Cyc_Absyn_PointerType_struct*)
_tmp71E;if(_tmp721->tag != 5)goto _LL328;else{_tmp722=_tmp721->f1;_tmp723=_tmp722.elt_typ;
_tmp724=_tmp722.elt_tq;_tmp725=_tmp722.ptr_atts;_tmp726=_tmp725.rgn;_tmp727=
_tmp725.nullable;_tmp728=_tmp725.bounds;_tmp729=_tmp725.zero_term;}}_LL327: {
struct Cyc_List_List*_tmp72F=Cyc_Toc_get_relns(_tmp560);int in_bnds=0;{void*
_tmp730=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp728);_LL32B:;_LL32C: in_bnds=Cyc_Toc_check_bounds(_tmp71D,_tmp72F,_tmp560,
_tmp561);if(Cyc_Toc_warn_bounds_checks  && !in_bnds){const char*_tmpD32;void*
_tmpD31[1];struct Cyc_String_pa_struct _tmpD30;(_tmpD30.tag=0,((_tmpD30.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpD31[0]=&
_tmpD30,Cyc_Tcutil_warn(e->loc,((_tmpD32="bounds check necessary for %s",
_tag_dyneither(_tmpD32,sizeof(char),30))),_tag_dyneither(_tmpD31,sizeof(void*),1)))))));}
_LL32A:;}Cyc_Toc_exp_to_c(nv,_tmp560);Cyc_Toc_exp_to_c(nv,_tmp561);++ Cyc_Toc_total_bounds_checks;{
void*_tmp734=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp728);struct Cyc_Absyn_Exp*_tmp736;_LL32E: {struct Cyc_Absyn_Upper_b_struct*
_tmp735=(struct Cyc_Absyn_Upper_b_struct*)_tmp734;if(_tmp735->tag != 1)goto _LL330;
else{_tmp736=_tmp735->f1;}}_LL32F: {int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp727);void*ta1=Cyc_Toc_typ_to_c(_tmp723);void*ta2=
Cyc_Absyn_cstar_typ(ta1,_tmp724);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp729)){if(!Cyc_Evexp_c_can_eval(_tmp736)){const char*_tmpD35;void*_tmpD34;(
_tmpD34=0,Cyc_Tcutil_terr(e->loc,((_tmpD35="cannot determine subscript is in bounds",
_tag_dyneither(_tmpD35,sizeof(char),40))),_tag_dyneither(_tmpD34,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp560);struct Cyc_Absyn_Exp*_tmpD36[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpD36[2]=_tmp561,((_tmpD36[1]=_tmp736,((
_tmpD36[0]=_tmp560,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD36,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp736)){const char*_tmpD39;void*_tmpD38;(
_tmpD38=0,Cyc_Tcutil_terr(e->loc,((_tmpD39="cannot determine subscript is in bounds",
_tag_dyneither(_tmpD39,sizeof(char),40))),_tag_dyneither(_tmpD38,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){const char*_tmpD3C;void*_tmpD3B;(_tmpD3B=0,Cyc_Tcutil_warn(
e->loc,((_tmpD3C="inserted null check due to dereference",_tag_dyneither(_tmpD3C,
sizeof(char),39))),_tag_dyneither(_tmpD3B,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpD3D[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((
_tmpD3D[3]=_tmp561,((_tmpD3D[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD3D[1]=
_tmp736,((_tmpD3D[0]=_tmp560,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD3D,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{if(
!Cyc_Evexp_c_can_eval(_tmp736)){const char*_tmpD40;void*_tmpD3F;(_tmpD3F=0,Cyc_Tcutil_terr(
e->loc,((_tmpD40="cannot determine subscript is in bounds",_tag_dyneither(
_tmpD40,sizeof(char),40))),_tag_dyneither(_tmpD3F,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpD41[2];_tmp561->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((
_tmpD41[1]=Cyc_Absyn_copy_exp(_tmp561),((_tmpD41[0]=_tmp736,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD41,sizeof(struct Cyc_Absyn_Exp*),
2)))))));};}}}goto _LL32D;}_LL330: {struct Cyc_Absyn_DynEither_b_struct*_tmp737=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp734;if(_tmp737->tag != 0)goto _LL32D;}
_LL331: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp723);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp724),
Cyc_Absyn_aggrmember_exp(_tmp560,Cyc_Toc_curr_sp,0)),_tmp561);}else{struct Cyc_Absyn_Exp*
_tmp743=Cyc_Toc__check_dyneither_subscript_e;struct Cyc_Absyn_Exp*_tmpD42[3];e->r=
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp724),Cyc_Absyn_fncall_exp(
_tmp743,((_tmpD42[2]=_tmp561,((_tmpD42[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((
_tmpD42[0]=_tmp560,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD42,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL32D;}
_LL32D:;}goto _LL323;}_LL328:;_LL329: {const char*_tmpD45;void*_tmpD44;(_tmpD44=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD45="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(
_tmpD45,sizeof(char),49))),_tag_dyneither(_tmpD44,sizeof(void*),0)));}_LL323:;}
Cyc_Toc_set_lhs(nv,_tmp71C);goto _LL22C;};}_LL265: {struct Cyc_Absyn_Tuple_e_struct*
_tmp562=(struct Cyc_Absyn_Tuple_e_struct*)_tmp503;if(_tmp562->tag != 25)goto _LL267;
else{_tmp563=_tmp562->f1;}}_LL266: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(
nv,0,0,_tmp563))->r;else{struct Cyc_List_List*_tmp747=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_tup_to_c,_tmp563);void*_tmp748=Cyc_Toc_add_tuple_type(_tmp747);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp563 != 0;(_tmp563=_tmp563->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp563->hd);{struct _tuple15*_tmpD48;struct Cyc_List_List*
_tmpD47;dles=((_tmpD47=_cycalloc(sizeof(*_tmpD47)),((_tmpD47->hd=((_tmpD48=
_cycalloc(sizeof(*_tmpD48)),((_tmpD48->f1=0,((_tmpD48->f2=(struct Cyc_Absyn_Exp*)
_tmp563->hd,_tmpD48)))))),((_tmpD47->tl=dles,_tmpD47))))));};}}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);e->r=Cyc_Toc_unresolvedmem_exp_r(
0,dles);}goto _LL22C;_LL267: {struct Cyc_Absyn_Array_e_struct*_tmp564=(struct Cyc_Absyn_Array_e_struct*)
_tmp503;if(_tmp564->tag != 27)goto _LL269;else{_tmp565=_tmp564->f1;}}_LL268: e->r=
Cyc_Toc_unresolvedmem_exp_r(0,_tmp565);{struct Cyc_List_List*_tmp74B=_tmp565;for(
0;_tmp74B != 0;_tmp74B=_tmp74B->tl){struct _tuple15 _tmp74D;struct Cyc_Absyn_Exp*
_tmp74E;struct _tuple15*_tmp74C=(struct _tuple15*)_tmp74B->hd;_tmp74D=*_tmp74C;
_tmp74E=_tmp74D.f2;Cyc_Toc_exp_to_c(nv,_tmp74E);}}goto _LL22C;_LL269: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp566=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp503;if(_tmp566->tag != 28)
goto _LL26B;else{_tmp567=_tmp566->f1;_tmp568=_tmp566->f2;_tmp569=_tmp566->f3;
_tmp56A=_tmp566->f4;}}_LL26A: {unsigned int _tmp750;int _tmp751;struct _tuple10
_tmp74F=Cyc_Evexp_eval_const_uint_exp(_tmp568);_tmp750=_tmp74F.f1;_tmp751=
_tmp74F.f2;{void*_tmp752=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp569->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp569);{struct Cyc_List_List*
es=0;if(!Cyc_Toc_is_zero(_tmp569)){if(!_tmp751){const char*_tmpD4B;void*_tmpD4A;(
_tmpD4A=0,Cyc_Tcutil_terr(_tmp568->loc,((_tmpD4B="cannot determine value of constant",
_tag_dyneither(_tmpD4B,sizeof(char),35))),_tag_dyneither(_tmpD4A,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp750;++ i){struct _tuple15*_tmpD4E;struct Cyc_List_List*
_tmpD4D;es=((_tmpD4D=_cycalloc(sizeof(*_tmpD4D)),((_tmpD4D->hd=((_tmpD4E=
_cycalloc(sizeof(*_tmpD4E)),((_tmpD4E->f1=0,((_tmpD4E->f2=_tmp569,_tmpD4E)))))),((
_tmpD4D->tl=es,_tmpD4D))))));}}if(_tmp56A){struct Cyc_Absyn_Exp*_tmp757=Cyc_Toc_cast_it(
_tmp752,Cyc_Absyn_uint_exp(0,0));struct _tuple15*_tmpD51;struct Cyc_List_List*
_tmpD50;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,((_tmpD50=_cycalloc(sizeof(*_tmpD50)),((_tmpD50->hd=((
_tmpD51=_cycalloc(sizeof(*_tmpD51)),((_tmpD51->f1=0,((_tmpD51->f2=_tmp757,
_tmpD51)))))),((_tmpD50->tl=0,_tmpD50)))))));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL22C;};};}_LL26B: {struct Cyc_Absyn_Aggregate_e_struct*_tmp56B=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp503;if(_tmp56B->tag != 29)goto _LL26D;
else{_tmp56C=_tmp56B->f1;_tmp56D=_tmp56B->f2;_tmp56E=_tmp56B->f3;_tmp56F=_tmp56B->f4;}}
_LL26C: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp56D
!= 0,0,0,_tmp56E,_tmp56C))->r;else{if(_tmp56F == 0){const char*_tmpD54;void*
_tmpD53;(_tmpD53=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD54="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpD54,sizeof(
char),38))),_tag_dyneither(_tmpD53,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp56F;struct Cyc_Toc_Env _tmp75D;struct
_RegionHandle*_tmp75E;struct Cyc_Toc_Env*_tmp75C=nv;_tmp75D=*_tmp75C;_tmp75E=
_tmp75D.rgn;{struct Cyc_List_List*_tmp75F=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum 
Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp75E,e->loc,_tmp56E,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp56F->impl))->tagged){struct
_tuple21 _tmp761;struct Cyc_Absyn_Aggrfield*_tmp762;struct Cyc_Absyn_Exp*_tmp763;
struct _tuple21*_tmp760=(struct _tuple21*)((struct Cyc_List_List*)_check_null(
_tmp75F))->hd;_tmp761=*_tmp760;_tmp762=_tmp761.f1;_tmp763=_tmp761.f2;{void*
_tmp764=_tmp762->type;Cyc_Toc_exp_to_c(nv,_tmp763);if(Cyc_Toc_is_void_star_or_boxed_tvar(
_tmp764))_tmp763->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp763->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp56F,_tmp762->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple15*_tmpD59;struct _tuple15*_tmpD58;struct _tuple15*_tmpD57[2];
struct Cyc_List_List*_tmp765=(_tmpD57[1]=((_tmpD58=_cycalloc(sizeof(*_tmpD58)),((
_tmpD58->f1=0,((_tmpD58->f2=_tmp763,_tmpD58)))))),((_tmpD57[0]=((_tmpD59=
_cycalloc(sizeof(*_tmpD59)),((_tmpD59->f1=0,((_tmpD59->f2=field_tag_exp,_tmpD59)))))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD57,sizeof(struct _tuple15*),2)))));struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(
0,_tmp765,0);struct Cyc_Absyn_FieldName_struct*_tmpD5F;struct Cyc_Absyn_FieldName_struct
_tmpD5E;void*_tmpD5D[1];struct Cyc_List_List*ds=(_tmpD5D[0]=(void*)((_tmpD5F=
_cycalloc(sizeof(*_tmpD5F)),((_tmpD5F[0]=((_tmpD5E.tag=1,((_tmpD5E.f1=_tmp762->name,
_tmpD5E)))),_tmpD5F)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD5D,sizeof(void*),1)));struct _tuple15*_tmpD62;struct _tuple15*
_tmpD61[1];struct Cyc_List_List*dles=(_tmpD61[0]=((_tmpD62=_cycalloc(sizeof(*
_tmpD62)),((_tmpD62->f1=ds,((_tmpD62->f2=umem,_tmpD62)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD61,sizeof(struct
_tuple15*),1)));e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{struct Cyc_List_List*
_tmp76E=0;struct Cyc_List_List*_tmp76F=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp76F != 0;_tmp76F=_tmp76F->tl){struct Cyc_List_List*
_tmp770=_tmp75F;for(0;_tmp770 != 0;_tmp770=_tmp770->tl){if((*((struct _tuple21*)
_tmp770->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp76F->hd){struct _tuple21
_tmp772;struct Cyc_Absyn_Aggrfield*_tmp773;struct Cyc_Absyn_Exp*_tmp774;struct
_tuple21*_tmp771=(struct _tuple21*)_tmp770->hd;_tmp772=*_tmp771;_tmp773=_tmp772.f1;
_tmp774=_tmp772.f2;{void*_tmp775=_tmp773->type;Cyc_Toc_exp_to_c(nv,_tmp774);if(
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp775))_tmp774->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp774->r,0));{struct _tuple15*_tmpD65;struct Cyc_List_List*
_tmpD64;_tmp76E=((_tmpD64=_cycalloc(sizeof(*_tmpD64)),((_tmpD64->hd=((_tmpD65=
_cycalloc(sizeof(*_tmpD65)),((_tmpD65->f1=0,((_tmpD65->f2=_tmp774,_tmpD65)))))),((
_tmpD64->tl=_tmp76E,_tmpD64))))));}break;};}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp76E));}};};}
goto _LL22C;_LL26D: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp570=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp503;if(_tmp570->tag != 30)goto _LL26F;else{_tmp571=(void*)_tmp570->f1;_tmp572=
_tmp570->f2;}}_LL26E: {struct Cyc_List_List*fs;{void*_tmp778=Cyc_Tcutil_compress(
_tmp571);struct Cyc_List_List*_tmp77A;_LL333: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp779=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp778;if(_tmp779->tag != 13)goto
_LL335;else{_tmp77A=_tmp779->f2;}}_LL334: fs=_tmp77A;goto _LL332;_LL335:;_LL336: {
const char*_tmpD69;void*_tmpD68[1];struct Cyc_String_pa_struct _tmpD67;(_tmpD67.tag=
0,((_tmpD67.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp571)),((_tmpD68[0]=& _tmpD67,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD69="anon struct has type %s",
_tag_dyneither(_tmpD69,sizeof(char),24))),_tag_dyneither(_tmpD68,sizeof(void*),1)))))));}
_LL332:;}{struct Cyc_Toc_Env _tmp77F;struct _RegionHandle*_tmp780;struct Cyc_Toc_Env*
_tmp77E=nv;_tmp77F=*_tmp77E;_tmp780=_tmp77F.rgn;{struct Cyc_List_List*_tmp781=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp780,e->loc,_tmp572,Cyc_Absyn_StructA,fs);for(0;_tmp781 != 0;_tmp781=_tmp781->tl){
struct _tuple21 _tmp783;struct Cyc_Absyn_Aggrfield*_tmp784;struct Cyc_Absyn_Exp*
_tmp785;struct _tuple21*_tmp782=(struct _tuple21*)_tmp781->hd;_tmp783=*_tmp782;
_tmp784=_tmp783.f1;_tmp785=_tmp783.f2;{void*_tmp786=_tmp784->type;Cyc_Toc_exp_to_c(
nv,_tmp785);if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp786))_tmp785->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp785->r,0));};}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp572);}goto _LL22C;};}_LL26F: {struct Cyc_Absyn_Datatype_e_struct*_tmp573=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp503;if(_tmp573->tag != 31)goto _LL271;else{
_tmp574=_tmp573->f1;_tmp575=_tmp573->f2;_tmp576=_tmp573->f3;}}_LL270: {void*
datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple0*_tmp787=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp788=Cyc_Absyn_var_exp(_tmp787,0);struct Cyc_Absyn_Exp*
member_exp;{const char*_tmpD6A;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp576->name,((_tmpD6A="_struct",_tag_dyneither(_tmpD6A,sizeof(char),8)))));}
tag_exp=_tmp575->is_extensible?Cyc_Absyn_var_exp(_tmp576->name,0): Cyc_Toc_datatype_tag(
_tmp575,_tmp576->name);member_exp=_tmp788;{struct Cyc_List_List*_tmp78A=_tmp576->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp574 != 0;(
_tmp574=_tmp574->tl,_tmp78A=_tmp78A->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp574->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp78A))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_boxed_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct _tuple15*_tmpD6D;struct
Cyc_List_List*_tmpD6C;dles=((_tmpD6C=_cycalloc(sizeof(*_tmpD6C)),((_tmpD6C->hd=((
_tmpD6D=_cycalloc(sizeof(*_tmpD6D)),((_tmpD6D->f1=0,((_tmpD6D->f2=cur_e,_tmpD6D)))))),((
_tmpD6C->tl=dles,_tmpD6C))))));};}{struct _tuple15*_tmpD70;struct Cyc_List_List*
_tmpD6F;dles=((_tmpD6F=_cycalloc(sizeof(*_tmpD6F)),((_tmpD6F->hd=((_tmpD70=
_cycalloc(sizeof(*_tmpD70)),((_tmpD70->f1=0,((_tmpD70->f2=tag_exp,_tmpD70)))))),((
_tmpD6F->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles),_tmpD6F))))));}e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{struct Cyc_List_List*
_tmpD71;struct Cyc_List_List*_tmp78F=(_tmpD71=_cycalloc(sizeof(*_tmpD71)),((
_tmpD71->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,
0),tag_exp,0),((_tmpD71->tl=0,_tmpD71)))));{int i=1;for(0;_tmp574 != 0;(((_tmp574=
_tmp574->tl,i ++)),_tmp78A=_tmp78A->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp574->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp78A))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_boxed_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp790=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpD72;_tmp78F=((_tmpD72=_cycalloc(sizeof(*
_tmpD72)),((_tmpD72->hd=_tmp790,((_tmpD72->tl=_tmp78F,_tmpD72))))));};}}{struct
Cyc_Absyn_Stmt*_tmp792=Cyc_Absyn_exp_stmt(_tmp788,0);struct Cyc_List_List*_tmpD73;
struct Cyc_Absyn_Stmt*_tmp793=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpD73=_cycalloc(sizeof(*_tmpD73)),((
_tmpD73->hd=_tmp792,((_tmpD73->tl=_tmp78F,_tmpD73))))))),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp787,datatype_ctype,0,_tmp793,0));};}goto _LL22C;};}
_LL271: {struct Cyc_Absyn_Enum_e_struct*_tmp577=(struct Cyc_Absyn_Enum_e_struct*)
_tmp503;if(_tmp577->tag != 32)goto _LL273;}_LL272: goto _LL274;_LL273: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp578=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp503;if(_tmp578->tag != 33)goto
_LL275;}_LL274: goto _LL22C;_LL275: {struct Cyc_Absyn_Malloc_e_struct*_tmp579=(
struct Cyc_Absyn_Malloc_e_struct*)_tmp503;if(_tmp579->tag != 34)goto _LL277;else{
_tmp57A=_tmp579->f1;_tmp57B=_tmp57A.is_calloc;_tmp57C=_tmp57A.rgn;_tmp57D=
_tmp57A.elt_type;_tmp57E=_tmp57A.num_elts;_tmp57F=_tmp57A.fat_result;}}_LL276: {
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp57D)));Cyc_Toc_exp_to_c(nv,
_tmp57E);if(_tmp57F){struct _tuple0*_tmp796=Cyc_Toc_temp_var();struct _tuple0*
_tmp797=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp57B){xexp=_tmp57E;if(_tmp57C != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp57C;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp796,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp57D,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp796,0));}{struct Cyc_Absyn_Exp*_tmpD74[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpD74[2]=Cyc_Absyn_var_exp(_tmp796,0),((_tmpD74[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpD74[0]=Cyc_Absyn_var_exp(_tmp797,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD74,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{if(_tmp57C != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp57C;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp796,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp57D,Cyc_Absyn_var_exp(_tmp796,0));}{struct Cyc_Absyn_Exp*_tmpD75[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpD75[2]=Cyc_Absyn_var_exp(_tmp796,0),((_tmpD75[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpD75[0]=Cyc_Absyn_var_exp(_tmp797,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD75,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);};}{struct Cyc_Absyn_Stmt*_tmp79A=Cyc_Absyn_declare_stmt(_tmp796,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)_tmp57E,Cyc_Absyn_declare_stmt(_tmp797,Cyc_Absyn_cstar_typ(
t_c,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp79A);};}else{if(_tmp57B){if(_tmp57C != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp57C;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp57E))->r;}else{e->r=(
Cyc_Toc_calloc_exp(*_tmp57D,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp57E))->r;}}else{
if(_tmp57C != 0  && !Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp57C;Cyc_Toc_exp_to_c(nv,rgn);e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp57E))->r;}
else{e->r=(Cyc_Toc_malloc_exp(*_tmp57D,_tmp57E))->r;}}}goto _LL22C;}_LL277: {
struct Cyc_Absyn_Swap_e_struct*_tmp580=(struct Cyc_Absyn_Swap_e_struct*)_tmp503;
if(_tmp580->tag != 35)goto _LL279;else{_tmp581=_tmp580->f1;_tmp582=_tmp580->f2;}}
_LL278: {int is_dyneither_ptr=0;void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp581->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp582->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,&
is_dyneither_ptr)){const char*_tmpD78;void*_tmpD77;(_tmpD77=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD78="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpD78,sizeof(char),57))),_tag_dyneither(_tmpD77,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}{int f1_tag=0;void*tagged_mem_type1=(void*)&
Cyc_Absyn_VoidType_val;int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp581,&
f1_tag,& tagged_mem_type1,1);int f2_tag=0;void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp582,& f2_tag,& tagged_mem_type2,1);
Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp581);Cyc_Toc_exp_to_c(nv,_tmp582);
Cyc_Toc_set_lhs(nv,0);if(e1_tmem)Cyc_Toc_add_tagged_union_check_for_swap(_tmp581,
f1_tag,tagged_mem_type1);else{_tmp581=Cyc_Toc_push_address_exp(_tmp581);}if(
e2_tmem)Cyc_Toc_add_tagged_union_check_for_swap(_tmp582,f2_tag,tagged_mem_type2);
else{_tmp582=Cyc_Toc_push_address_exp(_tmp582);}{struct Cyc_Absyn_Exp*_tmpD79[2];
e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpD79[1]=_tmp582,((_tmpD79[0]=_tmp581,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD79,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL22C;};};}_LL279: {struct
Cyc_Absyn_Tagcheck_e_struct*_tmp583=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp503;
if(_tmp583->tag != 38)goto _LL27B;else{_tmp584=_tmp583->f1;_tmp585=_tmp583->f2;}}
_LL27A: {void*_tmp79E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp584->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp584);{void*_tmp79F=
_tmp79E;struct Cyc_Absyn_AggrInfo _tmp7A1;union Cyc_Absyn_AggrInfoU _tmp7A2;struct
Cyc_Absyn_Aggrdecl**_tmp7A3;struct Cyc_Absyn_Aggrdecl*_tmp7A4;_LL338: {struct Cyc_Absyn_AggrType_struct*
_tmp7A0=(struct Cyc_Absyn_AggrType_struct*)_tmp79F;if(_tmp7A0->tag != 12)goto
_LL33A;else{_tmp7A1=_tmp7A0->f1;_tmp7A2=_tmp7A1.aggr_info;if((_tmp7A2.KnownAggr).tag
!= 2)goto _LL33A;_tmp7A3=(struct Cyc_Absyn_Aggrdecl**)(_tmp7A2.KnownAggr).val;
_tmp7A4=*_tmp7A3;}}_LL339: {struct Cyc_Absyn_Exp*_tmp7A5=Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp7A4,_tmp585),0);struct Cyc_Absyn_Exp*_tmp7A6=Cyc_Absyn_aggrmember_exp(
_tmp584,_tmp585,0);struct Cyc_Absyn_Exp*_tmp7A7=Cyc_Absyn_aggrmember_exp(_tmp7A6,
Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(_tmp7A7,_tmp7A5,0))->r;goto _LL337;}
_LL33A:;_LL33B: {const char*_tmpD7C;void*_tmpD7B;(_tmpD7B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD7C="non-aggregate type in tagcheck",
_tag_dyneither(_tmpD7C,sizeof(char),31))),_tag_dyneither(_tmpD7B,sizeof(void*),0)));}
_LL337:;}goto _LL22C;}_LL27B: {struct Cyc_Absyn_StmtExp_e_struct*_tmp586=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp503;if(_tmp586->tag != 37)goto _LL27D;else{_tmp587=
_tmp586->f1;}}_LL27C: Cyc_Toc_stmt_to_c(nv,_tmp587);goto _LL22C;_LL27D: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp588=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp503;if(_tmp588->tag != 36)
goto _LL27F;}_LL27E: {const char*_tmpD7F;void*_tmpD7E;(_tmpD7E=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD7F="UnresolvedMem",
_tag_dyneither(_tmpD7F,sizeof(char),14))),_tag_dyneither(_tmpD7E,sizeof(void*),0)));}
_LL27F: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp589=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp503;if(_tmp589->tag != 26)goto _LL281;}_LL280: {const char*_tmpD82;void*_tmpD81;(
_tmpD81=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpD82="compoundlit",_tag_dyneither(_tmpD82,sizeof(char),12))),_tag_dyneither(
_tmpD81,sizeof(void*),0)));}_LL281: {struct Cyc_Absyn_Valueof_e_struct*_tmp58A=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp503;if(_tmp58A->tag != 39)goto _LL283;}
_LL282: {const char*_tmpD85;void*_tmpD84;(_tmpD84=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD85="valueof(-)",
_tag_dyneither(_tmpD85,sizeof(char),11))),_tag_dyneither(_tmpD84,sizeof(void*),0)));}
_LL283: {struct Cyc_Absyn_Asm_e_struct*_tmp58B=(struct Cyc_Absyn_Asm_e_struct*)
_tmp503;if(_tmp58B->tag != 40)goto _LL22C;}_LL284: {const char*_tmpD88;void*_tmpD87;(
_tmpD87=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD88="__asm__",_tag_dyneither(_tmpD88,sizeof(char),8))),_tag_dyneither(
_tmpD87,sizeof(void*),0)));}_LL22C:;};}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0);}struct _tuple22{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*
f3;};struct _tuple23{struct _tuple0*f1;void*f2;};struct _tuple24{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple22 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*
nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
struct Cyc_Absyn_Stmt*s;{void*_tmp7B2=p->r;struct Cyc_Absyn_Vardecl*_tmp7B4;struct
Cyc_Absyn_Vardecl _tmp7B5;struct _tuple0*_tmp7B6;struct Cyc_Absyn_Pat*_tmp7B7;
struct Cyc_Absyn_Vardecl*_tmp7B9;struct Cyc_Absyn_Vardecl _tmp7BA;struct _tuple0*
_tmp7BB;struct Cyc_Absyn_Vardecl*_tmp7BE;struct Cyc_Absyn_Pat*_tmp7BF;enum Cyc_Absyn_Sign
_tmp7C2;int _tmp7C3;char _tmp7C5;struct _dyneither_ptr _tmp7C7;struct Cyc_Absyn_Enumdecl*
_tmp7C9;struct Cyc_Absyn_Enumfield*_tmp7CA;void*_tmp7CC;struct Cyc_Absyn_Enumfield*
_tmp7CD;struct Cyc_Absyn_Pat*_tmp7CF;struct Cyc_Absyn_Pat _tmp7D0;void*_tmp7D1;
struct Cyc_Absyn_Datatypedecl*_tmp7D3;struct Cyc_Absyn_Datatypefield*_tmp7D4;
struct Cyc_List_List*_tmp7D5;struct Cyc_List_List*_tmp7D7;struct Cyc_List_List*
_tmp7D9;struct Cyc_Absyn_AggrInfo*_tmp7DB;struct Cyc_Absyn_AggrInfo*_tmp7DD;struct
Cyc_Absyn_AggrInfo _tmp7DE;union Cyc_Absyn_AggrInfoU _tmp7DF;struct Cyc_List_List*
_tmp7E0;struct Cyc_Absyn_Pat*_tmp7E2;_LL33D: {struct Cyc_Absyn_Var_p_struct*
_tmp7B3=(struct Cyc_Absyn_Var_p_struct*)_tmp7B2;if(_tmp7B3->tag != 1)goto _LL33F;
else{_tmp7B4=_tmp7B3->f1;_tmp7B5=*_tmp7B4;_tmp7B6=_tmp7B5.name;_tmp7B7=_tmp7B3->f2;}}
_LL33E: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp7B6,r),rgn,t,r,path,
_tmp7B7,fail_stmt,decls);_LL33F: {struct Cyc_Absyn_TagInt_p_struct*_tmp7B8=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp7B2;if(_tmp7B8->tag != 3)goto _LL341;else{
_tmp7B9=_tmp7B8->f2;_tmp7BA=*_tmp7B9;_tmp7BB=_tmp7BA.name;}}_LL340: nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp7BB,r);goto _LL342;_LL341: {struct Cyc_Absyn_Wild_p_struct*_tmp7BC=(
struct Cyc_Absyn_Wild_p_struct*)_tmp7B2;if(_tmp7BC->tag != 0)goto _LL343;}_LL342: s=
Cyc_Toc_skip_stmt_dl();goto _LL33C;_LL343: {struct Cyc_Absyn_Reference_p_struct*
_tmp7BD=(struct Cyc_Absyn_Reference_p_struct*)_tmp7B2;if(_tmp7BD->tag != 2)goto
_LL345;else{_tmp7BE=_tmp7BD->f1;_tmp7BF=_tmp7BD->f2;}}_LL344: {struct _tuple0*
_tmp7E6=Cyc_Toc_temp_var();{struct _tuple23*_tmpD8B;struct Cyc_List_List*_tmpD8A;
decls=((_tmpD8A=_region_malloc(rgn,sizeof(*_tmpD8A)),((_tmpD8A->hd=((_tmpD8B=
_region_malloc(rgn,sizeof(*_tmpD8B)),((_tmpD8B->f1=_tmp7E6,((_tmpD8B->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpD8B)))))),((_tmpD8A->tl=decls,_tmpD8A))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp7BE->name,Cyc_Absyn_var_exp(_tmp7E6,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp7E6,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Toc_push_address_exp(path)),0);{struct _tuple22 _tmp7E9=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp7BF,fail_stmt,decls);_tmp7E9.f3=Cyc_Absyn_seq_stmt(s,_tmp7E9.f3,
0);return _tmp7E9;};}_LL345: {struct Cyc_Absyn_Null_p_struct*_tmp7C0=(struct Cyc_Absyn_Null_p_struct*)
_tmp7B2;if(_tmp7C0->tag != 8)goto _LL347;}_LL346: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),fail_stmt);goto _LL33C;_LL347: {struct Cyc_Absyn_Int_p_struct*_tmp7C1=(struct
Cyc_Absyn_Int_p_struct*)_tmp7B2;if(_tmp7C1->tag != 9)goto _LL349;else{_tmp7C2=
_tmp7C1->f1;_tmp7C3=_tmp7C1->f2;}}_LL348: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp7C2,_tmp7C3,0),fail_stmt);goto _LL33C;_LL349: {struct Cyc_Absyn_Char_p_struct*
_tmp7C4=(struct Cyc_Absyn_Char_p_struct*)_tmp7B2;if(_tmp7C4->tag != 10)goto _LL34B;
else{_tmp7C5=_tmp7C4->f1;}}_LL34A: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(
_tmp7C5,0),fail_stmt);goto _LL33C;_LL34B: {struct Cyc_Absyn_Float_p_struct*_tmp7C6=(
struct Cyc_Absyn_Float_p_struct*)_tmp7B2;if(_tmp7C6->tag != 11)goto _LL34D;else{
_tmp7C7=_tmp7C6->f1;}}_LL34C: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp7C7,
0),fail_stmt);goto _LL33C;_LL34D: {struct Cyc_Absyn_Enum_p_struct*_tmp7C8=(struct
Cyc_Absyn_Enum_p_struct*)_tmp7B2;if(_tmp7C8->tag != 12)goto _LL34F;else{_tmp7C9=
_tmp7C8->f1;_tmp7CA=_tmp7C8->f2;}}_LL34E:{struct Cyc_Absyn_Enum_e_struct _tmpD8E;
struct Cyc_Absyn_Enum_e_struct*_tmpD8D;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD8D=_cycalloc(sizeof(*_tmpD8D)),((_tmpD8D[0]=((_tmpD8E.tag=32,((
_tmpD8E.f1=_tmp7CA->name,((_tmpD8E.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7C9,((
_tmpD8E.f3=(struct Cyc_Absyn_Enumfield*)_tmp7CA,_tmpD8E)))))))),_tmpD8D)))),0),
fail_stmt);}goto _LL33C;_LL34F: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp7CB=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp7B2;if(_tmp7CB->tag != 13)goto _LL351;else{
_tmp7CC=(void*)_tmp7CB->f1;_tmp7CD=_tmp7CB->f2;}}_LL350:{struct Cyc_Absyn_AnonEnum_e_struct
_tmpD91;struct Cyc_Absyn_AnonEnum_e_struct*_tmpD90;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD90=_cycalloc(sizeof(*_tmpD90)),((_tmpD90[0]=((_tmpD91.tag=33,((
_tmpD91.f1=_tmp7CD->name,((_tmpD91.f2=(void*)_tmp7CC,((_tmpD91.f3=(struct Cyc_Absyn_Enumfield*)
_tmp7CD,_tmpD91)))))))),_tmpD90)))),0),fail_stmt);}goto _LL33C;_LL351: {struct Cyc_Absyn_Pointer_p_struct*
_tmp7CE=(struct Cyc_Absyn_Pointer_p_struct*)_tmp7B2;if(_tmp7CE->tag != 5)goto
_LL353;else{_tmp7CF=_tmp7CE->f1;_tmp7D0=*_tmp7CF;_tmp7D1=_tmp7D0.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp7D2=(struct Cyc_Absyn_Datatype_p_struct*)_tmp7D1;if(_tmp7D2->tag != 7)goto
_LL353;else{_tmp7D3=_tmp7D2->f1;_tmp7D4=_tmp7D2->f2;_tmp7D5=_tmp7D2->f3;}};}}
_LL352: s=Cyc_Toc_skip_stmt_dl();{struct _tuple0*_tmp7EE=Cyc_Toc_temp_var();const
char*_tmpD92;struct _tuple0*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp7D4->name,((
_tmpD92="_struct",_tag_dyneither(_tmpD92,sizeof(char),8))));void*_tmp7EF=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);int cnt=1;struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
_tmp7EF,r);struct Cyc_List_List*_tmp7F0=_tmp7D4->typs;for(0;_tmp7D5 != 0;(((
_tmp7D5=_tmp7D5->tl,_tmp7F0=((struct Cyc_List_List*)_check_null(_tmp7F0))->tl)),
++ cnt)){struct Cyc_Absyn_Pat*_tmp7F1=(struct Cyc_Absyn_Pat*)_tmp7D5->hd;if(_tmp7F1->r
== (void*)& Cyc_Absyn_Wild_p_val)continue;{void*_tmp7F2=(*((struct _tuple9*)((
struct Cyc_List_List*)_check_null(_tmp7F0))->hd)).f2;struct _tuple0*_tmp7F3=Cyc_Toc_temp_var();
void*_tmp7F4=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp7F1->topt))->v;void*
_tmp7F5=Cyc_Toc_typ_to_c(_tmp7F4);struct Cyc_Absyn_Exp*_tmp7F6=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp7EE,0),Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_boxed_tvar(
Cyc_Toc_typ_to_c(_tmp7F2)))_tmp7F6=Cyc_Toc_cast_it(_tmp7F5,_tmp7F6);{struct
_tuple23*_tmpD95;struct Cyc_List_List*_tmpD94;decls=((_tmpD94=_region_malloc(rgn,
sizeof(*_tmpD94)),((_tmpD94->hd=((_tmpD95=_region_malloc(rgn,sizeof(*_tmpD95)),((
_tmpD95->f1=_tmp7F3,((_tmpD95->f2=_tmp7F5,_tmpD95)))))),((_tmpD94->tl=decls,
_tmpD94))))));}{struct _tuple22 _tmp7F9=Cyc_Toc_xlate_pat(nv,rgn,_tmp7F4,Cyc_Absyn_var_exp(
_tmp7F3,0),_tmp7F6,_tmp7F1,fail_stmt,decls);nv=_tmp7F9.f1;decls=_tmp7F9.f2;{
struct Cyc_Absyn_Stmt*_tmp7FA=_tmp7F9.f3;struct Cyc_Absyn_Stmt*_tmp7FB=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp7F3,0),_tmp7F6,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp7FB,_tmp7FA,0),0);};};};}{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp7EE,0),Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*tag_exp=
_tmp7D3->is_extensible?Cyc_Absyn_var_exp(_tmp7D4->name,0): Cyc_Toc_datatype_tag(
_tmp7D3,_tmp7D4->name);struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,
tag_exp,0);s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);if(Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v))s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp7EE,0),Cyc_Absyn_uint_exp(0,0),0),
fail_stmt,s,0);s=Cyc_Absyn_declare_stmt(_tmp7EE,_tmp7EF,(struct Cyc_Absyn_Exp*)
rcast,s,0);goto _LL33C;};};_LL353: {struct Cyc_Absyn_Datatype_p_struct*_tmp7D6=(
struct Cyc_Absyn_Datatype_p_struct*)_tmp7B2;if(_tmp7D6->tag != 7)goto _LL355;else{
_tmp7D7=_tmp7D6->f3;}}_LL354: _tmp7D9=_tmp7D7;goto _LL356;_LL355: {struct Cyc_Absyn_Tuple_p_struct*
_tmp7D8=(struct Cyc_Absyn_Tuple_p_struct*)_tmp7B2;if(_tmp7D8->tag != 4)goto _LL357;
else{_tmp7D9=_tmp7D8->f1;}}_LL356: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;
_tmp7D9 != 0;(_tmp7D9=_tmp7D9->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp7FD=(struct Cyc_Absyn_Pat*)
_tmp7D9->hd;if(_tmp7FD->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{struct _tuple0*
_tmp7FE=Cyc_Toc_temp_var();void*_tmp7FF=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7FD->topt))->v;{struct _tuple23*_tmpD98;struct Cyc_List_List*_tmpD97;decls=((
_tmpD97=_region_malloc(rgn,sizeof(*_tmpD97)),((_tmpD97->hd=((_tmpD98=
_region_malloc(rgn,sizeof(*_tmpD98)),((_tmpD98->f1=_tmp7FE,((_tmpD98->f2=Cyc_Toc_typ_to_c(
_tmp7FF),_tmpD98)))))),((_tmpD97->tl=decls,_tmpD97))))));}{struct _tuple22 _tmp802=
Cyc_Toc_xlate_pat(nv,rgn,_tmp7FF,Cyc_Absyn_var_exp(_tmp7FE,0),Cyc_Absyn_aggrmember_exp(
path,Cyc_Absyn_fieldname(cnt),0),_tmp7FD,fail_stmt,decls);nv=_tmp802.f1;decls=
_tmp802.f2;{struct Cyc_Absyn_Stmt*_tmp803=_tmp802.f3;struct Cyc_Absyn_Stmt*_tmp804=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7FE,0),Cyc_Absyn_aggrmember_exp(r,
Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp804,
_tmp803,0),0);};};};}goto _LL33C;};_LL357: {struct Cyc_Absyn_Aggr_p_struct*_tmp7DA=(
struct Cyc_Absyn_Aggr_p_struct*)_tmp7B2;if(_tmp7DA->tag != 6)goto _LL359;else{
_tmp7DB=_tmp7DA->f1;if(_tmp7DB != 0)goto _LL359;}}_LL358: {const char*_tmpD9B;void*
_tmpD9A;(_tmpD9A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD9B="unresolved aggregate pattern!",_tag_dyneither(_tmpD9B,sizeof(char),30))),
_tag_dyneither(_tmpD9A,sizeof(void*),0)));}_LL359: {struct Cyc_Absyn_Aggr_p_struct*
_tmp7DC=(struct Cyc_Absyn_Aggr_p_struct*)_tmp7B2;if(_tmp7DC->tag != 6)goto _LL35B;
else{_tmp7DD=_tmp7DC->f1;if(_tmp7DD == 0)goto _LL35B;_tmp7DE=*_tmp7DD;_tmp7DF=
_tmp7DE.aggr_info;_tmp7E0=_tmp7DC->f3;}}_LL35A: {struct Cyc_Absyn_Aggrdecl*
_tmp807=Cyc_Absyn_get_known_aggrdecl(_tmp7DF);if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp807->impl))->tagged){struct _tuple24 _tmp809;struct Cyc_List_List*
_tmp80A;struct Cyc_Absyn_Pat*_tmp80B;struct _tuple24*_tmp808=(struct _tuple24*)((
struct Cyc_List_List*)_check_null(_tmp7E0))->hd;_tmp809=*_tmp808;_tmp80A=_tmp809.f1;
_tmp80B=_tmp809.f2;{struct _dyneither_ptr*f;{void*_tmp80C=(void*)((struct Cyc_List_List*)
_check_null(_tmp80A))->hd;struct _dyneither_ptr*_tmp80E;_LL364: {struct Cyc_Absyn_FieldName_struct*
_tmp80D=(struct Cyc_Absyn_FieldName_struct*)_tmp80C;if(_tmp80D->tag != 1)goto
_LL366;else{_tmp80E=_tmp80D->f1;}}_LL365: f=_tmp80E;goto _LL363;_LL366:;_LL367: {
const char*_tmpD9E;void*_tmpD9D;(_tmpD9D=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD9E="no field name in tagged union pattern",
_tag_dyneither(_tmpD9E,sizeof(char),38))),_tag_dyneither(_tmpD9D,sizeof(void*),0)));}
_LL363:;}{struct _tuple0*_tmp811=Cyc_Toc_temp_var();void*_tmp812=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp80B->topt))->v;void*_tmp813=Cyc_Toc_typ_to_c(
_tmp812);{struct _tuple23*_tmpDA1;struct Cyc_List_List*_tmpDA0;decls=((_tmpDA0=
_region_malloc(rgn,sizeof(*_tmpDA0)),((_tmpDA0->hd=((_tmpDA1=_region_malloc(rgn,
sizeof(*_tmpDA1)),((_tmpDA1->f1=_tmp811,((_tmpDA1->f2=_tmp813,_tmpDA1)))))),((
_tmpDA0->tl=decls,_tmpDA0))))));}{struct Cyc_Absyn_Exp*_tmp816=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp817=
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp817=Cyc_Toc_cast_it(_tmp813,_tmp817);_tmp816=Cyc_Toc_cast_it(_tmp813,_tmp816);{
struct _tuple22 _tmp818=Cyc_Toc_xlate_pat(nv,rgn,_tmp812,Cyc_Absyn_var_exp(_tmp811,
0),_tmp816,_tmp80B,fail_stmt,decls);nv=_tmp818.f1;decls=_tmp818.f2;{struct Cyc_Absyn_Stmt*
_tmp819=_tmp818.f3;struct Cyc_Absyn_Stmt*_tmp81A=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp807,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp81B=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp811,0),_tmp817,0);s=Cyc_Absyn_seq_stmt(_tmp81A,Cyc_Absyn_seq_stmt(_tmp81B,
_tmp819,0),0);};};};};};}else{s=Cyc_Toc_skip_stmt_dl();for(0;_tmp7E0 != 0;_tmp7E0=
_tmp7E0->tl){struct _tuple24*_tmp81C=(struct _tuple24*)_tmp7E0->hd;struct Cyc_Absyn_Pat*
_tmp81D=(*_tmp81C).f2;if(_tmp81D->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _dyneither_ptr*f;{void*_tmp81E=(void*)((struct Cyc_List_List*)_check_null((*
_tmp81C).f1))->hd;struct _dyneither_ptr*_tmp820;_LL369: {struct Cyc_Absyn_FieldName_struct*
_tmp81F=(struct Cyc_Absyn_FieldName_struct*)_tmp81E;if(_tmp81F->tag != 1)goto
_LL36B;else{_tmp820=_tmp81F->f1;}}_LL36A: f=_tmp820;goto _LL368;_LL36B:;_LL36C: {
struct Cyc_Toc_Match_error_struct _tmpDA4;struct Cyc_Toc_Match_error_struct*_tmpDA3;(
int)_throw((void*)((_tmpDA3=_cycalloc_atomic(sizeof(*_tmpDA3)),((_tmpDA3[0]=((
_tmpDA4.tag=Cyc_Toc_Match_error,_tmpDA4)),_tmpDA3)))));}_LL368:;}{struct _tuple0*
_tmp823=Cyc_Toc_temp_var();void*_tmp824=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp81D->topt))->v;void*_tmp825=Cyc_Toc_typ_to_c(_tmp824);{struct _tuple23*
_tmpDA7;struct Cyc_List_List*_tmpDA6;decls=((_tmpDA6=_region_malloc(rgn,sizeof(*
_tmpDA6)),((_tmpDA6->hd=((_tmpDA7=_region_malloc(rgn,sizeof(*_tmpDA7)),((_tmpDA7->f1=
_tmp823,((_tmpDA7->f2=_tmp825,_tmpDA7)))))),((_tmpDA6->tl=decls,_tmpDA6))))));}{
struct _tuple22 _tmp828=Cyc_Toc_xlate_pat(nv,rgn,_tmp824,Cyc_Absyn_var_exp(_tmp823,
0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp81D,fail_stmt,decls);nv=_tmp828.f1;
decls=_tmp828.f2;{struct Cyc_Absyn_Exp*_tmp829=Cyc_Absyn_aggrmember_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(
Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp807->impl))->fields,
f)))->type))_tmp829=Cyc_Toc_cast_it(_tmp825,_tmp829);{struct Cyc_Absyn_Stmt*
_tmp82A=_tmp828.f3;struct Cyc_Absyn_Stmt*_tmp82B=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp823,0),_tmp829,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp82B,_tmp82A,
0),0);};};};};};}}goto _LL33C;}_LL35B: {struct Cyc_Absyn_Pointer_p_struct*_tmp7E1=(
struct Cyc_Absyn_Pointer_p_struct*)_tmp7B2;if(_tmp7E1->tag != 5)goto _LL35D;else{
_tmp7E2=_tmp7E1->f1;}}_LL35C: {struct _tuple0*_tmp82C=Cyc_Toc_temp_var();void*
_tmp82D=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp7E2->topt))->v;{struct
_tuple23*_tmpDAA;struct Cyc_List_List*_tmpDA9;decls=((_tmpDA9=_region_malloc(rgn,
sizeof(*_tmpDA9)),((_tmpDA9->hd=((_tmpDAA=_region_malloc(rgn,sizeof(*_tmpDAA)),((
_tmpDAA->f1=_tmp82C,((_tmpDAA->f2=Cyc_Toc_typ_to_c(_tmp82D),_tmpDAA)))))),((
_tmpDA9->tl=decls,_tmpDA9))))));}{struct _tuple22 _tmp830=Cyc_Toc_xlate_pat(nv,rgn,
_tmp82D,Cyc_Absyn_var_exp(_tmp82C,0),Cyc_Absyn_deref_exp(path,0),_tmp7E2,
fail_stmt,decls);nv=_tmp830.f1;decls=_tmp830.f2;{struct Cyc_Absyn_Stmt*_tmp831=
_tmp830.f3;struct Cyc_Absyn_Stmt*_tmp832=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp82C,0),Cyc_Absyn_deref_exp(r,0),0),_tmp831,0);if(Cyc_Toc_is_nullable(
t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(
0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),_tmp832,0);else{s=_tmp832;}goto
_LL33C;};};}_LL35D: {struct Cyc_Absyn_UnknownId_p_struct*_tmp7E3=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp7B2;if(_tmp7E3->tag != 14)goto _LL35F;}_LL35E: {const char*_tmpDAD;void*_tmpDAC;(
_tmpDAC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDAD="unknownid pat",_tag_dyneither(_tmpDAD,sizeof(char),14))),_tag_dyneither(
_tmpDAC,sizeof(void*),0)));}_LL35F: {struct Cyc_Absyn_UnknownCall_p_struct*
_tmp7E4=(struct Cyc_Absyn_UnknownCall_p_struct*)_tmp7B2;if(_tmp7E4->tag != 15)goto
_LL361;}_LL360: {const char*_tmpDB0;void*_tmpDAF;(_tmpDAF=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDB0="unknowncall pat",
_tag_dyneither(_tmpDB0,sizeof(char),16))),_tag_dyneither(_tmpDAF,sizeof(void*),0)));}
_LL361: {struct Cyc_Absyn_Exp_p_struct*_tmp7E5=(struct Cyc_Absyn_Exp_p_struct*)
_tmp7B2;if(_tmp7E5->tag != 16)goto _LL33C;}_LL362: {const char*_tmpDB3;void*_tmpDB2;(
_tmpDB2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDB3="exp pat",_tag_dyneither(_tmpDB3,sizeof(char),8))),_tag_dyneither(
_tmpDB2,sizeof(void*),0)));}_LL33C:;}{struct _tuple22 _tmpDB4;return(_tmpDB4.f1=nv,((
_tmpDB4.f2=decls,((_tmpDB4.f3=s,_tmpDB4)))));};}struct _tuple25{struct
_dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
static struct _tuple25*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){struct _tuple25*_tmpDB5;return(_tmpDB5=_region_malloc(r,sizeof(*_tmpDB5)),((
_tmpDB5->f1=Cyc_Toc_fresh_label(),((_tmpDB5->f2=Cyc_Toc_fresh_label(),((_tmpDB5->f3=
sc,_tmpDB5)))))));}static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{
void*_tmp83B=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int
leave_as_switch;{void*_tmp83C=Cyc_Tcutil_compress(_tmp83B);_LL36E: {struct Cyc_Absyn_IntType_struct*
_tmp83D=(struct Cyc_Absyn_IntType_struct*)_tmp83C;if(_tmp83D->tag != 6)goto _LL370;}
_LL36F: goto _LL371;_LL370: {struct Cyc_Absyn_EnumType_struct*_tmp83E=(struct Cyc_Absyn_EnumType_struct*)
_tmp83C;if(_tmp83E->tag != 14)goto _LL372;}_LL371: leave_as_switch=1;goto _LL36D;
_LL372:;_LL373: leave_as_switch=0;goto _LL36D;_LL36D:;}{struct Cyc_List_List*
_tmp83F=scs;for(0;_tmp83F != 0;_tmp83F=_tmp83F->tl){if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp83F->hd)->pat_vars))->v
!= 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp83F->hd)->where_clause != 0){
leave_as_switch=0;break;}}}if(leave_as_switch){struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp840=scs;for(0;_tmp840 != 0;_tmp840=_tmp840->tl){struct Cyc_Absyn_Stmt*
_tmp841=((struct Cyc_Absyn_Switch_clause*)_tmp840->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp840->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp841,0);next_l=Cyc_Toc_fresh_label();{
struct Cyc_Toc_Env _tmp843;struct _RegionHandle*_tmp844;struct Cyc_Toc_Env*_tmp842=
nv;_tmp843=*_tmp842;_tmp844=_tmp843.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
_tmp844,nv,next_l),_tmp841);};}}return;}{struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp846;
struct _RegionHandle*_tmp847;struct Cyc_Toc_Env*_tmp845=nv;_tmp846=*_tmp845;
_tmp847=_tmp846.rgn;{struct Cyc_Toc_Env*_tmp848=Cyc_Toc_share_env(_tmp847,nv);
struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple25*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp847,Cyc_Toc_gen_label,
_tmp847,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct
Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp849=lscs;for(0;_tmp849 != 0;
_tmp849=_tmp849->tl){struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple25*)
_tmp849->hd)).f3;struct _dyneither_ptr*fail_lab=_tmp849->tl == 0?end_l:(*((struct
_tuple25*)((struct Cyc_List_List*)_check_null(_tmp849->tl))->hd)).f1;struct Cyc_Toc_Env*
_tmp84B;struct Cyc_List_List*_tmp84C;struct Cyc_Absyn_Stmt*_tmp84D;struct _tuple22
_tmp84A=Cyc_Toc_xlate_pat(_tmp848,_tmp847,_tmp83B,r,path,sc->pattern,Cyc_Absyn_goto_stmt(
fail_lab,0),decls);_tmp84B=_tmp84A.f1;_tmp84C=_tmp84A.f2;_tmp84D=_tmp84A.f3;if(
sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp84E=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp84B,_tmp84E);_tmp84D=Cyc_Absyn_seq_stmt(
_tmp84D,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp84E,0),
Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp84C;{
struct Cyc_List_List*_tmpDB6;nvs=((_tmpDB6=_region_malloc(_tmp847,sizeof(*_tmpDB6)),((
_tmpDB6->hd=_tmp84B,((_tmpDB6->tl=nvs,_tmpDB6))))));}{struct Cyc_List_List*
_tmpDB7;test_stmts=((_tmpDB7=_region_malloc(_tmp847,sizeof(*_tmpDB7)),((_tmpDB7->hd=
_tmp84D,((_tmpDB7->tl=test_stmts,_tmpDB7))))));};}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple25 _tmp852;struct _dyneither_ptr*_tmp853;struct _dyneither_ptr*_tmp854;
struct Cyc_Absyn_Switch_clause*_tmp855;struct _tuple25*_tmp851=(struct _tuple25*)
lscs->hd;_tmp852=*_tmp851;_tmp853=_tmp852.f1;_tmp854=_tmp852.f2;_tmp855=_tmp852.f3;{
struct Cyc_Toc_Env*_tmp856=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp855->body;struct Cyc_Toc_Env _tmp858;struct
_RegionHandle*_tmp859;struct Cyc_Toc_Env*_tmp857=_tmp848;_tmp858=*_tmp857;_tmp859=
_tmp858.rgn;if(lscs->tl != 0){struct _tuple25 _tmp85B;struct _dyneither_ptr*_tmp85C;
struct Cyc_Absyn_Switch_clause*_tmp85D;struct _tuple25*_tmp85A=(struct _tuple25*)((
struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp85B=*_tmp85A;_tmp85C=_tmp85B.f2;
_tmp85D=_tmp85B.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp859,
_tmp856,end_l,_tmp85C,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp85D->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp859,
_tmp856,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp853,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp854,s,0),0);{struct Cyc_List_List*_tmpDB8;stmts=((_tmpDB8=_region_malloc(
_tmp847,sizeof(*_tmpDB8)),((_tmpDB8->hd=s,((_tmpDB8->tl=stmts,_tmpDB8))))));};};}{
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,
Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple23
_tmp860;struct _tuple0*_tmp861;void*_tmp862;struct _tuple23*_tmp85F=(struct
_tuple23*)decls->hd;_tmp860=*_tmp85F;_tmp861=_tmp860.f1;_tmp862=_tmp860.f2;res=
Cyc_Absyn_declare_stmt(_tmp861,_tmp862,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(
v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(
struct Cyc_Absyn_Exp*)e,res,0))->r;};};};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){struct Cyc_List_List*
_tmpDB9;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,((
_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9->hd=Cyc_Absyn_uint_exp((
unsigned int)(n - 1),0),((_tmpDB9->tl=0,_tmpDB9)))))),0),0);}void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s){while(1){void*_tmp864=s->r;struct Cyc_Absyn_Exp*_tmp867;
struct Cyc_Absyn_Stmt*_tmp869;struct Cyc_Absyn_Stmt*_tmp86A;struct Cyc_Absyn_Exp*
_tmp86C;struct Cyc_Absyn_Exp*_tmp86E;struct Cyc_Absyn_Stmt*_tmp86F;struct Cyc_Absyn_Stmt*
_tmp870;struct _tuple8 _tmp872;struct Cyc_Absyn_Exp*_tmp873;struct Cyc_Absyn_Stmt*
_tmp874;struct Cyc_Absyn_Stmt*_tmp876;struct Cyc_Absyn_Stmt*_tmp878;struct Cyc_Absyn_Stmt*
_tmp87A;struct Cyc_Absyn_Exp*_tmp87C;struct _tuple8 _tmp87D;struct Cyc_Absyn_Exp*
_tmp87E;struct _tuple8 _tmp87F;struct Cyc_Absyn_Exp*_tmp880;struct Cyc_Absyn_Stmt*
_tmp881;struct Cyc_Absyn_Exp*_tmp883;struct Cyc_List_List*_tmp884;struct Cyc_List_List*
_tmp886;struct Cyc_Absyn_Switch_clause**_tmp887;struct Cyc_Absyn_Decl*_tmp889;
struct Cyc_Absyn_Stmt*_tmp88A;struct _dyneither_ptr*_tmp88C;struct Cyc_Absyn_Stmt*
_tmp88D;struct Cyc_Absyn_Stmt*_tmp88F;struct _tuple8 _tmp890;struct Cyc_Absyn_Exp*
_tmp891;struct Cyc_Absyn_Stmt*_tmp893;struct Cyc_List_List*_tmp894;struct Cyc_Absyn_Exp*
_tmp896;_LL375: {struct Cyc_Absyn_Skip_s_struct*_tmp865=(struct Cyc_Absyn_Skip_s_struct*)
_tmp864;if(_tmp865->tag != 0)goto _LL377;}_LL376: return;_LL377: {struct Cyc_Absyn_Exp_s_struct*
_tmp866=(struct Cyc_Absyn_Exp_s_struct*)_tmp864;if(_tmp866->tag != 1)goto _LL379;
else{_tmp867=_tmp866->f1;}}_LL378: Cyc_Toc_exp_to_c(nv,_tmp867);return;_LL379: {
struct Cyc_Absyn_Seq_s_struct*_tmp868=(struct Cyc_Absyn_Seq_s_struct*)_tmp864;if(
_tmp868->tag != 2)goto _LL37B;else{_tmp869=_tmp868->f1;_tmp86A=_tmp868->f2;}}
_LL37A: Cyc_Toc_stmt_to_c(nv,_tmp869);s=_tmp86A;continue;_LL37B: {struct Cyc_Absyn_Return_s_struct*
_tmp86B=(struct Cyc_Absyn_Return_s_struct*)_tmp864;if(_tmp86B->tag != 3)goto _LL37D;
else{_tmp86C=_tmp86B->f1;}}_LL37C: {struct Cyc_Core_Opt*topt=0;if(_tmp86C != 0){{
struct Cyc_Core_Opt*_tmpDBA;topt=((_tmpDBA=_cycalloc(sizeof(*_tmpDBA)),((_tmpDBA->v=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp86C->topt))->v),
_tmpDBA))));}Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp86C);}if(s->try_depth
> 0){if(topt != 0){struct _tuple0*_tmp898=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*
_tmp899=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp898,0),
0);s->r=(Cyc_Absyn_declare_stmt(_tmp898,(void*)topt->v,_tmp86C,Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler(s->try_depth),_tmp899,0),0))->r;}else{Cyc_Toc_do_npop_before(
s->try_depth,s);}}return;}_LL37D: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp86D=(
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp864;if(_tmp86D->tag != 4)goto _LL37F;
else{_tmp86E=_tmp86D->f1;_tmp86F=_tmp86D->f2;_tmp870=_tmp86D->f3;}}_LL37E: Cyc_Toc_exp_to_c(
nv,_tmp86E);Cyc_Toc_stmt_to_c(nv,_tmp86F);s=_tmp870;continue;_LL37F: {struct Cyc_Absyn_While_s_struct*
_tmp871=(struct Cyc_Absyn_While_s_struct*)_tmp864;if(_tmp871->tag != 5)goto _LL381;
else{_tmp872=_tmp871->f1;_tmp873=_tmp872.f1;_tmp874=_tmp871->f2;}}_LL380: Cyc_Toc_exp_to_c(
nv,_tmp873);{struct Cyc_Toc_Env _tmp89B;struct _RegionHandle*_tmp89C;struct Cyc_Toc_Env*
_tmp89A=nv;_tmp89B=*_tmp89A;_tmp89C=_tmp89B.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp89C,nv),_tmp874);return;};_LL381: {struct Cyc_Absyn_Break_s_struct*_tmp875=(
struct Cyc_Absyn_Break_s_struct*)_tmp864;if(_tmp875->tag != 6)goto _LL383;else{
_tmp876=_tmp875->f1;}}_LL382: {struct Cyc_Toc_Env _tmp89E;struct _dyneither_ptr**
_tmp89F;struct Cyc_Toc_Env*_tmp89D=nv;_tmp89E=*_tmp89D;_tmp89F=_tmp89E.break_lab;
if(_tmp89F != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp89F,0);{int dest_depth=_tmp876 == 0?0:
_tmp876->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;};}
_LL383: {struct Cyc_Absyn_Continue_s_struct*_tmp877=(struct Cyc_Absyn_Continue_s_struct*)
_tmp864;if(_tmp877->tag != 7)goto _LL385;else{_tmp878=_tmp877->f1;}}_LL384: {
struct Cyc_Toc_Env _tmp8A1;struct _dyneither_ptr**_tmp8A2;struct Cyc_Toc_Env*_tmp8A0=
nv;_tmp8A1=*_tmp8A0;_tmp8A2=_tmp8A1.continue_lab;if(_tmp8A2 != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp8A2,0);_tmp87A=_tmp878;goto _LL386;}_LL385: {struct Cyc_Absyn_Goto_s_struct*
_tmp879=(struct Cyc_Absyn_Goto_s_struct*)_tmp864;if(_tmp879->tag != 8)goto _LL387;
else{_tmp87A=_tmp879->f2;}}_LL386: Cyc_Toc_do_npop_before(s->try_depth - ((struct
Cyc_Absyn_Stmt*)_check_null(_tmp87A))->try_depth,s);return;_LL387: {struct Cyc_Absyn_For_s_struct*
_tmp87B=(struct Cyc_Absyn_For_s_struct*)_tmp864;if(_tmp87B->tag != 9)goto _LL389;
else{_tmp87C=_tmp87B->f1;_tmp87D=_tmp87B->f2;_tmp87E=_tmp87D.f1;_tmp87F=_tmp87B->f3;
_tmp880=_tmp87F.f1;_tmp881=_tmp87B->f4;}}_LL388: Cyc_Toc_exp_to_c(nv,_tmp87C);Cyc_Toc_exp_to_c(
nv,_tmp87E);Cyc_Toc_exp_to_c(nv,_tmp880);{struct Cyc_Toc_Env _tmp8A4;struct
_RegionHandle*_tmp8A5;struct Cyc_Toc_Env*_tmp8A3=nv;_tmp8A4=*_tmp8A3;_tmp8A5=
_tmp8A4.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp8A5,nv),_tmp881);return;};
_LL389: {struct Cyc_Absyn_Switch_s_struct*_tmp882=(struct Cyc_Absyn_Switch_s_struct*)
_tmp864;if(_tmp882->tag != 10)goto _LL38B;else{_tmp883=_tmp882->f1;_tmp884=_tmp882->f2;}}
_LL38A: Cyc_Toc_xlate_switch(nv,s,_tmp883,_tmp884);return;_LL38B: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp885=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp864;if(_tmp885->tag != 11)goto
_LL38D;else{_tmp886=_tmp885->f1;_tmp887=_tmp885->f2;}}_LL38C: {struct Cyc_Toc_Env
_tmp8A7;struct Cyc_Toc_FallthruInfo*_tmp8A8;struct Cyc_Toc_Env*_tmp8A6=nv;_tmp8A7=*
_tmp8A6;_tmp8A8=_tmp8A7.fallthru_info;if(_tmp8A8 == 0){const char*_tmpDBD;void*
_tmpDBC;(_tmpDBC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDBD="fallthru in unexpected place",_tag_dyneither(_tmpDBD,sizeof(char),29))),
_tag_dyneither(_tmpDBC,sizeof(void*),0)));}{struct _dyneither_ptr*_tmp8AC;struct
Cyc_List_List*_tmp8AD;struct Cyc_Dict_Dict _tmp8AE;struct Cyc_Toc_FallthruInfo
_tmp8AB=*_tmp8A8;_tmp8AC=_tmp8AB.label;_tmp8AD=_tmp8AB.binders;_tmp8AE=_tmp8AB.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp8AC,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp887)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp8AF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp8AD);struct Cyc_List_List*_tmp8B0=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp886);for(0;_tmp8AF != 0;(_tmp8AF=_tmp8AF->tl,
_tmp8B0=_tmp8B0->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp8B0))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp8AE,(
struct _tuple0*)_tmp8AF->hd),(struct Cyc_Absyn_Exp*)_tmp8B0->hd,0),s2,0);}s->r=s2->r;
return;};};};}_LL38D: {struct Cyc_Absyn_Decl_s_struct*_tmp888=(struct Cyc_Absyn_Decl_s_struct*)
_tmp864;if(_tmp888->tag != 12)goto _LL38F;else{_tmp889=_tmp888->f1;_tmp88A=_tmp888->f2;}}
_LL38E:{void*_tmp8B1=_tmp889->r;struct Cyc_Absyn_Vardecl*_tmp8B3;struct Cyc_Absyn_Pat*
_tmp8B5;struct Cyc_Absyn_Exp*_tmp8B6;struct Cyc_List_List*_tmp8B8;struct Cyc_Absyn_Fndecl*
_tmp8BA;struct Cyc_Absyn_Tvar*_tmp8BC;struct Cyc_Absyn_Vardecl*_tmp8BD;int _tmp8BE;
struct Cyc_Absyn_Exp*_tmp8BF;struct Cyc_Absyn_Exp*_tmp8C1;struct Cyc_Absyn_Tvar*
_tmp8C2;struct Cyc_Absyn_Vardecl*_tmp8C3;_LL398: {struct Cyc_Absyn_Var_d_struct*
_tmp8B2=(struct Cyc_Absyn_Var_d_struct*)_tmp8B1;if(_tmp8B2->tag != 0)goto _LL39A;
else{_tmp8B3=_tmp8B2->f1;}}_LL399: {struct Cyc_Toc_Env _tmp8C5;struct _RegionHandle*
_tmp8C6;struct Cyc_Toc_Env*_tmp8C4=nv;_tmp8C5=*_tmp8C4;_tmp8C6=_tmp8C5.rgn;{
struct Cyc_Absyn_Local_b_struct _tmpDC0;struct Cyc_Absyn_Local_b_struct*_tmpDBF;
struct Cyc_Toc_Env*_tmp8C7=Cyc_Toc_add_varmap(_tmp8C6,nv,_tmp8B3->name,Cyc_Absyn_varb_exp(
_tmp8B3->name,(void*)((_tmpDBF=_cycalloc(sizeof(*_tmpDBF)),((_tmpDBF[0]=((
_tmpDC0.tag=4,((_tmpDC0.f1=_tmp8B3,_tmpDC0)))),_tmpDBF)))),0));Cyc_Toc_local_decl_to_c(
_tmp8C7,_tmp8C7,_tmp8B3,_tmp88A);}goto _LL397;}_LL39A: {struct Cyc_Absyn_Let_d_struct*
_tmp8B4=(struct Cyc_Absyn_Let_d_struct*)_tmp8B1;if(_tmp8B4->tag != 2)goto _LL39C;
else{_tmp8B5=_tmp8B4->f1;_tmp8B6=_tmp8B4->f3;}}_LL39B:{void*_tmp8CA=_tmp8B5->r;
struct Cyc_Absyn_Vardecl*_tmp8CC;struct Cyc_Absyn_Pat*_tmp8CD;struct Cyc_Absyn_Pat
_tmp8CE;void*_tmp8CF;_LL3A7: {struct Cyc_Absyn_Var_p_struct*_tmp8CB=(struct Cyc_Absyn_Var_p_struct*)
_tmp8CA;if(_tmp8CB->tag != 1)goto _LL3A9;else{_tmp8CC=_tmp8CB->f1;_tmp8CD=_tmp8CB->f2;
_tmp8CE=*_tmp8CD;_tmp8CF=_tmp8CE.r;{struct Cyc_Absyn_Wild_p_struct*_tmp8D0=(
struct Cyc_Absyn_Wild_p_struct*)_tmp8CF;if(_tmp8D0->tag != 0)goto _LL3A9;};}}_LL3A8: {
struct _tuple0*old_name=_tmp8CC->name;struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp8CC->name=new_name;_tmp8CC->initializer=(struct Cyc_Absyn_Exp*)_tmp8B6;{
struct Cyc_Absyn_Var_d_struct _tmpDC3;struct Cyc_Absyn_Var_d_struct*_tmpDC2;_tmp889->r=(
void*)((_tmpDC2=_cycalloc(sizeof(*_tmpDC2)),((_tmpDC2[0]=((_tmpDC3.tag=0,((
_tmpDC3.f1=_tmp8CC,_tmpDC3)))),_tmpDC2))));}{struct Cyc_Toc_Env _tmp8D4;struct
_RegionHandle*_tmp8D5;struct Cyc_Toc_Env*_tmp8D3=nv;_tmp8D4=*_tmp8D3;_tmp8D5=
_tmp8D4.rgn;{struct Cyc_Absyn_Local_b_struct _tmpDC6;struct Cyc_Absyn_Local_b_struct*
_tmpDC5;struct Cyc_Toc_Env*_tmp8D6=Cyc_Toc_add_varmap(_tmp8D5,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpDC5=_cycalloc(sizeof(*_tmpDC5)),((_tmpDC5[0]=((_tmpDC6.tag=
4,((_tmpDC6.f1=_tmp8CC,_tmpDC6)))),_tmpDC5)))),0));Cyc_Toc_local_decl_to_c(
_tmp8D6,nv,_tmp8CC,_tmp88A);}goto _LL3A6;};}_LL3A9:;_LL3AA: s->r=(Cyc_Toc_letdecl_to_c(
nv,_tmp8B5,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp8B6->topt))->v,_tmp8B6,
_tmp88A))->r;goto _LL3A6;_LL3A6:;}goto _LL397;_LL39C: {struct Cyc_Absyn_Letv_d_struct*
_tmp8B7=(struct Cyc_Absyn_Letv_d_struct*)_tmp8B1;if(_tmp8B7->tag != 3)goto _LL39E;
else{_tmp8B8=_tmp8B7->f1;}}_LL39D: {struct Cyc_List_List*_tmp8D9=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp8B8);if(_tmp8D9 == 0){const char*_tmpDC9;
void*_tmpDC8;(_tmpDC8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpDC9="empty Letv_d",_tag_dyneither(_tmpDC9,sizeof(char),
13))),_tag_dyneither(_tmpDC8,sizeof(void*),0)));}{struct Cyc_Absyn_Var_d_struct
_tmpDCC;struct Cyc_Absyn_Var_d_struct*_tmpDCB;_tmp889->r=(void*)((_tmpDCB=
_cycalloc(sizeof(*_tmpDCB)),((_tmpDCB[0]=((_tmpDCC.tag=0,((_tmpDCC.f1=(struct Cyc_Absyn_Vardecl*)
_tmp8D9->hd,_tmpDCC)))),_tmpDCB))));}_tmp8D9=_tmp8D9->tl;for(0;_tmp8D9 != 0;
_tmp8D9=_tmp8D9->tl){struct Cyc_Absyn_Var_d_struct _tmpDCF;struct Cyc_Absyn_Var_d_struct*
_tmpDCE;struct Cyc_Absyn_Decl*_tmp8DE=Cyc_Absyn_new_decl((void*)((_tmpDCE=
_cycalloc(sizeof(*_tmpDCE)),((_tmpDCE[0]=((_tmpDCF.tag=0,((_tmpDCF.f1=(struct Cyc_Absyn_Vardecl*)
_tmp8D9->hd,_tmpDCF)))),_tmpDCE)))),0);s->r=(Cyc_Absyn_decl_stmt(_tmp8DE,Cyc_Absyn_new_stmt(
s->r,0),0))->r;}Cyc_Toc_stmt_to_c(nv,s);goto _LL397;}_LL39E: {struct Cyc_Absyn_Fn_d_struct*
_tmp8B9=(struct Cyc_Absyn_Fn_d_struct*)_tmp8B1;if(_tmp8B9->tag != 1)goto _LL3A0;
else{_tmp8BA=_tmp8B9->f1;}}_LL39F: {struct _tuple0*_tmp8E1=_tmp8BA->name;struct
Cyc_Toc_Env _tmp8E3;struct _RegionHandle*_tmp8E4;struct Cyc_Toc_Env*_tmp8E2=nv;
_tmp8E3=*_tmp8E2;_tmp8E4=_tmp8E3.rgn;{struct Cyc_Toc_Env*_tmp8E5=Cyc_Toc_add_varmap(
_tmp8E4,nv,_tmp8BA->name,Cyc_Absyn_var_exp(_tmp8E1,0));Cyc_Toc_fndecl_to_c(
_tmp8E5,_tmp8BA,0);Cyc_Toc_stmt_to_c(_tmp8E5,_tmp88A);}goto _LL397;}_LL3A0: {
struct Cyc_Absyn_Region_d_struct*_tmp8BB=(struct Cyc_Absyn_Region_d_struct*)
_tmp8B1;if(_tmp8BB->tag != 4)goto _LL3A2;else{_tmp8BC=_tmp8BB->f1;_tmp8BD=_tmp8BB->f2;
_tmp8BE=_tmp8BB->f3;_tmp8BF=_tmp8BB->f4;}}_LL3A1: {struct Cyc_Absyn_Stmt*_tmp8E6=
_tmp88A;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple0*
rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp8BD->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp8E8;struct _RegionHandle*_tmp8E9;struct Cyc_Toc_Env*
_tmp8E7=nv;_tmp8E8=*_tmp8E7;_tmp8E9=_tmp8E8.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp8E9,nv,x_var,x_exp),_tmp8E6);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp8E6,0))->r;
else{if(_tmp8BF == 0){struct Cyc_Absyn_Exp*_tmpDD2[1];struct Cyc_Absyn_Exp*_tmpDD1[
1];struct Cyc_List_List*_tmpDD0;s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpDD0=
_cycalloc(sizeof(*_tmpDD0)),((_tmpDD0->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0),((_tmpDD0->tl=0,_tmpDD0)))))),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpDD1[0]=
x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDD1,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp8E6,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpDD2[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDD2,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp8BF);{struct Cyc_Absyn_Exp*
_tmpDD4[1];struct Cyc_Absyn_Exp*_tmpDD3[2];s->r=(Cyc_Absyn_declare_stmt(rh_var,
Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpDD3[1]=(struct Cyc_Absyn_Exp*)_tmp8BF,((_tmpDD3[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDD3,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp8E6,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpDD4[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDD4,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
return;}_LL3A2: {struct Cyc_Absyn_Alias_d_struct*_tmp8C0=(struct Cyc_Absyn_Alias_d_struct*)
_tmp8B1;if(_tmp8C0->tag != 5)goto _LL3A4;else{_tmp8C1=_tmp8C0->f1;_tmp8C2=_tmp8C0->f2;
_tmp8C3=_tmp8C0->f3;}}_LL3A3: {struct _tuple0*old_name=_tmp8C3->name;struct
_tuple0*new_name=Cyc_Toc_temp_var();_tmp8C3->name=new_name;_tmp8C3->initializer=(
struct Cyc_Absyn_Exp*)_tmp8C1;{struct Cyc_Absyn_Decl_s_struct _tmpDE3;struct Cyc_Absyn_Var_d_struct*
_tmpDE2;struct Cyc_Absyn_Var_d_struct _tmpDE1;struct Cyc_Absyn_Decl*_tmpDE0;struct
Cyc_Absyn_Decl_s_struct*_tmpDDF;s->r=(void*)((_tmpDDF=_cycalloc(sizeof(*_tmpDDF)),((
_tmpDDF[0]=((_tmpDE3.tag=12,((_tmpDE3.f1=((_tmpDE0=_cycalloc(sizeof(*_tmpDE0)),((
_tmpDE0->r=(void*)((_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2[0]=((_tmpDE1.tag=
0,((_tmpDE1.f1=_tmp8C3,_tmpDE1)))),_tmpDE2)))),((_tmpDE0->loc=0,_tmpDE0)))))),((
_tmpDE3.f2=_tmp88A,_tmpDE3)))))),_tmpDDF))));}{struct Cyc_Toc_Env _tmp8F5;struct
_RegionHandle*_tmp8F6;struct Cyc_Toc_Env*_tmp8F4=nv;_tmp8F5=*_tmp8F4;_tmp8F6=
_tmp8F5.rgn;{struct Cyc_Absyn_Local_b_struct _tmpDE6;struct Cyc_Absyn_Local_b_struct*
_tmpDE5;struct Cyc_Toc_Env*_tmp8F7=Cyc_Toc_add_varmap(_tmp8F6,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5[0]=((_tmpDE6.tag=
4,((_tmpDE6.f1=_tmp8C3,_tmpDE6)))),_tmpDE5)))),0));Cyc_Toc_local_decl_to_c(
_tmp8F7,nv,_tmp8C3,_tmp88A);}return;};}_LL3A4:;_LL3A5: {const char*_tmpDE9;void*
_tmpDE8;(_tmpDE8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpDE9="bad nested declaration within function",_tag_dyneither(_tmpDE9,sizeof(
char),39))),_tag_dyneither(_tmpDE8,sizeof(void*),0)));}_LL397:;}return;_LL38F: {
struct Cyc_Absyn_Label_s_struct*_tmp88B=(struct Cyc_Absyn_Label_s_struct*)_tmp864;
if(_tmp88B->tag != 13)goto _LL391;else{_tmp88C=_tmp88B->f1;_tmp88D=_tmp88B->f2;}}
_LL390: s=_tmp88D;continue;_LL391: {struct Cyc_Absyn_Do_s_struct*_tmp88E=(struct
Cyc_Absyn_Do_s_struct*)_tmp864;if(_tmp88E->tag != 14)goto _LL393;else{_tmp88F=
_tmp88E->f1;_tmp890=_tmp88E->f2;_tmp891=_tmp890.f1;}}_LL392: {struct Cyc_Toc_Env
_tmp8FD;struct _RegionHandle*_tmp8FE;struct Cyc_Toc_Env*_tmp8FC=nv;_tmp8FD=*
_tmp8FC;_tmp8FE=_tmp8FD.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp8FE,nv),
_tmp88F);Cyc_Toc_exp_to_c(nv,_tmp891);return;}_LL393: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp892=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp864;if(_tmp892->tag != 15)goto
_LL395;else{_tmp893=_tmp892->f1;_tmp894=_tmp892->f2;}}_LL394: {struct _tuple0*
h_var=Cyc_Toc_temp_var();struct _tuple0*e_var=Cyc_Toc_temp_var();struct _tuple0*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);{struct Cyc_Core_Opt*_tmpDEA;
e_exp->topt=((_tmpDEA=_cycalloc(sizeof(*_tmpDEA)),((_tmpDEA->v=e_typ,_tmpDEA))));}{
struct Cyc_Toc_Env _tmp901;struct _RegionHandle*_tmp902;struct Cyc_Toc_Env*_tmp900=
nv;_tmp901=*_tmp900;_tmp902=_tmp901.rgn;{struct Cyc_Toc_Env*_tmp903=Cyc_Toc_add_varmap(
_tmp902,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp903,_tmp893);{struct Cyc_Absyn_Stmt*
_tmp904=Cyc_Absyn_seq_stmt(_tmp893,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple0*_tmp905=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp906=
Cyc_Absyn_var_exp(_tmp905,0);struct Cyc_Absyn_Vardecl*_tmp907=Cyc_Absyn_new_vardecl(
_tmp905,Cyc_Absyn_exn_typ(),0);{struct Cyc_Core_Opt*_tmpDEB;_tmp906->topt=((
_tmpDEB=_cycalloc(sizeof(*_tmpDEB)),((_tmpDEB->v=Cyc_Absyn_exn_typ(),_tmpDEB))));}{
struct Cyc_Core_Opt*_tmpDFC;struct Cyc_Absyn_Var_p_struct*_tmpDFB;struct Cyc_Absyn_Pat*
_tmpDFA;struct Cyc_Core_Opt*_tmpDF9;struct Cyc_Absyn_Var_p_struct _tmpDF8;struct Cyc_Absyn_Pat*
_tmpDF7;struct Cyc_Absyn_Pat*_tmp909=(_tmpDF7=_cycalloc(sizeof(*_tmpDF7)),((
_tmpDF7->r=(void*)((_tmpDFB=_cycalloc(sizeof(*_tmpDFB)),((_tmpDFB[0]=((_tmpDF8.tag=
1,((_tmpDF8.f1=_tmp907,((_tmpDF8.f2=((_tmpDFA=_cycalloc(sizeof(*_tmpDFA)),((
_tmpDFA->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpDFA->topt=((_tmpDF9=_cycalloc(
sizeof(*_tmpDF9)),((_tmpDF9->v=Cyc_Absyn_exn_typ(),_tmpDF9)))),((_tmpDFA->loc=0,
_tmpDFA)))))))),_tmpDF8)))))),_tmpDFB)))),((_tmpDF7->topt=((_tmpDFC=_cycalloc(
sizeof(*_tmpDFC)),((_tmpDFC->v=Cyc_Absyn_exn_typ(),_tmpDFC)))),((_tmpDF7->loc=0,
_tmpDF7)))))));struct Cyc_Absyn_Exp*_tmp90A=Cyc_Absyn_throw_exp(_tmp906,0);{
struct Cyc_Core_Opt*_tmpDFD;_tmp90A->topt=((_tmpDFD=_cycalloc(sizeof(*_tmpDFD)),((
_tmpDFD->v=(void*)& Cyc_Absyn_VoidType_val,_tmpDFD))));}{struct Cyc_Absyn_Stmt*
_tmp90C=Cyc_Absyn_exp_stmt(_tmp90A,0);struct Cyc_Core_Opt*_tmpE03;struct Cyc_List_List*
_tmpE02;struct Cyc_Absyn_Switch_clause*_tmpE01;struct Cyc_Absyn_Switch_clause*
_tmp90D=(_tmpE01=_cycalloc(sizeof(*_tmpE01)),((_tmpE01->pattern=_tmp909,((
_tmpE01->pat_vars=((_tmpE03=_cycalloc(sizeof(*_tmpE03)),((_tmpE03->v=((_tmpE02=
_cycalloc(sizeof(*_tmpE02)),((_tmpE02->hd=_tmp907,((_tmpE02->tl=0,_tmpE02)))))),
_tmpE03)))),((_tmpE01->where_clause=0,((_tmpE01->body=_tmp90C,((_tmpE01->loc=0,
_tmpE01)))))))))));struct Cyc_List_List*_tmpE04;struct Cyc_Absyn_Stmt*_tmp90E=Cyc_Absyn_switch_stmt(
e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp894,((_tmpE04=_cycalloc(sizeof(*_tmpE04)),((_tmpE04->hd=_tmp90D,((_tmpE04->tl=
0,_tmpE04))))))),0);Cyc_Toc_stmt_to_c(_tmp903,_tmp90E);{struct Cyc_List_List*
_tmpE05;struct Cyc_Absyn_Exp*_tmp90F=Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,((
_tmpE05=_cycalloc(sizeof(*_tmpE05)),((_tmpE05->hd=Cyc_Absyn_aggrmember_exp(h_exp,
Cyc_Toc_handler_sp,0),((_tmpE05->tl=0,_tmpE05)))))),0);struct Cyc_List_List*
_tmpE06;struct Cyc_Absyn_Stmt*_tmp910=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpE06=_cycalloc(sizeof(*_tmpE06)),((_tmpE06->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpE06->tl=0,_tmpE06)))))),0),0);struct Cyc_Absyn_Exp*_tmp911=Cyc_Absyn_int_exp(
Cyc_Absyn_Signed,0,0);struct Cyc_Absyn_Exp*_tmp912=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
1,0);s->r=(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp910,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp911,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp90F,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp912,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp904,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp90E,0),0),0),0),0),0))->r;};};};};}
return;};}_LL395: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp895=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp864;if(_tmp895->tag != 16)goto _LL374;else{_tmp896=_tmp895->f1;}}_LL396: if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp896);{struct Cyc_List_List*
_tmpE07;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((
_tmpE07=_cycalloc(sizeof(*_tmpE07)),((_tmpE07->hd=_tmp896,((_tmpE07->tl=0,
_tmpE07)))))),0));};}return;_LL374:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp920=_new_region("frgn");struct _RegionHandle*frgn=& _tmp920;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp921=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp922=f->args;for(0;_tmp922 != 0;_tmp922=_tmp922->tl){struct
_tuple0*_tmpE08;struct _tuple0*_tmp923=(_tmpE08=_cycalloc(sizeof(*_tmpE08)),((
_tmpE08->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpE08->f2=(*((struct
_tuple26*)_tmp922->hd)).f1,_tmpE08)))));(*((struct _tuple26*)_tmp922->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple26*)_tmp922->hd)).f3);_tmp921=Cyc_Toc_add_varmap(frgn,_tmp921,
_tmp923,Cyc_Absyn_var_exp(_tmp923,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp926;struct Cyc_Absyn_Tqual
_tmp927;void*_tmp928;int _tmp929;struct Cyc_Absyn_VarargInfo _tmp925=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp926=_tmp925.name;_tmp927=_tmp925.tq;_tmp928=
_tmp925.type;_tmp929=_tmp925.inject;{void*_tmp92A=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp928,(void*)& Cyc_Absyn_HeapRgn_val,_tmp927,Cyc_Absyn_false_conref));struct
_tuple0*_tmpE09;struct _tuple0*_tmp92B=(_tmpE09=_cycalloc(sizeof(*_tmpE09)),((
_tmpE09->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpE09->f2=(struct
_dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null(_tmp926))->v,_tmpE09)))));{
struct _tuple26*_tmpE0C;struct Cyc_List_List*_tmpE0B;f->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpE0B=
_cycalloc(sizeof(*_tmpE0B)),((_tmpE0B->hd=((_tmpE0C=_cycalloc(sizeof(*_tmpE0C)),((
_tmpE0C->f1=(struct _dyneither_ptr*)_tmp926->v,((_tmpE0C->f2=_tmp927,((_tmpE0C->f3=
_tmp92A,_tmpE0C)))))))),((_tmpE0B->tl=0,_tmpE0B)))))));}_tmp921=Cyc_Toc_add_varmap(
frgn,_tmp921,_tmp92B,Cyc_Absyn_var_exp(_tmp92B,0));f->cyc_varargs=0;};}{struct
Cyc_List_List*_tmp92F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp92F != 0;_tmp92F=_tmp92F->tl){((struct Cyc_Absyn_Vardecl*)_tmp92F->hd)->type=
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp92F->hd)->type);}}Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel(frgn,_tmp921),f->body);};_pop_region(frgn);};}static enum 
Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){switch(s){case Cyc_Absyn_Abstract:
_LL3AB: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3AC: return Cyc_Absyn_Extern;
default: _LL3AD: return s;}}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad,int add_to_decls){struct _tuple0*_tmp930=ad->name;struct _DynRegionHandle*
_tmp932;struct Cyc_Dict_Dict*_tmp933;struct Cyc_Toc_TocState _tmp931=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp932=_tmp931.dyn;_tmp933=_tmp931.aggrs_so_far;{
struct _DynRegionFrame _tmp934;struct _RegionHandle*d=_open_dynregion(& _tmp934,
_tmp932);{int seen_defn_before;struct _tuple14**_tmp935=((struct _tuple14**(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp933,_tmp930);if(
_tmp935 == 0){seen_defn_before=0;{struct _tuple14*v;if(ad->kind == Cyc_Absyn_StructA){
struct _tuple14*_tmpE0D;v=((_tmpE0D=_region_malloc(d,sizeof(*_tmpE0D)),((_tmpE0D->f1=
ad,((_tmpE0D->f2=Cyc_Absyn_strctq(_tmp930),_tmpE0D))))));}else{struct _tuple14*
_tmpE0E;v=((_tmpE0E=_region_malloc(d,sizeof(*_tmpE0E)),((_tmpE0E->f1=ad,((
_tmpE0E->f2=Cyc_Absyn_unionq_typ(_tmp930),_tmpE0E))))));}*_tmp933=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp933,
_tmp930,v);};}else{struct _tuple14 _tmp939;struct Cyc_Absyn_Aggrdecl*_tmp93A;void*
_tmp93B;struct _tuple14*_tmp938=*_tmp935;_tmp939=*_tmp938;_tmp93A=_tmp939.f1;
_tmp93B=_tmp939.f2;if(_tmp93A->impl == 0){{struct _tuple14*_tmpE0F;*_tmp933=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))
Cyc_Dict_insert)(*_tmp933,_tmp930,((_tmpE0F=_region_malloc(d,sizeof(*_tmpE0F)),((
_tmpE0F->f1=ad,((_tmpE0F->f2=_tmp93B,_tmpE0F)))))));}seen_defn_before=0;}else{
seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*_tmpE10;struct Cyc_Absyn_Aggrdecl*
new_ad=(_tmpE10=_cycalloc(sizeof(*_tmpE10)),((_tmpE10->kind=ad->kind,((_tmpE10->sc=
Cyc_Absyn_Public,((_tmpE10->name=ad->name,((_tmpE10->tvs=0,((_tmpE10->impl=0,((
_tmpE10->attributes=ad->attributes,_tmpE10)))))))))))));if(ad->impl != 0  && !
seen_defn_before){{struct Cyc_Absyn_AggrdeclImpl*_tmpE11;new_ad->impl=((_tmpE11=
_cycalloc(sizeof(*_tmpE11)),((_tmpE11->exist_vars=0,((_tmpE11->rgn_po=0,((
_tmpE11->fields=0,((_tmpE11->tagged=0,_tmpE11))))))))));}{struct Cyc_List_List*
new_fields=0;{struct Cyc_List_List*_tmp93E=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp93E != 0;_tmp93E=_tmp93E->tl){struct Cyc_Absyn_Aggrfield*
_tmp93F=(struct Cyc_Absyn_Aggrfield*)_tmp93E->hd;if(_tmp93E->tl == 0  && Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp93F->type)))continue;{struct Cyc_Absyn_Aggrfield*
_tmpE12;struct Cyc_Absyn_Aggrfield*_tmp940=(_tmpE12=_cycalloc(sizeof(*_tmpE12)),((
_tmpE12->name=_tmp93F->name,((_tmpE12->tq=Cyc_Toc_mt_tq,((_tmpE12->type=Cyc_Toc_typ_to_c_array(
_tmp93F->type),((_tmpE12->width=_tmp93F->width,((_tmpE12->attributes=_tmp93F->attributes,
_tmpE12)))))))))));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp941=_tmp940->type;struct _dyneither_ptr*_tmp942=_tmp940->name;const char*
_tmpE17;void*_tmpE16[2];struct Cyc_String_pa_struct _tmpE15;struct Cyc_String_pa_struct
_tmpE14;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpE14.tag=0,((_tmpE14.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp942),((_tmpE15.tag=0,((_tmpE15.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpE16[0]=&
_tmpE15,((_tmpE16[1]=& _tmpE14,Cyc_aprintf(((_tmpE17="_union_%s_%s",
_tag_dyneither(_tmpE17,sizeof(char),13))),_tag_dyneither(_tmpE16,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpE18;struct _dyneither_ptr*str=(_tmpE18=_cycalloc(sizeof(*
_tmpE18)),((_tmpE18[0]=s,_tmpE18)));struct Cyc_Absyn_Aggrfield*_tmpE19;struct Cyc_Absyn_Aggrfield*
_tmp943=(_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19->name=Cyc_Toc_val_sp,((
_tmpE19->tq=Cyc_Toc_mt_tq,((_tmpE19->type=_tmp941,((_tmpE19->width=0,((_tmpE19->attributes=
0,_tmpE19)))))))))));struct Cyc_Absyn_Aggrfield*_tmpE1A;struct Cyc_Absyn_Aggrfield*
_tmp944=(_tmpE1A=_cycalloc(sizeof(*_tmpE1A)),((_tmpE1A->name=Cyc_Toc_tag_sp,((
_tmpE1A->tq=Cyc_Toc_mt_tq,((_tmpE1A->type=Cyc_Absyn_sint_typ,((_tmpE1A->width=0,((
_tmpE1A->attributes=0,_tmpE1A)))))))))));struct Cyc_Absyn_Aggrfield*_tmpE1B[2];
struct Cyc_List_List*_tmp945=(_tmpE1B[1]=_tmp943,((_tmpE1B[0]=_tmp944,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpE1B,sizeof(struct Cyc_Absyn_Aggrfield*),
2)))));struct Cyc_Absyn_AggrdeclImpl*_tmpE20;struct _tuple0*_tmpE1F;struct Cyc_Absyn_Aggrdecl*
_tmpE1E;struct Cyc_Absyn_Aggrdecl*_tmp946=(_tmpE1E=_cycalloc(sizeof(*_tmpE1E)),((
_tmpE1E->kind=Cyc_Absyn_StructA,((_tmpE1E->sc=Cyc_Absyn_Public,((_tmpE1E->name=((
_tmpE1F=_cycalloc(sizeof(*_tmpE1F)),((_tmpE1F->f1=Cyc_Absyn_Loc_n,((_tmpE1F->f2=
str,_tmpE1F)))))),((_tmpE1E->tvs=0,((_tmpE1E->impl=((_tmpE20=_cycalloc(sizeof(*
_tmpE20)),((_tmpE20->exist_vars=0,((_tmpE20->rgn_po=0,((_tmpE20->fields=_tmp945,((
_tmpE20->tagged=0,_tmpE20)))))))))),((_tmpE1E->attributes=0,_tmpE1E)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmpE26;struct Cyc_Absyn_Aggr_d_struct _tmpE25;
struct Cyc_List_List*_tmpE24;Cyc_Toc_result_decls=((_tmpE24=_cycalloc(sizeof(*
_tmpE24)),((_tmpE24->hd=Cyc_Absyn_new_decl((void*)((_tmpE26=_cycalloc(sizeof(*
_tmpE26)),((_tmpE26[0]=((_tmpE25.tag=6,((_tmpE25.f1=_tmp946,_tmpE25)))),_tmpE26)))),
0),((_tmpE24->tl=Cyc_Toc_result_decls,_tmpE24))))));}_tmp940->type=Cyc_Absyn_strct(
str);}{struct Cyc_List_List*_tmpE27;new_fields=((_tmpE27=_cycalloc(sizeof(*
_tmpE27)),((_tmpE27->hd=_tmp940,((_tmpE27->tl=new_fields,_tmpE27))))));};};}}(
new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_fields);};}if(add_to_decls){struct Cyc_Absyn_Decl*_tmpE31;struct Cyc_Absyn_Aggr_d_struct
_tmpE30;struct Cyc_Absyn_Aggr_d_struct*_tmpE2F;struct Cyc_List_List*_tmpE2E;Cyc_Toc_result_decls=((
_tmpE2E=_cycalloc(sizeof(*_tmpE2E)),((_tmpE2E->hd=((_tmpE31=_cycalloc(sizeof(*
_tmpE31)),((_tmpE31->r=(void*)((_tmpE2F=_cycalloc(sizeof(*_tmpE2F)),((_tmpE2F[0]=((
_tmpE30.tag=6,((_tmpE30.f1=new_ad,_tmpE30)))),_tmpE2F)))),((_tmpE31->loc=0,
_tmpE31)))))),((_tmpE2E->tl=Cyc_Toc_result_decls,_tmpE2E))))));}};};
_pop_dynregion(d);};}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud){struct _DynRegionHandle*_tmp95D;struct Cyc_Set_Set**_tmp95E;struct Cyc_Toc_TocState
_tmp95C=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp95D=
_tmp95C.dyn;_tmp95E=_tmp95C.datatypes_so_far;{struct _DynRegionFrame _tmp95F;
struct _RegionHandle*d=_open_dynregion(& _tmp95F,_tmp95D);{struct _tuple0*_tmp960=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))
Cyc_Set_member)(*_tmp95E,_tmp960)){_npop_handler(0);return;}*_tmp95E=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*
_tmp95E,_tmp960);}{struct Cyc_List_List*_tmp961=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp961 != 0;_tmp961=_tmp961->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp961->hd;struct Cyc_List_List*_tmp962=0;int i=
1;{struct Cyc_List_List*_tmp963=f->typs;for(0;_tmp963 != 0;(_tmp963=_tmp963->tl,i
++)){struct _dyneither_ptr*_tmp964=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpE32;struct Cyc_Absyn_Aggrfield*_tmp965=(_tmpE32=_cycalloc(sizeof(*_tmpE32)),((
_tmpE32->name=_tmp964,((_tmpE32->tq=(*((struct _tuple9*)_tmp963->hd)).f1,((
_tmpE32->type=Cyc_Toc_typ_to_c_array((*((struct _tuple9*)_tmp963->hd)).f2),((
_tmpE32->width=0,((_tmpE32->attributes=0,_tmpE32)))))))))));struct Cyc_List_List*
_tmpE33;_tmp962=((_tmpE33=_cycalloc(sizeof(*_tmpE33)),((_tmpE33->hd=_tmp965,((
_tmpE33->tl=_tmp962,_tmpE33))))));}}{struct Cyc_Absyn_Aggrfield*_tmpE36;struct Cyc_List_List*
_tmpE35;_tmp962=((_tmpE35=_cycalloc(sizeof(*_tmpE35)),((_tmpE35->hd=((_tmpE36=
_cycalloc(sizeof(*_tmpE36)),((_tmpE36->name=Cyc_Toc_tag_sp,((_tmpE36->tq=Cyc_Toc_mt_tq,((
_tmpE36->type=Cyc_Absyn_sint_typ,((_tmpE36->width=0,((_tmpE36->attributes=0,
_tmpE36)))))))))))),((_tmpE35->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp962),_tmpE35))))));}{struct Cyc_Absyn_AggrdeclImpl*
_tmpE3B;const char*_tmpE3A;struct Cyc_Absyn_Aggrdecl*_tmpE39;struct Cyc_Absyn_Aggrdecl*
_tmp96A=(_tmpE39=_cycalloc(sizeof(*_tmpE39)),((_tmpE39->kind=Cyc_Absyn_StructA,((
_tmpE39->sc=Cyc_Absyn_Public,((_tmpE39->name=Cyc_Toc_collapse_qvar_tag(f->name,((
_tmpE3A="_struct",_tag_dyneither(_tmpE3A,sizeof(char),8)))),((_tmpE39->tvs=0,((
_tmpE39->impl=((_tmpE3B=_cycalloc(sizeof(*_tmpE3B)),((_tmpE3B->exist_vars=0,((
_tmpE3B->rgn_po=0,((_tmpE3B->fields=_tmp962,((_tmpE3B->tagged=0,_tmpE3B)))))))))),((
_tmpE39->attributes=0,_tmpE39)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpE41;
struct Cyc_Absyn_Aggr_d_struct _tmpE40;struct Cyc_List_List*_tmpE3F;Cyc_Toc_result_decls=((
_tmpE3F=_cycalloc(sizeof(*_tmpE3F)),((_tmpE3F->hd=Cyc_Absyn_new_decl((void*)((
_tmpE41=_cycalloc(sizeof(*_tmpE41)),((_tmpE41[0]=((_tmpE40.tag=6,((_tmpE40.f1=
_tmp96A,_tmpE40)))),_tmpE41)))),0),((_tmpE3F->tl=Cyc_Toc_result_decls,_tmpE3F))))));};}};;
_pop_dynregion(d);};}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd){if(xd->fields == 0)return;{struct _DynRegionHandle*_tmp972;struct Cyc_Dict_Dict*
_tmp973;struct Cyc_Toc_TocState _tmp971=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp972=_tmp971.dyn;_tmp973=_tmp971.xdatatypes_so_far;{struct _DynRegionFrame
_tmp974;struct _RegionHandle*d=_open_dynregion(& _tmp974,_tmp972);{struct _tuple0*
_tmp975=xd->name;struct Cyc_List_List*_tmp976=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp976 != 0;_tmp976=_tmp976->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp976->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp977=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp978=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp977,Cyc_Absyn_false_conref,0);int*_tmp979=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp973,f->name);int
_tmp97A;_LL3B0: if(_tmp979 != 0)goto _LL3B2;_LL3B1: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp97B=Cyc_Absyn_new_vardecl(f->name,_tmp978,
initopt);_tmp97B->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*_tmpE47;struct Cyc_Absyn_Var_d_struct
_tmpE46;struct Cyc_List_List*_tmpE45;Cyc_Toc_result_decls=((_tmpE45=_cycalloc(
sizeof(*_tmpE45)),((_tmpE45->hd=Cyc_Absyn_new_decl((void*)((_tmpE47=_cycalloc(
sizeof(*_tmpE47)),((_tmpE47[0]=((_tmpE46.tag=0,((_tmpE46.f1=_tmp97B,_tmpE46)))),
_tmpE47)))),0),((_tmpE45->tl=Cyc_Toc_result_decls,_tmpE45))))));}*_tmp973=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp973,f->name,f->sc != Cyc_Absyn_Extern);{struct Cyc_List_List*fields=0;int i=1;{
struct Cyc_List_List*_tmp97F=f->typs;for(0;_tmp97F != 0;(_tmp97F=_tmp97F->tl,i ++)){
struct Cyc_Int_pa_struct _tmpE4F;void*_tmpE4E[1];const char*_tmpE4D;struct
_dyneither_ptr*_tmpE4C;struct _dyneither_ptr*_tmp980=(_tmpE4C=_cycalloc(sizeof(*
_tmpE4C)),((_tmpE4C[0]=(struct _dyneither_ptr)((_tmpE4F.tag=1,((_tmpE4F.f1=(
unsigned long)i,((_tmpE4E[0]=& _tmpE4F,Cyc_aprintf(((_tmpE4D="f%d",_tag_dyneither(
_tmpE4D,sizeof(char),4))),_tag_dyneither(_tmpE4E,sizeof(void*),1)))))))),_tmpE4C)));
struct Cyc_Absyn_Aggrfield*_tmpE50;struct Cyc_Absyn_Aggrfield*_tmp981=(_tmpE50=
_cycalloc(sizeof(*_tmpE50)),((_tmpE50->name=_tmp980,((_tmpE50->tq=(*((struct
_tuple9*)_tmp97F->hd)).f1,((_tmpE50->type=Cyc_Toc_typ_to_c_array((*((struct
_tuple9*)_tmp97F->hd)).f2),((_tmpE50->width=0,((_tmpE50->attributes=0,_tmpE50)))))))))));
struct Cyc_List_List*_tmpE51;fields=((_tmpE51=_cycalloc(sizeof(*_tmpE51)),((
_tmpE51->hd=_tmp981,((_tmpE51->tl=fields,_tmpE51))))));}}{struct Cyc_Absyn_Aggrfield*
_tmpE54;struct Cyc_List_List*_tmpE53;fields=((_tmpE53=_cycalloc(sizeof(*_tmpE53)),((
_tmpE53->hd=((_tmpE54=_cycalloc(sizeof(*_tmpE54)),((_tmpE54->name=Cyc_Toc_tag_sp,((
_tmpE54->tq=Cyc_Toc_mt_tq,((_tmpE54->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq),((_tmpE54->width=0,((_tmpE54->attributes=0,_tmpE54)))))))))))),((
_tmpE53->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmpE53))))));}{struct Cyc_Absyn_AggrdeclImpl*_tmpE59;const char*_tmpE58;
struct Cyc_Absyn_Aggrdecl*_tmpE57;struct Cyc_Absyn_Aggrdecl*_tmp98A=(_tmpE57=
_cycalloc(sizeof(*_tmpE57)),((_tmpE57->kind=Cyc_Absyn_StructA,((_tmpE57->sc=Cyc_Absyn_Public,((
_tmpE57->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpE58="_struct",
_tag_dyneither(_tmpE58,sizeof(char),8)))),((_tmpE57->tvs=0,((_tmpE57->impl=((
_tmpE59=_cycalloc(sizeof(*_tmpE59)),((_tmpE59->exist_vars=0,((_tmpE59->rgn_po=0,((
_tmpE59->fields=fields,((_tmpE59->tagged=0,_tmpE59)))))))))),((_tmpE57->attributes=
0,_tmpE57)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpE5F;struct Cyc_Absyn_Aggr_d_struct
_tmpE5E;struct Cyc_List_List*_tmpE5D;Cyc_Toc_result_decls=((_tmpE5D=_cycalloc(
sizeof(*_tmpE5D)),((_tmpE5D->hd=Cyc_Absyn_new_decl((void*)((_tmpE5F=_cycalloc(
sizeof(*_tmpE5F)),((_tmpE5F[0]=((_tmpE5E.tag=6,((_tmpE5E.f1=_tmp98A,_tmpE5E)))),
_tmpE5F)))),0),((_tmpE5D->tl=Cyc_Toc_result_decls,_tmpE5D))))));}goto _LL3AF;};};};}
_LL3B2: if(_tmp979 == 0)goto _LL3B4;_tmp97A=*_tmp979;if(_tmp97A != 0)goto _LL3B4;
_LL3B3: if(f->sc != Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*_tmp991=Cyc_Absyn_string_exp(*
fn,0);struct Cyc_Absyn_Vardecl*_tmp992=Cyc_Absyn_new_vardecl(f->name,_tmp978,(
struct Cyc_Absyn_Exp*)_tmp991);_tmp992->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*
_tmpE65;struct Cyc_Absyn_Var_d_struct _tmpE64;struct Cyc_List_List*_tmpE63;Cyc_Toc_result_decls=((
_tmpE63=_cycalloc(sizeof(*_tmpE63)),((_tmpE63->hd=Cyc_Absyn_new_decl((void*)((
_tmpE65=_cycalloc(sizeof(*_tmpE65)),((_tmpE65[0]=((_tmpE64.tag=0,((_tmpE64.f1=
_tmp992,_tmpE64)))),_tmpE65)))),0),((_tmpE63->tl=Cyc_Toc_result_decls,_tmpE63))))));}*
_tmp973=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp973,f->name,1);}goto _LL3AF;_LL3B4:;_LL3B5: goto _LL3AF;_LL3AF:;}};
_pop_dynregion(d);};};}static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*
ed){ed->sc=Cyc_Absyn_Public;if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(
struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,
struct Cyc_Absyn_Stmt*s){void*old_typ=vd->type;vd->type=Cyc_Toc_typ_to_c_array(
old_typ);if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ))vd->sc=Cyc_Absyn_Public;Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp996=init->r;struct Cyc_Absyn_Vardecl*_tmp998;struct Cyc_Absyn_Exp*_tmp999;
struct Cyc_Absyn_Exp*_tmp99A;int _tmp99B;_LL3B7: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp997=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp996;if(_tmp997->tag != 28)
goto _LL3B9;else{_tmp998=_tmp997->f1;_tmp999=_tmp997->f2;_tmp99A=_tmp997->f3;
_tmp99B=_tmp997->f4;}}_LL3B8: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp998,_tmp999,_tmp99A,_tmp99B,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL3B6;_LL3B9:;_LL3BA: if(vd->sc == Cyc_Absyn_Static){struct Cyc_Toc_Env
_tmp99D;struct _RegionHandle*_tmp99E;struct Cyc_Toc_Env*_tmp99C=init_nv;_tmp99D=*
_tmp99C;_tmp99E=_tmp99D.rgn;{struct Cyc_Toc_Env*_tmp99F=Cyc_Toc_set_toplevel(
_tmp99E,init_nv);Cyc_Toc_exp_to_c(_tmp99F,init);};}else{Cyc_Toc_exp_to_c(init_nv,
init);}goto _LL3B6;_LL3B6:;}else{void*_tmp9A0=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_ArrayInfo _tmp9A2;void*_tmp9A3;struct Cyc_Absyn_Exp*_tmp9A4;union Cyc_Absyn_Constraint*
_tmp9A5;_LL3BC:{struct Cyc_Absyn_ArrayType_struct*_tmp9A1=(struct Cyc_Absyn_ArrayType_struct*)
_tmp9A0;if(_tmp9A1->tag != 9)goto _LL3BE;else{_tmp9A2=_tmp9A1->f1;_tmp9A3=_tmp9A2.elt_type;
_tmp9A4=_tmp9A2.num_elts;_tmp9A5=_tmp9A2.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp9A5))goto _LL3BE;_LL3BD: if(_tmp9A4 == 0){const char*
_tmpE68;void*_tmpE67;(_tmpE67=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE68="can't initialize zero-terminated array -- size unknown",
_tag_dyneither(_tmpE68,sizeof(char),55))),_tag_dyneither(_tmpE67,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp9A4;struct Cyc_Absyn_Exp*
_tmp9A8=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp9A9=Cyc_Absyn_signed_int_exp(
0,0);s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp9A8,
_tmp9A9,0),0),Cyc_Absyn_new_stmt(s->r,0));goto _LL3BB;};_LL3BE:;_LL3BF: goto _LL3BB;
_LL3BB:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple0*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp9AB;struct
_RegionHandle*_tmp9AC;struct Cyc_Toc_Env*_tmp9AA=nv;_tmp9AB=*_tmp9AA;_tmp9AC=
_tmp9AB.rgn;{struct Cyc_Absyn_Stmt*_tmp9AD=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp9AE=Cyc_Toc_share_env(_tmp9AC,nv);struct Cyc_Toc_Env*_tmp9B0;struct Cyc_List_List*
_tmp9B1;struct Cyc_Absyn_Stmt*_tmp9B2;struct _tuple22 _tmp9AF=Cyc_Toc_xlate_pat(
_tmp9AE,_tmp9AC,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp9B0=_tmp9AF.f1;_tmp9B1=_tmp9AF.f2;_tmp9B2=_tmp9AF.f3;Cyc_Toc_stmt_to_c(
_tmp9B0,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp9B2,s,0),0);for(0;_tmp9B1 != 0;_tmp9B1=_tmp9B1->tl){
struct _tuple23 _tmp9B4;struct _tuple0*_tmp9B5;void*_tmp9B6;struct _tuple23*_tmp9B3=(
struct _tuple23*)_tmp9B1->hd;_tmp9B4=*_tmp9B3;_tmp9B5=_tmp9B4.f1;_tmp9B6=_tmp9B4.f2;
s=Cyc_Absyn_declare_stmt(_tmp9B5,_tmp9B6,0,s,0);}}return s;};}static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e){void*_tmp9B7=e->r;struct Cyc_Absyn_Exp*_tmp9B9;struct Cyc_Absyn_Exp*
_tmp9BB;struct Cyc_Absyn_Exp*_tmp9BD;struct Cyc_Absyn_Exp*_tmp9BF;struct Cyc_Absyn_Exp*
_tmp9C1;struct Cyc_Absyn_Exp*_tmp9C3;struct Cyc_Absyn_Exp*_tmp9C5;struct Cyc_Absyn_Exp*
_tmp9C7;struct Cyc_List_List*_tmp9C9;struct Cyc_Absyn_Exp*_tmp9CB;struct Cyc_Absyn_Exp*
_tmp9CC;struct Cyc_Absyn_Exp*_tmp9CE;struct Cyc_Absyn_Exp*_tmp9CF;struct Cyc_Absyn_Exp*
_tmp9D1;struct Cyc_Absyn_Exp*_tmp9D2;struct Cyc_Absyn_Exp*_tmp9D4;struct Cyc_Absyn_Exp*
_tmp9D5;struct Cyc_Absyn_Exp*_tmp9D7;struct Cyc_Absyn_Exp*_tmp9D8;struct Cyc_Absyn_Exp*
_tmp9DA;struct Cyc_Absyn_Exp*_tmp9DB;struct Cyc_Absyn_Exp*_tmp9DD;struct Cyc_Absyn_Exp*
_tmp9DE;struct Cyc_Absyn_Exp*_tmp9DF;struct Cyc_Absyn_Exp*_tmp9E1;struct Cyc_List_List*
_tmp9E2;struct Cyc_Absyn_Exp*_tmp9E4;struct Cyc_List_List*_tmp9E5;void*_tmp9E7;
void**_tmp9E8;struct Cyc_Absyn_Exp*_tmp9E9;struct _tuple7*_tmp9EB;struct _tuple7
_tmp9EC;void*_tmp9ED;void**_tmp9EE;struct Cyc_List_List*_tmp9EF;struct Cyc_List_List*
_tmp9F1;struct Cyc_List_List*_tmp9F3;void*_tmp9F5;void**_tmp9F6;void*_tmp9F8;void**
_tmp9F9;struct Cyc_Absyn_Stmt*_tmp9FB;struct Cyc_Absyn_MallocInfo _tmp9FD;struct Cyc_Absyn_MallocInfo*
_tmp9FE;_LL3C1: {struct Cyc_Absyn_Deref_e_struct*_tmp9B8=(struct Cyc_Absyn_Deref_e_struct*)
_tmp9B7;if(_tmp9B8->tag != 21)goto _LL3C3;else{_tmp9B9=_tmp9B8->f1;}}_LL3C2:
_tmp9BB=_tmp9B9;goto _LL3C4;_LL3C3: {struct Cyc_Absyn_AggrMember_e_struct*_tmp9BA=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp9B7;if(_tmp9BA->tag != 22)goto _LL3C5;
else{_tmp9BB=_tmp9BA->f1;}}_LL3C4: _tmp9BD=_tmp9BB;goto _LL3C6;_LL3C5: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp9BC=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp9B7;if(_tmp9BC->tag != 23)goto
_LL3C7;else{_tmp9BD=_tmp9BC->f1;}}_LL3C6: _tmp9BF=_tmp9BD;goto _LL3C8;_LL3C7: {
struct Cyc_Absyn_Address_e_struct*_tmp9BE=(struct Cyc_Absyn_Address_e_struct*)
_tmp9B7;if(_tmp9BE->tag != 16)goto _LL3C9;else{_tmp9BF=_tmp9BE->f1;}}_LL3C8:
_tmp9C1=_tmp9BF;goto _LL3CA;_LL3C9: {struct Cyc_Absyn_Throw_e_struct*_tmp9C0=(
struct Cyc_Absyn_Throw_e_struct*)_tmp9B7;if(_tmp9C0->tag != 12)goto _LL3CB;else{
_tmp9C1=_tmp9C0->f1;}}_LL3CA: _tmp9C3=_tmp9C1;goto _LL3CC;_LL3CB: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp9C2=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp9B7;if(_tmp9C2->tag != 13)
goto _LL3CD;else{_tmp9C3=_tmp9C2->f1;}}_LL3CC: _tmp9C5=_tmp9C3;goto _LL3CE;_LL3CD: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp9C4=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp9B7;if(_tmp9C4->tag != 19)goto _LL3CF;else{_tmp9C5=_tmp9C4->f1;}}_LL3CE:
_tmp9C7=_tmp9C5;goto _LL3D0;_LL3CF: {struct Cyc_Absyn_Increment_e_struct*_tmp9C6=(
struct Cyc_Absyn_Increment_e_struct*)_tmp9B7;if(_tmp9C6->tag != 5)goto _LL3D1;else{
_tmp9C7=_tmp9C6->f1;}}_LL3D0: Cyc_Toc_exptypes_to_c(_tmp9C7);goto _LL3C0;_LL3D1: {
struct Cyc_Absyn_Primop_e_struct*_tmp9C8=(struct Cyc_Absyn_Primop_e_struct*)
_tmp9B7;if(_tmp9C8->tag != 3)goto _LL3D3;else{_tmp9C9=_tmp9C8->f2;}}_LL3D2:((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,
_tmp9C9);goto _LL3C0;_LL3D3: {struct Cyc_Absyn_And_e_struct*_tmp9CA=(struct Cyc_Absyn_And_e_struct*)
_tmp9B7;if(_tmp9CA->tag != 7)goto _LL3D5;else{_tmp9CB=_tmp9CA->f1;_tmp9CC=_tmp9CA->f2;}}
_LL3D4: _tmp9CE=_tmp9CB;_tmp9CF=_tmp9CC;goto _LL3D6;_LL3D5: {struct Cyc_Absyn_Or_e_struct*
_tmp9CD=(struct Cyc_Absyn_Or_e_struct*)_tmp9B7;if(_tmp9CD->tag != 8)goto _LL3D7;
else{_tmp9CE=_tmp9CD->f1;_tmp9CF=_tmp9CD->f2;}}_LL3D6: _tmp9D1=_tmp9CE;_tmp9D2=
_tmp9CF;goto _LL3D8;_LL3D7: {struct Cyc_Absyn_SeqExp_e_struct*_tmp9D0=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp9B7;if(_tmp9D0->tag != 9)goto _LL3D9;else{_tmp9D1=_tmp9D0->f1;_tmp9D2=_tmp9D0->f2;}}
_LL3D8: _tmp9D4=_tmp9D1;_tmp9D5=_tmp9D2;goto _LL3DA;_LL3D9: {struct Cyc_Absyn_Subscript_e_struct*
_tmp9D3=(struct Cyc_Absyn_Subscript_e_struct*)_tmp9B7;if(_tmp9D3->tag != 24)goto
_LL3DB;else{_tmp9D4=_tmp9D3->f1;_tmp9D5=_tmp9D3->f2;}}_LL3DA: _tmp9D7=_tmp9D4;
_tmp9D8=_tmp9D5;goto _LL3DC;_LL3DB: {struct Cyc_Absyn_Swap_e_struct*_tmp9D6=(
struct Cyc_Absyn_Swap_e_struct*)_tmp9B7;if(_tmp9D6->tag != 35)goto _LL3DD;else{
_tmp9D7=_tmp9D6->f1;_tmp9D8=_tmp9D6->f2;}}_LL3DC: _tmp9DA=_tmp9D7;_tmp9DB=_tmp9D8;
goto _LL3DE;_LL3DD: {struct Cyc_Absyn_AssignOp_e_struct*_tmp9D9=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp9B7;if(_tmp9D9->tag != 4)goto _LL3DF;else{_tmp9DA=_tmp9D9->f1;_tmp9DB=_tmp9D9->f3;}}
_LL3DE: Cyc_Toc_exptypes_to_c(_tmp9DA);Cyc_Toc_exptypes_to_c(_tmp9DB);goto _LL3C0;
_LL3DF: {struct Cyc_Absyn_Conditional_e_struct*_tmp9DC=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp9B7;if(_tmp9DC->tag != 6)goto _LL3E1;else{_tmp9DD=_tmp9DC->f1;_tmp9DE=_tmp9DC->f2;
_tmp9DF=_tmp9DC->f3;}}_LL3E0: Cyc_Toc_exptypes_to_c(_tmp9DD);Cyc_Toc_exptypes_to_c(
_tmp9DE);Cyc_Toc_exptypes_to_c(_tmp9DF);goto _LL3C0;_LL3E1: {struct Cyc_Absyn_FnCall_e_struct*
_tmp9E0=(struct Cyc_Absyn_FnCall_e_struct*)_tmp9B7;if(_tmp9E0->tag != 11)goto
_LL3E3;else{_tmp9E1=_tmp9E0->f1;_tmp9E2=_tmp9E0->f2;}}_LL3E2: _tmp9E4=_tmp9E1;
_tmp9E5=_tmp9E2;goto _LL3E4;_LL3E3: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp9E3=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp9B7;if(_tmp9E3->tag != 10)goto _LL3E5;
else{_tmp9E4=_tmp9E3->f1;_tmp9E5=_tmp9E3->f2;}}_LL3E4: Cyc_Toc_exptypes_to_c(
_tmp9E4);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp9E5);goto _LL3C0;_LL3E5: {struct Cyc_Absyn_Cast_e_struct*
_tmp9E6=(struct Cyc_Absyn_Cast_e_struct*)_tmp9B7;if(_tmp9E6->tag != 15)goto _LL3E7;
else{_tmp9E7=(void**)& _tmp9E6->f1;_tmp9E8=(void**)((void**)& _tmp9E6->f1);_tmp9E9=
_tmp9E6->f2;}}_LL3E6:*_tmp9E8=Cyc_Toc_typ_to_c(*_tmp9E8);Cyc_Toc_exptypes_to_c(
_tmp9E9);goto _LL3C0;_LL3E7: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp9EA=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp9B7;if(_tmp9EA->tag != 26)goto _LL3E9;
else{_tmp9EB=_tmp9EA->f1;_tmp9EC=*_tmp9EB;_tmp9ED=_tmp9EC.f3;_tmp9EE=(void**)&(*
_tmp9EA->f1).f3;_tmp9EF=_tmp9EA->f2;}}_LL3E8:*_tmp9EE=Cyc_Toc_typ_to_c(*_tmp9EE);
_tmp9F1=_tmp9EF;goto _LL3EA;_LL3E9: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp9F0=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp9B7;if(_tmp9F0->tag != 36)
goto _LL3EB;else{_tmp9F1=_tmp9F0->f2;}}_LL3EA: _tmp9F3=_tmp9F1;goto _LL3EC;_LL3EB: {
struct Cyc_Absyn_Array_e_struct*_tmp9F2=(struct Cyc_Absyn_Array_e_struct*)_tmp9B7;
if(_tmp9F2->tag != 27)goto _LL3ED;else{_tmp9F3=_tmp9F2->f1;}}_LL3EC: for(0;_tmp9F3
!= 0;_tmp9F3=_tmp9F3->tl){struct Cyc_Absyn_Exp*_tmpA0F;struct _tuple15 _tmpA0E=*((
struct _tuple15*)_tmp9F3->hd);_tmpA0F=_tmpA0E.f2;Cyc_Toc_exptypes_to_c(_tmpA0F);}
goto _LL3C0;_LL3ED: {struct Cyc_Absyn_Offsetof_e_struct*_tmp9F4=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp9B7;if(_tmp9F4->tag != 20)goto _LL3EF;else{_tmp9F5=(void**)& _tmp9F4->f1;
_tmp9F6=(void**)((void**)& _tmp9F4->f1);}}_LL3EE: _tmp9F9=_tmp9F6;goto _LL3F0;
_LL3EF: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp9F7=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp9B7;if(_tmp9F7->tag != 18)goto _LL3F1;else{_tmp9F8=(void**)& _tmp9F7->f1;
_tmp9F9=(void**)((void**)& _tmp9F7->f1);}}_LL3F0:*_tmp9F9=Cyc_Toc_typ_to_c(*
_tmp9F9);goto _LL3C0;_LL3F1: {struct Cyc_Absyn_StmtExp_e_struct*_tmp9FA=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp9B7;if(_tmp9FA->tag != 37)goto _LL3F3;else{_tmp9FB=_tmp9FA->f1;}}_LL3F2: Cyc_Toc_stmttypes_to_c(
_tmp9FB);goto _LL3C0;_LL3F3: {struct Cyc_Absyn_Malloc_e_struct*_tmp9FC=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp9B7;if(_tmp9FC->tag != 34)goto _LL3F5;else{_tmp9FD=_tmp9FC->f1;_tmp9FE=(struct
Cyc_Absyn_MallocInfo*)& _tmp9FC->f1;}}_LL3F4: if(_tmp9FE->elt_type != 0){void**
_tmpE69;_tmp9FE->elt_type=((_tmpE69=_cycalloc(sizeof(*_tmpE69)),((_tmpE69[0]=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp9FE->elt_type))),_tmpE69))));}Cyc_Toc_exptypes_to_c(
_tmp9FE->num_elts);goto _LL3C0;_LL3F5: {struct Cyc_Absyn_Const_e_struct*_tmp9FF=(
struct Cyc_Absyn_Const_e_struct*)_tmp9B7;if(_tmp9FF->tag != 0)goto _LL3F7;}_LL3F6:
goto _LL3F8;_LL3F7: {struct Cyc_Absyn_Var_e_struct*_tmpA00=(struct Cyc_Absyn_Var_e_struct*)
_tmp9B7;if(_tmpA00->tag != 1)goto _LL3F9;}_LL3F8: goto _LL3FA;_LL3F9: {struct Cyc_Absyn_UnknownId_e_struct*
_tmpA01=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp9B7;if(_tmpA01->tag != 2)goto
_LL3FB;}_LL3FA: goto _LL3FC;_LL3FB: {struct Cyc_Absyn_Enum_e_struct*_tmpA02=(struct
Cyc_Absyn_Enum_e_struct*)_tmp9B7;if(_tmpA02->tag != 32)goto _LL3FD;}_LL3FC: goto
_LL3FE;_LL3FD: {struct Cyc_Absyn_Asm_e_struct*_tmpA03=(struct Cyc_Absyn_Asm_e_struct*)
_tmp9B7;if(_tmpA03->tag != 40)goto _LL3FF;}_LL3FE: goto _LL400;_LL3FF: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmpA04=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp9B7;if(_tmpA04->tag != 33)goto
_LL401;}_LL400: goto _LL3C0;_LL401: {struct Cyc_Absyn_AnonStruct_e_struct*_tmpA05=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp9B7;if(_tmpA05->tag != 30)goto _LL403;}
_LL402: goto _LL404;_LL403: {struct Cyc_Absyn_Datatype_e_struct*_tmpA06=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp9B7;if(_tmpA06->tag != 31)goto _LL405;}_LL404: goto _LL406;_LL405: {struct Cyc_Absyn_Aggregate_e_struct*
_tmpA07=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp9B7;if(_tmpA07->tag != 29)goto
_LL407;}_LL406: goto _LL408;_LL407: {struct Cyc_Absyn_Comprehension_e_struct*
_tmpA08=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp9B7;if(_tmpA08->tag != 28)
goto _LL409;}_LL408: goto _LL40A;_LL409: {struct Cyc_Absyn_Tuple_e_struct*_tmpA09=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp9B7;if(_tmpA09->tag != 25)goto _LL40B;}_LL40A:
goto _LL40C;_LL40B: {struct Cyc_Absyn_Instantiate_e_struct*_tmpA0A=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp9B7;if(_tmpA0A->tag != 14)goto _LL40D;}_LL40C: goto _LL40E;_LL40D: {struct Cyc_Absyn_New_e_struct*
_tmpA0B=(struct Cyc_Absyn_New_e_struct*)_tmp9B7;if(_tmpA0B->tag != 17)goto _LL40F;}
_LL40E: goto _LL410;_LL40F: {struct Cyc_Absyn_Valueof_e_struct*_tmpA0C=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp9B7;if(_tmpA0C->tag != 39)goto _LL411;}_LL410: goto _LL412;_LL411: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmpA0D=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp9B7;if(_tmpA0D->tag != 38)goto
_LL3C0;}_LL412:{const char*_tmpE6C;void*_tmpE6B;(_tmpE6B=0,Cyc_Tcutil_terr(e->loc,((
_tmpE6C="Cyclone expression within C code",_tag_dyneither(_tmpE6C,sizeof(char),
33))),_tag_dyneither(_tmpE6B,sizeof(void*),0)));}goto _LL3C0;_LL3C0:;}static void
Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){void*_tmpA13=d->r;struct Cyc_Absyn_Vardecl*
_tmpA15;struct Cyc_Absyn_Fndecl*_tmpA17;struct Cyc_Absyn_Aggrdecl*_tmpA19;struct
Cyc_Absyn_Enumdecl*_tmpA1B;struct Cyc_Absyn_Typedefdecl*_tmpA1D;_LL414: {struct
Cyc_Absyn_Var_d_struct*_tmpA14=(struct Cyc_Absyn_Var_d_struct*)_tmpA13;if(_tmpA14->tag
!= 0)goto _LL416;else{_tmpA15=_tmpA14->f1;}}_LL415: _tmpA15->type=Cyc_Toc_typ_to_c(
_tmpA15->type);if(_tmpA15->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmpA15->initializer));goto _LL413;_LL416: {struct Cyc_Absyn_Fn_d_struct*
_tmpA16=(struct Cyc_Absyn_Fn_d_struct*)_tmpA13;if(_tmpA16->tag != 1)goto _LL418;
else{_tmpA17=_tmpA16->f1;}}_LL417: _tmpA17->ret_type=Cyc_Toc_typ_to_c(_tmpA17->ret_type);{
struct Cyc_List_List*_tmpA29=_tmpA17->args;for(0;_tmpA29 != 0;_tmpA29=_tmpA29->tl){(*((
struct _tuple26*)_tmpA29->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple26*)_tmpA29->hd)).f3);}}
goto _LL413;_LL418: {struct Cyc_Absyn_Aggr_d_struct*_tmpA18=(struct Cyc_Absyn_Aggr_d_struct*)
_tmpA13;if(_tmpA18->tag != 6)goto _LL41A;else{_tmpA19=_tmpA18->f1;}}_LL419: Cyc_Toc_aggrdecl_to_c(
_tmpA19,1);goto _LL413;_LL41A: {struct Cyc_Absyn_Enum_d_struct*_tmpA1A=(struct Cyc_Absyn_Enum_d_struct*)
_tmpA13;if(_tmpA1A->tag != 8)goto _LL41C;else{_tmpA1B=_tmpA1A->f1;}}_LL41B: if(
_tmpA1B->fields != 0){struct Cyc_List_List*_tmpA2A=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmpA1B->fields))->v;for(0;_tmpA2A != 0;_tmpA2A=_tmpA2A->tl){
struct Cyc_Absyn_Enumfield*_tmpA2B=(struct Cyc_Absyn_Enumfield*)_tmpA2A->hd;if(
_tmpA2B->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmpA2B->tag));}}
goto _LL413;_LL41C: {struct Cyc_Absyn_Typedef_d_struct*_tmpA1C=(struct Cyc_Absyn_Typedef_d_struct*)
_tmpA13;if(_tmpA1C->tag != 9)goto _LL41E;else{_tmpA1D=_tmpA1C->f1;}}_LL41D:{struct
Cyc_Core_Opt*_tmpE6D;_tmpA1D->defn=((_tmpE6D=_cycalloc(sizeof(*_tmpE6D)),((
_tmpE6D->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA1D->defn))->v),_tmpE6D))));}goto _LL413;_LL41E: {struct Cyc_Absyn_Let_d_struct*
_tmpA1E=(struct Cyc_Absyn_Let_d_struct*)_tmpA13;if(_tmpA1E->tag != 2)goto _LL420;}
_LL41F: goto _LL421;_LL420: {struct Cyc_Absyn_Letv_d_struct*_tmpA1F=(struct Cyc_Absyn_Letv_d_struct*)
_tmpA13;if(_tmpA1F->tag != 3)goto _LL422;}_LL421: goto _LL423;_LL422: {struct Cyc_Absyn_Datatype_d_struct*
_tmpA20=(struct Cyc_Absyn_Datatype_d_struct*)_tmpA13;if(_tmpA20->tag != 7)goto
_LL424;}_LL423: goto _LL425;_LL424: {struct Cyc_Absyn_Namespace_d_struct*_tmpA21=(
struct Cyc_Absyn_Namespace_d_struct*)_tmpA13;if(_tmpA21->tag != 10)goto _LL426;}
_LL425: goto _LL427;_LL426: {struct Cyc_Absyn_Using_d_struct*_tmpA22=(struct Cyc_Absyn_Using_d_struct*)
_tmpA13;if(_tmpA22->tag != 11)goto _LL428;}_LL427: goto _LL429;_LL428: {struct Cyc_Absyn_ExternC_d_struct*
_tmpA23=(struct Cyc_Absyn_ExternC_d_struct*)_tmpA13;if(_tmpA23->tag != 12)goto
_LL42A;}_LL429: goto _LL42B;_LL42A: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmpA24=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmpA13;if(_tmpA24->tag != 13)
goto _LL42C;}_LL42B: goto _LL42D;_LL42C: {struct Cyc_Absyn_Region_d_struct*_tmpA25=(
struct Cyc_Absyn_Region_d_struct*)_tmpA13;if(_tmpA25->tag != 4)goto _LL42E;}_LL42D:
goto _LL42F;_LL42E: {struct Cyc_Absyn_Alias_d_struct*_tmpA26=(struct Cyc_Absyn_Alias_d_struct*)
_tmpA13;if(_tmpA26->tag != 5)goto _LL430;}_LL42F:{const char*_tmpE70;void*_tmpE6F;(
_tmpE6F=0,Cyc_Tcutil_terr(d->loc,((_tmpE70="Cyclone declaration within C code",
_tag_dyneither(_tmpE70,sizeof(char),34))),_tag_dyneither(_tmpE6F,sizeof(void*),0)));}
goto _LL413;_LL430: {struct Cyc_Absyn_Porton_d_struct*_tmpA27=(struct Cyc_Absyn_Porton_d_struct*)
_tmpA13;if(_tmpA27->tag != 14)goto _LL432;}_LL431: goto _LL433;_LL432: {struct Cyc_Absyn_Portoff_d_struct*
_tmpA28=(struct Cyc_Absyn_Portoff_d_struct*)_tmpA13;if(_tmpA28->tag != 15)goto
_LL413;}_LL433: goto _LL413;_LL413:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s){void*_tmpA2F=s->r;struct Cyc_Absyn_Exp*_tmpA31;struct Cyc_Absyn_Stmt*_tmpA33;
struct Cyc_Absyn_Stmt*_tmpA34;struct Cyc_Absyn_Exp*_tmpA36;struct Cyc_Absyn_Exp*
_tmpA38;struct Cyc_Absyn_Stmt*_tmpA39;struct Cyc_Absyn_Stmt*_tmpA3A;struct _tuple8
_tmpA3C;struct Cyc_Absyn_Exp*_tmpA3D;struct Cyc_Absyn_Stmt*_tmpA3E;struct Cyc_Absyn_Exp*
_tmpA40;struct _tuple8 _tmpA41;struct Cyc_Absyn_Exp*_tmpA42;struct _tuple8 _tmpA43;
struct Cyc_Absyn_Exp*_tmpA44;struct Cyc_Absyn_Stmt*_tmpA45;struct Cyc_Absyn_Exp*
_tmpA47;struct Cyc_List_List*_tmpA48;struct Cyc_Absyn_Decl*_tmpA4A;struct Cyc_Absyn_Stmt*
_tmpA4B;struct Cyc_Absyn_Stmt*_tmpA4D;struct _tuple8 _tmpA4E;struct Cyc_Absyn_Exp*
_tmpA4F;_LL435: {struct Cyc_Absyn_Exp_s_struct*_tmpA30=(struct Cyc_Absyn_Exp_s_struct*)
_tmpA2F;if(_tmpA30->tag != 1)goto _LL437;else{_tmpA31=_tmpA30->f1;}}_LL436: Cyc_Toc_exptypes_to_c(
_tmpA31);goto _LL434;_LL437: {struct Cyc_Absyn_Seq_s_struct*_tmpA32=(struct Cyc_Absyn_Seq_s_struct*)
_tmpA2F;if(_tmpA32->tag != 2)goto _LL439;else{_tmpA33=_tmpA32->f1;_tmpA34=_tmpA32->f2;}}
_LL438: Cyc_Toc_stmttypes_to_c(_tmpA33);Cyc_Toc_stmttypes_to_c(_tmpA34);goto
_LL434;_LL439: {struct Cyc_Absyn_Return_s_struct*_tmpA35=(struct Cyc_Absyn_Return_s_struct*)
_tmpA2F;if(_tmpA35->tag != 3)goto _LL43B;else{_tmpA36=_tmpA35->f1;}}_LL43A: if(
_tmpA36 != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmpA36);goto _LL434;
_LL43B: {struct Cyc_Absyn_IfThenElse_s_struct*_tmpA37=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmpA2F;if(_tmpA37->tag != 4)goto _LL43D;else{_tmpA38=_tmpA37->f1;_tmpA39=_tmpA37->f2;
_tmpA3A=_tmpA37->f3;}}_LL43C: Cyc_Toc_exptypes_to_c(_tmpA38);Cyc_Toc_stmttypes_to_c(
_tmpA39);Cyc_Toc_stmttypes_to_c(_tmpA3A);goto _LL434;_LL43D: {struct Cyc_Absyn_While_s_struct*
_tmpA3B=(struct Cyc_Absyn_While_s_struct*)_tmpA2F;if(_tmpA3B->tag != 5)goto _LL43F;
else{_tmpA3C=_tmpA3B->f1;_tmpA3D=_tmpA3C.f1;_tmpA3E=_tmpA3B->f2;}}_LL43E: Cyc_Toc_exptypes_to_c(
_tmpA3D);Cyc_Toc_stmttypes_to_c(_tmpA3E);goto _LL434;_LL43F: {struct Cyc_Absyn_For_s_struct*
_tmpA3F=(struct Cyc_Absyn_For_s_struct*)_tmpA2F;if(_tmpA3F->tag != 9)goto _LL441;
else{_tmpA40=_tmpA3F->f1;_tmpA41=_tmpA3F->f2;_tmpA42=_tmpA41.f1;_tmpA43=_tmpA3F->f3;
_tmpA44=_tmpA43.f1;_tmpA45=_tmpA3F->f4;}}_LL440: Cyc_Toc_exptypes_to_c(_tmpA40);
Cyc_Toc_exptypes_to_c(_tmpA42);Cyc_Toc_exptypes_to_c(_tmpA44);Cyc_Toc_stmttypes_to_c(
_tmpA45);goto _LL434;_LL441: {struct Cyc_Absyn_Switch_s_struct*_tmpA46=(struct Cyc_Absyn_Switch_s_struct*)
_tmpA2F;if(_tmpA46->tag != 10)goto _LL443;else{_tmpA47=_tmpA46->f1;_tmpA48=_tmpA46->f2;}}
_LL442: Cyc_Toc_exptypes_to_c(_tmpA47);for(0;_tmpA48 != 0;_tmpA48=_tmpA48->tl){Cyc_Toc_stmttypes_to_c(((
struct Cyc_Absyn_Switch_clause*)_tmpA48->hd)->body);}goto _LL434;_LL443: {struct
Cyc_Absyn_Decl_s_struct*_tmpA49=(struct Cyc_Absyn_Decl_s_struct*)_tmpA2F;if(
_tmpA49->tag != 12)goto _LL445;else{_tmpA4A=_tmpA49->f1;_tmpA4B=_tmpA49->f2;}}
_LL444: Cyc_Toc_decltypes_to_c(_tmpA4A);Cyc_Toc_stmttypes_to_c(_tmpA4B);goto
_LL434;_LL445: {struct Cyc_Absyn_Do_s_struct*_tmpA4C=(struct Cyc_Absyn_Do_s_struct*)
_tmpA2F;if(_tmpA4C->tag != 14)goto _LL447;else{_tmpA4D=_tmpA4C->f1;_tmpA4E=_tmpA4C->f2;
_tmpA4F=_tmpA4E.f1;}}_LL446: Cyc_Toc_stmttypes_to_c(_tmpA4D);Cyc_Toc_exptypes_to_c(
_tmpA4F);goto _LL434;_LL447: {struct Cyc_Absyn_Skip_s_struct*_tmpA50=(struct Cyc_Absyn_Skip_s_struct*)
_tmpA2F;if(_tmpA50->tag != 0)goto _LL449;}_LL448: goto _LL44A;_LL449: {struct Cyc_Absyn_Break_s_struct*
_tmpA51=(struct Cyc_Absyn_Break_s_struct*)_tmpA2F;if(_tmpA51->tag != 6)goto _LL44B;}
_LL44A: goto _LL44C;_LL44B: {struct Cyc_Absyn_Continue_s_struct*_tmpA52=(struct Cyc_Absyn_Continue_s_struct*)
_tmpA2F;if(_tmpA52->tag != 7)goto _LL44D;}_LL44C: goto _LL44E;_LL44D: {struct Cyc_Absyn_Goto_s_struct*
_tmpA53=(struct Cyc_Absyn_Goto_s_struct*)_tmpA2F;if(_tmpA53->tag != 8)goto _LL44F;}
_LL44E: goto _LL434;_LL44F: {struct Cyc_Absyn_Fallthru_s_struct*_tmpA54=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmpA2F;if(_tmpA54->tag != 11)goto _LL451;}_LL450: goto _LL452;_LL451: {struct Cyc_Absyn_Label_s_struct*
_tmpA55=(struct Cyc_Absyn_Label_s_struct*)_tmpA2F;if(_tmpA55->tag != 13)goto _LL453;}
_LL452: goto _LL454;_LL453: {struct Cyc_Absyn_TryCatch_s_struct*_tmpA56=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmpA2F;if(_tmpA56->tag != 15)goto _LL455;}_LL454: goto _LL456;_LL455: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmpA57=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmpA2F;if(_tmpA57->tag != 16)goto
_LL434;}_LL456:{const char*_tmpE73;void*_tmpE72;(_tmpE72=0,Cyc_Tcutil_terr(s->loc,((
_tmpE73="Cyclone statement in C code",_tag_dyneither(_tmpE73,sizeof(char),28))),
_tag_dyneither(_tmpE72,sizeof(void*),0)));}goto _LL434;_LL434:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude){for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpE76;void*_tmpE75;(_tmpE75=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE76="decls_to_c: not at toplevel!",
_tag_dyneither(_tmpE76,sizeof(char),29))),_tag_dyneither(_tmpE75,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmpA5C=d->r;struct Cyc_Absyn_Vardecl*
_tmpA5E;struct Cyc_Absyn_Fndecl*_tmpA60;struct Cyc_Absyn_Aggrdecl*_tmpA66;struct
Cyc_Absyn_Datatypedecl*_tmpA68;struct Cyc_Absyn_Enumdecl*_tmpA6A;struct Cyc_Absyn_Typedefdecl*
_tmpA6C;struct Cyc_List_List*_tmpA70;struct Cyc_List_List*_tmpA72;struct Cyc_List_List*
_tmpA74;struct Cyc_List_List*_tmpA76;_LL458: {struct Cyc_Absyn_Var_d_struct*
_tmpA5D=(struct Cyc_Absyn_Var_d_struct*)_tmpA5C;if(_tmpA5D->tag != 0)goto _LL45A;
else{_tmpA5E=_tmpA5D->f1;}}_LL459: {struct _tuple0*_tmpA77=_tmpA5E->name;if(
_tmpA5E->sc == Cyc_Absyn_ExternC){struct _tuple0*_tmpE77;_tmpA77=((_tmpE77=
_cycalloc(sizeof(*_tmpE77)),((_tmpE77->f1=Cyc_Absyn_Rel_n(0),((_tmpE77->f2=(*
_tmpA77).f2,_tmpE77))))));}if(_tmpA5E->initializer != 0){if(cinclude)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmpA5E->initializer));else{Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_check_null(_tmpA5E->initializer));}}{struct Cyc_Absyn_Global_b_struct
_tmpE7A;struct Cyc_Absyn_Global_b_struct*_tmpE79;nv=Cyc_Toc_add_varmap(r,nv,
_tmpA5E->name,Cyc_Absyn_varb_exp(_tmpA77,(void*)((_tmpE79=_cycalloc(sizeof(*
_tmpE79)),((_tmpE79[0]=((_tmpE7A.tag=1,((_tmpE7A.f1=_tmpA5E,_tmpE7A)))),_tmpE79)))),
0));}_tmpA5E->name=_tmpA77;_tmpA5E->sc=Cyc_Toc_scope_to_c(_tmpA5E->sc);_tmpA5E->type=
Cyc_Toc_typ_to_c_array(_tmpA5E->type);{struct Cyc_List_List*_tmpE7B;Cyc_Toc_result_decls=((
_tmpE7B=_cycalloc(sizeof(*_tmpE7B)),((_tmpE7B->hd=d,((_tmpE7B->tl=Cyc_Toc_result_decls,
_tmpE7B))))));}goto _LL457;}_LL45A: {struct Cyc_Absyn_Fn_d_struct*_tmpA5F=(struct
Cyc_Absyn_Fn_d_struct*)_tmpA5C;if(_tmpA5F->tag != 1)goto _LL45C;else{_tmpA60=
_tmpA5F->f1;}}_LL45B: {struct _tuple0*_tmpA7C=_tmpA60->name;if(_tmpA60->sc == Cyc_Absyn_ExternC){{
struct _tuple0*_tmpE7C;_tmpA7C=((_tmpE7C=_cycalloc(sizeof(*_tmpE7C)),((_tmpE7C->f1=
Cyc_Absyn_Rel_n(0),((_tmpE7C->f2=(*_tmpA7C).f2,_tmpE7C))))));}_tmpA60->sc=Cyc_Absyn_Public;}
nv=Cyc_Toc_add_varmap(r,nv,_tmpA60->name,Cyc_Absyn_var_exp(_tmpA7C,0));_tmpA60->name=
_tmpA7C;Cyc_Toc_fndecl_to_c(nv,_tmpA60,cinclude);{struct Cyc_List_List*_tmpE7D;
Cyc_Toc_result_decls=((_tmpE7D=_cycalloc(sizeof(*_tmpE7D)),((_tmpE7D->hd=d,((
_tmpE7D->tl=Cyc_Toc_result_decls,_tmpE7D))))));}goto _LL457;}_LL45C: {struct Cyc_Absyn_Let_d_struct*
_tmpA61=(struct Cyc_Absyn_Let_d_struct*)_tmpA5C;if(_tmpA61->tag != 2)goto _LL45E;}
_LL45D: goto _LL45F;_LL45E: {struct Cyc_Absyn_Letv_d_struct*_tmpA62=(struct Cyc_Absyn_Letv_d_struct*)
_tmpA5C;if(_tmpA62->tag != 3)goto _LL460;}_LL45F: {const char*_tmpE80;void*_tmpE7F;(
_tmpE7F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE80="letdecl at toplevel",_tag_dyneither(_tmpE80,sizeof(char),20))),
_tag_dyneither(_tmpE7F,sizeof(void*),0)));}_LL460: {struct Cyc_Absyn_Region_d_struct*
_tmpA63=(struct Cyc_Absyn_Region_d_struct*)_tmpA5C;if(_tmpA63->tag != 4)goto _LL462;}
_LL461: {const char*_tmpE83;void*_tmpE82;(_tmpE82=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE83="region decl at toplevel",
_tag_dyneither(_tmpE83,sizeof(char),24))),_tag_dyneither(_tmpE82,sizeof(void*),0)));}
_LL462: {struct Cyc_Absyn_Alias_d_struct*_tmpA64=(struct Cyc_Absyn_Alias_d_struct*)
_tmpA5C;if(_tmpA64->tag != 5)goto _LL464;}_LL463: {const char*_tmpE86;void*_tmpE85;(
_tmpE85=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE86="alias decl at toplevel",_tag_dyneither(_tmpE86,sizeof(char),23))),
_tag_dyneither(_tmpE85,sizeof(void*),0)));}_LL464: {struct Cyc_Absyn_Aggr_d_struct*
_tmpA65=(struct Cyc_Absyn_Aggr_d_struct*)_tmpA5C;if(_tmpA65->tag != 6)goto _LL466;
else{_tmpA66=_tmpA65->f1;}}_LL465: Cyc_Toc_aggrdecl_to_c(_tmpA66,1);goto _LL457;
_LL466: {struct Cyc_Absyn_Datatype_d_struct*_tmpA67=(struct Cyc_Absyn_Datatype_d_struct*)
_tmpA5C;if(_tmpA67->tag != 7)goto _LL468;else{_tmpA68=_tmpA67->f1;}}_LL467: if(
_tmpA68->is_extensible)Cyc_Toc_xdatatypedecl_to_c(_tmpA68);else{Cyc_Toc_datatypedecl_to_c(
_tmpA68);}goto _LL457;_LL468: {struct Cyc_Absyn_Enum_d_struct*_tmpA69=(struct Cyc_Absyn_Enum_d_struct*)
_tmpA5C;if(_tmpA69->tag != 8)goto _LL46A;else{_tmpA6A=_tmpA69->f1;}}_LL469: Cyc_Toc_enumdecl_to_c(
_tmpA6A);{struct Cyc_List_List*_tmpE87;Cyc_Toc_result_decls=((_tmpE87=_cycalloc(
sizeof(*_tmpE87)),((_tmpE87->hd=d,((_tmpE87->tl=Cyc_Toc_result_decls,_tmpE87))))));}
goto _LL457;_LL46A: {struct Cyc_Absyn_Typedef_d_struct*_tmpA6B=(struct Cyc_Absyn_Typedef_d_struct*)
_tmpA5C;if(_tmpA6B->tag != 9)goto _LL46C;else{_tmpA6C=_tmpA6B->f1;}}_LL46B: _tmpA6C->name=
_tmpA6C->name;_tmpA6C->tvs=0;if(_tmpA6C->defn != 0){struct Cyc_Core_Opt*_tmpE88;
_tmpA6C->defn=((_tmpE88=_cycalloc(sizeof(*_tmpE88)),((_tmpE88->v=Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA6C->defn))->v),_tmpE88))));}else{
switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmpA6C->kind))->v)->kind){
case Cyc_Absyn_BoxKind: _LL478:{struct Cyc_Core_Opt*_tmpE89;_tmpA6C->defn=((_tmpE89=
_cycalloc(sizeof(*_tmpE89)),((_tmpE89->v=Cyc_Absyn_void_star_typ(),_tmpE89))));}
break;default: _LL479:{struct Cyc_Core_Opt*_tmpE8A;_tmpA6C->defn=((_tmpE8A=
_cycalloc(sizeof(*_tmpE8A)),((_tmpE8A->v=(void*)& Cyc_Absyn_VoidType_val,_tmpE8A))));}
break;}}{struct Cyc_List_List*_tmpE8B;Cyc_Toc_result_decls=((_tmpE8B=_cycalloc(
sizeof(*_tmpE8B)),((_tmpE8B->hd=d,((_tmpE8B->tl=Cyc_Toc_result_decls,_tmpE8B))))));}
goto _LL457;_LL46C: {struct Cyc_Absyn_Porton_d_struct*_tmpA6D=(struct Cyc_Absyn_Porton_d_struct*)
_tmpA5C;if(_tmpA6D->tag != 14)goto _LL46E;}_LL46D: goto _LL46F;_LL46E: {struct Cyc_Absyn_Portoff_d_struct*
_tmpA6E=(struct Cyc_Absyn_Portoff_d_struct*)_tmpA5C;if(_tmpA6E->tag != 15)goto
_LL470;}_LL46F: goto _LL457;_LL470: {struct Cyc_Absyn_Namespace_d_struct*_tmpA6F=(
struct Cyc_Absyn_Namespace_d_struct*)_tmpA5C;if(_tmpA6F->tag != 10)goto _LL472;
else{_tmpA70=_tmpA6F->f2;}}_LL471: _tmpA72=_tmpA70;goto _LL473;_LL472: {struct Cyc_Absyn_Using_d_struct*
_tmpA71=(struct Cyc_Absyn_Using_d_struct*)_tmpA5C;if(_tmpA71->tag != 11)goto _LL474;
else{_tmpA72=_tmpA71->f2;}}_LL473: _tmpA74=_tmpA72;goto _LL475;_LL474: {struct Cyc_Absyn_ExternC_d_struct*
_tmpA73=(struct Cyc_Absyn_ExternC_d_struct*)_tmpA5C;if(_tmpA73->tag != 12)goto
_LL476;else{_tmpA74=_tmpA73->f1;}}_LL475: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA74,top,
cinclude);goto _LL457;_LL476: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA75=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmpA5C;if(_tmpA75->tag != 13)goto _LL457;
else{_tmpA76=_tmpA75->f1;}}_LL477: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA76,top,1);goto
_LL457;_LL457:;};}return nv;}static void Cyc_Toc_init(){struct _DynRegionHandle*
_tmpA8B;struct Cyc_Core_NewRegion _tmpA8A=Cyc_Core__new_dynregion((const char*)"internal-error",
0);_tmpA8B=_tmpA8A.dynregion;{struct _DynRegionFrame _tmpA8C;struct _RegionHandle*d=
_open_dynregion(& _tmpA8C,_tmpA8B);{struct Cyc_Dict_Dict*_tmpE98;struct Cyc_Dict_Dict*
_tmpE97;struct Cyc_Set_Set**_tmpE96;struct Cyc_List_List**_tmpE95;struct Cyc_Dict_Dict*
_tmpE94;struct Cyc_List_List**_tmpE93;struct Cyc_Toc_TocState*_tmpE92;Cyc_Toc_toc_state=((
_tmpE92=_cycalloc(sizeof(*_tmpE92)),((_tmpE92->dyn=(struct _DynRegionHandle*)
_tmpA8B,((_tmpE92->tuple_types=(struct Cyc_List_List**)((_tmpE93=_region_malloc(d,
sizeof(*_tmpE93)),((_tmpE93[0]=0,_tmpE93)))),((_tmpE92->aggrs_so_far=(struct Cyc_Dict_Dict*)((
_tmpE94=_region_malloc(d,sizeof(*_tmpE94)),((_tmpE94[0]=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
d,Cyc_Absyn_qvar_cmp),_tmpE94)))),((_tmpE92->abs_struct_types=(struct Cyc_List_List**)((
_tmpE95=_region_malloc(d,sizeof(*_tmpE95)),((_tmpE95[0]=0,_tmpE95)))),((_tmpE92->datatypes_so_far=(
struct Cyc_Set_Set**)((_tmpE96=_region_malloc(d,sizeof(*_tmpE96)),((_tmpE96[0]=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE96)))),((_tmpE92->xdatatypes_so_far=(
struct Cyc_Dict_Dict*)((_tmpE97=_region_malloc(d,sizeof(*_tmpE97)),((_tmpE97[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE97)))),((_tmpE92->qvar_tags=(
struct Cyc_Dict_Dict*)((_tmpE98=_region_malloc(d,sizeof(*_tmpE98)),((_tmpE98[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple13*,struct
_tuple13*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpE98)))),((_tmpE92->temp_labels=(
struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(
d),_tmpE92))))))))))))))))));}Cyc_Toc_result_decls=0;Cyc_Toc_tuple_type_counter=
0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=0;Cyc_Toc_total_bounds_checks=
0;Cyc_Toc_bounds_checks_eliminated=0;{struct _dyneither_ptr**_tmpE99;Cyc_Toc_globals=
_tag_dyneither(((_tmpE99=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpE99[
0]=& Cyc_Toc__throw_str,((_tmpE99[1]=& Cyc_Toc_setjmp_str,((_tmpE99[2]=& Cyc_Toc__push_handler_str,((
_tmpE99[3]=& Cyc_Toc__pop_handler_str,((_tmpE99[4]=& Cyc_Toc__exn_thrown_str,((
_tmpE99[5]=& Cyc_Toc__npop_handler_str,((_tmpE99[6]=& Cyc_Toc__check_null_str,((
_tmpE99[7]=& Cyc_Toc__check_known_subscript_null_str,((_tmpE99[8]=& Cyc_Toc__check_known_subscript_notnull_str,((
_tmpE99[9]=& Cyc_Toc__check_dyneither_subscript_str,((_tmpE99[10]=& Cyc_Toc__dyneither_ptr_str,((
_tmpE99[11]=& Cyc_Toc__tag_dyneither_str,((_tmpE99[12]=& Cyc_Toc__init_dyneither_ptr_str,((
_tmpE99[13]=& Cyc_Toc__untag_dyneither_ptr_str,((_tmpE99[14]=& Cyc_Toc__get_dyneither_size_str,((
_tmpE99[15]=& Cyc_Toc__get_zero_arr_size_str,((_tmpE99[16]=& Cyc_Toc__dyneither_ptr_plus_str,((
_tmpE99[17]=& Cyc_Toc__zero_arr_plus_str,((_tmpE99[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((
_tmpE99[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpE99[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((
_tmpE99[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpE99[22]=& Cyc_Toc__cycalloc_str,((
_tmpE99[23]=& Cyc_Toc__cyccalloc_str,((_tmpE99[24]=& Cyc_Toc__cycalloc_atomic_str,((
_tmpE99[25]=& Cyc_Toc__cyccalloc_atomic_str,((_tmpE99[26]=& Cyc_Toc__region_malloc_str,((
_tmpE99[27]=& Cyc_Toc__region_calloc_str,((_tmpE99[28]=& Cyc_Toc__check_times_str,((
_tmpE99[29]=& Cyc_Toc__new_region_str,((_tmpE99[30]=& Cyc_Toc__push_region_str,((
_tmpE99[31]=& Cyc_Toc__pop_region_str,((_tmpE99[32]=& Cyc_Toc__open_dynregion_str,((
_tmpE99[33]=& Cyc_Toc__push_dynregion_str,((_tmpE99[34]=& Cyc_Toc__pop_dynregion_str,((
_tmpE99[35]=& Cyc_Toc__reset_region_str,((_tmpE99[36]=& Cyc_Toc__throw_arraybounds_str,((
_tmpE99[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpE99[38]=& Cyc_Toc__throw_match_str,((
_tmpE99[39]=& Cyc_Toc__swap_word_str,((_tmpE99[40]=& Cyc_Toc__swap_dyneither_str,
_tmpE99)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),41);};;_pop_dynregion(d);};}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();{struct _RegionHandle _tmpA95=
_new_region("start");struct _RegionHandle*start=& _tmpA95;_push_region(start);Cyc_Toc_decls_to_c(
start,Cyc_Toc_empty_env(start),ds,1,0);{struct _DynRegionHandle*_tmpA97;struct Cyc_Toc_TocState
_tmpA96=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA97=
_tmpA96.dyn;Cyc_Core_free_dynregion(_tmpA97);};{struct Cyc_List_List*_tmpA98=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);
_npop_handler(0);return _tmpA98;};;_pop_region(start);};}
