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
Cyc_List_list_t;typedef struct Cyc_List_List*Cyc_List_List_t;struct Cyc_List_List*
Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{
char*tag;};struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[
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
char*tag;};struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(unsigned int);extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_exn_typ();
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(
unsigned int loc);struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);extern int
Cyc_Absyn_no_regions;struct Cyc_PP_Ppstate;typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;
struct Cyc_PP_Out;typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;typedef
struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int
qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int
print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int
print_externC_stmts;int print_full_evars;int print_zeroterm;int
generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;typedef struct Cyc_Set_Set*
Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*
tag;};struct Cyc_Dict_T;typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};typedef
struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{
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
Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(
unsigned int,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);void*
Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(
struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,unsigned int,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct
_RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*continue_dest);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct
Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_get_try_depth(struct
Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_goto(struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt**);
struct _tuple10{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const
struct Cyc_Tcenv_CList*f3;};const struct _tuple10*const Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct
_dyneither_ptr*,struct Cyc_Absyn_Stmt*);int Cyc_Tcenv_all_labels_resolved(struct
Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,
unsigned int,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(
struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*
name);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc);
struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**
oldtv,unsigned int loc);void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int
resetable,int opened);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,
unsigned int,void*rgn);void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,
unsigned int,void*rgn);void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*
te);void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(unsigned int,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(unsigned int,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_copy_type(void*t);
void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*,int*alias_coercion);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_is_pointer_type(
void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*
k);void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
struct _tuple11*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);int
Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(
struct _RegionHandle*,void*t);int Cyc_Tcutil_new_tvar_id();int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_extract_const_from_typedef(
unsigned int,int declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);void Cyc_Tcutil_check_no_qual(unsigned int loc,
void*t);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
struct _tuple12{void*f1;void*f2;};struct Cyc_Tcexp_TestEnv{struct _tuple12*eq;int
isTrue;};typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;struct Cyc_Tcexp_TestEnv
Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr msg_part);extern int Cyc_Tcexp_in_stmt_exp;struct _tuple13{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple13*
tvars_and_bounds_opt;struct Cyc_List_List*patvars;};typedef struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcpat_result_t;struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p,void**topt);void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_switch_exhaustive(unsigned int,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);int Cyc_Tcpat_check_let_pat_exhaustive(
unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_catch_overlap(
unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{
int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{
int tag;int f1;void*f2;};typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 
1,Cyc_CfFlowInfo_AllIL  = 2};typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;
struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple14{struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple14 val;};struct
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
_union_FlowInfo_BottomFL{int tag;int val;};struct _tuple15{struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple15
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
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_Tcstmt_tcStmt(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);static void Cyc_Tcstmt_simplify_unused_result_exp(
struct Cyc_Absyn_Exp*e){void*_tmp0=e->r;struct Cyc_Absyn_Exp*_tmp2;enum Cyc_Absyn_Incrementor
_tmp3;struct Cyc_Absyn_Exp*_tmp5;enum Cyc_Absyn_Incrementor _tmp6;_LL1: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*
_tmp1=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp1->tag != 4)
goto _LL3;else{_tmp2=_tmp1->f1;_tmp3=_tmp1->f2;if(_tmp3 != Cyc_Absyn_PostInc)goto
_LL3;}}_LL2:{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp17B;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*
_tmp17A;e->r=(void*)((_tmp17A=_cycalloc(sizeof(*_tmp17A)),((_tmp17A[0]=((_tmp17B.tag=
4,((_tmp17B.f1=_tmp2,((_tmp17B.f2=Cyc_Absyn_PreInc,_tmp17B)))))),_tmp17A))));}
goto _LL0;_LL3: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp4=(struct
Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp4->tag != 4)goto _LL5;
else{_tmp5=_tmp4->f1;_tmp6=_tmp4->f2;if(_tmp6 != Cyc_Absyn_PostDec)goto _LL5;}}
_LL4:{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp17E;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*
_tmp17D;e->r=(void*)((_tmp17D=_cycalloc(sizeof(*_tmp17D)),((_tmp17D[0]=((_tmp17E.tag=
4,((_tmp17E.f1=_tmp5,((_tmp17E.f2=Cyc_Absyn_PreDec,_tmp17E)))))),_tmp17D))));}
goto _LL0;_LL5:;_LL6: goto _LL0;_LL0:;}static void Cyc_Tcstmt_decorate_stmt(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s){Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(
te));s->try_depth=Cyc_Tcenv_get_try_depth(te);}static void Cyc_Tcstmt_pattern_synth(
unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){struct
_tuple13*_tmpC;struct Cyc_List_List*_tmpD;struct Cyc_Tcpat_TcPatResult _tmpB=
pat_res;_tmpC=_tmpB.tvars_and_bounds_opt;_tmpD=_tmpB.patvars;{struct Cyc_List_List*
_tmpE=_tmpC == 0?0:(*_tmpC).f1;struct Cyc_List_List*_tmpF=_tmpC == 0?0:(*_tmpC).f2;
struct _RegionHandle _tmp10=_new_region("r");struct _RegionHandle*r=& _tmp10;
_push_region(r);{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r,loc,te,_tmpE);
te2=Cyc_Tcenv_new_outlives_constraints(r,te2,_tmpF,loc);if(new_block)te2=Cyc_Tcenv_new_block(
r,loc,te2);{void*_tmp11=Cyc_Tcenv_curr_rgn(te2);{struct Cyc_List_List*_tmp12=
_tmpD;for(0;_tmp12 != 0;_tmp12=_tmp12->tl){te2=Cyc_Tcenv_add_pat_var(r,loc,te2,(
struct Cyc_Absyn_Vardecl*)_tmp12->hd);((struct Cyc_Absyn_Vardecl*)_tmp12->hd)->rgn=(
void*)_tmp11;}}if(where_opt != 0){const char*_tmp17F;Cyc_Tcexp_tcTest(te2,(struct
Cyc_Absyn_Exp*)where_opt,((_tmp17F="switch clause guard",_tag_dyneither(_tmp17F,
sizeof(char),20))));}if(_tmpD != 0)te2=Cyc_Tcenv_set_encloser(r,te2,s);Cyc_Tcstmt_tcStmt(
te2,s,0);};}if(_tmpD != 0)Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(
te));;_pop_region(r);};}static struct Cyc_List_List*Cyc_Tcstmt_cmap_c(struct
_RegionHandle*r,void*(*f)(void*,void*),void*env,const struct Cyc_Tcenv_CList*const
x){if(x == (const struct Cyc_Tcenv_CList*)0)return 0;else{struct Cyc_List_List*
_tmp180;return(_tmp180=_region_malloc(r,sizeof(*_tmp180)),((_tmp180->hd=(void*)f(
env,(void*)x->hd),((_tmp180->tl=Cyc_Tcstmt_cmap_c(r,f,env,x->tl),_tmp180)))));}}
static void Cyc_Tcstmt_tcStmtRefine(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s0,void*r1,void*r2,int new_block){struct _RegionHandle _tmp15=_new_region("r");
struct _RegionHandle*r=& _tmp15;_push_region(r);{struct _tuple11*tk=0;struct Cyc_Tcenv_Tenv*
_tmp16=Cyc_Tcenv_add_region_equality(r,te,r1,r2,& tk,s0->loc);Cyc_Tcstmt_tcStmt(
_tmp16,s0,new_block);if(tk != 0)((*((struct _tuple11*)_check_null(tk))).f1)->kind=(*((
struct _tuple11*)_check_null(tk))).f2;};_pop_region(r);}void Cyc_Tcstmt_tcStmt(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){Cyc_Tcstmt_decorate_stmt(
te,s0);{void*_tmp17=s0->r;struct Cyc_Absyn_Exp*_tmp1A;struct Cyc_Absyn_Stmt*_tmp1C;
struct Cyc_Absyn_Stmt*_tmp1D;struct Cyc_Absyn_Exp*_tmp1F;struct Cyc_Absyn_Exp*
_tmp21;struct Cyc_Absyn_Stmt*_tmp22;struct Cyc_Absyn_Stmt*_tmp23;struct _tuple9
_tmp25;struct Cyc_Absyn_Exp*_tmp26;struct Cyc_Absyn_Stmt*_tmp27;struct Cyc_Absyn_Stmt*
_tmp28;struct Cyc_Absyn_Exp*_tmp2A;struct _tuple9 _tmp2B;struct Cyc_Absyn_Exp*_tmp2C;
struct Cyc_Absyn_Stmt*_tmp2D;struct _tuple9 _tmp2E;struct Cyc_Absyn_Exp*_tmp2F;
struct Cyc_Absyn_Stmt*_tmp30;struct Cyc_Absyn_Stmt*_tmp31;struct Cyc_Absyn_Stmt*
_tmp33;struct _tuple9 _tmp34;struct Cyc_Absyn_Exp*_tmp35;struct Cyc_Absyn_Stmt*
_tmp36;struct Cyc_Absyn_Stmt*_tmp38;struct Cyc_Absyn_Stmt**_tmp39;struct Cyc_Absyn_Stmt*
_tmp3B;struct Cyc_Absyn_Stmt**_tmp3C;struct _dyneither_ptr*_tmp3E;struct Cyc_Absyn_Stmt*
_tmp3F;struct Cyc_Absyn_Stmt**_tmp40;struct Cyc_List_List*_tmp42;struct Cyc_Absyn_Switch_clause**
_tmp43;struct Cyc_Absyn_Switch_clause***_tmp44;struct _dyneither_ptr*_tmp46;struct
Cyc_Absyn_Stmt*_tmp47;struct Cyc_Absyn_Exp*_tmp49;struct Cyc_List_List*_tmp4A;
struct Cyc_Absyn_Stmt*_tmp4C;struct Cyc_List_List*_tmp4D;struct Cyc_Absyn_Decl*
_tmp4F;struct Cyc_Absyn_Stmt*_tmp50;struct Cyc_Absyn_Exp*_tmp52;_LL8: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*
_tmp18=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp17;if(_tmp18->tag != 0)
goto _LLA;}_LL9: return;_LLA: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp19=(
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp17;if(_tmp19->tag != 1)goto _LLC;
else{_tmp1A=_tmp19->f1;}}_LLB: Cyc_Tcexp_tcExp(te,0,_tmp1A);if(!Cyc_Tcexp_in_stmt_exp)
Cyc_Tcstmt_simplify_unused_result_exp(_tmp1A);return;_LLC: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*
_tmp1B=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp17;if(_tmp1B->tag != 2)
goto _LLE;else{_tmp1C=_tmp1B->f1;_tmp1D=_tmp1B->f2;}}_LLD: {struct _RegionHandle
_tmp53=_new_region("r");struct _RegionHandle*r=& _tmp53;_push_region(r);{struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct
_tmp183;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp182;struct Cyc_Tcenv_Tenv*
_tmp54=Cyc_Tcenv_set_next(r,te,(void*)((_tmp182=_cycalloc(sizeof(*_tmp182)),((
_tmp182[0]=((_tmp183.tag=3,((_tmp183.f1=_tmp1D,_tmp183)))),_tmp182)))));Cyc_Tcstmt_tcStmt(
_tmp54,_tmp1C,1);}Cyc_Tcstmt_tcStmt(te,_tmp1D,1);_npop_handler(0);return;;
_pop_region(r);}_LLE: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp1E=(
struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp17;if(_tmp1E->tag != 3)goto
_LL10;else{_tmp1F=_tmp1E->f1;}}_LLF: {void*t=Cyc_Tcenv_return_typ(te);if(_tmp1F
== 0){void*_tmp57=Cyc_Tcutil_compress(t);_LL2B: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*
_tmp58=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp57;if(_tmp58->tag != 0)
goto _LL2D;}_LL2C: goto _LL2A;_LL2D: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*
_tmp59=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp57;if(_tmp59->tag != 7)
goto _LL2F;}_LL2E: goto _LL30;_LL2F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp5A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp57;if(_tmp5A->tag != 6)
goto _LL31;}_LL30:{const char*_tmp187;void*_tmp186[1];struct Cyc_String_pa_PrintArg_struct
_tmp185;(_tmp185.tag=0,((_tmp185.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp186[0]=& _tmp185,Cyc_Tcutil_warn(s0->loc,((
_tmp187="should return a value of type %s",_tag_dyneither(_tmp187,sizeof(char),
33))),_tag_dyneither(_tmp186,sizeof(void*),1)))))));}goto _LL2A;_LL31:;_LL32:{
const char*_tmp18B;void*_tmp18A[1];struct Cyc_String_pa_PrintArg_struct _tmp189;(
_tmp189.tag=0,((_tmp189.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp18A[0]=& _tmp189,Cyc_Tcutil_terr(s0->loc,((_tmp18B="must return a value of type %s",
_tag_dyneither(_tmp18B,sizeof(char),31))),_tag_dyneither(_tmp18A,sizeof(void*),1)))))));}
goto _LL2A;_LL2A:;}else{int bogus=0;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
_tmp1F;Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){{
const char*_tmp190;void*_tmp18F[2];struct Cyc_String_pa_PrintArg_struct _tmp18E;
struct Cyc_String_pa_PrintArg_struct _tmp18D;(_tmp18D.tag=0,((_tmp18D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp18E.tag=0,((
_tmp18E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)_check_null(e->topt))),((_tmp18F[0]=& _tmp18E,((_tmp18F[1]=& _tmp18D,Cyc_Tcutil_terr(
s0->loc,((_tmp190="returns value of type %s but requires %s",_tag_dyneither(
_tmp190,sizeof(char),41))),_tag_dyneither(_tmp18F,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{struct _RegionHandle _tmp65=_new_region("temp");
struct _RegionHandle*temp=& _tmp65;_push_region(temp);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){const char*_tmp193;void*_tmp192;(
_tmp192=0,Cyc_Tcutil_terr(e->loc,((_tmp193="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp193,sizeof(char),49))),_tag_dyneither(_tmp192,sizeof(void*),0)));};
_pop_region(temp);};}return;}_LL10: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*
_tmp20=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp17;if(_tmp20->tag
!= 4)goto _LL12;else{_tmp21=_tmp20->f1;_tmp22=_tmp20->f2;_tmp23=_tmp20->f3;}}
_LL11: {struct _tuple12*_tmp6A;int _tmp6B;const char*_tmp194;struct Cyc_Tcexp_TestEnv
_tmp69=Cyc_Tcexp_tcTest(te,_tmp21,((_tmp194="if statement",_tag_dyneither(
_tmp194,sizeof(char),13))));_tmp6A=_tmp69.eq;_tmp6B=_tmp69.isTrue;{struct
_tuple12*_tmp6C=_tmp6A;struct _tuple12 _tmp6D;void*_tmp6E;void*_tmp6F;_LL34: if(
_tmp6C == 0)goto _LL36;_tmp6D=*_tmp6C;_tmp6E=_tmp6D.f1;_tmp6F=_tmp6D.f2;_LL35: if(
_tmp6B){Cyc_Tcstmt_tcStmtRefine(te,_tmp22,_tmp6E,_tmp6F,1);Cyc_Tcstmt_tcStmt(te,
_tmp23,1);}else{Cyc_Tcstmt_tcStmt(te,_tmp22,1);Cyc_Tcstmt_tcStmtRefine(te,_tmp23,
_tmp6E,_tmp6F,1);}goto _LL33;_LL36:;_LL37: Cyc_Tcstmt_tcStmt(te,_tmp22,1);Cyc_Tcstmt_tcStmt(
te,_tmp23,1);goto _LL33;_LL33:;}return;}_LL12: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*
_tmp24=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp17;if(_tmp24->tag != 5)
goto _LL14;else{_tmp25=_tmp24->f1;_tmp26=_tmp25.f1;_tmp27=_tmp25.f2;_tmp28=_tmp24->f2;}}
_LL13: Cyc_Tcstmt_decorate_stmt(te,_tmp27);{const char*_tmp195;Cyc_Tcexp_tcTest(te,
_tmp26,((_tmp195="while loop",_tag_dyneither(_tmp195,sizeof(char),11))));}{
struct _RegionHandle _tmp71=_new_region("r");struct _RegionHandle*r=& _tmp71;
_push_region(r);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(r,te,_tmp27),_tmp28,1);
_npop_handler(0);return;;_pop_region(r);};_LL14: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*
_tmp29=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp17;if(_tmp29->tag != 9)
goto _LL16;else{_tmp2A=_tmp29->f1;_tmp2B=_tmp29->f2;_tmp2C=_tmp2B.f1;_tmp2D=
_tmp2B.f2;_tmp2E=_tmp29->f3;_tmp2F=_tmp2E.f1;_tmp30=_tmp2E.f2;_tmp31=_tmp29->f4;}}
_LL15: Cyc_Tcstmt_decorate_stmt(te,_tmp2D);Cyc_Tcstmt_decorate_stmt(te,_tmp30);
Cyc_Tcexp_tcExp(te,0,_tmp2A);{const char*_tmp196;Cyc_Tcexp_tcTest(te,_tmp2C,((
_tmp196="for loop",_tag_dyneither(_tmp196,sizeof(char),9))));}{struct
_RegionHandle _tmp73=_new_region("r");struct _RegionHandle*r=& _tmp73;_push_region(
r);{struct Cyc_Tcenv_Tenv*_tmp74=Cyc_Tcenv_set_in_loop(r,te,_tmp30);Cyc_Tcstmt_tcStmt(
_tmp74,_tmp31,1);Cyc_Tcexp_tcExp(_tmp74,0,_tmp2F);Cyc_Tcstmt_simplify_unused_result_exp(
_tmp2F);}_npop_handler(0);return;;_pop_region(r);};_LL16: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*
_tmp32=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp17;if(_tmp32->tag != 14)
goto _LL18;else{_tmp33=_tmp32->f1;_tmp34=_tmp32->f2;_tmp35=_tmp34.f1;_tmp36=
_tmp34.f2;}}_LL17: Cyc_Tcstmt_decorate_stmt(te,_tmp36);{struct _RegionHandle _tmp75=
_new_region("r");struct _RegionHandle*r=& _tmp75;_push_region(r);Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_in_loop(r,te,_tmp36),_tmp33,1);{const char*_tmp197;Cyc_Tcexp_tcTest(
te,_tmp35,((_tmp197="do loop",_tag_dyneither(_tmp197,sizeof(char),8))));}
_npop_handler(0);return;;_pop_region(r);};_LL18: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*
_tmp37=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp17;if(_tmp37->tag != 6)
goto _LL1A;else{_tmp38=_tmp37->f1;_tmp39=(struct Cyc_Absyn_Stmt**)& _tmp37->f1;}}
_LL19: Cyc_Tcenv_process_break(te,s0,_tmp39);return;_LL1A: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*
_tmp3A=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp17;if(_tmp3A->tag
!= 7)goto _LL1C;else{_tmp3B=_tmp3A->f1;_tmp3C=(struct Cyc_Absyn_Stmt**)& _tmp3A->f1;}}
_LL1B: Cyc_Tcenv_process_continue(te,s0,_tmp3C);return;_LL1C: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*
_tmp3D=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp17;if(_tmp3D->tag != 8)
goto _LL1E;else{_tmp3E=_tmp3D->f1;_tmp3F=_tmp3D->f2;_tmp40=(struct Cyc_Absyn_Stmt**)&
_tmp3D->f2;}}_LL1D: Cyc_Tcenv_process_goto(te,s0,_tmp3E,_tmp40);return;_LL1E: {
struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp41=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)
_tmp17;if(_tmp41->tag != 11)goto _LL20;else{_tmp42=_tmp41->f1;_tmp43=_tmp41->f2;
_tmp44=(struct Cyc_Absyn_Switch_clause***)& _tmp41->f2;}}_LL1F: {const struct
_tuple10*_tmp77=Cyc_Tcenv_process_fallthru(te,s0,_tmp44);if(_tmp77 == (const
struct _tuple10*)0){{const char*_tmp19A;void*_tmp199;(_tmp199=0,Cyc_Tcutil_terr(s0->loc,((
_tmp19A="fallthru not in a non-last case",_tag_dyneither(_tmp19A,sizeof(char),32))),
_tag_dyneither(_tmp199,sizeof(void*),0)));}return;}{struct Cyc_List_List*_tmp7A=(*
_tmp77).f2;const struct Cyc_Tcenv_CList*_tmp7B=(*_tmp77).f3;struct Cyc_List_List*
instantiation=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_make_inst_var,Cyc_Tcenv_lookup_type_vars(te),_tmp7A);struct Cyc_List_List*
_tmp7C=((struct Cyc_List_List*(*)(struct _RegionHandle*r,void*(*f)(struct Cyc_List_List*,
void*),struct Cyc_List_List*env,const struct Cyc_Tcenv_CList*x))Cyc_Tcstmt_cmap_c)(
Cyc_Core_heap_region,Cyc_Tcutil_substitute,instantiation,_tmp7B);for(0;_tmp7C != 
0  && _tmp42 != 0;(_tmp7C=_tmp7C->tl,_tmp42=_tmp42->tl)){int bogus=0;Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmp42->hd);if(!Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)
_tmp42->hd,(void*)_tmp7C->hd,& bogus)){{const char*_tmp19F;void*_tmp19E[2];struct
Cyc_String_pa_PrintArg_struct _tmp19D;struct Cyc_String_pa_PrintArg_struct _tmp19C;(
_tmp19C.tag=0,((_tmp19C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)_tmp7C->hd)),((_tmp19D.tag=0,((_tmp19D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)
_tmp42->hd)->topt))),((_tmp19E[0]=& _tmp19D,((_tmp19E[1]=& _tmp19C,Cyc_Tcutil_terr(
s0->loc,((_tmp19F="fallthru argument has type %s but pattern variable has type %s",
_tag_dyneither(_tmp19F,sizeof(char),63))),_tag_dyneither(_tmp19E,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{struct _RegionHandle _tmp81=_new_region("temp");
struct _RegionHandle*temp=& _tmp81;_push_region(temp);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
temp,(void*)_tmp7C->hd) && !Cyc_Tcutil_is_noalias_path(temp,(struct Cyc_Absyn_Exp*)
_tmp42->hd)){const char*_tmp1A2;void*_tmp1A1;(_tmp1A1=0,Cyc_Tcutil_terr(((struct
Cyc_Absyn_Exp*)_tmp42->hd)->loc,((_tmp1A2="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp1A2,sizeof(char),49))),_tag_dyneither(_tmp1A1,sizeof(void*),0)));};
_pop_region(temp);};}if(_tmp42 != 0){const char*_tmp1A5;void*_tmp1A4;(_tmp1A4=0,
Cyc_Tcutil_terr(s0->loc,((_tmp1A5="too many arguments to explicit fallthru",
_tag_dyneither(_tmp1A5,sizeof(char),40))),_tag_dyneither(_tmp1A4,sizeof(void*),0)));}
if(_tmp7C != 0){const char*_tmp1A8;void*_tmp1A7;(_tmp1A7=0,Cyc_Tcutil_terr(s0->loc,((
_tmp1A8="too few arguments to explicit fallthru",_tag_dyneither(_tmp1A8,sizeof(
char),39))),_tag_dyneither(_tmp1A7,sizeof(void*),0)));}return;};}_LL20: {struct
Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp45=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)
_tmp17;if(_tmp45->tag != 13)goto _LL22;else{_tmp46=_tmp45->f1;_tmp47=_tmp45->f2;}}
_LL21: {struct _RegionHandle _tmp88=_new_region("r");struct _RegionHandle*r=& _tmp88;
_push_region(r);{struct _dyneither_ptr*_tmp1B5;const char*_tmp1B4;void*_tmp1B3[1];
struct Cyc_String_pa_PrintArg_struct _tmp1B2;struct Cyc_Absyn_Tvar*_tmp1B1;Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_new_named_block(r,s0->loc,Cyc_Tcenv_add_label(te,_tmp46,_tmp47),((
_tmp1B1=_cycalloc(sizeof(*_tmp1B1)),((_tmp1B1->name=((_tmp1B5=_cycalloc(sizeof(*
_tmp1B5)),((_tmp1B5[0]=(struct _dyneither_ptr)((_tmp1B2.tag=0,((_tmp1B2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp46),((_tmp1B3[0]=& _tmp1B2,Cyc_aprintf(((
_tmp1B4="`%s",_tag_dyneither(_tmp1B4,sizeof(char),4))),_tag_dyneither(_tmp1B3,
sizeof(void*),1)))))))),_tmp1B5)))),((_tmp1B1->identity=Cyc_Tcutil_new_tvar_id(),((
_tmp1B1->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmp1B1))))))))),_tmp47,0);}
_npop_handler(0);return;;_pop_region(r);}_LL22: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*
_tmp48=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp17;if(_tmp48->tag != 
10)goto _LL24;else{_tmp49=_tmp48->f1;_tmp4A=_tmp48->f2;}}_LL23: Cyc_Tcexp_tcExp(te,
0,_tmp49);{void*_tmp8E=(void*)_check_null(_tmp49->topt);struct _RegionHandle
_tmp8F=_new_region("r");struct _RegionHandle*r=& _tmp8F;_push_region(r);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,_tmp8E) && !Cyc_Tcutil_is_noalias_path(r,_tmp49)){const char*_tmp1B8;void*
_tmp1B7;(_tmp1B7=0,Cyc_Tcutil_terr(_tmp49->loc,((_tmp1B8="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp1B8,sizeof(char),49))),_tag_dyneither(_tmp1B7,sizeof(void*),0)));}{
struct Cyc_Tcenv_Tenv*_tmp92=Cyc_Tcenv_set_in_switch(r,te);_tmp92=Cyc_Tcenv_clear_fallthru(
r,_tmp92);{struct Cyc_List_List*scs=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(_tmp4A);for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Pat*_tmp93=((
struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;struct Cyc_Tcpat_TcPatResult
_tmp94=Cyc_Tcpat_tcPat(_tmp92,_tmp93,& _tmp8E);struct Cyc_List_List*_tmp95=_tmp94.tvars_and_bounds_opt
== 0?0:(*_tmp94.tvars_and_bounds_opt).f1;if(!Cyc_Tcutil_unify((void*)_check_null(
_tmp93->topt),_tmp8E)){{const char*_tmp1BD;void*_tmp1BC[2];struct Cyc_String_pa_PrintArg_struct
_tmp1BB;struct Cyc_String_pa_PrintArg_struct _tmp1BA;(_tmp1BA.tag=0,((_tmp1BA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)
_check_null(_tmp93->topt))),((_tmp1BB.tag=0,((_tmp1BB.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp8E)),((_tmp1BC[0]=& _tmp1BB,((
_tmp1BC[1]=& _tmp1BA,Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,((
_tmp1BD="switch on type %s, but case expects type %s",_tag_dyneither(_tmp1BD,
sizeof(char),44))),_tag_dyneither(_tmp1BC,sizeof(void*),2)))))))))))));}Cyc_Tcutil_explain_failure();}
else{Cyc_Tcpat_check_pat_regions(_tmp92,_tmp93);}{struct Cyc_Core_Opt*_tmp1BE;((
struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=((_tmp1BE=_cycalloc(sizeof(*
_tmp1BE)),((_tmp1BE->v=_tmp94.patvars,_tmp1BE))));}Cyc_Tcstmt_pattern_synth(((
struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,_tmp92,_tmp94,((struct Cyc_Absyn_Switch_clause*)
scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);if(
_tmp94.tvars_and_bounds_opt != 0  && (*_tmp94.tvars_and_bounds_opt).f2 != 0)_tmp92=
Cyc_Tcenv_clear_fallthru(r,_tmp92);else{_tmp92=Cyc_Tcenv_set_fallthru(r,_tmp92,
_tmp95,_tmp94.patvars,(struct Cyc_Absyn_Switch_clause*)scs->hd);}}};};Cyc_Tcpat_check_switch_exhaustive(
s0->loc,te,_tmp4A);_npop_handler(0);return;;_pop_region(r);};_LL24: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*
_tmp4B=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp17;if(_tmp4B->tag
!= 15)goto _LL26;else{_tmp4C=_tmp4B->f1;_tmp4D=_tmp4B->f2;}}_LL25:{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct
_tmp1C1;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp1C0;_tmp4C->r=(Cyc_Absyn_new_stmt((
void*)((_tmp1C0=_cycalloc(sizeof(*_tmp1C0)),((_tmp1C0[0]=((_tmp1C1.tag=2,((
_tmp1C1.f1=Cyc_Absyn_new_stmt(_tmp4C->r,_tmp4C->loc),((_tmp1C1.f2=Cyc_Absyn_skip_stmt(
_tmp4C->loc),_tmp1C1)))))),_tmp1C0)))),_tmp4C->loc))->r;}{struct _RegionHandle
_tmp9D=_new_region("r");struct _RegionHandle*r=& _tmp9D;_push_region(r);Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_encloser(r,Cyc_Tcenv_enter_try(r,te),s0),_tmp4C,1);{struct
_RegionHandle _tmp9E=_new_region("r2");struct _RegionHandle*r2=& _tmp9E;
_push_region(r2);{struct Cyc_Tcenv_Tenv*_tmp9F=Cyc_Tcenv_set_in_switch(r2,te);
_tmp9F=Cyc_Tcenv_clear_fallthru(r2,_tmp9F);{struct Cyc_List_List*_tmpA0=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp4D);for(0;_tmpA0 != 0;
_tmpA0=_tmpA0->tl){struct Cyc_Absyn_Pat*_tmpA1=((struct Cyc_Absyn_Switch_clause*)
_tmpA0->hd)->pattern;void*_tmpA2=Cyc_Absyn_exn_typ();struct Cyc_Tcpat_TcPatResult
_tmpA3=Cyc_Tcpat_tcPat(_tmp9F,_tmpA1,& _tmpA2);struct Cyc_List_List*_tmpA4=_tmpA3.tvars_and_bounds_opt
== 0?0:(*_tmpA3.tvars_and_bounds_opt).f1;if(!Cyc_Tcutil_unify((void*)_check_null(
_tmpA1->topt),_tmpA2)){const char*_tmp1C5;void*_tmp1C4[1];struct Cyc_String_pa_PrintArg_struct
_tmp1C3;(_tmp1C3.tag=0,((_tmp1C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmpA1->topt))),((_tmp1C4[0]=& _tmp1C3,
Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)_tmpA0->hd)->loc,((_tmp1C5="expected datatype exn but found %s",
_tag_dyneither(_tmp1C5,sizeof(char),35))),_tag_dyneither(_tmp1C4,sizeof(void*),1)))))));}
else{Cyc_Tcpat_check_pat_regions(_tmp9F,_tmpA1);}{struct Cyc_Core_Opt*_tmp1C6;((
struct Cyc_Absyn_Switch_clause*)_tmpA0->hd)->pat_vars=((_tmp1C6=_cycalloc(sizeof(*
_tmp1C6)),((_tmp1C6->v=_tmpA3.patvars,_tmp1C6))));}Cyc_Tcstmt_pattern_synth(((
struct Cyc_Absyn_Switch_clause*)_tmpA0->hd)->loc,_tmp9F,_tmpA3,((struct Cyc_Absyn_Switch_clause*)
_tmpA0->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmpA0->hd)->where_clause,1);
if(_tmpA3.tvars_and_bounds_opt != 0  && (*_tmpA3.tvars_and_bounds_opt).f2 != 0)
_tmp9F=Cyc_Tcenv_clear_fallthru(r2,_tmp9F);else{_tmp9F=Cyc_Tcenv_set_fallthru(r2,
_tmp9F,_tmpA4,_tmpA3.patvars,(struct Cyc_Absyn_Switch_clause*)_tmpA0->hd);}}};}
Cyc_Tcpat_check_catch_overlap(s0->loc,te,_tmp4D);_npop_handler(1);return;;
_pop_region(r2);};;_pop_region(r);};_LL26: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*
_tmp4E=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp17;if(_tmp4E->tag != 12)
goto _LL28;else{_tmp4F=_tmp4E->f1;_tmp50=_tmp4E->f2;}}_LL27: {struct
_dyneither_ptr unimp_msg_part;struct _RegionHandle _tmpA9=_new_region("decl_rgn");
struct _RegionHandle*decl_rgn=& _tmpA9;_push_region(decl_rgn);{struct Cyc_Tcenv_Tenv*
te2;if(new_block)te2=Cyc_Tcenv_new_block(decl_rgn,s0->loc,te);else{te2=Cyc_Tcenv_copy_tenv(
decl_rgn,te);}{struct Cyc_Tcenv_Tenv*_tmpAA=te2;void*_tmpAB=_tmp4F->r;struct Cyc_Absyn_Vardecl*
_tmpAD;struct Cyc_Absyn_Pat*_tmpAF;struct Cyc_Core_Opt*_tmpB0;struct Cyc_Core_Opt**
_tmpB1;struct Cyc_Absyn_Exp*_tmpB2;struct Cyc_List_List*_tmpB4;struct Cyc_Absyn_Tvar*
_tmpB6;struct Cyc_Absyn_Vardecl*_tmpB7;int _tmpB8;struct Cyc_Absyn_Exp*_tmpB9;
struct Cyc_Absyn_Exp*_tmpBB;struct Cyc_Absyn_Tvar*_tmpBC;struct Cyc_Absyn_Vardecl*
_tmpBD;struct Cyc_Absyn_Fndecl*_tmpBF;struct _dyneither_ptr*_tmpC1;struct Cyc_List_List*
_tmpC2;struct _tuple0*_tmpC4;struct Cyc_List_List*_tmpC5;_LL39: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*
_tmpAC=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpAB;if(_tmpAC->tag != 0)
goto _LL3B;else{_tmpAD=_tmpAC->f1;}}_LL3A: {struct Cyc_Absyn_Vardecl _tmpCF;enum 
Cyc_Absyn_Scope _tmpD0;struct _tuple0*_tmpD1;struct _tuple0 _tmpD2;union Cyc_Absyn_Nmspace
_tmpD3;struct _dyneither_ptr*_tmpD4;struct Cyc_Absyn_Tqual _tmpD5;void*_tmpD6;
struct Cyc_Absyn_Exp*_tmpD7;void*_tmpD8;void**_tmpD9;struct Cyc_List_List*_tmpDA;
struct Cyc_Absyn_Vardecl*_tmpCE=_tmpAD;_tmpCF=*_tmpCE;_tmpD0=_tmpCF.sc;_tmpD1=
_tmpCF.name;_tmpD2=*_tmpD1;_tmpD3=_tmpD2.f1;_tmpD4=_tmpD2.f2;_tmpD5=_tmpCF.tq;
_tmpD6=_tmpCF.type;_tmpD7=_tmpCF.initializer;_tmpD8=_tmpCF.rgn;_tmpD9=(void**)&(*
_tmpCE).rgn;_tmpDA=_tmpCF.attributes;{void*_tmpDB=Cyc_Tcenv_curr_rgn(_tmpAA);int
is_local;switch(_tmpD0){case Cyc_Absyn_Static: _LL59: goto _LL5A;case Cyc_Absyn_Extern:
_LL5A: goto _LL5B;case Cyc_Absyn_ExternC: _LL5B: is_local=0;break;case Cyc_Absyn_Abstract:
_LL5C:{const char*_tmp1C9;void*_tmp1C8;(_tmp1C8=0,Cyc_Tcutil_terr(_tmp4F->loc,((
_tmp1C9="bad abstract scope for local variable",_tag_dyneither(_tmp1C9,sizeof(
char),38))),_tag_dyneither(_tmp1C8,sizeof(void*),0)));}goto _LL5D;case Cyc_Absyn_Register:
_LL5D: goto _LL5E;case Cyc_Absyn_Public: _LL5E: is_local=1;break;}*_tmpD9=(void*)(
is_local?_tmpDB:(void*)& Cyc_Absyn_HeapRgn_val);{union Cyc_Absyn_Nmspace _tmpDE=
_tmpD3;int _tmpDF;struct Cyc_List_List*_tmpE0;struct Cyc_List_List*_tmpE1;struct Cyc_List_List*
_tmpE2;_LL61: if((_tmpDE.Loc_n).tag != 4)goto _LL63;_tmpDF=(int)(_tmpDE.Loc_n).val;
_LL62: goto _LL60;_LL63: if((_tmpDE.Rel_n).tag != 1)goto _LL65;_tmpE0=(struct Cyc_List_List*)(
_tmpDE.Rel_n).val;if(_tmpE0 != 0)goto _LL65;_LL64:(*_tmpAD->name).f1=Cyc_Absyn_Loc_n;
goto _LL60;_LL65: if((_tmpDE.C_n).tag != 3)goto _LL67;_tmpE1=(struct Cyc_List_List*)(
_tmpDE.C_n).val;_LL66: {const char*_tmp1CC;void*_tmp1CB;(int)_throw(((_tmp1CB=0,((
void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1CC="tcstmt: C_n declaration",_tag_dyneither(_tmp1CC,sizeof(char),24))),
_tag_dyneither(_tmp1CB,sizeof(void*),0)))));}_LL67: if((_tmpDE.Abs_n).tag != 2)
goto _LL69;_tmpE2=(struct Cyc_List_List*)(_tmpDE.Abs_n).val;_LL68: {const char*
_tmp1CF;void*_tmp1CE;(int)_throw(((_tmp1CE=0,((void*(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1CF="tcstmt: Abs_n declaration",
_tag_dyneither(_tmp1CF,sizeof(char),26))),_tag_dyneither(_tmp1CE,sizeof(void*),0)))));}
_LL69:;_LL6A:{const char*_tmp1D2;void*_tmp1D1;(_tmp1D1=0,Cyc_Tcutil_terr(_tmp4F->loc,((
_tmp1D2="cannot declare a qualified local variable",_tag_dyneither(_tmp1D2,
sizeof(char),42))),_tag_dyneither(_tmp1D1,sizeof(void*),0)));}goto _LL60;_LL60:;}{
void*_tmpE9=Cyc_Tcutil_compress(_tmpD6);struct Cyc_Absyn_ArrayInfo _tmpEB;void*
_tmpEC;struct Cyc_Absyn_Tqual _tmpED;struct Cyc_Absyn_Exp*_tmpEE;union Cyc_Absyn_Constraint*
_tmpEF;unsigned int _tmpF0;_LL6C:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmpEA=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE9;if(_tmpEA->tag != 8)
goto _LL6E;else{_tmpEB=_tmpEA->f1;_tmpEC=_tmpEB.elt_type;_tmpED=_tmpEB.tq;_tmpEE=
_tmpEB.num_elts;if(_tmpEE != 0)goto _LL6E;_tmpEF=_tmpEB.zero_term;_tmpF0=_tmpEB.zt_loc;}}
if(!(_tmpAD->initializer != 0))goto _LL6E;_LL6D:{void*_tmpF1=((struct Cyc_Absyn_Exp*)
_check_null(_tmpAD->initializer))->r;union Cyc_Absyn_Cnst _tmpF3;struct
_dyneither_ptr _tmpF4;union Cyc_Absyn_Cnst _tmpF6;struct _dyneither_ptr _tmpF7;struct
Cyc_Absyn_Exp*_tmpF9;struct Cyc_List_List*_tmpFB;struct Cyc_List_List*_tmpFD;_LL71: {
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpF2=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmpF1;if(_tmpF2->tag != 0)goto _LL73;else{_tmpF3=_tmpF2->f1;if((_tmpF3.String_c).tag
!= 8)goto _LL73;_tmpF4=(struct _dyneither_ptr)(_tmpF3.String_c).val;}}_LL72: _tmpD6=(
_tmpAD->type=Cyc_Absyn_array_typ(_tmpEC,_tmpED,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
_get_dyneither_size(_tmpF4,sizeof(char)),0),_tmpEF,_tmpF0));goto _LL70;_LL73: {
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpF5=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmpF1;if(_tmpF5->tag != 0)goto _LL75;else{_tmpF6=_tmpF5->f1;if((_tmpF6.Wstring_c).tag
!= 9)goto _LL75;_tmpF7=(struct _dyneither_ptr)(_tmpF6.Wstring_c).val;}}_LL74:
_tmpD6=(_tmpAD->type=Cyc_Absyn_array_typ(_tmpEC,_tmpED,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
1,0),_tmpEF,_tmpF0));goto _LL70;_LL75: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*
_tmpF8=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpF1;if(_tmpF8->tag
!= 26)goto _LL77;else{_tmpF9=_tmpF8->f2;}}_LL76: _tmpD6=(_tmpAD->type=Cyc_Absyn_array_typ(
_tmpEC,_tmpED,(struct Cyc_Absyn_Exp*)_tmpF9,_tmpEF,_tmpF0));goto _LL70;_LL77: {
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpFA=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)
_tmpF1;if(_tmpFA->tag != 34)goto _LL79;else{_tmpFB=_tmpFA->f2;}}_LL78: _tmpFD=
_tmpFB;goto _LL7A;_LL79: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpFC=(
struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpF1;if(_tmpFC->tag != 25)goto
_LL7B;else{_tmpFD=_tmpFC->f1;}}_LL7A: _tmpD6=(_tmpAD->type=Cyc_Absyn_array_typ(
_tmpEC,_tmpED,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpFD),0),_tmpEF,_tmpF0));goto _LL70;
_LL7B:;_LL7C: goto _LL70;_LL70:;}goto _LL6B;_LL6E:;_LL6F: goto _LL6B;_LL6B:;}{struct
Cyc_List_List*_tmpFE=!is_local?0: Cyc_Tcenv_lookup_type_vars(_tmpAA);int _tmpFF=!
is_local?0: 1;Cyc_Tcutil_check_type(s0->loc,_tmpAA,_tmpFE,& Cyc_Tcutil_tmk,_tmpFF,
_tmpD6);(_tmpAD->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(s0->loc,(
_tmpAD->tq).print_const,_tmpD6);{struct Cyc_Tcenv_Tenv*_tmp100=Cyc_Tcenv_add_local_var(
decl_rgn,_tmp50->loc,_tmpAA,_tmpAD);if(_tmpD0 == Cyc_Absyn_Extern  || _tmpD0 == Cyc_Absyn_ExternC){
const char*_tmp1D5;void*_tmp1D4;(_tmp1D4=0,Cyc_Tcutil_terr(_tmp4F->loc,((_tmp1D5="local extern declarations not yet supported",
_tag_dyneither(_tmp1D5,sizeof(char),44))),_tag_dyneither(_tmp1D4,sizeof(void*),0)));}
if(_tmpD7 != 0){Cyc_Tcexp_tcExpInitializer(_tmp100,(void**)& _tmpD6,(struct Cyc_Absyn_Exp*)
_tmpD7);if(!is_local  && !Cyc_Tcutil_is_const_exp(_tmpAA,(struct Cyc_Absyn_Exp*)
_tmpD7)){const char*_tmp1D8;void*_tmp1D7;(_tmp1D7=0,Cyc_Tcutil_terr(_tmp4F->loc,((
_tmp1D8="initializer needs to be a constant expression",_tag_dyneither(_tmp1D8,
sizeof(char),46))),_tag_dyneither(_tmp1D7,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_assign(
_tmp100,(struct Cyc_Absyn_Exp*)_tmpD7,_tmpD6)){struct _dyneither_ptr _tmp105=*
_tmpD4;struct _dyneither_ptr _tmp106=Cyc_Absynpp_typ2string(_tmpD6);struct
_dyneither_ptr _tmp107=Cyc_Absynpp_typ2string((void*)_check_null(_tmpD7->topt));
if(((_get_dyneither_size(_tmp105,sizeof(char))+ _get_dyneither_size(_tmp106,
sizeof(char)))+ _get_dyneither_size(_tmp107,sizeof(char)))+ 50 < 80){const char*
_tmp1DE;void*_tmp1DD[3];struct Cyc_String_pa_PrintArg_struct _tmp1DC;struct Cyc_String_pa_PrintArg_struct
_tmp1DB;struct Cyc_String_pa_PrintArg_struct _tmp1DA;(_tmp1DA.tag=0,((_tmp1DA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp107),((_tmp1DB.tag=0,((_tmp1DB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp106),((_tmp1DC.tag=0,((_tmp1DC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp105),((_tmp1DD[0]=& _tmp1DC,((
_tmp1DD[1]=& _tmp1DB,((_tmp1DD[2]=& _tmp1DA,Cyc_Tcutil_terr(_tmp4F->loc,((_tmp1DE="%s declared with type %s but initialized with type %s.",
_tag_dyneither(_tmp1DE,sizeof(char),55))),_tag_dyneither(_tmp1DD,sizeof(void*),3)))))))))))))))))));}
else{if((_get_dyneither_size(_tmp105,sizeof(char))+ _get_dyneither_size(_tmp106,
sizeof(char)))+ 25 < 80  && _get_dyneither_size(_tmp107,sizeof(char))+ 25 < 80){
const char*_tmp1E4;void*_tmp1E3[3];struct Cyc_String_pa_PrintArg_struct _tmp1E2;
struct Cyc_String_pa_PrintArg_struct _tmp1E1;struct Cyc_String_pa_PrintArg_struct
_tmp1E0;(_tmp1E0.tag=0,((_tmp1E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp107),((_tmp1E1.tag=0,((_tmp1E1.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp106),((_tmp1E2.tag=0,((_tmp1E2.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp105),((_tmp1E3[0]=& _tmp1E2,((_tmp1E3[1]=& _tmp1E1,((
_tmp1E3[2]=& _tmp1E0,Cyc_Tcutil_terr(_tmp4F->loc,((_tmp1E4="%s declared with type %s\n but initialized with type %s.",
_tag_dyneither(_tmp1E4,sizeof(char),56))),_tag_dyneither(_tmp1E3,sizeof(void*),3)))))))))))))))))));}
else{const char*_tmp1EA;void*_tmp1E9[3];struct Cyc_String_pa_PrintArg_struct
_tmp1E8;struct Cyc_String_pa_PrintArg_struct _tmp1E7;struct Cyc_String_pa_PrintArg_struct
_tmp1E6;(_tmp1E6.tag=0,((_tmp1E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp107),((_tmp1E7.tag=0,((_tmp1E7.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp106),((_tmp1E8.tag=0,((_tmp1E8.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp105),((_tmp1E9[0]=& _tmp1E8,((_tmp1E9[1]=& _tmp1E7,((
_tmp1E9[2]=& _tmp1E6,Cyc_Tcutil_terr(_tmp4F->loc,((_tmp1EA="%s declared with type \n%s\n but initialized with type \n%s.",
_tag_dyneither(_tmp1EA,sizeof(char),58))),_tag_dyneither(_tmp1E9,sizeof(void*),3)))))))))))))))))));}}
Cyc_Tcutil_unify(_tmpD6,(void*)_check_null(_tmpD7->topt));Cyc_Tcutil_explain_failure();}}
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmp100,s0),_tmp50,0);
_npop_handler(0);return;};};};}_LL3B: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*
_tmpAE=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpAB;if(_tmpAE->tag != 2)
goto _LL3D;else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAE->f2;_tmpB1=(struct Cyc_Core_Opt**)&
_tmpAE->f2;_tmpB2=_tmpAE->f3;}}_LL3C: Cyc_Tcexp_tcExpInitializer(_tmpAA,0,_tmpB2);{
void*pat_type=(void*)_check_null(_tmpB2->topt);struct Cyc_Tcpat_TcPatResult
_tmp117=Cyc_Tcpat_tcPat(_tmpAA,_tmpAF,& pat_type);{struct Cyc_Core_Opt*_tmp1EB;*
_tmpB1=((_tmp1EB=_cycalloc(sizeof(*_tmp1EB)),((_tmp1EB->v=_tmp117.patvars,
_tmp1EB))));}if(!Cyc_Tcutil_unify((void*)_check_null(_tmpAF->topt),(void*)
_check_null(_tmpB2->topt)) && !Cyc_Tcutil_coerce_assign(_tmpAA,_tmpB2,(void*)
_check_null(_tmpAF->topt))){{const char*_tmp1F0;void*_tmp1EF[2];struct Cyc_String_pa_PrintArg_struct
_tmp1EE;struct Cyc_String_pa_PrintArg_struct _tmp1ED;(_tmp1ED.tag=0,((_tmp1ED.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)
_check_null(_tmpB2->topt))),((_tmp1EE.tag=0,((_tmp1EE.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpAF->topt))),((
_tmp1EF[0]=& _tmp1EE,((_tmp1EF[1]=& _tmp1ED,Cyc_Tcutil_terr(_tmp4F->loc,((_tmp1F0="pattern type %s does not match definition type %s",
_tag_dyneither(_tmp1F0,sizeof(char),50))),_tag_dyneither(_tmp1EF,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)_check_null(_tmpAF->topt),(void*)_check_null(_tmpB2->topt));
Cyc_Tcutil_explain_failure();}else{Cyc_Tcpat_check_pat_regions(_tmpAA,_tmpAF);}
Cyc_Tcpat_check_let_pat_exhaustive(_tmpAF->loc,_tmpAA,_tmpAF);Cyc_Tcstmt_pattern_synth(
s0->loc,_tmpAA,_tmp117,_tmp50,0,0);_npop_handler(0);return;};_LL3D: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*
_tmpB3=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpAB;if(_tmpB3->tag != 3)
goto _LL3F;else{_tmpB4=_tmpB3->f1;}}_LL3E: {void*_tmp11D=Cyc_Tcenv_curr_rgn(
_tmpAA);struct Cyc_Tcenv_Tenv*_tmp11E=_tmpAA;for(0;_tmpB4 != 0;_tmpB4=_tmpB4->tl){
struct Cyc_Absyn_Vardecl*_tmp11F=(struct Cyc_Absyn_Vardecl*)_tmpB4->hd;struct Cyc_Absyn_Vardecl
_tmp121;struct _tuple0*_tmp122;struct _tuple0 _tmp123;union Cyc_Absyn_Nmspace _tmp124;
void*_tmp125;void*_tmp126;void**_tmp127;struct Cyc_Absyn_Vardecl*_tmp120=_tmp11F;
_tmp121=*_tmp120;_tmp122=_tmp121.name;_tmp123=*_tmp122;_tmp124=_tmp123.f1;
_tmp125=_tmp121.type;_tmp126=_tmp121.rgn;_tmp127=(void**)&(*_tmp120).rgn;*
_tmp127=(void*)_tmp11D;{union Cyc_Absyn_Nmspace _tmp128=_tmp124;int _tmp129;struct
Cyc_List_List*_tmp12A;struct Cyc_List_List*_tmp12B;_LL7E: if((_tmp128.Loc_n).tag != 
4)goto _LL80;_tmp129=(int)(_tmp128.Loc_n).val;_LL7F: goto _LL7D;_LL80: if((_tmp128.Rel_n).tag
!= 1)goto _LL82;_tmp12A=(struct Cyc_List_List*)(_tmp128.Rel_n).val;if(_tmp12A != 0)
goto _LL82;_LL81:(*_tmp11F->name).f1=Cyc_Absyn_Loc_n;goto _LL7D;_LL82: if((_tmp128.Abs_n).tag
!= 2)goto _LL84;_tmp12B=(struct Cyc_List_List*)(_tmp128.Abs_n).val;_LL83: {const
char*_tmp1F3;void*_tmp1F2;(int)_throw(((_tmp1F2=0,((void*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1F3="tcstmt: Abs_n declaration",
_tag_dyneither(_tmp1F3,sizeof(char),26))),_tag_dyneither(_tmp1F2,sizeof(void*),0)))));}
_LL84:;_LL85:{const char*_tmp1F6;void*_tmp1F5;(_tmp1F5=0,Cyc_Tcutil_terr(_tmp4F->loc,((
_tmp1F6="cannot declare a qualified local variable",_tag_dyneither(_tmp1F6,
sizeof(char),42))),_tag_dyneither(_tmp1F5,sizeof(void*),0)));}goto _LL7D;_LL7D:;}
Cyc_Tcutil_check_type(s0->loc,_tmp11E,Cyc_Tcenv_lookup_type_vars(_tmp11E),& Cyc_Tcutil_tmk,
1,_tmp125);_tmp11E=Cyc_Tcenv_add_local_var(decl_rgn,_tmp50->loc,_tmp11E,_tmp11F);}
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmp11E,s0),_tmp50,0);
_npop_handler(0);return;}_LL3F: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*
_tmpB5=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpAB;if(_tmpB5->tag != 
4)goto _LL41;else{_tmpB6=_tmpB5->f1;_tmpB7=_tmpB5->f2;_tmpB8=_tmpB5->f3;_tmpB9=
_tmpB5->f4;}}_LL40:{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp1F9;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*
_tmp1F8;_tmp50->r=(Cyc_Absyn_new_stmt((void*)((_tmp1F8=_cycalloc(sizeof(*_tmp1F8)),((
_tmp1F8[0]=((_tmp1F9.tag=2,((_tmp1F9.f1=Cyc_Absyn_new_stmt(_tmp50->r,_tmp50->loc),((
_tmp1F9.f2=Cyc_Absyn_skip_stmt(_tmp50->loc),_tmp1F9)))))),_tmp1F8)))),_tmp50->loc))->r;}{
struct Cyc_Tcenv_Tenv*_tmp132=_tmpAA;struct Cyc_Absyn_Vardecl _tmp134;void*_tmp135;
void**_tmp136;void*_tmp137;void**_tmp138;struct Cyc_Absyn_Vardecl*_tmp133=_tmpB7;
_tmp134=*_tmp133;_tmp135=_tmp134.type;_tmp136=(void**)&(*_tmp133).type;_tmp137=
_tmp134.rgn;_tmp138=(void**)&(*_tmp133).rgn;{void*_tmp139=Cyc_Tcenv_curr_rgn(
_tmpAA);*_tmp138=(void*)_tmp139;{void*rgn_typ;if(_tmpB9 != 0){struct Cyc_Absyn_Exp*
open_exp=(struct Cyc_Absyn_Exp*)_tmpB9;struct Cyc_List_List*_tmp13A=Cyc_Tcenv_lookup_type_vars(
_tmpAA);{struct Cyc_Core_Opt*_tmp1FA;rgn_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rko,((_tmp1FA=_cycalloc(sizeof(*_tmp1FA)),((_tmp1FA->v=_tmp13A,
_tmp1FA)))));}{struct Cyc_Core_Opt*_tmp1FB;void*rgn2_typ=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,((_tmp1FB=_cycalloc(sizeof(*_tmp1FB)),((
_tmp1FB->v=_tmp13A,_tmp1FB)))));struct Cyc_Absyn_DynRgnType_Absyn_Type_struct
_tmp1FE;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1FD;void*expected=(void*)((
_tmp1FD=_cycalloc(sizeof(*_tmp1FD)),((_tmp1FD[0]=((_tmp1FE.tag=16,((_tmp1FE.f1=(
void*)rgn_typ,((_tmp1FE.f2=(void*)rgn2_typ,_tmp1FE)))))),_tmp1FD))));{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct
_tmp201;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp200;*_tmp136=(void*)((
_tmp200=_cycalloc(sizeof(*_tmp200)),((_tmp200[0]=((_tmp201.tag=15,((_tmp201.f1=(
void*)rgn_typ,_tmp201)))),_tmp200))));}if(!Cyc_Tcutil_unify(Cyc_Tcexp_tcExp(
_tmpAA,(void**)& expected,open_exp),expected)){const char*_tmp206;void*_tmp205[2];
struct Cyc_String_pa_PrintArg_struct _tmp204;struct Cyc_String_pa_PrintArg_struct
_tmp203;(_tmp203.tag=0,((_tmp203.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(open_exp->topt))),((_tmp204.tag=0,((
_tmp204.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
expected)),((_tmp205[0]=& _tmp204,((_tmp205[1]=& _tmp203,Cyc_Tcutil_terr(open_exp->loc,((
_tmp206="expecting %s but found %s",_tag_dyneither(_tmp206,sizeof(char),26))),
_tag_dyneither(_tmp205,sizeof(void*),2)))))))))))));}Cyc_Tcenv_check_rgn_accessible(
_tmpAA,open_exp->loc,rgn2_typ);_tmp132=Cyc_Tcenv_add_region(decl_rgn,_tmp132,
rgn_typ,0,1);};}else{{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp209;struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp208;rgn_typ=(void*)((_tmp208=_cycalloc(sizeof(*_tmp208)),((_tmp208[0]=((
_tmp209.tag=2,((_tmp209.f1=_tmpB6,_tmp209)))),_tmp208))));}{struct Cyc_List_List*
_tmp20A;_tmp132=Cyc_Tcenv_add_type_vars(decl_rgn,s0->loc,_tmp132,((_tmp20A=
_cycalloc(sizeof(*_tmp20A)),((_tmp20A->hd=_tmpB6,((_tmp20A->tl=0,_tmp20A)))))));}
_tmp132=Cyc_Tcenv_add_region(decl_rgn,_tmp132,rgn_typ,_tmpB8,1);}Cyc_Tcutil_check_type(
s0->loc,_tmpAA,Cyc_Tcenv_lookup_type_vars(_tmp132),& Cyc_Tcutil_bk,1,*_tmp136);{
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp20D;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*
_tmp20C;if(!Cyc_Tcutil_unify(*_tmp136,(void*)((_tmp20C=_cycalloc(sizeof(*_tmp20C)),((
_tmp20C[0]=((_tmp20D.tag=15,((_tmp20D.f1=(void*)rgn_typ,_tmp20D)))),_tmp20C)))))){
const char*_tmp210;void*_tmp20F;(_tmp20F=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp210="region stmt: type of region handle is wrong!",
_tag_dyneither(_tmp210,sizeof(char),45))),_tag_dyneither(_tmp20F,sizeof(void*),0)));}}
if(!Cyc_Absyn_no_regions)_tmp132=Cyc_Tcenv_enter_try(decl_rgn,_tmp132);Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_encloser(decl_rgn,Cyc_Tcenv_add_local_var(decl_rgn,_tmp50->loc,
_tmp132,_tmpB7),s0),_tmp50,0);_npop_handler(0);return;};};};_LL41: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*
_tmpBA=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmpAB;if(_tmpBA->tag != 5)
goto _LL43;else{_tmpBB=_tmpBA->f1;_tmpBC=_tmpBA->f2;_tmpBD=_tmpBA->f3;}}_LL42: {
struct _RegionHandle _tmp14C=_new_region("r");struct _RegionHandle*r=& _tmp14C;
_push_region(r);{struct Cyc_Absyn_Tvar*_tmp211[1];struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(
r,s0->loc,_tmpAA,((_tmp211[0]=_tmpBC,((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp211,
sizeof(struct Cyc_Absyn_Tvar*),1)))));{struct Cyc_Absyn_VarType_Absyn_Type_struct
_tmp214;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp213;te2=Cyc_Tcenv_add_region(
r,te2,(void*)((_tmp213=_cycalloc(sizeof(*_tmp213)),((_tmp213[0]=((_tmp214.tag=2,((
_tmp214.f1=_tmpBC,_tmp214)))),_tmp213)))),0,1);}Cyc_Tcexp_tcExpInitializer(te2,0,
_tmpBB);if(!Cyc_Tcutil_is_pointer_type((void*)_check_null(_tmpBB->topt))){{const
char*_tmp218;void*_tmp217[1];struct Cyc_String_pa_PrintArg_struct _tmp216;(_tmp216.tag=
0,((_tmp216.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)_check_null(_tmpBB->topt))),((_tmp217[0]=& _tmp216,Cyc_Tcutil_terr(_tmpBB->loc,((
_tmp218="%s is not a pointer type",_tag_dyneither(_tmp218,sizeof(char),25))),
_tag_dyneither(_tmp217,sizeof(void*),1)))))));}_npop_handler(1);return;}_tmpBD->type=
Cyc_Tcutil_copy_type((void*)_check_null(_tmpBB->topt));Cyc_Tcutil_check_type(
_tmpBB->loc,_tmpAA,Cyc_Tcenv_lookup_type_vars(te2),& Cyc_Tcutil_mk,1,_tmpBD->type);
_tmpBD->rgn=(void*)Cyc_Tcenv_curr_rgn(te2);te2=Cyc_Tcenv_add_local_var(r,s0->loc,
te2,_tmpBD);te2=Cyc_Tcenv_set_encloser(r,te2,_tmp50);{struct Cyc_List_List*
_tmp152=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tcstmt_tcStmt(te2,_tmp50,0);};}Cyc_NewControlFlow_set_encloser(
s0,Cyc_Tcenv_get_encloser(_tmpAA));_npop_handler(1);return;;_pop_region(r);}
_LL43: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpBE=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)
_tmpAB;if(_tmpBE->tag != 1)goto _LL45;else{_tmpBF=_tmpBE->f1;}}_LL44: {void*
_tmp154=Cyc_Tcenv_curr_rgn(_tmpAA);if(_tmpBF->sc != Cyc_Absyn_Public){const char*
_tmp21B;void*_tmp21A;(_tmp21A=0,Cyc_Tcutil_terr(_tmp4F->loc,((_tmp21B="bad storage class for inner function",
_tag_dyneither(_tmp21B,sizeof(char),37))),_tag_dyneither(_tmp21A,sizeof(void*),0)));}{
union Cyc_Absyn_Nmspace _tmp157=(*_tmpBF->name).f1;struct Cyc_List_List*_tmp158;
struct Cyc_List_List*_tmp159;_LL87: if((_tmp157.Rel_n).tag != 1)goto _LL89;_tmp158=(
struct Cyc_List_List*)(_tmp157.Rel_n).val;if(_tmp158 != 0)goto _LL89;_LL88: goto
_LL86;_LL89: if((_tmp157.Abs_n).tag != 2)goto _LL8B;_tmp159=(struct Cyc_List_List*)(
_tmp157.Abs_n).val;_LL8A: {const char*_tmp21E;void*_tmp21D;(_tmp21D=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp21E="tc: Abs_n in tcStmt var decl",
_tag_dyneither(_tmp21E,sizeof(char),29))),_tag_dyneither(_tmp21D,sizeof(void*),0)));}
_LL8B:;_LL8C: {const char*_tmp221;void*_tmp220;(_tmp220=0,Cyc_Tcutil_terr(_tmp4F->loc,((
_tmp221="explicit namespace not allowed on inner function declaration",
_tag_dyneither(_tmp221,sizeof(char),61))),_tag_dyneither(_tmp220,sizeof(void*),0)));}
_LL86:;}(*_tmpBF->name).f1=Cyc_Absyn_Loc_n;Cyc_Tcutil_check_fndecl_valid_type(
_tmp4F->loc,_tmpAA,_tmpBF);{void*t=Cyc_Tcutil_fndecl2typ(_tmpBF);_tmpBF->attributes=
Cyc_Tcutil_transfer_fn_type_atts(t,_tmpBF->attributes);{struct Cyc_List_List*atts=
_tmpBF->attributes;for(0;(unsigned int)atts;atts=atts->tl){void*_tmp15E=(void*)
atts->hd;_LL8E: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp15F=(
struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp15E;if(_tmp15F->tag != 7)
goto _LL90;}_LL8F: goto _LL91;_LL90: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*
_tmp160=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp15E;if(_tmp160->tag
!= 6)goto _LL92;}_LL91:{const char*_tmp225;void*_tmp224[1];struct Cyc_String_pa_PrintArg_struct
_tmp223;(_tmp223.tag=0,((_tmp223.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp224[0]=& _tmp223,Cyc_Tcutil_terr(
_tmp4F->loc,((_tmp225="bad attribute %s for function",_tag_dyneither(_tmp225,
sizeof(char),30))),_tag_dyneither(_tmp224,sizeof(void*),1)))))));}goto _LL8D;
_LL92:;_LL93: goto _LL8D;_LL8D:;}}{struct Cyc_Absyn_Vardecl*_tmp226;struct Cyc_Absyn_Vardecl*
vd=(_tmp226=_cycalloc(sizeof(*_tmp226)),((_tmp226->sc=_tmpBF->sc,((_tmp226->name=
_tmpBF->name,((_tmp226->tq=Cyc_Absyn_const_tqual(0),((_tmp226->type=Cyc_Absyn_at_typ(
t,_tmp154,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),((_tmp226->initializer=
0,((_tmp226->rgn=(void*)_tmp154,((_tmp226->attributes=0,((_tmp226->escapes=0,
_tmp226)))))))))))))))));_tmpBF->fn_vardecl=(struct Cyc_Absyn_Vardecl*)vd;_tmpAA=
Cyc_Tcenv_add_local_var(decl_rgn,_tmp4F->loc,_tmpAA,vd);{struct Cyc_Tcenv_Fenv*
old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(_tmpAA->le);_tmpAA->le=(struct Cyc_Tcenv_Fenv*)
Cyc_Tcenv_nested_fenv(_tmp4F->loc,old_fenv,_tmpBF);Cyc_Tcstmt_tcStmt(_tmpAA,
_tmpBF->body,0);Cyc_Tcenv_check_delayed_effects(_tmpAA);Cyc_Tcenv_check_delayed_constraints(
_tmpAA);if(!Cyc_Tcenv_all_labels_resolved(_tmpAA)){const char*_tmp229;void*
_tmp228;(_tmp228=0,Cyc_Tcutil_terr(_tmp4F->loc,((_tmp229="function has goto statements to undefined labels",
_tag_dyneither(_tmp229,sizeof(char),49))),_tag_dyneither(_tmp228,sizeof(void*),0)));}
_tmpAA->le=(struct Cyc_Tcenv_Fenv*)old_fenv;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
decl_rgn,_tmpAA,s0),_tmp50,0);_npop_handler(0);return;};};};}_LL45: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*
_tmpC0=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpAB;if(_tmpC0->tag
!= 10)goto _LL47;else{_tmpC1=_tmpC0->f1;_tmpC2=_tmpC0->f2;}}_LL46:{const char*
_tmp22A;unimp_msg_part=((_tmp22A="namespace",_tag_dyneither(_tmp22A,sizeof(char),
10)));}goto _LL38;_LL47: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpC3=(
struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpAB;if(_tmpC3->tag != 11)goto
_LL49;else{_tmpC4=_tmpC3->f1;_tmpC5=_tmpC3->f2;}}_LL48:{const char*_tmp22B;
unimp_msg_part=((_tmp22B="using",_tag_dyneither(_tmp22B,sizeof(char),6)));}goto
_LL38;_LL49: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpC6=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)
_tmpAB;if(_tmpC6->tag != 6)goto _LL4B;}_LL4A:{const char*_tmp22C;unimp_msg_part=((
_tmp22C="type",_tag_dyneither(_tmp22C,sizeof(char),5)));}goto _LL38;_LL4B: {
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmpC7=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)
_tmpAB;if(_tmpC7->tag != 7)goto _LL4D;}_LL4C:{const char*_tmp22D;unimp_msg_part=((
_tmp22D="datatype",_tag_dyneither(_tmp22D,sizeof(char),9)));}goto _LL38;_LL4D: {
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmpC8=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)
_tmpAB;if(_tmpC8->tag != 8)goto _LL4F;}_LL4E:{const char*_tmp22E;unimp_msg_part=((
_tmp22E="enum",_tag_dyneither(_tmp22E,sizeof(char),5)));}goto _LL38;_LL4F: {
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmpC9=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)
_tmpAB;if(_tmpC9->tag != 9)goto _LL51;}_LL50:{const char*_tmp22F;unimp_msg_part=((
_tmp22F="typedef",_tag_dyneither(_tmp22F,sizeof(char),8)));}goto _LL38;_LL51: {
struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpCA=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)
_tmpAB;if(_tmpCA->tag != 12)goto _LL53;}_LL52:{const char*_tmp230;unimp_msg_part=((
_tmp230="extern \"C\"",_tag_dyneither(_tmp230,sizeof(char),11)));}goto _LL38;
_LL53: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpCB=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)
_tmpAB;if(_tmpCB->tag != 13)goto _LL55;}_LL54:{const char*_tmp231;unimp_msg_part=((
_tmp231="extern \"C include\"",_tag_dyneither(_tmp231,sizeof(char),19)));}goto
_LL38;_LL55: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmpCC=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)
_tmpAB;if(_tmpCC->tag != 14)goto _LL57;}_LL56:{const char*_tmp232;unimp_msg_part=((
_tmp232="__cyclone_port_on__",_tag_dyneither(_tmp232,sizeof(char),20)));}goto
_LL38;_LL57: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmpCD=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)
_tmpAB;if(_tmpCD->tag != 15)goto _LL38;}_LL58:{const char*_tmp233;unimp_msg_part=((
_tmp233="__cyclone_port_off__",_tag_dyneither(_tmp233,sizeof(char),21)));}goto
_LL38;_LL38:;};}{const char*_tmp237;void*_tmp236[1];struct Cyc_String_pa_PrintArg_struct
_tmp235;(int)_throw(((_tmp235.tag=0,((_tmp235.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)unimp_msg_part),((_tmp236[0]=& _tmp235,((void*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp237="tcStmt: nested %s declarations unimplemented",
_tag_dyneither(_tmp237,sizeof(char),45))),_tag_dyneither(_tmp236,sizeof(void*),1)))))))));};;
_pop_region(decl_rgn);}_LL28: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*
_tmp51=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp17;if(_tmp51->tag
!= 16)goto _LL7;else{_tmp52=_tmp51->f1;}}_LL29: {void*rgn_type=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te));struct
Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp23A;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*
_tmp239;void*etype=(void*)((_tmp239=_cycalloc(sizeof(*_tmp239)),((_tmp239[0]=((
_tmp23A.tag=15,((_tmp23A.f1=(void*)rgn_type,_tmp23A)))),_tmp239))));if(!Cyc_Tcutil_unify(
Cyc_Tcexp_tcExp(te,(void**)& etype,_tmp52),etype)){const char*_tmp23E;void*_tmp23D[
1];struct Cyc_String_pa_PrintArg_struct _tmp23C;(_tmp23C.tag=0,((_tmp23C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)
_check_null(_tmp52->topt))),((_tmp23D[0]=& _tmp23C,Cyc_Tcutil_terr(_tmp52->loc,((
_tmp23E="expecting region_t but found %s",_tag_dyneither(_tmp23E,sizeof(char),32))),
_tag_dyneither(_tmp23D,sizeof(void*),1)))))));}Cyc_Tcenv_check_rgn_resetable(te,
s0->loc,rgn_type);return;}_LL7:;};}
