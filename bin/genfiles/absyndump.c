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
struct Cyc_Null_Exception_exn_struct { char *tag; };
struct Cyc_Array_bounds_exn_struct { char *tag; };
struct Cyc_Match_Exception_exn_struct { char *tag; };
struct Cyc_Bad_alloc_exn_struct { char *tag; };
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
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_String_pa_PrintArg_struct{int tag;
struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};
struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};typedef void*Cyc_parg_t;
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);int
Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);int
Cyc_fputc(int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;
short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{
int tag;struct _dyneither_ptr f1;};typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[
15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[
14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};int Cyc_file_string_write(
struct Cyc___cycFILE*,struct _dyneither_ptr src,int src_offset,int max_count);typedef
unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_unique_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*
tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{
char*tag;};inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,
unsigned int sz){if(bd >> 20  || sz >> 12)return 0;{unsigned char*ptrbd=dyn.curr + bd * 
sz;if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
return 0;return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
typedef struct Cyc_List_List*Cyc_List_list_t;typedef struct Cyc_List_List*Cyc_List_List_t;
int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*
f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{
char*tag;};struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
4];struct Cyc_List_Nth_exn_struct{char*tag;};int Cyc_List_exists(int(*pred)(void*),
struct Cyc_List_List*x);unsigned long Cyc_strlen(struct _dyneither_ptr s);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];
struct Cyc_Position_Exit_exn_struct{char*tag;};typedef unsigned int Cyc_Position_seg_t;
struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{
int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};typedef void*
Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;
unsigned int seg;void*kind;struct _dyneither_ptr desc;};typedef struct Cyc_Position_Error*
Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{
char*tag;};struct _dyneither_ptr Cyc_Position_get_line_directive(unsigned int loc);
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
Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*
f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int
identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int
zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;
union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*
ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct
Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{
int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};
struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{
int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};
struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{
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
struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*
f1;};typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;
unsigned int loc;};typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct
Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int
tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo
f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};
struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;
struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;
struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{
int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;
void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{
int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{
int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct
Cyc_List_List*f5;};typedef void*Cyc_Absyn_funcparams_t;enum Cyc_Absyn_Format_Type{
Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{
int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};
struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};
struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct
Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type
f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;
int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};
struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{
int tag;struct _dyneither_ptr f1;};extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct
Cyc_Absyn_Stdcall_att_val;extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct
Cyc_Absyn_Cdecl_att_val;extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct
Cyc_Absyn_Fastcall_att_val;extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct
Cyc_Absyn_Unused_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{
int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};
struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts
f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int
tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{
int tag;unsigned int f1;struct Cyc_List_List*f2;};typedef void*Cyc_Absyn_type_modifier_t;
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
_union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};enum Cyc_Absyn_Primop{
Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod
 = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte
 = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand
 = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift
 = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{
Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec
 = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*
injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{
int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{
int tag;unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,
Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion
 = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**
elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;
struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;
enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;
enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*
f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;
void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{
int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*
f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct
_tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;
int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};
struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{
int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;
};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{
int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct
_dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*
annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int
tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};
struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{
int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*
f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int
tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;
struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{
int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int
tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int
tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;
int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*
f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{
int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;
int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{
int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{
int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int
tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct
Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};
struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int
loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope
sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;
void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*
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
kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;
struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;
};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*
f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*
f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{
int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{
void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{
int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{
char*tag;};struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_bounds_one;
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind
f1;struct _tuple0*f2;};struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
struct Cyc_PP_Ppstate;typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int
add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;
int print_all_kinds;int print_all_effects;int print_using_stmts;int
print_externC_stmts;int print_full_evars;int print_zeroterm;int
generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern
struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);struct _dyneither_ptr Cyc_Absynpp_string_escape(
struct _dyneither_ptr);struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop
p);int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple8*Cyc_Absynpp_arg_mk_opt(
struct _tuple8*arg);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*
f3;};struct _tuple11 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual
tq,void*t);struct _tuple12{unsigned int f1;int f2;};struct _tuple12 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[
7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;typedef const
struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;const struct Cyc_Dict_T*t;};typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;
extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct
Cyc_RgnOrder_RgnPO;typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;struct
Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*
tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct
_RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct
Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int Cyc_RgnOrder_is_region_resetable(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);int Cyc_RgnOrder_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);int Cyc_RgnOrder_satisfies_constraints(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,
int do_pin);int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*
eff1,void*eff2);void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);
struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};typedef const struct
Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct
Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;
struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};typedef void*Cyc_Tcenv_resolved_t;
struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Set_Set*namespaces;
struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict
enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*
availables;};typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct{
int tag;};struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct{
int tag;};struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};typedef struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);void Cyc_Tcutil_check_no_qual(
unsigned int loc,void*t);enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c
 = 1};extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;static int Cyc_Absyndump_expand_typedefs;
