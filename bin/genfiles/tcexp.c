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
Cyc_sarg_t;struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct
_dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{
char*tag;struct _dyneither_ptr f1;};typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{
void*v;};typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;
};void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17];
struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};void*Cyc_List_nth(
struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*
x,int i);int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*
r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{struct
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
_tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};typedef struct _tuple1*
Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;typedef struct _tuple1*
Cyc_Absyn_typedef_name_t;typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;
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
_tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int
tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;
struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);
struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;
struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct
_tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;
struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**
val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;
struct _union_AggrInfoU_KnownAggr KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(
struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU
aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;
struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*
zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;};typedef void*Cyc_Absyn_raw_type_decl_t;
struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};typedef struct Cyc_Absyn_TypeDecl*
Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct
Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;
struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct
Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct
Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int
tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo
f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};
struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;
struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;
struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{
int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;
void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{
int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct
Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{
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
struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;
char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct
_union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};
struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct
_union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;
struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct
_dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct
_union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};union Cyc_Absyn_Cnst
Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(
enum Cyc_Absyn_Sign,short);union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,
int);enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus
 = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt
 = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,
Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor
 = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 
18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc
 = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct
_dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;
unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion
 = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{
int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;
int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst
f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;
};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;
struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{
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
int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct
_tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;
int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};
struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{
int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;
};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*
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
struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{
int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*
f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int
tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{
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
int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int
tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;
unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;
struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;
};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual
ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;
struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;
struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind
kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct
_tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};
struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{
struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{
enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{
int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{
void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{
int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{
char*tag;};struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(unsigned int);union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(
void*x);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();void*Cyc_Absyn_conref_val(
union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*
x);void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);extern union
Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*
Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_schar_typ;
extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
void*Cyc_Absyn_float_typ(int);void*Cyc_Absyn_wchar_typ();void*Cyc_Absyn_exn_typ();
extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_star_typ(void*t,
void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_at_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
union Cyc_Absyn_Cnst,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,
struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);struct
Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(
void*,unsigned int);extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int
loc);void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info);int Cyc_strcmp(struct _dyneither_ptr s1,struct
_dyneither_ptr s2);struct Cyc_PP_Ppstate;typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;
struct Cyc_PP_Out;typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;typedef
struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int
qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int
print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int
print_externC_stmts;int print_full_evars;int print_zeroterm;int
generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;typedef struct Cyc_Set_Set*Cyc_Set_set_t;
extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};typedef struct Cyc_Dict_Dict
Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*
tag;};struct Cyc_RgnOrder_RgnPO;typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
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
Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct
Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
unsigned int,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);struct Cyc_Absyn_Datatypedecl**
Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(unsigned int,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(unsigned int,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_copy_type(void*t);
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_kind_leq(
struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(
void*t);void*Cyc_Tcutil_compress(void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,
void*,struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);struct _tuple11{struct Cyc_Absyn_Decl*
f1;struct Cyc_Absyn_Exp*f2;};struct _tuple11 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*
e,void*e_typ);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*
is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_pointer_region(
void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,
void*t,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*,struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);struct _tuple12{struct Cyc_List_List*f1;struct
_RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};struct
_tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);void
Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_type(
unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*
k,int allow_evars,void*);void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_region(void*r,
int must_be_unique);int Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(
struct _RegionHandle*,void*t);struct _tuple14{int f1;void*f2;};struct _tuple14 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_bits_only(void*t);
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_rgn_of_pointer(
void*t,void**rgn);void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct
_tuple15{unsigned int f1;int f2;};struct _tuple15 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_okay_szofarg(void*t);
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int
new_block);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{
struct _tuple0*eq;int isTrue;};typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr msg_part);extern int Cyc_Tcexp_in_stmt_exp;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{
int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{
int tag;int f1;void*f2;};typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 
1,Cyc_CfFlowInfo_AllIL  = 2};typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;
struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple16{struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple16 val;};struct
_union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessConst{int tag;unsigned int val;};struct
_union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct
_union_RelnOp_LessEqConst LessEqConst;};typedef union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_reln_op_t;
struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp
rop;};typedef struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_reln_t;typedef struct Cyc_List_List*
Cyc_CfFlowInfo_relns_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{
char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{
char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*
val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{
struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;typedef void*Cyc_CfFlowInfo_absRval_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;typedef struct _dyneither_ptr
Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int
fieldnum;};typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;
struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{
int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};
struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel
f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel
f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo
f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{
int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct
_union_FlowInfo_BottomFL{int tag;int val;};struct _tuple17{struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple17
val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct
_union_FlowInfo_ReachableFL ReachableFL;};typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;
void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*
esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict
mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};typedef struct Cyc_CfFlowInfo_FlowEnv*
Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{
int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};
struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};typedef void*Cyc_CfFlowInfo_killrgn_t;void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);static void*Cyc_Tcexp_expr_err(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _dyneither_ptr msg,
struct _dyneither_ptr ap){{void*_tmp6CA;(_tmp6CA=0,Cyc_Tcutil_terr(loc,(struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp6CA,
sizeof(void*),0)));}if(topt == 0)return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));else{return*topt;}}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple18*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){
struct _tuple18*_tmp6CB;return(_tmp6CB=_cycalloc(sizeof(*_tmp6CB)),((_tmp6CB->f1=
0,((_tmp6CB->f2=e,_tmp6CB)))));}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*
_tmp2=e1->r;struct _tuple1*_tmp4;void*_tmp5;void**_tmp6;_LL1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2;if(_tmp3->tag != 1)goto
_LL3;else{_tmp4=_tmp3->f1;_tmp5=(void**)& _tmp3->f2;_tmp6=(void**)((void**)& _tmp3->f2);}}
_LL2: {struct _handler_cons _tmp7;_push_handler(& _tmp7);{int _tmp9=0;if(setjmp(
_tmp7.handler))_tmp9=1;if(!_tmp9){{struct _RegionHandle _tmpA=_new_region("r");
struct _RegionHandle*r=& _tmpA;_push_region(r);{void*_tmpB=Cyc_Tcenv_lookup_ordinary(
r,te,e1->loc,_tmp4);void*_tmpD;struct Cyc_Absyn_Datatypedecl*_tmpF;struct Cyc_Absyn_Datatypefield*
_tmp10;struct Cyc_Absyn_Aggrdecl*_tmp12;_LL6: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*
_tmpC=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpB;if(_tmpC->tag != 0)
goto _LL8;else{_tmpD=(void*)_tmpC->f1;}}_LL7:*_tmp6=_tmpD;_npop_handler(1);
return;_LL8: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpE=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)
_tmpB;if(_tmpE->tag != 2)goto _LLA;else{_tmpF=_tmpE->f1;_tmp10=_tmpE->f2;}}_LL9:
if(_tmp10->typs == 0){const char*_tmp6CF;void*_tmp6CE[1];struct Cyc_String_pa_PrintArg_struct
_tmp6CD;(_tmp6CD.tag=0,((_tmp6CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp10->name)),((_tmp6CE[0]=& _tmp6CD,Cyc_Tcutil_terr(e->loc,((
_tmp6CF="%s is a constant, not a function",_tag_dyneither(_tmp6CF,sizeof(char),
33))),_tag_dyneither(_tmp6CE,sizeof(void*),1)))))));}{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct
_tmp6D2;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp6D1;e->r=(void*)((
_tmp6D1=_cycalloc(sizeof(*_tmp6D1)),((_tmp6D1[0]=((_tmp6D2.tag=29,((_tmp6D2.f1=
es,((_tmp6D2.f2=_tmpF,((_tmp6D2.f3=_tmp10,_tmp6D2)))))))),_tmp6D1))));}
_npop_handler(1);return;_LLA: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*
_tmp11=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpB;if(_tmp11->tag != 1)
goto _LLC;else{_tmp12=_tmp11->f1;}}_LLB: {struct Cyc_List_List*_tmp1A=((struct Cyc_List_List*(*)(
struct _tuple18*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcexp_make_struct_arg,es);{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct
_tmp6D5;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp6D4;e->r=(void*)((
_tmp6D4=_cycalloc(sizeof(*_tmp6D4)),((_tmp6D4[0]=((_tmp6D5.tag=27,((_tmp6D5.f1=
_tmp12->name,((_tmp6D5.f2=0,((_tmp6D5.f3=_tmp1A,((_tmp6D5.f4=(struct Cyc_Absyn_Aggrdecl*)
_tmp12,_tmp6D5)))))))))),_tmp6D4))));}_npop_handler(1);return;}_LLC: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*
_tmp13=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpB;if(_tmp13->tag
!= 4)goto _LLE;}_LLD: goto _LLF;_LLE: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*
_tmp14=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpB;if(_tmp14->tag != 3)
goto _LL5;}_LLF:{const char*_tmp6D9;void*_tmp6D8[1];struct Cyc_String_pa_PrintArg_struct
_tmp6D7;(_tmp6D7.tag=0,((_tmp6D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp4)),((_tmp6D8[0]=& _tmp6D7,Cyc_Tcutil_terr(e->loc,((
_tmp6D9="%s is an enum constructor, not a function",_tag_dyneither(_tmp6D9,
sizeof(char),42))),_tag_dyneither(_tmp6D8,sizeof(void*),1)))))));}_npop_handler(
1);return;_LL5:;};_pop_region(r);};_pop_handler();}else{void*_tmp8=(void*)
_exn_thrown;void*_tmp21=_tmp8;_LL11: {struct Cyc_Dict_Absent_exn_struct*_tmp22=(
struct Cyc_Dict_Absent_exn_struct*)_tmp21;if(_tmp22->tag != Cyc_Dict_Absent)goto
_LL13;}_LL12: return;_LL13:;_LL14:(void)_throw(_tmp21);_LL10:;}};}_LL3:;_LL4:
return;_LL0:;}static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,
unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){if(
topt == 0){{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6DC;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*
_tmp6DB;e->r=(void*)((_tmp6DB=_cycalloc(sizeof(*_tmp6DB)),((_tmp6DB[0]=((_tmp6DC.tag=
25,((_tmp6DC.f1=des,_tmp6DC)))),_tmp6DB))));}return;}{void*t=*topt;void*_tmp25=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp27;union Cyc_Absyn_AggrInfoU
_tmp28;struct Cyc_Absyn_ArrayInfo _tmp2A;void*_tmp2B;struct Cyc_Absyn_Tqual _tmp2C;
_LL16: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp26=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmp25;if(_tmp26->tag != 11)goto _LL18;else{_tmp27=_tmp26->f1;_tmp28=_tmp27.aggr_info;}}
_LL17:{union Cyc_Absyn_AggrInfoU _tmp2E=_tmp28;struct _tuple3 _tmp2F;struct Cyc_Absyn_Aggrdecl**
_tmp30;struct Cyc_Absyn_Aggrdecl*_tmp31;_LL1F: if((_tmp2E.UnknownAggr).tag != 1)
goto _LL21;_tmp2F=(struct _tuple3)(_tmp2E.UnknownAggr).val;_LL20: {const char*
_tmp6DF;void*_tmp6DE;(_tmp6DE=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6DF="struct type not properly set",
_tag_dyneither(_tmp6DF,sizeof(char),29))),_tag_dyneither(_tmp6DE,sizeof(void*),0)));}
_LL21: if((_tmp2E.KnownAggr).tag != 2)goto _LL1E;_tmp30=(struct Cyc_Absyn_Aggrdecl**)(
_tmp2E.KnownAggr).val;_tmp31=*_tmp30;_LL22: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct
_tmp6E2;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp6E1;e->r=(void*)((
_tmp6E1=_cycalloc(sizeof(*_tmp6E1)),((_tmp6E1[0]=((_tmp6E2.tag=27,((_tmp6E2.f1=
_tmp31->name,((_tmp6E2.f2=0,((_tmp6E2.f3=des,((_tmp6E2.f4=(struct Cyc_Absyn_Aggrdecl*)
_tmp31,_tmp6E2)))))))))),_tmp6E1))));}_LL1E:;}goto _LL15;_LL18: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp29=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp25;if(_tmp29->tag != 8)
goto _LL1A;else{_tmp2A=_tmp29->f1;_tmp2B=_tmp2A.elt_type;_tmp2C=_tmp2A.tq;}}_LL19:{
struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6E5;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*
_tmp6E4;e->r=(void*)((_tmp6E4=_cycalloc(sizeof(*_tmp6E4)),((_tmp6E4[0]=((_tmp6E5.tag=
25,((_tmp6E5.f1=des,_tmp6E5)))),_tmp6E4))));}goto _LL15;_LL1A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*
_tmp2D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp25;if(_tmp2D->tag != 
12)goto _LL1C;}_LL1B:{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp6E8;
struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp6E7;e->r=(void*)((_tmp6E7=
_cycalloc(sizeof(*_tmp6E7)),((_tmp6E7[0]=((_tmp6E8.tag=28,((_tmp6E8.f1=(void*)t,((
_tmp6E8.f2=des,_tmp6E8)))))),_tmp6E7))));}goto _LL15;_LL1C:;_LL1D:{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct
_tmp6EB;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6EA;e->r=(void*)((
_tmp6EA=_cycalloc(sizeof(*_tmp6EA)),((_tmp6EA[0]=((_tmp6EB.tag=25,((_tmp6EB.f1=
des,_tmp6EB)))),_tmp6EA))));}goto _LL15;_LL15:;};}static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)es->hd);}}struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){Cyc_Tcutil_check_contains_assign(
e);Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(
te,e)){const char*_tmp6F0;void*_tmp6EF[2];struct Cyc_String_pa_PrintArg_struct
_tmp6EE;struct Cyc_String_pa_PrintArg_struct _tmp6ED;(_tmp6ED.tag=0,((_tmp6ED.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)
_check_null(e->topt))),((_tmp6EE.tag=0,((_tmp6EE.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)msg_part),((_tmp6EF[0]=& _tmp6EE,((_tmp6EF[1]=& _tmp6ED,Cyc_Tcutil_terr(
e->loc,((_tmp6F0="test of %s has type %s instead of integral or * type",
_tag_dyneither(_tmp6F0,sizeof(char),53))),_tag_dyneither(_tmp6EF,sizeof(void*),2)))))))))))));}{
void*_tmp40=e->r;enum Cyc_Absyn_Primop _tmp42;struct Cyc_List_List*_tmp43;_LL24: {
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp41=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)
_tmp40;if(_tmp41->tag != 2)goto _LL26;else{_tmp42=_tmp41->f1;_tmp43=_tmp41->f2;}}
_LL25: if(_tmp42 == Cyc_Absyn_Eq  || _tmp42 == Cyc_Absyn_Neq){struct _tuple0 _tmp6F1;
struct _tuple0 _tmp45=(_tmp6F1.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp43))->hd)->topt),((_tmp6F1.f2=(void*)
_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct
Cyc_List_List*)_check_null(_tmp43))->tl))->hd)->topt),_tmp6F1)));void*_tmp46;
void*_tmp48;void*_tmp49;void*_tmp4B;_LL29: _tmp46=_tmp45.f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*
_tmp47=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp46;if(_tmp47->tag != 
15)goto _LL2B;else{_tmp48=(void*)_tmp47->f1;}};_tmp49=_tmp45.f2;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*
_tmp4A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp49;if(_tmp4A->tag != 
15)goto _LL2B;else{_tmp4B=(void*)_tmp4A->f1;}};_LL2A: {struct _tuple0*_tmp6F4;
struct Cyc_Tcexp_TestEnv _tmp6F3;return(_tmp6F3.eq=((_tmp6F4=_region_malloc(Cyc_Tcenv_get_fnrgn(
te),sizeof(*_tmp6F4)),((_tmp6F4->f1=_tmp48,((_tmp6F4->f2=_tmp4B,_tmp6F4)))))),((
_tmp6F3.isTrue=_tmp42 == Cyc_Absyn_Eq,_tmp6F3)));}_LL2B:;_LL2C: goto _LL28;_LL28:;}
goto _LL23;_LL26:;_LL27: goto _LL23;_LL23:;}{struct Cyc_Tcexp_TestEnv _tmp6F5;return(
_tmp6F5.eq=0,((_tmp6F5.isTrue=0,_tmp6F5)));};}static int Cyc_Tcexp_wchar_numelts(
struct _dyneither_ptr s){return 1;}static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*
te,unsigned int loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){void*
t;void*string_elt_typ=Cyc_Absyn_char_typ;int string_numelts=0;{union Cyc_Absyn_Cnst
_tmp4F=*((union Cyc_Absyn_Cnst*)_check_null(c));struct _tuple4 _tmp50;enum Cyc_Absyn_Sign
_tmp51;struct _tuple4 _tmp52;enum Cyc_Absyn_Sign _tmp53;struct _tuple4 _tmp54;enum 
Cyc_Absyn_Sign _tmp55;struct _dyneither_ptr _tmp56;struct _tuple5 _tmp57;enum Cyc_Absyn_Sign
_tmp58;struct _tuple7 _tmp59;enum Cyc_Absyn_Sign _tmp5A;struct _tuple8 _tmp5B;int
_tmp5C;struct _tuple6 _tmp5D;enum Cyc_Absyn_Sign _tmp5E;int _tmp5F;struct
_dyneither_ptr _tmp60;struct _dyneither_ptr _tmp61;int _tmp62;_LL2E: if((_tmp4F.Char_c).tag
!= 2)goto _LL30;_tmp50=(struct _tuple4)(_tmp4F.Char_c).val;_tmp51=_tmp50.f1;if(
_tmp51 != Cyc_Absyn_Signed)goto _LL30;_LL2F: t=Cyc_Absyn_schar_typ;goto _LL2D;_LL30:
if((_tmp4F.Char_c).tag != 2)goto _LL32;_tmp52=(struct _tuple4)(_tmp4F.Char_c).val;
_tmp53=_tmp52.f1;if(_tmp53 != Cyc_Absyn_Unsigned)goto _LL32;_LL31: t=Cyc_Absyn_uchar_typ;
goto _LL2D;_LL32: if((_tmp4F.Char_c).tag != 2)goto _LL34;_tmp54=(struct _tuple4)(
_tmp4F.Char_c).val;_tmp55=_tmp54.f1;if(_tmp55 != Cyc_Absyn_None)goto _LL34;_LL33: t=
Cyc_Absyn_char_typ;goto _LL2D;_LL34: if((_tmp4F.Wchar_c).tag != 3)goto _LL36;_tmp56=(
struct _dyneither_ptr)(_tmp4F.Wchar_c).val;_LL35: t=Cyc_Absyn_wchar_typ();goto
_LL2D;_LL36: if((_tmp4F.Short_c).tag != 4)goto _LL38;_tmp57=(struct _tuple5)(_tmp4F.Short_c).val;
_tmp58=_tmp57.f1;_LL37: t=_tmp58 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;
goto _LL2D;_LL38: if((_tmp4F.LongLong_c).tag != 6)goto _LL3A;_tmp59=(struct _tuple7)(
_tmp4F.LongLong_c).val;_tmp5A=_tmp59.f1;_LL39: t=_tmp5A == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ:
Cyc_Absyn_slonglong_typ;goto _LL2D;_LL3A: if((_tmp4F.Float_c).tag != 7)goto _LL3C;
_tmp5B=(struct _tuple8)(_tmp4F.Float_c).val;_tmp5C=_tmp5B.f2;_LL3B: t=Cyc_Absyn_float_typ(
_tmp5C);goto _LL2D;_LL3C: if((_tmp4F.Int_c).tag != 5)goto _LL3E;_tmp5D=(struct
_tuple6)(_tmp4F.Int_c).val;_tmp5E=_tmp5D.f1;_tmp5F=_tmp5D.f2;_LL3D: if(topt == 0)t=
_tmp5E == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{void*
_tmp63=Cyc_Tcutil_compress(*topt);enum Cyc_Absyn_Sign _tmp65;enum Cyc_Absyn_Size_of
_tmp66;enum Cyc_Absyn_Sign _tmp68;enum Cyc_Absyn_Size_of _tmp69;enum Cyc_Absyn_Sign
_tmp6B;enum Cyc_Absyn_Size_of _tmp6C;enum Cyc_Absyn_Sign _tmp6E;enum Cyc_Absyn_Size_of
_tmp6F;struct Cyc_Absyn_PtrInfo _tmp71;void*_tmp72;struct Cyc_Absyn_Tqual _tmp73;
struct Cyc_Absyn_PtrAtts _tmp74;void*_tmp75;union Cyc_Absyn_Constraint*_tmp76;union
Cyc_Absyn_Constraint*_tmp77;union Cyc_Absyn_Constraint*_tmp78;void*_tmp7A;_LL45: {
struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp64=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp63;if(_tmp64->tag != 6)goto _LL47;else{_tmp65=_tmp64->f1;_tmp66=_tmp64->f2;if(
_tmp66 != Cyc_Absyn_Char_sz)goto _LL47;}}_LL46: switch(_tmp65){case Cyc_Absyn_Unsigned:
_LL53: t=Cyc_Absyn_uchar_typ;break;case Cyc_Absyn_Signed: _LL54: t=Cyc_Absyn_schar_typ;
break;case Cyc_Absyn_None: _LL55: t=Cyc_Absyn_char_typ;break;}*c=Cyc_Absyn_Char_c(
_tmp65,(char)_tmp5F);goto _LL44;_LL47: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp67=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp63;if(_tmp67->tag != 6)
goto _LL49;else{_tmp68=_tmp67->f1;_tmp69=_tmp67->f2;if(_tmp69 != Cyc_Absyn_Short_sz)
goto _LL49;}}_LL48: t=_tmp68 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;*
c=Cyc_Absyn_Short_c(_tmp68,(short)_tmp5F);goto _LL44;_LL49: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp6A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp63;if(_tmp6A->tag != 6)
goto _LL4B;else{_tmp6B=_tmp6A->f1;_tmp6C=_tmp6A->f2;if(_tmp6C != Cyc_Absyn_Int_sz)
goto _LL4B;}}_LL4A: t=_tmp6B == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL44;_LL4B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp63;if(_tmp6D->tag != 6)goto _LL4D;else{_tmp6E=_tmp6D->f1;_tmp6F=_tmp6D->f2;if(
_tmp6F != Cyc_Absyn_Long_sz)goto _LL4D;}}_LL4C: t=_tmp6E == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL44;_LL4D:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp70=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63;if(_tmp70->tag != 5)
goto _LL4F;else{_tmp71=_tmp70->f1;_tmp72=_tmp71.elt_typ;_tmp73=_tmp71.elt_tq;
_tmp74=_tmp71.ptr_atts;_tmp75=_tmp74.rgn;_tmp76=_tmp74.nullable;_tmp77=_tmp74.bounds;
_tmp78=_tmp74.zero_term;}}if(!(_tmp5F == 0))goto _LL4F;_LL4E: {static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct
nullc={0,{.Null_c={1,0}}};e->r=(void*)& nullc;if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(1,_tmp76))return*topt;{struct Cyc_List_List*_tmp7B=
Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Absyn_PointerType_Absyn_Type_struct
_tmp6FF;struct Cyc_Absyn_PtrAtts _tmp6FE;struct Cyc_Absyn_PtrInfo _tmp6FD;struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp6FC;t=(void*)((_tmp6FC=_cycalloc(sizeof(*_tmp6FC)),((_tmp6FC[0]=((_tmp6FF.tag=
5,((_tmp6FF.f1=((_tmp6FD.elt_typ=_tmp72,((_tmp6FD.elt_tq=_tmp73,((_tmp6FD.ptr_atts=((
_tmp6FE.rgn=_tmp75,((_tmp6FE.nullable=Cyc_Absyn_true_conref,((_tmp6FE.bounds=
_tmp77,((_tmp6FE.zero_term=_tmp78,((_tmp6FE.ptrloc=0,_tmp6FE)))))))))),_tmp6FD)))))),
_tmp6FF)))),_tmp6FC))));}goto _LL44;};}_LL4F: {struct Cyc_Absyn_TagType_Absyn_Type_struct*
_tmp79=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp63;if(_tmp79->tag != 19)
goto _LL51;else{_tmp7A=(void*)_tmp79->f1;}}_LL50: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct
_tmp702;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp701;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*
_tmp81=(_tmp701=_cycalloc(sizeof(*_tmp701)),((_tmp701[0]=((_tmp702.tag=18,((
_tmp702.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp5F,0),_tmp702)))),_tmp701)));if(
!Cyc_Tcutil_unify(_tmp7A,(void*)_tmp81)){{const char*_tmp707;void*_tmp706[2];
struct Cyc_String_pa_PrintArg_struct _tmp705;struct Cyc_String_pa_PrintArg_struct
_tmp704;(_tmp704.tag=0,((_tmp704.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_tmp81)),((_tmp705.tag=0,((_tmp705.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp7A)),((_tmp706[
0]=& _tmp705,((_tmp706[1]=& _tmp704,Cyc_Tcutil_terr(loc,((_tmp707="expecting %s but found %s",
_tag_dyneither(_tmp707,sizeof(char),26))),_tag_dyneither(_tmp706,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp70A;
struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp709;t=(void*)((_tmp709=_cycalloc(
sizeof(*_tmp709)),((_tmp709[0]=((_tmp70A.tag=19,((_tmp70A.f1=(void*)((void*)
_tmp81),_tmp70A)))),_tmp709))));}goto _LL44;}_LL51:;_LL52: t=_tmp5E == Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto _LL44;_LL44:;}goto _LL2D;_LL3E: if((
_tmp4F.String_c).tag != 8)goto _LL40;_tmp60=(struct _dyneither_ptr)(_tmp4F.String_c).val;
_LL3F: string_numelts=(int)_get_dyneither_size(_tmp60,sizeof(char));_tmp61=_tmp60;
goto _LL41;_LL40: if((_tmp4F.Wstring_c).tag != 9)goto _LL42;_tmp61=(struct
_dyneither_ptr)(_tmp4F.Wstring_c).val;_LL41: if(string_numelts == 0){
string_numelts=Cyc_Tcexp_wchar_numelts(_tmp61);string_elt_typ=Cyc_Absyn_wchar_typ();}{
struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,
string_numelts),loc);elen->topt=(void*)Cyc_Absyn_uint_typ;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct
_tmp70D;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp70C;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp8A=(_tmp70C=_cycalloc(sizeof(*_tmp70C)),((_tmp70C[0]=((_tmp70D.tag=1,((
_tmp70D.f1=elen,_tmp70D)))),_tmp70C)));t=Cyc_Absyn_atb_typ(string_elt_typ,(void*)&
Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(0),(void*)_tmp8A,Cyc_Absyn_true_conref);
if(topt != 0){void*_tmp8B=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo
_tmp8D;struct Cyc_Absyn_Tqual _tmp8E;_LL58: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp8C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8B;if(_tmp8C->tag != 8)
goto _LL5A;else{_tmp8D=_tmp8C->f1;_tmp8E=_tmp8D.tq;}}_LL59: return Cyc_Absyn_array_typ(
string_elt_typ,_tmp8E,(struct Cyc_Absyn_Exp*)elen,((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),0);_LL5A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp8F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B;if(_tmp8F->tag != 5)
goto _LL5C;}_LL5B: if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,
loc,t,*topt)){e->topt=(void*)t;Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{t=Cyc_Absyn_atb_typ(string_elt_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te)),Cyc_Absyn_const_tqual(0),(
void*)_tmp8A,Cyc_Absyn_true_conref);if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){e->topt=(void*)t;Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}}goto _LL57;_LL5C:;_LL5D: goto _LL57;_LL57:;}return t;};};_LL42: if((_tmp4F.Null_c).tag
!= 1)goto _LL2D;_tmp62=(int)(_tmp4F.Null_c).val;_LL43: if(topt != 0){void*_tmp92=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp94;void*_tmp95;struct Cyc_Absyn_Tqual
_tmp96;struct Cyc_Absyn_PtrAtts _tmp97;void*_tmp98;union Cyc_Absyn_Constraint*
_tmp99;union Cyc_Absyn_Constraint*_tmp9A;union Cyc_Absyn_Constraint*_tmp9B;_LL5F: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp93=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmp92;if(_tmp93->tag != 5)goto _LL61;else{_tmp94=_tmp93->f1;_tmp95=_tmp94.elt_typ;
_tmp96=_tmp94.elt_tq;_tmp97=_tmp94.ptr_atts;_tmp98=_tmp97.rgn;_tmp99=_tmp97.nullable;
_tmp9A=_tmp97.bounds;_tmp9B=_tmp97.zero_term;}}_LL60: if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(1,_tmp99))return*topt;{struct Cyc_Absyn_PointerType_Absyn_Type_struct
_tmp717;struct Cyc_Absyn_PtrAtts _tmp716;struct Cyc_Absyn_PtrInfo _tmp715;struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp714;return(void*)((_tmp714=_cycalloc(sizeof(*_tmp714)),((_tmp714[0]=((
_tmp717.tag=5,((_tmp717.f1=((_tmp715.elt_typ=_tmp95,((_tmp715.elt_tq=_tmp96,((
_tmp715.ptr_atts=((_tmp716.rgn=_tmp98,((_tmp716.nullable=Cyc_Absyn_true_conref,((
_tmp716.bounds=_tmp9A,((_tmp716.zero_term=_tmp9B,((_tmp716.ptrloc=0,_tmp716)))))))))),
_tmp715)))))),_tmp717)))),_tmp714))));};_LL61:;_LL62: goto _LL5E;_LL5E:;}{struct
Cyc_List_List*_tmpA0=Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Absyn_PointerType_Absyn_Type_struct
_tmp72A;struct Cyc_Absyn_PtrAtts _tmp729;struct Cyc_Core_Opt*_tmp728;struct Cyc_Core_Opt*
_tmp727;struct Cyc_Absyn_PtrInfo _tmp726;struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp725;t=(void*)((_tmp725=_cycalloc(sizeof(*_tmp725)),((_tmp725[0]=((_tmp72A.tag=
5,((_tmp72A.f1=((_tmp726.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tako,((
_tmp727=_cycalloc(sizeof(*_tmp727)),((_tmp727->v=_tmpA0,_tmp727))))),((_tmp726.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp726.ptr_atts=((_tmp729.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((_tmp728=_cycalloc(sizeof(*_tmp728)),((
_tmp728->v=_tmpA0,_tmp728))))),((_tmp729.nullable=Cyc_Absyn_true_conref,((
_tmp729.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp729.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp729.ptrloc=0,_tmp729)))))))))),_tmp726)))))),_tmp72A)))),_tmp725))));}goto
_LL2D;};_LL2D:;}return t;}static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,
unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct
Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf);static void*Cyc_Tcexp_tcVar(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct
_tuple1*q,void**b){void*_tmpA7=*((void**)_check_null(b));struct Cyc_Absyn_Vardecl*
_tmpAA;struct Cyc_Absyn_Fndecl*_tmpAC;struct Cyc_Absyn_Vardecl*_tmpAE;struct Cyc_Absyn_Vardecl*
_tmpB0;struct Cyc_Absyn_Vardecl*_tmpB2;_LL64: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*
_tmpA8=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpA7;if(_tmpA8->tag
!= 0)goto _LL66;}_LL65: {struct _handler_cons _tmpB3;_push_handler(& _tmpB3);{int
_tmpB5=0;if(setjmp(_tmpB3.handler))_tmpB5=1;if(!_tmpB5){{struct _RegionHandle
_tmpB6=_new_region("r");struct _RegionHandle*r=& _tmpB6;_push_region(r);{void*
_tmpB7=Cyc_Tcenv_lookup_ordinary(r,te,e->loc,q);void*_tmpB9;struct Cyc_Absyn_Enumdecl*
_tmpBB;struct Cyc_Absyn_Enumfield*_tmpBC;void*_tmpBE;struct Cyc_Absyn_Enumfield*
_tmpBF;struct Cyc_Absyn_Datatypedecl*_tmpC1;struct Cyc_Absyn_Datatypefield*_tmpC2;
_LL71: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpB8=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)
_tmpB7;if(_tmpB8->tag != 0)goto _LL73;else{_tmpB9=(void*)_tmpB8->f1;}}_LL72:*b=
_tmpB9;{void*_tmpC4=Cyc_Tcexp_tcVar(te,loc,topt,e,q,b);_npop_handler(1);return
_tmpC4;};_LL73: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpBA=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)
_tmpB7;if(_tmpBA->tag != 3)goto _LL75;else{_tmpBB=_tmpBA->f1;_tmpBC=_tmpBA->f2;}}
_LL74:{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp72D;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*
_tmp72C;e->r=(void*)((_tmp72C=_cycalloc(sizeof(*_tmp72C)),((_tmp72C[0]=((_tmp72D.tag=
30,((_tmp72D.f1=_tmpBC->name,((_tmp72D.f2=(struct Cyc_Absyn_Enumdecl*)_tmpBB,((
_tmp72D.f3=(struct Cyc_Absyn_Enumfield*)_tmpBC,_tmp72D)))))))),_tmp72C))));}{
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp730;struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp72F;void*_tmpC9=(void*)((_tmp72F=_cycalloc(sizeof(*_tmp72F)),((_tmp72F[0]=((
_tmp730.tag=13,((_tmp730.f1=_tmpBB->name,((_tmp730.f2=(struct Cyc_Absyn_Enumdecl*)
_tmpBB,_tmp730)))))),_tmp72F))));_npop_handler(1);return _tmpC9;};_LL75: {struct
Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpBD=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)
_tmpB7;if(_tmpBD->tag != 4)goto _LL77;else{_tmpBE=(void*)_tmpBD->f1;_tmpBF=_tmpBD->f2;}}
_LL76:{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp733;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*
_tmp732;e->r=(void*)((_tmp732=_cycalloc(sizeof(*_tmp732)),((_tmp732[0]=((_tmp733.tag=
31,((_tmp733.f1=_tmpBF->name,((_tmp733.f2=(void*)_tmpBE,((_tmp733.f3=(struct Cyc_Absyn_Enumfield*)
_tmpBF,_tmp733)))))))),_tmp732))));}{void*_tmpCC=_tmpBE;_npop_handler(1);return
_tmpCC;};_LL77: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpC0=(
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpB7;if(_tmpC0->tag != 2)
goto _LL79;else{_tmpC1=_tmpC0->f1;_tmpC2=_tmpC0->f2;}}_LL78:{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct
_tmp736;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp735;e->r=(void*)((
_tmp735=_cycalloc(sizeof(*_tmp735)),((_tmp735[0]=((_tmp736.tag=29,((_tmp736.f1=0,((
_tmp736.f2=_tmpC1,((_tmp736.f3=_tmpC2,_tmp736)))))))),_tmp735))));}{void*_tmpCF=
Cyc_Tcexp_tcDatatype(te,loc,topt,e,0,_tmpC1,_tmpC2);_npop_handler(1);return
_tmpCF;};_LL79: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpC3=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)
_tmpB7;if(_tmpC3->tag != 1)goto _LL70;}_LL7A: {const char*_tmp73A;void*_tmp739[1];
struct Cyc_String_pa_PrintArg_struct _tmp738;void*_tmpD3=(_tmp738.tag=0,((_tmp738.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((
_tmp739[0]=& _tmp738,Cyc_Tcexp_expr_err(te,loc,0,((_tmp73A="bad occurrence of type name %s",
_tag_dyneither(_tmp73A,sizeof(char),31))),_tag_dyneither(_tmp739,sizeof(void*),1)))))));
_npop_handler(1);return _tmpD3;}_LL70:;};_pop_region(r);};_pop_handler();}else{
void*_tmpB4=(void*)_exn_thrown;void*_tmpD5=_tmpB4;_LL7C: {struct Cyc_Dict_Absent_exn_struct*
_tmpD6=(struct Cyc_Dict_Absent_exn_struct*)_tmpD5;if(_tmpD6->tag != Cyc_Dict_Absent)
goto _LL7E;}_LL7D: {const char*_tmp73E;void*_tmp73D[1];struct Cyc_String_pa_PrintArg_struct
_tmp73C;return(_tmp73C.tag=0,((_tmp73C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp73D[0]=& _tmp73C,Cyc_Tcexp_expr_err(
te,loc,0,((_tmp73E="undeclared identifier: %s",_tag_dyneither(_tmp73E,sizeof(
char),26))),_tag_dyneither(_tmp73D,sizeof(void*),1)))))));}_LL7E:;_LL7F:(void)
_throw(_tmpD5);_LL7B:;}};}_LL66: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmpA9=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpA7;if(_tmpA9->tag != 1)
goto _LL68;else{_tmpAA=_tmpA9->f1;}}_LL67:*q=*_tmpAA->name;return _tmpAA->type;
_LL68: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpAB=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)
_tmpA7;if(_tmpAB->tag != 2)goto _LL6A;else{_tmpAC=_tmpAB->f1;}}_LL69:*q=*_tmpAC->name;
return Cyc_Tcutil_fndecl2typ(_tmpAC);_LL6A: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*
_tmpAD=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpA7;if(_tmpAD->tag != 5)
goto _LL6C;else{_tmpAE=_tmpAD->f1;}}_LL6B: _tmpB0=_tmpAE;goto _LL6D;_LL6C: {struct
Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpAF=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)
_tmpA7;if(_tmpAF->tag != 4)goto _LL6E;else{_tmpB0=_tmpAF->f1;}}_LL6D: _tmpB2=_tmpB0;
goto _LL6F;_LL6E: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpB1=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)
_tmpA7;if(_tmpB1->tag != 3)goto _LL63;else{_tmpB2=_tmpB1->f1;}}_LL6F:{union Cyc_Absyn_Nmspace
_tmp73F;(*q).f1=(((_tmp73F.Loc_n).val=0,(((_tmp73F.Loc_n).tag=4,_tmp73F))));}if(
te->allow_valueof){void*_tmpDB=Cyc_Tcutil_compress(_tmpB2->type);void*_tmpDD;
_LL81: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpDC=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)
_tmpDB;if(_tmpDC->tag != 19)goto _LL83;else{_tmpDD=(void*)_tmpDC->f1;}}_LL82:{
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp742;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*
_tmp741;e->r=(void*)((_tmp741=_cycalloc(sizeof(*_tmp741)),((_tmp741[0]=((_tmp742.tag=
37,((_tmp742.f1=(void*)_tmpDD,_tmp742)))),_tmp741))));}goto _LL80;_LL83:;_LL84:
goto _LL80;_LL80:;}return _tmpB2->type;_LL63:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int
arg_cnt,struct Cyc_List_List**alias_arg_exps,struct _RegionHandle*temp,struct Cyc_List_List*(*
type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,
unsigned int)){struct Cyc_List_List*desc_types;{void*_tmpE0=fmt->r;union Cyc_Absyn_Cnst
_tmpE2;struct _dyneither_ptr _tmpE3;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_Absyn_Exp
_tmpE6;void*_tmpE7;union Cyc_Absyn_Cnst _tmpE9;struct _dyneither_ptr _tmpEA;_LL86: {
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmpE0;if(_tmpE1->tag != 0)goto _LL88;else{_tmpE2=_tmpE1->f1;if((_tmpE2.String_c).tag
!= 8)goto _LL88;_tmpE3=(struct _dyneither_ptr)(_tmpE2.String_c).val;}}_LL87: _tmpEA=
_tmpE3;goto _LL89;_LL88: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpE4=(
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpE4->tag != 13)goto
_LL8A;else{_tmpE5=_tmpE4->f2;_tmpE6=*_tmpE5;_tmpE7=_tmpE6.r;{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmpE8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE7;if(_tmpE8->tag != 0)
goto _LL8A;else{_tmpE9=_tmpE8->f1;if((_tmpE9.String_c).tag != 8)goto _LL8A;_tmpEA=(
struct _dyneither_ptr)(_tmpE9.String_c).val;}};}}_LL89: desc_types=type_getter(
temp,te,(struct _dyneither_ptr)_tmpEA,fmt->loc);goto _LL85;_LL8A:;_LL8B: if(
opt_args != 0){struct Cyc_List_List*_tmpEB=(struct Cyc_List_List*)opt_args->v;for(0;
_tmpEB != 0;_tmpEB=_tmpEB->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmpEB->hd);{
struct _RegionHandle _tmpEC=_new_region("temp");struct _RegionHandle*temp=& _tmpEC;
_push_region(temp);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)
_check_null(((struct Cyc_Absyn_Exp*)_tmpEB->hd)->topt)) && !Cyc_Tcutil_is_noalias_path(
temp,(struct Cyc_Absyn_Exp*)_tmpEB->hd)){const char*_tmp745;void*_tmp744;(_tmp744=
0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpEB->hd)->loc,((_tmp745="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp745,sizeof(char),49))),_tag_dyneither(_tmp744,sizeof(void*),0)));};
_pop_region(temp);};}}return;_LL85:;}if(opt_args != 0){struct Cyc_List_List*_tmpEF=(
struct Cyc_List_List*)opt_args->v;for(0;desc_types != 0  && _tmpEF != 0;(((
desc_types=desc_types->tl,_tmpEF=_tmpEF->tl)),arg_cnt ++)){int alias_coercion=0;
void*t=(void*)desc_types->hd;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpEF->hd;
Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t,&
alias_coercion)){{const char*_tmp74A;void*_tmp749[2];struct Cyc_String_pa_PrintArg_struct
_tmp748;struct Cyc_String_pa_PrintArg_struct _tmp747;(_tmp747.tag=0,((_tmp747.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)
_check_null(e->topt))),((_tmp748.tag=0,((_tmp748.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp749[0]=& _tmp748,((_tmp749[
1]=& _tmp747,Cyc_Tcutil_terr(e->loc,((_tmp74A="descriptor has type \n%s\n but argument has type \n%s",
_tag_dyneither(_tmp74A,sizeof(char),51))),_tag_dyneither(_tmp749,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(alias_coercion){struct Cyc_List_List*_tmp74B;*
alias_arg_exps=(struct Cyc_List_List*)((_tmp74B=_cycalloc(sizeof(*_tmp74B)),((
_tmp74B->hd=(void*)arg_cnt,((_tmp74B->tl=*alias_arg_exps,_tmp74B))))));}{struct
_RegionHandle _tmpF5=_new_region("temp");struct _RegionHandle*temp=& _tmpF5;
_push_region(temp);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(
temp,e)){const char*_tmp74E;void*_tmp74D;(_tmp74D=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmpEF->hd)->loc,((_tmp74E="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp74E,sizeof(char),49))),_tag_dyneither(_tmp74D,sizeof(void*),0)));};
_pop_region(temp);};}if(desc_types != 0){const char*_tmp751;void*_tmp750;(_tmp750=
0,Cyc_Tcutil_terr(fmt->loc,((_tmp751="too few arguments",_tag_dyneither(_tmp751,
sizeof(char),18))),_tag_dyneither(_tmp750,sizeof(void*),0)));}if(_tmpEF != 0){
const char*_tmp754;void*_tmp753;(_tmp753=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmpEF->hd)->loc,((_tmp754="too many arguments",_tag_dyneither(_tmp754,sizeof(
char),19))),_tag_dyneither(_tmp753,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct
Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));switch(
p){case Cyc_Absyn_Plus: _LL8C: goto _LL8D;case Cyc_Absyn_Minus: _LL8D: if(!Cyc_Tcutil_is_numeric(
e)){const char*_tmp758;void*_tmp757[1];struct Cyc_String_pa_PrintArg_struct _tmp756;(
_tmp756.tag=0,((_tmp756.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp757[0]=& _tmp756,Cyc_Tcutil_terr(loc,((_tmp758="expecting arithmetic type but found %s",
_tag_dyneither(_tmp758,sizeof(char),39))),_tag_dyneither(_tmp757,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL8E: Cyc_Tcutil_check_contains_assign(
e);if(!Cyc_Tcutil_coerce_to_bool(te,e)){const char*_tmp75C;void*_tmp75B[1];struct
Cyc_String_pa_PrintArg_struct _tmp75A;(_tmp75A.tag=0,((_tmp75A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp75B[0]=&
_tmp75A,Cyc_Tcutil_terr(loc,((_tmp75C="expecting integral or * type but found %s",
_tag_dyneither(_tmp75C,sizeof(char),42))),_tag_dyneither(_tmp75B,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL8F: if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp760;void*_tmp75F[1];struct Cyc_String_pa_PrintArg_struct _tmp75E;(
_tmp75E.tag=0,((_tmp75E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp75F[0]=& _tmp75E,Cyc_Tcutil_terr(loc,((_tmp760="expecting integral type but found %s",
_tag_dyneither(_tmp760,sizeof(char),37))),_tag_dyneither(_tmp75F,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL90:{void*_tmp105=t;
struct Cyc_Absyn_PtrInfo _tmp108;struct Cyc_Absyn_PtrAtts _tmp109;union Cyc_Absyn_Constraint*
_tmp10A;_LL93: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp106=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)
_tmp105;if(_tmp106->tag != 8)goto _LL95;}_LL94: goto _LL92;_LL95: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp107=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp105;if(_tmp107->tag
!= 5)goto _LL97;else{_tmp108=_tmp107->f1;_tmp109=_tmp108.ptr_atts;_tmp10A=_tmp109.bounds;}}
_LL96:{void*_tmp10B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((
void*)& Cyc_Absyn_DynEither_b_val,_tmp10A);struct Cyc_Absyn_Exp*_tmp10E;_LL9A: {
struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp10C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)
_tmp10B;if(_tmp10C->tag != 0)goto _LL9C;}_LL9B: goto _LL99;_LL9C: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp10D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp10B;if(_tmp10D->tag != 
1)goto _LL99;else{_tmp10E=_tmp10D->f1;}}_LL9D: if(!Cyc_Evexp_c_can_eval(_tmp10E)){
const char*_tmp763;void*_tmp762;(_tmp762=0,Cyc_Tcutil_terr(loc,((_tmp763="cannot apply numelts to a pointer with abstract bounds",
_tag_dyneither(_tmp763,sizeof(char),55))),_tag_dyneither(_tmp762,sizeof(void*),0)));}
goto _LL99;_LL99:;}goto _LL92;_LL97:;_LL98: {const char*_tmp767;void*_tmp766[1];
struct Cyc_String_pa_PrintArg_struct _tmp765;(_tmp765.tag=0,((_tmp765.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp766[0]=&
_tmp765,Cyc_Tcutil_terr(loc,((_tmp767="numelts requires pointer or array type, not %s",
_tag_dyneither(_tmp767,sizeof(char),47))),_tag_dyneither(_tmp766,sizeof(void*),1)))))));}
_LL92:;}return Cyc_Absyn_uint_typ;default: _LL91: {const char*_tmp76A;void*_tmp769;(
_tmp769=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp76A="Non-unary primop",_tag_dyneither(_tmp76A,sizeof(char),17))),
_tag_dyneither(_tmp769,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){{const char*_tmp76E;void*_tmp76D[
1];struct Cyc_String_pa_PrintArg_struct _tmp76C;(_tmp76C.tag=0,((_tmp76C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)
_check_null(e1->topt))),((_tmp76D[0]=& _tmp76C,Cyc_Tcutil_terr(e1->loc,((_tmp76E="type %s cannot be used here",
_tag_dyneither(_tmp76E,sizeof(char),28))),_tag_dyneither(_tmp76D,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}if(!checker(e2)){{
const char*_tmp772;void*_tmp771[1];struct Cyc_String_pa_PrintArg_struct _tmp770;(
_tmp770.tag=0,((_tmp770.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)_check_null(e2->topt))),((_tmp771[0]=& _tmp770,Cyc_Tcutil_terr(e2->loc,((
_tmp772="type %s cannot be used here",_tag_dyneither(_tmp772,sizeof(char),28))),
_tag_dyneither(_tmp771,sizeof(void*),1)))))));}return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}{void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*t2=Cyc_Tcutil_compress((
void*)_check_null(e2->topt));return Cyc_Tcutil_max_arithmetic_type(t1,t2);};}
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));void*_tmp11C=t1;struct
Cyc_Absyn_PtrInfo _tmp11E;void*_tmp11F;struct Cyc_Absyn_Tqual _tmp120;struct Cyc_Absyn_PtrAtts
_tmp121;void*_tmp122;union Cyc_Absyn_Constraint*_tmp123;union Cyc_Absyn_Constraint*
_tmp124;union Cyc_Absyn_Constraint*_tmp125;_LLA0: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp11D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11C;if(_tmp11D->tag
!= 5)goto _LLA2;else{_tmp11E=_tmp11D->f1;_tmp11F=_tmp11E.elt_typ;_tmp120=_tmp11E.elt_tq;
_tmp121=_tmp11E.ptr_atts;_tmp122=_tmp121.rgn;_tmp123=_tmp121.nullable;_tmp124=
_tmp121.bounds;_tmp125=_tmp121.zero_term;}}_LLA1: if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
_tmp11F),& Cyc_Tcutil_tmk)){const char*_tmp775;void*_tmp774;(_tmp774=0,Cyc_Tcutil_terr(
e1->loc,((_tmp775="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp775,sizeof(char),50))),_tag_dyneither(_tmp774,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp778;void*_tmp777;(_tmp777=0,
Cyc_Tcutil_terr(e1->loc,((_tmp778="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp778,sizeof(char),54))),_tag_dyneither(_tmp777,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){const char*_tmp77C;void*_tmp77B[1];struct
Cyc_String_pa_PrintArg_struct _tmp77A;(_tmp77A.tag=0,((_tmp77A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp77B[0]=&
_tmp77A,Cyc_Tcutil_terr(e2->loc,((_tmp77C="expecting int but found %s",
_tag_dyneither(_tmp77C,sizeof(char),27))),_tag_dyneither(_tmp77B,sizeof(void*),1)))))));}{
void*_tmp12D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((
void*)& Cyc_Absyn_DynEither_b_val,_tmp124);struct Cyc_Absyn_Exp*_tmp130;_LLA5: {
struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp12E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)
_tmp12D;if(_tmp12E->tag != 0)goto _LLA7;}_LLA6: return t1;_LLA7: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp12F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp12D;if(_tmp12F->tag != 
1)goto _LLA4;else{_tmp130=_tmp12F->f1;}}_LLA8: if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp125)){const char*_tmp77F;void*_tmp77E;(_tmp77E=0,
Cyc_Tcutil_warn(e1->loc,((_tmp77F="pointer arithmetic on thin, zero-terminated pointer may be expensive.",
_tag_dyneither(_tmp77F,sizeof(char),70))),_tag_dyneither(_tmp77E,sizeof(void*),0)));}{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp789;struct Cyc_Absyn_PtrAtts
_tmp788;struct Cyc_Absyn_PtrInfo _tmp787;struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp786;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp133=(_tmp786=_cycalloc(
sizeof(*_tmp786)),((_tmp786[0]=((_tmp789.tag=5,((_tmp789.f1=((_tmp787.elt_typ=
_tmp11F,((_tmp787.elt_tq=_tmp120,((_tmp787.ptr_atts=((_tmp788.rgn=_tmp122,((
_tmp788.nullable=Cyc_Absyn_true_conref,((_tmp788.bounds=Cyc_Absyn_bounds_dyneither_conref,((
_tmp788.zero_term=_tmp125,((_tmp788.ptrloc=0,_tmp788)))))))))),_tmp787)))))),
_tmp789)))),_tmp786)));Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmp133,Cyc_Absyn_Other_coercion);
return(void*)_tmp133;};_LLA4:;};_LLA2:;_LLA3: return Cyc_Tcexp_tcArithBinop(te,e1,
e2,Cyc_Tcutil_is_numeric);_LL9F:;}static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);void*t1_elt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(Cyc_Tcutil_is_tagged_pointer_typ(
t2)){if(!Cyc_Tcutil_unify(t1,t2)){{const char*_tmp78E;void*_tmp78D[2];struct Cyc_String_pa_PrintArg_struct
_tmp78C;struct Cyc_String_pa_PrintArg_struct _tmp78B;(_tmp78B.tag=0,((_tmp78B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((
_tmp78C.tag=0,((_tmp78C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp78D[0]=& _tmp78C,((_tmp78D[1]=& _tmp78B,Cyc_Tcutil_terr(e1->loc,((
_tmp78E="pointer arithmetic on values of different types (%s != %s)",
_tag_dyneither(_tmp78E,sizeof(char),59))),_tag_dyneither(_tmp78D,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}else{if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){const char*_tmp791;void*_tmp790;(
_tmp790=0,Cyc_Tcutil_terr(e1->loc,((_tmp791="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp791,sizeof(char),50))),_tag_dyneither(_tmp790,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp794;void*_tmp793;(_tmp793=0,
Cyc_Tcutil_terr(e1->loc,((_tmp794="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp794,sizeof(char),54))),_tag_dyneither(_tmp793,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){{const char*_tmp799;void*_tmp798[2];struct
Cyc_String_pa_PrintArg_struct _tmp797;struct Cyc_String_pa_PrintArg_struct _tmp796;(
_tmp796.tag=0,((_tmp796.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp797.tag=0,((_tmp797.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp798[0]=& _tmp797,((_tmp798[1]=& _tmp796,Cyc_Tcutil_terr(
e2->loc,((_tmp799="expecting either %s or int but found %s",_tag_dyneither(
_tmp799,sizeof(char),40))),_tag_dyneither(_tmp798,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return t1;}}if(Cyc_Tcutil_is_pointer_type(t1))Cyc_Tcutil_unchecked_cast(
te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);if(Cyc_Tcutil_is_pointer_type(
t2))Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}static void*Cyc_Tcexp_tcAnyBinop(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){int e1_is_num=Cyc_Tcutil_is_numeric(e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*t2=Cyc_Tcutil_compress((
void*)_check_null(e2->topt));if(e1_is_num  && e2_is_num)return Cyc_Absyn_sint_typ;
else{if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind  || Cyc_Tcutil_unify(
t1,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(
te)))){if(Cyc_Tcutil_unify(t1,t2))return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_silent_castable(
te,loc,t2,t1)){Cyc_Tcutil_unchecked_cast(te,e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e1,t2,Cyc_Absyn_Other_coercion);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_zero_to_null(
te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))return Cyc_Absyn_sint_typ;else{
goto pointer_cmp;}}}}}else{pointer_cmp: {struct _tuple0 _tmp79A;struct _tuple0
_tmp145=(_tmp79A.f1=Cyc_Tcutil_compress(t1),((_tmp79A.f2=Cyc_Tcutil_compress(t2),
_tmp79A)));void*_tmp146;struct Cyc_Absyn_PtrInfo _tmp148;void*_tmp149;void*_tmp14A;
struct Cyc_Absyn_PtrInfo _tmp14C;void*_tmp14D;void*_tmp14E;void*_tmp150;_LLAA:
_tmp146=_tmp145.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp147=(
struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp146;if(_tmp147->tag != 5)goto
_LLAC;else{_tmp148=_tmp147->f1;_tmp149=_tmp148.elt_typ;}};_tmp14A=_tmp145.f2;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp14B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmp14A;if(_tmp14B->tag != 5)goto _LLAC;else{_tmp14C=_tmp14B->f1;_tmp14D=_tmp14C.elt_typ;}};
_LLAB: if(Cyc_Tcutil_unify(_tmp149,_tmp14D))return Cyc_Absyn_sint_typ;goto _LLA9;
_LLAC: _tmp14E=_tmp145.f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*
_tmp14F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp14E;if(_tmp14F->tag
!= 15)goto _LLAE;};_tmp150=_tmp145.f2;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*
_tmp151=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp150;if(_tmp151->tag
!= 15)goto _LLAE;};_LLAD: return Cyc_Absyn_sint_typ;_LLAE:;_LLAF: goto _LLA9;_LLA9:;}{
const char*_tmp79F;void*_tmp79E[2];struct Cyc_String_pa_PrintArg_struct _tmp79D;
struct Cyc_String_pa_PrintArg_struct _tmp79C;(_tmp79C.tag=0,((_tmp79C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp79D.tag=
0,((_tmp79D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp79E[0]=& _tmp79D,((_tmp79E[1]=& _tmp79C,Cyc_Tcutil_terr(loc,((_tmp79F="comparison not allowed between %s and %s",
_tag_dyneither(_tmp79F,sizeof(char),41))),_tag_dyneither(_tmp79E,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}}static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){if(Cyc_Tcexp_tcAnyBinop(te,loc,e1,
e2)== Cyc_Absyn_sint_typ)return Cyc_Absyn_sint_typ;{void*t1=Cyc_Tcutil_compress((
void*)_check_null(e1->topt));void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));{
struct _tuple0 _tmp7A0;struct _tuple0 _tmp157=(_tmp7A0.f1=t1,((_tmp7A0.f2=t2,_tmp7A0)));
void*_tmp158;void*_tmp15A;void*_tmp15B;void*_tmp15D;_LLB1: _tmp158=_tmp157.f1;{
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp159=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)
_tmp158;if(_tmp159->tag != 15)goto _LLB3;else{_tmp15A=(void*)_tmp159->f1;}};
_tmp15B=_tmp157.f2;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp15C=(
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp15B;if(_tmp15C->tag != 15)
goto _LLB3;else{_tmp15D=(void*)_tmp15C->f1;}};_LLB2: return Cyc_Absyn_sint_typ;
_LLB3:;_LLB4: goto _LLB0;_LLB0:;}{const char*_tmp7A5;void*_tmp7A4[2];struct Cyc_String_pa_PrintArg_struct
_tmp7A3;struct Cyc_String_pa_PrintArg_struct _tmp7A2;(_tmp7A2.tag=0,((_tmp7A2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((
_tmp7A3.tag=0,((_tmp7A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp7A4[0]=& _tmp7A3,((_tmp7A4[1]=& _tmp7A2,Cyc_Tcutil_terr(loc,((_tmp7A5="comparison not allowed between %s and %s",
_tag_dyneither(_tmp7A5,sizeof(char),41))),_tag_dyneither(_tmp7A4,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));};}static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,
void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
switch(p){case Cyc_Absyn_Plus: _LLB5: return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus:
_LLB6: return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LLB7: goto _LLB8;case
Cyc_Absyn_Div: _LLB8: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
case Cyc_Absyn_Mod: _LLB9: goto _LLBA;case Cyc_Absyn_Bitand: _LLBA: goto _LLBB;case Cyc_Absyn_Bitor:
_LLBB: goto _LLBC;case Cyc_Absyn_Bitxor: _LLBC: goto _LLBD;case Cyc_Absyn_Bitlshift:
_LLBD: goto _LLBE;case Cyc_Absyn_Bitlrshift: _LLBE: goto _LLBF;case Cyc_Absyn_Bitarshift:
_LLBF: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq:
_LLC0: goto _LLC1;case Cyc_Absyn_Neq: _LLC1: return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);
case Cyc_Absyn_Gt: _LLC2: goto _LLC3;case Cyc_Absyn_Lt: _LLC3: goto _LLC4;case Cyc_Absyn_Gte:
_LLC4: goto _LLC5;case Cyc_Absyn_Lte: _LLC5: return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);
default: _LLC6: {const char*_tmp7A8;void*_tmp7A7;(_tmp7A7=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7A8="bad binary primop",
_tag_dyneither(_tmp7A8,sizeof(char),18))),_tag_dyneither(_tmp7A7,sizeof(void*),0)));}}}
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,
enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){if(p == Cyc_Absyn_Minus  && ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(es)== 1)return Cyc_Tcexp_tcExp(te,topt,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(
te,es);{void*t;switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case
0: _LLC8: {const char*_tmp7AB;void*_tmp7AA;return(_tmp7AA=0,Cyc_Tcexp_expr_err(te,
loc,topt,((_tmp7AB="primitive operator has 0 arguments",_tag_dyneither(_tmp7AB,
sizeof(char),35))),_tag_dyneither(_tmp7AA,sizeof(void*),0)));}case 1: _LLC9: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLCA: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLCB: {const char*_tmp7AE;void*_tmp7AD;return(_tmp7AD=0,Cyc_Tcexp_expr_err(te,
loc,topt,((_tmp7AE="primitive operator has > 2 arguments",_tag_dyneither(_tmp7AE,
sizeof(char),37))),_tag_dyneither(_tmp7AD,sizeof(void*),0)));}}return t;};}struct
_tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};static int Cyc_Tcexp_check_writable_aggr(
unsigned int loc,void*t){void*_tmp168=Cyc_Tcutil_compress(t);void*_tmp169=_tmp168;
struct Cyc_Absyn_AggrInfo _tmp16B;union Cyc_Absyn_AggrInfoU _tmp16C;struct Cyc_Absyn_Aggrdecl**
_tmp16D;struct Cyc_Absyn_Aggrdecl*_tmp16E;struct Cyc_List_List*_tmp170;struct Cyc_Absyn_DatatypeFieldInfo
_tmp172;union Cyc_Absyn_DatatypeFieldInfoU _tmp173;struct _tuple2 _tmp174;struct Cyc_Absyn_Datatypefield*
_tmp175;struct Cyc_Absyn_ArrayInfo _tmp177;void*_tmp178;struct Cyc_Absyn_Tqual
_tmp179;struct Cyc_List_List*_tmp17B;_LLCE: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp16A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp169;if(_tmp16A->tag != 
11)goto _LLD0;else{_tmp16B=_tmp16A->f1;_tmp16C=_tmp16B.aggr_info;if((_tmp16C.KnownAggr).tag
!= 2)goto _LLD0;_tmp16D=(struct Cyc_Absyn_Aggrdecl**)(_tmp16C.KnownAggr).val;
_tmp16E=*_tmp16D;}}_LLCF: _tmp170=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp16E->impl))->fields;goto _LLD1;_LLD0: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*
_tmp16F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp169;if(_tmp16F->tag
!= 12)goto _LLD2;else{_tmp170=_tmp16F->f2;}}_LLD1: for(0;_tmp170 != 0;_tmp170=
_tmp170->tl){struct Cyc_Absyn_Aggrfield*_tmp17C=(struct Cyc_Absyn_Aggrfield*)
_tmp170->hd;if((_tmp17C->tq).real_const){{const char*_tmp7B2;void*_tmp7B1[1];
struct Cyc_String_pa_PrintArg_struct _tmp7B0;(_tmp7B0.tag=0,((_tmp7B0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp17C->name),((_tmp7B1[0]=& _tmp7B0,Cyc_Tcutil_terr(
loc,((_tmp7B2="attempt to over-write an aggregate with const member %s",
_tag_dyneither(_tmp7B2,sizeof(char),56))),_tag_dyneither(_tmp7B1,sizeof(void*),1)))))));}
return 0;}if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp17C->type))return 0;}return 1;
_LLD2: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp171=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)
_tmp169;if(_tmp171->tag != 4)goto _LLD4;else{_tmp172=_tmp171->f1;_tmp173=_tmp172.field_info;
if((_tmp173.KnownDatatypefield).tag != 2)goto _LLD4;_tmp174=(struct _tuple2)(
_tmp173.KnownDatatypefield).val;_tmp175=_tmp174.f2;}}_LLD3:{struct Cyc_List_List*
_tmp180=_tmp175->typs;for(0;_tmp180 != 0;_tmp180=_tmp180->tl){struct _tuple19
_tmp182;struct Cyc_Absyn_Tqual _tmp183;void*_tmp184;struct _tuple19*_tmp181=(struct
_tuple19*)_tmp180->hd;_tmp182=*_tmp181;_tmp183=_tmp182.f1;_tmp184=_tmp182.f2;if(
_tmp183.real_const){{const char*_tmp7B6;void*_tmp7B5[1];struct Cyc_String_pa_PrintArg_struct
_tmp7B4;(_tmp7B4.tag=0,((_tmp7B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp175->name)),((_tmp7B5[0]=& _tmp7B4,Cyc_Tcutil_terr(
loc,((_tmp7B6="attempt to over-write a datatype field (%s) with a const member",
_tag_dyneither(_tmp7B6,sizeof(char),64))),_tag_dyneither(_tmp7B5,sizeof(void*),1)))))));}
return 0;}if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp184))return 0;}}return 1;_LLD4: {
struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp176=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)
_tmp169;if(_tmp176->tag != 8)goto _LLD6;else{_tmp177=_tmp176->f1;_tmp178=_tmp177.elt_type;
_tmp179=_tmp177.tq;}}_LLD5: if(_tmp179.real_const){{const char*_tmp7B9;void*
_tmp7B8;(_tmp7B8=0,Cyc_Tcutil_terr(loc,((_tmp7B9="attempt to over-write a const array",
_tag_dyneither(_tmp7B9,sizeof(char),36))),_tag_dyneither(_tmp7B8,sizeof(void*),0)));}
return 0;}return Cyc_Tcexp_check_writable_aggr(loc,_tmp178);_LLD6: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmp17A=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp169;if(_tmp17A->tag != 
10)goto _LLD8;else{_tmp17B=_tmp17A->f1;}}_LLD7: for(0;_tmp17B != 0;_tmp17B=_tmp17B->tl){
struct _tuple19 _tmp18B;struct Cyc_Absyn_Tqual _tmp18C;void*_tmp18D;struct _tuple19*
_tmp18A=(struct _tuple19*)_tmp17B->hd;_tmp18B=*_tmp18A;_tmp18C=_tmp18B.f1;_tmp18D=
_tmp18B.f2;if(_tmp18C.real_const){{const char*_tmp7BC;void*_tmp7BB;(_tmp7BB=0,Cyc_Tcutil_terr(
loc,((_tmp7BC="attempt to over-write a tuple field with a const member",
_tag_dyneither(_tmp7BC,sizeof(char),56))),_tag_dyneither(_tmp7BB,sizeof(void*),0)));}
return 0;}if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp18D))return 0;}return 1;_LLD8:;
_LLD9: return 1;_LLCD:;}static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)
_check_null(e->topt)))return;{void*_tmp190=e->r;void*_tmp192;struct Cyc_Absyn_Vardecl*
_tmp194;void*_tmp196;struct Cyc_Absyn_Vardecl*_tmp198;void*_tmp19A;struct Cyc_Absyn_Vardecl*
_tmp19C;void*_tmp19E;struct Cyc_Absyn_Vardecl*_tmp1A0;struct Cyc_Absyn_Exp*_tmp1A2;
struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A5;struct _dyneither_ptr*
_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A8;struct _dyneither_ptr*_tmp1A9;struct Cyc_Absyn_Exp*
_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AF;_LLDB: {struct
Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp191=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp190;if(_tmp191->tag != 1)goto _LLDD;else{_tmp192=(void*)_tmp191->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*
_tmp193=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp192;if(_tmp193->tag != 
3)goto _LLDD;else{_tmp194=_tmp193->f1;}};}}_LLDC: _tmp198=_tmp194;goto _LLDE;_LLDD: {
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp195=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp190;if(_tmp195->tag != 1)goto _LLDF;else{_tmp196=(void*)_tmp195->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*
_tmp197=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp196;if(_tmp197->tag != 
4)goto _LLDF;else{_tmp198=_tmp197->f1;}};}}_LLDE: _tmp19C=_tmp198;goto _LLE0;_LLDF: {
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp199=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp190;if(_tmp199->tag != 1)goto _LLE1;else{_tmp19A=(void*)_tmp199->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*
_tmp19B=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp19A;if(_tmp19B->tag != 5)
goto _LLE1;else{_tmp19C=_tmp19B->f1;}};}}_LLE0: _tmp1A0=_tmp19C;goto _LLE2;_LLE1: {
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp19D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp190;if(_tmp19D->tag != 1)goto _LLE3;else{_tmp19E=(void*)_tmp19D->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmp19F=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp19E;if(_tmp19F->tag
!= 1)goto _LLE3;else{_tmp1A0=_tmp19F->f1;}};}}_LLE2: if(!(_tmp1A0->tq).real_const)
return;goto _LLDA;_LLE3: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*
_tmp1A1=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp190;if(_tmp1A1->tag
!= 22)goto _LLE5;else{_tmp1A2=_tmp1A1->f1;_tmp1A3=_tmp1A1->f2;}}_LLE4:{void*
_tmp1B0=Cyc_Tcutil_compress((void*)_check_null(_tmp1A2->topt));struct Cyc_Absyn_PtrInfo
_tmp1B2;struct Cyc_Absyn_Tqual _tmp1B3;struct Cyc_Absyn_ArrayInfo _tmp1B5;struct Cyc_Absyn_Tqual
_tmp1B6;struct Cyc_List_List*_tmp1B8;_LLF2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp1B1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B0;if(_tmp1B1->tag
!= 5)goto _LLF4;else{_tmp1B2=_tmp1B1->f1;_tmp1B3=_tmp1B2.elt_tq;}}_LLF3: _tmp1B6=
_tmp1B3;goto _LLF5;_LLF4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1B4=(
struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B0;if(_tmp1B4->tag != 8)goto
_LLF6;else{_tmp1B5=_tmp1B4->f1;_tmp1B6=_tmp1B5.tq;}}_LLF5: if(!_tmp1B6.real_const)
return;goto _LLF1;_LLF6: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1B7=(
struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1B0;if(_tmp1B7->tag != 10)goto
_LLF8;else{_tmp1B8=_tmp1B7->f1;}}_LLF7: {unsigned int _tmp1BA;int _tmp1BB;struct
_tuple15 _tmp1B9=Cyc_Evexp_eval_const_uint_exp(_tmp1A3);_tmp1BA=_tmp1B9.f1;
_tmp1BB=_tmp1B9.f2;if(!_tmp1BB){{const char*_tmp7BF;void*_tmp7BE;(_tmp7BE=0,Cyc_Tcutil_terr(
e->loc,((_tmp7BF="tuple projection cannot use sizeof or offsetof",_tag_dyneither(
_tmp7BF,sizeof(char),47))),_tag_dyneither(_tmp7BE,sizeof(void*),0)));}return;}{
struct _handler_cons _tmp1BE;_push_handler(& _tmp1BE);{int _tmp1C0=0;if(setjmp(
_tmp1BE.handler))_tmp1C0=1;if(!_tmp1C0){{struct _tuple19 _tmp1C2;struct Cyc_Absyn_Tqual
_tmp1C3;struct _tuple19*_tmp1C1=((struct _tuple19*(*)(struct Cyc_List_List*x,int n))
Cyc_List_nth)(_tmp1B8,(int)_tmp1BA);_tmp1C2=*_tmp1C1;_tmp1C3=_tmp1C2.f1;if(!
_tmp1C3.real_const){_npop_handler(0);return;}};_pop_handler();}else{void*_tmp1BF=(
void*)_exn_thrown;void*_tmp1C5=_tmp1BF;_LLFB: {struct Cyc_List_Nth_exn_struct*
_tmp1C6=(struct Cyc_List_Nth_exn_struct*)_tmp1C5;if(_tmp1C6->tag != Cyc_List_Nth)
goto _LLFD;}_LLFC: return;_LLFD:;_LLFE:(void)_throw(_tmp1C5);_LLFA:;}};}goto _LLF1;}
_LLF8:;_LLF9: goto _LLF1;_LLF1:;}goto _LLDA;_LLE5: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*
_tmp1A4=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp190;if(_tmp1A4->tag
!= 20)goto _LLE7;else{_tmp1A5=_tmp1A4->f1;_tmp1A6=_tmp1A4->f2;}}_LLE6:{void*
_tmp1C7=Cyc_Tcutil_compress((void*)_check_null(_tmp1A5->topt));struct Cyc_Absyn_AggrInfo
_tmp1C9;union Cyc_Absyn_AggrInfoU _tmp1CA;struct Cyc_Absyn_Aggrdecl**_tmp1CB;struct
Cyc_List_List*_tmp1CD;_LL100: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp1C8=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1C7;if(_tmp1C8->tag != 
11)goto _LL102;else{_tmp1C9=_tmp1C8->f1;_tmp1CA=_tmp1C9.aggr_info;if((_tmp1CA.KnownAggr).tag
!= 2)goto _LL102;_tmp1CB=(struct Cyc_Absyn_Aggrdecl**)(_tmp1CA.KnownAggr).val;}}
_LL101: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)_tmp1CB == 0?0:
Cyc_Absyn_lookup_decl_field(*_tmp1CB,_tmp1A6);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLFF;}_LL102: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*
_tmp1CC=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1C7;if(_tmp1CC->tag
!= 12)goto _LL104;else{_tmp1CD=_tmp1CC->f2;}}_LL103: {struct Cyc_Absyn_Aggrfield*
sf=Cyc_Absyn_lookup_field(_tmp1CD,_tmp1A6);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLFF;}_LL104:;_LL105: goto _LLFF;_LLFF:;}goto _LLDA;_LLE7: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*
_tmp1A7=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp190;if(_tmp1A7->tag
!= 21)goto _LLE9;else{_tmp1A8=_tmp1A7->f1;_tmp1A9=_tmp1A7->f2;}}_LLE8:{void*
_tmp1CE=Cyc_Tcutil_compress((void*)_check_null(_tmp1A8->topt));struct Cyc_Absyn_PtrInfo
_tmp1D0;void*_tmp1D1;_LL107: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp1CF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1CE;if(_tmp1CF->tag
!= 5)goto _LL109;else{_tmp1D0=_tmp1CF->f1;_tmp1D1=_tmp1D0.elt_typ;}}_LL108:{void*
_tmp1D2=Cyc_Tcutil_compress(_tmp1D1);struct Cyc_Absyn_AggrInfo _tmp1D4;union Cyc_Absyn_AggrInfoU
_tmp1D5;struct Cyc_Absyn_Aggrdecl**_tmp1D6;struct Cyc_List_List*_tmp1D8;_LL10C: {
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1D3=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmp1D2;if(_tmp1D3->tag != 11)goto _LL10E;else{_tmp1D4=_tmp1D3->f1;_tmp1D5=_tmp1D4.aggr_info;
if((_tmp1D5.KnownAggr).tag != 2)goto _LL10E;_tmp1D6=(struct Cyc_Absyn_Aggrdecl**)(
_tmp1D5.KnownAggr).val;}}_LL10D: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp1D6 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp1D6,_tmp1A9);if(sf == 0  || !(sf->tq).real_const)
return;goto _LL10B;}_LL10E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*
_tmp1D7=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1D2;if(_tmp1D7->tag
!= 12)goto _LL110;else{_tmp1D8=_tmp1D7->f2;}}_LL10F: {struct Cyc_Absyn_Aggrfield*
sf=Cyc_Absyn_lookup_field(_tmp1D8,_tmp1A9);if(sf == 0  || !(sf->tq).real_const)
return;goto _LL10B;}_LL110:;_LL111: goto _LL10B;_LL10B:;}goto _LL106;_LL109:;_LL10A:
goto _LL106;_LL106:;}goto _LLDA;_LLE9: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*
_tmp1AA=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp190;if(_tmp1AA->tag != 
19)goto _LLEB;else{_tmp1AB=_tmp1AA->f1;}}_LLEA:{void*_tmp1D9=Cyc_Tcutil_compress((
void*)_check_null(_tmp1AB->topt));struct Cyc_Absyn_PtrInfo _tmp1DB;struct Cyc_Absyn_Tqual
_tmp1DC;struct Cyc_Absyn_ArrayInfo _tmp1DE;struct Cyc_Absyn_Tqual _tmp1DF;_LL113: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1DA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmp1D9;if(_tmp1DA->tag != 5)goto _LL115;else{_tmp1DB=_tmp1DA->f1;_tmp1DC=_tmp1DB.elt_tq;}}
_LL114: _tmp1DF=_tmp1DC;goto _LL116;_LL115: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp1DD=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D9;if(_tmp1DD->tag != 
8)goto _LL117;else{_tmp1DE=_tmp1DD->f1;_tmp1DF=_tmp1DE.tq;}}_LL116: if(!_tmp1DF.real_const)
return;goto _LL112;_LL117:;_LL118: goto _LL112;_LL112:;}goto _LLDA;_LLEB: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*
_tmp1AC=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp190;if(
_tmp1AC->tag != 11)goto _LLED;else{_tmp1AD=_tmp1AC->f1;}}_LLEC: _tmp1AF=_tmp1AD;
goto _LLEE;_LLED: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp1AE=(
struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp190;if(_tmp1AE->tag != 12)
goto _LLEF;else{_tmp1AF=_tmp1AE->f1;}}_LLEE: Cyc_Tcexp_check_writable(te,_tmp1AF);
return;_LLEF:;_LLF0: goto _LLDA;_LLDA:;}{const char*_tmp7C3;void*_tmp7C2[1];struct
Cyc_String_pa_PrintArg_struct _tmp7C1;(_tmp7C1.tag=0,((_tmp7C1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp7C2[0]=&
_tmp7C1,Cyc_Tcutil_terr(e->loc,((_tmp7C3="attempt to write a const location: %s",
_tag_dyneither(_tmp7C3,sizeof(char),38))),_tag_dyneither(_tmp7C2,sizeof(void*),1)))))));};}
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**
topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){struct _RegionHandle
_tmp1E3=_new_region("temp");struct _RegionHandle*temp=& _tmp1E3;_push_region(temp);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(temp,te),0,e);if(!Cyc_Absyn_is_lvalue(
e)){const char*_tmp7C6;void*_tmp7C5;void*_tmp1E6=(_tmp7C5=0,Cyc_Tcexp_expr_err(te,
loc,topt,((_tmp7C6="increment/decrement of non-lvalue",_tag_dyneither(_tmp7C6,
sizeof(char),34))),_tag_dyneither(_tmp7C5,sizeof(void*),0)));_npop_handler(0);
return _tmp1E6;}Cyc_Tcexp_check_writable(te,e);{void*t=(void*)_check_null(e->topt);
if(!Cyc_Tcutil_is_numeric(e)){void*telt=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc
 || i == Cyc_Absyn_PostInc)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),&
