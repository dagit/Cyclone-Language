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
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct
_dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _dyneither_ptr Cyc_Core_new_string(
unsigned int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};void Cyc_List_iter(void(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};int Cyc_List_exists(
int(*pred)(void*),struct Cyc_List_List*x);int Cyc_List_list_cmp(int(*cmp)(void*,
void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);int Cyc_List_list_prefix(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);extern int
Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;void Cyc_PP_file_of_doc(
struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f);struct _dyneither_ptr Cyc_PP_string_of_doc(
struct Cyc_PP_Doc*d,int w);struct Cyc_PP_Doc*Cyc_PP_nil_doc();struct Cyc_PP_Doc*Cyc_PP_blank_doc();
struct Cyc_PP_Doc*Cyc_PP_line_doc();struct Cyc_PP_Doc*Cyc_PP_text(struct
_dyneither_ptr s);struct Cyc_PP_Doc*Cyc_PP_textptr(struct _dyneither_ptr*p);struct
Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,int w);struct Cyc_PP_Doc*Cyc_PP_nest(
int k,struct Cyc_PP_Doc*d);struct Cyc_PP_Doc*Cyc_PP_cat(struct _dyneither_ptr);
struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct Cyc_List_List*l);
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr
sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,
struct Cyc_List_List*l0);struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(
void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_group(
struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,
struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,
struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);struct
Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int
line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct Cyc_Position_Lex_struct{int tag;};
struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{int tag;}
;struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
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
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;union
Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
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
struct Cyc_Buffer_t;unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strptrcmp(
struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_str_sepstr(
struct Cyc_List_List*,struct _dyneither_ptr);struct _tuple10{unsigned int f1;int f2;}
;struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{
char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,
struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn,struct Cyc_Position_Segment*loc);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,
struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int Cyc_RgnOrder_is_region_resetable(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);int Cyc_RgnOrder_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);int Cyc_RgnOrder_satisfies_constraints(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,
int do_pin);int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*
eff1,void*eff2);void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);
struct Cyc_Tcenv_CList{void*hd;struct Cyc_Tcenv_CList*tl;};struct Cyc_Tcenv_VarRes_struct{
int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Tcenv_DatatypeRes_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;
struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct
Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_NotLoop_j_struct{int tag;};struct Cyc_Tcenv_CaseEnd_j_struct{
int tag;};struct Cyc_Tcenv_FnEnd_j_struct{int tag;};struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_compress(
void*t);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*);struct Cyc_Absynpp_Params{int expand_typedefs: 1;int
qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};extern int Cyc_Absynpp_print_for_cycdoc;
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params
Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_tc_params_r;void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,
struct Cyc___cycFILE*f);struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*
d);struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_typ2cstring(
void*);struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct _dyneither_ptr
Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct _dyneither_ptr Cyc_Absynpp_prim2string(
enum Cyc_Absyn_Primop p);struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*
p);struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc);int Cyc_Absynpp_is_anon_aggrtype(
void*t);extern struct _dyneither_ptr Cyc_Absynpp_cyc_string;extern struct
_dyneither_ptr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);struct _dyneither_ptr Cyc_Absynpp_string_escape(
struct _dyneither_ptr);struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop
p);int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple11{struct
_dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct _tuple7*Cyc_Absynpp_arg_mk_opt(
struct _tuple11*arg);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*
f3;};struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual
tq,void*t);enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c
 = 1};extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple13{
struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(
struct _tuple13*dp);struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*
cs);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope);
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*,int expstmt);struct
Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int
inprec,struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct
_tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*);struct
Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst);struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
enum Cyc_Absyn_Primop);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum 
Cyc_Absyn_Primop p,struct Cyc_List_List*es);struct _tuple14{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de);
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*
dopt);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct
Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*
vd);static int Cyc_Absynpp_expand_typedefs;static int Cyc_Absynpp_qvar_to_Cids;
static char _tmp0[4]="Cyc";struct _dyneither_ptr Cyc_Absynpp_cyc_string={_tmp0,_tmp0,
_tmp0 + 4};struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;
static int Cyc_Absynpp_add_cyc_prefix;static int Cyc_Absynpp_to_VC;static int Cyc_Absynpp_decls_first;
static int Cyc_Absynpp_rewrite_temp_tvars;static int Cyc_Absynpp_print_all_tvars;
static int Cyc_Absynpp_print_all_kinds;static int Cyc_Absynpp_print_all_effects;
static int Cyc_Absynpp_print_using_stmts;static int Cyc_Absynpp_print_externC_stmts;
static int Cyc_Absynpp_print_full_evars;static int Cyc_Absynpp_generate_line_directives;
static int Cyc_Absynpp_use_curr_namespace;static int Cyc_Absynpp_print_zeroterm;
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;int Cyc_Absynpp_print_for_cycdoc=
0;struct Cyc_Absynpp_Params;void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*
fs);void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs){Cyc_Absynpp_expand_typedefs=
fs->expand_typedefs;Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
fs->add_cyc_prefix;Cyc_Absynpp_to_VC=fs->to_VC;Cyc_Absynpp_decls_first=fs->decls_first;
Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;Cyc_Absynpp_print_all_tvars=
fs->print_all_tvars;Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;Cyc_Absynpp_print_all_effects=
fs->print_all_effects;Cyc_Absynpp_print_using_stmts=fs->print_using_stmts;Cyc_Absynpp_print_externC_stmts=
fs->print_externC_stmts;Cyc_Absynpp_print_full_evars=fs->print_full_evars;Cyc_Absynpp_print_zeroterm=
fs->print_zeroterm;Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;Cyc_Absynpp_curr_namespace=
fs->curr_namespace;}struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,
1,0,0,0,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,0,
0,0,1,0,0,1,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,
0,1,0,0,0,0,0,0,0,0,0,0,0};struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,
0,0,0,0,0,0,0,1,1,0,1,0,0,0};static void Cyc_Absynpp_curr_namespace_add(struct
_dyneither_ptr*v);static void Cyc_Absynpp_curr_namespace_add(struct _dyneither_ptr*
v){struct Cyc_List_List*_tmp5DC;Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,((
_tmp5DC=_cycalloc(sizeof(*_tmp5DC)),((_tmp5DC->hd=v,((_tmp5DC->tl=0,_tmp5DC)))))));}
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l);static void Cyc_Absynpp_suppr_last(
struct Cyc_List_List**l){if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)*l=0;
else{Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)_check_null(*l))->tl);}}
static void Cyc_Absynpp_curr_namespace_drop();static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c);struct _dyneither_ptr Cyc_Absynpp_char_escape(
char c){switch(c){case '\a': _LL0: {const char*_tmp5DD;return(_tmp5DD="\\a",
_tag_dyneither(_tmp5DD,sizeof(char),3));}case '\b': _LL1: {const char*_tmp5DE;
return(_tmp5DE="\\b",_tag_dyneither(_tmp5DE,sizeof(char),3));}case '\f': _LL2: {
const char*_tmp5DF;return(_tmp5DF="\\f",_tag_dyneither(_tmp5DF,sizeof(char),3));}
case '\n': _LL3: {const char*_tmp5E0;return(_tmp5E0="\\n",_tag_dyneither(_tmp5E0,
sizeof(char),3));}case '\r': _LL4: {const char*_tmp5E1;return(_tmp5E1="\\r",
_tag_dyneither(_tmp5E1,sizeof(char),3));}case '\t': _LL5: {const char*_tmp5E2;
return(_tmp5E2="\\t",_tag_dyneither(_tmp5E2,sizeof(char),3));}case '\v': _LL6: {
const char*_tmp5E3;return(_tmp5E3="\\v",_tag_dyneither(_tmp5E3,sizeof(char),3));}
case '\\': _LL7: {const char*_tmp5E4;return(_tmp5E4="\\\\",_tag_dyneither(_tmp5E4,
sizeof(char),3));}case '"': _LL8: {const char*_tmp5E5;return(_tmp5E5="\"",
_tag_dyneither(_tmp5E5,sizeof(char),2));}case '\'': _LL9: {const char*_tmp5E6;
return(_tmp5E6="\\'",_tag_dyneither(_tmp5E6,sizeof(char),3));}default: _LLA: if(c
>= ' '  && c <= '~'){struct _dyneither_ptr _tmpC=Cyc_Core_new_string(2);{char _tmp5E9;
char _tmp5E8;struct _dyneither_ptr _tmp5E7;(_tmp5E7=_dyneither_ptr_plus(_tmpC,
sizeof(char),0),((_tmp5E8=*((char*)_check_dyneither_subscript(_tmp5E7,sizeof(
char),0)),((_tmp5E9=c,((_get_dyneither_size(_tmp5E7,sizeof(char))== 1  && (
_tmp5E8 == '\000'  && _tmp5E9 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5E7.curr)=
_tmp5E9)))))));}return(struct _dyneither_ptr)_tmpC;}else{struct _dyneither_ptr
_tmp10=Cyc_Core_new_string(5);int j=0;{char _tmp5EC;char _tmp5EB;struct
_dyneither_ptr _tmp5EA;(_tmp5EA=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp5EB=*((char*)_check_dyneither_subscript(_tmp5EA,sizeof(char),0)),((_tmp5EC='\\',((
_get_dyneither_size(_tmp5EA,sizeof(char))== 1  && (_tmp5EB == '\000'  && _tmp5EC != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5EA.curr)=_tmp5EC)))))));}{char _tmp5EF;char
_tmp5EE;struct _dyneither_ptr _tmp5ED;(_tmp5ED=_dyneither_ptr_plus(_tmp10,sizeof(
char),j ++),((_tmp5EE=*((char*)_check_dyneither_subscript(_tmp5ED,sizeof(char),0)),((
_tmp5EF=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp5ED,
sizeof(char))== 1  && (_tmp5EE == '\000'  && _tmp5EF != '\000')?_throw_arraybounds():
1,*((char*)_tmp5ED.curr)=_tmp5EF)))))));}{char _tmp5F2;char _tmp5F1;struct
_dyneither_ptr _tmp5F0;(_tmp5F0=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp5F1=*((char*)_check_dyneither_subscript(_tmp5F0,sizeof(char),0)),((_tmp5F2=(
char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp5F0,sizeof(char))== 1  && (
_tmp5F1 == '\000'  && _tmp5F2 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F0.curr)=
_tmp5F2)))))));}{char _tmp5F5;char _tmp5F4;struct _dyneither_ptr _tmp5F3;(_tmp5F3=
_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5F4=*((char*)
_check_dyneither_subscript(_tmp5F3,sizeof(char),0)),((_tmp5F5=(char)('0' + (c & 7)),((
_get_dyneither_size(_tmp5F3,sizeof(char))== 1  && (_tmp5F4 == '\000'  && _tmp5F5 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5F3.curr)=_tmp5F5)))))));}return(struct
_dyneither_ptr)_tmp10;}}}static int Cyc_Absynpp_special(struct _dyneither_ptr s);
static int Cyc_Absynpp_special(struct _dyneither_ptr s){int sz=(int)(
_get_dyneither_size(s,sizeof(char))- 1);{int i=0;for(0;i < sz;++ i){char c=*((const
char*)_check_dyneither_subscript(s,sizeof(char),i));if(((c <= ' '  || c >= '~') || 
c == '"') || c == '\\')return 1;}}return 0;}struct _dyneither_ptr Cyc_Absynpp_string_escape(
struct _dyneither_ptr s);struct _dyneither_ptr Cyc_Absynpp_string_escape(struct
_dyneither_ptr s){if(!Cyc_Absynpp_special(s))return s;{int n=(int)(
_get_dyneither_size(s,sizeof(char))- 1);if(n > 0  && *((const char*)
_check_dyneither_subscript(s,sizeof(char),n))== '\000')-- n;{int len=0;{int i=0;
for(0;i <= n;++ i){char _tmp1D=*((const char*)_check_dyneither_subscript(s,sizeof(
char),i));_LLD: if(_tmp1D != '\a')goto _LLF;_LLE: goto _LL10;_LLF: if(_tmp1D != '\b')
goto _LL11;_LL10: goto _LL12;_LL11: if(_tmp1D != '\f')goto _LL13;_LL12: goto _LL14;_LL13:
if(_tmp1D != '\n')goto _LL15;_LL14: goto _LL16;_LL15: if(_tmp1D != '\r')goto _LL17;
_LL16: goto _LL18;_LL17: if(_tmp1D != '\t')goto _LL19;_LL18: goto _LL1A;_LL19: if(_tmp1D
!= '\v')goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp1D != '\\')goto _LL1D;_LL1C: goto
_LL1E;_LL1D: if(_tmp1D != '"')goto _LL1F;_LL1E: len +=2;goto _LLC;_LL1F:;_LL20: if(
_tmp1D >= ' '  && _tmp1D <= '~')++ len;else{len +=4;}goto _LLC;_LLC:;}}{struct
_dyneither_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));int j=0;{int i=0;for(0;
i <= n;++ i){char _tmp1E=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:{char _tmp5F8;char _tmp5F7;struct
_dyneither_ptr _tmp5F6;(_tmp5F6=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5F7=*((
char*)_check_dyneither_subscript(_tmp5F6,sizeof(char),0)),((_tmp5F8='\\',((
_get_dyneither_size(_tmp5F6,sizeof(char))== 1  && (_tmp5F7 == '\000'  && _tmp5F8 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5F6.curr)=_tmp5F8)))))));}{char _tmp5FB;char
_tmp5FA;struct _dyneither_ptr _tmp5F9;(_tmp5F9=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp5FA=*((char*)_check_dyneither_subscript(_tmp5F9,sizeof(char),0)),((
_tmp5FB='a',((_get_dyneither_size(_tmp5F9,sizeof(char))== 1  && (_tmp5FA == '\000'
 && _tmp5FB != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F9.curr)=_tmp5FB)))))));}
goto _LL21;_LL24: if(_tmp1E != '\b')goto _LL26;_LL25:{char _tmp5FE;char _tmp5FD;struct
_dyneither_ptr _tmp5FC;(_tmp5FC=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5FD=*((
char*)_check_dyneither_subscript(_tmp5FC,sizeof(char),0)),((_tmp5FE='\\',((
_get_dyneither_size(_tmp5FC,sizeof(char))== 1  && (_tmp5FD == '\000'  && _tmp5FE != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5FC.curr)=_tmp5FE)))))));}{char _tmp601;char
_tmp600;struct _dyneither_ptr _tmp5FF;(_tmp5FF=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp600=*((char*)_check_dyneither_subscript(_tmp5FF,sizeof(char),0)),((
_tmp601='b',((_get_dyneither_size(_tmp5FF,sizeof(char))== 1  && (_tmp600 == '\000'
 && _tmp601 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5FF.curr)=_tmp601)))))));}
goto _LL21;_LL26: if(_tmp1E != '\f')goto _LL28;_LL27:{char _tmp604;char _tmp603;struct
_dyneither_ptr _tmp602;(_tmp602=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp603=*((
char*)_check_dyneither_subscript(_tmp602,sizeof(char),0)),((_tmp604='\\',((
_get_dyneither_size(_tmp602,sizeof(char))== 1  && (_tmp603 == '\000'  && _tmp604 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp602.curr)=_tmp604)))))));}{char _tmp607;char
_tmp606;struct _dyneither_ptr _tmp605;(_tmp605=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp606=*((char*)_check_dyneither_subscript(_tmp605,sizeof(char),0)),((
_tmp607='f',((_get_dyneither_size(_tmp605,sizeof(char))== 1  && (_tmp606 == '\000'
 && _tmp607 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp605.curr)=_tmp607)))))));}
goto _LL21;_LL28: if(_tmp1E != '\n')goto _LL2A;_LL29:{char _tmp60A;char _tmp609;struct
_dyneither_ptr _tmp608;(_tmp608=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp609=*((
char*)_check_dyneither_subscript(_tmp608,sizeof(char),0)),((_tmp60A='\\',((
_get_dyneither_size(_tmp608,sizeof(char))== 1  && (_tmp609 == '\000'  && _tmp60A != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp608.curr)=_tmp60A)))))));}{char _tmp60D;char
_tmp60C;struct _dyneither_ptr _tmp60B;(_tmp60B=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp60C=*((char*)_check_dyneither_subscript(_tmp60B,sizeof(char),0)),((
_tmp60D='n',((_get_dyneither_size(_tmp60B,sizeof(char))== 1  && (_tmp60C == '\000'
 && _tmp60D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp60B.curr)=_tmp60D)))))));}
goto _LL21;_LL2A: if(_tmp1E != '\r')goto _LL2C;_LL2B:{char _tmp610;char _tmp60F;struct
_dyneither_ptr _tmp60E;(_tmp60E=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp60F=*((
char*)_check_dyneither_subscript(_tmp60E,sizeof(char),0)),((_tmp610='\\',((
_get_dyneither_size(_tmp60E,sizeof(char))== 1  && (_tmp60F == '\000'  && _tmp610 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp60E.curr)=_tmp610)))))));}{char _tmp613;char
_tmp612;struct _dyneither_ptr _tmp611;(_tmp611=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp612=*((char*)_check_dyneither_subscript(_tmp611,sizeof(char),0)),((
_tmp613='r',((_get_dyneither_size(_tmp611,sizeof(char))== 1  && (_tmp612 == '\000'
 && _tmp613 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp611.curr)=_tmp613)))))));}
goto _LL21;_LL2C: if(_tmp1E != '\t')goto _LL2E;_LL2D:{char _tmp616;char _tmp615;struct
_dyneither_ptr _tmp614;(_tmp614=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp615=*((
char*)_check_dyneither_subscript(_tmp614,sizeof(char),0)),((_tmp616='\\',((
_get_dyneither_size(_tmp614,sizeof(char))== 1  && (_tmp615 == '\000'  && _tmp616 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp614.curr)=_tmp616)))))));}{char _tmp619;char
_tmp618;struct _dyneither_ptr _tmp617;(_tmp617=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp618=*((char*)_check_dyneither_subscript(_tmp617,sizeof(char),0)),((
_tmp619='t',((_get_dyneither_size(_tmp617,sizeof(char))== 1  && (_tmp618 == '\000'
 && _tmp619 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp617.curr)=_tmp619)))))));}
goto _LL21;_LL2E: if(_tmp1E != '\v')goto _LL30;_LL2F:{char _tmp61C;char _tmp61B;struct
_dyneither_ptr _tmp61A;(_tmp61A=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp61B=*((
char*)_check_dyneither_subscript(_tmp61A,sizeof(char),0)),((_tmp61C='\\',((
_get_dyneither_size(_tmp61A,sizeof(char))== 1  && (_tmp61B == '\000'  && _tmp61C != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp61A.curr)=_tmp61C)))))));}{char _tmp61F;char
_tmp61E;struct _dyneither_ptr _tmp61D;(_tmp61D=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp61E=*((char*)_check_dyneither_subscript(_tmp61D,sizeof(char),0)),((
_tmp61F='v',((_get_dyneither_size(_tmp61D,sizeof(char))== 1  && (_tmp61E == '\000'
 && _tmp61F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61D.curr)=_tmp61F)))))));}
goto _LL21;_LL30: if(_tmp1E != '\\')goto _LL32;_LL31:{char _tmp622;char _tmp621;struct
_dyneither_ptr _tmp620;(_tmp620=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp621=*((
char*)_check_dyneither_subscript(_tmp620,sizeof(char),0)),((_tmp622='\\',((
_get_dyneither_size(_tmp620,sizeof(char))== 1  && (_tmp621 == '\000'  && _tmp622 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp620.curr)=_tmp622)))))));}{char _tmp625;char
_tmp624;struct _dyneither_ptr _tmp623;(_tmp623=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp624=*((char*)_check_dyneither_subscript(_tmp623,sizeof(char),0)),((
_tmp625='\\',((_get_dyneither_size(_tmp623,sizeof(char))== 1  && (_tmp624 == '\000'
 && _tmp625 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp623.curr)=_tmp625)))))));}
goto _LL21;_LL32: if(_tmp1E != '"')goto _LL34;_LL33:{char _tmp628;char _tmp627;struct
_dyneither_ptr _tmp626;(_tmp626=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp627=*((
char*)_check_dyneither_subscript(_tmp626,sizeof(char),0)),((_tmp628='\\',((
_get_dyneither_size(_tmp626,sizeof(char))== 1  && (_tmp627 == '\000'  && _tmp628 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp626.curr)=_tmp628)))))));}{char _tmp62B;char
_tmp62A;struct _dyneither_ptr _tmp629;(_tmp629=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp62A=*((char*)_check_dyneither_subscript(_tmp629,sizeof(char),0)),((
_tmp62B='"',((_get_dyneither_size(_tmp629,sizeof(char))== 1  && (_tmp62A == '\000'
 && _tmp62B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp629.curr)=_tmp62B)))))));}