static int Cyc_Absyndump_qvar_to_Cids;static int Cyc_Absyndump_add_cyc_prefix;static
int Cyc_Absyndump_generate_line_directives;static int Cyc_Absyndump_to_VC;void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params*fs){Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;
Cyc_Absyndump_qvar_to_Cids=fs->qvar_to_Cids;Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absyndump_to_VC=fs->to_VC;Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_set_params(fs);}void Cyc_Absyndump_dumptyp(void*);void Cyc_Absyndump_dumpntyp(
void*t);void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumpexp_prec(
int,struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*);void
Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*,int expstmt);void Cyc_Absyndump_dumpvardecl(
struct Cyc_Absyn_Vardecl*,unsigned int);void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*);
void Cyc_Absyndump_dumptms(struct Cyc_List_List*tms,void(*f)(void*),void*a);void
Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(void*),void*);void
Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields);void Cyc_Absyndump_dumpenumfields(
struct Cyc_List_List*fields);typedef struct _dyneither_ptr Cyc_Absyndump_dump_string_t;
void Cyc_Absyndump_dumploc(unsigned int);struct Cyc___cycFILE**Cyc_Absyndump_dump_file=&
Cyc_stdout;void Cyc_Absyndump_ignore(void*x){return;}static unsigned int Cyc_Absyndump_pos=
0;static char Cyc_Absyndump_prev_char='x';int Cyc_Absyndump_need_space_before(){
switch(Cyc_Absyndump_prev_char){case '{': _LL0: goto _LL1;case '}': _LL1: goto _LL2;case
'(': _LL2: goto _LL3;case ')': _LL3: goto _LL4;case '[': _LL4: goto _LL5;case ']': _LL5: goto
_LL6;case ';': _LL6: goto _LL7;case ',': _LL7: goto _LL8;case '=': _LL8: goto _LL9;case '?':
_LL9: goto _LLA;case '!': _LLA: goto _LLB;case ' ': _LLB: goto _LLC;case '\n': _LLC: goto _LLD;
case '*': _LLD: return 0;default: _LLE: return 1;}}void Cyc_Absyndump_dump(struct
_dyneither_ptr s){int sz=(int)Cyc_strlen((struct _dyneither_ptr)s);Cyc_Absyndump_pos
+=sz;if(Cyc_Absyndump_pos > 80){Cyc_Absyndump_pos=(unsigned int)sz;Cyc_fputc((int)'\n',*
Cyc_Absyndump_dump_file);}else{if(Cyc_Absyndump_need_space_before())Cyc_fputc((
int)' ',*Cyc_Absyndump_dump_file);}if(sz >= 1){Cyc_Absyndump_prev_char=*((const
char*)_check_dyneither_subscript(s,sizeof(char),sz - 1));Cyc_file_string_write(*
Cyc_Absyndump_dump_file,s,0,sz);}}void Cyc_Absyndump_dump_nospace(struct
_dyneither_ptr s){int sz=(int)Cyc_strlen((struct _dyneither_ptr)s);Cyc_Absyndump_pos
+=sz;if(sz >= 1){Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);Cyc_Absyndump_prev_char=*((
const char*)_check_dyneither_subscript(s,sizeof(char),sz - 1));}}void Cyc_Absyndump_dump_char(
int c){++ Cyc_Absyndump_pos;Cyc_fputc(c,*Cyc_Absyndump_dump_file);Cyc_Absyndump_prev_char=(
char)c;}void Cyc_Absyndump_dumploc(unsigned int loc){if(loc == 0)return;if(!Cyc_Absyndump_generate_line_directives)
return;{struct _dyneither_ptr _tmp0=Cyc_Position_get_line_directive(loc);Cyc_Absyndump_dump(
_tmp0);};}void Cyc_Absyndump_dump_str(struct _dyneither_ptr*s){Cyc_Absyndump_dump(*
s);}void Cyc_Absyndump_dump_semi(){Cyc_Absyndump_dump_char((int)';');}void Cyc_Absyndump_dump_sep(
void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr sep){if(l == 0)return;
for(0;l->tl != 0;l=l->tl){f((void*)l->hd);Cyc_Absyndump_dump_nospace(sep);}f((
void*)l->hd);}void Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct
Cyc_List_List*l,struct _dyneither_ptr sep){if(l == 0)return;for(0;l->tl != 0;l=l->tl){
f(env,(void*)l->hd);Cyc_Absyndump_dump_nospace(sep);}f(env,(void*)l->hd);}void
Cyc_Absyndump_group(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr
start,struct _dyneither_ptr end,struct _dyneither_ptr sep){Cyc_Absyndump_dump_nospace(
start);Cyc_Absyndump_dump_sep(f,l,sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_group_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr start,
struct _dyneither_ptr end,struct _dyneither_ptr sep){Cyc_Absyndump_dump_nospace(
start);Cyc_Absyndump_dump_sep_c(f,env,l,sep);Cyc_Absyndump_dump_nospace(end);}
void Cyc_Absyndump_egroup(void(*f)(void*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){if(l != 0)
Cyc_Absyndump_group(f,l,start,end,sep);}void Cyc_Absyndump_dumpqvar(struct _tuple0*
v){struct Cyc_List_List*_tmp1=0;struct _dyneither_ptr**prefix=0;{union Cyc_Absyn_Nmspace
_tmp2=(*v).f1;int _tmp3;struct Cyc_List_List*_tmp4;struct Cyc_List_List*_tmp5;
struct Cyc_List_List*_tmp6;_LL11: if((_tmp2.Loc_n).tag != 4)goto _LL13;_tmp3=(int)(
_tmp2.Loc_n).val;_LL12: _tmp4=0;goto _LL14;_LL13: if((_tmp2.Rel_n).tag != 1)goto
_LL15;_tmp4=(struct Cyc_List_List*)(_tmp2.Rel_n).val;_LL14: _tmp1=_tmp4;goto _LL10;
_LL15: if((_tmp2.C_n).tag != 3)goto _LL17;_tmp5=(struct Cyc_List_List*)(_tmp2.C_n).val;
_LL16: Cyc_Absyndump_dump_char((int)' ');goto _LL10;_LL17: if((_tmp2.Abs_n).tag != 2)
goto _LL10;_tmp6=(struct Cyc_List_List*)(_tmp2.Abs_n).val;_LL18: if(Cyc_Absyndump_qvar_to_Cids
 && Cyc_Absyndump_add_cyc_prefix)prefix=(struct _dyneither_ptr**)& Cyc_Absynpp_cyc_stringptr;
_tmp1=_tmp6;goto _LL10;_LL10:;}if(prefix != 0){Cyc_Absyndump_dump_str(*prefix);if(
Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{const char*
_tmp3C3;Cyc_Absyndump_dump_nospace(((_tmp3C3="::",_tag_dyneither(_tmp3C3,sizeof(
char),3))));}}if(_tmp1 != 0){Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)
_tmp1->hd));for(_tmp1=_tmp1->tl;_tmp1 != 0;_tmp1=_tmp1->tl){if(Cyc_Absyndump_qvar_to_Cids)
Cyc_Absyndump_dump_char((int)'_');else{const char*_tmp3C4;Cyc_Absyndump_dump_nospace(((
_tmp3C4="::",_tag_dyneither(_tmp3C4,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(*((
struct _dyneither_ptr*)_tmp1->hd));}if(Cyc_Absyndump_qvar_to_Cids){const char*
_tmp3C5;Cyc_Absyndump_dump_nospace(((_tmp3C5="_",_tag_dyneither(_tmp3C5,sizeof(
char),2))));}else{const char*_tmp3C6;Cyc_Absyndump_dump_nospace(((_tmp3C6="::",
_tag_dyneither(_tmp3C6,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(*(*v).f2);}
else{if(prefix != 0)Cyc_Absyndump_dump_nospace(*(*v).f2);else{Cyc_Absyndump_dump_str((*
v).f2);}}}void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){if(tq.q_restrict){
const char*_tmp3C7;Cyc_Absyndump_dump(((_tmp3C7="restrict",_tag_dyneither(_tmp3C7,
sizeof(char),9))));}if(tq.q_volatile){const char*_tmp3C8;Cyc_Absyndump_dump(((
_tmp3C8="volatile",_tag_dyneither(_tmp3C8,sizeof(char),9))));}if(tq.print_const){
const char*_tmp3C9;Cyc_Absyndump_dump(((_tmp3C9="const",_tag_dyneither(_tmp3C9,
sizeof(char),6))));}}void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope sc){switch(
sc){case Cyc_Absyn_Static: _LL19:{const char*_tmp3CA;Cyc_Absyndump_dump(((_tmp3CA="static",
_tag_dyneither(_tmp3CA,sizeof(char),7))));}return;case Cyc_Absyn_Public: _LL1A:
return;case Cyc_Absyn_Extern: _LL1B:{const char*_tmp3CB;Cyc_Absyndump_dump(((
_tmp3CB="extern",_tag_dyneither(_tmp3CB,sizeof(char),7))));}return;case Cyc_Absyn_ExternC:
_LL1C:{const char*_tmp3CC;Cyc_Absyndump_dump(((_tmp3CC="extern \"C\"",
_tag_dyneither(_tmp3CC,sizeof(char),11))));}return;case Cyc_Absyn_Abstract: _LL1D:{
const char*_tmp3CD;Cyc_Absyndump_dump(((_tmp3CD="abstract",_tag_dyneither(_tmp3CD,
sizeof(char),9))));}return;case Cyc_Absyn_Register: _LL1E:{const char*_tmp3CE;Cyc_Absyndump_dump(((
_tmp3CE="register",_tag_dyneither(_tmp3CE,sizeof(char),9))));}return;}}void Cyc_Absyndump_dumpkind(
struct Cyc_Absyn_Kind*ka){struct Cyc_Absyn_Kind _tmp14;enum Cyc_Absyn_KindQual
_tmp15;enum Cyc_Absyn_AliasQual _tmp16;struct Cyc_Absyn_Kind*_tmp13=ka;_tmp14=*
_tmp13;_tmp15=_tmp14.kind;_tmp16=_tmp14.aliasqual;switch(_tmp15){case Cyc_Absyn_AnyKind:
_LL20: switch(_tmp16){case Cyc_Absyn_Aliasable: _LL22:{const char*_tmp3CF;Cyc_Absyndump_dump(((
_tmp3CF="A",_tag_dyneither(_tmp3CF,sizeof(char),2))));}return;case Cyc_Absyn_Unique:
_LL23:{const char*_tmp3D0;Cyc_Absyndump_dump(((_tmp3D0="UA",_tag_dyneither(
_tmp3D0,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL24:{const char*_tmp3D1;
Cyc_Absyndump_dump(((_tmp3D1="TA",_tag_dyneither(_tmp3D1,sizeof(char),3))));}
return;}case Cyc_Absyn_MemKind: _LL21: switch(_tmp16){case Cyc_Absyn_Aliasable: _LL27:{
const char*_tmp3D2;Cyc_Absyndump_dump(((_tmp3D2="M",_tag_dyneither(_tmp3D2,
sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL28:{const char*_tmp3D3;Cyc_Absyndump_dump(((
_tmp3D3="UM",_tag_dyneither(_tmp3D3,sizeof(char),3))));}return;case Cyc_Absyn_Top:
_LL29:{const char*_tmp3D4;Cyc_Absyndump_dump(((_tmp3D4="TM",_tag_dyneither(
_tmp3D4,sizeof(char),3))));}return;}case Cyc_Absyn_BoxKind: _LL26: switch(_tmp16){
case Cyc_Absyn_Aliasable: _LL2C:{const char*_tmp3D5;Cyc_Absyndump_dump(((_tmp3D5="B",
_tag_dyneither(_tmp3D5,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL2D:{
const char*_tmp3D6;Cyc_Absyndump_dump(((_tmp3D6="UB",_tag_dyneither(_tmp3D6,
sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL2E:{const char*_tmp3D7;Cyc_Absyndump_dump(((
_tmp3D7="TB",_tag_dyneither(_tmp3D7,sizeof(char),3))));}return;}case Cyc_Absyn_RgnKind:
_LL2B: switch(_tmp16){case Cyc_Absyn_Aliasable: _LL31:{const char*_tmp3D8;Cyc_Absyndump_dump(((
_tmp3D8="R",_tag_dyneither(_tmp3D8,sizeof(char),2))));}return;case Cyc_Absyn_Unique:
_LL32:{const char*_tmp3D9;Cyc_Absyndump_dump(((_tmp3D9="UR",_tag_dyneither(
_tmp3D9,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL33:{const char*_tmp3DA;
Cyc_Absyndump_dump(((_tmp3DA="TR",_tag_dyneither(_tmp3DA,sizeof(char),3))));}
return;}case Cyc_Absyn_EffKind: _LL30:{const char*_tmp3DB;Cyc_Absyndump_dump(((
_tmp3DB="E",_tag_dyneither(_tmp3DB,sizeof(char),2))));}return;case Cyc_Absyn_IntKind:
_LL35:{const char*_tmp3DC;Cyc_Absyndump_dump(((_tmp3DC="I",_tag_dyneither(_tmp3DC,
sizeof(char),2))));}return;}}void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind
k){switch(k){case Cyc_Absyn_StructA: _LL37:{const char*_tmp3DD;Cyc_Absyndump_dump(((
_tmp3DD="struct",_tag_dyneither(_tmp3DD,sizeof(char),7))));}return;case Cyc_Absyn_UnionA:
_LL38:{const char*_tmp3DE;Cyc_Absyndump_dump(((_tmp3DE="union",_tag_dyneither(
_tmp3DE,sizeof(char),6))));}return;}}void Cyc_Absyndump_dumptps(struct Cyc_List_List*
ts){const char*_tmp3E1;const char*_tmp3E0;const char*_tmp3DF;((void(*)(void(*f)(
void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,
struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptyp,ts,((
_tmp3DF="<",_tag_dyneither(_tmp3DF,sizeof(char),2))),((_tmp3E0=">",
_tag_dyneither(_tmp3E0,sizeof(char),2))),((_tmp3E1=",",_tag_dyneither(_tmp3E1,
sizeof(char),2))));}void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(
tv->name);}void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(
tv->name);{void*_tmp2A=Cyc_Absyn_compress_kb(tv->kind);struct Cyc_Absyn_Kind*
_tmp2C;struct Cyc_Absyn_Kind _tmp2D;enum Cyc_Absyn_KindQual _tmp2E;enum Cyc_Absyn_AliasQual
_tmp2F;struct Cyc_Absyn_Kind*_tmp33;_LL3B: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*
_tmp2B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2A;if(_tmp2B->tag != 0)
goto _LL3D;else{_tmp2C=_tmp2B->f1;_tmp2D=*_tmp2C;_tmp2E=_tmp2D.kind;if(_tmp2E != 
Cyc_Absyn_BoxKind)goto _LL3D;_tmp2F=_tmp2D.aliasqual;if(_tmp2F != Cyc_Absyn_Aliasable)
goto _LL3D;}}_LL3C: goto _LL3E;_LL3D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmp30=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2A;if(_tmp30->tag != 
2)goto _LL3F;}_LL3E: goto _LL40;_LL3F: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*
_tmp31=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp2A;if(_tmp31->tag
!= 1)goto _LL41;}_LL40:{const char*_tmp3E2;Cyc_Absyndump_dump(((_tmp3E2="::?",
_tag_dyneither(_tmp3E2,sizeof(char),4))));}goto _LL3A;_LL41: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*
_tmp32=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2A;if(_tmp32->tag != 0)
goto _LL3A;else{_tmp33=_tmp32->f1;}}_LL42:{const char*_tmp3E3;Cyc_Absyndump_dump(((
_tmp3E3="::",_tag_dyneither(_tmp3E3,sizeof(char),3))));}Cyc_Absyndump_dumpkind(
_tmp33);goto _LL3A;_LL3A:;};}void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){
const char*_tmp3E6;const char*_tmp3E5;const char*_tmp3E4;((void(*)(void(*f)(struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,
tvs,((_tmp3E4="<",_tag_dyneither(_tmp3E4,sizeof(char),2))),((_tmp3E5=">",
_tag_dyneither(_tmp3E5,sizeof(char),2))),((_tmp3E6=",",_tag_dyneither(_tmp3E6,
sizeof(char),2))));}void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){
const char*_tmp3E9;const char*_tmp3E8;const char*_tmp3E7;((void(*)(void(*f)(struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,
tvs,((_tmp3E7="<",_tag_dyneither(_tmp3E7,sizeof(char),2))),((_tmp3E8=">",
_tag_dyneither(_tmp3E8,sizeof(char),2))),((_tmp3E9=",",_tag_dyneither(_tmp3E9,
sizeof(char),2))));}struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;};void Cyc_Absyndump_dumparg(
struct _tuple13*pr){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*
pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpargs(
struct Cyc_List_List*ts){const char*_tmp3EC;const char*_tmp3EB;const char*_tmp3EA;((
void(*)(void(*f)(struct _tuple13*),struct Cyc_List_List*l,struct _dyneither_ptr
start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,
ts,((_tmp3EA="(",_tag_dyneither(_tmp3EA,sizeof(char),2))),((_tmp3EB=")",
_tag_dyneither(_tmp3EB,sizeof(char),2))),((_tmp3EC=",",_tag_dyneither(_tmp3EC,
sizeof(char),2))));}void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp3F=(void*)atts->hd;_LL44: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*
_tmp40=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp3F;if(_tmp40->tag
!= 1)goto _LL46;}_LL45:{const char*_tmp3ED;Cyc_Absyndump_dump(((_tmp3ED="_stdcall",
_tag_dyneither(_tmp3ED,sizeof(char),9))));}return;_LL46: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*
_tmp41=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp3F;if(_tmp41->tag
!= 2)goto _LL48;}_LL47:{const char*_tmp3EE;Cyc_Absyndump_dump(((_tmp3EE="_cdecl",
_tag_dyneither(_tmp3EE,sizeof(char),7))));}return;_LL48: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*
_tmp42=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp3F;if(_tmp42->tag
!= 3)goto _LL4A;}_LL49:{const char*_tmp3EF;Cyc_Absyndump_dump(((_tmp3EF="_fastcall",
_tag_dyneither(_tmp3EF,sizeof(char),10))));}return;_LL4A:;_LL4B: goto _LL43;_LL43:;}}
void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){int hasatt=0;{struct
Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){void*_tmp46=(void*)((
struct Cyc_List_List*)_check_null(atts))->hd;_LL4D: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*
_tmp47=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp46;if(_tmp47->tag
!= 1)goto _LL4F;}_LL4E: goto _LL50;_LL4F: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*
_tmp48=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp46;if(_tmp48->tag
!= 2)goto _LL51;}_LL50: goto _LL52;_LL51: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*
_tmp49=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp46;if(_tmp49->tag
!= 3)goto _LL53;}_LL52: goto _LL4C;_LL53:;_LL54: hasatt=1;goto _LL4C;_LL4C:;}}if(!
hasatt)return;{const char*_tmp3F0;Cyc_Absyndump_dump(((_tmp3F0="__declspec(",
_tag_dyneither(_tmp3F0,sizeof(char),12))));}for(0;atts != 0;atts=atts->tl){void*
_tmp4B=(void*)atts->hd;_LL56: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*
_tmp4C=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp4B;if(_tmp4C->tag
!= 1)goto _LL58;}_LL57: goto _LL59;_LL58: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*
_tmp4D=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp4B;if(_tmp4D->tag
!= 2)goto _LL5A;}_LL59: goto _LL5B;_LL5A: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*
_tmp4E=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp4B;if(_tmp4E->tag
!= 3)goto _LL5C;}_LL5B: goto _LL55;_LL5C:;_LL5D: Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((
void*)atts->hd));goto _LL55;_LL55:;}Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpatts(
struct Cyc_List_List*atts){if(atts == 0)return;switch(Cyc_Cyclone_c_compiler){case
Cyc_Cyclone_Gcc_c: _LL5E:{const char*_tmp3F1;Cyc_Absyndump_dump(((_tmp3F1=" __attribute__((",
_tag_dyneither(_tmp3F1,sizeof(char),17))));}for(0;atts != 0;atts=atts->tl){Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string((void*)atts->hd));if(atts->tl != 0){const char*_tmp3F2;
Cyc_Absyndump_dump(((_tmp3F2=",",_tag_dyneither(_tmp3F2,sizeof(char),2))));}}{
const char*_tmp3F3;Cyc_Absyndump_dump(((_tmp3F3=")) ",_tag_dyneither(_tmp3F3,
sizeof(char),4))));}return;case Cyc_Cyclone_Vc_c: _LL5F: Cyc_Absyndump_dump_noncallconv(
atts);return;}}int Cyc_Absyndump_next_is_pointer(struct Cyc_List_List*tms){if(tms
== 0)return 0;{void*_tmp52=(void*)tms->hd;_LL62: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*
_tmp53=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp52;if(_tmp53->tag
!= 2)goto _LL64;}_LL63: return 1;_LL64:;_LL65: return 0;_LL61:;};}static void Cyc_Absyndump_dumprgn(
void*t){void*_tmp54=Cyc_Tcutil_compress(t);_LL67: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*
_tmp55=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp54;if(_tmp55->tag != 20)
goto _LL69;}_LL68:{const char*_tmp3F4;Cyc_Absyndump_dump(((_tmp3F4="`H",
_tag_dyneither(_tmp3F4,sizeof(char),3))));}goto _LL66;_LL69:;_LL6A: Cyc_Absyndump_dumpntyp(
t);goto _LL66;_LL66:;}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};static struct _tuple14 Cyc_Absyndump_effects_split(void*t){struct Cyc_List_List*
rgions=0;struct Cyc_List_List*effects=0;{void*_tmp57=Cyc_Tcutil_compress(t);void*
_tmp59;struct Cyc_List_List*_tmp5B;_LL6C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*
_tmp58=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp57;if(_tmp58->tag != 23)
goto _LL6E;else{_tmp59=(void*)_tmp58->f1;}}_LL6D:{struct Cyc_List_List*_tmp3F5;
rgions=((_tmp3F5=_cycalloc(sizeof(*_tmp3F5)),((_tmp3F5->hd=_tmp59,((_tmp3F5->tl=
rgions,_tmp3F5))))));}goto _LL6B;_LL6E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*
_tmp5A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp57;if(_tmp5A->tag != 24)
goto _LL70;else{_tmp5B=_tmp5A->f1;}}_LL6F: for(0;_tmp5B != 0;_tmp5B=_tmp5B->tl){
struct Cyc_List_List*_tmp5E;struct Cyc_List_List*_tmp5F;struct _tuple14 _tmp5D=Cyc_Absyndump_effects_split((
void*)_tmp5B->hd);_tmp5E=_tmp5D.f1;_tmp5F=_tmp5D.f2;rgions=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp5E,rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp5F,effects);}goto _LL6B;_LL70:;_LL71:{struct Cyc_List_List*
_tmp3F6;effects=((_tmp3F6=_cycalloc(sizeof(*_tmp3F6)),((_tmp3F6->hd=t,((_tmp3F6->tl=
effects,_tmp3F6))))));}goto _LL6B;_LL6B:;}{struct _tuple14 _tmp3F7;return(_tmp3F7.f1=
rgions,((_tmp3F7.f2=effects,_tmp3F7)));};}static void Cyc_Absyndump_dumpeff(void*t){
struct Cyc_List_List*_tmp63;struct Cyc_List_List*_tmp64;struct _tuple14 _tmp62=Cyc_Absyndump_effects_split(
t);_tmp63=_tmp62.f1;_tmp64=_tmp62.f2;_tmp63=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp63);_tmp64=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp64);for(0;_tmp64 != 0;_tmp64=_tmp64->tl){Cyc_Absyndump_dumpntyp((
void*)_tmp64->hd);Cyc_Absyndump_dump_char((int)'+');}Cyc_Absyndump_dump_char((
int)'{');for(0;_tmp63 != 0;_tmp63=_tmp63->tl){Cyc_Absyndump_dumprgn((void*)_tmp63->hd);
if(_tmp63->tl != 0)Cyc_Absyndump_dump_char((int)',');}Cyc_Absyndump_dump_char((
int)'}');}static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*);
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*);static void Cyc_Absyndump_dump_datatypedecl(
struct Cyc_Absyn_Datatypedecl*);void Cyc_Absyndump_dumpntyp(void*t){void*_tmp65=t;
struct Cyc_Absyn_Tvar*_tmp6B;struct Cyc_Core_Opt*_tmp6D;void*_tmp6E;int _tmp6F;
struct Cyc_Core_Opt*_tmp71;void*_tmp72;int _tmp73;struct Cyc_Absyn_DatatypeInfo
_tmp75;union Cyc_Absyn_DatatypeInfoU _tmp76;struct Cyc_List_List*_tmp77;struct Cyc_Absyn_DatatypeFieldInfo
_tmp79;union Cyc_Absyn_DatatypeFieldInfoU _tmp7A;struct Cyc_List_List*_tmp7B;enum 
Cyc_Absyn_Sign _tmp7D;enum Cyc_Absyn_Size_of _tmp7E;enum Cyc_Absyn_Sign _tmp80;
enum Cyc_Absyn_Size_of _tmp81;enum Cyc_Absyn_Sign _tmp83;enum Cyc_Absyn_Size_of
_tmp84;enum Cyc_Absyn_Sign _tmp86;enum Cyc_Absyn_Size_of _tmp87;enum Cyc_Absyn_Sign
_tmp89;enum Cyc_Absyn_Size_of _tmp8A;enum Cyc_Absyn_Sign _tmp8C;enum Cyc_Absyn_Size_of
_tmp8D;enum Cyc_Absyn_Sign _tmp8F;enum Cyc_Absyn_Size_of _tmp90;enum Cyc_Absyn_Sign
_tmp92;enum Cyc_Absyn_Size_of _tmp93;enum Cyc_Absyn_Sign _tmp95;enum Cyc_Absyn_Size_of
_tmp96;enum Cyc_Absyn_Sign _tmp98;enum Cyc_Absyn_Size_of _tmp99;enum Cyc_Absyn_Sign
_tmp9B;enum Cyc_Absyn_Size_of _tmp9C;enum Cyc_Absyn_Sign _tmp9E;enum Cyc_Absyn_Size_of
_tmp9F;enum Cyc_Absyn_Sign _tmpA1;enum Cyc_Absyn_Size_of _tmpA2;enum Cyc_Absyn_Sign
_tmpA4;enum Cyc_Absyn_Size_of _tmpA5;enum Cyc_Absyn_Sign _tmpA7;enum Cyc_Absyn_Size_of
_tmpA8;int _tmpAA;struct Cyc_List_List*_tmpAC;struct Cyc_Absyn_AggrInfo _tmpAE;union
Cyc_Absyn_AggrInfoU _tmpAF;struct Cyc_List_List*_tmpB0;enum Cyc_Absyn_AggrKind
_tmpB2;struct Cyc_List_List*_tmpB3;struct _tuple0*_tmpB5;struct Cyc_List_List*
_tmpB7;struct _tuple0*_tmpB9;struct Cyc_List_List*_tmpBA;struct Cyc_Absyn_Exp*
_tmpBC;void*_tmpBE;void*_tmpC0;void*_tmpC1;void*_tmpC3;struct Cyc_Absyn_TypeDecl*
_tmpC7;struct Cyc_Absyn_TypeDecl _tmpC8;void*_tmpC9;struct Cyc_Absyn_Aggrdecl*
_tmpCB;struct Cyc_Absyn_TypeDecl*_tmpCD;struct Cyc_Absyn_TypeDecl _tmpCE;void*
_tmpCF;struct Cyc_Absyn_Enumdecl*_tmpD1;struct Cyc_Absyn_TypeDecl*_tmpD3;struct Cyc_Absyn_TypeDecl
_tmpD4;void*_tmpD5;struct Cyc_Absyn_Datatypedecl*_tmpD7;_LL73: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp66=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp65;if(_tmp66->tag != 8)
goto _LL75;}_LL74: goto _LL76;_LL75: {struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp67=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65;if(_tmp67->tag != 9)goto
_LL77;}_LL76: goto _LL78;_LL77: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp68=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65;if(_tmp68->tag != 5)
goto _LL79;}_LL78: return;_LL79: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*
_tmp69=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp65;if(_tmp69->tag != 0)
goto _LL7B;}_LL7A:{const char*_tmp3F8;Cyc_Absyndump_dump(((_tmp3F8="void",
_tag_dyneither(_tmp3F8,sizeof(char),5))));}return;_LL7B: {struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp6A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp65;if(_tmp6A->tag != 2)
goto _LL7D;else{_tmp6B=_tmp6A->f1;}}_LL7C: Cyc_Absyndump_dump_str(_tmp6B->name);
return;_LL7D: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp6C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)
_tmp65;if(_tmp6C->tag != 1)goto _LL7F;else{_tmp6D=_tmp6C->f1;_tmp6E=(void*)_tmp6C->f2;
if(_tmp6E != 0)goto _LL7F;_tmp6F=_tmp6C->f3;}}_LL7E:{const char*_tmp3F9;Cyc_Absyndump_dump(((
_tmp3F9="%",_tag_dyneither(_tmp3F9,sizeof(char),2))));}if(_tmp6D == 0){const char*
_tmp3FA;Cyc_Absyndump_dump(((_tmp3FA="?",_tag_dyneither(_tmp3FA,sizeof(char),2))));}
else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)_tmp6D->v);}{const char*
_tmp3FE;void*_tmp3FD[1];struct Cyc_Int_pa_PrintArg_struct _tmp3FC;Cyc_Absyndump_dump((
struct _dyneither_ptr)((_tmp3FC.tag=1,((_tmp3FC.f1=(unsigned long)_tmp6F,((
_tmp3FD[0]=& _tmp3FC,Cyc_aprintf(((_tmp3FE="(%d)",_tag_dyneither(_tmp3FE,sizeof(
char),5))),_tag_dyneither(_tmp3FD,sizeof(void*),1)))))))));}return;_LL7F: {
struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)
_tmp65;if(_tmp70->tag != 1)goto _LL81;else{_tmp71=_tmp70->f1;_tmp72=(void*)_tmp70->f2;
_tmp73=_tmp70->f3;}}_LL80: Cyc_Absyndump_dumpntyp((void*)_check_null(_tmp72));
return;_LL81: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp74=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)
_tmp65;if(_tmp74->tag != 3)goto _LL83;else{_tmp75=_tmp74->f1;_tmp76=_tmp75.datatype_info;
_tmp77=_tmp75.targs;}}_LL82:{union Cyc_Absyn_DatatypeInfoU _tmpE2=_tmp76;struct Cyc_Absyn_UnknownDatatypeInfo
_tmpE3;struct _tuple0*_tmpE4;int _tmpE5;struct Cyc_Absyn_Datatypedecl**_tmpE6;
struct Cyc_Absyn_Datatypedecl*_tmpE7;struct Cyc_Absyn_Datatypedecl _tmpE8;struct
_tuple0*_tmpE9;int _tmpEA;_LLCC: if((_tmpE2.UnknownDatatype).tag != 1)goto _LLCE;
_tmpE3=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpE2.UnknownDatatype).val;_tmpE4=
_tmpE3.name;_tmpE5=_tmpE3.is_extensible;_LLCD: _tmpE9=_tmpE4;_tmpEA=_tmpE5;goto
_LLCF;_LLCE: if((_tmpE2.KnownDatatype).tag != 2)goto _LLCB;_tmpE6=(struct Cyc_Absyn_Datatypedecl**)(
_tmpE2.KnownDatatype).val;_tmpE7=*_tmpE6;_tmpE8=*_tmpE7;_tmpE9=_tmpE8.name;
_tmpEA=_tmpE8.is_extensible;_LLCF: if(_tmpEA){const char*_tmp3FF;Cyc_Absyndump_dump(((
_tmp3FF="@extensible ",_tag_dyneither(_tmp3FF,sizeof(char),13))));}{const char*
_tmp400;Cyc_Absyndump_dump(((_tmp400="datatype ",_tag_dyneither(_tmp400,sizeof(
char),10))));}Cyc_Absyndump_dumpqvar(_tmpE9);Cyc_Absyndump_dumptps(_tmp77);goto
_LLCB;_LLCB:;}goto _LL72;_LL83: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*
_tmp78=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp65;if(_tmp78->tag
!= 4)goto _LL85;else{_tmp79=_tmp78->f1;_tmp7A=_tmp79.field_info;_tmp7B=_tmp79.targs;}}
_LL84:{union Cyc_Absyn_DatatypeFieldInfoU _tmpED=_tmp7A;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpEE;struct _tuple0*_tmpEF;struct _tuple0*_tmpF0;int _tmpF1;struct _tuple1 _tmpF2;
struct Cyc_Absyn_Datatypedecl*_tmpF3;struct Cyc_Absyn_Datatypedecl _tmpF4;struct
_tuple0*_tmpF5;int _tmpF6;struct Cyc_Absyn_Datatypefield*_tmpF7;struct Cyc_Absyn_Datatypefield
_tmpF8;struct _tuple0*_tmpF9;_LLD1: if((_tmpED.UnknownDatatypefield).tag != 1)goto
_LLD3;_tmpEE=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpED.UnknownDatatypefield).val;
_tmpEF=_tmpEE.datatype_name;_tmpF0=_tmpEE.field_name;_tmpF1=_tmpEE.is_extensible;
_LLD2: _tmpF5=_tmpEF;_tmpF6=_tmpF1;_tmpF9=_tmpF0;goto _LLD4;_LLD3: if((_tmpED.KnownDatatypefield).tag
!= 2)goto _LLD0;_tmpF2=(struct _tuple1)(_tmpED.KnownDatatypefield).val;_tmpF3=
_tmpF2.f1;_tmpF4=*_tmpF3;_tmpF5=_tmpF4.name;_tmpF6=_tmpF4.is_extensible;_tmpF7=
_tmpF2.f2;_tmpF8=*_tmpF7;_tmpF9=_tmpF8.name;_LLD4: if(_tmpF6){const char*_tmp401;
Cyc_Absyndump_dump(((_tmp401="@extensible ",_tag_dyneither(_tmp401,sizeof(char),
13))));}{const char*_tmp402;Cyc_Absyndump_dump(((_tmp402="datatype ",
_tag_dyneither(_tmp402,sizeof(char),10))));}Cyc_Absyndump_dumpqvar(_tmpF5);{
const char*_tmp403;Cyc_Absyndump_dump(((_tmp403=".",_tag_dyneither(_tmp403,
sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmpF9);Cyc_Absyndump_dumptps(_tmp7B);
goto _LLD0;_LLD0:;}goto _LL72;_LL85: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp7C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp65;if(_tmp7C->tag != 6)
goto _LL87;else{_tmp7D=_tmp7C->f1;if(_tmp7D != Cyc_Absyn_None)goto _LL87;_tmp7E=
_tmp7C->f2;if(_tmp7E != Cyc_Absyn_Int_sz)goto _LL87;}}_LL86: goto _LL88;_LL87: {
struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp65;if(_tmp7F->tag != 6)goto _LL89;else{_tmp80=_tmp7F->f1;if(_tmp80 != Cyc_Absyn_Signed)
goto _LL89;_tmp81=_tmp7F->f2;if(_tmp81 != Cyc_Absyn_Int_sz)goto _LL89;}}_LL88:{
const char*_tmp404;Cyc_Absyndump_dump(((_tmp404="int",_tag_dyneither(_tmp404,
sizeof(char),4))));}return;_LL89: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp82=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp65;if(_tmp82->tag != 6)
goto _LL8B;else{_tmp83=_tmp82->f1;if(_tmp83 != Cyc_Absyn_None)goto _LL8B;_tmp84=
_tmp82->f2;if(_tmp84 != Cyc_Absyn_Long_sz)goto _LL8B;}}_LL8A: goto _LL8C;_LL8B: {
struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp85=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp65;if(_tmp85->tag != 6)goto _LL8D;else{_tmp86=_tmp85->f1;if(_tmp86 != Cyc_Absyn_Signed)
goto _LL8D;_tmp87=_tmp85->f2;if(_tmp87 != Cyc_Absyn_Long_sz)goto _LL8D;}}_LL8C:{
const char*_tmp405;Cyc_Absyndump_dump(((_tmp405="long",_tag_dyneither(_tmp405,
sizeof(char),5))));}return;_LL8D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp88=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp65;if(_tmp88->tag != 6)
goto _LL8F;else{_tmp89=_tmp88->f1;if(_tmp89 != Cyc_Absyn_None)goto _LL8F;_tmp8A=
_tmp88->f2;if(_tmp8A != Cyc_Absyn_Char_sz)goto _LL8F;}}_LL8E:{const char*_tmp406;
Cyc_Absyndump_dump(((_tmp406="char",_tag_dyneither(_tmp406,sizeof(char),5))));}
return;_LL8F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp65;if(_tmp8B->tag != 6)goto _LL91;else{_tmp8C=_tmp8B->f1;if(_tmp8C != Cyc_Absyn_Signed)
goto _LL91;_tmp8D=_tmp8B->f2;if(_tmp8D != Cyc_Absyn_Char_sz)goto _LL91;}}_LL90:{
const char*_tmp407;Cyc_Absyndump_dump(((_tmp407="signed char",_tag_dyneither(
_tmp407,sizeof(char),12))));}return;_LL91: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp8E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp65;if(_tmp8E->tag != 6)
goto _LL93;else{_tmp8F=_tmp8E->f1;if(_tmp8F != Cyc_Absyn_Unsigned)goto _LL93;_tmp90=
_tmp8E->f2;if(_tmp90 != Cyc_Absyn_Char_sz)goto _LL93;}}_LL92:{const char*_tmp408;
Cyc_Absyndump_dump(((_tmp408="unsigned char",_tag_dyneither(_tmp408,sizeof(char),
14))));}return;_LL93: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp91=(struct
Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp65;if(_tmp91->tag != 6)goto _LL95;else{
_tmp92=_tmp91->f1;if(_tmp92 != Cyc_Absyn_None)goto _LL95;_tmp93=_tmp91->f2;if(
_tmp93 != Cyc_Absyn_Short_sz)goto _LL95;}}_LL94: goto _LL96;_LL95: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp94=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp65;if(_tmp94->tag != 6)
goto _LL97;else{_tmp95=_tmp94->f1;if(_tmp95 != Cyc_Absyn_Signed)goto _LL97;_tmp96=
_tmp94->f2;if(_tmp96 != Cyc_Absyn_Short_sz)goto _LL97;}}_LL96:{const char*_tmp409;
Cyc_Absyndump_dump(((_tmp409="short",_tag_dyneither(_tmp409,sizeof(char),6))));}
return;_LL97: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp97=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp65;if(_tmp97->tag != 6)goto _LL99;else{_tmp98=_tmp97->f1;if(_tmp98 != Cyc_Absyn_Unsigned)
goto _LL99;_tmp99=_tmp97->f2;if(_tmp99 != Cyc_Absyn_Short_sz)goto _LL99;}}_LL98:{
const char*_tmp40A;Cyc_Absyndump_dump(((_tmp40A="unsigned short",_tag_dyneither(
_tmp40A,sizeof(char),15))));}return;_LL99: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp9A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp65;if(_tmp9A->tag != 6)
goto _LL9B;else{_tmp9B=_tmp9A->f1;if(_tmp9B != Cyc_Absyn_Unsigned)goto _LL9B;_tmp9C=
_tmp9A->f2;if(_tmp9C != Cyc_Absyn_Int_sz)goto _LL9B;}}_LL9A:{const char*_tmp40B;Cyc_Absyndump_dump(((
_tmp40B="unsigned int",_tag_dyneither(_tmp40B,sizeof(char),13))));}return;_LL9B: {
struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp65;if(_tmp9D->tag != 6)goto _LL9D;else{_tmp9E=_tmp9D->f1;if(_tmp9E != Cyc_Absyn_Unsigned)
goto _LL9D;_tmp9F=_tmp9D->f2;if(_tmp9F != Cyc_Absyn_Long_sz)goto _LL9D;}}_LL9C:{
const char*_tmp40C;Cyc_Absyndump_dump(((_tmp40C="unsigned long",_tag_dyneither(
_tmp40C,sizeof(char),14))));}return;_LL9D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmpA0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp65;if(_tmpA0->tag != 6)
goto _LL9F;else{_tmpA1=_tmpA0->f1;if(_tmpA1 != Cyc_Absyn_None)goto _LL9F;_tmpA2=
_tmpA0->f2;if(_tmpA2 != Cyc_Absyn_LongLong_sz)goto _LL9F;}}_LL9E: goto _LLA0;_LL9F: {
struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp65;if(_tmpA3->tag != 6)goto _LLA1;else{_tmpA4=_tmpA3->f1;if(_tmpA4 != Cyc_Absyn_Signed)
goto _LLA1;_tmpA5=_tmpA3->f2;if(_tmpA5 != Cyc_Absyn_LongLong_sz)goto _LLA1;}}_LLA0:
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LLD5:{const char*_tmp40D;
Cyc_Absyndump_dump(((_tmp40D="long long",_tag_dyneither(_tmp40D,sizeof(char),10))));}
return;case Cyc_Cyclone_Vc_c: _LLD6:{const char*_tmp40E;Cyc_Absyndump_dump(((
_tmp40E="__int64",_tag_dyneither(_tmp40E,sizeof(char),8))));}return;}_LLA1: {
struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp65;if(_tmpA6->tag != 6)goto _LLA3;else{_tmpA7=_tmpA6->f1;if(_tmpA7 != Cyc_Absyn_Unsigned)
goto _LLA3;_tmpA8=_tmpA6->f2;if(_tmpA8 != Cyc_Absyn_LongLong_sz)goto _LLA3;}}_LLA2:
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LLD8:{const char*_tmp40F;Cyc_Absyndump_dump(((
_tmp40F="unsigned __int64",_tag_dyneither(_tmp40F,sizeof(char),17))));}return;
case Cyc_Cyclone_Gcc_c: _LLD9:{const char*_tmp410;Cyc_Absyndump_dump(((_tmp410="unsigned long long",
_tag_dyneither(_tmp410,sizeof(char),19))));}return;}_LLA3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmpA9=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp65;if(_tmpA9->tag != 7)
goto _LLA5;else{_tmpAA=_tmpA9->f1;}}_LLA4: if(_tmpAA == 0){const char*_tmp411;Cyc_Absyndump_dump(((
_tmp411="float",_tag_dyneither(_tmp411,sizeof(char),6))));}else{if(_tmpAA == 1){
const char*_tmp412;Cyc_Absyndump_dump(((_tmp412="double",_tag_dyneither(_tmp412,
sizeof(char),7))));}else{const char*_tmp413;Cyc_Absyndump_dump(((_tmp413="long double",
_tag_dyneither(_tmp413,sizeof(char),12))));}}return;_LLA5: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmpAB=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp65;if(_tmpAB->tag != 10)
goto _LLA7;else{_tmpAC=_tmpAB->f1;}}_LLA6: Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(
_tmpAC);return;_LLA7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpAD=(struct
Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp65;if(_tmpAD->tag != 11)goto _LLA9;else{
_tmpAE=_tmpAD->f1;_tmpAF=_tmpAE.aggr_info;_tmpB0=_tmpAE.targs;}}_LLA8: {enum Cyc_Absyn_AggrKind
_tmp10E;struct _tuple0*_tmp10F;struct _tuple10 _tmp10D=Cyc_Absyn_aggr_kinded_name(
_tmpAF);_tmp10E=_tmp10D.f1;_tmp10F=_tmp10D.f2;Cyc_Absyndump_dumpaggr_kind(
_tmp10E);Cyc_Absyndump_dumpqvar(_tmp10F);Cyc_Absyndump_dumptps(_tmpB0);return;}
_LLA9: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB1=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)
_tmp65;if(_tmpB1->tag != 12)goto _LLAB;else{_tmpB2=_tmpB1->f1;_tmpB3=_tmpB1->f2;}}
_LLAA: Cyc_Absyndump_dumpaggr_kind(_tmpB2);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(
_tmpB3);Cyc_Absyndump_dump_char((int)'}');return;_LLAB: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmpB4=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp65;if(_tmpB4->tag != 13)
goto _LLAD;else{_tmpB5=_tmpB4->f1;}}_LLAC:{const char*_tmp414;Cyc_Absyndump_dump(((
_tmp414="enum ",_tag_dyneither(_tmp414,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(
_tmpB5);return;_LLAD: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpB6=(
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp65;if(_tmpB6->tag != 14)goto
_LLAF;else{_tmpB7=_tmpB6->f1;}}_LLAE:{const char*_tmp415;Cyc_Absyndump_dump(((
_tmp415="enum {",_tag_dyneither(_tmp415,sizeof(char),7))));}Cyc_Absyndump_dumpenumfields(
_tmpB7);{const char*_tmp416;Cyc_Absyndump_dump(((_tmp416="}",_tag_dyneither(
_tmp416,sizeof(char),2))));}return;_LLAF: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*
_tmpB8=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp65;if(_tmpB8->tag != 
17)goto _LLB1;else{_tmpB9=_tmpB8->f1;_tmpBA=_tmpB8->f2;}}_LLB0:(Cyc_Absyndump_dumpqvar(
_tmpB9),Cyc_Absyndump_dumptps(_tmpBA));return;_LLB1: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*
_tmpBB=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp65;if(_tmpBB->tag != 
18)goto _LLB3;else{_tmpBC=_tmpBB->f1;}}_LLB2:{const char*_tmp417;Cyc_Absyndump_dump(((
_tmp417="valueof_t(",_tag_dyneither(_tmp417,sizeof(char),11))));}Cyc_Absyndump_dumpexp(
_tmpBC);{const char*_tmp418;Cyc_Absyndump_dump(((_tmp418=")",_tag_dyneither(
_tmp418,sizeof(char),2))));}return;_LLB3: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*
_tmpBD=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp65;if(_tmpBD->tag != 
15)goto _LLB5;else{_tmpBE=(void*)_tmpBD->f1;}}_LLB4:{const char*_tmp419;Cyc_Absyndump_dump(((
_tmp419="region_t<",_tag_dyneither(_tmp419,sizeof(char),10))));}Cyc_Absyndump_dumprgn(
_tmpBE);{const char*_tmp41A;Cyc_Absyndump_dump(((_tmp41A=">",_tag_dyneither(
_tmp41A,sizeof(char),2))));}return;_LLB5: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*
_tmpBF=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp65;if(_tmpBF->tag != 16)
goto _LLB7;else{_tmpC0=(void*)_tmpBF->f1;_tmpC1=(void*)_tmpBF->f2;}}_LLB6:{const
char*_tmp41B;Cyc_Absyndump_dump(((_tmp41B="dynregion_t<",_tag_dyneither(_tmp41B,
sizeof(char),13))));}Cyc_Absyndump_dumprgn(_tmpC0);{const char*_tmp41C;Cyc_Absyndump_dump(((
_tmp41C=",",_tag_dyneither(_tmp41C,sizeof(char),2))));}Cyc_Absyndump_dumprgn(
_tmpC1);{const char*_tmp41D;Cyc_Absyndump_dump(((_tmp41D=">",_tag_dyneither(
_tmp41D,sizeof(char),2))));}return;_LLB7: {struct Cyc_Absyn_TagType_Absyn_Type_struct*
_tmpC2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp65;if(_tmpC2->tag != 19)
goto _LLB9;else{_tmpC3=(void*)_tmpC2->f1;}}_LLB8:{const char*_tmp41E;Cyc_Absyndump_dump(((
_tmp41E="tag_t<",_tag_dyneither(_tmp41E,sizeof(char),7))));}Cyc_Absyndump_dumpntyp(
_tmpC3);{const char*_tmp41F;Cyc_Absyndump_dump(((_tmp41F=">",_tag_dyneither(
_tmp41F,sizeof(char),2))));}return;_LLB9: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*
_tmpC4=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp65;if(_tmpC4->tag != 21)
goto _LLBB;}_LLBA:{const char*_tmp420;Cyc_Absyndump_dump(((_tmp420="`U",
_tag_dyneither(_tmp420,sizeof(char),3))));}goto _LL72;_LLBB: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*
_tmpC5=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp65;if(_tmpC5->tag != 22)
goto _LLBD;}_LLBC:{const char*_tmp421;Cyc_Absyndump_dump(((_tmp421="`RC",
_tag_dyneither(_tmp421,sizeof(char),4))));}goto _LL72;_LLBD: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*
_tmpC6=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp65;if(_tmpC6->tag != 
26)goto _LLBF;else{_tmpC7=_tmpC6->f1;_tmpC8=*_tmpC7;_tmpC9=_tmpC8.r;{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*
_tmpCA=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmpC9;if(_tmpCA->tag
!= 0)goto _LLBF;else{_tmpCB=_tmpCA->f1;}};}}_LLBE: Cyc_Absyndump_dump_aggrdecl(
_tmpCB);return;_LLBF: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpCC=(
struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp65;if(_tmpCC->tag != 26)goto
_LLC1;else{_tmpCD=_tmpCC->f1;_tmpCE=*_tmpCD;_tmpCF=_tmpCE.r;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*
_tmpD0=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmpCF;if(_tmpD0->tag
!= 1)goto _LLC1;else{_tmpD1=_tmpD0->f1;}};}}_LLC0: Cyc_Absyndump_dump_enumdecl(
_tmpD1);return;_LLC1: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpD2=(
struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp65;if(_tmpD2->tag != 26)goto
_LLC3;else{_tmpD3=_tmpD2->f1;_tmpD4=*_tmpD3;_tmpD5=_tmpD4.r;{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*
_tmpD6=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmpD5;if(_tmpD6->tag
!= 2)goto _LLC3;else{_tmpD7=_tmpD6->f1;}};}}_LLC2: Cyc_Absyndump_dump_datatypedecl(
_tmpD7);return;_LLC3: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpD8=(struct
Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp65;if(_tmpD8->tag != 20)goto _LLC5;}_LLC4:
goto _LLC6;_LLC5: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpD9=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)
_tmp65;if(_tmpD9->tag != 23)goto _LLC7;}_LLC6: goto _LLC8;_LLC7: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*
_tmpDA=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp65;if(_tmpDA->tag != 25)
goto _LLC9;}_LLC8: goto _LLCA;_LLC9: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*
_tmpDB=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp65;if(_tmpDB->tag != 24)
goto _LL72;}_LLCA: return;_LL72:;}void Cyc_Absyndump_dumpvaropt(struct
_dyneither_ptr*vo){if(vo != 0)Cyc_Absyndump_dump_str((struct _dyneither_ptr*)vo);}
void Cyc_Absyndump_dumpfunarg(struct _tuple8*t){((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)((*
t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}struct _tuple15{void*f1;void*f2;
};void Cyc_Absyndump_dump_rgncmp(struct _tuple15*cmp){struct _tuple15 _tmp11F;void*
_tmp120;void*_tmp121;struct _tuple15*_tmp11E=cmp;_tmp11F=*_tmp11E;_tmp120=_tmp11F.f1;
_tmp121=_tmp11F.f2;Cyc_Absyndump_dumpeff(_tmp120);Cyc_Absyndump_dump_char((int)'>');
Cyc_Absyndump_dumprgn(_tmp121);}void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*
rgn_po){const char*_tmp422;((void(*)(void(*f)(struct _tuple15*),struct Cyc_List_List*
l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,
rgn_po,((_tmp422=",",_tag_dyneither(_tmp422,sizeof(char),2))));}void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
void*effopt,struct Cyc_List_List*rgn_po){Cyc_Absyndump_dump_char((int)'(');for(0;
args != 0;args=args->tl){Cyc_Absyndump_dumpfunarg((struct _tuple8*)args->hd);if((
args->tl != 0  || c_varargs) || cyc_varargs != 0)Cyc_Absyndump_dump_char((int)',');}
if(c_varargs){const char*_tmp423;Cyc_Absyndump_dump(((_tmp423="...",
_tag_dyneither(_tmp423,sizeof(char),4))));}else{if(cyc_varargs != 0){struct
_tuple8*_tmp424;struct _tuple8*_tmp124=(_tmp424=_cycalloc(sizeof(*_tmp424)),((
_tmp424->f1=cyc_varargs->name,((_tmp424->f2=cyc_varargs->tq,((_tmp424->f3=
cyc_varargs->type,_tmp424)))))));{const char*_tmp425;Cyc_Absyndump_dump(((_tmp425="...",
_tag_dyneither(_tmp425,sizeof(char),4))));}if(cyc_varargs->inject){const char*
_tmp426;Cyc_Absyndump_dump(((_tmp426=" inject ",_tag_dyneither(_tmp426,sizeof(
char),9))));}Cyc_Absyndump_dumpfunarg(_tmp124);}}if(effopt != 0){Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpeff((void*)effopt);}if(rgn_po != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dump_rgnpo(rgn_po);}Cyc_Absyndump_dump_char((int)')');}
void Cyc_Absyndump_dumptyp(void*t){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(
int),int))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,(void(*)(int x))Cyc_Absyndump_ignore,
0);}void Cyc_Absyndump_dumpdesignator(void*d){void*_tmp128=d;struct Cyc_Absyn_Exp*
_tmp12A;struct _dyneither_ptr*_tmp12C;_LLDC: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*
_tmp129=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp128;if(
_tmp129->tag != 0)goto _LLDE;else{_tmp12A=_tmp129->f1;}}_LLDD:{const char*_tmp427;
Cyc_Absyndump_dump(((_tmp427=".[",_tag_dyneither(_tmp427,sizeof(char),3))));}Cyc_Absyndump_dumpexp(
_tmp12A);Cyc_Absyndump_dump_char((int)']');goto _LLDB;_LLDE: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*
_tmp12B=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp128;if(_tmp12B->tag
!= 1)goto _LLDB;else{_tmp12C=_tmp12B->f1;}}_LLDF: Cyc_Absyndump_dump_char((int)'.');
Cyc_Absyndump_dump_nospace(*_tmp12C);goto _LLDB;_LLDB:;}struct _tuple16{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};void Cyc_Absyndump_dumpde(struct _tuple16*de){{const
char*_tmp42A;const char*_tmp429;const char*_tmp428;((void(*)(void(*f)(void*),
struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct
_dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,(*de).f1,((
_tmp428="",_tag_dyneither(_tmp428,sizeof(char),1))),((_tmp429="=",_tag_dyneither(
_tmp429,sizeof(char),2))),((_tmp42A="=",_tag_dyneither(_tmp42A,sizeof(char),2))));}
Cyc_Absyndump_dumpexp((*de).f2);}void Cyc_Absyndump_dumpexps_prec(int inprec,
struct Cyc_List_List*es){const char*_tmp42D;const char*_tmp42C;const char*_tmp42B;((
void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec,inprec,es,((_tmp42B="",_tag_dyneither(_tmp42B,sizeof(
char),1))),((_tmp42C="",_tag_dyneither(_tmp42C,sizeof(char),1))),((_tmp42D=",",
_tag_dyneither(_tmp42D,sizeof(char),2))));}void Cyc_Absyndump_dumpexp_prec(int
inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);if(inprec >= 
myprec){const char*_tmp42E;Cyc_Absyndump_dump_nospace(((_tmp42E="(",
_tag_dyneither(_tmp42E,sizeof(char),2))));}{void*_tmp135=e->r;union Cyc_Absyn_Cnst
_tmp137;struct _tuple3 _tmp138;enum Cyc_Absyn_Sign _tmp139;char _tmp13A;union Cyc_Absyn_Cnst
_tmp13C;struct _dyneither_ptr _tmp13D;union Cyc_Absyn_Cnst _tmp13F;struct _tuple4
_tmp140;enum Cyc_Absyn_Sign _tmp141;short _tmp142;union Cyc_Absyn_Cnst _tmp144;
struct _tuple5 _tmp145;enum Cyc_Absyn_Sign _tmp146;int _tmp147;union Cyc_Absyn_Cnst
_tmp149;struct _tuple5 _tmp14A;enum Cyc_Absyn_Sign _tmp14B;int _tmp14C;union Cyc_Absyn_Cnst
_tmp14E;struct _tuple5 _tmp14F;enum Cyc_Absyn_Sign _tmp150;int _tmp151;union Cyc_Absyn_Cnst
_tmp153;struct _tuple6 _tmp154;enum Cyc_Absyn_Sign _tmp155;long long _tmp156;union
Cyc_Absyn_Cnst _tmp158;struct _tuple7 _tmp159;struct _dyneither_ptr _tmp15A;union Cyc_Absyn_Cnst
_tmp15C;int _tmp15D;union Cyc_Absyn_Cnst _tmp15F;struct _dyneither_ptr _tmp160;union
Cyc_Absyn_Cnst _tmp162;struct _dyneither_ptr _tmp163;struct _tuple0*_tmp165;enum Cyc_Absyn_Primop
_tmp167;struct Cyc_List_List*_tmp168;struct Cyc_Absyn_Exp*_tmp16A;struct Cyc_Core_Opt*
_tmp16B;struct Cyc_Absyn_Exp*_tmp16C;struct Cyc_Absyn_Exp*_tmp16E;enum Cyc_Absyn_Incrementor
_tmp16F;struct Cyc_Absyn_Exp*_tmp171;enum Cyc_Absyn_Incrementor _tmp172;struct Cyc_Absyn_Exp*
_tmp174;enum Cyc_Absyn_Incrementor _tmp175;struct Cyc_Absyn_Exp*_tmp177;enum Cyc_Absyn_Incrementor
_tmp178;struct Cyc_Absyn_Exp*_tmp17A;struct Cyc_Absyn_Exp*_tmp17B;struct Cyc_Absyn_Exp*
_tmp17C;struct Cyc_Absyn_Exp*_tmp17E;struct Cyc_Absyn_Exp*_tmp17F;struct Cyc_Absyn_Exp*
_tmp181;struct Cyc_Absyn_Exp*_tmp182;struct Cyc_Absyn_Exp*_tmp184;struct Cyc_Absyn_Exp*
_tmp185;struct Cyc_Absyn_Exp*_tmp187;struct Cyc_List_List*_tmp188;struct Cyc_Absyn_Exp*
_tmp18A;struct Cyc_Absyn_Exp*_tmp18C;struct Cyc_Absyn_Exp*_tmp18E;void*_tmp190;
struct Cyc_Absyn_Exp*_tmp191;struct Cyc_Absyn_Exp*_tmp193;struct Cyc_Absyn_Exp*
_tmp195;struct Cyc_Absyn_Exp*_tmp196;void*_tmp198;struct Cyc_Absyn_Exp*_tmp19A;
void*_tmp19C;int _tmp19E;struct _dyneither_ptr _tmp19F;struct Cyc_Absyn_Exp*_tmp1A1;
struct _dyneither_ptr*_tmp1A2;void*_tmp1A4;void*_tmp1A5;struct _dyneither_ptr*
_tmp1A7;void*_tmp1A9;void*_tmp1AA;unsigned int _tmp1AC;struct Cyc_Absyn_Exp*
_tmp1AE;struct Cyc_Absyn_Exp*_tmp1B0;struct _dyneither_ptr*_tmp1B1;struct Cyc_Absyn_Exp*
_tmp1B3;struct _dyneither_ptr*_tmp1B4;struct Cyc_Absyn_Exp*_tmp1B6;struct Cyc_Absyn_Exp*
_tmp1B7;struct Cyc_List_List*_tmp1B9;struct _tuple8*_tmp1BB;struct Cyc_List_List*
_tmp1BC;struct Cyc_List_List*_tmp1BE;struct Cyc_Absyn_Vardecl*_tmp1C0;struct Cyc_Absyn_Exp*
_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C2;struct _tuple0*_tmp1C4;struct Cyc_List_List*
_tmp1C5;struct Cyc_List_List*_tmp1C6;struct Cyc_List_List*_tmp1C8;struct Cyc_List_List*
_tmp1CA;struct Cyc_Absyn_Datatypefield*_tmp1CB;struct _tuple0*_tmp1CD;struct
_tuple0*_tmp1CF;struct Cyc_Absyn_MallocInfo _tmp1D1;int _tmp1D2;struct Cyc_Absyn_Exp*
_tmp1D3;void**_tmp1D4;struct Cyc_Absyn_Exp*_tmp1D5;struct Cyc_Absyn_Exp*_tmp1D7;
struct Cyc_Absyn_Exp*_tmp1D8;struct Cyc_Core_Opt*_tmp1DA;struct Cyc_List_List*
_tmp1DB;struct Cyc_Absyn_Stmt*_tmp1DD;_LLE1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp136=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp136->tag != 
0)goto _LLE3;else{_tmp137=_tmp136->f1;if((_tmp137.Char_c).tag != 2)goto _LLE3;
_tmp138=(struct _tuple3)(_tmp137.Char_c).val;_tmp139=_tmp138.f1;_tmp13A=_tmp138.f2;}}
_LLE2: Cyc_Absyndump_dump_char((int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(
_tmp13A));Cyc_Absyndump_dump_char((int)'\'');goto _LLE0;_LLE3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp13B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp13B->tag != 
0)goto _LLE5;else{_tmp13C=_tmp13B->f1;if((_tmp13C.Wchar_c).tag != 3)goto _LLE5;
_tmp13D=(struct _dyneither_ptr)(_tmp13C.Wchar_c).val;}}_LLE4:{const char*_tmp432;
void*_tmp431[1];struct Cyc_String_pa_PrintArg_struct _tmp430;Cyc_Absyndump_dump((
struct _dyneither_ptr)((_tmp430.tag=0,((_tmp430.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp13D),((_tmp431[0]=& _tmp430,Cyc_aprintf(((_tmp432="L'%s'",
_tag_dyneither(_tmp432,sizeof(char),6))),_tag_dyneither(_tmp431,sizeof(void*),1)))))))));}
goto _LLE0;_LLE5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp13E=(struct
Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp13E->tag != 0)goto _LLE7;
else{_tmp13F=_tmp13E->f1;if((_tmp13F.Short_c).tag != 4)goto _LLE7;_tmp140=(struct
_tuple4)(_tmp13F.Short_c).val;_tmp141=_tmp140.f1;_tmp142=_tmp140.f2;}}_LLE6:{
const char*_tmp436;void*_tmp435[1];struct Cyc_Int_pa_PrintArg_struct _tmp434;Cyc_Absyndump_dump((
struct _dyneither_ptr)((_tmp434.tag=1,((_tmp434.f1=(unsigned long)((int)_tmp142),((
_tmp435[0]=& _tmp434,Cyc_aprintf(((_tmp436="%d",_tag_dyneither(_tmp436,sizeof(
char),3))),_tag_dyneither(_tmp435,sizeof(void*),1)))))))));}goto _LLE0;_LLE7: {
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp143=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmp135;if(_tmp143->tag != 0)goto _LLE9;else{_tmp144=_tmp143->f1;if((_tmp144.Int_c).tag
!= 5)goto _LLE9;_tmp145=(struct _tuple5)(_tmp144.Int_c).val;_tmp146=_tmp145.f1;if(
_tmp146 != Cyc_Absyn_None)goto _LLE9;_tmp147=_tmp145.f2;}}_LLE8: _tmp14C=_tmp147;
goto _LLEA;_LLE9: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp148=(struct
Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp148->tag != 0)goto _LLEB;
else{_tmp149=_tmp148->f1;if((_tmp149.Int_c).tag != 5)goto _LLEB;_tmp14A=(struct
_tuple5)(_tmp149.Int_c).val;_tmp14B=_tmp14A.f1;if(_tmp14B != Cyc_Absyn_Signed)
goto _LLEB;_tmp14C=_tmp14A.f2;}}_LLEA:{const char*_tmp43A;void*_tmp439[1];struct
Cyc_Int_pa_PrintArg_struct _tmp438;Cyc_Absyndump_dump((struct _dyneither_ptr)((
_tmp438.tag=1,((_tmp438.f1=(unsigned long)_tmp14C,((_tmp439[0]=& _tmp438,Cyc_aprintf(((
_tmp43A="%d",_tag_dyneither(_tmp43A,sizeof(char),3))),_tag_dyneither(_tmp439,
sizeof(void*),1)))))))));}goto _LLE0;_LLEB: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp14D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp14D->tag != 
0)goto _LLED;else{_tmp14E=_tmp14D->f1;if((_tmp14E.Int_c).tag != 5)goto _LLED;
_tmp14F=(struct _tuple5)(_tmp14E.Int_c).val;_tmp150=_tmp14F.f1;if(_tmp150 != Cyc_Absyn_Unsigned)
goto _LLED;_tmp151=_tmp14F.f2;}}_LLEC:{const char*_tmp43E;void*_tmp43D[1];struct
Cyc_Int_pa_PrintArg_struct _tmp43C;Cyc_Absyndump_dump((struct _dyneither_ptr)((
_tmp43C.tag=1,((_tmp43C.f1=(unsigned int)_tmp151,((_tmp43D[0]=& _tmp43C,Cyc_aprintf(((
_tmp43E="%u",_tag_dyneither(_tmp43E,sizeof(char),3))),_tag_dyneither(_tmp43D,
sizeof(void*),1)))))))));}goto _LLE0;_LLED: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp152=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp152->tag != 
0)goto _LLEF;else{_tmp153=_tmp152->f1;if((_tmp153.LongLong_c).tag != 6)goto _LLEF;
_tmp154=(struct _tuple6)(_tmp153.LongLong_c).val;_tmp155=_tmp154.f1;_tmp156=
_tmp154.f2;}}_LLEE:{const char*_tmp43F;Cyc_Absyndump_dump(((_tmp43F="<<FIX LONG LONG CONSTANT>>",
_tag_dyneither(_tmp43F,sizeof(char),27))));}goto _LLE0;_LLEF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp157=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp157->tag != 
0)goto _LLF1;else{_tmp158=_tmp157->f1;if((_tmp158.Float_c).tag != 7)goto _LLF1;
_tmp159=(struct _tuple7)(_tmp158.Float_c).val;_tmp15A=_tmp159.f1;}}_LLF0: Cyc_Absyndump_dump(
_tmp15A);goto _LLE0;_LLF1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp15B=(
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp15B->tag != 0)goto
_LLF3;else{_tmp15C=_tmp15B->f1;if((_tmp15C.Null_c).tag != 1)goto _LLF3;_tmp15D=(
int)(_tmp15C.Null_c).val;}}_LLF2:{const char*_tmp440;Cyc_Absyndump_dump(((_tmp440="NULL",
_tag_dyneither(_tmp440,sizeof(char),5))));}goto _LLE0;_LLF3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp15E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp15E->tag != 
0)goto _LLF5;else{_tmp15F=_tmp15E->f1;if((_tmp15F.String_c).tag != 8)goto _LLF5;
_tmp160=(struct _dyneither_ptr)(_tmp15F.String_c).val;}}_LLF4: Cyc_Absyndump_dump_char((
int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp160));Cyc_Absyndump_dump_char((
int)'"');goto _LLE0;_LLF5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp161=(
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp161->tag != 0)goto
_LLF7;else{_tmp162=_tmp161->f1;if((_tmp162.Wstring_c).tag != 9)goto _LLF7;_tmp163=(
struct _dyneither_ptr)(_tmp162.Wstring_c).val;}}_LLF6:{const char*_tmp441;Cyc_Absyndump_dump(((
_tmp441="L\"",_tag_dyneither(_tmp441,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(
_tmp163);Cyc_Absyndump_dump_char((int)'"');goto _LLE0;_LLF7: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp164=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp164->tag != 1)
goto _LLF9;else{_tmp165=_tmp164->f1;}}_LLF8: Cyc_Absyndump_dumpqvar(_tmp165);goto
_LLE0;_LLF9: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp166=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)
_tmp135;if(_tmp166->tag != 2)goto _LLFB;else{_tmp167=_tmp166->f1;_tmp168=_tmp166->f2;}}
_LLFA: {struct _dyneither_ptr _tmp1ED=Cyc_Absynpp_prim2str(_tmp167);switch(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp168)){case 1: _LL14B: if(_tmp167 == (
enum Cyc_Absyn_Primop)Cyc_Absyn_Numelts){{const char*_tmp442;Cyc_Absyndump_dump(((
_tmp442="numelts(",_tag_dyneither(_tmp442,sizeof(char),9))));}Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp168))->hd);{const
char*_tmp443;Cyc_Absyndump_dump(((_tmp443=")",_tag_dyneither(_tmp443,sizeof(char),
2))));};}else{Cyc_Absyndump_dump(_tmp1ED);Cyc_Absyndump_dumpexp_prec(myprec,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp168))->hd);}break;
case 2: _LL14C: Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp168))->hd);Cyc_Absyndump_dump(_tmp1ED);Cyc_Absyndump_dump_char((
int)' ');Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp168->tl))->hd);break;default: _LL14D: {struct Cyc_Core_Failure_exn_struct
_tmp449;const char*_tmp448;struct Cyc_Core_Failure_exn_struct*_tmp447;(int)_throw((
void*)((_tmp447=_cycalloc(sizeof(*_tmp447)),((_tmp447[0]=((_tmp449.tag=Cyc_Core_Failure,((
_tmp449.f1=((_tmp448="Absyndump -- Bad number of arguments to primop",
_tag_dyneither(_tmp448,sizeof(char),47))),_tmp449)))),_tmp447)))));}}goto _LLE0;}
_LLFB: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp169=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)
_tmp135;if(_tmp169->tag != 3)goto _LLFD;else{_tmp16A=_tmp169->f1;_tmp16B=_tmp169->f2;
_tmp16C=_tmp169->f3;}}_LLFC: Cyc_Absyndump_dumpexp_prec(myprec,_tmp16A);if(
_tmp16B != 0)Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)
_tmp16B->v));{const char*_tmp44A;Cyc_Absyndump_dump_nospace(((_tmp44A="=",
_tag_dyneither(_tmp44A,sizeof(char),2))));}Cyc_Absyndump_dumpexp_prec(myprec,
_tmp16C);goto _LLE0;_LLFD: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*
_tmp16D=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp16D->tag
!= 4)goto _LLFF;else{_tmp16E=_tmp16D->f1;_tmp16F=_tmp16D->f2;if(_tmp16F != Cyc_Absyn_PreInc)
goto _LLFF;}}_LLFE:{const char*_tmp44B;Cyc_Absyndump_dump(((_tmp44B="++",
_tag_dyneither(_tmp44B,sizeof(char),3))));}Cyc_Absyndump_dumpexp_prec(myprec,
_tmp16E);goto _LLE0;_LLFF: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*
_tmp170=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp170->tag
!= 4)goto _LL101;else{_tmp171=_tmp170->f1;_tmp172=_tmp170->f2;if(_tmp172 != Cyc_Absyn_PreDec)
goto _LL101;}}_LL100:{const char*_tmp44C;Cyc_Absyndump_dump(((_tmp44C="--",
_tag_dyneither(_tmp44C,sizeof(char),3))));}Cyc_Absyndump_dumpexp_prec(myprec,
_tmp171);goto _LLE0;_LL101: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*
_tmp173=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp173->tag
!= 4)goto _LL103;else{_tmp174=_tmp173->f1;_tmp175=_tmp173->f2;if(_tmp175 != Cyc_Absyn_PostInc)
goto _LL103;}}_LL102: Cyc_Absyndump_dumpexp_prec(myprec,_tmp174);{const char*
_tmp44D;Cyc_Absyndump_dump(((_tmp44D="++",_tag_dyneither(_tmp44D,sizeof(char),3))));}
goto _LLE0;_LL103: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp176=(
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp176->tag != 4)
goto _LL105;else{_tmp177=_tmp176->f1;_tmp178=_tmp176->f2;if(_tmp178 != Cyc_Absyn_PostDec)
goto _LL105;}}_LL104: Cyc_Absyndump_dumpexp_prec(myprec,_tmp177);{const char*
_tmp44E;Cyc_Absyndump_dump(((_tmp44E="--",_tag_dyneither(_tmp44E,sizeof(char),3))));}
goto _LLE0;_LL105: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp179=(
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp179->tag != 5)
goto _LL107;else{_tmp17A=_tmp179->f1;_tmp17B=_tmp179->f2;_tmp17C=_tmp179->f3;}}
_LL106: Cyc_Absyndump_dumpexp_prec(myprec,_tmp17A);Cyc_Absyndump_dump_char((int)'?');
Cyc_Absyndump_dumpexp_prec(0,_tmp17B);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp17C);goto _LLE0;_LL107: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*
_tmp17D=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp17D->tag != 6)
goto _LL109;else{_tmp17E=_tmp17D->f1;_tmp17F=_tmp17D->f2;}}_LL108: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp17E);{const char*_tmp44F;Cyc_Absyndump_dump(((_tmp44F=" && ",
_tag_dyneither(_tmp44F,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(myprec,
_tmp17F);goto _LLE0;_LL109: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp180=(
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp180->tag != 7)goto
_LL10B;else{_tmp181=_tmp180->f1;_tmp182=_tmp180->f2;}}_LL10A: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp181);{const char*_tmp450;Cyc_Absyndump_dump(((_tmp450=" || ",
_tag_dyneither(_tmp450,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(myprec,
_tmp182);goto _LLE0;_LL10B: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*
_tmp183=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp183->tag
!= 8)goto _LL10D;else{_tmp184=_tmp183->f1;_tmp185=_tmp183->f2;}}_LL10C: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumpexp_prec(myprec,_tmp184);Cyc_Absyndump_dump_char((int)',');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp185);Cyc_Absyndump_dump_char((int)')');
goto _LLE0;_LL10D: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp186=(struct
Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp186->tag != 9)goto _LL10F;
else{_tmp187=_tmp186->f1;_tmp188=_tmp186->f2;}}_LL10E: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp187);{const char*_tmp451;Cyc_Absyndump_dump_nospace(((_tmp451="(",
_tag_dyneither(_tmp451,sizeof(char),2))));}Cyc_Absyndump_dumpexps_prec(20,
_tmp188);{const char*_tmp452;Cyc_Absyndump_dump_nospace(((_tmp452=")",
_tag_dyneither(_tmp452,sizeof(char),2))));}goto _LLE0;_LL10F: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*
_tmp189=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp189->tag != 
10)goto _LL111;else{_tmp18A=_tmp189->f1;}}_LL110:{const char*_tmp453;Cyc_Absyndump_dump(((
_tmp453="throw",_tag_dyneither(_tmp453,sizeof(char),6))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp18A);goto _LLE0;_LL111: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*
_tmp18B=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp135;if(
_tmp18B->tag != 11)goto _LL113;else{_tmp18C=_tmp18B->f1;}}_LL112: _tmp18E=_tmp18C;
goto _LL114;_LL113: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp18D=(
struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp18D->tag != 12)
goto _LL115;else{_tmp18E=_tmp18D->f1;}}_LL114: Cyc_Absyndump_dumpexp_prec(inprec,
_tmp18E);goto _LLE0;_LL115: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp18F=(
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp18F->tag != 13)goto
_LL117;else{_tmp190=(void*)_tmp18F->f1;_tmp191=_tmp18F->f2;}}_LL116: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumptyp(_tmp190);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp191);goto _LLE0;_LL117: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*
_tmp192=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp192->tag
!= 14)goto _LL119;else{_tmp193=_tmp192->f1;}}_LL118: Cyc_Absyndump_dump_char((int)'&');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp193);goto _LLE0;_LL119: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*
_tmp194=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp194->tag != 
15)goto _LL11B;else{_tmp195=_tmp194->f1;_tmp196=_tmp194->f2;}}_LL11A:{const char*
_tmp454;Cyc_Absyndump_dump(((_tmp454="new ",_tag_dyneither(_tmp454,sizeof(char),
5))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp196);goto _LLE0;_LL11B: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*
_tmp197=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp197->tag
!= 16)goto _LL11D;else{_tmp198=(void*)_tmp197->f1;}}_LL11C:{const char*_tmp455;Cyc_Absyndump_dump(((
_tmp455="sizeof(",_tag_dyneither(_tmp455,sizeof(char),8))));}Cyc_Absyndump_dumptyp(
_tmp198);Cyc_Absyndump_dump_char((int)')');goto _LLE0;_LL11D: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*
_tmp199=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp199->tag
!= 17)goto _LL11F;else{_tmp19A=_tmp199->f1;}}_LL11E:{const char*_tmp456;Cyc_Absyndump_dump(((
_tmp456="sizeof(",_tag_dyneither(_tmp456,sizeof(char),8))));}Cyc_Absyndump_dumpexp(
_tmp19A);Cyc_Absyndump_dump_char((int)')');goto _LLE0;_LL11F: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*
_tmp19B=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp19B->tag
!= 37)goto _LL121;else{_tmp19C=(void*)_tmp19B->f1;}}_LL120:{const char*_tmp457;Cyc_Absyndump_dump(((
_tmp457="valueof(",_tag_dyneither(_tmp457,sizeof(char),9))));}Cyc_Absyndump_dumptyp(
_tmp19C);Cyc_Absyndump_dump_char((int)')');goto _LLE0;_LL121: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*
_tmp19D=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp19D->tag != 
38)goto _LL123;else{_tmp19E=_tmp19D->f1;_tmp19F=_tmp19D->f2;}}_LL122:{const char*
_tmp458;Cyc_Absyndump_dump(((_tmp458="__asm__",_tag_dyneither(_tmp458,sizeof(
char),8))));}if(_tmp19E){const char*_tmp459;Cyc_Absyndump_dump(((_tmp459=" volatile ",
_tag_dyneither(_tmp459,sizeof(char),11))));}Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_nospace(_tmp19F);Cyc_Absyndump_dump_char((int)')');goto _LLE0;
_LL123: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp1A0=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)
_tmp135;if(_tmp1A0->tag != 36)goto _LL125;else{_tmp1A1=_tmp1A0->f1;_tmp1A2=_tmp1A0->f2;}}
_LL124:{const char*_tmp45A;Cyc_Absyndump_dump(((_tmp45A="tagcheck(",
_tag_dyneither(_tmp45A,sizeof(char),10))));}Cyc_Absyndump_dumpexp(_tmp1A1);Cyc_Absyndump_dump_char((
int)'.');Cyc_Absyndump_dump_nospace(*_tmp1A2);Cyc_Absyndump_dump_char((int)')');
goto _LLE0;_LL125: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp1A3=(
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp1A3->tag != 18)
goto _LL127;else{_tmp1A4=(void*)_tmp1A3->f1;_tmp1A5=(void*)_tmp1A3->f2;{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*
_tmp1A6=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp1A5;if(
_tmp1A6->tag != 0)goto _LL127;else{_tmp1A7=_tmp1A6->f1;}};}}_LL126:{const char*
_tmp45B;Cyc_Absyndump_dump(((_tmp45B="offsetof(",_tag_dyneither(_tmp45B,sizeof(
char),10))));}Cyc_Absyndump_dumptyp(_tmp1A4);Cyc_Absyndump_dump_char((int)',');
Cyc_Absyndump_dump_nospace(*_tmp1A7);Cyc_Absyndump_dump_char((int)')');goto _LLE0;
_LL127: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp1A8=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)
_tmp135;if(_tmp1A8->tag != 18)goto _LL129;else{_tmp1A9=(void*)_tmp1A8->f1;_tmp1AA=(
void*)_tmp1A8->f2;{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp1AB=(
struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp1AA;if(_tmp1AB->tag != 
1)goto _LL129;else{_tmp1AC=_tmp1AB->f1;}};}}_LL128:{const char*_tmp45C;Cyc_Absyndump_dump(((
_tmp45C="offsetof(",_tag_dyneither(_tmp45C,sizeof(char),10))));}Cyc_Absyndump_dumptyp(
_tmp1A9);Cyc_Absyndump_dump_char((int)',');{const char*_tmp460;void*_tmp45F[1];
struct Cyc_Int_pa_PrintArg_struct _tmp45E;Cyc_Absyndump_dump((struct _dyneither_ptr)((
_tmp45E.tag=1,((_tmp45E.f1=(unsigned long)((int)_tmp1AC),((_tmp45F[0]=& _tmp45E,
Cyc_aprintf(((_tmp460="%d",_tag_dyneither(_tmp460,sizeof(char),3))),
_tag_dyneither(_tmp45F,sizeof(void*),1)))))))));}Cyc_Absyndump_dump_char((int)')');
goto _LLE0;_LL129: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp1AD=(struct
Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp1AD->tag != 19)goto _LL12B;
else{_tmp1AE=_tmp1AD->f1;}}_LL12A: Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp1AE);goto _LLE0;_LL12B: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*
_tmp1AF=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp1AF->tag
!= 20)goto _LL12D;else{_tmp1B0=_tmp1AF->f1;_tmp1B1=_tmp1AF->f2;}}_LL12C: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp1B0);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp1B1);goto _LLE0;_LL12D: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*
_tmp1B2=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp1B2->tag
!= 21)goto _LL12F;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B2->f2;}}_LL12E: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp1B3);{const char*_tmp461;Cyc_Absyndump_dump_nospace(((_tmp461="->",
_tag_dyneither(_tmp461,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(*_tmp1B4);
goto _LLE0;_LL12F: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1B5=(
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp1B5->tag != 22)
goto _LL131;else{_tmp1B6=_tmp1B5->f1;_tmp1B7=_tmp1B5->f2;}}_LL130: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp1B6);Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp1B7);
Cyc_Absyndump_dump_char((int)']');goto _LLE0;_LL131: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*
_tmp1B8=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp1B8->tag != 
23)goto _LL133;else{_tmp1B9=_tmp1B8->f1;}}_LL132:{const char*_tmp462;Cyc_Absyndump_dump(((
_tmp462="$(",_tag_dyneither(_tmp462,sizeof(char),3))));}Cyc_Absyndump_dumpexps_prec(
20,_tmp1B9);Cyc_Absyndump_dump_char((int)')');goto _LLE0;_LL133: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*
_tmp1BA=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp1BA->tag
!= 24)goto _LL135;else{_tmp1BB=_tmp1BA->f1;_tmp1BC=_tmp1BA->f2;}}_LL134: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumptyp((*_tmp1BB).f3);Cyc_Absyndump_dump_char((int)')');{
const char*_tmp465;const char*_tmp464;const char*_tmp463;((void(*)(void(*f)(struct
_tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr
end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp1BC,((
_tmp463="{",_tag_dyneither(_tmp463,sizeof(char),2))),((_tmp464="}",
_tag_dyneither(_tmp464,sizeof(char),2))),((_tmp465=",",_tag_dyneither(_tmp465,
sizeof(char),2))));}goto _LLE0;_LL135: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*
_tmp1BD=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp1BD->tag != 
25)goto _LL137;else{_tmp1BE=_tmp1BD->f1;}}_LL136:{const char*_tmp468;const char*
_tmp467;const char*_tmp466;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp1BE,((_tmp466="{",_tag_dyneither(_tmp466,sizeof(char),2))),((
_tmp467="}",_tag_dyneither(_tmp467,sizeof(char),2))),((_tmp468=",",
_tag_dyneither(_tmp468,sizeof(char),2))));}goto _LLE0;_LL137: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*
_tmp1BF=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp135;if(
_tmp1BF->tag != 26)goto _LL139;else{_tmp1C0=_tmp1BF->f1;_tmp1C1=_tmp1BF->f2;
_tmp1C2=_tmp1BF->f3;}}_LL138:{const char*_tmp469;Cyc_Absyndump_dump(((_tmp469="new {for",
_tag_dyneither(_tmp469,sizeof(char),9))));}Cyc_Absyndump_dump_str((*_tmp1C0->name).f2);
Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp1C1);Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dumpexp(_tmp1C2);Cyc_Absyndump_dump_char((int)'}');goto
_LLE0;_LL139: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1C3=(struct
Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp1C3->tag != 27)goto
_LL13B;else{_tmp1C4=_tmp1C3->f1;_tmp1C5=_tmp1C3->f2;_tmp1C6=_tmp1C3->f3;}}_LL13A:
Cyc_Absyndump_dumpqvar(_tmp1C4);Cyc_Absyndump_dump_char((int)'{');if(_tmp1C5 != 0)
Cyc_Absyndump_dumptps(_tmp1C5);{const char*_tmp46C;const char*_tmp46B;const char*
_tmp46A;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp1C6,((_tmp46A="",_tag_dyneither(_tmp46A,sizeof(char),1))),((
_tmp46B="}",_tag_dyneither(_tmp46B,sizeof(char),2))),((_tmp46C=",",
_tag_dyneither(_tmp46C,sizeof(char),2))));}goto _LLE0;_LL13B: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*
_tmp1C7=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp1C7->tag
!= 28)goto _LL13D;else{_tmp1C8=_tmp1C7->f2;}}_LL13C:{const char*_tmp46F;const char*
_tmp46E;const char*_tmp46D;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp1C8,((_tmp46D="{",_tag_dyneither(_tmp46D,sizeof(char),2))),((
_tmp46E="}",_tag_dyneither(_tmp46E,sizeof(char),2))),((_tmp46F=",",
_tag_dyneither(_tmp46F,sizeof(char),2))));}goto _LLE0;_LL13D: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*
_tmp1C9=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp1C9->tag
!= 29)goto _LL13F;else{_tmp1CA=_tmp1C9->f1;_tmp1CB=_tmp1C9->f3;}}_LL13E: Cyc_Absyndump_dumpqvar(
_tmp1CB->name);if(_tmp1CA != 0){const char*_tmp472;const char*_tmp471;const char*
_tmp470;((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp,_tmp1CA,((_tmp470="(",_tag_dyneither(_tmp470,sizeof(char),
2))),((_tmp471=")",_tag_dyneither(_tmp471,sizeof(char),2))),((_tmp472=",",
_tag_dyneither(_tmp472,sizeof(char),2))));}goto _LLE0;_LL13F: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*
_tmp1CC=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp1CC->tag != 
30)goto _LL141;else{_tmp1CD=_tmp1CC->f1;}}_LL140: Cyc_Absyndump_dumpqvar(_tmp1CD);
goto _LLE0;_LL141: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp1CE=(
struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp1CE->tag != 31)
goto _LL143;else{_tmp1CF=_tmp1CE->f1;}}_LL142: Cyc_Absyndump_dumpqvar(_tmp1CF);
goto _LLE0;_LL143: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp1D0=(struct
Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp1D0->tag != 32)goto _LL145;
else{_tmp1D1=_tmp1D0->f1;_tmp1D2=_tmp1D1.is_calloc;_tmp1D3=_tmp1D1.rgn;_tmp1D4=
_tmp1D1.elt_type;_tmp1D5=_tmp1D1.num_elts;}}_LL144: if(_tmp1D2){if(_tmp1D3 != 0){{
const char*_tmp473;Cyc_Absyndump_dump(((_tmp473="rcalloc(",_tag_dyneither(_tmp473,
sizeof(char),9))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp1D3);{const
char*_tmp474;Cyc_Absyndump_dump(((_tmp474=",",_tag_dyneither(_tmp474,sizeof(char),
2))));};}else{const char*_tmp475;Cyc_Absyndump_dump(((_tmp475="calloc",
_tag_dyneither(_tmp475,sizeof(char),7))));}Cyc_Absyndump_dumpexp(_tmp1D5);{const
char*_tmp476;Cyc_Absyndump_dump(((_tmp476=",",_tag_dyneither(_tmp476,sizeof(char),
2))));}Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp1D4)),0));{const char*_tmp477;Cyc_Absyndump_dump(((_tmp477=")",_tag_dyneither(
_tmp477,sizeof(char),2))));};}else{if(_tmp1D3 != 0){{const char*_tmp478;Cyc_Absyndump_dump(((
_tmp478="rmalloc(",_tag_dyneither(_tmp478,sizeof(char),9))));}Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_tmp1D3);{const char*_tmp479;Cyc_Absyndump_dump(((_tmp479=",",
_tag_dyneither(_tmp479,sizeof(char),2))));};}else{const char*_tmp47A;Cyc_Absyndump_dump(((
_tmp47A="malloc(",_tag_dyneither(_tmp47A,sizeof(char),8))));}if(_tmp1D4 != 0)Cyc_Absyndump_dumpexp(
Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp1D4,0),_tmp1D5,0));else{Cyc_Absyndump_dumpexp(
_tmp1D5);}{const char*_tmp47B;Cyc_Absyndump_dump(((_tmp47B=")",_tag_dyneither(
_tmp47B,sizeof(char),2))));};}goto _LLE0;_LL145: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*
_tmp1D6=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp1D6->tag != 
33)goto _LL147;else{_tmp1D7=_tmp1D6->f1;_tmp1D8=_tmp1D6->f2;}}_LL146: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp1D7);{const char*_tmp47C;Cyc_Absyndump_dump_nospace(((_tmp47C=":=:",
_tag_dyneither(_tmp47C,sizeof(char),4))));}Cyc_Absyndump_dumpexp_prec(myprec,
_tmp1D8);goto _LLE0;_LL147: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*
_tmp1D9=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp135;if(
_tmp1D9->tag != 34)goto _LL149;else{_tmp1DA=_tmp1D9->f1;_tmp1DB=_tmp1D9->f2;}}
_LL148:{const char*_tmp47F;const char*_tmp47E;const char*_tmp47D;((void(*)(void(*f)(
struct _tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
_tmp1DB,((_tmp47D="{",_tag_dyneither(_tmp47D,sizeof(char),2))),((_tmp47E="}",
_tag_dyneither(_tmp47E,sizeof(char),2))),((_tmp47F=",",_tag_dyneither(_tmp47F,
sizeof(char),2))));}goto _LLE0;_LL149: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*
_tmp1DC=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp135;if(_tmp1DC->tag
!= 35)goto _LLE0;else{_tmp1DD=_tmp1DC->f1;}}_LL14A:{const char*_tmp480;Cyc_Absyndump_dump_nospace(((
_tmp480="({",_tag_dyneither(_tmp480,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(
_tmp1DD,1);{const char*_tmp481;Cyc_Absyndump_dump_nospace(((_tmp481="})",
_tag_dyneither(_tmp481,sizeof(char),3))));}goto _LLE0;_LLE0:;}if(inprec >= myprec)
Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*
e){Cyc_Absyndump_dumpexp_prec(0,e);}void Cyc_Absyndump_dumpswitchclauses(struct
Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause*
_tmp22A=(struct Cyc_Absyn_Switch_clause*)scs->hd;if(_tmp22A->where_clause == 0  && (
_tmp22A->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){const char*_tmp482;Cyc_Absyndump_dump(((
_tmp482="default:",_tag_dyneither(_tmp482,sizeof(char),9))));}else{{const char*
_tmp483;Cyc_Absyndump_dump(((_tmp483="case",_tag_dyneither(_tmp483,sizeof(char),
5))));}Cyc_Absyndump_dumppat(_tmp22A->pattern);if(_tmp22A->where_clause != 0){{
const char*_tmp484;Cyc_Absyndump_dump(((_tmp484="&&",_tag_dyneither(_tmp484,
sizeof(char),3))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(
_tmp22A->where_clause));}{const char*_tmp485;Cyc_Absyndump_dump_nospace(((_tmp485=":",
_tag_dyneither(_tmp485,sizeof(char),2))));};}Cyc_Absyndump_dumpstmt(_tmp22A->body,
0);}}void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt){void*_tmp22F=
s->r;struct Cyc_Absyn_Exp*_tmp232;struct Cyc_Absyn_Stmt*_tmp234;struct Cyc_Absyn_Stmt*
_tmp235;struct Cyc_Absyn_Exp*_tmp237;struct Cyc_Absyn_Exp*_tmp239;struct Cyc_Absyn_Exp*
_tmp23B;struct Cyc_Absyn_Stmt*_tmp23C;struct Cyc_Absyn_Stmt*_tmp23D;struct _tuple9
_tmp23F;struct Cyc_Absyn_Exp*_tmp240;struct Cyc_Absyn_Stmt*_tmp241;struct
_dyneither_ptr*_tmp245;struct Cyc_Absyn_Exp*_tmp247;struct _tuple9 _tmp248;struct
Cyc_Absyn_Exp*_tmp249;struct _tuple9 _tmp24A;struct Cyc_Absyn_Exp*_tmp24B;struct Cyc_Absyn_Stmt*
_tmp24C;struct Cyc_Absyn_Exp*_tmp24E;struct Cyc_List_List*_tmp24F;struct Cyc_Absyn_Decl*
_tmp251;struct Cyc_Absyn_Stmt*_tmp252;struct _dyneither_ptr*_tmp254;struct Cyc_Absyn_Stmt*
_tmp255;struct Cyc_Absyn_Stmt*_tmp257;struct _tuple9 _tmp258;struct Cyc_Absyn_Exp*
_tmp259;struct Cyc_List_List*_tmp25B;struct Cyc_List_List*_tmp25D;struct Cyc_Absyn_Stmt*
_tmp25F;struct Cyc_List_List*_tmp260;struct Cyc_Absyn_Exp*_tmp262;_LL150: {struct
Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp230=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)
_tmp22F;if(_tmp230->tag != 0)goto _LL152;}_LL151: Cyc_Absyndump_dump_semi();goto
_LL14F;_LL152: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp231=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)
_tmp22F;if(_tmp231->tag != 1)goto _LL154;else{_tmp232=_tmp231->f1;}}_LL153: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dumpexp(_tmp232);Cyc_Absyndump_dump_semi();goto _LL14F;
_LL154: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp233=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)
_tmp22F;if(_tmp233->tag != 2)goto _LL156;else{_tmp234=_tmp233->f1;_tmp235=_tmp233->f2;}}
_LL155: if(Cyc_Absynpp_is_declaration(_tmp234)){Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpstmt(_tmp234,0);Cyc_Absyndump_dump_char((int)'}');}else{Cyc_Absyndump_dumpstmt(
_tmp234,0);}if(Cyc_Absynpp_is_declaration(_tmp235)){if(expstmt)Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp235,
expstmt);Cyc_Absyndump_dump_char((int)'}');if(expstmt)Cyc_Absyndump_dump_char((
int)')');Cyc_Absyndump_dump_semi();}else{Cyc_Absyndump_dumpstmt(_tmp235,expstmt);}
goto _LL14F;_LL156: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp236=(
struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp22F;if(_tmp236->tag != 3)goto
_LL158;else{_tmp237=_tmp236->f1;if(_tmp237 != 0)goto _LL158;}}_LL157: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp486;Cyc_Absyndump_dump(((_tmp486="return;",_tag_dyneither(
_tmp486,sizeof(char),8))));}goto _LL14F;_LL158: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*
_tmp238=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp22F;if(_tmp238->tag
!= 3)goto _LL15A;else{_tmp239=_tmp238->f1;}}_LL159: Cyc_Absyndump_dumploc(s->loc);{
const char*_tmp487;Cyc_Absyndump_dump(((_tmp487="return",_tag_dyneither(_tmp487,
sizeof(char),7))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(
_tmp239));Cyc_Absyndump_dump_semi();goto _LL14F;_LL15A: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*
_tmp23A=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp22F;if(_tmp23A->tag
!= 4)goto _LL15C;else{_tmp23B=_tmp23A->f1;_tmp23C=_tmp23A->f2;_tmp23D=_tmp23A->f3;}}
_LL15B: Cyc_Absyndump_dumploc(s->loc);{const char*_tmp488;Cyc_Absyndump_dump(((
_tmp488="if(",_tag_dyneither(_tmp488,sizeof(char),4))));}Cyc_Absyndump_dumpexp(
_tmp23B);{void*_tmp266=_tmp23C->r;_LL177: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*
_tmp267=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp266;if(_tmp267->tag != 
2)goto _LL179;}_LL178: goto _LL17A;_LL179: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*
_tmp268=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp266;if(_tmp268->tag != 
12)goto _LL17B;}_LL17A: goto _LL17C;_LL17B: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*
_tmp269=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp266;if(_tmp269->tag
!= 4)goto _LL17D;}_LL17C: goto _LL17E;_LL17D: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*
_tmp26A=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp266;if(_tmp26A->tag
!= 13)goto _LL17F;}_LL17E:{const char*_tmp489;Cyc_Absyndump_dump_nospace(((_tmp489="){",
_tag_dyneither(_tmp489,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(_tmp23C,0);Cyc_Absyndump_dump_char((
int)'}');goto _LL176;_LL17F:;_LL180: Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(
_tmp23C,0);_LL176:;}{void*_tmp26C=_tmp23D->r;_LL182: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*
_tmp26D=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp26C;if(_tmp26D->tag != 
0)goto _LL184;}_LL183: goto _LL181;_LL184:;_LL185:{const char*_tmp48A;Cyc_Absyndump_dump(((
_tmp48A="else{",_tag_dyneither(_tmp48A,sizeof(char),6))));}Cyc_Absyndump_dumpstmt(
_tmp23D,0);Cyc_Absyndump_dump_char((int)'}');goto _LL181;_LL181:;}goto _LL14F;
_LL15C: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp23E=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)
_tmp22F;if(_tmp23E->tag != 5)goto _LL15E;else{_tmp23F=_tmp23E->f1;_tmp240=_tmp23F.f1;
_tmp241=_tmp23E->f2;}}_LL15D: Cyc_Absyndump_dumploc(s->loc);{const char*_tmp48B;
Cyc_Absyndump_dump(((_tmp48B="while(",_tag_dyneither(_tmp48B,sizeof(char),7))));}
Cyc_Absyndump_dumpexp(_tmp240);{const char*_tmp48C;Cyc_Absyndump_dump_nospace(((
_tmp48C="){",_tag_dyneither(_tmp48C,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(
_tmp241,0);Cyc_Absyndump_dump_char((int)'}');goto _LL14F;_LL15E: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*
_tmp242=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp22F;if(_tmp242->tag
!= 6)goto _LL160;}_LL15F: Cyc_Absyndump_dumploc(s->loc);{const char*_tmp48D;Cyc_Absyndump_dump(((
_tmp48D="break;",_tag_dyneither(_tmp48D,sizeof(char),7))));}goto _LL14F;_LL160: {
struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp243=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)
_tmp22F;if(_tmp243->tag != 7)goto _LL162;}_LL161: Cyc_Absyndump_dumploc(s->loc);{
const char*_tmp48E;Cyc_Absyndump_dump(((_tmp48E="continue;",_tag_dyneither(
_tmp48E,sizeof(char),10))));}goto _LL14F;_LL162: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*
_tmp244=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp22F;if(_tmp244->tag != 
8)goto _LL164;else{_tmp245=_tmp244->f1;}}_LL163: Cyc_Absyndump_dumploc(s->loc);{
const char*_tmp48F;Cyc_Absyndump_dump(((_tmp48F="goto",_tag_dyneither(_tmp48F,
sizeof(char),5))));}Cyc_Absyndump_dump_str(_tmp245);Cyc_Absyndump_dump_semi();
goto _LL14F;_LL164: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp246=(struct
Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp22F;if(_tmp246->tag != 9)goto _LL166;
else{_tmp247=_tmp246->f1;_tmp248=_tmp246->f2;_tmp249=_tmp248.f1;_tmp24A=_tmp246->f3;
_tmp24B=_tmp24A.f1;_tmp24C=_tmp246->f4;}}_LL165: Cyc_Absyndump_dumploc(s->loc);{
const char*_tmp490;Cyc_Absyndump_dump(((_tmp490="for(",_tag_dyneither(_tmp490,
sizeof(char),5))));}Cyc_Absyndump_dumpexp(_tmp247);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(
_tmp249);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp24B);{const char*
_tmp491;Cyc_Absyndump_dump_nospace(((_tmp491="){",_tag_dyneither(_tmp491,sizeof(
char),3))));}Cyc_Absyndump_dumpstmt(_tmp24C,0);Cyc_Absyndump_dump_char((int)'}');
goto _LL14F;_LL166: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp24D=(
struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp22F;if(_tmp24D->tag != 10)
goto _LL168;else{_tmp24E=_tmp24D->f1;_tmp24F=_tmp24D->f2;}}_LL167: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp492;Cyc_Absyndump_dump(((_tmp492="switch(",_tag_dyneither(
_tmp492,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp24E);{const char*_tmp493;
Cyc_Absyndump_dump_nospace(((_tmp493="){",_tag_dyneither(_tmp493,sizeof(char),3))));}
Cyc_Absyndump_dumpswitchclauses(_tmp24F);Cyc_Absyndump_dump_char((int)'}');goto
_LL14F;_LL168: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp250=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)
_tmp22F;if(_tmp250->tag != 12)goto _LL16A;else{_tmp251=_tmp250->f1;_tmp252=_tmp250->f2;}}
_LL169: Cyc_Absyndump_dumpdecl(_tmp251);Cyc_Absyndump_dumpstmt(_tmp252,expstmt);
goto _LL14F;_LL16A: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp253=(
struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp22F;if(_tmp253->tag != 13)goto
_LL16C;else{_tmp254=_tmp253->f1;_tmp255=_tmp253->f2;}}_LL16B: if(Cyc_Absynpp_is_declaration(
_tmp255)){Cyc_Absyndump_dump_str(_tmp254);if(expstmt){const char*_tmp494;Cyc_Absyndump_dump_nospace(((
_tmp494=": ({",_tag_dyneither(_tmp494,sizeof(char),5))));}else{const char*_tmp495;
Cyc_Absyndump_dump_nospace(((_tmp495=": {",_tag_dyneither(_tmp495,sizeof(char),4))));}
Cyc_Absyndump_dumpstmt(_tmp255,expstmt);if(expstmt){const char*_tmp496;Cyc_Absyndump_dump_nospace(((
_tmp496="});}",_tag_dyneither(_tmp496,sizeof(char),5))));}else{Cyc_Absyndump_dump_char((
int)'}');}}else{Cyc_Absyndump_dump_str(_tmp254);Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpstmt(_tmp255,expstmt);}goto _LL14F;_LL16C: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*
_tmp256=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp22F;if(_tmp256->tag != 
14)goto _LL16E;else{_tmp257=_tmp256->f1;_tmp258=_tmp256->f2;_tmp259=_tmp258.f1;}}
_LL16D: Cyc_Absyndump_dumploc(s->loc);{const char*_tmp497;Cyc_Absyndump_dump(((
_tmp497="do{",_tag_dyneither(_tmp497,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(
_tmp257,0);{const char*_tmp498;Cyc_Absyndump_dump_nospace(((_tmp498="}while(",
_tag_dyneither(_tmp498,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp259);{const
char*_tmp499;Cyc_Absyndump_dump_nospace(((_tmp499=");",_tag_dyneither(_tmp499,
sizeof(char),3))));}goto _LL14F;_LL16E: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*
_tmp25A=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp22F;if(_tmp25A->tag
!= 11)goto _LL170;else{_tmp25B=_tmp25A->f1;if(_tmp25B != 0)goto _LL170;}}_LL16F:{
const char*_tmp49A;Cyc_Absyndump_dump(((_tmp49A="fallthru;",_tag_dyneither(
_tmp49A,sizeof(char),10))));}goto _LL14F;_LL170: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*
_tmp25C=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp22F;if(_tmp25C->tag
!= 11)goto _LL172;else{_tmp25D=_tmp25C->f1;}}_LL171:{const char*_tmp49B;Cyc_Absyndump_dump(((
_tmp49B="fallthru(",_tag_dyneither(_tmp49B,sizeof(char),10))));}Cyc_Absyndump_dumpexps_prec(
20,_tmp25D);{const char*_tmp49C;Cyc_Absyndump_dump_nospace(((_tmp49C=");",
_tag_dyneither(_tmp49C,sizeof(char),3))));}goto _LL14F;_LL172: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*
_tmp25E=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp22F;if(_tmp25E->tag
!= 15)goto _LL174;else{_tmp25F=_tmp25E->f1;_tmp260=_tmp25E->f2;}}_LL173: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp49D;Cyc_Absyndump_dump(((_tmp49D="try",_tag_dyneither(
_tmp49D,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(_tmp25F,0);{const char*_tmp49E;
Cyc_Absyndump_dump(((_tmp49E="catch{",_tag_dyneither(_tmp49E,sizeof(char),7))));}
Cyc_Absyndump_dumpswitchclauses(_tmp260);Cyc_Absyndump_dump_char((int)'}');goto
_LL14F;_LL174: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp261=(
struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp22F;if(_tmp261->tag != 
16)goto _LL14F;else{_tmp262=_tmp261->f1;}}_LL175: Cyc_Absyndump_dumploc(s->loc);{
const char*_tmp49F;Cyc_Absyndump_dump(((_tmp49F="reset_region(",_tag_dyneither(
_tmp49F,sizeof(char),14))));}Cyc_Absyndump_dumpexp(_tmp262);{const char*_tmp4A0;
Cyc_Absyndump_dump(((_tmp4A0=");",_tag_dyneither(_tmp4A0,sizeof(char),3))));}
goto _LL14F;_LL14F:;}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;
};void Cyc_Absyndump_dumpdp(struct _tuple17*dp){{const char*_tmp4A3;const char*
_tmp4A2;const char*_tmp4A1;((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(*dp).f1,((_tmp4A1="",_tag_dyneither(_tmp4A1,
sizeof(char),1))),((_tmp4A2="=",_tag_dyneither(_tmp4A2,sizeof(char),2))),((
_tmp4A3="=",_tag_dyneither(_tmp4A3,sizeof(char),2))));}Cyc_Absyndump_dumppat((*
dp).f2);}void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){void*_tmp288=p->r;
enum Cyc_Absyn_Sign _tmp28C;int _tmp28D;enum Cyc_Absyn_Sign _tmp28F;int _tmp290;
enum Cyc_Absyn_Sign _tmp292;int _tmp293;char _tmp295;struct _dyneither_ptr _tmp297;
struct Cyc_Absyn_Vardecl*_tmp299;struct Cyc_Absyn_Pat*_tmp29A;struct Cyc_Absyn_Pat
_tmp29B;void*_tmp29C;struct Cyc_Absyn_Vardecl*_tmp29F;struct Cyc_Absyn_Pat*_tmp2A0;
struct Cyc_List_List*_tmp2A2;int _tmp2A3;struct Cyc_Absyn_Pat*_tmp2A5;struct Cyc_Absyn_Vardecl*
_tmp2A7;struct Cyc_Absyn_Pat*_tmp2A8;struct Cyc_Absyn_Pat _tmp2A9;void*_tmp2AA;
struct Cyc_Absyn_Vardecl*_tmp2AD;struct Cyc_Absyn_Pat*_tmp2AE;struct Cyc_Absyn_Tvar*
_tmp2B0;struct Cyc_Absyn_Vardecl*_tmp2B1;struct _tuple0*_tmp2B3;struct _tuple0*
_tmp2B5;struct Cyc_List_List*_tmp2B6;int _tmp2B7;struct Cyc_Absyn_AggrInfo*_tmp2B9;
struct Cyc_Absyn_AggrInfo _tmp2BA;union Cyc_Absyn_AggrInfoU _tmp2BB;struct Cyc_List_List*
_tmp2BC;struct Cyc_List_List*_tmp2BD;int _tmp2BE;struct Cyc_Absyn_AggrInfo*_tmp2C0;
struct Cyc_List_List*_tmp2C1;struct Cyc_List_List*_tmp2C2;int _tmp2C3;struct Cyc_Absyn_Datatypefield*
_tmp2C5;struct Cyc_List_List*_tmp2C6;int _tmp2C7;struct Cyc_Absyn_Enumfield*_tmp2C9;
struct Cyc_Absyn_Enumfield*_tmp2CB;struct Cyc_Absyn_Exp*_tmp2CD;_LL187: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*
_tmp289=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp288;if(_tmp289->tag != 
0)goto _LL189;}_LL188: Cyc_Absyndump_dump_char((int)'_');goto _LL186;_LL189: {
struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp28A=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)
_tmp288;if(_tmp28A->tag != 8)goto _LL18B;}_LL18A:{const char*_tmp4A4;Cyc_Absyndump_dump(((
_tmp4A4="NULL",_tag_dyneither(_tmp4A4,sizeof(char),5))));}goto _LL186;_LL18B: {
struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp28B=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)
_tmp288;if(_tmp28B->tag != 9)goto _LL18D;else{_tmp28C=_tmp28B->f1;if(_tmp28C != Cyc_Absyn_None)
goto _LL18D;_tmp28D=_tmp28B->f2;}}_LL18C: _tmp290=_tmp28D;goto _LL18E;_LL18D: {
struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp28E=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)
_tmp288;if(_tmp28E->tag != 9)goto _LL18F;else{_tmp28F=_tmp28E->f1;if(_tmp28F != Cyc_Absyn_Signed)
goto _LL18F;_tmp290=_tmp28E->f2;}}_LL18E:{const char*_tmp4A8;void*_tmp4A7[1];
struct Cyc_Int_pa_PrintArg_struct _tmp4A6;Cyc_Absyndump_dump((struct _dyneither_ptr)((
_tmp4A6.tag=1,((_tmp4A6.f1=(unsigned long)_tmp290,((_tmp4A7[0]=& _tmp4A6,Cyc_aprintf(((
_tmp4A8="%d",_tag_dyneither(_tmp4A8,sizeof(char),3))),_tag_dyneither(_tmp4A7,
sizeof(void*),1)))))))));}goto _LL186;_LL18F: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*
_tmp291=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp288;if(_tmp291->tag != 9)
goto _LL191;else{_tmp292=_tmp291->f1;if(_tmp292 != Cyc_Absyn_Unsigned)goto _LL191;
_tmp293=_tmp291->f2;}}_LL190:{const char*_tmp4AC;void*_tmp4AB[1];struct Cyc_Int_pa_PrintArg_struct
_tmp4AA;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp4AA.tag=1,((_tmp4AA.f1=(
unsigned int)_tmp293,((_tmp4AB[0]=& _tmp4AA,Cyc_aprintf(((_tmp4AC="%u",
_tag_dyneither(_tmp4AC,sizeof(char),3))),_tag_dyneither(_tmp4AB,sizeof(void*),1)))))))));}
goto _LL186;_LL191: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp294=(struct
Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp288;if(_tmp294->tag != 10)goto _LL193;
else{_tmp295=_tmp294->f1;}}_LL192:{const char*_tmp4AD;Cyc_Absyndump_dump(((
_tmp4AD="'",_tag_dyneither(_tmp4AD,sizeof(char),2))));}Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_char_escape(_tmp295));{const char*_tmp4AE;Cyc_Absyndump_dump_nospace(((
_tmp4AE="'",_tag_dyneither(_tmp4AE,sizeof(char),2))));}goto _LL186;_LL193: {
struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp296=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)
_tmp288;if(_tmp296->tag != 11)goto _LL195;else{_tmp297=_tmp296->f1;}}_LL194: Cyc_Absyndump_dump(
_tmp297);goto _LL186;_LL195: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp298=(
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp288;if(_tmp298->tag != 1)goto
_LL197;else{_tmp299=_tmp298->f1;_tmp29A=_tmp298->f2;_tmp29B=*_tmp29A;_tmp29C=
_tmp29B.r;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp29D=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)
_tmp29C;if(_tmp29D->tag != 0)goto _LL197;};}}_LL196: Cyc_Absyndump_dumpqvar(_tmp299->name);
goto _LL186;_LL197: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp29E=(struct
Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp288;if(_tmp29E->tag != 1)goto _LL199;
else{_tmp29F=_tmp29E->f1;_tmp2A0=_tmp29E->f2;}}_LL198: Cyc_Absyndump_dumpqvar(
_tmp29F->name);{const char*_tmp4AF;Cyc_Absyndump_dump(((_tmp4AF=" as ",
_tag_dyneither(_tmp4AF,sizeof(char),5))));}Cyc_Absyndump_dumppat(_tmp2A0);goto
_LL186;_LL199: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp2A1=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)
_tmp288;if(_tmp2A1->tag != 4)goto _LL19B;else{_tmp2A2=_tmp2A1->f1;_tmp2A3=_tmp2A1->f2;}}
_LL19A: {const char*_tmp4B1;const char*_tmp4B0;struct _dyneither_ptr term=_tmp2A3?(
_tmp4B1=", ...)",_tag_dyneither(_tmp4B1,sizeof(char),7)):((_tmp4B0=")",
_tag_dyneither(_tmp4B0,sizeof(char),2)));{const char*_tmp4B3;const char*_tmp4B2;((
void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp2A2,((_tmp4B2="$(",_tag_dyneither(_tmp4B2,sizeof(char),
3))),term,((_tmp4B3=",",_tag_dyneither(_tmp4B3,sizeof(char),2))));}goto _LL186;}
_LL19B: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp2A4=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)
_tmp288;if(_tmp2A4->tag != 5)goto _LL19D;else{_tmp2A5=_tmp2A4->f1;}}_LL19C:{const
char*_tmp4B4;Cyc_Absyndump_dump(((_tmp4B4="&",_tag_dyneither(_tmp4B4,sizeof(char),
2))));}Cyc_Absyndump_dumppat(_tmp2A5);goto _LL186;_LL19D: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*
_tmp2A6=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp288;if(_tmp2A6->tag
!= 2)goto _LL19F;else{_tmp2A7=_tmp2A6->f1;_tmp2A8=_tmp2A6->f2;_tmp2A9=*_tmp2A8;
_tmp2AA=_tmp2A9.r;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp2AB=(struct
Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp2AA;if(_tmp2AB->tag != 0)goto _LL19F;};}}
_LL19E:{const char*_tmp4B5;Cyc_Absyndump_dump(((_tmp4B5="*",_tag_dyneither(
_tmp4B5,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmp2A7->name);goto _LL186;
_LL19F: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp2AC=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)
_tmp288;if(_tmp2AC->tag != 2)goto _LL1A1;else{_tmp2AD=_tmp2AC->f1;_tmp2AE=_tmp2AC->f2;}}
_LL1A0:{const char*_tmp4B6;Cyc_Absyndump_dump(((_tmp4B6="*",_tag_dyneither(
_tmp4B6,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmp2AD->name);{const char*
_tmp4B7;Cyc_Absyndump_dump(((_tmp4B7=" as ",_tag_dyneither(_tmp4B7,sizeof(char),
5))));}Cyc_Absyndump_dumppat(_tmp2AE);goto _LL186;_LL1A1: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*
_tmp2AF=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp288;if(_tmp2AF->tag
!= 3)goto _LL1A3;else{_tmp2B0=_tmp2AF->f1;_tmp2B1=_tmp2AF->f2;}}_LL1A2: Cyc_Absyndump_dumpqvar(
_tmp2B1->name);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumptvar(_tmp2B0);
Cyc_Absyndump_dump_char((int)'>');goto _LL186;_LL1A3: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*
_tmp2B2=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp288;if(_tmp2B2->tag
!= 14)goto _LL1A5;else{_tmp2B3=_tmp2B2->f1;}}_LL1A4: Cyc_Absyndump_dumpqvar(
_tmp2B3);goto _LL186;_LL1A5: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*
_tmp2B4=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp288;if(_tmp2B4->tag
!= 15)goto _LL1A7;else{_tmp2B5=_tmp2B4->f1;_tmp2B6=_tmp2B4->f2;_tmp2B7=_tmp2B4->f3;}}
_LL1A6: {const char*_tmp4B9;const char*_tmp4B8;struct _dyneither_ptr term=_tmp2B7?(
_tmp4B9=", ...)",_tag_dyneither(_tmp4B9,sizeof(char),7)):((_tmp4B8=")",
_tag_dyneither(_tmp4B8,sizeof(char),2)));Cyc_Absyndump_dumpqvar(_tmp2B5);{const
char*_tmp4BB;const char*_tmp4BA;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct
Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct
_dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp2B6,((_tmp4BA="(",
_tag_dyneither(_tmp4BA,sizeof(char),2))),term,((_tmp4BB=",",_tag_dyneither(
_tmp4BB,sizeof(char),2))));}goto _LL186;}_LL1A7: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*
_tmp2B8=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp288;if(_tmp2B8->tag != 
6)goto _LL1A9;else{_tmp2B9=_tmp2B8->f1;if(_tmp2B9 == 0)goto _LL1A9;_tmp2BA=*_tmp2B9;
_tmp2BB=_tmp2BA.aggr_info;_tmp2BC=_tmp2B8->f2;_tmp2BD=_tmp2B8->f3;_tmp2BE=
_tmp2B8->f4;}}_LL1A8: {struct _tuple0*_tmp2E5;struct _tuple10 _tmp2E4=Cyc_Absyn_aggr_kinded_name(
_tmp2BB);_tmp2E5=_tmp2E4.f2;{const char*_tmp4BD;const char*_tmp4BC;struct
_dyneither_ptr term=_tmp2BE?(_tmp4BD=", ...)",_tag_dyneither(_tmp4BD,sizeof(char),
7)):((_tmp4BC=")",_tag_dyneither(_tmp4BC,sizeof(char),2)));Cyc_Absyndump_dumpqvar(
_tmp2E5);Cyc_Absyndump_dump_char((int)'{');{const char*_tmp4C0;const char*_tmp4BF;
const char*_tmp4BE;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptvar,_tmp2BC,((_tmp4BE="[",_tag_dyneither(_tmp4BE,sizeof(char),
2))),((_tmp4BF="]",_tag_dyneither(_tmp4BF,sizeof(char),2))),((_tmp4C0=",",
_tag_dyneither(_tmp4C0,sizeof(char),2))));}{const char*_tmp4C2;const char*_tmp4C1;((
void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr
start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,
_tmp2BD,((_tmp4C1="",_tag_dyneither(_tmp4C1,sizeof(char),1))),term,((_tmp4C2=",",
_tag_dyneither(_tmp4C2,sizeof(char),2))));}goto _LL186;};}_LL1A9: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*
_tmp2BF=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp288;if(_tmp2BF->tag != 
6)goto _LL1AB;else{_tmp2C0=_tmp2BF->f1;if(_tmp2C0 != 0)goto _LL1AB;_tmp2C1=_tmp2BF->f2;
_tmp2C2=_tmp2BF->f3;_tmp2C3=_tmp2BF->f4;}}_LL1AA: {const char*_tmp4C4;const char*
_tmp4C3;struct _dyneither_ptr term=_tmp2C3?(_tmp4C4=", ...)",_tag_dyneither(
_tmp4C4,sizeof(char),7)):((_tmp4C3=")",_tag_dyneither(_tmp4C3,sizeof(char),2)));
Cyc_Absyndump_dump_char((int)'{');{const char*_tmp4C7;const char*_tmp4C6;const char*
_tmp4C5;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptvar,_tmp2C1,((_tmp4C5="[",_tag_dyneither(_tmp4C5,sizeof(char),
2))),((_tmp4C6="]",_tag_dyneither(_tmp4C6,sizeof(char),2))),((_tmp4C7=",",
_tag_dyneither(_tmp4C7,sizeof(char),2))));}{const char*_tmp4C9;const char*_tmp4C8;((
void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr
start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,
_tmp2C2,((_tmp4C8="",_tag_dyneither(_tmp4C8,sizeof(char),1))),term,((_tmp4C9=",",
_tag_dyneither(_tmp4C9,sizeof(char),2))));}goto _LL186;}_LL1AB: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*
_tmp2C4=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp288;if(_tmp2C4->tag
!= 7)goto _LL1AD;else{_tmp2C5=_tmp2C4->f2;_tmp2C6=_tmp2C4->f3;_tmp2C7=_tmp2C4->f4;}}
_LL1AC: {const char*_tmp4CB;const char*_tmp4CA;struct _dyneither_ptr term=_tmp2C7?(
_tmp4CB=", ...)",_tag_dyneither(_tmp4CB,sizeof(char),7)):((_tmp4CA=")",
_tag_dyneither(_tmp4CA,sizeof(char),2)));Cyc_Absyndump_dumpqvar(_tmp2C5->name);
if(_tmp2C6 != 0){const char*_tmp4CD;const char*_tmp4CC;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct
_dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp2C6,((_tmp4CC="(",
_tag_dyneither(_tmp4CC,sizeof(char),2))),term,((_tmp4CD=",",_tag_dyneither(
_tmp4CD,sizeof(char),2))));}goto _LL186;}_LL1AD: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*
_tmp2C8=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp288;if(_tmp2C8->tag != 
12)goto _LL1AF;else{_tmp2C9=_tmp2C8->f2;}}_LL1AE: _tmp2CB=_tmp2C9;goto _LL1B0;
_LL1AF: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp2CA=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)
_tmp288;if(_tmp2CA->tag != 13)goto _LL1B1;else{_tmp2CB=_tmp2CA->f2;}}_LL1B0: Cyc_Absyndump_dumpqvar(
_tmp2CB->name);goto _LL186;_LL1B1: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*
_tmp2CC=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp288;if(_tmp2CC->tag != 
16)goto _LL186;else{_tmp2CD=_tmp2CC->f1;}}_LL1B2: Cyc_Absyndump_dumpexp(_tmp2CD);
goto _LL186;_LL186:;}void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*
ef){Cyc_Absyndump_dumpqvar(ef->name);if(ef->typs != 0)Cyc_Absyndump_dumpargs(ef->typs);}
void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*fields){const char*
_tmp4CE;((void(*)(void(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*l,
struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,
fields,((_tmp4CE=",",_tag_dyneither(_tmp4CE,sizeof(char),2))));}void Cyc_Absyndump_dumpenumfield(
struct Cyc_Absyn_Enumfield*ef){Cyc_Absyndump_dumpqvar(ef->name);if(ef->tag != 0){{
const char*_tmp4CF;Cyc_Absyndump_dump(((_tmp4CF=" = ",_tag_dyneither(_tmp4CF,
sizeof(char),4))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){const char*_tmp4D0;((
void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,struct
_dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,fields,((
_tmp4D0=",",_tag_dyneither(_tmp4D0,sizeof(char),2))));}void Cyc_Absyndump_dumpaggrfields(
struct Cyc_List_List*fields){for(0;fields != 0;fields=fields->tl){struct Cyc_Absyn_Aggrfield
_tmp2FC;struct _dyneither_ptr*_tmp2FD;struct Cyc_Absyn_Tqual _tmp2FE;void*_tmp2FF;
struct Cyc_Absyn_Exp*_tmp300;struct Cyc_List_List*_tmp301;struct Cyc_Absyn_Aggrfield*
_tmp2FB=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmp2FC=*_tmp2FB;_tmp2FD=_tmp2FC.name;
_tmp2FE=_tmp2FC.tq;_tmp2FF=_tmp2FC.type;_tmp300=_tmp2FC.width;_tmp301=_tmp2FC.attributes;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL1B3:((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(
_tmp2FE,_tmp2FF,Cyc_Absyndump_dump_str,_tmp2FD);Cyc_Absyndump_dumpatts(_tmp301);
break;case Cyc_Cyclone_Vc_c: _LL1B4: Cyc_Absyndump_dumpatts(_tmp301);((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))
Cyc_Absyndump_dumptqtd)(_tmp2FE,_tmp2FF,Cyc_Absyndump_dump_str,_tmp2FD);break;}
if(_tmp300 != 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp((struct
Cyc_Absyn_Exp*)_tmp300);}Cyc_Absyndump_dump_semi();}}void Cyc_Absyndump_dumptypedefname(
struct Cyc_Absyn_Typedefdecl*td){Cyc_Absyndump_dumpqvar(td->name);Cyc_Absyndump_dumptvars(
td->tvs);}static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dumpatts(
fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple18{void*f1;struct
_tuple0*f2;};static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple18*pr){{void*
_tmp302=(*pr).f1;_LL1B7: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*
_tmp303=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp302;if(_tmp303->tag
!= 11)goto _LL1B9;}_LL1B8: goto _LL1B6;_LL1B9: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*
_tmp304=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp302;if(_tmp304->tag
!= 1)goto _LL1BB;}_LL1BA:{const char*_tmp4D1;Cyc_Absyndump_dump(((_tmp4D1="_stdcall",
_tag_dyneither(_tmp4D1,sizeof(char),9))));}goto _LL1B6;_LL1BB: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*
_tmp305=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp302;if(_tmp305->tag
!= 2)goto _LL1BD;}_LL1BC:{const char*_tmp4D2;Cyc_Absyndump_dump(((_tmp4D2="_cdecl",
_tag_dyneither(_tmp4D2,sizeof(char),7))));}goto _LL1B6;_LL1BD: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*
_tmp306=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp302;if(_tmp306->tag
!= 3)goto _LL1BF;}_LL1BE:{const char*_tmp4D3;Cyc_Absyndump_dump(((_tmp4D3="_fastcall",
_tag_dyneither(_tmp4D3,sizeof(char),10))));}goto _LL1B6;_LL1BF:;_LL1C0: goto _LL1B6;
_LL1B6:;}Cyc_Absyndump_dumpqvar((*pr).f2);}static void Cyc_Absyndump_dump_callconv_fdqvar(
struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dump_callconv(fd->attributes);Cyc_Absyndump_dumpqvar(
fd->name);}static void Cyc_Absyndump_dumpids(struct Cyc_List_List*vds){for(0;vds != 
0;vds=vds->tl){Cyc_Absyndump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);
if(vds->tl != 0)Cyc_Absyndump_dump_char((int)',');}}void Cyc_Absyndump_dumpvardecl(
struct Cyc_Absyn_Vardecl*vd,unsigned int loc){struct Cyc_Absyn_Vardecl _tmp30B;enum 
Cyc_Absyn_Scope _tmp30C;struct _tuple0*_tmp30D;struct Cyc_Absyn_Tqual _tmp30E;void*
_tmp30F;struct Cyc_Absyn_Exp*_tmp310;struct Cyc_List_List*_tmp311;struct Cyc_Absyn_Vardecl*
_tmp30A=vd;_tmp30B=*_tmp30A;_tmp30C=_tmp30B.sc;_tmp30D=_tmp30B.name;_tmp30E=
_tmp30B.tq;_tmp30F=_tmp30B.type;_tmp310=_tmp30B.initializer;_tmp311=_tmp30B.attributes;
Cyc_Absyndump_dumploc(loc);switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c:
_LL1C1: if(_tmp30C == Cyc_Absyn_Extern  && Cyc_Absyndump_qvar_to_Cids){void*_tmp312=
Cyc_Tcutil_compress(_tmp30F);_LL1C4: {struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp313=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312;if(_tmp313->tag != 9)
goto _LL1C6;}_LL1C5: goto _LL1C3;_LL1C6:;_LL1C7: Cyc_Absyndump_dumpscope(_tmp30C);
_LL1C3:;}else{Cyc_Absyndump_dumpscope(_tmp30C);}((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp30E,
_tmp30F,Cyc_Absyndump_dumpqvar,_tmp30D);Cyc_Absyndump_dumpatts(_tmp311);break;
case Cyc_Cyclone_Vc_c: _LL1C2: Cyc_Absyndump_dumpatts(_tmp311);Cyc_Absyndump_dumpscope(
_tmp30C);{struct _RegionHandle _tmp314=_new_region("temp");struct _RegionHandle*
temp=& _tmp314;_push_region(temp);{struct Cyc_Absyn_Tqual _tmp316;void*_tmp317;
struct Cyc_List_List*_tmp318;struct _tuple11 _tmp315=Cyc_Absynpp_to_tms(temp,
_tmp30E,_tmp30F);_tmp316=_tmp315.f1;_tmp317=_tmp315.f2;_tmp318=_tmp315.f3;{void*
call_conv=(void*)& Cyc_Absyn_Unused_att_val;{struct Cyc_List_List*tms2=_tmp318;
for(0;tms2 != 0;tms2=tms2->tl){void*_tmp319=(void*)tms2->hd;struct Cyc_List_List*
_tmp31B;_LL1CA: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*
_tmp31A=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp319;if(
_tmp31A->tag != 5)goto _LL1CC;else{_tmp31B=_tmp31A->f2;}}_LL1CB: for(0;_tmp31B != 0;
_tmp31B=_tmp31B->tl){void*_tmp31C=(void*)_tmp31B->hd;_LL1CF: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*
_tmp31D=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp31C;if(_tmp31D->tag
!= 1)goto _LL1D1;}_LL1D0: call_conv=(void*)& Cyc_Absyn_Stdcall_att_val;goto _LL1CE;
_LL1D1: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp31E=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)
_tmp31C;if(_tmp31E->tag != 2)goto _LL1D3;}_LL1D2: call_conv=(void*)& Cyc_Absyn_Cdecl_att_val;
goto _LL1CE;_LL1D3: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp31F=(
struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp31C;if(_tmp31F->tag != 3)
goto _LL1D5;}_LL1D4: call_conv=(void*)& Cyc_Absyn_Fastcall_att_val;goto _LL1CE;
_LL1D5:;_LL1D6: goto _LL1CE;_LL1CE:;}goto _LL1C9;_LL1CC:;_LL1CD: goto _LL1C9;_LL1C9:;}}
Cyc_Absyndump_dumptq(_tmp316);Cyc_Absyndump_dumpntyp(_tmp317);{struct _tuple18
_tmp4D4;struct _tuple18 _tmp320=(_tmp4D4.f1=call_conv,((_tmp4D4.f2=_tmp30D,_tmp4D4)));((
void(*)(struct Cyc_List_List*tms,void(*f)(struct _tuple18*),struct _tuple18*a))Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp318),Cyc_Absyndump_dump_callconv_qvar,&
_tmp320);};};}_npop_handler(0);break;;_pop_region(temp);};}if(_tmp310 != 0){Cyc_Absyndump_dump_char((
int)'=');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp310);}Cyc_Absyndump_dump_semi();}
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*ad){Cyc_Absyndump_dumpscope(
ad->sc);Cyc_Absyndump_dumpaggr_kind(ad->kind);Cyc_Absyndump_dumpqvar(ad->name);
Cyc_Absyndump_dumptvars(ad->tvs);if(ad->impl == 0)return;else{Cyc_Absyndump_dump_char((
int)'{');if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 
0){const char*_tmp4D7;const char*_tmp4D6;const char*_tmp4D5;((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,((_tmp4D5="<",
_tag_dyneither(_tmp4D5,sizeof(char),2))),((_tmp4D6=">",_tag_dyneither(_tmp4D6,
sizeof(char),2))),((_tmp4D7=",",_tag_dyneither(_tmp4D7,sizeof(char),2))));}if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);}
Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);{
const char*_tmp4D8;Cyc_Absyndump_dump(((_tmp4D8="}",_tag_dyneither(_tmp4D8,
sizeof(char),2))));}Cyc_Absyndump_dumpatts(ad->attributes);}}static void Cyc_Absyndump_dump_enumdecl(
struct Cyc_Absyn_Enumdecl*ed){struct Cyc_Absyn_Enumdecl _tmp327;enum Cyc_Absyn_Scope
_tmp328;struct _tuple0*_tmp329;struct Cyc_Core_Opt*_tmp32A;struct Cyc_Absyn_Enumdecl*
_tmp326=ed;_tmp327=*_tmp326;_tmp328=_tmp327.sc;_tmp329=_tmp327.name;_tmp32A=
_tmp327.fields;Cyc_Absyndump_dumpscope(_tmp328);{const char*_tmp4D9;Cyc_Absyndump_dump(((
_tmp4D9="enum ",_tag_dyneither(_tmp4D9,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(
_tmp329);if(_tmp32A != 0){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpenumfields((
struct Cyc_List_List*)_tmp32A->v);{const char*_tmp4DA;Cyc_Absyndump_dump_nospace(((
_tmp4DA="}",_tag_dyneither(_tmp4DA,sizeof(char),2))));};}}static void Cyc_Absyndump_dump_datatypedecl(
struct Cyc_Absyn_Datatypedecl*dd){struct Cyc_Absyn_Datatypedecl _tmp32E;enum Cyc_Absyn_Scope
_tmp32F;struct _tuple0*_tmp330;struct Cyc_List_List*_tmp331;struct Cyc_Core_Opt*
_tmp332;int _tmp333;struct Cyc_Absyn_Datatypedecl*_tmp32D=dd;_tmp32E=*_tmp32D;
_tmp32F=_tmp32E.sc;_tmp330=_tmp32E.name;_tmp331=_tmp32E.tvs;_tmp332=_tmp32E.fields;
_tmp333=_tmp32E.is_extensible;Cyc_Absyndump_dumpscope(_tmp32F);if(_tmp333){const
char*_tmp4DB;Cyc_Absyndump_dump(((_tmp4DB="@extensible ",_tag_dyneither(_tmp4DB,
sizeof(char),13))));}{const char*_tmp4DC;Cyc_Absyndump_dump(((_tmp4DC="datatype ",
_tag_dyneither(_tmp4DC,sizeof(char),10))));}Cyc_Absyndump_dumpqvar(_tmp330);Cyc_Absyndump_dumptvars(
_tmp331);if(_tmp332 != 0){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpdatatypefields((
struct Cyc_List_List*)_tmp332->v);{const char*_tmp4DD;Cyc_Absyndump_dump_nospace(((
_tmp4DD="}",_tag_dyneither(_tmp4DD,sizeof(char),2))));};}}struct _tuple19{
unsigned int f1;struct _tuple0*f2;int f3;};void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*
d){void*_tmp337=d->r;struct Cyc_Absyn_Vardecl*_tmp339;struct Cyc_Absyn_Fndecl*
_tmp33B;struct Cyc_Absyn_Aggrdecl*_tmp33D;struct Cyc_Absyn_Datatypedecl*_tmp33F;
struct Cyc_Absyn_Enumdecl*_tmp341;struct Cyc_Absyn_Pat*_tmp343;struct Cyc_Absyn_Exp*
_tmp344;struct Cyc_List_List*_tmp346;struct Cyc_Absyn_Tvar*_tmp348;struct Cyc_Absyn_Vardecl*
_tmp349;int _tmp34A;struct Cyc_Absyn_Exp*_tmp34B;struct Cyc_Absyn_Exp*_tmp34D;
struct Cyc_Absyn_Tvar*_tmp34E;struct Cyc_Absyn_Vardecl*_tmp34F;struct Cyc_Absyn_Typedefdecl*
_tmp351;struct _dyneither_ptr*_tmp353;struct Cyc_List_List*_tmp354;struct _tuple0*
_tmp356;struct Cyc_List_List*_tmp357;struct Cyc_List_List*_tmp359;struct Cyc_List_List*
_tmp35B;struct Cyc_List_List*_tmp35C;_LL1D8: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*
_tmp338=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp337;if(_tmp338->tag != 
0)goto _LL1DA;else{_tmp339=_tmp338->f1;}}_LL1D9: Cyc_Absyndump_dumpvardecl(_tmp339,
d->loc);goto _LL1D7;_LL1DA: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp33A=(
struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp337;if(_tmp33A->tag != 1)goto
_LL1DC;else{_tmp33B=_tmp33A->f1;}}_LL1DB: Cyc_Absyndump_dumploc(d->loc);switch(
Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL1F8: Cyc_Absyndump_dumpatts(
_tmp33B->attributes);break;case Cyc_Cyclone_Gcc_c: _LL1F9: break;}if(_tmp33B->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp35F=Cyc_Cyclone_c_compiler;_LL1FC: if(_tmp35F != 
Cyc_Cyclone_Vc_c)goto _LL1FE;_LL1FD:{const char*_tmp4DE;Cyc_Absyndump_dump(((
_tmp4DE="__inline",_tag_dyneither(_tmp4DE,sizeof(char),9))));}goto _LL1FB;_LL1FE:;
_LL1FF:{const char*_tmp4DF;Cyc_Absyndump_dump(((_tmp4DF="inline",_tag_dyneither(
_tmp4DF,sizeof(char),7))));}goto _LL1FB;_LL1FB:;}Cyc_Absyndump_dumpscope(_tmp33B->sc);{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp4E5;struct Cyc_Absyn_FnInfo _tmp4E4;
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4E3;void*t=(void*)((_tmp4E3=
_cycalloc(sizeof(*_tmp4E3)),((_tmp4E3[0]=((_tmp4E5.tag=9,((_tmp4E5.f1=((_tmp4E4.tvars=
_tmp33B->tvs,((_tmp4E4.effect=_tmp33B->effect,((_tmp4E4.ret_tqual=_tmp33B->ret_tqual,((
_tmp4E4.ret_typ=_tmp33B->ret_type,((_tmp4E4.args=((struct Cyc_List_List*(*)(
struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,
_tmp33B->args),((_tmp4E4.c_varargs=_tmp33B->c_varargs,((_tmp4E4.cyc_varargs=
_tmp33B->cyc_varargs,((_tmp4E4.rgn_po=_tmp33B->rgn_po,((_tmp4E4.attributes=0,
_tmp4E4)))))))))))))))))),_tmp4E5)))),_tmp4E3))));switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL200:((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(
struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(
0),t,Cyc_Absyndump_dump_atts_qvar,_tmp33B);break;case Cyc_Cyclone_Vc_c: _LL201:((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))
Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_callconv_fdqvar,
_tmp33B);break;}Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp33B->body,
0);Cyc_Absyndump_dump_char((int)'}');goto _LL1D7;};_LL1DC: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*
_tmp33C=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp337;if(_tmp33C->tag != 
6)goto _LL1DE;else{_tmp33D=_tmp33C->f1;}}_LL1DD: Cyc_Absyndump_dump_aggrdecl(
_tmp33D);Cyc_Absyndump_dump_semi();goto _LL1D7;_LL1DE: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*
_tmp33E=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp337;if(_tmp33E->tag
!= 7)goto _LL1E0;else{_tmp33F=_tmp33E->f1;}}_LL1DF: Cyc_Absyndump_dump_datatypedecl(
_tmp33F);Cyc_Absyndump_dump_semi();goto _LL1D7;_LL1E0: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*
_tmp340=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp337;if(_tmp340->tag != 
8)goto _LL1E2;else{_tmp341=_tmp340->f1;}}_LL1E1: Cyc_Absyndump_dump_enumdecl(
_tmp341);Cyc_Absyndump_dump_semi();goto _LL1D7;_LL1E2: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*
_tmp342=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp337;if(_tmp342->tag != 
2)goto _LL1E4;else{_tmp343=_tmp342->f1;_tmp344=_tmp342->f3;}}_LL1E3:{const char*
_tmp4E6;Cyc_Absyndump_dump(((_tmp4E6="let",_tag_dyneither(_tmp4E6,sizeof(char),4))));}
Cyc_Absyndump_dumppat(_tmp343);Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(
_tmp344);Cyc_Absyndump_dump_semi();goto _LL1D7;_LL1E4: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*
_tmp345=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp337;if(_tmp345->tag != 
3)goto _LL1E6;else{_tmp346=_tmp345->f1;}}_LL1E5:{const char*_tmp4E7;Cyc_Absyndump_dump(((
_tmp4E7="let ",_tag_dyneither(_tmp4E7,sizeof(char),5))));}Cyc_Absyndump_dumpids(
_tmp346);Cyc_Absyndump_dump_semi();goto _LL1D7;_LL1E6: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*
_tmp347=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp337;if(_tmp347->tag
!= 4)goto _LL1E8;else{_tmp348=_tmp347->f1;_tmp349=_tmp347->f2;_tmp34A=_tmp347->f3;
_tmp34B=_tmp347->f4;}}_LL1E7:{const char*_tmp4E8;Cyc_Absyndump_dump(((_tmp4E8="region",
_tag_dyneither(_tmp4E8,sizeof(char),7))));}{const char*_tmp4E9;Cyc_Absyndump_dump(((
_tmp4E9="<",_tag_dyneither(_tmp4E9,sizeof(char),2))));}Cyc_Absyndump_dumptvar(
_tmp348);{const char*_tmp4EA;Cyc_Absyndump_dump(((_tmp4EA="> ",_tag_dyneither(
_tmp4EA,sizeof(char),3))));}Cyc_Absyndump_dumpqvar(_tmp349->name);if(_tmp34A){
const char*_tmp4EB;Cyc_Absyndump_dump(((_tmp4EB="@resetable",_tag_dyneither(
_tmp4EB,sizeof(char),11))));}if(_tmp34B != 0){{const char*_tmp4EC;Cyc_Absyndump_dump(((
_tmp4EC=" = open(",_tag_dyneither(_tmp4EC,sizeof(char),9))));}Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_tmp34B);{const char*_tmp4ED;Cyc_Absyndump_dump(((_tmp4ED=")",
_tag_dyneither(_tmp4ED,sizeof(char),2))));};}Cyc_Absyndump_dump_semi();goto
_LL1D7;_LL1E8: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp34C=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)
_tmp337;if(_tmp34C->tag != 5)goto _LL1EA;else{_tmp34D=_tmp34C->f1;_tmp34E=_tmp34C->f2;
_tmp34F=_tmp34C->f3;}}_LL1E9:{const char*_tmp4EE;Cyc_Absyndump_dump(((_tmp4EE="alias ",
_tag_dyneither(_tmp4EE,sizeof(char),7))));}{const char*_tmp4EF;Cyc_Absyndump_dump(((
_tmp4EF="<",_tag_dyneither(_tmp4EF,sizeof(char),2))));}Cyc_Absyndump_dumptvar(
_tmp34E);{const char*_tmp4F0;Cyc_Absyndump_dump(((_tmp4F0=">",_tag_dyneither(
_tmp4F0,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmp34F->name);{const char*
_tmp4F1;Cyc_Absyndump_dump(((_tmp4F1=" = ",_tag_dyneither(_tmp4F1,sizeof(char),4))));}
Cyc_Absyndump_dumpexp(_tmp34D);Cyc_Absyndump_dump_semi();goto _LL1D7;_LL1EA: {
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp350=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)
_tmp337;if(_tmp350->tag != 9)goto _LL1EC;else{_tmp351=_tmp350->f1;}}_LL1EB:{const
char*_tmp4F2;Cyc_Absyndump_dump(((_tmp4F2="typedef",_tag_dyneither(_tmp4F2,
sizeof(char),8))));}{void*t;if(_tmp351->defn == 0)t=Cyc_Absyn_new_evar(_tmp351->kind,
0);else{t=(void*)_check_null(_tmp351->defn);}((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(
_tmp351->tq,t,Cyc_Absyndump_dumptypedefname,_tmp351);Cyc_Absyndump_dumpatts(
_tmp351->atts);Cyc_Absyndump_dump_semi();goto _LL1D7;};_LL1EC: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*
_tmp352=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp337;if(_tmp352->tag
!= 10)goto _LL1EE;else{_tmp353=_tmp352->f1;_tmp354=_tmp352->f2;}}_LL1ED:{const
char*_tmp4F3;Cyc_Absyndump_dump(((_tmp4F3="namespace",_tag_dyneither(_tmp4F3,
sizeof(char),10))));}Cyc_Absyndump_dump_str(_tmp353);Cyc_Absyndump_dump_char((
int)'{');for(0;_tmp354 != 0;_tmp354=_tmp354->tl){Cyc_Absyndump_dumpdecl((struct
Cyc_Absyn_Decl*)_tmp354->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1D7;
_LL1EE: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp355=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)
_tmp337;if(_tmp355->tag != 11)goto _LL1F0;else{_tmp356=_tmp355->f1;_tmp357=_tmp355->f2;}}
_LL1EF:{const char*_tmp4F4;Cyc_Absyndump_dump(((_tmp4F4="using",_tag_dyneither(
_tmp4F4,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(_tmp356);Cyc_Absyndump_dump_char((
int)'{');for(0;_tmp357 != 0;_tmp357=_tmp357->tl){Cyc_Absyndump_dumpdecl((struct
Cyc_Absyn_Decl*)_tmp357->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1D7;
_LL1F0: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp358=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)
_tmp337;if(_tmp358->tag != 12)goto _LL1F2;else{_tmp359=_tmp358->f1;}}_LL1F1:{const
char*_tmp4F5;Cyc_Absyndump_dump(((_tmp4F5="extern \"C\" {",_tag_dyneither(
_tmp4F5,sizeof(char),13))));}for(0;_tmp359 != 0;_tmp359=_tmp359->tl){Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)_tmp359->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1D7;
_LL1F2: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp35A=(struct
Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp337;if(_tmp35A->tag != 13)
goto _LL1F4;else{_tmp35B=_tmp35A->f1;_tmp35C=_tmp35A->f2;}}_LL1F3:{const char*
_tmp4F6;Cyc_Absyndump_dump(((_tmp4F6="extern \"C include\" {",_tag_dyneither(
_tmp4F6,sizeof(char),21))));}for(0;_tmp35B != 0;_tmp35B=_tmp35B->tl){Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)_tmp35B->hd);}Cyc_Absyndump_dump_char((int)'}');if(_tmp35C
!= 0){{const char*_tmp4F7;Cyc_Absyndump_dump(((_tmp4F7=" export {",_tag_dyneither(
_tmp4F7,sizeof(char),10))));}for(0;_tmp35C != 0;_tmp35C=_tmp35C->tl){struct
_tuple0*_tmp378;struct _tuple19 _tmp377=*((struct _tuple19*)_tmp35C->hd);_tmp378=
_tmp377.f2;Cyc_Absyndump_dumpqvar(_tmp378);if(_tmp35C->tl != 0)Cyc_Absyndump_dump_char((
int)',');}{const char*_tmp4F8;Cyc_Absyndump_dump(((_tmp4F8="}",_tag_dyneither(
_tmp4F8,sizeof(char),2))));};}goto _LL1D7;_LL1F4: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*
_tmp35D=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp337;if(_tmp35D->tag
!= 14)goto _LL1F6;}_LL1F5:{const char*_tmp4F9;Cyc_Absyndump_dump(((_tmp4F9=" __cyclone_port_on__; ",
_tag_dyneither(_tmp4F9,sizeof(char),23))));}goto _LL1D7;_LL1F6: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*
_tmp35E=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp337;if(_tmp35E->tag
!= 15)goto _LL1D7;}_LL1F7:{const char*_tmp4FA;Cyc_Absyndump_dump(((_tmp4FA=" __cyclone_port_off__; ",
_tag_dyneither(_tmp4FA,sizeof(char),24))));}goto _LL1D7;_LL1D7:;}static void Cyc_Absyndump_dump_upperbound(
struct Cyc_Absyn_Exp*e){struct _tuple12 pr=Cyc_Evexp_eval_const_uint_exp(e);if(pr.f1
!= 1  || !pr.f2){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((
int)'}');}}void Cyc_Absyndump_dumptms(struct Cyc_List_List*tms,void(*f)(void*),
void*a){if(tms == 0){f(a);return;}{void*_tmp37C=(void*)tms->hd;struct Cyc_Absyn_PtrAtts
_tmp37E;void*_tmp37F;union Cyc_Absyn_Constraint*_tmp380;union Cyc_Absyn_Constraint*
_tmp381;union Cyc_Absyn_Constraint*_tmp382;struct Cyc_Absyn_Tqual _tmp383;_LL204: {
struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp37D=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)
_tmp37C;if(_tmp37D->tag != 2)goto _LL206;else{_tmp37E=_tmp37D->f1;_tmp37F=_tmp37E.rgn;
_tmp380=_tmp37E.nullable;_tmp381=_tmp37E.bounds;_tmp382=_tmp37E.zero_term;
_tmp383=_tmp37D->f2;}}_LL205:{void*_tmp384=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp381);struct Cyc_Absyn_Exp*
_tmp387;_LL209: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp385=(struct
Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp384;if(_tmp385->tag != 0)goto
_LL20B;}_LL20A: Cyc_Absyndump_dump_char((int)'?');goto _LL208;_LL20B: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp386=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp384;if(_tmp386->tag != 
1)goto _LL208;else{_tmp387=_tmp386->f1;}}_LL20C: Cyc_Absyndump_dump_char((int)(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp380)?'*':'@'));
Cyc_Absyndump_dump_upperbound(_tmp387);goto _LL208;_LL208:;}if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp382)){const char*_tmp4FB;
Cyc_Absyndump_dump(((_tmp4FB="@zeroterm",_tag_dyneither(_tmp4FB,sizeof(char),10))));}{
void*_tmp389=Cyc_Tcutil_compress(_tmp37F);struct Cyc_Absyn_Tvar*_tmp38E;void*
_tmp390;_LL20E: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp38A=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)
_tmp389;if(_tmp38A->tag != 20)goto _LL210;}_LL20F: goto _LL20D;_LL210: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*
_tmp38B=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp389;if(_tmp38B->tag != 
21)goto _LL212;}_LL211:{const char*_tmp4FC;Cyc_Absyndump_dump(((_tmp4FC="`U",
_tag_dyneither(_tmp4FC,sizeof(char),3))));}goto _LL20D;_LL212: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*
_tmp38C=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp389;if(_tmp38C->tag != 
22)goto _LL214;}_LL213:{const char*_tmp4FD;Cyc_Absyndump_dump(((_tmp4FD="`RC",
_tag_dyneither(_tmp4FD,sizeof(char),4))));}goto _LL20D;_LL214: {struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp38D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp389;if(_tmp38D->tag != 2)
goto _LL216;else{_tmp38E=_tmp38D->f1;}}_LL215: Cyc_Absyndump_dump_str(_tmp38E->name);
goto _LL20D;_LL216: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp38F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)
_tmp389;if(_tmp38F->tag != 1)goto _LL218;else{_tmp390=(void*)_tmp38F->f2;if(
_tmp390 != 0)goto _LL218;}}_LL217: Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(
_tmp37F));goto _LL20D;_LL218: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*
_tmp391=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp389;if(_tmp391->tag
!= 17)goto _LL21A;}_LL219:{const char*_tmp4FE;Cyc_Absyndump_dump(((_tmp4FE="@region(",
_tag_dyneither(_tmp4FE,sizeof(char),9))));}Cyc_Absyndump_dumptyp(_tmp37F);{const
char*_tmp4FF;Cyc_Absyndump_dump(((_tmp4FF=")",_tag_dyneither(_tmp4FF,sizeof(char),
2))));}goto _LL20D;_LL21A:;_LL21B: {const char*_tmp502;void*_tmp501;(_tmp501=0,Cyc_Tcutil_impos(((
_tmp502="dumptms: bad rgn type in Pointer_mod",_tag_dyneither(_tmp502,sizeof(
char),37))),_tag_dyneither(_tmp501,sizeof(void*),0)));}_LL20D:;}Cyc_Absyndump_dumptq(
_tmp383);Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL206:;_LL207: {int
next_is_pointer=0;if(tms->tl != 0){void*_tmp398=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;_LL21D: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*
_tmp399=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp398;if(
_tmp399->tag != 2)goto _LL21F;}_LL21E: next_is_pointer=1;goto _LL21C;_LL21F:;_LL220:
goto _LL21C;_LL21C:;}if(next_is_pointer)Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptms(
tms->tl,f,a);if(next_is_pointer)Cyc_Absyndump_dump_char((int)')');{void*_tmp39A=(
void*)tms->hd;union Cyc_Absyn_Constraint*_tmp39C;struct Cyc_Absyn_Exp*_tmp39E;
union Cyc_Absyn_Constraint*_tmp39F;void*_tmp3A1;struct Cyc_List_List*_tmp3A3;int
_tmp3A4;struct Cyc_Absyn_VarargInfo*_tmp3A5;void*_tmp3A6;struct Cyc_List_List*
_tmp3A7;void*_tmp3A9;struct Cyc_List_List*_tmp3AB;unsigned int _tmp3AC;struct Cyc_List_List*
_tmp3AE;unsigned int _tmp3AF;int _tmp3B0;struct Cyc_List_List*_tmp3B2;_LL222: {
struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp39B=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)
_tmp39A;if(_tmp39B->tag != 0)goto _LL224;else{_tmp39C=_tmp39B->f1;}}_LL223:{const
char*_tmp503;Cyc_Absyndump_dump(((_tmp503="[]",_tag_dyneither(_tmp503,sizeof(
char),3))));}if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp39C)){const char*_tmp504;Cyc_Absyndump_dump(((_tmp504="@zeroterm",
_tag_dyneither(_tmp504,sizeof(char),10))));}goto _LL221;_LL224: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*
_tmp39D=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp39A;if(
_tmp39D->tag != 1)goto _LL226;else{_tmp39E=_tmp39D->f1;_tmp39F=_tmp39D->f2;}}
_LL225: Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp39E);Cyc_Absyndump_dump_char((
int)']');if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp39F)){const char*_tmp505;Cyc_Absyndump_dump(((_tmp505="@zeroterm",
_tag_dyneither(_tmp505,sizeof(char),10))));}goto _LL221;_LL226: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*
_tmp3A0=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp39A;if(
_tmp3A0->tag != 3)goto _LL228;else{_tmp3A1=(void*)_tmp3A0->f1;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*
_tmp3A2=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp3A1;if(_tmp3A2->tag
!= 1)goto _LL228;else{_tmp3A3=_tmp3A2->f1;_tmp3A4=_tmp3A2->f2;_tmp3A5=_tmp3A2->f3;
_tmp3A6=(void*)_tmp3A2->f4;_tmp3A7=_tmp3A2->f5;}};}}_LL227: Cyc_Absyndump_dumpfunargs(
_tmp3A3,_tmp3A4,_tmp3A5,_tmp3A6,_tmp3A7);goto _LL221;_LL228: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*
_tmp3A8=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp39A;if(
_tmp3A8->tag != 3)goto _LL22A;else{_tmp3A9=(void*)_tmp3A8->f1;{struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*
_tmp3AA=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp3A9;if(_tmp3AA->tag
!= 0)goto _LL22A;else{_tmp3AB=_tmp3AA->f1;_tmp3AC=_tmp3AA->f2;}};}}_LL229:{const
char*_tmp508;const char*_tmp507;const char*_tmp506;((void(*)(void(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,
_tmp3AB,((_tmp506="(",_tag_dyneither(_tmp506,sizeof(char),2))),((_tmp507=")",
_tag_dyneither(_tmp507,sizeof(char),2))),((_tmp508=",",_tag_dyneither(_tmp508,
sizeof(char),2))));}goto _LL221;_LL22A: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*
_tmp3AD=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp39A;if(
_tmp3AD->tag != 4)goto _LL22C;else{_tmp3AE=_tmp3AD->f1;_tmp3AF=_tmp3AD->f2;_tmp3B0=
_tmp3AD->f3;}}_LL22B: if(_tmp3B0)Cyc_Absyndump_dumpkindedtvars(_tmp3AE);else{Cyc_Absyndump_dumptvars(
_tmp3AE);}goto _LL221;_LL22C: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*
_tmp3B1=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp39A;if(
_tmp3B1->tag != 5)goto _LL22E;else{_tmp3B2=_tmp3B1->f2;}}_LL22D: Cyc_Absyndump_dumpatts(
_tmp3B2);goto _LL221;_LL22E: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*
_tmp3B3=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp39A;if(
_tmp3B3->tag != 2)goto _LL221;}_LL22F: {const char*_tmp50B;void*_tmp50A;(_tmp50A=0,
Cyc_Tcutil_impos(((_tmp50B="dumptms",_tag_dyneither(_tmp50B,sizeof(char),8))),
_tag_dyneither(_tmp50A,sizeof(void*),0)));}_LL221:;}return;}_LL203:;};}void Cyc_Absyndump_dumptqtd(
struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){struct _RegionHandle
_tmp3BC=_new_region("temp");struct _RegionHandle*temp=& _tmp3BC;_push_region(temp);{
struct Cyc_Absyn_Tqual _tmp3BE;void*_tmp3BF;struct Cyc_List_List*_tmp3C0;struct
_tuple11 _tmp3BD=Cyc_Absynpp_to_tms(temp,tq,t);_tmp3BE=_tmp3BD.f1;_tmp3BF=_tmp3BD.f2;
_tmp3C0=_tmp3BD.f3;Cyc_Absyndump_dumptq(_tmp3BE);Cyc_Absyndump_dumpntyp(_tmp3BF);
Cyc_Absyndump_dumptms(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp3C0),f,a);};_pop_region(temp);}void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*
tdl,struct Cyc___cycFILE*f){Cyc_Absyndump_pos=0;*Cyc_Absyndump_dump_file=f;for(0;
tdl != 0;tdl=tdl->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}{
const char*_tmp50E;void*_tmp50D;(_tmp50D=0,Cyc_fprintf(f,((_tmp50E="\n",
_tag_dyneither(_tmp50E,sizeof(char),2))),_tag_dyneither(_tmp50D,sizeof(void*),0)));};}
