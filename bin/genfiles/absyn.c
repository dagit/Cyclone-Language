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
_zero_arr_inplace_plus_char(char *x, int orig_i) {
  char **_zap_x = &x;
  *_zap_x = _zero_arr_plus_char(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_short(short *x, int orig_i) {
  short **_zap_x = &x;
  *_zap_x = _zero_arr_plus_short(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_int(int *x, int orig_i) {
  int **_zap_x = &x;
  *_zap_x = _zero_arr_plus_int(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_float(float *x, int orig_i) {
  float **_zap_x = &x;
  *_zap_x = _zero_arr_plus_float(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_double(double *x, int orig_i) {
  double **_zap_x = &x;
  *_zap_x = _zero_arr_plus_double(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_longdouble(long double *x, int orig_i) {
  long double **_zap_x = &x;
  *_zap_x = _zero_arr_plus_longdouble(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_voidstar(void **x, int orig_i) {
  void ***_zap_x = &x;
  *_zap_x = _zero_arr_plus_voidstar(*_zap_x,1,orig_i);
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
_zero_arr_inplace_plus_post_char(char *x, int orig_i){
  char ** _zap_x = &x;
  char * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_char(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE short *
_zero_arr_inplace_plus_post_short(short *x, int orig_i){
  short **_zap_x = &x;
  short * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_short(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE int *
_zero_arr_inplace_plus_post_int(int *x, int orig_i){
  int **_zap_x = &x;
  int * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_int(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE float *
_zero_arr_inplace_plus_post_float(float *x, int orig_i){
  float **_zap_x = &x;
  float * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_float(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE double *
_zero_arr_inplace_plus_post_double(double *x, int orig_i){
  double **_zap_x = &x;
  double * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_double(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE long double *
_zero_arr_inplace_plus_post_longdouble(long double *x, int orig_i){
  long double **_zap_x = &x;
  long double * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_longdouble(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE void **
_zero_arr_inplace_plus_post_voidstar(void **x, int orig_i){
  void ***_zap_x = &x;
  void ** _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_voidstar(_zap_res,1,orig_i);
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
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[
17];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*
tag;};int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct Cyc_Position_Segment*Cyc_Position_segment_join(
struct Cyc_Position_Segment*,struct Cyc_Position_Segment*);struct Cyc_Position_Lex_struct{
int tag;};struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{
int tag;};struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};struct _union_Nmspace_Rel_n{int tag;
struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct
_dyneither_ptr*f2;};enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract
 = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register
 = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int
real_const;struct Cyc_Position_Segment*loc;};enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz
 = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz
 = 4};enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top
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
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{
struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);union
Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,
struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU
field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;
struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int
tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**
val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;
struct _union_AggrInfoU_KnownAggr KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(
enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);union Cyc_Absyn_AggrInfoU
Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union
Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{
void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*
zero_term;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_VoidType_struct{
int tag;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;}
;struct Cyc_Absyn_DatatypeFieldType_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo
f1;};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};
struct Cyc_Absyn_IntType_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of
f2;};struct Cyc_Absyn_FloatType_struct{int tag;};struct Cyc_Absyn_DoubleType_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;struct Cyc_Absyn_ArrayInfo
f1;};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple0*f1;
struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct
Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_struct{
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};extern struct Cyc_Absyn_HeapRgn_struct
Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;extern struct Cyc_Absyn_FloatType_struct
Cyc_Absyn_FloatType_val;struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*
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
int tag;struct _dyneither_ptr f1;};extern struct Cyc_Absyn_Stdcall_att_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_struct Cyc_Absyn_Cdecl_att_val;extern struct Cyc_Absyn_Fastcall_att_struct
Cyc_Absyn_Fastcall_att_val;extern struct Cyc_Absyn_Noreturn_att_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_struct Cyc_Absyn_Const_att_val;extern struct Cyc_Absyn_Packed_att_struct
Cyc_Absyn_Packed_att_val;extern struct Cyc_Absyn_Nocommon_att_struct Cyc_Absyn_Nocommon_att_val;
extern struct Cyc_Absyn_Shared_att_struct Cyc_Absyn_Shared_att_val;extern struct Cyc_Absyn_Unused_att_struct
Cyc_Absyn_Unused_att_val;extern struct Cyc_Absyn_Weak_att_struct Cyc_Absyn_Weak_att_val;
extern struct Cyc_Absyn_Dllimport_att_struct Cyc_Absyn_Dllimport_att_val;extern
struct Cyc_Absyn_Dllexport_att_struct Cyc_Absyn_Dllexport_att_val;extern struct Cyc_Absyn_No_instrument_function_att_struct
Cyc_Absyn_No_instrument_function_att_val;extern struct Cyc_Absyn_Constructor_att_struct
Cyc_Absyn_Constructor_att_val;extern struct Cyc_Absyn_Destructor_att_struct Cyc_Absyn_Destructor_att_val;
extern struct Cyc_Absyn_No_check_memory_usage_att_struct Cyc_Absyn_No_check_memory_usage_att_val;
extern struct Cyc_Absyn_Pure_att_struct Cyc_Absyn_Pure_att_val;struct Cyc_Absyn_Carray_mod_struct{
int tag;union Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
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
_union_Cnst_String_c String_c;};extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;union
Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(
enum Cyc_Absyn_Sign,short);union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,
int);union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr);union Cyc_Absyn_Cnst
Cyc_Absyn_String_c(struct _dyneither_ptr);enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 
0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,
Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte
 = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand
 = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift
 = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{
Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec
 = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*
injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_struct{
int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{int tag;
unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion
 = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{
int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;
int fat_result;};struct Cyc_Absyn_Const_e_struct{int tag;union Cyc_Absyn_Cnst f1;};
struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{
int tag;struct _tuple0*f1;};struct Cyc_Absyn_Primop_e_struct{int tag;enum Cyc_Absyn_Primop
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_And_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_Or_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnknownCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;};struct Cyc_Absyn_Throw_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{int tag;void*f1;struct
Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};
struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple7{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple7*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*
f3;};struct Cyc_Absyn_Malloc_e_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};
struct Cyc_Absyn_Swap_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{int tag;struct Cyc_Core_Opt*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Absyn_Tagcheck_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{
struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*loc;void*annot;};
struct Cyc_Absyn_Skip_s_struct{int tag;};struct Cyc_Absyn_Exp_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple8{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple8 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple8 f2;struct _tuple8 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple8 f2;
};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};extern
struct Cyc_Absyn_Skip_s_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;
struct Cyc_Position_Segment*loc;struct Cyc_List_List*non_local_preds;int try_depth;
void*annot;};struct Cyc_Absyn_Wild_p_struct{int tag;};struct Cyc_Absyn_Var_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{
int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;
struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo*
f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct
Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_struct{int tag;};struct Cyc_Absyn_Int_p_struct{
int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_struct{int tag;char f1;
};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Enum_p_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{
int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_Wild_p_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*
r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct
Cyc_Absyn_Unresolved_b_struct{int tag;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};extern struct Cyc_Absyn_Unresolved_b_struct Cyc_Absyn_Unresolved_b_val;
struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple0*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual
tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*defn;
struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*
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
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};extern struct Cyc_Absyn_Porton_d_struct
Cyc_Absyn_Porton_d_val;extern struct Cyc_Absyn_Portoff_d_struct Cyc_Absyn_Portoff_d_val;
struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};char Cyc_Absyn_EmptyAnnot[11]="EmptyAnnot";struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};extern struct Cyc_Absyn_EmptyAnnot_struct Cyc_Absyn_EmptyAnnot_val;int
Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*);int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*
x);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();union Cyc_Absyn_Constraint*
Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_val(
union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*
x);void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);extern union
Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(void*);struct Cyc_Absyn_Kind*
Cyc_Absyn_force_kb(void*kb);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct
Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);void*Cyc_Absyn_int_typ(
enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);extern void*Cyc_Absyn_char_typ;extern
void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_schar_typ;
extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;
extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_double_typ(
int);extern void*Cyc_Absyn_empty_effect;extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;void*Cyc_Absyn_exn_typ();
extern struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar;extern struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar;
void*Cyc_Absyn_string_typ(void*rgn);void*Cyc_Absyn_const_string_typ(void*rgn);
void*Cyc_Absyn_file_typ();extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;
extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct
Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_atb_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union
Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_cstar_typ(void*t,struct
Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(
struct _dyneither_ptr*name);void*Cyc_Absyn_strctq(struct _tuple0*name);void*Cyc_Absyn_unionq_typ(
struct _tuple0*name);void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name);void*Cyc_Absyn_array_typ(
void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*
zero_term,struct Cyc_Position_Segment*ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct
Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*
es,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct
Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct
Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,
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
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(
struct _dyneither_ptr*lab,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(
void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*
init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,
struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(
struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int
tagged);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum 
Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*
i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct
_tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(
enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_extensible,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);void*Cyc_Absyn_pointer_expand(void*,int
fresh_evar);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple9*Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*,int);struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
int Cyc_Absyn_fntype_att(void*a);struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};struct _tuple10 Cyc_Absyn_aggr_kinded_name(
union Cyc_Absyn_AggrInfoU);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info);int Cyc_Absyn_is_union_type(void*);int Cyc_Absyn_is_nontagged_union_type(
void*);int Cyc_Absyn_is_aggr_type(void*t);extern int Cyc_Absyn_porting_c_code;
extern int Cyc_Absyn_no_regions;int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct
_dyneither_ptr*s2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);}
;int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[
7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct
Cyc_Dict_Absent_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct
Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct
_RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
loc);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int
Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*
r);int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*
constraints,void*default_bound,int do_pin);int Cyc_RgnOrder_eff_outlives_eff(
struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);void Cyc_RgnOrder_print_region_po(
struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;struct Cyc_Tcenv_CList*
tl;};struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_NotLoop_j_struct{int tag;};struct Cyc_Tcenv_CaseEnd_j_struct{
int tag;};struct Cyc_Tcenv_FnEnd_j_struct{int tag;};struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*
k);extern int Cyc_Cyclone_tovc_r;enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 
0,Cyc_Cyclone_Vc_c  = 1};extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;
int Cyc_Cyclone_tovc_r=0;enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;
static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*ss1,struct Cyc_List_List*ss2);
static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*ss1,struct Cyc_List_List*ss2){
return((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,ss1,ss2);}int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*vs1,struct Cyc_List_List*vs2);int Cyc_Absyn_varlist_cmp(struct
Cyc_List_List*vs1,struct Cyc_List_List*vs2){if((int)vs1 == (int)vs2)return 0;return
Cyc_Absyn_strlist_cmp(vs1,vs2);}struct _tuple11{union Cyc_Absyn_Nmspace f1;union Cyc_Absyn_Nmspace
f2;};int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct _tuple0*q2);int Cyc_Absyn_qvar_cmp(
struct _tuple0*q1,struct _tuple0*q2){union Cyc_Absyn_Nmspace _tmp0=(*q1).f1;union Cyc_Absyn_Nmspace
_tmp1=(*q2).f1;{struct _tuple11 _tmp216;struct _tuple11 _tmp3=(_tmp216.f1=_tmp0,((
_tmp216.f2=_tmp1,_tmp216)));union Cyc_Absyn_Nmspace _tmp4;int _tmp5;union Cyc_Absyn_Nmspace
_tmp6;int _tmp7;union Cyc_Absyn_Nmspace _tmp8;struct Cyc_List_List*_tmp9;union Cyc_Absyn_Nmspace
_tmpA;struct Cyc_List_List*_tmpB;union Cyc_Absyn_Nmspace _tmpC;struct Cyc_List_List*
_tmpD;union Cyc_Absyn_Nmspace _tmpE;struct Cyc_List_List*_tmpF;union Cyc_Absyn_Nmspace
_tmp10;int _tmp11;union Cyc_Absyn_Nmspace _tmp12;int _tmp13;union Cyc_Absyn_Nmspace
_tmp14;struct Cyc_List_List*_tmp15;union Cyc_Absyn_Nmspace _tmp16;struct Cyc_List_List*
_tmp17;_LL1: _tmp4=_tmp3.f1;if((_tmp4.Loc_n).tag != 3)goto _LL3;_tmp5=(int)(_tmp4.Loc_n).val;
_tmp6=_tmp3.f2;if((_tmp6.Loc_n).tag != 3)goto _LL3;_tmp7=(int)(_tmp6.Loc_n).val;
_LL2: goto _LL0;_LL3: _tmp8=_tmp3.f1;if((_tmp8.Rel_n).tag != 1)goto _LL5;_tmp9=(
struct Cyc_List_List*)(_tmp8.Rel_n).val;_tmpA=_tmp3.f2;if((_tmpA.Rel_n).tag != 1)
goto _LL5;_tmpB=(struct Cyc_List_List*)(_tmpA.Rel_n).val;_LL4: _tmpD=_tmp9;_tmpF=
_tmpB;goto _LL6;_LL5: _tmpC=_tmp3.f1;if((_tmpC.Abs_n).tag != 2)goto _LL7;_tmpD=(
struct Cyc_List_List*)(_tmpC.Abs_n).val;_tmpE=_tmp3.f2;if((_tmpE.Abs_n).tag != 2)
goto _LL7;_tmpF=(struct Cyc_List_List*)(_tmpE.Abs_n).val;_LL6: {int i=Cyc_Absyn_strlist_cmp(
_tmpD,_tmpF);if(i != 0)return i;goto _LL0;}_LL7: _tmp10=_tmp3.f1;if((_tmp10.Loc_n).tag
!= 3)goto _LL9;_tmp11=(int)(_tmp10.Loc_n).val;_LL8: return - 1;_LL9: _tmp12=_tmp3.f2;
if((_tmp12.Loc_n).tag != 3)goto _LLB;_tmp13=(int)(_tmp12.Loc_n).val;_LLA: return 1;
_LLB: _tmp14=_tmp3.f1;if((_tmp14.Rel_n).tag != 1)goto _LLD;_tmp15=(struct Cyc_List_List*)(
_tmp14.Rel_n).val;_LLC: return - 1;_LLD: _tmp16=_tmp3.f2;if((_tmp16.Rel_n).tag != 1)
goto _LL0;_tmp17=(struct Cyc_List_List*)(_tmp16.Rel_n).val;_LLE: return 1;_LL0:;}
return Cyc_strptrcmp((*q1).f2,(*q2).f2);}int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*
tv1,struct Cyc_Absyn_Tvar*tv2);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,
struct Cyc_Absyn_Tvar*tv2){int i=Cyc_strptrcmp(tv1->name,tv2->name);if(i != 0)
return i;return tv1->identity - tv2->identity;}union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n={.Loc_n={
3,0}};union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*x);union Cyc_Absyn_Nmspace
Cyc_Absyn_Abs_n(struct Cyc_List_List*x){union Cyc_Absyn_Nmspace _tmp217;return((
_tmp217.Abs_n).val=x,(((_tmp217.Abs_n).tag=2,_tmp217)));}union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*x);union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(
struct Cyc_List_List*x){union Cyc_Absyn_Nmspace _tmp218;return((_tmp218.Rel_n).val=
x,(((_tmp218.Rel_n).tag=1,_tmp218)));}union Cyc_Absyn_Nmspace Cyc_Absyn_rel_ns_null={.Rel_n={
1,0}};int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv);int Cyc_Absyn_is_qvar_qualified(
struct _tuple0*qv){union Cyc_Absyn_Nmspace _tmp1A=(*qv).f1;struct Cyc_List_List*
_tmp1B;struct Cyc_List_List*_tmp1C;int _tmp1D;_LL10: if((_tmp1A.Rel_n).tag != 1)goto
_LL12;_tmp1B=(struct Cyc_List_List*)(_tmp1A.Rel_n).val;if(_tmp1B != 0)goto _LL12;
_LL11: goto _LL13;_LL12: if((_tmp1A.Abs_n).tag != 2)goto _LL14;_tmp1C=(struct Cyc_List_List*)(
_tmp1A.Abs_n).val;if(_tmp1C != 0)goto _LL14;_LL13: goto _LL15;_LL14: if((_tmp1A.Loc_n).tag
!= 3)goto _LL16;_tmp1D=(int)(_tmp1A.Loc_n).val;_LL15: return 0;_LL16:;_LL17: return 1;
_LLF:;}static int Cyc_Absyn_new_type_counter=0;void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*env);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*
env){struct Cyc_Absyn_Evar_struct _tmp21B;struct Cyc_Absyn_Evar_struct*_tmp21A;
return(void*)((_tmp21A=_cycalloc(sizeof(*_tmp21A)),((_tmp21A[0]=((_tmp21B.tag=1,((
_tmp21B.f1=k,((_tmp21B.f2=0,((_tmp21B.f3=Cyc_Absyn_new_type_counter ++,((_tmp21B.f4=
env,_tmp21B)))))))))),_tmp21A))));}void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*
tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko,tenv);}struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){struct Cyc_Absyn_Tqual _tmp21C;
return(_tmp21C.print_const=x.print_const  || y.print_const,((_tmp21C.q_volatile=x.q_volatile
 || y.q_volatile,((_tmp21C.q_restrict=x.q_restrict  || y.q_restrict,((_tmp21C.real_const=
x.real_const  || y.real_const,((_tmp21C.loc=Cyc_Position_segment_join(x.loc,y.loc),
_tmp21C)))))))));}struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Tqual _tmp21D;return(_tmp21D.print_const=0,((_tmp21D.q_volatile=0,((
_tmp21D.q_restrict=0,((_tmp21D.real_const=0,((_tmp21D.loc=loc,_tmp21D)))))))));}
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Tqual _tmp21E;return(_tmp21E.print_const=1,((_tmp21E.q_volatile=0,((
_tmp21E.q_restrict=0,((_tmp21E.real_const=1,((_tmp21E.loc=loc,_tmp21E)))))))));}
struct Cyc_Absyn_EmptyAnnot_struct Cyc_Absyn_EmptyAnnot_val={Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Const_e_struct Cyc_Absyn_one_b_raw={0,{.Int_c={4,{Cyc_Absyn_Unsigned,
1}}}};struct Cyc_Absyn_Exp Cyc_Absyn_exp_unsigned_one_v={0,(void*)& Cyc_Absyn_one_b_raw,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one=&
Cyc_Absyn_exp_unsigned_one_v;static struct Cyc_Absyn_Upper_b_struct Cyc_Absyn_one_bt={
1,& Cyc_Absyn_exp_unsigned_one_v};void*Cyc_Absyn_bounds_one=(void*)& Cyc_Absyn_one_bt;
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo
udi);union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo
udi){union Cyc_Absyn_DatatypeInfoU _tmp21F;return((_tmp21F.UnknownDatatype).val=
udi,(((_tmp21F.UnknownDatatype).tag=1,_tmp21F)));}union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**d);extern union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**d){union Cyc_Absyn_DatatypeInfoU
_tmp220;return((_tmp220.KnownDatatype).val=d,(((_tmp220.KnownDatatype).tag=2,
_tmp220)));}union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(
struct Cyc_Absyn_UnknownDatatypeFieldInfo s);union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(
struct Cyc_Absyn_UnknownDatatypeFieldInfo s){union Cyc_Absyn_DatatypeFieldInfoU
_tmp221;return((_tmp221.UnknownDatatypefield).val=s,(((_tmp221.UnknownDatatypefield).tag=
1,_tmp221)));}union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(
struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df);union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*
df){struct _tuple1 _tmp224;union Cyc_Absyn_DatatypeFieldInfoU _tmp223;return((
_tmp223.KnownDatatypefield).val=((_tmp224.f1=dd,((_tmp224.f2=df,_tmp224)))),(((
_tmp223.KnownDatatypefield).tag=2,_tmp223)));}union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(
enum Cyc_Absyn_AggrKind ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged);union Cyc_Absyn_AggrInfoU
Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind ak,struct _tuple0*n,struct Cyc_Core_Opt*
tagged){struct _tuple2 _tmp227;union Cyc_Absyn_AggrInfoU _tmp226;return((_tmp226.UnknownAggr).val=((
_tmp227.f1=ak,((_tmp227.f2=n,((_tmp227.f3=tagged,_tmp227)))))),(((_tmp226.UnknownAggr).tag=
1,_tmp226)));}union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**
ad);union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**ad){
union Cyc_Absyn_AggrInfoU _tmp228;return((_tmp228.KnownAggr).val=ad,(((_tmp228.KnownAggr).tag=
2,_tmp228)));}union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);union Cyc_Absyn_Constraint*
Cyc_Absyn_new_conref(void*x){union Cyc_Absyn_Constraint*_tmp229;return(_tmp229=
_cycalloc(sizeof(*_tmp229)),(((_tmp229->Eq_constr).val=(void*)x,(((_tmp229->Eq_constr).tag=
1,_tmp229)))));}union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();union Cyc_Absyn_Constraint*
Cyc_Absyn_empty_conref(){union Cyc_Absyn_Constraint*_tmp22A;return(_tmp22A=
_cycalloc_atomic(sizeof(*_tmp22A)),(((_tmp22A->No_constr).val=0,(((_tmp22A->No_constr).tag=
3,_tmp22A)))));}struct Cyc_Absyn_DynEither_b_struct Cyc_Absyn_DynEither_b_val={0};
static union Cyc_Absyn_Constraint Cyc_Absyn_true_conref_v={.Eq_constr={1,(void*)1}};
union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref=& Cyc_Absyn_true_conref_v;static
union Cyc_Absyn_Constraint Cyc_Absyn_false_conref_v={.Eq_constr={1,(void*)0}};
union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref=& Cyc_Absyn_false_conref_v;
static union Cyc_Absyn_Constraint Cyc_Absyn_bounds_one_conref_v={.Eq_constr={1,(
void*)((void*)& Cyc_Absyn_one_bt)}};union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref=&
Cyc_Absyn_bounds_one_conref_v;static union Cyc_Absyn_Constraint Cyc_Absyn_bounds_dyneither_conref_v={.Eq_constr={
1,(void*)((void*)& Cyc_Absyn_DynEither_b_val)}};union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref=&
Cyc_Absyn_bounds_dyneither_conref_v;union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(
union Cyc_Absyn_Constraint*x);union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(
union Cyc_Absyn_Constraint*x){union Cyc_Absyn_Constraint*_tmp31=x;union Cyc_Absyn_Constraint
_tmp32;int _tmp33;union Cyc_Absyn_Constraint _tmp34;void*_tmp35;union Cyc_Absyn_Constraint
_tmp36;union Cyc_Absyn_Constraint*_tmp37;_LL19: _tmp32=*_tmp31;if((_tmp32.No_constr).tag
!= 3)goto _LL1B;_tmp33=(int)(_tmp32.No_constr).val;_LL1A: goto _LL1C;_LL1B: _tmp34=*
_tmp31;if((_tmp34.Eq_constr).tag != 1)goto _LL1D;_tmp35=(void*)(_tmp34.Eq_constr).val;
_LL1C: return x;_LL1D: _tmp36=*_tmp31;if((_tmp36.Forward_constr).tag != 2)goto _LL18;
_tmp37=(union Cyc_Absyn_Constraint*)(_tmp36.Forward_constr).val;_LL1E: {union Cyc_Absyn_Constraint*
_tmp38=Cyc_Absyn_compress_conref(_tmp37);{struct _union_Constraint_Forward_constr*
_tmp22B;(_tmp22B=& x->Forward_constr,((_tmp22B->tag=2,_tmp22B->val=_tmp38)));}
return _tmp38;}_LL18:;}void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x){union Cyc_Absyn_Constraint*
_tmp3A=Cyc_Absyn_compress_conref(x);union Cyc_Absyn_Constraint _tmp3B;void*_tmp3C;
_LL20: _tmp3B=*_tmp3A;if((_tmp3B.Eq_constr).tag != 1)goto _LL22;_tmp3C=(void*)(
_tmp3B.Eq_constr).val;_LL21: return _tmp3C;_LL22:;_LL23: {const char*_tmp22E;void*
_tmp22D;(_tmp22D=0,Cyc_Tcutil_impos(((_tmp22E="conref_val",_tag_dyneither(
_tmp22E,sizeof(char),11))),_tag_dyneither(_tmp22D,sizeof(void*),0)));}_LL1F:;}
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x){union Cyc_Absyn_Constraint*_tmp3F=Cyc_Absyn_compress_conref(
x);union Cyc_Absyn_Constraint _tmp40;void*_tmp41;_LL25: _tmp40=*_tmp3F;if((_tmp40.Eq_constr).tag
!= 1)goto _LL27;_tmp41=(void*)(_tmp40.Eq_constr).val;_LL26: return _tmp41;_LL27:;
_LL28: return y;_LL24:;}void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*
x);void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x){x=Cyc_Absyn_compress_conref(
x);{union Cyc_Absyn_Constraint*_tmp42=x;union Cyc_Absyn_Constraint _tmp43;void*
_tmp44;_LL2A: _tmp43=*_tmp42;if((_tmp43.Eq_constr).tag != 1)goto _LL2C;_tmp44=(void*)(
_tmp43.Eq_constr).val;_LL2B: return _tmp44;_LL2C:;_LL2D:{struct
_union_Constraint_Eq_constr*_tmp22F;(_tmp22F=& x->Eq_constr,((_tmp22F->tag=1,
_tmp22F->val=y)));}return y;_LL29:;};}void*Cyc_Absyn_compress_kb(void*k);void*Cyc_Absyn_compress_kb(
void*k){void*_tmp46=k;struct Cyc_Core_Opt*_tmp49;struct Cyc_Core_Opt*_tmp4B;struct
Cyc_Core_Opt*_tmp4D;struct Cyc_Core_Opt _tmp4E;void*_tmp4F;void**_tmp50;struct Cyc_Core_Opt*
_tmp52;struct Cyc_Core_Opt _tmp53;void*_tmp54;void**_tmp55;_LL2F: {struct Cyc_Absyn_Eq_kb_struct*
_tmp47=(struct Cyc_Absyn_Eq_kb_struct*)_tmp46;if(_tmp47->tag != 0)goto _LL31;}_LL30:
goto _LL32;_LL31: {struct Cyc_Absyn_Unknown_kb_struct*_tmp48=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp46;if(_tmp48->tag != 1)goto _LL33;else{_tmp49=_tmp48->f1;if(_tmp49 != 0)goto
_LL33;}}_LL32: goto _LL34;_LL33: {struct Cyc_Absyn_Less_kb_struct*_tmp4A=(struct Cyc_Absyn_Less_kb_struct*)
_tmp46;if(_tmp4A->tag != 2)goto _LL35;else{_tmp4B=_tmp4A->f1;if(_tmp4B != 0)goto
_LL35;}}_LL34: return k;_LL35: {struct Cyc_Absyn_Unknown_kb_struct*_tmp4C=(struct
Cyc_Absyn_Unknown_kb_struct*)_tmp46;if(_tmp4C->tag != 1)goto _LL37;else{_tmp4D=
_tmp4C->f1;if(_tmp4D == 0)goto _LL37;_tmp4E=*_tmp4D;_tmp4F=(void*)_tmp4E.v;_tmp50=(
void**)&(*_tmp4C->f1).v;}}_LL36: _tmp55=_tmp50;goto _LL38;_LL37: {struct Cyc_Absyn_Less_kb_struct*
_tmp51=(struct Cyc_Absyn_Less_kb_struct*)_tmp46;if(_tmp51->tag != 2)goto _LL2E;
else{_tmp52=_tmp51->f1;if(_tmp52 == 0)goto _LL2E;_tmp53=*_tmp52;_tmp54=(void*)
_tmp53.v;_tmp55=(void**)&(*_tmp51->f1).v;}}_LL38:*_tmp55=Cyc_Absyn_compress_kb(*
_tmp55);return*_tmp55;_LL2E:;}struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb);
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb){void*_tmp56=Cyc_Absyn_compress_kb(
kb);struct Cyc_Absyn_Kind*_tmp58;struct Cyc_Core_Opt*_tmp5A;struct Cyc_Core_Opt**
_tmp5B;struct Cyc_Core_Opt*_tmp5D;struct Cyc_Core_Opt**_tmp5E;struct Cyc_Absyn_Kind*
_tmp5F;_LL3A: {struct Cyc_Absyn_Eq_kb_struct*_tmp57=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp56;if(_tmp57->tag != 0)goto _LL3C;else{_tmp58=_tmp57->f1;}}_LL3B: return _tmp58;
_LL3C: {struct Cyc_Absyn_Unknown_kb_struct*_tmp59=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp56;if(_tmp59->tag != 1)goto _LL3E;else{_tmp5A=_tmp59->f1;_tmp5B=(struct Cyc_Core_Opt**)&
_tmp59->f1;}}_LL3D: _tmp5E=_tmp5B;_tmp5F=& Cyc_Tcutil_bk;goto _LL3F;_LL3E: {struct
Cyc_Absyn_Less_kb_struct*_tmp5C=(struct Cyc_Absyn_Less_kb_struct*)_tmp56;if(
_tmp5C->tag != 2)goto _LL39;else{_tmp5D=_tmp5C->f1;_tmp5E=(struct Cyc_Core_Opt**)&
_tmp5C->f1;_tmp5F=_tmp5C->f2;}}_LL3F:{struct Cyc_Core_Opt*_tmp230;*_tmp5E=((
_tmp230=_cycalloc(sizeof(*_tmp230)),((_tmp230->v=Cyc_Tcutil_kind_to_bound(_tmp5F),
_tmp230))));}return _tmp5F;_LL39:;}struct Cyc_Absyn_HeapRgn_struct Cyc_Absyn_HeapRgn_val={
21};struct Cyc_Absyn_UniqueRgn_struct Cyc_Absyn_UniqueRgn_val={22};struct Cyc_Absyn_VoidType_struct
Cyc_Absyn_VoidType_val={0};struct Cyc_Absyn_FloatType_struct Cyc_Absyn_FloatType_val={
7};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_char_tt={6,Cyc_Absyn_None,Cyc_Absyn_Char_sz};
static struct Cyc_Absyn_IntType_struct Cyc_Absyn_uchar_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_Char_sz};
static struct Cyc_Absyn_IntType_struct Cyc_Absyn_ushort_tt={6,Cyc_Absyn_Unsigned,
Cyc_Absyn_Short_sz};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_uint_tt={6,Cyc_Absyn_Unsigned,
Cyc_Absyn_Int_sz};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_ulong_tt={6,Cyc_Absyn_Unsigned,
Cyc_Absyn_Long_sz};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_ulonglong_tt={6,
Cyc_Absyn_Unsigned,Cyc_Absyn_LongLong_sz};void*Cyc_Absyn_char_typ=(void*)& Cyc_Absyn_char_tt;
void*Cyc_Absyn_uchar_typ=(void*)& Cyc_Absyn_uchar_tt;void*Cyc_Absyn_ushort_typ=(
void*)& Cyc_Absyn_ushort_tt;void*Cyc_Absyn_uint_typ=(void*)& Cyc_Absyn_uint_tt;
void*Cyc_Absyn_ulong_typ=(void*)& Cyc_Absyn_ulong_tt;void*Cyc_Absyn_ulonglong_typ=(
void*)& Cyc_Absyn_ulonglong_tt;static struct Cyc_Absyn_IntType_struct Cyc_Absyn_schar_tt={
6,Cyc_Absyn_Signed,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_sshort_tt={
6,Cyc_Absyn_Signed,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_sint_tt={
6,Cyc_Absyn_Signed,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_slong_tt={
6,Cyc_Absyn_Signed,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_slonglong_tt={
6,Cyc_Absyn_Signed,Cyc_Absyn_LongLong_sz};void*Cyc_Absyn_schar_typ=(void*)& Cyc_Absyn_schar_tt;
void*Cyc_Absyn_sshort_typ=(void*)& Cyc_Absyn_sshort_tt;void*Cyc_Absyn_sint_typ=(
void*)& Cyc_Absyn_sint_tt;void*Cyc_Absyn_slong_typ=(void*)& Cyc_Absyn_slong_tt;
void*Cyc_Absyn_slonglong_typ=(void*)& Cyc_Absyn_slonglong_tt;static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_nshort_tt={6,Cyc_Absyn_None,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_nint_tt={6,Cyc_Absyn_None,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_nlong_tt={6,Cyc_Absyn_None,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_nlonglong_tt={6,Cyc_Absyn_None,Cyc_Absyn_LongLong_sz};void*Cyc_Absyn_nshort_typ=(
void*)& Cyc_Absyn_nshort_tt;void*Cyc_Absyn_nint_typ=(void*)& Cyc_Absyn_nint_tt;
void*Cyc_Absyn_nlong_typ=(void*)& Cyc_Absyn_nlong_tt;void*Cyc_Absyn_nlonglong_typ=(
void*)& Cyc_Absyn_nlonglong_tt;void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign sn,enum 
Cyc_Absyn_Size_of sz);void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign sn,enum Cyc_Absyn_Size_of
sz){switch(sn){case Cyc_Absyn_Signed: _LL40: switch(sz){case Cyc_Absyn_Char_sz: _LL42:
return Cyc_Absyn_schar_typ;case Cyc_Absyn_Short_sz: _LL43: return Cyc_Absyn_sshort_typ;
case Cyc_Absyn_Int_sz: _LL44: return Cyc_Absyn_sint_typ;case Cyc_Absyn_Long_sz: _LL45:
return Cyc_Absyn_slong_typ;case Cyc_Absyn_LongLong_sz: _LL46: return Cyc_Absyn_slonglong_typ;}
case Cyc_Absyn_Unsigned: _LL41: switch(sz){case Cyc_Absyn_Char_sz: _LL49: return Cyc_Absyn_uchar_typ;
case Cyc_Absyn_Short_sz: _LL4A: return Cyc_Absyn_ushort_typ;case Cyc_Absyn_Int_sz:
_LL4B: return Cyc_Absyn_uint_typ;case Cyc_Absyn_Long_sz: _LL4C: return Cyc_Absyn_ulong_typ;
case Cyc_Absyn_LongLong_sz: _LL4D: return Cyc_Absyn_ulonglong_typ;}case Cyc_Absyn_None:
_LL48: switch(sz){case Cyc_Absyn_Char_sz: _LL50: return Cyc_Absyn_char_typ;case Cyc_Absyn_Short_sz:
_LL51: return Cyc_Absyn_nshort_typ;case Cyc_Absyn_Int_sz: _LL52: return Cyc_Absyn_nint_typ;
case Cyc_Absyn_Long_sz: _LL53: return Cyc_Absyn_nlong_typ;case Cyc_Absyn_LongLong_sz:
_LL54: return Cyc_Absyn_nlonglong_typ;}}}void*Cyc_Absyn_float_typ=(void*)& Cyc_Absyn_FloatType_val;
void*Cyc_Absyn_double_typ(int b);void*Cyc_Absyn_double_typ(int b){static struct Cyc_Absyn_DoubleType_struct
dt={8,1};static struct Cyc_Absyn_DoubleType_struct df={8,0};return(void*)(b?& dt:& df);}
static char _tmp76[4]="exn";static struct _dyneither_ptr Cyc_Absyn_exn_str={_tmp76,
_tmp76,_tmp76 + 4};static struct _tuple0 Cyc_Absyn_exn_name_v={{.Abs_n={2,0}},& Cyc_Absyn_exn_str};
struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static char _tmp77[15]="Null_Exception";
static struct _dyneither_ptr Cyc_Absyn_Null_Exception_str={_tmp77,_tmp77,_tmp77 + 15};
static struct _tuple0 Cyc_Absyn_Null_Exception_pr={{.Abs_n={2,0}},& Cyc_Absyn_Null_Exception_str};
struct _tuple0*Cyc_Absyn_Null_Exception_name=& Cyc_Absyn_Null_Exception_pr;static
struct Cyc_Absyn_Datatypefield Cyc_Absyn_Null_Exception_tuf_v={& Cyc_Absyn_Null_Exception_pr,
0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Null_Exception_tuf=&
Cyc_Absyn_Null_Exception_tuf_v;static char _tmp78[13]="Array_bounds";static struct
_dyneither_ptr Cyc_Absyn_Array_bounds_str={_tmp78,_tmp78,_tmp78 + 13};static struct
_tuple0 Cyc_Absyn_Array_bounds_pr={{.Abs_n={2,0}},& Cyc_Absyn_Array_bounds_str};
struct _tuple0*Cyc_Absyn_Array_bounds_name=& Cyc_Absyn_Array_bounds_pr;static
struct Cyc_Absyn_Datatypefield Cyc_Absyn_Array_bounds_tuf_v={& Cyc_Absyn_Array_bounds_pr,
0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Array_bounds_tuf=&
Cyc_Absyn_Array_bounds_tuf_v;static char _tmp79[16]="Match_Exception";static struct
_dyneither_ptr Cyc_Absyn_Match_Exception_str={_tmp79,_tmp79,_tmp79 + 16};static
struct _tuple0 Cyc_Absyn_Match_Exception_pr={{.Abs_n={2,0}},& Cyc_Absyn_Match_Exception_str};
struct _tuple0*Cyc_Absyn_Match_Exception_name=& Cyc_Absyn_Match_Exception_pr;
static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Match_Exception_tuf_v={& Cyc_Absyn_Match_Exception_pr,
0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Match_Exception_tuf=&
Cyc_Absyn_Match_Exception_tuf_v;static char _tmp7A[10]="Bad_alloc";static struct
_dyneither_ptr Cyc_Absyn_Bad_alloc_str={_tmp7A,_tmp7A,_tmp7A + 10};static struct
_tuple0 Cyc_Absyn_Bad_alloc_pr={{.Abs_n={2,0}},& Cyc_Absyn_Bad_alloc_str};struct
_tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;static struct Cyc_Absyn_Datatypefield
Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,0,0,Cyc_Absyn_Extern};struct
Cyc_Absyn_Datatypefield*Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v;
static struct Cyc_List_List Cyc_Absyn_exn_l0={(void*)& Cyc_Absyn_Null_Exception_tuf_v,
0};static struct Cyc_List_List Cyc_Absyn_exn_l1={(void*)& Cyc_Absyn_Array_bounds_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l0};static struct Cyc_List_List Cyc_Absyn_exn_l2={(
void*)& Cyc_Absyn_Match_Exception_tuf_v,(struct Cyc_List_List*)& Cyc_Absyn_exn_l1};
static struct Cyc_List_List Cyc_Absyn_exn_l3={(void*)& Cyc_Absyn_Bad_alloc_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l2};static struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(
void*)((struct Cyc_List_List*)& Cyc_Absyn_exn_l3)};static struct Cyc_Absyn_Datatypedecl
Cyc_Absyn_exn_tud_v={Cyc_Absyn_Extern,& Cyc_Absyn_exn_name_v,0,(struct Cyc_Core_Opt*)&
Cyc_Absyn_exn_ol,1};struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud=& Cyc_Absyn_exn_tud_v;
static struct Cyc_Core_Opt Cyc_Absyn_heap_opt={(void*)((void*)& Cyc_Absyn_HeapRgn_val)};
static struct Cyc_Absyn_DatatypeType_struct Cyc_Absyn_exn_typ_tt={3,{{.KnownDatatype={
2,& Cyc_Absyn_exn_tud}},0}};void*Cyc_Absyn_exn_typ();void*Cyc_Absyn_exn_typ(){
static struct Cyc_Core_Opt*exn_type_val=0;if(exn_type_val == 0){struct Cyc_Core_Opt*
_tmp231;exn_type_val=((_tmp231=_cycalloc(sizeof(*_tmp231)),((_tmp231->v=Cyc_Absyn_at_typ((
void*)& Cyc_Absyn_exn_typ_tt,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref),_tmp231))));}return(void*)exn_type_val->v;}static char
_tmp7D[9]="PrintArg";static struct _dyneither_ptr Cyc_Absyn_printarg_str={_tmp7D,
_tmp7D,_tmp7D + 9};static char _tmp7E[9]="ScanfArg";static struct _dyneither_ptr Cyc_Absyn_scanfarg_str={
_tmp7E,_tmp7E,_tmp7E + 9};static struct _tuple0 Cyc_Absyn_datatype_print_arg_qvar_p={{.Abs_n={
2,0}},& Cyc_Absyn_printarg_str};static struct _tuple0 Cyc_Absyn_datatype_scanf_arg_qvar_p={{.Abs_n={
2,0}},& Cyc_Absyn_scanfarg_str};struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar=&
Cyc_Absyn_datatype_print_arg_qvar_p;struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar=&
Cyc_Absyn_datatype_scanf_arg_qvar_p;static void**Cyc_Absyn_string_t_opt=0;void*
Cyc_Absyn_string_typ(void*rgn);void*Cyc_Absyn_string_typ(void*rgn){void*_tmp7F=
Cyc_Tcutil_compress(rgn);_LL57: {struct Cyc_Absyn_HeapRgn_struct*_tmp80=(struct
Cyc_Absyn_HeapRgn_struct*)_tmp7F;if(_tmp80->tag != 21)goto _LL59;}_LL58: if(Cyc_Absyn_string_t_opt
== 0){void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Absyn_empty_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
void**_tmp232;Cyc_Absyn_string_t_opt=((_tmp232=_cycalloc(sizeof(*_tmp232)),((
_tmp232[0]=t,_tmp232))));}return*((void**)_check_null(Cyc_Absyn_string_t_opt));
_LL59:;_LL5A: return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_empty_tqual(
0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);_LL56:;}static void**
Cyc_Absyn_const_string_t_opt=0;void*Cyc_Absyn_const_string_typ(void*rgn);void*
Cyc_Absyn_const_string_typ(void*rgn){void*_tmp82=Cyc_Tcutil_compress(rgn);_LL5C: {
struct Cyc_Absyn_HeapRgn_struct*_tmp83=(struct Cyc_Absyn_HeapRgn_struct*)_tmp82;
if(_tmp83->tag != 21)goto _LL5E;}_LL5D: if(Cyc_Absyn_const_string_t_opt == 0){void*t=
Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(
0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);void**_tmp233;Cyc_Absyn_const_string_t_opt=((
_tmp233=_cycalloc(sizeof(*_tmp233)),((_tmp233[0]=t,_tmp233))));}return*((void**)
_check_null(Cyc_Absyn_const_string_t_opt));_LL5E:;_LL5F: return Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,
Cyc_Absyn_true_conref);_LL5B:;}void*Cyc_Absyn_starb_typ(void*t,void*r,struct Cyc_Absyn_Tqual
tq,void*b,union Cyc_Absyn_Constraint*zeroterm);void*Cyc_Absyn_starb_typ(void*t,
void*r,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zeroterm){struct
Cyc_Absyn_PointerType_struct _tmp23D;struct Cyc_Absyn_PtrAtts _tmp23C;struct Cyc_Absyn_PtrInfo
_tmp23B;struct Cyc_Absyn_PointerType_struct*_tmp23A;return(void*)((_tmp23A=
_cycalloc(sizeof(*_tmp23A)),((_tmp23A[0]=((_tmp23D.tag=5,((_tmp23D.f1=((_tmp23B.elt_typ=
t,((_tmp23B.elt_tq=tq,((_tmp23B.ptr_atts=((_tmp23C.rgn=r,((_tmp23C.nullable=Cyc_Absyn_true_conref,((
_tmp23C.bounds=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),((
_tmp23C.zero_term=zeroterm,((_tmp23C.ptrloc=0,_tmp23C)))))))))),_tmp23B)))))),
_tmp23D)))),_tmp23A))));}void*Cyc_Absyn_atb_typ(void*t,void*r,struct Cyc_Absyn_Tqual
tq,void*b,union Cyc_Absyn_Constraint*zeroterm);void*Cyc_Absyn_atb_typ(void*t,void*
r,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zeroterm){struct Cyc_Absyn_PointerType_struct
_tmp247;struct Cyc_Absyn_PtrAtts _tmp246;struct Cyc_Absyn_PtrInfo _tmp245;struct Cyc_Absyn_PointerType_struct*
_tmp244;return(void*)((_tmp244=_cycalloc(sizeof(*_tmp244)),((_tmp244[0]=((
_tmp247.tag=5,((_tmp247.f1=((_tmp245.elt_typ=t,((_tmp245.elt_tq=tq,((_tmp245.ptr_atts=((
_tmp246.rgn=r,((_tmp246.nullable=Cyc_Absyn_false_conref,((_tmp246.bounds=((union
Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),((_tmp246.zero_term=
zeroterm,((_tmp246.ptrloc=0,_tmp246)))))))))),_tmp245)))))),_tmp247)))),_tmp244))));}
void*Cyc_Absyn_star_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zeroterm);void*Cyc_Absyn_star_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zeroterm){struct Cyc_Absyn_PointerType_struct _tmp251;struct Cyc_Absyn_PtrAtts
_tmp250;struct Cyc_Absyn_PtrInfo _tmp24F;struct Cyc_Absyn_PointerType_struct*
_tmp24E;return(void*)((_tmp24E=_cycalloc(sizeof(*_tmp24E)),((_tmp24E[0]=((
_tmp251.tag=5,((_tmp251.f1=((_tmp24F.elt_typ=t,((_tmp24F.elt_tq=tq,((_tmp24F.ptr_atts=((
_tmp250.rgn=r,((_tmp250.nullable=Cyc_Absyn_true_conref,((_tmp250.bounds=Cyc_Absyn_bounds_one_conref,((
_tmp250.zero_term=zeroterm,((_tmp250.ptrloc=0,_tmp250)))))))))),_tmp24F)))))),
_tmp251)))),_tmp24E))));}void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq){struct Cyc_Absyn_PointerType_struct
_tmp25B;struct Cyc_Absyn_PtrAtts _tmp25A;struct Cyc_Absyn_PtrInfo _tmp259;struct Cyc_Absyn_PointerType_struct*
_tmp258;return(void*)((_tmp258=_cycalloc(sizeof(*_tmp258)),((_tmp258[0]=((
_tmp25B.tag=5,((_tmp25B.f1=((_tmp259.elt_typ=t,((_tmp259.elt_tq=tq,((_tmp259.ptr_atts=((
_tmp25A.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((_tmp25A.nullable=Cyc_Absyn_true_conref,((
_tmp25A.bounds=Cyc_Absyn_bounds_one_conref,((_tmp25A.zero_term=Cyc_Absyn_false_conref,((
_tmp25A.ptrloc=0,_tmp25A)))))))))),_tmp259)))))),_tmp25B)))),_tmp258))));}void*
Cyc_Absyn_at_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zeroterm);void*Cyc_Absyn_at_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zeroterm){struct Cyc_Absyn_PointerType_struct _tmp265;struct Cyc_Absyn_PtrAtts
_tmp264;struct Cyc_Absyn_PtrInfo _tmp263;struct Cyc_Absyn_PointerType_struct*
_tmp262;return(void*)((_tmp262=_cycalloc(sizeof(*_tmp262)),((_tmp262[0]=((
_tmp265.tag=5,((_tmp265.f1=((_tmp263.elt_typ=t,((_tmp263.elt_tq=tq,((_tmp263.ptr_atts=((
_tmp264.rgn=r,((_tmp264.nullable=Cyc_Absyn_false_conref,((_tmp264.bounds=Cyc_Absyn_bounds_one_conref,((
_tmp264.zero_term=zeroterm,((_tmp264.ptrloc=0,_tmp264)))))))))),_tmp263)))))),
_tmp265)))),_tmp262))));}void*Cyc_Absyn_dyneither_typ(void*t,void*r,struct Cyc_Absyn_Tqual
tq,union Cyc_Absyn_Constraint*zeroterm);void*Cyc_Absyn_dyneither_typ(void*t,void*
r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){struct Cyc_Absyn_PointerType_struct
_tmp26F;struct Cyc_Absyn_PtrAtts _tmp26E;struct Cyc_Absyn_PtrInfo _tmp26D;struct Cyc_Absyn_PointerType_struct*
_tmp26C;return(void*)((_tmp26C=_cycalloc(sizeof(*_tmp26C)),((_tmp26C[0]=((
_tmp26F.tag=5,((_tmp26F.f1=((_tmp26D.elt_typ=t,((_tmp26D.elt_tq=tq,((_tmp26D.ptr_atts=((
_tmp26E.rgn=r,((_tmp26E.nullable=Cyc_Absyn_true_conref,((_tmp26E.bounds=Cyc_Absyn_bounds_dyneither_conref,((
_tmp26E.zero_term=zeroterm,((_tmp26E.ptrloc=0,_tmp26E)))))))))),_tmp26D)))))),
_tmp26F)))),_tmp26C))));}void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual
tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,struct Cyc_Position_Segment*
ztloc);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,union Cyc_Absyn_Constraint*zero_term,struct Cyc_Position_Segment*ztloc){
struct Cyc_Absyn_ArrayType_struct _tmp275;struct Cyc_Absyn_ArrayInfo _tmp274;struct
Cyc_Absyn_ArrayType_struct*_tmp273;return(void*)((_tmp273=_cycalloc(sizeof(*
_tmp273)),((_tmp273[0]=((_tmp275.tag=9,((_tmp275.f1=((_tmp274.elt_type=elt_type,((
_tmp274.tq=tq,((_tmp274.num_elts=num_elts,((_tmp274.zero_term=zero_term,((
_tmp274.zt_loc=ztloc,_tmp274)))))))))),_tmp275)))),_tmp273))));}static char _tmpA7[
8]="__sFILE";void*Cyc_Absyn_file_typ();void*Cyc_Absyn_file_typ(){static void**
file_t_opt=0;static struct _dyneither_ptr sf_str={_tmpA7,_tmpA7,_tmpA7 + 8};static
struct _dyneither_ptr*sf=& sf_str;if(file_t_opt == 0){struct _tuple0*_tmp276;struct
_tuple0*file_t_name=(_tmp276=_cycalloc(sizeof(*_tmp276)),((_tmp276->f1=Cyc_Absyn_Abs_n(
0),((_tmp276->f2=sf,_tmp276)))));struct Cyc_Absyn_Aggrdecl*_tmp277;struct Cyc_Absyn_Aggrdecl*
sd=(_tmp277=_cycalloc(sizeof(*_tmp277)),((_tmp277->kind=Cyc_Absyn_StructA,((
_tmp277->sc=Cyc_Absyn_Abstract,((_tmp277->name=file_t_name,((_tmp277->tvs=0,((
_tmp277->impl=0,((_tmp277->attributes=0,_tmp277)))))))))))));struct Cyc_Absyn_AggrType_struct
_tmp281;struct Cyc_Absyn_Aggrdecl**_tmp280;struct Cyc_Absyn_AggrInfo _tmp27F;struct
Cyc_Absyn_AggrType_struct*_tmp27E;void*file_struct_typ=(void*)((_tmp27E=
_cycalloc(sizeof(*_tmp27E)),((_tmp27E[0]=((_tmp281.tag=12,((_tmp281.f1=((_tmp27F.aggr_info=
Cyc_Absyn_KnownAggr(((_tmp280=_cycalloc(sizeof(*_tmp280)),((_tmp280[0]=sd,
_tmp280))))),((_tmp27F.targs=0,_tmp27F)))),_tmp281)))),_tmp27E))));void**_tmp282;
file_t_opt=((_tmp282=_cycalloc(sizeof(*_tmp282)),((_tmp282[0]=Cyc_Absyn_at_typ(
file_struct_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),
_tmp282))));}return*file_t_opt;}void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_void_star_typ(){
static void**void_star_t_opt=0;if(void_star_t_opt == 0){void**_tmp283;
void_star_t_opt=((_tmp283=_cycalloc(sizeof(*_tmp283)),((_tmp283[0]=Cyc_Absyn_star_typ((
void*)& Cyc_Absyn_VoidType_val,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref),_tmp283))));}return*void_star_t_opt;}static struct Cyc_Absyn_JoinEff_struct
Cyc_Absyn_empty_eff={24,0};void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_eff;
void*Cyc_Absyn_aggr_typ(enum Cyc_Absyn_AggrKind k,struct _dyneither_ptr*name);void*
Cyc_Absyn_aggr_typ(enum Cyc_Absyn_AggrKind k,struct _dyneither_ptr*name){struct Cyc_Absyn_AggrType_struct
_tmp28D;struct _tuple0*_tmp28C;struct Cyc_Absyn_AggrInfo _tmp28B;struct Cyc_Absyn_AggrType_struct*
_tmp28A;return(void*)((_tmp28A=_cycalloc(sizeof(*_tmp28A)),((_tmp28A[0]=((
_tmp28D.tag=12,((_tmp28D.f1=((_tmp28B.aggr_info=Cyc_Absyn_UnknownAggr(k,((
_tmp28C=_cycalloc(sizeof(*_tmp28C)),((_tmp28C->f1=Cyc_Absyn_rel_ns_null,((
_tmp28C->f2=name,_tmp28C)))))),0),((_tmp28B.targs=0,_tmp28B)))),_tmp28D)))),
_tmp28A))));}void*Cyc_Absyn_strct(struct _dyneither_ptr*name);void*Cyc_Absyn_strct(
struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ(Cyc_Absyn_StructA,name);}
void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name);void*Cyc_Absyn_union_typ(
struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ(Cyc_Absyn_UnionA,name);}void*
Cyc_Absyn_strctq(struct _tuple0*name);void*Cyc_Absyn_strctq(struct _tuple0*name){
struct Cyc_Absyn_AggrType_struct _tmp293;struct Cyc_Absyn_AggrInfo _tmp292;struct Cyc_Absyn_AggrType_struct*
_tmp291;return(void*)((_tmp291=_cycalloc(sizeof(*_tmp291)),((_tmp291[0]=((
_tmp293.tag=12,((_tmp293.f1=((_tmp292.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,
name,0),((_tmp292.targs=0,_tmp292)))),_tmp293)))),_tmp291))));}void*Cyc_Absyn_unionq_typ(
struct _tuple0*name);void*Cyc_Absyn_unionq_typ(struct _tuple0*name){struct Cyc_Absyn_AggrType_struct
_tmp299;struct Cyc_Absyn_AggrInfo _tmp298;struct Cyc_Absyn_AggrType_struct*_tmp297;
return(void*)((_tmp297=_cycalloc(sizeof(*_tmp297)),((_tmp297[0]=((_tmp299.tag=12,((
_tmp299.f1=((_tmp298.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_UnionA,name,0),((
_tmp298.targs=0,_tmp298)))),_tmp299)))),_tmp297))));}union Cyc_Absyn_Cnst Cyc_Absyn_Null_c={.Null_c={
1,0}};union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign sn,char c);union Cyc_Absyn_Cnst
Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign sn,char c){struct _tuple3 _tmp29C;union Cyc_Absyn_Cnst
_tmp29B;return((_tmp29B.Char_c).val=((_tmp29C.f1=sn,((_tmp29C.f2=c,_tmp29C)))),(((
_tmp29B.Char_c).tag=2,_tmp29B)));}union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign
sn,short s);union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign sn,short s){
struct _tuple4 _tmp29F;union Cyc_Absyn_Cnst _tmp29E;return((_tmp29E.Short_c).val=((
_tmp29F.f1=sn,((_tmp29F.f2=s,_tmp29F)))),(((_tmp29E.Short_c).tag=3,_tmp29E)));}
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign sn,int i);union Cyc_Absyn_Cnst
Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign sn,int i){struct _tuple5 _tmp2A2;union Cyc_Absyn_Cnst
_tmp2A1;return((_tmp2A1.Int_c).val=((_tmp2A2.f1=sn,((_tmp2A2.f2=i,_tmp2A2)))),(((
_tmp2A1.Int_c).tag=4,_tmp2A1)));}union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum 
Cyc_Absyn_Sign sn,long long l);union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign
sn,long long l){struct _tuple6 _tmp2A5;union Cyc_Absyn_Cnst _tmp2A4;return((_tmp2A4.LongLong_c).val=((
_tmp2A5.f1=sn,((_tmp2A5.f2=l,_tmp2A5)))),(((_tmp2A4.LongLong_c).tag=5,_tmp2A4)));}
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr s);union Cyc_Absyn_Cnst
Cyc_Absyn_Float_c(struct _dyneither_ptr s){union Cyc_Absyn_Cnst _tmp2A6;return((
_tmp2A6.Float_c).val=s,(((_tmp2A6.Float_c).tag=6,_tmp2A6)));}union Cyc_Absyn_Cnst
Cyc_Absyn_String_c(struct _dyneither_ptr s);union Cyc_Absyn_Cnst Cyc_Absyn_String_c(
struct _dyneither_ptr s){union Cyc_Absyn_Cnst _tmp2A7;return((_tmp2A7.String_c).val=
s,(((_tmp2A7.String_c).tag=7,_tmp2A7)));}struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*r,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Exp*_tmp2A8;return(
_tmp2A8=_cycalloc(sizeof(*_tmp2A8)),((_tmp2A8->topt=0,((_tmp2A8->r=r,((_tmp2A8->loc=
loc,((_tmp2A8->annot=(void*)& Cyc_Absyn_EmptyAnnot_val,_tmp2A8)))))))));}struct
Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct
Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_New_e_struct _tmp2AB;struct Cyc_Absyn_New_e_struct*_tmp2AA;return
Cyc_Absyn_new_exp((void*)((_tmp2AA=_cycalloc(sizeof(*_tmp2AA)),((_tmp2AA[0]=((
_tmp2AB.tag=17,((_tmp2AB.f1=rgn_handle,((_tmp2AB.f2=e,_tmp2AB)))))),_tmp2AA)))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Absyn_Exp*
Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_tmp2AC;return(
_tmp2AC=_cycalloc(sizeof(*_tmp2AC)),((_tmp2AC[0]=*e,_tmp2AC)));}struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Const_e_struct _tmp2AF;struct Cyc_Absyn_Const_e_struct*
_tmp2AE;return Cyc_Absyn_new_exp((void*)((_tmp2AE=_cycalloc(sizeof(*_tmp2AE)),((
_tmp2AE[0]=((_tmp2AF.tag=0,((_tmp2AF.f1=c,_tmp2AF)))),_tmp2AE)))),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Const_e_struct
_tmp2B2;struct Cyc_Absyn_Const_e_struct*_tmp2B1;return Cyc_Absyn_new_exp((void*)((
_tmp2B1=_cycalloc(sizeof(*_tmp2B1)),((_tmp2B1[0]=((_tmp2B2.tag=0,((_tmp2B2.f1=
Cyc_Absyn_Null_c,_tmp2B2)))),_tmp2B1)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
enum Cyc_Absyn_Sign s,int i,struct Cyc_Position_Segment*seg);struct Cyc_Absyn_Exp*
Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign s,int i,struct Cyc_Position_Segment*seg){
return Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(s,i),seg);}struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int i,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int i,struct Cyc_Position_Segment*loc){return Cyc_Absyn_int_exp(Cyc_Absyn_Signed,i,
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int i,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int i,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_int_exp(Cyc_Absyn_Unsigned,(int)i,loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_bool_exp(int b,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(
int b,struct Cyc_Position_Segment*loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*
Cyc_Absyn_true_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_bool_exp(1,
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*loc){return
Cyc_Absyn_bool_exp(0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(Cyc_Absyn_None,c),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(f),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_string_exp(struct _dyneither_ptr s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_const_exp(Cyc_Absyn_String_c(s),loc);}struct Cyc_Absyn_Unresolved_b_struct
Cyc_Absyn_Unresolved_b_val={0};struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct
_tuple0*q,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
struct _tuple0*q,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Var_e_struct
_tmp2B5;struct Cyc_Absyn_Var_e_struct*_tmp2B4;return Cyc_Absyn_new_exp((void*)((
_tmp2B4=_cycalloc(sizeof(*_tmp2B4)),((_tmp2B4[0]=((_tmp2B5.tag=1,((_tmp2B5.f1=q,((
_tmp2B5.f2=(void*)((void*)& Cyc_Absyn_Unresolved_b_val),_tmp2B5)))))),_tmp2B4)))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*q,void*b,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*q,void*b,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Var_e_struct _tmp2B8;struct Cyc_Absyn_Var_e_struct*_tmp2B7;
return Cyc_Absyn_new_exp((void*)((_tmp2B7=_cycalloc(sizeof(*_tmp2B7)),((_tmp2B7[0]=((
_tmp2B8.tag=1,((_tmp2B8.f1=q,((_tmp2B8.f2=(void*)b,_tmp2B8)))))),_tmp2B7)))),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*q,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*q,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_UnknownId_e_struct _tmp2BB;struct Cyc_Absyn_UnknownId_e_struct*
_tmp2BA;return Cyc_Absyn_new_exp((void*)((_tmp2BA=_cycalloc(sizeof(*_tmp2BA)),((
_tmp2BA[0]=((_tmp2BB.tag=2,((_tmp2BB.f1=q,_tmp2BB)))),_tmp2BA)))),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop p,struct Cyc_List_List*es,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum 
Cyc_Absyn_Primop p,struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){struct
Cyc_Absyn_Primop_e_struct _tmp2BE;struct Cyc_Absyn_Primop_e_struct*_tmp2BD;return
Cyc_Absyn_new_exp((void*)((_tmp2BD=_cycalloc(sizeof(*_tmp2BD)),((_tmp2BD[0]=((
_tmp2BE.tag=3,((_tmp2BE.f1=p,((_tmp2BE.f2=es,_tmp2BE)))))),_tmp2BD)))),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum 
Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct
Cyc_List_List*_tmp2BF;return Cyc_Absyn_primop_exp(p,((_tmp2BF=_cycalloc(sizeof(*
_tmp2BF)),((_tmp2BF->hd=e,((_tmp2BF->tl=0,_tmp2BF)))))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum 
Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){struct Cyc_List_List*_tmp2C2;struct Cyc_List_List*_tmp2C1;return Cyc_Absyn_primop_exp(
p,((_tmp2C1=_cycalloc(sizeof(*_tmp2C1)),((_tmp2C1->hd=e1,((_tmp2C1->tl=((_tmp2C2=
_cycalloc(sizeof(*_tmp2C2)),((_tmp2C2->hd=e2,((_tmp2C2->tl=0,_tmp2C2)))))),
_tmp2C1)))))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,e1,
e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,e1,e2,
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gt,e1,e2,
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_prim2_exp(Cyc_Absyn_Lt,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gte,e1,e2,
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_prim2_exp(Cyc_Absyn_Lte,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
popt,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_AssignOp_e_struct
_tmp2C5;struct Cyc_Absyn_AssignOp_e_struct*_tmp2C4;return Cyc_Absyn_new_exp((void*)((
_tmp2C4=_cycalloc(sizeof(*_tmp2C4)),((_tmp2C4[0]=((_tmp2C5.tag=4,((_tmp2C5.f1=e1,((
_tmp2C5.f2=popt,((_tmp2C5.f3=e2,_tmp2C5)))))))),_tmp2C4)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor
i,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(
struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Increment_e_struct _tmp2C8;struct Cyc_Absyn_Increment_e_struct*
_tmp2C7;return Cyc_Absyn_new_exp((void*)((_tmp2C7=_cycalloc(sizeof(*_tmp2C7)),((
_tmp2C7[0]=((_tmp2C8.tag=5,((_tmp2C8.f1=e,((_tmp2C8.f2=i,_tmp2C8)))))),_tmp2C7)))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostInc,loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PreInc,loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PreDec,loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostDec,loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct
Cyc_Absyn_Exp*e3,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Conditional_e_struct _tmp2CB;struct Cyc_Absyn_Conditional_e_struct*
_tmp2CA;return Cyc_Absyn_new_exp((void*)((_tmp2CA=_cycalloc(sizeof(*_tmp2CA)),((
_tmp2CA[0]=((_tmp2CB.tag=6,((_tmp2CB.f1=e1,((_tmp2CB.f2=e2,((_tmp2CB.f3=e3,
_tmp2CB)))))))),_tmp2CA)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,
struct Cyc_Position_Segment*loc){struct Cyc_Absyn_And_e_struct _tmp2CE;struct Cyc_Absyn_And_e_struct*
_tmp2CD;return Cyc_Absyn_new_exp((void*)((_tmp2CD=_cycalloc(sizeof(*_tmp2CD)),((
_tmp2CD[0]=((_tmp2CE.tag=7,((_tmp2CE.f1=e1,((_tmp2CE.f2=e2,_tmp2CE)))))),_tmp2CD)))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){struct
Cyc_Absyn_Or_e_struct _tmp2D1;struct Cyc_Absyn_Or_e_struct*_tmp2D0;return Cyc_Absyn_new_exp((
void*)((_tmp2D0=_cycalloc(sizeof(*_tmp2D0)),((_tmp2D0[0]=((_tmp2D1.tag=8,((
_tmp2D1.f1=e1,((_tmp2D1.f2=e2,_tmp2D1)))))),_tmp2D0)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_SeqExp_e_struct _tmp2D4;struct
Cyc_Absyn_SeqExp_e_struct*_tmp2D3;return Cyc_Absyn_new_exp((void*)((_tmp2D3=
_cycalloc(sizeof(*_tmp2D3)),((_tmp2D3[0]=((_tmp2D4.tag=9,((_tmp2D4.f1=e1,((
_tmp2D4.f2=e2,_tmp2D4)))))),_tmp2D3)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_UnknownCall_e_struct _tmp2D7;
struct Cyc_Absyn_UnknownCall_e_struct*_tmp2D6;return Cyc_Absyn_new_exp((void*)((
_tmp2D6=_cycalloc(sizeof(*_tmp2D6)),((_tmp2D6[0]=((_tmp2D7.tag=10,((_tmp2D7.f1=e,((
_tmp2D7.f2=es,_tmp2D7)))))),_tmp2D6)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_FnCall_e_struct _tmp2DA;struct
Cyc_Absyn_FnCall_e_struct*_tmp2D9;return Cyc_Absyn_new_exp((void*)((_tmp2D9=
_cycalloc(sizeof(*_tmp2D9)),((_tmp2D9[0]=((_tmp2DA.tag=11,((_tmp2DA.f1=e,((
_tmp2DA.f2=es,((_tmp2DA.f3=0,_tmp2DA)))))))),_tmp2D9)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_NoInstantiate_e_struct _tmp2DD;struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp2DC;return Cyc_Absyn_new_exp((void*)((_tmp2DC=_cycalloc(sizeof(*_tmp2DC)),((
_tmp2DC[0]=((_tmp2DD.tag=13,((_tmp2DD.f1=e,_tmp2DD)))),_tmp2DC)))),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
ts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Instantiate_e_struct _tmp2E0;struct Cyc_Absyn_Instantiate_e_struct*
_tmp2DF;return Cyc_Absyn_new_exp((void*)((_tmp2DF=_cycalloc(sizeof(*_tmp2DF)),((
_tmp2DF[0]=((_tmp2E0.tag=14,((_tmp2E0.f1=e,((_tmp2E0.f2=ts,_tmp2E0)))))),_tmp2DF)))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int
user_cast,enum Cyc_Absyn_Coercion c,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,enum Cyc_Absyn_Coercion
c,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Cast_e_struct _tmp2E3;struct Cyc_Absyn_Cast_e_struct*
_tmp2E2;return Cyc_Absyn_new_exp((void*)((_tmp2E2=_cycalloc(sizeof(*_tmp2E2)),((
_tmp2E2[0]=((_tmp2E3.tag=15,((_tmp2E3.f1=(void*)t,((_tmp2E3.f2=e,((_tmp2E3.f3=
user_cast,((_tmp2E3.f4=c,_tmp2E3)))))))))),_tmp2E2)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Throw_e_struct _tmp2E6;struct Cyc_Absyn_Throw_e_struct*
_tmp2E5;return Cyc_Absyn_new_exp((void*)((_tmp2E5=_cycalloc(sizeof(*_tmp2E5)),((
_tmp2E5[0]=((_tmp2E6.tag=12,((_tmp2E6.f1=e,_tmp2E6)))),_tmp2E5)))),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Address_e_struct _tmp2E9;struct Cyc_Absyn_Address_e_struct*
_tmp2E8;return Cyc_Absyn_new_exp((void*)((_tmp2E8=_cycalloc(sizeof(*_tmp2E8)),((
_tmp2E8[0]=((_tmp2E9.tag=16,((_tmp2E9.f1=e,_tmp2E9)))),_tmp2E8)))),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Sizeoftyp_e_struct _tmp2EC;struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp2EB;return Cyc_Absyn_new_exp((void*)((_tmp2EB=_cycalloc(sizeof(*_tmp2EB)),((
_tmp2EB[0]=((_tmp2EC.tag=18,((_tmp2EC.f1=(void*)t,_tmp2EC)))),_tmp2EB)))),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Sizeofexp_e_struct _tmp2EF;struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp2EE;return Cyc_Absyn_new_exp((void*)((_tmp2EE=_cycalloc(sizeof(*_tmp2EE)),((
_tmp2EE[0]=((_tmp2EF.tag=19,((_tmp2EF.f1=e,_tmp2EF)))),_tmp2EE)))),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*t,void*of,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*t,void*of,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Offsetof_e_struct _tmp2F2;struct Cyc_Absyn_Offsetof_e_struct*
_tmp2F1;return Cyc_Absyn_new_exp((void*)((_tmp2F1=_cycalloc(sizeof(*_tmp2F1)),((
_tmp2F1[0]=((_tmp2F2.tag=20,((_tmp2F2.f1=(void*)t,((_tmp2F2.f2=(void*)of,_tmp2F2)))))),
_tmp2F1)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Deref_e_struct
_tmp2F5;struct Cyc_Absyn_Deref_e_struct*_tmp2F4;return Cyc_Absyn_new_exp((void*)((
_tmp2F4=_cycalloc(sizeof(*_tmp2F4)),((_tmp2F4[0]=((_tmp2F5.tag=21,((_tmp2F5.f1=e,
_tmp2F5)))),_tmp2F4)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*n,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_AggrMember_e_struct
_tmp2F8;struct Cyc_Absyn_AggrMember_e_struct*_tmp2F7;return Cyc_Absyn_new_exp((
void*)((_tmp2F7=_cycalloc(sizeof(*_tmp2F7)),((_tmp2F7[0]=((_tmp2F8.tag=22,((
_tmp2F8.f1=e,((_tmp2F8.f2=n,((_tmp2F8.f3=0,((_tmp2F8.f4=0,_tmp2F8)))))))))),
_tmp2F7)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr*n,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_AggrArrow_e_struct _tmp2FB;struct Cyc_Absyn_AggrArrow_e_struct*
_tmp2FA;return Cyc_Absyn_new_exp((void*)((_tmp2FA=_cycalloc(sizeof(*_tmp2FA)),((
_tmp2FA[0]=((_tmp2FB.tag=23,((_tmp2FB.f1=e,((_tmp2FB.f2=n,((_tmp2FB.f3=0,((
_tmp2FB.f4=0,_tmp2FB)))))))))),_tmp2FA)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Subscript_e_struct _tmp2FE;
struct Cyc_Absyn_Subscript_e_struct*_tmp2FD;return Cyc_Absyn_new_exp((void*)((
_tmp2FD=_cycalloc(sizeof(*_tmp2FD)),((_tmp2FD[0]=((_tmp2FE.tag=24,((_tmp2FE.f1=
e1,((_tmp2FE.f2=e2,_tmp2FE)))))),_tmp2FD)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Tuple_e_struct
_tmp301;struct Cyc_Absyn_Tuple_e_struct*_tmp300;return Cyc_Absyn_new_exp((void*)((
_tmp300=_cycalloc(sizeof(*_tmp300)),((_tmp300[0]=((_tmp301.tag=25,((_tmp301.f1=
es,_tmp301)))),_tmp300)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct
Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_StmtExp_e_struct
_tmp304;struct Cyc_Absyn_StmtExp_e_struct*_tmp303;return Cyc_Absyn_new_exp((void*)((
_tmp303=_cycalloc(sizeof(*_tmp303)),((_tmp303[0]=((_tmp304.tag=37,((_tmp304.f1=s,
_tmp304)))),_tmp303)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*
loc){return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_valueof_exp(void*t,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*
Cyc_Absyn_valueof_exp(void*t,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Valueof_e_struct
_tmp307;struct Cyc_Absyn_Valueof_e_struct*_tmp306;return Cyc_Absyn_new_exp((void*)((
_tmp306=_cycalloc(sizeof(*_tmp306)),((_tmp306[0]=((_tmp307.tag=39,((_tmp307.f1=(
void*)t,_tmp307)))),_tmp306)))),loc);}struct _tuple12{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){struct Cyc_List_List*dles=
0;for(0;es != 0;es=es->tl){struct _tuple12*_tmp30A;struct Cyc_List_List*_tmp309;
dles=((_tmp309=_cycalloc(sizeof(*_tmp309)),((_tmp309->hd=((_tmp30A=_cycalloc(
sizeof(*_tmp30A)),((_tmp30A->f1=0,((_tmp30A->f2=(struct Cyc_Absyn_Exp*)es->hd,
_tmp30A)))))),((_tmp309->tl=dles,_tmp309))))));}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);{struct Cyc_Absyn_Array_e_struct
_tmp30D;struct Cyc_Absyn_Array_e_struct*_tmp30C;return Cyc_Absyn_new_exp((void*)((
_tmp30C=_cycalloc(sizeof(*_tmp30C)),((_tmp30C[0]=((_tmp30D.tag=27,((_tmp30D.f1=
dles,_tmp30D)))),_tmp30C)))),loc);};}struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*n,struct Cyc_List_List*
dles,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp310;struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp30F;return Cyc_Absyn_new_exp((
void*)((_tmp30F=_cycalloc(sizeof(*_tmp30F)),((_tmp30F[0]=((_tmp310.tag=36,((
_tmp310.f1=n,((_tmp310.f2=dles,_tmp310)))))),_tmp30F)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Stmt*
_tmp311;return(_tmp311=_cycalloc(sizeof(*_tmp311)),((_tmp311->r=s,((_tmp311->loc=
loc,((_tmp311->non_local_preds=0,((_tmp311->try_depth=0,((_tmp311->annot=(void*)&
Cyc_Absyn_EmptyAnnot_val,_tmp311)))))))))));}struct Cyc_Absyn_Skip_s_struct Cyc_Absyn_Skip_s_val={
0};struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Skip_s_val,loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Exp_s_struct
_tmp314;struct Cyc_Absyn_Exp_s_struct*_tmp313;return Cyc_Absyn_new_stmt((void*)((
_tmp313=_cycalloc(sizeof(*_tmp313)),((_tmp313[0]=((_tmp314.tag=1,((_tmp314.f1=e,
_tmp314)))),_tmp313)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*
ss,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(
struct Cyc_List_List*ss,struct Cyc_Position_Segment*loc){if(ss == 0)return Cyc_Absyn_skip_stmt(
loc);else{if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{return Cyc_Absyn_seq_stmt((
struct Cyc_Absyn_Stmt*)ss->hd,Cyc_Absyn_seq_stmts(ss->tl,loc),loc);}}}struct Cyc_Absyn_Stmt*
Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Return_s_struct _tmp317;struct Cyc_Absyn_Return_s_struct*
_tmp316;return Cyc_Absyn_new_stmt((void*)((_tmp316=_cycalloc(sizeof(*_tmp316)),((
_tmp316[0]=((_tmp317.tag=3,((_tmp317.f1=e,_tmp317)))),_tmp316)))),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*
s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct
Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_IfThenElse_s_struct
_tmp31A;struct Cyc_Absyn_IfThenElse_s_struct*_tmp319;return Cyc_Absyn_new_stmt((
void*)((_tmp319=_cycalloc(sizeof(*_tmp319)),((_tmp319[0]=((_tmp31A.tag=4,((
_tmp31A.f1=e,((_tmp31A.f2=s1,((_tmp31A.f3=s2,_tmp31A)))))))),_tmp319)))),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_While_s_struct _tmp320;struct _tuple8 _tmp31F;struct Cyc_Absyn_While_s_struct*
_tmp31E;return Cyc_Absyn_new_stmt((void*)((_tmp31E=_cycalloc(sizeof(*_tmp31E)),((
_tmp31E[0]=((_tmp320.tag=5,((_tmp320.f1=((_tmp31F.f1=e,((_tmp31F.f2=Cyc_Absyn_skip_stmt(
e->loc),_tmp31F)))),((_tmp320.f2=s,_tmp320)))))),_tmp31E)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_break_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Break_s_struct _tmp323;struct Cyc_Absyn_Break_s_struct*
_tmp322;return Cyc_Absyn_new_stmt((void*)((_tmp322=_cycalloc(sizeof(*_tmp322)),((
_tmp322[0]=((_tmp323.tag=6,((_tmp323.f1=0,_tmp323)))),_tmp322)))),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Continue_s_struct
_tmp326;struct Cyc_Absyn_Continue_s_struct*_tmp325;return Cyc_Absyn_new_stmt((void*)((
_tmp325=_cycalloc(sizeof(*_tmp325)),((_tmp325[0]=((_tmp326.tag=7,((_tmp326.f1=0,
_tmp326)))),_tmp325)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct
Cyc_Position_Segment*loc){struct Cyc_Absyn_For_s_struct _tmp32F;struct _tuple8
_tmp32E;struct _tuple8 _tmp32D;struct Cyc_Absyn_For_s_struct*_tmp32C;return Cyc_Absyn_new_stmt((
void*)((_tmp32C=_cycalloc(sizeof(*_tmp32C)),((_tmp32C[0]=((_tmp32F.tag=9,((
_tmp32F.f1=e1,((_tmp32F.f2=((_tmp32E.f1=e2,((_tmp32E.f2=Cyc_Absyn_skip_stmt(e3->loc),
_tmp32E)))),((_tmp32F.f3=((_tmp32D.f1=e3,((_tmp32D.f2=Cyc_Absyn_skip_stmt(e3->loc),
_tmp32D)))),((_tmp32F.f4=s,_tmp32F)))))))))),_tmp32C)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Switch_s_struct _tmp332;
struct Cyc_Absyn_Switch_s_struct*_tmp331;return Cyc_Absyn_new_stmt((void*)((
_tmp331=_cycalloc(sizeof(*_tmp331)),((_tmp331[0]=((_tmp332.tag=10,((_tmp332.f1=e,((
_tmp332.f2=scs,_tmp332)))))),_tmp331)))),loc);}struct _tuple13{void*f1;void*f2;};
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc){
struct _tuple13 _tmp333;struct _tuple13 _tmp11C=(_tmp333.f1=s1->r,((_tmp333.f2=s2->r,
_tmp333)));void*_tmp11D;void*_tmp11F;_LL61: _tmp11D=_tmp11C.f1;{struct Cyc_Absyn_Skip_s_struct*
_tmp11E=(struct Cyc_Absyn_Skip_s_struct*)_tmp11D;if(_tmp11E->tag != 0)goto _LL63;};
_LL62: return s2;_LL63: _tmp11F=_tmp11C.f2;{struct Cyc_Absyn_Skip_s_struct*_tmp120=(
struct Cyc_Absyn_Skip_s_struct*)_tmp11F;if(_tmp120->tag != 0)goto _LL65;};_LL64:
return s1;_LL65:;_LL66: {struct Cyc_Absyn_Seq_s_struct _tmp336;struct Cyc_Absyn_Seq_s_struct*
_tmp335;return Cyc_Absyn_new_stmt((void*)((_tmp335=_cycalloc(sizeof(*_tmp335)),((
_tmp335[0]=((_tmp336.tag=2,((_tmp336.f1=s1,((_tmp336.f2=s2,_tmp336)))))),_tmp335)))),
loc);}_LL60:;}struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*
el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Fallthru_s_struct
_tmp339;struct Cyc_Absyn_Fallthru_s_struct*_tmp338;return Cyc_Absyn_new_stmt((void*)((
_tmp338=_cycalloc(sizeof(*_tmp338)),((_tmp338[0]=((_tmp339.tag=11,((_tmp339.f1=
el,((_tmp339.f2=0,_tmp339)))))),_tmp338)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Decl_s_struct _tmp33C;struct Cyc_Absyn_Decl_s_struct*
_tmp33B;return Cyc_Absyn_new_stmt((void*)((_tmp33B=_cycalloc(sizeof(*_tmp33B)),((
_tmp33B[0]=((_tmp33C.tag=12,((_tmp33C.f1=d,((_tmp33C.f2=s,_tmp33C)))))),_tmp33B)))),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct
Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Var_d_struct
_tmp33F;struct Cyc_Absyn_Var_d_struct*_tmp33E;struct Cyc_Absyn_Decl*d=Cyc_Absyn_new_decl((
void*)((_tmp33E=_cycalloc(sizeof(*_tmp33E)),((_tmp33E[0]=((_tmp33F.tag=0,((
_tmp33F.f1=Cyc_Absyn_new_vardecl(x,t,init),_tmp33F)))),_tmp33E)))),loc);struct
Cyc_Absyn_Decl_s_struct _tmp342;struct Cyc_Absyn_Decl_s_struct*_tmp341;return Cyc_Absyn_new_stmt((
void*)((_tmp341=_cycalloc(sizeof(*_tmp341)),((_tmp341[0]=((_tmp342.tag=12,((
_tmp342.f1=d,((_tmp342.f2=s,_tmp342)))))),_tmp341)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Label_s_struct _tmp345;struct
Cyc_Absyn_Label_s_struct*_tmp344;return Cyc_Absyn_new_stmt((void*)((_tmp344=
_cycalloc(sizeof(*_tmp344)),((_tmp344[0]=((_tmp345.tag=13,((_tmp345.f1=v,((
_tmp345.f2=s,_tmp345)))))),_tmp344)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Do_s_struct _tmp34B;struct
_tuple8 _tmp34A;struct Cyc_Absyn_Do_s_struct*_tmp349;return Cyc_Absyn_new_stmt((
void*)((_tmp349=_cycalloc(sizeof(*_tmp349)),((_tmp349[0]=((_tmp34B.tag=14,((
_tmp34B.f1=s,((_tmp34B.f2=((_tmp34A.f1=e,((_tmp34A.f2=Cyc_Absyn_skip_stmt(e->loc),
_tmp34A)))),_tmp34B)))))),_tmp349)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_TryCatch_s_struct _tmp34E;
struct Cyc_Absyn_TryCatch_s_struct*_tmp34D;return Cyc_Absyn_new_stmt((void*)((
_tmp34D=_cycalloc(sizeof(*_tmp34D)),((_tmp34D[0]=((_tmp34E.tag=15,((_tmp34E.f1=s,((
_tmp34E.f2=scs,_tmp34E)))))),_tmp34D)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(
struct _dyneither_ptr*lab,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Goto_s_struct _tmp351;struct Cyc_Absyn_Goto_s_struct*_tmp350;
return Cyc_Absyn_new_stmt((void*)((_tmp350=_cycalloc(sizeof(*_tmp350)),((_tmp350[
0]=((_tmp351.tag=8,((_tmp351.f1=lab,((_tmp351.f2=0,_tmp351)))))),_tmp350)))),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(e1,e2,loc),loc);}struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(
void*p,struct Cyc_Position_Segment*s){struct Cyc_Absyn_Pat*_tmp352;return(_tmp352=
_cycalloc(sizeof(*_tmp352)),((_tmp352->r=p,((_tmp352->topt=0,((_tmp352->loc=s,
_tmp352)))))));}struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_p_struct
_tmp355;struct Cyc_Absyn_Exp_p_struct*_tmp354;return Cyc_Absyn_new_pat((void*)((
_tmp354=_cycalloc(sizeof(*_tmp354)),((_tmp354[0]=((_tmp355.tag=16,((_tmp355.f1=e,
_tmp355)))),_tmp354)))),e->loc);}struct Cyc_Absyn_Wild_p_struct Cyc_Absyn_Wild_p_val={
0};struct Cyc_Absyn_Null_p_struct Cyc_Absyn_Null_p_val={8};struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Decl*
_tmp356;return(_tmp356=_cycalloc(sizeof(*_tmp356)),((_tmp356->r=r,((_tmp356->loc=
loc,_tmp356)))));}struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Let_d_struct _tmp359;struct Cyc_Absyn_Let_d_struct*_tmp358;return
Cyc_Absyn_new_decl((void*)((_tmp358=_cycalloc(sizeof(*_tmp358)),((_tmp358[0]=((
_tmp359.tag=2,((_tmp359.f1=p,((_tmp359.f2=0,((_tmp359.f3=e,_tmp359)))))))),
_tmp358)))),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*
vds,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*vds,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Letv_d_struct
_tmp35C;struct Cyc_Absyn_Letv_d_struct*_tmp35B;return Cyc_Absyn_new_decl((void*)((
_tmp35B=_cycalloc(sizeof(*_tmp35B)),((_tmp35B[0]=((_tmp35C.tag=3,((_tmp35C.f1=
vds,_tmp35C)))),_tmp35B)))),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(
struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,int resetable,struct Cyc_Absyn_Exp*
eo,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(
struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,int resetable,struct Cyc_Absyn_Exp*
eo,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Region_d_struct _tmp35F;struct
Cyc_Absyn_Region_d_struct*_tmp35E;return Cyc_Absyn_new_decl((void*)((_tmp35E=
_cycalloc(sizeof(*_tmp35E)),((_tmp35E[0]=((_tmp35F.tag=4,((_tmp35F.f1=tv,((
_tmp35F.f2=vd,((_tmp35F.f3=resetable,((_tmp35F.f4=eo,_tmp35F)))))))))),_tmp35E)))),
loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Tvar*
tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Alias_d_struct _tmp362;struct
Cyc_Absyn_Alias_d_struct*_tmp361;return Cyc_Absyn_new_decl((void*)((_tmp361=
_cycalloc(sizeof(*_tmp361)),((_tmp361[0]=((_tmp362.tag=5,((_tmp362.f1=e,((
_tmp362.f2=tv,((_tmp362.f3=vd,_tmp362)))))))),_tmp361)))),loc);}struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){struct Cyc_Absyn_Vardecl*
_tmp363;return(_tmp363=_cycalloc(sizeof(*_tmp363)),((_tmp363->sc=Cyc_Absyn_Public,((
_tmp363->name=x,((_tmp363->tq=Cyc_Absyn_empty_tqual(0),((_tmp363->type=t,((
_tmp363->initializer=init,((_tmp363->rgn=0,((_tmp363->attributes=0,((_tmp363->escapes=
0,_tmp363)))))))))))))))));}struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){struct Cyc_Absyn_Vardecl*_tmp364;
return(_tmp364=_cycalloc(sizeof(*_tmp364)),((_tmp364->sc=Cyc_Absyn_Static,((
_tmp364->name=x,((_tmp364->tq=Cyc_Absyn_empty_tqual(0),((_tmp364->type=t,((
_tmp364->initializer=init,((_tmp364->rgn=0,((_tmp364->attributes=0,((_tmp364->escapes=
0,_tmp364)))))))))))))))));}struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(
struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int
tagged);struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*
exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged){struct Cyc_Absyn_AggrdeclImpl*
_tmp365;return(_tmp365=_cycalloc(sizeof(*_tmp365)),((_tmp365->exist_vars=exists,((
_tmp365->rgn_po=po,((_tmp365->fields=fs,((_tmp365->tagged=tagged,_tmp365)))))))));}
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope
s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct
Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(
enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Aggr_d_struct _tmp36B;struct Cyc_Absyn_Aggrdecl*_tmp36A;
struct Cyc_Absyn_Aggr_d_struct*_tmp369;return Cyc_Absyn_new_decl((void*)((_tmp369=
_cycalloc(sizeof(*_tmp369)),((_tmp369[0]=((_tmp36B.tag=6,((_tmp36B.f1=((_tmp36A=
_cycalloc(sizeof(*_tmp36A)),((_tmp36A->kind=k,((_tmp36A->sc=s,((_tmp36A->name=n,((
_tmp36A->tvs=ts,((_tmp36A->impl=i,((_tmp36A->attributes=atts,_tmp36A)))))))))))))),
_tmp36B)))),_tmp369)))),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum 
Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*
i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_aggr_decl(Cyc_Absyn_StructA,s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*
Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct
_tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl(Cyc_Absyn_UnionA,
s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope
s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(
enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_extensible,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Datatype_d_struct
_tmp371;struct Cyc_Absyn_Datatypedecl*_tmp370;struct Cyc_Absyn_Datatype_d_struct*
_tmp36F;return Cyc_Absyn_new_decl((void*)((_tmp36F=_cycalloc(sizeof(*_tmp36F)),((
_tmp36F[0]=((_tmp371.tag=7,((_tmp371.f1=((_tmp370=_cycalloc(sizeof(*_tmp370)),((
_tmp370->sc=s,((_tmp370->name=n,((_tmp370->tvs=ts,((_tmp370->fields=fs,((_tmp370->is_extensible=
is_extensible,_tmp370)))))))))))),_tmp371)))),_tmp36F)))),loc);}static struct
_tuple7*Cyc_Absyn_expand_arg(struct _tuple7*a);static struct _tuple7*Cyc_Absyn_expand_arg(
struct _tuple7*a){struct _tuple7*_tmp372;return(_tmp372=_cycalloc(sizeof(*_tmp372)),((
_tmp372->f1=(*a).f1,((_tmp372->f2=(*a).f2,((_tmp372->f3=Cyc_Absyn_pointer_expand((*
a).f3,1),_tmp372)))))));}void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,
struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*args,int c_varargs,
struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*
atts);void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*
eff_typ,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts){struct Cyc_Absyn_FnType_struct
_tmp378;struct Cyc_Absyn_FnInfo _tmp377;struct Cyc_Absyn_FnType_struct*_tmp376;
return(void*)((_tmp376=_cycalloc(sizeof(*_tmp376)),((_tmp376[0]=((_tmp378.tag=10,((
_tmp378.f1=((_tmp377.tvars=tvs,((_tmp377.ret_typ=Cyc_Absyn_pointer_expand(
ret_typ,0),((_tmp377.effect=eff_typ,((_tmp377.args=((struct Cyc_List_List*(*)(
struct _tuple7*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absyn_expand_arg,
args),((_tmp377.c_varargs=c_varargs,((_tmp377.cyc_varargs=cyc_varargs,((_tmp377.rgn_po=
rgn_po,((_tmp377.attributes=atts,_tmp377)))))))))))))))),_tmp378)))),_tmp376))));}
void*Cyc_Absyn_pointer_expand(void*t,int fresh_evar);void*Cyc_Absyn_pointer_expand(
void*t,int fresh_evar){void*_tmp14F=Cyc_Tcutil_compress(t);_LL68: {struct Cyc_Absyn_FnType_struct*
_tmp150=(struct Cyc_Absyn_FnType_struct*)_tmp14F;if(_tmp150->tag != 10)goto _LL6A;}
_LL69: {struct Cyc_Core_Opt*_tmp379;return Cyc_Absyn_at_typ(t,fresh_evar?Cyc_Absyn_new_evar(((
_tmp379=_cycalloc(sizeof(*_tmp379)),((_tmp379->v=& Cyc_Tcutil_rk,_tmp379)))),0):(
void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);}
_LL6A:;_LL6B: return t;_LL67:;}int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e);int
Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e){void*_tmp152=e->r;void*_tmp154;void*
_tmp157;struct Cyc_Absyn_Vardecl*_tmp159;void*_tmp15B;struct Cyc_Absyn_Vardecl*
_tmp15D;struct Cyc_Absyn_Exp*_tmp163;struct Cyc_Absyn_Exp*_tmp165;struct Cyc_Absyn_Exp*
_tmp167;_LL6D: {struct Cyc_Absyn_Var_e_struct*_tmp153=(struct Cyc_Absyn_Var_e_struct*)
_tmp152;if(_tmp153->tag != 1)goto _LL6F;else{_tmp154=(void*)_tmp153->f2;{struct Cyc_Absyn_Funname_b_struct*
_tmp155=(struct Cyc_Absyn_Funname_b_struct*)_tmp154;if(_tmp155->tag != 2)goto _LL6F;};}}
_LL6E: return 0;_LL6F: {struct Cyc_Absyn_Var_e_struct*_tmp156=(struct Cyc_Absyn_Var_e_struct*)
_tmp152;if(_tmp156->tag != 1)goto _LL71;else{_tmp157=(void*)_tmp156->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp158=(struct Cyc_Absyn_Global_b_struct*)_tmp157;if(_tmp158->tag != 1)goto _LL71;
else{_tmp159=_tmp158->f1;}};}}_LL70: _tmp15D=_tmp159;goto _LL72;_LL71: {struct Cyc_Absyn_Var_e_struct*
_tmp15A=(struct Cyc_Absyn_Var_e_struct*)_tmp152;if(_tmp15A->tag != 1)goto _LL73;
else{_tmp15B=(void*)_tmp15A->f2;{struct Cyc_Absyn_Local_b_struct*_tmp15C=(struct
Cyc_Absyn_Local_b_struct*)_tmp15B;if(_tmp15C->tag != 4)goto _LL73;else{_tmp15D=
_tmp15C->f1;}};}}_LL72: {void*_tmp168=Cyc_Tcutil_compress(_tmp15D->type);_LL84: {
struct Cyc_Absyn_ArrayType_struct*_tmp169=(struct Cyc_Absyn_ArrayType_struct*)
_tmp168;if(_tmp169->tag != 9)goto _LL86;}_LL85: return 0;_LL86:;_LL87: return 1;_LL83:;}
_LL73: {struct Cyc_Absyn_Var_e_struct*_tmp15E=(struct Cyc_Absyn_Var_e_struct*)
_tmp152;if(_tmp15E->tag != 1)goto _LL75;}_LL74: goto _LL76;_LL75: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp15F=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp152;if(_tmp15F->tag != 23)goto
_LL77;}_LL76: goto _LL78;_LL77: {struct Cyc_Absyn_Deref_e_struct*_tmp160=(struct Cyc_Absyn_Deref_e_struct*)
_tmp152;if(_tmp160->tag != 21)goto _LL79;}_LL78: goto _LL7A;_LL79: {struct Cyc_Absyn_Subscript_e_struct*
_tmp161=(struct Cyc_Absyn_Subscript_e_struct*)_tmp152;if(_tmp161->tag != 24)goto
_LL7B;}_LL7A: return 1;_LL7B: {struct Cyc_Absyn_AggrMember_e_struct*_tmp162=(struct
Cyc_Absyn_AggrMember_e_struct*)_tmp152;if(_tmp162->tag != 22)goto _LL7D;else{
_tmp163=_tmp162->f1;}}_LL7C: return Cyc_Absyn_is_lvalue(_tmp163);_LL7D: {struct Cyc_Absyn_Instantiate_e_struct*
_tmp164=(struct Cyc_Absyn_Instantiate_e_struct*)_tmp152;if(_tmp164->tag != 14)goto
_LL7F;else{_tmp165=_tmp164->f1;}}_LL7E: return Cyc_Absyn_is_lvalue(_tmp165);_LL7F: {
struct Cyc_Absyn_NoInstantiate_e_struct*_tmp166=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp152;if(_tmp166->tag != 13)goto _LL81;else{_tmp167=_tmp166->f1;}}_LL80: return
Cyc_Absyn_is_lvalue(_tmp167);_LL81:;_LL82: return 0;_LL6C:;}struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _dyneither_ptr*v);struct
Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct
_dyneither_ptr*v){{struct Cyc_List_List*_tmp16A=fields;for(0;_tmp16A != 0;_tmp16A=
_tmp16A->tl){if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)_tmp16A->hd)->name,v)
== 0)return(struct Cyc_Absyn_Aggrfield*)((struct Cyc_Absyn_Aggrfield*)_tmp16A->hd);}}
return 0;}struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*
ad,struct _dyneither_ptr*v);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*v){return ad->impl == 0?0: Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}struct _tuple9*
Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i);struct _tuple9*Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*ts,int i){for(0;i != 0;-- i){if(ts == 0)return 0;ts=ts->tl;}if(ts
== 0)return 0;return(struct _tuple9*)((struct _tuple9*)ts->hd);}struct Cyc_Absyn_Stdcall_att_struct
Cyc_Absyn_Stdcall_att_val={1};struct Cyc_Absyn_Cdecl_att_struct Cyc_Absyn_Cdecl_att_val={
2};struct Cyc_Absyn_Fastcall_att_struct Cyc_Absyn_Fastcall_att_val={3};struct Cyc_Absyn_Noreturn_att_struct
Cyc_Absyn_Noreturn_att_val={4};struct Cyc_Absyn_Const_att_struct Cyc_Absyn_Const_att_val={
5};struct Cyc_Absyn_Packed_att_struct Cyc_Absyn_Packed_att_val={7};struct Cyc_Absyn_Nocommon_att_struct
Cyc_Absyn_Nocommon_att_val={9};struct Cyc_Absyn_Shared_att_struct Cyc_Absyn_Shared_att_val={
10};struct Cyc_Absyn_Unused_att_struct Cyc_Absyn_Unused_att_val={11};struct Cyc_Absyn_Weak_att_struct
Cyc_Absyn_Weak_att_val={12};struct Cyc_Absyn_Dllimport_att_struct Cyc_Absyn_Dllimport_att_val={
13};struct Cyc_Absyn_Dllexport_att_struct Cyc_Absyn_Dllexport_att_val={14};struct
Cyc_Absyn_No_instrument_function_att_struct Cyc_Absyn_No_instrument_function_att_val={
15};struct Cyc_Absyn_Constructor_att_struct Cyc_Absyn_Constructor_att_val={16};
struct Cyc_Absyn_Destructor_att_struct Cyc_Absyn_Destructor_att_val={17};struct Cyc_Absyn_No_check_memory_usage_att_struct
Cyc_Absyn_No_check_memory_usage_att_val={18};struct Cyc_Absyn_Pure_att_struct Cyc_Absyn_Pure_att_val={
22};struct _dyneither_ptr Cyc_Absyn_attribute2string(void*a);struct _dyneither_ptr
Cyc_Absyn_attribute2string(void*a){void*_tmp17C=a;int _tmp17E;int _tmp185;struct
_dyneither_ptr _tmp188;enum Cyc_Absyn_Format_Type _tmp194;int _tmp195;int _tmp196;
enum Cyc_Absyn_Format_Type _tmp198;int _tmp199;int _tmp19A;int _tmp19C;int _tmp19E;
struct _dyneither_ptr _tmp1A1;_LL89: {struct Cyc_Absyn_Regparm_att_struct*_tmp17D=(
struct Cyc_Absyn_Regparm_att_struct*)_tmp17C;if(_tmp17D->tag != 0)goto _LL8B;else{
_tmp17E=_tmp17D->f1;}}_LL8A: {const char*_tmp37D;void*_tmp37C[1];struct Cyc_Int_pa_struct
_tmp37B;return(struct _dyneither_ptr)((_tmp37B.tag=1,((_tmp37B.f1=(unsigned long)
_tmp17E,((_tmp37C[0]=& _tmp37B,Cyc_aprintf(((_tmp37D="regparm(%d)",_tag_dyneither(
_tmp37D,sizeof(char),12))),_tag_dyneither(_tmp37C,sizeof(void*),1))))))));}_LL8B: {
struct Cyc_Absyn_Stdcall_att_struct*_tmp17F=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp17C;if(_tmp17F->tag != 1)goto _LL8D;}_LL8C: {const char*_tmp37E;return(_tmp37E="stdcall",
_tag_dyneither(_tmp37E,sizeof(char),8));}_LL8D: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp180=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp17C;if(_tmp180->tag != 2)goto _LL8F;}
_LL8E: {const char*_tmp37F;return(_tmp37F="cdecl",_tag_dyneither(_tmp37F,sizeof(
char),6));}_LL8F: {struct Cyc_Absyn_Fastcall_att_struct*_tmp181=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp17C;if(_tmp181->tag != 3)goto _LL91;}_LL90: {const char*_tmp380;return(_tmp380="fastcall",
_tag_dyneither(_tmp380,sizeof(char),9));}_LL91: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp182=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp17C;if(_tmp182->tag != 4)goto
_LL93;}_LL92: {const char*_tmp381;return(_tmp381="noreturn",_tag_dyneither(
_tmp381,sizeof(char),9));}_LL93: {struct Cyc_Absyn_Const_att_struct*_tmp183=(
struct Cyc_Absyn_Const_att_struct*)_tmp17C;if(_tmp183->tag != 5)goto _LL95;}_LL94: {
const char*_tmp382;return(_tmp382="const",_tag_dyneither(_tmp382,sizeof(char),6));}
_LL95: {struct Cyc_Absyn_Aligned_att_struct*_tmp184=(struct Cyc_Absyn_Aligned_att_struct*)
_tmp17C;if(_tmp184->tag != 6)goto _LL97;else{_tmp185=_tmp184->f1;}}_LL96: if(
_tmp185 == - 1){const char*_tmp383;return(_tmp383="aligned",_tag_dyneither(_tmp383,
sizeof(char),8));}else{switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c:
_LLBB: {const char*_tmp387;void*_tmp386[1];struct Cyc_Int_pa_struct _tmp385;return(
struct _dyneither_ptr)((_tmp385.tag=1,((_tmp385.f1=(unsigned long)_tmp185,((
_tmp386[0]=& _tmp385,Cyc_aprintf(((_tmp387="aligned(%d)",_tag_dyneither(_tmp387,
sizeof(char),12))),_tag_dyneither(_tmp386,sizeof(void*),1))))))));}case Cyc_Cyclone_Vc_c:
_LLBC: {const char*_tmp38B;void*_tmp38A[1];struct Cyc_Int_pa_struct _tmp389;return(
struct _dyneither_ptr)((_tmp389.tag=1,((_tmp389.f1=(unsigned long)_tmp185,((
_tmp38A[0]=& _tmp389,Cyc_aprintf(((_tmp38B="align(%d)",_tag_dyneither(_tmp38B,
sizeof(char),10))),_tag_dyneither(_tmp38A,sizeof(void*),1))))))));}}}_LL97: {
struct Cyc_Absyn_Packed_att_struct*_tmp186=(struct Cyc_Absyn_Packed_att_struct*)
_tmp17C;if(_tmp186->tag != 7)goto _LL99;}_LL98: {const char*_tmp38C;return(_tmp38C="packed",
_tag_dyneither(_tmp38C,sizeof(char),7));}_LL99: {struct Cyc_Absyn_Section_att_struct*
_tmp187=(struct Cyc_Absyn_Section_att_struct*)_tmp17C;if(_tmp187->tag != 8)goto
_LL9B;else{_tmp188=_tmp187->f1;}}_LL9A: {const char*_tmp390;void*_tmp38F[1];
struct Cyc_String_pa_struct _tmp38E;return(struct _dyneither_ptr)((_tmp38E.tag=0,((
_tmp38E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp188),((_tmp38F[0]=&
_tmp38E,Cyc_aprintf(((_tmp390="section(\"%s\")",_tag_dyneither(_tmp390,sizeof(
char),14))),_tag_dyneither(_tmp38F,sizeof(void*),1))))))));}_LL9B: {struct Cyc_Absyn_Nocommon_att_struct*
_tmp189=(struct Cyc_Absyn_Nocommon_att_struct*)_tmp17C;if(_tmp189->tag != 9)goto
_LL9D;}_LL9C: {const char*_tmp391;return(_tmp391="nocommon",_tag_dyneither(
_tmp391,sizeof(char),9));}_LL9D: {struct Cyc_Absyn_Shared_att_struct*_tmp18A=(
struct Cyc_Absyn_Shared_att_struct*)_tmp17C;if(_tmp18A->tag != 10)goto _LL9F;}_LL9E: {
const char*_tmp392;return(_tmp392="shared",_tag_dyneither(_tmp392,sizeof(char),7));}
_LL9F: {struct Cyc_Absyn_Unused_att_struct*_tmp18B=(struct Cyc_Absyn_Unused_att_struct*)
_tmp17C;if(_tmp18B->tag != 11)goto _LLA1;}_LLA0: {const char*_tmp393;return(_tmp393="unused",
_tag_dyneither(_tmp393,sizeof(char),7));}_LLA1: {struct Cyc_Absyn_Weak_att_struct*
_tmp18C=(struct Cyc_Absyn_Weak_att_struct*)_tmp17C;if(_tmp18C->tag != 12)goto _LLA3;}
_LLA2: {const char*_tmp394;return(_tmp394="weak",_tag_dyneither(_tmp394,sizeof(
char),5));}_LLA3: {struct Cyc_Absyn_Dllimport_att_struct*_tmp18D=(struct Cyc_Absyn_Dllimport_att_struct*)
_tmp17C;if(_tmp18D->tag != 13)goto _LLA5;}_LLA4: {const char*_tmp395;return(_tmp395="dllimport",
_tag_dyneither(_tmp395,sizeof(char),10));}_LLA5: {struct Cyc_Absyn_Dllexport_att_struct*
_tmp18E=(struct Cyc_Absyn_Dllexport_att_struct*)_tmp17C;if(_tmp18E->tag != 14)goto
_LLA7;}_LLA6: {const char*_tmp396;return(_tmp396="dllexport",_tag_dyneither(
_tmp396,sizeof(char),10));}_LLA7: {struct Cyc_Absyn_No_instrument_function_att_struct*
_tmp18F=(struct Cyc_Absyn_No_instrument_function_att_struct*)_tmp17C;if(_tmp18F->tag
!= 15)goto _LLA9;}_LLA8: {const char*_tmp397;return(_tmp397="no_instrument_function",
_tag_dyneither(_tmp397,sizeof(char),23));}_LLA9: {struct Cyc_Absyn_Constructor_att_struct*
_tmp190=(struct Cyc_Absyn_Constructor_att_struct*)_tmp17C;if(_tmp190->tag != 16)
goto _LLAB;}_LLAA: {const char*_tmp398;return(_tmp398="constructor",_tag_dyneither(
_tmp398,sizeof(char),12));}_LLAB: {struct Cyc_Absyn_Destructor_att_struct*_tmp191=(
struct Cyc_Absyn_Destructor_att_struct*)_tmp17C;if(_tmp191->tag != 17)goto _LLAD;}
_LLAC: {const char*_tmp399;return(_tmp399="destructor",_tag_dyneither(_tmp399,
sizeof(char),11));}_LLAD: {struct Cyc_Absyn_No_check_memory_usage_att_struct*
_tmp192=(struct Cyc_Absyn_No_check_memory_usage_att_struct*)_tmp17C;if(_tmp192->tag
!= 18)goto _LLAF;}_LLAE: {const char*_tmp39A;return(_tmp39A="no_check_memory_usage",
_tag_dyneither(_tmp39A,sizeof(char),22));}_LLAF: {struct Cyc_Absyn_Format_att_struct*
_tmp193=(struct Cyc_Absyn_Format_att_struct*)_tmp17C;if(_tmp193->tag != 19)goto
_LLB1;else{_tmp194=_tmp193->f1;if(_tmp194 != Cyc_Absyn_Printf_ft)goto _LLB1;
_tmp195=_tmp193->f2;_tmp196=_tmp193->f3;}}_LLB0: {const char*_tmp39F;void*_tmp39E[
2];struct Cyc_Int_pa_struct _tmp39D;struct Cyc_Int_pa_struct _tmp39C;return(struct
_dyneither_ptr)((_tmp39C.tag=1,((_tmp39C.f1=(unsigned int)_tmp196,((_tmp39D.tag=
1,((_tmp39D.f1=(unsigned int)_tmp195,((_tmp39E[0]=& _tmp39D,((_tmp39E[1]=& _tmp39C,
Cyc_aprintf(((_tmp39F="format(printf,%u,%u)",_tag_dyneither(_tmp39F,sizeof(char),
21))),_tag_dyneither(_tmp39E,sizeof(void*),2))))))))))))));}_LLB1: {struct Cyc_Absyn_Format_att_struct*
_tmp197=(struct Cyc_Absyn_Format_att_struct*)_tmp17C;if(_tmp197->tag != 19)goto
_LLB3;else{_tmp198=_tmp197->f1;if(_tmp198 != Cyc_Absyn_Scanf_ft)goto _LLB3;_tmp199=
_tmp197->f2;_tmp19A=_tmp197->f3;}}_LLB2: {const char*_tmp3A4;void*_tmp3A3[2];
struct Cyc_Int_pa_struct _tmp3A2;struct Cyc_Int_pa_struct _tmp3A1;return(struct
_dyneither_ptr)((_tmp3A1.tag=1,((_tmp3A1.f1=(unsigned int)_tmp19A,((_tmp3A2.tag=
1,((_tmp3A2.f1=(unsigned int)_tmp199,((_tmp3A3[0]=& _tmp3A2,((_tmp3A3[1]=& _tmp3A1,
Cyc_aprintf(((_tmp3A4="format(scanf,%u,%u)",_tag_dyneither(_tmp3A4,sizeof(char),
20))),_tag_dyneither(_tmp3A3,sizeof(void*),2))))))))))))));}_LLB3: {struct Cyc_Absyn_Initializes_att_struct*
_tmp19B=(struct Cyc_Absyn_Initializes_att_struct*)_tmp17C;if(_tmp19B->tag != 20)
goto _LLB5;else{_tmp19C=_tmp19B->f1;}}_LLB4: {const char*_tmp3A8;void*_tmp3A7[1];
struct Cyc_Int_pa_struct _tmp3A6;return(struct _dyneither_ptr)((_tmp3A6.tag=1,((
_tmp3A6.f1=(unsigned long)_tmp19C,((_tmp3A7[0]=& _tmp3A6,Cyc_aprintf(((_tmp3A8="initializes(%d)",
_tag_dyneither(_tmp3A8,sizeof(char),16))),_tag_dyneither(_tmp3A7,sizeof(void*),1))))))));}
_LLB5: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp19D=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp17C;if(_tmp19D->tag != 21)goto _LLB7;else{_tmp19E=_tmp19D->f1;}}_LLB6: {const
char*_tmp3AC;void*_tmp3AB[1];struct Cyc_Int_pa_struct _tmp3AA;return(struct
_dyneither_ptr)((_tmp3AA.tag=1,((_tmp3AA.f1=(unsigned long)_tmp19E,((_tmp3AB[0]=&
_tmp3AA,Cyc_aprintf(((_tmp3AC="noliveunique(%d)",_tag_dyneither(_tmp3AC,sizeof(
char),17))),_tag_dyneither(_tmp3AB,sizeof(void*),1))))))));}_LLB7: {struct Cyc_Absyn_Pure_att_struct*
_tmp19F=(struct Cyc_Absyn_Pure_att_struct*)_tmp17C;if(_tmp19F->tag != 22)goto _LLB9;}
_LLB8: {const char*_tmp3AD;return(_tmp3AD="pure",_tag_dyneither(_tmp3AD,sizeof(
char),5));}_LLB9: {struct Cyc_Absyn_Mode_att_struct*_tmp1A0=(struct Cyc_Absyn_Mode_att_struct*)
_tmp17C;if(_tmp1A0->tag != 23)goto _LL88;else{_tmp1A1=_tmp1A0->f1;}}_LLBA: {const
char*_tmp3B1;void*_tmp3B0[1];struct Cyc_String_pa_struct _tmp3AF;return(struct
_dyneither_ptr)((_tmp3AF.tag=0,((_tmp3AF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp1A1),((_tmp3B0[0]=& _tmp3AF,Cyc_aprintf(((_tmp3B1="__mode__(\"%s\")",
_tag_dyneither(_tmp3B1,sizeof(char),15))),_tag_dyneither(_tmp3B0,sizeof(void*),1))))))));}
_LL88:;}int Cyc_Absyn_fntype_att(void*a);int Cyc_Absyn_fntype_att(void*a){void*
_tmp1D1=a;int _tmp1D3;_LLBF: {struct Cyc_Absyn_Regparm_att_struct*_tmp1D2=(struct
Cyc_Absyn_Regparm_att_struct*)_tmp1D1;if(_tmp1D2->tag != 0)goto _LLC1;else{_tmp1D3=
_tmp1D2->f1;}}_LLC0: goto _LLC2;_LLC1: {struct Cyc_Absyn_Fastcall_att_struct*
_tmp1D4=(struct Cyc_Absyn_Fastcall_att_struct*)_tmp1D1;if(_tmp1D4->tag != 3)goto
_LLC3;}_LLC2: goto _LLC4;_LLC3: {struct Cyc_Absyn_Stdcall_att_struct*_tmp1D5=(
struct Cyc_Absyn_Stdcall_att_struct*)_tmp1D1;if(_tmp1D5->tag != 1)goto _LLC5;}_LLC4:
goto _LLC6;_LLC5: {struct Cyc_Absyn_Cdecl_att_struct*_tmp1D6=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmp1D1;if(_tmp1D6->tag != 2)goto _LLC7;}_LLC6: goto _LLC8;_LLC7: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp1D7=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp1D1;if(_tmp1D7->tag != 4)goto
_LLC9;}_LLC8: goto _LLCA;_LLC9: {struct Cyc_Absyn_Pure_att_struct*_tmp1D8=(struct
Cyc_Absyn_Pure_att_struct*)_tmp1D1;if(_tmp1D8->tag != 22)goto _LLCB;}_LLCA: goto
_LLCC;_LLCB: {struct Cyc_Absyn_Format_att_struct*_tmp1D9=(struct Cyc_Absyn_Format_att_struct*)
_tmp1D1;if(_tmp1D9->tag != 19)goto _LLCD;}_LLCC: goto _LLCE;_LLCD: {struct Cyc_Absyn_Const_att_struct*
_tmp1DA=(struct Cyc_Absyn_Const_att_struct*)_tmp1D1;if(_tmp1DA->tag != 5)goto _LLCF;}
_LLCE: return 1;_LLCF: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp1DB=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp1D1;if(_tmp1DB->tag != 21)goto _LLD1;}_LLD0: return 1;_LLD1: {struct Cyc_Absyn_Initializes_att_struct*
_tmp1DC=(struct Cyc_Absyn_Initializes_att_struct*)_tmp1D1;if(_tmp1DC->tag != 20)
goto _LLD3;}_LLD2: return 1;_LLD3:;_LLD4: return 0;_LLBE:;}static char _tmp1DD[3]="f0";
static struct _dyneither_ptr Cyc_Absyn_f0={_tmp1DD,_tmp1DD,_tmp1DD + 3};static struct
_dyneither_ptr*Cyc_Absyn_field_names_v[1]={& Cyc_Absyn_f0};static struct
_dyneither_ptr Cyc_Absyn_field_names={(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(
void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(void*)((struct
_dyneither_ptr**)Cyc_Absyn_field_names_v + 1)};struct _dyneither_ptr*Cyc_Absyn_fieldname(
int i);static void _tmp3BF(unsigned int*fsz,unsigned int*_tmp3BE,unsigned int*
_tmp3BD,struct _dyneither_ptr***_tmp3BB){for(*_tmp3BE=0;*_tmp3BE < *_tmp3BD;(*
_tmp3BE)++){struct Cyc_Int_pa_struct _tmp3B9;void*_tmp3B8[1];const char*_tmp3B7;
struct _dyneither_ptr*_tmp3B6;(*_tmp3BB)[*_tmp3BE]=*_tmp3BE < *fsz?*((struct
_dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct
_dyneither_ptr*),(int)*_tmp3BE)):((_tmp3B6=_cycalloc(sizeof(*_tmp3B6)),((_tmp3B6[
0]=(struct _dyneither_ptr)((_tmp3B9.tag=1,((_tmp3B9.f1=(unsigned long)((int)*
_tmp3BE),((_tmp3B8[0]=& _tmp3B9,Cyc_aprintf(((_tmp3B7="f%d",_tag_dyneither(
_tmp3B7,sizeof(char),4))),_tag_dyneither(_tmp3B8,sizeof(void*),1)))))))),_tmp3B6))));}}
struct _dyneither_ptr*Cyc_Absyn_fieldname(int i){unsigned int fsz=
_get_dyneither_size(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*));if(i >= 
fsz){unsigned int _tmp3BE;unsigned int _tmp3BD;struct _dyneither_ptr _tmp3BC;struct
_dyneither_ptr**_tmp3BB;unsigned int _tmp3BA;Cyc_Absyn_field_names=((_tmp3BA=(
unsigned int)(i + 1),((_tmp3BB=(struct _dyneither_ptr**)_cycalloc(_check_times(
sizeof(struct _dyneither_ptr*),_tmp3BA)),((_tmp3BC=_tag_dyneither(_tmp3BB,sizeof(
struct _dyneither_ptr*),_tmp3BA),((((_tmp3BD=_tmp3BA,_tmp3BF(& fsz,& _tmp3BE,&
_tmp3BD,& _tmp3BB))),_tmp3BC))))))));}return*((struct _dyneither_ptr**)
_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),i));}
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU info);struct
_tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU info){union Cyc_Absyn_AggrInfoU
_tmp1E6=info;struct _tuple2 _tmp1E7;enum Cyc_Absyn_AggrKind _tmp1E8;struct _tuple0*
_tmp1E9;struct Cyc_Absyn_Aggrdecl**_tmp1EA;struct Cyc_Absyn_Aggrdecl*_tmp1EB;
struct Cyc_Absyn_Aggrdecl _tmp1EC;enum Cyc_Absyn_AggrKind _tmp1ED;struct _tuple0*
_tmp1EE;_LLD6: if((_tmp1E6.UnknownAggr).tag != 1)goto _LLD8;_tmp1E7=(struct _tuple2)(
_tmp1E6.UnknownAggr).val;_tmp1E8=_tmp1E7.f1;_tmp1E9=_tmp1E7.f2;_LLD7: {struct
_tuple10 _tmp3C0;return(_tmp3C0.f1=_tmp1E8,((_tmp3C0.f2=_tmp1E9,_tmp3C0)));}_LLD8:
if((_tmp1E6.KnownAggr).tag != 2)goto _LLD5;_tmp1EA=(struct Cyc_Absyn_Aggrdecl**)(
_tmp1E6.KnownAggr).val;_tmp1EB=*_tmp1EA;_tmp1EC=*_tmp1EB;_tmp1ED=_tmp1EC.kind;
_tmp1EE=_tmp1EC.name;_LLD9: {struct _tuple10 _tmp3C1;return(_tmp3C1.f1=_tmp1ED,((
_tmp3C1.f2=_tmp1EE,_tmp3C1)));}_LLD5:;}struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info){union Cyc_Absyn_AggrInfoU _tmp1F1=info;struct _tuple2
_tmp1F2;struct Cyc_Absyn_Aggrdecl**_tmp1F3;struct Cyc_Absyn_Aggrdecl*_tmp1F4;_LLDB:
if((_tmp1F1.UnknownAggr).tag != 1)goto _LLDD;_tmp1F2=(struct _tuple2)(_tmp1F1.UnknownAggr).val;
_LLDC: {const char*_tmp3C4;void*_tmp3C3;(_tmp3C3=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3C4="unchecked aggrdecl",
_tag_dyneither(_tmp3C4,sizeof(char),19))),_tag_dyneither(_tmp3C3,sizeof(void*),0)));}
_LLDD: if((_tmp1F1.KnownAggr).tag != 2)goto _LLDA;_tmp1F3=(struct Cyc_Absyn_Aggrdecl**)(
_tmp1F1.KnownAggr).val;_tmp1F4=*_tmp1F3;_LLDE: return _tmp1F4;_LLDA:;}int Cyc_Absyn_is_union_type(
void*t);int Cyc_Absyn_is_union_type(void*t){void*_tmp1F7=Cyc_Tcutil_compress(t);
enum Cyc_Absyn_AggrKind _tmp1F9;struct Cyc_Absyn_AggrInfo _tmp1FB;union Cyc_Absyn_AggrInfoU
_tmp1FC;_LLE0: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1F8=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1F7;if(_tmp1F8->tag != 13)goto _LLE2;else{_tmp1F9=_tmp1F8->f1;if(_tmp1F9 != Cyc_Absyn_UnionA)
goto _LLE2;}}_LLE1: return 1;_LLE2: {struct Cyc_Absyn_AggrType_struct*_tmp1FA=(
struct Cyc_Absyn_AggrType_struct*)_tmp1F7;if(_tmp1FA->tag != 12)goto _LLE4;else{
_tmp1FB=_tmp1FA->f1;_tmp1FC=_tmp1FB.aggr_info;}}_LLE3: return(Cyc_Absyn_aggr_kinded_name(
_tmp1FC)).f1 == Cyc_Absyn_UnionA;_LLE4:;_LLE5: return 0;_LLDF:;}int Cyc_Absyn_is_nontagged_union_type(
void*t);int Cyc_Absyn_is_nontagged_union_type(void*t){void*_tmp1FD=Cyc_Tcutil_compress(
t);enum Cyc_Absyn_AggrKind _tmp1FF;struct Cyc_Absyn_AggrInfo _tmp201;union Cyc_Absyn_AggrInfoU
_tmp202;_LLE7: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1FE=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1FD;if(_tmp1FE->tag != 13)goto _LLE9;else{_tmp1FF=_tmp1FE->f1;if(_tmp1FF != Cyc_Absyn_UnionA)
goto _LLE9;}}_LLE8: return 1;_LLE9: {struct Cyc_Absyn_AggrType_struct*_tmp200=(
struct Cyc_Absyn_AggrType_struct*)_tmp1FD;if(_tmp200->tag != 12)goto _LLEB;else{
_tmp201=_tmp200->f1;_tmp202=_tmp201.aggr_info;}}_LLEA: {union Cyc_Absyn_AggrInfoU
_tmp203=_tmp202;struct Cyc_Absyn_Aggrdecl**_tmp204;struct Cyc_Absyn_Aggrdecl*
_tmp205;struct _tuple2 _tmp206;enum Cyc_Absyn_AggrKind _tmp207;struct Cyc_Core_Opt*
_tmp208;struct _tuple2 _tmp209;enum Cyc_Absyn_AggrKind _tmp20A;struct Cyc_Core_Opt*
_tmp20B;struct Cyc_Core_Opt _tmp20C;int _tmp20D;_LLEE: if((_tmp203.KnownAggr).tag != 
2)goto _LLF0;_tmp204=(struct Cyc_Absyn_Aggrdecl**)(_tmp203.KnownAggr).val;_tmp205=*
_tmp204;_LLEF: return _tmp205->kind == Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp205->impl))->tagged == 0;_LLF0: if((_tmp203.UnknownAggr).tag != 1)
goto _LLF2;_tmp206=(struct _tuple2)(_tmp203.UnknownAggr).val;_tmp207=_tmp206.f1;
_tmp208=_tmp206.f3;if(_tmp208 != 0)goto _LLF2;_LLF1: return _tmp207 == Cyc_Absyn_UnionA;
_LLF2: if((_tmp203.UnknownAggr).tag != 1)goto _LLED;_tmp209=(struct _tuple2)(_tmp203.UnknownAggr).val;
_tmp20A=_tmp209.f1;_tmp20B=_tmp209.f3;if(_tmp20B == 0)goto _LLED;_tmp20C=*_tmp20B;
_tmp20D=(int)_tmp20C.v;_LLF3: return _tmp20A == Cyc_Absyn_UnionA  && !_tmp20D;_LLED:;}
_LLEB:;_LLEC: return 0;_LLE6:;}int Cyc_Absyn_is_aggr_type(void*t);int Cyc_Absyn_is_aggr_type(
void*t){void*_tmp20E=Cyc_Tcutil_compress(t);_LLF5: {struct Cyc_Absyn_DatatypeType_struct*
_tmp20F=(struct Cyc_Absyn_DatatypeType_struct*)_tmp20E;if(_tmp20F->tag != 3)goto
_LLF7;}_LLF6: goto _LLF8;_LLF7: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp210=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp20E;if(_tmp210->tag != 4)goto _LLF9;}
_LLF8: goto _LLFA;_LLF9: {struct Cyc_Absyn_TupleType_struct*_tmp211=(struct Cyc_Absyn_TupleType_struct*)
_tmp20E;if(_tmp211->tag != 11)goto _LLFB;}_LLFA: goto _LLFC;_LLFB: {struct Cyc_Absyn_AggrType_struct*
_tmp212=(struct Cyc_Absyn_AggrType_struct*)_tmp20E;if(_tmp212->tag != 12)goto _LLFD;}
_LLFC: goto _LLFE;_LLFD: {struct Cyc_Absyn_AnonAggrType_struct*_tmp213=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp20E;if(_tmp213->tag != 13)goto _LLFF;}_LLFE: return 1;_LLFF:;_LL100: return 0;
_LLF4:;}struct Cyc_Absyn_Porton_d_struct Cyc_Absyn_Porton_d_val={14};struct Cyc_Absyn_Portoff_d_struct
Cyc_Absyn_Portoff_d_val={15};int Cyc_Absyn_porting_c_code=0;int Cyc_Absyn_no_regions=
0;
