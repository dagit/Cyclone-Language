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
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};
struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};
struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{
int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};
struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};typedef void*
Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{
char*tag;struct _dyneither_ptr f1;};typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{
void*v;};typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _dyneither_ptr Cyc_Core_new_string(
unsigned int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_unique_region;
struct Cyc_Core_NewRegion Cyc_Core__new_dynregion(const char*file,int lineno);extern
char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*
tag;};void Cyc_Core_free_dynregion(struct _DynRegionHandle*);inline static void* 
arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){if(bd >> 20  || sz
>> 12)return 0;{unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < dyn.curr  || dyn.curr
== 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)return 0;return dyn.curr;};}
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};typedef struct Cyc_List_List*
Cyc_List_list_t;typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{
char*tag;};extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*
tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct
Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;
struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,
void*);void*read_fun_state;};typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;
struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr
lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct
_dyneither_ptr lex_check;};typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
struct _dyneither_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);char Cyc_Lexing_lexeme_char(
struct Cyc_Lexing_lexbuf*,int);int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;int
Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;typedef struct Cyc_Set_Set*
Cyc_Set_set_t;struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));struct Cyc_Set_Set*
Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*
s,void*elt);void Cyc_Set_iter(void(*f)(void*),struct Cyc_Set_Set*s);extern char Cyc_Set_Absent[
7];struct Cyc_Set_Absent_exn_struct{char*tag;};unsigned long Cyc_strlen(struct
_dyneither_ptr s);int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
struct _dyneither_ptr Cyc_zstrncpy(struct _dyneither_ptr,struct _dyneither_ptr,
unsigned long);struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,
unsigned long n);struct Cyc_Xarray_Xarray{struct _RegionHandle*r;struct
_dyneither_ptr elmts;int num_elmts;};typedef struct Cyc_Xarray_Xarray*Cyc_Xarray_xarray_t;
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate(
struct _RegionHandle*,int,void*);void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];
struct Cyc_Position_Exit_exn_struct{char*tag;};typedef unsigned int Cyc_Position_seg_t;
struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{
int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};typedef void*
Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;
unsigned int seg;void*kind;struct _dyneither_ptr desc;};typedef struct Cyc_Position_Error*
Cyc_Position_error_t;struct Cyc_Position_Error*Cyc_Position_mk_err_lex(
unsigned int,struct _dyneither_ptr);struct Cyc_Position_Error*Cyc_Position_mk_err_parse(
unsigned int,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct
Cyc_Position_Nocontext_exn_struct{char*tag;};void Cyc_Position_post_error(struct
Cyc_Position_Error*);typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;typedef
struct _dyneither_ptr*Cyc_Absyn_var_t;typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int
tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
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
typedef void*Cyc_Absyn_type_opt_t;typedef union Cyc_Absyn_Cnst Cyc_Absyn_cnst_t;
typedef enum Cyc_Absyn_Primop Cyc_Absyn_primop_t;typedef enum Cyc_Absyn_Incrementor
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
char*tag;};int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
struct _tuple10{struct _tuple10*tl;};struct Cyc_FlatList{struct _tuple10*tl;};
typedef struct _tuple10*Cyc_flat_list_t;struct Cyc_Type_specifier{int Signed_spec: 1;
int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int
Valid_type_spec: 1;void*Type_spec;unsigned int loc;};typedef struct Cyc_Type_specifier
Cyc_type_specifier_t;struct Cyc_Declarator{struct _tuple0*id;struct Cyc_List_List*
tms;};typedef struct Cyc_Declarator Cyc_declarator_t;struct _tuple11{struct Cyc_Declarator
f1;struct Cyc_Absyn_Exp*f2;};struct _tuple12{struct _tuple12*tl;struct _tuple11 hd;};
typedef struct _tuple12*Cyc_declarator_list_t;enum Cyc_Storage_class{Cyc_Typedef_sc
 = 0,Cyc_Extern_sc  = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc
 = 5,Cyc_Abstract_sc  = 6};typedef enum Cyc_Storage_class Cyc_storage_class_t;
struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;
struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};
typedef struct Cyc_Declaration_spec Cyc_decl_spec_t;struct Cyc_Abstractdeclarator{
struct Cyc_List_List*tms;};typedef struct Cyc_Abstractdeclarator Cyc_abstractdeclarator_t;
struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct
_union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;
struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*
val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple13{
int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct
_tuple13 val;};struct _tuple14{unsigned int f1;union Cyc_Absyn_Constraint*f2;union
Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple14*val;};
struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*
val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple15{struct Cyc_List_List*
f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple15*val;};struct
_union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple16{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple16*val;
};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY16{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY17{int tag;
struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Declaration_spec val;};struct
_union_YYSTYPE_YY20{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY21{int tag;
struct _tuple12*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Storage_class*val;
};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Type_specifier val;};struct
_union_YYSTYPE_YY24{int tag;enum Cyc_Absyn_AggrKind val;};struct
_union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY26{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY29{int tag;struct Cyc_Declarator val;};struct _union_YYSTYPE_YY30{
int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;
};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct
_union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct
_union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_Absyn_Tqual
f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{
int tag;struct _tuple17 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct
_union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct
_union_YYSTYPE_YY39{int tag;struct _tuple18*val;};struct _union_YYSTYPE_YY40{int tag;
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
Cyc_yyltype;extern struct Cyc_Yyltype Cyc_yylloc;struct Cyc_Dict_T;typedef const
struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;const struct Cyc_Dict_T*t;};typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;
extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct
Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict Cyc_Dict_insert(
struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,
void*k);struct Cyc_RgnOrder_RgnPO;typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
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
const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*
f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*
f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{
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
Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;void Cyc_Tcutil_warn(unsigned int,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_check_no_qual(
unsigned int loc,void*t);void Cyc_yyerror(struct _dyneither_ptr s){Cyc_Position_post_error(
Cyc_Position_mk_err_parse((unsigned int)Cyc_yylloc.first_line,s));}struct Cyc_Lex_Trie;
typedef struct Cyc_Lex_Trie*Cyc_Lex_trie_t;struct _union_TrieChildren_Zero{int tag;
int val;};struct _tuple19{int f1;struct Cyc_Lex_Trie*f2;};struct
_union_TrieChildren_One{int tag;struct _tuple19 val;};struct
_union_TrieChildren_Many{int tag;struct Cyc_Lex_Trie**val;};union Cyc_Lex_TrieChildren{
struct _union_TrieChildren_Zero Zero;struct _union_TrieChildren_One One;struct
_union_TrieChildren_Many Many;};union Cyc_Lex_TrieChildren Cyc_Lex_Zero(){union Cyc_Lex_TrieChildren
_tmp130;return((_tmp130.Zero).val=0,(((_tmp130.Zero).tag=1,_tmp130)));}union Cyc_Lex_TrieChildren
Cyc_Lex_One(int i,struct Cyc_Lex_Trie*t){struct _tuple19 _tmp133;union Cyc_Lex_TrieChildren
_tmp132;return((_tmp132.One).val=((_tmp133.f1=i,((_tmp133.f2=t,_tmp133)))),(((
_tmp132.One).tag=2,_tmp132)));}union Cyc_Lex_TrieChildren Cyc_Lex_Many(struct Cyc_Lex_Trie**
ts){union Cyc_Lex_TrieChildren _tmp134;return((_tmp134.Many).val=ts,(((_tmp134.Many).tag=
3,_tmp134)));}struct Cyc_Lex_Trie{union Cyc_Lex_TrieChildren children;int shared_str;
};static int Cyc_Lex_num_kws=0;static struct _dyneither_ptr Cyc_Lex_kw_nums={(void*)0,(
void*)0,(void*)(0 + 0)};struct Cyc_Lex_DynTrie{struct _DynRegionHandle*dyn;struct
Cyc_Lex_Trie*t;};typedef struct Cyc_Lex_DynTrie*Cyc_Lex_dyn_trie_t;struct Cyc_Lex_DynTrieSymbols{
struct _DynRegionHandle*dyn;struct Cyc_Lex_Trie*t;struct Cyc_Xarray_Xarray*symbols;
};typedef struct Cyc_Lex_DynTrieSymbols*Cyc_Lex_dyn_trie_symbols_t;typedef struct
Cyc_Lex_DynSymbols*Cyc_Lex_dyn_symbols_t;static struct Cyc_Lex_DynTrieSymbols*Cyc_Lex_ids_trie=
0;static struct Cyc_Lex_DynTrie*Cyc_Lex_typedefs_trie=0;static int Cyc_Lex_comment_depth=
0;static struct _tuple5 Cyc_Lex_token_int={Cyc_Absyn_Signed,0};static char _tmp4[8]="*bogus*";
static struct _dyneither_ptr Cyc_Lex_bogus_string={_tmp4,_tmp4,_tmp4 + 8};static
struct _tuple0 Cyc_Lex_token_id_pair={{.Loc_n={4,0}},& Cyc_Lex_bogus_string};static
char Cyc_Lex_token_char='\000';static char _tmp5[1]="";static struct _dyneither_ptr
Cyc_Lex_token_string={_tmp5,_tmp5,_tmp5 + 1};static struct _tuple0*Cyc_Lex_token_qvar=&
Cyc_Lex_token_id_pair;static char _tmp6[1]="";static struct _tuple13 Cyc_Lex_token_asm={
0,{_tmp6,_tmp6,_tmp6 + 1}};static int Cyc_Lex_runaway_start=0;static int Cyc_Lex_paren_depth=
0;static void Cyc_Lex_err(struct _dyneither_ptr msg,struct Cyc_Lexing_lexbuf*lb){
unsigned int s=(unsigned int)Cyc_Lexing_lexeme_start(lb);Cyc_Position_post_error(
Cyc_Position_mk_err_lex(s,msg));}static void Cyc_Lex_runaway_err(struct
_dyneither_ptr msg,struct Cyc_Lexing_lexbuf*lb){unsigned int s=(unsigned int)Cyc_Lex_runaway_start;
Cyc_Position_post_error(Cyc_Position_mk_err_lex(s,msg));}struct _tuple20{struct
_dyneither_ptr f1;short f2;int f3;};static char _tmp7[14]="__attribute__";static char
_tmp8[9]="abstract";static char _tmp9[6]="alias";static char _tmpA[5]="auto";static
char _tmpB[6]="break";static char _tmpC[7]="calloc";static char _tmpD[5]="case";
static char _tmpE[6]="catch";static char _tmpF[5]="char";static char _tmp10[6]="const";
static char _tmp11[9]="continue";static char _tmp12[9]="datatype";static char _tmp13[8]="default";
static char _tmp14[3]="do";static char _tmp15[7]="double";static char _tmp16[12]="dynregion_t";
static char _tmp17[5]="else";static char _tmp18[5]="enum";static char _tmp19[7]="export";
static char _tmp1A[7]="extern";static char _tmp1B[9]="fallthru";static char _tmp1C[6]="float";
static char _tmp1D[4]="for";static char _tmp1E[5]="goto";static char _tmp1F[3]="if";
static char _tmp20[7]="inline";static char _tmp21[11]="__inline__";static char _tmp22[
4]="int";static char _tmp23[4]="let";static char _tmp24[5]="long";static char _tmp25[7]="malloc";
static char _tmp26[10]="namespace";static char _tmp27[4]="new";static char _tmp28[11]="NOZEROTERM";
static char _tmp29[5]="NULL";static char _tmp2A[8]="numelts";static char _tmp2B[9]="offsetof";
static char _tmp2C[20]="__cyclone_port_on__";static char _tmp2D[21]="__cyclone_port_off__";
static char _tmp2E[8]="rcalloc";static char _tmp2F[9]="region_t";static char _tmp30[7]="region";
static char _tmp31[8]="regions";static char _tmp32[9]="register";static char _tmp33[13]="reset_region";
static char _tmp34[9]="restrict";static char _tmp35[7]="return";static char _tmp36[8]="rmalloc";
static char _tmp37[5]="rnew";static char _tmp38[6]="short";static char _tmp39[7]="signed";
static char _tmp3A[7]="sizeof";static char _tmp3B[7]="static";static char _tmp3C[7]="struct";
static char _tmp3D[7]="switch";static char _tmp3E[9]="tagcheck";static char _tmp3F[6]="tag_t";
static char _tmp40[6]="throw";static char _tmp41[4]="try";static char _tmp42[7]="tunion";
static char _tmp43[8]="typedef";static char _tmp44[6]="union";static char _tmp45[9]="unsigned";
static char _tmp46[6]="using";static char _tmp47[8]="valueof";static char _tmp48[10]="valueof_t";
static char _tmp49[5]="void";static char _tmp4A[9]="volatile";static char _tmp4B[6]="while";
static char _tmp4C[8]="xtunion";static char _tmp4D[9]="ZEROTERM";static struct
_tuple20 Cyc_Lex_rw_array[71]={{{_tmp7,_tmp7,_tmp7 + 14},372,1},{{_tmp8,_tmp8,
_tmp8 + 9},300,0},{{_tmp9,_tmp9,_tmp9 + 6},324,0},{{_tmpA,_tmpA,_tmpA + 5},258,1},{{
_tmpB,_tmpB,_tmpB + 6},290,1},{{_tmpC,_tmpC,_tmpC + 7},309,0},{{_tmpD,_tmpD,_tmpD + 
5},277,1},{{_tmpE,_tmpE,_tmpE + 6},297,1},{{_tmpF,_tmpF,_tmpF + 5},264,1},{{_tmp10,
_tmp10,_tmp10 + 6},272,1},{{_tmp11,_tmp11,_tmp11 + 9},289,1},{{_tmp12,_tmp12,
_tmp12 + 9},304,0},{{_tmp13,_tmp13,_tmp13 + 8},278,1},{{_tmp14,_tmp14,_tmp14 + 3},
286,1},{{_tmp15,_tmp15,_tmp15 + 7},269,1},{{_tmp16,_tmp16,_tmp16 + 12},323,0},{{
_tmp17,_tmp17,_tmp17 + 5},283,1},{{_tmp18,_tmp18,_tmp18 + 5},292,1},{{_tmp19,
_tmp19,_tmp19 + 7},298,0},{{_tmp1A,_tmp1A,_tmp1A + 7},261,1},{{_tmp1B,_tmp1B,
_tmp1B + 9},301,0},{{_tmp1C,_tmp1C,_tmp1C + 6},268,1},{{_tmp1D,_tmp1D,_tmp1D + 4},
287,1},{{_tmp1E,_tmp1E,_tmp1E + 5},288,1},{{_tmp1F,_tmp1F,_tmp1F + 3},282,1},{{
_tmp20,_tmp20,_tmp20 + 7},279,1},{{_tmp21,_tmp21,_tmp21 + 11},279,1},{{_tmp22,
_tmp22,_tmp22 + 4},266,1},{{_tmp23,_tmp23,_tmp23 + 4},294,0},{{_tmp24,_tmp24,
_tmp24 + 5},267,1},{{_tmp25,_tmp25,_tmp25 + 7},307,0},{{_tmp26,_tmp26,_tmp26 + 10},
303,0},{{_tmp27,_tmp27,_tmp27 + 4},299,0},{{_tmp28,_tmp28,_tmp28 + 11},318,0},{{
_tmp29,_tmp29,_tmp29 + 5},293,0},{{_tmp2A,_tmp2A,_tmp2A + 8},325,0},{{_tmp2B,
_tmp2B,_tmp2B + 9},281,1},{{_tmp2C,_tmp2C,_tmp2C + 20},321,0},{{_tmp2D,_tmp2D,
_tmp2D + 21},322,0},{{_tmp2E,_tmp2E,_tmp2E + 8},310,0},{{_tmp2F,_tmp2F,_tmp2F + 9},
312,0},{{_tmp30,_tmp30,_tmp30 + 7},314,0},{{_tmp31,_tmp31,_tmp31 + 8},316,0},{{
_tmp32,_tmp32,_tmp32 + 9},259,1},{{_tmp33,_tmp33,_tmp33 + 13},317,0},{{_tmp34,
_tmp34,_tmp34 + 9},274,1},{{_tmp35,_tmp35,_tmp35 + 7},291,1},{{_tmp36,_tmp36,
_tmp36 + 8},308,0},{{_tmp37,_tmp37,_tmp37 + 5},315,0},{{_tmp38,_tmp38,_tmp38 + 6},
265,1},{{_tmp39,_tmp39,_tmp39 + 7},270,1},{{_tmp3A,_tmp3A,_tmp3A + 7},280,1},{{
_tmp3B,_tmp3B,_tmp3B + 7},260,1},{{_tmp3C,_tmp3C,_tmp3C + 7},275,1},{{_tmp3D,
_tmp3D,_tmp3D + 7},284,1},{{_tmp3E,_tmp3E,_tmp3E + 9},328,0},{{_tmp3F,_tmp3F,
_tmp3F + 6},313,0},{{_tmp40,_tmp40,_tmp40 + 6},295,0},{{_tmp41,_tmp41,_tmp41 + 4},
296,0},{{_tmp42,_tmp42,_tmp42 + 7},306,0},{{_tmp43,_tmp43,_tmp43 + 8},262,1},{{
_tmp44,_tmp44,_tmp44 + 6},276,1},{{_tmp45,_tmp45,_tmp45 + 9},271,1},{{_tmp46,
_tmp46,_tmp46 + 6},302,0},{{_tmp47,_tmp47,_tmp47 + 8},326,0},{{_tmp48,_tmp48,
_tmp48 + 10},327,0},{{_tmp49,_tmp49,_tmp49 + 5},263,1},{{_tmp4A,_tmp4A,_tmp4A + 9},
273,1},{{_tmp4B,_tmp4B,_tmp4B + 6},285,1},{{_tmp4C,_tmp4C,_tmp4C + 8},305,0},{{
_tmp4D,_tmp4D,_tmp4D + 9},319,0}};static int Cyc_Lex_num_keywords(int
include_cyclone_keywords){int sum=0;{unsigned int i=0;for(0;i < 71;++ i){if(
include_cyclone_keywords  || (Cyc_Lex_rw_array[(int)i]).f3)++ sum;}}return sum;}
static int Cyc_Lex_trie_char(int c){if(c >= 95)return c - 59;else{if(c > 64)return c - 55;}
return c - 48;}static struct Cyc_Lex_Trie*Cyc_Lex_trie_lookup(struct _RegionHandle*r,
struct Cyc_Lex_Trie*t,struct _dyneither_ptr buff,int offset,int len);static void
_tmp13A(unsigned int*_tmp139,unsigned int*_tmp138,struct Cyc_Lex_Trie***_tmp137){
for(*_tmp139=0;*_tmp139 < *_tmp138;(*_tmp139)++){(*_tmp137)[*_tmp139]=0;}}static
struct Cyc_Lex_Trie*Cyc_Lex_trie_lookup(struct _RegionHandle*r,struct Cyc_Lex_Trie*
t,struct _dyneither_ptr buff,int offset,int len){int i=offset;int last=(offset + len)- 
1;while(i <= last){union Cyc_Lex_TrieChildren _tmp4E=((struct Cyc_Lex_Trie*)
_check_null(t))->children;struct Cyc_Lex_Trie**_tmp4F;struct _tuple19 _tmp50;int
_tmp51;struct Cyc_Lex_Trie*_tmp52;int _tmp53;_LL1: if((_tmp4E.Many).tag != 3)goto
_LL3;_tmp4F=(struct Cyc_Lex_Trie**)(_tmp4E.Many).val;_LL2: {int ch=Cyc_Lex_trie_char((
int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),i)));if(_tmp4F[
_check_known_subscript_notnull(64,ch)]== 0){struct Cyc_Lex_Trie*_tmp135;_tmp4F[
_check_known_subscript_notnull(64,ch)]=((_tmp135=_region_malloc(r,sizeof(*
_tmp135)),((_tmp135->children=Cyc_Lex_Zero(),((_tmp135->shared_str=0,_tmp135))))));}
t=_tmp4F[_check_known_subscript_notnull(64,ch)];++ i;goto _LL0;}_LL3: if((_tmp4E.One).tag
!= 2)goto _LL5;_tmp50=(struct _tuple19)(_tmp4E.One).val;_tmp51=_tmp50.f1;_tmp52=
_tmp50.f2;_LL4: if(_tmp51 == *((const char*)_check_dyneither_subscript(buff,sizeof(
char),i)))t=_tmp52;else{unsigned int _tmp139;unsigned int _tmp138;struct Cyc_Lex_Trie**
_tmp137;unsigned int _tmp136;struct Cyc_Lex_Trie**_tmp55=(_tmp136=64,((_tmp137=(
struct Cyc_Lex_Trie**)_region_malloc(r,_check_times(sizeof(struct Cyc_Lex_Trie*),
_tmp136)),((((_tmp138=_tmp136,_tmp13A(& _tmp139,& _tmp138,& _tmp137))),_tmp137)))));
_tmp55[_check_known_subscript_notnull(64,Cyc_Lex_trie_char(_tmp51))]=_tmp52;{int
_tmp56=Cyc_Lex_trie_char((int)*((const char*)_check_dyneither_subscript(buff,
sizeof(char),i)));{struct Cyc_Lex_Trie*_tmp13B;_tmp55[
_check_known_subscript_notnull(64,_tmp56)]=((_tmp13B=_region_malloc(r,sizeof(*
_tmp13B)),((_tmp13B->children=Cyc_Lex_Zero(),((_tmp13B->shared_str=0,_tmp13B))))));}
t->children=Cyc_Lex_Many(_tmp55);t=_tmp55[_check_known_subscript_notnull(64,
_tmp56)];};}++ i;goto _LL0;_LL5: if((_tmp4E.Zero).tag != 1)goto _LL0;_tmp53=(int)(
_tmp4E.Zero).val;_LL6: while(i <= last){struct Cyc_Lex_Trie*_tmp13C;struct Cyc_Lex_Trie*
_tmp5B=(_tmp13C=_region_malloc(r,sizeof(*_tmp13C)),((_tmp13C->children=Cyc_Lex_Zero(),((
_tmp13C->shared_str=0,_tmp13C)))));((struct Cyc_Lex_Trie*)_check_null(t))->children=
Cyc_Lex_One((int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),i ++)),
_tmp5B);t=_tmp5B;}return t;_LL0:;}return t;}static int Cyc_Lex_str_index(struct
_dyneither_ptr buff,int offset,int len){struct _DynRegionHandle*_tmp5E;struct Cyc_Lex_Trie*
_tmp5F;struct Cyc_Xarray_Xarray*_tmp60;struct Cyc_Lex_DynTrieSymbols _tmp5D=*((
struct Cyc_Lex_DynTrieSymbols*)_check_null(Cyc_Lex_ids_trie));_tmp5E=_tmp5D.dyn;
_tmp5F=_tmp5D.t;_tmp60=_tmp5D.symbols;{struct _DynRegionFrame _tmp61;struct
_RegionHandle*d=_open_dynregion(& _tmp61,_tmp5E);{struct Cyc_Lex_Trie*_tmp62=Cyc_Lex_trie_lookup(
d,_tmp5F,buff,offset,len);if(_tmp62->shared_str == 0){struct _dyneither_ptr _tmp63=
Cyc_Core_new_string((unsigned int)(len + 1));Cyc_zstrncpy(
_dyneither_ptr_decrease_size(_tmp63,sizeof(char),1),(struct _dyneither_ptr)
_dyneither_ptr_plus(buff,sizeof(char),offset),(unsigned long)len);{struct
_dyneither_ptr*_tmp13D;int ans=((int(*)(struct Cyc_Xarray_Xarray*,struct
_dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp60,((_tmp13D=_cycalloc(sizeof(*_tmp13D)),((
_tmp13D[0]=(struct _dyneither_ptr)_tmp63,_tmp13D)))));_tmp62->shared_str=ans;};}{
int _tmp65=_tmp62->shared_str;_npop_handler(0);return _tmp65;};};_pop_dynregion(d);};}
static int Cyc_Lex_str_index_lbuf(struct Cyc_Lexing_lexbuf*lbuf){return Cyc_Lex_str_index((
struct _dyneither_ptr)lbuf->lex_buffer,lbuf->lex_start_pos,lbuf->lex_curr_pos - 
lbuf->lex_start_pos);}static void Cyc_Lex_insert_typedef(struct _dyneither_ptr*sptr){
struct _dyneither_ptr _tmp66=*sptr;struct _DynRegionHandle*_tmp68;struct Cyc_Lex_Trie*
_tmp69;struct Cyc_Lex_DynTrie _tmp67=*((struct Cyc_Lex_DynTrie*)_check_null(Cyc_Lex_typedefs_trie));
_tmp68=_tmp67.dyn;_tmp69=_tmp67.t;{struct _DynRegionFrame _tmp6A;struct
_RegionHandle*d=_open_dynregion(& _tmp6A,_tmp68);{struct Cyc_Lex_Trie*_tmp6B=Cyc_Lex_trie_lookup(
d,_tmp69,_tmp66,0,(int)(_get_dyneither_size(_tmp66,sizeof(char))- 1));((struct
Cyc_Lex_Trie*)_check_null(_tmp6B))->shared_str=1;}_npop_handler(0);return;;
_pop_dynregion(d);};}static struct _dyneither_ptr*Cyc_Lex_get_symbol(int symbol_num){
struct _DynRegionHandle*_tmp6D;struct Cyc_Lex_Trie*_tmp6E;struct Cyc_Xarray_Xarray*
_tmp6F;struct Cyc_Lex_DynTrieSymbols _tmp6C=*((struct Cyc_Lex_DynTrieSymbols*)
_check_null(Cyc_Lex_ids_trie));_tmp6D=_tmp6C.dyn;_tmp6E=_tmp6C.t;_tmp6F=_tmp6C.symbols;{
struct _DynRegionFrame _tmp70;struct _RegionHandle*d=_open_dynregion(& _tmp70,_tmp6D);{
struct _dyneither_ptr*_tmp71=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp6F,symbol_num);_npop_handler(0);return _tmp71;};
_pop_dynregion(d);};}static int Cyc_Lex_int_of_char(char c){if('0' <= c  && c <= '9')
return c - '0';else{if('a' <= c  && c <= 'f')return(10 + c)- 'a';else{if('A' <= c  && c
<= 'F')return(10 + c)- 'A';else{struct Cyc_Core_Invalid_argument_exn_struct _tmp143;
const char*_tmp142;struct Cyc_Core_Invalid_argument_exn_struct*_tmp141;(int)_throw((
void*)((_tmp141=_cycalloc(sizeof(*_tmp141)),((_tmp141[0]=((_tmp143.tag=Cyc_Core_Invalid_argument,((
_tmp143.f1=((_tmp142="string to integer conversion",_tag_dyneither(_tmp142,
sizeof(char),29))),_tmp143)))),_tmp141)))));}}}}static struct _tuple5 Cyc_Lex_intconst(
struct Cyc_Lexing_lexbuf*lbuf,int start,int end,int base){unsigned int n=0;int end2=
lbuf->lex_curr_pos - end;struct _dyneither_ptr buff=lbuf->lex_buffer;int i=start + 
lbuf->lex_start_pos;{int i=start + lbuf->lex_start_pos;for(0;i < end2;++ i){char c=*((
char*)_check_dyneither_subscript(buff,sizeof(char),i));switch(c){case 'u': _LL7:
goto _LL8;case 'U': _LL8: {struct _tuple5 _tmp144;return(_tmp144.f1=Cyc_Absyn_Unsigned,((
_tmp144.f2=(int)n,_tmp144)));}case 'l': _LL9: break;case 'L': _LLA: break;default: _LLB:
n=n * base + (unsigned int)Cyc_Lex_int_of_char(c);break;}}}{struct _tuple5 _tmp145;
return(_tmp145.f1=Cyc_Absyn_Signed,((_tmp145.f2=(int)n,_tmp145)));};}char Cyc_Lex_string_buffer_v[
11]={'x','x','x','x','x','x','x','x','x','x','\000'};struct _dyneither_ptr Cyc_Lex_string_buffer={(
void*)Cyc_Lex_string_buffer_v,(void*)Cyc_Lex_string_buffer_v,(void*)(Cyc_Lex_string_buffer_v
+ 11)};int Cyc_Lex_string_pos=0;void Cyc_Lex_store_string_char(char c);static void
_tmp14B(int*sz,unsigned int*_tmp14A,unsigned int*_tmp149,char**_tmp147){for(*
_tmp14A=0;*_tmp14A < *_tmp149;(*_tmp14A)++){(*_tmp147)[*_tmp14A]=*_tmp14A < *sz?*((
char*)_check_dyneither_subscript(Cyc_Lex_string_buffer,sizeof(char),(int)*
_tmp14A)):'\000';}}void Cyc_Lex_store_string_char(char c){int sz=(int)(
_get_dyneither_size(Cyc_Lex_string_buffer,sizeof(char))- 1);if(Cyc_Lex_string_pos
>= sz){int newsz=sz;while(Cyc_Lex_string_pos >= newsz){newsz=newsz * 2;}{
unsigned int _tmp14A;unsigned int _tmp149;struct _dyneither_ptr _tmp148;char*_tmp147;
unsigned int _tmp146;struct _dyneither_ptr str=(_tmp146=(unsigned int)newsz,((
_tmp147=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp146 + 1)),((_tmp148=
_tag_dyneither(_tmp147,sizeof(char),_tmp146 + 1),((((_tmp149=_tmp146,((_tmp14B(&
sz,& _tmp14A,& _tmp149,& _tmp147),_tmp147[_tmp149]=(char)0)))),_tmp148)))))));Cyc_Lex_string_buffer=
str;};}{char _tmp14E;char _tmp14D;struct _dyneither_ptr _tmp14C;(_tmp14C=
_dyneither_ptr_plus(Cyc_Lex_string_buffer,sizeof(char),Cyc_Lex_string_pos),((
_tmp14D=*((char*)_check_dyneither_subscript(_tmp14C,sizeof(char),0)),((_tmp14E=c,((
_get_dyneither_size(_tmp14C,sizeof(char))== 1  && (_tmp14D == '\000'  && _tmp14E != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp14C.curr)=_tmp14E)))))));}++ Cyc_Lex_string_pos;}
void Cyc_Lex_store_string(struct _dyneither_ptr s){int sz=(int)Cyc_strlen((struct
_dyneither_ptr)s);int i=0;for(0;i < sz;++ i){Cyc_Lex_store_string_char(*((const char*)
_check_dyneither_subscript(s,sizeof(char),i)));}}struct _dyneither_ptr Cyc_Lex_get_stored_string(){
struct _dyneither_ptr str=Cyc_substring((struct _dyneither_ptr)Cyc_Lex_string_buffer,
0,(unsigned long)Cyc_Lex_string_pos);Cyc_Lex_string_pos=0;return str;}struct Cyc_Lex_Ldecls{
struct Cyc_Set_Set*typedefs;struct Cyc_Set_Set*namespaces;};typedef struct Cyc_Lex_Ldecls*
Cyc_Lex_ldecls_t;struct Cyc_Lex_Lvis{struct Cyc_List_List*current_namespace;struct
Cyc_List_List*imported_namespaces;};typedef struct Cyc_Lex_Lvis*Cyc_Lex_lvis_t;
struct Cyc_Lex_Lstate{struct Cyc_List_List*lstack;struct Cyc_Dict_Dict decls;};
typedef struct Cyc_Lex_Lstate*Cyc_Lex_lstate_t;static struct Cyc_Core_Opt*Cyc_Lex_lstate=
0;static void Cyc_Lex_typedef_init(){struct Cyc_Lex_Lvis*_tmp14F;struct Cyc_Lex_Lvis*
_tmp7E=(_tmp14F=_cycalloc(sizeof(*_tmp14F)),((_tmp14F->current_namespace=0,((
_tmp14F->imported_namespaces=0,_tmp14F)))));struct Cyc_List_List*_tmp150;struct
Cyc_List_List*_tmp7F=(_tmp150=_cycalloc(sizeof(*_tmp150)),((_tmp150->hd=_tmp7E,((
_tmp150->tl=0,_tmp150)))));struct Cyc_Lex_Ldecls*_tmp151;struct Cyc_Dict_Dict
init_decls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,
struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(int(*cmp)(
struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp),
0,((_tmp151=_cycalloc(sizeof(*_tmp151)),((_tmp151->typedefs=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp),((
_tmp151->namespaces=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp),_tmp151)))))));struct Cyc_Lex_Lstate*
_tmp154;struct Cyc_Core_Opt*_tmp153;Cyc_Lex_lstate=((_tmp153=_cycalloc(sizeof(*
_tmp153)),((_tmp153->v=((_tmp154=_cycalloc(sizeof(*_tmp154)),((_tmp154->lstack=
_tmp7F,((_tmp154->decls=init_decls,_tmp154)))))),_tmp153))));}static struct Cyc_List_List*
Cyc_Lex_get_absolute_namespace(struct Cyc_List_List*ns){struct _dyneither_ptr*n=(
struct _dyneither_ptr*)ns->hd;{struct Cyc_List_List*ls=(struct Cyc_List_List*)((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack;
for(0;ls != 0;ls=ls->tl){struct Cyc_Lex_Lvis*lv=(struct Cyc_Lex_Lvis*)ls->hd;struct
Cyc_List_List*_tmp155;struct Cyc_List_List*x=(_tmp155=_cycalloc(sizeof(*_tmp155)),((
_tmp155->hd=lv->current_namespace,((_tmp155->tl=lv->imported_namespaces,_tmp155)))));
for(0;x != 0;x=x->tl){struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,(struct Cyc_List_List*)
x->hd);if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
ld->namespaces,n))return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)((struct Cyc_List_List*)x->hd,(struct Cyc_List_List*)
ns);}}}{const char*_tmp15B;void*_tmp15A[1];const char*_tmp159;struct Cyc_String_pa_PrintArg_struct
_tmp158;Cyc_yyerror((struct _dyneither_ptr)((_tmp158.tag=0,((_tmp158.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_str_sepstr((struct Cyc_List_List*)ns,((
_tmp159="::",_tag_dyneither(_tmp159,sizeof(char),3))))),((_tmp15A[0]=& _tmp158,
Cyc_aprintf(((_tmp15B="undeclared namespace %s",_tag_dyneither(_tmp15B,sizeof(
char),24))),_tag_dyneither(_tmp15A,sizeof(void*),1)))))))));}return 0;}static void
Cyc_Lex_recompute_typedefs(){struct _DynRegionHandle*_tmp8B;struct Cyc_Lex_Trie*
_tmp8C;struct Cyc_Lex_DynTrie _tmp8A=*((struct Cyc_Lex_DynTrie*)_check_null(Cyc_Lex_typedefs_trie));
_tmp8B=_tmp8A.dyn;_tmp8C=_tmp8A.t;Cyc_Core_free_dynregion(_tmp8B);{struct
_DynRegionHandle*_tmp8E;struct Cyc_Core_NewRegion _tmp8D=Cyc_Core__new_dynregion((
const char*)"internal-error",0);_tmp8E=_tmp8D.dynregion;{struct _DynRegionFrame
_tmp8F;struct _RegionHandle*d2=_open_dynregion(& _tmp8F,_tmp8E);{struct Cyc_Lex_Trie*
_tmp15E;struct Cyc_Lex_DynTrie*_tmp15D;Cyc_Lex_typedefs_trie=((_tmp15D=_cycalloc(
sizeof(*_tmp15D)),((_tmp15D->dyn=(struct _DynRegionHandle*)_tmp8E,((_tmp15D->t=(
struct Cyc_Lex_Trie*)((_tmp15E=_region_malloc(d2,sizeof(*_tmp15E)),((_tmp15E->children=
Cyc_Lex_Zero(),((_tmp15E->shared_str=0,_tmp15E)))))),(struct Cyc_Lex_DynTrie*)
_tmp15D))))));}{struct Cyc_List_List*ls=(struct Cyc_List_List*)((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack;for(0;ls != 0;ls=ls->tl){
struct Cyc_Lex_Lvis*lv=(struct Cyc_Lex_Lvis*)ls->hd;struct Cyc_List_List*_tmp15F;
struct Cyc_List_List*x=(_tmp15F=_cycalloc(sizeof(*_tmp15F)),((_tmp15F->hd=lv->current_namespace,((
_tmp15F->tl=lv->imported_namespaces,_tmp15F)))));for(0;x != 0;x=x->tl){struct Cyc_Lex_Ldecls*
ld=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,(
struct Cyc_List_List*)x->hd);((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*
s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,ld->typedefs);}}};;_pop_dynregion(d2);};};}
static int Cyc_Lex_is_typedef_in_namespace(struct Cyc_List_List*ns,struct
_dyneither_ptr*v){struct Cyc_List_List*ans=Cyc_Lex_get_absolute_namespace(ns);
struct _handler_cons _tmp93;_push_handler(& _tmp93);{int _tmp95=0;if(setjmp(_tmp93.handler))
_tmp95=1;if(!_tmp95){{struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,ans);int _tmp96=((int(*)(
struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ld->typedefs,v);
_npop_handler(0);return _tmp96;};_pop_handler();}else{void*_tmp94=(void*)
_exn_thrown;void*_tmp98=_tmp94;_LLE: {struct Cyc_Dict_Absent_exn_struct*_tmp99=(
struct Cyc_Dict_Absent_exn_struct*)_tmp98;if(_tmp99->tag != Cyc_Dict_Absent)goto
_LL10;}_LLF: return 0;_LL10:;_LL11:(void)_throw(_tmp98);_LLD:;}};}static int Cyc_Lex_is_typedef(
struct Cyc_List_List*ns,struct _dyneither_ptr*v){if(ns != 0)return Cyc_Lex_is_typedef_in_namespace((
struct Cyc_List_List*)ns,v);{struct _dyneither_ptr _tmp9A=*v;int len=(int)(
_get_dyneither_size(_tmp9A,sizeof(char))- 1);struct _DynRegionHandle*_tmp9C;
struct Cyc_Lex_Trie*_tmp9D;struct Cyc_Lex_DynTrie _tmp9B=*((struct Cyc_Lex_DynTrie*)
_check_null(Cyc_Lex_typedefs_trie));_tmp9C=_tmp9B.dyn;_tmp9D=_tmp9B.t;{struct
_DynRegionFrame _tmp9E;struct _RegionHandle*d=_open_dynregion(& _tmp9E,_tmp9C);{int
i=0;for(0;i < len;++ i){union Cyc_Lex_TrieChildren _tmp9F=((struct Cyc_Lex_Trie*)
_check_null(_tmp9D))->children;int _tmpA0;struct _tuple19 _tmpA1;int _tmpA2;struct
Cyc_Lex_Trie*_tmpA3;struct _tuple19 _tmpA4;struct Cyc_Lex_Trie*_tmpA5;struct Cyc_Lex_Trie**
_tmpA6;_LL13: if((_tmp9F.Zero).tag != 1)goto _LL15;_tmpA0=(int)(_tmp9F.Zero).val;
_LL14: {int _tmpA7=0;_npop_handler(0);return _tmpA7;}_LL15: if((_tmp9F.One).tag != 2)
goto _LL17;_tmpA1=(struct _tuple19)(_tmp9F.One).val;_tmpA2=_tmpA1.f1;_tmpA3=_tmpA1.f2;
if(!(_tmpA2 != *((const char*)_check_dyneither_subscript(_tmp9A,sizeof(char),i))))
goto _LL17;_LL16: {int _tmpA8=0;_npop_handler(0);return _tmpA8;}_LL17: if((_tmp9F.One).tag
!= 2)goto _LL19;_tmpA4=(struct _tuple19)(_tmp9F.One).val;_tmpA5=_tmpA4.f2;_LL18:
_tmp9D=_tmpA5;goto _LL12;_LL19: if((_tmp9F.Many).tag != 3)goto _LL12;_tmpA6=(struct
Cyc_Lex_Trie**)(_tmp9F.Many).val;_LL1A: {struct Cyc_Lex_Trie*_tmpA9=_tmpA6[
_check_known_subscript_notnull(64,Cyc_Lex_trie_char((int)*((const char*)
_check_dyneither_subscript(_tmp9A,sizeof(char),i))))];if(_tmpA9 == 0){int _tmpAA=0;
_npop_handler(0);return _tmpAA;}_tmp9D=_tmpA9;goto _LL12;}_LL12:;}}{int _tmpAB=((
struct Cyc_Lex_Trie*)_check_null(_tmp9D))->shared_str;_npop_handler(0);return
_tmpAB;};;_pop_dynregion(d);};};}void Cyc_Lex_enter_namespace(struct
_dyneither_ptr*s){struct Cyc_List_List*ns=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_List_List*_tmp160;struct Cyc_List_List*new_ns=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ns,((_tmp160=
_cycalloc(sizeof(*_tmp160)),((_tmp160->hd=s,((_tmp160->tl=0,_tmp160)))))));{
struct Cyc_Lex_Lvis*_tmp163;struct Cyc_List_List*_tmp162;((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack=((_tmp162=_cycalloc(
sizeof(*_tmp162)),((_tmp162->hd=((_tmp163=_cycalloc(sizeof(*_tmp163)),((_tmp163->current_namespace=
new_ns,((_tmp163->imported_namespaces=0,_tmp163)))))),((_tmp162->tl=(struct Cyc_List_List*)((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack,
_tmp162))))));}{struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->decls,ns);if(!((int(*)(struct Cyc_Set_Set*s,
struct _dyneither_ptr*elt))Cyc_Set_member)(ld->namespaces,s)){{struct Cyc_Lex_Ldecls*
_tmp164;((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*
v))Cyc_Dict_insert)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->decls,ns,((_tmp164=_cycalloc(sizeof(*_tmp164)),((_tmp164->typedefs=
ld->typedefs,((_tmp164->namespaces=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,
struct _dyneither_ptr*elt))Cyc_Set_insert)(ld->namespaces,s),_tmp164)))))));}{
struct Cyc_Lex_Ldecls*_tmp165;((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->decls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,new_ns,((_tmp165=
_cycalloc(sizeof(*_tmp165)),((_tmp165->typedefs=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp),((
_tmp165->namespaces=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp),_tmp165)))))));};}((void(*)(
void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,(((
struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,
new_ns))->typedefs);};}void Cyc_Lex_leave_namespace(){((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack=(struct Cyc_List_List*)
_check_null((((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->tl);
Cyc_Lex_recompute_typedefs();}void Cyc_Lex_enter_using(struct _tuple0*q){struct Cyc_List_List*
ns;{union Cyc_Absyn_Nmspace _tmpB1=(*q).f1;int _tmpB2;struct Cyc_List_List*_tmpB3;
struct Cyc_List_List*_tmpB4;struct Cyc_List_List*_tmpB5;_LL1C: if((_tmpB1.Loc_n).tag
!= 4)goto _LL1E;_tmpB2=(int)(_tmpB1.Loc_n).val;_LL1D:{struct Cyc_List_List*_tmp166;
ns=(struct Cyc_List_List*)((_tmp166=_cycalloc(sizeof(*_tmp166)),((_tmp166->hd=(*q).f2,((
_tmp166->tl=0,_tmp166))))));}goto _LL1B;_LL1E: if((_tmpB1.Rel_n).tag != 1)goto _LL20;
_tmpB3=(struct Cyc_List_List*)(_tmpB1.Rel_n).val;_LL1F: _tmpB4=_tmpB3;goto _LL21;
_LL20: if((_tmpB1.C_n).tag != 3)goto _LL22;_tmpB4=(struct Cyc_List_List*)(_tmpB1.C_n).val;
_LL21: _tmpB5=_tmpB4;goto _LL23;_LL22: if((_tmpB1.Abs_n).tag != 2)goto _LL1B;_tmpB5=(
struct Cyc_List_List*)(_tmpB1.Abs_n).val;_LL23:{struct Cyc_List_List*_tmp167;ns=(
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(_tmpB5,((_tmp167=_cycalloc(sizeof(*
_tmp167)),((_tmp167->hd=(*q).f2,((_tmp167->tl=0,_tmp167))))))));}goto _LL1B;_LL1B:;}{
struct Cyc_List_List*_tmpB8=Cyc_Lex_get_absolute_namespace(ns);struct Cyc_List_List*
_tmpB9=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;{struct Cyc_List_List*
_tmp168;((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=((_tmp168=
_cycalloc(sizeof(*_tmp168)),((_tmp168->hd=_tmpB8,((_tmp168->tl=_tmpB9,_tmp168))))));}((
void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,(((
struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,
_tmpB8))->typedefs);};}void Cyc_Lex_leave_using(){struct Cyc_List_List*_tmpBB=((
struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;((struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=((
struct Cyc_List_List*)_check_null(_tmpBB))->tl;Cyc_Lex_recompute_typedefs();}void
Cyc_Lex_register_typedef(struct _tuple0*q){struct Cyc_List_List*_tmpBC=((struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_Dict_Dict _tmpBD=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->decls;struct Cyc_Lex_Ldecls*_tmpBE=((struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmpBD,_tmpBC);
struct Cyc_Lex_Ldecls*_tmp169;struct Cyc_Lex_Ldecls*_tmpBF=(_tmp169=_cycalloc(
sizeof(*_tmp169)),((_tmp169->typedefs=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*
s,struct _dyneither_ptr*elt))Cyc_Set_insert)(_tmpBE->typedefs,(*q).f2),((_tmp169->namespaces=
_tmpBE->namespaces,_tmp169)))));((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->decls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(_tmpBD,_tmpBC,
_tmpBF);Cyc_Lex_insert_typedef((*q).f2);}static short Cyc_Lex_process_id(struct Cyc_Lexing_lexbuf*
lbuf){int symbol_num=Cyc_Lex_str_index_lbuf(lbuf);if(symbol_num <= Cyc_Lex_num_kws){
short res=(short)*((int*)_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(int),
symbol_num - 1));switch(res){case 319: _LL24:{const char*_tmp16C;void*_tmp16B;(
_tmp16B=0,Cyc_Tcutil_warn((unsigned int)Cyc_yylloc.first_line,((_tmp16C="deprecated ZEROTERM, use @zeroterm",
_tag_dyneither(_tmp16C,sizeof(char),35))),_tag_dyneither(_tmp16B,sizeof(void*),0)));}
break;case 318: _LL25:{const char*_tmp16F;void*_tmp16E;(_tmp16E=0,Cyc_Tcutil_warn((
unsigned int)Cyc_yylloc.first_line,((_tmp16F="deprecated NOZEROTERM, use @nozeroterm",
_tag_dyneither(_tmp16F,sizeof(char),39))),_tag_dyneither(_tmp16E,sizeof(void*),0)));}
break;case 306: _LL26:{const char*_tmp172;void*_tmp171;(_tmp171=0,Cyc_Tcutil_warn((
unsigned int)Cyc_yylloc.first_line,((_tmp172="deprecated tunion, use datatype",
_tag_dyneither(_tmp172,sizeof(char),32))),_tag_dyneither(_tmp171,sizeof(void*),0)));}
break;case 305: _LL27:{const char*_tmp175;void*_tmp174;(_tmp174=0,Cyc_Tcutil_warn((
unsigned int)Cyc_yylloc.first_line,((_tmp175="deprecated xtunion, use @extensible datatype",
_tag_dyneither(_tmp175,sizeof(char),45))),_tag_dyneither(_tmp174,sizeof(void*),0)));}
break;default: _LL28: break;}return(short)*((int*)_check_dyneither_subscript(Cyc_Lex_kw_nums,
sizeof(int),symbol_num - 1));}{struct _dyneither_ptr*_tmpC9=Cyc_Lex_get_symbol(
symbol_num);Cyc_Lex_token_string=*_tmpC9;if(Cyc_Lex_is_typedef(0,_tmpC9))return
369;return 361;};}static short Cyc_Lex_process_qual_id(struct Cyc_Lexing_lexbuf*lbuf){
int i=lbuf->lex_start_pos;int end=lbuf->lex_curr_pos;struct _dyneither_ptr s=lbuf->lex_buffer;
struct _dyneither_ptr*v=0;struct Cyc_List_List*rev_vs=0;while(i < end){int start=i;
for(0;i < end  && *((char*)_check_dyneither_subscript(s,sizeof(char),i))!= ':';++ i){;}
if(start == i){struct Cyc_Core_Impossible_exn_struct _tmp17B;const char*_tmp17A;
struct Cyc_Core_Impossible_exn_struct*_tmp179;(int)_throw((void*)((_tmp179=
_cycalloc(sizeof(*_tmp179)),((_tmp179[0]=((_tmp17B.tag=Cyc_Core_Impossible,((
_tmp17B.f1=((_tmp17A="bad namespace",_tag_dyneither(_tmp17A,sizeof(char),14))),
_tmp17B)))),_tmp179)))));}{int vlen=i - start;if(v != 0){struct Cyc_List_List*
_tmp17C;rev_vs=((_tmp17C=_cycalloc(sizeof(*_tmp17C)),((_tmp17C->hd=(struct
_dyneither_ptr*)v,((_tmp17C->tl=rev_vs,_tmp17C))))));}v=(struct _dyneither_ptr*)
Cyc_Lex_get_symbol(Cyc_Lex_str_index((struct _dyneither_ptr)s,start,vlen));i +=2;};}
if(v == 0){struct Cyc_Core_Impossible_exn_struct _tmp182;const char*_tmp181;struct
Cyc_Core_Impossible_exn_struct*_tmp180;(int)_throw((void*)((_tmp180=_cycalloc(
sizeof(*_tmp180)),((_tmp180[0]=((_tmp182.tag=Cyc_Core_Impossible,((_tmp182.f1=((
_tmp181="bad namespace",_tag_dyneither(_tmp181,sizeof(char),14))),_tmp182)))),
_tmp180)))));}{struct Cyc_List_List*vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(rev_vs);{struct _tuple0*_tmp183;Cyc_Lex_token_qvar=((_tmp183=
_cycalloc(sizeof(*_tmp183)),((_tmp183->f1=Cyc_Absyn_Rel_n(vs),((_tmp183->f2=(
struct _dyneither_ptr*)v,_tmp183))))));}if(Cyc_Lex_is_typedef(vs,(struct
_dyneither_ptr*)v))return 371;return 370;};}int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_scan_charconst(struct Cyc_Lexing_lexbuf*);int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_wstrng(struct Cyc_Lexing_lexbuf*);int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_cyc_qual(struct Cyc_Lexing_lexbuf*);int Cyc_Lex_snarf_asm(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_snarf_asm_body(struct Cyc_Lexing_lexbuf*);int Cyc_yylex(struct Cyc_Lexing_lexbuf*
lbuf,union Cyc_YYSTYPE*yylval,struct Cyc_Yyltype*yyllocptr){int ans=((int(*)(struct
Cyc_Lexing_lexbuf*))Cyc_Lex_token)(lbuf);Cyc_yylloc.first_line=(yyllocptr->first_line=((
int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_start)(lbuf));Cyc_yylloc.last_line=(
yyllocptr->last_line=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_end)(
lbuf));switch(ans){case 361: _LL2A:{union Cyc_YYSTYPE _tmp184;*yylval=(((_tmp184.String_tok).val=
Cyc_Lex_token_string,(((_tmp184.String_tok).tag=3,_tmp184))));}break;case 369:
_LL2B:{union Cyc_YYSTYPE _tmp185;*yylval=(((_tmp185.String_tok).val=Cyc_Lex_token_string,(((
_tmp185.String_tok).tag=3,_tmp185))));}break;case 370: _LL2C:{union Cyc_YYSTYPE
_tmp186;*yylval=(((_tmp186.QualId_tok).val=Cyc_Lex_token_qvar,(((_tmp186.QualId_tok).tag=
5,_tmp186))));}break;case 371: _LL2D:{union Cyc_YYSTYPE _tmp187;*yylval=(((_tmp187.QualId_tok).val=
Cyc_Lex_token_qvar,(((_tmp187.QualId_tok).tag=5,_tmp187))));}break;case 368: _LL2E:{
union Cyc_YYSTYPE _tmp188;*yylval=(((_tmp188.String_tok).val=Cyc_Lex_token_string,(((
_tmp188.String_tok).tag=3,_tmp188))));}break;case 362: _LL2F:{union Cyc_YYSTYPE
_tmp189;*yylval=(((_tmp189.Int_tok).val=Cyc_Lex_token_int,(((_tmp189.Int_tok).tag=
1,_tmp189))));}break;case 365: _LL30:{union Cyc_YYSTYPE _tmp18A;*yylval=(((_tmp18A.Char_tok).val=
Cyc_Lex_token_char,(((_tmp18A.Char_tok).tag=2,_tmp18A))));}break;case 366: _LL31:{
union Cyc_YYSTYPE _tmp18B;*yylval=(((_tmp18B.String_tok).val=Cyc_Lex_token_string,(((
_tmp18B.String_tok).tag=3,_tmp18B))));}break;case 367: _LL32:{union Cyc_YYSTYPE
_tmp18C;*yylval=(((_tmp18C.String_tok).val=Cyc_Lex_token_string,(((_tmp18C.String_tok).tag=
3,_tmp18C))));}break;case 363: _LL33:{union Cyc_YYSTYPE _tmp18D;*yylval=(((_tmp18D.String_tok).val=
Cyc_Lex_token_string,(((_tmp18D.String_tok).tag=3,_tmp18D))));}break;case 364:
_LL34:{union Cyc_YYSTYPE _tmp18E;*yylval=(((_tmp18E.String_tok).val=Cyc_Lex_token_string,(((
_tmp18E.String_tok).tag=3,_tmp18E))));}break;case 373: _LL35:{union Cyc_YYSTYPE
_tmp18F;*yylval=(((_tmp18F.Asm_tok).val=Cyc_Lex_token_asm,(((_tmp18F.Asm_tok).tag=
6,_tmp18F))));}break;default: _LL36: break;}return ans;}const int Cyc_Lex_lex_base[
279]={0,24,117,118,119,94,138,120,- 7,- 1,4,- 4,- 5,2,- 6,- 2,5,- 3,6,153,- 1,12,25,38,
21,34,34,42,162,134,12,21,- 16,- 17,9,173,165,- 18,- 12,- 11,181,- 13,- 10,- 4,- 5,- 8,- 9,
238,261,189,- 14,245,2,- 17,- 17,- 18,10,213,331,- 19,11,347,404,427,355,- 15,288,- 15,
- 16,12,- 1,482,363,555,630,459,- 16,- 23,24,48,73,56,83,57,45,57,53,76,65,79,705,86,
86,82,780,92,95,97,855,97,93,100,87,109,109,100,120,930,113,115,1005,130,142,119,
147,145,143,149,1080,168,168,160,168,1155,160,171,159,163,160,177,168,175,1230,
179,191,192,1305,186,1380,203,222,214,210,227,235,226,241,1455,- 61,336,30,- 43,
341,31,305,507,32,310,327,501,377,1532,1575,294,331,295,316,- 7,1645,1734,296,
1809,1867,1942,303,- 60,- 25,- 31,2017,309,2092,2167,2225,2300,2375,311,2450,2525,
2600,2675,2750,2825,2900,2975,- 26,- 44,- 45,326,- 29,359,- 34,- 28,- 37,363,- 38,3050,
3079,516,345,355,1542,3089,3119,1613,3060,3152,3183,3221,347,358,3291,3329,350,
360,401,411,403,414,- 9,- 42,433,- 41,13,376,3261,- 36,- 21,- 35,- 20,- 22,1,3369,2,386,
1561,387,398,400,401,403,405,409,411,426,3442,3526,- 58,- 52,- 51,- 50,- 49,- 48,- 47,-
46,- 53,- 56,- 57,1636,446,- 54,- 55,- 59,- 30,- 27,- 24,1386,- 39,14,530};const int Cyc_Lex_lex_backtrk[
279]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,- 1,- 1,6,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,5,3,4,- 1,- 1,15,1,18,- 1,- 1,- 1,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,14,13,- 1,- 1,- 1,- 1,
- 1,- 1,16,1,19,- 1,2,14,- 1,15,14,- 1,- 1,- 1,- 1,14,- 1,17,12,- 1,13,12,- 1,- 1,22,22,22,
22,22,22,- 1,- 1,- 1,- 1,- 1,- 1,12,- 1,- 1,- 1,18,- 1,- 1,- 1,11,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,
- 1,- 1,16,- 1,- 1,- 1,- 1,- 1,- 1,- 1,17,- 1,- 1,- 1,- 1,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,13,- 1,- 1,
- 1,20,- 1,19,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,14,- 1,39,60,- 1,60,60,60,60,60,60,60,60,60,8,
10,60,60,60,60,- 1,2,2,60,60,60,2,60,- 1,- 1,- 1,2,- 1,2,0,- 1,4,4,- 1,5,3,3,3,3,3,3,1,
- 1,- 1,- 1,32,- 1,31,- 1,- 1,- 1,- 1,- 1,12,10,- 1,10,10,- 1,11,12,- 1,- 1,12,8,9,8,8,- 1,7,7,
7,9,9,8,8,- 1,- 1,- 1,- 1,40,- 1,12,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,
39};const int Cyc_Lex_lex_default[279]={148,77,67,54,32,29,- 1,8,0,0,16,0,0,- 1,0,0,
16,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,29,- 1,- 1,0,0,- 1,- 1,- 1,0,0,0,- 1,0,0,0,0,0,0,
- 1,- 1,- 1,0,- 1,- 1,0,0,0,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,0,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,
0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,275,- 1,- 1,240,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,230,0,- 1,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,0,- 1,- 1,0,0,0,0,0,0,275,0,
- 1,- 1};const int Cyc_Lex_lex_trans[3783]={0,0,0,0,0,0,0,0,0,149,149,149,149,149,17,
17,17,18,18,53,37,17,76,231,276,0,0,0,0,0,0,0,149,150,151,152,20,153,154,155,271,
270,156,157,15,158,159,160,161,162,162,162,162,162,162,162,162,162,163,15,164,
165,166,20,167,168,168,168,168,168,168,168,168,168,168,168,169,168,168,168,168,
168,168,168,168,168,168,168,168,168,168,65,274,239,170,171,172,173,168,168,168,
168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,
168,168,22,174,78,79,68,55,33,69,56,34,23,80,24,30,25,81,26,82,31,27,28,139,137,
83,19,19,19,19,19,57,35,9,10,70,99,84,85,11,12,19,19,19,19,19,13,86,87,19,28,28,
28,28,28,- 1,88,20,89,91,- 1,51,51,111,19,51,92,90,95,112,93,94,20,28,96,97,98,108,
38,100,102,15,103,39,51,104,105,106,71,58,36,101,40,40,40,40,40,40,40,40,107,66,
66,109,110,66,119,41,49,49,49,49,49,49,49,49,50,50,50,50,50,50,50,50,66,124,20,
115,52,113,114,125,116,51,51,175,42,51,117,- 1,- 1,17,43,118,120,121,44,122,123,
133,21,126,127,128,14,129,51,130,8,- 1,45,131,46,132,47,48,48,48,48,48,48,48,48,
48,48,134,66,66,135,136,66,138,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,
140,66,52,20,141,142,143,48,48,48,48,48,48,144,145,146,48,48,48,48,48,48,17,147,
272,60,278,278,278,278,278,17,276,202,37,277,203,50,194,48,48,48,48,48,48,176,41,
273,182,278,182,38,238,59,53,59,37,14,53,197,61,61,61,61,61,61,61,61,198,236,237,
- 1,199,76,200,50,64,64,64,64,64,64,64,64,65,65,65,65,65,65,65,65,75,75,75,75,75,
75,75,75,229,201,204,235,39,230,269,265,177,43,44,39,39,46,14,46,45,45,264,77,
263,262,8,261,231,260,45,232,46,259,42,258,62,63,63,63,63,63,63,63,63,63,63,39,
39,46,257,46,45,45,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,266,42,42,228,
0,228,0,63,63,63,63,63,63,0,0,0,63,63,63,63,63,63,41,41,41,41,41,41,41,41,0,39,-
1,42,42,228,42,228,0,63,63,63,63,63,63,72,72,72,72,72,72,72,72,0,278,278,278,278,
278,0,38,- 1,233,0,234,234,234,234,234,234,234,234,234,234,210,0,210,278,0,211,
211,211,211,211,211,211,211,211,211,46,0,0,0,0,15,17,0,0,0,43,0,0,0,0,0,0,0,44,0,
0,0,14,- 1,8,241,45,0,73,74,74,74,74,74,74,74,74,74,74,0,0,0,0,0,0,0,74,74,74,74,
74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,0,0,0,0,0,0,74,
74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,
74,74,74,74,74,74,74,74,0,- 1,0,0,0,0,0,74,74,74,74,74,74,74,74,74,74,74,74,74,74,
74,74,74,74,74,74,74,74,74,74,74,74,0,0,0,0,0,0,74,74,74,74,74,74,74,74,74,74,74,
74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,15,15,15,15,15,15,15,15,15,15,- 1,0,
0,0,0,0,0,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
15,15,15,0,0,0,0,15,0,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,45,45,45,45,45,45,45,45,45,45,0,0,0,0,0,0,0,45,45,45,45,45,
45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,0,0,0,0,45,0,45,
45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,20,20,
20,20,20,20,20,20,20,20,0,0,0,0,0,0,0,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
20,20,20,20,20,20,20,20,20,20,20,20,0,0,0,0,20,0,20,20,20,20,20,20,20,20,20,20,
20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,44,44,44,44,44,44,44,44,44,44,0,
0,0,0,0,0,0,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,0,0,0,0,44,0,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,14,14,14,14,14,14,14,14,14,14,0,0,0,0,0,0,0,14,14,14,14,14,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,0,0,0,0,14,0,14,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,8,8,8,
8,8,8,8,8,8,8,0,0,0,0,0,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
0,0,0,0,8,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,39,39,39,39,39,
39,39,39,39,39,0,0,0,0,0,0,0,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,
39,39,39,39,39,39,39,39,39,0,0,0,0,39,0,39,39,39,39,39,39,39,39,39,39,39,39,39,
39,39,39,39,39,39,39,39,39,39,39,39,39,17,17,17,17,17,17,17,17,17,17,0,0,0,0,0,0,
0,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
0,0,0,0,17,0,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
17,17,17,17,42,42,42,42,42,42,42,42,42,42,0,0,0,0,0,0,0,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,276,0,0,277,42,0,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,46,46,46,46,
46,46,46,46,46,46,0,0,0,0,0,0,0,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,
46,46,46,46,46,46,46,46,46,46,0,0,0,0,46,0,46,46,46,46,46,46,46,46,46,46,46,46,
46,46,46,46,46,46,46,46,46,46,46,46,46,46,43,43,43,43,43,43,43,43,43,43,0,0,0,0,
0,0,0,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,
43,0,0,0,0,43,0,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,
43,43,43,43,43,205,0,216,216,216,216,216,216,216,216,217,217,211,211,211,211,211,
211,211,211,211,211,266,207,0,0,0,0,0,0,218,267,267,267,267,267,267,267,267,219,
0,0,220,205,0,206,206,206,206,206,206,206,206,206,206,207,0,0,0,0,0,0,218,0,- 1,0,
207,0,0,0,0,219,0,208,220,0,0,0,214,0,214,0,209,215,215,215,215,215,215,215,215,
215,215,0,0,0,0,266,207,0,0,0,0,0,0,208,268,268,268,268,268,268,268,268,209,178,
178,178,178,178,178,178,178,178,178,179,0,0,0,0,0,0,178,178,178,178,178,178,178,
178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,0,0,
0,0,178,0,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,
178,178,178,178,178,178,178,178,178,195,0,0,0,0,196,0,0,0,0,0,0,0,0,178,178,178,
178,178,178,178,178,178,178,179,0,0,0,0,0,0,178,178,178,178,178,178,178,178,178,
178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,0,0,0,0,178,
0,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,
178,178,178,178,178,178,178,187,187,187,187,187,187,187,187,187,187,0,0,0,0,0,0,
0,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,
187,187,187,187,187,187,187,0,0,0,0,188,0,187,187,187,187,187,187,187,187,187,
187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,186,186,186,
186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,
186,186,186,0,0,0,0,186,0,186,186,186,186,186,186,186,186,186,186,186,186,186,
186,186,186,186,186,186,186,186,186,186,186,186,186,178,178,178,178,178,178,178,
178,178,178,179,0,0,0,0,0,0,178,178,178,178,178,178,178,178,178,178,178,178,178,
178,178,178,178,178,178,178,178,178,178,178,178,178,0,0,0,0,178,0,178,178,178,
178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,180,178,178,178,178,
178,178,178,178,178,178,178,178,178,178,178,178,178,179,0,0,0,0,0,0,178,178,178,
178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,
178,178,178,0,0,0,0,178,0,178,178,178,178,178,178,178,178,178,178,178,178,178,
178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,
178,178,178,179,0,0,0,0,0,0,178,178,178,178,178,178,178,178,178,178,178,178,178,
178,178,178,178,178,178,178,178,178,178,178,178,178,0,0,0,0,178,0,178,178,178,
178,178,178,178,178,178,178,178,178,181,178,178,178,178,178,178,178,178,178,178,
178,178,178,178,178,178,178,178,178,178,178,178,178,179,0,0,0,0,0,0,178,178,178,
178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,
178,178,178,0,0,0,0,178,0,178,178,178,178,178,178,178,178,178,178,178,178,178,
178,178,178,178,178,178,178,178,178,178,178,178,178,183,183,183,183,183,183,183,
183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,0,0,
0,0,184,0,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,
183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,185,
0,0,0,0,0,0,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,
183,183,183,183,183,183,183,183,183,0,0,0,0,183,0,183,183,183,183,183,183,183,
183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,184,
184,184,184,184,184,184,184,184,184,0,0,0,0,0,0,0,184,184,184,184,184,184,184,
184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,0,0,
0,0,184,0,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,
184,184,184,184,184,184,184,184,184,186,186,186,186,186,186,186,186,186,186,0,0,
0,0,0,0,0,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,
186,186,186,186,186,186,186,186,186,0,0,0,0,186,0,186,186,186,186,186,186,186,
186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,187,
187,187,187,187,187,187,187,187,187,0,0,0,0,0,0,0,187,187,187,187,187,187,187,
187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,0,0,
0,0,187,0,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,
187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,0,0,
0,0,0,0,0,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,
187,187,187,187,187,187,187,187,187,0,0,0,0,187,0,189,187,187,187,187,187,187,
187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,
187,187,187,187,187,187,187,187,187,0,0,0,0,0,0,0,187,187,187,187,187,187,187,
187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,0,0,
0,0,187,0,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,
187,190,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,0,0,
0,0,0,0,0,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,
187,187,187,187,187,187,187,187,187,0,0,0,0,187,0,187,187,187,187,187,187,187,
187,187,187,187,187,191,187,187,187,187,187,187,187,187,187,187,187,187,187,187,
187,187,187,187,187,187,187,187,187,0,0,0,0,0,0,0,187,187,187,187,187,187,187,
187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,0,0,
0,0,192,0,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,
187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,0,0,
0,0,0,0,0,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,
187,187,187,187,187,187,187,187,187,0,0,0,0,193,0,187,187,187,187,187,187,187,
187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,
187,187,187,187,187,187,187,187,187,0,0,0,0,0,0,0,187,187,187,187,187,187,187,
187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,0,0,
0,0,187,0,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,
187,187,187,187,187,187,187,187,187,212,212,212,212,212,212,212,212,212,212,215,
215,215,215,215,215,215,215,215,215,0,213,41,0,0,0,0,205,41,206,206,206,206,206,
206,206,206,206,206,211,211,211,211,211,211,211,211,211,211,0,207,0,0,213,41,0,0,
208,0,0,41,38,0,0,0,0,209,38,0,212,212,212,212,212,212,212,212,212,212,0,0,0,207,
0,0,0,0,0,0,208,213,41,0,38,0,0,0,41,209,38,0,0,215,215,215,215,215,215,215,215,
215,215,0,0,0,0,0,0,0,0,0,0,213,41,41,0,0,0,0,41,41,205,0,216,216,216,216,216,
216,216,216,217,217,0,0,0,0,0,0,0,0,0,0,0,207,0,41,0,0,0,0,226,41,0,0,0,0,0,0,
205,227,217,217,217,217,217,217,217,217,217,217,0,0,0,0,0,207,0,0,0,0,0,207,226,
0,0,0,0,0,224,0,0,227,0,0,0,0,0,225,0,0,234,234,234,234,234,234,234,234,234,234,
0,0,0,207,0,0,0,0,0,0,224,213,41,0,0,0,0,0,41,225,221,221,221,221,221,221,221,
221,221,221,0,0,0,0,0,0,0,221,221,221,221,221,221,213,41,0,0,0,0,0,41,0,0,0,0,0,
0,0,221,221,221,221,221,221,221,221,221,221,0,221,221,221,221,221,221,221,221,
221,221,221,221,0,0,0,242,0,222,0,0,243,0,0,0,0,0,223,0,0,244,244,244,244,244,
244,244,244,0,221,221,221,221,221,221,245,0,0,0,0,222,0,0,0,0,0,0,0,0,223,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,246,0,0,0,0,247,248,0,0,0,249,0,0,0,0,0,0,0,250,0,0,0,251,
0,252,0,253,0,254,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,0,0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,256,0,0,0,0,0,0,0,0,255,255,
255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,0,0,0,
0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};const int Cyc_Lex_lex_check[
3783]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,10,16,18,10,16,34,56,60,69,232,277,-
1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,52,0,0,0,240,242,0,0,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
30,0,0,0,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,150,153,156,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,21,0,1,1,2,3,4,2,3,4,22,
1,23,5,24,1,25,1,5,26,27,78,79,1,6,6,6,6,6,3,4,7,7,2,81,83,84,7,7,19,19,19,19,19,
7,85,86,6,28,28,28,28,28,29,87,6,88,82,29,35,35,80,19,35,82,89,91,80,92,93,19,28,
95,96,97,100,36,99,101,28,102,36,35,103,104,105,2,3,4,99,36,36,36,36,36,36,36,36,
106,57,57,108,109,57,113,36,40,40,40,40,40,40,40,40,49,49,49,49,49,49,49,49,57,
111,57,114,35,112,112,111,115,51,51,0,36,51,116,10,16,36,36,117,119,120,36,121,
122,124,19,125,126,127,36,128,51,129,36,1,36,130,36,131,36,47,47,47,47,47,47,47,
47,47,47,133,66,66,134,135,66,137,47,47,47,47,47,47,48,48,48,48,48,48,48,48,48,
48,139,66,51,66,140,141,142,48,48,48,48,48,48,143,144,145,47,47,47,47,47,47,58,
146,154,58,149,149,149,149,149,5,152,163,157,152,163,165,170,48,48,48,48,48,48,
174,58,154,179,149,185,58,157,158,2,3,4,7,166,166,58,58,58,58,58,58,58,58,197,
158,158,29,164,164,164,58,61,61,61,61,61,61,61,61,64,64,64,64,64,64,64,64,72,72,
72,72,72,72,72,72,160,199,203,233,58,160,243,245,174,58,58,208,209,218,58,219,
222,223,246,160,247,248,58,249,230,250,58,230,58,251,58,252,58,62,62,62,62,62,62,
62,62,62,62,208,209,218,253,219,222,223,62,62,62,62,62,62,63,63,63,63,63,63,63,
63,63,63,268,224,225,226,- 1,227,- 1,63,63,63,63,63,63,- 1,- 1,- 1,62,62,62,62,62,62,
75,75,75,75,75,75,75,75,- 1,71,155,224,225,226,71,227,- 1,63,63,63,63,63,63,71,71,
71,71,71,71,71,71,- 1,278,278,278,278,278,- 1,71,155,159,- 1,159,159,159,159,159,
159,159,159,159,159,207,- 1,207,278,- 1,207,207,207,207,207,207,207,207,207,207,71,
- 1,- 1,- 1,- 1,71,71,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,71,152,71,155,71,
- 1,71,73,73,73,73,73,73,73,73,73,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,73,73,73,73,73,73,73,
73,73,73,73,73,73,73,73,73,73,73,73,73,73,73,73,73,73,- 1,- 1,- 1,- 1,- 1,- 1,73,73,73,
73,73,73,73,73,73,73,73,73,73,73,73,73,73,73,73,73,73,73,73,73,73,73,74,74,74,74,
74,74,74,74,74,74,- 1,230,- 1,- 1,- 1,- 1,- 1,74,74,74,74,74,74,74,74,74,74,74,74,74,
74,74,74,74,74,74,74,74,74,74,74,74,74,- 1,- 1,- 1,- 1,- 1,- 1,74,74,74,74,74,74,74,74,
74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,90,90,90,90,90,90,90,90,90,
90,155,- 1,- 1,- 1,- 1,- 1,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,90,90,90,90,90,- 1,- 1,- 1,- 1,90,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,94,94,94,94,94,94,94,94,94,94,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,
94,94,- 1,- 1,- 1,- 1,94,- 1,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,
94,94,94,94,94,94,94,98,98,98,98,98,98,98,98,98,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,- 1,- 1,- 1,- 1,
98,- 1,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,107,107,107,107,107,107,107,107,107,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,107,107,107,
107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,
107,107,- 1,- 1,- 1,- 1,107,- 1,107,107,107,107,107,107,107,107,107,107,107,107,107,
107,107,107,107,107,107,107,107,107,107,107,107,107,110,110,110,110,110,110,110,
110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,110,110,110,110,110,110,110,110,110,110,110,
110,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,110,- 1,110,
110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,
110,110,110,110,110,118,118,118,118,118,118,118,118,118,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,
118,118,118,118,118,118,- 1,- 1,- 1,- 1,118,- 1,118,118,118,118,118,118,118,118,118,
118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,123,123,123,
123,123,123,123,123,123,123,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,123,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,- 1,- 1,- 1,
- 1,123,- 1,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,132,132,132,132,132,132,132,132,132,132,- 1,-
1,- 1,- 1,- 1,- 1,- 1,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,
132,132,132,132,132,132,132,132,132,132,- 1,- 1,- 1,- 1,132,- 1,132,132,132,132,132,
132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,
132,136,136,136,136,136,136,136,136,136,136,- 1,- 1,- 1,- 1,- 1,- 1,- 1,136,136,136,136,
136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,
136,136,275,- 1,- 1,275,136,- 1,136,136,136,136,136,136,136,136,136,136,136,136,136,
136,136,136,136,136,136,136,136,136,136,136,136,136,138,138,138,138,138,138,138,
138,138,138,- 1,- 1,- 1,- 1,- 1,- 1,- 1,138,138,138,138,138,138,138,138,138,138,138,138,
138,138,138,138,138,138,138,138,138,138,138,138,138,138,- 1,- 1,- 1,- 1,138,- 1,138,
138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,
138,138,138,138,138,147,147,147,147,147,147,147,147,147,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,
147,147,147,147,147,147,- 1,- 1,- 1,- 1,147,- 1,147,147,147,147,147,147,147,147,147,
147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,161,- 1,161,
161,161,161,161,161,161,161,161,161,210,210,210,210,210,210,210,210,210,210,244,
161,- 1,- 1,- 1,- 1,- 1,- 1,161,244,244,244,244,244,244,244,244,161,- 1,- 1,161,162,- 1,
162,162,162,162,162,162,162,162,162,162,161,- 1,- 1,- 1,- 1,- 1,- 1,161,- 1,275,- 1,162,
- 1,- 1,- 1,- 1,161,- 1,162,161,- 1,- 1,- 1,213,- 1,213,- 1,162,213,213,213,213,213,213,
213,213,213,213,- 1,- 1,- 1,- 1,267,162,- 1,- 1,- 1,- 1,- 1,- 1,162,267,267,267,267,267,
267,267,267,162,168,168,168,168,168,168,168,168,168,168,168,- 1,- 1,- 1,- 1,- 1,- 1,
168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,
168,168,168,168,168,168,- 1,- 1,- 1,- 1,168,- 1,168,168,168,168,168,168,168,168,168,
168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,169,- 1,- 1,- 1,
- 1,169,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,169,169,169,169,169,169,169,169,169,169,169,- 1,- 1,
- 1,- 1,- 1,- 1,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,
169,169,169,169,169,169,169,169,169,- 1,- 1,- 1,- 1,169,- 1,169,169,169,169,169,169,
169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,
171,171,171,171,171,171,171,171,171,171,- 1,- 1,- 1,- 1,- 1,- 1,- 1,171,171,171,171,171,
171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,
171,- 1,- 1,- 1,- 1,171,- 1,171,171,171,171,171,171,171,171,171,171,171,171,171,171,
171,171,171,171,171,171,171,171,171,171,171,171,172,172,172,172,172,172,172,172,
172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,- 1,- 1,- 1,
- 1,172,- 1,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,
172,172,172,172,172,172,172,172,172,173,173,173,173,173,173,173,173,173,173,173,
- 1,- 1,- 1,- 1,- 1,- 1,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,
173,173,173,173,173,173,173,173,173,173,173,- 1,- 1,- 1,- 1,173,- 1,173,173,173,173,
173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,
173,173,178,178,178,178,178,178,178,178,178,178,178,- 1,- 1,- 1,- 1,- 1,- 1,178,178,
178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,
178,178,178,178,- 1,- 1,- 1,- 1,178,- 1,178,178,178,178,178,178,178,178,178,178,178,
178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,180,180,180,180,180,
180,180,180,180,180,180,- 1,- 1,- 1,- 1,- 1,- 1,180,180,180,180,180,180,180,180,180,
180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,- 1,- 1,- 1,- 1,
180,- 1,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,
180,180,180,180,180,180,180,180,181,181,181,181,181,181,181,181,181,181,181,- 1,-
1,- 1,- 1,- 1,- 1,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,
181,181,181,181,181,181,181,181,181,181,- 1,- 1,- 1,- 1,181,- 1,181,181,181,181,181,
181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,
181,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,
182,182,182,182,182,182,182,- 1,- 1,- 1,- 1,182,- 1,182,182,182,182,182,182,182,182,
182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,183,183,
183,183,183,183,183,183,183,183,183,- 1,- 1,- 1,- 1,- 1,- 1,183,183,183,183,183,183,
183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,
- 1,- 1,- 1,- 1,183,- 1,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,
183,183,183,183,183,183,183,183,183,183,183,184,184,184,184,184,184,184,184,184,
184,- 1,- 1,- 1,- 1,- 1,- 1,- 1,184,184,184,184,184,184,184,184,184,184,184,184,184,184,
184,184,184,184,184,184,184,184,184,184,184,184,- 1,- 1,- 1,- 1,184,- 1,184,184,184,
184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,
184,184,184,186,186,186,186,186,186,186,186,186,186,- 1,- 1,- 1,- 1,- 1,- 1,- 1,186,186,
186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,
186,186,186,186,- 1,- 1,- 1,- 1,186,- 1,186,186,186,186,186,186,186,186,186,186,186,
186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,187,187,187,187,187,
187,187,187,187,187,- 1,- 1,- 1,- 1,- 1,- 1,- 1,187,187,187,187,187,187,187,187,187,187,
187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,- 1,- 1,- 1,- 1,187,
- 1,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,
187,187,187,187,187,187,187,188,188,188,188,188,188,188,188,188,188,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,
188,188,188,188,188,188,188,188,- 1,- 1,- 1,- 1,188,- 1,188,188,188,188,188,188,188,
188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,189,
189,189,189,189,189,189,189,189,189,- 1,- 1,- 1,- 1,- 1,- 1,- 1,189,189,189,189,189,189,
189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,
- 1,- 1,- 1,- 1,189,- 1,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,
189,189,189,189,189,189,189,189,189,189,189,190,190,190,190,190,190,190,190,190,
190,- 1,- 1,- 1,- 1,- 1,- 1,- 1,190,190,190,190,190,190,190,190,190,190,190,190,190,190,
190,190,190,190,190,190,190,190,190,190,190,190,- 1,- 1,- 1,- 1,190,- 1,190,190,190,
190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,
190,190,190,191,191,191,191,191,191,191,191,191,191,- 1,- 1,- 1,- 1,- 1,- 1,- 1,191,191,
191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,
191,191,191,191,- 1,- 1,- 1,- 1,191,- 1,191,191,191,191,191,191,191,191,191,191,191,
191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,192,192,192,192,192,
192,192,192,192,192,- 1,- 1,- 1,- 1,- 1,- 1,- 1,192,192,192,192,192,192,192,192,192,192,
192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,- 1,- 1,- 1,- 1,192,
- 1,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,
192,192,192,192,192,192,192,193,193,193,193,193,193,193,193,193,193,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,
193,193,193,193,193,193,193,193,- 1,- 1,- 1,- 1,193,- 1,193,193,193,193,193,193,193,
193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,205,
205,205,205,205,205,205,205,205,205,214,214,214,214,214,214,214,214,214,214,- 1,
205,205,- 1,- 1,- 1,- 1,206,205,206,206,206,206,206,206,206,206,206,206,211,211,211,
211,211,211,211,211,211,211,- 1,206,- 1,- 1,205,205,- 1,- 1,206,- 1,- 1,205,211,- 1,- 1,-
1,- 1,206,211,- 1,212,212,212,212,212,212,212,212,212,212,- 1,- 1,- 1,206,- 1,- 1,- 1,- 1,
- 1,- 1,206,212,212,- 1,211,- 1,- 1,- 1,212,206,211,- 1,- 1,215,215,215,215,215,215,215,
215,215,215,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,212,212,215,- 1,- 1,- 1,- 1,212,215,216,- 1,
216,216,216,216,216,216,216,216,216,216,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,216,- 1,
215,- 1,- 1,- 1,- 1,216,215,- 1,- 1,- 1,- 1,- 1,- 1,217,216,217,217,217,217,217,217,217,
217,217,217,- 1,- 1,- 1,- 1,- 1,216,- 1,- 1,- 1,- 1,- 1,217,216,- 1,- 1,- 1,- 1,- 1,217,- 1,- 1,
216,- 1,- 1,- 1,- 1,- 1,217,- 1,- 1,234,234,234,234,234,234,234,234,234,234,- 1,- 1,- 1,
217,- 1,- 1,- 1,- 1,- 1,- 1,217,234,234,- 1,- 1,- 1,- 1,- 1,234,217,220,220,220,220,220,220,
220,220,220,220,- 1,- 1,- 1,- 1,- 1,- 1,- 1,220,220,220,220,220,220,234,234,- 1,- 1,- 1,- 1,
- 1,234,- 1,- 1,- 1,- 1,- 1,- 1,- 1,221,221,221,221,221,221,221,221,221,221,- 1,220,220,
220,220,220,220,221,221,221,221,221,221,- 1,- 1,- 1,241,- 1,221,- 1,- 1,241,- 1,- 1,- 1,-
1,- 1,221,- 1,- 1,241,241,241,241,241,241,241,241,- 1,221,221,221,221,221,221,241,- 1,
- 1,- 1,- 1,221,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,221,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,241,- 1,- 1,- 1,- 1,241,241,- 1,- 1,- 1,241,- 1,- 1,- 1,- 1,- 1,- 1,- 1,241,- 1,- 1,- 1,241,- 1,
241,- 1,241,- 1,241,254,254,254,254,254,254,254,254,254,254,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,
254,254,254,254,254,254,- 1,- 1,- 1,- 1,- 1,- 1,254,254,254,254,254,254,254,254,254,
254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,255,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,255,255,255,255,255,255,255,255,255,255,- 1,- 1,- 1,- 1,- 1,- 1,- 1,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,- 1,- 1,- 1,- 1,- 1,- 1,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};int Cyc_Lex_lex_engine(
int start_state,struct Cyc_Lexing_lexbuf*lbuf){int state;int base;int backtrk;int c;
state=start_state;if(state >= 0){lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);
lbuf->lex_last_action=- 1;}else{state=(- state)- 1;}while(1){base=Cyc_Lex_lex_base[
_check_known_subscript_notnull(279,state)];if(base < 0)return(- base)- 1;backtrk=
Cyc_Lex_lex_backtrk[_check_known_subscript_notnull(279,state)];if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}if(lbuf->lex_curr_pos
>= lbuf->lex_buffer_len){if(!lbuf->lex_eof_reached)return(- state)- 1;else{c=256;}}
else{c=(int)*((char*)_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),
lbuf->lex_curr_pos ++));if(c == - 1)c=256;}if(Cyc_Lex_lex_check[
_check_known_subscript_notnull(3783,base + c)]== state)state=Cyc_Lex_lex_trans[
_check_known_subscript_notnull(3783,base + c)];else{state=Cyc_Lex_lex_default[
_check_known_subscript_notnull(279,state)];}if(state < 0){lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1){struct Cyc_Lexing_Error_exn_struct _tmp195;const
char*_tmp194;struct Cyc_Lexing_Error_exn_struct*_tmp193;(int)_throw((void*)((
_tmp193=_cycalloc(sizeof(*_tmp193)),((_tmp193[0]=((_tmp195.tag=Cyc_Lexing_Error,((
_tmp195.f1=((_tmp194="empty token",_tag_dyneither(_tmp194,sizeof(char),12))),
_tmp195)))),_tmp193)))));}else{return lbuf->lex_last_action;}}else{if(c == 256)
lbuf->lex_eof_reached=0;}}}int Cyc_Lex_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,
int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);switch(lexstate){case 0:
_LL38: return Cyc_Lex_snarf_asm(lexbuf);case 1: _LL39: return Cyc_Lex_snarf_asm(lexbuf);
case 2: _LL3A: return(int)Cyc_Lex_process_id(lexbuf);case 3: _LL3B: return(int)Cyc_Lex_process_id(
lexbuf);case 4: _LL3C: return(int)Cyc_Lex_process_qual_id(lexbuf);case 5: _LL3D: Cyc_Lex_token_string=*
Cyc_Lex_get_symbol(Cyc_Lex_str_index_lbuf(lexbuf));return 368;case 6: _LL3E: return
Cyc_Lex_cyc_qual(lexbuf);case 7: _LL3F: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,2,
0,16);return 362;case 8: _LL40: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,8);
return 362;case 9: _LL41: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);return
362;case 10: _LL42: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);return 362;
case 11: _LL43: Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);
return 367;case 12: _LL44: Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf);return 367;case 13: _LL45: return 344;case 14: _LL46: return 345;case 15: _LL47:
return 342;case 16: _LL48: return 343;case 17: _LL49: return 338;case 18: _LL4A: return 339;
case 19: _LL4B: return 351;case 20: _LL4C: return 352;case 21: _LL4D: return 348;case 22:
_LL4E: return 349;case 23: _LL4F: return 350;case 24: _LL50: return 357;case 25: _LL51:
return 356;case 26: _LL52: return 355;case 27: _LL53: return 353;case 28: _LL54: return 354;
case 29: _LL55: return 346;case 30: _LL56: return 347;case 31: _LL57: return 340;case 32:
_LL58: return 341;case 33: _LL59: return 359;case 34: _LL5A: return 337;case 35: _LL5B:
return 358;case 36: _LL5C: return 360;case 37: _LL5D: return 311;case 38: _LL5E: return Cyc_Lex_token(
lexbuf);case 39: _LL5F: return Cyc_Lex_token(lexbuf);case 40: _LL60: return Cyc_Lex_token(
lexbuf);case 41: _LL61: Cyc_Lex_comment_depth=1;Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(
lexbuf);Cyc_Lex_comment(lexbuf);return Cyc_Lex_token(lexbuf);case 42: _LL62: Cyc_Lex_string_pos=
0;Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);while(Cyc_Lex_strng(
lexbuf)){;}Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 363;case 43: _LL63: Cyc_Lex_string_pos=0;Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(
lexbuf);while(Cyc_Lex_wstrng(lexbuf)){;}Cyc_Lex_token_string=(struct
_dyneither_ptr)Cyc_Lex_get_stored_string();return 364;case 44: _LL64: Cyc_Lex_string_pos=
0;Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);while(Cyc_Lex_scan_charconst(
lexbuf)){;}Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 366;case 45: _LL65: Cyc_Lex_token_char='\a';return 365;case 46: _LL66: Cyc_Lex_token_char='\b';
return 365;case 47: _LL67: Cyc_Lex_token_char='\f';return 365;case 48: _LL68: Cyc_Lex_token_char='\n';
return 365;case 49: _LL69: Cyc_Lex_token_char='\r';return 365;case 50: _LL6A: Cyc_Lex_token_char='\t';
return 365;case 51: _LL6B: Cyc_Lex_token_char='\v';return 365;case 52: _LL6C: Cyc_Lex_token_char='\\';
return 365;case 53: _LL6D: Cyc_Lex_token_char='\'';return 365;case 54: _LL6E: Cyc_Lex_token_char='"';
return 365;case 55: _LL6F: Cyc_Lex_token_char='?';return 365;case 56: _LL70: Cyc_Lex_token_char=(
char)(Cyc_Lex_intconst(lexbuf,2,1,8)).f2;return 365;case 57: _LL71: Cyc_Lex_token_char=(
char)(Cyc_Lex_intconst(lexbuf,3,1,16)).f2;return 365;case 58: _LL72: Cyc_Lex_token_char=
Cyc_Lexing_lexeme_char(lexbuf,1);return 365;case 59: _LL73: return - 1;case 60: _LL74:
return(int)Cyc_Lexing_lexeme_char(lexbuf,0);default: _LL75:(lexbuf->refill_buff)(
lexbuf);return Cyc_Lex_token_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_exn_struct
_tmp19B;const char*_tmp19A;struct Cyc_Lexing_Error_exn_struct*_tmp199;(int)_throw((
void*)((_tmp199=_cycalloc(sizeof(*_tmp199)),((_tmp199[0]=((_tmp19B.tag=Cyc_Lexing_Error,((
_tmp19B.f1=((_tmp19A="some action didn't return!",_tag_dyneither(_tmp19A,sizeof(
char),27))),_tmp19B)))),_tmp199)))));};}int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_Lex_token_rec(lexbuf,0);}int Cyc_Lex_cyc_qual_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL77:{const char*_tmp19C;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp19C="taggedZ",
_tag_dyneither(_tmp19C,sizeof(char),8))));}return(int)'@';case 1: _LL78:{const char*
_tmp19D;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp19D="zerotermZ",_tag_dyneither(
_tmp19D,sizeof(char),10))));}return(int)'@';case 2: _LL79:{const char*_tmp19E;
lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp19E="nozerotermZ",_tag_dyneither(_tmp19E,
sizeof(char),12))));}return(int)'@';case 3: _LL7A:{const char*_tmp19F;lexbuf->lex_curr_pos
-=Cyc_strlen(((_tmp19F="extensibleZ",_tag_dyneither(_tmp19F,sizeof(char),12))));}
return(int)'@';case 4: _LL7B:{const char*_tmp1A0;lexbuf->lex_curr_pos -=Cyc_strlen(((
_tmp1A0="resetableZ",_tag_dyneither(_tmp1A0,sizeof(char),11))));}return(int)'@';
case 5: _LL7C:{const char*_tmp1A1;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1A1="regionZ",
_tag_dyneither(_tmp1A1,sizeof(char),8))));}return(int)'@';case 6: _LL7D:{const char*
_tmp1A2;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1A2="numeltsZ",_tag_dyneither(
_tmp1A2,sizeof(char),9))));}return(int)'@';case 7: _LL7E:{const char*_tmp1A3;lexbuf->lex_curr_pos
-=Cyc_strlen(((_tmp1A3="thinZ",_tag_dyneither(_tmp1A3,sizeof(char),6))));}return(
int)'@';case 8: _LL7F:{const char*_tmp1A4;lexbuf->lex_curr_pos -=Cyc_strlen(((
_tmp1A4="fatZ",_tag_dyneither(_tmp1A4,sizeof(char),5))));}return(int)'@';case 9:
_LL80:{const char*_tmp1A5;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1A5="notnullZ",
_tag_dyneither(_tmp1A5,sizeof(char),9))));}return(int)'@';case 10: _LL81:{const
char*_tmp1A6;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1A6="nullableZ",
_tag_dyneither(_tmp1A6,sizeof(char),10))));}return(int)'@';case 11: _LL82: return
334;case 12: _LL83: return 319;case 13: _LL84: return 318;case 14: _LL85: return 335;case 15:
_LL86: return 336;case 16: _LL87: return 320;case 17: _LL88: return 329;case 18: _LL89:
return 330;case 19: _LL8A: return 331;case 20: _LL8B: return 332;case 21: _LL8C: return 333;
case 22: _LL8D: lexbuf->lex_curr_pos -=1;return(int)'@';default: _LL8E:(lexbuf->refill_buff)(
lexbuf);return Cyc_Lex_cyc_qual_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_exn_struct
_tmp1AC;const char*_tmp1AB;struct Cyc_Lexing_Error_exn_struct*_tmp1AA;(int)_throw((
void*)((_tmp1AA=_cycalloc(sizeof(*_tmp1AA)),((_tmp1AA[0]=((_tmp1AC.tag=Cyc_Lexing_Error,((
_tmp1AC.f1=((_tmp1AB="some action didn't return!",_tag_dyneither(_tmp1AB,sizeof(
char),27))),_tmp1AC)))),_tmp1AA)))));};}int Cyc_Lex_cyc_qual(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_Lex_cyc_qual_rec(lexbuf,1);}int Cyc_Lex_scan_charconst_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL90: return 0;case 1: _LL91: Cyc_Lex_store_string_char('\a');
return 1;case 2: _LL92: Cyc_Lex_store_string_char('\b');return 1;case 3: _LL93: Cyc_Lex_store_string_char('\f');
return 1;case 4: _LL94: Cyc_Lex_store_string_char('\n');return 1;case 5: _LL95: Cyc_Lex_store_string_char('\r');
return 1;case 6: _LL96: Cyc_Lex_store_string_char('\t');return 1;case 7: _LL97: Cyc_Lex_store_string_char('\v');
return 1;case 8: _LL98: Cyc_Lex_store_string_char('\\');return 1;case 9: _LL99: Cyc_Lex_store_string_char('\'');
return 1;case 10: _LL9A: Cyc_Lex_store_string_char('"');return 1;case 11: _LL9B: Cyc_Lex_store_string_char('?');
return 1;case 12: _LL9C: Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 13: _LL9D: Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 14: _LL9E: Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(
lexbuf,0));return 1;case 15: _LL9F:{const char*_tmp1AD;Cyc_Lex_runaway_err(((_tmp1AD="wide character ends in newline",
_tag_dyneither(_tmp1AD,sizeof(char),31))),lexbuf);}return 0;case 16: _LLA0:{const
char*_tmp1AE;Cyc_Lex_runaway_err(((_tmp1AE="unterminated wide character",
_tag_dyneither(_tmp1AE,sizeof(char),28))),lexbuf);}return 0;case 17: _LLA1:{const
char*_tmp1AF;Cyc_Lex_err(((_tmp1AF="bad character following backslash in wide character",
_tag_dyneither(_tmp1AF,sizeof(char),52))),lexbuf);}return 1;default: _LLA2:(lexbuf->refill_buff)(
lexbuf);return Cyc_Lex_scan_charconst_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_exn_struct
_tmp1B5;const char*_tmp1B4;struct Cyc_Lexing_Error_exn_struct*_tmp1B3;(int)_throw((
void*)((_tmp1B3=_cycalloc(sizeof(*_tmp1B3)),((_tmp1B3[0]=((_tmp1B5.tag=Cyc_Lexing_Error,((
_tmp1B5.f1=((_tmp1B4="some action didn't return!",_tag_dyneither(_tmp1B4,sizeof(
char),27))),_tmp1B5)))),_tmp1B3)))));};}int Cyc_Lex_scan_charconst(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_Lex_scan_charconst_rec(lexbuf,2);}int Cyc_Lex_strng_rec(struct
Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LLA4: return 1;case 1: _LLA5: return 0;case 2: _LLA6:
return 1;case 3: _LLA7: Cyc_Lex_store_string_char('\a');return 1;case 4: _LLA8: Cyc_Lex_store_string_char('\b');
return 1;case 5: _LLA9: Cyc_Lex_store_string_char('\f');return 1;case 6: _LLAA: Cyc_Lex_store_string_char('\n');
return 1;case 7: _LLAB: Cyc_Lex_store_string_char('\r');return 1;case 8: _LLAC: Cyc_Lex_store_string_char('\t');
return 1;case 9: _LLAD: Cyc_Lex_store_string_char('\v');return 1;case 10: _LLAE: Cyc_Lex_store_string_char('\\');
return 1;case 11: _LLAF: Cyc_Lex_store_string_char('\'');return 1;case 12: _LLB0: Cyc_Lex_store_string_char('"');
return 1;case 13: _LLB1: Cyc_Lex_store_string_char('?');return 1;case 14: _LLB2: Cyc_Lex_store_string_char((
char)(Cyc_Lex_intconst(lexbuf,1,0,8)).f2);return 1;case 15: _LLB3: Cyc_Lex_store_string_char((
char)(Cyc_Lex_intconst(lexbuf,2,0,16)).f2);return 1;case 16: _LLB4: Cyc_Lex_store_string_char(
Cyc_Lexing_lexeme_char(lexbuf,0));return 1;case 17: _LLB5:{const char*_tmp1B6;Cyc_Lex_runaway_err(((
_tmp1B6="string ends in newline",_tag_dyneither(_tmp1B6,sizeof(char),23))),
lexbuf);}return 0;case 18: _LLB6:{const char*_tmp1B7;Cyc_Lex_runaway_err(((_tmp1B7="unterminated string",
_tag_dyneither(_tmp1B7,sizeof(char),20))),lexbuf);}return 0;case 19: _LLB7:{const
char*_tmp1B8;Cyc_Lex_err(((_tmp1B8="bad character following backslash in string",
_tag_dyneither(_tmp1B8,sizeof(char),44))),lexbuf);}return 1;default: _LLB8:(lexbuf->refill_buff)(
lexbuf);return Cyc_Lex_strng_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_exn_struct
_tmp1BE;const char*_tmp1BD;struct Cyc_Lexing_Error_exn_struct*_tmp1BC;(int)_throw((
void*)((_tmp1BC=_cycalloc(sizeof(*_tmp1BC)),((_tmp1BC[0]=((_tmp1BE.tag=Cyc_Lexing_Error,((
_tmp1BE.f1=((_tmp1BD="some action didn't return!",_tag_dyneither(_tmp1BD,sizeof(
char),27))),_tmp1BE)))),_tmp1BC)))));};}int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_Lex_strng_rec(lexbuf,3);}int Cyc_Lex_wstrng_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LLBA:{const char*_tmp1BF;Cyc_Lex_store_string(((_tmp1BF="\" L\"",
_tag_dyneither(_tmp1BF,sizeof(char),5))));}return 1;case 1: _LLBB: return 0;case 2:
_LLBC:{const char*_tmp1C0;Cyc_Lex_store_string(((_tmp1C0="\\a",_tag_dyneither(
_tmp1C0,sizeof(char),3))));}return 1;case 3: _LLBD:{const char*_tmp1C1;Cyc_Lex_store_string(((
_tmp1C1="\\b",_tag_dyneither(_tmp1C1,sizeof(char),3))));}return 1;case 4: _LLBE:{
const char*_tmp1C2;Cyc_Lex_store_string(((_tmp1C2="\\f",_tag_dyneither(_tmp1C2,
sizeof(char),3))));}return 1;case 5: _LLBF:{const char*_tmp1C3;Cyc_Lex_store_string(((
_tmp1C3="\\n",_tag_dyneither(_tmp1C3,sizeof(char),3))));}return 1;case 6: _LLC0:{
const char*_tmp1C4;Cyc_Lex_store_string(((_tmp1C4="\\r",_tag_dyneither(_tmp1C4,
sizeof(char),3))));}return 1;case 7: _LLC1:{const char*_tmp1C5;Cyc_Lex_store_string(((
_tmp1C5="\\t",_tag_dyneither(_tmp1C5,sizeof(char),3))));}return 1;case 8: _LLC2:{
const char*_tmp1C6;Cyc_Lex_store_string(((_tmp1C6="\\v",_tag_dyneither(_tmp1C6,
sizeof(char),3))));}return 1;case 9: _LLC3:{const char*_tmp1C7;Cyc_Lex_store_string(((
_tmp1C7="\\\\",_tag_dyneither(_tmp1C7,sizeof(char),3))));}return 1;case 10: _LLC4:{
const char*_tmp1C8;Cyc_Lex_store_string(((_tmp1C8="\\'",_tag_dyneither(_tmp1C8,
sizeof(char),3))));}return 1;case 11: _LLC5:{const char*_tmp1C9;Cyc_Lex_store_string(((
_tmp1C9="\\\"",_tag_dyneither(_tmp1C9,sizeof(char),3))));}return 1;case 12: _LLC6:{
const char*_tmp1CA;Cyc_Lex_store_string(((_tmp1CA="\\?",_tag_dyneither(_tmp1CA,
sizeof(char),3))));}return 1;case 13: _LLC7: Cyc_Lex_store_string((struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 14: _LLC8: Cyc_Lex_store_string((
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 15: _LLC9: Cyc_Lex_store_string_char(
Cyc_Lexing_lexeme_char(lexbuf,0));return 1;case 16: _LLCA:{const char*_tmp1CB;Cyc_Lex_runaway_err(((
_tmp1CB="string ends in newline",_tag_dyneither(_tmp1CB,sizeof(char),23))),
lexbuf);}return 0;case 17: _LLCB:{const char*_tmp1CC;Cyc_Lex_runaway_err(((_tmp1CC="unterminated string",
_tag_dyneither(_tmp1CC,sizeof(char),20))),lexbuf);}return 0;case 18: _LLCC:{const
char*_tmp1CD;Cyc_Lex_err(((_tmp1CD="bad character following backslash in string",
_tag_dyneither(_tmp1CD,sizeof(char),44))),lexbuf);}return 1;default: _LLCD:(lexbuf->refill_buff)(
lexbuf);return Cyc_Lex_wstrng_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_exn_struct
_tmp1D3;const char*_tmp1D2;struct Cyc_Lexing_Error_exn_struct*_tmp1D1;(int)_throw((
void*)((_tmp1D1=_cycalloc(sizeof(*_tmp1D1)),((_tmp1D1[0]=((_tmp1D3.tag=Cyc_Lexing_Error,((
_tmp1D3.f1=((_tmp1D2="some action didn't return!",_tag_dyneither(_tmp1D2,sizeof(
char),27))),_tmp1D3)))),_tmp1D1)))));};}int Cyc_Lex_wstrng(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_Lex_wstrng_rec(lexbuf,4);}int Cyc_Lex_comment_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LLCF: ++ Cyc_Lex_comment_depth;return Cyc_Lex_comment(lexbuf);case 1: _LLD0: --
Cyc_Lex_comment_depth;if(Cyc_Lex_comment_depth > 0)return Cyc_Lex_comment(lexbuf);
return 0;case 2: _LLD1:{const char*_tmp1D4;Cyc_Lex_runaway_err(((_tmp1D4="unterminated comment",
_tag_dyneither(_tmp1D4,sizeof(char),21))),lexbuf);}return 0;case 3: _LLD2: return Cyc_Lex_comment(
lexbuf);case 4: _LLD3: return Cyc_Lex_comment(lexbuf);case 5: _LLD4: return Cyc_Lex_comment(
lexbuf);default: _LLD5:(lexbuf->refill_buff)(lexbuf);return Cyc_Lex_comment_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_exn_struct _tmp1DA;const char*_tmp1D9;
struct Cyc_Lexing_Error_exn_struct*_tmp1D8;(int)_throw((void*)((_tmp1D8=_cycalloc(
sizeof(*_tmp1D8)),((_tmp1D8[0]=((_tmp1DA.tag=Cyc_Lexing_Error,((_tmp1DA.f1=((
_tmp1D9="some action didn't return!",_tag_dyneither(_tmp1D9,sizeof(char),27))),
_tmp1DA)))),_tmp1D8)))));};}int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_Lex_comment_rec(lexbuf,5);}int Cyc_Lex_snarf_asm_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LLD7: Cyc_Lex_token_asm.f1=0;Cyc_Lex_paren_depth=1;Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);while(Cyc_Lex_snarf_asm_body(
lexbuf)){;}Cyc_Lex_token_asm.f2=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 373;case 1: _LLD8: Cyc_Lex_token_asm.f1=1;Cyc_Lex_paren_depth=1;Cyc_Lex_string_pos=
0;Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);while(Cyc_Lex_snarf_asm_body(
lexbuf)){;}Cyc_Lex_token_asm.f2=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 373;default: _LLD9:(lexbuf->refill_buff)(lexbuf);return Cyc_Lex_snarf_asm_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_exn_struct _tmp1E0;const char*_tmp1DF;
struct Cyc_Lexing_Error_exn_struct*_tmp1DE;(int)_throw((void*)((_tmp1DE=_cycalloc(
sizeof(*_tmp1DE)),((_tmp1DE[0]=((_tmp1E0.tag=Cyc_Lexing_Error,((_tmp1E0.f1=((
_tmp1DF="some action didn't return!",_tag_dyneither(_tmp1DF,sizeof(char),27))),
_tmp1E0)))),_tmp1DE)))));};}int Cyc_Lex_snarf_asm(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_Lex_snarf_asm_rec(lexbuf,6);}int Cyc_Lex_snarf_asm_body_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LLDB: Cyc_Lex_store_string_char('"');while(Cyc_Lex_strng(lexbuf)){;}Cyc_Lex_store_string_char('"');
return 1;case 1: _LLDC: Cyc_Lex_comment_depth=1;Cyc_Lex_comment(lexbuf);Cyc_Lex_store_string_char(' ');
return 1;case 2: _LLDD: Cyc_Lex_store_string_char('\n');return 1;case 3: _LLDE: Cyc_Lex_paren_depth
+=1;Cyc_Lex_store_string_char('(');return 1;case 4: _LLDF: Cyc_Lex_paren_depth -=1;
if(Cyc_Lex_paren_depth == 0)return 0;else{Cyc_Lex_store_string_char(')');return 1;}
case 5: _LLE0:{const char*_tmp1E1;Cyc_Lex_runaway_err(((_tmp1E1="unterminated asm",
_tag_dyneither(_tmp1E1,sizeof(char),17))),lexbuf);}return 0;case 6: _LLE1: Cyc_Lex_store_string_char(
Cyc_Lexing_lexeme_char(lexbuf,0));return 1;default: _LLE2:(lexbuf->refill_buff)(
lexbuf);return Cyc_Lex_snarf_asm_body_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_exn_struct
_tmp1E7;const char*_tmp1E6;struct Cyc_Lexing_Error_exn_struct*_tmp1E5;(int)_throw((
void*)((_tmp1E5=_cycalloc(sizeof(*_tmp1E5)),((_tmp1E5[0]=((_tmp1E7.tag=Cyc_Lexing_Error,((
_tmp1E7.f1=((_tmp1E6="some action didn't return!",_tag_dyneither(_tmp1E6,sizeof(
char),27))),_tmp1E7)))),_tmp1E5)))));};}int Cyc_Lex_snarf_asm_body(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_Lex_snarf_asm_body_rec(lexbuf,7);}void Cyc_Lex_lex_init(int
include_cyclone_keywords);static void _tmp1F6(unsigned int*_tmp1F5,unsigned int*
_tmp1F4,int**_tmp1F2){for(*_tmp1F5=0;*_tmp1F5 < *_tmp1F4;(*_tmp1F5)++){(*_tmp1F2)[*
_tmp1F5]=0;}}void Cyc_Lex_lex_init(int include_cyclone_keywords){if(Cyc_Lex_ids_trie
!= 0){struct _DynRegionHandle*_tmp11C;struct Cyc_Lex_DynTrieSymbols _tmp11B=*((
struct Cyc_Lex_DynTrieSymbols*)_check_null(Cyc_Lex_ids_trie));_tmp11C=_tmp11B.dyn;
Cyc_Core_free_dynregion(_tmp11C);}if(Cyc_Lex_typedefs_trie != 0){struct
_DynRegionHandle*_tmp11E;struct Cyc_Lex_DynTrie _tmp11D=*((struct Cyc_Lex_DynTrie*)
_check_null(Cyc_Lex_typedefs_trie));_tmp11E=_tmp11D.dyn;Cyc_Core_free_dynregion(
_tmp11E);}{struct _DynRegionHandle*_tmp120;struct Cyc_Core_NewRegion _tmp11F=Cyc_Core__new_dynregion((
const char*)"internal-error",0);_tmp120=_tmp11F.dynregion;{struct _DynRegionFrame
_tmp121;struct _RegionHandle*id_rgn=_open_dynregion(& _tmp121,_tmp120);{const char*
_tmp1EA;struct _dyneither_ptr*_tmp1E9;struct Cyc_Xarray_Xarray*symbols=((struct Cyc_Xarray_Xarray*(*)(
struct _RegionHandle*,int,struct _dyneither_ptr*))Cyc_Xarray_rcreate)(id_rgn,101,((
_tmp1E9=_cycalloc(sizeof(*_tmp1E9)),((_tmp1E9[0]=(struct _dyneither_ptr)((_tmp1EA="",
_tag_dyneither(_tmp1EA,sizeof(char),1))),_tmp1E9)))));((void(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add)(symbols,& Cyc_Lex_bogus_string);{struct Cyc_Lex_Trie*
_tmp1ED;struct Cyc_Lex_DynTrieSymbols*_tmp1EC;Cyc_Lex_ids_trie=((_tmp1EC=
_cycalloc(sizeof(*_tmp1EC)),((_tmp1EC->dyn=(struct _DynRegionHandle*)_tmp120,((
_tmp1EC->t=(struct Cyc_Lex_Trie*)((_tmp1ED=_region_malloc(id_rgn,sizeof(*_tmp1ED)),((
_tmp1ED->children=Cyc_Lex_Zero(),((_tmp1ED->shared_str=0,_tmp1ED)))))),((_tmp1EC->symbols=(
struct Cyc_Xarray_Xarray*)symbols,(struct Cyc_Lex_DynTrieSymbols*)_tmp1EC))))))));};}{
struct _DynRegionHandle*_tmp127;struct Cyc_Core_NewRegion _tmp126=Cyc_Core__new_dynregion((
const char*)"internal-error",0);_tmp127=_tmp126.dynregion;{struct _DynRegionFrame
_tmp128;struct _RegionHandle*typedefs_rgn=_open_dynregion(& _tmp128,_tmp127);{
struct Cyc_Lex_Trie*_tmp1F0;struct Cyc_Lex_DynTrie*_tmp1EF;Cyc_Lex_typedefs_trie=((
_tmp1EF=_cycalloc(sizeof(*_tmp1EF)),((_tmp1EF->dyn=(struct _DynRegionHandle*)
_tmp127,((_tmp1EF->t=(struct Cyc_Lex_Trie*)((_tmp1F0=_region_malloc(typedefs_rgn,
sizeof(*_tmp1F0)),((_tmp1F0->children=Cyc_Lex_Zero(),((_tmp1F0->shared_str=0,
_tmp1F0)))))),(struct Cyc_Lex_DynTrie*)_tmp1EF))))));}Cyc_Lex_num_kws=Cyc_Lex_num_keywords(
include_cyclone_keywords);{unsigned int _tmp1F5;unsigned int _tmp1F4;struct
_dyneither_ptr _tmp1F3;int*_tmp1F2;unsigned int _tmp1F1;Cyc_Lex_kw_nums=((_tmp1F1=(
unsigned int)Cyc_Lex_num_kws,((_tmp1F2=(int*)_cycalloc_atomic(_check_times(
sizeof(int),_tmp1F1)),((_tmp1F3=_tag_dyneither(_tmp1F2,sizeof(int),_tmp1F1),((((
_tmp1F4=_tmp1F1,_tmp1F6(& _tmp1F5,& _tmp1F4,& _tmp1F2))),_tmp1F3))))))));}{
unsigned int i=0;unsigned int rwsze=71;{unsigned int j=0;for(0;j < rwsze;++ j){if(
include_cyclone_keywords  || (Cyc_Lex_rw_array[(int)j]).f3){struct _dyneither_ptr
_tmp12F=(Cyc_Lex_rw_array[(int)j]).f1;Cyc_Lex_str_index(_tmp12F,0,(int)Cyc_strlen((
struct _dyneither_ptr)_tmp12F));*((int*)_check_dyneither_subscript(Cyc_Lex_kw_nums,
sizeof(int),(int)i))=(int)(Cyc_Lex_rw_array[(int)j]).f2;++ i;}}}Cyc_Lex_typedef_init();
Cyc_Lex_comment_depth=0;};;_pop_dynregion(typedefs_rgn);};};;_pop_dynregion(
id_rgn);};};}
