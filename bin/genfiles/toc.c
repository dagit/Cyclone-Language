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
ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const char*_tmpA68;void*_tmpA67;(_tmpA67=0,
Cyc_fprintf(Cyc_stderr,((_tmpA68="\n",_tag_dyneither(_tmpA68,sizeof(char),2))),
_tag_dyneither(_tmpA67,sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_struct _tmpA6B;struct Cyc_Toc_Toc_Unimplemented_struct*
_tmpA6A;(int)_throw((void*)((_tmpA6A=_cycalloc_atomic(sizeof(*_tmpA6A)),((
_tmpA6A[0]=((_tmpA6B.tag=Cyc_Toc_Toc_Unimplemented,_tmpA6B)),_tmpA6A)))));};}
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(
Cyc_stderr,fmt,ap);{const char*_tmpA6E;void*_tmpA6D;(_tmpA6D=0,Cyc_fprintf(Cyc_stderr,((
_tmpA6E="\n",_tag_dyneither(_tmpA6E,sizeof(char),2))),_tag_dyneither(_tmpA6D,
sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Impossible_struct
_tmpA71;struct Cyc_Toc_Toc_Impossible_struct*_tmpA70;(int)_throw((void*)((_tmpA70=
_cycalloc_atomic(sizeof(*_tmpA70)),((_tmpA70[0]=((_tmpA71.tag=Cyc_Toc_Toc_Impossible,
_tmpA71)),_tmpA70)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_struct{
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
_tmpA72;skip_stmt_opt=((_tmpA72=_cycalloc(sizeof(*_tmpA72)),((_tmpA72[0]=Cyc_Absyn_skip_stmt(
0),_tmpA72))));}return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(
void*t,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,
0);}static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Cast_e_struct
_tmpA75;struct Cyc_Absyn_Cast_e_struct*_tmpA74;return(void*)((_tmpA74=_cycalloc(
sizeof(*_tmpA74)),((_tmpA74[0]=((_tmpA75.tag=15,((_tmpA75.f1=(void*)t,((_tmpA75.f2=
e,((_tmpA75.f3=0,((_tmpA75.f4=Cyc_Absyn_No_coercion,_tmpA75)))))))))),_tmpA74))));}
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Deref_e_struct
_tmpA78;struct Cyc_Absyn_Deref_e_struct*_tmpA77;return(void*)((_tmpA77=_cycalloc(
sizeof(*_tmpA77)),((_tmpA77[0]=((_tmpA78.tag=21,((_tmpA78.f1=e,_tmpA78)))),
_tmpA77))));}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){struct Cyc_Absyn_Subscript_e_struct _tmpA7B;struct Cyc_Absyn_Subscript_e_struct*
_tmpA7A;return(void*)((_tmpA7A=_cycalloc(sizeof(*_tmpA7A)),((_tmpA7A[0]=((
_tmpA7B.tag=24,((_tmpA7B.f1=e1,((_tmpA7B.f2=e2,_tmpA7B)))))),_tmpA7A))));}static
void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct
_tmpA7E;struct Cyc_Absyn_StmtExp_e_struct*_tmpA7D;return(void*)((_tmpA7D=
_cycalloc(sizeof(*_tmpA7D)),((_tmpA7D[0]=((_tmpA7E.tag=37,((_tmpA7E.f1=s,_tmpA7E)))),
_tmpA7D))));}static void*Cyc_Toc_sizeoftyp_exp_r(void*t){struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpA81;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpA80;return(void*)((_tmpA80=
_cycalloc(sizeof(*_tmpA80)),((_tmpA80[0]=((_tmpA81.tag=18,((_tmpA81.f1=(void*)t,
_tmpA81)))),_tmpA80))));}static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct _tmpA84;struct Cyc_Absyn_FnCall_e_struct*
_tmpA83;return(void*)((_tmpA83=_cycalloc(sizeof(*_tmpA83)),((_tmpA83[0]=((
_tmpA84.tag=11,((_tmpA84.f1=e,((_tmpA84.f2=es,((_tmpA84.f3=0,_tmpA84)))))))),
_tmpA83))));}static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_struct
_tmpA87;struct Cyc_Absyn_Exp_s_struct*_tmpA86;return(void*)((_tmpA86=_cycalloc(
sizeof(*_tmpA86)),((_tmpA86[0]=((_tmpA87.tag=1,((_tmpA87.f1=e,_tmpA87)))),
_tmpA86))));}static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2){struct Cyc_Absyn_Seq_s_struct _tmpA8A;struct Cyc_Absyn_Seq_s_struct*_tmpA89;
return(void*)((_tmpA89=_cycalloc(sizeof(*_tmpA89)),((_tmpA89[0]=((_tmpA8A.tag=2,((
_tmpA8A.f1=s1,((_tmpA8A.f2=s2,_tmpA8A)))))),_tmpA89))));}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct
_tmpA8D;struct Cyc_Absyn_Conditional_e_struct*_tmpA8C;return(void*)((_tmpA8C=
_cycalloc(sizeof(*_tmpA8C)),((_tmpA8C[0]=((_tmpA8D.tag=6,((_tmpA8D.f1=e1,((
_tmpA8D.f2=e2,((_tmpA8D.f3=e3,_tmpA8D)))))))),_tmpA8C))));}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrMember_e_struct
_tmpA90;struct Cyc_Absyn_AggrMember_e_struct*_tmpA8F;return(void*)((_tmpA8F=
_cycalloc(sizeof(*_tmpA8F)),((_tmpA8F[0]=((_tmpA90.tag=22,((_tmpA90.f1=e,((
_tmpA90.f2=n,((_tmpA90.f3=0,((_tmpA90.f4=0,_tmpA90)))))))))),_tmpA8F))));}static
void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_struct _tmpA93;struct Cyc_Absyn_AggrArrow_e_struct*
_tmpA92;return(void*)((_tmpA92=_cycalloc(sizeof(*_tmpA92)),((_tmpA92[0]=((
_tmpA93.tag=23,((_tmpA93.f1=e,((_tmpA93.f2=n,((_tmpA93.f3=0,((_tmpA93.f4=0,
_tmpA93)))))))))),_tmpA92))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmpA96;
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpA95;return(void*)((_tmpA95=_cycalloc(
sizeof(*_tmpA95)),((_tmpA95[0]=((_tmpA96.tag=36,((_tmpA96.f1=tdopt,((_tmpA96.f2=
ds,_tmpA96)))))),_tmpA95))));}static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_struct _tmpA99;
struct Cyc_Absyn_Goto_s_struct*_tmpA98;return(void*)((_tmpA98=_cycalloc(sizeof(*
_tmpA98)),((_tmpA98[0]=((_tmpA99.tag=8,((_tmpA99.f1=v,((_tmpA99.f2=s,_tmpA99)))))),
_tmpA98))));}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,{.Int_c={4,{
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
_tmpA9F;const char*_tmpA9E;struct Cyc_Core_Impossible_struct*_tmpA9D;(int)_throw((
void*)((_tmpA9D=_cycalloc(sizeof(*_tmpA9D)),((_tmpA9D[0]=((_tmpA9F.tag=Cyc_Core_Impossible,((
_tmpA9F.f1=((_tmpA9E="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpA9E,sizeof(char),44))),_tmpA9F)))),_tmpA9D)))));}}goto _LL0;
_LL3: {struct Cyc_Absyn_FloatType_struct*_tmpC9=(struct Cyc_Absyn_FloatType_struct*)
_tmpC6;if(_tmpC9->tag != 7)goto _LL5;}_LL4: function=fS->ffloat;goto _LL0;_LL5: {
struct Cyc_Absyn_DoubleType_struct*_tmpCA=(struct Cyc_Absyn_DoubleType_struct*)
_tmpC6;if(_tmpCA->tag != 8)goto _LL7;else{_tmpCB=_tmpCA->f1;}}_LL6: switch(_tmpCB){
case 1: _LL10: function=fS->flongdouble;break;default: _LL11: function=fS->fdouble;}
goto _LL0;_LL7: {struct Cyc_Absyn_PointerType_struct*_tmpCC=(struct Cyc_Absyn_PointerType_struct*)
_tmpC6;if(_tmpCC->tag != 5)goto _LL9;}_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;
_LLA: {struct Cyc_Core_Impossible_struct _tmpAAC;const char*_tmpAAB;void*_tmpAAA[1];
struct Cyc_String_pa_struct _tmpAA9;struct Cyc_Core_Impossible_struct*_tmpAA8;(int)
_throw((void*)((_tmpAA8=_cycalloc(sizeof(*_tmpAA8)),((_tmpAA8[0]=((_tmpAAC.tag=
Cyc_Core_Impossible,((_tmpAAC.f1=(struct _dyneither_ptr)((_tmpAA9.tag=0,((_tmpAA9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAAA[
0]=& _tmpAA9,Cyc_aprintf(((_tmpAAB="impossible expression type %s (not int, float, double, or pointer)",
_tag_dyneither(_tmpAAB,sizeof(char),67))),_tag_dyneither(_tmpAAA,sizeof(void*),1)))))))),
_tmpAAC)))),_tmpAA8)))));}_LL0:;}return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(
fS,(void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v);}struct Cyc_Absyn_Exp*
Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*
arr){void*_tmpD5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
arr->topt))->v);struct Cyc_Absyn_PtrInfo _tmpD7;void*_tmpD8;_LL14: {struct Cyc_Absyn_PointerType_struct*
_tmpD6=(struct Cyc_Absyn_PointerType_struct*)_tmpD5;if(_tmpD6->tag != 5)goto _LL16;
else{_tmpD7=_tmpD6->f1;_tmpD8=_tmpD7.elt_typ;}}_LL15: return Cyc_Toc_getFunctionType(
fS,_tmpD8);_LL16:;_LL17: {struct Cyc_Core_Impossible_struct _tmpAB2;const char*
_tmpAB1;struct Cyc_Core_Impossible_struct*_tmpAB0;(int)_throw((void*)((_tmpAB0=
_cycalloc(sizeof(*_tmpAB0)),((_tmpAB0[0]=((_tmpAB2.tag=Cyc_Core_Impossible,((
_tmpAB2.f1=((_tmpAB1="impossible type (not pointer)",_tag_dyneither(_tmpAB1,
sizeof(char),30))),_tmpAB2)))),_tmpAB0)))));}_LL13:;}struct _tuple15{struct Cyc_List_List*
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
Cyc_Absyn_conref_def)(0,_tmp100);_LL34:;_LL35: {const char*_tmpAB5;void*_tmpAB4;(
_tmpAB4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAB5="is_nullable",_tag_dyneither(_tmpAB5,sizeof(char),12))),_tag_dyneither(
_tmpAB4,sizeof(void*),0)));}_LL31:;}static char _tmp10F[1]="";static char _tmp110[8]="*bogus*";
static struct _tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct
_dyneither_ptr tag){struct _DynRegionHandle*_tmp104;struct Cyc_Dict_Dict*_tmp105;
struct Cyc_Toc_TocState _tmp103=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp104=_tmp103.dyn;_tmp105=_tmp103.qvar_tags;{static struct _dyneither_ptr
bogus_string={_tmp110,_tmp110,_tmp110 + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
4,0}},& bogus_string};static struct _tuple13 pair={& bogus_qvar,{_tmp10F,_tmp10F,
_tmp10F + 1}};{struct _tuple13 _tmpAB6;pair=((_tmpAB6.f1=x,((_tmpAB6.f2=tag,_tmpAB6))));}{
struct _DynRegionFrame _tmp107;struct _RegionHandle*d=_open_dynregion(& _tmp107,
_tmp104);{struct _tuple0**_tmp108=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,
struct _tuple13*k))Cyc_Dict_lookup_opt)(*_tmp105,(struct _tuple13*)& pair);if(
_tmp108 != 0){struct _tuple0*_tmp109=*_tmp108;_npop_handler(0);return _tmp109;}{
struct _tuple13*_tmpAB7;struct _tuple13*_tmp10A=(_tmpAB7=_cycalloc(sizeof(*_tmpAB7)),((
_tmpAB7->f1=x,((_tmpAB7->f2=tag,_tmpAB7)))));struct _dyneither_ptr*_tmpABA;struct
_tuple0*_tmpAB9;struct _tuple0*res=(_tmpAB9=_cycalloc(sizeof(*_tmpAB9)),((_tmpAB9->f1=(*
x).f1,((_tmpAB9->f2=((_tmpABA=_cycalloc(sizeof(*_tmpABA)),((_tmpABA[0]=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*x).f2,(struct
_dyneither_ptr)tag),_tmpABA)))),_tmpAB9)))));*_tmp105=((struct Cyc_Dict_Dict(*)(
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
_npop_handler(0);return _tmp11B;}};}}{struct Cyc_Int_pa_struct _tmpAC2;void*_tmpAC1[
1];const char*_tmpAC0;struct _dyneither_ptr*_tmpABF;struct _dyneither_ptr*xname=(
_tmpABF=_cycalloc(sizeof(*_tmpABF)),((_tmpABF[0]=(struct _dyneither_ptr)((_tmpAC2.tag=
1,((_tmpAC2.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAC1[0]=& _tmpAC2,
Cyc_aprintf(((_tmpAC0="_tuple%d",_tag_dyneither(_tmpAC0,sizeof(char),9))),
_tag_dyneither(_tmpAC1,sizeof(void*),1)))))))),_tmpABF)));void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp11C=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmpAC5;struct Cyc_List_List*
_tmpAC4;_tmp11C=((_tmpAC4=_cycalloc(sizeof(*_tmpAC4)),((_tmpAC4->hd=((_tmpAC5=
_cycalloc(sizeof(*_tmpAC5)),((_tmpAC5->name=Cyc_Absyn_fieldname(i),((_tmpAC5->tq=
Cyc_Toc_mt_tq,((_tmpAC5->type=(void*)ts2->hd,((_tmpAC5->width=0,((_tmpAC5->attributes=
0,_tmpAC5)))))))))))),((_tmpAC4->tl=_tmp11C,_tmpAC4))))));}}_tmp11C=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp11C);{struct Cyc_Absyn_AggrdeclImpl*
_tmpACA;struct _tuple0*_tmpAC9;struct Cyc_Absyn_Aggrdecl*_tmpAC8;struct Cyc_Absyn_Aggrdecl*
_tmp11F=(_tmpAC8=_cycalloc(sizeof(*_tmpAC8)),((_tmpAC8->kind=Cyc_Absyn_StructA,((
_tmpAC8->sc=Cyc_Absyn_Public,((_tmpAC8->name=((_tmpAC9=_cycalloc(sizeof(*_tmpAC9)),((
_tmpAC9->f1=Cyc_Absyn_Rel_n(0),((_tmpAC9->f2=xname,_tmpAC9)))))),((_tmpAC8->tvs=
0,((_tmpAC8->impl=((_tmpACA=_cycalloc(sizeof(*_tmpACA)),((_tmpACA->exist_vars=0,((
_tmpACA->rgn_po=0,((_tmpACA->fields=_tmp11C,((_tmpACA->tagged=0,_tmpACA)))))))))),((
_tmpAC8->attributes=0,_tmpAC8)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*
_tmpAD0;struct Cyc_Absyn_Aggr_d_struct _tmpACF;struct Cyc_List_List*_tmpACE;Cyc_Toc_result_decls=((
_tmpACE=_cycalloc(sizeof(*_tmpACE)),((_tmpACE->hd=Cyc_Absyn_new_decl((void*)((
_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((_tmpAD0[0]=((_tmpACF.tag=6,((_tmpACF.f1=
_tmp11F,_tmpACF)))),_tmpAD0)))),0),((_tmpACE->tl=Cyc_Toc_result_decls,_tmpACE))))));}{
struct _tuple16*_tmpAD3;struct Cyc_List_List*_tmpAD2;*_tmp113=((_tmpAD2=
_region_malloc(d,sizeof(*_tmpAD2)),((_tmpAD2->hd=((_tmpAD3=_region_malloc(d,
sizeof(*_tmpAD3)),((_tmpAD3->f1=x,((_tmpAD3->f2=ts,_tmpAD3)))))),((_tmpAD2->tl=*
_tmp113,_tmpAD2))))));}{void*_tmp125=x;_npop_handler(0);return _tmp125;};};};;
_pop_dynregion(d);};}struct _tuple0*Cyc_Toc_temp_var(){int _tmp12D=Cyc_Toc_temp_var_counter
++;struct _dyneither_ptr*_tmpAE0;const char*_tmpADF;void*_tmpADE[1];struct Cyc_Int_pa_struct
_tmpADD;struct _tuple0*_tmpADC;struct _tuple0*res=(_tmpADC=_cycalloc(sizeof(*
_tmpADC)),((_tmpADC->f1=Cyc_Absyn_Loc_n,((_tmpADC->f2=((_tmpAE0=_cycalloc(
sizeof(*_tmpAE0)),((_tmpAE0[0]=(struct _dyneither_ptr)((_tmpADD.tag=1,((_tmpADD.f1=(
unsigned int)_tmp12D,((_tmpADE[0]=& _tmpADD,Cyc_aprintf(((_tmpADF="_tmp%X",
_tag_dyneither(_tmpADF,sizeof(char),7))),_tag_dyneither(_tmpADE,sizeof(void*),1)))))))),
_tmpAE0)))),_tmpADC)))));return res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp134;struct Cyc_Xarray_Xarray*_tmp135;struct Cyc_Toc_TocState
_tmp133=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp134=
_tmp133.dyn;_tmp135=_tmp133.temp_labels;{struct _DynRegionFrame _tmp136;struct
_RegionHandle*d=_open_dynregion(& _tmp136,_tmp134);{int _tmp137=Cyc_Toc_fresh_label_counter
++;if(_tmp137 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp135)){
struct _dyneither_ptr*_tmp138=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp135,_tmp137);_npop_handler(0);return _tmp138;}{struct Cyc_Int_pa_struct
_tmpAE8;void*_tmpAE7[1];const char*_tmpAE6;struct _dyneither_ptr*_tmpAE5;struct
_dyneither_ptr*res=(_tmpAE5=_cycalloc(sizeof(*_tmpAE5)),((_tmpAE5[0]=(struct
_dyneither_ptr)((_tmpAE8.tag=1,((_tmpAE8.f1=(unsigned int)_tmp137,((_tmpAE7[0]=&
_tmpAE8,Cyc_aprintf(((_tmpAE6="_LL%X",_tag_dyneither(_tmpAE6,sizeof(char),6))),
_tag_dyneither(_tmpAE7,sizeof(void*),1)))))))),_tmpAE5)));if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp135,res)!= _tmp137){const char*
_tmpAEB;void*_tmpAEA;(_tmpAEA=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAEB="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmpAEB,sizeof(char),43))),_tag_dyneither(_tmpAEA,sizeof(void*),0)));}{
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
_tmp142=_tmp141.f1;_tmp143=_tmp141.f2;_tmp144=_tmp141.f3;{struct _tuple7*_tmpAEC;
return(_tmpAEC=_cycalloc(sizeof(*_tmpAEC)),((_tmpAEC->f1=_tmp142,((_tmpAEC->f2=
_tmp143,((_tmpAEC->f3=Cyc_Toc_typ_to_c(_tmp144),_tmpAEC)))))));};}static struct
_tuple9*Cyc_Toc_typ_to_c_f(struct _tuple9*x){struct Cyc_Absyn_Tqual _tmp147;void*
_tmp148;struct _tuple9 _tmp146=*x;_tmp147=_tmp146.f1;_tmp148=_tmp146.f2;{struct
_tuple9*_tmpAED;return(_tmpAED=_cycalloc(sizeof(*_tmpAED)),((_tmpAED->f1=_tmp147,((
_tmpAED->f2=Cyc_Toc_typ_to_c(_tmp148),_tmpAED)))));};}static void*Cyc_Toc_typ_to_c_array(
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
f){struct Cyc_Absyn_Aggrfield*_tmpAEE;return(_tmpAEE=_cycalloc(sizeof(*_tmpAEE)),((
_tmpAEE->name=f->name,((_tmpAEE->tq=Cyc_Toc_mt_tq,((_tmpAEE->type=Cyc_Toc_typ_to_c(
f->type),((_tmpAEE->width=f->width,((_tmpAEE->attributes=f->attributes,_tmpAEE)))))))))));}
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){return;}static void*Cyc_Toc_char_star_typ(){
static void**cs=0;if(cs == 0){void**_tmpAEF;cs=((_tmpAEF=_cycalloc(sizeof(*_tmpAEF)),((
_tmpAEF[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpAEF))));}return*cs;}static void*Cyc_Toc_rgn_typ(){
static void**r=0;if(r == 0){void**_tmpAF0;r=((_tmpAF0=_cycalloc(sizeof(*_tmpAF0)),((
_tmpAF0[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),
_tmpAF0))));}return*r;}static void*Cyc_Toc_dyn_rgn_typ(){static void**r=0;if(r == 0){
void**_tmpAF1;r=((_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1[0]=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpAF1))));}return*
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
const char*_tmpAF2;return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp16A->name,((
_tmpAF2="_struct",_tag_dyneither(_tmpAF2,sizeof(char),8)))));}_LL4A: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp16B=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp15A;if(_tmp16B->tag != 4)
goto _LL4C;}_LL4B: {const char*_tmpAF5;void*_tmpAF4;(_tmpAF4=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF5="unresolved DatatypeFieldType",
_tag_dyneither(_tmpAF5,sizeof(char),29))),_tag_dyneither(_tmpAF4,sizeof(void*),0)));}
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
_LL8B: continue;_LL8C:;_LL8D:{struct Cyc_List_List*_tmpAF6;_tmp1B6=((_tmpAF6=
_cycalloc(sizeof(*_tmpAF6)),((_tmpAF6->hd=(void*)_tmp181->hd,((_tmpAF6->tl=
_tmp1B6,_tmpAF6))))));}goto _LL81;_LL81:;}{struct Cyc_List_List*_tmp1BE=((struct
Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_arg_to_c,_tmp17E);if(_tmp180 != 0){void*_tmp1BF=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp180->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple7*_tmpAF7;struct _tuple7*_tmp1C0=(_tmpAF7=_cycalloc(sizeof(*_tmpAF7)),((
_tmpAF7->f1=_tmp180->name,((_tmpAF7->f2=_tmp180->tq,((_tmpAF7->f3=_tmp1BF,
_tmpAF7)))))));struct Cyc_List_List*_tmpAF8;_tmp1BE=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1BE,((
_tmpAF8=_cycalloc(sizeof(*_tmpAF8)),((_tmpAF8->hd=_tmp1C0,((_tmpAF8->tl=0,
_tmpAF8)))))));}{struct Cyc_Absyn_FnType_struct _tmpAFE;struct Cyc_Absyn_FnInfo
_tmpAFD;struct Cyc_Absyn_FnType_struct*_tmpAFC;return(void*)((_tmpAFC=_cycalloc(
sizeof(*_tmpAFC)),((_tmpAFC[0]=((_tmpAFE.tag=10,((_tmpAFE.f1=((_tmpAFD.tvars=0,((
_tmpAFD.effect=0,((_tmpAFD.ret_typ=Cyc_Toc_typ_to_c(_tmp17D),((_tmpAFD.args=
_tmp1BE,((_tmpAFD.c_varargs=_tmp17F,((_tmpAFD.cyc_varargs=0,((_tmpAFD.rgn_po=0,((
_tmpAFD.attributes=_tmp1B6,_tmpAFD)))))))))))))))),_tmpAFE)))),_tmpAFC))));};};}
_LL58: {struct Cyc_Absyn_TupleType_struct*_tmp182=(struct Cyc_Absyn_TupleType_struct*)
_tmp15A;if(_tmp182->tag != 11)goto _LL5A;else{_tmp183=_tmp182->f1;}}_LL59: _tmp183=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp183);return Cyc_Toc_add_tuple_type(_tmp183);
_LL5A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp184=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp15A;if(_tmp184->tag != 13)goto _LL5C;else{_tmp185=_tmp184->f1;_tmp186=_tmp184->f2;}}
_LL5B: {struct Cyc_Absyn_AnonAggrType_struct _tmpB01;struct Cyc_Absyn_AnonAggrType_struct*
_tmpB00;return(void*)((_tmpB00=_cycalloc(sizeof(*_tmpB00)),((_tmpB00[0]=((
_tmpB01.tag=13,((_tmpB01.f1=_tmp185,((_tmpB01.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp186),_tmpB01)))))),_tmpB00))));}_LL5C: {
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
if(_tmp191 != 0){struct Cyc_Absyn_TypedefType_struct _tmpB04;struct Cyc_Absyn_TypedefType_struct*
_tmpB03;return(void*)((_tmpB03=_cycalloc(sizeof(*_tmpB03)),((_tmpB03[0]=((
_tmpB04.tag=18,((_tmpB04.f1=_tmp190,((_tmpB04.f2=0,((_tmpB04.f3=_tmp192,((
_tmpB04.f4=0,_tmpB04)))))))))),_tmpB03))));}else{return t;}}else{struct Cyc_Absyn_TypedefType_struct
_tmpB0A;void**_tmpB09;struct Cyc_Absyn_TypedefType_struct*_tmpB08;return(void*)((
_tmpB08=_cycalloc(sizeof(*_tmpB08)),((_tmpB08[0]=((_tmpB0A.tag=18,((_tmpB0A.f1=
_tmp190,((_tmpB0A.f2=0,((_tmpB0A.f3=_tmp192,((_tmpB0A.f4=((_tmpB09=_cycalloc(
sizeof(*_tmpB09)),((_tmpB09[0]=Cyc_Toc_typ_to_c_array(*_tmp193),_tmpB09)))),
_tmpB0A)))))))))),_tmpB08))));}_LL64: {struct Cyc_Absyn_TagType_struct*_tmp194=(
struct Cyc_Absyn_TagType_struct*)_tmp15A;if(_tmp194->tag != 20)goto _LL66;}_LL65:
return Cyc_Absyn_uint_typ;_LL66: {struct Cyc_Absyn_RgnHandleType_struct*_tmp195=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp15A;if(_tmp195->tag != 16)goto _LL68;
else{_tmp196=(void*)_tmp195->f1;}}_LL67: return Cyc_Toc_rgn_typ();_LL68: {struct
Cyc_Absyn_DynRgnType_struct*_tmp197=(struct Cyc_Absyn_DynRgnType_struct*)_tmp15A;
if(_tmp197->tag != 17)goto _LL6A;}_LL69: return Cyc_Toc_dyn_rgn_typ();_LL6A: {struct
Cyc_Absyn_HeapRgn_struct*_tmp198=(struct Cyc_Absyn_HeapRgn_struct*)_tmp15A;if(
_tmp198->tag != 21)goto _LL6C;}_LL6B: {const char*_tmpB0D;void*_tmpB0C;(_tmpB0C=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB0D="Toc::typ_to_c: type translation passed the heap region",_tag_dyneither(
_tmpB0D,sizeof(char),55))),_tag_dyneither(_tmpB0C,sizeof(void*),0)));}_LL6C: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp199=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp15A;if(_tmp199->tag != 22)goto _LL6E;}_LL6D: {const char*_tmpB10;void*_tmpB0F;(
_tmpB0F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB10="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpB10,sizeof(char),57))),_tag_dyneither(_tmpB0F,sizeof(void*),0)));}
_LL6E: {struct Cyc_Absyn_AccessEff_struct*_tmp19A=(struct Cyc_Absyn_AccessEff_struct*)
_tmp15A;if(_tmp19A->tag != 23)goto _LL70;}_LL6F: goto _LL71;_LL70: {struct Cyc_Absyn_JoinEff_struct*
_tmp19B=(struct Cyc_Absyn_JoinEff_struct*)_tmp15A;if(_tmp19B->tag != 24)goto _LL72;}
_LL71: goto _LL73;_LL72: {struct Cyc_Absyn_RgnsEff_struct*_tmp19C=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp15A;if(_tmp19C->tag != 25)goto _LL74;}_LL73: {const char*_tmpB13;void*_tmpB12;(
_tmpB12=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB13="Toc::typ_to_c: type translation passed an effect",_tag_dyneither(
_tmpB13,sizeof(char),49))),_tag_dyneither(_tmpB12,sizeof(void*),0)));}_LL74: {
struct Cyc_Absyn_ValueofType_struct*_tmp19D=(struct Cyc_Absyn_ValueofType_struct*)
_tmp15A;if(_tmp19D->tag != 19)goto _LL76;}_LL75: {const char*_tmpB16;void*_tmpB15;(
_tmpB15=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB16="Toc::typ_to_c: type translation passed a valueof_t",_tag_dyneither(
_tmpB16,sizeof(char),51))),_tag_dyneither(_tmpB15,sizeof(void*),0)));}_LL76: {
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
_LLBD;}_LLBC: return 0;_LLBD:;_LLBE: {const char*_tmpB1A;void*_tmpB19[1];struct Cyc_String_pa_struct
_tmpB18;(_tmpB18.tag=0,((_tmpB18.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpB19[0]=& _tmpB18,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB1A="atomic_typ:  bad type %s",
_tag_dyneither(_tmpB1A,sizeof(char),25))),_tag_dyneither(_tmpB19,sizeof(void*),1)))))));}
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
if(_tmp210->impl == 0){const char*_tmpB1D;void*_tmpB1C;(_tmpB1C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB1D="is_poly_field: type missing fields",
_tag_dyneither(_tmpB1D,sizeof(char),35))),_tag_dyneither(_tmpB1C,sizeof(void*),0)));}
_tmp20F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp210->impl))->fields;goto
_LLD7;}_LLD6: {struct Cyc_Absyn_AnonAggrType_struct*_tmp20E=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp20A;if(_tmp20E->tag != 13)goto _LLD8;else{_tmp20F=_tmp20E->f2;}}_LLD7: {struct
Cyc_Absyn_Aggrfield*_tmp213=Cyc_Absyn_lookup_field(_tmp20F,f);if(_tmp213 == 0){
const char*_tmpB21;void*_tmpB20[1];struct Cyc_String_pa_struct _tmpB1F;(_tmpB1F.tag=
0,((_tmpB1F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB20[0]=&
_tmpB1F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB21="is_poly_field: bad field %s",_tag_dyneither(_tmpB21,sizeof(char),28))),
_tag_dyneither(_tmpB20,sizeof(void*),1)))))));}return Cyc_Toc_is_void_star_or_tvar(
_tmp213->type);}_LLD8:;_LLD9: {const char*_tmpB25;void*_tmpB24[1];struct Cyc_String_pa_struct
_tmpB23;(_tmpB23.tag=0,((_tmpB23.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpB24[0]=& _tmpB23,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB25="is_poly_field: bad type %s",
_tag_dyneither(_tmpB25,sizeof(char),27))),_tag_dyneither(_tmpB24,sizeof(void*),1)))))));}
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
_tmpB29;void*_tmpB28[1];struct Cyc_String_pa_struct _tmpB27;(_tmpB27.tag=0,((
_tmpB27.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp21F->topt))->v)),((_tmpB28[0]=&
_tmpB27,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB29="is_poly_project: bad type %s",_tag_dyneither(_tmpB29,sizeof(char),29))),
_tag_dyneither(_tmpB28,sizeof(void*),1)))))));}_LLE1:;}_LLDF:;_LLE0: return 0;
_LLDA:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB2A;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((
_tmpB2A=_cycalloc(sizeof(*_tmpB2A)),((_tmpB2A->hd=s,((_tmpB2A->tl=0,_tmpB2A)))))),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB2B;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((
_tmpB2B=_cycalloc(sizeof(*_tmpB2B)),((_tmpB2B->hd=s,((_tmpB2B->tl=0,_tmpB2B)))))),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(
s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmpB2C[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((
_tmpB2C[1]=s,((_tmpB2C[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpB2C,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,
struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*
_tmpB2D[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpB2D[1]=n,((
_tmpB2D[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB2D,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{struct Cyc_Absyn_Exp*
_tmpB2E[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpB2E[1]=n,((
_tmpB2E[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB2E,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n){struct Cyc_Absyn_Exp*_tmpB2F[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((
_tmpB2F[2]=n,((_tmpB2F[1]=s,((_tmpB2F[0]=rgn,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB2F,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmpB30;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmpB30=_cycalloc(sizeof(*_tmpB30)),((_tmpB30->hd=e,((_tmpB30->tl=0,_tmpB30)))))),
0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){struct Cyc_Absyn_Exp*
_tmpB31[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpB31,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*
_tmp230=e->r;union Cyc_Absyn_Cnst _tmp232;struct _dyneither_ptr _tmp233;_LLE7: {
struct Cyc_Absyn_Const_e_struct*_tmp231=(struct Cyc_Absyn_Const_e_struct*)_tmp230;
if(_tmp231->tag != 0)goto _LLE9;else{_tmp232=_tmp231->f1;if((_tmp232.String_c).tag
!= 7)goto _LLE9;_tmp233=(struct _dyneither_ptr)(_tmp232.String_c).val;}}_LLE8:
is_string=1;goto _LLE6;_LLE9:;_LLEA: goto _LLE6;_LLE6:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple0*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_struct*_tmpB37;struct
Cyc_Absyn_Var_d_struct _tmpB36;struct Cyc_List_List*_tmpB35;Cyc_Toc_result_decls=((
_tmpB35=_cycalloc(sizeof(*_tmpB35)),((_tmpB35->hd=Cyc_Absyn_new_decl((void*)((
_tmpB37=_cycalloc(sizeof(*_tmpB37)),((_tmpB37[0]=((_tmpB36.tag=0,((_tmpB36.f1=vd,
_tmpB36)))),_tmpB37)))),0),((_tmpB35->tl=Cyc_Toc_result_decls,_tmpB35))))));}
xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*urm_exp;{struct _tuple15*_tmpB3E;
struct _tuple15*_tmpB3D;struct _tuple15*_tmpB3C;struct _tuple15*_tmpB3B[3];urm_exp=
Cyc_Absyn_unresolvedmem_exp(0,((_tmpB3B[2]=((_tmpB3C=_cycalloc(sizeof(*_tmpB3C)),((
_tmpB3C->f1=0,((_tmpB3C->f2=xplussz,_tmpB3C)))))),((_tmpB3B[1]=((_tmpB3D=
_cycalloc(sizeof(*_tmpB3D)),((_tmpB3D->f1=0,((_tmpB3D->f2=xexp,_tmpB3D)))))),((
_tmpB3B[0]=((_tmpB3E=_cycalloc(sizeof(*_tmpB3E)),((_tmpB3E->f1=0,((_tmpB3E->f2=
xexp,_tmpB3E)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB3B,sizeof(struct _tuple15*),3)))))))),0);}return urm_exp;};};}
struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*
binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct
_dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct
_RegionHandle*rgn;};static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env
_tmp23C;int _tmp23D;struct Cyc_Toc_Env*_tmp23B=nv;_tmp23C=*_tmp23B;_tmp23D=_tmp23C.toplevel;
return _tmp23D;}static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env
_tmp23F;int*_tmp240;struct Cyc_Toc_Env*_tmp23E=nv;_tmp23F=*_tmp23E;_tmp240=
_tmp23F.in_lhs;return*_tmp240;}static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(
struct Cyc_Toc_Env*nv,struct _tuple0*x){struct Cyc_Toc_Env _tmp242;struct Cyc_Dict_Dict
_tmp243;struct Cyc_Toc_Env*_tmp241=nv;_tmp242=*_tmp241;_tmp243=_tmp242.varmap;
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(
_tmp243,x);}static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){int*
_tmpB41;struct Cyc_Toc_Env*_tmpB40;return(_tmpB40=_region_malloc(r,sizeof(*
_tmpB40)),((_tmpB40->break_lab=(struct _dyneither_ptr**)0,((_tmpB40->continue_lab=(
struct _dyneither_ptr**)0,((_tmpB40->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((
_tmpB40->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((
_tmpB40->toplevel=(int)1,((_tmpB40->in_lhs=(int*)((_tmpB41=_region_malloc(r,
sizeof(*_tmpB41)),((_tmpB41[0]=0,_tmpB41)))),((_tmpB40->rgn=(struct _RegionHandle*)
r,_tmpB40)))))))))))))));}static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp247;struct
_dyneither_ptr**_tmp248;struct _dyneither_ptr**_tmp249;struct Cyc_Toc_FallthruInfo*
_tmp24A;struct Cyc_Dict_Dict _tmp24B;int _tmp24C;int*_tmp24D;struct Cyc_Toc_Env*
_tmp246=e;_tmp247=*_tmp246;_tmp248=_tmp247.break_lab;_tmp249=_tmp247.continue_lab;
_tmp24A=_tmp247.fallthru_info;_tmp24B=_tmp247.varmap;_tmp24C=_tmp247.toplevel;
_tmp24D=_tmp247.in_lhs;{struct Cyc_Toc_Env*_tmpB42;return(_tmpB42=_region_malloc(
r,sizeof(*_tmpB42)),((_tmpB42->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp248),((_tmpB42->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp249),((_tmpB42->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp24A,((_tmpB42->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp24B),((_tmpB42->toplevel=(
int)_tmp24C,((_tmpB42->in_lhs=(int*)_tmp24D,((_tmpB42->rgn=(struct _RegionHandle*)
r,_tmpB42)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp250;struct
_dyneither_ptr**_tmp251;struct _dyneither_ptr**_tmp252;struct Cyc_Toc_FallthruInfo*
_tmp253;struct Cyc_Dict_Dict _tmp254;int _tmp255;int*_tmp256;struct Cyc_Toc_Env*
_tmp24F=e;_tmp250=*_tmp24F;_tmp251=_tmp250.break_lab;_tmp252=_tmp250.continue_lab;
_tmp253=_tmp250.fallthru_info;_tmp254=_tmp250.varmap;_tmp255=_tmp250.toplevel;
_tmp256=_tmp250.in_lhs;{struct Cyc_Toc_Env*_tmpB43;return(_tmpB43=_region_malloc(
r,sizeof(*_tmpB43)),((_tmpB43->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp251),((_tmpB43->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp252),((_tmpB43->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp253,((_tmpB43->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp254),((_tmpB43->toplevel=(
int)0,((_tmpB43->in_lhs=(int*)_tmp256,((_tmpB43->rgn=(struct _RegionHandle*)r,
_tmpB43)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct
_RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp259;struct
_dyneither_ptr**_tmp25A;struct _dyneither_ptr**_tmp25B;struct Cyc_Toc_FallthruInfo*
_tmp25C;struct Cyc_Dict_Dict _tmp25D;int _tmp25E;int*_tmp25F;struct Cyc_Toc_Env*
_tmp258=e;_tmp259=*_tmp258;_tmp25A=_tmp259.break_lab;_tmp25B=_tmp259.continue_lab;
_tmp25C=_tmp259.fallthru_info;_tmp25D=_tmp259.varmap;_tmp25E=_tmp259.toplevel;
_tmp25F=_tmp259.in_lhs;{struct Cyc_Toc_Env*_tmpB44;return(_tmpB44=_region_malloc(
r,sizeof(*_tmpB44)),((_tmpB44->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp25A),((_tmpB44->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp25B),((_tmpB44->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp25C,((_tmpB44->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp25D),((_tmpB44->toplevel=(
int)1,((_tmpB44->in_lhs=(int*)_tmp25F,((_tmpB44->rgn=(struct _RegionHandle*)r,
_tmpB44)))))))))))))));};}static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env _tmp262;int*_tmp263;struct Cyc_Toc_Env*_tmp261=e;_tmp262=*
_tmp261;_tmp263=_tmp262.in_lhs;*_tmp263=b;}static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace _tmp264=(*x).f1;struct Cyc_List_List*_tmp265;_LLEC: if((
_tmp264.Rel_n).tag != 1)goto _LLEE;_tmp265=(struct Cyc_List_List*)(_tmp264.Rel_n).val;
_LLED: {const char*_tmpB48;void*_tmpB47[1];struct Cyc_String_pa_struct _tmpB46;(
_tmpB46.tag=0,((_tmpB46.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
x)),((_tmpB47[0]=& _tmpB46,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB48="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(
_tmpB48,sizeof(char),30))),_tag_dyneither(_tmpB47,sizeof(void*),1)))))));}_LLEE:;
_LLEF: goto _LLEB;_LLEB:;}{struct Cyc_Toc_Env _tmp26A;struct _dyneither_ptr**_tmp26B;
struct _dyneither_ptr**_tmp26C;struct Cyc_Toc_FallthruInfo*_tmp26D;struct Cyc_Dict_Dict
_tmp26E;int _tmp26F;int*_tmp270;struct Cyc_Toc_Env*_tmp269=e;_tmp26A=*_tmp269;
_tmp26B=_tmp26A.break_lab;_tmp26C=_tmp26A.continue_lab;_tmp26D=_tmp26A.fallthru_info;
_tmp26E=_tmp26A.varmap;_tmp26F=_tmp26A.toplevel;_tmp270=_tmp26A.in_lhs;{struct
Cyc_Dict_Dict _tmp271=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*
k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp26E),x,y);struct Cyc_Toc_Env*
_tmpB49;return(_tmpB49=_region_malloc(r,sizeof(*_tmpB49)),((_tmpB49->break_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp26B),((_tmpB49->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp26C),((_tmpB49->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp26D,((_tmpB49->varmap=(struct Cyc_Dict_Dict)
_tmp271,((_tmpB49->toplevel=(int)_tmp26F,((_tmpB49->in_lhs=(int*)_tmp270,((
_tmpB49->rgn=(struct _RegionHandle*)r,_tmpB49)))))))))))))));};};}static struct Cyc_Toc_Env*
Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp274;struct _dyneither_ptr**_tmp275;struct _dyneither_ptr**_tmp276;struct Cyc_Toc_FallthruInfo*
_tmp277;struct Cyc_Dict_Dict _tmp278;int _tmp279;int*_tmp27A;struct Cyc_Toc_Env*
_tmp273=e;_tmp274=*_tmp273;_tmp275=_tmp274.break_lab;_tmp276=_tmp274.continue_lab;
_tmp277=_tmp274.fallthru_info;_tmp278=_tmp274.varmap;_tmp279=_tmp274.toplevel;
_tmp27A=_tmp274.in_lhs;{struct Cyc_Toc_Env*_tmpB4A;return(_tmpB4A=_region_malloc(
r,sizeof(*_tmpB4A)),((_tmpB4A->break_lab=(struct _dyneither_ptr**)0,((_tmpB4A->continue_lab=(
struct _dyneither_ptr**)0,((_tmpB4A->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp277,((_tmpB4A->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp278),((_tmpB4A->toplevel=(
int)_tmp279,((_tmpB4A->in_lhs=(int*)_tmp27A,((_tmpB4A->rgn=(struct _RegionHandle*)
r,_tmpB4A)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmpB4B;fallthru_vars=((
_tmpB4B=_region_malloc(r,sizeof(*_tmpB4B)),((_tmpB4B->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmpB4B->tl=fallthru_vars,_tmpB4B))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp27E;struct _dyneither_ptr**_tmp27F;struct
_dyneither_ptr**_tmp280;struct Cyc_Toc_FallthruInfo*_tmp281;struct Cyc_Dict_Dict
_tmp282;int _tmp283;int*_tmp284;struct Cyc_Toc_Env*_tmp27D=e;_tmp27E=*_tmp27D;
_tmp27F=_tmp27E.break_lab;_tmp280=_tmp27E.continue_lab;_tmp281=_tmp27E.fallthru_info;
_tmp282=_tmp27E.varmap;_tmp283=_tmp27E.toplevel;_tmp284=_tmp27E.in_lhs;{struct
Cyc_Toc_Env _tmp286;struct Cyc_Dict_Dict _tmp287;struct Cyc_Toc_Env*_tmp285=
next_case_env;_tmp286=*_tmp285;_tmp287=_tmp286.varmap;{struct Cyc_Toc_FallthruInfo*
_tmpB4C;struct Cyc_Toc_FallthruInfo*fi=(_tmpB4C=_region_malloc(r,sizeof(*_tmpB4C)),((
_tmpB4C->label=fallthru_l,((_tmpB4C->binders=fallthru_vars,((_tmpB4C->next_case_env=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp287),_tmpB4C)))))));struct _dyneither_ptr**_tmpB4F;struct Cyc_Toc_Env*
_tmpB4E;return(_tmpB4E=_region_malloc(r,sizeof(*_tmpB4E)),((_tmpB4E->break_lab=(
struct _dyneither_ptr**)((_tmpB4F=_region_malloc(r,sizeof(*_tmpB4F)),((_tmpB4F[0]=
break_l,_tmpB4F)))),((_tmpB4E->continue_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp280),((_tmpB4E->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
fi,((_tmpB4E->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp282),((_tmpB4E->toplevel=(
int)_tmp283,((_tmpB4E->in_lhs=(int*)((int*)_tmp284),((_tmpB4E->rgn=(struct
_RegionHandle*)r,_tmpB4E)))))))))))))));};};};}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){struct
Cyc_Toc_Env _tmp28C;struct _dyneither_ptr**_tmp28D;struct _dyneither_ptr**_tmp28E;
struct Cyc_Toc_FallthruInfo*_tmp28F;struct Cyc_Dict_Dict _tmp290;int _tmp291;int*
_tmp292;struct Cyc_Toc_Env*_tmp28B=e;_tmp28C=*_tmp28B;_tmp28D=_tmp28C.break_lab;
_tmp28E=_tmp28C.continue_lab;_tmp28F=_tmp28C.fallthru_info;_tmp290=_tmp28C.varmap;
_tmp291=_tmp28C.toplevel;_tmp292=_tmp28C.in_lhs;{struct _dyneither_ptr**_tmpB52;
struct Cyc_Toc_Env*_tmpB51;return(_tmpB51=_region_malloc(r,sizeof(*_tmpB51)),((
_tmpB51->break_lab=(struct _dyneither_ptr**)((_tmpB52=_region_malloc(r,sizeof(*
_tmpB52)),((_tmpB52[0]=break_l,_tmpB52)))),((_tmpB51->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp28E),((_tmpB51->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpB51->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp290),((_tmpB51->toplevel=(
int)_tmp291,((_tmpB51->in_lhs=(int*)_tmp292,((_tmpB51->rgn=(struct _RegionHandle*)
r,_tmpB51)))))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp296;struct _dyneither_ptr**_tmp297;struct _dyneither_ptr**_tmp298;struct Cyc_Toc_FallthruInfo*
_tmp299;struct Cyc_Dict_Dict _tmp29A;int _tmp29B;int*_tmp29C;struct Cyc_Toc_Env*
_tmp295=e;_tmp296=*_tmp295;_tmp297=_tmp296.break_lab;_tmp298=_tmp296.continue_lab;
_tmp299=_tmp296.fallthru_info;_tmp29A=_tmp296.varmap;_tmp29B=_tmp296.toplevel;
_tmp29C=_tmp296.in_lhs;{struct Cyc_Toc_FallthruInfo*_tmpB55;struct Cyc_Toc_Env*
_tmpB54;return(_tmpB54=_region_malloc(r,sizeof(*_tmpB54)),((_tmpB54->break_lab=(
struct _dyneither_ptr**)0,((_tmpB54->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp298),((_tmpB54->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((
_tmpB55=_region_malloc(r,sizeof(*_tmpB55)),((_tmpB55->label=next_l,((_tmpB55->binders=
0,((_tmpB55->next_case_env=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*
cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),
_tmpB55)))))))),((_tmpB54->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29A),((_tmpB54->toplevel=(
int)_tmp29B,((_tmpB54->in_lhs=(int*)((int*)_tmp29C),((_tmpB54->rgn=(struct
_RegionHandle*)r,_tmpB54)))))))))))))));};}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp29F=e->annot;_LLF1: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2A0=(
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp29F;if(_tmp2A0->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLF3;}_LLF2: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLF3: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp2A1=(
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp29F;if(_tmp2A1->tag != Cyc_CfFlowInfo_NotZero)
goto _LLF5;}_LLF4: return 0;_LLF5: {struct Cyc_CfFlowInfo_IsZero_struct*_tmp2A2=(
struct Cyc_CfFlowInfo_IsZero_struct*)_tmp29F;if(_tmp2A2->tag != Cyc_CfFlowInfo_IsZero)
goto _LLF7;}_LLF6:{const char*_tmpB58;void*_tmpB57;(_tmpB57=0,Cyc_Tcutil_terr(e->loc,((
_tmpB58="dereference of NULL pointer",_tag_dyneither(_tmpB58,sizeof(char),28))),
_tag_dyneither(_tmpB57,sizeof(void*),0)));}return 0;_LLF7: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp2A3=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp29F;if(_tmp2A3->tag != Cyc_Absyn_EmptyAnnot)
goto _LLF9;}_LLF8: return 0;_LLF9:;_LLFA: {const char*_tmpB5B;void*_tmpB5A;(_tmpB5A=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB5B="need_null_check",_tag_dyneither(_tmpB5B,sizeof(char),16))),
_tag_dyneither(_tmpB5A,sizeof(void*),0)));}_LLF0:;}static struct Cyc_List_List*Cyc_Toc_get_relns(
struct Cyc_Absyn_Exp*e){void*_tmp2A8=e->annot;struct Cyc_List_List*_tmp2AA;struct
Cyc_List_List*_tmp2AC;_LLFC: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2A9=(
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp2A8;if(_tmp2A9->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLFE;else{_tmp2AA=_tmp2A9->f1;}}_LLFD: return _tmp2AA;_LLFE: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmp2AB=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmp2A8;if(_tmp2AB->tag != Cyc_CfFlowInfo_NotZero)
goto _LL100;else{_tmp2AC=_tmp2AB->f1;}}_LLFF: return _tmp2AC;_LL100: {struct Cyc_CfFlowInfo_IsZero_struct*
_tmp2AD=(struct Cyc_CfFlowInfo_IsZero_struct*)_tmp2A8;if(_tmp2AD->tag != Cyc_CfFlowInfo_IsZero)
goto _LL102;}_LL101:{const char*_tmpB5E;void*_tmpB5D;(_tmpB5D=0,Cyc_Tcutil_terr(e->loc,((
_tmpB5E="dereference of NULL pointer",_tag_dyneither(_tmpB5E,sizeof(char),28))),
_tag_dyneither(_tmpB5D,sizeof(void*),0)));}return 0;_LL102: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp2AE=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp2A8;if(_tmp2AE->tag != Cyc_Absyn_EmptyAnnot)
goto _LL104;}_LL103: return 0;_LL104:;_LL105: {const char*_tmpB61;void*_tmpB60;(
_tmpB60=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB61="get_relns",_tag_dyneither(_tmpB61,sizeof(char),10))),_tag_dyneither(
_tmpB60,sizeof(void*),0)));}_LLFB:;}static int Cyc_Toc_check_const_array(
unsigned int i,void*t){void*_tmp2B3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp2B5;struct Cyc_Absyn_PtrAtts _tmp2B6;union Cyc_Absyn_Constraint*_tmp2B7;union
Cyc_Absyn_Constraint*_tmp2B8;struct Cyc_Absyn_ArrayInfo _tmp2BA;struct Cyc_Absyn_Exp*
_tmp2BB;_LL107: {struct Cyc_Absyn_PointerType_struct*_tmp2B4=(struct Cyc_Absyn_PointerType_struct*)
_tmp2B3;if(_tmp2B4->tag != 5)goto _LL109;else{_tmp2B5=_tmp2B4->f1;_tmp2B6=_tmp2B5.ptr_atts;
_tmp2B7=_tmp2B6.bounds;_tmp2B8=_tmp2B6.zero_term;}}_LL108: {void*_tmp2BC=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp2B7);struct Cyc_Absyn_Exp*_tmp2BF;_LL10E: {struct Cyc_Absyn_DynEither_b_struct*
_tmp2BD=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2BC;if(_tmp2BD->tag != 0)goto
_LL110;}_LL10F: return 0;_LL110: {struct Cyc_Absyn_Upper_b_struct*_tmp2BE=(struct
Cyc_Absyn_Upper_b_struct*)_tmp2BC;if(_tmp2BE->tag != 1)goto _LL10D;else{_tmp2BF=
_tmp2BE->f1;}}_LL111: {unsigned int _tmp2C1;int _tmp2C2;struct _tuple10 _tmp2C0=Cyc_Evexp_eval_const_uint_exp(
_tmp2BF);_tmp2C1=_tmp2C0.f1;_tmp2C2=_tmp2C0.f2;return _tmp2C2  && i <= _tmp2C1;}
_LL10D:;}_LL109: {struct Cyc_Absyn_ArrayType_struct*_tmp2B9=(struct Cyc_Absyn_ArrayType_struct*)
_tmp2B3;if(_tmp2B9->tag != 9)goto _LL10B;else{_tmp2BA=_tmp2B9->f1;_tmp2BB=_tmp2BA.num_elts;}}
_LL10A: if(_tmp2BB == 0)return 0;{unsigned int _tmp2C4;int _tmp2C5;struct _tuple10
_tmp2C3=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp2BB);_tmp2C4=
_tmp2C3.f1;_tmp2C5=_tmp2C3.f2;return _tmp2C5  && i <= _tmp2C4;};_LL10B:;_LL10C:
return 0;_LL106:;}static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2C6=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(
_tmp2C6->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp _tmp2C7=_tmp2C6->rop;struct
Cyc_Absyn_Vardecl*_tmp2C8;struct Cyc_Absyn_Vardecl*_tmp2C9;_LL113: if((_tmp2C7.LessNumelts).tag
!= 3)goto _LL115;_tmp2C8=(struct Cyc_Absyn_Vardecl*)(_tmp2C7.LessNumelts).val;
_LL114: _tmp2C9=_tmp2C8;goto _LL116;_LL115: if((_tmp2C7.LessEqNumelts).tag != 5)goto
_LL117;_tmp2C9=(struct Cyc_Absyn_Vardecl*)(_tmp2C7.LessEqNumelts).val;_LL116: if(
_tmp2C9 == v)return 1;else{goto _LL112;}_LL117:;_LL118: continue;_LL112:;};}return 0;}
static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Exp*e,void*vtype){{void*_tmp2CA=e->r;void*_tmp2CC;struct Cyc_Absyn_Vardecl*
_tmp2CE;void*_tmp2D0;struct Cyc_Absyn_Vardecl*_tmp2D2;void*_tmp2D4;struct Cyc_Absyn_Vardecl*
_tmp2D6;void*_tmp2D8;struct Cyc_Absyn_Vardecl*_tmp2DA;union Cyc_Absyn_Cnst _tmp2DC;
struct _tuple5 _tmp2DD;enum Cyc_Absyn_Sign _tmp2DE;int _tmp2DF;union Cyc_Absyn_Cnst
_tmp2E1;struct _tuple5 _tmp2E2;enum Cyc_Absyn_Sign _tmp2E3;int _tmp2E4;union Cyc_Absyn_Cnst
_tmp2E6;struct _tuple5 _tmp2E7;enum Cyc_Absyn_Sign _tmp2E8;int _tmp2E9;enum Cyc_Absyn_Primop
_tmp2EB;struct Cyc_List_List*_tmp2EC;struct Cyc_List_List _tmp2ED;struct Cyc_Absyn_Exp*
_tmp2EE;_LL11A: {struct Cyc_Absyn_Var_e_struct*_tmp2CB=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CA;if(_tmp2CB->tag != 1)goto _LL11C;else{_tmp2CC=(void*)_tmp2CB->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2CD=(struct Cyc_Absyn_Pat_b_struct*)_tmp2CC;if(_tmp2CD->tag
!= 5)goto _LL11C;else{_tmp2CE=_tmp2CD->f1;}};}}_LL11B: _tmp2D2=_tmp2CE;goto _LL11D;
_LL11C: {struct Cyc_Absyn_Var_e_struct*_tmp2CF=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CA;if(_tmp2CF->tag != 1)goto _LL11E;else{_tmp2D0=(void*)_tmp2CF->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2D1=(struct Cyc_Absyn_Local_b_struct*)_tmp2D0;if(
_tmp2D1->tag != 4)goto _LL11E;else{_tmp2D2=_tmp2D1->f1;}};}}_LL11D: _tmp2D6=_tmp2D2;
goto _LL11F;_LL11E: {struct Cyc_Absyn_Var_e_struct*_tmp2D3=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CA;if(_tmp2D3->tag != 1)goto _LL120;else{_tmp2D4=(void*)_tmp2D3->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2D5=(struct Cyc_Absyn_Global_b_struct*)_tmp2D4;if(
_tmp2D5->tag != 1)goto _LL120;else{_tmp2D6=_tmp2D5->f1;}};}}_LL11F: _tmp2DA=_tmp2D6;
goto _LL121;_LL120: {struct Cyc_Absyn_Var_e_struct*_tmp2D7=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CA;if(_tmp2D7->tag != 1)goto _LL122;else{_tmp2D8=(void*)_tmp2D7->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp2D9=(struct Cyc_Absyn_Param_b_struct*)_tmp2D8;if(
_tmp2D9->tag != 3)goto _LL122;else{_tmp2DA=_tmp2D9->f1;}};}}_LL121: if(_tmp2DA->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp2DA))return 1;goto _LL119;_LL122: {
struct Cyc_Absyn_Const_e_struct*_tmp2DB=(struct Cyc_Absyn_Const_e_struct*)_tmp2CA;
if(_tmp2DB->tag != 0)goto _LL124;else{_tmp2DC=_tmp2DB->f1;if((_tmp2DC.Int_c).tag != 
4)goto _LL124;_tmp2DD=(struct _tuple5)(_tmp2DC.Int_c).val;_tmp2DE=_tmp2DD.f1;if(
_tmp2DE != Cyc_Absyn_None)goto _LL124;_tmp2DF=_tmp2DD.f2;}}_LL123: _tmp2E4=_tmp2DF;
goto _LL125;_LL124: {struct Cyc_Absyn_Const_e_struct*_tmp2E0=(struct Cyc_Absyn_Const_e_struct*)
_tmp2CA;if(_tmp2E0->tag != 0)goto _LL126;else{_tmp2E1=_tmp2E0->f1;if((_tmp2E1.Int_c).tag
!= 4)goto _LL126;_tmp2E2=(struct _tuple5)(_tmp2E1.Int_c).val;_tmp2E3=_tmp2E2.f1;
if(_tmp2E3 != Cyc_Absyn_Signed)goto _LL126;_tmp2E4=_tmp2E2.f2;}}_LL125: return
_tmp2E4 >= 0  && Cyc_Toc_check_const_array((unsigned int)_tmp2E4,vtype);_LL126: {
struct Cyc_Absyn_Const_e_struct*_tmp2E5=(struct Cyc_Absyn_Const_e_struct*)_tmp2CA;
if(_tmp2E5->tag != 0)goto _LL128;else{_tmp2E6=_tmp2E5->f1;if((_tmp2E6.Int_c).tag != 
4)goto _LL128;_tmp2E7=(struct _tuple5)(_tmp2E6.Int_c).val;_tmp2E8=_tmp2E7.f1;if(
_tmp2E8 != Cyc_Absyn_Unsigned)goto _LL128;_tmp2E9=_tmp2E7.f2;}}_LL127: return Cyc_Toc_check_const_array((
unsigned int)_tmp2E9,vtype);_LL128: {struct Cyc_Absyn_Primop_e_struct*_tmp2EA=(
struct Cyc_Absyn_Primop_e_struct*)_tmp2CA;if(_tmp2EA->tag != 3)goto _LL12A;else{
_tmp2EB=_tmp2EA->f1;if(_tmp2EB != Cyc_Absyn_Numelts)goto _LL12A;_tmp2EC=_tmp2EA->f2;
if(_tmp2EC == 0)goto _LL12A;_tmp2ED=*_tmp2EC;_tmp2EE=(struct Cyc_Absyn_Exp*)_tmp2ED.hd;}}
_LL129:{void*_tmp2EF=_tmp2EE->r;void*_tmp2F1;struct Cyc_Absyn_Vardecl*_tmp2F3;
void*_tmp2F5;struct Cyc_Absyn_Vardecl*_tmp2F7;void*_tmp2F9;struct Cyc_Absyn_Vardecl*
_tmp2FB;void*_tmp2FD;struct Cyc_Absyn_Vardecl*_tmp2FF;_LL12D: {struct Cyc_Absyn_Var_e_struct*
_tmp2F0=(struct Cyc_Absyn_Var_e_struct*)_tmp2EF;if(_tmp2F0->tag != 1)goto _LL12F;
else{_tmp2F1=(void*)_tmp2F0->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp2F2=(struct Cyc_Absyn_Pat_b_struct*)
_tmp2F1;if(_tmp2F2->tag != 5)goto _LL12F;else{_tmp2F3=_tmp2F2->f1;}};}}_LL12E:
_tmp2F7=_tmp2F3;goto _LL130;_LL12F: {struct Cyc_Absyn_Var_e_struct*_tmp2F4=(struct
Cyc_Absyn_Var_e_struct*)_tmp2EF;if(_tmp2F4->tag != 1)goto _LL131;else{_tmp2F5=(
void*)_tmp2F4->f2;{struct Cyc_Absyn_Local_b_struct*_tmp2F6=(struct Cyc_Absyn_Local_b_struct*)
_tmp2F5;if(_tmp2F6->tag != 4)goto _LL131;else{_tmp2F7=_tmp2F6->f1;}};}}_LL130:
_tmp2FB=_tmp2F7;goto _LL132;_LL131: {struct Cyc_Absyn_Var_e_struct*_tmp2F8=(struct
Cyc_Absyn_Var_e_struct*)_tmp2EF;if(_tmp2F8->tag != 1)goto _LL133;else{_tmp2F9=(
void*)_tmp2F8->f2;{struct Cyc_Absyn_Global_b_struct*_tmp2FA=(struct Cyc_Absyn_Global_b_struct*)
_tmp2F9;if(_tmp2FA->tag != 1)goto _LL133;else{_tmp2FB=_tmp2FA->f1;}};}}_LL132:
_tmp2FF=_tmp2FB;goto _LL134;_LL133: {struct Cyc_Absyn_Var_e_struct*_tmp2FC=(struct
Cyc_Absyn_Var_e_struct*)_tmp2EF;if(_tmp2FC->tag != 1)goto _LL135;else{_tmp2FD=(
void*)_tmp2FC->f2;{struct Cyc_Absyn_Param_b_struct*_tmp2FE=(struct Cyc_Absyn_Param_b_struct*)
_tmp2FD;if(_tmp2FE->tag != 3)goto _LL135;else{_tmp2FF=_tmp2FE->f1;}};}}_LL134:
return _tmp2FF == v;_LL135:;_LL136: goto _LL12C;_LL12C:;}goto _LL119;_LL12A:;_LL12B:
goto _LL119;_LL119:;}return 0;}static char _tmp311[8]="*bogus*";struct _tuple17{void*
f1;void*f2;};static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,
struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){struct Cyc_Absyn_Vardecl*x;{void*
_tmp300=a->r;void*_tmp302;struct Cyc_Absyn_Vardecl*_tmp304;void*_tmp306;struct Cyc_Absyn_Vardecl*
_tmp308;void*_tmp30A;struct Cyc_Absyn_Vardecl*_tmp30C;void*_tmp30E;struct Cyc_Absyn_Vardecl*
_tmp310;_LL138: {struct Cyc_Absyn_Var_e_struct*_tmp301=(struct Cyc_Absyn_Var_e_struct*)
_tmp300;if(_tmp301->tag != 1)goto _LL13A;else{_tmp302=(void*)_tmp301->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp303=(struct Cyc_Absyn_Pat_b_struct*)_tmp302;if(_tmp303->tag
!= 5)goto _LL13A;else{_tmp304=_tmp303->f1;}};}}_LL139: _tmp308=_tmp304;goto _LL13B;
_LL13A: {struct Cyc_Absyn_Var_e_struct*_tmp305=(struct Cyc_Absyn_Var_e_struct*)
_tmp300;if(_tmp305->tag != 1)goto _LL13C;else{_tmp306=(void*)_tmp305->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp307=(struct Cyc_Absyn_Local_b_struct*)_tmp306;if(
_tmp307->tag != 4)goto _LL13C;else{_tmp308=_tmp307->f1;}};}}_LL13B: _tmp30C=_tmp308;
goto _LL13D;_LL13C: {struct Cyc_Absyn_Var_e_struct*_tmp309=(struct Cyc_Absyn_Var_e_struct*)
_tmp300;if(_tmp309->tag != 1)goto _LL13E;else{_tmp30A=(void*)_tmp309->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp30B=(struct Cyc_Absyn_Global_b_struct*)_tmp30A;if(
_tmp30B->tag != 1)goto _LL13E;else{_tmp30C=_tmp30B->f1;}};}}_LL13D: _tmp310=_tmp30C;
goto _LL13F;_LL13E: {struct Cyc_Absyn_Var_e_struct*_tmp30D=(struct Cyc_Absyn_Var_e_struct*)
_tmp300;if(_tmp30D->tag != 1)goto _LL140;else{_tmp30E=(void*)_tmp30D->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp30F=(struct Cyc_Absyn_Param_b_struct*)_tmp30E;if(
_tmp30F->tag != 3)goto _LL140;else{_tmp310=_tmp30F->f1;}};}}_LL13F: if(_tmp310->escapes)
goto _LL141;x=_tmp310;goto _LL137;_LL140:;_LL141: {static struct _dyneither_ptr
bogus_string={_tmp311,_tmp311,_tmp311 + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
4,0}},& bogus_string};static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,&
bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};x=& bogus_vardecl;
x->type=a_typ;}_LL137:;}{void*_tmp312=a_typ;inner_loop: {void*_tmp313=i->r;void*
_tmp315;struct Cyc_Absyn_Exp*_tmp316;union Cyc_Absyn_Cnst _tmp318;struct _tuple5
_tmp319;enum Cyc_Absyn_Sign _tmp31A;int _tmp31B;union Cyc_Absyn_Cnst _tmp31D;struct
_tuple5 _tmp31E;enum Cyc_Absyn_Sign _tmp31F;int _tmp320;union Cyc_Absyn_Cnst _tmp322;
struct _tuple5 _tmp323;enum Cyc_Absyn_Sign _tmp324;int _tmp325;enum Cyc_Absyn_Primop
_tmp327;struct Cyc_List_List*_tmp328;struct Cyc_List_List _tmp329;struct Cyc_Absyn_Exp*
_tmp32A;struct Cyc_List_List*_tmp32B;struct Cyc_List_List _tmp32C;struct Cyc_Absyn_Exp*
_tmp32D;void*_tmp32F;struct Cyc_Absyn_Vardecl*_tmp331;void*_tmp333;struct Cyc_Absyn_Vardecl*
_tmp335;void*_tmp337;struct Cyc_Absyn_Vardecl*_tmp339;void*_tmp33B;struct Cyc_Absyn_Vardecl*
_tmp33D;_LL143: {struct Cyc_Absyn_Cast_e_struct*_tmp314=(struct Cyc_Absyn_Cast_e_struct*)
_tmp313;if(_tmp314->tag != 15)goto _LL145;else{_tmp315=(void*)_tmp314->f1;_tmp316=
_tmp314->f2;}}_LL144: i=_tmp316;goto inner_loop;_LL145: {struct Cyc_Absyn_Const_e_struct*
_tmp317=(struct Cyc_Absyn_Const_e_struct*)_tmp313;if(_tmp317->tag != 0)goto _LL147;
else{_tmp318=_tmp317->f1;if((_tmp318.Int_c).tag != 4)goto _LL147;_tmp319=(struct
_tuple5)(_tmp318.Int_c).val;_tmp31A=_tmp319.f1;if(_tmp31A != Cyc_Absyn_None)goto
_LL147;_tmp31B=_tmp319.f2;}}_LL146: _tmp320=_tmp31B;goto _LL148;_LL147: {struct Cyc_Absyn_Const_e_struct*
_tmp31C=(struct Cyc_Absyn_Const_e_struct*)_tmp313;if(_tmp31C->tag != 0)goto _LL149;
else{_tmp31D=_tmp31C->f1;if((_tmp31D.Int_c).tag != 4)goto _LL149;_tmp31E=(struct
_tuple5)(_tmp31D.Int_c).val;_tmp31F=_tmp31E.f1;if(_tmp31F != Cyc_Absyn_Signed)
goto _LL149;_tmp320=_tmp31E.f2;}}_LL148: return _tmp320 >= 0  && Cyc_Toc_check_const_array((
unsigned int)(_tmp320 + 1),_tmp312);_LL149: {struct Cyc_Absyn_Const_e_struct*
_tmp321=(struct Cyc_Absyn_Const_e_struct*)_tmp313;if(_tmp321->tag != 0)goto _LL14B;
else{_tmp322=_tmp321->f1;if((_tmp322.Int_c).tag != 4)goto _LL14B;_tmp323=(struct
_tuple5)(_tmp322.Int_c).val;_tmp324=_tmp323.f1;if(_tmp324 != Cyc_Absyn_Unsigned)
goto _LL14B;_tmp325=_tmp323.f2;}}_LL14A: return Cyc_Toc_check_const_array((
unsigned int)(_tmp325 + 1),_tmp312);_LL14B: {struct Cyc_Absyn_Primop_e_struct*
_tmp326=(struct Cyc_Absyn_Primop_e_struct*)_tmp313;if(_tmp326->tag != 3)goto _LL14D;
else{_tmp327=_tmp326->f1;if(_tmp327 != Cyc_Absyn_Mod)goto _LL14D;_tmp328=_tmp326->f2;
if(_tmp328 == 0)goto _LL14D;_tmp329=*_tmp328;_tmp32A=(struct Cyc_Absyn_Exp*)_tmp329.hd;
_tmp32B=_tmp329.tl;if(_tmp32B == 0)goto _LL14D;_tmp32C=*_tmp32B;_tmp32D=(struct Cyc_Absyn_Exp*)
_tmp32C.hd;}}_LL14C: return Cyc_Toc_check_leq_size(relns,x,_tmp32D,_tmp312);_LL14D: {
struct Cyc_Absyn_Var_e_struct*_tmp32E=(struct Cyc_Absyn_Var_e_struct*)_tmp313;if(
_tmp32E->tag != 1)goto _LL14F;else{_tmp32F=(void*)_tmp32E->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp330=(struct Cyc_Absyn_Pat_b_struct*)_tmp32F;if(_tmp330->tag != 5)goto _LL14F;
else{_tmp331=_tmp330->f1;}};}}_LL14E: _tmp335=_tmp331;goto _LL150;_LL14F: {struct
Cyc_Absyn_Var_e_struct*_tmp332=(struct Cyc_Absyn_Var_e_struct*)_tmp313;if(_tmp332->tag
!= 1)goto _LL151;else{_tmp333=(void*)_tmp332->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp334=(struct Cyc_Absyn_Local_b_struct*)_tmp333;if(_tmp334->tag != 4)goto _LL151;
else{_tmp335=_tmp334->f1;}};}}_LL150: _tmp339=_tmp335;goto _LL152;_LL151: {struct
Cyc_Absyn_Var_e_struct*_tmp336=(struct Cyc_Absyn_Var_e_struct*)_tmp313;if(_tmp336->tag
!= 1)goto _LL153;else{_tmp337=(void*)_tmp336->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp338=(struct Cyc_Absyn_Global_b_struct*)_tmp337;if(_tmp338->tag != 1)goto _LL153;
else{_tmp339=_tmp338->f1;}};}}_LL152: _tmp33D=_tmp339;goto _LL154;_LL153: {struct
Cyc_Absyn_Var_e_struct*_tmp33A=(struct Cyc_Absyn_Var_e_struct*)_tmp313;if(_tmp33A->tag
!= 1)goto _LL155;else{_tmp33B=(void*)_tmp33A->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp33C=(struct Cyc_Absyn_Param_b_struct*)_tmp33B;if(_tmp33C->tag != 3)goto _LL155;
else{_tmp33D=_tmp33C->f1;}};}}_LL154: if(_tmp33D->escapes)return 0;{struct Cyc_List_List*
_tmp33E=relns;for(0;_tmp33E != 0;_tmp33E=_tmp33E->tl){struct Cyc_CfFlowInfo_Reln*
_tmp33F=(struct Cyc_CfFlowInfo_Reln*)_tmp33E->hd;if(_tmp33F->vd == _tmp33D){union
Cyc_CfFlowInfo_RelnOp _tmp340=_tmp33F->rop;struct Cyc_Absyn_Vardecl*_tmp341;struct
_tuple11 _tmp342;struct Cyc_Absyn_Vardecl*_tmp343;void*_tmp344;unsigned int _tmp345;
unsigned int _tmp346;_LL158: if((_tmp340.LessNumelts).tag != 3)goto _LL15A;_tmp341=(
struct Cyc_Absyn_Vardecl*)(_tmp340.LessNumelts).val;_LL159: if(x == _tmp341)return 1;
else{goto _LL157;}_LL15A: if((_tmp340.LessVar).tag != 2)goto _LL15C;_tmp342=(struct
_tuple11)(_tmp340.LessVar).val;_tmp343=_tmp342.f1;_tmp344=_tmp342.f2;_LL15B:{
struct _tuple17 _tmpB62;struct _tuple17 _tmp348=(_tmpB62.f1=Cyc_Tcutil_compress(
_tmp344),((_tmpB62.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(a->topt))->v),_tmpB62)));void*_tmp349;void*_tmp34B;void*_tmp34C;
struct Cyc_Absyn_PtrInfo _tmp34E;struct Cyc_Absyn_PtrAtts _tmp34F;union Cyc_Absyn_Constraint*
_tmp350;_LL163: _tmp349=_tmp348.f1;{struct Cyc_Absyn_TagType_struct*_tmp34A=(
struct Cyc_Absyn_TagType_struct*)_tmp349;if(_tmp34A->tag != 20)goto _LL165;else{
_tmp34B=(void*)_tmp34A->f1;}};_tmp34C=_tmp348.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp34D=(struct Cyc_Absyn_PointerType_struct*)_tmp34C;if(_tmp34D->tag != 5)goto
_LL165;else{_tmp34E=_tmp34D->f1;_tmp34F=_tmp34E.ptr_atts;_tmp350=_tmp34F.bounds;}};
_LL164:{void*_tmp351=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp350);struct Cyc_Absyn_Exp*_tmp353;_LL168: {struct Cyc_Absyn_Upper_b_struct*
_tmp352=(struct Cyc_Absyn_Upper_b_struct*)_tmp351;if(_tmp352->tag != 1)goto _LL16A;
else{_tmp353=_tmp352->f1;}}_LL169: {struct Cyc_Absyn_Exp*_tmp354=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp34B,0),0,Cyc_Absyn_No_coercion,0);
if(Cyc_Evexp_lte_const_exp(_tmp354,_tmp353))return 1;goto _LL167;}_LL16A:;_LL16B:
goto _LL167;_LL167:;}goto _LL162;_LL165:;_LL166: goto _LL162;_LL162:;}{struct Cyc_List_List*
_tmp355=relns;for(0;_tmp355 != 0;_tmp355=_tmp355->tl){struct Cyc_CfFlowInfo_Reln*
_tmp356=(struct Cyc_CfFlowInfo_Reln*)_tmp355->hd;if(_tmp356->vd == _tmp343){union
Cyc_CfFlowInfo_RelnOp _tmp357=_tmp356->rop;struct Cyc_Absyn_Vardecl*_tmp358;struct
Cyc_Absyn_Vardecl*_tmp359;unsigned int _tmp35A;unsigned int _tmp35B;struct _tuple11
_tmp35C;struct Cyc_Absyn_Vardecl*_tmp35D;_LL16D: if((_tmp357.LessEqNumelts).tag != 
5)goto _LL16F;_tmp358=(struct Cyc_Absyn_Vardecl*)(_tmp357.LessEqNumelts).val;
_LL16E: _tmp359=_tmp358;goto _LL170;_LL16F: if((_tmp357.LessNumelts).tag != 3)goto
_LL171;_tmp359=(struct Cyc_Absyn_Vardecl*)(_tmp357.LessNumelts).val;_LL170: if(x == 
_tmp359)return 1;goto _LL16C;_LL171: if((_tmp357.EqualConst).tag != 1)goto _LL173;
_tmp35A=(unsigned int)(_tmp357.EqualConst).val;_LL172: return Cyc_Toc_check_const_array(
_tmp35A,_tmp312);_LL173: if((_tmp357.LessEqConst).tag != 6)goto _LL175;_tmp35B=(
unsigned int)(_tmp357.LessEqConst).val;if(!(_tmp35B > 0))goto _LL175;_LL174: return
Cyc_Toc_check_const_array(_tmp35B,_tmp312);_LL175: if((_tmp357.LessVar).tag != 2)
goto _LL177;_tmp35C=(struct _tuple11)(_tmp357.LessVar).val;_tmp35D=_tmp35C.f1;
_LL176: if(Cyc_Toc_check_leq_size_var(relns,x,_tmp35D))return 1;goto _LL16C;_LL177:;
_LL178: goto _LL16C;_LL16C:;}}}goto _LL157;_LL15C: if((_tmp340.LessConst).tag != 4)
goto _LL15E;_tmp345=(unsigned int)(_tmp340.LessConst).val;_LL15D: return Cyc_Toc_check_const_array(
_tmp345,_tmp312);_LL15E: if((_tmp340.LessEqConst).tag != 6)goto _LL160;_tmp346=(
unsigned int)(_tmp340.LessEqConst).val;_LL15F: return Cyc_Toc_check_const_array(
_tmp346 + 1,_tmp312);_LL160:;_LL161: goto _LL157;_LL157:;}}}goto _LL142;_LL155:;
_LL156: goto _LL142;_LL142:;}return 0;};}static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*
e){if(e->topt == 0){const char*_tmpB65;void*_tmpB64;(_tmpB64=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB65="Missing type in primop ",
_tag_dyneither(_tmpB65,sizeof(char),24))),_tag_dyneither(_tmpB64,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*
_tmpB68;void*_tmpB67;(_tmpB67=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB68="Missing type in primop ",
_tag_dyneither(_tmpB68,sizeof(char),24))),_tag_dyneither(_tmpB67,sizeof(void*),0)));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple9*
Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){struct _tuple9*_tmpB69;return(_tmpB69=
_cycalloc(sizeof(*_tmpB69)),((_tmpB69->f1=Cyc_Toc_mt_tq,((_tmpB69->f2=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmpB69)))));}static struct
_tuple15*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple15*_tmpB6A;return(_tmpB6A=_cycalloc(sizeof(*_tmpB6A)),((
_tmpB6A->f1=0,((_tmpB6A->f2=e,_tmpB6A)))));};}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(
struct Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int
pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;
if(pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp364=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0  || Cyc_Absyn_no_regions)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(
_tmp364): Cyc_Toc_malloc_ptr(_tmp364));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp364);}};}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp365=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp365 != 0;_tmp365=_tmp365->tl){struct _tuple15 _tmp367;struct Cyc_List_List*
_tmp368;struct Cyc_Absyn_Exp*_tmp369;struct _tuple15*_tmp366=(struct _tuple15*)
_tmp365->hd;_tmp367=*_tmp366;_tmp368=_tmp367.f1;_tmp369=_tmp367.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp368 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp368->tl != 0){const char*_tmpB6D;void*_tmpB6C;(_tmpB6C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB6D="multiple designators in array",
_tag_dyneither(_tmpB6D,sizeof(char),30))),_tag_dyneither(_tmpB6C,sizeof(void*),0)));}{
void*_tmp36C=(void*)_tmp368->hd;void*_tmp36D=_tmp36C;struct Cyc_Absyn_Exp*_tmp36F;
_LL17A: {struct Cyc_Absyn_ArrayElement_struct*_tmp36E=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp36D;if(_tmp36E->tag != 0)goto _LL17C;else{_tmp36F=_tmp36E->f1;}}_LL17B: Cyc_Toc_exp_to_c(
nv,_tmp36F);e_index=_tmp36F;goto _LL179;_LL17C: {struct Cyc_Absyn_FieldName_struct*
_tmp370=(struct Cyc_Absyn_FieldName_struct*)_tmp36D;if(_tmp370->tag != 1)goto
_LL179;}_LL17D: {const char*_tmpB70;void*_tmpB6F;(_tmpB6F=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB70="field name designators in array",
_tag_dyneither(_tmpB70,sizeof(char),32))),_tag_dyneither(_tmpB6F,sizeof(void*),0)));}
_LL179:;};}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*
_tmp373=_tmp369->r;struct Cyc_List_List*_tmp375;struct Cyc_Absyn_Vardecl*_tmp377;
struct Cyc_Absyn_Exp*_tmp378;struct Cyc_Absyn_Exp*_tmp379;int _tmp37A;void*_tmp37C;
struct Cyc_List_List*_tmp37D;_LL17F: {struct Cyc_Absyn_Array_e_struct*_tmp374=(
struct Cyc_Absyn_Array_e_struct*)_tmp373;if(_tmp374->tag != 27)goto _LL181;else{
_tmp375=_tmp374->f1;}}_LL180: s=Cyc_Toc_init_array(nv,lval,_tmp375,s);goto _LL17E;
_LL181: {struct Cyc_Absyn_Comprehension_e_struct*_tmp376=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp373;if(_tmp376->tag != 28)goto _LL183;else{_tmp377=_tmp376->f1;_tmp378=_tmp376->f2;
_tmp379=_tmp376->f3;_tmp37A=_tmp376->f4;}}_LL182: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp377,_tmp378,_tmp379,_tmp37A,s,0);goto _LL17E;_LL183: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp37B=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp373;if(_tmp37B->tag != 30)goto
_LL185;else{_tmp37C=(void*)_tmp37B->f1;_tmp37D=_tmp37B->f2;}}_LL184: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp37C,_tmp37D,s);goto _LL17E;_LL185:;_LL186: Cyc_Toc_exp_to_c(nv,_tmp369);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
_tmp369,0),s,0);goto _LL17E;_LL17E:;};};}}return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple0*_tmp37E=vd->name;void*_tmp37F=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env _tmp381;struct _RegionHandle*_tmp382;
struct Cyc_Toc_Env*_tmp380=nv;_tmp381=*_tmp380;_tmp382=_tmp381.rgn;{struct Cyc_Absyn_Local_b_struct
_tmpB73;struct Cyc_Absyn_Local_b_struct*_tmpB72;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
_tmp382,nv,_tmp37E,Cyc_Absyn_varb_exp(_tmp37E,(void*)((_tmpB72=_cycalloc(sizeof(*
_tmpB72)),((_tmpB72[0]=((_tmpB73.tag=4,((_tmpB73.f1=vd,_tmpB73)))),_tmpB72)))),0));
struct _tuple0*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp37E,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp37E,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp37E,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp37E,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp383=e2->r;struct Cyc_List_List*_tmp385;
struct Cyc_Absyn_Vardecl*_tmp387;struct Cyc_Absyn_Exp*_tmp388;struct Cyc_Absyn_Exp*
_tmp389;int _tmp38A;void*_tmp38C;struct Cyc_List_List*_tmp38D;_LL188: {struct Cyc_Absyn_Array_e_struct*
_tmp384=(struct Cyc_Absyn_Array_e_struct*)_tmp383;if(_tmp384->tag != 27)goto _LL18A;
else{_tmp385=_tmp384->f1;}}_LL189: body=Cyc_Toc_init_array(nv2,lval,_tmp385,Cyc_Toc_skip_stmt_dl());
goto _LL187;_LL18A: {struct Cyc_Absyn_Comprehension_e_struct*_tmp386=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp383;if(_tmp386->tag != 28)goto _LL18C;else{_tmp387=_tmp386->f1;_tmp388=_tmp386->f2;
_tmp389=_tmp386->f3;_tmp38A=_tmp386->f4;}}_LL18B: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp387,_tmp388,_tmp389,_tmp38A,Cyc_Toc_skip_stmt_dl(),0);goto _LL187;
_LL18C: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp38B=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp383;if(_tmp38B->tag != 30)goto _LL18E;else{_tmp38C=(void*)_tmp38B->f1;_tmp38D=
_tmp38B->f2;}}_LL18D: body=Cyc_Toc_init_anon_struct(nv,lval,_tmp38C,_tmp38D,Cyc_Toc_skip_stmt_dl());
goto _LL187;_LL18E:;_LL18F: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL187;_LL187:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp37F,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp37E,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp390=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp390 != 0;_tmp390=_tmp390->tl){struct _tuple15 _tmp392;
struct Cyc_List_List*_tmp393;struct Cyc_Absyn_Exp*_tmp394;struct _tuple15*_tmp391=(
struct _tuple15*)_tmp390->hd;_tmp392=*_tmp391;_tmp393=_tmp392.f1;_tmp394=_tmp392.f2;
if(_tmp393 == 0){const char*_tmpB76;void*_tmpB75;(_tmpB75=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB76="empty designator list",
_tag_dyneither(_tmpB76,sizeof(char),22))),_tag_dyneither(_tmpB75,sizeof(void*),0)));}
if(_tmp393->tl != 0){const char*_tmpB79;void*_tmpB78;(_tmpB78=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB79="too many designators in anonymous struct",
_tag_dyneither(_tmpB79,sizeof(char),41))),_tag_dyneither(_tmpB78,sizeof(void*),0)));}{
void*_tmp399=(void*)_tmp393->hd;struct _dyneither_ptr*_tmp39B;_LL191: {struct Cyc_Absyn_FieldName_struct*
_tmp39A=(struct Cyc_Absyn_FieldName_struct*)_tmp399;if(_tmp39A->tag != 1)goto
_LL193;else{_tmp39B=_tmp39A->f1;}}_LL192: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(
lhs,_tmp39B,0);{void*_tmp39C=_tmp394->r;struct Cyc_List_List*_tmp39E;struct Cyc_Absyn_Vardecl*
_tmp3A0;struct Cyc_Absyn_Exp*_tmp3A1;struct Cyc_Absyn_Exp*_tmp3A2;int _tmp3A3;void*
_tmp3A5;struct Cyc_List_List*_tmp3A6;_LL196: {struct Cyc_Absyn_Array_e_struct*
_tmp39D=(struct Cyc_Absyn_Array_e_struct*)_tmp39C;if(_tmp39D->tag != 27)goto _LL198;
else{_tmp39E=_tmp39D->f1;}}_LL197: s=Cyc_Toc_init_array(nv,lval,_tmp39E,s);goto
_LL195;_LL198: {struct Cyc_Absyn_Comprehension_e_struct*_tmp39F=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp39C;if(_tmp39F->tag != 28)goto _LL19A;else{_tmp3A0=_tmp39F->f1;_tmp3A1=_tmp39F->f2;
_tmp3A2=_tmp39F->f3;_tmp3A3=_tmp39F->f4;}}_LL199: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp3A0,_tmp3A1,_tmp3A2,_tmp3A3,s,0);goto _LL195;_LL19A: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3A4=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp39C;if(_tmp3A4->tag != 30)goto
_LL19C;else{_tmp3A5=(void*)_tmp3A4->f1;_tmp3A6=_tmp3A4->f2;}}_LL19B: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3A5,_tmp3A6,s);goto _LL195;_LL19C:;_LL19D: Cyc_Toc_exp_to_c(nv,_tmp394);
if(Cyc_Toc_is_poly_field(struct_type,_tmp39B))_tmp394=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp394);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp394,0),0),s,0);goto _LL195;_LL195:;}goto _LL190;}_LL193:;_LL194: {const char*
_tmpB7C;void*_tmpB7B;(_tmpB7B=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB7C="array designator in struct",
_tag_dyneither(_tmpB7C,sizeof(char),27))),_tag_dyneither(_tmpB7B,sizeof(void*),0)));}
_LL190:;};}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct Cyc_Toc_Env
_tmp3AA;struct _RegionHandle*_tmp3AB;struct Cyc_Toc_Env*_tmp3A9=nv;_tmp3AA=*
_tmp3A9;_tmp3AB=_tmp3AA.rgn;{struct Cyc_List_List*_tmp3AC=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_rmap)(_tmp3AB,Cyc_Toc_tup_to_c,es);void*_tmp3AD=Cyc_Toc_add_tuple_type(
_tmp3AC);struct _tuple0*_tmp3AE=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp3AF=
Cyc_Absyn_var_exp(_tmp3AE,0);struct Cyc_Absyn_Stmt*_tmp3B0=Cyc_Absyn_exp_stmt(
_tmp3AF,0);struct Cyc_Absyn_Exp*(*_tmp3B1)(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp3B2=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp3AB,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp3B2);for(0;_tmp3B2 != 0;(_tmp3B2=
_tmp3B2->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp3B2->hd;struct
Cyc_Absyn_Exp*lval=_tmp3B1(_tmp3AF,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp3B3=e->r;struct Cyc_List_List*_tmp3B5;struct Cyc_Absyn_Vardecl*_tmp3B7;
struct Cyc_Absyn_Exp*_tmp3B8;struct Cyc_Absyn_Exp*_tmp3B9;int _tmp3BA;_LL19F: {
struct Cyc_Absyn_Array_e_struct*_tmp3B4=(struct Cyc_Absyn_Array_e_struct*)_tmp3B3;
if(_tmp3B4->tag != 27)goto _LL1A1;else{_tmp3B5=_tmp3B4->f1;}}_LL1A0: _tmp3B0=Cyc_Toc_init_array(
nv,lval,_tmp3B5,_tmp3B0);goto _LL19E;_LL1A1: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp3B6=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp3B3;if(_tmp3B6->tag != 28)
goto _LL1A3;else{_tmp3B7=_tmp3B6->f1;_tmp3B8=_tmp3B6->f2;_tmp3B9=_tmp3B6->f3;
_tmp3BA=_tmp3B6->f4;}}_LL1A2: _tmp3B0=Cyc_Toc_init_comprehension(nv,lval,_tmp3B7,
_tmp3B8,_tmp3B9,_tmp3BA,_tmp3B0,0);goto _LL19E;_LL1A3:;_LL1A4: Cyc_Toc_exp_to_c(nv,
e);_tmp3B0=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3B1(
_tmp3AF,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp3B0,0);goto _LL19E;_LL19E:;};}}
return Cyc_Toc_make_struct(nv,_tmp3AE,_tmp3AD,_tmp3B0,pointer,rgnopt,is_atomic);};}
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct
_dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp3BB=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp3BB != 0;_tmp3BB=_tmp3BB->tl){struct Cyc_Absyn_Aggrfield*
_tmp3BC=(struct Cyc_Absyn_Aggrfield*)_tmp3BB->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp3BC->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}{
struct Cyc_String_pa_struct _tmpB84;void*_tmpB83[1];const char*_tmpB82;void*_tmpB81;(
_tmpB81=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)((_tmpB84.tag=0,((_tmpB84.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmpB83[0]=& _tmpB84,Cyc_aprintf(((_tmpB82="get_member_offset %s failed",
_tag_dyneither(_tmpB82,sizeof(char),28))),_tag_dyneither(_tmpB83,sizeof(void*),1)))))))),
_tag_dyneither(_tmpB81,sizeof(void*),0)));};}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){struct _tuple0*_tmp3C1=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3C2=Cyc_Absyn_var_exp(_tmp3C1,0);struct Cyc_Absyn_Stmt*
_tmp3C3=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp3C2),0);struct Cyc_Absyn_Exp*(*
_tmp3C4)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*)=
pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;void*_tmp3C5=Cyc_Toc_aggrdecl_type(
tdn,Cyc_Absyn_strctq);int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp3C6=
Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp3C8;union Cyc_Absyn_AggrInfoU
_tmp3C9;_LL1A6: {struct Cyc_Absyn_AggrType_struct*_tmp3C7=(struct Cyc_Absyn_AggrType_struct*)
_tmp3C6;if(_tmp3C7->tag != 12)goto _LL1A8;else{_tmp3C8=_tmp3C7->f1;_tmp3C9=_tmp3C8.aggr_info;}}
_LL1A7: ad=Cyc_Absyn_get_known_aggrdecl(_tmp3C9);goto _LL1A5;_LL1A8:;_LL1A9: {
const char*_tmpB87;void*_tmpB86;(_tmpB86=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB87="init_struct: bad struct type",
_tag_dyneither(_tmpB87,sizeof(char),29))),_tag_dyneither(_tmpB86,sizeof(void*),0)));}
_LL1A5:;}{int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct Cyc_Toc_Env _tmp3CD;struct _RegionHandle*_tmp3CE;struct Cyc_Toc_Env*_tmp3CC=
nv;_tmp3CD=*_tmp3CC;_tmp3CE=_tmp3CD.rgn;{struct Cyc_List_List*_tmp3CF=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp3CE,dles);for(0;
_tmp3CF != 0;_tmp3CF=_tmp3CF->tl){struct _tuple15 _tmp3D1;struct Cyc_List_List*
_tmp3D2;struct Cyc_Absyn_Exp*_tmp3D3;struct _tuple15*_tmp3D0=(struct _tuple15*)
_tmp3CF->hd;_tmp3D1=*_tmp3D0;_tmp3D2=_tmp3D1.f1;_tmp3D3=_tmp3D1.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3D3->topt))->v);
if(_tmp3D2 == 0){const char*_tmpB8A;void*_tmpB89;(_tmpB89=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8A="empty designator list",
_tag_dyneither(_tmpB8A,sizeof(char),22))),_tag_dyneither(_tmpB89,sizeof(void*),0)));}
if(_tmp3D2->tl != 0){struct _tuple0*_tmp3D6=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp3D7=Cyc_Absyn_var_exp(_tmp3D6,0);for(0;_tmp3D2 != 0;_tmp3D2=_tmp3D2->tl){void*
_tmp3D8=(void*)_tmp3D2->hd;struct _dyneither_ptr*_tmp3DA;_LL1AB: {struct Cyc_Absyn_FieldName_struct*
_tmp3D9=(struct Cyc_Absyn_FieldName_struct*)_tmp3D8;if(_tmp3D9->tag != 1)goto
_LL1AD;else{_tmp3DA=_tmp3D9->f1;}}_LL1AC: if(Cyc_Toc_is_poly_field(struct_type,
_tmp3DA))_tmp3D7=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3D7);_tmp3C3=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3C4(_tmp3C2,_tmp3DA,0),_tmp3D7,0),0),
_tmp3C3,0);goto _LL1AA;_LL1AD:;_LL1AE: {const char*_tmpB8D;void*_tmpB8C;(_tmpB8C=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB8D="array designator in struct",_tag_dyneither(_tmpB8D,sizeof(char),27))),
_tag_dyneither(_tmpB8C,sizeof(void*),0)));}_LL1AA:;}Cyc_Toc_exp_to_c(nv,_tmp3D3);
_tmp3C3=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3D7,
_tmp3D3,0),0),_tmp3C3,0);}else{void*_tmp3DD=(void*)_tmp3D2->hd;struct
_dyneither_ptr*_tmp3DF;_LL1B0: {struct Cyc_Absyn_FieldName_struct*_tmp3DE=(struct
Cyc_Absyn_FieldName_struct*)_tmp3DD;if(_tmp3DE->tag != 1)goto _LL1B2;else{_tmp3DF=
_tmp3DE->f1;}}_LL1B1: {struct Cyc_Absyn_Exp*lval=_tmp3C4(_tmp3C2,_tmp3DF,0);if(
is_tagged_union){int i=Cyc_Toc_get_member_offset(ad,_tmp3DF);struct Cyc_Absyn_Exp*
f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_aggrmember_exp(
lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,
f_tag_exp,0);_tmp3C3=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3C3,0);
lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{void*_tmp3E0=_tmp3D3->r;
struct Cyc_List_List*_tmp3E2;struct Cyc_Absyn_Vardecl*_tmp3E4;struct Cyc_Absyn_Exp*
_tmp3E5;struct Cyc_Absyn_Exp*_tmp3E6;int _tmp3E7;void*_tmp3E9;struct Cyc_List_List*
_tmp3EA;_LL1B5: {struct Cyc_Absyn_Array_e_struct*_tmp3E1=(struct Cyc_Absyn_Array_e_struct*)
_tmp3E0;if(_tmp3E1->tag != 27)goto _LL1B7;else{_tmp3E2=_tmp3E1->f1;}}_LL1B6:
_tmp3C3=Cyc_Toc_init_array(nv,lval,_tmp3E2,_tmp3C3);goto _LL1B4;_LL1B7: {struct
Cyc_Absyn_Comprehension_e_struct*_tmp3E3=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3E0;if(_tmp3E3->tag != 28)goto _LL1B9;else{_tmp3E4=_tmp3E3->f1;_tmp3E5=_tmp3E3->f2;
_tmp3E6=_tmp3E3->f3;_tmp3E7=_tmp3E3->f4;}}_LL1B8: _tmp3C3=Cyc_Toc_init_comprehension(
nv,lval,_tmp3E4,_tmp3E5,_tmp3E6,_tmp3E7,_tmp3C3,0);goto _LL1B4;_LL1B9: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3E8=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3E0;if(_tmp3E8->tag != 30)goto
_LL1BB;else{_tmp3E9=(void*)_tmp3E8->f1;_tmp3EA=_tmp3E8->f2;}}_LL1BA: _tmp3C3=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3E9,_tmp3EA,_tmp3C3);goto _LL1B4;_LL1BB:;_LL1BC: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3D3->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp3D3);{struct Cyc_Absyn_Aggrfield*_tmp3EB=Cyc_Absyn_lookup_decl_field(ad,
_tmp3DF);if(Cyc_Toc_is_poly_field(struct_type,_tmp3DF) && !was_ptr_type)_tmp3D3=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3D3);if(has_exists)_tmp3D3=Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3EB))->type),
_tmp3D3);_tmp3C3=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp3D3,0),0),_tmp3C3,0);goto _LL1B4;};}_LL1B4:;}goto _LL1AF;}_LL1B2:;_LL1B3: {
const char*_tmpB90;void*_tmpB8F;(_tmpB8F=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB90="array designator in struct",
_tag_dyneither(_tmpB90,sizeof(char),27))),_tag_dyneither(_tmpB8F,sizeof(void*),0)));}
_LL1AF:;}}}return Cyc_Toc_make_struct(nv,_tmp3C1,_tmp3C5,_tmp3C3,pointer,rgnopt,
is_atomic);};}struct _tuple18{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct
_tuple18*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(
e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,
enum Cyc_Absyn_Incrementor incr){struct Cyc_Absyn_Increment_e_struct _tmpB93;struct
Cyc_Absyn_Increment_e_struct*_tmpB92;return Cyc_Absyn_new_exp((void*)((_tmpB92=
_cycalloc(sizeof(*_tmpB92)),((_tmpB92[0]=((_tmpB93.tag=5,((_tmpB93.f1=e1,((
_tmpB93.f2=incr,_tmpB93)))))),_tmpB92)))),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp3F0=e1->r;struct Cyc_Absyn_Stmt*_tmp3F2;void*_tmp3F4;struct Cyc_Absyn_Exp*
_tmp3F5;struct Cyc_Absyn_Exp*_tmp3F7;struct _dyneither_ptr*_tmp3F8;int _tmp3F9;int
_tmp3FA;_LL1BE: {struct Cyc_Absyn_StmtExp_e_struct*_tmp3F1=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp3F0;if(_tmp3F1->tag != 37)goto _LL1C0;else{_tmp3F2=_tmp3F1->f1;}}_LL1BF: Cyc_Toc_lvalue_assign_stmt(
_tmp3F2,fs,f,f_env);goto _LL1BD;_LL1C0: {struct Cyc_Absyn_Cast_e_struct*_tmp3F3=(
struct Cyc_Absyn_Cast_e_struct*)_tmp3F0;if(_tmp3F3->tag != 15)goto _LL1C2;else{
_tmp3F4=(void*)_tmp3F3->f1;_tmp3F5=_tmp3F3->f2;}}_LL1C1: Cyc_Toc_lvalue_assign(
_tmp3F5,fs,f,f_env);goto _LL1BD;_LL1C2: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp3F6=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp3F0;if(_tmp3F6->tag != 22)goto
_LL1C4;else{_tmp3F7=_tmp3F6->f1;_tmp3F8=_tmp3F6->f2;_tmp3F9=_tmp3F6->f3;_tmp3FA=
_tmp3F6->f4;}}_LL1C3: e1->r=_tmp3F7->r;{struct Cyc_List_List*_tmpB94;Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)((_tmpB94=_cycalloc(sizeof(*_tmpB94)),((_tmpB94->hd=
_tmp3F8,((_tmpB94->tl=fs,_tmpB94)))))),f,f_env);}goto _LL1BD;_LL1C4:;_LL1C5: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;goto _LL1BD;}_LL1BD:;}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env){void*_tmp3FC=s->r;struct Cyc_Absyn_Exp*_tmp3FE;struct Cyc_Absyn_Decl*
_tmp400;struct Cyc_Absyn_Stmt*_tmp401;struct Cyc_Absyn_Stmt*_tmp403;_LL1C7: {
struct Cyc_Absyn_Exp_s_struct*_tmp3FD=(struct Cyc_Absyn_Exp_s_struct*)_tmp3FC;if(
_tmp3FD->tag != 1)goto _LL1C9;else{_tmp3FE=_tmp3FD->f1;}}_LL1C8: Cyc_Toc_lvalue_assign(
_tmp3FE,fs,f,f_env);goto _LL1C6;_LL1C9: {struct Cyc_Absyn_Decl_s_struct*_tmp3FF=(
struct Cyc_Absyn_Decl_s_struct*)_tmp3FC;if(_tmp3FF->tag != 12)goto _LL1CB;else{
_tmp400=_tmp3FF->f1;_tmp401=_tmp3FF->f2;}}_LL1CA: Cyc_Toc_lvalue_assign_stmt(
_tmp401,fs,f,f_env);goto _LL1C6;_LL1CB: {struct Cyc_Absyn_Seq_s_struct*_tmp402=(
struct Cyc_Absyn_Seq_s_struct*)_tmp3FC;if(_tmp402->tag != 2)goto _LL1CD;else{
_tmp403=_tmp402->f2;}}_LL1CC: Cyc_Toc_lvalue_assign_stmt(_tmp403,fs,f,f_env);goto
_LL1C6;_LL1CD:;_LL1CE: {const char*_tmpB98;void*_tmpB97[1];struct Cyc_String_pa_struct
_tmpB96;(_tmpB96.tag=0,((_tmpB96.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmpB97[0]=& _tmpB96,Cyc_Toc_toc_impos(((_tmpB98="lvalue_assign_stmt: %s",
_tag_dyneither(_tmpB98,sizeof(char),23))),_tag_dyneither(_tmpB97,sizeof(void*),1)))))));}
_LL1C6:;}static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);static
struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){void*_tmp407=
e->r;void*_tmp409;void**_tmp40A;struct Cyc_Absyn_Exp*_tmp40B;struct Cyc_Absyn_Exp**
_tmp40C;struct Cyc_Absyn_Exp*_tmp40E;struct Cyc_Absyn_Stmt*_tmp410;_LL1D0: {struct
Cyc_Absyn_Cast_e_struct*_tmp408=(struct Cyc_Absyn_Cast_e_struct*)_tmp407;if(
_tmp408->tag != 15)goto _LL1D2;else{_tmp409=(void**)& _tmp408->f1;_tmp40A=(void**)((
void**)& _tmp408->f1);_tmp40B=_tmp408->f2;_tmp40C=(struct Cyc_Absyn_Exp**)& _tmp408->f2;}}
_LL1D1:*_tmp40C=Cyc_Toc_push_address_exp(*_tmp40C);*_tmp40A=Cyc_Absyn_cstar_typ(*
_tmp40A,Cyc_Toc_mt_tq);return e;_LL1D2: {struct Cyc_Absyn_Deref_e_struct*_tmp40D=(
struct Cyc_Absyn_Deref_e_struct*)_tmp407;if(_tmp40D->tag != 21)goto _LL1D4;else{
_tmp40E=_tmp40D->f1;}}_LL1D3: return _tmp40E;_LL1D4: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp40F=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp407;if(_tmp40F->tag != 37)goto
_LL1D6;else{_tmp410=_tmp40F->f1;}}_LL1D5: Cyc_Toc_push_address_stmt(_tmp410);
return e;_LL1D6:;_LL1D7: if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpB9C;void*_tmpB9B[1];struct Cyc_String_pa_struct _tmpB9A;(_tmpB9A.tag=
0,((_tmpB9A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpB9B[0]=& _tmpB9A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB9C="can't take & of exp %s",_tag_dyneither(_tmpB9C,
sizeof(char),23))),_tag_dyneither(_tmpB9B,sizeof(void*),1)))))));};_LL1CF:;}
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){void*_tmp414=s->r;
struct Cyc_Absyn_Stmt*_tmp416;struct Cyc_Absyn_Stmt*_tmp418;struct Cyc_Absyn_Exp*
_tmp41A;struct Cyc_Absyn_Exp**_tmp41B;_LL1D9: {struct Cyc_Absyn_Seq_s_struct*
_tmp415=(struct Cyc_Absyn_Seq_s_struct*)_tmp414;if(_tmp415->tag != 2)goto _LL1DB;
else{_tmp416=_tmp415->f2;}}_LL1DA: _tmp418=_tmp416;goto _LL1DC;_LL1DB: {struct Cyc_Absyn_Decl_s_struct*
_tmp417=(struct Cyc_Absyn_Decl_s_struct*)_tmp414;if(_tmp417->tag != 12)goto _LL1DD;
else{_tmp418=_tmp417->f2;}}_LL1DC: Cyc_Toc_push_address_stmt(_tmp418);goto _LL1D8;
_LL1DD: {struct Cyc_Absyn_Exp_s_struct*_tmp419=(struct Cyc_Absyn_Exp_s_struct*)
_tmp414;if(_tmp419->tag != 1)goto _LL1DF;else{_tmp41A=_tmp419->f1;_tmp41B=(struct
Cyc_Absyn_Exp**)& _tmp419->f1;}}_LL1DE:*_tmp41B=Cyc_Toc_push_address_exp(*_tmp41B);
goto _LL1D8;_LL1DF:;_LL1E0: {const char*_tmpBA0;void*_tmpB9F[1];struct Cyc_String_pa_struct
_tmpB9E;(_tmpB9E.tag=0,((_tmpB9E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmpB9F[0]=& _tmpB9E,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA0="can't take & of stmt %s",
_tag_dyneither(_tmpBA0,sizeof(char),24))),_tag_dyneither(_tmpB9F,sizeof(void*),1)))))));}
_LL1D8:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;
struct Cyc_List_List*prev;if(x == 0)return 0;{struct Cyc_List_List*_tmpBA1;result=((
_tmpBA1=_region_malloc(r2,sizeof(*_tmpBA1)),((_tmpBA1->hd=(void*)f((void*)x->hd,
env),((_tmpBA1->tl=0,_tmpBA1))))));}prev=result;for(x=x->tl;x != 0;x=x->tl){{
struct Cyc_List_List*_tmpBA2;((struct Cyc_List_List*)_check_null(prev))->tl=((
_tmpBA2=_region_malloc(r2,sizeof(*_tmpBA2)),((_tmpBA2->hd=(void*)f((void*)x->hd,
env),((_tmpBA2->tl=0,_tmpBA2))))));}prev=prev->tl;}return result;}static struct Cyc_List_List*
Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region,f,env,x);}static struct _tuple15*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*
e){struct _tuple15*_tmpBA3;return(_tmpBA3=_cycalloc(sizeof(*_tmpBA3)),((_tmpBA3->f1=
0,((_tmpBA3->f2=e,_tmpBA3)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(
void*t){void*_tmp422=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp424;
_LL1E2: {struct Cyc_Absyn_PointerType_struct*_tmp423=(struct Cyc_Absyn_PointerType_struct*)
_tmp422;if(_tmp423->tag != 5)goto _LL1E4;else{_tmp424=_tmp423->f1;}}_LL1E3: return
_tmp424;_LL1E4:;_LL1E5: {const char*_tmpBA6;void*_tmpBA5;(_tmpBA5=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA6="get_ptr_typ: not a pointer!",
_tag_dyneither(_tmpBA6,sizeof(char),28))),_tag_dyneither(_tmpBA5,sizeof(void*),0)));}
_LL1E1:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp427=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp42A;enum Cyc_Absyn_Size_of
_tmp42B;enum Cyc_Absyn_Sign _tmp42D;enum Cyc_Absyn_Size_of _tmp42E;enum Cyc_Absyn_Sign
_tmp432;enum Cyc_Absyn_Size_of _tmp433;enum Cyc_Absyn_Sign _tmp435;enum Cyc_Absyn_Size_of
_tmp436;enum Cyc_Absyn_Sign _tmp438;enum Cyc_Absyn_Size_of _tmp439;_LL1E7: {struct
Cyc_Absyn_PointerType_struct*_tmp428=(struct Cyc_Absyn_PointerType_struct*)
_tmp427;if(_tmp428->tag != 5)goto _LL1E9;}_LL1E8: res=Cyc_Absyn_null_exp(0);goto
_LL1E6;_LL1E9: {struct Cyc_Absyn_IntType_struct*_tmp429=(struct Cyc_Absyn_IntType_struct*)
_tmp427;if(_tmp429->tag != 6)goto _LL1EB;else{_tmp42A=_tmp429->f1;_tmp42B=_tmp429->f2;
if(_tmp42B != Cyc_Absyn_Char_sz)goto _LL1EB;}}_LL1EA: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(
_tmp42A,'\000'),0);goto _LL1E6;_LL1EB: {struct Cyc_Absyn_IntType_struct*_tmp42C=(
struct Cyc_Absyn_IntType_struct*)_tmp427;if(_tmp42C->tag != 6)goto _LL1ED;else{
_tmp42D=_tmp42C->f1;_tmp42E=_tmp42C->f2;if(_tmp42E != Cyc_Absyn_Short_sz)goto
_LL1ED;}}_LL1EC: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp42D,0),0);goto
_LL1E6;_LL1ED: {struct Cyc_Absyn_EnumType_struct*_tmp42F=(struct Cyc_Absyn_EnumType_struct*)
_tmp427;if(_tmp42F->tag != 14)goto _LL1EF;}_LL1EE: goto _LL1F0;_LL1EF: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp430=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp427;if(_tmp430->tag != 15)goto
_LL1F1;}_LL1F0: _tmp432=Cyc_Absyn_Unsigned;goto _LL1F2;_LL1F1: {struct Cyc_Absyn_IntType_struct*
_tmp431=(struct Cyc_Absyn_IntType_struct*)_tmp427;if(_tmp431->tag != 6)goto _LL1F3;
else{_tmp432=_tmp431->f1;_tmp433=_tmp431->f2;if(_tmp433 != Cyc_Absyn_Int_sz)goto
_LL1F3;}}_LL1F2: _tmp435=_tmp432;goto _LL1F4;_LL1F3: {struct Cyc_Absyn_IntType_struct*
_tmp434=(struct Cyc_Absyn_IntType_struct*)_tmp427;if(_tmp434->tag != 6)goto _LL1F5;
else{_tmp435=_tmp434->f1;_tmp436=_tmp434->f2;if(_tmp436 != Cyc_Absyn_Long_sz)goto
_LL1F5;}}_LL1F4: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp435,0),0);goto _LL1E6;
_LL1F5: {struct Cyc_Absyn_IntType_struct*_tmp437=(struct Cyc_Absyn_IntType_struct*)
_tmp427;if(_tmp437->tag != 6)goto _LL1F7;else{_tmp438=_tmp437->f1;_tmp439=_tmp437->f2;
if(_tmp439 != Cyc_Absyn_LongLong_sz)goto _LL1F7;}}_LL1F6: res=Cyc_Absyn_const_exp(
Cyc_Absyn_LongLong_c(_tmp438,(long long)0),0);goto _LL1E6;_LL1F7: {struct Cyc_Absyn_FloatType_struct*
_tmp43A=(struct Cyc_Absyn_FloatType_struct*)_tmp427;if(_tmp43A->tag != 7)goto
_LL1F9;}_LL1F8: goto _LL1FA;_LL1F9: {struct Cyc_Absyn_DoubleType_struct*_tmp43B=(
struct Cyc_Absyn_DoubleType_struct*)_tmp427;if(_tmp43B->tag != 8)goto _LL1FB;}
_LL1FA:{const char*_tmpBA7;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBA7="0.0",
_tag_dyneither(_tmpBA7,sizeof(char),4)))),0);}goto _LL1E6;_LL1FB:;_LL1FC: {const
char*_tmpBAB;void*_tmpBAA[1];struct Cyc_String_pa_struct _tmpBA9;(_tmpBA9.tag=0,((
_tmpBA9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmpBAA[0]=& _tmpBA9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBAB="found non-zero type %s in generate_zero",_tag_dyneither(_tmpBAB,sizeof(
char),40))),_tag_dyneither(_tmpBAA,sizeof(void*),1)))))));}_LL1E6:;}{struct Cyc_Core_Opt*
_tmpBAC;res->topt=((_tmpBAC=_cycalloc(sizeof(*_tmpBAC)),((_tmpBAC->v=t,_tmpBAC))));}
return res;}static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dyneither,void*elt_type){void*fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);void*
_tmp441=Cyc_Toc_typ_to_c(elt_type);void*_tmp442=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp443=Cyc_Absyn_cstar_typ(_tmp441,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmpBAD;struct Cyc_Core_Opt*_tmp444=(_tmpBAD=_cycalloc(sizeof(*_tmpBAD)),((
_tmpBAD->v=_tmp443,_tmpBAD)));struct Cyc_Absyn_Exp*xinit;{void*_tmp445=e1->r;
struct Cyc_Absyn_Exp*_tmp447;struct Cyc_Absyn_Exp*_tmp449;struct Cyc_Absyn_Exp*
_tmp44A;_LL1FE: {struct Cyc_Absyn_Deref_e_struct*_tmp446=(struct Cyc_Absyn_Deref_e_struct*)
_tmp445;if(_tmp446->tag != 21)goto _LL200;else{_tmp447=_tmp446->f1;}}_LL1FF: if(!
is_dyneither){_tmp447=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp447,0,Cyc_Absyn_Other_coercion,
0);{struct Cyc_Core_Opt*_tmpBAE;_tmp447->topt=((_tmpBAE=_cycalloc(sizeof(*_tmpBAE)),((
_tmpBAE->v=fat_ptr_type,_tmpBAE))));};}Cyc_Toc_exp_to_c(nv,_tmp447);xinit=
_tmp447;goto _LL1FD;_LL200: {struct Cyc_Absyn_Subscript_e_struct*_tmp448=(struct
Cyc_Absyn_Subscript_e_struct*)_tmp445;if(_tmp448->tag != 24)goto _LL202;else{
_tmp449=_tmp448->f1;_tmp44A=_tmp448->f2;}}_LL201: if(!is_dyneither){_tmp449=Cyc_Absyn_cast_exp(
fat_ptr_type,_tmp449,0,Cyc_Absyn_Other_coercion,0);{struct Cyc_Core_Opt*_tmpBAF;
_tmp449->topt=((_tmpBAF=_cycalloc(sizeof(*_tmpBAF)),((_tmpBAF->v=fat_ptr_type,
_tmpBAF))));};}Cyc_Toc_exp_to_c(nv,_tmp449);Cyc_Toc_exp_to_c(nv,_tmp44A);{struct
Cyc_Absyn_Exp*_tmpBB0[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,((
_tmpBB0[2]=_tmp44A,((_tmpBB0[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmpBB0[0]=_tmp449,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpBB0,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1FD;_LL202:;_LL203: {const char*_tmpBB3;void*_tmpBB2;(_tmpBB2=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB3="found bad lhs for zero-terminated pointer assignment",
_tag_dyneither(_tmpBB3,sizeof(char),53))),_tag_dyneither(_tmpBB2,sizeof(void*),0)));}
_LL1FD:;}{struct _tuple0*_tmp450=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp452;
struct _RegionHandle*_tmp453;struct Cyc_Toc_Env*_tmp451=nv;_tmp452=*_tmp451;
_tmp453=_tmp452.rgn;{struct Cyc_Toc_Env*_tmp454=Cyc_Toc_add_varmap(_tmp453,nv,
_tmp450,Cyc_Absyn_var_exp(_tmp450,0));struct Cyc_Absyn_Vardecl*_tmpBB4;struct Cyc_Absyn_Vardecl*
_tmp455=(_tmpBB4=_cycalloc(sizeof(*_tmpBB4)),((_tmpBB4->sc=Cyc_Absyn_Public,((
_tmpBB4->name=_tmp450,((_tmpBB4->tq=Cyc_Toc_mt_tq,((_tmpBB4->type=_tmp442,((
_tmpBB4->initializer=(struct Cyc_Absyn_Exp*)xinit,((_tmpBB4->rgn=0,((_tmpBB4->attributes=
0,((_tmpBB4->escapes=0,_tmpBB4)))))))))))))))));struct Cyc_Absyn_Local_b_struct
_tmpBB7;struct Cyc_Absyn_Local_b_struct*_tmpBB6;struct Cyc_Absyn_Local_b_struct*
_tmp456=(_tmpBB6=_cycalloc(sizeof(*_tmpBB6)),((_tmpBB6[0]=((_tmpBB7.tag=4,((
_tmpBB7.f1=_tmp455,_tmpBB7)))),_tmpBB6)));struct Cyc_Absyn_Exp*_tmp457=Cyc_Absyn_varb_exp(
_tmp450,(void*)_tmp456,0);{struct Cyc_Core_Opt*_tmpBB8;_tmp457->topt=((_tmpBB8=
_cycalloc(sizeof(*_tmpBB8)),((_tmpBB8->v=fat_ptr_type,_tmpBB8))));}{struct Cyc_Absyn_Exp*
_tmp459=Cyc_Absyn_deref_exp(_tmp457,0);{struct Cyc_Core_Opt*_tmpBB9;_tmp459->topt=((
_tmpBB9=_cycalloc(sizeof(*_tmpBB9)),((_tmpBB9->v=elt_type,_tmpBB9))));}Cyc_Toc_exp_to_c(
_tmp454,_tmp459);{struct _tuple0*_tmp45B=Cyc_Toc_temp_var();_tmp454=Cyc_Toc_add_varmap(
_tmp453,_tmp454,_tmp45B,Cyc_Absyn_var_exp(_tmp45B,0));{struct Cyc_Absyn_Vardecl*
_tmpBBA;struct Cyc_Absyn_Vardecl*_tmp45C=(_tmpBBA=_cycalloc(sizeof(*_tmpBBA)),((
_tmpBBA->sc=Cyc_Absyn_Public,((_tmpBBA->name=_tmp45B,((_tmpBBA->tq=Cyc_Toc_mt_tq,((
_tmpBBA->type=_tmp441,((_tmpBBA->initializer=(struct Cyc_Absyn_Exp*)_tmp459,((
_tmpBBA->rgn=0,((_tmpBBA->attributes=0,((_tmpBBA->escapes=0,_tmpBBA)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpBBD;struct Cyc_Absyn_Local_b_struct*_tmpBBC;
struct Cyc_Absyn_Local_b_struct*_tmp45D=(_tmpBBC=_cycalloc(sizeof(*_tmpBBC)),((
_tmpBBC[0]=((_tmpBBD.tag=4,((_tmpBBD.f1=_tmp45C,_tmpBBD)))),_tmpBBC)));struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp45E=Cyc_Absyn_varb_exp(_tmp45B,(
void*)_tmp45D,0);_tmp45E->topt=_tmp459->topt;z_init=Cyc_Absyn_prim2_exp((enum 
Cyc_Absyn_Primop)popt->v,_tmp45E,e2,0);z_init->topt=_tmp45E->topt;}Cyc_Toc_exp_to_c(
_tmp454,z_init);{struct _tuple0*_tmp45F=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmpBBE;struct Cyc_Absyn_Vardecl*_tmp460=(_tmpBBE=_cycalloc(sizeof(*_tmpBBE)),((
_tmpBBE->sc=Cyc_Absyn_Public,((_tmpBBE->name=_tmp45F,((_tmpBBE->tq=Cyc_Toc_mt_tq,((
_tmpBBE->type=_tmp441,((_tmpBBE->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmpBBE->rgn=0,((_tmpBBE->attributes=0,((_tmpBBE->escapes=0,_tmpBBE)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpBC1;struct Cyc_Absyn_Local_b_struct*_tmpBC0;
struct Cyc_Absyn_Local_b_struct*_tmp461=(_tmpBC0=_cycalloc(sizeof(*_tmpBC0)),((
_tmpBC0[0]=((_tmpBC1.tag=4,((_tmpBC1.f1=_tmp460,_tmpBC1)))),_tmpBC0)));_tmp454=
Cyc_Toc_add_varmap(_tmp453,_tmp454,_tmp45F,Cyc_Absyn_var_exp(_tmp45F,0));{struct
Cyc_Absyn_Exp*_tmp462=Cyc_Absyn_varb_exp(_tmp45B,(void*)_tmp45D,0);_tmp462->topt=
_tmp459->topt;{struct Cyc_Absyn_Exp*_tmp463=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp464=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp462,_tmp463,0);{
struct Cyc_Core_Opt*_tmpBC2;_tmp464->topt=((_tmpBC2=_cycalloc(sizeof(*_tmpBC2)),((
_tmpBC2->v=Cyc_Absyn_sint_typ,_tmpBC2))));}Cyc_Toc_exp_to_c(_tmp454,_tmp464);{
struct Cyc_Absyn_Exp*_tmp466=Cyc_Absyn_varb_exp(_tmp45F,(void*)_tmp461,0);_tmp466->topt=
_tmp459->topt;{struct Cyc_Absyn_Exp*_tmp467=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp468=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp466,_tmp467,0);{
struct Cyc_Core_Opt*_tmpBC3;_tmp468->topt=((_tmpBC3=_cycalloc(sizeof(*_tmpBC3)),((
_tmpBC3->v=Cyc_Absyn_sint_typ,_tmpBC3))));}Cyc_Toc_exp_to_c(_tmp454,_tmp468);{
struct Cyc_Absyn_Exp*_tmpBC4[2];struct Cyc_List_List*_tmp46A=(_tmpBC4[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0),((_tmpBC4[0]=Cyc_Absyn_varb_exp(_tmp450,(void*)
_tmp456,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBC4,sizeof(struct Cyc_Absyn_Exp*),2)))));struct Cyc_Absyn_Exp*
_tmp46B=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;xsize=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp46A,0),
_tmp46B,0);{struct Cyc_Absyn_Exp*_tmp46C=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp464,_tmp468,0),0);struct Cyc_Absyn_Stmt*_tmp46D=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp46E=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp450,(void*)_tmp456,0),Cyc_Toc_curr_sp,0);_tmp46E=Cyc_Toc_cast_it(
_tmp443,_tmp46E);{struct Cyc_Absyn_Exp*_tmp46F=Cyc_Absyn_deref_exp(_tmp46E,0);
struct Cyc_Absyn_Exp*_tmp470=Cyc_Absyn_assign_exp(_tmp46F,Cyc_Absyn_var_exp(
_tmp45F,0),0);struct Cyc_Absyn_Stmt*_tmp471=Cyc_Absyn_exp_stmt(_tmp470,0);_tmp471=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp46C,_tmp46D,Cyc_Absyn_skip_stmt(
0),0),_tmp471,0);{struct Cyc_Absyn_Var_d_struct*_tmpBCA;struct Cyc_Absyn_Var_d_struct
_tmpBC9;struct Cyc_Absyn_Decl*_tmpBC8;_tmp471=Cyc_Absyn_decl_stmt(((_tmpBC8=
_cycalloc(sizeof(*_tmpBC8)),((_tmpBC8->r=(void*)((_tmpBCA=_cycalloc(sizeof(*
_tmpBCA)),((_tmpBCA[0]=((_tmpBC9.tag=0,((_tmpBC9.f1=_tmp460,_tmpBC9)))),_tmpBCA)))),((
_tmpBC8->loc=0,_tmpBC8)))))),_tmp471,0);}{struct Cyc_Absyn_Var_d_struct*_tmpBD0;
struct Cyc_Absyn_Var_d_struct _tmpBCF;struct Cyc_Absyn_Decl*_tmpBCE;_tmp471=Cyc_Absyn_decl_stmt(((
_tmpBCE=_cycalloc(sizeof(*_tmpBCE)),((_tmpBCE->r=(void*)((_tmpBD0=_cycalloc(
sizeof(*_tmpBD0)),((_tmpBD0[0]=((_tmpBCF.tag=0,((_tmpBCF.f1=_tmp45C,_tmpBCF)))),
_tmpBD0)))),((_tmpBCE->loc=0,_tmpBCE)))))),_tmp471,0);}{struct Cyc_Absyn_Var_d_struct*
_tmpBD6;struct Cyc_Absyn_Var_d_struct _tmpBD5;struct Cyc_Absyn_Decl*_tmpBD4;_tmp471=
Cyc_Absyn_decl_stmt(((_tmpBD4=_cycalloc(sizeof(*_tmpBD4)),((_tmpBD4->r=(void*)((
_tmpBD6=_cycalloc(sizeof(*_tmpBD6)),((_tmpBD6[0]=((_tmpBD5.tag=0,((_tmpBD5.f1=
_tmp455,_tmpBD5)))),_tmpBD6)))),((_tmpBD4->loc=0,_tmpBD4)))))),_tmp471,0);}e->r=
Cyc_Toc_stmt_exp_r(_tmp471);};};};};};};};};};};};};};}static void*Cyc_Toc_check_tagged_union(
struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int
in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,
struct Cyc_Position_Segment*)){struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp486=Cyc_Tcutil_compress(
aggrtype);struct Cyc_Absyn_AggrInfo _tmp488;union Cyc_Absyn_AggrInfoU _tmp489;_LL205: {
struct Cyc_Absyn_AggrType_struct*_tmp487=(struct Cyc_Absyn_AggrType_struct*)
_tmp486;if(_tmp487->tag != 12)goto _LL207;else{_tmp488=_tmp487->f1;_tmp489=_tmp488.aggr_info;}}
_LL206: ad=Cyc_Absyn_get_known_aggrdecl(_tmp489);goto _LL204;_LL207:;_LL208: {
struct Cyc_String_pa_struct _tmpBDE;void*_tmpBDD[1];const char*_tmpBDC;void*_tmpBDB;(
_tmpBDB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((
struct _dyneither_ptr)((_tmpBDE.tag=0,((_tmpBDE.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpBDD[0]=& _tmpBDE,Cyc_aprintf(((
_tmpBDC="expecting union but found %s in check_tagged_union",_tag_dyneither(
_tmpBDC,sizeof(char),51))),_tag_dyneither(_tmpBDD,sizeof(void*),1)))))))),
_tag_dyneither(_tmpBDB,sizeof(void*),0)));}_LL204:;}{struct _tuple0*_tmp48E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp48F=Cyc_Absyn_var_exp(_tmp48E,0);struct Cyc_Absyn_Exp*
_tmp490=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);if(in_lhs){
struct Cyc_Absyn_Exp*_tmp491=Cyc_Absyn_aggrarrow_exp(_tmp48F,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp492=Cyc_Absyn_neq_exp(_tmp491,_tmp490,0);struct Cyc_Absyn_Exp*
_tmp493=Cyc_Absyn_aggrarrow_exp(_tmp48F,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Stmt*
_tmp494=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp493,0),0);struct Cyc_Absyn_Stmt*
_tmp495=Cyc_Absyn_ifthenelse_stmt(_tmp492,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);void*_tmp496=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp497=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);struct Cyc_Absyn_Stmt*_tmp498=
Cyc_Absyn_declare_stmt(_tmp48E,_tmp496,(struct Cyc_Absyn_Exp*)_tmp497,Cyc_Absyn_seq_stmt(
_tmp495,_tmp494,0),0);return Cyc_Toc_stmt_exp_r(_tmp498);}else{struct Cyc_Absyn_Exp*
_tmp499=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp48F,f,0),Cyc_Toc_tag_sp,0);struct
Cyc_Absyn_Exp*_tmp49A=Cyc_Absyn_neq_exp(_tmp499,_tmp490,0);struct Cyc_Absyn_Exp*
_tmp49B=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp48F,f,0),Cyc_Toc_val_sp,0);struct
Cyc_Absyn_Stmt*_tmp49C=Cyc_Absyn_exp_stmt(_tmp49B,0);struct Cyc_Absyn_Stmt*
_tmp49D=Cyc_Absyn_ifthenelse_stmt(_tmp49A,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp49E=Cyc_Absyn_declare_stmt(_tmp48E,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp49D,_tmp49C,0),0);return Cyc_Toc_stmt_exp_r(
_tmp49E);}};}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read){void*_tmp49F=e->r;struct Cyc_Absyn_Exp*
_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A3;struct _dyneither_ptr*_tmp4A4;int _tmp4A5;int*
_tmp4A6;struct Cyc_Absyn_Exp*_tmp4A8;struct _dyneither_ptr*_tmp4A9;int _tmp4AA;int*
_tmp4AB;_LL20A: {struct Cyc_Absyn_Cast_e_struct*_tmp4A0=(struct Cyc_Absyn_Cast_e_struct*)
_tmp49F;if(_tmp4A0->tag != 15)goto _LL20C;else{_tmp4A1=_tmp4A0->f2;}}_LL20B: {
const char*_tmpBE1;void*_tmpBE0;(_tmpBE0=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE1="cast on lhs!",
_tag_dyneither(_tmpBE1,sizeof(char),13))),_tag_dyneither(_tmpBE0,sizeof(void*),0)));}
_LL20C: {struct Cyc_Absyn_AggrMember_e_struct*_tmp4A2=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp49F;if(_tmp4A2->tag != 22)goto _LL20E;else{_tmp4A3=_tmp4A2->f1;_tmp4A4=_tmp4A2->f2;
_tmp4A5=_tmp4A2->f4;_tmp4A6=(int*)& _tmp4A2->f4;}}_LL20D: {void*_tmp4AE=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4A3->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp4B0;union Cyc_Absyn_AggrInfoU _tmp4B1;_LL213: {struct Cyc_Absyn_AggrType_struct*
_tmp4AF=(struct Cyc_Absyn_AggrType_struct*)_tmp4AE;if(_tmp4AF->tag != 12)goto
_LL215;else{_tmp4B0=_tmp4AF->f1;_tmp4B1=_tmp4B0.aggr_info;}}_LL214: {struct Cyc_Absyn_Aggrdecl*
_tmp4B2=Cyc_Absyn_get_known_aggrdecl(_tmp4B1);*f_tag=Cyc_Toc_get_member_offset(
_tmp4B2,_tmp4A4);{const char*_tmpBE6;void*_tmpBE5[2];struct Cyc_String_pa_struct
_tmpBE4;struct Cyc_String_pa_struct _tmpBE3;struct _dyneither_ptr str=(struct
_dyneither_ptr)((_tmpBE3.tag=0,((_tmpBE3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp4A4),((_tmpBE4.tag=0,((_tmpBE4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp4B2->name).f2),((_tmpBE5[0]=& _tmpBE4,((_tmpBE5[1]=&
_tmpBE3,Cyc_aprintf(((_tmpBE6="_union_%s_%s",_tag_dyneither(_tmpBE6,sizeof(char),
13))),_tag_dyneither(_tmpBE5,sizeof(void*),2))))))))))))));{struct _dyneither_ptr*
_tmpBE7;*tagged_member_type=Cyc_Absyn_strct(((_tmpBE7=_cycalloc(sizeof(*_tmpBE7)),((
_tmpBE7[0]=str,_tmpBE7)))));}if(clear_read)*_tmp4A6=0;return((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4B2->impl))->tagged;};}_LL215:;_LL216: return 0;_LL212:;}_LL20E: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp4A7=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp49F;if(_tmp4A7->tag != 23)goto _LL210;else{_tmp4A8=_tmp4A7->f1;_tmp4A9=_tmp4A7->f2;
_tmp4AA=_tmp4A7->f4;_tmp4AB=(int*)& _tmp4A7->f4;}}_LL20F: {void*_tmp4B8=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4A8->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp4BA;void*_tmp4BB;_LL218: {struct Cyc_Absyn_PointerType_struct*_tmp4B9=(struct
Cyc_Absyn_PointerType_struct*)_tmp4B8;if(_tmp4B9->tag != 5)goto _LL21A;else{
_tmp4BA=_tmp4B9->f1;_tmp4BB=_tmp4BA.elt_typ;}}_LL219: {void*_tmp4BC=Cyc_Tcutil_compress(
_tmp4BB);struct Cyc_Absyn_AggrInfo _tmp4BE;union Cyc_Absyn_AggrInfoU _tmp4BF;_LL21D: {
struct Cyc_Absyn_AggrType_struct*_tmp4BD=(struct Cyc_Absyn_AggrType_struct*)
_tmp4BC;if(_tmp4BD->tag != 12)goto _LL21F;else{_tmp4BE=_tmp4BD->f1;_tmp4BF=_tmp4BE.aggr_info;}}
_LL21E: {struct Cyc_Absyn_Aggrdecl*_tmp4C0=Cyc_Absyn_get_known_aggrdecl(_tmp4BF);*
f_tag=Cyc_Toc_get_member_offset(_tmp4C0,_tmp4A9);{const char*_tmpBEC;void*_tmpBEB[
2];struct Cyc_String_pa_struct _tmpBEA;struct Cyc_String_pa_struct _tmpBE9;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpBE9.tag=0,((_tmpBE9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp4A9),((_tmpBEA.tag=0,((_tmpBEA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4C0->name).f2),((_tmpBEB[0]=&
_tmpBEA,((_tmpBEB[1]=& _tmpBE9,Cyc_aprintf(((_tmpBEC="_union_%s_%s",
_tag_dyneither(_tmpBEC,sizeof(char),13))),_tag_dyneither(_tmpBEB,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpBED;*tagged_member_type=Cyc_Absyn_strct(((_tmpBED=
_cycalloc(sizeof(*_tmpBED)),((_tmpBED[0]=str,_tmpBED)))));}if(clear_read)*
_tmp4AB=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4C0->impl))->tagged;};}
_LL21F:;_LL220: return 0;_LL21C:;}_LL21A:;_LL21B: return 0;_LL217:;}_LL210:;_LL211:
return 0;_LL209:;}void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*
e,int tag,void*mem_type){struct _tuple0*_tmp4C6=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
temp_exp=Cyc_Absyn_var_exp(_tmp4C6,0);struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(
tag,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,
0),0);struct Cyc_Absyn_Exp*_tmp4C7=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_ifthenelse_stmt(_tmp4C7,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4C6,Cyc_Absyn_cstar_typ(
mem_type,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(
e)),Cyc_Absyn_seq_stmt(s2,s3,0),0);e->r=Cyc_Toc_stmt_exp_r(s1);}static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type){struct _tuple0*_tmp4C8=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4C8,0);struct Cyc_Absyn_Exp*
temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*
temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 
0)s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*
_tmp4C9=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp4C9,
Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=
Cyc_Absyn_declare_stmt(_tmp4C8,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(e1),Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple19{struct _tuple0*f1;void*f2;struct
Cyc_Absyn_Exp*f3;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*
f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void _tmpC40(unsigned int*_tmpC3F,unsigned int*_tmpC3E,struct _tuple0***
_tmpC3C){for(*_tmpC3F=0;*_tmpC3F < *_tmpC3E;(*_tmpC3F)++){(*_tmpC3C)[*_tmpC3F]=
Cyc_Toc_temp_var();}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){void*_tmp4CA=e->r;if(e->topt == 0){const char*_tmpBF1;void*_tmpBF0[1];struct Cyc_String_pa_struct
_tmpBEF;(_tmpBEF.tag=0,((_tmpBEF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpBF0[0]=& _tmpBEF,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF1="exp_to_c: no type for %s",
_tag_dyneither(_tmpBF1,sizeof(char),25))),_tag_dyneither(_tmpBF0,sizeof(void*),1)))))));}{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp4CE=
_tmp4CA;union Cyc_Absyn_Cnst _tmp4D0;int _tmp4D1;struct _tuple0*_tmp4D4;void*_tmp4D5;
struct _tuple0*_tmp4D7;enum Cyc_Absyn_Primop _tmp4D9;struct Cyc_List_List*_tmp4DA;
struct Cyc_Absyn_Exp*_tmp4DC;enum Cyc_Absyn_Incrementor _tmp4DD;struct Cyc_Absyn_Exp*
_tmp4DF;struct Cyc_Core_Opt*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4E1;struct Cyc_Absyn_Exp*
_tmp4E3;struct Cyc_Absyn_Exp*_tmp4E4;struct Cyc_Absyn_Exp*_tmp4E5;struct Cyc_Absyn_Exp*
_tmp4E7;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_Absyn_Exp*_tmp4EA;struct Cyc_Absyn_Exp*
_tmp4EB;struct Cyc_Absyn_Exp*_tmp4ED;struct Cyc_Absyn_Exp*_tmp4EE;struct Cyc_Absyn_Exp*
_tmp4F0;struct Cyc_List_List*_tmp4F1;struct Cyc_Absyn_Exp*_tmp4F3;struct Cyc_List_List*
_tmp4F4;struct Cyc_Absyn_VarargCallInfo*_tmp4F5;struct Cyc_Absyn_Exp*_tmp4F7;
struct Cyc_List_List*_tmp4F8;struct Cyc_Absyn_VarargCallInfo*_tmp4F9;struct Cyc_Absyn_VarargCallInfo
_tmp4FA;int _tmp4FB;struct Cyc_List_List*_tmp4FC;struct Cyc_Absyn_VarargInfo*
_tmp4FD;struct Cyc_Absyn_Exp*_tmp4FF;struct Cyc_Absyn_Exp*_tmp501;struct Cyc_Absyn_Exp*
_tmp503;struct Cyc_List_List*_tmp504;void*_tmp506;void**_tmp507;struct Cyc_Absyn_Exp*
_tmp508;int _tmp509;enum Cyc_Absyn_Coercion _tmp50A;struct Cyc_Absyn_Exp*_tmp50C;
struct Cyc_Absyn_Exp*_tmp50E;struct Cyc_Absyn_Exp*_tmp50F;struct Cyc_Absyn_Exp*
_tmp511;void*_tmp513;void*_tmp515;void*_tmp516;struct _dyneither_ptr*_tmp518;void*
_tmp51A;void*_tmp51B;unsigned int _tmp51D;struct Cyc_Absyn_Exp*_tmp51F;struct Cyc_Absyn_Exp*
_tmp521;struct _dyneither_ptr*_tmp522;int _tmp523;int _tmp524;struct Cyc_Absyn_Exp*
_tmp526;struct _dyneither_ptr*_tmp527;int _tmp528;int _tmp529;struct Cyc_Absyn_Exp*
_tmp52B;struct Cyc_Absyn_Exp*_tmp52C;struct Cyc_List_List*_tmp52E;struct Cyc_List_List*
_tmp530;struct Cyc_Absyn_Vardecl*_tmp532;struct Cyc_Absyn_Exp*_tmp533;struct Cyc_Absyn_Exp*
_tmp534;int _tmp535;struct _tuple0*_tmp537;struct Cyc_List_List*_tmp538;struct Cyc_List_List*
_tmp539;struct Cyc_Absyn_Aggrdecl*_tmp53A;void*_tmp53C;struct Cyc_List_List*
_tmp53D;struct Cyc_List_List*_tmp53F;struct Cyc_Absyn_Datatypedecl*_tmp540;struct
Cyc_Absyn_Datatypefield*_tmp541;struct Cyc_Absyn_MallocInfo _tmp545;int _tmp546;
struct Cyc_Absyn_Exp*_tmp547;void**_tmp548;struct Cyc_Absyn_Exp*_tmp549;int _tmp54A;
struct Cyc_Absyn_Exp*_tmp54C;struct Cyc_Absyn_Exp*_tmp54D;struct Cyc_Absyn_Exp*
_tmp54F;struct _dyneither_ptr*_tmp550;struct Cyc_Absyn_Stmt*_tmp552;_LL222: {
struct Cyc_Absyn_Const_e_struct*_tmp4CF=(struct Cyc_Absyn_Const_e_struct*)_tmp4CE;
if(_tmp4CF->tag != 0)goto _LL224;else{_tmp4D0=_tmp4CF->f1;if((_tmp4D0.Null_c).tag
!= 1)goto _LL224;_tmp4D1=(int)(_tmp4D0.Null_c).val;}}_LL223: {struct Cyc_Absyn_Exp*
_tmp557=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp557,
_tmp557))->r;else{struct Cyc_Absyn_Exp*_tmpBF2[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((
_tmpBF2[2]=_tmp557,((_tmpBF2[1]=_tmp557,((_tmpBF2[0]=_tmp557,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF2,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL221;}_LL224: {struct Cyc_Absyn_Const_e_struct*
_tmp4D2=(struct Cyc_Absyn_Const_e_struct*)_tmp4CE;if(_tmp4D2->tag != 0)goto _LL226;}
_LL225: goto _LL221;_LL226: {struct Cyc_Absyn_Var_e_struct*_tmp4D3=(struct Cyc_Absyn_Var_e_struct*)
_tmp4CE;if(_tmp4D3->tag != 1)goto _LL228;else{_tmp4D4=_tmp4D3->f1;_tmp4D5=(void*)
_tmp4D3->f2;}}_LL227:{struct _handler_cons _tmp559;_push_handler(& _tmp559);{int
_tmp55B=0;if(setjmp(_tmp559.handler))_tmp55B=1;if(!_tmp55B){e->r=(Cyc_Toc_lookup_varmap(
nv,_tmp4D4))->r;;_pop_handler();}else{void*_tmp55A=(void*)_exn_thrown;void*
_tmp55D=_tmp55A;_LL27B: {struct Cyc_Dict_Absent_struct*_tmp55E=(struct Cyc_Dict_Absent_struct*)
_tmp55D;if(_tmp55E->tag != Cyc_Dict_Absent)goto _LL27D;}_LL27C: {const char*_tmpBF6;
void*_tmpBF5[1];struct Cyc_String_pa_struct _tmpBF4;(_tmpBF4.tag=0,((_tmpBF4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4D4)),((
_tmpBF5[0]=& _tmpBF4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBF6="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpBF6,sizeof(char),
32))),_tag_dyneither(_tmpBF5,sizeof(void*),1)))))));}_LL27D:;_LL27E:(void)_throw(
_tmp55D);_LL27A:;}};}goto _LL221;_LL228: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp4D6=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp4CE;if(_tmp4D6->tag != 2)goto
_LL22A;else{_tmp4D7=_tmp4D6->f1;}}_LL229: {const char*_tmpBF9;void*_tmpBF8;(
_tmpBF8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBF9="unknownid",_tag_dyneither(_tmpBF9,sizeof(char),10))),_tag_dyneither(
_tmpBF8,sizeof(void*),0)));}_LL22A: {struct Cyc_Absyn_Primop_e_struct*_tmp4D8=(
struct Cyc_Absyn_Primop_e_struct*)_tmp4CE;if(_tmp4D8->tag != 3)goto _LL22C;else{
_tmp4D9=_tmp4D8->f1;_tmp4DA=_tmp4D8->f2;}}_LL22B: {struct Cyc_List_List*_tmp564=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4DA);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp4DA);switch(_tmp4D9){case Cyc_Absyn_Numelts: _LL27F: {
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp4DA))->hd;{void*_tmp565=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo _tmp567;struct Cyc_Absyn_Exp*
_tmp568;struct Cyc_Absyn_PtrInfo _tmp56A;void*_tmp56B;struct Cyc_Absyn_PtrAtts
_tmp56C;union Cyc_Absyn_Constraint*_tmp56D;union Cyc_Absyn_Constraint*_tmp56E;
union Cyc_Absyn_Constraint*_tmp56F;_LL282: {struct Cyc_Absyn_ArrayType_struct*
_tmp566=(struct Cyc_Absyn_ArrayType_struct*)_tmp565;if(_tmp566->tag != 9)goto
_LL284;else{_tmp567=_tmp566->f1;_tmp568=_tmp567.num_elts;}}_LL283: if(!Cyc_Evexp_c_can_eval((
struct Cyc_Absyn_Exp*)_check_null(_tmp568))){const char*_tmpBFC;void*_tmpBFB;(
_tmpBFB=0,Cyc_Tcutil_terr(e->loc,((_tmpBFC="can't calculate numelts",
_tag_dyneither(_tmpBFC,sizeof(char),24))),_tag_dyneither(_tmpBFB,sizeof(void*),0)));}
e->r=_tmp568->r;goto _LL281;_LL284: {struct Cyc_Absyn_PointerType_struct*_tmp569=(
struct Cyc_Absyn_PointerType_struct*)_tmp565;if(_tmp569->tag != 5)goto _LL286;else{
_tmp56A=_tmp569->f1;_tmp56B=_tmp56A.elt_typ;_tmp56C=_tmp56A.ptr_atts;_tmp56D=
_tmp56C.nullable;_tmp56E=_tmp56C.bounds;_tmp56F=_tmp56C.zero_term;}}_LL285:{void*
_tmp572=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp56E);struct Cyc_Absyn_Exp*_tmp575;_LL289: {struct Cyc_Absyn_DynEither_b_struct*
_tmp573=(struct Cyc_Absyn_DynEither_b_struct*)_tmp572;if(_tmp573->tag != 0)goto
_LL28B;}_LL28A:{struct Cyc_Absyn_Exp*_tmpBFD[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,((
_tmpBFD[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp56B),0),((_tmpBFD[0]=(
struct Cyc_Absyn_Exp*)_tmp4DA->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpBFD,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL288;_LL28B: {struct Cyc_Absyn_Upper_b_struct*_tmp574=(struct Cyc_Absyn_Upper_b_struct*)
_tmp572;if(_tmp574->tag != 1)goto _LL288;else{_tmp575=_tmp574->f1;}}_LL28C: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp56F)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp4DA->hd);struct Cyc_Absyn_Exp*_tmpBFE[2];e->r=Cyc_Toc_fncall_exp_r(
function_e,((_tmpBFE[1]=_tmp575,((_tmpBFE[0]=(struct Cyc_Absyn_Exp*)_tmp4DA->hd,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpBFE,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp56D)){if(!Cyc_Evexp_c_can_eval(_tmp575)){const
char*_tmpC01;void*_tmpC00;(_tmpC00=0,Cyc_Tcutil_terr(e->loc,((_tmpC01="can't calculate numelts",
_tag_dyneither(_tmpC01,sizeof(char),24))),_tag_dyneither(_tmpC00,sizeof(void*),0)));}
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp575,Cyc_Absyn_uint_exp(0,0));}else{e->r=
_tmp575->r;goto _LL288;}}goto _LL288;_LL288:;}goto _LL281;_LL286:;_LL287: {const
char*_tmpC06;void*_tmpC05[2];struct Cyc_String_pa_struct _tmpC04;struct Cyc_String_pa_struct
_tmpC03;(_tmpC03.tag=0,((_tmpC03.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((
_tmpC04.tag=0,((_tmpC04.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmpC05[0]=& _tmpC04,((
_tmpC05[1]=& _tmpC03,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC06="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpC06,
sizeof(char),41))),_tag_dyneither(_tmpC05,sizeof(void*),2)))))))))))));}_LL281:;}
break;}case Cyc_Absyn_Plus: _LL280:{void*_tmp57E=Cyc_Tcutil_compress((void*)((
struct Cyc_List_List*)_check_null(_tmp564))->hd);struct Cyc_Absyn_PtrInfo _tmp580;
void*_tmp581;struct Cyc_Absyn_PtrAtts _tmp582;union Cyc_Absyn_Constraint*_tmp583;
union Cyc_Absyn_Constraint*_tmp584;_LL28F: {struct Cyc_Absyn_PointerType_struct*
_tmp57F=(struct Cyc_Absyn_PointerType_struct*)_tmp57E;if(_tmp57F->tag != 5)goto
_LL291;else{_tmp580=_tmp57F->f1;_tmp581=_tmp580.elt_typ;_tmp582=_tmp580.ptr_atts;
_tmp583=_tmp582.bounds;_tmp584=_tmp582.zero_term;}}_LL290:{void*_tmp585=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp583);struct Cyc_Absyn_Exp*_tmp588;_LL294: {struct Cyc_Absyn_DynEither_b_struct*
_tmp586=(struct Cyc_Absyn_DynEither_b_struct*)_tmp585;if(_tmp586->tag != 0)goto
_LL296;}_LL295: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp4DA))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp4DA->tl))->hd;{struct Cyc_Absyn_Exp*_tmpC07[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpC07[2]=e2,((_tmpC07[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp581),0),((_tmpC07[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC07,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}goto _LL293;}_LL296: {struct Cyc_Absyn_Upper_b_struct*_tmp587=(struct
Cyc_Absyn_Upper_b_struct*)_tmp585;if(_tmp587->tag != 1)goto _LL293;else{_tmp588=
_tmp587->f1;}}_LL297: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp584)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp4DA))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp4DA->tl))->hd;struct Cyc_Absyn_Exp*_tmpC08[3];e->r=(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
e1),((_tmpC08[2]=e2,((_tmpC08[1]=_tmp588,((_tmpC08[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC08,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r;}goto _LL293;_LL293:;}goto _LL28E;_LL291:;_LL292: goto _LL28E;
_LL28E:;}break;case Cyc_Absyn_Minus: _LL28D: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp564))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4DA))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4DA->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp564->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{struct Cyc_Absyn_Exp*_tmpC09[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpC09[2]=Cyc_Absyn_prim1_exp(
Cyc_Absyn_Minus,e2,0),((_tmpC09[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpC09[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC09,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}break;}case Cyc_Absyn_Eq:
_LL298: goto _LL299;case Cyc_Absyn_Neq: _LL299: goto _LL29A;case Cyc_Absyn_Gt: _LL29A:
goto _LL29B;case Cyc_Absyn_Gte: _LL29B: goto _LL29C;case Cyc_Absyn_Lt: _LL29C: goto
_LL29D;case Cyc_Absyn_Lte: _LL29D: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4DA))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4DA->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp564))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp564->tl))->hd;void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& elt_typ))e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,
0));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp,0));break;}default: _LL29E: break;}goto _LL221;}_LL22C: {
struct Cyc_Absyn_Increment_e_struct*_tmp4DB=(struct Cyc_Absyn_Increment_e_struct*)
_tmp4CE;if(_tmp4DB->tag != 5)goto _LL22E;else{_tmp4DC=_tmp4DB->f1;_tmp4DD=_tmp4DB->f2;}}
_LL22D: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4DC->topt))->v;
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;const char*_tmpC0A;struct _dyneither_ptr incr_str=(_tmpC0A="increment",
_tag_dyneither(_tmpC0A,sizeof(char),10));if(_tmp4DD == Cyc_Absyn_PreDec  || 
_tmp4DD == Cyc_Absyn_PostDec){const char*_tmpC0B;incr_str=((_tmpC0B="decrement",
_tag_dyneither(_tmpC0B,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp4DC,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpC0F;void*_tmpC0E[1];
struct Cyc_String_pa_struct _tmpC0D;(_tmpC0D.tag=0,((_tmpC0D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC0E[0]=& _tmpC0D,Cyc_Tcutil_terr(
e->loc,((_tmpC0F="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpC0F,sizeof(char),74))),_tag_dyneither(_tmpC0E,sizeof(void*),1)))))));}{
const char*_tmpC12;void*_tmpC11;(_tmpC11=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC12="in-place inc/dec on zero-term",
_tag_dyneither(_tmpC12,sizeof(char),30))),_tag_dyneither(_tmpC11,sizeof(void*),0)));};}{
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp4DC,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp592=Cyc_Absyn_signed_int_exp(
1,0);{struct Cyc_Core_Opt*_tmpC13;_tmp592->topt=((_tmpC13=_cycalloc(sizeof(*
_tmpC13)),((_tmpC13->v=Cyc_Absyn_sint_typ,_tmpC13))));}switch(_tmp4DD){case Cyc_Absyn_PreInc:
_LL2A0:{struct Cyc_Absyn_AssignOp_e_struct _tmpC19;struct Cyc_Core_Opt*_tmpC18;
struct Cyc_Absyn_AssignOp_e_struct*_tmpC17;e->r=(void*)((_tmpC17=_cycalloc(
sizeof(*_tmpC17)),((_tmpC17[0]=((_tmpC19.tag=4,((_tmpC19.f1=_tmp4DC,((_tmpC19.f2=((
_tmpC18=_cycalloc_atomic(sizeof(*_tmpC18)),((_tmpC18->v=(void*)Cyc_Absyn_Plus,
_tmpC18)))),((_tmpC19.f3=_tmp592,_tmpC19)))))))),_tmpC17))));}Cyc_Toc_exp_to_c(
nv,e);return;case Cyc_Absyn_PreDec: _LL2A1:{struct Cyc_Absyn_AssignOp_e_struct
_tmpC1F;struct Cyc_Core_Opt*_tmpC1E;struct Cyc_Absyn_AssignOp_e_struct*_tmpC1D;e->r=(
void*)((_tmpC1D=_cycalloc(sizeof(*_tmpC1D)),((_tmpC1D[0]=((_tmpC1F.tag=4,((
_tmpC1F.f1=_tmp4DC,((_tmpC1F.f2=((_tmpC1E=_cycalloc_atomic(sizeof(*_tmpC1E)),((
_tmpC1E->v=(void*)Cyc_Absyn_Minus,_tmpC1E)))),((_tmpC1F.f3=_tmp592,_tmpC1F)))))))),
_tmpC1D))));}Cyc_Toc_exp_to_c(nv,e);return;default: _LL2A2:{const char*_tmpC23;
void*_tmpC22[1];struct Cyc_String_pa_struct _tmpC21;(_tmpC21.tag=0,((_tmpC21.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC22[0]=& _tmpC21,Cyc_Tcutil_terr(
e->loc,((_tmpC23="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpC23,sizeof(char),59))),_tag_dyneither(_tmpC22,sizeof(void*),1)))))));}{
const char*_tmpC26;void*_tmpC25;(_tmpC25=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC26="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpC26,sizeof(char),34))),_tag_dyneither(_tmpC25,sizeof(void*),0)));};}}
Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp4DC);Cyc_Toc_set_lhs(nv,0);{void*
elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp4DD == Cyc_Absyn_PostInc
 || _tmp4DD == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp4DD == Cyc_Absyn_PreDec  || _tmp4DD == Cyc_Absyn_PostDec)change=- 1;{struct
Cyc_Absyn_Exp*_tmpC27[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC27[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpC27[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpC27[0]=Cyc_Toc_push_address_exp(_tmp4DC),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC27,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Toc_functionSet*_tmp5A0=_tmp4DD == Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:&
Cyc_Toc__zero_arr_inplace_plus_post_functionSet;struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(
_tmp5A0,_tmp4DC);struct Cyc_Absyn_Exp*_tmpC28[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((
_tmpC28[1]=Cyc_Absyn_signed_int_exp(1,0),((_tmpC28[0]=Cyc_Toc_push_address_exp(
_tmp4DC),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC28,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(elt_typ == (
void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4DC)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(
_tmp4DC,0,Cyc_Toc_incr_lvalue,_tmp4DD);e->r=_tmp4DC->r;}}}goto _LL221;};};}_LL22E: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp4DE=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp4CE;if(_tmp4DE->tag != 4)goto _LL230;else{_tmp4DF=_tmp4DE->f1;_tmp4E0=_tmp4DE->f2;
_tmp4E1=_tmp4DE->f3;}}_LL22F: {void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4DF->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4E1->topt))->v;int f_tag=0;void*tagged_member_struct_type=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Toc_is_tagged_union_project(_tmp4DF,& f_tag,&
tagged_member_struct_type,1)){Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp4DF);
Cyc_Toc_set_lhs(nv,0);Cyc_Toc_exp_to_c(nv,_tmp4E1);e->r=Cyc_Toc_tagged_union_assignop(
_tmp4DF,e1_old_typ,_tmp4E0,_tmp4E1,e2_old_typ,f_tag,tagged_member_struct_type);
return;}{void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4DF,& ptr_type,&
is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4DF,_tmp4E0,
_tmp4E1,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp4DF);Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp4DF);Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4E1);{int done=0;if(_tmp4E0 != 0){void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;switch((enum Cyc_Absyn_Primop)_tmp4E0->v){case Cyc_Absyn_Plus: _LL2A4:
change=_tmp4E1;break;case Cyc_Absyn_Minus: _LL2A5: change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,
_tmp4E1,0);break;default: _LL2A6: {const char*_tmpC2B;void*_tmpC2A;(_tmpC2A=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC2B="bad t ? pointer arithmetic",_tag_dyneither(_tmpC2B,sizeof(char),27))),
_tag_dyneither(_tmpC2A,sizeof(void*),0)));}}done=1;{struct Cyc_Absyn_Exp*_tmp5A5=
Cyc_Toc__dyneither_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*_tmpC2C[3];e->r=Cyc_Toc_fncall_exp_r(
_tmp5A5,((_tmpC2C[2]=change,((_tmpC2C[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpC2C[0]=Cyc_Toc_push_address_exp(_tmp4DF),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2C,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))
switch((enum Cyc_Absyn_Primop)_tmp4E0->v){case Cyc_Absyn_Plus: _LL2A8: done=1;{
struct Cyc_Absyn_Exp*_tmpC2D[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4DF),((_tmpC2D[1]=_tmp4E1,((
_tmpC2D[0]=_tmp4DF,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC2D,sizeof(struct Cyc_Absyn_Exp*),2)))))));}break;default:
_LL2A9: {const char*_tmpC30;void*_tmpC2F;(_tmpC2F=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC30="bad zero-terminated pointer arithmetic",
_tag_dyneither(_tmpC30,sizeof(char),39))),_tag_dyneither(_tmpC2F,sizeof(void*),0)));}}}}
if(!done){if(e1_poly)_tmp4E1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp4E1->r,0));if(!Cyc_Absyn_is_lvalue(_tmp4DF)){{struct _tuple18 _tmpC33;struct
_tuple18*_tmpC32;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple18*),struct _tuple18*f_env))
Cyc_Toc_lvalue_assign)(_tmp4DF,0,Cyc_Toc_assignop_lvalue,((_tmpC32=_cycalloc(
sizeof(struct _tuple18)* 1),((_tmpC32[0]=((_tmpC33.f1=_tmp4E0,((_tmpC33.f2=
_tmp4E1,_tmpC33)))),_tmpC32)))));}e->r=_tmp4DF->r;}}goto _LL221;};};};}_LL230: {
struct Cyc_Absyn_Conditional_e_struct*_tmp4E2=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp4CE;if(_tmp4E2->tag != 6)goto _LL232;else{_tmp4E3=_tmp4E2->f1;_tmp4E4=_tmp4E2->f2;
_tmp4E5=_tmp4E2->f3;}}_LL231: Cyc_Toc_exp_to_c(nv,_tmp4E3);Cyc_Toc_exp_to_c(nv,
_tmp4E4);Cyc_Toc_exp_to_c(nv,_tmp4E5);goto _LL221;_LL232: {struct Cyc_Absyn_And_e_struct*
_tmp4E6=(struct Cyc_Absyn_And_e_struct*)_tmp4CE;if(_tmp4E6->tag != 7)goto _LL234;
else{_tmp4E7=_tmp4E6->f1;_tmp4E8=_tmp4E6->f2;}}_LL233: Cyc_Toc_exp_to_c(nv,
_tmp4E7);Cyc_Toc_exp_to_c(nv,_tmp4E8);goto _LL221;_LL234: {struct Cyc_Absyn_Or_e_struct*
_tmp4E9=(struct Cyc_Absyn_Or_e_struct*)_tmp4CE;if(_tmp4E9->tag != 8)goto _LL236;
else{_tmp4EA=_tmp4E9->f1;_tmp4EB=_tmp4E9->f2;}}_LL235: Cyc_Toc_exp_to_c(nv,
_tmp4EA);Cyc_Toc_exp_to_c(nv,_tmp4EB);goto _LL221;_LL236: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp4EC=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp4CE;if(_tmp4EC->tag != 9)goto _LL238;
else{_tmp4ED=_tmp4EC->f1;_tmp4EE=_tmp4EC->f2;}}_LL237: Cyc_Toc_exp_to_c(nv,
_tmp4ED);Cyc_Toc_exp_to_c(nv,_tmp4EE);goto _LL221;_LL238: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp4EF=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp4CE;if(_tmp4EF->tag != 10)goto
_LL23A;else{_tmp4F0=_tmp4EF->f1;_tmp4F1=_tmp4EF->f2;}}_LL239: _tmp4F3=_tmp4F0;
_tmp4F4=_tmp4F1;goto _LL23B;_LL23A: {struct Cyc_Absyn_FnCall_e_struct*_tmp4F2=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp4CE;if(_tmp4F2->tag != 11)goto _LL23C;else{
_tmp4F3=_tmp4F2->f1;_tmp4F4=_tmp4F2->f2;_tmp4F5=_tmp4F2->f3;if(_tmp4F5 != 0)goto
_LL23C;}}_LL23B: Cyc_Toc_exp_to_c(nv,_tmp4F3);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp4F4);goto _LL221;_LL23C: {struct Cyc_Absyn_FnCall_e_struct*
_tmp4F6=(struct Cyc_Absyn_FnCall_e_struct*)_tmp4CE;if(_tmp4F6->tag != 11)goto
_LL23E;else{_tmp4F7=_tmp4F6->f1;_tmp4F8=_tmp4F6->f2;_tmp4F9=_tmp4F6->f3;if(
_tmp4F9 == 0)goto _LL23E;_tmp4FA=*_tmp4F9;_tmp4FB=_tmp4FA.num_varargs;_tmp4FC=
_tmp4FA.injectors;_tmp4FD=_tmp4FA.vai;}}_LL23D: {struct Cyc_Toc_Env _tmp5AD;struct
_RegionHandle*_tmp5AE;struct Cyc_Toc_Env*_tmp5AC=nv;_tmp5AD=*_tmp5AC;_tmp5AE=
_tmp5AD.rgn;{struct _tuple0*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=
Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp4FB,0);void*cva_type=Cyc_Toc_typ_to_c(_tmp4FD->type);void*
arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp4F8);int num_normargs=num_args - _tmp4FB;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp4F8=_tmp4F8->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4F8))->hd);{
struct Cyc_List_List*_tmpC34;new_args=((_tmpC34=_cycalloc(sizeof(*_tmpC34)),((
_tmpC34->hd=(struct Cyc_Absyn_Exp*)_tmp4F8->hd,((_tmpC34->tl=new_args,_tmpC34))))));};}}{
struct Cyc_Absyn_Exp*_tmpC37[3];struct Cyc_List_List*_tmpC36;new_args=((_tmpC36=
_cycalloc(sizeof(*_tmpC36)),((_tmpC36->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((
_tmpC37[2]=num_varargs_exp,((_tmpC37[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0),((
_tmpC37[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC37,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpC36->tl=
new_args,_tmpC36))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp4F7);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4F7,new_args,0),0);if(_tmp4FD->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp5B2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp4FD->type));struct Cyc_Absyn_DatatypeInfo _tmp5B4;union Cyc_Absyn_DatatypeInfoU
_tmp5B5;struct Cyc_Absyn_Datatypedecl**_tmp5B6;struct Cyc_Absyn_Datatypedecl*
_tmp5B7;_LL2AC: {struct Cyc_Absyn_DatatypeType_struct*_tmp5B3=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp5B2;if(_tmp5B3->tag != 3)goto _LL2AE;else{_tmp5B4=_tmp5B3->f1;_tmp5B5=_tmp5B4.datatype_info;
if((_tmp5B5.KnownDatatype).tag != 2)goto _LL2AE;_tmp5B6=(struct Cyc_Absyn_Datatypedecl**)(
_tmp5B5.KnownDatatype).val;_tmp5B7=*_tmp5B6;}}_LL2AD: tud=_tmp5B7;goto _LL2AB;
_LL2AE:;_LL2AF: {const char*_tmpC3A;void*_tmpC39;(_tmpC39=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC3A="toc: unknown datatype in vararg with inject",
_tag_dyneither(_tmpC3A,sizeof(char),44))),_tag_dyneither(_tmpC39,sizeof(void*),0)));}
_LL2AB:;}{unsigned int _tmpC3F;unsigned int _tmpC3E;struct _dyneither_ptr _tmpC3D;
struct _tuple0**_tmpC3C;unsigned int _tmpC3B;struct _dyneither_ptr vs=(_tmpC3B=(
unsigned int)_tmp4FB,((_tmpC3C=(struct _tuple0**)_region_malloc(_tmp5AE,
_check_times(sizeof(struct _tuple0*),_tmpC3B)),((_tmpC3D=_tag_dyneither(_tmpC3C,
sizeof(struct _tuple0*),_tmpC3B),((((_tmpC3E=_tmpC3B,_tmpC40(& _tmpC3F,& _tmpC3E,&
_tmpC3C))),_tmpC3D)))))));if(_tmp4FB != 0){struct Cyc_List_List*_tmp5BA=0;{int i=
_tmp4FB - 1;for(0;i >= 0;-- i){struct Cyc_List_List*_tmpC41;_tmp5BA=((_tmpC41=
_cycalloc(sizeof(*_tmpC41)),((_tmpC41->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct
_tuple0*),i)),0),0)),((_tmpC41->tl=_tmp5BA,_tmpC41))))));}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp5BA,0),s,0);{
int i=0;for(0;_tmp4F8 != 0;(((_tmp4F8=_tmp4F8->tl,_tmp4FC=_tmp4FC->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4F8->hd;void*arg_type=(void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct _tuple0*var=*((struct
_tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield _tmp5BD;struct
_tuple0*_tmp5BE;struct Cyc_List_List*_tmp5BF;struct Cyc_Absyn_Datatypefield*
_tmp5BC=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp4FC))->hd;_tmp5BD=*_tmp5BC;_tmp5BE=_tmp5BD.name;_tmp5BF=_tmp5BD.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)_check_null(
_tmp5BF))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp5BE),0),s,0);{const char*_tmpC42;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp5BE,((_tmpC42="_struct",_tag_dyneither(_tmpC42,
sizeof(char),8))))),0,s,0);};};}};}else{struct _tuple15*_tmpC43[3];struct Cyc_List_List*
_tmp5C1=(_tmpC43[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC43[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpC43[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC43,sizeof(struct _tuple15*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{{int i=0;for(0;_tmp4F8
!= 0;(_tmp4F8=_tmp4F8->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp4F8->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4F8->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL221;}_LL23E: {struct Cyc_Absyn_Throw_e_struct*_tmp4FE=(struct Cyc_Absyn_Throw_e_struct*)
_tmp4CE;if(_tmp4FE->tag != 12)goto _LL240;else{_tmp4FF=_tmp4FE->f1;}}_LL23F: Cyc_Toc_exp_to_c(
nv,_tmp4FF);e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(
_tmp4FF),0))->r;goto _LL221;_LL240: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp500=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp4CE;if(_tmp500->tag != 13)
goto _LL242;else{_tmp501=_tmp500->f1;}}_LL241: Cyc_Toc_exp_to_c(nv,_tmp501);goto
_LL221;_LL242: {struct Cyc_Absyn_Instantiate_e_struct*_tmp502=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp4CE;if(_tmp502->tag != 14)goto _LL244;else{_tmp503=_tmp502->f1;_tmp504=_tmp502->f2;}}
_LL243: Cyc_Toc_exp_to_c(nv,_tmp503);for(0;_tmp504 != 0;_tmp504=_tmp504->tl){enum 
Cyc_Absyn_KindQual _tmp5C7=(Cyc_Tcutil_typ_kind((void*)_tmp504->hd))->kind;if(
_tmp5C7 != Cyc_Absyn_EffKind  && _tmp5C7 != Cyc_Absyn_RgnKind){{void*_tmp5C8=Cyc_Tcutil_compress((
void*)_tmp504->hd);_LL2B1: {struct Cyc_Absyn_VarType_struct*_tmp5C9=(struct Cyc_Absyn_VarType_struct*)
_tmp5C8;if(_tmp5C9->tag != 2)goto _LL2B3;}_LL2B2: goto _LL2B4;_LL2B3: {struct Cyc_Absyn_DatatypeType_struct*
_tmp5CA=(struct Cyc_Absyn_DatatypeType_struct*)_tmp5C8;if(_tmp5CA->tag != 3)goto
_LL2B5;}_LL2B4: continue;_LL2B5:;_LL2B6: e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp503,0))->r;goto _LL2B0;
_LL2B0:;}break;}}goto _LL221;_LL244: {struct Cyc_Absyn_Cast_e_struct*_tmp505=(
struct Cyc_Absyn_Cast_e_struct*)_tmp4CE;if(_tmp505->tag != 15)goto _LL246;else{
_tmp506=(void**)& _tmp505->f1;_tmp507=(void**)((void**)& _tmp505->f1);_tmp508=
_tmp505->f2;_tmp509=_tmp505->f3;_tmp50A=_tmp505->f4;}}_LL245: {void*old_t2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp508->topt))->v;void*new_typ=*_tmp507;*
_tmp507=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp508);{struct _tuple17
_tmpC44;struct _tuple17 _tmp5CC=(_tmpC44.f1=Cyc_Tcutil_compress(old_t2),((_tmpC44.f2=
Cyc_Tcutil_compress(new_typ),_tmpC44)));void*_tmp5CD;struct Cyc_Absyn_PtrInfo
_tmp5CF;void*_tmp5D0;struct Cyc_Absyn_PtrInfo _tmp5D2;void*_tmp5D3;struct Cyc_Absyn_PtrInfo
_tmp5D5;void*_tmp5D6;_LL2B8: _tmp5CD=_tmp5CC.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp5CE=(struct Cyc_Absyn_PointerType_struct*)_tmp5CD;if(_tmp5CE->tag != 5)goto
_LL2BA;else{_tmp5CF=_tmp5CE->f1;}};_tmp5D0=_tmp5CC.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp5D1=(struct Cyc_Absyn_PointerType_struct*)_tmp5D0;if(_tmp5D1->tag != 5)goto
_LL2BA;else{_tmp5D2=_tmp5D1->f1;}};_LL2B9: {int _tmp5D8=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp5CF.ptr_atts).nullable);int _tmp5D9=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5D2.ptr_atts).nullable);
void*_tmp5DA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp5CF.ptr_atts).bounds);void*_tmp5DB=((void*(*)(void*y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5D2.ptr_atts).bounds);
int _tmp5DC=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp5CF.ptr_atts).zero_term);int _tmp5DD=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp5D2.ptr_atts).zero_term);{struct _tuple17 _tmpC45;
struct _tuple17 _tmp5DF=(_tmpC45.f1=_tmp5DA,((_tmpC45.f2=_tmp5DB,_tmpC45)));void*
_tmp5E0;struct Cyc_Absyn_Exp*_tmp5E2;void*_tmp5E3;struct Cyc_Absyn_Exp*_tmp5E5;
void*_tmp5E6;struct Cyc_Absyn_Exp*_tmp5E8;void*_tmp5E9;void*_tmp5EB;void*_tmp5ED;
struct Cyc_Absyn_Exp*_tmp5EF;void*_tmp5F0;void*_tmp5F2;_LL2BF: _tmp5E0=_tmp5DF.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp5E1=(struct Cyc_Absyn_Upper_b_struct*)_tmp5E0;
if(_tmp5E1->tag != 1)goto _LL2C1;else{_tmp5E2=_tmp5E1->f1;}};_tmp5E3=_tmp5DF.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp5E4=(struct Cyc_Absyn_Upper_b_struct*)_tmp5E3;
if(_tmp5E4->tag != 1)goto _LL2C1;else{_tmp5E5=_tmp5E4->f1;}};_LL2C0: if((!Cyc_Evexp_c_can_eval(
_tmp5E2) || !Cyc_Evexp_c_can_eval(_tmp5E5)) && !Cyc_Evexp_same_const_exp(_tmp5E2,
_tmp5E5)){const char*_tmpC48;void*_tmpC47;(_tmpC47=0,Cyc_Tcutil_terr(e->loc,((
_tmpC48="can't validate cast due to potential size differences",_tag_dyneither(
_tmpC48,sizeof(char),54))),_tag_dyneither(_tmpC47,sizeof(void*),0)));}if(_tmp5D8
 && !_tmp5D9){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC4B;void*_tmpC4A;(
_tmpC4A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC4B="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC4B,
sizeof(char),44))),_tag_dyneither(_tmpC4A,sizeof(void*),0)));}if(_tmp50A != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC4F;void*_tmpC4E[1];struct Cyc_String_pa_struct _tmpC4D;(_tmpC4D.tag=
0,((_tmpC4D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpC4E[0]=& _tmpC4D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpC4F="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpC4F,sizeof(char),41))),_tag_dyneither(_tmpC4E,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp508);if(do_null_check){if(!_tmp509){
const char*_tmpC52;void*_tmpC51;(_tmpC51=0,Cyc_Tcutil_warn(e->loc,((_tmpC52="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpC52,sizeof(char),58))),_tag_dyneither(_tmpC51,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC53;e->r=Cyc_Toc_cast_it_r(*_tmp507,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC53=_cycalloc(sizeof(*_tmpC53)),((_tmpC53->hd=
_tmp508,((_tmpC53->tl=0,_tmpC53)))))),0));};}};}goto _LL2BE;_LL2C1: _tmp5E6=
_tmp5DF.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp5E7=(struct Cyc_Absyn_Upper_b_struct*)
_tmp5E6;if(_tmp5E7->tag != 1)goto _LL2C3;else{_tmp5E8=_tmp5E7->f1;}};_tmp5E9=
_tmp5DF.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5EA=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5E9;if(_tmp5EA->tag != 0)goto _LL2C3;};_LL2C2: if(!Cyc_Evexp_c_can_eval(_tmp5E8)){
const char*_tmpC56;void*_tmpC55;(_tmpC55=0,Cyc_Tcutil_terr(e->loc,((_tmpC56="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC56,sizeof(char),71))),_tag_dyneither(_tmpC55,sizeof(void*),0)));}
if(_tmp50A == Cyc_Absyn_NonNull_to_Null){const char*_tmpC5A;void*_tmpC59[1];struct
Cyc_String_pa_struct _tmpC58;(_tmpC58.tag=0,((_tmpC58.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC59[0]=& _tmpC58,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC5A="conversion mis-classified as null-check: %s",
_tag_dyneither(_tmpC5A,sizeof(char),44))),_tag_dyneither(_tmpC59,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){if((_tmp5DC  && !(_tmp5D2.elt_tq).real_const) && !
_tmp5DD)_tmp5E8=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp5E8,Cyc_Absyn_uint_exp(1,
0),0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp5E8,_tmp508))->r;}else{
struct Cyc_Absyn_Exp*_tmp603=Cyc_Toc__tag_dyneither_e;if(_tmp5DC){struct _tuple0*
_tmp604=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp605=Cyc_Absyn_var_exp(_tmp604,
0);struct Cyc_Absyn_Exp*arg3;{void*_tmp606=_tmp508->r;union Cyc_Absyn_Cnst _tmp608;
struct _dyneither_ptr _tmp609;_LL2C8: {struct Cyc_Absyn_Const_e_struct*_tmp607=(
struct Cyc_Absyn_Const_e_struct*)_tmp606;if(_tmp607->tag != 0)goto _LL2CA;else{
_tmp608=_tmp607->f1;if((_tmp608.String_c).tag != 7)goto _LL2CA;_tmp609=(struct
_dyneither_ptr)(_tmp608.String_c).val;}}_LL2C9: arg3=_tmp5E8;goto _LL2C7;_LL2CA:;
_LL2CB:{struct Cyc_Absyn_Exp*_tmpC5B[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp508),((_tmpC5B[1]=_tmp5E8,((_tmpC5B[0]=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp605),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5B,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);}goto _LL2C7;_LL2C7:;}if(!_tmp5DD  && !(_tmp5D2.elt_tq).real_const)arg3=
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*
_tmp60B=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5D2.elt_typ),0);struct Cyc_Absyn_Exp*
_tmpC5C[3];struct Cyc_Absyn_Exp*_tmp60C=Cyc_Absyn_fncall_exp(_tmp603,((_tmpC5C[2]=
arg3,((_tmpC5C[1]=_tmp60B,((_tmpC5C[0]=_tmp605,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5C,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);struct Cyc_Absyn_Stmt*_tmp60D=Cyc_Absyn_exp_stmt(_tmp60C,0);_tmp60D=
Cyc_Absyn_declare_stmt(_tmp604,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)
_tmp508,_tmp60D,0);e->r=Cyc_Toc_stmt_exp_r(_tmp60D);};}else{struct Cyc_Absyn_Exp*
_tmpC5D[3];e->r=Cyc_Toc_fncall_exp_r(_tmp603,((_tmpC5D[2]=_tmp5E8,((_tmpC5D[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5D2.elt_typ),0),((_tmpC5D[0]=
_tmp508,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC5D,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2BE;
_LL2C3: _tmp5EB=_tmp5DF.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp5EC=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5EB;if(_tmp5EC->tag != 0)goto _LL2C5;};_tmp5ED=_tmp5DF.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp5EE=(struct Cyc_Absyn_Upper_b_struct*)_tmp5ED;if(_tmp5EE->tag != 1)goto _LL2C5;
else{_tmp5EF=_tmp5EE->f1;}};_LL2C4: if(!Cyc_Evexp_c_can_eval(_tmp5EF)){const char*
_tmpC60;void*_tmpC5F;(_tmpC5F=0,Cyc_Tcutil_terr(e->loc,((_tmpC60="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC60,sizeof(char),71))),_tag_dyneither(_tmpC5F,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC63;void*_tmpC62;(_tmpC62=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC63="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpC63,sizeof(char),45))),_tag_dyneither(_tmpC62,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp614=_tmp5EF;if(_tmp5DC  && !_tmp5DD)_tmp614=Cyc_Absyn_add_exp(
_tmp5EF,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp615=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC64[3];struct Cyc_Absyn_Exp*_tmp616=Cyc_Absyn_fncall_exp(
_tmp615,((_tmpC64[2]=_tmp614,((_tmpC64[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp5CF.elt_typ),0),((_tmpC64[0]=_tmp508,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC64,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp5D9){struct Cyc_List_List*_tmpC65;_tmp616->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_null_e,((_tmpC65=_cycalloc(sizeof(*_tmpC65)),((_tmpC65->hd=Cyc_Absyn_copy_exp(
_tmp616),((_tmpC65->tl=0,_tmpC65)))))));}e->r=Cyc_Toc_cast_it_r(*_tmp507,_tmp616);
goto _LL2BE;};};_LL2C5: _tmp5F0=_tmp5DF.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp5F1=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5F0;if(_tmp5F1->tag != 0)goto
_LL2BE;};_tmp5F2=_tmp5DF.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5F3=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp5F2;if(_tmp5F3->tag != 0)goto _LL2BE;};_LL2C6:
DynCast: if((_tmp5DC  && !_tmp5DD) && !(_tmp5D2.elt_tq).real_const){if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpC68;void*_tmpC67;(_tmpC67=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC68="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpC68,sizeof(char),70))),_tag_dyneither(_tmpC67,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp61B=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpC69[3];e->r=Cyc_Toc_fncall_exp_r(_tmp61B,((_tmpC69[2]=Cyc_Absyn_uint_exp(1,0),((
_tmpC69[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5CF.elt_typ),0),((
_tmpC69[0]=_tmp508,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC69,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}goto _LL2BE;
_LL2BE:;}goto _LL2B7;}_LL2BA: _tmp5D3=_tmp5CC.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp5D4=(struct Cyc_Absyn_PointerType_struct*)_tmp5D3;if(_tmp5D4->tag != 5)goto
_LL2BC;else{_tmp5D5=_tmp5D4->f1;}};_tmp5D6=_tmp5CC.f2;{struct Cyc_Absyn_IntType_struct*
_tmp5D7=(struct Cyc_Absyn_IntType_struct*)_tmp5D6;if(_tmp5D7->tag != 6)goto _LL2BC;};
_LL2BB:{void*_tmp61D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp5D5.ptr_atts).bounds);_LL2CD: {struct Cyc_Absyn_DynEither_b_struct*
_tmp61E=(struct Cyc_Absyn_DynEither_b_struct*)_tmp61D;if(_tmp61E->tag != 0)goto
_LL2CF;}_LL2CE: _tmp508->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp508->r,
_tmp508->loc),Cyc_Toc_curr_sp);goto _LL2CC;_LL2CF:;_LL2D0: goto _LL2CC;_LL2CC:;}
goto _LL2B7;_LL2BC:;_LL2BD: goto _LL2B7;_LL2B7:;}goto _LL221;}_LL246: {struct Cyc_Absyn_Address_e_struct*
_tmp50B=(struct Cyc_Absyn_Address_e_struct*)_tmp4CE;if(_tmp50B->tag != 16)goto
_LL248;else{_tmp50C=_tmp50B->f1;}}_LL247:{void*_tmp61F=_tmp50C->r;struct _tuple0*
_tmp621;struct Cyc_List_List*_tmp622;struct Cyc_List_List*_tmp623;struct Cyc_List_List*
_tmp625;_LL2D2: {struct Cyc_Absyn_Aggregate_e_struct*_tmp620=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp61F;if(_tmp620->tag != 29)goto _LL2D4;else{_tmp621=_tmp620->f1;_tmp622=_tmp620->f2;
_tmp623=_tmp620->f3;}}_LL2D3: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC6D;void*
_tmpC6C[1];struct Cyc_String_pa_struct _tmpC6B;(_tmpC6B.tag=0,((_tmpC6B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp50C->loc)),((
_tmpC6C[0]=& _tmpC6B,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC6D="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC6D,
sizeof(char),42))),_tag_dyneither(_tmpC6C,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp50C->topt))->v,_tmp622 != 0,1,0,
_tmp623,_tmp621))->r;goto _LL2D1;_LL2D4: {struct Cyc_Absyn_Tuple_e_struct*_tmp624=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp61F;if(_tmp624->tag != 25)goto _LL2D6;else{
_tmp625=_tmp624->f1;}}_LL2D5: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC71;void*
_tmpC70[1];struct Cyc_String_pa_struct _tmpC6F;(_tmpC6F.tag=0,((_tmpC6F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp50C->loc)),((
_tmpC70[0]=& _tmpC6F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC71="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC71,
sizeof(char),42))),_tag_dyneither(_tmpC70,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_tuple(
nv,1,0,_tmp625))->r;goto _LL2D1;_LL2D6:;_LL2D7: Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(
nv,_tmp50C);Cyc_Toc_set_lhs(nv,0);if(!Cyc_Absyn_is_lvalue(_tmp50C)){((void(*)(
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
int),int f_env))Cyc_Toc_lvalue_assign)(_tmp50C,0,Cyc_Toc_address_lvalue,1);e->r=
Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmp50C);}goto _LL2D1;_LL2D1:;}goto _LL221;_LL248: {struct Cyc_Absyn_New_e_struct*
_tmp50D=(struct Cyc_Absyn_New_e_struct*)_tmp4CE;if(_tmp50D->tag != 17)goto _LL24A;
else{_tmp50E=_tmp50D->f1;_tmp50F=_tmp50D->f2;}}_LL249: if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC75;void*_tmpC74[1];struct Cyc_String_pa_struct _tmpC73;(_tmpC73.tag=
0,((_tmpC73.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
_tmp50F->loc)),((_tmpC74[0]=& _tmpC73,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC75="%s: new at top-level",_tag_dyneither(
_tmpC75,sizeof(char),21))),_tag_dyneither(_tmpC74,sizeof(void*),1)))))));}{void*
_tmp62F=_tmp50F->r;struct Cyc_List_List*_tmp631;struct Cyc_Absyn_Vardecl*_tmp633;
struct Cyc_Absyn_Exp*_tmp634;struct Cyc_Absyn_Exp*_tmp635;int _tmp636;struct _tuple0*
_tmp638;struct Cyc_List_List*_tmp639;struct Cyc_List_List*_tmp63A;struct Cyc_Absyn_Aggrdecl*
_tmp63B;struct Cyc_List_List*_tmp63D;_LL2D9: {struct Cyc_Absyn_Array_e_struct*
_tmp630=(struct Cyc_Absyn_Array_e_struct*)_tmp62F;if(_tmp630->tag != 27)goto _LL2DB;
else{_tmp631=_tmp630->f1;}}_LL2DA: {struct _tuple0*_tmp63E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp63F=Cyc_Absyn_var_exp(_tmp63E,0);struct Cyc_Absyn_Stmt*
_tmp640=Cyc_Toc_init_array(nv,_tmp63F,_tmp631,Cyc_Absyn_exp_stmt(_tmp63F,0));
void*old_elt_typ;{void*_tmp641=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp643;void*_tmp644;struct Cyc_Absyn_Tqual _tmp645;struct Cyc_Absyn_PtrAtts _tmp646;
union Cyc_Absyn_Constraint*_tmp647;_LL2E4: {struct Cyc_Absyn_PointerType_struct*
_tmp642=(struct Cyc_Absyn_PointerType_struct*)_tmp641;if(_tmp642->tag != 5)goto
_LL2E6;else{_tmp643=_tmp642->f1;_tmp644=_tmp643.elt_typ;_tmp645=_tmp643.elt_tq;
_tmp646=_tmp643.ptr_atts;_tmp647=_tmp646.zero_term;}}_LL2E5: old_elt_typ=_tmp644;
goto _LL2E3;_LL2E6:;_LL2E7: {const char*_tmpC78;void*_tmpC77;old_elt_typ=((_tmpC77=
0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC78="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(
_tmpC78,sizeof(char),52))),_tag_dyneither(_tmpC77,sizeof(void*),0))));}_LL2E3:;}{
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp64A=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp64B=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp631),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp50E == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp64B);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp50E;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp64B);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp63E,_tmp64A,(struct Cyc_Absyn_Exp*)e1,_tmp640,0));goto
_LL2D8;};}_LL2DB: {struct Cyc_Absyn_Comprehension_e_struct*_tmp632=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp62F;if(_tmp632->tag != 28)goto _LL2DD;else{_tmp633=_tmp632->f1;_tmp634=_tmp632->f2;
_tmp635=_tmp632->f3;_tmp636=_tmp632->f4;}}_LL2DC: {int is_dyneither_ptr=0;{void*
_tmp64C=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp64E;void*_tmp64F;
struct Cyc_Absyn_Tqual _tmp650;struct Cyc_Absyn_PtrAtts _tmp651;union Cyc_Absyn_Constraint*
_tmp652;union Cyc_Absyn_Constraint*_tmp653;_LL2E9: {struct Cyc_Absyn_PointerType_struct*
_tmp64D=(struct Cyc_Absyn_PointerType_struct*)_tmp64C;if(_tmp64D->tag != 5)goto
_LL2EB;else{_tmp64E=_tmp64D->f1;_tmp64F=_tmp64E.elt_typ;_tmp650=_tmp64E.elt_tq;
_tmp651=_tmp64E.ptr_atts;_tmp652=_tmp651.bounds;_tmp653=_tmp651.zero_term;}}
_LL2EA: is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp652)== (void*)& Cyc_Absyn_DynEither_b_val;goto _LL2E8;
_LL2EB:;_LL2EC: {const char*_tmpC7B;void*_tmpC7A;(_tmpC7A=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC7B="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpC7B,sizeof(char),42))),_tag_dyneither(_tmpC7A,sizeof(void*),0)));}
_LL2E8:;}{struct _tuple0*max=Cyc_Toc_temp_var();struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp635->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp634);{struct Cyc_Absyn_Exp*_tmp656=
Cyc_Absyn_var_exp(max,0);if(_tmp636)_tmp656=Cyc_Absyn_add_exp(_tmp656,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp633,Cyc_Absyn_var_exp(max,0),_tmp635,_tmp636,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp658;struct _RegionHandle*_tmp659;struct Cyc_Toc_Env*_tmp657=
nv;_tmp658=*_tmp657;_tmp659=_tmp658.rgn;{struct _tuple19*_tmpC7E;struct Cyc_List_List*
_tmpC7D;struct Cyc_List_List*decls=(_tmpC7D=_region_malloc(_tmp659,sizeof(*
_tmpC7D)),((_tmpC7D->hd=((_tmpC7E=_region_malloc(_tmp659,sizeof(*_tmpC7E)),((
_tmpC7E->f1=max,((_tmpC7E->f2=Cyc_Absyn_uint_typ,((_tmpC7E->f3=(struct Cyc_Absyn_Exp*)
_tmp634,_tmpC7E)))))))),((_tmpC7D->tl=0,_tmpC7D)))));struct Cyc_Absyn_Exp*ai;if(
_tmp50E == 0  || Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*_tmpC7F[2];ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((_tmpC7F[1]=_tmp656,((
_tmpC7F[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC7F,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp50E;Cyc_Toc_exp_to_c(
nv,r);{struct Cyc_Absyn_Exp*_tmpC80[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpC80[1]=_tmp656,((_tmpC80[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC80,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct _tuple19*_tmpC83;struct Cyc_List_List*
_tmpC82;decls=((_tmpC82=_region_malloc(_tmp659,sizeof(*_tmpC82)),((_tmpC82->hd=((
_tmpC83=_region_malloc(_tmp659,sizeof(*_tmpC83)),((_tmpC83->f1=a,((_tmpC83->f2=
ptr_typ,((_tmpC83->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpC83)))))))),((_tmpC82->tl=
decls,_tmpC82))))));}if(is_dyneither_ptr){struct _tuple0*_tmp65E=Cyc_Toc_temp_var();
void*_tmp65F=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp660=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC84[3];struct Cyc_Absyn_Exp*_tmp661=Cyc_Absyn_fncall_exp(
_tmp660,((_tmpC84[2]=_tmp656,((_tmpC84[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((
_tmpC84[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC84,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{
struct _tuple19*_tmpC87;struct Cyc_List_List*_tmpC86;decls=((_tmpC86=
_region_malloc(_tmp659,sizeof(*_tmpC86)),((_tmpC86->hd=((_tmpC87=_region_malloc(
_tmp659,sizeof(*_tmpC87)),((_tmpC87->f1=_tmp65E,((_tmpC87->f2=_tmp65F,((_tmpC87->f3=(
struct Cyc_Absyn_Exp*)_tmp661,_tmpC87)))))))),((_tmpC86->tl=decls,_tmpC86))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp65E,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp665=decls;for(0;_tmp665 != 0;_tmp665=_tmp665->tl){struct
_tuple0*_tmp667;void*_tmp668;struct Cyc_Absyn_Exp*_tmp669;struct _tuple19 _tmp666=*((
struct _tuple19*)_tmp665->hd);_tmp667=_tmp666.f1;_tmp668=_tmp666.f2;_tmp669=
_tmp666.f3;s=Cyc_Absyn_declare_stmt(_tmp667,_tmp668,_tmp669,s,0);}}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL2D8;};};};}_LL2DD: {struct Cyc_Absyn_Aggregate_e_struct*_tmp637=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp62F;if(_tmp637->tag != 29)goto _LL2DF;
else{_tmp638=_tmp637->f1;_tmp639=_tmp637->f2;_tmp63A=_tmp637->f3;_tmp63B=_tmp637->f4;}}
_LL2DE: e->r=(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp50F->topt))->v,_tmp639 != 0,1,_tmp50E,_tmp63A,_tmp638))->r;goto _LL2D8;_LL2DF: {
struct Cyc_Absyn_Tuple_e_struct*_tmp63C=(struct Cyc_Absyn_Tuple_e_struct*)_tmp62F;
if(_tmp63C->tag != 25)goto _LL2E1;else{_tmp63D=_tmp63C->f1;}}_LL2E0: e->r=(Cyc_Toc_init_tuple(
nv,1,_tmp50E,_tmp63D))->r;goto _LL2D8;_LL2E1:;_LL2E2: {void*old_elt_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp50F->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);struct _tuple0*_tmp66C=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp66D=Cyc_Absyn_var_exp(_tmp66C,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(_tmp66D,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp50E
== 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp50E;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp66E=_tmp50F->r;void*_tmp670;
struct Cyc_Absyn_Exp*_tmp671;_LL2EE: {struct Cyc_Absyn_Cast_e_struct*_tmp66F=(
struct Cyc_Absyn_Cast_e_struct*)_tmp66E;if(_tmp66F->tag != 15)goto _LL2F0;else{
_tmp670=(void*)_tmp66F->f1;_tmp671=_tmp66F->f2;}}_LL2EF:{struct _tuple17 _tmpC88;
struct _tuple17 _tmp673=(_tmpC88.f1=Cyc_Tcutil_compress(_tmp670),((_tmpC88.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp671->topt))->v),_tmpC88)));void*
_tmp674;struct Cyc_Absyn_PtrInfo _tmp676;void*_tmp677;struct Cyc_Absyn_PtrAtts
_tmp678;union Cyc_Absyn_Constraint*_tmp679;void*_tmp67A;struct Cyc_Absyn_PtrInfo
_tmp67C;struct Cyc_Absyn_PtrAtts _tmp67D;union Cyc_Absyn_Constraint*_tmp67E;_LL2F3:
_tmp674=_tmp673.f1;{struct Cyc_Absyn_PointerType_struct*_tmp675=(struct Cyc_Absyn_PointerType_struct*)
_tmp674;if(_tmp675->tag != 5)goto _LL2F5;else{_tmp676=_tmp675->f1;_tmp677=_tmp676.elt_typ;
_tmp678=_tmp676.ptr_atts;_tmp679=_tmp678.bounds;}};_tmp67A=_tmp673.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp67B=(struct Cyc_Absyn_PointerType_struct*)_tmp67A;if(_tmp67B->tag != 5)goto
_LL2F5;else{_tmp67C=_tmp67B->f1;_tmp67D=_tmp67C.ptr_atts;_tmp67E=_tmp67D.bounds;}};
_LL2F4:{struct _tuple17 _tmpC89;struct _tuple17 _tmp680=(_tmpC89.f1=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp679),((_tmpC89.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp67E),_tmpC89)));void*_tmp681;void*_tmp683;struct Cyc_Absyn_Exp*
_tmp685;_LL2F8: _tmp681=_tmp680.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp682=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp681;if(_tmp682->tag != 0)goto _LL2FA;};
_tmp683=_tmp680.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp684=(struct Cyc_Absyn_Upper_b_struct*)
_tmp683;if(_tmp684->tag != 1)goto _LL2FA;else{_tmp685=_tmp684->f1;}};_LL2F9: Cyc_Toc_exp_to_c(
nv,_tmp671);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp686=Cyc_Toc__init_dyneither_ptr_e;{struct Cyc_Absyn_Exp*_tmpC8A[4];e->r=Cyc_Toc_fncall_exp_r(
_tmp686,((_tmpC8A[3]=_tmp685,((_tmpC8A[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp677),0),((_tmpC8A[1]=_tmp671,((_tmpC8A[0]=mexp,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC8A,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))));}goto _LL2F7;};_LL2FA:;_LL2FB: goto _LL2F7;_LL2F7:;}goto _LL2F2;_LL2F5:;
_LL2F6: goto _LL2F2;_LL2F2:;}goto _LL2ED;_LL2F0:;_LL2F1: goto _LL2ED;_LL2ED:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp688=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp66D),
0);struct Cyc_Absyn_Exp*_tmp689=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,
_tmp50F);_tmp688=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
_tmp66D,_tmp689,0),_tmp50F,0),_tmp688,0);{void*_tmp68A=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp66C,
_tmp68A,(struct Cyc_Absyn_Exp*)mexp,_tmp688,0));};}goto _LL2D8;};}_LL2D8:;}goto
_LL221;_LL24A: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp510=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp4CE;if(_tmp510->tag != 19)goto _LL24C;else{_tmp511=_tmp510->f1;}}_LL24B: Cyc_Toc_exp_to_c(
nv,_tmp511);goto _LL221;_LL24C: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp512=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp4CE;if(_tmp512->tag != 18)goto _LL24E;
else{_tmp513=(void*)_tmp512->f1;}}_LL24D:{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpC8D;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpC8C;e->r=(void*)((_tmpC8C=
_cycalloc(sizeof(*_tmpC8C)),((_tmpC8C[0]=((_tmpC8D.tag=18,((_tmpC8D.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp513),_tmpC8D)))),_tmpC8C))));}goto _LL221;_LL24E: {
struct Cyc_Absyn_Offsetof_e_struct*_tmp514=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp4CE;if(_tmp514->tag != 20)goto _LL250;else{_tmp515=(void*)_tmp514->f1;_tmp516=(
void*)_tmp514->f2;{struct Cyc_Absyn_StructField_struct*_tmp517=(struct Cyc_Absyn_StructField_struct*)
_tmp516;if(_tmp517->tag != 0)goto _LL250;else{_tmp518=_tmp517->f1;}};}}_LL24F:{
struct Cyc_Absyn_Offsetof_e_struct _tmpC97;struct Cyc_Absyn_StructField_struct
_tmpC96;struct Cyc_Absyn_StructField_struct*_tmpC95;struct Cyc_Absyn_Offsetof_e_struct*
_tmpC94;e->r=(void*)((_tmpC94=_cycalloc(sizeof(*_tmpC94)),((_tmpC94[0]=((_tmpC97.tag=
20,((_tmpC97.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp515),((_tmpC97.f2=(void*)((
void*)((_tmpC95=_cycalloc(sizeof(*_tmpC95)),((_tmpC95[0]=((_tmpC96.tag=0,((
_tmpC96.f1=_tmp518,_tmpC96)))),_tmpC95))))),_tmpC97)))))),_tmpC94))));}goto
_LL221;_LL250: {struct Cyc_Absyn_Offsetof_e_struct*_tmp519=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp4CE;if(_tmp519->tag != 20)goto _LL252;else{_tmp51A=(void*)_tmp519->f1;_tmp51B=(
void*)_tmp519->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp51C=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp51B;if(_tmp51C->tag != 1)goto _LL252;else{_tmp51D=_tmp51C->f1;}};}}_LL251:{
void*_tmp691=Cyc_Tcutil_compress(_tmp51A);struct Cyc_Absyn_AggrInfo _tmp693;union
Cyc_Absyn_AggrInfoU _tmp694;struct Cyc_List_List*_tmp696;_LL2FD: {struct Cyc_Absyn_AggrType_struct*
_tmp692=(struct Cyc_Absyn_AggrType_struct*)_tmp691;if(_tmp692->tag != 12)goto
_LL2FF;else{_tmp693=_tmp692->f1;_tmp694=_tmp693.aggr_info;}}_LL2FE: {struct Cyc_Absyn_Aggrdecl*
_tmp699=Cyc_Absyn_get_known_aggrdecl(_tmp694);if(_tmp699->impl == 0){const char*
_tmpC9A;void*_tmpC99;(_tmpC99=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC9A="struct fields must be known",
_tag_dyneither(_tmpC9A,sizeof(char),28))),_tag_dyneither(_tmpC99,sizeof(void*),0)));}
_tmp696=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp699->impl))->fields;goto
_LL300;}_LL2FF: {struct Cyc_Absyn_AnonAggrType_struct*_tmp695=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp691;if(_tmp695->tag != 13)goto _LL301;else{_tmp696=_tmp695->f2;}}_LL300: {
struct Cyc_Absyn_Aggrfield*_tmp69C=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp696,(int)_tmp51D);{struct Cyc_Absyn_Offsetof_e_struct
_tmpCA4;struct Cyc_Absyn_StructField_struct _tmpCA3;struct Cyc_Absyn_StructField_struct*
_tmpCA2;struct Cyc_Absyn_Offsetof_e_struct*_tmpCA1;e->r=(void*)((_tmpCA1=
_cycalloc(sizeof(*_tmpCA1)),((_tmpCA1[0]=((_tmpCA4.tag=20,((_tmpCA4.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp51A),((_tmpCA4.f2=(void*)((void*)((_tmpCA2=_cycalloc(
sizeof(*_tmpCA2)),((_tmpCA2[0]=((_tmpCA3.tag=0,((_tmpCA3.f1=_tmp69C->name,
_tmpCA3)))),_tmpCA2))))),_tmpCA4)))))),_tmpCA1))));}goto _LL2FC;}_LL301: {struct
Cyc_Absyn_TupleType_struct*_tmp697=(struct Cyc_Absyn_TupleType_struct*)_tmp691;
if(_tmp697->tag != 11)goto _LL303;}_LL302:{struct Cyc_Absyn_Offsetof_e_struct
_tmpCAE;struct Cyc_Absyn_StructField_struct _tmpCAD;struct Cyc_Absyn_StructField_struct*
_tmpCAC;struct Cyc_Absyn_Offsetof_e_struct*_tmpCAB;e->r=(void*)((_tmpCAB=
_cycalloc(sizeof(*_tmpCAB)),((_tmpCAB[0]=((_tmpCAE.tag=20,((_tmpCAE.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp51A),((_tmpCAE.f2=(void*)((void*)((_tmpCAC=_cycalloc(
sizeof(*_tmpCAC)),((_tmpCAC[0]=((_tmpCAD.tag=0,((_tmpCAD.f1=Cyc_Absyn_fieldname((
int)(_tmp51D + 1)),_tmpCAD)))),_tmpCAC))))),_tmpCAE)))))),_tmpCAB))));}goto _LL2FC;
_LL303: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp698=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp691;if(_tmp698->tag != 4)goto _LL305;}_LL304: if(_tmp51D == 0){struct Cyc_Absyn_Offsetof_e_struct
_tmpCB8;struct Cyc_Absyn_StructField_struct _tmpCB7;struct Cyc_Absyn_StructField_struct*
_tmpCB6;struct Cyc_Absyn_Offsetof_e_struct*_tmpCB5;e->r=(void*)((_tmpCB5=
_cycalloc(sizeof(*_tmpCB5)),((_tmpCB5[0]=((_tmpCB8.tag=20,((_tmpCB8.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp51A),((_tmpCB8.f2=(void*)((void*)((_tmpCB6=_cycalloc(
sizeof(*_tmpCB6)),((_tmpCB6[0]=((_tmpCB7.tag=0,((_tmpCB7.f1=Cyc_Toc_tag_sp,
_tmpCB7)))),_tmpCB6))))),_tmpCB8)))))),_tmpCB5))));}else{struct Cyc_Absyn_Offsetof_e_struct
_tmpCC2;struct Cyc_Absyn_StructField_struct _tmpCC1;struct Cyc_Absyn_StructField_struct*
_tmpCC0;struct Cyc_Absyn_Offsetof_e_struct*_tmpCBF;e->r=(void*)((_tmpCBF=
_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF[0]=((_tmpCC2.tag=20,((_tmpCC2.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp51A),((_tmpCC2.f2=(void*)((void*)((_tmpCC0=_cycalloc(
sizeof(*_tmpCC0)),((_tmpCC0[0]=((_tmpCC1.tag=0,((_tmpCC1.f1=Cyc_Absyn_fieldname((
int)_tmp51D),_tmpCC1)))),_tmpCC0))))),_tmpCC2)))))),_tmpCBF))));}goto _LL2FC;
_LL305:;_LL306: {const char*_tmpCC5;void*_tmpCC4;(_tmpCC4=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCC5="impossible type for offsetof tuple index",
_tag_dyneither(_tmpCC5,sizeof(char),41))),_tag_dyneither(_tmpCC4,sizeof(void*),0)));}
_LL2FC:;}goto _LL221;_LL252: {struct Cyc_Absyn_Deref_e_struct*_tmp51E=(struct Cyc_Absyn_Deref_e_struct*)
_tmp4CE;if(_tmp51E->tag != 21)goto _LL254;else{_tmp51F=_tmp51E->f1;}}_LL253: {int
_tmp6AF=Cyc_Toc_in_lhs(nv);Cyc_Toc_set_lhs(nv,0);{void*_tmp6B0=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp51F->topt))->v);{void*_tmp6B1=
_tmp6B0;struct Cyc_Absyn_PtrInfo _tmp6B3;void*_tmp6B4;struct Cyc_Absyn_Tqual _tmp6B5;
struct Cyc_Absyn_PtrAtts _tmp6B6;void*_tmp6B7;union Cyc_Absyn_Constraint*_tmp6B8;
union Cyc_Absyn_Constraint*_tmp6B9;union Cyc_Absyn_Constraint*_tmp6BA;_LL308: {
struct Cyc_Absyn_PointerType_struct*_tmp6B2=(struct Cyc_Absyn_PointerType_struct*)
_tmp6B1;if(_tmp6B2->tag != 5)goto _LL30A;else{_tmp6B3=_tmp6B2->f1;_tmp6B4=_tmp6B3.elt_typ;
_tmp6B5=_tmp6B3.elt_tq;_tmp6B6=_tmp6B3.ptr_atts;_tmp6B7=_tmp6B6.rgn;_tmp6B8=
_tmp6B6.nullable;_tmp6B9=_tmp6B6.bounds;_tmp6BA=_tmp6B6.zero_term;}}_LL309:{void*
_tmp6BB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6B9);struct Cyc_Absyn_Exp*_tmp6BD;_LL30D: {struct Cyc_Absyn_Upper_b_struct*
_tmp6BC=(struct Cyc_Absyn_Upper_b_struct*)_tmp6BB;if(_tmp6BC->tag != 1)goto _LL30F;
else{_tmp6BD=_tmp6BC->f1;}}_LL30E: {int do_null_check=Cyc_Toc_need_null_check(
_tmp51F);Cyc_Toc_exp_to_c(nv,_tmp51F);if(do_null_check){if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCC8;void*_tmpCC7;(_tmpCC7=0,Cyc_Tcutil_warn(e->loc,((_tmpCC8="inserted null check due to dereference",
_tag_dyneither(_tmpCC8,sizeof(char),39))),_tag_dyneither(_tmpCC7,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpCC9;_tmp51F->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp6B0),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpCC9=_cycalloc(sizeof(*
_tmpCC9)),((_tmpCC9->hd=Cyc_Absyn_copy_exp(_tmp51F),((_tmpCC9->tl=0,_tmpCC9)))))),
0));};}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6BA)){unsigned int _tmp6C3;int _tmp6C4;struct _tuple10 _tmp6C2=Cyc_Evexp_eval_const_uint_exp(
_tmp6BD);_tmp6C3=_tmp6C2.f1;_tmp6C4=_tmp6C2.f2;if(!_tmp6C4  || _tmp6C3 <= 0){const
char*_tmpCCC;void*_tmpCCB;(_tmpCCB=0,Cyc_Tcutil_terr(e->loc,((_tmpCCC="cannot determine dereference is in bounds",
_tag_dyneither(_tmpCCC,sizeof(char),42))),_tag_dyneither(_tmpCCB,sizeof(void*),0)));}}
goto _LL30C;}_LL30F: {struct Cyc_Absyn_DynEither_b_struct*_tmp6BE=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp6BB;if(_tmp6BE->tag != 0)goto _LL30C;}_LL310: {struct Cyc_Absyn_Exp*_tmp6C7=Cyc_Absyn_uint_exp(
0,0);{struct Cyc_Core_Opt*_tmpCCD;_tmp6C7->topt=((_tmpCCD=_cycalloc(sizeof(*
_tmpCCD)),((_tmpCCD->v=Cyc_Absyn_uint_typ,_tmpCCD))));}e->r=Cyc_Toc_subscript_exp_r(
_tmp51F,_tmp6C7);Cyc_Toc_exp_to_c(nv,e);goto _LL30C;}_LL30C:;}goto _LL307;_LL30A:;
_LL30B: {const char*_tmpCD0;void*_tmpCCF;(_tmpCCF=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD0="exp_to_c: Deref: non-pointer",
_tag_dyneither(_tmpCD0,sizeof(char),29))),_tag_dyneither(_tmpCCF,sizeof(void*),0)));}
_LL307:;}Cyc_Toc_set_lhs(nv,_tmp6AF);goto _LL221;};}_LL254: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp520=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp4CE;if(_tmp520->tag != 22)goto
_LL256;else{_tmp521=_tmp520->f1;_tmp522=_tmp520->f2;_tmp523=_tmp520->f3;_tmp524=
_tmp520->f4;}}_LL255: {int is_poly=Cyc_Toc_is_poly_project(e);void*e1_cyc_type=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp521->topt))->v;Cyc_Toc_exp_to_c(nv,
_tmp521);if(_tmp523  && _tmp524)e->r=Cyc_Toc_check_tagged_union(_tmp521,Cyc_Toc_typ_to_c(
e1_cyc_type),e1_cyc_type,_tmp522,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrmember_exp);
if(is_poly)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp(e->r,0),0))->r;goto _LL221;}_LL256: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp525=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp4CE;if(_tmp525->tag != 23)goto _LL258;else{_tmp526=_tmp525->f1;_tmp527=_tmp525->f2;
_tmp528=_tmp525->f3;_tmp529=_tmp525->f4;}}_LL257: {int _tmp6CB=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp526->topt))->v);int do_null_check=Cyc_Toc_need_null_check(_tmp526);
Cyc_Toc_exp_to_c(nv,_tmp526);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp6CD;
struct Cyc_Absyn_Tqual _tmp6CE;struct Cyc_Absyn_PtrAtts _tmp6CF;void*_tmp6D0;union
Cyc_Absyn_Constraint*_tmp6D1;union Cyc_Absyn_Constraint*_tmp6D2;union Cyc_Absyn_Constraint*
_tmp6D3;struct Cyc_Absyn_PtrInfo _tmp6CC=Cyc_Toc_get_ptr_type(e1typ);_tmp6CD=
_tmp6CC.elt_typ;_tmp6CE=_tmp6CC.elt_tq;_tmp6CF=_tmp6CC.ptr_atts;_tmp6D0=_tmp6CF.rgn;
_tmp6D1=_tmp6CF.nullable;_tmp6D2=_tmp6CF.bounds;_tmp6D3=_tmp6CF.zero_term;{void*
_tmp6D4=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6D2);struct Cyc_Absyn_Exp*_tmp6D6;_LL312: {struct Cyc_Absyn_Upper_b_struct*
_tmp6D5=(struct Cyc_Absyn_Upper_b_struct*)_tmp6D4;if(_tmp6D5->tag != 1)goto _LL314;
else{_tmp6D6=_tmp6D5->f1;}}_LL313: {unsigned int _tmp6D9;int _tmp6DA;struct
_tuple10 _tmp6D8=Cyc_Evexp_eval_const_uint_exp(_tmp6D6);_tmp6D9=_tmp6D8.f1;
_tmp6DA=_tmp6D8.f2;if(_tmp6DA){if(_tmp6D9 < 1){const char*_tmpCD3;void*_tmpCD2;(
_tmpCD2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpCD3="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpCD3,
sizeof(char),44))),_tag_dyneither(_tmpCD2,sizeof(void*),0)));}if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCD6;void*_tmpCD5;(_tmpCD5=0,Cyc_Tcutil_warn(
e->loc,((_tmpCD6="inserted null check due to dereference",_tag_dyneither(_tmpCD6,
sizeof(char),39))),_tag_dyneither(_tmpCD5,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCD7[1];_tmp526->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpCD7[0]=Cyc_Absyn_new_exp(_tmp526->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD7,sizeof(struct Cyc_Absyn_Exp*),
1)))),0));};}}else{if(!Cyc_Evexp_c_can_eval(_tmp6D6)){const char*_tmpCDA;void*
_tmpCD9;(_tmpCD9=0,Cyc_Tcutil_terr(e->loc,((_tmpCDA="cannot determine pointer dereference in bounds",
_tag_dyneither(_tmpCDA,sizeof(char),47))),_tag_dyneither(_tmpCD9,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpCDB[4];_tmp526->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((_tmpCDB[3]=
Cyc_Absyn_uint_exp(0,0),((_tmpCDB[2]=Cyc_Absyn_sizeoftyp_exp(_tmp6CD,0),((
_tmpCDB[1]=_tmp6D6,((_tmpCDB[0]=Cyc_Absyn_new_exp(_tmp526->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCDB,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0));};}goto _LL311;}_LL314: {struct Cyc_Absyn_DynEither_b_struct*
_tmp6D7=(struct Cyc_Absyn_DynEither_b_struct*)_tmp6D4;if(_tmp6D7->tag != 0)goto
_LL311;}_LL315: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp6CD);{struct Cyc_Absyn_Exp*
_tmpCDC[3];_tmp526->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp6CE),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_dyneither_subscript_e,((_tmpCDC[2]=Cyc_Absyn_uint_exp(0,0),((
_tmpCDC[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCDC[0]=Cyc_Absyn_new_exp(_tmp526->r,
0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCDC,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}goto _LL311;}_LL311:;}if(
_tmp528  && _tmp529)e->r=Cyc_Toc_check_tagged_union(_tmp526,Cyc_Toc_typ_to_c(
e1typ),_tmp6CD,_tmp527,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);if(is_poly
 && _tmp529)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp(e->r,0),0))->r;Cyc_Toc_set_lhs(nv,
_tmp6CB);goto _LL221;};};}_LL258: {struct Cyc_Absyn_Subscript_e_struct*_tmp52A=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp4CE;if(_tmp52A->tag != 24)goto _LL25A;
else{_tmp52B=_tmp52A->f1;_tmp52C=_tmp52A->f2;}}_LL259: {int _tmp6E4=Cyc_Toc_in_lhs(
nv);Cyc_Toc_set_lhs(nv,0);{void*_tmp6E5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp52B->topt))->v);{void*_tmp6E6=_tmp6E5;struct Cyc_List_List*
_tmp6E8;struct Cyc_Absyn_PtrInfo _tmp6EA;void*_tmp6EB;struct Cyc_Absyn_Tqual _tmp6EC;
struct Cyc_Absyn_PtrAtts _tmp6ED;void*_tmp6EE;union Cyc_Absyn_Constraint*_tmp6EF;
union Cyc_Absyn_Constraint*_tmp6F0;union Cyc_Absyn_Constraint*_tmp6F1;_LL317: {
struct Cyc_Absyn_TupleType_struct*_tmp6E7=(struct Cyc_Absyn_TupleType_struct*)
_tmp6E6;if(_tmp6E7->tag != 11)goto _LL319;else{_tmp6E8=_tmp6E7->f1;}}_LL318: Cyc_Toc_exp_to_c(
nv,_tmp52B);Cyc_Toc_exp_to_c(nv,_tmp52C);{unsigned int _tmp6F3;int _tmp6F4;struct
_tuple10 _tmp6F2=Cyc_Evexp_eval_const_uint_exp(_tmp52C);_tmp6F3=_tmp6F2.f1;
_tmp6F4=_tmp6F2.f2;if(!_tmp6F4){const char*_tmpCDF;void*_tmpCDE;(_tmpCDE=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCDF="unknown tuple subscript in translation to C",
_tag_dyneither(_tmpCDF,sizeof(char),44))),_tag_dyneither(_tmpCDE,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp52B,Cyc_Absyn_fieldname((int)(_tmp6F3 + 1)));
goto _LL316;};_LL319: {struct Cyc_Absyn_PointerType_struct*_tmp6E9=(struct Cyc_Absyn_PointerType_struct*)
_tmp6E6;if(_tmp6E9->tag != 5)goto _LL31B;else{_tmp6EA=_tmp6E9->f1;_tmp6EB=_tmp6EA.elt_typ;
_tmp6EC=_tmp6EA.elt_tq;_tmp6ED=_tmp6EA.ptr_atts;_tmp6EE=_tmp6ED.rgn;_tmp6EF=
_tmp6ED.nullable;_tmp6F0=_tmp6ED.bounds;_tmp6F1=_tmp6ED.zero_term;}}_LL31A: {
struct Cyc_List_List*_tmp6F7=Cyc_Toc_get_relns(_tmp52B);int in_bnds=0;{void*
_tmp6F8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6F0);_LL31E:;_LL31F: in_bnds=Cyc_Toc_check_bounds(_tmp6E5,_tmp6F7,_tmp52B,
_tmp52C);if(Cyc_Toc_warn_bounds_checks  && !in_bnds){const char*_tmpCE3;void*
_tmpCE2[1];struct Cyc_String_pa_struct _tmpCE1;(_tmpCE1.tag=0,((_tmpCE1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCE2[0]=&
_tmpCE1,Cyc_Tcutil_warn(e->loc,((_tmpCE3="bounds check necessary for %s",
_tag_dyneither(_tmpCE3,sizeof(char),30))),_tag_dyneither(_tmpCE2,sizeof(void*),1)))))));}
_LL31D:;}Cyc_Toc_exp_to_c(nv,_tmp52B);Cyc_Toc_exp_to_c(nv,_tmp52C);++ Cyc_Toc_total_bounds_checks;{
void*_tmp6FC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp6F0);struct Cyc_Absyn_Exp*_tmp6FE;_LL321: {struct Cyc_Absyn_Upper_b_struct*
_tmp6FD=(struct Cyc_Absyn_Upper_b_struct*)_tmp6FC;if(_tmp6FD->tag != 1)goto _LL323;
else{_tmp6FE=_tmp6FD->f1;}}_LL322: {int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp6EF);void*ta1=Cyc_Toc_typ_to_c(_tmp6EB);void*ta2=
Cyc_Absyn_cstar_typ(ta1,_tmp6EC);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6F1)){if(!Cyc_Evexp_c_can_eval(_tmp6FE)){const char*_tmpCE6;void*_tmpCE5;(
_tmpCE5=0,Cyc_Tcutil_terr(e->loc,((_tmpCE6="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCE6,sizeof(char),40))),_tag_dyneither(_tmpCE5,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp52B);struct Cyc_Absyn_Exp*_tmpCE7[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpCE7[2]=_tmp52C,((_tmpCE7[1]=_tmp6FE,((
_tmpCE7[0]=_tmp52B,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCE7,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp6FE)){const char*_tmpCEA;void*_tmpCE9;(
_tmpCE9=0,Cyc_Tcutil_terr(e->loc,((_tmpCEA="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCEA,sizeof(char),40))),_tag_dyneither(_tmpCE9,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCED;void*_tmpCEC;(_tmpCEC=0,Cyc_Tcutil_warn(
e->loc,((_tmpCED="inserted null check due to dereference",_tag_dyneither(_tmpCED,
sizeof(char),39))),_tag_dyneither(_tmpCEC,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCEE[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((
_tmpCEE[3]=_tmp52C,((_tmpCEE[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCEE[1]=
_tmp6FE,((_tmpCEE[0]=_tmp52B,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCEE,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{if(
!Cyc_Evexp_c_can_eval(_tmp6FE)){const char*_tmpCF1;void*_tmpCF0;(_tmpCF0=0,Cyc_Tcutil_terr(
e->loc,((_tmpCF1="cannot determine subscript is in bounds",_tag_dyneither(
_tmpCF1,sizeof(char),40))),_tag_dyneither(_tmpCF0,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCF2[2];_tmp52C->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((
_tmpCF2[1]=Cyc_Absyn_copy_exp(_tmp52C),((_tmpCF2[0]=_tmp6FE,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCF2,sizeof(struct Cyc_Absyn_Exp*),
2)))))));};}}}goto _LL320;}_LL323: {struct Cyc_Absyn_DynEither_b_struct*_tmp6FF=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp6FC;if(_tmp6FF->tag != 0)goto _LL320;}
_LL324: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp6EB);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6EC),
Cyc_Absyn_aggrmember_exp(_tmp52B,Cyc_Toc_curr_sp,0)),_tmp52C);}else{struct Cyc_Absyn_Exp*
_tmp70B=Cyc_Toc__check_dyneither_subscript_e;struct Cyc_Absyn_Exp*_tmpCF3[3];e->r=
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6EC),Cyc_Absyn_fncall_exp(
_tmp70B,((_tmpCF3[2]=_tmp52C,((_tmpCF3[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((
_tmpCF3[0]=_tmp52B,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCF3,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL320;}
_LL320:;}goto _LL316;}_LL31B:;_LL31C: {const char*_tmpCF6;void*_tmpCF5;(_tmpCF5=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCF6="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(
_tmpCF6,sizeof(char),49))),_tag_dyneither(_tmpCF5,sizeof(void*),0)));}_LL316:;}
Cyc_Toc_set_lhs(nv,_tmp6E4);goto _LL221;};}_LL25A: {struct Cyc_Absyn_Tuple_e_struct*
_tmp52D=(struct Cyc_Absyn_Tuple_e_struct*)_tmp4CE;if(_tmp52D->tag != 25)goto _LL25C;
else{_tmp52E=_tmp52D->f1;}}_LL25B: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(
nv,0,0,_tmp52E))->r;else{struct Cyc_List_List*_tmp70F=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_tup_to_c,_tmp52E);void*_tmp710=Cyc_Toc_add_tuple_type(_tmp70F);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp52E != 0;(_tmp52E=_tmp52E->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp52E->hd);{struct _tuple15*_tmpCF9;struct Cyc_List_List*
_tmpCF8;dles=((_tmpCF8=_cycalloc(sizeof(*_tmpCF8)),((_tmpCF8->hd=((_tmpCF9=
_cycalloc(sizeof(*_tmpCF9)),((_tmpCF9->f1=0,((_tmpCF9->f2=(struct Cyc_Absyn_Exp*)
_tmp52E->hd,_tmpCF9)))))),((_tmpCF8->tl=dles,_tmpCF8))))));};}}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);e->r=Cyc_Toc_unresolvedmem_exp_r(
0,dles);}goto _LL221;_LL25C: {struct Cyc_Absyn_Array_e_struct*_tmp52F=(struct Cyc_Absyn_Array_e_struct*)
_tmp4CE;if(_tmp52F->tag != 27)goto _LL25E;else{_tmp530=_tmp52F->f1;}}_LL25D: e->r=
Cyc_Toc_unresolvedmem_exp_r(0,_tmp530);{struct Cyc_List_List*_tmp713=_tmp530;for(
0;_tmp713 != 0;_tmp713=_tmp713->tl){struct _tuple15 _tmp715;struct Cyc_Absyn_Exp*
_tmp716;struct _tuple15*_tmp714=(struct _tuple15*)_tmp713->hd;_tmp715=*_tmp714;
_tmp716=_tmp715.f2;Cyc_Toc_exp_to_c(nv,_tmp716);}}goto _LL221;_LL25E: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp531=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp4CE;if(_tmp531->tag != 28)
goto _LL260;else{_tmp532=_tmp531->f1;_tmp533=_tmp531->f2;_tmp534=_tmp531->f3;
_tmp535=_tmp531->f4;}}_LL25F: {unsigned int _tmp718;int _tmp719;struct _tuple10
_tmp717=Cyc_Evexp_eval_const_uint_exp(_tmp533);_tmp718=_tmp717.f1;_tmp719=
_tmp717.f2;{void*_tmp71A=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp534->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp534);{struct Cyc_List_List*
es=0;if(!Cyc_Toc_is_zero(_tmp534)){if(!_tmp719){const char*_tmpCFC;void*_tmpCFB;(
_tmpCFB=0,Cyc_Tcutil_terr(_tmp533->loc,((_tmpCFC="cannot determine value of constant",
_tag_dyneither(_tmpCFC,sizeof(char),35))),_tag_dyneither(_tmpCFB,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp718;++ i){struct _tuple15*_tmpCFF;struct Cyc_List_List*
_tmpCFE;es=((_tmpCFE=_cycalloc(sizeof(*_tmpCFE)),((_tmpCFE->hd=((_tmpCFF=
_cycalloc(sizeof(*_tmpCFF)),((_tmpCFF->f1=0,((_tmpCFF->f2=_tmp534,_tmpCFF)))))),((
_tmpCFE->tl=es,_tmpCFE))))));}}if(_tmp535){struct Cyc_Absyn_Exp*_tmp71F=Cyc_Toc_cast_it(
_tmp71A,Cyc_Absyn_uint_exp(0,0));struct _tuple15*_tmpD02;struct Cyc_List_List*
_tmpD01;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,((_tmpD01=_cycalloc(sizeof(*_tmpD01)),((_tmpD01->hd=((
_tmpD02=_cycalloc(sizeof(*_tmpD02)),((_tmpD02->f1=0,((_tmpD02->f2=_tmp71F,
_tmpD02)))))),((_tmpD01->tl=0,_tmpD01)))))));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL221;};};}_LL260: {struct Cyc_Absyn_Aggregate_e_struct*_tmp536=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp4CE;if(_tmp536->tag != 29)goto _LL262;
else{_tmp537=_tmp536->f1;_tmp538=_tmp536->f2;_tmp539=_tmp536->f3;_tmp53A=_tmp536->f4;}}
_LL261: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp538
!= 0,0,0,_tmp539,_tmp537))->r;else{if(_tmp53A == 0){const char*_tmpD05;void*
_tmpD04;(_tmpD04=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD05="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpD05,sizeof(
char),38))),_tag_dyneither(_tmpD04,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp53A;struct Cyc_Toc_Env _tmp725;struct
_RegionHandle*_tmp726;struct Cyc_Toc_Env*_tmp724=nv;_tmp725=*_tmp724;_tmp726=
_tmp725.rgn;{struct Cyc_List_List*_tmp727=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum 
Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp726,e->loc,_tmp539,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp53A->impl))->tagged){struct
_tuple20 _tmp729;struct Cyc_Absyn_Aggrfield*_tmp72A;struct Cyc_Absyn_Exp*_tmp72B;
struct _tuple20*_tmp728=(struct _tuple20*)((struct Cyc_List_List*)_check_null(
_tmp727))->hd;_tmp729=*_tmp728;_tmp72A=_tmp729.f1;_tmp72B=_tmp729.f2;{void*
_tmp72C=_tmp72A->type;Cyc_Toc_exp_to_c(nv,_tmp72B);if(Cyc_Toc_is_void_star_or_tvar(
_tmp72C))_tmp72B->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp72B->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp53A,_tmp72A->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple15*_tmpD0A;struct _tuple15*_tmpD09;struct _tuple15*_tmpD08[2];
struct Cyc_List_List*_tmp72D=(_tmpD08[1]=((_tmpD09=_cycalloc(sizeof(*_tmpD09)),((
_tmpD09->f1=0,((_tmpD09->f2=_tmp72B,_tmpD09)))))),((_tmpD08[0]=((_tmpD0A=
_cycalloc(sizeof(*_tmpD0A)),((_tmpD0A->f1=0,((_tmpD0A->f2=field_tag_exp,_tmpD0A)))))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD08,sizeof(struct _tuple15*),2)))));struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(
0,_tmp72D,0);struct Cyc_Absyn_FieldName_struct*_tmpD10;struct Cyc_Absyn_FieldName_struct
_tmpD0F;void*_tmpD0E[1];struct Cyc_List_List*ds=(_tmpD0E[0]=(void*)((_tmpD10=
_cycalloc(sizeof(*_tmpD10)),((_tmpD10[0]=((_tmpD0F.tag=1,((_tmpD0F.f1=_tmp72A->name,
_tmpD0F)))),_tmpD10)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD0E,sizeof(void*),1)));struct _tuple15*_tmpD13;struct _tuple15*
_tmpD12[1];struct Cyc_List_List*dles=(_tmpD12[0]=((_tmpD13=_cycalloc(sizeof(*
_tmpD13)),((_tmpD13->f1=ds,((_tmpD13->f2=umem,_tmpD13)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD12,sizeof(struct
_tuple15*),1)));e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{struct Cyc_List_List*
_tmp736=0;struct Cyc_List_List*_tmp737=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp737 != 0;_tmp737=_tmp737->tl){struct Cyc_List_List*
_tmp738=_tmp727;for(0;_tmp738 != 0;_tmp738=_tmp738->tl){if((*((struct _tuple20*)
_tmp738->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp737->hd){struct _tuple20
_tmp73A;struct Cyc_Absyn_Aggrfield*_tmp73B;struct Cyc_Absyn_Exp*_tmp73C;struct
_tuple20*_tmp739=(struct _tuple20*)_tmp738->hd;_tmp73A=*_tmp739;_tmp73B=_tmp73A.f1;
_tmp73C=_tmp73A.f2;{void*_tmp73D=_tmp73B->type;Cyc_Toc_exp_to_c(nv,_tmp73C);if(
Cyc_Toc_is_void_star_or_tvar(_tmp73D))_tmp73C->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp73C->r,0));{struct _tuple15*_tmpD16;struct Cyc_List_List*
_tmpD15;_tmp736=((_tmpD15=_cycalloc(sizeof(*_tmpD15)),((_tmpD15->hd=((_tmpD16=
_cycalloc(sizeof(*_tmpD16)),((_tmpD16->f1=0,((_tmpD16->f2=_tmp73C,_tmpD16)))))),((
_tmpD15->tl=_tmp736,_tmpD15))))));}break;};}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp736));}};};}
goto _LL221;_LL262: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp53B=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp4CE;if(_tmp53B->tag != 30)goto _LL264;else{_tmp53C=(void*)_tmp53B->f1;_tmp53D=
_tmp53B->f2;}}_LL263: {struct Cyc_List_List*fs;{void*_tmp740=Cyc_Tcutil_compress(
_tmp53C);struct Cyc_List_List*_tmp742;_LL326: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp741=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp740;if(_tmp741->tag != 13)goto
_LL328;else{_tmp742=_tmp741->f2;}}_LL327: fs=_tmp742;goto _LL325;_LL328:;_LL329: {
const char*_tmpD1A;void*_tmpD19[1];struct Cyc_String_pa_struct _tmpD18;(_tmpD18.tag=
0,((_tmpD18.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp53C)),((_tmpD19[0]=& _tmpD18,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD1A="anon struct has type %s",
_tag_dyneither(_tmpD1A,sizeof(char),24))),_tag_dyneither(_tmpD19,sizeof(void*),1)))))));}
_LL325:;}{struct Cyc_Toc_Env _tmp747;struct _RegionHandle*_tmp748;struct Cyc_Toc_Env*
_tmp746=nv;_tmp747=*_tmp746;_tmp748=_tmp747.rgn;{struct Cyc_List_List*_tmp749=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp748,e->loc,_tmp53D,Cyc_Absyn_StructA,fs);for(0;_tmp749 != 0;_tmp749=_tmp749->tl){
struct _tuple20 _tmp74B;struct Cyc_Absyn_Aggrfield*_tmp74C;struct Cyc_Absyn_Exp*
_tmp74D;struct _tuple20*_tmp74A=(struct _tuple20*)_tmp749->hd;_tmp74B=*_tmp74A;
_tmp74C=_tmp74B.f1;_tmp74D=_tmp74B.f2;{void*_tmp74E=_tmp74C->type;Cyc_Toc_exp_to_c(
nv,_tmp74D);if(Cyc_Toc_is_void_star_or_tvar(_tmp74E))_tmp74D->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp74D->r,0));};}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp53D);}goto _LL221;};}_LL264: {struct Cyc_Absyn_Datatype_e_struct*_tmp53E=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp4CE;if(_tmp53E->tag != 31)goto _LL266;else{
_tmp53F=_tmp53E->f1;_tmp540=_tmp53E->f2;_tmp541=_tmp53E->f3;}}_LL265: {void*
datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple0*_tmp74F=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp750=Cyc_Absyn_var_exp(_tmp74F,0);struct Cyc_Absyn_Exp*
member_exp;{const char*_tmpD1B;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp541->name,((_tmpD1B="_struct",_tag_dyneither(_tmpD1B,sizeof(char),8)))));}
tag_exp=_tmp540->is_extensible?Cyc_Absyn_var_exp(_tmp541->name,0): Cyc_Toc_datatype_tag(
_tmp540,_tmp541->name);member_exp=_tmp750;{struct Cyc_List_List*_tmp752=_tmp541->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp53F != 0;(
_tmp53F=_tmp53F->tl,_tmp752=_tmp752->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp53F->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp752))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct _tuple15*_tmpD1E;struct
Cyc_List_List*_tmpD1D;dles=((_tmpD1D=_cycalloc(sizeof(*_tmpD1D)),((_tmpD1D->hd=((
_tmpD1E=_cycalloc(sizeof(*_tmpD1E)),((_tmpD1E->f1=0,((_tmpD1E->f2=cur_e,_tmpD1E)))))),((
_tmpD1D->tl=dles,_tmpD1D))))));};}{struct _tuple15*_tmpD21;struct Cyc_List_List*
_tmpD20;dles=((_tmpD20=_cycalloc(sizeof(*_tmpD20)),((_tmpD20->hd=((_tmpD21=
_cycalloc(sizeof(*_tmpD21)),((_tmpD21->f1=0,((_tmpD21->f2=tag_exp,_tmpD21)))))),((
_tmpD20->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles),_tmpD20))))));}e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{struct Cyc_List_List*
_tmpD22;struct Cyc_List_List*_tmp757=(_tmpD22=_cycalloc(sizeof(*_tmpD22)),((
_tmpD22->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,
0),tag_exp,0),((_tmpD22->tl=0,_tmpD22)))));{int i=1;for(0;_tmp53F != 0;(((_tmp53F=
_tmp53F->tl,i ++)),_tmp752=_tmp752->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp53F->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp752))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp758=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpD23;_tmp757=((_tmpD23=_cycalloc(sizeof(*
_tmpD23)),((_tmpD23->hd=_tmp758,((_tmpD23->tl=_tmp757,_tmpD23))))));};}}{struct
Cyc_Absyn_Stmt*_tmp75A=Cyc_Absyn_exp_stmt(_tmp750,0);struct Cyc_List_List*_tmpD24;
struct Cyc_Absyn_Stmt*_tmp75B=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpD24=_cycalloc(sizeof(*_tmpD24)),((
_tmpD24->hd=_tmp75A,((_tmpD24->tl=_tmp757,_tmpD24))))))),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp74F,datatype_ctype,0,_tmp75B,0));};}goto _LL221;};}
_LL266: {struct Cyc_Absyn_Enum_e_struct*_tmp542=(struct Cyc_Absyn_Enum_e_struct*)
_tmp4CE;if(_tmp542->tag != 32)goto _LL268;}_LL267: goto _LL269;_LL268: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp543=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp4CE;if(_tmp543->tag != 33)goto
_LL26A;}_LL269: goto _LL221;_LL26A: {struct Cyc_Absyn_Malloc_e_struct*_tmp544=(
struct Cyc_Absyn_Malloc_e_struct*)_tmp4CE;if(_tmp544->tag != 34)goto _LL26C;else{
_tmp545=_tmp544->f1;_tmp546=_tmp545.is_calloc;_tmp547=_tmp545.rgn;_tmp548=
_tmp545.elt_type;_tmp549=_tmp545.num_elts;_tmp54A=_tmp545.fat_result;}}_LL26B: {
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp548)));Cyc_Toc_exp_to_c(nv,
_tmp549);if(_tmp54A){struct _tuple0*_tmp75E=Cyc_Toc_temp_var();struct _tuple0*
_tmp75F=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp546){xexp=_tmp549;if(_tmp547 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp547;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp75E,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp548,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp75E,0));}{struct Cyc_Absyn_Exp*_tmpD25[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpD25[2]=Cyc_Absyn_var_exp(_tmp75E,0),((_tmpD25[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpD25[0]=Cyc_Absyn_var_exp(_tmp75F,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD25,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{xexp=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp549,0);if(_tmp547 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp547;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp75E,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp548,Cyc_Absyn_var_exp(_tmp75E,0));}{struct Cyc_Absyn_Exp*_tmpD26[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpD26[2]=Cyc_Absyn_var_exp(_tmp75E,0),((_tmpD26[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpD26[0]=Cyc_Absyn_var_exp(_tmp75F,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD26,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);};}{struct Cyc_Absyn_Stmt*_tmp762=Cyc_Absyn_declare_stmt(_tmp75E,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp75F,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp762);};}else{struct Cyc_Absyn_Exp*_tmp763=Cyc_Absyn_sizeoftyp_exp(
t_c,0);{void*_tmp764=_tmp549->r;union Cyc_Absyn_Cnst _tmp766;struct _tuple5 _tmp767;
int _tmp768;_LL32B: {struct Cyc_Absyn_Const_e_struct*_tmp765=(struct Cyc_Absyn_Const_e_struct*)
_tmp764;if(_tmp765->tag != 0)goto _LL32D;else{_tmp766=_tmp765->f1;if((_tmp766.Int_c).tag
!= 4)goto _LL32D;_tmp767=(struct _tuple5)(_tmp766.Int_c).val;_tmp768=_tmp767.f2;
if(_tmp768 != 1)goto _LL32D;}}_LL32C: goto _LL32A;_LL32D:;_LL32E: _tmp763=Cyc_Absyn_times_exp(
_tmp763,_tmp549,0);goto _LL32A;_LL32A:;}if(_tmp547 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp547;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rmalloc_exp(rgn,_tmp763))->r;}else{e->r=(Cyc_Toc_malloc_exp(*_tmp548,
_tmp763))->r;}}goto _LL221;}_LL26C: {struct Cyc_Absyn_Swap_e_struct*_tmp54B=(
struct Cyc_Absyn_Swap_e_struct*)_tmp4CE;if(_tmp54B->tag != 35)goto _LL26E;else{
_tmp54C=_tmp54B->f1;_tmp54D=_tmp54B->f2;}}_LL26D: {int is_dyneither_ptr=0;void*
e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp54C->topt))->v;void*
e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp54D->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(
e1_old_typ,& is_dyneither_ptr)){const char*_tmpD29;void*_tmpD28;(_tmpD28=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD29="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpD29,sizeof(char),57))),_tag_dyneither(_tmpD28,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}{int f1_tag=0;void*tagged_mem_type1=(void*)&
Cyc_Absyn_VoidType_val;int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp54C,&
f1_tag,& tagged_mem_type1,1);int f2_tag=0;void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp54D,& f2_tag,& tagged_mem_type2,1);
Cyc_Toc_set_lhs(nv,1);Cyc_Toc_exp_to_c(nv,_tmp54C);Cyc_Toc_exp_to_c(nv,_tmp54D);
Cyc_Toc_set_lhs(nv,0);if(e1_tmem)Cyc_Toc_add_tagged_union_check_for_swap(_tmp54C,
f1_tag,tagged_mem_type1);else{_tmp54C=Cyc_Toc_push_address_exp(_tmp54C);}if(
e2_tmem)Cyc_Toc_add_tagged_union_check_for_swap(_tmp54D,f2_tag,tagged_mem_type2);
else{_tmp54D=Cyc_Toc_push_address_exp(_tmp54D);}{struct Cyc_Absyn_Exp*_tmpD2A[2];
e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpD2A[1]=_tmp54D,((_tmpD2A[0]=_tmp54C,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpD2A,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL221;};};}_LL26E: {struct
Cyc_Absyn_Tagcheck_e_struct*_tmp54E=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp4CE;
if(_tmp54E->tag != 38)goto _LL270;else{_tmp54F=_tmp54E->f1;_tmp550=_tmp54E->f2;}}
_LL26F: {void*_tmp76C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp54F->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp54F);{void*_tmp76D=
_tmp76C;struct Cyc_Absyn_AggrInfo _tmp76F;union Cyc_Absyn_AggrInfoU _tmp770;struct
Cyc_Absyn_Aggrdecl**_tmp771;struct Cyc_Absyn_Aggrdecl*_tmp772;_LL330: {struct Cyc_Absyn_AggrType_struct*
_tmp76E=(struct Cyc_Absyn_AggrType_struct*)_tmp76D;if(_tmp76E->tag != 12)goto
_LL332;else{_tmp76F=_tmp76E->f1;_tmp770=_tmp76F.aggr_info;if((_tmp770.KnownAggr).tag
!= 2)goto _LL332;_tmp771=(struct Cyc_Absyn_Aggrdecl**)(_tmp770.KnownAggr).val;
_tmp772=*_tmp771;}}_LL331: {struct Cyc_Absyn_Exp*_tmp773=Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp772,_tmp550),0);struct Cyc_Absyn_Exp*_tmp774=Cyc_Absyn_aggrmember_exp(
_tmp54F,_tmp550,0);struct Cyc_Absyn_Exp*_tmp775=Cyc_Absyn_aggrmember_exp(_tmp774,
Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(_tmp775,_tmp773,0))->r;goto _LL32F;}
_LL332:;_LL333: {const char*_tmpD2D;void*_tmpD2C;(_tmpD2C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD2D="non-aggregate type in tagcheck",
_tag_dyneither(_tmpD2D,sizeof(char),31))),_tag_dyneither(_tmpD2C,sizeof(void*),0)));}
_LL32F:;}goto _LL221;}_LL270: {struct Cyc_Absyn_StmtExp_e_struct*_tmp551=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp4CE;if(_tmp551->tag != 37)goto _LL272;else{_tmp552=
_tmp551->f1;}}_LL271: Cyc_Toc_stmt_to_c(nv,_tmp552);goto _LL221;_LL272: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp553=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp4CE;if(_tmp553->tag != 36)
goto _LL274;}_LL273: {const char*_tmpD30;void*_tmpD2F;(_tmpD2F=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD30="UnresolvedMem",
_tag_dyneither(_tmpD30,sizeof(char),14))),_tag_dyneither(_tmpD2F,sizeof(void*),0)));}
_LL274: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp554=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp4CE;if(_tmp554->tag != 26)goto _LL276;}_LL275: {const char*_tmpD33;void*_tmpD32;(
_tmpD32=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpD33="compoundlit",_tag_dyneither(_tmpD33,sizeof(char),12))),_tag_dyneither(
_tmpD32,sizeof(void*),0)));}_LL276: {struct Cyc_Absyn_Valueof_e_struct*_tmp555=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp4CE;if(_tmp555->tag != 39)goto _LL278;}
_LL277: {const char*_tmpD36;void*_tmpD35;(_tmpD35=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD36="valueof(-)",
_tag_dyneither(_tmpD36,sizeof(char),11))),_tag_dyneither(_tmpD35,sizeof(void*),0)));}
_LL278: {struct Cyc_Absyn_Asm_e_struct*_tmp556=(struct Cyc_Absyn_Asm_e_struct*)
_tmp4CE;if(_tmp556->tag != 40)goto _LL221;}_LL279: {const char*_tmpD39;void*_tmpD38;(
_tmpD38=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD39="__asm__",_tag_dyneither(_tmpD39,sizeof(char),8))),_tag_dyneither(
_tmpD38,sizeof(void*),0)));}_LL221:;};}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0);}struct _tuple21{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*
f3;};struct _tuple22{struct _tuple0*f1;void*f2;};struct _tuple23{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple21 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*
nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
struct Cyc_Absyn_Stmt*s;{void*_tmp780=p->r;struct Cyc_Absyn_Vardecl*_tmp782;struct
Cyc_Absyn_Vardecl _tmp783;struct _tuple0*_tmp784;struct Cyc_Absyn_Pat*_tmp785;
struct Cyc_Absyn_Vardecl*_tmp787;struct Cyc_Absyn_Vardecl _tmp788;struct _tuple0*
_tmp789;struct Cyc_Absyn_Vardecl*_tmp78C;struct Cyc_Absyn_Pat*_tmp78D;enum Cyc_Absyn_Sign
_tmp790;int _tmp791;char _tmp793;struct _dyneither_ptr _tmp795;struct Cyc_Absyn_Enumdecl*
_tmp797;struct Cyc_Absyn_Enumfield*_tmp798;void*_tmp79A;struct Cyc_Absyn_Enumfield*
_tmp79B;struct Cyc_Absyn_Pat*_tmp79D;struct Cyc_Absyn_Pat _tmp79E;void*_tmp79F;
struct Cyc_Absyn_Datatypedecl*_tmp7A1;struct Cyc_Absyn_Datatypefield*_tmp7A2;
struct Cyc_List_List*_tmp7A3;struct Cyc_List_List*_tmp7A5;struct Cyc_List_List*
_tmp7A7;struct Cyc_Absyn_AggrInfo*_tmp7A9;struct Cyc_Absyn_AggrInfo*_tmp7AB;struct
Cyc_Absyn_AggrInfo _tmp7AC;union Cyc_Absyn_AggrInfoU _tmp7AD;struct Cyc_List_List*
_tmp7AE;struct Cyc_Absyn_Pat*_tmp7B0;_LL335: {struct Cyc_Absyn_Var_p_struct*
_tmp781=(struct Cyc_Absyn_Var_p_struct*)_tmp780;if(_tmp781->tag != 1)goto _LL337;
else{_tmp782=_tmp781->f1;_tmp783=*_tmp782;_tmp784=_tmp783.name;_tmp785=_tmp781->f2;}}
_LL336: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp784,r),rgn,t,r,path,
_tmp785,fail_stmt,decls);_LL337: {struct Cyc_Absyn_TagInt_p_struct*_tmp786=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp780;if(_tmp786->tag != 3)goto _LL339;else{
_tmp787=_tmp786->f2;_tmp788=*_tmp787;_tmp789=_tmp788.name;}}_LL338: nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp789,r);goto _LL33A;_LL339: {struct Cyc_Absyn_Wild_p_struct*_tmp78A=(
struct Cyc_Absyn_Wild_p_struct*)_tmp780;if(_tmp78A->tag != 0)goto _LL33B;}_LL33A: s=
Cyc_Toc_skip_stmt_dl();goto _LL334;_LL33B: {struct Cyc_Absyn_Reference_p_struct*
_tmp78B=(struct Cyc_Absyn_Reference_p_struct*)_tmp780;if(_tmp78B->tag != 2)goto
_LL33D;else{_tmp78C=_tmp78B->f1;_tmp78D=_tmp78B->f2;}}_LL33C: {struct _tuple0*
_tmp7B4=Cyc_Toc_temp_var();{struct _tuple22*_tmpD3C;struct Cyc_List_List*_tmpD3B;
decls=((_tmpD3B=_region_malloc(rgn,sizeof(*_tmpD3B)),((_tmpD3B->hd=((_tmpD3C=
_region_malloc(rgn,sizeof(*_tmpD3C)),((_tmpD3C->f1=_tmp7B4,((_tmpD3C->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpD3C)))))),((_tmpD3B->tl=decls,_tmpD3B))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp78C->name,Cyc_Absyn_var_exp(_tmp7B4,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp7B4,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Toc_push_address_exp(path)),0);{struct _tuple21 _tmp7B7=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp78D,fail_stmt,decls);_tmp7B7.f3=Cyc_Absyn_seq_stmt(s,_tmp7B7.f3,
0);return _tmp7B7;};}_LL33D: {struct Cyc_Absyn_Null_p_struct*_tmp78E=(struct Cyc_Absyn_Null_p_struct*)
_tmp780;if(_tmp78E->tag != 8)goto _LL33F;}_LL33E: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),fail_stmt);goto _LL334;_LL33F: {struct Cyc_Absyn_Int_p_struct*_tmp78F=(struct
Cyc_Absyn_Int_p_struct*)_tmp780;if(_tmp78F->tag != 9)goto _LL341;else{_tmp790=
_tmp78F->f1;_tmp791=_tmp78F->f2;}}_LL340: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp790,_tmp791,0),fail_stmt);goto _LL334;_LL341: {struct Cyc_Absyn_Char_p_struct*
_tmp792=(struct Cyc_Absyn_Char_p_struct*)_tmp780;if(_tmp792->tag != 10)goto _LL343;
else{_tmp793=_tmp792->f1;}}_LL342: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(
_tmp793,0),fail_stmt);goto _LL334;_LL343: {struct Cyc_Absyn_Float_p_struct*_tmp794=(
struct Cyc_Absyn_Float_p_struct*)_tmp780;if(_tmp794->tag != 11)goto _LL345;else{
_tmp795=_tmp794->f1;}}_LL344: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp795,
0),fail_stmt);goto _LL334;_LL345: {struct Cyc_Absyn_Enum_p_struct*_tmp796=(struct
Cyc_Absyn_Enum_p_struct*)_tmp780;if(_tmp796->tag != 12)goto _LL347;else{_tmp797=
_tmp796->f1;_tmp798=_tmp796->f2;}}_LL346:{struct Cyc_Absyn_Enum_e_struct _tmpD3F;
struct Cyc_Absyn_Enum_e_struct*_tmpD3E;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD3E=_cycalloc(sizeof(*_tmpD3E)),((_tmpD3E[0]=((_tmpD3F.tag=32,((
_tmpD3F.f1=_tmp798->name,((_tmpD3F.f2=(struct Cyc_Absyn_Enumdecl*)_tmp797,((
_tmpD3F.f3=(struct Cyc_Absyn_Enumfield*)_tmp798,_tmpD3F)))))))),_tmpD3E)))),0),
fail_stmt);}goto _LL334;_LL347: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp799=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp780;if(_tmp799->tag != 13)goto _LL349;else{
_tmp79A=(void*)_tmp799->f1;_tmp79B=_tmp799->f2;}}_LL348:{struct Cyc_Absyn_AnonEnum_e_struct
_tmpD42;struct Cyc_Absyn_AnonEnum_e_struct*_tmpD41;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD41=_cycalloc(sizeof(*_tmpD41)),((_tmpD41[0]=((_tmpD42.tag=33,((
_tmpD42.f1=_tmp79B->name,((_tmpD42.f2=(void*)_tmp79A,((_tmpD42.f3=(struct Cyc_Absyn_Enumfield*)
_tmp79B,_tmpD42)))))))),_tmpD41)))),0),fail_stmt);}goto _LL334;_LL349: {struct Cyc_Absyn_Pointer_p_struct*
_tmp79C=(struct Cyc_Absyn_Pointer_p_struct*)_tmp780;if(_tmp79C->tag != 5)goto
_LL34B;else{_tmp79D=_tmp79C->f1;_tmp79E=*_tmp79D;_tmp79F=_tmp79E.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp7A0=(struct Cyc_Absyn_Datatype_p_struct*)_tmp79F;if(_tmp7A0->tag != 7)goto
_LL34B;else{_tmp7A1=_tmp7A0->f1;_tmp7A2=_tmp7A0->f2;_tmp7A3=_tmp7A0->f3;}};}}
_LL34A: s=Cyc_Toc_skip_stmt_dl();{struct _tuple0*_tmp7BC=Cyc_Toc_temp_var();const
char*_tmpD43;struct _tuple0*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp7A2->name,((
_tmpD43="_struct",_tag_dyneither(_tmpD43,sizeof(char),8))));void*_tmp7BD=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);int cnt=1;struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
_tmp7BD,r);struct Cyc_List_List*_tmp7BE=_tmp7A2->typs;for(0;_tmp7A3 != 0;(((
_tmp7A3=_tmp7A3->tl,_tmp7BE=((struct Cyc_List_List*)_check_null(_tmp7BE))->tl)),
++ cnt)){struct Cyc_Absyn_Pat*_tmp7BF=(struct Cyc_Absyn_Pat*)_tmp7A3->hd;if(_tmp7BF->r
== (void*)& Cyc_Absyn_Wild_p_val)continue;{void*_tmp7C0=(*((struct _tuple9*)((
struct Cyc_List_List*)_check_null(_tmp7BE))->hd)).f2;struct _tuple0*_tmp7C1=Cyc_Toc_temp_var();
void*_tmp7C2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp7BF->topt))->v;void*
_tmp7C3=Cyc_Toc_typ_to_c(_tmp7C2);struct Cyc_Absyn_Exp*_tmp7C4=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp7BC,0),Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_tvar(
Cyc_Toc_typ_to_c(_tmp7C0)))_tmp7C4=Cyc_Toc_cast_it(_tmp7C3,_tmp7C4);{struct
_tuple22*_tmpD46;struct Cyc_List_List*_tmpD45;decls=((_tmpD45=_region_malloc(rgn,
sizeof(*_tmpD45)),((_tmpD45->hd=((_tmpD46=_region_malloc(rgn,sizeof(*_tmpD46)),((
_tmpD46->f1=_tmp7C1,((_tmpD46->f2=_tmp7C3,_tmpD46)))))),((_tmpD45->tl=decls,
_tmpD45))))));}{struct _tuple21 _tmp7C7=Cyc_Toc_xlate_pat(nv,rgn,_tmp7C2,Cyc_Absyn_var_exp(
_tmp7C1,0),_tmp7C4,_tmp7BF,fail_stmt,decls);nv=_tmp7C7.f1;decls=_tmp7C7.f2;{
struct Cyc_Absyn_Stmt*_tmp7C8=_tmp7C7.f3;struct Cyc_Absyn_Stmt*_tmp7C9=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp7C1,0),_tmp7C4,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp7C9,_tmp7C8,0),0);};};};}{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp7BC,0),Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*tag_exp=
_tmp7A1->is_extensible?Cyc_Absyn_var_exp(_tmp7A2->name,0): Cyc_Toc_datatype_tag(
_tmp7A1,_tmp7A2->name);struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,
tag_exp,0);s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);if(Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v))s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp7BC,0),Cyc_Absyn_uint_exp(0,0),0),
fail_stmt,s,0);s=Cyc_Absyn_declare_stmt(_tmp7BC,_tmp7BD,(struct Cyc_Absyn_Exp*)
rcast,s,0);goto _LL334;};};_LL34B: {struct Cyc_Absyn_Datatype_p_struct*_tmp7A4=(
struct Cyc_Absyn_Datatype_p_struct*)_tmp780;if(_tmp7A4->tag != 7)goto _LL34D;else{
_tmp7A5=_tmp7A4->f3;}}_LL34C: _tmp7A7=_tmp7A5;goto _LL34E;_LL34D: {struct Cyc_Absyn_Tuple_p_struct*
_tmp7A6=(struct Cyc_Absyn_Tuple_p_struct*)_tmp780;if(_tmp7A6->tag != 4)goto _LL34F;
else{_tmp7A7=_tmp7A6->f1;}}_LL34E: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;
_tmp7A7 != 0;(_tmp7A7=_tmp7A7->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp7CB=(struct Cyc_Absyn_Pat*)
_tmp7A7->hd;if(_tmp7CB->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{struct _tuple0*
_tmp7CC=Cyc_Toc_temp_var();void*_tmp7CD=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7CB->topt))->v;{struct _tuple22*_tmpD49;struct Cyc_List_List*_tmpD48;decls=((
_tmpD48=_region_malloc(rgn,sizeof(*_tmpD48)),((_tmpD48->hd=((_tmpD49=
_region_malloc(rgn,sizeof(*_tmpD49)),((_tmpD49->f1=_tmp7CC,((_tmpD49->f2=Cyc_Toc_typ_to_c(
_tmp7CD),_tmpD49)))))),((_tmpD48->tl=decls,_tmpD48))))));}{struct _tuple21 _tmp7D0=
Cyc_Toc_xlate_pat(nv,rgn,_tmp7CD,Cyc_Absyn_var_exp(_tmp7CC,0),Cyc_Absyn_aggrmember_exp(
path,Cyc_Absyn_fieldname(cnt),0),_tmp7CB,fail_stmt,decls);nv=_tmp7D0.f1;decls=
_tmp7D0.f2;{struct Cyc_Absyn_Stmt*_tmp7D1=_tmp7D0.f3;struct Cyc_Absyn_Stmt*_tmp7D2=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7CC,0),Cyc_Absyn_aggrmember_exp(r,
Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7D2,
_tmp7D1,0),0);};};};}goto _LL334;};_LL34F: {struct Cyc_Absyn_Aggr_p_struct*_tmp7A8=(
struct Cyc_Absyn_Aggr_p_struct*)_tmp780;if(_tmp7A8->tag != 6)goto _LL351;else{
_tmp7A9=_tmp7A8->f1;if(_tmp7A9 != 0)goto _LL351;}}_LL350: {const char*_tmpD4C;void*
_tmpD4B;(_tmpD4B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD4C="unresolved aggregate pattern!",_tag_dyneither(_tmpD4C,sizeof(char),30))),
_tag_dyneither(_tmpD4B,sizeof(void*),0)));}_LL351: {struct Cyc_Absyn_Aggr_p_struct*
_tmp7AA=(struct Cyc_Absyn_Aggr_p_struct*)_tmp780;if(_tmp7AA->tag != 6)goto _LL353;
else{_tmp7AB=_tmp7AA->f1;if(_tmp7AB == 0)goto _LL353;_tmp7AC=*_tmp7AB;_tmp7AD=
_tmp7AC.aggr_info;_tmp7AE=_tmp7AA->f3;}}_LL352: {struct Cyc_Absyn_Aggrdecl*
_tmp7D5=Cyc_Absyn_get_known_aggrdecl(_tmp7AD);if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp7D5->impl))->tagged){struct _tuple23 _tmp7D7;struct Cyc_List_List*
_tmp7D8;struct Cyc_Absyn_Pat*_tmp7D9;struct _tuple23*_tmp7D6=(struct _tuple23*)((
struct Cyc_List_List*)_check_null(_tmp7AE))->hd;_tmp7D7=*_tmp7D6;_tmp7D8=_tmp7D7.f1;
_tmp7D9=_tmp7D7.f2;{struct _dyneither_ptr*f;{void*_tmp7DA=(void*)((struct Cyc_List_List*)
_check_null(_tmp7D8))->hd;struct _dyneither_ptr*_tmp7DC;_LL35C: {struct Cyc_Absyn_FieldName_struct*
_tmp7DB=(struct Cyc_Absyn_FieldName_struct*)_tmp7DA;if(_tmp7DB->tag != 1)goto
_LL35E;else{_tmp7DC=_tmp7DB->f1;}}_LL35D: f=_tmp7DC;goto _LL35B;_LL35E:;_LL35F: {
const char*_tmpD4F;void*_tmpD4E;(_tmpD4E=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD4F="no field name in tagged union pattern",
_tag_dyneither(_tmpD4F,sizeof(char),38))),_tag_dyneither(_tmpD4E,sizeof(void*),0)));}
_LL35B:;}{struct _tuple0*_tmp7DF=Cyc_Toc_temp_var();void*_tmp7E0=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp7D9->topt))->v;void*_tmp7E1=Cyc_Toc_typ_to_c(
_tmp7E0);{struct _tuple22*_tmpD52;struct Cyc_List_List*_tmpD51;decls=((_tmpD51=
_region_malloc(rgn,sizeof(*_tmpD51)),((_tmpD51->hd=((_tmpD52=_region_malloc(rgn,
sizeof(*_tmpD52)),((_tmpD52->f1=_tmp7DF,((_tmpD52->f2=_tmp7E1,_tmpD52)))))),((
_tmpD51->tl=decls,_tmpD51))))));}{struct Cyc_Absyn_Exp*_tmp7E4=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp7E5=
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp7E5=Cyc_Toc_cast_it(_tmp7E1,_tmp7E5);_tmp7E4=Cyc_Toc_cast_it(_tmp7E1,_tmp7E4);{
struct _tuple21 _tmp7E6=Cyc_Toc_xlate_pat(nv,rgn,_tmp7E0,Cyc_Absyn_var_exp(_tmp7DF,
0),_tmp7E4,_tmp7D9,fail_stmt,decls);nv=_tmp7E6.f1;decls=_tmp7E6.f2;{struct Cyc_Absyn_Stmt*
_tmp7E7=_tmp7E6.f3;struct Cyc_Absyn_Stmt*_tmp7E8=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp7D5,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp7E9=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp7DF,0),_tmp7E5,0);s=Cyc_Absyn_seq_stmt(_tmp7E8,Cyc_Absyn_seq_stmt(_tmp7E9,
_tmp7E7,0),0);};};};};};}else{s=Cyc_Toc_skip_stmt_dl();for(0;_tmp7AE != 0;_tmp7AE=
_tmp7AE->tl){struct _tuple23*_tmp7EA=(struct _tuple23*)_tmp7AE->hd;struct Cyc_Absyn_Pat*
_tmp7EB=(*_tmp7EA).f2;if(_tmp7EB->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _dyneither_ptr*f;{void*_tmp7EC=(void*)((struct Cyc_List_List*)_check_null((*
_tmp7EA).f1))->hd;struct _dyneither_ptr*_tmp7EE;_LL361: {struct Cyc_Absyn_FieldName_struct*
_tmp7ED=(struct Cyc_Absyn_FieldName_struct*)_tmp7EC;if(_tmp7ED->tag != 1)goto
_LL363;else{_tmp7EE=_tmp7ED->f1;}}_LL362: f=_tmp7EE;goto _LL360;_LL363:;_LL364: {
struct Cyc_Toc_Match_error_struct _tmpD55;struct Cyc_Toc_Match_error_struct*_tmpD54;(
int)_throw((void*)((_tmpD54=_cycalloc_atomic(sizeof(*_tmpD54)),((_tmpD54[0]=((
_tmpD55.tag=Cyc_Toc_Match_error,_tmpD55)),_tmpD54)))));}_LL360:;}{struct _tuple0*
_tmp7F1=Cyc_Toc_temp_var();void*_tmp7F2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7EB->topt))->v;void*_tmp7F3=Cyc_Toc_typ_to_c(_tmp7F2);{struct _tuple22*
_tmpD58;struct Cyc_List_List*_tmpD57;decls=((_tmpD57=_region_malloc(rgn,sizeof(*
_tmpD57)),((_tmpD57->hd=((_tmpD58=_region_malloc(rgn,sizeof(*_tmpD58)),((_tmpD58->f1=
_tmp7F1,((_tmpD58->f2=_tmp7F3,_tmpD58)))))),((_tmpD57->tl=decls,_tmpD57))))));}{
struct _tuple21 _tmp7F6=Cyc_Toc_xlate_pat(nv,rgn,_tmp7F2,Cyc_Absyn_var_exp(_tmp7F1,
0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp7EB,fail_stmt,decls);nv=_tmp7F6.f1;
decls=_tmp7F6.f2;{struct Cyc_Absyn_Exp*_tmp7F7=Cyc_Absyn_aggrmember_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7D5->impl))->fields,f)))->type))
_tmp7F7=Cyc_Toc_cast_it(_tmp7F3,_tmp7F7);{struct Cyc_Absyn_Stmt*_tmp7F8=_tmp7F6.f3;
struct Cyc_Absyn_Stmt*_tmp7F9=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7F1,0),
_tmp7F7,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7F9,_tmp7F8,0),0);};};};};};}}
goto _LL334;}_LL353: {struct Cyc_Absyn_Pointer_p_struct*_tmp7AF=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp780;if(_tmp7AF->tag != 5)goto _LL355;else{_tmp7B0=_tmp7AF->f1;}}_LL354: {
struct _tuple0*_tmp7FA=Cyc_Toc_temp_var();void*_tmp7FB=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp7B0->topt))->v;{struct _tuple22*_tmpD5B;struct Cyc_List_List*
_tmpD5A;decls=((_tmpD5A=_region_malloc(rgn,sizeof(*_tmpD5A)),((_tmpD5A->hd=((
_tmpD5B=_region_malloc(rgn,sizeof(*_tmpD5B)),((_tmpD5B->f1=_tmp7FA,((_tmpD5B->f2=
Cyc_Toc_typ_to_c(_tmp7FB),_tmpD5B)))))),((_tmpD5A->tl=decls,_tmpD5A))))));}{
struct _tuple21 _tmp7FE=Cyc_Toc_xlate_pat(nv,rgn,_tmp7FB,Cyc_Absyn_var_exp(_tmp7FA,
0),Cyc_Absyn_deref_exp(path,0),_tmp7B0,fail_stmt,decls);nv=_tmp7FE.f1;decls=
_tmp7FE.f2;{struct Cyc_Absyn_Stmt*_tmp7FF=_tmp7FE.f3;struct Cyc_Absyn_Stmt*_tmp800=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7FA,0),Cyc_Absyn_deref_exp(
r,0),0),_tmp7FF,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp800,0);else{s=_tmp800;}goto _LL334;};};}_LL355: {struct Cyc_Absyn_UnknownId_p_struct*
_tmp7B1=(struct Cyc_Absyn_UnknownId_p_struct*)_tmp780;if(_tmp7B1->tag != 14)goto
_LL357;}_LL356: {const char*_tmpD5E;void*_tmpD5D;(_tmpD5D=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD5E="unknownid pat",
_tag_dyneither(_tmpD5E,sizeof(char),14))),_tag_dyneither(_tmpD5D,sizeof(void*),0)));}
_LL357: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp7B2=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp780;if(_tmp7B2->tag != 15)goto _LL359;}_LL358: {const char*_tmpD61;void*_tmpD60;(
_tmpD60=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD61="unknowncall pat",_tag_dyneither(_tmpD61,sizeof(char),16))),
_tag_dyneither(_tmpD60,sizeof(void*),0)));}_LL359: {struct Cyc_Absyn_Exp_p_struct*
_tmp7B3=(struct Cyc_Absyn_Exp_p_struct*)_tmp780;if(_tmp7B3->tag != 16)goto _LL334;}
_LL35A: {const char*_tmpD64;void*_tmpD63;(_tmpD63=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD64="exp pat",
_tag_dyneither(_tmpD64,sizeof(char),8))),_tag_dyneither(_tmpD63,sizeof(void*),0)));}
_LL334:;}{struct _tuple21 _tmpD65;return(_tmpD65.f1=nv,((_tmpD65.f2=decls,((
_tmpD65.f3=s,_tmpD65)))));};}struct _tuple24{struct _dyneither_ptr*f1;struct
_dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){struct _tuple24*_tmpD66;
return(_tmpD66=_region_malloc(r,sizeof(*_tmpD66)),((_tmpD66->f1=Cyc_Toc_fresh_label(),((
_tmpD66->f2=Cyc_Toc_fresh_label(),((_tmpD66->f3=sc,_tmpD66)))))));}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*_tmp809=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;int leave_as_switch;{void*_tmp80A=Cyc_Tcutil_compress(
_tmp809);_LL366: {struct Cyc_Absyn_IntType_struct*_tmp80B=(struct Cyc_Absyn_IntType_struct*)
_tmp80A;if(_tmp80B->tag != 6)goto _LL368;}_LL367: goto _LL369;_LL368: {struct Cyc_Absyn_EnumType_struct*
_tmp80C=(struct Cyc_Absyn_EnumType_struct*)_tmp80A;if(_tmp80C->tag != 14)goto
_LL36A;}_LL369: leave_as_switch=1;goto _LL365;_LL36A:;_LL36B: leave_as_switch=0;
goto _LL365;_LL365:;}{struct Cyc_List_List*_tmp80D=scs;for(0;_tmp80D != 0;_tmp80D=
_tmp80D->tl){if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Switch_clause*)_tmp80D->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)
_tmp80D->hd)->where_clause != 0){leave_as_switch=0;break;}}}if(leave_as_switch){
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{struct Cyc_List_List*_tmp80E=
scs;for(0;_tmp80E != 0;_tmp80E=_tmp80E->tl){struct Cyc_Absyn_Stmt*_tmp80F=((struct
Cyc_Absyn_Switch_clause*)_tmp80E->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp80E->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp80F,0);next_l=Cyc_Toc_fresh_label();{
struct Cyc_Toc_Env _tmp811;struct _RegionHandle*_tmp812;struct Cyc_Toc_Env*_tmp810=
nv;_tmp811=*_tmp810;_tmp812=_tmp811.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
_tmp812,nv,next_l),_tmp80F);};}}return;}{struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp814;
struct _RegionHandle*_tmp815;struct Cyc_Toc_Env*_tmp813=nv;_tmp814=*_tmp813;
_tmp815=_tmp814.rgn;{struct Cyc_Toc_Env*_tmp816=Cyc_Toc_share_env(_tmp815,nv);
struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple24*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp815,Cyc_Toc_gen_label,
_tmp815,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct
Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp817=lscs;for(0;_tmp817 != 0;
_tmp817=_tmp817->tl){struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple24*)
_tmp817->hd)).f3;struct _dyneither_ptr*fail_lab=_tmp817->tl == 0?end_l:(*((struct
_tuple24*)((struct Cyc_List_List*)_check_null(_tmp817->tl))->hd)).f1;struct Cyc_Toc_Env*
_tmp819;struct Cyc_List_List*_tmp81A;struct Cyc_Absyn_Stmt*_tmp81B;struct _tuple21
_tmp818=Cyc_Toc_xlate_pat(_tmp816,_tmp815,_tmp809,r,path,sc->pattern,Cyc_Absyn_goto_stmt(
fail_lab,0),decls);_tmp819=_tmp818.f1;_tmp81A=_tmp818.f2;_tmp81B=_tmp818.f3;if(
sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp81C=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp819,_tmp81C);_tmp81B=Cyc_Absyn_seq_stmt(
_tmp81B,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp81C,0),
Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp81A;{
struct Cyc_List_List*_tmpD67;nvs=((_tmpD67=_region_malloc(_tmp815,sizeof(*_tmpD67)),((
_tmpD67->hd=_tmp819,((_tmpD67->tl=nvs,_tmpD67))))));}{struct Cyc_List_List*
_tmpD68;test_stmts=((_tmpD68=_region_malloc(_tmp815,sizeof(*_tmpD68)),((_tmpD68->hd=
_tmp81B,((_tmpD68->tl=test_stmts,_tmpD68))))));};}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple24 _tmp820;struct _dyneither_ptr*_tmp821;struct _dyneither_ptr*_tmp822;
struct Cyc_Absyn_Switch_clause*_tmp823;struct _tuple24*_tmp81F=(struct _tuple24*)
lscs->hd;_tmp820=*_tmp81F;_tmp821=_tmp820.f1;_tmp822=_tmp820.f2;_tmp823=_tmp820.f3;{
struct Cyc_Toc_Env*_tmp824=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp823->body;struct Cyc_Toc_Env _tmp826;struct
_RegionHandle*_tmp827;struct Cyc_Toc_Env*_tmp825=_tmp816;_tmp826=*_tmp825;_tmp827=
_tmp826.rgn;if(lscs->tl != 0){struct _tuple24 _tmp829;struct _dyneither_ptr*_tmp82A;
struct Cyc_Absyn_Switch_clause*_tmp82B;struct _tuple24*_tmp828=(struct _tuple24*)((
struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp829=*_tmp828;_tmp82A=_tmp829.f2;
_tmp82B=_tmp829.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp827,
_tmp824,end_l,_tmp82A,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp82B->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp827,
_tmp824,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp821,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp822,s,0),0);{struct Cyc_List_List*_tmpD69;stmts=((_tmpD69=_region_malloc(
_tmp815,sizeof(*_tmpD69)),((_tmpD69->hd=s,((_tmpD69->tl=stmts,_tmpD69))))));};};}{
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,
Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple22
_tmp82E;struct _tuple0*_tmp82F;void*_tmp830;struct _tuple22*_tmp82D=(struct
_tuple22*)decls->hd;_tmp82E=*_tmp82D;_tmp82F=_tmp82E.f1;_tmp830=_tmp82E.f2;res=
Cyc_Absyn_declare_stmt(_tmp82F,_tmp830,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(
v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(
struct Cyc_Absyn_Exp*)e,res,0))->r;};};};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){struct Cyc_List_List*
_tmpD6A;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,((
_tmpD6A=_cycalloc(sizeof(*_tmpD6A)),((_tmpD6A->hd=Cyc_Absyn_uint_exp((
unsigned int)(n - 1),0),((_tmpD6A->tl=0,_tmpD6A)))))),0),0);}void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s){while(1){void*_tmp832=s->r;struct Cyc_Absyn_Exp*_tmp835;
struct Cyc_Absyn_Stmt*_tmp837;struct Cyc_Absyn_Stmt*_tmp838;struct Cyc_Absyn_Exp*
_tmp83A;struct Cyc_Absyn_Exp*_tmp83C;struct Cyc_Absyn_Stmt*_tmp83D;struct Cyc_Absyn_Stmt*
_tmp83E;struct _tuple8 _tmp840;struct Cyc_Absyn_Exp*_tmp841;struct Cyc_Absyn_Stmt*
_tmp842;struct Cyc_Absyn_Stmt*_tmp844;struct Cyc_Absyn_Stmt*_tmp846;struct Cyc_Absyn_Stmt*
_tmp848;struct Cyc_Absyn_Exp*_tmp84A;struct _tuple8 _tmp84B;struct Cyc_Absyn_Exp*
_tmp84C;struct _tuple8 _tmp84D;struct Cyc_Absyn_Exp*_tmp84E;struct Cyc_Absyn_Stmt*
_tmp84F;struct Cyc_Absyn_Exp*_tmp851;struct Cyc_List_List*_tmp852;struct Cyc_List_List*
_tmp854;struct Cyc_Absyn_Switch_clause**_tmp855;struct Cyc_Absyn_Decl*_tmp857;
struct Cyc_Absyn_Stmt*_tmp858;struct _dyneither_ptr*_tmp85A;struct Cyc_Absyn_Stmt*
_tmp85B;struct Cyc_Absyn_Stmt*_tmp85D;struct _tuple8 _tmp85E;struct Cyc_Absyn_Exp*
_tmp85F;struct Cyc_Absyn_Stmt*_tmp861;struct Cyc_List_List*_tmp862;struct Cyc_Absyn_Exp*
_tmp864;_LL36D: {struct Cyc_Absyn_Skip_s_struct*_tmp833=(struct Cyc_Absyn_Skip_s_struct*)
_tmp832;if(_tmp833->tag != 0)goto _LL36F;}_LL36E: return;_LL36F: {struct Cyc_Absyn_Exp_s_struct*
_tmp834=(struct Cyc_Absyn_Exp_s_struct*)_tmp832;if(_tmp834->tag != 1)goto _LL371;
else{_tmp835=_tmp834->f1;}}_LL370: Cyc_Toc_exp_to_c(nv,_tmp835);return;_LL371: {
struct Cyc_Absyn_Seq_s_struct*_tmp836=(struct Cyc_Absyn_Seq_s_struct*)_tmp832;if(
_tmp836->tag != 2)goto _LL373;else{_tmp837=_tmp836->f1;_tmp838=_tmp836->f2;}}
_LL372: Cyc_Toc_stmt_to_c(nv,_tmp837);s=_tmp838;continue;_LL373: {struct Cyc_Absyn_Return_s_struct*
_tmp839=(struct Cyc_Absyn_Return_s_struct*)_tmp832;if(_tmp839->tag != 3)goto _LL375;
else{_tmp83A=_tmp839->f1;}}_LL374: {struct Cyc_Core_Opt*topt=0;if(_tmp83A != 0){{
struct Cyc_Core_Opt*_tmpD6B;topt=((_tmpD6B=_cycalloc(sizeof(*_tmpD6B)),((_tmpD6B->v=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp83A->topt))->v),
_tmpD6B))));}Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp83A);}if(s->try_depth
> 0){if(topt != 0){struct _tuple0*_tmp866=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*
_tmp867=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp866,0),
0);s->r=(Cyc_Absyn_declare_stmt(_tmp866,(void*)topt->v,_tmp83A,Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler(s->try_depth),_tmp867,0),0))->r;}else{Cyc_Toc_do_npop_before(
s->try_depth,s);}}return;}_LL375: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp83B=(
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp832;if(_tmp83B->tag != 4)goto _LL377;
else{_tmp83C=_tmp83B->f1;_tmp83D=_tmp83B->f2;_tmp83E=_tmp83B->f3;}}_LL376: Cyc_Toc_exp_to_c(
nv,_tmp83C);Cyc_Toc_stmt_to_c(nv,_tmp83D);s=_tmp83E;continue;_LL377: {struct Cyc_Absyn_While_s_struct*
_tmp83F=(struct Cyc_Absyn_While_s_struct*)_tmp832;if(_tmp83F->tag != 5)goto _LL379;
else{_tmp840=_tmp83F->f1;_tmp841=_tmp840.f1;_tmp842=_tmp83F->f2;}}_LL378: Cyc_Toc_exp_to_c(
nv,_tmp841);{struct Cyc_Toc_Env _tmp869;struct _RegionHandle*_tmp86A;struct Cyc_Toc_Env*
_tmp868=nv;_tmp869=*_tmp868;_tmp86A=_tmp869.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp86A,nv),_tmp842);return;};_LL379: {struct Cyc_Absyn_Break_s_struct*_tmp843=(
struct Cyc_Absyn_Break_s_struct*)_tmp832;if(_tmp843->tag != 6)goto _LL37B;else{
_tmp844=_tmp843->f1;}}_LL37A: {struct Cyc_Toc_Env _tmp86C;struct _dyneither_ptr**
_tmp86D;struct Cyc_Toc_Env*_tmp86B=nv;_tmp86C=*_tmp86B;_tmp86D=_tmp86C.break_lab;
if(_tmp86D != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp86D,0);{int dest_depth=_tmp844 == 0?0:
_tmp844->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;};}
_LL37B: {struct Cyc_Absyn_Continue_s_struct*_tmp845=(struct Cyc_Absyn_Continue_s_struct*)
_tmp832;if(_tmp845->tag != 7)goto _LL37D;else{_tmp846=_tmp845->f1;}}_LL37C: {
struct Cyc_Toc_Env _tmp86F;struct _dyneither_ptr**_tmp870;struct Cyc_Toc_Env*_tmp86E=
nv;_tmp86F=*_tmp86E;_tmp870=_tmp86F.continue_lab;if(_tmp870 != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp870,0);_tmp848=_tmp846;goto _LL37E;}_LL37D: {struct Cyc_Absyn_Goto_s_struct*
_tmp847=(struct Cyc_Absyn_Goto_s_struct*)_tmp832;if(_tmp847->tag != 8)goto _LL37F;
else{_tmp848=_tmp847->f2;}}_LL37E: Cyc_Toc_do_npop_before(s->try_depth - ((struct
Cyc_Absyn_Stmt*)_check_null(_tmp848))->try_depth,s);return;_LL37F: {struct Cyc_Absyn_For_s_struct*
_tmp849=(struct Cyc_Absyn_For_s_struct*)_tmp832;if(_tmp849->tag != 9)goto _LL381;
else{_tmp84A=_tmp849->f1;_tmp84B=_tmp849->f2;_tmp84C=_tmp84B.f1;_tmp84D=_tmp849->f3;
_tmp84E=_tmp84D.f1;_tmp84F=_tmp849->f4;}}_LL380: Cyc_Toc_exp_to_c(nv,_tmp84A);Cyc_Toc_exp_to_c(
nv,_tmp84C);Cyc_Toc_exp_to_c(nv,_tmp84E);{struct Cyc_Toc_Env _tmp872;struct
_RegionHandle*_tmp873;struct Cyc_Toc_Env*_tmp871=nv;_tmp872=*_tmp871;_tmp873=
_tmp872.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp873,nv),_tmp84F);return;};
_LL381: {struct Cyc_Absyn_Switch_s_struct*_tmp850=(struct Cyc_Absyn_Switch_s_struct*)
_tmp832;if(_tmp850->tag != 10)goto _LL383;else{_tmp851=_tmp850->f1;_tmp852=_tmp850->f2;}}
_LL382: Cyc_Toc_xlate_switch(nv,s,_tmp851,_tmp852);return;_LL383: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp853=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp832;if(_tmp853->tag != 11)goto
_LL385;else{_tmp854=_tmp853->f1;_tmp855=_tmp853->f2;}}_LL384: {struct Cyc_Toc_Env
_tmp875;struct Cyc_Toc_FallthruInfo*_tmp876;struct Cyc_Toc_Env*_tmp874=nv;_tmp875=*
_tmp874;_tmp876=_tmp875.fallthru_info;if(_tmp876 == 0){const char*_tmpD6E;void*
_tmpD6D;(_tmpD6D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD6E="fallthru in unexpected place",_tag_dyneither(_tmpD6E,sizeof(char),29))),
_tag_dyneither(_tmpD6D,sizeof(void*),0)));}{struct _dyneither_ptr*_tmp87A;struct
Cyc_List_List*_tmp87B;struct Cyc_Dict_Dict _tmp87C;struct Cyc_Toc_FallthruInfo
_tmp879=*_tmp876;_tmp87A=_tmp879.label;_tmp87B=_tmp879.binders;_tmp87C=_tmp879.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp87A,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp855)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp87D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp87B);struct Cyc_List_List*_tmp87E=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp854);for(0;_tmp87D != 0;(_tmp87D=_tmp87D->tl,
_tmp87E=_tmp87E->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp87E))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp87C,(
struct _tuple0*)_tmp87D->hd),(struct Cyc_Absyn_Exp*)_tmp87E->hd,0),s2,0);}s->r=s2->r;
return;};};};}_LL385: {struct Cyc_Absyn_Decl_s_struct*_tmp856=(struct Cyc_Absyn_Decl_s_struct*)
_tmp832;if(_tmp856->tag != 12)goto _LL387;else{_tmp857=_tmp856->f1;_tmp858=_tmp856->f2;}}
_LL386:{void*_tmp87F=_tmp857->r;struct Cyc_Absyn_Vardecl*_tmp881;struct Cyc_Absyn_Pat*
_tmp883;struct Cyc_Absyn_Exp*_tmp884;struct Cyc_List_List*_tmp886;struct Cyc_Absyn_Fndecl*
_tmp888;struct Cyc_Absyn_Tvar*_tmp88A;struct Cyc_Absyn_Vardecl*_tmp88B;int _tmp88C;
struct Cyc_Absyn_Exp*_tmp88D;struct Cyc_Absyn_Exp*_tmp88F;struct Cyc_Absyn_Tvar*
_tmp890;struct Cyc_Absyn_Vardecl*_tmp891;_LL390: {struct Cyc_Absyn_Var_d_struct*
_tmp880=(struct Cyc_Absyn_Var_d_struct*)_tmp87F;if(_tmp880->tag != 0)goto _LL392;
else{_tmp881=_tmp880->f1;}}_LL391: {struct Cyc_Toc_Env _tmp893;struct _RegionHandle*
_tmp894;struct Cyc_Toc_Env*_tmp892=nv;_tmp893=*_tmp892;_tmp894=_tmp893.rgn;{
struct Cyc_Absyn_Local_b_struct _tmpD71;struct Cyc_Absyn_Local_b_struct*_tmpD70;
struct Cyc_Toc_Env*_tmp895=Cyc_Toc_add_varmap(_tmp894,nv,_tmp881->name,Cyc_Absyn_varb_exp(
_tmp881->name,(void*)((_tmpD70=_cycalloc(sizeof(*_tmpD70)),((_tmpD70[0]=((
_tmpD71.tag=4,((_tmpD71.f1=_tmp881,_tmpD71)))),_tmpD70)))),0));Cyc_Toc_local_decl_to_c(
_tmp895,_tmp895,_tmp881,_tmp858);}goto _LL38F;}_LL392: {struct Cyc_Absyn_Let_d_struct*
_tmp882=(struct Cyc_Absyn_Let_d_struct*)_tmp87F;if(_tmp882->tag != 2)goto _LL394;
else{_tmp883=_tmp882->f1;_tmp884=_tmp882->f3;}}_LL393:{void*_tmp898=_tmp883->r;
struct Cyc_Absyn_Vardecl*_tmp89A;struct Cyc_Absyn_Pat*_tmp89B;struct Cyc_Absyn_Pat
_tmp89C;void*_tmp89D;_LL39F: {struct Cyc_Absyn_Var_p_struct*_tmp899=(struct Cyc_Absyn_Var_p_struct*)
_tmp898;if(_tmp899->tag != 1)goto _LL3A1;else{_tmp89A=_tmp899->f1;_tmp89B=_tmp899->f2;
_tmp89C=*_tmp89B;_tmp89D=_tmp89C.r;{struct Cyc_Absyn_Wild_p_struct*_tmp89E=(
struct Cyc_Absyn_Wild_p_struct*)_tmp89D;if(_tmp89E->tag != 0)goto _LL3A1;};}}_LL3A0: {
struct _tuple0*old_name=_tmp89A->name;struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp89A->name=new_name;_tmp89A->initializer=(struct Cyc_Absyn_Exp*)_tmp884;{
struct Cyc_Absyn_Var_d_struct _tmpD74;struct Cyc_Absyn_Var_d_struct*_tmpD73;_tmp857->r=(
void*)((_tmpD73=_cycalloc(sizeof(*_tmpD73)),((_tmpD73[0]=((_tmpD74.tag=0,((
_tmpD74.f1=_tmp89A,_tmpD74)))),_tmpD73))));}{struct Cyc_Toc_Env _tmp8A2;struct
_RegionHandle*_tmp8A3;struct Cyc_Toc_Env*_tmp8A1=nv;_tmp8A2=*_tmp8A1;_tmp8A3=
_tmp8A2.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD77;struct Cyc_Absyn_Local_b_struct*
_tmpD76;struct Cyc_Toc_Env*_tmp8A4=Cyc_Toc_add_varmap(_tmp8A3,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD76=_cycalloc(sizeof(*_tmpD76)),((_tmpD76[0]=((_tmpD77.tag=
4,((_tmpD77.f1=_tmp89A,_tmpD77)))),_tmpD76)))),0));Cyc_Toc_local_decl_to_c(
_tmp8A4,nv,_tmp89A,_tmp858);}goto _LL39E;};}_LL3A1:;_LL3A2: s->r=(Cyc_Toc_letdecl_to_c(
nv,_tmp883,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp884->topt))->v,_tmp884,
_tmp858))->r;goto _LL39E;_LL39E:;}goto _LL38F;_LL394: {struct Cyc_Absyn_Letv_d_struct*
_tmp885=(struct Cyc_Absyn_Letv_d_struct*)_tmp87F;if(_tmp885->tag != 3)goto _LL396;
else{_tmp886=_tmp885->f1;}}_LL395: {struct Cyc_List_List*_tmp8A7=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp886);if(_tmp8A7 == 0){const char*_tmpD7A;
void*_tmpD79;(_tmpD79=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpD7A="empty Letv_d",_tag_dyneither(_tmpD7A,sizeof(char),
13))),_tag_dyneither(_tmpD79,sizeof(void*),0)));}{struct Cyc_Absyn_Var_d_struct
_tmpD7D;struct Cyc_Absyn_Var_d_struct*_tmpD7C;_tmp857->r=(void*)((_tmpD7C=
_cycalloc(sizeof(*_tmpD7C)),((_tmpD7C[0]=((_tmpD7D.tag=0,((_tmpD7D.f1=(struct Cyc_Absyn_Vardecl*)
_tmp8A7->hd,_tmpD7D)))),_tmpD7C))));}_tmp8A7=_tmp8A7->tl;for(0;_tmp8A7 != 0;
_tmp8A7=_tmp8A7->tl){struct Cyc_Absyn_Var_d_struct _tmpD80;struct Cyc_Absyn_Var_d_struct*
_tmpD7F;struct Cyc_Absyn_Decl*_tmp8AC=Cyc_Absyn_new_decl((void*)((_tmpD7F=
_cycalloc(sizeof(*_tmpD7F)),((_tmpD7F[0]=((_tmpD80.tag=0,((_tmpD80.f1=(struct Cyc_Absyn_Vardecl*)
_tmp8A7->hd,_tmpD80)))),_tmpD7F)))),0);s->r=(Cyc_Absyn_decl_stmt(_tmp8AC,Cyc_Absyn_new_stmt(
s->r,0),0))->r;}Cyc_Toc_stmt_to_c(nv,s);goto _LL38F;}_LL396: {struct Cyc_Absyn_Fn_d_struct*
_tmp887=(struct Cyc_Absyn_Fn_d_struct*)_tmp87F;if(_tmp887->tag != 1)goto _LL398;
else{_tmp888=_tmp887->f1;}}_LL397: {struct _tuple0*_tmp8AF=_tmp888->name;struct
Cyc_Toc_Env _tmp8B1;struct _RegionHandle*_tmp8B2;struct Cyc_Toc_Env*_tmp8B0=nv;
_tmp8B1=*_tmp8B0;_tmp8B2=_tmp8B1.rgn;{struct Cyc_Toc_Env*_tmp8B3=Cyc_Toc_add_varmap(
_tmp8B2,nv,_tmp888->name,Cyc_Absyn_var_exp(_tmp8AF,0));Cyc_Toc_fndecl_to_c(
_tmp8B3,_tmp888,0);Cyc_Toc_stmt_to_c(_tmp8B3,_tmp858);}goto _LL38F;}_LL398: {
struct Cyc_Absyn_Region_d_struct*_tmp889=(struct Cyc_Absyn_Region_d_struct*)
_tmp87F;if(_tmp889->tag != 4)goto _LL39A;else{_tmp88A=_tmp889->f1;_tmp88B=_tmp889->f2;
_tmp88C=_tmp889->f3;_tmp88D=_tmp889->f4;}}_LL399: {struct Cyc_Absyn_Stmt*_tmp8B4=
_tmp858;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple0*
rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp88B->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp8B6;struct _RegionHandle*_tmp8B7;struct Cyc_Toc_Env*
_tmp8B5=nv;_tmp8B6=*_tmp8B5;_tmp8B7=_tmp8B6.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp8B7,nv,x_var,x_exp),_tmp8B4);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp8B4,0))->r;
else{if(_tmp88D == 0){struct Cyc_Absyn_Exp*_tmpD83[1];struct Cyc_Absyn_Exp*_tmpD82[
1];struct Cyc_List_List*_tmpD81;s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpD81=
_cycalloc(sizeof(*_tmpD81)),((_tmpD81->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0),((_tmpD81->tl=0,_tmpD81)))))),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD82[0]=
x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD82,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp8B4,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD83[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD83,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp88D);{struct Cyc_Absyn_Exp*
_tmpD85[1];struct Cyc_Absyn_Exp*_tmpD84[2];s->r=(Cyc_Absyn_declare_stmt(rh_var,
Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpD84[1]=(struct Cyc_Absyn_Exp*)_tmp88D,((_tmpD84[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD84,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp8B4,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpD85[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD85,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
return;}_LL39A: {struct Cyc_Absyn_Alias_d_struct*_tmp88E=(struct Cyc_Absyn_Alias_d_struct*)
_tmp87F;if(_tmp88E->tag != 5)goto _LL39C;else{_tmp88F=_tmp88E->f1;_tmp890=_tmp88E->f2;
_tmp891=_tmp88E->f3;}}_LL39B: {struct _tuple0*old_name=_tmp891->name;struct
_tuple0*new_name=Cyc_Toc_temp_var();_tmp891->name=new_name;_tmp891->initializer=(
struct Cyc_Absyn_Exp*)_tmp88F;{struct Cyc_Absyn_Decl_s_struct _tmpD94;struct Cyc_Absyn_Var_d_struct*
_tmpD93;struct Cyc_Absyn_Var_d_struct _tmpD92;struct Cyc_Absyn_Decl*_tmpD91;struct
Cyc_Absyn_Decl_s_struct*_tmpD90;s->r=(void*)((_tmpD90=_cycalloc(sizeof(*_tmpD90)),((
_tmpD90[0]=((_tmpD94.tag=12,((_tmpD94.f1=((_tmpD91=_cycalloc(sizeof(*_tmpD91)),((
_tmpD91->r=(void*)((_tmpD93=_cycalloc(sizeof(*_tmpD93)),((_tmpD93[0]=((_tmpD92.tag=
0,((_tmpD92.f1=_tmp891,_tmpD92)))),_tmpD93)))),((_tmpD91->loc=0,_tmpD91)))))),((
_tmpD94.f2=_tmp858,_tmpD94)))))),_tmpD90))));}{struct Cyc_Toc_Env _tmp8C3;struct
_RegionHandle*_tmp8C4;struct Cyc_Toc_Env*_tmp8C2=nv;_tmp8C3=*_tmp8C2;_tmp8C4=
_tmp8C3.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD97;struct Cyc_Absyn_Local_b_struct*
_tmpD96;struct Cyc_Toc_Env*_tmp8C5=Cyc_Toc_add_varmap(_tmp8C4,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD96=_cycalloc(sizeof(*_tmpD96)),((_tmpD96[0]=((_tmpD97.tag=
4,((_tmpD97.f1=_tmp891,_tmpD97)))),_tmpD96)))),0));Cyc_Toc_local_decl_to_c(
_tmp8C5,nv,_tmp891,_tmp858);}return;};}_LL39C:;_LL39D: {const char*_tmpD9A;void*
_tmpD99;(_tmpD99=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD9A="bad nested declaration within function",_tag_dyneither(_tmpD9A,sizeof(
char),39))),_tag_dyneither(_tmpD99,sizeof(void*),0)));}_LL38F:;}return;_LL387: {
struct Cyc_Absyn_Label_s_struct*_tmp859=(struct Cyc_Absyn_Label_s_struct*)_tmp832;
if(_tmp859->tag != 13)goto _LL389;else{_tmp85A=_tmp859->f1;_tmp85B=_tmp859->f2;}}
_LL388: s=_tmp85B;continue;_LL389: {struct Cyc_Absyn_Do_s_struct*_tmp85C=(struct
Cyc_Absyn_Do_s_struct*)_tmp832;if(_tmp85C->tag != 14)goto _LL38B;else{_tmp85D=
_tmp85C->f1;_tmp85E=_tmp85C->f2;_tmp85F=_tmp85E.f1;}}_LL38A: {struct Cyc_Toc_Env
_tmp8CB;struct _RegionHandle*_tmp8CC;struct Cyc_Toc_Env*_tmp8CA=nv;_tmp8CB=*
_tmp8CA;_tmp8CC=_tmp8CB.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp8CC,nv),
_tmp85D);Cyc_Toc_exp_to_c(nv,_tmp85F);return;}_LL38B: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp860=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp832;if(_tmp860->tag != 15)goto
_LL38D;else{_tmp861=_tmp860->f1;_tmp862=_tmp860->f2;}}_LL38C: {struct _tuple0*
h_var=Cyc_Toc_temp_var();struct _tuple0*e_var=Cyc_Toc_temp_var();struct _tuple0*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);{struct Cyc_Core_Opt*_tmpD9B;
e_exp->topt=((_tmpD9B=_cycalloc(sizeof(*_tmpD9B)),((_tmpD9B->v=e_typ,_tmpD9B))));}{
struct Cyc_Toc_Env _tmp8CF;struct _RegionHandle*_tmp8D0;struct Cyc_Toc_Env*_tmp8CE=
nv;_tmp8CF=*_tmp8CE;_tmp8D0=_tmp8CF.rgn;{struct Cyc_Toc_Env*_tmp8D1=Cyc_Toc_add_varmap(
_tmp8D0,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp8D1,_tmp861);{struct Cyc_Absyn_Stmt*
_tmp8D2=Cyc_Absyn_seq_stmt(_tmp861,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple0*_tmp8D3=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp8D4=
Cyc_Absyn_var_exp(_tmp8D3,0);struct Cyc_Absyn_Vardecl*_tmp8D5=Cyc_Absyn_new_vardecl(
_tmp8D3,Cyc_Absyn_exn_typ(),0);{struct Cyc_Core_Opt*_tmpD9C;_tmp8D4->topt=((
_tmpD9C=_cycalloc(sizeof(*_tmpD9C)),((_tmpD9C->v=Cyc_Absyn_exn_typ(),_tmpD9C))));}{
struct Cyc_Core_Opt*_tmpDAD;struct Cyc_Absyn_Var_p_struct*_tmpDAC;struct Cyc_Absyn_Pat*
_tmpDAB;struct Cyc_Core_Opt*_tmpDAA;struct Cyc_Absyn_Var_p_struct _tmpDA9;struct Cyc_Absyn_Pat*
_tmpDA8;struct Cyc_Absyn_Pat*_tmp8D7=(_tmpDA8=_cycalloc(sizeof(*_tmpDA8)),((
_tmpDA8->r=(void*)((_tmpDAC=_cycalloc(sizeof(*_tmpDAC)),((_tmpDAC[0]=((_tmpDA9.tag=
1,((_tmpDA9.f1=_tmp8D5,((_tmpDA9.f2=((_tmpDAB=_cycalloc(sizeof(*_tmpDAB)),((
_tmpDAB->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpDAB->topt=((_tmpDAA=_cycalloc(
sizeof(*_tmpDAA)),((_tmpDAA->v=Cyc_Absyn_exn_typ(),_tmpDAA)))),((_tmpDAB->loc=0,
_tmpDAB)))))))),_tmpDA9)))))),_tmpDAC)))),((_tmpDA8->topt=((_tmpDAD=_cycalloc(
sizeof(*_tmpDAD)),((_tmpDAD->v=Cyc_Absyn_exn_typ(),_tmpDAD)))),((_tmpDA8->loc=0,
_tmpDA8)))))));struct Cyc_Absyn_Exp*_tmp8D8=Cyc_Absyn_throw_exp(_tmp8D4,0);{
struct Cyc_Core_Opt*_tmpDAE;_tmp8D8->topt=((_tmpDAE=_cycalloc(sizeof(*_tmpDAE)),((
_tmpDAE->v=(void*)& Cyc_Absyn_VoidType_val,_tmpDAE))));}{struct Cyc_Absyn_Stmt*
_tmp8DA=Cyc_Absyn_exp_stmt(_tmp8D8,0);struct Cyc_Core_Opt*_tmpDB4;struct Cyc_List_List*
_tmpDB3;struct Cyc_Absyn_Switch_clause*_tmpDB2;struct Cyc_Absyn_Switch_clause*
_tmp8DB=(_tmpDB2=_cycalloc(sizeof(*_tmpDB2)),((_tmpDB2->pattern=_tmp8D7,((
_tmpDB2->pat_vars=((_tmpDB4=_cycalloc(sizeof(*_tmpDB4)),((_tmpDB4->v=((_tmpDB3=
_cycalloc(sizeof(*_tmpDB3)),((_tmpDB3->hd=_tmp8D5,((_tmpDB3->tl=0,_tmpDB3)))))),
_tmpDB4)))),((_tmpDB2->where_clause=0,((_tmpDB2->body=_tmp8DA,((_tmpDB2->loc=0,
_tmpDB2)))))))))));struct Cyc_List_List*_tmpDB5;struct Cyc_Absyn_Stmt*_tmp8DC=Cyc_Absyn_switch_stmt(
e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp862,((_tmpDB5=_cycalloc(sizeof(*_tmpDB5)),((_tmpDB5->hd=_tmp8DB,((_tmpDB5->tl=
0,_tmpDB5))))))),0);Cyc_Toc_stmt_to_c(_tmp8D1,_tmp8DC);{struct Cyc_List_List*
_tmpDB6;struct Cyc_Absyn_Exp*_tmp8DD=Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,((
_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6->hd=Cyc_Absyn_aggrmember_exp(h_exp,
Cyc_Toc_handler_sp,0),((_tmpDB6->tl=0,_tmpDB6)))))),0);struct Cyc_List_List*
_tmpDB7;struct Cyc_Absyn_Stmt*_tmp8DE=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpDB7=_cycalloc(sizeof(*_tmpDB7)),((_tmpDB7->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpDB7->tl=0,_tmpDB7)))))),0),0);struct Cyc_Absyn_Exp*_tmp8DF=Cyc_Absyn_int_exp(
Cyc_Absyn_Signed,0,0);struct Cyc_Absyn_Exp*_tmp8E0=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
1,0);s->r=(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp8DE,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp8DF,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp8DD,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp8E0,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp8D2,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp8DC,0),0),0),0),0),0))->r;};};};};}
return;};}_LL38D: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp863=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp832;if(_tmp863->tag != 16)goto _LL36C;else{_tmp864=_tmp863->f1;}}_LL38E: if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp864);{struct Cyc_List_List*
_tmpDB8;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((
_tmpDB8=_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8->hd=_tmp864,((_tmpDB8->tl=0,
_tmpDB8)))))),0));};}return;_LL36C:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);struct _tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8EE=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8EE;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp8EF=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp8F0=f->args;for(0;_tmp8F0 != 0;_tmp8F0=_tmp8F0->tl){struct
_tuple0*_tmpDB9;struct _tuple0*_tmp8F1=(_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((
_tmpDB9->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpDB9->f2=(*((struct
_tuple25*)_tmp8F0->hd)).f1,_tmpDB9)))));(*((struct _tuple25*)_tmp8F0->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple25*)_tmp8F0->hd)).f3);_tmp8EF=Cyc_Toc_add_varmap(frgn,_tmp8EF,
_tmp8F1,Cyc_Absyn_var_exp(_tmp8F1,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp8F4;struct Cyc_Absyn_Tqual
_tmp8F5;void*_tmp8F6;int _tmp8F7;struct Cyc_Absyn_VarargInfo _tmp8F3=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp8F4=_tmp8F3.name;_tmp8F5=_tmp8F3.tq;_tmp8F6=
_tmp8F3.type;_tmp8F7=_tmp8F3.inject;{void*_tmp8F8=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp8F6,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8F5,Cyc_Absyn_false_conref));struct
_tuple0*_tmpDBA;struct _tuple0*_tmp8F9=(_tmpDBA=_cycalloc(sizeof(*_tmpDBA)),((
_tmpDBA->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpDBA->f2=(struct
_dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null(_tmp8F4))->v,_tmpDBA)))));{
struct _tuple25*_tmpDBD;struct Cyc_List_List*_tmpDBC;f->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpDBC=
_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC->hd=((_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((
_tmpDBD->f1=(struct _dyneither_ptr*)_tmp8F4->v,((_tmpDBD->f2=_tmp8F5,((_tmpDBD->f3=
_tmp8F8,_tmpDBD)))))))),((_tmpDBC->tl=0,_tmpDBC)))))));}_tmp8EF=Cyc_Toc_add_varmap(
frgn,_tmp8EF,_tmp8F9,Cyc_Absyn_var_exp(_tmp8F9,0));f->cyc_varargs=0;};}{struct
Cyc_List_List*_tmp8FD=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp8FD != 0;_tmp8FD=_tmp8FD->tl){((struct Cyc_Absyn_Vardecl*)_tmp8FD->hd)->type=
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp8FD->hd)->type);}}Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel(frgn,_tmp8EF),f->body);};_pop_region(frgn);};}static enum 
Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){switch(s){case Cyc_Absyn_Abstract:
_LL3A3: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3A4: return Cyc_Absyn_Extern;
default: _LL3A5: return s;}}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad,int add_to_decls){struct _tuple0*_tmp8FE=ad->name;struct _DynRegionHandle*
_tmp900;struct Cyc_Dict_Dict*_tmp901;struct Cyc_Toc_TocState _tmp8FF=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp900=_tmp8FF.dyn;_tmp901=_tmp8FF.aggrs_so_far;{
struct _DynRegionFrame _tmp902;struct _RegionHandle*d=_open_dynregion(& _tmp902,
_tmp900);{int seen_defn_before;struct _tuple14**_tmp903=((struct _tuple14**(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp901,_tmp8FE);if(
_tmp903 == 0){seen_defn_before=0;{struct _tuple14*v;if(ad->kind == Cyc_Absyn_StructA){
struct _tuple14*_tmpDBE;v=((_tmpDBE=_region_malloc(d,sizeof(*_tmpDBE)),((_tmpDBE->f1=
ad,((_tmpDBE->f2=Cyc_Absyn_strctq(_tmp8FE),_tmpDBE))))));}else{struct _tuple14*
_tmpDBF;v=((_tmpDBF=_region_malloc(d,sizeof(*_tmpDBF)),((_tmpDBF->f1=ad,((
_tmpDBF->f2=Cyc_Absyn_unionq_typ(_tmp8FE),_tmpDBF))))));}*_tmp901=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp901,
_tmp8FE,v);};}else{struct _tuple14 _tmp907;struct Cyc_Absyn_Aggrdecl*_tmp908;void*
_tmp909;struct _tuple14*_tmp906=*_tmp903;_tmp907=*_tmp906;_tmp908=_tmp907.f1;
_tmp909=_tmp907.f2;if(_tmp908->impl == 0){{struct _tuple14*_tmpDC0;*_tmp901=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))
Cyc_Dict_insert)(*_tmp901,_tmp8FE,((_tmpDC0=_region_malloc(d,sizeof(*_tmpDC0)),((
_tmpDC0->f1=ad,((_tmpDC0->f2=_tmp909,_tmpDC0)))))));}seen_defn_before=0;}else{
seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*_tmpDC1;struct Cyc_Absyn_Aggrdecl*
new_ad=(_tmpDC1=_cycalloc(sizeof(*_tmpDC1)),((_tmpDC1->kind=ad->kind,((_tmpDC1->sc=
Cyc_Absyn_Public,((_tmpDC1->name=ad->name,((_tmpDC1->tvs=0,((_tmpDC1->impl=0,((
_tmpDC1->attributes=ad->attributes,_tmpDC1)))))))))))));if(ad->impl != 0  && !
seen_defn_before){{struct Cyc_Absyn_AggrdeclImpl*_tmpDC2;new_ad->impl=((_tmpDC2=
_cycalloc(sizeof(*_tmpDC2)),((_tmpDC2->exist_vars=0,((_tmpDC2->rgn_po=0,((
_tmpDC2->fields=0,((_tmpDC2->tagged=0,_tmpDC2))))))))));}{struct Cyc_List_List*
new_fields=0;{struct Cyc_List_List*_tmp90C=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp90C != 0;_tmp90C=_tmp90C->tl){struct Cyc_Absyn_Aggrfield*
_tmp90D=(struct Cyc_Absyn_Aggrfield*)_tmp90C->hd;struct Cyc_Absyn_Aggrfield*
_tmpDC3;struct Cyc_Absyn_Aggrfield*_tmp90E=(_tmpDC3=_cycalloc(sizeof(*_tmpDC3)),((
_tmpDC3->name=_tmp90D->name,((_tmpDC3->tq=Cyc_Toc_mt_tq,((_tmpDC3->type=Cyc_Toc_typ_to_c_array(
_tmp90D->type),((_tmpDC3->width=_tmp90D->width,((_tmpDC3->attributes=_tmp90D->attributes,
_tmpDC3)))))))))));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp90F=_tmp90E->type;struct _dyneither_ptr*_tmp910=_tmp90E->name;const char*
_tmpDC8;void*_tmpDC7[2];struct Cyc_String_pa_struct _tmpDC6;struct Cyc_String_pa_struct
_tmpDC5;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpDC5.tag=0,((_tmpDC5.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp910),((_tmpDC6.tag=0,((_tmpDC6.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpDC7[0]=&
_tmpDC6,((_tmpDC7[1]=& _tmpDC5,Cyc_aprintf(((_tmpDC8="_union_%s_%s",
_tag_dyneither(_tmpDC8,sizeof(char),13))),_tag_dyneither(_tmpDC7,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpDC9;struct _dyneither_ptr*str=(_tmpDC9=_cycalloc(sizeof(*
_tmpDC9)),((_tmpDC9[0]=s,_tmpDC9)));struct Cyc_Absyn_Aggrfield*_tmpDCA;struct Cyc_Absyn_Aggrfield*
_tmp911=(_tmpDCA=_cycalloc(sizeof(*_tmpDCA)),((_tmpDCA->name=Cyc_Toc_val_sp,((
_tmpDCA->tq=Cyc_Toc_mt_tq,((_tmpDCA->type=_tmp90F,((_tmpDCA->width=0,((_tmpDCA->attributes=
0,_tmpDCA)))))))))));struct Cyc_Absyn_Aggrfield*_tmpDCB;struct Cyc_Absyn_Aggrfield*
_tmp912=(_tmpDCB=_cycalloc(sizeof(*_tmpDCB)),((_tmpDCB->name=Cyc_Toc_tag_sp,((
_tmpDCB->tq=Cyc_Toc_mt_tq,((_tmpDCB->type=Cyc_Absyn_sint_typ,((_tmpDCB->width=0,((
_tmpDCB->attributes=0,_tmpDCB)))))))))));struct Cyc_Absyn_Aggrfield*_tmpDCC[2];
struct Cyc_List_List*_tmp913=(_tmpDCC[1]=_tmp911,((_tmpDCC[0]=_tmp912,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDCC,sizeof(struct Cyc_Absyn_Aggrfield*),
2)))));struct Cyc_Absyn_AggrdeclImpl*_tmpDD1;struct _tuple0*_tmpDD0;struct Cyc_Absyn_Aggrdecl*
_tmpDCF;struct Cyc_Absyn_Aggrdecl*_tmp914=(_tmpDCF=_cycalloc(sizeof(*_tmpDCF)),((
_tmpDCF->kind=Cyc_Absyn_StructA,((_tmpDCF->sc=Cyc_Absyn_Public,((_tmpDCF->name=((
_tmpDD0=_cycalloc(sizeof(*_tmpDD0)),((_tmpDD0->f1=Cyc_Absyn_Loc_n,((_tmpDD0->f2=
str,_tmpDD0)))))),((_tmpDCF->tvs=0,((_tmpDCF->impl=((_tmpDD1=_cycalloc(sizeof(*
_tmpDD1)),((_tmpDD1->exist_vars=0,((_tmpDD1->rgn_po=0,((_tmpDD1->fields=_tmp913,((
_tmpDD1->tagged=0,_tmpDD1)))))))))),((_tmpDCF->attributes=0,_tmpDCF)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmpDD7;struct Cyc_Absyn_Aggr_d_struct _tmpDD6;
struct Cyc_List_List*_tmpDD5;Cyc_Toc_result_decls=((_tmpDD5=_cycalloc(sizeof(*
_tmpDD5)),((_tmpDD5->hd=Cyc_Absyn_new_decl((void*)((_tmpDD7=_cycalloc(sizeof(*
_tmpDD7)),((_tmpDD7[0]=((_tmpDD6.tag=6,((_tmpDD6.f1=_tmp914,_tmpDD6)))),_tmpDD7)))),
0),((_tmpDD5->tl=Cyc_Toc_result_decls,_tmpDD5))))));}_tmp90E->type=Cyc_Absyn_strct(
str);}{struct Cyc_List_List*_tmpDD8;new_fields=((_tmpDD8=_cycalloc(sizeof(*
_tmpDD8)),((_tmpDD8->hd=_tmp90E,((_tmpDD8->tl=new_fields,_tmpDD8))))));};}}(
new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_fields);};}if(add_to_decls){struct Cyc_Absyn_Decl*_tmpDE2;struct Cyc_Absyn_Aggr_d_struct
_tmpDE1;struct Cyc_Absyn_Aggr_d_struct*_tmpDE0;struct Cyc_List_List*_tmpDDF;Cyc_Toc_result_decls=((
_tmpDDF=_cycalloc(sizeof(*_tmpDDF)),((_tmpDDF->hd=((_tmpDE2=_cycalloc(sizeof(*
_tmpDE2)),((_tmpDE2->r=(void*)((_tmpDE0=_cycalloc(sizeof(*_tmpDE0)),((_tmpDE0[0]=((
_tmpDE1.tag=6,((_tmpDE1.f1=new_ad,_tmpDE1)))),_tmpDE0)))),((_tmpDE2->loc=0,
_tmpDE2)))))),((_tmpDDF->tl=Cyc_Toc_result_decls,_tmpDDF))))));}};};
_pop_dynregion(d);};}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud){struct _DynRegionHandle*_tmp92B;struct Cyc_Set_Set**_tmp92C;struct Cyc_Toc_TocState
_tmp92A=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp92B=
_tmp92A.dyn;_tmp92C=_tmp92A.datatypes_so_far;{struct _DynRegionFrame _tmp92D;
struct _RegionHandle*d=_open_dynregion(& _tmp92D,_tmp92B);{struct _tuple0*_tmp92E=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))
Cyc_Set_member)(*_tmp92C,_tmp92E)){_npop_handler(0);return;}*_tmp92C=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*
_tmp92C,_tmp92E);}{struct Cyc_List_List*_tmp92F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp92F != 0;_tmp92F=_tmp92F->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp92F->hd;struct Cyc_List_List*_tmp930=0;int i=
1;{struct Cyc_List_List*_tmp931=f->typs;for(0;_tmp931 != 0;(_tmp931=_tmp931->tl,i
++)){struct _dyneither_ptr*_tmp932=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpDE3;struct Cyc_Absyn_Aggrfield*_tmp933=(_tmpDE3=_cycalloc(sizeof(*_tmpDE3)),((
_tmpDE3->name=_tmp932,((_tmpDE3->tq=(*((struct _tuple9*)_tmp931->hd)).f1,((
_tmpDE3->type=Cyc_Toc_typ_to_c_array((*((struct _tuple9*)_tmp931->hd)).f2),((
_tmpDE3->width=0,((_tmpDE3->attributes=0,_tmpDE3)))))))))));struct Cyc_List_List*
_tmpDE4;_tmp930=((_tmpDE4=_cycalloc(sizeof(*_tmpDE4)),((_tmpDE4->hd=_tmp933,((
_tmpDE4->tl=_tmp930,_tmpDE4))))));}}{struct Cyc_Absyn_Aggrfield*_tmpDE7;struct Cyc_List_List*
_tmpDE6;_tmp930=((_tmpDE6=_cycalloc(sizeof(*_tmpDE6)),((_tmpDE6->hd=((_tmpDE7=
_cycalloc(sizeof(*_tmpDE7)),((_tmpDE7->name=Cyc_Toc_tag_sp,((_tmpDE7->tq=Cyc_Toc_mt_tq,((
_tmpDE7->type=Cyc_Absyn_sint_typ,((_tmpDE7->width=0,((_tmpDE7->attributes=0,
_tmpDE7)))))))))))),((_tmpDE6->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp930),_tmpDE6))))));}{struct Cyc_Absyn_AggrdeclImpl*
_tmpDEC;const char*_tmpDEB;struct Cyc_Absyn_Aggrdecl*_tmpDEA;struct Cyc_Absyn_Aggrdecl*
_tmp938=(_tmpDEA=_cycalloc(sizeof(*_tmpDEA)),((_tmpDEA->kind=Cyc_Absyn_StructA,((
_tmpDEA->sc=Cyc_Absyn_Public,((_tmpDEA->name=Cyc_Toc_collapse_qvar_tag(f->name,((
_tmpDEB="_struct",_tag_dyneither(_tmpDEB,sizeof(char),8)))),((_tmpDEA->tvs=0,((
_tmpDEA->impl=((_tmpDEC=_cycalloc(sizeof(*_tmpDEC)),((_tmpDEC->exist_vars=0,((
_tmpDEC->rgn_po=0,((_tmpDEC->fields=_tmp930,((_tmpDEC->tagged=0,_tmpDEC)))))))))),((
_tmpDEA->attributes=0,_tmpDEA)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpDF2;
struct Cyc_Absyn_Aggr_d_struct _tmpDF1;struct Cyc_List_List*_tmpDF0;Cyc_Toc_result_decls=((
_tmpDF0=_cycalloc(sizeof(*_tmpDF0)),((_tmpDF0->hd=Cyc_Absyn_new_decl((void*)((
_tmpDF2=_cycalloc(sizeof(*_tmpDF2)),((_tmpDF2[0]=((_tmpDF1.tag=6,((_tmpDF1.f1=
_tmp938,_tmpDF1)))),_tmpDF2)))),0),((_tmpDF0->tl=Cyc_Toc_result_decls,_tmpDF0))))));};}};;
_pop_dynregion(d);};}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd){if(xd->fields == 0)return;{struct _DynRegionHandle*_tmp940;struct Cyc_Dict_Dict*
_tmp941;struct Cyc_Toc_TocState _tmp93F=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp940=_tmp93F.dyn;_tmp941=_tmp93F.xdatatypes_so_far;{struct _DynRegionFrame
_tmp942;struct _RegionHandle*d=_open_dynregion(& _tmp942,_tmp940);{struct _tuple0*
_tmp943=xd->name;struct Cyc_List_List*_tmp944=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp944 != 0;_tmp944=_tmp944->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp944->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp945=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp946=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp945,Cyc_Absyn_false_conref,0);int*_tmp947=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp941,f->name);int
_tmp948;_LL3A8: if(_tmp947 != 0)goto _LL3AA;_LL3A9: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp949=Cyc_Absyn_new_vardecl(f->name,_tmp946,
initopt);_tmp949->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*_tmpDF8;struct Cyc_Absyn_Var_d_struct
_tmpDF7;struct Cyc_List_List*_tmpDF6;Cyc_Toc_result_decls=((_tmpDF6=_cycalloc(
sizeof(*_tmpDF6)),((_tmpDF6->hd=Cyc_Absyn_new_decl((void*)((_tmpDF8=_cycalloc(
sizeof(*_tmpDF8)),((_tmpDF8[0]=((_tmpDF7.tag=0,((_tmpDF7.f1=_tmp949,_tmpDF7)))),
_tmpDF8)))),0),((_tmpDF6->tl=Cyc_Toc_result_decls,_tmpDF6))))));}*_tmp941=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp941,f->name,f->sc != Cyc_Absyn_Extern);{struct Cyc_List_List*fields=0;int i=1;{
struct Cyc_List_List*_tmp94D=f->typs;for(0;_tmp94D != 0;(_tmp94D=_tmp94D->tl,i ++)){
struct Cyc_Int_pa_struct _tmpE00;void*_tmpDFF[1];const char*_tmpDFE;struct
_dyneither_ptr*_tmpDFD;struct _dyneither_ptr*_tmp94E=(_tmpDFD=_cycalloc(sizeof(*
_tmpDFD)),((_tmpDFD[0]=(struct _dyneither_ptr)((_tmpE00.tag=1,((_tmpE00.f1=(
unsigned long)i,((_tmpDFF[0]=& _tmpE00,Cyc_aprintf(((_tmpDFE="f%d",_tag_dyneither(
_tmpDFE,sizeof(char),4))),_tag_dyneither(_tmpDFF,sizeof(void*),1)))))))),_tmpDFD)));
struct Cyc_Absyn_Aggrfield*_tmpE01;struct Cyc_Absyn_Aggrfield*_tmp94F=(_tmpE01=
_cycalloc(sizeof(*_tmpE01)),((_tmpE01->name=_tmp94E,((_tmpE01->tq=(*((struct
_tuple9*)_tmp94D->hd)).f1,((_tmpE01->type=Cyc_Toc_typ_to_c_array((*((struct
_tuple9*)_tmp94D->hd)).f2),((_tmpE01->width=0,((_tmpE01->attributes=0,_tmpE01)))))))))));
struct Cyc_List_List*_tmpE02;fields=((_tmpE02=_cycalloc(sizeof(*_tmpE02)),((
_tmpE02->hd=_tmp94F,((_tmpE02->tl=fields,_tmpE02))))));}}{struct Cyc_Absyn_Aggrfield*
_tmpE05;struct Cyc_List_List*_tmpE04;fields=((_tmpE04=_cycalloc(sizeof(*_tmpE04)),((
_tmpE04->hd=((_tmpE05=_cycalloc(sizeof(*_tmpE05)),((_tmpE05->name=Cyc_Toc_tag_sp,((
_tmpE05->tq=Cyc_Toc_mt_tq,((_tmpE05->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq),((_tmpE05->width=0,((_tmpE05->attributes=0,_tmpE05)))))))))))),((
_tmpE04->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmpE04))))));}{struct Cyc_Absyn_AggrdeclImpl*_tmpE0A;const char*_tmpE09;
struct Cyc_Absyn_Aggrdecl*_tmpE08;struct Cyc_Absyn_Aggrdecl*_tmp958=(_tmpE08=
_cycalloc(sizeof(*_tmpE08)),((_tmpE08->kind=Cyc_Absyn_StructA,((_tmpE08->sc=Cyc_Absyn_Public,((
_tmpE08->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpE09="_struct",
_tag_dyneither(_tmpE09,sizeof(char),8)))),((_tmpE08->tvs=0,((_tmpE08->impl=((
_tmpE0A=_cycalloc(sizeof(*_tmpE0A)),((_tmpE0A->exist_vars=0,((_tmpE0A->rgn_po=0,((
_tmpE0A->fields=fields,((_tmpE0A->tagged=0,_tmpE0A)))))))))),((_tmpE08->attributes=
0,_tmpE08)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpE10;struct Cyc_Absyn_Aggr_d_struct
_tmpE0F;struct Cyc_List_List*_tmpE0E;Cyc_Toc_result_decls=((_tmpE0E=_cycalloc(
sizeof(*_tmpE0E)),((_tmpE0E->hd=Cyc_Absyn_new_decl((void*)((_tmpE10=_cycalloc(
sizeof(*_tmpE10)),((_tmpE10[0]=((_tmpE0F.tag=6,((_tmpE0F.f1=_tmp958,_tmpE0F)))),
_tmpE10)))),0),((_tmpE0E->tl=Cyc_Toc_result_decls,_tmpE0E))))));}goto _LL3A7;};};};}
_LL3AA: if(_tmp947 == 0)goto _LL3AC;_tmp948=*_tmp947;if(_tmp948 != 0)goto _LL3AC;
_LL3AB: if(f->sc != Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*_tmp95F=Cyc_Absyn_string_exp(*
fn,0);struct Cyc_Absyn_Vardecl*_tmp960=Cyc_Absyn_new_vardecl(f->name,_tmp946,(
struct Cyc_Absyn_Exp*)_tmp95F);_tmp960->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*
_tmpE16;struct Cyc_Absyn_Var_d_struct _tmpE15;struct Cyc_List_List*_tmpE14;Cyc_Toc_result_decls=((
_tmpE14=_cycalloc(sizeof(*_tmpE14)),((_tmpE14->hd=Cyc_Absyn_new_decl((void*)((
_tmpE16=_cycalloc(sizeof(*_tmpE16)),((_tmpE16[0]=((_tmpE15.tag=0,((_tmpE15.f1=
_tmp960,_tmpE15)))),_tmpE16)))),0),((_tmpE14->tl=Cyc_Toc_result_decls,_tmpE14))))));}*
_tmp941=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp941,f->name,1);}goto _LL3A7;_LL3AC:;_LL3AD: goto _LL3A7;_LL3A7:;}};
_pop_dynregion(d);};};}static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*
ed){ed->sc=Cyc_Absyn_Public;if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(
struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,
struct Cyc_Absyn_Stmt*s){void*old_typ=vd->type;vd->type=Cyc_Toc_typ_to_c_array(
old_typ);if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ))vd->sc=Cyc_Absyn_Public;Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp964=init->r;struct Cyc_Absyn_Vardecl*_tmp966;struct Cyc_Absyn_Exp*_tmp967;
struct Cyc_Absyn_Exp*_tmp968;int _tmp969;_LL3AF: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp965=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp964;if(_tmp965->tag != 28)
goto _LL3B1;else{_tmp966=_tmp965->f1;_tmp967=_tmp965->f2;_tmp968=_tmp965->f3;
_tmp969=_tmp965->f4;}}_LL3B0: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp966,_tmp967,_tmp968,_tmp969,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL3AE;_LL3B1:;_LL3B2: if(vd->sc == Cyc_Absyn_Static){struct Cyc_Toc_Env
_tmp96B;struct _RegionHandle*_tmp96C;struct Cyc_Toc_Env*_tmp96A=init_nv;_tmp96B=*
_tmp96A;_tmp96C=_tmp96B.rgn;{struct Cyc_Toc_Env*_tmp96D=Cyc_Toc_set_toplevel(
_tmp96C,init_nv);Cyc_Toc_exp_to_c(_tmp96D,init);};}else{Cyc_Toc_exp_to_c(init_nv,
init);}goto _LL3AE;_LL3AE:;}else{void*_tmp96E=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_ArrayInfo _tmp970;void*_tmp971;struct Cyc_Absyn_Exp*_tmp972;union Cyc_Absyn_Constraint*
_tmp973;_LL3B4:{struct Cyc_Absyn_ArrayType_struct*_tmp96F=(struct Cyc_Absyn_ArrayType_struct*)
_tmp96E;if(_tmp96F->tag != 9)goto _LL3B6;else{_tmp970=_tmp96F->f1;_tmp971=_tmp970.elt_type;
_tmp972=_tmp970.num_elts;_tmp973=_tmp970.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp973))goto _LL3B6;_LL3B5: if(_tmp972 == 0){const char*
_tmpE19;void*_tmpE18;(_tmpE18=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE19="can't initialize zero-terminated array -- size unknown",
_tag_dyneither(_tmpE19,sizeof(char),55))),_tag_dyneither(_tmpE18,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp972;struct Cyc_Absyn_Exp*
_tmp976=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp977=Cyc_Absyn_signed_int_exp(
0,0);s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp976,
_tmp977,0),0),Cyc_Absyn_new_stmt(s->r,0));goto _LL3B3;};_LL3B6:;_LL3B7: goto _LL3B3;
_LL3B3:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple0*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp979;struct
_RegionHandle*_tmp97A;struct Cyc_Toc_Env*_tmp978=nv;_tmp979=*_tmp978;_tmp97A=
_tmp979.rgn;{struct Cyc_Absyn_Stmt*_tmp97B=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp97C=Cyc_Toc_share_env(_tmp97A,nv);struct Cyc_Toc_Env*_tmp97E;struct Cyc_List_List*
_tmp97F;struct Cyc_Absyn_Stmt*_tmp980;struct _tuple21 _tmp97D=Cyc_Toc_xlate_pat(
_tmp97C,_tmp97A,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp97E=_tmp97D.f1;_tmp97F=_tmp97D.f2;_tmp980=_tmp97D.f3;Cyc_Toc_stmt_to_c(
_tmp97E,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp980,s,0),0);for(0;_tmp97F != 0;_tmp97F=_tmp97F->tl){
struct _tuple22 _tmp982;struct _tuple0*_tmp983;void*_tmp984;struct _tuple22*_tmp981=(
struct _tuple22*)_tmp97F->hd;_tmp982=*_tmp981;_tmp983=_tmp982.f1;_tmp984=_tmp982.f2;
s=Cyc_Absyn_declare_stmt(_tmp983,_tmp984,0,s,0);}}return s;};}static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e){void*_tmp985=e->r;struct Cyc_Absyn_Exp*_tmp987;struct Cyc_Absyn_Exp*
_tmp989;struct Cyc_Absyn_Exp*_tmp98B;struct Cyc_Absyn_Exp*_tmp98D;struct Cyc_Absyn_Exp*
_tmp98F;struct Cyc_Absyn_Exp*_tmp991;struct Cyc_Absyn_Exp*_tmp993;struct Cyc_Absyn_Exp*
_tmp995;struct Cyc_List_List*_tmp997;struct Cyc_Absyn_Exp*_tmp999;struct Cyc_Absyn_Exp*
_tmp99A;struct Cyc_Absyn_Exp*_tmp99C;struct Cyc_Absyn_Exp*_tmp99D;struct Cyc_Absyn_Exp*
_tmp99F;struct Cyc_Absyn_Exp*_tmp9A0;struct Cyc_Absyn_Exp*_tmp9A2;struct Cyc_Absyn_Exp*
_tmp9A3;struct Cyc_Absyn_Exp*_tmp9A5;struct Cyc_Absyn_Exp*_tmp9A6;struct Cyc_Absyn_Exp*
_tmp9A8;struct Cyc_Absyn_Exp*_tmp9A9;struct Cyc_Absyn_Exp*_tmp9AB;struct Cyc_Absyn_Exp*
_tmp9AC;struct Cyc_Absyn_Exp*_tmp9AD;struct Cyc_Absyn_Exp*_tmp9AF;struct Cyc_List_List*
_tmp9B0;struct Cyc_Absyn_Exp*_tmp9B2;struct Cyc_List_List*_tmp9B3;void*_tmp9B5;
void**_tmp9B6;struct Cyc_Absyn_Exp*_tmp9B7;struct _tuple7*_tmp9B9;struct _tuple7
_tmp9BA;void*_tmp9BB;void**_tmp9BC;struct Cyc_List_List*_tmp9BD;struct Cyc_List_List*
_tmp9BF;struct Cyc_List_List*_tmp9C1;void*_tmp9C3;void**_tmp9C4;void*_tmp9C6;void**
_tmp9C7;struct Cyc_Absyn_Stmt*_tmp9C9;struct Cyc_Absyn_MallocInfo _tmp9CB;struct Cyc_Absyn_MallocInfo*
_tmp9CC;_LL3B9: {struct Cyc_Absyn_Deref_e_struct*_tmp986=(struct Cyc_Absyn_Deref_e_struct*)
_tmp985;if(_tmp986->tag != 21)goto _LL3BB;else{_tmp987=_tmp986->f1;}}_LL3BA:
_tmp989=_tmp987;goto _LL3BC;_LL3BB: {struct Cyc_Absyn_AggrMember_e_struct*_tmp988=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp985;if(_tmp988->tag != 22)goto _LL3BD;
else{_tmp989=_tmp988->f1;}}_LL3BC: _tmp98B=_tmp989;goto _LL3BE;_LL3BD: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp98A=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp985;if(_tmp98A->tag != 23)goto
_LL3BF;else{_tmp98B=_tmp98A->f1;}}_LL3BE: _tmp98D=_tmp98B;goto _LL3C0;_LL3BF: {
struct Cyc_Absyn_Address_e_struct*_tmp98C=(struct Cyc_Absyn_Address_e_struct*)
_tmp985;if(_tmp98C->tag != 16)goto _LL3C1;else{_tmp98D=_tmp98C->f1;}}_LL3C0:
_tmp98F=_tmp98D;goto _LL3C2;_LL3C1: {struct Cyc_Absyn_Throw_e_struct*_tmp98E=(
struct Cyc_Absyn_Throw_e_struct*)_tmp985;if(_tmp98E->tag != 12)goto _LL3C3;else{
_tmp98F=_tmp98E->f1;}}_LL3C2: _tmp991=_tmp98F;goto _LL3C4;_LL3C3: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp990=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp985;if(_tmp990->tag != 13)
goto _LL3C5;else{_tmp991=_tmp990->f1;}}_LL3C4: _tmp993=_tmp991;goto _LL3C6;_LL3C5: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp992=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp985;if(_tmp992->tag != 19)goto _LL3C7;else{_tmp993=_tmp992->f1;}}_LL3C6:
_tmp995=_tmp993;goto _LL3C8;_LL3C7: {struct Cyc_Absyn_Increment_e_struct*_tmp994=(
struct Cyc_Absyn_Increment_e_struct*)_tmp985;if(_tmp994->tag != 5)goto _LL3C9;else{
_tmp995=_tmp994->f1;}}_LL3C8: Cyc_Toc_exptypes_to_c(_tmp995);goto _LL3B8;_LL3C9: {
struct Cyc_Absyn_Primop_e_struct*_tmp996=(struct Cyc_Absyn_Primop_e_struct*)
_tmp985;if(_tmp996->tag != 3)goto _LL3CB;else{_tmp997=_tmp996->f2;}}_LL3CA:((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,
_tmp997);goto _LL3B8;_LL3CB: {struct Cyc_Absyn_And_e_struct*_tmp998=(struct Cyc_Absyn_And_e_struct*)
_tmp985;if(_tmp998->tag != 7)goto _LL3CD;else{_tmp999=_tmp998->f1;_tmp99A=_tmp998->f2;}}
_LL3CC: _tmp99C=_tmp999;_tmp99D=_tmp99A;goto _LL3CE;_LL3CD: {struct Cyc_Absyn_Or_e_struct*
_tmp99B=(struct Cyc_Absyn_Or_e_struct*)_tmp985;if(_tmp99B->tag != 8)goto _LL3CF;
else{_tmp99C=_tmp99B->f1;_tmp99D=_tmp99B->f2;}}_LL3CE: _tmp99F=_tmp99C;_tmp9A0=
_tmp99D;goto _LL3D0;_LL3CF: {struct Cyc_Absyn_SeqExp_e_struct*_tmp99E=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp985;if(_tmp99E->tag != 9)goto _LL3D1;else{_tmp99F=_tmp99E->f1;_tmp9A0=_tmp99E->f2;}}
_LL3D0: _tmp9A2=_tmp99F;_tmp9A3=_tmp9A0;goto _LL3D2;_LL3D1: {struct Cyc_Absyn_Subscript_e_struct*
_tmp9A1=(struct Cyc_Absyn_Subscript_e_struct*)_tmp985;if(_tmp9A1->tag != 24)goto
_LL3D3;else{_tmp9A2=_tmp9A1->f1;_tmp9A3=_tmp9A1->f2;}}_LL3D2: _tmp9A5=_tmp9A2;
_tmp9A6=_tmp9A3;goto _LL3D4;_LL3D3: {struct Cyc_Absyn_Swap_e_struct*_tmp9A4=(
struct Cyc_Absyn_Swap_e_struct*)_tmp985;if(_tmp9A4->tag != 35)goto _LL3D5;else{
_tmp9A5=_tmp9A4->f1;_tmp9A6=_tmp9A4->f2;}}_LL3D4: _tmp9A8=_tmp9A5;_tmp9A9=_tmp9A6;
goto _LL3D6;_LL3D5: {struct Cyc_Absyn_AssignOp_e_struct*_tmp9A7=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp985;if(_tmp9A7->tag != 4)goto _LL3D7;else{_tmp9A8=_tmp9A7->f1;_tmp9A9=_tmp9A7->f3;}}
_LL3D6: Cyc_Toc_exptypes_to_c(_tmp9A8);Cyc_Toc_exptypes_to_c(_tmp9A9);goto _LL3B8;
_LL3D7: {struct Cyc_Absyn_Conditional_e_struct*_tmp9AA=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp985;if(_tmp9AA->tag != 6)goto _LL3D9;else{_tmp9AB=_tmp9AA->f1;_tmp9AC=_tmp9AA->f2;
_tmp9AD=_tmp9AA->f3;}}_LL3D8: Cyc_Toc_exptypes_to_c(_tmp9AB);Cyc_Toc_exptypes_to_c(
_tmp9AC);Cyc_Toc_exptypes_to_c(_tmp9AD);goto _LL3B8;_LL3D9: {struct Cyc_Absyn_FnCall_e_struct*
_tmp9AE=(struct Cyc_Absyn_FnCall_e_struct*)_tmp985;if(_tmp9AE->tag != 11)goto
_LL3DB;else{_tmp9AF=_tmp9AE->f1;_tmp9B0=_tmp9AE->f2;}}_LL3DA: _tmp9B2=_tmp9AF;
_tmp9B3=_tmp9B0;goto _LL3DC;_LL3DB: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp9B1=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp985;if(_tmp9B1->tag != 10)goto _LL3DD;
else{_tmp9B2=_tmp9B1->f1;_tmp9B3=_tmp9B1->f2;}}_LL3DC: Cyc_Toc_exptypes_to_c(
_tmp9B2);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp9B3);goto _LL3B8;_LL3DD: {struct Cyc_Absyn_Cast_e_struct*
_tmp9B4=(struct Cyc_Absyn_Cast_e_struct*)_tmp985;if(_tmp9B4->tag != 15)goto _LL3DF;
else{_tmp9B5=(void**)& _tmp9B4->f1;_tmp9B6=(void**)((void**)& _tmp9B4->f1);_tmp9B7=
_tmp9B4->f2;}}_LL3DE:*_tmp9B6=Cyc_Toc_typ_to_c(*_tmp9B6);Cyc_Toc_exptypes_to_c(
_tmp9B7);goto _LL3B8;_LL3DF: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp9B8=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp985;if(_tmp9B8->tag != 26)goto _LL3E1;
else{_tmp9B9=_tmp9B8->f1;_tmp9BA=*_tmp9B9;_tmp9BB=_tmp9BA.f3;_tmp9BC=(void**)&(*
_tmp9B8->f1).f3;_tmp9BD=_tmp9B8->f2;}}_LL3E0:*_tmp9BC=Cyc_Toc_typ_to_c(*_tmp9BC);
_tmp9BF=_tmp9BD;goto _LL3E2;_LL3E1: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp9BE=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp985;if(_tmp9BE->tag != 36)
goto _LL3E3;else{_tmp9BF=_tmp9BE->f2;}}_LL3E2: _tmp9C1=_tmp9BF;goto _LL3E4;_LL3E3: {
struct Cyc_Absyn_Array_e_struct*_tmp9C0=(struct Cyc_Absyn_Array_e_struct*)_tmp985;
if(_tmp9C0->tag != 27)goto _LL3E5;else{_tmp9C1=_tmp9C0->f1;}}_LL3E4: for(0;_tmp9C1
!= 0;_tmp9C1=_tmp9C1->tl){struct Cyc_Absyn_Exp*_tmp9DD;struct _tuple15 _tmp9DC=*((
struct _tuple15*)_tmp9C1->hd);_tmp9DD=_tmp9DC.f2;Cyc_Toc_exptypes_to_c(_tmp9DD);}
goto _LL3B8;_LL3E5: {struct Cyc_Absyn_Offsetof_e_struct*_tmp9C2=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp985;if(_tmp9C2->tag != 20)goto _LL3E7;else{_tmp9C3=(void**)& _tmp9C2->f1;
_tmp9C4=(void**)((void**)& _tmp9C2->f1);}}_LL3E6: _tmp9C7=_tmp9C4;goto _LL3E8;
_LL3E7: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp9C5=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp985;if(_tmp9C5->tag != 18)goto _LL3E9;else{_tmp9C6=(void**)& _tmp9C5->f1;
_tmp9C7=(void**)((void**)& _tmp9C5->f1);}}_LL3E8:*_tmp9C7=Cyc_Toc_typ_to_c(*
_tmp9C7);goto _LL3B8;_LL3E9: {struct Cyc_Absyn_StmtExp_e_struct*_tmp9C8=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp985;if(_tmp9C8->tag != 37)goto _LL3EB;else{_tmp9C9=_tmp9C8->f1;}}_LL3EA: Cyc_Toc_stmttypes_to_c(
_tmp9C9);goto _LL3B8;_LL3EB: {struct Cyc_Absyn_Malloc_e_struct*_tmp9CA=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp985;if(_tmp9CA->tag != 34)goto _LL3ED;else{_tmp9CB=_tmp9CA->f1;_tmp9CC=(struct
Cyc_Absyn_MallocInfo*)& _tmp9CA->f1;}}_LL3EC: if(_tmp9CC->elt_type != 0){void**
_tmpE1A;_tmp9CC->elt_type=((_tmpE1A=_cycalloc(sizeof(*_tmpE1A)),((_tmpE1A[0]=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp9CC->elt_type))),_tmpE1A))));}Cyc_Toc_exptypes_to_c(
_tmp9CC->num_elts);goto _LL3B8;_LL3ED: {struct Cyc_Absyn_Const_e_struct*_tmp9CD=(
struct Cyc_Absyn_Const_e_struct*)_tmp985;if(_tmp9CD->tag != 0)goto _LL3EF;}_LL3EE:
goto _LL3F0;_LL3EF: {struct Cyc_Absyn_Var_e_struct*_tmp9CE=(struct Cyc_Absyn_Var_e_struct*)
_tmp985;if(_tmp9CE->tag != 1)goto _LL3F1;}_LL3F0: goto _LL3F2;_LL3F1: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp9CF=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp985;if(_tmp9CF->tag != 2)goto
_LL3F3;}_LL3F2: goto _LL3F4;_LL3F3: {struct Cyc_Absyn_Enum_e_struct*_tmp9D0=(struct
Cyc_Absyn_Enum_e_struct*)_tmp985;if(_tmp9D0->tag != 32)goto _LL3F5;}_LL3F4: goto
_LL3F6;_LL3F5: {struct Cyc_Absyn_Asm_e_struct*_tmp9D1=(struct Cyc_Absyn_Asm_e_struct*)
_tmp985;if(_tmp9D1->tag != 40)goto _LL3F7;}_LL3F6: goto _LL3F8;_LL3F7: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp9D2=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp985;if(_tmp9D2->tag != 33)goto
_LL3F9;}_LL3F8: goto _LL3B8;_LL3F9: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp9D3=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp985;if(_tmp9D3->tag != 30)goto _LL3FB;}
_LL3FA: goto _LL3FC;_LL3FB: {struct Cyc_Absyn_Datatype_e_struct*_tmp9D4=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp985;if(_tmp9D4->tag != 31)goto _LL3FD;}_LL3FC: goto _LL3FE;_LL3FD: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp9D5=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp985;if(_tmp9D5->tag != 29)goto
_LL3FF;}_LL3FE: goto _LL400;_LL3FF: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp9D6=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp985;if(_tmp9D6->tag != 28)
goto _LL401;}_LL400: goto _LL402;_LL401: {struct Cyc_Absyn_Tuple_e_struct*_tmp9D7=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp985;if(_tmp9D7->tag != 25)goto _LL403;}_LL402:
goto _LL404;_LL403: {struct Cyc_Absyn_Instantiate_e_struct*_tmp9D8=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp985;if(_tmp9D8->tag != 14)goto _LL405;}_LL404: goto _LL406;_LL405: {struct Cyc_Absyn_New_e_struct*
_tmp9D9=(struct Cyc_Absyn_New_e_struct*)_tmp985;if(_tmp9D9->tag != 17)goto _LL407;}
_LL406: goto _LL408;_LL407: {struct Cyc_Absyn_Valueof_e_struct*_tmp9DA=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp985;if(_tmp9DA->tag != 39)goto _LL409;}_LL408: goto _LL40A;_LL409: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp9DB=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp985;if(_tmp9DB->tag != 38)goto
_LL3B8;}_LL40A:{const char*_tmpE1D;void*_tmpE1C;(_tmpE1C=0,Cyc_Tcutil_terr(e->loc,((
_tmpE1D="Cyclone expression within C code",_tag_dyneither(_tmpE1D,sizeof(char),
33))),_tag_dyneither(_tmpE1C,sizeof(void*),0)));}goto _LL3B8;_LL3B8:;}static void
Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){void*_tmp9E1=d->r;struct Cyc_Absyn_Vardecl*
_tmp9E3;struct Cyc_Absyn_Fndecl*_tmp9E5;struct Cyc_Absyn_Aggrdecl*_tmp9E7;struct
Cyc_Absyn_Enumdecl*_tmp9E9;struct Cyc_Absyn_Typedefdecl*_tmp9EB;_LL40C: {struct
Cyc_Absyn_Var_d_struct*_tmp9E2=(struct Cyc_Absyn_Var_d_struct*)_tmp9E1;if(_tmp9E2->tag
!= 0)goto _LL40E;else{_tmp9E3=_tmp9E2->f1;}}_LL40D: _tmp9E3->type=Cyc_Toc_typ_to_c(
_tmp9E3->type);if(_tmp9E3->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp9E3->initializer));goto _LL40B;_LL40E: {struct Cyc_Absyn_Fn_d_struct*
_tmp9E4=(struct Cyc_Absyn_Fn_d_struct*)_tmp9E1;if(_tmp9E4->tag != 1)goto _LL410;
else{_tmp9E5=_tmp9E4->f1;}}_LL40F: _tmp9E5->ret_type=Cyc_Toc_typ_to_c(_tmp9E5->ret_type);{
struct Cyc_List_List*_tmp9F7=_tmp9E5->args;for(0;_tmp9F7 != 0;_tmp9F7=_tmp9F7->tl){(*((
struct _tuple25*)_tmp9F7->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple25*)_tmp9F7->hd)).f3);}}
goto _LL40B;_LL410: {struct Cyc_Absyn_Aggr_d_struct*_tmp9E6=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp9E1;if(_tmp9E6->tag != 6)goto _LL412;else{_tmp9E7=_tmp9E6->f1;}}_LL411: Cyc_Toc_aggrdecl_to_c(
_tmp9E7,1);goto _LL40B;_LL412: {struct Cyc_Absyn_Enum_d_struct*_tmp9E8=(struct Cyc_Absyn_Enum_d_struct*)
_tmp9E1;if(_tmp9E8->tag != 8)goto _LL414;else{_tmp9E9=_tmp9E8->f1;}}_LL413: if(
_tmp9E9->fields != 0){struct Cyc_List_List*_tmp9F8=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp9E9->fields))->v;for(0;_tmp9F8 != 0;_tmp9F8=_tmp9F8->tl){
struct Cyc_Absyn_Enumfield*_tmp9F9=(struct Cyc_Absyn_Enumfield*)_tmp9F8->hd;if(
_tmp9F9->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9F9->tag));}}
goto _LL40B;_LL414: {struct Cyc_Absyn_Typedef_d_struct*_tmp9EA=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp9E1;if(_tmp9EA->tag != 9)goto _LL416;else{_tmp9EB=_tmp9EA->f1;}}_LL415:{struct
Cyc_Core_Opt*_tmpE1E;_tmp9EB->defn=((_tmpE1E=_cycalloc(sizeof(*_tmpE1E)),((
_tmpE1E->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp9EB->defn))->v),_tmpE1E))));}goto _LL40B;_LL416: {struct Cyc_Absyn_Let_d_struct*
_tmp9EC=(struct Cyc_Absyn_Let_d_struct*)_tmp9E1;if(_tmp9EC->tag != 2)goto _LL418;}
_LL417: goto _LL419;_LL418: {struct Cyc_Absyn_Letv_d_struct*_tmp9ED=(struct Cyc_Absyn_Letv_d_struct*)
_tmp9E1;if(_tmp9ED->tag != 3)goto _LL41A;}_LL419: goto _LL41B;_LL41A: {struct Cyc_Absyn_Datatype_d_struct*
_tmp9EE=(struct Cyc_Absyn_Datatype_d_struct*)_tmp9E1;if(_tmp9EE->tag != 7)goto
_LL41C;}_LL41B: goto _LL41D;_LL41C: {struct Cyc_Absyn_Namespace_d_struct*_tmp9EF=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp9E1;if(_tmp9EF->tag != 10)goto _LL41E;}
_LL41D: goto _LL41F;_LL41E: {struct Cyc_Absyn_Using_d_struct*_tmp9F0=(struct Cyc_Absyn_Using_d_struct*)
_tmp9E1;if(_tmp9F0->tag != 11)goto _LL420;}_LL41F: goto _LL421;_LL420: {struct Cyc_Absyn_ExternC_d_struct*
_tmp9F1=(struct Cyc_Absyn_ExternC_d_struct*)_tmp9E1;if(_tmp9F1->tag != 12)goto
_LL422;}_LL421: goto _LL423;_LL422: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp9F2=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9E1;if(_tmp9F2->tag != 13)
goto _LL424;}_LL423: goto _LL425;_LL424: {struct Cyc_Absyn_Region_d_struct*_tmp9F3=(
struct Cyc_Absyn_Region_d_struct*)_tmp9E1;if(_tmp9F3->tag != 4)goto _LL426;}_LL425:
goto _LL427;_LL426: {struct Cyc_Absyn_Alias_d_struct*_tmp9F4=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9E1;if(_tmp9F4->tag != 5)goto _LL428;}_LL427:{const char*_tmpE21;void*_tmpE20;(
_tmpE20=0,Cyc_Tcutil_terr(d->loc,((_tmpE21="Cyclone declaration within C code",
_tag_dyneither(_tmpE21,sizeof(char),34))),_tag_dyneither(_tmpE20,sizeof(void*),0)));}
goto _LL40B;_LL428: {struct Cyc_Absyn_Porton_d_struct*_tmp9F5=(struct Cyc_Absyn_Porton_d_struct*)
_tmp9E1;if(_tmp9F5->tag != 14)goto _LL42A;}_LL429: goto _LL42B;_LL42A: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9F6=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9E1;if(_tmp9F6->tag != 15)goto
_LL40B;}_LL42B: goto _LL40B;_LL40B:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s){void*_tmp9FD=s->r;struct Cyc_Absyn_Exp*_tmp9FF;struct Cyc_Absyn_Stmt*_tmpA01;
struct Cyc_Absyn_Stmt*_tmpA02;struct Cyc_Absyn_Exp*_tmpA04;struct Cyc_Absyn_Exp*
_tmpA06;struct Cyc_Absyn_Stmt*_tmpA07;struct Cyc_Absyn_Stmt*_tmpA08;struct _tuple8
_tmpA0A;struct Cyc_Absyn_Exp*_tmpA0B;struct Cyc_Absyn_Stmt*_tmpA0C;struct Cyc_Absyn_Exp*
_tmpA0E;struct _tuple8 _tmpA0F;struct Cyc_Absyn_Exp*_tmpA10;struct _tuple8 _tmpA11;
struct Cyc_Absyn_Exp*_tmpA12;struct Cyc_Absyn_Stmt*_tmpA13;struct Cyc_Absyn_Exp*
_tmpA15;struct Cyc_List_List*_tmpA16;struct Cyc_Absyn_Decl*_tmpA18;struct Cyc_Absyn_Stmt*
_tmpA19;struct Cyc_Absyn_Stmt*_tmpA1B;struct _tuple8 _tmpA1C;struct Cyc_Absyn_Exp*
_tmpA1D;_LL42D: {struct Cyc_Absyn_Exp_s_struct*_tmp9FE=(struct Cyc_Absyn_Exp_s_struct*)
_tmp9FD;if(_tmp9FE->tag != 1)goto _LL42F;else{_tmp9FF=_tmp9FE->f1;}}_LL42E: Cyc_Toc_exptypes_to_c(
_tmp9FF);goto _LL42C;_LL42F: {struct Cyc_Absyn_Seq_s_struct*_tmpA00=(struct Cyc_Absyn_Seq_s_struct*)
_tmp9FD;if(_tmpA00->tag != 2)goto _LL431;else{_tmpA01=_tmpA00->f1;_tmpA02=_tmpA00->f2;}}
_LL430: Cyc_Toc_stmttypes_to_c(_tmpA01);Cyc_Toc_stmttypes_to_c(_tmpA02);goto
_LL42C;_LL431: {struct Cyc_Absyn_Return_s_struct*_tmpA03=(struct Cyc_Absyn_Return_s_struct*)
_tmp9FD;if(_tmpA03->tag != 3)goto _LL433;else{_tmpA04=_tmpA03->f1;}}_LL432: if(
_tmpA04 != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmpA04);goto _LL42C;
_LL433: {struct Cyc_Absyn_IfThenElse_s_struct*_tmpA05=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp9FD;if(_tmpA05->tag != 4)goto _LL435;else{_tmpA06=_tmpA05->f1;_tmpA07=_tmpA05->f2;
_tmpA08=_tmpA05->f3;}}_LL434: Cyc_Toc_exptypes_to_c(_tmpA06);Cyc_Toc_stmttypes_to_c(
_tmpA07);Cyc_Toc_stmttypes_to_c(_tmpA08);goto _LL42C;_LL435: {struct Cyc_Absyn_While_s_struct*
_tmpA09=(struct Cyc_Absyn_While_s_struct*)_tmp9FD;if(_tmpA09->tag != 5)goto _LL437;
else{_tmpA0A=_tmpA09->f1;_tmpA0B=_tmpA0A.f1;_tmpA0C=_tmpA09->f2;}}_LL436: Cyc_Toc_exptypes_to_c(
_tmpA0B);Cyc_Toc_stmttypes_to_c(_tmpA0C);goto _LL42C;_LL437: {struct Cyc_Absyn_For_s_struct*
_tmpA0D=(struct Cyc_Absyn_For_s_struct*)_tmp9FD;if(_tmpA0D->tag != 9)goto _LL439;
else{_tmpA0E=_tmpA0D->f1;_tmpA0F=_tmpA0D->f2;_tmpA10=_tmpA0F.f1;_tmpA11=_tmpA0D->f3;
_tmpA12=_tmpA11.f1;_tmpA13=_tmpA0D->f4;}}_LL438: Cyc_Toc_exptypes_to_c(_tmpA0E);
Cyc_Toc_exptypes_to_c(_tmpA10);Cyc_Toc_exptypes_to_c(_tmpA12);Cyc_Toc_stmttypes_to_c(
_tmpA13);goto _LL42C;_LL439: {struct Cyc_Absyn_Switch_s_struct*_tmpA14=(struct Cyc_Absyn_Switch_s_struct*)
_tmp9FD;if(_tmpA14->tag != 10)goto _LL43B;else{_tmpA15=_tmpA14->f1;_tmpA16=_tmpA14->f2;}}
_LL43A: Cyc_Toc_exptypes_to_c(_tmpA15);for(0;_tmpA16 != 0;_tmpA16=_tmpA16->tl){Cyc_Toc_stmttypes_to_c(((
struct Cyc_Absyn_Switch_clause*)_tmpA16->hd)->body);}goto _LL42C;_LL43B: {struct
Cyc_Absyn_Decl_s_struct*_tmpA17=(struct Cyc_Absyn_Decl_s_struct*)_tmp9FD;if(
_tmpA17->tag != 12)goto _LL43D;else{_tmpA18=_tmpA17->f1;_tmpA19=_tmpA17->f2;}}
_LL43C: Cyc_Toc_decltypes_to_c(_tmpA18);Cyc_Toc_stmttypes_to_c(_tmpA19);goto
_LL42C;_LL43D: {struct Cyc_Absyn_Do_s_struct*_tmpA1A=(struct Cyc_Absyn_Do_s_struct*)
_tmp9FD;if(_tmpA1A->tag != 14)goto _LL43F;else{_tmpA1B=_tmpA1A->f1;_tmpA1C=_tmpA1A->f2;
_tmpA1D=_tmpA1C.f1;}}_LL43E: Cyc_Toc_stmttypes_to_c(_tmpA1B);Cyc_Toc_exptypes_to_c(
_tmpA1D);goto _LL42C;_LL43F: {struct Cyc_Absyn_Skip_s_struct*_tmpA1E=(struct Cyc_Absyn_Skip_s_struct*)
_tmp9FD;if(_tmpA1E->tag != 0)goto _LL441;}_LL440: goto _LL442;_LL441: {struct Cyc_Absyn_Break_s_struct*
_tmpA1F=(struct Cyc_Absyn_Break_s_struct*)_tmp9FD;if(_tmpA1F->tag != 6)goto _LL443;}
_LL442: goto _LL444;_LL443: {struct Cyc_Absyn_Continue_s_struct*_tmpA20=(struct Cyc_Absyn_Continue_s_struct*)
_tmp9FD;if(_tmpA20->tag != 7)goto _LL445;}_LL444: goto _LL446;_LL445: {struct Cyc_Absyn_Goto_s_struct*
_tmpA21=(struct Cyc_Absyn_Goto_s_struct*)_tmp9FD;if(_tmpA21->tag != 8)goto _LL447;}
_LL446: goto _LL42C;_LL447: {struct Cyc_Absyn_Fallthru_s_struct*_tmpA22=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp9FD;if(_tmpA22->tag != 11)goto _LL449;}_LL448: goto _LL44A;_LL449: {struct Cyc_Absyn_Label_s_struct*
_tmpA23=(struct Cyc_Absyn_Label_s_struct*)_tmp9FD;if(_tmpA23->tag != 13)goto _LL44B;}
_LL44A: goto _LL44C;_LL44B: {struct Cyc_Absyn_TryCatch_s_struct*_tmpA24=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp9FD;if(_tmpA24->tag != 15)goto _LL44D;}_LL44C: goto _LL44E;_LL44D: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmpA25=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp9FD;if(_tmpA25->tag != 16)goto
_LL42C;}_LL44E:{const char*_tmpE24;void*_tmpE23;(_tmpE23=0,Cyc_Tcutil_terr(s->loc,((
_tmpE24="Cyclone statement in C code",_tag_dyneither(_tmpE24,sizeof(char),28))),
_tag_dyneither(_tmpE23,sizeof(void*),0)));}goto _LL42C;_LL42C:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude){for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpE27;void*_tmpE26;(_tmpE26=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE27="decls_to_c: not at toplevel!",
_tag_dyneither(_tmpE27,sizeof(char),29))),_tag_dyneither(_tmpE26,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmpA2A=d->r;struct Cyc_Absyn_Vardecl*
_tmpA2C;struct Cyc_Absyn_Fndecl*_tmpA2E;struct Cyc_Absyn_Aggrdecl*_tmpA34;struct
Cyc_Absyn_Datatypedecl*_tmpA36;struct Cyc_Absyn_Enumdecl*_tmpA38;struct Cyc_Absyn_Typedefdecl*
_tmpA3A;struct Cyc_List_List*_tmpA3E;struct Cyc_List_List*_tmpA40;struct Cyc_List_List*
_tmpA42;struct Cyc_List_List*_tmpA44;_LL450: {struct Cyc_Absyn_Var_d_struct*
_tmpA2B=(struct Cyc_Absyn_Var_d_struct*)_tmpA2A;if(_tmpA2B->tag != 0)goto _LL452;
else{_tmpA2C=_tmpA2B->f1;}}_LL451: {struct _tuple0*_tmpA45=_tmpA2C->name;if(
_tmpA2C->sc == Cyc_Absyn_ExternC){struct _tuple0*_tmpE28;_tmpA45=((_tmpE28=
_cycalloc(sizeof(*_tmpE28)),((_tmpE28->f1=Cyc_Absyn_Rel_n(0),((_tmpE28->f2=(*
_tmpA45).f2,_tmpE28))))));}if(_tmpA2C->initializer != 0){if(cinclude)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmpA2C->initializer));else{Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_check_null(_tmpA2C->initializer));}}{struct Cyc_Absyn_Global_b_struct
_tmpE2B;struct Cyc_Absyn_Global_b_struct*_tmpE2A;nv=Cyc_Toc_add_varmap(r,nv,
_tmpA2C->name,Cyc_Absyn_varb_exp(_tmpA45,(void*)((_tmpE2A=_cycalloc(sizeof(*
_tmpE2A)),((_tmpE2A[0]=((_tmpE2B.tag=1,((_tmpE2B.f1=_tmpA2C,_tmpE2B)))),_tmpE2A)))),
0));}_tmpA2C->name=_tmpA45;_tmpA2C->sc=Cyc_Toc_scope_to_c(_tmpA2C->sc);_tmpA2C->type=
Cyc_Toc_typ_to_c_array(_tmpA2C->type);{struct Cyc_List_List*_tmpE2C;Cyc_Toc_result_decls=((
_tmpE2C=_cycalloc(sizeof(*_tmpE2C)),((_tmpE2C->hd=d,((_tmpE2C->tl=Cyc_Toc_result_decls,
_tmpE2C))))));}goto _LL44F;}_LL452: {struct Cyc_Absyn_Fn_d_struct*_tmpA2D=(struct
Cyc_Absyn_Fn_d_struct*)_tmpA2A;if(_tmpA2D->tag != 1)goto _LL454;else{_tmpA2E=
_tmpA2D->f1;}}_LL453: {struct _tuple0*_tmpA4A=_tmpA2E->name;if(_tmpA2E->sc == Cyc_Absyn_ExternC){{
struct _tuple0*_tmpE2D;_tmpA4A=((_tmpE2D=_cycalloc(sizeof(*_tmpE2D)),((_tmpE2D->f1=
Cyc_Absyn_Rel_n(0),((_tmpE2D->f2=(*_tmpA4A).f2,_tmpE2D))))));}_tmpA2E->sc=Cyc_Absyn_Public;}
nv=Cyc_Toc_add_varmap(r,nv,_tmpA2E->name,Cyc_Absyn_var_exp(_tmpA4A,0));_tmpA2E->name=
_tmpA4A;Cyc_Toc_fndecl_to_c(nv,_tmpA2E,cinclude);{struct Cyc_List_List*_tmpE2E;
Cyc_Toc_result_decls=((_tmpE2E=_cycalloc(sizeof(*_tmpE2E)),((_tmpE2E->hd=d,((
_tmpE2E->tl=Cyc_Toc_result_decls,_tmpE2E))))));}goto _LL44F;}_LL454: {struct Cyc_Absyn_Let_d_struct*
_tmpA2F=(struct Cyc_Absyn_Let_d_struct*)_tmpA2A;if(_tmpA2F->tag != 2)goto _LL456;}
_LL455: goto _LL457;_LL456: {struct Cyc_Absyn_Letv_d_struct*_tmpA30=(struct Cyc_Absyn_Letv_d_struct*)
_tmpA2A;if(_tmpA30->tag != 3)goto _LL458;}_LL457: {const char*_tmpE31;void*_tmpE30;(
_tmpE30=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE31="letdecl at toplevel",_tag_dyneither(_tmpE31,sizeof(char),20))),
_tag_dyneither(_tmpE30,sizeof(void*),0)));}_LL458: {struct Cyc_Absyn_Region_d_struct*
_tmpA31=(struct Cyc_Absyn_Region_d_struct*)_tmpA2A;if(_tmpA31->tag != 4)goto _LL45A;}
_LL459: {const char*_tmpE34;void*_tmpE33;(_tmpE33=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE34="region decl at toplevel",
_tag_dyneither(_tmpE34,sizeof(char),24))),_tag_dyneither(_tmpE33,sizeof(void*),0)));}
_LL45A: {struct Cyc_Absyn_Alias_d_struct*_tmpA32=(struct Cyc_Absyn_Alias_d_struct*)
_tmpA2A;if(_tmpA32->tag != 5)goto _LL45C;}_LL45B: {const char*_tmpE37;void*_tmpE36;(
_tmpE36=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE37="alias decl at toplevel",_tag_dyneither(_tmpE37,sizeof(char),23))),
_tag_dyneither(_tmpE36,sizeof(void*),0)));}_LL45C: {struct Cyc_Absyn_Aggr_d_struct*
_tmpA33=(struct Cyc_Absyn_Aggr_d_struct*)_tmpA2A;if(_tmpA33->tag != 6)goto _LL45E;
else{_tmpA34=_tmpA33->f1;}}_LL45D: Cyc_Toc_aggrdecl_to_c(_tmpA34,1);goto _LL44F;
_LL45E: {struct Cyc_Absyn_Datatype_d_struct*_tmpA35=(struct Cyc_Absyn_Datatype_d_struct*)
_tmpA2A;if(_tmpA35->tag != 7)goto _LL460;else{_tmpA36=_tmpA35->f1;}}_LL45F: if(
_tmpA36->is_extensible)Cyc_Toc_xdatatypedecl_to_c(_tmpA36);else{Cyc_Toc_datatypedecl_to_c(
_tmpA36);}goto _LL44F;_LL460: {struct Cyc_Absyn_Enum_d_struct*_tmpA37=(struct Cyc_Absyn_Enum_d_struct*)
_tmpA2A;if(_tmpA37->tag != 8)goto _LL462;else{_tmpA38=_tmpA37->f1;}}_LL461: Cyc_Toc_enumdecl_to_c(
_tmpA38);{struct Cyc_List_List*_tmpE38;Cyc_Toc_result_decls=((_tmpE38=_cycalloc(
sizeof(*_tmpE38)),((_tmpE38->hd=d,((_tmpE38->tl=Cyc_Toc_result_decls,_tmpE38))))));}
goto _LL44F;_LL462: {struct Cyc_Absyn_Typedef_d_struct*_tmpA39=(struct Cyc_Absyn_Typedef_d_struct*)
_tmpA2A;if(_tmpA39->tag != 9)goto _LL464;else{_tmpA3A=_tmpA39->f1;}}_LL463: _tmpA3A->name=
_tmpA3A->name;_tmpA3A->tvs=0;if(_tmpA3A->defn != 0){struct Cyc_Core_Opt*_tmpE39;
_tmpA3A->defn=((_tmpE39=_cycalloc(sizeof(*_tmpE39)),((_tmpE39->v=Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA3A->defn))->v),_tmpE39))));}else{
switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmpA3A->kind))->v)->kind){
case Cyc_Absyn_BoxKind: _LL470:{struct Cyc_Core_Opt*_tmpE3A;_tmpA3A->defn=((_tmpE3A=
_cycalloc(sizeof(*_tmpE3A)),((_tmpE3A->v=Cyc_Absyn_void_star_typ(),_tmpE3A))));}
break;default: _LL471:{struct Cyc_Core_Opt*_tmpE3B;_tmpA3A->defn=((_tmpE3B=
_cycalloc(sizeof(*_tmpE3B)),((_tmpE3B->v=(void*)& Cyc_Absyn_VoidType_val,_tmpE3B))));}
break;}}{struct Cyc_List_List*_tmpE3C;Cyc_Toc_result_decls=((_tmpE3C=_cycalloc(
sizeof(*_tmpE3C)),((_tmpE3C->hd=d,((_tmpE3C->tl=Cyc_Toc_result_decls,_tmpE3C))))));}
goto _LL44F;_LL464: {struct Cyc_Absyn_Porton_d_struct*_tmpA3B=(struct Cyc_Absyn_Porton_d_struct*)
_tmpA2A;if(_tmpA3B->tag != 14)goto _LL466;}_LL465: goto _LL467;_LL466: {struct Cyc_Absyn_Portoff_d_struct*
_tmpA3C=(struct Cyc_Absyn_Portoff_d_struct*)_tmpA2A;if(_tmpA3C->tag != 15)goto
_LL468;}_LL467: goto _LL44F;_LL468: {struct Cyc_Absyn_Namespace_d_struct*_tmpA3D=(
struct Cyc_Absyn_Namespace_d_struct*)_tmpA2A;if(_tmpA3D->tag != 10)goto _LL46A;
else{_tmpA3E=_tmpA3D->f2;}}_LL469: _tmpA40=_tmpA3E;goto _LL46B;_LL46A: {struct Cyc_Absyn_Using_d_struct*
_tmpA3F=(struct Cyc_Absyn_Using_d_struct*)_tmpA2A;if(_tmpA3F->tag != 11)goto _LL46C;
else{_tmpA40=_tmpA3F->f2;}}_LL46B: _tmpA42=_tmpA40;goto _LL46D;_LL46C: {struct Cyc_Absyn_ExternC_d_struct*
_tmpA41=(struct Cyc_Absyn_ExternC_d_struct*)_tmpA2A;if(_tmpA41->tag != 12)goto
_LL46E;else{_tmpA42=_tmpA41->f1;}}_LL46D: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA42,top,
cinclude);goto _LL44F;_LL46E: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA43=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmpA2A;if(_tmpA43->tag != 13)goto _LL44F;
else{_tmpA44=_tmpA43->f1;}}_LL46F: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA44,top,1);goto
_LL44F;_LL44F:;};}return nv;}static void Cyc_Toc_init(){struct _DynRegionHandle*
_tmpA59;struct Cyc_Core_NewRegion _tmpA58=Cyc_Core__new_dynregion((const char*)"internal-error",
0);_tmpA59=_tmpA58.dynregion;{struct _DynRegionFrame _tmpA5A;struct _RegionHandle*d=
_open_dynregion(& _tmpA5A,_tmpA59);{struct Cyc_Dict_Dict*_tmpE47;struct Cyc_Dict_Dict*
_tmpE46;struct Cyc_Set_Set**_tmpE45;struct Cyc_Dict_Dict*_tmpE44;struct Cyc_List_List**
_tmpE43;struct Cyc_Toc_TocState*_tmpE42;Cyc_Toc_toc_state=((_tmpE42=_cycalloc(
sizeof(*_tmpE42)),((_tmpE42->dyn=(struct _DynRegionHandle*)_tmpA59,((_tmpE42->tuple_types=(
struct Cyc_List_List**)((_tmpE43=_region_malloc(d,sizeof(*_tmpE43)),((_tmpE43[0]=
0,_tmpE43)))),((_tmpE42->aggrs_so_far=(struct Cyc_Dict_Dict*)((_tmpE44=
_region_malloc(d,sizeof(*_tmpE44)),((_tmpE44[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE44)))),((_tmpE42->datatypes_so_far=(struct Cyc_Set_Set**)((_tmpE45=
_region_malloc(d,sizeof(*_tmpE45)),((_tmpE45[0]=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE45)))),((_tmpE42->xdatatypes_so_far=(struct Cyc_Dict_Dict*)((_tmpE46=
_region_malloc(d,sizeof(*_tmpE46)),((_tmpE46[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE46)))),((_tmpE42->qvar_tags=(struct Cyc_Dict_Dict*)((_tmpE47=_region_malloc(
d,sizeof(*_tmpE47)),((_tmpE47[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,
int(*cmp)(struct _tuple13*,struct _tuple13*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),
_tmpE47)))),((_tmpE42->temp_labels=(struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(
struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpE42))))))))))))))));}Cyc_Toc_result_decls=
0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=
0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=0;{struct
_dyneither_ptr**_tmpE48;Cyc_Toc_globals=_tag_dyneither(((_tmpE48=_cycalloc(
sizeof(struct _dyneither_ptr*)* 41),((_tmpE48[0]=& Cyc_Toc__throw_str,((_tmpE48[1]=&
Cyc_Toc_setjmp_str,((_tmpE48[2]=& Cyc_Toc__push_handler_str,((_tmpE48[3]=& Cyc_Toc__pop_handler_str,((
_tmpE48[4]=& Cyc_Toc__exn_thrown_str,((_tmpE48[5]=& Cyc_Toc__npop_handler_str,((
_tmpE48[6]=& Cyc_Toc__check_null_str,((_tmpE48[7]=& Cyc_Toc__check_known_subscript_null_str,((
_tmpE48[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpE48[9]=& Cyc_Toc__check_dyneither_subscript_str,((
_tmpE48[10]=& Cyc_Toc__dyneither_ptr_str,((_tmpE48[11]=& Cyc_Toc__tag_dyneither_str,((
_tmpE48[12]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpE48[13]=& Cyc_Toc__untag_dyneither_ptr_str,((
_tmpE48[14]=& Cyc_Toc__get_dyneither_size_str,((_tmpE48[15]=& Cyc_Toc__get_zero_arr_size_str,((
_tmpE48[16]=& Cyc_Toc__dyneither_ptr_plus_str,((_tmpE48[17]=& Cyc_Toc__zero_arr_plus_str,((
_tmpE48[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpE48[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((
_tmpE48[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpE48[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((
_tmpE48[22]=& Cyc_Toc__cycalloc_str,((_tmpE48[23]=& Cyc_Toc__cyccalloc_str,((
_tmpE48[24]=& Cyc_Toc__cycalloc_atomic_str,((_tmpE48[25]=& Cyc_Toc__cyccalloc_atomic_str,((
_tmpE48[26]=& Cyc_Toc__region_malloc_str,((_tmpE48[27]=& Cyc_Toc__region_calloc_str,((
_tmpE48[28]=& Cyc_Toc__check_times_str,((_tmpE48[29]=& Cyc_Toc__new_region_str,((
_tmpE48[30]=& Cyc_Toc__push_region_str,((_tmpE48[31]=& Cyc_Toc__pop_region_str,((
_tmpE48[32]=& Cyc_Toc__open_dynregion_str,((_tmpE48[33]=& Cyc_Toc__push_dynregion_str,((
_tmpE48[34]=& Cyc_Toc__pop_dynregion_str,((_tmpE48[35]=& Cyc_Toc__reset_region_str,((
_tmpE48[36]=& Cyc_Toc__throw_arraybounds_str,((_tmpE48[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,((
_tmpE48[38]=& Cyc_Toc__throw_match_str,((_tmpE48[39]=& Cyc_Toc__swap_word_str,((
_tmpE48[40]=& Cyc_Toc__swap_dyneither_str,_tmpE48)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),41);};;_pop_dynregion(d);};}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();{struct _RegionHandle _tmpA62=
_new_region("start");struct _RegionHandle*start=& _tmpA62;_push_region(start);Cyc_Toc_decls_to_c(
start,Cyc_Toc_empty_env(start),ds,1,0);{struct _DynRegionHandle*_tmpA64;struct Cyc_Toc_TocState
_tmpA63=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA64=
_tmpA63.dyn;Cyc_Core_free_dynregion(_tmpA64);};{struct Cyc_List_List*_tmpA65=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);
_npop_handler(0);return _tmpA65;};;_pop_region(start);};}
