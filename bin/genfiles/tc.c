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
Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{
char*tag;struct _dyneither_ptr f1;};typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{
void*v;};typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[
17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{
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
char*tag;};void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};int Cyc_List_mem(
int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc_List_List*Cyc_List_filter_c(
int(*f)(void*,void*),void*env,struct Cyc_List_List*x);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr
logical_file;struct _dyneither_ptr line;int line_no;int col;};typedef struct Cyc_Lineno_Pos*
Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{
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
f1;};extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;
struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual
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
char*tag;};int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual(unsigned int);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
unsigned int);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
void*Cyc_Absyn_string_typ(void*rgn);void*Cyc_Absyn_const_string_typ(void*rgn);
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);extern struct
_tuple0*Cyc_Absyn_uniquergn_qvar;struct _dyneither_ptr Cyc_Absyn_attribute2string(
void*);struct Cyc_PP_Ppstate;typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct
Cyc_PP_Out;typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;typedef struct
Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int
qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int
print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int
print_externC_stmts;int print_full_evars;int print_zeroterm;int
generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern
struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;struct _dyneither_ptr Cyc_Absynpp_typ2string(
void*);struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct
_dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Iter_Iter{void*
env;int(*next)(void*env,void*dest);};typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;typedef struct Cyc_Set_Set*
Cyc_Set_set_t;struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(
void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*
s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[
7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;typedef const
struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;const struct Cyc_Dict_T*t;};typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;
extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};int Cyc_Dict_is_empty(
struct Cyc_Dict_Dict d);int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict d);struct _tuple10{void*f1;void*f2;};struct _tuple10*
Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict
Cyc_Dict_rfilter_c(struct _RegionHandle*,int(*f)(void*,void*,void*),void*env,
struct Cyc_Dict_Dict d);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*
rgn,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;typedef struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_rgn_po_t;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct
_RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct
Cyc_Absyn_Tvar*fst_rgn,unsigned int);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
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
Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct
_RegionHandle*);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,
unsigned int,struct Cyc_Absyn_Fndecl*);struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv*,unsigned int,struct _dyneither_ptr*,struct Cyc_List_List*);
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*
te);void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(unsigned int,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(unsigned int,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_is_function_type(void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*
k);void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);void Cyc_Tcutil_check_valid_toplevel_type(
unsigned int,struct Cyc_Tcenv_Tenv*,void*);void Cyc_Tcutil_check_fndecl_valid_type(
unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(
unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*
k,int allow_evars,void*);void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_supports_default(
void*);int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,
void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*
atts);void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);void*Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{
struct _tuple10*eq;int isTrue;};typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr msg_part);void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Stmt*s,int new_block);struct _tuple11{unsigned int f1;int f2;};
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);void Cyc_Tc_tc(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*
ds);struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,
struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Enumdecl*);
extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_exn_struct{
char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;
struct Cyc_Absyn_Datatypefield*field;};typedef struct Cyc_Tcdecl_Xdatatypefielddecl*
Cyc_Tcdecl_xdatatypefielddecl_t;struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(
struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct
_dyneither_ptr*msg);struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(
struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,
struct _dyneither_ptr*msg);struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct
_dyneither_ptr*msg);void*Cyc_Tcdecl_merge_binding(void*d0,void*d1,unsigned int
loc,struct _dyneither_ptr*msg);struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(
struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct
_dyneither_ptr*msg);static char _tmp0[1]="";static struct _dyneither_ptr Cyc_Tc_tc_msg_c={
_tmp0,_tmp0,_tmp0 + 1};static struct _dyneither_ptr*Cyc_Tc_tc_msg=(struct
_dyneither_ptr*)& Cyc_Tc_tc_msg_c;struct _tuple12{unsigned int f1;struct _tuple0*f2;
int f3;};static int Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*
exports){for(0;exports != 0;exports=exports->tl){struct _tuple12*_tmp1=(struct
_tuple12*)exports->hd;if(Cyc_Absyn_qvar_cmp(x,(*_tmp1).f2)== 0){(*_tmp1).f3=1;
return 1;}}return 0;}struct _tuple13{void*f1;int f2;};static void Cyc_Tc_tcVardecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Vardecl*
vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){struct Cyc_Absyn_Vardecl
_tmp3;enum Cyc_Absyn_Scope _tmp4;struct _tuple0*_tmp5;struct _tuple0 _tmp6;union Cyc_Absyn_Nmspace
_tmp7;struct _dyneither_ptr*_tmp8;void*_tmp9;struct Cyc_Absyn_Exp*_tmpA;struct Cyc_List_List*
_tmpB;struct Cyc_Absyn_Vardecl*_tmp2=vd;_tmp3=*_tmp2;_tmp4=_tmp3.sc;_tmp5=_tmp3.name;
_tmp6=*_tmp5;_tmp7=_tmp6.f1;_tmp8=_tmp6.f2;_tmp9=_tmp3.type;_tmpA=_tmp3.initializer;
_tmpB=_tmp3.attributes;{union Cyc_Absyn_Nmspace _tmpC=_tmp7;struct Cyc_List_List*
_tmpD;struct Cyc_List_List*_tmpE;struct Cyc_List_List*_tmpF;_LL1: if((_tmpC.Rel_n).tag
!= 1)goto _LL3;_tmpD=(struct Cyc_List_List*)(_tmpC.Rel_n).val;if(_tmpD != 0)goto
_LL3;_LL2: goto _LL4;_LL3: if((_tmpC.C_n).tag != 3)goto _LL5;_tmpE=(struct Cyc_List_List*)(
_tmpC.C_n).val;if(_tmpE != 0)goto _LL5;_LL4: goto _LL6;_LL5: if((_tmpC.Abs_n).tag != 2)
goto _LL7;_tmpF=(struct Cyc_List_List*)(_tmpC.Abs_n).val;if(_tmpF != 0)goto _LL7;
_LL6: goto _LL0;_LL7:;_LL8:{const char*_tmp2B4;void*_tmp2B3[1];struct Cyc_String_pa_PrintArg_struct
_tmp2B2;(_tmp2B2.tag=0,((_tmp2B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name)),((_tmp2B3[0]=& _tmp2B2,Cyc_Tcutil_terr(loc,((
_tmp2B4="qualified variable declarations are not implemented (%s)",
_tag_dyneither(_tmp2B4,sizeof(char),57))),_tag_dyneither(_tmp2B3,sizeof(void*),1)))))));}
return;_LL0:;}(*vd->name).f1=Cyc_Absyn_Abs_n(te->ns,vd->sc == Cyc_Absyn_ExternC
 || in_cinclude);{void*_tmp13=Cyc_Tcutil_compress(_tmp9);struct Cyc_Absyn_ArrayInfo
_tmp15;void*_tmp16;struct Cyc_Absyn_Tqual _tmp17;struct Cyc_Absyn_Exp*_tmp18;union
Cyc_Absyn_Constraint*_tmp19;unsigned int _tmp1A;_LLA:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp14=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13;if(_tmp14->tag != 8)
goto _LLC;else{_tmp15=_tmp14->f1;_tmp16=_tmp15.elt_type;_tmp17=_tmp15.tq;_tmp18=
_tmp15.num_elts;if(_tmp18 != 0)goto _LLC;_tmp19=_tmp15.zero_term;_tmp1A=_tmp15.zt_loc;}}
if(!(_tmpA != 0))goto _LLC;_LLB:{void*_tmp1B=_tmpA->r;union Cyc_Absyn_Cnst _tmp1D;
struct _dyneither_ptr _tmp1E;union Cyc_Absyn_Cnst _tmp20;struct _dyneither_ptr _tmp21;
struct Cyc_Absyn_Exp*_tmp23;struct Cyc_List_List*_tmp25;struct Cyc_List_List*_tmp27;
_LLF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmp1B;if(_tmp1C->tag != 0)goto _LL11;else{_tmp1D=_tmp1C->f1;if((_tmp1D.String_c).tag
!= 8)goto _LL11;_tmp1E=(struct _dyneither_ptr)(_tmp1D.String_c).val;}}_LL10: _tmp9=(
vd->type=Cyc_Absyn_array_typ(_tmp16,_tmp17,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
_get_dyneither_size(_tmp1E,sizeof(char)),0),_tmp19,_tmp1A));goto _LLE;_LL11: {
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmp1B;if(_tmp1F->tag != 0)goto _LL13;else{_tmp20=_tmp1F->f1;if((_tmp20.Wstring_c).tag
!= 9)goto _LL13;_tmp21=(struct _dyneither_ptr)(_tmp20.Wstring_c).val;}}_LL12: _tmp9=(
vd->type=Cyc_Absyn_array_typ(_tmp16,_tmp17,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
1,0),_tmp19,_tmp1A));goto _LLE;_LL13: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*
_tmp22=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp1B;if(_tmp22->tag
!= 26)goto _LL15;else{_tmp23=_tmp22->f2;}}_LL14: _tmp9=(vd->type=Cyc_Absyn_array_typ(
_tmp16,_tmp17,(struct Cyc_Absyn_Exp*)_tmp23,_tmp19,_tmp1A));goto _LLE;_LL15: {
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp24=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)
_tmp1B;if(_tmp24->tag != 34)goto _LL17;else{_tmp25=_tmp24->f2;}}_LL16: _tmp27=
_tmp25;goto _LL18;_LL17: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp26=(
struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp1B;if(_tmp26->tag != 25)goto
_LL19;else{_tmp27=_tmp26->f1;}}_LL18: _tmp9=(vd->type=Cyc_Absyn_array_typ(_tmp16,
_tmp17,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp27),0),_tmp19,_tmp1A));goto _LLE;_LL19:;_LL1A: goto _LLE;
_LLE:;}goto _LL9;_LLC:;_LLD: goto _LL9;_LL9:;}Cyc_Tcutil_check_valid_toplevel_type(
loc,te,_tmp9);(vd->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,
_tmp9);{void*_tmp28=Cyc_Tcutil_compress(_tmp9);_LL1C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp29=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp28;if(_tmp29->tag != 8)
goto _LL1E;}_LL1D: vd->escapes=0;goto _LL1B;_LL1E:;_LL1F: vd->escapes=1;goto _LL1B;
_LL1B:;}if(Cyc_Tcutil_is_function_type(_tmp9))_tmpB=Cyc_Tcutil_transfer_fn_type_atts(
_tmp9,_tmpB);if(_tmp4 == Cyc_Absyn_Extern  || _tmp4 == Cyc_Absyn_ExternC){if(_tmpA
!= 0){const char*_tmp2B7;void*_tmp2B6;(_tmp2B6=0,Cyc_Tcutil_terr(loc,((_tmp2B7="extern declaration should not have initializer",
_tag_dyneither(_tmp2B7,sizeof(char),47))),_tag_dyneither(_tmp2B6,sizeof(void*),0)));}}
else{if(!Cyc_Tcutil_is_function_type(_tmp9)){for(0;_tmpB != 0;_tmpB=_tmpB->tl){
void*_tmp2C=(void*)_tmpB->hd;_LL21: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*
_tmp2D=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2C;if(_tmp2D->tag
!= 6)goto _LL23;}_LL22: goto _LL24;_LL23: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*
_tmp2E=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2C;if(_tmp2E->tag
!= 8)goto _LL25;}_LL24: goto _LL26;_LL25: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*
_tmp2F=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp2C;if(_tmp2F->tag
!= 9)goto _LL27;}_LL26: goto _LL28;_LL27: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*
_tmp30=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp2C;if(_tmp30->tag
!= 10)goto _LL29;}_LL28: goto _LL2A;_LL29: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*
_tmp31=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp2C;if(_tmp31->tag
!= 11)goto _LL2B;}_LL2A: goto _LL2C;_LL2B: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*
_tmp32=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp2C;if(_tmp32->tag != 
12)goto _LL2D;}_LL2C: goto _LL2E;_LL2D: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*
_tmp33=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp2C;if(_tmp33->tag
!= 13)goto _LL2F;}_LL2E: goto _LL30;_LL2F: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*
_tmp34=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp2C;if(_tmp34->tag
!= 14)goto _LL31;}_LL30: continue;_LL31:;_LL32:{const char*_tmp2BC;void*_tmp2BB[2];
struct Cyc_String_pa_PrintArg_struct _tmp2BA;struct Cyc_String_pa_PrintArg_struct
_tmp2B9;(_tmp2B9.tag=0,((_tmp2B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name)),((_tmp2BA.tag=0,((_tmp2BA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmpB->hd)),((
_tmp2BB[0]=& _tmp2BA,((_tmp2BB[1]=& _tmp2B9,Cyc_Tcutil_terr(loc,((_tmp2BC="bad attribute %s for variable %s",
_tag_dyneither(_tmp2BC,sizeof(char),33))),_tag_dyneither(_tmp2BB,sizeof(void*),2)))))))))))));}
goto _LL20;_LL20:;}if(_tmpA == 0  || in_cinclude){if(check_var_init  && !Cyc_Tcutil_supports_default(
_tmp9)){const char*_tmp2C1;void*_tmp2C0[2];struct Cyc_String_pa_PrintArg_struct
_tmp2BF;struct Cyc_String_pa_PrintArg_struct _tmp2BE;(_tmp2BE.tag=0,((_tmp2BE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp9)),((
_tmp2BF.tag=0,((_tmp2BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
vd->name)),((_tmp2C0[0]=& _tmp2BF,((_tmp2C0[1]=& _tmp2BE,Cyc_Tcutil_terr(loc,((
_tmp2C1="initializer required for variable %s of type %s",_tag_dyneither(_tmp2C1,
sizeof(char),48))),_tag_dyneither(_tmp2C0,sizeof(void*),2)))))))))))));}}else{
struct Cyc_Absyn_Exp*_tmp3D=(struct Cyc_Absyn_Exp*)_tmpA;void*_tmp3E=Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp9,_tmp3D);if(!Cyc_Tcutil_coerce_assign(te,_tmp3D,_tmp9)){{const
char*_tmp2C7;void*_tmp2C6[3];struct Cyc_String_pa_PrintArg_struct _tmp2C5;struct
Cyc_String_pa_PrintArg_struct _tmp2C4;struct Cyc_String_pa_PrintArg_struct _tmp2C3;(
_tmp2C3.tag=0,((_tmp2C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp3E)),((_tmp2C4.tag=0,((_tmp2C4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp9)),((_tmp2C5.tag=0,((_tmp2C5.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((
_tmp2C6[0]=& _tmp2C5,((_tmp2C6[1]=& _tmp2C4,((_tmp2C6[2]=& _tmp2C3,Cyc_Tcutil_terr(
loc,((_tmp2C7="%s declared with type \n%s\n but initialized with type \n%s",
_tag_dyneither(_tmp2C7,sizeof(char),57))),_tag_dyneither(_tmp2C6,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}if(!Cyc_Tcutil_is_const_exp(te,_tmp3D)){const char*
_tmp2CA;void*_tmp2C9;(_tmp2C9=0,Cyc_Tcutil_terr(loc,((_tmp2CA="initializer is not a constant expression",
_tag_dyneither(_tmp2CA,sizeof(char),41))),_tag_dyneither(_tmp2C9,sizeof(void*),0)));}}}
else{for(0;_tmpB != 0;_tmpB=_tmpB->tl){void*_tmp46=(void*)_tmpB->hd;_LL34: {
struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp47=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)
_tmp46;if(_tmp47->tag != 0)goto _LL36;}_LL35: goto _LL37;_LL36: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*
_tmp48=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp46;if(_tmp48->tag
!= 1)goto _LL38;}_LL37: goto _LL39;_LL38: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*
_tmp49=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp46;if(_tmp49->tag
!= 2)goto _LL3A;}_LL39: goto _LL3B;_LL3A: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*
_tmp4A=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp46;if(_tmp4A->tag
!= 3)goto _LL3C;}_LL3B: goto _LL3D;_LL3C: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*
_tmp4B=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp46;if(_tmp4B->tag
!= 4)goto _LL3E;}_LL3D: goto _LL3F;_LL3E: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*
_tmp4C=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp46;if(_tmp4C->tag
!= 19)goto _LL40;}_LL3F: goto _LL41;_LL40: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*
_tmp4D=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp46;if(_tmp4D->tag
!= 20)goto _LL42;}_LL41: goto _LL43;_LL42: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*
_tmp4E=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp46;if(_tmp4E->tag != 
22)goto _LL44;}_LL43: goto _LL45;_LL44: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*
_tmp4F=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp46;if(_tmp4F->tag
!= 5)goto _LL46;}_LL45: {const char*_tmp2CD;void*_tmp2CC;(_tmp2CC=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp2CD="tcVardecl: fn type atts in function var decl",
_tag_dyneither(_tmp2CD,sizeof(char),45))),_tag_dyneither(_tmp2CC,sizeof(void*),0)));}
_LL46: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp50=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)
_tmp46;if(_tmp50->tag != 6)goto _LL48;}_LL47: goto _LL49;_LL48: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*
_tmp51=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp46;if(_tmp51->tag
!= 7)goto _LL4A;}_LL49:{const char*_tmp2D1;void*_tmp2D0[1];struct Cyc_String_pa_PrintArg_struct
_tmp2CF;(_tmp2CF.tag=0,((_tmp2CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)_tmpB->hd)),((_tmp2D0[0]=& _tmp2CF,Cyc_Tcutil_terr(
loc,((_tmp2D1="bad attribute %s in function declaration",_tag_dyneither(_tmp2D1,
sizeof(char),41))),_tag_dyneither(_tmp2D0,sizeof(void*),1)))))));}goto _LL33;
_LL4A:;_LL4B: continue;_LL33:;}}}{struct _handler_cons _tmp57;_push_handler(& _tmp57);{
int _tmp59=0;if(setjmp(_tmp57.handler))_tmp59=1;if(!_tmp59){{struct _tuple13*
_tmp5A=((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
ge->ordinaries,_tmp8);void*_tmp5B=(*_tmp5A).f1;void*_tmp5D;_LL4D: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*
_tmp5C=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp5B;if(_tmp5C->tag != 0)
goto _LL4F;else{_tmp5D=(void*)_tmp5C->f1;}}_LL4E: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct
_tmp2D4;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2D3;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmp62=(_tmp2D3=_cycalloc(sizeof(*_tmp2D3)),((_tmp2D3[0]=((_tmp2D4.tag=1,((
_tmp2D4.f1=vd,_tmp2D4)))),_tmp2D3)));void*_tmp63=Cyc_Tcdecl_merge_binding(_tmp5D,(
void*)_tmp62,loc,Cyc_Tc_tc_msg);if(_tmp63 == (void*)& Cyc_Absyn_Unresolved_b_val){
_npop_handler(0);return;}if(exports == 0  || Cyc_Tc_export_member(vd->name,*
exports)){if(!(_tmp63 == _tmp5D  && (*_tmp5A).f2)){struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*
_tmp2DA;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp2D9;struct _tuple13*
_tmp2D8;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,_tmp8,(struct
_tuple13*)((_tmp2D8=_cycalloc(sizeof(*_tmp2D8)),((_tmp2D8->f1=(void*)((_tmp2DA=
_cycalloc(sizeof(*_tmp2DA)),((_tmp2DA[0]=((_tmp2D9.tag=0,((_tmp2D9.f1=(void*)
_tmp63,_tmp2D9)))),_tmp2DA)))),((_tmp2D8->f2=1,_tmp2D8)))))));}}_npop_handler(0);
return;}_LL4F: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp5E=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)
_tmp5B;if(_tmp5E->tag != 1)goto _LL51;}_LL50:{const char*_tmp2DD;void*_tmp2DC;(
_tmp2DC=0,Cyc_Tcutil_warn(loc,((_tmp2DD="variable declaration shadows previous struct declaration",
_tag_dyneither(_tmp2DD,sizeof(char),57))),_tag_dyneither(_tmp2DC,sizeof(void*),0)));}
goto _LL4C;_LL51: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp5F=(
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp5B;if(_tmp5F->tag != 2)
goto _LL53;}_LL52:{const char*_tmp2E0;void*_tmp2DF;(_tmp2DF=0,Cyc_Tcutil_warn(loc,((
_tmp2E0="variable declaration shadows previous datatype constructor",
_tag_dyneither(_tmp2E0,sizeof(char),59))),_tag_dyneither(_tmp2DF,sizeof(void*),0)));}
goto _LL4C;_LL53: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp60=(
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp5B;if(_tmp60->tag != 4)
goto _LL55;}_LL54: goto _LL56;_LL55: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*
_tmp61=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp5B;if(_tmp61->tag != 3)
goto _LL4C;}_LL56:{const char*_tmp2E3;void*_tmp2E2;(_tmp2E2=0,Cyc_Tcutil_warn(loc,((
_tmp2E3="variable declaration shadows previous enum tag",_tag_dyneither(_tmp2E3,
sizeof(char),47))),_tag_dyneither(_tmp2E2,sizeof(void*),0)));}goto _LL4C;_LL4C:;};
_pop_handler();}else{void*_tmp58=(void*)_exn_thrown;void*_tmp70=_tmp58;_LL58: {
struct Cyc_Dict_Absent_exn_struct*_tmp71=(struct Cyc_Dict_Absent_exn_struct*)
_tmp70;if(_tmp71->tag != Cyc_Dict_Absent)goto _LL5A;}_LL59: goto _LL57;_LL5A:;_LL5B:(
void)_throw(_tmp70);_LL57:;}};}if(exports == 0  || Cyc_Tc_export_member(vd->name,*
exports)){struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp2F2;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmp2F1;struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp2F0;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct
_tmp2EF;struct _tuple13*_tmp2EE;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,_tmp8,(
struct _tuple13*)((_tmp2EE=_cycalloc(sizeof(*_tmp2EE)),((_tmp2EE->f1=(void*)((
_tmp2F2=_cycalloc(sizeof(*_tmp2F2)),((_tmp2F2[0]=((_tmp2EF.tag=0,((_tmp2EF.f1=(
void*)((void*)((_tmp2F1=_cycalloc(sizeof(*_tmp2F1)),((_tmp2F1[0]=((_tmp2F0.tag=1,((
_tmp2F0.f1=vd,_tmp2F0)))),_tmp2F1))))),_tmp2EF)))),_tmp2F2)))),((_tmp2EE->f2=0,
_tmp2EE)))))));}}static int Cyc_Tc_is_main(struct _tuple0*n){struct _tuple0 _tmp78;
union Cyc_Absyn_Nmspace _tmp79;struct _dyneither_ptr*_tmp7A;struct _tuple0*_tmp77=n;
_tmp78=*_tmp77;_tmp79=_tmp78.f1;_tmp7A=_tmp78.f2;{union Cyc_Absyn_Nmspace _tmp7B=
_tmp79;struct Cyc_List_List*_tmp7C;_LL5D: if((_tmp7B.Abs_n).tag != 2)goto _LL5F;
_tmp7C=(struct Cyc_List_List*)(_tmp7B.Abs_n).val;if(_tmp7C != 0)goto _LL5F;_LL5E: {
const char*_tmp2F3;return Cyc_strcmp((struct _dyneither_ptr)*_tmp7A,((_tmp2F3="main",
_tag_dyneither(_tmp2F3,sizeof(char),5))))== 0;}_LL5F:;_LL60: return 0;_LL5C:;};}
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,
unsigned int loc,struct Cyc_Absyn_Fndecl*fd,int in_cinclude,struct Cyc_List_List**
exports){struct _dyneither_ptr*v=(*fd->name).f2;if(fd->sc == Cyc_Absyn_ExternC  && 
!in_cinclude){const char*_tmp2F6;void*_tmp2F5;(_tmp2F5=0,Cyc_Tcutil_terr(loc,((
_tmp2F6="extern \"C\" functions cannot be implemented in Cyclone",_tag_dyneither(
_tmp2F6,sizeof(char),54))),_tag_dyneither(_tmp2F5,sizeof(void*),0)));}{union Cyc_Absyn_Nmspace
_tmp80=(*fd->name).f1;struct Cyc_List_List*_tmp81;struct Cyc_List_List*_tmp82;
struct Cyc_List_List*_tmp83;_LL62: if((_tmp80.Rel_n).tag != 1)goto _LL64;_tmp81=(
struct Cyc_List_List*)(_tmp80.Rel_n).val;if(_tmp81 != 0)goto _LL64;_LL63: goto _LL61;
_LL64: if((_tmp80.C_n).tag != 3)goto _LL66;_tmp82=(struct Cyc_List_List*)(_tmp80.C_n).val;
if(_tmp82 != 0)goto _LL66;_LL65: {const char*_tmp2F9;void*_tmp2F8;(_tmp2F8=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp2F9="tc: C_n in tcFndecl",
_tag_dyneither(_tmp2F9,sizeof(char),20))),_tag_dyneither(_tmp2F8,sizeof(void*),0)));}
_LL66: if((_tmp80.Abs_n).tag != 2)goto _LL68;_tmp83=(struct Cyc_List_List*)(_tmp80.Abs_n).val;
_LL67: {const char*_tmp2FC;void*_tmp2FB;(_tmp2FB=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp2FC="tc: Abs_n in tcFndecl",
_tag_dyneither(_tmp2FC,sizeof(char),22))),_tag_dyneither(_tmp2FB,sizeof(void*),0)));}
_LL68:;_LL69:{const char*_tmp300;void*_tmp2FF[1];struct Cyc_String_pa_PrintArg_struct
_tmp2FE;(_tmp2FE.tag=0,((_tmp2FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(fd->name)),((_tmp2FF[0]=& _tmp2FE,Cyc_Tcutil_terr(loc,((
_tmp300="qualified function declarations are not implemented (%s)",
_tag_dyneither(_tmp300,sizeof(char),57))),_tag_dyneither(_tmp2FF,sizeof(void*),1)))))));}
return;_LL61:;}(*fd->name).f1=Cyc_Absyn_Abs_n(te->ns,fd->sc == Cyc_Absyn_ExternC
 || in_cinclude);Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{void*t=Cyc_Tcutil_fndecl2typ(
fd);fd->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,fd->attributes);{struct Cyc_List_List*
atts=fd->attributes;for(0;atts != 0;atts=atts->tl){void*_tmp8B=(void*)atts->hd;
_LL6B: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp8C=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)
_tmp8B;if(_tmp8C->tag != 7)goto _LL6D;}_LL6C: goto _LL6E;_LL6D: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*
_tmp8D=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp8B;if(_tmp8D->tag
!= 6)goto _LL6F;}_LL6E:{const char*_tmp304;void*_tmp303[1];struct Cyc_String_pa_PrintArg_struct
_tmp302;(_tmp302.tag=0,((_tmp302.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp303[0]=& _tmp302,Cyc_Tcutil_terr(
loc,((_tmp304="bad attribute %s for function",_tag_dyneither(_tmp304,sizeof(char),
30))),_tag_dyneither(_tmp303,sizeof(void*),1)))))));}goto _LL6A;_LL6F:;_LL70: goto
_LL6A;_LL6A:;}}{struct _handler_cons _tmp91;_push_handler(& _tmp91);{int _tmp93=0;
if(setjmp(_tmp91.handler))_tmp93=1;if(!_tmp93){{struct _tuple13*_tmp94=((struct
_tuple13*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,
v);void*_tmp95=(*_tmp94).f1;void*_tmp97;_LL72: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*
_tmp96=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp95;if(_tmp96->tag != 0)
goto _LL74;else{_tmp97=(void*)_tmp96->f1;}}_LL73: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct
_tmp307;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp306;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*
_tmp9C=(_tmp306=_cycalloc(sizeof(*_tmp306)),((_tmp306[0]=((_tmp307.tag=2,((
_tmp307.f1=fd,_tmp307)))),_tmp306)));void*_tmp9D=Cyc_Tcdecl_merge_binding(_tmp97,(
void*)_tmp9C,loc,Cyc_Tc_tc_msg);if(_tmp9D == (void*)& Cyc_Absyn_Unresolved_b_val)
goto _LL71;if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){if(!(_tmp9D
== _tmp97  && (*_tmp94).f2)){struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp30D;
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp30C;struct _tuple13*_tmp30B;ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct
_tuple13*v))Cyc_Dict_insert)(ge->ordinaries,v,(struct _tuple13*)((_tmp30B=
_cycalloc(sizeof(*_tmp30B)),((_tmp30B->f1=(void*)((_tmp30D=_cycalloc(sizeof(*
_tmp30D)),((_tmp30D[0]=((_tmp30C.tag=0,((_tmp30C.f1=(void*)_tmp9D,_tmp30C)))),
_tmp30D)))),((_tmp30B->f2=1,_tmp30B)))))));}}goto _LL71;}_LL74: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*
_tmp98=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp95;if(_tmp98->tag != 1)
goto _LL76;}_LL75:{const char*_tmp310;void*_tmp30F;(_tmp30F=0,Cyc_Tcutil_warn(loc,((
_tmp310="function declaration shadows previous type declaration",_tag_dyneither(
_tmp310,sizeof(char),55))),_tag_dyneither(_tmp30F,sizeof(void*),0)));}goto _LL71;
_LL76: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp99=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)
_tmp95;if(_tmp99->tag != 2)goto _LL78;}_LL77:{const char*_tmp313;void*_tmp312;(
_tmp312=0,Cyc_Tcutil_warn(loc,((_tmp313="function declaration shadows previous datatype constructor",
_tag_dyneither(_tmp313,sizeof(char),59))),_tag_dyneither(_tmp312,sizeof(void*),0)));}
goto _LL71;_LL78: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp9A=(
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp95;if(_tmp9A->tag != 4)
goto _LL7A;}_LL79: goto _LL7B;_LL7A: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*
_tmp9B=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp95;if(_tmp9B->tag != 3)
goto _LL71;}_LL7B:{const char*_tmp316;void*_tmp315;(_tmp315=0,Cyc_Tcutil_warn(loc,((
_tmp316="function declaration shadows previous enum tag",_tag_dyneither(_tmp316,
sizeof(char),47))),_tag_dyneither(_tmp315,sizeof(void*),0)));}goto _LL71;_LL71:;};
_pop_handler();}else{void*_tmp92=(void*)_exn_thrown;void*_tmpAA=_tmp92;_LL7D: {
struct Cyc_Dict_Absent_exn_struct*_tmpAB=(struct Cyc_Dict_Absent_exn_struct*)
_tmpAA;if(_tmpAB->tag != Cyc_Dict_Absent)goto _LL7F;}_LL7E: if(exports == 0  || Cyc_Tc_export_member(
fd->name,*exports)){struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp319;struct
Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp318;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*
_tmpAC=(_tmp318=_cycalloc(sizeof(*_tmp318)),((_tmp318[0]=((_tmp319.tag=2,((
_tmp319.f1=fd,_tmp319)))),_tmp318)));struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*
_tmp328;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp327;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct
_tmp326;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp325;struct _tuple13*
_tmp324;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,v,(struct
_tuple13*)((_tmp324=_cycalloc(sizeof(*_tmp324)),((_tmp324->f1=(void*)((_tmp328=
_cycalloc(sizeof(*_tmp328)),((_tmp328[0]=((_tmp325.tag=0,((_tmp325.f1=(void*)((
void*)((_tmp327=_cycalloc(sizeof(*_tmp327)),((_tmp327[0]=((_tmp326.tag=2,((
_tmp326.f1=fd,_tmp326)))),_tmp327))))),_tmp325)))),_tmp328)))),((_tmp324->f2=0,
_tmp324)))))));}goto _LL7C;_LL7F:;_LL80:(void)_throw(_tmpAA);_LL7C:;}};}if(
in_cinclude)return;{struct _RegionHandle _tmpB4=_new_region("fnrgn");struct
_RegionHandle*fnrgn=& _tmpB4;_push_region(fnrgn);{struct Cyc_Tcenv_Fenv*_tmpB5=Cyc_Tcenv_new_fenv(
fnrgn,loc,fd);struct Cyc_Tcenv_Tenv*_tmp329;struct Cyc_Tcenv_Tenv*_tmpB6=(_tmp329=
_region_malloc(fnrgn,sizeof(*_tmp329)),((_tmp329->ns=te->ns,((_tmp329->ae=te->ae,((
_tmp329->le=(struct Cyc_Tcenv_Fenv*)_tmpB5,((_tmp329->allow_valueof=0,_tmp329)))))))));
Cyc_Tcstmt_tcStmt(_tmpB6,fd->body,0);Cyc_Tcenv_check_delayed_effects(_tmpB6);Cyc_Tcenv_check_delayed_constraints(
_tmpB6);if(!Cyc_Tcenv_all_labels_resolved(_tmpB6)){const char*_tmp32C;void*
_tmp32B;(_tmp32B=0,Cyc_Tcutil_terr(loc,((_tmp32C="function has goto statements to undefined labels",
_tag_dyneither(_tmp32C,sizeof(char),49))),_tag_dyneither(_tmp32B,sizeof(void*),0)));}}
if(Cyc_Tc_is_main(fd->name)){{void*_tmpBA=Cyc_Tcutil_compress(fd->ret_type);
enum Cyc_Absyn_Size_of _tmpBD;_LL82: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*
_tmpBB=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpBA;if(_tmpBB->tag != 0)
goto _LL84;}_LL83:{const char*_tmp32F;void*_tmp32E;(_tmp32E=0,Cyc_Tcutil_warn(loc,((
_tmp32F="main declared with return type void",_tag_dyneither(_tmp32F,sizeof(char),
36))),_tag_dyneither(_tmp32E,sizeof(void*),0)));}goto _LL81;_LL84: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmpBC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpBA;if(_tmpBC->tag != 6)
goto _LL86;else{_tmpBD=_tmpBC->f2;}}_LL85: goto _LL81;_LL86:;_LL87:{const char*
_tmp333;void*_tmp332[1];struct Cyc_String_pa_PrintArg_struct _tmp331;(_tmp331.tag=
0,((_tmp331.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
fd->ret_type)),((_tmp332[0]=& _tmp331,Cyc_Tcutil_terr(loc,((_tmp333="main declared with return type %s instead of int or void",
_tag_dyneither(_tmp333,sizeof(char),57))),_tag_dyneither(_tmp332,sizeof(void*),1)))))));}
goto _LL81;_LL81:;}if(fd->c_varargs  || fd->cyc_varargs != 0){const char*_tmp336;
void*_tmp335;(_tmp335=0,Cyc_Tcutil_terr(loc,((_tmp336="main declared with varargs",
_tag_dyneither(_tmp336,sizeof(char),27))),_tag_dyneither(_tmp335,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC5=fd->args;if(_tmpC5 != 0){struct _tuple8 _tmpC7;void*
_tmpC8;struct _tuple8*_tmpC6=(struct _tuple8*)_tmpC5->hd;_tmpC7=*_tmpC6;_tmpC8=
_tmpC7.f3;{void*_tmpC9=Cyc_Tcutil_compress(_tmpC8);enum Cyc_Absyn_Size_of _tmpCB;
_LL89: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpCA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmpC9;if(_tmpCA->tag != 6)goto _LL8B;else{_tmpCB=_tmpCA->f2;}}_LL8A: goto _LL88;
_LL8B:;_LL8C:{const char*_tmp33A;void*_tmp339[1];struct Cyc_String_pa_PrintArg_struct
_tmp338;(_tmp338.tag=0,((_tmp338.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpC8)),((_tmp339[0]=& _tmp338,Cyc_Tcutil_terr(loc,((
_tmp33A="main declared with first argument of type %s instead of int",
_tag_dyneither(_tmp33A,sizeof(char),60))),_tag_dyneither(_tmp339,sizeof(void*),1)))))));}
goto _LL88;_LL88:;}_tmpC5=_tmpC5->tl;if(_tmpC5 != 0){struct _tuple8 _tmpD0;void*
_tmpD1;struct _tuple8*_tmpCF=(struct _tuple8*)_tmpC5->hd;_tmpD0=*_tmpCF;_tmpD1=
_tmpD0.f3;_tmpC5=_tmpC5->tl;if(_tmpC5 != 0){const char*_tmp33D;void*_tmp33C;(
_tmp33C=0,Cyc_Tcutil_terr(loc,((_tmp33D="main declared with too many arguments",
_tag_dyneither(_tmp33D,sizeof(char),38))),_tag_dyneither(_tmp33C,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmp33E;struct Cyc_Core_Opt*tvs=(_tmp33E=_cycalloc(sizeof(*
_tmp33E)),((_tmp33E->v=fd->tvs,_tmp33E)));struct Cyc_Core_Opt*_tmp346;struct Cyc_Core_Opt*
_tmp345;struct Cyc_Core_Opt*_tmp344;struct Cyc_Core_Opt*_tmp343;struct Cyc_Core_Opt*
_tmp342;struct Cyc_Core_Opt*_tmp341;struct Cyc_Core_Opt*_tmp340;struct Cyc_Core_Opt*
_tmp33F;if(((!Cyc_Tcutil_unify(_tmpD1,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(
Cyc_Absyn_new_evar(((_tmp345=_cycalloc(sizeof(*_tmp345)),((_tmp345->v=& Cyc_Tcutil_rk,
_tmp345)))),tvs)),Cyc_Absyn_new_evar(((_tmp346=_cycalloc(sizeof(*_tmp346)),((
_tmp346->v=& Cyc_Tcutil_rk,_tmp346)))),tvs),Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)())) && !Cyc_Tcutil_unify(_tmpD1,Cyc_Absyn_dyneither_typ(
Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((_tmp343=_cycalloc(sizeof(*
_tmp343)),((_tmp343->v=& Cyc_Tcutil_rk,_tmp343)))),tvs)),Cyc_Absyn_new_evar(((
_tmp344=_cycalloc(sizeof(*_tmp344)),((_tmp344->v=& Cyc_Tcutil_rk,_tmp344)))),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())))
 && !Cyc_Tcutil_unify(_tmpD1,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((
_tmp341=_cycalloc(sizeof(*_tmp341)),((_tmp341->v=& Cyc_Tcutil_rk,_tmp341)))),tvs)),
Cyc_Absyn_new_evar(((_tmp342=_cycalloc(sizeof(*_tmp342)),((_tmp342->v=& Cyc_Tcutil_rk,
_tmp342)))),tvs),Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())))
 && !Cyc_Tcutil_unify(_tmpD1,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(
Cyc_Absyn_new_evar(((_tmp33F=_cycalloc(sizeof(*_tmp33F)),((_tmp33F->v=& Cyc_Tcutil_rk,
_tmp33F)))),tvs)),Cyc_Absyn_new_evar(((_tmp340=_cycalloc(sizeof(*_tmp340)),((
_tmp340->v=& Cyc_Tcutil_rk,_tmp340)))),tvs),Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)()))){const char*_tmp34A;void*_tmp349[1];struct Cyc_String_pa_PrintArg_struct
_tmp348;(_tmp348.tag=0,((_tmp348.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpD1)),((_tmp349[0]=& _tmp348,Cyc_Tcutil_terr(loc,((
_tmp34A="second argument of main has type %s instead of char??",_tag_dyneither(
_tmp34A,sizeof(char),54))),_tag_dyneither(_tmp349,sizeof(void*),1)))))));}};}}};};
_pop_region(fnrgn);};};}static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Typedefdecl*td){struct
_dyneither_ptr*v=(*td->name).f2;{union Cyc_Absyn_Nmspace _tmpE0=(*td->name).f1;
struct Cyc_List_List*_tmpE1;struct Cyc_List_List*_tmpE2;struct Cyc_List_List*_tmpE3;
_LL8E: if((_tmpE0.Rel_n).tag != 1)goto _LL90;_tmpE1=(struct Cyc_List_List*)(_tmpE0.Rel_n).val;
if(_tmpE1 != 0)goto _LL90;_LL8F: goto _LL91;_LL90: if((_tmpE0.C_n).tag != 3)goto _LL92;
_tmpE2=(struct Cyc_List_List*)(_tmpE0.C_n).val;if(_tmpE2 != 0)goto _LL92;_LL91: goto
_LL93;_LL92: if((_tmpE0.Abs_n).tag != 2)goto _LL94;_tmpE3=(struct Cyc_List_List*)(
_tmpE0.Abs_n).val;if(_tmpE3 != 0)goto _LL94;_LL93: goto _LL8D;_LL94:;_LL95:{const
char*_tmp34E;void*_tmp34D[1];struct Cyc_String_pa_PrintArg_struct _tmp34C;(_tmp34C.tag=
0,((_tmp34C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
td->name)),((_tmp34D[0]=& _tmp34C,Cyc_Tcutil_terr(loc,((_tmp34E="qualified typedef declarations are not implemented (%s)",
_tag_dyneither(_tmp34E,sizeof(char),56))),_tag_dyneither(_tmp34D,sizeof(void*),1)))))));}
return;_LL8D:;}if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(
ge->typedefs,v)){{const char*_tmp352;void*_tmp351[1];struct Cyc_String_pa_PrintArg_struct
_tmp350;(_tmp350.tag=0,((_tmp350.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
v),((_tmp351[0]=& _tmp350,Cyc_Tcutil_terr(loc,((_tmp352="redeclaration of typedef %s",
_tag_dyneither(_tmp352,sizeof(char),28))),_tag_dyneither(_tmp351,sizeof(void*),1)))))));}
return;}(*td->name).f1=Cyc_Absyn_Abs_n(te->ns,0);Cyc_Tcutil_check_unique_tvars(
loc,td->tvs);Cyc_Tcutil_add_tvar_identities(td->tvs);if(td->defn != 0){Cyc_Tcutil_check_type(
loc,te,td->tvs,& Cyc_Tcutil_tak,0,(void*)_check_null(td->defn));(td->tq).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)
_check_null(td->defn));}{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
void*_tmpEA=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);struct
Cyc_Core_Opt*_tmpEC;struct Cyc_Core_Opt**_tmpED;struct Cyc_Core_Opt*_tmpEF;struct
Cyc_Core_Opt**_tmpF0;struct Cyc_Absyn_Kind*_tmpF1;_LL97: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*
_tmpEB=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpEA;if(_tmpEB->tag
!= 1)goto _LL99;else{_tmpEC=_tmpEB->f1;_tmpED=(struct Cyc_Core_Opt**)& _tmpEB->f1;}}
_LL98: if(td->defn != 0){const char*_tmp356;void*_tmp355[1];struct Cyc_String_pa_PrintArg_struct
_tmp354;(_tmp354.tag=0,((_tmp354.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Tvar*)tvs->hd)->name),((_tmp355[0]=& _tmp354,Cyc_Tcutil_warn(loc,((
_tmp356="type variable %s is not used in typedef definition",_tag_dyneither(
_tmp356,sizeof(char),51))),_tag_dyneither(_tmp355,sizeof(void*),1)))))));}{
struct Cyc_Core_Opt*_tmp357;*_tmpED=((_tmp357=_cycalloc(sizeof(*_tmp357)),((
_tmp357->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk),_tmp357))));}goto _LL96;
_LL99: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpEE=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)
_tmpEA;if(_tmpEE->tag != 2)goto _LL9B;else{_tmpEF=_tmpEE->f1;_tmpF0=(struct Cyc_Core_Opt**)&
_tmpEE->f1;_tmpF1=_tmpEE->f2;}}_LL9A:{struct Cyc_Core_Opt*_tmp358;*_tmpF0=((
_tmp358=_cycalloc(sizeof(*_tmp358)),((_tmp358->v=Cyc_Tcutil_kind_to_bound(_tmpF1),
_tmp358))));}goto _LL96;_LL9B:;_LL9C: continue;_LL96:;}}ge->typedefs=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(
ge->typedefs,v,td);}static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Tcenv_Genv*ge,unsigned int loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*
tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){struct _RegionHandle
_tmpF7=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpF7;
_push_region(uprev_rgn);for(0;rpo != 0;rpo=rpo->tl){struct _tuple10 _tmpF9;void*
_tmpFA;void*_tmpFB;struct _tuple10*_tmpF8=(struct _tuple10*)rpo->hd;_tmpF9=*_tmpF8;
_tmpFA=_tmpF9.f1;_tmpFB=_tmpF9.f2;Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_ek,
0,_tmpFA);Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,_tmpFB);}{struct Cyc_List_List*
prev_fields=0;struct Cyc_List_List*_tmpFC=fields;for(0;_tmpFC != 0;_tmpFC=_tmpFC->tl){
struct Cyc_Absyn_Aggrfield _tmpFE;struct _dyneither_ptr*_tmpFF;struct Cyc_Absyn_Tqual
_tmp100;void*_tmp101;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_List_List*_tmp103;
struct Cyc_Absyn_Aggrfield*_tmpFD=(struct Cyc_Absyn_Aggrfield*)_tmpFC->hd;_tmpFE=*
_tmpFD;_tmpFF=_tmpFE.name;_tmp100=_tmpFE.tq;_tmp101=_tmpFE.type;_tmp102=_tmpFE.width;
_tmp103=_tmpFE.attributes;if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct
_dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,
prev_fields,_tmpFF)){const char*_tmp35C;void*_tmp35B[1];struct Cyc_String_pa_PrintArg_struct
_tmp35A;(_tmp35A.tag=0,((_tmp35A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpFF),((_tmp35B[0]=& _tmp35A,Cyc_Tcutil_terr(loc,((_tmp35C="duplicate member %s",
_tag_dyneither(_tmp35C,sizeof(char),20))),_tag_dyneither(_tmp35B,sizeof(void*),1)))))));}{
const char*_tmp35D;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpFF,((_tmp35D="",
_tag_dyneither(_tmp35D,sizeof(char),1))))!= 0){struct Cyc_List_List*_tmp35E;
prev_fields=((_tmp35E=_region_malloc(uprev_rgn,sizeof(*_tmp35E)),((_tmp35E->hd=
_tmpFF,((_tmp35E->tl=prev_fields,_tmp35E))))));}}{struct Cyc_Absyn_Kind*
field_kind=& Cyc_Tcutil_tmk;if(_tmpFC->tl == 0  && str_or_union == Cyc_Absyn_StructA)
field_kind=& Cyc_Tcutil_tak;Cyc_Tcutil_check_type(loc,te,tvs,field_kind,0,_tmp101);(((
struct Cyc_Absyn_Aggrfield*)_tmpFC->hd)->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(((struct Cyc_Absyn_Aggrfield*)_tmpFC->hd)->tq).print_const,_tmp101);Cyc_Tcutil_check_bitfield(
loc,te,_tmp101,_tmp102,_tmpFF);};}};;_pop_region(uprev_rgn);}static void Cyc_Tc_rule_out_memkind(
unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*tvs,int
constrain_top_kind){struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp109=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);
struct Cyc_Core_Opt*_tmp10B;struct Cyc_Core_Opt**_tmp10C;struct Cyc_Core_Opt*
_tmp10E;struct Cyc_Core_Opt**_tmp10F;struct Cyc_Absyn_Kind*_tmp110;struct Cyc_Absyn_Kind
_tmp111;enum Cyc_Absyn_KindQual _tmp112;enum Cyc_Absyn_AliasQual _tmp113;struct Cyc_Core_Opt*
_tmp115;struct Cyc_Core_Opt**_tmp116;struct Cyc_Absyn_Kind*_tmp117;struct Cyc_Absyn_Kind
_tmp118;enum Cyc_Absyn_KindQual _tmp119;enum Cyc_Absyn_AliasQual _tmp11A;struct Cyc_Core_Opt*
_tmp11C;struct Cyc_Core_Opt**_tmp11D;struct Cyc_Absyn_Kind*_tmp11E;struct Cyc_Absyn_Kind*
_tmp120;struct Cyc_Absyn_Kind _tmp121;enum Cyc_Absyn_KindQual _tmp122;enum Cyc_Absyn_AliasQual
_tmp123;_LL9E: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp10A=(
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp109;if(_tmp10A->tag != 1)
goto _LLA0;else{_tmp10B=_tmp10A->f1;_tmp10C=(struct Cyc_Core_Opt**)& _tmp10A->f1;}}
_LL9F:{struct Cyc_Core_Opt*_tmp35F;*_tmp10C=((_tmp35F=_cycalloc(sizeof(*_tmp35F)),((
_tmp35F->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp35F))));}continue;_LLA0: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp10D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)
_tmp109;if(_tmp10D->tag != 2)goto _LLA2;else{_tmp10E=_tmp10D->f1;_tmp10F=(struct
Cyc_Core_Opt**)& _tmp10D->f1;_tmp110=_tmp10D->f2;_tmp111=*_tmp110;_tmp112=_tmp111.kind;
if(_tmp112 != Cyc_Absyn_MemKind)goto _LLA2;_tmp113=_tmp111.aliasqual;}}_LLA1: if(
constrain_top_kind  && _tmp113 == Cyc_Absyn_Top){struct Cyc_Absyn_Kind*_tmp362;
struct Cyc_Core_Opt*_tmp361;*_tmp10F=((_tmp361=_cycalloc(sizeof(*_tmp361)),((
_tmp361->v=Cyc_Tcutil_kind_to_bound(((_tmp362=_cycalloc_atomic(sizeof(*_tmp362)),((
_tmp362->kind=Cyc_Absyn_BoxKind,((_tmp362->aliasqual=Cyc_Absyn_Aliasable,_tmp362))))))),
_tmp361))));}else{struct Cyc_Absyn_Kind*_tmp365;struct Cyc_Core_Opt*_tmp364;*
_tmp10F=((_tmp364=_cycalloc(sizeof(*_tmp364)),((_tmp364->v=Cyc_Tcutil_kind_to_bound(((
_tmp365=_cycalloc_atomic(sizeof(*_tmp365)),((_tmp365->kind=Cyc_Absyn_BoxKind,((
_tmp365->aliasqual=_tmp113,_tmp365))))))),_tmp364))));}continue;_LLA2:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmp114=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp109;if(_tmp114->tag
!= 2)goto _LLA4;else{_tmp115=_tmp114->f1;_tmp116=(struct Cyc_Core_Opt**)& _tmp114->f1;
_tmp117=_tmp114->f2;_tmp118=*_tmp117;_tmp119=_tmp118.kind;if(_tmp119 != Cyc_Absyn_BoxKind)
goto _LLA4;_tmp11A=_tmp118.aliasqual;if(_tmp11A != Cyc_Absyn_Top)goto _LLA4;}}if(!
constrain_top_kind)goto _LLA4;_LLA3:{struct Cyc_Absyn_Kind*_tmp368;struct Cyc_Core_Opt*
_tmp367;*_tmp116=((_tmp367=_cycalloc(sizeof(*_tmp367)),((_tmp367->v=Cyc_Tcutil_kind_to_bound(((
_tmp368=_cycalloc_atomic(sizeof(*_tmp368)),((_tmp368->kind=Cyc_Absyn_BoxKind,((
_tmp368->aliasqual=Cyc_Absyn_Aliasable,_tmp368))))))),_tmp367))));}continue;
_LLA4: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp11B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)
_tmp109;if(_tmp11B->tag != 2)goto _LLA6;else{_tmp11C=_tmp11B->f1;_tmp11D=(struct
Cyc_Core_Opt**)& _tmp11B->f1;_tmp11E=_tmp11B->f2;}}_LLA5:{struct Cyc_Core_Opt*
_tmp369;*_tmp11D=((_tmp369=_cycalloc(sizeof(*_tmp369)),((_tmp369->v=Cyc_Tcutil_kind_to_bound(
_tmp11E),_tmp369))));}continue;_LLA6: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*
_tmp11F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp109;if(_tmp11F->tag != 
0)goto _LLA8;else{_tmp120=_tmp11F->f1;_tmp121=*_tmp120;_tmp122=_tmp121.kind;if(
_tmp122 != Cyc_Absyn_MemKind)goto _LLA8;_tmp123=_tmp121.aliasqual;}}_LLA7:{const
char*_tmp371;void*_tmp370[3];struct Cyc_String_pa_PrintArg_struct _tmp36F;struct
Cyc_String_pa_PrintArg_struct _tmp36E;struct Cyc_Absyn_Kind*_tmp36D;struct Cyc_String_pa_PrintArg_struct
_tmp36C;(_tmp36C.tag=0,((_tmp36C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp36D=_cycalloc_atomic(sizeof(*_tmp36D)),((_tmp36D->kind=
Cyc_Absyn_MemKind,((_tmp36D->aliasqual=_tmp123,_tmp36D)))))))),((_tmp36E.tag=0,((
_tmp36E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
tvs2->hd)->name),((_tmp36F.tag=0,((_tmp36F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*n),((_tmp370[0]=& _tmp36F,((_tmp370[1]=& _tmp36E,((_tmp370[2]=&
_tmp36C,Cyc_Tcutil_terr(loc,((_tmp371="type %s attempts to abstract type variable %s of kind %s",
_tag_dyneither(_tmp371,sizeof(char),57))),_tag_dyneither(_tmp370,sizeof(void*),3)))))))))))))))))));}
continue;_LLA8:;_LLA9: continue;_LL9D:;}}struct _tuple14{struct Cyc_Absyn_AggrdeclImpl*
f1;struct Cyc_Absyn_Aggrdecl***f2;};void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Aggrdecl*ad){struct
_dyneither_ptr*_tmp132=(*ad->name).f2;{struct Cyc_List_List*atts=ad->attributes;
for(0;atts != 0;atts=atts->tl){void*_tmp133=(void*)atts->hd;_LLAB: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*
_tmp134=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp133;if(_tmp134->tag
!= 7)goto _LLAD;}_LLAC: goto _LLAE;_LLAD: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*
_tmp135=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp133;if(_tmp135->tag
!= 6)goto _LLAF;}_LLAE: continue;_LLAF:;_LLB0:{const char*_tmp376;void*_tmp375[2];
struct Cyc_String_pa_PrintArg_struct _tmp374;struct Cyc_String_pa_PrintArg_struct
_tmp373;(_tmp373.tag=0,((_tmp373.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp132),((_tmp374.tag=0,((_tmp374.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp375[0]=&
_tmp374,((_tmp375[1]=& _tmp373,Cyc_Tcutil_terr(loc,((_tmp376="bad attribute %s in  %s definition",
_tag_dyneither(_tmp376,sizeof(char),35))),_tag_dyneither(_tmp375,sizeof(void*),2)))))))))))));}
goto _LLAA;_LLAA:;}}{struct Cyc_List_List*_tmp13A=ad->tvs;{union Cyc_Absyn_Nmspace
_tmp13B=(*ad->name).f1;struct Cyc_List_List*_tmp13C;struct Cyc_List_List*_tmp13D;
struct Cyc_List_List*_tmp13E;_LLB2: if((_tmp13B.Rel_n).tag != 1)goto _LLB4;_tmp13C=(
struct Cyc_List_List*)(_tmp13B.Rel_n).val;if(_tmp13C != 0)goto _LLB4;_LLB3: goto
_LLB5;_LLB4: if((_tmp13B.C_n).tag != 3)goto _LLB6;_tmp13D=(struct Cyc_List_List*)(
_tmp13B.C_n).val;if(_tmp13D != 0)goto _LLB6;_LLB5: goto _LLB7;_LLB6: if((_tmp13B.Abs_n).tag
!= 2)goto _LLB8;_tmp13E=(struct Cyc_List_List*)(_tmp13B.Abs_n).val;if(_tmp13E != 0)
goto _LLB8;_LLB7: goto _LLB1;_LLB8:;_LLB9:{const char*_tmp37A;void*_tmp379[1];struct
Cyc_String_pa_PrintArg_struct _tmp378;(_tmp378.tag=0,((_tmp378.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(ad->name)),((
_tmp379[0]=& _tmp378,Cyc_Tcutil_terr(loc,((_tmp37A="qualified struct declarations are not implemented (%s)",
_tag_dyneither(_tmp37A,sizeof(char),55))),_tag_dyneither(_tmp379,sizeof(void*),1)))))));}
return;_LLB1:;}(*ad->name).f1=Cyc_Absyn_Abs_n(te->ns,ad->sc == Cyc_Absyn_ExternC);
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);Cyc_Tcutil_add_tvar_identities(ad->tvs);{
struct _tuple14 _tmp37B;struct _tuple14 _tmp143=(_tmp37B.f1=ad->impl,((_tmp37B.f2=((
struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(
ge->aggrdecls,_tmp132),_tmp37B)));struct Cyc_Absyn_AggrdeclImpl*_tmp144;struct Cyc_Absyn_Aggrdecl***
_tmp145;struct Cyc_Absyn_AggrdeclImpl*_tmp146;struct Cyc_Absyn_AggrdeclImpl _tmp147;
struct Cyc_List_List*_tmp148;struct Cyc_List_List*_tmp149;struct Cyc_List_List*
_tmp14A;int _tmp14B;struct Cyc_Absyn_Aggrdecl***_tmp14C;struct Cyc_Absyn_AggrdeclImpl*
_tmp14D;struct Cyc_Absyn_AggrdeclImpl _tmp14E;struct Cyc_List_List*_tmp14F;struct
Cyc_List_List*_tmp150;struct Cyc_List_List*_tmp151;int _tmp152;struct Cyc_Absyn_Aggrdecl***
_tmp153;struct Cyc_Absyn_Aggrdecl**_tmp154;struct Cyc_Absyn_AggrdeclImpl*_tmp155;
struct Cyc_Absyn_Aggrdecl***_tmp156;struct Cyc_Absyn_Aggrdecl**_tmp157;_LLBB:
_tmp144=_tmp143.f1;if(_tmp144 != 0)goto _LLBD;_tmp145=_tmp143.f2;if(_tmp145 != 0)
goto _LLBD;_LLBC: Cyc_Tc_rule_out_memkind(loc,_tmp132,_tmp13A,0);{struct Cyc_Absyn_Aggrdecl**
_tmp37C;ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,
_tmp132,((_tmp37C=_cycalloc(sizeof(*_tmp37C)),((_tmp37C[0]=ad,_tmp37C)))));}goto
_LLBA;_LLBD: _tmp146=_tmp143.f1;if(_tmp146 == 0)goto _LLBF;_tmp147=*_tmp146;_tmp148=
_tmp147.exist_vars;_tmp149=_tmp147.rgn_po;_tmp14A=_tmp147.fields;_tmp14B=_tmp147.tagged;
_tmp14C=_tmp143.f2;if(_tmp14C != 0)goto _LLBF;_LLBE: {struct Cyc_Absyn_Aggrdecl*
_tmp37F;struct Cyc_Absyn_Aggrdecl**_tmp37E;struct Cyc_Absyn_Aggrdecl**_tmp159=(
_tmp37E=_cycalloc(sizeof(*_tmp37E)),((_tmp37E[0]=((_tmp37F=_cycalloc(sizeof(*
_tmp37F)),((_tmp37F->kind=ad->kind,((_tmp37F->sc=Cyc_Absyn_Extern,((_tmp37F->name=
ad->name,((_tmp37F->tvs=_tmp13A,((_tmp37F->impl=0,((_tmp37F->attributes=ad->attributes,
_tmp37F)))))))))))))),_tmp37E)));ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(
ge->aggrdecls,_tmp132,_tmp159);Cyc_Tcutil_check_unique_tvars(loc,_tmp148);Cyc_Tcutil_add_tvar_identities(
_tmp148);if(_tmp14B  && ad->kind == Cyc_Absyn_StructA){const char*_tmp382;void*
_tmp381;(_tmp381=0,Cyc_Tcutil_terr(loc,((_tmp382="@tagged qualifier is only allowed on union declarations",
_tag_dyneither(_tmp382,sizeof(char),56))),_tag_dyneither(_tmp381,sizeof(void*),0)));}
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp13A,_tmp148),_tmp149,_tmp14A);Cyc_Tc_rule_out_memkind(
loc,_tmp132,_tmp13A,0);Cyc_Tc_rule_out_memkind(loc,_tmp132,_tmp148,1);if(ad->kind
== Cyc_Absyn_UnionA  && !_tmp14B){struct Cyc_List_List*f=_tmp14A;for(0;f != 0;f=f->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)){const char*
_tmp388;void*_tmp387[3];struct Cyc_String_pa_PrintArg_struct _tmp386;struct Cyc_String_pa_PrintArg_struct
_tmp385;struct Cyc_String_pa_PrintArg_struct _tmp384;(_tmp384.tag=0,((_tmp384.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)
f->hd)->type)),((_tmp385.tag=0,((_tmp385.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp132),((_tmp386.tag=0,((_tmp386.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name),((_tmp387[0]=&
_tmp386,((_tmp387[1]=& _tmp385,((_tmp387[2]=& _tmp384,Cyc_Tcutil_warn(loc,((
_tmp388="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp388,sizeof(char),98))),_tag_dyneither(_tmp387,sizeof(void*),3)))))))))))))))))));}}}*
_tmp159=ad;goto _LLBA;}_LLBF: _tmp14D=_tmp143.f1;if(_tmp14D == 0)goto _LLC1;_tmp14E=*
_tmp14D;_tmp14F=_tmp14E.exist_vars;_tmp150=_tmp14E.rgn_po;_tmp151=_tmp14E.fields;
_tmp152=_tmp14E.tagged;_tmp153=_tmp143.f2;if(_tmp153 == 0)goto _LLC1;_tmp154=*
_tmp153;_LLC0: if(ad->kind != (*_tmp154)->kind){const char*_tmp38B;void*_tmp38A;(
_tmp38A=0,Cyc_Tcutil_terr(loc,((_tmp38B="cannot reuse struct names for unions and vice-versa",
_tag_dyneither(_tmp38B,sizeof(char),52))),_tag_dyneither(_tmp38A,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*_tmp165=*_tmp154;{struct Cyc_Absyn_Aggrdecl*_tmp38C;*
_tmp154=((_tmp38C=_cycalloc(sizeof(*_tmp38C)),((_tmp38C->kind=ad->kind,((_tmp38C->sc=
Cyc_Absyn_Extern,((_tmp38C->name=ad->name,((_tmp38C->tvs=_tmp13A,((_tmp38C->impl=
0,((_tmp38C->attributes=ad->attributes,_tmp38C))))))))))))));}Cyc_Tcutil_check_unique_tvars(
loc,_tmp14F);Cyc_Tcutil_add_tvar_identities(_tmp14F);if(_tmp152  && ad->kind == 
Cyc_Absyn_StructA){const char*_tmp38F;void*_tmp38E;(_tmp38E=0,Cyc_Tcutil_terr(loc,((
_tmp38F="@tagged qualifier is only allowed on union declarations",_tag_dyneither(
_tmp38F,sizeof(char),56))),_tag_dyneither(_tmp38E,sizeof(void*),0)));}Cyc_Tc_tcAggrImpl(
te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp13A,_tmp14F),_tmp150,_tmp151);*_tmp154=_tmp165;_tmp157=
_tmp154;goto _LLC2;};_LLC1: _tmp155=_tmp143.f1;if(_tmp155 != 0)goto _LLBA;_tmp156=
_tmp143.f2;if(_tmp156 == 0)goto _LLBA;_tmp157=*_tmp156;_LLC2: {struct Cyc_Absyn_Aggrdecl*
_tmp169=Cyc_Tcdecl_merge_aggrdecl(*_tmp157,ad,loc,Cyc_Tc_tc_msg);if(_tmp169 == 0)
return;else{Cyc_Tc_rule_out_memkind(loc,_tmp132,_tmp13A,0);if(ad->impl != 0)Cyc_Tc_rule_out_memkind(
loc,_tmp132,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);*
_tmp157=(struct Cyc_Absyn_Aggrdecl*)_tmp169;ad=(struct Cyc_Absyn_Aggrdecl*)_tmp169;
goto _LLBA;}}_LLBA:;}{struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp395;
struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct _tmp394;struct _tuple13*_tmp393;ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct
_tuple13*v))Cyc_Dict_insert)(ge->ordinaries,_tmp132,(struct _tuple13*)((_tmp393=
_cycalloc(sizeof(*_tmp393)),((_tmp393->f1=(void*)((_tmp395=_cycalloc(sizeof(*
_tmp395)),((_tmp395[0]=((_tmp394.tag=1,((_tmp394.f1=ad,_tmp394)))),_tmp395)))),((
_tmp393->f2=1,_tmp393)))))));};};}static void Cyc_Tc_rule_out_mem_and_unique(
unsigned int loc,struct _dyneither_ptr*v,struct Cyc_List_List*tvs){struct Cyc_List_List*
tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){void*_tmp16D=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt*_tmp16F;struct Cyc_Core_Opt**
_tmp170;struct Cyc_Core_Opt*_tmp172;struct Cyc_Core_Opt**_tmp173;struct Cyc_Absyn_Kind*
_tmp174;struct Cyc_Absyn_Kind _tmp175;enum Cyc_Absyn_KindQual _tmp176;enum Cyc_Absyn_AliasQual
_tmp177;struct Cyc_Core_Opt*_tmp179;struct Cyc_Core_Opt**_tmp17A;struct Cyc_Absyn_Kind*
_tmp17B;struct Cyc_Absyn_Kind _tmp17C;enum Cyc_Absyn_KindQual _tmp17D;enum Cyc_Absyn_AliasQual
_tmp17E;struct Cyc_Core_Opt*_tmp180;struct Cyc_Core_Opt**_tmp181;struct Cyc_Absyn_Kind*
_tmp182;struct Cyc_Absyn_Kind _tmp183;enum Cyc_Absyn_KindQual _tmp184;enum Cyc_Absyn_AliasQual
_tmp185;struct Cyc_Core_Opt*_tmp187;struct Cyc_Core_Opt**_tmp188;struct Cyc_Absyn_Kind*
_tmp189;struct Cyc_Absyn_Kind _tmp18A;enum Cyc_Absyn_KindQual _tmp18B;enum Cyc_Absyn_AliasQual
_tmp18C;struct Cyc_Core_Opt*_tmp18E;struct Cyc_Core_Opt**_tmp18F;struct Cyc_Absyn_Kind*
_tmp190;struct Cyc_Absyn_Kind _tmp191;enum Cyc_Absyn_KindQual _tmp192;enum Cyc_Absyn_AliasQual
_tmp193;struct Cyc_Absyn_Kind*_tmp195;struct Cyc_Absyn_Kind _tmp196;enum Cyc_Absyn_KindQual
_tmp197;enum Cyc_Absyn_AliasQual _tmp198;struct Cyc_Core_Opt*_tmp19A;struct Cyc_Core_Opt**
_tmp19B;struct Cyc_Absyn_Kind*_tmp19C;struct Cyc_Absyn_Kind _tmp19D;enum Cyc_Absyn_KindQual
_tmp19E;enum Cyc_Absyn_AliasQual _tmp19F;struct Cyc_Absyn_Kind*_tmp1A1;struct Cyc_Absyn_Kind
_tmp1A2;enum Cyc_Absyn_KindQual _tmp1A3;enum Cyc_Absyn_AliasQual _tmp1A4;struct Cyc_Absyn_Kind*
_tmp1A6;struct Cyc_Absyn_Kind _tmp1A7;enum Cyc_Absyn_KindQual _tmp1A8;enum Cyc_Absyn_AliasQual
_tmp1A9;_LLC4: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp16E=(
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp16D;if(_tmp16E->tag != 1)
goto _LLC6;else{_tmp16F=_tmp16E->f1;_tmp170=(struct Cyc_Core_Opt**)& _tmp16E->f1;}}
_LLC5: _tmp173=_tmp170;goto _LLC7;_LLC6: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmp171=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16D;if(_tmp171->tag
!= 2)goto _LLC8;else{_tmp172=_tmp171->f1;_tmp173=(struct Cyc_Core_Opt**)& _tmp171->f1;
_tmp174=_tmp171->f2;_tmp175=*_tmp174;_tmp176=_tmp175.kind;if(_tmp176 != Cyc_Absyn_MemKind)
goto _LLC8;_tmp177=_tmp175.aliasqual;if(_tmp177 != Cyc_Absyn_Top)goto _LLC8;}}_LLC7:
_tmp17A=_tmp173;goto _LLC9;_LLC8: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmp178=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16D;if(_tmp178->tag
!= 2)goto _LLCA;else{_tmp179=_tmp178->f1;_tmp17A=(struct Cyc_Core_Opt**)& _tmp178->f1;
_tmp17B=_tmp178->f2;_tmp17C=*_tmp17B;_tmp17D=_tmp17C.kind;if(_tmp17D != Cyc_Absyn_MemKind)
goto _LLCA;_tmp17E=_tmp17C.aliasqual;if(_tmp17E != Cyc_Absyn_Aliasable)goto _LLCA;}}
_LLC9:{struct Cyc_Core_Opt*_tmp396;*_tmp17A=((_tmp396=_cycalloc(sizeof(*_tmp396)),((
_tmp396->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp396))));}goto _LLC3;_LLCA: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp17F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)
_tmp16D;if(_tmp17F->tag != 2)goto _LLCC;else{_tmp180=_tmp17F->f1;_tmp181=(struct
Cyc_Core_Opt**)& _tmp17F->f1;_tmp182=_tmp17F->f2;_tmp183=*_tmp182;_tmp184=_tmp183.kind;
if(_tmp184 != Cyc_Absyn_AnyKind)goto _LLCC;_tmp185=_tmp183.aliasqual;if(_tmp185 != 
Cyc_Absyn_Top)goto _LLCC;}}_LLCB: _tmp188=_tmp181;goto _LLCD;_LLCC: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*
_tmp186=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16D;if(_tmp186->tag
!= 2)goto _LLCE;else{_tmp187=_tmp186->f1;_tmp188=(struct Cyc_Core_Opt**)& _tmp186->f1;
_tmp189=_tmp186->f2;_tmp18A=*_tmp189;_tmp18B=_tmp18A.kind;if(_tmp18B != Cyc_Absyn_AnyKind)
goto _LLCE;_tmp18C=_tmp18A.aliasqual;if(_tmp18C != Cyc_Absyn_Aliasable)goto _LLCE;}}
_LLCD:{struct Cyc_Core_Opt*_tmp397;*_tmp188=((_tmp397=_cycalloc(sizeof(*_tmp397)),((
_tmp397->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak),_tmp397))));}goto _LLC3;_LLCE: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp18D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)
_tmp16D;if(_tmp18D->tag != 2)goto _LLD0;else{_tmp18E=_tmp18D->f1;_tmp18F=(struct
Cyc_Core_Opt**)& _tmp18D->f1;_tmp190=_tmp18D->f2;_tmp191=*_tmp190;_tmp192=_tmp191.kind;
if(_tmp192 != Cyc_Absyn_RgnKind)goto _LLD0;_tmp193=_tmp191.aliasqual;if(_tmp193 != 
Cyc_Absyn_Top)goto _LLD0;}}_LLCF:{struct Cyc_Core_Opt*_tmp398;*_tmp18F=((_tmp398=
_cycalloc(sizeof(*_tmp398)),((_tmp398->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),
_tmp398))));}goto _LLC3;_LLD0: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*
_tmp194=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16D;if(_tmp194->tag != 
0)goto _LLD2;else{_tmp195=_tmp194->f1;_tmp196=*_tmp195;_tmp197=_tmp196.kind;if(
_tmp197 != Cyc_Absyn_RgnKind)goto _LLD2;_tmp198=_tmp196.aliasqual;if(_tmp198 != Cyc_Absyn_Top)
goto _LLD2;}}_LLD1:{const char*_tmp39D;void*_tmp39C[2];struct Cyc_String_pa_PrintArg_struct
_tmp39B;struct Cyc_String_pa_PrintArg_struct _tmp39A;(_tmp39A.tag=0,((_tmp39A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((
_tmp39B.tag=0,((_tmp39B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((
_tmp39C[0]=& _tmp39B,((_tmp39C[1]=& _tmp39A,Cyc_Tcutil_terr(loc,((_tmp39D="type %s attempts to abstract type variable %s of kind TR",
_tag_dyneither(_tmp39D,sizeof(char),57))),_tag_dyneither(_tmp39C,sizeof(void*),2)))))))))))));}
goto _LLC3;_LLD2: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp199=(struct
Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16D;if(_tmp199->tag != 2)goto _LLD4;
else{_tmp19A=_tmp199->f1;_tmp19B=(struct Cyc_Core_Opt**)& _tmp199->f1;_tmp19C=
_tmp199->f2;_tmp19D=*_tmp19C;_tmp19E=_tmp19D.kind;_tmp19F=_tmp19D.aliasqual;if(
_tmp19F != Cyc_Absyn_Unique)goto _LLD4;}}_LLD3: _tmp1A3=_tmp19E;goto _LLD5;_LLD4: {
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1A0=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)
_tmp16D;if(_tmp1A0->tag != 0)goto _LLD6;else{_tmp1A1=_tmp1A0->f1;_tmp1A2=*_tmp1A1;
_tmp1A3=_tmp1A2.kind;_tmp1A4=_tmp1A2.aliasqual;if(_tmp1A4 != Cyc_Absyn_Unique)
goto _LLD6;}}_LLD5:{const char*_tmp3A5;void*_tmp3A4[3];struct Cyc_String_pa_PrintArg_struct
_tmp3A3;struct Cyc_String_pa_PrintArg_struct _tmp3A2;struct Cyc_Absyn_Kind*_tmp3A1;
struct Cyc_String_pa_PrintArg_struct _tmp3A0;(_tmp3A0.tag=0,((_tmp3A0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(((_tmp3A1=
_cycalloc_atomic(sizeof(*_tmp3A1)),((_tmp3A1->kind=_tmp1A3,((_tmp3A1->aliasqual=
Cyc_Absyn_Unique,_tmp3A1)))))))),((_tmp3A2.tag=0,((_tmp3A2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((
_tmp3A3.tag=0,((_tmp3A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((
_tmp3A4[0]=& _tmp3A3,((_tmp3A4[1]=& _tmp3A2,((_tmp3A4[2]=& _tmp3A0,Cyc_Tcutil_terr(
loc,((_tmp3A5="type %s attempts to abstract type variable %s of kind %s",
_tag_dyneither(_tmp3A5,sizeof(char),57))),_tag_dyneither(_tmp3A4,sizeof(void*),3)))))))))))))))))));}
goto _LLC3;_LLD6: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1A5=(struct
Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16D;if(_tmp1A5->tag != 0)goto _LLD8;
else{_tmp1A6=_tmp1A5->f1;_tmp1A7=*_tmp1A6;_tmp1A8=_tmp1A7.kind;if(_tmp1A8 != Cyc_Absyn_MemKind)
goto _LLD8;_tmp1A9=_tmp1A7.aliasqual;}}_LLD7:{const char*_tmp3AD;void*_tmp3AC[3];
struct Cyc_String_pa_PrintArg_struct _tmp3AB;struct Cyc_String_pa_PrintArg_struct
_tmp3AA;struct Cyc_Absyn_Kind*_tmp3A9;struct Cyc_String_pa_PrintArg_struct _tmp3A8;(
_tmp3A8.tag=0,((_tmp3A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(((
_tmp3A9=_cycalloc_atomic(sizeof(*_tmp3A9)),((_tmp3A9->kind=Cyc_Absyn_MemKind,((
_tmp3A9->aliasqual=_tmp1A9,_tmp3A9)))))))),((_tmp3AA.tag=0,((_tmp3AA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((
_tmp3AB.tag=0,((_tmp3AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((
_tmp3AC[0]=& _tmp3AB,((_tmp3AC[1]=& _tmp3AA,((_tmp3AC[2]=& _tmp3A8,Cyc_Tcutil_terr(
loc,((_tmp3AD="type %s attempts to abstract type variable %s of kind %s",
_tag_dyneither(_tmp3AD,sizeof(char),57))),_tag_dyneither(_tmp3AC,sizeof(void*),3)))))))))))))))))));}
goto _LLC3;_LLD8:;_LLD9: goto _LLC3;_LLC3:;}}struct _tuple15{struct Cyc_Absyn_Tqual f1;
void*f2;};static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct _dyneither_ptr obj,int
is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*
tvs,struct Cyc_Absyn_Datatypedecl*tudres){{struct Cyc_List_List*_tmp1BD=fields;
for(0;_tmp1BD != 0;_tmp1BD=_tmp1BD->tl){struct Cyc_Absyn_Datatypefield*_tmp1BE=(
struct Cyc_Absyn_Datatypefield*)_tmp1BD->hd;{struct Cyc_List_List*typs=_tmp1BE->typs;
for(0;typs != 0;typs=typs->tl){Cyc_Tcutil_check_type(_tmp1BE->loc,te,tvs,& Cyc_Tcutil_tmk,
0,(*((struct _tuple15*)typs->hd)).f2);{struct _RegionHandle _tmp1BF=_new_region("temp");
struct _RegionHandle*temp=& _tmp1BF;_push_region(temp);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
temp,(*((struct _tuple15*)typs->hd)).f2)){const char*_tmp3B1;void*_tmp3B0[1];
struct Cyc_String_pa_PrintArg_struct _tmp3AF;(_tmp3AF.tag=0,((_tmp3AF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp1BE->name)),((
_tmp3B0[0]=& _tmp3AF,Cyc_Tcutil_terr(_tmp1BE->loc,((_tmp3B1="noalias pointers in datatypes are not allowed (%s)",
_tag_dyneither(_tmp3B1,sizeof(char),51))),_tag_dyneither(_tmp3B0,sizeof(void*),1)))))));}((*((
struct _tuple15*)typs->hd)).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
_tmp1BE->loc,((*((struct _tuple15*)typs->hd)).f1).print_const,(*((struct _tuple15*)
typs->hd)).f2);;_pop_region(temp);};}}{union Cyc_Absyn_Nmspace _tmp1C3=(*_tmp1BE->name).f1;
struct Cyc_List_List*_tmp1C4;struct Cyc_List_List*_tmp1C5;struct Cyc_List_List*
_tmp1C6;struct Cyc_List_List*_tmp1C7;int _tmp1C8;_LLDB: if((_tmp1C3.Rel_n).tag != 1)
goto _LLDD;_tmp1C4=(struct Cyc_List_List*)(_tmp1C3.Rel_n).val;if(_tmp1C4 != 0)goto
_LLDD;_LLDC: if(is_extensible)(*_tmp1BE->name).f1=Cyc_Absyn_Abs_n(te->ns,0);else{(*
_tmp1BE->name).f1=(*name).f1;}goto _LLDA;_LLDD: if((_tmp1C3.Rel_n).tag != 1)goto
_LLDF;_tmp1C5=(struct Cyc_List_List*)(_tmp1C3.Rel_n).val;_LLDE:{const char*_tmp3B5;
void*_tmp3B4[1];struct Cyc_String_pa_PrintArg_struct _tmp3B3;(_tmp3B3.tag=0,((
_tmp3B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp1BE->name)),((_tmp3B4[0]=& _tmp3B3,Cyc_Tcutil_terr(_tmp1BE->loc,((_tmp3B5="qualified datatypefield declarations are not allowed (%s)",
_tag_dyneither(_tmp3B5,sizeof(char),58))),_tag_dyneither(_tmp3B4,sizeof(void*),1)))))));}
goto _LLDA;_LLDF: if((_tmp1C3.C_n).tag != 3)goto _LLE1;_tmp1C6=(struct Cyc_List_List*)(
_tmp1C3.C_n).val;_LLE0: goto _LLDA;_LLE1: if((_tmp1C3.Abs_n).tag != 2)goto _LLE3;
_tmp1C7=(struct Cyc_List_List*)(_tmp1C3.Abs_n).val;_LLE2: goto _LLDA;_LLE3: if((
_tmp1C3.Loc_n).tag != 4)goto _LLDA;_tmp1C8=(int)(_tmp1C3.Loc_n).val;_LLE4: {const
char*_tmp3B8;void*_tmp3B7;(_tmp3B7=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3B8="tcDatatypeFields: Loc_n",
_tag_dyneither(_tmp3B8,sizeof(char),24))),_tag_dyneither(_tmp3B7,sizeof(void*),0)));}
_LLDA:;};}}{struct Cyc_List_List*fields2;if(is_extensible){int _tmp1CE=1;struct Cyc_List_List*
_tmp1CF=Cyc_Tcdecl_sort_xdatatype_fields(fields,& _tmp1CE,(*name).f2,loc,Cyc_Tc_tc_msg);
if(_tmp1CE)fields2=_tmp1CF;else{fields2=0;}}else{struct _RegionHandle _tmp1D0=
_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp1D0;_push_region(
uprev_rgn);{struct Cyc_List_List*prev_fields=0;{struct Cyc_List_List*fs=fields;
for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Datatypefield*_tmp1D1=(struct Cyc_Absyn_Datatypefield*)
fs->hd;if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),
struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,
prev_fields,(*_tmp1D1->name).f2)){const char*_tmp3BD;void*_tmp3BC[2];struct Cyc_String_pa_PrintArg_struct
_tmp3BB;struct Cyc_String_pa_PrintArg_struct _tmp3BA;(_tmp3BA.tag=0,((_tmp3BA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)obj),((_tmp3BB.tag=0,((_tmp3BB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1D1->name).f2),((_tmp3BC[0]=&
_tmp3BB,((_tmp3BC[1]=& _tmp3BA,Cyc_Tcutil_terr(_tmp1D1->loc,((_tmp3BD="duplicate field name %s in %s",
_tag_dyneither(_tmp3BD,sizeof(char),30))),_tag_dyneither(_tmp3BC,sizeof(void*),2)))))))))))));}
else{struct Cyc_List_List*_tmp3BE;prev_fields=((_tmp3BE=_region_malloc(uprev_rgn,
sizeof(*_tmp3BE)),((_tmp3BE->hd=(*_tmp1D1->name).f2,((_tmp3BE->tl=prev_fields,
_tmp3BE))))));}if(_tmp1D1->sc != Cyc_Absyn_Public){{const char*_tmp3C2;void*
_tmp3C1[1];struct Cyc_String_pa_PrintArg_struct _tmp3C0;(_tmp3C0.tag=0,((_tmp3C0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1D1->name).f2),((_tmp3C1[0]=&
_tmp3C0,Cyc_Tcutil_warn(loc,((_tmp3C2="ignoring scope of field %s",
_tag_dyneither(_tmp3C2,sizeof(char),27))),_tag_dyneither(_tmp3C1,sizeof(void*),1)))))));}
_tmp1D1->sc=Cyc_Absyn_Public;}}}fields2=fields;};_pop_region(uprev_rgn);}{struct
Cyc_List_List*_tmp1DA=fields;for(0;_tmp1DA != 0;_tmp1DA=_tmp1DA->tl){struct Cyc_Absyn_Datatypefield*
_tmp1DB=(struct Cyc_Absyn_Datatypefield*)_tmp1DA->hd;struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*
_tmp3C8;struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp3C7;struct _tuple13*
_tmp3C6;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp1DB->name).f2,(
struct _tuple13*)((_tmp3C6=_cycalloc(sizeof(*_tmp3C6)),((_tmp3C6->f1=(void*)((
_tmp3C8=_cycalloc(sizeof(*_tmp3C8)),((_tmp3C8[0]=((_tmp3C7.tag=2,((_tmp3C7.f1=
tudres,((_tmp3C7.f2=_tmp1DB,_tmp3C7)))))),_tmp3C8)))),((_tmp3C6->f2=1,_tmp3C6)))))));}}
return fields2;};}struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***
f2;};void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,
unsigned int loc,struct Cyc_Absyn_Datatypedecl*tud){struct _dyneither_ptr*v=(*tud->name).f2;
const char*_tmp3CA;const char*_tmp3C9;struct _dyneither_ptr obj=tud->is_extensible?(
_tmp3CA="@extensible datatype",_tag_dyneither(_tmp3CA,sizeof(char),21)):((
_tmp3C9="datatype",_tag_dyneither(_tmp3C9,sizeof(char),9)));struct Cyc_List_List*
tvs=tud->tvs;Cyc_Tcutil_check_unique_tvars(loc,tvs);Cyc_Tcutil_add_tvar_identities(
tvs);{struct _RegionHandle _tmp1DF=_new_region("temp");struct _RegionHandle*temp=&
_tmp1DF;_push_region(temp);{struct Cyc_Absyn_Datatypedecl***tud_opt;{struct
_handler_cons _tmp1E0;_push_handler(& _tmp1E0);{int _tmp1E2=0;if(setjmp(_tmp1E0.handler))
_tmp1E2=1;if(!_tmp1E2){tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(temp,te,loc,tud->name);
if(tud_opt == 0  && !tud->is_extensible){struct Cyc_Dict_Absent_exn_struct _tmp3CD;
struct Cyc_Dict_Absent_exn_struct*_tmp3CC;(int)_throw((void*)((_tmp3CC=
_cycalloc_atomic(sizeof(*_tmp3CC)),((_tmp3CC[0]=((_tmp3CD.tag=Cyc_Dict_Absent,
_tmp3CD)),_tmp3CC)))));}if(tud_opt != 0)tud->name=(*(*tud_opt))->name;else{(*tud->name).f1=
Cyc_Absyn_Abs_n(te->ns,0);};_pop_handler();}else{void*_tmp1E1=(void*)_exn_thrown;
void*_tmp1E6=_tmp1E1;_LLE6: {struct Cyc_Dict_Absent_exn_struct*_tmp1E7=(struct Cyc_Dict_Absent_exn_struct*)
_tmp1E6;if(_tmp1E7->tag != Cyc_Dict_Absent)goto _LLE8;}_LLE7:{union Cyc_Absyn_Nmspace
_tmp1E8=(*tud->name).f1;struct Cyc_List_List*_tmp1E9;struct Cyc_List_List*_tmp1EA;
_LLEB: if((_tmp1E8.Rel_n).tag != 1)goto _LLED;_tmp1E9=(struct Cyc_List_List*)(
_tmp1E8.Rel_n).val;if(_tmp1E9 != 0)goto _LLED;_LLEC:(*tud->name).f1=Cyc_Absyn_Abs_n(
te->ns,0);goto _LLEA;_LLED: if((_tmp1E8.Abs_n).tag != 2)goto _LLEF;_tmp1EA=(struct
Cyc_List_List*)(_tmp1E8.Abs_n).val;_LLEE: goto _LLF0;_LLEF:;_LLF0:{const char*
_tmp3D1;void*_tmp3D0[1];struct Cyc_String_pa_PrintArg_struct _tmp3CF;(_tmp3CF.tag=
0,((_tmp3CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tud->name)),((_tmp3D0[0]=& _tmp3CF,Cyc_Tcutil_terr(loc,((_tmp3D1="qualified (non-extensible) datatype declarations are not implemented (%s)",
_tag_dyneither(_tmp3D1,sizeof(char),74))),_tag_dyneither(_tmp3D0,sizeof(void*),1)))))));}
_npop_handler(0);return;_LLEA:;}{struct Cyc_Absyn_Datatypedecl***_tmp1EE=((struct
Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(
ge->datatypedecls,v);{struct Cyc_Absyn_Datatypedecl***_tmp3D2;tud_opt=(
unsigned int)_tmp1EE?(_tmp3D2=_region_malloc(temp,sizeof(*_tmp3D2)),((_tmp3D2[0]=*
_tmp1EE,_tmp3D2))): 0;}goto _LLE5;};_LLE8:;_LLE9:(void)_throw(_tmp1E6);_LLE5:;}};}{
struct _tuple16 _tmp3D3;struct _tuple16 _tmp1F1=(_tmp3D3.f1=tud->fields,((_tmp3D3.f2=
tud_opt,_tmp3D3)));struct Cyc_Core_Opt*_tmp1F2;struct Cyc_Absyn_Datatypedecl***
_tmp1F3;struct Cyc_Core_Opt*_tmp1F4;struct Cyc_Core_Opt _tmp1F5;struct Cyc_List_List*
_tmp1F6;struct Cyc_List_List**_tmp1F7;struct Cyc_Absyn_Datatypedecl***_tmp1F8;
struct Cyc_Core_Opt*_tmp1F9;struct Cyc_Core_Opt _tmp1FA;struct Cyc_List_List*_tmp1FB;
struct Cyc_List_List**_tmp1FC;struct Cyc_Absyn_Datatypedecl***_tmp1FD;struct Cyc_Absyn_Datatypedecl**
_tmp1FE;struct Cyc_Core_Opt*_tmp1FF;struct Cyc_Absyn_Datatypedecl***_tmp200;struct
Cyc_Absyn_Datatypedecl**_tmp201;_LLF2: _tmp1F2=_tmp1F1.f1;if(_tmp1F2 != 0)goto
_LLF4;_tmp1F3=_tmp1F1.f2;if(_tmp1F3 != 0)goto _LLF4;_LLF3: Cyc_Tc_rule_out_mem_and_unique(
loc,v,tvs);{struct Cyc_Absyn_Datatypedecl**_tmp3D4;ge->datatypedecls=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(
ge->datatypedecls,v,((_tmp3D4=_cycalloc(sizeof(*_tmp3D4)),((_tmp3D4[0]=tud,
_tmp3D4)))));}goto _LLF1;_LLF4: _tmp1F4=_tmp1F1.f1;if(_tmp1F4 == 0)goto _LLF6;
_tmp1F5=*_tmp1F4;_tmp1F6=(struct Cyc_List_List*)_tmp1F5.v;_tmp1F7=(struct Cyc_List_List**)&(*
_tmp1F1.f1).v;_tmp1F8=_tmp1F1.f2;if(_tmp1F8 != 0)goto _LLF6;_LLF5: {struct Cyc_Absyn_Datatypedecl*
_tmp3D7;struct Cyc_Absyn_Datatypedecl**_tmp3D6;struct Cyc_Absyn_Datatypedecl**
_tmp203=(_tmp3D6=_cycalloc(sizeof(*_tmp3D6)),((_tmp3D6[0]=((_tmp3D7=_cycalloc(
sizeof(*_tmp3D7)),((_tmp3D7->sc=Cyc_Absyn_Extern,((_tmp3D7->name=tud->name,((
_tmp3D7->tvs=tvs,((_tmp3D7->fields=0,((_tmp3D7->is_extensible=tud->is_extensible,
_tmp3D7)))))))))))),_tmp3D6)));ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(
ge->datatypedecls,v,_tmp203);*_tmp1F7=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,
tud->name,*_tmp1F7,tvs,tud);Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);*_tmp203=
tud;goto _LLF1;}_LLF6: _tmp1F9=_tmp1F1.f1;if(_tmp1F9 == 0)goto _LLF8;_tmp1FA=*
_tmp1F9;_tmp1FB=(struct Cyc_List_List*)_tmp1FA.v;_tmp1FC=(struct Cyc_List_List**)&(*
_tmp1F1.f1).v;_tmp1FD=_tmp1F1.f2;if(_tmp1FD == 0)goto _LLF8;_tmp1FE=*_tmp1FD;_LLF7: {
struct Cyc_Absyn_Datatypedecl*_tmp206=*_tmp1FE;if((!tud->is_extensible  && (
unsigned int)_tmp206) && _tmp206->is_extensible)tud->is_extensible=1;{struct Cyc_Absyn_Datatypedecl*
_tmp3D8;*_tmp1FE=((_tmp3D8=_cycalloc(sizeof(*_tmp3D8)),((_tmp3D8->sc=Cyc_Absyn_Extern,((
_tmp3D8->name=tud->name,((_tmp3D8->tvs=tvs,((_tmp3D8->fields=0,((_tmp3D8->is_extensible=
tud->is_extensible,_tmp3D8))))))))))));}*_tmp1FC=Cyc_Tc_tcDatatypeFields(te,ge,
loc,obj,tud->is_extensible,tud->name,*_tmp1FC,tvs,tud);*_tmp1FE=_tmp206;_tmp201=
_tmp1FE;goto _LLF9;}_LLF8: _tmp1FF=_tmp1F1.f1;if(_tmp1FF != 0)goto _LLF1;_tmp200=
_tmp1F1.f2;if(_tmp200 == 0)goto _LLF1;_tmp201=*_tmp200;_LLF9: {struct Cyc_Absyn_Datatypedecl*
_tmp208=Cyc_Tcdecl_merge_datatypedecl(*_tmp201,tud,loc,Cyc_Tc_tc_msg);Cyc_Tc_rule_out_mem_and_unique(
loc,v,tvs);if(_tmp208 == 0){_npop_handler(0);return;}else{*_tmp201=(struct Cyc_Absyn_Datatypedecl*)
_tmp208;goto _LLF1;}}_LLF1:;};};_pop_region(temp);};}void Cyc_Tc_tcEnumdecl(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Enumdecl*
ed){struct _dyneither_ptr*v=(*ed->name).f2;{union Cyc_Absyn_Nmspace _tmp20B=(*ed->name).f1;
struct Cyc_List_List*_tmp20C;struct Cyc_List_List*_tmp20D;_LLFB: if((_tmp20B.Rel_n).tag
!= 1)goto _LLFD;_tmp20C=(struct Cyc_List_List*)(_tmp20B.Rel_n).val;if(_tmp20C != 0)
goto _LLFD;_LLFC: goto _LLFE;_LLFD: if((_tmp20B.Abs_n).tag != 2)goto _LLFF;_tmp20D=(
struct Cyc_List_List*)(_tmp20B.Abs_n).val;if(_tmp20D != 0)goto _LLFF;_LLFE: goto
_LLFA;_LLFF:;_LL100:{const char*_tmp3DC;void*_tmp3DB[1];struct Cyc_String_pa_PrintArg_struct
_tmp3DA;(_tmp3DA.tag=0,((_tmp3DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(ed->name)),((_tmp3DB[0]=& _tmp3DA,Cyc_Tcutil_terr(loc,((
_tmp3DC="qualified enum declarations are not implemented (%s)",_tag_dyneither(
_tmp3DC,sizeof(char),53))),_tag_dyneither(_tmp3DB,sizeof(void*),1)))))));}
return;_LLFA:;}(*ed->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);
if(ed->fields != 0){struct _RegionHandle _tmp211=_new_region("uprev_rgn");struct
_RegionHandle*uprev_rgn=& _tmp211;_push_region(uprev_rgn);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;struct Cyc_List_List*fs=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct
Cyc_Absyn_Enumfield*_tmp212=(struct Cyc_Absyn_Enumfield*)fs->hd;if(((int(*)(int(*
compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,
struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp212->name).f2)){
const char*_tmp3E0;void*_tmp3DF[1];struct Cyc_String_pa_PrintArg_struct _tmp3DE;(
_tmp3DE.tag=0,((_tmp3DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*
_tmp212->name).f2),((_tmp3DF[0]=& _tmp3DE,Cyc_Tcutil_terr(_tmp212->loc,((_tmp3E0="duplicate field name %s",
_tag_dyneither(_tmp3E0,sizeof(char),24))),_tag_dyneither(_tmp3DF,sizeof(void*),1)))))));}
else{struct Cyc_List_List*_tmp3E1;prev_fields=((_tmp3E1=_region_malloc(uprev_rgn,
sizeof(*_tmp3E1)),((_tmp3E1->hd=(*_tmp212->name).f2,((_tmp3E1->tl=prev_fields,
_tmp3E1))))));}if(((struct _tuple13**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->ordinaries,(*_tmp212->name).f2)!= 0){
const char*_tmp3E5;void*_tmp3E4[1];struct Cyc_String_pa_PrintArg_struct _tmp3E3;(
_tmp3E3.tag=0,((_tmp3E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*
_tmp212->name).f2),((_tmp3E4[0]=& _tmp3E3,Cyc_Tcutil_terr(_tmp212->loc,((_tmp3E5="enum field name %s shadows global name",
_tag_dyneither(_tmp3E5,sizeof(char),39))),_tag_dyneither(_tmp3E4,sizeof(void*),1)))))));}
if(_tmp212->tag == 0){_tmp212->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp212->loc);++ tag_count;}else{if(Cyc_Tcutil_is_const_exp(te,(struct
Cyc_Absyn_Exp*)_check_null(_tmp212->tag))){unsigned int _tmp21B;int _tmp21C;struct
_tuple11 _tmp21A=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmp212->tag));_tmp21B=_tmp21A.f1;_tmp21C=_tmp21A.f2;if(_tmp21C)tag_count=
_tmp21B + 1;}}(*_tmp212->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);}};
_pop_region(uprev_rgn);}{struct _handler_cons _tmp21D;_push_handler(& _tmp21D);{int
_tmp21F=0;if(setjmp(_tmp21D.handler))_tmp21F=1;if(!_tmp21F){{struct Cyc_Absyn_Enumdecl**
_tmp220=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->enumdecls,v);struct Cyc_Absyn_Enumdecl*
_tmp221=Cyc_Tcdecl_merge_enumdecl(*_tmp220,ed,loc,Cyc_Tc_tc_msg);if(_tmp221 == 0){
_npop_handler(0);return;}*_tmp220=(struct Cyc_Absyn_Enumdecl*)_tmp221;};
_pop_handler();}else{void*_tmp21E=(void*)_exn_thrown;void*_tmp223=_tmp21E;_LL102: {
struct Cyc_Dict_Absent_exn_struct*_tmp224=(struct Cyc_Dict_Absent_exn_struct*)
_tmp223;if(_tmp224->tag != Cyc_Dict_Absent)goto _LL104;}_LL103: {struct Cyc_Absyn_Enumdecl**
_tmp3E6;struct Cyc_Absyn_Enumdecl**_tmp225=(_tmp3E6=_cycalloc(sizeof(*_tmp3E6)),((
_tmp3E6[0]=ed,_tmp3E6)));ge->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(ge->enumdecls,
v,_tmp225);goto _LL101;}_LL104:;_LL105:(void)_throw(_tmp223);_LL101:;}};}if(ed->fields
!= 0){struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*
_tmp227=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*
_tmp3EC;struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct _tmp3EB;struct _tuple13*
_tmp3EA;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp227->name).f2,(
struct _tuple13*)((_tmp3EA=_cycalloc(sizeof(*_tmp3EA)),((_tmp3EA->f1=(void*)((
_tmp3EC=_cycalloc(sizeof(*_tmp3EC)),((_tmp3EC[0]=((_tmp3EB.tag=3,((_tmp3EB.f1=ed,((
_tmp3EB.f2=_tmp227,_tmp3EB)))))),_tmp3EC)))),((_tmp3EA->f2=1,_tmp3EA)))))));}}
if(ed->fields != 0){struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*
_tmp22B=(struct Cyc_Absyn_Enumfield*)fs->hd;Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_check_null(_tmp22B->tag));if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp22B->tag))){const char*_tmp3F1;void*_tmp3F0[2];struct Cyc_String_pa_PrintArg_struct
_tmp3EF;struct Cyc_String_pa_PrintArg_struct _tmp3EE;(_tmp3EE.tag=0,((_tmp3EE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp22B->name).f2),((_tmp3EF.tag=
0,((_tmp3EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3F0[0]=&
_tmp3EF,((_tmp3F0[1]=& _tmp3EE,Cyc_Tcutil_terr(loc,((_tmp3F1="enum %s, field %s: expression is not constant",
_tag_dyneither(_tmp3F1,sizeof(char),46))),_tag_dyneither(_tmp3F0,sizeof(void*),2)))))))))))));}}}}
static int Cyc_Tc_okay_externC(unsigned int loc,enum Cyc_Absyn_Scope sc,int
in_include){switch(sc){case Cyc_Absyn_Static: _LL106: if(!in_include){const char*
_tmp3F4;void*_tmp3F3;(_tmp3F3=0,Cyc_Tcutil_warn(loc,((_tmp3F4="static declaration nested within extern \"C\"",
_tag_dyneither(_tmp3F4,sizeof(char),44))),_tag_dyneither(_tmp3F3,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Abstract: _LL107:{const char*_tmp3F7;void*_tmp3F6;(_tmp3F6=0,
Cyc_Tcutil_warn(loc,((_tmp3F7="abstract declaration nested within extern \"C\"",
_tag_dyneither(_tmp3F7,sizeof(char),46))),_tag_dyneither(_tmp3F6,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Public: _LL108: goto _LL109;case Cyc_Absyn_Register: _LL109:
goto _LL10A;case Cyc_Absyn_Extern: _LL10A: return 1;case Cyc_Absyn_ExternC: _LL10B:{
const char*_tmp3FA;void*_tmp3F9;(_tmp3F9=0,Cyc_Tcutil_warn(loc,((_tmp3FA="nested extern \"C\" declaration",
_tag_dyneither(_tmp3FA,sizeof(char),30))),_tag_dyneither(_tmp3F9,sizeof(void*),0)));}
return 1;}}static void Cyc_Tc_resolve_export_namespace(struct Cyc_Tcenv_Tenv*te,
struct _tuple12*exp){unsigned int _tmp237;struct _tuple0*_tmp238;struct _tuple12
_tmp236=*exp;_tmp237=_tmp236.f1;_tmp238=_tmp236.f2;{struct _tuple0 _tmp23A;union
Cyc_Absyn_Nmspace _tmp23B;struct _dyneither_ptr*_tmp23C;struct _tuple0*_tmp239=
_tmp238;_tmp23A=*_tmp239;_tmp23B=_tmp23A.f1;_tmp23C=_tmp23A.f2;{union Cyc_Absyn_Nmspace
_tmp23D=_tmp23B;struct Cyc_List_List*_tmp23E;struct Cyc_List_List*_tmp23F;struct
Cyc_List_List*_tmp240;_LL10E: if((_tmp23D.Rel_n).tag != 1)goto _LL110;_tmp23E=(
struct Cyc_List_List*)(_tmp23D.Rel_n).val;if(_tmp23E != 0)goto _LL110;_LL10F: goto
_LL111;_LL110: if((_tmp23D.C_n).tag != 3)goto _LL112;_tmp23F=(struct Cyc_List_List*)(
_tmp23D.C_n).val;if(_tmp23F != 0)goto _LL112;_LL111: goto _LL10D;_LL112: if((_tmp23D.Abs_n).tag
!= 2)goto _LL114;_tmp240=(struct Cyc_List_List*)(_tmp23D.Abs_n).val;if(_tmp240 != 0)
goto _LL114;_LL113: goto _LL10D;_LL114:;_LL115:{const char*_tmp3FE;void*_tmp3FD[1];
struct Cyc_String_pa_PrintArg_struct _tmp3FC;(_tmp3FC.tag=0,((_tmp3FC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp238)),((
_tmp3FD[0]=& _tmp3FC,Cyc_Tcutil_terr(_tmp237,((_tmp3FE="qualified export variables are not implemented (%s)",
_tag_dyneither(_tmp3FE,sizeof(char),52))),_tag_dyneither(_tmp3FD,sizeof(void*),1)))))));}
return;_LL10D:;}(*_tmp238).f1=Cyc_Absyn_Abs_n(te->ns,1);};}static void Cyc_Tc_tc_decls(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int
in_externCinclude,int check_var_init,struct _RegionHandle*grgn,struct Cyc_List_List**
exports){struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_List_List*last=
0;struct Cyc_List_List*_tmp244=ds0;for(0;_tmp244 != 0;(last=_tmp244,_tmp244=
_tmp244->tl)){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp244->hd;
unsigned int loc=d->loc;void*_tmp245=d->r;struct Cyc_Absyn_Vardecl*_tmp24B;struct
Cyc_Absyn_Fndecl*_tmp24D;struct Cyc_Absyn_Typedefdecl*_tmp24F;struct Cyc_Absyn_Aggrdecl*
_tmp251;struct Cyc_Absyn_Datatypedecl*_tmp253;struct Cyc_Absyn_Enumdecl*_tmp255;
struct _dyneither_ptr*_tmp259;struct Cyc_List_List*_tmp25A;struct _tuple0*_tmp25C;
struct _tuple0 _tmp25D;union Cyc_Absyn_Nmspace _tmp25E;struct _dyneither_ptr*_tmp25F;
struct Cyc_List_List*_tmp260;struct Cyc_List_List*_tmp262;struct Cyc_List_List*
_tmp264;struct Cyc_List_List*_tmp265;_LL117: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*
_tmp246=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp245;if(_tmp246->tag != 
2)goto _LL119;}_LL118: goto _LL11A;_LL119: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*
_tmp247=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp245;if(_tmp247->tag != 
3)goto _LL11B;}_LL11A:{const char*_tmp401;void*_tmp400;(_tmp400=0,Cyc_Tcutil_terr(
loc,((_tmp401="top level let-declarations are not implemented",_tag_dyneither(
_tmp401,sizeof(char),47))),_tag_dyneither(_tmp400,sizeof(void*),0)));}goto _LL116;
_LL11B: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp248=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)
_tmp245;if(_tmp248->tag != 4)goto _LL11D;}_LL11C:{const char*_tmp404;void*_tmp403;(
_tmp403=0,Cyc_Tcutil_terr(loc,((_tmp404="top level region declarations are not implemented",
_tag_dyneither(_tmp404,sizeof(char),50))),_tag_dyneither(_tmp403,sizeof(void*),0)));}
goto _LL116;_LL11D: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp249=(
struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp245;if(_tmp249->tag != 5)goto
_LL11F;}_LL11E:{const char*_tmp407;void*_tmp406;(_tmp406=0,Cyc_Tcutil_terr(loc,((
_tmp407="top level alias declarations are not implemented",_tag_dyneither(
_tmp407,sizeof(char),49))),_tag_dyneither(_tmp406,sizeof(void*),0)));}goto _LL116;
_LL11F: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp24A=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)
_tmp245;if(_tmp24A->tag != 0)goto _LL121;else{_tmp24B=_tmp24A->f1;}}_LL120: if(
in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp24B->sc,in_externCinclude))_tmp24B->sc=
Cyc_Absyn_ExternC;Cyc_Tc_tcVardecl(te,ge,loc,_tmp24B,check_var_init,
in_externCinclude,exports);goto _LL116;_LL121: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*
_tmp24C=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp245;if(_tmp24C->tag != 1)
goto _LL123;else{_tmp24D=_tmp24C->f1;}}_LL122: if(in_externC  && Cyc_Tc_okay_externC(
d->loc,_tmp24D->sc,in_externCinclude))_tmp24D->sc=Cyc_Absyn_ExternC;Cyc_Tc_tcFndecl(
te,ge,loc,_tmp24D,in_externCinclude,exports);goto _LL116;_LL123: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*
_tmp24E=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp245;if(_tmp24E->tag
!= 9)goto _LL125;else{_tmp24F=_tmp24E->f1;}}_LL124: Cyc_Tc_tcTypedefdecl(te,ge,loc,
_tmp24F);goto _LL116;_LL125: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*
_tmp250=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp245;if(_tmp250->tag != 
6)goto _LL127;else{_tmp251=_tmp250->f1;}}_LL126: if(in_externC  && Cyc_Tc_okay_externC(
d->loc,_tmp251->sc,in_externCinclude))_tmp251->sc=Cyc_Absyn_ExternC;Cyc_Tc_tcAggrdecl(
te,ge,loc,_tmp251);goto _LL116;_LL127: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*
_tmp252=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp245;if(_tmp252->tag
!= 7)goto _LL129;else{_tmp253=_tmp252->f1;}}_LL128: if(in_externC  && Cyc_Tc_okay_externC(
d->loc,_tmp253->sc,in_externCinclude))_tmp253->sc=Cyc_Absyn_ExternC;Cyc_Tc_tcDatatypedecl(
te,ge,loc,_tmp253);goto _LL116;_LL129: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*
_tmp254=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp245;if(_tmp254->tag != 
8)goto _LL12B;else{_tmp255=_tmp254->f1;}}_LL12A: if(in_externC  && Cyc_Tc_okay_externC(
d->loc,_tmp255->sc,in_externCinclude))_tmp255->sc=Cyc_Absyn_ExternC;Cyc_Tc_tcEnumdecl(
te,ge,loc,_tmp255);goto _LL116;_LL12B: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*
_tmp256=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp245;if(_tmp256->tag
!= 14)goto _LL12D;}_LL12C:{const char*_tmp40A;void*_tmp409;(_tmp409=0,Cyc_Tcutil_warn(
d->loc,((_tmp40A="spurious __cyclone_port_on__",_tag_dyneither(_tmp40A,sizeof(
char),29))),_tag_dyneither(_tmp409,sizeof(void*),0)));}goto _LL116;_LL12D: {
struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp257=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)
_tmp245;if(_tmp257->tag != 15)goto _LL12F;}_LL12E: goto _LL116;_LL12F: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*
_tmp258=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp245;if(_tmp258->tag
!= 10)goto _LL131;else{_tmp259=_tmp258->f1;_tmp25A=_tmp258->f2;}}_LL130: {struct
Cyc_List_List*_tmp26E=te->ns;struct Cyc_List_List*_tmp40B;struct Cyc_List_List*
_tmp26F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(_tmp26E,((_tmp40B=_cycalloc(sizeof(*_tmp40B)),((_tmp40B->hd=
_tmp259,((_tmp40B->tl=0,_tmp40B)))))));if(!((int(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_member)(ge->namespaces,_tmp259)){ge->namespaces=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_rinsert)(grgn,ge->namespaces,_tmp259);te->ae=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*
v))Cyc_Dict_insert)(te->ae,_tmp26F,Cyc_Tcenv_empty_genv(grgn));}te->ns=_tmp26F;
Cyc_Tc_tc_decls(te,_tmp25A,in_externC,in_externCinclude,check_var_init,grgn,
exports);te->ns=_tmp26E;goto _LL116;}_LL131: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*
_tmp25B=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp245;if(_tmp25B->tag
!= 11)goto _LL133;else{_tmp25C=_tmp25B->f1;_tmp25D=*_tmp25C;_tmp25E=_tmp25D.f1;
_tmp25F=_tmp25D.f2;_tmp260=_tmp25B->f2;}}_LL132: {struct _dyneither_ptr*first;
struct Cyc_List_List*rest;{union Cyc_Absyn_Nmspace _tmp271=_tmp25E;int _tmp272;
struct Cyc_List_List*_tmp273;struct Cyc_List_List*_tmp274;struct Cyc_List_List*
_tmp275;struct Cyc_List_List*_tmp276;struct Cyc_List_List _tmp277;struct
_dyneither_ptr*_tmp278;struct Cyc_List_List*_tmp279;struct Cyc_List_List*_tmp27A;
struct Cyc_List_List _tmp27B;struct _dyneither_ptr*_tmp27C;struct Cyc_List_List*
_tmp27D;struct Cyc_List_List*_tmp27E;struct Cyc_List_List _tmp27F;struct
_dyneither_ptr*_tmp280;struct Cyc_List_List*_tmp281;_LL138: if((_tmp271.Loc_n).tag
!= 4)goto _LL13A;_tmp272=(int)(_tmp271.Loc_n).val;_LL139: goto _LL13B;_LL13A: if((
_tmp271.Rel_n).tag != 1)goto _LL13C;_tmp273=(struct Cyc_List_List*)(_tmp271.Rel_n).val;
if(_tmp273 != 0)goto _LL13C;_LL13B: goto _LL13D;_LL13C: if((_tmp271.C_n).tag != 3)goto
_LL13E;_tmp274=(struct Cyc_List_List*)(_tmp271.C_n).val;if(_tmp274 != 0)goto _LL13E;
_LL13D: goto _LL13F;_LL13E: if((_tmp271.Abs_n).tag != 2)goto _LL140;_tmp275=(struct
Cyc_List_List*)(_tmp271.Abs_n).val;if(_tmp275 != 0)goto _LL140;_LL13F: first=
_tmp25F;rest=0;goto _LL137;_LL140: if((_tmp271.Rel_n).tag != 1)goto _LL142;_tmp276=(
struct Cyc_List_List*)(_tmp271.Rel_n).val;if(_tmp276 == 0)goto _LL142;_tmp277=*
_tmp276;_tmp278=(struct _dyneither_ptr*)_tmp277.hd;_tmp279=_tmp277.tl;_LL141:
_tmp27C=_tmp278;_tmp27D=_tmp279;goto _LL143;_LL142: if((_tmp271.C_n).tag != 3)goto
_LL144;_tmp27A=(struct Cyc_List_List*)(_tmp271.C_n).val;if(_tmp27A == 0)goto _LL144;
_tmp27B=*_tmp27A;_tmp27C=(struct _dyneither_ptr*)_tmp27B.hd;_tmp27D=_tmp27B.tl;
_LL143: _tmp280=_tmp27C;_tmp281=_tmp27D;goto _LL145;_LL144: if((_tmp271.Abs_n).tag
!= 2)goto _LL137;_tmp27E=(struct Cyc_List_List*)(_tmp271.Abs_n).val;if(_tmp27E == 0)
goto _LL137;_tmp27F=*_tmp27E;_tmp280=(struct _dyneither_ptr*)_tmp27F.hd;_tmp281=
_tmp27F.tl;_LL145: first=_tmp280;{struct Cyc_List_List*_tmp40C;rest=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp281,((_tmp40C=
_cycalloc(sizeof(*_tmp40C)),((_tmp40C->hd=_tmp25F,((_tmp40C->tl=0,_tmp40C)))))));}
goto _LL137;_LL137:;}{struct Cyc_List_List*_tmp283=Cyc_Tcenv_resolve_namespace(te,
loc,first,rest);{struct Cyc_List_List*_tmp40D;ge->availables=(struct Cyc_List_List*)((
_tmp40D=_cycalloc(sizeof(*_tmp40D)),((_tmp40D->hd=_tmp283,((_tmp40D->tl=ge->availables,
_tmp40D))))));}Cyc_Tc_tc_decls(te,_tmp260,in_externC,in_externCinclude,
check_var_init,grgn,exports);ge->availables=((struct Cyc_List_List*)_check_null(
ge->availables))->tl;goto _LL116;};}_LL133: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*
_tmp261=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp245;if(_tmp261->tag
!= 12)goto _LL135;else{_tmp262=_tmp261->f1;}}_LL134: Cyc_Tc_tc_decls(te,_tmp262,1,
in_externCinclude,check_var_init,grgn,exports);goto _LL116;_LL135: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*
_tmp263=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp245;if(
_tmp263->tag != 13)goto _LL116;else{_tmp264=_tmp263->f1;_tmp265=_tmp263->f2;}}
_LL136:((void(*)(void(*f)(struct Cyc_Tcenv_Tenv*,struct _tuple12*),struct Cyc_Tcenv_Tenv*
env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tc_resolve_export_namespace,te,
_tmp265);{struct Cyc_List_List*newexs=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp265,(unsigned int)exports?*exports:
0);Cyc_Tc_tc_decls(te,_tmp264,1,1,check_var_init,grgn,(struct Cyc_List_List**)&
newexs);for(0;_tmp265 != 0;_tmp265=_tmp265->tl){struct _tuple12*_tmp285=(struct
_tuple12*)_tmp265->hd;if(!(*_tmp285).f3){const char*_tmp411;void*_tmp410[1];
struct Cyc_String_pa_PrintArg_struct _tmp40F;(_tmp40F.tag=0,((_tmp40F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((*_tmp285).f2)),((
_tmp410[0]=& _tmp40F,Cyc_Tcutil_warn((*_tmp285).f1,((_tmp411="%s is exported but not defined",
_tag_dyneither(_tmp411,sizeof(char),31))),_tag_dyneither(_tmp410,sizeof(void*),1)))))));}}
goto _LL116;};_LL116:;}}void Cyc_Tc_tc(struct _RegionHandle*g,struct Cyc_Tcenv_Tenv*
te,int check_var_init,struct Cyc_List_List*ds){Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,0,check_var_init,g,0);}struct Cyc_Tc_TreeshakeEnv{struct
_RegionHandle*rgn;int in_cinclude;struct Cyc_Dict_Dict nsdict;};typedef struct Cyc_Tc_TreeshakeEnv*
Cyc_Tc_treeshake_env_t;static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*
env,struct Cyc_Absyn_Decl*d);static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct
Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){return((struct Cyc_List_List*(*)(
int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*
env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}
struct _tuple17{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*f2;};static int Cyc_Tc_vardecl_needed(
struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){void*_tmp289=d->r;struct
Cyc_Absyn_Vardecl*_tmp28B;struct Cyc_List_List*_tmp28D;struct Cyc_List_List**
_tmp28E;struct Cyc_List_List*_tmp290;struct Cyc_List_List**_tmp291;struct Cyc_List_List*
_tmp293;struct Cyc_List_List**_tmp294;struct Cyc_List_List*_tmp296;struct Cyc_List_List**
_tmp297;_LL147: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp28A=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)
_tmp289;if(_tmp28A->tag != 0)goto _LL149;else{_tmp28B=_tmp28A->f1;}}_LL148: if((env->in_cinclude
 || _tmp28B->sc != Cyc_Absyn_Extern  && _tmp28B->sc != Cyc_Absyn_ExternC) || !Cyc_Absyn_qvar_cmp(
_tmp28B->name,Cyc_Absyn_uniquergn_qvar))return 1;{struct _tuple0 _tmp299;union Cyc_Absyn_Nmspace
_tmp29A;struct _dyneither_ptr*_tmp29B;struct _tuple0*_tmp298=_tmp28B->name;_tmp299=*
_tmp298;_tmp29A=_tmp299.f1;_tmp29B=_tmp299.f2;{struct Cyc_List_List*ns;{union Cyc_Absyn_Nmspace
_tmp29C=_tmp29A;int _tmp29D;struct Cyc_List_List*_tmp29E;struct Cyc_List_List*
_tmp29F;struct Cyc_List_List*_tmp2A0;_LL154: if((_tmp29C.Loc_n).tag != 4)goto _LL156;
_tmp29D=(int)(_tmp29C.Loc_n).val;_LL155: ns=0;goto _LL153;_LL156: if((_tmp29C.Rel_n).tag
!= 1)goto _LL158;_tmp29E=(struct Cyc_List_List*)(_tmp29C.Rel_n).val;_LL157: ns=
_tmp29E;goto _LL153;_LL158: if((_tmp29C.C_n).tag != 3)goto _LL15A;_tmp29F=(struct Cyc_List_List*)(
_tmp29C.C_n).val;_LL159: ns=_tmp29F;goto _LL153;_LL15A: if((_tmp29C.Abs_n).tag != 2)
goto _LL153;_tmp2A0=(struct Cyc_List_List*)(_tmp29C.Abs_n).val;_LL15B: ns=_tmp2A0;
goto _LL153;_LL153:;}{struct _tuple17*_tmp2A1=((struct _tuple17*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(env->nsdict,ns);struct Cyc_Tcenv_Genv*
_tmp2A2=(*_tmp2A1).f1;int _tmp2A3=(*((struct _tuple13*(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp2A2->ordinaries,_tmp29B)).f2;if(!
_tmp2A3)(*_tmp2A1).f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*
s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(env->rgn,(*_tmp2A1).f2,_tmp29B);
return _tmp2A3;};};};_LL149: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*
_tmp28C=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp289;if(_tmp28C->tag
!= 12)goto _LL14B;else{_tmp28D=_tmp28C->f1;_tmp28E=(struct Cyc_List_List**)&
_tmp28C->f1;}}_LL14A: _tmp291=_tmp28E;goto _LL14C;_LL14B: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*
_tmp28F=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp289;if(_tmp28F->tag
!= 11)goto _LL14D;else{_tmp290=_tmp28F->f2;_tmp291=(struct Cyc_List_List**)&
_tmp28F->f2;}}_LL14C: _tmp294=_tmp291;goto _LL14E;_LL14D: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*
_tmp292=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp289;if(_tmp292->tag
!= 10)goto _LL14F;else{_tmp293=_tmp292->f2;_tmp294=(struct Cyc_List_List**)&
_tmp292->f2;}}_LL14E:*_tmp294=Cyc_Tc_treeshake_f(env,*_tmp294);return 1;_LL14F: {
struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp295=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)
_tmp289;if(_tmp295->tag != 13)goto _LL151;else{_tmp296=_tmp295->f1;_tmp297=(struct
Cyc_List_List**)& _tmp295->f1;}}_LL150: {int in_cinclude=env->in_cinclude;env->in_cinclude=
1;*_tmp297=Cyc_Tc_treeshake_f(env,*_tmp297);env->in_cinclude=in_cinclude;return 1;}
_LL151:;_LL152: return 1;_LL146:;}static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*
set,struct _dyneither_ptr*x,void*y){return !((int(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_member)(set,x);}static struct _tuple17*Cyc_Tc_treeshake_make_env_f(
struct _RegionHandle*rgn,struct Cyc_Tcenv_Genv*ge){struct _tuple17*_tmp412;return(
_tmp412=_region_malloc(rgn,sizeof(*_tmp412)),((_tmp412->f1=ge,((_tmp412->f2=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_rempty)(rgn,Cyc_strptrcmp),_tmp412)))));}struct
_tuple18{struct Cyc_List_List*f1;struct _tuple17*f2;};struct Cyc_List_List*Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){struct _RegionHandle _tmp2A5=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp2A5;_push_region(rgn);{struct Cyc_Tc_TreeshakeEnv
_tmp413;struct Cyc_Tc_TreeshakeEnv _tmp2A6=(_tmp413.rgn=rgn,((_tmp413.in_cinclude=
0,((_tmp413.nsdict=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct _tuple17*(*
f)(struct _RegionHandle*,struct Cyc_Tcenv_Genv*),struct _RegionHandle*env,struct Cyc_Dict_Dict
d))Cyc_Dict_rmap_c)(rgn,Cyc_Tc_treeshake_make_env_f,rgn,te->ae),_tmp413)))));
struct Cyc_List_List*_tmp2A7=Cyc_Tc_treeshake_f(& _tmp2A6,ds);if(((int(*)(struct
Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp2A6.nsdict)){struct Cyc_List_List*_tmp2A8=
_tmp2A7;_npop_handler(0);return _tmp2A8;}{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(Cyc_Core_heap_region,
_tmp2A6.nsdict);struct _tuple18 _tmp2A9=*((struct _tuple18*(*)(struct _RegionHandle*,
struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(rgn,_tmp2A6.nsdict);while(((int(*)(
struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(iter,& _tmp2A9)){struct
_tuple17*_tmp2AB;struct _tuple17 _tmp2AC;struct Cyc_Tcenv_Genv*_tmp2AD;struct Cyc_Set_Set*
_tmp2AE;struct _tuple18 _tmp2AA=_tmp2A9;_tmp2AB=_tmp2AA.f2;_tmp2AC=*_tmp2AB;
_tmp2AD=_tmp2AC.f1;_tmp2AE=_tmp2AC.f2;_tmp2AD->ordinaries=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*f)(struct Cyc_Set_Set*,struct _dyneither_ptr*,struct
_tuple13*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(
_tmp2AD->grgn,(int(*)(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,struct
_tuple13*y))Cyc_Tc_treeshake_remove_f,_tmp2AE,_tmp2AD->ordinaries);}{struct Cyc_List_List*
_tmp2AF=_tmp2A7;_npop_handler(0);return _tmp2AF;};};};_pop_region(rgn);}