Cyc_Tcutil_tmk)){const char*_tmp7C9;void*_tmp7C8;(_tmp7C8=0,Cyc_Tcutil_terr(e->loc,((
_tmp7C9="can't perform arithmetic on abstract pointer type",_tag_dyneither(
_tmp7C9,sizeof(char),50))),_tag_dyneither(_tmp7C8,sizeof(void*),0)));}if(Cyc_Tcutil_is_noalias_pointer(
t)){const char*_tmp7CC;void*_tmp7CB;(_tmp7CB=0,Cyc_Tcutil_terr(e->loc,((_tmp7CC="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp7CC,sizeof(char),54))),_tag_dyneither(_tmp7CB,sizeof(void*),0)));}}
else{const char*_tmp7D0;void*_tmp7CF[1];struct Cyc_String_pa_PrintArg_struct
_tmp7CE;(_tmp7CE.tag=0,((_tmp7CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp7CF[0]=& _tmp7CE,Cyc_Tcutil_terr(e->loc,((
_tmp7D0="expecting arithmetic or ? type but found %s",_tag_dyneither(_tmp7D0,
sizeof(char),44))),_tag_dyneither(_tmp7CF,sizeof(void*),1)))))));}}{void*_tmp1EE=
t;_npop_handler(0);return _tmp1EE;};};;_pop_region(temp);}static void*Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct _tuple0*_tmp1F1;int _tmp1F2;const
char*_tmp7D1;struct Cyc_Tcexp_TestEnv _tmp1F0=Cyc_Tcexp_tcTest(te,e1,((_tmp7D1="conditional expression",
_tag_dyneither(_tmp7D1,sizeof(char),23))));_tmp1F1=_tmp1F0.eq;_tmp1F2=_tmp1F0.isTrue;
Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(te,topt,e3);{void*t=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));struct
Cyc_List_List _tmp7D2;struct Cyc_List_List _tmp1F3=(_tmp7D2.hd=e3,((_tmp7D2.tl=0,
_tmp7D2)));struct Cyc_List_List _tmp7D3;struct Cyc_List_List _tmp1F4=(_tmp7D3.hd=e2,((
_tmp7D3.tl=(struct Cyc_List_List*)& _tmp1F3,_tmp7D3)));if(!Cyc_Tcutil_coerce_list(
te,t,(struct Cyc_List_List*)& _tmp1F4)){{const char*_tmp7D8;void*_tmp7D7[2];struct
Cyc_String_pa_PrintArg_struct _tmp7D6;struct Cyc_String_pa_PrintArg_struct _tmp7D5;(
_tmp7D5.tag=0,((_tmp7D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)_check_null(e3->topt))),((_tmp7D6.tag=0,((_tmp7D6.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((
_tmp7D7[0]=& _tmp7D6,((_tmp7D7[1]=& _tmp7D5,Cyc_Tcutil_terr(loc,((_tmp7D8="conditional clause types do not match: %s != %s",
_tag_dyneither(_tmp7D8,sizeof(char),48))),_tag_dyneither(_tmp7D7,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return t;};}static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*
te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{const char*
_tmp7D9;Cyc_Tcexp_tcTest(te,e1,((_tmp7D9="logical-and expression",_tag_dyneither(
_tmp7D9,sizeof(char),23))));}{const char*_tmp7DA;Cyc_Tcexp_tcTest(te,e2,((_tmp7DA="logical-and expression",
_tag_dyneither(_tmp7DA,sizeof(char),23))));}return Cyc_Absyn_sint_typ;}static void*
Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){{const char*_tmp7DB;Cyc_Tcexp_tcTest(te,e1,((_tmp7DB="logical-or expression",
_tag_dyneither(_tmp7DB,sizeof(char),22))));}{const char*_tmp7DC;Cyc_Tcexp_tcTest(
te,e2,((_tmp7DC="logical-or expression",_tag_dyneither(_tmp7DC,sizeof(char),22))));}
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*
te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,
struct Cyc_Absyn_Exp*e2){struct _RegionHandle _tmp1FF=_new_region("r");struct
_RegionHandle*r=& _tmp1FF;_push_region(r);Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(
r,Cyc_Tcenv_enter_notreadctxt(r,te)),0,e1);{void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,(void**)& t1,e2);{void*t2=(void*)_check_null(e2->topt);{void*
_tmp200=Cyc_Tcutil_compress(t1);_LL11A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp201=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp200;if(_tmp201->tag != 
8)goto _LL11C;}_LL11B:{const char*_tmp7DF;void*_tmp7DE;(_tmp7DE=0,Cyc_Tcutil_terr(
loc,((_tmp7DF="cannot assign to an array",_tag_dyneither(_tmp7DF,sizeof(char),26))),
_tag_dyneither(_tmp7DE,sizeof(void*),0)));}goto _LL119;_LL11C:;_LL11D: goto _LL119;
_LL119:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk)){const
char*_tmp7E2;void*_tmp7E1;(_tmp7E1=0,Cyc_Tcutil_terr(loc,((_tmp7E2="type is abstract (can't determine size).",
_tag_dyneither(_tmp7E2,sizeof(char),41))),_tag_dyneither(_tmp7E1,sizeof(void*),0)));}
if(!Cyc_Absyn_is_lvalue(e1)){const char*_tmp7E5;void*_tmp7E4;void*_tmp208=(
_tmp7E4=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7E5="assignment to non-lvalue",
_tag_dyneither(_tmp7E5,sizeof(char),25))),_tag_dyneither(_tmp7E4,sizeof(void*),0)));
_npop_handler(0);return _tmp208;}Cyc_Tcexp_check_writable(te,e1);if(po == 0){
struct _RegionHandle _tmp209=_new_region("temp");struct _RegionHandle*temp=& _tmp209;
_push_region(temp);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t2) && !Cyc_Tcutil_is_noalias_path(
temp,e2)){const char*_tmp7E8;void*_tmp7E7;(_tmp7E7=0,Cyc_Tcutil_terr(e2->loc,((
_tmp7E8="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp7E8,sizeof(char),49))),_tag_dyneither(_tmp7E7,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_assign(
te,e2,t1)){const char*_tmp7ED;void*_tmp7EC[2];struct Cyc_String_pa_PrintArg_struct
_tmp7EB;struct Cyc_String_pa_PrintArg_struct _tmp7EA;void*_tmp20C=(_tmp7EA.tag=0,((
_tmp7EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp7EB.tag=0,((_tmp7EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7EC[0]=& _tmp7EB,((_tmp7EC[1]=& _tmp7EA,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp7ED="type mismatch: %s != %s",_tag_dyneither(_tmp7ED,sizeof(
char),24))),_tag_dyneither(_tmp7EC,sizeof(void*),2)))))))))))));Cyc_Tcutil_unify(
t1,t2);Cyc_Tcutil_explain_failure();{void*_tmp20D=_tmp20C;_npop_handler(1);
return _tmp20D;};};_pop_region(temp);}else{enum Cyc_Absyn_Primop _tmp212=(enum Cyc_Absyn_Primop)
po->v;void*_tmp213=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp212,e1,e2);if(!(Cyc_Tcutil_unify(
_tmp213,t1) || Cyc_Tcutil_coerceable(_tmp213) && Cyc_Tcutil_coerceable(t1))){
const char*_tmp7F2;void*_tmp7F1[2];struct Cyc_String_pa_PrintArg_struct _tmp7F0;
struct Cyc_String_pa_PrintArg_struct _tmp7EF;void*_tmp214=(_tmp7EF.tag=0,((_tmp7EF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((
_tmp7F0.tag=0,((_tmp7F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp7F1[0]=& _tmp7F0,((_tmp7F1[1]=& _tmp7EF,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp7F2="Cannot use this operator in an assignment when the arguments have types %s and %s",
_tag_dyneither(_tmp7F2,sizeof(char),82))),_tag_dyneither(_tmp7F1,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(_tmp213,t1);Cyc_Tcutil_explain_failure();{void*_tmp215=_tmp214;
_npop_handler(0);return _tmp215;};}{void*_tmp21A=_tmp213;_npop_handler(0);return
_tmp21A;};}{void*_tmp21B=t1;_npop_handler(0);return _tmp21B;};};};;_pop_region(r);}
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(
te,topt,e2);return(void*)_check_null(e2->topt);}static struct Cyc_Absyn_Datatypefield*
Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,
struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){struct Cyc_List_List*
fields;void*t1=(void*)_check_null(e->topt);{void*_tmp21C=Cyc_Tcutil_compress(t1);
int _tmp21E;enum Cyc_Absyn_Size_of _tmp220;enum Cyc_Absyn_Size_of _tmp222;_LL11F: {
struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp21D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)
_tmp21C;if(_tmp21D->tag != 7)goto _LL121;else{_tmp21E=_tmp21D->f1;if(_tmp21E != 0)
goto _LL121;}}_LL120: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);
t1=Cyc_Absyn_float_typ(1);goto _LL11E;_LL121: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp21F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp21C;if(_tmp21F->tag != 6)
goto _LL123;else{_tmp220=_tmp21F->f2;if(_tmp220 != Cyc_Absyn_Char_sz)goto _LL123;}}
_LL122: goto _LL124;_LL123: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp221=(
struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp21C;if(_tmp221->tag != 6)goto
_LL125;else{_tmp222=_tmp221->f2;if(_tmp222 != Cyc_Absyn_Short_sz)goto _LL125;}}
_LL124: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
t1=Cyc_Absyn_sint_typ;goto _LL11E;_LL125:;_LL126: goto _LL11E;_LL11E:;}for(fields=
fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp224;struct Cyc_List_List*
_tmp225;unsigned int _tmp226;enum Cyc_Absyn_Scope _tmp227;struct Cyc_Absyn_Datatypefield
_tmp223=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp224=_tmp223.name;
_tmp225=_tmp223.typs;_tmp226=_tmp223.loc;_tmp227=_tmp223.sc;if(_tmp225 == 0  || 
_tmp225->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple19*)_tmp225->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Datatypefield*)((
struct Cyc_Absyn_Datatypefield*)fields->hd);};}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp229;struct Cyc_List_List*_tmp22A;unsigned int
_tmp22B;enum Cyc_Absyn_Scope _tmp22C;struct Cyc_Absyn_Datatypefield _tmp228=*((
struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp229=_tmp228.name;_tmp22A=_tmp228.typs;
_tmp22B=_tmp228.loc;_tmp22C=_tmp228.sc;if(_tmp22A == 0  || _tmp22A->tl != 0)
continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple19*)_tmp22A->hd)).f2);
int bogus=0;if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))return(struct Cyc_Absyn_Datatypefield*)((
struct Cyc_Absyn_Datatypefield*)fields->hd);};}{const char*_tmp7F7;void*_tmp7F6[2];
struct Cyc_String_pa_PrintArg_struct _tmp7F5;struct Cyc_String_pa_PrintArg_struct
_tmp7F4;(_tmp7F4.tag=0,((_tmp7F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7F5.tag=0,((_tmp7F5.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu)),((_tmp7F6[0]=& _tmp7F5,((_tmp7F6[
1]=& _tmp7F4,Cyc_Tcutil_terr(e->loc,((_tmp7F7="can't find a field in %s to inject a value of type %s",
_tag_dyneither(_tmp7F7,sizeof(char),54))),_tag_dyneither(_tmp7F6,sizeof(void*),2)))))))))))));}
return 0;}static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**
vararg_call_info,struct Cyc_List_List**alias_arg_exps){struct Cyc_List_List*
_tmp231=args;int _tmp232=0;struct _RegionHandle _tmp233=_new_region("ter");struct
_RegionHandle*ter=& _tmp233;_push_region(ter);{struct Cyc_Tcenv_Tenv*_tmp234=Cyc_Tcenv_new_block(
ter,loc,te_orig);Cyc_Tcexp_tcExp(_tmp234,0,e);{void*t=Cyc_Tcutil_compress((void*)
_check_null(e->topt));void*_tmp235=t;struct Cyc_Absyn_PtrInfo _tmp237;void*_tmp238;
struct Cyc_Absyn_Tqual _tmp239;struct Cyc_Absyn_PtrAtts _tmp23A;void*_tmp23B;union
Cyc_Absyn_Constraint*_tmp23C;union Cyc_Absyn_Constraint*_tmp23D;union Cyc_Absyn_Constraint*
_tmp23E;_LL128: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp236=(struct
Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp235;if(_tmp236->tag != 5)goto _LL12A;
else{_tmp237=_tmp236->f1;_tmp238=_tmp237.elt_typ;_tmp239=_tmp237.elt_tq;_tmp23A=
_tmp237.ptr_atts;_tmp23B=_tmp23A.rgn;_tmp23C=_tmp23A.nullable;_tmp23D=_tmp23A.bounds;
_tmp23E=_tmp23A.zero_term;}}_LL129: Cyc_Tcenv_check_rgn_accessible(_tmp234,loc,
_tmp23B);Cyc_Tcutil_check_nonzero_bound(loc,_tmp23D);{void*_tmp23F=Cyc_Tcutil_compress(
_tmp238);struct Cyc_Absyn_FnInfo _tmp241;struct Cyc_List_List*_tmp242;void*_tmp243;
struct Cyc_Absyn_Tqual _tmp244;void*_tmp245;struct Cyc_List_List*_tmp246;int _tmp247;
struct Cyc_Absyn_VarargInfo*_tmp248;struct Cyc_List_List*_tmp249;struct Cyc_List_List*
_tmp24A;_LL12D: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp240=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)
_tmp23F;if(_tmp240->tag != 9)goto _LL12F;else{_tmp241=_tmp240->f1;_tmp242=_tmp241.tvars;
_tmp243=_tmp241.effect;_tmp244=_tmp241.ret_tqual;_tmp245=_tmp241.ret_typ;_tmp246=
_tmp241.args;_tmp247=_tmp241.c_varargs;_tmp248=_tmp241.cyc_varargs;_tmp249=
_tmp241.rgn_po;_tmp24A=_tmp241.attributes;}}_LL12E: if(topt != 0)Cyc_Tcutil_unify(
_tmp245,*topt);while(_tmp231 != 0  && _tmp246 != 0){int alias_coercion=0;struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)_tmp231->hd;void*t2=(*((struct _tuple9*)_tmp246->hd)).f3;
Cyc_Tcexp_tcExp(_tmp234,(void**)& t2,e1);if(!Cyc_Tcutil_coerce_arg(_tmp234,e1,t2,&
alias_coercion)){{const char*_tmp7FC;void*_tmp7FB[2];struct Cyc_String_pa_PrintArg_struct
_tmp7FA;struct Cyc_String_pa_PrintArg_struct _tmp7F9;(_tmp7F9.tag=0,((_tmp7F9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((
_tmp7FA.tag=0,((_tmp7FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)_check_null(e1->topt))),((_tmp7FB[0]=& _tmp7FA,((_tmp7FB[1]=& _tmp7F9,Cyc_Tcutil_terr(
e1->loc,((_tmp7FC="actual argument has type \n\t%s\n but formal has type \n\t%s.",
_tag_dyneither(_tmp7FC,sizeof(char),57))),_tag_dyneither(_tmp7FB,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);Cyc_Tcutil_explain_failure();}
if(alias_coercion){struct Cyc_List_List*_tmp7FD;*alias_arg_exps=(struct Cyc_List_List*)((
_tmp7FD=_cycalloc(sizeof(*_tmp7FD)),((_tmp7FD->hd=(void*)_tmp232,((_tmp7FD->tl=*
alias_arg_exps,_tmp7FD))))));}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,t2)
 && !Cyc_Tcutil_is_noalias_path(ter,e1)){const char*_tmp800;void*_tmp7FF;(_tmp7FF=
0,Cyc_Tcutil_terr(e1->loc,((_tmp800="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp800,sizeof(char),49))),_tag_dyneither(_tmp7FF,sizeof(void*),0)));}
_tmp231=_tmp231->tl;_tmp246=_tmp246->tl;++ _tmp232;}{int args_already_checked=0;{
struct Cyc_List_List*a=_tmp24A;for(0;a != 0;a=a->tl){void*_tmp252=(void*)a->hd;
enum Cyc_Absyn_Format_Type _tmp254;int _tmp255;int _tmp256;_LL132: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*
_tmp253=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp252;if(_tmp253->tag
!= 19)goto _LL134;else{_tmp254=_tmp253->f1;_tmp255=_tmp253->f2;_tmp256=_tmp253->f3;}}
_LL133:{struct _handler_cons _tmp257;_push_handler(& _tmp257);{int _tmp259=0;if(
setjmp(_tmp257.handler))_tmp259=1;if(!_tmp259){{struct Cyc_Absyn_Exp*_tmp25A=((
struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp255 - 
1);struct Cyc_Core_Opt*fmt_args;if(_tmp256 == 0)fmt_args=0;else{struct Cyc_Core_Opt*
_tmp801;fmt_args=((_tmp801=_cycalloc(sizeof(*_tmp801)),((_tmp801->v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp256 - 1),_tmp801))));}
args_already_checked=1;{struct _RegionHandle _tmp25C=_new_region("temp");struct
_RegionHandle*temp=& _tmp25C;_push_region(temp);switch(_tmp254){case Cyc_Absyn_Printf_ft:
_LL136: Cyc_Tcexp_check_format_args(_tmp234,_tmp25A,fmt_args,_tmp256 - 1,
alias_arg_exps,temp,Cyc_Formatstr_get_format_typs);break;case Cyc_Absyn_Scanf_ft:
_LL137: Cyc_Tcexp_check_format_args(_tmp234,_tmp25A,fmt_args,_tmp256 - 1,
alias_arg_exps,temp,Cyc_Formatstr_get_scanf_typs);break;};_pop_region(temp);};};
_pop_handler();}else{void*_tmp258=(void*)_exn_thrown;void*_tmp25E=_tmp258;_LL13A: {
struct Cyc_List_Nth_exn_struct*_tmp25F=(struct Cyc_List_Nth_exn_struct*)_tmp25E;
if(_tmp25F->tag != Cyc_List_Nth)goto _LL13C;}_LL13B:{const char*_tmp804;void*
_tmp803;(_tmp803=0,Cyc_Tcutil_terr(loc,((_tmp804="bad format arguments",
_tag_dyneither(_tmp804,sizeof(char),21))),_tag_dyneither(_tmp803,sizeof(void*),0)));}
goto _LL139;_LL13C:;_LL13D:(void)_throw(_tmp25E);_LL139:;}};}goto _LL131;_LL134:;
_LL135: goto _LL131;_LL131:;}}if(_tmp246 != 0){const char*_tmp807;void*_tmp806;(
_tmp806=0,Cyc_Tcutil_terr(loc,((_tmp807="too few arguments for function",
_tag_dyneither(_tmp807,sizeof(char),31))),_tag_dyneither(_tmp806,sizeof(void*),0)));}
else{if((_tmp231 != 0  || _tmp247) || _tmp248 != 0){if(_tmp247)for(0;_tmp231 != 0;
_tmp231=_tmp231->tl){Cyc_Tcexp_tcExp(_tmp234,0,(struct Cyc_Absyn_Exp*)_tmp231->hd);}
else{if(_tmp248 == 0){const char*_tmp80A;void*_tmp809;(_tmp809=0,Cyc_Tcutil_terr(
loc,((_tmp80A="too many arguments for function",_tag_dyneither(_tmp80A,sizeof(
char),32))),_tag_dyneither(_tmp809,sizeof(void*),0)));}else{void*_tmp267;int
_tmp268;struct Cyc_Absyn_VarargInfo _tmp266=*_tmp248;_tmp267=_tmp266.type;_tmp268=
_tmp266.inject;{struct Cyc_Absyn_VarargCallInfo*_tmp80B;struct Cyc_Absyn_VarargCallInfo*
_tmp269=(_tmp80B=_cycalloc(sizeof(*_tmp80B)),((_tmp80B->num_varargs=0,((_tmp80B->injectors=
0,((_tmp80B->vai=(struct Cyc_Absyn_VarargInfo*)_tmp248,_tmp80B)))))));*
vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)_tmp269;if(!_tmp268)for(0;
_tmp231 != 0;(_tmp231=_tmp231->tl,_tmp232 ++)){int alias_coercion=0;struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)_tmp231->hd;++ _tmp269->num_varargs;Cyc_Tcexp_tcExp(
_tmp234,(void**)& _tmp267,e1);if(!Cyc_Tcutil_coerce_arg(_tmp234,e1,_tmp267,&
alias_coercion)){{const char*_tmp810;void*_tmp80F[2];struct Cyc_String_pa_PrintArg_struct
_tmp80E;struct Cyc_String_pa_PrintArg_struct _tmp80D;(_tmp80D.tag=0,((_tmp80D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)
_check_null(e1->topt))),((_tmp80E.tag=0,((_tmp80E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp267)),((_tmp80F[0]=& _tmp80E,((
_tmp80F[1]=& _tmp80D,Cyc_Tcutil_terr(loc,((_tmp810="vararg requires type %s but argument has type %s",
_tag_dyneither(_tmp810,sizeof(char),49))),_tag_dyneither(_tmp80F,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(alias_coercion){struct Cyc_List_List*_tmp811;*
alias_arg_exps=(struct Cyc_List_List*)((_tmp811=_cycalloc(sizeof(*_tmp811)),((
_tmp811->hd=(void*)_tmp232,((_tmp811->tl=*alias_arg_exps,_tmp811))))));}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
ter,_tmp267) && !Cyc_Tcutil_is_noalias_path(ter,e1)){const char*_tmp814;void*
_tmp813;(_tmp813=0,Cyc_Tcutil_terr(e1->loc,((_tmp814="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp814,sizeof(char),49))),_tag_dyneither(_tmp813,sizeof(void*),0)));}}
else{void*_tmp271=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp267));
struct Cyc_Absyn_DatatypeInfo _tmp273;union Cyc_Absyn_DatatypeInfoU _tmp274;struct
Cyc_Absyn_Datatypedecl**_tmp275;struct Cyc_Absyn_Datatypedecl*_tmp276;struct Cyc_List_List*
_tmp277;_LL13F: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp272=(struct
Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp271;if(_tmp272->tag != 3)goto _LL141;
else{_tmp273=_tmp272->f1;_tmp274=_tmp273.datatype_info;if((_tmp274.KnownDatatype).tag
!= 2)goto _LL141;_tmp275=(struct Cyc_Absyn_Datatypedecl**)(_tmp274.KnownDatatype).val;
_tmp276=*_tmp275;_tmp277=_tmp273.targs;}}_LL140: {struct Cyc_Absyn_Datatypedecl*
_tmp278=*Cyc_Tcenv_lookup_datatypedecl(_tmp234,loc,_tmp276->name);struct Cyc_List_List*
fields=0;if(_tmp278->fields == 0){const char*_tmp818;void*_tmp817[1];struct Cyc_String_pa_PrintArg_struct
_tmp816;(_tmp816.tag=0,((_tmp816.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp267)),((_tmp817[0]=& _tmp816,Cyc_Tcutil_terr(loc,((
_tmp818="can't inject into abstract datatype %s",_tag_dyneither(_tmp818,sizeof(
char),39))),_tag_dyneither(_tmp817,sizeof(void*),1)))))));}else{fields=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp278->fields))->v;}if(!Cyc_Tcutil_unify(
Cyc_Tcutil_pointer_region(_tmp267),Cyc_Tcenv_curr_rgn(_tmp234))){const char*
_tmp81D;void*_tmp81C[2];struct Cyc_String_pa_PrintArg_struct _tmp81B;struct Cyc_String_pa_PrintArg_struct
_tmp81A;(_tmp81A.tag=0,((_tmp81A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp234))),((_tmp81B.tag=0,((_tmp81B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp267)),((
_tmp81C[0]=& _tmp81B,((_tmp81C[1]=& _tmp81A,Cyc_Tcutil_terr(loc,((_tmp81D="can't unify pointer region for %s to curr_rgn %s",
_tag_dyneither(_tmp81D,sizeof(char),49))),_tag_dyneither(_tmp81C,sizeof(void*),2)))))))))))));}{
struct _RegionHandle _tmp280=_new_region("rgn");struct _RegionHandle*rgn=& _tmp280;
_push_region(rgn);{struct Cyc_List_List*_tmp281=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp278->tvs,_tmp277);for(0;_tmp231 != 0;_tmp231=
_tmp231->tl){++ _tmp269->num_varargs;{struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)
_tmp231->hd;if(!args_already_checked){Cyc_Tcexp_tcExp(_tmp234,0,e1);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,(void*)_check_null(e1->topt)) && !Cyc_Tcutil_is_noalias_path(rgn,e1)){const
char*_tmp820;void*_tmp81F;(_tmp81F=0,Cyc_Tcutil_terr(e1->loc,((_tmp820="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp820,sizeof(char),49))),_tag_dyneither(_tmp81F,sizeof(void*),0)));}}{
struct Cyc_Absyn_Datatypefield*_tmp284=Cyc_Tcexp_tcInjection(_tmp234,e1,Cyc_Tcutil_pointer_elt_type(
_tmp267),rgn,_tmp281,fields);if(_tmp284 != 0){struct Cyc_List_List*_tmp821;_tmp269->injectors=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp269->injectors,((_tmp821=_cycalloc(sizeof(*_tmp821)),((_tmp821->hd=(struct
Cyc_Absyn_Datatypefield*)_tmp284,((_tmp821->tl=0,_tmp821)))))));}};};}}
_npop_handler(0);goto _LL13E;;_pop_region(rgn);};}_LL141:;_LL142:{const char*
_tmp824;void*_tmp823;(_tmp823=0,Cyc_Tcutil_terr(loc,((_tmp824="bad inject vararg type",
_tag_dyneither(_tmp824,sizeof(char),23))),_tag_dyneither(_tmp823,sizeof(void*),0)));}
goto _LL13E;_LL13E:;}};}}}}Cyc_Tcenv_check_effect_accessible(_tmp234,loc,(void*)
_check_null(_tmp243));Cyc_Tcenv_check_rgn_partial_order(_tmp234,loc,_tmp249);{
void*_tmp289=_tmp245;_npop_handler(0);return _tmp289;};};_LL12F:;_LL130: {const
char*_tmp827;void*_tmp826;void*_tmp28C=(_tmp826=0,Cyc_Tcexp_expr_err(_tmp234,loc,
topt,((_tmp827="expected pointer to function",_tag_dyneither(_tmp827,sizeof(char),
29))),_tag_dyneither(_tmp826,sizeof(void*),0)));_npop_handler(0);return _tmp28C;}
_LL12C:;};_LL12A:;_LL12B: {const char*_tmp82A;void*_tmp829;void*_tmp28F=(_tmp829=
0,Cyc_Tcexp_expr_err(_tmp234,loc,topt,((_tmp82A="expected pointer to function",
_tag_dyneither(_tmp82A,sizeof(char),29))),_tag_dyneither(_tmp829,sizeof(void*),0)));
_npop_handler(0);return _tmp28F;}_LL127:;};};_pop_region(ter);}static void*Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){int
bogus=0;void*_tmp290=Cyc_Absyn_exn_typ();Cyc_Tcexp_tcExp(te,(void**)& _tmp290,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp290,& bogus)){const char*_tmp82F;void*_tmp82E[2];
struct Cyc_String_pa_PrintArg_struct _tmp82D;struct Cyc_String_pa_PrintArg_struct
_tmp82C;(_tmp82C.tag=0,((_tmp82C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp82D.tag=0,((_tmp82D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp290)),((
_tmp82E[0]=& _tmp82D,((_tmp82E[1]=& _tmp82C,Cyc_Tcutil_terr(loc,((_tmp82F="expected %s but found %s",
_tag_dyneither(_tmp82F,sizeof(char),25))),_tag_dyneither(_tmp82E,sizeof(void*),2)))))))))))));}
if(topt != 0)return*topt;return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,
void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){Cyc_Tcexp_tcExpNoInst(
te,0,e);e->topt=(void*)Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));{void*_tmp295=t1;struct
Cyc_Absyn_PtrInfo _tmp297;void*_tmp298;struct Cyc_Absyn_Tqual _tmp299;struct Cyc_Absyn_PtrAtts
_tmp29A;void*_tmp29B;union Cyc_Absyn_Constraint*_tmp29C;union Cyc_Absyn_Constraint*
_tmp29D;union Cyc_Absyn_Constraint*_tmp29E;_LL144: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp296=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp295;if(_tmp296->tag
!= 5)goto _LL146;else{_tmp297=_tmp296->f1;_tmp298=_tmp297.elt_typ;_tmp299=_tmp297.elt_tq;
_tmp29A=_tmp297.ptr_atts;_tmp29B=_tmp29A.rgn;_tmp29C=_tmp29A.nullable;_tmp29D=
_tmp29A.bounds;_tmp29E=_tmp29A.zero_term;}}_LL145:{void*_tmp29F=Cyc_Tcutil_compress(
_tmp298);struct Cyc_Absyn_FnInfo _tmp2A1;struct Cyc_List_List*_tmp2A2;void*_tmp2A3;
struct Cyc_Absyn_Tqual _tmp2A4;void*_tmp2A5;struct Cyc_List_List*_tmp2A6;int _tmp2A7;
struct Cyc_Absyn_VarargInfo*_tmp2A8;struct Cyc_List_List*_tmp2A9;struct Cyc_List_List*
_tmp2AA;_LL149: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2A0=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)
_tmp29F;if(_tmp2A0->tag != 9)goto _LL14B;else{_tmp2A1=_tmp2A0->f1;_tmp2A2=_tmp2A1.tvars;
_tmp2A3=_tmp2A1.effect;_tmp2A4=_tmp2A1.ret_tqual;_tmp2A5=_tmp2A1.ret_typ;_tmp2A6=
_tmp2A1.args;_tmp2A7=_tmp2A1.c_varargs;_tmp2A8=_tmp2A1.cyc_varargs;_tmp2A9=
_tmp2A1.rgn_po;_tmp2AA=_tmp2A1.attributes;}}_LL14A: {struct _RegionHandle _tmp2AB=
_new_region("temp");struct _RegionHandle*temp=& _tmp2AB;_push_region(temp);{struct
Cyc_List_List*instantiation=0;for(0;ts != 0  && _tmp2A2 != 0;(ts=ts->tl,_tmp2A2=
_tmp2A2->tl)){struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp2A2->hd,& Cyc_Tcutil_bk);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),k,1,(void*)ts->hd);Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{struct
_tuple13*_tmp832;struct Cyc_List_List*_tmp831;instantiation=((_tmp831=
_region_malloc(temp,sizeof(*_tmp831)),((_tmp831->hd=((_tmp832=_region_malloc(
temp,sizeof(*_tmp832)),((_tmp832->f1=(struct Cyc_Absyn_Tvar*)_tmp2A2->hd,((
_tmp832->f2=(void*)ts->hd,_tmp832)))))),((_tmp831->tl=instantiation,_tmp831))))));};}
if(ts != 0){const char*_tmp835;void*_tmp834;void*_tmp2B0=(_tmp834=0,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp835="too many type variables in instantiation",_tag_dyneither(
_tmp835,sizeof(char),41))),_tag_dyneither(_tmp834,sizeof(void*),0)));
_npop_handler(0);return _tmp2B0;}{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp83B;
struct Cyc_Absyn_FnInfo _tmp83A;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp839;
void*new_fn_typ=Cyc_Tcutil_rsubstitute(temp,instantiation,(void*)((_tmp839=
_cycalloc(sizeof(*_tmp839)),((_tmp839[0]=((_tmp83B.tag=9,((_tmp83B.f1=((_tmp83A.tvars=
_tmp2A2,((_tmp83A.effect=_tmp2A3,((_tmp83A.ret_tqual=_tmp2A4,((_tmp83A.ret_typ=
_tmp2A5,((_tmp83A.args=_tmp2A6,((_tmp83A.c_varargs=_tmp2A7,((_tmp83A.cyc_varargs=
_tmp2A8,((_tmp83A.rgn_po=_tmp2A9,((_tmp83A.attributes=_tmp2AA,_tmp83A)))))))))))))))))),
_tmp83B)))),_tmp839)))));struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp845;
struct Cyc_Absyn_PtrAtts _tmp844;struct Cyc_Absyn_PtrInfo _tmp843;struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp842;void*_tmp2B5=(void*)((_tmp842=_cycalloc(sizeof(*_tmp842)),((_tmp842[0]=((
_tmp845.tag=5,((_tmp845.f1=((_tmp843.elt_typ=new_fn_typ,((_tmp843.elt_tq=_tmp299,((
_tmp843.ptr_atts=((_tmp844.rgn=_tmp29B,((_tmp844.nullable=_tmp29C,((_tmp844.bounds=
_tmp29D,((_tmp844.zero_term=_tmp29E,((_tmp844.ptrloc=0,_tmp844)))))))))),_tmp843)))))),
_tmp845)))),_tmp842))));_npop_handler(0);return _tmp2B5;};};_pop_region(temp);}
_LL14B:;_LL14C: goto _LL148;_LL148:;}goto _LL143;_LL146:;_LL147: goto _LL143;_LL143:;}{
const char*_tmp849;void*_tmp848[1];struct Cyc_String_pa_PrintArg_struct _tmp847;
return(_tmp847.tag=0,((_tmp847.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp848[0]=& _tmp847,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp849="expecting polymorphic type but found %s",_tag_dyneither(_tmp849,sizeof(
char),40))),_tag_dyneither(_tmp848,sizeof(void*),1)))))));};};}static void*Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,
enum Cyc_Absyn_Coercion*c){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),& Cyc_Tcutil_tmk,1,t);Cyc_Tcutil_check_no_qual(loc,t);Cyc_Tcexp_tcExp(te,(
void**)& t,e);{void*t2=(void*)_check_null(e->topt);if(Cyc_Tcutil_silent_castable(
te,loc,t2,t))*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;
else{enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,loc,t2,t);if(crc != Cyc_Absyn_Unknown_coercion)*((
enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,
t,e))*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{Cyc_Tcutil_unify(
t2,t);{const char*_tmp84E;void*_tmp84D[2];struct Cyc_String_pa_PrintArg_struct
_tmp84C;struct Cyc_String_pa_PrintArg_struct _tmp84B;void*_tmp2BC=(_tmp84B.tag=0,((
_tmp84B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp84C.tag=0,((_tmp84C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp84D[0]=& _tmp84C,((_tmp84D[1]=& _tmp84B,Cyc_Tcexp_expr_err(te,loc,(void**)&
t,((_tmp84E="cannot cast %s to %s",_tag_dyneither(_tmp84E,sizeof(char),21))),
_tag_dyneither(_tmp84D,sizeof(void*),2)))))))))))));Cyc_Tcutil_explain_failure();
return _tmp2BC;};}}}{struct _tuple0 _tmp84F;struct _tuple0 _tmp2C2=(_tmp84F.f1=e->r,((
_tmp84F.f2=Cyc_Tcutil_compress(t),_tmp84F)));void*_tmp2C3;struct Cyc_Absyn_MallocInfo
_tmp2C5;int _tmp2C6;void*_tmp2C7;struct Cyc_Absyn_PtrInfo _tmp2C9;struct Cyc_Absyn_PtrAtts
_tmp2CA;union Cyc_Absyn_Constraint*_tmp2CB;union Cyc_Absyn_Constraint*_tmp2CC;
union Cyc_Absyn_Constraint*_tmp2CD;_LL14E: _tmp2C3=_tmp2C2.f1;{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*
_tmp2C4=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2C3;if(_tmp2C4->tag
!= 32)goto _LL150;else{_tmp2C5=_tmp2C4->f1;_tmp2C6=_tmp2C5.fat_result;}};_tmp2C7=
_tmp2C2.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2C8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmp2C7;if(_tmp2C8->tag != 5)goto _LL150;else{_tmp2C9=_tmp2C8->f1;_tmp2CA=_tmp2C9.ptr_atts;
_tmp2CB=_tmp2CA.nullable;_tmp2CC=_tmp2CA.bounds;_tmp2CD=_tmp2CA.zero_term;}};
_LL14F: if((_tmp2C6  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp2CD)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp2CB)){void*_tmp2CE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp2CC);struct Cyc_Absyn_Exp*_tmp2D0;_LL153: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp2CF=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2CE;if(_tmp2CF->tag != 
1)goto _LL155;else{_tmp2D0=_tmp2CF->f1;}}_LL154: if((Cyc_Evexp_eval_const_uint_exp(
_tmp2D0)).f1 == 1){const char*_tmp852;void*_tmp851;(_tmp851=0,Cyc_Tcutil_warn(loc,((
_tmp852="cast from ? pointer to * pointer will lose size information",
_tag_dyneither(_tmp852,sizeof(char),60))),_tag_dyneither(_tmp851,sizeof(void*),0)));}
goto _LL152;_LL155:;_LL156: goto _LL152;_LL152:;}goto _LL14D;_LL150:;_LL151: goto
_LL14D;_LL14D:;}return t;};}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*
te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){void**
_tmp2D3=0;struct Cyc_Absyn_Tqual _tmp2D4=Cyc_Absyn_empty_tqual(0);if(topt != 0){
void*_tmp2D5=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp2D7;void*
_tmp2D8;void**_tmp2D9;struct Cyc_Absyn_Tqual _tmp2DA;struct Cyc_Absyn_PtrAtts
_tmp2DB;union Cyc_Absyn_Constraint*_tmp2DC;_LL158: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp2D6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D5;if(_tmp2D6->tag
!= 5)goto _LL15A;else{_tmp2D7=_tmp2D6->f1;_tmp2D8=_tmp2D7.elt_typ;_tmp2D9=(void**)&(
_tmp2D6->f1).elt_typ;_tmp2DA=_tmp2D7.elt_tq;_tmp2DB=_tmp2D7.ptr_atts;_tmp2DC=
_tmp2DB.zero_term;}}_LL159: _tmp2D3=(void**)_tmp2D9;_tmp2D4=_tmp2DA;goto _LL157;
_LL15A:;_LL15B: goto _LL157;_LL157:;}{struct _RegionHandle _tmp2DD=_new_region("r");
struct _RegionHandle*r=& _tmp2DD;_push_region(r);Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_enter_lhs(
r,Cyc_Tcenv_clear_notreadctxt(r,te)),_tmp2D3,e);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,(void*)_check_null(e->topt))){const char*_tmp855;void*_tmp854;(_tmp854=0,Cyc_Tcutil_terr(
e->loc,((_tmp855="Cannot take the address of an alias-free path",_tag_dyneither(
_tmp855,sizeof(char),46))),_tag_dyneither(_tmp854,sizeof(void*),0)));}{void*
_tmp2E0=e->r;struct Cyc_Absyn_Exp*_tmp2E2;struct Cyc_Absyn_Exp*_tmp2E3;_LL15D: {
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp2E1=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)
_tmp2E0;if(_tmp2E1->tag != 22)goto _LL15F;else{_tmp2E2=_tmp2E1->f1;_tmp2E3=_tmp2E1->f2;}}
_LL15E:{void*_tmp2E4=Cyc_Tcutil_compress((void*)_check_null(_tmp2E2->topt));
_LL162: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2E5=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)
_tmp2E4;if(_tmp2E5->tag != 10)goto _LL164;}_LL163: goto _LL161;_LL164:;_LL165: e0->r=(
Cyc_Absyn_add_exp(_tmp2E2,_tmp2E3,0))->r;{void*_tmp2E6=Cyc_Tcexp_tcPlus(te,
_tmp2E2,_tmp2E3);_npop_handler(0);return _tmp2E6;};_LL161:;}goto _LL15C;_LL15F:;
_LL160: goto _LL15C;_LL15C:;}{void*_tmp2E7=e->r;int _tmp2E9;int _tmp2EB;_LL167: {
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp2E8=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)
_tmp2E7;if(_tmp2E8->tag != 20)goto _LL169;else{_tmp2E9=_tmp2E8->f3;if(_tmp2E9 != 1)
goto _LL169;}}_LL168: goto _LL16A;_LL169: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*
_tmp2EA=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2E7;if(_tmp2EA->tag
!= 21)goto _LL16B;else{_tmp2EB=_tmp2EA->f3;if(_tmp2EB != 1)goto _LL16B;}}_LL16A:{
const char*_tmp858;void*_tmp857;(_tmp857=0,Cyc_Tcutil_terr(e->loc,((_tmp858="cannot take the address of a @tagged union member",
_tag_dyneither(_tmp858,sizeof(char),50))),_tag_dyneither(_tmp857,sizeof(void*),0)));}
goto _LL166;_LL16B:;_LL16C: goto _LL166;_LL166:;}{int _tmp2EF;void*_tmp2F0;struct
_tuple14 _tmp2EE=Cyc_Tcutil_addressof_props(te,e);_tmp2EF=_tmp2EE.f1;_tmp2F0=
_tmp2EE.f2;if(Cyc_Tcutil_is_noalias_region(_tmp2F0,0)){const char*_tmp85B;void*
_tmp85A;(_tmp85A=0,Cyc_Tcutil_terr(e->loc,((_tmp85B="using & would manufacture an alias to an alias-free pointer",
_tag_dyneither(_tmp85B,sizeof(char),60))),_tag_dyneither(_tmp85A,sizeof(void*),0)));}{
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp2EF){tq.print_const=1;tq.real_const=
1;}{void*t=Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp2F0,tq,Cyc_Absyn_false_conref);
void*_tmp2F3=t;_npop_handler(0);return _tmp2F3;};};};;_pop_region(r);};}static
void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*
t){if(te->allow_valueof)return Cyc_Absyn_uint_typ;Cyc_Tcutil_check_type(loc,te,
Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);Cyc_Tcutil_check_no_qual(loc,
t);if(!Cyc_Evexp_okay_szofarg(t)){const char*_tmp85F;void*_tmp85E[1];struct Cyc_String_pa_PrintArg_struct
_tmp85D;(_tmp85D.tag=0,((_tmp85D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp85E[0]=& _tmp85D,Cyc_Tcutil_terr(loc,((_tmp85F="sizeof applied to type %s, which has unknown size here",
_tag_dyneither(_tmp85F,sizeof(char),55))),_tag_dyneither(_tmp85E,sizeof(void*),1)))))));}
if(topt != 0){void*_tmp2F7=Cyc_Tcutil_compress(*topt);void*_tmp2F9;_LL16E: {
struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2F8=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)
_tmp2F7;if(_tmp2F8->tag != 19)goto _LL170;else{_tmp2F9=(void*)_tmp2F8->f1;}}_LL16F: {
struct Cyc_Absyn_Exp*_tmp2FA=Cyc_Absyn_sizeoftyp_exp(t,0);struct Cyc_Absyn_ValueofType_Absyn_Type_struct
_tmp862;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp861;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*
_tmp2FB=(_tmp861=_cycalloc(sizeof(*_tmp861)),((_tmp861[0]=((_tmp862.tag=18,((
_tmp862.f1=_tmp2FA,_tmp862)))),_tmp861)));if(Cyc_Tcutil_unify(_tmp2F9,(void*)
_tmp2FB))return _tmp2F7;goto _LL16D;}_LL170:;_LL171: goto _LL16D;_LL16D:;}return Cyc_Absyn_uint_typ;}
int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*
sf){return Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 
0;}static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**
topt,void*t,void*n){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),&
Cyc_Tcutil_tmk,1,t);Cyc_Tcutil_check_no_qual(loc,t);{void*_tmp2FE=n;struct
_dyneither_ptr*_tmp300;unsigned int _tmp302;_LL173: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*
_tmp2FF=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2FE;if(
_tmp2FF->tag != 0)goto _LL175;else{_tmp300=_tmp2FF->f1;}}_LL174: {int bad_type=1;{
void*_tmp303=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp305;union Cyc_Absyn_AggrInfoU
_tmp306;struct Cyc_Absyn_Aggrdecl**_tmp307;struct Cyc_List_List*_tmp309;_LL178: {
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp304=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmp303;if(_tmp304->tag != 11)goto _LL17A;else{_tmp305=_tmp304->f1;_tmp306=_tmp305.aggr_info;
if((_tmp306.KnownAggr).tag != 2)goto _LL17A;_tmp307=(struct Cyc_Absyn_Aggrdecl**)(
_tmp306.KnownAggr).val;}}_LL179: if((*_tmp307)->impl == 0)goto _LL177;if(!((int(*)(
int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct
_dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp300,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp307)->impl))->fields)){
const char*_tmp866;void*_tmp865[1];struct Cyc_String_pa_PrintArg_struct _tmp864;(
_tmp864.tag=0,((_tmp864.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp300),((
_tmp865[0]=& _tmp864,Cyc_Tcutil_terr(loc,((_tmp866="no field of struct/union has name %s",
_tag_dyneither(_tmp866,sizeof(char),37))),_tag_dyneither(_tmp865,sizeof(void*),1)))))));}
bad_type=0;goto _LL177;_LL17A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*
_tmp308=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp303;if(_tmp308->tag
!= 12)goto _LL17C;else{_tmp309=_tmp308->f2;}}_LL17B: if(!((int(*)(int(*pred)(
struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,
struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp300,
_tmp309)){const char*_tmp86A;void*_tmp869[1];struct Cyc_String_pa_PrintArg_struct
_tmp868;(_tmp868.tag=0,((_tmp868.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp300),((_tmp869[0]=& _tmp868,Cyc_Tcutil_terr(loc,((_tmp86A="no field of struct/union has name %s",
_tag_dyneither(_tmp86A,sizeof(char),37))),_tag_dyneither(_tmp869,sizeof(void*),1)))))));}
bad_type=0;goto _LL177;_LL17C:;_LL17D: goto _LL177;_LL177:;}if(bad_type){const char*
_tmp86E;void*_tmp86D[1];struct Cyc_String_pa_PrintArg_struct _tmp86C;(_tmp86C.tag=
0,((_tmp86C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp86D[0]=& _tmp86C,Cyc_Tcutil_terr(loc,((_tmp86E="%s is not a known struct/union type",
_tag_dyneither(_tmp86E,sizeof(char),36))),_tag_dyneither(_tmp86D,sizeof(void*),1)))))));}
goto _LL172;}_LL175: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*
_tmp301=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp2FE;if(
_tmp301->tag != 1)goto _LL172;else{_tmp302=_tmp301->f1;}}_LL176: {int bad_type=1;{
void*_tmp313=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp315;union Cyc_Absyn_AggrInfoU
_tmp316;struct Cyc_Absyn_Aggrdecl**_tmp317;struct Cyc_List_List*_tmp319;struct Cyc_List_List*
_tmp31B;struct Cyc_Absyn_DatatypeFieldInfo _tmp31D;union Cyc_Absyn_DatatypeFieldInfoU
_tmp31E;struct _tuple2 _tmp31F;struct Cyc_Absyn_Datatypefield*_tmp320;_LL17F: {
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp314=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmp313;if(_tmp314->tag != 11)goto _LL181;else{_tmp315=_tmp314->f1;_tmp316=_tmp315.aggr_info;
if((_tmp316.KnownAggr).tag != 2)goto _LL181;_tmp317=(struct Cyc_Absyn_Aggrdecl**)(
_tmp316.KnownAggr).val;}}_LL180: if((*_tmp317)->impl == 0)goto _LL17E;_tmp319=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp317)->impl))->fields;goto _LL182;
_LL181: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp318=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)
_tmp313;if(_tmp318->tag != 12)goto _LL183;else{_tmp319=_tmp318->f2;}}_LL182: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp319)<= _tmp302){const char*
_tmp873;void*_tmp872[2];struct Cyc_Int_pa_PrintArg_struct _tmp871;struct Cyc_Int_pa_PrintArg_struct
_tmp870;(_tmp870.tag=1,((_tmp870.f1=(unsigned long)((int)_tmp302),((_tmp871.tag=
1,((_tmp871.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp319),((_tmp872[0]=& _tmp871,((_tmp872[1]=& _tmp870,Cyc_Tcutil_terr(loc,((
_tmp873="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp873,
sizeof(char),46))),_tag_dyneither(_tmp872,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL17E;_LL183: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp31A=(
struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp313;if(_tmp31A->tag != 10)goto
_LL185;else{_tmp31B=_tmp31A->f1;}}_LL184: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp31B)<= _tmp302){const char*_tmp878;void*_tmp877[2];struct Cyc_Int_pa_PrintArg_struct
_tmp876;struct Cyc_Int_pa_PrintArg_struct _tmp875;(_tmp875.tag=1,((_tmp875.f1=(
unsigned long)((int)_tmp302),((_tmp876.tag=1,((_tmp876.f1=(unsigned long)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp31B),((_tmp877[0]=& _tmp876,((_tmp877[
1]=& _tmp875,Cyc_Tcutil_terr(loc,((_tmp878="tuple has too few components: %d <= %d",
_tag_dyneither(_tmp878,sizeof(char),39))),_tag_dyneither(_tmp877,sizeof(void*),2)))))))))))));}
bad_type=0;goto _LL17E;_LL185: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*
_tmp31C=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp313;if(_tmp31C->tag
!= 4)goto _LL187;else{_tmp31D=_tmp31C->f1;_tmp31E=_tmp31D.field_info;if((_tmp31E.KnownDatatypefield).tag
!= 2)goto _LL187;_tmp31F=(struct _tuple2)(_tmp31E.KnownDatatypefield).val;_tmp320=
_tmp31F.f2;}}_LL186: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp320->typs)
< _tmp302){const char*_tmp87D;void*_tmp87C[2];struct Cyc_Int_pa_PrintArg_struct
_tmp87B;struct Cyc_Int_pa_PrintArg_struct _tmp87A;(_tmp87A.tag=1,((_tmp87A.f1=(
unsigned long)((int)_tmp302),((_tmp87B.tag=1,((_tmp87B.f1=(unsigned long)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp320->typs),((_tmp87C[0]=& _tmp87B,((
_tmp87C[1]=& _tmp87A,Cyc_Tcutil_terr(loc,((_tmp87D="datatype field has too few components: %d < %d",
_tag_dyneither(_tmp87D,sizeof(char),47))),_tag_dyneither(_tmp87C,sizeof(void*),2)))))))))))));}
bad_type=0;goto _LL17E;_LL187:;_LL188: goto _LL17E;_LL17E:;}if(bad_type){const char*
_tmp881;void*_tmp880[1];struct Cyc_String_pa_PrintArg_struct _tmp87F;(_tmp87F.tag=
0,((_tmp87F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp880[0]=& _tmp87F,Cyc_Tcutil_terr(loc,((_tmp881="%s is not a known type",
_tag_dyneither(_tmp881,sizeof(char),23))),_tag_dyneither(_tmp880,sizeof(void*),1)))))));}
goto _LL172;}_LL172:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct _RegionHandle _tmp330=_new_region("r");struct _RegionHandle*r=& _tmp330;
_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp331=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(
r,te_orig));Cyc_Tcexp_tcExp(_tmp331,0,e);{void*t=Cyc_Tcutil_compress((void*)
_check_null(e->topt));void*_tmp332=t;struct Cyc_Absyn_PtrInfo _tmp335;void*_tmp336;
struct Cyc_Absyn_PtrAtts _tmp337;void*_tmp338;union Cyc_Absyn_Constraint*_tmp339;
union Cyc_Absyn_Constraint*_tmp33A;_LL18A: {struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmp333=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp332;if(_tmp333->tag != 1)
goto _LL18C;}_LL18B: {struct Cyc_List_List*_tmp33B=Cyc_Tcenv_lookup_type_vars(
_tmp331);struct Cyc_Core_Opt*_tmp882;void*_tmp33C=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mko,((_tmp882=_cycalloc(sizeof(*_tmp882)),((_tmp882->v=_tmp33B,
_tmp882)))));struct Cyc_Core_Opt*_tmp883;void*_tmp33D=Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_trko,((_tmp883=_cycalloc(sizeof(*_tmp883)),((_tmp883->v=
_tmp33B,_tmp883)))));union Cyc_Absyn_Constraint*_tmp33E=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();union Cyc_Absyn_Constraint*_tmp33F=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();struct Cyc_Absyn_PtrAtts _tmp884;struct Cyc_Absyn_PtrAtts
_tmp340=(_tmp884.rgn=_tmp33D,((_tmp884.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp884.bounds=_tmp33E,((_tmp884.zero_term=_tmp33F,((
_tmp884.ptrloc=0,_tmp884)))))))));struct Cyc_Absyn_PointerType_Absyn_Type_struct
_tmp88A;struct Cyc_Absyn_PtrInfo _tmp889;struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp888;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp341=(_tmp888=_cycalloc(
sizeof(*_tmp888)),((_tmp888[0]=((_tmp88A.tag=5,((_tmp88A.f1=((_tmp889.elt_typ=
_tmp33C,((_tmp889.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp889.ptr_atts=_tmp340,
_tmp889)))))),_tmp88A)))),_tmp888)));Cyc_Tcutil_unify(t,(void*)_tmp341);_tmp336=
_tmp33C;_tmp338=_tmp33D;_tmp339=_tmp33E;_tmp33A=_tmp33F;goto _LL18D;}_LL18C: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp334=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmp332;if(_tmp334->tag != 5)goto _LL18E;else{_tmp335=_tmp334->f1;_tmp336=_tmp335.elt_typ;
_tmp337=_tmp335.ptr_atts;_tmp338=_tmp337.rgn;_tmp339=_tmp337.bounds;_tmp33A=
_tmp337.zero_term;}}_LL18D: Cyc_Tcenv_check_rgn_accessible(_tmp331,loc,_tmp338);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp339);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
_tmp336),& Cyc_Tcutil_tmk)){const char*_tmp88D;void*_tmp88C;(_tmp88C=0,Cyc_Tcutil_terr(
loc,((_tmp88D="can't dereference abstract pointer type",_tag_dyneither(_tmp88D,
sizeof(char),40))),_tag_dyneither(_tmp88C,sizeof(void*),0)));}{void*_tmp34A=
_tmp336;_npop_handler(0);return _tmp34A;};_LL18E:;_LL18F: {const char*_tmp891;void*
_tmp890[1];struct Cyc_String_pa_PrintArg_struct _tmp88F;void*_tmp34E=(_tmp88F.tag=
0,((_tmp88F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp890[0]=& _tmp88F,Cyc_Tcexp_expr_err(_tmp331,loc,topt,((_tmp891="expecting *, @, or ? type but found %s",
_tag_dyneither(_tmp891,sizeof(char),39))),_tag_dyneither(_tmp890,sizeof(void*),1)))))));
_npop_handler(0);return _tmp34E;}_LL189:;};};_pop_region(r);}static void*Cyc_Tcexp_tcAggrMember(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){struct
_RegionHandle _tmp34F=_new_region("r");struct _RegionHandle*r=& _tmp34F;
_push_region(r);Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_clear_notreadctxt(r,te),0,e);*
is_read=!Cyc_Tcenv_in_notreadctxt(te);{void*_tmp350=Cyc_Tcutil_compress((void*)
_check_null(e->topt));struct Cyc_Absyn_AggrInfo _tmp352;union Cyc_Absyn_AggrInfoU
_tmp353;struct Cyc_Absyn_Aggrdecl**_tmp354;struct Cyc_Absyn_Aggrdecl*_tmp355;
struct Cyc_List_List*_tmp356;enum Cyc_Absyn_AggrKind _tmp358;struct Cyc_List_List*
_tmp359;_LL191: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp351=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmp350;if(_tmp351->tag != 11)goto _LL193;else{_tmp352=_tmp351->f1;_tmp353=_tmp352.aggr_info;
if((_tmp353.KnownAggr).tag != 2)goto _LL193;_tmp354=(struct Cyc_Absyn_Aggrdecl**)(
_tmp353.KnownAggr).val;_tmp355=*_tmp354;_tmp356=_tmp352.targs;}}_LL192: {struct
Cyc_Absyn_Aggrfield*_tmp35E=Cyc_Absyn_lookup_decl_field(_tmp355,f);if(_tmp35E == 
0){const char*_tmp896;void*_tmp895[2];struct Cyc_String_pa_PrintArg_struct _tmp894;
struct Cyc_String_pa_PrintArg_struct _tmp893;void*_tmp363=(_tmp893.tag=0,((_tmp893.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp894.tag=0,((_tmp894.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp355->name)),((
_tmp895[0]=& _tmp894,((_tmp895[1]=& _tmp893,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp896="type %s has no %s field",_tag_dyneither(_tmp896,sizeof(char),24))),
_tag_dyneither(_tmp895,sizeof(void*),2)))))))))))));_npop_handler(0);return
_tmp363;}if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp355->impl))->tagged)*
is_tagged=1;{void*t2=_tmp35E->type;if(_tmp356 != 0){struct _RegionHandle _tmp364=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp364;_push_region(rgn);{struct Cyc_List_List*
_tmp365=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp355->tvs,
_tmp356);t2=Cyc_Tcutil_rsubstitute(rgn,_tmp365,_tmp35E->type);};_pop_region(rgn);}
if(((_tmp355->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp355->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t2)){const char*_tmp89A;void*_tmp899[1];struct Cyc_String_pa_PrintArg_struct
_tmp898;void*_tmp369=(_tmp898.tag=0,((_tmp898.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp899[0]=& _tmp898,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp89A="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp89A,sizeof(char),56))),_tag_dyneither(_tmp899,sizeof(void*),1)))))));
_npop_handler(0);return _tmp369;}if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp355->impl))->exist_vars != 0){if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te)))){const char*_tmp89E;void*_tmp89D[1];struct Cyc_String_pa_PrintArg_struct
_tmp89C;void*_tmp36D=(_tmp89C.tag=0,((_tmp89C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp89D[0]=& _tmp89C,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp89E="must use pattern-matching to access field %s\n\tdue to existential type variables.",
_tag_dyneither(_tmp89E,sizeof(char),81))),_tag_dyneither(_tmp89D,sizeof(void*),1)))))));
_npop_handler(0);return _tmp36D;}}{void*_tmp36E=t2;_npop_handler(0);return _tmp36E;};};}
_LL193: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp357=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)
_tmp350;if(_tmp357->tag != 12)goto _LL195;else{_tmp358=_tmp357->f1;_tmp359=_tmp357->f2;}}
_LL194: {struct Cyc_Absyn_Aggrfield*_tmp36F=Cyc_Absyn_lookup_field(_tmp359,f);if(
_tmp36F == 0){const char*_tmp8A2;void*_tmp8A1[1];struct Cyc_String_pa_PrintArg_struct
_tmp8A0;void*_tmp373=(_tmp8A0.tag=0,((_tmp8A0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp8A1[0]=& _tmp8A0,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp8A2="type has no %s field",_tag_dyneither(_tmp8A2,sizeof(char),21))),
_tag_dyneither(_tmp8A1,sizeof(void*),1)))))));_npop_handler(0);return _tmp373;}
if((_tmp358 == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
_tmp36F->type)){const char*_tmp8A6;void*_tmp8A5[1];struct Cyc_String_pa_PrintArg_struct
_tmp8A4;void*_tmp377=(_tmp8A4.tag=0,((_tmp8A4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp8A5[0]=& _tmp8A4,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp8A6="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp8A6,sizeof(char),56))),_tag_dyneither(_tmp8A5,sizeof(void*),1)))))));
_npop_handler(0);return _tmp377;}{void*_tmp378=_tmp36F->type;_npop_handler(0);
return _tmp378;};}_LL195:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp35A=(
struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp350;if(_tmp35A->tag != 8)goto
_LL197;}{const char*_tmp8A7;if(!(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp8A7="size",
_tag_dyneither(_tmp8A7,sizeof(char),5))))== 0))goto _LL197;};_LL196: goto _LL198;
_LL197:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp35C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmp350;if(_tmp35C->tag != 5)goto _LL199;}{const char*_tmp8A8;if(!(Cyc_strcmp((
struct _dyneither_ptr)*f,((_tmp8A8="size",_tag_dyneither(_tmp8A8,sizeof(char),5))))
== 0))goto _LL199;};_LL198:{const char*_tmp8AC;void*_tmp8AB[1];struct Cyc_String_pa_PrintArg_struct
_tmp8AA;(_tmp8AA.tag=0,((_tmp8AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmp8AB[0]=& _tmp8AA,Cyc_Tcutil_warn(e->loc,((
_tmp8AC="deprecated `.size' used here -- change to numelts(%s)",_tag_dyneither(
_tmp8AC,sizeof(char),54))),_tag_dyneither(_tmp8AB,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp8B2;struct Cyc_List_List*_tmp8B1;
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp8B0;outer_e->r=(void*)((
_tmp8B0=_cycalloc(sizeof(*_tmp8B0)),((_tmp8B0[0]=((_tmp8B2.tag=2,((_tmp8B2.f1=
Cyc_Absyn_Numelts,((_tmp8B2.f2=((_tmp8B1=_cycalloc(sizeof(*_tmp8B1)),((_tmp8B1->hd=
e,((_tmp8B1->tl=0,_tmp8B1)))))),_tmp8B2)))))),_tmp8B0))));}{void*_tmp37F=Cyc_Absyn_uint_typ;
_npop_handler(0);return _tmp37F;};_LL199:;_LL19A: {const char*_tmp8B3;if(Cyc_strcmp((
struct _dyneither_ptr)*f,((_tmp8B3="size",_tag_dyneither(_tmp8B3,sizeof(char),5))))
== 0){const char*_tmp8B7;void*_tmp8B6[1];struct Cyc_String_pa_PrintArg_struct
_tmp8B5;void*_tmp384=(_tmp8B5.tag=0,((_tmp8B5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp8B6[0]=&
_tmp8B5,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8B7="expecting struct, union, or array, found %s",
_tag_dyneither(_tmp8B7,sizeof(char),44))),_tag_dyneither(_tmp8B6,sizeof(void*),1)))))));
_npop_handler(0);return _tmp384;}else{const char*_tmp8BB;void*_tmp8BA[1];struct Cyc_String_pa_PrintArg_struct
_tmp8B9;void*_tmp388=(_tmp8B9.tag=0,((_tmp8B9.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp8BA[0]=&
_tmp8B9,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8BB="expecting struct or union, found %s",
_tag_dyneither(_tmp8BB,sizeof(char),36))),_tag_dyneither(_tmp8BA,sizeof(void*),1)))))));
_npop_handler(0);return _tmp388;}}_LL190:;};;_pop_region(r);}static void*Cyc_Tcexp_tcAggrArrow(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*f,int*is_tagged,int*is_read){struct _RegionHandle _tmp389=
_new_region("r");struct _RegionHandle*r=& _tmp389;_push_region(r);Cyc_Tcexp_tcExp(
Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te)),0,e);*is_read=!Cyc_Tcenv_in_notreadctxt(
te);{void*_tmp38A=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_Absyn_PtrInfo
_tmp38C;void*_tmp38D;struct Cyc_Absyn_PtrAtts _tmp38E;void*_tmp38F;union Cyc_Absyn_Constraint*
_tmp390;union Cyc_Absyn_Constraint*_tmp391;_LL19C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp38B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38A;if(_tmp38B->tag
!= 5)goto _LL19E;else{_tmp38C=_tmp38B->f1;_tmp38D=_tmp38C.elt_typ;_tmp38E=_tmp38C.ptr_atts;
_tmp38F=_tmp38E.rgn;_tmp390=_tmp38E.bounds;_tmp391=_tmp38E.zero_term;}}_LL19D:
Cyc_Tcutil_check_nonzero_bound(loc,_tmp390);{void*_tmp392=Cyc_Tcutil_compress(
_tmp38D);struct Cyc_Absyn_AggrInfo _tmp394;union Cyc_Absyn_AggrInfoU _tmp395;struct
Cyc_Absyn_Aggrdecl**_tmp396;struct Cyc_Absyn_Aggrdecl*_tmp397;struct Cyc_List_List*
_tmp398;enum Cyc_Absyn_AggrKind _tmp39A;struct Cyc_List_List*_tmp39B;_LL1A1: {
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp393=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmp392;if(_tmp393->tag != 11)goto _LL1A3;else{_tmp394=_tmp393->f1;_tmp395=_tmp394.aggr_info;
if((_tmp395.KnownAggr).tag != 2)goto _LL1A3;_tmp396=(struct Cyc_Absyn_Aggrdecl**)(
_tmp395.KnownAggr).val;_tmp397=*_tmp396;_tmp398=_tmp394.targs;}}_LL1A2: {struct
Cyc_Absyn_Aggrfield*_tmp39C=Cyc_Absyn_lookup_decl_field(_tmp397,f);if(_tmp39C == 
0){const char*_tmp8C0;void*_tmp8BF[2];struct Cyc_String_pa_PrintArg_struct _tmp8BE;
struct Cyc_String_pa_PrintArg_struct _tmp8BD;void*_tmp3A1=(_tmp8BD.tag=0,((_tmp8BD.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8BE.tag=0,((_tmp8BE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp397->name)),((
_tmp8BF[0]=& _tmp8BE,((_tmp8BF[1]=& _tmp8BD,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp8C0="type %s has no %s field",_tag_dyneither(_tmp8C0,sizeof(char),24))),
_tag_dyneither(_tmp8BF,sizeof(void*),2)))))))))))));_npop_handler(0);return
_tmp3A1;}if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp397->impl))->tagged)*
is_tagged=1;{void*t3=_tmp39C->type;if(_tmp398 != 0){struct _RegionHandle _tmp3A2=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp3A2;_push_region(rgn);{struct Cyc_List_List*
_tmp3A3=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp397->tvs,
_tmp398);t3=Cyc_Tcutil_rsubstitute(rgn,_tmp3A3,_tmp39C->type);};_pop_region(rgn);}{
struct Cyc_Absyn_Kind*_tmp3A4=Cyc_Tcutil_typ_kind(t3);if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,
_tmp3A4)){const char*_tmp8C4;void*_tmp8C3[1];struct Cyc_String_pa_PrintArg_struct
_tmp8C2;void*_tmp3A8=(_tmp8C2.tag=0,((_tmp8C2.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp8C3[0]=& _tmp8C2,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp8C4="cannot get member %s since its type is abstract",_tag_dyneither(_tmp8C4,
sizeof(char),48))),_tag_dyneither(_tmp8C3,sizeof(void*),1)))))));_npop_handler(0);
return _tmp3A8;}if(((_tmp397->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp397->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t3)){const char*_tmp8C8;void*_tmp8C7[1];struct Cyc_String_pa_PrintArg_struct
_tmp8C6;void*_tmp3AC=(_tmp8C6.tag=0,((_tmp8C6.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp8C7[0]=& _tmp8C6,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp8C8="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp8C8,sizeof(char),56))),_tag_dyneither(_tmp8C7,sizeof(void*),1)))))));
_npop_handler(0);return _tmp3AC;}if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp397->impl))->exist_vars != 0){if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te)))){const char*_tmp8CC;void*_tmp8CB[1];struct Cyc_String_pa_PrintArg_struct
_tmp8CA;void*_tmp3B0=(_tmp8CA.tag=0,((_tmp8CA.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp8CB[0]=& _tmp8CA,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp8CC="must use pattern-matching to access field %s\n\tdue to extistential types",
_tag_dyneither(_tmp8CC,sizeof(char),72))),_tag_dyneither(_tmp8CB,sizeof(void*),1)))))));
_npop_handler(0);return _tmp3B0;}}{void*_tmp3B1=t3;_npop_handler(0);return _tmp3B1;};};};}
_LL1A3: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp399=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)
_tmp392;if(_tmp399->tag != 12)goto _LL1A5;else{_tmp39A=_tmp399->f1;_tmp39B=_tmp399->f2;}}
_LL1A4: {struct Cyc_Absyn_Aggrfield*_tmp3B2=Cyc_Absyn_lookup_field(_tmp39B,f);if(
_tmp3B2 == 0){const char*_tmp8D0;void*_tmp8CF[1];struct Cyc_String_pa_PrintArg_struct
_tmp8CE;void*_tmp3B6=(_tmp8CE.tag=0,((_tmp8CE.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp8CF[0]=& _tmp8CE,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp8D0="type has no %s field",_tag_dyneither(_tmp8D0,sizeof(char),21))),
_tag_dyneither(_tmp8CF,sizeof(void*),1)))))));_npop_handler(0);return _tmp3B6;}
if((_tmp39A == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
_tmp3B2->type)){const char*_tmp8D4;void*_tmp8D3[1];struct Cyc_String_pa_PrintArg_struct
_tmp8D2;void*_tmp3BA=(_tmp8D2.tag=0,((_tmp8D2.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp8D3[0]=& _tmp8D2,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp8D4="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp8D4,sizeof(char),56))),_tag_dyneither(_tmp8D3,sizeof(void*),1)))))));
_npop_handler(0);return _tmp3BA;}{void*_tmp3BB=_tmp3B2->type;_npop_handler(0);
return _tmp3BB;};}_LL1A5:;_LL1A6: goto _LL1A0;_LL1A0:;}goto _LL19B;_LL19E:;_LL19F:
goto _LL19B;_LL19B:;}{const char*_tmp8D8;void*_tmp8D7[1];struct Cyc_String_pa_PrintArg_struct
_tmp8D6;void*_tmp3BF=(_tmp8D6.tag=0,((_tmp8D6.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp8D7[0]=&
_tmp8D6,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8D8="expecting struct or union pointer, found %s",
_tag_dyneither(_tmp8D8,sizeof(char),44))),_tag_dyneither(_tmp8D7,sizeof(void*),1)))))));
_npop_handler(0);return _tmp3BF;};;_pop_region(r);}static void*Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*
index){unsigned int _tmp3C1;int _tmp3C2;struct _tuple15 _tmp3C0=Cyc_Evexp_eval_const_uint_exp(
index);_tmp3C1=_tmp3C0.f1;_tmp3C2=_tmp3C0.f2;if(!_tmp3C2){const char*_tmp8DB;void*
_tmp8DA;return(_tmp8DA=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8DB="tuple projection cannot use sizeof or offsetof",
_tag_dyneither(_tmp8DB,sizeof(char),47))),_tag_dyneither(_tmp8DA,sizeof(void*),0)));}{
struct _handler_cons _tmp3C5;_push_handler(& _tmp3C5);{int _tmp3C7=0;if(setjmp(
_tmp3C5.handler))_tmp3C7=1;if(!_tmp3C7){{void*_tmp3C8=(*((struct _tuple19*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp3C1)).f2;_npop_handler(0);
return _tmp3C8;};_pop_handler();}else{void*_tmp3C6=(void*)_exn_thrown;void*
_tmp3CA=_tmp3C6;_LL1A8: {struct Cyc_List_Nth_exn_struct*_tmp3CB=(struct Cyc_List_Nth_exn_struct*)
_tmp3CA;if(_tmp3CB->tag != Cyc_List_Nth)goto _LL1AA;}_LL1A9: {const char*_tmp8E0;
void*_tmp8DF[2];struct Cyc_Int_pa_PrintArg_struct _tmp8DE;struct Cyc_Int_pa_PrintArg_struct
_tmp8DD;return(_tmp8DD.tag=1,((_tmp8DD.f1=(unsigned long)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(ts),((_tmp8DE.tag=1,((_tmp8DE.f1=(unsigned long)((int)
_tmp3C1),((_tmp8DF[0]=& _tmp8DE,((_tmp8DF[1]=& _tmp8DD,Cyc_Tcexp_expr_err(te,loc,0,((
_tmp8E0="index is %d but tuple has only %d fields",_tag_dyneither(_tmp8E0,
sizeof(char),41))),_tag_dyneither(_tmp8DF,sizeof(void*),2)))))))))))));}_LL1AA:;
_LL1AB:(void)_throw(_tmp3CA);_LL1A7:;}};};}static void*Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){struct _RegionHandle _tmp3D0=_new_region("r");struct
_RegionHandle*r=& _tmp3D0;_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp3D1=Cyc_Tcenv_clear_lhs(
r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));Cyc_Tcexp_tcExp(_tmp3D1,0,e1);Cyc_Tcexp_tcExp(
_tmp3D1,0,e2);{void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*t2=
Cyc_Tcutil_compress((void*)_check_null(e2->topt));if(!Cyc_Tcutil_coerce_sint_typ(
_tmp3D1,e2)){const char*_tmp8E4;void*_tmp8E3[1];struct Cyc_String_pa_PrintArg_struct
_tmp8E2;void*_tmp3D5=(_tmp8E2.tag=0,((_tmp8E2.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp8E3[0]=& _tmp8E2,Cyc_Tcexp_expr_err(
_tmp3D1,e2->loc,topt,((_tmp8E4="expecting int subscript, found %s",
_tag_dyneither(_tmp8E4,sizeof(char),34))),_tag_dyneither(_tmp8E3,sizeof(void*),1)))))));
_npop_handler(0);return _tmp3D5;}{void*_tmp3D6=t1;struct Cyc_Absyn_PtrInfo _tmp3D8;
void*_tmp3D9;struct Cyc_Absyn_Tqual _tmp3DA;struct Cyc_Absyn_PtrAtts _tmp3DB;void*
_tmp3DC;union Cyc_Absyn_Constraint*_tmp3DD;union Cyc_Absyn_Constraint*_tmp3DE;
struct Cyc_List_List*_tmp3E0;_LL1AD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp3D7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D6;if(_tmp3D7->tag
!= 5)goto _LL1AF;else{_tmp3D8=_tmp3D7->f1;_tmp3D9=_tmp3D8.elt_typ;_tmp3DA=_tmp3D8.elt_tq;
_tmp3DB=_tmp3D8.ptr_atts;_tmp3DC=_tmp3DB.rgn;_tmp3DD=_tmp3DB.bounds;_tmp3DE=
_tmp3DB.zero_term;}}_LL1AE: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp3DE)){int emit_warning=1;{void*_tmp3E1=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp3DD);struct Cyc_Absyn_Exp*
_tmp3E3;_LL1B4: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3E2=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)
_tmp3E1;if(_tmp3E2->tag != 1)goto _LL1B6;else{_tmp3E3=_tmp3E2->f1;}}_LL1B5: if(Cyc_Tcutil_is_const_exp(
_tmp3D1,e2)){unsigned int _tmp3E6;int _tmp3E7;struct _tuple15 _tmp3E5=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp3E6=_tmp3E5.f1;_tmp3E7=_tmp3E5.f2;if(_tmp3E7){unsigned int _tmp3E9;int
_tmp3EA;struct _tuple15 _tmp3E8=Cyc_Evexp_eval_const_uint_exp(_tmp3E3);_tmp3E9=
_tmp3E8.f1;_tmp3EA=_tmp3E8.f2;if(_tmp3EA  && _tmp3E9 > _tmp3E6)emit_warning=0;}}
goto _LL1B3;_LL1B6: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp3E4=(
struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp3E1;if(_tmp3E4->tag != 0)
goto _LL1B3;}_LL1B7: emit_warning=0;goto _LL1B3;_LL1B3:;}if(emit_warning){const char*
_tmp8E7;void*_tmp8E6;(_tmp8E6=0,Cyc_Tcutil_warn(e2->loc,((_tmp8E7="subscript on thin, zero-terminated pointer could be expensive.",
_tag_dyneither(_tmp8E7,sizeof(char),63))),_tag_dyneither(_tmp8E6,sizeof(void*),0)));}}
else{if(Cyc_Tcutil_is_const_exp(_tmp3D1,e2)){unsigned int _tmp3EE;int _tmp3EF;
struct _tuple15 _tmp3ED=Cyc_Evexp_eval_const_uint_exp(e2);_tmp3EE=_tmp3ED.f1;
_tmp3EF=_tmp3ED.f2;if(_tmp3EF)Cyc_Tcutil_check_bound(loc,_tmp3EE,_tmp3DD);}else{
if(Cyc_Tcutil_is_bound_one(_tmp3DD) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp3DE)){const char*_tmp8EA;void*_tmp8E9;(_tmp8E9=0,
Cyc_Tcutil_warn(e1->loc,((_tmp8EA="subscript applied to pointer to one object",
_tag_dyneither(_tmp8EA,sizeof(char),43))),_tag_dyneither(_tmp8E9,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp3DD);}}Cyc_Tcenv_check_rgn_accessible(
_tmp3D1,loc,_tmp3DC);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp3D9),& Cyc_Tcutil_tmk)){
const char*_tmp8ED;void*_tmp8EC;(_tmp8EC=0,Cyc_Tcutil_terr(e1->loc,((_tmp8ED="can't subscript an abstract pointer",
_tag_dyneither(_tmp8ED,sizeof(char),36))),_tag_dyneither(_tmp8EC,sizeof(void*),0)));}{
void*_tmp3F4=_tmp3D9;_npop_handler(0);return _tmp3F4;};_LL1AF: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmp3DF=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3D6;if(_tmp3DF->tag != 
10)goto _LL1B1;else{_tmp3E0=_tmp3DF->f1;}}_LL1B0: {void*_tmp3F5=Cyc_Tcexp_ithTupleType(
_tmp3D1,loc,_tmp3E0,e2);_npop_handler(0);return _tmp3F5;}_LL1B1:;_LL1B2: {const
char*_tmp8F1;void*_tmp8F0[1];struct Cyc_String_pa_PrintArg_struct _tmp8EF;void*
_tmp3F9=(_tmp8EF.tag=0,((_tmp8EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp8F0[0]=& _tmp8EF,Cyc_Tcexp_expr_err(_tmp3D1,loc,
topt,((_tmp8F1="subscript applied to %s",_tag_dyneither(_tmp8F1,sizeof(char),24))),
_tag_dyneither(_tmp8F0,sizeof(void*),1)))))));_npop_handler(0);return _tmp3F9;}
_LL1AC:;};};};_pop_region(r);}static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*
te,unsigned int loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*
fields=0;if(topt != 0){void*_tmp3FA=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*
_tmp3FC;_LL1B9: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3FB=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)
_tmp3FA;if(_tmp3FB->tag != 10)goto _LL1BB;else{_tmp3FC=_tmp3FB->f1;}}_LL1BA: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3FC)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1B8;for(0;es != 0;(es=es->tl,_tmp3FC=_tmp3FC->tl)){
int bogus=0;void*_tmp3FD=(*((struct _tuple19*)((struct Cyc_List_List*)_check_null(
_tmp3FC))->hd)).f2;Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp3FD,(struct Cyc_Absyn_Exp*)
es->hd);Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple19*)
_tmp3FC->hd)).f2,& bogus);{struct _tuple19*_tmp8F4;struct Cyc_List_List*_tmp8F3;
fields=((_tmp8F3=_cycalloc(sizeof(*_tmp8F3)),((_tmp8F3->hd=((_tmp8F4=_cycalloc(
sizeof(*_tmp8F4)),((_tmp8F4->f1=(*((struct _tuple19*)_tmp3FC->hd)).f1,((_tmp8F4->f2=(
void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp8F4)))))),((_tmp8F3->tl=
fields,_tmp8F3))))));};}done=1;goto _LL1B8;_LL1BB:;_LL1BC: goto _LL1B8;_LL1B8:;}if(
!done)for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)
es->hd);{struct _tuple19*_tmp8F7;struct Cyc_List_List*_tmp8F6;fields=((_tmp8F6=
_cycalloc(sizeof(*_tmp8F6)),((_tmp8F6->hd=((_tmp8F7=_cycalloc(sizeof(*_tmp8F7)),((
_tmp8F7->f1=Cyc_Absyn_empty_tqual(0),((_tmp8F7->f2=(void*)_check_null(((struct
Cyc_Absyn_Exp*)es->hd)->topt),_tmp8F7)))))),((_tmp8F6->tl=fields,_tmp8F6))))));};}{
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp8FA;struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmp8F9;return(void*)((_tmp8F9=_cycalloc(sizeof(*_tmp8F9)),((_tmp8F9[0]=((
_tmp8FA.tag=10,((_tmp8FA.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmp8FA)))),_tmp8F9))));};}static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*
te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){const char*
_tmp8FD;void*_tmp8FC;return(_tmp8FC=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8FD="tcCompoundLit",
_tag_dyneither(_tmp8FD,sizeof(char),14))),_tag_dyneither(_tmp8FC,sizeof(void*),0)));}
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**
elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
void*res_t2;struct _RegionHandle _tmp406=_new_region("r");struct _RegionHandle*r=&
_tmp406;_push_region(r);{int _tmp407=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
des);struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(
r,(struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));struct
Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp900;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp8FF;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp408=(_tmp8FF=_cycalloc(
sizeof(*_tmp8FF)),((_tmp8FF[0]=((_tmp900.tag=0,((_tmp900.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,
_tmp407),_tmp900)))),_tmp8FF)));struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((
void*)_tmp408,loc);if(zero_term){struct Cyc_Absyn_Exp*_tmp409=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp407 - 1);if(!Cyc_Tcutil_is_zero(
_tmp409)){const char*_tmp903;void*_tmp902;(_tmp902=0,Cyc_Tcutil_terr(_tmp409->loc,((
_tmp903="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp903,
sizeof(char),45))),_tag_dyneither(_tmp902,sizeof(void*),0)));}}sz_exp->topt=(
void*)Cyc_Absyn_uint_typ;res_t2=Cyc_Absyn_array_typ(res,(unsigned int)elt_tqopt?*
elt_tqopt: Cyc_Absyn_empty_tqual(0),(struct Cyc_Absyn_Exp*)sz_exp,zero_term?Cyc_Absyn_true_conref:
Cyc_Absyn_false_conref,0);{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(
te,res,es)){const char*_tmp907;void*_tmp906[1];struct Cyc_String_pa_PrintArg_struct
_tmp905;(_tmp905.tag=0,((_tmp905.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(res)),((_tmp906[0]=& _tmp905,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp907="elements of array do not all have the same type (%s)",
_tag_dyneither(_tmp907,sizeof(char),53))),_tag_dyneither(_tmp906,sizeof(void*),1)))))));}}{
int offset=0;for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((
struct _tuple18*)des->hd)).f1;if(ds != 0){void*_tmp411=(void*)ds->hd;struct Cyc_Absyn_Exp*
_tmp414;_LL1BE: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp412=(
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp411;if(_tmp412->tag != 1)
goto _LL1C0;}_LL1BF:{const char*_tmp90A;void*_tmp909;(_tmp909=0,Cyc_Tcutil_terr(
loc,((_tmp90A="only array index designators are supported",_tag_dyneither(
_tmp90A,sizeof(char),43))),_tag_dyneither(_tmp909,sizeof(void*),0)));}goto _LL1BD;
_LL1C0: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp413=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)
_tmp411;if(_tmp413->tag != 0)goto _LL1BD;else{_tmp414=_tmp413->f1;}}_LL1C1: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp414);{unsigned int _tmp418;int _tmp419;struct _tuple15 _tmp417=Cyc_Evexp_eval_const_uint_exp(
_tmp414);_tmp418=_tmp417.f1;_tmp419=_tmp417.f2;if(!_tmp419){const char*_tmp90D;
void*_tmp90C;(_tmp90C=0,Cyc_Tcutil_terr(_tmp414->loc,((_tmp90D="index designator cannot use sizeof or offsetof",
_tag_dyneither(_tmp90D,sizeof(char),47))),_tag_dyneither(_tmp90C,sizeof(void*),0)));}
else{if(_tmp418 != offset){const char*_tmp912;void*_tmp911[2];struct Cyc_Int_pa_PrintArg_struct
_tmp910;struct Cyc_Int_pa_PrintArg_struct _tmp90F;(_tmp90F.tag=1,((_tmp90F.f1=(
unsigned long)((int)_tmp418),((_tmp910.tag=1,((_tmp910.f1=(unsigned long)offset,((
_tmp911[0]=& _tmp910,((_tmp911[1]=& _tmp90F,Cyc_Tcutil_terr(_tmp414->loc,((_tmp912="expecting index designator %d but found %d",
_tag_dyneither(_tmp912,sizeof(char),43))),_tag_dyneither(_tmp911,sizeof(void*),2)))))))))))));}}
goto _LL1BD;};_LL1BD:;}}}{void*_tmp420=res_t2;_npop_handler(0);return _tmp420;};;
_pop_region(r);}static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,
unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,
struct Cyc_Absyn_Exp*body,int*is_zero_term){Cyc_Tcexp_tcExp(te,0,bound);{void*
_tmp421=Cyc_Tcutil_compress((void*)_check_null(bound->topt));_LL1C3: {struct Cyc_Absyn_TagType_Absyn_Type_struct*
_tmp422=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp421;if(_tmp422->tag != 19)
goto _LL1C5;}_LL1C4: goto _LL1C2;_LL1C5:;_LL1C6: if(!Cyc_Tcutil_coerce_uint_typ(te,
bound)){const char*_tmp916;void*_tmp915[1];struct Cyc_String_pa_PrintArg_struct
_tmp914;(_tmp914.tag=0,((_tmp914.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp915[0]=& _tmp914,
Cyc_Tcutil_terr(bound->loc,((_tmp916="expecting unsigned int, found %s",
_tag_dyneither(_tmp916,sizeof(char),33))),_tag_dyneither(_tmp915,sizeof(void*),1)))))));}
_LL1C2:;}if(!(vd->tq).real_const){const char*_tmp919;void*_tmp918;(_tmp918=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp919="comprehension index variable is not declared const!",_tag_dyneither(
_tmp919,sizeof(char),52))),_tag_dyneither(_tmp918,sizeof(void*),0)));}{struct
_RegionHandle _tmp428=_new_region("r");struct _RegionHandle*r=& _tmp428;
_push_region(r);{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(r,te);if(te2->le != 
0){te2=Cyc_Tcenv_new_block(r,loc,te2);te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{
struct Cyc_Tcenv_Tenv*_tmp429=te2;void**_tmp42A=0;struct Cyc_Absyn_Tqual*_tmp42B=0;
union Cyc_Absyn_Constraint**_tmp42C=0;if(topt != 0){void*_tmp42D=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp42F;struct Cyc_Absyn_ArrayInfo _tmp431;void*
_tmp432;struct Cyc_Absyn_Tqual _tmp433;struct Cyc_Absyn_Exp*_tmp434;union Cyc_Absyn_Constraint*
_tmp435;_LL1C8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp42E=(struct
Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42D;if(_tmp42E->tag != 5)goto _LL1CA;
else{_tmp42F=_tmp42E->f1;}}_LL1C9:{void**_tmp91A;_tmp42A=((_tmp91A=
_region_malloc(r,sizeof(*_tmp91A)),((_tmp91A[0]=_tmp42F.elt_typ,_tmp91A))));}{
struct Cyc_Absyn_Tqual*_tmp91B;_tmp42B=((_tmp91B=_region_malloc(r,sizeof(*_tmp91B)),((
_tmp91B[0]=_tmp42F.elt_tq,_tmp91B))));}{union Cyc_Absyn_Constraint**_tmp91C;
_tmp42C=((_tmp91C=_region_malloc(r,sizeof(*_tmp91C)),((_tmp91C[0]=(_tmp42F.ptr_atts).zero_term,
_tmp91C))));}goto _LL1C7;_LL1CA: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp430=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp42D;if(_tmp430->tag != 
8)goto _LL1CC;else{_tmp431=_tmp430->f1;_tmp432=_tmp431.elt_type;_tmp433=_tmp431.tq;
_tmp434=_tmp431.num_elts;_tmp435=_tmp431.zero_term;}}_LL1CB:{void**_tmp91D;
_tmp42A=((_tmp91D=_region_malloc(r,sizeof(*_tmp91D)),((_tmp91D[0]=_tmp432,
_tmp91D))));}{struct Cyc_Absyn_Tqual*_tmp91E;_tmp42B=((_tmp91E=_region_malloc(r,
sizeof(*_tmp91E)),((_tmp91E[0]=_tmp433,_tmp91E))));}{union Cyc_Absyn_Constraint**
_tmp91F;_tmp42C=((_tmp91F=_region_malloc(r,sizeof(*_tmp91F)),((_tmp91F[0]=
_tmp435,_tmp91F))));}goto _LL1C7;_LL1CC:;_LL1CD: goto _LL1C7;_LL1C7:;}{void*t=Cyc_Tcexp_tcExp(
_tmp429,_tmp42A,body);if(_tmp429->le == 0){if(!Cyc_Tcutil_is_const_exp(_tmp429,
bound)){const char*_tmp922;void*_tmp921;(_tmp921=0,Cyc_Tcutil_terr(bound->loc,((
_tmp922="bound is not constant",_tag_dyneither(_tmp922,sizeof(char),22))),
_tag_dyneither(_tmp921,sizeof(void*),0)));}if(!Cyc_Tcutil_is_const_exp(_tmp429,
body)){const char*_tmp925;void*_tmp924;(_tmp924=0,Cyc_Tcutil_terr(bound->loc,((
_tmp925="body is not constant",_tag_dyneither(_tmp925,sizeof(char),21))),
_tag_dyneither(_tmp924,sizeof(void*),0)));}}if(_tmp42C != 0  && ((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp42C)){struct Cyc_Absyn_Exp*
_tmp440=Cyc_Absyn_uint_exp(1,0);_tmp440->topt=(void*)Cyc_Absyn_uint_typ;bound=
Cyc_Absyn_add_exp(bound,_tmp440,0);bound->topt=(void*)Cyc_Absyn_uint_typ;*
is_zero_term=1;}{void*_tmp441=Cyc_Absyn_array_typ(t,_tmp42B == 0?Cyc_Absyn_empty_tqual(
0):*_tmp42B,(struct Cyc_Absyn_Exp*)bound,_tmp42C == 0?Cyc_Absyn_false_conref:*
_tmp42C,0);void*_tmp442=_tmp441;_npop_handler(0);return _tmp442;};};};};
_pop_region(r);};}struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*
f2;};static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,
void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,
struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl**adptr;struct Cyc_Absyn_Aggrdecl*
ad;if(*ad_opt != 0){ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);{struct Cyc_Absyn_Aggrdecl**
_tmp926;adptr=((_tmp926=_cycalloc(sizeof(*_tmp926)),((_tmp926[0]=ad,_tmp926))));};}
else{{struct _handler_cons _tmp444;_push_handler(& _tmp444);{int _tmp446=0;if(setjmp(
_tmp444.handler))_tmp446=1;if(!_tmp446){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*
tn);ad=*adptr;;_pop_handler();}else{void*_tmp445=(void*)_exn_thrown;void*_tmp448=
_tmp445;_LL1CF: {struct Cyc_Dict_Absent_exn_struct*_tmp449=(struct Cyc_Dict_Absent_exn_struct*)
_tmp448;if(_tmp449->tag != Cyc_Dict_Absent)goto _LL1D1;}_LL1D0:{const char*_tmp92A;
void*_tmp929[1];struct Cyc_String_pa_PrintArg_struct _tmp928;(_tmp928.tag=0,((
_tmp928.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*
tn)),((_tmp929[0]=& _tmp928,Cyc_Tcutil_terr(loc,((_tmp92A="unbound struct/union name %s",
_tag_dyneither(_tmp92A,sizeof(char),29))),_tag_dyneither(_tmp929,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1D1:;_LL1D2:(void)_throw(
_tmp448);_LL1CE:;}};}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;*tn=ad->name;}if(ad->impl
== 0){{const char*_tmp932;void*_tmp931[1];const char*_tmp930;const char*_tmp92F;
struct Cyc_String_pa_PrintArg_struct _tmp92E;(_tmp92E.tag=0,((_tmp92E.f1=(struct
_dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp92F="struct",_tag_dyneither(
_tmp92F,sizeof(char),7)):((_tmp930="union",_tag_dyneither(_tmp930,sizeof(char),6)))),((
_tmp931[0]=& _tmp92E,Cyc_Tcutil_terr(loc,((_tmp932="can't construct abstract %s",
_tag_dyneither(_tmp932,sizeof(char),28))),_tag_dyneither(_tmp931,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{struct
_RegionHandle _tmp452=_new_region("rgn");struct _RegionHandle*rgn=& _tmp452;
_push_region(rgn);{struct _tuple12 _tmp933;struct _tuple12 _tmp453=(_tmp933.f1=Cyc_Tcenv_lookup_type_vars(
te),((_tmp933.f2=rgn,_tmp933)));struct Cyc_List_List*_tmp454=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),
struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp453,ad->tvs);struct Cyc_List_List*_tmp455=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct
_tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp453,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp456=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,
_tmp454);struct Cyc_List_List*_tmp457=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,
_tmp455);struct Cyc_List_List*_tmp458=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp454,_tmp455);void*res_typ;if(topt != 0){void*_tmp459=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_AggrInfo _tmp45B;union Cyc_Absyn_AggrInfoU _tmp45C;struct Cyc_Absyn_Aggrdecl**
_tmp45D;struct Cyc_List_List*_tmp45E;_LL1D4: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp45A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp459;if(_tmp45A->tag != 
11)goto _LL1D6;else{_tmp45B=_tmp45A->f1;_tmp45C=_tmp45B.aggr_info;if((_tmp45C.KnownAggr).tag
!= 2)goto _LL1D6;_tmp45D=(struct Cyc_Absyn_Aggrdecl**)(_tmp45C.KnownAggr).val;
_tmp45E=_tmp45B.targs;}}_LL1D5: if(*_tmp45D == *adptr){{struct Cyc_List_List*
_tmp45F=_tmp456;for(0;_tmp45F != 0  && _tmp45E != 0;(_tmp45F=_tmp45F->tl,_tmp45E=
_tmp45E->tl)){Cyc_Tcutil_unify((void*)_tmp45F->hd,(void*)_tmp45E->hd);}}res_typ=*
topt;goto _LL1D3;}goto _LL1D7;_LL1D6:;_LL1D7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct
_tmp939;struct Cyc_Absyn_AggrInfo _tmp938;struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp937;res_typ=(void*)((_tmp937=_cycalloc(sizeof(*_tmp937)),((_tmp937[0]=((
_tmp939.tag=11,((_tmp939.f1=((_tmp938.aggr_info=Cyc_Absyn_KnownAggr(adptr),((
_tmp938.targs=_tmp456,_tmp938)))),_tmp939)))),_tmp937))));}_LL1D3:;}else{struct
Cyc_Absyn_AggrType_Absyn_Type_struct _tmp93F;struct Cyc_Absyn_AggrInfo _tmp93E;
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp93D;res_typ=(void*)((_tmp93D=
_cycalloc(sizeof(*_tmp93D)),((_tmp93D[0]=((_tmp93F.tag=11,((_tmp93F.f1=((_tmp93E.aggr_info=
Cyc_Absyn_KnownAggr(adptr),((_tmp93E.targs=_tmp456,_tmp93E)))),_tmp93F)))),
_tmp93D))));}{struct Cyc_List_List*_tmp466=*ts;struct Cyc_List_List*_tmp467=
_tmp457;while(_tmp466 != 0  && _tmp467 != 0){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),& Cyc_Tcutil_ak,1,(void*)_tmp466->hd);Cyc_Tcutil_check_no_qual(loc,(void*)
_tmp466->hd);Cyc_Tcutil_unify((void*)_tmp466->hd,(void*)_tmp467->hd);_tmp466=
_tmp466->tl;_tmp467=_tmp467->tl;}if(_tmp466 != 0){const char*_tmp942;void*_tmp941;(
_tmp941=0,Cyc_Tcutil_terr(loc,((_tmp942="too many explicit witness types",
_tag_dyneither(_tmp942,sizeof(char),32))),_tag_dyneither(_tmp941,sizeof(void*),0)));}*
ts=_tmp457;{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,
args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){int bogus=0;struct _tuple20 _tmp46B;struct Cyc_Absyn_Aggrfield*
_tmp46C;struct Cyc_Absyn_Exp*_tmp46D;struct _tuple20*_tmp46A=(struct _tuple20*)
fields->hd;_tmp46B=*_tmp46A;_tmp46C=_tmp46B.f1;_tmp46D=_tmp46B.f2;{void*_tmp46E=
Cyc_Tcutil_rsubstitute(rgn,_tmp458,_tmp46C->type);Cyc_Tcexp_tcExpInitializer(te,(
void**)& _tmp46E,_tmp46D);if(!Cyc_Tcutil_coerce_arg(te,_tmp46D,_tmp46E,& bogus)){{
const char*_tmp94E;void*_tmp94D[5];struct Cyc_String_pa_PrintArg_struct _tmp94C;
const char*_tmp94B;const char*_tmp94A;struct Cyc_String_pa_PrintArg_struct _tmp949;
struct Cyc_String_pa_PrintArg_struct _tmp948;struct Cyc_String_pa_PrintArg_struct
_tmp947;struct Cyc_String_pa_PrintArg_struct _tmp946;(_tmp946.tag=0,((_tmp946.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)
_check_null(_tmp46D->topt))),((_tmp947.tag=0,((_tmp947.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp46E)),((_tmp948.tag=0,((_tmp948.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((
_tmp949.tag=0,((_tmp949.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(
_tmp94A="struct",_tag_dyneither(_tmp94A,sizeof(char),7)):((_tmp94B="union",
_tag_dyneither(_tmp94B,sizeof(char),6)))),((_tmp94C.tag=0,((_tmp94C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp46C->name),((_tmp94D[0]=& _tmp94C,((
_tmp94D[1]=& _tmp949,((_tmp94D[2]=& _tmp948,((_tmp94D[3]=& _tmp947,((_tmp94D[4]=&
_tmp946,Cyc_Tcutil_terr(_tmp46D->loc,((_tmp94E="field %s of %s %s expects type %s != %s",
_tag_dyneither(_tmp94E,sizeof(char),40))),_tag_dyneither(_tmp94D,sizeof(void*),5)))))))))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();}};}{struct Cyc_List_List*_tmp478=0;{struct Cyc_List_List*
_tmp479=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;
_tmp479 != 0;_tmp479=_tmp479->tl){struct _tuple0*_tmp951;struct Cyc_List_List*
_tmp950;_tmp478=((_tmp950=_cycalloc(sizeof(*_tmp950)),((_tmp950->hd=((_tmp951=
_cycalloc(sizeof(*_tmp951)),((_tmp951->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp458,(*((
struct _tuple0*)_tmp479->hd)).f1),((_tmp951->f2=Cyc_Tcutil_rsubstitute(rgn,
_tmp458,(*((struct _tuple0*)_tmp479->hd)).f2),_tmp951)))))),((_tmp950->tl=_tmp478,
_tmp950))))));}}_tmp478=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp478);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp478);{void*_tmp47C=res_typ;
_npop_handler(0);return _tmp47C;};};};};};_pop_region(rgn);};}static void*Cyc_Tcexp_tcAnonStruct(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){struct
_RegionHandle _tmp47E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp47E;
_push_region(rgn);{void*_tmp47F=Cyc_Tcutil_compress(ts);enum Cyc_Absyn_AggrKind
_tmp481;struct Cyc_List_List*_tmp482;_LL1D9: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*
_tmp480=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp47F;if(_tmp480->tag
!= 12)goto _LL1DB;else{_tmp481=_tmp480->f1;_tmp482=_tmp480->f2;}}_LL1DA: if(
_tmp481 == Cyc_Absyn_UnionA){const char*_tmp954;void*_tmp953;(_tmp953=0,Cyc_Tcutil_terr(
loc,((_tmp954="expecting struct but found union",_tag_dyneither(_tmp954,sizeof(
char),33))),_tag_dyneither(_tmp953,sizeof(void*),0)));}{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct
Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
rgn,loc,args,Cyc_Absyn_StructA,_tmp482);for(0;fields != 0;fields=fields->tl){int
bogus=0;struct _tuple20 _tmp486;struct Cyc_Absyn_Aggrfield*_tmp487;struct Cyc_Absyn_Exp*
_tmp488;struct _tuple20*_tmp485=(struct _tuple20*)fields->hd;_tmp486=*_tmp485;
_tmp487=_tmp486.f1;_tmp488=_tmp486.f2;Cyc_Tcexp_tcExpInitializer(te,(void**)&
_tmp487->type,_tmp488);if(!Cyc_Tcutil_coerce_arg(te,_tmp488,_tmp487->type,& bogus)){{
const char*_tmp95A;void*_tmp959[3];struct Cyc_String_pa_PrintArg_struct _tmp958;
struct Cyc_String_pa_PrintArg_struct _tmp957;struct Cyc_String_pa_PrintArg_struct
_tmp956;(_tmp956.tag=0,((_tmp956.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp488->topt))),((_tmp957.tag=0,((
_tmp957.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp487->type)),((_tmp958.tag=0,((_tmp958.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp487->name),((_tmp959[0]=& _tmp958,((_tmp959[1]=& _tmp957,((
_tmp959[2]=& _tmp956,Cyc_Tcutil_terr(_tmp488->loc,((_tmp95A="field %s of struct expects type %s != %s",
_tag_dyneither(_tmp95A,sizeof(char),41))),_tag_dyneither(_tmp959,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}goto _LL1D8;};_LL1DB:;_LL1DC: {const char*_tmp95D;
void*_tmp95C;(_tmp95C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp95D="tcAnonStruct: wrong type",_tag_dyneither(_tmp95D,
sizeof(char),25))),_tag_dyneither(_tmp95C,sizeof(void*),0)));}_LL1D8:;}{void*
_tmp490=ts;_npop_handler(0);return _tmp490;};;_pop_region(rgn);}static void*Cyc_Tcexp_tcDatatype(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*
tuf){struct _RegionHandle _tmp491=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp491;_push_region(rgn);{struct _tuple12 _tmp95E;struct _tuple12 _tmp492=(_tmp95E.f1=
Cyc_Tcenv_lookup_type_vars(te),((_tmp95E.f2=rgn,_tmp95E)));struct Cyc_List_List*
_tmp493=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(
struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))
Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp492,tud->tvs);struct Cyc_List_List*
_tmp494=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*
x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp493);struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct
_tmp964;struct Cyc_Absyn_DatatypeFieldInfo _tmp963;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*
_tmp962;void*res=(void*)((_tmp962=_cycalloc(sizeof(*_tmp962)),((_tmp962[0]=((
_tmp964.tag=4,((_tmp964.f1=((_tmp963.field_info=Cyc_Absyn_KnownDatatypefield(tud,
tuf),((_tmp963.targs=_tmp494,_tmp963)))),_tmp964)))),_tmp962))));if(topt != 0){
void*_tmp495=Cyc_Tcutil_compress(*topt);_LL1DE: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*
_tmp496=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp495;if(_tmp496->tag
!= 4)goto _LL1E0;}_LL1DF: Cyc_Tcutil_unify(*topt,res);goto _LL1DD;_LL1E0:;_LL1E1:
goto _LL1DD;_LL1DD:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 0  && ts != 0;(es=
es->tl,ts=ts->tl)){int bogus=0;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple19*)ts->hd)).f2;if(_tmp493 != 0)t=Cyc_Tcutil_rsubstitute(
rgn,_tmp493,t);Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(
te,e,t,& bogus)){{const char*_tmp96C;void*_tmp96B[3];struct Cyc_String_pa_PrintArg_struct
_tmp96A;struct Cyc_String_pa_PrintArg_struct _tmp969;const char*_tmp968;struct Cyc_String_pa_PrintArg_struct
_tmp967;(_tmp967.tag=0,((_tmp967.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
e->topt == 0?(struct _dyneither_ptr)((_tmp968="?",_tag_dyneither(_tmp968,sizeof(
char),2))): Cyc_Absynpp_typ2string((void*)_check_null(e->topt)))),((_tmp969.tag=0,((
_tmp969.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp96A.tag=0,((_tmp96A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name)),((_tmp96B[0]=& _tmp96A,((_tmp96B[1]=& _tmp969,((_tmp96B[2]=& _tmp967,
Cyc_Tcutil_terr(e->loc,((_tmp96C="datatype constructor %s expects argument of type %s but this argument has type %s",
_tag_dyneither(_tmp96C,sizeof(char),82))),_tag_dyneither(_tmp96B,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}if(es != 0){const char*_tmp970;void*_tmp96F[1];
struct Cyc_String_pa_PrintArg_struct _tmp96E;void*_tmp4A0=(_tmp96E.tag=0,((_tmp96E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((
_tmp96F[0]=& _tmp96E,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,
topt,((_tmp970="too many arguments for datatype constructor %s",_tag_dyneither(
_tmp970,sizeof(char),47))),_tag_dyneither(_tmp96F,sizeof(void*),1)))))));
_npop_handler(0);return _tmp4A0;}if(ts != 0){const char*_tmp974;void*_tmp973[1];
struct Cyc_String_pa_PrintArg_struct _tmp972;void*_tmp4A4=(_tmp972.tag=0,((_tmp972.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((
_tmp973[0]=& _tmp972,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp974="too few arguments for datatype constructor %s",
_tag_dyneither(_tmp974,sizeof(char),46))),_tag_dyneither(_tmp973,sizeof(void*),1)))))));
_npop_handler(0);return _tmp4A4;}{void*_tmp4A5=res;_npop_handler(0);return _tmp4A5;};};};
_pop_region(rgn);}static int Cyc_Tcexp_zeroable_type(void*t){void*_tmp4AA=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp4B1;struct Cyc_Absyn_PtrAtts _tmp4B2;union Cyc_Absyn_Constraint*
_tmp4B3;struct Cyc_Absyn_ArrayInfo _tmp4B7;void*_tmp4B8;struct Cyc_List_List*
_tmp4BB;struct Cyc_Absyn_AggrInfo _tmp4BD;union Cyc_Absyn_AggrInfoU _tmp4BE;struct
Cyc_List_List*_tmp4BF;struct Cyc_List_List*_tmp4C3;_LL1E3: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*
_tmp4AB=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp4AA;if(_tmp4AB->tag != 0)
goto _LL1E5;}_LL1E4: return 1;_LL1E5: {struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmp4AC=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4AA;if(_tmp4AC->tag != 1)
goto _LL1E7;}_LL1E6: goto _LL1E8;_LL1E7: {struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp4AD=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4AA;if(_tmp4AD->tag != 2)
goto _LL1E9;}_LL1E8: goto _LL1EA;_LL1E9: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*
_tmp4AE=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4AA;if(_tmp4AE->tag
!= 3)goto _LL1EB;}_LL1EA: goto _LL1EC;_LL1EB: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*
_tmp4AF=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4AA;if(_tmp4AF->tag
!= 4)goto _LL1ED;}_LL1EC: return 0;_LL1ED: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp4B0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AA;if(_tmp4B0->tag
!= 5)goto _LL1EF;else{_tmp4B1=_tmp4B0->f1;_tmp4B2=_tmp4B1.ptr_atts;_tmp4B3=
_tmp4B2.nullable;}}_LL1EE: return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
1,_tmp4B3);_LL1EF: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4B4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp4AA;if(_tmp4B4->tag != 6)goto _LL1F1;}_LL1F0: goto _LL1F2;_LL1F1: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmp4B5=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4AA;if(_tmp4B5->tag != 
7)goto _LL1F3;}_LL1F2: return 1;_LL1F3: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp4B6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4AA;if(_tmp4B6->tag != 
8)goto _LL1F5;else{_tmp4B7=_tmp4B6->f1;_tmp4B8=_tmp4B7.elt_type;}}_LL1F4: return
Cyc_Tcexp_zeroable_type(_tmp4B8);_LL1F5: {struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp4B9=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4AA;if(_tmp4B9->tag != 9)
goto _LL1F7;}_LL1F6: return 0;_LL1F7: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmp4BA=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4AA;if(_tmp4BA->tag != 
10)goto _LL1F9;else{_tmp4BB=_tmp4BA->f1;}}_LL1F8: for(0;(unsigned int)_tmp4BB;
_tmp4BB=_tmp4BB->tl){if(!Cyc_Tcexp_zeroable_type((*((struct _tuple19*)_tmp4BB->hd)).f2))
return 0;}return 1;_LL1F9: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4BC=(
struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4AA;if(_tmp4BC->tag != 11)goto
_LL1FB;else{_tmp4BD=_tmp4BC->f1;_tmp4BE=_tmp4BD.aggr_info;_tmp4BF=_tmp4BD.targs;}}
_LL1FA: {struct Cyc_Absyn_Aggrdecl*_tmp4D0=Cyc_Absyn_get_known_aggrdecl(_tmp4BE);
if(_tmp4D0->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4D0->impl))->exist_vars
!= 0)return 0;{struct _RegionHandle _tmp4D1=_new_region("r");struct _RegionHandle*r=&
_tmp4D1;_push_region(r);{struct Cyc_List_List*_tmp4D2=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,_tmp4D0->tvs,_tmp4BF);{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4D0->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(
Cyc_Tcutil_rsubstitute(r,_tmp4D2,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp4D3=0;_npop_handler(0);return _tmp4D3;}}}{int _tmp4D4=1;_npop_handler(0);
return _tmp4D4;};};_pop_region(r);};}_LL1FB: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp4C0=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4AA;if(_tmp4C0->tag != 
13)goto _LL1FD;}_LL1FC: return 1;_LL1FD: {struct Cyc_Absyn_TagType_Absyn_Type_struct*
_tmp4C1=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4AA;if(_tmp4C1->tag != 19)
goto _LL1FF;}_LL1FE: return 1;_LL1FF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*
_tmp4C2=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4AA;if(_tmp4C2->tag
!= 12)goto _LL201;else{_tmp4C3=_tmp4C2->f2;}}_LL200: for(0;_tmp4C3 != 0;_tmp4C3=
_tmp4C3->tl){if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmp4C3->hd)->type))
return 0;}return 1;_LL201: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp4C4=(
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4AA;if(_tmp4C4->tag != 14)
goto _LL203;}_LL202: return 1;_LL203: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*
_tmp4C5=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4AA;if(_tmp4C5->tag
!= 17)goto _LL205;}_LL204: return 0;_LL205: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*
_tmp4C6=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4AA;if(_tmp4C6->tag != 
16)goto _LL207;}_LL206: return 0;_LL207: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*
_tmp4C7=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4AA;if(_tmp4C7->tag
!= 15)goto _LL209;}_LL208: return 0;_LL209: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*
_tmp4C8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4AA;if(_tmp4C8->tag
!= 26)goto _LL20B;}_LL20A: goto _LL20C;_LL20B: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*
_tmp4C9=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4AA;if(_tmp4C9->tag
!= 18)goto _LL20D;}_LL20C: goto _LL20E;_LL20D: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*
_tmp4CA=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4AA;if(_tmp4CA->tag != 20)
goto _LL20F;}_LL20E: goto _LL210;_LL20F: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*
_tmp4CB=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp4AA;if(_tmp4CB->tag != 
21)goto _LL211;}_LL210: goto _LL212;_LL211: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*
_tmp4CC=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp4AA;if(_tmp4CC->tag != 
22)goto _LL213;}_LL212: goto _LL214;_LL213: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*
_tmp4CD=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4AA;if(_tmp4CD->tag != 
23)goto _LL215;}_LL214: goto _LL216;_LL215: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*
_tmp4CE=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4AA;if(_tmp4CE->tag != 24)
goto _LL217;}_LL216: goto _LL218;_LL217: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*
_tmp4CF=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4AA;if(_tmp4CF->tag != 25)
goto _LL1E2;}_LL218: {const char*_tmp978;void*_tmp977[1];struct Cyc_String_pa_PrintArg_struct
_tmp976;(_tmp976.tag=0,((_tmp976.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp977[0]=& _tmp976,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp978="bad type `%s' in zeroable type",
_tag_dyneither(_tmp978,sizeof(char),31))),_tag_dyneither(_tmp977,sizeof(void*),1)))))));}
_LL1E2:;}static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,
void**topt,void*t){if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(
t))return;if(topt != 0){void*_tmp4D8=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp4DA;void*_tmp4DB;_LL21A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp4D9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D8;if(_tmp4D9->tag
!= 5)goto _LL21C;else{_tmp4DA=_tmp4D9->f1;_tmp4DB=_tmp4DA.elt_typ;}}_LL21B: Cyc_Tcutil_unify(
_tmp4DB,t);if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))
return;goto _LL219;_LL21C:;_LL21D: goto _LL219;_LL219:;}{const char*_tmp981;void*
_tmp980[2];const char*_tmp97F;const char*_tmp97E;struct Cyc_String_pa_PrintArg_struct
_tmp97D;struct Cyc_String_pa_PrintArg_struct _tmp97C;(_tmp97C.tag=0,((_tmp97C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp97D.tag=
0,((_tmp97D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct
_dyneither_ptr)((_tmp97E="calloc",_tag_dyneither(_tmp97E,sizeof(char),7))):(
struct _dyneither_ptr)((_tmp97F="malloc",_tag_dyneither(_tmp97F,sizeof(char),7))))),((
_tmp980[0]=& _tmp97D,((_tmp980[1]=& _tmp97C,Cyc_Tcutil_terr(loc,((_tmp981="%s cannot be used with type %s\n\t(type needs initialization)",
_tag_dyneither(_tmp981,sizeof(char),60))),_tag_dyneither(_tmp980,sizeof(void*),2)))))))))))));};}
static void*Cyc_Tcexp_mallocRgn(void*rgn){switch((Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(
rgn)))->aliasqual){case Cyc_Absyn_Unique: _LL21E: return(void*)& Cyc_Absyn_UniqueRgn_val;
default: _LL21F: return(void*)& Cyc_Absyn_HeapRgn_val;}}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,
void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp984;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*
_tmp983;void*expected_type=(void*)((_tmp983=_cycalloc(sizeof(*_tmp983)),((
_tmp983[0]=((_tmp984.tag=15,((_tmp984.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp984)))),_tmp983))));void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(*ropt));void*_tmp4E2=Cyc_Tcutil_compress(handle_type);void*_tmp4E4;
_LL222: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4E3=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)
_tmp4E2;if(_tmp4E3->tag != 15)goto _LL224;else{_tmp4E4=(void*)_tmp4E3->f1;}}_LL223:
rgn=_tmp4E4;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL221;_LL224:;_LL225:{
const char*_tmp988;void*_tmp987[1];struct Cyc_String_pa_PrintArg_struct _tmp986;(
_tmp986.tag=0,((_tmp986.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
handle_type)),((_tmp987[0]=& _tmp986,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_check_null(*ropt))->loc,((_tmp988="expecting region_t type but found %s",
_tag_dyneither(_tmp988,sizeof(char),37))),_tag_dyneither(_tmp987,sizeof(void*),1)))))));}
goto _LL221;_LL221:;}else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){rgn=Cyc_Tcexp_mallocRgn(optrgn);if(
rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uniquergn_exp;}}}
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{void*elt_type;struct Cyc_Absyn_Exp*
num_elts;int one_elt;if(*is_calloc){if(*t == 0){const char*_tmp98B;void*_tmp98A;(
_tmp98A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp98B="calloc with empty type",_tag_dyneither(_tmp98B,sizeof(char),23))),
_tag_dyneither(_tmp98A,sizeof(void*),0)));}elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,
elt_type);Cyc_Tcutil_check_no_qual(loc,elt_type);Cyc_Tcexp_check_malloc_type(1,
loc,topt,elt_type);num_elts=*e;one_elt=0;}else{void*er=(*e)->r;retry_sizeof: {
void*_tmp4EC=er;void*_tmp4EE;enum Cyc_Absyn_Primop _tmp4F0;struct Cyc_List_List*
_tmp4F1;struct Cyc_List_List _tmp4F2;struct Cyc_Absyn_Exp*_tmp4F3;struct Cyc_List_List*
_tmp4F4;struct Cyc_List_List _tmp4F5;struct Cyc_Absyn_Exp*_tmp4F6;struct Cyc_List_List*
_tmp4F7;_LL227: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4ED=(
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4EC;if(_tmp4ED->tag != 16)
goto _LL229;else{_tmp4EE=(void*)_tmp4ED->f1;}}_LL228: elt_type=_tmp4EE;{void**
_tmp98C;*t=(void**)((_tmp98C=_cycalloc(sizeof(*_tmp98C)),((_tmp98C[0]=elt_type,
_tmp98C))));}num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,
num_elts);one_elt=1;goto _LL226;_LL229: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*
_tmp4EF=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4EC;if(_tmp4EF->tag
!= 2)goto _LL22B;else{_tmp4F0=_tmp4EF->f1;if(_tmp4F0 != Cyc_Absyn_Times)goto _LL22B;
_tmp4F1=_tmp4EF->f2;if(_tmp4F1 == 0)goto _LL22B;_tmp4F2=*_tmp4F1;_tmp4F3=(struct
Cyc_Absyn_Exp*)_tmp4F2.hd;_tmp4F4=_tmp4F2.tl;if(_tmp4F4 == 0)goto _LL22B;_tmp4F5=*
_tmp4F4;_tmp4F6=(struct Cyc_Absyn_Exp*)_tmp4F5.hd;_tmp4F7=_tmp4F5.tl;if(_tmp4F7 != 
0)goto _LL22B;}}_LL22A:{struct _tuple0 _tmp98D;struct _tuple0 _tmp4FA=(_tmp98D.f1=
_tmp4F3->r,((_tmp98D.f2=_tmp4F6->r,_tmp98D)));void*_tmp4FB;void*_tmp4FD;void*
_tmp4FE;void*_tmp500;_LL22E: _tmp4FB=_tmp4FA.f1;{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*
_tmp4FC=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4FB;if(_tmp4FC->tag
!= 16)goto _LL230;else{_tmp4FD=(void*)_tmp4FC->f1;}};_LL22F: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp4FD);elt_type=_tmp4FD;{void**_tmp98E;*t=(void**)((_tmp98E=
_cycalloc(sizeof(*_tmp98E)),((_tmp98E[0]=elt_type,_tmp98E))));}num_elts=_tmp4F6;
one_elt=0;goto _LL22D;_LL230: _tmp4FE=_tmp4FA.f2;{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*
_tmp4FF=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp4FF->tag
!= 16)goto _LL232;else{_tmp500=(void*)_tmp4FF->f1;}};_LL231: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp500);elt_type=_tmp500;{void**_tmp98F;*t=(void**)((_tmp98F=
_cycalloc(sizeof(*_tmp98F)),((_tmp98F[0]=elt_type,_tmp98F))));}num_elts=_tmp4F3;
one_elt=0;goto _LL22D;_LL232:;_LL233: goto No_sizeof;_LL22D:;}goto _LL226;_LL22B:;
_LL22C: No_sizeof: {struct Cyc_Absyn_Exp*_tmp503=*e;{void*_tmp504=_tmp503->r;
struct Cyc_Absyn_Exp*_tmp506;_LL235: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*
_tmp505=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp504;if(_tmp505->tag != 
13)goto _LL237;else{_tmp506=_tmp505->f2;}}_LL236: _tmp503=_tmp506;goto _LL234;
_LL237:;_LL238: goto _LL234;_LL234:;}{void*_tmp507=Cyc_Tcutil_compress((void*)
_check_null(_tmp503->topt));void*_tmp509;_LL23A: {struct Cyc_Absyn_TagType_Absyn_Type_struct*
_tmp508=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp507;if(_tmp508->tag != 19)
goto _LL23C;else{_tmp509=(void*)_tmp508->f1;}}_LL23B:{void*_tmp50A=Cyc_Tcutil_compress(
_tmp509);struct Cyc_Absyn_Exp*_tmp50C;_LL23F: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*
_tmp50B=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp50A;if(_tmp50B->tag
!= 18)goto _LL241;else{_tmp50C=_tmp50B->f1;}}_LL240: er=_tmp50C->r;goto
retry_sizeof;_LL241:;_LL242: goto _LL23E;_LL23E:;}goto _LL239;_LL23C:;_LL23D: goto
_LL239;_LL239:;}elt_type=Cyc_Absyn_char_typ;{void**_tmp990;*t=(void**)((_tmp990=
_cycalloc(sizeof(*_tmp990)),((_tmp990[0]=elt_type,_tmp990))));}num_elts=*e;
one_elt=0;}goto _LL226;_LL226:;}}*is_fat=!one_elt;{void*_tmp50E=elt_type;struct
Cyc_Absyn_AggrInfo _tmp510;union Cyc_Absyn_AggrInfoU _tmp511;struct Cyc_Absyn_Aggrdecl**
_tmp512;struct Cyc_Absyn_Aggrdecl*_tmp513;_LL244: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp50F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp50E;if(_tmp50F->tag != 
11)goto _LL246;else{_tmp510=_tmp50F->f1;_tmp511=_tmp510.aggr_info;if((_tmp511.KnownAggr).tag
!= 2)goto _LL246;_tmp512=(struct Cyc_Absyn_Aggrdecl**)(_tmp511.KnownAggr).val;
_tmp513=*_tmp512;}}_LL245: if(_tmp513->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp513->impl))->exist_vars != 0){const char*_tmp993;void*_tmp992;(
_tmp992=0,Cyc_Tcutil_terr(loc,((_tmp993="malloc with existential types not yet implemented",
_tag_dyneither(_tmp993,sizeof(char),50))),_tag_dyneither(_tmp992,sizeof(void*),0)));}
goto _LL243;_LL246:;_LL247: goto _LL243;_LL243:;}{void*(*_tmp516)(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp517=Cyc_Absyn_false_conref;if(topt != 0){void*
_tmp518=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp51A;struct Cyc_Absyn_PtrAtts
_tmp51B;union Cyc_Absyn_Constraint*_tmp51C;union Cyc_Absyn_Constraint*_tmp51D;
union Cyc_Absyn_Constraint*_tmp51E;_LL249: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp519=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp518;if(_tmp519->tag
!= 5)goto _LL24B;else{_tmp51A=_tmp519->f1;_tmp51B=_tmp51A.ptr_atts;_tmp51C=
_tmp51B.nullable;_tmp51D=_tmp51B.bounds;_tmp51E=_tmp51B.zero_term;}}_LL24A:
_tmp517=_tmp51E;if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp51C))_tmp516=Cyc_Absyn_star_typ;if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp51E) && !(*is_calloc)){{const char*_tmp996;void*
_tmp995;(_tmp995=0,Cyc_Tcutil_warn(loc,((_tmp996="converting malloc to calloc to ensure zero-termination",
_tag_dyneither(_tmp996,sizeof(char),55))),_tag_dyneither(_tmp995,sizeof(void*),0)));}*
is_calloc=1;}{void*_tmp521=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp51D);struct Cyc_Absyn_Exp*_tmp524;_LL24E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*
_tmp522=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp521;if(_tmp522->tag
!= 0)goto _LL250;}_LL24F: goto _LL24D;_LL250:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp523=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp521;if(_tmp523->tag != 
1)goto _LL252;else{_tmp524=_tmp523->f1;}}if(!(!one_elt))goto _LL252;_LL251: {int
_tmp525=Cyc_Evexp_c_can_eval(num_elts);if(_tmp525  && Cyc_Evexp_same_const_exp(
_tmp524,num_elts)){*is_fat=0;return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp51D),
_tmp517);}{void*_tmp526=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));
void*_tmp528;_LL255: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp527=(struct
Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp526;if(_tmp527->tag != 19)goto _LL257;
else{_tmp528=(void*)_tmp527->f1;}}_LL256: {struct Cyc_Absyn_Exp*_tmp529=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp528,0),0,Cyc_Absyn_No_coercion,0);
if(Cyc_Evexp_same_const_exp(_tmp529,_tmp524)){*is_fat=0;return Cyc_Absyn_atb_typ(
elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_val)(_tmp51D),_tmp517);}goto _LL254;}_LL257:;_LL258: goto _LL254;
_LL254:;}goto _LL24D;}_LL252:;_LL253: goto _LL24D;_LL24D:;}goto _LL248;_LL24B:;
_LL24C: goto _LL248;_LL248:;}if(!one_elt)_tmp516=Cyc_Absyn_dyneither_typ;return
_tmp516(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp517);};};}static void*Cyc_Tcexp_tcSwap(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){struct _RegionHandle _tmp52A=_new_region("r");struct
_RegionHandle*r=& _tmp52A;_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp52B=Cyc_Tcenv_enter_lhs(
r,Cyc_Tcenv_enter_notreadctxt(r,te));Cyc_Tcexp_tcExp(_tmp52B,0,e1);{void*_tmp52C=(
void*)_check_null(e1->topt);Cyc_Tcexp_tcExp(_tmp52B,(void**)& _tmp52C,e2);};}{
void*t1=(void*)_check_null(e1->topt);void*t2=(void*)_check_null(e2->topt);{void*
_tmp52D=Cyc_Tcutil_compress(t1);_LL25A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp52E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp52D;if(_tmp52E->tag != 
8)goto _LL25C;}_LL25B:{const char*_tmp999;void*_tmp998;(_tmp998=0,Cyc_Tcutil_terr(
loc,((_tmp999="cannot assign to an array",_tag_dyneither(_tmp999,sizeof(char),26))),
_tag_dyneither(_tmp998,sizeof(void*),0)));}goto _LL259;_LL25C:;_LL25D: goto _LL259;
_LL259:;}{int ign_1=0;if(!Cyc_Tcutil_is_pointer_or_boxed(t1,& ign_1)){const char*
_tmp99C;void*_tmp99B;(_tmp99B=0,Cyc_Tcutil_terr(loc,((_tmp99C="Swap not allowed for non-pointer or non-word-sized types.",
_tag_dyneither(_tmp99C,sizeof(char),58))),_tag_dyneither(_tmp99B,sizeof(void*),0)));}
if(!Cyc_Absyn_is_lvalue(e1)){const char*_tmp99F;void*_tmp99E;void*_tmp535=(
_tmp99E=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp99F="swap non-lvalue",
_tag_dyneither(_tmp99F,sizeof(char),16))),_tag_dyneither(_tmp99E,sizeof(void*),0)));
_npop_handler(0);return _tmp535;}if(!Cyc_Absyn_is_lvalue(e2)){const char*_tmp9A2;
void*_tmp9A1;void*_tmp538=(_tmp9A1=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((
_tmp9A2="swap non-lvalue",_tag_dyneither(_tmp9A2,sizeof(char),16))),
_tag_dyneither(_tmp9A1,sizeof(void*),0)));_npop_handler(0);return _tmp538;}{void*
t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){const
char*_tmp9A5;void*_tmp9A4;void*_tmp53B=(_tmp9A4=0,Cyc_Tcexp_expr_err(te,e1->loc,
topt,((_tmp9A5="swap value in zeroterm array",_tag_dyneither(_tmp9A5,sizeof(char),
29))),_tag_dyneither(_tmp9A4,sizeof(void*),0)));_npop_handler(0);return _tmp53B;}
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp9A8;
void*_tmp9A7;void*_tmp53E=(_tmp9A7=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((
_tmp9A8="swap value in zeroterm array",_tag_dyneither(_tmp9A8,sizeof(char),29))),
_tag_dyneither(_tmp9A7,sizeof(void*),0)));_npop_handler(0);return _tmp53E;}Cyc_Tcexp_check_writable(
te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(t1,t2)){const char*
_tmp9AD;void*_tmp9AC[2];struct Cyc_String_pa_PrintArg_struct _tmp9AB;struct Cyc_String_pa_PrintArg_struct
_tmp9AA;void*_tmp53F=(_tmp9AA.tag=0,((_tmp9AA.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp9AB.tag=0,((_tmp9AB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp9AC[0]=&
_tmp9AB,((_tmp9AC[1]=& _tmp9AA,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp9AD="type mismatch: %s != %s",
_tag_dyneither(_tmp9AD,sizeof(char),24))),_tag_dyneither(_tmp9AC,sizeof(void*),2)))))))))))));
void*_tmp540=_tmp53F;_npop_handler(0);return _tmp540;}{void*_tmp545=(void*)& Cyc_Absyn_VoidType_val;
_npop_handler(0);return _tmp545;};};};};;_pop_region(r);}int Cyc_Tcexp_in_stmt_exp=
0;static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**
topt,struct Cyc_Absyn_Stmt*s){struct _RegionHandle _tmp546=_new_region("r");struct
_RegionHandle*r=& _tmp546;_push_region(r);{int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,te,s),s,1);Cyc_Tcexp_in_stmt_exp=
old_stmt_exp_state;}Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
while(1){void*_tmp547=s->r;struct Cyc_Absyn_Exp*_tmp549;struct Cyc_Absyn_Stmt*
_tmp54B;struct Cyc_Absyn_Stmt*_tmp54C;struct Cyc_Absyn_Decl*_tmp54E;struct Cyc_Absyn_Stmt*
_tmp54F;_LL25F: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp548=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)
_tmp547;if(_tmp548->tag != 1)goto _LL261;else{_tmp549=_tmp548->f1;}}_LL260: {void*
_tmp550=(void*)_check_null(_tmp549->topt);if(!Cyc_Tcutil_unify(_tmp550,Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te)))){{const char*_tmp9B1;void*_tmp9B0[1];struct
Cyc_String_pa_PrintArg_struct _tmp9AF;(_tmp9AF.tag=0,((_tmp9AF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp550)),((_tmp9B0[
0]=& _tmp9AF,Cyc_Tcutil_terr(loc,((_tmp9B1="statement expression returns type %s",
_tag_dyneither(_tmp9B1,sizeof(char),37))),_tag_dyneither(_tmp9B0,sizeof(void*),1)))))));}
Cyc_Tcutil_explain_failure();}{void*_tmp554=_tmp550;_npop_handler(0);return
_tmp554;};}_LL261: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp54A=(struct
Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp547;if(_tmp54A->tag != 2)goto _LL263;
else{_tmp54B=_tmp54A->f1;_tmp54C=_tmp54A->f2;}}_LL262: s=_tmp54C;continue;_LL263: {
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp54D=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)
_tmp547;if(_tmp54D->tag != 12)goto _LL265;else{_tmp54E=_tmp54D->f1;_tmp54F=_tmp54D->f2;}}
_LL264: s=_tmp54F;continue;_LL265:;_LL266: {const char*_tmp9B4;void*_tmp9B3;void*
_tmp557=(_tmp9B3=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp9B4="statement expression must end with expression",
_tag_dyneither(_tmp9B4,sizeof(char),46))),_tag_dyneither(_tmp9B3,sizeof(void*),0)));
_npop_handler(0);return _tmp557;}_LL25E:;};_pop_region(r);}static void*Cyc_Tcexp_tcTagcheck(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*f){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(te,0,e));{void*
_tmp558=t;struct Cyc_Absyn_AggrInfo _tmp55A;union Cyc_Absyn_AggrInfoU _tmp55B;struct
Cyc_Absyn_Aggrdecl**_tmp55C;struct Cyc_Absyn_Aggrdecl*_tmp55D;_LL268: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp559=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp558;if(_tmp559->tag != 
11)goto _LL26A;else{_tmp55A=_tmp559->f1;_tmp55B=_tmp55A.aggr_info;if((_tmp55B.KnownAggr).tag
!= 2)goto _LL26A;_tmp55C=(struct Cyc_Absyn_Aggrdecl**)(_tmp55B.KnownAggr).val;
_tmp55D=*_tmp55C;}}_LL269: if((_tmp55D->kind == Cyc_Absyn_UnionA  && _tmp55D->impl
!= 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp55D->impl))->tagged)
goto _LL267;goto _LL26B;_LL26A:;_LL26B:{const char*_tmp9B8;void*_tmp9B7[1];struct
Cyc_String_pa_PrintArg_struct _tmp9B6;(_tmp9B6.tag=0,((_tmp9B6.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp9B7[0]=&
_tmp9B6,Cyc_Tcutil_terr(loc,((_tmp9B8="expecting @tagged union but found %s",
_tag_dyneither(_tmp9B8,sizeof(char),37))),_tag_dyneither(_tmp9B7,sizeof(void*),1)))))));}
goto _LL267;_LL267:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcNew(struct
Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*rgn_handle != 0){struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp9BB;
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp9BA;void*expected_type=(void*)((
_tmp9BA=_cycalloc(sizeof(*_tmp9BA)),((_tmp9BA[0]=((_tmp9BB.tag=15,((_tmp9BB.f1=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(
te)),_tmp9BB)))),_tmp9BA))));void*handle_type=Cyc_Tcexp_tcExp(te,(void**)&
expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));void*_tmp561=Cyc_Tcutil_compress(
handle_type);void*_tmp563;_LL26D: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*
_tmp562=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp561;if(_tmp562->tag
!= 15)goto _LL26F;else{_tmp563=(void*)_tmp562->f1;}}_LL26E: rgn=_tmp563;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL26C;_LL26F:;_LL270:{const char*_tmp9BF;void*_tmp9BE[1];struct
Cyc_String_pa_PrintArg_struct _tmp9BD;(_tmp9BD.tag=0,((_tmp9BD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type)),((
_tmp9BE[0]=& _tmp9BD,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*
rgn_handle))->loc,((_tmp9BF="expecting region_t type but found %s",
_tag_dyneither(_tmp9BF,sizeof(char),37))),_tag_dyneither(_tmp9BE,sizeof(void*),1)))))));}
goto _LL26C;_LL26C:;}else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){rgn=Cyc_Tcexp_mallocRgn(optrgn);if(
rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uniquergn_exp;}}}{
void*_tmp569=e1->r;struct Cyc_Core_Opt*_tmp56C;struct Cyc_List_List*_tmp56D;struct
Cyc_List_List*_tmp56F;union Cyc_Absyn_Cnst _tmp571;struct _dyneither_ptr _tmp572;
union Cyc_Absyn_Cnst _tmp574;struct _dyneither_ptr _tmp575;_LL272: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*
_tmp56A=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp569;if(
_tmp56A->tag != 26)goto _LL274;}_LL273: {void*_tmp576=Cyc_Tcexp_tcExpNoPromote(te,
topt,e1);void*_tmp577=Cyc_Tcutil_compress(_tmp576);struct Cyc_Absyn_ArrayInfo
_tmp579;void*_tmp57A;struct Cyc_Absyn_Tqual _tmp57B;struct Cyc_Absyn_Exp*_tmp57C;
union Cyc_Absyn_Constraint*_tmp57D;_LL27F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp578=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp577;if(_tmp578->tag != 
8)goto _LL281;else{_tmp579=_tmp578->f1;_tmp57A=_tmp579.elt_type;_tmp57B=_tmp579.tq;
_tmp57C=_tmp579.num_elts;_tmp57D=_tmp579.zero_term;}}_LL280: {struct Cyc_Absyn_Exp*
bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp57C);void*b;{void*_tmp57E=Cyc_Tcutil_compress((
void*)_check_null(bnd->topt));void*_tmp580;_LL284: {struct Cyc_Absyn_TagType_Absyn_Type_struct*
_tmp57F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp57E;if(_tmp57F->tag != 19)
goto _LL286;else{_tmp580=(void*)_tmp57F->f1;}}_LL285:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct
_tmp9C2;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9C1;b=(void*)((_tmp9C1=
_cycalloc(sizeof(*_tmp9C1)),((_tmp9C1[0]=((_tmp9C2.tag=1,((_tmp9C2.f1=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp580,0),0,Cyc_Absyn_No_coercion,0),
_tmp9C2)))),_tmp9C1))));}goto _LL283;_LL286:;_LL287: if(Cyc_Tcutil_is_const_exp(te,
bnd)){struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9C5;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp9C4;b=(void*)((_tmp9C4=_cycalloc(sizeof(*_tmp9C4)),((_tmp9C4[0]=((_tmp9C5.tag=
1,((_tmp9C5.f1=bnd,_tmp9C5)))),_tmp9C4))));}else{b=(void*)& Cyc_Absyn_DynEither_b_val;}
_LL283:;}{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9CF;struct Cyc_Absyn_PtrAtts
_tmp9CE;struct Cyc_Absyn_PtrInfo _tmp9CD;struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp9CC;void*res_typ=(void*)((_tmp9CC=_cycalloc(sizeof(*_tmp9CC)),((_tmp9CC[0]=((
_tmp9CF.tag=5,((_tmp9CF.f1=((_tmp9CD.elt_typ=_tmp57A,((_tmp9CD.elt_tq=_tmp57B,((
_tmp9CD.ptr_atts=((_tmp9CE.rgn=rgn,((_tmp9CE.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp9CE.bounds=((union Cyc_Absyn_Constraint*(*)(void*
x))Cyc_Absyn_new_conref)(b),((_tmp9CE.zero_term=_tmp57D,((_tmp9CE.ptrloc=0,
_tmp9CE)))))))))),_tmp9CD)))))),_tmp9CF)))),_tmp9CC))));if(topt != 0){if(!Cyc_Tcutil_unify(*
topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){e->topt=(void*)
res_typ;Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);res_typ=*
topt;}}return res_typ;};}_LL281:;_LL282: {const char*_tmp9D2;void*_tmp9D1;(_tmp9D1=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp9D2="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp9D2,
sizeof(char),45))),_tag_dyneither(_tmp9D1,sizeof(void*),0)));}_LL27E:;}_LL274: {
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp56B=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)
_tmp569;if(_tmp56B->tag != 34)goto _LL276;else{_tmp56C=_tmp56B->f1;_tmp56D=_tmp56B->f2;}}
_LL275:{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp9D5;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*
_tmp9D4;e1->r=(void*)((_tmp9D4=_cycalloc(sizeof(*_tmp9D4)),((_tmp9D4[0]=((
_tmp9D5.tag=25,((_tmp9D5.f1=_tmp56D,_tmp9D5)))),_tmp9D4))));}_tmp56F=_tmp56D;
goto _LL277;_LL276: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp56E=(struct
Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp569;if(_tmp56E->tag != 25)goto _LL278;
else{_tmp56F=_tmp56E->f1;}}_LL277: {void**elt_typ_opt=0;int zero_term=0;if(topt != 
0){void*_tmp58D=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp58F;void*
_tmp590;void**_tmp591;struct Cyc_Absyn_Tqual _tmp592;struct Cyc_Absyn_PtrAtts
_tmp593;union Cyc_Absyn_Constraint*_tmp594;_LL289: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp58E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp58D;if(_tmp58E->tag
!= 5)goto _LL28B;else{_tmp58F=_tmp58E->f1;_tmp590=_tmp58F.elt_typ;_tmp591=(void**)&(
_tmp58E->f1).elt_typ;_tmp592=_tmp58F.elt_tq;_tmp593=_tmp58F.ptr_atts;_tmp594=
_tmp593.zero_term;}}_LL28A: elt_typ_opt=(void**)_tmp591;zero_term=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp594);goto _LL288;_LL28B:;
_LL28C: goto _LL288;_LL288:;}{void*_tmp595=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,0,zero_term,_tmp56F);e1->topt=(void*)_tmp595;{void*res_typ;{void*
_tmp596=Cyc_Tcutil_compress(_tmp595);struct Cyc_Absyn_ArrayInfo _tmp598;void*
_tmp599;struct Cyc_Absyn_Tqual _tmp59A;struct Cyc_Absyn_Exp*_tmp59B;union Cyc_Absyn_Constraint*
_tmp59C;_LL28E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp597=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)
_tmp596;if(_tmp597->tag != 8)goto _LL290;else{_tmp598=_tmp597->f1;_tmp599=_tmp598.elt_type;
_tmp59A=_tmp598.tq;_tmp59B=_tmp598.num_elts;_tmp59C=_tmp598.zero_term;}}_LL28F:{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9EA;struct Cyc_Absyn_PtrAtts
_tmp9E9;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9E8;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp9E7;struct Cyc_Absyn_PtrInfo _tmp9E6;struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp9E5;res_typ=(void*)((_tmp9E5=_cycalloc(sizeof(*_tmp9E5)),((_tmp9E5[0]=((
_tmp9EA.tag=5,((_tmp9EA.f1=((_tmp9E6.elt_typ=_tmp599,((_tmp9E6.elt_tq=_tmp59A,((
_tmp9E6.ptr_atts=((_tmp9E9.rgn=rgn,((_tmp9E9.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp9E9.bounds=((union Cyc_Absyn_Constraint*(*)(void*
x))Cyc_Absyn_new_conref)((void*)((_tmp9E7=_cycalloc(sizeof(*_tmp9E7)),((_tmp9E7[
0]=((_tmp9E8.tag=1,((_tmp9E8.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp59B),
_tmp9E8)))),_tmp9E7))))),((_tmp9E9.zero_term=_tmp59C,((_tmp9E9.ptrloc=0,_tmp9E9)))))))))),
_tmp9E6)))))),_tmp9EA)))),_tmp9E5))));}if(topt != 0){if(!Cyc_Tcutil_unify(*topt,
res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){e->topt=(void*)
res_typ;Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);res_typ=*
topt;}}goto _LL28D;_LL290:;_LL291: {const char*_tmp9ED;void*_tmp9EC;(_tmp9EC=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp9ED="tcExpNoPromote on Array_e returned non-array type",_tag_dyneither(
_tmp9ED,sizeof(char),50))),_tag_dyneither(_tmp9EC,sizeof(void*),0)));}_LL28D:;}
return res_typ;};};}_LL278: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp570=(
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp569;if(_tmp570->tag != 0)goto
_LL27A;else{_tmp571=_tmp570->f1;if((_tmp571.String_c).tag != 8)goto _LL27A;_tmp572=(
struct _dyneither_ptr)(_tmp571.String_c).val;}}_LL279: {void*_tmp5A5=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,
Cyc_Absyn_true_conref);void*_tmp5A6=Cyc_Tcexp_tcExp(te,(void**)& _tmp5A5,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9F0;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp9EF;return Cyc_Absyn_atb_typ(_tmp5A6,rgn,Cyc_Absyn_empty_tqual(0),(void*)((
_tmp9EF=_cycalloc(sizeof(*_tmp9EF)),((_tmp9EF[0]=((_tmp9F0.tag=1,((_tmp9F0.f1=
Cyc_Absyn_uint_exp(1,0),_tmp9F0)))),_tmp9EF)))),Cyc_Absyn_false_conref);}_LL27A: {
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp573=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmp569;if(_tmp573->tag != 0)goto _LL27C;else{_tmp574=_tmp573->f1;if((_tmp574.Wstring_c).tag
!= 9)goto _LL27C;_tmp575=(struct _dyneither_ptr)(_tmp574.Wstring_c).val;}}_LL27B: {
void*_tmp5A9=Cyc_Absyn_atb_typ(Cyc_Absyn_wchar_typ(),rgn,Cyc_Absyn_const_tqual(0),(
void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);void*_tmp5AA=Cyc_Tcexp_tcExp(
te,(void**)& _tmp5A9,e1);struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9F3;
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9F2;return Cyc_Absyn_atb_typ(
_tmp5AA,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp9F2=_cycalloc(sizeof(*_tmp9F2)),((
_tmp9F2[0]=((_tmp9F3.tag=1,((_tmp9F3.f1=Cyc_Absyn_uint_exp(1,0),_tmp9F3)))),
_tmp9F2)))),Cyc_Absyn_false_conref);}_LL27C:;_LL27D: RG: {void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;if(topt != 0){void*_tmp5AD=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp5AF;void*_tmp5B0;void**_tmp5B1;struct Cyc_Absyn_Tqual _tmp5B2;_LL293: {struct
Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5AE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmp5AD;if(_tmp5AE->tag != 5)goto _LL295;else{_tmp5AF=_tmp5AE->f1;_tmp5B0=_tmp5AF.elt_typ;
_tmp5B1=(void**)&(_tmp5AE->f1).elt_typ;_tmp5B2=_tmp5AF.elt_tq;}}_LL294: topt2=(
void**)_tmp5B1;goto _LL292;_LL295: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*
_tmp5B3=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5AD;if(_tmp5B3->tag
!= 3)goto _LL297;}_LL296: bogus=*topt;topt2=(void**)& bogus;goto _LL292;_LL297:;
_LL298: goto _LL292;_LL292:;}{void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);struct Cyc_Absyn_PointerType_Absyn_Type_struct
_tmp9FD;struct Cyc_Absyn_PtrAtts _tmp9FC;struct Cyc_Absyn_PtrInfo _tmp9FB;struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp9FA;void*res_typ=(void*)((_tmp9FA=_cycalloc(sizeof(*_tmp9FA)),((_tmp9FA[0]=((
_tmp9FD.tag=5,((_tmp9FD.f1=((_tmp9FB.elt_typ=telt,((_tmp9FB.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp9FB.ptr_atts=((_tmp9FC.rgn=rgn,((_tmp9FC.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp9FC.bounds=Cyc_Absyn_bounds_one_conref,((_tmp9FC.zero_term=
Cyc_Absyn_false_conref,((_tmp9FC.ptrloc=0,_tmp9FC)))))))))),_tmp9FB)))))),
_tmp9FD)))),_tmp9FA))));if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){e->topt=(void*)res_typ;Cyc_Tcutil_unchecked_cast(te,e,*
topt,Cyc_Absyn_Other_coercion);res_typ=*topt;}}return res_typ;};}_LL271:;};}void*
Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=
Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));void*_tmp5B8=t;struct
Cyc_Absyn_ArrayInfo _tmp5BA;void*_tmp5BB;struct Cyc_Absyn_Tqual _tmp5BC;struct Cyc_Absyn_Exp*
_tmp5BD;union Cyc_Absyn_Constraint*_tmp5BE;_LL29A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp5B9=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B8;if(_tmp5B9->tag != 
8)goto _LL29C;else{_tmp5BA=_tmp5B9->f1;_tmp5BB=_tmp5BA.elt_type;_tmp5BC=_tmp5BA.tq;
_tmp5BD=_tmp5BA.num_elts;_tmp5BE=_tmp5BA.zero_term;}}_LL29B: {void*_tmp5C0;
struct _tuple14 _tmp5BF=Cyc_Tcutil_addressof_props(te,e);_tmp5C0=_tmp5BF.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpA00;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmp9FF;void*_tmp5C1=_tmp5BD == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((
_tmp9FF=_cycalloc(sizeof(*_tmp9FF)),((_tmp9FF[0]=((_tmpA00.tag=1,((_tmpA00.f1=(
struct Cyc_Absyn_Exp*)_tmp5BD,_tmpA00)))),_tmp9FF))));t=Cyc_Absyn_atb_typ(_tmp5BB,
_tmp5C0,_tmp5BC,_tmp5C1,_tmp5BE);e->topt=(void*)t;return t;};}_LL29C:;_LL29D:
return t;_LL299:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**
topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);struct
_RegionHandle _tmp5C4=_new_region("temp");struct _RegionHandle*temp=& _tmp5C4;
_push_region(temp);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(
temp,e)){const char*_tmpA03;void*_tmpA02;(_tmpA02=0,Cyc_Tcutil_terr(e->loc,((
_tmpA03="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmpA03,sizeof(char),49))),_tag_dyneither(_tmpA02,sizeof(void*),0)));}{void*
_tmp5C7=e->r;union Cyc_Absyn_Cnst _tmp5CB;struct _dyneither_ptr _tmp5CC;union Cyc_Absyn_Cnst
_tmp5CE;struct _dyneither_ptr _tmp5CF;_LL29F: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*
_tmp5C8=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp5C8->tag != 
25)goto _LL2A1;}_LL2A0: goto _LL2A2;_LL2A1: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*
_tmp5C9=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5C7;if(
_tmp5C9->tag != 26)goto _LL2A3;}_LL2A2: goto _LL2A4;_LL2A3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp5CA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp5CA->tag != 
0)goto _LL2A5;else{_tmp5CB=_tmp5CA->f1;if((_tmp5CB.Wstring_c).tag != 9)goto _LL2A5;
_tmp5CC=(struct _dyneither_ptr)(_tmp5CB.Wstring_c).val;}}_LL2A4: goto _LL2A6;_LL2A5: {
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5CD=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmp5C7;if(_tmp5CD->tag != 0)goto _LL2A7;else{_tmp5CE=_tmp5CD->f1;if((_tmp5CE.String_c).tag
!= 8)goto _LL2A7;_tmp5CF=(struct _dyneither_ptr)(_tmp5CE.String_c).val;}}_LL2A6: {
void*_tmp5D0=t;_npop_handler(0);return _tmp5D0;}_LL2A7:;_LL2A8: t=Cyc_Tcutil_compress(
t);{void*_tmp5D1=t;struct Cyc_Absyn_ArrayInfo _tmp5D3;void*_tmp5D4;struct Cyc_Absyn_Tqual
_tmp5D5;struct Cyc_Absyn_Exp*_tmp5D6;union Cyc_Absyn_Constraint*_tmp5D7;_LL2AA: {
struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5D2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)
_tmp5D1;if(_tmp5D2->tag != 8)goto _LL2AC;else{_tmp5D3=_tmp5D2->f1;_tmp5D4=_tmp5D3.elt_type;
_tmp5D5=_tmp5D3.tq;_tmp5D6=_tmp5D3.num_elts;_tmp5D7=_tmp5D3.zero_term;}}_LL2AB: {
void*_tmp5D9;struct _tuple14 _tmp5D8=Cyc_Tcutil_addressof_props(te,e);_tmp5D9=
_tmp5D8.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpA06;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*
_tmpA05;void*b=_tmp5D6 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmpA05=
_cycalloc(sizeof(*_tmpA05)),((_tmpA05[0]=((_tmpA06.tag=1,((_tmpA06.f1=(struct Cyc_Absyn_Exp*)
_tmp5D6,_tmpA06)))),_tmpA05))));t=Cyc_Absyn_atb_typ(_tmp5D4,_tmp5D9,_tmp5D5,b,
_tmp5D7);Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);{void*
_tmp5DA=t;_npop_handler(0);return _tmp5DA;};};}_LL2AC:;_LL2AD: {void*_tmp5DD=t;
_npop_handler(0);return _tmp5DD;}_LL2A9:;};_LL29E:;};;_pop_region(temp);}static
void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e){{void*_tmp5DE=e->r;struct Cyc_Absyn_Exp*_tmp5E0;_LL2AF: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*
_tmp5DF=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5DE;if(
_tmp5DF->tag != 11)goto _LL2B1;else{_tmp5E0=_tmp5DF->f1;}}_LL2B0: Cyc_Tcexp_tcExpNoInst(
te,topt,_tmp5E0);_tmp5E0->topt=(void*)Cyc_Absyn_pointer_expand((void*)
_check_null(_tmp5E0->topt),0);e->topt=_tmp5E0->topt;goto _LL2AE;_LL2B1:;_LL2B2:
Cyc_Tcexp_tcExpNoInst(te,topt,e);e->topt=(void*)Cyc_Absyn_pointer_expand((void*)
_check_null(e->topt),0);{void*_tmp5E1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
struct Cyc_Absyn_PtrInfo _tmp5E3;void*_tmp5E4;struct Cyc_Absyn_Tqual _tmp5E5;struct
Cyc_Absyn_PtrAtts _tmp5E6;void*_tmp5E7;union Cyc_Absyn_Constraint*_tmp5E8;union Cyc_Absyn_Constraint*
_tmp5E9;union Cyc_Absyn_Constraint*_tmp5EA;_LL2B4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp5E2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E1;if(_tmp5E2->tag
!= 5)goto _LL2B6;else{_tmp5E3=_tmp5E2->f1;_tmp5E4=_tmp5E3.elt_typ;_tmp5E5=_tmp5E3.elt_tq;
_tmp5E6=_tmp5E3.ptr_atts;_tmp5E7=_tmp5E6.rgn;_tmp5E8=_tmp5E6.nullable;_tmp5E9=
_tmp5E6.bounds;_tmp5EA=_tmp5E6.zero_term;}}_LL2B5:{void*_tmp5EB=Cyc_Tcutil_compress(
_tmp5E4);struct Cyc_Absyn_FnInfo _tmp5ED;struct Cyc_List_List*_tmp5EE;void*_tmp5EF;
struct Cyc_Absyn_Tqual _tmp5F0;void*_tmp5F1;struct Cyc_List_List*_tmp5F2;int _tmp5F3;
struct Cyc_Absyn_VarargInfo*_tmp5F4;struct Cyc_List_List*_tmp5F5;struct Cyc_List_List*
_tmp5F6;_LL2B9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5EC=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)
_tmp5EB;if(_tmp5EC->tag != 9)goto _LL2BB;else{_tmp5ED=_tmp5EC->f1;_tmp5EE=_tmp5ED.tvars;
_tmp5EF=_tmp5ED.effect;_tmp5F0=_tmp5ED.ret_tqual;_tmp5F1=_tmp5ED.ret_typ;_tmp5F2=
_tmp5ED.args;_tmp5F3=_tmp5ED.c_varargs;_tmp5F4=_tmp5ED.cyc_varargs;_tmp5F5=
_tmp5ED.rgn_po;_tmp5F6=_tmp5ED.attributes;}}_LL2BA: if(_tmp5EE != 0){struct
_RegionHandle _tmp5F7=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5F7;
_push_region(rgn);{struct _tuple12 _tmpA07;struct _tuple12 _tmp5F8=(_tmpA07.f1=Cyc_Tcenv_lookup_type_vars(
te),((_tmpA07.f2=rgn,_tmpA07)));struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),
struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp5F8,_tmp5EE);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))
Cyc_Core_snd,inst);struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpA0D;struct Cyc_Absyn_FnInfo
_tmpA0C;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA0B;void*ftyp=Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)((_tmpA0B=_cycalloc(sizeof(*_tmpA0B)),((_tmpA0B[0]=((_tmpA0D.tag=
9,((_tmpA0D.f1=((_tmpA0C.tvars=0,((_tmpA0C.effect=_tmp5EF,((_tmpA0C.ret_tqual=
_tmp5F0,((_tmpA0C.ret_typ=_tmp5F1,((_tmpA0C.args=_tmp5F2,((_tmpA0C.c_varargs=
_tmp5F3,((_tmpA0C.cyc_varargs=_tmp5F4,((_tmpA0C.rgn_po=_tmp5F5,((_tmpA0C.attributes=
_tmp5F6,_tmpA0C)))))))))))))))))),_tmpA0D)))),_tmpA0B)))));struct Cyc_Absyn_PointerType_Absyn_Type_struct
_tmpA17;struct Cyc_Absyn_PtrAtts _tmpA16;struct Cyc_Absyn_PtrInfo _tmpA15;struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmpA14;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F9=(_tmpA14=_cycalloc(
sizeof(*_tmpA14)),((_tmpA14[0]=((_tmpA17.tag=5,((_tmpA17.f1=((_tmpA15.elt_typ=
ftyp,((_tmpA15.elt_tq=_tmp5E5,((_tmpA15.ptr_atts=((_tmpA16.rgn=_tmp5E7,((_tmpA16.nullable=
_tmp5E8,((_tmpA16.bounds=_tmp5E9,((_tmpA16.zero_term=_tmp5EA,((_tmpA16.ptrloc=0,
_tmpA16)))))))))),_tmpA15)))))),_tmpA17)))),_tmpA14)));struct Cyc_Absyn_Exp*
_tmp5FA=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct
_tmpA1A;struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpA19;e->r=(void*)((
_tmpA19=_cycalloc(sizeof(*_tmpA19)),((_tmpA19[0]=((_tmpA1A.tag=12,((_tmpA1A.f1=
_tmp5FA,((_tmpA1A.f2=ts,_tmpA1A)))))),_tmpA19))));}e->topt=(void*)_tmp5F9;};
_pop_region(rgn);}goto _LL2B8;_LL2BB:;_LL2BC: goto _LL2B8;_LL2B8:;}goto _LL2B3;
_LL2B6:;_LL2B7: goto _LL2B3;_LL2B3:;}goto _LL2AE;_LL2AE:;}return(void*)_check_null(
e->topt);}static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
struct _RegionHandle _tmp605=_new_region("r");struct _RegionHandle*r=& _tmp605;
_push_region(r);{struct Cyc_List_List*_tmp606=0;{void*_tmp607=fn_exp->r;struct Cyc_List_List*
_tmp609;_LL2BE: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp608=(struct
Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp607;if(_tmp608->tag != 9)goto _LL2C0;
else{_tmp609=_tmp608->f2;}}_LL2BF:{void*_tmp60A=e->r;struct Cyc_List_List*_tmp60C;
_LL2C3: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp60B=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)
_tmp60A;if(_tmp60B->tag != 9)goto _LL2C5;else{_tmp60C=_tmp60B->f2;}}_LL2C4: {
struct Cyc_List_List*_tmp60D=alias_arg_exps;int _tmp60E=0;while(_tmp60D != 0){
while(_tmp60E != (int)_tmp60D->hd){if(_tmp609 == 0){struct Cyc_Int_pa_PrintArg_struct
_tmpA24;struct Cyc_Int_pa_PrintArg_struct _tmpA23;void*_tmpA22[2];const char*
_tmpA21;void*_tmpA20;(_tmpA20=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpA24.tag=1,((
_tmpA24.f1=(unsigned long)((int)_tmp60D->hd),((_tmpA23.tag=1,((_tmpA23.f1=(
unsigned long)_tmp60E,((_tmpA22[0]=& _tmpA23,((_tmpA22[1]=& _tmpA24,Cyc_aprintf(((
_tmpA21="bad count %d/%d for alias coercion!",_tag_dyneither(_tmpA21,sizeof(char),
36))),_tag_dyneither(_tmpA22,sizeof(void*),2)))))))))))))),_tag_dyneither(
_tmpA20,sizeof(void*),0)));}++ _tmp60E;_tmp609=_tmp609->tl;_tmp60C=((struct Cyc_List_List*)
_check_null(_tmp60C))->tl;}{struct Cyc_Absyn_Decl*_tmp615;struct Cyc_Absyn_Exp*
_tmp616;struct _tuple11 _tmp614=Cyc_Tcutil_insert_alias((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp609))->hd,Cyc_Tcutil_copy_type((void*)
_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp60C))->hd)->topt)));
_tmp615=_tmp614.f1;_tmp616=_tmp614.f2;_tmp609->hd=(void*)_tmp616;{struct Cyc_List_List*
_tmpA25;_tmp606=((_tmpA25=_region_malloc(r,sizeof(*_tmpA25)),((_tmpA25->hd=
_tmp615,((_tmpA25->tl=_tmp606,_tmpA25))))));}_tmp60D=_tmp60D->tl;};}goto _LL2C2;}
_LL2C5:;_LL2C6: {const char*_tmpA28;void*_tmpA27;(_tmpA27=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA28="not a function call!",
_tag_dyneither(_tmpA28,sizeof(char),21))),_tag_dyneither(_tmpA27,sizeof(void*),0)));}
_LL2C2:;}goto _LL2BD;_LL2C0:;_LL2C1: {const char*_tmpA2B;void*_tmpA2A;(_tmpA2A=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpA2B="not a function call!",_tag_dyneither(_tmpA2B,sizeof(char),21))),
_tag_dyneither(_tmpA2A,sizeof(void*),0)));}_LL2BD:;}while(_tmp606 != 0){struct Cyc_Absyn_Decl*
_tmp61C=(struct Cyc_Absyn_Decl*)_tmp606->hd;fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(
_tmp61C,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);_tmp606=_tmp606->tl;}e->topt=
0;e->r=fn_exp->r;};_pop_region(r);}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){unsigned int loc=e->loc;void*t;{void*_tmp61D=
e->r;struct Cyc_Absyn_Exp*_tmp61F;struct Cyc_Absyn_Exp*_tmp621;struct Cyc_List_List*
_tmp622;struct Cyc_Absyn_VarargCallInfo*_tmp623;struct Cyc_Absyn_VarargCallInfo**
_tmp624;int _tmp625;int*_tmp626;struct Cyc_Core_Opt*_tmp628;struct Cyc_List_List*
_tmp629;union Cyc_Absyn_Cnst _tmp62B;union Cyc_Absyn_Cnst*_tmp62C;struct _tuple1*
_tmp62E;void*_tmp62F;void**_tmp630;enum Cyc_Absyn_Primop _tmp632;struct Cyc_List_List*
_tmp633;struct Cyc_Absyn_Exp*_tmp635;enum Cyc_Absyn_Incrementor _tmp636;struct Cyc_Absyn_Exp*
_tmp638;struct Cyc_Core_Opt*_tmp639;struct Cyc_Absyn_Exp*_tmp63A;struct Cyc_Absyn_Exp*
_tmp63C;struct Cyc_Absyn_Exp*_tmp63D;struct Cyc_Absyn_Exp*_tmp63E;struct Cyc_Absyn_Exp*
_tmp640;struct Cyc_Absyn_Exp*_tmp641;struct Cyc_Absyn_Exp*_tmp643;struct Cyc_Absyn_Exp*
_tmp644;struct Cyc_Absyn_Exp*_tmp646;struct Cyc_Absyn_Exp*_tmp647;struct Cyc_Absyn_Exp*
_tmp649;struct Cyc_List_List*_tmp64A;struct Cyc_Absyn_VarargCallInfo*_tmp64B;
struct Cyc_Absyn_VarargCallInfo**_tmp64C;struct Cyc_Absyn_Exp*_tmp64E;struct Cyc_Absyn_Exp*
_tmp650;struct Cyc_List_List*_tmp651;void*_tmp653;struct Cyc_Absyn_Exp*_tmp654;
enum Cyc_Absyn_Coercion _tmp655;enum Cyc_Absyn_Coercion*_tmp656;struct Cyc_Absyn_Exp*
_tmp658;struct Cyc_Absyn_Exp*_tmp65A;struct Cyc_Absyn_Exp**_tmp65B;struct Cyc_Absyn_Exp*
_tmp65C;struct Cyc_Absyn_Exp*_tmp65E;void*_tmp660;void*_tmp662;void*_tmp663;
struct Cyc_Absyn_Exp*_tmp665;struct Cyc_Absyn_Exp*_tmp667;struct _dyneither_ptr*
_tmp668;int _tmp669;int*_tmp66A;int _tmp66B;int*_tmp66C;struct Cyc_Absyn_Exp*
_tmp66E;struct _dyneither_ptr*_tmp66F;int _tmp670;int*_tmp671;int _tmp672;int*
_tmp673;struct Cyc_Absyn_Exp*_tmp675;struct Cyc_Absyn_Exp*_tmp676;struct Cyc_List_List*
_tmp678;struct _tuple9*_tmp67A;struct Cyc_List_List*_tmp67B;struct Cyc_List_List*
_tmp67D;struct Cyc_Absyn_Stmt*_tmp67F;struct Cyc_Absyn_Vardecl*_tmp681;struct Cyc_Absyn_Exp*
_tmp682;struct Cyc_Absyn_Exp*_tmp683;int _tmp684;int*_tmp685;struct _tuple1*_tmp687;
struct _tuple1**_tmp688;struct Cyc_List_List*_tmp689;struct Cyc_List_List**_tmp68A;
struct Cyc_List_List*_tmp68B;struct Cyc_Absyn_Aggrdecl*_tmp68C;struct Cyc_Absyn_Aggrdecl**
_tmp68D;void*_tmp68F;struct Cyc_List_List*_tmp690;struct Cyc_List_List*_tmp692;
struct Cyc_Absyn_Datatypedecl*_tmp693;struct Cyc_Absyn_Datatypefield*_tmp694;
struct _tuple1*_tmp696;struct _tuple1**_tmp697;struct Cyc_Absyn_Enumdecl*_tmp698;
struct Cyc_Absyn_Enumfield*_tmp699;struct _tuple1*_tmp69B;struct _tuple1**_tmp69C;
void*_tmp69D;struct Cyc_Absyn_Enumfield*_tmp69E;struct Cyc_Absyn_MallocInfo _tmp6A0;
int _tmp6A1;int*_tmp6A2;struct Cyc_Absyn_Exp*_tmp6A3;struct Cyc_Absyn_Exp**_tmp6A4;
void**_tmp6A5;void***_tmp6A6;struct Cyc_Absyn_Exp*_tmp6A7;struct Cyc_Absyn_Exp**
_tmp6A8;int _tmp6A9;int*_tmp6AA;struct Cyc_Absyn_Exp*_tmp6AC;struct Cyc_Absyn_Exp*
_tmp6AD;struct Cyc_Absyn_Exp*_tmp6AF;struct _dyneither_ptr*_tmp6B0;void*_tmp6B2;
_LL2C8: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp61E=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)
_tmp61D;if(_tmp61E->tag != 11)goto _LL2CA;else{_tmp61F=_tmp61E->f1;}}_LL2C9: Cyc_Tcexp_tcExpNoInst(
te,0,_tmp61F);return;_LL2CA:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*
_tmp620=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp620->tag
!= 9)goto _LL2CC;else{_tmp621=_tmp620->f1;_tmp622=_tmp620->f2;_tmp623=_tmp620->f3;
_tmp624=(struct Cyc_Absyn_VarargCallInfo**)& _tmp620->f3;_tmp625=_tmp620->f4;
_tmp626=(int*)& _tmp620->f4;}}if(!(!(*_tmp626)))goto _LL2CC;_LL2CB: Cyc_Tcexp_resolve_unknown_fn(
te,e,_tmp621,_tmp622);*_tmp626=1;Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL2CC: {
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp627=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)
_tmp61D;if(_tmp627->tag != 34)goto _LL2CE;else{_tmp628=_tmp627->f1;_tmp629=_tmp627->f2;}}
_LL2CD: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp629);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL2CE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp62A=(
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp62A->tag != 0)goto
_LL2D0;else{_tmp62B=_tmp62A->f1;_tmp62C=(union Cyc_Absyn_Cnst*)& _tmp62A->f1;}}
_LL2CF: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst*)_tmp62C,e);goto
_LL2C7;_LL2D0: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp62D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp61D;if(_tmp62D->tag != 1)goto _LL2D2;else{_tmp62E=_tmp62D->f1;_tmp62F=(void**)&
_tmp62D->f2;_tmp630=(void**)((void**)& _tmp62D->f2);}}_LL2D1: t=Cyc_Tcexp_tcVar(te,
loc,topt,e,_tmp62E,(void**)_tmp630);goto _LL2C7;_LL2D2: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*
_tmp631=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp631->tag
!= 2)goto _LL2D4;else{_tmp632=_tmp631->f1;_tmp633=_tmp631->f2;}}_LL2D3: t=Cyc_Tcexp_tcPrimop(
te,loc,topt,_tmp632,_tmp633);goto _LL2C7;_LL2D4: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*
_tmp634=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp634->tag
!= 4)goto _LL2D6;else{_tmp635=_tmp634->f1;_tmp636=_tmp634->f2;}}_LL2D5: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp635,_tmp636);goto _LL2C7;_LL2D6: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*
_tmp637=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp637->tag
!= 3)goto _LL2D8;else{_tmp638=_tmp637->f1;_tmp639=_tmp637->f2;_tmp63A=_tmp637->f3;}}
_LL2D7: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp638,_tmp639,_tmp63A);goto _LL2C7;
_LL2D8: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp63B=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)
_tmp61D;if(_tmp63B->tag != 5)goto _LL2DA;else{_tmp63C=_tmp63B->f1;_tmp63D=_tmp63B->f2;
_tmp63E=_tmp63B->f3;}}_LL2D9: t=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp63C,
_tmp63D,_tmp63E);goto _LL2C7;_LL2DA: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*
_tmp63F=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp63F->tag != 6)
goto _LL2DC;else{_tmp640=_tmp63F->f1;_tmp641=_tmp63F->f2;}}_LL2DB: t=Cyc_Tcexp_tcAnd(
te,loc,_tmp640,_tmp641);goto _LL2C7;_LL2DC: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*
_tmp642=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp642->tag != 7)
goto _LL2DE;else{_tmp643=_tmp642->f1;_tmp644=_tmp642->f2;}}_LL2DD: t=Cyc_Tcexp_tcOr(
te,loc,_tmp643,_tmp644);goto _LL2C7;_LL2DE: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*
_tmp645=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp645->tag
!= 8)goto _LL2E0;else{_tmp646=_tmp645->f1;_tmp647=_tmp645->f2;}}_LL2DF: t=Cyc_Tcexp_tcSeqExp(
te,loc,topt,_tmp646,_tmp647);goto _LL2C7;_LL2E0: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*
_tmp648=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp648->tag
!= 9)goto _LL2E2;else{_tmp649=_tmp648->f1;_tmp64A=_tmp648->f2;_tmp64B=_tmp648->f3;
_tmp64C=(struct Cyc_Absyn_VarargCallInfo**)& _tmp648->f3;}}_LL2E1: {struct Cyc_List_List*
alias_arg_exps=0;int ok=1;struct Cyc_Absyn_Exp*fn_exp;{struct _handler_cons _tmp6B4;
_push_handler(& _tmp6B4);{int _tmp6B6=0;if(setjmp(_tmp6B4.handler))_tmp6B6=1;if(!
_tmp6B6){fn_exp=Cyc_Tcutil_deep_copy_exp(e);;_pop_handler();}else{void*_tmp6B5=(
void*)_exn_thrown;void*_tmp6B8=_tmp6B5;struct _dyneither_ptr _tmp6BA;_LL319: {
struct Cyc_Core_Failure_exn_struct*_tmp6B9=(struct Cyc_Core_Failure_exn_struct*)
_tmp6B8;if(_tmp6B9->tag != Cyc_Core_Failure)goto _LL31B;else{_tmp6BA=_tmp6B9->f1;}}
_LL31A: ok=0;fn_exp=e;goto _LL318;_LL31B:;_LL31C:(void)_throw(_tmp6B8);_LL318:;}};}
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp649,_tmp64A,_tmp64C,& alias_arg_exps);if(
alias_arg_exps != 0  && ok){alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(alias_arg_exps);Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,
alias_arg_exps);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;}goto _LL2C7;}_LL2E2: {
struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp64D=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)
_tmp61D;if(_tmp64D->tag != 10)goto _LL2E4;else{_tmp64E=_tmp64D->f1;}}_LL2E3: t=Cyc_Tcexp_tcThrow(
te,loc,topt,_tmp64E);goto _LL2C7;_LL2E4: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*
_tmp64F=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp64F->tag
!= 12)goto _LL2E6;else{_tmp650=_tmp64F->f1;_tmp651=_tmp64F->f2;}}_LL2E5: t=Cyc_Tcexp_tcInstantiate(
te,loc,topt,_tmp650,_tmp651);goto _LL2C7;_LL2E6: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*
_tmp652=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp652->tag != 
13)goto _LL2E8;else{_tmp653=(void*)_tmp652->f1;_tmp654=_tmp652->f2;_tmp655=
_tmp652->f4;_tmp656=(enum Cyc_Absyn_Coercion*)& _tmp652->f4;}}_LL2E7: t=Cyc_Tcexp_tcCast(
te,loc,topt,_tmp653,_tmp654,(enum Cyc_Absyn_Coercion*)_tmp656);goto _LL2C7;_LL2E8: {
struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp657=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)
_tmp61D;if(_tmp657->tag != 14)goto _LL2EA;else{_tmp658=_tmp657->f1;}}_LL2E9: t=Cyc_Tcexp_tcAddress(
te,loc,e,topt,_tmp658);goto _LL2C7;_LL2EA: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*
_tmp659=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp659->tag != 
15)goto _LL2EC;else{_tmp65A=_tmp659->f1;_tmp65B=(struct Cyc_Absyn_Exp**)& _tmp659->f1;
_tmp65C=_tmp659->f2;}}_LL2EB: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp65B,e,_tmp65C);
goto _LL2C7;_LL2EC: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp65D=(
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp65D->tag != 17)
goto _LL2EE;else{_tmp65E=_tmp65D->f1;}}_LL2ED: {void*_tmp6BB=Cyc_Tcexp_tcExpNoPromote(
te,0,_tmp65E);t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp6BB);goto _LL2C7;}_LL2EE: {
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp65F=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)
_tmp61D;if(_tmp65F->tag != 16)goto _LL2F0;else{_tmp660=(void*)_tmp65F->f1;}}_LL2EF:
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp660);goto _LL2C7;_LL2F0: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*
_tmp661=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp661->tag
!= 18)goto _LL2F2;else{_tmp662=(void*)_tmp661->f1;_tmp663=(void*)_tmp661->f2;}}
_LL2F1: t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp662,_tmp663);goto _LL2C7;_LL2F2: {
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp664=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)
_tmp61D;if(_tmp664->tag != 19)goto _LL2F4;else{_tmp665=_tmp664->f1;}}_LL2F3: t=Cyc_Tcexp_tcDeref(
te,loc,topt,_tmp665);goto _LL2C7;_LL2F4: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*
_tmp666=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp666->tag
!= 20)goto _LL2F6;else{_tmp667=_tmp666->f1;_tmp668=_tmp666->f2;_tmp669=_tmp666->f3;
_tmp66A=(int*)& _tmp666->f3;_tmp66B=_tmp666->f4;_tmp66C=(int*)& _tmp666->f4;}}
_LL2F5: t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp667,_tmp668,_tmp66A,_tmp66C);
goto _LL2C7;_LL2F6: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp66D=(
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp66D->tag != 21)
goto _LL2F8;else{_tmp66E=_tmp66D->f1;_tmp66F=_tmp66D->f2;_tmp670=_tmp66D->f3;
_tmp671=(int*)& _tmp66D->f3;_tmp672=_tmp66D->f4;_tmp673=(int*)& _tmp66D->f4;}}
_LL2F7: t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp66E,_tmp66F,_tmp671,_tmp673);goto
_LL2C7;_LL2F8: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp674=(struct
Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp674->tag != 22)goto
_LL2FA;else{_tmp675=_tmp674->f1;_tmp676=_tmp674->f2;}}_LL2F9: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp675,_tmp676);goto _LL2C7;_LL2FA: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*
_tmp677=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp677->tag != 
23)goto _LL2FC;else{_tmp678=_tmp677->f1;}}_LL2FB: t=Cyc_Tcexp_tcTuple(te,loc,topt,
_tmp678);goto _LL2C7;_LL2FC: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*
_tmp679=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp679->tag
!= 24)goto _LL2FE;else{_tmp67A=_tmp679->f1;_tmp67B=_tmp679->f2;}}_LL2FD: t=Cyc_Tcexp_tcCompoundLit(
te,loc,topt,_tmp67A,_tmp67B);goto _LL2C7;_LL2FE: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*
_tmp67C=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp67C->tag != 
25)goto _LL300;else{_tmp67D=_tmp67C->f1;}}_LL2FF: {void**elt_topt=0;struct Cyc_Absyn_Tqual*
elt_tqopt=0;int zero_term=0;if(topt != 0){void*_tmp6BC=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_ArrayInfo _tmp6BE;void*_tmp6BF;void**_tmp6C0;struct Cyc_Absyn_Tqual
_tmp6C1;struct Cyc_Absyn_Tqual*_tmp6C2;union Cyc_Absyn_Constraint*_tmp6C3;_LL31E: {
struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp6BD=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)
_tmp6BC;if(_tmp6BD->tag != 8)goto _LL320;else{_tmp6BE=_tmp6BD->f1;_tmp6BF=_tmp6BE.elt_type;
_tmp6C0=(void**)&(_tmp6BD->f1).elt_type;_tmp6C1=_tmp6BE.tq;_tmp6C2=(struct Cyc_Absyn_Tqual*)&(
_tmp6BD->f1).tq;_tmp6C3=_tmp6BE.zero_term;}}_LL31F: elt_topt=(void**)_tmp6C0;
elt_tqopt=(struct Cyc_Absyn_Tqual*)_tmp6C2;zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp6C3);goto _LL31D;_LL320:;_LL321: goto _LL31D;_LL31D:;}
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp67D);goto _LL2C7;}
_LL300: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp67E=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)
_tmp61D;if(_tmp67E->tag != 35)goto _LL302;else{_tmp67F=_tmp67E->f1;}}_LL301: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp67F);goto _LL2C7;_LL302: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*
_tmp680=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp61D;if(
_tmp680->tag != 26)goto _LL304;else{_tmp681=_tmp680->f1;_tmp682=_tmp680->f2;
_tmp683=_tmp680->f3;_tmp684=_tmp680->f4;_tmp685=(int*)& _tmp680->f4;}}_LL303: t=
Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp681,_tmp682,_tmp683,_tmp685);goto
_LL2C7;_LL304: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp686=(struct
Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp686->tag != 27)goto
_LL306;else{_tmp687=_tmp686->f1;_tmp688=(struct _tuple1**)& _tmp686->f1;_tmp689=
_tmp686->f2;_tmp68A=(struct Cyc_List_List**)& _tmp686->f2;_tmp68B=_tmp686->f3;
_tmp68C=_tmp686->f4;_tmp68D=(struct Cyc_Absyn_Aggrdecl**)& _tmp686->f4;}}_LL305: t=
Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp688,_tmp68A,_tmp68B,_tmp68D);goto _LL2C7;
_LL306: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp68E=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)
_tmp61D;if(_tmp68E->tag != 28)goto _LL308;else{_tmp68F=(void*)_tmp68E->f1;_tmp690=
_tmp68E->f2;}}_LL307: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp68F,_tmp690);goto _LL2C7;
_LL308: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp691=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)
_tmp61D;if(_tmp691->tag != 29)goto _LL30A;else{_tmp692=_tmp691->f1;_tmp693=_tmp691->f2;
_tmp694=_tmp691->f3;}}_LL309: t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp692,
_tmp693,_tmp694);goto _LL2C7;_LL30A: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*
_tmp695=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp695->tag != 
30)goto _LL30C;else{_tmp696=_tmp695->f1;_tmp697=(struct _tuple1**)& _tmp695->f1;
_tmp698=_tmp695->f2;_tmp699=_tmp695->f3;}}_LL30B:*_tmp697=((struct Cyc_Absyn_Enumfield*)
_check_null(_tmp699))->name;{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpA2E;
struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpA2D;t=(void*)((_tmpA2D=_cycalloc(
sizeof(*_tmpA2D)),((_tmpA2D[0]=((_tmpA2E.tag=13,((_tmpA2E.f1=((struct Cyc_Absyn_Enumdecl*)
_check_null(_tmp698))->name,((_tmpA2E.f2=_tmp698,_tmpA2E)))))),_tmpA2D))));}goto
_LL2C7;_LL30C: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp69A=(struct
Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp69A->tag != 31)goto
_LL30E;else{_tmp69B=_tmp69A->f1;_tmp69C=(struct _tuple1**)& _tmp69A->f1;_tmp69D=(
void*)_tmp69A->f2;_tmp69E=_tmp69A->f3;}}_LL30D:*_tmp69C=((struct Cyc_Absyn_Enumfield*)
_check_null(_tmp69E))->name;t=_tmp69D;goto _LL2C7;_LL30E: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*
_tmp69F=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp69F->tag
!= 32)goto _LL310;else{_tmp6A0=_tmp69F->f1;_tmp6A1=_tmp6A0.is_calloc;_tmp6A2=(int*)&(
_tmp69F->f1).is_calloc;_tmp6A3=_tmp6A0.rgn;_tmp6A4=(struct Cyc_Absyn_Exp**)&(
_tmp69F->f1).rgn;_tmp6A5=_tmp6A0.elt_type;_tmp6A6=(void***)&(_tmp69F->f1).elt_type;
_tmp6A7=_tmp6A0.num_elts;_tmp6A8=(struct Cyc_Absyn_Exp**)&(_tmp69F->f1).num_elts;
_tmp6A9=_tmp6A0.fat_result;_tmp6AA=(int*)&(_tmp69F->f1).fat_result;}}_LL30F: t=
Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp6A4,_tmp6A6,_tmp6A8,_tmp6A2,_tmp6AA);goto
_LL2C7;_LL310: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp6AB=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)
_tmp61D;if(_tmp6AB->tag != 33)goto _LL312;else{_tmp6AC=_tmp6AB->f1;_tmp6AD=_tmp6AB->f2;}}
_LL311: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp6AC,_tmp6AD);goto _LL2C7;_LL312: {
struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp6AE=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)
_tmp61D;if(_tmp6AE->tag != 36)goto _LL314;else{_tmp6AF=_tmp6AE->f1;_tmp6B0=_tmp6AE->f2;}}
_LL313: t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp6AF,_tmp6B0);goto _LL2C7;_LL314: {
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp6B1=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)
_tmp61D;if(_tmp6B1->tag != 37)goto _LL316;else{_tmp6B2=(void*)_tmp6B1->f1;}}_LL315:
if(!te->allow_valueof){const char*_tmpA31;void*_tmpA30;(_tmpA30=0,Cyc_Tcutil_terr(
e->loc,((_tmpA31="valueof(-) can only occur within types",_tag_dyneither(_tmpA31,
sizeof(char),39))),_tag_dyneither(_tmpA30,sizeof(void*),0)));}t=Cyc_Absyn_sint_typ;
goto _LL2C7;_LL316: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp6B3=(struct
Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp61D;if(_tmp6B3->tag != 38)goto _LL2C7;}
_LL317:{const char*_tmpA34;void*_tmpA33;(_tmpA33=0,Cyc_Tcutil_terr(e->loc,((
_tmpA34="asm expressions cannot occur within Cyclone code.",_tag_dyneither(
_tmpA34,sizeof(char),50))),_tag_dyneither(_tmpA33,sizeof(void*),0)));}t=Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));_LL2C7:;}e->topt=(void*)t;}
