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
Cyc_Absyndump_dump_char((int)'_');else{const char*_tmp39D;Cyc_Absyndump_dump_nospace(((
_tmp39D="::",_tag_dyneither(_tmp39D,sizeof(char),3))));}}if(_tmp1 != 0){Cyc_Absyndump_dump_nospace(*((
struct _dyneither_ptr*)_tmp1->hd));for(_tmp1=_tmp1->tl;_tmp1 != 0;_tmp1=_tmp1->tl){
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{const char*
_tmp39E;Cyc_Absyndump_dump_nospace(((_tmp39E="::",_tag_dyneither(_tmp39E,sizeof(
char),3))));}Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmp1->hd));}
if(Cyc_Absyndump_qvar_to_Cids){const char*_tmp39F;Cyc_Absyndump_dump_nospace(((
_tmp39F="_",_tag_dyneither(_tmp39F,sizeof(char),2))));}else{const char*_tmp3A0;
Cyc_Absyndump_dump_nospace(((_tmp3A0="::",_tag_dyneither(_tmp3A0,sizeof(char),3))));}
Cyc_Absyndump_dump_nospace(*(*v).f2);}else{if(prefix != 0)Cyc_Absyndump_dump_nospace(*(*
v).f2);else{Cyc_Absyndump_dump_str((*v).f2);}}}void Cyc_Absyndump_dumptq(struct
Cyc_Absyn_Tqual tq);void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){if(tq.q_restrict){
const char*_tmp3A1;Cyc_Absyndump_dump(((_tmp3A1="restrict",_tag_dyneither(_tmp3A1,
sizeof(char),9))));}if(tq.q_volatile){const char*_tmp3A2;Cyc_Absyndump_dump(((
_tmp3A2="volatile",_tag_dyneither(_tmp3A2,sizeof(char),9))));}if(tq.print_const){
const char*_tmp3A3;Cyc_Absyndump_dump(((_tmp3A3="const",_tag_dyneither(_tmp3A3,
sizeof(char),6))));}}void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope sc);void Cyc_Absyndump_dumpscope(
enum Cyc_Absyn_Scope sc){switch(sc){case Cyc_Absyn_Static: _LL17:{const char*_tmp3A4;
Cyc_Absyndump_dump(((_tmp3A4="static",_tag_dyneither(_tmp3A4,sizeof(char),7))));}
return;case Cyc_Absyn_Public: _LL18: return;case Cyc_Absyn_Extern: _LL19:{const char*
_tmp3A5;Cyc_Absyndump_dump(((_tmp3A5="extern",_tag_dyneither(_tmp3A5,sizeof(char),
7))));}return;case Cyc_Absyn_ExternC: _LL1A:{const char*_tmp3A6;Cyc_Absyndump_dump(((
_tmp3A6="extern \"C\"",_tag_dyneither(_tmp3A6,sizeof(char),11))));}return;case
Cyc_Absyn_Abstract: _LL1B:{const char*_tmp3A7;Cyc_Absyndump_dump(((_tmp3A7="abstract",
_tag_dyneither(_tmp3A7,sizeof(char),9))));}return;case Cyc_Absyn_Register: _LL1C:{
const char*_tmp3A8;Cyc_Absyndump_dump(((_tmp3A8="register",_tag_dyneither(_tmp3A8,
sizeof(char),9))));}return;}}void Cyc_Absyndump_dumpkind(struct Cyc_Absyn_Kind*ka);
void Cyc_Absyndump_dumpkind(struct Cyc_Absyn_Kind*ka){struct Cyc_Absyn_Kind _tmp13;
enum Cyc_Absyn_KindQual _tmp14;enum Cyc_Absyn_AliasQual _tmp15;struct Cyc_Absyn_Kind*
_tmp12=ka;_tmp13=*_tmp12;_tmp14=_tmp13.kind;_tmp15=_tmp13.aliasqual;switch(
_tmp14){case Cyc_Absyn_AnyKind: _LL1E: switch(_tmp15){case Cyc_Absyn_Aliasable: _LL20:{
const char*_tmp3A9;Cyc_Absyndump_dump(((_tmp3A9="A",_tag_dyneither(_tmp3A9,
sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL21:{const char*_tmp3AA;Cyc_Absyndump_dump(((
_tmp3AA="UA",_tag_dyneither(_tmp3AA,sizeof(char),3))));}return;case Cyc_Absyn_Top:
_LL22:{const char*_tmp3AB;Cyc_Absyndump_dump(((_tmp3AB="TA",_tag_dyneither(
_tmp3AB,sizeof(char),3))));}return;}case Cyc_Absyn_MemKind: _LL1F: switch(_tmp15){
case Cyc_Absyn_Aliasable: _LL25:{const char*_tmp3AC;Cyc_Absyndump_dump(((_tmp3AC="M",
_tag_dyneither(_tmp3AC,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL26:{
const char*_tmp3AD;Cyc_Absyndump_dump(((_tmp3AD="UM",_tag_dyneither(_tmp3AD,
sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL27:{const char*_tmp3AE;Cyc_Absyndump_dump(((
_tmp3AE="TM",_tag_dyneither(_tmp3AE,sizeof(char),3))));}return;}case Cyc_Absyn_BoxKind:
_LL24: switch(_tmp15){case Cyc_Absyn_Aliasable: _LL2A:{const char*_tmp3AF;Cyc_Absyndump_dump(((
_tmp3AF="B",_tag_dyneither(_tmp3AF,sizeof(char),2))));}return;case Cyc_Absyn_Unique:
_LL2B:{const char*_tmp3B0;Cyc_Absyndump_dump(((_tmp3B0="UB",_tag_dyneither(
_tmp3B0,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL2C:{const char*_tmp3B1;
Cyc_Absyndump_dump(((_tmp3B1="TB",_tag_dyneither(_tmp3B1,sizeof(char),3))));}
return;}case Cyc_Absyn_RgnKind: _LL29: switch(_tmp15){case Cyc_Absyn_Aliasable: _LL2F:{
const char*_tmp3B2;Cyc_Absyndump_dump(((_tmp3B2="R",_tag_dyneither(_tmp3B2,
sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL30:{const char*_tmp3B3;Cyc_Absyndump_dump(((
_tmp3B3="UR",_tag_dyneither(_tmp3B3,sizeof(char),3))));}return;case Cyc_Absyn_Top:
_LL31:{const char*_tmp3B4;Cyc_Absyndump_dump(((_tmp3B4="TR",_tag_dyneither(
_tmp3B4,sizeof(char),3))));}return;}case Cyc_Absyn_EffKind: _LL2E:{const char*
_tmp3B5;Cyc_Absyndump_dump(((_tmp3B5="E",_tag_dyneither(_tmp3B5,sizeof(char),2))));}
return;case Cyc_Absyn_IntKind: _LL33:{const char*_tmp3B6;Cyc_Absyndump_dump(((
_tmp3B6="I",_tag_dyneither(_tmp3B6,sizeof(char),2))));}return;}}void Cyc_Absyndump_dumpaggr_kind(
enum Cyc_Absyn_AggrKind k);void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind
k){switch(k){case Cyc_Absyn_StructA: _LL35:{const char*_tmp3B7;Cyc_Absyndump_dump(((
_tmp3B7="struct",_tag_dyneither(_tmp3B7,sizeof(char),7))));}return;case Cyc_Absyn_UnionA:
_LL36:{const char*_tmp3B8;Cyc_Absyndump_dump(((_tmp3B8="union",_tag_dyneither(
_tmp3B8,sizeof(char),6))));}return;}}void Cyc_Absyndump_dumptps(struct Cyc_List_List*
ts);void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){const char*_tmp3BB;const
char*_tmp3BA;const char*_tmp3B9;((void(*)(void(*f)(void*),struct Cyc_List_List*l,
struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptyp,ts,((_tmp3B9="<",_tag_dyneither(_tmp3B9,sizeof(char),2))),((
_tmp3BA=">",_tag_dyneither(_tmp3BA,sizeof(char),2))),((_tmp3BB=",",
_tag_dyneither(_tmp3BB,sizeof(char),2))));}void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*
tv);void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(
tv->name);}void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv);void Cyc_Absyndump_dumpkindedtvar(
struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(tv->name);{void*_tmp29=Cyc_Absyn_compress_kb(
tv->kind);struct Cyc_Absyn_Kind*_tmp2B;struct Cyc_Absyn_Kind*_tmp2F;_LL39: {struct
Cyc_Absyn_Eq_kb_struct*_tmp2A=(struct Cyc_Absyn_Eq_kb_struct*)_tmp29;if(_tmp2A->tag
!= 0)goto _LL3B;else{_tmp2B=_tmp2A->f1;if(_tmp2B != Cyc_Absyn_BoxKind)goto _LL3B;}}
_LL3A: goto _LL3C;_LL3B: {struct Cyc_Absyn_Less_kb_struct*_tmp2C=(struct Cyc_Absyn_Less_kb_struct*)
_tmp29;if(_tmp2C->tag != 2)goto _LL3D;}_LL3C: goto _LL3E;_LL3D: {struct Cyc_Absyn_Unknown_kb_struct*
_tmp2D=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp29;if(_tmp2D->tag != 1)goto _LL3F;}
_LL3E:{const char*_tmp3BC;Cyc_Absyndump_dump(((_tmp3BC="::?",_tag_dyneither(
_tmp3BC,sizeof(char),4))));}goto _LL38;_LL3F: {struct Cyc_Absyn_Eq_kb_struct*
_tmp2E=(struct Cyc_Absyn_Eq_kb_struct*)_tmp29;if(_tmp2E->tag != 0)goto _LL38;else{
_tmp2F=_tmp2E->f1;}}_LL40:{const char*_tmp3BD;Cyc_Absyndump_dump(((_tmp3BD="::",
_tag_dyneither(_tmp3BD,sizeof(char),3))));}Cyc_Absyndump_dumpkind(_tmp2F);goto
_LL38;_LL38:;};}void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs);void Cyc_Absyndump_dumptvars(
struct Cyc_List_List*tvs){const char*_tmp3C0;const char*_tmp3BF;const char*_tmp3BE;((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptvar,tvs,((_tmp3BE="<",_tag_dyneither(_tmp3BE,sizeof(char),2))),((
_tmp3BF=">",_tag_dyneither(_tmp3BF,sizeof(char),2))),((_tmp3C0=",",
_tag_dyneither(_tmp3C0,sizeof(char),2))));}void Cyc_Absyndump_dumpkindedtvars(
struct Cyc_List_List*tvs);void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*
tvs){const char*_tmp3C3;const char*_tmp3C2;const char*_tmp3C1;((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,
tvs,((_tmp3C1="<",_tag_dyneither(_tmp3C1,sizeof(char),2))),((_tmp3C2=">",
_tag_dyneither(_tmp3C2,sizeof(char),2))),((_tmp3C3=",",_tag_dyneither(_tmp3C3,
sizeof(char),2))));}struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;};void Cyc_Absyndump_dumparg(
struct _tuple13*pr);void Cyc_Absyndump_dumparg(struct _tuple13*pr){((void(*)(struct
Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*pr).f1,(*pr).f2,(
void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpargs(struct Cyc_List_List*
ts);void Cyc_Absyndump_dumpargs(struct Cyc_List_List*ts){const char*_tmp3C6;const
char*_tmp3C5;const char*_tmp3C4;((void(*)(void(*f)(struct _tuple13*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumparg,ts,((_tmp3C4="(",_tag_dyneither(_tmp3C4,sizeof(char),2))),((
_tmp3C5=")",_tag_dyneither(_tmp3C5,sizeof(char),2))),((_tmp3C6=",",
_tag_dyneither(_tmp3C6,sizeof(char),2))));}void Cyc_Absyndump_dump_callconv(
struct Cyc_List_List*atts);void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*
atts){for(0;atts != 0;atts=atts->tl){void*_tmp3B=(void*)atts->hd;_LL42: {struct
Cyc_Absyn_Stdcall_att_struct*_tmp3C=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp3B;
if(_tmp3C->tag != 1)goto _LL44;}_LL43:{const char*_tmp3C7;Cyc_Absyndump_dump(((
_tmp3C7="_stdcall",_tag_dyneither(_tmp3C7,sizeof(char),9))));}return;_LL44: {
struct Cyc_Absyn_Cdecl_att_struct*_tmp3D=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmp3B;if(_tmp3D->tag != 2)goto _LL46;}_LL45:{const char*_tmp3C8;Cyc_Absyndump_dump(((
_tmp3C8="_cdecl",_tag_dyneither(_tmp3C8,sizeof(char),7))));}return;_LL46: {
struct Cyc_Absyn_Fastcall_att_struct*_tmp3E=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp3B;if(_tmp3E->tag != 3)goto _LL48;}_LL47:{const char*_tmp3C9;Cyc_Absyndump_dump(((
_tmp3C9="_fastcall",_tag_dyneither(_tmp3C9,sizeof(char),10))));}return;_LL48:;
_LL49: goto _LL41;_LL41:;}}void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*
atts);void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){int hasatt=0;{
struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){void*_tmp42=(
void*)((struct Cyc_List_List*)_check_null(atts))->hd;_LL4B: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp43=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp42;if(_tmp43->tag != 1)goto _LL4D;}
_LL4C: goto _LL4E;_LL4D: {struct Cyc_Absyn_Cdecl_att_struct*_tmp44=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmp42;if(_tmp44->tag != 2)goto _LL4F;}_LL4E: goto _LL50;_LL4F: {struct Cyc_Absyn_Fastcall_att_struct*
_tmp45=(struct Cyc_Absyn_Fastcall_att_struct*)_tmp42;if(_tmp45->tag != 3)goto _LL51;}
_LL50: goto _LL4A;_LL51:;_LL52: hasatt=1;goto _LL4A;_LL4A:;}}if(!hasatt)return;{
const char*_tmp3CA;Cyc_Absyndump_dump(((_tmp3CA="__declspec(",_tag_dyneither(
_tmp3CA,sizeof(char),12))));}for(0;atts != 0;atts=atts->tl){void*_tmp47=(void*)
atts->hd;_LL54: {struct Cyc_Absyn_Stdcall_att_struct*_tmp48=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp47;if(_tmp48->tag != 1)goto _LL56;}_LL55: goto _LL57;_LL56: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp49=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp47;if(_tmp49->tag != 2)goto _LL58;}
_LL57: goto _LL59;_LL58: {struct Cyc_Absyn_Fastcall_att_struct*_tmp4A=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp47;if(_tmp4A->tag != 3)goto _LL5A;}_LL59: goto _LL53;_LL5A:;_LL5B: Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string((void*)atts->hd));goto _LL53;_LL53:;}Cyc_Absyndump_dump_char((
int)')');}void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts);void Cyc_Absyndump_dumpatts(
struct Cyc_List_List*atts){if(atts == 0)return;switch(Cyc_Cyclone_c_compiler){case
Cyc_Cyclone_Gcc_c: _LL5C:{const char*_tmp3CB;Cyc_Absyndump_dump(((_tmp3CB=" __attribute__((",
_tag_dyneither(_tmp3CB,sizeof(char),17))));}for(0;atts != 0;atts=atts->tl){Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string((void*)atts->hd));if(atts->tl != 0){const char*_tmp3CC;
Cyc_Absyndump_dump(((_tmp3CC=",",_tag_dyneither(_tmp3CC,sizeof(char),2))));}}{
const char*_tmp3CD;Cyc_Absyndump_dump(((_tmp3CD=")) ",_tag_dyneither(_tmp3CD,
sizeof(char),4))));}return;case Cyc_Cyclone_Vc_c: _LL5D: Cyc_Absyndump_dump_noncallconv(
atts);return;}}int Cyc_Absyndump_next_is_pointer(struct Cyc_List_List*tms);int Cyc_Absyndump_next_is_pointer(
struct Cyc_List_List*tms){if(tms == 0)return 0;{void*_tmp4E=(void*)tms->hd;_LL60: {
struct Cyc_Absyn_Pointer_mod_struct*_tmp4F=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp4E;if(_tmp4F->tag != 2)goto _LL62;}_LL61: return 1;_LL62:;_LL63: return 0;_LL5F:;};}
static void Cyc_Absyndump_dumprgn(void*t);static void Cyc_Absyndump_dumprgn(void*t){
void*_tmp50=Cyc_Tcutil_compress(t);_LL65: {struct Cyc_Absyn_HeapRgn_struct*_tmp51=(
struct Cyc_Absyn_HeapRgn_struct*)_tmp50;if(_tmp51->tag != 21)goto _LL67;}_LL66:{
const char*_tmp3CE;Cyc_Absyndump_dump(((_tmp3CE="`H",_tag_dyneither(_tmp3CE,
sizeof(char),3))));}goto _LL64;_LL67:;_LL68: Cyc_Absyndump_dumpntyp(t);goto _LL64;
_LL64:;}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};static
struct _tuple14 Cyc_Absyndump_effects_split(void*t);static struct _tuple14 Cyc_Absyndump_effects_split(
void*t){struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;{void*_tmp53=
Cyc_Tcutil_compress(t);void*_tmp55;struct Cyc_List_List*_tmp57;_LL6A: {struct Cyc_Absyn_AccessEff_struct*
_tmp54=(struct Cyc_Absyn_AccessEff_struct*)_tmp53;if(_tmp54->tag != 23)goto _LL6C;
else{_tmp55=(void*)_tmp54->f1;}}_LL6B:{struct Cyc_List_List*_tmp3CF;rgions=((
_tmp3CF=_cycalloc(sizeof(*_tmp3CF)),((_tmp3CF->hd=_tmp55,((_tmp3CF->tl=rgions,
_tmp3CF))))));}goto _LL69;_LL6C: {struct Cyc_Absyn_JoinEff_struct*_tmp56=(struct
Cyc_Absyn_JoinEff_struct*)_tmp53;if(_tmp56->tag != 24)goto _LL6E;else{_tmp57=
_tmp56->f1;}}_LL6D: for(0;_tmp57 != 0;_tmp57=_tmp57->tl){struct Cyc_List_List*
_tmp5A;struct Cyc_List_List*_tmp5B;struct _tuple14 _tmp59=Cyc_Absyndump_effects_split((
void*)_tmp57->hd);_tmp5A=_tmp59.f1;_tmp5B=_tmp59.f2;rgions=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp5A,rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp5B,effects);}goto _LL69;_LL6E:;_LL6F:{struct Cyc_List_List*
_tmp3D0;effects=((_tmp3D0=_cycalloc(sizeof(*_tmp3D0)),((_tmp3D0->hd=t,((_tmp3D0->tl=
effects,_tmp3D0))))));}goto _LL69;_LL69:;}{struct _tuple14 _tmp3D1;return(_tmp3D1.f1=
rgions,((_tmp3D1.f2=effects,_tmp3D1)));};}static void Cyc_Absyndump_dumpeff(void*t);
static void Cyc_Absyndump_dumpeff(void*t){struct Cyc_List_List*_tmp5F;struct Cyc_List_List*
_tmp60;struct _tuple14 _tmp5E=Cyc_Absyndump_effects_split(t);_tmp5F=_tmp5E.f1;
_tmp60=_tmp5E.f2;_tmp5F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp5F);_tmp60=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp60);for(0;_tmp60 != 0;_tmp60=_tmp60->tl){Cyc_Absyndump_dumpntyp((void*)_tmp60->hd);
Cyc_Absyndump_dump_char((int)'+');}Cyc_Absyndump_dump_char((int)'{');for(0;
_tmp5F != 0;_tmp5F=_tmp5F->tl){Cyc_Absyndump_dumprgn((void*)_tmp5F->hd);if(_tmp5F->tl
!= 0)Cyc_Absyndump_dump_char((int)',');}Cyc_Absyndump_dump_char((int)'}');}void
Cyc_Absyndump_dumpntyp(void*t);void Cyc_Absyndump_dumpntyp(void*t){void*_tmp61=t;
struct Cyc_Absyn_Tvar*_tmp67;struct Cyc_Core_Opt*_tmp69;struct Cyc_Core_Opt*_tmp6A;
int _tmp6B;struct Cyc_Core_Opt*_tmp6D;struct Cyc_Core_Opt*_tmp6E;struct Cyc_Core_Opt
_tmp6F;void*_tmp70;int _tmp71;struct Cyc_Absyn_DatatypeInfo _tmp73;union Cyc_Absyn_DatatypeInfoU
_tmp74;struct Cyc_List_List*_tmp75;struct Cyc_Absyn_DatatypeFieldInfo _tmp77;union
Cyc_Absyn_DatatypeFieldInfoU _tmp78;struct Cyc_List_List*_tmp79;enum Cyc_Absyn_Sign
_tmp7B;enum Cyc_Absyn_Size_of _tmp7C;enum Cyc_Absyn_Sign _tmp7E;enum Cyc_Absyn_Size_of
_tmp7F;enum Cyc_Absyn_Sign _tmp81;enum Cyc_Absyn_Size_of _tmp82;enum Cyc_Absyn_Sign
_tmp84;enum Cyc_Absyn_Size_of _tmp85;enum Cyc_Absyn_Sign _tmp87;enum Cyc_Absyn_Size_of
_tmp88;enum Cyc_Absyn_Sign _tmp8A;enum Cyc_Absyn_Size_of _tmp8B;enum Cyc_Absyn_Sign
_tmp8D;enum Cyc_Absyn_Size_of _tmp8E;enum Cyc_Absyn_Sign _tmp90;enum Cyc_Absyn_Size_of
_tmp91;enum Cyc_Absyn_Sign _tmp93;enum Cyc_Absyn_Size_of _tmp94;enum Cyc_Absyn_Sign
_tmp96;enum Cyc_Absyn_Size_of _tmp97;enum Cyc_Absyn_Sign _tmp99;enum Cyc_Absyn_Size_of
_tmp9A;enum Cyc_Absyn_Sign _tmp9C;enum Cyc_Absyn_Size_of _tmp9D;enum Cyc_Absyn_Sign
_tmp9F;enum Cyc_Absyn_Size_of _tmpA0;enum Cyc_Absyn_Sign _tmpA2;enum Cyc_Absyn_Size_of
_tmpA3;enum Cyc_Absyn_Sign _tmpA5;enum Cyc_Absyn_Size_of _tmpA6;int _tmpA9;struct
Cyc_List_List*_tmpAB;struct Cyc_Absyn_AggrInfo _tmpAD;union Cyc_Absyn_AggrInfoU
_tmpAE;struct Cyc_List_List*_tmpAF;enum Cyc_Absyn_AggrKind _tmpB1;struct Cyc_List_List*
_tmpB2;struct _tuple0*_tmpB4;struct Cyc_List_List*_tmpB6;struct _tuple0*_tmpB8;
struct Cyc_List_List*_tmpB9;struct Cyc_Absyn_Exp*_tmpBB;void*_tmpBD;void*_tmpBF;
void*_tmpC0;void*_tmpC2;_LL71: {struct Cyc_Absyn_ArrayType_struct*_tmp62=(struct
Cyc_Absyn_ArrayType_struct*)_tmp61;if(_tmp62->tag != 9)goto _LL73;}_LL72: goto _LL74;
_LL73: {struct Cyc_Absyn_FnType_struct*_tmp63=(struct Cyc_Absyn_FnType_struct*)
_tmp61;if(_tmp63->tag != 10)goto _LL75;}_LL74: goto _LL76;_LL75: {struct Cyc_Absyn_PointerType_struct*
_tmp64=(struct Cyc_Absyn_PointerType_struct*)_tmp61;if(_tmp64->tag != 5)goto _LL77;}
_LL76: return;_LL77: {struct Cyc_Absyn_VoidType_struct*_tmp65=(struct Cyc_Absyn_VoidType_struct*)
_tmp61;if(_tmp65->tag != 0)goto _LL79;}_LL78:{const char*_tmp3D2;Cyc_Absyndump_dump(((
_tmp3D2="void",_tag_dyneither(_tmp3D2,sizeof(char),5))));}return;_LL79: {struct
Cyc_Absyn_VarType_struct*_tmp66=(struct Cyc_Absyn_VarType_struct*)_tmp61;if(
_tmp66->tag != 2)goto _LL7B;else{_tmp67=_tmp66->f1;}}_LL7A: Cyc_Absyndump_dump_str(
_tmp67->name);return;_LL7B: {struct Cyc_Absyn_Evar_struct*_tmp68=(struct Cyc_Absyn_Evar_struct*)
_tmp61;if(_tmp68->tag != 1)goto _LL7D;else{_tmp69=_tmp68->f1;_tmp6A=_tmp68->f2;if(
_tmp6A != 0)goto _LL7D;_tmp6B=_tmp68->f3;}}_LL7C:{const char*_tmp3D3;Cyc_Absyndump_dump(((
_tmp3D3="%",_tag_dyneither(_tmp3D3,sizeof(char),2))));}if(_tmp69 == 0){const char*
_tmp3D4;Cyc_Absyndump_dump(((_tmp3D4="?",_tag_dyneither(_tmp3D4,sizeof(char),2))));}
else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)_tmp69->v);}{const char*
_tmp3D8;void*_tmp3D7[1];struct Cyc_Int_pa_struct _tmp3D6;Cyc_Absyndump_dump((
struct _dyneither_ptr)((_tmp3D6.tag=1,((_tmp3D6.f1=(unsigned long)_tmp6B,((
_tmp3D7[0]=& _tmp3D6,Cyc_aprintf(((_tmp3D8="(%d)",_tag_dyneither(_tmp3D8,sizeof(
char),5))),_tag_dyneither(_tmp3D7,sizeof(void*),1)))))))));}return;_LL7D: {
struct Cyc_Absyn_Evar_struct*_tmp6C=(struct Cyc_Absyn_Evar_struct*)_tmp61;if(
_tmp6C->tag != 1)goto _LL7F;else{_tmp6D=_tmp6C->f1;_tmp6E=_tmp6C->f2;if(_tmp6E == 0)
goto _LL7F;_tmp6F=*_tmp6E;_tmp70=(void*)_tmp6F.v;_tmp71=_tmp6C->f3;}}_LL7E: Cyc_Absyndump_dumpntyp(
_tmp70);return;_LL7F: {struct Cyc_Absyn_DatatypeType_struct*_tmp72=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp61;if(_tmp72->tag != 3)goto _LL81;else{_tmp73=_tmp72->f1;_tmp74=_tmp73.datatype_info;
_tmp75=_tmp73.targs;}}_LL80:{union Cyc_Absyn_DatatypeInfoU _tmpCE=_tmp74;struct Cyc_Absyn_UnknownDatatypeInfo
_tmpCF;struct _tuple0*_tmpD0;int _tmpD1;struct Cyc_Absyn_Datatypedecl**_tmpD2;
struct Cyc_Absyn_Datatypedecl*_tmpD3;struct Cyc_Absyn_Datatypedecl _tmpD4;struct
_tuple0*_tmpD5;int _tmpD6;_LLC4: if((_tmpCE.UnknownDatatype).tag != 1)goto _LLC6;
_tmpCF=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpCE.UnknownDatatype).val;_tmpD0=
_tmpCF.name;_tmpD1=_tmpCF.is_extensible;_LLC5: _tmpD5=_tmpD0;_tmpD6=_tmpD1;goto
_LLC7;_LLC6: if((_tmpCE.KnownDatatype).tag != 2)goto _LLC3;_tmpD2=(struct Cyc_Absyn_Datatypedecl**)(
_tmpCE.KnownDatatype).val;_tmpD3=*_tmpD2;_tmpD4=*_tmpD3;_tmpD5=_tmpD4.name;
_tmpD6=_tmpD4.is_extensible;_LLC7: if(_tmpD6){const char*_tmp3D9;Cyc_Absyndump_dump(((
_tmp3D9="@extensible ",_tag_dyneither(_tmp3D9,sizeof(char),13))));}{const char*
_tmp3DA;Cyc_Absyndump_dump(((_tmp3DA="datatype ",_tag_dyneither(_tmp3DA,sizeof(
char),10))));}Cyc_Absyndump_dumpqvar(_tmpD5);Cyc_Absyndump_dumptps(_tmp75);goto
_LLC3;_LLC3:;}goto _LL70;_LL81: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp76=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp61;if(_tmp76->tag != 4)goto _LL83;
else{_tmp77=_tmp76->f1;_tmp78=_tmp77.field_info;_tmp79=_tmp77.targs;}}_LL82:{
union Cyc_Absyn_DatatypeFieldInfoU _tmpD9=_tmp78;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpDA;struct _tuple0*_tmpDB;struct _tuple0*_tmpDC;int _tmpDD;struct _tuple1 _tmpDE;
struct Cyc_Absyn_Datatypedecl*_tmpDF;struct Cyc_Absyn_Datatypedecl _tmpE0;struct
_tuple0*_tmpE1;int _tmpE2;struct Cyc_Absyn_Datatypefield*_tmpE3;struct Cyc_Absyn_Datatypefield
_tmpE4;struct _tuple0*_tmpE5;_LLC9: if((_tmpD9.UnknownDatatypefield).tag != 1)goto
_LLCB;_tmpDA=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpD9.UnknownDatatypefield).val;
_tmpDB=_tmpDA.datatype_name;_tmpDC=_tmpDA.field_name;_tmpDD=_tmpDA.is_extensible;
_LLCA: _tmpE1=_tmpDB;_tmpE2=_tmpDD;_tmpE5=_tmpDC;goto _LLCC;_LLCB: if((_tmpD9.KnownDatatypefield).tag
!= 2)goto _LLC8;_tmpDE=(struct _tuple1)(_tmpD9.KnownDatatypefield).val;_tmpDF=
_tmpDE.f1;_tmpE0=*_tmpDF;_tmpE1=_tmpE0.name;_tmpE2=_tmpE0.is_extensible;_tmpE3=
_tmpDE.f2;_tmpE4=*_tmpE3;_tmpE5=_tmpE4.name;_LLCC: if(_tmpE2){const char*_tmp3DB;
Cyc_Absyndump_dump(((_tmp3DB="@extensible ",_tag_dyneither(_tmp3DB,sizeof(char),
13))));}{const char*_tmp3DC;Cyc_Absyndump_dump(((_tmp3DC="datatype ",
_tag_dyneither(_tmp3DC,sizeof(char),10))));}Cyc_Absyndump_dumpqvar(_tmpE1);{
const char*_tmp3DD;Cyc_Absyndump_dump(((_tmp3DD=".",_tag_dyneither(_tmp3DD,
sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmpE5);Cyc_Absyndump_dumptps(_tmp79);
goto _LLC8;_LLC8:;}goto _LL70;_LL83: {struct Cyc_Absyn_IntType_struct*_tmp7A=(
struct Cyc_Absyn_IntType_struct*)_tmp61;if(_tmp7A->tag != 6)goto _LL85;else{_tmp7B=
_tmp7A->f1;if(_tmp7B != Cyc_Absyn_None)goto _LL85;_tmp7C=_tmp7A->f2;if(_tmp7C != 
Cyc_Absyn_Int_sz)goto _LL85;}}_LL84: goto _LL86;_LL85: {struct Cyc_Absyn_IntType_struct*
_tmp7D=(struct Cyc_Absyn_IntType_struct*)_tmp61;if(_tmp7D->tag != 6)goto _LL87;
else{_tmp7E=_tmp7D->f1;if(_tmp7E != Cyc_Absyn_Signed)goto _LL87;_tmp7F=_tmp7D->f2;
if(_tmp7F != Cyc_Absyn_Int_sz)goto _LL87;}}_LL86:{const char*_tmp3DE;Cyc_Absyndump_dump(((
_tmp3DE="int",_tag_dyneither(_tmp3DE,sizeof(char),4))));}return;_LL87: {struct
Cyc_Absyn_IntType_struct*_tmp80=(struct Cyc_Absyn_IntType_struct*)_tmp61;if(
_tmp80->tag != 6)goto _LL89;else{_tmp81=_tmp80->f1;if(_tmp81 != Cyc_Absyn_None)goto
_LL89;_tmp82=_tmp80->f2;if(_tmp82 != Cyc_Absyn_Long_sz)goto _LL89;}}_LL88: goto
_LL8A;_LL89: {struct Cyc_Absyn_IntType_struct*_tmp83=(struct Cyc_Absyn_IntType_struct*)
_tmp61;if(_tmp83->tag != 6)goto _LL8B;else{_tmp84=_tmp83->f1;if(_tmp84 != Cyc_Absyn_Signed)
goto _LL8B;_tmp85=_tmp83->f2;if(_tmp85 != Cyc_Absyn_Long_sz)goto _LL8B;}}_LL8A:{
const char*_tmp3DF;Cyc_Absyndump_dump(((_tmp3DF="long",_tag_dyneither(_tmp3DF,
sizeof(char),5))));}return;_LL8B: {struct Cyc_Absyn_IntType_struct*_tmp86=(struct
Cyc_Absyn_IntType_struct*)_tmp61;if(_tmp86->tag != 6)goto _LL8D;else{_tmp87=_tmp86->f1;
if(_tmp87 != Cyc_Absyn_None)goto _LL8D;_tmp88=_tmp86->f2;if(_tmp88 != Cyc_Absyn_Char_sz)
goto _LL8D;}}_LL8C:{const char*_tmp3E0;Cyc_Absyndump_dump(((_tmp3E0="char",
_tag_dyneither(_tmp3E0,sizeof(char),5))));}return;_LL8D: {struct Cyc_Absyn_IntType_struct*
_tmp89=(struct Cyc_Absyn_IntType_struct*)_tmp61;if(_tmp89->tag != 6)goto _LL8F;
else{_tmp8A=_tmp89->f1;if(_tmp8A != Cyc_Absyn_Signed)goto _LL8F;_tmp8B=_tmp89->f2;
if(_tmp8B != Cyc_Absyn_Char_sz)goto _LL8F;}}_LL8E:{const char*_tmp3E1;Cyc_Absyndump_dump(((
_tmp3E1="signed char",_tag_dyneither(_tmp3E1,sizeof(char),12))));}return;_LL8F: {
struct Cyc_Absyn_IntType_struct*_tmp8C=(struct Cyc_Absyn_IntType_struct*)_tmp61;
if(_tmp8C->tag != 6)goto _LL91;else{_tmp8D=_tmp8C->f1;if(_tmp8D != Cyc_Absyn_Unsigned)
goto _LL91;_tmp8E=_tmp8C->f2;if(_tmp8E != Cyc_Absyn_Char_sz)goto _LL91;}}_LL90:{
const char*_tmp3E2;Cyc_Absyndump_dump(((_tmp3E2="unsigned char",_tag_dyneither(
_tmp3E2,sizeof(char),14))));}return;_LL91: {struct Cyc_Absyn_IntType_struct*
_tmp8F=(struct Cyc_Absyn_IntType_struct*)_tmp61;if(_tmp8F->tag != 6)goto _LL93;
else{_tmp90=_tmp8F->f1;if(_tmp90 != Cyc_Absyn_None)goto _LL93;_tmp91=_tmp8F->f2;
if(_tmp91 != Cyc_Absyn_Short_sz)goto _LL93;}}_LL92: goto _LL94;_LL93: {struct Cyc_Absyn_IntType_struct*
_tmp92=(struct Cyc_Absyn_IntType_struct*)_tmp61;if(_tmp92->tag != 6)goto _LL95;
else{_tmp93=_tmp92->f1;if(_tmp93 != Cyc_Absyn_Signed)goto _LL95;_tmp94=_tmp92->f2;
if(_tmp94 != Cyc_Absyn_Short_sz)goto _LL95;}}_LL94:{const char*_tmp3E3;Cyc_Absyndump_dump(((
_tmp3E3="short",_tag_dyneither(_tmp3E3,sizeof(char),6))));}return;_LL95: {struct
Cyc_Absyn_IntType_struct*_tmp95=(struct Cyc_Absyn_IntType_struct*)_tmp61;if(
_tmp95->tag != 6)goto _LL97;else{_tmp96=_tmp95->f1;if(_tmp96 != Cyc_Absyn_Unsigned)
goto _LL97;_tmp97=_tmp95->f2;if(_tmp97 != Cyc_Absyn_Short_sz)goto _LL97;}}_LL96:{
const char*_tmp3E4;Cyc_Absyndump_dump(((_tmp3E4="unsigned short",_tag_dyneither(
_tmp3E4,sizeof(char),15))));}return;_LL97: {struct Cyc_Absyn_IntType_struct*
_tmp98=(struct Cyc_Absyn_IntType_struct*)_tmp61;if(_tmp98->tag != 6)goto _LL99;
else{_tmp99=_tmp98->f1;if(_tmp99 != Cyc_Absyn_Unsigned)goto _LL99;_tmp9A=_tmp98->f2;
if(_tmp9A != Cyc_Absyn_Int_sz)goto _LL99;}}_LL98:{const char*_tmp3E5;Cyc_Absyndump_dump(((
_tmp3E5="unsigned int",_tag_dyneither(_tmp3E5,sizeof(char),13))));}return;_LL99: {
struct Cyc_Absyn_IntType_struct*_tmp9B=(struct Cyc_Absyn_IntType_struct*)_tmp61;
if(_tmp9B->tag != 6)goto _LL9B;else{_tmp9C=_tmp9B->f1;if(_tmp9C != Cyc_Absyn_Unsigned)
goto _LL9B;_tmp9D=_tmp9B->f2;if(_tmp9D != Cyc_Absyn_Long_sz)goto _LL9B;}}_LL9A:{
const char*_tmp3E6;Cyc_Absyndump_dump(((_tmp3E6="unsigned long",_tag_dyneither(
_tmp3E6,sizeof(char),14))));}return;_LL9B: {struct Cyc_Absyn_IntType_struct*
_tmp9E=(struct Cyc_Absyn_IntType_struct*)_tmp61;if(_tmp9E->tag != 6)goto _LL9D;
else{_tmp9F=_tmp9E->f1;if(_tmp9F != Cyc_Absyn_None)goto _LL9D;_tmpA0=_tmp9E->f2;
if(_tmpA0 != Cyc_Absyn_LongLong_sz)goto _LL9D;}}_LL9C: goto _LL9E;_LL9D: {struct Cyc_Absyn_IntType_struct*
_tmpA1=(struct Cyc_Absyn_IntType_struct*)_tmp61;if(_tmpA1->tag != 6)goto _LL9F;
else{_tmpA2=_tmpA1->f1;if(_tmpA2 != Cyc_Absyn_Signed)goto _LL9F;_tmpA3=_tmpA1->f2;
if(_tmpA3 != Cyc_Absyn_LongLong_sz)goto _LL9F;}}_LL9E: switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LLCD:{const char*_tmp3E7;Cyc_Absyndump_dump(((_tmp3E7="long long",
_tag_dyneither(_tmp3E7,sizeof(char),10))));}return;case Cyc_Cyclone_Vc_c: _LLCE:{
const char*_tmp3E8;Cyc_Absyndump_dump(((_tmp3E8="__int64",_tag_dyneither(_tmp3E8,
sizeof(char),8))));}return;}_LL9F: {struct Cyc_Absyn_IntType_struct*_tmpA4=(
struct Cyc_Absyn_IntType_struct*)_tmp61;if(_tmpA4->tag != 6)goto _LLA1;else{_tmpA5=
_tmpA4->f1;if(_tmpA5 != Cyc_Absyn_Unsigned)goto _LLA1;_tmpA6=_tmpA4->f2;if(_tmpA6
!= Cyc_Absyn_LongLong_sz)goto _LLA1;}}_LLA0: switch(Cyc_Cyclone_c_compiler){case
Cyc_Cyclone_Vc_c: _LLD0:{const char*_tmp3E9;Cyc_Absyndump_dump(((_tmp3E9="unsigned __int64",
_tag_dyneither(_tmp3E9,sizeof(char),17))));}return;case Cyc_Cyclone_Gcc_c: _LLD1:{
const char*_tmp3EA;Cyc_Absyndump_dump(((_tmp3EA="unsigned long long",
_tag_dyneither(_tmp3EA,sizeof(char),19))));}return;}_LLA1: {struct Cyc_Absyn_FloatType_struct*
_tmpA7=(struct Cyc_Absyn_FloatType_struct*)_tmp61;if(_tmpA7->tag != 7)goto _LLA3;}
_LLA2:{const char*_tmp3EB;Cyc_Absyndump_dump(((_tmp3EB="float",_tag_dyneither(
_tmp3EB,sizeof(char),6))));}return;_LLA3: {struct Cyc_Absyn_DoubleType_struct*
_tmpA8=(struct Cyc_Absyn_DoubleType_struct*)_tmp61;if(_tmpA8->tag != 8)goto _LLA5;
else{_tmpA9=_tmpA8->f1;}}_LLA4: if(_tmpA9){const char*_tmp3EC;Cyc_Absyndump_dump(((
_tmp3EC="long double",_tag_dyneither(_tmp3EC,sizeof(char),12))));}else{const char*
_tmp3ED;Cyc_Absyndump_dump(((_tmp3ED="double",_tag_dyneither(_tmp3ED,sizeof(char),
7))));}return;_LLA5: {struct Cyc_Absyn_TupleType_struct*_tmpAA=(struct Cyc_Absyn_TupleType_struct*)
_tmp61;if(_tmpAA->tag != 11)goto _LLA7;else{_tmpAB=_tmpAA->f1;}}_LLA6: Cyc_Absyndump_dump_char((
int)'$');Cyc_Absyndump_dumpargs(_tmpAB);return;_LLA7: {struct Cyc_Absyn_AggrType_struct*
_tmpAC=(struct Cyc_Absyn_AggrType_struct*)_tmp61;if(_tmpAC->tag != 12)goto _LLA9;
else{_tmpAD=_tmpAC->f1;_tmpAE=_tmpAD.aggr_info;_tmpAF=_tmpAD.targs;}}_LLA8: {
enum Cyc_Absyn_AggrKind _tmpFA;struct _tuple0*_tmpFB;struct _tuple9 _tmpF9=Cyc_Absyn_aggr_kinded_name(
_tmpAE);_tmpFA=_tmpF9.f1;_tmpFB=_tmpF9.f2;Cyc_Absyndump_dumpaggr_kind(_tmpFA);
Cyc_Absyndump_dumpqvar(_tmpFB);Cyc_Absyndump_dumptps(_tmpAF);return;}_LLA9: {
struct Cyc_Absyn_AnonAggrType_struct*_tmpB0=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp61;if(_tmpB0->tag != 13)goto _LLAB;else{_tmpB1=_tmpB0->f1;_tmpB2=_tmpB0->f2;}}
_LLAA: Cyc_Absyndump_dumpaggr_kind(_tmpB1);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(
_tmpB2);Cyc_Absyndump_dump_char((int)'}');return;_LLAB: {struct Cyc_Absyn_EnumType_struct*
_tmpB3=(struct Cyc_Absyn_EnumType_struct*)_tmp61;if(_tmpB3->tag != 14)goto _LLAD;
else{_tmpB4=_tmpB3->f1;}}_LLAC:{const char*_tmp3EE;Cyc_Absyndump_dump(((_tmp3EE="enum ",
_tag_dyneither(_tmp3EE,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(_tmpB4);
return;_LLAD: {struct Cyc_Absyn_AnonEnumType_struct*_tmpB5=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp61;if(_tmpB5->tag != 15)goto _LLAF;else{_tmpB6=_tmpB5->f1;}}_LLAE:{const char*
_tmp3EF;Cyc_Absyndump_dump(((_tmp3EF="enum {",_tag_dyneither(_tmp3EF,sizeof(char),
7))));}Cyc_Absyndump_dumpenumfields(_tmpB6);{const char*_tmp3F0;Cyc_Absyndump_dump(((
_tmp3F0="}",_tag_dyneither(_tmp3F0,sizeof(char),2))));}return;_LLAF: {struct Cyc_Absyn_TypedefType_struct*
_tmpB7=(struct Cyc_Absyn_TypedefType_struct*)_tmp61;if(_tmpB7->tag != 18)goto _LLB1;
else{_tmpB8=_tmpB7->f1;_tmpB9=_tmpB7->f2;}}_LLB0:(Cyc_Absyndump_dumpqvar(_tmpB8),
Cyc_Absyndump_dumptps(_tmpB9));return;_LLB1: {struct Cyc_Absyn_ValueofType_struct*
_tmpBA=(struct Cyc_Absyn_ValueofType_struct*)_tmp61;if(_tmpBA->tag != 19)goto _LLB3;
else{_tmpBB=_tmpBA->f1;}}_LLB2:{const char*_tmp3F1;Cyc_Absyndump_dump(((_tmp3F1="valueof_t(",
_tag_dyneither(_tmp3F1,sizeof(char),11))));}Cyc_Absyndump_dumpexp(_tmpBB);{const
char*_tmp3F2;Cyc_Absyndump_dump(((_tmp3F2=")",_tag_dyneither(_tmp3F2,sizeof(char),
2))));}return;_LLB3: {struct Cyc_Absyn_RgnHandleType_struct*_tmpBC=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp61;if(_tmpBC->tag != 16)goto _LLB5;else{_tmpBD=(void*)_tmpBC->f1;}}_LLB4:{
const char*_tmp3F3;Cyc_Absyndump_dump(((_tmp3F3="region_t<",_tag_dyneither(
_tmp3F3,sizeof(char),10))));}Cyc_Absyndump_dumprgn(_tmpBD);{const char*_tmp3F4;
Cyc_Absyndump_dump(((_tmp3F4=">",_tag_dyneither(_tmp3F4,sizeof(char),2))));}
return;_LLB5: {struct Cyc_Absyn_DynRgnType_struct*_tmpBE=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp61;if(_tmpBE->tag != 17)goto _LLB7;else{_tmpBF=(void*)_tmpBE->f1;_tmpC0=(void*)
_tmpBE->f2;}}_LLB6:{const char*_tmp3F5;Cyc_Absyndump_dump(((_tmp3F5="dynregion_t<",
_tag_dyneither(_tmp3F5,sizeof(char),13))));}Cyc_Absyndump_dumprgn(_tmpBF);{const
char*_tmp3F6;Cyc_Absyndump_dump(((_tmp3F6=",",_tag_dyneither(_tmp3F6,sizeof(char),
2))));}Cyc_Absyndump_dumprgn(_tmpC0);{const char*_tmp3F7;Cyc_Absyndump_dump(((
_tmp3F7=">",_tag_dyneither(_tmp3F7,sizeof(char),2))));}return;_LLB7: {struct Cyc_Absyn_TagType_struct*
_tmpC1=(struct Cyc_Absyn_TagType_struct*)_tmp61;if(_tmpC1->tag != 20)goto _LLB9;
else{_tmpC2=(void*)_tmpC1->f1;}}_LLB8:{const char*_tmp3F8;Cyc_Absyndump_dump(((
_tmp3F8="tag_t<",_tag_dyneither(_tmp3F8,sizeof(char),7))));}Cyc_Absyndump_dumpntyp(
_tmpC2);{const char*_tmp3F9;Cyc_Absyndump_dump(((_tmp3F9=">",_tag_dyneither(
_tmp3F9,sizeof(char),2))));}return;_LLB9: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpC3=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp61;if(_tmpC3->tag != 22)goto _LLBB;}
_LLBA:{const char*_tmp3FA;Cyc_Absyndump_dump(((_tmp3FA="`U",_tag_dyneither(
_tmp3FA,sizeof(char),3))));}goto _LL70;_LLBB: {struct Cyc_Absyn_HeapRgn_struct*
_tmpC4=(struct Cyc_Absyn_HeapRgn_struct*)_tmp61;if(_tmpC4->tag != 21)goto _LLBD;}
_LLBC: goto _LLBE;_LLBD: {struct Cyc_Absyn_AccessEff_struct*_tmpC5=(struct Cyc_Absyn_AccessEff_struct*)
_tmp61;if(_tmpC5->tag != 23)goto _LLBF;}_LLBE: goto _LLC0;_LLBF: {struct Cyc_Absyn_RgnsEff_struct*
_tmpC6=(struct Cyc_Absyn_RgnsEff_struct*)_tmp61;if(_tmpC6->tag != 25)goto _LLC1;}
_LLC0: goto _LLC2;_LLC1: {struct Cyc_Absyn_JoinEff_struct*_tmpC7=(struct Cyc_Absyn_JoinEff_struct*)
_tmp61;if(_tmpC7->tag != 24)goto _LL70;}_LLC2: return;_LL70:;}void Cyc_Absyndump_dumpvaropt(
struct Cyc_Core_Opt*vo);void Cyc_Absyndump_dumpvaropt(struct Cyc_Core_Opt*vo){if(vo
!= 0)Cyc_Absyndump_dump_str((struct _dyneither_ptr*)vo->v);}void Cyc_Absyndump_dumpfunarg(
struct _tuple7*t);void Cyc_Absyndump_dumpfunarg(struct _tuple7*t){((void(*)(struct
Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Core_Opt*),struct Cyc_Core_Opt*))Cyc_Absyndump_dumptqtd)((*
t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}struct _tuple15{void*f1;void*f2;
};void Cyc_Absyndump_dump_rgncmp(struct _tuple15*cmp);void Cyc_Absyndump_dump_rgncmp(
struct _tuple15*cmp){struct _tuple15 _tmp10A;void*_tmp10B;void*_tmp10C;struct
_tuple15*_tmp109=cmp;_tmp10A=*_tmp109;_tmp10B=_tmp10A.f1;_tmp10C=_tmp10A.f2;Cyc_Absyndump_dumpeff(
_tmp10B);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn(_tmp10C);}void
Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po);void Cyc_Absyndump_dump_rgnpo(
struct Cyc_List_List*rgn_po){const char*_tmp3FB;((void(*)(void(*f)(struct _tuple15*),
struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,
rgn_po,((_tmp3FB=",",_tag_dyneither(_tmp3FB,sizeof(char),2))));}void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po);void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){Cyc_Absyndump_dump_char((
int)'(');for(0;args != 0;args=args->tl){Cyc_Absyndump_dumpfunarg((struct _tuple7*)
args->hd);if((args->tl != 0  || c_varargs) || cyc_varargs != 0)Cyc_Absyndump_dump_char((
int)',');}if(c_varargs){const char*_tmp3FC;Cyc_Absyndump_dump(((_tmp3FC="...",
_tag_dyneither(_tmp3FC,sizeof(char),4))));}else{if(cyc_varargs != 0){struct
_tuple7*_tmp3FD;struct _tuple7*_tmp10F=(_tmp3FD=_cycalloc(sizeof(*_tmp3FD)),((
_tmp3FD->f1=cyc_varargs->name,((_tmp3FD->f2=cyc_varargs->tq,((_tmp3FD->f3=
cyc_varargs->type,_tmp3FD)))))));{const char*_tmp3FE;Cyc_Absyndump_dump(((_tmp3FE="...",
_tag_dyneither(_tmp3FE,sizeof(char),4))));}if(cyc_varargs->inject){const char*
_tmp3FF;Cyc_Absyndump_dump(((_tmp3FF=" inject ",_tag_dyneither(_tmp3FF,sizeof(
char),9))));}Cyc_Absyndump_dumpfunarg(_tmp10F);}}if(effopt != 0){Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpeff((void*)effopt->v);}if(rgn_po != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dump_rgnpo(rgn_po);}Cyc_Absyndump_dump_char((int)')');}
void Cyc_Absyndump_dumptyp(void*t);void Cyc_Absyndump_dumptyp(void*t){((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(
0),t,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpdesignator(
void*d);void Cyc_Absyndump_dumpdesignator(void*d){void*_tmp113=d;struct Cyc_Absyn_Exp*
_tmp115;struct _dyneither_ptr*_tmp117;_LLD4: {struct Cyc_Absyn_ArrayElement_struct*
_tmp114=(struct Cyc_Absyn_ArrayElement_struct*)_tmp113;if(_tmp114->tag != 0)goto
_LLD6;else{_tmp115=_tmp114->f1;}}_LLD5:{const char*_tmp400;Cyc_Absyndump_dump(((
_tmp400=".[",_tag_dyneither(_tmp400,sizeof(char),3))));}Cyc_Absyndump_dumpexp(
_tmp115);Cyc_Absyndump_dump_char((int)']');goto _LLD3;_LLD6: {struct Cyc_Absyn_FieldName_struct*
_tmp116=(struct Cyc_Absyn_FieldName_struct*)_tmp113;if(_tmp116->tag != 1)goto _LLD3;
else{_tmp117=_tmp116->f1;}}_LLD7: Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp117);goto _LLD3;_LLD3:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};void Cyc_Absyndump_dumpde(struct _tuple16*de);void Cyc_Absyndump_dumpde(struct
_tuple16*de){{const char*_tmp403;const char*_tmp402;const char*_tmp401;((void(*)(
void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,(*
de).f1,((_tmp401="",_tag_dyneither(_tmp401,sizeof(char),1))),((_tmp402="=",
_tag_dyneither(_tmp402,sizeof(char),2))),((_tmp403="=",_tag_dyneither(_tmp403,
sizeof(char),2))));}Cyc_Absyndump_dumpexp((*de).f2);}void Cyc_Absyndump_dumpexps_prec(
int inprec,struct Cyc_List_List*es);void Cyc_Absyndump_dumpexps_prec(int inprec,
struct Cyc_List_List*es){const char*_tmp406;const char*_tmp405;const char*_tmp404;((
void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec,inprec,es,((_tmp404="",_tag_dyneither(_tmp404,sizeof(
char),1))),((_tmp405="",_tag_dyneither(_tmp405,sizeof(char),1))),((_tmp406=",",
_tag_dyneither(_tmp406,sizeof(char),2))));}void Cyc_Absyndump_dumpexp_prec(int
inprec,struct Cyc_Absyn_Exp*e);void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*
e){int myprec=Cyc_Absynpp_exp_prec(e);if(inprec >= myprec){const char*_tmp407;Cyc_Absyndump_dump_nospace(((
_tmp407="(",_tag_dyneither(_tmp407,sizeof(char),2))));}{void*_tmp120=e->r;union
Cyc_Absyn_Cnst _tmp122;struct _tuple3 _tmp123;enum Cyc_Absyn_Sign _tmp124;char
_tmp125;union Cyc_Absyn_Cnst _tmp127;struct _tuple4 _tmp128;enum Cyc_Absyn_Sign
_tmp129;short _tmp12A;union Cyc_Absyn_Cnst _tmp12C;struct _tuple5 _tmp12D;enum Cyc_Absyn_Sign
_tmp12E;int _tmp12F;union Cyc_Absyn_Cnst _tmp131;struct _tuple5 _tmp132;enum Cyc_Absyn_Sign
_tmp133;int _tmp134;union Cyc_Absyn_Cnst _tmp136;struct _tuple5 _tmp137;enum Cyc_Absyn_Sign
_tmp138;int _tmp139;union Cyc_Absyn_Cnst _tmp13B;struct _tuple6 _tmp13C;enum Cyc_Absyn_Sign
_tmp13D;long long _tmp13E;union Cyc_Absyn_Cnst _tmp140;struct _dyneither_ptr _tmp141;
union Cyc_Absyn_Cnst _tmp143;int _tmp144;union Cyc_Absyn_Cnst _tmp146;struct
_dyneither_ptr _tmp147;struct _tuple0*_tmp149;struct _tuple0*_tmp14B;enum Cyc_Absyn_Primop
_tmp14D;struct Cyc_List_List*_tmp14E;struct Cyc_Absyn_Exp*_tmp150;struct Cyc_Core_Opt*
_tmp151;struct Cyc_Absyn_Exp*_tmp152;struct Cyc_Absyn_Exp*_tmp154;enum Cyc_Absyn_Incrementor
_tmp155;struct Cyc_Absyn_Exp*_tmp157;enum Cyc_Absyn_Incrementor _tmp158;struct Cyc_Absyn_Exp*
_tmp15A;enum Cyc_Absyn_Incrementor _tmp15B;struct Cyc_Absyn_Exp*_tmp15D;enum Cyc_Absyn_Incrementor
_tmp15E;struct Cyc_Absyn_Exp*_tmp160;struct Cyc_Absyn_Exp*_tmp161;struct Cyc_Absyn_Exp*
_tmp162;struct Cyc_Absyn_Exp*_tmp164;struct Cyc_Absyn_Exp*_tmp165;struct Cyc_Absyn_Exp*
_tmp167;struct Cyc_Absyn_Exp*_tmp168;struct Cyc_Absyn_Exp*_tmp16A;struct Cyc_Absyn_Exp*
_tmp16B;struct Cyc_Absyn_Exp*_tmp16D;struct Cyc_List_List*_tmp16E;struct Cyc_Absyn_Exp*
_tmp170;struct Cyc_List_List*_tmp171;struct Cyc_Absyn_Exp*_tmp173;struct Cyc_Absyn_Exp*
_tmp175;struct Cyc_Absyn_Exp*_tmp177;void*_tmp179;struct Cyc_Absyn_Exp*_tmp17A;
struct Cyc_Absyn_Exp*_tmp17C;struct Cyc_Absyn_Exp*_tmp17E;struct Cyc_Absyn_Exp*
_tmp17F;void*_tmp181;struct Cyc_Absyn_Exp*_tmp183;void*_tmp185;struct Cyc_Absyn_Exp*
_tmp187;struct _dyneither_ptr*_tmp188;void*_tmp18A;void*_tmp18B;struct
_dyneither_ptr*_tmp18D;void*_tmp18F;void*_tmp190;unsigned int _tmp192;struct Cyc_Absyn_Exp*
_tmp194;struct Cyc_Absyn_Exp*_tmp196;struct _dyneither_ptr*_tmp197;struct Cyc_Absyn_Exp*
_tmp199;struct _dyneither_ptr*_tmp19A;struct Cyc_Absyn_Exp*_tmp19C;struct Cyc_Absyn_Exp*
_tmp19D;struct Cyc_List_List*_tmp19F;struct _tuple7*_tmp1A1;struct Cyc_List_List*
_tmp1A2;struct Cyc_List_List*_tmp1A4;struct Cyc_Absyn_Vardecl*_tmp1A6;struct Cyc_Absyn_Exp*
_tmp1A7;struct Cyc_Absyn_Exp*_tmp1A8;struct _tuple0*_tmp1AA;struct Cyc_List_List*
_tmp1AB;struct Cyc_List_List*_tmp1AC;struct Cyc_List_List*_tmp1AE;struct Cyc_List_List*
_tmp1B0;struct Cyc_Absyn_Datatypefield*_tmp1B1;struct _tuple0*_tmp1B3;struct
_tuple0*_tmp1B5;struct Cyc_Absyn_MallocInfo _tmp1B7;int _tmp1B8;struct Cyc_Absyn_Exp*
_tmp1B9;void**_tmp1BA;struct Cyc_Absyn_Exp*_tmp1BB;struct Cyc_Absyn_Exp*_tmp1BD;
struct Cyc_Absyn_Exp*_tmp1BE;struct Cyc_Core_Opt*_tmp1C0;struct Cyc_List_List*
_tmp1C1;struct Cyc_Absyn_Stmt*_tmp1C3;_LLD9: {struct Cyc_Absyn_Const_e_struct*
_tmp121=(struct Cyc_Absyn_Const_e_struct*)_tmp120;if(_tmp121->tag != 0)goto _LLDB;
else{_tmp122=_tmp121->f1;if((_tmp122.Char_c).tag != 2)goto _LLDB;_tmp123=(struct
_tuple3)(_tmp122.Char_c).val;_tmp124=_tmp123.f1;_tmp125=_tmp123.f2;}}_LLDA: Cyc_Absyndump_dump_char((
int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp125));Cyc_Absyndump_dump_char((
int)'\'');goto _LLD8;_LLDB: {struct Cyc_Absyn_Const_e_struct*_tmp126=(struct Cyc_Absyn_Const_e_struct*)
_tmp120;if(_tmp126->tag != 0)goto _LLDD;else{_tmp127=_tmp126->f1;if((_tmp127.Short_c).tag
!= 3)goto _LLDD;_tmp128=(struct _tuple4)(_tmp127.Short_c).val;_tmp129=_tmp128.f1;
_tmp12A=_tmp128.f2;}}_LLDC:{const char*_tmp40B;void*_tmp40A[1];struct Cyc_Int_pa_struct
_tmp409;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp409.tag=1,((_tmp409.f1=(
unsigned long)((int)_tmp12A),((_tmp40A[0]=& _tmp409,Cyc_aprintf(((_tmp40B="%d",
_tag_dyneither(_tmp40B,sizeof(char),3))),_tag_dyneither(_tmp40A,sizeof(void*),1)))))))));}
goto _LLD8;_LLDD: {struct Cyc_Absyn_Const_e_struct*_tmp12B=(struct Cyc_Absyn_Const_e_struct*)
_tmp120;if(_tmp12B->tag != 0)goto _LLDF;else{_tmp12C=_tmp12B->f1;if((_tmp12C.Int_c).tag
!= 4)goto _LLDF;_tmp12D=(struct _tuple5)(_tmp12C.Int_c).val;_tmp12E=_tmp12D.f1;if(
_tmp12E != Cyc_Absyn_None)goto _LLDF;_tmp12F=_tmp12D.f2;}}_LLDE: _tmp134=_tmp12F;
goto _LLE0;_LLDF: {struct Cyc_Absyn_Const_e_struct*_tmp130=(struct Cyc_Absyn_Const_e_struct*)
_tmp120;if(_tmp130->tag != 0)goto _LLE1;else{_tmp131=_tmp130->f1;if((_tmp131.Int_c).tag
!= 4)goto _LLE1;_tmp132=(struct _tuple5)(_tmp131.Int_c).val;_tmp133=_tmp132.f1;if(
_tmp133 != Cyc_Absyn_Signed)goto _LLE1;_tmp134=_tmp132.f2;}}_LLE0:{const char*
_tmp40F;void*_tmp40E[1];struct Cyc_Int_pa_struct _tmp40D;Cyc_Absyndump_dump((
struct _dyneither_ptr)((_tmp40D.tag=1,((_tmp40D.f1=(unsigned long)_tmp134,((
_tmp40E[0]=& _tmp40D,Cyc_aprintf(((_tmp40F="%d",_tag_dyneither(_tmp40F,sizeof(
char),3))),_tag_dyneither(_tmp40E,sizeof(void*),1)))))))));}goto _LLD8;_LLE1: {
struct Cyc_Absyn_Const_e_struct*_tmp135=(struct Cyc_Absyn_Const_e_struct*)_tmp120;
if(_tmp135->tag != 0)goto _LLE3;else{_tmp136=_tmp135->f1;if((_tmp136.Int_c).tag != 
4)goto _LLE3;_tmp137=(struct _tuple5)(_tmp136.Int_c).val;_tmp138=_tmp137.f1;if(
_tmp138 != Cyc_Absyn_Unsigned)goto _LLE3;_tmp139=_tmp137.f2;}}_LLE2:{const char*
_tmp413;void*_tmp412[1];struct Cyc_Int_pa_struct _tmp411;Cyc_Absyndump_dump((
struct _dyneither_ptr)((_tmp411.tag=1,((_tmp411.f1=(unsigned int)_tmp139,((
_tmp412[0]=& _tmp411,Cyc_aprintf(((_tmp413="%u",_tag_dyneither(_tmp413,sizeof(
char),3))),_tag_dyneither(_tmp412,sizeof(void*),1)))))))));}goto _LLD8;_LLE3: {
struct Cyc_Absyn_Const_e_struct*_tmp13A=(struct Cyc_Absyn_Const_e_struct*)_tmp120;
if(_tmp13A->tag != 0)goto _LLE5;else{_tmp13B=_tmp13A->f1;if((_tmp13B.LongLong_c).tag
!= 5)goto _LLE5;_tmp13C=(struct _tuple6)(_tmp13B.LongLong_c).val;_tmp13D=_tmp13C.f1;
_tmp13E=_tmp13C.f2;}}_LLE4:{const char*_tmp414;Cyc_Absyndump_dump(((_tmp414="<<FIX LONG LONG CONSTANT>>",
_tag_dyneither(_tmp414,sizeof(char),27))));}goto _LLD8;_LLE5: {struct Cyc_Absyn_Const_e_struct*
_tmp13F=(struct Cyc_Absyn_Const_e_struct*)_tmp120;if(_tmp13F->tag != 0)goto _LLE7;
else{_tmp140=_tmp13F->f1;if((_tmp140.Float_c).tag != 6)goto _LLE7;_tmp141=(struct
_dyneither_ptr)(_tmp140.Float_c).val;}}_LLE6: Cyc_Absyndump_dump(_tmp141);goto
_LLD8;_LLE7: {struct Cyc_Absyn_Const_e_struct*_tmp142=(struct Cyc_Absyn_Const_e_struct*)
_tmp120;if(_tmp142->tag != 0)goto _LLE9;else{_tmp143=_tmp142->f1;if((_tmp143.Null_c).tag
!= 1)goto _LLE9;_tmp144=(int)(_tmp143.Null_c).val;}}_LLE8:{const char*_tmp415;Cyc_Absyndump_dump(((
_tmp415="NULL",_tag_dyneither(_tmp415,sizeof(char),5))));}goto _LLD8;_LLE9: {
struct Cyc_Absyn_Const_e_struct*_tmp145=(struct Cyc_Absyn_Const_e_struct*)_tmp120;
if(_tmp145->tag != 0)goto _LLEB;else{_tmp146=_tmp145->f1;if((_tmp146.String_c).tag
!= 7)goto _LLEB;_tmp147=(struct _dyneither_ptr)(_tmp146.String_c).val;}}_LLEA: Cyc_Absyndump_dump_char((
int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp147));Cyc_Absyndump_dump_char((
int)'"');goto _LLD8;_LLEB: {struct Cyc_Absyn_UnknownId_e_struct*_tmp148=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp120;if(_tmp148->tag != 2)goto _LLED;else{_tmp149=_tmp148->f1;}}_LLEC: _tmp14B=
_tmp149;goto _LLEE;_LLED: {struct Cyc_Absyn_Var_e_struct*_tmp14A=(struct Cyc_Absyn_Var_e_struct*)
_tmp120;if(_tmp14A->tag != 1)goto _LLEF;else{_tmp14B=_tmp14A->f1;}}_LLEE: Cyc_Absyndump_dumpqvar(
_tmp14B);goto _LLD8;_LLEF: {struct Cyc_Absyn_Primop_e_struct*_tmp14C=(struct Cyc_Absyn_Primop_e_struct*)
_tmp120;if(_tmp14C->tag != 3)goto _LLF1;else{_tmp14D=_tmp14C->f1;_tmp14E=_tmp14C->f2;}}
_LLF0: {struct _dyneither_ptr _tmp1CF=Cyc_Absynpp_prim2str(_tmp14D);switch(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp14E)){case 1: _LL141: if(_tmp14D == (
enum Cyc_Absyn_Primop)Cyc_Absyn_Numelts){{const char*_tmp416;Cyc_Absyndump_dump(((
_tmp416="numelts(",_tag_dyneither(_tmp416,sizeof(char),9))));}Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp14E))->hd);{const
char*_tmp417;Cyc_Absyndump_dump(((_tmp417=")",_tag_dyneither(_tmp417,sizeof(char),
2))));};}else{Cyc_Absyndump_dump(_tmp1CF);Cyc_Absyndump_dumpexp_prec(myprec,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp14E))->hd);}break;
case 2: _LL142: Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp14E))->hd);Cyc_Absyndump_dump(_tmp1CF);Cyc_Absyndump_dump_char((
int)' ');Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp14E->tl))->hd);break;default: _LL143: {struct Cyc_Core_Failure_struct
_tmp41D;const char*_tmp41C;struct Cyc_Core_Failure_struct*_tmp41B;(int)_throw((
void*)((_tmp41B=_cycalloc(sizeof(*_tmp41B)),((_tmp41B[0]=((_tmp41D.tag=Cyc_Core_Failure,((
_tmp41D.f1=((_tmp41C="Absyndump -- Bad number of arguments to primop",
_tag_dyneither(_tmp41C,sizeof(char),47))),_tmp41D)))),_tmp41B)))));}}goto _LLD8;}
_LLF1: {struct Cyc_Absyn_AssignOp_e_struct*_tmp14F=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp120;if(_tmp14F->tag != 4)goto _LLF3;else{_tmp150=_tmp14F->f1;_tmp151=_tmp14F->f2;
_tmp152=_tmp14F->f3;}}_LLF2: Cyc_Absyndump_dumpexp_prec(myprec,_tmp150);if(
_tmp151 != 0)Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)
_tmp151->v));{const char*_tmp41E;Cyc_Absyndump_dump_nospace(((_tmp41E="=",
_tag_dyneither(_tmp41E,sizeof(char),2))));}Cyc_Absyndump_dumpexp_prec(myprec,
_tmp152);goto _LLD8;_LLF3: {struct Cyc_Absyn_Increment_e_struct*_tmp153=(struct Cyc_Absyn_Increment_e_struct*)
_tmp120;if(_tmp153->tag != 5)goto _LLF5;else{_tmp154=_tmp153->f1;_tmp155=_tmp153->f2;
if(_tmp155 != Cyc_Absyn_PreInc)goto _LLF5;}}_LLF4:{const char*_tmp41F;Cyc_Absyndump_dump(((
_tmp41F="++",_tag_dyneither(_tmp41F,sizeof(char),3))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp154);goto _LLD8;_LLF5: {struct Cyc_Absyn_Increment_e_struct*_tmp156=(
struct Cyc_Absyn_Increment_e_struct*)_tmp120;if(_tmp156->tag != 5)goto _LLF7;else{
_tmp157=_tmp156->f1;_tmp158=_tmp156->f2;if(_tmp158 != Cyc_Absyn_PreDec)goto _LLF7;}}
_LLF6:{const char*_tmp420;Cyc_Absyndump_dump(((_tmp420="--",_tag_dyneither(
_tmp420,sizeof(char),3))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp157);goto _LLD8;
_LLF7: {struct Cyc_Absyn_Increment_e_struct*_tmp159=(struct Cyc_Absyn_Increment_e_struct*)
_tmp120;if(_tmp159->tag != 5)goto _LLF9;else{_tmp15A=_tmp159->f1;_tmp15B=_tmp159->f2;
if(_tmp15B != Cyc_Absyn_PostInc)goto _LLF9;}}_LLF8: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp15A);{const char*_tmp421;Cyc_Absyndump_dump(((_tmp421="++",
_tag_dyneither(_tmp421,sizeof(char),3))));}goto _LLD8;_LLF9: {struct Cyc_Absyn_Increment_e_struct*
_tmp15C=(struct Cyc_Absyn_Increment_e_struct*)_tmp120;if(_tmp15C->tag != 5)goto
_LLFB;else{_tmp15D=_tmp15C->f1;_tmp15E=_tmp15C->f2;if(_tmp15E != Cyc_Absyn_PostDec)
goto _LLFB;}}_LLFA: Cyc_Absyndump_dumpexp_prec(myprec,_tmp15D);{const char*_tmp422;
Cyc_Absyndump_dump(((_tmp422="--",_tag_dyneither(_tmp422,sizeof(char),3))));}
goto _LLD8;_LLFB: {struct Cyc_Absyn_Conditional_e_struct*_tmp15F=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp120;if(_tmp15F->tag != 6)goto _LLFD;else{_tmp160=_tmp15F->f1;_tmp161=_tmp15F->f2;
_tmp162=_tmp15F->f3;}}_LLFC: Cyc_Absyndump_dumpexp_prec(myprec,_tmp160);Cyc_Absyndump_dump_char((
int)'?');Cyc_Absyndump_dumpexp_prec(0,_tmp161);Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp162);goto _LLD8;_LLFD: {struct Cyc_Absyn_And_e_struct*
_tmp163=(struct Cyc_Absyn_And_e_struct*)_tmp120;if(_tmp163->tag != 7)goto _LLFF;
else{_tmp164=_tmp163->f1;_tmp165=_tmp163->f2;}}_LLFE: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp164);{const char*_tmp423;Cyc_Absyndump_dump(((_tmp423=" && ",
_tag_dyneither(_tmp423,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(myprec,
_tmp165);goto _LLD8;_LLFF: {struct Cyc_Absyn_Or_e_struct*_tmp166=(struct Cyc_Absyn_Or_e_struct*)
_tmp120;if(_tmp166->tag != 8)goto _LL101;else{_tmp167=_tmp166->f1;_tmp168=_tmp166->f2;}}
_LL100: Cyc_Absyndump_dumpexp_prec(myprec,_tmp167);{const char*_tmp424;Cyc_Absyndump_dump(((
_tmp424=" || ",_tag_dyneither(_tmp424,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp168);goto _LLD8;_LL101: {struct Cyc_Absyn_SeqExp_e_struct*_tmp169=(
struct Cyc_Absyn_SeqExp_e_struct*)_tmp120;if(_tmp169->tag != 9)goto _LL103;else{
_tmp16A=_tmp169->f1;_tmp16B=_tmp169->f2;}}_LL102: Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp16A);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp16B);Cyc_Absyndump_dump_char((int)')');goto _LLD8;_LL103: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp16C=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp120;if(_tmp16C->tag != 10)goto
_LL105;else{_tmp16D=_tmp16C->f1;_tmp16E=_tmp16C->f2;}}_LL104: _tmp170=_tmp16D;
_tmp171=_tmp16E;goto _LL106;_LL105: {struct Cyc_Absyn_FnCall_e_struct*_tmp16F=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp120;if(_tmp16F->tag != 11)goto _LL107;else{
_tmp170=_tmp16F->f1;_tmp171=_tmp16F->f2;}}_LL106: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp170);{const char*_tmp425;Cyc_Absyndump_dump_nospace(((_tmp425="(",
_tag_dyneither(_tmp425,sizeof(char),2))));}Cyc_Absyndump_dumpexps_prec(20,
_tmp171);{const char*_tmp426;Cyc_Absyndump_dump_nospace(((_tmp426=")",
_tag_dyneither(_tmp426,sizeof(char),2))));}goto _LLD8;_LL107: {struct Cyc_Absyn_Throw_e_struct*
_tmp172=(struct Cyc_Absyn_Throw_e_struct*)_tmp120;if(_tmp172->tag != 12)goto _LL109;
else{_tmp173=_tmp172->f1;}}_LL108:{const char*_tmp427;Cyc_Absyndump_dump(((
_tmp427="throw",_tag_dyneither(_tmp427,sizeof(char),6))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp173);goto _LLD8;_LL109: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp174=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp120;if(_tmp174->tag != 13)
goto _LL10B;else{_tmp175=_tmp174->f1;}}_LL10A: _tmp177=_tmp175;goto _LL10C;_LL10B: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp176=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp120;if(_tmp176->tag != 14)goto _LL10D;else{_tmp177=_tmp176->f1;}}_LL10C: Cyc_Absyndump_dumpexp_prec(
inprec,_tmp177);goto _LLD8;_LL10D: {struct Cyc_Absyn_Cast_e_struct*_tmp178=(struct
Cyc_Absyn_Cast_e_struct*)_tmp120;if(_tmp178->tag != 15)goto _LL10F;else{_tmp179=(
void*)_tmp178->f1;_tmp17A=_tmp178->f2;}}_LL10E: Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp(_tmp179);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp17A);goto _LLD8;_LL10F: {struct Cyc_Absyn_Address_e_struct*_tmp17B=(
struct Cyc_Absyn_Address_e_struct*)_tmp120;if(_tmp17B->tag != 16)goto _LL111;else{
_tmp17C=_tmp17B->f1;}}_LL110: Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp17C);goto _LLD8;_LL111: {struct Cyc_Absyn_New_e_struct*_tmp17D=(struct
Cyc_Absyn_New_e_struct*)_tmp120;if(_tmp17D->tag != 17)goto _LL113;else{_tmp17E=
_tmp17D->f1;_tmp17F=_tmp17D->f2;}}_LL112:{const char*_tmp428;Cyc_Absyndump_dump(((
_tmp428="new ",_tag_dyneither(_tmp428,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp17F);goto _LLD8;_LL113: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp180=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp120;if(_tmp180->tag != 18)goto _LL115;
else{_tmp181=(void*)_tmp180->f1;}}_LL114:{const char*_tmp429;Cyc_Absyndump_dump(((
_tmp429="sizeof(",_tag_dyneither(_tmp429,sizeof(char),8))));}Cyc_Absyndump_dumptyp(
_tmp181);Cyc_Absyndump_dump_char((int)')');goto _LLD8;_LL115: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp182=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp120;if(_tmp182->tag != 19)goto
_LL117;else{_tmp183=_tmp182->f1;}}_LL116:{const char*_tmp42A;Cyc_Absyndump_dump(((
_tmp42A="sizeof(",_tag_dyneither(_tmp42A,sizeof(char),8))));}Cyc_Absyndump_dumpexp(
_tmp183);Cyc_Absyndump_dump_char((int)')');goto _LLD8;_LL117: {struct Cyc_Absyn_Valueof_e_struct*
_tmp184=(struct Cyc_Absyn_Valueof_e_struct*)_tmp120;if(_tmp184->tag != 39)goto
_LL119;else{_tmp185=(void*)_tmp184->f1;}}_LL118:{const char*_tmp42B;Cyc_Absyndump_dump(((
_tmp42B="valueof(",_tag_dyneither(_tmp42B,sizeof(char),9))));}Cyc_Absyndump_dumptyp(
_tmp185);Cyc_Absyndump_dump_char((int)')');goto _LLD8;_LL119: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp186=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp120;if(_tmp186->tag != 38)goto
_LL11B;else{_tmp187=_tmp186->f1;_tmp188=_tmp186->f2;}}_LL11A:{const char*_tmp42C;
Cyc_Absyndump_dump(((_tmp42C="tagcheck(",_tag_dyneither(_tmp42C,sizeof(char),10))));}
Cyc_Absyndump_dumpexp(_tmp187);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp188);Cyc_Absyndump_dump_char((int)')');goto _LLD8;_LL11B: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp189=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp120;if(_tmp189->tag != 20)goto
_LL11D;else{_tmp18A=(void*)_tmp189->f1;_tmp18B=(void*)_tmp189->f2;{struct Cyc_Absyn_StructField_struct*
_tmp18C=(struct Cyc_Absyn_StructField_struct*)_tmp18B;if(_tmp18C->tag != 0)goto
_LL11D;else{_tmp18D=_tmp18C->f1;}};}}_LL11C:{const char*_tmp42D;Cyc_Absyndump_dump(((
_tmp42D="offsetof(",_tag_dyneither(_tmp42D,sizeof(char),10))));}Cyc_Absyndump_dumptyp(
_tmp18A);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump_nospace(*_tmp18D);
Cyc_Absyndump_dump_char((int)')');goto _LLD8;_LL11D: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp18E=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp120;if(_tmp18E->tag != 20)goto
_LL11F;else{_tmp18F=(void*)_tmp18E->f1;_tmp190=(void*)_tmp18E->f2;{struct Cyc_Absyn_TupleIndex_struct*
_tmp191=(struct Cyc_Absyn_TupleIndex_struct*)_tmp190;if(_tmp191->tag != 1)goto
_LL11F;else{_tmp192=_tmp191->f1;}};}}_LL11E:{const char*_tmp42E;Cyc_Absyndump_dump(((
_tmp42E="offsetof(",_tag_dyneither(_tmp42E,sizeof(char),10))));}Cyc_Absyndump_dumptyp(
_tmp18F);Cyc_Absyndump_dump_char((int)',');{const char*_tmp432;void*_tmp431[1];
struct Cyc_Int_pa_struct _tmp430;Cyc_Absyndump_dump((struct _dyneither_ptr)((
_tmp430.tag=1,((_tmp430.f1=(unsigned long)((int)_tmp192),((_tmp431[0]=& _tmp430,
Cyc_aprintf(((_tmp432="%d",_tag_dyneither(_tmp432,sizeof(char),3))),
_tag_dyneither(_tmp431,sizeof(void*),1)))))))));}Cyc_Absyndump_dump_char((int)')');
goto _LLD8;_LL11F: {struct Cyc_Absyn_Deref_e_struct*_tmp193=(struct Cyc_Absyn_Deref_e_struct*)
_tmp120;if(_tmp193->tag != 21)goto _LL121;else{_tmp194=_tmp193->f1;}}_LL120: Cyc_Absyndump_dump_char((
int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmp194);goto _LLD8;_LL121: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp195=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp120;if(_tmp195->tag != 22)goto
_LL123;else{_tmp196=_tmp195->f1;_tmp197=_tmp195->f2;}}_LL122: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp196);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp197);goto _LLD8;_LL123: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp198=(struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp120;if(_tmp198->tag != 23)goto _LL125;else{
_tmp199=_tmp198->f1;_tmp19A=_tmp198->f2;}}_LL124: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp199);{const char*_tmp433;Cyc_Absyndump_dump_nospace(((_tmp433="->",
_tag_dyneither(_tmp433,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(*_tmp19A);
goto _LLD8;_LL125: {struct Cyc_Absyn_Subscript_e_struct*_tmp19B=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp120;if(_tmp19B->tag != 24)goto _LL127;else{_tmp19C=_tmp19B->f1;_tmp19D=_tmp19B->f2;}}
_LL126: Cyc_Absyndump_dumpexp_prec(myprec,_tmp19C);Cyc_Absyndump_dump_char((int)'[');
Cyc_Absyndump_dumpexp(_tmp19D);Cyc_Absyndump_dump_char((int)']');goto _LLD8;
_LL127: {struct Cyc_Absyn_Tuple_e_struct*_tmp19E=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp120;if(_tmp19E->tag != 25)goto _LL129;else{_tmp19F=_tmp19E->f1;}}_LL128:{const
char*_tmp434;Cyc_Absyndump_dump(((_tmp434="$(",_tag_dyneither(_tmp434,sizeof(
char),3))));}Cyc_Absyndump_dumpexps_prec(20,_tmp19F);Cyc_Absyndump_dump_char((
int)')');goto _LLD8;_LL129: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp1A0=(struct
Cyc_Absyn_CompoundLit_e_struct*)_tmp120;if(_tmp1A0->tag != 26)goto _LL12B;else{
_tmp1A1=_tmp1A0->f1;_tmp1A2=_tmp1A0->f2;}}_LL12A: Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp((*_tmp1A1).f3);Cyc_Absyndump_dump_char((int)')');{const
char*_tmp437;const char*_tmp436;const char*_tmp435;((void(*)(void(*f)(struct
_tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr
end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp1A2,((
_tmp435="{",_tag_dyneither(_tmp435,sizeof(char),2))),((_tmp436="}",
_tag_dyneither(_tmp436,sizeof(char),2))),((_tmp437=",",_tag_dyneither(_tmp437,
sizeof(char),2))));}goto _LLD8;_LL12B: {struct Cyc_Absyn_Array_e_struct*_tmp1A3=(
struct Cyc_Absyn_Array_e_struct*)_tmp120;if(_tmp1A3->tag != 27)goto _LL12D;else{
_tmp1A4=_tmp1A3->f1;}}_LL12C:{const char*_tmp43A;const char*_tmp439;const char*
_tmp438;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp1A4,((_tmp438="{",_tag_dyneither(_tmp438,sizeof(char),2))),((
_tmp439="}",_tag_dyneither(_tmp439,sizeof(char),2))),((_tmp43A=",",
_tag_dyneither(_tmp43A,sizeof(char),2))));}goto _LLD8;_LL12D: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp1A5=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp120;if(_tmp1A5->tag != 28)
goto _LL12F;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A5->f2;_tmp1A8=_tmp1A5->f3;}}
_LL12E:{const char*_tmp43B;Cyc_Absyndump_dump(((_tmp43B="new {for",_tag_dyneither(
_tmp43B,sizeof(char),9))));}Cyc_Absyndump_dump_str((*_tmp1A6->name).f2);Cyc_Absyndump_dump_char((
int)'<');Cyc_Absyndump_dumpexp(_tmp1A7);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(
_tmp1A8);Cyc_Absyndump_dump_char((int)'}');goto _LLD8;_LL12F: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp1A9=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp120;if(_tmp1A9->tag != 29)goto
_LL131;else{_tmp1AA=_tmp1A9->f1;_tmp1AB=_tmp1A9->f2;_tmp1AC=_tmp1A9->f3;}}_LL130:
Cyc_Absyndump_dumpqvar(_tmp1AA);Cyc_Absyndump_dump_char((int)'{');if(_tmp1AB != 0)
Cyc_Absyndump_dumptps(_tmp1AB);{const char*_tmp43E;const char*_tmp43D;const char*
_tmp43C;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp1AC,((_tmp43C="",_tag_dyneither(_tmp43C,sizeof(char),1))),((
_tmp43D="}",_tag_dyneither(_tmp43D,sizeof(char),2))),((_tmp43E=",",
_tag_dyneither(_tmp43E,sizeof(char),2))));}goto _LLD8;_LL131: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp1AD=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp120;if(_tmp1AD->tag != 30)goto
_LL133;else{_tmp1AE=_tmp1AD->f2;}}_LL132:{const char*_tmp441;const char*_tmp440;
const char*_tmp43F;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,
struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp1AE,((_tmp43F="{",_tag_dyneither(_tmp43F,sizeof(char),2))),((
_tmp440="}",_tag_dyneither(_tmp440,sizeof(char),2))),((_tmp441=",",
_tag_dyneither(_tmp441,sizeof(char),2))));}goto _LLD8;_LL133: {struct Cyc_Absyn_Datatype_e_struct*
_tmp1AF=(struct Cyc_Absyn_Datatype_e_struct*)_tmp120;if(_tmp1AF->tag != 31)goto
_LL135;else{_tmp1B0=_tmp1AF->f1;_tmp1B1=_tmp1AF->f3;}}_LL134: Cyc_Absyndump_dumpqvar(
_tmp1B1->name);if(_tmp1B0 != 0){const char*_tmp444;const char*_tmp443;const char*
_tmp442;((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp,_tmp1B0,((_tmp442="(",_tag_dyneither(_tmp442,sizeof(char),
2))),((_tmp443=")",_tag_dyneither(_tmp443,sizeof(char),2))),((_tmp444=",",
_tag_dyneither(_tmp444,sizeof(char),2))));}goto _LLD8;_LL135: {struct Cyc_Absyn_Enum_e_struct*
_tmp1B2=(struct Cyc_Absyn_Enum_e_struct*)_tmp120;if(_tmp1B2->tag != 32)goto _LL137;
else{_tmp1B3=_tmp1B2->f1;}}_LL136: Cyc_Absyndump_dumpqvar(_tmp1B3);goto _LLD8;
_LL137: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp1B4=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp120;if(_tmp1B4->tag != 33)goto _LL139;else{_tmp1B5=_tmp1B4->f1;}}_LL138: Cyc_Absyndump_dumpqvar(
_tmp1B5);goto _LLD8;_LL139: {struct Cyc_Absyn_Malloc_e_struct*_tmp1B6=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp120;if(_tmp1B6->tag != 34)goto _LL13B;else{_tmp1B7=_tmp1B6->f1;_tmp1B8=_tmp1B7.is_calloc;
_tmp1B9=_tmp1B7.rgn;_tmp1BA=_tmp1B7.elt_type;_tmp1BB=_tmp1B7.num_elts;}}_LL13A:
if(_tmp1B8){if(_tmp1B9 != 0){{const char*_tmp445;Cyc_Absyndump_dump(((_tmp445="rcalloc(",
_tag_dyneither(_tmp445,sizeof(char),9))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_tmp1B9);{const char*_tmp446;Cyc_Absyndump_dump(((_tmp446=",",_tag_dyneither(
_tmp446,sizeof(char),2))));};}else{const char*_tmp447;Cyc_Absyndump_dump(((
_tmp447="calloc",_tag_dyneither(_tmp447,sizeof(char),7))));}Cyc_Absyndump_dumpexp(
_tmp1BB);{const char*_tmp448;Cyc_Absyndump_dump(((_tmp448=",",_tag_dyneither(
_tmp448,sizeof(char),2))));}Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((
void**)_check_null(_tmp1BA)),0));{const char*_tmp449;Cyc_Absyndump_dump(((_tmp449=")",
_tag_dyneither(_tmp449,sizeof(char),2))));};}else{if(_tmp1B9 != 0){{const char*
_tmp44A;Cyc_Absyndump_dump(((_tmp44A="rmalloc(",_tag_dyneither(_tmp44A,sizeof(
char),9))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp1B9);{const char*
_tmp44B;Cyc_Absyndump_dump(((_tmp44B=",",_tag_dyneither(_tmp44B,sizeof(char),2))));};}
else{const char*_tmp44C;Cyc_Absyndump_dump(((_tmp44C="malloc(",_tag_dyneither(
_tmp44C,sizeof(char),8))));}if(_tmp1BA != 0)Cyc_Absyndump_dumpexp(Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp1BA,0),_tmp1BB,0));else{Cyc_Absyndump_dumpexp(
_tmp1BB);}{const char*_tmp44D;Cyc_Absyndump_dump(((_tmp44D=")",_tag_dyneither(
_tmp44D,sizeof(char),2))));};}goto _LLD8;_LL13B: {struct Cyc_Absyn_Swap_e_struct*
_tmp1BC=(struct Cyc_Absyn_Swap_e_struct*)_tmp120;if(_tmp1BC->tag != 35)goto _LL13D;
else{_tmp1BD=_tmp1BC->f1;_tmp1BE=_tmp1BC->f2;}}_LL13C:{const char*_tmp44E;Cyc_Absyndump_dump(((
_tmp44E="cycswap(",_tag_dyneither(_tmp44E,sizeof(char),9))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp1BD);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp1BE);Cyc_Absyndump_dump_char((int)')');goto _LLD8;_LL13D: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp1BF=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp120;if(_tmp1BF->tag != 36)
goto _LL13F;else{_tmp1C0=_tmp1BF->f1;_tmp1C1=_tmp1BF->f2;}}_LL13E:{const char*
_tmp451;const char*_tmp450;const char*_tmp44F;((void(*)(void(*f)(struct _tuple16*),
struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct
_dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp1C1,((_tmp44F="{",
_tag_dyneither(_tmp44F,sizeof(char),2))),((_tmp450="}",_tag_dyneither(_tmp450,
sizeof(char),2))),((_tmp451=",",_tag_dyneither(_tmp451,sizeof(char),2))));}goto
_LLD8;_LL13F: {struct Cyc_Absyn_StmtExp_e_struct*_tmp1C2=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp120;if(_tmp1C2->tag != 37)goto _LLD8;else{_tmp1C3=_tmp1C2->f1;}}_LL140:{const
char*_tmp452;Cyc_Absyndump_dump_nospace(((_tmp452="({",_tag_dyneither(_tmp452,
sizeof(char),3))));}Cyc_Absyndump_dumpstmt(_tmp1C3,1);{const char*_tmp453;Cyc_Absyndump_dump_nospace(((
_tmp453="})",_tag_dyneither(_tmp453,sizeof(char),3))));}goto _LLD8;_LLD8:;}if(
inprec >= myprec)Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp*e);void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*e){Cyc_Absyndump_dumpexp_prec(
0,e);}void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List*scs);void Cyc_Absyndump_dumpswitchclauses(
struct Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause*
_tmp20A=(struct Cyc_Absyn_Switch_clause*)scs->hd;if(_tmp20A->where_clause == 0  && (
_tmp20A->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){const char*_tmp454;Cyc_Absyndump_dump(((
_tmp454="default:",_tag_dyneither(_tmp454,sizeof(char),9))));}else{{const char*
_tmp455;Cyc_Absyndump_dump(((_tmp455="case",_tag_dyneither(_tmp455,sizeof(char),
5))));}Cyc_Absyndump_dumppat(_tmp20A->pattern);if(_tmp20A->where_clause != 0){{
const char*_tmp456;Cyc_Absyndump_dump(((_tmp456="&&",_tag_dyneither(_tmp456,
sizeof(char),3))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(
_tmp20A->where_clause));}{const char*_tmp457;Cyc_Absyndump_dump_nospace(((_tmp457=":",
_tag_dyneither(_tmp457,sizeof(char),2))));};}Cyc_Absyndump_dumpstmt(_tmp20A->body,
0);}}void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt);void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt*s,int expstmt){void*_tmp20F=s->r;struct Cyc_Absyn_Exp*_tmp212;
struct Cyc_Absyn_Stmt*_tmp214;struct Cyc_Absyn_Stmt*_tmp215;struct Cyc_Absyn_Exp*
_tmp217;struct Cyc_Absyn_Exp*_tmp219;struct Cyc_Absyn_Exp*_tmp21B;struct Cyc_Absyn_Stmt*
_tmp21C;struct Cyc_Absyn_Stmt*_tmp21D;struct _tuple8 _tmp21F;struct Cyc_Absyn_Exp*
_tmp220;struct Cyc_Absyn_Stmt*_tmp221;struct _dyneither_ptr*_tmp225;struct Cyc_Absyn_Exp*
_tmp227;struct _tuple8 _tmp228;struct Cyc_Absyn_Exp*_tmp229;struct _tuple8 _tmp22A;
struct Cyc_Absyn_Exp*_tmp22B;struct Cyc_Absyn_Stmt*_tmp22C;struct Cyc_Absyn_Exp*
_tmp22E;struct Cyc_List_List*_tmp22F;struct Cyc_Absyn_Decl*_tmp231;struct Cyc_Absyn_Stmt*
_tmp232;struct _dyneither_ptr*_tmp234;struct Cyc_Absyn_Stmt*_tmp235;struct Cyc_Absyn_Stmt*
_tmp237;struct _tuple8 _tmp238;struct Cyc_Absyn_Exp*_tmp239;struct Cyc_List_List*
_tmp23B;struct Cyc_List_List*_tmp23D;struct Cyc_Absyn_Stmt*_tmp23F;struct Cyc_List_List*
_tmp240;struct Cyc_Absyn_Exp*_tmp242;_LL146: {struct Cyc_Absyn_Skip_s_struct*
_tmp210=(struct Cyc_Absyn_Skip_s_struct*)_tmp20F;if(_tmp210->tag != 0)goto _LL148;}
_LL147: Cyc_Absyndump_dump_semi();goto _LL145;_LL148: {struct Cyc_Absyn_Exp_s_struct*
_tmp211=(struct Cyc_Absyn_Exp_s_struct*)_tmp20F;if(_tmp211->tag != 1)goto _LL14A;
else{_tmp212=_tmp211->f1;}}_LL149: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dumpexp(
_tmp212);Cyc_Absyndump_dump_semi();goto _LL145;_LL14A: {struct Cyc_Absyn_Seq_s_struct*
_tmp213=(struct Cyc_Absyn_Seq_s_struct*)_tmp20F;if(_tmp213->tag != 2)goto _LL14C;
else{_tmp214=_tmp213->f1;_tmp215=_tmp213->f2;}}_LL14B: if(Cyc_Absynpp_is_declaration(
_tmp214)){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp214,0);
Cyc_Absyndump_dump_char((int)'}');}else{Cyc_Absyndump_dumpstmt(_tmp214,0);}if(
Cyc_Absynpp_is_declaration(_tmp215)){if(expstmt)Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp215,expstmt);Cyc_Absyndump_dump_char((
int)'}');if(expstmt)Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dump_semi();}
else{Cyc_Absyndump_dumpstmt(_tmp215,expstmt);}goto _LL145;_LL14C: {struct Cyc_Absyn_Return_s_struct*
_tmp216=(struct Cyc_Absyn_Return_s_struct*)_tmp20F;if(_tmp216->tag != 3)goto _LL14E;
else{_tmp217=_tmp216->f1;if(_tmp217 != 0)goto _LL14E;}}_LL14D: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp458;Cyc_Absyndump_dump(((_tmp458="return;",_tag_dyneither(
_tmp458,sizeof(char),8))));}goto _LL145;_LL14E: {struct Cyc_Absyn_Return_s_struct*
_tmp218=(struct Cyc_Absyn_Return_s_struct*)_tmp20F;if(_tmp218->tag != 3)goto _LL150;
else{_tmp219=_tmp218->f1;}}_LL14F: Cyc_Absyndump_dumploc(s->loc);{const char*
_tmp459;Cyc_Absyndump_dump(((_tmp459="return",_tag_dyneither(_tmp459,sizeof(char),
7))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp219));Cyc_Absyndump_dump_semi();
goto _LL145;_LL150: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp21A=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp20F;if(_tmp21A->tag != 4)goto _LL152;else{_tmp21B=_tmp21A->f1;_tmp21C=_tmp21A->f2;
_tmp21D=_tmp21A->f3;}}_LL151: Cyc_Absyndump_dumploc(s->loc);{const char*_tmp45A;
Cyc_Absyndump_dump(((_tmp45A="if(",_tag_dyneither(_tmp45A,sizeof(char),4))));}
Cyc_Absyndump_dumpexp(_tmp21B);{void*_tmp246=_tmp21C->r;_LL16D: {struct Cyc_Absyn_Seq_s_struct*
_tmp247=(struct Cyc_Absyn_Seq_s_struct*)_tmp246;if(_tmp247->tag != 2)goto _LL16F;}
_LL16E: goto _LL170;_LL16F: {struct Cyc_Absyn_Decl_s_struct*_tmp248=(struct Cyc_Absyn_Decl_s_struct*)
_tmp246;if(_tmp248->tag != 12)goto _LL171;}_LL170: goto _LL172;_LL171: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp249=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp246;if(_tmp249->tag != 4)goto
_LL173;}_LL172: goto _LL174;_LL173: {struct Cyc_Absyn_Label_s_struct*_tmp24A=(
struct Cyc_Absyn_Label_s_struct*)_tmp246;if(_tmp24A->tag != 13)goto _LL175;}_LL174:{
const char*_tmp45B;Cyc_Absyndump_dump_nospace(((_tmp45B="){",_tag_dyneither(
_tmp45B,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(_tmp21C,0);Cyc_Absyndump_dump_char((
int)'}');goto _LL16C;_LL175:;_LL176: Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(
_tmp21C,0);_LL16C:;}{void*_tmp24C=_tmp21D->r;_LL178: {struct Cyc_Absyn_Skip_s_struct*
_tmp24D=(struct Cyc_Absyn_Skip_s_struct*)_tmp24C;if(_tmp24D->tag != 0)goto _LL17A;}
_LL179: goto _LL177;_LL17A:;_LL17B:{const char*_tmp45C;Cyc_Absyndump_dump(((_tmp45C="else{",
_tag_dyneither(_tmp45C,sizeof(char),6))));}Cyc_Absyndump_dumpstmt(_tmp21D,0);Cyc_Absyndump_dump_char((
int)'}');goto _LL177;_LL177:;}goto _LL145;_LL152: {struct Cyc_Absyn_While_s_struct*
_tmp21E=(struct Cyc_Absyn_While_s_struct*)_tmp20F;if(_tmp21E->tag != 5)goto _LL154;
else{_tmp21F=_tmp21E->f1;_tmp220=_tmp21F.f1;_tmp221=_tmp21E->f2;}}_LL153: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp45D;Cyc_Absyndump_dump(((_tmp45D="while(",_tag_dyneither(
_tmp45D,sizeof(char),7))));}Cyc_Absyndump_dumpexp(_tmp220);{const char*_tmp45E;
Cyc_Absyndump_dump_nospace(((_tmp45E="){",_tag_dyneither(_tmp45E,sizeof(char),3))));}
Cyc_Absyndump_dumpstmt(_tmp221,0);Cyc_Absyndump_dump_char((int)'}');goto _LL145;
_LL154: {struct Cyc_Absyn_Break_s_struct*_tmp222=(struct Cyc_Absyn_Break_s_struct*)
_tmp20F;if(_tmp222->tag != 6)goto _LL156;}_LL155: Cyc_Absyndump_dumploc(s->loc);{
const char*_tmp45F;Cyc_Absyndump_dump(((_tmp45F="break;",_tag_dyneither(_tmp45F,
sizeof(char),7))));}goto _LL145;_LL156: {struct Cyc_Absyn_Continue_s_struct*
_tmp223=(struct Cyc_Absyn_Continue_s_struct*)_tmp20F;if(_tmp223->tag != 7)goto
_LL158;}_LL157: Cyc_Absyndump_dumploc(s->loc);{const char*_tmp460;Cyc_Absyndump_dump(((
_tmp460="continue;",_tag_dyneither(_tmp460,sizeof(char),10))));}goto _LL145;
_LL158: {struct Cyc_Absyn_Goto_s_struct*_tmp224=(struct Cyc_Absyn_Goto_s_struct*)
_tmp20F;if(_tmp224->tag != 8)goto _LL15A;else{_tmp225=_tmp224->f1;}}_LL159: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp461;Cyc_Absyndump_dump(((_tmp461="goto",_tag_dyneither(
_tmp461,sizeof(char),5))));}Cyc_Absyndump_dump_str(_tmp225);Cyc_Absyndump_dump_semi();
goto _LL145;_LL15A: {struct Cyc_Absyn_For_s_struct*_tmp226=(struct Cyc_Absyn_For_s_struct*)
_tmp20F;if(_tmp226->tag != 9)goto _LL15C;else{_tmp227=_tmp226->f1;_tmp228=_tmp226->f2;
_tmp229=_tmp228.f1;_tmp22A=_tmp226->f3;_tmp22B=_tmp22A.f1;_tmp22C=_tmp226->f4;}}
_LL15B: Cyc_Absyndump_dumploc(s->loc);{const char*_tmp462;Cyc_Absyndump_dump(((
_tmp462="for(",_tag_dyneither(_tmp462,sizeof(char),5))));}Cyc_Absyndump_dumpexp(
_tmp227);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp229);Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp(_tmp22B);{const char*_tmp463;Cyc_Absyndump_dump_nospace(((
_tmp463="){",_tag_dyneither(_tmp463,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(
_tmp22C,0);Cyc_Absyndump_dump_char((int)'}');goto _LL145;_LL15C: {struct Cyc_Absyn_Switch_s_struct*
_tmp22D=(struct Cyc_Absyn_Switch_s_struct*)_tmp20F;if(_tmp22D->tag != 10)goto
_LL15E;else{_tmp22E=_tmp22D->f1;_tmp22F=_tmp22D->f2;}}_LL15D: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp464;Cyc_Absyndump_dump(((_tmp464="switch(",_tag_dyneither(
_tmp464,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp22E);{const char*_tmp465;
Cyc_Absyndump_dump_nospace(((_tmp465="){",_tag_dyneither(_tmp465,sizeof(char),3))));}
Cyc_Absyndump_dumpswitchclauses(_tmp22F);Cyc_Absyndump_dump_char((int)'}');goto
_LL145;_LL15E: {struct Cyc_Absyn_Decl_s_struct*_tmp230=(struct Cyc_Absyn_Decl_s_struct*)
_tmp20F;if(_tmp230->tag != 12)goto _LL160;else{_tmp231=_tmp230->f1;_tmp232=_tmp230->f2;}}
_LL15F: Cyc_Absyndump_dumpdecl(_tmp231);Cyc_Absyndump_dumpstmt(_tmp232,expstmt);
goto _LL145;_LL160: {struct Cyc_Absyn_Label_s_struct*_tmp233=(struct Cyc_Absyn_Label_s_struct*)
_tmp20F;if(_tmp233->tag != 13)goto _LL162;else{_tmp234=_tmp233->f1;_tmp235=_tmp233->f2;}}
_LL161: if(Cyc_Absynpp_is_declaration(_tmp235)){Cyc_Absyndump_dump_str(_tmp234);
if(expstmt){const char*_tmp466;Cyc_Absyndump_dump_nospace(((_tmp466=": ({",
_tag_dyneither(_tmp466,sizeof(char),5))));}else{const char*_tmp467;Cyc_Absyndump_dump_nospace(((
_tmp467=": {",_tag_dyneither(_tmp467,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(
_tmp235,expstmt);if(expstmt){const char*_tmp468;Cyc_Absyndump_dump_nospace(((
_tmp468="});}",_tag_dyneither(_tmp468,sizeof(char),5))));}else{Cyc_Absyndump_dump_char((
int)'}');}}else{Cyc_Absyndump_dump_str(_tmp234);Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpstmt(_tmp235,expstmt);}goto _LL145;_LL162: {struct Cyc_Absyn_Do_s_struct*
_tmp236=(struct Cyc_Absyn_Do_s_struct*)_tmp20F;if(_tmp236->tag != 14)goto _LL164;
else{_tmp237=_tmp236->f1;_tmp238=_tmp236->f2;_tmp239=_tmp238.f1;}}_LL163: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp469;Cyc_Absyndump_dump(((_tmp469="do{",_tag_dyneither(
_tmp469,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(_tmp237,0);{const char*_tmp46A;
Cyc_Absyndump_dump_nospace(((_tmp46A="}while(",_tag_dyneither(_tmp46A,sizeof(
char),8))));}Cyc_Absyndump_dumpexp(_tmp239);{const char*_tmp46B;Cyc_Absyndump_dump_nospace(((
_tmp46B=");",_tag_dyneither(_tmp46B,sizeof(char),3))));}goto _LL145;_LL164: {
struct Cyc_Absyn_Fallthru_s_struct*_tmp23A=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp20F;if(_tmp23A->tag != 11)goto _LL166;else{_tmp23B=_tmp23A->f1;if(_tmp23B != 0)
goto _LL166;}}_LL165:{const char*_tmp46C;Cyc_Absyndump_dump(((_tmp46C="fallthru;",
_tag_dyneither(_tmp46C,sizeof(char),10))));}goto _LL145;_LL166: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp23C=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp20F;if(_tmp23C->tag != 11)goto
_LL168;else{_tmp23D=_tmp23C->f1;}}_LL167:{const char*_tmp46D;Cyc_Absyndump_dump(((
_tmp46D="fallthru(",_tag_dyneither(_tmp46D,sizeof(char),10))));}Cyc_Absyndump_dumpexps_prec(
20,_tmp23D);{const char*_tmp46E;Cyc_Absyndump_dump_nospace(((_tmp46E=");",
_tag_dyneither(_tmp46E,sizeof(char),3))));}goto _LL145;_LL168: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp23E=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp20F;if(_tmp23E->tag != 15)goto
_LL16A;else{_tmp23F=_tmp23E->f1;_tmp240=_tmp23E->f2;}}_LL169: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp46F;Cyc_Absyndump_dump(((_tmp46F="try",_tag_dyneither(
_tmp46F,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(_tmp23F,0);{const char*_tmp470;
Cyc_Absyndump_dump(((_tmp470="catch{",_tag_dyneither(_tmp470,sizeof(char),7))));}
Cyc_Absyndump_dumpswitchclauses(_tmp240);Cyc_Absyndump_dump_char((int)'}');goto
_LL145;_LL16A: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp241=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp20F;if(_tmp241->tag != 16)goto _LL145;else{_tmp242=_tmp241->f1;}}_LL16B: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp471;Cyc_Absyndump_dump(((_tmp471="reset_region(",
_tag_dyneither(_tmp471,sizeof(char),14))));}Cyc_Absyndump_dumpexp(_tmp242);{
const char*_tmp472;Cyc_Absyndump_dump(((_tmp472=");",_tag_dyneither(_tmp472,
sizeof(char),3))));}goto _LL145;_LL145:;}struct _tuple17{struct Cyc_List_List*f1;
struct Cyc_Absyn_Pat*f2;};void Cyc_Absyndump_dumpdp(struct _tuple17*dp);void Cyc_Absyndump_dumpdp(
struct _tuple17*dp){{const char*_tmp475;const char*_tmp474;const char*_tmp473;((void(*)(
void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,(*
dp).f1,((_tmp473="",_tag_dyneither(_tmp473,sizeof(char),1))),((_tmp474="=",
_tag_dyneither(_tmp474,sizeof(char),2))),((_tmp475="=",_tag_dyneither(_tmp475,
sizeof(char),2))));}Cyc_Absyndump_dumppat((*dp).f2);}void Cyc_Absyndump_dumppat(
struct Cyc_Absyn_Pat*p);void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){void*
_tmp268=p->r;enum Cyc_Absyn_Sign _tmp26C;int _tmp26D;enum Cyc_Absyn_Sign _tmp26F;
int _tmp270;enum Cyc_Absyn_Sign _tmp272;int _tmp273;char _tmp275;struct
_dyneither_ptr _tmp277;struct Cyc_Absyn_Vardecl*_tmp279;struct Cyc_Absyn_Pat*
_tmp27A;struct Cyc_Absyn_Pat _tmp27B;void*_tmp27C;struct Cyc_Absyn_Vardecl*_tmp27F;
struct Cyc_Absyn_Pat*_tmp280;struct Cyc_List_List*_tmp282;int _tmp283;struct Cyc_Absyn_Pat*
_tmp285;struct Cyc_Absyn_Vardecl*_tmp287;struct Cyc_Absyn_Pat*_tmp288;struct Cyc_Absyn_Pat
_tmp289;void*_tmp28A;struct Cyc_Absyn_Vardecl*_tmp28D;struct Cyc_Absyn_Pat*_tmp28E;
struct Cyc_Absyn_Tvar*_tmp290;struct Cyc_Absyn_Vardecl*_tmp291;struct _tuple0*
_tmp293;struct _tuple0*_tmp295;struct Cyc_List_List*_tmp296;int _tmp297;struct Cyc_Absyn_AggrInfo*
_tmp299;struct Cyc_Absyn_AggrInfo _tmp29A;union Cyc_Absyn_AggrInfoU _tmp29B;struct
Cyc_List_List*_tmp29C;struct Cyc_List_List*_tmp29D;int _tmp29E;struct Cyc_Absyn_AggrInfo*
_tmp2A0;struct Cyc_List_List*_tmp2A1;struct Cyc_List_List*_tmp2A2;int _tmp2A3;
struct Cyc_Absyn_Datatypefield*_tmp2A5;struct Cyc_List_List*_tmp2A6;int _tmp2A7;
struct Cyc_Absyn_Enumfield*_tmp2A9;struct Cyc_Absyn_Enumfield*_tmp2AB;struct Cyc_Absyn_Exp*
_tmp2AD;_LL17D: {struct Cyc_Absyn_Wild_p_struct*_tmp269=(struct Cyc_Absyn_Wild_p_struct*)
_tmp268;if(_tmp269->tag != 0)goto _LL17F;}_LL17E: Cyc_Absyndump_dump_char((int)'_');
goto _LL17C;_LL17F: {struct Cyc_Absyn_Null_p_struct*_tmp26A=(struct Cyc_Absyn_Null_p_struct*)
_tmp268;if(_tmp26A->tag != 8)goto _LL181;}_LL180:{const char*_tmp476;Cyc_Absyndump_dump(((
_tmp476="NULL",_tag_dyneither(_tmp476,sizeof(char),5))));}goto _LL17C;_LL181: {
struct Cyc_Absyn_Int_p_struct*_tmp26B=(struct Cyc_Absyn_Int_p_struct*)_tmp268;if(
_tmp26B->tag != 9)goto _LL183;else{_tmp26C=_tmp26B->f1;if(_tmp26C != Cyc_Absyn_None)
goto _LL183;_tmp26D=_tmp26B->f2;}}_LL182: _tmp270=_tmp26D;goto _LL184;_LL183: {
struct Cyc_Absyn_Int_p_struct*_tmp26E=(struct Cyc_Absyn_Int_p_struct*)_tmp268;if(
_tmp26E->tag != 9)goto _LL185;else{_tmp26F=_tmp26E->f1;if(_tmp26F != Cyc_Absyn_Signed)
goto _LL185;_tmp270=_tmp26E->f2;}}_LL184:{const char*_tmp47A;void*_tmp479[1];
struct Cyc_Int_pa_struct _tmp478;Cyc_Absyndump_dump((struct _dyneither_ptr)((
_tmp478.tag=1,((_tmp478.f1=(unsigned long)_tmp270,((_tmp479[0]=& _tmp478,Cyc_aprintf(((
_tmp47A="%d",_tag_dyneither(_tmp47A,sizeof(char),3))),_tag_dyneither(_tmp479,
sizeof(void*),1)))))))));}goto _LL17C;_LL185: {struct Cyc_Absyn_Int_p_struct*
_tmp271=(struct Cyc_Absyn_Int_p_struct*)_tmp268;if(_tmp271->tag != 9)goto _LL187;
else{_tmp272=_tmp271->f1;if(_tmp272 != Cyc_Absyn_Unsigned)goto _LL187;_tmp273=
_tmp271->f2;}}_LL186:{const char*_tmp47E;void*_tmp47D[1];struct Cyc_Int_pa_struct
_tmp47C;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp47C.tag=1,((_tmp47C.f1=(
unsigned int)_tmp273,((_tmp47D[0]=& _tmp47C,Cyc_aprintf(((_tmp47E="%u",
_tag_dyneither(_tmp47E,sizeof(char),3))),_tag_dyneither(_tmp47D,sizeof(void*),1)))))))));}
goto _LL17C;_LL187: {struct Cyc_Absyn_Char_p_struct*_tmp274=(struct Cyc_Absyn_Char_p_struct*)
_tmp268;if(_tmp274->tag != 10)goto _LL189;else{_tmp275=_tmp274->f1;}}_LL188:{const
char*_tmp47F;Cyc_Absyndump_dump(((_tmp47F="'",_tag_dyneither(_tmp47F,sizeof(char),
2))));}Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp275));{const char*
_tmp480;Cyc_Absyndump_dump_nospace(((_tmp480="'",_tag_dyneither(_tmp480,sizeof(
char),2))));}goto _LL17C;_LL189: {struct Cyc_Absyn_Float_p_struct*_tmp276=(struct
Cyc_Absyn_Float_p_struct*)_tmp268;if(_tmp276->tag != 11)goto _LL18B;else{_tmp277=
_tmp276->f1;}}_LL18A: Cyc_Absyndump_dump(_tmp277);goto _LL17C;_LL18B: {struct Cyc_Absyn_Var_p_struct*
_tmp278=(struct Cyc_Absyn_Var_p_struct*)_tmp268;if(_tmp278->tag != 1)goto _LL18D;
else{_tmp279=_tmp278->f1;_tmp27A=_tmp278->f2;_tmp27B=*_tmp27A;_tmp27C=_tmp27B.r;{
struct Cyc_Absyn_Wild_p_struct*_tmp27D=(struct Cyc_Absyn_Wild_p_struct*)_tmp27C;
if(_tmp27D->tag != 0)goto _LL18D;};}}_LL18C: Cyc_Absyndump_dumpqvar(_tmp279->name);
goto _LL17C;_LL18D: {struct Cyc_Absyn_Var_p_struct*_tmp27E=(struct Cyc_Absyn_Var_p_struct*)
_tmp268;if(_tmp27E->tag != 1)goto _LL18F;else{_tmp27F=_tmp27E->f1;_tmp280=_tmp27E->f2;}}
_LL18E: Cyc_Absyndump_dumpqvar(_tmp27F->name);{const char*_tmp481;Cyc_Absyndump_dump(((
_tmp481=" as ",_tag_dyneither(_tmp481,sizeof(char),5))));}Cyc_Absyndump_dumppat(
_tmp280);goto _LL17C;_LL18F: {struct Cyc_Absyn_Tuple_p_struct*_tmp281=(struct Cyc_Absyn_Tuple_p_struct*)
_tmp268;if(_tmp281->tag != 4)goto _LL191;else{_tmp282=_tmp281->f1;_tmp283=_tmp281->f2;}}
_LL190: {const char*_tmp483;const char*_tmp482;struct _dyneither_ptr term=_tmp283?(
_tmp483=", ...)",_tag_dyneither(_tmp483,sizeof(char),7)):((_tmp482=")",
_tag_dyneither(_tmp482,sizeof(char),2)));{const char*_tmp485;const char*_tmp484;((
void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp282,((_tmp484="$(",_tag_dyneither(_tmp484,sizeof(char),
3))),term,((_tmp485=",",_tag_dyneither(_tmp485,sizeof(char),2))));}goto _LL17C;}
_LL191: {struct Cyc_Absyn_Pointer_p_struct*_tmp284=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp268;if(_tmp284->tag != 5)goto _LL193;else{_tmp285=_tmp284->f1;}}_LL192:{const
char*_tmp486;Cyc_Absyndump_dump(((_tmp486="&",_tag_dyneither(_tmp486,sizeof(char),
2))));}Cyc_Absyndump_dumppat(_tmp285);goto _LL17C;_LL193: {struct Cyc_Absyn_Reference_p_struct*
_tmp286=(struct Cyc_Absyn_Reference_p_struct*)_tmp268;if(_tmp286->tag != 2)goto
_LL195;else{_tmp287=_tmp286->f1;_tmp288=_tmp286->f2;_tmp289=*_tmp288;_tmp28A=
_tmp289.r;{struct Cyc_Absyn_Wild_p_struct*_tmp28B=(struct Cyc_Absyn_Wild_p_struct*)
_tmp28A;if(_tmp28B->tag != 0)goto _LL195;};}}_LL194:{const char*_tmp487;Cyc_Absyndump_dump(((
_tmp487="*",_tag_dyneither(_tmp487,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(
_tmp287->name);goto _LL17C;_LL195: {struct Cyc_Absyn_Reference_p_struct*_tmp28C=(
struct Cyc_Absyn_Reference_p_struct*)_tmp268;if(_tmp28C->tag != 2)goto _LL197;else{
_tmp28D=_tmp28C->f1;_tmp28E=_tmp28C->f2;}}_LL196:{const char*_tmp488;Cyc_Absyndump_dump(((
_tmp488="*",_tag_dyneither(_tmp488,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(
_tmp28D->name);{const char*_tmp489;Cyc_Absyndump_dump(((_tmp489=" as ",
_tag_dyneither(_tmp489,sizeof(char),5))));}Cyc_Absyndump_dumppat(_tmp28E);goto
_LL17C;_LL197: {struct Cyc_Absyn_TagInt_p_struct*_tmp28F=(struct Cyc_Absyn_TagInt_p_struct*)
_tmp268;if(_tmp28F->tag != 3)goto _LL199;else{_tmp290=_tmp28F->f1;_tmp291=_tmp28F->f2;}}
_LL198: Cyc_Absyndump_dumpqvar(_tmp291->name);Cyc_Absyndump_dump_char((int)'<');
Cyc_Absyndump_dumptvar(_tmp290);Cyc_Absyndump_dump_char((int)'>');goto _LL17C;
_LL199: {struct Cyc_Absyn_UnknownId_p_struct*_tmp292=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp268;if(_tmp292->tag != 14)goto _LL19B;else{_tmp293=_tmp292->f1;}}_LL19A: Cyc_Absyndump_dumpqvar(
_tmp293);goto _LL17C;_LL19B: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp294=(
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp268;if(_tmp294->tag != 15)goto _LL19D;
else{_tmp295=_tmp294->f1;_tmp296=_tmp294->f2;_tmp297=_tmp294->f3;}}_LL19C: {
const char*_tmp48B;const char*_tmp48A;struct _dyneither_ptr term=_tmp297?(_tmp48B=", ...)",
_tag_dyneither(_tmp48B,sizeof(char),7)):((_tmp48A=")",_tag_dyneither(_tmp48A,
sizeof(char),2)));Cyc_Absyndump_dumpqvar(_tmp295);{const char*_tmp48D;const char*
_tmp48C;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp296,((_tmp48C="(",_tag_dyneither(_tmp48C,sizeof(char),
2))),term,((_tmp48D=",",_tag_dyneither(_tmp48D,sizeof(char),2))));}goto _LL17C;}
_LL19D: {struct Cyc_Absyn_Aggr_p_struct*_tmp298=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp268;if(_tmp298->tag != 6)goto _LL19F;else{_tmp299=_tmp298->f1;if(_tmp299 == 0)
goto _LL19F;_tmp29A=*_tmp299;_tmp29B=_tmp29A.aggr_info;_tmp29C=_tmp298->f2;
_tmp29D=_tmp298->f3;_tmp29E=_tmp298->f4;}}_LL19E: {struct _tuple0*_tmp2C5;struct
_tuple9 _tmp2C4=Cyc_Absyn_aggr_kinded_name(_tmp29B);_tmp2C5=_tmp2C4.f2;{const char*
_tmp48F;const char*_tmp48E;struct _dyneither_ptr term=_tmp29E?(_tmp48F=", ...)",
_tag_dyneither(_tmp48F,sizeof(char),7)):((_tmp48E=")",_tag_dyneither(_tmp48E,
sizeof(char),2)));Cyc_Absyndump_dumpqvar(_tmp2C5);Cyc_Absyndump_dump_char((int)'{');{
const char*_tmp492;const char*_tmp491;const char*_tmp490;((void(*)(void(*f)(struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,
_tmp29C,((_tmp490="[",_tag_dyneither(_tmp490,sizeof(char),2))),((_tmp491="]",
_tag_dyneither(_tmp491,sizeof(char),2))),((_tmp492=",",_tag_dyneither(_tmp492,
sizeof(char),2))));}{const char*_tmp494;const char*_tmp493;((void(*)(void(*f)(
struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,
_tmp29D,((_tmp493="",_tag_dyneither(_tmp493,sizeof(char),1))),term,((_tmp494=",",
_tag_dyneither(_tmp494,sizeof(char),2))));}goto _LL17C;};}_LL19F: {struct Cyc_Absyn_Aggr_p_struct*
_tmp29F=(struct Cyc_Absyn_Aggr_p_struct*)_tmp268;if(_tmp29F->tag != 6)goto _LL1A1;
else{_tmp2A0=_tmp29F->f1;if(_tmp2A0 != 0)goto _LL1A1;_tmp2A1=_tmp29F->f2;_tmp2A2=
_tmp29F->f3;_tmp2A3=_tmp29F->f4;}}_LL1A0: {const char*_tmp496;const char*_tmp495;
struct _dyneither_ptr term=_tmp2A3?(_tmp496=", ...)",_tag_dyneither(_tmp496,
sizeof(char),7)):((_tmp495=")",_tag_dyneither(_tmp495,sizeof(char),2)));Cyc_Absyndump_dump_char((
int)'{');{const char*_tmp499;const char*_tmp498;const char*_tmp497;((void(*)(void(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,
_tmp2A1,((_tmp497="[",_tag_dyneither(_tmp497,sizeof(char),2))),((_tmp498="]",
_tag_dyneither(_tmp498,sizeof(char),2))),((_tmp499=",",_tag_dyneither(_tmp499,
sizeof(char),2))));}{const char*_tmp49B;const char*_tmp49A;((void(*)(void(*f)(
struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,
_tmp2A2,((_tmp49A="",_tag_dyneither(_tmp49A,sizeof(char),1))),term,((_tmp49B=",",
_tag_dyneither(_tmp49B,sizeof(char),2))));}goto _LL17C;}_LL1A1: {struct Cyc_Absyn_Datatype_p_struct*
_tmp2A4=(struct Cyc_Absyn_Datatype_p_struct*)_tmp268;if(_tmp2A4->tag != 7)goto
_LL1A3;else{_tmp2A5=_tmp2A4->f2;_tmp2A6=_tmp2A4->f3;_tmp2A7=_tmp2A4->f4;}}_LL1A2: {
const char*_tmp49D;const char*_tmp49C;struct _dyneither_ptr term=_tmp2A7?(_tmp49D=", ...)",
_tag_dyneither(_tmp49D,sizeof(char),7)):((_tmp49C=")",_tag_dyneither(_tmp49C,
sizeof(char),2)));Cyc_Absyndump_dumpqvar(_tmp2A5->name);if(_tmp2A6 != 0){const
char*_tmp49F;const char*_tmp49E;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct
Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct
_dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp2A6,((_tmp49E="(",
_tag_dyneither(_tmp49E,sizeof(char),2))),term,((_tmp49F=",",_tag_dyneither(
_tmp49F,sizeof(char),2))));}goto _LL17C;}_LL1A3: {struct Cyc_Absyn_Enum_p_struct*
_tmp2A8=(struct Cyc_Absyn_Enum_p_struct*)_tmp268;if(_tmp2A8->tag != 12)goto _LL1A5;
else{_tmp2A9=_tmp2A8->f2;}}_LL1A4: _tmp2AB=_tmp2A9;goto _LL1A6;_LL1A5: {struct Cyc_Absyn_AnonEnum_p_struct*
_tmp2AA=(struct Cyc_Absyn_AnonEnum_p_struct*)_tmp268;if(_tmp2AA->tag != 13)goto
_LL1A7;else{_tmp2AB=_tmp2AA->f2;}}_LL1A6: Cyc_Absyndump_dumpqvar(_tmp2AB->name);
goto _LL17C;_LL1A7: {struct Cyc_Absyn_Exp_p_struct*_tmp2AC=(struct Cyc_Absyn_Exp_p_struct*)
_tmp268;if(_tmp2AC->tag != 16)goto _LL17C;else{_tmp2AD=_tmp2AC->f1;}}_LL1A8: Cyc_Absyndump_dumpexp(
_tmp2AD);goto _LL17C;_LL17C:;}void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*
ef);void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*ef){Cyc_Absyndump_dumpqvar(
ef->name);if(ef->typs != 0)Cyc_Absyndump_dumpargs(ef->typs);}void Cyc_Absyndump_dumpdatatypefields(
struct Cyc_List_List*fields);void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*
fields){const char*_tmp4A0;((void(*)(void(*f)(struct Cyc_Absyn_Datatypefield*),
struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,
fields,((_tmp4A0=",",_tag_dyneither(_tmp4A0,sizeof(char),2))));}void Cyc_Absyndump_dumpenumfield(
struct Cyc_Absyn_Enumfield*ef);void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*
ef){Cyc_Absyndump_dumpqvar(ef->name);if(ef->tag != 0){{const char*_tmp4A1;Cyc_Absyndump_dump(((
_tmp4A1=" = ",_tag_dyneither(_tmp4A1,sizeof(char),4))));}Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}void Cyc_Absyndump_dumpenumfields(
struct Cyc_List_List*fields);void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*
fields){const char*_tmp4A2;((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct
Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,
fields,((_tmp4A2=",",_tag_dyneither(_tmp4A2,sizeof(char),2))));}void Cyc_Absyndump_dumpaggrfields(
struct Cyc_List_List*fields);void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*
fields){for(0;fields != 0;fields=fields->tl){struct Cyc_Absyn_Aggrfield _tmp2DC;
struct _dyneither_ptr*_tmp2DD;struct Cyc_Absyn_Tqual _tmp2DE;void*_tmp2DF;struct Cyc_Absyn_Exp*
_tmp2E0;struct Cyc_List_List*_tmp2E1;struct Cyc_Absyn_Aggrfield*_tmp2DB=(struct Cyc_Absyn_Aggrfield*)
fields->hd;_tmp2DC=*_tmp2DB;_tmp2DD=_tmp2DC.name;_tmp2DE=_tmp2DC.tq;_tmp2DF=
_tmp2DC.type;_tmp2E0=_tmp2DC.width;_tmp2E1=_tmp2DC.attributes;switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL1A9:((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(
struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp2DE,
_tmp2DF,Cyc_Absyndump_dump_str,_tmp2DD);Cyc_Absyndump_dumpatts(_tmp2E1);break;
case Cyc_Cyclone_Vc_c: _LL1AA: Cyc_Absyndump_dumpatts(_tmp2E1);((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(
_tmp2DE,_tmp2DF,Cyc_Absyndump_dump_str,_tmp2DD);break;}if(_tmp2E0 != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp2E0);}Cyc_Absyndump_dump_semi();}}
void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td);void Cyc_Absyndump_dumptypedefname(
struct Cyc_Absyn_Typedefdecl*td){Cyc_Absyndump_dumpqvar(td->name);Cyc_Absyndump_dumptvars(
td->tvs);}static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd);
static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dumpatts(
fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple18{void*f1;struct
_tuple0*f2;};static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple18*pr);
static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple18*pr){{void*_tmp2E2=(*pr).f1;
_LL1AD: {struct Cyc_Absyn_Unused_att_struct*_tmp2E3=(struct Cyc_Absyn_Unused_att_struct*)
_tmp2E2;if(_tmp2E3->tag != 11)goto _LL1AF;}_LL1AE: goto _LL1AC;_LL1AF: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp2E4=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp2E2;if(_tmp2E4->tag != 1)goto
_LL1B1;}_LL1B0:{const char*_tmp4A3;Cyc_Absyndump_dump(((_tmp4A3="_stdcall",
_tag_dyneither(_tmp4A3,sizeof(char),9))));}goto _LL1AC;_LL1B1: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp2E5=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp2E2;if(_tmp2E5->tag != 2)goto
_LL1B3;}_LL1B2:{const char*_tmp4A4;Cyc_Absyndump_dump(((_tmp4A4="_cdecl",
_tag_dyneither(_tmp4A4,sizeof(char),7))));}goto _LL1AC;_LL1B3: {struct Cyc_Absyn_Fastcall_att_struct*
_tmp2E6=(struct Cyc_Absyn_Fastcall_att_struct*)_tmp2E2;if(_tmp2E6->tag != 3)goto
_LL1B5;}_LL1B4:{const char*_tmp4A5;Cyc_Absyndump_dump(((_tmp4A5="_fastcall",
_tag_dyneither(_tmp4A5,sizeof(char),10))));}goto _LL1AC;_LL1B5:;_LL1B6: goto _LL1AC;
_LL1AC:;}Cyc_Absyndump_dumpqvar((*pr).f2);}static void Cyc_Absyndump_dump_callconv_fdqvar(
struct Cyc_Absyn_Fndecl*fd);static void Cyc_Absyndump_dump_callconv_fdqvar(struct
Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dump_callconv(fd->attributes);Cyc_Absyndump_dumpqvar(
fd->name);}static void Cyc_Absyndump_dumpids(struct Cyc_List_List*vds);static void
Cyc_Absyndump_dumpids(struct Cyc_List_List*vds){for(0;vds != 0;vds=vds->tl){Cyc_Absyndump_dumpqvar(((
struct Cyc_Absyn_Vardecl*)vds->hd)->name);if(vds->tl != 0)Cyc_Absyndump_dump_char((
int)',');}}void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,struct Cyc_Position_Segment*
loc);void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Vardecl _tmp2EB;enum Cyc_Absyn_Scope _tmp2EC;struct _tuple0*
_tmp2ED;struct Cyc_Absyn_Tqual _tmp2EE;void*_tmp2EF;struct Cyc_Absyn_Exp*_tmp2F0;
struct Cyc_List_List*_tmp2F1;struct Cyc_Absyn_Vardecl*_tmp2EA=vd;_tmp2EB=*_tmp2EA;
_tmp2EC=_tmp2EB.sc;_tmp2ED=_tmp2EB.name;_tmp2EE=_tmp2EB.tq;_tmp2EF=_tmp2EB.type;
_tmp2F0=_tmp2EB.initializer;_tmp2F1=_tmp2EB.attributes;Cyc_Absyndump_dumploc(loc);
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL1B7: if(_tmp2EC == Cyc_Absyn_Extern
 && Cyc_Absyndump_qvar_to_Cids){void*_tmp2F2=Cyc_Tcutil_compress(_tmp2EF);_LL1BA: {
struct Cyc_Absyn_FnType_struct*_tmp2F3=(struct Cyc_Absyn_FnType_struct*)_tmp2F2;
if(_tmp2F3->tag != 10)goto _LL1BC;}_LL1BB: goto _LL1B9;_LL1BC:;_LL1BD: Cyc_Absyndump_dumpscope(
_tmp2EC);_LL1B9:;}else{Cyc_Absyndump_dumpscope(_tmp2EC);}((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp2EE,
_tmp2EF,Cyc_Absyndump_dumpqvar,_tmp2ED);Cyc_Absyndump_dumpatts(_tmp2F1);break;
case Cyc_Cyclone_Vc_c: _LL1B8: Cyc_Absyndump_dumpatts(_tmp2F1);Cyc_Absyndump_dumpscope(
_tmp2EC);{struct _RegionHandle _tmp2F4=_new_region("temp");struct _RegionHandle*
temp=& _tmp2F4;_push_region(temp);{struct Cyc_Absyn_Tqual _tmp2F6;void*_tmp2F7;
struct Cyc_List_List*_tmp2F8;struct _tuple11 _tmp2F5=Cyc_Absynpp_to_tms(temp,
_tmp2EE,_tmp2EF);_tmp2F6=_tmp2F5.f1;_tmp2F7=_tmp2F5.f2;_tmp2F8=_tmp2F5.f3;{void*
call_conv=(void*)& Cyc_Absyn_Unused_att_val;{struct Cyc_List_List*tms2=_tmp2F8;
for(0;tms2 != 0;tms2=tms2->tl){void*_tmp2F9=(void*)tms2->hd;struct Cyc_List_List*
_tmp2FB;_LL1C0: {struct Cyc_Absyn_Attributes_mod_struct*_tmp2FA=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmp2F9;if(_tmp2FA->tag != 5)goto _LL1C2;else{_tmp2FB=_tmp2FA->f2;}}_LL1C1: for(0;
_tmp2FB != 0;_tmp2FB=_tmp2FB->tl){void*_tmp2FC=(void*)_tmp2FB->hd;_LL1C5: {struct
Cyc_Absyn_Stdcall_att_struct*_tmp2FD=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp2FC;if(_tmp2FD->tag != 1)goto _LL1C7;}_LL1C6: call_conv=(void*)& Cyc_Absyn_Stdcall_att_val;
goto _LL1C4;_LL1C7: {struct Cyc_Absyn_Cdecl_att_struct*_tmp2FE=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmp2FC;if(_tmp2FE->tag != 2)goto _LL1C9;}_LL1C8: call_conv=(void*)& Cyc_Absyn_Cdecl_att_val;
goto _LL1C4;_LL1C9: {struct Cyc_Absyn_Fastcall_att_struct*_tmp2FF=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp2FC;if(_tmp2FF->tag != 3)goto _LL1CB;}_LL1CA: call_conv=(void*)& Cyc_Absyn_Fastcall_att_val;
goto _LL1C4;_LL1CB:;_LL1CC: goto _LL1C4;_LL1C4:;}goto _LL1BF;_LL1C2:;_LL1C3: goto
_LL1BF;_LL1BF:;}}Cyc_Absyndump_dumptq(_tmp2F6);Cyc_Absyndump_dumpntyp(_tmp2F7);{
struct _tuple18 _tmp4A6;struct _tuple18 _tmp300=(_tmp4A6.f1=call_conv,((_tmp4A6.f2=
_tmp2ED,_tmp4A6)));((void(*)(struct Cyc_List_List*tms,void(*f)(struct _tuple18*),
struct _tuple18*a))Cyc_Absyndump_dumptms)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp2F8),Cyc_Absyndump_dump_callconv_qvar,& _tmp300);};};}
_npop_handler(0);break;;_pop_region(temp);};}if(_tmp2F0 != 0){Cyc_Absyndump_dump_char((
int)'=');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp2F0);}Cyc_Absyndump_dump_semi();}
struct _tuple19{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*d);void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){void*
_tmp302=d->r;struct Cyc_Absyn_Vardecl*_tmp304;struct Cyc_Absyn_Fndecl*_tmp306;
struct Cyc_Absyn_Aggrdecl*_tmp308;struct Cyc_Absyn_Datatypedecl*_tmp30A;struct Cyc_Absyn_Datatypedecl
_tmp30B;enum Cyc_Absyn_Scope _tmp30C;struct _tuple0*_tmp30D;struct Cyc_List_List*
_tmp30E;struct Cyc_Core_Opt*_tmp30F;int _tmp310;struct Cyc_Absyn_Enumdecl*_tmp312;
struct Cyc_Absyn_Enumdecl _tmp313;enum Cyc_Absyn_Scope _tmp314;struct _tuple0*
_tmp315;struct Cyc_Core_Opt*_tmp316;struct Cyc_Absyn_Pat*_tmp318;struct Cyc_Absyn_Exp*
_tmp319;struct Cyc_List_List*_tmp31B;struct Cyc_Absyn_Tvar*_tmp31D;struct Cyc_Absyn_Vardecl*
_tmp31E;int _tmp31F;struct Cyc_Absyn_Exp*_tmp320;struct Cyc_Absyn_Exp*_tmp322;
struct Cyc_Absyn_Tvar*_tmp323;struct Cyc_Absyn_Vardecl*_tmp324;struct Cyc_Absyn_Typedefdecl*
_tmp326;struct _dyneither_ptr*_tmp328;struct Cyc_List_List*_tmp329;struct _tuple0*
_tmp32B;struct Cyc_List_List*_tmp32C;struct Cyc_List_List*_tmp32E;struct Cyc_List_List*
_tmp330;struct Cyc_List_List*_tmp331;_LL1CE: {struct Cyc_Absyn_Var_d_struct*
_tmp303=(struct Cyc_Absyn_Var_d_struct*)_tmp302;if(_tmp303->tag != 0)goto _LL1D0;
else{_tmp304=_tmp303->f1;}}_LL1CF: Cyc_Absyndump_dumpvardecl(_tmp304,d->loc);goto
_LL1CD;_LL1D0: {struct Cyc_Absyn_Fn_d_struct*_tmp305=(struct Cyc_Absyn_Fn_d_struct*)
_tmp302;if(_tmp305->tag != 1)goto _LL1D2;else{_tmp306=_tmp305->f1;}}_LL1D1: Cyc_Absyndump_dumploc(
d->loc);switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL1EE: Cyc_Absyndump_dumpatts(
_tmp306->attributes);break;case Cyc_Cyclone_Gcc_c: _LL1EF: break;}if(_tmp306->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp334=Cyc_Cyclone_c_compiler;_LL1F2: if(_tmp334 != 
Cyc_Cyclone_Vc_c)goto _LL1F4;_LL1F3:{const char*_tmp4A7;Cyc_Absyndump_dump(((
_tmp4A7="__inline",_tag_dyneither(_tmp4A7,sizeof(char),9))));}goto _LL1F1;_LL1F4:;
_LL1F5:{const char*_tmp4A8;Cyc_Absyndump_dump(((_tmp4A8="inline",_tag_dyneither(
_tmp4A8,sizeof(char),7))));}goto _LL1F1;_LL1F1:;}Cyc_Absyndump_dumpscope(_tmp306->sc);{
struct Cyc_Absyn_FnType_struct _tmp4AE;struct Cyc_Absyn_FnInfo _tmp4AD;struct Cyc_Absyn_FnType_struct*
_tmp4AC;void*t=(void*)((_tmp4AC=_cycalloc(sizeof(*_tmp4AC)),((_tmp4AC[0]=((
_tmp4AE.tag=10,((_tmp4AE.f1=((_tmp4AD.tvars=_tmp306->tvs,((_tmp4AD.effect=
_tmp306->effect,((_tmp4AD.ret_typ=_tmp306->ret_type,((_tmp4AD.args=((struct Cyc_List_List*(*)(
struct _tuple7*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,
_tmp306->args),((_tmp4AD.c_varargs=_tmp306->c_varargs,((_tmp4AD.cyc_varargs=
_tmp306->cyc_varargs,((_tmp4AD.rgn_po=_tmp306->rgn_po,((_tmp4AD.attributes=0,
_tmp4AD)))))))))))))))),_tmp4AE)))),_tmp4AC))));switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL1F6:((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(
struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(
0),t,Cyc_Absyndump_dump_atts_qvar,_tmp306);break;case Cyc_Cyclone_Vc_c: _LL1F7:((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))
Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_callconv_fdqvar,
_tmp306);break;}Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp306->body,
0);Cyc_Absyndump_dump_char((int)'}');goto _LL1CD;};_LL1D2: {struct Cyc_Absyn_Aggr_d_struct*
_tmp307=(struct Cyc_Absyn_Aggr_d_struct*)_tmp302;if(_tmp307->tag != 6)goto _LL1D4;
else{_tmp308=_tmp307->f1;}}_LL1D3: Cyc_Absyndump_dumpscope(_tmp308->sc);Cyc_Absyndump_dumpaggr_kind(
_tmp308->kind);Cyc_Absyndump_dumpqvar(_tmp308->name);Cyc_Absyndump_dumptvars(
_tmp308->tvs);if(_tmp308->impl == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp308->impl))->exist_vars
!= 0){const char*_tmp4B1;const char*_tmp4B0;const char*_tmp4AF;((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp308->impl))->exist_vars,((_tmp4AF="<",
_tag_dyneither(_tmp4AF,sizeof(char),2))),((_tmp4B0=">",_tag_dyneither(_tmp4B0,
sizeof(char),2))),((_tmp4B1=",",_tag_dyneither(_tmp4B1,sizeof(char),2))));}if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp308->impl))->rgn_po != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp308->impl))->rgn_po);}Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp308->impl))->fields);{const char*_tmp4B2;Cyc_Absyndump_dump(((
_tmp4B2="}",_tag_dyneither(_tmp4B2,sizeof(char),2))));}Cyc_Absyndump_dumpatts(
_tmp308->attributes);{const char*_tmp4B3;Cyc_Absyndump_dump(((_tmp4B3=";",
_tag_dyneither(_tmp4B3,sizeof(char),2))));};}goto _LL1CD;_LL1D4: {struct Cyc_Absyn_Datatype_d_struct*
_tmp309=(struct Cyc_Absyn_Datatype_d_struct*)_tmp302;if(_tmp309->tag != 7)goto
_LL1D6;else{_tmp30A=_tmp309->f1;_tmp30B=*_tmp30A;_tmp30C=_tmp30B.sc;_tmp30D=
_tmp30B.name;_tmp30E=_tmp30B.tvs;_tmp30F=_tmp30B.fields;_tmp310=_tmp30B.is_extensible;}}
_LL1D5: Cyc_Absyndump_dumpscope(_tmp30C);if(_tmp310){const char*_tmp4B4;Cyc_Absyndump_dump(((
_tmp4B4="@extensible ",_tag_dyneither(_tmp4B4,sizeof(char),13))));}{const char*
_tmp4B5;Cyc_Absyndump_dump(((_tmp4B5="datatype ",_tag_dyneither(_tmp4B5,sizeof(
char),10))));}Cyc_Absyndump_dumpqvar(_tmp30D);Cyc_Absyndump_dumptvars(_tmp30E);
if(_tmp30F == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpdatatypefields((struct Cyc_List_List*)_tmp30F->v);{const char*
_tmp4B6;Cyc_Absyndump_dump_nospace(((_tmp4B6="};",_tag_dyneither(_tmp4B6,sizeof(
char),3))));};}goto _LL1CD;_LL1D6: {struct Cyc_Absyn_Enum_d_struct*_tmp311=(struct
Cyc_Absyn_Enum_d_struct*)_tmp302;if(_tmp311->tag != 8)goto _LL1D8;else{_tmp312=
_tmp311->f1;_tmp313=*_tmp312;_tmp314=_tmp313.sc;_tmp315=_tmp313.name;_tmp316=
_tmp313.fields;}}_LL1D7: Cyc_Absyndump_dumpscope(_tmp314);{const char*_tmp4B7;Cyc_Absyndump_dump(((
_tmp4B7="enum ",_tag_dyneither(_tmp4B7,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(
_tmp315);if(_tmp316 == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp316->v);{const
char*_tmp4B8;Cyc_Absyndump_dump_nospace(((_tmp4B8="};",_tag_dyneither(_tmp4B8,
sizeof(char),3))));};}return;_LL1D8: {struct Cyc_Absyn_Let_d_struct*_tmp317=(
struct Cyc_Absyn_Let_d_struct*)_tmp302;if(_tmp317->tag != 2)goto _LL1DA;else{
_tmp318=_tmp317->f1;_tmp319=_tmp317->f3;}}_LL1D9:{const char*_tmp4B9;Cyc_Absyndump_dump(((
_tmp4B9="let",_tag_dyneither(_tmp4B9,sizeof(char),4))));}Cyc_Absyndump_dumppat(
_tmp318);Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(_tmp319);Cyc_Absyndump_dump_semi();
goto _LL1CD;_LL1DA: {struct Cyc_Absyn_Letv_d_struct*_tmp31A=(struct Cyc_Absyn_Letv_d_struct*)
_tmp302;if(_tmp31A->tag != 3)goto _LL1DC;else{_tmp31B=_tmp31A->f1;}}_LL1DB:{const
char*_tmp4BA;Cyc_Absyndump_dump(((_tmp4BA="let ",_tag_dyneither(_tmp4BA,sizeof(
char),5))));}Cyc_Absyndump_dumpids(_tmp31B);Cyc_Absyndump_dump_semi();goto _LL1CD;
_LL1DC: {struct Cyc_Absyn_Region_d_struct*_tmp31C=(struct Cyc_Absyn_Region_d_struct*)
_tmp302;if(_tmp31C->tag != 4)goto _LL1DE;else{_tmp31D=_tmp31C->f1;_tmp31E=_tmp31C->f2;
_tmp31F=_tmp31C->f3;_tmp320=_tmp31C->f4;}}_LL1DD:{const char*_tmp4BB;Cyc_Absyndump_dump(((
_tmp4BB="region",_tag_dyneither(_tmp4BB,sizeof(char),7))));}{const char*_tmp4BC;
Cyc_Absyndump_dump(((_tmp4BC="<",_tag_dyneither(_tmp4BC,sizeof(char),2))));}Cyc_Absyndump_dumptvar(
_tmp31D);{const char*_tmp4BD;Cyc_Absyndump_dump(((_tmp4BD="> ",_tag_dyneither(
_tmp4BD,sizeof(char),3))));}Cyc_Absyndump_dumpqvar(_tmp31E->name);if(_tmp31F){
const char*_tmp4BE;Cyc_Absyndump_dump(((_tmp4BE="@resetable",_tag_dyneither(
_tmp4BE,sizeof(char),11))));}if(_tmp320 != 0){{const char*_tmp4BF;Cyc_Absyndump_dump(((
_tmp4BF=" = open(",_tag_dyneither(_tmp4BF,sizeof(char),9))));}Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_tmp320);{const char*_tmp4C0;Cyc_Absyndump_dump(((_tmp4C0=")",
_tag_dyneither(_tmp4C0,sizeof(char),2))));};}Cyc_Absyndump_dump_semi();goto
_LL1CD;_LL1DE: {struct Cyc_Absyn_Alias_d_struct*_tmp321=(struct Cyc_Absyn_Alias_d_struct*)
_tmp302;if(_tmp321->tag != 5)goto _LL1E0;else{_tmp322=_tmp321->f1;_tmp323=_tmp321->f2;
_tmp324=_tmp321->f3;}}_LL1DF:{const char*_tmp4C1;Cyc_Absyndump_dump(((_tmp4C1="alias ",
_tag_dyneither(_tmp4C1,sizeof(char),7))));}{const char*_tmp4C2;Cyc_Absyndump_dump(((
_tmp4C2="<",_tag_dyneither(_tmp4C2,sizeof(char),2))));}Cyc_Absyndump_dumptvar(
_tmp323);{const char*_tmp4C3;Cyc_Absyndump_dump(((_tmp4C3=">",_tag_dyneither(
_tmp4C3,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmp324->name);{const char*
_tmp4C4;Cyc_Absyndump_dump(((_tmp4C4=" = ",_tag_dyneither(_tmp4C4,sizeof(char),4))));}
Cyc_Absyndump_dumpexp(_tmp322);Cyc_Absyndump_dump_semi();goto _LL1CD;_LL1E0: {
struct Cyc_Absyn_Typedef_d_struct*_tmp325=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp302;if(_tmp325->tag != 9)goto _LL1E2;else{_tmp326=_tmp325->f1;}}_LL1E1: if(!Cyc_Absyndump_expand_typedefs
 || _tmp326->defn != 0  && Cyc_Absynpp_is_anon_aggrtype((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp326->defn))->v)){{const char*_tmp4C5;Cyc_Absyndump_dump(((_tmp4C5="typedef",
_tag_dyneither(_tmp4C5,sizeof(char),8))));}{void*t;if(_tmp326->defn == 0)t=Cyc_Absyn_new_evar(
_tmp326->kind,0);else{t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp326->defn))->v;}((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(_tmp326->tq,t,Cyc_Absyndump_dumptypedefname,
_tmp326);Cyc_Absyndump_dumpatts(_tmp326->atts);Cyc_Absyndump_dump_semi();};}goto
_LL1CD;_LL1E2: {struct Cyc_Absyn_Namespace_d_struct*_tmp327=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp302;if(_tmp327->tag != 10)goto _LL1E4;else{_tmp328=_tmp327->f1;_tmp329=_tmp327->f2;}}
_LL1E3:{const char*_tmp4C6;Cyc_Absyndump_dump(((_tmp4C6="namespace",
_tag_dyneither(_tmp4C6,sizeof(char),10))));}Cyc_Absyndump_dump_str(_tmp328);Cyc_Absyndump_dump_char((
int)'{');for(0;_tmp329 != 0;_tmp329=_tmp329->tl){Cyc_Absyndump_dumpdecl((struct
Cyc_Absyn_Decl*)_tmp329->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1CD;
_LL1E4: {struct Cyc_Absyn_Using_d_struct*_tmp32A=(struct Cyc_Absyn_Using_d_struct*)
_tmp302;if(_tmp32A->tag != 11)goto _LL1E6;else{_tmp32B=_tmp32A->f1;_tmp32C=_tmp32A->f2;}}
_LL1E5:{const char*_tmp4C7;Cyc_Absyndump_dump(((_tmp4C7="using",_tag_dyneither(
_tmp4C7,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(_tmp32B);Cyc_Absyndump_dump_char((
int)'{');for(0;_tmp32C != 0;_tmp32C=_tmp32C->tl){Cyc_Absyndump_dumpdecl((struct
Cyc_Absyn_Decl*)_tmp32C->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1CD;
_LL1E6: {struct Cyc_Absyn_ExternC_d_struct*_tmp32D=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp302;if(_tmp32D->tag != 12)goto _LL1E8;else{_tmp32E=_tmp32D->f1;}}_LL1E7:{const
char*_tmp4C8;Cyc_Absyndump_dump(((_tmp4C8="extern \"C\" {",_tag_dyneither(
_tmp4C8,sizeof(char),13))));}for(0;_tmp32E != 0;_tmp32E=_tmp32E->tl){Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)_tmp32E->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1CD;
_LL1E8: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp32F=(struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp302;if(_tmp32F->tag != 13)goto _LL1EA;else{_tmp330=_tmp32F->f1;_tmp331=_tmp32F->f2;}}
_LL1E9:{const char*_tmp4C9;Cyc_Absyndump_dump(((_tmp4C9="extern \"C include\" {",
_tag_dyneither(_tmp4C9,sizeof(char),21))));}for(0;_tmp330 != 0;_tmp330=_tmp330->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp330->hd);}Cyc_Absyndump_dump_char((
int)'}');if(_tmp331 != 0){{const char*_tmp4CA;Cyc_Absyndump_dump(((_tmp4CA=" export {",
_tag_dyneither(_tmp4CA,sizeof(char),10))));}for(0;_tmp331 != 0;_tmp331=_tmp331->tl){
struct _tuple0*_tmp357;struct _tuple19 _tmp356=*((struct _tuple19*)_tmp331->hd);
_tmp357=_tmp356.f2;Cyc_Absyndump_dumpqvar(_tmp357);if(_tmp331->tl != 0)Cyc_Absyndump_dump_char((
int)',');}{const char*_tmp4CB;Cyc_Absyndump_dump(((_tmp4CB="}",_tag_dyneither(
_tmp4CB,sizeof(char),2))));};}goto _LL1CD;_LL1EA: {struct Cyc_Absyn_Porton_d_struct*
_tmp332=(struct Cyc_Absyn_Porton_d_struct*)_tmp302;if(_tmp332->tag != 14)goto
_LL1EC;}_LL1EB:{const char*_tmp4CC;Cyc_Absyndump_dump(((_tmp4CC=" __cyclone_port_on__; ",
_tag_dyneither(_tmp4CC,sizeof(char),23))));}goto _LL1CD;_LL1EC: {struct Cyc_Absyn_Portoff_d_struct*
_tmp333=(struct Cyc_Absyn_Portoff_d_struct*)_tmp302;if(_tmp333->tag != 15)goto
_LL1CD;}_LL1ED:{const char*_tmp4CD;Cyc_Absyndump_dump(((_tmp4CD=" __cyclone_port_off__; ",
_tag_dyneither(_tmp4CD,sizeof(char),24))));}goto _LL1CD;_LL1CD:;}static void Cyc_Absyndump_dump_upperbound(
struct Cyc_Absyn_Exp*e);static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*
e){struct _tuple12 pr=Cyc_Evexp_eval_const_uint_exp(e);if(pr.f1 != 1  || !pr.f2){Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}void Cyc_Absyndump_dumptms(
struct Cyc_List_List*tms,void(*f)(void*),void*a);void Cyc_Absyndump_dumptms(struct
Cyc_List_List*tms,void(*f)(void*),void*a){if(tms == 0){f(a);return;}{void*_tmp35B=(
void*)tms->hd;struct Cyc_Absyn_PtrAtts _tmp35D;void*_tmp35E;union Cyc_Absyn_Constraint*
_tmp35F;union Cyc_Absyn_Constraint*_tmp360;union Cyc_Absyn_Constraint*_tmp361;
struct Cyc_Absyn_Tqual _tmp362;_LL1FA: {struct Cyc_Absyn_Pointer_mod_struct*_tmp35C=(
struct Cyc_Absyn_Pointer_mod_struct*)_tmp35B;if(_tmp35C->tag != 2)goto _LL1FC;else{
_tmp35D=_tmp35C->f1;_tmp35E=_tmp35D.rgn;_tmp35F=_tmp35D.nullable;_tmp360=_tmp35D.bounds;
_tmp361=_tmp35D.zero_term;_tmp362=_tmp35C->f2;}}_LL1FB:{void*_tmp363=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp360);struct Cyc_Absyn_Exp*_tmp366;_LL1FF: {struct Cyc_Absyn_DynEither_b_struct*
_tmp364=(struct Cyc_Absyn_DynEither_b_struct*)_tmp363;if(_tmp364->tag != 0)goto
_LL201;}_LL200: Cyc_Absyndump_dump_char((int)'?');goto _LL1FE;_LL201: {struct Cyc_Absyn_Upper_b_struct*
_tmp365=(struct Cyc_Absyn_Upper_b_struct*)_tmp363;if(_tmp365->tag != 1)goto _LL1FE;
else{_tmp366=_tmp365->f1;}}_LL202: Cyc_Absyndump_dump_char((int)(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp35F)?'*':'@'));Cyc_Absyndump_dump_upperbound(
_tmp366);goto _LL1FE;_LL1FE:;}if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp361)){const char*_tmp4CE;Cyc_Absyndump_dump(((_tmp4CE="@zeroterm",
_tag_dyneither(_tmp4CE,sizeof(char),10))));}{void*_tmp368=Cyc_Tcutil_compress(
_tmp35E);struct Cyc_Absyn_Tvar*_tmp36C;struct Cyc_Core_Opt*_tmp36E;_LL204: {struct
Cyc_Absyn_HeapRgn_struct*_tmp369=(struct Cyc_Absyn_HeapRgn_struct*)_tmp368;if(
_tmp369->tag != 21)goto _LL206;}_LL205: goto _LL203;_LL206: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp36A=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp368;if(_tmp36A->tag != 22)goto
_LL208;}_LL207:{const char*_tmp4CF;Cyc_Absyndump_dump(((_tmp4CF="`U",
_tag_dyneither(_tmp4CF,sizeof(char),3))));}goto _LL203;_LL208: {struct Cyc_Absyn_VarType_struct*
_tmp36B=(struct Cyc_Absyn_VarType_struct*)_tmp368;if(_tmp36B->tag != 2)goto _LL20A;
else{_tmp36C=_tmp36B->f1;}}_LL209: Cyc_Absyndump_dump_str(_tmp36C->name);goto
_LL203;_LL20A: {struct Cyc_Absyn_Evar_struct*_tmp36D=(struct Cyc_Absyn_Evar_struct*)
_tmp368;if(_tmp36D->tag != 1)goto _LL20C;else{_tmp36E=_tmp36D->f2;if(_tmp36E != 0)
goto _LL20C;}}_LL20B: Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(_tmp35E));goto
_LL203;_LL20C:;_LL20D: {const char*_tmp4D2;void*_tmp4D1;(_tmp4D1=0,Cyc_Tcutil_impos(((
_tmp4D2="dumptms: bad rgn type in Pointer_mod",_tag_dyneither(_tmp4D2,sizeof(
char),37))),_tag_dyneither(_tmp4D1,sizeof(void*),0)));}_LL203:;}Cyc_Absyndump_dumptq(
_tmp362);Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL1FC:;_LL1FD: {int
next_is_pointer=0;if(tms->tl != 0){void*_tmp372=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;_LL20F: {struct Cyc_Absyn_Pointer_mod_struct*_tmp373=(
struct Cyc_Absyn_Pointer_mod_struct*)_tmp372;if(_tmp373->tag != 2)goto _LL211;}
_LL210: next_is_pointer=1;goto _LL20E;_LL211:;_LL212: goto _LL20E;_LL20E:;}if(
next_is_pointer)Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptms(tms->tl,
f,a);if(next_is_pointer)Cyc_Absyndump_dump_char((int)')');{void*_tmp374=(void*)
tms->hd;union Cyc_Absyn_Constraint*_tmp376;struct Cyc_Absyn_Exp*_tmp378;union Cyc_Absyn_Constraint*
_tmp379;void*_tmp37B;struct Cyc_List_List*_tmp37D;int _tmp37E;struct Cyc_Absyn_VarargInfo*
_tmp37F;struct Cyc_Core_Opt*_tmp380;struct Cyc_List_List*_tmp381;void*_tmp383;
struct Cyc_List_List*_tmp385;struct Cyc_Position_Segment*_tmp386;struct Cyc_List_List*
_tmp388;struct Cyc_Position_Segment*_tmp389;int _tmp38A;struct Cyc_List_List*
_tmp38C;_LL214: {struct Cyc_Absyn_Carray_mod_struct*_tmp375=(struct Cyc_Absyn_Carray_mod_struct*)
_tmp374;if(_tmp375->tag != 0)goto _LL216;else{_tmp376=_tmp375->f1;}}_LL215:{const
char*_tmp4D3;Cyc_Absyndump_dump(((_tmp4D3="[]",_tag_dyneither(_tmp4D3,sizeof(
char),3))));}if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp376)){const char*_tmp4D4;Cyc_Absyndump_dump(((_tmp4D4="@zeroterm",
_tag_dyneither(_tmp4D4,sizeof(char),10))));}goto _LL213;_LL216: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmp377=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmp374;if(_tmp377->tag != 1)goto
_LL218;else{_tmp378=_tmp377->f1;_tmp379=_tmp377->f2;}}_LL217: Cyc_Absyndump_dump_char((
int)'[');Cyc_Absyndump_dumpexp(_tmp378);Cyc_Absyndump_dump_char((int)']');if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp379)){const
char*_tmp4D5;Cyc_Absyndump_dump(((_tmp4D5="@zeroterm",_tag_dyneither(_tmp4D5,
sizeof(char),10))));}goto _LL213;_LL218: {struct Cyc_Absyn_Function_mod_struct*
_tmp37A=(struct Cyc_Absyn_Function_mod_struct*)_tmp374;if(_tmp37A->tag != 3)goto
_LL21A;else{_tmp37B=(void*)_tmp37A->f1;{struct Cyc_Absyn_WithTypes_struct*_tmp37C=(
struct Cyc_Absyn_WithTypes_struct*)_tmp37B;if(_tmp37C->tag != 1)goto _LL21A;else{
_tmp37D=_tmp37C->f1;_tmp37E=_tmp37C->f2;_tmp37F=_tmp37C->f3;_tmp380=_tmp37C->f4;
_tmp381=_tmp37C->f5;}};}}_LL219: Cyc_Absyndump_dumpfunargs(_tmp37D,_tmp37E,
_tmp37F,_tmp380,_tmp381);goto _LL213;_LL21A: {struct Cyc_Absyn_Function_mod_struct*
_tmp382=(struct Cyc_Absyn_Function_mod_struct*)_tmp374;if(_tmp382->tag != 3)goto
_LL21C;else{_tmp383=(void*)_tmp382->f1;{struct Cyc_Absyn_NoTypes_struct*_tmp384=(
struct Cyc_Absyn_NoTypes_struct*)_tmp383;if(_tmp384->tag != 0)goto _LL21C;else{
_tmp385=_tmp384->f1;_tmp386=_tmp384->f2;}};}}_LL21B:{const char*_tmp4D8;const char*
_tmp4D7;const char*_tmp4D6;((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dump_str,_tmp385,((_tmp4D6="(",_tag_dyneither(_tmp4D6,sizeof(char),
2))),((_tmp4D7=")",_tag_dyneither(_tmp4D7,sizeof(char),2))),((_tmp4D8=",",
_tag_dyneither(_tmp4D8,sizeof(char),2))));}goto _LL213;_LL21C: {struct Cyc_Absyn_TypeParams_mod_struct*
_tmp387=(struct Cyc_Absyn_TypeParams_mod_struct*)_tmp374;if(_tmp387->tag != 4)goto
_LL21E;else{_tmp388=_tmp387->f1;_tmp389=_tmp387->f2;_tmp38A=_tmp387->f3;}}_LL21D:
if(_tmp38A)Cyc_Absyndump_dumpkindedtvars(_tmp388);else{Cyc_Absyndump_dumptvars(
_tmp388);}goto _LL213;_LL21E: {struct Cyc_Absyn_Attributes_mod_struct*_tmp38B=(
struct Cyc_Absyn_Attributes_mod_struct*)_tmp374;if(_tmp38B->tag != 5)goto _LL220;
else{_tmp38C=_tmp38B->f2;}}_LL21F: Cyc_Absyndump_dumpatts(_tmp38C);goto _LL213;
_LL220: {struct Cyc_Absyn_Pointer_mod_struct*_tmp38D=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp374;if(_tmp38D->tag != 2)goto _LL213;}_LL221: {const char*_tmp4DB;void*_tmp4DA;(
_tmp4DA=0,Cyc_Tcutil_impos(((_tmp4DB="dumptms",_tag_dyneither(_tmp4DB,sizeof(
char),8))),_tag_dyneither(_tmp4DA,sizeof(void*),0)));}_LL213:;}return;}_LL1F9:;};}
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a);
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
struct _RegionHandle _tmp396=_new_region("temp");struct _RegionHandle*temp=& _tmp396;
_push_region(temp);{struct Cyc_Absyn_Tqual _tmp398;void*_tmp399;struct Cyc_List_List*
_tmp39A;struct _tuple11 _tmp397=Cyc_Absynpp_to_tms(temp,tq,t);_tmp398=_tmp397.f1;
_tmp399=_tmp397.f2;_tmp39A=_tmp397.f3;Cyc_Absyndump_dumptq(_tmp398);Cyc_Absyndump_dumpntyp(
_tmp399);Cyc_Absyndump_dumptms(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp39A),f,a);};_pop_region(temp);}void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){Cyc_Absyndump_pos=0;*Cyc_Absyndump_dump_file=
f;for(0;tdl != 0;tdl=tdl->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}{
const char*_tmp4DE;void*_tmp4DD;(_tmp4DD=0,Cyc_fprintf(f,((_tmp4DE="\n",
_tag_dyneither(_tmp4DE,sizeof(char),2))),_tag_dyneither(_tmp4DD,sizeof(void*),0)));};}
