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
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_fputc(int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};int Cyc_file_string_write(struct Cyc___cycFILE*,struct
_dyneither_ptr src,int src_offset,int max_count);struct Cyc_Core_Opt{void*v;};extern
char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{char*
tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
4];struct Cyc_List_Nth_struct{char*tag;};int Cyc_List_exists(int(*pred)(void*),
struct Cyc_List_List*x);unsigned long Cyc_strlen(struct _dyneither_ptr s);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct
Cyc_Position_Segment;struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};struct _dyneither_ptr Cyc_Position_get_line_directive(struct Cyc_Position_Segment*
loc);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*);struct _tuple0{union
Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};enum Cyc_Absyn_Scope{Cyc_Absyn_Static
 = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC
 = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;
int q_restrict;int real_const;struct Cyc_Position_Segment*loc;};enum Cyc_Absyn_Size_of{
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
Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_struct{
int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_struct{int tag;struct
Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int
identity;void*kind;};struct Cyc_Absyn_DynEither_b_struct{int tag;};struct Cyc_Absyn_Upper_b_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*
ptr_loc;struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*zt_loc;};
struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*
bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};
struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts
ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{int tag;struct Cyc_Absyn_Exp*f1;
};struct Cyc_Absyn_Region_ptrqual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_struct{
int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_struct{
int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_struct{
int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{int tag;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
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
zt_loc;};struct Cyc_Absyn_VoidType_struct{int tag;};struct Cyc_Absyn_Evar_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;}
;struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_struct{
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
int tag;int f1;};struct Cyc_Absyn_Pure_att_struct{int tag;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _dyneither_ptr f1;};extern struct Cyc_Absyn_Stdcall_att_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_struct Cyc_Absyn_Cdecl_att_val;extern struct Cyc_Absyn_Fastcall_att_struct
Cyc_Absyn_Fastcall_att_val;extern struct Cyc_Absyn_Unused_att_struct Cyc_Absyn_Unused_att_val;
struct Cyc_Absyn_Carray_mod_struct{int tag;union Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;struct Cyc_Absyn_Exp*f1;union
Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*f3;};struct Cyc_Absyn_Pointer_mod_struct{
int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{
int tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};struct
_union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};
struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _tuple4{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};
struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple6 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr
val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times
 = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,
Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte
 = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor
 = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,
Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc
 = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{
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
f2;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*loc;struct Cyc_List_List*
non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_struct{int tag;}
;struct Cyc_Absyn_Var_p_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*
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
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
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
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
extern void*Cyc_Absyn_bounds_one;struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);struct _dyneither_ptr
Cyc_Absyn_attribute2string(void*);struct _tuple9{enum Cyc_Absyn_AggrKind f1;struct
_tuple0*f2;};struct _tuple9 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);int Cyc_Absynpp_is_anon_aggrtype(void*t);extern
struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);struct _dyneither_ptr Cyc_Absynpp_string_escape(
struct _dyneither_ptr);struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop
p);int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple10{struct
_dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct _tuple7*Cyc_Absynpp_arg_mk_opt(
struct _tuple10*arg);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*
f3;};struct _tuple11 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual
tq,void*t);struct _tuple12{unsigned int f1;int f2;};struct _tuple12 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char
Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct
Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};
extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};extern
char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct
Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
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
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};extern
enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;static int Cyc_Absyndump_expand_typedefs;
static int Cyc_Absyndump_qvar_to_Cids;static int Cyc_Absyndump_add_cyc_prefix;static
int Cyc_Absyndump_generate_line_directives;static int Cyc_Absyndump_to_VC;void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params*fs);void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*
fs){Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;Cyc_Absyndump_qvar_to_Cids=
fs->qvar_to_Cids;Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;Cyc_Absyndump_to_VC=
fs->to_VC;Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_set_params(fs);}void Cyc_Absyndump_dumptyp(void*);void Cyc_Absyndump_dumpntyp(
void*t);void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumpexp_prec(
int,struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*);void
Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*,int expstmt);void Cyc_Absyndump_dumpvardecl(
struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*);void Cyc_Absyndump_dumptms(struct Cyc_List_List*tms,void(*f)(
void*),void*a);void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(
void*),void*);void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields);void
Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields);void Cyc_Absyndump_dumploc(
struct Cyc_Position_Segment*);struct Cyc___cycFILE**Cyc_Absyndump_dump_file=& Cyc_stdout;
void Cyc_Absyndump_ignore(void*x);void Cyc_Absyndump_ignore(void*x){return;}static
unsigned int Cyc_Absyndump_pos=0;static char Cyc_Absyndump_prev_char='x';int Cyc_Absyndump_need_space_before();
int Cyc_Absyndump_need_space_before(){switch(Cyc_Absyndump_prev_char){case '{':
_LL0: goto _LL1;case '}': _LL1: goto _LL2;case '(': _LL2: goto _LL3;case ')': _LL3: goto _LL4;
case '[': _LL4: goto _LL5;case ']': _LL5: goto _LL6;case ';': _LL6: goto _LL7;case ',': _LL7:
goto _LL8;case '=': _LL8: goto _LL9;case '?': _LL9: goto _LLA;case '!': _LLA: goto _LLB;case ' ':
_LLB: goto _LLC;case '\n': _LLC: goto _LLD;case '*': _LLD: return 0;default: _LLE: return 1;}}
void Cyc_Absyndump_dump(struct _dyneither_ptr s);void Cyc_Absyndump_dump(struct
_dyneither_ptr s){int sz=(int)Cyc_strlen((struct _dyneither_ptr)s);Cyc_Absyndump_pos
+=sz;if(Cyc_Absyndump_pos > 80){Cyc_Absyndump_pos=(unsigned int)sz;Cyc_fputc((int)'\n',*
Cyc_Absyndump_dump_file);}else{if(Cyc_Absyndump_need_space_before())Cyc_fputc((
int)' ',*Cyc_Absyndump_dump_file);}if(sz >= 1){Cyc_Absyndump_prev_char=*((const
char*)_check_dyneither_subscript(s,sizeof(char),sz - 1));Cyc_file_string_write(*
Cyc_Absyndump_dump_file,s,0,sz);}}void Cyc_Absyndump_dump_nospace(struct
_dyneither_ptr s);void Cyc_Absyndump_dump_nospace(struct _dyneither_ptr s){int sz=(
int)Cyc_strlen((struct _dyneither_ptr)s);Cyc_Absyndump_pos +=sz;if(sz >= 1){Cyc_file_string_write(*
Cyc_Absyndump_dump_file,s,0,sz);Cyc_Absyndump_prev_char=*((const char*)
_check_dyneither_subscript(s,sizeof(char),sz - 1));}}void Cyc_Absyndump_dump_char(
int c);void Cyc_Absyndump_dump_char(int c){++ Cyc_Absyndump_pos;Cyc_fputc(c,*Cyc_Absyndump_dump_file);
Cyc_Absyndump_prev_char=(char)c;}void Cyc_Absyndump_dumploc(struct Cyc_Position_Segment*
loc);void Cyc_Absyndump_dumploc(struct Cyc_Position_Segment*loc){if(loc == 0)
return;if(!Cyc_Absyndump_generate_line_directives)return;{struct _dyneither_ptr
_tmp0=Cyc_Position_get_line_directive(loc);Cyc_Absyndump_dump(_tmp0);};}void Cyc_Absyndump_dump_str(
struct _dyneither_ptr*s);void Cyc_Absyndump_dump_str(struct _dyneither_ptr*s){Cyc_Absyndump_dump(*
s);}void Cyc_Absyndump_dump_semi();void Cyc_Absyndump_dump_semi(){Cyc_Absyndump_dump_char((
int)';');}void Cyc_Absyndump_dump_sep(void(*f)(void*),struct Cyc_List_List*l,
struct _dyneither_ptr sep);void Cyc_Absyndump_dump_sep(void(*f)(void*),struct Cyc_List_List*
l,struct _dyneither_ptr sep){if(l == 0)return;for(0;l->tl != 0;l=l->tl){f((void*)l->hd);
Cyc_Absyndump_dump_nospace(sep);}f((void*)l->hd);}void Cyc_Absyndump_dump_sep_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr sep);
void Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*
l,struct _dyneither_ptr sep){if(l == 0)return;for(0;l->tl != 0;l=l->tl){f(env,(void*)
l->hd);Cyc_Absyndump_dump_nospace(sep);}f(env,(void*)l->hd);}void Cyc_Absyndump_group(
void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep);void Cyc_Absyndump_group(void(*f)(void*),
struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct
_dyneither_ptr sep){Cyc_Absyndump_dump_nospace(start);Cyc_Absyndump_dump_sep(f,l,
sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_group_c(void(*f)(void*,
void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep);void Cyc_Absyndump_group_c(void(*f)(
void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep){Cyc_Absyndump_dump_nospace(start);Cyc_Absyndump_dump_sep_c(
f,env,l,sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_egroup(void(*f)(
void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,
struct _dyneither_ptr sep);void Cyc_Absyndump_egroup(void(*f)(void*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){if(
l != 0)Cyc_Absyndump_group(f,l,start,end,sep);}void Cyc_Absyndump_dumpqvar(struct
_tuple0*v);void Cyc_Absyndump_dumpqvar(struct _tuple0*v){struct Cyc_List_List*_tmp1=
0;struct _dyneither_ptr**prefix=0;{union Cyc_Absyn_Nmspace _tmp2=(*v).f1;int _tmp3;
struct Cyc_List_List*_tmp4;struct Cyc_List_List*_tmp5;_LL11: if((_tmp2.Loc_n).tag != 
3)goto _LL13;_tmp3=(int)(_tmp2.Loc_n).val;_LL12: _tmp4=0;goto _LL14;_LL13: if((_tmp2.Rel_n).tag
!= 1)goto _LL15;_tmp4=(struct Cyc_List_List*)(_tmp2.Rel_n).val;_LL14: _tmp1=_tmp4;
goto _LL10;_LL15: if((_tmp2.Abs_n).tag != 2)goto _LL10;_tmp5=(struct Cyc_List_List*)(
_tmp2.Abs_n).val;_LL16: if(Cyc_Absyndump_qvar_to_Cids  && Cyc_Absyndump_add_cyc_prefix)
prefix=(struct _dyneither_ptr**)& Cyc_Absynpp_cyc_stringptr;_tmp1=_tmp5;goto _LL10;
_LL10:;}if(prefix != 0){Cyc_Absyndump_dump_str(*prefix);if(Cyc_Absyndump_qvar_to_Cids)
Cyc_Absyndump_dump_char((int)'_');else{const char*_tmp397;Cyc_Absyndump_dump_nospace(((
_tmp397="::",_tag_dyneither(_tmp397,sizeof(char),3))));}}if(_tmp1 != 0){Cyc_Absyndump_dump_nospace(*((
struct _dyneither_ptr*)_tmp1->hd));for(_tmp1=_tmp1->tl;_tmp1 != 0;_tmp1=_tmp1->tl){
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{const char*
_tmp398;Cyc_Absyndump_dump_nospace(((_tmp398="::",_tag_dyneither(_tmp398,sizeof(
char),3))));}Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmp1->hd));}
if(Cyc_Absyndump_qvar_to_Cids){const char*_tmp399;Cyc_Absyndump_dump_nospace(((
_tmp399="_",_tag_dyneither(_tmp399,sizeof(char),2))));}else{const char*_tmp39A;
Cyc_Absyndump_dump_nospace(((_tmp39A="::",_tag_dyneither(_tmp39A,sizeof(char),3))));}
Cyc_Absyndump_dump_nospace(*(*v).f2);}else{if(prefix != 0)Cyc_Absyndump_dump_nospace(*(*
v).f2);else{Cyc_Absyndump_dump_str((*v).f2);}}}void Cyc_Absyndump_dumptq(struct
Cyc_Absyn_Tqual tq);void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){if(tq.q_restrict){
const char*_tmp39B;Cyc_Absyndump_dump(((_tmp39B="restrict",_tag_dyneither(_tmp39B,
sizeof(char),9))));}if(tq.q_volatile){const char*_tmp39C;Cyc_Absyndump_dump(((
_tmp39C="volatile",_tag_dyneither(_tmp39C,sizeof(char),9))));}if(tq.print_const){
const char*_tmp39D;Cyc_Absyndump_dump(((_tmp39D="const",_tag_dyneither(_tmp39D,
sizeof(char),6))));}}void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope sc);void Cyc_Absyndump_dumpscope(
enum Cyc_Absyn_Scope sc){switch(sc){case Cyc_Absyn_Static: _LL17:{const char*_tmp39E;
Cyc_Absyndump_dump(((_tmp39E="static",_tag_dyneither(_tmp39E,sizeof(char),7))));}
return;case Cyc_Absyn_Public: _LL18: return;case Cyc_Absyn_Extern: _LL19:{const char*
_tmp39F;Cyc_Absyndump_dump(((_tmp39F="extern",_tag_dyneither(_tmp39F,sizeof(char),
7))));}return;case Cyc_Absyn_ExternC: _LL1A:{const char*_tmp3A0;Cyc_Absyndump_dump(((
_tmp3A0="extern \"C\"",_tag_dyneither(_tmp3A0,sizeof(char),11))));}return;case
Cyc_Absyn_Abstract: _LL1B:{const char*_tmp3A1;Cyc_Absyndump_dump(((_tmp3A1="abstract",
_tag_dyneither(_tmp3A1,sizeof(char),9))));}return;case Cyc_Absyn_Register: _LL1C:{
const char*_tmp3A2;Cyc_Absyndump_dump(((_tmp3A2="register",_tag_dyneither(_tmp3A2,
sizeof(char),9))));}return;}}void Cyc_Absyndump_dumpkind(struct Cyc_Absyn_Kind*ka);
void Cyc_Absyndump_dumpkind(struct Cyc_Absyn_Kind*ka){struct Cyc_Absyn_Kind _tmp13;
enum Cyc_Absyn_KindQual _tmp14;enum Cyc_Absyn_AliasQual _tmp15;struct Cyc_Absyn_Kind*
_tmp12=ka;_tmp13=*_tmp12;_tmp14=_tmp13.kind;_tmp15=_tmp13.aliasqual;switch(
_tmp14){case Cyc_Absyn_AnyKind: _LL1E:{const char*_tmp3A3;Cyc_Absyndump_dump(((
_tmp3A3="A",_tag_dyneither(_tmp3A3,sizeof(char),2))));}return;case Cyc_Absyn_MemKind:
_LL1F:{const char*_tmp3A4;Cyc_Absyndump_dump(((_tmp3A4="M",_tag_dyneither(_tmp3A4,
sizeof(char),2))));}return;case Cyc_Absyn_BoxKind: _LL20:{const char*_tmp3A5;Cyc_Absyndump_dump(((
_tmp3A5="B",_tag_dyneither(_tmp3A5,sizeof(char),2))));}return;case Cyc_Absyn_RgnKind:
_LL21: switch(_tmp15){case Cyc_Absyn_Aliasable: _LL23:{const char*_tmp3A6;Cyc_Absyndump_dump(((
_tmp3A6="R",_tag_dyneither(_tmp3A6,sizeof(char),2))));}return;case Cyc_Absyn_Unique:
_LL24:{const char*_tmp3A7;Cyc_Absyndump_dump(((_tmp3A7="UR",_tag_dyneither(
_tmp3A7,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL25:{const char*_tmp3A8;
Cyc_Absyndump_dump(((_tmp3A8="TR",_tag_dyneither(_tmp3A8,sizeof(char),3))));}
return;}case Cyc_Absyn_EffKind: _LL22:{const char*_tmp3A9;Cyc_Absyndump_dump(((
_tmp3A9="E",_tag_dyneither(_tmp3A9,sizeof(char),2))));}return;case Cyc_Absyn_IntKind:
_LL27:{const char*_tmp3AA;Cyc_Absyndump_dump(((_tmp3AA="I",_tag_dyneither(_tmp3AA,
sizeof(char),2))));}return;}}void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind
k);void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind k){switch(k){case Cyc_Absyn_StructA:
_LL29:{const char*_tmp3AB;Cyc_Absyndump_dump(((_tmp3AB="struct",_tag_dyneither(
_tmp3AB,sizeof(char),7))));}return;case Cyc_Absyn_UnionA: _LL2A:{const char*_tmp3AC;
Cyc_Absyndump_dump(((_tmp3AC="union",_tag_dyneither(_tmp3AC,sizeof(char),6))));}
return;}}void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts);void Cyc_Absyndump_dumptps(
struct Cyc_List_List*ts){const char*_tmp3AF;const char*_tmp3AE;const char*_tmp3AD;((
void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptyp,
ts,((_tmp3AD="<",_tag_dyneither(_tmp3AD,sizeof(char),2))),((_tmp3AE=">",
_tag_dyneither(_tmp3AE,sizeof(char),2))),((_tmp3AF=",",_tag_dyneither(_tmp3AF,
sizeof(char),2))));}void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv);void Cyc_Absyndump_dumptvar(
struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(tv->name);}void Cyc_Absyndump_dumpkindedtvar(
struct Cyc_Absyn_Tvar*tv);void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*
tv){Cyc_Absyndump_dump_str(tv->name);{void*_tmp23=Cyc_Absyn_compress_kb(tv->kind);
struct Cyc_Absyn_Kind*_tmp25;struct Cyc_Absyn_Kind*_tmp29;_LL2D: {struct Cyc_Absyn_Eq_kb_struct*
_tmp24=(struct Cyc_Absyn_Eq_kb_struct*)_tmp23;if(_tmp24->tag != 0)goto _LL2F;else{
_tmp25=_tmp24->f1;if(_tmp25 != Cyc_Absyn_BoxKind)goto _LL2F;}}_LL2E: goto _LL30;
_LL2F: {struct Cyc_Absyn_Less_kb_struct*_tmp26=(struct Cyc_Absyn_Less_kb_struct*)
_tmp23;if(_tmp26->tag != 2)goto _LL31;}_LL30: goto _LL32;_LL31: {struct Cyc_Absyn_Unknown_kb_struct*
_tmp27=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp23;if(_tmp27->tag != 1)goto _LL33;}
_LL32:{const char*_tmp3B0;Cyc_Absyndump_dump(((_tmp3B0="::?",_tag_dyneither(
_tmp3B0,sizeof(char),4))));}goto _LL2C;_LL33: {struct Cyc_Absyn_Eq_kb_struct*
_tmp28=(struct Cyc_Absyn_Eq_kb_struct*)_tmp23;if(_tmp28->tag != 0)goto _LL2C;else{
_tmp29=_tmp28->f1;}}_LL34:{const char*_tmp3B1;Cyc_Absyndump_dump(((_tmp3B1="::",
_tag_dyneither(_tmp3B1,sizeof(char),3))));}Cyc_Absyndump_dumpkind(_tmp29);goto
_LL2C;_LL2C:;};}void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs);void Cyc_Absyndump_dumptvars(
struct Cyc_List_List*tvs){const char*_tmp3B4;const char*_tmp3B3;const char*_tmp3B2;((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptvar,tvs,((_tmp3B2="<",_tag_dyneither(_tmp3B2,sizeof(char),2))),((
_tmp3B3=">",_tag_dyneither(_tmp3B3,sizeof(char),2))),((_tmp3B4=",",
_tag_dyneither(_tmp3B4,sizeof(char),2))));}void Cyc_Absyndump_dumpkindedtvars(
struct Cyc_List_List*tvs);void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*
tvs){const char*_tmp3B7;const char*_tmp3B6;const char*_tmp3B5;((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,
tvs,((_tmp3B5="<",_tag_dyneither(_tmp3B5,sizeof(char),2))),((_tmp3B6=">",
_tag_dyneither(_tmp3B6,sizeof(char),2))),((_tmp3B7=",",_tag_dyneither(_tmp3B7,
sizeof(char),2))));}struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;};void Cyc_Absyndump_dumparg(
struct _tuple13*pr);void Cyc_Absyndump_dumparg(struct _tuple13*pr){((void(*)(struct
Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*pr).f1,(*pr).f2,(
void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpargs(struct Cyc_List_List*
ts);void Cyc_Absyndump_dumpargs(struct Cyc_List_List*ts){const char*_tmp3BA;const
char*_tmp3B9;const char*_tmp3B8;((void(*)(void(*f)(struct _tuple13*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumparg,ts,((_tmp3B8="(",_tag_dyneither(_tmp3B8,sizeof(char),2))),((
_tmp3B9=")",_tag_dyneither(_tmp3B9,sizeof(char),2))),((_tmp3BA=",",
_tag_dyneither(_tmp3BA,sizeof(char),2))));}void Cyc_Absyndump_dump_callconv(
struct Cyc_List_List*atts);void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*
atts){for(0;atts != 0;atts=atts->tl){void*_tmp35=(void*)atts->hd;_LL36: {struct
Cyc_Absyn_Stdcall_att_struct*_tmp36=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp35;
if(_tmp36->tag != 1)goto _LL38;}_LL37:{const char*_tmp3BB;Cyc_Absyndump_dump(((
_tmp3BB="_stdcall",_tag_dyneither(_tmp3BB,sizeof(char),9))));}return;_LL38: {
struct Cyc_Absyn_Cdecl_att_struct*_tmp37=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmp35;if(_tmp37->tag != 2)goto _LL3A;}_LL39:{const char*_tmp3BC;Cyc_Absyndump_dump(((
_tmp3BC="_cdecl",_tag_dyneither(_tmp3BC,sizeof(char),7))));}return;_LL3A: {
struct Cyc_Absyn_Fastcall_att_struct*_tmp38=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp35;if(_tmp38->tag != 3)goto _LL3C;}_LL3B:{const char*_tmp3BD;Cyc_Absyndump_dump(((
_tmp3BD="_fastcall",_tag_dyneither(_tmp3BD,sizeof(char),10))));}return;_LL3C:;
_LL3D: goto _LL35;_LL35:;}}void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*
atts);void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){int hasatt=0;{
struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){void*_tmp3C=(
void*)((struct Cyc_List_List*)_check_null(atts))->hd;_LL3F: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp3D=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp3C;if(_tmp3D->tag != 1)goto _LL41;}
_LL40: goto _LL42;_LL41: {struct Cyc_Absyn_Cdecl_att_struct*_tmp3E=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmp3C;if(_tmp3E->tag != 2)goto _LL43;}_LL42: goto _LL44;_LL43: {struct Cyc_Absyn_Fastcall_att_struct*
_tmp3F=(struct Cyc_Absyn_Fastcall_att_struct*)_tmp3C;if(_tmp3F->tag != 3)goto _LL45;}
_LL44: goto _LL3E;_LL45:;_LL46: hasatt=1;goto _LL3E;_LL3E:;}}if(!hasatt)return;{
const char*_tmp3BE;Cyc_Absyndump_dump(((_tmp3BE="__declspec(",_tag_dyneither(
_tmp3BE,sizeof(char),12))));}for(0;atts != 0;atts=atts->tl){void*_tmp41=(void*)
atts->hd;_LL48: {struct Cyc_Absyn_Stdcall_att_struct*_tmp42=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp41;if(_tmp42->tag != 1)goto _LL4A;}_LL49: goto _LL4B;_LL4A: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp43=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp41;if(_tmp43->tag != 2)goto _LL4C;}
_LL4B: goto _LL4D;_LL4C: {struct Cyc_Absyn_Fastcall_att_struct*_tmp44=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp41;if(_tmp44->tag != 3)goto _LL4E;}_LL4D: goto _LL47;_LL4E:;_LL4F: Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string((void*)atts->hd));goto _LL47;_LL47:;}Cyc_Absyndump_dump_char((
int)')');}void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts);void Cyc_Absyndump_dumpatts(
struct Cyc_List_List*atts){if(atts == 0)return;switch(Cyc_Cyclone_c_compiler){case
Cyc_Cyclone_Gcc_c: _LL50:{const char*_tmp3BF;Cyc_Absyndump_dump(((_tmp3BF=" __attribute__((",
_tag_dyneither(_tmp3BF,sizeof(char),17))));}for(0;atts != 0;atts=atts->tl){Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string((void*)atts->hd));if(atts->tl != 0){const char*_tmp3C0;
Cyc_Absyndump_dump(((_tmp3C0=",",_tag_dyneither(_tmp3C0,sizeof(char),2))));}}{
const char*_tmp3C1;Cyc_Absyndump_dump(((_tmp3C1=")) ",_tag_dyneither(_tmp3C1,
sizeof(char),4))));}return;case Cyc_Cyclone_Vc_c: _LL51: Cyc_Absyndump_dump_noncallconv(
atts);return;}}int Cyc_Absyndump_next_is_pointer(struct Cyc_List_List*tms);int Cyc_Absyndump_next_is_pointer(
struct Cyc_List_List*tms){if(tms == 0)return 0;{void*_tmp48=(void*)tms->hd;_LL54: {
struct Cyc_Absyn_Pointer_mod_struct*_tmp49=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp48;if(_tmp49->tag != 2)goto _LL56;}_LL55: return 1;_LL56:;_LL57: return 0;_LL53:;};}
static void Cyc_Absyndump_dumprgn(void*t);static void Cyc_Absyndump_dumprgn(void*t){
void*_tmp4A=Cyc_Tcutil_compress(t);_LL59: {struct Cyc_Absyn_HeapRgn_struct*_tmp4B=(
struct Cyc_Absyn_HeapRgn_struct*)_tmp4A;if(_tmp4B->tag != 21)goto _LL5B;}_LL5A:{
const char*_tmp3C2;Cyc_Absyndump_dump(((_tmp3C2="`H",_tag_dyneither(_tmp3C2,
sizeof(char),3))));}goto _LL58;_LL5B:;_LL5C: Cyc_Absyndump_dumpntyp(t);goto _LL58;
_LL58:;}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};static
struct _tuple14 Cyc_Absyndump_effects_split(void*t);static struct _tuple14 Cyc_Absyndump_effects_split(
void*t){struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;{void*_tmp4D=
Cyc_Tcutil_compress(t);void*_tmp4F;struct Cyc_List_List*_tmp51;_LL5E: {struct Cyc_Absyn_AccessEff_struct*
_tmp4E=(struct Cyc_Absyn_AccessEff_struct*)_tmp4D;if(_tmp4E->tag != 23)goto _LL60;
else{_tmp4F=(void*)_tmp4E->f1;}}_LL5F:{struct Cyc_List_List*_tmp3C3;rgions=((
_tmp3C3=_cycalloc(sizeof(*_tmp3C3)),((_tmp3C3->hd=_tmp4F,((_tmp3C3->tl=rgions,
_tmp3C3))))));}goto _LL5D;_LL60: {struct Cyc_Absyn_JoinEff_struct*_tmp50=(struct
Cyc_Absyn_JoinEff_struct*)_tmp4D;if(_tmp50->tag != 24)goto _LL62;else{_tmp51=
_tmp50->f1;}}_LL61: for(0;_tmp51 != 0;_tmp51=_tmp51->tl){struct Cyc_List_List*
_tmp54;struct Cyc_List_List*_tmp55;struct _tuple14 _tmp53=Cyc_Absyndump_effects_split((
void*)_tmp51->hd);_tmp54=_tmp53.f1;_tmp55=_tmp53.f2;rgions=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp54,rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp55,effects);}goto _LL5D;_LL62:;_LL63:{struct Cyc_List_List*
_tmp3C4;effects=((_tmp3C4=_cycalloc(sizeof(*_tmp3C4)),((_tmp3C4->hd=t,((_tmp3C4->tl=
effects,_tmp3C4))))));}goto _LL5D;_LL5D:;}{struct _tuple14 _tmp3C5;return(_tmp3C5.f1=
rgions,((_tmp3C5.f2=effects,_tmp3C5)));};}static void Cyc_Absyndump_dumpeff(void*t);
static void Cyc_Absyndump_dumpeff(void*t){struct Cyc_List_List*_tmp59;struct Cyc_List_List*
_tmp5A;struct _tuple14 _tmp58=Cyc_Absyndump_effects_split(t);_tmp59=_tmp58.f1;
_tmp5A=_tmp58.f2;_tmp59=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp59);_tmp5A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp5A);for(0;_tmp5A != 0;_tmp5A=_tmp5A->tl){Cyc_Absyndump_dumpntyp((void*)_tmp5A->hd);
Cyc_Absyndump_dump_char((int)'+');}Cyc_Absyndump_dump_char((int)'{');for(0;
_tmp59 != 0;_tmp59=_tmp59->tl){Cyc_Absyndump_dumprgn((void*)_tmp59->hd);if(_tmp59->tl
!= 0)Cyc_Absyndump_dump_char((int)',');}Cyc_Absyndump_dump_char((int)'}');}void
Cyc_Absyndump_dumpntyp(void*t);void Cyc_Absyndump_dumpntyp(void*t){void*_tmp5B=t;
struct Cyc_Absyn_Tvar*_tmp61;struct Cyc_Core_Opt*_tmp63;struct Cyc_Core_Opt*_tmp64;
int _tmp65;struct Cyc_Core_Opt*_tmp67;struct Cyc_Core_Opt*_tmp68;struct Cyc_Core_Opt
_tmp69;void*_tmp6A;int _tmp6B;struct Cyc_Absyn_DatatypeInfo _tmp6D;union Cyc_Absyn_DatatypeInfoU
_tmp6E;struct Cyc_List_List*_tmp6F;struct Cyc_Absyn_DatatypeFieldInfo _tmp71;union
Cyc_Absyn_DatatypeFieldInfoU _tmp72;struct Cyc_List_List*_tmp73;enum Cyc_Absyn_Sign
_tmp75;enum Cyc_Absyn_Size_of _tmp76;enum Cyc_Absyn_Sign _tmp78;enum Cyc_Absyn_Size_of
_tmp79;enum Cyc_Absyn_Sign _tmp7B;enum Cyc_Absyn_Size_of _tmp7C;enum Cyc_Absyn_Sign
_tmp7E;enum Cyc_Absyn_Size_of _tmp7F;enum Cyc_Absyn_Sign _tmp81;enum Cyc_Absyn_Size_of
_tmp82;enum Cyc_Absyn_Sign _tmp84;enum Cyc_Absyn_Size_of _tmp85;enum Cyc_Absyn_Sign
_tmp87;enum Cyc_Absyn_Size_of _tmp88;enum Cyc_Absyn_Sign _tmp8A;enum Cyc_Absyn_Size_of
_tmp8B;enum Cyc_Absyn_Sign _tmp8D;enum Cyc_Absyn_Size_of _tmp8E;enum Cyc_Absyn_Sign
_tmp90;enum Cyc_Absyn_Size_of _tmp91;enum Cyc_Absyn_Sign _tmp93;enum Cyc_Absyn_Size_of
_tmp94;enum Cyc_Absyn_Sign _tmp96;enum Cyc_Absyn_Size_of _tmp97;enum Cyc_Absyn_Sign
_tmp99;enum Cyc_Absyn_Size_of _tmp9A;enum Cyc_Absyn_Sign _tmp9C;enum Cyc_Absyn_Size_of
_tmp9D;enum Cyc_Absyn_Sign _tmp9F;enum Cyc_Absyn_Size_of _tmpA0;int _tmpA3;struct
Cyc_List_List*_tmpA5;struct Cyc_Absyn_AggrInfo _tmpA7;union Cyc_Absyn_AggrInfoU
_tmpA8;struct Cyc_List_List*_tmpA9;enum Cyc_Absyn_AggrKind _tmpAB;struct Cyc_List_List*
_tmpAC;struct _tuple0*_tmpAE;struct Cyc_List_List*_tmpB0;struct _tuple0*_tmpB2;
struct Cyc_List_List*_tmpB3;struct Cyc_Absyn_Exp*_tmpB5;void*_tmpB7;void*_tmpB9;
void*_tmpBA;void*_tmpBC;_LL65: {struct Cyc_Absyn_ArrayType_struct*_tmp5C=(struct
Cyc_Absyn_ArrayType_struct*)_tmp5B;if(_tmp5C->tag != 9)goto _LL67;}_LL66: goto _LL68;
_LL67: {struct Cyc_Absyn_FnType_struct*_tmp5D=(struct Cyc_Absyn_FnType_struct*)
_tmp5B;if(_tmp5D->tag != 10)goto _LL69;}_LL68: goto _LL6A;_LL69: {struct Cyc_Absyn_PointerType_struct*
_tmp5E=(struct Cyc_Absyn_PointerType_struct*)_tmp5B;if(_tmp5E->tag != 5)goto _LL6B;}
_LL6A: return;_LL6B: {struct Cyc_Absyn_VoidType_struct*_tmp5F=(struct Cyc_Absyn_VoidType_struct*)
_tmp5B;if(_tmp5F->tag != 0)goto _LL6D;}_LL6C:{const char*_tmp3C6;Cyc_Absyndump_dump(((
_tmp3C6="void",_tag_dyneither(_tmp3C6,sizeof(char),5))));}return;_LL6D: {struct
Cyc_Absyn_VarType_struct*_tmp60=(struct Cyc_Absyn_VarType_struct*)_tmp5B;if(
_tmp60->tag != 2)goto _LL6F;else{_tmp61=_tmp60->f1;}}_LL6E: Cyc_Absyndump_dump_str(
_tmp61->name);return;_LL6F: {struct Cyc_Absyn_Evar_struct*_tmp62=(struct Cyc_Absyn_Evar_struct*)
_tmp5B;if(_tmp62->tag != 1)goto _LL71;else{_tmp63=_tmp62->f1;_tmp64=_tmp62->f2;if(
_tmp64 != 0)goto _LL71;_tmp65=_tmp62->f3;}}_LL70:{const char*_tmp3C7;Cyc_Absyndump_dump(((
_tmp3C7="%",_tag_dyneither(_tmp3C7,sizeof(char),2))));}if(_tmp63 == 0){const char*
_tmp3C8;Cyc_Absyndump_dump(((_tmp3C8="?",_tag_dyneither(_tmp3C8,sizeof(char),2))));}
else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)_tmp63->v);}{const char*
_tmp3CC;void*_tmp3CB[1];struct Cyc_Int_pa_struct _tmp3CA;Cyc_Absyndump_dump((
struct _dyneither_ptr)((_tmp3CA.tag=1,((_tmp3CA.f1=(unsigned long)_tmp65,((
_tmp3CB[0]=& _tmp3CA,Cyc_aprintf(((_tmp3CC="(%d)",_tag_dyneither(_tmp3CC,sizeof(
char),5))),_tag_dyneither(_tmp3CB,sizeof(void*),1)))))))));}return;_LL71: {
struct Cyc_Absyn_Evar_struct*_tmp66=(struct Cyc_Absyn_Evar_struct*)_tmp5B;if(
_tmp66->tag != 1)goto _LL73;else{_tmp67=_tmp66->f1;_tmp68=_tmp66->f2;if(_tmp68 == 0)
goto _LL73;_tmp69=*_tmp68;_tmp6A=(void*)_tmp69.v;_tmp6B=_tmp66->f3;}}_LL72: Cyc_Absyndump_dumpntyp(
_tmp6A);return;_LL73: {struct Cyc_Absyn_DatatypeType_struct*_tmp6C=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp5B;if(_tmp6C->tag != 3)goto _LL75;else{_tmp6D=_tmp6C->f1;_tmp6E=_tmp6D.datatype_info;
_tmp6F=_tmp6D.targs;}}_LL74:{union Cyc_Absyn_DatatypeInfoU _tmpC8=_tmp6E;struct Cyc_Absyn_UnknownDatatypeInfo
_tmpC9;struct _tuple0*_tmpCA;int _tmpCB;struct Cyc_Absyn_Datatypedecl**_tmpCC;
struct Cyc_Absyn_Datatypedecl*_tmpCD;struct Cyc_Absyn_Datatypedecl _tmpCE;struct
_tuple0*_tmpCF;int _tmpD0;_LLB8: if((_tmpC8.UnknownDatatype).tag != 1)goto _LLBA;
_tmpC9=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC8.UnknownDatatype).val;_tmpCA=
_tmpC9.name;_tmpCB=_tmpC9.is_extensible;_LLB9: _tmpCF=_tmpCA;_tmpD0=_tmpCB;goto
_LLBB;_LLBA: if((_tmpC8.KnownDatatype).tag != 2)goto _LLB7;_tmpCC=(struct Cyc_Absyn_Datatypedecl**)(
_tmpC8.KnownDatatype).val;_tmpCD=*_tmpCC;_tmpCE=*_tmpCD;_tmpCF=_tmpCE.name;
_tmpD0=_tmpCE.is_extensible;_LLBB: if(_tmpD0){const char*_tmp3CD;Cyc_Absyndump_dump(((
_tmp3CD="@extensible ",_tag_dyneither(_tmp3CD,sizeof(char),13))));}{const char*
_tmp3CE;Cyc_Absyndump_dump(((_tmp3CE="datatype ",_tag_dyneither(_tmp3CE,sizeof(
char),10))));}Cyc_Absyndump_dumpqvar(_tmpCF);Cyc_Absyndump_dumptps(_tmp6F);goto
_LLB7;_LLB7:;}goto _LL64;_LL75: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp70=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp5B;if(_tmp70->tag != 4)goto _LL77;
else{_tmp71=_tmp70->f1;_tmp72=_tmp71.field_info;_tmp73=_tmp71.targs;}}_LL76:{
union Cyc_Absyn_DatatypeFieldInfoU _tmpD3=_tmp72;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpD4;struct _tuple0*_tmpD5;struct _tuple0*_tmpD6;int _tmpD7;struct _tuple1 _tmpD8;
struct Cyc_Absyn_Datatypedecl*_tmpD9;struct Cyc_Absyn_Datatypedecl _tmpDA;struct
_tuple0*_tmpDB;int _tmpDC;struct Cyc_Absyn_Datatypefield*_tmpDD;struct Cyc_Absyn_Datatypefield
_tmpDE;struct _tuple0*_tmpDF;_LLBD: if((_tmpD3.UnknownDatatypefield).tag != 1)goto
_LLBF;_tmpD4=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpD3.UnknownDatatypefield).val;
_tmpD5=_tmpD4.datatype_name;_tmpD6=_tmpD4.field_name;_tmpD7=_tmpD4.is_extensible;
_LLBE: _tmpDB=_tmpD5;_tmpDC=_tmpD7;_tmpDF=_tmpD6;goto _LLC0;_LLBF: if((_tmpD3.KnownDatatypefield).tag
!= 2)goto _LLBC;_tmpD8=(struct _tuple1)(_tmpD3.KnownDatatypefield).val;_tmpD9=
_tmpD8.f1;_tmpDA=*_tmpD9;_tmpDB=_tmpDA.name;_tmpDC=_tmpDA.is_extensible;_tmpDD=
_tmpD8.f2;_tmpDE=*_tmpDD;_tmpDF=_tmpDE.name;_LLC0: if(_tmpDC){const char*_tmp3CF;
Cyc_Absyndump_dump(((_tmp3CF="@extensible ",_tag_dyneither(_tmp3CF,sizeof(char),
13))));}{const char*_tmp3D0;Cyc_Absyndump_dump(((_tmp3D0="datatype ",
_tag_dyneither(_tmp3D0,sizeof(char),10))));}Cyc_Absyndump_dumpqvar(_tmpDB);{
const char*_tmp3D1;Cyc_Absyndump_dump(((_tmp3D1=".",_tag_dyneither(_tmp3D1,
sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmpDF);Cyc_Absyndump_dumptps(_tmp73);
goto _LLBC;_LLBC:;}goto _LL64;_LL77: {struct Cyc_Absyn_IntType_struct*_tmp74=(
struct Cyc_Absyn_IntType_struct*)_tmp5B;if(_tmp74->tag != 6)goto _LL79;else{_tmp75=
_tmp74->f1;if(_tmp75 != Cyc_Absyn_None)goto _LL79;_tmp76=_tmp74->f2;if(_tmp76 != 
Cyc_Absyn_Int_sz)goto _LL79;}}_LL78: goto _LL7A;_LL79: {struct Cyc_Absyn_IntType_struct*
_tmp77=(struct Cyc_Absyn_IntType_struct*)_tmp5B;if(_tmp77->tag != 6)goto _LL7B;
else{_tmp78=_tmp77->f1;if(_tmp78 != Cyc_Absyn_Signed)goto _LL7B;_tmp79=_tmp77->f2;
if(_tmp79 != Cyc_Absyn_Int_sz)goto _LL7B;}}_LL7A:{const char*_tmp3D2;Cyc_Absyndump_dump(((
_tmp3D2="int",_tag_dyneither(_tmp3D2,sizeof(char),4))));}return;_LL7B: {struct
Cyc_Absyn_IntType_struct*_tmp7A=(struct Cyc_Absyn_IntType_struct*)_tmp5B;if(
_tmp7A->tag != 6)goto _LL7D;else{_tmp7B=_tmp7A->f1;if(_tmp7B != Cyc_Absyn_None)goto
_LL7D;_tmp7C=_tmp7A->f2;if(_tmp7C != Cyc_Absyn_Long_sz)goto _LL7D;}}_LL7C: goto
_LL7E;_LL7D: {struct Cyc_Absyn_IntType_struct*_tmp7D=(struct Cyc_Absyn_IntType_struct*)
_tmp5B;if(_tmp7D->tag != 6)goto _LL7F;else{_tmp7E=_tmp7D->f1;if(_tmp7E != Cyc_Absyn_Signed)
goto _LL7F;_tmp7F=_tmp7D->f2;if(_tmp7F != Cyc_Absyn_Long_sz)goto _LL7F;}}_LL7E:{
const char*_tmp3D3;Cyc_Absyndump_dump(((_tmp3D3="long",_tag_dyneither(_tmp3D3,
sizeof(char),5))));}return;_LL7F: {struct Cyc_Absyn_IntType_struct*_tmp80=(struct
Cyc_Absyn_IntType_struct*)_tmp5B;if(_tmp80->tag != 6)goto _LL81;else{_tmp81=_tmp80->f1;
if(_tmp81 != Cyc_Absyn_None)goto _LL81;_tmp82=_tmp80->f2;if(_tmp82 != Cyc_Absyn_Char_sz)
goto _LL81;}}_LL80:{const char*_tmp3D4;Cyc_Absyndump_dump(((_tmp3D4="char",
_tag_dyneither(_tmp3D4,sizeof(char),5))));}return;_LL81: {struct Cyc_Absyn_IntType_struct*
_tmp83=(struct Cyc_Absyn_IntType_struct*)_tmp5B;if(_tmp83->tag != 6)goto _LL83;
else{_tmp84=_tmp83->f1;if(_tmp84 != Cyc_Absyn_Signed)goto _LL83;_tmp85=_tmp83->f2;
if(_tmp85 != Cyc_Absyn_Char_sz)goto _LL83;}}_LL82:{const char*_tmp3D5;Cyc_Absyndump_dump(((
_tmp3D5="signed char",_tag_dyneither(_tmp3D5,sizeof(char),12))));}return;_LL83: {
struct Cyc_Absyn_IntType_struct*_tmp86=(struct Cyc_Absyn_IntType_struct*)_tmp5B;
if(_tmp86->tag != 6)goto _LL85;else{_tmp87=_tmp86->f1;if(_tmp87 != Cyc_Absyn_Unsigned)
goto _LL85;_tmp88=_tmp86->f2;if(_tmp88 != Cyc_Absyn_Char_sz)goto _LL85;}}_LL84:{
const char*_tmp3D6;Cyc_Absyndump_dump(((_tmp3D6="unsigned char",_tag_dyneither(
_tmp3D6,sizeof(char),14))));}return;_LL85: {struct Cyc_Absyn_IntType_struct*
_tmp89=(struct Cyc_Absyn_IntType_struct*)_tmp5B;if(_tmp89->tag != 6)goto _LL87;
else{_tmp8A=_tmp89->f1;if(_tmp8A != Cyc_Absyn_None)goto _LL87;_tmp8B=_tmp89->f2;
if(_tmp8B != Cyc_Absyn_Short_sz)goto _LL87;}}_LL86: goto _LL88;_LL87: {struct Cyc_Absyn_IntType_struct*
_tmp8C=(struct Cyc_Absyn_IntType_struct*)_tmp5B;if(_tmp8C->tag != 6)goto _LL89;
else{_tmp8D=_tmp8C->f1;if(_tmp8D != Cyc_Absyn_Signed)goto _LL89;_tmp8E=_tmp8C->f2;
if(_tmp8E != Cyc_Absyn_Short_sz)goto _LL89;}}_LL88:{const char*_tmp3D7;Cyc_Absyndump_dump(((
_tmp3D7="short",_tag_dyneither(_tmp3D7,sizeof(char),6))));}return;_LL89: {struct
Cyc_Absyn_IntType_struct*_tmp8F=(struct Cyc_Absyn_IntType_struct*)_tmp5B;if(
_tmp8F->tag != 6)goto _LL8B;else{_tmp90=_tmp8F->f1;if(_tmp90 != Cyc_Absyn_Unsigned)
goto _LL8B;_tmp91=_tmp8F->f2;if(_tmp91 != Cyc_Absyn_Short_sz)goto _LL8B;}}_LL8A:{
const char*_tmp3D8;Cyc_Absyndump_dump(((_tmp3D8="unsigned short",_tag_dyneither(
_tmp3D8,sizeof(char),15))));}return;_LL8B: {struct Cyc_Absyn_IntType_struct*
_tmp92=(struct Cyc_Absyn_IntType_struct*)_tmp5B;if(_tmp92->tag != 6)goto _LL8D;
else{_tmp93=_tmp92->f1;if(_tmp93 != Cyc_Absyn_Unsigned)goto _LL8D;_tmp94=_tmp92->f2;
if(_tmp94 != Cyc_Absyn_Int_sz)goto _LL8D;}}_LL8C:{const char*_tmp3D9;Cyc_Absyndump_dump(((
_tmp3D9="unsigned int",_tag_dyneither(_tmp3D9,sizeof(char),13))));}return;_LL8D: {
struct Cyc_Absyn_IntType_struct*_tmp95=(struct Cyc_Absyn_IntType_struct*)_tmp5B;
if(_tmp95->tag != 6)goto _LL8F;else{_tmp96=_tmp95->f1;if(_tmp96 != Cyc_Absyn_Unsigned)
goto _LL8F;_tmp97=_tmp95->f2;if(_tmp97 != Cyc_Absyn_Long_sz)goto _LL8F;}}_LL8E:{
const char*_tmp3DA;Cyc_Absyndump_dump(((_tmp3DA="unsigned long",_tag_dyneither(
_tmp3DA,sizeof(char),14))));}return;_LL8F: {struct Cyc_Absyn_IntType_struct*
_tmp98=(struct Cyc_Absyn_IntType_struct*)_tmp5B;if(_tmp98->tag != 6)goto _LL91;
else{_tmp99=_tmp98->f1;if(_tmp99 != Cyc_Absyn_None)goto _LL91;_tmp9A=_tmp98->f2;
if(_tmp9A != Cyc_Absyn_LongLong_sz)goto _LL91;}}_LL90: goto _LL92;_LL91: {struct Cyc_Absyn_IntType_struct*
_tmp9B=(struct Cyc_Absyn_IntType_struct*)_tmp5B;if(_tmp9B->tag != 6)goto _LL93;
else{_tmp9C=_tmp9B->f1;if(_tmp9C != Cyc_Absyn_Signed)goto _LL93;_tmp9D=_tmp9B->f2;
if(_tmp9D != Cyc_Absyn_LongLong_sz)goto _LL93;}}_LL92: switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LLC1:{const char*_tmp3DB;Cyc_Absyndump_dump(((_tmp3DB="long long",
_tag_dyneither(_tmp3DB,sizeof(char),10))));}return;case Cyc_Cyclone_Vc_c: _LLC2:{
const char*_tmp3DC;Cyc_Absyndump_dump(((_tmp3DC="__int64",_tag_dyneither(_tmp3DC,
sizeof(char),8))));}return;}_LL93: {struct Cyc_Absyn_IntType_struct*_tmp9E=(
struct Cyc_Absyn_IntType_struct*)_tmp5B;if(_tmp9E->tag != 6)goto _LL95;else{_tmp9F=
_tmp9E->f1;if(_tmp9F != Cyc_Absyn_Unsigned)goto _LL95;_tmpA0=_tmp9E->f2;if(_tmpA0
!= Cyc_Absyn_LongLong_sz)goto _LL95;}}_LL94: switch(Cyc_Cyclone_c_compiler){case
Cyc_Cyclone_Vc_c: _LLC4:{const char*_tmp3DD;Cyc_Absyndump_dump(((_tmp3DD="unsigned __int64",
_tag_dyneither(_tmp3DD,sizeof(char),17))));}return;case Cyc_Cyclone_Gcc_c: _LLC5:{
const char*_tmp3DE;Cyc_Absyndump_dump(((_tmp3DE="unsigned long long",
_tag_dyneither(_tmp3DE,sizeof(char),19))));}return;}_LL95: {struct Cyc_Absyn_FloatType_struct*
_tmpA1=(struct Cyc_Absyn_FloatType_struct*)_tmp5B;if(_tmpA1->tag != 7)goto _LL97;}
_LL96:{const char*_tmp3DF;Cyc_Absyndump_dump(((_tmp3DF="float",_tag_dyneither(
_tmp3DF,sizeof(char),6))));}return;_LL97: {struct Cyc_Absyn_DoubleType_struct*
_tmpA2=(struct Cyc_Absyn_DoubleType_struct*)_tmp5B;if(_tmpA2->tag != 8)goto _LL99;
else{_tmpA3=_tmpA2->f1;}}_LL98: if(_tmpA3){const char*_tmp3E0;Cyc_Absyndump_dump(((
_tmp3E0="long double",_tag_dyneither(_tmp3E0,sizeof(char),12))));}else{const char*
_tmp3E1;Cyc_Absyndump_dump(((_tmp3E1="double",_tag_dyneither(_tmp3E1,sizeof(char),
7))));}return;_LL99: {struct Cyc_Absyn_TupleType_struct*_tmpA4=(struct Cyc_Absyn_TupleType_struct*)
_tmp5B;if(_tmpA4->tag != 11)goto _LL9B;else{_tmpA5=_tmpA4->f1;}}_LL9A: Cyc_Absyndump_dump_char((
int)'$');Cyc_Absyndump_dumpargs(_tmpA5);return;_LL9B: {struct Cyc_Absyn_AggrType_struct*
_tmpA6=(struct Cyc_Absyn_AggrType_struct*)_tmp5B;if(_tmpA6->tag != 12)goto _LL9D;
else{_tmpA7=_tmpA6->f1;_tmpA8=_tmpA7.aggr_info;_tmpA9=_tmpA7.targs;}}_LL9C: {
enum Cyc_Absyn_AggrKind _tmpF4;struct _tuple0*_tmpF5;struct _tuple9 _tmpF3=Cyc_Absyn_aggr_kinded_name(
_tmpA8);_tmpF4=_tmpF3.f1;_tmpF5=_tmpF3.f2;Cyc_Absyndump_dumpaggr_kind(_tmpF4);
Cyc_Absyndump_dumpqvar(_tmpF5);Cyc_Absyndump_dumptps(_tmpA9);return;}_LL9D: {
struct Cyc_Absyn_AnonAggrType_struct*_tmpAA=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp5B;if(_tmpAA->tag != 13)goto _LL9F;else{_tmpAB=_tmpAA->f1;_tmpAC=_tmpAA->f2;}}
_LL9E: Cyc_Absyndump_dumpaggr_kind(_tmpAB);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(
_tmpAC);Cyc_Absyndump_dump_char((int)'}');return;_LL9F: {struct Cyc_Absyn_EnumType_struct*
_tmpAD=(struct Cyc_Absyn_EnumType_struct*)_tmp5B;if(_tmpAD->tag != 14)goto _LLA1;
else{_tmpAE=_tmpAD->f1;}}_LLA0:{const char*_tmp3E2;Cyc_Absyndump_dump(((_tmp3E2="enum ",
_tag_dyneither(_tmp3E2,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(_tmpAE);
return;_LLA1: {struct Cyc_Absyn_AnonEnumType_struct*_tmpAF=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp5B;if(_tmpAF->tag != 15)goto _LLA3;else{_tmpB0=_tmpAF->f1;}}_LLA2:{const char*
_tmp3E3;Cyc_Absyndump_dump(((_tmp3E3="enum {",_tag_dyneither(_tmp3E3,sizeof(char),
7))));}Cyc_Absyndump_dumpenumfields(_tmpB0);{const char*_tmp3E4;Cyc_Absyndump_dump(((
_tmp3E4="}",_tag_dyneither(_tmp3E4,sizeof(char),2))));}return;_LLA3: {struct Cyc_Absyn_TypedefType_struct*
_tmpB1=(struct Cyc_Absyn_TypedefType_struct*)_tmp5B;if(_tmpB1->tag != 18)goto _LLA5;
else{_tmpB2=_tmpB1->f1;_tmpB3=_tmpB1->f2;}}_LLA4:(Cyc_Absyndump_dumpqvar(_tmpB2),
Cyc_Absyndump_dumptps(_tmpB3));return;_LLA5: {struct Cyc_Absyn_ValueofType_struct*
_tmpB4=(struct Cyc_Absyn_ValueofType_struct*)_tmp5B;if(_tmpB4->tag != 19)goto _LLA7;
else{_tmpB5=_tmpB4->f1;}}_LLA6:{const char*_tmp3E5;Cyc_Absyndump_dump(((_tmp3E5="valueof_t(",
_tag_dyneither(_tmp3E5,sizeof(char),11))));}Cyc_Absyndump_dumpexp(_tmpB5);{const
char*_tmp3E6;Cyc_Absyndump_dump(((_tmp3E6=")",_tag_dyneither(_tmp3E6,sizeof(char),
2))));}return;_LLA7: {struct Cyc_Absyn_RgnHandleType_struct*_tmpB6=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp5B;if(_tmpB6->tag != 16)goto _LLA9;else{_tmpB7=(void*)_tmpB6->f1;}}_LLA8:{
const char*_tmp3E7;Cyc_Absyndump_dump(((_tmp3E7="region_t<",_tag_dyneither(
_tmp3E7,sizeof(char),10))));}Cyc_Absyndump_dumprgn(_tmpB7);{const char*_tmp3E8;
Cyc_Absyndump_dump(((_tmp3E8=">",_tag_dyneither(_tmp3E8,sizeof(char),2))));}
return;_LLA9: {struct Cyc_Absyn_DynRgnType_struct*_tmpB8=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp5B;if(_tmpB8->tag != 17)goto _LLAB;else{_tmpB9=(void*)_tmpB8->f1;_tmpBA=(void*)
_tmpB8->f2;}}_LLAA:{const char*_tmp3E9;Cyc_Absyndump_dump(((_tmp3E9="dynregion_t<",
_tag_dyneither(_tmp3E9,sizeof(char),13))));}Cyc_Absyndump_dumprgn(_tmpB9);{const
char*_tmp3EA;Cyc_Absyndump_dump(((_tmp3EA=",",_tag_dyneither(_tmp3EA,sizeof(char),
2))));}Cyc_Absyndump_dumprgn(_tmpBA);{const char*_tmp3EB;Cyc_Absyndump_dump(((
_tmp3EB=">",_tag_dyneither(_tmp3EB,sizeof(char),2))));}return;_LLAB: {struct Cyc_Absyn_TagType_struct*
_tmpBB=(struct Cyc_Absyn_TagType_struct*)_tmp5B;if(_tmpBB->tag != 20)goto _LLAD;
else{_tmpBC=(void*)_tmpBB->f1;}}_LLAC:{const char*_tmp3EC;Cyc_Absyndump_dump(((
_tmp3EC="tag_t<",_tag_dyneither(_tmp3EC,sizeof(char),7))));}Cyc_Absyndump_dumpntyp(
_tmpBC);{const char*_tmp3ED;Cyc_Absyndump_dump(((_tmp3ED=">",_tag_dyneither(
_tmp3ED,sizeof(char),2))));}return;_LLAD: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpBD=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp5B;if(_tmpBD->tag != 22)goto _LLAF;}
_LLAE:{const char*_tmp3EE;Cyc_Absyndump_dump(((_tmp3EE="`U",_tag_dyneither(
_tmp3EE,sizeof(char),3))));}goto _LL64;_LLAF: {struct Cyc_Absyn_HeapRgn_struct*
_tmpBE=(struct Cyc_Absyn_HeapRgn_struct*)_tmp5B;if(_tmpBE->tag != 21)goto _LLB1;}
_LLB0: goto _LLB2;_LLB1: {struct Cyc_Absyn_AccessEff_struct*_tmpBF=(struct Cyc_Absyn_AccessEff_struct*)
_tmp5B;if(_tmpBF->tag != 23)goto _LLB3;}_LLB2: goto _LLB4;_LLB3: {struct Cyc_Absyn_RgnsEff_struct*
_tmpC0=(struct Cyc_Absyn_RgnsEff_struct*)_tmp5B;if(_tmpC0->tag != 25)goto _LLB5;}
_LLB4: goto _LLB6;_LLB5: {struct Cyc_Absyn_JoinEff_struct*_tmpC1=(struct Cyc_Absyn_JoinEff_struct*)
_tmp5B;if(_tmpC1->tag != 24)goto _LL64;}_LLB6: return;_LL64:;}void Cyc_Absyndump_dumpvaropt(
struct Cyc_Core_Opt*vo);void Cyc_Absyndump_dumpvaropt(struct Cyc_Core_Opt*vo){if(vo
!= 0)Cyc_Absyndump_dump_str((struct _dyneither_ptr*)vo->v);}void Cyc_Absyndump_dumpfunarg(
struct _tuple7*t);void Cyc_Absyndump_dumpfunarg(struct _tuple7*t){((void(*)(struct
Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Core_Opt*),struct Cyc_Core_Opt*))Cyc_Absyndump_dumptqtd)((*
t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}struct _tuple15{void*f1;void*f2;
};void Cyc_Absyndump_dump_rgncmp(struct _tuple15*cmp);void Cyc_Absyndump_dump_rgncmp(
struct _tuple15*cmp){struct _tuple15 _tmp104;void*_tmp105;void*_tmp106;struct
_tuple15*_tmp103=cmp;_tmp104=*_tmp103;_tmp105=_tmp104.f1;_tmp106=_tmp104.f2;Cyc_Absyndump_dumpeff(
_tmp105);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn(_tmp106);}void
Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po);void Cyc_Absyndump_dump_rgnpo(
struct Cyc_List_List*rgn_po){const char*_tmp3EF;((void(*)(void(*f)(struct _tuple15*),
struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,
rgn_po,((_tmp3EF=",",_tag_dyneither(_tmp3EF,sizeof(char),2))));}void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po);void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){Cyc_Absyndump_dump_char((
int)'(');for(0;args != 0;args=args->tl){Cyc_Absyndump_dumpfunarg((struct _tuple7*)
args->hd);if((args->tl != 0  || c_varargs) || cyc_varargs != 0)Cyc_Absyndump_dump_char((
int)',');}if(c_varargs){const char*_tmp3F0;Cyc_Absyndump_dump(((_tmp3F0="...",
_tag_dyneither(_tmp3F0,sizeof(char),4))));}else{if(cyc_varargs != 0){struct
_tuple7*_tmp3F1;struct _tuple7*_tmp109=(_tmp3F1=_cycalloc(sizeof(*_tmp3F1)),((
_tmp3F1->f1=cyc_varargs->name,((_tmp3F1->f2=cyc_varargs->tq,((_tmp3F1->f3=
cyc_varargs->type,_tmp3F1)))))));{const char*_tmp3F2;Cyc_Absyndump_dump(((_tmp3F2="...",
_tag_dyneither(_tmp3F2,sizeof(char),4))));}if(cyc_varargs->inject){const char*
_tmp3F3;Cyc_Absyndump_dump(((_tmp3F3=" inject ",_tag_dyneither(_tmp3F3,sizeof(
char),9))));}Cyc_Absyndump_dumpfunarg(_tmp109);}}if(effopt != 0){Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpeff((void*)effopt->v);}if(rgn_po != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dump_rgnpo(rgn_po);}Cyc_Absyndump_dump_char((int)')');}
void Cyc_Absyndump_dumptyp(void*t);void Cyc_Absyndump_dumptyp(void*t){((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(
0),t,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpdesignator(
void*d);void Cyc_Absyndump_dumpdesignator(void*d){void*_tmp10D=d;struct Cyc_Absyn_Exp*
_tmp10F;struct _dyneither_ptr*_tmp111;_LLC8: {struct Cyc_Absyn_ArrayElement_struct*
_tmp10E=(struct Cyc_Absyn_ArrayElement_struct*)_tmp10D;if(_tmp10E->tag != 0)goto
_LLCA;else{_tmp10F=_tmp10E->f1;}}_LLC9:{const char*_tmp3F4;Cyc_Absyndump_dump(((
_tmp3F4=".[",_tag_dyneither(_tmp3F4,sizeof(char),3))));}Cyc_Absyndump_dumpexp(
_tmp10F);Cyc_Absyndump_dump_char((int)']');goto _LLC7;_LLCA: {struct Cyc_Absyn_FieldName_struct*
_tmp110=(struct Cyc_Absyn_FieldName_struct*)_tmp10D;if(_tmp110->tag != 1)goto _LLC7;
else{_tmp111=_tmp110->f1;}}_LLCB: Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp111);goto _LLC7;_LLC7:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};void Cyc_Absyndump_dumpde(struct _tuple16*de);void Cyc_Absyndump_dumpde(struct
_tuple16*de){{const char*_tmp3F7;const char*_tmp3F6;const char*_tmp3F5;((void(*)(
void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,(*
de).f1,((_tmp3F5="",_tag_dyneither(_tmp3F5,sizeof(char),1))),((_tmp3F6="=",
_tag_dyneither(_tmp3F6,sizeof(char),2))),((_tmp3F7="=",_tag_dyneither(_tmp3F7,
sizeof(char),2))));}Cyc_Absyndump_dumpexp((*de).f2);}void Cyc_Absyndump_dumpexps_prec(
int inprec,struct Cyc_List_List*es);void Cyc_Absyndump_dumpexps_prec(int inprec,
struct Cyc_List_List*es){const char*_tmp3FA;const char*_tmp3F9;const char*_tmp3F8;((
void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec,inprec,es,((_tmp3F8="",_tag_dyneither(_tmp3F8,sizeof(
char),1))),((_tmp3F9="",_tag_dyneither(_tmp3F9,sizeof(char),1))),((_tmp3FA=",",
_tag_dyneither(_tmp3FA,sizeof(char),2))));}void Cyc_Absyndump_dumpexp_prec(int
inprec,struct Cyc_Absyn_Exp*e);void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*
e){int myprec=Cyc_Absynpp_exp_prec(e);if(inprec >= myprec){const char*_tmp3FB;Cyc_Absyndump_dump_nospace(((
_tmp3FB="(",_tag_dyneither(_tmp3FB,sizeof(char),2))));}{void*_tmp11A=e->r;union
Cyc_Absyn_Cnst _tmp11C;struct _tuple3 _tmp11D;enum Cyc_Absyn_Sign _tmp11E;char
_tmp11F;union Cyc_Absyn_Cnst _tmp121;struct _tuple4 _tmp122;enum Cyc_Absyn_Sign
_tmp123;short _tmp124;union Cyc_Absyn_Cnst _tmp126;struct _tuple5 _tmp127;enum Cyc_Absyn_Sign
_tmp128;int _tmp129;union Cyc_Absyn_Cnst _tmp12B;struct _tuple5 _tmp12C;enum Cyc_Absyn_Sign
_tmp12D;int _tmp12E;union Cyc_Absyn_Cnst _tmp130;struct _tuple5 _tmp131;enum Cyc_Absyn_Sign
_tmp132;int _tmp133;union Cyc_Absyn_Cnst _tmp135;struct _tuple6 _tmp136;enum Cyc_Absyn_Sign
_tmp137;long long _tmp138;union Cyc_Absyn_Cnst _tmp13A;struct _dyneither_ptr _tmp13B;
union Cyc_Absyn_Cnst _tmp13D;int _tmp13E;union Cyc_Absyn_Cnst _tmp140;struct
_dyneither_ptr _tmp141;struct _tuple0*_tmp143;struct _tuple0*_tmp145;enum Cyc_Absyn_Primop
_tmp147;struct Cyc_List_List*_tmp148;struct Cyc_Absyn_Exp*_tmp14A;struct Cyc_Core_Opt*
_tmp14B;struct Cyc_Absyn_Exp*_tmp14C;struct Cyc_Absyn_Exp*_tmp14E;enum Cyc_Absyn_Incrementor
_tmp14F;struct Cyc_Absyn_Exp*_tmp151;enum Cyc_Absyn_Incrementor _tmp152;struct Cyc_Absyn_Exp*
_tmp154;enum Cyc_Absyn_Incrementor _tmp155;struct Cyc_Absyn_Exp*_tmp157;enum Cyc_Absyn_Incrementor
_tmp158;struct Cyc_Absyn_Exp*_tmp15A;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*
_tmp15C;struct Cyc_Absyn_Exp*_tmp15E;struct Cyc_Absyn_Exp*_tmp15F;struct Cyc_Absyn_Exp*
_tmp161;struct Cyc_Absyn_Exp*_tmp162;struct Cyc_Absyn_Exp*_tmp164;struct Cyc_Absyn_Exp*
_tmp165;struct Cyc_Absyn_Exp*_tmp167;struct Cyc_List_List*_tmp168;struct Cyc_Absyn_Exp*
_tmp16A;struct Cyc_List_List*_tmp16B;struct Cyc_Absyn_Exp*_tmp16D;struct Cyc_Absyn_Exp*
_tmp16F;struct Cyc_Absyn_Exp*_tmp171;void*_tmp173;struct Cyc_Absyn_Exp*_tmp174;
struct Cyc_Absyn_Exp*_tmp176;struct Cyc_Absyn_Exp*_tmp178;struct Cyc_Absyn_Exp*
_tmp179;void*_tmp17B;struct Cyc_Absyn_Exp*_tmp17D;void*_tmp17F;struct Cyc_Absyn_Exp*
_tmp181;struct _dyneither_ptr*_tmp182;void*_tmp184;void*_tmp185;struct
_dyneither_ptr*_tmp187;void*_tmp189;void*_tmp18A;unsigned int _tmp18C;struct Cyc_Absyn_Exp*
_tmp18E;struct Cyc_Absyn_Exp*_tmp190;struct _dyneither_ptr*_tmp191;struct Cyc_Absyn_Exp*
_tmp193;struct _dyneither_ptr*_tmp194;struct Cyc_Absyn_Exp*_tmp196;struct Cyc_Absyn_Exp*
_tmp197;struct Cyc_List_List*_tmp199;struct _tuple7*_tmp19B;struct Cyc_List_List*
_tmp19C;struct Cyc_List_List*_tmp19E;struct Cyc_Absyn_Vardecl*_tmp1A0;struct Cyc_Absyn_Exp*
_tmp1A1;struct Cyc_Absyn_Exp*_tmp1A2;struct _tuple0*_tmp1A4;struct Cyc_List_List*
_tmp1A5;struct Cyc_List_List*_tmp1A6;struct Cyc_List_List*_tmp1A8;struct Cyc_List_List*
_tmp1AA;struct Cyc_Absyn_Datatypefield*_tmp1AB;struct _tuple0*_tmp1AD;struct
_tuple0*_tmp1AF;struct Cyc_Absyn_MallocInfo _tmp1B1;int _tmp1B2;struct Cyc_Absyn_Exp*
_tmp1B3;void**_tmp1B4;struct Cyc_Absyn_Exp*_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B7;
struct Cyc_Absyn_Exp*_tmp1B8;struct Cyc_Core_Opt*_tmp1BA;struct Cyc_List_List*
_tmp1BB;struct Cyc_Absyn_Stmt*_tmp1BD;_LLCD: {struct Cyc_Absyn_Const_e_struct*
_tmp11B=(struct Cyc_Absyn_Const_e_struct*)_tmp11A;if(_tmp11B->tag != 0)goto _LLCF;
else{_tmp11C=_tmp11B->f1;if((_tmp11C.Char_c).tag != 2)goto _LLCF;_tmp11D=(struct
_tuple3)(_tmp11C.Char_c).val;_tmp11E=_tmp11D.f1;_tmp11F=_tmp11D.f2;}}_LLCE: Cyc_Absyndump_dump_char((
int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp11F));Cyc_Absyndump_dump_char((
int)'\'');goto _LLCC;_LLCF: {struct Cyc_Absyn_Const_e_struct*_tmp120=(struct Cyc_Absyn_Const_e_struct*)
_tmp11A;if(_tmp120->tag != 0)goto _LLD1;else{_tmp121=_tmp120->f1;if((_tmp121.Short_c).tag
!= 3)goto _LLD1;_tmp122=(struct _tuple4)(_tmp121.Short_c).val;_tmp123=_tmp122.f1;
_tmp124=_tmp122.f2;}}_LLD0:{const char*_tmp3FF;void*_tmp3FE[1];struct Cyc_Int_pa_struct
_tmp3FD;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp3FD.tag=1,((_tmp3FD.f1=(
unsigned long)((int)_tmp124),((_tmp3FE[0]=& _tmp3FD,Cyc_aprintf(((_tmp3FF="%d",
_tag_dyneither(_tmp3FF,sizeof(char),3))),_tag_dyneither(_tmp3FE,sizeof(void*),1)))))))));}
goto _LLCC;_LLD1: {struct Cyc_Absyn_Const_e_struct*_tmp125=(struct Cyc_Absyn_Const_e_struct*)
_tmp11A;if(_tmp125->tag != 0)goto _LLD3;else{_tmp126=_tmp125->f1;if((_tmp126.Int_c).tag
!= 4)goto _LLD3;_tmp127=(struct _tuple5)(_tmp126.Int_c).val;_tmp128=_tmp127.f1;if(
_tmp128 != Cyc_Absyn_None)goto _LLD3;_tmp129=_tmp127.f2;}}_LLD2: _tmp12E=_tmp129;
goto _LLD4;_LLD3: {struct Cyc_Absyn_Const_e_struct*_tmp12A=(struct Cyc_Absyn_Const_e_struct*)
_tmp11A;if(_tmp12A->tag != 0)goto _LLD5;else{_tmp12B=_tmp12A->f1;if((_tmp12B.Int_c).tag
!= 4)goto _LLD5;_tmp12C=(struct _tuple5)(_tmp12B.Int_c).val;_tmp12D=_tmp12C.f1;if(
_tmp12D != Cyc_Absyn_Signed)goto _LLD5;_tmp12E=_tmp12C.f2;}}_LLD4:{const char*
_tmp403;void*_tmp402[1];struct Cyc_Int_pa_struct _tmp401;Cyc_Absyndump_dump((
struct _dyneither_ptr)((_tmp401.tag=1,((_tmp401.f1=(unsigned long)_tmp12E,((
_tmp402[0]=& _tmp401,Cyc_aprintf(((_tmp403="%d",_tag_dyneither(_tmp403,sizeof(
char),3))),_tag_dyneither(_tmp402,sizeof(void*),1)))))))));}goto _LLCC;_LLD5: {
struct Cyc_Absyn_Const_e_struct*_tmp12F=(struct Cyc_Absyn_Const_e_struct*)_tmp11A;
if(_tmp12F->tag != 0)goto _LLD7;else{_tmp130=_tmp12F->f1;if((_tmp130.Int_c).tag != 
4)goto _LLD7;_tmp131=(struct _tuple5)(_tmp130.Int_c).val;_tmp132=_tmp131.f1;if(
_tmp132 != Cyc_Absyn_Unsigned)goto _LLD7;_tmp133=_tmp131.f2;}}_LLD6:{const char*
_tmp407;void*_tmp406[1];struct Cyc_Int_pa_struct _tmp405;Cyc_Absyndump_dump((
struct _dyneither_ptr)((_tmp405.tag=1,((_tmp405.f1=(unsigned int)_tmp133,((
_tmp406[0]=& _tmp405,Cyc_aprintf(((_tmp407="%u",_tag_dyneither(_tmp407,sizeof(
char),3))),_tag_dyneither(_tmp406,sizeof(void*),1)))))))));}goto _LLCC;_LLD7: {
struct Cyc_Absyn_Const_e_struct*_tmp134=(struct Cyc_Absyn_Const_e_struct*)_tmp11A;
if(_tmp134->tag != 0)goto _LLD9;else{_tmp135=_tmp134->f1;if((_tmp135.LongLong_c).tag
!= 5)goto _LLD9;_tmp136=(struct _tuple6)(_tmp135.LongLong_c).val;_tmp137=_tmp136.f1;
_tmp138=_tmp136.f2;}}_LLD8:{const char*_tmp408;Cyc_Absyndump_dump(((_tmp408="<<FIX LONG LONG CONSTANT>>",
_tag_dyneither(_tmp408,sizeof(char),27))));}goto _LLCC;_LLD9: {struct Cyc_Absyn_Const_e_struct*
_tmp139=(struct Cyc_Absyn_Const_e_struct*)_tmp11A;if(_tmp139->tag != 0)goto _LLDB;
else{_tmp13A=_tmp139->f1;if((_tmp13A.Float_c).tag != 6)goto _LLDB;_tmp13B=(struct
_dyneither_ptr)(_tmp13A.Float_c).val;}}_LLDA: Cyc_Absyndump_dump(_tmp13B);goto
_LLCC;_LLDB: {struct Cyc_Absyn_Const_e_struct*_tmp13C=(struct Cyc_Absyn_Const_e_struct*)
_tmp11A;if(_tmp13C->tag != 0)goto _LLDD;else{_tmp13D=_tmp13C->f1;if((_tmp13D.Null_c).tag
!= 1)goto _LLDD;_tmp13E=(int)(_tmp13D.Null_c).val;}}_LLDC:{const char*_tmp409;Cyc_Absyndump_dump(((
_tmp409="NULL",_tag_dyneither(_tmp409,sizeof(char),5))));}goto _LLCC;_LLDD: {
struct Cyc_Absyn_Const_e_struct*_tmp13F=(struct Cyc_Absyn_Const_e_struct*)_tmp11A;
if(_tmp13F->tag != 0)goto _LLDF;else{_tmp140=_tmp13F->f1;if((_tmp140.String_c).tag
!= 7)goto _LLDF;_tmp141=(struct _dyneither_ptr)(_tmp140.String_c).val;}}_LLDE: Cyc_Absyndump_dump_char((
int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp141));Cyc_Absyndump_dump_char((
int)'"');goto _LLCC;_LLDF: {struct Cyc_Absyn_UnknownId_e_struct*_tmp142=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp11A;if(_tmp142->tag != 2)goto _LLE1;else{_tmp143=_tmp142->f1;}}_LLE0: _tmp145=
_tmp143;goto _LLE2;_LLE1: {struct Cyc_Absyn_Var_e_struct*_tmp144=(struct Cyc_Absyn_Var_e_struct*)
_tmp11A;if(_tmp144->tag != 1)goto _LLE3;else{_tmp145=_tmp144->f1;}}_LLE2: Cyc_Absyndump_dumpqvar(
_tmp145);goto _LLCC;_LLE3: {struct Cyc_Absyn_Primop_e_struct*_tmp146=(struct Cyc_Absyn_Primop_e_struct*)
_tmp11A;if(_tmp146->tag != 3)goto _LLE5;else{_tmp147=_tmp146->f1;_tmp148=_tmp146->f2;}}
_LLE4: {struct _dyneither_ptr _tmp1C9=Cyc_Absynpp_prim2str(_tmp147);switch(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp148)){case 1: _LL135: if(_tmp147 == (
enum Cyc_Absyn_Primop)Cyc_Absyn_Numelts){{const char*_tmp40A;Cyc_Absyndump_dump(((
_tmp40A="numelts(",_tag_dyneither(_tmp40A,sizeof(char),9))));}Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp148))->hd);{const
char*_tmp40B;Cyc_Absyndump_dump(((_tmp40B=")",_tag_dyneither(_tmp40B,sizeof(char),
2))));};}else{Cyc_Absyndump_dump(_tmp1C9);Cyc_Absyndump_dumpexp_prec(myprec,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp148))->hd);}break;
case 2: _LL136: Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp148))->hd);Cyc_Absyndump_dump(_tmp1C9);Cyc_Absyndump_dump_char((
int)' ');Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp148->tl))->hd);break;default: _LL137: {struct Cyc_Core_Failure_struct
_tmp411;const char*_tmp410;struct Cyc_Core_Failure_struct*_tmp40F;(int)_throw((
void*)((_tmp40F=_cycalloc(sizeof(*_tmp40F)),((_tmp40F[0]=((_tmp411.tag=Cyc_Core_Failure,((
_tmp411.f1=((_tmp410="Absyndump -- Bad number of arguments to primop",
_tag_dyneither(_tmp410,sizeof(char),47))),_tmp411)))),_tmp40F)))));}}goto _LLCC;}
_LLE5: {struct Cyc_Absyn_AssignOp_e_struct*_tmp149=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp11A;if(_tmp149->tag != 4)goto _LLE7;else{_tmp14A=_tmp149->f1;_tmp14B=_tmp149->f2;
_tmp14C=_tmp149->f3;}}_LLE6: Cyc_Absyndump_dumpexp_prec(myprec,_tmp14A);if(
_tmp14B != 0)Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)
_tmp14B->v));{const char*_tmp412;Cyc_Absyndump_dump_nospace(((_tmp412="=",
_tag_dyneither(_tmp412,sizeof(char),2))));}Cyc_Absyndump_dumpexp_prec(myprec,
_tmp14C);goto _LLCC;_LLE7: {struct Cyc_Absyn_Increment_e_struct*_tmp14D=(struct Cyc_Absyn_Increment_e_struct*)
_tmp11A;if(_tmp14D->tag != 5)goto _LLE9;else{_tmp14E=_tmp14D->f1;_tmp14F=_tmp14D->f2;
if(_tmp14F != Cyc_Absyn_PreInc)goto _LLE9;}}_LLE8:{const char*_tmp413;Cyc_Absyndump_dump(((
_tmp413="++",_tag_dyneither(_tmp413,sizeof(char),3))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp14E);goto _LLCC;_LLE9: {struct Cyc_Absyn_Increment_e_struct*_tmp150=(
struct Cyc_Absyn_Increment_e_struct*)_tmp11A;if(_tmp150->tag != 5)goto _LLEB;else{
_tmp151=_tmp150->f1;_tmp152=_tmp150->f2;if(_tmp152 != Cyc_Absyn_PreDec)goto _LLEB;}}
_LLEA:{const char*_tmp414;Cyc_Absyndump_dump(((_tmp414="--",_tag_dyneither(
_tmp414,sizeof(char),3))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp151);goto _LLCC;
_LLEB: {struct Cyc_Absyn_Increment_e_struct*_tmp153=(struct Cyc_Absyn_Increment_e_struct*)
_tmp11A;if(_tmp153->tag != 5)goto _LLED;else{_tmp154=_tmp153->f1;_tmp155=_tmp153->f2;
if(_tmp155 != Cyc_Absyn_PostInc)goto _LLED;}}_LLEC: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp154);{const char*_tmp415;Cyc_Absyndump_dump(((_tmp415="++",
_tag_dyneither(_tmp415,sizeof(char),3))));}goto _LLCC;_LLED: {struct Cyc_Absyn_Increment_e_struct*
_tmp156=(struct Cyc_Absyn_Increment_e_struct*)_tmp11A;if(_tmp156->tag != 5)goto
_LLEF;else{_tmp157=_tmp156->f1;_tmp158=_tmp156->f2;if(_tmp158 != Cyc_Absyn_PostDec)
goto _LLEF;}}_LLEE: Cyc_Absyndump_dumpexp_prec(myprec,_tmp157);{const char*_tmp416;
Cyc_Absyndump_dump(((_tmp416="--",_tag_dyneither(_tmp416,sizeof(char),3))));}
goto _LLCC;_LLEF: {struct Cyc_Absyn_Conditional_e_struct*_tmp159=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp11A;if(_tmp159->tag != 6)goto _LLF1;else{_tmp15A=_tmp159->f1;_tmp15B=_tmp159->f2;
_tmp15C=_tmp159->f3;}}_LLF0: Cyc_Absyndump_dumpexp_prec(myprec,_tmp15A);Cyc_Absyndump_dump_char((
int)'?');Cyc_Absyndump_dumpexp_prec(0,_tmp15B);Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp15C);goto _LLCC;_LLF1: {struct Cyc_Absyn_And_e_struct*
_tmp15D=(struct Cyc_Absyn_And_e_struct*)_tmp11A;if(_tmp15D->tag != 7)goto _LLF3;
else{_tmp15E=_tmp15D->f1;_tmp15F=_tmp15D->f2;}}_LLF2: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp15E);{const char*_tmp417;Cyc_Absyndump_dump(((_tmp417=" && ",
_tag_dyneither(_tmp417,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(myprec,
_tmp15F);goto _LLCC;_LLF3: {struct Cyc_Absyn_Or_e_struct*_tmp160=(struct Cyc_Absyn_Or_e_struct*)
_tmp11A;if(_tmp160->tag != 8)goto _LLF5;else{_tmp161=_tmp160->f1;_tmp162=_tmp160->f2;}}
_LLF4: Cyc_Absyndump_dumpexp_prec(myprec,_tmp161);{const char*_tmp418;Cyc_Absyndump_dump(((
_tmp418=" || ",_tag_dyneither(_tmp418,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp162);goto _LLCC;_LLF5: {struct Cyc_Absyn_SeqExp_e_struct*_tmp163=(
struct Cyc_Absyn_SeqExp_e_struct*)_tmp11A;if(_tmp163->tag != 9)goto _LLF7;else{
_tmp164=_tmp163->f1;_tmp165=_tmp163->f2;}}_LLF6: Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp164);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp165);Cyc_Absyndump_dump_char((int)')');goto _LLCC;_LLF7: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp166=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp11A;if(_tmp166->tag != 10)goto
_LLF9;else{_tmp167=_tmp166->f1;_tmp168=_tmp166->f2;}}_LLF8: _tmp16A=_tmp167;
_tmp16B=_tmp168;goto _LLFA;_LLF9: {struct Cyc_Absyn_FnCall_e_struct*_tmp169=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp11A;if(_tmp169->tag != 11)goto _LLFB;else{
_tmp16A=_tmp169->f1;_tmp16B=_tmp169->f2;}}_LLFA: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp16A);{const char*_tmp419;Cyc_Absyndump_dump_nospace(((_tmp419="(",
_tag_dyneither(_tmp419,sizeof(char),2))));}Cyc_Absyndump_dumpexps_prec(20,
_tmp16B);{const char*_tmp41A;Cyc_Absyndump_dump_nospace(((_tmp41A=")",
_tag_dyneither(_tmp41A,sizeof(char),2))));}goto _LLCC;_LLFB: {struct Cyc_Absyn_Throw_e_struct*
_tmp16C=(struct Cyc_Absyn_Throw_e_struct*)_tmp11A;if(_tmp16C->tag != 12)goto _LLFD;
else{_tmp16D=_tmp16C->f1;}}_LLFC:{const char*_tmp41B;Cyc_Absyndump_dump(((_tmp41B="throw",
_tag_dyneither(_tmp41B,sizeof(char),6))));}Cyc_Absyndump_dumpexp_prec(myprec,
_tmp16D);goto _LLCC;_LLFD: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp16E=(
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp11A;if(_tmp16E->tag != 13)goto _LLFF;
else{_tmp16F=_tmp16E->f1;}}_LLFE: _tmp171=_tmp16F;goto _LL100;_LLFF: {struct Cyc_Absyn_Instantiate_e_struct*
_tmp170=(struct Cyc_Absyn_Instantiate_e_struct*)_tmp11A;if(_tmp170->tag != 14)goto
_LL101;else{_tmp171=_tmp170->f1;}}_LL100: Cyc_Absyndump_dumpexp_prec(inprec,
_tmp171);goto _LLCC;_LL101: {struct Cyc_Absyn_Cast_e_struct*_tmp172=(struct Cyc_Absyn_Cast_e_struct*)
_tmp11A;if(_tmp172->tag != 15)goto _LL103;else{_tmp173=(void*)_tmp172->f1;_tmp174=
_tmp172->f2;}}_LL102: Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(
_tmp173);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(myprec,
_tmp174);goto _LLCC;_LL103: {struct Cyc_Absyn_Address_e_struct*_tmp175=(struct Cyc_Absyn_Address_e_struct*)
_tmp11A;if(_tmp175->tag != 16)goto _LL105;else{_tmp176=_tmp175->f1;}}_LL104: Cyc_Absyndump_dump_char((
int)'&');Cyc_Absyndump_dumpexp_prec(myprec,_tmp176);goto _LLCC;_LL105: {struct Cyc_Absyn_New_e_struct*
_tmp177=(struct Cyc_Absyn_New_e_struct*)_tmp11A;if(_tmp177->tag != 17)goto _LL107;
else{_tmp178=_tmp177->f1;_tmp179=_tmp177->f2;}}_LL106:{const char*_tmp41C;Cyc_Absyndump_dump(((
_tmp41C="new ",_tag_dyneither(_tmp41C,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp179);goto _LLCC;_LL107: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp17A=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp11A;if(_tmp17A->tag != 18)goto _LL109;
else{_tmp17B=(void*)_tmp17A->f1;}}_LL108:{const char*_tmp41D;Cyc_Absyndump_dump(((
_tmp41D="sizeof(",_tag_dyneither(_tmp41D,sizeof(char),8))));}Cyc_Absyndump_dumptyp(
_tmp17B);Cyc_Absyndump_dump_char((int)')');goto _LLCC;_LL109: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp17C=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp11A;if(_tmp17C->tag != 19)goto
_LL10B;else{_tmp17D=_tmp17C->f1;}}_LL10A:{const char*_tmp41E;Cyc_Absyndump_dump(((
_tmp41E="sizeof(",_tag_dyneither(_tmp41E,sizeof(char),8))));}Cyc_Absyndump_dumpexp(
_tmp17D);Cyc_Absyndump_dump_char((int)')');goto _LLCC;_LL10B: {struct Cyc_Absyn_Valueof_e_struct*
_tmp17E=(struct Cyc_Absyn_Valueof_e_struct*)_tmp11A;if(_tmp17E->tag != 39)goto
_LL10D;else{_tmp17F=(void*)_tmp17E->f1;}}_LL10C:{const char*_tmp41F;Cyc_Absyndump_dump(((
_tmp41F="valueof(",_tag_dyneither(_tmp41F,sizeof(char),9))));}Cyc_Absyndump_dumptyp(
_tmp17F);Cyc_Absyndump_dump_char((int)')');goto _LLCC;_LL10D: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp180=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp11A;if(_tmp180->tag != 38)goto
_LL10F;else{_tmp181=_tmp180->f1;_tmp182=_tmp180->f2;}}_LL10E:{const char*_tmp420;
Cyc_Absyndump_dump(((_tmp420="tagcheck(",_tag_dyneither(_tmp420,sizeof(char),10))));}
Cyc_Absyndump_dumpexp(_tmp181);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp182);Cyc_Absyndump_dump_char((int)')');goto _LLCC;_LL10F: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp183=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp11A;if(_tmp183->tag != 20)goto
_LL111;else{_tmp184=(void*)_tmp183->f1;_tmp185=(void*)_tmp183->f2;{struct Cyc_Absyn_StructField_struct*
_tmp186=(struct Cyc_Absyn_StructField_struct*)_tmp185;if(_tmp186->tag != 0)goto
_LL111;else{_tmp187=_tmp186->f1;}};}}_LL110:{const char*_tmp421;Cyc_Absyndump_dump(((
_tmp421="offsetof(",_tag_dyneither(_tmp421,sizeof(char),10))));}Cyc_Absyndump_dumptyp(
_tmp184);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump_nospace(*_tmp187);
Cyc_Absyndump_dump_char((int)')');goto _LLCC;_LL111: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp188=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp11A;if(_tmp188->tag != 20)goto
_LL113;else{_tmp189=(void*)_tmp188->f1;_tmp18A=(void*)_tmp188->f2;{struct Cyc_Absyn_TupleIndex_struct*
_tmp18B=(struct Cyc_Absyn_TupleIndex_struct*)_tmp18A;if(_tmp18B->tag != 1)goto
_LL113;else{_tmp18C=_tmp18B->f1;}};}}_LL112:{const char*_tmp422;Cyc_Absyndump_dump(((
_tmp422="offsetof(",_tag_dyneither(_tmp422,sizeof(char),10))));}Cyc_Absyndump_dumptyp(
_tmp189);Cyc_Absyndump_dump_char((int)',');{const char*_tmp426;void*_tmp425[1];
struct Cyc_Int_pa_struct _tmp424;Cyc_Absyndump_dump((struct _dyneither_ptr)((
_tmp424.tag=1,((_tmp424.f1=(unsigned long)((int)_tmp18C),((_tmp425[0]=& _tmp424,
Cyc_aprintf(((_tmp426="%d",_tag_dyneither(_tmp426,sizeof(char),3))),
_tag_dyneither(_tmp425,sizeof(void*),1)))))))));}Cyc_Absyndump_dump_char((int)')');
goto _LLCC;_LL113: {struct Cyc_Absyn_Deref_e_struct*_tmp18D=(struct Cyc_Absyn_Deref_e_struct*)
_tmp11A;if(_tmp18D->tag != 21)goto _LL115;else{_tmp18E=_tmp18D->f1;}}_LL114: Cyc_Absyndump_dump_char((
int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmp18E);goto _LLCC;_LL115: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp18F=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp11A;if(_tmp18F->tag != 22)goto
_LL117;else{_tmp190=_tmp18F->f1;_tmp191=_tmp18F->f2;}}_LL116: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp190);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp191);goto _LLCC;_LL117: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp192=(struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp11A;if(_tmp192->tag != 23)goto _LL119;else{
_tmp193=_tmp192->f1;_tmp194=_tmp192->f2;}}_LL118: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp193);{const char*_tmp427;Cyc_Absyndump_dump_nospace(((_tmp427="->",
_tag_dyneither(_tmp427,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(*_tmp194);
goto _LLCC;_LL119: {struct Cyc_Absyn_Subscript_e_struct*_tmp195=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp11A;if(_tmp195->tag != 24)goto _LL11B;else{_tmp196=_tmp195->f1;_tmp197=_tmp195->f2;}}
_LL11A: Cyc_Absyndump_dumpexp_prec(myprec,_tmp196);Cyc_Absyndump_dump_char((int)'[');
Cyc_Absyndump_dumpexp(_tmp197);Cyc_Absyndump_dump_char((int)']');goto _LLCC;
_LL11B: {struct Cyc_Absyn_Tuple_e_struct*_tmp198=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp11A;if(_tmp198->tag != 25)goto _LL11D;else{_tmp199=_tmp198->f1;}}_LL11C:{const
char*_tmp428;Cyc_Absyndump_dump(((_tmp428="$(",_tag_dyneither(_tmp428,sizeof(
char),3))));}Cyc_Absyndump_dumpexps_prec(20,_tmp199);Cyc_Absyndump_dump_char((
int)')');goto _LLCC;_LL11D: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp19A=(struct
Cyc_Absyn_CompoundLit_e_struct*)_tmp11A;if(_tmp19A->tag != 26)goto _LL11F;else{
_tmp19B=_tmp19A->f1;_tmp19C=_tmp19A->f2;}}_LL11E: Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp((*_tmp19B).f3);Cyc_Absyndump_dump_char((int)')');{const
char*_tmp42B;const char*_tmp42A;const char*_tmp429;((void(*)(void(*f)(struct
_tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr
end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp19C,((
_tmp429="{",_tag_dyneither(_tmp429,sizeof(char),2))),((_tmp42A="}",
_tag_dyneither(_tmp42A,sizeof(char),2))),((_tmp42B=",",_tag_dyneither(_tmp42B,
sizeof(char),2))));}goto _LLCC;_LL11F: {struct Cyc_Absyn_Array_e_struct*_tmp19D=(
struct Cyc_Absyn_Array_e_struct*)_tmp11A;if(_tmp19D->tag != 27)goto _LL121;else{
_tmp19E=_tmp19D->f1;}}_LL120:{const char*_tmp42E;const char*_tmp42D;const char*
_tmp42C;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp19E,((_tmp42C="{",_tag_dyneither(_tmp42C,sizeof(char),2))),((
_tmp42D="}",_tag_dyneither(_tmp42D,sizeof(char),2))),((_tmp42E=",",
_tag_dyneither(_tmp42E,sizeof(char),2))));}goto _LLCC;_LL121: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp19F=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp11A;if(_tmp19F->tag != 28)
goto _LL123;else{_tmp1A0=_tmp19F->f1;_tmp1A1=_tmp19F->f2;_tmp1A2=_tmp19F->f3;}}
_LL122:{const char*_tmp42F;Cyc_Absyndump_dump(((_tmp42F="new {for",_tag_dyneither(
_tmp42F,sizeof(char),9))));}Cyc_Absyndump_dump_str((*_tmp1A0->name).f2);Cyc_Absyndump_dump_char((
int)'<');Cyc_Absyndump_dumpexp(_tmp1A1);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(
_tmp1A2);Cyc_Absyndump_dump_char((int)'}');goto _LLCC;_LL123: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp1A3=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp11A;if(_tmp1A3->tag != 29)goto
_LL125;else{_tmp1A4=_tmp1A3->f1;_tmp1A5=_tmp1A3->f2;_tmp1A6=_tmp1A3->f3;}}_LL124:
Cyc_Absyndump_dumpqvar(_tmp1A4);Cyc_Absyndump_dump_char((int)'{');if(_tmp1A5 != 0)
Cyc_Absyndump_dumptps(_tmp1A5);{const char*_tmp432;const char*_tmp431;const char*
_tmp430;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp1A6,((_tmp430="",_tag_dyneither(_tmp430,sizeof(char),1))),((
_tmp431="}",_tag_dyneither(_tmp431,sizeof(char),2))),((_tmp432=",",
_tag_dyneither(_tmp432,sizeof(char),2))));}goto _LLCC;_LL125: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp1A7=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp11A;if(_tmp1A7->tag != 30)goto
_LL127;else{_tmp1A8=_tmp1A7->f2;}}_LL126:{const char*_tmp435;const char*_tmp434;
const char*_tmp433;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,
struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp1A8,((_tmp433="{",_tag_dyneither(_tmp433,sizeof(char),2))),((
_tmp434="}",_tag_dyneither(_tmp434,sizeof(char),2))),((_tmp435=",",
_tag_dyneither(_tmp435,sizeof(char),2))));}goto _LLCC;_LL127: {struct Cyc_Absyn_Datatype_e_struct*
_tmp1A9=(struct Cyc_Absyn_Datatype_e_struct*)_tmp11A;if(_tmp1A9->tag != 31)goto
_LL129;else{_tmp1AA=_tmp1A9->f1;_tmp1AB=_tmp1A9->f3;}}_LL128: Cyc_Absyndump_dumpqvar(
_tmp1AB->name);if(_tmp1AA != 0){const char*_tmp438;const char*_tmp437;const char*
_tmp436;((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp,_tmp1AA,((_tmp436="(",_tag_dyneither(_tmp436,sizeof(char),
2))),((_tmp437=")",_tag_dyneither(_tmp437,sizeof(char),2))),((_tmp438=",",
_tag_dyneither(_tmp438,sizeof(char),2))));}goto _LLCC;_LL129: {struct Cyc_Absyn_Enum_e_struct*
_tmp1AC=(struct Cyc_Absyn_Enum_e_struct*)_tmp11A;if(_tmp1AC->tag != 32)goto _LL12B;
else{_tmp1AD=_tmp1AC->f1;}}_LL12A: Cyc_Absyndump_dumpqvar(_tmp1AD);goto _LLCC;
_LL12B: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp1AE=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp11A;if(_tmp1AE->tag != 33)goto _LL12D;else{_tmp1AF=_tmp1AE->f1;}}_LL12C: Cyc_Absyndump_dumpqvar(
_tmp1AF);goto _LLCC;_LL12D: {struct Cyc_Absyn_Malloc_e_struct*_tmp1B0=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp11A;if(_tmp1B0->tag != 34)goto _LL12F;else{_tmp1B1=_tmp1B0->f1;_tmp1B2=_tmp1B1.is_calloc;
_tmp1B3=_tmp1B1.rgn;_tmp1B4=_tmp1B1.elt_type;_tmp1B5=_tmp1B1.num_elts;}}_LL12E:
if(_tmp1B2){if(_tmp1B3 != 0){{const char*_tmp439;Cyc_Absyndump_dump(((_tmp439="rcalloc(",
_tag_dyneither(_tmp439,sizeof(char),9))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_tmp1B3);{const char*_tmp43A;Cyc_Absyndump_dump(((_tmp43A=",",_tag_dyneither(
_tmp43A,sizeof(char),2))));};}else{const char*_tmp43B;Cyc_Absyndump_dump(((
_tmp43B="calloc",_tag_dyneither(_tmp43B,sizeof(char),7))));}Cyc_Absyndump_dumpexp(
_tmp1B5);{const char*_tmp43C;Cyc_Absyndump_dump(((_tmp43C=",",_tag_dyneither(
_tmp43C,sizeof(char),2))));}Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((
void**)_check_null(_tmp1B4)),0));{const char*_tmp43D;Cyc_Absyndump_dump(((_tmp43D=")",
_tag_dyneither(_tmp43D,sizeof(char),2))));};}else{if(_tmp1B3 != 0){{const char*
_tmp43E;Cyc_Absyndump_dump(((_tmp43E="rmalloc(",_tag_dyneither(_tmp43E,sizeof(
char),9))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp1B3);{const char*
_tmp43F;Cyc_Absyndump_dump(((_tmp43F=",",_tag_dyneither(_tmp43F,sizeof(char),2))));};}
else{const char*_tmp440;Cyc_Absyndump_dump(((_tmp440="malloc(",_tag_dyneither(
_tmp440,sizeof(char),8))));}if(_tmp1B4 != 0)Cyc_Absyndump_dumpexp(Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp1B4,0),_tmp1B5,0));else{Cyc_Absyndump_dumpexp(
_tmp1B5);}{const char*_tmp441;Cyc_Absyndump_dump(((_tmp441=")",_tag_dyneither(
_tmp441,sizeof(char),2))));};}goto _LLCC;_LL12F: {struct Cyc_Absyn_Swap_e_struct*
_tmp1B6=(struct Cyc_Absyn_Swap_e_struct*)_tmp11A;if(_tmp1B6->tag != 35)goto _LL131;
else{_tmp1B7=_tmp1B6->f1;_tmp1B8=_tmp1B6->f2;}}_LL130:{const char*_tmp442;Cyc_Absyndump_dump(((
_tmp442="cycswap(",_tag_dyneither(_tmp442,sizeof(char),9))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp1B7);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp1B8);Cyc_Absyndump_dump_char((int)')');goto _LLCC;_LL131: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp1B9=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp11A;if(_tmp1B9->tag != 36)
goto _LL133;else{_tmp1BA=_tmp1B9->f1;_tmp1BB=_tmp1B9->f2;}}_LL132:{const char*
_tmp445;const char*_tmp444;const char*_tmp443;((void(*)(void(*f)(struct _tuple16*),
struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct
_dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp1BB,((_tmp443="{",
_tag_dyneither(_tmp443,sizeof(char),2))),((_tmp444="}",_tag_dyneither(_tmp444,
sizeof(char),2))),((_tmp445=",",_tag_dyneither(_tmp445,sizeof(char),2))));}goto
_LLCC;_LL133: {struct Cyc_Absyn_StmtExp_e_struct*_tmp1BC=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp11A;if(_tmp1BC->tag != 37)goto _LLCC;else{_tmp1BD=_tmp1BC->f1;}}_LL134:{const
char*_tmp446;Cyc_Absyndump_dump_nospace(((_tmp446="({",_tag_dyneither(_tmp446,
sizeof(char),3))));}Cyc_Absyndump_dumpstmt(_tmp1BD,1);{const char*_tmp447;Cyc_Absyndump_dump_nospace(((
_tmp447="})",_tag_dyneither(_tmp447,sizeof(char),3))));}goto _LLCC;_LLCC:;}if(
inprec >= myprec)Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp*e);void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*e){Cyc_Absyndump_dumpexp_prec(
0,e);}void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List*scs);void Cyc_Absyndump_dumpswitchclauses(
struct Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause*
_tmp204=(struct Cyc_Absyn_Switch_clause*)scs->hd;if(_tmp204->where_clause == 0  && (
_tmp204->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){const char*_tmp448;Cyc_Absyndump_dump(((
_tmp448="default:",_tag_dyneither(_tmp448,sizeof(char),9))));}else{{const char*
_tmp449;Cyc_Absyndump_dump(((_tmp449="case",_tag_dyneither(_tmp449,sizeof(char),
5))));}Cyc_Absyndump_dumppat(_tmp204->pattern);if(_tmp204->where_clause != 0){{
const char*_tmp44A;Cyc_Absyndump_dump(((_tmp44A="&&",_tag_dyneither(_tmp44A,
sizeof(char),3))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(
_tmp204->where_clause));}{const char*_tmp44B;Cyc_Absyndump_dump_nospace(((_tmp44B=":",
_tag_dyneither(_tmp44B,sizeof(char),2))));};}Cyc_Absyndump_dumpstmt(_tmp204->body,
0);}}void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt);void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt*s,int expstmt){void*_tmp209=s->r;struct Cyc_Absyn_Exp*_tmp20C;
struct Cyc_Absyn_Stmt*_tmp20E;struct Cyc_Absyn_Stmt*_tmp20F;struct Cyc_Absyn_Exp*
_tmp211;struct Cyc_Absyn_Exp*_tmp213;struct Cyc_Absyn_Exp*_tmp215;struct Cyc_Absyn_Stmt*
_tmp216;struct Cyc_Absyn_Stmt*_tmp217;struct _tuple8 _tmp219;struct Cyc_Absyn_Exp*
_tmp21A;struct Cyc_Absyn_Stmt*_tmp21B;struct _dyneither_ptr*_tmp21F;struct Cyc_Absyn_Exp*
_tmp221;struct _tuple8 _tmp222;struct Cyc_Absyn_Exp*_tmp223;struct _tuple8 _tmp224;
struct Cyc_Absyn_Exp*_tmp225;struct Cyc_Absyn_Stmt*_tmp226;struct Cyc_Absyn_Exp*
_tmp228;struct Cyc_List_List*_tmp229;struct Cyc_Absyn_Decl*_tmp22B;struct Cyc_Absyn_Stmt*
_tmp22C;struct _dyneither_ptr*_tmp22E;struct Cyc_Absyn_Stmt*_tmp22F;struct Cyc_Absyn_Stmt*
_tmp231;struct _tuple8 _tmp232;struct Cyc_Absyn_Exp*_tmp233;struct Cyc_List_List*
_tmp235;struct Cyc_List_List*_tmp237;struct Cyc_Absyn_Stmt*_tmp239;struct Cyc_List_List*
_tmp23A;struct Cyc_Absyn_Exp*_tmp23C;_LL13A: {struct Cyc_Absyn_Skip_s_struct*
_tmp20A=(struct Cyc_Absyn_Skip_s_struct*)_tmp209;if(_tmp20A->tag != 0)goto _LL13C;}
_LL13B: Cyc_Absyndump_dump_semi();goto _LL139;_LL13C: {struct Cyc_Absyn_Exp_s_struct*
_tmp20B=(struct Cyc_Absyn_Exp_s_struct*)_tmp209;if(_tmp20B->tag != 1)goto _LL13E;
else{_tmp20C=_tmp20B->f1;}}_LL13D: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dumpexp(
_tmp20C);Cyc_Absyndump_dump_semi();goto _LL139;_LL13E: {struct Cyc_Absyn_Seq_s_struct*
_tmp20D=(struct Cyc_Absyn_Seq_s_struct*)_tmp209;if(_tmp20D->tag != 2)goto _LL140;
else{_tmp20E=_tmp20D->f1;_tmp20F=_tmp20D->f2;}}_LL13F: if(Cyc_Absynpp_is_declaration(
_tmp20E)){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp20E,0);
Cyc_Absyndump_dump_char((int)'}');}else{Cyc_Absyndump_dumpstmt(_tmp20E,0);}if(
Cyc_Absynpp_is_declaration(_tmp20F)){if(expstmt)Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp20F,expstmt);Cyc_Absyndump_dump_char((
int)'}');if(expstmt)Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dump_semi();}
else{Cyc_Absyndump_dumpstmt(_tmp20F,expstmt);}goto _LL139;_LL140: {struct Cyc_Absyn_Return_s_struct*
_tmp210=(struct Cyc_Absyn_Return_s_struct*)_tmp209;if(_tmp210->tag != 3)goto _LL142;
else{_tmp211=_tmp210->f1;if(_tmp211 != 0)goto _LL142;}}_LL141: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp44C;Cyc_Absyndump_dump(((_tmp44C="return;",_tag_dyneither(
_tmp44C,sizeof(char),8))));}goto _LL139;_LL142: {struct Cyc_Absyn_Return_s_struct*
_tmp212=(struct Cyc_Absyn_Return_s_struct*)_tmp209;if(_tmp212->tag != 3)goto _LL144;
else{_tmp213=_tmp212->f1;}}_LL143: Cyc_Absyndump_dumploc(s->loc);{const char*
_tmp44D;Cyc_Absyndump_dump(((_tmp44D="return",_tag_dyneither(_tmp44D,sizeof(char),
7))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp213));Cyc_Absyndump_dump_semi();
goto _LL139;_LL144: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp214=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp209;if(_tmp214->tag != 4)goto _LL146;else{_tmp215=_tmp214->f1;_tmp216=_tmp214->f2;
_tmp217=_tmp214->f3;}}_LL145: Cyc_Absyndump_dumploc(s->loc);{const char*_tmp44E;
Cyc_Absyndump_dump(((_tmp44E="if(",_tag_dyneither(_tmp44E,sizeof(char),4))));}
Cyc_Absyndump_dumpexp(_tmp215);{void*_tmp240=_tmp216->r;_LL161: {struct Cyc_Absyn_Seq_s_struct*
_tmp241=(struct Cyc_Absyn_Seq_s_struct*)_tmp240;if(_tmp241->tag != 2)goto _LL163;}
_LL162: goto _LL164;_LL163: {struct Cyc_Absyn_Decl_s_struct*_tmp242=(struct Cyc_Absyn_Decl_s_struct*)
_tmp240;if(_tmp242->tag != 12)goto _LL165;}_LL164: goto _LL166;_LL165: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp243=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp240;if(_tmp243->tag != 4)goto
_LL167;}_LL166: goto _LL168;_LL167: {struct Cyc_Absyn_Label_s_struct*_tmp244=(
struct Cyc_Absyn_Label_s_struct*)_tmp240;if(_tmp244->tag != 13)goto _LL169;}_LL168:{
const char*_tmp44F;Cyc_Absyndump_dump_nospace(((_tmp44F="){",_tag_dyneither(
_tmp44F,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(_tmp216,0);Cyc_Absyndump_dump_char((
int)'}');goto _LL160;_LL169:;_LL16A: Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(
_tmp216,0);_LL160:;}{void*_tmp246=_tmp217->r;_LL16C: {struct Cyc_Absyn_Skip_s_struct*
_tmp247=(struct Cyc_Absyn_Skip_s_struct*)_tmp246;if(_tmp247->tag != 0)goto _LL16E;}
_LL16D: goto _LL16B;_LL16E:;_LL16F:{const char*_tmp450;Cyc_Absyndump_dump(((_tmp450="else{",
_tag_dyneither(_tmp450,sizeof(char),6))));}Cyc_Absyndump_dumpstmt(_tmp217,0);Cyc_Absyndump_dump_char((
int)'}');goto _LL16B;_LL16B:;}goto _LL139;_LL146: {struct Cyc_Absyn_While_s_struct*
_tmp218=(struct Cyc_Absyn_While_s_struct*)_tmp209;if(_tmp218->tag != 5)goto _LL148;
else{_tmp219=_tmp218->f1;_tmp21A=_tmp219.f1;_tmp21B=_tmp218->f2;}}_LL147: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp451;Cyc_Absyndump_dump(((_tmp451="while(",_tag_dyneither(
_tmp451,sizeof(char),7))));}Cyc_Absyndump_dumpexp(_tmp21A);{const char*_tmp452;
Cyc_Absyndump_dump_nospace(((_tmp452="){",_tag_dyneither(_tmp452,sizeof(char),3))));}
Cyc_Absyndump_dumpstmt(_tmp21B,0);Cyc_Absyndump_dump_char((int)'}');goto _LL139;
_LL148: {struct Cyc_Absyn_Break_s_struct*_tmp21C=(struct Cyc_Absyn_Break_s_struct*)
_tmp209;if(_tmp21C->tag != 6)goto _LL14A;}_LL149: Cyc_Absyndump_dumploc(s->loc);{
const char*_tmp453;Cyc_Absyndump_dump(((_tmp453="break;",_tag_dyneither(_tmp453,
sizeof(char),7))));}goto _LL139;_LL14A: {struct Cyc_Absyn_Continue_s_struct*
_tmp21D=(struct Cyc_Absyn_Continue_s_struct*)_tmp209;if(_tmp21D->tag != 7)goto
_LL14C;}_LL14B: Cyc_Absyndump_dumploc(s->loc);{const char*_tmp454;Cyc_Absyndump_dump(((
_tmp454="continue;",_tag_dyneither(_tmp454,sizeof(char),10))));}goto _LL139;
_LL14C: {struct Cyc_Absyn_Goto_s_struct*_tmp21E=(struct Cyc_Absyn_Goto_s_struct*)
_tmp209;if(_tmp21E->tag != 8)goto _LL14E;else{_tmp21F=_tmp21E->f1;}}_LL14D: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp455;Cyc_Absyndump_dump(((_tmp455="goto",_tag_dyneither(
_tmp455,sizeof(char),5))));}Cyc_Absyndump_dump_str(_tmp21F);Cyc_Absyndump_dump_semi();
goto _LL139;_LL14E: {struct Cyc_Absyn_For_s_struct*_tmp220=(struct Cyc_Absyn_For_s_struct*)
_tmp209;if(_tmp220->tag != 9)goto _LL150;else{_tmp221=_tmp220->f1;_tmp222=_tmp220->f2;
_tmp223=_tmp222.f1;_tmp224=_tmp220->f3;_tmp225=_tmp224.f1;_tmp226=_tmp220->f4;}}
_LL14F: Cyc_Absyndump_dumploc(s->loc);{const char*_tmp456;Cyc_Absyndump_dump(((
_tmp456="for(",_tag_dyneither(_tmp456,sizeof(char),5))));}Cyc_Absyndump_dumpexp(
_tmp221);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp223);Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp(_tmp225);{const char*_tmp457;Cyc_Absyndump_dump_nospace(((
_tmp457="){",_tag_dyneither(_tmp457,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(
_tmp226,0);Cyc_Absyndump_dump_char((int)'}');goto _LL139;_LL150: {struct Cyc_Absyn_Switch_s_struct*
_tmp227=(struct Cyc_Absyn_Switch_s_struct*)_tmp209;if(_tmp227->tag != 10)goto
_LL152;else{_tmp228=_tmp227->f1;_tmp229=_tmp227->f2;}}_LL151: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp458;Cyc_Absyndump_dump(((_tmp458="switch(",_tag_dyneither(
_tmp458,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp228);{const char*_tmp459;
Cyc_Absyndump_dump_nospace(((_tmp459="){",_tag_dyneither(_tmp459,sizeof(char),3))));}
Cyc_Absyndump_dumpswitchclauses(_tmp229);Cyc_Absyndump_dump_char((int)'}');goto
_LL139;_LL152: {struct Cyc_Absyn_Decl_s_struct*_tmp22A=(struct Cyc_Absyn_Decl_s_struct*)
_tmp209;if(_tmp22A->tag != 12)goto _LL154;else{_tmp22B=_tmp22A->f1;_tmp22C=_tmp22A->f2;}}
_LL153: Cyc_Absyndump_dumpdecl(_tmp22B);Cyc_Absyndump_dumpstmt(_tmp22C,expstmt);
goto _LL139;_LL154: {struct Cyc_Absyn_Label_s_struct*_tmp22D=(struct Cyc_Absyn_Label_s_struct*)
_tmp209;if(_tmp22D->tag != 13)goto _LL156;else{_tmp22E=_tmp22D->f1;_tmp22F=_tmp22D->f2;}}
_LL155: if(Cyc_Absynpp_is_declaration(_tmp22F)){Cyc_Absyndump_dump_str(_tmp22E);
if(expstmt){const char*_tmp45A;Cyc_Absyndump_dump_nospace(((_tmp45A=": ({",
_tag_dyneither(_tmp45A,sizeof(char),5))));}else{const char*_tmp45B;Cyc_Absyndump_dump_nospace(((
_tmp45B=": {",_tag_dyneither(_tmp45B,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(
_tmp22F,expstmt);if(expstmt){const char*_tmp45C;Cyc_Absyndump_dump_nospace(((
_tmp45C="});}",_tag_dyneither(_tmp45C,sizeof(char),5))));}else{Cyc_Absyndump_dump_char((
int)'}');}}else{Cyc_Absyndump_dump_str(_tmp22E);Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpstmt(_tmp22F,expstmt);}goto _LL139;_LL156: {struct Cyc_Absyn_Do_s_struct*
_tmp230=(struct Cyc_Absyn_Do_s_struct*)_tmp209;if(_tmp230->tag != 14)goto _LL158;
else{_tmp231=_tmp230->f1;_tmp232=_tmp230->f2;_tmp233=_tmp232.f1;}}_LL157: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp45D;Cyc_Absyndump_dump(((_tmp45D="do{",_tag_dyneither(
_tmp45D,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(_tmp231,0);{const char*_tmp45E;
Cyc_Absyndump_dump_nospace(((_tmp45E="}while(",_tag_dyneither(_tmp45E,sizeof(
char),8))));}Cyc_Absyndump_dumpexp(_tmp233);{const char*_tmp45F;Cyc_Absyndump_dump_nospace(((
_tmp45F=");",_tag_dyneither(_tmp45F,sizeof(char),3))));}goto _LL139;_LL158: {
struct Cyc_Absyn_Fallthru_s_struct*_tmp234=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp209;if(_tmp234->tag != 11)goto _LL15A;else{_tmp235=_tmp234->f1;if(_tmp235 != 0)
goto _LL15A;}}_LL159:{const char*_tmp460;Cyc_Absyndump_dump(((_tmp460="fallthru;",
_tag_dyneither(_tmp460,sizeof(char),10))));}goto _LL139;_LL15A: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp236=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp209;if(_tmp236->tag != 11)goto
_LL15C;else{_tmp237=_tmp236->f1;}}_LL15B:{const char*_tmp461;Cyc_Absyndump_dump(((
_tmp461="fallthru(",_tag_dyneither(_tmp461,sizeof(char),10))));}Cyc_Absyndump_dumpexps_prec(
20,_tmp237);{const char*_tmp462;Cyc_Absyndump_dump_nospace(((_tmp462=");",
_tag_dyneither(_tmp462,sizeof(char),3))));}goto _LL139;_LL15C: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp238=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp209;if(_tmp238->tag != 15)goto
_LL15E;else{_tmp239=_tmp238->f1;_tmp23A=_tmp238->f2;}}_LL15D: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp463;Cyc_Absyndump_dump(((_tmp463="try",_tag_dyneither(
_tmp463,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(_tmp239,0);{const char*_tmp464;
Cyc_Absyndump_dump(((_tmp464="catch{",_tag_dyneither(_tmp464,sizeof(char),7))));}
Cyc_Absyndump_dumpswitchclauses(_tmp23A);Cyc_Absyndump_dump_char((int)'}');goto
_LL139;_LL15E: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp23B=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp209;if(_tmp23B->tag != 16)goto _LL139;else{_tmp23C=_tmp23B->f1;}}_LL15F: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp465;Cyc_Absyndump_dump(((_tmp465="reset_region(",
_tag_dyneither(_tmp465,sizeof(char),14))));}Cyc_Absyndump_dumpexp(_tmp23C);{
const char*_tmp466;Cyc_Absyndump_dump(((_tmp466=");",_tag_dyneither(_tmp466,
sizeof(char),3))));}goto _LL139;_LL139:;}struct _tuple17{struct Cyc_List_List*f1;
struct Cyc_Absyn_Pat*f2;};void Cyc_Absyndump_dumpdp(struct _tuple17*dp);void Cyc_Absyndump_dumpdp(
struct _tuple17*dp){{const char*_tmp469;const char*_tmp468;const char*_tmp467;((void(*)(
void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,(*
dp).f1,((_tmp467="",_tag_dyneither(_tmp467,sizeof(char),1))),((_tmp468="=",
_tag_dyneither(_tmp468,sizeof(char),2))),((_tmp469="=",_tag_dyneither(_tmp469,
sizeof(char),2))));}Cyc_Absyndump_dumppat((*dp).f2);}void Cyc_Absyndump_dumppat(
struct Cyc_Absyn_Pat*p);void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){void*
_tmp262=p->r;enum Cyc_Absyn_Sign _tmp266;int _tmp267;enum Cyc_Absyn_Sign _tmp269;
int _tmp26A;enum Cyc_Absyn_Sign _tmp26C;int _tmp26D;char _tmp26F;struct
_dyneither_ptr _tmp271;struct Cyc_Absyn_Vardecl*_tmp273;struct Cyc_Absyn_Pat*
_tmp274;struct Cyc_Absyn_Pat _tmp275;void*_tmp276;struct Cyc_Absyn_Vardecl*_tmp279;
struct Cyc_Absyn_Pat*_tmp27A;struct Cyc_List_List*_tmp27C;int _tmp27D;struct Cyc_Absyn_Pat*
_tmp27F;struct Cyc_Absyn_Vardecl*_tmp281;struct Cyc_Absyn_Pat*_tmp282;struct Cyc_Absyn_Pat
_tmp283;void*_tmp284;struct Cyc_Absyn_Vardecl*_tmp287;struct Cyc_Absyn_Pat*_tmp288;
struct Cyc_Absyn_Tvar*_tmp28A;struct Cyc_Absyn_Vardecl*_tmp28B;struct _tuple0*
_tmp28D;struct _tuple0*_tmp28F;struct Cyc_List_List*_tmp290;int _tmp291;struct Cyc_Absyn_AggrInfo*
_tmp293;struct Cyc_Absyn_AggrInfo _tmp294;union Cyc_Absyn_AggrInfoU _tmp295;struct
Cyc_List_List*_tmp296;struct Cyc_List_List*_tmp297;int _tmp298;struct Cyc_Absyn_AggrInfo*
_tmp29A;struct Cyc_List_List*_tmp29B;struct Cyc_List_List*_tmp29C;int _tmp29D;
struct Cyc_Absyn_Datatypefield*_tmp29F;struct Cyc_List_List*_tmp2A0;int _tmp2A1;
struct Cyc_Absyn_Enumfield*_tmp2A3;struct Cyc_Absyn_Enumfield*_tmp2A5;struct Cyc_Absyn_Exp*
_tmp2A7;_LL171: {struct Cyc_Absyn_Wild_p_struct*_tmp263=(struct Cyc_Absyn_Wild_p_struct*)
_tmp262;if(_tmp263->tag != 0)goto _LL173;}_LL172: Cyc_Absyndump_dump_char((int)'_');
goto _LL170;_LL173: {struct Cyc_Absyn_Null_p_struct*_tmp264=(struct Cyc_Absyn_Null_p_struct*)
_tmp262;if(_tmp264->tag != 8)goto _LL175;}_LL174:{const char*_tmp46A;Cyc_Absyndump_dump(((
_tmp46A="NULL",_tag_dyneither(_tmp46A,sizeof(char),5))));}goto _LL170;_LL175: {
struct Cyc_Absyn_Int_p_struct*_tmp265=(struct Cyc_Absyn_Int_p_struct*)_tmp262;if(
_tmp265->tag != 9)goto _LL177;else{_tmp266=_tmp265->f1;if(_tmp266 != Cyc_Absyn_None)
goto _LL177;_tmp267=_tmp265->f2;}}_LL176: _tmp26A=_tmp267;goto _LL178;_LL177: {
struct Cyc_Absyn_Int_p_struct*_tmp268=(struct Cyc_Absyn_Int_p_struct*)_tmp262;if(
_tmp268->tag != 9)goto _LL179;else{_tmp269=_tmp268->f1;if(_tmp269 != Cyc_Absyn_Signed)
goto _LL179;_tmp26A=_tmp268->f2;}}_LL178:{const char*_tmp46E;void*_tmp46D[1];
struct Cyc_Int_pa_struct _tmp46C;Cyc_Absyndump_dump((struct _dyneither_ptr)((
_tmp46C.tag=1,((_tmp46C.f1=(unsigned long)_tmp26A,((_tmp46D[0]=& _tmp46C,Cyc_aprintf(((
_tmp46E="%d",_tag_dyneither(_tmp46E,sizeof(char),3))),_tag_dyneither(_tmp46D,
sizeof(void*),1)))))))));}goto _LL170;_LL179: {struct Cyc_Absyn_Int_p_struct*
_tmp26B=(struct Cyc_Absyn_Int_p_struct*)_tmp262;if(_tmp26B->tag != 9)goto _LL17B;
else{_tmp26C=_tmp26B->f1;if(_tmp26C != Cyc_Absyn_Unsigned)goto _LL17B;_tmp26D=
_tmp26B->f2;}}_LL17A:{const char*_tmp472;void*_tmp471[1];struct Cyc_Int_pa_struct
_tmp470;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp470.tag=1,((_tmp470.f1=(
unsigned int)_tmp26D,((_tmp471[0]=& _tmp470,Cyc_aprintf(((_tmp472="%u",
_tag_dyneither(_tmp472,sizeof(char),3))),_tag_dyneither(_tmp471,sizeof(void*),1)))))))));}
goto _LL170;_LL17B: {struct Cyc_Absyn_Char_p_struct*_tmp26E=(struct Cyc_Absyn_Char_p_struct*)
_tmp262;if(_tmp26E->tag != 10)goto _LL17D;else{_tmp26F=_tmp26E->f1;}}_LL17C:{const
char*_tmp473;Cyc_Absyndump_dump(((_tmp473="'",_tag_dyneither(_tmp473,sizeof(char),
2))));}Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp26F));{const char*
_tmp474;Cyc_Absyndump_dump_nospace(((_tmp474="'",_tag_dyneither(_tmp474,sizeof(
char),2))));}goto _LL170;_LL17D: {struct Cyc_Absyn_Float_p_struct*_tmp270=(struct
Cyc_Absyn_Float_p_struct*)_tmp262;if(_tmp270->tag != 11)goto _LL17F;else{_tmp271=
_tmp270->f1;}}_LL17E: Cyc_Absyndump_dump(_tmp271);goto _LL170;_LL17F: {struct Cyc_Absyn_Var_p_struct*
_tmp272=(struct Cyc_Absyn_Var_p_struct*)_tmp262;if(_tmp272->tag != 1)goto _LL181;
else{_tmp273=_tmp272->f1;_tmp274=_tmp272->f2;_tmp275=*_tmp274;_tmp276=_tmp275.r;{
struct Cyc_Absyn_Wild_p_struct*_tmp277=(struct Cyc_Absyn_Wild_p_struct*)_tmp276;
if(_tmp277->tag != 0)goto _LL181;};}}_LL180: Cyc_Absyndump_dumpqvar(_tmp273->name);
goto _LL170;_LL181: {struct Cyc_Absyn_Var_p_struct*_tmp278=(struct Cyc_Absyn_Var_p_struct*)
_tmp262;if(_tmp278->tag != 1)goto _LL183;else{_tmp279=_tmp278->f1;_tmp27A=_tmp278->f2;}}
_LL182: Cyc_Absyndump_dumpqvar(_tmp279->name);{const char*_tmp475;Cyc_Absyndump_dump(((
_tmp475=" as ",_tag_dyneither(_tmp475,sizeof(char),5))));}Cyc_Absyndump_dumppat(
_tmp27A);goto _LL170;_LL183: {struct Cyc_Absyn_Tuple_p_struct*_tmp27B=(struct Cyc_Absyn_Tuple_p_struct*)
_tmp262;if(_tmp27B->tag != 4)goto _LL185;else{_tmp27C=_tmp27B->f1;_tmp27D=_tmp27B->f2;}}
_LL184: {const char*_tmp477;const char*_tmp476;struct _dyneither_ptr term=_tmp27D?(
_tmp477=", ...)",_tag_dyneither(_tmp477,sizeof(char),7)):((_tmp476=")",
_tag_dyneither(_tmp476,sizeof(char),2)));{const char*_tmp479;const char*_tmp478;((
void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp27C,((_tmp478="$(",_tag_dyneither(_tmp478,sizeof(char),
3))),term,((_tmp479=",",_tag_dyneither(_tmp479,sizeof(char),2))));}goto _LL170;}
_LL185: {struct Cyc_Absyn_Pointer_p_struct*_tmp27E=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp262;if(_tmp27E->tag != 5)goto _LL187;else{_tmp27F=_tmp27E->f1;}}_LL186:{const
char*_tmp47A;Cyc_Absyndump_dump(((_tmp47A="&",_tag_dyneither(_tmp47A,sizeof(char),
2))));}Cyc_Absyndump_dumppat(_tmp27F);goto _LL170;_LL187: {struct Cyc_Absyn_Reference_p_struct*
_tmp280=(struct Cyc_Absyn_Reference_p_struct*)_tmp262;if(_tmp280->tag != 2)goto
_LL189;else{_tmp281=_tmp280->f1;_tmp282=_tmp280->f2;_tmp283=*_tmp282;_tmp284=
_tmp283.r;{struct Cyc_Absyn_Wild_p_struct*_tmp285=(struct Cyc_Absyn_Wild_p_struct*)
_tmp284;if(_tmp285->tag != 0)goto _LL189;};}}_LL188:{const char*_tmp47B;Cyc_Absyndump_dump(((
_tmp47B="*",_tag_dyneither(_tmp47B,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(
_tmp281->name);goto _LL170;_LL189: {struct Cyc_Absyn_Reference_p_struct*_tmp286=(
struct Cyc_Absyn_Reference_p_struct*)_tmp262;if(_tmp286->tag != 2)goto _LL18B;else{
_tmp287=_tmp286->f1;_tmp288=_tmp286->f2;}}_LL18A:{const char*_tmp47C;Cyc_Absyndump_dump(((
_tmp47C="*",_tag_dyneither(_tmp47C,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(
_tmp287->name);{const char*_tmp47D;Cyc_Absyndump_dump(((_tmp47D=" as ",
_tag_dyneither(_tmp47D,sizeof(char),5))));}Cyc_Absyndump_dumppat(_tmp288);goto
_LL170;_LL18B: {struct Cyc_Absyn_TagInt_p_struct*_tmp289=(struct Cyc_Absyn_TagInt_p_struct*)
_tmp262;if(_tmp289->tag != 3)goto _LL18D;else{_tmp28A=_tmp289->f1;_tmp28B=_tmp289->f2;}}
_LL18C: Cyc_Absyndump_dumpqvar(_tmp28B->name);Cyc_Absyndump_dump_char((int)'<');
Cyc_Absyndump_dumptvar(_tmp28A);Cyc_Absyndump_dump_char((int)'>');goto _LL170;
_LL18D: {struct Cyc_Absyn_UnknownId_p_struct*_tmp28C=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp262;if(_tmp28C->tag != 14)goto _LL18F;else{_tmp28D=_tmp28C->f1;}}_LL18E: Cyc_Absyndump_dumpqvar(
_tmp28D);goto _LL170;_LL18F: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp28E=(
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp262;if(_tmp28E->tag != 15)goto _LL191;
else{_tmp28F=_tmp28E->f1;_tmp290=_tmp28E->f2;_tmp291=_tmp28E->f3;}}_LL190: {
const char*_tmp47F;const char*_tmp47E;struct _dyneither_ptr term=_tmp291?(_tmp47F=", ...)",
_tag_dyneither(_tmp47F,sizeof(char),7)):((_tmp47E=")",_tag_dyneither(_tmp47E,
sizeof(char),2)));Cyc_Absyndump_dumpqvar(_tmp28F);{const char*_tmp481;const char*
_tmp480;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp290,((_tmp480="(",_tag_dyneither(_tmp480,sizeof(char),
2))),term,((_tmp481=",",_tag_dyneither(_tmp481,sizeof(char),2))));}goto _LL170;}
_LL191: {struct Cyc_Absyn_Aggr_p_struct*_tmp292=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp262;if(_tmp292->tag != 6)goto _LL193;else{_tmp293=_tmp292->f1;if(_tmp293 == 0)
goto _LL193;_tmp294=*_tmp293;_tmp295=_tmp294.aggr_info;_tmp296=_tmp292->f2;
_tmp297=_tmp292->f3;_tmp298=_tmp292->f4;}}_LL192: {struct _tuple0*_tmp2BF;struct
_tuple9 _tmp2BE=Cyc_Absyn_aggr_kinded_name(_tmp295);_tmp2BF=_tmp2BE.f2;{const char*
_tmp483;const char*_tmp482;struct _dyneither_ptr term=_tmp298?(_tmp483=", ...)",
_tag_dyneither(_tmp483,sizeof(char),7)):((_tmp482=")",_tag_dyneither(_tmp482,
sizeof(char),2)));Cyc_Absyndump_dumpqvar(_tmp2BF);Cyc_Absyndump_dump_char((int)'{');{
const char*_tmp486;const char*_tmp485;const char*_tmp484;((void(*)(void(*f)(struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,
_tmp296,((_tmp484="[",_tag_dyneither(_tmp484,sizeof(char),2))),((_tmp485="]",
_tag_dyneither(_tmp485,sizeof(char),2))),((_tmp486=",",_tag_dyneither(_tmp486,
sizeof(char),2))));}{const char*_tmp488;const char*_tmp487;((void(*)(void(*f)(
struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,
_tmp297,((_tmp487="",_tag_dyneither(_tmp487,sizeof(char),1))),term,((_tmp488=",",
_tag_dyneither(_tmp488,sizeof(char),2))));}goto _LL170;};}_LL193: {struct Cyc_Absyn_Aggr_p_struct*
_tmp299=(struct Cyc_Absyn_Aggr_p_struct*)_tmp262;if(_tmp299->tag != 6)goto _LL195;
else{_tmp29A=_tmp299->f1;if(_tmp29A != 0)goto _LL195;_tmp29B=_tmp299->f2;_tmp29C=
_tmp299->f3;_tmp29D=_tmp299->f4;}}_LL194: {const char*_tmp48A;const char*_tmp489;
struct _dyneither_ptr term=_tmp29D?(_tmp48A=", ...)",_tag_dyneither(_tmp48A,
sizeof(char),7)):((_tmp489=")",_tag_dyneither(_tmp489,sizeof(char),2)));Cyc_Absyndump_dump_char((
int)'{');{const char*_tmp48D;const char*_tmp48C;const char*_tmp48B;((void(*)(void(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,
_tmp29B,((_tmp48B="[",_tag_dyneither(_tmp48B,sizeof(char),2))),((_tmp48C="]",
_tag_dyneither(_tmp48C,sizeof(char),2))),((_tmp48D=",",_tag_dyneither(_tmp48D,
sizeof(char),2))));}{const char*_tmp48F;const char*_tmp48E;((void(*)(void(*f)(
struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,
_tmp29C,((_tmp48E="",_tag_dyneither(_tmp48E,sizeof(char),1))),term,((_tmp48F=",",
_tag_dyneither(_tmp48F,sizeof(char),2))));}goto _LL170;}_LL195: {struct Cyc_Absyn_Datatype_p_struct*
_tmp29E=(struct Cyc_Absyn_Datatype_p_struct*)_tmp262;if(_tmp29E->tag != 7)goto
_LL197;else{_tmp29F=_tmp29E->f2;_tmp2A0=_tmp29E->f3;_tmp2A1=_tmp29E->f4;}}_LL196: {
const char*_tmp491;const char*_tmp490;struct _dyneither_ptr term=_tmp2A1?(_tmp491=", ...)",
_tag_dyneither(_tmp491,sizeof(char),7)):((_tmp490=")",_tag_dyneither(_tmp490,
sizeof(char),2)));Cyc_Absyndump_dumpqvar(_tmp29F->name);if(_tmp2A0 != 0){const
char*_tmp493;const char*_tmp492;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct
Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct
_dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp2A0,((_tmp492="(",
_tag_dyneither(_tmp492,sizeof(char),2))),term,((_tmp493=",",_tag_dyneither(
_tmp493,sizeof(char),2))));}goto _LL170;}_LL197: {struct Cyc_Absyn_Enum_p_struct*
_tmp2A2=(struct Cyc_Absyn_Enum_p_struct*)_tmp262;if(_tmp2A2->tag != 12)goto _LL199;
else{_tmp2A3=_tmp2A2->f2;}}_LL198: _tmp2A5=_tmp2A3;goto _LL19A;_LL199: {struct Cyc_Absyn_AnonEnum_p_struct*
_tmp2A4=(struct Cyc_Absyn_AnonEnum_p_struct*)_tmp262;if(_tmp2A4->tag != 13)goto
_LL19B;else{_tmp2A5=_tmp2A4->f2;}}_LL19A: Cyc_Absyndump_dumpqvar(_tmp2A5->name);
goto _LL170;_LL19B: {struct Cyc_Absyn_Exp_p_struct*_tmp2A6=(struct Cyc_Absyn_Exp_p_struct*)
_tmp262;if(_tmp2A6->tag != 16)goto _LL170;else{_tmp2A7=_tmp2A6->f1;}}_LL19C: Cyc_Absyndump_dumpexp(
_tmp2A7);goto _LL170;_LL170:;}void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*
ef);void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*ef){Cyc_Absyndump_dumpqvar(
ef->name);if(ef->typs != 0)Cyc_Absyndump_dumpargs(ef->typs);}void Cyc_Absyndump_dumpdatatypefields(
struct Cyc_List_List*fields);void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*
fields){const char*_tmp494;((void(*)(void(*f)(struct Cyc_Absyn_Datatypefield*),
struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,
fields,((_tmp494=",",_tag_dyneither(_tmp494,sizeof(char),2))));}void Cyc_Absyndump_dumpenumfield(
struct Cyc_Absyn_Enumfield*ef);void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*
ef){Cyc_Absyndump_dumpqvar(ef->name);if(ef->tag != 0){{const char*_tmp495;Cyc_Absyndump_dump(((
_tmp495=" = ",_tag_dyneither(_tmp495,sizeof(char),4))));}Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}void Cyc_Absyndump_dumpenumfields(
struct Cyc_List_List*fields);void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*
fields){const char*_tmp496;((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct
Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,
fields,((_tmp496=",",_tag_dyneither(_tmp496,sizeof(char),2))));}void Cyc_Absyndump_dumpaggrfields(
struct Cyc_List_List*fields);void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*
fields){for(0;fields != 0;fields=fields->tl){struct Cyc_Absyn_Aggrfield _tmp2D6;
struct _dyneither_ptr*_tmp2D7;struct Cyc_Absyn_Tqual _tmp2D8;void*_tmp2D9;struct Cyc_Absyn_Exp*
_tmp2DA;struct Cyc_List_List*_tmp2DB;struct Cyc_Absyn_Aggrfield*_tmp2D5=(struct Cyc_Absyn_Aggrfield*)
fields->hd;_tmp2D6=*_tmp2D5;_tmp2D7=_tmp2D6.name;_tmp2D8=_tmp2D6.tq;_tmp2D9=
_tmp2D6.type;_tmp2DA=_tmp2D6.width;_tmp2DB=_tmp2D6.attributes;switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL19D:((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(
struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp2D8,
_tmp2D9,Cyc_Absyndump_dump_str,_tmp2D7);Cyc_Absyndump_dumpatts(_tmp2DB);break;
case Cyc_Cyclone_Vc_c: _LL19E: Cyc_Absyndump_dumpatts(_tmp2DB);((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(
_tmp2D8,_tmp2D9,Cyc_Absyndump_dump_str,_tmp2D7);break;}if(_tmp2DA != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp2DA);}Cyc_Absyndump_dump_semi();}}
void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td);void Cyc_Absyndump_dumptypedefname(
struct Cyc_Absyn_Typedefdecl*td){Cyc_Absyndump_dumpqvar(td->name);Cyc_Absyndump_dumptvars(
td->tvs);}static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd);
static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dumpatts(
fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple18{void*f1;struct
_tuple0*f2;};static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple18*pr);
static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple18*pr){{void*_tmp2DC=(*pr).f1;
_LL1A1: {struct Cyc_Absyn_Unused_att_struct*_tmp2DD=(struct Cyc_Absyn_Unused_att_struct*)
_tmp2DC;if(_tmp2DD->tag != 11)goto _LL1A3;}_LL1A2: goto _LL1A0;_LL1A3: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp2DE=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp2DC;if(_tmp2DE->tag != 1)goto
_LL1A5;}_LL1A4:{const char*_tmp497;Cyc_Absyndump_dump(((_tmp497="_stdcall",
_tag_dyneither(_tmp497,sizeof(char),9))));}goto _LL1A0;_LL1A5: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp2DF=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp2DC;if(_tmp2DF->tag != 2)goto
_LL1A7;}_LL1A6:{const char*_tmp498;Cyc_Absyndump_dump(((_tmp498="_cdecl",
_tag_dyneither(_tmp498,sizeof(char),7))));}goto _LL1A0;_LL1A7: {struct Cyc_Absyn_Fastcall_att_struct*
_tmp2E0=(struct Cyc_Absyn_Fastcall_att_struct*)_tmp2DC;if(_tmp2E0->tag != 3)goto
_LL1A9;}_LL1A8:{const char*_tmp499;Cyc_Absyndump_dump(((_tmp499="_fastcall",
_tag_dyneither(_tmp499,sizeof(char),10))));}goto _LL1A0;_LL1A9:;_LL1AA: goto _LL1A0;
_LL1A0:;}Cyc_Absyndump_dumpqvar((*pr).f2);}static void Cyc_Absyndump_dump_callconv_fdqvar(
struct Cyc_Absyn_Fndecl*fd);static void Cyc_Absyndump_dump_callconv_fdqvar(struct
Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dump_callconv(fd->attributes);Cyc_Absyndump_dumpqvar(
fd->name);}static void Cyc_Absyndump_dumpids(struct Cyc_List_List*vds);static void
Cyc_Absyndump_dumpids(struct Cyc_List_List*vds){for(0;vds != 0;vds=vds->tl){Cyc_Absyndump_dumpqvar(((
struct Cyc_Absyn_Vardecl*)vds->hd)->name);if(vds->tl != 0)Cyc_Absyndump_dump_char((
int)',');}}void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,struct Cyc_Position_Segment*
loc);void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Vardecl _tmp2E5;enum Cyc_Absyn_Scope _tmp2E6;struct _tuple0*
_tmp2E7;struct Cyc_Absyn_Tqual _tmp2E8;void*_tmp2E9;struct Cyc_Absyn_Exp*_tmp2EA;
struct Cyc_List_List*_tmp2EB;struct Cyc_Absyn_Vardecl*_tmp2E4=vd;_tmp2E5=*_tmp2E4;
_tmp2E6=_tmp2E5.sc;_tmp2E7=_tmp2E5.name;_tmp2E8=_tmp2E5.tq;_tmp2E9=_tmp2E5.type;
_tmp2EA=_tmp2E5.initializer;_tmp2EB=_tmp2E5.attributes;Cyc_Absyndump_dumploc(loc);
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL1AB: if(_tmp2E6 == Cyc_Absyn_Extern
 && Cyc_Absyndump_qvar_to_Cids){void*_tmp2EC=Cyc_Tcutil_compress(_tmp2E9);_LL1AE: {
struct Cyc_Absyn_FnType_struct*_tmp2ED=(struct Cyc_Absyn_FnType_struct*)_tmp2EC;
if(_tmp2ED->tag != 10)goto _LL1B0;}_LL1AF: goto _LL1AD;_LL1B0:;_LL1B1: Cyc_Absyndump_dumpscope(
_tmp2E6);_LL1AD:;}else{Cyc_Absyndump_dumpscope(_tmp2E6);}((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp2E8,
_tmp2E9,Cyc_Absyndump_dumpqvar,_tmp2E7);Cyc_Absyndump_dumpatts(_tmp2EB);break;
case Cyc_Cyclone_Vc_c: _LL1AC: Cyc_Absyndump_dumpatts(_tmp2EB);Cyc_Absyndump_dumpscope(
_tmp2E6);{struct _RegionHandle _tmp2EE=_new_region("temp");struct _RegionHandle*
temp=& _tmp2EE;_push_region(temp);{struct Cyc_Absyn_Tqual _tmp2F0;void*_tmp2F1;
struct Cyc_List_List*_tmp2F2;struct _tuple11 _tmp2EF=Cyc_Absynpp_to_tms(temp,
_tmp2E8,_tmp2E9);_tmp2F0=_tmp2EF.f1;_tmp2F1=_tmp2EF.f2;_tmp2F2=_tmp2EF.f3;{void*
call_conv=(void*)& Cyc_Absyn_Unused_att_val;{struct Cyc_List_List*tms2=_tmp2F2;
for(0;tms2 != 0;tms2=tms2->tl){void*_tmp2F3=(void*)tms2->hd;struct Cyc_List_List*
_tmp2F5;_LL1B4: {struct Cyc_Absyn_Attributes_mod_struct*_tmp2F4=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmp2F3;if(_tmp2F4->tag != 5)goto _LL1B6;else{_tmp2F5=_tmp2F4->f2;}}_LL1B5: for(0;
_tmp2F5 != 0;_tmp2F5=_tmp2F5->tl){void*_tmp2F6=(void*)_tmp2F5->hd;_LL1B9: {struct
Cyc_Absyn_Stdcall_att_struct*_tmp2F7=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp2F6;if(_tmp2F7->tag != 1)goto _LL1BB;}_LL1BA: call_conv=(void*)& Cyc_Absyn_Stdcall_att_val;
goto _LL1B8;_LL1BB: {struct Cyc_Absyn_Cdecl_att_struct*_tmp2F8=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmp2F6;if(_tmp2F8->tag != 2)goto _LL1BD;}_LL1BC: call_conv=(void*)& Cyc_Absyn_Cdecl_att_val;
goto _LL1B8;_LL1BD: {struct Cyc_Absyn_Fastcall_att_struct*_tmp2F9=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp2F6;if(_tmp2F9->tag != 3)goto _LL1BF;}_LL1BE: call_conv=(void*)& Cyc_Absyn_Fastcall_att_val;
goto _LL1B8;_LL1BF:;_LL1C0: goto _LL1B8;_LL1B8:;}goto _LL1B3;_LL1B6:;_LL1B7: goto
_LL1B3;_LL1B3:;}}Cyc_Absyndump_dumptq(_tmp2F0);Cyc_Absyndump_dumpntyp(_tmp2F1);{
struct _tuple18 _tmp49A;struct _tuple18 _tmp2FA=(_tmp49A.f1=call_conv,((_tmp49A.f2=
_tmp2E7,_tmp49A)));((void(*)(struct Cyc_List_List*tms,void(*f)(struct _tuple18*),
struct _tuple18*a))Cyc_Absyndump_dumptms)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp2F2),Cyc_Absyndump_dump_callconv_qvar,& _tmp2FA);};};}
_npop_handler(0);break;;_pop_region(temp);};}if(_tmp2EA != 0){Cyc_Absyndump_dump_char((
int)'=');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp2EA);}Cyc_Absyndump_dump_semi();}
struct _tuple19{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*d);void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){void*
_tmp2FC=d->r;struct Cyc_Absyn_Vardecl*_tmp2FE;struct Cyc_Absyn_Fndecl*_tmp300;
struct Cyc_Absyn_Aggrdecl*_tmp302;struct Cyc_Absyn_Datatypedecl*_tmp304;struct Cyc_Absyn_Datatypedecl
_tmp305;enum Cyc_Absyn_Scope _tmp306;struct _tuple0*_tmp307;struct Cyc_List_List*
_tmp308;struct Cyc_Core_Opt*_tmp309;int _tmp30A;struct Cyc_Absyn_Enumdecl*_tmp30C;
struct Cyc_Absyn_Enumdecl _tmp30D;enum Cyc_Absyn_Scope _tmp30E;struct _tuple0*
_tmp30F;struct Cyc_Core_Opt*_tmp310;struct Cyc_Absyn_Pat*_tmp312;struct Cyc_Absyn_Exp*
_tmp313;struct Cyc_List_List*_tmp315;struct Cyc_Absyn_Tvar*_tmp317;struct Cyc_Absyn_Vardecl*
_tmp318;int _tmp319;struct Cyc_Absyn_Exp*_tmp31A;struct Cyc_Absyn_Exp*_tmp31C;
struct Cyc_Absyn_Tvar*_tmp31D;struct Cyc_Absyn_Vardecl*_tmp31E;struct Cyc_Absyn_Typedefdecl*
_tmp320;struct _dyneither_ptr*_tmp322;struct Cyc_List_List*_tmp323;struct _tuple0*
_tmp325;struct Cyc_List_List*_tmp326;struct Cyc_List_List*_tmp328;struct Cyc_List_List*
_tmp32A;struct Cyc_List_List*_tmp32B;_LL1C2: {struct Cyc_Absyn_Var_d_struct*
_tmp2FD=(struct Cyc_Absyn_Var_d_struct*)_tmp2FC;if(_tmp2FD->tag != 0)goto _LL1C4;
else{_tmp2FE=_tmp2FD->f1;}}_LL1C3: Cyc_Absyndump_dumpvardecl(_tmp2FE,d->loc);goto
_LL1C1;_LL1C4: {struct Cyc_Absyn_Fn_d_struct*_tmp2FF=(struct Cyc_Absyn_Fn_d_struct*)
_tmp2FC;if(_tmp2FF->tag != 1)goto _LL1C6;else{_tmp300=_tmp2FF->f1;}}_LL1C5: Cyc_Absyndump_dumploc(
d->loc);switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL1E2: Cyc_Absyndump_dumpatts(
_tmp300->attributes);break;case Cyc_Cyclone_Gcc_c: _LL1E3: break;}if(_tmp300->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp32E=Cyc_Cyclone_c_compiler;_LL1E6: if(_tmp32E != 
Cyc_Cyclone_Vc_c)goto _LL1E8;_LL1E7:{const char*_tmp49B;Cyc_Absyndump_dump(((
_tmp49B="__inline",_tag_dyneither(_tmp49B,sizeof(char),9))));}goto _LL1E5;_LL1E8:;
_LL1E9:{const char*_tmp49C;Cyc_Absyndump_dump(((_tmp49C="inline",_tag_dyneither(
_tmp49C,sizeof(char),7))));}goto _LL1E5;_LL1E5:;}Cyc_Absyndump_dumpscope(_tmp300->sc);{
struct Cyc_Absyn_FnType_struct _tmp4A2;struct Cyc_Absyn_FnInfo _tmp4A1;struct Cyc_Absyn_FnType_struct*
_tmp4A0;void*t=(void*)((_tmp4A0=_cycalloc(sizeof(*_tmp4A0)),((_tmp4A0[0]=((
_tmp4A2.tag=10,((_tmp4A2.f1=((_tmp4A1.tvars=_tmp300->tvs,((_tmp4A1.effect=
_tmp300->effect,((_tmp4A1.ret_typ=_tmp300->ret_type,((_tmp4A1.args=((struct Cyc_List_List*(*)(
struct _tuple7*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,
_tmp300->args),((_tmp4A1.c_varargs=_tmp300->c_varargs,((_tmp4A1.cyc_varargs=
_tmp300->cyc_varargs,((_tmp4A1.rgn_po=_tmp300->rgn_po,((_tmp4A1.attributes=0,
_tmp4A1)))))))))))))))),_tmp4A2)))),_tmp4A0))));switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL1EA:((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(
struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(
0),t,Cyc_Absyndump_dump_atts_qvar,_tmp300);break;case Cyc_Cyclone_Vc_c: _LL1EB:((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))
Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_callconv_fdqvar,
_tmp300);break;}Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp300->body,
0);Cyc_Absyndump_dump_char((int)'}');goto _LL1C1;};_LL1C6: {struct Cyc_Absyn_Aggr_d_struct*
_tmp301=(struct Cyc_Absyn_Aggr_d_struct*)_tmp2FC;if(_tmp301->tag != 6)goto _LL1C8;
else{_tmp302=_tmp301->f1;}}_LL1C7: Cyc_Absyndump_dumpscope(_tmp302->sc);Cyc_Absyndump_dumpaggr_kind(
_tmp302->kind);Cyc_Absyndump_dumpqvar(_tmp302->name);Cyc_Absyndump_dumptvars(
_tmp302->tvs);if(_tmp302->impl == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp302->impl))->exist_vars
!= 0){const char*_tmp4A5;const char*_tmp4A4;const char*_tmp4A3;((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp302->impl))->exist_vars,((_tmp4A3="<",
_tag_dyneither(_tmp4A3,sizeof(char),2))),((_tmp4A4=">",_tag_dyneither(_tmp4A4,
sizeof(char),2))),((_tmp4A5=",",_tag_dyneither(_tmp4A5,sizeof(char),2))));}if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp302->impl))->rgn_po != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp302->impl))->rgn_po);}Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp302->impl))->fields);{const char*_tmp4A6;Cyc_Absyndump_dump(((
_tmp4A6="}",_tag_dyneither(_tmp4A6,sizeof(char),2))));}Cyc_Absyndump_dumpatts(
_tmp302->attributes);{const char*_tmp4A7;Cyc_Absyndump_dump(((_tmp4A7=";",
_tag_dyneither(_tmp4A7,sizeof(char),2))));};}goto _LL1C1;_LL1C8: {struct Cyc_Absyn_Datatype_d_struct*
_tmp303=(struct Cyc_Absyn_Datatype_d_struct*)_tmp2FC;if(_tmp303->tag != 7)goto
_LL1CA;else{_tmp304=_tmp303->f1;_tmp305=*_tmp304;_tmp306=_tmp305.sc;_tmp307=
_tmp305.name;_tmp308=_tmp305.tvs;_tmp309=_tmp305.fields;_tmp30A=_tmp305.is_extensible;}}
_LL1C9: Cyc_Absyndump_dumpscope(_tmp306);if(_tmp30A){const char*_tmp4A8;Cyc_Absyndump_dump(((
_tmp4A8="@extensible ",_tag_dyneither(_tmp4A8,sizeof(char),13))));}{const char*
_tmp4A9;Cyc_Absyndump_dump(((_tmp4A9="datatype ",_tag_dyneither(_tmp4A9,sizeof(
char),10))));}Cyc_Absyndump_dumpqvar(_tmp307);Cyc_Absyndump_dumptvars(_tmp308);
if(_tmp309 == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpdatatypefields((struct Cyc_List_List*)_tmp309->v);{const char*
_tmp4AA;Cyc_Absyndump_dump_nospace(((_tmp4AA="};",_tag_dyneither(_tmp4AA,sizeof(
char),3))));};}goto _LL1C1;_LL1CA: {struct Cyc_Absyn_Enum_d_struct*_tmp30B=(struct
Cyc_Absyn_Enum_d_struct*)_tmp2FC;if(_tmp30B->tag != 8)goto _LL1CC;else{_tmp30C=
_tmp30B->f1;_tmp30D=*_tmp30C;_tmp30E=_tmp30D.sc;_tmp30F=_tmp30D.name;_tmp310=
_tmp30D.fields;}}_LL1CB: Cyc_Absyndump_dumpscope(_tmp30E);{const char*_tmp4AB;Cyc_Absyndump_dump(((
_tmp4AB="enum ",_tag_dyneither(_tmp4AB,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(
_tmp30F);if(_tmp310 == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp310->v);{const
char*_tmp4AC;Cyc_Absyndump_dump_nospace(((_tmp4AC="};",_tag_dyneither(_tmp4AC,
sizeof(char),3))));};}return;_LL1CC: {struct Cyc_Absyn_Let_d_struct*_tmp311=(
struct Cyc_Absyn_Let_d_struct*)_tmp2FC;if(_tmp311->tag != 2)goto _LL1CE;else{
_tmp312=_tmp311->f1;_tmp313=_tmp311->f3;}}_LL1CD:{const char*_tmp4AD;Cyc_Absyndump_dump(((
_tmp4AD="let",_tag_dyneither(_tmp4AD,sizeof(char),4))));}Cyc_Absyndump_dumppat(
_tmp312);Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(_tmp313);Cyc_Absyndump_dump_semi();
goto _LL1C1;_LL1CE: {struct Cyc_Absyn_Letv_d_struct*_tmp314=(struct Cyc_Absyn_Letv_d_struct*)
_tmp2FC;if(_tmp314->tag != 3)goto _LL1D0;else{_tmp315=_tmp314->f1;}}_LL1CF:{const
char*_tmp4AE;Cyc_Absyndump_dump(((_tmp4AE="let ",_tag_dyneither(_tmp4AE,sizeof(
char),5))));}Cyc_Absyndump_dumpids(_tmp315);Cyc_Absyndump_dump_semi();goto _LL1C1;
_LL1D0: {struct Cyc_Absyn_Region_d_struct*_tmp316=(struct Cyc_Absyn_Region_d_struct*)
_tmp2FC;if(_tmp316->tag != 4)goto _LL1D2;else{_tmp317=_tmp316->f1;_tmp318=_tmp316->f2;
_tmp319=_tmp316->f3;_tmp31A=_tmp316->f4;}}_LL1D1:{const char*_tmp4AF;Cyc_Absyndump_dump(((
_tmp4AF="region",_tag_dyneither(_tmp4AF,sizeof(char),7))));}{const char*_tmp4B0;
Cyc_Absyndump_dump(((_tmp4B0="<",_tag_dyneither(_tmp4B0,sizeof(char),2))));}Cyc_Absyndump_dumptvar(
_tmp317);{const char*_tmp4B1;Cyc_Absyndump_dump(((_tmp4B1="> ",_tag_dyneither(
_tmp4B1,sizeof(char),3))));}Cyc_Absyndump_dumpqvar(_tmp318->name);if(_tmp319){
const char*_tmp4B2;Cyc_Absyndump_dump(((_tmp4B2="@resetable",_tag_dyneither(
_tmp4B2,sizeof(char),11))));}if(_tmp31A != 0){{const char*_tmp4B3;Cyc_Absyndump_dump(((
_tmp4B3=" = open(",_tag_dyneither(_tmp4B3,sizeof(char),9))));}Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_tmp31A);{const char*_tmp4B4;Cyc_Absyndump_dump(((_tmp4B4=")",
_tag_dyneither(_tmp4B4,sizeof(char),2))));};}Cyc_Absyndump_dump_semi();goto
_LL1C1;_LL1D2: {struct Cyc_Absyn_Alias_d_struct*_tmp31B=(struct Cyc_Absyn_Alias_d_struct*)
_tmp2FC;if(_tmp31B->tag != 5)goto _LL1D4;else{_tmp31C=_tmp31B->f1;_tmp31D=_tmp31B->f2;
_tmp31E=_tmp31B->f3;}}_LL1D3:{const char*_tmp4B5;Cyc_Absyndump_dump(((_tmp4B5="alias ",
_tag_dyneither(_tmp4B5,sizeof(char),7))));}{const char*_tmp4B6;Cyc_Absyndump_dump(((
_tmp4B6="<",_tag_dyneither(_tmp4B6,sizeof(char),2))));}Cyc_Absyndump_dumptvar(
_tmp31D);{const char*_tmp4B7;Cyc_Absyndump_dump(((_tmp4B7=">",_tag_dyneither(
_tmp4B7,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmp31E->name);{const char*
_tmp4B8;Cyc_Absyndump_dump(((_tmp4B8=" = ",_tag_dyneither(_tmp4B8,sizeof(char),4))));}
Cyc_Absyndump_dumpexp(_tmp31C);Cyc_Absyndump_dump_semi();goto _LL1C1;_LL1D4: {
struct Cyc_Absyn_Typedef_d_struct*_tmp31F=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp2FC;if(_tmp31F->tag != 9)goto _LL1D6;else{_tmp320=_tmp31F->f1;}}_LL1D5: if(!Cyc_Absyndump_expand_typedefs
 || _tmp320->defn != 0  && Cyc_Absynpp_is_anon_aggrtype((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp320->defn))->v)){{const char*_tmp4B9;Cyc_Absyndump_dump(((_tmp4B9="typedef",
_tag_dyneither(_tmp4B9,sizeof(char),8))));}{void*t;if(_tmp320->defn == 0)t=Cyc_Absyn_new_evar(
_tmp320->kind,0);else{t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp320->defn))->v;}((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(_tmp320->tq,t,Cyc_Absyndump_dumptypedefname,
_tmp320);Cyc_Absyndump_dumpatts(_tmp320->atts);Cyc_Absyndump_dump_semi();};}goto
_LL1C1;_LL1D6: {struct Cyc_Absyn_Namespace_d_struct*_tmp321=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp2FC;if(_tmp321->tag != 10)goto _LL1D8;else{_tmp322=_tmp321->f1;_tmp323=_tmp321->f2;}}
_LL1D7:{const char*_tmp4BA;Cyc_Absyndump_dump(((_tmp4BA="namespace",
_tag_dyneither(_tmp4BA,sizeof(char),10))));}Cyc_Absyndump_dump_str(_tmp322);Cyc_Absyndump_dump_char((
int)'{');for(0;_tmp323 != 0;_tmp323=_tmp323->tl){Cyc_Absyndump_dumpdecl((struct
Cyc_Absyn_Decl*)_tmp323->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1C1;
_LL1D8: {struct Cyc_Absyn_Using_d_struct*_tmp324=(struct Cyc_Absyn_Using_d_struct*)
_tmp2FC;if(_tmp324->tag != 11)goto _LL1DA;else{_tmp325=_tmp324->f1;_tmp326=_tmp324->f2;}}
_LL1D9:{const char*_tmp4BB;Cyc_Absyndump_dump(((_tmp4BB="using",_tag_dyneither(
_tmp4BB,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(_tmp325);Cyc_Absyndump_dump_char((
int)'{');for(0;_tmp326 != 0;_tmp326=_tmp326->tl){Cyc_Absyndump_dumpdecl((struct
Cyc_Absyn_Decl*)_tmp326->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1C1;
_LL1DA: {struct Cyc_Absyn_ExternC_d_struct*_tmp327=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp2FC;if(_tmp327->tag != 12)goto _LL1DC;else{_tmp328=_tmp327->f1;}}_LL1DB:{const
char*_tmp4BC;Cyc_Absyndump_dump(((_tmp4BC="extern \"C\" {",_tag_dyneither(
_tmp4BC,sizeof(char),13))));}for(0;_tmp328 != 0;_tmp328=_tmp328->tl){Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)_tmp328->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1C1;
_LL1DC: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp329=(struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp2FC;if(_tmp329->tag != 13)goto _LL1DE;else{_tmp32A=_tmp329->f1;_tmp32B=_tmp329->f2;}}
_LL1DD:{const char*_tmp4BD;Cyc_Absyndump_dump(((_tmp4BD="extern \"C include\" {",
_tag_dyneither(_tmp4BD,sizeof(char),21))));}for(0;_tmp32A != 0;_tmp32A=_tmp32A->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp32A->hd);}Cyc_Absyndump_dump_char((
int)'}');if(_tmp32B != 0){{const char*_tmp4BE;Cyc_Absyndump_dump(((_tmp4BE=" export {",
_tag_dyneither(_tmp4BE,sizeof(char),10))));}for(0;_tmp32B != 0;_tmp32B=_tmp32B->tl){
struct _tuple0*_tmp351;struct _tuple19 _tmp350=*((struct _tuple19*)_tmp32B->hd);
_tmp351=_tmp350.f2;Cyc_Absyndump_dumpqvar(_tmp351);if(_tmp32B->tl != 0)Cyc_Absyndump_dump_char((
int)',');}{const char*_tmp4BF;Cyc_Absyndump_dump(((_tmp4BF="}",_tag_dyneither(
_tmp4BF,sizeof(char),2))));};}goto _LL1C1;_LL1DE: {struct Cyc_Absyn_Porton_d_struct*
_tmp32C=(struct Cyc_Absyn_Porton_d_struct*)_tmp2FC;if(_tmp32C->tag != 14)goto
_LL1E0;}_LL1DF:{const char*_tmp4C0;Cyc_Absyndump_dump(((_tmp4C0=" __cyclone_port_on__; ",
_tag_dyneither(_tmp4C0,sizeof(char),23))));}goto _LL1C1;_LL1E0: {struct Cyc_Absyn_Portoff_d_struct*
_tmp32D=(struct Cyc_Absyn_Portoff_d_struct*)_tmp2FC;if(_tmp32D->tag != 15)goto
_LL1C1;}_LL1E1:{const char*_tmp4C1;Cyc_Absyndump_dump(((_tmp4C1=" __cyclone_port_off__; ",
_tag_dyneither(_tmp4C1,sizeof(char),24))));}goto _LL1C1;_LL1C1:;}static void Cyc_Absyndump_dump_upperbound(
struct Cyc_Absyn_Exp*e);static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*
e){struct _tuple12 pr=Cyc_Evexp_eval_const_uint_exp(e);if(pr.f1 != 1  || !pr.f2){Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}void Cyc_Absyndump_dumptms(
struct Cyc_List_List*tms,void(*f)(void*),void*a);void Cyc_Absyndump_dumptms(struct
Cyc_List_List*tms,void(*f)(void*),void*a){if(tms == 0){f(a);return;}{void*_tmp355=(
void*)tms->hd;struct Cyc_Absyn_PtrAtts _tmp357;void*_tmp358;union Cyc_Absyn_Constraint*
_tmp359;union Cyc_Absyn_Constraint*_tmp35A;union Cyc_Absyn_Constraint*_tmp35B;
struct Cyc_Absyn_Tqual _tmp35C;_LL1EE: {struct Cyc_Absyn_Pointer_mod_struct*_tmp356=(
struct Cyc_Absyn_Pointer_mod_struct*)_tmp355;if(_tmp356->tag != 2)goto _LL1F0;else{
_tmp357=_tmp356->f1;_tmp358=_tmp357.rgn;_tmp359=_tmp357.nullable;_tmp35A=_tmp357.bounds;
_tmp35B=_tmp357.zero_term;_tmp35C=_tmp356->f2;}}_LL1EF:{void*_tmp35D=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp35A);struct Cyc_Absyn_Exp*_tmp360;_LL1F3: {struct Cyc_Absyn_DynEither_b_struct*
_tmp35E=(struct Cyc_Absyn_DynEither_b_struct*)_tmp35D;if(_tmp35E->tag != 0)goto
_LL1F5;}_LL1F4: Cyc_Absyndump_dump_char((int)'?');goto _LL1F2;_LL1F5: {struct Cyc_Absyn_Upper_b_struct*
_tmp35F=(struct Cyc_Absyn_Upper_b_struct*)_tmp35D;if(_tmp35F->tag != 1)goto _LL1F2;
else{_tmp360=_tmp35F->f1;}}_LL1F6: Cyc_Absyndump_dump_char((int)(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp359)?'*':'@'));Cyc_Absyndump_dump_upperbound(
_tmp360);goto _LL1F2;_LL1F2:;}if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp35B)){const char*_tmp4C2;Cyc_Absyndump_dump(((_tmp4C2="@zeroterm",
_tag_dyneither(_tmp4C2,sizeof(char),10))));}{void*_tmp362=Cyc_Tcutil_compress(
_tmp358);struct Cyc_Absyn_Tvar*_tmp366;struct Cyc_Core_Opt*_tmp368;_LL1F8: {struct
Cyc_Absyn_HeapRgn_struct*_tmp363=(struct Cyc_Absyn_HeapRgn_struct*)_tmp362;if(
_tmp363->tag != 21)goto _LL1FA;}_LL1F9: goto _LL1F7;_LL1FA: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp364=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp362;if(_tmp364->tag != 22)goto
_LL1FC;}_LL1FB:{const char*_tmp4C3;Cyc_Absyndump_dump(((_tmp4C3="`U",
_tag_dyneither(_tmp4C3,sizeof(char),3))));}goto _LL1F7;_LL1FC: {struct Cyc_Absyn_VarType_struct*
_tmp365=(struct Cyc_Absyn_VarType_struct*)_tmp362;if(_tmp365->tag != 2)goto _LL1FE;
else{_tmp366=_tmp365->f1;}}_LL1FD: Cyc_Absyndump_dump_str(_tmp366->name);goto
_LL1F7;_LL1FE: {struct Cyc_Absyn_Evar_struct*_tmp367=(struct Cyc_Absyn_Evar_struct*)
_tmp362;if(_tmp367->tag != 1)goto _LL200;else{_tmp368=_tmp367->f2;if(_tmp368 != 0)
goto _LL200;}}_LL1FF: Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(_tmp358));goto
_LL1F7;_LL200:;_LL201: {const char*_tmp4C6;void*_tmp4C5;(_tmp4C5=0,Cyc_Tcutil_impos(((
_tmp4C6="dumptms: bad rgn type in Pointer_mod",_tag_dyneither(_tmp4C6,sizeof(
char),37))),_tag_dyneither(_tmp4C5,sizeof(void*),0)));}_LL1F7:;}Cyc_Absyndump_dumptq(
_tmp35C);Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL1F0:;_LL1F1: {int
next_is_pointer=0;if(tms->tl != 0){void*_tmp36C=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;_LL203: {struct Cyc_Absyn_Pointer_mod_struct*_tmp36D=(
struct Cyc_Absyn_Pointer_mod_struct*)_tmp36C;if(_tmp36D->tag != 2)goto _LL205;}
_LL204: next_is_pointer=1;goto _LL202;_LL205:;_LL206: goto _LL202;_LL202:;}if(
next_is_pointer)Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptms(tms->tl,
f,a);if(next_is_pointer)Cyc_Absyndump_dump_char((int)')');{void*_tmp36E=(void*)
tms->hd;union Cyc_Absyn_Constraint*_tmp370;struct Cyc_Absyn_Exp*_tmp372;union Cyc_Absyn_Constraint*
_tmp373;void*_tmp375;struct Cyc_List_List*_tmp377;int _tmp378;struct Cyc_Absyn_VarargInfo*
_tmp379;struct Cyc_Core_Opt*_tmp37A;struct Cyc_List_List*_tmp37B;void*_tmp37D;
struct Cyc_List_List*_tmp37F;struct Cyc_Position_Segment*_tmp380;struct Cyc_List_List*
_tmp382;struct Cyc_Position_Segment*_tmp383;int _tmp384;struct Cyc_List_List*
_tmp386;_LL208: {struct Cyc_Absyn_Carray_mod_struct*_tmp36F=(struct Cyc_Absyn_Carray_mod_struct*)
_tmp36E;if(_tmp36F->tag != 0)goto _LL20A;else{_tmp370=_tmp36F->f1;}}_LL209:{const
char*_tmp4C7;Cyc_Absyndump_dump(((_tmp4C7="[]",_tag_dyneither(_tmp4C7,sizeof(
char),3))));}if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp370)){const char*_tmp4C8;Cyc_Absyndump_dump(((_tmp4C8="@zeroterm",
_tag_dyneither(_tmp4C8,sizeof(char),10))));}goto _LL207;_LL20A: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmp371=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmp36E;if(_tmp371->tag != 1)goto
_LL20C;else{_tmp372=_tmp371->f1;_tmp373=_tmp371->f2;}}_LL20B: Cyc_Absyndump_dump_char((
int)'[');Cyc_Absyndump_dumpexp(_tmp372);Cyc_Absyndump_dump_char((int)']');if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp373)){const
char*_tmp4C9;Cyc_Absyndump_dump(((_tmp4C9="@zeroterm",_tag_dyneither(_tmp4C9,
sizeof(char),10))));}goto _LL207;_LL20C: {struct Cyc_Absyn_Function_mod_struct*
_tmp374=(struct Cyc_Absyn_Function_mod_struct*)_tmp36E;if(_tmp374->tag != 3)goto
_LL20E;else{_tmp375=(void*)_tmp374->f1;{struct Cyc_Absyn_WithTypes_struct*_tmp376=(
struct Cyc_Absyn_WithTypes_struct*)_tmp375;if(_tmp376->tag != 1)goto _LL20E;else{
_tmp377=_tmp376->f1;_tmp378=_tmp376->f2;_tmp379=_tmp376->f3;_tmp37A=_tmp376->f4;
_tmp37B=_tmp376->f5;}};}}_LL20D: Cyc_Absyndump_dumpfunargs(_tmp377,_tmp378,
_tmp379,_tmp37A,_tmp37B);goto _LL207;_LL20E: {struct Cyc_Absyn_Function_mod_struct*
_tmp37C=(struct Cyc_Absyn_Function_mod_struct*)_tmp36E;if(_tmp37C->tag != 3)goto
_LL210;else{_tmp37D=(void*)_tmp37C->f1;{struct Cyc_Absyn_NoTypes_struct*_tmp37E=(
struct Cyc_Absyn_NoTypes_struct*)_tmp37D;if(_tmp37E->tag != 0)goto _LL210;else{
_tmp37F=_tmp37E->f1;_tmp380=_tmp37E->f2;}};}}_LL20F:{const char*_tmp4CC;const char*
_tmp4CB;const char*_tmp4CA;((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dump_str,_tmp37F,((_tmp4CA="(",_tag_dyneither(_tmp4CA,sizeof(char),
2))),((_tmp4CB=")",_tag_dyneither(_tmp4CB,sizeof(char),2))),((_tmp4CC=",",
_tag_dyneither(_tmp4CC,sizeof(char),2))));}goto _LL207;_LL210: {struct Cyc_Absyn_TypeParams_mod_struct*
_tmp381=(struct Cyc_Absyn_TypeParams_mod_struct*)_tmp36E;if(_tmp381->tag != 4)goto
_LL212;else{_tmp382=_tmp381->f1;_tmp383=_tmp381->f2;_tmp384=_tmp381->f3;}}_LL211:
if(_tmp384)Cyc_Absyndump_dumpkindedtvars(_tmp382);else{Cyc_Absyndump_dumptvars(
_tmp382);}goto _LL207;_LL212: {struct Cyc_Absyn_Attributes_mod_struct*_tmp385=(
struct Cyc_Absyn_Attributes_mod_struct*)_tmp36E;if(_tmp385->tag != 5)goto _LL214;
else{_tmp386=_tmp385->f2;}}_LL213: Cyc_Absyndump_dumpatts(_tmp386);goto _LL207;
_LL214: {struct Cyc_Absyn_Pointer_mod_struct*_tmp387=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp36E;if(_tmp387->tag != 2)goto _LL207;}_LL215: {const char*_tmp4CF;void*_tmp4CE;(
_tmp4CE=0,Cyc_Tcutil_impos(((_tmp4CF="dumptms",_tag_dyneither(_tmp4CF,sizeof(
char),8))),_tag_dyneither(_tmp4CE,sizeof(void*),0)));}_LL207:;}return;}_LL1ED:;};}
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a);
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
struct _RegionHandle _tmp390=_new_region("temp");struct _RegionHandle*temp=& _tmp390;
_push_region(temp);{struct Cyc_Absyn_Tqual _tmp392;void*_tmp393;struct Cyc_List_List*
_tmp394;struct _tuple11 _tmp391=Cyc_Absynpp_to_tms(temp,tq,t);_tmp392=_tmp391.f1;
_tmp393=_tmp391.f2;_tmp394=_tmp391.f3;Cyc_Absyndump_dumptq(_tmp392);Cyc_Absyndump_dumpntyp(
_tmp393);Cyc_Absyndump_dumptms(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp394),f,a);};_pop_region(temp);}void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){Cyc_Absyndump_pos=0;*Cyc_Absyndump_dump_file=
f;for(0;tdl != 0;tdl=tdl->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}{
const char*_tmp4D2;void*_tmp4D1;(_tmp4D1=0,Cyc_fprintf(f,((_tmp4D2="\n",
_tag_dyneither(_tmp4D2,sizeof(char),2))),_tag_dyneither(_tmp4D1,sizeof(void*),0)));};}
