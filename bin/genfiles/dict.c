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
struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};
struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{
int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
typedef void*Cyc_parg_t;struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};
struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};
struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{
int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};
struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};typedef void*
Cyc_sarg_t;int Cyc_getw(struct Cyc___cycFILE*);extern char Cyc_FileCloseError[15];
struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];
struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};typedef
unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[
12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[
12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};inline static void* arrcast(
struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){if(bd >> 20  || sz >> 12)
return 0;{unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < dyn.curr  || dyn.curr == 
0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)return 0;return dyn.curr;};}
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};typedef struct Cyc_List_List*
Cyc_List_list_t;typedef struct Cyc_List_List*Cyc_List_List_t;int Cyc_List_length(
struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{
char*tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};typedef struct Cyc_Iter_Iter
Cyc_Iter_iter_t;int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};typedef struct Cyc_Dict_Dict
Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*
tag;};struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict
Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));struct Cyc_Dict_Dict
Cyc_Dict_rshare(struct _RegionHandle*,struct Cyc_Dict_Dict);int Cyc_Dict_is_empty(
struct Cyc_Dict_Dict d);int Cyc_Dict_cardinality(struct Cyc_Dict_Dict d);int Cyc_Dict_member(
struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict
d,void*k,void*v);struct Cyc_Dict_Dict Cyc_Dict_insert_new(struct Cyc_Dict_Dict d,
void*k,void*v);struct Cyc_Dict_Dict Cyc_Dict_inserts(struct Cyc_Dict_Dict d,struct
Cyc_List_List*l);struct Cyc_Dict_Dict Cyc_Dict_singleton(int(*cmp)(void*,void*),
void*k,void*v);struct Cyc_Dict_Dict Cyc_Dict_rsingleton(struct _RegionHandle*,int(*
cmp)(void*,void*),void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*
k);void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);int Cyc_Dict_lookup_bool(
struct Cyc_Dict_Dict d,void*k,void**ans);void*Cyc_Dict_fold(void*(*f)(void*,void*,
void*),struct Cyc_Dict_Dict d,void*accum);void*Cyc_Dict_fold_c(void*(*f)(void*,
void*,void*,void*),void*env,struct Cyc_Dict_Dict d,void*accum);void Cyc_Dict_app(
void*(*f)(void*,void*),struct Cyc_Dict_Dict d);void Cyc_Dict_app_c(void*(*f)(void*,
void*,void*),void*env,struct Cyc_Dict_Dict d);void Cyc_Dict_iter(void(*f)(void*,
void*),struct Cyc_Dict_Dict d);void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),
void*env,struct Cyc_Dict_Dict d);void Cyc_Dict_iter2(void(*f)(void*,void*),struct
Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);void Cyc_Dict_iter2_c(void(*f)(void*,void*,
void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);void*Cyc_Dict_fold2_c(
void*(*f)(void*,void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct
Cyc_Dict_Dict d2,void*accum);struct Cyc_Dict_Dict Cyc_Dict_rcopy(struct
_RegionHandle*,struct Cyc_Dict_Dict);struct Cyc_Dict_Dict Cyc_Dict_copy(struct Cyc_Dict_Dict);
struct Cyc_Dict_Dict Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict
Cyc_Dict_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_Dict_Dict d);struct
Cyc_Dict_Dict Cyc_Dict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d);
struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(
void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
struct Cyc_Dict_Dict Cyc_Dict_intersect(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2);struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,
void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);int Cyc_Dict_forall_c(
int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);int Cyc_Dict_forall_intersect(
int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct
_tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict d);struct Cyc_List_List*Cyc_Dict_to_list(struct Cyc_Dict_Dict d);
struct Cyc_List_List*Cyc_Dict_rto_list(struct _RegionHandle*,struct Cyc_Dict_Dict d);
struct Cyc_Dict_Dict Cyc_Dict_filter(int(*f)(void*,void*),struct Cyc_Dict_Dict d);
struct Cyc_Dict_Dict Cyc_Dict_rfilter(struct _RegionHandle*,int(*f)(void*,void*),
struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_filter_c(int(*f)(void*,void*,
void*),void*env,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(
struct _RegionHandle*,int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
struct Cyc_Dict_Dict Cyc_Dict_difference(struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2);struct Cyc_Dict_Dict Cyc_Dict_rdifference(struct _RegionHandle*,struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2);struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict,
void*);struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,
void*);struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict,void*);
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict
d);void*Cyc_Dict_marshal(struct _RegionHandle*rgn,void*env,void*(*write_key)(void*,
struct Cyc___cycFILE*,void*),void*(*write_val)(void*,struct Cyc___cycFILE*,void*),
struct Cyc___cycFILE*fp,struct Cyc_Dict_Dict dict);struct Cyc_Dict_Dict Cyc_Dict_unmarshal(
struct _RegionHandle*rgn,void*env,int(*cmp)(void*,void*),void*(*read_key)(void*,
struct Cyc___cycFILE*),void*(*read_val)(void*,struct Cyc___cycFILE*),struct Cyc___cycFILE*
fp);char Cyc_Dict_Absent[7]="Absent";char Cyc_Dict_Present[8]="Present";struct Cyc_Dict_Absent_exn_struct
Cyc_Dict_Absent_val={Cyc_Dict_Absent};struct Cyc_Dict_Present_exn_struct Cyc_Dict_Present_val={
Cyc_Dict_Present};enum Cyc_Dict_Color{Cyc_Dict_R  = 0,Cyc_Dict_B  = 1};struct Cyc_Dict_T{
enum Cyc_Dict_Color color;const struct Cyc_Dict_T*left;const struct Cyc_Dict_T*right;
struct _tuple0 key_val;};typedef const struct Cyc_Dict_T*Cyc_Dict_Tree_t;struct Cyc_Dict_Dict
Cyc_Dict_rempty(struct _RegionHandle*r,int(*comp)(void*,void*)){struct Cyc_Dict_Dict
_tmp12F;return(_tmp12F.rel=comp,((_tmp12F.r=r,((_tmp12F.t=0,_tmp12F)))));}struct
Cyc_Dict_Dict Cyc_Dict_empty(int(*comp)(void*,void*)){return Cyc_Dict_rempty(Cyc_Core_heap_region,
comp);}struct Cyc_Dict_Dict Cyc_Dict_rsingleton(struct _RegionHandle*r,int(*comp)(
void*,void*),void*key,void*data){struct Cyc_Dict_T*_tmp135;struct _tuple0 _tmp134;
struct Cyc_Dict_Dict _tmp133;return(_tmp133.rel=comp,((_tmp133.r=r,((_tmp133.t=(
const struct Cyc_Dict_T*)((_tmp135=_region_malloc(r,sizeof(*_tmp135)),((_tmp135->color=
Cyc_Dict_B,((_tmp135->left=0,((_tmp135->right=0,((_tmp135->key_val=((_tmp134.f1=
key,((_tmp134.f2=data,_tmp134)))),_tmp135)))))))))),_tmp133)))));}struct Cyc_Dict_Dict
Cyc_Dict_singleton(int(*comp)(void*,void*),void*key,void*data){return Cyc_Dict_rsingleton(
Cyc_Core_heap_region,comp,key,data);}struct Cyc_Dict_Dict Cyc_Dict_rshare(struct
_RegionHandle*r,struct Cyc_Dict_Dict d){const struct Cyc_Dict_T*t=d.t;const struct Cyc_Dict_T*
t2=(const struct Cyc_Dict_T*)t;struct Cyc_Dict_Dict _tmp136;return(_tmp136.rel=d.rel,((
_tmp136.r=r,((_tmp136.t=(const struct Cyc_Dict_T*)t2,_tmp136)))));}int Cyc_Dict_is_empty(
struct Cyc_Dict_Dict d){return d.t == (const struct Cyc_Dict_T*)0;}int Cyc_Dict_member(
struct Cyc_Dict_Dict d,void*key){int(*_tmp7)(void*,void*)=d.rel;const struct Cyc_Dict_T*
_tmp8=d.t;while(_tmp8 != (const struct Cyc_Dict_T*)0){int _tmp9=_tmp7(key,(_tmp8->key_val).f1);
if(_tmp9 < 0)_tmp8=_tmp8->left;else{if(_tmp9 > 0)_tmp8=_tmp8->right;else{return 1;}}}
return 0;}void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*key){int(*_tmpA)(void*,
void*)=d.rel;const struct Cyc_Dict_T*_tmpB=d.t;while(_tmpB != (const struct Cyc_Dict_T*)
0){int _tmpC=_tmpA(key,(_tmpB->key_val).f1);if(_tmpC < 0)_tmpB=_tmpB->left;else{
if(_tmpC > 0)_tmpB=_tmpB->right;else{return(_tmpB->key_val).f2;}}}(int)_throw((
void*)& Cyc_Dict_Absent_val);}void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*
cmp)(void*,void*),void*key){const struct Cyc_Dict_T*_tmpD=d.t;while(_tmpD != (const
struct Cyc_Dict_T*)0){int _tmpE=cmp(key,(_tmpD->key_val).f1);if(_tmpE < 0)_tmpD=
_tmpD->left;else{if(_tmpE > 0)_tmpD=_tmpD->right;else{return(_tmpD->key_val).f2;}}}(
int)_throw((void*)& Cyc_Dict_Absent_val);}void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict
d,void*key){int(*_tmpF)(void*,void*)=d.rel;const struct Cyc_Dict_T*_tmp10=d.t;
while(_tmp10 != (const struct Cyc_Dict_T*)0){int _tmp11=_tmpF(key,(_tmp10->key_val).f1);
if(_tmp11 < 0)_tmp10=_tmp10->left;else{if(_tmp11 > 0)_tmp10=_tmp10->right;else{
return(void**)&(_tmp10->key_val).f2;}}}return 0;}int Cyc_Dict_lookup_bool(struct
Cyc_Dict_Dict d,void*key,void**ans_place){int(*_tmp12)(void*,void*)=d.rel;const
struct Cyc_Dict_T*_tmp13=d.t;while(_tmp13 != (const struct Cyc_Dict_T*)0){int _tmp14=
_tmp12(key,(_tmp13->key_val).f1);if(_tmp14 < 0)_tmp13=_tmp13->left;else{if(_tmp14
> 0)_tmp13=_tmp13->right;else{*ans_place=(_tmp13->key_val).f2;return 1;}}}return 0;}
struct _tuple1{enum Cyc_Dict_Color f1;const struct Cyc_Dict_T*f2;const struct Cyc_Dict_T*
f3;struct _tuple0 f4;};static const struct Cyc_Dict_T*Cyc_Dict_balance(struct
_RegionHandle*r,struct _tuple1 quad){struct _tuple1 _tmp15=quad;enum Cyc_Dict_Color
_tmp16;const struct Cyc_Dict_T*_tmp17;struct Cyc_Dict_T _tmp18;enum Cyc_Dict_Color
_tmp19;const struct Cyc_Dict_T*_tmp1A;struct Cyc_Dict_T _tmp1B;enum Cyc_Dict_Color
_tmp1C;const struct Cyc_Dict_T*_tmp1D;const struct Cyc_Dict_T*_tmp1E;struct _tuple0
_tmp1F;const struct Cyc_Dict_T*_tmp20;struct _tuple0 _tmp21;const struct Cyc_Dict_T*
_tmp22;struct _tuple0 _tmp23;enum Cyc_Dict_Color _tmp24;const struct Cyc_Dict_T*
_tmp25;struct Cyc_Dict_T _tmp26;enum Cyc_Dict_Color _tmp27;const struct Cyc_Dict_T*
_tmp28;const struct Cyc_Dict_T*_tmp29;struct Cyc_Dict_T _tmp2A;enum Cyc_Dict_Color
_tmp2B;const struct Cyc_Dict_T*_tmp2C;const struct Cyc_Dict_T*_tmp2D;struct _tuple0
_tmp2E;struct _tuple0 _tmp2F;const struct Cyc_Dict_T*_tmp30;struct _tuple0 _tmp31;
enum Cyc_Dict_Color _tmp32;const struct Cyc_Dict_T*_tmp33;const struct Cyc_Dict_T*
_tmp34;struct Cyc_Dict_T _tmp35;enum Cyc_Dict_Color _tmp36;const struct Cyc_Dict_T*
_tmp37;struct Cyc_Dict_T _tmp38;enum Cyc_Dict_Color _tmp39;const struct Cyc_Dict_T*
_tmp3A;const struct Cyc_Dict_T*_tmp3B;struct _tuple0 _tmp3C;const struct Cyc_Dict_T*
_tmp3D;struct _tuple0 _tmp3E;struct _tuple0 _tmp3F;enum Cyc_Dict_Color _tmp40;const
struct Cyc_Dict_T*_tmp41;const struct Cyc_Dict_T*_tmp42;struct Cyc_Dict_T _tmp43;
enum Cyc_Dict_Color _tmp44;const struct Cyc_Dict_T*_tmp45;const struct Cyc_Dict_T*
_tmp46;struct Cyc_Dict_T _tmp47;enum Cyc_Dict_Color _tmp48;const struct Cyc_Dict_T*
_tmp49;const struct Cyc_Dict_T*_tmp4A;struct _tuple0 _tmp4B;struct _tuple0 _tmp4C;
struct _tuple0 _tmp4D;enum Cyc_Dict_Color _tmp4E;const struct Cyc_Dict_T*_tmp4F;const
struct Cyc_Dict_T*_tmp50;struct _tuple0 _tmp51;_LL1: _tmp16=_tmp15.f1;if(_tmp16 != 
Cyc_Dict_B)goto _LL3;_tmp17=_tmp15.f2;if(_tmp17 == 0)goto _LL3;_tmp18=*_tmp17;
_tmp19=_tmp18.color;if(_tmp19 != Cyc_Dict_R)goto _LL3;_tmp1A=_tmp18.left;if(_tmp1A
== 0)goto _LL3;_tmp1B=*_tmp1A;_tmp1C=_tmp1B.color;if(_tmp1C != Cyc_Dict_R)goto _LL3;
_tmp1D=_tmp1B.left;_tmp1E=_tmp1B.right;_tmp1F=_tmp1B.key_val;_tmp20=_tmp18.right;
_tmp21=_tmp18.key_val;_tmp22=_tmp15.f3;_tmp23=_tmp15.f4;_LL2: {struct Cyc_Dict_T*
_tmp13B;struct Cyc_Dict_T*_tmp13A;struct Cyc_Dict_T*_tmp139;return(const struct Cyc_Dict_T*)((
_tmp139=_region_malloc(r,sizeof(*_tmp139)),((_tmp139->color=Cyc_Dict_R,((_tmp139->left=(
const struct Cyc_Dict_T*)((_tmp13A=_region_malloc(r,sizeof(*_tmp13A)),((_tmp13A->color=
Cyc_Dict_B,((_tmp13A->left=_tmp1D,((_tmp13A->right=_tmp1E,((_tmp13A->key_val=
_tmp1F,_tmp13A)))))))))),((_tmp139->right=(const struct Cyc_Dict_T*)((_tmp13B=
_region_malloc(r,sizeof(*_tmp13B)),((_tmp13B->color=Cyc_Dict_B,((_tmp13B->left=
_tmp20,((_tmp13B->right=_tmp22,((_tmp13B->key_val=_tmp23,_tmp13B)))))))))),((
_tmp139->key_val=_tmp21,_tmp139))))))))));}_LL3: _tmp24=_tmp15.f1;if(_tmp24 != Cyc_Dict_B)
goto _LL5;_tmp25=_tmp15.f2;if(_tmp25 == 0)goto _LL5;_tmp26=*_tmp25;_tmp27=_tmp26.color;
if(_tmp27 != Cyc_Dict_R)goto _LL5;_tmp28=_tmp26.left;_tmp29=_tmp26.right;if(_tmp29
== 0)goto _LL5;_tmp2A=*_tmp29;_tmp2B=_tmp2A.color;if(_tmp2B != Cyc_Dict_R)goto _LL5;
_tmp2C=_tmp2A.left;_tmp2D=_tmp2A.right;_tmp2E=_tmp2A.key_val;_tmp2F=_tmp26.key_val;
_tmp30=_tmp15.f3;_tmp31=_tmp15.f4;_LL4: {struct Cyc_Dict_T*_tmp140;struct Cyc_Dict_T*
_tmp13F;struct Cyc_Dict_T*_tmp13E;return(const struct Cyc_Dict_T*)((_tmp13E=
_region_malloc(r,sizeof(*_tmp13E)),((_tmp13E->color=Cyc_Dict_R,((_tmp13E->left=(
const struct Cyc_Dict_T*)((_tmp13F=_region_malloc(r,sizeof(*_tmp13F)),((_tmp13F->color=
Cyc_Dict_B,((_tmp13F->left=_tmp28,((_tmp13F->right=_tmp2C,((_tmp13F->key_val=
_tmp2F,_tmp13F)))))))))),((_tmp13E->right=(const struct Cyc_Dict_T*)((_tmp140=
_region_malloc(r,sizeof(*_tmp140)),((_tmp140->color=Cyc_Dict_B,((_tmp140->left=
_tmp2D,((_tmp140->right=_tmp30,((_tmp140->key_val=_tmp31,_tmp140)))))))))),((
_tmp13E->key_val=_tmp2E,_tmp13E))))))))));}_LL5: _tmp32=_tmp15.f1;if(_tmp32 != Cyc_Dict_B)
goto _LL7;_tmp33=_tmp15.f2;_tmp34=_tmp15.f3;if(_tmp34 == 0)goto _LL7;_tmp35=*_tmp34;
_tmp36=_tmp35.color;if(_tmp36 != Cyc_Dict_R)goto _LL7;_tmp37=_tmp35.left;if(_tmp37
== 0)goto _LL7;_tmp38=*_tmp37;_tmp39=_tmp38.color;if(_tmp39 != Cyc_Dict_R)goto _LL7;
_tmp3A=_tmp38.left;_tmp3B=_tmp38.right;_tmp3C=_tmp38.key_val;_tmp3D=_tmp35.right;
_tmp3E=_tmp35.key_val;_tmp3F=_tmp15.f4;_LL6: {struct Cyc_Dict_T*_tmp145;struct Cyc_Dict_T*
_tmp144;struct Cyc_Dict_T*_tmp143;return(const struct Cyc_Dict_T*)((_tmp143=
_region_malloc(r,sizeof(*_tmp143)),((_tmp143->color=Cyc_Dict_R,((_tmp143->left=(
const struct Cyc_Dict_T*)((_tmp144=_region_malloc(r,sizeof(*_tmp144)),((_tmp144->color=
Cyc_Dict_B,((_tmp144->left=_tmp33,((_tmp144->right=_tmp3A,((_tmp144->key_val=
_tmp3F,_tmp144)))))))))),((_tmp143->right=(const struct Cyc_Dict_T*)((_tmp145=
_region_malloc(r,sizeof(*_tmp145)),((_tmp145->color=Cyc_Dict_B,((_tmp145->left=
_tmp3B,((_tmp145->right=_tmp3D,((_tmp145->key_val=_tmp3E,_tmp145)))))))))),((
_tmp143->key_val=_tmp3C,_tmp143))))))))));}_LL7: _tmp40=_tmp15.f1;if(_tmp40 != Cyc_Dict_B)
goto _LL9;_tmp41=_tmp15.f2;_tmp42=_tmp15.f3;if(_tmp42 == 0)goto _LL9;_tmp43=*_tmp42;
_tmp44=_tmp43.color;if(_tmp44 != Cyc_Dict_R)goto _LL9;_tmp45=_tmp43.left;_tmp46=
_tmp43.right;if(_tmp46 == 0)goto _LL9;_tmp47=*_tmp46;_tmp48=_tmp47.color;if(_tmp48
!= Cyc_Dict_R)goto _LL9;_tmp49=_tmp47.left;_tmp4A=_tmp47.right;_tmp4B=_tmp47.key_val;
_tmp4C=_tmp43.key_val;_tmp4D=_tmp15.f4;_LL8: {struct Cyc_Dict_T*_tmp14A;struct Cyc_Dict_T*
_tmp149;struct Cyc_Dict_T*_tmp148;return(const struct Cyc_Dict_T*)((_tmp148=
_region_malloc(r,sizeof(*_tmp148)),((_tmp148->color=Cyc_Dict_R,((_tmp148->left=(
const struct Cyc_Dict_T*)((_tmp149=_region_malloc(r,sizeof(*_tmp149)),((_tmp149->color=
Cyc_Dict_B,((_tmp149->left=_tmp41,((_tmp149->right=_tmp45,((_tmp149->key_val=
_tmp4D,_tmp149)))))))))),((_tmp148->right=(const struct Cyc_Dict_T*)((_tmp14A=
_region_malloc(r,sizeof(*_tmp14A)),((_tmp14A->color=Cyc_Dict_B,((_tmp14A->left=
_tmp49,((_tmp14A->right=_tmp4A,((_tmp14A->key_val=_tmp4B,_tmp14A)))))))))),((
_tmp148->key_val=_tmp4C,_tmp148))))))))));}_LL9: _tmp4E=_tmp15.f1;_tmp4F=_tmp15.f2;
_tmp50=_tmp15.f3;_tmp51=_tmp15.f4;_LLA: {struct Cyc_Dict_T*_tmp14B;return(const
struct Cyc_Dict_T*)((_tmp14B=_region_malloc(r,sizeof(*_tmp14B)),((_tmp14B->color=
_tmp4E,((_tmp14B->left=_tmp4F,((_tmp14B->right=_tmp50,((_tmp14B->key_val=_tmp51,
_tmp14B))))))))));}_LL0:;}static const struct Cyc_Dict_T*Cyc_Dict_ins(struct
_RegionHandle*r,int(*rel)(void*,void*),struct _tuple0 key_val,const struct Cyc_Dict_T*
t){const struct Cyc_Dict_T*_tmp5F=t;struct Cyc_Dict_T _tmp60;enum Cyc_Dict_Color
_tmp61;const struct Cyc_Dict_T*_tmp62;const struct Cyc_Dict_T*_tmp63;struct _tuple0
_tmp64;_LLC: if(_tmp5F != 0)goto _LLE;_LLD: {struct Cyc_Dict_T*_tmp14C;return(const
struct Cyc_Dict_T*)((_tmp14C=_region_malloc(r,sizeof(*_tmp14C)),((_tmp14C->color=
Cyc_Dict_R,((_tmp14C->left=0,((_tmp14C->right=0,((_tmp14C->key_val=key_val,
_tmp14C))))))))));}_LLE: if(_tmp5F == 0)goto _LLB;_tmp60=*_tmp5F;_tmp61=_tmp60.color;
_tmp62=_tmp60.left;_tmp63=_tmp60.right;_tmp64=_tmp60.key_val;_LLF: {int _tmp66=
rel(key_val.f1,_tmp64.f1);if(_tmp66 < 0){struct _tuple1 _tmp14D;return Cyc_Dict_balance(
r,((_tmp14D.f1=_tmp61,((_tmp14D.f2=Cyc_Dict_ins(r,rel,key_val,_tmp62),((_tmp14D.f3=
_tmp63,((_tmp14D.f4=_tmp64,_tmp14D)))))))));}else{if(_tmp66 > 0){struct _tuple1
_tmp14E;return Cyc_Dict_balance(r,((_tmp14E.f1=_tmp61,((_tmp14E.f2=_tmp62,((
_tmp14E.f3=Cyc_Dict_ins(r,rel,key_val,_tmp63),((_tmp14E.f4=_tmp64,_tmp14E)))))))));}
else{struct Cyc_Dict_T*_tmp14F;return(const struct Cyc_Dict_T*)((_tmp14F=
_region_malloc(r,sizeof(*_tmp14F)),((_tmp14F->color=_tmp61,((_tmp14F->left=
_tmp62,((_tmp14F->right=_tmp63,((_tmp14F->key_val=key_val,_tmp14F))))))))));}}}
_LLB:;}struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*key,void*
data){struct _tuple0 _tmp150;const struct Cyc_Dict_T*_tmp6A=Cyc_Dict_ins(d.r,d.rel,((
_tmp150.f1=key,((_tmp150.f2=data,_tmp150)))),d.t);((const struct Cyc_Dict_T*)
_check_null(_tmp6A))->color=Cyc_Dict_B;{struct Cyc_Dict_Dict _tmp151;struct Cyc_Dict_Dict
_tmp6B=(_tmp151.rel=d.rel,((_tmp151.r=d.r,((_tmp151.t=_tmp6A,_tmp151)))));return
_tmp6B;};}struct Cyc_Dict_Dict Cyc_Dict_insert_new(struct Cyc_Dict_Dict d,void*key,
void*data){if(Cyc_Dict_member(d,key))(int)_throw((void*)& Cyc_Dict_Absent_val);
return Cyc_Dict_insert(d,key,data);}struct Cyc_Dict_Dict Cyc_Dict_inserts(struct Cyc_Dict_Dict
d,struct Cyc_List_List*kds){for(0;kds != 0;kds=kds->tl){d=Cyc_Dict_insert(d,(((
struct _tuple0*)kds->hd)[0]).f1,(((struct _tuple0*)kds->hd)[0]).f2);}return d;}
static void*Cyc_Dict_fold_tree(void*(*f)(void*,void*,void*),const struct Cyc_Dict_T*
t,void*accum){struct Cyc_Dict_T _tmp6F;const struct Cyc_Dict_T*_tmp70;const struct Cyc_Dict_T*
_tmp71;struct _tuple0 _tmp72;void*_tmp73;void*_tmp74;const struct Cyc_Dict_T*_tmp6E=
t;_tmp6F=*_tmp6E;_tmp70=_tmp6F.left;_tmp71=_tmp6F.right;_tmp72=_tmp6F.key_val;
_tmp73=_tmp72.f1;_tmp74=_tmp72.f2;if(_tmp70 != (const struct Cyc_Dict_T*)0)accum=
Cyc_Dict_fold_tree(f,(const struct Cyc_Dict_T*)_tmp70,accum);accum=f(_tmp73,_tmp74,
accum);if(_tmp71 != (const struct Cyc_Dict_T*)0)accum=Cyc_Dict_fold_tree(f,(const
struct Cyc_Dict_T*)_tmp71,accum);return accum;}void*Cyc_Dict_fold(void*(*f)(void*,
void*,void*),struct Cyc_Dict_Dict d,void*accum){const struct Cyc_Dict_T*_tmp75=d.t;
if(_tmp75 == (const struct Cyc_Dict_T*)0)return accum;return Cyc_Dict_fold_tree(f,(
const struct Cyc_Dict_T*)_tmp75,accum);}static void*Cyc_Dict_fold_tree_c(void*(*f)(
void*,void*,void*,void*),void*env,const struct Cyc_Dict_T*t,void*accum){struct Cyc_Dict_T
_tmp77;const struct Cyc_Dict_T*_tmp78;const struct Cyc_Dict_T*_tmp79;struct _tuple0
_tmp7A;void*_tmp7B;void*_tmp7C;const struct Cyc_Dict_T*_tmp76=t;_tmp77=*_tmp76;
_tmp78=_tmp77.left;_tmp79=_tmp77.right;_tmp7A=_tmp77.key_val;_tmp7B=_tmp7A.f1;
_tmp7C=_tmp7A.f2;if(_tmp78 != (const struct Cyc_Dict_T*)0)accum=Cyc_Dict_fold_tree_c(
f,env,(const struct Cyc_Dict_T*)_tmp78,accum);accum=f(env,_tmp7B,_tmp7C,accum);if(
_tmp79 != (const struct Cyc_Dict_T*)0)accum=Cyc_Dict_fold_tree_c(f,env,(const struct
Cyc_Dict_T*)_tmp79,accum);return accum;}void*Cyc_Dict_fold_c(void*(*f)(void*,void*,
void*,void*),void*env,struct Cyc_Dict_Dict d,void*accum){const struct Cyc_Dict_T*
_tmp7D=d.t;if(_tmp7D == (const struct Cyc_Dict_T*)0)return accum;return Cyc_Dict_fold_tree_c(
f,env,(const struct Cyc_Dict_T*)_tmp7D,accum);}static void Cyc_Dict_app_tree(void*(*
f)(void*,void*),const struct Cyc_Dict_T*t){struct Cyc_Dict_T _tmp7F;const struct Cyc_Dict_T*
_tmp80;const struct Cyc_Dict_T*_tmp81;struct _tuple0 _tmp82;void*_tmp83;void*_tmp84;
const struct Cyc_Dict_T*_tmp7E=t;_tmp7F=*_tmp7E;_tmp80=_tmp7F.left;_tmp81=_tmp7F.right;
_tmp82=_tmp7F.key_val;_tmp83=_tmp82.f1;_tmp84=_tmp82.f2;if(_tmp80 != (const struct
Cyc_Dict_T*)0)Cyc_Dict_app_tree(f,(const struct Cyc_Dict_T*)_tmp80);f(_tmp83,
_tmp84);if(_tmp81 != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree(f,(const struct
Cyc_Dict_T*)_tmp81);}void Cyc_Dict_app(void*(*f)(void*,void*),struct Cyc_Dict_Dict
d){const struct Cyc_Dict_T*_tmp85=d.t;if(_tmp85 != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree(
f,(const struct Cyc_Dict_T*)_tmp85);}static void Cyc_Dict_app_tree_c(void*(*f)(void*,
void*,void*),void*env,const struct Cyc_Dict_T*t){struct Cyc_Dict_T _tmp87;const
struct Cyc_Dict_T*_tmp88;const struct Cyc_Dict_T*_tmp89;struct _tuple0 _tmp8A;void*
_tmp8B;void*_tmp8C;const struct Cyc_Dict_T*_tmp86=t;_tmp87=*_tmp86;_tmp88=_tmp87.left;
_tmp89=_tmp87.right;_tmp8A=_tmp87.key_val;_tmp8B=_tmp8A.f1;_tmp8C=_tmp8A.f2;if(
_tmp88 != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree_c(f,env,(const struct Cyc_Dict_T*)
_tmp88);f(env,_tmp8B,_tmp8C);if(_tmp89 != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree_c(
f,env,(const struct Cyc_Dict_T*)_tmp89);}void Cyc_Dict_app_c(void*(*f)(void*,void*,
void*),void*env,struct Cyc_Dict_Dict d){const struct Cyc_Dict_T*_tmp8D=d.t;if(_tmp8D
!= (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree_c(f,env,(const struct Cyc_Dict_T*)
_tmp8D);}static void Cyc_Dict_iter_tree(void(*f)(void*,void*),const struct Cyc_Dict_T*
t){struct Cyc_Dict_T _tmp8F;const struct Cyc_Dict_T*_tmp90;const struct Cyc_Dict_T*
_tmp91;struct _tuple0 _tmp92;void*_tmp93;void*_tmp94;const struct Cyc_Dict_T*_tmp8E=
t;_tmp8F=*_tmp8E;_tmp90=_tmp8F.left;_tmp91=_tmp8F.right;_tmp92=_tmp8F.key_val;
_tmp93=_tmp92.f1;_tmp94=_tmp92.f2;if(_tmp90 != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree(
f,(const struct Cyc_Dict_T*)_tmp90);f(_tmp93,_tmp94);if(_tmp91 != (const struct Cyc_Dict_T*)
0)Cyc_Dict_iter_tree(f,(const struct Cyc_Dict_T*)_tmp91);}void Cyc_Dict_iter(void(*
f)(void*,void*),struct Cyc_Dict_Dict d){const struct Cyc_Dict_T*_tmp95=d.t;if(_tmp95
!= (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree(f,(const struct Cyc_Dict_T*)_tmp95);}
static void Cyc_Dict_iter_tree_c(void(*f)(void*,void*,void*),void*env,const struct
Cyc_Dict_T*t){struct Cyc_Dict_T _tmp97;const struct Cyc_Dict_T*_tmp98;const struct Cyc_Dict_T*
_tmp99;struct _tuple0 _tmp9A;void*_tmp9B;void*_tmp9C;const struct Cyc_Dict_T*_tmp96=
t;_tmp97=*_tmp96;_tmp98=_tmp97.left;_tmp99=_tmp97.right;_tmp9A=_tmp97.key_val;
_tmp9B=_tmp9A.f1;_tmp9C=_tmp9A.f2;if(_tmp98 != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree_c(
f,env,(const struct Cyc_Dict_T*)_tmp98);f(env,_tmp9B,_tmp9C);if(_tmp99 != (const
struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree_c(f,env,(const struct Cyc_Dict_T*)_tmp99);}
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp9D=d.t;if(_tmp9D != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree_c(
f,env,(const struct Cyc_Dict_T*)_tmp9D);}static void Cyc_Dict_count_elem(int*cnt,
void*a,void*b){*cnt=*cnt + 1;}int Cyc_Dict_cardinality(struct Cyc_Dict_Dict d){int
num=0;((void(*)(void(*f)(int*,void*,void*),int*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_Dict_count_elem,& num,d);return num;}struct _tuple2{void(*f1)(void*,void*);
struct Cyc_Dict_Dict f2;};static void Cyc_Dict_iter2_f(struct _tuple2*env,void*a,void*
b1){struct _tuple2 _tmp9F;void(*_tmpA0)(void*,void*);struct Cyc_Dict_Dict _tmpA1;
struct _tuple2*_tmp9E=env;_tmp9F=*_tmp9E;_tmpA0=_tmp9F.f1;_tmpA1=_tmp9F.f2;_tmpA0(
b1,Cyc_Dict_lookup(_tmpA1,a));}void Cyc_Dict_iter2(void(*f)(void*,void*),struct
Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){struct _tuple2 _tmp152;struct _tuple2 _tmpA2=(
_tmp152.f1=f,((_tmp152.f2=d2,_tmp152)));((void(*)(void(*f)(struct _tuple2*,void*,
void*),struct _tuple2*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Dict_iter2_f,&
_tmpA2,d1);}struct _tuple3{void(*f1)(void*,void*,void*);struct Cyc_Dict_Dict f2;
void*f3;};static void Cyc_Dict_iter2_c_f(struct _tuple3*env,void*a,void*b1){struct
_tuple3 _tmpA5;void(*_tmpA6)(void*,void*,void*);struct Cyc_Dict_Dict _tmpA7;void*
_tmpA8;struct _tuple3*_tmpA4=env;_tmpA5=*_tmpA4;_tmpA6=_tmpA5.f1;_tmpA7=_tmpA5.f2;
_tmpA8=_tmpA5.f3;_tmpA6(_tmpA8,b1,Cyc_Dict_lookup(_tmpA7,a));}void Cyc_Dict_iter2_c(
void(*f)(void*,void*,void*),void*inner_env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2){struct _tuple3 _tmp153;struct _tuple3 _tmpA9=(_tmp153.f1=f,((_tmp153.f2=d2,((
_tmp153.f3=inner_env,_tmp153)))));((void(*)(void(*f)(struct _tuple3*,void*,void*),
struct _tuple3*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Dict_iter2_c_f,&
_tmpA9,d1);}struct _tuple4{void*(*f1)(void*,void*,void*,void*,void*);struct Cyc_Dict_Dict
f2;void*f3;};static void*Cyc_Dict_fold2_c_f(struct _tuple4*env,void*a,void*b1,void*
accum){struct _tuple4 _tmpAC;void*(*_tmpAD)(void*,void*,void*,void*,void*);struct
Cyc_Dict_Dict _tmpAE;void*_tmpAF;struct _tuple4*_tmpAB=env;_tmpAC=*_tmpAB;_tmpAD=
_tmpAC.f1;_tmpAE=_tmpAC.f2;_tmpAF=_tmpAC.f3;return _tmpAD(_tmpAF,a,b1,Cyc_Dict_lookup(
_tmpAE,a),accum);}void*Cyc_Dict_fold2_c(void*(*f)(void*,void*,void*,void*,void*),
void*inner_env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2,void*accum){struct
_tuple4 _tmp154;struct _tuple4 _tmpB0=(_tmp154.f1=f,((_tmp154.f2=d2,((_tmp154.f3=
inner_env,_tmp154)))));return((void*(*)(void*(*f)(struct _tuple4*,void*,void*,
void*),struct _tuple4*env,struct Cyc_Dict_Dict d,void*accum))Cyc_Dict_fold_c)(Cyc_Dict_fold2_c_f,&
_tmpB0,d1,accum);}static const struct Cyc_Dict_T*Cyc_Dict_copy_tree(struct
_RegionHandle*r2,const struct Cyc_Dict_T*t){if(t == (const struct Cyc_Dict_T*)0)
return 0;else{enum Cyc_Dict_Color _tmpB3;const struct Cyc_Dict_T*_tmpB4;const struct
Cyc_Dict_T*_tmpB5;struct _tuple0 _tmpB6;struct Cyc_Dict_T _tmpB2=*t;_tmpB3=_tmpB2.color;
_tmpB4=_tmpB2.left;_tmpB5=_tmpB2.right;_tmpB6=_tmpB2.key_val;{const struct Cyc_Dict_T*
_tmpB7=Cyc_Dict_copy_tree(r2,_tmpB4);const struct Cyc_Dict_T*_tmpB8=Cyc_Dict_copy_tree(
r2,_tmpB5);struct Cyc_Dict_T*_tmp155;return(const struct Cyc_Dict_T*)((_tmp155=
_region_malloc(r2,sizeof(*_tmp155)),((_tmp155->color=_tmpB3,((_tmp155->left=
_tmpB7,((_tmp155->right=_tmpB8,((_tmp155->key_val=_tmpB6,_tmp155))))))))));};}}
struct Cyc_Dict_Dict Cyc_Dict_rcopy(struct _RegionHandle*r2,struct Cyc_Dict_Dict d){
struct Cyc_Dict_Dict _tmp156;return(_tmp156.rel=d.rel,((_tmp156.r=r2,((_tmp156.t=
Cyc_Dict_copy_tree(r2,d.t),_tmp156)))));}struct Cyc_Dict_Dict Cyc_Dict_copy(struct
Cyc_Dict_Dict d){return Cyc_Dict_rcopy(Cyc_Core_heap_region,d);}static const struct
Cyc_Dict_T*Cyc_Dict_map_tree(struct _RegionHandle*r,void*(*f)(void*),const struct
Cyc_Dict_T*t){struct Cyc_Dict_T _tmpBC;enum Cyc_Dict_Color _tmpBD;const struct Cyc_Dict_T*
_tmpBE;const struct Cyc_Dict_T*_tmpBF;struct _tuple0 _tmpC0;void*_tmpC1;void*_tmpC2;
const struct Cyc_Dict_T*_tmpBB=t;_tmpBC=*_tmpBB;_tmpBD=_tmpBC.color;_tmpBE=_tmpBC.left;
_tmpBF=_tmpBC.right;_tmpC0=_tmpBC.key_val;_tmpC1=_tmpC0.f1;_tmpC2=_tmpC0.f2;{
const struct Cyc_Dict_T*_tmpC3=_tmpBE == (const struct Cyc_Dict_T*)0?(const struct Cyc_Dict_T*)
0: Cyc_Dict_map_tree(r,f,(const struct Cyc_Dict_T*)_tmpBE);void*_tmpC4=f(_tmpC2);
const struct Cyc_Dict_T*_tmpC5=_tmpBF == (const struct Cyc_Dict_T*)0?(const struct Cyc_Dict_T*)
0: Cyc_Dict_map_tree(r,f,(const struct Cyc_Dict_T*)_tmpBF);struct _tuple0 _tmp159;
struct Cyc_Dict_T*_tmp158;return(const struct Cyc_Dict_T*)((_tmp158=_region_malloc(
r,sizeof(*_tmp158)),((_tmp158->color=_tmpBD,((_tmp158->left=_tmpC3,((_tmp158->right=
_tmpC5,((_tmp158->key_val=((_tmp159.f1=_tmpC1,((_tmp159.f2=_tmpC4,_tmp159)))),
_tmp158))))))))));};}struct Cyc_Dict_Dict Cyc_Dict_rmap(struct _RegionHandle*r,void*(*
f)(void*),struct Cyc_Dict_Dict d){const struct Cyc_Dict_T*_tmpC8=d.t;if(_tmpC8 == (
const struct Cyc_Dict_T*)0){struct Cyc_Dict_Dict _tmp15A;return(_tmp15A.rel=d.rel,((
_tmp15A.r=r,((_tmp15A.t=0,_tmp15A)))));}{struct Cyc_Dict_Dict _tmp15B;return(
_tmp15B.rel=d.rel,((_tmp15B.r=r,((_tmp15B.t=Cyc_Dict_map_tree(r,f,(const struct
Cyc_Dict_T*)_tmpC8),_tmp15B)))));};}struct Cyc_Dict_Dict Cyc_Dict_map(void*(*f)(
void*),struct Cyc_Dict_Dict d){return Cyc_Dict_rmap(Cyc_Core_heap_region,f,d);}
static const struct Cyc_Dict_T*Cyc_Dict_map_tree_c(struct _RegionHandle*r,void*(*f)(
void*,void*),void*env,const struct Cyc_Dict_T*t){struct Cyc_Dict_T _tmpCC;enum Cyc_Dict_Color
_tmpCD;const struct Cyc_Dict_T*_tmpCE;const struct Cyc_Dict_T*_tmpCF;struct _tuple0
_tmpD0;void*_tmpD1;void*_tmpD2;const struct Cyc_Dict_T*_tmpCB=t;_tmpCC=*_tmpCB;
_tmpCD=_tmpCC.color;_tmpCE=_tmpCC.left;_tmpCF=_tmpCC.right;_tmpD0=_tmpCC.key_val;
_tmpD1=_tmpD0.f1;_tmpD2=_tmpD0.f2;{const struct Cyc_Dict_T*_tmpD3=_tmpCE == (const
struct Cyc_Dict_T*)0?(const struct Cyc_Dict_T*)0: Cyc_Dict_map_tree_c(r,f,env,(const
struct Cyc_Dict_T*)_tmpCE);void*_tmpD4=f(env,_tmpD2);const struct Cyc_Dict_T*_tmpD5=
_tmpCF == (const struct Cyc_Dict_T*)0?(const struct Cyc_Dict_T*)0: Cyc_Dict_map_tree_c(
r,f,env,(const struct Cyc_Dict_T*)_tmpCF);struct _tuple0 _tmp15E;struct Cyc_Dict_T*
_tmp15D;return(const struct Cyc_Dict_T*)((_tmp15D=_region_malloc(r,sizeof(*_tmp15D)),((
_tmp15D->color=_tmpCD,((_tmp15D->left=_tmpD3,((_tmp15D->right=_tmpD5,((_tmp15D->key_val=((
_tmp15E.f1=_tmpD1,((_tmp15E.f2=_tmpD4,_tmp15E)))),_tmp15D))))))))));};}struct Cyc_Dict_Dict
Cyc_Dict_rmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict
d){const struct Cyc_Dict_T*_tmpD8=d.t;if(_tmpD8 == (const struct Cyc_Dict_T*)0){
struct Cyc_Dict_Dict _tmp15F;return(_tmp15F.rel=d.rel,((_tmp15F.r=r,((_tmp15F.t=0,
_tmp15F)))));}{struct Cyc_Dict_Dict _tmp160;return(_tmp160.rel=d.rel,((_tmp160.r=r,((
_tmp160.t=Cyc_Dict_map_tree_c(r,f,env,(const struct Cyc_Dict_T*)_tmpD8),_tmp160)))));};}
struct Cyc_Dict_Dict Cyc_Dict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict
d){return Cyc_Dict_rmap_c(Cyc_Core_heap_region,f,env,d);}struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d){if(d.t == (const struct Cyc_Dict_T*)0)(
int)_throw((void*)& Cyc_Dict_Absent_val);{struct _tuple0*_tmp161;return(_tmp161=
_region_malloc(r,sizeof(*_tmp161)),((_tmp161->f1=((d.t)->key_val).f1,((_tmp161->f2=((
d.t)->key_val).f2,_tmp161)))));};}static int Cyc_Dict_forall_tree_c(int(*f)(void*,
void*,void*),void*env,const struct Cyc_Dict_T*t){struct Cyc_Dict_T _tmpDD;const
struct Cyc_Dict_T*_tmpDE;const struct Cyc_Dict_T*_tmpDF;struct _tuple0 _tmpE0;void*
_tmpE1;void*_tmpE2;const struct Cyc_Dict_T*_tmpDC=t;_tmpDD=*_tmpDC;_tmpDE=_tmpDD.left;
_tmpDF=_tmpDD.right;_tmpE0=_tmpDD.key_val;_tmpE1=_tmpE0.f1;_tmpE2=_tmpE0.f2;
return((_tmpDE == (const struct Cyc_Dict_T*)0  || Cyc_Dict_forall_tree_c(f,env,(
const struct Cyc_Dict_T*)_tmpDE)) && f(env,_tmpE1,_tmpE2)) && (_tmpDF == (const
struct Cyc_Dict_T*)0  || Cyc_Dict_forall_tree_c(f,env,(const struct Cyc_Dict_T*)
_tmpDF));}int Cyc_Dict_forall_c(int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict
d){const struct Cyc_Dict_T*_tmpE3=d.t;if(_tmpE3 == (const struct Cyc_Dict_T*)0)return
1;return Cyc_Dict_forall_tree_c(f,env,(const struct Cyc_Dict_T*)_tmpE3);}struct
_tuple5{int(*f1)(void*,void*,void*);struct Cyc_Dict_Dict f2;};static int Cyc_Dict_forall_intersect_f(
struct _tuple5*env,void*a,void*b){struct _tuple5 _tmpE5;int(*_tmpE6)(void*,void*,
void*);struct Cyc_Dict_Dict _tmpE7;struct _tuple5*_tmpE4=env;_tmpE5=*_tmpE4;_tmpE6=
_tmpE5.f1;_tmpE7=_tmpE5.f2;if(Cyc_Dict_member(_tmpE7,a))return _tmpE6(a,b,Cyc_Dict_lookup(
_tmpE7,a));return 1;}int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),
struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){struct _tuple5 _tmp162;struct _tuple5
_tmpE8=(_tmp162.f1=f,((_tmp162.f2=d2,_tmp162)));return((int(*)(int(*f)(struct
_tuple5*,void*,void*),struct _tuple5*env,struct Cyc_Dict_Dict d))Cyc_Dict_forall_c)(
Cyc_Dict_forall_intersect_f,& _tmpE8,d1);}struct _tuple6{void*(*f1)(void*,void*,
void*,void*);void*f2;};static struct Cyc_Dict_Dict*Cyc_Dict_union_f(struct _tuple6*
env,void*a,void*b,struct Cyc_Dict_Dict*d1){if(Cyc_Dict_member(*d1,a)){void*_tmpEA=
Cyc_Dict_lookup(*d1,a);void*_tmpEB=((*env).f1)((*env).f2,a,_tmpEA,b);if(_tmpEB != 
_tmpEA)*d1=Cyc_Dict_insert(*d1,a,_tmpEB);return d1;}*d1=Cyc_Dict_insert(*d1,a,b);
return d1;}struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,
void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){if((int)d1.t == (int)
d2.t)return d1;if(d1.t == (const struct Cyc_Dict_T*)0)return d2;if(d2.t == (const
struct Cyc_Dict_T*)0)return d1;{struct _tuple6 _tmp163;struct _tuple6 _tmpEC=(_tmp163.f1=
f,((_tmp163.f2=env,_tmp163)));((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*f)(
struct _tuple6*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple6*env,struct Cyc_Dict_Dict
d,struct Cyc_Dict_Dict*accum))Cyc_Dict_fold_c)(Cyc_Dict_union_f,& _tmpEC,d2,& d1);
return d1;};}struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,
void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);static void _tmp168(
const struct Cyc_Dict_T**_tmpEE,unsigned int*_tmp167,unsigned int*_tmp166,const
struct Cyc_Dict_T***_tmp165){for(*_tmp167=0;*_tmp167 < *_tmp166;(*_tmp167)++){(*
_tmp165)[*_tmp167]=(const struct Cyc_Dict_T*)*_tmpEE;}}static void _tmp16E(struct
_dyneither_ptr*queue,const struct Cyc_Dict_T**_tmpEE,unsigned int*_tmp16D,
unsigned int*_tmp16C,const struct Cyc_Dict_T***_tmp16A){for(*_tmp16D=0;*_tmp16D < *
_tmp16C;(*_tmp16D)++){(*_tmp16A)[*_tmp16D]=*_tmp16D < _get_dyneither_size(*queue,
sizeof(const struct Cyc_Dict_T*))?((const struct Cyc_Dict_T**)(*queue).curr)[(int)*
_tmp16D]:(const struct Cyc_Dict_T*)*_tmpEE;}}struct Cyc_Dict_Dict Cyc_Dict_intersect_c(
void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2){const struct Cyc_Dict_T*_tmpEE=d2.t;if(_tmpEE == (const struct Cyc_Dict_T*)0)
return d2;if((int)d1.t == (int)_tmpEE)return d2;{const struct Cyc_Dict_T*ans_tree=0;
struct _RegionHandle _tmpEF=_new_region("temp");struct _RegionHandle*temp=& _tmpEF;
_push_region(temp);{unsigned int _tmp167;unsigned int _tmp166;const struct Cyc_Dict_T**
_tmp165;unsigned int _tmp164;struct _dyneither_ptr queue=_tag_dyneither(((_tmp164=(
unsigned int)16,((_tmp165=(const struct Cyc_Dict_T**)_region_malloc(temp,
_check_times(sizeof(const struct Cyc_Dict_T*),_tmp164)),((((_tmp166=_tmp164,
_tmp168(& _tmpEE,& _tmp167,& _tmp166,& _tmp165))),_tmp165)))))),sizeof(const struct
Cyc_Dict_T*),(unsigned int)16);int ind=0;while(ind != - 1){struct Cyc_Dict_T _tmpF1;
const struct Cyc_Dict_T*_tmpF2;const struct Cyc_Dict_T*_tmpF3;struct _tuple0 _tmpF4;
void*_tmpF5;void*_tmpF6;const struct Cyc_Dict_T*_tmpF0=*((const struct Cyc_Dict_T**)
_check_dyneither_subscript(queue,sizeof(const struct Cyc_Dict_T*),ind --));_tmpF1=*
_tmpF0;_tmpF2=_tmpF1.left;_tmpF3=_tmpF1.right;_tmpF4=_tmpF1.key_val;_tmpF5=
_tmpF4.f1;_tmpF6=_tmpF4.f2;if(ind + 2 >= _get_dyneither_size(queue,sizeof(const
struct Cyc_Dict_T*))){unsigned int _tmp16D;unsigned int _tmp16C;struct
_dyneither_ptr _tmp16B;const struct Cyc_Dict_T**_tmp16A;unsigned int _tmp169;queue=((
_tmp169=_get_dyneither_size(queue,sizeof(const struct Cyc_Dict_T*))* 2,((_tmp16A=(
const struct Cyc_Dict_T**)_region_malloc(temp,_check_times(sizeof(const struct Cyc_Dict_T*),
_tmp169)),((_tmp16B=_tag_dyneither(_tmp16A,sizeof(const struct Cyc_Dict_T*),
_tmp169),((((_tmp16C=_tmp169,_tmp16E(& queue,& _tmpEE,& _tmp16D,& _tmp16C,& _tmp16A))),
_tmp16B))))))));}if(_tmpF2 != (const struct Cyc_Dict_T*)0)*((const struct Cyc_Dict_T**)
_check_dyneither_subscript(queue,sizeof(const struct Cyc_Dict_T*),++ ind))=(const
struct Cyc_Dict_T*)_tmpF2;if(_tmpF3 != (const struct Cyc_Dict_T*)0)*((const struct Cyc_Dict_T**)
_check_dyneither_subscript(queue,sizeof(const struct Cyc_Dict_T*),++ ind))=(const
struct Cyc_Dict_T*)_tmpF3;if(Cyc_Dict_member(d1,_tmpF5)){struct _tuple0 _tmp16F;
ans_tree=Cyc_Dict_ins(d2.r,d2.rel,((_tmp16F.f1=_tmpF5,((_tmp16F.f2=f(env,_tmpF5,
Cyc_Dict_lookup(d1,_tmpF5),_tmpF6),_tmp16F)))),ans_tree);}}}{struct Cyc_Dict_Dict
_tmp170;struct Cyc_Dict_Dict _tmp100=(_tmp170.rel=d2.rel,((_tmp170.r=d2.r,((
_tmp170.t=ans_tree,_tmp170)))));_npop_handler(0);return _tmp100;};;_pop_region(
temp);};}static void*Cyc_Dict_intersect_f(void*(*f)(void*,void*,void*),void*a,
void*b1,void*b2){return f(a,b1,b2);}struct Cyc_Dict_Dict Cyc_Dict_intersect(void*(*
f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){return((
struct Cyc_Dict_Dict(*)(void*(*f)(void*(*)(void*,void*,void*),void*,void*,void*),
void*(*env)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)(
Cyc_Dict_intersect_f,f,d1,d2);}static struct Cyc_List_List*Cyc_Dict_to_list_f(
struct _RegionHandle*r,void*k,void*v,struct Cyc_List_List*accum){struct _tuple0*
_tmp173;struct Cyc_List_List*_tmp172;return(_tmp172=_region_malloc(r,sizeof(*
_tmp172)),((_tmp172->hd=((_tmp173=_region_malloc(r,sizeof(*_tmp173)),((_tmp173->f1=
k,((_tmp173->f2=v,_tmp173)))))),((_tmp172->tl=accum,_tmp172)))));}struct Cyc_List_List*
Cyc_Dict_rto_list(struct _RegionHandle*r,struct Cyc_Dict_Dict d){return((struct Cyc_List_List*(*)(
struct Cyc_List_List*(*f)(struct _RegionHandle*,void*,void*,struct Cyc_List_List*),
struct _RegionHandle*env,struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold_c)(
Cyc_Dict_to_list_f,r,d,0);}struct Cyc_List_List*Cyc_Dict_to_list(struct Cyc_Dict_Dict
d){return Cyc_Dict_rto_list(Cyc_Core_heap_region,d);}struct _tuple7{int(*f1)(void*,
void*);struct _RegionHandle*f2;};static struct Cyc_Dict_Dict*Cyc_Dict_filter_f(
struct _tuple7*env,void*x,void*y,struct Cyc_Dict_Dict*acc){struct _tuple7 _tmp104;
int(*_tmp105)(void*,void*);struct _RegionHandle*_tmp106;struct _tuple7*_tmp103=env;
_tmp104=*_tmp103;_tmp105=_tmp104.f1;_tmp106=_tmp104.f2;if(_tmp105(x,y))*acc=Cyc_Dict_insert(*
acc,x,y);return acc;}struct Cyc_Dict_Dict Cyc_Dict_rfilter(struct _RegionHandle*r2,
int(*f)(void*,void*),struct Cyc_Dict_Dict d){struct _tuple7 _tmp174;struct _tuple7
_tmp107=(_tmp174.f1=f,((_tmp174.f2=r2,_tmp174)));struct Cyc_Dict_Dict _tmp108=Cyc_Dict_rempty(
r2,d.rel);return*((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*f)(struct
_tuple7*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple7*env,struct Cyc_Dict_Dict
d,struct Cyc_Dict_Dict*accum))Cyc_Dict_fold_c)(Cyc_Dict_filter_f,& _tmp107,d,&
_tmp108);}struct Cyc_Dict_Dict Cyc_Dict_filter(int(*f)(void*,void*),struct Cyc_Dict_Dict
d){return Cyc_Dict_rfilter(Cyc_Core_heap_region,f,d);}struct _tuple8{int(*f1)(void*,
void*,void*);void*f2;struct _RegionHandle*f3;};static struct Cyc_Dict_Dict*Cyc_Dict_filter_c_f(
struct _tuple8*env,void*x,void*y,struct Cyc_Dict_Dict*acc){struct _tuple8 _tmp10B;
int(*_tmp10C)(void*,void*,void*);void*_tmp10D;struct _RegionHandle*_tmp10E;struct
_tuple8*_tmp10A=env;_tmp10B=*_tmp10A;_tmp10C=_tmp10B.f1;_tmp10D=_tmp10B.f2;
_tmp10E=_tmp10B.f3;if(_tmp10C(_tmp10D,x,y))*acc=Cyc_Dict_insert(*acc,x,y);return
acc;}struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*r2,int(*f)(void*,
void*,void*),void*f_env,struct Cyc_Dict_Dict d){struct _tuple8 _tmp175;struct _tuple8
_tmp10F=(_tmp175.f1=f,((_tmp175.f2=f_env,((_tmp175.f3=r2,_tmp175)))));struct Cyc_Dict_Dict
_tmp110=Cyc_Dict_rempty(r2,d.rel);return*((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*
f)(struct _tuple8*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple8*env,struct Cyc_Dict_Dict
d,struct Cyc_Dict_Dict*accum))Cyc_Dict_fold_c)(Cyc_Dict_filter_c_f,& _tmp10F,d,&
_tmp110);}struct Cyc_Dict_Dict Cyc_Dict_filter_c(int(*f)(void*,void*,void*),void*
f_env,struct Cyc_Dict_Dict d){return Cyc_Dict_rfilter_c(Cyc_Core_heap_region,f,
f_env,d);}static int Cyc_Dict_difference_f(struct Cyc_Dict_Dict*d,void*x,void*y){
return !Cyc_Dict_member(*d,x);}struct Cyc_Dict_Dict Cyc_Dict_rdifference(struct
_RegionHandle*r2,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){return((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*r2,int(*f)(struct Cyc_Dict_Dict*,void*,void*),struct Cyc_Dict_Dict*
f_env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(r2,Cyc_Dict_difference_f,& d2,d1);}
struct Cyc_Dict_Dict Cyc_Dict_difference(struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2){return Cyc_Dict_rdifference(Cyc_Core_heap_region,d1,d2);}struct _tuple9{int(*
f1)(void*,void*);void*f2;};static int Cyc_Dict_delete_f(struct _tuple9*env,void*x,
void*y){struct _tuple9 _tmp113;int(*_tmp114)(void*,void*);void*_tmp115;struct
_tuple9*_tmp112=env;_tmp113=*_tmp112;_tmp114=_tmp113.f1;_tmp115=_tmp113.f2;
return _tmp114(_tmp115,x)!= 0;}struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct
_RegionHandle*r2,struct Cyc_Dict_Dict d,void*x){if(!Cyc_Dict_member(d,x))return Cyc_Dict_rcopy(
r2,d);{struct _tuple9 _tmp176;struct _tuple9 _tmp116=(_tmp176.f1=d.rel,((_tmp176.f2=
x,_tmp176)));return((struct Cyc_Dict_Dict(*)(struct _RegionHandle*r2,int(*f)(
struct _tuple9*,void*,void*),struct _tuple9*f_env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(
r2,Cyc_Dict_delete_f,& _tmp116,d);};}struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(
struct Cyc_Dict_Dict d,void*x){if(!Cyc_Dict_member(d,x))return d;{struct _tuple9
_tmp177;struct _tuple9 _tmp118=(_tmp177.f1=d.rel,((_tmp177.f2=x,_tmp177)));return((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*r2,int(*f)(struct _tuple9*,void*,void*),
struct _tuple9*f_env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(d.r,Cyc_Dict_delete_f,&
_tmp118,d);};}struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict d,void*x){
return Cyc_Dict_rdelete(Cyc_Core_heap_region,d,x);}struct _tuple10{struct
_dyneither_ptr f1;int f2;};int Cyc_Dict_iter_f(struct _tuple10*stk,struct _tuple0*
dest){struct _tuple10 _tmp11B;struct _dyneither_ptr _tmp11C;int _tmp11D;int*_tmp11E;
struct _tuple10*_tmp11A=stk;_tmp11B=*_tmp11A;_tmp11C=_tmp11B.f1;_tmp11D=_tmp11B.f2;
_tmp11E=(int*)&(*_tmp11A).f2;{int _tmp11F=*_tmp11E;if(_tmp11F == - 1)return 0;{const
struct Cyc_Dict_T*_tmp120=*((const struct Cyc_Dict_T**)_check_dyneither_subscript(
_tmp11C,sizeof(const struct Cyc_Dict_T*),_tmp11F));*dest=((const struct Cyc_Dict_T*)
_check_null(_tmp120))->key_val;-- _tmp11F;if((unsigned int)_tmp120->left)*((const
struct Cyc_Dict_T**)_check_dyneither_subscript(_tmp11C,sizeof(const struct Cyc_Dict_T*),
++ _tmp11F))=_tmp120->left;if((unsigned int)_tmp120->right)*((const struct Cyc_Dict_T**)
_check_dyneither_subscript(_tmp11C,sizeof(const struct Cyc_Dict_T*),++ _tmp11F))=
_tmp120->right;*_tmp11E=_tmp11F;return 1;};};}struct Cyc_Iter_Iter Cyc_Dict_make_iter(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);static void _tmp17D(unsigned int*
_tmp17C,unsigned int*_tmp17B,const struct Cyc_Dict_T***_tmp179,const struct Cyc_Dict_T**
_tmp121){for(*_tmp17C=0;*_tmp17C < *_tmp17B;(*_tmp17C)++){(*_tmp179)[*_tmp17C]=*
_tmp121;}}struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct
Cyc_Dict_Dict d){int half_max_size=1;const struct Cyc_Dict_T*_tmp121=d.t;while(
_tmp121 != (const struct Cyc_Dict_T*)0){_tmp121=_tmp121->left;++ half_max_size;}
_tmp121=d.t;{unsigned int _tmp17C;unsigned int _tmp17B;struct _dyneither_ptr _tmp17A;
const struct Cyc_Dict_T**_tmp179;unsigned int _tmp178;struct _dyneither_ptr _tmp122=(
_tmp178=(unsigned int)(2 * half_max_size),((_tmp179=(const struct Cyc_Dict_T**)
_region_malloc(rgn,_check_times(sizeof(const struct Cyc_Dict_T*),_tmp178)),((
_tmp17A=_tag_dyneither(_tmp179,sizeof(const struct Cyc_Dict_T*),_tmp178),((((
_tmp17B=_tmp178,_tmp17D(& _tmp17C,& _tmp17B,& _tmp179,& _tmp121))),_tmp17A)))))));
struct _tuple10*_tmp180;struct Cyc_Iter_Iter _tmp17F;return(_tmp17F.env=(void*)((
_tmp180=_region_malloc(rgn,sizeof(*_tmp180)),((_tmp180->f1=_tmp122,((_tmp180->f2=(
unsigned int)_tmp121?0: - 1,_tmp180)))))),((_tmp17F.next=(int(*)(void*env,void*
dest))Cyc_Dict_iter_f,(struct Cyc_Iter_Iter)_tmp17F)));};}void*Cyc_Dict_marshal(
struct _RegionHandle*rgn,void*env,void*(*write_key)(void*,struct Cyc___cycFILE*,
void*),void*(*write_val)(void*,struct Cyc___cycFILE*,void*),struct Cyc___cycFILE*
fp,struct Cyc_Dict_Dict dict){struct Cyc_List_List*dict_list=Cyc_Dict_rto_list(rgn,
dict);int len=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dict_list);{struct
Cyc_Core_Failure_exn_struct _tmp186;const char*_tmp185;struct Cyc_Core_Failure_exn_struct*
_tmp184;(int)_throw((void*)((_tmp184=_cycalloc(sizeof(*_tmp184)),((_tmp184[0]=((
_tmp186.tag=Cyc_Core_Failure,((_tmp186.f1=((_tmp185="Dict::marshal: Write failure",
_tag_dyneither(_tmp185,sizeof(char),29))),_tmp186)))),_tmp184)))));}while(
dict_list != 0){env=((void*(*)(void*,struct Cyc___cycFILE*,struct _tuple0*))
write_key)(env,fp,(struct _tuple0*)dict_list->hd);env=((void*(*)(void*,struct Cyc___cycFILE*,
struct _tuple0*))write_val)(env,fp,(struct _tuple0*)dict_list->hd);dict_list=
dict_list->tl;}return env;}struct Cyc_Dict_Dict Cyc_Dict_unmarshal(struct
_RegionHandle*rgn,void*env,int(*cmp)(void*,void*),void*(*read_key)(void*,struct
Cyc___cycFILE*),void*(*read_val)(void*,struct Cyc___cycFILE*),struct Cyc___cycFILE*
fp){struct Cyc_Dict_Dict dict=Cyc_Dict_empty(cmp);int len=Cyc_getw(fp);if(len == - 1){
struct Cyc_Core_Failure_exn_struct _tmp18C;const char*_tmp18B;struct Cyc_Core_Failure_exn_struct*
_tmp18A;(int)_throw((void*)((_tmp18A=_cycalloc(sizeof(*_tmp18A)),((_tmp18A[0]=((
_tmp18C.tag=Cyc_Core_Failure,((_tmp18C.f1=((_tmp18B="Dict::unmarshal: list length is -1",
_tag_dyneither(_tmp18B,sizeof(char),35))),_tmp18C)))),_tmp18A)))));}{int i=0;for(
0;i < len;++ i){void*key=read_key(env,fp);void*val=read_val(env,fp);dict=Cyc_Dict_insert(
dict,key,val);}}return dict;}
