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
typedef void*Cyc_parg_t;struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};
struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};
struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{
int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};
struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};typedef void*
Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{
char*tag;struct _dyneither_ptr f1;};typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{
void*v;};typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;void*Cyc_Core_identity(void*);
extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{
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
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern
char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;
};struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[
4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};typedef
struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{
char*tag;};typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{
int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{
int tag;};typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct
_dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[
10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};typedef struct
_dyneither_ptr*Cyc_Absyn_field_name_t;typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;
struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};
union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
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
int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct
Cyc_Absyn_HeapRgn_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;
struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{
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
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{
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
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;
unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;
struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;
};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual
ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;
struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;
struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind
kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct
_tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};
struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{
struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{
enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
char*tag;};int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
unsigned int);extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;extern void*
Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern struct _tuple0*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zero_term);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*
init);void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual
ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts);struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;typedef struct Cyc_Set_Set*
Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*
tag;};struct Cyc_Dict_T;typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};typedef
struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*
tag;};struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));int Cyc_Dict_member(
struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void*Cyc_Dict_fold(
void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d,void*accum);void Cyc_Dict_iter_c(
void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict
Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;
typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct
Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);struct
Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,
struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int Cyc_RgnOrder_is_region_resetable(
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
Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct
_RegionHandle*);struct Cyc_Interface_I;typedef struct Cyc_Interface_I*Cyc_Interface_i_t;
struct Cyc_Interface_I*Cyc_Interface_empty();struct Cyc_Interface_I*Cyc_Interface_final();
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict ae);struct
_tuple10{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info);struct Cyc_Interface_I*
Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct
_tuple10*info);struct Cyc_Interface_I*Cyc_Interface_merge_list(struct Cyc_List_List*
li,struct Cyc_List_List*linfo);struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(
struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*
linfo);void Cyc_Interface_print(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);void Cyc_Interface_save(
struct Cyc_Interface_I*,struct Cyc___cycFILE*);struct Cyc_Interface_I*Cyc_Interface_load(
struct Cyc___cycFILE*);typedef struct Cyc_Interface_I*Cyc_interface_t;int Cyc_strptrcmp(
struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;typedef
struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;typedef struct Cyc_PP_Out*
Cyc_PP_out_t;struct Cyc_PP_Doc;typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{
int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int
rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;
int print_using_stmts;int print_externC_stmts;int print_full_evars;int
print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern
struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple0*);void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int
var_default_init,struct Cyc_List_List*ds);extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(
struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;
struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,
void*);void*read_fun_state;};typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;
struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr
lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct
_dyneither_ptr lex_check;};typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);struct _tuple11{
struct _tuple11*tl;};struct Cyc_FlatList{struct _tuple11*tl;};typedef struct _tuple11*
Cyc_flat_list_t;struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int
Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*
Type_spec;unsigned int loc;};typedef struct Cyc_Type_specifier Cyc_type_specifier_t;
struct Cyc_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};typedef struct Cyc_Declarator
Cyc_declarator_t;struct _tuple12{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};
struct _tuple13{struct _tuple13*tl;struct _tuple12 hd;};typedef struct _tuple13*Cyc_declarator_list_t;
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc  = 1,Cyc_ExternC_sc  = 2,
Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,Cyc_Abstract_sc  = 6};
typedef enum Cyc_Storage_class Cyc_storage_class_t;struct Cyc_Declaration_spec{
enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier
type_specs;int is_inline;struct Cyc_List_List*attributes;};typedef struct Cyc_Declaration_spec
Cyc_decl_spec_t;struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};typedef
struct Cyc_Abstractdeclarator Cyc_abstractdeclarator_t;struct
_union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{
int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;
};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct
_union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple14{int f1;struct
_dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple14 val;};
struct _tuple15{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*
f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple15*val;};struct
_union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*
val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple16{struct Cyc_List_List*
f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple16*val;};struct
_union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple17*val;
};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY16{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY17{int tag;
struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Declaration_spec val;};struct
_union_YYSTYPE_YY20{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY21{int tag;
struct _tuple13*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Storage_class*val;
};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Type_specifier val;};struct
_union_YYSTYPE_YY24{int tag;enum Cyc_Absyn_AggrKind val;};struct
_union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY26{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY29{int tag;struct Cyc_Declarator val;};struct _union_YYSTYPE_YY30{
int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;
};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct
_union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct
_union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_Absyn_Tqual
f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{
int tag;struct _tuple18 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct
_union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple19{struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct
_union_YYSTYPE_YY39{int tag;struct _tuple19*val;};struct _union_YYSTYPE_YY40{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int
tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};
struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*
val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{
int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};
struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct
_union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct
_union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok
Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct
_union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_YY1 YY1;struct
_union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;
struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7
YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct
_union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12
YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct
_union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17
YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct
_union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22
YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct
_union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27
YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct
_union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32
YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct
_union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37
YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct
_union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42
YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct
_union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47
YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct
_union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52
YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct
_union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int
first_line;int first_column;int last_line;int last_column;};typedef struct Cyc_Yyltype
Cyc_yyltype;void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_function_type(
void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;void*Cyc_Tcutil_kind_to_bound(
struct Cyc_Absyn_Kind*k);void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_exn_struct{
char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;
struct Cyc_Absyn_Datatypefield*field;};typedef struct Cyc_Tcdecl_Xdatatypefielddecl*
Cyc_Tcdecl_xdatatypefielddecl_t;void Cyc_Tcdecl_merr(unsigned int loc,struct
_dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_Absyn_Aggrdecl*
Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*
d1,unsigned int loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(
struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,
struct _dyneither_ptr*msg);struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct
_dyneither_ptr*msg);struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*
d0,struct Cyc_Absyn_Vardecl*d1,unsigned int loc,struct _dyneither_ptr*msg);struct
Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*
d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct
Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,
unsigned int loc,struct _dyneither_ptr*msg);void Cyc_Lex_lex_init();typedef struct
Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Interface_xdatatypefielddecl_t;struct Cyc_Interface_Ienv{
struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict
enumdecls;struct Cyc_Dict_Dict typedefdecls;struct Cyc_Dict_Dict vardecls;struct Cyc_Dict_Dict
xdatatypefielddecls;};typedef struct Cyc_Interface_Ienv*Cyc_Interface_ienv_t;
struct Cyc_Interface_I{struct Cyc_Dict_Dict imports;struct Cyc_Dict_Dict exports;};
static struct Cyc_Interface_Ienv*Cyc_Interface_new_ienv(){struct Cyc_Interface_Ienv*
_tmp25D;return(_tmp25D=_cycalloc(sizeof(*_tmp25D)),((_tmp25D->aggrdecls=((struct
Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(
Cyc_strptrcmp),((_tmp25D->datatypedecls=((struct Cyc_Dict_Dict(*)(int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),((
_tmp25D->enumdecls=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),((_tmp25D->typedefdecls=((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))
Cyc_Dict_empty)(Cyc_strptrcmp),((_tmp25D->vardecls=((struct Cyc_Dict_Dict(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),((
_tmp25D->xdatatypefielddecls=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),_tmp25D)))))))))))));}
static struct Cyc_Interface_Ienv*Cyc_Interface_lazy_new_ienv(){static struct Cyc_Interface_Ienv*
ie=0;if(ie == 0)ie=(struct Cyc_Interface_Ienv*)Cyc_Interface_new_ienv();return(
struct Cyc_Interface_Ienv*)ie;}static struct Cyc_Interface_Ienv*Cyc_Interface_skel2i_f(
void*ignored){return Cyc_Interface_new_ienv();}static struct Cyc_Interface_I*Cyc_Interface_skel2i(
struct Cyc_Dict_Dict skel){struct Cyc_Interface_I*_tmp25E;return(_tmp25E=_cycalloc(
sizeof(*_tmp25E)),((_tmp25E->imports=((struct Cyc_Dict_Dict(*)(struct Cyc_Interface_Ienv*(*
f)(void*),struct Cyc_Dict_Dict d))Cyc_Dict_map)(Cyc_Interface_skel2i_f,skel),((
_tmp25E->exports=((struct Cyc_Dict_Dict(*)(struct Cyc_Interface_Ienv*(*f)(void*),
struct Cyc_Dict_Dict d))Cyc_Dict_map)(Cyc_Interface_skel2i_f,skel),_tmp25E)))));}
struct Cyc_Interface_I*Cyc_Interface_empty(){struct Cyc_Interface_I*_tmp25F;return(
_tmp25F=_cycalloc(sizeof(*_tmp25F)),((_tmp25F->imports=((struct Cyc_Dict_Dict(*)(
int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp),((
_tmp25F->exports=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct
Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp),_tmp25F)))));}struct Cyc_Interface_I*
Cyc_Interface_final(){struct Cyc_Interface_I*_tmp3=Cyc_Interface_empty();struct
Cyc_Interface_Ienv*_tmp4=Cyc_Interface_new_ienv();struct Cyc_Absyn_Datatypedecl*
_tmp260;struct Cyc_Absyn_Datatypedecl*_tmp5=(_tmp260=_cycalloc(sizeof(*_tmp260)),((
_tmp260[0]=*Cyc_Absyn_exn_tud,_tmp260)));_tmp5->sc=Cyc_Absyn_Public;_tmp4->datatypedecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*
v))Cyc_Dict_insert)(_tmp4->datatypedecls,(*Cyc_Absyn_exn_name).f2,_tmp5);{struct
Cyc_List_List*_tmp6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;
for(0;_tmp6 != 0;_tmp6=_tmp6->tl){struct Cyc_Absyn_Datatypefield*_tmp261;struct Cyc_Absyn_Datatypefield*
_tmp7=(_tmp261=_cycalloc(sizeof(*_tmp261)),((_tmp261[0]=*((struct Cyc_Absyn_Datatypefield*)
_tmp6->hd),_tmp261)));_tmp7->sc=Cyc_Absyn_Public;{struct Cyc_Tcdecl_Xdatatypefielddecl*
_tmp262;struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp8=(_tmp262=_cycalloc(sizeof(*
_tmp262)),((_tmp262->base=_tmp5,((_tmp262->field=_tmp7,_tmp262)))));_tmp4->xdatatypefielddecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Tcdecl_Xdatatypefielddecl*
v))Cyc_Dict_insert)(_tmp4->xdatatypefielddecls,(*((struct Cyc_Absyn_Datatypefield*)
_tmp6->hd)->name).f2,_tmp8);};}}_tmp3->exports=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(
_tmp3->exports,0,_tmp4);{struct Cyc_Interface_Ienv*_tmpB=Cyc_Interface_new_ienv();
struct _dyneither_ptr*_tmp26A;const char*_tmp269;union Cyc_Absyn_Nmspace _tmp268;
struct _tuple0*_tmp267;struct _tuple0*qmain=(_tmp267=_cycalloc(sizeof(*_tmp267)),((
_tmp267->f1=(((_tmp268.Abs_n).val=0,(((_tmp268.Abs_n).tag=2,_tmp268)))),((
_tmp267->f2=((_tmp26A=_cycalloc(sizeof(*_tmp26A)),((_tmp26A[0]=(struct
_dyneither_ptr)((_tmp269="main",_tag_dyneither(_tmp269,sizeof(char),5))),_tmp26A)))),
_tmp267)))));struct Cyc_List_List*_tmp272;struct _tuple8*_tmp271;struct _tuple8*
_tmp270;struct Cyc_List_List*_tmp26F;struct Cyc_Absyn_Vardecl*_tmpC=Cyc_Absyn_new_vardecl(
qmain,Cyc_Absyn_function_typ(0,0,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_sint_typ,((
_tmp26F=_cycalloc(sizeof(*_tmp26F)),((_tmp26F->hd=((_tmp270=_cycalloc(sizeof(*
_tmp270)),((_tmp270->f1=0,((_tmp270->f2=Cyc_Absyn_empty_tqual(0),((_tmp270->f3=
Cyc_Absyn_uint_typ,_tmp270)))))))),((_tmp26F->tl=((_tmp272=_cycalloc(sizeof(*
_tmp272)),((_tmp272->hd=((_tmp271=_cycalloc(sizeof(*_tmp271)),((_tmp271->f1=0,((
_tmp271->f2=Cyc_Absyn_empty_tqual(0),((_tmp271->f3=Cyc_Absyn_dyneither_typ(Cyc_Absyn_dyneither_typ(
Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),(
void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),
_tmp271)))))))),((_tmp272->tl=0,_tmp272)))))),_tmp26F)))))),0,0,0,0),0);_tmpC->sc=
Cyc_Absyn_Extern;_tmpB->vardecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpB->vardecls,(*
qmain).f2,_tmpC);_tmp3->imports=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(_tmp3->exports,
0,_tmpB);return _tmp3;};}static void Cyc_Interface_err(struct _dyneither_ptr msg){
void*_tmp273;(_tmp273=0,Cyc_Tcutil_terr(0,msg,_tag_dyneither(_tmp273,sizeof(void*),
0)));}static void*Cyc_Interface_invalid_arg(struct _dyneither_ptr s){struct Cyc_Core_Invalid_argument_exn_struct
_tmp276;struct Cyc_Core_Invalid_argument_exn_struct*_tmp275;(int)_throw((void*)((
_tmp275=_cycalloc(sizeof(*_tmp275)),((_tmp275[0]=((_tmp276.tag=Cyc_Core_Invalid_argument,((
_tmp276.f1=s,_tmp276)))),_tmp275)))));}static void Cyc_Interface_fields_err(struct
_dyneither_ptr sc,struct _dyneither_ptr t,struct _tuple0*n){const char*_tmp27C;void*
_tmp27B[3];struct Cyc_String_pa_PrintArg_struct _tmp27A;struct Cyc_String_pa_PrintArg_struct
_tmp279;struct Cyc_String_pa_PrintArg_struct _tmp278;Cyc_Interface_err((struct
_dyneither_ptr)((_tmp278.tag=0,((_tmp278.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(n)),((_tmp279.tag=0,((_tmp279.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)t),((_tmp27A.tag=0,((_tmp27A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)sc),((_tmp27B[0]=& _tmp27A,((_tmp27B[1]=&
_tmp279,((_tmp27B[2]=& _tmp278,Cyc_aprintf(((_tmp27C="fields of %s %s %s have never been defined",
_tag_dyneither(_tmp27C,sizeof(char),43))),_tag_dyneither(_tmp27B,sizeof(void*),3)))))))))))))))))))));}
static void Cyc_Interface_body_err(struct _dyneither_ptr sc,struct _tuple0*n){const
char*_tmp281;void*_tmp280[2];struct Cyc_String_pa_PrintArg_struct _tmp27F;struct
Cyc_String_pa_PrintArg_struct _tmp27E;Cyc_Interface_err((struct _dyneither_ptr)((
_tmp27E.tag=0,((_tmp27E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
n)),((_tmp27F.tag=0,((_tmp27F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sc),((
_tmp280[0]=& _tmp27F,((_tmp280[1]=& _tmp27E,Cyc_aprintf(((_tmp281="the body of %s function %s has never been defined",
_tag_dyneither(_tmp281,sizeof(char),50))),_tag_dyneither(_tmp280,sizeof(void*),2)))))))))))))));}
static void Cyc_Interface_static_err(struct _dyneither_ptr obj1,struct _tuple0*name1,
struct _dyneither_ptr obj2,struct _tuple0*name2){if((char*)obj1.curr != (char*)(
_tag_dyneither(0,0,0)).curr){const char*_tmp288;void*_tmp287[4];struct Cyc_String_pa_PrintArg_struct
_tmp286;struct Cyc_String_pa_PrintArg_struct _tmp285;struct Cyc_String_pa_PrintArg_struct
_tmp284;struct Cyc_String_pa_PrintArg_struct _tmp283;Cyc_Interface_err((struct
_dyneither_ptr)((_tmp283.tag=0,((_tmp283.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(name2)),((_tmp284.tag=0,((_tmp284.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)obj2),((_tmp285.tag=0,((_tmp285.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(name1)),((
_tmp286.tag=0,((_tmp286.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj1),((
_tmp287[0]=& _tmp286,((_tmp287[1]=& _tmp285,((_tmp287[2]=& _tmp284,((_tmp287[3]=&
_tmp283,Cyc_aprintf(((_tmp288="declaration of %s %s relies on static %s %s",
_tag_dyneither(_tmp288,sizeof(char),44))),_tag_dyneither(_tmp287,sizeof(void*),4)))))))))))))))))))))))))));}}
static void Cyc_Interface_abstract_err(struct _dyneither_ptr obj1,struct _tuple0*
name1,struct _dyneither_ptr obj2,struct _tuple0*name2){if((char*)obj1.curr != (char*)(
_tag_dyneither(0,0,0)).curr){const char*_tmp28F;void*_tmp28E[4];struct Cyc_String_pa_PrintArg_struct
_tmp28D;struct Cyc_String_pa_PrintArg_struct _tmp28C;struct Cyc_String_pa_PrintArg_struct
_tmp28B;struct Cyc_String_pa_PrintArg_struct _tmp28A;Cyc_Interface_err((struct
_dyneither_ptr)((_tmp28A.tag=0,((_tmp28A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(name2)),((_tmp28B.tag=0,((_tmp28B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)obj2),((_tmp28C.tag=0,((_tmp28C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(name1)),((
_tmp28D.tag=0,((_tmp28D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj1),((
_tmp28E[0]=& _tmp28D,((_tmp28E[1]=& _tmp28C,((_tmp28E[2]=& _tmp28B,((_tmp28E[3]=&
_tmp28A,Cyc_aprintf(((_tmp28F="declaration of %s %s relies on fields of abstract %s %s",
_tag_dyneither(_tmp28F,sizeof(char),56))),_tag_dyneither(_tmp28E,sizeof(void*),4)))))))))))))))))))))))))));}}
struct Cyc_Interface_Seen{struct Cyc_Dict_Dict aggrs;struct Cyc_Dict_Dict datatypes;};
typedef struct Cyc_Interface_Seen*Cyc_Interface_seen_t;static struct Cyc_Interface_Seen*
Cyc_Interface_new_seen(){struct Cyc_Interface_Seen*_tmp290;return(_tmp290=
_cycalloc(sizeof(*_tmp290)),((_tmp290->aggrs=((struct Cyc_Dict_Dict(*)(int(*cmp)(
struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),((_tmp290->datatypes=((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp),_tmp290)))));}typedef struct Cyc_Dict_Dict Cyc_Interface_aenv_t;
static int Cyc_Interface_check_public_type(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*
seen,struct _dyneither_ptr obj,struct _tuple0*name,void*t);static int Cyc_Interface_check_public_type_list(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,
struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l){int _tmp2F=1;for(0;l != 
0;l=l->tl){if(!Cyc_Interface_check_public_type(ae,seen,obj,name,f((void*)l->hd)))
_tmp2F=0;}return _tmp2F;}static int Cyc_Interface_check_public_aggrdecl(struct Cyc_Dict_Dict
ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Aggrdecl*d){{struct
_handler_cons _tmp30;_push_handler(& _tmp30);{int _tmp32=0;if(setjmp(_tmp30.handler))
_tmp32=1;if(!_tmp32){{int _tmp33=((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))
Cyc_Dict_lookup)(seen->aggrs,d->name);_npop_handler(0);return _tmp33;};
_pop_handler();}else{void*_tmp31=(void*)_exn_thrown;void*_tmp35=_tmp31;_LL1: {
struct Cyc_Dict_Absent_exn_struct*_tmp36=(struct Cyc_Dict_Absent_exn_struct*)
_tmp35;if(_tmp36->tag != Cyc_Dict_Absent)goto _LL3;}_LL2: goto _LL0;_LL3:;_LL4:(void)
_throw(_tmp35);_LL0:;}};}{int _tmp37=1;seen->aggrs=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,
_tmp37);if(d->impl != 0){struct Cyc_List_List*_tmp38=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(d->impl))->fields;for(0;_tmp38 != 0;_tmp38=_tmp38->tl){const char*
_tmp291;if(!Cyc_Interface_check_public_type(ae,seen,((_tmp291="type",
_tag_dyneither(_tmp291,sizeof(char),5))),d->name,((struct Cyc_Absyn_Aggrfield*)
_tmp38->hd)->type))_tmp37=0;}}seen->aggrs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,_tmp37);return
_tmp37;};}struct _tuple20{struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_Interface_get_type1(
struct _tuple20*x){return(*x).f2;}static void*Cyc_Interface_get_type2(struct
_tuple8*x){return(*x).f3;}static int Cyc_Interface_check_public_datatypedecl(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Datatypedecl*
d){{struct _handler_cons _tmp3A;_push_handler(& _tmp3A);{int _tmp3C=0;if(setjmp(
_tmp3A.handler))_tmp3C=1;if(!_tmp3C){{int _tmp3D=((int(*)(struct Cyc_Dict_Dict d,
struct _tuple0*k))Cyc_Dict_lookup)(seen->datatypes,d->name);_npop_handler(0);
return _tmp3D;};_pop_handler();}else{void*_tmp3B=(void*)_exn_thrown;void*_tmp3F=
_tmp3B;_LL6: {struct Cyc_Dict_Absent_exn_struct*_tmp40=(struct Cyc_Dict_Absent_exn_struct*)
_tmp3F;if(_tmp40->tag != Cyc_Dict_Absent)goto _LL8;}_LL7: goto _LL5;_LL8:;_LL9:(void)
_throw(_tmp3F);_LL5:;}};}{int _tmp41=1;seen->datatypes=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->datatypes,d->name,
_tmp41);if(d->fields != 0){struct Cyc_List_List*_tmp42=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(d->fields))->v;for(0;_tmp42 != 0;_tmp42=_tmp42->tl){
const char*_tmp292;if(!((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*
seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple20*),
struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,((_tmp292="datatype",
_tag_dyneither(_tmp292,sizeof(char),9))),d->name,Cyc_Interface_get_type1,((
struct Cyc_Absyn_Datatypefield*)_tmp42->hd)->typs))_tmp41=0;}}seen->datatypes=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(
seen->datatypes,d->name,_tmp41);return _tmp41;};}static int Cyc_Interface_check_public_enumdecl(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Enumdecl*d){
return 1;}static int Cyc_Interface_check_public_typedefdecl(struct Cyc_Dict_Dict ae,
struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Typedefdecl*d){if(d->defn != 0)
return Cyc_Interface_check_public_type(ae,seen,_tag_dyneither(0,0,0),d->name,(
void*)_check_null(d->defn));else{return 1;}}static int Cyc_Interface_check_public_vardecl(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Vardecl*d){
const char*_tmp293;return Cyc_Interface_check_public_type(ae,seen,((_tmp293="variable",
_tag_dyneither(_tmp293,sizeof(char),9))),d->name,d->type);}static struct Cyc_List_List*
Cyc_Interface_get_abs_ns(union Cyc_Absyn_Nmspace ns){union Cyc_Absyn_Nmspace _tmp45=
ns;struct Cyc_List_List*_tmp46;struct Cyc_List_List*_tmp47;_LLB: if((_tmp45.Abs_n).tag
!= 2)goto _LLD;_tmp46=(struct Cyc_List_List*)(_tmp45.Abs_n).val;_LLC: return _tmp46;
_LLD: if((_tmp45.C_n).tag != 3)goto _LLF;_tmp47=(struct Cyc_List_List*)(_tmp45.C_n).val;
_LLE: return _tmp47;_LLF:;_LL10: {const char*_tmp294;((int(*)(struct _dyneither_ptr s))
Cyc_Interface_invalid_arg)(((_tmp294="get_abs_ns",_tag_dyneither(_tmp294,sizeof(
char),11))));}_LLA:;}static int Cyc_Interface_check_public_type(struct Cyc_Dict_Dict
ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*
t){void*_tmp49=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp4B;void*_tmp4C;
struct Cyc_Absyn_ArrayInfo _tmp4E;void*_tmp4F;void*_tmp51;struct Cyc_Absyn_FnInfo
_tmp53;void*_tmp54;struct Cyc_List_List*_tmp55;struct Cyc_Absyn_VarargInfo*_tmp56;
struct Cyc_List_List*_tmp58;struct Cyc_Absyn_AggrInfo _tmp5A;union Cyc_Absyn_AggrInfoU
_tmp5B;struct Cyc_List_List*_tmp5C;struct _tuple0*_tmp5E;struct Cyc_Absyn_DatatypeInfo
_tmp60;union Cyc_Absyn_DatatypeInfoU _tmp61;struct Cyc_Absyn_Datatypedecl**_tmp62;
struct Cyc_Absyn_Datatypedecl*_tmp63;struct Cyc_List_List*_tmp64;struct Cyc_Absyn_DatatypeFieldInfo
_tmp66;union Cyc_Absyn_DatatypeFieldInfoU _tmp67;struct _tuple1 _tmp68;struct Cyc_Absyn_Datatypedecl*
_tmp69;struct Cyc_Absyn_Datatypefield*_tmp6A;struct Cyc_List_List*_tmp6B;_LL12: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmp49;if(_tmp4A->tag != 5)goto _LL14;else{_tmp4B=_tmp4A->f1;_tmp4C=_tmp4B.elt_typ;}}
_LL13: _tmp4F=_tmp4C;goto _LL15;_LL14: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp4D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp49;if(_tmp4D->tag != 8)
goto _LL16;else{_tmp4E=_tmp4D->f1;_tmp4F=_tmp4E.elt_type;}}_LL15: _tmp51=(void*)
_tmp4F;goto _LL17;_LL16:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp50=(
struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp49;if(_tmp50->tag != 17)goto
_LL18;else{_tmp51=(void*)_tmp50->f4;}}if(!(_tmp51 != 0))goto _LL18;_LL17: return Cyc_Interface_check_public_type(
ae,seen,obj,name,(void*)_check_null(_tmp51));_LL18: {struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp52=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp49;if(_tmp52->tag != 9)goto
_LL1A;else{_tmp53=_tmp52->f1;_tmp54=_tmp53.ret_typ;_tmp55=_tmp53.args;_tmp56=
_tmp53.cyc_varargs;}}_LL19: {int b=((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*
seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple8*),struct
Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,Cyc_Interface_get_type2,
_tmp55) && Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp54);if(_tmp56 != 
0){void*_tmp6D;struct Cyc_Absyn_VarargInfo _tmp6C=*_tmp56;_tmp6D=_tmp6C.type;b=Cyc_Interface_check_public_type(
ae,seen,obj,name,_tmp6D);}return b;}_LL1A: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmp57=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp49;if(_tmp57->tag != 10)
goto _LL1C;else{_tmp58=_tmp57->f1;}}_LL1B: return((int(*)(struct Cyc_Dict_Dict ae,
struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*
f)(struct _tuple20*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(
ae,seen,obj,name,Cyc_Interface_get_type1,_tmp58);_LL1C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp59=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp49;if(_tmp59->tag != 11)
goto _LL1E;else{_tmp5A=_tmp59->f1;_tmp5B=_tmp5A.aggr_info;_tmp5C=_tmp5A.targs;}}
_LL1D: {struct Cyc_Absyn_Aggrdecl*_tmp6E=Cyc_Absyn_get_known_aggrdecl(_tmp5B);if(
_tmp6E->sc == Cyc_Absyn_Static){{const char*_tmp295;Cyc_Interface_static_err(obj,
name,((_tmp295="type",_tag_dyneither(_tmp295,sizeof(char),5))),_tmp6E->name);}
return 0;}return((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,
struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*
l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,
_tmp5C) && Cyc_Interface_check_public_aggrdecl(ae,seen,_tmp6E);}_LL1E: {struct
Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp5D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)
_tmp49;if(_tmp5D->tag != 13)goto _LL20;else{_tmp5E=_tmp5D->f1;}}_LL1F: {struct
_tuple0 _tmp71;union Cyc_Absyn_Nmspace _tmp72;struct _dyneither_ptr*_tmp73;struct
_tuple0*_tmp70=_tmp5E;_tmp71=*_tmp70;_tmp72=_tmp71.f1;_tmp73=_tmp71.f2;{struct
Cyc_List_List*_tmp74=Cyc_Interface_get_abs_ns(_tmp72);struct Cyc_Absyn_Enumdecl*
ed;{struct _handler_cons _tmp75;_push_handler(& _tmp75);{int _tmp77=0;if(setjmp(
_tmp75.handler))_tmp77=1;if(!_tmp77){{struct Cyc_Tcenv_Genv*_tmp78=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmp74);ed=*((
struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp78->enumdecls,_tmp73);};_pop_handler();}else{void*_tmp76=(void*)_exn_thrown;
void*_tmp7A=_tmp76;_LL27: {struct Cyc_Dict_Absent_exn_struct*_tmp7B=(struct Cyc_Dict_Absent_exn_struct*)
_tmp7A;if(_tmp7B->tag != Cyc_Dict_Absent)goto _LL29;}_LL28: {const char*_tmp299;
void*_tmp298[1];struct Cyc_String_pa_PrintArg_struct _tmp297;((int(*)(struct
_dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)((_tmp297.tag=
0,((_tmp297.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp5E)),((_tmp298[0]=& _tmp297,Cyc_aprintf(((_tmp299="check_public_type (can't find enum %s)",
_tag_dyneither(_tmp299,sizeof(char),39))),_tag_dyneither(_tmp298,sizeof(void*),1)))))))));}
_LL29:;_LL2A:(void)_throw(_tmp7A);_LL26:;}};}if(ed->sc == Cyc_Absyn_Static){{
const char*_tmp29A;Cyc_Interface_static_err(obj,_tmp5E,((_tmp29A="enum",
_tag_dyneither(_tmp29A,sizeof(char),5))),ed->name);}return 0;}return 1;};}_LL20: {
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5F=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)
_tmp49;if(_tmp5F->tag != 3)goto _LL22;else{_tmp60=_tmp5F->f1;_tmp61=_tmp60.datatype_info;
if((_tmp61.KnownDatatype).tag != 2)goto _LL22;_tmp62=(struct Cyc_Absyn_Datatypedecl**)(
_tmp61.KnownDatatype).val;_tmp63=*_tmp62;_tmp64=_tmp60.targs;}}_LL21: {struct
_tuple0 _tmp81;union Cyc_Absyn_Nmspace _tmp82;struct _dyneither_ptr*_tmp83;struct
_tuple0*_tmp80=_tmp63->name;_tmp81=*_tmp80;_tmp82=_tmp81.f1;_tmp83=_tmp81.f2;{
struct Cyc_List_List*_tmp84=Cyc_Interface_get_abs_ns(_tmp82);struct Cyc_Absyn_Datatypedecl*
tud;{struct _handler_cons _tmp85;_push_handler(& _tmp85);{int _tmp87=0;if(setjmp(
_tmp85.handler))_tmp87=1;if(!_tmp87){{struct Cyc_Tcenv_Genv*_tmp88=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmp84);tud=*((
struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))
Cyc_Dict_lookup)(_tmp88->datatypedecls,_tmp83);};_pop_handler();}else{void*
_tmp86=(void*)_exn_thrown;void*_tmp8A=_tmp86;_LL2C: {struct Cyc_Dict_Absent_exn_struct*
_tmp8B=(struct Cyc_Dict_Absent_exn_struct*)_tmp8A;if(_tmp8B->tag != Cyc_Dict_Absent)
goto _LL2E;}_LL2D: {const char*_tmp29E;void*_tmp29D[1];struct Cyc_String_pa_PrintArg_struct
_tmp29C;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct
_dyneither_ptr)((_tmp29C.tag=0,((_tmp29C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp63->name)),((_tmp29D[0]=& _tmp29C,Cyc_aprintf(((
_tmp29E="check_public_type (can't find datatype %s)",_tag_dyneither(_tmp29E,
sizeof(char),43))),_tag_dyneither(_tmp29D,sizeof(void*),1)))))))));}_LL2E:;_LL2F:(
void)_throw(_tmp8A);_LL2B:;}};}if(tud->sc == Cyc_Absyn_Static){{const char*_tmp29F;
Cyc_Interface_static_err(obj,name,((_tmp29F="datatype",_tag_dyneither(_tmp29F,
sizeof(char),9))),tud->name);}return 0;}return((int(*)(struct Cyc_Dict_Dict ae,
struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*
f)(void*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,
obj,name,(void*(*)(void*))Cyc_Core_identity,_tmp64) && Cyc_Interface_check_public_datatypedecl(
ae,seen,tud);};}_LL22: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*
_tmp65=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp49;if(_tmp65->tag
!= 4)goto _LL24;else{_tmp66=_tmp65->f1;_tmp67=_tmp66.field_info;if((_tmp67.KnownDatatypefield).tag
!= 2)goto _LL24;_tmp68=(struct _tuple1)(_tmp67.KnownDatatypefield).val;_tmp69=
_tmp68.f1;_tmp6A=_tmp68.f2;_tmp6B=_tmp66.targs;}}_LL23: {struct _tuple0 _tmp91;
union Cyc_Absyn_Nmspace _tmp92;struct _dyneither_ptr*_tmp93;struct _tuple0*_tmp90=
_tmp69->name;_tmp91=*_tmp90;_tmp92=_tmp91.f1;_tmp93=_tmp91.f2;{struct Cyc_List_List*
_tmp94=Cyc_Interface_get_abs_ns(_tmp92);struct Cyc_Absyn_Datatypedecl*tud;{struct
_handler_cons _tmp95;_push_handler(& _tmp95);{int _tmp97=0;if(setjmp(_tmp95.handler))
_tmp97=1;if(!_tmp97){{struct Cyc_Tcenv_Genv*_tmp98=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmp94);tud=*((
struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))
Cyc_Dict_lookup)(_tmp98->datatypedecls,_tmp93);};_pop_handler();}else{void*
_tmp96=(void*)_exn_thrown;void*_tmp9A=_tmp96;_LL31: {struct Cyc_Dict_Absent_exn_struct*
_tmp9B=(struct Cyc_Dict_Absent_exn_struct*)_tmp9A;if(_tmp9B->tag != Cyc_Dict_Absent)
goto _LL33;}_LL32: {const char*_tmp2A3;void*_tmp2A2[1];struct Cyc_String_pa_PrintArg_struct
_tmp2A1;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct
_dyneither_ptr)((_tmp2A1.tag=0,((_tmp2A1.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp69->name)),((_tmp2A2[0]=& _tmp2A1,Cyc_aprintf(((
_tmp2A3="check_public_type (can't find datatype %s and search its fields)",
_tag_dyneither(_tmp2A3,sizeof(char),65))),_tag_dyneither(_tmp2A2,sizeof(void*),1)))))))));}
_LL33:;_LL34:(void)_throw(_tmp9A);_LL30:;}};}if(tud->fields == 0){const char*
_tmp2A7;void*_tmp2A6[1];struct Cyc_String_pa_PrintArg_struct _tmp2A5;((int(*)(
struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)((
_tmp2A5.tag=0,((_tmp2A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tud->name)),((_tmp2A6[0]=& _tmp2A5,Cyc_aprintf(((_tmp2A7="check_public_type (datatype %s has no fields)",
_tag_dyneither(_tmp2A7,sizeof(char),46))),_tag_dyneither(_tmp2A6,sizeof(void*),1)))))))));}{
struct Cyc_Absyn_Datatypefield*tuf1=0;{struct Cyc_List_List*_tmpA2=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmpA2 != 0;_tmpA2=_tmpA2->tl){
if(Cyc_strptrcmp((*_tmp6A->name).f2,(*((struct Cyc_Absyn_Datatypefield*)_tmpA2->hd)->name).f2)
== 0){tuf1=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_Absyn_Datatypefield*)
_tmpA2->hd);break;}}}if(tuf1 == 0){const char*_tmp2AB;void*_tmp2AA[1];struct Cyc_String_pa_PrintArg_struct
_tmp2A9;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct
_dyneither_ptr)((_tmp2A9.tag=0,((_tmp2A9.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6A->name)),((_tmp2AA[0]=& _tmp2A9,Cyc_aprintf(((
_tmp2AB="check_public_type (can't find datatypefield %s)",_tag_dyneither(_tmp2AB,
sizeof(char),48))),_tag_dyneither(_tmp2AA,sizeof(void*),1)))))))));}{struct Cyc_Absyn_Datatypefield*
tuf=(struct Cyc_Absyn_Datatypefield*)tuf1;if(tud->sc == Cyc_Absyn_Static){{const
char*_tmp2AC;Cyc_Interface_static_err(obj,name,((_tmp2AC="datatype",
_tag_dyneither(_tmp2AC,sizeof(char),9))),tud->name);}return 0;}if(tud->sc == Cyc_Absyn_Abstract){{
const char*_tmp2AD;Cyc_Interface_abstract_err(obj,name,((_tmp2AD="datatype",
_tag_dyneither(_tmp2AD,sizeof(char),9))),tud->name);}return 0;}if(tuf->sc == Cyc_Absyn_Static){{
const char*_tmp2B1;void*_tmp2B0[1];struct Cyc_String_pa_PrintArg_struct _tmp2AF;Cyc_Interface_static_err(
obj,name,(struct _dyneither_ptr)((_tmp2AF.tag=0,((_tmp2AF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((
_tmp2B0[0]=& _tmp2AF,Cyc_aprintf(((_tmp2B1="field %s of",_tag_dyneither(_tmp2B1,
sizeof(char),12))),_tag_dyneither(_tmp2B0,sizeof(void*),1)))))))),tud->name);}
return 0;}return((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,
struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*
l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,
_tmp6B) && Cyc_Interface_check_public_datatypedecl(ae,seen,tud);};};};}_LL24:;
_LL25: return 1;_LL11:;}struct _tuple21{struct Cyc_Interface_Ienv*f1;struct Cyc_Interface_Ienv*
f2;int f3;struct Cyc_Dict_Dict f4;struct Cyc_Interface_Seen*f5;struct Cyc_Interface_I*
f6;};static void Cyc_Interface_extract_aggrdecl(struct _tuple21*env,struct
_dyneither_ptr*x,struct Cyc_Absyn_Aggrdecl**dp){struct _tuple21 _tmpAC;struct Cyc_Interface_Ienv*
_tmpAD;struct Cyc_Interface_Ienv*_tmpAE;int _tmpAF;struct Cyc_Dict_Dict _tmpB0;
struct Cyc_Interface_Seen*_tmpB1;struct _tuple21*_tmpAB=env;_tmpAC=*_tmpAB;_tmpAD=
_tmpAC.f1;_tmpAE=_tmpAC.f2;_tmpAF=_tmpAC.f3;_tmpB0=_tmpAC.f4;_tmpB1=_tmpAC.f5;{
struct Cyc_Absyn_Aggrdecl*_tmpB2=*dp;switch(_tmpB2->sc){case Cyc_Absyn_Static:
_LL35: if(_tmpAF  && _tmpB2->impl == 0){const char*_tmp2B3;const char*_tmp2B2;Cyc_Interface_fields_err(((
_tmp2B2="static",_tag_dyneither(_tmp2B2,sizeof(char),7))),((_tmp2B3="struct/union",
_tag_dyneither(_tmp2B3,sizeof(char),13))),_tmpB2->name);}break;case Cyc_Absyn_Abstract:
_LL36: if(_tmpB2->impl == 0){if(_tmpAF){const char*_tmp2B5;const char*_tmp2B4;Cyc_Interface_fields_err(((
_tmp2B4="abstract",_tag_dyneither(_tmp2B4,sizeof(char),9))),((_tmp2B5="struct/union",
_tag_dyneither(_tmp2B5,sizeof(char),13))),_tmpB2->name);}}else{{struct Cyc_Absyn_Aggrdecl*
_tmp2B6;_tmpB2=((_tmp2B6=_cycalloc(sizeof(*_tmp2B6)),((_tmp2B6[0]=*_tmpB2,
_tmp2B6))));}_tmpB2->impl=0;}if(Cyc_Interface_check_public_aggrdecl(_tmpB0,
_tmpB1,_tmpB2))_tmpAE->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmpAE->aggrdecls,
x,_tmpB2);break;case Cyc_Absyn_Public: _LL37: if(_tmpB2->impl == 0){{const char*
_tmp2B8;const char*_tmp2B7;Cyc_Interface_fields_err(((_tmp2B7="public",
_tag_dyneither(_tmp2B7,sizeof(char),7))),((_tmp2B8="struct/union",_tag_dyneither(
_tmp2B8,sizeof(char),13))),_tmpB2->name);}{struct Cyc_Absyn_Aggrdecl*_tmp2B9;
_tmpB2=((_tmp2B9=_cycalloc(sizeof(*_tmp2B9)),((_tmp2B9[0]=*_tmpB2,_tmp2B9))));}
_tmpB2->sc=Cyc_Absyn_Abstract;}if(Cyc_Interface_check_public_aggrdecl(_tmpB0,
_tmpB1,_tmpB2))_tmpAE->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmpAE->aggrdecls,
x,_tmpB2);break;case Cyc_Absyn_ExternC: _LL38: goto _LL39;case Cyc_Absyn_Extern: _LL39:
if(Cyc_Interface_check_public_aggrdecl(_tmpB0,_tmpB1,_tmpB2))_tmpAD->aggrdecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*
v))Cyc_Dict_insert)(_tmpAD->aggrdecls,x,_tmpB2);break;case Cyc_Absyn_Register:
_LL3A:{const char*_tmp2BA;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((
_tmp2BA="add_aggrdecl",_tag_dyneither(_tmp2BA,sizeof(char),13))));}break;}};}
static void Cyc_Interface_extract_enumdecl(struct _tuple21*env,struct _dyneither_ptr*
x,struct Cyc_Absyn_Enumdecl**dp){struct _tuple21 _tmpBD;struct Cyc_Interface_Ienv*
_tmpBE;struct Cyc_Interface_Ienv*_tmpBF;int _tmpC0;struct Cyc_Dict_Dict _tmpC1;
struct Cyc_Interface_Seen*_tmpC2;struct _tuple21*_tmpBC=env;_tmpBD=*_tmpBC;_tmpBE=
_tmpBD.f1;_tmpBF=_tmpBD.f2;_tmpC0=_tmpBD.f3;_tmpC1=_tmpBD.f4;_tmpC2=_tmpBD.f5;{
struct Cyc_Absyn_Enumdecl*_tmpC3=*dp;switch(_tmpC3->sc){case Cyc_Absyn_Static:
_LL3C: if(_tmpC0  && _tmpC3->fields == 0){const char*_tmp2BC;const char*_tmp2BB;Cyc_Interface_fields_err(((
_tmp2BB="static",_tag_dyneither(_tmp2BB,sizeof(char),7))),((_tmp2BC="enum",
_tag_dyneither(_tmp2BC,sizeof(char),5))),_tmpC3->name);}break;case Cyc_Absyn_Abstract:
_LL3D: if(_tmpC3->fields == 0){if(_tmpC0){const char*_tmp2BE;const char*_tmp2BD;Cyc_Interface_fields_err(((
_tmp2BD="abstract",_tag_dyneither(_tmp2BD,sizeof(char),9))),((_tmp2BE="enum",
_tag_dyneither(_tmp2BE,sizeof(char),5))),_tmpC3->name);}}else{{struct Cyc_Absyn_Enumdecl*
_tmp2BF;_tmpC3=((_tmp2BF=_cycalloc(sizeof(*_tmp2BF)),((_tmp2BF[0]=*_tmpC3,
_tmp2BF))));}_tmpC3->fields=0;}if(Cyc_Interface_check_public_enumdecl(_tmpC1,
_tmpC2,_tmpC3))_tmpBF->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmpBF->enumdecls,
x,_tmpC3);break;case Cyc_Absyn_Public: _LL3E: if(_tmpC3->fields == 0){{const char*
_tmp2C1;const char*_tmp2C0;Cyc_Interface_fields_err(((_tmp2C0="public",
_tag_dyneither(_tmp2C0,sizeof(char),7))),((_tmp2C1="enum",_tag_dyneither(_tmp2C1,
sizeof(char),5))),_tmpC3->name);}{struct Cyc_Absyn_Enumdecl*_tmp2C2;_tmpC3=((
_tmp2C2=_cycalloc(sizeof(*_tmp2C2)),((_tmp2C2[0]=*_tmpC3,_tmp2C2))));}_tmpC3->sc=
Cyc_Absyn_Abstract;}if(Cyc_Interface_check_public_enumdecl(_tmpC1,_tmpC2,_tmpC3))
_tmpBF->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmpBF->enumdecls,
x,_tmpC3);break;case Cyc_Absyn_ExternC: _LL3F: goto _LL40;case Cyc_Absyn_Extern: _LL40:
if(Cyc_Interface_check_public_enumdecl(_tmpC1,_tmpC2,_tmpC3))_tmpBE->enumdecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*
v))Cyc_Dict_insert)(_tmpBE->enumdecls,x,_tmpC3);break;case Cyc_Absyn_Register:
_LL41:{const char*_tmp2C3;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((
_tmp2C3="add_enumdecl",_tag_dyneither(_tmp2C3,sizeof(char),13))));}break;}};}
static void Cyc_Interface_extract_xdatatypefielddecl(struct Cyc_Interface_I*i,
struct Cyc_Absyn_Datatypedecl*d,struct Cyc_Absyn_Datatypefield*f){struct Cyc_List_List*
_tmpCD=0;{union Cyc_Absyn_Nmspace _tmpCE=(*f->name).f1;struct Cyc_List_List*_tmpCF;
struct Cyc_List_List*_tmpD0;struct Cyc_List_List*_tmpD1;_LL44: if((_tmpCE.Rel_n).tag
!= 1)goto _LL46;_tmpCF=(struct Cyc_List_List*)(_tmpCE.Rel_n).val;_LL45: _tmpD0=
_tmpCF;goto _LL47;_LL46: if((_tmpCE.Abs_n).tag != 2)goto _LL48;_tmpD0=(struct Cyc_List_List*)(
_tmpCE.Abs_n).val;_LL47: _tmpCD=_tmpD0;goto _LL43;_LL48: if((_tmpCE.C_n).tag != 3)
goto _LL4A;_tmpD1=(struct Cyc_List_List*)(_tmpCE.C_n).val;_LL49: _tmpCD=_tmpD1;goto
_LL43;_LL4A:;_LL4B: goto _LL43;_LL43:;}{struct Cyc_Dict_Dict*dict;switch(f->sc){
case Cyc_Absyn_Static: _LL4C: return;case Cyc_Absyn_Extern: _LL4D: dict=& i->imports;
break;case Cyc_Absyn_Public: _LL4E: dict=& i->exports;break;default: _LL4F: {const
char*_tmp2C4;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((
_tmp2C4="add_xdatatypefielddecl",_tag_dyneither(_tmp2C4,sizeof(char),23))));}}{
struct Cyc_Interface_Ienv*env;{struct _handler_cons _tmpD3;_push_handler(& _tmpD3);{
int _tmpD5=0;if(setjmp(_tmpD3.handler))_tmpD5=1;if(!_tmpD5){env=((struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(*dict,_tmpCD);;
_pop_handler();}else{void*_tmpD4=(void*)_exn_thrown;void*_tmpD7=_tmpD4;_LL52: {
struct Cyc_Dict_Absent_exn_struct*_tmpD8=(struct Cyc_Dict_Absent_exn_struct*)
_tmpD7;if(_tmpD8->tag != Cyc_Dict_Absent)goto _LL54;}_LL53: env=Cyc_Interface_new_ienv();*
dict=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct
Cyc_Interface_Ienv*v))Cyc_Dict_insert)(*dict,_tmpCD,env);goto _LL51;_LL54:;_LL55:(
void)_throw(_tmpD7);_LL51:;}};}{struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp2C5;env->xdatatypefielddecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Tcdecl_Xdatatypefielddecl*
v))Cyc_Dict_insert)(env->xdatatypefielddecls,(*f->name).f2,((_tmp2C5=_cycalloc(
sizeof(*_tmp2C5)),((_tmp2C5->base=d,((_tmp2C5->field=f,_tmp2C5)))))));};};};}
static void Cyc_Interface_extract_datatypedecl(struct _tuple21*env,struct
_dyneither_ptr*x,struct Cyc_Absyn_Datatypedecl**dp){struct _tuple21 _tmpDB;struct
Cyc_Interface_Ienv*_tmpDC;struct Cyc_Interface_Ienv*_tmpDD;int _tmpDE;struct Cyc_Dict_Dict
_tmpDF;struct Cyc_Interface_Seen*_tmpE0;struct Cyc_Interface_I*_tmpE1;struct
_tuple21*_tmpDA=env;_tmpDB=*_tmpDA;_tmpDC=_tmpDB.f1;_tmpDD=_tmpDB.f2;_tmpDE=
_tmpDB.f3;_tmpDF=_tmpDB.f4;_tmpE0=_tmpDB.f5;_tmpE1=_tmpDB.f6;{struct Cyc_Absyn_Datatypedecl*
_tmpE2=*dp;switch(_tmpE2->sc){case Cyc_Absyn_Static: _LL56: if((!_tmpE2->is_extensible
 && _tmpDE) && _tmpE2->fields == 0){const char*_tmp2C7;const char*_tmp2C6;Cyc_Interface_fields_err(((
_tmp2C6="static",_tag_dyneither(_tmp2C6,sizeof(char),7))),((_tmp2C7="datatype",
_tag_dyneither(_tmp2C7,sizeof(char),9))),_tmpE2->name);}break;case Cyc_Absyn_Abstract:
_LL57: if(_tmpE2->fields == 0){if(!_tmpE2->is_extensible  && _tmpDE){const char*
_tmp2C9;const char*_tmp2C8;Cyc_Interface_fields_err(((_tmp2C8="abstract",
_tag_dyneither(_tmp2C8,sizeof(char),9))),((_tmp2C9="datatype",_tag_dyneither(
_tmp2C9,sizeof(char),9))),_tmpE2->name);}}else{{struct Cyc_Absyn_Datatypedecl*
_tmp2CA;_tmpE2=((_tmp2CA=_cycalloc(sizeof(*_tmp2CA)),((_tmp2CA[0]=*_tmpE2,
_tmp2CA))));}_tmpE2->fields=0;}if(Cyc_Interface_check_public_datatypedecl(_tmpDF,
_tmpE0,_tmpE2))_tmpDD->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpDD->datatypedecls,
x,_tmpE2);break;case Cyc_Absyn_Public: _LL58:{struct Cyc_Absyn_Datatypedecl*_tmp2CB;
_tmpE2=((_tmp2CB=_cycalloc(sizeof(*_tmp2CB)),((_tmp2CB[0]=*_tmpE2,_tmp2CB))));}
if(!_tmpE2->is_extensible  && _tmpE2->fields == 0){{const char*_tmp2CD;const char*
_tmp2CC;Cyc_Interface_fields_err(((_tmp2CC="public",_tag_dyneither(_tmp2CC,
sizeof(char),7))),((_tmp2CD="datatype",_tag_dyneither(_tmp2CD,sizeof(char),9))),
_tmpE2->name);}_tmpE2->sc=Cyc_Absyn_Abstract;}if(Cyc_Interface_check_public_datatypedecl(
_tmpDF,_tmpE0,_tmpE2)){if(_tmpE2->is_extensible  && _tmpE2->fields != 0){struct Cyc_List_List*
_tmpEB=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpE2->fields))->v;
_tmpE2->fields=0;{struct Cyc_List_List*_tmpEC=_tmpEB;for(0;_tmpEC != 0;_tmpEC=
_tmpEC->tl){Cyc_Interface_extract_xdatatypefielddecl(_tmpE1,_tmpE2,(struct Cyc_Absyn_Datatypefield*)
_tmpEC->hd);}};}_tmpDD->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpDD->datatypedecls,
x,_tmpE2);}break;case Cyc_Absyn_ExternC: _LL59: {const char*_tmp2CE;((int(*)(struct
_dyneither_ptr s))Cyc_Interface_invalid_arg)(((_tmp2CE="extract_datatypedecl",
_tag_dyneither(_tmp2CE,sizeof(char),21))));}case Cyc_Absyn_Extern: _LL5A: if(Cyc_Interface_check_public_datatypedecl(
_tmpDF,_tmpE0,_tmpE2)){if(_tmpE2->is_extensible  && _tmpE2->fields != 0){{struct
Cyc_Absyn_Datatypedecl*_tmp2CF;_tmpE2=((_tmp2CF=_cycalloc(sizeof(*_tmp2CF)),((
_tmp2CF[0]=*_tmpE2,_tmp2CF))));}{struct Cyc_List_List*_tmpEF=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmpE2->fields))->v;_tmpE2->fields=0;{struct Cyc_List_List*
_tmpF0=_tmpEF;for(0;_tmpF0 != 0;_tmpF0=_tmpF0->tl){Cyc_Interface_extract_xdatatypefielddecl(
_tmpE1,_tmpE2,(struct Cyc_Absyn_Datatypefield*)_tmpF0->hd);}};};}_tmpDC->datatypedecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*
v))Cyc_Dict_insert)(_tmpDC->datatypedecls,x,_tmpE2);}break;case Cyc_Absyn_Register:
_LL5B:{const char*_tmp2D0;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((
_tmp2D0="add_datatypedecl",_tag_dyneither(_tmp2D0,sizeof(char),17))));}break;}};}
static void Cyc_Interface_extract_typedef(struct _tuple21*env,struct _dyneither_ptr*
x,struct Cyc_Absyn_Typedefdecl*d){struct _tuple21 _tmpF3;struct Cyc_Interface_Ienv*
_tmpF4;struct Cyc_Dict_Dict _tmpF5;struct Cyc_Interface_Seen*_tmpF6;struct _tuple21*
_tmpF2=env;_tmpF3=*_tmpF2;_tmpF4=_tmpF3.f2;_tmpF5=_tmpF3.f4;_tmpF6=_tmpF3.f5;if(
Cyc_Interface_check_public_typedefdecl(_tmpF5,_tmpF6,d))_tmpF4->typedefdecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*
v))Cyc_Dict_insert)(_tmpF4->typedefdecls,x,d);}struct _tuple22{void*f1;int f2;};
static void Cyc_Interface_extract_ordinarie(struct _tuple21*env,struct
_dyneither_ptr*x,struct _tuple22*v){struct _tuple21 _tmpF8;struct Cyc_Interface_Ienv*
_tmpF9;struct Cyc_Interface_Ienv*_tmpFA;int _tmpFB;struct Cyc_Dict_Dict _tmpFC;
struct Cyc_Interface_Seen*_tmpFD;struct _tuple21*_tmpF7=env;_tmpF8=*_tmpF7;_tmpF9=
_tmpF8.f1;_tmpFA=_tmpF8.f2;_tmpFB=_tmpF8.f3;_tmpFC=_tmpF8.f4;_tmpFD=_tmpF8.f5;{
void*_tmpFE=(*v).f1;void*_tmpFF=_tmpFE;void*_tmp101;_LL5E: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*
_tmp100=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpFF;if(_tmp100->tag != 
0)goto _LL60;else{_tmp101=(void*)_tmp100->f1;}}_LL5F:{void*_tmp102=_tmp101;struct
Cyc_Absyn_Fndecl*_tmp104;struct Cyc_Absyn_Vardecl*_tmp106;_LL63: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*
_tmp103=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp102;if(_tmp103->tag
!= 2)goto _LL65;else{_tmp104=_tmp103->f1;}}_LL64: {struct Cyc_Absyn_Vardecl*
_tmp2D1;struct Cyc_Absyn_Vardecl*_tmp107=(_tmp2D1=_cycalloc(sizeof(*_tmp2D1)),((
_tmp2D1->sc=_tmp104->sc,((_tmp2D1->name=_tmp104->name,((_tmp2D1->tq=Cyc_Absyn_empty_tqual(
0),((_tmp2D1->type=(void*)_check_null(_tmp104->cached_typ),((_tmp2D1->initializer=
0,((_tmp2D1->rgn=0,((_tmp2D1->attributes=0,((_tmp2D1->escapes=0,_tmp2D1)))))))))))))))));
_tmpFB=0;_tmp106=_tmp107;goto _LL66;}_LL65: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmp105=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp102;if(_tmp105->tag
!= 1)goto _LL67;else{_tmp106=_tmp105->f1;}}_LL66: if(_tmp106->initializer != 0){{
struct Cyc_Absyn_Vardecl*_tmp2D2;_tmp106=((_tmp2D2=_cycalloc(sizeof(*_tmp2D2)),((
_tmp2D2[0]=*_tmp106,_tmp2D2))));}_tmp106->initializer=0;}switch(_tmp106->sc){
case Cyc_Absyn_Static: _LL69: if(_tmpFB  && Cyc_Tcutil_is_function_type(_tmp106->type)){
const char*_tmp2D3;Cyc_Interface_body_err(((_tmp2D3="static",_tag_dyneither(
_tmp2D3,sizeof(char),7))),_tmp106->name);}break;case Cyc_Absyn_Register: _LL6A:
goto _LL6B;case Cyc_Absyn_Abstract: _LL6B: {const char*_tmp2D4;((int(*)(struct
_dyneither_ptr s))Cyc_Interface_invalid_arg)(((_tmp2D4="extract_ordinarie",
_tag_dyneither(_tmp2D4,sizeof(char),18))));}case Cyc_Absyn_Public: _LL6C: if(_tmpFB
 && Cyc_Tcutil_is_function_type(_tmp106->type)){const char*_tmp2D5;Cyc_Interface_body_err(((
_tmp2D5="public",_tag_dyneither(_tmp2D5,sizeof(char),7))),_tmp106->name);}if(Cyc_Interface_check_public_vardecl(
_tmpFC,_tmpFD,_tmp106))_tmpFA->vardecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpFA->vardecls,
x,_tmp106);break;case Cyc_Absyn_ExternC: _LL6D: goto _LL6E;case Cyc_Absyn_Extern:
_LL6E: if(Cyc_Interface_check_public_vardecl(_tmpFC,_tmpFD,_tmp106))_tmpF9->vardecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Vardecl*
v))Cyc_Dict_insert)(_tmpF9->vardecls,x,_tmp106);break;}goto _LL62;_LL67:;_LL68:
goto _LL62;_LL62:;}goto _LL5D;_LL60:;_LL61: goto _LL5D;_LL5D:;};}struct _tuple23{
struct Cyc_Interface_I*f1;int f2;struct Cyc_Dict_Dict f3;struct Cyc_Interface_Seen*f4;
};static void Cyc_Interface_extract_f(struct _tuple23*env_f,struct Cyc_List_List*ns,
struct Cyc_Tcenv_Genv*ge){struct _tuple23 _tmp10E;struct Cyc_Interface_I*_tmp10F;int
_tmp110;struct Cyc_Dict_Dict _tmp111;struct Cyc_Interface_Seen*_tmp112;struct
_tuple23*_tmp10D=env_f;_tmp10E=*_tmp10D;_tmp10F=_tmp10E.f1;_tmp110=_tmp10E.f2;
_tmp111=_tmp10E.f3;_tmp112=_tmp10E.f4;{struct _tuple21 _tmp2D6;struct _tuple21
_tmp113=(_tmp2D6.f1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(_tmp10F->imports,ns),((_tmp2D6.f2=((struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp10F->exports,ns),((
_tmp2D6.f3=_tmp110,((_tmp2D6.f4=_tmp111,((_tmp2D6.f5=_tmp112,((_tmp2D6.f6=
_tmp10F,_tmp2D6)))))))))));((void(*)(void(*f)(struct _tuple21*,struct
_dyneither_ptr*,struct Cyc_Absyn_Aggrdecl**),struct _tuple21*env,struct Cyc_Dict_Dict
d))Cyc_Dict_iter_c)(Cyc_Interface_extract_aggrdecl,& _tmp113,ge->aggrdecls);((
void(*)(void(*f)(struct _tuple21*,struct _dyneither_ptr*,struct Cyc_Absyn_Datatypedecl**),
struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_datatypedecl,&
_tmp113,ge->datatypedecls);((void(*)(void(*f)(struct _tuple21*,struct
_dyneither_ptr*,struct Cyc_Absyn_Enumdecl**),struct _tuple21*env,struct Cyc_Dict_Dict
d))Cyc_Dict_iter_c)(Cyc_Interface_extract_enumdecl,& _tmp113,ge->enumdecls);((
void(*)(void(*f)(struct _tuple21*,struct _dyneither_ptr*,struct Cyc_Absyn_Typedefdecl*),
struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_typedef,&
_tmp113,ge->typedefs);((void(*)(void(*f)(struct _tuple21*,struct _dyneither_ptr*,
struct _tuple22*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_ordinarie,&
_tmp113,ge->ordinaries);};}static struct Cyc_Interface_I*Cyc_Interface_gen_extract(
struct Cyc_Dict_Dict ae,int check_complete_defs){struct _tuple23 _tmp2D7;struct
_tuple23 env=(_tmp2D7.f1=((struct Cyc_Interface_I*(*)(struct Cyc_Dict_Dict skel))Cyc_Interface_skel2i)(
ae),((_tmp2D7.f2=check_complete_defs,((_tmp2D7.f3=ae,((_tmp2D7.f4=Cyc_Interface_new_seen(),
_tmp2D7)))))));((void(*)(void(*f)(struct _tuple23*,struct Cyc_List_List*,struct Cyc_Tcenv_Genv*),
struct _tuple23*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_f,&
env,ae);return env.f1;}struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict
ae){return Cyc_Interface_gen_extract(ae,1);}inline static void Cyc_Interface_check_err(
struct _dyneither_ptr*msg1,struct _dyneither_ptr msg2){const char*_tmp2DB;void*
_tmp2DA[1];struct Cyc_String_pa_PrintArg_struct _tmp2D9;(_tmp2D9.tag=0,((_tmp2D9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)msg2),((_tmp2DA[0]=& _tmp2D9,Cyc_Tcdecl_merr(
0,msg1,((_tmp2DB="%s",_tag_dyneither(_tmp2DB,sizeof(char),3))),_tag_dyneither(
_tmp2DA,sizeof(void*),1)))))));}struct _tuple24{int f1;struct Cyc_Dict_Dict f2;int(*
f3)(void*,void*,struct _dyneither_ptr*);struct _dyneither_ptr f4;struct
_dyneither_ptr*f5;};static void Cyc_Interface_incl_dict_f(struct _tuple24*env,
struct _dyneither_ptr*x,void*y1){struct _tuple24 _tmp11A;int _tmp11B;int*_tmp11C;
struct Cyc_Dict_Dict _tmp11D;int(*_tmp11E)(void*,void*,struct _dyneither_ptr*);
struct _dyneither_ptr _tmp11F;struct _dyneither_ptr*_tmp120;struct _tuple24*_tmp119=
env;_tmp11A=*_tmp119;_tmp11B=_tmp11A.f1;_tmp11C=(int*)&(*_tmp119).f1;_tmp11D=
_tmp11A.f2;_tmp11E=_tmp11A.f3;_tmp11F=_tmp11A.f4;_tmp120=_tmp11A.f5;{struct
_handler_cons _tmp121;_push_handler(& _tmp121);{int _tmp123=0;if(setjmp(_tmp121.handler))
_tmp123=1;if(!_tmp123){{void*_tmp124=((void*(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(_tmp11D,x);if(!_tmp11E(y1,_tmp124,_tmp120))*
_tmp11C=0;};_pop_handler();}else{void*_tmp122=(void*)_exn_thrown;void*_tmp126=
_tmp122;_LL71: {struct Cyc_Dict_Absent_exn_struct*_tmp127=(struct Cyc_Dict_Absent_exn_struct*)
_tmp126;if(_tmp127->tag != Cyc_Dict_Absent)goto _LL73;}_LL72:{const char*_tmp2E0;
void*_tmp2DF[2];struct Cyc_String_pa_PrintArg_struct _tmp2DE;struct Cyc_String_pa_PrintArg_struct
_tmp2DD;Cyc_Interface_check_err(_tmp120,(struct _dyneither_ptr)((_tmp2DD.tag=0,((
_tmp2DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*x),((_tmp2DE.tag=0,((
_tmp2DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp11F),((_tmp2DF[0]=&
_tmp2DE,((_tmp2DF[1]=& _tmp2DD,Cyc_aprintf(((_tmp2E0="%s %s is missing",
_tag_dyneither(_tmp2E0,sizeof(char),17))),_tag_dyneither(_tmp2DF,sizeof(void*),2)))))))))))))));}*
_tmp11C=0;goto _LL70;_LL73:;_LL74:(void)_throw(_tmp126);_LL70:;}};};}static int Cyc_Interface_incl_dict(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(void*,void*,struct
_dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg){struct _tuple24
_tmp2E1;struct _tuple24 _tmp12C=(_tmp2E1.f1=1,((_tmp2E1.f2=dic2,((_tmp2E1.f3=
incl_f,((_tmp2E1.f4=t,((_tmp2E1.f5=msg,_tmp2E1)))))))));((void(*)(void(*f)(
struct _tuple24*,struct _dyneither_ptr*,void*),struct _tuple24*env,struct Cyc_Dict_Dict
d))Cyc_Dict_iter_c)(Cyc_Interface_incl_dict_f,& _tmp12C,dic1);return _tmp12C.f1;}
static int Cyc_Interface_incl_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*
d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Aggrdecl*_tmp12E=Cyc_Tcdecl_merge_aggrdecl(
d0,d1,0,msg);if(_tmp12E == 0)return 0;if((struct Cyc_Absyn_Aggrdecl*)d0 != _tmp12E){{
const char*_tmp2E5;void*_tmp2E4[1];struct Cyc_String_pa_PrintArg_struct _tmp2E3;Cyc_Interface_check_err(
msg,(struct _dyneither_ptr)((_tmp2E3.tag=0,((_tmp2E3.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name)),((_tmp2E4[0]=& _tmp2E3,
Cyc_aprintf(((_tmp2E5="declaration of type %s discloses too much information",
_tag_dyneither(_tmp2E5,sizeof(char),54))),_tag_dyneither(_tmp2E4,sizeof(void*),1)))))))));}
return 0;}return 1;}static int Cyc_Interface_incl_datatypedecl(struct Cyc_Absyn_Datatypedecl*
d0,struct Cyc_Absyn_Datatypedecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Datatypedecl*
_tmp132=Cyc_Tcdecl_merge_datatypedecl(d0,d1,0,msg);if(_tmp132 == 0)return 0;if((
struct Cyc_Absyn_Datatypedecl*)d0 != _tmp132){{const char*_tmp2E9;void*_tmp2E8[1];
struct Cyc_String_pa_PrintArg_struct _tmp2E7;Cyc_Interface_check_err(msg,(struct
_dyneither_ptr)((_tmp2E7.tag=0,((_tmp2E7.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name)),((_tmp2E8[0]=& _tmp2E7,Cyc_aprintf(((
_tmp2E9="declaration of datatype %s discloses too much information",
_tag_dyneither(_tmp2E9,sizeof(char),58))),_tag_dyneither(_tmp2E8,sizeof(void*),1)))))))));}
return 0;}return 1;}static int Cyc_Interface_incl_enumdecl(struct Cyc_Absyn_Enumdecl*
d0,struct Cyc_Absyn_Enumdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Enumdecl*
_tmp136=Cyc_Tcdecl_merge_enumdecl(d0,d1,0,msg);if(_tmp136 == 0)return 0;if((struct
Cyc_Absyn_Enumdecl*)d0 != _tmp136){{const char*_tmp2ED;void*_tmp2EC[1];struct Cyc_String_pa_PrintArg_struct
_tmp2EB;Cyc_Interface_check_err(msg,(struct _dyneither_ptr)((_tmp2EB.tag=0,((
_tmp2EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
d1->name)),((_tmp2EC[0]=& _tmp2EB,Cyc_aprintf(((_tmp2ED="declaration of enum %s discloses too much information",
_tag_dyneither(_tmp2ED,sizeof(char),54))),_tag_dyneither(_tmp2EC,sizeof(void*),1)))))))));}
return 0;}return 1;}static int Cyc_Interface_incl_vardecl(struct Cyc_Absyn_Vardecl*d0,
struct Cyc_Absyn_Vardecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Vardecl*
_tmp13A=Cyc_Tcdecl_merge_vardecl(d0,d1,0,msg);if(_tmp13A == 0)return 0;if((struct
Cyc_Absyn_Vardecl*)d0 != _tmp13A){{const char*_tmp2F1;void*_tmp2F0[1];struct Cyc_String_pa_PrintArg_struct
_tmp2EF;Cyc_Interface_check_err(msg,(struct _dyneither_ptr)((_tmp2EF.tag=0,((
_tmp2EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
d1->name)),((_tmp2F0[0]=& _tmp2EF,Cyc_aprintf(((_tmp2F1="declaration of variable %s discloses too much information",
_tag_dyneither(_tmp2F1,sizeof(char),58))),_tag_dyneither(_tmp2F0,sizeof(void*),1)))))))));}
return 0;}return 1;}static int Cyc_Interface_incl_typedefdecl(struct Cyc_Absyn_Typedefdecl*
d0,struct Cyc_Absyn_Typedefdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Typedefdecl*
_tmp13E=Cyc_Tcdecl_merge_typedefdecl(d0,d1,0,msg);if(_tmp13E == 0)return 0;if((
struct Cyc_Absyn_Typedefdecl*)d0 != _tmp13E){{const char*_tmp2F5;void*_tmp2F4[1];
struct Cyc_String_pa_PrintArg_struct _tmp2F3;Cyc_Interface_check_err(msg,(struct
_dyneither_ptr)((_tmp2F3.tag=0,((_tmp2F3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name)),((_tmp2F4[0]=& _tmp2F3,Cyc_aprintf(((
_tmp2F5="declaration of typedef %s discloses too much information",
_tag_dyneither(_tmp2F5,sizeof(char),57))),_tag_dyneither(_tmp2F4,sizeof(void*),1)))))))));}
return 0;}return 1;}static int Cyc_Interface_incl_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*
d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct _dyneither_ptr*msg){struct Cyc_Tcdecl_Xdatatypefielddecl*
_tmp142=Cyc_Tcdecl_merge_xdatatypefielddecl(d0,d1,0,msg);if(_tmp142 == 0)return 0;
if((struct Cyc_Tcdecl_Xdatatypefielddecl*)d0 != _tmp142){{const char*_tmp2F9;void*
_tmp2F8[1];struct Cyc_String_pa_PrintArg_struct _tmp2F7;Cyc_Interface_check_err(
msg,(struct _dyneither_ptr)((_tmp2F7.tag=0,((_tmp2F7.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string((d1->field)->name)),((_tmp2F8[0]=&
_tmp2F7,Cyc_aprintf(((_tmp2F9="declaration of xdatatypefield %s discloses too much information",
_tag_dyneither(_tmp2F9,sizeof(char),64))),_tag_dyneither(_tmp2F8,sizeof(void*),1)))))))));}
return 0;}return 1;}struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,
unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*
msg);static int Cyc_Interface_incl_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*
ie2,struct _dyneither_ptr*msg){const char*_tmp2FA;int _tmp146=((int(*)(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,
struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(
ie1->aggrdecls,ie2->aggrdecls,Cyc_Interface_incl_aggrdecl,((_tmp2FA="type",
_tag_dyneither(_tmp2FA,sizeof(char),5))),msg);const char*_tmp2FB;int _tmp147=((int(*)(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Datatypedecl*,
struct Cyc_Absyn_Datatypedecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,
struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->datatypedecls,ie2->datatypedecls,
Cyc_Interface_incl_datatypedecl,((_tmp2FB="datatype",_tag_dyneither(_tmp2FB,
sizeof(char),9))),msg);const char*_tmp2FC;int _tmp148=((int(*)(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,
struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(
ie1->enumdecls,ie2->enumdecls,Cyc_Interface_incl_enumdecl,((_tmp2FC="enum",
_tag_dyneither(_tmp2FC,sizeof(char),5))),msg);const char*_tmp2FD;int _tmp149=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct
Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*,unsigned int,struct _dyneither_ptr*),struct
_dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->typedefdecls,
ie2->typedefdecls,((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),Cyc_Tcdecl_merge_typedefdecl,((
_tmp2FD="typedef",_tag_dyneither(_tmp2FD,sizeof(char),8))),msg)!= 0;const char*
_tmp2FE;int _tmp14A=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*
incl_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,struct _dyneither_ptr*),
struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->vardecls,
ie2->vardecls,Cyc_Interface_incl_vardecl,((_tmp2FE="variable",_tag_dyneither(
_tmp2FE,sizeof(char),9))),msg);const char*_tmp2FF;int _tmp14B=((int(*)(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Tcdecl_Xdatatypefielddecl*,
struct Cyc_Tcdecl_Xdatatypefielddecl*,struct _dyneither_ptr*),struct _dyneither_ptr
t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->xdatatypefielddecls,
ie2->xdatatypefielddecls,Cyc_Interface_incl_xdatatypefielddecl,((_tmp2FF="xdatatypefield",
_tag_dyneither(_tmp2FF,sizeof(char),15))),msg);return((((_tmp146  && _tmp147) && 
_tmp148) && _tmp149) && _tmp14A) && _tmp14B;}struct _tuple25{int f1;struct Cyc_Dict_Dict
f2;struct _dyneither_ptr*f3;};static void Cyc_Interface_incl_ns_f(struct _tuple25*
env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie1){struct _tuple25 _tmp153;
int _tmp154;int*_tmp155;struct Cyc_Dict_Dict _tmp156;struct _dyneither_ptr*_tmp157;
struct _tuple25*_tmp152=env;_tmp153=*_tmp152;_tmp154=_tmp153.f1;_tmp155=(int*)&(*
_tmp152).f1;_tmp156=_tmp153.f2;_tmp157=_tmp153.f3;{struct Cyc_Interface_Ienv*ie2;{
struct _handler_cons _tmp158;_push_handler(& _tmp158);{int _tmp15A=0;if(setjmp(
_tmp158.handler))_tmp15A=1;if(!_tmp15A){ie2=((struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp156,ns);;
_pop_handler();}else{void*_tmp159=(void*)_exn_thrown;void*_tmp15C=_tmp159;_LL76: {
struct Cyc_Dict_Absent_exn_struct*_tmp15D=(struct Cyc_Dict_Absent_exn_struct*)
_tmp15C;if(_tmp15D->tag != Cyc_Dict_Absent)goto _LL78;}_LL77: ie2=Cyc_Interface_lazy_new_ienv();
goto _LL75;_LL78:;_LL79:(void)_throw(_tmp15C);_LL75:;}};}if(!Cyc_Interface_incl_ienv(
ie1,ie2,_tmp157))*_tmp155=0;};}static int Cyc_Interface_incl_ns(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr*msg){struct _tuple25 _tmp300;
struct _tuple25 _tmp15E=(_tmp300.f1=1,((_tmp300.f2=dic2,((_tmp300.f3=msg,_tmp300)))));((
void(*)(void(*f)(struct _tuple25*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_incl_ns_f,&
_tmp15E,dic1);return _tmp15E.f1;}int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*
i1,struct Cyc_Interface_I*i2,struct _tuple10*info){struct _handler_cons _tmp160;
_push_handler(& _tmp160);{int _tmp162=0;if(setjmp(_tmp160.handler))_tmp162=1;if(!
_tmp162){{int _tmp163=1;struct _dyneither_ptr*msg=0;if(info != 0){struct Cyc_String_pa_PrintArg_struct
_tmp30A;struct Cyc_String_pa_PrintArg_struct _tmp309;void*_tmp308[2];const char*
_tmp307;struct _dyneither_ptr*_tmp306;msg=((_tmp306=_cycalloc(sizeof(*_tmp306)),((
_tmp306[0]=(struct _dyneither_ptr)((_tmp30A.tag=0,((_tmp30A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)(*info).f2),((_tmp309.tag=0,((_tmp309.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1),((_tmp308[0]=& _tmp309,((
_tmp308[1]=& _tmp30A,Cyc_aprintf(((_tmp307="checking inclusion of %s exports into %s exports,",
_tag_dyneither(_tmp307,sizeof(char),50))),_tag_dyneither(_tmp308,sizeof(void*),2)))))))))))))),
_tmp306))));}if(!Cyc_Interface_incl_ns(i1->exports,i2->exports,msg))_tmp163=0;
if(info != 0){struct Cyc_String_pa_PrintArg_struct _tmp314;struct Cyc_String_pa_PrintArg_struct
_tmp313;void*_tmp312[2];const char*_tmp311;struct _dyneither_ptr*_tmp310;msg=((
_tmp310=_cycalloc(sizeof(*_tmp310)),((_tmp310[0]=(struct _dyneither_ptr)((_tmp314.tag=
0,((_tmp314.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1),((
_tmp313.tag=0,((_tmp313.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2),((
_tmp312[0]=& _tmp313,((_tmp312[1]=& _tmp314,Cyc_aprintf(((_tmp311="checking inclusion of %s imports into %s imports,",
_tag_dyneither(_tmp311,sizeof(char),50))),_tag_dyneither(_tmp312,sizeof(void*),2)))))))))))))),
_tmp310))));}if(!Cyc_Interface_incl_ns(i2->imports,i1->imports,msg))_tmp163=0;{
int _tmp16E=_tmp163;_npop_handler(0);return _tmp16E;};};_pop_handler();}else{void*
_tmp161=(void*)_exn_thrown;void*_tmp170=_tmp161;_LL7B: {struct Cyc_Tcdecl_Incompatible_exn_struct*
_tmp171=(struct Cyc_Tcdecl_Incompatible_exn_struct*)_tmp170;if(_tmp171->tag != Cyc_Tcdecl_Incompatible)
goto _LL7D;}_LL7C: return 0;_LL7D:;_LL7E:(void)_throw(_tmp170);_LL7A:;}};}struct
_tuple26{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct Cyc_Dict_Dict f4;
void*(*f5)(void*,void*,unsigned int,struct _dyneither_ptr*);struct _dyneither_ptr
f6;struct _dyneither_ptr*f7;};void Cyc_Interface_compat_merge_dict_f(struct
_tuple26*env,struct _dyneither_ptr*x,void*y2){struct _tuple26 _tmp173;int _tmp174;
int*_tmp175;struct Cyc_Dict_Dict _tmp176;struct Cyc_Dict_Dict*_tmp177;struct Cyc_Dict_Dict
_tmp178;struct Cyc_Dict_Dict _tmp179;void*(*_tmp17A)(void*,void*,unsigned int,
struct _dyneither_ptr*);struct _dyneither_ptr _tmp17B;struct _dyneither_ptr*_tmp17C;
struct _tuple26*_tmp172=env;_tmp173=*_tmp172;_tmp174=_tmp173.f1;_tmp175=(int*)&(*
_tmp172).f1;_tmp176=_tmp173.f2;_tmp177=(struct Cyc_Dict_Dict*)&(*_tmp172).f2;
_tmp178=_tmp173.f3;_tmp179=_tmp173.f4;_tmp17A=_tmp173.f5;_tmp17B=_tmp173.f6;
_tmp17C=_tmp173.f7;{void*y;{struct _handler_cons _tmp17D;_push_handler(& _tmp17D);{
int _tmp17F=0;if(setjmp(_tmp17D.handler))_tmp17F=1;if(!_tmp17F){{void*_tmp180=((
void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp178,x);
void*_tmp181=_tmp17A(_tmp180,y2,0,_tmp17C);if(!((unsigned int)_tmp181)){*_tmp175=
0;_npop_handler(0);return;}y=(void*)_tmp181;};_pop_handler();}else{void*_tmp17E=(
void*)_exn_thrown;void*_tmp183=_tmp17E;_LL80: {struct Cyc_Dict_Absent_exn_struct*
_tmp184=(struct Cyc_Dict_Absent_exn_struct*)_tmp183;if(_tmp184->tag != Cyc_Dict_Absent)
goto _LL82;}_LL81: y=y2;goto _LL7F;_LL82:;_LL83:(void)_throw(_tmp183);_LL7F:;}};}{
struct _handler_cons _tmp185;_push_handler(& _tmp185);{int _tmp187=0;if(setjmp(
_tmp185.handler))_tmp187=1;if(!_tmp187){{void*_tmp188=((void*(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp179,x);void*_tmp189=_tmp17A(
_tmp188,y,0,_tmp17C);if(_tmp189 != (void*)_tmp188){if((unsigned int)_tmp189){
const char*_tmp319;void*_tmp318[2];struct Cyc_String_pa_PrintArg_struct _tmp317;
struct Cyc_String_pa_PrintArg_struct _tmp316;Cyc_Interface_check_err(_tmp17C,(
struct _dyneither_ptr)((_tmp316.tag=0,((_tmp316.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*x),((_tmp317.tag=0,((_tmp317.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp17B),((_tmp318[0]=& _tmp317,((_tmp318[1]=& _tmp316,Cyc_aprintf(((
_tmp319="abstract %s %s is being imported as non-abstract",_tag_dyneither(
_tmp319,sizeof(char),49))),_tag_dyneither(_tmp318,sizeof(void*),2)))))))))))))));}*
_tmp175=0;}};_pop_handler();}else{void*_tmp186=(void*)_exn_thrown;void*_tmp18F=
_tmp186;_LL85: {struct Cyc_Dict_Absent_exn_struct*_tmp190=(struct Cyc_Dict_Absent_exn_struct*)
_tmp18F;if(_tmp190->tag != Cyc_Dict_Absent)goto _LL87;}_LL86: if(*_tmp175)*_tmp177=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*
_tmp177,x,y);goto _LL84;_LL87:;_LL88:(void)_throw(_tmp18F);_LL84:;}};};};}struct
Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,unsigned int,struct
_dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg){struct _tuple26
_tmp31A;struct _tuple26 _tmp191=(_tmp31A.f1=1,((_tmp31A.f2=dic1,((_tmp31A.f3=dic1,((
_tmp31A.f4=excl,((_tmp31A.f5=merge_f,((_tmp31A.f6=t,((_tmp31A.f7=msg,_tmp31A)))))))))))));((
void(*)(void(*f)(struct _tuple26*,struct _dyneither_ptr*,void*),struct _tuple26*env,
struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_compat_merge_dict_f,&
_tmp191,dic2);if(_tmp191.f1){struct Cyc_Dict_Dict*_tmp31B;return(_tmp31B=
_cycalloc(sizeof(*_tmp31B)),((_tmp31B[0]=_tmp191.f2,_tmp31B)));}return 0;}struct
Cyc_Interface_Ienv*Cyc_Interface_compat_merge_ienv(struct Cyc_Interface_Ienv*ie1,
struct Cyc_Interface_Ienv*ie2,struct Cyc_Interface_Ienv*iexcl,struct _dyneither_ptr*
msg){const char*_tmp31C;struct Cyc_Dict_Dict*_tmp194=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct
Cyc_Absyn_Aggrdecl*(*merge_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,
unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*
msg))Cyc_Interface_compat_merge_dict)(ie1->aggrdecls,ie2->aggrdecls,iexcl->aggrdecls,
Cyc_Tcdecl_merge_aggrdecl,((_tmp31C="type",_tag_dyneither(_tmp31C,sizeof(char),5))),
msg);const char*_tmp31D;struct Cyc_Dict_Dict*_tmp195=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct
Cyc_Absyn_Datatypedecl*(*merge_f)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,
unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*
msg))Cyc_Interface_compat_merge_dict)(ie1->datatypedecls,ie2->datatypedecls,
iexcl->datatypedecls,Cyc_Tcdecl_merge_datatypedecl,((_tmp31D="datatype",
_tag_dyneither(_tmp31D,sizeof(char),9))),msg);const char*_tmp31E;struct Cyc_Dict_Dict*
_tmp196=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Enumdecl*(*merge_f)(struct Cyc_Absyn_Enumdecl*,
struct Cyc_Absyn_Enumdecl*,unsigned int,struct _dyneither_ptr*),struct
_dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->enumdecls,
ie2->enumdecls,iexcl->enumdecls,Cyc_Tcdecl_merge_enumdecl,((_tmp31E="enum",
_tag_dyneither(_tmp31E,sizeof(char),5))),msg);const char*_tmp31F;struct Cyc_Dict_Dict*
_tmp197=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*,unsigned int,struct _dyneither_ptr*),struct
_dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->typedefdecls,
ie2->typedefdecls,iexcl->typedefdecls,Cyc_Tcdecl_merge_typedefdecl,((_tmp31F="typedef",
_tag_dyneither(_tmp31F,sizeof(char),8))),msg);const char*_tmp320;struct Cyc_Dict_Dict*
_tmp198=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Vardecl*(*merge_f)(struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*,unsigned int,struct _dyneither_ptr*),struct
_dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->vardecls,
ie2->vardecls,iexcl->vardecls,Cyc_Tcdecl_merge_vardecl,((_tmp320="variable",
_tag_dyneither(_tmp320,sizeof(char),9))),msg);const char*_tmp321;struct Cyc_Dict_Dict*
_tmp199=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,struct Cyc_Dict_Dict excl,struct Cyc_Tcdecl_Xdatatypefielddecl*(*merge_f)(
struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,
unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*
msg))Cyc_Interface_compat_merge_dict)(ie1->xdatatypefielddecls,ie2->xdatatypefielddecls,
iexcl->xdatatypefielddecls,Cyc_Tcdecl_merge_xdatatypefielddecl,((_tmp321="xdatatypefield",
_tag_dyneither(_tmp321,sizeof(char),15))),msg);if(((((!((unsigned int)_tmp194)
 || !((unsigned int)_tmp195)) || !((unsigned int)_tmp196)) || !((unsigned int)
_tmp197)) || !((unsigned int)_tmp198)) || !((unsigned int)_tmp199))return 0;{
struct Cyc_Interface_Ienv*_tmp322;return(_tmp322=_cycalloc(sizeof(*_tmp322)),((
_tmp322->aggrdecls=*_tmp194,((_tmp322->datatypedecls=*_tmp195,((_tmp322->enumdecls=*
_tmp196,((_tmp322->typedefdecls=*_tmp197,((_tmp322->vardecls=*_tmp198,((_tmp322->xdatatypefielddecls=*
_tmp199,_tmp322)))))))))))));};}struct _tuple27{int f1;struct Cyc_Dict_Dict f2;
struct Cyc_Dict_Dict f3;struct Cyc_Dict_Dict f4;struct _dyneither_ptr*f5;};void Cyc_Interface_compat_merge_ns_f(
struct _tuple27*env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie2){struct
_tuple27 _tmp1A2;int _tmp1A3;int*_tmp1A4;struct Cyc_Dict_Dict _tmp1A5;struct Cyc_Dict_Dict*
_tmp1A6;struct Cyc_Dict_Dict _tmp1A7;struct Cyc_Dict_Dict _tmp1A8;struct
_dyneither_ptr*_tmp1A9;struct _tuple27*_tmp1A1=env;_tmp1A2=*_tmp1A1;_tmp1A3=
_tmp1A2.f1;_tmp1A4=(int*)&(*_tmp1A1).f1;_tmp1A5=_tmp1A2.f2;_tmp1A6=(struct Cyc_Dict_Dict*)&(*
_tmp1A1).f2;_tmp1A7=_tmp1A2.f3;_tmp1A8=_tmp1A2.f4;_tmp1A9=_tmp1A2.f5;{struct Cyc_Interface_Ienv*
ie1;struct Cyc_Interface_Ienv*iexcl;{struct _handler_cons _tmp1AA;_push_handler(&
_tmp1AA);{int _tmp1AC=0;if(setjmp(_tmp1AA.handler))_tmp1AC=1;if(!_tmp1AC){iexcl=((
struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
_tmp1A8,ns);;_pop_handler();}else{void*_tmp1AB=(void*)_exn_thrown;void*_tmp1AE=
_tmp1AB;_LL8A: {struct Cyc_Dict_Absent_exn_struct*_tmp1AF=(struct Cyc_Dict_Absent_exn_struct*)
_tmp1AE;if(_tmp1AF->tag != Cyc_Dict_Absent)goto _LL8C;}_LL8B: iexcl=Cyc_Interface_lazy_new_ienv();
goto _LL89;_LL8C:;_LL8D:(void)_throw(_tmp1AE);_LL89:;}};}{struct _handler_cons
_tmp1B0;_push_handler(& _tmp1B0);{int _tmp1B2=0;if(setjmp(_tmp1B0.handler))_tmp1B2=
1;if(!_tmp1B2){ie1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*
k))Cyc_Dict_lookup)(_tmp1A7,ns);{struct Cyc_Interface_Ienv*_tmp1B3=Cyc_Interface_compat_merge_ienv(
Cyc_Interface_lazy_new_ienv(),ie1,iexcl,_tmp1A9);if(_tmp1B3 == 0){*_tmp1A4=0;
_npop_handler(0);return;}ie1=(struct Cyc_Interface_Ienv*)_tmp1B3;};;_pop_handler();}
else{void*_tmp1B1=(void*)_exn_thrown;void*_tmp1B5=_tmp1B1;_LL8F: {struct Cyc_Dict_Absent_exn_struct*
_tmp1B6=(struct Cyc_Dict_Absent_exn_struct*)_tmp1B5;if(_tmp1B6->tag != Cyc_Dict_Absent)
goto _LL91;}_LL90: ie1=Cyc_Interface_lazy_new_ienv();goto _LL8E;_LL91:;_LL92:(void)
_throw(_tmp1B5);_LL8E:;}};}{struct Cyc_Interface_Ienv*_tmp1B7=Cyc_Interface_compat_merge_ienv(
ie1,ie2,iexcl,_tmp1A9);if(_tmp1B7 == 0)*_tmp1A4=0;else{if(*_tmp1A4)*_tmp1A6=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*
v))Cyc_Dict_insert)(*_tmp1A6,ns,(struct Cyc_Interface_Ienv*)_tmp1B7);}};};}struct
Cyc_Dict_Dict*Cyc_Interface_compat_merge_ns(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,struct Cyc_Dict_Dict excl,struct _dyneither_ptr*msg){struct Cyc_Dict_Dict d=((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(
Cyc_Absyn_varlist_cmp);struct _tuple27 _tmp323;struct _tuple27 _tmp1B8=(_tmp323.f1=1,((
_tmp323.f2=d,((_tmp323.f3=dic1,((_tmp323.f4=excl,((_tmp323.f5=msg,_tmp323)))))))));((
void(*)(void(*f)(struct _tuple27*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_compat_merge_ns_f,&
_tmp1B8,dic2);if(_tmp1B8.f1){struct Cyc_Dict_Dict*_tmp324;return(_tmp324=
_cycalloc(sizeof(*_tmp324)),((_tmp324[0]=_tmp1B8.f2,_tmp324)));}return 0;}struct
_tuple28{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct _dyneither_ptr
f4;struct _dyneither_ptr*f5;};void Cyc_Interface_disj_merge_dict_f(struct _tuple28*
env,struct _dyneither_ptr*x,void*y){struct _tuple28 _tmp1BC;int _tmp1BD;int*_tmp1BE;
struct Cyc_Dict_Dict _tmp1BF;struct Cyc_Dict_Dict*_tmp1C0;struct Cyc_Dict_Dict
_tmp1C1;struct _dyneither_ptr _tmp1C2;struct _dyneither_ptr*_tmp1C3;struct _tuple28*
_tmp1BB=env;_tmp1BC=*_tmp1BB;_tmp1BD=_tmp1BC.f1;_tmp1BE=(int*)&(*_tmp1BB).f1;
_tmp1BF=_tmp1BC.f2;_tmp1C0=(struct Cyc_Dict_Dict*)&(*_tmp1BB).f2;_tmp1C1=_tmp1BC.f3;
_tmp1C2=_tmp1BC.f4;_tmp1C3=_tmp1BC.f5;if(((int(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_member)(_tmp1C1,x)){{const char*_tmp329;void*_tmp328[2];
struct Cyc_String_pa_PrintArg_struct _tmp327;struct Cyc_String_pa_PrintArg_struct
_tmp326;Cyc_Interface_check_err(_tmp1C3,(struct _dyneither_ptr)((_tmp326.tag=0,((
_tmp326.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*x),((_tmp327.tag=0,((
_tmp327.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1C2),((_tmp328[0]=&
_tmp327,((_tmp328[1]=& _tmp326,Cyc_aprintf(((_tmp329="%s %s is exported more than once",
_tag_dyneither(_tmp329,sizeof(char),33))),_tag_dyneither(_tmp328,sizeof(void*),2)))))))))))))));}*
_tmp1BE=0;}else{if(*_tmp1BE)*_tmp1C0=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*_tmp1C0,x,y);}}struct Cyc_Dict_Dict*
Cyc_Interface_disj_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,
struct _dyneither_ptr t,struct _dyneither_ptr*msg){struct _tuple28 _tmp32A;struct
_tuple28 _tmp1C8=(_tmp32A.f1=1,((_tmp32A.f2=dic1,((_tmp32A.f3=dic1,((_tmp32A.f4=t,((
_tmp32A.f5=msg,_tmp32A)))))))));((void(*)(void(*f)(struct _tuple28*,struct
_dyneither_ptr*,void*),struct _tuple28*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_Interface_disj_merge_dict_f,& _tmp1C8,dic2);if(_tmp1C8.f1){struct Cyc_Dict_Dict*
_tmp32B;return(_tmp32B=_cycalloc(sizeof(*_tmp32B)),((_tmp32B[0]=_tmp1C8.f2,
_tmp32B)));}return 0;}struct Cyc_Interface_Ienv*Cyc_Interface_disj_merge_ienv(
struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _dyneither_ptr*
msg){const char*_tmp32C;struct Cyc_Dict_Dict*_tmp1CB=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct
Cyc_Absyn_Aggrdecl*(*merge_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,
unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*
msg))Cyc_Interface_compat_merge_dict)(ie1->aggrdecls,ie2->aggrdecls,((struct Cyc_Dict_Dict(*)(
int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),
Cyc_Tcdecl_merge_aggrdecl,((_tmp32C="type",_tag_dyneither(_tmp32C,sizeof(char),5))),
msg);const char*_tmp32D;struct Cyc_Dict_Dict*_tmp1CC=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct
_dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->datatypedecls,ie2->datatypedecls,((
_tmp32D="datatype",_tag_dyneither(_tmp32D,sizeof(char),9))),msg);const char*
_tmp32E;struct Cyc_Dict_Dict*_tmp1CD=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(
ie1->enumdecls,ie2->enumdecls,((_tmp32E="enum",_tag_dyneither(_tmp32E,sizeof(
char),5))),msg);const char*_tmp32F;struct Cyc_Dict_Dict*_tmp1CE=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct
Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,
unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*
msg))Cyc_Interface_compat_merge_dict)(ie1->typedefdecls,ie2->typedefdecls,((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))
Cyc_Dict_empty)(Cyc_strptrcmp),Cyc_Tcdecl_merge_typedefdecl,((_tmp32F="typedef",
_tag_dyneither(_tmp32F,sizeof(char),8))),msg);const char*_tmp330;struct Cyc_Dict_Dict*
_tmp1CF=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(
ie1->vardecls,ie2->vardecls,((_tmp330="variable",_tag_dyneither(_tmp330,sizeof(
char),9))),msg);const char*_tmp331;struct Cyc_Dict_Dict*_tmp1D0=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct
_dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->xdatatypefielddecls,ie2->xdatatypefielddecls,((
_tmp331="xdatatypefield",_tag_dyneither(_tmp331,sizeof(char),15))),msg);if(((((!((
unsigned int)_tmp1CB) || !((unsigned int)_tmp1CC)) || !((unsigned int)_tmp1CD))
 || !((unsigned int)_tmp1CE)) || !((unsigned int)_tmp1CF)) || !((unsigned int)
_tmp1D0))return 0;{struct Cyc_Interface_Ienv*_tmp332;return(_tmp332=_cycalloc(
sizeof(*_tmp332)),((_tmp332->aggrdecls=*_tmp1CB,((_tmp332->datatypedecls=*
_tmp1CC,((_tmp332->enumdecls=*_tmp1CD,((_tmp332->typedefdecls=*_tmp1CE,((_tmp332->vardecls=*
_tmp1CF,((_tmp332->xdatatypefielddecls=*_tmp1D0,_tmp332)))))))))))));};}struct
_tuple29{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct _dyneither_ptr*
f4;};void Cyc_Interface_disj_merge_ns_f(struct _tuple29*env,struct Cyc_List_List*ns,
struct Cyc_Interface_Ienv*ie2){struct _tuple29 _tmp1D9;int _tmp1DA;int*_tmp1DB;
struct Cyc_Dict_Dict _tmp1DC;struct Cyc_Dict_Dict*_tmp1DD;struct Cyc_Dict_Dict
_tmp1DE;struct _dyneither_ptr*_tmp1DF;struct _tuple29*_tmp1D8=env;_tmp1D9=*_tmp1D8;
_tmp1DA=_tmp1D9.f1;_tmp1DB=(int*)&(*_tmp1D8).f1;_tmp1DC=_tmp1D9.f2;_tmp1DD=(
struct Cyc_Dict_Dict*)&(*_tmp1D8).f2;_tmp1DE=_tmp1D9.f3;_tmp1DF=_tmp1D9.f4;{
struct Cyc_Interface_Ienv*ie1;{struct _handler_cons _tmp1E0;_push_handler(& _tmp1E0);{
int _tmp1E2=0;if(setjmp(_tmp1E0.handler))_tmp1E2=1;if(!_tmp1E2){ie1=((struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp1DE,ns);;
_pop_handler();}else{void*_tmp1E1=(void*)_exn_thrown;void*_tmp1E4=_tmp1E1;_LL94: {
struct Cyc_Dict_Absent_exn_struct*_tmp1E5=(struct Cyc_Dict_Absent_exn_struct*)
_tmp1E4;if(_tmp1E5->tag != Cyc_Dict_Absent)goto _LL96;}_LL95: ie1=Cyc_Interface_lazy_new_ienv();
goto _LL93;_LL96:;_LL97:(void)_throw(_tmp1E4);_LL93:;}};}{struct Cyc_Interface_Ienv*
_tmp1E6=Cyc_Interface_disj_merge_ienv(ie1,ie2,_tmp1DF);if(_tmp1E6 == 0)*_tmp1DB=0;
else{if(*_tmp1DB)*_tmp1DD=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(*_tmp1DD,ns,(struct
Cyc_Interface_Ienv*)_tmp1E6);}};};}struct Cyc_Dict_Dict*Cyc_Interface_disj_merge_ns(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr*msg){struct
_tuple29 _tmp333;struct _tuple29 _tmp1E7=(_tmp333.f1=1,((_tmp333.f2=dic1,((_tmp333.f3=
dic1,((_tmp333.f4=msg,_tmp333)))))));((void(*)(void(*f)(struct _tuple29*,struct
Cyc_List_List*,struct Cyc_Interface_Ienv*),struct _tuple29*env,struct Cyc_Dict_Dict
d))Cyc_Dict_iter_c)(Cyc_Interface_disj_merge_ns_f,& _tmp1E7,dic2);if(_tmp1E7.f1){
struct Cyc_Dict_Dict*_tmp334;return(_tmp334=_cycalloc(sizeof(*_tmp334)),((_tmp334[
0]=_tmp1E7.f2,_tmp334)));}return 0;}struct Cyc_Interface_I*Cyc_Interface_merge(
struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info){struct
_handler_cons _tmp1EA;_push_handler(& _tmp1EA);{int _tmp1EC=0;if(setjmp(_tmp1EA.handler))
_tmp1EC=1;if(!_tmp1EC){{struct _dyneither_ptr*msg=0;if(info != 0){struct Cyc_String_pa_PrintArg_struct
_tmp33E;struct Cyc_String_pa_PrintArg_struct _tmp33D;void*_tmp33C[2];const char*
_tmp33B;struct _dyneither_ptr*_tmp33A;msg=((_tmp33A=_cycalloc(sizeof(*_tmp33A)),((
_tmp33A[0]=(struct _dyneither_ptr)((_tmp33E.tag=0,((_tmp33E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)(*info).f2),((_tmp33D.tag=0,((_tmp33D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1),((_tmp33C[0]=& _tmp33D,((
_tmp33C[1]=& _tmp33E,Cyc_aprintf(((_tmp33B="merging exports of %s and %s,",
_tag_dyneither(_tmp33B,sizeof(char),30))),_tag_dyneither(_tmp33C,sizeof(void*),2)))))))))))))),
_tmp33A))));}{struct Cyc_Dict_Dict*_tmp1F2=Cyc_Interface_disj_merge_ns(i1->exports,
i2->exports,msg);if(_tmp1F2 == 0){struct Cyc_Interface_I*_tmp1F3=0;_npop_handler(0);
return _tmp1F3;}if(info != 0){struct Cyc_String_pa_PrintArg_struct _tmp348;struct Cyc_String_pa_PrintArg_struct
_tmp347;void*_tmp346[2];const char*_tmp345;struct _dyneither_ptr*_tmp344;msg=((
_tmp344=_cycalloc(sizeof(*_tmp344)),((_tmp344[0]=(struct _dyneither_ptr)((_tmp348.tag=
0,((_tmp348.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2),((
_tmp347.tag=0,((_tmp347.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1),((
_tmp346[0]=& _tmp347,((_tmp346[1]=& _tmp348,Cyc_aprintf(((_tmp345="merging imports of %s and %s,",
_tag_dyneither(_tmp345,sizeof(char),30))),_tag_dyneither(_tmp346,sizeof(void*),2)))))))))))))),
_tmp344))));}{struct Cyc_Dict_Dict*_tmp1F9=Cyc_Interface_compat_merge_ns(i1->imports,
i2->imports,*_tmp1F2,msg);if(_tmp1F9 == 0){struct Cyc_Interface_I*_tmp1FA=0;
_npop_handler(0);return _tmp1FA;}{struct Cyc_Interface_I*_tmp349;struct Cyc_Interface_I*
_tmp1FC=(_tmp349=_cycalloc(sizeof(*_tmp349)),((_tmp349->imports=*_tmp1F9,((
_tmp349->exports=*_tmp1F2,_tmp349)))));_npop_handler(0);return _tmp1FC;};};};};
_pop_handler();}else{void*_tmp1EB=(void*)_exn_thrown;void*_tmp1FE=_tmp1EB;_LL99: {
struct Cyc_Tcdecl_Incompatible_exn_struct*_tmp1FF=(struct Cyc_Tcdecl_Incompatible_exn_struct*)
_tmp1FE;if(_tmp1FF->tag != Cyc_Tcdecl_Incompatible)goto _LL9B;}_LL9A: return 0;_LL9B:;
_LL9C:(void)_throw(_tmp1FE);_LL98:;}};}struct Cyc_Interface_I*Cyc_Interface_merge_list(
struct Cyc_List_List*li,struct Cyc_List_List*linfo){if(li == 0)return(struct Cyc_Interface_I*)
Cyc_Interface_empty();{struct Cyc_Interface_I*curr_i=(struct Cyc_Interface_I*)li->hd;
struct _dyneither_ptr*curr_info=linfo != 0?(struct _dyneither_ptr*)((struct
_dyneither_ptr*)linfo->hd): 0;li=li->tl;if(linfo != 0)linfo=linfo->tl;for(0;li != 0;
li=li->tl){struct _tuple10*_tmp34A;struct Cyc_Interface_I*_tmp200=Cyc_Interface_merge(
curr_i,(struct Cyc_Interface_I*)li->hd,curr_info != 0  && linfo != 0?(_tmp34A=
_cycalloc(sizeof(*_tmp34A)),((_tmp34A->f1=*curr_info,((_tmp34A->f2=*((struct
_dyneither_ptr*)linfo->hd),_tmp34A))))): 0);if(_tmp200 == 0)return 0;curr_i=(struct
Cyc_Interface_I*)_tmp200;if(curr_info != 0){struct Cyc_String_pa_PrintArg_struct
_tmp354;struct Cyc_String_pa_PrintArg_struct _tmp353;void*_tmp352[2];const char*
_tmp351;struct _dyneither_ptr*_tmp350;curr_info=linfo != 0?(_tmp350=_cycalloc(
sizeof(*_tmp350)),((_tmp350[0]=(struct _dyneither_ptr)((_tmp354.tag=0,((_tmp354.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)linfo->hd)),((
_tmp353.tag=0,((_tmp353.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
curr_info),((_tmp352[0]=& _tmp353,((_tmp352[1]=& _tmp354,Cyc_aprintf(((_tmp351="%s+%s",
_tag_dyneither(_tmp351,sizeof(char),6))),_tag_dyneither(_tmp352,sizeof(void*),2)))))))))))))),
_tmp350))): 0;}if(linfo != 0)linfo=linfo->tl;}return(struct Cyc_Interface_I*)curr_i;};}
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*
get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo){if(la == 0)return(
struct Cyc_Interface_I*)Cyc_Interface_empty();{struct Cyc_Interface_I*curr_i=get((
void*)la->hd);struct _dyneither_ptr*curr_info=linfo != 0?(struct _dyneither_ptr*)((
struct _dyneither_ptr*)linfo->hd): 0;la=la->tl;if(linfo != 0)linfo=linfo->tl;for(0;
la != 0;la=la->tl){struct _tuple10*_tmp355;struct Cyc_Interface_I*_tmp207=Cyc_Interface_merge(
curr_i,get((void*)la->hd),curr_info != 0  && linfo != 0?(_tmp355=_cycalloc(sizeof(*
_tmp355)),((_tmp355->f1=*curr_info,((_tmp355->f2=*((struct _dyneither_ptr*)linfo->hd),
_tmp355))))): 0);if(_tmp207 == 0)return 0;curr_i=(struct Cyc_Interface_I*)_tmp207;
if(curr_info != 0){struct Cyc_String_pa_PrintArg_struct _tmp35F;struct Cyc_String_pa_PrintArg_struct
_tmp35E;void*_tmp35D[2];const char*_tmp35C;struct _dyneither_ptr*_tmp35B;curr_info=
linfo != 0?(_tmp35B=_cycalloc(sizeof(*_tmp35B)),((_tmp35B[0]=(struct
_dyneither_ptr)((_tmp35F.tag=0,((_tmp35F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*((struct _dyneither_ptr*)linfo->hd)),((_tmp35E.tag=0,((_tmp35E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*curr_info),((_tmp35D[0]=& _tmp35E,((
_tmp35D[1]=& _tmp35F,Cyc_aprintf(((_tmp35C="%s+%s",_tag_dyneither(_tmp35C,sizeof(
char),6))),_tag_dyneither(_tmp35D,sizeof(void*),2)))))))))))))),_tmp35B))): 0;}
if(linfo != 0)linfo=linfo->tl;}return(struct Cyc_Interface_I*)curr_i;};}static
struct Cyc_List_List*Cyc_Interface_add_namespace(struct Cyc_List_List*tds,struct
Cyc_List_List*ns){if(ns == 0)return tds;{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*
_tmp365;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp364;struct Cyc_List_List*
_tmp363;return(_tmp363=_cycalloc(sizeof(*_tmp363)),((_tmp363->hd=Cyc_Absyn_new_decl((
void*)((_tmp365=_cycalloc(sizeof(*_tmp365)),((_tmp365[0]=((_tmp364.tag=10,((
_tmp364.f1=(struct _dyneither_ptr*)ns->hd,((_tmp364.f2=Cyc_Interface_add_namespace(
tds,ns->tl),_tmp364)))))),_tmp365)))),0),((_tmp363->tl=0,_tmp363)))));};}static
struct Cyc_List_List*Cyc_Interface_add_aggrdecl(struct _dyneither_ptr*x,struct Cyc_Absyn_Aggrdecl*
d,struct Cyc_List_List*tds){struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp36B;
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp36A;struct Cyc_List_List*_tmp369;
return(_tmp369=_cycalloc(sizeof(*_tmp369)),((_tmp369->hd=Cyc_Absyn_new_decl((
void*)((_tmp36B=_cycalloc(sizeof(*_tmp36B)),((_tmp36B[0]=((_tmp36A.tag=6,((
_tmp36A.f1=d,_tmp36A)))),_tmp36B)))),0),((_tmp369->tl=tds,_tmp369)))));}static
struct Cyc_List_List*Cyc_Interface_add_aggrdecl_header(struct _dyneither_ptr*x,
struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){{struct Cyc_Absyn_Aggrdecl*
_tmp36C;d=((_tmp36C=_cycalloc(sizeof(*_tmp36C)),((_tmp36C[0]=*d,_tmp36C))));}d->impl=
0;if(d->sc != Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*
_tmp372;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp371;struct Cyc_List_List*
_tmp370;return(_tmp370=_cycalloc(sizeof(*_tmp370)),((_tmp370->hd=Cyc_Absyn_new_decl((
void*)((_tmp372=_cycalloc(sizeof(*_tmp372)),((_tmp372[0]=((_tmp371.tag=6,((
_tmp371.f1=d,_tmp371)))),_tmp372)))),0),((_tmp370->tl=tds,_tmp370)))));};}static
struct Cyc_List_List*Cyc_Interface_add_datatypedecl(struct _dyneither_ptr*x,struct
Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*
_tmp378;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp377;struct Cyc_List_List*
_tmp376;return(_tmp376=_cycalloc(sizeof(*_tmp376)),((_tmp376->hd=Cyc_Absyn_new_decl((
void*)((_tmp378=_cycalloc(sizeof(*_tmp378)),((_tmp378[0]=((_tmp377.tag=7,((
_tmp377.f1=d,_tmp377)))),_tmp378)))),0),((_tmp376->tl=tds,_tmp376)))));}static
char _tmp21B[2]="_";static struct _dyneither_ptr Cyc_Interface_us={_tmp21B,_tmp21B,
_tmp21B + 2};static struct _dyneither_ptr*Cyc_Interface_us_p=& Cyc_Interface_us;
static struct _tuple20*Cyc_Interface_rewrite_datatypefield_type(struct _tuple20*x){
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp382;struct Cyc_Absyn_Tvar*_tmp381;
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp380;struct _tuple20*_tmp37F;return(
_tmp37F=_cycalloc(sizeof(*_tmp37F)),((_tmp37F->f1=Cyc_Absyn_empty_tqual(0),((
_tmp37F->f2=(void*)((_tmp382=_cycalloc(sizeof(*_tmp382)),((_tmp382[0]=((_tmp380.tag=
2,((_tmp380.f1=((_tmp381=_cycalloc(sizeof(*_tmp381)),((_tmp381->name=Cyc_Interface_us_p,((
_tmp381->identity=- 1,((_tmp381->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),
_tmp381)))))))),_tmp380)))),_tmp382)))),_tmp37F)))));}static struct Cyc_Absyn_Datatypefield*
Cyc_Interface_rewrite_datatypefield(struct Cyc_Absyn_Datatypefield*f){{struct Cyc_Absyn_Datatypefield*
_tmp383;f=((_tmp383=_cycalloc(sizeof(*_tmp383)),((_tmp383[0]=*f,_tmp383))));}f->typs=((
struct Cyc_List_List*(*)(struct _tuple20*(*f)(struct _tuple20*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Interface_rewrite_datatypefield_type,f->typs);return f;}
static struct Cyc_List_List*Cyc_Interface_add_datatypedecl_header(struct
_dyneither_ptr*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){{struct
Cyc_Absyn_Datatypedecl*_tmp384;d=((_tmp384=_cycalloc(sizeof(*_tmp384)),((_tmp384[
0]=*d,_tmp384))));}if(d->fields != 0){struct Cyc_Core_Opt*_tmp385;d->fields=((
_tmp385=_cycalloc(sizeof(*_tmp385)),((_tmp385->v=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Datatypefield*(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Interface_rewrite_datatypefield,(struct Cyc_List_List*)(d->fields)->v),
_tmp385))));}if(d->sc != Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*
_tmp38B;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp38A;struct Cyc_List_List*
_tmp389;return(_tmp389=_cycalloc(sizeof(*_tmp389)),((_tmp389->hd=Cyc_Absyn_new_decl((
void*)((_tmp38B=_cycalloc(sizeof(*_tmp38B)),((_tmp38B[0]=((_tmp38A.tag=7,((
_tmp38A.f1=d,_tmp38A)))),_tmp38B)))),0),((_tmp389->tl=tds,_tmp389)))));};}static
struct Cyc_List_List*Cyc_Interface_add_enumdecl(struct _dyneither_ptr*x,struct Cyc_Absyn_Enumdecl*
d,struct Cyc_List_List*tds){struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp391;
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp390;struct Cyc_List_List*_tmp38F;
return(_tmp38F=_cycalloc(sizeof(*_tmp38F)),((_tmp38F->hd=Cyc_Absyn_new_decl((
void*)((_tmp391=_cycalloc(sizeof(*_tmp391)),((_tmp391[0]=((_tmp390.tag=8,((
_tmp390.f1=d,_tmp390)))),_tmp391)))),0),((_tmp38F->tl=tds,_tmp38F)))));}static
struct Cyc_List_List*Cyc_Interface_add_enumdecl_header(struct _dyneither_ptr*x,
struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){{struct Cyc_Absyn_Enumdecl*
_tmp392;d=((_tmp392=_cycalloc(sizeof(*_tmp392)),((_tmp392[0]=*d,_tmp392))));}d->fields=
0;if(d->sc != Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*
_tmp398;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp397;struct Cyc_List_List*
_tmp396;return(_tmp396=_cycalloc(sizeof(*_tmp396)),((_tmp396->hd=Cyc_Absyn_new_decl((
void*)((_tmp398=_cycalloc(sizeof(*_tmp398)),((_tmp398[0]=((_tmp397.tag=8,((
_tmp397.f1=d,_tmp397)))),_tmp398)))),0),((_tmp396->tl=tds,_tmp396)))));};}static
struct Cyc_List_List*Cyc_Interface_add_typedef(struct _dyneither_ptr*x,struct Cyc_Absyn_Typedefdecl*
d,struct Cyc_List_List*tds){struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*
_tmp39E;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp39D;struct Cyc_List_List*
_tmp39C;return(_tmp39C=_cycalloc(sizeof(*_tmp39C)),((_tmp39C->hd=Cyc_Absyn_new_decl((
void*)((_tmp39E=_cycalloc(sizeof(*_tmp39E)),((_tmp39E[0]=((_tmp39D.tag=9,((
_tmp39D.f1=d,_tmp39D)))),_tmp39E)))),0),((_tmp39C->tl=tds,_tmp39C)))));}static
struct Cyc_List_List*Cyc_Interface_add_vardecl(struct _dyneither_ptr*x,struct Cyc_Absyn_Vardecl*
d,struct Cyc_List_List*tds){struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp3A4;
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp3A3;struct Cyc_List_List*_tmp3A2;
return(_tmp3A2=_cycalloc(sizeof(*_tmp3A2)),((_tmp3A2->hd=Cyc_Absyn_new_decl((
void*)((_tmp3A4=_cycalloc(sizeof(*_tmp3A4)),((_tmp3A4[0]=((_tmp3A3.tag=0,((
_tmp3A3.f1=d,_tmp3A3)))),_tmp3A4)))),0),((_tmp3A2->tl=tds,_tmp3A2)))));}static
struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl(struct _dyneither_ptr*x,
struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*tds){struct Cyc_Tcdecl_Xdatatypefielddecl
_tmp234;struct Cyc_Absyn_Datatypedecl*_tmp235;struct Cyc_Absyn_Datatypefield*
_tmp236;struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp233=d;_tmp234=*_tmp233;_tmp235=
_tmp234.base;_tmp236=_tmp234.field;{struct Cyc_Absyn_Datatypedecl*_tmp3A5;_tmp235=((
_tmp3A5=_cycalloc(sizeof(*_tmp3A5)),((_tmp3A5[0]=*_tmp235,_tmp3A5))));}{struct
Cyc_List_List*_tmp3A8;struct Cyc_Core_Opt*_tmp3A7;_tmp235->fields=((_tmp3A7=
_cycalloc(sizeof(*_tmp3A7)),((_tmp3A7->v=((_tmp3A8=_cycalloc(sizeof(*_tmp3A8)),((
_tmp3A8->hd=_tmp236,((_tmp3A8->tl=0,_tmp3A8)))))),_tmp3A7))));}_tmp235->sc=Cyc_Absyn_Extern;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp3AE;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct
_tmp3AD;struct Cyc_List_List*_tmp3AC;return(_tmp3AC=_cycalloc(sizeof(*_tmp3AC)),((
_tmp3AC->hd=Cyc_Absyn_new_decl((void*)((_tmp3AE=_cycalloc(sizeof(*_tmp3AE)),((
_tmp3AE[0]=((_tmp3AD.tag=7,((_tmp3AD.f1=_tmp235,_tmp3AD)))),_tmp3AE)))),0),((
_tmp3AC->tl=tds,_tmp3AC)))));};}static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl_header(
struct _dyneither_ptr*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*
tds){struct Cyc_Tcdecl_Xdatatypefielddecl _tmp23E;struct Cyc_Absyn_Datatypedecl*
_tmp23F;struct Cyc_Absyn_Datatypefield*_tmp240;struct Cyc_Tcdecl_Xdatatypefielddecl*
_tmp23D=d;_tmp23E=*_tmp23D;_tmp23F=_tmp23E.base;_tmp240=_tmp23E.field;{struct Cyc_Absyn_Datatypedecl*
_tmp3AF;_tmp23F=((_tmp3AF=_cycalloc(sizeof(*_tmp3AF)),((_tmp3AF[0]=*_tmp23F,
_tmp3AF))));}_tmp240=Cyc_Interface_rewrite_datatypefield(_tmp240);_tmp240->sc=
Cyc_Absyn_Extern;{struct Cyc_List_List*_tmp3B2;struct Cyc_Core_Opt*_tmp3B1;_tmp23F->fields=((
_tmp3B1=_cycalloc(sizeof(*_tmp3B1)),((_tmp3B1->v=((_tmp3B2=_cycalloc(sizeof(*
_tmp3B2)),((_tmp3B2->hd=_tmp240,((_tmp3B2->tl=0,_tmp3B2)))))),_tmp3B1))));}
_tmp23F->sc=Cyc_Absyn_Extern;{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*
_tmp3B8;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp3B7;struct Cyc_List_List*
_tmp3B6;return(_tmp3B6=_cycalloc(sizeof(*_tmp3B6)),((_tmp3B6->hd=Cyc_Absyn_new_decl((
void*)((_tmp3B8=_cycalloc(sizeof(*_tmp3B8)),((_tmp3B8[0]=((_tmp3B7.tag=7,((
_tmp3B7.f1=_tmp23F,_tmp3B7)))),_tmp3B8)))),0),((_tmp3B6->tl=tds,_tmp3B6)))));};}
static void Cyc_Interface_print_ns_headers(struct Cyc___cycFILE*f,struct Cyc_List_List*
ns,struct Cyc_Interface_Ienv*ie){struct Cyc_List_List*_tmp247=0;_tmp247=((struct
Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrdecl*,
struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(
Cyc_Interface_add_aggrdecl_header,ie->aggrdecls,_tmp247);_tmp247=((struct Cyc_List_List*(*)(
struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Datatypedecl*,
struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(
Cyc_Interface_add_datatypedecl_header,ie->datatypedecls,_tmp247);_tmp247=((
struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Enumdecl*,
struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(
Cyc_Interface_add_enumdecl_header,ie->enumdecls,_tmp247);if(_tmp247 != 0){_tmp247=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp247);
_tmp247=Cyc_Interface_add_namespace(_tmp247,ns);Cyc_Absynpp_decllist2file(
_tmp247,f);}}static void Cyc_Interface_print_ns_xdatatypefielddecl_headers(struct
Cyc___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct Cyc_List_List*
_tmp248=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,
struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,
struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xdatatypefielddecl_header,
ie->xdatatypefielddecls,0);if(_tmp248 != 0){_tmp248=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp248);_tmp248=Cyc_Interface_add_namespace(
_tmp248,ns);Cyc_Absynpp_decllist2file(_tmp248,f);}}static void Cyc_Interface_print_ns_typedefs(
struct Cyc___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct
Cyc_List_List*_tmp249=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_dyneither_ptr*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_typedef,ie->typedefdecls,
0);if(_tmp249 != 0){_tmp249=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp249);_tmp249=Cyc_Interface_add_namespace(_tmp249,ns);Cyc_Absynpp_decllist2file(
_tmp249,f);}}static void Cyc_Interface_print_ns_decls(struct Cyc___cycFILE*f,struct
Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct Cyc_List_List*_tmp24A=0;
_tmp24A=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,
struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*
accum))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl,ie->aggrdecls,_tmp24A);_tmp24A=((
struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Datatypedecl*,
struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(
Cyc_Interface_add_datatypedecl,ie->datatypedecls,_tmp24A);_tmp24A=((struct Cyc_List_List*(*)(
struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Enumdecl*,struct
Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(
Cyc_Interface_add_enumdecl,ie->enumdecls,_tmp24A);_tmp24A=((struct Cyc_List_List*(*)(
struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Vardecl*,struct
Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(
Cyc_Interface_add_vardecl,ie->vardecls,_tmp24A);_tmp24A=((struct Cyc_List_List*(*)(
struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Tcdecl_Xdatatypefielddecl*,
struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(
Cyc_Interface_add_xdatatypefielddecl,ie->xdatatypefielddecls,_tmp24A);if(_tmp24A
!= 0){_tmp24A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp24A);_tmp24A=Cyc_Interface_add_namespace(_tmp24A,ns);Cyc_Absynpp_decllist2file(
_tmp24A,f);}}void Cyc_Interface_print(struct Cyc_Interface_I*i,struct Cyc___cycFILE*
f){Cyc_Absynpp_set_params(& Cyc_Absynpp_cyci_params_r);{const char*_tmp3BB;void*
_tmp3BA;(_tmp3BA=0,Cyc_fprintf(f,((_tmp3BB="/****** needed (headers) ******/\n",
_tag_dyneither(_tmp3BB,sizeof(char),34))),_tag_dyneither(_tmp3BA,sizeof(void*),0)));}((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_headers,
f,i->imports);{const char*_tmp3BE;void*_tmp3BD;(_tmp3BD=0,Cyc_fprintf(f,((_tmp3BE="\n/****** provided (headers) ******/\n",
_tag_dyneither(_tmp3BE,sizeof(char),37))),_tag_dyneither(_tmp3BD,sizeof(void*),0)));}((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_headers,
f,i->exports);{const char*_tmp3C1;void*_tmp3C0;(_tmp3C0=0,Cyc_fprintf(f,((_tmp3C1="\n/****** needed (headers of xdatatypefielddecls) ******/\n",
_tag_dyneither(_tmp3C1,sizeof(char),58))),_tag_dyneither(_tmp3C0,sizeof(void*),0)));}((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_xdatatypefielddecl_headers,
f,i->imports);{const char*_tmp3C4;void*_tmp3C3;(_tmp3C3=0,Cyc_fprintf(f,((_tmp3C4="\n/****** provided (headers of xdatatypefielddecls) ******/\n",
_tag_dyneither(_tmp3C4,sizeof(char),60))),_tag_dyneither(_tmp3C3,sizeof(void*),0)));}((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_xdatatypefielddecl_headers,
f,i->exports);{const char*_tmp3C7;void*_tmp3C6;(_tmp3C6=0,Cyc_fprintf(f,((_tmp3C7="\n/****** provided (typedefs) ******/\n",
_tag_dyneither(_tmp3C7,sizeof(char),38))),_tag_dyneither(_tmp3C6,sizeof(void*),0)));}((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_typedefs,
f,i->exports);{const char*_tmp3CA;void*_tmp3C9;(_tmp3C9=0,Cyc_fprintf(f,((_tmp3CA="\n/****** needed (declarations) ******/\n",
_tag_dyneither(_tmp3CA,sizeof(char),40))),_tag_dyneither(_tmp3C9,sizeof(void*),0)));}((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_decls,
f,i->imports);{const char*_tmp3CD;void*_tmp3CC;(_tmp3CC=0,Cyc_fprintf(f,((_tmp3CD="\n/****** provided (declarations) ******/\n",
_tag_dyneither(_tmp3CD,sizeof(char),42))),_tag_dyneither(_tmp3CC,sizeof(void*),0)));}((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_decls,
f,i->exports);}struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*f){
Cyc_Lex_lex_init();{struct Cyc_List_List*_tmp259=Cyc_Parse_parse_file(f);struct
_RegionHandle _tmp25A=_new_region("r");struct _RegionHandle*r=& _tmp25A;
_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp25B=Cyc_Tcenv_tc_init(r);Cyc_Tc_tc(r,
_tmp25B,0,_tmp259);{struct Cyc_Interface_I*_tmp25C=Cyc_Interface_gen_extract(
_tmp25B->ae,0);_npop_handler(0);return _tmp25C;};};_pop_region(r);};}void Cyc_Interface_save(
struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){Cyc_Interface_print(i,f);}struct
Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*f){return Cyc_Interface_parse(
f);}