goto _LL21;_LL34:;_LL35: if(_tmp1E >= ' '  && _tmp1E <= '~'){char _tmp62E;char _tmp62D;
struct _dyneither_ptr _tmp62C;(_tmp62C=_dyneither_ptr_plus(t,sizeof(char),j ++),((
_tmp62D=*((char*)_check_dyneither_subscript(_tmp62C,sizeof(char),0)),((_tmp62E=
_tmp1E,((_get_dyneither_size(_tmp62C,sizeof(char))== 1  && (_tmp62D == '\000'  && 
_tmp62E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62C.curr)=_tmp62E)))))));}
else{{char _tmp631;char _tmp630;struct _dyneither_ptr _tmp62F;(_tmp62F=
_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp630=*((char*)
_check_dyneither_subscript(_tmp62F,sizeof(char),0)),((_tmp631='\\',((
_get_dyneither_size(_tmp62F,sizeof(char))== 1  && (_tmp630 == '\000'  && _tmp631 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp62F.curr)=_tmp631)))))));}{char _tmp634;char
_tmp633;struct _dyneither_ptr _tmp632;(_tmp632=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp633=*((char*)_check_dyneither_subscript(_tmp632,sizeof(char),0)),((
_tmp634=(char)('0' + (_tmp1E >> 6 & 7)),((_get_dyneither_size(_tmp632,sizeof(char))
== 1  && (_tmp633 == '\000'  && _tmp634 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp632.curr)=_tmp634)))))));}{char _tmp637;char _tmp636;struct _dyneither_ptr
_tmp635;(_tmp635=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp636=*((char*)
_check_dyneither_subscript(_tmp635,sizeof(char),0)),((_tmp637=(char)('0' + (
_tmp1E >> 3 & 7)),((_get_dyneither_size(_tmp635,sizeof(char))== 1  && (_tmp636 == '\000'
 && _tmp637 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp635.curr)=_tmp637)))))));}{
char _tmp63A;char _tmp639;struct _dyneither_ptr _tmp638;(_tmp638=_dyneither_ptr_plus(
t,sizeof(char),j ++),((_tmp639=*((char*)_check_dyneither_subscript(_tmp638,
sizeof(char),0)),((_tmp63A=(char)('0' + (_tmp1E & 7)),((_get_dyneither_size(
_tmp638,sizeof(char))== 1  && (_tmp639 == '\000'  && _tmp63A != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp638.curr)=_tmp63A)))))));};}goto _LL21;_LL21:;}}
return(struct _dyneither_ptr)t;};};};}static char _tmp64[9]="restrict";static struct
_dyneither_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64,_tmp64 + 9};static char
_tmp65[9]="volatile";static struct _dyneither_ptr Cyc_Absynpp_volatile_string={
_tmp65,_tmp65,_tmp65 + 9};static char _tmp66[6]="const";static struct _dyneither_ptr
Cyc_Absynpp_const_str={_tmp66,_tmp66,_tmp66 + 6};static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string;static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string;static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq);struct Cyc_PP_Doc*
Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*l=0;if(tq.q_restrict){
struct Cyc_List_List*_tmp63B;l=((_tmp63B=_cycalloc(sizeof(*_tmp63B)),((_tmp63B->hd=
Cyc_Absynpp_restrict_sp,((_tmp63B->tl=l,_tmp63B))))));}if(tq.q_volatile){struct
Cyc_List_List*_tmp63C;l=((_tmp63C=_cycalloc(sizeof(*_tmp63C)),((_tmp63C->hd=Cyc_Absynpp_volatile_sp,((
_tmp63C->tl=l,_tmp63C))))));}if(tq.print_const){struct Cyc_List_List*_tmp63D;l=((
_tmp63D=_cycalloc(sizeof(*_tmp63D)),((_tmp63D->hd=Cyc_Absynpp_const_sp,((_tmp63D->tl=
l,_tmp63D))))));}{const char*_tmp640;const char*_tmp63F;const char*_tmp63E;return
Cyc_PP_egroup(((_tmp63E="",_tag_dyneither(_tmp63E,sizeof(char),1))),((_tmp63F=" ",
_tag_dyneither(_tmp63F,sizeof(char),2))),((_tmp640=" ",_tag_dyneither(_tmp640,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));};}
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka);struct
_dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){struct Cyc_Absyn_Kind
_tmp6E;enum Cyc_Absyn_KindQual _tmp6F;enum Cyc_Absyn_AliasQual _tmp70;struct Cyc_Absyn_Kind*
_tmp6D=ka;_tmp6E=*_tmp6D;_tmp6F=_tmp6E.kind;_tmp70=_tmp6E.aliasqual;switch(
_tmp6F){case Cyc_Absyn_AnyKind: _LL36: {const char*_tmp641;return(_tmp641="A",
_tag_dyneither(_tmp641,sizeof(char),2));}case Cyc_Absyn_MemKind: _LL37: {const char*
_tmp642;return(_tmp642="M",_tag_dyneither(_tmp642,sizeof(char),2));}case Cyc_Absyn_BoxKind:
_LL38: {const char*_tmp643;return(_tmp643="B",_tag_dyneither(_tmp643,sizeof(char),
2));}case Cyc_Absyn_RgnKind: _LL39: switch(_tmp70){case Cyc_Absyn_Aliasable: _LL3B: {
const char*_tmp644;return(_tmp644="R",_tag_dyneither(_tmp644,sizeof(char),2));}
case Cyc_Absyn_Unique: _LL3C: {const char*_tmp645;return(_tmp645="UR",
_tag_dyneither(_tmp645,sizeof(char),3));}case Cyc_Absyn_Top: _LL3D: {const char*
_tmp646;return(_tmp646="TR",_tag_dyneither(_tmp646,sizeof(char),3));}}case Cyc_Absyn_EffKind:
_LL3A: {const char*_tmp647;return(_tmp647="E",_tag_dyneither(_tmp647,sizeof(char),
2));}case Cyc_Absyn_IntKind: _LL3F: {const char*_tmp648;return(_tmp648="I",
_tag_dyneither(_tmp648,sizeof(char),2));}}}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(
struct Cyc_Absyn_Kind*k);struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*
k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}struct _dyneither_ptr Cyc_Absynpp_kindbound2string(
void*c);struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){void*_tmp79=Cyc_Absyn_compress_kb(
c);struct Cyc_Absyn_Kind*_tmp7B;struct Cyc_Absyn_Kind*_tmp7E;_LL42: {struct Cyc_Absyn_Eq_kb_struct*
_tmp7A=(struct Cyc_Absyn_Eq_kb_struct*)_tmp79;if(_tmp7A->tag != 0)goto _LL44;else{
_tmp7B=_tmp7A->f1;}}_LL43: return Cyc_Absynpp_kind2string(_tmp7B);_LL44: {struct
Cyc_Absyn_Unknown_kb_struct*_tmp7C=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp79;
if(_tmp7C->tag != 1)goto _LL46;}_LL45: if(Cyc_PP_tex_output){const char*_tmp649;
return(_tmp649="{?}",_tag_dyneither(_tmp649,sizeof(char),4));}else{const char*
_tmp64A;return(_tmp64A="?",_tag_dyneither(_tmp64A,sizeof(char),2));}_LL46: {
struct Cyc_Absyn_Less_kb_struct*_tmp7D=(struct Cyc_Absyn_Less_kb_struct*)_tmp79;
if(_tmp7D->tag != 2)goto _LL41;else{_tmp7E=_tmp7D->f2;}}_LL47: return Cyc_Absynpp_kind2string(
_tmp7E);_LL41:;}struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c);struct Cyc_PP_Doc*
Cyc_Absynpp_kindbound2doc(void*c){void*_tmp81=Cyc_Absyn_compress_kb(c);struct Cyc_Absyn_Kind*
_tmp83;struct Cyc_Absyn_Kind*_tmp86;_LL49: {struct Cyc_Absyn_Eq_kb_struct*_tmp82=(
struct Cyc_Absyn_Eq_kb_struct*)_tmp81;if(_tmp82->tag != 0)goto _LL4B;else{_tmp83=
_tmp82->f1;}}_LL4A: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp83));_LL4B: {
struct Cyc_Absyn_Unknown_kb_struct*_tmp84=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp81;if(_tmp84->tag != 1)goto _LL4D;}_LL4C: if(Cyc_PP_tex_output){const char*
_tmp64B;return Cyc_PP_text_width(((_tmp64B="{?}",_tag_dyneither(_tmp64B,sizeof(
char),4))),1);}else{const char*_tmp64C;return Cyc_PP_text(((_tmp64C="?",
_tag_dyneither(_tmp64C,sizeof(char),2))));}_LL4D: {struct Cyc_Absyn_Less_kb_struct*
_tmp85=(struct Cyc_Absyn_Less_kb_struct*)_tmp81;if(_tmp85->tag != 2)goto _LL48;
else{_tmp86=_tmp85->f2;}}_LL4E: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp86));
_LL48:;}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts);struct Cyc_PP_Doc*
Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){const char*_tmp64F;const char*_tmp64E;
const char*_tmp64D;return Cyc_PP_egroup(((_tmp64D="<",_tag_dyneither(_tmp64D,
sizeof(char),2))),((_tmp64E=">",_tag_dyneither(_tmp64E,sizeof(char),2))),((
_tmp64F=",",_tag_dyneither(_tmp64F,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv);struct Cyc_PP_Doc*
Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){void*_tmp8C=Cyc_Absyn_compress_kb(
tv->kind);struct Cyc_Absyn_Kind*_tmp8F;struct Cyc_Absyn_Kind*_tmp91;struct Cyc_Absyn_Kind*
_tmp93;_LL50: {struct Cyc_Absyn_Unknown_kb_struct*_tmp8D=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp8C;if(_tmp8D->tag != 1)goto _LL52;}_LL51: goto _LL53;_LL52: {struct Cyc_Absyn_Eq_kb_struct*
_tmp8E=(struct Cyc_Absyn_Eq_kb_struct*)_tmp8C;if(_tmp8E->tag != 0)goto _LL54;else{
_tmp8F=_tmp8E->f1;if(_tmp8F != Cyc_Absyn_BoxKind)goto _LL54;}}_LL53: return Cyc_PP_textptr(
tv->name);_LL54: {struct Cyc_Absyn_Less_kb_struct*_tmp90=(struct Cyc_Absyn_Less_kb_struct*)
_tmp8C;if(_tmp90->tag != 2)goto _LL56;else{_tmp91=_tmp90->f2;}}_LL55: _tmp93=_tmp91;
goto _LL57;_LL56: {struct Cyc_Absyn_Eq_kb_struct*_tmp92=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp8C;if(_tmp92->tag != 0)goto _LL4F;else{_tmp93=_tmp92->f1;}}_LL57: {const char*
_tmp652;struct Cyc_PP_Doc*_tmp651[3];return(_tmp651[2]=Cyc_Absynpp_kind2doc(
_tmp93),((_tmp651[1]=Cyc_PP_text(((_tmp652="::",_tag_dyneither(_tmp652,sizeof(
char),3)))),((_tmp651[0]=Cyc_PP_textptr(tv->name),Cyc_PP_cat(_tag_dyneither(
_tmp651,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL4F:;}struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(
struct Cyc_List_List*tvs);struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*
tvs){const char*_tmp655;const char*_tmp654;const char*_tmp653;return Cyc_PP_egroup(((
_tmp653="<",_tag_dyneither(_tmp653,sizeof(char),2))),((_tmp654=">",
_tag_dyneither(_tmp654,sizeof(char),2))),((_tmp655=",",_tag_dyneither(_tmp655,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}static struct
_dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv);static struct
_dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){return tv->name;}
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs);struct Cyc_PP_Doc*
Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);{const char*_tmp658;const char*_tmp657;const char*
_tmp656;return Cyc_PP_egroup(((_tmp656="<",_tag_dyneither(_tmp656,sizeof(char),2))),((
_tmp657=">",_tag_dyneither(_tmp657,sizeof(char),2))),((_tmp658=",",
_tag_dyneither(_tmp658,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((
struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,tvs)));};}struct _tuple15{
struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct
_tuple15*t);struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){return Cyc_Absynpp_tqtd2doc((*
t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts);
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){const char*_tmp65B;
const char*_tmp65A;const char*_tmp659;return Cyc_PP_group(((_tmp659="(",
_tag_dyneither(_tmp659,sizeof(char),2))),((_tmp65A=")",_tag_dyneither(_tmp65A,
sizeof(char),2))),((_tmp65B=",",_tag_dyneither(_tmp65B,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(
void*att);struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){void*_tmp9F=att;
_LL59: {struct Cyc_Absyn_Stdcall_att_struct*_tmpA0=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp9F;if(_tmpA0->tag != 1)goto _LL5B;}_LL5A: goto _LL5C;_LL5B: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpA1=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp9F;if(_tmpA1->tag != 2)goto _LL5D;}
_LL5C: goto _LL5E;_LL5D: {struct Cyc_Absyn_Fastcall_att_struct*_tmpA2=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp9F;if(_tmpA2->tag != 3)goto _LL5F;}_LL5E: return Cyc_PP_nil_doc();_LL5F:;_LL60:
return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL58:;}struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(
struct Cyc_List_List*atts);struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*
atts){for(0;atts != 0;atts=atts->tl){void*_tmpA3=(void*)atts->hd;_LL62: {struct
Cyc_Absyn_Stdcall_att_struct*_tmpA4=(struct Cyc_Absyn_Stdcall_att_struct*)_tmpA3;
if(_tmpA4->tag != 1)goto _LL64;}_LL63: {const char*_tmp65C;return Cyc_PP_text(((
_tmp65C=" _stdcall ",_tag_dyneither(_tmp65C,sizeof(char),11))));}_LL64: {struct
Cyc_Absyn_Cdecl_att_struct*_tmpA5=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpA3;if(
_tmpA5->tag != 2)goto _LL66;}_LL65: {const char*_tmp65D;return Cyc_PP_text(((_tmp65D=" _cdecl ",
_tag_dyneither(_tmp65D,sizeof(char),9))));}_LL66: {struct Cyc_Absyn_Fastcall_att_struct*
_tmpA6=(struct Cyc_Absyn_Fastcall_att_struct*)_tmpA3;if(_tmpA6->tag != 3)goto _LL68;}
_LL67: {const char*_tmp65E;return Cyc_PP_text(((_tmp65E=" _fastcall ",
_tag_dyneither(_tmp65E,sizeof(char),12))));}_LL68:;_LL69: goto _LL61;_LL61:;}
return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*
atts);struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){int
hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){void*
_tmpAA=(void*)atts2->hd;_LL6B: {struct Cyc_Absyn_Stdcall_att_struct*_tmpAB=(
struct Cyc_Absyn_Stdcall_att_struct*)_tmpAA;if(_tmpAB->tag != 1)goto _LL6D;}_LL6C:
goto _LL6E;_LL6D: {struct Cyc_Absyn_Cdecl_att_struct*_tmpAC=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmpAA;if(_tmpAC->tag != 2)goto _LL6F;}_LL6E: goto _LL70;_LL6F: {struct Cyc_Absyn_Fastcall_att_struct*
_tmpAD=(struct Cyc_Absyn_Fastcall_att_struct*)_tmpAA;if(_tmpAD->tag != 3)goto _LL71;}
_LL70: goto _LL6A;_LL71:;_LL72: hasatt=1;goto _LL6A;_LL6A:;}}if(!hasatt)return Cyc_PP_nil_doc();{
const char*_tmp669;const char*_tmp668;const char*_tmp667;const char*_tmp666;const
char*_tmp665;struct Cyc_PP_Doc*_tmp664[3];return(_tmp664[2]=Cyc_PP_text(((_tmp665=")",
_tag_dyneither(_tmp665,sizeof(char),2)))),((_tmp664[1]=Cyc_PP_group(((_tmp668="",
_tag_dyneither(_tmp668,sizeof(char),1))),((_tmp667="",_tag_dyneither(_tmp667,
sizeof(char),1))),((_tmp666=" ",_tag_dyneither(_tmp666,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_noncallatt2doc,atts)),((_tmp664[0]=Cyc_PP_text(((_tmp669=" __declspec(",
_tag_dyneither(_tmp669,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp664,
sizeof(struct Cyc_PP_Doc*),3)))))));};}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*
a);struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(
a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts);struct Cyc_PP_Doc*
Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){if(atts == 0)return Cyc_PP_nil_doc();
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL73: return Cyc_Absynpp_noncallconv2doc(
atts);case Cyc_Cyclone_Gcc_c: _LL74: {const char*_tmp672;const char*_tmp671;const
char*_tmp670;const char*_tmp66F;struct Cyc_PP_Doc*_tmp66E[2];return(_tmp66E[1]=Cyc_PP_group(((
_tmp671="((",_tag_dyneither(_tmp671,sizeof(char),3))),((_tmp670="))",
_tag_dyneither(_tmp670,sizeof(char),3))),((_tmp66F=",",_tag_dyneither(_tmp66F,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((_tmp66E[0]=Cyc_PP_text(((
_tmp672=" __attribute__",_tag_dyneither(_tmp672,sizeof(char),15)))),Cyc_PP_cat(
_tag_dyneither(_tmp66E,sizeof(struct Cyc_PP_Doc*),2)))));}}}int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List*tms);int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{void*_tmpB9=(void*)tms->hd;_LL77: {struct Cyc_Absyn_Pointer_mod_struct*
_tmpBA=(struct Cyc_Absyn_Pointer_mod_struct*)_tmpB9;if(_tmpBA->tag != 2)goto _LL79;}
_LL78: return 1;_LL79: {struct Cyc_Absyn_Attributes_mod_struct*_tmpBB=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpB9;if(_tmpBB->tag != 5)goto _LL7B;}_LL7A: switch(Cyc_Cyclone_c_compiler){case
Cyc_Cyclone_Gcc_c: _LL7D: return 0;default: _LL7E: return Cyc_Absynpp_next_is_pointer(
tms->tl);}_LL7B:;_LL7C: return 0;_LL76:;};}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(
void*t);static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;static struct Cyc_PP_Doc*
Cyc_Absynpp_question();static struct Cyc_PP_Doc*Cyc_Absynpp_question(){if(!((
unsigned int)Cyc_Absynpp_cache_question)){if(Cyc_PP_tex_output){const char*
_tmp673;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp673="{?}",_tag_dyneither(_tmp673,sizeof(char),4))),1);}else{const char*
_tmp674;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp674="?",
_tag_dyneither(_tmp674,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_question);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb();static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){if(Cyc_PP_tex_output){const char*
_tmp675;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp675="{\\lb}",
_tag_dyneither(_tmp675,sizeof(char),6))),1);}else{const char*_tmp676;Cyc_Absynpp_cache_lb=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp676="{",_tag_dyneither(_tmp676,sizeof(char),
2))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}static struct
Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_rb();static
struct Cyc_PP_Doc*Cyc_Absynpp_rb(){if(!((unsigned int)Cyc_Absynpp_cache_rb)){if(
Cyc_PP_tex_output){const char*_tmp677;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp677="{\\rb}",_tag_dyneither(_tmp677,sizeof(char),6))),1);}else{const char*
_tmp678;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp678="}",
_tag_dyneither(_tmp678,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_rb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;static
struct Cyc_PP_Doc*Cyc_Absynpp_dollar();static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){if(Cyc_PP_tex_output){const char*
_tmp679;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp679="\\$",
_tag_dyneither(_tmp679,sizeof(char),3))),1);}else{const char*_tmp67A;Cyc_Absynpp_cache_dollar=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp67A="$",_tag_dyneither(_tmp67A,sizeof(char),
2))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}struct
Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*
ss);struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*
ss){struct Cyc_PP_Doc*_tmp67B[3];return(_tmp67B[2]=Cyc_Absynpp_rb(),((_tmp67B[1]=
Cyc_PP_seq(sep,ss),((_tmp67B[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(
_tmp67B,sizeof(struct Cyc_PP_Doc*),3)))))));}static void Cyc_Absynpp_print_tms(
struct Cyc_List_List*tms);static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){void*_tmpC5=(void*)tms->hd;void*_tmpC9;struct Cyc_List_List*
_tmpCB;_LL81: {struct Cyc_Absyn_Carray_mod_struct*_tmpC6=(struct Cyc_Absyn_Carray_mod_struct*)
_tmpC5;if(_tmpC6->tag != 0)goto _LL83;}_LL82:{const char*_tmp67E;void*_tmp67D;(
_tmp67D=0,Cyc_fprintf(Cyc_stderr,((_tmp67E="Carray_mod ",_tag_dyneither(_tmp67E,
sizeof(char),12))),_tag_dyneither(_tmp67D,sizeof(void*),0)));}goto _LL80;_LL83: {
struct Cyc_Absyn_ConstArray_mod_struct*_tmpC7=(struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpC5;if(_tmpC7->tag != 1)goto _LL85;}_LL84:{const char*_tmp681;void*_tmp680;(
_tmp680=0,Cyc_fprintf(Cyc_stderr,((_tmp681="ConstArray_mod ",_tag_dyneither(
_tmp681,sizeof(char),16))),_tag_dyneither(_tmp680,sizeof(void*),0)));}goto _LL80;
_LL85: {struct Cyc_Absyn_Function_mod_struct*_tmpC8=(struct Cyc_Absyn_Function_mod_struct*)
_tmpC5;if(_tmpC8->tag != 3)goto _LL87;else{_tmpC9=(void*)_tmpC8->f1;{struct Cyc_Absyn_WithTypes_struct*
_tmpCA=(struct Cyc_Absyn_WithTypes_struct*)_tmpC9;if(_tmpCA->tag != 1)goto _LL87;
else{_tmpCB=_tmpCA->f1;}};}}_LL86:{const char*_tmp684;void*_tmp683;(_tmp683=0,Cyc_fprintf(
Cyc_stderr,((_tmp684="Function_mod(",_tag_dyneither(_tmp684,sizeof(char),14))),
_tag_dyneither(_tmp683,sizeof(void*),0)));}for(0;_tmpCB != 0;_tmpCB=_tmpCB->tl){
struct Cyc_Core_Opt*_tmpD6=(*((struct _tuple7*)_tmpCB->hd)).f1;if(_tmpD6 == 0){
const char*_tmp687;void*_tmp686;(_tmp686=0,Cyc_fprintf(Cyc_stderr,((_tmp687="?",
_tag_dyneither(_tmp687,sizeof(char),2))),_tag_dyneither(_tmp686,sizeof(void*),0)));}
else{void*_tmp688;(_tmp688=0,Cyc_fprintf(Cyc_stderr,*((struct _dyneither_ptr*)
_tmpD6->v),_tag_dyneither(_tmp688,sizeof(void*),0)));}if(_tmpCB->tl != 0){const
char*_tmp68B;void*_tmp68A;(_tmp68A=0,Cyc_fprintf(Cyc_stderr,((_tmp68B=",",
_tag_dyneither(_tmp68B,sizeof(char),2))),_tag_dyneither(_tmp68A,sizeof(void*),0)));}}{
const char*_tmp68E;void*_tmp68D;(_tmp68D=0,Cyc_fprintf(Cyc_stderr,((_tmp68E=") ",
_tag_dyneither(_tmp68E,sizeof(char),3))),_tag_dyneither(_tmp68D,sizeof(void*),0)));}
goto _LL80;_LL87: {struct Cyc_Absyn_Function_mod_struct*_tmpCC=(struct Cyc_Absyn_Function_mod_struct*)
_tmpC5;if(_tmpCC->tag != 3)goto _LL89;}_LL88:{const char*_tmp691;void*_tmp690;(
_tmp690=0,Cyc_fprintf(Cyc_stderr,((_tmp691="Function_mod()",_tag_dyneither(
_tmp691,sizeof(char),15))),_tag_dyneither(_tmp690,sizeof(void*),0)));}goto _LL80;
_LL89: {struct Cyc_Absyn_Attributes_mod_struct*_tmpCD=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpC5;if(_tmpCD->tag != 5)goto _LL8B;}_LL8A:{const char*_tmp694;void*_tmp693;(
_tmp693=0,Cyc_fprintf(Cyc_stderr,((_tmp694="Attributes_mod ",_tag_dyneither(
_tmp694,sizeof(char),16))),_tag_dyneither(_tmp693,sizeof(void*),0)));}goto _LL80;
_LL8B: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpCE=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpC5;if(_tmpCE->tag != 4)goto _LL8D;}_LL8C:{const char*_tmp697;void*_tmp696;(
_tmp696=0,Cyc_fprintf(Cyc_stderr,((_tmp697="TypeParams_mod ",_tag_dyneither(
_tmp697,sizeof(char),16))),_tag_dyneither(_tmp696,sizeof(void*),0)));}goto _LL80;
_LL8D: {struct Cyc_Absyn_Pointer_mod_struct*_tmpCF=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpC5;if(_tmpCF->tag != 2)goto _LL80;}_LL8E:{const char*_tmp69A;void*_tmp699;(
_tmp699=0,Cyc_fprintf(Cyc_stderr,((_tmp69A="Pointer_mod ",_tag_dyneither(_tmp69A,
sizeof(char),13))),_tag_dyneither(_tmp699,sizeof(void*),0)));}goto _LL80;_LL80:;}{
const char*_tmp69D;void*_tmp69C;(_tmp69C=0,Cyc_fprintf(Cyc_stderr,((_tmp69D="\n",
_tag_dyneither(_tmp69D,sizeof(char),2))),_tag_dyneither(_tmp69C,sizeof(void*),0)));};}
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct
Cyc_List_List*tms);struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct
Cyc_PP_Doc*d,struct Cyc_List_List*tms){if(tms == 0)return d;{struct Cyc_PP_Doc*rest=
Cyc_Absynpp_dtms2doc(0,d,tms->tl);const char*_tmp6A2;const char*_tmp6A1;struct Cyc_PP_Doc*
_tmp6A0[3];struct Cyc_PP_Doc*p_rest=(_tmp6A0[2]=Cyc_PP_text(((_tmp6A1=")",
_tag_dyneither(_tmp6A1,sizeof(char),2)))),((_tmp6A0[1]=rest,((_tmp6A0[0]=Cyc_PP_text(((
_tmp6A2="(",_tag_dyneither(_tmp6A2,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp6A0,sizeof(struct Cyc_PP_Doc*),3)))))));void*_tmpE8=(void*)tms->hd;union Cyc_Absyn_Constraint*
_tmpEA;struct Cyc_Absyn_Exp*_tmpEC;union Cyc_Absyn_Constraint*_tmpED;void*_tmpEF;
struct Cyc_List_List*_tmpF1;struct Cyc_List_List*_tmpF3;struct Cyc_Position_Segment*
_tmpF4;int _tmpF5;struct Cyc_Absyn_PtrAtts _tmpF7;void*_tmpF8;union Cyc_Absyn_Constraint*
_tmpF9;union Cyc_Absyn_Constraint*_tmpFA;union Cyc_Absyn_Constraint*_tmpFB;struct
Cyc_Absyn_Tqual _tmpFC;_LL90: {struct Cyc_Absyn_Carray_mod_struct*_tmpE9=(struct
Cyc_Absyn_Carray_mod_struct*)_tmpE8;if(_tmpE9->tag != 0)goto _LL92;else{_tmpEA=
_tmpE9->f1;}}_LL91: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{const
char*_tmp6A7;const char*_tmp6A6;struct Cyc_PP_Doc*_tmp6A5[2];return(_tmp6A5[1]=((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEA)?Cyc_PP_text(((
_tmp6A6="[]@zeroterm",_tag_dyneither(_tmp6A6,sizeof(char),12)))): Cyc_PP_text(((
_tmp6A7="[]",_tag_dyneither(_tmp6A7,sizeof(char),3)))),((_tmp6A5[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp6A5,sizeof(struct Cyc_PP_Doc*),2)))));};_LL92: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmpEB=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmpE8;if(_tmpEB->tag != 1)goto
_LL94;else{_tmpEC=_tmpEB->f1;_tmpED=_tmpEB->f2;}}_LL93: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{const char*_tmp6AE;const char*_tmp6AD;const char*_tmp6AC;
struct Cyc_PP_Doc*_tmp6AB[4];return(_tmp6AB[3]=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmpED)?Cyc_PP_text(((_tmp6AC="]@zeroterm",
_tag_dyneither(_tmp6AC,sizeof(char),11)))): Cyc_PP_text(((_tmp6AD="]",
_tag_dyneither(_tmp6AD,sizeof(char),2)))),((_tmp6AB[2]=Cyc_Absynpp_exp2doc(
_tmpEC),((_tmp6AB[1]=Cyc_PP_text(((_tmp6AE="[",_tag_dyneither(_tmp6AE,sizeof(
char),2)))),((_tmp6AB[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6AB,sizeof(struct Cyc_PP_Doc*),
4)))))))));};_LL94: {struct Cyc_Absyn_Function_mod_struct*_tmpEE=(struct Cyc_Absyn_Function_mod_struct*)
_tmpE8;if(_tmpEE->tag != 3)goto _LL96;else{_tmpEF=(void*)_tmpEE->f1;}}_LL95: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{void*_tmp104=_tmpEF;struct Cyc_List_List*_tmp106;int _tmp107;
struct Cyc_Absyn_VarargInfo*_tmp108;struct Cyc_Core_Opt*_tmp109;struct Cyc_List_List*
_tmp10A;struct Cyc_List_List*_tmp10C;struct Cyc_Position_Segment*_tmp10D;_LL9D: {
struct Cyc_Absyn_WithTypes_struct*_tmp105=(struct Cyc_Absyn_WithTypes_struct*)
_tmp104;if(_tmp105->tag != 1)goto _LL9F;else{_tmp106=_tmp105->f1;_tmp107=_tmp105->f2;
_tmp108=_tmp105->f3;_tmp109=_tmp105->f4;_tmp10A=_tmp105->f5;}}_LL9E: {struct Cyc_PP_Doc*
_tmp6AF[2];return(_tmp6AF[1]=Cyc_Absynpp_funargs2doc(_tmp106,_tmp107,_tmp108,
_tmp109,_tmp10A),((_tmp6AF[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6AF,sizeof(
struct Cyc_PP_Doc*),2)))));}_LL9F: {struct Cyc_Absyn_NoTypes_struct*_tmp10B=(
struct Cyc_Absyn_NoTypes_struct*)_tmp104;if(_tmp10B->tag != 0)goto _LL9C;else{
_tmp10C=_tmp10B->f1;_tmp10D=_tmp10B->f2;}}_LLA0: {const char*_tmp6B6;const char*
_tmp6B5;const char*_tmp6B4;struct Cyc_PP_Doc*_tmp6B3[2];return(_tmp6B3[1]=Cyc_PP_group(((
_tmp6B6="(",_tag_dyneither(_tmp6B6,sizeof(char),2))),((_tmp6B5=")",
_tag_dyneither(_tmp6B5,sizeof(char),2))),((_tmp6B4=",",_tag_dyneither(_tmp6B4,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp10C)),((
_tmp6B3[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B3,sizeof(struct Cyc_PP_Doc*),2)))));}
_LL9C:;};_LL96: {struct Cyc_Absyn_Attributes_mod_struct*_tmpF0=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpE8;if(_tmpF0->tag != 5)goto _LL98;else{_tmpF1=_tmpF0->f2;}}_LL97: switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LLA1: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp6B7[2];return(_tmp6B7[1]=Cyc_Absynpp_atts2doc(_tmpF1),((
_tmp6B7[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B7,sizeof(struct Cyc_PP_Doc*),2)))));};
case Cyc_Cyclone_Vc_c: _LLA2: if(Cyc_Absynpp_next_is_pointer(tms->tl)){struct Cyc_PP_Doc*
_tmp6B8[2];return(_tmp6B8[1]=rest,((_tmp6B8[0]=Cyc_Absynpp_callconv2doc(_tmpF1),
Cyc_PP_cat(_tag_dyneither(_tmp6B8,sizeof(struct Cyc_PP_Doc*),2)))));}return rest;}
_LL98: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpF2=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpE8;if(_tmpF2->tag != 4)goto _LL9A;else{_tmpF3=_tmpF2->f1;_tmpF4=_tmpF2->f2;
_tmpF5=_tmpF2->f3;}}_LL99: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmpF5){struct Cyc_PP_Doc*_tmp6B9[2];return(_tmp6B9[1]=Cyc_Absynpp_ktvars2doc(
_tmpF3),((_tmp6B9[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B9,sizeof(struct Cyc_PP_Doc*),
2)))));}else{struct Cyc_PP_Doc*_tmp6BA[2];return(_tmp6BA[1]=Cyc_Absynpp_tvars2doc(
_tmpF3),((_tmp6BA[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6BA,sizeof(struct Cyc_PP_Doc*),
2)))));}_LL9A: {struct Cyc_Absyn_Pointer_mod_struct*_tmpF6=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpE8;if(_tmpF6->tag != 2)goto _LL8F;else{_tmpF7=_tmpF6->f1;_tmpF8=_tmpF7.rgn;
_tmpF9=_tmpF7.nullable;_tmpFA=_tmpF7.bounds;_tmpFB=_tmpF7.zero_term;_tmpFC=
_tmpF6->f2;}}_LL9B: {struct Cyc_PP_Doc*ptr;{void*_tmp117=((void*(*)(void*y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpFA);
struct Cyc_Absyn_Exp*_tmp11A;_LLA5: {struct Cyc_Absyn_DynEither_b_struct*_tmp118=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp117;if(_tmp118->tag != 0)goto _LLA7;}_LLA6:
ptr=Cyc_Absynpp_question();goto _LLA4;_LLA7: {struct Cyc_Absyn_Upper_b_struct*
_tmp119=(struct Cyc_Absyn_Upper_b_struct*)_tmp117;if(_tmp119->tag != 1)goto _LLA4;
else{_tmp11A=_tmp119->f1;}}_LLA8:{const char*_tmp6BC;const char*_tmp6BB;ptr=Cyc_PP_text(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpF9)?(
_tmp6BC="*",_tag_dyneither(_tmp6BC,sizeof(char),2)):((_tmp6BB="@",_tag_dyneither(
_tmp6BB,sizeof(char),2))));}{unsigned int _tmp11E;int _tmp11F;struct _tuple10
_tmp11D=Cyc_Evexp_eval_const_uint_exp(_tmp11A);_tmp11E=_tmp11D.f1;_tmp11F=
_tmp11D.f2;if(!_tmp11F  || _tmp11E != 1){struct Cyc_PP_Doc*_tmp6BD[4];ptr=((_tmp6BD[
3]=Cyc_Absynpp_rb(),((_tmp6BD[2]=Cyc_Absynpp_exp2doc(_tmp11A),((_tmp6BD[1]=Cyc_Absynpp_lb(),((
_tmp6BD[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6BD,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLA4;};_LLA4:;}if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr  && ((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFB)){const char*
_tmp6C0;struct Cyc_PP_Doc*_tmp6BF[2];ptr=((_tmp6BF[1]=Cyc_PP_text(((_tmp6C0="@zeroterm ",
_tag_dyneither(_tmp6C0,sizeof(char),11)))),((_tmp6BF[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6BF,sizeof(struct Cyc_PP_Doc*),2))))));}else{if(is_char_ptr
 && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFB)){
const char*_tmp6C3;struct Cyc_PP_Doc*_tmp6C2[2];ptr=((_tmp6C2[1]=Cyc_PP_text(((
_tmp6C3="@nozeroterm ",_tag_dyneither(_tmp6C3,sizeof(char),13)))),((_tmp6C2[0]=
ptr,Cyc_PP_cat(_tag_dyneither(_tmp6C2,sizeof(struct Cyc_PP_Doc*),2))))));}}}{void*
_tmp125=Cyc_Tcutil_compress(_tmpF8);_LLAA: {struct Cyc_Absyn_HeapRgn_struct*
_tmp126=(struct Cyc_Absyn_HeapRgn_struct*)_tmp125;if(_tmp126->tag != 21)goto _LLAC;}
_LLAB: goto _LLA9;_LLAC:{struct Cyc_Absyn_Evar_struct*_tmp127=(struct Cyc_Absyn_Evar_struct*)
_tmp125;if(_tmp127->tag != 1)goto _LLAE;}if(!Cyc_Absynpp_print_for_cycdoc)goto
_LLAE;_LLAD: goto _LLA9;_LLAE:;_LLAF: {const char*_tmp6C6;struct Cyc_PP_Doc*_tmp6C5[
3];ptr=((_tmp6C5[2]=Cyc_PP_text(((_tmp6C6=" ",_tag_dyneither(_tmp6C6,sizeof(char),
2)))),((_tmp6C5[1]=Cyc_Absynpp_typ2doc(_tmpF8),((_tmp6C5[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6C5,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLA9:;}{struct Cyc_PP_Doc*
_tmp6C7[2];ptr=((_tmp6C7[1]=Cyc_Absynpp_tqual2doc(_tmpFC),((_tmp6C7[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6C7,sizeof(struct Cyc_PP_Doc*),2))))));}{struct Cyc_PP_Doc*
_tmp6C8[2];return(_tmp6C8[1]=rest,((_tmp6C8[0]=ptr,Cyc_PP_cat(_tag_dyneither(
_tmp6C8,sizeof(struct Cyc_PP_Doc*),2)))));};}_LL8F:;};}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){void*_tmp12F=Cyc_Tcutil_compress(
t);_LLB1: {struct Cyc_Absyn_HeapRgn_struct*_tmp130=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp12F;if(_tmp130->tag != 21)goto _LLB3;}_LLB2: {const char*_tmp6C9;return Cyc_PP_text(((
_tmp6C9="`H",_tag_dyneither(_tmp6C9,sizeof(char),3))));}_LLB3: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp131=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp12F;if(_tmp131->tag != 22)goto
_LLB5;}_LLB4: {const char*_tmp6CA;return Cyc_PP_text(((_tmp6CA="`U",_tag_dyneither(
_tmp6CA,sizeof(char),3))));}_LLB5:;_LLB6: return Cyc_Absynpp_ntyp2doc(t);_LLB0:;}
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**
effects,void*t);static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,
struct Cyc_List_List**effects,void*t){void*_tmp134=Cyc_Tcutil_compress(t);void*
_tmp136;struct Cyc_List_List*_tmp138;_LLB8: {struct Cyc_Absyn_AccessEff_struct*
_tmp135=(struct Cyc_Absyn_AccessEff_struct*)_tmp134;if(_tmp135->tag != 23)goto
_LLBA;else{_tmp136=(void*)_tmp135->f1;}}_LLB9:{struct Cyc_List_List*_tmp6CB;*
rgions=((_tmp6CB=_cycalloc(sizeof(*_tmp6CB)),((_tmp6CB->hd=Cyc_Absynpp_rgn2doc(
_tmp136),((_tmp6CB->tl=*rgions,_tmp6CB))))));}goto _LLB7;_LLBA: {struct Cyc_Absyn_JoinEff_struct*
_tmp137=(struct Cyc_Absyn_JoinEff_struct*)_tmp134;if(_tmp137->tag != 24)goto _LLBC;
else{_tmp138=_tmp137->f1;}}_LLBB: for(0;_tmp138 != 0;_tmp138=_tmp138->tl){Cyc_Absynpp_effects2docs(
rgions,effects,(void*)_tmp138->hd);}goto _LLB7;_LLBC:;_LLBD:{struct Cyc_List_List*
_tmp6CC;*effects=((_tmp6CC=_cycalloc(sizeof(*_tmp6CC)),((_tmp6CC->hd=Cyc_Absynpp_typ2doc(
t),((_tmp6CC->tl=*effects,_tmp6CC))))));}goto _LLB7;_LLB7:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){struct Cyc_List_List*rgions=
0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0  && effects != 0){const char*_tmp6CF;const char*_tmp6CE;const
char*_tmp6CD;return Cyc_PP_group(((_tmp6CD="",_tag_dyneither(_tmp6CD,sizeof(char),
1))),((_tmp6CE="",_tag_dyneither(_tmp6CE,sizeof(char),1))),((_tmp6CF="+",
_tag_dyneither(_tmp6CF,sizeof(char),2))),effects);}else{const char*_tmp6D0;struct
Cyc_PP_Doc*_tmp13E=Cyc_Absynpp_group_braces(((_tmp6D0=",",_tag_dyneither(_tmp6D0,
sizeof(char),2))),rgions);struct Cyc_List_List*_tmp6D4;const char*_tmp6D3;const
char*_tmp6D2;const char*_tmp6D1;return Cyc_PP_group(((_tmp6D1="",_tag_dyneither(
_tmp6D1,sizeof(char),1))),((_tmp6D2="",_tag_dyneither(_tmp6D2,sizeof(char),1))),((
_tmp6D3="+",_tag_dyneither(_tmp6D3,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((
_tmp6D4=_cycalloc(sizeof(*_tmp6D4)),((_tmp6D4->hd=_tmp13E,((_tmp6D4->tl=0,
_tmp6D4))))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind
k);struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){switch(k){
case Cyc_Absyn_StructA: _LLBE: {const char*_tmp6D5;return Cyc_PP_text(((_tmp6D5="struct ",
_tag_dyneither(_tmp6D5,sizeof(char),8))));}case Cyc_Absyn_UnionA: _LLBF: {const
char*_tmp6D6;return Cyc_PP_text(((_tmp6D6="union ",_tag_dyneither(_tmp6D6,sizeof(
char),7))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);struct Cyc_PP_Doc*
Cyc_Absynpp_ntyp2doc(void*t){struct Cyc_PP_Doc*s;{void*_tmp146=t;struct Cyc_Core_Opt*
_tmp14C;struct Cyc_Core_Opt*_tmp14D;int _tmp14E;struct Cyc_Core_Opt*_tmp14F;struct
Cyc_Absyn_Tvar*_tmp151;struct Cyc_Absyn_DatatypeInfo _tmp153;union Cyc_Absyn_DatatypeInfoU
_tmp154;struct Cyc_List_List*_tmp155;struct Cyc_Absyn_DatatypeFieldInfo _tmp157;
union Cyc_Absyn_DatatypeFieldInfoU _tmp158;struct Cyc_List_List*_tmp159;enum Cyc_Absyn_Sign
_tmp15B;enum Cyc_Absyn_Size_of _tmp15C;int _tmp15F;struct Cyc_List_List*_tmp161;
struct Cyc_Absyn_AggrInfo _tmp163;union Cyc_Absyn_AggrInfoU _tmp164;struct Cyc_List_List*
_tmp165;enum Cyc_Absyn_AggrKind _tmp167;struct Cyc_List_List*_tmp168;struct Cyc_List_List*
_tmp16A;struct _tuple0*_tmp16C;struct Cyc_Absyn_Exp*_tmp16E;struct _tuple0*_tmp170;
struct Cyc_List_List*_tmp171;struct Cyc_Absyn_Typedefdecl*_tmp172;void*_tmp174;
void*_tmp176;void*_tmp177;void*_tmp179;void*_tmp17D;_LLC2: {struct Cyc_Absyn_ArrayType_struct*
_tmp147=(struct Cyc_Absyn_ArrayType_struct*)_tmp146;if(_tmp147->tag != 9)goto _LLC4;}
_LLC3: {const char*_tmp6D7;return Cyc_PP_text(((_tmp6D7="[[[array]]]",
_tag_dyneither(_tmp6D7,sizeof(char),12))));}_LLC4: {struct Cyc_Absyn_FnType_struct*
_tmp148=(struct Cyc_Absyn_FnType_struct*)_tmp146;if(_tmp148->tag != 10)goto _LLC6;}
_LLC5: return Cyc_PP_nil_doc();_LLC6: {struct Cyc_Absyn_PointerType_struct*_tmp149=(
struct Cyc_Absyn_PointerType_struct*)_tmp146;if(_tmp149->tag != 5)goto _LLC8;}_LLC7:
return Cyc_PP_nil_doc();_LLC8: {struct Cyc_Absyn_VoidType_struct*_tmp14A=(struct
Cyc_Absyn_VoidType_struct*)_tmp146;if(_tmp14A->tag != 0)goto _LLCA;}_LLC9:{const
char*_tmp6D8;s=Cyc_PP_text(((_tmp6D8="void",_tag_dyneither(_tmp6D8,sizeof(char),
5))));}goto _LLC1;_LLCA: {struct Cyc_Absyn_Evar_struct*_tmp14B=(struct Cyc_Absyn_Evar_struct*)
_tmp146;if(_tmp14B->tag != 1)goto _LLCC;else{_tmp14C=_tmp14B->f1;_tmp14D=_tmp14B->f2;
_tmp14E=_tmp14B->f3;_tmp14F=_tmp14B->f4;}}_LLCB: if(_tmp14D != 0)return Cyc_Absynpp_ntyp2doc((
void*)_tmp14D->v);else{const char*_tmp6E8;const char*_tmp6E7;const char*_tmp6E6;
struct Cyc_Int_pa_struct _tmp6E5;void*_tmp6E4[1];const char*_tmp6E3;const char*
_tmp6E2;struct Cyc_PP_Doc*_tmp6E1[6];s=((_tmp6E1[5]=_tmp14C == 0?Cyc_Absynpp_question():
Cyc_Absynpp_kind2doc((struct Cyc_Absyn_Kind*)_tmp14C->v),((_tmp6E1[4]=Cyc_PP_text(((
_tmp6E2=")::",_tag_dyneither(_tmp6E2,sizeof(char),4)))),((_tmp6E1[3]=(!Cyc_Absynpp_print_full_evars
 || _tmp14F == 0)?Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)
_tmp14F->v),((_tmp6E1[2]=Cyc_PP_text((struct _dyneither_ptr)((_tmp6E5.tag=1,((
_tmp6E5.f1=(unsigned long)_tmp14E,((_tmp6E4[0]=& _tmp6E5,Cyc_aprintf(((_tmp6E3="%d",
_tag_dyneither(_tmp6E3,sizeof(char),3))),_tag_dyneither(_tmp6E4,sizeof(void*),1))))))))),((
_tmp6E1[1]=Cyc_PP_text(((_tmp6E6="(",_tag_dyneither(_tmp6E6,sizeof(char),2)))),((
_tmp6E1[0]=Cyc_Absynpp_print_for_cycdoc?Cyc_PP_text(((_tmp6E7="\\%",
_tag_dyneither(_tmp6E7,sizeof(char),3)))): Cyc_PP_text(((_tmp6E8="%",
_tag_dyneither(_tmp6E8,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6E1,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));}goto _LLC1;_LLCC: {struct Cyc_Absyn_VarType_struct*
_tmp150=(struct Cyc_Absyn_VarType_struct*)_tmp146;if(_tmp150->tag != 2)goto _LLCE;
else{_tmp151=_tmp150->f1;}}_LLCD: s=Cyc_PP_textptr(_tmp151->name);if(Cyc_Absynpp_print_all_kinds){
const char*_tmp6EB;struct Cyc_PP_Doc*_tmp6EA[3];s=((_tmp6EA[2]=Cyc_Absynpp_kindbound2doc(
_tmp151->kind),((_tmp6EA[1]=Cyc_PP_text(((_tmp6EB="::",_tag_dyneither(_tmp6EB,
sizeof(char),3)))),((_tmp6EA[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp6EA,sizeof(
struct Cyc_PP_Doc*),3))))))));}if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(
_tmp151)){const char*_tmp6F0;const char*_tmp6EF;struct Cyc_PP_Doc*_tmp6EE[3];s=((
_tmp6EE[2]=Cyc_PP_text(((_tmp6EF=" */",_tag_dyneither(_tmp6EF,sizeof(char),4)))),((
_tmp6EE[1]=s,((_tmp6EE[0]=Cyc_PP_text(((_tmp6F0="_ /* ",_tag_dyneither(_tmp6F0,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp6EE,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLC1;_LLCE: {struct Cyc_Absyn_DatatypeType_struct*_tmp152=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp146;if(_tmp152->tag != 3)goto _LLD0;else{_tmp153=_tmp152->f1;_tmp154=_tmp153.datatype_info;
_tmp155=_tmp153.targs;}}_LLCF:{union Cyc_Absyn_DatatypeInfoU _tmp18F=_tmp154;
struct Cyc_Absyn_UnknownDatatypeInfo _tmp190;struct _tuple0*_tmp191;int _tmp192;
struct Cyc_Absyn_Datatypedecl**_tmp193;struct Cyc_Absyn_Datatypedecl*_tmp194;
struct Cyc_Absyn_Datatypedecl _tmp195;struct _tuple0*_tmp196;int _tmp197;_LLF7: if((
_tmp18F.UnknownDatatype).tag != 1)goto _LLF9;_tmp190=(struct Cyc_Absyn_UnknownDatatypeInfo)(
_tmp18F.UnknownDatatype).val;_tmp191=_tmp190.name;_tmp192=_tmp190.is_extensible;
_LLF8: _tmp196=_tmp191;_tmp197=_tmp192;goto _LLFA;_LLF9: if((_tmp18F.KnownDatatype).tag
!= 2)goto _LLF6;_tmp193=(struct Cyc_Absyn_Datatypedecl**)(_tmp18F.KnownDatatype).val;
_tmp194=*_tmp193;_tmp195=*_tmp194;_tmp196=_tmp195.name;_tmp197=_tmp195.is_extensible;
_LLFA: {const char*_tmp6F1;struct Cyc_PP_Doc*_tmp198=Cyc_PP_text(((_tmp6F1="datatype ",
_tag_dyneither(_tmp6F1,sizeof(char),10))));const char*_tmp6F2;struct Cyc_PP_Doc*
_tmp199=_tmp197?Cyc_PP_text(((_tmp6F2="@extensible ",_tag_dyneither(_tmp6F2,
sizeof(char),13)))): Cyc_PP_nil_doc();{struct Cyc_PP_Doc*_tmp6F3[4];s=((_tmp6F3[3]=
Cyc_Absynpp_tps2doc(_tmp155),((_tmp6F3[2]=Cyc_Absynpp_qvar2doc(_tmp196),((
_tmp6F3[1]=_tmp198,((_tmp6F3[0]=_tmp199,Cyc_PP_cat(_tag_dyneither(_tmp6F3,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLF6;}_LLF6:;}goto _LLC1;_LLD0: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp156=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp146;if(_tmp156->tag != 4)goto _LLD2;else{_tmp157=_tmp156->f1;_tmp158=_tmp157.field_info;
_tmp159=_tmp157.targs;}}_LLD1:{union Cyc_Absyn_DatatypeFieldInfoU _tmp19D=_tmp158;
struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp19E;struct _tuple0*_tmp19F;struct
_tuple0*_tmp1A0;int _tmp1A1;struct _tuple1 _tmp1A2;struct Cyc_Absyn_Datatypedecl*
_tmp1A3;struct Cyc_Absyn_Datatypedecl _tmp1A4;struct _tuple0*_tmp1A5;int _tmp1A6;
struct Cyc_Absyn_Datatypefield*_tmp1A7;struct Cyc_Absyn_Datatypefield _tmp1A8;
struct _tuple0*_tmp1A9;_LLFC: if((_tmp19D.UnknownDatatypefield).tag != 1)goto _LLFE;
_tmp19E=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp19D.UnknownDatatypefield).val;
_tmp19F=_tmp19E.datatype_name;_tmp1A0=_tmp19E.field_name;_tmp1A1=_tmp19E.is_extensible;
_LLFD: _tmp1A5=_tmp19F;_tmp1A6=_tmp1A1;_tmp1A9=_tmp1A0;goto _LLFF;_LLFE: if((
_tmp19D.KnownDatatypefield).tag != 2)goto _LLFB;_tmp1A2=(struct _tuple1)(_tmp19D.KnownDatatypefield).val;
_tmp1A3=_tmp1A2.f1;_tmp1A4=*_tmp1A3;_tmp1A5=_tmp1A4.name;_tmp1A6=_tmp1A4.is_extensible;
_tmp1A7=_tmp1A2.f2;_tmp1A8=*_tmp1A7;_tmp1A9=_tmp1A8.name;_LLFF: {const char*
_tmp6F5;const char*_tmp6F4;struct Cyc_PP_Doc*_tmp1AA=Cyc_PP_text(_tmp1A6?(_tmp6F5="@extensible datatype ",
_tag_dyneither(_tmp6F5,sizeof(char),22)):((_tmp6F4="datatype ",_tag_dyneither(
_tmp6F4,sizeof(char),10))));{const char*_tmp6F8;struct Cyc_PP_Doc*_tmp6F7[4];s=((
_tmp6F7[3]=Cyc_Absynpp_qvar2doc(_tmp1A9),((_tmp6F7[2]=Cyc_PP_text(((_tmp6F8=".",
_tag_dyneither(_tmp6F8,sizeof(char),2)))),((_tmp6F7[1]=Cyc_Absynpp_qvar2doc(
_tmp1A5),((_tmp6F7[0]=_tmp1AA,Cyc_PP_cat(_tag_dyneither(_tmp6F7,sizeof(struct Cyc_PP_Doc*),
4))))))))));}goto _LLFB;}_LLFB:;}goto _LLC1;_LLD2: {struct Cyc_Absyn_IntType_struct*
_tmp15A=(struct Cyc_Absyn_IntType_struct*)_tmp146;if(_tmp15A->tag != 6)goto _LLD4;
else{_tmp15B=_tmp15A->f1;_tmp15C=_tmp15A->f2;}}_LLD3: {struct _dyneither_ptr sns;
struct _dyneither_ptr ts;switch(_tmp15B){case Cyc_Absyn_None: _LL100: goto _LL101;case
Cyc_Absyn_Signed: _LL101:{const char*_tmp6F9;sns=((_tmp6F9="",_tag_dyneither(
_tmp6F9,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LL102:{const char*
_tmp6FA;sns=((_tmp6FA="unsigned ",_tag_dyneither(_tmp6FA,sizeof(char),10)));}
break;}switch(_tmp15C){case Cyc_Absyn_Char_sz: _LL104: switch(_tmp15B){case Cyc_Absyn_None:
_LL106:{const char*_tmp6FB;sns=((_tmp6FB="",_tag_dyneither(_tmp6FB,sizeof(char),1)));}
break;case Cyc_Absyn_Signed: _LL107:{const char*_tmp6FC;sns=((_tmp6FC="signed ",
_tag_dyneither(_tmp6FC,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL108:{
const char*_tmp6FD;sns=((_tmp6FD="unsigned ",_tag_dyneither(_tmp6FD,sizeof(char),
10)));}break;}{const char*_tmp6FE;ts=((_tmp6FE="char",_tag_dyneither(_tmp6FE,
sizeof(char),5)));}break;case Cyc_Absyn_Short_sz: _LL105:{const char*_tmp6FF;ts=((
_tmp6FF="short",_tag_dyneither(_tmp6FF,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz:
_LL10A:{const char*_tmp700;ts=((_tmp700="int",_tag_dyneither(_tmp700,sizeof(char),
4)));}break;case Cyc_Absyn_Long_sz: _LL10B:{const char*_tmp701;ts=((_tmp701="long",
_tag_dyneither(_tmp701,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL10C:
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL10E:{const char*_tmp702;
ts=((_tmp702="long long",_tag_dyneither(_tmp702,sizeof(char),10)));}break;case
Cyc_Cyclone_Vc_c: _LL10F:{const char*_tmp703;ts=((_tmp703="__int64",_tag_dyneither(
_tmp703,sizeof(char),8)));}break;}break;}{const char*_tmp708;void*_tmp707[2];
struct Cyc_String_pa_struct _tmp706;struct Cyc_String_pa_struct _tmp705;s=Cyc_PP_text((
struct _dyneither_ptr)((_tmp705.tag=0,((_tmp705.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ts),((_tmp706.tag=0,((_tmp706.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)sns),((_tmp707[0]=& _tmp706,((_tmp707[1]=& _tmp705,Cyc_aprintf(((
_tmp708="%s%s",_tag_dyneither(_tmp708,sizeof(char),5))),_tag_dyneither(_tmp707,
sizeof(void*),2)))))))))))))));}goto _LLC1;}_LLD4: {struct Cyc_Absyn_FloatType_struct*
_tmp15D=(struct Cyc_Absyn_FloatType_struct*)_tmp146;if(_tmp15D->tag != 7)goto _LLD6;}
_LLD5:{const char*_tmp709;s=Cyc_PP_text(((_tmp709="float",_tag_dyneither(_tmp709,
sizeof(char),6))));}goto _LLC1;_LLD6: {struct Cyc_Absyn_DoubleType_struct*_tmp15E=(
struct Cyc_Absyn_DoubleType_struct*)_tmp146;if(_tmp15E->tag != 8)goto _LLD8;else{
_tmp15F=_tmp15E->f1;}}_LLD7: if(_tmp15F){const char*_tmp70A;s=Cyc_PP_text(((
_tmp70A="long double",_tag_dyneither(_tmp70A,sizeof(char),12))));}else{const char*
_tmp70B;s=Cyc_PP_text(((_tmp70B="double",_tag_dyneither(_tmp70B,sizeof(char),7))));}
goto _LLC1;_LLD8: {struct Cyc_Absyn_TupleType_struct*_tmp160=(struct Cyc_Absyn_TupleType_struct*)
_tmp146;if(_tmp160->tag != 11)goto _LLDA;else{_tmp161=_tmp160->f1;}}_LLD9:{struct
Cyc_PP_Doc*_tmp70C[2];s=((_tmp70C[1]=Cyc_Absynpp_args2doc(_tmp161),((_tmp70C[0]=
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp70C,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LLC1;_LLDA: {struct Cyc_Absyn_AggrType_struct*_tmp162=(struct Cyc_Absyn_AggrType_struct*)
_tmp146;if(_tmp162->tag != 12)goto _LLDC;else{_tmp163=_tmp162->f1;_tmp164=_tmp163.aggr_info;
_tmp165=_tmp163.targs;}}_LLDB: {enum Cyc_Absyn_AggrKind _tmp1C3;struct _tuple0*
_tmp1C4;struct _tuple9 _tmp1C2=Cyc_Absyn_aggr_kinded_name(_tmp164);_tmp1C3=_tmp1C2.f1;
_tmp1C4=_tmp1C2.f2;{struct Cyc_PP_Doc*_tmp70D[3];s=((_tmp70D[2]=Cyc_Absynpp_tps2doc(
_tmp165),((_tmp70D[1]=Cyc_Absynpp_qvar2doc(_tmp1C4),((_tmp70D[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp1C3),Cyc_PP_cat(_tag_dyneither(_tmp70D,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLC1;}_LLDC: {struct Cyc_Absyn_AnonAggrType_struct*_tmp166=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp146;if(_tmp166->tag != 13)goto _LLDE;else{_tmp167=_tmp166->f1;_tmp168=_tmp166->f2;}}
_LLDD:{struct Cyc_PP_Doc*_tmp70E[4];s=((_tmp70E[3]=Cyc_Absynpp_rb(),((_tmp70E[2]=
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp168)),((_tmp70E[1]=Cyc_Absynpp_lb(),((
_tmp70E[0]=Cyc_Absynpp_aggr_kind2doc(_tmp167),Cyc_PP_cat(_tag_dyneither(_tmp70E,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLC1;_LLDE: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp169=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp146;if(_tmp169->tag != 15)goto
_LLE0;else{_tmp16A=_tmp169->f1;}}_LLDF:{const char*_tmp711;struct Cyc_PP_Doc*
_tmp710[4];s=((_tmp710[3]=Cyc_Absynpp_rb(),((_tmp710[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(
_tmp16A)),((_tmp710[1]=Cyc_Absynpp_lb(),((_tmp710[0]=Cyc_PP_text(((_tmp711="enum ",
_tag_dyneither(_tmp711,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp710,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLC1;_LLE0: {struct Cyc_Absyn_EnumType_struct*
_tmp16B=(struct Cyc_Absyn_EnumType_struct*)_tmp146;if(_tmp16B->tag != 14)goto _LLE2;
else{_tmp16C=_tmp16B->f1;}}_LLE1:{const char*_tmp714;struct Cyc_PP_Doc*_tmp713[2];
s=((_tmp713[1]=Cyc_Absynpp_qvar2doc(_tmp16C),((_tmp713[0]=Cyc_PP_text(((_tmp714="enum ",
_tag_dyneither(_tmp714,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp713,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLC1;_LLE2: {struct Cyc_Absyn_ValueofType_struct*
_tmp16D=(struct Cyc_Absyn_ValueofType_struct*)_tmp146;if(_tmp16D->tag != 19)goto
_LLE4;else{_tmp16E=_tmp16D->f1;}}_LLE3:{const char*_tmp719;const char*_tmp718;
struct Cyc_PP_Doc*_tmp717[3];s=((_tmp717[2]=Cyc_PP_text(((_tmp718=")",
_tag_dyneither(_tmp718,sizeof(char),2)))),((_tmp717[1]=Cyc_Absynpp_exp2doc(
_tmp16E),((_tmp717[0]=Cyc_PP_text(((_tmp719="valueof_t(",_tag_dyneither(_tmp719,
sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp717,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LLC1;_LLE4: {struct Cyc_Absyn_TypedefType_struct*_tmp16F=(struct
Cyc_Absyn_TypedefType_struct*)_tmp146;if(_tmp16F->tag != 18)goto _LLE6;else{
_tmp170=_tmp16F->f1;_tmp171=_tmp16F->f2;_tmp172=_tmp16F->f3;}}_LLE5:{struct Cyc_PP_Doc*
_tmp71A[2];s=((_tmp71A[1]=Cyc_Absynpp_tps2doc(_tmp171),((_tmp71A[0]=Cyc_Absynpp_qvar2doc(
_tmp170),Cyc_PP_cat(_tag_dyneither(_tmp71A,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLC1;_LLE6: {struct Cyc_Absyn_RgnHandleType_struct*_tmp173=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp146;if(_tmp173->tag != 16)goto _LLE8;else{_tmp174=(void*)_tmp173->f1;}}_LLE7:{
const char*_tmp71F;const char*_tmp71E;struct Cyc_PP_Doc*_tmp71D[3];s=((_tmp71D[2]=
Cyc_PP_text(((_tmp71E=">",_tag_dyneither(_tmp71E,sizeof(char),2)))),((_tmp71D[1]=
Cyc_Absynpp_rgn2doc(_tmp174),((_tmp71D[0]=Cyc_PP_text(((_tmp71F="region_t<",
_tag_dyneither(_tmp71F,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp71D,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLC1;_LLE8: {struct Cyc_Absyn_DynRgnType_struct*
_tmp175=(struct Cyc_Absyn_DynRgnType_struct*)_tmp146;if(_tmp175->tag != 17)goto
_LLEA;else{_tmp176=(void*)_tmp175->f1;_tmp177=(void*)_tmp175->f2;}}_LLE9:{const
char*_tmp726;const char*_tmp725;const char*_tmp724;struct Cyc_PP_Doc*_tmp723[5];s=((
_tmp723[4]=Cyc_PP_text(((_tmp724=">",_tag_dyneither(_tmp724,sizeof(char),2)))),((
_tmp723[3]=Cyc_Absynpp_rgn2doc(_tmp177),((_tmp723[2]=Cyc_PP_text(((_tmp725=",",
_tag_dyneither(_tmp725,sizeof(char),2)))),((_tmp723[1]=Cyc_Absynpp_rgn2doc(
_tmp176),((_tmp723[0]=Cyc_PP_text(((_tmp726="dynregion_t<",_tag_dyneither(
_tmp726,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp723,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LLC1;_LLEA: {struct Cyc_Absyn_TagType_struct*_tmp178=(struct
Cyc_Absyn_TagType_struct*)_tmp146;if(_tmp178->tag != 20)goto _LLEC;else{_tmp179=(
void*)_tmp178->f1;}}_LLEB:{const char*_tmp72B;const char*_tmp72A;struct Cyc_PP_Doc*
_tmp729[3];s=((_tmp729[2]=Cyc_PP_text(((_tmp72A=">",_tag_dyneither(_tmp72A,
sizeof(char),2)))),((_tmp729[1]=Cyc_Absynpp_typ2doc(_tmp179),((_tmp729[0]=Cyc_PP_text(((
_tmp72B="tag_t<",_tag_dyneither(_tmp72B,sizeof(char),7)))),Cyc_PP_cat(
_tag_dyneither(_tmp729,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLC1;_LLEC: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp17A=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp146;if(_tmp17A->tag != 22)goto _LLEE;}_LLED: goto _LLEF;_LLEE: {struct Cyc_Absyn_HeapRgn_struct*
_tmp17B=(struct Cyc_Absyn_HeapRgn_struct*)_tmp146;if(_tmp17B->tag != 21)goto _LLF0;}
_LLEF: s=Cyc_Absynpp_rgn2doc(t);goto _LLC1;_LLF0: {struct Cyc_Absyn_RgnsEff_struct*
_tmp17C=(struct Cyc_Absyn_RgnsEff_struct*)_tmp146;if(_tmp17C->tag != 25)goto _LLF2;
else{_tmp17D=(void*)_tmp17C->f1;}}_LLF1:{const char*_tmp730;const char*_tmp72F;
struct Cyc_PP_Doc*_tmp72E[3];s=((_tmp72E[2]=Cyc_PP_text(((_tmp72F=")",
_tag_dyneither(_tmp72F,sizeof(char),2)))),((_tmp72E[1]=Cyc_Absynpp_typ2doc(
_tmp17D),((_tmp72E[0]=Cyc_PP_text(((_tmp730="regions(",_tag_dyneither(_tmp730,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp72E,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLC1;_LLF2: {struct Cyc_Absyn_AccessEff_struct*_tmp17E=(struct Cyc_Absyn_AccessEff_struct*)
_tmp146;if(_tmp17E->tag != 23)goto _LLF4;}_LLF3: goto _LLF5;_LLF4: {struct Cyc_Absyn_JoinEff_struct*
_tmp17F=(struct Cyc_Absyn_JoinEff_struct*)_tmp146;if(_tmp17F->tag != 24)goto _LLC1;}
_LLF5: s=Cyc_Absynpp_eff2doc(t);goto _LLC1;_LLC1:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt*vo);struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct _dyneither_ptr*)vo->v));}
struct _tuple16{void*f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct
_tuple16*cmp);struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
struct _tuple16 _tmp1DD;void*_tmp1DE;void*_tmp1DF;struct _tuple16*_tmp1DC=cmp;
_tmp1DD=*_tmp1DC;_tmp1DE=_tmp1DD.f1;_tmp1DF=_tmp1DD.f2;{const char*_tmp733;struct
Cyc_PP_Doc*_tmp732[3];return(_tmp732[2]=Cyc_Absynpp_rgn2doc(_tmp1DF),((_tmp732[1]=
Cyc_PP_text(((_tmp733=" > ",_tag_dyneither(_tmp733,sizeof(char),4)))),((_tmp732[
0]=Cyc_Absynpp_rgn2doc(_tmp1DE),Cyc_PP_cat(_tag_dyneither(_tmp732,sizeof(struct
Cyc_PP_Doc*),3)))))));};}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*
po);struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){const char*
_tmp736;const char*_tmp735;const char*_tmp734;return Cyc_PP_group(((_tmp734="",
_tag_dyneither(_tmp734,sizeof(char),1))),((_tmp735="",_tag_dyneither(_tmp735,
sizeof(char),1))),((_tmp736=",",_tag_dyneither(_tmp736,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(
struct _tuple7*t);struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple7*t){struct
Cyc_Core_Opt*_tmp737;struct Cyc_Core_Opt*dopt=(*t).f1 == 0?0:((_tmp737=_cycalloc(
sizeof(*_tmp737)),((_tmp737->v=Cyc_PP_text(*((struct _dyneither_ptr*)((struct Cyc_Core_Opt*)
_check_null((*t).f1))->v)),_tmp737))));return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,
dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*
effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmp1E6=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,
args);struct Cyc_PP_Doc*eff_doc;if(c_varargs){const char*_tmp73A;struct Cyc_List_List*
_tmp739;_tmp1E6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1E6,((_tmp739=_cycalloc(sizeof(*_tmp739)),((_tmp739->hd=
Cyc_PP_text(((_tmp73A="...",_tag_dyneither(_tmp73A,sizeof(char),4)))),((_tmp739->tl=
0,_tmp739)))))));}else{if(cyc_varargs != 0){const char*_tmp743;const char*_tmp742;
const char*_tmp741;struct _tuple7*_tmp740;struct Cyc_PP_Doc*_tmp73F[3];struct Cyc_PP_Doc*
_tmp1E9=(_tmp73F[2]=Cyc_Absynpp_funarg2doc(((_tmp740=_cycalloc(sizeof(*_tmp740)),((
_tmp740->f1=cyc_varargs->name,((_tmp740->f2=cyc_varargs->tq,((_tmp740->f3=
cyc_varargs->type,_tmp740))))))))),((_tmp73F[1]=cyc_varargs->inject?Cyc_PP_text(((
_tmp741=" inject ",_tag_dyneither(_tmp741,sizeof(char),9)))): Cyc_PP_text(((
_tmp742=" ",_tag_dyneither(_tmp742,sizeof(char),2)))),((_tmp73F[0]=Cyc_PP_text(((
_tmp743="...",_tag_dyneither(_tmp743,sizeof(char),4)))),Cyc_PP_cat(
_tag_dyneither(_tmp73F,sizeof(struct Cyc_PP_Doc*),3)))))));struct Cyc_List_List*
_tmp744;_tmp1E6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1E6,((_tmp744=_cycalloc(sizeof(*_tmp744)),((_tmp744->hd=
_tmp1E9,((_tmp744->tl=0,_tmp744)))))));}}{const char*_tmp747;const char*_tmp746;
const char*_tmp745;struct Cyc_PP_Doc*_tmp1F0=Cyc_PP_group(((_tmp745="",
_tag_dyneither(_tmp745,sizeof(char),1))),((_tmp746="",_tag_dyneither(_tmp746,
sizeof(char),1))),((_tmp747=",",_tag_dyneither(_tmp747,sizeof(char),2))),_tmp1E6);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){const char*_tmp74A;struct Cyc_PP_Doc*
_tmp749[3];_tmp1F0=((_tmp749[2]=Cyc_Absynpp_eff2doc((void*)effopt->v),((_tmp749[
1]=Cyc_PP_text(((_tmp74A=";",_tag_dyneither(_tmp74A,sizeof(char),2)))),((_tmp749[
0]=_tmp1F0,Cyc_PP_cat(_tag_dyneither(_tmp749,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){const char*_tmp74D;struct Cyc_PP_Doc*_tmp74C[3];_tmp1F0=((_tmp74C[2]=
Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp74C[1]=Cyc_PP_text(((_tmp74D=":",
_tag_dyneither(_tmp74D,sizeof(char),2)))),((_tmp74C[0]=_tmp1F0,Cyc_PP_cat(
_tag_dyneither(_tmp74C,sizeof(struct Cyc_PP_Doc*),3))))))));}{const char*_tmp752;
const char*_tmp751;struct Cyc_PP_Doc*_tmp750[3];return(_tmp750[2]=Cyc_PP_text(((
_tmp751=")",_tag_dyneither(_tmp751,sizeof(char),2)))),((_tmp750[1]=_tmp1F0,((
_tmp750[0]=Cyc_PP_text(((_tmp752="(",_tag_dyneither(_tmp752,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp750,sizeof(struct Cyc_PP_Doc*),3)))))));};};}struct
_tuple7*Cyc_Absynpp_arg_mk_opt(struct _tuple11*arg);struct _tuple7*Cyc_Absynpp_arg_mk_opt(
struct _tuple11*arg){struct Cyc_Core_Opt*_tmp755;struct _tuple7*_tmp754;return(
_tmp754=_cycalloc(sizeof(*_tmp754)),((_tmp754->f1=((_tmp755=_cycalloc(sizeof(*
_tmp755)),((_tmp755->v=(*arg).f1,_tmp755)))),((_tmp754->f2=(*arg).f2,((_tmp754->f3=(*
arg).f3,_tmp754)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct
_dyneither_ptr*v);struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){
return Cyc_PP_text(*v);}struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*
q);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){struct Cyc_List_List*
_tmp1FD=0;int match;{union Cyc_Absyn_Nmspace _tmp1FE=(*q).f1;int _tmp1FF;struct Cyc_List_List*
_tmp200;struct Cyc_List_List*_tmp201;_LL112: if((_tmp1FE.Loc_n).tag != 3)goto _LL114;
_tmp1FF=(int)(_tmp1FE.Loc_n).val;_LL113: _tmp200=0;goto _LL115;_LL114: if((_tmp1FE.Rel_n).tag
!= 1)goto _LL116;_tmp200=(struct Cyc_List_List*)(_tmp1FE.Rel_n).val;_LL115: match=0;
_tmp1FD=_tmp200;goto _LL111;_LL116: if((_tmp1FE.Abs_n).tag != 2)goto _LL111;_tmp201=(
struct Cyc_List_List*)(_tmp1FE.Abs_n).val;_LL117: match=Cyc_Absynpp_use_curr_namespace
 && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp201,Cyc_Absynpp_curr_namespace);{
struct Cyc_List_List*_tmp756;_tmp1FD=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(
_tmp756=_cycalloc(sizeof(*_tmp756)),((_tmp756->hd=Cyc_Absynpp_cyc_stringptr,((
_tmp756->tl=_tmp201,_tmp756))))): _tmp201;}goto _LL111;_LL111:;}if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp758;struct Cyc_List_List*_tmp757;return(struct _dyneither_ptr)Cyc_str_sepstr(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1FD,((_tmp757=_cycalloc(sizeof(*_tmp757)),((_tmp757->hd=(*q).f2,((_tmp757->tl=
0,_tmp757))))))),((_tmp758="_",_tag_dyneither(_tmp758,sizeof(char),2))));}else{
if(match)return*(*q).f2;else{const char*_tmp75A;struct Cyc_List_List*_tmp759;
return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1FD,((_tmp759=_cycalloc(sizeof(*
_tmp759)),((_tmp759->hd=(*q).f2,((_tmp759->tl=0,_tmp759))))))),((_tmp75A="::",
_tag_dyneither(_tmp75A,sizeof(char),3))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){return
Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp207=Cyc_Absynpp_qvar2string(q);if(Cyc_PP_tex_output){
const char*_tmp75C;const char*_tmp75B;return Cyc_PP_text_width((struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp75B="\\textbf{",
_tag_dyneither(_tmp75B,sizeof(char),9))),(struct _dyneither_ptr)_tmp207),((
_tmp75C="}",_tag_dyneither(_tmp75C,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp207));}else{return Cyc_PP_text(_tmp207);}}struct _dyneither_ptr
Cyc_Absynpp_typedef_name2string(struct _tuple0*v);struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(
struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
if(Cyc_Absynpp_use_curr_namespace){union Cyc_Absyn_Nmspace _tmp20A=(*v).f1;int
_tmp20B;struct Cyc_List_List*_tmp20C;struct Cyc_List_List*_tmp20D;_LL119: if((
_tmp20A.Loc_n).tag != 3)goto _LL11B;_tmp20B=(int)(_tmp20A.Loc_n).val;_LL11A: goto
_LL11C;_LL11B: if((_tmp20A.Rel_n).tag != 1)goto _LL11D;_tmp20C=(struct Cyc_List_List*)(
_tmp20A.Rel_n).val;if(_tmp20C != 0)goto _LL11D;_LL11C: return*(*v).f2;_LL11D: if((
_tmp20A.Abs_n).tag != 2)goto _LL11F;_tmp20D=(struct Cyc_List_List*)(_tmp20A.Abs_n).val;
_LL11E: if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct
Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,
_tmp20D,Cyc_Absynpp_curr_namespace)== 0)return*(*v).f2;else{goto _LL120;}_LL11F:;
_LL120: {const char*_tmp75D;return(struct _dyneither_ptr)Cyc_strconcat(((_tmp75D="/* bad namespace : */ ",
_tag_dyneither(_tmp75D,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
v));}_LL118:;}else{return*(*v).f2;}}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*v);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*
v){struct _dyneither_ptr _tmp20F=Cyc_Absynpp_typedef_name2string(v);if(Cyc_PP_tex_output){
const char*_tmp75F;const char*_tmp75E;return Cyc_PP_text_width((struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp75E="\\textbf{",
_tag_dyneither(_tmp75E,sizeof(char),9))),(struct _dyneither_ptr)_tmp20F),((
_tmp75F="}",_tag_dyneither(_tmp75F,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp20F));}else{return Cyc_PP_text(_tmp20F);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(0),t,0);}int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e);
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){void*_tmp212=e->r;enum Cyc_Absyn_Primop
_tmp217;struct Cyc_Absyn_Exp*_tmp222;struct Cyc_Absyn_Exp*_tmp224;_LL122: {struct
Cyc_Absyn_Const_e_struct*_tmp213=(struct Cyc_Absyn_Const_e_struct*)_tmp212;if(
_tmp213->tag != 0)goto _LL124;}_LL123: goto _LL125;_LL124: {struct Cyc_Absyn_Var_e_struct*
_tmp214=(struct Cyc_Absyn_Var_e_struct*)_tmp212;if(_tmp214->tag != 1)goto _LL126;}
_LL125: goto _LL127;_LL126: {struct Cyc_Absyn_UnknownId_e_struct*_tmp215=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp212;if(_tmp215->tag != 2)goto _LL128;}_LL127: return 10000;_LL128: {struct Cyc_Absyn_Primop_e_struct*
_tmp216=(struct Cyc_Absyn_Primop_e_struct*)_tmp212;if(_tmp216->tag != 3)goto _LL12A;
else{_tmp217=_tmp216->f1;}}_LL129: switch(_tmp217){case Cyc_Absyn_Plus: _LL172:
return 100;case Cyc_Absyn_Times: _LL173: return 110;case Cyc_Absyn_Minus: _LL174: return
100;case Cyc_Absyn_Div: _LL175: goto _LL176;case Cyc_Absyn_Mod: _LL176: return 110;case
Cyc_Absyn_Eq: _LL177: goto _LL178;case Cyc_Absyn_Neq: _LL178: return 70;case Cyc_Absyn_Gt:
_LL179: goto _LL17A;case Cyc_Absyn_Lt: _LL17A: goto _LL17B;case Cyc_Absyn_Gte: _LL17B:
goto _LL17C;case Cyc_Absyn_Lte: _LL17C: return 80;case Cyc_Absyn_Not: _LL17D: goto _LL17E;
case Cyc_Absyn_Bitnot: _LL17E: return 130;case Cyc_Absyn_Bitand: _LL17F: return 60;case
Cyc_Absyn_Bitor: _LL180: return 40;case Cyc_Absyn_Bitxor: _LL181: return 50;case Cyc_Absyn_Bitlshift:
_LL182: return 90;case Cyc_Absyn_Bitlrshift: _LL183: return 80;case Cyc_Absyn_Bitarshift:
_LL184: return 80;case Cyc_Absyn_Numelts: _LL185: return 140;}_LL12A: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp218=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp212;if(_tmp218->tag != 4)goto
_LL12C;}_LL12B: return 20;_LL12C: {struct Cyc_Absyn_Increment_e_struct*_tmp219=(
struct Cyc_Absyn_Increment_e_struct*)_tmp212;if(_tmp219->tag != 5)goto _LL12E;}
_LL12D: return 130;_LL12E: {struct Cyc_Absyn_Conditional_e_struct*_tmp21A=(struct
Cyc_Absyn_Conditional_e_struct*)_tmp212;if(_tmp21A->tag != 6)goto _LL130;}_LL12F:
return 30;_LL130: {struct Cyc_Absyn_And_e_struct*_tmp21B=(struct Cyc_Absyn_And_e_struct*)
_tmp212;if(_tmp21B->tag != 7)goto _LL132;}_LL131: return 35;_LL132: {struct Cyc_Absyn_Or_e_struct*
_tmp21C=(struct Cyc_Absyn_Or_e_struct*)_tmp212;if(_tmp21C->tag != 8)goto _LL134;}
_LL133: return 30;_LL134: {struct Cyc_Absyn_SeqExp_e_struct*_tmp21D=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp212;if(_tmp21D->tag != 9)goto _LL136;}_LL135: return 10;_LL136: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp21E=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp212;if(_tmp21E->tag != 10)goto
_LL138;}_LL137: goto _LL139;_LL138: {struct Cyc_Absyn_FnCall_e_struct*_tmp21F=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp212;if(_tmp21F->tag != 11)goto _LL13A;}_LL139:
return 140;_LL13A: {struct Cyc_Absyn_Throw_e_struct*_tmp220=(struct Cyc_Absyn_Throw_e_struct*)
_tmp212;if(_tmp220->tag != 12)goto _LL13C;}_LL13B: return 130;_LL13C: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp221=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp212;if(_tmp221->tag != 13)
goto _LL13E;else{_tmp222=_tmp221->f1;}}_LL13D: return Cyc_Absynpp_exp_prec(_tmp222);
_LL13E: {struct Cyc_Absyn_Instantiate_e_struct*_tmp223=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp212;if(_tmp223->tag != 14)goto _LL140;else{_tmp224=_tmp223->f1;}}_LL13F: return
Cyc_Absynpp_exp_prec(_tmp224);_LL140: {struct Cyc_Absyn_Cast_e_struct*_tmp225=(
struct Cyc_Absyn_Cast_e_struct*)_tmp212;if(_tmp225->tag != 15)goto _LL142;}_LL141:
return 120;_LL142: {struct Cyc_Absyn_New_e_struct*_tmp226=(struct Cyc_Absyn_New_e_struct*)
_tmp212;if(_tmp226->tag != 17)goto _LL144;}_LL143: return 15;_LL144: {struct Cyc_Absyn_Address_e_struct*
_tmp227=(struct Cyc_Absyn_Address_e_struct*)_tmp212;if(_tmp227->tag != 16)goto
_LL146;}_LL145: goto _LL147;_LL146: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp228=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp212;if(_tmp228->tag != 18)goto _LL148;}
_LL147: goto _LL149;_LL148: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp229=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp212;if(_tmp229->tag != 19)goto _LL14A;}_LL149: goto _LL14B;_LL14A: {struct Cyc_Absyn_Valueof_e_struct*
_tmp22A=(struct Cyc_Absyn_Valueof_e_struct*)_tmp212;if(_tmp22A->tag != 39)goto
_LL14C;}_LL14B: goto _LL14D;_LL14C: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp22B=(
struct Cyc_Absyn_Tagcheck_e_struct*)_tmp212;if(_tmp22B->tag != 38)goto _LL14E;}
_LL14D: goto _LL14F;_LL14E: {struct Cyc_Absyn_Offsetof_e_struct*_tmp22C=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp212;if(_tmp22C->tag != 20)goto _LL150;}_LL14F: goto _LL151;_LL150: {struct Cyc_Absyn_Deref_e_struct*
_tmp22D=(struct Cyc_Absyn_Deref_e_struct*)_tmp212;if(_tmp22D->tag != 21)goto _LL152;}
_LL151: return 130;_LL152: {struct Cyc_Absyn_AggrMember_e_struct*_tmp22E=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp212;if(_tmp22E->tag != 22)goto _LL154;}_LL153: goto _LL155;_LL154: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp22F=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp212;if(_tmp22F->tag != 23)goto
_LL156;}_LL155: goto _LL157;_LL156: {struct Cyc_Absyn_Subscript_e_struct*_tmp230=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp212;if(_tmp230->tag != 24)goto _LL158;}
_LL157: return 140;_LL158: {struct Cyc_Absyn_Tuple_e_struct*_tmp231=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp212;if(_tmp231->tag != 25)goto _LL15A;}_LL159: return 150;_LL15A: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp232=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp212;if(_tmp232->tag != 26)goto
_LL15C;}_LL15B: goto _LL15D;_LL15C: {struct Cyc_Absyn_Array_e_struct*_tmp233=(
struct Cyc_Absyn_Array_e_struct*)_tmp212;if(_tmp233->tag != 27)goto _LL15E;}_LL15D:
goto _LL15F;_LL15E: {struct Cyc_Absyn_Comprehension_e_struct*_tmp234=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp212;if(_tmp234->tag != 28)goto _LL160;}_LL15F: goto _LL161;_LL160: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp235=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp212;if(_tmp235->tag != 29)goto
_LL162;}_LL161: goto _LL163;_LL162: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp236=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp212;if(_tmp236->tag != 30)goto _LL164;}
_LL163: goto _LL165;_LL164: {struct Cyc_Absyn_Datatype_e_struct*_tmp237=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp212;if(_tmp237->tag != 31)goto _LL166;}_LL165: goto _LL167;_LL166: {struct Cyc_Absyn_Enum_e_struct*
_tmp238=(struct Cyc_Absyn_Enum_e_struct*)_tmp212;if(_tmp238->tag != 32)goto _LL168;}
_LL167: goto _LL169;_LL168: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp239=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp212;if(_tmp239->tag != 33)goto _LL16A;}_LL169: goto _LL16B;_LL16A: {struct Cyc_Absyn_Malloc_e_struct*
_tmp23A=(struct Cyc_Absyn_Malloc_e_struct*)_tmp212;if(_tmp23A->tag != 34)goto
_LL16C;}_LL16B: goto _LL16D;_LL16C: {struct Cyc_Absyn_Swap_e_struct*_tmp23B=(struct
Cyc_Absyn_Swap_e_struct*)_tmp212;if(_tmp23B->tag != 35)goto _LL16E;}_LL16D: goto
_LL16F;_LL16E: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp23C=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp212;if(_tmp23C->tag != 36)goto _LL170;}_LL16F: return 140;_LL170: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp23D=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp212;if(_tmp23D->tag != 37)goto
_LL121;}_LL171: return 10000;_LL121:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct
Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int
inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*
s;{void*_tmp23E=e->r;union Cyc_Absyn_Cnst _tmp240;struct _tuple0*_tmp242;struct
_tuple0*_tmp244;enum Cyc_Absyn_Primop _tmp246;struct Cyc_List_List*_tmp247;struct
Cyc_Absyn_Exp*_tmp249;struct Cyc_Core_Opt*_tmp24A;struct Cyc_Absyn_Exp*_tmp24B;
struct Cyc_Absyn_Exp*_tmp24D;enum Cyc_Absyn_Incrementor _tmp24E;struct Cyc_Absyn_Exp*
_tmp250;struct Cyc_Absyn_Exp*_tmp251;struct Cyc_Absyn_Exp*_tmp252;struct Cyc_Absyn_Exp*
_tmp254;struct Cyc_Absyn_Exp*_tmp255;struct Cyc_Absyn_Exp*_tmp257;struct Cyc_Absyn_Exp*
_tmp258;struct Cyc_Absyn_Exp*_tmp25A;struct Cyc_Absyn_Exp*_tmp25B;struct Cyc_Absyn_Exp*
_tmp25D;struct Cyc_List_List*_tmp25E;struct Cyc_Absyn_Exp*_tmp260;struct Cyc_List_List*
_tmp261;struct Cyc_Absyn_Exp*_tmp263;struct Cyc_Absyn_Exp*_tmp265;struct Cyc_Absyn_Exp*
_tmp267;void*_tmp269;struct Cyc_Absyn_Exp*_tmp26A;struct Cyc_Absyn_Exp*_tmp26C;
struct Cyc_Absyn_Exp*_tmp26E;struct Cyc_Absyn_Exp*_tmp26F;void*_tmp271;struct Cyc_Absyn_Exp*
_tmp273;void*_tmp275;struct Cyc_Absyn_Exp*_tmp277;struct _dyneither_ptr*_tmp278;
void*_tmp27A;void*_tmp27B;struct _dyneither_ptr*_tmp27D;void*_tmp27F;void*_tmp280;
unsigned int _tmp282;struct Cyc_Absyn_Exp*_tmp284;struct Cyc_Absyn_Exp*_tmp286;
struct _dyneither_ptr*_tmp287;struct Cyc_Absyn_Exp*_tmp289;struct _dyneither_ptr*
_tmp28A;struct Cyc_Absyn_Exp*_tmp28C;struct Cyc_Absyn_Exp*_tmp28D;struct Cyc_List_List*
_tmp28F;struct _tuple7*_tmp291;struct Cyc_List_List*_tmp292;struct Cyc_List_List*
_tmp294;struct Cyc_Absyn_Vardecl*_tmp296;struct Cyc_Absyn_Exp*_tmp297;struct Cyc_Absyn_Exp*
_tmp298;struct _tuple0*_tmp29A;struct Cyc_List_List*_tmp29B;struct Cyc_List_List*
_tmp29C;struct Cyc_List_List*_tmp29E;struct Cyc_List_List*_tmp2A0;struct Cyc_Absyn_Datatypefield*
_tmp2A1;struct _tuple0*_tmp2A3;struct _tuple0*_tmp2A5;struct Cyc_Absyn_MallocInfo
_tmp2A7;int _tmp2A8;struct Cyc_Absyn_Exp*_tmp2A9;void**_tmp2AA;struct Cyc_Absyn_Exp*
_tmp2AB;struct Cyc_Absyn_Exp*_tmp2AD;struct Cyc_Absyn_Exp*_tmp2AE;struct Cyc_Core_Opt*
_tmp2B0;struct Cyc_List_List*_tmp2B1;struct Cyc_Absyn_Stmt*_tmp2B3;_LL188: {struct
Cyc_Absyn_Const_e_struct*_tmp23F=(struct Cyc_Absyn_Const_e_struct*)_tmp23E;if(
_tmp23F->tag != 0)goto _LL18A;else{_tmp240=_tmp23F->f1;}}_LL189: s=Cyc_Absynpp_cnst2doc(
_tmp240);goto _LL187;_LL18A: {struct Cyc_Absyn_Var_e_struct*_tmp241=(struct Cyc_Absyn_Var_e_struct*)
_tmp23E;if(_tmp241->tag != 1)goto _LL18C;else{_tmp242=_tmp241->f1;}}_LL18B: _tmp244=
_tmp242;goto _LL18D;_LL18C: {struct Cyc_Absyn_UnknownId_e_struct*_tmp243=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp23E;if(_tmp243->tag != 2)goto _LL18E;else{
_tmp244=_tmp243->f1;}}_LL18D: s=Cyc_Absynpp_qvar2doc(_tmp244);goto _LL187;_LL18E: {
struct Cyc_Absyn_Primop_e_struct*_tmp245=(struct Cyc_Absyn_Primop_e_struct*)
_tmp23E;if(_tmp245->tag != 3)goto _LL190;else{_tmp246=_tmp245->f1;_tmp247=_tmp245->f2;}}
_LL18F: s=Cyc_Absynpp_primapp2doc(myprec,_tmp246,_tmp247);goto _LL187;_LL190: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp248=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp23E;if(_tmp248->tag != 4)goto _LL192;else{_tmp249=_tmp248->f1;_tmp24A=_tmp248->f2;
_tmp24B=_tmp248->f3;}}_LL191:{const char*_tmp764;const char*_tmp763;struct Cyc_PP_Doc*
_tmp762[5];s=((_tmp762[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp24B),((_tmp762[3]=
Cyc_PP_text(((_tmp763="= ",_tag_dyneither(_tmp763,sizeof(char),3)))),((_tmp762[2]=
_tmp24A == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp24A->v),((
_tmp762[1]=Cyc_PP_text(((_tmp764=" ",_tag_dyneither(_tmp764,sizeof(char),2)))),((
_tmp762[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp249),Cyc_PP_cat(_tag_dyneither(
_tmp762,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL187;_LL192: {struct Cyc_Absyn_Increment_e_struct*
_tmp24C=(struct Cyc_Absyn_Increment_e_struct*)_tmp23E;if(_tmp24C->tag != 5)goto
_LL194;else{_tmp24D=_tmp24C->f1;_tmp24E=_tmp24C->f2;}}_LL193: {struct Cyc_PP_Doc*
_tmp2B7=Cyc_Absynpp_exp2doc_prec(myprec,_tmp24D);switch(_tmp24E){case Cyc_Absyn_PreInc:
_LL1DA:{const char*_tmp767;struct Cyc_PP_Doc*_tmp766[2];s=((_tmp766[1]=_tmp2B7,((
_tmp766[0]=Cyc_PP_text(((_tmp767="++",_tag_dyneither(_tmp767,sizeof(char),3)))),
Cyc_PP_cat(_tag_dyneither(_tmp766,sizeof(struct Cyc_PP_Doc*),2))))));}break;case
Cyc_Absyn_PreDec: _LL1DB:{const char*_tmp76A;struct Cyc_PP_Doc*_tmp769[2];s=((
_tmp769[1]=_tmp2B7,((_tmp769[0]=Cyc_PP_text(((_tmp76A="--",_tag_dyneither(
_tmp76A,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp769,sizeof(struct Cyc_PP_Doc*),
2))))));}break;case Cyc_Absyn_PostInc: _LL1DC:{const char*_tmp76D;struct Cyc_PP_Doc*
_tmp76C[2];s=((_tmp76C[1]=Cyc_PP_text(((_tmp76D="++",_tag_dyneither(_tmp76D,
sizeof(char),3)))),((_tmp76C[0]=_tmp2B7,Cyc_PP_cat(_tag_dyneither(_tmp76C,
sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1DD:{const char*
_tmp770;struct Cyc_PP_Doc*_tmp76F[2];s=((_tmp76F[1]=Cyc_PP_text(((_tmp770="--",
_tag_dyneither(_tmp770,sizeof(char),3)))),((_tmp76F[0]=_tmp2B7,Cyc_PP_cat(
_tag_dyneither(_tmp76F,sizeof(struct Cyc_PP_Doc*),2))))));}break;}goto _LL187;}
_LL194: {struct Cyc_Absyn_Conditional_e_struct*_tmp24F=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp23E;if(_tmp24F->tag != 6)goto _LL196;else{_tmp250=_tmp24F->f1;_tmp251=_tmp24F->f2;
_tmp252=_tmp24F->f3;}}_LL195:{const char*_tmp775;const char*_tmp774;struct Cyc_PP_Doc*
_tmp773[5];s=((_tmp773[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp252),((_tmp773[3]=
Cyc_PP_text(((_tmp774=" : ",_tag_dyneither(_tmp774,sizeof(char),4)))),((_tmp773[
2]=Cyc_Absynpp_exp2doc_prec(0,_tmp251),((_tmp773[1]=Cyc_PP_text(((_tmp775=" ? ",
_tag_dyneither(_tmp775,sizeof(char),4)))),((_tmp773[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp250),Cyc_PP_cat(_tag_dyneither(_tmp773,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL187;_LL196: {struct Cyc_Absyn_And_e_struct*_tmp253=(struct Cyc_Absyn_And_e_struct*)
_tmp23E;if(_tmp253->tag != 7)goto _LL198;else{_tmp254=_tmp253->f1;_tmp255=_tmp253->f2;}}
_LL197:{const char*_tmp778;struct Cyc_PP_Doc*_tmp777[3];s=((_tmp777[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp255),((_tmp777[1]=Cyc_PP_text(((_tmp778=" && ",_tag_dyneither(_tmp778,
sizeof(char),5)))),((_tmp777[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp254),Cyc_PP_cat(
_tag_dyneither(_tmp777,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL187;_LL198: {
struct Cyc_Absyn_Or_e_struct*_tmp256=(struct Cyc_Absyn_Or_e_struct*)_tmp23E;if(
_tmp256->tag != 8)goto _LL19A;else{_tmp257=_tmp256->f1;_tmp258=_tmp256->f2;}}
_LL199:{const char*_tmp77B;struct Cyc_PP_Doc*_tmp77A[3];s=((_tmp77A[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp258),((_tmp77A[1]=Cyc_PP_text(((_tmp77B=" || ",_tag_dyneither(_tmp77B,
sizeof(char),5)))),((_tmp77A[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp257),Cyc_PP_cat(
_tag_dyneither(_tmp77A,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL187;_LL19A: {
struct Cyc_Absyn_SeqExp_e_struct*_tmp259=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp23E;if(_tmp259->tag != 9)goto _LL19C;else{_tmp25A=_tmp259->f1;_tmp25B=_tmp259->f2;}}
_LL19B:{const char*_tmp782;const char*_tmp781;const char*_tmp780;struct Cyc_PP_Doc*
_tmp77F[5];s=((_tmp77F[4]=Cyc_PP_text(((_tmp780=")",_tag_dyneither(_tmp780,
sizeof(char),2)))),((_tmp77F[3]=Cyc_Absynpp_exp2doc(_tmp25B),((_tmp77F[2]=Cyc_PP_text(((
_tmp781=", ",_tag_dyneither(_tmp781,sizeof(char),3)))),((_tmp77F[1]=Cyc_Absynpp_exp2doc(
_tmp25A),((_tmp77F[0]=Cyc_PP_text(((_tmp782="(",_tag_dyneither(_tmp782,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp77F,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL187;_LL19C: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp25C=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp23E;if(_tmp25C->tag != 10)goto _LL19E;else{_tmp25D=_tmp25C->f1;_tmp25E=_tmp25C->f2;}}
_LL19D:{const char*_tmp787;const char*_tmp786;struct Cyc_PP_Doc*_tmp785[4];s=((
_tmp785[3]=Cyc_PP_text(((_tmp786=")",_tag_dyneither(_tmp786,sizeof(char),2)))),((
_tmp785[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp25E),((_tmp785[1]=Cyc_PP_text(((
_tmp787="(",_tag_dyneither(_tmp787,sizeof(char),2)))),((_tmp785[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp25D),Cyc_PP_cat(_tag_dyneither(_tmp785,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL187;_LL19E: {struct Cyc_Absyn_FnCall_e_struct*_tmp25F=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp23E;if(_tmp25F->tag != 11)goto _LL1A0;else{_tmp260=_tmp25F->f1;_tmp261=_tmp25F->f2;}}
_LL19F:{const char*_tmp78C;const char*_tmp78B;struct Cyc_PP_Doc*_tmp78A[4];s=((
_tmp78A[3]=Cyc_PP_text(((_tmp78B=")",_tag_dyneither(_tmp78B,sizeof(char),2)))),((
_tmp78A[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp261),((_tmp78A[1]=Cyc_PP_text(((
_tmp78C="(",_tag_dyneither(_tmp78C,sizeof(char),2)))),((_tmp78A[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp260),Cyc_PP_cat(_tag_dyneither(_tmp78A,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL187;_LL1A0: {struct Cyc_Absyn_Throw_e_struct*_tmp262=(struct Cyc_Absyn_Throw_e_struct*)
_tmp23E;if(_tmp262->tag != 12)goto _LL1A2;else{_tmp263=_tmp262->f1;}}_LL1A1:{const
char*_tmp78F;struct Cyc_PP_Doc*_tmp78E[2];s=((_tmp78E[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp263),((_tmp78E[0]=Cyc_PP_text(((_tmp78F="throw ",_tag_dyneither(
_tmp78F,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp78E,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL187;_LL1A2: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp264=(
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp23E;if(_tmp264->tag != 13)goto _LL1A4;
else{_tmp265=_tmp264->f1;}}_LL1A3: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp265);
goto _LL187;_LL1A4: {struct Cyc_Absyn_Instantiate_e_struct*_tmp266=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp23E;if(_tmp266->tag != 14)goto _LL1A6;else{_tmp267=_tmp266->f1;}}_LL1A5: s=Cyc_Absynpp_exp2doc_prec(
inprec,_tmp267);goto _LL187;_LL1A6: {struct Cyc_Absyn_Cast_e_struct*_tmp268=(
struct Cyc_Absyn_Cast_e_struct*)_tmp23E;if(_tmp268->tag != 15)goto _LL1A8;else{
_tmp269=(void*)_tmp268->f1;_tmp26A=_tmp268->f2;}}_LL1A7:{const char*_tmp794;const
char*_tmp793;struct Cyc_PP_Doc*_tmp792[4];s=((_tmp792[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp26A),((_tmp792[2]=Cyc_PP_text(((_tmp793=")",_tag_dyneither(_tmp793,
sizeof(char),2)))),((_tmp792[1]=Cyc_Absynpp_typ2doc(_tmp269),((_tmp792[0]=Cyc_PP_text(((
_tmp794="(",_tag_dyneither(_tmp794,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp792,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL187;_LL1A8: {struct Cyc_Absyn_Address_e_struct*
_tmp26B=(struct Cyc_Absyn_Address_e_struct*)_tmp23E;if(_tmp26B->tag != 16)goto
_LL1AA;else{_tmp26C=_tmp26B->f1;}}_LL1A9:{const char*_tmp797;struct Cyc_PP_Doc*
_tmp796[2];s=((_tmp796[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp26C),((_tmp796[0]=
Cyc_PP_text(((_tmp797="&",_tag_dyneither(_tmp797,sizeof(char),2)))),Cyc_PP_cat(
_tag_dyneither(_tmp796,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL187;_LL1AA: {
struct Cyc_Absyn_New_e_struct*_tmp26D=(struct Cyc_Absyn_New_e_struct*)_tmp23E;if(
_tmp26D->tag != 17)goto _LL1AC;else{_tmp26E=_tmp26D->f1;_tmp26F=_tmp26D->f2;}}
_LL1AB: if(_tmp26E == 0){const char*_tmp79A;struct Cyc_PP_Doc*_tmp799[2];s=((_tmp799[
1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp26F),((_tmp799[0]=Cyc_PP_text(((_tmp79A="new ",
_tag_dyneither(_tmp79A,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp799,
sizeof(struct Cyc_PP_Doc*),2))))));}else{const char*_tmp79F;const char*_tmp79E;
struct Cyc_PP_Doc*_tmp79D[4];s=((_tmp79D[3]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp26F),((_tmp79D[2]=Cyc_PP_text(((_tmp79E=") ",_tag_dyneither(_tmp79E,sizeof(
char),3)))),((_tmp79D[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp26E),((
_tmp79D[0]=Cyc_PP_text(((_tmp79F="rnew(",_tag_dyneither(_tmp79F,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp79D,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL187;_LL1AC: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp270=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp23E;if(_tmp270->tag != 18)goto _LL1AE;else{_tmp271=(void*)_tmp270->f1;}}_LL1AD:{
const char*_tmp7A4;const char*_tmp7A3;struct Cyc_PP_Doc*_tmp7A2[3];s=((_tmp7A2[2]=
Cyc_PP_text(((_tmp7A3=")",_tag_dyneither(_tmp7A3,sizeof(char),2)))),((_tmp7A2[1]=
Cyc_Absynpp_typ2doc(_tmp271),((_tmp7A2[0]=Cyc_PP_text(((_tmp7A4="sizeof(",
_tag_dyneither(_tmp7A4,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7A2,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL187;_LL1AE: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp272=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp23E;if(_tmp272->tag != 19)goto
_LL1B0;else{_tmp273=_tmp272->f1;}}_LL1AF:{const char*_tmp7A9;const char*_tmp7A8;
struct Cyc_PP_Doc*_tmp7A7[3];s=((_tmp7A7[2]=Cyc_PP_text(((_tmp7A8=")",
_tag_dyneither(_tmp7A8,sizeof(char),2)))),((_tmp7A7[1]=Cyc_Absynpp_exp2doc(
_tmp273),((_tmp7A7[0]=Cyc_PP_text(((_tmp7A9="sizeof(",_tag_dyneither(_tmp7A9,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7A7,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL187;_LL1B0: {struct Cyc_Absyn_Valueof_e_struct*_tmp274=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp23E;if(_tmp274->tag != 39)goto _LL1B2;else{_tmp275=(void*)_tmp274->f1;}}_LL1B1:{
const char*_tmp7AE;const char*_tmp7AD;struct Cyc_PP_Doc*_tmp7AC[3];s=((_tmp7AC[2]=
Cyc_PP_text(((_tmp7AD=")",_tag_dyneither(_tmp7AD,sizeof(char),2)))),((_tmp7AC[1]=
Cyc_Absynpp_typ2doc(_tmp275),((_tmp7AC[0]=Cyc_PP_text(((_tmp7AE="valueof(",
_tag_dyneither(_tmp7AE,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7AC,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL187;_LL1B2: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp276=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp23E;if(_tmp276->tag != 38)goto
_LL1B4;else{_tmp277=_tmp276->f1;_tmp278=_tmp276->f2;}}_LL1B3:{const char*_tmp7B5;
const char*_tmp7B4;const char*_tmp7B3;struct Cyc_PP_Doc*_tmp7B2[5];s=((_tmp7B2[4]=
Cyc_PP_text(((_tmp7B3=")",_tag_dyneither(_tmp7B3,sizeof(char),2)))),((_tmp7B2[3]=
Cyc_PP_textptr(_tmp278),((_tmp7B2[2]=Cyc_PP_text(((_tmp7B4=".",_tag_dyneither(
_tmp7B4,sizeof(char),2)))),((_tmp7B2[1]=Cyc_Absynpp_exp2doc(_tmp277),((_tmp7B2[0]=
Cyc_PP_text(((_tmp7B5="tagcheck(",_tag_dyneither(_tmp7B5,sizeof(char),10)))),Cyc_PP_cat(
_tag_dyneither(_tmp7B2,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL187;
_LL1B4: {struct Cyc_Absyn_Offsetof_e_struct*_tmp279=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp23E;if(_tmp279->tag != 20)goto _LL1B6;else{_tmp27A=(void*)_tmp279->f1;_tmp27B=(
void*)_tmp279->f2;{struct Cyc_Absyn_StructField_struct*_tmp27C=(struct Cyc_Absyn_StructField_struct*)
_tmp27B;if(_tmp27C->tag != 0)goto _LL1B6;else{_tmp27D=_tmp27C->f1;}};}}_LL1B5:{
const char*_tmp7BC;const char*_tmp7BB;const char*_tmp7BA;struct Cyc_PP_Doc*_tmp7B9[5];
s=((_tmp7B9[4]=Cyc_PP_text(((_tmp7BA=")",_tag_dyneither(_tmp7BA,sizeof(char),2)))),((
_tmp7B9[3]=Cyc_PP_textptr(_tmp27D),((_tmp7B9[2]=Cyc_PP_text(((_tmp7BB=",",
_tag_dyneither(_tmp7BB,sizeof(char),2)))),((_tmp7B9[1]=Cyc_Absynpp_typ2doc(
_tmp27A),((_tmp7B9[0]=Cyc_PP_text(((_tmp7BC="offsetof(",_tag_dyneither(_tmp7BC,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7B9,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL187;_LL1B6: {struct Cyc_Absyn_Offsetof_e_struct*_tmp27E=(
struct Cyc_Absyn_Offsetof_e_struct*)_tmp23E;if(_tmp27E->tag != 20)goto _LL1B8;else{
_tmp27F=(void*)_tmp27E->f1;_tmp280=(void*)_tmp27E->f2;{struct Cyc_Absyn_TupleIndex_struct*
_tmp281=(struct Cyc_Absyn_TupleIndex_struct*)_tmp280;if(_tmp281->tag != 1)goto
_LL1B8;else{_tmp282=_tmp281->f1;}};}}_LL1B7:{const char*_tmp7CA;const char*_tmp7C9;
struct Cyc_Int_pa_struct _tmp7C8;void*_tmp7C7[1];const char*_tmp7C6;const char*
_tmp7C5;struct Cyc_PP_Doc*_tmp7C4[5];s=((_tmp7C4[4]=Cyc_PP_text(((_tmp7C5=")",
_tag_dyneither(_tmp7C5,sizeof(char),2)))),((_tmp7C4[3]=Cyc_PP_text((struct
_dyneither_ptr)((_tmp7C8.tag=1,((_tmp7C8.f1=(unsigned long)((int)_tmp282),((
_tmp7C7[0]=& _tmp7C8,Cyc_aprintf(((_tmp7C6="%d",_tag_dyneither(_tmp7C6,sizeof(
char),3))),_tag_dyneither(_tmp7C7,sizeof(void*),1))))))))),((_tmp7C4[2]=Cyc_PP_text(((
_tmp7C9=",",_tag_dyneither(_tmp7C9,sizeof(char),2)))),((_tmp7C4[1]=Cyc_Absynpp_typ2doc(
_tmp27F),((_tmp7C4[0]=Cyc_PP_text(((_tmp7CA="offsetof(",_tag_dyneither(_tmp7CA,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7C4,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL187;_LL1B8: {struct Cyc_Absyn_Deref_e_struct*_tmp283=(
struct Cyc_Absyn_Deref_e_struct*)_tmp23E;if(_tmp283->tag != 21)goto _LL1BA;else{
_tmp284=_tmp283->f1;}}_LL1B9:{const char*_tmp7CD;struct Cyc_PP_Doc*_tmp7CC[2];s=((
_tmp7CC[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp284),((_tmp7CC[0]=Cyc_PP_text(((
_tmp7CD="*",_tag_dyneither(_tmp7CD,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp7CC,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL187;_LL1BA: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp285=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp23E;if(_tmp285->tag != 22)goto
_LL1BC;else{_tmp286=_tmp285->f1;_tmp287=_tmp285->f2;}}_LL1BB:{const char*_tmp7D0;
struct Cyc_PP_Doc*_tmp7CF[3];s=((_tmp7CF[2]=Cyc_PP_textptr(_tmp287),((_tmp7CF[1]=
Cyc_PP_text(((_tmp7D0=".",_tag_dyneither(_tmp7D0,sizeof(char),2)))),((_tmp7CF[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp286),Cyc_PP_cat(_tag_dyneither(_tmp7CF,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL187;_LL1BC: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp288=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp23E;if(_tmp288->tag != 23)goto
_LL1BE;else{_tmp289=_tmp288->f1;_tmp28A=_tmp288->f2;}}_LL1BD:{const char*_tmp7D3;
struct Cyc_PP_Doc*_tmp7D2[3];s=((_tmp7D2[2]=Cyc_PP_textptr(_tmp28A),((_tmp7D2[1]=
Cyc_PP_text(((_tmp7D3="->",_tag_dyneither(_tmp7D3,sizeof(char),3)))),((_tmp7D2[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp289),Cyc_PP_cat(_tag_dyneither(_tmp7D2,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL187;_LL1BE: {struct Cyc_Absyn_Subscript_e_struct*
_tmp28B=(struct Cyc_Absyn_Subscript_e_struct*)_tmp23E;if(_tmp28B->tag != 24)goto
_LL1C0;else{_tmp28C=_tmp28B->f1;_tmp28D=_tmp28B->f2;}}_LL1BF:{const char*_tmp7D8;
const char*_tmp7D7;struct Cyc_PP_Doc*_tmp7D6[4];s=((_tmp7D6[3]=Cyc_PP_text(((
_tmp7D7="]",_tag_dyneither(_tmp7D7,sizeof(char),2)))),((_tmp7D6[2]=Cyc_Absynpp_exp2doc(
_tmp28D),((_tmp7D6[1]=Cyc_PP_text(((_tmp7D8="[",_tag_dyneither(_tmp7D8,sizeof(
char),2)))),((_tmp7D6[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp28C),Cyc_PP_cat(
_tag_dyneither(_tmp7D6,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL187;_LL1C0: {
struct Cyc_Absyn_Tuple_e_struct*_tmp28E=(struct Cyc_Absyn_Tuple_e_struct*)_tmp23E;
if(_tmp28E->tag != 25)goto _LL1C2;else{_tmp28F=_tmp28E->f1;}}_LL1C1:{const char*
_tmp7DD;const char*_tmp7DC;struct Cyc_PP_Doc*_tmp7DB[4];s=((_tmp7DB[3]=Cyc_PP_text(((
_tmp7DC=")",_tag_dyneither(_tmp7DC,sizeof(char),2)))),((_tmp7DB[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmp28F),((_tmp7DB[1]=Cyc_PP_text(((_tmp7DD="(",_tag_dyneither(_tmp7DD,
sizeof(char),2)))),((_tmp7DB[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(
_tmp7DB,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL187;_LL1C2: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp290=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp23E;if(_tmp290->tag != 26)goto
_LL1C4;else{_tmp291=_tmp290->f1;_tmp292=_tmp290->f2;}}_LL1C3:{const char*_tmp7E4;
const char*_tmp7E3;const char*_tmp7E2;struct Cyc_PP_Doc*_tmp7E1[4];s=((_tmp7E1[3]=
Cyc_Absynpp_group_braces(((_tmp7E2=",",_tag_dyneither(_tmp7E2,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp292)),((_tmp7E1[2]=Cyc_PP_text(((_tmp7E3=")",
_tag_dyneither(_tmp7E3,sizeof(char),2)))),((_tmp7E1[1]=Cyc_Absynpp_typ2doc((*
_tmp291).f3),((_tmp7E1[0]=Cyc_PP_text(((_tmp7E4="(",_tag_dyneither(_tmp7E4,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7E1,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL187;_LL1C4: {struct Cyc_Absyn_Array_e_struct*_tmp293=(struct Cyc_Absyn_Array_e_struct*)
_tmp23E;if(_tmp293->tag != 27)goto _LL1C6;else{_tmp294=_tmp293->f1;}}_LL1C5:{const
char*_tmp7E5;s=Cyc_Absynpp_group_braces(((_tmp7E5=",",_tag_dyneither(_tmp7E5,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp294));}goto _LL187;
_LL1C6: {struct Cyc_Absyn_Comprehension_e_struct*_tmp295=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp23E;if(_tmp295->tag != 28)goto _LL1C8;else{_tmp296=_tmp295->f1;_tmp297=_tmp295->f2;
_tmp298=_tmp295->f3;}}_LL1C7:{const char*_tmp7EC;const char*_tmp7EB;const char*
_tmp7EA;struct Cyc_PP_Doc*_tmp7E9[8];s=((_tmp7E9[7]=Cyc_Absynpp_rb(),((_tmp7E9[6]=
Cyc_Absynpp_exp2doc(_tmp298),((_tmp7E9[5]=Cyc_PP_text(((_tmp7EA=" : ",
_tag_dyneither(_tmp7EA,sizeof(char),4)))),((_tmp7E9[4]=Cyc_Absynpp_exp2doc(
_tmp297),((_tmp7E9[3]=Cyc_PP_text(((_tmp7EB=" < ",_tag_dyneither(_tmp7EB,sizeof(
char),4)))),((_tmp7E9[2]=Cyc_PP_text(*(*_tmp296->name).f2),((_tmp7E9[1]=Cyc_PP_text(((
_tmp7EC="for ",_tag_dyneither(_tmp7EC,sizeof(char),5)))),((_tmp7E9[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp7E9,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL187;_LL1C8: {struct Cyc_Absyn_Aggregate_e_struct*_tmp299=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp23E;if(_tmp299->tag != 29)goto _LL1CA;else{_tmp29A=_tmp299->f1;_tmp29B=_tmp299->f2;
_tmp29C=_tmp299->f3;}}_LL1C9: {struct Cyc_List_List*_tmp30A=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp29C);{const char*_tmp7F1;struct Cyc_List_List*_tmp7F0;struct Cyc_PP_Doc*_tmp7EF[
2];s=((_tmp7EF[1]=Cyc_Absynpp_group_braces(((_tmp7F1=",",_tag_dyneither(_tmp7F1,
sizeof(char),2))),_tmp29B != 0?(_tmp7F0=_cycalloc(sizeof(*_tmp7F0)),((_tmp7F0->hd=
Cyc_Absynpp_tps2doc(_tmp29B),((_tmp7F0->tl=_tmp30A,_tmp7F0))))): _tmp30A),((
_tmp7EF[0]=Cyc_Absynpp_qvar2doc(_tmp29A),Cyc_PP_cat(_tag_dyneither(_tmp7EF,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL187;}_LL1CA: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp29D=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp23E;if(_tmp29D->tag != 30)goto
_LL1CC;else{_tmp29E=_tmp29D->f2;}}_LL1CB:{const char*_tmp7F2;s=Cyc_Absynpp_group_braces(((
_tmp7F2=",",_tag_dyneither(_tmp7F2,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp29E));}goto _LL187;_LL1CC: {struct Cyc_Absyn_Datatype_e_struct*_tmp29F=(struct
Cyc_Absyn_Datatype_e_struct*)_tmp23E;if(_tmp29F->tag != 31)goto _LL1CE;else{
_tmp2A0=_tmp29F->f1;_tmp2A1=_tmp29F->f3;}}_LL1CD: if(_tmp2A0 == 0)s=Cyc_Absynpp_qvar2doc(
_tmp2A1->name);else{const char*_tmp7F9;const char*_tmp7F8;const char*_tmp7F7;struct
Cyc_PP_Doc*_tmp7F6[2];s=((_tmp7F6[1]=Cyc_PP_egroup(((_tmp7F9="(",_tag_dyneither(
_tmp7F9,sizeof(char),2))),((_tmp7F8=")",_tag_dyneither(_tmp7F8,sizeof(char),2))),((
_tmp7F7=",",_tag_dyneither(_tmp7F7,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_exp2doc,_tmp2A0)),((_tmp7F6[0]=Cyc_Absynpp_qvar2doc(_tmp2A1->name),
Cyc_PP_cat(_tag_dyneither(_tmp7F6,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL187;
_LL1CE: {struct Cyc_Absyn_Enum_e_struct*_tmp2A2=(struct Cyc_Absyn_Enum_e_struct*)
_tmp23E;if(_tmp2A2->tag != 32)goto _LL1D0;else{_tmp2A3=_tmp2A2->f1;}}_LL1CF: s=Cyc_Absynpp_qvar2doc(
_tmp2A3);goto _LL187;_LL1D0: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp2A4=(struct
Cyc_Absyn_AnonEnum_e_struct*)_tmp23E;if(_tmp2A4->tag != 33)goto _LL1D2;else{
_tmp2A5=_tmp2A4->f1;}}_LL1D1: s=Cyc_Absynpp_qvar2doc(_tmp2A5);goto _LL187;_LL1D2: {
struct Cyc_Absyn_Malloc_e_struct*_tmp2A6=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp23E;if(_tmp2A6->tag != 34)goto _LL1D4;else{_tmp2A7=_tmp2A6->f1;_tmp2A8=_tmp2A7.is_calloc;
_tmp2A9=_tmp2A7.rgn;_tmp2AA=_tmp2A7.elt_type;_tmp2AB=_tmp2A7.num_elts;}}_LL1D3:
if(_tmp2A8){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp2AA)),0);if(_tmp2A9 == 0){const char*_tmp800;const char*_tmp7FF;const char*
_tmp7FE;struct Cyc_PP_Doc*_tmp7FD[5];s=((_tmp7FD[4]=Cyc_PP_text(((_tmp7FE=")",
_tag_dyneither(_tmp7FE,sizeof(char),2)))),((_tmp7FD[3]=Cyc_Absynpp_exp2doc(st),((
_tmp7FD[2]=Cyc_PP_text(((_tmp7FF=",",_tag_dyneither(_tmp7FF,sizeof(char),2)))),((
_tmp7FD[1]=Cyc_Absynpp_exp2doc(_tmp2AB),((_tmp7FD[0]=Cyc_PP_text(((_tmp800="calloc(",
_tag_dyneither(_tmp800,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7FD,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{const char*_tmp809;const char*
_tmp808;const char*_tmp807;const char*_tmp806;struct Cyc_PP_Doc*_tmp805[7];s=((
_tmp805[6]=Cyc_PP_text(((_tmp806=")",_tag_dyneither(_tmp806,sizeof(char),2)))),((
_tmp805[5]=Cyc_Absynpp_exp2doc(st),((_tmp805[4]=Cyc_PP_text(((_tmp807=",",
_tag_dyneither(_tmp807,sizeof(char),2)))),((_tmp805[3]=Cyc_Absynpp_exp2doc(
_tmp2AB),((_tmp805[2]=Cyc_PP_text(((_tmp808=",",_tag_dyneither(_tmp808,sizeof(
char),2)))),((_tmp805[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2A9),((
_tmp805[0]=Cyc_PP_text(((_tmp809="rcalloc(",_tag_dyneither(_tmp809,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp805,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmp2AA == 0)new_e=_tmp2AB;else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp2AA,0),_tmp2AB,0);}if(_tmp2A9 == 0){const char*
_tmp80E;const char*_tmp80D;struct Cyc_PP_Doc*_tmp80C[3];s=((_tmp80C[2]=Cyc_PP_text(((
_tmp80D=")",_tag_dyneither(_tmp80D,sizeof(char),2)))),((_tmp80C[1]=Cyc_Absynpp_exp2doc(
new_e),((_tmp80C[0]=Cyc_PP_text(((_tmp80E="malloc(",_tag_dyneither(_tmp80E,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp80C,sizeof(struct Cyc_PP_Doc*),3))))))));}
else{const char*_tmp815;const char*_tmp814;const char*_tmp813;struct Cyc_PP_Doc*
_tmp812[5];s=((_tmp812[4]=Cyc_PP_text(((_tmp813=")",_tag_dyneither(_tmp813,
sizeof(char),2)))),((_tmp812[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp812[2]=Cyc_PP_text(((
_tmp814=",",_tag_dyneither(_tmp814,sizeof(char),2)))),((_tmp812[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp2A9),((_tmp812[0]=Cyc_PP_text(((_tmp815="rmalloc(",
_tag_dyneither(_tmp815,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp812,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}}goto _LL187;_LL1D4: {struct Cyc_Absyn_Swap_e_struct*
_tmp2AC=(struct Cyc_Absyn_Swap_e_struct*)_tmp23E;if(_tmp2AC->tag != 35)goto _LL1D6;
else{_tmp2AD=_tmp2AC->f1;_tmp2AE=_tmp2AC->f2;}}_LL1D5:{const char*_tmp81C;const
char*_tmp81B;const char*_tmp81A;struct Cyc_PP_Doc*_tmp819[5];s=((_tmp819[4]=Cyc_PP_text(((
_tmp81A=")",_tag_dyneither(_tmp81A,sizeof(char),2)))),((_tmp819[3]=Cyc_Absynpp_exp2doc(
_tmp2AE),((_tmp819[2]=Cyc_PP_text(((_tmp81B=",",_tag_dyneither(_tmp81B,sizeof(
char),2)))),((_tmp819[1]=Cyc_Absynpp_exp2doc(_tmp2AD),((_tmp819[0]=Cyc_PP_text(((
_tmp81C="cycswap(",_tag_dyneither(_tmp81C,sizeof(char),9)))),Cyc_PP_cat(
_tag_dyneither(_tmp819,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL187;
_LL1D6: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp2AF=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp23E;if(_tmp2AF->tag != 36)goto _LL1D8;else{_tmp2B0=_tmp2AF->f1;_tmp2B1=_tmp2AF->f2;}}
_LL1D7:{const char*_tmp81D;s=Cyc_Absynpp_group_braces(((_tmp81D=",",
_tag_dyneither(_tmp81D,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2B1));}goto _LL187;_LL1D8: {struct Cyc_Absyn_StmtExp_e_struct*_tmp2B2=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp23E;if(_tmp2B2->tag != 37)goto _LL187;else{_tmp2B3=
_tmp2B2->f1;}}_LL1D9:{const char*_tmp822;const char*_tmp821;struct Cyc_PP_Doc*
_tmp820[7];s=((_tmp820[6]=Cyc_PP_text(((_tmp821=")",_tag_dyneither(_tmp821,
sizeof(char),2)))),((_tmp820[5]=Cyc_Absynpp_rb(),((_tmp820[4]=Cyc_PP_blank_doc(),((
_tmp820[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2B3,1)),((_tmp820[2]=Cyc_PP_blank_doc(),((
_tmp820[1]=Cyc_Absynpp_lb(),((_tmp820[0]=Cyc_PP_text(((_tmp822="(",
_tag_dyneither(_tmp822,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp820,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL187;_LL187:;}if(inprec >= 
myprec){const char*_tmp827;const char*_tmp826;struct Cyc_PP_Doc*_tmp825[3];s=((
_tmp825[2]=Cyc_PP_text(((_tmp826=")",_tag_dyneither(_tmp826,sizeof(char),2)))),((
_tmp825[1]=s,((_tmp825[0]=Cyc_PP_text(((_tmp827="(",_tag_dyneither(_tmp827,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp825,sizeof(struct Cyc_PP_Doc*),3))))))));}
return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d);struct Cyc_PP_Doc*
Cyc_Absynpp_designator2doc(void*d){void*_tmp32E=d;struct Cyc_Absyn_Exp*_tmp330;
struct _dyneither_ptr*_tmp332;_LL1E0: {struct Cyc_Absyn_ArrayElement_struct*
_tmp32F=(struct Cyc_Absyn_ArrayElement_struct*)_tmp32E;if(_tmp32F->tag != 0)goto
_LL1E2;else{_tmp330=_tmp32F->f1;}}_LL1E1: {const char*_tmp82C;const char*_tmp82B;
struct Cyc_PP_Doc*_tmp82A[3];return(_tmp82A[2]=Cyc_PP_text(((_tmp82B="]",
_tag_dyneither(_tmp82B,sizeof(char),2)))),((_tmp82A[1]=Cyc_Absynpp_exp2doc(
_tmp330),((_tmp82A[0]=Cyc_PP_text(((_tmp82C=".[",_tag_dyneither(_tmp82C,sizeof(
char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp82A,sizeof(struct Cyc_PP_Doc*),3)))))));}
_LL1E2: {struct Cyc_Absyn_FieldName_struct*_tmp331=(struct Cyc_Absyn_FieldName_struct*)
_tmp32E;if(_tmp331->tag != 1)goto _LL1DF;else{_tmp332=_tmp331->f1;}}_LL1E3: {const
char*_tmp82F;struct Cyc_PP_Doc*_tmp82E[2];return(_tmp82E[1]=Cyc_PP_textptr(
_tmp332),((_tmp82E[0]=Cyc_PP_text(((_tmp82F=".",_tag_dyneither(_tmp82F,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp82E,sizeof(struct Cyc_PP_Doc*),2)))));}
_LL1DF:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de);struct Cyc_PP_Doc*
Cyc_Absynpp_de2doc(struct _tuple14*de){if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*
de).f2);else{const char*_tmp836;const char*_tmp835;const char*_tmp834;struct Cyc_PP_Doc*
_tmp833[2];return(_tmp833[1]=Cyc_Absynpp_exp2doc((*de).f2),((_tmp833[0]=Cyc_PP_egroup(((
_tmp836="",_tag_dyneither(_tmp836,sizeof(char),1))),((_tmp835="=",_tag_dyneither(
_tmp835,sizeof(char),2))),((_tmp834="=",_tag_dyneither(_tmp834,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp833,sizeof(
struct Cyc_PP_Doc*),2)))));}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es){const char*_tmp839;const char*_tmp838;const char*_tmp837;
return Cyc_PP_group(((_tmp837="",_tag_dyneither(_tmp837,sizeof(char),1))),((
_tmp838="",_tag_dyneither(_tmp838,sizeof(char),1))),((_tmp839=",",_tag_dyneither(
_tmp839,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,
struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es));}struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c);struct
Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){union Cyc_Absyn_Cnst _tmp33F=
c;struct _tuple3 _tmp340;enum Cyc_Absyn_Sign _tmp341;char _tmp342;struct _tuple4
_tmp343;enum Cyc_Absyn_Sign _tmp344;short _tmp345;struct _tuple5 _tmp346;enum Cyc_Absyn_Sign
_tmp347;int _tmp348;struct _tuple5 _tmp349;enum Cyc_Absyn_Sign _tmp34A;int _tmp34B;
struct _tuple5 _tmp34C;enum Cyc_Absyn_Sign _tmp34D;int _tmp34E;struct _tuple6 _tmp34F;
enum Cyc_Absyn_Sign _tmp350;long long _tmp351;struct _dyneither_ptr _tmp352;int
_tmp353;struct _dyneither_ptr _tmp354;_LL1E5: if((_tmp33F.Char_c).tag != 2)goto
_LL1E7;_tmp340=(struct _tuple3)(_tmp33F.Char_c).val;_tmp341=_tmp340.f1;_tmp342=
_tmp340.f2;_LL1E6: {const char*_tmp83D;void*_tmp83C[1];struct Cyc_String_pa_struct
_tmp83B;return Cyc_PP_text((struct _dyneither_ptr)((_tmp83B.tag=0,((_tmp83B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp342)),((
_tmp83C[0]=& _tmp83B,Cyc_aprintf(((_tmp83D="'%s'",_tag_dyneither(_tmp83D,sizeof(
char),5))),_tag_dyneither(_tmp83C,sizeof(void*),1)))))))));}_LL1E7: if((_tmp33F.Short_c).tag
!= 3)goto _LL1E9;_tmp343=(struct _tuple4)(_tmp33F.Short_c).val;_tmp344=_tmp343.f1;
_tmp345=_tmp343.f2;_LL1E8: {const char*_tmp841;void*_tmp840[1];struct Cyc_Int_pa_struct
_tmp83F;return Cyc_PP_text((struct _dyneither_ptr)((_tmp83F.tag=1,((_tmp83F.f1=(
unsigned long)((int)_tmp345),((_tmp840[0]=& _tmp83F,Cyc_aprintf(((_tmp841="%d",
_tag_dyneither(_tmp841,sizeof(char),3))),_tag_dyneither(_tmp840,sizeof(void*),1)))))))));}
_LL1E9: if((_tmp33F.Int_c).tag != 4)goto _LL1EB;_tmp346=(struct _tuple5)(_tmp33F.Int_c).val;
_tmp347=_tmp346.f1;if(_tmp347 != Cyc_Absyn_None)goto _LL1EB;_tmp348=_tmp346.f2;
_LL1EA: _tmp34B=_tmp348;goto _LL1EC;_LL1EB: if((_tmp33F.Int_c).tag != 4)goto _LL1ED;
_tmp349=(struct _tuple5)(_tmp33F.Int_c).val;_tmp34A=_tmp349.f1;if(_tmp34A != Cyc_Absyn_Signed)
goto _LL1ED;_tmp34B=_tmp349.f2;_LL1EC: {const char*_tmp845;void*_tmp844[1];struct
Cyc_Int_pa_struct _tmp843;return Cyc_PP_text((struct _dyneither_ptr)((_tmp843.tag=1,((
_tmp843.f1=(unsigned long)_tmp34B,((_tmp844[0]=& _tmp843,Cyc_aprintf(((_tmp845="%d",
_tag_dyneither(_tmp845,sizeof(char),3))),_tag_dyneither(_tmp844,sizeof(void*),1)))))))));}
_LL1ED: if((_tmp33F.Int_c).tag != 4)goto _LL1EF;_tmp34C=(struct _tuple5)(_tmp33F.Int_c).val;
_tmp34D=_tmp34C.f1;if(_tmp34D != Cyc_Absyn_Unsigned)goto _LL1EF;_tmp34E=_tmp34C.f2;
_LL1EE: {const char*_tmp849;void*_tmp848[1];struct Cyc_Int_pa_struct _tmp847;return
Cyc_PP_text((struct _dyneither_ptr)((_tmp847.tag=1,((_tmp847.f1=(unsigned int)
_tmp34E,((_tmp848[0]=& _tmp847,Cyc_aprintf(((_tmp849="%u",_tag_dyneither(_tmp849,
sizeof(char),3))),_tag_dyneither(_tmp848,sizeof(void*),1)))))))));}_LL1EF: if((
_tmp33F.LongLong_c).tag != 5)goto _LL1F1;_tmp34F=(struct _tuple6)(_tmp33F.LongLong_c).val;
_tmp350=_tmp34F.f1;_tmp351=_tmp34F.f2;_LL1F0: {const char*_tmp84A;return Cyc_PP_text(((
_tmp84A="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp84A,sizeof(char),27))));}
_LL1F1: if((_tmp33F.Float_c).tag != 6)goto _LL1F3;_tmp352=(struct _dyneither_ptr)(
_tmp33F.Float_c).val;_LL1F2: return Cyc_PP_text(_tmp352);_LL1F3: if((_tmp33F.Null_c).tag
!= 1)goto _LL1F5;_tmp353=(int)(_tmp33F.Null_c).val;_LL1F4: {const char*_tmp84B;
return Cyc_PP_text(((_tmp84B="NULL",_tag_dyneither(_tmp84B,sizeof(char),5))));}
_LL1F5: if((_tmp33F.String_c).tag != 7)goto _LL1E4;_tmp354=(struct _dyneither_ptr)(
_tmp33F.String_c).val;_LL1F6: {const char*_tmp850;const char*_tmp84F;struct Cyc_PP_Doc*
_tmp84E[3];return(_tmp84E[2]=Cyc_PP_text(((_tmp84F="\"",_tag_dyneither(_tmp84F,
sizeof(char),2)))),((_tmp84E[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp354)),((
_tmp84E[0]=Cyc_PP_text(((_tmp850="\"",_tag_dyneither(_tmp850,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp84E,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL1E4:;}
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct
Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop
p,struct Cyc_List_List*es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);if(p == (
enum Cyc_Absyn_Primop)Cyc_Absyn_Numelts){if(es == 0  || es->tl != 0){struct Cyc_Core_Failure_struct
_tmp85D;const char*_tmp85C;void*_tmp85B[1];struct Cyc_String_pa_struct _tmp85A;
struct Cyc_Core_Failure_struct*_tmp859;(int)_throw((void*)((_tmp859=_cycalloc(
sizeof(*_tmp859)),((_tmp859[0]=((_tmp85D.tag=Cyc_Core_Failure,((_tmp85D.f1=(
struct _dyneither_ptr)((_tmp85A.tag=0,((_tmp85A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp85B[0]=& _tmp85A,Cyc_aprintf(((
_tmp85C="Absynpp::primapp2doc Numelts: %s with bad args",_tag_dyneither(_tmp85C,
sizeof(char),47))),_tag_dyneither(_tmp85B,sizeof(void*),1)))))))),_tmp85D)))),
_tmp859)))));}{const char*_tmp862;const char*_tmp861;struct Cyc_PP_Doc*_tmp860[3];
return(_tmp860[2]=Cyc_PP_text(((_tmp861=")",_tag_dyneither(_tmp861,sizeof(char),
2)))),((_tmp860[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((_tmp860[0]=
Cyc_PP_text(((_tmp862="numelts(",_tag_dyneither(_tmp862,sizeof(char),9)))),Cyc_PP_cat(
_tag_dyneither(_tmp860,sizeof(struct Cyc_PP_Doc*),3)))))));};}else{struct Cyc_List_List*
ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0){struct Cyc_Core_Failure_struct _tmp86F;const char*_tmp86E;void*_tmp86D[1];
struct Cyc_String_pa_struct _tmp86C;struct Cyc_Core_Failure_struct*_tmp86B;(int)
_throw((void*)((_tmp86B=_cycalloc(sizeof(*_tmp86B)),((_tmp86B[0]=((_tmp86F.tag=
Cyc_Core_Failure,((_tmp86F.f1=(struct _dyneither_ptr)((_tmp86C.tag=0,((_tmp86C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((
_tmp86D[0]=& _tmp86C,Cyc_aprintf(((_tmp86E="Absynpp::primapp2doc: %s with no args",
_tag_dyneither(_tmp86E,sizeof(char),38))),_tag_dyneither(_tmp86D,sizeof(void*),1)))))))),
_tmp86F)))),_tmp86B)))));}else{if(ds->tl == 0){const char*_tmp872;struct Cyc_PP_Doc*
_tmp871[3];return(_tmp871[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp871[1]=Cyc_PP_text(((
_tmp872=" ",_tag_dyneither(_tmp872,sizeof(char),2)))),((_tmp871[0]=ps,Cyc_PP_cat(
_tag_dyneither(_tmp871,sizeof(struct Cyc_PP_Doc*),3)))))));}else{if(((struct Cyc_List_List*)
_check_null(ds->tl))->tl != 0){struct Cyc_Core_Failure_struct _tmp87F;const char*
_tmp87E;void*_tmp87D[1];struct Cyc_String_pa_struct _tmp87C;struct Cyc_Core_Failure_struct*
_tmp87B;(int)_throw((void*)((_tmp87B=_cycalloc(sizeof(*_tmp87B)),((_tmp87B[0]=((
_tmp87F.tag=Cyc_Core_Failure,((_tmp87F.f1=(struct _dyneither_ptr)((_tmp87C.tag=0,((
_tmp87C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp87D[0]=& _tmp87C,Cyc_aprintf(((_tmp87E="Absynpp::primapp2doc: %s with more than 2 args",
_tag_dyneither(_tmp87E,sizeof(char),47))),_tag_dyneither(_tmp87D,sizeof(void*),1)))))))),
_tmp87F)))),_tmp87B)))));}else{const char*_tmp884;const char*_tmp883;struct Cyc_PP_Doc*
_tmp882[5];return(_tmp882[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)
_check_null(ds->tl))->hd,((_tmp882[3]=Cyc_PP_text(((_tmp883=" ",_tag_dyneither(
_tmp883,sizeof(char),2)))),((_tmp882[2]=ps,((_tmp882[1]=Cyc_PP_text(((_tmp884=" ",
_tag_dyneither(_tmp884,sizeof(char),2)))),((_tmp882[0]=(struct Cyc_PP_Doc*)ds->hd,
Cyc_PP_cat(_tag_dyneither(_tmp882,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);struct
_dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){switch(p){case Cyc_Absyn_Plus:
_LL1F7: {const char*_tmp885;return(_tmp885="+",_tag_dyneither(_tmp885,sizeof(char),
2));}case Cyc_Absyn_Times: _LL1F8: {const char*_tmp886;return(_tmp886="*",
_tag_dyneither(_tmp886,sizeof(char),2));}case Cyc_Absyn_Minus: _LL1F9: {const char*
_tmp887;return(_tmp887="-",_tag_dyneither(_tmp887,sizeof(char),2));}case Cyc_Absyn_Div:
_LL1FA: {const char*_tmp888;return(_tmp888="/",_tag_dyneither(_tmp888,sizeof(char),
2));}case Cyc_Absyn_Mod: _LL1FB: {const char*_tmp88A;const char*_tmp889;return Cyc_Absynpp_print_for_cycdoc?(
_tmp88A="\\%",_tag_dyneither(_tmp88A,sizeof(char),3)):((_tmp889="%",
_tag_dyneither(_tmp889,sizeof(char),2)));}case Cyc_Absyn_Eq: _LL1FC: {const char*
_tmp88B;return(_tmp88B="==",_tag_dyneither(_tmp88B,sizeof(char),3));}case Cyc_Absyn_Neq:
_LL1FD: {const char*_tmp88C;return(_tmp88C="!=",_tag_dyneither(_tmp88C,sizeof(
char),3));}case Cyc_Absyn_Gt: _LL1FE: {const char*_tmp88D;return(_tmp88D=">",
_tag_dyneither(_tmp88D,sizeof(char),2));}case Cyc_Absyn_Lt: _LL1FF: {const char*
_tmp88E;return(_tmp88E="<",_tag_dyneither(_tmp88E,sizeof(char),2));}case Cyc_Absyn_Gte:
_LL200: {const char*_tmp88F;return(_tmp88F=">=",_tag_dyneither(_tmp88F,sizeof(
char),3));}case Cyc_Absyn_Lte: _LL201: {const char*_tmp890;return(_tmp890="<=",
_tag_dyneither(_tmp890,sizeof(char),3));}case Cyc_Absyn_Not: _LL202: {const char*
_tmp891;return(_tmp891="!",_tag_dyneither(_tmp891,sizeof(char),2));}case Cyc_Absyn_Bitnot:
_LL203: {const char*_tmp892;return(_tmp892="~",_tag_dyneither(_tmp892,sizeof(char),
2));}case Cyc_Absyn_Bitand: _LL204: {const char*_tmp893;return(_tmp893="&",
_tag_dyneither(_tmp893,sizeof(char),2));}case Cyc_Absyn_Bitor: _LL205: {const char*
_tmp894;return(_tmp894="|",_tag_dyneither(_tmp894,sizeof(char),2));}case Cyc_Absyn_Bitxor:
_LL206: {const char*_tmp895;return(_tmp895="^",_tag_dyneither(_tmp895,sizeof(char),
2));}case Cyc_Absyn_Bitlshift: _LL207: {const char*_tmp896;return(_tmp896="<<",
_tag_dyneither(_tmp896,sizeof(char),3));}case Cyc_Absyn_Bitlrshift: _LL208: {const
char*_tmp897;return(_tmp897=">>",_tag_dyneither(_tmp897,sizeof(char),3));}case
Cyc_Absyn_Bitarshift: _LL209: {const char*_tmp898;return(_tmp898=">>>",
_tag_dyneither(_tmp898,sizeof(char),4));}case Cyc_Absyn_Numelts: _LL20A: {const
char*_tmp899;return(_tmp899="numelts",_tag_dyneither(_tmp899,sizeof(char),8));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p);struct Cyc_PP_Doc*
Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){return Cyc_PP_text(Cyc_Absynpp_prim2str(
p));}int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s){void*_tmp392=s->r;_LL20D: {struct Cyc_Absyn_Decl_s_struct*
_tmp393=(struct Cyc_Absyn_Decl_s_struct*)_tmp392;if(_tmp393->tag != 12)goto _LL20F;}
_LL20E: return 1;_LL20F:;_LL210: return 0;_LL20C:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*st,int stmtexp);struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct
Cyc_Absyn_Stmt*st,int stmtexp){struct Cyc_PP_Doc*s;{void*_tmp394=st->r;struct Cyc_Absyn_Exp*
_tmp397;struct Cyc_Absyn_Stmt*_tmp399;struct Cyc_Absyn_Stmt*_tmp39A;struct Cyc_Absyn_Exp*
_tmp39C;struct Cyc_Absyn_Exp*_tmp39E;struct Cyc_Absyn_Stmt*_tmp39F;struct Cyc_Absyn_Stmt*
_tmp3A0;struct _tuple8 _tmp3A2;struct Cyc_Absyn_Exp*_tmp3A3;struct Cyc_Absyn_Stmt*
_tmp3A4;struct _dyneither_ptr*_tmp3A8;struct Cyc_Absyn_Exp*_tmp3AA;struct _tuple8
_tmp3AB;struct Cyc_Absyn_Exp*_tmp3AC;struct _tuple8 _tmp3AD;struct Cyc_Absyn_Exp*
_tmp3AE;struct Cyc_Absyn_Stmt*_tmp3AF;struct Cyc_Absyn_Exp*_tmp3B1;struct Cyc_List_List*
_tmp3B2;struct Cyc_List_List*_tmp3B4;struct Cyc_List_List*_tmp3B6;struct Cyc_Absyn_Decl*
_tmp3B8;struct Cyc_Absyn_Stmt*_tmp3B9;struct _dyneither_ptr*_tmp3BB;struct Cyc_Absyn_Stmt*
_tmp3BC;struct Cyc_Absyn_Stmt*_tmp3BE;struct _tuple8 _tmp3BF;struct Cyc_Absyn_Exp*
_tmp3C0;struct Cyc_Absyn_Stmt*_tmp3C2;struct Cyc_List_List*_tmp3C3;struct Cyc_Absyn_Exp*
_tmp3C5;_LL212: {struct Cyc_Absyn_Skip_s_struct*_tmp395=(struct Cyc_Absyn_Skip_s_struct*)
_tmp394;if(_tmp395->tag != 0)goto _LL214;}_LL213:{const char*_tmp89A;s=Cyc_PP_text(((
_tmp89A=";",_tag_dyneither(_tmp89A,sizeof(char),2))));}goto _LL211;_LL214: {
struct Cyc_Absyn_Exp_s_struct*_tmp396=(struct Cyc_Absyn_Exp_s_struct*)_tmp394;if(
_tmp396->tag != 1)goto _LL216;else{_tmp397=_tmp396->f1;}}_LL215:{const char*_tmp89D;
struct Cyc_PP_Doc*_tmp89C[2];s=((_tmp89C[1]=Cyc_PP_text(((_tmp89D=";",
_tag_dyneither(_tmp89D,sizeof(char),2)))),((_tmp89C[0]=Cyc_Absynpp_exp2doc(
_tmp397),Cyc_PP_cat(_tag_dyneither(_tmp89C,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL211;_LL216: {struct Cyc_Absyn_Seq_s_struct*_tmp398=(struct Cyc_Absyn_Seq_s_struct*)
_tmp394;if(_tmp398->tag != 2)goto _LL218;else{_tmp399=_tmp398->f1;_tmp39A=_tmp398->f2;}}
_LL217: if(Cyc_Absynpp_decls_first){if(Cyc_Absynpp_is_declaration(_tmp399)){
struct Cyc_PP_Doc*_tmp8A8[5];struct Cyc_PP_Doc*_tmp8A7[7];const char*_tmp8A6;const
char*_tmp8A5;struct Cyc_PP_Doc*_tmp8A4[7];s=((_tmp8A4[6]=Cyc_Absynpp_is_declaration(
_tmp39A)?stmtexp?(_tmp8A7[6]=Cyc_PP_line_doc(),((_tmp8A7[5]=Cyc_PP_text(((
_tmp8A6=");",_tag_dyneither(_tmp8A6,sizeof(char),3)))),((_tmp8A7[4]=Cyc_Absynpp_rb(),((
_tmp8A7[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp39A,stmtexp)),((_tmp8A7[2]=Cyc_PP_blank_doc(),((
_tmp8A7[1]=Cyc_Absynpp_lb(),((_tmp8A7[0]=Cyc_PP_text(((_tmp8A5="(",
_tag_dyneither(_tmp8A5,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8A7,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))):((_tmp8A8[4]=Cyc_PP_line_doc(),((
_tmp8A8[3]=Cyc_Absynpp_rb(),((_tmp8A8[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp39A,stmtexp)),((_tmp8A8[1]=Cyc_PP_blank_doc(),((_tmp8A8[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp8A8,sizeof(struct Cyc_PP_Doc*),5)))))))))))): Cyc_Absynpp_stmt2doc(
_tmp39A,stmtexp),((_tmp8A4[5]=Cyc_PP_line_doc(),((_tmp8A4[4]=Cyc_Absynpp_rb(),((
_tmp8A4[3]=Cyc_PP_line_doc(),((_tmp8A4[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp399,0)),((_tmp8A4[1]=Cyc_PP_blank_doc(),((_tmp8A4[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dyneither(_tmp8A4,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{if(Cyc_Absynpp_is_declaration(
_tmp39A)){struct Cyc_PP_Doc*_tmp8B3[4];struct Cyc_PP_Doc*_tmp8B2[6];const char*
_tmp8B1;const char*_tmp8B0;struct Cyc_PP_Doc*_tmp8AF[4];s=((_tmp8AF[3]=Cyc_PP_line_doc(),((
_tmp8AF[2]=stmtexp?(_tmp8B2[5]=Cyc_PP_text(((_tmp8B1=");",_tag_dyneither(_tmp8B1,
sizeof(char),3)))),((_tmp8B2[4]=Cyc_Absynpp_rb(),((_tmp8B2[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp39A,stmtexp)),((_tmp8B2[2]=Cyc_PP_blank_doc(),((_tmp8B2[1]=Cyc_Absynpp_lb(),((
_tmp8B2[0]=Cyc_PP_text(((_tmp8B0="(",_tag_dyneither(_tmp8B0,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp8B2,sizeof(struct Cyc_PP_Doc*),6))))))))))))):((
_tmp8B3[3]=Cyc_Absynpp_rb(),((_tmp8B3[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp39A,stmtexp)),((_tmp8B3[1]=Cyc_PP_blank_doc(),((_tmp8B3[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp8B3,sizeof(struct Cyc_PP_Doc*),4)))))))))),((
_tmp8AF[1]=Cyc_PP_line_doc(),((_tmp8AF[0]=Cyc_Absynpp_stmt2doc(_tmp399,0),Cyc_PP_cat(
_tag_dyneither(_tmp8AF,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{struct Cyc_PP_Doc*
_tmp8B4[3];s=((_tmp8B4[2]=Cyc_Absynpp_stmt2doc(_tmp39A,stmtexp),((_tmp8B4[1]=Cyc_PP_line_doc(),((
_tmp8B4[0]=Cyc_Absynpp_stmt2doc(_tmp399,0),Cyc_PP_cat(_tag_dyneither(_tmp8B4,
sizeof(struct Cyc_PP_Doc*),3))))))));}}}else{struct Cyc_PP_Doc*_tmp8B5[3];s=((
_tmp8B5[2]=Cyc_Absynpp_stmt2doc(_tmp39A,stmtexp),((_tmp8B5[1]=Cyc_PP_line_doc(),((
_tmp8B5[0]=Cyc_Absynpp_stmt2doc(_tmp399,0),Cyc_PP_cat(_tag_dyneither(_tmp8B5,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL211;_LL218: {struct Cyc_Absyn_Return_s_struct*
_tmp39B=(struct Cyc_Absyn_Return_s_struct*)_tmp394;if(_tmp39B->tag != 3)goto _LL21A;
else{_tmp39C=_tmp39B->f1;}}_LL219: if(_tmp39C == 0){const char*_tmp8B6;s=Cyc_PP_text(((
_tmp8B6="return;",_tag_dyneither(_tmp8B6,sizeof(char),8))));}else{const char*
_tmp8BB;const char*_tmp8BA;struct Cyc_PP_Doc*_tmp8B9[3];s=((_tmp8B9[2]=Cyc_PP_text(((
_tmp8BA=";",_tag_dyneither(_tmp8BA,sizeof(char),2)))),((_tmp8B9[1]=_tmp39C == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp39C),((_tmp8B9[0]=
Cyc_PP_text(((_tmp8BB="return ",_tag_dyneither(_tmp8BB,sizeof(char),8)))),Cyc_PP_cat(
_tag_dyneither(_tmp8B9,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL211;_LL21A: {
struct Cyc_Absyn_IfThenElse_s_struct*_tmp39D=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp394;if(_tmp39D->tag != 4)goto _LL21C;else{_tmp39E=_tmp39D->f1;_tmp39F=_tmp39D->f2;
_tmp3A0=_tmp39D->f3;}}_LL21B: {int print_else;{void*_tmp3D9=_tmp3A0->r;_LL237: {
struct Cyc_Absyn_Skip_s_struct*_tmp3DA=(struct Cyc_Absyn_Skip_s_struct*)_tmp3D9;
if(_tmp3DA->tag != 0)goto _LL239;}_LL238: print_else=0;goto _LL236;_LL239:;_LL23A:
print_else=1;goto _LL236;_LL236:;}{const char*_tmp8CA;const char*_tmp8C9;struct Cyc_PP_Doc*
_tmp8C8[2];struct Cyc_PP_Doc*_tmp8C7[6];struct Cyc_PP_Doc*_tmp8C6[2];const char*
_tmp8C5;struct Cyc_PP_Doc*_tmp8C4[8];s=((_tmp8C4[7]=print_else?(_tmp8C7[5]=Cyc_Absynpp_rb(),((
_tmp8C7[4]=Cyc_PP_line_doc(),((_tmp8C7[3]=Cyc_PP_nest(2,((_tmp8C6[1]=Cyc_Absynpp_stmt2doc(
_tmp3A0,0),((_tmp8C6[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8C6,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8C7[2]=Cyc_Absynpp_lb(),((_tmp8C7[1]=
Cyc_PP_text(((_tmp8C5="else ",_tag_dyneither(_tmp8C5,sizeof(char),6)))),((
_tmp8C7[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8C7,sizeof(struct Cyc_PP_Doc*),
6))))))))))))): Cyc_PP_nil_doc(),((_tmp8C4[6]=Cyc_Absynpp_rb(),((_tmp8C4[5]=Cyc_PP_line_doc(),((
_tmp8C4[4]=Cyc_PP_nest(2,((_tmp8C8[1]=Cyc_Absynpp_stmt2doc(_tmp39F,0),((_tmp8C8[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8C8,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp8C4[3]=Cyc_Absynpp_lb(),((_tmp8C4[2]=Cyc_PP_text(((_tmp8C9=") ",
_tag_dyneither(_tmp8C9,sizeof(char),3)))),((_tmp8C4[1]=Cyc_Absynpp_exp2doc(
_tmp39E),((_tmp8C4[0]=Cyc_PP_text(((_tmp8CA="if (",_tag_dyneither(_tmp8CA,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8C4,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL211;}_LL21C: {struct Cyc_Absyn_While_s_struct*_tmp3A1=(struct Cyc_Absyn_While_s_struct*)
_tmp394;if(_tmp3A1->tag != 5)goto _LL21E;else{_tmp3A2=_tmp3A1->f1;_tmp3A3=_tmp3A2.f1;
_tmp3A4=_tmp3A1->f2;}}_LL21D:{const char*_tmp8D1;const char*_tmp8D0;struct Cyc_PP_Doc*
_tmp8CF[2];struct Cyc_PP_Doc*_tmp8CE[7];s=((_tmp8CE[6]=Cyc_Absynpp_rb(),((_tmp8CE[
5]=Cyc_PP_line_doc(),((_tmp8CE[4]=Cyc_PP_nest(2,((_tmp8CF[1]=Cyc_Absynpp_stmt2doc(
_tmp3A4,0),((_tmp8CF[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8CF,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8CE[3]=Cyc_Absynpp_lb(),((_tmp8CE[2]=
Cyc_PP_text(((_tmp8D0=") ",_tag_dyneither(_tmp8D0,sizeof(char),3)))),((_tmp8CE[1]=
Cyc_Absynpp_exp2doc(_tmp3A3),((_tmp8CE[0]=Cyc_PP_text(((_tmp8D1="while (",
_tag_dyneither(_tmp8D1,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8CE,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL211;_LL21E: {struct Cyc_Absyn_Break_s_struct*
_tmp3A5=(struct Cyc_Absyn_Break_s_struct*)_tmp394;if(_tmp3A5->tag != 6)goto _LL220;}
_LL21F:{const char*_tmp8D2;s=Cyc_PP_text(((_tmp8D2="break;",_tag_dyneither(
_tmp8D2,sizeof(char),7))));}goto _LL211;_LL220: {struct Cyc_Absyn_Continue_s_struct*
_tmp3A6=(struct Cyc_Absyn_Continue_s_struct*)_tmp394;if(_tmp3A6->tag != 7)goto
_LL222;}_LL221:{const char*_tmp8D3;s=Cyc_PP_text(((_tmp8D3="continue;",
_tag_dyneither(_tmp8D3,sizeof(char),10))));}goto _LL211;_LL222: {struct Cyc_Absyn_Goto_s_struct*
_tmp3A7=(struct Cyc_Absyn_Goto_s_struct*)_tmp394;if(_tmp3A7->tag != 8)goto _LL224;
else{_tmp3A8=_tmp3A7->f1;}}_LL223:{const char*_tmp8D7;void*_tmp8D6[1];struct Cyc_String_pa_struct
_tmp8D5;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp8D5.tag=0,((_tmp8D5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp3A8),((_tmp8D6[0]=& _tmp8D5,Cyc_aprintf(((
_tmp8D7="goto %s;",_tag_dyneither(_tmp8D7,sizeof(char),9))),_tag_dyneither(
_tmp8D6,sizeof(void*),1)))))))));}goto _LL211;_LL224: {struct Cyc_Absyn_For_s_struct*
_tmp3A9=(struct Cyc_Absyn_For_s_struct*)_tmp394;if(_tmp3A9->tag != 9)goto _LL226;
else{_tmp3AA=_tmp3A9->f1;_tmp3AB=_tmp3A9->f2;_tmp3AC=_tmp3AB.f1;_tmp3AD=_tmp3A9->f3;
_tmp3AE=_tmp3AD.f1;_tmp3AF=_tmp3A9->f4;}}_LL225:{const char*_tmp8E2;const char*
_tmp8E1;const char*_tmp8E0;const char*_tmp8DF;struct Cyc_PP_Doc*_tmp8DE[2];struct
Cyc_PP_Doc*_tmp8DD[11];s=((_tmp8DD[10]=Cyc_Absynpp_rb(),((_tmp8DD[9]=Cyc_PP_line_doc(),((
_tmp8DD[8]=Cyc_PP_nest(2,((_tmp8DE[1]=Cyc_Absynpp_stmt2doc(_tmp3AF,0),((_tmp8DE[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8DE,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp8DD[7]=Cyc_Absynpp_lb(),((_tmp8DD[6]=Cyc_PP_text(((_tmp8DF=") ",
_tag_dyneither(_tmp8DF,sizeof(char),3)))),((_tmp8DD[5]=Cyc_Absynpp_exp2doc(
_tmp3AE),((_tmp8DD[4]=Cyc_PP_text(((_tmp8E0="; ",_tag_dyneither(_tmp8E0,sizeof(
char),3)))),((_tmp8DD[3]=Cyc_Absynpp_exp2doc(_tmp3AC),((_tmp8DD[2]=Cyc_PP_text(((
_tmp8E1="; ",_tag_dyneither(_tmp8E1,sizeof(char),3)))),((_tmp8DD[1]=Cyc_Absynpp_exp2doc(
_tmp3AA),((_tmp8DD[0]=Cyc_PP_text(((_tmp8E2="for(",_tag_dyneither(_tmp8E2,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8DD,sizeof(struct Cyc_PP_Doc*),
11))))))))))))))))))))))));}goto _LL211;_LL226: {struct Cyc_Absyn_Switch_s_struct*
_tmp3B0=(struct Cyc_Absyn_Switch_s_struct*)_tmp394;if(_tmp3B0->tag != 10)goto
_LL228;else{_tmp3B1=_tmp3B0->f1;_tmp3B2=_tmp3B0->f2;}}_LL227:{const char*_tmp8E7;
const char*_tmp8E6;struct Cyc_PP_Doc*_tmp8E5[8];s=((_tmp8E5[7]=Cyc_Absynpp_rb(),((
_tmp8E5[6]=Cyc_PP_line_doc(),((_tmp8E5[5]=Cyc_Absynpp_switchclauses2doc(_tmp3B2),((
_tmp8E5[4]=Cyc_PP_line_doc(),((_tmp8E5[3]=Cyc_Absynpp_lb(),((_tmp8E5[2]=Cyc_PP_text(((
_tmp8E6=") ",_tag_dyneither(_tmp8E6,sizeof(char),3)))),((_tmp8E5[1]=Cyc_Absynpp_exp2doc(
_tmp3B1),((_tmp8E5[0]=Cyc_PP_text(((_tmp8E7="switch (",_tag_dyneither(_tmp8E7,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp8E5,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL211;_LL228: {struct Cyc_Absyn_Fallthru_s_struct*_tmp3B3=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp394;if(_tmp3B3->tag != 11)goto _LL22A;else{_tmp3B4=_tmp3B3->f1;if(_tmp3B4 != 0)
goto _LL22A;}}_LL229:{const char*_tmp8E8;s=Cyc_PP_text(((_tmp8E8="fallthru;",
_tag_dyneither(_tmp8E8,sizeof(char),10))));}goto _LL211;_LL22A: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp3B5=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp394;if(_tmp3B5->tag != 11)goto
_LL22C;else{_tmp3B6=_tmp3B5->f1;}}_LL22B:{const char*_tmp8ED;const char*_tmp8EC;
struct Cyc_PP_Doc*_tmp8EB[3];s=((_tmp8EB[2]=Cyc_PP_text(((_tmp8EC=");",
_tag_dyneither(_tmp8EC,sizeof(char),3)))),((_tmp8EB[1]=Cyc_Absynpp_exps2doc_prec(
20,_tmp3B6),((_tmp8EB[0]=Cyc_PP_text(((_tmp8ED="fallthru(",_tag_dyneither(
_tmp8ED,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp8EB,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL211;_LL22C: {struct Cyc_Absyn_Decl_s_struct*_tmp3B7=(struct Cyc_Absyn_Decl_s_struct*)
_tmp394;if(_tmp3B7->tag != 12)goto _LL22E;else{_tmp3B8=_tmp3B7->f1;_tmp3B9=_tmp3B7->f2;}}
_LL22D:{struct Cyc_PP_Doc*_tmp8EE[3];s=((_tmp8EE[2]=Cyc_Absynpp_stmt2doc(_tmp3B9,
stmtexp),((_tmp8EE[1]=Cyc_PP_line_doc(),((_tmp8EE[0]=Cyc_Absynpp_decl2doc(
_tmp3B8),Cyc_PP_cat(_tag_dyneither(_tmp8EE,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL211;_LL22E: {struct Cyc_Absyn_Label_s_struct*_tmp3BA=(struct Cyc_Absyn_Label_s_struct*)
_tmp394;if(_tmp3BA->tag != 13)goto _LL230;else{_tmp3BB=_tmp3BA->f1;_tmp3BC=_tmp3BA->f2;}}
_LL22F: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3BC)){if(
stmtexp){const char*_tmp8F3;const char*_tmp8F2;struct Cyc_PP_Doc*_tmp8F1[8];s=((
_tmp8F1[7]=Cyc_PP_text(((_tmp8F2=");",_tag_dyneither(_tmp8F2,sizeof(char),3)))),((
_tmp8F1[6]=Cyc_Absynpp_rb(),((_tmp8F1[5]=Cyc_PP_line_doc(),((_tmp8F1[4]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3BC,1)),((_tmp8F1[3]=Cyc_PP_line_doc(),((_tmp8F1[2]=
Cyc_Absynpp_lb(),((_tmp8F1[1]=Cyc_PP_text(((_tmp8F3=": (",_tag_dyneither(_tmp8F3,
sizeof(char),4)))),((_tmp8F1[0]=Cyc_PP_textptr(_tmp3BB),Cyc_PP_cat(
_tag_dyneither(_tmp8F1,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{const
char*_tmp8F6;struct Cyc_PP_Doc*_tmp8F5[7];s=((_tmp8F5[6]=Cyc_Absynpp_rb(),((
_tmp8F5[5]=Cyc_PP_line_doc(),((_tmp8F5[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3BC,0)),((_tmp8F5[3]=Cyc_PP_line_doc(),((_tmp8F5[2]=Cyc_Absynpp_lb(),((
_tmp8F5[1]=Cyc_PP_text(((_tmp8F6=": ",_tag_dyneither(_tmp8F6,sizeof(char),3)))),((
_tmp8F5[0]=Cyc_PP_textptr(_tmp3BB),Cyc_PP_cat(_tag_dyneither(_tmp8F5,sizeof(
struct Cyc_PP_Doc*),7))))))))))))))));}}else{const char*_tmp8F9;struct Cyc_PP_Doc*
_tmp8F8[3];s=((_tmp8F8[2]=Cyc_Absynpp_stmt2doc(_tmp3BC,stmtexp),((_tmp8F8[1]=Cyc_PP_text(((
_tmp8F9=": ",_tag_dyneither(_tmp8F9,sizeof(char),3)))),((_tmp8F8[0]=Cyc_PP_textptr(
_tmp3BB),Cyc_PP_cat(_tag_dyneither(_tmp8F8,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL211;_LL230: {struct Cyc_Absyn_Do_s_struct*_tmp3BD=(struct Cyc_Absyn_Do_s_struct*)
_tmp394;if(_tmp3BD->tag != 14)goto _LL232;else{_tmp3BE=_tmp3BD->f1;_tmp3BF=_tmp3BD->f2;
_tmp3C0=_tmp3BF.f1;}}_LL231:{const char*_tmp900;const char*_tmp8FF;const char*
_tmp8FE;struct Cyc_PP_Doc*_tmp8FD[9];s=((_tmp8FD[8]=Cyc_PP_text(((_tmp8FE=");",
_tag_dyneither(_tmp8FE,sizeof(char),3)))),((_tmp8FD[7]=Cyc_Absynpp_exp2doc(
_tmp3C0),((_tmp8FD[6]=Cyc_PP_text(((_tmp8FF=" while (",_tag_dyneither(_tmp8FF,
sizeof(char),9)))),((_tmp8FD[5]=Cyc_Absynpp_rb(),((_tmp8FD[4]=Cyc_PP_line_doc(),((
_tmp8FD[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3BE,0)),((_tmp8FD[2]=Cyc_PP_line_doc(),((
_tmp8FD[1]=Cyc_Absynpp_lb(),((_tmp8FD[0]=Cyc_PP_text(((_tmp900="do ",
_tag_dyneither(_tmp900,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp8FD,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL211;_LL232: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp3C1=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp394;if(_tmp3C1->tag != 15)goto
_LL234;else{_tmp3C2=_tmp3C1->f1;_tmp3C3=_tmp3C1->f2;}}_LL233:{const char*_tmp905;
const char*_tmp904;struct Cyc_PP_Doc*_tmp903[12];s=((_tmp903[11]=Cyc_Absynpp_rb(),((
_tmp903[10]=Cyc_PP_line_doc(),((_tmp903[9]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp3C3)),((_tmp903[8]=Cyc_PP_line_doc(),((_tmp903[7]=Cyc_Absynpp_lb(),((_tmp903[
6]=Cyc_PP_text(((_tmp904=" catch ",_tag_dyneither(_tmp904,sizeof(char),8)))),((
_tmp903[5]=Cyc_Absynpp_rb(),((_tmp903[4]=Cyc_PP_line_doc(),((_tmp903[3]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3C2,0)),((_tmp903[2]=Cyc_PP_line_doc(),((_tmp903[1]=
Cyc_Absynpp_lb(),((_tmp903[0]=Cyc_PP_text(((_tmp905="try ",_tag_dyneither(
_tmp905,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp903,sizeof(struct Cyc_PP_Doc*),
12))))))))))))))))))))))))));}goto _LL211;_LL234: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp3C4=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp394;if(_tmp3C4->tag != 16)goto
_LL211;else{_tmp3C5=_tmp3C4->f1;}}_LL235:{const char*_tmp90A;const char*_tmp909;
struct Cyc_PP_Doc*_tmp908[3];s=((_tmp908[2]=Cyc_PP_text(((_tmp909=");",
_tag_dyneither(_tmp909,sizeof(char),3)))),((_tmp908[1]=Cyc_Absynpp_exp2doc(
_tmp3C5),((_tmp908[0]=Cyc_PP_text(((_tmp90A="reset_region(",_tag_dyneither(
_tmp90A,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp908,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL211;_LL211:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p);struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*
p){struct Cyc_PP_Doc*s;{void*_tmp40A=p->r;enum Cyc_Absyn_Sign _tmp40E;int _tmp40F;
char _tmp411;struct _dyneither_ptr _tmp413;struct Cyc_Absyn_Vardecl*_tmp415;struct
Cyc_Absyn_Pat*_tmp416;struct Cyc_Absyn_Pat _tmp417;void*_tmp418;struct Cyc_Absyn_Vardecl*
_tmp41B;struct Cyc_Absyn_Pat*_tmp41C;struct Cyc_Absyn_Tvar*_tmp41E;struct Cyc_Absyn_Vardecl*
_tmp41F;struct Cyc_List_List*_tmp421;int _tmp422;struct Cyc_Absyn_Pat*_tmp424;
struct Cyc_Absyn_Vardecl*_tmp426;struct Cyc_Absyn_Pat*_tmp427;struct Cyc_Absyn_Pat
_tmp428;void*_tmp429;struct Cyc_Absyn_Vardecl*_tmp42C;struct Cyc_Absyn_Pat*_tmp42D;
struct _tuple0*_tmp42F;struct _tuple0*_tmp431;struct Cyc_List_List*_tmp432;int
_tmp433;struct Cyc_Absyn_AggrInfo*_tmp435;struct Cyc_Absyn_AggrInfo _tmp436;union
Cyc_Absyn_AggrInfoU _tmp437;struct Cyc_List_List*_tmp438;struct Cyc_List_List*
_tmp439;int _tmp43A;struct Cyc_Absyn_AggrInfo*_tmp43C;struct Cyc_List_List*_tmp43D;
struct Cyc_List_List*_tmp43E;int _tmp43F;struct Cyc_Absyn_Enumfield*_tmp441;struct
Cyc_Absyn_Enumfield*_tmp443;struct Cyc_Absyn_Datatypefield*_tmp445;struct Cyc_List_List*
_tmp446;struct Cyc_Absyn_Datatypefield*_tmp448;struct Cyc_List_List*_tmp449;int
_tmp44A;struct Cyc_Absyn_Exp*_tmp44C;_LL23C: {struct Cyc_Absyn_Wild_p_struct*
_tmp40B=(struct Cyc_Absyn_Wild_p_struct*)_tmp40A;if(_tmp40B->tag != 0)goto _LL23E;}
_LL23D:{const char*_tmp90B;s=Cyc_PP_text(((_tmp90B="_",_tag_dyneither(_tmp90B,
sizeof(char),2))));}goto _LL23B;_LL23E: {struct Cyc_Absyn_Null_p_struct*_tmp40C=(
struct Cyc_Absyn_Null_p_struct*)_tmp40A;if(_tmp40C->tag != 8)goto _LL240;}_LL23F:{
const char*_tmp90C;s=Cyc_PP_text(((_tmp90C="NULL",_tag_dyneither(_tmp90C,sizeof(
char),5))));}goto _LL23B;_LL240: {struct Cyc_Absyn_Int_p_struct*_tmp40D=(struct Cyc_Absyn_Int_p_struct*)
_tmp40A;if(_tmp40D->tag != 9)goto _LL242;else{_tmp40E=_tmp40D->f1;_tmp40F=_tmp40D->f2;}}
_LL241: if(_tmp40E != (enum Cyc_Absyn_Sign)Cyc_Absyn_Unsigned){const char*_tmp910;
void*_tmp90F[1];struct Cyc_Int_pa_struct _tmp90E;s=Cyc_PP_text((struct
_dyneither_ptr)((_tmp90E.tag=1,((_tmp90E.f1=(unsigned long)_tmp40F,((_tmp90F[0]=&
_tmp90E,Cyc_aprintf(((_tmp910="%d",_tag_dyneither(_tmp910,sizeof(char),3))),
_tag_dyneither(_tmp90F,sizeof(void*),1)))))))));}else{const char*_tmp914;void*
_tmp913[1];struct Cyc_Int_pa_struct _tmp912;s=Cyc_PP_text((struct _dyneither_ptr)((
_tmp912.tag=1,((_tmp912.f1=(unsigned int)_tmp40F,((_tmp913[0]=& _tmp912,Cyc_aprintf(((
_tmp914="%u",_tag_dyneither(_tmp914,sizeof(char),3))),_tag_dyneither(_tmp913,
sizeof(void*),1)))))))));}goto _LL23B;_LL242: {struct Cyc_Absyn_Char_p_struct*
_tmp410=(struct Cyc_Absyn_Char_p_struct*)_tmp40A;if(_tmp410->tag != 10)goto _LL244;
else{_tmp411=_tmp410->f1;}}_LL243:{const char*_tmp918;void*_tmp917[1];struct Cyc_String_pa_struct
_tmp916;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp916.tag=0,((_tmp916.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp411)),((
_tmp917[0]=& _tmp916,Cyc_aprintf(((_tmp918="'%s'",_tag_dyneither(_tmp918,sizeof(
char),5))),_tag_dyneither(_tmp917,sizeof(void*),1)))))))));}goto _LL23B;_LL244: {
struct Cyc_Absyn_Float_p_struct*_tmp412=(struct Cyc_Absyn_Float_p_struct*)_tmp40A;
if(_tmp412->tag != 11)goto _LL246;else{_tmp413=_tmp412->f1;}}_LL245: s=Cyc_PP_text(
_tmp413);goto _LL23B;_LL246: {struct Cyc_Absyn_Var_p_struct*_tmp414=(struct Cyc_Absyn_Var_p_struct*)
_tmp40A;if(_tmp414->tag != 1)goto _LL248;else{_tmp415=_tmp414->f1;_tmp416=_tmp414->f2;
_tmp417=*_tmp416;_tmp418=_tmp417.r;{struct Cyc_Absyn_Wild_p_struct*_tmp419=(
struct Cyc_Absyn_Wild_p_struct*)_tmp418;if(_tmp419->tag != 0)goto _LL248;};}}_LL247:
s=Cyc_Absynpp_qvar2doc(_tmp415->name);goto _LL23B;_LL248: {struct Cyc_Absyn_Var_p_struct*
_tmp41A=(struct Cyc_Absyn_Var_p_struct*)_tmp40A;if(_tmp41A->tag != 1)goto _LL24A;
else{_tmp41B=_tmp41A->f1;_tmp41C=_tmp41A->f2;}}_LL249:{const char*_tmp91B;struct
Cyc_PP_Doc*_tmp91A[3];s=((_tmp91A[2]=Cyc_Absynpp_pat2doc(_tmp41C),((_tmp91A[1]=
Cyc_PP_text(((_tmp91B=" as ",_tag_dyneither(_tmp91B,sizeof(char),5)))),((_tmp91A[
0]=Cyc_Absynpp_qvar2doc(_tmp41B->name),Cyc_PP_cat(_tag_dyneither(_tmp91A,sizeof(
struct Cyc_PP_Doc*),3))))))));}goto _LL23B;_LL24A: {struct Cyc_Absyn_TagInt_p_struct*
_tmp41D=(struct Cyc_Absyn_TagInt_p_struct*)_tmp40A;if(_tmp41D->tag != 3)goto _LL24C;
else{_tmp41E=_tmp41D->f1;_tmp41F=_tmp41D->f2;}}_LL24B:{const char*_tmp920;const
char*_tmp91F;struct Cyc_PP_Doc*_tmp91E[4];s=((_tmp91E[3]=Cyc_PP_text(((_tmp91F=">",
_tag_dyneither(_tmp91F,sizeof(char),2)))),((_tmp91E[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp41E)),((_tmp91E[1]=Cyc_PP_text(((_tmp920="<",_tag_dyneither(_tmp920,sizeof(
char),2)))),((_tmp91E[0]=Cyc_Absynpp_qvar2doc(_tmp41F->name),Cyc_PP_cat(
_tag_dyneither(_tmp91E,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL23B;_LL24C: {
struct Cyc_Absyn_Tuple_p_struct*_tmp420=(struct Cyc_Absyn_Tuple_p_struct*)_tmp40A;
if(_tmp420->tag != 4)goto _LL24E;else{_tmp421=_tmp420->f1;_tmp422=_tmp420->f2;}}
_LL24D:{const char*_tmp929;const char*_tmp928;const char*_tmp927;const char*_tmp926;
struct Cyc_PP_Doc*_tmp925[4];s=((_tmp925[3]=_tmp422?Cyc_PP_text(((_tmp926=", ...)",
_tag_dyneither(_tmp926,sizeof(char),7)))): Cyc_PP_text(((_tmp927=")",
_tag_dyneither(_tmp927,sizeof(char),2)))),((_tmp925[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp928=",",_tag_dyneither(_tmp928,
sizeof(char),2))),_tmp421),((_tmp925[1]=Cyc_PP_text(((_tmp929="(",_tag_dyneither(
_tmp929,sizeof(char),2)))),((_tmp925[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(
_tag_dyneither(_tmp925,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL23B;_LL24E: {
struct Cyc_Absyn_Pointer_p_struct*_tmp423=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp40A;if(_tmp423->tag != 5)goto _LL250;else{_tmp424=_tmp423->f1;}}_LL24F:{const
char*_tmp92C;struct Cyc_PP_Doc*_tmp92B[2];s=((_tmp92B[1]=Cyc_Absynpp_pat2doc(
_tmp424),((_tmp92B[0]=Cyc_PP_text(((_tmp92C="&",_tag_dyneither(_tmp92C,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp92B,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL23B;_LL250: {struct Cyc_Absyn_Reference_p_struct*_tmp425=(struct Cyc_Absyn_Reference_p_struct*)
_tmp40A;if(_tmp425->tag != 2)goto _LL252;else{_tmp426=_tmp425->f1;_tmp427=_tmp425->f2;
_tmp428=*_tmp427;_tmp429=_tmp428.r;{struct Cyc_Absyn_Wild_p_struct*_tmp42A=(
struct Cyc_Absyn_Wild_p_struct*)_tmp429;if(_tmp42A->tag != 0)goto _LL252;};}}_LL251:{
const char*_tmp92F;struct Cyc_PP_Doc*_tmp92E[2];s=((_tmp92E[1]=Cyc_Absynpp_qvar2doc(
_tmp426->name),((_tmp92E[0]=Cyc_PP_text(((_tmp92F="*",_tag_dyneither(_tmp92F,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp92E,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL23B;_LL252: {struct Cyc_Absyn_Reference_p_struct*_tmp42B=(struct Cyc_Absyn_Reference_p_struct*)
_tmp40A;if(_tmp42B->tag != 2)goto _LL254;else{_tmp42C=_tmp42B->f1;_tmp42D=_tmp42B->f2;}}
_LL253:{const char*_tmp934;const char*_tmp933;struct Cyc_PP_Doc*_tmp932[4];s=((
_tmp932[3]=Cyc_Absynpp_pat2doc(_tmp42D),((_tmp932[2]=Cyc_PP_text(((_tmp933=" as ",
_tag_dyneither(_tmp933,sizeof(char),5)))),((_tmp932[1]=Cyc_Absynpp_qvar2doc(
_tmp42C->name),((_tmp932[0]=Cyc_PP_text(((_tmp934="*",_tag_dyneither(_tmp934,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp932,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL23B;_LL254: {struct Cyc_Absyn_UnknownId_p_struct*_tmp42E=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp40A;if(_tmp42E->tag != 14)goto _LL256;else{_tmp42F=_tmp42E->f1;}}_LL255: s=Cyc_Absynpp_qvar2doc(
_tmp42F);goto _LL23B;_LL256: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp430=(
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp40A;if(_tmp430->tag != 15)goto _LL258;
else{_tmp431=_tmp430->f1;_tmp432=_tmp430->f2;_tmp433=_tmp430->f3;}}_LL257: {
const char*_tmp936;const char*_tmp935;struct _dyneither_ptr term=_tmp433?(_tmp936=", ...)",
_tag_dyneither(_tmp936,sizeof(char),7)):((_tmp935=")",_tag_dyneither(_tmp935,
sizeof(char),2)));{const char*_tmp93B;const char*_tmp93A;struct Cyc_PP_Doc*_tmp939[
2];s=((_tmp939[1]=Cyc_PP_group(((_tmp93B="(",_tag_dyneither(_tmp93B,sizeof(char),
2))),term,((_tmp93A=",",_tag_dyneither(_tmp93A,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_pat2doc,_tmp432)),((_tmp939[0]=Cyc_Absynpp_qvar2doc(_tmp431),Cyc_PP_cat(
_tag_dyneither(_tmp939,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL23B;}_LL258: {
struct Cyc_Absyn_Aggr_p_struct*_tmp434=(struct Cyc_Absyn_Aggr_p_struct*)_tmp40A;
if(_tmp434->tag != 6)goto _LL25A;else{_tmp435=_tmp434->f1;if(_tmp435 == 0)goto
_LL25A;_tmp436=*_tmp435;_tmp437=_tmp436.aggr_info;_tmp438=_tmp434->f2;_tmp439=
_tmp434->f3;_tmp43A=_tmp434->f4;}}_LL259: {const char*_tmp93D;const char*_tmp93C;
struct _dyneither_ptr term=_tmp43A?(_tmp93D=", ...}",_tag_dyneither(_tmp93D,
sizeof(char),7)):((_tmp93C="}",_tag_dyneither(_tmp93C,sizeof(char),2)));struct
_tuple0*_tmp46F;struct _tuple9 _tmp46E=Cyc_Absyn_aggr_kinded_name(_tmp437);_tmp46F=
_tmp46E.f2;{const char*_tmp948;const char*_tmp947;const char*_tmp946;const char*
_tmp945;const char*_tmp944;struct Cyc_PP_Doc*_tmp943[4];s=((_tmp943[3]=Cyc_PP_group(((
_tmp945="",_tag_dyneither(_tmp945,sizeof(char),1))),term,((_tmp944=",",
_tag_dyneither(_tmp944,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp439)),((_tmp943[2]=Cyc_PP_egroup(((_tmp948="[",_tag_dyneither(_tmp948,
sizeof(char),2))),((_tmp947="]",_tag_dyneither(_tmp947,sizeof(char),2))),((
_tmp946=",",_tag_dyneither(_tmp946,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp438))),((_tmp943[1]=
Cyc_Absynpp_lb(),((_tmp943[0]=Cyc_Absynpp_qvar2doc(_tmp46F),Cyc_PP_cat(
_tag_dyneither(_tmp943,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL23B;}_LL25A: {
struct Cyc_Absyn_Aggr_p_struct*_tmp43B=(struct Cyc_Absyn_Aggr_p_struct*)_tmp40A;
if(_tmp43B->tag != 6)goto _LL25C;else{_tmp43C=_tmp43B->f1;if(_tmp43C != 0)goto
_LL25C;_tmp43D=_tmp43B->f2;_tmp43E=_tmp43B->f3;_tmp43F=_tmp43B->f4;}}_LL25B: {
const char*_tmp94A;const char*_tmp949;struct _dyneither_ptr term=_tmp43F?(_tmp94A=", ...}",
_tag_dyneither(_tmp94A,sizeof(char),7)):((_tmp949="}",_tag_dyneither(_tmp949,
sizeof(char),2)));{const char*_tmp955;const char*_tmp954;const char*_tmp953;const
char*_tmp952;const char*_tmp951;struct Cyc_PP_Doc*_tmp950[3];s=((_tmp950[2]=Cyc_PP_group(((
_tmp952="",_tag_dyneither(_tmp952,sizeof(char),1))),term,((_tmp951=",",
_tag_dyneither(_tmp951,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp43E)),((_tmp950[1]=Cyc_PP_egroup(((_tmp955="[",_tag_dyneither(_tmp955,
sizeof(char),2))),((_tmp954="]",_tag_dyneither(_tmp954,sizeof(char),2))),((
_tmp953=",",_tag_dyneither(_tmp953,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp43D))),((_tmp950[0]=
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp950,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL23B;}_LL25C: {struct Cyc_Absyn_Enum_p_struct*_tmp440=(struct Cyc_Absyn_Enum_p_struct*)
_tmp40A;if(_tmp440->tag != 12)goto _LL25E;else{_tmp441=_tmp440->f2;}}_LL25D:
_tmp443=_tmp441;goto _LL25F;_LL25E: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp442=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp40A;if(_tmp442->tag != 13)goto _LL260;else{
_tmp443=_tmp442->f2;}}_LL25F: s=Cyc_Absynpp_qvar2doc(_tmp443->name);goto _LL23B;
_LL260: {struct Cyc_Absyn_Datatype_p_struct*_tmp444=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp40A;if(_tmp444->tag != 7)goto _LL262;else{_tmp445=_tmp444->f2;_tmp446=_tmp444->f3;
if(_tmp446 != 0)goto _LL262;}}_LL261: s=Cyc_Absynpp_qvar2doc(_tmp445->name);goto
_LL23B;_LL262: {struct Cyc_Absyn_Datatype_p_struct*_tmp447=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp40A;if(_tmp447->tag != 7)goto _LL264;else{_tmp448=_tmp447->f2;_tmp449=_tmp447->f3;
_tmp44A=_tmp447->f4;}}_LL263: {const char*_tmp957;const char*_tmp956;struct
_dyneither_ptr term=_tmp44A?(_tmp957=", ...)",_tag_dyneither(_tmp957,sizeof(char),
7)):((_tmp956=")",_tag_dyneither(_tmp956,sizeof(char),2)));{const char*_tmp95C;
const char*_tmp95B;struct Cyc_PP_Doc*_tmp95A[2];s=((_tmp95A[1]=Cyc_PP_egroup(((
_tmp95C="(",_tag_dyneither(_tmp95C,sizeof(char),2))),term,((_tmp95B=",",
_tag_dyneither(_tmp95B,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,
_tmp449)),((_tmp95A[0]=Cyc_Absynpp_qvar2doc(_tmp448->name),Cyc_PP_cat(
_tag_dyneither(_tmp95A,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL23B;}_LL264: {
struct Cyc_Absyn_Exp_p_struct*_tmp44B=(struct Cyc_Absyn_Exp_p_struct*)_tmp40A;if(
_tmp44B->tag != 16)goto _LL23B;else{_tmp44C=_tmp44B->f1;}}_LL265: s=Cyc_Absynpp_exp2doc(
_tmp44C);goto _LL23B;_LL23B:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct
_tuple13*dp);struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){const char*
_tmp963;const char*_tmp962;const char*_tmp961;struct Cyc_PP_Doc*_tmp960[2];return(
_tmp960[1]=Cyc_Absynpp_pat2doc((*dp).f2),((_tmp960[0]=Cyc_PP_egroup(((_tmp963="",
_tag_dyneither(_tmp963,sizeof(char),1))),((_tmp962="=",_tag_dyneither(_tmp962,
sizeof(char),2))),((_tmp961="=",_tag_dyneither(_tmp961,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp960,sizeof(
struct Cyc_PP_Doc*),2)))));}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c);struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0  && (c->pattern)->r == (void*)&
Cyc_Absyn_Wild_p_val){const char*_tmp968;struct Cyc_PP_Doc*_tmp967[2];struct Cyc_PP_Doc*
_tmp966[2];return(_tmp966[1]=Cyc_PP_nest(2,((_tmp967[1]=Cyc_Absynpp_stmt2doc(c->body,
0),((_tmp967[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp967,sizeof(
struct Cyc_PP_Doc*),2))))))),((_tmp966[0]=Cyc_PP_text(((_tmp968="default: ",
_tag_dyneither(_tmp968,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp966,
sizeof(struct Cyc_PP_Doc*),2)))));}else{if(c->where_clause == 0){const char*_tmp96F;
const char*_tmp96E;struct Cyc_PP_Doc*_tmp96D[2];struct Cyc_PP_Doc*_tmp96C[4];return(
_tmp96C[3]=Cyc_PP_nest(2,((_tmp96D[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp96D[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp96D,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp96C[2]=Cyc_PP_text(((_tmp96E=": ",_tag_dyneither(_tmp96E,sizeof(
char),3)))),((_tmp96C[1]=Cyc_Absynpp_pat2doc(c->pattern),((_tmp96C[0]=Cyc_PP_text(((
_tmp96F="case ",_tag_dyneither(_tmp96F,sizeof(char),6)))),Cyc_PP_cat(
_tag_dyneither(_tmp96C,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{const char*
_tmp978;const char*_tmp977;const char*_tmp976;struct Cyc_PP_Doc*_tmp975[2];struct
Cyc_PP_Doc*_tmp974[6];return(_tmp974[5]=Cyc_PP_nest(2,((_tmp975[1]=Cyc_Absynpp_stmt2doc(
c->body,0),((_tmp975[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp975,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp974[4]=Cyc_PP_text(((_tmp976=": ",
_tag_dyneither(_tmp976,sizeof(char),3)))),((_tmp974[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp974[2]=Cyc_PP_text(((
_tmp977=" && ",_tag_dyneither(_tmp977,sizeof(char),5)))),((_tmp974[1]=Cyc_Absynpp_pat2doc(
c->pattern),((_tmp974[0]=Cyc_PP_text(((_tmp978="case ",_tag_dyneither(_tmp978,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp974,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);struct Cyc_PP_Doc*
Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){const char*_tmp979;return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((
_tmp979="",_tag_dyneither(_tmp979,sizeof(char),1))),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f);struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct
Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(f->name);else{
const char*_tmp97C;struct Cyc_PP_Doc*_tmp97B[3];return(_tmp97B[2]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp97B[1]=Cyc_PP_text(((_tmp97C=" = ",
_tag_dyneither(_tmp97C,sizeof(char),4)))),((_tmp97B[0]=Cyc_Absynpp_qvar2doc(f->name),
Cyc_PP_cat(_tag_dyneither(_tmp97B,sizeof(struct Cyc_PP_Doc*),3)))))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List*fs){const char*_tmp97D;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp97D=",",_tag_dyneither(_tmp97D,
sizeof(char),2))),fs);}static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*
v);static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return
Cyc_Absynpp_qvar2doc(v->name);}static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct
Cyc_List_List*vds);static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*
vds){const char*_tmp97E;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(
Cyc_Absynpp_id2doc,((_tmp97E=",",_tag_dyneither(_tmp97E,sizeof(char),2))),vds);}
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);struct Cyc_PP_Doc*
Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl
_tmp49B;enum Cyc_Absyn_Scope _tmp49C;struct _tuple0*_tmp49D;struct Cyc_Absyn_Tqual
_tmp49E;void*_tmp49F;struct Cyc_Absyn_Exp*_tmp4A0;struct Cyc_List_List*_tmp4A1;
struct Cyc_Absyn_Vardecl*_tmp49A=vd;_tmp49B=*_tmp49A;_tmp49C=_tmp49B.sc;_tmp49D=
_tmp49B.name;_tmp49E=_tmp49B.tq;_tmp49F=_tmp49B.type;_tmp4A0=_tmp49B.initializer;
_tmp4A1=_tmp49B.attributes;{struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(
_tmp49D);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4A1);struct Cyc_PP_Doc*
beforenamedoc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL266:
beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL267:{void*_tmp4A2=Cyc_Tcutil_compress(
_tmp49F);struct Cyc_Absyn_FnInfo _tmp4A4;struct Cyc_List_List*_tmp4A5;_LL26A: {
struct Cyc_Absyn_FnType_struct*_tmp4A3=(struct Cyc_Absyn_FnType_struct*)_tmp4A2;
if(_tmp4A3->tag != 10)goto _LL26C;else{_tmp4A4=_tmp4A3->f1;_tmp4A5=_tmp4A4.attributes;}}
_LL26B: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4A5);goto _LL269;_LL26C:;_LL26D:
beforenamedoc=Cyc_PP_nil_doc();goto _LL269;_LL269:;}break;}{struct Cyc_PP_Doc*
tmp_doc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL26E: tmp_doc=Cyc_PP_nil_doc();
break;case Cyc_Cyclone_Vc_c: _LL26F: tmp_doc=attsdoc;break;}{struct Cyc_Core_Opt*
_tmp98B;struct Cyc_PP_Doc*_tmp98A[2];struct Cyc_PP_Doc*_tmp989[2];const char*
_tmp988;const char*_tmp987;struct Cyc_PP_Doc*_tmp986[5];s=((_tmp986[4]=Cyc_PP_text(((
_tmp987=";",_tag_dyneither(_tmp987,sizeof(char),2)))),((_tmp986[3]=_tmp4A0 == 0?
Cyc_PP_nil_doc():((_tmp989[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp4A0),((
_tmp989[0]=Cyc_PP_text(((_tmp988=" = ",_tag_dyneither(_tmp988,sizeof(char),4)))),
Cyc_PP_cat(_tag_dyneither(_tmp989,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp986[2]=
Cyc_Absynpp_tqtd2doc(_tmp49E,_tmp49F,((_tmp98B=_cycalloc(sizeof(*_tmp98B)),((
_tmp98B->v=((_tmp98A[1]=sn,((_tmp98A[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(
_tmp98A,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp98B))))),((_tmp986[1]=Cyc_Absynpp_scope2doc(
_tmp49C),((_tmp986[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp986,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}return s;};};}struct _tuple17{struct Cyc_Position_Segment*f1;struct
_tuple0*f2;int f3;};struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x);
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){struct _tuple0*_tmp4AD;
struct _tuple17 _tmp4AC=*x;_tmp4AD=_tmp4AC.f2;return Cyc_Absynpp_qvar2doc(_tmp4AD);}
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);struct Cyc_PP_Doc*
Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*_tmp4AE=d->r;
struct Cyc_Absyn_Fndecl*_tmp4B0;struct Cyc_Absyn_Aggrdecl*_tmp4B2;struct Cyc_Absyn_Vardecl*
_tmp4B4;struct Cyc_Absyn_Tvar*_tmp4B6;struct Cyc_Absyn_Vardecl*_tmp4B7;int _tmp4B8;
struct Cyc_Absyn_Exp*_tmp4B9;struct Cyc_Absyn_Exp*_tmp4BB;struct Cyc_Absyn_Tvar*
_tmp4BC;struct Cyc_Absyn_Vardecl*_tmp4BD;struct Cyc_Absyn_Datatypedecl*_tmp4BF;
struct Cyc_Absyn_Datatypedecl _tmp4C0;enum Cyc_Absyn_Scope _tmp4C1;struct _tuple0*
_tmp4C2;struct Cyc_List_List*_tmp4C3;struct Cyc_Core_Opt*_tmp4C4;int _tmp4C5;struct
Cyc_Absyn_Pat*_tmp4C7;struct Cyc_Absyn_Exp*_tmp4C8;struct Cyc_List_List*_tmp4CA;
struct Cyc_Absyn_Enumdecl*_tmp4CC;struct Cyc_Absyn_Enumdecl _tmp4CD;enum Cyc_Absyn_Scope
_tmp4CE;struct _tuple0*_tmp4CF;struct Cyc_Core_Opt*_tmp4D0;struct Cyc_Absyn_Typedefdecl*
_tmp4D2;struct _dyneither_ptr*_tmp4D4;struct Cyc_List_List*_tmp4D5;struct _tuple0*
_tmp4D7;struct Cyc_List_List*_tmp4D8;struct Cyc_List_List*_tmp4DA;struct Cyc_List_List*
_tmp4DC;struct Cyc_List_List*_tmp4DD;_LL272: {struct Cyc_Absyn_Fn_d_struct*_tmp4AF=(
struct Cyc_Absyn_Fn_d_struct*)_tmp4AE;if(_tmp4AF->tag != 1)goto _LL274;else{_tmp4B0=
_tmp4AF->f1;}}_LL273: {struct Cyc_Absyn_FnType_struct _tmp991;struct Cyc_Absyn_FnInfo
_tmp990;struct Cyc_Absyn_FnType_struct*_tmp98F;void*t=(void*)((_tmp98F=_cycalloc(
sizeof(*_tmp98F)),((_tmp98F[0]=((_tmp991.tag=10,((_tmp991.f1=((_tmp990.tvars=
_tmp4B0->tvs,((_tmp990.effect=_tmp4B0->effect,((_tmp990.ret_typ=_tmp4B0->ret_type,((
_tmp990.args=((struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple11*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp4B0->args),((
_tmp990.c_varargs=_tmp4B0->c_varargs,((_tmp990.cyc_varargs=_tmp4B0->cyc_varargs,((
_tmp990.rgn_po=_tmp4B0->rgn_po,((_tmp990.attributes=0,_tmp990)))))))))))))))),
_tmp991)))),_tmp98F))));struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4B0->attributes);
struct Cyc_PP_Doc*inlinedoc;if(_tmp4B0->is_inline)switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL292:{const char*_tmp992;inlinedoc=Cyc_PP_text(((_tmp992="inline ",
_tag_dyneither(_tmp992,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL293:{
const char*_tmp993;inlinedoc=Cyc_PP_text(((_tmp993="__inline ",_tag_dyneither(
_tmp993,sizeof(char),10))));}break;}else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*
scopedoc=Cyc_Absynpp_scope2doc(_tmp4B0->sc);struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL295: beforenamedoc=
attsdoc;break;case Cyc_Cyclone_Vc_c: _LL296: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp4B0->attributes);break;}{struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp4B0->name);struct Cyc_PP_Doc*_tmp996[2];struct Cyc_Core_Opt*_tmp995;struct Cyc_PP_Doc*
tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,((_tmp995=_cycalloc(
sizeof(*_tmp995)),((_tmp995->v=((_tmp996[1]=namedoc,((_tmp996[0]=beforenamedoc,
Cyc_PP_cat(_tag_dyneither(_tmp996,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp995)))));
struct Cyc_PP_Doc*_tmp999[2];struct Cyc_PP_Doc*_tmp998[5];struct Cyc_PP_Doc*bodydoc=(
_tmp998[4]=Cyc_Absynpp_rb(),((_tmp998[3]=Cyc_PP_line_doc(),((_tmp998[2]=Cyc_PP_nest(
2,((_tmp999[1]=Cyc_Absynpp_stmt2doc(_tmp4B0->body,0),((_tmp999[0]=Cyc_PP_line_doc(),
Cyc_PP_cat(_tag_dyneither(_tmp999,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp998[1]=
Cyc_Absynpp_lb(),((_tmp998[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(
_tmp998,sizeof(struct Cyc_PP_Doc*),5)))))))))));{struct Cyc_PP_Doc*_tmp99A[4];s=((
_tmp99A[3]=bodydoc,((_tmp99A[2]=tqtddoc,((_tmp99A[1]=scopedoc,((_tmp99A[0]=
inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmp99A,sizeof(struct Cyc_PP_Doc*),4))))))))));}
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL298:{struct Cyc_PP_Doc*
_tmp99B[2];s=((_tmp99B[1]=s,((_tmp99B[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(
_tmp99B,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL299: break;}goto
_LL271;};};}_LL274: {struct Cyc_Absyn_Aggr_d_struct*_tmp4B1=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp4AE;if(_tmp4B1->tag != 6)goto _LL276;else{_tmp4B2=_tmp4B1->f1;}}_LL275: if(
_tmp4B2->impl == 0){const char*_tmp99E;struct Cyc_PP_Doc*_tmp99D[5];s=((_tmp99D[4]=
Cyc_PP_text(((_tmp99E=";",_tag_dyneither(_tmp99E,sizeof(char),2)))),((_tmp99D[3]=
Cyc_Absynpp_ktvars2doc(_tmp4B2->tvs),((_tmp99D[2]=Cyc_Absynpp_qvar2bolddoc(
_tmp4B2->name),((_tmp99D[1]=Cyc_Absynpp_aggr_kind2doc(_tmp4B2->kind),((_tmp99D[0]=
Cyc_Absynpp_scope2doc(_tmp4B2->sc),Cyc_PP_cat(_tag_dyneither(_tmp99D,sizeof(
struct Cyc_PP_Doc*),5))))))))))));}else{struct Cyc_PP_Doc*_tmp9A8[2];const char*
_tmp9A7;struct Cyc_PP_Doc*_tmp9A6[2];const char*_tmp9A5;struct Cyc_PP_Doc*_tmp9A4[
13];s=((_tmp9A4[12]=Cyc_PP_text(((_tmp9A5=";",_tag_dyneither(_tmp9A5,sizeof(char),
2)))),((_tmp9A4[11]=Cyc_Absynpp_atts2doc(_tmp4B2->attributes),((_tmp9A4[10]=Cyc_Absynpp_rb(),((
_tmp9A4[9]=Cyc_PP_line_doc(),((_tmp9A4[8]=Cyc_PP_nest(2,((_tmp9A6[1]=Cyc_Absynpp_aggrfields2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4B2->impl))->fields),((_tmp9A6[0]=
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9A6,sizeof(struct Cyc_PP_Doc*),2))))))),((
_tmp9A4[7]=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4B2->impl))->rgn_po == 
0?Cyc_PP_nil_doc():((_tmp9A8[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4B2->impl))->rgn_po),((_tmp9A8[0]=Cyc_PP_text(((_tmp9A7=":",
_tag_dyneither(_tmp9A7,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp9A8,
sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9A4[6]=Cyc_Absynpp_ktvars2doc(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4B2->impl))->exist_vars),((_tmp9A4[5]=
Cyc_Absynpp_lb(),((_tmp9A4[4]=Cyc_PP_blank_doc(),((_tmp9A4[3]=Cyc_Absynpp_ktvars2doc(
_tmp4B2->tvs),((_tmp9A4[2]=Cyc_Absynpp_qvar2bolddoc(_tmp4B2->name),((_tmp9A4[1]=
Cyc_Absynpp_aggr_kind2doc(_tmp4B2->kind),((_tmp9A4[0]=Cyc_Absynpp_scope2doc(
_tmp4B2->sc),Cyc_PP_cat(_tag_dyneither(_tmp9A4,sizeof(struct Cyc_PP_Doc*),13))))))))))))))))))))))))))));}
goto _LL271;_LL276: {struct Cyc_Absyn_Var_d_struct*_tmp4B3=(struct Cyc_Absyn_Var_d_struct*)
_tmp4AE;if(_tmp4B3->tag != 0)goto _LL278;else{_tmp4B4=_tmp4B3->f1;}}_LL277: s=Cyc_Absynpp_vardecl2doc(
_tmp4B4);goto _LL271;_LL278: {struct Cyc_Absyn_Region_d_struct*_tmp4B5=(struct Cyc_Absyn_Region_d_struct*)
_tmp4AE;if(_tmp4B5->tag != 4)goto _LL27A;else{_tmp4B6=_tmp4B5->f1;_tmp4B7=_tmp4B5->f2;
_tmp4B8=_tmp4B5->f3;_tmp4B9=_tmp4B5->f4;}}_LL279:{const char*_tmp9BB;const char*
_tmp9BA;const char*_tmp9B9;const char*_tmp9B8;struct Cyc_PP_Doc*_tmp9B7[3];const
char*_tmp9B6;const char*_tmp9B5;const char*_tmp9B4;struct Cyc_PP_Doc*_tmp9B3[8];s=((
_tmp9B3[7]=Cyc_PP_text(((_tmp9B4=";",_tag_dyneither(_tmp9B4,sizeof(char),2)))),((
_tmp9B3[6]=_tmp4B9 != 0?(_tmp9B7[2]=Cyc_PP_text(((_tmp9B6=")",_tag_dyneither(
_tmp9B6,sizeof(char),2)))),((_tmp9B7[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_tmp4B9),((_tmp9B7[0]=Cyc_PP_text(((_tmp9B5=" = open(",_tag_dyneither(_tmp9B5,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp9B7,sizeof(struct Cyc_PP_Doc*),3))))))):
Cyc_PP_nil_doc(),((_tmp9B3[5]=_tmp4B8?Cyc_PP_text(((_tmp9B8=" @resetable",
_tag_dyneither(_tmp9B8,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmp9B3[4]=Cyc_Absynpp_qvar2doc(
_tmp4B7->name),((_tmp9B3[3]=Cyc_PP_text(((_tmp9B9=">",_tag_dyneither(_tmp9B9,
sizeof(char),2)))),((_tmp9B3[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4B6)),((
_tmp9B3[1]=Cyc_PP_text(((_tmp9BA="<",_tag_dyneither(_tmp9BA,sizeof(char),2)))),((
_tmp9B3[0]=Cyc_PP_text(((_tmp9BB="region",_tag_dyneither(_tmp9BB,sizeof(char),7)))),
Cyc_PP_cat(_tag_dyneither(_tmp9B3,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL271;_LL27A: {struct Cyc_Absyn_Alias_d_struct*_tmp4BA=(struct Cyc_Absyn_Alias_d_struct*)
_tmp4AE;if(_tmp4BA->tag != 5)goto _LL27C;else{_tmp4BB=_tmp4BA->f1;_tmp4BC=_tmp4BA->f2;
_tmp4BD=_tmp4BA->f3;}}_LL27B:{const char*_tmp9C6;const char*_tmp9C5;const char*
_tmp9C4;const char*_tmp9C3;const char*_tmp9C2;struct Cyc_PP_Doc*_tmp9C1[8];s=((
_tmp9C1[7]=Cyc_PP_text(((_tmp9C2=";",_tag_dyneither(_tmp9C2,sizeof(char),2)))),((
_tmp9C1[6]=Cyc_Absynpp_exp2doc(_tmp4BB),((_tmp9C1[5]=Cyc_PP_text(((_tmp9C3=" = ",
_tag_dyneither(_tmp9C3,sizeof(char),4)))),((_tmp9C1[4]=Cyc_Absynpp_qvar2doc(
_tmp4BD->name),((_tmp9C1[3]=Cyc_PP_text(((_tmp9C4="> ",_tag_dyneither(_tmp9C4,
sizeof(char),3)))),((_tmp9C1[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4BC)),((
_tmp9C1[1]=Cyc_PP_text(((_tmp9C5=" <",_tag_dyneither(_tmp9C5,sizeof(char),3)))),((
_tmp9C1[0]=Cyc_PP_text(((_tmp9C6="alias",_tag_dyneither(_tmp9C6,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp9C1,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL271;_LL27C: {struct Cyc_Absyn_Datatype_d_struct*_tmp4BE=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp4AE;if(_tmp4BE->tag != 7)goto _LL27E;else{_tmp4BF=_tmp4BE->f1;_tmp4C0=*_tmp4BF;
_tmp4C1=_tmp4C0.sc;_tmp4C2=_tmp4C0.name;_tmp4C3=_tmp4C0.tvs;_tmp4C4=_tmp4C0.fields;
_tmp4C5=_tmp4C0.is_extensible;}}_LL27D: if(_tmp4C4 == 0){const char*_tmp9CD;const
char*_tmp9CC;const char*_tmp9CB;struct Cyc_PP_Doc*_tmp9CA[6];s=((_tmp9CA[5]=Cyc_PP_text(((
_tmp9CB=";",_tag_dyneither(_tmp9CB,sizeof(char),2)))),((_tmp9CA[4]=Cyc_Absynpp_ktvars2doc(
_tmp4C3),((_tmp9CA[3]=_tmp4C5?Cyc_Absynpp_qvar2bolddoc(_tmp4C2): Cyc_Absynpp_typedef_name2bolddoc(
_tmp4C2),((_tmp9CA[2]=Cyc_PP_text(((_tmp9CC="datatype ",_tag_dyneither(_tmp9CC,
sizeof(char),10)))),((_tmp9CA[1]=_tmp4C5?Cyc_PP_text(((_tmp9CD="@extensible",
_tag_dyneither(_tmp9CD,sizeof(char),12)))): Cyc_PP_blank_doc(),((_tmp9CA[0]=Cyc_Absynpp_scope2doc(
_tmp4C1),Cyc_PP_cat(_tag_dyneither(_tmp9CA,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmp9D6;const char*_tmp9D5;struct Cyc_PP_Doc*_tmp9D4[2];const char*
_tmp9D3;struct Cyc_PP_Doc*_tmp9D2[11];s=((_tmp9D2[10]=Cyc_PP_text(((_tmp9D3=";",
_tag_dyneither(_tmp9D3,sizeof(char),2)))),((_tmp9D2[9]=Cyc_Absynpp_rb(),((
_tmp9D2[8]=Cyc_PP_line_doc(),((_tmp9D2[7]=Cyc_PP_nest(2,((_tmp9D4[1]=Cyc_Absynpp_datatypefields2doc((
struct Cyc_List_List*)_tmp4C4->v),((_tmp9D4[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp9D4,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9D2[6]=Cyc_Absynpp_lb(),((
_tmp9D2[5]=Cyc_PP_blank_doc(),((_tmp9D2[4]=Cyc_Absynpp_ktvars2doc(_tmp4C3),((
_tmp9D2[3]=_tmp4C5?Cyc_Absynpp_qvar2bolddoc(_tmp4C2): Cyc_Absynpp_typedef_name2bolddoc(
_tmp4C2),((_tmp9D2[2]=Cyc_PP_text(((_tmp9D5="datatype ",_tag_dyneither(_tmp9D5,
sizeof(char),10)))),((_tmp9D2[1]=_tmp4C5?Cyc_PP_text(((_tmp9D6="@extensible ",
_tag_dyneither(_tmp9D6,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9D2[0]=Cyc_Absynpp_scope2doc(
_tmp4C1),Cyc_PP_cat(_tag_dyneither(_tmp9D2,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL271;_LL27E: {struct Cyc_Absyn_Let_d_struct*_tmp4C6=(struct Cyc_Absyn_Let_d_struct*)
_tmp4AE;if(_tmp4C6->tag != 2)goto _LL280;else{_tmp4C7=_tmp4C6->f1;_tmp4C8=_tmp4C6->f3;}}
_LL27F:{const char*_tmp9DD;const char*_tmp9DC;const char*_tmp9DB;struct Cyc_PP_Doc*
_tmp9DA[5];s=((_tmp9DA[4]=Cyc_PP_text(((_tmp9DB=";",_tag_dyneither(_tmp9DB,
sizeof(char),2)))),((_tmp9DA[3]=Cyc_Absynpp_exp2doc(_tmp4C8),((_tmp9DA[2]=Cyc_PP_text(((
_tmp9DC=" = ",_tag_dyneither(_tmp9DC,sizeof(char),4)))),((_tmp9DA[1]=Cyc_Absynpp_pat2doc(
_tmp4C7),((_tmp9DA[0]=Cyc_PP_text(((_tmp9DD="let ",_tag_dyneither(_tmp9DD,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9DA,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL271;_LL280: {struct Cyc_Absyn_Letv_d_struct*_tmp4C9=(struct Cyc_Absyn_Letv_d_struct*)
_tmp4AE;if(_tmp4C9->tag != 3)goto _LL282;else{_tmp4CA=_tmp4C9->f1;}}_LL281:{const
char*_tmp9E2;const char*_tmp9E1;struct Cyc_PP_Doc*_tmp9E0[3];s=((_tmp9E0[2]=Cyc_PP_text(((
_tmp9E1=";",_tag_dyneither(_tmp9E1,sizeof(char),2)))),((_tmp9E0[1]=Cyc_Absynpp_ids2doc(
_tmp4CA),((_tmp9E0[0]=Cyc_PP_text(((_tmp9E2="let ",_tag_dyneither(_tmp9E2,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9E0,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL271;_LL282: {struct Cyc_Absyn_Enum_d_struct*_tmp4CB=(struct Cyc_Absyn_Enum_d_struct*)
_tmp4AE;if(_tmp4CB->tag != 8)goto _LL284;else{_tmp4CC=_tmp4CB->f1;_tmp4CD=*_tmp4CC;
_tmp4CE=_tmp4CD.sc;_tmp4CF=_tmp4CD.name;_tmp4D0=_tmp4CD.fields;}}_LL283: if(
_tmp4D0 == 0){const char*_tmp9E7;const char*_tmp9E6;struct Cyc_PP_Doc*_tmp9E5[4];s=((
_tmp9E5[3]=Cyc_PP_text(((_tmp9E6=";",_tag_dyneither(_tmp9E6,sizeof(char),2)))),((
_tmp9E5[2]=Cyc_Absynpp_typedef_name2bolddoc(_tmp4CF),((_tmp9E5[1]=Cyc_PP_text(((
_tmp9E7="enum ",_tag_dyneither(_tmp9E7,sizeof(char),6)))),((_tmp9E5[0]=Cyc_Absynpp_scope2doc(
_tmp4CE),Cyc_PP_cat(_tag_dyneither(_tmp9E5,sizeof(struct Cyc_PP_Doc*),4))))))))));}
else{const char*_tmp9EE;struct Cyc_PP_Doc*_tmp9ED[2];const char*_tmp9EC;struct Cyc_PP_Doc*
_tmp9EB[9];s=((_tmp9EB[8]=Cyc_PP_text(((_tmp9EC=";",_tag_dyneither(_tmp9EC,
sizeof(char),2)))),((_tmp9EB[7]=Cyc_Absynpp_rb(),((_tmp9EB[6]=Cyc_PP_line_doc(),((
_tmp9EB[5]=Cyc_PP_nest(2,((_tmp9ED[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)
_tmp4D0->v),((_tmp9ED[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9ED,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9EB[4]=Cyc_Absynpp_lb(),((_tmp9EB[3]=
Cyc_PP_blank_doc(),((_tmp9EB[2]=Cyc_Absynpp_qvar2bolddoc(_tmp4CF),((_tmp9EB[1]=
Cyc_PP_text(((_tmp9EE="enum ",_tag_dyneither(_tmp9EE,sizeof(char),6)))),((
_tmp9EB[0]=Cyc_Absynpp_scope2doc(_tmp4CE),Cyc_PP_cat(_tag_dyneither(_tmp9EB,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL271;_LL284: {struct Cyc_Absyn_Typedef_d_struct*
_tmp4D1=(struct Cyc_Absyn_Typedef_d_struct*)_tmp4AE;if(_tmp4D1->tag != 9)goto
_LL286;else{_tmp4D2=_tmp4D1->f1;}}_LL285: {void*t;if(_tmp4D2->defn != 0)t=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4D2->defn))->v;else{t=Cyc_Absyn_new_evar(
_tmp4D2->kind,0);}{const char*_tmp9F8;struct Cyc_Core_Opt*_tmp9F7;struct Cyc_PP_Doc*
_tmp9F6[2];const char*_tmp9F5;struct Cyc_PP_Doc*_tmp9F4[4];s=((_tmp9F4[3]=Cyc_PP_text(((
_tmp9F5=";",_tag_dyneither(_tmp9F5,sizeof(char),2)))),((_tmp9F4[2]=Cyc_Absynpp_atts2doc(
_tmp4D2->atts),((_tmp9F4[1]=Cyc_Absynpp_tqtd2doc(_tmp4D2->tq,t,((_tmp9F7=
_cycalloc(sizeof(*_tmp9F7)),((_tmp9F7->v=((_tmp9F6[1]=Cyc_Absynpp_tvars2doc(
_tmp4D2->tvs),((_tmp9F6[0]=Cyc_Absynpp_typedef_name2bolddoc(_tmp4D2->name),Cyc_PP_cat(
_tag_dyneither(_tmp9F6,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9F7))))),((_tmp9F4[
0]=Cyc_PP_text(((_tmp9F8="typedef ",_tag_dyneither(_tmp9F8,sizeof(char),9)))),
Cyc_PP_cat(_tag_dyneither(_tmp9F4,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL271;}_LL286: {struct Cyc_Absyn_Namespace_d_struct*_tmp4D3=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp4AE;if(_tmp4D3->tag != 10)goto _LL288;else{_tmp4D4=_tmp4D3->f1;_tmp4D5=_tmp4D3->f2;}}
_LL287: if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4D4);{
const char*_tmp9FD;const char*_tmp9FC;struct Cyc_PP_Doc*_tmp9FB[8];s=((_tmp9FB[7]=
Cyc_Absynpp_rb(),((_tmp9FB[6]=Cyc_PP_line_doc(),((_tmp9FB[5]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmp9FC="",_tag_dyneither(_tmp9FC,
sizeof(char),1))),_tmp4D5),((_tmp9FB[4]=Cyc_PP_line_doc(),((_tmp9FB[3]=Cyc_Absynpp_lb(),((
_tmp9FB[2]=Cyc_PP_blank_doc(),((_tmp9FB[1]=Cyc_PP_textptr(_tmp4D4),((_tmp9FB[0]=
Cyc_PP_text(((_tmp9FD="namespace ",_tag_dyneither(_tmp9FD,sizeof(char),11)))),
Cyc_PP_cat(_tag_dyneither(_tmp9FB,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();goto _LL271;
_LL288: {struct Cyc_Absyn_Using_d_struct*_tmp4D6=(struct Cyc_Absyn_Using_d_struct*)
_tmp4AE;if(_tmp4D6->tag != 11)goto _LL28A;else{_tmp4D7=_tmp4D6->f1;_tmp4D8=_tmp4D6->f2;}}
_LL289: if(Cyc_Absynpp_print_using_stmts){const char*_tmpA02;const char*_tmpA01;
struct Cyc_PP_Doc*_tmpA00[8];s=((_tmpA00[7]=Cyc_Absynpp_rb(),((_tmpA00[6]=Cyc_PP_line_doc(),((
_tmpA00[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA01="",_tag_dyneither(_tmpA01,sizeof(char),1))),_tmp4D8),((_tmpA00[4]=Cyc_PP_line_doc(),((
_tmpA00[3]=Cyc_Absynpp_lb(),((_tmpA00[2]=Cyc_PP_blank_doc(),((_tmpA00[1]=Cyc_Absynpp_qvar2doc(
_tmp4D7),((_tmpA00[0]=Cyc_PP_text(((_tmpA02="using ",_tag_dyneither(_tmpA02,
sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA00,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
else{const char*_tmpA0D;const char*_tmpA0C;const char*_tmpA0B;const char*_tmpA0A;
const char*_tmpA09;struct Cyc_PP_Doc*_tmpA08[11];s=((_tmpA08[10]=Cyc_PP_text(((
_tmpA09=" */",_tag_dyneither(_tmpA09,sizeof(char),4)))),((_tmpA08[9]=Cyc_Absynpp_rb(),((
_tmpA08[8]=Cyc_PP_text(((_tmpA0A="/* ",_tag_dyneither(_tmpA0A,sizeof(char),4)))),((
_tmpA08[7]=Cyc_PP_line_doc(),((_tmpA08[6]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA0B="",_tag_dyneither(_tmpA0B,sizeof(char),1))),
_tmp4D8),((_tmpA08[5]=Cyc_PP_line_doc(),((_tmpA08[4]=Cyc_PP_text(((_tmpA0C=" */",
_tag_dyneither(_tmpA0C,sizeof(char),4)))),((_tmpA08[3]=Cyc_Absynpp_lb(),((
_tmpA08[2]=Cyc_PP_blank_doc(),((_tmpA08[1]=Cyc_Absynpp_qvar2doc(_tmp4D7),((
_tmpA08[0]=Cyc_PP_text(((_tmpA0D="/* using ",_tag_dyneither(_tmpA0D,sizeof(char),
10)))),Cyc_PP_cat(_tag_dyneither(_tmpA08,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL271;_LL28A: {struct Cyc_Absyn_ExternC_d_struct*_tmp4D9=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp4AE;if(_tmp4D9->tag != 12)goto _LL28C;else{_tmp4DA=_tmp4D9->f1;}}_LL28B: if(Cyc_Absynpp_print_externC_stmts){
const char*_tmpA12;const char*_tmpA11;struct Cyc_PP_Doc*_tmpA10[6];s=((_tmpA10[5]=
Cyc_Absynpp_rb(),((_tmpA10[4]=Cyc_PP_line_doc(),((_tmpA10[3]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA11="",_tag_dyneither(_tmpA11,
sizeof(char),1))),_tmp4DA),((_tmpA10[2]=Cyc_PP_line_doc(),((_tmpA10[1]=Cyc_Absynpp_lb(),((
_tmpA10[0]=Cyc_PP_text(((_tmpA12="extern \"C\" ",_tag_dyneither(_tmpA12,sizeof(
char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA10,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmpA1D;const char*_tmpA1C;const char*_tmpA1B;const char*_tmpA1A;
const char*_tmpA19;struct Cyc_PP_Doc*_tmpA18[9];s=((_tmpA18[8]=Cyc_PP_text(((
_tmpA19=" */",_tag_dyneither(_tmpA19,sizeof(char),4)))),((_tmpA18[7]=Cyc_Absynpp_rb(),((
_tmpA18[6]=Cyc_PP_text(((_tmpA1A="/* ",_tag_dyneither(_tmpA1A,sizeof(char),4)))),((
_tmpA18[5]=Cyc_PP_line_doc(),((_tmpA18[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA1B="",_tag_dyneither(_tmpA1B,sizeof(char),1))),
_tmp4DA),((_tmpA18[3]=Cyc_PP_line_doc(),((_tmpA18[2]=Cyc_PP_text(((_tmpA1C=" */",
_tag_dyneither(_tmpA1C,sizeof(char),4)))),((_tmpA18[1]=Cyc_Absynpp_lb(),((
_tmpA18[0]=Cyc_PP_text(((_tmpA1D="/* extern \"C\" ",_tag_dyneither(_tmpA1D,
sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA18,sizeof(struct Cyc_PP_Doc*),
9))))))))))))))))))));}goto _LL271;_LL28C: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp4DB=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp4AE;if(_tmp4DB->tag != 13)
goto _LL28E;else{_tmp4DC=_tmp4DB->f1;_tmp4DD=_tmp4DB->f2;}}_LL28D: if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;if(_tmp4DD != 0){const char*_tmpA22;const char*_tmpA21;
struct Cyc_PP_Doc*_tmpA20[7];exs_doc=((_tmpA20[6]=Cyc_Absynpp_rb(),((_tmpA20[5]=
Cyc_PP_line_doc(),((_tmpA20[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_export2doc,((_tmpA21=",",_tag_dyneither(_tmpA21,sizeof(char),2))),
_tmp4DD),((_tmpA20[3]=Cyc_PP_line_doc(),((_tmpA20[2]=Cyc_Absynpp_lb(),((_tmpA20[
1]=Cyc_PP_text(((_tmpA22=" export ",_tag_dyneither(_tmpA22,sizeof(char),9)))),((
_tmpA20[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA20,sizeof(struct Cyc_PP_Doc*),
7))))))))))))))));}else{exs_doc=Cyc_Absynpp_rb();}{const char*_tmpA27;const char*
_tmpA26;struct Cyc_PP_Doc*_tmpA25[6];s=((_tmpA25[5]=exs_doc,((_tmpA25[4]=Cyc_PP_line_doc(),((
_tmpA25[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA26="",_tag_dyneither(_tmpA26,sizeof(char),1))),_tmp4DC),((_tmpA25[2]=Cyc_PP_line_doc(),((
_tmpA25[1]=Cyc_Absynpp_lb(),((_tmpA25[0]=Cyc_PP_text(((_tmpA27="extern \"C include\" ",
_tag_dyneither(_tmpA27,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA25,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{const char*_tmpA32;const char*
_tmpA31;const char*_tmpA30;const char*_tmpA2F;const char*_tmpA2E;struct Cyc_PP_Doc*
_tmpA2D[9];s=((_tmpA2D[8]=Cyc_PP_text(((_tmpA2E=" */",_tag_dyneither(_tmpA2E,
sizeof(char),4)))),((_tmpA2D[7]=Cyc_Absynpp_rb(),((_tmpA2D[6]=Cyc_PP_text(((
_tmpA2F="/* ",_tag_dyneither(_tmpA2F,sizeof(char),4)))),((_tmpA2D[5]=Cyc_PP_line_doc(),((
_tmpA2D[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA30="",_tag_dyneither(_tmpA30,sizeof(char),1))),_tmp4DC),((_tmpA2D[3]=Cyc_PP_line_doc(),((
_tmpA2D[2]=Cyc_PP_text(((_tmpA31=" */",_tag_dyneither(_tmpA31,sizeof(char),4)))),((
_tmpA2D[1]=Cyc_Absynpp_lb(),((_tmpA2D[0]=Cyc_PP_text(((_tmpA32="/* extern \"C include\" ",
_tag_dyneither(_tmpA32,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA2D,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL271;_LL28E: {struct Cyc_Absyn_Porton_d_struct*
_tmp4DE=(struct Cyc_Absyn_Porton_d_struct*)_tmp4AE;if(_tmp4DE->tag != 14)goto
_LL290;}_LL28F:{const char*_tmpA35;struct Cyc_PP_Doc*_tmpA34[2];s=((_tmpA34[1]=Cyc_Absynpp_lb(),((
_tmpA34[0]=Cyc_PP_text(((_tmpA35="__cyclone_port_on__;",_tag_dyneither(_tmpA35,
sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA34,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL271;_LL290: {struct Cyc_Absyn_Portoff_d_struct*_tmp4DF=(struct Cyc_Absyn_Portoff_d_struct*)
_tmp4AE;if(_tmp4DF->tag != 15)goto _LL271;}_LL291:{const char*_tmpA38;struct Cyc_PP_Doc*
_tmpA37[2];s=((_tmpA37[1]=Cyc_Absynpp_lb(),((_tmpA37[0]=Cyc_PP_text(((_tmpA38="__cyclone_port_off__;",
_tag_dyneither(_tmpA38,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpA37,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL271;_LL271:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(
enum Cyc_Absyn_Scope sc){if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();
switch(sc){case Cyc_Absyn_Static: _LL29B: {const char*_tmpA39;return Cyc_PP_text(((
_tmpA39="static ",_tag_dyneither(_tmpA39,sizeof(char),8))));}case Cyc_Absyn_Public:
_LL29C: return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL29D: {const char*_tmpA3A;
return Cyc_PP_text(((_tmpA3A="extern ",_tag_dyneither(_tmpA3A,sizeof(char),8))));}
case Cyc_Absyn_ExternC: _LL29E: {const char*_tmpA3B;return Cyc_PP_text(((_tmpA3B="extern \"C\" ",
_tag_dyneither(_tmpA3B,sizeof(char),12))));}case Cyc_Absyn_Abstract: _LL29F: {
const char*_tmpA3C;return Cyc_PP_text(((_tmpA3C="abstract ",_tag_dyneither(_tmpA3C,
sizeof(char),10))));}case Cyc_Absyn_Register: _LL2A0: {const char*_tmpA3D;return Cyc_PP_text(((
_tmpA3D="register ",_tag_dyneither(_tmpA3D,sizeof(char),10))));}}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t);int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){void*_tmp547=t;struct
Cyc_Absyn_Tvar*_tmp549;struct Cyc_List_List*_tmp54B;_LL2A3: {struct Cyc_Absyn_VarType_struct*
_tmp548=(struct Cyc_Absyn_VarType_struct*)_tmp547;if(_tmp548->tag != 2)goto _LL2A5;
else{_tmp549=_tmp548->f1;}}_LL2A4: return Cyc_Tcutil_is_temp_tvar(_tmp549);_LL2A5: {
struct Cyc_Absyn_JoinEff_struct*_tmp54A=(struct Cyc_Absyn_JoinEff_struct*)_tmp547;
if(_tmp54A->tag != 24)goto _LL2A7;else{_tmp54B=_tmp54A->f1;}}_LL2A6: return((int(*)(
int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,
_tmp54B);_LL2A7:;_LL2A8: return 0;_LL2A2:;}int Cyc_Absynpp_is_anon_aggrtype(void*t);
int Cyc_Absynpp_is_anon_aggrtype(void*t){void*_tmp54C=t;void**_tmp550;void*
_tmp551;_LL2AA: {struct Cyc_Absyn_AnonAggrType_struct*_tmp54D=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp54C;if(_tmp54D->tag != 13)goto _LL2AC;}_LL2AB: return 1;_LL2AC: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp54E=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp54C;if(_tmp54E->tag != 15)goto
_LL2AE;}_LL2AD: return 1;_LL2AE: {struct Cyc_Absyn_TypedefType_struct*_tmp54F=(
struct Cyc_Absyn_TypedefType_struct*)_tmp54C;if(_tmp54F->tag != 18)goto _LL2B0;
else{_tmp550=_tmp54F->f4;if(_tmp550 == 0)goto _LL2B0;_tmp551=*_tmp550;}}_LL2AF:
return Cyc_Absynpp_is_anon_aggrtype(_tmp551);_LL2B0:;_LL2B1: return 0;_LL2A9:;}
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,
void*atts,struct Cyc_List_List*tms);static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(
struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){if(tms != 0  && tms->tl != 
0){struct _tuple16 _tmpA3E;struct _tuple16 _tmp553=(_tmpA3E.f1=(void*)tms->hd,((
_tmpA3E.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpA3E)));
void*_tmp554;void*_tmp556;_LL2B3: _tmp554=_tmp553.f1;{struct Cyc_Absyn_Pointer_mod_struct*
_tmp555=(struct Cyc_Absyn_Pointer_mod_struct*)_tmp554;if(_tmp555->tag != 2)goto
_LL2B5;};_tmp556=_tmp553.f2;{struct Cyc_Absyn_Function_mod_struct*_tmp557=(struct
Cyc_Absyn_Function_mod_struct*)_tmp556;if(_tmp557->tag != 3)goto _LL2B5;};_LL2B4: {
struct Cyc_List_List*_tmpA41;struct Cyc_List_List*_tmpA40;return(_tmpA40=
_region_malloc(r,sizeof(*_tmpA40)),((_tmpA40->hd=(void*)tms->hd,((_tmpA40->tl=((
_tmpA41=_region_malloc(r,sizeof(*_tmpA41)),((_tmpA41->hd=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd,((_tmpA41->tl=Cyc_Absynpp_bubble_attributes(r,atts,((
struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpA41)))))),_tmpA40)))));}
_LL2B5:;_LL2B6: {struct Cyc_List_List*_tmpA42;return(_tmpA42=_region_malloc(r,
sizeof(*_tmpA42)),((_tmpA42->hd=atts,((_tmpA42->tl=tms,_tmpA42)))));}_LL2B2:;}
else{struct Cyc_List_List*_tmpA43;return(_tmpA43=_region_malloc(r,sizeof(*_tmpA43)),((
_tmpA43->hd=atts,((_tmpA43->tl=tms,_tmpA43)))));}}struct _tuple12 Cyc_Absynpp_to_tms(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple12 Cyc_Absynpp_to_tms(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){void*_tmp55C=t;struct Cyc_Absyn_ArrayInfo
_tmp55E;void*_tmp55F;struct Cyc_Absyn_Tqual _tmp560;struct Cyc_Absyn_Exp*_tmp561;
union Cyc_Absyn_Constraint*_tmp562;struct Cyc_Position_Segment*_tmp563;struct Cyc_Absyn_PtrInfo
_tmp565;void*_tmp566;struct Cyc_Absyn_Tqual _tmp567;struct Cyc_Absyn_PtrAtts _tmp568;
struct Cyc_Absyn_FnInfo _tmp56A;struct Cyc_List_List*_tmp56B;struct Cyc_Core_Opt*
_tmp56C;void*_tmp56D;struct Cyc_List_List*_tmp56E;int _tmp56F;struct Cyc_Absyn_VarargInfo*
_tmp570;struct Cyc_List_List*_tmp571;struct Cyc_List_List*_tmp572;struct Cyc_Core_Opt*
_tmp574;struct Cyc_Core_Opt*_tmp575;int _tmp576;struct _tuple0*_tmp578;struct Cyc_List_List*
_tmp579;void**_tmp57A;_LL2B8: {struct Cyc_Absyn_ArrayType_struct*_tmp55D=(struct
Cyc_Absyn_ArrayType_struct*)_tmp55C;if(_tmp55D->tag != 9)goto _LL2BA;else{_tmp55E=
_tmp55D->f1;_tmp55F=_tmp55E.elt_type;_tmp560=_tmp55E.tq;_tmp561=_tmp55E.num_elts;
_tmp562=_tmp55E.zero_term;_tmp563=_tmp55E.zt_loc;}}_LL2B9: {struct Cyc_Absyn_Tqual
_tmp57C;void*_tmp57D;struct Cyc_List_List*_tmp57E;struct _tuple12 _tmp57B=Cyc_Absynpp_to_tms(
r,_tmp560,_tmp55F);_tmp57C=_tmp57B.f1;_tmp57D=_tmp57B.f2;_tmp57E=_tmp57B.f3;{
void*tm;if(_tmp561 == 0){struct Cyc_Absyn_Carray_mod_struct _tmpA46;struct Cyc_Absyn_Carray_mod_struct*
_tmpA45;tm=(void*)((_tmpA45=_region_malloc(r,sizeof(*_tmpA45)),((_tmpA45[0]=((
_tmpA46.tag=0,((_tmpA46.f1=_tmp562,((_tmpA46.f2=_tmp563,_tmpA46)))))),_tmpA45))));}
else{struct Cyc_Absyn_ConstArray_mod_struct _tmpA49;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpA48;tm=(void*)((_tmpA48=_region_malloc(r,sizeof(*_tmpA48)),((_tmpA48[0]=((
_tmpA49.tag=1,((_tmpA49.f1=(struct Cyc_Absyn_Exp*)_tmp561,((_tmpA49.f2=_tmp562,((
_tmpA49.f3=_tmp563,_tmpA49)))))))),_tmpA48))));}{struct Cyc_List_List*_tmpA4C;
struct _tuple12 _tmpA4B;return(_tmpA4B.f1=_tmp57C,((_tmpA4B.f2=_tmp57D,((_tmpA4B.f3=((
_tmpA4C=_region_malloc(r,sizeof(*_tmpA4C)),((_tmpA4C->hd=tm,((_tmpA4C->tl=
_tmp57E,_tmpA4C)))))),_tmpA4B)))));};};}_LL2BA: {struct Cyc_Absyn_PointerType_struct*
_tmp564=(struct Cyc_Absyn_PointerType_struct*)_tmp55C;if(_tmp564->tag != 5)goto
_LL2BC;else{_tmp565=_tmp564->f1;_tmp566=_tmp565.elt_typ;_tmp567=_tmp565.elt_tq;
_tmp568=_tmp565.ptr_atts;}}_LL2BB: {struct Cyc_Absyn_Tqual _tmp586;void*_tmp587;
struct Cyc_List_List*_tmp588;struct _tuple12 _tmp585=Cyc_Absynpp_to_tms(r,_tmp567,
_tmp566);_tmp586=_tmp585.f1;_tmp587=_tmp585.f2;_tmp588=_tmp585.f3;{struct Cyc_Absyn_Pointer_mod_struct*
_tmpA52;struct Cyc_Absyn_Pointer_mod_struct _tmpA51;struct Cyc_List_List*_tmpA50;
_tmp588=((_tmpA50=_region_malloc(r,sizeof(*_tmpA50)),((_tmpA50->hd=(void*)((
_tmpA52=_region_malloc(r,sizeof(*_tmpA52)),((_tmpA52[0]=((_tmpA51.tag=2,((
_tmpA51.f1=_tmp568,((_tmpA51.f2=tq,_tmpA51)))))),_tmpA52)))),((_tmpA50->tl=
_tmp588,_tmpA50))))));}{struct _tuple12 _tmpA53;return(_tmpA53.f1=_tmp586,((
_tmpA53.f2=_tmp587,((_tmpA53.f3=_tmp588,_tmpA53)))));};}_LL2BC: {struct Cyc_Absyn_FnType_struct*
_tmp569=(struct Cyc_Absyn_FnType_struct*)_tmp55C;if(_tmp569->tag != 10)goto _LL2BE;
else{_tmp56A=_tmp569->f1;_tmp56B=_tmp56A.tvars;_tmp56C=_tmp56A.effect;_tmp56D=
_tmp56A.ret_typ;_tmp56E=_tmp56A.args;_tmp56F=_tmp56A.c_varargs;_tmp570=_tmp56A.cyc_varargs;
_tmp571=_tmp56A.rgn_po;_tmp572=_tmp56A.attributes;}}_LL2BD: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp56C == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp56C->v)){
_tmp56C=0;_tmp56B=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp56B);}{struct Cyc_Absyn_Tqual _tmp58E;void*_tmp58F;struct Cyc_List_List*_tmp590;
struct _tuple12 _tmp58D=Cyc_Absynpp_to_tms(r,Cyc_Absyn_empty_tqual(0),_tmp56D);
_tmp58E=_tmp58D.f1;_tmp58F=_tmp58D.f2;_tmp590=_tmp58D.f3;{struct Cyc_List_List*
tms=_tmp590;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2C4: if(
_tmp572 != 0){struct Cyc_Absyn_Attributes_mod_struct _tmpA56;struct Cyc_Absyn_Attributes_mod_struct*
_tmpA55;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpA55=_region_malloc(r,
sizeof(*_tmpA55)),((_tmpA55[0]=((_tmpA56.tag=5,((_tmpA56.f1=0,((_tmpA56.f2=
_tmp572,_tmpA56)))))),_tmpA55)))),tms);}{struct Cyc_Absyn_Function_mod_struct*
_tmpA65;struct Cyc_Absyn_WithTypes_struct*_tmpA64;struct Cyc_Absyn_WithTypes_struct
_tmpA63;struct Cyc_Absyn_Function_mod_struct _tmpA62;struct Cyc_List_List*_tmpA61;
tms=((_tmpA61=_region_malloc(r,sizeof(*_tmpA61)),((_tmpA61->hd=(void*)((_tmpA65=
_region_malloc(r,sizeof(*_tmpA65)),((_tmpA65[0]=((_tmpA62.tag=3,((_tmpA62.f1=(
void*)((void*)((_tmpA64=_region_malloc(r,sizeof(*_tmpA64)),((_tmpA64[0]=((
_tmpA63.tag=1,((_tmpA63.f1=_tmp56E,((_tmpA63.f2=_tmp56F,((_tmpA63.f3=_tmp570,((
_tmpA63.f4=_tmp56C,((_tmpA63.f5=_tmp571,_tmpA63)))))))))))),_tmpA64))))),_tmpA62)))),
_tmpA65)))),((_tmpA61->tl=tms,_tmpA61))))));}break;case Cyc_Cyclone_Vc_c: _LL2C5:{
struct Cyc_Absyn_Function_mod_struct*_tmpA74;struct Cyc_Absyn_WithTypes_struct*
_tmpA73;struct Cyc_Absyn_WithTypes_struct _tmpA72;struct Cyc_Absyn_Function_mod_struct
_tmpA71;struct Cyc_List_List*_tmpA70;tms=((_tmpA70=_region_malloc(r,sizeof(*
_tmpA70)),((_tmpA70->hd=(void*)((_tmpA74=_region_malloc(r,sizeof(*_tmpA74)),((
_tmpA74[0]=((_tmpA71.tag=3,((_tmpA71.f1=(void*)((void*)((_tmpA73=_region_malloc(
r,sizeof(*_tmpA73)),((_tmpA73[0]=((_tmpA72.tag=1,((_tmpA72.f1=_tmp56E,((_tmpA72.f2=
_tmp56F,((_tmpA72.f3=_tmp570,((_tmpA72.f4=_tmp56C,((_tmpA72.f5=_tmp571,_tmpA72)))))))))))),
_tmpA73))))),_tmpA71)))),_tmpA74)))),((_tmpA70->tl=tms,_tmpA70))))));}for(0;
_tmp572 != 0;_tmp572=_tmp572->tl){void*_tmp59D=(void*)_tmp572->hd;_LL2C8: {struct
Cyc_Absyn_Stdcall_att_struct*_tmp59E=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp59D;if(_tmp59E->tag != 1)goto _LL2CA;}_LL2C9: goto _LL2CB;_LL2CA: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp59F=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp59D;if(_tmp59F->tag != 2)goto
_LL2CC;}_LL2CB: goto _LL2CD;_LL2CC: {struct Cyc_Absyn_Fastcall_att_struct*_tmp5A0=(
struct Cyc_Absyn_Fastcall_att_struct*)_tmp59D;if(_tmp5A0->tag != 3)goto _LL2CE;}
_LL2CD:{struct Cyc_Absyn_Attributes_mod_struct*_tmpA7E;struct Cyc_List_List*
_tmpA7D;struct Cyc_Absyn_Attributes_mod_struct _tmpA7C;struct Cyc_List_List*_tmpA7B;
tms=((_tmpA7B=_region_malloc(r,sizeof(*_tmpA7B)),((_tmpA7B->hd=(void*)((_tmpA7E=
_region_malloc(r,sizeof(*_tmpA7E)),((_tmpA7E[0]=((_tmpA7C.tag=5,((_tmpA7C.f1=0,((
_tmpA7C.f2=((_tmpA7D=_cycalloc(sizeof(*_tmpA7D)),((_tmpA7D->hd=(void*)_tmp572->hd,((
_tmpA7D->tl=0,_tmpA7D)))))),_tmpA7C)))))),_tmpA7E)))),((_tmpA7B->tl=tms,_tmpA7B))))));}
goto AfterAtts;_LL2CE:;_LL2CF: goto _LL2C7;_LL2C7:;}break;}AfterAtts: if(_tmp56B != 0){
struct Cyc_Absyn_TypeParams_mod_struct*_tmpA84;struct Cyc_Absyn_TypeParams_mod_struct
_tmpA83;struct Cyc_List_List*_tmpA82;tms=((_tmpA82=_region_malloc(r,sizeof(*
_tmpA82)),((_tmpA82->hd=(void*)((_tmpA84=_region_malloc(r,sizeof(*_tmpA84)),((
_tmpA84[0]=((_tmpA83.tag=4,((_tmpA83.f1=_tmp56B,((_tmpA83.f2=0,((_tmpA83.f3=1,
_tmpA83)))))))),_tmpA84)))),((_tmpA82->tl=tms,_tmpA82))))));}{struct _tuple12
_tmpA85;return(_tmpA85.f1=_tmp58E,((_tmpA85.f2=_tmp58F,((_tmpA85.f3=tms,_tmpA85)))));};};};
_LL2BE: {struct Cyc_Absyn_Evar_struct*_tmp573=(struct Cyc_Absyn_Evar_struct*)
_tmp55C;if(_tmp573->tag != 1)goto _LL2C0;else{_tmp574=_tmp573->f1;_tmp575=_tmp573->f2;
_tmp576=_tmp573->f3;}}_LL2BF: if(_tmp575 == 0){struct _tuple12 _tmpA86;return(
_tmpA86.f1=tq,((_tmpA86.f2=t,((_tmpA86.f3=0,_tmpA86)))));}else{return Cyc_Absynpp_to_tms(
r,tq,(void*)_tmp575->v);}_LL2C0: {struct Cyc_Absyn_TypedefType_struct*_tmp577=(
struct Cyc_Absyn_TypedefType_struct*)_tmp55C;if(_tmp577->tag != 18)goto _LL2C2;
else{_tmp578=_tmp577->f1;_tmp579=_tmp577->f2;_tmp57A=_tmp577->f4;}}_LL2C1: if((
_tmp57A == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(*
_tmp57A)){struct _tuple12 _tmpA87;return(_tmpA87.f1=tq,((_tmpA87.f2=t,((_tmpA87.f3=
0,_tmpA87)))));}else{return Cyc_Absynpp_to_tms(r,tq,*_tmp57A);}_LL2C2:;_LL2C3: {
struct _tuple12 _tmpA88;return(_tmpA88.f1=tq,((_tmpA88.f2=t,((_tmpA88.f3=0,_tmpA88)))));}
_LL2B7:;}static int Cyc_Absynpp_is_char_ptr(void*t);static int Cyc_Absynpp_is_char_ptr(
void*t){void*_tmp5AC=t;struct Cyc_Core_Opt*_tmp5AE;struct Cyc_Core_Opt _tmp5AF;void*
_tmp5B0;struct Cyc_Absyn_PtrInfo _tmp5B2;void*_tmp5B3;_LL2D1: {struct Cyc_Absyn_Evar_struct*
_tmp5AD=(struct Cyc_Absyn_Evar_struct*)_tmp5AC;if(_tmp5AD->tag != 1)goto _LL2D3;
else{_tmp5AE=_tmp5AD->f2;if(_tmp5AE == 0)goto _LL2D3;_tmp5AF=*_tmp5AE;_tmp5B0=(
void*)_tmp5AF.v;}}_LL2D2: return Cyc_Absynpp_is_char_ptr(_tmp5B0);_LL2D3: {struct
Cyc_Absyn_PointerType_struct*_tmp5B1=(struct Cyc_Absyn_PointerType_struct*)
_tmp5AC;if(_tmp5B1->tag != 5)goto _LL2D5;else{_tmp5B2=_tmp5B1->f1;_tmp5B3=_tmp5B2.elt_typ;}}
_LL2D4: L: {void*_tmp5B4=_tmp5B3;struct Cyc_Core_Opt*_tmp5B6;struct Cyc_Core_Opt
_tmp5B7;void*_tmp5B8;void**_tmp5BA;void*_tmp5BB;enum Cyc_Absyn_Size_of _tmp5BD;
_LL2D8: {struct Cyc_Absyn_Evar_struct*_tmp5B5=(struct Cyc_Absyn_Evar_struct*)
_tmp5B4;if(_tmp5B5->tag != 1)goto _LL2DA;else{_tmp5B6=_tmp5B5->f2;if(_tmp5B6 == 0)
goto _LL2DA;_tmp5B7=*_tmp5B6;_tmp5B8=(void*)_tmp5B7.v;}}_LL2D9: _tmp5B3=_tmp5B8;
goto L;_LL2DA: {struct Cyc_Absyn_TypedefType_struct*_tmp5B9=(struct Cyc_Absyn_TypedefType_struct*)
_tmp5B4;if(_tmp5B9->tag != 18)goto _LL2DC;else{_tmp5BA=_tmp5B9->f4;if(_tmp5BA == 0)
goto _LL2DC;_tmp5BB=*_tmp5BA;}}_LL2DB: _tmp5B3=_tmp5BB;goto L;_LL2DC: {struct Cyc_Absyn_IntType_struct*
_tmp5BC=(struct Cyc_Absyn_IntType_struct*)_tmp5B4;if(_tmp5BC->tag != 6)goto _LL2DE;
else{_tmp5BD=_tmp5BC->f2;if(_tmp5BD != Cyc_Absyn_Char_sz)goto _LL2DE;}}_LL2DD:
return 1;_LL2DE:;_LL2DF: return 0;_LL2D7:;}_LL2D5:;_LL2D6: return 0;_LL2D0:;}struct
Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*
dopt);struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,
struct Cyc_Core_Opt*dopt){struct _RegionHandle _tmp5BE=_new_region("temp");struct
_RegionHandle*temp=& _tmp5BE;_push_region(temp);{struct Cyc_Absyn_Tqual _tmp5C0;
void*_tmp5C1;struct Cyc_List_List*_tmp5C2;struct _tuple12 _tmp5BF=Cyc_Absynpp_to_tms(
temp,tq,typ);_tmp5C0=_tmp5BF.f1;_tmp5C1=_tmp5BF.f2;_tmp5C2=_tmp5BF.f3;_tmp5C2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5C2);if(
_tmp5C2 == 0  && dopt == 0){struct Cyc_PP_Doc*_tmpA89[2];struct Cyc_PP_Doc*_tmp5C4=(
_tmpA89[1]=Cyc_Absynpp_ntyp2doc(_tmp5C1),((_tmpA89[0]=Cyc_Absynpp_tqual2doc(
_tmp5C0),Cyc_PP_cat(_tag_dyneither(_tmpA89,sizeof(struct Cyc_PP_Doc*),2)))));
_npop_handler(0);return _tmp5C4;}else{const char*_tmpA8C;struct Cyc_PP_Doc*_tmpA8B[
4];struct Cyc_PP_Doc*_tmp5C7=(_tmpA8B[3]=Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(
typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp5C2),((_tmpA8B[2]=
Cyc_PP_text(((_tmpA8C=" ",_tag_dyneither(_tmpA8C,sizeof(char),2)))),((_tmpA8B[1]=
Cyc_Absynpp_ntyp2doc(_tmp5C1),((_tmpA8B[0]=Cyc_Absynpp_tqual2doc(_tmp5C0),Cyc_PP_cat(
_tag_dyneither(_tmpA8B,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);
return _tmp5C7;}};_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(
struct Cyc_Absyn_Aggrfield*f);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct
Cyc_Absyn_Aggrfield*f){switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c:
_LL2E0: if(f->width != 0){struct Cyc_Core_Opt*_tmpA93;const char*_tmpA92;const char*
_tmpA91;struct Cyc_PP_Doc*_tmpA90[5];return(_tmpA90[4]=Cyc_PP_text(((_tmpA91=";",
_tag_dyneither(_tmpA91,sizeof(char),2)))),((_tmpA90[3]=Cyc_Absynpp_atts2doc(f->attributes),((
_tmpA90[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((
_tmpA90[1]=Cyc_PP_text(((_tmpA92=":",_tag_dyneither(_tmpA92,sizeof(char),2)))),((
_tmpA90[0]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpA93=_cycalloc(sizeof(*
_tmpA93)),((_tmpA93->v=Cyc_PP_textptr(f->name),_tmpA93))))),Cyc_PP_cat(
_tag_dyneither(_tmpA90,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{struct Cyc_Core_Opt*
_tmpA98;const char*_tmpA97;struct Cyc_PP_Doc*_tmpA96[3];return(_tmpA96[2]=Cyc_PP_text(((
_tmpA97=";",_tag_dyneither(_tmpA97,sizeof(char),2)))),((_tmpA96[1]=Cyc_Absynpp_atts2doc(
f->attributes),((_tmpA96[0]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpA98=
_cycalloc(sizeof(*_tmpA98)),((_tmpA98->v=Cyc_PP_textptr(f->name),_tmpA98))))),
Cyc_PP_cat(_tag_dyneither(_tmpA96,sizeof(struct Cyc_PP_Doc*),3)))))));}case Cyc_Cyclone_Vc_c:
_LL2E1: if(f->width != 0){struct Cyc_Core_Opt*_tmpA9F;const char*_tmpA9E;const char*
_tmpA9D;struct Cyc_PP_Doc*_tmpA9C[5];return(_tmpA9C[4]=Cyc_PP_text(((_tmpA9D=";",
_tag_dyneither(_tmpA9D,sizeof(char),2)))),((_tmpA9C[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpA9C[2]=Cyc_PP_text(((_tmpA9E=":",
_tag_dyneither(_tmpA9E,sizeof(char),2)))),((_tmpA9C[1]=Cyc_Absynpp_tqtd2doc(f->tq,
f->type,((_tmpA9F=_cycalloc(sizeof(*_tmpA9F)),((_tmpA9F->v=Cyc_PP_textptr(f->name),
_tmpA9F))))),((_tmpA9C[0]=Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(
_tag_dyneither(_tmpA9C,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{struct Cyc_Core_Opt*
_tmpAA4;const char*_tmpAA3;struct Cyc_PP_Doc*_tmpAA2[3];return(_tmpAA2[2]=Cyc_PP_text(((
_tmpAA3=";",_tag_dyneither(_tmpAA3,sizeof(char),2)))),((_tmpAA2[1]=Cyc_Absynpp_tqtd2doc(
f->tq,f->type,((_tmpAA4=_cycalloc(sizeof(*_tmpAA4)),((_tmpAA4->v=Cyc_PP_textptr(
f->name),_tmpAA4))))),((_tmpAA2[0]=Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(
_tag_dyneither(_tmpAA2,sizeof(struct Cyc_PP_Doc*),3)))))));}}}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(
struct Cyc_List_List*fields){const char*_tmpAA5;return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmpAA5="",
_tag_dyneither(_tmpAA5,sizeof(char),1))),fields);}struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(
struct Cyc_Absyn_Datatypefield*f);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(
struct Cyc_Absyn_Datatypefield*f){struct Cyc_PP_Doc*_tmpAA6[3];return(_tmpAA6[2]=f->typs
== 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((_tmpAA6[1]=Cyc_Absynpp_typedef_name2doc(
f->name),((_tmpAA6[0]=Cyc_Absynpp_scope2doc(f->sc),Cyc_PP_cat(_tag_dyneither(
_tmpAA6,sizeof(struct Cyc_PP_Doc*),3)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields){const char*_tmpAA7;return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((_tmpAA7=",",
_tag_dyneither(_tmpAA7,sizeof(char),2))),fields);}void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{const char*_tmpAAA;
void*_tmpAA9;(_tmpAA9=0,Cyc_fprintf(f,((_tmpAAA="\n",_tag_dyneither(_tmpAAA,
sizeof(char),2))),_tag_dyneither(_tmpAA9,sizeof(void*),0)));};}}struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){const char*
_tmpAAB;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpAAB="",_tag_dyneither(
_tmpAAB,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),
72);}struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(
Cyc_Absynpp_exp2doc(e),72);}struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct
Cyc_Absyn_Stmt*s);struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*
s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0),72);}struct
_dyneither_ptr Cyc_Absynpp_typ2string(void*t);struct _dyneither_ptr Cyc_Absynpp_typ2string(
void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}struct
_dyneither_ptr Cyc_Absynpp_typ2cstring(void*t);struct _dyneither_ptr Cyc_Absynpp_typ2cstring(
void*t){int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;Cyc_Absynpp_add_cyc_prefix=0;{struct _dyneither_ptr s=
Cyc_Absynpp_typ2string(t);Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
old_add_cyc_prefix;return s;};}struct _dyneither_ptr Cyc_Absynpp_prim2string(enum 
Cyc_Absyn_Primop p);struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop
p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}struct _dyneither_ptr
Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p);struct _dyneither_ptr Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc);struct
_dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(
Cyc_Absynpp_scope2doc(sc),72);}
